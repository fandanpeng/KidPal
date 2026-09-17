/*
 * reminder_page.h — KidPal 提醒弹窗消费面 (TaskPackage 260829-schedule-reminder,
 * 契约 §三 提醒推送 / §四 mcp kidpal/alert / spec S-F1.4 / 决策 20·23·24·27)。
 *
 * 语义权威 = 冻结验收 firmware/main/test/schedule_reminder_b.acceptance.c 文件头
 * （FROZEN OFFLINE CONTRACT：符号名 + 数值码 + 行为表不得偏移）。本头只声明冻结
 * 契约 + 本包设备接线所需的两个非冻结附加符号（单例状态存取 + 提示音缝）。
 *
 * 形态与 schedule_page.{c,h}（包 6）同家法：纯 C + 自足式极简 JSON 扫描（不引外部
 * JSON 库）+ 纯 LVGL 渲染面（lv_obj_/lv_label only，无 Board/Display class 依赖）
 * -> host_app linux target 与设备树两侧同源可编。
 *
 * 介质禁令（决策 24 哑终端纯洁性）：本模块零持久层、零动态尺寸容器 —— known-id 集
 * 与队列只驻 RAM（定长数组，重启即全忘），不落 Flash/SD/文件（RMD-A6/A7 结构齿）。
 * 三档阶梯时刻（20:30 / 07:45 / 事前 15 分）是纯服务端配置，永不进固件（RMD-A7 齿）。
 *
 * 降级语义（V27 / spec §4.0）：kidpal_alert_parse 对畸形/缺必需字段一律 -1，调用方
 * 必须显式降级、绝不入队、绝不盲渲染。
 */
#ifndef KIDPAL_REMINDER_PAGE_H
#define KIDPAL_REMINDER_PAGE_H

#include <stddef.h>

/* 相对本头自身目录解析（firmware/main/display/ -> firmware/main/），与冻结验收
 * 文件 "../key_gesture_parser.h" 同款模式；页序/键位单源取包 3 底座。 */
#include "../key_gesture_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- 载荷解析（契约 §三 kidpal/alert 设备 wire 形态）------------------------
 * 设备侧 mcp Property 仅 bool/int/string 三型（mcp_server.h:52-56），故 §三 载荷里
 * 嵌套的 view 显示块在 tools/call arguments 上以 **JSON 字符串** 承载 —— 与
 * kidpal/display 的 data 字符串化同一纪律（mcp_server.cc kidpal/display 的
 * Property("data", kPropertyTypeString)）。
 * 业务幂等 id（§三 字符串 id）与 JSON-RPC 传输 id（必须是数字，
 * mcp_server.cc cJSON_IsNumber 判据）是两个不同的 id：业务 id 留在 arguments.id 供
 * 易失去重，数字 id 只作回执关联（云端侧数字化见 server/gateway/reminders.py）。 */
/* 字段容量（与冻结件 kidpal_alert_t 的 id[40]/fire_at[32]/title[64]/speech[96]/
 * view_json[256] 逐一相等 —— 跨 TU 布局必须一致，故用宏单源，不重复字面值）。 */
#define KRM_CAP_ID     40
#define KRM_CAP_FIRE   32
#define KRM_CAP_TITLE  64
#define KRM_CAP_SPEECH 96
#define KRM_CAP_VIEW   256

typedef struct {
    char id[KRM_CAP_ID];              /* §三 业务幂等 id（确定性散列，非空 ASCII token） */
    char fire_at[KRM_CAP_FIRE];       /* ISO8601+08:00（也接受 Z / ±HH:MM 偏移） */
    char title[KRM_CAP_TITLE];        /* 大字事项（含要带的东西） */
    char speech[KRM_CAP_SPEECH];      /* TTS 文案；载荷缺省该项 -> ""（降级仅弹窗+提示音） */
    char view_json[KRM_CAP_VIEW];     /* 内嵌 display 块，JSON 字符串原样保真；缺省 -> "" */
} kidpal_alert_t;

