/*******************************************************************************
 * Size: 10 px
 * Bpp: 1
 * Opts: --font ../../firmware/main/test/host_app/managed_components/lvgl__lvgl/tests/src/test_files/fonts/noto/NotoSansSC-Regular.ttf --size 10 --bpp 1 --format lvgl --no-compress --lv-font-name font_test_10 --symbols 永国藏酬囊舞攀赢魔小伴勇课表故事写0123456789px -o ../../firmware/main/display/font_test_10.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef FONT_TEST_10
#define FONT_TEST_10 1
#endif

#if FONT_TEST_10

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0030 "0" */
    0x76, 0xe3, 0x18, 0xc7, 0x6e,

    /* U+0031 "1" */
    0x62, 0x22, 0x22, 0x2f,

    /* U+0032 "2" */
    0x70, 0x42, 0x11, 0x11, 0x1f,

    /* U+0033 "3" */
    0x70, 0x42, 0x60, 0x84, 0x2e,

    /* U+0034 "4" */
    0x11, 0x94, 0xa9, 0x7c, 0x42,

    /* U+0035 "5" */
    0x7a, 0x10, 0xe0, 0x84, 0x2e,

    /* U+0036 "6" */
    0x72, 0x21, 0x6c, 0xc7, 0x2e,

    /* U+0037 "7" */
    0xf8, 0x44, 0x22, 0x10, 0x84,

    /* U+0038 "8" */
    0x69, 0x96, 0xa9, 0x96,

    /* U+0039 "9" */
    0x74, 0xe3, 0x17, 0x84, 0x4e,

    /* U+0070 "p" */
    0xb6, 0x63, 0x19, 0xfa, 0x10,

    /* U+0078 "x" */
    0x52, 0x88, 0x45, 0x44,

    /* U+4E8B "事" */
    0xff, 0xbf, 0x92, 0x47, 0xc7, 0xf7, 0xfd, 0xfc,
    0x10, 0x18, 0x0,

    /* U+4F34 "伴" */
    0x22, 0xa, 0xa2, 0xa1, 0xbf, 0x22, 0xf, 0xf2,
    0x20, 0x88, 0x22, 0x0,

    /* U+5199 "写" */
    0xff, 0xa1, 0x3e, 0x40, 0x7e, 0x2, 0xfe, 0x2,
    0xc,

    /* U+52C7 "勇" */
    0xfe, 0x38, 0xff, 0xff, 0xff, 0x91, 0xff, 0x21,
    0xc6,

    /* U+56CA "囊" */
    0x7f, 0x92, 0x4f, 0xfa, 0xdc, 0x37, 0xa, 0x87,
    0xf8, 0x52, 0x7b, 0x80,

    /* U+56FD "国" */
    0xff, 0x81, 0xbf, 0x89, 0xbf, 0x8b, 0xbf, 0xff,
    0x81,

    /* U+5C0F "小" */
    0x8, 0x4, 0x12, 0x49, 0x24, 0x8c, 0x46, 0x22,
    0x10, 0x38, 0x0,

    /* U+6500 "攀" */
    0x7f, 0xb2, 0xab, 0xe0, 0xf, 0xfb, 0xea, 0xfe,
    0xff, 0x18, 0x0,

    /* U+6545 "故" */
    0x22, 0x3e, 0xf2, 0x48, 0x92, 0x7b, 0x12, 0xc4,
    0x91, 0xec, 0x44, 0xc0,

    /* U+6C38 "永" */
    0x18, 0x1, 0x83, 0x80, 0x12, 0xf5, 0x6, 0x82,
    0xa1, 0xa4, 0xc8, 0xc6, 0x0,

    /* U+821E "舞" */
    0x3f, 0x9a, 0xc7, 0xf8, 0xac, 0x74, 0x8f, 0xed,
    0x50, 0x9f, 0x41, 0x0,

    /* U+85CF "藏" */
    0x7f, 0xc2, 0x29, 0x3e, 0x37, 0xa6, 0x95, 0xdf,
    0x1b, 0x23, 0x7d, 0xaa, 0x40,

    /* U+8868 "表" */
    0xff, 0x84, 0x1f, 0xdf, 0xf1, 0x83, 0x2a, 0x88,
    0x7b, 0x20, 0x0,

    /* U+8BFE "课" */
    0x9f, 0xb2, 0x4f, 0xfb, 0xf4, 0x43, 0xfd, 0x38,
    0xaa, 0x24, 0x0,

    /* U+8D62 "赢" */
    0x8, 0x3f, 0xf3, 0xf8, 0xfe, 0x3f, 0x9f, 0xe7,
    0xf9, 0xfe, 0x76, 0x6d, 0x10,

    /* U+916C "酬" */
    0xf2, 0xb5, 0x7e, 0xbf, 0xff, 0xfc, 0xd7, 0xeb,
    0xf5, 0x9a, 0x80,

    /* U+9B54 "魔" */
    0x8, 0x1d, 0xe7, 0xbd, 0x6e, 0x44, 0x1f, 0xef,
    0xfb, 0xfe, 0x8e, 0xbd, 0xf0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 89, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 5, .adv_w = 89, .box_w = 4, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 9, .adv_w = 89, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 14, .adv_w = 89, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 19, .adv_w = 89, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 24, .adv_w = 89, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 29, .adv_w = 89, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 34, .adv_w = 89, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 39, .adv_w = 89, .box_w = 4, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 43, .adv_w = 89, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 48, .adv_w = 99, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 53, .adv_w = 80, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 57, .adv_w = 160, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 68, .adv_w = 160, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 80, .adv_w = 160, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 89, .adv_w = 160, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 98, .adv_w = 160, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 110, .adv_w = 160, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 119, .adv_w = 160, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 130, .adv_w = 160, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 141, .adv_w = 160, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 153, .adv_w = 160, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 166, .adv_w = 160, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 178, .adv_w = 160, .box_w = 11, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 191, .adv_w = 160, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 202, .adv_w = 160, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 213, .adv_w = 160, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 226, .adv_w = 160, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 237, .adv_w = 160, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = -1}
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
    -4
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
const lv_font_t font_test_10 = {
#else
lv_font_t font_test_10 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 11,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if FONT_TEST_10*/

