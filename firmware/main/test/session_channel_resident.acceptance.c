/*
 * Frozen acceptance tests — TaskPackage 261116-channel-resident-core (subsystem:
 * firmware; mergeOrder 77; rigor=full / riskTier=high; P1 of the
 * 261115-session-channel-resident train).
 *
 * 承载 story = S-F1.4（日程提醒：会话间隙提醒必达）+ requirements §2.1（结束三通道：
 * 60s 无语音超时 / 云端结束意图上游 / 再按 KEY）+ §2.3（录音态子态按键口径）+
 * S-F2.2-AC4（取消与三式收尾回归）。语义权威 = docs/architecture.md §6.4（通道常驻）
 * + ADR-019（会话生命周期与 WS 通道生命周期解耦）+ 契约 §四 listen / kidpal_keepalive。
 * 技术方案 = delivery/taskpackages/261116-channel-resident-core/plan.md「技术方案
 * （planner+architect 预评）」T1–T4 与「验收方向修订建议」1–9。
 *
 * 本文件是 P1 新增的 per-package 冻结验收（SCR-A1..A15）。真机半
 * （轮 A A1–A13）归 human 持板证据，不做 host 伪造。
 *
 * 真机轮 A 修复轮（2026-11-17，human「三个问题都修」）追加 SCR-A8（D3 空素材会话空闲
 * 收尾 producer）。设备结果标记（前置升级闸触发输入，成对；路径须与 launch-card
 * `deviceOutcome.hostUncompilablePaths` 申报一致）:
 *   device-outcome: SCR-A8
 *   host-uncompilable-path: firmware/main/application.cc
 * **范围收窄（真机轮 B 修复轮 2026-11-18）**：判据本体已迁入 host 可编纯逻辑缝
 * session_voice_watch.{c,h}（[S8]），SCR-A8/A9–A14 的判据语义改由 host 真行为断言
 * 承载；`device-outcome` 标记保留但覆盖范围收窄为「接线与主循环运行时」——真机只剩
 * 「真 VAD 电平进入缝、返回动作驱动收口」的端到端确认。
 *
 * 真机轮 B 修复轮（2026-11-18，human「按 B 修」；plan.md「### 真机轮 B 修复轮
 * （2026-11-18）技术方案」）三处改动，设计已冻结，本文件不改设计只重锚/补断言:
 *   (a) SCR-A5 电平门极性反转 + 补 tick 正锚（旧判据的瞬时电平必要条件退役）；
 *   (b) SCR-A8 素材门换源（`!s_story_rec_entered` → 缝内 `asr_seen`）+ VAD 电平门
 *       计数条整条退役（同一事实单一承载点，防 co-red 噪声）；
 *   (c) 新增 SCR-A9–A15（缝真行为 8 条 + 应用层接线结构 1 条）。
 * 逐条「旧钉 → 新钉 → 为什么」见文件尾 REVISION LOG。
 *
 * ===========================================================================
 * Frozen seam contract pinned by this file（builder implements；本文件只读扫真实工作树
 * firmware/main/application.cc —— 结构齿 = 源锚代理，逐 byte 行为归真机轮）:
 *
 *   [S1] 收尾点收口（T1）：四个收尾点一律不再「拆通道 + 降 LOW_POWER + 5s 重建」，
 *        改投统一会话结束处理器。形态两类、不得混用：
 *        - 协议回调上下文 `Application::CloseAudioChannel()`（历史名保留，语义 = 会话
 *          收尾）→ 投事件位 `EndVoiceSession()`（不可直调 handler——跨任务不安全）；
 *        - 主循环上下文 `HandleToggleChatEvent` Listening 分支 / `WakeWordInvoke`
 *          Listening 分支 → 直调 `HandleEndVoiceSessionEvent()`（保同步语义）。
 *        四区共同不变量：区内 0 处 `protocol_->CloseAudioChannel(`、0 处
 *        `ScheduleResidentRebuild(`、0 处 `SetPowerSaveLevel(PowerSaveLevel::LOW_POWER)`；
 *        四个 `src=` 审计 token（discard_record / toggle_chat / overlay_close /
 *        wake_invoke）留在各自调用点、恒在世。
 *        needle 收窄说明（修订建议 1）：负锚用 `protocol_->CloseAudioChannel(` 而非
 *        `CloseAudioChannel(`——区切 helper 的 start needle 自身即 `...CloseAudioChannel()`，
 *        宽 needle 结构上不可满足（恒命中 start 自身）。
 *
 *   [S2] 统一处理器动作序（T1）：`HandleEndVoiceSessionEvent` 区 = 唯一收口点：
 *        `!protocol_` 早退 + `auto_connect_in_flight_` 守卫 + 三态门（Connecting /
 *        Listening / Speaking）逐字保留；Speaking 分支 `ResetDecoder()` + `AbortSpeaking`
 *        不动；`protocol_->SendStopListening()` 恰 1（替 `CloseAudioChannel()`）；
 *        `SetDeviceState(kDeviceStateIdle)` 保留；`SetPowerSaveLevel(LOW_POWER)` 与
 *        `ScheduleResidentRebuild()` 删除。
 *
 *   [S3] 五个真拆通道点保绿（must-stay-green）：HandleNetworkDisconnectedEvent /
 *        Reboot / UpgradeFirmware / SetAecMode / ResetProtocol 五区各 `protocol_->
 *        CloseAudioChannel()` 恰 1（既有语义零偏移）。
 *
 *   [S4] Idle 分支八件清位链逐条保留（must-stay-green）：EnableVoiceProcessing(false)
 *        / EnableWakeWordDetection(true) / kidpal_story_session_end /
 *        KidpalStorySessionEndSync / keybar 场景复位（REMINDER 在顶豁免：keyword
 *        `front_is_reminder`）/ kidpal_render_node_voice_end /
 *        kidpal_story_session_ctx_set(0) / RaiseReminderQueue。
 *
 *   [S5] 60s 无语音超时 producer（T2；真机轮 B 修复轮重锚为「纯逻辑缝 + 接线」形态）：
 *        判据本体迁入 host 可编纯逻辑缝 `firmware/main/session_voice_watch.{c,h}`
 *        （冻结语义见 [S8]），application.cc 主循环 CLOCK_TICK 区只留接线：每 1 秒
 *        恰一次 `kidpal_voice_watch_tick(`（电平入参 = `s_vad_speaking`）→ 返回
 *        KVW_ACT_IDLE_FIRE 时投统一收口处理器 `HandleEndVoiceSessionEvent()`。
 *        命名常量 `kSessionIdleTimeoutS`（值 60）经 Initialize 区单个赋值点注入
 *        `cfg.idle_timeout_s`（单源；两个 60 常量各自漂移 = 红）。
 *        **电平门极性反转（轮 B 真机证据）**：旧钉「CLOCK_TICK 区含 `!s_vad_speaking`」
 *        来自「瞬时电平作必要条件」的旧判据；新判据以缝内 `quiet_s`（距上次有效语音
 *        证据秒数）承载，电平只作「连续段时长」输入，CLOCK_TICK 区
 *        `!s_vad_speaking` 冻结为**恰 0**（电平门不得回潮）。
 *        说话沿重置：MAIN_EVENT_VAD_CHANGE 区 `clock_ticks_ = 0` 零 diff（`clock_ticks_`
 *        降级为诊断字段），真实计时重置改由 Listening 入口 `kidpal_voice_watch_reset(`
 *        承载。
 *        must-stay-green 地雷：producer 只用命名常量 + 收尾调用，**不得**接上纯模型
 *        事件 `SES_EVT_SESSION_TIMEOUT_60S`（voice_round_ui.acceptance.c VQ-E1 tooth 2
 *        钉全 firmware/main 恰 2 文件）。
 *
 *   [S6] 心跳 no-op 分支（T3）：InitializeProtocol 的 OnIncomingJson lambda 内新增
 *        `kidpal_keepalive` 分支（直接 return），位次 = `#endif`（CONFIG_RECEIVE_CUSTOM_
 *        MESSAGE 块尾）之后、`Unknown message type` ESP_LOGW 之前（偏移量断言）。
 *
 *   [S7] 自愈巡检（T4，同 CLOCK_TICK 区）：判据 = `GetDeviceState() == kDeviceStateIdle`
 *        && `protocol_` && `!auto_connect_in_flight_` && `!protocol_->IsAudioChannelOpened()`
 *        → `TryStartAutoConnect()`。守卫短路顺序承重：`auto_connect_in_flight_` 偏移
 *        **先于** `IsAudioChannelOpened(`（修订建议 9 = 降频机制机判属性）。隔离：巡检
 *        区 0 处 `OpenAudioChannel(`（零第二条重连路径）。
 *
 *   [S8] 纯逻辑语音看门缝（真机轮 B 修复轮新增；home =
 *        firmware/main/session_voice_watch.{c,h}，新建；C linkage、零芯片依赖、
 *        零 ESP-IDF include——host_app linux target 与设备树同源，接线见两侧
 *        CMakeLists）。冻结签名见本文件顶部 `[S8]` 声明块，冻结语义六条：
 *        (1) tick() 每 1 秒恰一次：`level_run_s = vad_level ? level_run_s+1 : 0`；
 *            `asr_age_s = min(asr_age_s+1, 999)`；`tick_s++`。
 *        (2) `sustained = level_run_s >= cfg->voice_sustain_s &&
 *            level_run_s <= cfg->vad_trust_s`。
 *        (3) `asr_fresh = (w->asr_seen != 0) && (asr_age_s < cfg->asr_fresh_s)`
 *            ——`asr_seen` 是必须的门：reset 后 `asr_age_s` 归零，缺此门则前 25 秒
 *            恒 fresh，把 60s 收尾拖到 85s（这正是本文件要抓到的一类错误）。
 *        (4) `voice_evidence = sustained || asr_fresh`；
 *            `quiet_s = voice_evidence ? 0 : quiet_s+1`；
 *            `suspect = (level_run_s > cfg->vad_trust_s) ? 1 : 0`（审计位，非独立触发源）；
 *            返回 `quiet_s >= cfg->idle_timeout_s ? KVW_ACT_IDLE_FIRE : KVW_ACT_NONE`。
 *        (5) `reset()` 全字段归零（含 `asr_seen`）；`note_asr()` 令 `asr_age_s = 0`
 *            且 `asr_seen = 1`；`cfg_default()` 填五个头宏（voice_sustain / asr_fresh /
 *            vad_trust / 诊断两件），`idle_timeout_s` 由调用方注入（application.cc 用
 *            既有 `kSessionIdleTimeoutS = 60`）。
 *            **第六件 `reset_soft()` = 会话内软复位**：清 quiet_s / level_run_s /
 *            asr_age_s / tick_s / diag_last_s / suspect 六件，**保留 asr_seen**——
 *            本会话的素材证据在「聆听→播报→聆听」往返中不得丢失。存在理由：引导
 *            播报让设备 Listening → Speaking → Listening，若回到聆听时走全清
 *            `reset()`，`asr_seen` 被清成 0，D3 空素材 producer 随后会把一个「孩子
 *            已经讲过话、有素材」的故事会话判成空素材，静置 60 秒后不归档地释放
 *            （孩子已讲的内容丢失）。跨会话边界仍走 `reset()` 七字段全清——两种复位
 *            语义不同，不得互相替代。
 *        (6) `diag_due()` 决策表：`interval_s<=0 || min_gap_s<=0` -> -1；
 *            `now_s < last_s` -> 0；`now_s-last_s >= interval_s` -> 1；
 *            `changed != 0 && now_s-last_s >= min_gap_s` -> 1；否则 0。
 *        头宏（单一源；本文件另以头文件字面扫描再钉一次）：
 *        KIDPAL_VW_VOICE_SUSTAIN_S=3 / ASR_FRESH_S=25 / VAD_TRUST_S=100 /
 *        DIAG_INTERVAL_S=10 / DIAG_MIN_GAP_S=3 / ASR_AGE_SAT=999；
 *        动作值 KVW_ACT_NONE=0 / KVW_ACT_IDLE_FIRE=1。
 *        应用层接线（builder 实现，SCR-A15 逐条断言）：CLOCK_TICK 区 tick 恰 1；`stt`
 *        分支 `kidpal_voice_watch_note_asr(` 恰 1；状态迁移监听器把 `old_state` 存进
 *        file-static（如 `s_watch_prev_state`），Listening case 尾部按迁移条件分派：
 *        旧状态 == `kDeviceStateSpeaking` -> `kidpal_voice_watch_reset_soft(&s_voice_watch)`；
 *        否则 -> `kidpal_voice_watch_reset(&s_voice_watch)`（两条带实参字面各 >=1 且
 *        `kDeviceStateSpeaking` 在同一区内 = 复位是迁移条件化的，不是无条件软复位，
 *        也不是无条件全清）；之后仍 `s_vad_speaking = false;` 与 `src=enter` 诊断行。
 *        CLOCK_TICK 区 `!s_vad_speaking` 恰 0；Initialize 区
 *        `kSessionIdleTimeoutS` 与 cfg 接线在世；CLOCK_TICK 区 `!s_story_rec_entered`
 *        恰 0（D3 素材门改读缝的 `asr_seen`）。
 *
 * ---------------------------------------------------------------------------
 * 强度申报（契约条目 -> 承载断言 -> 性质档；acceptance-evidence T2 纪律）:
 *   §6.4 通道常驻 / ADR-019 解耦 -> SCR-A1（四收尾点负锚 + 正锚闭集）机制级（文本源锚
 *     代理）+ SCR-A3（五真拆通道点正锚）机制级; 行为本体（真实 run 时通道不拆、提醒
 *     仍达）= 真机轮 A4/A8。
 *   §2.1 60s 超时 -> SCR-A5 机制级（命名常量值 60 + 判据 token + VAD 沿重置）+ SCR-A9
 *     / SCR-A11 / SCR-A12 **行为级**（缝真行为：纯静默恰第 60 秒收尾 / 起音沿风暴不拖
 *     不误 / 连续讲话与有 stt 会话零误收）; 实时「60s 首发生」= 真机轮 A6，「连续讲
 *     >90s 不误收」= 轮 A A7（新判据在 host 半已可判，真机只剩端到端确认）。
 *   §2.3 录音态按键口径 / S-F2.2-AC4 -> SCR-A1（toggle / wake-invoke 区收口）+ SCR-A3
 *     （五真拆通道点）机制级; 端到端全链 = 真机轮 A5。
 *   契约 §四 kidpal_keepalive -> SCR-A6 机制级（字面 + 偏移位次 = 防 SD 日志噪声冲轮转）。
 *   契约 §四 listen（自愈巡检侧）-> SCR-A7 机制级（判据 token + 守卫短路序 + 零第二条
 *     重连路径负锚）。
 *   requirements §2.1「记录会话不适用 60s 超时」的收窄 / device-round-a-evidence §二
 *     -> SCR-A8 **源锚代理**（常量 60 + 定义位次 + 记录语境正向门 + 素材门换源负锚 +
 *     审计 token + 零第二条重连路径 / 零 %10 负锚）+ SCR-A10 **行为级**（电平卡死
 *     有界收尾 = 轮 B 唯一离线警报）; 运行时本体（空素材记录会话静置 ~60s 自动回列表、
 *     零归档）仍落 host 不可编面 firmware/main/application.cc（头注 device-outcome
 *     标记，范围已收窄为接线与主循环）。
 *   轮 B 可观测性（human 必办）-> SCR-A14 行为级（诊断限频决策表）+ 结构半（token /
 *     字段 / 零 `clock_ticks_ % 10` 双保险）。
 *   缝契约 [S8] 六条 -> SCR-A9/A10/A11/A12/A13/A14 行为级（决策表与不变量 host 可判）;
 *     应用层接线六件 -> SCR-A15 源锚代理（application.cc 不进 host 构建）。
 *   会话内软复位（reset_soft 保 asr_seen；2026-11-18 tighten）-> SCR-A13 **行为级**
 *     （六清一保 + 硬复位仍全清）+ SCR-A15 结构半（Listening case 内两条带实参调用与
 *     `kDeviceStateSpeaking` 同时在场 = 迁移条件分派）。「真机引导播报后静置 60 秒不误
 *     释放、孩子已讲内容仍归档」的端到端行为 = 真机轮 B 重测，本文件只钉判据与接线
 *     （与 GAP-4 同族；host 不可编面 = firmware/main/application.cc）。
 *   弱于契约处（显式缺口，不得当已兑现）:
 *     GAP-1 「四收尾点当刻不再拆通道 + 5s 不重建」的运行时观测 = 真机 KPLOG029 温通道
 *       进聆听 ≤0.82s（轮 A A1/A2）; host 仅钉源锚（结构）。
 *     GAP-2 「自愈巡检使 3 次退避变长跑（无上限）」形态 = P2 封顶长跑承接，本包只在
 *       crossPackageBehaviors 登记; 真机观测 = 轮 A A3/A12。
 *     GAP-3 IsTimeout ESP_LOGE 频率降级为事实降频（守卫短路）→ 未量测，真机轮带量测回报。
 *     GAP-4 SCR-A8 结构半 + SCR-A10 行为半证明「判据在有界时间收尾」；但「空素材记录
 *       会话真机静置 ~60s 自动回列表 + 零 story_list / 零归档 + ≥10s 后普通问答恢复」
 *       的端到端行为 = 轮 B B-D3 重测（三源对账：设备 KPLOG `story_empty_session_timeout`
 *       + front 无 story_list 推送 + 引擎语境释放）。host 不可编面 =
 *       firmware/main/application.cc（头注 device-outcome，范围收窄）。
 *     GAP-5 「真实 VAD 电平在设备上进入缝」的接线段 = SCR-A15 源锚代理；电平值与缝
 *       判据的端到端一致性由真机轮 B 诊断行（`listening_diag` 的 `vad=` / `run=` /
 *       `quiet=` / `asr=` / `suspect=`）承载。
 *   行为等价假设（显式；不写即默认不成立）:
 *     EQ-1 「source-token 在世界 ≙ 行为兑现」：结构断言证明代码在场，真行为归轮 A。
 *     EQ-2 「区切 start needle 含自身 ≙ 负锚需收窄」：本文件所有以成员函数签名起切者，
 *       负锚一律用不含量于 start 自身的具体 needle（如 `protocol_->CloseAudioChannel(`）。
 *     EQ-3 「缝 tick 序列 ≙ 设备主循环每秒一次」：设备侧由 1s clock timer 驱动
 *       （`skip_unhandled_events=true` + 事件位幂等 ⇒ 只会漏 tick 不会多 tick，计时偏慢
 *       方向安全）; 本文件以显式注入 tick 序列承载同一状态机，不假设真实时钟。
 *     EQ-4 「Listening 区内出现 `kDeviceStateSpeaking` ≙ 复位按迁移条件分派」（2026-11-18
 *       tighten）：SCR-A15 钉的是 planner 冻结的接线形态——旧状态比较写在 Listening
 *       case 内。把比较上移到状态迁移监听器（例如在监听器里算出一个「来自 Speaking」
 *       的布尔再在 case 内读）属语义等价的另一种写法，但会在该断言下红：这不是假红，
 *       而是冻结形态的要求（[S8] 应用层接线节给出唯一形态）；builder 须按该形态落位。
 *       反过来，「无条件软复位」与「无条件全清复位」两种真错各自被
 *       `kidpal_voice_watch_reset(&s_voice_watch)` / `reset_soft(&s_voice_watch)`
 *       计数与 `kDeviceStateSpeaking` 三件联立杀死（见文件尾 REVISION LOG 变异表）。
 * ===========================================================================
 */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>

