#include "kidpal_ui.h"
#include "lvgl/lvgl.h"

/* KidPal P1 home page — 400x300 landscape, hand-drawn black-and-white style.
 * Uses LVGL built-in primitives only (no images), so it previews without
 * external assets.  Chinese labels rely on FreeType (LV_USE_FREETYPE=1).
 */

static lv_obj_t* g_colon_top = NULL;
static lv_obj_t* g_colon_btm = NULL;
static lv_font_t* g_cjk_font = NULL;

static void colon_blink_timer(lv_timer_t* timer)
{
    (void)timer;
    if (g_colon_top && g_colon_btm) {
        lv_obj_t* arr[] = {g_colon_top, g_colon_btm};
        for (int i = 0; i < 2; i++) {
            lv_obj_t* o = arr[i];
            int32_t h = lv_obj_get_height(o);
            lv_obj_set_height(o, h == 23 ? 2 : 23);
        }
    }
}

static lv_obj_t* make_label(lv_obj_t* parent, const char* text, int32_t x, int32_t y,
                            const lv_font_t* font, lv_align_t align)
{
    lv_obj_t* lbl = lv_label_create(parent);
    lv_label_set_text(lbl, text);
    lv_obj_set_style_text_font(lbl, font ? font : g_cjk_font, 0);
    lv_obj_set_style_text_color(lbl, lv_color_black(), 0);
    lv_obj_set_pos(lbl, x, y);
    lv_obj_set_align(lbl, align);
    return lbl;
}

