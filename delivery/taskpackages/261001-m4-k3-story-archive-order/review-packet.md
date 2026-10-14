# Review Packet · 261001-m4-k3-story-archive-order

> integrator 合并门前产出（2026-10-01）。供 supervisor 机械闸自动复核；`riskTier=high` 必升 human review。
> TaskPackage：`delivery/taskpackages/261001-m4-k3-story-archive-order/taskpackage.json`（② 单包 Full / rigor=full / sourceType=regression / riskTier=high / docsSyncRequired=true）。
> 本件不替代契约门，也不允许跳过可回滚路径（本地 merge）。

## Change Size

- **本包实现面（server，tracked，工作树 vs HEAD）**：**4 文件，+602 / −56**。
  - 实现 2 改：`server/gateway/front/front_app.py`(+62/−25) / `server/gateway/story.py`(+117/−4)。
  - 验收件改：`server/acceptance/test_m4k1_story_cloud_link.acceptance.py`(+297/−27，既有归档链齿改真机序 + 新增 SAOArchiveOrderAcceptance)。
  - 单测增：`server/tests/test_story_units.py`(+126)。
- **簿记面（delivery，非实现）**：taskpackage.json / pod-events.jsonl / verify-runs.jsonl / verifier-evidence/rounds/r1–r3.json / test-scope-final-r3.txt / docs-delta.md / review-packet.md / verdict.json / verification.json / current/ 投影 / progress.md 段。
- **Scope**：纯 server 面（`server/gateway/**` + 验收/单测）；固件 / 契约 / spec 零改**（`git status --porcelain docs/` 零输出）**。
- **Split recommendation**：无（120s sweep 分支 / 聆听形态 / 引擎侧抑制已分划 B2/B3，见未覆盖面申报）。

## Verification Evidence

- **Structural gates**：`firmware-build`（真编译，xiaozhi.bin 0x311920，app 分区余量 11%；本包不用固件、纯跑闸一致性）/ `contract-lint` / `change-manifest`（pre-cutoff 存量放行）/ `taskpackage-required`。
- **Final 相位全闸（10 闸）**：`verifier-evidence/rounds/r3.json` **ok:true / observedGateExit 0**（含 full `unit` + `acceptance`）。
  - 轨迹：scoped **r1**（红=`mutation-check` taskpackage 元数据缺口，invocationId d9448957-c7ad-4bde-9c4b-bc032ae264d8）→ scoped **r2** 全绿（invocationId a425a698-bd2a-4186-af85-543365431e0c）→ final **r3** 全绿（invocationId 9b8db103-a158-4676-8c63-a73e1f48f861）。
  - 机械核销：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261001-m4-k3-story-archive-order --phase final --round 3 --invocation-id 9b8db103-a158-4676-8c63-a73e1f48f861` → `{"ok":true,"factOk":true,"observedGateExit":0}`（exit 0）。
- **功能闸真跑（scope 圈定）**：`unit` 闸 **149 passed / 0 failed / 5 subtests passed**（7 文件，含 acceptance 件）；`acceptance` 闸 **77 passed / 0 failed / 5 subtests passed**（5 文件）。
- **全量锚（demo 对账用，integrator 真跑）**：
  - `server/acceptance` 全目录 → **133 passed / 0 failed / 4 deselected / 5 subtests passed**（修前红基线 = 12 failed / 124 passed）。
  - `server/tests` → **89 passed**。合并 server 全面 = **222 passed / 4 deselected / 5 subtests passed**。
- **Test-scope 清单落盘路径（Step 4）**：`delivery/taskpackages/261001-m4-k3-story-archive-order/test-scope-final-r3.txt`。
  - **圈定方法**：detect_changes（project `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，since `base/m4-star-notebook` / merge_base 60744cb）→ changed_files 25 / **seed_symbols 48 / impacted_total 76 节点**（server/gateway 44 · server/acceptance 20 · firmware/main 3〔名碰撞 loose 边，非真影响〕 · builtins 9）∪ taskpackage impactedTestsSeed[] ∪ 共享 seam（front_app / story 模块装载面）∪ 安全余量。
  - **范围内容**：本包验收件 `test_m4k1_story_cloud_link.acceptance.py` + 单测 `test_story_units.py` + blast radius 映射 `test_m4k3_record_session_live.acceptance.py` + 共享 seam 4 件（`test_m3k3_voice_schedule_entry` / `test_schedule_grid_restore` / `test_tech260928_school_start_wiring` / `test_reminder_units`）。
  - **圈定 vs 全 blast radius**：server 面 12 在职测试件圈 7；圈外 5 件（`test_char_meaning_merge` / `test_m3k1_schedule_producer` / `test_perf_k1_roundtrip_latency` / `test_prompt_v7_round_routing` / `test_schedule_producer_units`）不引 front_app/story，非影响面；archive 面（含 detect_changes 命中的 `archive/test_schedule_reminder.acceptance.py`）退役、闸 SKIP_DIRS 不收。