/* =========================================================================== *
 * [S8] 纯逻辑语音看门缝（home = firmware/main/session_voice_watch.{c,h}，builder 新建）。
 * 头在世则直接消费真实声明（单源）；头缺席时用本地最小镜像冻结签名/结构体/宏，使
 * 本文件在 builder 落地前**编译得过、跑起来红**（weak 哨兵返回 -2 / 不写字段 ->
 * 行为断言读作「frozen seam not implemented」，不是链接错误）。头出现后 builder 的
 * 强符号覆盖本文件 weak 哨兵（char_stroke / time_sync 同款先例）。
 * 镜像与头必须逐字一致；任何偏离以头为准（头文件字面由 SCR-A15 再钉一次）。
 * =========================================================================== */
#if defined(__has_include)
#  if __has_include("../session_voice_watch.h")
#    define SCR_VW_HEADER_PRESENT 1
#    include "../session_voice_watch.h"
#  endif
#endif

#ifndef SCR_VW_HEADER_PRESENT
/* ---- 本地最小镜像（头缺席时的冻结契约；与头逐字一致）---- */
#define KIDPAL_VW_VOICE_SUSTAIN_S    3
#define KIDPAL_VW_ASR_FRESH_S       25
#define KIDPAL_VW_VAD_TRUST_S      100
#define KIDPAL_VW_DIAG_INTERVAL_S   10
#define KIDPAL_VW_DIAG_MIN_GAP_S     3
#define KIDPAL_VW_ASR_AGE_SAT      999

#define KVW_ACT_NONE       0
#define KVW_ACT_IDLE_FIRE  1

typedef struct {
    int idle_timeout_s, voice_sustain_s, asr_fresh_s, vad_trust_s;
} kidpal_voice_watch_cfg_t;
typedef struct {
    int quiet_s, level_run_s, asr_age_s, tick_s, diag_last_s, suspect, asr_seen;
} kidpal_voice_watch_t;
#endif /* SCR_VW_HEADER_PRESENT */

/* 头存在但缺宏时的哨兵回退：编译不破、宏字面断言转红（而不是编译通道报错——冻结
 * 相位要求编译通道 error 为零）。 */
#ifndef KIDPAL_VW_VOICE_SUSTAIN_S
#  define KIDPAL_VW_VOICE_SUSTAIN_S (-1)
#endif
#ifndef KIDPAL_VW_ASR_FRESH_S
#  define KIDPAL_VW_ASR_FRESH_S (-1)
#endif
#ifndef KIDPAL_VW_VAD_TRUST_S
#  define KIDPAL_VW_VAD_TRUST_S (-1)
#endif
#ifndef KIDPAL_VW_DIAG_INTERVAL_S
#  define KIDPAL_VW_DIAG_INTERVAL_S (-1)
#endif
#ifndef KIDPAL_VW_DIAG_MIN_GAP_S
#  define KIDPAL_VW_DIAG_MIN_GAP_S (-1)
#endif
#ifndef KIDPAL_VW_ASR_AGE_SAT
#  define KIDPAL_VW_ASR_AGE_SAT (-1)
#endif
#ifndef KVW_ACT_NONE
#  define KVW_ACT_NONE     (-1)
#endif
#ifndef KVW_ACT_IDLE_FIRE
#  define KVW_ACT_IDLE_FIRE (-1)
#endif

/* 冻结签名原型（头缺席时由镜像提供结构体；两路径共用同一组声明）。
 * 本组声明在「头在场」与「头缺席」两分支下都生效：C 语言里同型重复声明合法，故即便
 * 头文件已经声明了同名函数也不冲突（2026-11-18 tighten 新增 reset_soft——头文件在
 * 本轮落地时尚无该原型，本文件自带声明 + weak 哨兵使新断言在设计态必红）。 */
void kidpal_voice_watch_cfg_default(kidpal_voice_watch_cfg_t *cfg);
void kidpal_voice_watch_reset(kidpal_voice_watch_t *w);
void kidpal_voice_watch_reset_soft(kidpal_voice_watch_t *w);
void kidpal_voice_watch_note_asr(kidpal_voice_watch_t *w);
int kidpal_voice_watch_tick(kidpal_voice_watch_t *w, int vad_level,
                            const kidpal_voice_watch_cfg_t *cfg);
int kidpal_voice_watch_diag_due(int now_s, int last_s, int interval_s,
                                int min_gap_s, int changed);

/* weak 哨兵：-2 不在任何合法返回域（tick 合法 = {0,1}；diag_due 合法 = {-1,0,1}）。
 * void 哨兵原则上不写字段——调用方 poison 保留，行为断言据此转红；唯一例外 =
 * reset_soft 哨兵刻意把 asr_seen 清 0（见其上方注释：等价于按旧全清语义兜底）。 */
#define SCR_VW_UNIMPLEMENTED (-2)

__attribute__((weak)) void kidpal_voice_watch_cfg_default(kidpal_voice_watch_cfg_t *cfg)
{
    (void)cfg;
}
__attribute__((weak)) void kidpal_voice_watch_reset(kidpal_voice_watch_t *w)
{
    (void)w;
}
/* 软复位哨兵：**刻意把 asr_seen 清 0**（= 按全清 reset 的旧语义兜底）。设计态 /
 * 未实现态下调用方已把 asr_seen 置 1，哨兵把它清掉 -> SCR-A13 的「软复位保素材」
 * 断言必红；实现落地后强符号覆盖本哨兵，断言转绿。 */
