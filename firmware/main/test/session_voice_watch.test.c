/*
 * Builder unit tests — TaskPackage 261116-channel-resident-core（真机轮 B 修复轮）。
 *
 * 定位：**冻结验收 session_channel_resident.acceptance.c SCR-A9..A14 的补强**，不是
 * 替代。冻结件已钉行为本体（静默 60 秒收尾 / 电平卡死有界 160 秒收尾 / 起音沿风暴
 * vs 短暴发 / A7 连续讲保护 / reset 零残留 / 诊断限频决策表）。本件只补冻结集未穷尽的
 * 防御面与边界：
 *   1. NULL 入参防御：四个函数全部不崩；tick 的 NULL 路径**零写字段**（不污染调用方
 *      状态），返回域仍在 {KVW_ACT_NONE} 内；
 *   2. cfg 数值 <= 0 的降级方向：该路证据「不采信」= fail-safe 方向（更早收尾），
 *      绝不是更晚——这是线上一旦 cfg 被零初始化时的安全底线；
 *   3. asr_age_s 饱和：长静置下停在 KIDPAL_VW_ASR_AGE_SAT（999），不溢出成负数（负数
 *      会让 asr_fresh 与审计字段双双失真）；
 *   4. diag_due 的 now == last 边界与最小间隔/周期恰等值两侧；
 *   5. tick 在 level_run_s 恰等于可信窗上界（仍采信）与 +1（停止采信）两侧的边界，
 *      以及连续段下界（sustain-1 不采信 / sustain 采信）。
 *
 * 纯逻辑、零芯片依赖（session_voice_watch.c 在 host runner 与设备树两侧同源）。
 * 用例内部均先 poison 再调用、不依赖整轮执行序。
 */

#include <unity.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "../session_voice_watch.h"

#define SVW_BUILDER_RED "SVW builder RED: "

/* 冻结语义镜像：idle 阈值由调用方注入（application.cc 用 kSessionIdleTimeoutS）。 */
#define SVW_TEST_IDLE_S 60

static void svw_cfg(kidpal_voice_watch_cfg_t *cfg)
{
    memset(cfg, 0xA5, sizeof(*cfg));
    kidpal_voice_watch_cfg_default(cfg);
    cfg->idle_timeout_s = SVW_TEST_IDLE_S;
}

