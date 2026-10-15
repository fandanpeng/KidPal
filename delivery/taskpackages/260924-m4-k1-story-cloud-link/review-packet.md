# Review Packet · 260924-m4-k1-story-cloud-link

> integrator 收拢制品（2026-09-24），供 supervisor 机械合并门复核、高风险/critical 时供人类 review。本包 = server 单端故事数据链路（契约三触点对齐 + session_ctx 拦截 + story_list 推送 + 删除执行面 + seq 流水号账本 + 收尾词/120s 自动收尾），**firmware 零触碰**、OpenClaw 本体（ECS）架构零改动、prompt 面零触碰（v7 归 K3）。闭环口径：`M4K1-A1`~`A6`（offline 齿）+ `M4K1-A7`（live 全链 structural deferred，归 M4-K3 设备轮）/ `M4K1-E1`~`E3` exitCriteria。

## Change Size

- **实现面（3 文件改动 + 1 件新增单测）**：
  - `server/gateway/story.py` **+261 / −5**：`STORY_LIST_WINDOW=16`、`AUTO_STOP_SECONDS=120`、`is_finish_phrase`、持久流水号账本（`_load_ledger`/`_rebuild_ledger`/`_save_ledger`/`_assign_seq`，原子写 `<stories_dir>/.seq.json`）、`archive_story` 返回 record 增 `seq`、`list_stories` 最新 ≤16 窗口（`total` 仍全量现存篇数）、`story_text` 增可选 `seq`（additive）、`delete_story`（哨兵头校验 + 路径穿越拒绝 + 硬删 + 审计 + 幂等 no-op）、`StoryAccumulator.timed_out_sessions` + `last_activity`（start/feed 记录）。
  - `server/gateway/display_schema.py` **+24 / −0**：Q8 story 轻量形状 floor（`story_list.total` 在场须非负 int、`items` 须数组且条目三键 `{id,title,date}`；`story_text.seq` 在场须 int）；白名单零扩。
  - `server/gateway/front/front_app.py` **+149 / −21**：`_STORY_SINGLE_SOURCE_VIEWS` + Q7 relay 收紧（`build_display_tool_calls` 对引擎侧 `story_*` 块降级）、`_push_display_payload` / `_push_story_list`（front 自推单帧缝）、`_consume_kidpal_control`（上行泵拦截 `kidpal_session_ctx` / `kidpal_story_delete`）、`_story_finish_on_phrase`（累积态收尾词 → stop 归档）、`_story_timeout_sweep`（120s 超时 sweep）、三处 pump/轮询/建连接线。
  - `server/tests/test_story_units.py` **新增 22 单测**（builder 面单元证据）。
- **冻结验收面**：`server/acceptance/test_m4k1_story_cloud_link.acceptance.py`（26 齿 = 21 设计态红 + 5 现状绿，已提交 `388719d`，sha256 = `5d4ebbf28f13cf325483738430f62a60105f54176eba1d85fc15a355c625a287`，本批**只读**）+ `server/acceptance/M4K1-ACCEPTANCE-NOTES.md`。
- **Net diff（工作树，未含本收口批）**：`git diff --stat server/` = **3 files, +413 / −21**（另新增单测 1 件）。
- **Scope**：单一子系统（server）+ 单条链路（故事云端数据面），firmware / 契约 / spec / prompt 零触碰。
- **Split recommendation**：无需拆分。变更是「story.py producer 单源（窗口/账本/删除/决策）+ display floor + front 接线」一条内聚链，切开会破坏「producer ↔ wire ↔ 机械门」的不变式。

## Verification Evidence

- **Structural gates（scoped 轮）**：`contract-lint` **exit 0**（契约未漂移）；`firmware-build` **exit 0**（真编译 build-pass，`xiaozhi.bin` 0x30e840B / app 分区余 11%——本包 firmware 零触碰，编译为回归护栏）。
- **Acceptance/unit（integrator 全量复跑，逐字）**：
  - `python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance` → **`182 passed, 4 deselected in 4.15s`**（exit 0）。
  - 构成 = 上游锚 134@5e862d0 + 冻结 26 齿全绿 + builder 新增单测 22。零新增红。
