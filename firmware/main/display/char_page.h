/*
 * char_page.h — KidPal 查生字 P4 页 (LVGL, 设备侧): 米字格大字盘 + 右栏速查卡 +
 * 底部提示条 + 逐笔动画驱动。教案红线: 米字格示范字只走 strokes 数据光栅渲染
 * (kidpal_char_raster_stroke), 伴随文字才用字体。
 *
 * 数据结构经 kidpal_char_data_validate 防御降级门后喂入; kidpal_char_* 纯逻辑
 * 契约模块 (display/char_stroke.c) 是共享底座, 本页只消费不重写。
 */
#ifndef KIDPAL_CHAR_PAGE_H
#define KIDPAL_CHAR_PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/* 打开/刷新查生字页 (在 LVGL 主线程上调用): data_json = view:"char" 的完整
 * data 对象 JSON 文本。缺字段或 NULL 由 mcp_server 先行 data_validate 降级;
 * 本函数仍防御 (不崩、不盲渲染)。 */
void kidpal_char_page_show(const char *data_json);

/* KEY 短按重播: 从第一笔重新逐笔演示 (S-F3.3; 由 key 手势消费面在按键时调用)。
 * 页面内部将动画进度归零重来。 */
void kidpal_char_page_replay(void);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_CHAR_PAGE_H */