static void make_rounded_rect(lv_obj_t* parent, int32_t x, int32_t y,
                              int32_t w, int32_t h, int32_t radius,
                              lv_color_t bg, lv_color_t border,
                              int32_t border_width)
{
    lv_obj_t* r = lv_obj_create(parent);
    lv_obj_remove_style_all(r);
    lv_obj_set_pos(r, x, y);
    lv_obj_set_size(r, w, h);
    lv_obj_set_style_radius(r, radius, 0);
    lv_obj_set_style_bg_color(r, bg, 0);
    lv_obj_set_style_bg_opa(r, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(r, border, 0);
    lv_obj_set_style_border_width(r, border_width, 0);
    lv_obj_set_style_border_opa(r, LV_OPA_COVER, 0);
    lv_obj_clear_flag(r, LV_OBJ_FLAG_SCROLLABLE);
}

static lv_obj_t* make_pill(lv_obj_t* parent, const char* text, int32_t x, int32_t y,
                           lv_color_t bg, lv_color_t fg)
{
    lv_obj_t* lbl = lv_label_create(parent);
    lv_label_set_text(lbl, text);
    lv_obj_set_style_text_color(lbl, fg, 0);
    lv_obj_set_style_pad_hor(lbl, 8, 0);
    lv_obj_set_style_pad_ver(lbl, 2, 0);
    lv_obj_set_style_bg_color(lbl, bg, 0);
    lv_obj_set_style_bg_opa(lbl, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(lbl, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_color(lbl, lv_color_black(), 0);
    lv_obj_set_style_border_width(lbl, 2, 0);
    lv_obj_set_pos(lbl, x, y);
    return lbl;
}

/* Draw a simple Hello-Kitty-style mascot using a group of shapes.
 * Head is a wide white circle/ellipse, ears are small triangles,
 * body sits below.  We approximate the SVG pose-daily look.
 */
static void draw_mascot(lv_obj_t* parent, int32_t x, int32_t y)
{
    lv_obj_t* c = lv_obj_create(parent);
    lv_obj_remove_style_all(c);
    lv_obj_set_pos(c, x, y);
    lv_obj_set_size(c, 96, 96);
    lv_obj_set_style_bg_opa(c, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(c, LV_OBJ_FLAG_SCROLLABLE);

    /* body */
    lv_obj_t* body = lv_obj_create(c);
    lv_obj_remove_style_all(body);
    lv_obj_set_size(body, 42, 32);
    lv_obj_set_pos(body, 27, 52);
    lv_obj_set_style_radius(body, 18, 0);
    lv_obj_set_style_bg_color(body, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(body, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(body, lv_color_black(), 0);
    lv_obj_set_style_border_width(body, 3, 0);
    lv_obj_set_style_border_opa(body, LV_OPA_COVER, 0);
    lv_obj_clear_flag(body, LV_OBJ_FLAG_SCROLLABLE);

    /* left foot */
    lv_obj_t* fl = lv_obj_create(c);
    lv_obj_remove_style_all(fl);
    lv_obj_set_size(fl, 14, 9);
    lv_obj_set_pos(fl, 22, 78);
    lv_obj_set_style_radius(fl, 5, 0);
    lv_obj_set_style_bg_color(fl, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(fl, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(fl, lv_color_black(), 0);
    lv_obj_set_style_border_width(fl, 3, 0);
    lv_obj_clear_flag(fl, LV_OBJ_FLAG_SCROLLABLE);

    /* right foot */
    lv_obj_t* fr = lv_obj_create(c);
    lv_obj_remove_style_all(fr);
    lv_obj_set_size(fr, 14, 9);
    lv_obj_set_pos(fr, 60, 78);
    lv_obj_set_style_radius(fr, 5, 0);
    lv_obj_set_style_bg_color(fr, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(fr, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(fr, lv_color_black(), 0);
    lv_obj_set_style_border_width(fr, 3, 0);
    lv_obj_clear_flag(fr, LV_OBJ_FLAG_SCROLLABLE);

    /* head ellipse */
    lv_obj_t* head = lv_obj_create(c);
    lv_obj_remove_style_all(head);
    lv_obj_set_size(head, 74, 58);
    lv_obj_set_pos(head, 11, 8);
    lv_obj_set_style_radius(head, 29, 0);
    lv_obj_set_style_bg_color(head, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(head, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(head, lv_color_black(), 0);
    lv_obj_set_style_border_width(head, 4, 0);
    lv_obj_clear_flag(head, LV_OBJ_FLAG_SCROLLABLE);

    /* left ear */
    lv_obj_t* el = lv_obj_create(c);
    lv_obj_remove_style_all(el);
    lv_obj_set_size(el, 14, 14);
    lv_obj_set_pos(el, 16, 0);
    lv_obj_set_style_radius(el, 2, 0);
    lv_obj_set_style_bg_color(el, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(el, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(el, lv_color_black(), 0);
    lv_obj_set_style_border_width(el, 3, 0);
    lv_obj_set_style_transform_rotation(el, 450, 0);   /* 45 deg */
    lv_obj_clear_flag(el, LV_OBJ_FLAG_SCROLLABLE);

    /* right ear */
    lv_obj_t* er = lv_obj_create(c);
    lv_obj_remove_style_all(er);
    lv_obj_set_size(er, 14, 14);
    lv_obj_set_pos(er, 66, 0);
    lv_obj_set_style_radius(er, 2, 0);
    lv_obj_set_style_bg_color(er, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(er, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(er, lv_color_black(), 0);
    lv_obj_set_style_border_width(er, 3, 0);
    lv_obj_set_style_transform_rotation(er, -450, 0);  /* -45 deg */
    lv_obj_clear_flag(er, LV_OBJ_FLAG_SCROLLABLE);

    /* eyes */
    for (int i = 0; i < 2; i++) {
        lv_obj_t* eye = lv_obj_create(c);
        lv_obj_remove_style_all(eye);
        lv_obj_set_size(eye, 7, 7);
        lv_obj_set_pos(eye, 32 + i * 22, 34);
        lv_obj_set_style_radius(eye, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(eye, lv_color_black(), 0);
        lv_obj_set_style_bg_opa(eye, LV_OPA_COVER, 0);
        lv_obj_clear_flag(eye, LV_OBJ_FLAG_SCROLLABLE);
    }

    /* nose */
    lv_obj_t* nose = lv_obj_create(c);
    lv_obj_remove_style_all(nose);
    lv_obj_set_size(nose, 5, 4);
    lv_obj_set_pos(nose, 45, 45);
    lv_obj_set_style_radius(nose, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(nose, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(nose, LV_OPA_COVER, 0);
    lv_obj_clear_flag(nose, LV_OBJ_FLAG_SCROLLABLE);

    /* mouth */
    lv_obj_t* mouth = lv_line_create(c);
    static lv_point_precise_t pts[] = {{40, 52}, {45, 56}, {50, 52}};
    lv_line_set_points(mouth, pts, 3);
    lv_obj_set_style_line_color(mouth, lv_color_black(), 0);
    lv_obj_set_style_line_width(mouth, 2, 0);
    lv_obj_set_style_line_rounded(mouth, true, 0);
}

/* Simple thermometer person and droplet person, approximated with shapes. */
static void draw_thermometer(lv_obj_t* parent, int32_t x, int32_t y)
{
    lv_obj_t* c = lv_obj_create(parent);
    lv_obj_remove_style_all(c);
    lv_obj_set_pos(c, x, y);
    lv_obj_set_size(c, 40, 66);
    lv_obj_clear_flag(c, LV_OBJ_FLAG_SCROLLABLE);

    /* bulb */
    lv_obj_t* bulb = lv_obj_create(c);
    lv_obj_remove_style_all(bulb);
    lv_obj_set_size(bulb, 18, 18);
    lv_obj_set_pos(bulb, 11, 42);
    lv_obj_set_style_radius(bulb, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(bulb, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(bulb, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(bulb, lv_color_black(), 0);
    lv_obj_set_style_border_width(bulb, 3, 0);
    lv_obj_clear_flag(bulb, LV_OBJ_FLAG_SCROLLABLE);

    /* stem */
    lv_obj_t* stem = lv_obj_create(c);
    lv_obj_remove_style_all(stem);
    lv_obj_set_size(stem, 10, 32);
    lv_obj_set_pos(stem, 15, 12);
    lv_obj_set_style_radius(stem, 5, 0);
    lv_obj_set_style_bg_color(stem, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(stem, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(stem, lv_color_black(), 0);
    lv_obj_set_style_border_width(stem, 3, 0);
    lv_obj_clear_flag(stem, LV_OBJ_FLAG_SCROLLABLE);

    /* inner column */
    lv_obj_t* col = lv_obj_create(c);
    lv_obj_remove_style_all(col);
    lv_obj_set_size(col, 4, 22);
    lv_obj_set_pos(col, 18, 18);
    lv_obj_set_style_radius(col, 2, 0);
    lv_obj_set_style_bg_color(col, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(col, LV_OPA_COVER, 0);
    lv_obj_clear_flag(col, LV_OBJ_FLAG_SCROLLABLE);

    /* arms */
    lv_obj_t* arm_l = lv_line_create(c);
    static lv_point_precise_t al[] = {{11, 30}, {3, 22}};
    lv_line_set_points(arm_l, al, 2);
    lv_obj_set_style_line_color(arm_l, lv_color_black(), 0);
    lv_obj_set_style_line_width(arm_l, 3, 0);
    lv_obj_set_style_line_rounded(arm_l, true, 0);

    lv_obj_t* arm_r = lv_line_create(c);
    static lv_point_precise_t ar[] = {{29, 30}, {37, 22}};
    lv_line_set_points(arm_r, ar, 2);
    lv_obj_set_style_line_color(arm_r, lv_color_black(), 0);
    lv_obj_set_style_line_width(arm_r, 3, 0);
    lv_obj_set_style_line_rounded(arm_r, true, 0);
}

static void draw_droplet(lv_obj_t* parent, int32_t x, int32_t y)
{
    lv_obj_t* c = lv_obj_create(parent);
    lv_obj_remove_style_all(c);
    lv_obj_set_pos(c, x, y);
    lv_obj_set_size(c, 30, 50);
    lv_obj_clear_flag(c, LV_OBJ_FLAG_SCROLLABLE);

    /* teardrop body approximated as a circle with a triangle mask-line */
    lv_obj_t* drop = lv_obj_create(c);
    lv_obj_remove_style_all(drop);
    lv_obj_set_size(drop, 22, 28);
    lv_obj_set_pos(drop, 4, 12);
    lv_obj_set_style_radius(drop, 11, 0);
    lv_obj_set_style_bg_color(drop, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(drop, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(drop, lv_color_black(), 0);
    lv_obj_set_style_border_width(drop, 3, 0);
    lv_obj_clear_flag(drop, LV_OBJ_FLAG_SCROLLABLE);

    /* eyes */
    for (int i = 0; i < 2; i++) {
        lv_obj_t* e = lv_obj_create(c);
        lv_obj_remove_style_all(e);
        lv_obj_set_size(e, 3, 3);
        lv_obj_set_pos(e, 9 + i * 8, 22);
        lv_obj_set_style_radius(e, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(e, lv_color_black(), 0);
        lv_obj_set_style_bg_opa(e, LV_OPA_COVER, 0);
        lv_obj_clear_flag(e, LV_OBJ_FLAG_SCROLLABLE);
    }

    /* mouth */
    lv_obj_t* mouth = lv_line_create(c);
    static lv_point_precise_t pts[] = {{10, 30}, {15, 33}, {20, 30}};
    lv_line_set_points(mouth, pts, 3);
    lv_obj_set_style_line_color(mouth, lv_color_black(), 0);
    lv_obj_set_style_line_width(mouth, 2, 0);
    lv_obj_set_style_line_rounded(mouth, true, 0);
}

void kidpal_ui_create(lv_obj_t* parent)
{
    /* Load Chinese font via FreeType */
    if (!g_cjk_font) {
        g_cjk_font = lv_freetype_font_create(
            "C:/Windows/Fonts/msjh.ttc",
            LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
            16,
            LV_FREETYPE_FONT_STYLE_NORMAL);
    }

    /* White paper background */
    lv_obj_set_style_bg_color(parent, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);

    /* Default text font for this screen */
    if (g_cjk_font) {
        lv_obj_set_style_text_font(parent, g_cjk_font, 0);
    }

    /* Top date bar */
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

    make_label(parent, "8\u670827\u65e5 \u5468\u56db", 40, 10, NULL, LV_ALIGN_TOP_LEFT);

    /* Battery */
    lv_obj_t* batt = lv_obj_create(parent);
    lv_obj_remove_style_all(batt);
    lv_obj_set_size(batt, 32, 16);
    lv_obj_set_pos(batt, 356, 12);
    lv_obj_set_style_radius(batt, 4, 0);
    lv_obj_set_style_bg_color(batt, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(batt, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(batt, lv_color_black(), 0);
    lv_obj_set_style_border_width(batt, 3, 0);
    lv_obj_clear_flag(batt, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_t* batt_nip = lv_obj_create(parent);
    lv_obj_remove_style_all(batt_nip);
    lv_obj_set_size(batt_nip, 4, 7);
    lv_obj_set_pos(batt_nip, 388, 16);
    lv_obj_set_style_radius(batt_nip, 1, 0);
    lv_obj_set_style_bg_color(batt_nip, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(batt_nip, LV_OPA_COVER, 0);
    lv_obj_clear_flag(batt_nip, LV_OBJ_FLAG_SCROLLABLE);
    make_label(parent, "80%", 358, 28, LV_FONT_DEFAULT, LV_ALIGN_TOP_RIGHT);

    /* Big time */
    make_label(parent, "10", 16, 44, &lv_font_montserrat_48, LV_ALIGN_TOP_LEFT);

    /* Colon = two blinking eyes */
    g_colon_top = lv_obj_create(parent);
    lv_obj_remove_style_all(g_colon_top);
    lv_obj_set_size(g_colon_top, 19, 23);
    lv_obj_set_pos(g_colon_top, 94, 50);
    lv_obj_set_style_radius(g_colon_top, 8, 0);
    lv_obj_set_style_bg_color(g_colon_top, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(g_colon_top, LV_OPA_COVER, 0);
    lv_obj_clear_flag(g_colon_top, LV_OBJ_FLAG_SCROLLABLE);

    g_colon_btm = lv_obj_create(parent);
    lv_obj_remove_style_all(g_colon_btm);
    lv_obj_set_size(g_colon_btm, 19, 23);
    lv_obj_set_pos(g_colon_btm, 94, 78);
    lv_obj_set_style_radius(g_colon_btm, 8, 0);
    lv_obj_set_style_bg_color(g_colon_btm, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(g_colon_btm, LV_OPA_COVER, 0);
    lv_obj_clear_flag(g_colon_btm, LV_OBJ_FLAG_SCROLLABLE);

    lv_timer_create(colon_blink_timer, 4400, NULL);

    make_label(parent, "24", 118, 44, &lv_font_montserrat_48, LV_ALIGN_TOP_LEFT);

    /* Upcoming card */
    make_rounded_rect(parent, 268, 52, 122, 118, 14,
                      lv_color_white(), lv_color_black(), 3);
    make_pill(parent, "\u63a5\u4e0b\u6765", 282, 42,
              lv_color_black(), lv_color_white());
    make_label(parent, "14:00", 290, 78, &lv_font_montserrat_24, LV_ALIGN_TOP_LEFT);
    make_label(parent, "\u94a2\u7434\u8bfe", 290, 108, NULL, LV_ALIGN_TOP_LEFT);
    make_label(parent, "\u7434\u623f 2", 290, 132, NULL, LV_ALIGN_TOP_LEFT);

    /* Countdown sticker */
    make_pill(parent, "\u8fd8\u6709 3 \u4e2a\u534a\u5c0f\u65f6", 270, 178,
              lv_color_white(), lv_color_black());

    /* Dotted separator + tomorrow hint */
    lv_obj_t* line = lv_line_create(parent);
    static lv_point_precise_t line_pts[] = {{16, 182}, {212, 182}};
    lv_line_set_points(line, line_pts, 2);
    lv_obj_set_style_line_color(line, lv_color_black(), 0);
    lv_obj_set_style_line_width(line, 2, 0);
    lv_obj_set_style_line_dash_width(line, 2, 0);
    lv_obj_set_style_line_dash_gap(line, 4, 0);

    make_label(parent, "\u660e\u5929 09:00 \u00b7 \u6e38\u6cf3\u8bfe",
               110, 190, NULL, LV_ALIGN_TOP_LEFT);

    /* Thermometer + droplet + readings */
    draw_thermometer(parent, 118, 196);
    make_label(parent, "22\u00b0", 162, 206, &lv_font_montserrat_24, LV_ALIGN_TOP_LEFT);
    draw_droplet(parent, 216, 204);
    make_label(parent, "45%", 252, 212, &lv_font_montserrat_24, LV_ALIGN_TOP_LEFT);

    /* Mascot sitting on the bottom bar */
    draw_mascot(parent, 8, 172);

    /* Bottom key bar */
    make_rounded_rect(parent, 0, 262, 400, 38, 0,
                      lv_color_white(), lv_color_black(), 3);
    make_pill(parent, "KEY", 10, 268,
              lv_color_white(), lv_color_black());
    make_label(parent, "\u6309\u4e00\u4e0b\u8bf4\u8bdd", 56, 270, NULL, LV_ALIGN_TOP_LEFT);

    /* Page dots */
    for (int i = 0; i < 4; i++) {
        lv_obj_t* dot = lv_obj_create(parent);
        lv_obj_remove_style_all(dot);
        lv_obj_set_size(dot, i == 0 ? 11 : 9, i == 0 ? 11 : 9);
        lv_obj_set_pos(dot, 188 + i * 14, 274);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(dot, i == 0 ? lv_color_black() : lv_color_white(), 0);
        lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
        lv_obj_set_style_border_color(dot, lv_color_black(), 0);
        lv_obj_set_style_border_width(dot, 2, 0);
        lv_obj_clear_flag(dot, LV_OBJ_FLAG_SCROLLABLE);
    }

    make_pill(parent, "BOOT", 342, 268,
              lv_color_white(), lv_color_black());
    make_label(parent, "\u7ffb\u9875", 388, 270, NULL, LV_ALIGN_TOP_RIGHT);
}
