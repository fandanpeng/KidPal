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
