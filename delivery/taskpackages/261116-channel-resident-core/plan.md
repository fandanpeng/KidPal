---
changeId: 261116-channel-resident-core
title: 通道常驻核心——会话结束收口到 listen stop、front 心跳保活与设备自愈巡检、60s 无语音超时接线
rigor: full
riskTier: high
sourceType: proposal
origin:
  ref: delivery/workstreams/261115-session-channel-resident/PROPOSAL.md
  workstream: 261115-session-channel-resident
reviewed-by: 主线程按 FROZEN workstream PROPOSAL 拆包（2026-11-16）；门 1 待 architect checklist review
date: 2026-11-16
gateProfile: ["evidence-integrity", "test-quality", "state-machine", "mcp-tool-contract"]
dependsOn: []
mergeOrder: 77
---

# 背景摘录（整改型锚「问题与根因」，PROPOSAL §2）

- **每次结束会话都在拆了再建**：四条本地关收尾路径（录音丢弃 / KEY 单击结束 / 覆盖层关层 / 编程唤起）一律 `CloseAudioChannel()` + 降 `LOW_POWER` + 5 秒后 `ScheduleResidentRebuild()` 重建。常驻通道本就是设计目标态（提醒推送与故事预取都跑在它上面），断了再建纯绕路。真机代价（KPLOG029 uptime 毫秒轴）：温通道按键进聆听 0.82s，撞上重建窗的冷通道 1.57s（含 TLS 证书验证 0.53s），每次重建另付约 15 秒 bootstrap burst。
- **闲置约 137 秒后首按键必走重连**：设备侧「通道是否打开」的判据含硬编码的「120 秒无下行数据即判死」，而会话间隙本来就该没有下行——判据必然到期（TECH-261012 首按键失同步的第一段根因）。
- **60 秒无语音超时零 producer**：纯模型 `ses_machine_dispatch` 生产调用点为 0（本批复核为真），真机从未发生 60 秒自动关闭。今天设备一轮聊完停在聆听态、麦克风常开，最终是被「约 137 秒后 WS 通道超时」这个意外兜底拽回 Idle 的。

**根因**（详见 FROZEN PROPOSAL T1–T3）：会话与通道揉在一个概念里——「结束会话」的物理动作就是「拆通道」；120s 判据的语义在常驻模型下已错位；3 次退避是 boot 期预算，被复用成唯一长时重连路径。

# 可行性判据

- 引擎侧零改动成立：`listen stop` 在引擎侧只置 `client_voice_stop` 并触发尾音 ASR，不置 `close_after_chat`、不关连接；`listen start` 调 `reset_audio_states()`，同一连接多轮 start/stop 是引擎既有支持形态；记录语境闭锁已由 engine-patch 既有的 `_kidpal_listen_closed` 承载。ADR-015 的 `user=<device_id>` + 持久会话把对话连续性权威移到 OpenClaw，本批对记忆口径零影响。
- 设备侧 `IsTimeout()` 的阈值与逻辑零改动，只把「120 秒无下行 = 死链」升级为「120 秒无心跳 = 死链」；`WebSocket::Ping()` 保活已实证否决（Pong 分支空实现、`Ping()` 返回 void）。
- front 已有自推通路先例（`_push_display_payload`、`_push_story_list`、`kidpal/alert`）与 `listen stop` 既有旁路消费缝（`_story_stop_on_listen_stop`），本包只扩执行面、不新造通道。

# 跨包事实与共享冻结面约束（含跨切片边角）

