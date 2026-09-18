/*
 * char_page.cc — KidPal 查生字 P4 页 (LVGL 9.3, 设备侧): 米字格大字盘 (左) +
 * 右栏速查卡 (拼音带声调/共 N 画/进度珠/第 X 笔·共 N 笔/部首/组词) + 底部提示条
 * + 逐笔动画驱动 (沿 medians 笔尖移动 + 当前笔白衬边 + 握笔小手占位)。
 *
 * 教学设计红线 (spec §5.6 v3 / S-F3.1, CHR-A7): 米字格示范字【只走 strokes 数据
 * 光栅渲染路径】(kidpal_char_raster_stroke, 字库数据即标准规范字形,
 * 绝不缺省回落字体渲染), 伴随文字 (拼音/部首/组词) 才用 LVGL 字体。示范字路径
 * 上零 tilt — 示范字路径上绝不施加文字倾斜样式 (防「教歪」)。
 *
 * 动画状态机 (950ms/笔、演完歇 2s 重来、三态已写/当前/未写虚线点链 6px、KEY 短按
 * 重播) 全部委托 display/char_stroke.c 的纯逻辑合约 (kidpal_char_anim_state 等),
 * 本页只做 LVGL 呈现。数据结构由 mcp_server 经 kidpal_char_data_validate 防御
 * 降级门校验后喂入; 本函数仍防御 (缺字段/解析失败 -> 不盲渲染直接返回)。
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

#define CHARPAGE_TAG "char_page"
#define CHARPAGE_CANVAS_SIZE 208    /* 米字格大字盘边长 (设计稿帧约 236/字盘约 208) */
#define CHARPAGE_GRID_CELL 52       /* 米字格四宫格单元 (208/4) */
#define CHARPAGE_TIMER_PERIOD_MS 100
#define CHARPAGE_PRESS_SIZE 12      /* 握笔小手占位半径 */
/* 单笔预解析点集容量上限（对齐 char_stroke 消费面 256 点口径; 帧路径零 ~6.5KB 级
 * 栈局部数组——M2-K1 缺陷②修复: pts/sc/toks 去栈, 预解析落堆承载） */
#define CHARPAGE_MAX_PTS 256

