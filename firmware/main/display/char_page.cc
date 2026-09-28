/*
 * char_page.cc — KidPal 查字窗 `char-window` (LVGL 9.3, 设备侧): 米字格大字盘 (左) +
 * 右栏速查卡 (拼音带声调 / 共 N 画 / 部首 / 释义) + 底部提示条 + 逐笔动画驱动
 * (沿 medians 笔尖移动 + 当前笔白衬边)。**会话内覆盖窗**（M2-K3 页→窗改造）: 在当前页
 * 之上叠一层窗面板、**不清屏**——关窗（dismiss_front_overlay 清 transient 回基层）后
 * 原页原样回来、不翻页。
 *
 * 教学设计红线 (spec §5.6 v3 / S-F3.1, CHR-A7): 米字格示范字【只走 strokes 数据
 * 光栅渲染路径】(kidpal_char_raster_stroke, 字库数据即标准规范字形,
 * 绝不缺省回落字体渲染), 伴随文字 (拼音/部首/释义) 才用 LVGL 字体。示范字路径
 * 上零 tilt — 示范字路径上绝不施加文字倾斜样式 (防「教歪」)。
 *
 * 动画状态机 (950ms/笔、三态已写/当前/未写虚线点链 6px) 全部委托 display/char_stroke.c
 * 的纯逻辑合约 (kidpal_char_anim_state 等), 本窗只做 LVGL 呈现。
 *
 * v1.59 KEY 起播口径 (本包 260928-char-window-key-triggered-play): 建窗即米字格静态
 * 显示完整示范字 (整字实心黑——charpage_render_frame(-1, n_strokes) 判每笔 WRITTEN;
 * 未按 KEY 前零逐笔演示——timer 建表即 lv_timer_pause); KEY 短按 = 首次起播 / 从头重播
 * (replay 归零 elapsed + 置 started + lv_timer_resume, 首播重播同路径); 演完定格
 * 「写完啦·共 N 笔」等按键、不自动循环 (timer_cb REST 相位渲染定格帧后 lv_timer_pause +
 * return, 取模回绕在装配层闸断——char_stroke.c 纯逻辑合约零触碰)。键帽随态: 未起播
 * 「播报」/ 起播后「重播」(show 末尾与 replay 均刷 kidpal_lcd_keybar_sync)。
 *
 * 底部提示条文案消费 kidpal_char_card_build 的 out->progress 单源 (演示中「第 X 笔·
 * 共 N 笔」/ 定格「写完啦·共 N 笔」, S-F3.3 AC2——本窗零重抄文案格式)。数据结构由
 * mcp_server 经 kidpal_char_data_validate 防御降级门校验后喂入; 本函数仍防御 (缺字段/
 * 解析失败 -> 不盲渲染直接返回)。
 *
 * v1.38 对账清理 (本包落地): 右栏已砍项死成员清零 + 头注虚标清除; 握笔小手 12px 圆环
 * 占位 (占位绘制函数与尺寸常量整体) 删除——改口径后小手资产不再需要, 当前笔改白衬边。
 * 锚 = `char-window` (原 canvas-p4, v1.38 对账改名收尾)。
 *
 * v5 收敛 (本包 260929-char-window-v5-trim, human 2026-09-29 真机轮裁定): ①去右栏底部
 * 装饰星 (星资产消费整删, 资产转孤儿账面注记); ②去组词行——运行时 words 恒空、口径
 * 收敛 S-F3.2 四行 (拼音/共 N 画/部首/释义; card.words 成员保留, 本窗 info 传空串);
 * ③米字格引导线原灰度点在设备 RLCD 阈值化后不可见 -> 改纯黑点线; ④右栏 cardlabel
 * 让位清出重烘圆角框右缘 (x 228 -> 244)。外框 frm_236_236.bin 重烘为正圆角矩形 (资产轮)。
 */

#include "char_page.h"

#include "char_stroke.h"
#include "art_assets.h"

#include <cJSON.h>
#include <lvgl.h>
#include <esp_log.h>
#include <esp_lvgl_port.h> /* replay 持 LVGL 锁 (lvgl_port_lock/unlock): KEY 短按经板级
                              app.Schedule 在主任务跑、当前无锁; 锁内改动画态 + 启表。 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cstdio>

/* 键帽条三槽刷新 C 桥 (lcd_display.cc 定义; 内部递归取锁安全——kidpal_render_node.cc
 * :141 同款先例)。show 末尾 (建窗即「播报」) 与 replay (起播后「重播」) 的键帽驱动点。 */
extern "C" void kidpal_lcd_keybar_sync(void);

LV_FONT_DECLARE(font_puhui_14_1); /* 全量 14px 普惠体: 右栏速查卡 + 底部提示条显式接线
                                     (v1.56 对账④ ——LV_FONT_DEFAULT 内置 montserrat_14 纯拉丁,
                                     无汉字与带调韵母字形, 中文 label 绝不能钉默认字体; 与
                                     lcd_display.cc 键帽条/语音轮五处真机验证同款先例) */

#define CHARPAGE_TAG "char_page"
#define CHARPAGE_CANVAS_SIZE 208    /* 米字格大字盘边长 (设计稿帧约 236/字盘约 208) */
#define CHARPAGE_GRID_CELL 52       /* 米字格四宫格单元 (208/4) */
#define CHARPAGE_TIMER_PERIOD_MS 100
/* 单笔预解析点集容量上限（对齐 char_stroke 消费面 256 点口径; 帧路径零 ~6.5KB 级
 * 栈局部数组——M2-K1 缺陷②修复: pts/sc/toks 去栈, 预解析落堆承载） */
