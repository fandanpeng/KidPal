/*
 * session_voice_watch.h — 语音会话「有效语音证据」看门缝（TaskPackage
 * 261116-channel-resident-core，真机轮 B 修复轮新增，2026-11-18）。
 *
 * 契约权威 = firmware/main/test/session_channel_resident.acceptance.c 文件头 [S8]
 * （冻结，逐字；真行为半由 SCR-A9..A14 承载，应用层接线由 SCR-A15 钉）。
 *
 * 存在理由（轮 B 真机证据，device-evidence/round-b/kplog-excerpt.txt）：
 *   旧 60s 无语音判据以「瞬时 VAD 电平 s_vad_speaking 为必要条件」——电平卡死
 *   （M1）时它恒假、起音沿风暴（M2）下 clock_ticks_ 反复归零，两种失效在真机日志里
 *   同构（普通会话静置 3m13s 不触发）。本缝把判据本体从「瞬时电平」换成「距上次
 *   有效语音证据的静默秒数」：
 *     - 有效 = 连续电平落在可信窗内（下界拒短促噪声暴发 = M2 解药，上界截断电平
 *       可信窗 = M1/M3 解药）**或** 引擎 stt 新鲜（语义级证据，把连续讲话保护延伸到
 *       远超可信窗）；
 *     - 电平只作「连续段时长」输入，不再是收尾的必要条件。
 *
 * 设计约束（铁律，与 diagnostics/log_semantics.h 同款）：
 *   - **零芯片依赖 / 零 ESP-IDF include / 零 FreeRTOS / 零 LVGL**；
 *   - 纯函数 + 调用方持有状态（本文件零全局可变态），host_app linux target 与设备树
 *     编**同一份 .c**（两侧 CMakeLists 各自登记），行为断言在 host 上真跑；
 *   - 时间基准 = 主循环 1s tick 显式注入（本缝不读真实时钟）。
 *
 * 状态机（每 tick 恰一次，冻结语义六条见冻结件 [S8]）：
 *   level_run_s  电平连续为真秒数（vad_level 假即归零）
 *   asr_age_s    距上次 stt 秒数（饱和 KIDPAL_VW_ASR_AGE_SAT）
 *   quiet_s      距上次**有效语音证据**秒数（主累加器；无证据才 +1）
 *   tick_s       缝自持 tick 计数（诊断限频的时钟；**不复用 clock_ticks_**——后者
 *                有两个归零点，正是 M2 成因本体）
 *   suspect      电平卡死审计位（连续段越过可信窗上界即置 1；**非独立触发源**——
 *                有界性唯一来源 = 单一 quiet_s 累加器 + 可信窗截断）
 *   asr_seen     本会话是否真的收到过引擎 stt（D3 素材门 + asr_fresh 的共同门；
 *                会话内软复位 reset_soft 保留，跨会话硬复位 reset 才清）
 */

#ifndef KIDPAL_SESSION_VOICE_WATCH_H
#define KIDPAL_SESSION_VOICE_WATCH_H

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- *
 * 冻结数值单源（冻结件另以头文件字面扫描再钉一次 = SCR-A15 附 A；改值须回 planner
 * 重冻结）。调用点不得裸写字面。
 * ------------------------------------------------------------------------ */

/* 有效语音连续段下界：短于此的连续段按「瞬态噪声」不采信（M2 起音沿风暴解药；
 * vad_min_noise_ms=100 + VAD_MODE_0 下噪声电平连续段是几百毫秒量级，3s 留一个数量级
 * 余量；短于 3s 的真话语由 asr_fresh 兜住）。 */
#define KIDPAL_VW_VOICE_SUSTAIN_S    3

/* 引擎证据新鲜窗：stt 到达后多少秒内视为「正在说话」（引擎相邻 stt 间距假设的
 * 1.25~2.5 倍；A7 连续讲保护的主要延伸臂）。 */
#define KIDPAL_VW_ASR_FRESH_S       25

/* 电平可信窗上界（严格大于 A7 的 90 秒界，留 10s 余量）：连续电平超过此秒数即视为
 * 「电平卡死」不再采信为语音证据（M1/M3 解药，有界收尾 = 上界 + idle_timeout）。 */
#define KIDPAL_VW_VAD_TRUST_S      100

/* 诊断行周期（Listening 态低频诊断 = 轮 B 可观测性；非 Listening 态零输出）。 */
#define KIDPAL_VW_DIAG_INTERVAL_S   10

/* 诊断行最小间隔：跳变触发（电平/标志翻转、suspect 0->1、会话入口）受此封顶，
 * 防抖动刷屏。 */