/* 解析 kidpal/alert tools/call 的 ARGUMENTS 对象 JSON 串。0 = 成功（字段 safe
 * 截断、恒 NUL 结尾）；-1 = 降级信号（NULL / 畸形 JSON / id 缺失或非非空串 /
 * title 缺失 / fire_at 不可解析 / speech|view 存在但非串）。容忍缺 speech、缺
 * view（-> ""）与未知扩展字段。返回 -1 时调用方 MUST NOT 入队、MUST NOT 盲渲染。 */
int kidpal_alert_parse(const char *args_json, kidpal_alert_t *out);

/* ISO8601 瞬时 -> 自 1970-01-01T00:00:00Z 起的**整分钟**数（UTC 归一，同一瞬时不同
 * 偏移写法得同值；秒的小数向下截断）。NULL / 畸形 -> -1。任何输入不崩。
 * V18「事件时刻过后」顶替判定的算术底座。 */
long long kidpal_fire_at_minutes(const char *fire_at_iso);

/* ---- 入层门控 + 时间行单源（TaskPackage 260915-m1-k4-reminder-overlay-unify）
 * 语义权威 = frozen acceptance firmware/main/test/reminder_overlay_unify.acceptance.c
 * 文件头 Seam contract [1]/[2]。KRM_GATE_* 冻结数值与本文件验收件内自持声明逐值一致
 * （偏移即预处理器重定义冲突 fail-loud）；builder 落此不得偏离。 ------------------
 *
 * [1] 提醒层入层门控决策表（RM-A2 最高层覆盖 / RM-A3 录音排队例外机制单源）：
 *   front_layer = 到达时前台 owner（kidpal_layer.h 闭集值 KIDPAL_LAYER_*）;
 *   recording   = 录音态在世（0/1）。
 *   recording != 0 -> KRM_GATE_DEFER（唯一例外 = 录音态：录音期间提醒排队延后、录音
 *     会话结束立刻弹——防提醒 TTS 混进录音内容, 对任何前台一律延后）;
 *   recording == 0 && front_layer >= 0 -> KRM_GATE_RAISE（最高层覆盖: 基层/查字窗/
 *     播报中/语音轮/聊天/无前台一律允许入层弹窗; 实际压层由 layer 栈优先级承载）;
 *   front_layer < 0 或 recording 不在 {0,1} -> KRM_GATE_INVALID（从不为 UB）。 */
#define KRM_GATE_RAISE   1   /* 立即入层弹窗（最高层覆盖, RM-A2） */
#define KRM_GATE_DEFER   0   /* 排队延后（录音排队例外, RM-A3） */
#define KRM_GATE_INVALID (-1) /* 越界防御 */
int kidpal_reminder_gate(int front_layer, int recording);

/* [2] 弹窗时间行单源（RM-A1 内容机制, 现有 krm_hhmm_of 逻辑的导出, 行为零变化）:
 *   fire_at 字面 11..15 位组 "HH:MM"（孩子看到课表里的那个时刻, 不做设备时区二次换算）:
 *   0 = 组出 HH:MM; 1 = 形状不可读 -> "--:--" 兜底占位（含 NULL fire_at, 明确占位不
 *   空白）; -1 = out NULL / cap < 6（"--:--"+NUL 最小 6 字节）。恒 NUL 结尾、绝不溢出。 */
int kidpal_reminder_hhmm(const char *fire_at, char *out, size_t cap);

/* ---- 设备侧提醒状态机（四态 received / queued / rendered / acked）----------- */
enum {
    KRM_STATE_RECEIVED = 0, /* 接收即入 known-id 集（屏蔽重复入队） */
    KRM_STATE_QUEUED = 1,   /* 新 id：入集且入队 */
    KRM_STATE_RENDERED = 2, /* 已渲染驻留项被重推 -> 补回回执 */
    KRM_STATE_ACKED = 3     /* 已回执出队 */
};
#define KRM_STATE_INVALID (-1)

