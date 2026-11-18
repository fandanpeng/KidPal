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

# 技术方案（planner+architect 预评）

> **留痕**：2026-11-16，planner Step 1g 前置技术预评档（architect 只读 advisory，dispatch 见 pod-events dispatch-completed）。效力声明：**不豁免任何门、不改验收与 TaskPackage 事实、不构成通过判据**。本节为 planner 只读消费 plan.md 规则的唯一例外写入点（`task-intake-and-slicing.md` 例外条款）。方案基线 = `package-split.md` §2.2/§2.3，以下六处为 planner 预评对基线的**修正与补充**（其余沿用基线，不重复抄写）。

### 固件侧（`firmware/main/application.cc` + `application.h`）

**T1 · 收尾点收口的承载形态（对基线 §2.2-2 的细化与一处修正）**

- `Application::CloseAudioChannel()`（:972-990）**唯一调用方 = `mcp_server.cc:1025` DISCARD_RECORD case**（实核；五个真拆通道点全部直调 `protocol_->CloseAudioChannel()`，不经本 helper）。**调用上下文 = 协议接收任务**（`OnIncomingJson` → `McpServer::ParseMessage` → mcp 回调），**禁止直调 `HandleEndVoiceSessionEvent()`**（后者碰 `protocol_`/状态机/`audio_service_`，跨任务不安全）。故：helper 函数体改为「本地审计行 `src=discard_record` + `EndVoiceSession()` 事件位投递」，删 `protocol_->CloseAudioChannel()` / `SetDeviceState(Idle)` / `SetPowerSaveLevel(LOW_POWER)` / `ScheduleResidentRebuild()`。**`mcp_server.cc` 零改动即满足「`SendKidpalStoryDiscard()` 先于会话结束」的同一 TCP 有序约束**（帧在前、事件位在后，事件位在主循环下一拍消费）——这是守住 P2 边界（control case 0 归 P2）的关键。helper 保留历史名，头注写明「名字为历史名、语义 = 会话收尾」。
- `HandleToggleChatEvent` Listening 分支（:1031-1041）：**主循环上下文 → 直调 `HandleEndVoiceSessionEvent()`**（保同步语义），本区保留 `local_close_settle src=toggle_chat` 审计行，删五件套。
- `WakeWordInvoke` Listening 分支（:1508-1526）：已在 `Schedule` lambda 内 = 主循环 → lambda 体改直调 handler，保留 `src=wake_invoke` 审计行，删五件套。
- **收口形态两类，不得混用**：协议回调上下文（helper）走事件位；主循环上下文（toggle / wake-invoke / 60s producer）走直调。混用会引入跨任务竞态或丢同步语义。
- `HandleEndVoiceSessionEvent`（:1141-1170）= 唯一收口点：`!protocol_` 早退 / `auto_connect_in_flight_` 守卫 / 三态门（Connecting/Listening/Speaking）逐字保留；Speaking 分支 `ResetDecoder()` + `AbortSpeaking()` 不动；`protocol_->CloseAudioChannel()` → `protocol_->SendStopListening()`（恰 1）；`SetDeviceState(kDeviceStateIdle)` 保留；**删** `SetPowerSaveLevel(LOW_POWER)` 与 `ScheduleResidentRebuild()`；本区审计行 `src=overlay_close` 保留。
- **四个 `src=` token 的保绿形态（预评修正，acceptance-author 须按此冻结）**：审计行留在**各自调用点**、不在统一处理器里按参数重打——否则 `src=overlay_close` 会随其它三路一起进入处理器区、语义失真，而 `voice_round_ui.acceptance.c` OCE-A1 的「`local_close_settle` ≥1 + `src=overlay_close` ≥1」是**行级 region 断言**。保绿口径 = 四个字面 token（`overlay_close` / `discard_record` / `toggle_chat` / `wake_invoke`）在 `application.cc` 恒在世；OCE-A1 的 region 断言保持「处理器区含 `src=overlay_close`」成立，同区「`protocol_->CloseAudioChannel()` + `ScheduleResidentRebuild()`」两条按 SCR-A1 负锚反转。
- 五个真拆通道点（:508 / :1407 / :1426 / :1588 / :1601）与 `local_close_pending_` 分类、`OnDisconnected` A 路径逐字不动；Idle 分支八件清位链（:1244-1299）逐字不动。

**T2 · 60s 无语音超时 producer（CLOCK_TICK 消费区 :429-470）**

- 命名常量 `kSessionIdleTimeoutS = 60`（放 `kAutoConnectBackoffS` 同区，接替被删的 `kResidentRebuildDelayS` 位置）；判据 **四件**：`GetDeviceState() == kDeviceStateListening && !s_vad_speaking && clock_ticks_ >= kSessionIdleTimeoutS && !kidpal_story_session_ctx_active()` → `HandleEndVoiceSessionEvent()`（主循环同步）+ 审计行（`session_idle_timeout` 冻结 token）。记录语境豁免 = requirements §2.1 v1.65 硬语义。
- **`!s_vad_speaking` 电平门是预评补的承重件（对基线 SCR-A5 的加严）**：`s_vad_speaking` 是电平、`MAIN_EVENT_VAD_CHANGE` 只在起音沿产生一次——一段无停顿的 90s 连续讲话只有一个上升沿，**仅靠沿重置会在第 60 秒误收**（轮 A 的 A7「连续讲 >90s 不被误收」直接翻车）。加电平门后 A7 才必然成立，且不改 PROPOSAL A4 的「VAD 沿重置」语义（沿重置照做）。
- **说话沿重置**：`MAIN_EVENT_VAD_CHANGE` 的 Listening 分支内（:399-417）`if (s_vad_speaking) { clock_ticks_ = 0; }`。不要另写状态迁移归零（`HandleStateChangedEvent:1235` 已承载）。
- 记录会话豁免的边界：整理过渡态设备已 Idle，天然不命中。
- **VQ-E1 地雷（must-stay-green）**：`voice_round_ui.acceptance.c:1263-1271` 钉 `firmware/main` 下 `SES_EVT_SESSION_TIMEOUT_60S` 命中文件数恰 2（只许 `session_state_machine.h/.cc`）。producer 只用命名常量 + 收尾调用，**不得顺手接上纯模型事件**——否则该件由绿转红、且看起来像 must-stay-green 回归。

**T3 · 心跳 no-op 分支（InitializeProtocol 的 `OnIncomingJson` lambda）**

- 在 `else { ESP_LOGW(TAG, "Unknown message type: %s", ...) }`（:924-926）**之前**新增 `kidpal_keepalive` 分支：直接 return（DEBUG 级或不打）。位置先于 WARN 是 SCR-A6 的偏移量断言对象；每 30s 一条 WARN 会按 ~144KB/日 冲 SD 日志轮转预算。

