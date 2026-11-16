# dispatch-progress · builder · 261116-channel-resident-core

## 已完成

### 1) 新 host 验收件登记 + 首跑
- `firmware/main/test/host_app/main/CMakeLists.txt`：`HOST_ACCEPTANCE_SRCS` 内、
  `"../../host_infra_smoke.acceptance.c"` 之前追加
  `"../../session_channel_resident.acceptance.c"`（附登记注释）。build 通过。
- 首跑（执行目录 `firmware/main/test/host_app`）：**306 Tests / 5 Failures / 0 Ignored**
  = 3 上游已知红（p1_auxbar AB-A1/A2/A4）+ 2 本包设计态红（boot_auto_connect RC-A2
  tooth6 `ScheduleResidentRebuild` 5→0、voice_round_ui OCE-A1 `SendStopListening` 缺失）。
  与 acceptance-author 冻结的红基线名单一致（新增件的 SCR-A1..A7 在登记后的首跑里
  全部按设计态红呈现）。

### 2) 固件实现（`firmware/main/application.cc` / `application.h`）
- T1 收尾点收口（四区）：
  - `Application::CloseAudioChannel()`（历史名，语义 = 会话收尾）：删 `protocol_->CloseAudioChannel()`
    / `SetDeviceState(Idle)` / `SetPowerSaveLevel(LOW_POWER)` / `ScheduleResidentRebuild()`，
    改为「`src=discard_record` 审计行 + `EndVoiceSession()` 事件位投递」（协议接收任务
    上下文 → 不直调处理器）。
  - `HandleToggleChatEvent` Listening 分支：删五件套，改 `src=toggle_chat` 审计行 + 直调
    `HandleEndVoiceSessionEvent()`（主循环上下文，保同步语义）。
  - `WakeWordInvoke` Listening 分支（Schedule lambda 内）：同上，`src=wake_invoke`。
  - `HandleEndVoiceSessionEvent`：`protocol_->CloseAudioChannel()` → `protocol_->SendStopListening()`；
    删 `SetPowerSaveLevel(LOW_POWER)` 与 `ScheduleResidentRebuild()`；`src=overlay_close`
    审计行与三态门 / in-flight 守卫 / `!protocol_` 早退 / Speaking 分支 ResetDecoder+Abort
    逐字保留（不重写守卫行，守 P2 行段冲突面）。
- 删除 `ScheduleResidentRebuild()` 定义、`kResidentRebuildDelayS` 常量、头文件声明与
  同步注释（RC-A2 tooth6 回潮守卫 = 0）。
- T2 60s 无语音超时 producer：新增 `static const int kSessionIdleTimeoutS = 60;`（接替
  被删常量位置）；主循环 CLOCK_TICK 区加四判据（聆听态 + `!s_vad_speaking` 电平门 +
  `clock_ticks_ >= kSessionIdleTimeoutS` + 记录语境豁免）→ 直调 `HandleEndVoiceSessionEvent()`
  + `session_idle_timeout` 审计行；VAD_CHANGE 区 Listening 分支加 `if (s_vad_speaking)
  clock_ticks_ = 0;`。未接纯模型事件（VQ-E1 = 2 文件保持）。
- T4 自愈巡检：同 CLOCK_TICK 区，判据 `Idle && protocol_ && !auto_connect_in_flight_ &&
  !IsAudioChannelOpened()` → `TryStartAutoConnect()`；守卫短路序照写（in-flight 先于
  通道判据）；区内零直连建连调用、不新建 task；未使用 `clock_ticks_ % 10`（DLG-A4 保绿）。
- T3 心跳 no-op：`OnIncomingJson` 的 `#endif` 之后、`Unknown message type` WARN 之前
  新增 `kidpal_keepalive` 分支（直接 return）。

### 3) front 实现（`server/gateway/front/front_app.py`）
- T5 每连接心跳：`CHANNEL_HEARTBEAT_ENV` / `CHANNEL_HEARTBEAT_INTERVAL_ENV` /
  `channel_heartbeat_enabled()` / `CHANNEL_HEARTBEAT_INTERVAL_S` / `_channel_heartbeat_loop()`
  全部定义在 `ws_gateway` 之前（靠 `story_restore_enabled` 一带，未插进三处腿登记/课表
  推送之间）；`ws_gateway` 内在 `_push_schedule_connect` 之后、`asyncio.wait(FIRST_COMPLETED)`
  之前创建 task；循环 sleep-first + 裸 `send_text` + `except Exception` 静默退出；
  `finally` 首行 `cancel()` + `gather(return_exceptions=True)`；未进 FIRST_COMPLETED 集合。