- **Acceptance-coverage / mutation-check（scoped 轮）**：`acceptance-coverage` **exit 0**（A1–A6 functional、`structural:["M4K1-A7"]`、`suiteGates:[]`）；`mutation-check` **exit 0**（A1–A6 各有 `mutationEvidence`，A7 structural 豁免）。
- **Docs sync**：`docs-sync` **exit 0**（`docRefs` 三锚走 `docsSyncReason` 分支；详见 `docs-delta.md`）。`security` **exit 0**（`securityReviewRequired:false`）。
- **Bearing 机器件**：scoped 轮 fact = `verifier-evidence/rounds/r1.json`（`authority:bearing`、`producedBy:verify.mjs`、`observedGateExit:0`、`ok:true`），7 闸全绿（`taskpackage-required` / `firmware-build` / `contract-lint` / `acceptance-coverage` / `mutation-check` / `docs-sync` / `security`）。**本批未跑 final 相位 `verify.mjs --phase=final`**（派单未含 final `round` 与 `invocationId`）；如需 final fact，请 planner 指定轮次与 invocationId 后补跑——本包 integrator 侧不自行生成轮次号以免造伪 round fact。
- **Mutation 重放（builder 第一方 + acceptance-author 冻结期，落 `evidence/mutation-replay.md`）**：七组（MUT-A1~A6 + MUT-5）逐组转红 → 还原复绿 `182 passed, 4 deselected`；canonical 更正版 expectedRed 逐条命中、零多声明、零漏命中；两条 advisory 齿实测保绿（见 Findings #2/#3）。

## Acceptance 逐锚结果（M4K1-A1~A6）

| 锚 | 内容 | 齿（冻结件） | 结果 |
|---|---|---|---|
| **A1** | 契约门齿（三触点 + Q7 relay 收紧 + Q8 story floor） | `a1_story_list_total_must_be_nonneg_int` / `a1_story_list_items_three_keys_required` / `a1_story_text_seq_must_be_int_when_present` / `a1_relay_rejects_engine_side_story_blocks_q7`（+ 正样本 `a1_story_list_new_shape_passes_floor` / `a1_story_text_seq_optional_passes_floor` + 不过宽守卫 `a1_relay_keeps_other_views_unchanged` + 落文锚 `a1_contract_carries_three_touchpoints`） | **绿** |
| **A2** | session_ctx 拦截（整条不下透 + `source_page=story` 切累积态 + 幂等收敛 + 省略向后兼容） | `a2_session_ctx_intercepted_not_forwarded` / `a2_source_page_values_and_backward_compat` / `a2_idempotent_convergence_with_start_recording` | **绿** |
| **A3** | story_list 三时点推送（建连/归档后/删除后）+ items ≤16 窗口 + total 全量 | `a3_list_stories_window_and_total_independent` / `a3_push_seam_connect_and_wire_timepoints` | **绿** |
| **A4** | 删除执行（哨兵头 + 硬删 + 越权/穿越拒绝 + 审计 + 幂等 + 重推对账） | `a4_delete_executes_and_repushes` / `a4_delete_nonexistent_id_idempotent_repush` / `a4_delete_rejects_traversal_and_no_sentinel` | **绿** |
| **A5** | 收尾词检测 + 仅累积态消费 + 120s 纯决策 | `a5_finish_phrase_detection` / `a5_finish_phrase_triggers_stop_in_accumulate_only` / `a5_timeout_120s_decision`（+ 源锚代理 `a5_front_wiring_references_timeout_decision`） | **绿**（源锚代理齿见 Findings #2） |
| **A6** | 持久 seq 账本（max+1 / 删除不回退 / 重启存活 / 缺失重建不倒退）+ story_text.seq + total 口径分离 | `a6_archive_assigns_persistent_monotonic_seq` / `a6_restart_survival` / `a6_delete_does_not_rollback_seq` / `a6_ledger_missing_rebuilds_without_rollback`（+ advisory `a6_rearchive_same_id_keeps_seq`） | **绿**（advisory 见 Findings #3） |
| **E1** | 既有语义零偏移守卫 | `e1_existing_story_semantics_zero_offset` | **绿**（must-stay） |

## exitCriteria 铁律逐项复核

