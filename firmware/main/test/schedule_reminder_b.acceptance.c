/*
 * Frozen acceptance tests — TaskPackage 260829-schedule-reminder
 * (subsystem: firmware+server; 日程提醒全链路 · 设备面 = state-reminder 富弹窗三件套 /
 *  排队逐个弹 / V18 滞留顶替 / 四态 received-queued-rendered-acked 幂等去重 / 渲染才允许
 *  回执 / 易失 known-id 只驻 RAM / KEY 关闭回当前页、BOOT 关闭照常翻页;
 *  mergeOrder 11; dependsOn 260829-schedule-p2).
 *
 * Nature: host-runnable BEHAVIORAL + structural frozen acceptance on the DEVICE alert
 * consumption face, following the schedule_p2.acceptance.c (KSP) /
 * voice_roundtrip_b.acceptance.c (VRT) frozen-offline-contract precedent: the builder
 * MUST implement the pure-logic contract below (the __attribute__((weak)) sentinels here
 * are overridden by the strong definitions once the implementation is wired into the host
 * runner), so the red baseline is RED on real missing behaviour, never on a build/link
 * failure.
 *
 * ===========================================================================
 * FROZEN OFFLINE CONTRACT (builder MUST implement, pure C/C++-callable-from-C, host
 * compilable, NO chip/FreeRTOS/ESP-component deps, self-contained minimal JSON parse —
 * NO external JSON library dependency; recommended home:
 * firmware/main/display/reminder_page.{c,h}; builder also wires BOTH the implementation
 * source and THIS acceptance file into HOST_ACCEPTANCE_SRCS in
 * firmware/main/test/host_app/main/CMakeLists.txt, adding the new lines BEFORE the
 * "../../host_infra_smoke.acceptance.c" entry — that entry MUST stay last so 包 0 的
 * nothing-rendered 边界齿仍在进程内最先执行, see HOST_TEST_CONVENTIONS.md [C4.2] and the
 * 上游 launch 指令对相对顺序的明文要求):
 *
 *   ---- 载荷解析 (契约 §三 kidpal/alert 设备 wire 形态) ------------------------
 *   注意：设备侧 mcp Property 只能是 Boolean/Integer/String 三型 (mcp_server.h:52-56)，故
 *   §三 载荷里嵌套的 view 显示块在 tools/call arguments 上以 JSON 字符串 承载 ——
 *   与 kidpal/display 的 data 字符串化同一纪律 (mcp_server.cc:104 区段 Property
 *   ("data", kPropertyTypeString))。业务幂等 id (§三 字符串 id) 与 JSON-RPC 传输 id
 *   (必须是数字, mcp_server.cc:546 cJSON_IsNumber) 是两个不同的 id：业务 id 留在
 *   arguments.id 供易失去重，数字 id 只作回执关联（云端侧数字化，见 python 孪生档）。
 *
 *   typedef struct {
 *       char id[40];          -- §三 业务幂等 id (确定性散列, 非空 ASCII token)
 *       char fire_at[32];     -- ISO8601+08:00 (也接受 Z / ±HH:MM 偏移)
 *       char title[64];       -- 大字事项 (含要带的东西)
 *       char speech[96];      -- TTS 文案; 载荷缺省该项 -> ""（降级仅弹窗+提示音）
 *       char view_json[256];  -- 内嵌 display 块, JSON **字符串** 原样保真; 缺省 -> ""
 *   } kidpal_alert_t;
 *
 *   int kidpal_alert_parse(const char *args_json, kidpal_alert_t *out);
 *       Parse the kidpal/alert tools/call ARGUMENTS object as a JSON string.
 *       Returns 0 on success (fields copied, safe-truncated, always NUL-terminated),
 *       -1 on: NULL args_json / NULL out / malformed JSON / "id" missing or not a
 *       non-empty string / "title" missing or not a non-empty string / "fire_at"
 *       missing or not parseable by kidpal_fire_at_minutes() / "speech" or "view"
 *       present but not a string.  Tolerant (additive, 契约 data 形状不锁死扩展位):
 *       missing "speech" -> "", missing "view" -> "", unknown extra fields ignored.
 *       A -1 result is the device degrade signal: the caller MUST NOT enqueue and MUST
 *       NOT blind-render (V27 / spec §4.0, same discipline as kidpal_schedule_parse).
 *
 *   long long kidpal_fire_at_minutes(const char *fire_at_iso);
 *       Whole-minute resolution of an ISO8601 instant, expressed as minutes since
 *       1970-01-01T00:00:00Z (UTC-normalised, so the same instant written with a
 *       different UTC offset yields the SAME value; fractional seconds truncated down).
 *       -1 on NULL / malformed (no 'T' separator, bad date/time digits, missing or
 *       invalid offset, non-digit junk).  Never crashes on any input.
 *       Reference points (frozen, host-checkable):
 *         "2026-09-07T20:30:00+08:00" == "2026-09-07T12:30:00Z"      -> 29813070
 *         "2026-09-08T20:30:00+08:00"                                 -> 29814510
 *         "2026-09-09T07:45:00+08:00"                                 -> 29815185
 *         "2026-09-09T16:15:00+08:00"                                 -> 29815695
 *         "2026-09-09T16:30:00+08:00"                                 -> 29815710
 *         "2026-09-09T19:00:00+08:00"                                 -> 29815860
 *
 *   ---- 设备侧提醒状态机 (四态 received / queued / rendered / acked) ----------
 *   enum { KRM_STATE_RECEIVED = 0, KRM_STATE_QUEUED = 1, KRM_STATE_RENDERED = 2,
 *          KRM_STATE_ACKED = 3 };
 *   enum { KRM_KEY_DISMISS = 0, KRM_KEY_PASS = 1 };
 *   enum { KRM_ACT_SHOW = 1, KRM_ACT_TONE = 2, KRM_ACT_SPEAK = 4 };
 *
 *   #define KRM_QUEUE_CAP  8      -- 排队深度有界 (spec: 当天有效事项数, 提醒低频)
 *   #define KRM_KNOWN_CAP  32     -- 易失 known-id 集容量, 固定数组, 不动态分配
 *
 *   typedef struct kidpal_reminder_s { ... } kidpal_reminder_t;
 *       Plain-C struct, FIXED SIZE, no pointers to heap, no file handles.  内部布局由
 *       实现方定义（本冻结件只按指针使用它 = 只锁行为不锁内部）。  介质禁令
 *       (决策 24 哑终端纯洁性): the reminder surface MUST NOT persist anything —
 *       no NVS, no filesystem, no nvs_open/nvs_set/fopen/malloc in this module.  The
 *       known-id set is RAM-only and is expected to be lost on reboot/power-cycle.
 *
 *   void kidpal_reminder_init(kidpal_reminder_t *s);
 *       Zero state: no known ids, empty queue, no resident popup.  Re-init MUST forget
 *       everything previously recorded (this is the machine-judgable form of 易失).
 *
 *   int kidpal_reminder_receive(kidpal_reminder_t *s, const kidpal_alert_t *a);
 *       接收即入易失 known-id 集 (幂等去重, 与渲染无关) + 排队。Returns:
 *         KRM_STATE_QUEUED   — unseen id: recorded in known set AND appended to the
 *                              arrival-order queue (does NOT touch the resident popup;
 *                              弹窗期间新提醒入队不叠屏).
 *         KRM_STATE_RECEIVED — id already in the known set: NOT enqueued again
 *                              (屏蔽重复入队) and nothing else changes.  Two sub-cases
 *                              are distinguished by the follow-up queries below:
 *                              a queued-but-unrendered duplicate stays unrendered and
 *                              MUST NOT emit a receipt; a duplicate whose id is the
 *                              currently rendered resident popup re-arms the receipt
 *                              permission (补回回执, 重复弹窗根除).
 *         KRM_STATE_RENDERED — id already known AND it is the resident popup AND that
 *                              popup has completed rendering: the caller MUST (re-)emit
 *                              the JSON-RPC receipt, and kidpal_reminder_may_emit_receipt
 *                              returns 1 again afterwards.  No re-enqueue.
 *         KRM_STATE_INVALID (-1, == NULL s / NULL a / a->id empty) — nothing changes.
 *       Queue overflow (queue already KRM_QUEUE_CAP deep) MUST NOT crash and MUST NOT
 *       enqueue: returns KRM_STATE_RECEIVED (id still recorded, so a re-push cannot
 *       re-enqueue it later either).
 *
 *   int kidpal_reminder_known_count(const kidpal_reminder_t *s);   -- >=0, <= KRM_KNOWN_CAP
 *   int kidpal_reminder_queue_count(const kidpal_reminder_t *s);    -- 未渲染排队项数
 *   int kidpal_reminder_shown_id(const kidpal_reminder_t *s, char *out, size_t cap);
 *       1 + copy the resident popup's id when a popup is resident, 0 when none
 *       (out untouched), -1 on NULL args.
 *
 *   int kidpal_reminder_raise_next(kidpal_reminder_t *s, long long now_min);
 *       弹窗弹出/顶替判定 (无超时、驻留到按键 —— 本函数不含任何定时器, 只在有事件时判定):
 *         1 — no popup resident and the queue is non-empty: pop the FIFO head and make
 *             it the resident popup (rendered=0, acked=0, 事件分钟 = its fire_at).
 *         2 — V18 滞留顶替: a popup IS resident and HAS completed rendering and
 *             now_min >= that popup's 事件分钟 (即其 fire_at 已过去) and the queue is
 *             non-empty: the resident popup is replaced by the FIFO head.  This is NOT
 *             an extra delivery (它早已出队) — the known set and the acked state of the
 *             displaced popup MUST NOT be resurrected or re-delivered.
 *         0 — nothing to raise (empty queue, or the resident popup is unrendered, or
 *             its 事件时刻尚未过去 —— 不替换、不叠屏).
 *       NULL s -> -1.
 *
 *   int kidpal_reminder_may_emit_receipt(const kidpal_reminder_t *s);
 *       THE 渲染即回执 DECISION TABLE (the machine-checkable boundary of "接收时不回执"):
 *         1 — iff a popup is resident AND kidpal_reminder_mark_rendered() has completed
 *             for it AND no receipt has been emitted for it yet (or a duplicate push
 *             re-armed it, see KRM_STATE_RENDERED).
 *         0 — nothing resident (接收/排队阶段), or the resident popup has not finished
 *             rendering, or its receipt was already emitted.
 *       The tool callback MUST NOT return/emit a JSON-RPC result that means 送达确认
 *       before this predicate is 1; the deferred reply face is McpServer::ReplyResult
 *       (mcp_server.cc:603, Application::SendMcpMessage application.cc:906) — the tool
 *       body may return an "accepted" ack only, never the render receipt.
 *
 *   int kidpal_reminder_mark_rendered(kidpal_reminder_t *s, const char *id);
 *       1 when id is the resident popup (marks it rendered -> receipt becomes allowed),
 *       0 when id is not the resident popup, -1 on NULL args.  Idempotent.
 *   int kidpal_reminder_mark_acked(kidpal_reminder_t *s, const char *id);
 *       1 when a receipt was just permitted for that id (渲染即确认、即出队), 0 otherwise
 *       (never demote an acked popup, never ack an unrendered one), -1 on NULL.
 *
 *   int kidpal_reminder_render_actions(const kidpal_alert_t *a);
 *       三件套动作集 (决策 27 主动播报 + V20 speech 解耦降级), pure & offline:
 *         KRM_ACT_SHOW | KRM_ACT_TONE           — 弹窗 + 提示音必达 (语音只是可选增强)
 *         | KRM_ACT_SPEAK                       — only when a->speech is non-empty
 *         -1 on NULL / a->id empty.
 *       A payload whose speech did not synthesise (missing/empty field) therefore still
 *       delivers popup + tone, never blocks (RMD-A3 必达面).
 *
 *   int kidpal_reminder_key(kidpal_reminder_t *s, key_id_t key, key_gesture_t g,
 *                           page_id_t current, page_id_t *page_out);
 *       弹窗驻留期的按键裁决 (spec S-F1.4 弹窗形态):
 *         KRM_KEY_DISMISS, *page_out = current            — KEY CLICK: 关闭弹窗、回当前页
 *                                                             (条目已于渲染时确认出队)
 *         KRM_KEY_DISMISS, *page_out = 下一页 (P1->P2->P3->P4->P1 回绕)
 *                                                         — BOOT CLICK: 关闭弹窗并照常翻页
 *                                                             (保住「翻页不被打断」手势)
 *         KRM_KEY_DISMISS, *page_out = PAGE_P1            — KEY LONG_PRESS: 关闭 + 全局回
 *                                                             首页 (所有页面一致)
 *         KRM_KEY_PASS, *page_out = current               — BOOT LONG_PRESS / BOOT
 *                                                             DOUBLE_CLICK / KEY
 *                                                             DOUBLE_CLICK / 越界键位:
 *                                                             弹窗不吞其它层手势, 驻留不变
 *       NULL s/page_out -> -1; 弹窗不驻留时任意手势 -> KRM_KEY_PASS 且不改页.
 *       (BOOT CLICK 的「下一页」MUST be taken from the shared 包 3 底座 key_page_next()
 *       — 页序单源, 本包不得另立表; 断言按观测值钉, 见 P2 先例。)
 *
 *   int kidpal_reminder_page_render(const kidpal_alert_t *a);
 *       渲染面 (pure LVGL on the ACTIVE screen — lv_obj_ / lv_label / lv_anim only, no
 *       Board/Display class deps — so it compiles and runs in the host linux-target
 *       runner): clears the active screen FIRST (idempotent; a re-render of any alert
 *       must never accumulate/叠屏 with the previous frame), then draws 整屏压网点聚焦 +
 *       小伴 pose-jump 位 + 大字事项 (title) + 时间行 (fire_at 的 HH:MM) + 语音缺省时
 *       的「仅弹窗」形态.  Returns 1 when it drew, -1 on NULL/invalid (a->id empty) —
 *       NEVER crashes.  真实屏幕观感/网点疏密/动画细节属设计 review（不钉成脆断言）；
 *       本齿只钉「真画到屏上 + 不同内容产生可见差异 + 不叠加」这四条任务关键事实。
 * ===========================================================================
 *
 * While the implementation is absent, the __attribute__((weak)) defaults below return the
 * sentinel KRM_UNIMPLEMENTED (-2): every behavioral test asserts NOT-sentinel first, so
 * the red baseline reads "frozen contract not implemented" — a real missing-behaviour red,
 * not a link error.
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef / exitCriteria / sourceRef -> test):
 *   coverage: RMD-A2 (device face 消费面装配, structural)
 *             -> test RMD-A1 (提醒设备面装配: reminder 模块实现 frozen contract + 编入设备
 *                构建 CMakeLists + mcp_server.cc 注册 kidpal/alert 并路由到弹窗面 +
 *                渲染函数与「渲染才回执」判定门都有消费调用点(>=2 文件) + 提示音面在世 +
 *                包 2 view_router / 包 6 schedule_page 底座守卫)
 *   coverage: RMD-A1 guard (上游装配完整性) -> test upstream-integrity (见该 TEST_CASE 注记:
 *             实测 main 上 mcp_server.cc case 4 的 app.Schedule lambda 闭合被 char-stroke
 *             merge 覆盖丢失 —— 设备构建真红, 非本包缺失; 已回报 planner)
 *   coverage: RMD-A2 (契约 §三 载荷消费) + RMD-A4 (载荷形状逐字段到端)
 *             -> test RMD-A2/A4-parse (id/fire_at/title/speech/view 逐字段保真 + view
 *                JSON 字符串原样 + 缺 speech/view 容忍 + 畸形/缺 id/缺 title/坏 fire_at
 *                一律 -1 降级 + 安全截断不溢出 + fire_at 分钟数与偏移归一)
 *   coverage: RMD-A3 (三件套 + speech 降级必达)
 *             -> test RMD-A3 (render_actions: 弹窗+提示音恒在、speech 非空才 SPEAK、
 *                speech 缺失仍 SHOW|TONE 不阻塞)
 *   coverage: RMD-A5 (排队逐个弹 + V18 滞留顶替) + RMD-A7 (离线无本地触发)
 *             -> test RMD-A5-queue (入队不叠屏 / FIFO 逐个弹 / 事件时刻前不替换、过后
 *                允许顶替 / 顶替非追加送达 / 队列溢出有界不崩)
 *   coverage: RMD-A5 (KEY 关闭回当前页、BOOT 关闭照常翻页)
 *             -> test RMD-A5-key (四态手势裁决 + 与包 3 页序底座接缝 + 不吞其它层)
 *   coverage: RMD-A6 (四态送达语义 + 渲染才允许回执决策表 + 幂等去重 + 同 id 重推两态分处)
 *             -> test RMD-A6-states (接收/排队期 may_emit_receipt 恒 0、mark_rendered 后
 *                才 1、回执一次后回 0、排队未渲染重复推送不入队不回执、已渲染补回回执)
 *   coverage: RMD-A6/A7 (易失 known-id 只驻 RAM, 决策 24 介质禁令)
 *             -> test RMD-A6-ephemeral (行为: re-init 即全忘 + 已知 id 再推被屏蔽 +
 *                集合有界; 结构: 模块零 NVS/文件系统/动态分配 + 结构体尺寸有界)
 *   coverage: RMD-A5 (渲染面真帧证据) -> test RMD-A5-render (host-runner-only, lvgl 软
 *                渲染真帧: 画到屏上 + 两个不同事项真帧可见差异 + 重渲染不叠屏 +
 *                NULL/非法不崩) — 需 lvgl include path, 独立红基线 stub 编译时被编译排除,
 *                其红态在 builder 接线进 host runner 时兑现 (文件头注记, 如实记录)
 *   coverage: RMD-A5 (不叠屏 = 「先清屏」对象级具名化, tighten ADD 2026-09-08)
 *             -> test RMD-A5-render-tighten (host-runner-only: 测试自建纯净基线后对比
 *                「只渲染 B」与「先渲染 A 再渲染 B」的活动屏子对象数 + 重复渲染不增长 +
 *                两路径最终帧一致; 钉同一既有意图, 不新增行为定义 — 补 mutation
 *                sensitive:false 缺口, 详见文件尾 REVISION LOG)
 *   coverage: RMD-A7 (断网无提醒 = 设备不缓存事项) -> test RMD-A7 (设备零本地事项源:
 *                不读 schedule.md / 不读 pending.json / 不读语义产物 + 三档阶梯常量
 *                (20:30 / 07:45 / remind_before_min) 永不进固件面 = 纯服务端配置永不烧板子)
 *   coverage: RMD-A8 (契约零变更边界) + scanner 自检 (防假绿)
 *                -> test boundary (提醒载荷内嵌 view 只能用 §1.3 白名单, 设备不得发明
 *                   "reminder" view -> VRT_ROUTE_UNKNOWN; 扫描器四态自检 + 计数器校验)
 *   coverage: RMD-A1/A2 (tighten ADD 2026-09-08, 同源审计)
 *             -> test RMD-A1-tighten-degrade-marker (提醒模块必须带**显式**降级标记
 *                降级/degrade/fallback/友好提示; 补既有 structural 齿 "-1" 逃逸口的无鉴别力
 *                —— 与 .v3 carrier 宽词齿同一类假绿, 见文件尾 REVISION LOG)
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.2]/[C4.4]):
 *   0) builder wiring: add "../../schedule_reminder_b.acceptance.c" AND the reminder page
 *      implementation source to HOST_ACCEPTANCE_SRCS in
 *      firmware/main/test/host_app/main/CMakeLists.txt, keeping
 *      "../../host_infra_smoke.acceptance.c" LAST in that list.
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *      (注意: 本 host_app 是独立工程, 直接 build; idf.py 的 -T main 是设备工程用法)
 *   2) cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
 *   Every TEST_CASE must execute and pass; the runner must exit 0.
 *   Freeze-phase red baseline (this round) used the in-scope standalone stub per
 *   firmware/main/test/acceptance/red-baseline/ — see
 *   delivery/taskpackages/260829-schedule-reminder/red-baseline-summary.md.
 * ---------------------------------------------------------------------------
 * Red-baseline 判读口径 (2026-09-08 acceptance-author 实测, 与 planner 更正一致):
 *   main 的 host ELF 基线 = 60 Tests / 3 Failures, 三条既有红全是同族 [evidence] 齿
 *   (VRT-A2/E2 回环证据记录缺失、WSA-A6 ecs-teardown-evidence.md 缺失、WSA-A4 flash.log
 *   缺失; 本地-only gitignore 证据从未入仓 -> main 本身即红)。本包判读 = 「3 条既有红
 *   不变 + 本包新增齿全红」。本文件的 structural/守卫齿 (RMD-A1 的底座守卫部分、
 *   boundary 的扫描器自检部分) 属必须保持绿的守卫, 不算新增行为红。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <assert.h>

/* 包 2 / 包 3 / 包 6 的共享底座头（相对路径, host runner 与独立 stub 编译都可达）。 */
#include "../key_gesture_parser.h"
#include "../display/view_router.h"

