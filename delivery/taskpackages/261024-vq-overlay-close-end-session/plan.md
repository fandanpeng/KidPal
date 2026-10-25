# 261024-vq-overlay-close-end-session · plan.md（full · riskTier high · 技术语义权威）

TaskPackage: `delivery/taskpackages/261024-vq-overlay-close-end-session/`
来源: regression（2026-10-24 真机事故，设备 KPLOG011；debt 条目 `docs/debt/tech/TECH-261024-overlay-close-speaking-no-session-end.md`）
档位: ②单包 Full（architect `/triage-change` 比例化 review 裁决，human 触点① 确认）；`sourceType: regression`
需求锚点: `docs/requirements.md:147` §2.3「问答 · 播报中」行
本 plan 为 architect reviewed plan 原样落盘，planner 只读消费、改需留痕。

## ① 改动意图（一句）

播报 / 连接态下，覆盖层关层手势（BOOT 短按、KEY 长按）必须真正结束语音会话（断 WS → Idle → 省电 → 常驻重建），而不是只打断停播、把状态推回聆听——消除「回首页后麦克风仍开、4 分钟后仍在首页出声问答」的真机事故，实现在冻结需求 §2.3「问答 · 播报中 / 聆听 · 思考」行的 BOOT / 长按语义。

## ② 目标符号 / 文件锚（changeManifest 种子）

实现面（builder）：

- `firmware/main/application.h`
  - 新增 `#define MAIN_EVENT_END_VOICE_SESSION (1 << 13)`（bit 13 现空闲，现有定义只到 bit 12）
  - 公开 `void EndVoiceSession();`
  - 私有 `void HandleEndVoiceSessionEvent();`
- `firmware/main/application.cc`
  - `Run()` 的 `ALL_EVENTS` 掩码加入新位（约 `:322-335`）——漏加会导致该位不能唤醒事件组、被延迟处理
  - 主循环新分支置于 `MAIN_EVENT_TOGGLE_CHAT` 之后、`MAIN_EVENT_SCHEDULE` 之前（约 `:361-363` 与 `:407-414` 之间）
  - `Application::EndVoiceSession()` 定义（近 `:939-948`）
  - `HandleEndVoiceSessionEvent()` 定义，落 `HandleStopListeningEvent()` 与 `HandleWakeWordDetectedEvent()` 之间（约 `:1093-1094` 之间）——此位置由既有验收切区锚决定，勿挪
- `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`
  - `GLOBAL_ACT_OVERLAY_CLOSE`（`:148-169`）会话级联改 `app.EndVoiceSession()`
  - 重写 `:149-160` 注释（现注释写「复用 ToggleChatState / listening 态 = CloseAudioChannel」，改后即谎）

验收面（acceptance-author，builder 只读）：

- `firmware/main/test/voice_round_ui.acceptance.c`（KVR-A4 tooth 2 重钉 + 新增结构断言）
- `firmware/main/test/boot_auto_connect.acceptance.c`（RC-A2 tooth 6 计数 4→5；RC-B2 sites 增第四项）
- `firmware/main/test/host_app/main/CMakeLists.txt`（仅当新增验收文件时）

## ③ 预期行为差（before / after）

- **播报中 BOOT 短按**：before = `AbortSpeaking` → 服务端 tts stop → 设备回 Listening → 通道保持 → 静默开麦，实际可继续问答；after = 即时静音（`ResetDecoder`）+ abort 帧 + 关 WS（本地关分类、不触发自动重连）+ Idle + LOW_POWER + `local_close_settle(src=overlay_close)` + T 秒常驻重建；首页不再收音。
- **聆听态 BOOT 短按**：before = ToggleChat Listening 分支关通道回 Idle；after = 同一语义经新入口（唯一可观测差 = settle 日志 `src` 由 `toggle_chat` 变 `overlay_close`，属诊断口径变更，不是行为变更）。
- **Connecting 态 BOOT 短按**：before = no-op（会话继续推进 / 静默持通道）；after = 关通道回 Idle。
- **播报中 KEY 短按**：不变（仍打断 → 聆听，走 SESSION_TOGGLE / ToggleChat，需求冻结语义）。
- **非会话覆盖层（提醒弹窗 / 查字窗）**：不变（`front_is_voice` 守卫 + 顶层裁决消费先行）。