- **M4K1-E1（新增 offline 齿全绿 + 既有冻结语义零偏移）**：`182 passed / 0 failed / 4 deselected`（integrator 复跑逐字）；`e1_existing_story_semantics_zero_offset` 在档（archive/read/list/story_text/StoryAccumulator V31 四回退 + 有界）；`archive_story` / `StoryAccumulator` 既有语义经 mutation MUT-6/MUT-7 反证非空过。**遵**。
- **M4K1-E2（铁律）**：① 契约 diff 仅经契约门通道落地（四分类预审 + human 签字在案，A1）——本批 `git status --porcelain docs/contracts/` 零输出、`contract-lint` 绿；② 不改 spec（`docs/requirements.md` / `docs/architecture.md` 本批零写入）、不改 agent 定义/gate；③ `kidpal_story_delete` 为 §四端云消息、**不进 display view 白名单**（`display_schema.py` 白名单零扩——floor 新增仅对已存在的 `story_list`/`story_text` view 加形状约束，`kidpal_story_delete` 无 view 条目）；④ OpenClaw 本体（ECS）架构零改动；⑤ prompt 面零触碰（v7 bump 归 K3）。**遵**。
- **M4K1-E3（live 半 deferred 挂账 + docs-sync 面在案）**：`M4K1-A7` structural deferred 挂 K3 设备轮（`acceptance-coverage` 闸 `structural:["M4K1-A7"]` 认账）；docs-sync 面（arch §5.2④/§5.4/§5.1.2/§7 收口）随 integrator 收口义务在案——**本批因无 `docs/architecture.md` 写权，以 patch 形态 delta 落 `docs-delta.md §4`**，合并后由 human spec 批落地（见「未覆盖面申报」第 13 条）。**遵**。

## demo 对账结论（锚 3 ①）

- **本包 demo 证据形态（roadmap ③ 本包条目逐字）** = 「python -m pytest server/acceptance/ offline 齿（拦截/推送/删除/收尾/超时/账本模拟）+ 契约门 evidence 制品（十节骨架）；live 全链复证 deferred 归 K3 设备轮」。
- **本包效果段**（roadmap ③「完成后人能看到什么」）= 云端成为数据权威：连网/存档/删档都推 P3 列表数据、P3 开的会话云侧识别记录语境、说「我说完了」或静默 120 秒自动收尾归档带只增不减 seq 发回、删除云端真删并即时重推列表对账、契约三触点同批收口。
- **对账（逐项一致性结论）**：
  1. **offline 齿套实跑输出** = `182 passed / 0 failed / 4 deselected`（三跑一致：acceptance-author 4.46s / planner 132.93s〔机器负载波动〕/ builder 4.64s / integrator 4.15s）——拦截（A2）/推送（A3）/删除（A4）/收尾（A5）/超时（A5）/账本（A6）六轴 offline 齿全绿，与效果段各半一一对应。
  2. **契约门 evidence 制品** = `architecture-gate-evidence.md`（九节骨架 + Status LANDED，landing commit `5e862d0` / `3a9ff24`）+ CCP v2 + `evidence/validate-display-samples.md`，与效果段「契约三触点同批收口」一致。
  3. **形态差异（如实）**：效果段末句「live 全链复证 deferred 归 K3 设备轮」= 本包 **不产 live 证据**（`M4K1-A7` structural deferred）——故 demo 形态为「offline 全绿 + 契约门 CLOSED 路径声明 + A7 挂账声明」，**非** live 目击；live「双入口开录→收尾三式→归档盖章 seq→列表推送→删除同步」的最终目击证据归 M4-K3（`M4K3-A6`）。
  - **结论 = 一致**：本包交付的证据形态与 roadmap ③ 声明的形态逐项吻合，无「声明 live 而未跑」的落差（roadmap 明写 live 归 K3），无「声明 offline 而缺齿」的落差（六轴齿全在世且全绿）。

## 未覆盖面申报（对照齿覆盖轴差集：`acceptance[]` + 契约条目 + roadmap 效果段）

素材 = acceptance-notes §5 八条缺口/自由度 + §6 三条行为等价假设 + `M4K1-A7` deferred + gate evidence C1–C5 + C4 处置，逐条一行：