- **Security gate**：`securityReviewRequired=false` → `security` 闸绿。
- **Mutation check**：4 条 SAO mutationEvidence 逐功能 AC 敏感（见下表；由 builder 落 taskpackage，`mutation-check` 闸 scoped r2/r3 exit 0）。
- **Docs sync**：`docs-delta.md` 申报 spec 面零自改 + §5.2④ 判不需要文本补正 + 检出三处归属他项。
- **structural / suiteGates 豁免清单（closeout 消费）**：`acceptance-coverage` 闸输出 `structural: []` `suiteGates: []` —— **两清单均空，无豁免条目**（无 A10 型 / suiteGate 贴标）。

### Mutation 证据（逐 AC；引 taskpackage.mutationEvidence）

| id | 变异点 | counterEvidence 摘要 |
|---|---|---|
| SAO-A1 | `front_app.py:153` `_story_stop_on_listen_stop` 去掉 pending 挂起分支（accum 空直接 return None）→ 回旧「stop 帧立即归档」语义 | exit 1 / 9 failed / 125 passed；`test_sao_a1_...: AssertionError: 0 != 1`（真机序未产出归档） |
| SAO-A2 | `story.py:658` `mark_pending` 把 routing 置 chat（旧缺陷复现） | exit 1；`test_sao_a2_...: 'chat' == 'chat'`（空 stop 会话被打回 chat） |
| SAO-A3 | `front_app.py:267` `_story_timeout_sweep` 去掉 pending 释放环（挂起态永不超时释放） | exit 1；`test_sao_a3_pending_window_boundary: 'pending_stop' != 'chat'`（超时未释放） |
| SAO-A4 | `front_app.py:118` pending 归档路径停用剔词（`strip_finish=False`） | exit 1 / 3 subtest failed；`'我说完了' unexpectedly found in ...`（正文含尾随收尾词） |

> 注：mutationEvidence 行号（front_app.py:118/153/267、story.py:658）为 builder 填报时点锚，本包实现落点同源；`mutation-check` 闸绿 = 逐功能 AC 有对应 mutationEvidence。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| — | no new `minimal:` markers introduced | — | no-trigger |

> 本包实现面零新增 `minimal:` 标记（pending-stop 挂起态 + 剔词函数皆直接落、无投机简化）。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | judgement-call | Test quality | 改真机序后，`_story_finish_on_phrase` 的**正齿**（accumulate 态 stt 命中收尾词 → stop + 剔词）无直接断言——`test_a5_finish_phrase_triggers_stop_in_accumulate_only` 现走真机序（stop→pending→stt），归档由 pending 路径产出，`_story_finish_on_phrase` 因 routing 已回 chat 返 None。 | changed-code hunk：`test_m4k1...acceptance.py` a5 齿改序（diff :1194-1231）；`front_app.py` `_story_finish_on_phrase`（routing!=accumulate -> None）。剔词语义本身由 SAO-A4 齿 + `StripFinishWordsUnits` 单测覆盖。 | 无（该路径仅 auto 模式可达、归 B2/B3；剔词由同 `stop(strip_finish=True)` 单源保证，风险低。记未覆盖面 #4 供 B2/B3 补齿） |
| NIT | judgement-call | Correctness | pending 态下若引擎把同一段转写**分多帧 stt** 到达，仅**首帧**归档（首帧 feed+stop → routing 回 chat，后续帧被丢弃）；真机实录为单帧 turn（`c:/tmp/serial-live-repair-r2.log` 单条 `>>`），故实际风险低。 | changed-code hunk：`front_app.py` `_story_feed_from_stt` pending 分支（feed + stop 一次）；`story.py` `feed` 仅 routing∈{accumulate,pending_stop} 入 accum。 | 无（单帧 turn 为现役形态；若 B2/B3 改逐句 stt，须复评合并窗口语义） |

