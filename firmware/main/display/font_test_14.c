/*******************************************************************************
 * Size: 14 px
 * Bpp: 1
 * Opts: --font ../../firmware/main/test/host_app/managed_components/lvgl__lvgl/tests/src/test_files/fonts/noto/NotoSansSC-Regular.ttf --size 14 --bpp 1 --format lvgl --no-compress --lv-font-name font_test_14 --symbols 永国藏酬囊舞攀赢魔小伴勇课表故事写0123456789px -o ../../firmware/main/display/font_test_14.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef FONT_TEST_14
#define FONT_TEST_14 1
#endif

#if FONT_TEST_14

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0030 "0" */
    0x31, 0x28, 0x61, 0x86, 0x18, 0x61, 0x85, 0x23,
    0x0,

    /* U+0031 "1" */
    0x63, 0x82, 0x8, 0x20, 0x82, 0x8, 0x20, 0x8f,
    0xc0,

    /* U+0032 "2" */
    0x7b, 0x30, 0x41, 0x4, 0x20, 0x84, 0x21, 0x8f,
    0xc0,

    /* U+0033 "3" */
    0x78, 0x98, 0x10, 0x20, 0xc7, 0x1, 0x1, 0x3,
    0x8d, 0xf0,

    /* U+0034 "4" */
    0xc, 0x18, 0x50, 0xa2, 0x4c, 0x91, 0x7f, 0x4,
    0x8, 0x10,

    /* U+0035 "5" */
    0x7c, 0x81, 0x2, 0x7, 0xc8, 0xc0, 0x81, 0x3,
    0x89, 0xe0,

    /* U+0036 "6" */
    0x39, 0xc, 0x20, 0xbb, 0x38, 0x61, 0x85, 0x33,
    0x80,

    /* U+0037 "7" */
    0xfc, 0x20, 0x84, 0x10, 0x42, 0x8, 0x20, 0x82,
    0x0,

    /* U+0038 "8" */
    0x39, 0x14, 0x51, 0x28, 0xe4, 0xe1, 0x87, 0x37,
    0x80,

    /* U+0039 "9" */
    0x73, 0x28, 0x61, 0x87, 0x37, 0x41, 0x8, 0x27,
    0x0,

    /* U+0070 "p" */
    0xbb, 0x38, 0x61, 0x86, 0x18, 0xbc, 0x82, 0x8,
    0x0,

    /* U+0078 "x" */
    0x44, 0xd8, 0xa0, 0xc3, 0x85, 0x13, 0x62,

    /* U+4E8B "事" */
    0x2, 0x3, 0xff, 0xc0, 0x80, 0x7f, 0xc2, 0x22,
    0xf, 0xe0, 0xff, 0x80, 0x44, 0xff, 0xf8, 0x11,
    0x1f, 0xf8, 0x4, 0x40, 0xe0, 0x0,

    /* U+4F34 "伴" */
    0x18, 0x80, 0xa4, 0x8d, 0x24, 0x45, 0x46, 0x8,
    0x73, 0xfe, 0x82, 0x4, 0x10, 0x2f, 0xf9, 0x4,
    0x8, 0x20, 0x41, 0x2, 0x8, 0x0,

    /* U+5199 "写" */
    0xff, 0xf9, 0x1, 0x90, 0x11, 0xfc, 0x10, 0x3,
    0x0, 0x3f, 0xe0, 0x6, 0xff, 0x40, 0x4, 0x0,
    0x40, 0x3c,

    /* U+52C7 "勇" */
    0xff, 0xe3, 0x70, 0x1c, 0x7e, 0x7f, 0xff, 0x88,
    0x7f, 0xfe, 0x21, 0x7f, 0xe1, 0x4, 0xc0, 0xf0,
    0xe0,

    /* U+56CA "囊" */
    0x2, 0x7, 0xff, 0x9f, 0xf8, 0xff, 0xcf, 0xff,
    0x7e, 0xf9, 0xf7, 0x8f, 0xfe, 0x7f, 0xf7, 0xff,
    0xc3, 0xc1, 0xe3, 0xc3, 0xc7, 0x80,

    /* U+56FD "国" */
    0xff, 0xfe, 0x0, 0xf7, 0xf7, 0x84, 0x3c, 0x21,
    0xef, 0xef, 0xa, 0x78, 0x4b, 0xdf, 0xde, 0x0,
    0xff, 0xff, 0x80, 0x30,

    /* U+5C0F "小" */
    0x2, 0x0, 0x10, 0x0, 0x80, 0x44, 0x2, 0x22,
    0x11, 0x11, 0x88, 0x48, 0x42, 0x42, 0x1c, 0x10,
    0x40, 0x80, 0x4, 0x0, 0xe0, 0x0,

    /* U+6500 "攀" */
    0x11, 0x20, 0x99, 0x1e, 0xde, 0x60, 0x65, 0xb6,
    0xeb, 0x94, 0x10, 0x1f, 0x7f, 0x3f, 0x36, 0xff,
    0x4f, 0xfc, 0x2, 0x0, 0x60, 0x0,

    /* U+6545 "故" */
    0x10, 0x80, 0x88, 0x4, 0x41, 0xff, 0xf1, 0x31,
    0x9, 0x8b, 0xfc, 0x98, 0x94, 0xc4, 0xa6, 0x26,
    0x31, 0x31, 0xfb, 0x4c, 0x31, 0x80, 0x0,

    /* U+6C38 "永" */
    0xc, 0x0, 0x38, 0x0, 0x20, 0x7e, 0x0, 0x30,
    0x81, 0x8b, 0xec, 0x81, 0x78, 0x13, 0xc1, 0x9b,
    0x18, 0xcd, 0x86, 0x30, 0xe0, 0x0,

    /* U+821E "舞" */
    0x20, 0x2, 0x0, 0x7f, 0xea, 0x94, 0x7f, 0xf2,
    0x94, 0xff, 0xf2, 0x4, 0x7b, 0xf8, 0xa4, 0x33,
    0xf6, 0x4, 0xc0, 0x40,

    /* U+85CF "藏" */
    0x8, 0x41, 0xff, 0xe0, 0x85, 0x0, 0x12, 0x5f,
    0xf9, 0x5f, 0x7, 0x65, 0x5, 0xf4, 0xf4, 0xb1,
    0x5c, 0x85, 0x7a, 0x55, 0x1e, 0xa0, 0x98,

    /* U+8868 "表" */
    0x2, 0x3, 0xff, 0x80, 0x80, 0x4, 0x3, 0xff,
    0x1, 0x3, 0xff, 0xe0, 0xa0, 0x19, 0x33, 0x87,
    0x34, 0x30, 0x24, 0xc1, 0xc1, 0x80, 0x0,

    /* U+8BFE "课" */
    0x67, 0xf1, 0xa4, 0x81, 0xfc, 0x9, 0x2e, 0x49,
    0x11, 0xf0, 0xbf, 0xe4, 0x38, 0x2a, 0xa1, 0xb5,
    0x9b, 0x26, 0x1, 0x0,

    /* U+8D62 "赢" */
    0x2, 0x7, 0xff, 0x88, 0x0, 0x3f, 0xc3, 0xfe,
    0x10, 0x10, 0x7f, 0xd, 0xfc, 0x69, 0xa3, 0x6f,
    0x3b, 0x79, 0xcd, 0x5a, 0x91, 0x0,

    /* U+916C "酬" */
    0x2, 0x1f, 0xe5, 0x52, 0x5f, 0xa5, 0xfa, 0x5d,
    0xe7, 0xde, 0xfd, 0xef, 0x8a, 0x5f, 0xa5, 0x8a,
    0x5f, 0xc5, 0x88, 0x50,

    /* U+9B54 "魔" */
    0x1, 0x1, 0xff, 0xf6, 0x42, 0x1f, 0xfe, 0x6e,
    0x71, 0xd2, 0xb6, 0xff, 0x1a, 0x44, 0x6f, 0xf1,
    0xbf, 0xc6, 0x2b, 0x11, 0xb9, 0xd8, 0xfc
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 124, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 9, .adv_w = 124, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 18, .adv_w = 124, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 27, .adv_w = 124, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 37, .adv_w = 124, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 47, .adv_w = 124, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 57, .adv_w = 124, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 66, .adv_w = 124, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 75, .adv_w = 124, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 84, .adv_w = 124, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 93, .adv_w = 139, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 102, .adv_w = 112, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 109, .adv_w = 224, .box_w = 13, .box_h = 13, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 131, .adv_w = 224, .box_w = 13, .box_h = 13, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 153, .adv_w = 224, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 171, .adv_w = 224, .box_w = 11, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 188, .adv_w = 224, .box_w = 13, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 210, .adv_w = 224, .box_w = 13, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 230, .adv_w = 224, .box_w = 13, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 252, .adv_w = 224, .box_w = 13, .box_h = 13, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 274, .adv_w = 224, .box_w = 13, .box_h = 14, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 297, .adv_w = 224, .box_w = 13, .box_h = 13, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 319, .adv_w = 224, .box_w = 12, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 339, .adv_w = 224, .box_w = 14, .box_h = 13, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 362, .adv_w = 224, .box_w = 13, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 385, .adv_w = 224, .box_w = 13, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 405, .adv_w = 224, .box_w = 13, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 427, .adv_w = 224, .box_w = 12, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 447, .adv_w = 224, .box_w = 14, .box_h = 13, .ofs_x = 0, .ofs_y = -1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_1[] = {
    0x0, 0x8, 0x4e1b, 0x4ec4, 0x5129, 0x5257, 0x565a, 0x568d,
    0x5b9f, 0x6490, 0x64d5, 0x6bc8, 0x81ae, 0x855f, 0x87f8, 0x8b8e,
    0x8cf2, 0x90fc, 0x9ae4
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 48, .range_length = 10, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 112, .range_length = 39653, .glyph_id_start = 11,
        .unicode_list = unicode_list_1, .glyph_id_ofs_list = NULL, .list_length = 19, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    11, 12
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -5
};

/*Collect the kern pair's data in one place*/
static const lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 1,
    .glyph_ids_size = 0
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_pairs,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t font_test_14 = {
#else
lv_font_t font_test_14 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 15,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if FONT_TEST_14*/

