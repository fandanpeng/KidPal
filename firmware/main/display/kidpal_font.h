/*
 * kidpal_font.h — 统一 kidpal_font 句柄 seam（[K1]-[K4]）
 * TaskPackage 261028-m6-k1-font-assets-runtime-loading（P0 spike，ADR-017 资产与运行时
 * 底座切片；F-5 / F-6 / F-7）。签名逐字对齐冻结件
 * firmware/main/test/font_assets_runtime.acceptance.c 头注 seam contract 节。
 *
 * 语义（P1/P2/P3 唯一消费口——不得各自另造加载/降级分支，crossPackageBehaviors）：
 *   [K1] 资产数据源缝：reader(name, &data, &size) 返回 0 = 命中（data 指向裸 .bin
 *        payload，缓冲存活期覆盖句柄使用期）；非 0 = 缺失。
 *   [K2] 装载核心：from_cbin(data,size)；data==NULL 或 size<32 -> font_puhui_14_1。
 *        设备 = cbin_font_create（32-bit cbin 内存镜像）；host = 编译期宽度感知解码器
 *        （64-bit host 直调 cbin_font_create 必崩，预基线修正裁定 2）。
 *   [K3] mount：对两枚冻结名各取数装载；reader==NULL 等价全缺 -> 全部 14px（合法完成态）。
 *   [K4] body()/title() 页面消费句柄，恒非 NULL；title 缺字经 fallback 链回退 body（16px）。
 */
#ifndef KIDPAL_FONT_H_
#define KIDPAL_FONT_H_

#include <stddef.h>
#include <stdint.h>

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* [K5] 冻结资产名（改名 = 契约协商走 planner，不静默改）。 */
#define KIDPAL_FONT_ASSET_BODY_16 "font_body_16.bin"
#define KIDPAL_FONT_ASSET_TITLE_20 "font_title_20.bin"

/* [K1] 资产数据源缝。 */
typedef int (*kidpal_font_asset_reader_t)(const char *name,
                                          const uint8_t **data, size_t *size);

/* [K2] cbin 装载核心（数据 -> 统一句柄 + 14px 兜底；绝不返回 NULL）。 */
const lv_font_t *kidpal_font_from_cbin(const uint8_t *data, size_t size);

/* [K3] 资产挂载缝（开机 mount；reader==NULL 等价全缺 -> 14px 合法完成态）。 */
int kidpal_font_mount_assets(kidpal_font_asset_reader_t reader);

/* [K4] 页面消费句柄（恒非 NULL）。 */
const lv_font_t *kidpal_font_body(void);
const lv_font_t *kidpal_font_title(void);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_FONT_H_ */
