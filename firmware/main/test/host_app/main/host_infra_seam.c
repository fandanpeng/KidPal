/*
 * host_infra_seam.c — LVGL host render seam for the linux-target host test app.
 *
 * Provides the two symbols the frozen acceptance contract pins:
 *   int     host_infra_render_frame(void);
 *   int32_t host_infra_read_pixel(int x, int y);
 *
 * Backend choice: pure LVGL software renderer drawing into an offscreen ARGB8888
 * draw buffer (no window, no SDL/X11). Renderer is LVGL's own software renderer.
 * This is genuinely HEADLESS — suitable for CI — while still producing a real
 * solid 0x336699 frame that the seam probes back. "Rendered" state is tracked so
 * read_pixel() returns the HOST_INFRA_NOT_RENDERED (-1) sentinel until a frame is
 * actually flushed.
 *
 * Headers (LVGL 9): the LV component provides "lvgl.h". Color depth is 32
 * (ARGB8888), selected via CONFIG_LV_COLOR_DEPTH_32=y in host_app/sdkconfig.defaults,
 * so each flushed pixel is 4 bytes [B,G,R,A] (little-endian); we mask off alpha -> 0xRRGGBB.
 */
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include "lvgl.h"

/* Must match the acceptance file's HOST_INFRA_FRAME_W/H and color. */
#define SEAM_FRAME_W 400
#define SEAM_FRAME_H 300
#define SEAM_NOT_RENDERED ((int32_t)-1)

/* One ARGB8888 pixel per cell, final value masked to 0xRRGGBB. */
static uint32_t s_fb[SEAM_FRAME_W * SEAM_FRAME_H];
static bool     s_rendered   = false;
static bool     s_init_done  = false;
static lv_display_t *s_disp  = NULL;

static uint32_t seam_ms(void)
{
    /* Monotonic millisecond source for LVGL's tick callback (lv_tick_set_cb expects
     * `uint32_t (*)(void)`). We only ever force one synchronous refresh, so this just
     * keeps any LVGL timer/animation bookkeeping well-defined. */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)((uint64_t)tv.tv_sec * 1000u + (uint64_t)tv.tv_usec / 1000u);
}

static void seam_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t x0 = (uint32_t)(area->x1 < 0 ? 0 : area->x1);
    uint32_t y0 = (uint32_t)(area->y1 < 0 ? 0 : area->y1);
    uint32_t w  = (uint32_t)(area->x2 - area->x1 + 1);
    uint32_t h  = (uint32_t)(area->y2 - area->y1 + 1);

    /* ARGB8888 -> 4 bytes/pixel. Pitch in pixels matches the draw buffer stride. */
    const uint32_t pitch_px = (uint32_t)SEAM_FRAME_W; /* full-frame single buffer, no packing gap */
    const uint32_t *src = (const uint32_t *)px_map;

    for (uint32_t row = 0; row < h; row++) {
        for (uint32_t col = 0; col < w; col++) {
            uint32_t fx = x0 + col;
            uint32_t fy = y0 + row;
            if (fx < SEAM_FRAME_W && fy < SEAM_FRAME_H) {
                s_fb[fy * SEAM_FRAME_W + fx] = src[col] & 0x00FFFFFFu; /* drop alpha */
            }
        }
        src += pitch_px;
    }
    lv_display_flush_ready(disp);
}

static void seam_init(void)
{
    lv_init();
    lv_tick_set_cb(seam_ms);

    s_disp = lv_display_create(SEAM_FRAME_W, SEAM_FRAME_H);
    assert(s_disp != NULL);

    lv_draw_buf_t *buf = lv_draw_buf_create(SEAM_FRAME_W, SEAM_FRAME_H,
                                            LV_COLOR_FORMAT_ARGB8888,
                                            LV_DRAW_BUF_STRIDE(SEAM_FRAME_W, LV_COLOR_FORMAT_ARGB8888));
    assert(buf != NULL);
    lv_display_set_draw_buffers(s_disp, buf, NULL);

    lv_display_set_flush_cb(s_disp, seam_flush_cb);
    lv_display_set_flush_wait_cb(s_disp, NULL);
}

int host_infra_render_frame(void)
{
    if (!s_init_done) {
        seam_init();
        s_init_done = true;
    }

    /* Start from a blank frame so any unflushed region is not reported stale. */
    memset(s_fb, 0, sizeof(s_fb));
    s_rendered = false;

    /* Full-screen opaque solid fill object -> the whole frame flushes to 0x336699. */
    lv_obj_t *fill = lv_obj_create(lv_screen_active());
    assert(fill != NULL);
    lv_obj_remove_style_all(fill);
    lv_obj_set_size(fill, SEAM_FRAME_W, SEAM_FRAME_H);
    lv_obj_set_pos(fill, 0, 0);
    lv_obj_set_style_bg_color(fill, lv_color_hex(0x336699u), 0);
    lv_obj_set_style_bg_opa(fill, LV_OPA_COVER, 0);

    /* Force an immediate, synchronous refresh: LVGL runs the software renderer and
     * calls seam_flush_cb, which copies the framebuffer into s_fb. */
    lv_refr_now(s_disp);
    lv_obj_delete(fill);

    s_rendered = true;
    return 0; /* 0 == a frame was flushed */
}

int32_t host_infra_read_pixel(int x, int y)
{
    if (!s_rendered) {
        return SEAM_NOT_RENDERED;
    }
    if (x < 0 || y < 0 || x >= SEAM_FRAME_W || y >= SEAM_FRAME_H) {
        return SEAM_NOT_RENDERED;
    }
    return (int32_t)s_fb[(uint32_t)y * SEAM_FRAME_W + (uint32_t)x];
}

/* ============================================================================
 * additive P1 渲染入口 (TaskPackage 260829-home-p1) —— 不改任何既有断言行为。
 *
 * home_p1.c (纯逻辑, device 亦可编) 以自身 weak 默认定义同名符号; 本文件以强符号
 * 覆盖之, 把 home_p1 构建好的 ARGB8888 一帧打印到可探针帧缓冲 s_fb 并置 rendered,
 * 使 host_infra_read_pixel 能探针 P1 首页版面 (布局钉区域墨水在位)。
 * 包 0 冻结断言 (host_infra_smoke.acceptance.c) 只关住 host_infra_render_frame /
 * host_infra_read_pixel 自身——本新增函数不影响其语义, 每次渲染都是整帧覆盖。
 * ========================================================================== */
int host_infra_render_p1_frame(const uint32_t *argb, int w, int h)
{
    if (argb == NULL || w != SEAM_FRAME_W || h != SEAM_FRAME_H) {
        return -1;
    }
    for (int i = 0; i < SEAM_FRAME_W * SEAM_FRAME_H; i++) {
        s_fb[i] = argb[i] & 0x00FFFFFFu; /* drop alpha, 与 seam_flush_cb 同口径 */
    }
    s_rendered = true;
    return 0; /* 0 == a P1 frame was flushed */
}