#define KRM_MAX_PATH 1024
#define KRM_MAX_READ (8u * 1024u * 1024u)

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — constants + weak defaults.
 * ------------------------------------------------------------------------ */
#define KRM_UNIMPLEMENTED (-2) /* weak-default sentinel — never a pass */

#define KRM_QUEUE_CAP 8
#define KRM_KNOWN_CAP 32

/* 四态送达语义 (spec S-F1.4 V14 解耦口径) */
enum {
    KRM_STATE_RECEIVED = 0,   /* 接收即入 known-id 集 (屏蔽重复入队) */
    KRM_STATE_QUEUED = 1,     /* 新 id: 入集且入队 */
    KRM_STATE_RENDERED = 2,   /* 已渲染驻留项被重推 -> 补回回执 */
    KRM_STATE_ACKED = 3       /* 已回执出队 */
};
#define KRM_STATE_INVALID (-1)

/* 按键裁决 */
enum { KRM_KEY_DISMISS = 0, KRM_KEY_PASS = 1 };

/* 三件套动作集 */
enum { KRM_ACT_SHOW = 1, KRM_ACT_TONE = 2, KRM_ACT_SPEAK = 4 };

typedef struct {
    char id[40];
    char fire_at[32];
    char title[64];
    char speech[96];
    char view_json[256];
} kidpal_alert_t;

__attribute__((weak)) int kidpal_alert_parse(const char *args_json, kidpal_alert_t *out)
{
    (void)args_json; (void)out;
    return KRM_UNIMPLEMENTED;
}

__attribute__((weak)) long long kidpal_fire_at_minutes(const char *fire_at_iso)
{
    (void)fire_at_iso;
    return KRM_UNIMPLEMENTED;
}

/* 冻结的**不透明**状态类型：实现方必须以标签 `struct kidpal_reminder_s` 定义其内部布局
 * （本冻结件只按指针使用它，不依赖字段顺序 —— 这是「验收不锁内部、只锁行为」的写法）。 */
typedef struct kidpal_reminder_s kidpal_reminder_t;

