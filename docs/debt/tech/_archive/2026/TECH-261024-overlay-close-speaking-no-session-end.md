---
id: TECH-261024-overlay-close-speaking-no-session-end
title: 播报态关层不结束会话——overlay_close 复用 ToggleChatState 只打断停播，回首页后麦克风仍开并继续语音问答（KPLOG011 真机实证）
domain: tech
status: closed
severity: MAJOR
owner: dev-loop（固件 application.cc 会话收尾面 + waveshare-s3-rlcd-4.2 板级路由消费）
trigger: 已拍板——2026-10-24 human 报障 + /triage-change 比例化 review 判 ②单包 Full（riskTier high），即刻起包 261024-vq-overlay-close-end-session
opened: 2026-10-24
closed: 2026-10-24
closedBy: 261024-vq-overlay-close-end-session
paths: ["firmware/main/application.cc", "firmware/main/application.h", "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc", "firmware/main/test/voice_round_ui.acceptance.c", "firmware/main/test/boot_auto_connect.acceptance.c"]
source: 2026-10-24 18:38-18:42 真机事故（设备 TF 卡 KPLOG011 段拉取判读 + KPLOG010 段同日 7 次同动作对照）
handoff: 包 261024-vq-overlay-close-end-session（②单包 Full / riskTier high）；reviewed plan = delivery/taskpackages/261024-vq-overlay-close-end-session/plan.md
---

**现象（human 2026-10-24 报障原话）**：「他刚才都返回首页了，结果还在聆听，而且还有语音回复，但是页面还是首页。」

**真机时间线（设备 TF 卡 KPLOG011，2026-10-24）**：

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

**对照证据**：同日 KPLOG010/011 共 8 次 `action=4(overlay_close)`，其余 7 次都发生在聆听态，全部正常走 `listening -> idle` + WS 断开 + `local_close_settle`；只有 18:38:00 这一次发生在播报态，暴露缺口。

**根因**：板级 `GLOBAL_ACT_OVERLAY_CLOSE` 分支在会话态统一调 `Application::ToggleChatState()` 期望「关层 = 结束会话」，但 `HandleToggleChatEvent()` 的 Speaking 分支只做 `AbortSpeaking(kAbortReasonNone)`——这是 requirements §2.3「播报中 KEY 短按 = 打断停播 → 聆听」的语义，不是结束会话。真正收尾（`CloseAudioChannel` + `SetDeviceState(Idle)` + `LOW_POWER` + `local_close_settle` + `ScheduleResidentRebuild`）只写在 Listening 分支。打断后服务端回 tts stop，状态机按对话轮出口回到 Listening 并重启音频处理，通道始终保持。Connecting 态在 `HandleToggleChatEvent()` 里没有任何分支，是同一类洞。

**需求锚点**：`docs/requirements.md:147` §2.3「问答 · 播报中」行——KEY 短按 = 打断停播 → 聆听；KEY 长按 / BOOT 短按 = 结束会话回原页。实现只做了前者。这是实现偏离冻结需求，需求 / 架构 / 契约 / 治理四面均无需变更。

**修法方向（architect reviewed）**：新增事件位 `MAIN_EVENT_END_VOICE_SESSION` + `Application::EndVoiceSession()` 统一结束入口，覆盖 Connecting / Listening / Speaking 三态；板级 OVERLAY_CLOSE 改调该入口。播报态收尾须显式 `ResetDecoder()` 即时静音（Idle 分支不重置解码队列），再 abort、关通道、回 Idle。详见 reviewed plan。

**残余观察（另案，不纳入本包）**：路由表覆盖层列 / 查字窗列 / 基层列的 BOOT 双击均返回 `GLOBAL_ACT_NONE`，与 requirements §2.3「BOOT 双击 = 同短按」不一致，属路由面漂移；修好后 BOOT 双击会自然汇入同一个 `EndVoiceSession()` 入口。

---

**闭账留痕（2026-10-24）**：包 `261024-vq-overlay-close-end-session`（②单包 Full / riskTier high / sourceType regression）已 merge 本地 main（`d5515ce0`，human-approval-ref `5355ab69`）。代码面：scoped r3 + final r5 全闸绿，host 237 Tests 3 Failures（3 条全为先存 p1_auxbar 红，零新增红），固件真编译通过；真机目检：播报中 BOOT 短按（22:00:04）与 KEY 长按（22:05:59）均走 `Abort speaking → Websocket disconnected → speaking -> idle → local_close_settle src=overlay_close`，播报中 KEY 短按仍仅打断，聆听态 BOOT 行为不变，20:58:40 关闭后 61 分钟零 ASR/零出声，常驻重建 5–6 秒。**未覆盖申报**：Connecting 态关层（需弱网复现）经 human 2026-10-24 明示跳过，真机未验；该分支代码已实现、host 结构断言覆盖调用位，运行时行为待将来真机轮补验。