__attribute__((weak)) void kidpal_voice_watch_reset_soft(kidpal_voice_watch_t *w)
{
    if (w != NULL) {
        w->asr_seen = 0;
    }
}
__attribute__((weak)) void kidpal_voice_watch_note_asr(kidpal_voice_watch_t *w)
{
    (void)w;
}
__attribute__((weak)) int kidpal_voice_watch_tick(kidpal_voice_watch_t *w, int vad_level,
                                                  const kidpal_voice_watch_cfg_t *cfg)
{
    (void)w;
    (void)vad_level;
    (void)cfg;
    return SCR_VW_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_voice_watch_diag_due(int now_s, int last_s, int interval_s,
                                                      int min_gap_s, int changed)
{
    (void)now_s; (void)last_s; (void)interval_s; (void)min_gap_s; (void)changed;
    return SCR_VW_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * 被扫文件（真实工作树; root 自手持 cwd 定位——evidence 类扫描齿 root 注入纪律，
 * 禁依赖调用点 cwd; test 目录不进扫描, 本文件自身注释合法提及 needle）。
 * ------------------------------------------------------------------------ */
#define SCR_APP_CC "firmware/main/application.cc"

#define SCR_MAX_PATH 1024
#define SCR_MAX_READ (8u * 1024u * 1024u)

static char scr_root[SCR_MAX_PATH];
static int scr_root_ready;

static int scr_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int scr_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../..", "../../../../../../.."
    };
    char probe[SCR_MAX_PATH];

    if (scr_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (scr_is_regular(probe)) {
            snprintf(scr_root, sizeof(scr_root), "%s", cands[i]);
            scr_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int scr_rel(char *out, size_t cap, const char *rel)
{
    if (!scr_locate_root()) {
        return 0;
    }
    snprintf(out, cap, "%s/%s", scr_root, rel);
    return 1;
}

static int scr_exists(const char *rel)
{
    char p[SCR_MAX_PATH];
    return scr_rel(p, sizeof(p), rel) && scr_is_regular(p);
}

static char *scr_read(const char *rel, size_t *out_len)
{
    char p[SCR_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!scr_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > SCR_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int scr_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int scr_count_in_slice(const char *slice, const char *needle)
{
    int count = 0;
    const char *h;
    size_t n;

    if (slice == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = slice; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    return count;
}

/* Slice of rel from the FIRST occurrence of start_needle up to (exclusive) the first
 * occurrence of end_needle AFTER it. Returns a malloc'd NUL-terminated slice or NULL.
 * Caller frees. end_needle == NULL -> slice to EOF. NOTE: the start needle itself is
 * INSIDE the slice (span = e - s) —— 以成员函数签名起切时负锚须收窄避开 start 自身。 */
static char *scr_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = scr_read(rel, NULL);
    const char *s, *e;
    char *slice;
    size_t span;

    if (buf == NULL) {
        return NULL;
    }
    s = strstr(buf, start_needle);
    if (s == NULL) {
        free(buf);
        return NULL;
    }
    if (end_needle == NULL) {
        e = buf + strlen(buf);
    } else {
        e = strstr(s + strlen(start_needle), end_needle);
        if (e == NULL) {
            e = buf + strlen(buf);
        }
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

/* printf-style helper（boot_auto_connect KAC 先例: 动态诊断格式化进静态缓冲再进单串宏）。 */
static char scr_fm_buf[1024];
static const char *scr_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *scr_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(scr_fm_buf, sizeof(scr_fm_buf), fmt, ap);
    va_end(ap);
    return scr_fm_buf;
}

#define SCR_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(scr_locate_root(), \
    "SCR RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* -------------------------------------------------------------------------- *
 * [S8] 缝真行为夹具（fixture 自含性，显式声明）：
 *   - cfg 先 poison（0xA5）再 cfg_default()：cfg_default 漏填字段必红；idle_timeout_s
 *     按冻结设计由调用方注入 60（application.cc 用既有 kSessionIdleTimeoutS = 60）。
 *   - w 先 poison 再 reset()：reset 漏清任何字段（含 asr_seen / diag_last_s）必红。
 *   - 零外部资产 / 零真实时钟 / 零环境前提——纯状态机显式注入 tick 序列。
 * ------------------------------------------------------------------------ */
#define SCR_VW_IDLE_TIMEOUT_S 60

static void scr_vw_default_cfg(kidpal_voice_watch_cfg_t *cfg)
{
    memset(cfg, 0xA5, sizeof(*cfg));
    kidpal_voice_watch_cfg_default(cfg);
    cfg->idle_timeout_s = SCR_VW_IDLE_TIMEOUT_S;
}

static void scr_vw_fresh_watch(kidpal_voice_watch_t *w)
{
    memset(w, 0xA5, sizeof(*w));
    kidpal_voice_watch_reset(w);
}

/* 文件级 needle 计数（SCR-A15：seam 源进设备构建的单点判定）。 */
static int scr_count_in_file(const char *rel, const char *needle)
{
    char *buf = scr_read(rel, NULL);
    int n;

    if (buf == NULL) {
        return 0;
    }
    n = scr_count_in_slice(buf, needle);
    free(buf);
    return n;
}

/* `#define <name> <int>` 的值解析（SCR-A15：头宏字面再钉一次；缺失 -> 哨兵值）。 */
#define SCR_VW_MACRO_ABSENT (-1000000L)
static long scr_vw_macro_value(const char *buf, const char *define_needle)
{
    const char *p;

    if (buf == NULL || define_needle == NULL) {
        return SCR_VW_MACRO_ABSENT;
    }
    p = strstr(buf, define_needle);
    if (p == NULL) {
        return SCR_VW_MACRO_ABSENT;
    }
    p += strlen(define_needle);
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    return strtol(p, NULL, 10);
}

/* 收尾点区切 start/end 锚（与既有 boot_auto_connect RC-B2 / RC-B3 / voice_round_ui
 * OCE-A1 同族; 函数定义顺序是硬锚, builder 不得在四对相邻函数间插新函数）。 */
#define SCR_HELPER_START  "void Application::CloseAudioChannel()"
#define SCR_HELPER_END    "void Application::HandleToggleChatEvent()"
#define SCR_TOGGLE_START  "void Application::HandleToggleChatEvent()"
#define SCR_TOGGLE_END    "void Application::HandleStartListeningEvent()"
#define SCR_HANDLER_START "void Application::HandleEndVoiceSessionEvent()"
#define SCR_HANDLER_END   "void Application::HandleWakeWordDetectedEvent()"
#define SCR_WAKEINV_START "void Application::WakeWordInvoke("
#define SCR_WAKEINV_END   "bool Application::CanEnterSleepMode()"

/* =========================================================================== *
 * SCR-A1 — 四收尾点收口（负锚三件 + 正锚闭集 + src token 在世）。
 * 设计态红: 四区今天各自 protocol_->CloseAudioChannel() + SetPowerSaveLevel(LOW_POWER)
 *           + ScheduleResidentRebuild()（拆通道旧形态）。
 * =========================================================================== */
/* coverage: SCR-A1 -- 四收尾点（CloseAudioChannel helper / HandleToggleChatEvent Listening / HandleEndVoiceSessionEvent / WakeWordInvoke Listening）负锚：区内 protocol_->CloseAudioChannel( = 0 + ScheduleResidentRebuild( = 0 + SetPowerSaveLevel(PowerSaveLevel::LOW_POWER) = 0；正锚闭集：helper 区 EndVoiceSession(（事件位）、toggle/wake-invoke 区 HandleEndVoiceSessionEvent(（主循环直调）、handler 区 SendStopListening(（收口动作）；四 src 审计 token 各自在区 */
TEST_CASE("会话通道常驻: 四收尾点收口负锚 (SCR-A1)",
          "[session_channel_resident][SCR-A1][structural]")
{
    static const struct {
        const char *start;
        const char *end;
        const char *label;
        const char *src_token;
    } sites[] = {
        { SCR_HELPER_START, SCR_HELPER_END, "CloseAudioChannel helper", "src=discard_record" },
        { SCR_TOGGLE_START, SCR_TOGGLE_END, "ToggleChat Listening", "src=toggle_chat" },
        { SCR_HANDLER_START, SCR_HANDLER_END, "EndVoiceSession handler", "src=overlay_close" },
        { SCR_WAKEINV_START, SCR_WAKEINV_END, "WakeWordInvoke Listening", "src=wake_invoke" },
    };
    char *region;
    int n;
    size_t i;

    SCR_REQUIRE_ROOT();

    for (i = 0; i < sizeof(sites) / sizeof(sites[0]); i++) {
        region = scr_region_between(SCR_APP_CC, sites[i].start, sites[i].end);
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            scr_fm("SCR-A1 RED(infra): %s 区切取失败——函数定义顺序锚漂移须复核"
                   "（禁改名绕开; 禁在相邻函数间插新函数）", sites[i].label));

        /* 负锚 1：区内不再直调协议层拆通道（needle 收窄 = 修订建议 1）。 */
        n = scr_count_in_slice(region, "protocol_->CloseAudioChannel(");
        TEST_ASSERT_TRUE_MESSAGE(n == 0,
            scr_fm("SCR-A1 RED: %s 区 protocol_->CloseAudioChannel( = %d 处（冻结 0）"
                   "——收尾点仍拆通道（会话结束 = 拆通道旧形态未收口; §6.4 / ADR-019）",
                   sites[i].label, n));

        /* 负锚 2：区内不再排常驻重建（B helper 整条退役）。 */
        n = scr_count_in_slice(region, "ScheduleResidentRebuild(");
        TEST_ASSERT_TRUE_MESSAGE(n == 0,
            scr_fm("SCR-A1 RED: %s 区 ScheduleResidentRebuild( = %d 处（冻结 0）——收尾点"
                   "仍排 5s 常驻重建（断而再建绕路未除; plan T1）", sites[i].label, n));

        /* 负锚 3：区内不再降 LOW_POWER（通道在世, 省电剖面维持）。 */
        n = scr_count_in_slice(region, "SetPowerSaveLevel(PowerSaveLevel::LOW_POWER)");
        TEST_ASSERT_TRUE_MESSAGE(n == 0,
            scr_fm("SCR-A1 RED: %s 区 SetPowerSaveLevel(PowerSaveLevel::LOW_POWER) = %d "
                   "处（冻结 0）——收尾降省电档（RC-B6 观测口径重定: 通道仍在世）",
                   sites[i].label, n));

        /* 正锚：收口动作/投递在世。helper 区走 EndVoiceSession(；toggle/wake-invoke 区
         * 走 HandleEndVoiceSessionEvent()；handler 区（= 收口点本体）走 SendStopListening(。
         * helper/toggle/wake-invoke 用「二选一闭集」，兼容 builder 统一投事件位或统一直调。 */
        if (strcmp(sites[i].label, "EndVoiceSession handler") == 0) {
            TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "SendStopListening("),
                "SCR-A1 RED: HandleEndVoiceSessionEvent 区零 SendStopListening(——收口点"
                "未发 listen stop（会话结束未通知 front 收尾; 契约 §四 listen）");
        } else {
            TEST_ASSERT_TRUE_MESSAGE(
                scr_contains(region, "EndVoiceSession(") ||
                    scr_contains(region, "HandleEndVoiceSessionEvent()"),
                scr_fm("SCR-A1 RED: %s 区零收口投递（EndVoiceSession( 与 "
                       "HandleEndVoiceSessionEvent() 皆无）——收尾点未接统一会话结束处理器"
                       "（plan T1 收口形态缺位）", sites[i].label));
        }

        /* src 审计 token 在世（T1: 四字面 token 恒在世; 诊断口径区分四路）。 */
        TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, sites[i].src_token),
            scr_fm("SCR-A1 RED: %s 区缺审计 token %s——收尾诊断口径丢失（四 src 字面"
                   "token 须留在各自调用点; plan T1）", sites[i].label, sites[i].src_token));

        free(region);
    }
}

/* =========================================================================== *
 * SCR-A2 — 统一处理器动作序（SendStopListening 恰 1 + ResetDecoder 恰 1 在 Speaking
 *          分支内 + 三态门 / auto_connect_in_flight_ / !protocol_ 逐字在世）。
 * =========================================================================== */
/* coverage: SCR-A2 -- HandleEndVoiceSessionEvent 统一处理器：protocol_->SendStopListening() 恰 1 + ResetDecoder() 恰 1 且位次在 Speaking 分支内（state == kDeviceStateSpeaking < ResetDecoder < SendStopListening）+ 三态门（kDeviceStateConnecting/Listening/Speaking）+ auto_connect_in_flight_ 守卫 + !protocol_ 早退逐字在世（261024 语义零偏移） */
TEST_CASE("会话通道常驻: 统一处理器动作序 (SCR-A2)",
          "[session_channel_resident][SCR-A2][structural]")
{
    char *region;
    const char *p_speak, *p_reset, *p_stop;
    int n;

    SCR_REQUIRE_ROOT();

    region = scr_region_between(SCR_APP_CC, SCR_HANDLER_START, SCR_HANDLER_END);
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "SCR-A2 RED(infra): HandleEndVoiceSessionEvent 区切取失败——锚漂移须复核");

    /* 收口动作 = SendStopListening()（恰 1）。 */
    n = scr_count_in_slice(region, "SendStopListening()");
    TEST_ASSERT_TRUE_MESSAGE(n == 1,
        scr_fm("SCR-A2 RED: HandleEndVoiceSessionEvent 区 SendStopListening() = %d 处"
               "（冻结恰 1）——统一收口未发 listen stop（0 = 根因现状; >1 = 双发）", n));

    /* ResetDecoder() 恰 1（即时静音）。 */
    n = scr_count_in_slice(region, "ResetDecoder()");
    TEST_ASSERT_TRUE_MESSAGE(n == 1,
        scr_fm("SCR-A2 RED: HandleEndVoiceSessionEvent 区 ResetDecoder() = %d 处（冻结"
               "恰 1）——Speaking 分支即时静音缺失或重复", n));

    /* ResetDecoder 在 Speaking 分支内：state == kDeviceStateSpeaking 位次 < ResetDecoder
     * 位次 < SendStopListening 位次（后者住分支外 = 上界）。 */
    p_speak = strstr(region, "state == kDeviceStateSpeaking");
    p_reset = strstr(region, "ResetDecoder()");
    p_stop = strstr(region, "SendStopListening()");
    TEST_ASSERT_TRUE_MESSAGE(p_speak != NULL && p_reset != NULL && p_stop != NULL &&
                                 p_speak < p_reset && p_reset < p_stop,
        "SCR-A2 RED: ResetDecoder() 未落在 state == kDeviceStateSpeaking 分支内"
        "（位次须 kDeviceStateSpeaking < ResetDecoder < SendStopListening）——即时静音"
        "脱离 Speaking 门（plan T1: 仅 Speaking 复位解码队列）");

    /* 三态门 + in-flight 守卫 + !protocol_ 早退逐字在世（261024 语义零偏移）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        scr_contains(region, "kDeviceStateConnecting") &&
            scr_contains(region, "kDeviceStateListening") &&
            scr_contains(region, "kDeviceStateSpeaking"),
        "SCR-A2 RED: HandleEndVoiceSessionEvent 三态门不全（Connecting/Listening/"
        "Speaking 缺一）——统一收口的三态门被拆（261024 overlay_close 时序自证破）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "auto_connect_in_flight_"),
        "SCR-A2 RED: HandleEndVoiceSessionEvent 区零 auto_connect_in_flight_ 守卫——"
        "与后台 AutoConnect 抢 websocket_ 的竞态入口（plan T1 守卫逐字保留）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "!protocol_"),
        "SCR-A2 RED: HandleEndVoiceSessionEvent 区零 !protocol_ 早退——空协议指针"
        "解引用风险（plan T1 早退逐字保留）");

    free(region);
}

/* =========================================================================== *
 * SCR-A3 — 五个真拆通道点保绿（must-stay-green；既有语义零偏移）。
 * =========================================================================== */
/* coverage: SCR-A3 -- 五个真拆通道点（HandleNetworkDisconnectedEvent / Reboot / UpgradeFirmware / SetAecMode / ResetProtocol）各区 protocol_->CloseAudioChannel() 恰 1（must-stay-green: 真拆通道点不动, local_close_pending_ 分类零误重连不变量不动） */
TEST_CASE("会话通道常驻: 五个真拆通道保绿 (SCR-A3)",
          "[session_channel_resident][SCR-A3][structural]")
{
    static const struct {
        const char *start;
        const char *end;
        const char *label;
    } points[] = {
        { "void Application::HandleNetworkDisconnectedEvent()",
          "void Application::HandleActivationDoneEvent()", "NetworkDisconnected" },
        { "void Application::Reboot()",
          "bool Application::UpgradeFirmware(", "Reboot" },
        { "bool Application::UpgradeFirmware(",
          "void Application::WakeWordInvoke(", "UpgradeFirmware" },
        { "void Application::SetAecMode(",
          "void Application::PlaySound(", "SetAecMode" },
        { "void Application::ResetProtocol()", NULL, "ResetProtocol" },
    };
    char *region;
    int n;
    size_t i;

    SCR_REQUIRE_ROOT();

    for (i = 0; i < sizeof(points) / sizeof(points[0]); i++) {
        region = scr_region_between(SCR_APP_CC, points[i].start, points[i].end);
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            scr_fm("SCR-A3 RED(infra): %s 区切取失败——锚漂移须复核（禁整函数退役绕过）",
                   points[i].label));
        n = scr_count_in_slice(region, "protocol_->CloseAudioChannel()");
        TEST_ASSERT_TRUE_MESSAGE(n == 1,
            scr_fm("SCR-A3 RED: %s 区 protocol_->CloseAudioChannel() = %d 处（冻结恰 1）"
                   "——真拆通道点被误动（must-stay-green; 网络断/重启/OTA/AEC/Reset 五路"
                   "真拆通道语义零偏移破）", points[i].label, n));
        free(region);
    }
}

/* =========================================================================== *
 * SCR-A4 — Idle 分支八件清位链逐条保留（must-stay-green）。
 * =========================================================================== */