__attribute__((weak)) void kidpal_reminder_init(kidpal_reminder_t *s) { (void)s; }
__attribute__((weak)) int kidpal_reminder_receive(kidpal_reminder_t *s, const kidpal_alert_t *a)
{
    (void)s; (void)a;
    return KRM_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_reminder_known_count(const kidpal_reminder_t *s) { (void)s; return KRM_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_reminder_queue_count(const kidpal_reminder_t *s) { (void)s; return KRM_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_reminder_shown_id(const kidpal_reminder_t *s, char *out, size_t cap)
{
    (void)s; (void)out; (void)cap;
    return KRM_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_reminder_raise_next(kidpal_reminder_t *s, long long now_min)
{
    (void)s; (void)now_min;
    return KRM_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_reminder_may_emit_receipt(const kidpal_reminder_t *s) { (void)s; return KRM_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_reminder_mark_rendered(kidpal_reminder_t *s, const char *id) { (void)s; (void)id; return KRM_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_reminder_mark_acked(kidpal_reminder_t *s, const char *id) { (void)s; (void)id; return KRM_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_reminder_render_actions(const kidpal_alert_t *a) { (void)a; return KRM_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_reminder_key(kidpal_reminder_t *s, key_id_t key, key_gesture_t g,
                                             page_id_t current, page_id_t *page_out)
{
    (void)s; (void)key; (void)g; (void)current; (void)page_out;
    return KRM_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_reminder_page_render(const kidpal_alert_t *a) { (void)a; return KRM_UNIMPLEMENTED; }

/* 包 2 底座的真实强符号（host runner 里由 view_router.c 提供；独立 stub 编译时未链接
 * —— 以 weak 指针判空跳过，见 boundary 测试内注记）。 */
extern __attribute__((weak)) int kidpal_display_route(const char *view);

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers (host-side structural teeth), KSP/VRT precedent.
 * All scans run against the REAL working tree located from the runner cwd.
 * Directories named test / host_app / build / .git / assets are EXCLUDED from
 * every scan (test/ hosts THIS frozen file whose comments legitimately mention
 * the frozen needles; host_app/build are build artifacts; assets are binary
 * art, not implementation surface).
 * ------------------------------------------------------------------------ */

static char krm_root[KRM_MAX_PATH];
static int krm_root_ready;

static int krm_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int krm_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int krm_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int krm_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KRM_MAX_PATH];

    if (krm_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (krm_is_regular(probe)) {
            snprintf(krm_root, sizeof(krm_root), "%s", cands[i]);
            krm_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int krm_rel(char *out, size_t cap, const char *rel)
{
    if (!krm_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", krm_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", krm_root, rel);
    return 1;
}

static int krm_exists(const char *rel)
{
    char p[KRM_MAX_PATH];
    return krm_rel(p, sizeof(p), rel) && krm_is_regular(p);
}

static char *krm_read(const char *rel, size_t *out_len)
{
    char p[KRM_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!krm_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KRM_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* logic guard: host OOM must fail loudly, not read as "red" */
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        free(buf);
        return NULL;
    }
    buf[sz] = '\0';
    fclose(f);
    if (out_len != NULL) {
        *out_len = (size_t)sz;
    }
    return buf;
}

static int krm_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int krm_contains_ci(const char *hay, const char *needle)
{
    size_t n;

    assert(hay != NULL && needle != NULL);
    n = strlen(needle);
    if (n == 0) {
        return 1;
    }
    for (const char *h = hay; *h != '\0'; h++) {
        size_t i = 0;
        while (i < n && h[i] != '\0' &&
               tolower((unsigned char)h[i]) == tolower((unsigned char)needle[i])) {
            i++;
        }
        if (i == n) {
            return 1;
        }
    }
    return 0;
}

static int krm_file_contains(const char *rel, const char *needle)
{
    char *buf = krm_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = krm_contains(buf, needle);
    free(buf);
    return r;
}

static int krm_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = krm_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = buf; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    free(buf);
    return count;
}

/* Locate the FIRST occurrence of needle inside abs_dir (recursive) and report the
 * relative path; also return the number of matching FILES. */
static int krm_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
                                       const char *needle, int ci,
                                       char *first_hit, size_t cap, int depth)
{
    DIR *d;
    struct dirent *e;
    int count = 0;

    if (depth > 10) {
        return 0;
    }
    d = opendir(abs_dir);
    if (d == NULL) {
        return 0;
    }
    while ((e = readdir(d)) != NULL) {
        char abs_next[KRM_MAX_PATH];
        char rel_next[KRM_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            krm_is_skipped_dir(e->d_name)) {
            continue;
        }
        snprintf(abs_next, sizeof(abs_next), "%s/%s", abs_dir, e->d_name);
        if (rel_prefix[0] != '\0') {
            snprintf(rel_next, sizeof(rel_next), "%s/%s", rel_prefix, e->d_name);
        } else {
            snprintf(rel_next, sizeof(rel_next), "%s", e->d_name);
        }
        if (stat(abs_next, &st) != 0) {
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            count += krm_count_files_with_needle(abs_next, rel_next, needle, ci,
                                                 first_hit, cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > KRM_MAX_READ) {
                continue;
            }
            FILE *f = fopen(abs_next, "rb");
            if (f == NULL) {
                continue;
            }
            char *buf = (char *)malloc((size_t)st.st_size + 1);
            assert(buf != NULL);
            if (fread(buf, 1, (size_t)st.st_size, f) == (size_t)st.st_size) {
                buf[st.st_size] = '\0';
                int m = ci ? krm_contains_ci(buf, needle) : krm_contains(buf, needle);
                if (m) {
                    if (count == 0 && first_hit != NULL) {
                        snprintf(first_hit, cap, "%s", rel_next);
                    }
                    count++;
                }
            }
            free(buf);
            fclose(f);
        }
    }
    closedir(d);
    return count;
}

static int krm_scan_count(const char *rel_dir, const char *needle, int ci,
                          char *first_hit, size_t cap)
{
    char abs[KRM_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!krm_rel(abs, sizeof(abs), rel_dir) || !krm_is_dir(abs)) {
        return 0;
    }
    return krm_count_files_with_needle(abs, rel_dir, needle, ci, first_hit, cap, 0);
}

#define KRM_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(krm_locate_root(), \
    "KRM RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* Shared red-string for the frozen offline contract not being implemented yet. */
static void krm_require_implemented(int value, const char *what)
{
    if (value == KRM_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "KRM RED: frozen offline contract NOT implemented — %s "
                 "(builder must land the pure-logic 提醒弹窗/四态状态机 per the frozen "
                 "contract in firmware/main/test/schedule_reminder_b.acceptance.c header "
                 "and wire it into HOST_ACCEPTANCE_SRCS)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* Canonical kidpal/alert tools/call ARGUMENTS object as it reaches the device
 * (契约 §三 载荷 + §四 mcp 工具调用; view 以 JSON 字符串承载, 与 kidpal/display 的
 * data 字符串化同纪律). 业务幂等 id 是字符串 (§三), 与数字 JSON-RPC id 分处。 */
static const char *KRM_ARGS_CANONICAL =
    "{\"id\":\"rmd-3f2a1b9c4d5e6f70\","
    "\"fire_at\":\"2026-09-09T16:15:00+08:00\","
    "\"title\":\"明天钢琴课 16:30 开始前\","
    "\"speech\":\"16:30 有钢琴课，别忘了带琴谱！\","
    "\"view\":\"{\\\"view\\\":\\\"text\\\",\\\"data\\\":{\\\"text\\\":\\\"16:30 钢琴课\\\"}}\"}";

static const char *KRM_ARGS_NO_SPEECH =
    "{\"id\":\"rmd-aaaa0000bbbb1111\",\"fire_at\":\"2026-09-08T20:30:00+08:00\","
    "\"title\":\"明日美术班\",\"view\":\"{\\\"view\\\":\\\"text\\\",\\\"data\\\":{\\\"text\\\":\\\"10:00 美术班\\\"}}\"}";

static void krm_fill(kidpal_alert_t *a, const char *id, const char *fire_at, const char *speech)
{
    memset(a, 0, sizeof(*a));
    snprintf(a->id, sizeof(a->id), "%s", id);
    snprintf(a->fire_at, sizeof(a->fire_at), "%s", fire_at);
    snprintf(a->title, sizeof(a->title), "%s", id); /* title 不参与判定, 非空即可 */
    if (speech != NULL) {
        snprintf(a->speech, sizeof(a->speech), "%s", speech);
    }
}

/* -------------------------------------------------------------------------- *
 * RMD-A1 (structural) — 提醒设备消费面装配（coverage: RMD-A2 device face 装配）
 * Teeth, each a REAL repo fact:
 *   1) a reminder module exists under firmware/main implementing the frozen contract
 *      (kidpal_reminder_receive( — today NOTHING on the device consumes kidpal/alert);
 *   2) the module is compiled into the DEVICE build (CMakeLists assembly fact);
 *   3) mcp_server.cc registers the kidpal/alert tool (契约 §四 mcp 工具调用) AND carries
 *      an alert route ack (today: only kidpal/display exists);
 *   4) the popup render face is REACHED: >= 2 distinct files reference
 *      kidpal_reminder_page_render( (definition + the alert handler call site) —
 *      defined-but-never-called fails this tooth;
 *   5) the 渲染才允许回执 gate is actually consulted by the consumer: >= 2 distinct
 *      files reference kidpal_reminder_may_emit_receipt( — this is the machine-checkable
 *      landing point of 「接收时不回回执、渲染完成才回执」 across the sync-tool-reply
 *      shape of DoToolCall (mcp_server.cc:730-739) and the deferred ReplyResult face
 *      (mcp_server.cc:603 / application.cc:906);
 *   6) 三件套的提示音面在世 (application.cc PlaySound 用法先例): 提醒模块内出现
 *      PlaySound / OGG_POPUP 引用;
 *   7) 解析失败 = 显式降级语义标记 (V27 纪律在 alert 载荷形状上)。
 * guards (must stay green): 包 2 view_router 底座在世 + kidpal/display 注册在世 +
 *   包 6 schedule_page 渲染面未被本包改坏。
 * RED today: teeth 1)-7) have no implementation; guards green (native).
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule reminder b: 提醒设备消费面装配结构 (RMD-A1)", "[schedule_reminder][RMD-A1][structural]")
{
    char hit[KRM_MAX_PATH];
    int n_recv, n_render, n_gate;

    KRM_REQUIRE_ROOT();

    /* tooth 1: 提醒弹窗/状态机模块存在于设备树（优先看 display/ —— 包 2/5/6/8 的共享
     * 显示底座都在这里，是本冻结契约的建议家；但不锁死位置，全树兜底，两处都零命中
     * 才是真缺失。这样后续齿 2/7 拿到的 hit 就是模块文件本身，判定更精确）。 */
    n_recv = krm_scan_count("firmware/main/display", "kidpal_reminder_receive(", 0,
                            hit, sizeof(hit));
    if (n_recv == 0) {
        n_recv = krm_scan_count("firmware/main", "kidpal_reminder_receive(", 0,
                                hit, sizeof(hit));
    }
    TEST_ASSERT_TRUE_MESSAGE(n_recv >= 1,
        "KRM-A1 RED: no module under firmware/main implements the frozen reminder contract "
        "(kidpal_reminder_receive absent) — state-reminder 设备面零消费, kidpal/alert 到达即"
        "无处可去 (契约 §三 / §四, spec S-F1.4)");

    /* tooth 2: 模块被设备构建引用（CMakeLists 装配，非浮文件） */
    {
        char base[KRM_MAX_PATH];
        const char *slash = strrchr(hit, '/');
        snprintf(base, sizeof(base), "%s", slash ? slash + 1 : hit);
        TEST_ASSERT_TRUE_MESSAGE(krm_file_contains("firmware/main/CMakeLists.txt", base),
            "KRM-A1 RED: the reminder module source is not referenced by "
            "firmware/main/CMakeLists.txt — 模块未编入设备构建（装配缺失）");
    }

    /* tooth 3: mcp_server.cc 注册 kidpal/alert 工具并路由到弹窗面 */
    TEST_ASSERT_TRUE_MESSAGE(krm_file_contains("firmware/main/mcp_server.cc", "kidpal/alert"),
        "KRM-A1 RED: mcp_server.cc registers no kidpal/alert tool — 契约 §四「mcp 工具调用云→端"
        "两种载荷」的设备侧一半不在世, 提醒推送到设备即被 McpServer 按未知工具拒收");
    TEST_ASSERT_TRUE_MESSAGE(krm_file_contains("firmware/main/mcp_server.cc", "kidpal_reminder"),
        "KRM-A1 RED: the kidpal/alert handler never reaches the reminder face "
        "(mcp_server.cc has no kidpal_reminder* reference) — 注册与消费未接线");

    /* tooth 4: 渲染面定义 + 消费调用点 >= 2 个文件 */
    n_render = krm_scan_count("firmware/main", "kidpal_reminder_page_render(", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n_render >= 2,
        "KRM-A1 RED: kidpal_reminder_page_render is defined but never called — 弹窗渲染面未接入 "
        "kidpal/alert 消费链路（仅定义不调用不算接入, RMD-A5 三件套无落点）");

    /* tooth 5: 「渲染才允许发回执」的判定门被消费方真查询 */
    n_gate = krm_scan_count("firmware/main", "kidpal_reminder_may_emit_receipt(", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n_gate >= 2,
        "KRM-A1 RED: kidpal_reminder_may_emit_receipt is defined but never consulted — "
        "「接收时不回执、渲染完成才回执」(RMD-A6 / V14 解耦口径) 在设备侧没有落点; "
        "DoToolCall 的同步自动回复 (mcp_server.cc:730-739) 与延迟 ReplyResult "
        "(mcp_server.cc:603) 之间必须有这道判定门");

    /* tooth 6: 三件套的提示音面在提醒模块内 */
    {
        char render_hit[KRM_MAX_PATH];
        char *buf;
        (void)krm_scan_count("firmware/main", "kidpal_reminder_page_render(", 0,
                             render_hit, sizeof(render_hit));
        if (render_hit[0] == '\0') {
            snprintf(render_hit, sizeof(render_hit), "firmware/main/display/reminder_page.c");
        }
        buf = krm_read(render_hit, NULL);
        TEST_ASSERT_NOT_NULL_MESSAGE(buf, "KRM-A1 RED: reminder render module unreadable");
        TEST_ASSERT_TRUE_MESSAGE(
            krm_contains(buf, "PlaySound") || krm_contains(buf, "OGG_POPUP"),
            "KRM-A1 RED: the reminder face carries no 提示音 call (PlaySound/OGG_POPUP) — "
            "三件套「提示音 + TTS + 弹窗」的提示音一半缺失 (决策 27, RMD-A3 必达面)");
        free(buf);
    }

    /* tooth 7: 载荷解析失败 = 显式降级语义标记（绝不盲渲染） */
    {
        char *buf = krm_read(hit, NULL);
        TEST_ASSERT_NOT_NULL_MESSAGE(buf, "KRM-A1 RED: reminder module file unreadable");
        TEST_ASSERT_TRUE_MESSAGE(
            krm_contains(buf, "降级") || krm_contains_ci(buf, "degrade") ||
            strstr(buf, "-1") != NULL,
            "KRM-A1 RED: the reminder module carries no parse-degrade semantic marker — "
            "畸形 alert 载荷必须显式降级而非盲渲染 (V27 / spec §4.0)");
        free(buf);
    }

    /* guards (must stay green): 包 2 / 包 6 底座不被本包改坏 */
    TEST_ASSERT_TRUE_MESSAGE(krm_file_contains("firmware/main/mcp_server.cc",
                                               "display/view_router.h"),
        "KRM-A1 RED(infra): mcp_server.cc no longer includes display/view_router.h — "
        "包 2 路由底座被移除");
    TEST_ASSERT_TRUE_MESSAGE(krm_file_contains("firmware/main/CMakeLists.txt", "view_router.c"),
        "KRM-A1 RED(infra): view_router.c no longer compiled into the device build — "
        "包 2 底座装配被破坏");
    TEST_ASSERT_TRUE_MESSAGE(krm_file_contains("firmware/main/mcp_server.cc", "kidpal/display"),
        "KRM-A1 RED(infra): kidpal/display tool no longer registered in mcp_server.cc — "
        "display 下发到达面被破坏（本包只做 additive 扩展）");
    TEST_ASSERT_TRUE_MESSAGE(krm_file_contains("firmware/main/CMakeLists.txt", "schedule_page.c"),
        "KRM-A1 RED(infra): schedule_page.c no longer compiled into the device build — "
        "包 6 P2 渲染面被本包改坏");
}

/* -------------------------------------------------------------------------- *
 * upstream-integrity guard (RMD-A1 guard) — 既有装配完整性缺陷，非本包缺失行为
 * 实测: main 上 firmware/main/mcp_server.cc 的 kidpal/display case 4 (VRT_ROUTE_SCHEDULE)
 * 内 app.Schedule([...]() { kidpal_schedule_page_render(...) ... 的 lambda **闭合被
 * 260829-char-stroke merge 覆盖丢失**（205faa8 有 "}); / break; / }" 三行, f2a67dd 后
 * 直接接 case 3:），设备构建 (idf.py build / firmware-build-check.mjs) 必红。本齿把它钉
 * 成可机判的装配完整性守卫：case 4 分支必须在进入下一个 case 标签前闭合 lambda 并 break。
 * 已回报 planner（可另行派 fixer，或本包 builder 顺手补回）。
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule reminder b: 上游 mcp display case 装配完整性守卫 (RMD-A1 guard)",
          "[schedule_reminder][RMD-A1][upstream-integrity]")
{
    char *buf;
    const char *p_render, *p_case3;

    KRM_REQUIRE_ROOT();
    buf = krm_read("firmware/main/mcp_server.cc", NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "guard RED(infra): mcp_server.cc unreadable");
    p_render = strstr(buf, "kidpal_schedule_page_render(");
    TEST_ASSERT_TRUE_MESSAGE(p_render != NULL,
        "guard RED(infra): kidpal_schedule_page_render no longer called in mcp_server.cc — "
        "包 6 P2 消费面被移除（守卫前提失效）");
    p_case3 = strstr(p_render, "case 3:");
    TEST_ASSERT_TRUE_MESSAGE(p_case3 != NULL,
        "guard RED: no case 3 label after the schedule render call — 无法核对 case 4 闭合");
    {
        size_t span = (size_t)(p_case3 - p_render);
        char *slice = (char *)malloc(span + 1);
        assert(slice != NULL);
        memcpy(slice, p_render, span);
        slice[span] = '\0';
        TEST_ASSERT_TRUE_MESSAGE(strstr(slice, "});") != NULL,
            "guard RED: the kidpal/display case-4 app.Schedule( lambda is NOT closed before "
            "the next case label — mcp_server.cc 语法不合法, 设备构建必红 (char-stroke merge "
            "覆盖丢失的「}); / break; / }」三行, 已回报 planner)");
        TEST_ASSERT_TRUE_MESSAGE(strstr(slice, "break;") != NULL,
            "guard RED: the kidpal/display case-4 branch never breaks before case 3 — "
            "switch fall-through, schedule 分支会掉进 char 分支 (上游装配完整性)");
        free(slice);
    }
    free(buf);
}

/* -------------------------------------------------------------------------- *
 * RMD-A2 / RMD-A4 (device face) — kidpal/alert 载荷解析与降级 + fire_at 分钟数
 * (behavioral, frozen contract)
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule reminder b: kidpal/alert 载荷解析降级与 fire_at 归一 (RMD-A2/A4)",
          "[schedule_reminder][RMD-A2][RMD-A4]")
{
    kidpal_alert_t a;
    int r;
    long long m;

    /* canonical payload: 逐字段保真 (契约 §三 {id, fire_at, title, speech, view}) */
    memset(&a, 0, sizeof(a));
    r = kidpal_alert_parse(KRM_ARGS_CANONICAL, &a);
    krm_require_implemented(r, "kidpal_alert_parse unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "KRM-A2 RED: canonical 契约 §三 payload did not parse (must return 0)");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(a.id, "rmd-3f2a1b9c4d5e6f70") == 0,
        "KRM-A2 RED: 业务幂等 id 未逐字段保真 — 易失去重与回执关联全靠这个 id (V15)");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(a.fire_at, "2026-09-09T16:15:00+08:00") == 0,
        "KRM-A2 RED: fire_at not kept verbatim (ISO8601+08:00 契约 §三)");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(a.title, "明天钢琴课 16:30 开始前") == 0,
        "KRM-A4 RED: title 字段丢失/变形 — 大字事项是弹窗正文 (RMD-A5)");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(a.speech, "16:30 有钢琴课，别忘了带琴谱！") == 0,
        "KRM-A3 RED: speech 未保真 — TTS 读文案的来源字段 (决策 27 三件套)");
    /* view rides as a JSON STRING on the mcp wire (Property types are bool/int/string
     * only) — it must be preserved byte-for-byte, not silently dropped. */
    TEST_ASSERT_TRUE_MESSAGE(krm_contains(a.view_json, "\\\"view\\\":\\\"text\\\"") ||
                                 krm_contains(a.view_json, "\"view\":\"text\""),
        "KRM-A4 RED: the nested view block was lost or re-shaped — 载荷内嵌 display 块必须"
        "原样保真交给 §1.3 白名单路由 (契约 §三/§四)");

    /* tolerant: missing speech -> "" (合成失败/缺文案降级的设备侧一半) */
    memset(&a, 0, sizeof(a));
    r = kidpal_alert_parse(KRM_ARGS_NO_SPEECH, &a);
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "KRM-A3 RED: a payload without speech must still parse (语音是可选增强, "
        "缺省不得拒绝整条提醒)");
    TEST_ASSERT_TRUE_MESSAGE(a.speech[0] == '\0',
        "KRM-A3 RED: missing speech must parse as empty string (降级标记给 render_actions)");

    /* tolerant: missing view -> ""; extra unknown fields ignored */
    r = kidpal_alert_parse("{\"id\":\"x1\",\"fire_at\":\"2026-09-09T07:45:00+08:00\","
                           "\"title\":\"今日日程\",\"extra\":7}", &a);
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "KRM-A2 RED: payload with an unknown extra field / missing view must parse "
        "(additive tolerance, 契约 data 形状不锁死扩展位)");
    TEST_ASSERT_TRUE_MESSAGE(a.view_json[0] == '\0',
        "KRM-A2 RED: missing view must parse as empty string");

    /* degrade: 畸形/缺必需字段 -> -1 (绝不入队、绝不盲渲染) */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_alert_parse("{\"id\":", &a) == -1,
        "KRM-A2 RED: malformed JSON must return -1 (degrade), not 0/partial");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_alert_parse(NULL, &a) == -1,
        "KRM-A2 RED: NULL payload must be defended (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_alert_parse(KRM_ARGS_CANONICAL, NULL) == -1,
        "KRM-A2 RED: NULL out must be defended (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_alert_parse(
        "{\"fire_at\":\"2026-09-09T07:45:00+08:00\",\"title\":\"缺 id\"}", &a) == -1,
        "KRM-A2 RED: a payload WITHOUT id must be rejected (-1) — 无 id 无法幂等去重与"
        "回执关联 (V15/契约 §三 id 是必带字段)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_alert_parse(
        "{\"id\":\"\",\"fire_at\":\"2026-09-09T07:45:00+08:00\",\"title\":\"空 id\"}", &a) == -1,
        "KRM-A2 RED: an empty-string id must be rejected (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_alert_parse(
        "{\"id\":\"x2\",\"fire_at\":\"2026-09-09T07:45:00+08:00\"}", &a) == -1,
        "KRM-A2 RED: a payload without title must be rejected (-1) — 弹窗大字是必带内容");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_alert_parse(
        "{\"id\":\"x3\",\"fire_at\":\"昨天下午\",\"title\":\"坏时刻\"}", &a) == -1,
        "KRM-A2 RED: an unparseable fire_at must be rejected (-1) — V18 顶替判定依赖它");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_alert_parse(
        "{\"id\":7,\"fire_at\":\"2026-09-09T07:45:00+08:00\",\"title\":\"id 非串\"}", &a) == -1,
        "KRM-A2 RED: a non-string id must be rejected (-1) — §三 业务 id 是字符串 token "
        "(数字 JSON-RPC 传输 id 是另一件事, 不得混用)");

    /* safe truncation: over-long fields never overflow, always NUL-terminated */
    {
        char big[512];
        int i;
        for (i = 0; i < 300; i++) {
            big[i] = 'a';
        }
        big[300] = '\0';
        {
            char *p = (char *)malloc(strlen(big) + 96);
            assert(p != NULL);
            sprintf(p, "{\"id\":\"%s\",\"fire_at\":\"2026-09-09T07:45:00+08:00\","
                       "\"title\":\"t\"}", big);
            memset(&a, 0, sizeof(a));
            r = kidpal_alert_parse(p, &a);
            TEST_ASSERT_TRUE_MESSAGE(r == 0,
                "KRM-A2 RED: an over-long id must be safe-truncated, not rejected outright");
            TEST_ASSERT_TRUE_MESSAGE(strlen(a.id) < sizeof(a.id),
                "KRM-A2 RED: field copy overflowed the destination buffer");
            free(p);
        }
    }

    /* fire_at -> 分钟数: 偏移归一 + 排序 + 畸形 -1 (V18「事件时刻过后」判定的算术底座) */
    m = kidpal_fire_at_minutes("2026-09-07T20:30:00+08:00");
    if (m == (long long)KRM_UNIMPLEMENTED) {
        krm_require_implemented((int)m, "kidpal_fire_at_minutes unimplemented");
    }
    TEST_ASSERT_TRUE_MESSAGE(m == 29813070LL,
        "KRM-A2 RED: fire_at minute resolution wrong for +08:00 (expected 29813070)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_fire_at_minutes("2026-09-07T12:30:00Z") == m,
        "KRM-A5 RED: the same instant written with a different UTC offset must compare "
        "EQUAL — V18 顶替判定按绝对时刻, 不按字面 (ISO8601 偏移归一)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_fire_at_minutes("2026-09-09T16:15:00+08:00") == 29815695LL,
        "KRM-A2 RED: 事前 15 分钟档 (16:30 开始) 的 fire_at 分钟数 != 29815695");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_fire_at_minutes("2026-09-09T07:45:00+08:00") == 29815185LL,
        "KRM-A2 RED: 07:45 早档的 fire_at 分钟数 != 29815185");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_fire_at_minutes("2026-09-09T16:15:00+08:00") <
            kidpal_fire_at_minutes("2026-09-09T16:30:00+08:00"),
        "KRM-A5 RED: fire_at ordering broken (later instant must be larger)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_fire_at_minutes(NULL) == -1,
        "KRM-A2 RED: NULL fire_at must be defended (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_fire_at_minutes("garbage") == -1,
        "KRM-A2 RED: a non-ISO fire_at string must be rejected (-1), never read as 0");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_fire_at_minutes("2026-09-09 16:15:00") == -1,
        "KRM-A2 RED: a fire_at without the 'T' separator / offset must be rejected (-1)");
}

