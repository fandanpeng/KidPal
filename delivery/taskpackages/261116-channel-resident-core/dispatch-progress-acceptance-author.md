# dispatch-progress · acceptance-author · 261116-channel-resident-core

> 本文件 = 当前派发（真机轮 C 修复轮：冻结 SCRF-C1..C4 四条新验收并跑红基线）的自包含
> 进度快照。覆盖写；不载 tokens / gates / rounds / verdict。

## 本轮目标（一句话）

在已冻结的 `server/acceptance/test_session_channel_resident.acceptance.py` 末尾纯 additive
追加两个 TestCase 类，把 TaskPackage 的 SCRF-C1 / C2 / C3 / C4 钉成可执行断言，并实跑
记录真实红基线（实现未落地 → 设计态红）。

## 已完成段落

1. 输入通读：`input-brief.md`（含 ② 跨端面与 ① 证据清单）、`plan.md`「真机轮 C 修复轮
   （2026-11-18）技术方案」全节、TaskPackage `acceptance[]` 的 SCRF-C1..C4、既有验收件
   全文（B1..B10 与全部 helper）、`server/gateway/front/front_app.py` 只读核对
   （`_story_feed_from_stt` / `_organize_and_archive` / `_story_finish_session` /
   `_story_timeout_sweep_body` / `_story_guide_window_active` /
   `_archive_material_reject_reason` / `_consume_kidpal_control` / `pump_*`）、
   `server/gateway/story.py` 的 accumulator 与 list/story_text 形状、round-c 三源证据。
2. 变更点（只改本验收件，实现面零改动）：
   - 头注追加「轮 C 修复轮 REVISION LOG」段：范围、真机缺陷面（round-c）、强度申报
     （契约条目 → 承载断言 → 性质档）、显式缺口 GAP-C1/C2/C4、行为等价假设 EQ-C1/C2、
     与 human「不推 story_list」原话的口径分法留痕、夹具自含性、device-outcome 不适用说明。
   - `import re`（新断言做集合不变式的归一化度量）。
   - 新 helper：`_guard_compact` / `_story_text_frames` / `_kidpal_speak_frames` /
     `_ledger_snapshot`；冻结 token 常量 `SILENT_ASR_DROP_TOKEN` /
     `ORGANIZE_PLACEHOLDER_REASON`。
   - 新类 A `ScrfSilentAsrSegmentFilterAcceptance`（SCRF-C1 + SCRF-C4，4 个 case）。
   - 新类 B `ScrfOrganizePlaceholderGuardAcceptance`（SCRF-C2 + SCRF-C3，4 个 case）。
   - `git diff -U0 --numstat` 实测：**新增 750 行 / 删除 0 行**（既有 B1..B10 断言行零改动）。

## 红基线（本轮实跑）

- 命令（仓根起，沿用 TaskPackage redBaselineCommand 的 server 半）：
  `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance`
- 全量结果：**13 failed / 490 passed / 9 deselected / 35 subtests passed**。
  其中既有 486 条全绿（零回归，含本件 B1..B10 的 28 条：`-k scr_b` → 28 passed）。
- 本件结果：**13 failed / 32 passed / 2 subtests passed**（37 条 case）。
  设计态红逐条：
  - C1 `test_scr_c1_pending_stop_silent_asr_dropped_then_sweep_releases`：段级过滤缺席，
    `Okay.` 进 pending_stop 累积器 → `_story_finish_session` 实记 `['sess-c1']`。
  - C2 `test_scr_c2_organize_placeholder_guard_releases_with_page_exit`：占位回复被当正文
    归档，落盘 id 逐字复现 `2026-11-18-s33477cee.md`（round-c 垃圾篇）。
  - C2 `test_scr_c2_output_guard_mechanical_discrimination`：8 条拒绝样本各 SUBFAILED
    （`。` / `嗯` / `没有可整理的内容` / `无内容` / `Nothing to organize.` / `【无】` /
    `(no content)` / `（本轮素材不足）` 全部照常归档）；放行面 2 条 + 引号族 + title==body
    负锚已绿。
  - C4 `test_scr_c4_hit_family_dropped_and_mixed_speech_kept`：首个命中样本 `Okay.` 仍入
    累积器。
  - C4 `test_scr_c4_pending_stop_filter_does_not_release_immediately`：幻觉 stt 走了
    收尾单源并即时释放（实记 `['sess-c4-pend']`）。
  - C4 `test_scr_c4_segment_set_invariant_and_single_source`：模块级
    `_FRONT_SILENT_ASR_HALLUCINATIONS` 不存在。
  设计态绿（must-stay-green，不造红）：C3 两条（正常归档 + llm_unavailable 安抚/退出帧）、
  C4 `test_scr_c4_finish_phrase_surface_unaffected`、C2 判别力 case 的放行面。
- 线束冒烟：pytest 收集零错误（`--collect-only` = 37 tests collected）。无 live/SSH 面。

## 下一步

- builder 落两处实现：① `_story_feed_from_stt` 段级整串过滤 + 新模块常量
  `_FRONT_SILENT_ASR_HALLUCINATIONS`（审计 `dropped=silent_asr_hallucination`）；
  ② `_organize_and_archive` 在 `archive_story` 之前加 R1–R4 产物守卫（`reason_sink`
  keyword-only）+ `_story_finish_session` 占位命中走静默释放类（零安抚 + 页面退出帧 +
  `reason=organize_placeholder`）。
- 落地后按同命令重跑：期望本件 37 条全绿，其中 C3 四条面保持绿（回归网）。