#define CHARPAGE_MAX_PTS 256

/* 单窗状态 (设备同一时刻只有查字窗一扇, 静态单例足够且 additive 不动底座) */
struct CharPageState {
    lv_obj_t *panel;           /* 查字窗面板：活动屏（s_kidpal_scr）之上叠的一层，不自建屏 */
    lv_obj_t *canvas;
    lv_obj_t *char_label;        /* 大字 */
    lv_obj_t *pinyin_label;
    lv_obj_t *stroke_total_label;
    lv_obj_t *words_label;
    lv_obj_t *hint_label;
    lv_timer_t *timer;
    lv_color_t *canvas_buf;
    uint8_t *rast;
    int n_strokes;
    char **stroke_paths;         /* 每笔 SVG path (cJSON 生命周期内取出 strdup) */
    char **median_jsons;         /* 每笔 medians 块 JSON */
    int *med_pts;                /* 每笔 medians 点数 (供笔尖移动) */
    kidpal_cs_point *pre_pts;    /* 每笔预解析点集堆缓冲 (n_strokes × CHARPAGE_MAX_PTS,
                                    M2-K1 缺陷②去栈机制面——帧路径零解析调用零大栈数组) */
    int *pre_counts;             /* 每笔预解析点数 (<=0 = 该笔不可用; 帧路径复用) */
    /* 速查卡单源 (S-F3.2 AC1 / S-F3.3 AC2)：kidpal_char_card_build 装配结果——pinyin/
     * radical/stroke_total/meaning 供 label 拼装 (v5 收敛: 组词行删), progress 供底部
     * 提示条（演示中进度 vs 定格文案）。char 数组副本, cJSON 释放后安全。 */
    kidpal_char_card card;
    int elapsed_ms;
    int started;   /* v1.59 演示起播态 (WF-F3-01 键帽随态变): 0 = 未起播 (键帽「播报」)
                    * / 非 0 = 已起播 (键帽「重播」)。建窗 memset 归 0, replay 置 1。 */
};

static struct CharPageState g_page;

/* ------------------------------------------------------------------------- *
 * cJSON data 解析: 取必需字段 + 逐笔 strokes/medians 字符串。成功返回 1。
 * ------------------------------------------------------------------------ */
static int charpage_parse_data(const char *data_json)
{
    cJSON *root = NULL;
    cJSON *strokes = NULL;
    cJSON *medians = NULL;
    int n = 0;

    if (data_json == NULL || data_json[0] == '\0') {
        return 0;
    }
    root = cJSON_Parse(data_json);
    if (root == NULL) {
        return 0;
    }
    strokes = cJSON_GetObjectItem(root, "strokes");
    medians = cJSON_GetObjectItem(root, "medians");
    if (strokes == NULL || !cJSON_IsArray(strokes)) {
        cJSON_Delete(root);
        return 0;
    }
    n = cJSON_GetArraySize(strokes);
    if (n <= 0) {
        cJSON_Delete(root);
        return 0;
    }
    g_page.n_strokes = n;
    g_page.stroke_paths = (char **)calloc((size_t)n, sizeof(char *));
    g_page.median_jsons = (char **)calloc((size_t)n, sizeof(char *));
    g_page.med_pts = (int *)calloc((size_t)n, sizeof(int));
    if (g_page.stroke_paths == NULL || g_page.median_jsons == NULL ||
        g_page.med_pts == NULL) {
        cJSON_Delete(root);
        return 0;
    }
    for (int i = 0; i < n; i++) {
        cJSON *path = cJSON_GetArrayItem(strokes, i);
        if (path != NULL && cJSON_IsString(path)) {
            g_page.stroke_paths[i] = strdup(path->valuestring);
        }
        if (medians != NULL && i < cJSON_GetArraySize(medians)) {
            cJSON *one = cJSON_GetArrayItem(medians, i);
            char *one_json = one ? cJSON_PrintUnformatted(one) : NULL;
            if (one_json != NULL) {
                g_page.median_jsons[i] = strdup(one_json);
                cJSON_free(one_json);
            }
        }
    }
    /* 预解析去栈（M2-K1 缺陷②修复机制面）：把每笔 stroke path 一次性离散成点集落
     * 堆（帧路径逐笔逐帧重复解析 + pts[256] 局部大数组 → 移除）。parse 内部 toks[256]
     * ~2KB 栈随调用点移出帧路径而离场。 */
    g_page.pre_pts = (kidpal_cs_point *)calloc(
        (size_t)n * CHARPAGE_MAX_PTS, sizeof(kidpal_cs_point));
    g_page.pre_counts = (int *)calloc((size_t)n, sizeof(int));
    if (g_page.pre_pts == NULL || g_page.pre_counts == NULL) {
        cJSON_Delete(root);
        return 0;
    }
    for (int i = 0; i < n; i++) {
        if (g_page.stroke_paths[i] != NULL) {
            int pc = kidpal_char_stroke_parse(
                g_page.stroke_paths[i],
                g_page.pre_pts + (size_t)i * CHARPAGE_MAX_PTS, CHARPAGE_MAX_PTS);
            g_page.pre_counts[i] = (pc > 0) ? pc : 0;
        }
    }
    cJSON_Delete(root);
    return 1;
}

static void charpage_free_state(void)
{
    if (g_page.stroke_paths != NULL) {
        for (int i = 0; i < g_page.n_strokes; i++) {
            free(g_page.stroke_paths[i]);
            free(g_page.median_jsons ? g_page.median_jsons[i] : NULL);
        }
        free(g_page.stroke_paths);
        free(g_page.median_jsons);
        free(g_page.med_pts);
        g_page.stroke_paths = NULL;
        g_page.median_jsons = NULL;
        g_page.med_pts = NULL;
    }
    free(g_page.pre_pts);
    free(g_page.pre_counts);
    g_page.pre_pts = NULL;
    g_page.pre_counts = NULL;
    free(g_page.canvas_buf);
    free(g_page.rast);
    g_page.canvas_buf = NULL;
    g_page.rast = NULL;
}