**T4 · 自愈巡检（同一 CLOCK_TICK 区）+ `IsTimeout()` 日志降频**

- 巡检判据（**守卫短路顺序是承重件，逐字照写**）：`GetDeviceState() == kDeviceStateIdle && protocol_ && !auto_connect_in_flight_ && !protocol_->IsAudioChannelOpened()` → `TryStartAutoConnect()`。`!auto_connect_in_flight_` **必须先于** `IsAudioChannelOpened()`——in-flight 期（3 次退避 ≈35s）短路掉后，死链期 `IsTimeout()` 的 `ESP_LOGE` 频率从「每秒一条」降到「每轮一条」= 事实降频，**因此本包不改 `protocol.cc`**（预评结论；原 T4 的扩单撤销）。
- 隔离：巡检**不新建 task、不直调 `OpenAudioChannel`**（SCR-A7 负锚）；如需分频（如每 5s 一次）用独立 file-static，**不得复用 `clock_ticks_`**（它与 60s 判据共用会互相干扰、且状态迁移即归零）；**禁止写 `clock_ticks_ % 10`**（`diag_log.acceptance.c:1195-1198` DLG-A4 有该字面恰 0 负锚）。
- **必须申报的副作用**：常驻 + 每秒巡检把今天的「3 次退避即止、然后静默」变成事实上「每 ≈35s 一轮、无上限」的长跑重试（离线期每轮一次 TLS 握手尝试）。封顶长跑归 P2（SEI-A2/A3），但 P1 已提前改变形态 → 写进 review-packet 与轮 A A3/A12 观测项，并登记 `crossPackageBehaviors` 给 P2 更新行号锚与形态基线。
- 若真机轮实测 `IsTimeout()` 日志仍过密（>每轮一条），**带量测回报 planner 再走扩单通道**（届时 `protocol.cc` + `protocol.h` 两件同时入 manifest，`mutable` 跳变沿成员）；不得 builder 自扩。

### front 侧（`server/gateway/front/front_app.py`）

**T5 · 每连接心跳 task（`ws_gateway` :2446-2573）**

- 落点：`ws_gateway` 内、`await _push_schedule_connect(websocket)`（:2527）之后、`asyncio.wait({pump_up, pump_dn}, FIRST_COMPLETED)`（:2545）之前创建；**不得插在 `_register_alert_legs`(:2508) / `_register_session_legs`(:2512) / `_push_schedule_connect`(:2527) 三者之间**（`test_schedule_grid_restore.acceptance.py` K7-A4 钉这三者首现位次）。
- 发送形态 = 裸 `websocket.send_text(json.dumps({"type": "kidpal_keepalive"}))`（心跳不是 display 帧：**不过** `display_schema`、不走 `kidpal/display` mcp 封套）。
- **`finally` 内首行 `hb.cancel()` + `await asyncio.gather(hb, return_exceptions=True)`**（与 `_unregister_alert_legs` 同块）；**绝不进 `asyncio.wait(..., FIRST_COMPLETED)` 集合**（无限任务会永久拖住收尾——`test_record_cancel_lifecycle.acceptance.py` RCL-A3 的 6s 限时齿即此错误的唯一离线警报）。
- **循环体必须 sleep-first**（`while True: await asyncio.sleep(interval); await websocket.send_text(...)`）并整体包 `try/except Exception: return`。理由：`test_m4k1`（FIX-A1 :1177 / :1235）、`test_m4k3`（:717）、`test_record_cancel_lifecycle`（:992）**都真跑 `ws_gateway`**，建连即发帧会往假设备腿注入额外帧；sleep-first + 30s 间隔 → 上述四件零偏移。
- 开关/周期：`KIDPAL_CHANNEL_HEARTBEAT`（**代码默认开**；`0/false/off/no` 关 = 回滚开关，极性见 plan 跨包事实节）与 `KIDPAL_CHANNEL_HEARTBEAT_INTERVAL_S`（默认 30）。**模块级常量与 `channel_heartbeat_enabled()` helper 必须定义在 `ws_gateway` 之前**（建议靠 `story_restore_enabled()` :624 一带同型纯函数）——否则 `test_m4k3` 的 `test_a7_push_seam_lands_in_ctx_consume_not_connect`（:545-591，切 `await websocket.accept()` → `pump_up = asyncio.create_task`）位次齿会误伤。
- 部署面零改：`server/deploy/manifest.yaml` 的 `verify: md5` 是 src↔dst 现算比对、不存期望值；env 闸先例（`KIDPAL_STORY_RESTORE_PUSH`）不落部署制品。

**T6 · `listen stop` 分层收尾（`pump_client_to_upstream`，`_story_stop_on_listen_stop` 消费点之后）**

- 门控：先用 `up_obj.get("type") == "listen" and up_obj.get("state") == "stop"` 门住再做任何事（否则只投 `kidpal_story_fetch` 等帧的既有件会被无谓触达，如 `test_story_fetch.py:251` 真跑本泵）。
- 每连接「记录语境曾置位」标记（命名冻结 `_record_ctx_seen`，形态沿 `_POINTS_OBSERVED` 先例 = `weakref.WeakKeyDictionary` + 属性回退，按 device websocket 建）。**置位点** = ctx(story) 的 `ctx_push_edge` 分支（:1035-1053）+ `_inject_record_context_enter` 的两个调用点（:1621 / :1636，`pump_upstream_to_client` 有 `websocket` 形参可用）；**清位单点 = `_release_record_context`（:1673-1685）体内**按 `legs` 的 device_ws 逐个清——这样三个既有调用点（:1716/:1727/:1747）与新增的 sweep 调用点自动一致，SCRF-B2「chat 且标记假 → 零注入」与 SCRF-B4「重复 ctx 零新增注入」由它单源承载。
- **routing 陷阱（预评承重）**：accum 非空时 `_story_finish_session` 已 `acc.cancel(sid, reason="finish")`（:1712）→ routing 回 `"chat"`，与「普通问答 chat」**不可由 routing 区分**。∴ 分层表第 1 行与第 2 行只能靠上述标记分辨——这是标记不可省的原因。
- 分层动作按 `acc.routing(sid)`：`chat` → 标记假则**零动作零注入**；`accumulate` + accum 非空 → 已被归档路径消费，**不动作**；`accumulate` + accum 空（落 `pending_stop`）与 `pending_stop` 重复 stop → **禁止 cancel、禁止释放**（protected state，交 T7 的 10s sweep）；防御性未消费分支 → 不 cancel。
- 审计一行含冻结 token `session_end_boundary`（记 routing 分支 + 是否触发释放）。**当刻绝不 cancel pending_stop**（F-01 承重）。

**T7 · pending 超时补释放（`_story_timeout_sweep_body` :1254-1257）**

