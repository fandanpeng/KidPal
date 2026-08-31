#include "kidpal_ui.h"
#include "kidpal_snapshot.h"
#include "assets/assets.h"
#include "lvgl/lvgl.h"

/* KidPal P1 home — 「LVGL 骨架 + 预栅格化 RGB565 资产」双层方案实战版 (260830-arch-review-ui-impl-strategy)
 * 骨架: 文本/键帽/页点/反白/列表线走 LVGL 控件
 * 资产: 吉祥物/涂鸦/温度计小人/水滴宝宝/电池/铃铛 全部 lv_image(RGB565 位图, 倾斜已烘进资产)
 */

static void colon_blink_timer(lv_timer_t* timer);

static lv_obj_t* g_colon_top = NULL;
static lv_obj_t* g_colon_btm = NULL;
static lv_font_t* g_cjk16 = NULL;    /* 正文中文 16px */
static lv_font_t* g_cjk21 = NULL;    /* 课程名 21px */
static lv_font_t* g_cjk13 = NULL;    /* 明日预告 13px */
static lv_font_t* g_num_30 = NULL;   /* 卡片时间 30px */
static lv_font_t* g_num_21 = NULL;   /* 温湿度读数 21px */

static void colon_blink_timer(lv_timer_t* timer)
{
    (void)timer;
    /* 眨眼: 4400ms 周期内只在 150ms 窗口眯眼(scaleY≈0.07),其余时间睁开 */
    static uint32_t last = 0;
    uint32_t now = lv_tick_get();
    bool squint = (now % 4400) < 150;
    if (squint == last) return;
    last = squint;
    lv_obj_t* arr[] = {g_colon_top, g_colon_btm};
    for (int i = 0; i < 2; i++) {
        lv_obj_set_height(arr[i], squint ? 3 : 23);
    }
}

static lv_obj_t* label_at(lv_obj_t* parent, const char* text, int32_t x, int32_t y,
                          const lv_font_t* font)
{
    lv_obj_t* l = lv_label_create(parent);
    lv_label_set_text(l, text);
    lv_obj_set_style_text_font(l, font, 0);
    lv_obj_set_style_text_color(l, lv_color_black(), 0);
    lv_obj_set_pos(l, x, y);
    return l;
}

/* 资产渲染: 管线直出 RGB565 图像(见 gen_assets.mjs 注释——LVGL 软件渲染的 I1 路径
 * 绕过调色板,把位图当 alpha/灰度,仅 bin-decoder 对文件源才查表;RGB565 双端行为一致)。
 * 直接 lv_image_set_src 即可,无需转换。 */

static void img_at(lv_obj_t* parent, const lv_image_dsc_t* src, int32_t x, int32_t y)
{
    lv_obj_t* im = lv_image_create(parent);
    lv_image_set_src(im, src);
    lv_obj_set_pos(im, x, y);
}