1. **120s 前端计时循环接线** — 性质：真计时循环 offline 不可验（只冻纯决策 `timed_out_sessions` + 源锚代理）；现状：`_story_timeout_sweep` 已接线（轮询循环同节拍），但「每 tick 调决策并 stop + 下发」为**未验行为等价假设**（notes §6 假设③）；去向：live 全链归 **M4-K3**（`M4K3-A6`）。
2. **WS 建连时点（三时点①）真建连面** — 性质：真 ws 建连 offline 不跑；现状：`_push_story_list` 行为齿直调该缝 + 源锚（`ws_gateway` 引用）；行为等价假设②「ws_gateway 调 `_push_story_list` ≙ 建连即推」显式化；去向：**M4-K3** live 复证。
3. **Q7「引擎侧来源」判据** — 性质：offline 只钉 relay 路径（`build_display_tool_calls`）对 `story_*` 降级，不断言真实引擎帧来源标记；现状：行为等价假设①「relay 路径 = 引擎侧来源面」显式化（front 自推走 `_push_story_list`、不经 relay 天然豁免）；去向：**M4-K3** live（真引擎帧）。
4. **floor 深度 = 形状 floor** — 性质：不钉逐字段深度（item 额外键、id 非空、date 格式）——契约未定义处 = 实现自由；现状：只钉 `total` 类型 / `items` 三键 / `seq` 类型；去向：**明示接受不覆盖**（无齿义务，非缺口）。
5. **删除审计正文格式** — 性质：审计格式不钉，只钉「拒绝路径日志含 offending id 或拒绝类 token」；现状：`delete_story` 各拒绝路径 `log.warning(... reason=...)` 留痕，齿断言留痕存在；去向：**明示接受不覆盖**。
6. **账本内部 JSON 结构** — 性质：机制级冻结（文件名 `.seq.json`），内部结构不钉；现状：行为级断言 seq 语义（重建/回退方向）；去向：**明示接受不覆盖**。
7. **窗口双层钉** — 性质：非缺口（producer + wire 双层已钉）；现状：`a3_list_stories_window_and_total_independent`（producer）+ `a3_push_seam…`（wire 帧）双层齿在世；去向：**已覆盖**。
8. **`delete_story` 拒绝路径是否重推** — 性质：契约未明定拒绝路径重推行为；现状：齿只钉返回 `bool` + 拒绝不误删/不崩；去向：**明示接受**（契约未定义处）。
9. **`M4K1-A7` live 全链复证** — 性质：structural deferred（单真机不两轮撞车）；现状：未跑；去向：**M4-K3 设备轮**（`M4K3-A6`）。
10. **gate evidence C2（板端窗口消费：items ≤16 + 计数恒信 total + seq 盖章消费）** — 性质：板端归属；现状：未做（K1 无板端面）；去向：**K2**（`M4K2-A2/A5`）。
11. **gate evidence C3（板端 delete 发送原语 + 隐式 ack 超时重发 + live）** — 性质：板端 + live；现状：未做；去向：**K2**（发送/重发）+ **K3**（live，`M4K3-A6⑥⑦`）。
12. **gate evidence C5（prompt v7 三处 v6 钉行文面收口 + carrier 翻转）** — 性质：prompt 版本化（ADR-001）；现状：v6 产块指令被 Q7 relay 条款**闸口事实失效**，文面未收；去向：**K3**（`M4K3-A4`）。
13. **C4（arch §5.2④/§5.4/§5.1.2/§7 收口）** — 性质：**结构性不可达**（integrator 无 `docs/architecture.md` 写权，见 Findings #1）；现状：delta 以 patch 形态成文于 `docs-delta.md §4`（五处 hunk）；去向：**human spec 批**（合并后落地，可与 `ARQ-260918` 四处同批搭载）。
14. **C1（Q7 relay 收紧 + Q8 story floor 机械执行面）** — 性质：本包义务；现状：**已完成**（A1 齿 + MUT-A1 组 + 本收口 commit，gate evidence §6 C1 已翻 done）；去向：**已完成**。
15. **跨子系统 e2e / correlationId 链路验证** — 性质：结构性跨端；现状：本包 server 单端、firmware 零触碰；`crossEnd.counterparts` 三件（`application.cc::HandleToggleChatEvent` / `mcp_server.cc::kidpal_story_list_parse` / `story_page.c::kidpal_story_page_render`）为**对端消费面**（K2/K3 承接），本批不改；去向：**不适用（none）**——本批无跨端对偶面需本批证，跨端 live 链归 M4-K3。

> **零缺口项明写 none**：跨子系统 e2e / correlationId 链路 = **none**（本包 server-only、firmware 零触碰、无本批需证的跨端对偶面）。

