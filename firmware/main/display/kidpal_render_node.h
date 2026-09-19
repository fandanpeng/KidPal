/*
 * kidpal_render_node.h — 设备渲染节点（包 260911-screen-unification 归一屏收口）
 *
 * 把 KidPal 首页 P1（400×300 ARGB 帧）推至 RLCD，并提供 3 页 BOOT 双按翻页驱动。
 * 阶段 B：BOOT 双按 → home→schedule→story→home 三页循环（P4 查字常驻页已撤销，
 * 包 260917-m2-k2-retract-p4-three-page；页序对齐 spec P1→P2→P3）。
 * 前台归属 = kidpal_layer 前台 layer 栈管理单一权威（取代原三散标志 + 1s tick 对账）：
 * 语音轮页(3) / 服务端推送页(4) / 查字窗(5) / 本地浏览页(2) / 提醒弹窗(6) / 配网(7)
 * 按优先级表由 layer 栈 push/pop 事件驱动进出；聊天 layer(1) 常驻非栈成员。
 * （2026-09-19 M2-K3 页→窗改造：插入查字窗层，REMINDER 5→6 / PROVISION 6→7。）
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

/* 页枚举（与 cycle_next() 的步进顺序对应；记录「本地浏览层当前画哪一页」）。
 * 2026-09-17 M2-K2 撤 P4：查生字页枚举位整值撤出（CHAR 退出本地浏览页循环），
 * 三页重排为 spec 页序 KP_PAGE_HOME → KP_PAGE_SCHEDULE → KP_PAGE_STORY，
 * 与 key_gesture_parser.h 的 PAGE_P1/P2/P3 逐项同序同值。 */
