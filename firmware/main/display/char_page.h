/*
 * char_page.h — KidPal 查字窗 `char-window` (LVGL, 设备侧): 米字格大字盘 + 右栏速查卡 +
 * 底部提示条 + 逐笔动画驱动。**会话内覆盖窗**（M2-K3 页→窗改造；非常驻页——原 P4 常驻页
 * 时代已随 v1.36 撤页作废，本文件即窗的呈现面）。教案红线: 米字格示范字只走 strokes 数据
 * 光栅渲染 (kidpal_char_raster_stroke), 伴随文字才用字体。
 *
 * 数据结构经 kidpal_char_data_validate 防御降级门后喂入; kidpal_char_* 纯逻辑
 * 契约模块 (display/char_stroke.c) 是共享底座, 本窗只消费不重写。
 * 锚: UI = `char-window`（原锚 canvas-p4——P4 常驻页时代锚，v1.38 对账改名收尾）。
 * 层归属 = KIDPAL_LAYER_CHAR_WINDOW（会话内覆盖层；关窗走 dismiss_front_overlay 回原页、
 * 不翻页）。
 */
#ifndef KIDPAL_CHAR_PAGE_H
#define KIDPAL_CHAR_PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/* 打开/刷新查字窗 (在 LVGL 主线程上调用): data_json = view:"char" 的完整
 * data 对象 JSON 文本。在当前页之上叠一层窗面板、**不清屏**（关窗后原页原样回来、
 * 不翻页——M2-K3 页→窗改造）。缺字段或 NULL 由 mcp_server 先行 data_validate 降级;
 * 本函数仍防御 (不崩、不盲渲染)。 */
void kidpal_char_page_show(const char *data_json);

/* KEY 短按重播: 从第一笔重新逐笔演示 (S-F3.3; 由 key 手势消费面在按键时调用)。
 * 页面内部将动画进度归零重来。 */
void kidpal_char_page_replay(void);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_CHAR_PAGE_H */