- `acc.release_pending(sid)` 返回 True 的分支追加 `await _release_record_context(sid, _session_legs(sid) or list(_alert_legs))`（腿集形态逐字沿 `_story_auto_finish:1353`）。守卫天然成立：`release_pending` 只对 `pending_stop` 返 True，而 `pending_stop` 只能经 ctx(story)/`start_recording` 进语境后到达。`server/gateway/story.py` 三缝（`release_pending` / `cancel` / `PENDING_STOP_SECONDS`）**零改动**。

**T8 · `kidpal_session_ctx` 边沿完整化分态（`_consume_kidpal_control` ctx 分支）**

- 收到**非 `story`** 的合法 ctx（`home` / `schedule` / `story_cover` / `points_home` / `points_ledger`）且标记真：`pending_stop` → 不动作只 INFO（保护在途转写，≤10s 上界）；`accumulate` → `acc.cancel(sid, reason="session_end_stale")` + 释放；`chat` 但标记真 → 只释放。重复 ctx(非 story) → 零新增注入（幂等）。points 两值的摘要注入照旧（零偏移）。
- **未知 `source_page` 仍拒绝 + warning 且不释放**（现 `elif` 行为逐字保留）——`test_m4k3` A7 与 `test_points_device_ui` K2D-A3 都钉这条；不得把未知值并入兜底释放分支。
- **契约读法风险（不得就地「修」）**：契约 §四写「离开 `story` 且记录语境仍活 = 释放」但**未钉时点**；F-01 的 `pending_stop` 延迟 ≤10s 释放与之相容。builder 若按字面理解成「当刻释放」就会把 F-01 改回去（毁在途转写）——判定契约要求当刻释放 = `SPEC_CONTRACT_DRIFT`，回 planner 判，不就地改。

**T9 · 连接级 teardown 保绿**

- `finally` 三件（腿双摘除 + `acc.cancel(reason="ws_disconnect")`）逐字保留；改动只是触发频率从「每会话」降为「每连接一次」。

### manifest / impactedTests 回填结论（本预评，已回填 taskpackage）

- **manifest 增 1 条**：`server/acceptance/test_m4k3_record_session_live.acceptance.py`——package-split §2.1 v1.2 已把它升为非条件成员（`:508` docstring 前提精确化 + 新增释放边沿姊妹齿，断言零删弱），taskpackage 原清单漏了它，照 package-split 实施会越界。
- **manifest 撤销原扩单**：`firmware/main/protocols/protocol.cc` **不入**（T4 的守卫短路方案使日志降到每轮一条；若实测仍密，带量测回报后 `protocol.cc` + `protocol.h` 两件同时入）。
- **manifest 沿用**：其余九条与 taskpackage 现值一致；`mcp_server.cc` / `server/gateway/story.py` / `display_schema.py` / `conftest.py` / `server/deploy/manifest.yaml` / `server/deploy/engine-patch/**` 维持负锚（`conftest.py` 无登记表、新验收件自动收集；`manifest.yaml` md5 现算）。
- **impactedTests 增 5 条（固件 must-stay-green）**：`record_session_lifecycle.acceptance.c`（Idle 分支八件链邻居）、`record_session_repair.acceptance.c`（VAD 块切区 + Idle 链切区）、`chrome_ghost.acceptance.c`（keybar scene reset + REMINDER 豁免）、`reminder_overlay_unify.acceptance.c`（`RaiseReminderQueue(` ≥1 = A8 的离线齿）、`diag_log.acceptance.c`（DLG-A4：`clock_ticks_ % 10` 恰 0 负锚）。
- **impactedTests 增 2 条（server must-stay-green）**：`test_points_device_ui.acceptance.py`（K2D-A3：points 两值零 record 注入 + 零 story_list，且未知值零 story_list）、`test_story_cover_nav.acceptance.py`（C8：`story_cover` 普通问答、真跑 `pump_client_to_upstream`）。
- **既有 8 条 note 精确化**：见 taskpackage `impactedTestsSeed[]`（RC-A2 tooth6 / OCE-A1 红绿面 / RSL 处置性质 / m4k3 精确锚 / RCL-A3 6s 限时 / FIX-A1 真跑 ws_gateway）。

### 验收方向修订建议（acceptance-author 冻结权；plan「验收方向」节原文保留不删）

1. **SCR-A1 负锚对 `Application::CloseAudioChannel()` 自身区结构上不可满足**（region helper `s` 即 start needle，区里恒含 `CloseAudioChannel(`）→ 负锚 needle 收窄为 `protocol_->CloseAudioChannel(`（与 SCR-A3 五区正锚同族、语义更准）；改名绕开会强制触碰 `mcp_server.cc` = 越界。
2. **SCR-A1 正锚 needle 与「主循环直调」不兼容**：`EndVoiceSession(` 在直调 `HandleEndVoiceSessionEvent()` 的区命中 0 → 正锚闭集显式加 `HandleEndVoiceSessionEvent()`（推荐，保同步语义），或四处统一投事件位（须复核 261024 的 SCHEDULE 位次自证）。
3. **SCR-A5 判据补 `!s_vad_speaking` 电平门**（T2）——否则 A7「连续讲 >90s 不被误收」不必然成立。
4. **RC-A2 tooth6 必须重钉**：`boot_auto_connect.acceptance.c:1269-1273` 钉「全文件 `ScheduleResidentRebuild()` 恰 5 处」→ P1 删净后为 0，须整条退役或重钉（plan 原 KRC-E2 漏列）。
5. **OCE-A1 红面比原描述宽**：tooth2 两条正锚（`protocol_->CloseAudioChannel()` ≥1、`ScheduleResidentRebuild()` ≥1）+ tooth3 三位次序（`ResetDecoder < AbortSpeaking < CloseAudioChannel`）→ 重锚为 `< SendStopListening()`；保绿面 = `local_close_settle` ≥1、`src=overlay_close` ≥1、tooth1 区切、tooth4 板级区。
6. **`record_session_live.acceptance.c` 处置性质更正**：RSL-A5/M4K3-A3 与 RCL-F1 扫的是 `mcp_server.cc` / `protocol.cc`（P1 两者零改）→ **文本仍绿、语义前提失真**（frozen-rebaseline 留痕），不是「改为有序断言」——有序断言今天已在 `:787-797` 且保绿。真正需要新增的有序/收口断言在**新文件**里。
7. **VQ-E1 tooth2 列入 must-stay-green**：`SES_EVT_SESSION_TIMEOUT_60S` 全 `firmware/main` 命中文件数恰 2。
8. **SCR-A6 位次断言的插入点**：`kidpal_keepalive` 分支须在 `#endif`（`CONFIG_RECEIVE_CUSTOM_MESSAGE` 块尾，:923）之后、`} else { ESP_LOGW("Unknown message type") }` 之前。
9. **SCR-A7 可加位次断言**：`auto_connect_in_flight_` 偏移 < `IsAudioChannelOpened(` 偏移（降频机制做成机判属性）。