/* coverage: SCR-A4 -- HandleStateChangedEvent 的 case kDeviceStateIdle 区八件清位链各 >=1：EnableVoiceProcessing(false) / EnableWakeWordDetection(true) / kidpal_story_session_end / KidpalStorySessionEndSync / keybar 场景复位 kidpal_keybar_scene_reset( + REMINDER 在顶豁免 front_is_reminder / kidpal_render_node_voice_end / kidpal_story_session_ctx_set(0) / RaiseReminderQueue */
TEST_CASE("会话通道常驻: Idle八件清位链保绿 (SCR-A4)",
          "[session_channel_resident][SCR-A4][structural]")
{
    char *region;

    SCR_REQUIRE_ROOT();

    region = scr_region_between(SCR_APP_CC, "case kDeviceStateIdle:",
                                "case kDeviceStateConnecting:");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "SCR-A4 RED(infra): case kDeviceStateIdle 区切取失败——锚漂移须复核");

    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "EnableVoiceProcessing(false)"),
        "SCR-A4 RED: Idle 区零 EnableVoiceProcessing(false)——麦克风口径不变破");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "EnableWakeWordDetection(true)"),
        "SCR-A4 RED: Idle 区零 EnableWakeWordDetection(true)——唤醒检测恢复破");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "kidpal_story_session_end"),
        "SCR-A4 RED: Idle 区零 kidpal_story_session_end——[R2] 录音页态复位破"
        "（record_session_repair RSR-A2 邻居）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "KidpalStorySessionEndSync"),
        "SCR-A4 RED: Idle 区零 KidpalStorySessionEndSync——[R5] 镜像同步破（RSL2-A2）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "kidpal_keybar_scene_reset(") &&
                                 scr_contains(region, "front_is_reminder"),
        "SCR-A4 RED: Idle 区缺 keybar 场景复位 / REMINDER 在顶豁免判据——"
        "chrome_ghost CG-A1（鬼影修复 + 提醒驻留不熄）被顺删");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "kidpal_render_node_voice_end"),
        "SCR-A4 RED: Idle 区零 kidpal_render_node_voice_end——OQ3=c 前台回落破");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "kidpal_story_session_ctx_set(0)"),
        "SCR-A4 RED: Idle 区零 kidpal_story_session_ctx_set(0)——记录语境清位破");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "RaiseReminderQueue"),
        "SCR-A4 RED: Idle 区零 RaiseReminderQueue——S-F1.4 录音态提醒排队例外（轮 A A8）"
        "收尾回 Idle 立刻弹的既有离线齿（reminder_overlay_unify RM-A3 tooth 5）被顺删");

    free(region);
}

/* =========================================================================== *
 * SCR-A5 — 60s 无语音超时 producer（CLOCK_TICK 消费区 + VAD 沿重置）。
 * 真机轮 B 修复轮重锚：判据本体迁入纯逻辑缝（[S8]，真行为由 SCR-A9–A14 承载），
 * 本 case 只钉应用层接线与「电平门不得回潮」的负锚。
 * 设计态红: CLOCK_TICK 区零 kidpal_voice_watch_tick( 调用（缝接线缺席）;
 *           `!s_vad_speaking` 仍在区内 2 处（旧判据的电平门在场的现状）。
 * =========================================================================== */
/* coverage: SCR-A5 -- 60s 无语音超时 producer（requirements §2.1）：命名常量 kSessionIdleTimeoutS 定义值 60 + CLOCK_TICK 消费区 kDeviceStateListening 判据 + kidpal_voice_watch_tick( 正锚（缝接线在世）+ kidpal_story_session_ctx_active() 记录语境豁免 + HandleEndVoiceSessionEvent() 主循环直调 + 负锚 !s_vad_speaking 恰 0（电平门不得回潮；极性反转见文件尾 REVISION LOG）+ MAIN_EVENT_VAD_CHANGE 区 clock_ticks_ = 0 说话沿重置（不接纯模型 SES_EVT_SESSION_TIMEOUT_60S） */
TEST_CASE("会话通道常驻: 60s超时producer+说话沿重置 (SCR-A5)",
          "[session_channel_resident][SCR-A5][structural]")
{
    char *clock_region, *vad_region, *def_region;
    int n;

    SCR_REQUIRE_ROOT();

    clock_region = scr_region_between(SCR_APP_CC, "if (bits & MAIN_EVENT_CLOCK_TICK)",
                                      "void Application::HandleNetworkConnectedEvent()");
    TEST_ASSERT_NOT_NULL_MESSAGE(clock_region,
        "SCR-A5 RED(infra): CLOCK_TICK 消费区切取失败——锚漂移须复核");

    /* 命名常量 kSessionIdleTimeoutS，定义值 60（首定义片段含 60, RC-B2 tooth2 先例形态）。 */
    def_region = scr_region_between(SCR_APP_CC, "kSessionIdleTimeoutS", ";");
    TEST_ASSERT_NOT_NULL_MESSAGE(def_region,
        "SCR-A5 RED: application.cc 无 kSessionIdleTimeoutS 定义——60s 超时命名常量"
        "未落（纯模型无 producer = 根因现状; plan T2）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(def_region, "60"),
        "SCR-A5 RED: kSessionIdleTimeoutS 首定义片段不含 60——阈值偏离 requirements §2.1"
        "（60s 无语音; 偏离回 planner 重冻结）");
    free(def_region);

    /* 判据 token（应用层接线；判据语义本体 = [S8] 缝，真行为见 SCR-A9–A14）。 */
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(clock_region, "kDeviceStateListening"),
        "SCR-A5 RED: CLOCK_TICK 区无 kDeviceStateListening 判据——非聆听态误计时");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(clock_region, "kidpal_story_session_ctx_active()"),
        "SCR-A5 RED: CLOCK_TICK 区无 kidpal_story_session_ctx_active() 记录语境豁免——"
        "记录会话被 60s 误收（requirements §2.1 v1.65 硬语义）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(clock_region, "HandleEndVoiceSessionEvent()"),
        "SCR-A5 RED: CLOCK_TICK 区无 HandleEndVoiceSessionEvent() 投递——producer 未接"
        "统一收口（纯模型 producer = 0 的根因现状）");

    /* 正锚（新钉）：缝 tick 接线在世——每 1 秒恰一次的驱动点（真机轮 B 修复轮）。 */
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(clock_region, "kidpal_voice_watch_tick("),
        "SCR-A5 RED: CLOCK_TICK 区零 kidpal_voice_watch_tick( ——语音看门缝未被主循环"
        "驱动（判据本体住在缝里，无接线 = 缝恒不前进，两条 producer 全域哑火）");

    /* 负锚（新钉，极性反转）：瞬时电平门不得回潮。旧判据以 !s_vad_speaking 为必要
     * 条件，既救不了电平卡死（M1）也挡不住起音沿风暴（M2）；新判据以缝内 quiet_s
     * 承载，电平只作连续段时长输入。区内出现该字面 = 旧判据回潮。 */
    n = scr_count_in_slice(clock_region, "!s_vad_speaking");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        scr_fm("SCR-A5 RED: CLOCK_TICK 区 !s_vad_speaking = %d 处（冻结 0）——瞬时电平"
               "门回潮（旧判据的必要条件；电平卡死时永不收尾 = 轮 B A6 实测 3m13s "
               "不触发；判据须走缝的 quiet_s + asr_fresh）", n));
    free(clock_region);

    /* 说话沿重置：VAD_CHANGE 区 clock_ticks_ = 0（零 diff 保留；clock_ticks_ 降级为
     * 诊断字段，真实计时重置由 Listening 入口 kidpal_voice_watch_reset( 承载）。 */
    vad_region = scr_region_between(SCR_APP_CC, "if (bits & MAIN_EVENT_VAD_CHANGE)",
                                    "if (bits & MAIN_EVENT_SCHEDULE)");
    TEST_ASSERT_NOT_NULL_MESSAGE(vad_region,
        "SCR-A5 RED(infra): VAD_CHANGE 区切取失败——锚漂移须复核");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(vad_region, "clock_ticks_ = 0"),
        "SCR-A5 RED: MAIN_EVENT_VAD_CHANGE 区无 clock_ticks_ = 0——说话沿不重置计时"
        "（诊断字段失真：`ticks=` 不再能读作本会话静置时长）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(vad_region, "s_vad_speaking"),
        "SCR-A5 RED: MAIN_EVENT_VAD_CHANGE 区无 s_vad_speaking 引用——说话沿重置未"
        "按电平判据门控（plan T2 if (s_vad_speaking) clock_ticks_ = 0）");
    free(vad_region);
}

/* =========================================================================== *
 * SCR-A6 — 心跳 no-op 分支（kidpal_keepalive），位次 = #endif 之后、WARN 之前。
 * 设计态红: OnIncomingJson 无 kidpal_keepalive 分支（每 30s 心跳落 Unknown WARN,
 *          按 ~144KB/日 冲 SD 日志轮转预算）。
 * =========================================================================== */
/* coverage: SCR-A6 -- 契约 §四 kidpal_keepalive：OnIncomingJson lambda 区 kidpal_keepalive 字面 >=1，且位次在 #endif（CONFIG_RECEIVE_CUSTOM_MESSAGE 块尾）之后、Unknown message type ESP_LOGW 之前（偏移量断言 = 防心跳落 WARN 刷 SD 日志; plan T3） */
TEST_CASE("会话通道常驻: 心跳noop分支位次 (SCR-A6)",
          "[session_channel_resident][SCR-A6][structural]")
{
    char *region;
    const char *p_keep, *p_end, *p_warn;

    SCR_REQUIRE_ROOT();

    region = scr_region_between(SCR_APP_CC, "protocol_->OnIncomingJson(",
                                "void Application::Alert(");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "SCR-A6 RED(infra): OnIncomingJson lambda 区切取失败——锚漂移须复核");

    p_keep = strstr(region, "kidpal_keepalive");
    TEST_ASSERT_TRUE_MESSAGE(p_keep != NULL,
        "SCR-A6 RED: OnIncomingJson 区零 kidpal_keepalive——心跳下行帧落 Unknown "
        "message type WARN（每 30s 一条, 冲 SD 日志轮转预算 ~144KB/日; 契约 §四; plan T3）");

    /* 位次：先于 Unknown message type WARN。 */
    p_warn = strstr(region, "Unknown message type");
    TEST_ASSERT_TRUE_MESSAGE(p_warn != NULL && p_keep != NULL && p_keep < p_warn,
        "SCR-A6 RED: kidpal_keepalive 分支未先于 Unknown message type WARN——心跳仍落 "
        "else 分支（WARN 噪声未除）");

    /* 位次：#endif（CONFIG_RECEIVE_CUSTOM_MESSAGE 块尾）之后。区内首个 #endif 即该块
     * 尾（lambda 内更早无条件编译）。 */
    p_end = strstr(region, "#endif");
    TEST_ASSERT_TRUE_MESSAGE(p_end != NULL && p_keep != NULL && p_keep > p_end,
        "SCR-A6 RED: kidpal_keepalive 分支未在 #endif 之后——落在 "
        "CONFIG_RECEIVE_CUSTOM_MESSAGE 条件块内（该配置关时心跳分支消失 = WARN 噪声回归; "
        "修订建议 8）");

    free(region);
}

/* =========================================================================== *
 * SCR-A7 — 自愈巡检（CLOCK_TICK 区 IsAudioChannelOpened + TryStartAutoConnect +
 *          守卫短路序 auto_connect_in_flight_ < IsAudioChannelOpened + 零第二条重连路径）。
 * 设计态红: 通道常驻后会话间隙无下行 → 120s 判死必然到期（首按键失同步第一段根因）。
 * =========================================================================== */
/* coverage: SCR-A7 -- 自愈巡检（§6.4）：CLOCK_TICK 区 IsAudioChannelOpened( >=1 + TryStartAutoConnect( >=1 + 守卫 auto_connect_in_flight_ 且其偏移 < IsAudioChannelOpened( 偏移（降频机制机判属性; 修订建议 9）+ 区内 OpenAudioChannel( = 0（零第二条重连路径; 巡检不直连不新建 task） */
TEST_CASE("会话通道常驻: 自愈巡检短路序 (SCR-A7)",
          "[session_channel_resident][SCR-A7][structural]")
{
    char *region;
    const char *p_inflight, *p_opened;
    int n;

    SCR_REQUIRE_ROOT();

    region = scr_region_between(SCR_APP_CC, "if (bits & MAIN_EVENT_CLOCK_TICK)",
                                "void Application::HandleNetworkConnectedEvent()");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "SCR-A7 RED(infra): CLOCK_TICK 消费区切取失败——锚漂移须复核");

    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "IsAudioChannelOpened("),
        "SCR-A7 RED: CLOCK_TICK 区零 IsAudioChannelOpened(——自愈巡检未判通道开合"
        "（TECH-261012 首按键失同步第一段根因未修）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "TryStartAutoConnect("),
        "SCR-A7 RED: CLOCK_TICK 区零 TryStartAutoConnect(——自愈巡检未接既有重连任务"
        "（§6.4 常驻自愈未接线）");

    /* 守卫短路序：auto_connect_in_flight_ 偏移先于 IsAudioChannelOpened(（降频: in-flight
     * 期短路掉 IsTimeout ESP_LOGE 每秒一条）。 */
    p_inflight = strstr(region, "auto_connect_in_flight_");
    p_opened = strstr(region, "IsAudioChannelOpened(");
    TEST_ASSERT_TRUE_MESSAGE(p_inflight != NULL && p_opened != NULL &&
                                 p_inflight < p_opened,
        "SCR-A7 RED: 巡检守卫短路序破（auto_connect_in_flight_ 须先于 "
        "IsAudioChannelOpened(）——in-flight 期未短路, IsTimeout ESP_LOGE 每秒一条刷屏"
        "（修订建议 9 降频机制; plan T4）");

    /* 零第二条重连路径：巡检区不直连 OpenAudioChannel(（只经 TryStartAutoConnect）。 */
    n = scr_count_in_slice(region, "OpenAudioChannel(");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        scr_fm("SCR-A7 RED: 巡检区出现 OpenAudioChannel( = %d 处——绕过 AutoConnect task "
               "直连（第二条重连路径; 守卫与退避被旁路; plan T4 铁律）", n));

    free(region);
}

/* =========================================================================== *
 * SCR-A8 — 空素材会话空闲收尾 producer（D3；CLOCK_TICK 区）。
 * 真机轮 B 修复轮重锚：素材门换源（planner 对 architect 回执的承重修订）。
 *   旧钉 = 「本次会话从未收到素材」读 `!s_story_rec_entered`（VAD 起音沿置位的
 *   file-static 镜像）。
 *   新钉 = 读缝内 `asr_seen`（引擎 `stt` 真的转写出一句话才算素材——与 front 累积器
 *   同源）。原因：轮 B 设备日志证明故事会话起步 0.25–0.7 秒内必出一条
 *   `story_vad_recording_enter`（无人说话时 VAD 也让出一记 speech 沿），它把
 *   `s_story_rec_entered` 置真 → 旧判据在真机上**结构性不可达**，修复等于白做
 *   （device-evidence/round-b/kplog-excerpt.txt 07:14:30 / 07:15:04）。
 * 同时退役「`!s_vad_speaking >= 2`」计数条：同一事实（电平门不得回潮）已由 SCR-A5
 * 的全 CLOCK_TICK 区负锚（恰 0）单一承载，双钉只会产生 co-red 噪声。
 * 设备结果标记见本文件头注（device-outcome / host-uncompilable-path 成对，范围已收窄
 * 为接线与主循环）；判据语义本体由 SCR-A9/A10/A11/A12 的真行为断言承载。
 * 设计态红: CLOCK_TICK 区仍在场 `!s_story_rec_entered`（旧素材门）、零 `asr_seen`
 *          （新素材门缺席）。
 * =========================================================================== */
