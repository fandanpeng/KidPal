/*
 * char_page.cc — KidPal 查字窗 `char-window` (LVGL 9.3, 设备侧): 米字格大字盘 (左) +
 * 右栏速查卡 (拼音带声调 / 共 N 画 / 部首 / 组词) + 底部提示条 + 逐笔动画驱动
 * (沿 medians 笔尖移动 + 当前笔白衬边)。**会话内覆盖窗**（M2-K3 页→窗改造）: 在当前页
 * 之上叠一层窗面板、**不清屏**——关窗（dismiss_front_overlay 清 transient 回基层）后
 * 原页原样回来、不翻页。
 *
 * 教学设计红线 (spec §5.6 v3 / S-F3.1, CHR-A7): 米字格示范字【只走 strokes 数据
 * 光栅渲染路径】(kidpal_char_raster_stroke, 字库数据即标准规范字形,
 * 绝不缺省回落字体渲染), 伴随文字 (拼音/部首/组词) 才用 LVGL 字体。示范字路径
 * 上零 tilt — 示范字路径上绝不施加文字倾斜样式 (防「教歪」)。
 *
 * 动画状态机 (950ms/笔、演完歇 2s 重来、三态已写/当前/未写虚线点链 6px、KEY 短按
 * 重播) 全部委托 display/char_stroke.c 的纯逻辑合约 (kidpal_char_anim_state 等),
 * 本窗只做 LVGL 呈现。底部提示条文案消费 kidpal_char_card_build 的 out->progress
 * 单源 (演示中「第 X 笔·共 N 笔」/ 定格「写完啦·共 N 笔」, S-F3.3 AC2——本窗零重抄
 * 文案格式)。数据结构由 mcp_server 经 kidpal_char_data_validate 防御降级门校验后喂入;
 * 本函数仍防御 (缺字段/解析失败 -> 不盲渲染直接返回)。
 *
 * v1.38 对账清理 (本包落地): 右栏已砍项死成员清零 + 头注虚标清除; 握笔小手 12px 圆环
 * 占位 (占位绘制函数与尺寸常量整体) 删除——改口径后小手资产不再需要, 当前笔改白衬边。
 * 锚 = `char-window` (原 canvas-p4, v1.38 对账改名收尾)。
 */

#include "char_page.h"

#include "char_stroke.h"
#include "art_assets.h"

#include <cJSON.h>
#include <lvgl.h>
#include <esp_log.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cstdio>

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
     * radical/stroke_total/words 供 label 拼装, progress 供底部提示条（演示中进度 vs
     * 定格文案）。char 数组副本, cJSON 释放后安全。 */
    kidpal_char_card card;
    int elapsed_ms;
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

/* 绘制米字格引导线 (ui-v5-align §2 查字窗行 1, 2026-09-26 批; 阶段5 architect 修正:
 * 母版八向 = 十字 + 两对角**恰四条线**, 无四宫线——旧「灰实线十字+四宫」退役, 四宫
 * 线一并退役不保留)。点线画法（点距 3px: 1 on 2 off; 母版 dasharray 3 on 9 off,
 * 密度差与灰两档仅 host 观感, 像素级归真机轮）。色调弱于示范字纯黑
 * （0x99 十字 / 0xbb 对角; 设备 RLCD 阈值化后呈浅点线）。 */
