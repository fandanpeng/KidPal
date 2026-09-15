/*
 * Frozen acceptance tests — TaskPackage 260829-session-logic (subsystem: firmware).
 * KidPal 包 3：会话状态机（聆听/思考/播报/超时/结束）+ KEY/BOOT 全局按键手势解析。
 * mergeOrder 6, riskTier high, testHost host (ESP-IDF linux target unity runner).
 *
 * Nature: HOST-SIDE pure-logic unit acceptance (linux target, NOT on-device).
 * No chip peripherals, no FreeRTOS primitives: timeouts are EXPLICIT event /
 * timestamp injection (SES_EVT_SESSION_TIMEOUT_60S, SES_EVT_THINKING_TIMEOUT_4S,
 * key timestamps in ms) — never real timers. FROZEN: builder/fixer must satisfy
 * these assertions; changing assertion semantics here requires planner re-dispatch.
 *
 * ---------------------------------------------------------------------------
 * Seam contract pinned by this file (builder implements, tests only #include):
 *
 *   Header 1: firmware/main/session_state_machine.h  (C-compatible, extern "C")
 *   Header 2: firmware/main/key_gesture_parser.h     (C-compatible, extern "C")
 *   Implementation (builder): firmware/main/session_state_machine.cc,
 *                             firmware/main/key_gesture_parser.cc
 *   Builder FIRST ACTION: register this acceptance file AND both implementation
 *   .cc files into firmware/main/test/host_app/main/CMakeLists.txt
 *   HOST_ACCEPTANCE_SRCS (acceptance-author is scope-locked out of that file).
 *   The relative includes below pin the header LOCATION; the builder may add
 *   include dirs but must keep the headers at these paths.
 *
 *   session_state_machine.h must define (plain C structs, no dynamic alloc):
 *     typedef enum { SES_STATE_IDLE=0, SES_STATE_LISTENING, SES_STATE_THINKING,
 *                    SES_STATE_SPEAKING, SES_STATE_MANUAL_RECORDING,
 *                    SES_STATE_ENDED } ses_state_t;
 *       -- ENDED is the terminal state of a ONE-SHOT session object; a new
 *          session requires ses_machine_init() of a fresh instance.
 *       -- SES_STATE_MANUAL_RECORDING is the 包8 接合面 (PROPOSAL §2 包8 预留):
 *          manual 聆听/录音累积态, entered by SES_EVT_START_RECORDING.
 *     typedef enum { SES_EVT_KEY_SHORT_PRESS=0, SES_EVT_BOOT_SHORT_PRESS,
 *                    SES_EVT_END_INTENT, SES_EVT_SESSION_TIMEOUT_60S,
 *                    SES_EVT_UTT_START, SES_EVT_REPLY_READY, SES_EVT_SPEAK_DONE,
 *                    SES_EVT_ABORT_SPEECH, SES_EVT_THINKING_TIMEOUT_4S,
 *                    SES_EVT_START_RECORDING, SES_EVT_STOP_RECORDING,
 *                    SES_EVT_COUNT } ses_event_t;
 *     #define SES_FLAG_ABORT_SPEECH   (1u<<0)  -- 打断停播 side-effect marker
 *     #define SES_FLAG_COMFORT_SPEECH (1u<<1)  -- 4s 思考超时安抚播报 marker
 *     typedef struct { int accepted; ses_state_t from, to; uint32_t flags; }
 *                     ses_transition_t;
 *       -- accepted==0 (rejected): to==from==current state, flags==0, and the
 *          state does NOT change (illegal transition refused, not crash).
 *     typedef struct { ses_state_t state; ses_transition_t last;
 *                      uint32_t accepted_count, rejected_count; } ses_machine_t;
 *     void ses_machine_init(ses_machine_t *m);   -- initializes to SES_STATE_IDLE
 *     const ses_transition_t *ses_machine_dispatch(ses_machine_t *m,
 *                                                  ses_event_t ev);
 *       -- returns &m->last (never NULL); invalid event id (>=SES_EVT_COUNT)
 *          is rejected like any illegal transition.
 *
 *     Transition table pinned by SES-A2/A3/A4/A5 + 包8 接合 (accept -> to):
 *       IDLE:      KEY_SHORT_PRESS -> LISTENING.  Everything else rejected
 *                  (incl. BOOT_SHORT_PRESS: BOOT never starts a session).
 *       LISTENING: UTT_START->THINKING; END_INTENT->ENDED;
 *                  SESSION_TIMEOUT_60S->ENDED; KEY_SHORT_PRESS->ENDED;
 *                  BOOT_SHORT_PRESS->ENDED (对话中 BOOT 也可结束会话);
 *                  START_RECORDING->MANUAL_RECORDING (包8 接合).
 *                  REPLY_READY / SPEAK_DONE / ABORT_SPEECH /
 *                  THINKING_TIMEOUT_4S / STOP_RECORDING rejected.
 *       THINKING:  REPLY_READY->SPEAKING (flags 0);
 *                  THINKING_TIMEOUT_4S->SPEAKING|SES_FLAG_COMFORT_SPEECH;
 *                  END_INTENT / SESSION_TIMEOUT_60S / KEY_SHORT_PRESS /
 *                  BOOT_SHORT_PRESS -> ENDED. Others rejected.
 *       SPEAKING:  SPEAK_DONE->LISTENING;
 *                  ABORT_SPEECH->LISTENING|SES_FLAG_ABORT_SPEECH (人声喊停回聆听);
 *                  KEY_SHORT_PRESS->ENDED|SES_FLAG_ABORT_SPEECH (KEY 打断停播
 *                  并手动关闭会话, spec §4.0 KEY 短按「打断播报」+ §5.2 ③);
 *                  BOOT_SHORT_PRESS->ENDED; END_INTENT->ENDED;
 *                  SESSION_TIMEOUT_60S->ENDED. Others rejected.
 *       MANUAL_RECORDING: STOP_RECORDING->LISTENING; END_INTENT /
 *                  SESSION_TIMEOUT_60S / KEY_SHORT_PRESS / BOOT_SHORT_PRESS ->
 *                  ENDED (会话生命周期语义在累积态不被破坏).
 *                  UTT_START / REPLY_READY / START_RECORDING rejected
 *                  (累积态不进对话轮, 重复切入拒绝).
 *       ENDED:     terminal — every event rejected.
 *
 *   key_gesture_parser.h must define (plain C, one parser instance per key):
 *     typedef enum { KEY_ID_KEY=0, KEY_ID_BOOT, KEY_ID_COUNT } key_id_t;
 *       -- NO PWR key id exists. PWR is the hardware power key; firmware cannot
 *          read it and it NEVER enters the gesture parser, the global routing
 *          layer, or the session state machine (spec §4.0/§5.3, SES-E3). Any
 *          key id >= KEY_ID_COUNT is refused: feed() returns NONE without
 *          state change, route() returns GLOBAL_ACT_NONE. This is the maximal
 *          PWR assertion surface expressible at a pure-logic seam.
 *     typedef enum { KEY_GESTURE_NONE=0, KEY_GESTURE_CLICK,
 *                    KEY_GESTURE_DOUBLE_CLICK, KEY_GESTURE_LONG_PRESS }
 *                    key_gesture_t;
 *     typedef enum { PAGE_P1=0, PAGE_P2, PAGE_P3, PAGE_P4, PAGE_COUNT }
 *                    page_id_t;
 *     typedef struct { uint32_t debounce_ms, click_max_ms, double_gap_ms; }
 *                    key_gesture_config_t;
 *       -- semantics: the very first raw edge (no prior accepted edge) is
 *          ALWAYS accepted (init must mark "no edge seen yet", not ts=0);
 *          raw edges within debounce_ms of the last ACCEPTED edge
 *          are ignored; release duration >= click_max_ms -> LONG_PRESS,
 *          else CLICK candidate; a CLICK candidate whose release is within
 *          double_gap_ms (<=) of the pending candidate -> DOUBLE_CLICK (and
 *          pending cleared); any emitted LONG_PRESS/DOUBLE_CLICK also clears
 *          the pending candidate; a click released after the pending window
 *          expired first flushes the stale pending as CLICK (feed return),
 *          then becomes the new pending; poll(p, now) flushes the pending
 *          CLICK when now - pending_ts > double_gap_ms.
 *     typedef struct { key_gesture_config_t cfg; int last_stable_pressed;
 *                      uint32_t last_edge_ms, press_start_ms;
 *                      int pending_click; uint32_t pending_click_ts;
 *                      key_gesture_t out; } key_gesture_parser_t;
 *     void key_gesture_parser_init(key_gesture_parser_t *p,
 *                                  const key_gesture_config_t *cfg);
 *     key_gesture_t key_gesture_parser_feed(key_gesture_parser_t *p,
 *                                           key_id_t key, uint32_t ts_ms,
 *                                           int pressed);  -- pressed: 1=down 0=up
 *     key_gesture_t key_gesture_parser_poll(key_gesture_parser_t *p,
 *                                           uint32_t now_ms);
 *     page_id_t key_page_next(page_id_t pg);  -- P1->P2->P3->P4->P1 wrap;
 *                                                invalid page -> PAGE_P1
 *     typedef enum { GLOBAL_ACT_NONE=0, GLOBAL_ACT_SESSION_TOGGLE,
 *                    GLOBAL_ACT_HOME, GLOBAL_ACT_PAGE_NEXT } global_action_t;
 *     global_action_t key_route_global(key_id_t key, key_gesture_t g);
 *       -- spec §4.0 全局按键手势表, global layer only (no page parameter,
 *          context-free -> 长按回首页「所有页面一致」由无页面参入参表达):
 *          KEY  CLICK -> GLOBAL_ACT_SESSION_TOGGLE (启动/结束会话;
 *                        会话中打断停播由状态机 SES_EVT_KEY_SHORT_PRESS 承载)
 *          KEY  DOUBLE_CLICK -> GLOBAL_ACT_NONE (KEY 双击全局层无语义)
 *          KEY  LONG_PRESS   -> GLOBAL_ACT_HOME (全局一致)
 *          BOOT CLICK -> GLOBAL_ACT_PAGE_NEXT (P1→P2→P3→P4 循环;
 *                        对话中 BOOT 结束会话由状态机 SES_EVT_BOOT_SHORT_PRESS
 *                        独立承载, 路由层与状态机各取所需)
 *          BOOT LONG_PRESS / DOUBLE_CLICK -> GLOBAL_ACT_NONE
 *          any invalid key (>=KEY_ID_COUNT, e.g. PWR) -> GLOBAL_ACT_NONE
 *
 * ---------------------------------------------------------------------------
 * Scope notes (exit criteria):
 *   SES-E2 (comment-registered, verifier-manual): 本包只做手势解析纯逻辑 +
 *   会话状态机；「4 页循环翻页」跨页端到端整合不在本包（页序纯逻辑由本文件
 *   key_page_next 断言承载），显式由包 9b 全功能端到端真机兜底验收
 *   （PROPOSAL §7 留根）。本文件不含跨页渲染/页面内容断言。
 *   SES-E3 (comment + behavioral): PWR 为硬件电源键、固件不可读、不入交互
 *   状态机——本文件头注释、头文件契约注释与 invalid-key 断言（T8）共同承载；
 *   状态机事件表从定义上不含任何 PWR 事件。
 *   包 8 接合面（PROPOSAL §2 包 8 预留，非 SES 编号，挂 SES-A2 注记）：
 *   MANUAL_RECORDING 进入/退出接缝 + 累积态生命周期语义 → T9。
 *
 * ---------------------------------------------------------------------------
 * RED baseline (2026-09-05 acceptance-author 实测，见本文件尾注释与交付记录):
 *   统一红（全新组件类）——本包全部 SES 断言红于同一原因「实现面不存在
 *   （session_state_machine.h / key_gesture_parser.h 未创建）+ 验收文件未被
 *   host runner 收集（HOST_ACCEPTANCE_SRCS 注册行归 builder 首动作）」。
 *   统一红对逐条断言敏感性零信息：高风险包按条目映射记录缺失行为（见交付
 *   记录），并声明依赖 verifier 相位 mutation-check 兜底逐条断言敏感性
 *   （不得豁免 mutation）。
 * ------------------------------------------------------------------------ */

