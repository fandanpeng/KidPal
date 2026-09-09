/*
 * art_assets.h — 固件侧美术资产共享层（TaskPackage 260901-ui-art-integration, 包 U）
 *
 * 冻结离线契约：数值码、类型形状、函数语义以
 * firmware/main/test/ui_art_integration.acceptance.c 头注为准，不得偏移。
 * 本头供 display/ 下六页渲染面（包 2/4/5/6/7/8）统一消费共享资产层（ART-A6 换皮接线）。
 *
 * Exposure: C linkage、零芯片依赖、host-compilable（设备/host 同源）。
 * 设备侧 SPIFFS VFS 注册（esp spiffs）放芯片装配层、不进本模块——本模块只收
 * mount() 的 root 路径字符串。
 */
#ifndef KIDPAL_ART_ASSETS_H
#define KIDPAL_ART_ASSETS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- 冻结数值码 ---- */
#define KIDPAL_ART_UNIMPLEMENTED (-2)   /* weak 哨兵 — 永不是合法返回值 */
#define KIDPAL_ART_FMT_1BPP   1
#define KIDPAL_ART_FMT_RGB565 2
#define KIDPAL_ART_MAGIC      0x3150414Bu /* "KAP1" little-endian */
#define KIDPAL_ART_9SLICE_CORNER_PX 24   /* v3 定稿 24px 角片协议（三框通用） */

/* ---- 类型形状（冻结） ---- */
typedef struct {
    int w;               /* 像素宽, > 0 */
    int h;               /* 像素高, > 0 */
    int stride_bytes;    /* >= 每行最小字节数（1BPP: (w+7)/8; RGB565: w*2） */
    int format;          /* KIDPAL_ART_FMT_1BPP | KIDPAL_ART_FMT_RGB565 */
    const uint8_t *data; /* 像素数据（单槽缓存，有效期至下一次 mount/load） */
} kidpal_art_img_t;

/* ---- 冻结函数契约（全部 int 返回; -2 未实现哨兵, -1 防御/无效） ---- */
int kidpal_art_mount(const char *root);
int kidpal_art_load(const char *path, kidpal_art_img_t *out);
int kidpal_art_pixel(const kidpal_art_img_t *img, int x, int y);
int kidpal_art_glyph(char ch, kidpal_art_img_t *out);
int kidpal_art_glyph_advance(void);
int kidpal_art_render_asset(const char *path, int dx, int dy);
int kidpal_art_draw_nine_slice(const char *name, int dx, int dy, int w, int h);
int kidpal_art_draw_dots(const char *name, int dx, int dy, int w, int h);
int kidpal_art_draw_time(const char *hhmm, int dx, int dy);

/* additive 渲染缝：本模块以弱默认实现，host_app/main/host_infra_seam.c 以强符号覆盖，
 * 把整帧 ARGB 打印到可探针帧缓冲；设备侧无强符号 -> 用弱默认 -1（真机渲染节点集成
 * 殿后 9b）。具体弱默认定义在 art_assets.c。 */
int art_render_blit_frame(const uint32_t *argb, int w, int h);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_ART_ASSETS_H */