/* 按键裁决：DISMISS = 弹窗关闭并交出页面裁决；PASS = 弹窗不吞、驻留不变 */
enum { KRM_KEY_DISMISS = 0, KRM_KEY_PASS = 1 };

/* 三件套动作集（决策 27）：SHOW 弹窗 / TONE 提示音 必达，SPEAK 为可选增强。
 * SPEAK 的出音不在设备侧（裁定二：服务端到点向引擎腿注入 kidpal_speak，借引擎既有
 * edge-tts + opus 下行链出音）；设备侧只据此决定弹窗内的「小伴在说」形态。 */
enum { KRM_ACT_SHOW = 1, KRM_ACT_TONE = 2, KRM_ACT_SPEAK = 4 };

#define KRM_QUEUE_CAP 8   /* 排队深度上界（当天有效事项数、提醒低频） */
#define KRM_KNOWN_CAP 32  /* 易失 known-id 集容量，定长数组 */

/* 不透明状态类型：内部布局只在本模块的 .c 里定义（冻结件按指针使用 = 只锁行为不锁
 * 内部）。定长、无堆指针、无文件句柄；尺寸上界受 host runner 裸缓冲纪律约束
 * （_Static_assert 在 .c 里钉 <= 4096 字节）。 */
typedef struct kidpal_reminder_s kidpal_reminder_t;

/* 零态：无 known id、空队列、无驻留弹窗。re-init 必遗忘既往一切（= 易失的可机判形态）。 */
void kidpal_reminder_init(kidpal_reminder_t *s);

/* 接收即入易失 known-id 集（幂等去重，与渲染无关）+ 排队。返回 KRM_STATE_QUEUED（新
 * id：入集并入队，不动驻留弹窗）/ KRM_STATE_RECEIVED（已知 id：不重复入队；含队列
 * 溢出时的有界屏蔽）/ KRM_STATE_RENDERED（已知且正是驻留弹窗且已渲染完成：调用方
 * MUST 补发 JSON-RPC 回执）/ KRM_STATE_INVALID（NULL / id 为空）。 */
int kidpal_reminder_receive(kidpal_reminder_t *s, const kidpal_alert_t *a);

/* known-id 计数（>=0, <= KRM_KNOWN_CAP）/ 未渲染排队项数 */
int kidpal_reminder_known_count(const kidpal_reminder_t *s);
int kidpal_reminder_queue_count(const kidpal_reminder_t *s);

/* 有驻留弹窗 -> 1 且把其 id 拷进 out（cap 内 safe 截断）；无弹窗 -> 0（out 不动）；
 * NULL 参数 -> -1。 */
int kidpal_reminder_shown_id(const kidpal_reminder_t *s, char *out, size_t cap);

/* 弹窗弹出 / V18 顶替判定（无超时、驻留到按键；本函数不含定时器，只在有事件时判定）：
 *   1 = 无弹窗且队列非空 -> 弹出 FIFO 头为驻留弹窗（rendered=0、事件分钟 = fire_at）
 *   2 = V18 滞留顶替：驻留弹窗**已渲染完成**且 now_min >= 其事件分钟（事件已过）且
 *       队列非空 -> 被 FIFO 头顶替（顶替非追加送达：known 集与既已出队项不复活）
 *   0 = 无可弹（空队列 / 驻留未渲染 / 事件时刻未过 —— 不替换、不叠屏）
 * NULL s -> -1。 */
int kidpal_reminder_raise_next(kidpal_reminder_t *s, long long now_min);

/* 渲染即回执决策表（「接收时不回执」的可机判边界）：
 *   1 = 有驻留弹窗 AND 该弹窗已完成渲染 AND 尚未为其发过回执（或被重复推送重新授权）
 *   0 = 无弹窗（接收/排队期）/ 未完成渲染 / 回执已发
 * 工具回调在此判定为 0 时 MUST NOT 发出表示「送达确认」的 JSON-RPC result；设备侧的
 * 延后回执出口是 McpServer::ReplyResult（内部 Application::SendMcpMessage）。 */
int kidpal_reminder_may_emit_receipt(const kidpal_reminder_t *s);