### 实现约束与踩坑清单（builder/fixer 必读）

1. **函数定义顺序与相邻函数是硬锚，不能挪、不能在中途插新函数**：RC-A2/RC-B2/RC-B3/OCE-A1/RSL-A2 都用「起始函数签名 → 下一个函数签名」切区（`HandleEndVoiceSessionEvent`→`HandleWakeWordDetectedEvent`、`CloseAudioChannel`→`HandleToggleChatEvent`、`WakeWordInvoke`→`CanEnterSleepMode`、`UpgradeFirmware`→`WakeWordInvoke`、`ResetProtocol`→EOF）。60s producer / 巡检逻辑写在 `Run()` 内，**不要新建夹在四对相邻函数间的 helper**。
2. **与 P2 的行段冲突面**：P2 改 `HandleEndVoiceSessionEvent` 与 `HandleToggleChatEvent` 的 in-flight 守卫行；P1 **不要顺手重写守卫行注释或格式**（如把 :1145-1148 三行合并），否则 P2 重取行号锚时撞无谓 diff。
3. **`mcp_server.cc:1011-1024` 的注释在 P1 后成为代码-注释双源**（P1 后 discard 路径恰恰会发 `listen stop`，靠 discard 帧先行把 routing 打回 `chat` 才不归档）——**P1 不改它**，登记 `crossPackageBehaviors` 交 P2（P2 本就改该文件）。
4. **迟到下行帧**：通道常驻后，`listen stop` 之后引擎尾音 ASR 仍可能下发 stt / sentence_start / tts，`OnIncomingJson` 的 stt 分支（:845-877）会写 chat message、`sentence_start`（:811-843）可能把设备从 Idle 推回 Speaking。manual-stop 问答路径本就如此（`HandleStopListeningEvent` 早已发 stop 不关通道），但四条收尾路径首次获得该行为 → 写进轮 A 观测项（「结束后 1–2s 内屏幕是否被迟到帧拽回思考/播报」），不是 P1 要修的缺陷。
5. **心跳三方并发写同一 device websocket**（上泵、sweep 里的 `_story_finish_session`、心跳 task）；front 现无 asyncio 发送锁。P1 不新建锁（超范围），如实记为 residual——心跳帧极小、单次 await，实际风险低。

### 真机轮 A 修复轮（2026-11-17）技术方案（planner+architect 预评）

> **留痕**：2026-11-17，planner Step 1g 前置技术预评档（architect 只读 advisory，dispatch `a3d0206ffd866ca3a`，见 pod-events `dispatch-completed`）。效力声明：**不豁免任何门、不改验收与 TaskPackage 事实、不构成通过判据**。输入 = `device-round-a-evidence.md`（三源真机证据）+ human 2026-11-17「三个问题都修」（pod-events `human-decision`）。本子节为 planner 只读消费 plan.md 规则的例外写入点（`task-intake-and-slicing.md` 例外条款）。

**D1 · 记录语境注入去边缘化（front）**

- `_consume_kidpal_control` ctx(story) 分支：注入条件 `ctx_push_edge and upstream is not None` → `upstream is not None`（`front_app.py:1211`）；`_push_story_list` 仍守 `ctx_push_edge`。`_record_ctx_mark` 已无条件、零改。
- `control/start_recording` 消费沿：`_story_route_display_payload` 返回语义 `enter_edge` → `consumed`（命中即 True；`_relay_display_blocks_to_device` 同步改语义 + docstring；唯一消费者 = pump 两处 mark/inject）。
- 可观测性：新增审计 token **`story_record_ctx_enter`**，单一 emit 点放 `_inject_record_context_enter`（additive kwargs `session_id/src/edge`），字段 `corr_id=<sid> src=ctx_story|start_recording edge=true|false injected=true|false`。三处调用点共用单点 → 源文本计数断言稳定。
- 正向收益：引擎腿重建（front 重启 / 24h 日切）后无条件注入可补真新腿的语境标志。
- 已知副作用（申报）：每会话 1 + 引导轮数（≤2）帧 active=true（tts stop 出口回 Listening 会重发 ctx），≤3 帧/会话、约 50B/帧；引擎每帧一条 INFO。
- **冻结件重锚（显式，非静默改齿）**：`test_record_dialogue_guide.acceptance.py` RDG-A4（`:935-939`）「重复 ctx(story) 零注入」由绿转红 → 重锚为「第二次仍恰一帧 active=true；story_list 零新增」。按 KRC-E2 口径列「旧钉 → 新钉 → 为什么（human 2026-11-17 指示 + evidence §三）」。

**D2 · 丢弃/会话关闭终止在途回合（front；**不引入 abort 帧**）**

- **决策：不发 abort**。依据（a）轮 A 实证 abort 无效：引擎 22:46:50 有 `Abort message received` / `-end`，仍产 TTS「嗯～」（22:46:53）；（b）引擎侧冻结纪律四处明写「绝不置 `client_abort`（防波及 stop 后 front 注入的合法 speak/story 下发）」（`engine-patch/connection.py:266-267 / 733-735 / 1559 / 1578`），front 主动发 abort = 违反该纪律，风险面正是 S-F1.4 会话间隙提醒出音与整理失败安抚句；（c）human 的「择一」目标是「终止在途回合」，下述机制更硬且零新语义。
- **机制**：`kidpal_story_discard` 分支内、返回前 `await _release_record_context(session_id, [(websocket, upstream)])`——**泵内本连接引擎腿直发**（不用 `_session_legs(sid) or _alert_legs`：多连接时会误打别的腿）。同一协程、同一 upstream、顺序 await ⇒ `active=false` **严格先于**随后被转发的 `listen stop`。引擎收 stop 时语境已假 → `_kidpal_on_listen_message` 置 `_kidpal_listen_closed` + `asr_audio.clear()` + 排空队列（先清后转）→ 尾音 ASR 不成立；万一在途，`chat()` depth==0 首守卫丢弃。`client_listen_mode` 实证 `auto`，条件成立。
- 清标记副作用 = `_story_listen_stop_boundary` 因 `_record_ctx_seen` 已清早退 → 重复 stop 零第二次 `active=false`（幂等由单点承载，不另写计数）。
- **边界**：只落 discard 分支；`_story_stop_on_listen_stop` / `_story_listen_stop_boundary` / KEY 尾音归档路径一字不动（KEY 收尾「stop 时语境仍真」是 pending-stop 归档承重通道，截断即毁稿）。
- **迟到帧丢弃门（B9，front 侧）**：front 在 `_release_record_context` 单点 arm「会话关闭门」，下一次设备上行 `listen start` / ctx(story) disarm。门内丢弃引擎腿→设备腿的**二进制音频帧** + `type in ("tts","llm")` 文本帧（含 text 同步帧）；**不拦** `type=="mcp"`（alert/story_text/story_list）与 `stt`。front 自注 `kidpal_speak` 登记有界「预期出音」令牌（deadline 30s，重复取 max；`tts stop` 清除，且**先判令牌后清令牌** → 令牌在场放行整轮含其 tts stop，防 Speaking 悬挂）。令牌登记点两处：`_speak_to_legs`（引导/安抚）与 `_deliver_alert_now`（提醒 speech；`reminders.py` 引擎腿注入不经 `_speak_to_legs`）。风险量化：提醒「弹窗+提示音」半直发设备腿、不经泵 ⇒ 结构性免疫；最坏 = 提醒语音丢失、必达判据不破（ADR-003 V20 不对称口径）。残余（申报）：迟到回合的 display 块仍可能闪屏 → B8 修好后该回合不应存在，列 residual。

