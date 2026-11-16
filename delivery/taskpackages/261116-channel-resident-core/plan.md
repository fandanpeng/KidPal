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
- **`application.cc` 同区域串行**：P2 会改 `TryStartAutoConnect` task 体与四个人工入口守卫行；P1 改四条收尾路径、`HandleEndVoiceSessionEvent` 与 CLOCK_TICK 消费区。两包**在同一批函数内改不相交行段**（`HandleEndVoiceSessionEvent` 与 `HandleToggleChatEvent` 被两包分别改不同行段：P1 = 收尾动作序，P2 = in-flight 守卫行），不是「不相交函数区」；P1 冻结件 `session_channel_resident.acceptance.c` 的 SCR-A2 将被 P2 的 SEI-A4 重锚（已登记跨包转手项）。P1 合并后 P2 从新 main 起分支并重取行号锚，覆盖这两处。
- **server 面相邻冻结锚（扫查结果非空，逐条处置）**：
  1. `server/acceptance/test_record_cancel_lifecycle.acceptance.py` —— **must-stay-green**：`FIRST_COMPLETED` 不悬挂回归齿 + `cancel(reason="ws_disconnect")` + 会话腿摘除。P1 新增的心跳 task 不得进 `FIRST_COMPLETED` 集合（否则心跳永不结束会拖住收尾）；此件是离线阶段唯一能抓住该错误的既有断言。
  2. `server/acceptance/test_m4k1_story_cloud_link.acceptance.py` —— **must-stay-green**：SAO-A1/A2 真机序归档链（`listen stop` 先、`stt` 后）；P1 的收尾三件不得 cancel `pending_stop`。
  3. `server/acceptance/test_m4k3_record_session_live.acceptance.py` —— **frozen-rebaseline**：A7「`ctx` 非 story 零帧」的冻结语义前提被 P1 的 `ctx` 边沿完整化取代，须显式记处置（预期仍绿但语义失真）；A4 断连后会话态释放保绿。
  4. `server/acceptance/test_record_dialogue_guide.acceptance.py` —— **frozen-rebaseline**：三处经泵投 `listen stop`；P1 的收尾三件会在既有 case 里新跑一遍，凡钉引擎腿帧序/帧数的断言逐条核偏移。
  5. `server/acceptance/test_record_front_polish.acceptance.py` —— **must-stay-green**：「accumulate 照常 feed、pending_stop 照常」零偏移。
  处置原则：沿用 / 重锚 / human 裁三选一逐条留痕；若判定必须改其冻结断言，走 planner 扩单通道回评补 manifest，不就地自扩。
- **验收齿成批重写（显式申报，非静默改齿）**：`boot_auto_connect.acceptance.c` 的 RC-B2（四收尾点各调 `ScheduleResidentRebuild()` 恰 1）与 RC-A3（helper 区消费 `kResidentRebuildDelayS`）**整条退役**；K6-A1/A3/A5 的守卫三条件与人工入口 in-flight 守卫**保绿零改动**。`voice_round_ui.acceptance.c` 的 `local_close_settle` 4 命中 src token 族**保绿**，同区三件套断言按 SCR-A1 负锚重写。`record_session_live.acceptance.c` 的录音丢弃面改为「`SendKidpalStoryDiscard()` 先、会话结束事件后」的**有序断言**。
- **front 心跳闸极性**：`KIDPAL_CHANNEL_HEARTBEAT` **代码默认开**，`0/false/off/no` 为**回滚开关**而非上线开关——与 `KIDPAL_STORY_RESTORE_PUSH` 先例极性相反（旧固件收到 keepalive 的唯一副作用是一条 WARN；默认关会让新固件退化成每 ~120s 判死 + 立即重连的 churn）。front 与固件必须同批上线，回滚剧本须写「两半一起回滚」。
- **记录语境释放是承重单点**：引擎 `_kidpal_record_context` 为真时 `chat()` 在 depth==0 第三守卫直接 return（零 LLM 零 TTS）；连接级 teardown 在 L1 常驻后不再兜底，front 漏发 `active=false` = 引擎对该连接永久抑制 = 设备哑火。**三层防线**都要有独立验收断言：① `listen stop` 分层收尾（`pending_stop` 受保护、当刻绝不 cancel）② `_story_timeout_sweep` 的 pending 超时分支补 `_release_record_context` 调用（**本批新增的承重缺口**：该分支今天只 `release_pending`、不释放记录语境，通道常驻后兜底消失 = 该连接后续问答永久哑火）③ `ctx` 边沿兜底分态（`accumulate` → cancel+释放；`pending_stop` → 不动作；`chat` 但标记真 → 只释放）。窗口上界 = 既有冻结常量 `PENDING_STOP_SECONDS=10.0`；`story.py` 三缝签名与判据零改动。

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
- 一轮真机轮 A（human 持板，front 由 agent 部署）：A1–A13 全绿（A8 录音态提醒排队例外与 A9 查字窗关闭路径为 package-split 明列回归项；A11–A13 为追加项），三源日志（设备 KPLOG / front systemd / 引擎 docker）互证；证据落本包 `current/`。
# 验收方向（acceptance-author 细化，非冻结）