/* 键帽: 高26 圆角8 3px描边, 内嵌数字体文字——唯一的「控件长相」 */
static void keycap_at(lv_obj_t* parent, const char* text, int32_t x, int32_t y)
{
    lv_obj_t* k = lv_obj_create(parent);
    lv_obj_remove_style_all(k);
    lv_obj_set_size(k, LV_SIZE_CONTENT, 26);
    lv_obj_set_pos(k, x, y);
    lv_obj_set_style_pad_hor(k, 7, 0);
    lv_obj_set_style_radius(k, 8, 0);
    lv_obj_set_style_bg_color(k, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(k, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(k, lv_color_black(), 0);
    lv_obj_set_style_border_width(k, 3, 0);
    lv_obj_clear_flag(k, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t* l = lv_label_create(k);
    lv_label_set_text(l, text);
    lv_obj_set_style_text_font(l, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(l, lv_color_black(), 0);
    lv_obj_center(l);
}

/* 反白胶囊标签(「接下来」) */
static void inv_pill_at(lv_obj_t* parent, const char* text, int32_t x, int32_t y)
{
    lv_obj_t* p = lv_label_create(parent);
    lv_label_set_text(p, text);
    lv_obj_set_style_text_font(p, g_cjk16, 0);
    lv_obj_set_style_text_color(p, lv_color_white(), 0);
    lv_obj_set_style_bg_color(p, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(p, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(p, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_hor(p, 10, 0);
    lv_obj_set_style_pad_ver(p, 1, 0);
    lv_obj_set_pos(p, x, y);
}

static void fonts_init(void)
{
    if (g_cjk16) return;
    g_cjk16 = lv_freetype_font_create("C:/Windows/Fonts/msjh.ttc",
                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 16,
                                      LV_FREETYPE_FONT_STYLE_NORMAL);
    g_cjk21 = lv_freetype_font_create("C:/Windows/Fonts/msjhbd.ttc",
                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 21,
                                      LV_FREETYPE_FONT_STYLE_NORMAL);
    g_cjk13 = lv_freetype_font_create("C:/Windows/Fonts/msjh.ttc",
                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 13,
                                      LV_FREETYPE_FONT_STYLE_NORMAL);
    g_num_30 = lv_freetype_font_create("C:/Windows/Fonts/segoeuib.ttf",
                                       LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 30,
                                       LV_FREETYPE_FONT_STYLE_NORMAL);
    g_num_21 = lv_freetype_font_create("C:/Windows/Fonts/segoeuib.ttf",
                                       LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 21,
                                       LV_FREETYPE_FONT_STYLE_NORMAL);
}

void kidpal_ui_create(lv_obj_t* parent)
{
    fonts_init();
    lv_obj_set_style_bg_color(parent, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);
    if (g_cjk16) lv_obj_set_style_text_font(parent, g_cjk16, 0);

    /* ── 顶部状态栏: 日历块(原生) + 日期 + 电池(资产) ── */
    lv_obj_t* cal = lv_obj_create(parent);
    lv_obj_remove_style_all(cal);
    lv_obj_set_size(cal, 22, 19);
    lv_obj_set_pos(cal, 14, 11);
    lv_obj_set_style_radius(cal, 6, 0);
    lv_obj_set_style_bg_color(cal, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(cal, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(cal, lv_color_black(), 0);
    lv_obj_set_style_border_width(cal, 3, 0);
    lv_obj_clear_flag(cal, LV_OBJ_FLAG_SCROLLABLE);
    label_at(parent, "8月27日 周四", 43, 11, g_cjk16);
    img_at(parent, &ic_batt_32, 330, 11);
    label_at(parent, "80%", 364, 14, &lv_font_montserrat_14);

    /* ── 涂鸦点缀(资产, ≤2 处) ── */
    img_at(parent, &doodle_star_26, 228, 14);
    img_at(parent, &doodle_cloud_52, 230, 30);

    /* ── 大时间: 96px 数字烘成资产(含 -1° 倾斜, 消除与冒号的行距对齐问题),
     * 眨眼冒号保留原生动画 ── */
    img_at(parent, &time_10_24, 14, 42);
    g_colon_top = lv_obj_create(parent);
    lv_obj_remove_style_all(g_colon_top);
    lv_obj_set_size(g_colon_top, 19, 23);
    lv_obj_set_pos(g_colon_top, 96, 82);
    lv_obj_set_style_radius(g_colon_top, 8, 0);
    lv_obj_set_style_bg_color(g_colon_top, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(g_colon_top, LV_OPA_COVER, 0);
    lv_obj_clear_flag(g_colon_top, LV_OBJ_FLAG_SCROLLABLE);
    g_colon_btm = lv_obj_create(parent);
    lv_obj_remove_style_all(g_colon_btm);
    lv_obj_set_size(g_colon_btm, 19, 23);
    lv_obj_set_pos(g_colon_btm, 96, 110);
    lv_obj_set_style_radius(g_colon_btm, 8, 0);
    lv_obj_set_style_bg_color(g_colon_btm, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(g_colon_btm, LV_OPA_COVER, 0);
    lv_obj_clear_flag(g_colon_btm, LV_OBJ_FLAG_SCROLLABLE);
    lv_timer_create(colon_blink_timer, 30, NULL);

    /* ── 接下来卡: 边框骨架原生 + 反白胶囊 ── */
    lv_obj_t* card = lv_obj_create(parent);
    lv_obj_remove_style_all(card);
    lv_obj_set_size(card, 122, 118);
    lv_obj_set_pos(card, 268, 52);
    lv_obj_set_style_radius(card, 14, 0);
    lv_obj_set_style_bg_color(card, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(card, lv_color_black(), 0);
    lv_obj_set_style_border_width(card, 3, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    inv_pill_at(parent, "接下来", 296, 40);
    label_at(parent, "14:00", 282, 66, g_num_30);
    label_at(parent, "钢琴课", 290, 102, g_cjk21);
    label_at(parent, "琴房 2", 300, 130, g_cjk16);

    /* 倒计时贴纸: 白底胶囊 + 铃铛资产 */
    lv_obj_t* stk = lv_obj_create(parent);
    lv_obj_remove_style_all(stk);
    lv_obj_set_size(stk, LV_SIZE_CONTENT, 22);
    lv_obj_set_pos(stk, 252, 180);
    lv_obj_set_style_pad_left(stk, 9, 0);
    lv_obj_set_style_pad_right(stk, 6, 0);
    lv_obj_set_style_radius(stk, 10, 0);
    lv_obj_set_style_bg_color(stk, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(stk, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(stk, lv_color_black(), 0);
    lv_obj_set_style_border_width(stk, 3, 0);
    lv_obj_clear_flag(stk, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t* sl = lv_label_create(stk);
    lv_label_set_text(sl, "还有 3 个半小时");
    lv_obj_set_style_text_font(sl, g_cjk16, 0);
    lv_obj_set_style_text_color(sl, lv_color_black(), 0);
    lv_obj_align(sl, LV_ALIGN_LEFT_MID, 0, 0);
    img_at(parent, &ic_bell_13, 372, 184);

    /* ── 明日预告虚线分隔(原生 dash line) ── */
    lv_obj_t* line = lv_line_create(parent);
    static lv_point_precise_t line_pts[] = {{16, 182}, {212, 182}};
    lv_line_set_points(line, line_pts, 2);
    lv_obj_set_style_line_color(line, lv_color_black(), 0);
    lv_obj_set_style_line_width(line, 3, 0);
    lv_obj_set_style_line_dash_width(line, 2, 0);
    lv_obj_set_style_line_dash_gap(line, 5, 0);
    label_at(parent, "明天 09:00 · 游泳课", 16, 188, g_cjk13);

    /* ── 温湿度小人(资产) + 读数 ── */
    img_at(parent, &thermo_man, 118, 196);
    label_at(parent, "22°", 162, 206, g_num_21);
    img_at(parent, &drop_baby, 216, 204);
    label_at(parent, "45%", 252, 212, g_num_21);

    /* ── 小伴趴场(资产, -1° 已烘入) ── */
    img_at(parent, &pose_daily_96, 8, 172);

    /* ── 底部键帽提示条: 38px 通栏贴底 ── */
    lv_obj_t* bar = lv_obj_create(parent);
    lv_obj_remove_style_all(bar);
    lv_obj_set_size(bar, 400, 38);
    lv_obj_set_pos(bar, 0, 262);
    lv_obj_set_style_radius(bar, 0, 0);
    lv_obj_set_style_bg_color(bar, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(bar, lv_color_black(), 0);
    lv_obj_set_style_border_width(bar, 3, 0);
    lv_obj_set_style_border_side(bar, LV_BORDER_SIDE_TOP, 0);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);
    keycap_at(parent, "KEY", 10, 268);
    label_at(parent, "按一下说话", 60, 271, g_cjk16);
    /* 页点: 当前页实心放大 */
    for (int i = 0; i < 4; i++) {
        lv_obj_t* d = lv_obj_create(parent);
        lv_obj_remove_style_all(d);
        int on = (i == 0);
        lv_obj_set_size(d, on ? 11 : 9, on ? 11 : 9);
        lv_obj_set_pos(d, 186 + i * 15, 275);
        lv_obj_set_style_radius(d, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(d, on ? lv_color_black() : lv_color_white(), 0);
        lv_obj_set_style_bg_opa(d, LV_OPA_COVER, 0);
        lv_obj_set_style_border_color(d, lv_color_black(), 0);
        lv_obj_set_style_border_width(d, 2, 0);
        lv_obj_clear_flag(d, LV_OBJ_FLAG_SCROLLABLE);
    }
    label_at(parent, "翻页", 300, 271, g_cjk16);
    keycap_at(parent, "BOOT", 342, 268);

    /* ── 1bpp 阈值化快照: 一键截当前屏幕 → 阈值化 PNG → 退出(正式工具 kidpal_snapshot.c) ── */
    kidpal_snapshot_schedule();
}