## Detect Changes / 影响面核对

- **detect_changes**（project `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` 池树专属索引，`since=388719d`）= changed_files **9**、seed_symbols **69**、**impacted_total 83**（`server/acceptance` 62 / `server/gateway` 16 / `server/tests` 5）。索引**可见**本次 diff（含工作树改动），**非** `N/A (worktree)`——核对有效。
- **影响面自洽**：83 个 impacted 节点**全在 `server/**`**，无 firmware / 跨子系统节点。其中：
  - `server/gateway`（16）= 本包自有改动面（`front_app._push_display_payload`/`_push_story_list`/`_consume_kidpal_control`/`_story_finish_on_phrase`/`_story_timeout_sweep` 等新符号）+ 既有**未改**消费方（`reminders.AlertDispatcher.tick`、`reminders.due_entries/read_pending/validate_alert_entry`、`story.StoryAccumulator.stop/recover/_ensure`）。
  - `server/acceptance`（62）= 本包新冻齿 + 既有齿（含 `archive/` 下已退役不收集齿如 `test_story_p3` / `test_g_relay_fix`）；`server/tests`（5）。
- **关键共享符号 trace_path 抽查**：`display_schema.validate_display`（本包改了它——Q8 floor 新增）inbound 调用方 **5** 个 = `front_app._push_display_payload` / `front_app.adaptation_dryrun` / `front_app.build_display_tool_calls` / `front_app.command` / `reminders.validate_alert_entry`，**全部在 `server/gateway/**`**（本包自有面或既有未改调用方）。floor 为 **additive 收紧且仅对 `story_list`/`story_text` 两个 view 分支**——非 story view 调用路径行为不变（由 `a1_relay_keeps_other_views_unchanged` + M4K3/既有 schedule 齿绿灯实证）。**跨层/回归风险为零**。
- 结论：本包 blast radius 限定在 `server/gateway` 故事面 + 其机械门；既有 62 个 acceptance 节点与 5 个 unit 节点零回归（全量 `182 passed / 0 failed`）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （无 `minimal:` 标记） | — | — | no-trigger |

- `grep -rn "minimal:" server/gateway/story.py server/gateway/display_schema.py server/gateway/front/front_app.py server/tests/test_story_units.py` **零命中**（exit 1）——本包无 `minimal:` 简化债务标记。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | Governance / write-scope | **C4 类义务对本包角色结构性不可直达**：integrator 的 scope-map allow 含已废弃的 `docs/systems/**`（AGENTS.md 明载 docs 已平铺、不再套 `systems/<sys>/` 层），却**不含 `docs/architecture.md`**——故「arch 面收口」为 docs-sync 声明的义务却无写权路径。 | changed-artifact 锚：`.claude/scope-map.json` `agents.integrator.allow`（本批实读）+ `AGENTS.md`「docs 平铺」句；heuristic = 声明义务 vs 授权写路径不匹配（义务落在无写权的角色上，只能降级为「delta 待批」） | **交 loop-retro / human**：delta 已成文 `docs-delta.md §4`（合并后 spec 批落地）；scope-map 修订方向 = integrator allow 撤 `docs/systems/**`、按需补白名单文档（**本 Pod 不自登记 ARQ、不动 `docs/crq/**`**，处置归 loop-retro/human） |
| NIT | judgement-call | Test strength | `a5_front_wiring_references_timeout_decision` 为**源锚代理齿**（文本子串判据、docstring 提及即满足）——实测对实现变异**不敏感**（MUT-A5 下保绿）。 | `evidence/mutation-replay.md §2 MUT-A5` + `M4K1-ACCEPTANCE-NOTES.md §5 缺口 1 / §9.8`；heuristic = 源锚代理齿 ≠ 行为齿（120s 前端计时循环 offline 不可验，见未覆盖面第 1 条） | **本次不收紧**（范围帽）；收紧评估归 loop-retro 素材（若收紧 → 需引入可注入 tick seam）。不阻断合并 |
| NIT | judgement-call | Test strength | `a6_rearchive_same_id_keeps_seq` 在「计数派生」标本 mutation 下**偶然保绿**（同 id 重写计数派生亦不涨 → 变异下仍绿）= advisory。 | `evidence/mutation-replay.md §2 MUT-A6` + `M4K1-ACCEPTANCE-NOTES.md §9.8`；heuristic = 该齿对「持久 vs 计数派生」单轴判别力弱（同 id 场景两向同值） | **本次不收紧**（范围帽）；收紧评估归 loop-retro 素材（补「同 id 重写 + 他篇删除」组合 scenario 可增判别力）。不阻断合并 |
| NIT | judgement-call | Operability | `_story_timeout_sweep` 对命中会话把 `story_text`/`story_list` **广播到全部在线设备腿**（`_alert_legs` 遍历），非按会话归属腿定向。 | changed-code hunk `server/gateway/front/front_app.py::_story_timeout_sweep`（`for device_ws, _engine_ws in list(_alert_legs)`）；heuristic = MVP 单设备场景无实害，多设备场景会向无关腿推 story 内容 | **明示接受（MVP 单设备）**；多设备化时按会话归属腿定向（可另开后续） |