#include <unity.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

/* Seam headers (do not exist yet -> unified red until builder creates them). */
#include "../session_state_machine.h"
#include "../key_gesture_parser.h"

/* ------------------------------------------------------------------------ */
/* Shared helpers                                                            */
/* ------------------------------------------------------------------------ */

/* Drive a fresh machine to the given state along its only legal path. */
static void ses_drive_to(ses_machine_t *m, ses_state_t target)
{
    ses_machine_init(m);
    if (target == SES_STATE_IDLE) {
        return;
    }
    (void)ses_machine_dispatch(m, SES_EVT_KEY_SHORT_PRESS);   /* -> LISTENING */
    if (target == SES_STATE_LISTENING) {
        return;
    }
    (void)ses_machine_dispatch(m, SES_EVT_START_RECORDING);   /* -> MANUAL_RECORDING */
    if (target == SES_STATE_MANUAL_RECORDING) {
        return;
    }
    /* MANUAL_RECORDING 不达 THINKING：回退到聆听路径。改走标准对话链。 */
    ses_machine_init(m);
    (void)ses_machine_dispatch(m, SES_EVT_KEY_SHORT_PRESS);   /* -> LISTENING */
    if (target == SES_STATE_THINKING) {
        (void)ses_machine_dispatch(m, SES_EVT_UTT_START);
        return;
    }
    if (target == SES_STATE_SPEAKING) {
        (void)ses_machine_dispatch(m, SES_EVT_UTT_START);
        (void)ses_machine_dispatch(m, SES_EVT_REPLY_READY);
        return;
    }
    if (target == SES_STATE_ENDED) {
        (void)ses_machine_dispatch(m, SES_EVT_SESSION_TIMEOUT_60S);
        return;
    }
}