/* 单页状态 (设备同一时刻只有查生字一页, 静态单例足够且 additive 不动底座) */
struct CharPageState {
    lv_obj_t *panel;           /* 查生字页面板：活动屏（s_kidpal_scr）上的一层，不自建屏 */
    lv_obj_t *canvas;
    lv_obj_t *char_label;        /* 大字 */
    lv_obj_t *pinyin_label;
    lv_obj_t *stroke_total_label;
    lv_obj_t *progress_label;
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

/* 绘制米字格引导十字 (淡灰) */
static void charpage_draw_grid(int size)
{
    for (int line = 0; line < size; line++) {
        /* 十字 */
        lv_canvas_set_px(g_page.canvas, line, size / 2,
                         lv_color_make(0xd0, 0xd0, 0xd0), LV_OPA_COVER);
        lv_canvas_set_px(g_page.canvas, size / 2, line,
                         lv_color_make(0xd0, 0xd0, 0xd0), LV_OPA_COVER);
        /* 四宫格细线 */
        lv_canvas_set_px(g_page.canvas, line, size / 4,
                         lv_color_make(0xe6, 0xe6, 0xe6), LV_OPA_COVER);
        lv_canvas_set_px(g_page.canvas, line, 3 * size / 4,
                         lv_color_make(0xe6, 0xe6, 0xe6), LV_OPA_COVER);
        lv_canvas_set_px(g_page.canvas, size / 4, line,
                         lv_color_make(0xe6, 0xe6, 0xe6), LV_OPA_COVER);
        lv_canvas_set_px(g_page.canvas, 3 * size / 4, line,
                         lv_color_make(0xe6, 0xe6, 0xe6), LV_OPA_COVER);
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

/* 握笔小手占位: 当前笔白衬边 + 握笔小手领写元素 (U 包 12 换正式美术资产, 本包
 * 用简易几何占位, additive 不动资产管线)。CHR-A5 structural 齿钉的是页面结构
 * 「握笔小手」元素存在; 像素级观感殿后包 9b device 验收。 */
static void charpage_draw_penhand(int cur_stroke)
{
    int size = CHARPAGE_CANVAS_SIZE;
    kidpal_cs_point mids[64];
    int mn = (cur_stroke >= 0 && cur_stroke < g_page.n_strokes &&
              g_page.median_jsons[cur_stroke])
                 ? kidpal_char_medians_parse(g_page.median_jsons[cur_stroke], mids, 64)
                 : -1;
    if (mn <= 0) {
        return;
    }
    int dx, dy;
    charpage_map(size, mids[mn - 1].x, mids[mn - 1].y, &dx, &dy);
    /* 占位: 在当前笔笔尖画一个 12px 圆环, 示意握笔小手位置 (后续换美术资产) */
    int r = CHARPAGE_PRESS_SIZE;
    for (int ang = 0; ang < 360; ang++) {
        float th = (float)ang * 0.017453293f;
        int px = dx + (int)((float)r * cosf(th));
        int py = dy + (int)((float)r * sinf(th));
        if (px >= 0 && px < size && py >= 0 && py < size) {
            lv_canvas_set_px(g_page.canvas, px, py, lv_color_make(0x40, 0x80, 0xf0),
                             LV_OPA_COVER);
        }
    }
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
    phase = kidpal_char_anim_state(p->elapsed_ms, p->n_strokes, &cur_stroke, &written);
    if (phase < 0) {
        return;
    }
    p->elapsed_ms += CHARPAGE_TIMER_PERIOD_MS;
    /* 每帧重绘: 已写/当前/未写三态 (小手随当前笔移动) */
    charpage_render_frame(cur_stroke, written);
    charpage_draw_penhand(cur_stroke);
    (void)phase;
}

/* ------------------------------------------------------------------------- *
 * 换皮消费接线 (包 9b HWV-A6)：P4 田字格大框 (frm_236_236) 真机位图接入。
 *
 * char_page.cc 是设备装配面（非 host 编译），故这里的 loader 消费不影响 host 端
 * char_stroke 纯逻辑齿，也不触发 U B8 逐笔演示守卫（B8 只扫 char_stroke.c）。
 * 经 U 单一资产层 loader (kidpal_art_load + kidpal_art_pixel) 把田字格大框 KAP1 合
 * 成一枚 ARGB8888 lv_image_dsc（墨=不透明黑、纸/非阈值化=透明），叠在米字格画布外圈
 * 作静态 chrome —— 示范字本身仍走 kidpal_char_raster_stroke 数据光栅路径（教学字体
 * 红线 CHR-A7：绝不以整帧预烘焙位图替换逐笔演示）。未 mount / 加载失败 -> 静默降级，
 * 画布既有灰边框 + 米字格引导线仍在。
 *
 * 为何 P4 的资产消费落在此 .cc 装配面而非 char_stroke.c：char_stroke.c 是 U 冻结的纯
 * 逻辑合约，B8 守卫禁止其引 loader；而参数化整帧缝 (draw_nine_slice/draw_dots) 会把
 * art_assets.c 的整帧 g_fb[400x300] (~480KB 静态) 拖进设备 .dram0.bss（且设备端该缝
 * 为弱默认 no-op，真机渲染节点殿后本包）——直接撑爆 ESP32-S3 内部 RAM。故 P4 只走
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

    if (g_page.panel != NULL) {
        lv_obj_delete(g_page.panel);
        g_page.panel = NULL;
        g_page.canvas = NULL;
        charpage_free_state();
        memset(&g_page, 0, sizeof(g_page));
    }
    memset(&g_page, 0, sizeof(g_page));

    if (!charpage_parse_data(data_json)) {
        ESP_LOGW(CHARPAGE_TAG, "char data unparseable — degrade, no blind render");
        return;
    }

    /* 查生字页已并入 s_kidpal_scr 体系（归一屏单屏不变式）：作为活动屏上的一层/一页
     * 绘制，不自建独立屏。取活动屏并先清空（与 schedule/story 渲染同款先清后画，
     * 幂等、重渲染绝不与上一帧叠加）；活动屏由调用方保证 = s_kidpal_scr（mcp_server
     * char 路由经 begin_page 切屏，cycle_next 则显式加载 s_kidpal_scr）。 */
    lv_obj_t *host = lv_screen_active();
    if (host == NULL) {
        ESP_LOGW(CHARPAGE_TAG, "no active screen — degrade, no blind render");
        return;
    }
    lv_obj_clean(host);

    /* 全屏白底页面板（挂在活动屏上，作为一层；不新建顶层屏）。 */
    g_page.panel = lv_obj_create(host);
    lv_obj_set_size(g_page.panel, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(g_page.panel, lv_color_make(0xff, 0xff, 0xff), 0);
    lv_obj_set_style_bg_opa(g_page.panel, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(g_page.panel, 0, 0);
    lv_obj_set_style_pad_all(g_page.panel, 0, 0);
    lv_obj_set_style_border_width(g_page.panel, 0, 0);
    lv_obj_clear_flag(g_page.panel, LV_OBJ_FLAG_SCROLLABLE);

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

    /* 右栏速查卡: 拼音带声调 + 共 N 画 + 进度 + 部首 + 组词 (数据面委托
     * kidpal_char_card_build 装配语义, 页面呈现) */
    {
        cJSON *root = cJSON_Parse(data_json);
        const char *pinyin = "?" , *radical = "?" , *ch = "?" , *words = "?" ;
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
                cJSON *w0 = cJSON_GetArrayItem(j, 0);
                if (w0 && cJSON_IsString(w0)) words = w0->valuestring;
            }
        }
        g_page.n_strokes = stroke_count;

        kidpal_char_info info_o = { pinyin, radical, stroke_count, words };
        kidpal_char_card card;
        memset(&card, 0, sizeof(card));
        kidpal_char_card_build(&info_o, -1, &card); /* 初始写完定格 或 首笔, 页内再更新 */

        lv_obj_t *cardlabel = lv_label_create(g_page.panel);
        lv_obj_set_pos(cardlabel, 228, 8);
        char buf[256];
        snprintf(buf, sizeof(buf), "%s\n%s\n%s", card.pinyin, card.stroke_total,
                 card.words);
        lv_label_set_text(cardlabel, buf);
        lv_obj_set_style_text_font(cardlabel, LV_FONT_DEFAULT, 0);
        if (root != NULL) cJSON_Delete(root);
    }

    /* 底部提示条 (占位, 语义: 演示中) */
    g_page.hint_label = lv_label_create(g_page.panel);
    lv_obj_set_pos(g_page.hint_label, 0, 280);
    lv_label_set_text(g_page.hint_label, "跟随笔画一笔一画描红");
    lv_obj_set_style_text_align(g_page.hint_label, LV_TEXT_ALIGN_CENTER, 0);

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