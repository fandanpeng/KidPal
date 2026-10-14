/*
 * art_assets.c — 固件侧美术资产共享层实现（TaskPackage 260901-ui-art-integration, 包 U）
 *
 * 冻结离线契约（firmware/main/test/ui_art_integration.acceptance.c 头注）9 函数：
 *   mount / load / pixel / glyph / glyph_advance + render_asset / draw_nine_slice /
 *   draw_dots / draw_time。数值码与语义以冻结文件为准，不得偏移。
 *
 * 纯逻辑、零芯片依赖、零 LVGL 依赖（渲染经 additive 缝 art_render_blit_frame 交付，
 * host 上由 host_infra_seam.c 强符号覆盖打印到可探针帧缓冲；设备上弱默认 -1 = 真机
 * 渲染节点集成殿后 9b）。loader 用 stdio 通路读当前 mount root（host = 真实目录；
 * 设备 = esp spiffs 装配层注册的 VFS 路径）。KAP1 解码见冻结头注（little-endian）。
 *
 * Buffer 纪律：单槽缓存（kidpal_art_load 重复覆盖；测试在两次 load 之间消费）；
 * 渲染组合帧为整帧 400x300 ARGB，每次渲染 op 先填纸色再组合再整帧交付。
 */
#include "art_assets.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

/* ---- 渲染缝弱默认（host_infra_seam.c 强覆盖; 设备弱默认 -1） ---- */
__attribute__((weak))
int art_render_blit_frame(const uint32_t *argb, int w, int h)
{
    (void)argb; (void)w; (void)h;
    return -1;
}

/* ---- 帧常量（spec §5.5 设备屏 400×300; 渲染缝同尺寸） ---- */
#define KIDPAL_ART_FRAME_W 400
#define KIDPAL_ART_FRAME_H 300
#define KIDPAL_ART_PAPER   ((uint32_t)0x00FFFFFFu) /* 纸色（阈值化白） */
#define KIDPAL_ART_INK     0x00000000u             /* 墨色 */

/* ---- 加载器状态（单槽缓存） ---- */
static char        s_root[512];
static int         s_mounted = 0;
static uint8_t    *g_cache   = NULL;
static size_t      g_cache_cap = 0;
static kidpal_art_img_t g_img;

/* ---- 渲染组合帧 ---- */
static uint32_t g_fb[KIDPAL_ART_FRAME_W * KIDPAL_ART_FRAME_H];

int kidpal_art_mount(const char *root)
{
    DIR *d;

    if (root == NULL || root[0] == '\0') {
        return -1;
    }
    d = opendir(root);
    if (d == NULL) {
        return -1;
    }
    closedir(d);
    snprintf(s_root, sizeof(s_root), "%s", root);
    s_mounted = 1;
    return 0;
}

/* 从当前 root 用 stdio 通路把整文件读入缓存（容量不足则扩展）。返回长度; =0 失败。 */
static size_t art_read_file(const char *path, const uint8_t **out)
{
    FILE *f;
    long sz;
    size_t n;

    *out = NULL;
    f = fopen(path, "rb");
    if (f == NULL) {
        return 0;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return 0;
    }
    n = (size_t)sz;
    if (n > g_cache_cap) {
        uint8_t *nr = (uint8_t *)realloc(g_cache, n);
        if (nr == NULL) {
            fclose(f);
            return 0;
        }
        g_cache = nr;
        g_cache_cap = n;
    }
    if (n > 0 && fread(g_cache, 1, n, f) != n) {
        fclose(f);
        return 0;
    }
    fclose(f);
    *out = g_cache;
    return n;
}

static uint16_t u16le(const uint8_t *b)
{
    return (uint16_t)(b[0] | ((uint16_t)b[1] << 8));
}

static uint32_t u32le_at(const uint8_t *b)
{
    return (uint32_t)b[0] | ((uint32_t)b[1] << 8) |
           ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
}

int kidpal_art_load(const char *path, kidpal_art_img_t *out)
{
    const uint8_t *whole;
    size_t n, need;
    char full[600];
    uint16_t fmt, w, h, stride;
    size_t min_stride;

    if (path == NULL || out == NULL || !s_mounted) {
        return -1;
    }
    snprintf(full, sizeof(full), "%s/%s", s_root, path);
    n = art_read_file(full, &whole);
    if (whole == NULL || n < 16) {
        return -1;
    }
    if (u32le_at(whole) != KIDPAL_ART_MAGIC) {
        return -1;
    }
    fmt    = u16le(whole + 4);
    /* flags @6 预留在读头时忽略 */
    w      = u16le(whole + 8);
    h      = u16le(whole + 10);
    stride = u16le(whole + 12);
    /* reserved @14 忽略 */
    if (fmt != KIDPAL_ART_FMT_1BPP && fmt != KIDPAL_ART_FMT_RGB565) {
        return -1;
    }
    if (w == 0 || h == 0) {
        return -1;
    }
    min_stride = (fmt == KIDPAL_ART_FMT_1BPP) ? ((size_t)w + 7u) / 8u : (size_t)w * 2u;
    if ((size_t)stride < min_stride) {
        return -1;
    }
    need = (size_t)stride * (size_t)h;
    if (n < 16u + need) { /* 文件被截断 */
        return -1;
    }

    g_img.w            = (int)w;
    g_img.h            = (int)h;
    g_img.stride_bytes = (int)stride;
    g_img.format       = (int)fmt;
    g_img.data         = g_cache + 16;
    *out = g_img;
    return 0;
}

