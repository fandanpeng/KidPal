---
id: TECH-261014-session-end-channels-unwired
title: 会话结束三通道有两通道未接线——60 秒超时设备侧零 timer producer、结束意图（云端判）WS 链路无下发，致一轮聊完后设备常驻聆听（实测：说「拜拜」后仍开着麦直到 WS 通道超时约 137 秒）
domain: tech
status: deferred
severity: MAJOR
owner: dev-loop（固件面 + 引擎/网关面）
trigger: human 排期后的会话生命周期修复包；或下次动 session_state_machine / 会话收尾链的包顺带评估（本条体量含双阈值与云端判据取舍，不建议纯顺带）
opened: 2026-10-14
paths: ["firmware/main/session_state_machine.cc", "firmware/main/application.cc", "server/deploy/（引擎侧兜底配置）"]
source: 2026-10-14 真机联测（human 持板 + 串口全程捕获 /c/tmp/live-261014-round2-r2.log，说「先到这儿」「好，拜拜」两轮后状态停在 listening）；requirements.md §2.1（:43 结束三通道）+ :47 ⚠️ 60 秒超时实现侧对账（Phase 1 修复项）；代码锚 ses_machine_dispatch 全仓仅测试调用
handoff: 独立包（体量含录音态 120 秒双阈值 + 云端/设备侧判据取舍）；与本日 261014-voice-round-entry-exit-fixes 的出口守卫同族但不同面，不并入该包
---

**现象（2026-10-14 真机联测 human 报）**：正常问答多轮后孩子说「先到这儿」「好，拜拜」，设备播完告别语即停在聆听态、麦克风常开，直到 WS 通道超时（约 137 秒，`Protocol: Channel timeout`）才落 idle。串口证据：`State: speaking -> listening`（:300）后无任何后续收尾转换。

**设计口径（requirements §2.1 结束三通道，:42-45）**：① 超时 = 60 秒无语音自动关闭；② 结束意图 = 孩子说「拜拜/不用了」→ **云端判为**结束意图 → 关闭；③ 再按 KEY/BOOT 手动关闭。三者任一即应关闭。

**实现侧对账（两通道均未接线，逐条核实）**：

- **① 60 秒超时**：`SESSION_TIMEOUT_60S` 在纯模型（`session_state_machine.cc`）有逻辑与迁移表，但 `ses_machine_dispatch` **全仓调用点仅在 `firmware/main/test/**`（含 archive，逐点核实）——生产代码零调用、零 timer producer**。requirements.md:47 ⚠️ 已登记（2026-09-14 查证）：**真机 60 秒自动关闭从未发生**；MVP 实际代管 = 引擎侧（已修正为 24h 兜底 + 告别语关闭）。本条目 = 该 ⚠️ 注记所指 Phase 1 修复项的正式账目落点（原仅存于需求注记与 261014-post-speak-listening-lingering 条目的引用中，无独立条目）。
- **② 结束意图**：需求明写「**云端**判为结束意图」——设备侧 `SES_EVT_END_INTENT` 同样只有纯模型、无活侧 producer；而 `goodbye` 消息处理仅在 **MQTT 协议**（`firmware/main/protocols/mqtt_protocol.cc:115`），**现役 WebSocket（方向 B）链路无 goodbye/关闭语义的消费**，引擎侧亦无该下发（设备 `OnIncomingJson` 只处理 tts/stt/llm/mcp/system/alert/custom）。故「拜拜」不会被任何一侧判成结束。

**影响面**：日常每次对话结束（尤其孩子主动道别）设备都继续开麦，一是与 261014-post-speak-listening-lingering 同族的「AI 插话 / 环境音被当输入」风险（本轮已修的是提醒/查字旁路与多句轮出口，本条是**正常对话轮的自然收尾**），二是白耗电量与网络。触发频率 = 每轮对话。

**修法方向（归包计划期裁，含真取舍）**：

- ① 设备侧补 timer producer（60 秒，需带**录音态 120 秒例外**——requirements §2.1 录音态超时口径，双阈值随接线一并落地）；
- ② 结束意图二选一：**引擎/网关侧**判「拜拜/不用了」→ 下发关闭（贴合需求原文「云端判」；需新 wire 语义或复用既有 `alert`/新 message type，**若走新消息形态即触「固件协议与服务端语义同时动」→ 命中跨系统结构性升级判据，走 Track A + 契约面评估**）vs **设备侧**本地判据（不动 wire，但与需求「云端判」口径不符，需需求面确认改口径）。
- 两条**独立但也可能合并**：若仅接 ①，设备在说了拜拜后仍要空等 60 秒；若仅做 ②，则无 goodbye 的会话（家长不在、孩子走开）仍永不超时。

**风险**：riskTier 预估 medium-high——①纯设备侧且口径已成文，但 ② 的选择可能触跨系统结构性升级（存疑向严，排期时由 architect 裁）；双阈值接线触会话状态机核心路径，回归面覆盖各会话类型（问答 / 记录 / 查字 / 提醒）。

**范围收窄注记（`261019-p3-record-dialogue-guide`，2026-10-19，spine q4 裁定 = 主线程 append、结项不归该批）**：本条目「双阈值 60s/120s timer 零 producer」中的 **120s 录音态阈值已随 requirements v1.65 + ADR-013 作废**——记录会话自动收尾改归网关侧卡壳计时器族（卡壳引导 2 轮无回应自动收尾，取代 120s 纯静默超时；架构权威 = architecture.md §5.2 场景④ / §5.1.2，120s 语义联动对账见该批 §12 变更记录行）。修法方向①的接线范围相应**收窄为单阈值**（60s 问答会话超时 timer producer）；方向②的结束意图取舍不受影响。结项仍归独立包（体量重估后含单阈值接线 + 结束意图二选一），不随该批闭环。