static key_gesture_config_t test_gesture_cfg(void)
{
    key_gesture_config_t cfg;
    cfg.debounce_ms  = 30u;
    cfg.click_max_ms = 600u;   /* release duration >= 600ms -> LONG_PRESS */
    cfg.double_gap_ms = 350u;  /* two clicks within 350ms -> DOUBLE_CLICK */
    return cfg;
}

/* ------------------------------------------------------------------------ */
/* coverage: SES-A1 (structural) + SES-E1 (structural)                       */
/* 被测实现面存在且可实例化/可调用：两个纯逻辑接缝头文件被本文件编译期锚定， */
/* 运行期验证两台机器可 init 且落初始态。头文件缺失时本文件无法编译（红基线  */
/* 判据之一），运行到此处即证明结构性接缝成立。                              */
/* ------------------------------------------------------------------------ */
TEST_CASE("会话状态机: 纯逻辑接缝头文件存在且两台机器可实例化落初始态 (SES-A1)",
          "[session_logic][SES-A1][SES-E1][linux]")
{
    ses_machine_t m;
    key_gesture_parser_t p;
    key_gesture_config_t cfg = test_gesture_cfg();

    ses_machine_init(&m);
    TEST_ASSERT_EQUAL(SES_STATE_IDLE, m.state);
    TEST_ASSERT_EQUAL_UINT32(0u, m.accepted_count);
    TEST_ASSERT_EQUAL_UINT32(0u, m.rejected_count);

    key_gesture_parser_init(&p, &cfg);
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE, p.out);
    TEST_ASSERT_FALSE(p.pending_click);

    /* coverage: SES-A1 -- key_page_next 对非法页回 P1（接缝可调用且防御） */
    TEST_ASSERT_EQUAL(PAGE_P1, key_page_next(PAGE_COUNT));
}