/* -------------------------------------------------------------------------- *
 * RMD-A3 — 三件套动作集与 speech 降级（必达面, 决策 27 + V20）
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule reminder b: 三件套动作集与 speech 缺省降级 (RMD-A3)", "[schedule_reminder][RMD-A3]")
{
    kidpal_alert_t a;
    int act;

    krm_fill(&a, "rmd-1", "2026-09-09T16:15:00+08:00", "16:30 有钢琴课，别忘了带琴谱！");
    act = kidpal_reminder_render_actions(&a);
    krm_require_implemented(act, "kidpal_reminder_render_actions unimplemented");
    TEST_ASSERT_TRUE_MESSAGE((act & KRM_ACT_SHOW) != 0,
        "KRM-A3 RED: 弹窗不在动作集里 — 三件套的驻留弹窗是必达项 (决策 27)");
    TEST_ASSERT_TRUE_MESSAGE((act & KRM_ACT_TONE) != 0,
        "KRM-A3 RED: 提示音不在动作集里 — 三件套的提示音是必达项 (决策 27)");
    TEST_ASSERT_TRUE_MESSAGE((act & KRM_ACT_SPEAK) != 0,
        "KRM-A3 RED: speech 非空却没要求 TTS 播报 — 孩子不看屏也能听到这条被削弱 "
        "(spec S-F1.4 主动播报)");

    /* speech 缺失/为空 = 合成失败降级的设备侧: 仍 SHOW|TONE, 不 SPEAK, 不拒绝 */
    krm_fill(&a, "rmd-2", "2026-09-08T20:30:00+08:00", "");
    act = kidpal_reminder_render_actions(&a);
    TEST_ASSERT_TRUE_MESSAGE((act & KRM_ACT_SPEAK) == 0,
        "KRM-A3 RED: 空 speech 仍要求播报 — 应降级为「仅弹窗+提示音」");
    TEST_ASSERT_TRUE_MESSAGE((act & (KRM_ACT_SHOW | KRM_ACT_TONE)) == (KRM_ACT_SHOW | KRM_ACT_TONE),
        "KRM-A3 RED: 语音缺失时弹窗+提示音不再必达 — 合成失败/超时不得阻塞投递 (V20)");

    /* NULL / 无 id = 非法载荷, 不给任何动作 (不盲投递) */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_render_actions(NULL) == -1,
        "KRM-A3 RED: NULL alert must be defended (-1), never a live action set");
    krm_fill(&a, "", "2026-09-08T20:30:00+08:00", "文案");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_render_actions(&a) == -1,
        "KRM-A3 RED: an id-less alert must yield -1 (幂等去重无锚, 不得投递)");
}

