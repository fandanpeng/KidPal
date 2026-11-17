# dispatch-progress · acceptance-author · 261116-channel-resident-core

> 本文件 = 当前派发（真机轮 A 修复轮 D1/D2/D3 验收冻结）的自包含进度快照。
> 覆盖写；不载 tokens / gates / rounds / verdict。

## 本轮目标（一句话）

为真机轮 A 三条缺陷（D1 语境注入去边缘化 / D2 丢弃终止在途回合 + 迟到帧门 / D3 空素材
收尾）冻结新增验收断言（SCRF-B7/B8/B9、SCR-A8/B10）并确认设计态红基线；RDG-A4 显式重锚。

## 已落盘文件

1. `server/acceptance/test_session_channel_resident.acceptance.py`
   - 头注追加 B7–B10 scope + 修复轮强度申报（含 GAP/EQ/夹具自含性）。
   - 新增 helpers：`AUDIO_BYTES` / `_frames_of_type` / `_frame_indices` /
     `_is_release_frame` / `_is_listen_stop_frame` / `_is_abort_frame` / `_tts_frame` /
     `_llm_frame` / `_mcp_probe_frame` / `_display_blocks_frame` /
     `_start_recording_block` / `_log_lines_with` / `_log_capture`（+ `import logging`）。
   - 新增 13 个 test method（4 个 class）：
     - `ScrfRecordCtxUnconditionalAcceptance`（B7×3，:1380 起）
     - `ScrfDiscardTerminatesRoundAcceptance`（B8×4，:1490 起；含 must-stay-green 对照齿）
     - `ScrfClosedSessionFrameGateAcceptance`（B9×4，:1650 起）
     - `ScrfEmptyMaterialReleaseAcceptance`（B10×2，:1837 起；预期设计态绿）
2. `firmware/main/test/session_channel_resident.acceptance.c`
   - 头注加设备结果标记（成对）：`device-outcome: SCR-A8` +
     `host-uncompilable-path: firmware/main/application.cc`（与 launch-card 申报一致）。
   - 头注强度申报追加 SCR-A8 行 + GAP-4。
   - 新增 `TEST_CASE("会话通道常驻: 空素材空闲收尾producer (SCR-A8)", ...)`（:667 起）。
3. `server/acceptance/test_record_dialogue_guide.acceptance.py`
   - RDG-A4 重复 ctx(story) 断言显式重锚：旧钉「第二次零 active=true」→ 新钉「第二次
     仍恰一帧 active=true + story_list 零新增」。
   - 冻结契约 C 节「进语境两收敛入口」措辞同步为无条件注入（幂等由引擎认领缝承载）。
   - REVISION LOG 追加两条 2026-11-17 条（旧钉→新钉→为什么→授权）。

## 红基线（本轮已实跑）

- host（执行目录 `firmware/main/test/host_app`）：build 编译通道 error 0；
  `307 Tests / 4 Failures / 0 Ignored` = 3 上游存量红（p1_auxbar_style AB-A1/A2/A4）
  + 1 本包设计态红（SCR-A8：`kStoryEmptySessionTimeoutS` 缺席）。
- server（仓根 `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance`）：
  `9 failed / 477 passed / 9 deselected`。9 红 = 新件 8（B7×3 / B8×2 / B9×3）+
  RDG-A4 重锚 1；既有 473 条零回归。
- 新件 5 条设计态绿（B8 源锚负锚、B8 KEY 收尾对照齿、B9 提醒令牌登记点、B10×2），
  敏感性由 mutation 兜底（见交付报告变异表）。

## 下一步（builder / planner）

- builder 按 plan.md「真机轮 A 修复轮」D1/D2/D3 实施；`_inject_record_context_enter`
  审计 token `story_record_ctx_enter` 在 front_app.py 只出现 1 次；实现注释不得复述
  判据字面（B7 计数断言、SCR-A8 计数断言会因此稀释）。
- builder 逐条做变异取证（转红→还原），并注意 DISCARD 分支腿集写死
  `[(websocket, upstream)]`（`_alert_legs` 负锚会红）。
- planner：TaskPackage `acceptance[]` 由 intent 细化冻结（本文件只冻结断言，不改
  TaskPackage）；`mutationEvidence.perAcceptance` 需补 5 条新验收的变异记录。

## 本轮收紧（观测面补盲）

- 2026-11-18 `test_scr_b10_empty_material_stop_releases_without_archive`：B10「零新增
  story_list」断言原只观测外层 `dev.sent_text`，漏掉 sweep 释放按
  `_session_legs(sid) or _alert_legs` 投递的会话设备腿 `dev_leg`（builder kill-shot
  变异未使该断言转红）。补一条对 `dev_leg.sent_text` 的同方向零新增断言（纯 additive，
  原外层断言保留），语义方向不变、仅扩观测面；kill-shot 复核（运行时 monkeypatch 向会话
  腿推 story_list）已使新断言转红（`1 != 0`）。`-k b10` 2 passed；全量非 live 486
  passed 不变。