/* ------------------------------------------------------------------------ */
/* coverage: SES-A2 -- 正向全链：聆听→思考→播报→回聆听→60s 超时结束；        */
/* 结束意图路径（聆听中 / 思考中 / 播报中）。断言真实终态 + 转移记录 + 计数。 */
/* ------------------------------------------------------------------------ */
TEST_CASE("会话状态机: 聆听思考播报全转移链与 60s 超时、结束意图收束 (SES-A2)",
          "[session_logic][SES-A2][linux]")
{
    const ses_transition_t *t;
    ses_machine_t m;

    /* 全链：KEY 启动 -> 聆听 -> 开口思考 -> 回包播报 -> 播完回聆听 -> 60s 超时结束 */
    ses_machine_init(&m);
    TEST_ASSERT_EQUAL(SES_STATE_IDLE, m.state);

    t = ses_machine_dispatch(&m, SES_EVT_KEY_SHORT_PRESS);
    TEST_ASSERT_NOT_NULL(t);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_LISTENING, m.state);
    TEST_ASSERT_EQUAL(SES_STATE_IDLE, t->from);
    TEST_ASSERT_EQUAL(SES_STATE_LISTENING, t->to);
    TEST_ASSERT_EQUAL_UINT32(0u, t->flags);

    t = ses_machine_dispatch(&m, SES_EVT_UTT_START);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_THINKING, m.state);

    t = ses_machine_dispatch(&m, SES_EVT_REPLY_READY);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_SPEAKING, m.state);
    TEST_ASSERT_EQUAL_UINT32(0u, t->flags);

    t = ses_machine_dispatch(&m, SES_EVT_SPEAK_DONE);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_LISTENING, m.state);

    t = ses_machine_dispatch(&m, SES_EVT_SESSION_TIMEOUT_60S);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_ENDED, m.state);
    TEST_ASSERT_EQUAL(SES_STATE_LISTENING, t->from);
    TEST_ASSERT_EQUAL_UINT32(0u, m.rejected_count);
    TEST_ASSERT_EQUAL_UINT32(5u, m.accepted_count);

    /* 结束意图路径 1：聆听中云端判「拜拜/不用了」-> 结束 */
    ses_machine_init(&m);
    (void)ses_machine_dispatch(&m, SES_EVT_KEY_SHORT_PRESS);
    t = ses_machine_dispatch(&m, SES_EVT_END_INTENT);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_ENDED, m.state);

    /* 结束意图路径 2：思考中 -> 结束 */
    ses_machine_init(&m);
    (void)ses_machine_dispatch(&m, SES_EVT_KEY_SHORT_PRESS);
    (void)ses_machine_dispatch(&m, SES_EVT_UTT_START);
    t = ses_machine_dispatch(&m, SES_EVT_END_INTENT);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_ENDED, m.state);

    /* 结束意图路径 3：播报中云端判停 -> 结束 */
    ses_machine_init(&m);
    ses_drive_to(&m, SES_STATE_SPEAKING);
    t = ses_machine_dispatch(&m, SES_EVT_END_INTENT);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_ENDED, m.state);
}

/* ------------------------------------------------------------------------ */
/* coverage: SES-A2 -- 非法转移被拒绝：状态不变、记录 rejected、不崩溃。     */
/* 每行对应一个「当前 story 必须存在的拒绝行为」（红基线逐条映射见文件尾）。 */
/* ------------------------------------------------------------------------ */
TEST_CASE("会话状态机: 非法转移逐条拒绝且状态不被改写 (SES-A2)",
          "[session_logic][SES-A2][linux]")
{
    struct { ses_state_t from; ses_event_t ev; } cases[] = {
        { SES_STATE_IDLE,             SES_EVT_REPLY_READY },         /* idle→播报 直跳 */
        { SES_STATE_IDLE,             SES_EVT_UTT_START },           /* 未进会话无音频轮 */
        { SES_STATE_IDLE,             SES_EVT_BOOT_SHORT_PRESS },    /* BOOT 不启动会话 */
        { SES_STATE_IDLE,             SES_EVT_THINKING_TIMEOUT_4S },
        { SES_STATE_LISTENING,        SES_EVT_REPLY_READY },         /* 未开口不得直接播报 */
        { SES_STATE_LISTENING,        SES_EVT_SPEAK_DONE },
        { SES_STATE_LISTENING,        SES_EVT_ABORT_SPEECH },        /* 无播报可打断 */
        { SES_STATE_LISTENING,        SES_EVT_THINKING_TIMEOUT_4S },
        { SES_STATE_THINKING,         SES_EVT_UTT_START },           /* 已在思考不重入 */
        { SES_STATE_THINKING,         SES_EVT_SPEAK_DONE },
        { SES_STATE_SPEAKING,         SES_EVT_REPLY_READY },
        { SES_STATE_SPEAKING,         SES_EVT_UTT_START },           /* 插话走 ABORT 非 UTT */
        { SES_STATE_MANUAL_RECORDING, SES_EVT_UTT_START },           /* 累积态不进对话轮 */
        { SES_STATE_MANUAL_RECORDING, SES_EVT_REPLY_READY },
        { SES_STATE_MANUAL_RECORDING, SES_EVT_START_RECORDING },     /* 重复切入拒绝 */
        { SES_STATE_ENDED,            SES_EVT_KEY_SHORT_PRESS },     /* 终态：新会话须新实例 */
        { SES_STATE_ENDED,            SES_EVT_REPLY_READY },
        { SES_STATE_ENDED,            SES_EVT_SESSION_TIMEOUT_60S },
    };
    const size_t n = sizeof(cases) / sizeof(cases[0]);
    ses_machine_t m;

    for (size_t i = 0; i < n; i++) {
        uint32_t rejected_before;
        ses_drive_to(&m, cases[i].from);
        rejected_before = m.rejected_count;
        const ses_transition_t *t = ses_machine_dispatch(&m, cases[i].ev);
        TEST_ASSERT_NOT_NULL(t);
        TEST_ASSERT_FALSE(t->accepted);
        TEST_ASSERT_EQUAL(cases[i].from, m.state);
        TEST_ASSERT_EQUAL(cases[i].from, t->to);
        TEST_ASSERT_EQUAL_UINT32(0u, t->flags);
        TEST_ASSERT_EQUAL_UINT32(rejected_before + 1u, m.rejected_count);
        /* Real assert() fail-stop logic guard (HOST_TEST_CONVENTIONS [C4.2],
         * additive per verifier round 1): state-domain invariant -- a rejected
         * (or any) dispatch must NEVER leave the machine outside the valid
         * ses_state_t domain; corruption here means the table is unsound. */
        assert(m.state >= SES_STATE_IDLE && m.state <= SES_STATE_ENDED);
    }

    /* 越界事件 id 同样按非法转移拒绝（防御性，不崩溃）。 */
    ses_machine_init(&m);
    (void)ses_machine_dispatch(&m, SES_EVT_KEY_SHORT_PRESS);
    const ses_transition_t *t = ses_machine_dispatch(&m, SES_EVT_COUNT);
    TEST_ASSERT_FALSE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_LISTENING, m.state);
}

