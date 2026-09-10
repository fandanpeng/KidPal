/*
 * kidpal_render_node.h — 设备渲染节点（包 9b HWV-A6 收口）
 *
 * 把 KidPal 首页 P1（400×300 ARGB 帧）推至 RLCD，并提供 4 页 BOOT 双按翻页驱动。
 * 阶段 A：启动后显示首页帧 + 成功日志。
 * 阶段 B：BOOT 双按 → home→char→schedule→story→home 循环。
 *
 * 实现文件（kidpal_render_node.cc）是设备专属（不进入 host 构建），
 * 在此头声明 C linkage 供 board .cc 和 mcp_server.cc 调用。
 */
#ifndef KIDPAL_RENDER_NODE_H_
#define KIDPAL_RENDER_NODE_H_

#include "schedule_page.h"
#include "story_page.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 页枚举（与 cycle_next() 的步进顺序对应）。 */
typedef enum {
    KP_PAGE_HOME     = 0,  /* P1 首页常显（400×300 XRGB8888 → RLCD）*/
    KP_PAGE_CHAR     = 1,  /* P4 查生字（需服务端下发笔顺数据；无数据则跳过）*/
    KP_PAGE_SCHEDULE = 2,  /* P2 课表页 */
    KP_PAGE_STORY    = 3,  /* P3 故事列表页 */
    KP_PAGE_COUNT    = 4
} kidpal_page_id_t;

/*
 * 初始化渲染节点。必须在 CustomLcdDisplay 构造完成后（LVGL 已初始化）调用。
 * 只调用一次；重复调用是幂等的。
 */
void kidpal_render_node_init(void);

/*
 * 显示首页 P1。注入当前 RTC/温湿度/电量状态，调用 kidpal_home_p1_render_frame()
 * 填充 s_p1_fb（PSRAM），通过 host_infra_render_p1_frame 强符号把帧交付 LVGL，
 * 最后 lv_screen_load() 把 KidPal 专属屏切到前台。
 *
 * 线程约束：在 main app task（Schedule 回调）或 CustomBoard 构造期调用均可；
 * 函数内部持 lvgl_port_lock，调用方无需预先加锁。
 */
void kidpal_render_node_show_home(void);

/*
 * 把 xiaozhi SetupUI 屏（聆听/表情/状态）切回前台（回归修复 包 9b HWV-A6 fix）。
 * 语音交互期间 KidPal 首页不得盖住 SetupUI，否则用户看不到「聆听」态、
 * 会反复单击反而把音频通道开→关（引擎侧只见空会话超时）。
 * show_chat() 内部持 lvgl_port_lock，调用方无需预加锁；可安全在 main app task
 * （Schedule 回调）中调用。语音会话自然结束回到 Idle 后，渲染节点的 tick 定时器
 * 会把屏幕交还 KidPal 页（见 kidpal_render_node.cc 状态对账）。
 */
void kidpal_render_node_show_chat(void);

/*
 * 服务端推送页面渲染的持锁夹钳（崩溃修复 包 260829-hardware-validation）。
 *
 * 根因：语音轮期间活动屏是 xiaozhi SetupUI 屏（s_chat_scr），若服务端页面渲染
 * 函数（kidpal_story_page_render / kidpal_schedule_page_render）就地
 * lv_screen_active() 取到该屏并 lv_obj_clean()，会连同 xiaozhi 的 status_label_
 * 等子控件一并销毁，而 LvglDisplay::SetStatus 仍持该成员指针 → 状态跃迁时
 * lv_label_set_text(悬空) → lv_free 垃圾指针 → LoadProhibited 崩溃。且这些渲染
 * 在 main 任务裸调 lv_*，与 LVGL 任务竞态。
 *
 * 修法则：所有服务端推送页面渲染**必须**夹在 begin_page()/end_page() 之间——
 * begin_page() 持 lvgl_port_lock、把活动屏切到 KidPal 专属屏 s_kidpal_scr（后续
 * 渲染函数 lv_screen_active() 即取到此屏，clean 只清它、绝不碰 s_chat_scr）、
 * 清聊天覆盖标志、并置 s_page_pinned（使 tick 在 pin 期间不把页面抢回聊天屏）；
 * end_page() 释放锁。调用方在 Application::Schedule 闭包内、页面渲染前后成对调用。
 *
 * 线程约束：二者均在 main app task（Schedule 闭包）调用；begin_page 内部取锁，
 * 调用方不得已持有 LVGL 锁（否则递归死锁——例如不得在 LVGL 定时器回调内调用）。
 */
void kidpal_render_node_begin_page(void);
void kidpal_render_node_end_page(void);

/*
 * BOOT 双按翻页。依次推进 home→char→schedule→story→home。
 * CHAR 页在无笔顺数据时自动跳过（进入下一轮）。
 */
void kidpal_render_node_cycle_next(void);

/*
 * 注入 RTC 时间（CustomBoard::InitializeSensors() 读完后调用）。
 * hour: 0..23, minute: 0..59。
 */
void kidpal_render_node_inject_rtc(int hour, int minute);

/*
 * 注入温湿度（CustomBoard 传感器读后调用）。
 * env_valid: 1 = 数据有效（温湿度区可见）; 0 = 无效（隐藏）。
 * temp_c: 摄氏度（-40..60）。humidity_pct: 0..100。
 */
void kidpal_render_node_inject_env(int env_valid, int temp_c, int humidity_pct);

/*
 * 返回当前显示页（kidpal_page_id_t）。用于外部判断是否处于 KidPal 模式。
 */
int kidpal_render_node_get_page(void);

/*
 * 更新动画 tick 并重渲当前帧（调用方在 LVGL 定时器中按 4.4s 周期调用）。
 * 仅当当前页为 HOME 时才触发重渲，其他页无副作用。
 */
void kidpal_render_node_tick(void);

/*
 * 缓存 P2 课表数据（供 cycle_next 翻页使用）。
 * mcp_server 在收到服务端下发课表后调用；title/items 可为 NULL（则保持上次缓存）。
 */
void kidpal_render_node_cache_schedule(const char *title,
                                       const kidpal_schedule_item_t *items,
                                       int n,
                                       int is_tomorrow,
                                       const char *now_hhmm);

/*
 * 缓存 P3 故事页状态（供 cycle_next 翻页使用）。
 * mcp_server 的 ScheduleStoryRender 在渲染后调用，以同步最新状态。
 */
void kidpal_render_node_cache_story(const kidpal_story_view_t *v,
                                    const kidpal_story_list_t *l,
                                    const kidpal_story_text_t *t);

/*
 * 标记 P4 查生字页是否有有效数据。
 * mcp_server 在 kidpal_char_page_show() 调用前后设置。
 * has_data=true 后 cycle_next 才会在 4 页循环中停靠 CHAR 页。
 */
void kidpal_render_node_cache_char(int has_data);

/*
 * 缓存最近一次 P4 笔顺 JSON（供 cycle_next 翻页重新渲染）。
 * mcp_server 在 char_page_show 后调用，传入原始 data_json 字符串。
 * 内部 strdup 存储，动态分配，不增加静态 DRAM。
 */
void kidpal_render_node_cache_char_data(const char *data_json);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_RENDER_NODE_H_ */