**D3 · 空素材会话自动收尾（设备 producer + front 既有释放链）**

- `application.cc` CLOCK_TICK 区（60s producer 之后、巡检之前）新增 producer：`Listening && kidpal_story_session_ctx_active() && !s_story_rec_entered && !s_vad_speaking && clock_ticks_ >= kStoryEmptySessionTimeoutS` → 新审计 token `story_empty_session_timeout` + `HandleEndVoiceSessionEvent()`。
- **素材信号决策 = `!s_story_rec_entered`（不用 `kidpal_story_echo_count()`）**：echo 计数的唯一 `reset` 在 `mcp_server.cc:861`（KEY 入口），`control/start_recording` 入口（`:1731-1751`）无 reset → 上一会话计数残留、触发词入口静默不修；补 reset 须碰 `mcp_server.cc`（P2 边界、manifest 外）。`s_story_rec_entered` 是 application.cc file-static，VAD 起音沿置位、Idle 分支清位，两入口同覆盖、零跨包依赖。
- **阈值决策 = 60**（`static const int kStoryEmptySessionTimeoutS = 60;`，定义在常量区 `kSessionIdleTimeoutS` 之后）：与 requirements §2.1 单一 60s 口径对齐、不引入第二魔数；空素材会话无引导播报，不存在需保护的「AI 引导中」窗口。A7（连续讲 >90s 不误收）由 `!s_story_rec_entered` + `!s_vad_speaking` 两门承载，与阈值取值无关。
- **源文本区切硬约束**：新常量注释块**不得出现 `kSessionIdleTimeoutS` 字面**、不得定义在其前——SCR-A5 取该字面首现到下一分号，注释写回会让区不含 `60` 而假红。新 producer 不得写 `clock_ticks_ % 10`（DLG-A4⑤ 负锚）。
- front 侧零改动即衔接：`listen stop`（accum 空）→ `mark_pending` → 边界函数 pending protected → 10s sweep `release_pending` + `_release_record_context` → `active=false` 恰一帧；零归档 / 零 story_list / 零 start_organizing / 零盖章。`_story_guide_round` 入口守卫**不动**（[SC-B] 冻结面）；`skipped=no_session` INFO 保留到轮 B 通过（轮 B 三源对账的 front 侧唯一可观测证据）。
- 明确的错误替代（否掉）：只让 front 计轮触发 AC5 自动收尾 → front 释放语境但设备仍 Listening 开麦（front 无下行能让设备结束会话，`end_session` 三段补链归 P2），环境音直进 LLM，比现状更糟。

**边界与治理（本轮）**

- manifest **零扩**：实现落点 `front_app.py` + `application.cc`（已在册）；断言落两个在册验收件。`mcp_server.cc` / `display_schema.py` / `story.py` / `protocol.*` / `story_page.c` / `docs/contracts/**` / 引擎代码维持零 diff。
- **spec 漂移（出 Pod 后由主线程路由，本 Pod 不改 spec）**：requirements §2.1 逐字冻结「记录会话不适用 60s 会话超时」、AC5 只覆盖「有素材 + 2 轮引导无回应」；D3 对空素材记录会话新增设备侧超时 = 对该豁免的**收窄**，spec 本身有洞。`docsSyncRequired=false` 的现值与 D3 实际影响不一致 → review-packet 显式申报，主线程按需求面轻量注记处理（§2.1 记录会话段加对账句或 §7 开放问题 7 追加），不必重开 `/requirement-change` 全档。
- 轮 B 清单口径修正：`device-round-a-evidence.md` B-D3 的「随后普通问答**立即**可应答」与已冻结 accepted residual（pending ≤10s 抑制窗）冲突 → 改为「≥10s 后普通问答可应答；或 10s 内首次话语允许被抑制、再按一次即恢复」。

### 真机轮 B 修复轮（2026-11-18）技术方案（planner+architect 预评）

> **留痕**：2026-11-18，planner Step 1g 前置技术预评档。输入 = 真机轮 B 实测（human 2026-11-18 指示「按 B 修」，pod-events `human-correction`）+ planner 亲拉的设备日志（`device-evidence/round-b/kplog-excerpt.txt`，源 `KPLOG000.TXT` 全量 sha256 `21599cbc…`）。效力声明同前：architect 意见为 advisory 输入，**不豁免任何门、不改验收与 TaskPackage 事实、不构成通过判据**。本子节为 planner 只读消费 plan.md 规则的例外写入点（`task-intake-and-slicing.md` 例外条款）。

**一、轮 B 实测（human 持板）**

- D1 通过、D2 通过、A5 归档通过。
- D3 失败：故事页起会话、全程不说话，07:15:03 listen start → 07:17:43 手动丢弃（2m41s），无 `story_empty_session_timeout`。
- A6 回归失败：普通会话 07:18:23 → 07:21:36（3m13s），无 `session_idle_timeout`；**A6 producer 本轮一字未动**，轮 A 同判据可正常触发（`seconds=60`）。

**二、planner 只读定位（设备日志，新证据）**

