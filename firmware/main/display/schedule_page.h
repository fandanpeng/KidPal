/*
 * schedule_page.h — KidPal P2 课表页消费面 (TaskPackage 260829-schedule-p2,
 * 契约 §1.3 ② schedule view / S-F1.2 / S-F1.3)。
 *
 * 五个 FROZEN OFFLINE CONTRACT 函数的声明。语义权威 = 冻结验收
 * firmware/main/test/schedule_p2.acceptance.c 文件头 —— 本头仅供设备消费面
 * (mcp_server.cc 的 kidpal/display tool schedule 路由 ack) 引用，不得偏移。
 * (实现: firmware/main/display/schedule_page.c)
 *
 * 纯逻辑 + 纯 LVGL 渲染面（lv_obj_/lv_label/lv_anim only，无 Board/Display
 * class 依赖）-> host_app linux target 与设备树两侧同源可编。
 *
 * 降级语义：kidpal_schedule_parse 对畸形/缺字段/越界 cap 一律返回 -1 —— 调用方
 * 必须展示友好降级提示、绝不盲渲染 (V27 / spec §4.0 与 view 白名单同纪律)。
 */
#ifndef KIDPAL_SCHEDULE_PAGE_H
#define KIDPAL_SCHEDULE_PAGE_H

#include <stddef.h>

/* 相对头文件自身目录解析（firmware/main/display/ -> firmware/main/），与冻结验收
 * 文件的 "../key_gesture_parser.h" 同款模式，host_app linux target 与设备树两侧
 * include 均可达。 */
#include "../key_gesture_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 契约 §1.3 ② schedule view item（data 形状零变更，不新增字段）。 */
typedef struct {
    char time[16];     /* "HH:MM" 或 "HH:MM-HH:MM"（producer 下发原样） */
    char name[48];     /* 课程/事项名（UTF-8 字节，safe-truncated） */
    char location[48]; /* 地点；载荷缺省该项时为 "" */
} kidpal_schedule_item_t;

/* P2 页视图天（P2-A4: BOOT 长按 今天⇄明天）。 */
typedef enum {
    KSP_DAY_TODAY = 0,
    KSP_DAY_TOMORROW = 1
} ksp_day_t;

/* 解析 schedule view 的 data 对象 JSON 字符串（mcp kidpal/display handler 持有的
 * `data` 参数，非整个 {view,…} 信封）。返回 item 数（>=0）；畸形/缺 title/非数组
 * items/item 缺 time 或 name/cap 不足一律 -1（显式降级信号，绝不盲渲染）。
 * 容忍：缺 location 解析为 ""、未知扩展字段忽略（additive tolerance，契约 data
 * 形状不锁死扩展位）。title 可传 NULL（调用方不需要），但载荷必须携带 title。
 * 各字段拷贝按目标 cap safe-truncated（恒 NUL 结尾、不溢出）。 */
int kidpal_schedule_parse(const char *data_json, char *title, size_t title_cap,
                          kidpal_schedule_item_t *items, int cap);

/* P2-A3 圆点两态 (S-F1.2)：返回 0=空心（已上完/明天）、1=实心（待上）、
 * -1 防御（NULL 参数 / 今天视图无 now / 时刻不可解析）。
 * start_hhmm 可为 "HH:MM" 或 "HH:MM-HH:MM"（取前导 HH:MM 判定）。
 * 语义（frozen）：is_tomorrow != 0 -> 0（明天全空心，now 无关、NULL 亦 0）；
 * 今天：start <= now -> 0（已开始/已上完），start > now -> 1（待上）。 */
int kidpal_schedule_dot_state(const char *start_hhmm, const char *now_hhmm,
                              int is_tomorrow);

/* P2 页内输入特例 (spec §4.0 手势表页内特例行)：BOOT 长按 = 今天⇄明天切换 ->
 * 返回 current 的反向（标题弹跳切换由渲染面承载）；其余 key/gesture 组合
 * （KEY 长按回首页 / BOOT 单击翻页 / KEY 单击双击 / 越界键位）属全局层，
 * 一律原样返回 current 不吞。 */
ksp_day_t kidpal_schedule_page_input(key_id_t key, key_gesture_t g,
                                     ksp_day_t current);

/* P2-A5 滚动定位：返回与 (time, name) 精确匹配的行下标。time 可为 ""（仅按
 * name 通配）；name == NULL / items == NULL / n <= 0 -> -1；未命中 -> -1。
 * 两侧非空项均 EXACT 匹配（课程名是儿童可见文本，不做子串模糊）。 */
int kidpal_schedule_locate(const kidpal_schedule_item_t *items, int n,
                           const char *time, const char *name);

/* P2-A3/A4/A5 渲染面（纯 LVGL，画在 ACTIVE screen）：先清屏（幂等可反复调用），
 * 逐字绘制 title（P2-A4 标题带 lv_anim 弹跳切换）+ 自上而下 n 行（每行 = 圆点 +
 * 时间 + 课程名 + 地点，圆点两态按 kidpal_schedule_dot_state），当
 * 0 <= locate_row < n 时在该行画星标（P2-A5 星标呼应——设备侧渲染标志，
 * 不进 display data 形状；不做反白高亮态，human 裁砍不得引入）。
 * 返回 n；非法参数（NULL title、n>0 而 items==NULL、n<0）-> -1；绝不崩溃。 */
int kidpal_schedule_page_render(const char *title,
                                const kidpal_schedule_item_t *items, int n,
                                int is_tomorrow, const char *now_hhmm,
                                int locate_row);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_SCHEDULE_PAGE_H */