> 无 BLOCKING / SHOULD。spec/contract 面零 `hard-violation`（本批零契约 / spec 改动）。

## Five-Axis Review

1. **Correctness**：四条验收（SAO-A1..A4）逐条对上实现——真机序 `stop→pending→stt→归档`（SAO-A1/A2）打通；10s pending 超时释放（SAO-A3）；剔尾随收尾词（SAO-A4）。边界：重复 stop 去重（guard `routing!=accumulate`）、cancel 弹会话（`_sessions.pop` 连 pending 态同清，无泄漏）、accum 非空走立即归档（B2/B3 auto 兼容分支，既有 E1/a6 齿覆盖）。全量 133P/89P 双锚 + final r3 十闸全绿。
2. **Test quality**：断言真实行为（pytest 真跑 pump + 真落盘 MD + 真帧）；4 条 mutation 逐 AC 敏感（真红→真绿）；接受面承接既有 M4K1 归档链齿（改真机序，非新造）；`test_story_units.py` 补 `PendingStopUnits`/`StripFinishWordsUnits` 机判边界。**缺口如实**：见 Findings 两条 + 未覆盖面 #4。
3. **Contract/boundary safety**：契约零改、spec 零改（`docs/` 零写入）；无新依赖；改动限 front_app 泵接线 + story.py 状态机内，无跨层调用、无循环依赖；relay/wire 形状零变。
4. **Operability/security**：`securityReviewRequired=false`；新增日志 `story_stop archived=false reason=pending_stop` / `story_pending_archive` / `story_pending_timeout released=true` 可运维观测、含 corr_id、无敏感数据；`mark_pending`/`release_pending`/`pending_timed_out_sessions` 纯内存状态查询、零副作用、绝不抛异常。
5. **Maintainability/simplicity**：`pending-stop` 纳入 `StoryAccumulator` 状态机（`mark_pending`/`has_accum`/`pending_timed_out_sessions`/`release_pending` 四方法，与 `stop`/`cancel`/`timed_out_sessions` 互斥协调）；剔词抽单源 `strip_trailing_finish_words`（stdlib `str.rstrip`/`endswith`，无手写正则）；改动局部（deletion test：删 pending 分支 → 真机序内容丢失缺陷复现，复杂度不会转移，非空壳）。**兼容分支刻意保留**（stop 时 accum 非空 → 立即归档）使 B2/B3 auto 模式落地无需返工（plan §6 接口约定）。

## Residual Risk

- **Known limitation**：120s 自动收尾路径（`_story_timeout_sweep` 内 `stop()`）未传 `strip_finish=True` → 该路径归档正文不剔尾随收尾词（plan §3 划归 B2/B3，manual 模式下该路径 unable-to-fire）。逐句回显 / 说「我说完了」自动收尾 / 120s sweep 复活 / 收尾后引擎播报 = B2/B3 面。
- **Follow-up recommended**：① `docs-delta.md` §3.1 §5.2④「归档动作等转写到达」文本补正（advisory）随 spec 批并入；② §4.2 引擎侧抑制（收尾后播报）随 B3；③ §4.3 120s sweep 剔词随 B2/B3；④ 未覆盖面 #4 `_story_finish_on_phrase` 正齿随 B2/B3 补。
- **Human decision needed**：
  - **【升级项】真机复验（SAO-E3，human 持板，尚未跑）**：P3 按 KEY 停录 → 列表新增一篇 + 盖章；串口判据 = `story_key stop_rec archive_pend=1` 之后出现转写、front 不再报 `empty_accum`。**本包 host/验收面已绿，真机待 human 持板跑（如实标 pending，不虚报）**。
  - 本包 `riskTier=high` → supervisor 机械闸必产 **ESCALATE**，合并须 human 批。
- **decisionCoverage 中 `landsIn: dropped` + `reason: risk-accepted` 条目**：**无**（本包 `taskpackage.json` 无 `decisionCoverage` 键）。

## 未覆盖面申报（逐条「性质 + 现状 + 去向」）

