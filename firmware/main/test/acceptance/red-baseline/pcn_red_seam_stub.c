/*
 * pcn_red_seam_stub.c — P1CJK (TaskPackage 260921-m3-k4-p1-cjk-dynamic-text)
 * red-baseline standalone scaffold STAND-IN (see unity.h header note; NOT a frozen
 * file, NOT the gate runner).
 *
 * The gate runner uses host_app/main/host_infra_seam.c (real LVGL offscreen
 * rendering). That file needs LVGL, which the bare gcc scaffold does not build —
 * this stub provides:
 *   - strong host_infra_render_p1_frame / host_infra_read_pixel with a trivial
 *     framebuffer, so the in-tree home_p1.c REALLY renders and the red lands on
 *     the glyph seam (kidpal_p1_font_glyph weak sentinel = unavailable -> name
 *     never drawn), not on a missing render seam;
 *   - kidpal_art_* primitives (art not mounted -> home_p1 fallback-stroke paths,
 *     same as the gate runner where art_assets.c is intentionally not wired).
 * Nothing here is compiled into the host runner (builder wires only the frozen
 * acceptance file per BUILDER DUTY 4).
 */
#include <stdint.h>
#include <string.h>
/* art 契约头（仅类型；本桩恒报未挂载）。 */
#include "../../../display/art_assets.h"

#define PCN_STUB_W 400
#define PCN_STUB_H 300

static uint32_t s_fb[PCN_STUB_W * PCN_STUB_H];
static int s_rendered = 0;

/* home_p1.c declares this as its own weak default; this strong override plays the
 * role of host_app/main/host_infra_seam.c in the standalone scaffold. */
int host_infra_render_p1_frame(const uint32_t *argb, int w, int h)
{
    if (argb == NULL || w != PCN_STUB_W || h != PCN_STUB_H) {
        return -1;
    }
    for (int i = 0; i < PCN_STUB_W * PCN_STUB_H; i++) {
        s_fb[i] = argb[i] & 0x00FFFFFFu;
    }
    s_rendered = 1;
    return 0;
}

int32_t host_infra_read_pixel(int x, int y)
{
    if (!s_rendered) {
        return -1;
    }
    if (x < 0 || y < 0 || x >= PCN_STUB_W || y >= PCN_STUB_H) {
        return -1;
    }
    return (int32_t)s_fb[(size_t)y * PCN_STUB_W + (size_t)x];
}

/* art primitives: unmounted (glyph_advance == 0 -> s_art_ok == 0 -> home_p1
 * fallback-stroke paths; identical to the gate runner's no-art wiring). */
int kidpal_art_mount(const char *root)
{
    (void)root;
    return -1;
}

int kidpal_art_load(const char *path, kidpal_art_img_t *out)
{
    (void)path; (void)out;
    return -1;
}

int kidpal_art_pixel(const kidpal_art_img_t *img, int x, int y)
{
    (void)img; (void)x; (void)y;
    return 0;
}

int kidpal_art_glyph(char ch, kidpal_art_img_t *out)
{
    (void)ch; (void)out;
    return -1;
}

int kidpal_art_glyph_advance(void)
{
    return 0;
}