/* ------------------------------------------------------------------------ */
/* coverage: SES-A3 -- KEY 短按 = 启动/结束会话 + 播报中打断停播；           */
/* KEY 长按 = 回首页（全局一致，路由函数无页面/状态入参即表达「任何页面同   */
/* 语义」）；解析→路由全链走真实数据路径。                                   */
/* ------------------------------------------------------------------------ */
TEST_CASE("会话状态机: KEY 短按启动结束会话打断停播与长按回首页 (SES-A3)",
          "[session_logic][SES-A3][linux]")
{
    const ses_transition_t *t;
    ses_machine_t m;
    key_gesture_parser_t p;
    key_gesture_config_t cfg = test_gesture_cfg();

    /* idle + KEY 短按 = 启动会话（≈唤醒词体验, spec §5.2） */
    ses_machine_init(&m);
    t = ses_machine_dispatch(&m, SES_EVT_KEY_SHORT_PRESS);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_LISTENING, m.state);

    /* 对话中（聆听）再按 KEY = 手动结束（spec §5.2 ③） */
    t = ses_machine_dispatch(&m, SES_EVT_KEY_SHORT_PRESS);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_ENDED, m.state);

    /* 播报中按 KEY = 打断停播 + 手动关闭：终态 ENDED 且带 ABORT 标记 */
    ses_drive_to(&m, SES_STATE_SPEAKING);
    t = ses_machine_dispatch(&m, SES_EVT_KEY_SHORT_PRESS);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_ENDED, m.state);
    TEST_ASSERT_TRUE((t->flags & SES_FLAG_ABORT_SPEECH) != 0u);

    /* 对照：人声喊停 ABORT_SPEECH = 停播回聆听（不结束会话） */
    ses_drive_to(&m, SES_STATE_SPEAKING);
    t = ses_machine_dispatch(&m, SES_EVT_ABORT_SPEECH);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_LISTENING, m.state);
    TEST_ASSERT_TRUE((t->flags & SES_FLAG_ABORT_SPEECH) != 0u);

    /* KEY 长按 = 回首页，全局一致：路由函数无页面参数，任何页面同值。 */
    TEST_ASSERT_EQUAL(GLOBAL_ACT_HOME,
                      key_route_global(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS));
    TEST_ASSERT_EQUAL(GLOBAL_ACT_HOME,
                      key_route_global(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS));

    /* 解析→路由全链：真实长按时序（按下 0ms / 抬起 700ms ≥ 600ms） */
    key_gesture_parser_init(&p, &cfg);
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE,
                      key_gesture_parser_feed(&p, KEY_ID_KEY, 0u, 1));
    TEST_ASSERT_EQUAL(KEY_GESTURE_LONG_PRESS,
                      key_gesture_parser_feed(&p, KEY_ID_KEY, 700u, 0));
    TEST_ASSERT_EQUAL(GLOBAL_ACT_HOME, key_route_global(KEY_ID_KEY, p.out));
}