- T6 `listen stop` 分层收尾：`pump_client_to_upstream` 在 `_story_stop_on_listen_stop`
  消费点之后、`type/state == listen/stop` 门控内调 `_story_listen_stop_boundary`；
  记录语境标记假 → 零动作；pending_stop / accumulate → 保护不动作；chat + 标记真 → 只释放；
  审计行含 `session_end_boundary`。
- T6 标记：`_RECORD_CTX_SEEN`（WeakKeyDictionary + 属性回退）+ `_record_ctx_mark/seen_get/clear`；
  置位点 = ctx(story) 沿 + `_inject_record_context_enter` 的两个调用点；清位单点 =
  `_release_record_context` 体内（另在 `ws_gateway` finally 兜底清一次）。
- T7 pending 超时补释放：`_story_timeout_sweep_body` 的 `release_pending` 成功分支追加
  `await _release_record_context(sid, _session_legs(sid) or list(_alert_legs))`。
- T8 ctx 边沿分态：新增 `_story_ctx_edge_fallback`（pending_stop → 不动作；accumulate →
  `cancel(reason="session_end_stale")` + 释放；chat + 标记真 → 只释放）；接入
  `kidpal_session_ctx` 的 points 两值分支与 home/schedule/story_cover 合法非 story 分支；
  未知 source_page 的 `elif` 逐字保留（拒绝 + warning + 不释放）。
- T9 连接级 teardown 三件逐字保留。

### 4) 两面自测
- host（执行目录 `firmware/main/test/host_app`）：**306 Tests / 4 Failures / 0 Ignored**。
  4 红 = 3 上游已知红（p1_auxbar AB-A1/A2/A4）+ **1 项跨文件冻结齿冲突**（见「阻塞项」）。
  本包 SCR-A1..A7 与四件重锚件（boot_auto_connect / voice_round_ui / record_session_live /
  m4k3）全绿；五真拆通道、Idle 八件链、VQ-E1（2 文件）、DLG-A4、RSR-A2/A3、CG-A1、
  RM-A3、RSL2-A2 等 must-stay-green 全绿。
- server（仓根）：**473 passed / 0 failed / 9 deselected**（命令
  `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance`）。

### 5) mutation evidence

变异方法：源文件先备份到仓外临时副本（`cp`），逐条用精确字符串替换把实现改坏，跑对应
冻结验收取 FAIL 行，再以副本还原并用 sha1 校验还原一致。host 全量 runner 无筛选参数，
每条变异跑一次全量（约 44s）；server 用 `-k` 定点跑。

基线（未变异）= host 306 Tests / 4 Failures（3 上游 p1_auxbar + RCL-F1 跨文件冲突）；
server 全绿。

