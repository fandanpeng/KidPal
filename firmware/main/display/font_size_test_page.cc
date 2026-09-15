/*
 * font_size_test_page.cc — NFR-READ-1 实验页（ARQ-260831-small-cjk-1bpp-readability）：
 * 单页列出 10–32px 共 9 档 1bpp NotoSansSC 字号的真机可读性对照。
 *
 * 字体 = tools/asset-pipeline 的 lv_font_conv 产线同源参数（NotoSansSC-Regular +
 * --bpp 1 --no-compress，C 数组直编进固件），与 font_body.bin（14px 1bpp）同管线，
 * 板端 flush 回调本就逐像素二值化（RGB565 < 0x7fff → 黑/白），1bpp 字体渲染
 * 所见即所得。
 *
 * 页面挂 lv_layer_top() 顶层、开机只画一次，浮在正常 UI 之上不被覆盖；仅供
 * human 真机目测字号下限，看完烧回正常固件即可。实验性构建专用，不入产线。
 */

#include "font_size_test_page.h"

#include <lvgl.h>

#include <cstdio>

LV_FONT_DECLARE(font_test_10);
LV_FONT_DECLARE(font_test_12);
LV_FONT_DECLARE(font_test_14);
LV_FONT_DECLARE(font_test_16);
LV_FONT_DECLARE(font_test_18);
LV_FONT_DECLARE(font_test_20);
LV_FONT_DECLARE(font_test_24);
LV_FONT_DECLARE(font_test_28);
LV_FONT_DECLARE(font_test_32);

void font_size_test_page_show(void)
{
    static const lv_font_t *fonts[] = {
        &font_test_10, &font_test_12, &font_test_14, &font_test_16, &font_test_18,
        &font_test_20, &font_test_24, &font_test_28, &font_test_32,
    };
    static const int sizes[] = { 10, 12, 14, 16, 18, 20, 24, 28, 32 };
    const int n = (int)(sizeof(sizes) / sizeof(sizes[0]));

    /* 顶层整屏白底容器：浮在正常 UI（状态栏/聊天层）之上，不被后续绘制覆盖 */
    lv_obj_t *layer = lv_layer_top();
    lv_obj_clean(layer);
    lv_obj_set_size(layer, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(layer, lv_color_make(0xff, 0xff, 0xff), 0);
    lv_obj_set_style_bg_opa(layer, LV_OPA_COVER, 0);

    lv_coord_t y = 2;
    for (int i = 0; i < n; i++) {
        lv_obj_t *label = lv_label_create(layer);
        char buf[64];
        snprintf(buf, sizeof(buf), "%dpx 永国藏酬囊舞攀赢魔", sizes[i]);
        lv_label_set_text(label, buf);
        lv_obj_set_style_text_font(label, fonts[i], 0);
        lv_obj_set_style_text_color(label, lv_color_make(0x00, 0x00, 0x00), 0);
        lv_obj_set_pos(label, 2, y);
        y += fonts[i]->line_height + 2;
    }

    lv_obj_invalidate(layer);
}
