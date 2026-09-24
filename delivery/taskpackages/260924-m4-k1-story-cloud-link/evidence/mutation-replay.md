# Mutation Replay Evidence · 260924-m4-k1-story-cloud-link（修订后快照）

> 落盘责任人 = planner（派发书三条硬约定③：第一选择 = 有写权的执行角色本人；builder 无 `delivery/taskpackages/**` 写权，故由 planner 代落）。
> **第一方原始输出 = builder 闭环复跑报告（2026-09-24，修订后快照）**，planner 原样收录、不改写；planner 另做终态抽查（§4）。
> 冻结件基线：`server/acceptance/test_m4k1_story_cloud_link.acceptance.py` sha256 = `5d4ebbf28f13cf325483738430f62a60105f54176eba1d85fc15a355c625a287`（tighten-only §6 修订后交付形态，commit `388719d`）。
> 冻结期变异实测（MUT-1~4、6、7 运行期扰动 + 原变异重放 `3 != 4`）= acceptance-author 第一方记录，见 `server/acceptance/M4K1-ACCEPTANCE-NOTES.md` §4.2 / §9（不在此重复）。

## 1. 绿定义复跑（builder 面闭环证据，仓根逐字命令）

```
python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance
```

尾行逐字（`PYTHONIOENCODING=utf-8`）：

```
182 passed, 4 deselected in 4.64s
```

构成 = 上游锚 134@5e862d0 + 冻结 26 齿全绿 + builder 新增单测 22（`server/tests/test_story_units.py`）。同结果三跑一致（acceptance-author 4.46s / planner 132.93s〔机器负载波动〕/ builder 4.64s）。

## 2. 全套 mutation 重放（六组 + MUT-5，builder 第一方实跑）

运行口径（builder 申报）：变异体应用后跑单文件 `python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/test_m4k1_story_cloud_link.acceptance.py`；每次还原后以全命令（`server/tests server/acceptance`）复绿。

### MUT-A1（canonical：移除 Q8 story_list/story_text 形状 floor + 移除 build_display_tool_calls 对 story_* 降级过滤）

变异点：`display_schema.py` story_list/story_text 两条件 `and False`；`front_app.py` `_STORY_SINGLE_SOURCE_VIEWS` 分支 `if False and …`。

```
FAILED …::M4K1ContractGateAcceptance::test_a1_relay_rejects_engine_side_story_blocks_q7
FAILED …::M4K1ContractGateAcceptance::test_a1_story_list_items_three_keys_required
FAILED …::M4K1ContractGateAcceptance::test_a1_story_list_total_must_be_nonneg_int
FAILED …::M4K1ContractGateAcceptance::test_a1_story_text_seq_must_be_int_when_present
4 failed, 22 passed in 0.92s
```

还原复绿：`182 passed, 4 deselected`。expectedRed **4/4** ✓

### MUT-A2（移除 pump 对 kidpal_session_ctx 拦截 + source_page=story 切累积态）

变异点：`front_app.py` `_consume_kidpal_control` story 分支 `if False and …` 且 `return True`→`return False`。

```
FAILED …::M4K1SessionCtxAcceptance::test_a2_idempotent_convergence_with_start_recording
FAILED …::M4K1SessionCtxAcceptance::test_a2_session_ctx_intercepted_not_forwarded
FAILED …::M4K1SessionCtxAcceptance::test_a2_source_page_values_and_backward_compat
FAILED …::M4K1StoryListPushAcceptance::test_a3_push_seam_connect_and_wire_timepoints
FAILED …::M4K1FinishTimeoutAcceptance::test_a5_finish_phrase_triggers_stop_in_accumulate_only
5 failed, 21 passed in 0.99s
```

还原复绿：`182 passed, 4 deselected`。expectedRed **3/3** ✓（连带红 2：a3_push_seam / a5_finish_phrase——依赖 session_ctx→累积态，同源合理）

### MUT-A3（list_stories 去窗口返全量 + 移除三时点 story_list 推送）

变异点：`story.py` `window = len(ordered)`；`front_app.py` `_push_story_list` 体内早 `return  # MUT-A3`。

```
FAILED …::M4K1StoryListPushAcceptance::test_a3_list_stories_window_and_total_independent
FAILED …::M4K1StoryListPushAcceptance::test_a3_push_seam_connect_and_wire_timepoints
FAILED …::M4K1DeleteAcceptance::test_a4_delete_executes_and_repushes
FAILED …::M4K1DeleteAcceptance::test_a4_delete_nonexistent_id_idempotent_repush
4 failed, 22 passed in 0.75s
```

还原复绿：`182 passed, 4 deselected`。expectedRed **2/2** ✓（连带红 2：a4 两齿——删除后重推 = `_push_story_list`，同源合理）

### MUT-A4（delete_story 去哨兵头/路径穿越校验 + 去删除后重推对账）