## ④ 目标区域既有覆盖态申报

- **有**：路由表三层列钉值（`key_layer_dispatch.acceptance.c` TEST 1）、OVERLAY_CLOSE 消费链与 dismiss 映射（同件 TEST 2）。
- **必红待重钉**：
  - `voice_round_ui.acceptance.c:1129-1135`（KVR-A4 tooth 2）——板级 `app.ToggleChatState()` 断言 ≥2，改后 = 1。
  - `boot_auto_connect.acceptance.c:1258-1264`（RC-A2 tooth 6）——全文件 `ScheduleResidentRebuild()` 恰 4，改后 = 5。
  - `boot_auto_connect.acceptance.c:1388-1457`（RC-B2 tooth 4）——三收尾点各恰 1，须追加第四个 site 并保持各 1。
- **无**：会话收尾时序（断连 / 静音 / 不收音）的 host 行为覆盖——`Application` / `Protocol` / `AudioService` 不进 host 构建。**申报：无行为断言，真机目检兜底。**

## ⑤ 爆炸半径预估

- 符号：`keya_dispatch_global_action` inbound 1；新符号 inbound ≤1；`ToggleChatState`（inbound 102）、`CloseAudioChannel`、`device_state_machine.cc`、协议层均不改。
- 文件：3 个实现面文件（+2 个验收文件由 acceptance-author 改）。
- 设备面：`OVERLAY_CLOSE` 仅 `waveshare-s3-rlcd-4.2` 一块板消费（全仓 grep 唯一），其余 100+ 板零触面。
- 风险点：abort / 关通道顺序（同 TCP 先例）、auto-connect 竞态（沿用 in-flight 守卫）、`SetDeviceState` → 事件位异步（Idle 分支晚一轮跑）、常驻重建计数断言、tts stop 晚到需自愈。

## ⑥ 实现约束（负锚，builder 必读）

- 不改 `HandleToggleChatEvent` Speaking 分支（打断语义，需求冻结）。
- 不改 `device_state_machine.cc` 转移表；不改协议层 / 契约面。
- 不发 `SendStopListening`（会改写录音收尾归档语义）。
- 不把 `SetDeviceState` 收尾块抽 helper（沿用「收尾落调用点」既有纪律）。
- 不新增 `aborted_` 复位（该标志全仓无读者）。
- 新分支必须位于 `MAIN_EVENT_SCHEDULE` 之前（关层落定先于 `voice_end`）。
- 新事件位必须同时改宏定义 / `ALL_EVENTS` 掩码 / 主循环分支三处。

## ⑦ 修法形态与动作序列（architect 裁决）

### 采用方案

新事件位 + `Application::EndVoiceSession()` 统一结束入口。板级 `GLOBAL_ACT_OVERLAY_CLOSE` 的会话级联改为 `if (front_is_voice()) app.EndVoiceSession();` + `app.Schedule(dismiss_front_overlay)`；状态门（Connecting / Listening / Speaking）下沉到处理器单源裁决（板级只保留「前台是不是 VOICE 覆盖层」这个路由策略）。

### 被否掉的替代方案

- **板级直接同步调 `Application::CloseAudioChannel()`**：按钮回调不在主任务上下文；且该方法日志 src 固定 `discard_record`（录音丢弃面），Speaking 下还需要 abort + 本地解码队列清空，语义对不上。
- **给 `ToggleChatState` 加参数**：事件位无载荷；且其 Speaking 分支 = 打断，是需求冻结语义并被 `voice_round_ui.acceptance.c:1137-1143` 钉住；改它的语义会波及 102 个 inbound 板级调用点。
- **复用 `HandleToggleChatEvent` 里加分支**：KEY 单击（SESSION_TOGGLE）与 BOOT 短按 / KEY 长按（OVERLAY_CLOSE）必须是两个语义入口，合流会把「播报中 KEY 短按 = 打断」改坏。