/* ------------------------------------------------------------------------- *
 * 把源点 (1024 坐标) 缩放到 canvas 坐标。Y 翻转 (字库坐标与屏幕 Y 朝相反)。
 * ------------------------------------------------------------------------ */
static void charpage_map(int size, float sx, float sy, int *dx, int *dy)
{
    *dx = (int)(sx * (float)(size - 1) / 1024.0f);
    *dy = (int)((1024.0f - sy) * (float)(size - 1) / 1024.0f);
}

/* 把 canvas 1bpp 光栅 blit 到 canvas 像素 (ink=黑) */
static void charpage_blit(int size)
{
    int stride = (size + 7) / 8;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int ink = (g_page.rast[(size_t)y * (size_t)stride + (size_t)(x / 8)] >>
                       (7 - (x % 8))) & 1;
            lv_canvas_set_px(g_page.canvas, x, y,
                             ink ? lv_color_make(0x00, 0x00, 0x00)
                                 : lv_color_make(0xff, 0xff, 0xff),
                             LV_OPA_COVER);
        }
    }
    lv_obj_invalidate(g_page.canvas);
}

/* 1bpp 光栅置 ink 位 (MSB-first, 与 charpage_blit :215 读法同款位序; **只置位不清除**
 * ——与 kidpal_char_stroke 光栅路径 cs_raster_row 同款)。q7 修法 (spine openQ q7,
 * human 2026-09-29 真机目检 finding, human-ratified) 共用落点: 米字格点线
 * (charpage_draw_grid) 与未写笔点链 (charpage_render_frame PENDING) 均写 g_page.rast
 * 1bpp 光栅域, 由 charpage_blit 每帧兜底一次成型上屏——旧「直绘 canvas」的格点/点链
 * 必被 blit 全幅 ink?黑:白 覆写吃掉, 真机不可见 (缺陷机制本体)。 */
static void charpage_rast_ink(uint8_t *rast, int stride, int x, int y)
{
    rast[(size_t)y * (size_t)stride + (size_t)(x / 8)] |=
        (uint8_t)(1u << (7 - (x % 8)));
}

/* 绘制米字格引导线 (ui-v5-align §2 查字窗行 1, 2026-09-26 批; 阶段5 architect 修正:
 * 母版八向 = 十字 + 两对角**恰四条线**, 无四宫线——旧「灰实线十字+四宫」退役, 四宫
 * 线一并退役不保留)。点线画法（点距 3px: 1 on 2 off; 母版 dasharray 3 on 9 off,
 * 密度差仅 host 观感, 像素级归真机轮）。**纯黑点线** (260929-char-window-v5-trim:
 * 原灰度点在设备 RLCD 阈值化后不可见、真机呈空框——改纯黑, 与示范字同色)。
 *
 * q7 修法: 格点不再直绘 canvas (必被 blit 覆写吃掉), 改写 g_page.rast 1bpp 光栅域
 * (纯黑 = ink 位 1), 由 charpage_blit 兜底一次成型; 调用点移入 charpage_render_frame
 * (memset 之后、笔画循环之前——每帧重画格点, 渲染帧路径自洽)。 */
static void charpage_draw_grid(int size)
{
    int stride = (size + 7) / 8;

    for (int i = 0; i < size; i += 3) {
        /* 十字点线 (纯黑 = 1bpp ink 位) */
        charpage_rast_ink(g_page.rast, stride, i, size / 2);
        charpage_rast_ink(g_page.rast, stride, size / 2, i);
        /* 两对角点线: 主对角 + 副对角 */
        charpage_rast_ink(g_page.rast, stride, i, i);
        charpage_rast_ink(g_page.rast, stride, i, size - 1 - i);
    }
}

/* ------------------------------------------------------------------------- *
 * 重绘一帧: 已写(实心黑) + 当前(白衬边) + 未写(medians 虚线点链 6px)。
 * 示范字只走 kidpal_char_raster_stroke 数据光栅渲染路径 (教学字体红线)。
 * ------------------------------------------------------------------------ */