static void svw_watch(kidpal_voice_watch_t *w)
{
    memset(w, 0xA5, sizeof(*w));
    kidpal_voice_watch_reset(w);
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — NULL 入参防御（冻结件只覆盖合法路径；真实调用链上 cfg 指针首次使用前
 * 若初始化顺序被打乱，必须降级为「不动作」而不是崩溃）。
 * ------------------------------------------------------------------------ */
TEST_CASE("voice watch builder: NULL pointer defence never crashes or writes",
          "[session_voice_watch][builder][linux]")
{
    kidpal_voice_watch_t w;
    kidpal_voice_watch_cfg_t cfg;
    int i;

    /* void 面：NULL 一律 no-op。 */
    kidpal_voice_watch_cfg_default(NULL);
    kidpal_voice_watch_reset(NULL);
    kidpal_voice_watch_reset_soft(NULL);
    kidpal_voice_watch_note_asr(NULL);

    /* tick：w 为 NULL -> NONE（不崩、不读 cfg 之外的东西）。 */
    svw_cfg(&cfg);
    TEST_ASSERT_EQUAL_INT_MESSAGE(KVW_ACT_NONE, kidpal_voice_watch_tick(NULL, 1, &cfg),
        SVW_BUILDER_RED "tick(NULL w) 返回域越界——NULL 防御缺失");

    /* tick：cfg 为 NULL -> NONE 且**零写字段**（poison 必须原样保留）。 */
    svw_watch(&w);
    w.tick_s = 7;
    w.quiet_s = 11;
    w.level_run_s = 13;
    w.asr_age_s = 17;
    w.asr_seen = 1;
    w.suspect = 1;
    w.diag_last_s = 23;
    for (i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL_INT_MESSAGE(KVW_ACT_NONE,
            kidpal_voice_watch_tick(&w, 1, NULL),
            SVW_BUILDER_RED "tick(cfg=NULL) 返回域越界——NULL 防御缺失");
    }
    TEST_ASSERT_TRUE_MESSAGE(w.tick_s == 7 && w.quiet_s == 11 && w.level_run_s == 13 &&
                                 w.asr_age_s == 17 && w.asr_seen == 1 &&
                                 w.suspect == 1 && w.diag_last_s == 23,
        SVW_BUILDER_RED "tick(cfg=NULL) 写了状态字段——NULL 路径必须零写（否则调用方"
        "状态被半推进，判据与审计双失真）");
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — cfg 数值 <= 0 的降级方向 = 更早收尾（fail-safe）。
 * 零/负阈值只可能来自坏初始化；此时宁可早收（会话可重开），绝不晚收（开麦空转）。
 * ------------------------------------------------------------------------ */
TEST_CASE("voice watch builder: non-positive cfg degrades toward earlier fire",
          "[session_voice_watch][builder][linux]")
{
    kidpal_voice_watch_cfg_t cfg;
    kidpal_voice_watch_t w;
    int t, first_fire;

    /* (a) sustain 下界 <= 0：连续段证据不采信（否则 level_run>=0 恒真 = 永不收尾）。 */
    svw_cfg(&cfg);
    cfg.voice_sustain_s = 0;
    svw_watch(&w);
    first_fire = -1;
    for (t = 1; t <= 200; t++) {
        int ret = kidpal_voice_watch_tick(&w, 1, &cfg); /* 电平恒真也不采信 */
        if (ret == KVW_ACT_IDLE_FIRE && first_fire < 0) {
            first_fire = t;
        }
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(SVW_TEST_IDLE_S, first_fire,
        SVW_BUILDER_RED "voice_sustain_s=0 时恒真电平被采信（或收尾被无限推迟）——"
        "降级方向必须是更早收尾");

    /* (b) 可信窗上界 <= 0：持续电平不采信（上界失效 = M1 电平卡死永不收尾）。 */
    svw_cfg(&cfg);
    cfg.vad_trust_s = 0;
    svw_watch(&w);
    first_fire = -1;
    for (t = 1; t <= 200; t++) {
        int ret = kidpal_voice_watch_tick(&w, 1, &cfg);
        if (ret == KVW_ACT_IDLE_FIRE && first_fire < 0) {
            first_fire = t;
        }
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(SVW_TEST_IDLE_S, first_fire,
        SVW_BUILDER_RED "vad_trust_s=0 时持续电平仍被采信——可信窗上界失效，电平卡死"
        "场景永不收尾");

    /* (c) asr 新鲜窗 <= 0：note_asr 后也不采信（年龄 0 不构成「新鲜」）。 */
    svw_cfg(&cfg);
    cfg.asr_fresh_s = 0;
    svw_watch(&w);
    kidpal_voice_watch_note_asr(&w);
    (void)kidpal_voice_watch_tick(&w, 0, &cfg);
    TEST_ASSERT_TRUE_MESSAGE(w.asr_seen == 1 && w.asr_age_s == 1 && w.quiet_s == 1,
        SVW_BUILDER_RED "asr_fresh_s=0 时 asr 证据仍被采信——零窗必须不采信（更早收尾）");

    /* (d) idle 阈值 <= 0：首个 tick 即收尾（最激进的 fail-safe 方向）。 */
    svw_cfg(&cfg);
    cfg.idle_timeout_s = 0;
    svw_watch(&w);
    TEST_ASSERT_EQUAL_INT_MESSAGE(KVW_ACT_IDLE_FIRE, kidpal_voice_watch_tick(&w, 0, &cfg),
        SVW_BUILDER_RED "idle_timeout_s=0 时首 tick 未收尾——零阈值必须立即收尾");
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — asr_age_s 饱和（长静置不溢出；负年龄会让 asr_fresh 与审计字段失真）。
 * ------------------------------------------------------------------------ */
TEST_CASE("voice watch builder: asr age saturates and never overflows",
          "[session_voice_watch][builder][linux]")
{
    kidpal_voice_watch_cfg_t cfg;
    kidpal_voice_watch_t w;
    int i;

    svw_cfg(&cfg);
    svw_watch(&w);
    kidpal_voice_watch_note_asr(&w);
    for (i = 0; i < 1500; i++) {
        (void)kidpal_voice_watch_tick(&w, 0, &cfg);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(KIDPAL_VW_ASR_AGE_SAT, w.asr_age_s,
        SVW_BUILDER_RED "asr_age_s 未饱和到 KIDPAL_VW_ASR_AGE_SAT——长静置下年龄溢出");
    TEST_ASSERT_TRUE_MESSAGE(w.tick_s == 1500 && w.asr_seen == 1,
        SVW_BUILDER_RED "饱和路径误清了 tick 计数或 asr_seen");

    /* 饱和后 note_asr 仍能归零（饱和不是闩锁）。 */
    kidpal_voice_watch_note_asr(&w);
    TEST_ASSERT_TRUE_MESSAGE(w.asr_age_s == 0 && w.asr_seen == 1,
        SVW_BUILDER_RED "饱和后 note_asr 未能归零年龄——饱和被实现成闩锁");
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — diag_due 边界：now == last / 周期恰等 / 最小间隔恰等 / 回退。
 * ------------------------------------------------------------------------ */
TEST_CASE("voice watch builder: diag_due equality boundaries",
          "[session_voice_watch][builder][linux]")
{
    /* now == last：无跳变不放行、有跳变也被最小间隔封顶。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_voice_watch_diag_due(5, 5, KIDPAL_VW_DIAG_INTERVAL_S,
                                    KIDPAL_VW_DIAG_MIN_GAP_S, 0),
        SVW_BUILDER_RED "now==last 且无跳变被放行——限频失效");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_voice_watch_diag_due(5, 5, KIDPAL_VW_DIAG_INTERVAL_S,
                                    KIDPAL_VW_DIAG_MIN_GAP_S, 1),
        SVW_BUILDER_RED "now==last 且跳变被放行——零间隔放行 = 每 tick 一条");

    /* 周期恰等（10）放行 / 差一（9）不放行；最小间隔恰等（3）放行 / 差一（2）不放行。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(1,
        kidpal_voice_watch_diag_due(KIDPAL_VW_DIAG_INTERVAL_S, 0,
                                    KIDPAL_VW_DIAG_INTERVAL_S, KIDPAL_VW_DIAG_MIN_GAP_S, 0),
        SVW_BUILDER_RED "周期恰等值未放行");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_voice_watch_diag_due(KIDPAL_VW_DIAG_INTERVAL_S - 1, 0,
                                    KIDPAL_VW_DIAG_INTERVAL_S, KIDPAL_VW_DIAG_MIN_GAP_S, 0),
        SVW_BUILDER_RED "周期差一被放行——周期支提前");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1,
        kidpal_voice_watch_diag_due(KIDPAL_VW_DIAG_MIN_GAP_S, 0,
                                    KIDPAL_VW_DIAG_INTERVAL_S, KIDPAL_VW_DIAG_MIN_GAP_S, 1),
        SVW_BUILDER_RED "跳变最小间隔恰等值未放行");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_voice_watch_diag_due(KIDPAL_VW_DIAG_MIN_GAP_S - 1, 0,
                                    KIDPAL_VW_DIAG_INTERVAL_S, KIDPAL_VW_DIAG_MIN_GAP_S, 1),
        SVW_BUILDER_RED "跳变早于最小间隔被放行——抖动封顶失效");

    /* 时钟回退：即使跳变且满足周期也不放行（fail-closed）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_voice_watch_diag_due(3, 100, KIDPAL_VW_DIAG_INTERVAL_S,
                                    KIDPAL_VW_DIAG_MIN_GAP_S, 1),
        SVW_BUILDER_RED "时钟回退未 fail-closed——回拨后诊断行频率不可预测");

    /* 非法配置：两个零/负参数各返回 -1（不下发）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_voice_watch_diag_due(10, 0, 0, KIDPAL_VW_DIAG_MIN_GAP_S, 0),
        SVW_BUILDER_RED "interval_s=0 未返回 -1");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_voice_watch_diag_due(10, 0, KIDPAL_VW_DIAG_INTERVAL_S, 0, 0),
        SVW_BUILDER_RED "min_gap_s=0 未返回 -1");
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — 连续段下界/上界两侧边界（level_run_s == sustain-1 / sustain /
 * vad_trust / vad_trust+1）。
 * ------------------------------------------------------------------------ */
TEST_CASE("voice watch builder: sustained window boundary inclusivity",
          "[session_voice_watch][builder][linux]")
{
    kidpal_voice_watch_cfg_t cfg;
    kidpal_voice_watch_t w;
    int i;

    svw_cfg(&cfg);
    svw_watch(&w);

    /* 下界：sustain-1 秒不采信（quiet 递增），恰 sustain 秒采信（quiet 归零）。 */
    for (i = 1; i < cfg.voice_sustain_s; i++) {
        (void)kidpal_voice_watch_tick(&w, 1, &cfg);
        TEST_ASSERT_EQUAL_INT_MESSAGE(i, w.quiet_s,
            SVW_BUILDER_RED "连续段低于 sustain 下界却被采信——M2 起音沿风暴解药失效");
    }
    (void)kidpal_voice_watch_tick(&w, 1, &cfg);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, w.quiet_s,
        SVW_BUILDER_RED "连续段恰等于 sustain 下界未采信——真实短话被当噪声");

    /* 上界：level_run_s 恰 == vad_trust 仍采信；+1 起停止采信（quiet 开始累加）。 */
    svw_watch(&w);
    for (i = 1; i <= cfg.vad_trust_s; i++) {
        (void)kidpal_voice_watch_tick(&w, 1, &cfg);
    }
    TEST_ASSERT_TRUE_MESSAGE(w.level_run_s == cfg.vad_trust_s && w.quiet_s == 0 &&
                                 w.suspect == 0,
        SVW_BUILDER_RED "可信窗上界恰值被排除——上界须闭区间采信（vad_trust 秒内"
        "电平恒被采信是 A7 无条件的来源）");
    (void)kidpal_voice_watch_tick(&w, 1, &cfg);
    TEST_ASSERT_TRUE_MESSAGE(w.level_run_s == cfg.vad_trust_s + 1 && w.quiet_s == 1 &&
                                 w.suspect == 1,
        SVW_BUILDER_RED "可信窗上界 +1 未截断——电平卡死（M1）永不收尾");

    /* 电平回落：连续段归零、suspect 随之清零（审计位不粘滞）。 */
    (void)kidpal_voice_watch_tick(&w, 0, &cfg);
    TEST_ASSERT_TRUE_MESSAGE(w.level_run_s == 0 && w.suspect == 0,
        SVW_BUILDER_RED "电平回落未清 continuous run / suspect");
}

/* -------------------------------------------------------------------------- *
 * TEST 6 — asr_seen 门（承重）：asr_age_s 在窗内但从未 note_asr 时不构成证据。
 * 这正是「缺门会把纯静默 60s 收尾拖到 85s」那类错误的直接断言（冻结件 SCR-A9 在
 * 第 85 tick 才红；本件在第 1 tick 就红，定位更快）。
 * ------------------------------------------------------------------------ */
TEST_CASE("voice watch builder: asr freshness requires asr_seen gate",
          "[session_voice_watch][builder][linux]")
{
    kidpal_voice_watch_cfg_t cfg;
    kidpal_voice_watch_t w;
    int t, first_fire = -1;

    svw_cfg(&cfg);
    svw_watch(&w);

    (void)kidpal_voice_watch_tick(&w, 0, &cfg);
    TEST_ASSERT_TRUE_MESSAGE(w.asr_seen == 0 && w.asr_age_s == 1 && w.quiet_s == 1,
        SVW_BUILDER_RED "未收到 stt 时 asr 年龄在窗内即被采信——asr_seen 门缺失"
        "（纯静默收尾会被拖到 idle + asr_fresh 窗）");

    svw_watch(&w);
    for (t = 1; t <= 200; t++) {
        int ret = kidpal_voice_watch_tick(&w, 0, &cfg);
        if (ret == KVW_ACT_IDLE_FIRE && first_fire < 0) {
            first_fire = t;
        }
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(SVW_TEST_IDLE_S, first_fire,
        SVW_BUILDER_RED "纯静默首次收尾不在 idle 阈值——asr_seen 门或累加语义漂移");
}

/* -------------------------------------------------------------------------- *
 * TEST 7 — reset_soft：六清一保 + NULL 防御（2026-11-18 tighten）。
 * 场景 = 记录会话中引导播报把设备带走一次「聆听 -> 播报 -> 聆听」往返。软复位必须
 * 清掉六件计时/诊断/审计字段（播报期间累积的静默秒数不得算进孩子的回答间隙），但
 * 必须保留 asr_seen（D3 空素材 producer 的唯一素材门）。硬复位仍七字段全清。
 * 冻结件 SCR-A13 已钉同一行为；本 case 补的是 NULL 路径（真实调用链上复位点若在
 * 初始化前被触发，必须降级为不动而不是崩）与「六件逐字段」的定位粒度。
 * ------------------------------------------------------------------------ */
TEST_CASE("voice watch builder: reset_soft keeps asr_seen and clears other six",
          "[session_voice_watch][builder][linux]")
{
    kidpal_voice_watch_t w;

    kidpal_voice_watch_reset_soft(NULL); /* no-op、不崩 */

    svw_watch(&w);
    kidpal_voice_watch_note_asr(&w); /* asr_seen = 1（本会话素材证据） */
    w.quiet_s = 41;
    w.level_run_s = 7;
    w.asr_age_s = 13;
    w.tick_s = 88;
    w.diag_last_s = 77;
    w.suspect = 1;

    kidpal_voice_watch_reset_soft(&w);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, w.asr_seen,
        SVW_BUILDER_RED "reset_soft 清了 asr_seen——D3 会把已有素材的故事会话判成空素材"
        "（引导播报往返后静置 60 秒不归档释放，孩子已讲内容丢失）");
    TEST_ASSERT_TRUE_MESSAGE(w.quiet_s == 0 && w.level_run_s == 0 && w.asr_age_s == 0 &&
                                 w.tick_s == 0 && w.diag_last_s == 0 &&
                                 w.suspect == 0,
        SVW_BUILDER_RED "reset_soft 未清计时/诊断/审计六件——播报期间的静默/电平/"
        "审计位被跨往返继承");

    /* 硬复位仍全清（软复位不得削弱跨会话语义）。 */
    kidpal_voice_watch_reset(&w);
    TEST_ASSERT_TRUE_MESSAGE(w.asr_seen == 0 && w.quiet_s == 0 && w.level_run_s == 0 &&
                                 w.asr_age_s == 0 && w.tick_s == 0 &&
                                 w.diag_last_s == 0 && w.suspect == 0,
        SVW_BUILDER_RED "reset_soft 之后 reset 的七字段未全清——跨会话硬复位被软复位"
        "改弱（新会话继承上一会话的素材证据）");
}