/* ------------------------------------------------------------------------ */
/* coverage: SES-A4 -- BOOT 短按 = P1→P2→P3→P4 循环翻页（含 P4→P1 回绕）；  */
/* 对话中 BOOT 也可结束会话。                                               */
/* SES-E2 注记：本包只断言页序纯逻辑（key_page_next）与状态机侧 BOOT 语义； */
/* 跨页端到端整合（真实页面渲染/内容切换）不在本包，由包 9b 兜底验收。      */
/* ------------------------------------------------------------------------ */
TEST_CASE("会话状态机: BOOT 短按四页循环翻页与对话中结束会话 (SES-A4)",
          "[session_logic][SES-A4][linux]")
{
    const ses_transition_t *t;
    ses_machine_t m;

    /* 页序纯逻辑：P1→P2→P3→P4→P1 回绕；非法页回 P1 */
    TEST_ASSERT_EQUAL(PAGE_P2, key_page_next(PAGE_P1));
    TEST_ASSERT_EQUAL(PAGE_P3, key_page_next(PAGE_P2));
    TEST_ASSERT_EQUAL(PAGE_P4, key_page_next(PAGE_P3));
    TEST_ASSERT_EQUAL(PAGE_P1, key_page_next(PAGE_P4));
    TEST_ASSERT_EQUAL(PAGE_P1, key_page_next(PAGE_COUNT));
    TEST_ASSERT_EQUAL(PAGE_P1, key_page_next((page_id_t)99));

    /* 全局路由：BOOT 单击 = 翻页；BOOT 长按/双击无全局语义 */
    TEST_ASSERT_EQUAL(GLOBAL_ACT_PAGE_NEXT,
                      key_route_global(KEY_ID_BOOT, KEY_GESTURE_CLICK));
    TEST_ASSERT_EQUAL(GLOBAL_ACT_NONE,
                      key_route_global(KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS));
    TEST_ASSERT_EQUAL(GLOBAL_ACT_NONE,
                      key_route_global(KEY_ID_BOOT, KEY_GESTURE_DOUBLE_CLICK));

    /* 对话中 BOOT 短按也可结束会话（spec §5.2 ③ / §5.3 BOOT 行） */
    ses_drive_to(&m, SES_STATE_LISTENING);
    t = ses_machine_dispatch(&m, SES_EVT_BOOT_SHORT_PRESS);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_ENDED, m.state);

    ses_drive_to(&m, SES_STATE_THINKING);
    t = ses_machine_dispatch(&m, SES_EVT_BOOT_SHORT_PRESS);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_ENDED, m.state);

    ses_drive_to(&m, SES_STATE_SPEAKING);
    t = ses_machine_dispatch(&m, SES_EVT_BOOT_SHORT_PRESS);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_ENDED, m.state);
    /* BOOT 结束不带打断停播标记（打断停播是 KEY 短按的职责）。 */
    TEST_ASSERT_EQUAL_UINT32(0u, t->flags);
}

/* ------------------------------------------------------------------------ */
/* coverage: SES-A5 -- 思考中超 4 秒先开口安抚（不冷场）：                   */
/* THINKING + THINKING_TIMEOUT_4S -> SPEAKING 且带 SES_FLAG_COMFORT_SPEECH； */
/* 对照正常回包无安抚标记；安抚转移只从思考态出发。                          */
/* ------------------------------------------------------------------------ */
TEST_CASE("会话状态机: 思考超四秒切入安抚播报且与正常回包可区分 (SES-A5)",
          "[session_logic][SES-A5][linux]")
{
    const ses_transition_t *t;
    ses_machine_t m;

    /* 4s 超时：thinking → speaking（安抚） */
    ses_drive_to(&m, SES_STATE_THINKING);
    t = ses_machine_dispatch(&m, SES_EVT_THINKING_TIMEOUT_4S);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_THINKING, t->from);
    TEST_ASSERT_EQUAL(SES_STATE_SPEAKING, m.state);
    TEST_ASSERT_TRUE((t->flags & SES_FLAG_COMFORT_SPEECH) != 0u);

    /* 对照：同一状态下正常回包 → speaking 但无安抚标记（两条路径可区分） */
    ses_drive_to(&m, SES_STATE_THINKING);
    t = ses_machine_dispatch(&m, SES_EVT_REPLY_READY);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_SPEAKING, m.state);
    TEST_ASSERT_EQUAL_UINT32(0u, t->flags);

    /* 安抚转移只从思考态：聆听态收到 4s 超时事件被拒绝 */
    ses_drive_to(&m, SES_STATE_LISTENING);
    t = ses_machine_dispatch(&m, SES_EVT_THINKING_TIMEOUT_4S);
    TEST_ASSERT_FALSE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_LISTENING, m.state);
}