static void charpage_render_frame(int cur_stroke, int written)
{
    int size = CHARPAGE_CANVAS_SIZE;
    int stride = (size + 7) / 8;

    if (g_page.rast == NULL || g_page.canvas == NULL) {
        return;
    }
    memset(g_page.rast, 0, (size_t)size * (size_t)stride);
    /* 米字格点线每帧重画 (q7: 格点入 rast 域, 与笔画同帧由 blit 兜底一次成型上屏——
     * 旧「建窗时直绘 canvas」的格点被 blit 全幅覆写吃掉, 真机不可见缺陷本体)。 */
    charpage_draw_grid(size);

    for (int i = 0; i < g_page.n_strokes; i++) {
        int st = kidpal_char_stroke_state(i, written, cur_stroke);
        /* 预解析点集（parse_data 落堆: 帧路径零 kidpal_char_stroke_parse 调用、
         * 零 [256] 级栈局部数组——M2-K1 缺陷②去栈）。 */
        int n = (g_page.pre_pts != NULL && g_page.pre_counts != NULL)
                    ? g_page.pre_counts[i]
                    : 0;
        if (n <= 0) {
            continue;
        }
        const kidpal_cs_point *pts =
            g_page.pre_pts + (size_t)i * CHARPAGE_MAX_PTS;
        if (st == 2 /* PENDING */) {
            /* 未写: 沿 medians 画虚线点链, 点距 kidpal_char_dash_dot_pitch() */
            kidpal_cs_point mids[64];
            int mn = g_page.median_jsons[i]
                         ? kidpal_char_medians_parse(g_page.median_jsons[i], mids, 64)
                         : -1;
            int pitch = kidpal_char_dash_dot_pitch();
            if (mn <= 0) {
                continue;
            }
            for (int s = 0; s < mn; s++) {
                /* 沿笔尖骨架每隔 pitch(=6px) 点距打一个点, 虚线点链; q7: 写 1bpp
                 * 光栅域 (呈纯黑点链——1bpp 无灰度, spine q7 ruling 钉此形状)。 */
                if (s % pitch != 0) {
                    continue;
                }
                int dx, dy;
                charpage_map(size, mids[s].x, mids[s].y, &dx, &dy);
                if (dx >= 0 && dx < size && dy >= 0 && dy < size) {
                    charpage_rast_ink(g_page.rast, stride, dx, dy);
                }
            }
            continue;
        }
        if (st == 0 /* WRITTEN */) {
            /* 已写: 轮廓变换到 canvas 尺寸后光栅填充 (实心黑); sc = 静态承载
             * (const pts 手工经静态数组拷贝并缩放到 canvas 域, 帧路径零栈) */
            static kidpal_cs_point sc[CHARPAGE_MAX_PTS];
            if (n > CHARPAGE_MAX_PTS) {
                n = CHARPAGE_MAX_PTS;
            }
            for (int k = 0; k < n; k++) {
                int ox, oy;
                charpage_map(size, pts[k].x, pts[k].y, &ox, &oy);
                sc[k].x = (float)ox;
                sc[k].y = (float)oy;
            }
            if (kidpal_char_raster_stroke(sc, n, g_page.rast, size, size, stride) != 0) {
                /* 该笔无法填充 -> 跳过, 不致整页崩溃 */
            }
        } else if (st == 1 /* CURRENT */) {
            /* 当前笔: 白衬边 = 光栅 + 外扩一笔 (简化: 先画黑, 再由衬边重叠);
             * sc = 静态承载 (帧路径零栈, 与 WRITTEN 分支同款) */
            static kidpal_cs_point sc[CHARPAGE_MAX_PTS];
            if (n > CHARPAGE_MAX_PTS) {
                n = CHARPAGE_MAX_PTS;
            }
            for (int k = 0; k < n; k++) {
                int ox, oy;
                charpage_map(size, pts[k].x, pts[k].y, &ox, &oy);
                sc[k].x = (float)ox;
                sc[k].y = (float)oy;
            }
            if (kidpal_char_raster_stroke(sc, n, g_page.rast, size, size, stride) != 0) {
            }
        }
    }

    charpage_blit(size);
}

/* 底部提示条文案刷新: 消费速查卡单源 kidpal_char_card_build 的 out->progress
 * (演示中「第 X 笔·共 N 笔」/ 定格「写完啦·共 N 笔」——S-F3.3 AC2 演示写完定格文案上屏;
 * 本窗零重抄文案格式, 文案文本只住 char_stroke.c 契约)。cur_stroke < 0 = 定格/
 * 演示写完（与 kidpal_char_anim_state 的 REST 相位同源）。 */
static void charpage_refresh_hint(int cur_stroke)
{
    /* repositional -> designated initializer (260920-m2-charwindow-card-fix):
     * kidpal_char_info 增 meaning 成员后, positional 补成员易漏/易漂移——designated
     * 防成员漂移（plan.md 改动面 3④）。meaning 透传 g_page.card.meaning（缺义已由
     * card_build 装配空串）。 */
    kidpal_char_info info = {
        .pinyin       = g_page.card.pinyin,
        .radical      = g_page.card.radical,
        .stroke_count = g_page.n_strokes,
        .words        = g_page.card.words,
        .meaning      = g_page.card.meaning,
    };
    kidpal_char_card c;

    if (kidpal_char_card_build(&info, cur_stroke, &c) != 0) {
        return; /* 数据面缺字段防御: 保持上次文案, 不盲写 */
    }
    if (strcmp(g_page.card.progress, c.progress) == 0) {
        return; /* 文案未变 -> 不重复 set（省 LVGL 无效化） */
    }
    snprintf(g_page.card.progress, sizeof(g_page.card.progress), "%s", c.progress);
    if (g_page.hint_label != NULL) {
        lv_label_set_text(g_page.hint_label, g_page.card.progress);
    }
}

/* 面板销毁即停表: 面板可经多条路径拆掉 (关窗清层换页清理 / 重开 lv_obj_delete / 换屏),
 * 全部收敛在 panel 的 LV_EVENT_DELETE 上删动画 timer——否则关页后 timer 存活,
 * 下一拍 charpage_render_frame 摸已删 canvas → 空指针崩溃
 * (2026-09-18 真机实证: BOOT 离开查字页 render HOME 下一拍 LoadProhibited @0x0)。 */
static void charpage_on_panel_delete(lv_event_t *e)
{
    (void)e;
    if (g_page.timer != NULL) {
        lv_timer_del(g_page.timer);
        g_page.timer = NULL;
    }
    /* 同步清 panel/canvas 句柄：外部路径（关窗清层 —— dismiss_front_overlay 清 transient
     * 后重渲基层页 / 翻页清屏 / 换屏）销毁本面板后句柄不得悬垂，否则下一扇窗打开时
     * show() 会对已删对象二次 lv_obj_delete（双重删除崩溃）。 */
    g_page.panel = NULL;
    g_page.canvas = NULL;
}

