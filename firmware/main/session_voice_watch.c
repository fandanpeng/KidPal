/*
 * session_voice_watch.c — 语音会话「有效语音证据」看门缝实现（TaskPackage
 * 261116-channel-resident-core，真机轮 B 修复轮新增，2026-11-18）。
 *
 * 契约权威 = firmware/main/test/session_channel_resident.acceptance.c 文件头 [S8]
 * （冻结，逐字；真行为半由 SCR-A9..A14 承载）。
 *
 * 铁律：本文件**零芯片依赖**——不 include esp_* / FreeRTOS / LVGL，也不读真实时钟；
 * 宿主态（host_app linux target）与设备树编同一份源 = 双侧同源，行为断言在 host 上
 * 真跑（结构锚只覆盖芯片面接线，跑不到这里的判据）。
 *
 * 判据一句话：**quiet_s = 距上次「有效语音证据」的秒数**，到 idle_timeout 即收尾。
 * 有效证据两种、互补：
 *   - sustained 电平连续段落在可信窗 [voice_sustain, vad_trust] 内
 *     （下界拒瞬态噪声暴发；上界让卡死电平在 vad_trust 秒后自然失效）；
 *   - asr_fresh 引擎 stt 新鲜（asr_age_s < asr_fresh_s，且本会话**确实收到过** stt
 *     ——asr_seen 是必须的门：reset 后 asr_age_s 归零，缺此门则前 25 秒恒 fresh，
 *     会把纯静默会话的 60s 收尾拖到 85s）。
 * suspect 只作审计位（电平卡死可归因），**不是**独立触发源——有界收尾的唯一来源是
 * 单一 quiet_s 累加器 + 可信窗截断，不引入第二个定时器。
 */

#include "session_voice_watch.h"

#include <stddef.h>

/* 有效连续段判定：下界拒瞬态噪声、上界截断电平可信窗。
 * cfg 数值 <= 0 = 该路证据不采信（fail-safe 方向 = 更早收尾，不是更晚）。 */
static int kvw_level_sustained(const kidpal_voice_watch_t *w,
                               const kidpal_voice_watch_cfg_t *cfg)
{
    if (cfg->voice_sustain_s <= 0 || cfg->vad_trust_s <= 0) {
        return 0;
    }
    return (w->level_run_s >= cfg->voice_sustain_s &&
            w->level_run_s <= cfg->vad_trust_s) ? 1 : 0;
}

/* 引擎证据新鲜判定：asr_seen 门（本会话真的收到过 stt）+ 年龄窗。
 * asr_fresh_s <= 0 -> 不采信（同上 fail-safe 方向）。 */
static int kvw_asr_fresh(const kidpal_voice_watch_t *w,
                         const kidpal_voice_watch_cfg_t *cfg)
{
    if (cfg->asr_fresh_s <= 0 || w->asr_seen == 0) {
        return 0;
    }
    return (w->asr_age_s < cfg->asr_fresh_s) ? 1 : 0;
}

void kidpal_voice_watch_cfg_default(kidpal_voice_watch_cfg_t *cfg)
{
    if (cfg == NULL) {
        return;
    }
    /* 三个阈值取头宏单源。idle_timeout_s 刻意不写：它由调用方注入既有命名常量
     * （application.cc 的 kSessionIdleTimeoutS = 60），这里覆盖就会把单源注入点
     * 变成「两个 60 各自漂移」的第二源。 */
    cfg->voice_sustain_s = KIDPAL_VW_VOICE_SUSTAIN_S;
    cfg->asr_fresh_s = KIDPAL_VW_ASR_FRESH_S;
    cfg->vad_trust_s = KIDPAL_VW_VAD_TRUST_S;
}

void kidpal_voice_watch_reset(kidpal_voice_watch_t *w)
{
    if (w == NULL) {
        return;
    }
    /* 七字段全零：跨会话零残留（会话入口调用；漏清 asr_seen = 新会话继承上一会话
     * 的素材证据 -> D3 空素材门恒不可达）。 */
    w->quiet_s = 0;
    w->level_run_s = 0;
    w->asr_age_s = 0;
    w->tick_s = 0;
    w->diag_last_s = 0;
    w->suspect = 0;
    w->asr_seen = 0;
}

void kidpal_voice_watch_reset_soft(kidpal_voice_watch_t *w)
{
    if (w == NULL) {
        return;
    }
    /* 六清一保（[S8] 第六条）：「聆听 -> 引导播报 -> 聆听」往返不是会话边界，
     * 计时/诊断/审计六件必须归零（播报期间累积的静默秒数不得算进孩子的回答
     * 间隙），但 asr_seen（本会话素材证据）**不动**——清掉等于宣称本会话没有
     * 素材，D3 会把已有素材的故事会话判成空素材、不归档地释放。 */
    w->quiet_s = 0;
    w->level_run_s = 0;
    w->asr_age_s = 0;
    w->tick_s = 0;
    w->diag_last_s = 0;
    w->suspect = 0;
}

void kidpal_voice_watch_note_asr(kidpal_voice_watch_t *w)
{
    if (w == NULL) {
        return;
    }
    w->asr_age_s = 0;
    w->asr_seen = 1;
}

int kidpal_voice_watch_tick(kidpal_voice_watch_t *w, int vad_level,
                            const kidpal_voice_watch_cfg_t *cfg)
{
    int voice_evidence;

    if (w == NULL || cfg == NULL) {
        return KVW_ACT_NONE;
    }

    /* (1) 每 tick 恰一次：电平连续段 / 引擎年龄 / 自持 tick 计数。 */
    w->level_run_s = (vad_level != 0) ? w->level_run_s + 1 : 0;
    if (w->asr_age_s < KIDPAL_VW_ASR_AGE_SAT) {
        w->asr_age_s++;
    } else {
        w->asr_age_s = KIDPAL_VW_ASR_AGE_SAT;
    }
    w->tick_s++;

    /* (2) 两条有效证据（互补；电平只作连续段时长输入，不再是收尾必要条件）。 */
    voice_evidence = kvw_level_sustained(w, cfg) || kvw_asr_fresh(w, cfg);

    /* (3) 主累加器：有证据归零，无证据 +1（无上限）。 */
    w->quiet_s = voice_evidence ? 0 : w->quiet_s + 1;

    /* (4) 电平卡死审计位：连续段越过可信窗上界即置 1（非独立触发源）。 */
    w->suspect = (cfg->vad_trust_s > 0 && w->level_run_s > cfg->vad_trust_s) ? 1 : 0;

    /* (5) 到阈值持续 FIRE（sticky，不做自我抑制——抑制会把「设备漏收口」变成
     * 不可重试的哑火）。idle_timeout_s <= 0 = 立即收尾（fail-safe 更早）。 */
    return (w->quiet_s >= cfg->idle_timeout_s) ? KVW_ACT_IDLE_FIRE : KVW_ACT_NONE;
}

int kidpal_voice_watch_diag_due(int now_s, int last_s, int interval_s,
                                int min_gap_s, int changed)
{
    int gap;

    if (interval_s <= 0 || min_gap_s <= 0) {
        return -1; /* 非法配置：不下发（零/负间隔等于每 tick 一条 = 没限频） */
    }
    if (now_s < last_s) {
        return 0; /* 时钟回退 / 复位竞态：fail-closed，诊断行不得反弹成风暴 */
    }
    gap = now_s - last_s;
    if (gap >= interval_s) {
        return 1; /* 周期放行 */
    }
    if (changed != 0 && gap >= min_gap_s) {
        return 1; /* 跳变提前放行，最小间隔封顶 */
    }
    return 0;
}