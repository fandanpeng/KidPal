# 变异取证 · 261116-channel-resident-core 真机轮 A 修复轮

> 执行者 = builder（dispatch `a139ea79cc99dab42` implementation-r2 + `ad3e2e0f2cc1cc416` mutation-b10）；落盘 = planner（builder 对 `delivery/taskpackages/**` 无写权，按派发书硬约定③代落）。
> 还原方式 = 原文件全文写回 + `sha256` 比对（逐条一致）；未用 git checkout。
> 命令基准：server = `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance`（仓根）；host = 执行目录 `firmware/main/test/host_app` 跑 `./build/host_infra_test.elf`。

## 一、实现落点基线

- `server/gateway/front/front_app.py` sha256（变异前/还原后）= `1f097edbdf29e1e2f868009e2af488eab8be487f2e18b3cc13eca0194c38b98e`
- 基线测试：server `486 passed / 0 failed / 9 deselected`；host `307 Tests / 3 Failures / 0 Ignored`（3 红 = 上游存量 `p1_auxbar_style` AB-A1/A2/A4）。

## 二、变异表

| # | Acceptance | 精确变异 | 命令 | 转红测试 | 还原 |
|---|---|---|---|---|---|
| 1 | SCRF-B7 | ctx 分支 `if upstream is not None:` → `if ctx_push_edge and upstream is not None:` | `pytest ... -k "b7 or a4"` | `test_scr_b7_repeat_ctx_story_injects_again_story_list_stays_edge_only`、`test_rdg_a4_front_injects_record_context_enter_exit_on_engine_leg` | sha256 一致 |
| 2 | SCRF-B7 | `_story_route_display_payload` 末 `return True` → 先算 `enter_edge` 再 `return enter_edge` | 同上 | `test_scr_b7_start_recording_consumption_injects_when_accumulating` | sha256 一致 |
| 3 | SCRF-B7 | token `story_record_ctx_enter` → `story_record_ctx_hit`（非超串） | 同上 | B7 三例（含 `test_scr_b7_audit_token_single_emit_point`） | sha256 一致 |
| 4 | SCRF-B7 | 删 ctx 分支 `_inject_record_context_enter(...)` 调用 | 同上 | `test_scr_b7_repeat_ctx...`、`test_rdg_a4_...` | sha256 一致 |
| 5 | SCRF-B8 | 删 discard 分支 `_release_record_context(session_id, [(websocket, upstream)])` | `pytest ... -k b8` | `test_scr_b8_discard_releases_context_before_forwarded_stop`、`test_scr_b8_repeated_stop_injects_no_second_release` | sha256 一致 |
| 6 | SCRF-B8 | discard 腿集改 `_session_legs(session_id) or list(_alert_legs)` | 同上 | `test_scr_b8_discard_branch_has_no_global_alert_legs` + 两例 | sha256 一致 |
| 7 | SCRF-B9 | 删门内二进制丢弃（`if _closed_gate_closed(...) and not _speech_token_active(...): continue`） | `pytest ... -k b9` | `test_scr_b9_gate_drops_late_audio_tts_llm_keeps_mcp_and_stt`、`test_scr_b9_speak_token_allows_round_then_tts_stop_clears` | sha256 一致 |
| 8 | SCRF-B9 | 丢弃集合 `("tts","llm")` → `== "tts"` | 同上 | `test_scr_b9_gate_drops_late_audio_tts_llm_keeps_mcp_and_stt` | sha256 一致 |
| 9 | SCRF-B9 | 丢弃集合加 `"mcp"` | 同上 | 同上 | sha256 一致 |
| 10 | SCRF-B9 | 删 `_deliver_alert_now` 内 `_speech_token_register(device_ws)` | 同上 | `test_scr_b9_alert_speech_registers_expected_round_token` | sha256 一致 |
| 11 | SCRF-B9 | tts stop 先判令牌后清 → 先清后判 | 同上 | `test_scr_b9_speak_token_allows_round_then_tts_stop_clears` | sha256 一致 |
| 12 | SCR-A8 | `kStoryEmptySessionTimeoutS = 60;` → `= 30;` | host 全量 | `(SCR-A8):FAIL`（307/4） | sha256 一致 |
| 13 | SCR-A8 | producer 删 `!s_story_rec_entered &&` | host 全量 | `(SCR-A8):FAIL`（307/4） | sha256 一致 |
| 14 | SCR-A8 | 审计 token `story_empty_session_timeout` → `story_empty_session_hit` | host 全量 | `(SCR-A8):FAIL`（307/4） | sha256 一致 |
| 15 | SCR-A8（交叉） | 既有 60s 常量定义行**前**插入含其符号字面 + ASCII `;` 的注释 | host 全量 | `(SCR-A8):FAIL` **+ `(SCR-A5):FAIL`**（307/5） | sha256 一致 |
| 16 | SCRF-B8（对照齿） | 在转发 `listen stop` 前插入一次 `_release_record_context(session_id, [(websocket, upstream)])` | `pytest ... -k b8` | B8 三例，含 `test_scr_b8_key_stop_still_archives_once_and_releases_after_stop` | sha256 一致 |
| 17 | SCRF-B10 | 空素材释放分支改走 `_story_finish_session(...)`（kill-shot） | `pytest ... -k b10` | `test_scr_b10_empty_material_stop_releases_without_archive`（:1863 断言 `fin == []`，实记 `['sess-b10-empty']`） | sha256 一致 |
| 18 | SCRF-B10 | `_archive_material_reject_reason` 首行改 `return None`（内容守卫失效） | `pytest ... -k b10` | `test_scr_b10_tail_hallucination_rejected_same_release_contract`（:1917 零 `start_organizing` 失败，实得 1 帧） | sha256 一致 |
| 19 | SCRF-B10（观测面收紧后） | 会话腿 push `story_list`（runtime monkeypatch 等价复现源变异：`_release_record_context` 后对每个会话设备腿 `_push_story_list`） | `pytest ... -k b10` | `test_scr_b10_empty_material_stop_releases_without_archive`（新增会话腿断言 :1885 `1 != 0`） | 临时脚本已删，`git status` 无残留 |

## 三、如实申报（观察项，非阻塞）

1. **变异 #3 首试超串改名不转红**：断言用子串/`count` 匹配，`story_record_ctx_enter` → `story_record_ctx_enter_renamed`（原名是新名子串）仍绿；改非超串 token 后转红。**结论：token 断言对「超串改名」不敏感**，登记为验收质量 NIT（不影响本轮判别力总量）。
2. **变异 #15 首试注释不含 ASCII `;` 不转红**：SCR-A5 区切从注释到定义行之间仍含 `60`；补 ASCII `;` 后 A8 + A5 同红。说明 SCR-A5 交叉负锚只对「含分号的注释」成立——**登记为观察项**（新常量注释纪律本身仍必须遵守）。
3. **B10 原「零 story_list」断言观测面盲区已闭合**：原断言只看外层 `dev.sent_text`，而 sweep 释放投到 `_session_legs(sid)` 登记的会话设备腿；变异 #17 之外的单纯「会话腿推 story_list」原不转红。经 acceptance-author tighten-only 补会话腿观测（:1885）后，#19 转红。**残余同类形状**：同测试的 `start_organizing` 断言仍只观测外层 `dev`（空素材释放路径本不下发该帧，本轮不扩改，登记观察项）。
4. **B10 的「零归档 md」断言在当前离线无 LLM stub 时判别力有限**：变异 #18 由「零演出」断言抓红（`_issue_finish_downlink` 先于 LLM），md 断言未单独承担判别力。登记观察项（不伪造其判别力）。