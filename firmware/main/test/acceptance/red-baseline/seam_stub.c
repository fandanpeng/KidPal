/*
 * seam_stub.c — minimal host_infra render-seam STAND-IN for the HP1 red-baseline
 * standalone scaffold (see unity.h header note; NOT a frozen file, NOT the gate
 * runner).
 *
 * The gate runner uses host_app/main/host_infra_seam.c (real LVGL offscreen
 * rendering, package 0). That file needs LVGL, which the bare gcc scaffold does
 * not build — so this stub provides the two seam symbols with a trivial white
 * framebuffer, purely so the SAME frozen acceptance file links and EXECUTES.
 * The render TEST_CASEs fail at the kidpal_home_p1_render_frame weak sentinel
 * long before any pixel probe, so the stub's contents never mask a red.
 */
#include <stdint.h>
#include <string.h>

#define STUB_W 400
#define STUB_H 300

static uint32_t s_fb[STUB_W * STUB_H];
static int s_rendered = 0;

int host_infra_render_frame(void)
{
    uint32_t white = 0x00FFFFFFu;
    for (size_t i = 0; i < (size_t)(STUB_W * STUB_H); i++) {
        s_fb[i] = white;
    }
    s_rendered = 1;
    return 0;
}

int32_t host_infra_read_pixel(int x, int y)
{
    if (!s_rendered) {
        return -1;
    }
    if (x < 0 || y < 0 || x >= STUB_W || y >= STUB_H) {
        return -1;
    }
    return (int32_t)s_fb[(size_t)y * STUB_W + (size_t)x];
}
