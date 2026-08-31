/*
 * KidPal 1bpp 阈值化快照工具（正式版实现）。
 *
 * Owner: tools/simulator（dev-loop, TaskPackage 260831-simulator-lvgl-align）。
 * 背景: 模拟器是 32bit 真彩软件渲染, 灰度抗锯齿让细线/小字在模拟器上显得比真机可用;
 *       真机是 1bpp 纯黑白。本工具把「整屏快照 → 亮度阈值化 → 2 级 PNG 落盘」做成一键自查,
 *       阈值与真机 flush 后处理同一来源, 使「模拟器验证结论 ≈ 真机结论」重新成立。
 */
#include "kidpal_snapshot.h"
#include "lvgl/lvgl.h"
#include "lvgl/src/libs/lodepng/lodepng.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * 阈值单一源: LV_DRAW_SW_I1_LUM_THRESHOLD 经 LV_CONF_INCLUDE_SIMPLE 由 lv_conf.h(127) 传入,
 * 与 firmware/sdkconfig 的 CONFIG_LV_DRAW_SW_I1_LUM_THRESHOLD=127 同值。
 * 真机 flush 就是把 RGB565 用同一亮度阈值转 1bpp 推屏(见 lvgl src/draw/sw/blend/
 * lv_draw_sw_blend_to_i1.c, 其 I1_LUM_THRESHOLD == LV_DRAW_SW_I1_LUM_THRESHOLD)。
 * 快照与真机后处理用同一阈值, 阈值化语义一致: luminance < 阈值 → 黑(墨水), 否则白。
 * 注意: 与 tools/asset-pipeline/gen_assets.mjs 的 LUM_THRESHOLD=127 同值, 三处不得漂移。
 */
#define KIDPAL_SNAPSHOT_LUM_THRESHOLD LV_DRAW_SW_I1_LUM_THRESHOLD

/* RGB565 → 8bit 亮度(Rec.601 近似), 与真机 flush 的 I1 阈值化同口径。 */
static uint8_t rgb565_luma(uint16_t c)
{
    uint32_t r = (c >> 11) & 0x1F;
    uint32_t g = (c >> 5) & 0x3F;
    uint32_t b = c & 0x1F;
    r = (r << 3) | (r >> 2);   /* 5bit -> 8bit */
    g = (g << 2) | (g >> 4);   /* 6bit -> 8bit */
    b = (b << 3) | (b >> 2);   /* 5bit -> 8bit */
    return (uint8_t)((r * 77u + g * 150u + b * 29u) >> 8);
}

void kidpal_snapshot_capture_and_quit(void)
{
    lv_obj_t * scr = lv_screen_active();
    /* RGB565: 与真机显示 flush 同格式, 阈值化语义对齐; 避免 ARGB8888 字节序歧义。 */
    lv_draw_buf_t * snap = lv_snapshot_take(scr, LV_COLOR_FORMAT_RGB565);
    if(snap == NULL) {
        fprintf(stderr, "kidpal_snapshot: lv_snapshot_take failed\n");
        exit(1);
        return;
    }

    uint32_t w = snap->header.w;
    uint32_t h = snap->header.h;
    uint32_t stride = snap->header.stride;
    const uint8_t * src = snap->data;

    /* 逐像素两级输出: 每像素 RGB 三通道同值且 ∈ {0,255}。 */
    unsigned char * rgb = (unsigned char *)malloc(w * h * 3);
    if(rgb == NULL) {
        lv_draw_buf_destroy(snap);
        exit(1);
        return;
    }

    for(uint32_t y = 0; y < h; y++) {
        const uint8_t * row = src + (size_t)y * stride;
        unsigned char * dst = rgb + (size_t)y * w * 3;
        for(uint32_t x = 0; x < w; x++) {
            uint16_t c = (uint16_t)(row[x * 2] | (row[x * 2 + 1] << 8));
            uint8_t pix = (rgb565_luma(c) < KIDPAL_SNAPSHOT_LUM_THRESHOLD) ? 0u : 255u;
            dst[x * 3] = pix;
            dst[x * 3 + 1] = pix;
            dst[x * 3 + 2] = pix;
        }
    }

    /* 落盘路径: KIDPAL_SNAPSHOT_PATH 覆盖(脚本化回归用); 默认工作目录 kidpal_snapshot.png。
     * 不再散落 C:/tmp/... 实验路径。 */
    const char * out_path = getenv("KIDPAL_SNAPSHOT_PATH");
    if(out_path == NULL || out_path[0] == '\0') {
        out_path = "kidpal_snapshot.png";
    }

    unsigned char * png = NULL;
    size_t png_size = 0;
    unsigned err = lodepng_encode24(&png, &png_size, rgb, w, h);
    free(rgb);
    lv_draw_buf_destroy(snap);

    if(err != 0) {
        fprintf(stderr, "kidpal_snapshot: lodepng_encode24 error %u\n", err);
        exit(1);
        return;
    }

    FILE * f = fopen(out_path, "wb");
    if(f == NULL) {
        fprintf(stderr, "kidpal_snapshot: cannot open %s\n", out_path);
        free(png);
        exit(1);
        return;
    }
    fwrite(png, 1, png_size, f);
    fclose(f);
    free(png);

    fprintf(stderr, "kidpal_snapshot: wrote %s (%ux%u, thresholded 2-level)\n", out_path, w, h);

    /* 截完即退(原实验版 semantics = PostQuitMessage(0))。但本 port 的 main 是
     * `while(1){ lv_timer_handler(); lv_delay_ms(); }` 不泵 Win32 消息, 单纯
     * PostQuitMessage 不会让进程退出(真正常 exit 点在 lv_windows_context.c 的
     * display-existence timer)。故用 exit(0) 确保脚本化回归不挂起——语义不变: 截完即退。 */
    exit(0);
}

static void kidpal_snapshot_timer_cb(lv_timer_t * timer)
{
    (void)timer;
    kidpal_snapshot_capture_and_quit();
}

void kidpal_snapshot_schedule(void)
{
    /* 1500ms 一次性: 等首帧渲染/字体加载 settle 后再截。 */
    lv_timer_t * t = lv_timer_create(kidpal_snapshot_timer_cb, 1500, NULL);
    lv_timer_set_repeat_count(t, 1);
}