int kidpal_art_pixel(const kidpal_art_img_t *img, int x, int y)
{
    int byte_idx;

    if (img == NULL || img->data == NULL) {
        return -1;
    }
    if (x < 0 || y < 0 || x >= img->w || y >= img->h) {
        return -1;
    }
    if (img->format == KIDPAL_ART_FMT_1BPP) {
        byte_idx = y * img->stride_bytes + (x >> 3);
        return (img->data[byte_idx] >> (7 - (x & 7))) & 1;
    }
    if (img->format == KIDPAL_ART_FMT_RGB565) {
        uint8_t lo = img->data[y * img->stride_bytes + x * 2];
        uint8_t hi = img->data[y * img->stride_bytes + x * 2 + 1];
        uint16_t v = (uint16_t)(lo | ((uint16_t)hi << 8));
        if (v == 0x0000u) {
            return 1; /* 墨 */
        }
        if (v == 0xFFFFu) {
            return 0; /* 纸 */
        }
        return -1; /* 非阈值化产物 = 语义错误 */
    }
    return -1;
}

/* 当前 root 是否存在某文件（imgfont 族在场检测） */
static int art_has_file(const char *name)
{
    char full[600];
    FILE *f;

    if (!s_mounted) {
        return 0;
    }
    snprintf(full, sizeof(full), "%s/%s", s_root, name);
    f = fopen(full, "rb");
    if (f == NULL) {
        return 0;
    }
    fclose(f);
    return 1;
}

int kidpal_art_glyph(char ch, kidpal_art_img_t *out)
{
    char name[32];

    if (out == NULL) {
        return -1;
    }
    if (ch >= '0' && ch <= '9') {
        snprintf(name, sizeof(name), "imgfont_digit_%c.bin", ch);
    } else if (ch == ':') {
        snprintf(name, sizeof(name), "imgfont_colon.bin");
    } else if (ch == '%') {
        /* '%' 记号（M1-N1b 看屏终验：手工基元修形不达标 -> 改真字形）。与字母/数字同
         * cell 几何（48x64），由 tools/asset-pipeline/gen_imgfont_letters.py 生成 fixed
         * 名 imgfont_pct.bin（避开字母大小写分流命名；不在 gen_art_spiffs.mjs 白名单）。 */
        snprintf(name, sizeof(name), "imgfont_pct.bin");
    } else if ((unsigned char)ch == 0xB0) {
        /* '°' 度符号（2026-09-26 真机对齐轮：P1 温度 °C 后缀）。非 ASCII，调用方以
         * (char)0xB0 传入（ARM signed char 下为负值，故按 unsigned char 比对）。同 cell
         * 几何（48x64），由 gen_imgfont_letters.py 生成 fixed 名 imgfont_degree.bin
         * （避开字母大小写分流命名；不在 gen_art_spiffs.mjs 白名单）。 */
        snprintf(name, sizeof(name), "imgfont_degree.bin");
    } else if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
        /* 字母字形扩展（包 260915-m1-k1a-key-layer-dispatch，M1-N1b 日期 DD-Mon）：
         * 字母按大小写分流文件（NTFS 大小写不敏感属跨端缺陷——同名模板会使小写
         * 覆盖大写，故大写走 imgfont_letter_%c.bin、小写走 imgfont_letter_lc_%c.bin，
         * 与 tools/asset-pipeline/gen_imgfont_letters.py 生成侧同规）。与数字同 cell
         * 几何（48x64），由 gen_imgfont_letters.py 生成，47 枚闭集白名单管线不产出
         * （gen_art_spiffs.mjs 只写其白名单件、重跑不删字母 bin）。
         * 大写 'A' -> imgfont_letter_A.bin；小写 'a' -> imgfont_letter_lc_a.bin。 */
        if (ch >= 'a' && ch <= 'z') {
            snprintf(name, sizeof(name), "imgfont_letter_lc_%c.bin", ch);
        } else {
            snprintf(name, sizeof(name), "imgfont_letter_%c.bin", ch);
        }
    } else {
        return -1;
    }
    return kidpal_art_load(name, out);
}

int kidpal_art_glyph_advance(void)
{
    /* 冻结：= 数字字形宽（现资产 48）; 当前 root 无 imgfont 族 -> -1 */
    if (!s_mounted || !art_has_file("imgfont_digit_0.bin")) {
        return -1;
    }
    return 48;
}

/* ---- 渲染组合辅助 ---- */
static void art_fill_paper(void)
{
    for (int i = 0; i < KIDPAL_ART_FRAME_W * KIDPAL_ART_FRAME_H; i++) {
        g_fb[i] = KIDPAL_ART_PAPER;
    }
}