1. **D3 的真根因不是电平门**：每次**故事**会话进入聆听后 0.25–0.7 秒内必出一条 `MCP: story_vad_recording_enter consumed=1 page=2`（本日志窗口 07:14:30、07:15:04；`grep` 全量 12 条均落在会话起步 1 秒内）。该行唯一发射点 = `application.cc:436` 的 `KidpalStoryVadEnterRecording()`，前置条件含 `s_vad_speaking == 1` → 即 **VAD 在无人说话时让出一记 speech 沿**。该沿把 `s_story_rec_entered` 置真，而它正是 D3 producer 的「从未收到素材」判据；**置真后该会话对 D3 producer 永久不可达，与 VAD 电平其后是否回落无关**。副作用：记录会话 0.7 秒内从列表页跳到 RECORDING 录音页（`PAGE_SWITCH … sview=2`），孩子一句话没说。
2. **A6 窗内零状态迁移**：07:18:23 `idle -> listening` → 07:21:36 手动 `overlay_close`，中间无 `State:` 行 → `clock_ticks_` 未被 `HandleStateChangedEvent`（`:1314`）清零，计时连续增长；`clock_ticks_` 全仓只有两处写点（`:420` VAD 起音沿归零、`:452` 每 tick +1，`grep` 已核）。故 A6 未触发只能落在两因之一——**M1 电平恒真**（`!s_vad_speaking` 恒假）或 **M2 起音沿周期性清零**（永远到不了 60）。设备当前不打印 VAD 电平，日志无法再细分，这正是 human 要求补低频 listening 诊断行的直接理由。
3. 控制组：07:14:30–07:14:48 是**有素材**的真实记录会话（07:14:38 起出 `>>` 转写；前一次收尾走 `stop_rec archive_pend=1` → `story_sd_write` → `story_saved_stamp seq=32`）——记录语境与归档链正常，缺陷只在「空素材」一支与「普通会话静置」一支。

**三、本轮必须满足的设计约束（planner 定，architect 给方案）**

1. **主判据去电平门化**：收尾判据不得以瞬时 `s_vad_speaking` 电平为必要条件；须以「最后一次**有效**语音活动时间戳」类判据承载，并定义「有效」（能区分短促噪声 / 环境噪声顶住 VAD 与真的在说话）。
2. **A7 语义保住**：连续讲 >90 秒不得被误收——新判据必须在 M1/M2 两种失效下都保住 A7，并给出论证。
3. **M1/M2 都要被覆盖**：电平恒真与起音沿风暴两条路径都必须能走到收尾；若用「VAD 卡死看门狗」，阈值须严格大于 90 秒并给出依据。
4. **D3 复用同一新判据**：记录语境正向门保留；「从未收到素材」判据**须换源**——现判据 `s_story_rec_entered` 已被证明会被起步噪声沿提前置真；新素材信号须与 front 累积器同源（front 的素材 = 引擎 `stt` 文本），避免设备与网关对「有没有素材」判断不一致。D3 口径不变：只释放、不归档。
5. **可观测性（human 必办）**：设备侧低频 listening 诊断行（state / VAD 电平 / 计时分量 / 记录语境标志 / 距上次有效语音秒数），限频且轮转友好；两条 producer 命中审计行保留并可加 `src=` / `last_voice_age_s=` 字段。**须绕开 `diag_log.acceptance.c`（DLG-A4）对 `clock_ticks_ % 10` 恰 0 的负锚**。
6. **可测性**：判据语义须能被 host 机判（优先纯逻辑缝 + 注入 tick 序列的真行为断言），不得整段落回 `application.cc` 只剩源文本锚；真机轮只承载「真实 VAD/能量环境下」的端到端确认。
7. **零改动面**：`docs/contracts/**`、`engine-patch/**`、引擎侧、`mcp_server.cc` 不改（如需扩单，走 planner 扩单通道回评后补 manifest）。

**四、architect 预评回执**

> dispatch `a691226a871b7ede5`（architect，只读 advisory，2026-11-18；pod-events `dispatch-completed` 有记录）。下为 planner 摘录 + 逐条对账；对不上的条目见「planner 裁定」节。

1. **方案 = B+（推荐并采纳）**：判据本体搬进新的纯逻辑 seam `firmware/main/session_voice_watch.{c,h}`（host 可编、可注入 tick 序列做真行为断言），`application.cc` 只留接线。第三路证据 = **引擎 `stt` 到达时刻**（不用输入能量）。否掉能量档的理由：能量地板无任何标定数据（轮 A/B 三源证据里零个电平数），盲选地板会把缺陷换成「掐断正在讲话的孩子」，且要动 AFE/AudioService 五个文件（含抽象基类），风险与收益不成比例；正确顺序 = 先观测后门控。
2. **核心取舍定理（本方案的地基）**：只用「VAD 电平 + 时间」两个量，**不可能**同时满足 A7（连续电平 ≥90s 不收尾）与「电平卡死 ≤70s 收尾」——这两种情形在信号层面逐 tick 同构。要把卡死收尾压到 70s 量级必须引入第三证据并接受其缺失时的降级。
3. **失效模式从 2 种扩到 4 种**：M1 电平卡死；M2 起音沿风暴；**M3 电平闩锁跨会话**（`AfeAudioProcessor::Stop()` 不复位成员 `is_speaking_`，`application.cc` 的 `s_vad_speaking` 也无迁移复位点）；**M4 记录语境镜像卡死**（`kidpal_story_session_ctx_active()` 残留 → A6 被硬豁免、D3 又不满足素材门）。
4. **判据定义（seam 每 tick 一次）**：
   - `level_run_s` = 电平连续为真秒数；`asr_age_s` = 距上次 stt 秒数（饱和 999）；`quiet_s` = 距上次**有效语音证据**秒数（主累加器）。
   - `sustained_level = level_run_s >= 3 && level_run_s <= 100`（**下界拒短促噪声暴发 = M2 解药；上界截断电平可信窗 = M1/M3 解药**）。
   - `asr_fresh = asr_age_s < 25`（语义级证据，把 A7 保护延伸到 >160s 真长讲）。
   - `voice_evidence = sustained_level || asr_fresh`；`quiet_s` 无证据时 +1；`quiet_s >= idle_timeout_s(60)` → 收尾。`suspect = level_run_s > 100`（审计位，非独立触发源——**不引入第二个定时器**）。
