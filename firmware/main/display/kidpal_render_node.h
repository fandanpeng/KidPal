/*
 * kidpal_render_node.h — 设备渲染节点（包 260911-screen-unification 归一屏收口）
 *
 * 把 KidPal 首页 P1（400×300 ARGB 帧）推至 RLCD，并提供 3 页 BOOT 双按翻页驱动。
 * 阶段 B：BOOT 双按 → home→schedule→story→home 三页循环（P4 查字常驻页已撤销，
 * 包 260917-m2-k2-retract-p4-three-page；页序对齐 spec P1→P2→P3）。
 * 前台归属 = kidpal_layer 前台 layer 栈管理单一权威（取代原三散标志 + 1s tick 对账）：
 * 语音轮页(3) / 服务端推送页(4) / 查字窗(5) / 删除确认弹窗(6) / 本地浏览页(2) / 提醒弹窗(7) / 配网(8)
 * 按优先级表由 layer 栈 push/pop 事件驱动进出；聊天 layer(1) 常驻非栈成员。
 * （2026-09-19 M2-K3 页→窗改造：插入查字窗层，REMINDER 5→6 / PROVISION 6→7。
 * 2026-09-29 260924-m4-k2-p3-star-notebook-ui seam [K10]：插入删除确认层
 * STORY_DELETE=6，REMINDER 6→7 / PROVISION 7→8——符号级插入 + 上移，注释数值随动。）
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
 * P3 删除确认覆盖层的持锁夹钳（TaskPackage 260924-m4-k2-p3-star-notebook-ui,
 * seam [K10]; WF-F2-05 整屏覆盖层）：持锁 + 把 KIDPAL_LAYER_STORY_DELETE 推入
 * layer 栈（覆盖当前页之上、提醒之下——提醒最高层压删除弹窗序不变），切活动屏到
 * s_kidpal_scr。与 end_story_delete() 配对释放锁。设备侧由 mcp_server.cc 的
 * story 删除确认子态（DELETE_ENTER）渲染路径调用。关层（确认/取消/10s idle）
 * 走 kidpal_render_node_dismiss_front_overlay（pop_transients 已纳入本层）。
 */
void kidpal_render_node_begin_story_delete(void);
void kidpal_render_node_end_story_delete(void);

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
 * 注入设备本地星期（M1-K1a RTC 注入家族同款第四缝, TaskPackage
 * 260921-m3-k2-p2-week-grid-page：inject_rtc/inject_env/inject_battery 之后）。
 * mon0_wday = 设备本地星期（周一=0..周日=6, 与 producer day 同基）; 0..6 合法
 * 写入, 越域忽略保持原值（防御, 不崩）。板级在 inject_rtc/inject_env 同一 RTC
 * 装配点调用（LVGL 就绪后）; 本地零点跨天后板级重读 RTC 即切换（P2 网格每次
 * 落屏重取今天列, 今天列 = 注入星期列;「今天」按设备本地日期零点切换、不依赖
 * 云端, NFR-OFF-1 语义一致）。
 */
void kidpal_render_node_inject_weekday(int mon0_wday);

/*
 * 当前注入星期列（今天列; 未注入/越域 = -1）。P2 网格渲染以此画今天列高亮,
 * 越域不高亮（不画假「今天」）。无锁整数读, 可跨任务调用。
 */
int kidpal_render_node_today_col(void);

/*
 * P1 首页是否在屏（LOCAL_PAGE 前台且当前页 = HOME）。mcp home 路由「data.upcoming
 * 注入成功且 P1 首页在屏 -> 重渲染首页日程主体」的判据单一权威（M3-K2 A5 接线）。
 */
int kidpal_render_node_home_on_screen(void);