| # | acceptance | 变异动作（实现侧） | 命令 | 转红证据（FAIL 行摘要） | 还原 |
|---|---|---|---|---|---|
| 1 | SCR-A1 | `CloseAudioChannel()` helper 体内插回 `if (protocol_) { protocol_->CloseAudioChannel(); }` | host 全量 | `session_channel_resident.acceptance.c:316 ... (SCR-A1):FAIL:Function [session_channel_resident][SCR-A1][structural]`（runner 306/5，基线 4 → +1，无其它 co-red） | sha1 一致；复跑回 306/4 |
| 2 | SCR-A2 | handler Speaking 分支内复制一行 `audio_service_.ResetDecoder();`（区计数 1→2） | host 全量 | `:385 ... (SCR-A2):FAIL`（306/5，无 co-red） | sha1 一致 |
| 3 | SCR-A3 | `Reboot()` 的 `protocol_->CloseAudioChannel();` → `protocol_->SendStopListening();` | host 全量 | `:451 ... (SCR-A3):FAIL`（306/5） | sha1 一致 |
| 4 | SCR-A4 | `kidpal_keybar_scene_reset();` → `kidpal_keybar_scene_restore();` | host 全量 | `:484 ... (SCR-A4):FAIL`；co-red：`record_session_lifecycle.acceptance.c:727 (RSL2-A2)`、`chrome_ghost.acceptance.c:1074 (CG-A1)`（三齿同钉该符号，预期同向） | sha1 一致 |
| 5 | SCR-A5 | `static const int kSessionIdleTimeoutS = 60;` → `= 30;` | host 全量 | `:521 ... (SCR-A5):FAIL`（306/5） | sha1 一致 |
| 6 | SCR-A6 | `else if (strcmp(type->valuestring, "kidpal_keepalive") == 0)` → `else if (false)` | host 全量 | `:574 ... (SCR-A6):FAIL`（306/5） | sha1 一致 |
| 7 | SCR-A7 | 巡检判据两操作数互换（`IsAudioChannelOpened` 前置于 `auto_connect_in_flight_`） | host 全量 | `:626 ... (SCR-A7):FAIL`（306/5，standalone 复跑确认） | sha1 一致 |
| 8 | SCRF-B1 | `channel_heartbeat_enabled` 极性反转（`not in` → `in`） | `pytest ... -k test_scr_b1_heartbeat_gate_default_on_and_rollback_off` | `AssertionError: True is not false : SCRF-B1 RED: channel_heartbeat_enabled('0') 未关 —— 回滚开关识别集不全`（1 failed / 14 deselected） | sha1 一致 |
| 9 | SCRF-B2 | `_story_listen_stop_boundary` 的 pending_stop 保护分支改成 `acc.cancel(reason="session_end_stale")` + 释放 | `-k test_scr_b2_pending_protected_and_repeated_stop_idempotent` | `AssertionError: 'chat' != 'pending_stop' ... SCRF-B2 RED: accum 空 stop 后未落 pending_stop`（1 failed） | sha1 一致 |
| 10 | SCRF-B2b | sweep 的 `await _release_record_context(...)` → `pass` | `-k test_scr_b2b_sweep_releases_at_ten_seconds` | `Lists differ: [] != ['sess-b2b'] ... SCRF-B2b RED: +10s sweep 未释放记录语境`（1 failed） | sha1 一致 |
| 11 | SCRF-B3 | discard 分支 `cancel(reason="discard")` → `dropped = False` | `-k test_scr_b3_discard_then_stop_no_archive` | `Lists differ: ['sess-b3'] != [] ... SCRF-B3 RED: discard→stop 触发了 _story_finish_session`（1 failed） | sha1 一致 |
| 12 | SCRF-B4 | ctx 边沿 accumulate 分支删掉 `acc.cancel(reason="session_end_stale")`（只留释放） | `-k test_scr_b4_accumulate_cancels_and_releases` | `('sess-b4-acc', 'session_end_stale') not found in [] ... SCRF-B4 RED: 非 story ctx 后未落 cancel(reason=...)`（1 failed） | sha1 一致 |
| 13 | SCRF-B4b | ctx 边沿 pending_stop 保护分支改成 cancel + 释放 | `-k test_scr_b4b_pending_stop_edge_then_stt_archives_once` | `Lists differ: [('sess-b4b', 'session_end_stale')] != [] ... SCRF-B4b RED: ctx 边沿把 pending_stop 取消了`（1 failed） | sha1 一致 |
| 14 | SCRF-B5 | teardown `_get_story_acc().cancel(session_id, reason="ws_disconnect")` → `pass` | `-k test_scr_b5_teardown_completes_cancel_and_legs_removed` | `('sess-b5...','ws_disconnect') not found in [] ... SCRF-B5 RED: 断连收尾 cancel(ws_disconnect) 未落`（1 failed） | sha1 一致 |
| 15 | SCRF-B6 | 临时把 `server/acceptance/test_m4k1_story_cloud_link.acceptance.py` 改名挂起（presence 守卫变异），跑完立即改回 | `-k test_scr_b6_frozen_oracles_present` | `AssertionError: False is not true : SCRF-B6 RED: 既有冻结 oracle test_m4k1_story_cloud_link.acceptance.py 丢失`（1 failed） | 改名还原 + sha1 一致 |

不敏感项（如实申报，非实现缺陷）：
- **SCR-A4 的 `RaiseReminderQueue` needle 被既有注释遮蔽**：`application.cc:1342` 的既有
  注释文字含该符号，把代码里那一行改名（`RaiseReminderQueue` → `NotifyReminderQueue`）
  后 SCR-A4 仍绿（该次变异只红 `reminder_overlay_unify` RM-A3）。故 SCR-A4 的敏感证据
  改用同区另一符号（`kidpal_keybar_scene_reset`，代码单点、注释无该字面）承载。该注释位
  于「Idle 八件清位链逐字不动」区，本包未改动，留作验收质量观察项。