/* ------------------------------------------------------------------------ */
/* coverage: SES-A6 -- 手势解析消抖正确：单击（pending → poll 冲刷）、双击   */
/* （窗口内/窗口外边界）、长按（emit 即清 pending，不留幽灵单击）、抖动边沿 */
/* 折叠为一次有效按键。全部显式时间戳注入，无真实 timer。                    */
/* ------------------------------------------------------------------------ */
TEST_CASE("按键手势解析: 单击双击长按时序消抖判定正确 (SES-A6)",
          "[session_logic][SES-A6][linux]")
{
    key_gesture_parser_t p;
    key_gesture_config_t cfg = test_gesture_cfg(); /* debounce 30 / click 600 / gap 350 */

    /* 单击：抬起后进入 pending（feed 返回 NONE），超过双击窗后 poll 冲刷为 CLICK */
    key_gesture_parser_init(&p, &cfg);
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE, key_gesture_parser_feed(&p, KEY_ID_KEY, 1000u, 1));
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE, key_gesture_parser_feed(&p, KEY_ID_KEY, 1150u, 0));
    TEST_ASSERT_TRUE(p.pending_click);
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE, key_gesture_parser_poll(&p, 1400u)); /* 窗口内不冲刷 */
    TEST_ASSERT_EQUAL(KEY_GESTURE_CLICK, key_gesture_parser_poll(&p, 1600u)); /* 1600-1150>350 */
    TEST_ASSERT_FALSE(p.pending_click);
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE, key_gesture_parser_poll(&p, 2000u)); /* 已清不再重复 */

    /* 双击：第二次抬起落在双击窗内（间隔 200 ≤ 350）→ 直接发 DOUBLE_CLICK */
    key_gesture_parser_init(&p, &cfg);
    (void)key_gesture_parser_feed(&p, KEY_ID_KEY, 0u, 1);
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE, key_gesture_parser_feed(&p, KEY_ID_KEY, 100u, 0));
    (void)key_gesture_parser_feed(&p, KEY_ID_KEY, 200u, 1);
    TEST_ASSERT_EQUAL(KEY_GESTURE_DOUBLE_CLICK,
                      key_gesture_parser_feed(&p, KEY_ID_KEY, 300u, 0));
    TEST_ASSERT_FALSE(p.pending_click);
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE, key_gesture_parser_poll(&p, 2000u)); /* 无幽灵单击 */

    /* 长按：按下 700ms（≥ 600）抬起即发 LONG_PRESS，并清掉之前的过期 pending */
    key_gesture_parser_init(&p, &cfg);
    (void)key_gesture_parser_feed(&p, KEY_ID_KEY, 0u, 1);
    (void)key_gesture_parser_feed(&p, KEY_ID_KEY, 100u, 0);   /* pending click @100 */
    (void)key_gesture_parser_feed(&p, KEY_ID_KEY, 1000u, 1);
    TEST_ASSERT_EQUAL(KEY_GESTURE_LONG_PRESS,
                      key_gesture_parser_feed(&p, KEY_ID_KEY, 1800u, 0)); /* dur 800 */
    TEST_ASSERT_FALSE(p.pending_click);
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE, key_gesture_parser_poll(&p, 3000u));

    /* 双击窗边界外：间隔 351 > 350 → 两次独立 CLICK（先冲刷前次，无 DOUBLE） */
    key_gesture_parser_init(&p, &cfg);
    (void)key_gesture_parser_feed(&p, KEY_ID_KEY, 0u, 1);
    (void)key_gesture_parser_feed(&p, KEY_ID_KEY, 100u, 0);   /* click1 pending @100 */
    (void)key_gesture_parser_feed(&p, KEY_ID_KEY, 400u, 1);
    TEST_ASSERT_EQUAL(KEY_GESTURE_CLICK,                     /* 冲刷 click1 */
                      key_gesture_parser_feed(&p, KEY_ID_KEY, 500u, 0));
    TEST_ASSERT_TRUE(p.pending_click);                        /* click2 成为新 pending */
    TEST_ASSERT_EQUAL(KEY_GESTURE_CLICK, key_gesture_parser_poll(&p, 900u)); /* 900-500>350 */
    TEST_ASSERT_FALSE(p.pending_click);

    /* 消抖：抖动边沿（≤30ms 内的 up/down/up）折叠为一次有效按键 → 一次 CLICK */
    key_gesture_parser_init(&p, &cfg);
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE, key_gesture_parser_feed(&p, KEY_ID_KEY, 5000u, 1));
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE, key_gesture_parser_feed(&p, KEY_ID_KEY, 5005u, 0)); /* 抖动,忽略 */
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE, key_gesture_parser_feed(&p, KEY_ID_KEY, 5012u, 1)); /* 抖动,忽略 */
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE, key_gesture_parser_feed(&p, KEY_ID_KEY, 5400u, 0)); /* 稳定抬起 */
    TEST_ASSERT_TRUE(p.pending_click);                        /* 恰一次按键 */
    TEST_ASSERT_EQUAL(KEY_GESTURE_CLICK, key_gesture_parser_poll(&p, 5800u));
}

/* ------------------------------------------------------------------------ */
/* coverage: SES-A6 -- KEY/BOOT 职责分离（KEY 会话、BOOT 翻页）全局路由表；  */
/* coverage: SES-E3 -- 路由/解析 API 不接受不产生 PWR 事件：键位枚举只有     */
/* KEY/BOOT，任何越界键位（含模拟 PWR 的 id）一律 NONE 且不污染解析器状态。  */
/* （PWR 为硬件电源键固件不可读——设计注释见本文件头与头文件契约注释，      */
/* 状态机事件表从定义上无 PWR 事件。）                                       */
/* ------------------------------------------------------------------------ */
TEST_CASE("按键手势解析: KEY BOOT 职责分离全局路由表与 PWR 键位拒收 (SES-A6)",
          "[session_logic][SES-A6][SES-E3][linux]")
{
    key_gesture_parser_t p;
    key_gesture_config_t cfg = test_gesture_cfg();

    /* 全局路由表（spec §4.0 全局按键手势表）： */
    TEST_ASSERT_EQUAL(GLOBAL_ACT_SESSION_TOGGLE,
                      key_route_global(KEY_ID_KEY, KEY_GESTURE_CLICK));
    TEST_ASSERT_EQUAL(GLOBAL_ACT_NONE,
                      key_route_global(KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK)); /* KEY 双击全局无语义 */
    TEST_ASSERT_EQUAL(GLOBAL_ACT_HOME,
                      key_route_global(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS));
    TEST_ASSERT_EQUAL(GLOBAL_ACT_PAGE_NEXT,
                      key_route_global(KEY_ID_BOOT, KEY_GESTURE_CLICK));
    TEST_ASSERT_EQUAL(GLOBAL_ACT_NONE,
                      key_route_global(KEY_ID_BOOT, KEY_GESTURE_DOUBLE_CLICK));
    TEST_ASSERT_EQUAL(GLOBAL_ACT_NONE,
                      key_route_global(KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS));

    /* PWR 模拟（越界键位）：路由一律 NONE，不产生任何会话/翻页动作 */
    TEST_ASSERT_EQUAL(GLOBAL_ACT_NONE,
                      key_route_global((key_id_t)KEY_ID_COUNT, KEY_GESTURE_CLICK));
    TEST_ASSERT_EQUAL(GLOBAL_ACT_NONE,
                      key_route_global((key_id_t)2, KEY_GESTURE_LONG_PRESS));
    TEST_ASSERT_EQUAL(GLOBAL_ACT_NONE,
                      key_route_global((key_id_t)0xFF, KEY_GESTURE_DOUBLE_CLICK));
    /* Real assert() fail-stop logic guard (additive per verifier round 1):
     * key-id domain invariant -- the parser/routing API admits EXACTLY the two
     * physical keys KEY/BOOT; any wider domain would mean a PWR (or other
     * firmware-unreadable) key leaked into the interactive seam (SES-E3). */
    assert(KEY_ID_COUNT == 2);

    /* 解析器拒收越界键位：返回 NONE、状态零污染（后续真键解析不受影响） */
    key_gesture_parser_init(&p, &cfg);
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE,
                      key_gesture_parser_feed(&p, (key_id_t)KEY_ID_COUNT, 0u, 1));
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE,
                      key_gesture_parser_feed(&p, (key_id_t)2, 100u, 0));
    TEST_ASSERT_FALSE(p.pending_click);
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE, key_gesture_parser_poll(&p, 1000u));
    /* 被污染检验：同实例随后解析真实 KEY 单击仍正常 */
    (void)key_gesture_parser_feed(&p, KEY_ID_KEY, 2000u, 1);
    TEST_ASSERT_EQUAL(KEY_GESTURE_NONE, key_gesture_parser_feed(&p, KEY_ID_KEY, 2100u, 0));
    TEST_ASSERT_TRUE(p.pending_click);
    TEST_ASSERT_EQUAL(KEY_GESTURE_CLICK, key_gesture_parser_poll(&p, 2500u));
}