5. **阈值与依据**：`KIDPAL_VW_VOICE_SUSTAIN_S=3`（`vad_min_noise_ms=100` + `VAD_MODE_0` ⇒ 噪声电平连续段是几百毫秒量级，3s 留一个数量级余量；短于 3s 的真话语由 `asr_fresh` 兜住，故调大无 A7 风险）；`KIDPAL_VW_ASR_FRESH_S=25`（引擎相邻 stt 间距假设的 1.25~2.5 倍；A7 不硬依赖它）；`KIDPAL_VW_VAD_TRUST_S=100`（**human 硬约束「严格大于 A7 的 90s 界」**，留 10s 余量；100s 内电平恒被采信 ⇒ A7 无条件成立、零云端依赖）；`DIAG_INTERVAL_S=10`；`DIAG_MIN_GAP_S=3`。
6. **逐场景结果**：纯静默 → 恰 60s 收尾（B-D3/A6 验收口径）；M1 电平卡死 → `suspect=1`、`100+60=160s` 收尾（有界）；M2 起音沿风暴 → 尖峰不采信，仍恰 60s 收尾；M3 闩锁 → 由 Listening 入口复位 `s_vad_speaking=false` + `watch reset` 掐掉，残余 ≤160s；A7 连续讲 → 有 stt 时永不收尾，零 stt（云端挂）时 ≤90s 绝不收尾（160s 才收）；M4 → 本轮只由诊断行 `rec_ctx=` 暴露、不修（`mcp_server.cc` 置位单点在 P2 边界与 manifest 外）。
7. **D3 共用形态**：两个 producer 共用同一 `quiet_s`，在 `kidpal_story_session_ctx_active()` 上一正一负严格互斥；`tick()` 每 tick 恰一次且在两 producer 之前；**只释放不归档口径不变**，fire 之后仍走 `HandleEndVoiceSessionEvent()`，front 零 diff。
8. **时钟基准**：继续用 1s tick；但累加器**不复用 `clock_ticks_`**（它有两个归零点，正是 M2 成因本体），改由 seam 自持 `quiet_s/level_run_s/asr_age_s/tick_s`；`clock_ticks_` 降级为诊断字段 `ticks=`，`MAIN_EVENT_VAD_CHANGE` 区因此**零 diff**。时序残差申报：clock timer `skip_unhandled_events=true` + 事件位幂等 ⇒ 计时只会偏慢不会偏快（安全方向）。
9. **可观测性**：token `listening_diag`（`ESP_LOGI`——DEBUG 默认不落盘，本轮不能再吃这个亏），字段十件 `src/state/vad/run/quiet/asr/ticks/rec_ctx/rec_mat/suspect`；触发 = Listening 态每 10s + 跳变（电平/标志翻转、`suspect` 0→1、会话入口）受 3s 最小间隔封顶；非 Listening 态零输出。轮转预算：单行约 190 字节，最坏 ~3.8 KB/分钟，16MB 封顶 ≈ 70 小时 Listening；现实使用（每日约 30 分钟 Listening）约 140 天，**严格低于**被消掉的 `Unknown message type` WARN（24h 常开 ≈144 KB/日）。**不加运行期开关**（诊断行的价值就在真机窗）。
10. **绕开 `clock_ticks_ % 10` 负锚**：分频决策住 seam 纯函数 `kidpal_voice_watch_diag_due()`（模数用 seam 自持 `tick_s`），`application.cc` 物理上不出现该字面；并在验收里再钉一次同字面 == 0 作双保险。
11. **审计行加字段**：两条 producer 行保留 `seconds=`（轮 A 对账链不断）+ 追加 `quiet=` / `asr=` / `suspect=` / `src=idle|empty_story`；**不得改动 `local_close_settle` 既有四 token**（OCE-A1 钉它们）。
12. **既有验收判定**：SCR-A5 只有一条断言假红（`!s_vad_speaking` 电平门在场）→ 极性反转为负锚 + 补 `kidpal_voice_watch_tick(` 正锚；SCR-A8 只一条假红（`!s_vad_speaking >=2`）→ 整条退役（同一事实单一承载点，防 co-red 噪声）；两件头注 `[S5]`/`coverage:` 行同步改写；`device-outcome` 成对申报保留但**范围收窄**（判据本体已可离线机判，只剩接线与主循环运行时）。
13. **新增验收建议**：SCR-A9（静默 59 不 fire / 60 fire，真行为）、SCR-A10（M1 400 tick 卡死 → 恰 160 fire + `suspect=1`，**kill-shot**）、SCR-A11（M2 风暴 vs 4s 暴发成对，真行为）、SCR-A12（A7 有 stt 200 tick 零 fire + 零 stt 150 tick 零 fire，真行为）、SCR-A13（reset 跨会话零残留，真行为）、SCR-A14（诊断限频决策表 + 结构半，真行为+结构）、SCR-A15（application.cc 接线五件，结构）。变异清单 11 条，其中 #2（删可信窗上界）是轮 B 缺陷的唯一离线警报、#6（`VAD_TRUST_S` 100→30）是 A7 承重、#10（电平门回潮负锚）是回归防线。
14. **manifest 扩单申请 3 件**：`firmware/main/session_voice_watch.c`（新）、`.h`（新）、`firmware/main/CMakeLists.txt`（additive 一行）。**已在册要改**：`application.cc`、`session_channel_resident.acceptance.c`、`firmware/main/test/host_app/main/CMakeLists.txt`（新 seam 源 + 注册序必须在 smoke 之前）；`application.h` 保持零 diff（用 file-static 而非成员）。

**五、planner 裁定（对回执的采纳、修订与新增）**

1. **采纳 B+ 与全部阈值（保守档）**。`VAD_TRUST_S` 取固定 100，不取「ASR 陈旧时收缩到 15」的激进档：后者把「麦克风常开」的运维缺陷换成「ASR 中断时把孩子话头掐断」的产品缺陷，而 A7 是本包已冻结、riskTier=high 的承重面。轮 C 的 `asr=` 字段就是把这条决策变成可量测数据的装置；若实测 160s 不可忍，再做快速后续批。
2. **采纳四条失效模式扩表（M1–M4）**，含 M3 卫生复位（Listening 入口 `s_vad_speaking=false` + `watch reset`）。M3 的**根源**（`AfeAudioProcessor::Stop()` 不复位 `is_speaking_`）本轮不修：新判据对 M3 已鲁棒，而改它要连带决定「是否补发一次 offset 回调」（影响 `voice_detected_` → LED），是独立的需自验语义变更；列入 review-packet residual + 主线程路由（Pod 零写 `docs/debt/**`）。
3. **采纳 M4「只诊断不修」的判断**：`mcp_server.cc` 置位单点在 P2 边界与 manifest 外；本轮由 `rec_ctx=` 字段使其可归因，修复归 P2/另立批。review-packet 显式申报。
4. **采纳「不加运行期开关」**。诊断行是纯设备侧、无跨端配对语义，回滚固件即消失；加闸会在最需要它的真机窗被关掉。
5. **采纳 manifest 扩单 3 件**（planner 扩单通道，见 taskpackage `notes` 与 `changeManifest[]` 回填）。`sdkconfig` 不入（不设编译期闸）。
6. **【planner 修订 · 承重】D3 的「从未收到素材」判据必须换源**——architect 回执写「保留 `!s_story_rec_entered`」，与 planner 本轮亲拉的设备日志**直接冲突**，不予采纳：
   - 证据：`device-evidence/round-b/kplog-excerpt.txt`。本日志 12 条 `story_vad_recording_enter` 全部落在故事会话起步后 1 秒内（本文件窗口 07:14:30 / 07:15:04），发射前置条件含 `s_vad_speaking == 1`——即**无人说话时 VAD 也会让出一记 speech 沿**，它把 `s_story_rec_entered` 置真；而 `s_story_rec_entered` 正是 D3 producer 的素材判据。**沿用旧信号 = D3 在真机上依旧结构性不可达**，修复等于白做。
   - 修订：新增 `asr_seen` 到 seam 状态（`kidpal_voice_watch_note_asr()` 置 1，`reset()` 清 0），D3 producer 的素材门改为 `!s_voice_watch.asr_seen`。语义 = 「本会话引擎真的转写出一句话」——与 front 累积器的素材来源**同源**（front 的 accum 也只吃 `stt` 文本），设备与网关对「有没有素材」不再可能判断不一致；ASR 失败时两侧同时判无素材、同时释放不归档，口径自洽。
   - `s_story_rec_entered` 保留原用途（录音页切换 + 幂等），**不进 producer 判据**；起步噪声沿导致的无端跳录音页登记为 residual（不阻塞，D3 收尾后回列表），根治归 `AfeAudioProcessor` 卫生复位后续批。
   - 对应验收改动：SCR-A8 原 `!s_story_rec_entered` ≥1 tooth 重锚为 `!s_voice_watch.asr_seen`（或 seam 访问器形态）≥1；SCR-A15 接线五件增第 ⑥ 件（stt lambda 内 `kidpal_voice_watch_note_asr(` 恰 1，与 architect 的 ② 合并为同一行断言，但语义上另加一条「素材门读 asr_seen 而非 s_story_rec_entered」负锚：CLOCK_TICK 区 `!s_story_rec_entered` == 0）。
