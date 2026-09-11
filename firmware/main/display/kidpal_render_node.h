/*
 * kidpal_render_node.h — 设备渲染节点（包 260911-screen-unification 归一屏收口）
 *
 * 把 KidPal 首页 P1（400×300 ARGB 帧）推至 RLCD，并提供 4 页 BOOT 双按翻页驱动。
 * 阶段 B：BOOT 双按 → home→char→schedule→story→home 循环。
 * 前台归属 = kidpal_layer 前台 layer 栈管理单一权威（取代原三散标志 + 1s tick 对账）：
 * 语音轮页(3) / 服务端推送页(4) / 本地浏览页(2) / 提醒弹窗(5) / 配网(6) 按优先级表
 * 由 layer 栈 push/pop 事件驱动进出；聊天 layer(1) 常驻非栈成员。
 *
 * 实现文件（kidpal_render_node.cc）是设备专属（不进入 host 构建），
 * 在此头声明 C linkage 供 board .cc / application.cc / mcp_server.cc 调用。
 */
#ifndef KIDPAL_RENDER_NODE_H_
#define KIDPAL_RENDER_NODE_H_

#include "schedule_page.h"
#include "story_page.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 页枚举（与 cycle_next() 的步进顺序对应；记录「本地浏览层当前画哪一页」）。 */
typedef enum {
    KP_PAGE_HOME     = 0,  /* P1 首页常显（400×300 XRGB8888 → RLCD）*/
    KP_PAGE_CHAR     = 1,  /* P4 查生字（需服务端下发笔顺数据；无数据则停靠占位）*/
    KP_PAGE_SCHEDULE = 2,  /* P2 课表页 */
    KP_PAGE_STORY    = 3,  /* P3 故事列表页 */
    KP_PAGE_COUNT    = 4
} kidpal_page_id_t;

/*
 * 初始化渲染节点。必须在 CustomLcdDisplay 构造完成后（LVGL 已初始化）、且聊天 chrome
 * 已由板装配层 reparent 到 lv_layer_top() 之后调用（板装配层在 init 前调
 * LcdDisplay::KidPalReparentChatToLayer）。只调用一次；重复调用是幂等的。创建
 * display/ 下唯一的顶层屏（s_kidpal_scr）。
 */
void kidpal_render_node_init(void);

/*
 * 显示首页 P1：出掉一切 transient 上层，凝为本地浏览基层 LOCAL_PAGE，然后渲染首页
 * 帧 + 切前台。线程约束：内部持 lvgl_port_lock，调用方无需预加锁。
 */
void kidpal_render_node_show_home(void);

/*
 * 把语音/聆听聊天面置前：推 VOICE 层并按下前台规则收盾物理屏。若服务端内容页在更上层
 * （SERVER_PAGE > VOICE），内容页保持前台、语音不拦内容页（SU-RULING-01 方案A）。
 * 内部持 lvgl_port_lock，调用方无需预加锁。按键触发新会话前先置前以免「聆听」不可见。
 */
void kidpal_render_node_show_chat(void);

/*
 * 新语音轮开始（对应设备状态进入 Connecting）：清除上一轮驻留 transient（服务端回答页/
 * 提醒）并推 VOICE 层。幂等（push 幂等去重），可在按键预置与状态事件处重复调用。
 * 设备侧由 application.cc HandleStateChangedEvent 在 kDeviceStateConnecting 驱动，
 * 取代原 1s tick 轮询 GetDeviceState 对账。
 */
void kidpal_render_node_voice_begin(void);

/*
 * 语音轮结束（对应设备状态回到 Idle）：若栈中并无 VOICE 层则 no-op；否则出栈 VOICE，
 * 无服务端页驻留时回首页常显，有服务端回答页驻留时保持其前台（OQ3=c 回答驻留）。
 * 设备侧由 application.cc HandleStateChangedEvent 在 kDeviceStateIdle 驱动。
 */
void kidpal_render_node_voice_end(void);

/*
 * 服务端推送页面渲染的持锁夹钳（崩溃修复 包 260829-hardware-validation 语义保留）。
 *
 * begin_page()：持 lvgl_port_lock、把 SERVER_PAGE 推入 layer 栈（使服务端内容页置前）、
 * 把活动屏切到 KidPal 专属屏 s_kidpal_scr（后续渲染函数 lv_screen_active() 即取到此屏，
 * clean 只清它、绝不碰聊天层视觉控件）；end_page() 释放锁。调用方在 Application::Schedule
 * 闭包内、页面渲染前后成对调用。
 *
 * 线程约束：二者均在 main app task（Schedule 闭包）调用；begin_page 内部取锁，
 * 调用方不得已持有 LVGL 锁（否则递归死锁——不得在 LVGL 定时器回调内调用）。
 */
void kidpal_render_node_begin_page(void);
void kidpal_render_node_end_page(void);

/*
 * 提醒弹窗渲染的持锁夹钳：持锁 + 把 REMINDER 推入 layer 栈（REMINDER > VOICE/SERVER_PAGE，
 * review F8 架构修复——提醒恰在语音轮到点也置前、不崩），切活动屏到 s_kidpal_scr。
 * 与 end_page() 配对释放锁。设备侧由 mcp_server.cc RaiseReminderQueue 调用。
 */
void kidpal_render_node_begin_reminder(void);

/*
 * 配网进 layer 优先级表最高层（PROVISION=6，SU-A2 残项补全）：推 PROVISION + 收盾物理屏。
 * 配网进/退由 WifiBoard 配网模式进/退事件驱动（waveshare-s3-rlcd-4.2 板
 * EnterWifiConfigMode / StartWifiConfigMode 与 WifiManager 事件回调）。独立持锁释放，
 * 跨任务不成对调用（不像 begin_page/end_page 配锁）。
 */
void kidpal_render_node_begin_provision(void);

/*
 * 配网退 layer：出栈 PROVISION，前台按优先级表回落。与 begin_provision() 呼对，
 * 各自独立持锁释放。
 */
void kidpal_render_node_end_provision(void);

/*
 * BOOT 双按翻页。依次推进 home→char→schedule→story→home（本地浏览层生命周期，
 * 出掉一切 transient 上层）。CHAR 页已并入 s_kidpal_scr 体系（不再自建独立屏）。
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
 * 返回当前本地浏览页（kidpal_page_id_t）。仅记录 LOCAL_PAGE 基层当前画的是哪一页，
 * 前台 owner 由 layer 栈 top() 单一裁决。
 */
int kidpal_render_node_get_page(void);

/*
 * 更新动画 tick 并重渲当前帧（由 LVGL 定时器 in init 创建，1s 周期调用）。
 * 仅当本地首页为前台 owner（layer top == LOCAL_PAGE）时重渲动画，其它页无副作用；
 * 不含任何前台/语音轮询对账（对账已由事件驱动 layer push/pop 接管）。
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