/* coverage: SCR-A8 -- 空素材空闲收尾 producer（D3）：命名常量 kStoryEmptySessionTimeoutS 首现片段含 60 + 定义位次在 kSessionIdleTimeoutS 之后 + CLOCK_TICK 区 kidpal_story_session_ctx_active() >=2（一处非否定 = D3 正向记录语境门 + 一处否定 = 普通 60s producer 的豁免门）+ asr_seen >=1（素材门换源：读缝内「引擎已转写出一句」）+ !s_story_rec_entered 恰 0（旧素材门退役，防回潮）+ HandleEndVoiceSessionEvent() >=2 + 审计 token story_empty_session_timeout + 区内 OpenAudioChannel( = 0 + 区内无 clock_ticks_ % 10；交叉负锚 = kSessionIdleTimeoutS 首现仍在原定义行（新常量注释不得写回该字面，否则 SCR-A5 假红） */
TEST_CASE("会话通道常驻: 空素材空闲收尾producer (SCR-A8)",
          "[session_channel_resident][SCR-A8][structural]")
{
    char *clock_region;
    char *def_region;
    char *full;
    const char *p_idle_def, *p_empty_def, *line_start, *line_end;
    int n;

    SCR_REQUIRE_ROOT();

    clock_region = scr_region_between(SCR_APP_CC, "if (bits & MAIN_EVENT_CLOCK_TICK)",
                                      "void Application::HandleNetworkConnectedEvent()");
    TEST_ASSERT_NOT_NULL_MESSAGE(clock_region,
        "SCR-A8 RED(infra): CLOCK_TICK 消费区切取失败——锚漂移须复核");

    /* 命名常量 kStoryEmptySessionTimeoutS：首现到下一分号片段含 60（SCR-A5 同形态）。 */
    def_region = scr_region_between(SCR_APP_CC, "kStoryEmptySessionTimeoutS", ";");
    TEST_ASSERT_NOT_NULL_MESSAGE(def_region,
        "SCR-A8 RED: application.cc 无 kStoryEmptySessionTimeoutS —— 空素材空闲收尾"
        "常量未落（记录语境空素材会话零收尾 producer = 静置不结束的根因现状; "
        "device-round-a-evidence §二; plan D3）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(def_region, "60"),
        "SCR-A8 RED: kStoryEmptySessionTimeoutS 首现片段不含 60——阈值偏离 "
        "requirements §2.1 单一 60s 口径（plan D3 阈值决策 = 60，不引入第二魔数）");
    free(def_region);

    /* 常量定义位次 + 交叉负锚（新常量不得抢 SCR-A5 的 kSessionIdleTimeoutS 首现区）。 */
    full = scr_read(SCR_APP_CC, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(full,
        "SCR-A8 RED(infra): application.cc 读取失败——源锚代理无输入");
    p_idle_def = strstr(full, "kSessionIdleTimeoutS");
    p_empty_def = strstr(full, "kStoryEmptySessionTimeoutS");
    TEST_ASSERT_TRUE_MESSAGE(p_idle_def != NULL && p_empty_def != NULL &&
                                 p_empty_def > p_idle_def,
        "SCR-A8 RED: kStoryEmptySessionTimeoutS 未定义在 kSessionIdleTimeoutS 之后"
        "——常量区位次漂移（plan D3: 定义在既有 60s 常量之后；也防 SCR-A5 的"
        "「kSessionIdleTimeoutS 首现到下一分号」区切被抢）");

    if (p_idle_def != NULL) {
        size_t line_len;
        char *line;

        line_start = p_idle_def;
        while (line_start > full && line_start[-1] != '\n') {
            line_start--;
        }
        line_end = p_idle_def;
        while (*line_end != '\0' && *line_end != '\n') {
            line_end++;
        }
        line_len = (size_t)(line_end - line_start);
        line = (char *)malloc(line_len + 1);
        TEST_ASSERT_NOT_NULL(line); /* logic guard: host OOM must fail loudly */
        memcpy(line, line_start, line_len);
        line[line_len] = '\0';
        TEST_ASSERT_TRUE_MESSAGE(
            scr_contains(line, "static const int kSessionIdleTimeoutS") &&
                scr_contains(line, "60"),
            scr_fm("SCR-A8 RED(交叉负锚): kSessionIdleTimeoutS 首现行 = [%s]（须为原"
                   "定义行 static const int kSessionIdleTimeoutS = 60）——首现点漂到"
                   "非定义行 = 新常量/注释写回了该字面（SCR-A5 的「首现到下一分号」"
                   "区切被抢 → SCR-A5 阈值断言假红）", line));
        free(line);
    }
    free(full);

    /* D3 producer 判据（与普通 60s producer 共存于同区）。 */
    n = scr_count_in_slice(clock_region, "kidpal_story_session_ctx_active()");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        scr_fm("SCR-A8 RED: CLOCK_TICK 区 kidpal_story_session_ctx_active() = %d 处"
               "（冻结 >=2）——空素材收尾缺「记录语境会话」正向门（普通 60s producer "
               "的豁免门一处 + D3 正向门一处; 非记录语境不得命中）", n));

    n = scr_count_in_slice(clock_region, "!kidpal_story_session_ctx_active()");
    TEST_ASSERT_TRUE_MESSAGE(n == 1,
        scr_fm("SCR-A8 RED: CLOCK_TICK 区 !kidpal_story_session_ctx_active() = %d 处"
               "（冻结恰 1）——普通 60s 无语音 producer 的记录语境豁免须唯一；D3 "
               "producer 用正向门，复制成否定门 = 空素材收尾永不命中", n));

    /* 素材门换源（新钉，承重）：读缝内 asr_seen（引擎 stt 真的转写过 = 与 front
     * 累积器同源）。 */
    n = scr_count_in_slice(clock_region, "asr_seen");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        scr_fm("SCR-A8 RED: CLOCK_TICK 区 asr_seen = %d 处（冻结 >=1）——D3 素材门未换"
               "源到缝（旧 `!s_story_rec_entered` 会被起步噪声 VAD 沿提前置真 → 空素材"
               "会话在真机上结构性不可达；轮 B 设备日志 12 条 story_vad_recording_enter "
               "全落会话起步 1 秒内）", n));

    /* 旧素材门退役（负锚，防回潮）：CLOCK_TICK 区出现即旧判据复活 = D3 再次不可达。 */
    n = scr_count_in_slice(clock_region, "!s_story_rec_entered");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        scr_fm("SCR-A8 RED: CLOCK_TICK 区 !s_story_rec_entered = %d 处（冻结 0）——"
               "旧素材门回潮（该信号被起步噪声沿置真，D3 在真机上结构性不可达；"
               "s_story_rec_entered 保留其录音页切换/幂等原用途，不进 producer 判据）",
               n));

    n = scr_count_in_slice(clock_region, "HandleEndVoiceSessionEvent()");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        scr_fm("SCR-A8 RED: CLOCK_TICK 区 HandleEndVoiceSessionEvent() = %d 处（冻结 "
               ">=2）——空素材收尾未接统一收口处理器（只打日志不收尾 = 设备仍 Listening）",
               n));

    TEST_ASSERT_TRUE_MESSAGE(scr_contains(clock_region, "story_empty_session_timeout"),
        "SCR-A8 RED: CLOCK_TICK 区缺审计 token story_empty_session_timeout——空素材"
        "收尾不可判别（轮 B B-D3 三源对账的设备侧唯一可观测证据）");

    n = scr_count_in_slice(clock_region, "OpenAudioChannel(");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        scr_fm("SCR-A8 RED: CLOCK_TICK 区出现 OpenAudioChannel( = %d 处——空素材收尾"
               "不得直连建连（零第二条重连路径; SCR-A7 同族铁律）", n));

    n = scr_count_in_slice(clock_region, "clock_ticks_ % 10");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        scr_fm("SCR-A8 RED: CLOCK_TICK 区出现 clock_ticks_ %% 10 = %d 处——diag_log "
               "DLG-A4⑤ 负锚被破（分频不得复用 clock_ticks_，与 60s 判据互相干扰）", n));

    free(clock_region);
}

/* =========================================================================== *
 * SCR-A9 — 纯静默 60 秒，恰在第 60 tick 收尾（真行为半；[S8] 语义 (1)(4)）。
 * 设计态红: 缝未落地（weak 哨兵 -2）-> 首 tick 即不等于 KVW_ACT_NONE。
 * 判别力注记: 若实现漏了 `asr_seen` 门（asr_fresh 只看 asr_age_s < 25），本 case
 * 会红在 tick 85 而不是 60——这正是该门必须被断言抓到的一类错误。
 * =========================================================================== */
/* coverage: SCR-A9 -- 纯静默收尾（requirements §2.1 结束通道①）：vad_level=0 且零 stt，tick 1..59 返回 KVW_ACT_NONE、tick 60 恰返回 KVW_ACT_IDLE_FIRE 且该 tick quiet_s == 60；60 之后未 reset 前持续返回 KVW_ACT_IDLE_FIRE（sticky，不冒新语义）；末端 level_run_s=0 / asr_seen=0 / suspect=0 */
TEST_CASE("会话通道常驻: 静默60秒恰tick60收尾 (SCR-A9)",
          "[session_channel_resident][SCR-A9][behavior]")
{
    kidpal_voice_watch_cfg_t cfg;
    kidpal_voice_watch_t w;
    int t, ret, first_fire = -1;

    scr_vw_default_cfg(&cfg);
    scr_vw_fresh_watch(&w);

    for (t = 1; t <= 120; t++) {
        ret = kidpal_voice_watch_tick(&w, 0, &cfg);
        if (ret == KVW_ACT_IDLE_FIRE && first_fire < 0) {
            first_fire = t;
        }
        if (t < 60) {
            TEST_ASSERT_TRUE_MESSAGE(ret == KVW_ACT_NONE,
                scr_fm("SCR-A9 RED: 静默第 %d tick 返回 %d（期望 KVW_ACT_NONE=%d）——"
                       "空闲判据提前触发（阈值或累加语义漂移）",
                       t, ret, KVW_ACT_NONE));
        } else if (t == 60) {
            TEST_ASSERT_TRUE_MESSAGE(ret == KVW_ACT_IDLE_FIRE,
                scr_fm("SCR-A9 RED: 静默第 60 tick 返回 %d（期望 KVW_ACT_IDLE_FIRE=%d）"
                       "——静默收尾未在 60 秒命中（B-D3 / A6 验收口径）",
                       ret, KVW_ACT_IDLE_FIRE));
            TEST_ASSERT_TRUE_MESSAGE(w.quiet_s == 60,
                scr_fm("SCR-A9 RED: 第 60 tick 的 quiet_s = %d（期望 60）——收尾当刻"
                       "证据秒数不符（quiet_s 语义漂移；审计行 quiet= 字段随之失真）",
                       w.quiet_s));
        } else {
            TEST_ASSERT_TRUE_MESSAGE(ret == KVW_ACT_IDLE_FIRE,
                scr_fm("SCR-A9 RED: 第 %d tick（60 之后未 reset）返回 %d（期望持续 "
                       "KVW_ACT_IDLE_FIRE=%d）——收尾动作非 sticky（设备会漏收口）",
                       t, ret, KVW_ACT_IDLE_FIRE));
        }
    }

    TEST_ASSERT_TRUE_MESSAGE(first_fire == 60,
        scr_fm("SCR-A9 RED: 首次 fire 于第 %d tick（期望恰 60）——静默收尾时点漂移"
               "（requirements §2.1 单一 60s 口径）", first_fire));
    TEST_ASSERT_TRUE_MESSAGE(w.tick_s == 120 && w.level_run_s == 0 &&
                                 w.asr_seen == 0 && w.suspect == 0,
        scr_fm("SCR-A9 RED: 末端状态 tick_s=%d level_run_s=%d asr_seen=%d suspect=%d"
               "（期望 120/0/0/0）——静默路径下电平/素材/审计位被误置",
               w.tick_s, w.level_run_s, w.asr_seen, w.suspect));
}

/* =========================================================================== *
 * SCR-A10 — 电平卡死（M1）有界收尾：恒 vad=1 400 tick、零 stt -> 恰第 160 秒收尾。
 * kill-shot：这是轮 B 缺陷（电平卡死时永不收尾）的唯一离线警报——删掉可信窗上界或
 * 把 VAD_TRUST_S 调错，本 case 与 SCR-A12 必有一红。
 * =========================================================================== */