## 落盘路径
- `firmware/main/application.cc`（T1–T4）
- `firmware/main/application.h`（声明/注释同步）
- `firmware/main/test/host_app/main/CMakeLists.txt`（登记行）
- `server/gateway/front/front_app.py`（T5–T9）

## 阻塞项（需 planner 决策，本 builder 未自行处置）

**`firmware/main/test/record_dialogue_guide.acceptance.c` 的 RCL-F1 与 P1 冻结验收结构性冲突。**

- 现象：实现后 host 全量多出 1 个红：`record_dialogue_guide.acceptance.c:1057 (RCL-F1)`，
  `Expected 1 Was 2`（`SendStopListening(` 在区内的出现次数）。
- 根因：RCL-F1 的区切 = `void Application::HandleStopListeningEvent` →
  `void Application::HandleWakeWordDetectedEvent`（粗粒度函数序锚），该跨度**必然包含**
  `HandleEndVoiceSessionEvent` 的函数体；而 P1 冻结验收（`session_channel_resident`
  SCR-A2 / `voice_round_ui` OCE-A1）明确要求该处理器区内 `protocol_->SendStopListening()`
  恰 1。于是跨度内计数必然从 1 变 2。两条要求互斥，无法用实现规避（改名/拆 helper 都会
  同时打破 SCR-A2 的区内字面恰 1）。
- 该文件**不在** `changeManifest[]`，也不在 `impactedTestsSeed[]`（1g 扫描补了
  diag_log / record_session_lifecycle / record_session_repair / chrome_ghost /
  reminder_overlay_unify 五件，漏了本件）。按边界纪律停手，未改任何验收件。
- 建议处置（二选一，请 planner 定）：
  1. **（推荐）扩单重锚 RCL-F1**：`fn_end` 由 `"void Application::HandleWakeWordDetectedEvent"`
     收窄为 `"void Application::HandleEndVoiceSessionEvent"`（恢复原意：只量 KEY 路径那一个
     发送点），并加一条 REVISION LOG 说明「统一会话结束处理器新增 listen stop 发送点，
     原粗粒度区切把处理器纳入计数导致假红；断言语义零变化」。改动 1 行 + 日志。
  2. 授权把 `HandleStopListeningEvent` 的 Listening 分支改为直调统一处理器（五收尾点
     归一）。可行但属 plan T1 之外的语义改动（in-flight 期由「发 stop + Idle」变 no-op；
     `!protocol_` 分支可显式保留），需 planner 明确授权后我再落，并同步改 RCL-F1 的
     意图措辞。
- 除该件外，其余 must-stay-green 与既有冻结件零回归（host 4 红中 3 项为上游存量）。

## 下一步
1. planner 就 RCL-F1 给处置（扩单重锚 / 授权归一）。
2. 处置落地后重跑 host 全量，预期 306 Tests / 3 Failures / 0 Ignored（仅上游 p1_auxbar 族）。
3. 设备构建（`application.cc` / `application.h` 编译面）与真机轮 A（A1–A13）未在本
   worktree 执行——application.cc 不进 host 构建，本地仅做源文本级验证；设备编译与轮 A
   归 integrator / human。

## planner 补记（2026-11-16，planner 代落）

- **RCL-F1 处置已落**：planner 走扩单回评，manifest 纳入 `firmware/main/test/record_dialogue_guide.acceptance.c`，acceptance-author 已把 RCL-F1 的 `fn_end` 收窄为 `HandleEndVoiceSessionEvent` + REVISION LOG（断言语义零变化、tighten-only）。
- **基线刷新**：重锚后 host 实测 **306 Tests / 3 Failures / 0 Ignored**（3 红全为上游 `p1_auxbar_style` AB-A1/AB-A2/AB-A4）。上表 mutation 列的「基线 4」为当时含 RCL-F1 假红的口径，重锚后各变异的「+1 转红」结论不变（已由 acceptance-author 与 planner 复跑确认）。
- 其余偏离与残差（标记清位跨连接、审计行 `state=session_end*`、心跳三方无锁、`IsTimeout` 日志密度待真机量测）planner 已接收，随 review-packet 申报。