> 本节是拆包期的**断言方向与承重判据**，不是冻结验收；`acceptance` / `acceptanceTestsFile` / `redBaselineResult` 在 taskpackage 中保持空骨架，由 Pod 内 acceptance-author 唯一产出并冻结。

- SCR-A1: 六触发源收口——四个收尾点函数区（Application::CloseAudioChannel / HandleToggleChatEvent Listening 分支 / HandleEndVoiceSessionEvent / WakeWordInvoke Listening 分支）负锚 = 区内 `CloseAudioChannel(` 命中 0、`ScheduleResidentRebuild(` 命中 0、`SetPowerSaveLevel(PowerSaveLevel::LOW_POWER)` 命中 0；正锚 = 区内 `EndVoiceSession(` 或 `MAIN_EVENT_END_VOICE_SESSION` 命中 ≥ 1。回引 PROPOSAL T2 改法 1、ADR-019 ②。设计态红 = 四条收尾路径仍各自 CloseAudioChannel + LOW_POWER + ScheduleResidentRebuild。
- SCR-A2: 统一处理器动作序——HandleEndVoiceSessionEvent 区内 `SendStopListening()` 恰 1、`ResetDecoder()` 恰 1 且在 Speaking 分支内；三态门（Connecting/Listening/Speaking）、`auto_connect_in_flight_` 守卫、`!protocol_` 早退三件逐字在世（261024 语义零偏移）。回引 PROPOSAL T2、261024 overlay_close 时序自证。
- SCR-A3: 五个真拆通道点保绿（must-stay-green）——HandleNetworkDisconnectedEvent / Reboot / UpgradeFirmware / SetAecMode / ResetProtocol 五区各含 `CloseAudioChannel()` 恰 1。回引 PROPOSAL T2 正负锚分界。
- SCR-A4: Idle 分支八件清位链全保留——HandleStateChangedEvent 的 kDeviceStateIdle case 内八个符号各命中 ≥ 1（EnableVoiceProcessing(false) / EnableWakeWordDetection(true) / kidpal_story_session_end / KidpalStorySessionEndSync / kidpal_keybar_scene_reset 或 front_is_reminder 豁免判据 / kidpal_render_node_voice_end / kidpal_story_session_ctx_set(0) / RaiseReminderQueue）。回引 PROPOSAL T2 不变量、OQ3=c、S-F1.4。
- SCR-A5: 60s 超时 producer——CLOCK_TICK 消费区内命名常量（值 60）+ `kDeviceStateListening` 判据 + `kidpal_story_session_ctx_active()` 记录语境豁免判据 + EndVoiceSession 投递四件在世；VAD 说话沿重置在 MAIN_EVENT_VAD_CHANGE 区内。回引 requirements §2.1 ①、PROPOSAL T2/T3。设计态红 = 生产零 timer producer。
- SCR-A6: 心跳 no-op 分支——OnIncomingJson lambda 区内 `kidpal_keepalive` 字面命中 ≥ 1，且其位置先于 `Unknown message type` 的 ESP_LOGW（按区内偏移量断言）。回引契约 §四 kidpal_keepalive、PROPOSAL T1 改法 2。
- SCR-A7: 自愈巡检——CLOCK_TICK 区内 `IsAudioChannelOpened()` 与 `TryStartAutoConnect()` 各命中 ≥ 1 且守卫含 `auto_connect_in_flight_`；负锚 = 巡检区内 `OpenAudioChannel(` 命中 0（零第二条重连路径）。回引 PROPOSAL T1 改法 3。
- SCRF-B1: 心跳 producer（front）——注入假时钟跑一个周期，设备腿恰收一帧 `{"type":"kidpal_keepalive"}`；env `KIDPAL_CHANNEL_HEARTBEAT=0` → 零帧（闸有效，代码默认开）；发送抛异常 → task 静默退出、双泵 finally 三件仍执行。回引契约 §四、PROPOSAL T1 改法 2 与部署极性。
- SCRF-B2: `listen stop` 会话收尾**分层**（v1.2 F-01 修订）——① `chat`（问答会话）：不动作；「记录语境曾置位」标记为假时零注入（防每轮问答打一帧无用 `active=false`）② `accumulate` + accum 非空：已被 `_story_finish_session` 消费，收尾三件不得再动 ③ `accumulate` + accum 空 → `pending_stop`：**禁止 cancel、禁止释放（protected state）**，等待 sweep ④ `pending_stop` 重复 stop：`mark_pending` 幂等、不动作 ⑤ `accumulate` 未被归档路径消费（结构上不可达，防御性）：不 cancel，留既有卡壳/自动收尾 sweep（2 轮无回应 → 归档不丢弃）。回引 PROPOSAL T2 v1.2、SAO-A1/A2/A3、M4K1-A2。设计态红 = 当刻 cancel pending_stop（0 篇归档）。
- SCRF-B2b: pending 保护与 sweep 释放——注入时钟：`listen stop` 后 +9s 零释放、+10s 由 sweep 释放且 `active=false` 恰一帧入归属腿（腿集沿 `_story_auto_finish` 的 `_session_legs(sid) or list(_alert_legs)`）；先投 stt 走恢复归档路径则 sweep 不重复释放。回引 `front_app.py:1254-1257` 补释放缺口、`story.py` 三缝零改动。设计态红 = pending 超时不调 `_release_record_context`（常驻后永久哑火）。
- SCRF-B3: discard→stop 有序不归档（承重齿）——同一 session 依次投 `kidpal_story_discard` + `listen stop` → routing 回 chat、_story_finish_session 未被调、STORIES_DIR 文件数不变。回引契约 §四顺序约束、S-F2.2 AC4。
- SCRF-B4: ctx 边沿兜底**分态**（只在设备未发 stop 时可达）——`accumulate` → `cancel(reason="session_end_stale")` + 释放（常驻时代的 cancel-on-disconnect 同族，沿 human 2026-10-21 裁定口径，不发明「弃录自动归档」）；`pending_stop` → **不动作**（保护在途转写，只记 INFO）；`chat` 但「记录语境曾置位」标记真 → 只释放。重复 ctx(非 story) → 零新增注入（幂等）。回引契约 §四 kidpal_session_ctx、PROPOSAL T2 v1.2。
- SCRF-B4b: `pending_stop` 分支不动作（F-01 从 ctx 边沿这扇门复发的防线）——先投 ctx(story) 进语境 → 投 `listen stop`（accum 空 → pending_stop）→ 再投 ctx(home)：断言**零 `acc.cancel`**、routing 仍 `pending_stop`、`active=false` 零帧；随后 stt 到达仍恰一篇归档。回引 F-01 分层表第 3/4 行。
- SCRF-B5: 连接级 teardown 保绿（must-stay-green）——断开 → 腿双摘除 + acc.cancel(reason="ws_disconnect") 逐字不变。回引 PROPOSAL T2（teardown 降兜底、不删除）。
- SCRF-B6: 既有冻结 oracle 保绿回归网——`test_m4k1_story_cloud_link.acceptance.py`（SAO-A1/A2/A3、M4K1-A2）全数跑绿、`test_m4k3_record_session_live.acceptance.py` A7 处置留痕、`test_record_cancel_lifecycle.acceptance.py` FIRST_COMPLETED 齿保绿、`test_record_front_polish.acceptance.py` pending_stop 语义零偏移；任一红即不得进 integrator。

**建议验收文件（acceptance-author 定名可调整）**：`firmware/main/test/session_channel_resident.acceptance.c`、`server/acceptance/test_session_channel_resident.acceptance.py`

**红基线命令（沿用）**：`host: wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf（仓根起；执行目录 = firmware/main/test/host_app）；server: python -m pytest -q --import-mode=importlib -m "not live" server/acceptance（仓根起）`

**红基线预期形态**：新验收件落地时设计态红（实现缺席）；不得以推算代替实跑，数值由 acceptance-author 冻结时回填。