/* id 是驻留弹窗 -> 1（标记已渲染 -> 回执变许可）；不是驻留弹窗 -> 0；NULL -> -1。幂等。 */
int kidpal_reminder_mark_rendered(kidpal_reminder_t *s, const char *id);

/* 刚为该 id 许可过回执 -> 1（渲染即确认、即出队）；否则 0（已确认的不降级、未渲染的
 * 不确认）；NULL -> -1。 */
int kidpal_reminder_mark_acked(kidpal_reminder_t *s, const char *id);

/* 三件套动作集（纯函数、离线）：恒含 KRM_ACT_SHOW | KRM_ACT_TONE（弹窗+提示音必达）；
 * a->speech 非空时再或上 KRM_ACT_SPEAK。NULL / id 为空 -> -1（不盲投递）。 */
int kidpal_reminder_render_actions(const kidpal_alert_t *a);

/* 弹窗驻留期的按键裁决（spec S-F1.4 弹窗形态 + §4.0 全局手势一致）：
 *   KEY  CLICK       -> DISMISS，*page_out = current（关闭回当前页）
 *   BOOT CLICK       -> DISMISS，*page_out = key_page_next(current)（照常翻页不打断）
 *   KEY  LONG_PRESS  -> DISMISS，*page_out = PAGE_P1（全局回首页，所有页面一致）
 *   其余手势/越界键位 -> PASS，页与驻留都不变（弹窗不吞其它层手势）
 * 无弹窗驻留时任意手势 -> PASS 且不介入。NULL s/page_out -> -1。 */
int kidpal_reminder_key(kidpal_reminder_t *s, key_id_t key, key_gesture_t g,
                        page_id_t current, page_id_t *page_out);

/* 渲染面（纯 LVGL，画在活动屏）：**先清活动屏**（幂等，重渲染绝不与上一帧叠加）→
 * 整屏压 d50 网点聚焦 → 小伴 pose-jump 位 → 大字事项（title）→ 时间行（fire_at 的
 * HH:MM）→ 语音缺省时的「仅弹窗」形态；并按动作集起提示音缝
 * kidpal_reminder_play_popup_tone()（三件套的提示音一半，OGG_POPUP 音源在设备侧由
 * Application::PlaySound 实现，见 mcp_server.cc 的强定义覆盖本模块弱默认）。
 * 画成 -> 1；NULL / id 为空 -> -1；绝不崩。真实屏幕观感/网点疏密/pose 动画细节属
 * 设计 review 与 9b，本面只保证「真画到屏上 + 内容随载荷变化 + 不叠屏」。 */
int kidpal_reminder_page_render(const kidpal_alert_t *a);

/* ---- 非冻结附加符号（本包设备接线所需，冻结件不引用、不影响其行为断言）-------- */

/* 本模块唯一的设备侧提醒状态单例（定长静态存储、零动态分配；弹窗面全局单实例，与
 * McpServer 单例同构）。设备消费方（mcp_server.cc 的 kidpal/alert handler）经此取得
 * 状态，不需知晓内部布局。 */
kidpal_reminder_t *kidpal_reminder_default(void);

/* 把驻留弹窗的内容（id/fire_at/title/speech）拷给调用方，供渲染与播报决策；
 * view_json 不出队面（弹窗正文取 title，内嵌 display 块的渲染由 display 面按 §1.3
 * 白名单承载）。有驻留弹窗 -> 1；无 -> 0（out 不动）；NULL -> -1。 */
int kidpal_reminder_shown_alert(const kidpal_reminder_t *s, kidpal_alert_t *out);

/* 提示音缝：本模块给出弱默认（host / 无音源平台 = 不动作），设备侧强定义
 * （mcp_server.cc: Application::GetInstance().PlaySound(Lang::Sounds::OGG_POPUP)）。
 * 名称里的 OGG_POPUP 是小智资产音源名（assets/lang_config.h Lang::Sounds）。 */
void kidpal_reminder_play_popup_tone(void);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_REMINDER_PAGE_H */
