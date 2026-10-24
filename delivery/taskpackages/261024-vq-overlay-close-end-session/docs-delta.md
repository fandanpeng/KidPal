# docs-delta · 261024-vq-overlay-close-end-session

> 产出方: integrator（closeout 制品，2026-10-24）。`docsSyncRequired: false`——本包零 canonical spec / contracts 改动，本文如实申报并登记改动面摘要。

## 1. spec / contracts 过时面申报

**无 spec 过时面。**

- `docs/requirements.md` §2.3（:147「问答 · 播报中」行）已冻结「BOOT 短按 / KEY 长按关层 = 结束会话回原页」的目标语义——本包不是需求变更，而是**实施偏离修正**：板级 `GLOBAL_ACT_OVERLAY_CLOSE` 会话级联此前复用 `ToggleChatState`（播报态 = 打断 → 聆听），与冻结语义不符，TECH-261024 真机事故（KPLOG011）即该偏离的显形。
- `docs/architecture.md`：零触面。无模块边界 / 数据流 / 分层变化——新增的是 `Application` 内一个事件位 + 统一结束入口，属既有 Application 主循环事件分发模式的就地延伸。
- `docs/contracts/interface-contract.md`：零 diff。abort（`AbortSpeaking(kAbortReasonNone)`）与关通道（`CloseAudioChannel`）均为既有 WS 消息语义，不新增 wire 语义、不改帧类型与字段类型；`crossEnd.exempt` 申报已留痕（triage architect 预查 = NO_CONTRACT_CHANGE）。
- `docs/debt/tech/TECH-261024-overlay-close-speaking-no-session-end.md`（sourceRefs）：本包落地即该 debt 的修复件；debt 条目关闭/降级处置归 planner 收尾相位，非 integrator 写权面。

## 2. 改动面摘要（3 实现 + 2 验收）

实现面（builder，代码已冻结、verifier 判绿）：

| 文件 | 改动 |
|---|---|
| `firmware/main/application.h` | 新增事件位 `MAIN_EVENT_END_VOICE_SESSION (1 << 13)`（bit 13 现空闲）；公开 `void EndVoiceSession();`；私有 `void HandleEndVoiceSessionEvent();` |
| `firmware/main/application.cc` | `ALL_EVENTS` 掩码加新位；主循环新分支置于 `MAIN_EVENT_TOGGLE_CHAT` 之后、`MAIN_EVENT_SCHEDULE` 之前；`EndVoiceSession()`（事件位置位，线程安全）；`HandleEndVoiceSessionEvent()`（三态门 + 播报态 ResetDecoder/AbortSpeaking + CloseAudioChannel + Idle + LOW_POWER + `local_close_settle src=overlay_close` + ScheduleResidentRebuild；时序自证注释在位） |
| `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc` | `GLOBAL_ACT_OVERLAY_CLOSE` 会话级联改调 `app.EndVoiceSession()`（三态门下沉处理器单源裁决）；注释重写（原「复用 ToggleChatState」注释改后即谎，已消除） |

验收面（acceptance-author，纯 additive 收紧、语义零放松）：

| 文件 | 改动 |
|---|---|
| `firmware/main/test/voice_round_ui.acceptance.c` | KVR-A4 tooth 2 重钉（板级 `app.ToggleChatState()` 文件级 ≥2 → ≥1 + OVERLAY_CLOSE 切区含 `app.EndVoiceSession()`）；新增 TEST 12（OCE-A1 结构断言：收尾六件内含 + 静音→abort→关通道冻结序 + 板级切区零 `app.ToggleChatState()` 负向） |
| `firmware/main/test/boot_auto_connect.acceptance.c` | RC-A2 tooth 6 计数 4→5（定义 + 四调用点）；RC-B2 sites 数组追加第四项（`HandleEndVoiceSessionEvent` 函数区）且四项各恰 1；头注 / 重钉留痕补记（纯注释行） |

非 manifest 观察项（如实申报）：`firmware/sdkconfig` 工作树出现 5 行 diff——仅删除「Image Effects Configuration」空注释块（ESP-IDF 构建再生的 sdkconfig 注释层漂移，零功能语义）。已随本包 commit 收口并在 review-packet / scope-file 留痕。

`firmware/main/test/host_app/main/CMakeLists.txt` 零 diff（两验收文件均已注册 :69/:168，changeManifest 该条目为预置条件触发面、本包不触发——冻结回填 ⑥ 已留痕）。

## 3. OCE-A4 真机目检 pending 状态

OCE-A4（live-manual 真机目检六项：① 播报中 BOOT 短按全链日志序 + ≥4 分钟无 ASR/无出声；② KEY 长按同效；③ KEY 短按仍打断→聆听；④ 聆听态 BOOT 行为不变；⑤ Connecting 态关层回 Idle；⑥ T 秒常驻重建）= **pending human 持板**。Pod 到真机门前停等（`awaiting-human:real-device`），不自行烧录；OCE-X4 由 human 执行并留痕。

## 4. docs-sync 判定

`docsSyncRequired: false` + r3 docs-sync 闸绿（reason 原文见 taskpackage.json `docsSyncReason`）。canonical 面零写、零漂移检出，无 architect 移交项。