1. **120s 自动收尾路径 `_story_timeout_sweep` 内 `stop()` 未加剔词** — 性质：同源缺陷但 plan §3 明划 B2/B3；本包只管 listen-stop / 收尾词两条路径的剔词。现状：manual 模式下该路径 unable-to-fire（accum 恒空、无 stt）；auto 模式（B2/B3）可达。去向：B2/B3 返工（plan §3/§6；builder 已标）。
2. **逐句回显 / 说「我说完了」自动收尾 / 120s 静默 sweep 复活** — 性质：依赖 B2/B3 的聆听形态变更（manual→auto）。现状：本包交付到「KEY 兜底收尾在当前 manual 模式可用」。去向：B2/B3（plan §6）。
3. **收尾后引擎仍把整段当一轮回答并播报（`<< …`）** — 性质：引擎侧抑制面（违反「收尾即盖章、AI 闭嘴」）。现状：实录 `<< 放假还有作业`。去向：B3 / Track A（plan §6）。
4. **`_story_finish_on_phrase` 正齿（accumulate + 收尾词 → stop + 剔词）无直接断言** — 性质：改真机序后该路径在 manual 模式不可达（a5 齿现走 pending 路径）。现状：剔词语义由 SAO-A4 + 单测覆盖，该 call-site 未单独断言。去向：B2/B3 补齿（见 Findings 1）。
5. **真机复验（SAO-E3）** — 性质：设备轮目击项（human 持板）。现状：**尚未跑，如实标 pending**。去向：human 持板跑（P3 按 KEY 停录 → 列表新增 + 盖章）。
6. **固件面 / 跨端 wire** — 性质：不适用（纯 server 面、不改任何 wire 消息、不触固件）。现状：零改动（`crossEnd.counterparts=[]`，exempt 述「聆听形态变更与引擎侧抑制逐句回复随 Package B 另立」）。去向：N/A。

**零缺口不适用**（本包有上述 6 条如实申报，非 `none`）。

## demo 对账行（anchor3：验收套件红→绿 + 缺陷本源真机实录）

> 本包 machine-demo 形态（按 roadmap §③ 本包上游条 = `260924-m4-k3-record-session-live` 的 live 域证据链）：host/验收套件红→绿 + 缺陷本源真机实录 `c:/tmp/serial-live-repair-r2.log`。

| 效果段 / 缺陷 | demo 证据 | 一致性结论 |
|---|---|---|
| 归档时序缺陷（内容彻底丢失）——修后真机序 `stop→pending→stt→归档` 产出恰一篇 | **machine-demo**：验收套件红基线 **12 failed / 124 passed** → 修后 **133 passed / 0 failed / 4 deselected**（`server/acceptance` 全目录，integrator 真跑）；单测 **89 passed**；final r3 十闸全绿 | **一致**：新齿 SAO-A1..A4 + 改序既有齿全绿，逐 AC 变异敏感 |
| 缺陷本源（归档早于转写 = 顺序缺陷） | **真机实录**：`c:/tmp/serial-live-repair-r2.log` t=131012 `MCP: story_key stop_rec archive_pend=1` → t=131062 `listening -> idle`（上行 listen/state=stop）→ t=132772 `>> …说完了`（转写 1.7s 后到）→ 旧实现归档瞬间 accum 必空 | **一致**：spine q8 三方复核口径（真因 = 顺序缺陷，非竞态、非「没 stt」）与本包修法对齐 |
| SAO-E3 真机复验（P3 按 KEY 停录 → 列表新增 + 盖章） | **无（human 持板，尚未跑）** | **pending**：host/验收面已绿，真机未跑，**如实标 pending、不虚报** |

## UI design-review 证据（文本）

- **不适用**：本包无 UI 工作（纯 server front/ story 语义修复，无新增版式 / 交互面）。TaskPackage 无 `uiux` block / `designReviewRequired`。

## pod-events 交叉引用

- `delivery/taskpackages/261001-m4-k3-story-archive-order/pod-events.jsonl` **在世**（4 条）：2 friction（`42a7c4ea` acceptance-author 超读 7 件 / `559ec927` launch-prep acceptance id 全角「：」致 acceptance-coverage 红）+ 2 dispatch-completed（`74c20a5f` acceptance-author / `1c0d716c` builder）。
- 上游 spine：`delivery/changes/intake/261001-m4-k3-record-session-live-repair.md`（q8 根因订正 / q9 本包立项）。
- post-cutoff 口径：不产 session-digest / loop-health / loop-metrics / pod-journal。