7. **【planner 追加】验收条目合并纪律**：architect 建议 20→约 27 条；本 Pod 采纳但要求 SCR-A13/A14 各以单 case 多断言承载，SCR-A15 合并为一条多断言 case，避免 case 数膨胀。**不可降级为源文本的是 SCR-A10 与 SCR-A12**。
8. **【planner 追加】轮 C 清单增补**（写进 review-packet）：C-1 审计行 `suspect=`/`quiet=` 必采（判本轮是否命中真实失效模式）；C-2 A7 窗内 `asr=` 列最大值（`ASR_FRESH_S=25` 的标定）；C-3 home 会话 `rec_ctx=` 恒 0（M4 排查）；C-4 静置窗 `run=` 是否反复归零（M2 排查）；C-5 日志落盘增量与预算对账；C-6 **D3 窗内 `rec_ctx=1` 且 `asr_seen=0` 且 `quiet` 到 60 即收尾**（新素材信号的真机承载）。

**六、planner 白盒复核补丁（2026-11-18，builder 首版之后）**

builder 首版把 `kidpal_voice_watch_reset(&s_voice_watch)` **无条件**落在 `case kDeviceStateListening:` 尾部（每次进入聆听全清七字段）。planner 复核发现的高风险回归：

- 记录会话里**引导播报**（卡壳轻引导，`kidpal_speak` TTS）会让设备 `Listening → Speaking → Listening`。回到聆听时那次全清会把 `asr_seen`（D3 空素材 producer 的**唯一素材门**）清 0。
- 后果：一个**有素材**、孩子已讲过话的故事会话，在引导播报后静置 60 秒会被 D3 判成「空素材」并释放——front 侧该 `listen stop` 走 `pending_stop` 释放、**不归档**（front `kidpal_story_discard` / pending-stop 释放链的口径），孩子已讲内容丢失。
- 修法（seam 与接线各一处，均为 additive）：① 缝新增 `kidpal_voice_watch_reset_soft()`——清 `quiet_s / level_run_s / asr_age_s / tick_s / diag_last_s / suspect` 六件、**保留 `asr_seen`**；② 状态迁移监听器把 `old_state` 存进 file-static，`Listening` 入口按迁移来源分派：旧状态 == `kDeviceStateSpeaking` 走软复位，否则走全清。
- 验收收紧（acceptance-author 补丁）：SCR-A13 追加「`reset_soft` 后 `asr_seen` 仍为 1、其余六件全 0；其后 `reset()` 仍七字段全清」；SCR-A15 追加 Listening 区三件（`reset_soft(&s_voice_watch)` ≥1、`reset(&s_voice_watch)` ≥1、`kDeviceStateSpeaking` ≥1）。红基线 320/5（仅 SCR-A13 / SCR-A15 两条设计态红）。
- 教训（登记 pod-retro）：**「会话入口复位」这类卫生动作必须区分「新会话」与「会话内状态往返」**，否则会把有状态价值的会话级证据当成残留清掉；判据改造轮尤其要逐条追问「这个标志在什么时机被清、清了会不会丢素材」。

# demo 证据形态（与 roadmap 一致）

- host runner + server pytest：SCR-A1–A7 与 SCRF-B1–B6（含 B2b/B4b）的 RED→GREEN 和变异证据；四个收尾点负锚 / 五个真拆通道正锚 / discard→stop 有序不归档为承重齿。
- device build：本轮改动的编译面（`application.cc` / `application.h` / 验收件）通过；`protocol.cc` 若经扩通道纳入则一并构建。
- 一轮真机轮 A（human 持板，front 由 agent 部署）：A1–A13 全绿（A8 录音态提醒排队例外与 A9 查字窗关闭路径为 package-split 明列回归项；A11–A13 为追加项），三源日志（设备 KPLOG / front systemd / 引擎 docker）互证；证据落本包 `current/`。
# 验收方向（acceptance-author 细化，非冻结）

> 本节是拆包期的**断言方向与承重判据**，不是冻结验收；`acceptance` / `acceptanceTestsFile` / `redBaselineResult` 在 taskpackage 中保持空骨架，由 Pod 内 acceptance-author 唯一产出并冻结。

- SCR-A1: 四收尾点收口（六触发源之四，余二 = SCR-A5 的 60s 与 P2 SEI-A1 的云端 end_session）——四个收尾点函数区（Application::CloseAudioChannel / HandleToggleChatEvent Listening 分支 / HandleEndVoiceSessionEvent / WakeWordInvoke Listening 分支）负锚 = 区内 `CloseAudioChannel(` 命中 0、`ScheduleResidentRebuild(` 命中 0、`SetPowerSaveLevel(PowerSaveLevel::LOW_POWER)` 命中 0；正锚 = 区内 `EndVoiceSession(` 或 `MAIN_EVENT_END_VOICE_SESSION` 命中 ≥ 1。回引 PROPOSAL T2 改法 1、ADR-019 ②。设计态红 = 四条收尾路径仍各自 CloseAudioChannel + LOW_POWER + ScheduleResidentRebuild。
- SCR-A2: 统一处理器动作序——HandleEndVoiceSessionEvent 区内 `SendStopListening()` 恰 1、`ResetDecoder()` 恰 1 且在 Speaking 分支内；三态门（Connecting/Listening/Speaking）、`auto_connect_in_flight_` 守卫、`!protocol_` 早退三件逐字在世（261024 语义零偏移；其中 `auto_connect_in_flight_` 守卫一项由 P2 SEI-A4 重锚，见跨包事实节）。回引 PROPOSAL T2、261024 overlay_close 时序自证。
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