#define KIDPAL_VW_DIAG_MIN_GAP_S     3

/* asr_age_s 饱和值（防长静置整数溢出的上限，纯审计量）。 */
#define KIDPAL_VW_ASR_AGE_SAT      999

/* tick() 返回动作闭集。到阈值后**持续返回** KVW_ACT_IDLE_FIRE（sticky，不做自我
 * 抑制——抑制会把「设备漏收口」变成不可重试的哑火）。 */
#define KVW_ACT_NONE       0
#define KVW_ACT_IDLE_FIRE  1

/* 阈值配置。idle_timeout_s **不由 cfg_default 填写**——由调用方注入既有命名常量
 * （application.cc 用 kSessionIdleTimeoutS = 60，单源、防两个 60 各自漂移）。 */
typedef struct {
    int idle_timeout_s, voice_sustain_s, asr_fresh_s, vad_trust_s;
} kidpal_voice_watch_cfg_t;

/* 看门状态（调用方持有，零全局态）。 */
typedef struct {
    int quiet_s, level_run_s, asr_age_s, tick_s, diag_last_s, suspect, asr_seen;
} kidpal_voice_watch_t;

/* 阈值默认值：填 voice_sustain_s / asr_fresh_s / vad_trust_s 三件头宏；
 * idle_timeout_s **保留调用方注入值**（不覆盖）。cfg == NULL -> no-op。 */
void kidpal_voice_watch_cfg_default(kidpal_voice_watch_cfg_t *cfg);

/* 会话入口复位：七字段全零（含 asr_seen / suspect / diag_last_s）。
 * w == NULL -> no-op。 */
void kidpal_voice_watch_reset(kidpal_voice_watch_t *w);

/* 会话内软复位（冻结件 [S8] 第六条）：清 quiet_s / level_run_s / asr_age_s /
 * tick_s / diag_last_s / suspect 六件，**保留 asr_seen**——本会话的素材证据在
 * 「聆听 -> 播报 -> 聆听」往返中不得丢失。
 * 存在理由：记录会话的引导播报（TTS）造成 Listening -> Speaking -> Listening；
 * 回到聆听若走跨会话全清 reset，asr_seen 被清成 0，D3 空素材 producer 随后会把
 * 「孩子已经讲过话、有素材」的故事会话判成空素材，静置 60 秒不归档地释放（孩子
 * 已讲内容丢失）。跨会话边界仍走 kidpal_voice_watch_reset（七字段全清）——两种
 * 复位语义不同，不得互相替代。
 * 应用层分派形态由冻结件 EQ-4 钉：旧状态比较必须写在 Listening case 内。
 * w == NULL -> no-op。 */
void kidpal_voice_watch_reset_soft(kidpal_voice_watch_t *w);

/* 引擎 stt 到达：asr_age_s = 0 且 asr_seen = 1（本会话「真的有素材」的证据）。
 * w == NULL -> no-op。调用上下文 = 主循环（Schedule lambda 内），单写者零锁。 */
void kidpal_voice_watch_note_asr(kidpal_voice_watch_t *w);

/* 每 1 秒恰一次推进状态机；返回 KVW_ACT_NONE / KVW_ACT_IDLE_FIRE。
 *   vad_level != 0 -> 电平为真（只作连续段时长输入，**不是**收尾必要条件）。
 *   判据 = quiet_s >= cfg->idle_timeout_s（到阈值后持续 FIRE）。
 * NULL 入参 -> 返回 KVW_ACT_NONE 且不写字段（绝不崩）；
 * cfg 数值 <= 0 时该路证据按「不采信」降级（fail-safe 方向 = 更早收尾）。 */
int kidpal_voice_watch_tick(kidpal_voice_watch_t *w, int vad_level,
                            const kidpal_voice_watch_cfg_t *cfg);

/* 诊断行限频决策（纯函数、幂等、无隐藏状态）。决策表：
 *   interval_s <= 0 或 min_gap_s <= 0 -> -1（非法配置 = 不下发，绝不退化成每 tick 一条）；
 *   now_s < last_s                    -> 0 （时钟回退 fail-closed）；
 *   now_s - last_s >= interval_s      -> 1 （周期放行）；
 *   changed != 0 且 gap >= min_gap_s  -> 1 （跳变提前放行，受最小间隔封顶）；
 *   否则                              -> 0。
 * 调用点语义：now_s = 缝自持 tick_s，emit 后把 diag_last_s 写回 now_s。 */
int kidpal_voice_watch_diag_due(int now_s, int last_s, int interval_s,
                                int min_gap_s, int changed);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_SESSION_VOICE_WATCH_H */