/* -------------------------------------------------------------------------- *
 * RMD-A5 — 排队逐个弹 + V18 滞留顶替（行为, 无超时驻留）
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
#define KRM_EV_1615 29815695LL   /* 2026-09-09T16:15+08 */
#define KRM_EV_1900 29815860LL   /* 2026-09-09T19:00+08 */
#define KRM_EV_0745 29815185LL   /* 2026-09-09T07:45+08 */

TEST_CASE("schedule reminder b: 排队逐个弹 + V18 滞留顶替 (RMD-A5)", "[schedule_reminder][RMD-A5]")
{
    /* 契约类型 kidpal_reminder_t 的内部布局由 builder 落在实现头里; 本测试通过 void 裸缓冲
     * + 指针转型持有实例, 以免冻结件依赖未定型的字段顺序。尺寸上界由 RMD-A6-ephemeral
     * 的静态守卫核。 */
    static unsigned char store[4096];
    kidpal_reminder_t *s = (kidpal_reminder_t *)store;
    kidpal_alert_t a;
    char shown[64];
    int r;

    memset(store, 0xA5, sizeof(store));
    kidpal_reminder_init(s);

    r = kidpal_reminder_known_count(s);
    krm_require_implemented(r, "kidpal_reminder_init/known_count unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0, "KRM-A5 RED: a fresh state must know zero ids");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_queue_count(s) == 0,
        "KRM-A5 RED: a fresh state must have an empty queue");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, shown, sizeof(shown)) == 0,
        "KRM-A5 RED: a fresh state must have no resident popup");

    /* 无弹窗时到一条 -> 入队; raise_next 才弹 (接收不等于渲染) */
    krm_fill(&a, "rmd-A", "2026-09-09T16:15:00+08:00", "A 文案");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &a) == KRM_STATE_QUEUED,
        "KRM-A5 RED: a first-time id must be recorded AND enqueued (KRM_STATE_QUEUED)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_queue_count(s) == 1,
        "KRM-A5 RED: the new id did not enter the queue");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, shown, sizeof(shown)) == 0,
        "KRM-A5 RED: 接收即弹窗 — 渲染必须走 raise_next (四态解耦被压平)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(s, KRM_EV_1615) == 1,
        "KRM-A5 RED: raise_next did not pop the queued popup");
    r = kidpal_reminder_shown_id(s, shown, sizeof(shown));
    TEST_ASSERT_TRUE_MESSAGE(r == 1 && strcmp(shown, "rmd-A") == 0,
        "KRM-A5 RED: the resident popup id is wrong after raise_next");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_queue_count(s) == 0,
        "KRM-A5 RED: the popped item stayed in the queue (双重投递风险)");

    /* 弹窗期间新提醒: 入队不叠屏 (未渲染队列不合并、不替换) */
    krm_fill(&a, "rmd-B", "2026-09-09T19:00:00+08:00", "B 文案");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &a) == KRM_STATE_QUEUED,
        "KRM-A5 RED: a new reminder arriving during a resident popup must still be queued");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, shown, sizeof(shown)) == 1 &&
                                 strcmp(shown, "rmd-A") == 0,
        "KRM-A5 RED: 新提醒到达即叠屏/替换 — 未渲染队列不得顶替当前弹窗 (spec S-F1.4)");

    /* 事件时刻未过 -> 不顶替 (rmd-A 的 fire_at 16:15, 此刻 16:00) */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(s, KRM_EV_1615 - 15) == 0,
        "KRM-A5 RED: 滞留弹窗在事件时刻之前就被顶替 — V18 只在事件时刻过后允许");

    /* A 渲染完成 + 回执出队, 但永未按键; 事件时刻过后 -> 允许 B 顶替 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_mark_rendered(s, "rmd-A") == 1,
        "KRM-A5 RED: mark_rendered on the resident popup failed");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_mark_acked(s, "rmd-A") == 1,
        "KRM-A5 RED: mark_acked on a rendered resident popup failed");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(s, KRM_EV_1615) == 2,
        "KRM-A5 RED: 事件时刻已过、永未按键的滞留弹窗没有被后续提醒顶替 (V18)");
    r = kidpal_reminder_shown_id(s, shown, sizeof(shown));
    TEST_ASSERT_TRUE_MESSAGE(r == 1 && strcmp(shown, "rmd-B") == 0,
        "KRM-A5 RED: the displacing popup is not rmd-B");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_known_count(s) == 2,
        "KRM-A5 RED: V18 顶替不得追加送达 — known 集必须仍是 2 项 (早已出队)");

    /* 顶替后 B 未渲染 -> 不再顶替 (不叠屏) */
    krm_fill(&a, "rmd-C", "2026-09-09T16:30:00+08:00", "C 文案");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &a) == KRM_STATE_QUEUED,
        "KRM-A5 RED: a third reminder must queue behind the resident one");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(s, KRM_EV_1900 + 60) == 0,
        "KRM-A5 RED: 未渲染完成的弹窗被顶替 — 顶替只允许发生在「已渲染且永未按键」的滞留弹窗");

    /* 关掉当前 -> 自动弹下一个 (FIFO 逐个弹) */
    {
        page_id_t nxt = PAGE_P1;
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_key(s, KEY_ID_KEY, KEY_GESTURE_CLICK,
                                                     PAGE_P2, &nxt) == KRM_KEY_DISMISS,
            "KRM-A5 RED: KEY click did not dismiss the popup");
    }
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(s, KRM_EV_1900 + 61) == 1,
        "KRM-A5 RED: 关闭当前弹窗后下一条提醒没有自动弹出 (排队逐个弹缺失)");
    r = kidpal_reminder_shown_id(s, shown, sizeof(shown));
    TEST_ASSERT_TRUE_MESSAGE(r == 1 && strcmp(shown, "rmd-C") == 0,
        "KRM-A5 RED: the FIFO order is broken (expected rmd-C next, got the wrong id)");

    /* 空队列 raise_next -> 0; NULL 防御 -> -1 */
    {
        static unsigned char empty[4096];
        kidpal_reminder_t *e = (kidpal_reminder_t *)empty;
        kidpal_reminder_init(e);
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(e, KRM_EV_0745) == 0,
            "KRM-A5 RED: raise_next on an empty queue must do nothing (0)");
    }
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(NULL, KRM_EV_0745) == -1,
        "KRM-A5 RED: NULL state must be defended (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(NULL, &a) == KRM_STATE_INVALID,
        "KRM-A5 RED: NULL state on receive must be defended, no state pollution");
    krm_fill(&a, "", "2026-09-09T16:15:00+08:00", "无 id");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &a) == KRM_STATE_INVALID,
        "KRM-A5 RED: an id-less alert must be refused (-1) and must not touch the queue");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_queue_count(s) == 0,
        "KRM-A5 RED: a refused receive mutated the queue (状态污染)");

    /* 队列深度有界: 灌爆不崩、不无限增长 */
    {
        static unsigned char of[4096];
        kidpal_reminder_t *o = (kidpal_reminder_t *)of;
        int i, accepted = 0;
        kidpal_reminder_init(o);
        for (i = 0; i < KRM_QUEUE_CAP + 6; i++) {
            char idb[24];
            snprintf(idb, sizeof(idb), "rmd-of-%d", i);
            krm_fill(&a, idb, "2026-09-09T16:15:00+08:00", "溢出项");
            if (kidpal_reminder_receive(o, &a) == KRM_STATE_QUEUED) {
                accepted++;
            }
        }
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_queue_count(o) <= KRM_QUEUE_CAP,
            "KRM-A5 RED: the queue grew past KRM_QUEUE_CAP — 有界性缺失 (溢出即内存踩踏风险)");
        TEST_ASSERT_TRUE_MESSAGE(accepted >= KRM_QUEUE_CAP,
            "KRM-A5 RED: the queue dropped items below its declared capacity");
    }
}