> 结构闸（`taskpackage-required`/`contract-lint`/`acceptance-coverage`/`mutation-check`/`docs-sync`/`security`）PASS/FAIL 不复述为本表 finding；本批**无 hard-violation finding**（全闸绿、零契约/spec 漂移）。

## Five-Axis Review

1. **Correctness（是否满足声明 `M4K1-Ax`，边界条件）**：A1–A6 六锚 offline 齿全绿、E1 零偏移守卫绿。边界条件齐备：`total` 字符串/负数/浮点被真门拒；item 缺键/非对象被拒；`seq` 字符串/浮点被拒；路径穿越（`..`/分隔符）+ 无哨兵头杂物（`README.md` 等）+ 不存在 id 均拒绝且不误删目录外文件；`delete_story` 畸形 id 不崩（`invalid_type` → False）；120s 边界（119/120/130s，`>=120` 计入）；账本缺失/损坏重建不倒退；空转写 `archive_story` 抛 `ValueError`。一次真缺陷（`test_a6_restart_survival` fixture 缺 `feed`）经 `§6 tighten-only` 修订闭合（见「Residual Risk」）。
2. **Test quality（是否断言真实行为）**：数据路径用**真实落盘**（tempdir + 真 `archive_story`/`delete_story`/真 `.seq.json` 读写），front 面用**真实 front_app 模块**（fastapi stub 离线装载）+ 真实 pump 协程 + I/O 双桩，断言到达设备的 mcp 帧的 **wire 形状**（`{"view","data"}` 解回 JSON 逐字段）——非「调用了就算过」。mutation 打在实现接线（MUT-A1~A6 + MUT-5 逐组转红 + 还原复绿）。**强**（两条源锚代理/advisory 弱齿已如实申报，见 Findings #2/#3）。
3. **Contract/boundary safety（是否漂移契约/spec）**：`docs/contracts/**` 零触碰（`contract-lint` 绿 + git 零输出）；Q8 floor 为对已存在 view 的 additive 收紧、白名单零扩（`kidpal_story_delete` 不进 display view）；`docs/requirements.md` 零写入；`docs/architecture.md` 未改（C4 delta 待批，如实申报）。**无漂移**。
4. **Operability/security（错误/权限/输入/secret/日志）**：删除路径的输入校验 = 类型 + `..`/分隔符 + `resolve()` 落目录内双保险（拒越权 + 拒穿越）+ 哨兵头校验，全路径审计留痕（`log.warning/info` 带 `corr_id`）；账本 I/O 失败**不崩归档**（`_save_ledger` 吞 `OSError` 记日志，已分配 seq 仍返回）；WS 入口鉴权不变（front 唯一公网入口、accept 前强制鉴权）——`kidpal_story_delete` 消费不新增鉴权面（已在鉴权后的通道内）；无 secret 落档、无 shell 注入面。**无安全残余需人类判断**（见 Residual Risk）。
5. **Maintainability/simplicity（局部/易懂/无过早抽象/无重复实现）**：seq 账本 = 单一权威（`story.py` 内 `_load/_rebuild/_save/_assign` 四函数，原子写先例沿用 `pending.json`）；窗口语义单源在 producer（`list_stories`），front `_push_story_list` 只接线不重算；Q7/Q8 机械门各一处（relay 降级 + floor）；`timed_out_sessions` 纯决策与接线（`_story_timeout_sweep`）分离，便于 offline 测。Findings #1（写权）与 #4（sweep 广播）为结构性/运维 minor。