static void charpage_draw_grid(int size)
{
    for (int i = 0; i < size; i += 3) {
        /* 十字 (0x99) */
        lv_canvas_set_px(g_page.canvas, i, size / 2,
                         lv_color_make(0x99, 0x99, 0x99), LV_OPA_COVER);
        lv_canvas_set_px(g_page.canvas, size / 2, i,
                         lv_color_make(0x99, 0x99, 0x99), LV_OPA_COVER);
        /* 两对角 (0xbb): 主对角 + 副对角。 */
        lv_canvas_set_px(g_page.canvas, i, i,
                         lv_color_make(0xbb, 0xbb, 0xbb), LV_OPA_COVER);
        lv_canvas_set_px(g_page.canvas, i, size - 1 - i,
                         lv_color_make(0xbb, 0xbb, 0xbb), LV_OPA_COVER);
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
                /* 沿笔尖骨架每隔 pitch(=6px) 点距打一个灰点, 虚线点链 */
                if (s % pitch != 0) {
                    continue;
                }
                int dx, dy;
                charpage_map(size, mids[s].x, mids[s].y, &dx, &dy);
                if (dx >= 0 && dx < size && dy >= 0 && dy < size) {
                    lv_canvas_set_px(g_page.canvas, dx, dy,
                                     lv_color_make(0x99, 0x99, 0x99), LV_OPA_COVER);
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
 * 画布既有灰边框 + 米字格引导线仍在。
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

/* V5 对齐 (ui-v5-align §2 查字窗行 4, 2026-09-26 批): 右栏底部★ (设计稿锚图附加;
 * ic_star_22 资产, 缺场不画——纯装饰)。dsc/px file-static 存活（lv_image 持 dsc
 * 指针, 窗重开复用零泄漏——同 frame dsc 先例）。 */
static lv_image_dsc_t s_cp_star_dsc;
static uint32_t      *s_cp_star_px = NULL;
static int            s_cp_star_st = 0; /* 0=未试 1=可用 2=缺场免重试（同 frame 先例） */

static const lv_image_dsc_t *charpage_art_star_dsc(void)
{
    kidpal_art_img_t img;

    if (s_cp_star_st == 2) {
        return NULL;
    }
    if (s_cp_star_st == 1) {
        return &s_cp_star_dsc;
    }
    if (kidpal_art_load("ic_star_22.bin", &img) != 0 ||
        img.w <= 0 || img.h <= 0) {
        s_cp_star_st = 2;
        return NULL;
    }
    s_cp_star_px = (uint32_t *)lv_malloc((size_t)img.w * img.h * sizeof(uint32_t));
    if (s_cp_star_px == NULL) {
        return NULL;
    }
    for (int y = 0; y < img.h; y++) {
        for (int x = 0; x < img.w; x++) {
            int p = kidpal_art_pixel(&img, x, y);
            s_cp_star_px[y * img.w + x] = (p == 1) ? 0xFF000000u : 0x00000000u;
        }
    }
    memset(&s_cp_star_dsc, 0, sizeof(s_cp_star_dsc));
    s_cp_star_dsc.header.cf     = LV_COLOR_FORMAT_ARGB8888;
    s_cp_star_dsc.header.w      = img.w;
    s_cp_star_dsc.header.h      = img.h;
    s_cp_star_dsc.header.stride = (uint32_t)img.w * 4u;
    s_cp_star_dsc.data          = (const uint8_t *)s_cp_star_px;
    s_cp_star_dsc.data_size     = (uint32_t)(img.w * img.h * 4);
    s_cp_star_st = 1;
    return &s_cp_star_dsc;
}

void kidpal_char_page_replay(void)
{
    g_page.elapsed_ms = 0; /* KEY 短按重播: 从第一笔重新逐笔 (S-F3.3) */
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
    lv_obj_set_style_border_width(g_page.canvas, 1, 0);
    lv_obj_set_style_border_color(g_page.canvas, lv_color_make(0xbb, 0xbb, 0xbb), 0);
    if (g_page.canvas_buf != NULL && g_page.rast != NULL) {
        lv_canvas_set_buffer(g_page.canvas, g_page.canvas_buf,
                             CHARPAGE_CANVAS_SIZE, CHARPAGE_CANVAS_SIZE,
                             LV_COLOR_FORMAT_NATIVE);
        charpage_draw_grid(CHARPAGE_CANVAS_SIZE);
        charpage_render_frame(-1, 0);
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

    /* V5 对齐 (ui-v5-align §2 查字窗行 4): 右栏底部★ (阶段5 architect 修正: 母版
     * (280,226) 悬于右栏速查卡下方; 初版 (232,218) 压框角已纠正)。ic_star_22
     * 缺场不画——纯装饰。 */
    {
        const lv_image_dsc_t *st = charpage_art_star_dsc();
        if (st != NULL) {
            lv_obj_t *star = lv_image_create(g_page.panel);
            lv_image_set_src(star, st);
            lv_obj_set_pos(star, 280, 226);
        }
    }

    /* 右栏速查卡: 拼音带声调 + 共 N 画 + 部首 + 组词 (数据面委托
     * kidpal_char_card_build 装配语义, 窗内呈现; S-F3.2 AC1 四行) */
    {
        cJSON *root = cJSON_Parse(data_json);
        const char *pinyin = "?" , *radical = "?" , *ch = "?" , *words = "?" ,
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
            j = cJSON_GetObjectItem(root, "words");
            if (j && cJSON_IsArray(j) && cJSON_GetArraySize(j) > 0) {
                /* 组词多词 join (MCD-A4): words 数组前 N 项以「、」顿号 join（非只取
                 * [0]——kidpal_char_info.words 注释本就写明「、-joined」、card.words[96]
                 * 容量按多词设计）。装配落 char_page 层, 不动 char_stroke.c 纯逻辑合约。
                 * UTF-8 边界封顶 (planner 裁决②, 装配面义务): 先按 96B 容量域逐词拷贝,
                 * 若容限截断劈了多字节半字（末字节是 0x80-0xBF 连续字节）则回退到完整
                 * 字符边界, 绝不交 card_build 半字（card_build 的 96B 字节截断冻结语义
                 * 不动, 超长串永不到它手上）。 */
                static char joined[96];                 /* 对齐 card.words[96] 容量 */
                static const char ws[] = "、";          /* 「、」= E3 80 81, 3 字节 */
                const size_t wslen = sizeof(ws) - 1;
                const int nw = cJSON_GetArraySize(j);
                size_t used = 0;
                int truncated = 0;
                for (int wi = 0; wi < nw; wi++) {
                    cJSON *w = cJSON_GetArrayItem(j, wi);
                    if (!w || !cJSON_IsString(w)) continue;
                    const char *s = w->valuestring;
                    if (used > 0) {                     /* 非首项先加顿号 */
                        if (used + wslen + 1 >= sizeof(joined)) { truncated = 1; break; }
                        memcpy(joined + used, ws, wslen);
                        used += wslen;
                    }
                    size_t sl = strlen(s);
                    if (used + sl + 1 >= sizeof(joined)) {
                        sl = sizeof(joined) - 1 - used;  /* 容量封顶 */
                        truncated = 1;
                    }
                    if (sl > 0) { memcpy(joined + used, s, sl); used += sl; }
                }
                joined[used] = '\0';
                if (truncated) {
                    /* UTF-8 半字回退: **只在真发生容量截断时**回退到完整字符边界——
                     * 否则末尾合法 CJK 字符的末字节（本身是 0x80-0xBF）会被误删。 */
                    while (used > 0 &&
                           ((unsigned char)joined[used - 1] & 0xC0) == 0x80) {
                        used--;
                    }
                    joined[used] = '\0';
                }
                words = joined;
            }
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
            .words        = words,
            .meaning      = meaning,
        };
        memset(&g_page.card, 0, sizeof(g_page.card));
        /* 初始装配 (cur_stroke < 0 = 定格语义; 动画拍内按相位刷新进度文案)。 */
        kidpal_char_card_build(&info_o, -1, &g_page.card);

        lv_obj_t *cardlabel = lv_label_create(g_page.panel);
        /* 右栏去遮挡 (MCD-A1): 恒显顶部状态带 top_bar_/status_bar_（LV_OPA_50 半透,
         * y0-24 任何状态恒显）压住拼音行——起点 y8 -> y30 清出状态带 + 余量 (内容让位,
         * 不动 chrome)。配定宽 + wrap: 释义改中文后变长必冲出右缘被裁 (右缘 388 - x228
         * = 160), 同刀钉死。 */
        lv_obj_set_pos(cardlabel, 228, 30);
        lv_obj_set_width(cardlabel, 160);
        lv_label_set_long_mode(cardlabel, LV_LABEL_LONG_WRAP);
        /* 五行拼装缓冲: 各字段 snprintf 截断上限和（pinyin/radical 47 + stroke_total +
         * meaning 95 + words 95 + 标签/换行，编译分析最坏 ~354B）——五行不能因缓冲太小被
         * 尾部截断（S-F3.2 AC1 五行全见; 并满足 -Werror=format-truncation 设备构建闸）。 */
        char buf[384];
        /* 右栏速查卡五行拼装 (S-F3.2 AC1 v1.56 五行口径): 拼音带声调 + 共 N 画 + 部首 +
         * 释义 + 组词。MCD-A3 空行不显示: 任一行（组词/部首等）数据为空 -> 该行不拼装
         * （无空行、无占位符）。释义行保留 MCF 裁定的「释义：—」缺义兜底（human 裁:
         * 缺义只损右栏一行、不降级整窗——形态钉死不留现场裁量; — = U+2014 全角破折号）。
         * 五行 < buf[384] 容量（拼音/部首 47 + stroke 47 + 释义 95 + words 95 + 标签/
         * 换行，编译分析最坏 ~354B; 部首行条件化后恒不大于原无条件最坏值）。 */
        {
            size_t w = 0;
            w += (size_t)snprintf(buf + w, sizeof(buf) - w, "%s\n%s\n",
                                  g_page.card.pinyin, g_page.card.stroke_total);
            if (g_page.card.radical[0] != '\0') {        /* 部首非空才拼该行 */
                w += (size_t)snprintf(buf + w, sizeof(buf) - w, "部首：%s\n",
                                      g_page.card.radical);
            }
            w += (size_t)snprintf(buf + w, sizeof(buf) - w, "释义：%s\n",
                                  (g_page.card.meaning[0] != '\0')
                                      ? g_page.card.meaning : "—");
            if (g_page.card.words[0] != '\0') {          /* 组词非空才拼该行 */
                (void)snprintf(buf + w, sizeof(buf) - w, "%s", g_page.card.words);
            }
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
    charpage_refresh_hint(0); /* 首拍相位 = 第 1 笔 (与动画 t=0 同源) */

    /* 逐笔动画驱动 */
    g_page.timer = lv_timer_create(charpage_timer_cb, CHARPAGE_TIMER_PERIOD_MS, &g_page);
    if (g_page.timer != NULL) {
        lv_timer_set_user_data(g_page.timer, &g_page);
    }
    g_page.elapsed_ms = 0;

    ESP_LOGI(CHARPAGE_TAG, "char page open (笔画=%d, 米字格示范字光栅渲染)",
             g_page.n_strokes);
}

/* 编译期自检: 未写虚线点链点距 6px 与契约对齐 (CHR-A5) */
static const char charpage_dash_contract[1] = { (char)(kidpal_char_dash_dot_pitch() == 6 ? 'y' : 'n') };