/* -------------------------------------------------------------------------- *
 * RMD-A5 — KEY 关闭回当前页 / BOOT 关闭照常翻页（与包 3 手势底座的接缝）
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule reminder b: KEY 关闭回当前页、BOOT 关闭照常翻页 (RMD-A5)",
          "[schedule_reminder][RMD-A5][gesture]")
{
    static unsigned char store[4096];
    kidpal_reminder_t *s = (kidpal_reminder_t *)store;
    kidpal_alert_t a;
    page_id_t out;
    char shown[64];

    memset(store, 0xA5, sizeof(store));
    kidpal_reminder_init(s);
    krm_fill(&a, "rmd-K", "2026-09-09T16:15:00+08:00", "文案");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &a) == KRM_STATE_QUEUED,
        "KRM-A5 RED: fixture receive did not enqueue");
    krm_require_implemented(kidpal_reminder_raise_next(s, KRM_EV_1615 - 100),
                            "kidpal_reminder_raise_next unimplemented");
    (void)kidpal_reminder_raise_next(s, KRM_EV_1615 - 100);
    out = PAGE_P3;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_key(s, KEY_ID_KEY, KEY_GESTURE_CLICK, PAGE_P3, &out)
                                 == KRM_KEY_DISMISS,
        "KRM-A5 RED: KEY 单击没有关闭驻留弹窗 (spec: KEY = 关闭回当前页)");
    TEST_ASSERT_TRUE_MESSAGE(out == PAGE_P3,
        "KRM-A5 RED: KEY 关闭后跳了页 — 必须回当前页 (不改变孩子正在看的页)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, shown, sizeof(shown)) == 0,
        "KRM-A5 RED: 关闭后弹窗仍驻留");

    /* BOOT 关闭 -> 照常翻页 (P1->P2, 页序取包 3 底座 key_page_next 单源) */
    krm_fill(&a, "rmd-B2", "2026-09-09T16:15:00+08:00", "文案");
    (void)kidpal_reminder_receive(s, &a);
    (void)kidpal_reminder_raise_next(s, KRM_EV_1615);
    out = PAGE_P1;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_key(s, KEY_ID_BOOT, KEY_GESTURE_CLICK, PAGE_P1, &out)
                                 == KRM_KEY_DISMISS,
        "KRM-A5 RED: BOOT 单击没有关闭弹窗");
    TEST_ASSERT_TRUE_MESSAGE(out == PAGE_P2,
        "KRM-A5 RED: BOOT 关闭没有照常翻页 (P1 应到 P2) — 「翻页不被打断」手势被吞");

    /* P4 -> P1 回绕 (与包 3 key_page_next 页序接缝一致) */
    krm_fill(&a, "rmd-B3", "2026-09-09T16:15:00+08:00", "文案");
    (void)kidpal_reminder_receive(s, &a);
    (void)kidpal_reminder_raise_next(s, KRM_EV_1615);
    out = PAGE_P2;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_key(s, KEY_ID_BOOT, KEY_GESTURE_CLICK, PAGE_P4, &out)
                                 == KRM_KEY_DISMISS,
        "KRM-A5 RED: BOOT click on P4 did not dismiss");
    TEST_ASSERT_TRUE_MESSAGE(out == PAGE_P1,
        "KRM-A5 RED: P4 的 BOOT 翻页没有回绕到 P1 — 页序与包 3 底座 key_page_next 分叉");

    /* KEY 长按 = 关闭 + 全局回首页 (spec §4.0 所有页面一致) */
    krm_fill(&a, "rmd-B4", "2026-09-09T16:15:00+08:00", "文案");
    (void)kidpal_reminder_receive(s, &a);
    (void)kidpal_reminder_raise_next(s, KRM_EV_1615);
    out = PAGE_P3;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_key(s, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS, PAGE_P3, &out)
                                 == KRM_KEY_DISMISS,
        "KRM-A5 RED: KEY 长按没有关闭弹窗");
    TEST_ASSERT_TRUE_MESSAGE(out == PAGE_P1,
        "KRM-A5 RED: KEY 长按没有回首页 — 全局手势被弹窗吞掉会破坏「所有页面一致」");

    /* 其余手势不吞 (弹窗驻留、页不变) */
    krm_fill(&a, "rmd-B5", "2026-09-09T16:15:00+08:00", "文案");
    (void)kidpal_reminder_receive(s, &a);
    (void)kidpal_reminder_raise_next(s, KRM_EV_1615);
    {
        static const struct {
            key_id_t k;
            key_gesture_t g;
        } pass[] = {
            { KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS },
            { KEY_ID_BOOT, KEY_GESTURE_DOUBLE_CLICK },
            { KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK },
            { KEY_ID_COUNT, KEY_GESTURE_CLICK },   /* 越界键位 (PWR 模拟) */
        };
        size_t i;
        for (i = 0; i < sizeof(pass) / sizeof(pass[0]); i++) {
            out = PAGE_P2;
            TEST_ASSERT_TRUE_MESSAGE(
                kidpal_reminder_key(s, pass[i].k, pass[i].g, PAGE_P2, &out) == KRM_KEY_PASS,
                "KRM-A5 RED: 弹窗吞掉了不属于它的手势 — 应 PASS 给下层并保持驻留 "
                "(spec 只裁 KEY=关闭回当前页 / BOOT=关闭并照常翻页)");
            TEST_ASSERT_TRUE_MESSAGE(out == PAGE_P2,
                "KRM-A5 RED: a passed-through gesture mutated the page");
            TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, shown, sizeof(shown)) == 1,
                "KRM-A5 RED: a passed-through gesture dismissed the popup");
        }
    }

    /* 无弹窗时按键 = 完全不介入 */
    {
        static unsigned char fresh[4096];
        kidpal_reminder_t *f2 = (kidpal_reminder_t *)fresh;
        kidpal_reminder_init(f2);
        out = PAGE_P2;
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_key(f2, KEY_ID_BOOT, KEY_GESTURE_CLICK,
                                                     PAGE_P2, &out) == KRM_KEY_PASS,
            "KRM-A5 RED: 无弹窗时弹窗层仍改了翻页语义 — 不得干预正常手势");
        TEST_ASSERT_TRUE_MESSAGE(out == PAGE_P2,
            "KRM-A5 RED: 无弹窗时页被改动");
    }
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_key(NULL, KEY_ID_KEY, KEY_GESTURE_CLICK,
                                                 PAGE_P1, &out) == -1,
        "KRM-A5 RED: NULL state must be defended (-1)");
}

/* -------------------------------------------------------------------------- *
 * RMD-A6 — 四态送达语义与「渲染才允许发回执」决策表 + 同 id 重推两态分处
 * (这是本包设备侧最承重的一条：工具回调同步回复形态 (DoToolCall) 与「渲染即确认」
 *  之间的边界, 用纯逻辑决策表钉死, 不把 C++ 线程时序编成断言)
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule reminder b: 四态决策表——渲染才允许回执 (RMD-A6)", "[schedule_reminder][RMD-A6]")
{
    static unsigned char store[4096];
    kidpal_reminder_t *s = (kidpal_reminder_t *)store;
    kidpal_alert_t a;
    int v;

    memset(store, 0xA5, sizeof(store));
    kidpal_reminder_init(s);
    krm_fill(&a, "rmd-S", "2026-09-09T16:15:00+08:00", "文案");

    /* 接收期: 一条回执都不许发 */
    v = kidpal_reminder_may_emit_receipt(s);
    krm_require_implemented(v, "kidpal_reminder_may_emit_receipt unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(v == 0,
        "KRM-A6 RED: 空状态就允许发回执 — 回执语义被压平");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &a) == KRM_STATE_QUEUED,
        "KRM-A6 RED: fixture receive did not enqueue");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_may_emit_receipt(s) == 0,
        "KRM-A6 RED: 接收即允许回执 — 「接收即入 known-id 集」与「渲染才回执」必须解耦 "
        "(V14 解耦口径, 契约 §三)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(s, KRM_EV_1615 - 60) == 1,
        "KRM-A6 RED: fixture raise_next did not pop");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_may_emit_receipt(s) == 0,
        "KRM-A6 RED: 弹窗尚未渲染完成就允许回执 — 送达确认必须晚于真渲染");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_mark_rendered(s, "rmd-nope") == 0,
        "KRM-A6 RED: mark_rendered accepted an id that is not the resident popup");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_may_emit_receipt(s) == 0,
        "KRM-A6 RED: a bogus mark_rendered armed the receipt (判定门可被伪造)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_mark_rendered(s, "rmd-S") == 1,
        "KRM-A6 RED: mark_rendered on the resident popup failed");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_may_emit_receipt(s) == 1,
        "KRM-A6 RED: 渲染完成后仍不允许发回执 — 渲染即确认缺失, 网关 60s 超时会重推三条"
        "已弹过的提醒 (RMD-A4 重试到确认打不通)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_mark_rendered(s, "rmd-S") == 1,
        "KRM-A6 RED: mark_rendered must be idempotent");
    /* 回执一次即出队, 判定门回落到 0 (不重复发) */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_mark_acked(s, "rmd-S") == 1,
        "KRM-A6 RED: mark_acked on the rendered popup failed");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_may_emit_receipt(s) == 0,
        "KRM-A6 RED: 回执发出后判定门仍开 — 会重复发回执");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_mark_acked(s, "rmd-S") == 0,
        "KRM-A6 RED: 同一 id 的回执被确认了两次");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_mark_acked(s, "rmd-other") == 0,
        "KRM-A6 RED: mark_acked accepted an unknown id");

    /* 同 id 重推 · 态一：排队未渲染 -> 不重复入队、不回执 */
    {
        static unsigned char store2[4096];
        kidpal_reminder_t *q = (kidpal_reminder_t *)store2;
        kidpal_reminder_init(q);
        krm_fill(&a, "rmd-Q", "2026-09-09T16:15:00+08:00", "文案");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(q, &a) == KRM_STATE_QUEUED,
            "KRM-A6 RED: fixture enqueue failed");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(q, &a) == KRM_STATE_RECEIVED,
            "KRM-A6 RED: 排队未渲染的重复推送必须走幂等去重 (RECEIVED), 不重复入队、不回执");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_queue_count(q) == 1,
            "KRM-A6 RED: 同 id 重推重复入队 — 关掉一个还会再弹一个 (重复弹窗根因)");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_may_emit_receipt(q) == 0,
            "KRM-A6 RED: 排队未渲染项被允许回执 — 两态分处失败");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_known_count(q) == 1,
            "KRM-A6 RED: known-id 集重复记账");
    }

    /* 同 id 重推 · 态二：已渲染驻留 -> 补回回执、且不重复入队 */
    {
        static unsigned char store3[4096];
        kidpal_reminder_t *r3 = (kidpal_reminder_t *)store3;
        char shown3[64];
        kidpal_reminder_init(r3);
        krm_fill(&a, "rmd-R", "2026-09-09T16:15:00+08:00", "文案");
        (void)kidpal_reminder_receive(r3, &a);
        (void)kidpal_reminder_raise_next(r3, KRM_EV_1615);
        (void)kidpal_reminder_mark_rendered(r3, "rmd-R");
        (void)kidpal_reminder_mark_acked(r3, "rmd-R");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_may_emit_receipt(r3) == 0,
            "KRM-A6 RED fixture drifted (receipt armed after ack)");
        v = kidpal_reminder_receive(r3, &a);
        TEST_ASSERT_TRUE_MESSAGE(v == KRM_STATE_RENDERED,
            "KRM-A6 RED: 已渲染的驻留项被重推应回 RENDERED 并补回执 (回执在途丢失时"
            "网关重推 = 唯一收敛路径, 重复弹窗必须根除)");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_may_emit_receipt(r3) == 1,
            "KRM-A6 RED: 补回回执没有被重新授权");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_queue_count(r3) == 0,
            "KRM-A6 RED: 补回执路径把已渲染项又塞回队列 (会二次弹窗)");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(r3, shown3, sizeof(shown3)) == 1,
            "KRM-A6 RED: 重推把驻留弹窗弄丢了");
    }

    /* 幂等去重跨整条生命周期：dismiss 后再推同一 id = 纯屏蔽, 不再回执也不入队 */
    {
        page_id_t out = PAGE_P1;
        char shown[64];
        (void)kidpal_reminder_mark_rendered(s, "rmd-S");
        out = PAGE_P1;
        (void)kidpal_reminder_key(s, KEY_ID_KEY, KEY_GESTURE_CLICK, PAGE_P1, &out);
        krm_fill(&a, "rmd-S", "2026-09-09T16:15:00+08:00", "文案");
        v = kidpal_reminder_receive(s, &a);
        TEST_ASSERT_TRUE_MESSAGE(v == KRM_STATE_RECEIVED,
            "KRM-A6 RED: 已出队且已关闭的 id 再推不得再入队/再回执 (known-id 屏蔽)");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_queue_count(s) == 0 &&
                                     kidpal_reminder_shown_id(s, shown, sizeof(shown)) == 0,
            "KRM-A6 RED: 屏蔽路径污染了队列/驻留态");
    }
}