## C4 arch 收口处置（本批不可写）

- 详见 `docs-delta.md §4`：五处 hunk（§5.2④ `:213` / §5.1.2 front 网关行 `:185` / §5.1.2 故事归档行 `:192` 连带 / §5.4 故事行 `:261` + 新增 `.seq.json` 行 / §7 在办 `:341`），每处带上下文行号与原文/新文逐字对照，可直接 `git apply`。
- 处置声明：**合并后由 human 主线程 spec 批落地**（`docs/crq/tech/ARQ-260918-spec-face-3page-reconcile.md` 四处可同批搭载）；落地后回填 `architecture-gate-evidence.md §6` C4 = done。**本批不写 `docs/architecture.md` / `docs/requirements.md` / `docs/contracts/**`**（治理批通道专属）。
- `architecture-gate-evidence.md §6`：**C1 已 pending→done**（证据 = 冻结齿 + MUT-A1 组 + 本收口 commit）；**C4 保持 pending + 追加处置注记**；C2/C3/C5 不动（归 K2/K3）。

## Residual Risk

- **Known limitation**：① C4 arch 收口 delta 待 human spec 批落地（`docs-delta.md §4`）——未落地前 `docs/architecture.md` 仍写收口前形态（§5.4【链路载体待契约】、§7 在办①②④、§5.2④ 旧口径）；② `M4K1-A7` live 全链未跑（deferred 归 M4-K3）；③ 两条源锚代理/advisory 弱齿（Findings #2/#3）；④ `_story_timeout_sweep` 广播到全部在线腿（Findings #4）。
- **历史（已闭合）**：builder 实现期 `Red Flag` 上报 `test_a6_restart_survival` 冻结齿 fixture 缺陷（`start` 后缺 `feed` → 空累积下 `stop` 返 `None`，齿两向皆红 = 不可满足）→ planner 重派 acceptance-author 走 `§6 tighten-only` 修订闭合（纯 additive 0 删除行 + 原变异重放转红 `3 != 4` + sha256 核销链 `61c0501…→1be76af9…→5d4ebbf2…`；`agent-escalation c1096ab9` → `resolved 1be9997e`）。**非未决风险**，如实留痕供 retro。
- **Follow-up recommended**：① K2（板端窗口消费 + seq 盖章 + 删除确认子态/发送原语，`M4K2-A2/A3/A5`）；② K3（prompt v7 文面收口 + live 全链目击，`M4K3-A4/A6`）；③ loop-retro：源锚代理齿收紧评估 + scope-map integrator allow 修订（Findings #1/#2/#3）。
- **Human decision needed**：① **C4 delta spec 批落地**（与 ARQ-260918 同批；本批合并后）；② scope-map integrator allow 修订方向（撤 `docs/systems/**`、按需补白名单文档，Findings #1）——均非阻断本包合并，归 human/主线程。
- **decisionCoverage dropped/risk-accepted**：本 TaskPackage `decisionCoverage` 字段为空数组 `[]`，**无** `landsIn === 'dropped' && reason.startsWith('risk-accepted')` 条目——**none**。

## 回滚方式

- **本仓改动回滚**：本 Pod 分支未 push、未 merge；弃用 = 丢弃本收口 commit 即可。回滚面 = `server/gateway/story.py` / `server/gateway/display_schema.py` / `server/gateway/front/front_app.py`（三件回到 `388719d` 态）+ 删除新增 `server/tests/test_story_units.py`；closeout 制品与 pod 簿记为新增文件，无既有面破坏。
- **契约面回滚**（如需退回契约落地批）：`git revert 5e862d0` 单批全还原（契约 diff + `story.py` 三处钉行 + CDQ 三行 + ADR-007 + evidence 同 commit）——**与本包实现回滚须同批**（本包实现消费契约 §1.3③④/§四的冻结形状）。
- **运行时面回滚**：账本 `<stories_dir>/.seq.json` 为新增文件，删除即退回「无 seq」态（既有 `archive_story` 行为）；无 ECS / OpenClaw 配置改动（`git status` 实证本包不动 `server/deploy/**`）。
- **合并门**：本批 pending supervisor 机械闸复核；supervisor APPROVE + 前置升级闸放行后，由 planner 主线程 `merge-driver --pod` 本地合并（integrator 不 push、不 merge）。