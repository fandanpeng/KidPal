/*
 * kidpal_p1_font.h — 首页 P1 动态日程名字形获取 seam [S1] 生产侧声明
 * (TaskPackage 260921-m3-k4-p1-cjk-dynamic-text)。
 *
 * 与冻结验收 firmware/main/test/p1_cjk_name.acceptance.c 头注 [S1] 逐字段一致
 * （本文件按 C 符号名链接；冻结件本地 pcn_glyph_t 与本类型 ABI-兼容）。
 *
 * 强符号落点（每个二进制恰一个强符号覆盖 home_p1.c 弱默认，多重 weak 合法）：
 *   - 设备：firmware/main/display/kidpal_render_node.cc（font_puhui_14_1）
 *   - host：firmware/main/test/host_app/main/host_infra_seam.c（Source Han 子集）
 *
 * 本头刻意零 LVGL/芯片依赖——home_p1.c 纯逻辑侧也 include 无需拉进渲染后端。
 */
#ifndef KIDPAL_P1_FONT_H
#define KIDPAL_P1_FONT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 字形墨盒描述（[S1]）：与冻结件 pcn_glyph_t 布局逐字段一致。 */
typedef struct {
    int w;                     /* 字形墨盒宽 px, 1..64 */
    int h;                     /* 字形墨盒高 px, 1..64 */
    int advance;               /* 笔进 px, >= 0 */
    const unsigned char *bits; /* 1bpp 行主序位图, stride = (w+7)/8, MSB 先行, 1 = 墨;
                                  * 有效期到本函数下一次调用 */
} kidpal_p1_glyph_t;

/*
 * 取字形：返回 0 = 字形可用（*out 已填充）；非 0 = 不可用（out 不消费）。
 * out == NULL -> 非 0（防御）。弱默认（home_p1.c 内）恒返回非 0 = 不可用。
 */
int kidpal_p1_font_glyph(uint32_t codepoint, kidpal_p1_glyph_t *out);

/*
 * [S7] 字面源缝（TaskPackage 261028-m6-k2a-p1-home-16px，P1 新增 / [F1]）：
 * 返回 P0 统一句柄 seam [K4] 的 16px 正文档句柄 kidpal_font_body()——设备经
 * kidpal_render_node_body_font() 透传、host 经 kidpal_font_body()，同源同值。
 * 恒不崩；返回 NULL = 未实现/降级（[S1] 消费方回落到既有 14px 字面）。
 * 前向声明 struct _lv_font_t（lv_font_t 同型）以免把 LVGL 头拉进本纯声明头
 * ——home_p1.c 等零 LVGL 依赖消费方 include 本头不受影响。
 */
struct _lv_font_t;
const struct _lv_font_t *kidpal_p1_font_face(void);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_P1_FONT_H */