/* ------------------------------------------------------------------------ */
/* coverage: SES-A2（注记）-- 包 8 接合面（PROPOSAL §2 包 8 预留，非 SES 编  */
/* 号）：start_recording 切入 manual 聆听/录音累积态的设备侧状态归本包定义。 */
/* 断言：切入/退出接缝可转移、累积态下会话生命周期语义不被破坏（可被结束）、 */
/* 累积态不进对话轮、切入只从聆听态。                                       */
/* ------------------------------------------------------------------------ */
TEST_CASE("会话状态机: manual 录音累积态切入退出接缝且生命周期不被破坏 (SES-A2)",
          "[session_logic][SES-A2][pkg8_seam][linux]")
{
    const ses_transition_t *t;
    ses_machine_t m;

    /* idle 直接切入被拒绝（先要有会话） */
    ses_machine_init(&m);
    t = ses_machine_dispatch(&m, SES_EVT_START_RECORDING);
    TEST_ASSERT_FALSE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_IDLE, m.state);

    /* 聆听中 start_recording → 切入 MANUAL_RECORDING 累积态 */
    ses_drive_to(&m, SES_STATE_LISTENING);
    t = ses_machine_dispatch(&m, SES_EVT_START_RECORDING);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_MANUAL_RECORDING, m.state);

    /* stop_recording → 退出接缝回聆听，会话仍活着 */
    t = ses_machine_dispatch(&m, SES_EVT_STOP_RECORDING);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_LISTENING, m.state);

    /* 累积态下生命周期语义不被破坏：三种收束任一可达 ENDED */
    ses_drive_to(&m, SES_STATE_MANUAL_RECORDING);
    t = ses_machine_dispatch(&m, SES_EVT_END_INTENT);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_ENDED, m.state);

    ses_drive_to(&m, SES_STATE_MANUAL_RECORDING);
    t = ses_machine_dispatch(&m, SES_EVT_KEY_SHORT_PRESS);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_ENDED, m.state);

    ses_drive_to(&m, SES_STATE_MANUAL_RECORDING);
    t = ses_machine_dispatch(&m, SES_EVT_SESSION_TIMEOUT_60S);
    TEST_ASSERT_TRUE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_ENDED, m.state);

    /* 播报中切入被拒绝（接缝只从聆听态进入） */
    ses_drive_to(&m, SES_STATE_SPEAKING);
    t = ses_machine_dispatch(&m, SES_EVT_START_RECORDING);
    TEST_ASSERT_FALSE(t->accepted);
    TEST_ASSERT_EQUAL(SES_STATE_SPEAKING, m.state);
}

/* ---------------------------------------------------------------------------
 * RED baseline 逐条映射（统一红：实现面不存在 + 验收未收集，2026-09-05 实测）。
 * 全部 SES 断言红于同一原因，按条目映射的「对应缺失行为」：
 *   SES-A1/SES-E1  接缝头文件 session_state_machine.h / key_gesture_parser.h
 *                  未创建 → 本文件编译失败（结构性红）。
 *   SES-A2         会话状态机转移表（含非法转移拒绝表）无实现 → 全链/拒绝
 *                  断言无被测对象。
 *   SES-A3         KEY 短按启动/结束/打断 + 长按回首页路由无实现。
 *   SES-A4         BOOT 四页循环 key_page_next + 对话中 BOOT 结束会话无实现。
 *   SES-A5         THINKING_TIMEOUT_4S → 安抚播报转移（COMFORT 标记）无实现。
 *   SES-A6         手势消抖/单击/双击/长按解析 + 全局路由表 + PWR 拒收无实现。
 *   包 8 接合面     MANUAL_RECORDING 累积态进/出/生命周期转移无实现。
 * 敏感性兜底声明：统一红对逐条断言敏感性零信息，verifier 相位必须执行
 * mutation-check（临时破坏一处转移表/解析规则，验收必须变红），不得豁免。
 * ------------------------------------------------------------------------- */