/* -------------------------------------------------------------------------- *
 * RMD-A6 / RMD-A7 — 易失 known-id 集「只驻 RAM」(决策 24 介质禁令)
 * 行为面: re-init 即全忘 (无持久化可恢复) + 集合有界;
 * 结构面: 提醒模块零 NVS / 零文件系统 / 零动态分配 (可机判的介质禁令)。
 * RED today: weak-default UNIMPLEMENTED sentinel on the behavioral half.
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule reminder b: 易失 known-id 只驻 RAM 与介质禁令 (RMD-A6/A7)",
          "[schedule_reminder][RMD-A6][RMD-A7][security]")
{
    static unsigned char store[4096];
    kidpal_reminder_t *s = (kidpal_reminder_t *)store;
    kidpal_alert_t a;
    char hit[KRM_MAX_PATH];
    char sink[64];
    int n;

    /* 行为面: 记满 -> re-init -> 全忘, 且已知 id 再推被接受 (证明确无持久层) */
    memset(store, 0xA5, sizeof(store));
    kidpal_reminder_init(s);
    krm_fill(&a, "rmd-E1", "2026-09-09T16:15:00+08:00", "文案");
    n = kidpal_reminder_receive(s, &a);
    krm_require_implemented(n, "kidpal_reminder_receive unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_known_count(s) == 1,
        "KRM-A6 RED: fixture did not record the id");
    kidpal_reminder_init(s);
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_known_count(s) == 0,
        "KRM-A6 RED: re-init 后 known-id 集仍有内容 — known-id 必须易失只驻 RAM "
        "(决策 24 哑终端纯洁性; 若靠持久层「记住」即违禁)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_queue_count(s) == 0,
        "KRM-A6 RED: re-init did not clear the queue");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, sink, sizeof(sink)) == 0,
        "KRM-A6 RED: re-init left a resident popup");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &a) == KRM_STATE_QUEUED,
        "KRM-A6 RED: after re-init a previously-known id is still refused — 说明去重集"
        "活在了 RAM 之外 (重启/重新初始化后仍能记住)");

    /* 集合有界 (RAM 上界 = KRM_KNOWN_CAP, 不随条目无限增长) */
    {
        int i, known;
        for (i = 0; i < KRM_KNOWN_CAP + 10; i++) {
            char idb[24];
            snprintf(idb, sizeof(idb), "rmd-bnd-%d", i);
            krm_fill(&a, idb, "2026-09-09T16:15:00+08:00", "文案");
            (void)kidpal_reminder_receive(s, &a);
        }
        known = kidpal_reminder_known_count(s);
        TEST_ASSERT_TRUE_MESSAGE(known >= 0 && known <= KRM_KNOWN_CAP,
            "KRM-A6 RED: known-id 集超出声明上界 — 有界性缺失 (RAM 无上界=设备侧 OOM 面)");
    }

    /* 结构面: 介质禁令 — 提醒模块不得碰 NVS / 文件系统 / 堆（同样 display/ 优先定位模块文件） */
    n = krm_scan_count("firmware/main/display", "kidpal_reminder_receive(", 0, hit, sizeof(hit));
    if (n == 0) {
        n = krm_scan_count("firmware/main", "kidpal_reminder_receive(", 0, hit, sizeof(hit));
    }
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "KRM-A6 RED(infra): no reminder module located for the 介质禁令 scan");
    {
        char *buf = krm_read(hit, NULL);
        assert(buf != NULL);
        TEST_ASSERT_TRUE_MESSAGE(!krm_contains(buf, "nvs_open") && !krm_contains(buf, "nvs_set") &&
                                     !krm_contains(buf, "NvsHandle"),
            "KRM-A6 RED: the reminder module uses NVS — 提醒 known-id/队列禁持久化 "
            "(决策 24 介质禁令: 设备不缓存事项, 易失 id 集合只驻 RAM)");
        TEST_ASSERT_TRUE_MESSAGE(!krm_contains(buf, "fopen") && !krm_contains(buf, "freopen") &&
                                     !krm_contains(buf, "std::ofstream") &&
                                     !krm_contains(buf, "pending.json"),
            "KRM-A6 RED: the reminder module touches the filesystem — 提醒事项/交接文件"
            "不得落到设备侧 (RMD-A7 断网无提醒; pending.json 是 ECS 同机交接面)");
        TEST_ASSERT_TRUE_MESSAGE(!krm_contains(buf, "malloc(") && !krm_contains(buf, "calloc(") &&
                                     !krm_contains(buf, "= new ") &&
                                     !krm_contains(buf, "std::vector"),
            "KRM-A6 RED: the reminder module allocates dynamically — known-id/队列必须是"
            "定长结构 (有界 RAM; 队列深度由 KRM_QUEUE_CAP 上界约束)");
        free(buf);
    }
    /* kidpal_alert_t 定长、尺寸可预期 (上面用 4096 字节裸缓冲承载状态机的纪律据此) */
    TEST_ASSERT_TRUE_MESSAGE(sizeof(kidpal_alert_t) <= 512 && sizeof(kidpal_alert_t) >= 64,
        "KRM-A7 RED(infra): kidpal_alert_t size drifted outside the frozen envelope — "
        "定长结构是「无动态分配」的前提");
}

/* -------------------------------------------------------------------------- *
 * RMD-A7 — 断网无提醒 = 设备零本地事项源 (决策 24, 哑终端纯洁性)
 * 可机判的负向面: 提醒只能由云端推送到达 (kidpal/alert), 设备不得自带课表/提醒数据源、
 * 不得有本地定时触发, 三档阶梯常量不得进固件 (纯服务端配置, 永不需烧板子)。
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule reminder b: 断网无提醒——设备零本地事项源 (RMD-A7 boundary)",
          "[schedule_reminder][RMD-A7][boundary]")
{
    char hit[KRM_MAX_PATH];
    int n;

    KRM_REQUIRE_ROOT();

    n = krm_scan_count("firmware/main", "schedule.md", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "RMD-A7 RED: firmware reads schedule.md — 课表原文只活在云端 (OpenClaw workspace), "
        "设备拿不到事项源就没有离线可提醒的东西 (决策 24)");

    n = krm_scan_count("firmware/main", "pending.json", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "RMD-A7 RED: firmware references alerts/pending.json — 该文件是 ECS 同机交接面, "
        "设备侧读到它 = 自建本地提醒队列, 违「断网无提醒」");

    n = krm_scan_count("firmware/main", "semantic.json", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "RMD-A7 RED: firmware references the OpenClaw semantic intermediate artifact — "
        "语义判断与文案产出在云侧 agent 回合, 设备零参与 (裁定一混合分层)");

    /* 三档阶梯时刻 = 纯服务端配置, 永不烧板子 (spec S-F1.4) */
    n = krm_scan_count("firmware/main", "\"20:30\"", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "RMD-A7 RED: the 20:30 ladder constant is baked into firmware — 三档时刻必须只从 "
        "OpenClaw 侧提醒配置读, 改时刻不得需要烧板子 (spec S-F1.4 三段阶梯)");
    n = krm_scan_count("firmware/main", "\"07:45\"", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "RMD-A7 RED: the 07:45 ladder constant is baked into firmware (同上)");
    n = krm_scan_count("firmware/main", "remind_before_min", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "RMD-A8 RED: firmware references remind_before_min — 该字段已废止不消费 "
        "(契约 §2.3, 2026-09-03 契约门裁定), 提前量模型不得复活在任一侧");
}

/* -------------------------------------------------------------------------- *
 * boundary / RMD-A8 / scanner 自检（防假绿）
 *  - scanner integrity: real file exists / bogus file absent / unreadable path NULL /
 *    no-match needle matches nothing / counter counts a known literal;
 *  - RMD-A8 契约零变更: 提醒载荷内嵌 view 只能取 §1.3 白名单, 设备不得为提醒发明
 *    "reminder" view (state-reminder 是 UI 状态名, 不是契约 view);
 *  - 弹窗叠加裁决以包 2 路由底座为准 (kidpal_display_route 数值码不得偏移)。
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule reminder b: 扫描器自检 + 不发明 view 契约零变更边界 (boundary/security)",
          "[schedule_reminder][RMD-A8][boundary][security]")
{
    char hit[KRM_MAX_PATH];
    int n;

    KRM_REQUIRE_ROOT();

    /* scanner integrity */
    TEST_ASSERT_TRUE_MESSAGE(krm_exists("firmware/main/display/view_router.c"),
        "boundary RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(krm_exists("firmware/main/display/__krm_no_such_file__.c"),
        "boundary RED(infra): scanner reports a nonexistent file as present");
    TEST_ASSERT_NULL_MESSAGE(krm_read("firmware/main/display/__krm_no_such_file__.c", NULL),
        "boundary RED(infra): krm_read returned data for a nonexistent file");
    n = krm_scan_count("firmware/main/display", "__krm_no_such_needle__", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "boundary RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    n = krm_count_in_file("firmware/main/display/view_router.c", "VRT_ROUTE_SCHEDULE");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "boundary RED(infra): occurrence counter cannot count a known literal — 计数器失灵");

    /* 包 2 冻结数值码不得偏移 (提醒弹窗与首页/P2 的叠加以此为准) */
    TEST_ASSERT_TRUE_MESSAGE(VRT_ROUTE_TEXT == 0 && VRT_ROUTE_HOME == 1 &&
                                 VRT_ROUTE_CONTROL == 2 && VRT_ROUTE_CHAR == 3 &&
                                 VRT_ROUTE_SCHEDULE == 4 && VRT_ROUTE_STORY_LIST == 5 &&
                                 VRT_ROUTE_STORY_TEXT == 6 && VRT_ROUTE_UNKNOWN == -1,
        "boundary RED: view_router 数值码偏移 — VRT 冻结面被破坏 (契约 §1.3 白名单单一源)");

    /* host-runner-only: 白名单外/reminder view 一律落降级, 不得有专门路由 */
    if (kidpal_display_route != NULL) {
        TEST_ASSERT_TRUE_MESSAGE(kidpal_display_route("text") == VRT_ROUTE_TEXT,
            "boundary RED(infra): kidpal_display_route(\"text\") regressed — 提醒载荷内嵌"
            "view 的默认落点不再可用");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_display_route("reminder") == VRT_ROUTE_UNKNOWN,
            "RMD-A8 RED: a reminder/state-reminder view got a real route — 契约 §1.3 "
            "白名单零变更红线: state-reminder 是 UI 状态名, 不得成为新契约 view");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_display_route("state-reminder") == VRT_ROUTE_UNKNOWN,
            "RMD-A8 RED: view \"state-reminder\" routed — 同上 (提醒弹窗用白名单视图承载)");
        TEST_ASSERT_TRUE_MESSAGE(kidpal_display_route(NULL) == VRT_ROUTE_UNKNOWN,
            "boundary RED: NULL view must route to the degrade path (V27 兜底)");
    }
}

/* -------------------------------------------------------------------------- *
 * RMD-A1/A2 tighten ADD（2026-09-08 第四单同源审计）— 显式降级标记「具名化」
 * 既有 RMD-A1 tooth 7 的判据是 降级 || degrade || strstr("-1")：第三支是无鉴别力的逃逸口
 * —— 实测被测模块 reminder_page.c 里 "-1" 出现 47 次（每个防御分支都 return -1），该支
 * 恒真，等于「任何会返回负数的 C 文件都算带降级语义」= 与 carrier 宽词齿同一类假绿。
 * 本齿只钉那条 tooth 当初声明的同一件事（解析失败 = 显式降级而非盲渲染，V27 / spec §4.0），
 * 但去掉 "-1" 逃逸：提醒模块内必须出现**显式**降级标记（降级 / degrade / fallback / 友好提示
 * 四种措辞任一），即四种全缺席才红。实现侧现已满足（实测 11 处显式标记）→ 本齿当下即绿；
 * 日后若有人把降级注释/日志措辞删光只留 return -1，本齿咬。
 * 不新增 acceptance id、不改行为定义（tighten-only）。
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule reminder b: 提醒模块显式降级标记具名化 (RMD-A1 tighten)",
          "[schedule_reminder][RMD-A1][RMD-A2][structural]")
{
    char hit[KRM_MAX_PATH];
    char *buf;
    int n;

    KRM_REQUIRE_ROOT();
    n = krm_scan_count("firmware/main/display", "kidpal_reminder_receive(", 0, hit, sizeof(hit));
    if (n == 0) {
        n = krm_scan_count("firmware/main", "kidpal_reminder_receive(", 0, hit, sizeof(hit));
    }
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "KRM-A1 tighten RED(infra): 定位不到提醒模块，无法核显式降级标记");
    buf = krm_read(hit, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "KRM-A1 tighten RED(infra): 提醒模块不可读");
    TEST_ASSERT_TRUE_MESSAGE(
        krm_contains(buf, "降级") || krm_contains_ci(buf, "degrade") ||
            krm_contains_ci(buf, "fallback") || krm_contains(buf, "友好提示"),
        "KRM-A1 tighten RED: 提醒模块内没有任何**显式**降级语义标记（降级 / degrade / fallback /"
        " 友好提示 四种措辞全部缺席）— 契约 §三 + V27 要求畸形 alert 载荷走显式降级路径、"
        "绝不盲入队盲渲染；既有结构齿的 -1 逃逸支对这种缺失不敏感（任何返回负数的 C 文件都命中），"
        "本齿按同一声明意图把它具名化补上。");
    free(buf);
}

/* -------------------------------------------------------------------------- *
 * RMD-A5 render face — host-runner-only LVGL 软渲染真帧冒烟（渲染面证据;
 * 真实屏幕观感/网点疏密/pose 动画细节殿后 9b + 设计 review, 不钉脆断言）。
 * 机制: 先经包 0 冻结渲染缝 host_infra_render_frame() 初始化 LVGL 离屏显示并置
 * rendered 态, 再用被测渲染契约函数在活动屏上真画, lv_refr_now 刷新后经
 * host_infra_read_pixel 采样网格断言真实像素。requires lvgl.h — 独立红基线 stub
 * 编译无 lvgl include path, 本测试被编译排除（如实记录, 其红态在 builder 接线进
 * host runner 时兑现）。
 * ------------------------------------------------------------------------ */
#if defined(__has_include)
#if __has_include("lvgl.h")
#define KRM_HAVE_LVGL 1
#include <lvgl.h>
#endif
#endif