变异点：`story.py` `delete_story` 三处校验各自 `if False`；`front_app.py` 删除分支 `await _push_story_list(websocket)` → `pass  # MUT-A4`。

```
FAILED …::M4K1DeleteAcceptance::test_a4_delete_executes_and_repushes
FAILED …::M4K1DeleteAcceptance::test_a4_delete_nonexistent_id_idempotent_repush
FAILED …::M4K1DeleteAcceptance::test_a4_delete_rejects_traversal_and_no_sentinel
3 failed, 23 passed in 0.72s
```

还原复绿：`182 passed, 4 deselected`。expectedRed **3/3** ✓

### MUT-A5（AUTO_STOP_SECONDS=999 + is_finish_phrase 恒 False + front 收尾触发短路）

变异点：`story.py` 常数改 999、`is_finish_phrase` 恒 `return False`；`front_app.py` `_story_finish_on_phrase` 判据 `if True`。

```
FAILED …::M4K1FinishTimeoutAcceptance::test_a5_finish_phrase_detection
FAILED …::M4K1FinishTimeoutAcceptance::test_a5_finish_phrase_triggers_stop_in_accumulate_only
FAILED …::M4K1FinishTimeoutAcceptance::test_a5_timeout_120s_decision
3 failed, 23 passed in 0.81s
```

还原复绿：`182 passed, 4 deselected`。expectedRed（canonical 更正版）**3/3** ✓；advisory `test_a5_front_wiring_references_timeout_decision` **保绿**（源锚代理齿、子串判据对实现变异不敏感——与 canonical A5 更正一致，收紧评估归 loop-retro 素材）

### MUT-A6（移除 seq 持久账本，按现存 MD 计数派生分配、不回写 .seq.json）——首跑未闭环项本轮闭合

变异点：`story.py` `_assign_seq` 体首插计数派生（`ids.index(story_id)+1` / `len(records)+1`），持久账本路径不可达。

```
FAILED …::M4K1LedgerAcceptance::test_a6_archive_assigns_persistent_monotonic_seq
FAILED …::M4K1LedgerAcceptance::test_a6_delete_does_not_rollback_seq
FAILED …::M4K1LedgerAcceptance::test_a6_ledger_missing_rebuilds_without_rollback
FAILED …::M4K1LedgerAcceptance::test_a6_restart_survival
4 failed, 22 passed in 0.83s
```

`test_a6_restart_survival` 转红断言逐字（`-k restart_survival`）：

```
E           AssertionError: 3 != 4 : M4K1-A6 RED: 重启后 seq = 3 != 4 —— 账本未存活、靠扫描回推（删除后会回退）。
server\acceptance\test_m4k1_story_cloud_link.acceptance.py:1131: AssertionError
1 failed, 25 deselected in 0.27s
```

（与 acceptance-author §6 件 2 独立实测一致：seed 3 删中间 1 → 现存 2，计数派生 3 vs 账本续分配 4，可判别。）
还原复绿：`182 passed, 4 deselected`。expectedRed（canonical 更正版）**4/4** ✓；advisory `test_a6_rearchive_same_id_keeps_seq` **保绿**（计数派生偶然保同 id 序号——与 canonical A6 更正一致）

### MUT-5（post-wiring 幂等方向：StoryAccumulator.start 重复触发清空累积缓冲）

变异点：`story.py` `start` 内 `st.routing = "accumulate"` 前插 `st.accum = []; st.accum_chars = 0`。

```
FAILED …::M4K1SessionCtxAcceptance::test_a2_idempotent_convergence_with_start_recording
1 failed, 25 passed in 0.63s
```

还原复绿：`182 passed, 4 deselected`。**1/1** ✓

## 3. expectedRed 比对结论（builder 报告原样）

修订后快照下，canonical 更正版 mutationEvidence 的 expectedRed **7 组全部逐条命中、零多声明、零漏命中**；两条 advisory（front_wiring 源锚齿 / rearchive）实测保绿，与 canonical A5/A6 注记一致；A6 组首跑未闭环项（restart_survival）本轮闭合。

## 4. 终态自检（builder 三条 + planner 独立抽查）

builder 申报：`git status --short` 仅 server/**（实现 3 + 单测 1）；冻结件 sha256 == `5d4ebbf2…c625a287`；`grep -rn "MUT-\|and False\|if False" server/gateway/` 无残余。

planner 独立抽查（同日，代落时点）：sha256 / git status / grep 三项复跑与 builder 申报一致（结果见本批 commit 时段 pod 记录）。

## 5. 来源与责任链

- 变异设计与冻结期实测：acceptance-author（notes §4 / §9，第一方）。
- 实现与转绿后全套重放：builder（本报告 §1–§4 原始输出，第一方）。
- 代落与抽查：planner（硬约定③；本文件为证据汇总、非第二事实源——权威 = 各自第一方记录 + verifier 机械闸）。