- **P1→P2 边界**：P1 建立统一会话结束处理器与 P1 内触发源（四条本地关 + 60s 超时）；云端 `end_session` 的三段补链（prompt 告别纪律 / `display_schema` self_push 豁免口扩 / 设备 `case 0` → `EndVoiceSession`）与自动重连封顶长跑 + 人工意向位**归 P2**。P1 不得提前改 `mcp_server.cc`、`display_schema.py`、prompt 制品。
- **`application.cc` 同区域串行**：P2 会改 `TryStartAutoConnect` task 体与四个人工入口守卫行；P1 改四条收尾路径、`HandleEndVoiceSessionEvent` 与 CLOCK_TICK 消费区，两者不相交。P1 合并后 P2 从新 main 起分支并重取行号锚。
- **验收齿成批重写（显式申报，非静默改齿）**：`boot_auto_connect.acceptance.c` 的 RC-B2（四收尾点各调 `ScheduleResidentRebuild()` 恰 1）与 RC-A3（helper 区消费 `kResidentRebuildDelayS`）**整条退役**；K6-A1/A3/A5 的守卫三条件与人工入口 in-flight 守卫**保绿零改动**。`voice_round_ui.acceptance.c` 的 `local_close_settle` 4 命中 src token 族**保绿**，同区三件套断言按 SCR-A1 负锚重写。`record_session_live.acceptance.c` 的录音丢弃面改为「`SendKidpalStoryDiscard()` 先、会话结束事件后」的**有序断言**。
- **front 心跳闸极性**：`KIDPAL_CHANNEL_HEARTBEAT` **代码默认开**，`0/false/off/no` 为**回滚开关**而非上线开关——与 `KIDPAL_STORY_RESTORE_PUSH` 先例极性相反（旧固件收到 keepalive 的唯一副作用是一条 WARN；默认关会让新固件退化成每 ~120s 判死 + 立即重连的 churn）。front 与固件必须同批上线，回滚剧本须写「两半一起回滚」。
- **记录语境释放是承重单点**：引擎 `_kidpal_record_context` 为真时 `chat()` 在 depth==0 第三守卫直接 return（零 LLM 零 TTS）；连接级 teardown 在 L1 常驻后不再兜底，front 漏发 `active=false` = 引擎对该连接永久抑制 = 设备哑火。双层防线（`listen stop` 主 + `ctx` 边沿兜底）都要有独立验收断言。

# 铁律约束

- **不改引擎代码与 config**：`server/deploy/engine-patch/**` 与 ECS `core/**` 零 diff；`close_connection_no_voice_time: 86400` + `end_prompt.enable: false` 保持。
- **不动契约与 spec 文件**：`docs/contracts/**`、`docs/architecture.md`、`docs/requirements.md` 只读消费；发现需要新线格式或新语义立即停并升级。
- **Idle 分支八件清位链逐条保留**（`EnableVoiceProcessing(false)` = 麦克风口径不变、[R2] 页态复位、`KidpalStorySessionEndSync` 镜像同步、keybar 场景复位（REMINDER 豁免）、`kidpal_render_node_voice_end`（OQ3=c）、记录语境清位、`RaiseReminderQueue`）。
- **五个真拆通道点保绿**：`HandleNetworkDisconnectedEvent` / `Reboot` / `UpgradeFirmware` / `SetAecMode` / `ResetProtocol`，`local_close_pending_` 分类机制与「零误重连」不变量不动。
- **零第二条重连路径**：自愈巡检只调既有 `TryStartAutoConnect`，不自建 task、不自己 `OpenAudioChannel`。
- **discard 先于 listen stop**：同一 TCP 有序；front 先 `acc.cancel` 再消费 stop，已丢弃素材不得被复活成归档。
- **`IsTimeout()` 命中日志**改为跳变沿单次或降频（巡检每秒调用不得刷日志）；若需改 `protocol.cc`，经 planner 扩单通道回评补 manifest，不自扩。

# demo 证据形态（与 roadmap 一致）

- host runner + server pytest：SCR-A1–A7 与 SCRF-B1–B5 的 RED→GREEN 和变异证据；四个收尾点负锚 / 五个真拆通道正锚 / discard→stop 有序不归档为承重齿。
- device build：本轮改动的编译面（`application.cc` / `application.h` / 验收件）通过；`protocol.cc` 若经扩通道纳入则一并构建。
- 一轮真机轮 A（human 持板，front 由 agent 部署）：A1–A10 全绿，三源日志（设备 KPLOG / front systemd / 引擎 docker）互证；证据落本包 `current/`。