static void art_set_px(int x, int y, int ink)
{
    if (x < 0 || y < 0 || x >= KIDPAL_ART_FRAME_W || y >= KIDPAL_ART_FRAME_H) {
        return;
    }
    g_fb[y * KIDPAL_ART_FRAME_W + x] = ink ? KIDPAL_ART_INK : KIDPAL_ART_PAPER;
}

static int art_flush(void)
{
    return art_render_blit_frame(g_fb, KIDPAL_ART_FRAME_W, KIDPAL_ART_FRAME_H);
}

int kidpal_art_render_asset(const char *path, int dx, int dy)
{
    kidpal_art_img_t img;

    if (path == NULL) {
        return -1;
    }
    if (kidpal_art_load(path, &img) != 0) {
        return -1;
    }
    art_fill_paper();
    for (int y = 0; y < img.h; y++) {
        for (int x = 0; x < img.w; x++) {
            int p = kidpal_art_pixel(&img, x, y);
            if (p < 0) {
                return -1;
            }
            art_set_px(dx + x, dy + y, p);
        }
    }
    art_flush();
    return 0;
}

int kidpal_art_draw_nine_slice(const char *name, int dx, int dy, int w, int h)
{
    const int C = KIDPAL_ART_9SLICE_CORNER_PX;
    kidpal_art_img_t base;
    char fname[40];

    if (name == NULL || w < 2 * C || h < 2 * C) {
        return -1;
    }
    snprintf(fname, sizeof(fname), "%s.bin", name);
    if (kidpal_art_load(fname, &base) != 0) {
        return -1;
    }

    art_fill_paper();
    {
        const int srcw_mid = base.w - 2 * C; /* 源中段宽（>=0, 框族源均 >= 2C） */
        const int srch_mid = base.h - 2 * C; /* 源中段高 */
        const int dstw_mid = w - 2 * C;      /* 目标中段宽（>=0, 因 w>=2C） */
        const int dsth_mid = h - 2 * C;      /* 目标中段高 */

        for (int ty = 0; ty < h; ty++) {
            int sy;
            if (ty < C) {
                sy = ty;                                   /* 顶部: 行恒等 */
            } else if (ty >= h - C) {
                sy = base.h - (h - ty);                    /* 底部: 行恒等 */
            } else {
                sy = C + (ty - C) * srch_mid / dsth_mid;   /* 中部: 竖直拉伸 */
            }
            for (int tx = 0; tx < w; tx++) {
                int sx;
                if (tx < C) {
                    sx = tx;                               /* 左侧: 列恒等 */
                } else if (tx >= w - C) {
                    sx = base.w - (w - tx);                /* 右侧: 列恒等 */
                } else {
                    sx = C + (tx - C) * srcw_mid / dstw_mid; /* 中部: 水平拉伸 */
                }
                int p = kidpal_art_pixel(&base, sx, sy);
                if (p < 0) {
                    return -1;
                }
                art_set_px(dx + tx, dy + ty, p);
            }
        }
    }
    art_flush();
    return 0;
}

int kidpal_art_draw_dots(const char *name, int dx, int dy, int w, int h)
{
    kidpal_art_img_t dot;
    char fname[40];

    if (name == NULL || w <= 0 || h <= 0) {
        return -1;
    }
    snprintf(fname, sizeof(fname), "%s.bin", name);
    if (kidpal_art_load(fname, &dot) != 0) {
        return -1;
    }
    if (dot.w <= 0 || dot.h <= 0) {
        return -1;
    }

    art_fill_paper();
    for (int ty = 0; ty < h; ty++) {
        for (int tx = 0; tx < w; tx++) {
            int p = kidpal_art_pixel(&dot, tx % dot.w, ty % dot.h);
            if (p < 0) {
                return -1;
            }
            art_set_px(dx + tx, dy + ty, p);
        }
    }
    art_flush();
    return 0;
}

int kidpal_art_draw_time(const char *hhmm, int dx, int dy)
{
    int adv;
    size_t len;

    if (hhmm == NULL || hhmm[0] == '\0') {
        return -1;
    }
    len = strlen(hhmm);
    for (size_t i = 0; i < len; i++) {
        char c = hhmm[i];
        if (!((c >= '0' && c <= '9') || c == ':')) {
            return -1;
        }
    }
    if ((adv = kidpal_art_glyph_advance()) <= 0) {
        return -1;
    }

    art_fill_paper();
    for (size_t i = 0; i < len; i++) {
        kidpal_art_img_t g;
        if (kidpal_art_glyph(hhmm[i], &g) != 0) {
            return -1;
        }
        for (int y = 0; y < g.h; y++) {
            for (int x = 0; x < g.w; x++) {
                int p = kidpal_art_pixel(&g, x, y);
                if (p < 0) {
                    return -1;
                }
                art_set_px(dx + (int)i * adv + x, dy + y, p);
            }
        }
    }
    art_flush();
    return 0;
}