/* coverage: SCR-A10 -- M1 电平卡死有界收尾：vad_level=1 恒 400 tick 且零 stt -> tick 1..159 零 fire（第 100 tick quiet_s 仍 0 = 可信窗内采信）、tick 101 quiet_s == 1（可信窗到期）、tick 160 恰 fire 且 quiet_s == 60 / level_run_s == 160 / suspect == 1、之后未 reset 前持续 fire；首次 fire 恰在 160（suspect 不是独立触发源） */
TEST_CASE("会话通道常驻: 电平卡死有界收尾 (SCR-A10)",
          "[session_channel_resident][SCR-A10][behavior]")
{
    kidpal_voice_watch_cfg_t cfg;
    kidpal_voice_watch_t w;
    int t, ret, first_fire = -1;

    scr_vw_default_cfg(&cfg);
    scr_vw_fresh_watch(&w);

    for (t = 1; t <= 400; t++) {
        ret = kidpal_voice_watch_tick(&w, 1, &cfg);
        if (ret == KVW_ACT_IDLE_FIRE && first_fire < 0) {
            first_fire = t;
        }
        if (t == 100) {
            TEST_ASSERT_TRUE_MESSAGE(ret == KVW_ACT_NONE && w.quiet_s == 0,
                scr_fm("SCR-A10 RED: 第 100 tick（可信窗末）ret=%d quiet_s=%d（期望 "
                       "KVW_ACT_NONE/0）——可信窗内电平证据未被采信（A7 连续讲保护破）",
                       ret, w.quiet_s));
        } else if (t == 101) {
            TEST_ASSERT_TRUE_MESSAGE(w.quiet_s == 1,
                scr_fm("SCR-A10 RED: 第 101 tick quiet_s = %d（期望 1）——可信窗上界"
                       "失效（VAD_TRUST_S 未截断电平证据），电平卡死永不计静默",
                       w.quiet_s));
        } else if (t < 160) {
            TEST_ASSERT_TRUE_MESSAGE(ret == KVW_ACT_NONE,
                scr_fm("SCR-A10 RED: 电平卡死第 %d tick 返回 %d（期望 KVW_ACT_NONE=%d）"
                       "——有界收尾提前触发", t, ret, KVW_ACT_NONE));
        } else if (t == 160) {
            TEST_ASSERT_TRUE_MESSAGE(ret == KVW_ACT_IDLE_FIRE && w.quiet_s == 60,
                scr_fm("SCR-A10 RED: 第 160 tick ret=%d quiet_s=%d（期望 FIRE/60）——"
                       "电平卡死未在 100+60 秒有界收尾", ret, w.quiet_s));
            TEST_ASSERT_TRUE_MESSAGE(w.suspect == 1 && w.level_run_s == 160,
                scr_fm("SCR-A10 RED: 第 160 tick suspect=%d level_run_s=%d（期望 1/160）"
                       "——卡死审计位未置（轮 C 无从判定是否命中 M1）",
                       w.suspect, w.level_run_s));
        } else {
            TEST_ASSERT_TRUE_MESSAGE(ret == KVW_ACT_IDLE_FIRE && w.suspect == 1,
                scr_fm("SCR-A10 RED: 第 %d tick（160 之后未 reset）ret=%d suspect=%d"
                       "——收尾动作非 sticky 或审计位丢失", t, ret, w.suspect));
        }
    }

    TEST_ASSERT_TRUE_MESSAGE(first_fire == 160,
        scr_fm("SCR-A10 RED: 首次 fire 于第 %d tick（期望恰 160）——suspect 不得成为"
               "独立触发源（有界性的唯一来源 = 单一 quiet_s 累加器 + 可信窗截断）",
               first_fire));
}

/* =========================================================================== *
 * SCR-A11 — 起音沿风暴（M2）不拖收尾 vs 4 秒暴发不误收（成对真行为）。
 * =========================================================================== */