/* 动画节拍回调 */
static void charpage_timer_cb(lv_timer_t *timer)
{
    struct CharPageState *p = (struct CharPageState *)lv_timer_get_user_data(timer);
    int cur_stroke = -1, written = 0;
    int phase;

    if (p == NULL) {
        return;
    }
    if (p->canvas == NULL) {
        return; /* 面板已拆但 timer 尚未销毁的窗口期: 空守卫, 不摸悬空句柄 */
    }
    phase = kidpal_char_anim_state(p->elapsed_ms, p->n_strokes, &cur_stroke, &written);
    if (phase < 0) {
        return;
    }
    if (phase == 1 /* KIDPAL_CS_PHASE_REST */) {
        /* 演完定格闸断 (v1.59 S-F3.1 AC3 / S-F3.3 AC2): 渲染定格帧 + 刷新定格文案
         * 后停表并返回——**不推进 elapsed_ms**。取模回绕的自动重播在装配层闸断
         * (char_stroke.c 纯逻辑合约零触碰, DC-1); 重播唯一路径 = KEY 短按 -> replay
         * 的 lv_timer_resume。 */
        charpage_render_frame(cur_stroke, written);
        charpage_refresh_hint(cur_stroke);
        lv_timer_pause(timer);
        return;
    }
    p->elapsed_ms += CHARPAGE_TIMER_PERIOD_MS;
    /* 每帧重绘: 已写/当前/未写三态; 底部提示条随相位刷新 (演示中进度 / 定格文案)。 */
    charpage_render_frame(cur_stroke, written);
    charpage_refresh_hint(cur_stroke);
    (void)phase;
}

/* ------------------------------------------------------------------------- *
 * 换皮消费接线 (包 9b HWV-A6)：查字窗田字格大框 (frm_236_236) 真机位图接入。
 *
 * char_page.cc 是设备装配面（非 host 编译），故这里的 loader 消费不影响 host 端
 * char_stroke 纯逻辑齿，也不触发 U B8 逐笔演示守卫（B8 只扫 char_stroke.c）。
 * 经 U 单一资产层 loader (kidpal_art_load + kidpal_art_pixel) 把田字格大框 KAP1 合
 * 成一枚 ARGB8888 lv_image_dsc（墨=不透明黑、纸/非阈值化=透明），叠在米字格画布外圈
 * 作静态 chrome —— 示范字本身仍走 kidpal_char_raster_stroke 数据光栅路径（教学字体
 * 红线 CHR-A7：绝不以整帧预烘焙位图替换逐笔演示）。未 mount / 加载失败 -> 静默降级，
 * 米字格引导线仍在（画布自绘灰边框已随 v5 收敛撤除——重烘框资产接管外圈, 见 show()）。
 *
 * 为何查字窗的资产消费落在此 .cc 装配面而非 char_stroke.c：char_stroke.c 是 U 冻结的纯
 * 逻辑合约，B8 守卫禁止其引 loader；而参数化整帧缝 (draw_nine_slice/draw_dots) 会把
 * art_assets.c 的整帧 g_fb[400x300] (~480KB 静态) 拖进设备 .dram0.bss（且设备端该缝
 * 为弱默认 no-op，真机渲染节点殿后本包）——直接撑爆 ESP32-S3 内部 RAM。故查字窗只走
 * loader→lv_image 的设备真实通路，既消费单一资产层、又不引整帧 g_fb。
 * ------------------------------------------------------------------------ */
#define CHARPAGE_FRAME_W 236
static lv_image_dsc_t s_cp_frame_dsc;
static uint32_t      *s_cp_frame_px = NULL;
static int            s_cp_frame_st = 0; /* 0=未试 1=可用 2=不可用 */

static const lv_image_dsc_t *charpage_art_frame_dsc(void)
{
    kidpal_art_img_t img;

    if (s_cp_frame_st == 2) {
        return NULL;
    }
    if (s_cp_frame_st == 1) {
        return &s_cp_frame_dsc;
    }
    if (kidpal_art_load("frm_236_236.bin", &img) != 0 ||
        img.w <= 0 || img.h <= 0 || img.w > CHARPAGE_FRAME_W || img.h > CHARPAGE_FRAME_W) {
        s_cp_frame_st = 2;
        return NULL;
    }
    s_cp_frame_px = (uint32_t *)lv_malloc((size_t)img.w * img.h * sizeof(uint32_t));
    if (s_cp_frame_px == NULL) {
        s_cp_frame_st = 2;
        return NULL;
    }
    for (int y = 0; y < img.h; y++) {
        for (int x = 0; x < img.w; x++) {
            int p = kidpal_art_pixel(&img, x, y);
            s_cp_frame_px[y * img.w + x] = (p == 1) ? 0xFF000000u : 0x00000000u;
        }
    }
    memset(&s_cp_frame_dsc, 0, sizeof(s_cp_frame_dsc));
    s_cp_frame_dsc.header.cf     = LV_COLOR_FORMAT_ARGB8888;
    s_cp_frame_dsc.header.w      = img.w;
    s_cp_frame_dsc.header.h      = img.h;
    s_cp_frame_dsc.header.stride = (uint32_t)img.w * 4u;
    s_cp_frame_dsc.data          = (const uint8_t *)s_cp_frame_px;
    s_cp_frame_dsc.data_size     = (uint32_t)(img.w * img.h * 4);
    s_cp_frame_st = 1;
    return &s_cp_frame_dsc;
}