/*
 * P2 课表页是否在屏（LOCAL_PAGE 前台且当前页 = SCHEDULE）。沿 home_on_screen 先例
 * 口径逐字同构：kidpal_layer_top == KIDPAL_LAYER_LOCAL_PAGE && s_local_page ==
 * KP_PAGE_SCHEDULE。schedule 路由 restore 分支「P2 在屏重绘本页 / 接管态仅填缓存
 * 不重绘」的判据单一权威（ADR-005 R10）。LOCAL_PAGE 判定使接管态（SERVER_PAGE 等
 * 非本地页前台）一律返回 0——restore 只填缓存、不接管前台、不重绘。
 */
int kidpal_render_node_schedule_on_screen(void);

/*
 * P3 故事列表页是否在屏（LOCAL_PAGE 前台且当前页 = STORY）。沿 home_on_screen /
 * schedule_on_screen 先例口径逐字同构：kidpal_layer_top == KIDPAL_LAYER_LOCAL_PAGE &&
 * s_local_page == KP_PAGE_STORY。story_list 路由 restore 分支「P3 在屏仅重绘本页 /
 * 接管态与记录语境前台只填缓存」的判据单一权威（TaskPackage
 * 261010-p3-storylist-cold-start-restore, seam [SL2]）。LOCAL_PAGE 判定使记录语境
 * 前台（VOICE 层 = 引导态/录音态/问答会话）与接管态（SERVER_PAGE 等非本地页前台）
 * 一律返回 0——restore 帧只填缓存、不重绘、不推前台。只读静态态，零副作用。
 */
int kidpal_render_node_story_on_screen(void);

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
 * 缓存 P2 一周总览网格数据（供 cycle_next 翻页使用; cache_schedule 网格化——旧
 * 16 项列表缓存作废, TaskPackage 260921-m3-k2-p2-week-grid-page, 容量
 * KSG_COURSE_CAP=84 定容冻结值）。mcp_server schedule 路由解析成功后调用填充;
 * n > KSG_COURSE_CAP 时只拷前 cap 条、绝不溢出; n <= 0 或 courses == NULL 时
 * 清空缓存 = 空态（BOOT 翻页 cycle_next 读此缓存, 缓存空 -> 友好空态降级画面,
 * R-S0.2 纪律沿用, 绝不盲渲染）。
 */
void kidpal_render_node_cache_schedule_grid(const kidpal_schedule_course_t *courses,
                                            int n);

/*
 * 缓存 P3 故事页状态（供 cycle_next 翻页使用）。
 * mcp_server 的 ScheduleStoryRender 在渲染后调用，以同步最新状态。
 */
void kidpal_render_node_cache_story(const kidpal_story_view_t *v,
                                    const kidpal_story_list_t *l,
                                    const kidpal_story_text_t *t);

/*
 * P3 本地导航重渲（TaskPackage 260924-m4-k2-p3-star-notebook-ui 双通道方案）：
 * 持锁 + 经 rn_render_kidpal_page_locked(KP_PAGE_STORY) 消费 cache_story 缓存重渲
 * 基层当前页。本地导航（页内输入驱动：光标/跳屏/阅读翻屏/退出阅读）经本函数落屏、
 * 前台保持 LOCAL_PAGE（页点自然可见）——与云端推送路径（begin_page 夹钳、SERVER_PAGE
 * 前台）拆双通道（plan「P3 子态 × 层模型 × 页点」裁法）。调用方在 Schedule 闭包内
 * 先 cache_story 再调本函数。内部持 lvgl_port_lock，调用方无需预加锁。
 */
void kidpal_render_node_refresh_story(void);

/*
 * P3 本地导航通道的前台回落（260924-m4-k2 双通道方案配套）：把云端渲染夹钳
 * （begin_page 推 SERVER_PAGE）用过后的前台回落 LOCAL_PAGE 基层（pop SERVER_PAGE +
 * push LOCAL_PAGE + 键帽条刷新）。本地导航（页内输入动作）经 ScheduleStoryRender 的
 * begin_page 夹钳完成渲染后调用——渲染仍走云端夹钳纪律（260829 崩溃修复语义保留），
 * 前台在动作落定后保持 LOCAL_PAGE（页点可见、板级 P3 页内分流基层判据成立）。
 * 只回落本夹钳产生的 SERVER_PAGE 单层，其它层（REMINDER/STORY_DELETE 等）不碰。
 * 内部持 lvgl_port_lock，调用方无需预加锁。
 */