/* coverage: SCR-A11 -- M2 起音沿风暴与短暴发的对称判据：(a) 200 tick 重复 [1,0,0,0,0]、零 stt -> 首次 fire 恰在 tick 60（单秒尖峰 < VOICE_SUSTAIN_S，不构成语音证据，静默计时不被清零）；(b) 200 tick 重复 [1,1,1,1,0]、零 stt -> 200 tick 内零 fire 且全程 quiet_s <= 3（4 秒连续段越过 sustain 下界 = 真语音证据，计时持续归零） */
TEST_CASE("会话通道常驻: 起音沿风暴vs短暴发 (SCR-A11)",
          "[session_channel_resident][SCR-A11][behavior]")
{
    static const int storm[5] = {1, 0, 0, 0, 0};
    static const int burst[5] = {1, 1, 1, 1, 0};
    kidpal_voice_watch_cfg_t cfg;
    kidpal_voice_watch_t w;
    int t, ret, first_fire, fires, max_quiet;

    scr_vw_default_cfg(&cfg);

    /* (a) 风暴：单秒尖峰 < VOICE_SUSTAIN_S(3) 且零 stt -> 不构成证据。 */
    scr_vw_fresh_watch(&w);
    first_fire = -1;
    for (t = 1; t <= 200; t++) {
        ret = kidpal_voice_watch_tick(&w, storm[(t - 1) % 5], &cfg);
        if (ret == KVW_ACT_IDLE_FIRE && first_fire < 0) {
            first_fire = t;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(first_fire == 60,
        scr_fm("SCR-A11 RED: 风暴序列首次 fire 于第 %d tick（期望恰 60）——单秒尖峰被"
               "采信为语音证据（sustain 下界失效 -> M2 起音沿风暴把收尾无限推迟，"
               "轮 B A6 实测 3m13s 不触发）", first_fire));

    /* (b) 4 秒暴发：连续段 >= 3 秒 = 真语音证据 -> 每个 5 tick 周期内 quiet_s 归零。 */
    scr_vw_fresh_watch(&w);
    fires = 0;
    max_quiet = -1;
    for (t = 1; t <= 200; t++) {
        ret = kidpal_voice_watch_tick(&w, burst[(t - 1) % 5], &cfg);
        if (ret == KVW_ACT_IDLE_FIRE) {
            fires++;
        }
        if (w.quiet_s > max_quiet) {
            max_quiet = w.quiet_s;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(fires == 0,
        scr_fm("SCR-A11 RED: 4 秒暴发序列 200 tick 内 fire %d 次（期望 0）——真语音"
               "证据被丢弃（sustain 下界过高 = 把孩子的话当噪声，正在讲话被掐断）",
               fires));
    TEST_ASSERT_TRUE_MESSAGE(max_quiet <= 3,
        scr_fm("SCR-A11 RED: 4 秒暴发序列 quiet_s 峰值 = %d（期望 <= 3）——连续段"
               "未被采信（static 下界语义漂移）", max_quiet));
}

/* =========================================================================== *
 * SCR-A12 — A7 连续讲话零误收（成对真行为；planner 明令不可降级为源文本）。
 * =========================================================================== */
/* coverage: SCR-A12 -- A7 连续讲话保护（成对）：(a) vad_level=1 恒 200 tick + 每 10 tick 一次 note_asr -> 200 tick 零 fire 且 asr_seen == 1（asr_fresh 把保护延伸到 >160s 真长讲；可信窗截断后仍有引擎证据）；(b) vad_level=1 恒 150 tick + 零 stt -> tick 1..150 零 fire（有界收尾须 100+60，A7 的 90s 界守住） */
TEST_CASE("会话通道常驻: A7连续讲话零误收 (SCR-A12)",
          "[session_channel_resident][SCR-A12][behavior]")
{
    kidpal_voice_watch_cfg_t cfg;
    kidpal_voice_watch_t w;
    int t, ret, fires;

    scr_vw_default_cfg(&cfg);

    /* (a) 连续电平 + 周期 stt：证据链永不断。 */
    scr_vw_fresh_watch(&w);
    fires = 0;
    for (t = 1; t <= 200; t++) {
        if (t % 10 == 0) {
            kidpal_voice_watch_note_asr(&w);
        }
        ret = kidpal_voice_watch_tick(&w, 1, &cfg);
        if (ret == KVW_ACT_IDLE_FIRE) {
            fires++;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(fires == 0,
        scr_fm("SCR-A12 RED: 连续讲话 + 周期 stt 200 tick 内 fire %d 次（期望 0）——"
               "A7 连续讲 >90s 被误收（asr_fresh 未把保护延伸过可信窗上界）", fires));
    TEST_ASSERT_TRUE_MESSAGE(w.asr_seen == 1,
        scr_fm("SCR-A12 RED: note_asr 后 asr_seen = %d（期望 1）——素材信号未落"
               "（D3 素材门与 A7 证据链共用该标志）", w.asr_seen));

    /* (b) 连续电平 + 零 stt：150 tick 内绝不收尾（A7 90 秒界）。 */
    scr_vw_fresh_watch(&w);
    fires = 0;
    for (t = 1; t <= 150; t++) {
        ret = kidpal_voice_watch_tick(&w, 1, &cfg);
        if (ret == KVW_ACT_IDLE_FIRE) {
            fires++;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(fires == 0,
        scr_fm("SCR-A12 RED: 连续电平 + 零 stt 150 tick 内 fire %d 次（期望 0）——"
               "A7「连续讲 >90s 不误收」失守（收尾窗口压得比 90s 短）", fires));
}

/* =========================================================================== *
 * SCR-A13 — 跨会话零残留（reset 全字段归零）+ note_asr 契约（真行为）。
 * =========================================================================== */
/* coverage: SCR-A13 -- 跨会话零残留与 note_asr 契约：cfg_default 三字段 = 3/25/100 且 idle_timeout_s 由调用方注入保持 60；跑 50 tick（vad=1）后 level_run_s=50/tick_s=50/quiet_s=0；note_asr -> asr_seen == 1 且 asr_age_s == 0；再 5 静默 tick -> asr_age_s == 5；reset 后七字段全零（quiet_s/level_run_s/asr_age_s/tick_s/diag_last_s/suspect/asr_seen）；reset 后首 tick quiet_s == 1 / level_run_s == 0 / tick_s == 1 / asr_age_s == 1；会话内软复位（reset_soft，2026-11-18 tighten）：asr_seen=1 且 quiet_s/level_run_s/asr_age_s/tick_s/diag_last_s/suspect 六件脏置 -> reset_soft 后 **asr_seen == 1 且其余六件全 0**；reset_soft 之后再 reset() -> 七字段仍全 0（硬复位不得被软复位改弱） */
TEST_CASE("会话通道常驻: reset清零与note_asr契约 (SCR-A13)",
          "[session_channel_resident][SCR-A13][behavior]")
{
    kidpal_voice_watch_cfg_t cfg;
    kidpal_voice_watch_t w;
    int i;

    scr_vw_default_cfg(&cfg);
    TEST_ASSERT_TRUE_MESSAGE(cfg.voice_sustain_s == 3 && cfg.asr_fresh_s == 25 &&
                                 cfg.vad_trust_s == 100,
        scr_fm("SCR-A13 RED: cfg_default 填 voice_sustain_s=%d asr_fresh_s=%d "
               "vad_trust_s=%d（期望 3/25/100）——缝默认阈值未落（头宏到 cfg 的映射断）",
               cfg.voice_sustain_s, cfg.asr_fresh_s, cfg.vad_trust_s));
    TEST_ASSERT_TRUE_MESSAGE(cfg.idle_timeout_s == SCR_VW_IDLE_TIMEOUT_S,
        scr_fm("SCR-A13 RED: cfg.idle_timeout_s = %d（期望调用方注入的 %d）——"
               "cfg_default 覆盖了调用方注入值（单源注入点失效）",
               cfg.idle_timeout_s, SCR_VW_IDLE_TIMEOUT_S));

    scr_vw_fresh_watch(&w);
    for (i = 0; i < 50; i++) {
        (void)kidpal_voice_watch_tick(&w, 1, &cfg);
    }
    TEST_ASSERT_TRUE_MESSAGE(w.level_run_s == 50 && w.tick_s == 50 && w.quiet_s == 0,
        scr_fm("SCR-A13 RED: 50 tick 后 level_run_s=%d tick_s=%d quiet_s=%d（期望 "
               "50/50/0）——连续段/计时/静默累加语义漂移",
               w.level_run_s, w.tick_s, w.quiet_s));

    kidpal_voice_watch_note_asr(&w);
    TEST_ASSERT_TRUE_MESSAGE(w.asr_seen == 1 && w.asr_age_s == 0,
        scr_fm("SCR-A13 RED: note_asr 后 asr_seen=%d asr_age_s=%d（期望 1/0）——"
               "素材标志或年龄未置（D3 素材门与 asr_fresh 的共同输入）",
               w.asr_seen, w.asr_age_s));

    for (i = 0; i < 5; i++) {
        (void)kidpal_voice_watch_tick(&w, 0, &cfg);
    }
    TEST_ASSERT_TRUE_MESSAGE(w.asr_age_s == 5,
        scr_fm("SCR-A13 RED: 5 静默 tick 后 asr_age_s = %d（期望 5）——年龄累加语义"
               "漂移", w.asr_age_s));

    /* poison 诊断字段：reset 必须真的清（漏清 = 跨会话残留 = M3/M4 同族）。 */
    w.diag_last_s = 777;
    w.tick_s = 4242;
    w.suspect = 1;
    kidpal_voice_watch_reset(&w);
    TEST_ASSERT_TRUE_MESSAGE(w.quiet_s == 0 && w.level_run_s == 0 && w.asr_age_s == 0 &&
                                 w.tick_s == 0 && w.diag_last_s == 0 &&
                                 w.suspect == 0 && w.asr_seen == 0,
        scr_fm("SCR-A13 RED: reset 后 quiet_s=%d level_run_s=%d asr_age_s=%d tick_s=%d "
               "diag_last_s=%d suspect=%d asr_seen=%d（期望全 0）——跨会话残留"
               "（会话入口 reset 不净 = 新会话继承上一会话证据/审计位）",
               w.quiet_s, w.level_run_s, w.asr_age_s, w.tick_s, w.diag_last_s,
               w.suspect, w.asr_seen));

    (void)kidpal_voice_watch_tick(&w, 0, &cfg);
    TEST_ASSERT_TRUE_MESSAGE(w.quiet_s == 1 && w.level_run_s == 0 && w.tick_s == 1 &&
                                 w.asr_age_s == 1,
        scr_fm("SCR-A13 RED: reset 后首 tick quiet_s=%d level_run_s=%d tick_s=%d "
               "asr_age_s=%d（期望 1/0/1/1）——reset 基准态漂移",
               w.quiet_s, w.level_run_s, w.tick_s, w.asr_age_s));

    /* ---- 会话内软复位（reset_soft）：六清一保（2026-11-18 tighten）----
     * 场景 = 会话中的引导播报把设备带走一次「聆听→播报→聆听」往返。软复位必须清掉
     * 计时/诊断六件（播报期间累积的静默秒数不得算进孩子的回答间隙），但**必须保留
     * asr_seen**——它同时是 D3 空素材 producer 的素材门与 asr_fresh 的门；清掉就等于
     * 宣布「本会话没有素材」，已有素材的故事会话会在播报后静置 60 秒被不归档地释放。 */
    w.asr_seen = 1;      /* 本会话素材证据：引擎真的转写出过一句 */
    w.quiet_s = 41;      /* 往返前累积的静默秒数 */
    w.level_run_s = 7;
    w.asr_age_s = 13;
    w.tick_s = 88;
    w.diag_last_s = 77;
    w.suspect = 1;
    kidpal_voice_watch_reset_soft(&w);
    TEST_ASSERT_TRUE_MESSAGE(w.asr_seen == 1,
        scr_fm("SCR-A13 RED: reset_soft 后 asr_seen = %d（期望 1）——会话内软复位把"
               "素材证据清了（Listening->Speaking->Listening 往返后 D3 空素材 producer"
               "会把已有素材的会话判成空素材、不归档地释放，孩子已讲的内容丢失）",
               w.asr_seen));
    TEST_ASSERT_TRUE_MESSAGE(w.quiet_s == 0 && w.level_run_s == 0 && w.asr_age_s == 0 &&
                                 w.tick_s == 0 && w.diag_last_s == 0 &&
                                 w.suspect == 0,
        scr_fm("SCR-A13 RED: reset_soft 后 quiet_s=%d level_run_s=%d asr_age_s=%d "
               "tick_s=%d diag_last_s=%d suspect=%d（期望六件全 0）——软复位只该保留"
               "asr_seen；播报期间的静默/电平/审计位不得跨往返继承",
               w.quiet_s, w.level_run_s, w.asr_age_s, w.tick_s, w.diag_last_s,
               w.suspect));

    /* 硬复位仍全清：软复位不得削弱跨会话语义（软复位之后走 reset()，七字段含
     * asr_seen 一律归零）。 */
    kidpal_voice_watch_reset(&w);
    TEST_ASSERT_TRUE_MESSAGE(w.asr_seen == 0 && w.quiet_s == 0 && w.level_run_s == 0 &&
                                 w.asr_age_s == 0 && w.tick_s == 0 &&
                                 w.diag_last_s == 0 && w.suspect == 0,
        scr_fm("SCR-A13 RED: reset_soft 之后 reset 的七字段 = asr_seen %d / quiet_s %d / "
               "level_run_s %d / asr_age_s %d / tick_s %d / diag_last_s %d / suspect %d"
               "（期望全 0）——跨会话硬复位被软复位改弱（新会话继承上一会话的素材证据）",
               w.asr_seen, w.quiet_s, w.level_run_s, w.asr_age_s, w.tick_s,
               w.diag_last_s, w.suspect));
}

/* =========================================================================== *
 * SCR-A14 — 诊断行限频决策表（真行为）+ listening_diag 结构半。
 * 决策表本体 = 缝纯函数 diag_due（模数用缝自持 tick_s）；应用层物理上不出现
 * `clock_ticks_ % 10`（与 DLG-A4⑤ 负锚双保险）。
 * =========================================================================== */
/* coverage: SCR-A14 -- 诊断限频（human 必办可观测性）：diag_due 决策表四支（周期 10s 恰每 10 秒一次 / changed=1 且间隔 >= min_gap=3 提前放行 / changed 每 tick 抖动被 min_gap 封顶到每 3 秒一次 / 非法入参与时钟回退 fail-closed）；结构半：CLOCK_TICK 区 token listening_diag >=1 + quiet=/vad=/rec_ctx=/asr=/suspect=/run= 六字段各 >=1 + clock_ticks_ % 10 恰 0 */
TEST_CASE("会话通道常驻: 诊断限频决策表+结构 (SCR-A14)",
          "[session_channel_resident][SCR-A14][behavior][structural]")
{
    static const char *const fields[] = {
        "quiet=", "vad=", "rec_ctx=", "asr=", "suspect=", "run="
    };
    char *region;
    int last, fires, now, due, first_fire_at;
    size_t i;

    /* (1) 决策表定值点。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_watch_diag_due(9, 0, 10, 3, 0) == 0 &&
                                 kidpal_voice_watch_diag_due(10, 0, 10, 3, 0) == 1,
        "SCR-A14 RED: diag_due 周期支漂移（9 秒不放行 / 10 秒放行）——诊断行频率"
        "偏离冻结的每 10 秒一次");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_watch_diag_due(2, 0, 10, 3, 1) == 0 &&
                                 kidpal_voice_watch_diag_due(3, 0, 10, 3, 1) == 1,
        "SCR-A14 RED: diag_due 跳变提前支漂移（changed=1 时须 >= min_gap 才放行）——"
        "短于 3 秒的抖动被放行 = 日志刷屏");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_watch_diag_due(2, 0, 10, 3, 0) == 0,
        "SCR-A14 RED: diag_due 无跳变且未满间隔时放行——限频失效");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_watch_diag_due(5, 10, 10, 3, 1) == 0,
        "SCR-A14 RED: diag_due 时钟回退（now < last）未 fail-closed——回拨后可能"
        "恒放行或恒抑制，诊断行频率不可预测");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_watch_diag_due(1, 0, 0, 3, 0) == -1 &&
                                 kidpal_voice_watch_diag_due(1, 0, -5, 3, 0) == -1,
        "SCR-A14 RED: diag_due 非法 interval_s 未返回 -1（防御缺失）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_watch_diag_due(1, 0, 10, 0, 0) == -1 &&
                                 kidpal_voice_watch_diag_due(1, 0, 10, -3, 0) == -1,
        "SCR-A14 RED: diag_due 非法 min_gap_s 未返回 -1（防御缺失）");

    /* (2) 周期支：100 秒恰 10 次，首次在 10。 */
    last = 0;
    fires = 0;
    first_fire_at = -1;
    for (now = 1; now <= 100; now++) {
        due = kidpal_voice_watch_diag_due(now, last, KIDPAL_VW_DIAG_INTERVAL_S,
                                          KIDPAL_VW_DIAG_MIN_GAP_S, 0);
        if (due == 1) {
            fires++;
            if (first_fire_at < 0) {
                first_fire_at = now;
            }
            last = now;
        } else {
            TEST_ASSERT_TRUE_MESSAGE(due == 0,
                scr_fm("SCR-A14 RED: 周期支第 %d 秒返回 %d（期望 1 或 0）——决策表"
                       "返回域漂移", now, due));
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(fires == 10 && first_fire_at == 10,
        scr_fm("SCR-A14 RED: 100 秒周期内放行 %d 次（首次第 %d 秒；期望 10 次 / 10）"
               "——诊断行频率漂移（轮转预算随之漂移）", fires, first_fire_at));

    /* (3) 跳变抖动支：changed 每 tick 为真 -> 被 min_gap=3 封顶（30 秒恰 10 次）。 */
    last = 0;
    fires = 0;
    for (now = 1; now <= 30; now++) {
        due = kidpal_voice_watch_diag_due(now, last, KIDPAL_VW_DIAG_INTERVAL_S,
                                          KIDPAL_VW_DIAG_MIN_GAP_S, 1);
        if (due == 1) {
            fires++;
            TEST_ASSERT_TRUE_MESSAGE(now - last >= KIDPAL_VW_DIAG_MIN_GAP_S,
                scr_fm("SCR-A14 RED: 跳变放行间隔 %d 秒 < min_gap %d——抖动封顶失效",
                       now - last, KIDPAL_VW_DIAG_MIN_GAP_S));
            last = now;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(fires == 10,
        scr_fm("SCR-A14 RED: 30 秒全跳变放行 %d 次（期望 10，即每 3 秒一次）——"
               "min_gap 封顶语义漂移", fires));

    /* (4) 结构半：诊断行落 CLOCK_TICK 区，字段齐全，且不复用 clock_ticks_ 分频。 */
    SCR_REQUIRE_ROOT();

    region = scr_region_between(SCR_APP_CC, "if (bits & MAIN_EVENT_CLOCK_TICK)",
                                "void Application::HandleNetworkConnectedEvent()");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "SCR-A14 RED(infra): CLOCK_TICK 消费区切取失败——锚漂移须复核");

    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "listening_diag"),
        "SCR-A14 RED: CLOCK_TICK 区缺 token listening_diag——设备侧低频聆听诊断行"
        "未落（轮 B 的 M1/M2 不可判别正是缺这一行；human 必办）");
    for (i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
        TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, fields[i]),
            scr_fm("SCR-A14 RED: listening_diag 缺字段 %s——诊断行信息量不足"
                   "（state/vad/run/quiet/asr/ticks/rec_ctx/suspect 至少六件）",
                   fields[i]));
    }
    TEST_ASSERT_TRUE_MESSAGE(
        scr_count_in_slice(region, "clock_ticks_ % 10") == 0,
        scr_fm("SCR-A14 RED: CLOCK_TICK 区出现 clock_ticks_ %% 10 = %d 处（冻结 0）"
               "——分频复用 clock_ticks_（其有两个归零点，正是 M2 成因本体；分频须走"
               "缝自持 tick_s；DLG-A4⑤ 负锚双保险）",
               scr_count_in_slice(region, "clock_ticks_ % 10")));

    free(region);
}

/* =========================================================================== *
 * SCR-A15 — 应用层接线六件（结构，源锚代理：application.cc 不进 host 构建）。
 * ④ / ⑥ 与 SCR-A5 / SCR-A8 同事实双保险（planner 明示要求，非冗余设计失误）：
 * 接线 case 单独可读，且两条负锚在接线面再次出现以防「只改一个 case 的锚就过」。
 * =========================================================================== */
/* coverage: SCR-A15 -- 应用层接线六件（[S8]）：① CLOCK_TICK 区 kidpal_voice_watch_tick( 恰 1；② "stt".."llm" 区 kidpal_voice_watch_note_asr( 恰 1；③ HandleStateChangedEvent 的 Listening case 内 kidpal_voice_watch_reset( >= 1 + kidpal_voice_watch_reset_soft(&s_voice_watch) >= 1（会话内软复位）+ kidpal_voice_watch_reset(&s_voice_watch) >= 1（跨会话全清路径仍在）+ kDeviceStateSpeaking >= 1（复位按迁移条件分派，非无条件软复位；带实参 needle 防 reset_soft 前缀混计）；④ CLOCK_TICK 区 !s_vad_speaking 恰 0；⑤ Initialize 区 kSessionIdleTimeoutS + kidpal_voice_watch_cfg_default( + idle_timeout_s 三件在世（单源，防两个 60 漂移）；⑥ CLOCK_TICK 区 !s_story_rec_entered 恰 0（D3 素材门读缝 asr_seen）；附：seam 头在世 + 六宏字面（3/25/100/10/3/999）+ seam 源进设备构建 */
TEST_CASE("会话通道常驻: 语音看门缝接线六件 (SCR-A15)",
          "[session_channel_resident][SCR-A15][structural]")
{
    static const struct {
        const char *define_needle;
        long value;
        const char *label;
    } macros[] = {
        { "#define KIDPAL_VW_VOICE_SUSTAIN_S", KIDPAL_VW_VOICE_SUSTAIN_S, "VOICE_SUSTAIN_S" },
        { "#define KIDPAL_VW_ASR_FRESH_S",     KIDPAL_VW_ASR_FRESH_S,     "ASR_FRESH_S" },
        { "#define KIDPAL_VW_VAD_TRUST_S",     KIDPAL_VW_VAD_TRUST_S,     "VAD_TRUST_S" },
        { "#define KIDPAL_VW_DIAG_INTERVAL_S", KIDPAL_VW_DIAG_INTERVAL_S, "DIAG_INTERVAL_S" },
        { "#define KIDPAL_VW_DIAG_MIN_GAP_S",  KIDPAL_VW_DIAG_MIN_GAP_S,  "DIAG_MIN_GAP_S" },
        { "#define KIDPAL_VW_ASR_AGE_SAT",      KIDPAL_VW_ASR_AGE_SAT,     "ASR_AGE_SAT" },
    };
    char *region, *cfg_region, *hdr;
    int n;
    size_t i;

    SCR_REQUIRE_ROOT();

    /* ① / ④ / ⑥：CLOCK_TICK 区接线与两条负锚。 */
    region = scr_region_between(SCR_APP_CC, "if (bits & MAIN_EVENT_CLOCK_TICK)",
                                "void Application::HandleNetworkConnectedEvent()");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "SCR-A15 RED(infra): CLOCK_TICK 消费区切取失败——锚漂移须复核");

    n = scr_count_in_slice(region, "kidpal_voice_watch_tick(");
    TEST_ASSERT_TRUE_MESSAGE(n == 1,
        scr_fm("SCR-A15 RED: CLOCK_TICK 区 kidpal_voice_watch_tick( = %d 处（冻结恰 1）"
               "——0 = 缝未被主循环驱动；>1 = 同一秒推进两次（计时偏快，60s 提早）", n));

    n = scr_count_in_slice(region, "!s_vad_speaking");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        scr_fm("SCR-A15 RED: CLOCK_TICK 区 !s_vad_speaking = %d 处（冻结 0）——瞬时"
               "电平门回潮（与 SCR-A5 同事实，接线面双保险）", n));

    n = scr_count_in_slice(region, "!s_story_rec_entered");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        scr_fm("SCR-A15 RED: CLOCK_TICK 区 !s_story_rec_entered = %d 处（冻结 0）——"
               "D3 素材门未换源到 asr_seen（旧信号被起步噪声 VAD 沿置真；与 SCR-A8 "
               "同事实，接线面双保险）", n));
    free(region);

    /* ② stt 分支：引擎转写到达即 note_asr（与 front 累积器同源的素材证据）。 */
    region = scr_region_between(SCR_APP_CC, "\"stt\"", "\"llm\"");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "SCR-A15 RED(infra): \"stt\"..\"llm\" 区切取失败——协议分支锚漂移须复核");
    n = scr_count_in_slice(region, "kidpal_voice_watch_note_asr(");
    TEST_ASSERT_TRUE_MESSAGE(n == 1,
        scr_fm("SCR-A15 RED: stt 分支 kidpal_voice_watch_note_asr( = %d 处（冻结恰 1）"
               "——0 = 「本会话是否真的转写出素材」永不置位（D3 恒判空素材、A7 保护"
               "失去引擎证据）；>1 = 重复置位（幂等浪费）", n));
    free(region);

    /* ③ Listening case：会话入口复位（M3 电平闩锁 + 跨会话残留的卫生复位）。 */
    region = scr_region_between(SCR_APP_CC, "case kDeviceStateListening:",
                                "case kDeviceStateSpeaking:");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "SCR-A15 RED(infra): case kDeviceStateListening 区切取失败——锚漂移须复核");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "kidpal_voice_watch_reset("),
        "SCR-A15 RED: Listening case 内零 kidpal_voice_watch_reset(——会话入口不复位"
        "看门缝（M3 电平闩锁跨会话残留 ≤160s；quiet_s/asr_seen 继承上一会话）");

    /* ③追加（2026-11-18 tighten）：复位按迁移条件分派——「聆听→播报→聆听」往返走
     * 软复位（保 asr_seen = 保本会话素材证据），其余入口仍走跨会话全清复位。三条字面
     * 同时在场才说明是条件分派：只出现软复位 = 跨会话残留回归；只出现全清复位 =
     * 引导播报后素材证据丢失（D3 把有素材的会话判成空素材、不归档地释放）。
     * needle 一律用带实参形态：裸 `kidpal_voice_watch_reset(` 是
     * `kidpal_voice_watch_reset_soft(` 的前缀子串，两者会被混计；而
     * `kidpal_voice_watch_reset(&s_voice_watch)` 不匹配软复位调用（reset 之后是
     * `_soft`，不是 `(`），两条计数互不污染。 */
    n = scr_count_in_slice(region, "kidpal_voice_watch_reset_soft(&s_voice_watch)");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        scr_fm("SCR-A15 RED: Listening case 内 "
               "kidpal_voice_watch_reset_soft(&s_voice_watch) = %d 处（冻结 >=1）——"
               "软复位接线缺席（引导播报往返后仍走全清复位：asr_seen 被清 0，D3 把"
               "已有素材的故事会话判成空素材、不归档地释放）", n));

    n = scr_count_in_slice(region, "kidpal_voice_watch_reset(&s_voice_watch)");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        scr_fm("SCR-A15 RED: Listening case 内 kidpal_voice_watch_reset(&s_voice_watch) "
               "= %d 处（冻结 >=1）——跨会话全清复位被软复位顶掉（新会话继承上一会话的"
               "素材证据/asr_age_s：D3 与 A7 判据跨会话污染）", n));

    n = scr_count_in_slice(region, "kDeviceStateSpeaking");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        scr_fm("SCR-A15 RED: Listening case 内 kDeviceStateSpeaking = %d 处（冻结 >=1）"
               "——复位不分派于状态迁移（无条件软复位 = 跨会话残留回归；无条件全清 = "
               "引导播报后素材证据丢失；须按旧状态 == kDeviceStateSpeaking 条件分派）",
               n));
    free(region);

    /* ⑤ Initialize 区：单源 60 注入 + cfg 接线（防两个 60 常量各自漂移）。 */
    cfg_region = scr_region_between(SCR_APP_CC, "void Application::Initialize()",
                                    "void Application::Run()");
    TEST_ASSERT_NOT_NULL_MESSAGE(cfg_region,
        "SCR-A15 RED(infra): Initialize 区切取失败——锚漂移须复核");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(cfg_region, "kSessionIdleTimeoutS"),
        "SCR-A15 RED: Initialize 区零 kSessionIdleTimeoutS——超时阈值未从既有命名"
        "常量注入看门缝（第二处 60 魔数 = 两个 60 各自漂移）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(cfg_region, "kidpal_voice_watch_cfg_default("),
        "SCR-A15 RED: Initialize 区零 kidpal_voice_watch_cfg_default(——缝默认阈值"
        "未接线（sustain/asr/trust 三阈值悬空 = 零值 -> 判据全错）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(cfg_region, "idle_timeout_s"),
        "SCR-A15 RED: Initialize 区零 idle_timeout_s 字段赋值——调用方注入点缺失"
        "（cfg 的 60 未落地）");
    free(cfg_region);

    /* 附 A：seam 头在世 + 六个宏字面（单一源；本文件以头文件文本再钉一次）。 */
    hdr = scr_read("firmware/main/session_voice_watch.h", NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(hdr,
        "SCR-A15 RED: firmware/main/session_voice_watch.h 不存在——纯逻辑看门缝"
        "契约头未落（BUILDER DUTY：新建 session_voice_watch.{c,h}）");
    for (i = 0; i < sizeof(macros) / sizeof(macros[0]); i++) {
        long v = scr_vw_macro_value(hdr, macros[i].define_needle);
        TEST_ASSERT_TRUE_MESSAGE(v == macros[i].value,
            scr_fm("SCR-A15 RED: seam 头宏 %s 解析 = %ld（期望 %ld）——头宏字面偏离"
                   "冻结值（阈值单源被改 = 判据语义漂移，须回 planner 重冻结）",
                   macros[i].label, v, macros[i].value));
    }
    free(hdr);

    /* 附 B：seam 源进设备构建（firmware/main/CMakeLists.txt，manifest 在册）。 */
    n = scr_count_in_file("firmware/main/CMakeLists.txt", "session_voice_watch.c");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        scr_fm("SCR-A15 RED: firmware/main/CMakeLists.txt 零 session_voice_watch.c"
               "（冻结 >=1，实测 %d）——缝未进设备树（固件构建缺符号，接线在设备上"
               "不生效）", n));
}