#ifdef KRM_HAVE_LVGL

extern int host_infra_render_frame(void);
extern int32_t host_infra_read_pixel(int x, int y);

#define KRM_GRID_STEP 20
#define KRM_GRID_W (400 / KRM_GRID_STEP)
#define KRM_GRID_H (300 / KRM_GRID_STEP)

static void krm_sample_grid(int32_t *out)
{
    int i = 0;
    for (int y = KRM_GRID_STEP / 2; y < 300; y += KRM_GRID_STEP) {
        for (int x = KRM_GRID_STEP / 2; x < 400; x += KRM_GRID_STEP) {
            out[i++] = host_infra_read_pixel(x, y);
        }
    }
}

static void krm_render_and_sample(const kidpal_alert_t *a, int32_t *grid)
{
    int drawn = kidpal_reminder_page_render(a);
    krm_require_implemented(drawn, "kidpal_reminder_page_render unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(drawn == 1,
        "KRM-A5 render RED: the render face did not report a drawn frame");
    lv_refr_now(NULL);
    krm_sample_grid(grid);
}

TEST_CASE("schedule reminder b: 提醒弹窗渲染面真帧——压屏聚焦 + 内容可见差异 + 不叠屏 (RMD-A5 render)",
          "[schedule_reminder][RMD-A5][render]")
{
    static int32_t grid_a[KRM_GRID_W * KRM_GRID_H];
    static int32_t grid_a2[KRM_GRID_W * KRM_GRID_H];
    static int32_t grid_b[KRM_GRID_W * KRM_GRID_H];
    kidpal_alert_t a, b;
    int diff = 0, dark = 0, uniform_fill = 1;

    /* 包 0 冻结渲染缝: 初始化 + rendered 态 (fill 0x336699)。 */
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KRM render RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    krm_fill(&a, "rmd-render-1", "2026-09-09T16:15:00+08:00", "16:30 有钢琴课，别忘了带琴谱！");
    snprintf(a.title, sizeof(a.title), "钢琴课 16:30 · 带琴谱");
    krm_render_and_sample(&a, grid_a);

    for (int i = 0; i < KRM_GRID_W * KRM_GRID_H; i++) {
        if (grid_a[i] != 0x336699) {
            uniform_fill = 0;
        }
        if (grid_a[i] < 0xC0C0C0) {
            dark++;
        }
    }
    TEST_ASSERT_FALSE_MESSAGE(uniform_fill,
        "KRM-A5 render RED: frame still uniform seam-fill after render — 提醒弹窗没有真的"
        "画到屏上 (整屏压 + 大字事项缺失)");
    TEST_ASSERT_TRUE_MESSAGE(dark >= 5,
        "KRM-A5 render RED: too few dark pixels in the sampled grid — 大字事项/时间行/形象"
        "内容缺失 (spec S-F1.4 弹窗形态: 整屏压网点 + pose-jump + 大字 + 时间)");

    /* 不同事项的真帧必须可见差异 (内容正确性: 不是「同一张占位图」) */
    krm_fill(&b, "rmd-render-2", "2026-09-08T20:30:00+08:00", "明天美术班，带画笔！");
    snprintf(b.title, sizeof(b.title), "美术班 10:00 · 带画笔");
    krm_render_and_sample(&b, grid_b);
    for (int i = 0; i < KRM_GRID_W * KRM_GRID_H; i++) {
        if (grid_a[i] != grid_b[i]) {
            diff++;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(diff > 0,
        "KRM-A5 render RED: two different reminders produced pixel-identical frames — "
        "弹窗内容不随载荷变化 (数据可见性/内容正确性不成立)");

    /* 重渲染同一弹窗 = 先清屏再画, 不叠屏 (关掉当前弹下一个时的帧不变式) */
    krm_render_and_sample(&a, grid_a2);
    {
        int accum = 0;
        for (int i = 0; i < KRM_GRID_W * KRM_GRID_H; i++) {
            if (grid_a[i] != grid_a2[i]) {
                accum++;
            }
        }
        TEST_ASSERT_TRUE_MESSAGE(accum == 0,
            "KRM-A5 render RED: re-rendering a popup accumulated with the previous frame — "
            "渲染面必须先清活动屏 (排队逐个弹「不叠屏」的渲染级证据)");
    }

    /* 非法载荷不崩、不画 */
    {
        int drawn = kidpal_reminder_page_render(NULL);
        TEST_ASSERT_TRUE_MESSAGE(drawn == -1,
            "KRM-A5 render RED: NULL alert must be rejected (-1), never crash");
        lv_refr_now(NULL);
        krm_fill(&a, "", "2026-09-09T16:15:00+08:00", "文案");
        drawn = kidpal_reminder_page_render(&a);
        TEST_ASSERT_TRUE_MESSAGE(drawn == -1,
            "KRM-A5 render RED: an id-less alert must be rejected (-1) — 无幂等锚不投递");
        lv_refr_now(NULL);
    }
}

/* -------------------------------------------------------------------------- *
 * RMD-A5 render — tighten 补齿（2026-09-08 mutation 暴露的覆盖缺口）
 * 「先清屏」具名化：B 渲染完成后，活动屏上**不得残留 A 的场景对象**。
 *
 * 为什么这条与上面 :1591 的 accum==0 齿不重复：那条齿用**同一载荷、同一几何**重渲染，
 * 观测到的是「像素不累积」这个结果；而被测渲染面每帧都先画整屏白底 + 整屏 50% 压层 +
 * 不透明白卡（reminder_page.c KRM_CARD_* 几何与载荷无关），所以哪怕漏掉 lv_obj_clean，
 * 上一帧的残留对象也被本帧的不透明层完全盖住 —— 像素级看不出差别（mutation
 * 「删 lv_obj_clean」时 :1591 齿仍绿 = 该缺口）。本齿因此把「不叠屏」钉在**场景对象残留**
 * 这一层（同一声明意图 RMD-A5「新提醒排队逐个弹、不叠屏」，不新增行为定义）：
 * 由测试自己建立纯净基线（与被测实现的清屏动作无关），再用「只渲染 B」与
 * 「先渲染 A 再渲染 B」两条路径对比活动屏子对象数 —— 漏清屏时第二条路径多出 A 的全部对象。
 * 最后附一条像素级一致性断言：它在本变异下仍绿（如上所述被不透明层掩盖），作用是
 * 把「A 的可见内容不得残留」这一意图也钉在案上，并在几何日后随载荷变化时（渲染面
 * 头注记的升级路径 = 接中文字库与 pose 位图资源）自动获得鉴别力。
 * 本齿只用既有缝与采样手法（host_infra_render_frame / krm_sample_grid），不引入
 * 任何依赖 LVGL 渲染时序的断言：子对象数是 lv_obj_clean 之后的纯同步读数。
 * ------------------------------------------------------------------------ */
static uint32_t krm_scr_children(void)
{
    lv_obj_t *scr = lv_screen_active();

    return (scr != NULL) ? lv_obj_get_child_count(scr) : 0u;
}

TEST_CASE("schedule reminder b: 渲染面先清屏具名化——B 后不残留 A 的场景对象 (RMD-A5 tighten)",
          "[schedule_reminder][RMD-A5][render]")
{
    static int32_t frame_b_alone[KRM_GRID_W * KRM_GRID_H];
    static int32_t frame_b_after_a[KRM_GRID_W * KRM_GRID_H];
    kidpal_alert_t a, b;
    uint32_t alone_n = 0, seq_n = 0, repeat_n = 0;
    int i, diff = 0;

    /* 包 0 冻结渲染缝: 初始化 + rendered 态。 */
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KRM-A5 tighten RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    /* A 与 B 故意走**不同的绘制支路**：A 有 speech（三件套全画）、B 无 speech
     * （仅弹窗支路）；标题一长一短。两支路的对象集不同，残留探针才更严。 */
    krm_fill(&a, "rmd-residue-a", "2026-09-09T16:15:00+08:00", "16:30 有钢琴课，别忘了带琴谱！");
    snprintf(a.title, sizeof(a.title), "钢琴课 16:30 · 带琴谱 · 琴行");
    krm_fill(&b, "rmd-residue-b", "2026-09-08T20:30:00+08:00", "");
    snprintf(b.title, sizeof(b.title), "美术班");

    /* --- 路径 1（基线）：测试自建纯净屏上只渲染 B ------------------------------ */
    lv_obj_clean(lv_screen_active());
    krm_render_and_sample(&b, frame_b_alone);
    alone_n = krm_scr_children();
    TEST_ASSERT_TRUE_MESSAGE(alone_n > 0,
        "KRM-A5 tighten RED: 只渲染 B 之后活动屏子对象数为 0 — 渲染面什么都没画，"
        "本齿的对比基线不成立（防空实现假绿）");

    /* --- 路径 2（对照）：同一纯净起点，先渲染 A 再渲染 B ----------------------- */
    lv_obj_clean(lv_screen_active());
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_page_render(&a) == 1,
        "KRM-A5 tighten RED: 残留探针的前置渲染 A 未成功");
    krm_render_and_sample(&b, frame_b_after_a);
    seq_n = krm_scr_children();
    TEST_ASSERT_TRUE_MESSAGE(seq_n == alone_n,
        "KRM-A5 tighten RED: 渲染 B 之后活动屏仍带着 A 的残留对象 — 「先清屏」缺失即"
        "场景对象累积（排队逐个弹「不叠屏」的对象级不变式被破, spec S-F1.4 / RMD-A5）。"
        " B-only 与 A→B 两条路径的子对象数必须相等。");

    /* --- 再弹一次同一提醒：数量仍不得增长（关掉当前弹下一个时的同一不变式）------ */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_page_render(&b) == 1,
        "KRM-A5 tighten RED: 重复渲染 B 未成功");
    repeat_n = krm_scr_children();
    TEST_ASSERT_TRUE_MESSAGE(repeat_n == alone_n,
        "KRM-A5 tighten RED: 重复渲染同一提醒使场景对象数增长 — 渲染面未先清活动屏");

    /* --- 画面级：两条路径的最终帧必须一致（A 的可见内容不残留）------------------ */
    for (i = 0; i < KRM_GRID_W * KRM_GRID_H; i++) {
        if (frame_b_alone[i] != frame_b_after_a[i]) {
            diff++;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(diff == 0,
        "KRM-A5 tighten RED: A→B 路径的最终帧与「只渲染 B」的基线帧不一致 — 上一弹窗的内容"
        "残留到了本弹窗画面上（不叠屏的像素级证据, RMD-A5）");
}

#endif /* KRM_HAVE_LVGL */

/* --------------------------------------------------------------------------- *
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-08  initial freeze (acceptance-author, TaskPackage 260829-schedule-reminder).
 *   2026-09-08  tighten-only ADD: one extra host-runner render TEST_CASE
 *               "渲染面先清屏具名化——B 后不残留 A 的场景对象 (RMD-A5 tighten)" —
 *               pins the SAME declared RMD-A5 intent (排队逐个弹、不叠屏) at the
 *               scene-object-residue level. Reason: mutation evidence sensitive:false —
 *               deleting lv_obj_clean(scr) in reminder_page.c kept the existing
 *               accum==0 tooth GREEN, because that tooth re-renders the same payload at
 *               the same geometry and every frame repaints an opaque full-screen white
 *               base + 50% dim + opaque card, so stale objects are pixel-masked (the
 *               card geometry is payload-independent, so no payload choice can expose
 *               the residue in pixels). New tooth builds its own pristine baseline and
 *               compares child-object counts of "render B alone" vs "render A then B"
 *               (plus a repeat-render count and a frame-equality assertion). No existing
 *               assertion removed or modified; no new acceptance id, no behaviour
 *               re-definition.
 *   2026-09-08  tighten-only ADD (同源审计, 第四单): one extra structural TEST_CASE
 *               "提醒模块显式降级标记具名化 (RMD-A1 tighten)" — requires an EXPLICIT
 *               degrade marker (降级 / degrade / fallback / 友好提示) inside the reminder
 *               module. Reason: same oracle-defect family as the carrier-tooth false
 *               green integrator found on the .v3 slot — the existing RMD-A1 tooth 7
 *               accepts strstr("-1") as an alternative, which is undiscriminating
 *               (reminder_page.c returns -1 from 47 defensive branches, so the branch is
 *               permanently true and the tooth can never fail on a missing degrade path).
 *               This ADD keeps the same declared intent (V27 / spec §4.0 畸形载荷显式降级
 *               而非盲渲染), removes no assertion, adds no acceptance id, and is already
 *               satisfied by the landed implementation (11 explicit markers).
 * --------------------------------------------------------------------------- */