typedef enum {
    KP_PAGE_HOME     = 0,  /* P1 首页常显（400×300 XRGB8888 → RLCD）*/
    KP_PAGE_SCHEDULE = 1,  /* P2 课表页 */
    KP_PAGE_STORY    = 2,  /* P3 故事列表页 */
    KP_PAGE_COUNT    = 3
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
 * 查字窗渲染的持锁夹钳（TaskPackage 260917-m2-k3-char-window-overlay, M2-K3 页→窗改造）：
 * 持锁 + 把 KIDPAL_LAYER_CHAR_WINDOW 推入 layer 栈（会话内覆盖窗——覆盖当前页之上、
 * 非常驻页），切活动屏到 s_kidpal_scr，并刷新键帽条（查字窗前台时中槽页点隐藏）。
 * 与 end_char_window() 配对释放锁。设备侧由 mcp_server.cc 的 view=char 渲染路径调用。
 *
 * **不起会话 / 聆听**（无 ToggleChatState / show_chat / voice_begin）——「播完会话即
 * 结束、不回聆听态」（§2.2 行 68 / F3 窗口交互包 v1.37）。关窗走
 * kidpal_render_node_dismiss_front_overlay（清 transient 回基层、回原页不翻页）。
 */
void kidpal_render_node_begin_char_window(void);
void kidpal_render_node_end_char_window(void);

/*
 * 提醒弹窗渲染的持锁夹钳：持锁 + 把 REMINDER 推入 layer 栈（REMINDER > VOICE/SERVER_PAGE，
 * review F8 架构修复——提醒恰在语音轮到点也置前、不崩），切活动屏到 s_kidpal_scr。
 * 与 end_page() 配对释放锁。设备侧由 mcp_server.cc RaiseReminderQueue 调用。
 */
void kidpal_render_node_begin_reminder(void);

/*
 * 配网进 layer 优先级表最高层（PROVISION=7，SU-A2 残项补全）：推 PROVISION + 收盾物理屏。
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
 * BOOT 双按翻页。依次推进 home→schedule→story→home（本地浏览层生命周期，
 * 出掉一切 transient 上层）。P4 查字常驻页已撤销（M2-K2，页序对齐 spec P1→P2→P3）；
 * 查字渲染面保留，其前台归属归 M2-K3 改窗决定。
 */
void kidpal_render_node_cycle_next(void);

/*
 * 前台归一化查询（layer 栈 owner 侧，TaskPackage 260915-m1-k1a-key-layer-dispatch）：
 * 前台 owner ∈ {VOICE, SERVER_PAGE, REMINDER, CHAR_WINDOW} -> 1（覆盖层顶层）；
 * LOCAL_PAGE / PROVISION / CHAT / NONE -> 0（配网态按键语义零触碰）。板级按键 dispatch
 * 消费它做 layer_top 分流；keybar 消费它做中槽页点隐藏（覆盖层不显示页点）。
 * （2026-09-19 M2-K3：闭集纳入 CHAR_WINDOW——查字窗前台自动隐藏页点。） */
int kidpal_render_node_front_is_overlay(void);

/*
 * 前台顶层是否为会话类覆盖层（VOICE，KD-A5 关层级联判据）：1 = VOICE 顶层；0 = 其它
 * （提醒弹窗 / 服务端推送页 / 基层 / 配网）。板级 OVERLAY_CLOSE 消费它决定是否级联
 * 结束会话——会话类覆盖层关层 = 结束会话回原页（requirements §2.3「问答·聆听/思考」
 * 行）；提醒弹窗等非会话覆盖层关层语义不变（本就无会话）。
 */
int kidpal_render_node_front_is_voice(void);

/*
 * 前台顶层是否为提醒弹窗覆盖层（REMINDER）：提醒态材料外壳显隐的判据（K4 消费——
 * 复用语音轮四态骨架时仅当 REMINDER 层确在顶层才点亮外壳, 关窗后自动回落不残留）。
 */
int kidpal_render_node_front_is_reminder(void);

/*
 * 当前前台顶层 layer 值（kidpal_layer.h 闭集 KIDPAL_LAYER_*; 空栈 = KIDPAL_LAYER_NONE）。
 * 提醒入层门控 kidpal_reminder_gate 的 front_layer 入参（RM-A2/RM-A3 机制单源）取此。
 */
int kidpal_render_node_front_layer(void);

/*
 * 设备侧显式关层步骤（「关窗」唯一入口，KD-A2 清层/进页分离）：前台为覆盖层
 * （front_is_overlay()）时出掉 transient 层回基层并重渲基层页；内部消费
 * kidpal_layer_pop_transients（单一清层事实源），永不映射翻页/cycle_next。
 */
void kidpal_render_node_dismiss_front_overlay(void);

/*
 * 板级任意键手势归零查字窗驻留计时（TaskPackage 260917-m2-k3-char-window-overlay，seam [9]e；
 * requirements §2.2 查字窗行 v1.43「计时随任意按键重计」）：板级 KEY/BOOT 任意键按下时调用，
 * 置「本拍有按键」标志——kidpal_render_node_tick 内作为 key_pressed 喂
 * kidpal_char_window_idle_step，使 5 分钟无操作驻留计时随任意按键重计（不关窗）。
 * 无锁标志写（tick 内取出并清位），可跨任务调用（按键回调任务 != LVGL 定时器任务）。
 */
void kidpal_render_node_char_window_key_hit(void);

/*
 * 注入 RTC 时间 + 日期（CustomBoard::InitializeSensors() 读完后调用）。
 * hour: 0..23, minute: 0..59。month: 1..12 / mday: 1..31（0 = 未知，渲染叠描边兜底）；
 * 日期字段为 M1-N1b 日期 DD-Mon 扩展（板级 PCF85063 寄存器 0x09=月 / 0x07=日 带入）。
 */
void kidpal_render_node_inject_rtc(int hour, int minute, int month, int mday);

/*
 * 注入温湿度（CustomBoard 传感器读后调用）。
 * env_valid: 1 = 数据有效（温湿度区可见）; 0 = 无效（隐藏）。
 * temp_c: 摄氏度（-40..60）。humidity_pct: 0..100。
 */
void kidpal_render_node_inject_env(int env_valid, int temp_c, int humidity_pct);

/*
 * 注入电量（包 260915-m1-n1b-p1-home-battery：CustomBoard ADC 读数后在
 * inject_rtc/inject_env 同一装配点调用，LVGL 就绪后）。
 * battery_pct: 0..100（越界值丢弃、保持原值——声明初始化器的保守默认 100 仅作
 * 无 ADC 读数时的兜底，本函数是它的唯一运行期写点）。
 * charging: 非 0 归 1。
 */
void kidpal_render_node_inject_battery(int battery_pct, int charging);

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

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_RENDER_NODE_H_ */