/* ===========================================================================
 * RED baseline 判读口径（freeze 相位，真机轮 B 修复轮 2026-11-18 实测）:
 *   本文件已在 host runner 注册（firmware/main/test/host_app/main/CMakeLists.txt），
 *   故按 host runner 全量实跑取红基线（DEV-GOVERNANCE 验收测试纪律 §3 冻结即冒烟 +
 *   §4 C 类冻结验收实编；编译通道 error 为零）。
 *   设计态红（builder 未落 session_voice_watch.{c,h} 与接线时）:
 *     - SCR-A5：CLOCK_TICK 区零 kidpal_voice_watch_tick( + !s_vad_speaking 仍 2 处；
 *     - SCR-A8：asr_seen 零命中 + !s_story_rec_entered 仍 1 处；
 *     - SCR-A9/A10/A11/A12/A13/A14 行为半：weak 哨兵 -2 / 不写字段 = frozen seam
 *       not implemented（真实缺失，非链接错误）；
 *     - SCR-A14 结构半：零 listening_diag / 零字段；
 *     - SCR-A15：六件接线全缺 + 头文件不存在 + 设备 CMakeLists 零登记。
 *   MUST-STAY-GREEN at baseline: SCR-A1/A2/A3/A4/A6/A7（上一轮实现面零偏移）。
 *   tighten 补丁 2026-11-18（会话内软复位保素材）实测：320 Tests / 5 Failures ——
 *   3 条上游存量红（p1_auxbar_style AB-A1/A2/A4）+ **恰 2 条本补丁设计态红**：
 *     - SCR-A13：weak 哨兵把 asr_seen 清 0 -> 「reset_soft 后 asr_seen == 1」红；
 *     - SCR-A15：Listening case 内缺 `kidpal_voice_watch_reset_soft(&s_voice_watch)`
 *       （实参形态计数 0）——接线未按迁移条件分派。
 *   其余本包 case（A5/A8/A9–A12/A14 与 A15 的既有六件）全绿 = 新断言各自只打自己
 *   那一处缺失，未污染既有判别力。
 *   cwd 口径：结构扫描以真实工作树为输入，root 由 cwd 自持定位（仓根 /
 *   firmware/main/test/host_app 均可）。
 * =========================================================================== */

/* ===========================================================================
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-11-17  真机轮 A 修复轮 initial additive：SCR-A8 新增（D3 空素材收尾
 *               producer 源锚代理）+ 设备结果标记成对申报。
 *   2026-11-18  真机轮 B 修复轮（授权链 = human 2026-11-18「按 B 修」+ plan.md
 *               「### 真机轮 B 修复轮（2026-11-18）技术方案」planner 裁定 5/6/7）。
 *               纯 additive 补断言 + 极性重锚 + 一条退役；零放宽、零删除既有
 *               非退役断言：
 *
 *               ① SCR-A5 重锚（极性反转 + 正锚）：
 *                  旧钉 —— CLOCK_TICK 区含 `!s_vad_speaking`（瞬时电平门在场，
 *                  修订建议 3 时代：电平是收尾的必要条件）。
 *                  新钉 —— CLOCK_TICK 区 `!s_vad_speaking` **恰 0**（负锚，禁回潮）
 *                  + 新增正锚 `kidpal_voice_watch_tick(` >= 1（承接原「驱动点在
 *                  场」之意，驱动点从裸判据改为缝调用）。其余断言（常量 60 /
 *                  定义位次 / kDeviceStateListening / 记录语境豁免 / 收口调用 /
 *                  VAD 沿重置两件）零改动。
 *                  为什么 —— 轮 B 真机证据：普通会话静置 3m13s 不触发
 *                  （07:18:23 idle->listening 至 07:21:36 手动 overlay_close，
 *                  中间零状态迁移），两条失效候选（M1 电平恒真 / M2 起音沿周期
 *                  性归零）在「瞬时电平必要」判据下都无解：电平卡死时 !电平恒假，
 *                  起音沿风暴时 clock_ticks_ 反复归零。判据本体迁入缝的 quiet_s
 *                  （有效语音证据累计静默），电平只作连续段时长输入。保留旧正锚
 *                  会让 builder 照旧判据实现，故极性必须反转为负锚。
 *
 *               ② SCR-A8 重锚（素材门换源）+ 一条退役：
 *                  旧钉 A —— CLOCK_TICK 区 `!s_story_rec_entered` >= 1（素材门
 *                  读 VAD 起音沿置位的 file-static 镜像）。
 *                  新钉 A —— CLOCK_TICK 区 `asr_seen` >= 1（素材门读缝内「引擎真的
 *                  转写出一句」）+ `!s_story_rec_entered` **恰 0**（旧门退役负锚）。
 *                  旧钉 B —— CLOCK_TICK 区 `!s_vad_speaking` >= 2。
 *                  新钉 B —— 整条退役（同一事实「电平门不得回潮」已由 SCR-A5 的
 *                  全区负锚（恰 0）单一承载；双钉只会产生 co-red 噪声，且 SCR-A8 的
 *                  计数会随实现形态抖动）。
 *                  为什么 —— 轮 B 设备日志：12 条 `story_vad_recording_enter` 全落
 *                  故事会话起步 1 秒内，该行前置条件含 `s_vad_speaking == 1`，即
 *                  无人说话时 VAD 也让出一记 speech 沿并把 `s_story_rec_entered`
 *                  置真；而它正是 D3 的「从未收到素材」判据 —— 旧信号下 D3 在真机上
 *                  结构性不可达。新信号与 front 累积器同源（front 的 accum 也只吃
 *                  `stt` 文本），两侧对「有没有素材」不再可能判断不一致。
 *                  `s_story_rec_entered` 保留其录音页切换/幂等原用途，不进 producer
 *                  判据。其余断言（常量 60 / 定义位次 / 交叉负锚 / 记录语境正负门 /
 *                  收口调用 >=2 / 审计 token / 零 OpenAudioChannel( / 零 %10）零改动。
 *
 *               ③ 新增 SCR-A9–A15：
 *                  A9 静默恰第 60 秒收尾（真行为；含 asr_seen 门的判别力——
 *                  漏门会红在 85 秒）；
 *                  A10 M1 电平卡死 400 tick -> 恰 160 秒收尾 + suspect=1
 *                  （真行为，kill-shot）；
 *                  A11 M2 起音沿风暴 vs 4 秒暴发成对（真行为）；
 *                  A12 A7 连续讲话零误收成对（真行为；planner 明令不可降级为
 *                  源文本）；
 *                  A13 reset 跨会话零残留 + note_asr 契约 + cfg_default 三阈值
 *                  （真行为）；
 *                  A14 诊断限频决策表 + listening_diag 结构半（真行为 + 结构）；
 *                  A15 应用层接线六件 + seam 头宏单源 + 设备构建登记（结构，
 *                  源锚代理）。A13/A14 各为单 case 多断言、A15 合为一条多断言
 *                  case（planner 裁定 7：避免 case 数膨胀）。
 *
 *               ④ 头注同步：[S5] 改写为「缝 + 接线」判据；新增 [S8] 缝契约节；
 *                  强度申报增补 A9–A15 与 GAP-4 重述 / GAP-5 / EQ-3；
 *                  device-outcome 成对申报保留但范围收窄为接线与主循环运行时。
 *
 *               ⑤ 零偏移面：SCR-A1/A2/A3/A4/A6/A7 全部断言零改动；
 *                  device-outcome 标记字面（`device-outcome: SCR-A8` /
 *                  `host-uncompilable-path: firmware/main/application.cc`）保留；
 *                  host CMakeLists 零改（本文件已注册，修订断言随文件自动携带）。
 *
 *   2026-11-18  tighten 补丁（planner 白盒复核发现的高风险回归；授权链 = 本轮
 *               tighten-only 派发）。纯 additive：零放宽、零删除、零改既有断言语义。
 *               ① 契约 [S8] (5) 增补第六件 `kidpal_voice_watch_reset_soft()`：
 *                  会话内软复位 = 清 quiet_s / level_run_s / asr_age_s / tick_s /
 *                  diag_last_s / suspect 六件，**保留 asr_seen**；跨会话边界仍走
 *                  `reset()` 七字段全清。
 *                  为什么 —— 修复轮落地的 `case kDeviceStateListening:` 尾部每次进入
 *                  聆听都调 `reset()`。会话中的引导播报让设备 Listening -> Speaking
 *                  -> Listening，回到聆听时这次 `reset()` 把 `asr_seen` 清成 0；而
 *                  `asr_seen` 是 D3 空素材 producer 的唯一素材门 —— 一个孩子已经讲过
 *                  话、有素材的故事会话，在引导播报后静置 60 秒会被判成「空素材」并
 *                  不归档地释放，孩子已讲的内容丢失。
 *               ② [S8] 接线形态增补：状态迁移监听器把 `old_state` 存进 file-static，
 *                  Listening case 尾部按旧状态分派（旧 == Speaking -> reset_soft；
 *                  否则 -> reset），之后 `s_vad_speaking = false;` 与 `src=enter`
 *                  诊断行维持不变。
 *               ③ SCR-A13 追加（同 case，不新开）：脏置 asr_seen=1 与六件非零 ->
 *                  reset_soft 后 asr_seen == 1 且其余六件全 0；再以 reset() 追一条
 *                  「软复位之后再硬复位仍七字段全清」（软复位不得削弱跨会话语义）。
 *               ④ SCR-A15 ③ 追加（同 case，不新开）：Listening 区三条字面同时在场
 *                  —— `kidpal_voice_watch_reset_soft(&s_voice_watch)` >= 1、
 *                  `kidpal_voice_watch_reset(&s_voice_watch)` >= 1、
 *                  `kDeviceStateSpeaking` >= 1。两条 needle 一律带实参：裸
 *                  `kidpal_voice_watch_reset(` 是 `_reset_soft(` 的前缀子串，会把
 *                  两者混计。既有 `kidpal_voice_watch_reset(` 断言原样保留（只加不减）。
 *               ⑤ 头门控/哨兵段：本文件自带 `void kidpal_voice_watch_reset_soft(
 *                  kidpal_voice_watch_t *w);` 原型（与头同型重复声明合法，头文件本轮
 *                  尚无该原型）并给 `__attribute__((weak))` 哨兵实现 —— 哨兵刻意把
 *                  `asr_seen` 清 0（等价于按旧全清语义兜底），使新断言在缝未实现 /
 *                  接线未落时必红；实现落地后强符号覆盖哨兵。
 *               ⑥ 零偏移面：SCR-A1..A12、A14 全部断言零改动；A13 / A15 只追加断言。
 *                  新增红基线实测（2026-11-18）：320 Tests / 5 Failures —— 3 条上游
 *                  存量红（p1_auxbar_style AB-A1/A2/A4）+ 本补丁 2 条设计态红
 *                  （SCR-A13 软复位保素材、SCR-A15 条件分派三件）；其余本包 case
 *                  全绿，证明新断言各自只打自己那一处缺失。
 * =========================================================================== */