static void charpage_art_overlay(void)
{
    const lv_image_dsc_t *fr = charpage_art_frame_dsc();

    if (fr != NULL) {
        /* 框住米字格画布 (canvas: x 4..212 / y 8..216)；236 框锚 (0,4) 完整入屏。
         * 后建于 canvas -> z 序在上，透明内芯只显外圈墨边（框）不遮示范字。 */
        lv_obj_t *fim = lv_image_create(g_page.panel);
        lv_image_set_src(fim, fr);
        lv_obj_set_pos(fim, 0, 4);
    }
}

int kidpal_char_page_started(void)
{
    return g_page.started; /* v1.59 演示起播态外露 (WF-F3-01 键帽随态变; lcd_display.cc
                            * 键帽条查字窗前台分支经 include 消费) */
}

void kidpal_char_page_replay(void)
{
    /* KEY 短按 = 首次起播 / 从头重播同路径 (v1.59 S-F3.1 AC5 / S-F3.3 AC1)。replay 经
     * 板级 dispatch 的 app.Schedule 在主任务跑、当前无 LVGL 锁——锁内归零动画态 + 置
     * 起播位 + 启表, 锁外刷键帽 (「播报」->「重播」; 内部递归取锁安全, kidpal_render_
     * node.cc:141 同款先例)。 */
    if (g_page.panel == NULL || g_page.timer == NULL) {
        return; /* 窗未开 / 表未建: 无重播对象, 防御返回 (不摸悬垂句柄) */
    }
    if (!lvgl_port_lock(30000)) {
        ESP_LOGE(CHARPAGE_TAG, "replay lock timeout started=%d", g_page.started);
        return;
    }
    g_page.elapsed_ms = 0;   /* 从头起播 (首播/重播同路径) */
    g_page.started = 1;      /* 演示起播态置位 (键帽「播报」->「重播」) */
    lv_timer_resume(g_page.timer);
    lvgl_port_unlock();
    kidpal_lcd_keybar_sync(); /* 锁外刷键帽: 「播报」->「重播」 */
    ESP_LOGI(CHARPAGE_TAG, "char page replay from first stroke");
}