### 播报态收尾动作序列（单处理器内、主任务上下文、同步顺序）

1. `if (!protocol_) return;`（对齐 ToggleChat 前置）。
2. `if (auto_connect_in_flight_) { log; return; }`（对齐 K6 人工路径守卫，避免与后台 AutoConnect 抢 `websocket_`）。
3. `state = GetDeviceState(); if (state 不在 {Connecting, Listening, Speaking}) return;`。
4. **仅 Speaking 时**：`audio_service_.ResetDecoder()` 先做即时静音。这是本方案唯一的隐性坑：Idle 分支只调 `EnableVoiceProcessing(false)`，它只 `Stop()` 音频处理器、不 `ResetDecoder()`；旧播放队列入 Idle 后仍会继续出声。现有「打断 → 聆听」之所以能停，是因为进 Listening 时 `EnableVoiceProcessing(true)` 内会 `ResetDecoder()`；结束会话没有这一步，必须显式补。
5. **仍仅 Speaking 时**：`AbortSpeaking(kAbortReasonNone)`——先把 abort 帧发出去，让服务端停 TTS（与 `SendKidpalStoryDiscard` 的「先发帧后关通道、同一 TCP 有序」先例同构）。
6. `protocol_->CloseAudioChannel()`——本地关分类置位，`OnDisconnected` 消费标记后不触发 A 路径（不排自动重连、不异步再改 Idle）。
7. **不等 abort ack**。abort 是单向消息、协议层无 ack 语义；断连本身即服务端侧的会话终止信号。
8. `SetDeviceState(kDeviceStateIdle)`（三态 → Idle 全合法）。
9. `Board::GetInstance().SetPowerSaveLevel(PowerSaveLevel::LOW_POWER)` + `local_close_settle` 日志（**src 用新值 `overlay_close`**，与 `discard_record` / `toggle_chat` / `wake_invoke` 并列）+ `ScheduleResidentRebuild()`。
10. **不发 `SendStopListening`**：那是录音收尾保存通道，发了会改变 front 的归档语义。
11. I2S / 音频输出不需要额外关闭：`ResetDecoder` 清空解码 / 播放队列后，输出任务自然空转；Idle 分支停麦、重开唤醒词检测。

**`aborted_` 复位：不需要，也不要加**——全仓 `aborted_` 只有写没有读，下一次 `tts start` 帧会复位它。

**时序自证**：本处理器在 SCHEDULE 之前跑 → 置 Idle 只置 `MAIN_EVENT_STATE_CHANGED` 位 → 同轮 SCHEDULE 先执行 `dismiss_front_overlay`（VOICE 出栈，回原页落定）→ 下一轮 STATE_CHANGED 跑 Idle 分支，`kidpal_render_node_voice_end()` 见 VOICE 已不在栈上而零副作用。若服务端 `tts stop` 帧在本处理器之后到，其出口判据 `GetDeviceState() == Speaking` 已为假 → no-op，不会把状态推回 Listening。这条竞态自愈性应在注释里写死。

## ⑧ 最小断言形态（host 侧）

