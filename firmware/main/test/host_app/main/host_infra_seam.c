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
#include "kidpal_p1_font.h"

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
 * additive 截图缝 (human 一次性授权 2026-09-13, happy-flow 截图记录) —— 不改任何
 * 既有断言行为。把当前帧缓冲 s_fb 原样写成 24-bit BMP（BMP 底行在前，逐行翻好转
 * 换；像素 0xRRGGBB 直接落 BGR 字节序）。没有帧时返回 -1（不凭空造帧）；落盘失败
 * （目录不存在等）返回 -2，由调用方决定是否断言。纯 additive：不影响 render/read
 * 语义与既有齿。
 * ========================================================================== */
#include <stdio.h>

int host_infra_dump_bmp(const char *path)
{
    if (path == NULL || !s_rendered) {
        return -1;
    }

    /* 24-bit BMP: 14B file header + 40B info header + 底行在前的 BGR 像素,
     * 每行 4 字节对齐（400*3=1200 恰好对齐, 无 padding）。 */
    enum { ROW_BYTES = SEAM_FRAME_W * 3 }; /* 1200, 编译期常量, 4 对齐 */
    const uint32_t data_size = (uint32_t)ROW_BYTES * SEAM_FRAME_H;
    const uint32_t file_size = 14 + 40 + data_size;
    uint8_t hdr[54];
    uint32_t u32;
    uint16_t u16;
    int32_t s32;
    FILE *f;
    int y;

    memset(hdr, 0, sizeof(hdr));
    hdr[0] = 'B'; hdr[1] = 'M';
    u32 = file_size;        memcpy(&hdr[2], &u32, 4);
    u32 = 54;               memcpy(&hdr[10], &u32, 4);   /* pixel data offset */
    u32 = 40;               memcpy(&hdr[14], &u32, 4);   /* info header size */
    s32 = SEAM_FRAME_W;     memcpy(&hdr[18], &s32, 4);
    s32 = SEAM_FRAME_H;     memcpy(&hdr[22], &s32, 4);
    u16 = 1;                memcpy(&hdr[26], &u16, 2);   /* planes */
    u16 = 24;               memcpy(&hdr[28], &u16, 2);   /* bpp */
    u32 = 0;                memcpy(&hdr[30], &u32, 4);   /* BI_RGB */
    u32 = data_size;        memcpy(&hdr[34], &u32, 4);

    f = fopen(path, "wb");
    if (f == NULL) {
        return -2;
    }
    if (fwrite(hdr, 1, sizeof(hdr), f) != sizeof(hdr)) {
        fclose(f);
        return -2;
    }
    for (y = SEAM_FRAME_H - 1; y >= 0; y--) {
        uint8_t row[ROW_BYTES];
        int x;
        for (x = 0; x < SEAM_FRAME_W; x++) {
            uint32_t px = s_fb[(uint32_t)y * SEAM_FRAME_W + (uint32_t)x] & 0xFFFFFFu;
            row[x * 3 + 0] = (uint8_t)(px & 0xFFu);         /* B */
            row[x * 3 + 1] = (uint8_t)((px >> 8) & 0xFFu);  /* G */
            row[x * 3 + 2] = (uint8_t)((px >> 16) & 0xFFu); /* R */
        }
        if (fwrite(row, 1, (size_t)ROW_BYTES, f) != (size_t)ROW_BYTES) {
            fclose(f);
            return -2;
        }
    }
    fclose(f);
    return 0;
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

/* ============================================================================
 * additive 美术资产渲染缝 (TaskPackage 260901-ui-art-integration, 包子 U) ——
 * art_assets.c 以弱默认定义同名符号; 本文件以强符号覆盖之, 把 art_assets 组合的
 * 400x300 ARGB 整帧打印到可探针帧缓冲并置 rendered, 使 host_infra_read_pixel 能
 * 探针资产渲染齿 (render_asset / 9-slice / 网点 / 动态时间)。纯 additive: 不改
 * host_infra_render_frame / host_infra_read_pixel 自身语义 (ART-A6 换皮用缝交付)。
 * ========================================================================== */
int art_render_blit_frame(const uint32_t *argb, int w, int h)
{
    if (argb == NULL || w != SEAM_FRAME_W || h != SEAM_FRAME_H) {
        return -1;
    }
    for (int i = 0; i < SEAM_FRAME_W * SEAM_FRAME_H; i++) {
        s_fb[i] = argb[i] & 0x00FFFFFFu;
    }
    s_rendered = true;
    return 0;
}

/* ============================================================================
 * [S1]/[S6] host 字形强符号（覆盖 home_p1.c 弱默认）。
 *
 * lv_font_source_han_sans_sc_14_cjk 子集同款路径（schedule_page.c 先例；host_app
 * REQUIRES lvgl 已在, CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_14_CJK 已开）。**方案 B
 * （human 拍板 2026-09-22）**：去掉 req_raw_bitmap=1，走 LVGL 官方解码
 * （lv_font_get_glyph_bitmap 把 glyph 转 A8 写入 draw_buf，内部按行处理 stride）→
 * A8 阈值 128 化 1bpp（A8@128 ≡ A4@8 ≡ A2@2 ≡ A1@1 = 50% 覆盖，逐齿期望零偏移）。
 * 字形盒上限收 32×32（对 [S1] 冻结 1..64 的实现收紧子集，非契约变更）。
 *
 * **共享核心（[S6] 追加 seam，第三追加轮 A11，human 硬要求 2026-09-22）**：解码+
 * 阈值化核心重构为字体参数化 p1_glyph_core()——[S1] 入口 = 固定 host 字体调核心，
 * [S6] 入口 kidpal_p1_font_glyph_for_font（宿主字体作参数）调同一核心。两入口对
 * 同字体输出逐字节一致（TEST 10 tooth 5 承载）；核心与设备 TU（kidpal_render_node.
 * cc）同构（设备生产入口仍 [S1] 单一，零新增导出符号）。host runner 恰一个强符号
 * 覆盖 home_p1 weak；设备由 kidpal_render_node.cc 提供同符号。
 * ========================================================================== */
/* 方案 B A8 静态 draw_buf（LVGL 官方解码 scratch；32×32 盒上限）。静态内存增量 =
 * 本缓冲大小 + 1bpp 输出（32*((32+7)/8)=128B）。[S1] 与 [S6] 共享同一缓冲/输出
 * 位图（TEST 10 tooth 5 已快照分隔，共享合法）。 */
#define P1_GLYPH_BOX_CAP 32
static unsigned char s_glyph_a8_data[LV_DRAW_BUF_SIZE(P1_GLYPH_BOX_CAP, P1_GLYPH_BOX_CAP, LV_COLOR_FORMAT_A8)]
    __attribute__((aligned(LV_DRAW_BUF_ALIGN)));
static lv_draw_buf_t s_glyph_a8_buf;
static int s_glyph_a8_ready = 0;

static int p1_glyph_a8_buf_init(void)
{
    if (s_glyph_a8_ready) {
        return 1;
    }
    if (lv_draw_buf_init(&s_glyph_a8_buf, P1_GLYPH_BOX_CAP, P1_GLYPH_BOX_CAP,
                         LV_COLOR_FORMAT_A8, 0,
                         s_glyph_a8_data, sizeof(s_glyph_a8_data)) != LV_RESULT_OK) {
        return 0;
    }
    s_glyph_a8_ready = 1;
    return 1;
}

/* [S6] 共享核心：字体参数化（[S1] 与 [S6] 共用；设备 TU 同构副本）。
 * 返回 0 = 字形可用（*out 已填充）; 非 0 = 不可用（out 不消费）。out == NULL -> 非 0。
 * 语义与 [S1] 逐字段一致（1bpp 行主序 MSB 先行、stride=(w+7)/8、盒上限 32×32）。
 * dsc.adv_w 已经 LVGL 官方尺度化为像素（(raw+8)>>4 舍入口径），即 [S1] 冻结 advance。 */
static int p1_glyph_core(const lv_font_t *font, uint32_t codepoint,
                         kidpal_p1_glyph_t *out)
{
    lv_font_glyph_dsc_t dsc;
    const uint8_t *a8;
    static unsigned char s_bits[P1_GLYPH_BOX_CAP * ((P1_GLYPH_BOX_CAP + 7) / 8)];
    int byte_stride, a8_stride;
    int x, y;

    if (out == NULL || font == NULL) {
        return -1; /* [S1]/[S6]: out == NULL -> 非 0；NULL 字体防御 */
    }
    if (!lv_font_get_glyph_dsc(font, &dsc, codepoint, 0)) {
        return -1;
    }
    if (dsc.is_placeholder || dsc.box_w == 0 || dsc.box_h == 0) {
        return -1;
    }
    if ((int)dsc.box_w > P1_GLYPH_BOX_CAP || (int)dsc.box_h > P1_GLYPH_BOX_CAP) {
        return -1; /* 32×32 实现收紧子集（[S1] 冻结 1..64 的实现子集） */
    }
    /* 方案 B：req_raw_bitmap 保持 0（默认）——LVGL 官方解码到 A8 draw_buf。
     * **live 轮乱码根因**：lv_font_get_glyph_dsc 不初始化 req_raw_bitmap（stack 局部
     * 未清零）；直接调 font->get_glyph_bitmap 会旁路掉 LVGL 官方包装
     * lv_font_get_glyph_bitmap 的 req_raw_bitmap=0 归一化，未清零字段随机命中
     * `if(req_raw_bitmap) return &glyph_bitmap[...]` 的 raw 早退 → 返回原图而非解码
     * A8，文字行乱码。TEST 10（A11）在 host 实证复现此根因（通道④分支1），修复 =
     * 显式清零（与官方包装语义一致；两 TU 同构同改）。 */
    dsc.req_raw_bitmap = 0;
    if (!p1_glyph_a8_buf_init()) {
        return -1;
    }
    {
        /* **live 轮乱码根因（通道④分支1 host 实证）**：LVGL 9 get_glyph_bitmap /
         * lv_font_get_bitmap_fmt_txt 的返回是 `lv_draw_buf_t *`（struct），不是像素
         * 数据指针；必须读返回对象的 .data。旧实现把 struct 指针强转 const uint8_t*
         * 当 A8 像素读 → 读到的全是 draw_buf 结构头垃圾值 → 字形位图错乱（文字行
         * 乱码）。两 TU 同构同改，与官方 lv_font_get_glyph_bitmap 消费语义一致。 */
        lv_draw_buf_t *s_dec_db = (lv_draw_buf_t *)font->get_glyph_bitmap(&dsc, &s_glyph_a8_buf);
        if (s_dec_db == NULL) {
            return -1;
        }
        a8 = (const uint8_t *)s_dec_db->data;
    }
    if (a8 == NULL) {
        return -1;
    }
    byte_stride = ((int)dsc.box_w + 7) / 8;
    a8_stride = (int)lv_draw_buf_width_to_stride((uint32_t)dsc.box_w, LV_COLOR_FORMAT_A8);
    memset(s_bits, 0, sizeof(s_bits));
    for (y = 0; y < (int)dsc.box_h; y++) {
        for (x = 0; x < (int)dsc.box_w; x++) {
            if (a8[y * a8_stride + x] >= 128) { /* A8 阈值 128 = 50% 覆盖 */
                s_bits[y * byte_stride + (x >> 3)] |=
                    (uint8_t)(1u << (7 - (x & 7)));
            }
        }
    }
    out->w = (int)dsc.box_w;
    out->h = (int)dsc.box_h;
    out->advance = (int)dsc.adv_w;
    out->bits = s_bits;
    return 0;
}

/* [S1] host 强符号 := 固定 host 子集字体调共享核心（模块语义零改——host 字体面
 * 恒 lv_font_source_han_sans_sc_14_cjk，[S1] 语义与旧实现逐字节等价）。 */
int kidpal_p1_font_glyph(uint32_t codepoint, kidpal_p1_glyph_t *out)
{
#if LV_FONT_SOURCE_HAN_SANS_SC_14_CJK
    LV_FONT_DECLARE(lv_font_source_han_sans_sc_14_cjk);
    return p1_glyph_core(&lv_font_source_han_sans_sc_14_cjk, codepoint, out);
#else
    if (out != NULL) {
        out->w = 0;
        out->h = 0;
        out->advance = 0;
        out->bits = NULL;
    }
    return -1;
#endif
}

/* [S6] 追加 seam 强符号（第三追加轮 A11）：按字体取字形，与 [S1] 共享同一解码+阈值
 * 化核心（测试可达缝——设备生产路径唯一字体入口仍 [S1]；本符号仅在 host runner
 * 定义，TEST 10 承载）。语义与 [S1] 逐字段一致。 */
int kidpal_p1_font_glyph_for_font(const lv_font_t *font, uint32_t codepoint,
                                  kidpal_p1_glyph_t *out)
{
    if (font == NULL) {
        if (out != NULL) {
            out->w = 0;
            out->h = 0;
            out->advance = 0;
            out->bits = NULL;
        }
        return -1;
    }
    return p1_glyph_core(font, codepoint, out);
}