void kidpal_char_page_show(const char *data_json)
{
    if (data_json == NULL) {
        ESP_LOGW(CHARPAGE_TAG, "invoked with NULL data — degrade, no blind render");
        return;
    }
    if (kidpal_char_data_validate(data_json) != 0) {
        /* 缺字段防御降级: 友好提示、不盲渲染、不崩 (闸 A 裁定 / UNKNOWN 对偶) */
        ESP_LOGW(CHARPAGE_TAG, "char data missing required keys — degrade, no blind render");
        return;
    }

    /* 上一扇窗的残留一律先清：外部路径（关窗清层 / 翻页清屏）可能已销毁 panel、只剩
     * 堆缓冲与句柄——若以 panel 存活与否决定是否回收，会漏释放且对悬垂 panel 再
     * lv_obj_delete（双重删除崩溃）。先删 LVGL 对象（触发停表 + 清句柄），再释放堆缓冲。 */
    if (g_page.panel != NULL) {
        lv_obj_delete(g_page.panel);
    }
    charpage_free_state();
    memset(&g_page, 0, sizeof(g_page));

    if (!charpage_parse_data(data_json)) {
        ESP_LOGW(CHARPAGE_TAG, "char data unparseable — degrade, no blind render");
        return;
    }

    /* 查字窗已并入 s_kidpal_scr 体系（归一屏单屏不变式，M2-K3 页→窗改造）：**在当前页
     * 之上叠一层窗面板**，不自建独立屏、**不清屏**——底层原页对象保持存活，关窗
     * （dismiss_front_overlay 清 transient 层 + 重渲基层页）后原页原样回来、不翻页。
     * 后建的 panel 是活动屏末位子对象，z 序天然叠于既有页内容之上（覆盖窗）。活动屏由
     * 调用方保证 = s_kidpal_scr（mcp_server char 路由经 begin_char_window 切屏）。 */
    lv_obj_t *host = lv_screen_active();
    if (host == NULL) {
        ESP_LOGW(CHARPAGE_TAG, "no active screen — degrade, no blind render");
        return;
    }

    /* 覆盖窗面板（挂在活动屏上、叠于当前页之上；不新建顶层屏、不清底、不翻页）。 */
    g_page.panel = lv_obj_create(host);
    lv_obj_set_size(g_page.panel, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(g_page.panel, lv_color_make(0xff, 0xff, 0xff), 0);
    lv_obj_set_style_bg_opa(g_page.panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(g_page.panel, 0, 0);
    lv_obj_set_style_pad_all(g_page.panel, 0, 0);
    lv_obj_set_style_border_width(g_page.panel, 0, 0);
    lv_obj_clear_flag(g_page.panel, LV_OBJ_FLAG_SCROLLABLE);

    /* 动画 timer 与面板同寿: 面板删 → timer 删 (见 charpage_on_panel_delete) */
    lv_obj_add_event_cb(g_page.panel, charpage_on_panel_delete, LV_EVENT_DELETE, NULL);

    /* 米字格大字盘 (左, ~208px) */
    g_page.canvas_buf = (lv_color_t *)malloc(sizeof(lv_color_t) *
                                             (size_t)CHARPAGE_CANVAS_SIZE *
                                             (size_t)CHARPAGE_CANVAS_SIZE);
    g_page.rast = (uint8_t *)calloc(1, (size_t)((CHARPAGE_CANVAS_SIZE + 7) / 8) *
                                           (size_t)CHARPAGE_CANVAS_SIZE);
    g_page.canvas = lv_canvas_create(g_page.panel);
    lv_obj_set_pos(g_page.canvas, 4, 8);
    /* 画布自绘灰边框撤除 (v5 收敛): 重烘框资产 (frm_236_236 正圆角矩形、纯黑 3px 边框)
     * 已接管外圈——保留自绘方框会与圆角框叠成双框; 米字格引导线仍自绘 (纯黑点线,
     * q7 起入 rast 域由 charpage_render_frame 每帧重画——见 charpage_draw_grid)。 */
    if (g_page.canvas_buf != NULL && g_page.rast != NULL) {
        lv_canvas_set_buffer(g_page.canvas, g_page.canvas_buf,
                             CHARPAGE_CANVAS_SIZE, CHARPAGE_CANVAS_SIZE,
                             LV_COLOR_FORMAT_NATIVE);
        /* 建窗首帧 = 静态整字 (v1.59 S-F3.1 AC1 前半): cur_stroke=-1 + written=n_strokes
         * -> kidpal_char_stroke_state 判每笔 WRITTEN = 整字实心黑 (char_stroke.c 相位复用,
         * 冻结面零触碰)。此时 n_strokes = strokes 数组数 (strokeCount JSON 覆写在其后)。
         * 米字格点线不再于建窗时直绘 canvas——由 render_frame 内重画 (q7)。 */
        charpage_render_frame(-1, g_page.n_strokes);
    }

    /* 换皮消费接线 (包 9b)：田字格大框位图 + 整帧缝生产调用点。 */
    charpage_art_overlay();

    /* V5 对齐 (ui-v5-align §2 查字窗行 2, 2026-09-26 批): 框内左上「跟着写」黑帽标
     * （设计稿黑底白字帽标; 无烘焙资产 -> 程序化黑底圆角 + 白字, 同提醒「记得哦」
     * 气泡先例; 资产化归后续资产轮）。后建于框位图 -> z 序在上不被遮。 */
    {
        lv_obj_t *chip = lv_obj_create(g_page.panel);
        lv_obj_remove_style_all(chip);
        lv_obj_set_size(chip, LV_SIZE_CONTENT, 20);
        lv_obj_set_style_bg_color(chip, lv_color_black(), 0);
        lv_obj_set_style_bg_opa(chip, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(chip, 4, 0);
        lv_obj_set_style_pad_hor(chip, 8, 0);
        lv_obj_set_style_pad_ver(chip, 2, 0);
        lv_obj_set_pos(chip, 10, 12);
        lv_obj_clear_flag(chip, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_t *chip_lbl = lv_label_create(chip);
        lv_label_set_text(chip_lbl, "跟着写");
        lv_obj_set_style_text_color(chip_lbl, lv_color_white(), 0);
        lv_obj_set_style_text_font(chip_lbl, &font_puhui_14_1, 0);
        lv_obj_center(chip_lbl);
    }

    /* 右栏速查卡: 拼音带声调 + 共 N 画 + 部首 + 释义 (数据面委托
     * kidpal_char_card_build 装配语义, 窗内呈现; S-F3.2 AC1 四行) */
    {
        cJSON *root = cJSON_Parse(data_json);
        const char *pinyin = "?" , *radical = "?" , *ch = "?" ,
                   *meaning = "?";
        int stroke_count = g_page.n_strokes;
        if (root != NULL) {
            cJSON *j = cJSON_GetObjectItem(root, "pinyin");
            if (j && cJSON_IsString(j)) pinyin = j->valuestring;
            j = cJSON_GetObjectItem(root, "radical");
            if (j && cJSON_IsString(j)) radical = j->valuestring;
            j = cJSON_GetObjectItem(root, "char");
            if (j && cJSON_IsString(j)) ch = j->valuestring;
            j = cJSON_GetObjectItem(root, "strokeCount");
            if (j && cJSON_IsNumber(j) && j->valueint > 0) stroke_count = j->valueint;
            j = cJSON_GetObjectItem(root, "meaning");
            if (j && cJSON_IsString(j)) meaning = j->valuestring;
        }
        g_page.n_strokes = stroke_count;

        /* repositional -> designated initializer (260920-m2-charwindow-card-fix;
         * plan.md 改动面 3④): kidpal_char_info 增 meaning 后 positional 补成员易漂移。
         * meaning 缺省走 "?" 兜底——card_build 的 NULL 容忍分支装配空串呈现面再兜底。 */
        kidpal_char_info info_o = {
            .pinyin       = pinyin,
            .radical      = radical,
            .stroke_count = stroke_count,
            .words        = "",   /* 组词行整删 (v5 收敛): 空串满足 card_build 非 NULL,
                                   * card.words 成员保留 (char_stroke.c 零触, DC-1)。 */
            .meaning      = meaning,
        };
        memset(&g_page.card, 0, sizeof(g_page.card));
        /* 初始装配 (cur_stroke < 0 = 定格语义; 动画拍内按相位刷新进度文案)。 */
        kidpal_char_card_build(&info_o, -1, &g_page.card);

        lv_obj_t *cardlabel = lv_label_create(g_page.panel);
        /* 右栏去遮挡 (MCD-A1): 恒显顶部状态带 top_bar_/status_bar_（LV_OPA_50 半透,
         * y0-24 任何状态恒显）压住拼音行——起点 y8 -> y30 清出状态带 + 余量 (内容让位,
         * 不动 chrome)。右栏让位 (v5 收敛 CWVT-A5): 文本左缘 x 228 -> 244 清出重烘圆角
         * 框右缘 235 (旧 228 被框右缘压住, human 2026-09-29 真机轮 finding④); 配定宽 +
         * wrap: 释义改中文后变长必冲出右缘被裁 (右缘 388 - x244 = 144), 同刀钉死。 */
        lv_obj_set_pos(cardlabel, 244, 30);
        lv_obj_set_width(cardlabel, 144);
        lv_label_set_long_mode(cardlabel, LV_LABEL_LONG_WRAP);
        /* 四行拼装缓冲: 各字段 snprintf 截断上限和（pinyin/radical 47 + stroke_total +
         * meaning 95 + 标签/换行，编译分析最坏 ~259B）——四行不能因缓冲太小被尾部截断
         * （S-F3.2 AC1 四行全见; 并满足 -Werror=format-truncation 设备构建闸）。 */
        char buf[384];
        /* 右栏速查卡四行拼装 (S-F3.2 AC1 v1.60 四行口径): 拼音带声调 + 共 N 画 + 部首 +
         * 释义 (组词行整删——运行时 words 恒空、口径收敛, human 2026-09-29 真机轮裁定)。
         * MCD-A3 空行不显示: 部首数据为空 -> 该行不拼装（无空行、无占位符）。释义行保留
         * MCF 裁定的「释义：—」缺义兜底（human 裁: 缺义只损右栏一行、不降级整窗——
         * 形态钉死不留现场裁量; — = U+2014 全角破折号）。四行 < buf[384] 容量（拼音/
         * 部首 47 + stroke 47 + 释义 95 + 标签/换行，编译分析最坏 ~259B; 部首行条件化后
         * 恒不大于原无条件最坏值）。 */
        {
            size_t w = 0;
            w += (size_t)snprintf(buf + w, sizeof(buf) - w, "%s\n%s\n",
                                  g_page.card.pinyin, g_page.card.stroke_total);
            if (g_page.card.radical[0] != '\0') {        /* 部首非空才拼该行 */
                w += (size_t)snprintf(buf + w, sizeof(buf) - w, "部首：%s\n",
                                      g_page.card.radical);
            }
            (void)snprintf(buf + w, sizeof(buf) - w, "释义：%s\n",
                           (g_page.card.meaning[0] != '\0')
                               ? g_page.card.meaning : "—");
        }
        lv_label_set_text(cardlabel, buf);
        lv_obj_set_style_text_font(cardlabel, &font_puhui_14_1, 0);
        if (root != NULL) cJSON_Delete(root);
    }

    /* 底部提示条: 文案消费 kidpal_char_card_build 的 progress 单源 (演示中「第 X 笔·
     * 共 N 笔」/ 演示写完定格「写完啦·共 N 笔」——S-F3.3 AC2, 本窗零重抄文案格式)。 */
    g_page.hint_label = lv_label_create(g_page.panel);
    /* 提示条让位 (MCD-A2): 底部提示条原 (0,280) 落 bottom_bar_ 键帽条带
     * （LV_ALIGN_BOTTOM_MID；2026-09-26 真机对齐轮起高 38 -> 带区 y262-300，旧注
     * 载高 48/y252-300）内被压——上移到 y244 避开该带。 */
    lv_obj_set_pos(g_page.hint_label, 0, 244);
    lv_label_set_text(g_page.hint_label, g_page.card.progress);
    lv_obj_set_style_text_align(g_page.hint_label, LV_TEXT_ALIGN_CENTER, 0);
    /* 底部提示条显式接线 font_puhui_14_1（v1.56 对账④: 不赌 s_kidpal_scr 字体继承——
     * 中文 label 绝不经由继承落到纯拉丁默认字体, 需显式钉全量普惠体）。 */
    lv_obj_set_style_text_font(g_page.hint_label, &font_puhui_14_1, 0);
    /* 建窗零动画拍 (v1.59 S-F3.1 AC1 后半): 不再刷首拍「第 1 笔」文案——初始提示条保持
     * 上方 card_build 单源定格文案「写完啦·共 N 笔」(g_page.card.progress, card 以
     * cur_stroke=-1 装配, 零新文案零重抄)。 */

    /* 逐笔动画驱动 */
    g_page.timer = lv_timer_create(charpage_timer_cb, CHARPAGE_TIMER_PERIOD_MS, &g_page);
    if (g_page.timer != NULL) {
        lv_timer_set_user_data(g_page.timer, &g_page);
    }
    g_page.elapsed_ms = 0;

    /* 建窗即停表 (v1.59 S-F3.1 AC1 后半): timer 创建后立即 pause, 未按 KEY 前零逐笔
     * 演示——起播/重播唯一启动路径 = replay() 的 lv_timer_resume (DC-2 裁定: 保 MCF-A1
     * 区域齿末端锚 g_page.timer = lv_timer_create 原位不动, 本句紧跟其后)。 */
    if (g_page.timer != NULL) {
        lv_timer_pause(g_page.timer);
    }

    /* 建窗末尾刷键帽 (v1.59 WF-F3-01 键帽随态变): 此时 CHAR_WINDOW 层已由 begin_char_
     * window 推入 -> sync 的查字窗分支裁决生效; 新查字重开窗经上方 memset 归 0 ->
     * started = 0 -> 键帽回「播报」。show 已持 LVGL 锁 (begin/end_char_window 夹钳),
     * sync 内部递归取锁安全 (kidpal_render_node.cc:141 同款先例)。 */
    kidpal_lcd_keybar_sync();

    ESP_LOGI(CHARPAGE_TAG, "char page open (笔画=%d, 米字格示范字光栅渲染)",
             g_page.n_strokes);
}

/* 编译期自检: 未写虚线点链点距 6px 与契约对齐 (CHR-A5) */
static const char charpage_dash_contract[1] = { (char)(kidpal_char_dash_dot_pitch() == 6 ? 'y' : 'n') };