1. **重钉 KVR-A4 tooth 2**：板级 OVERLAY_CLOSE 切片内含 `app.EndVoiceSession()`，且 `app.ToggleChatState()` ≥ 1（SESSION_TOGGLE 仍在世）；意图描述改为「两入口分工：短按 toggle / 关层结束」。
2. **重钉 RC-A2 tooth 6**：全文件 `ScheduleResidentRebuild()` 恰 5，措辞改「定义 + 四调用点」；RC-B2 sites 数组加第四项（`void Application::HandleEndVoiceSessionEvent()` → 下一函数锚），tooth 4 对四项各恰 1。
3. **新增一条结构断言**（放 `voice_round_ui.acceptance.c`）：`HandleEndVoiceSessionEvent` 切区内含 `audio_service_.ResetDecoder()`、`AbortSpeaking(kAbortReasonNone)`、`protocol_->CloseAudioChannel()`、`SetDeviceState(kDeviceStateIdle)`、`src=overlay_close`、`ScheduleResidentRebuild()`；且板级 OVERLAY_CLOSE 切区内**零** `app.ToggleChatState()`。
4. **不做**纯逻辑缝抽取（决策只是三态映射，host 行为断言价值低、徒增模块）；结构断言 + 真机目检是这个设备缝的诚实验收形态。
5. **真机目检（必须，写进 acceptance）**：
   - ① 播报中 BOOT 短按 → 日志序 `Abort speaking` → `local_close_settle ... src=overlay_close` → `Websocket disconnected` → `State: speaking -> idle`，页面回原页，此后 ≥4 分钟无 ASR / 无出声；
   - ② 播报中 KEY 长按同效；
   - ③ 播报中 KEY 短按仍 = 打断 → 聆听（不得结束会话）；
   - ④ 聆听态 BOOT 短按行为不变；
   - ⑤ Connecting 态关层可回 Idle（必要时用弱网复现）；
   - ⑥ T 秒后常驻重建照常发生。
6. **回归面**：`key_layer_dispatch` / `voice_round_ui` / `boot_auto_connect` / `record_session_live`（录音收尾负锚不得被误伤）四件套全绿；设备侧 `firmware-build` 相位全跑（快路径不适用，理由：`Application` / `Protocol` / `AudioService` 不进 host 构建）。

## ⑨ out-of-scope 观察（另案，不纳入本包）

路由表覆盖层列 / 查字窗列 / 基层列的 `BOOT DOUBLE_CLICK` 全部返回 `GLOBAL_ACT_NONE`（`key_gesture_parser.cc:130, 158-159, 179`），板级 `OnDoubleClick` 除故事页外无接线（`waveshare-s3-rlcd-4.2.cc:733-739`）；而 `requirements.md:138-149` 定义 BOOT 双击 = 同短按（覆盖层 / 提醒 / 查字窗 / 播报中 / 聆听中均含）。这是路由面漂移，另案立包；与本包无冲突——将来修好后 BOOT 双击在 VOICE 覆盖层会自然汇入同一个 `EndVoiceSession()` 入口。

## ⑩ 事故证据附录（KPLOG011 真机）

| 时间 | 日志 | 含义 |
|---|---|---|
| 18:37:56 | `StateMachine: State: listening -> speaking` | 会话正常进入播报 |
| 18:38:00 | `key=1(boot) gesture=1(click)` → `key_route_dispatch action=4(overlay_close)` | 用户按 BOOT 关语音浮层，意图回首页 |
| 18:38:00 | `Application: Abort speaking` | 只执行了「打断停播」 |
| 18:38:00 | `PAGE_SWITCH from=home to=home` / `render page HOME (P1)` / `dismiss_front_overlay` | 页面确实回了首页 |
| 18:38:00 | `StateMachine: State: speaking -> listening` | 会话未结束，退回聆听，麦克风继续上传 |
| 18:42:13 | `Application: >> 那温差有多大呢？`（`voice_look scene=1 top=2`） | 首页状态下仍在收音并识别到人说话 |
| 18:42:17 | `listening -> speaking`，`AudioCodec output enable=true`，`chrome_mid_sync front=0 voice_live=0` | 直接出声回答，前台仍是首页 |
| 18:42:30 | `key=0 click → session_toggle`，`listening -> idle`，`local_close_settle` | 用户再按 KEY 才真正收尾 |

对照：同日 8 次 `action=4(overlay_close)`，其余 7 次在聆听态全部正常走到 `listening -> idle` + WS 断开 + `local_close_settle`。