void kidpal_render_node_story_local_front(void);

/*
 * P3 子态查询（键帽随态变 getter，TaskPackage 260924-m4-k2-p3-star-notebook-ui）：
 * 删除确认覆盖层为前台（STORY_DELETE 层 top）-> KSTORY_PAGE_DELETE；
 * 本地基层当前页 = STORY -> 缓存视图的页态（LIST/READER/RECORDING/STAMP）；
 * 其余（非 P3 页 / 前台为其它覆盖层）-> -1（消费方回落既有键帽路径）。
 * 无锁整数读，可跨任务调用。
 */
int kidpal_render_node_story_substate(void);

/*
 * D2 真机轮 R1 根修缝（2026-10-20, 261019-p3-record-dialogue-guide fixer; additive
 * 尾部追加）。任何路径 lv_obj_clean(lv_screen_active()) 销毁屏上对象后必须调用本缝：
 * 渲染节点把静态首页图像控件指针 s_kidpal_img 置 NULL（clean 已销毁, 不置空 = 悬垂;
 * 旧 lv_obj_is_valid 出帧守卫对「地址复用假阳性」不可靠——悬垂地址被新对象复用时
 * is_valid 判真, lv_image_set_src 打在非图像对象上）。消费点 = story_page.c 渲染入口
 * （唯一在 clean s_kidpal_scr 时不经 rn_render_home_locked /
 * rn_render_kidpal_page_locked 的路径）; 设备实现住 kidpal_render_node.cc, host runner
 * 由 host_infra_seam.c 提供同符号空实现。调用方已持 lvgl 锁（渲染路径持锁纪律）。
 */
void kidpal_render_node_notify_screen_cleaned(void);

/*
 * D2 真机轮 R1 根修缝（同上批注）: 渲染成功出口把「基层内容页 = P3」真相同步给渲染
 * 节点（s_local_page = KP_PAGE_STORY 单点写）。修复 ScheduleStoryRender 本地导航通道
 * 裸调 kidpal_story_page_render 导致的 s_local_page 陈旧 HOME——voice_end [R3]
 * on_story_front 误判走 else 首页重建 + 首页动画 tick 悬垂 UAF 的共同前提。调用方已
 * 持 lvgl 锁; host runner 空实现（同上）。
 */
void kidpal_render_node_story_page_front(void);

/*
 * D2 真机轮 R1 根修缝（同上批注）: P3 录音演出定时器渲染前的「前台真相核」单一权威:
 * 基层内容页 = P3（s_local_page == KP_PAGE_STORY）才允许 StoryRecTimerCb 落渲染——
 * 任何其它路径已把基层重建为首页/课表时, 定时器跳过本拍（演出断一拍无害, 录音页
 * 盖掉基层 = 错 + 悬垂链）。返回 1 = P3 在基层（可渲）; 0 = 否。无锁整数读。
 */
int kidpal_render_node_story_page_is_local(void);

/*
 * P3 引导播报中子态查询（D3 v7 键帽路由用, 同批 additive）: 缓存视图的 rec_substate
 * （0 = 常规录音聆听, 1 = 引导播报中; story_page.h [G3] 冻结名）。消费点 = lcd_display
 * 键帽三槽随态（RECORDING 子态时按本值分叉帧 A/帧 B 键帽集——v7 p3-recording-guide）。
 * 无锁整数读，可跨任务调用。
 */
int kidpal_render_node_story_rec_substate(void);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_RENDER_NODE_H_ */