# Review Packet — 260911-display-first-m1（PKG-4 显示先行 M1 · 围栏闭合即发）

> integrator 产出 · 2026-09-12 · 分支 `dev/260911-display-first-m1`（base = main，HEAD = `438f85a` 260911-screen-unification 合并夜收口）。
> 供 supervisor 合并门复核。本包 `riskTier=high`（`gateProfile=[frozen-acceptance-revision, device-round, contract-drift]`），照常触发 supervisor 升级人类。
> 交付本体：引擎补丁 `kidpal_display_strip.py` 把「块闭合」从 `finish()` 提前到 `feed()` 内（`DisplayFenceStateMachine.__init__(on_block_closed=None)` 闭合回调 API + `feed()` 内块闭合即同步发射 + `finish()` 内 unclosed 尾恰发一次 + `finish().blocks` 只作观察镜像）+ `server/deploy/engine-patch/connection.py`（构造点注入回调、单块包装 `display_blocks_message([block])` 立即发射、`finish()` 处撤销重复发射）。
> **合并门必读**：本包无 BLOCKING finding。承重 final fact `r5` **非全绿**（`ok:false`），唯一红 = `unit`/`acceptance` 两闸同源的 `test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（SMK-A4）——**在册 existing-red-inventory R1**（owner=260829-smoke-e2e，`server/acceptance/_artifacts/smoke-e2e/screen.md` 证据件缺口），与本包零相关、零新增红。host runner 基线 4 条红（WSA-A6:830 / WSA-A4:663 / P2-A1:571 / VRT-A2·E2:878）均为在册既存红。故合并门需**按红证据 ESCALATE 通道**提 human 裁定「留红如案、红转交各自 owner」，本包自身面（scoped 7 闸 + 6 新齿红→绿 + mutation 2 条敏感自证 + 契约复核 CONFIRM_NO_CONTRACT_CHANGE）全绿。

## Change Size

- Files changed（repo 侧，`438f85a..工作树` 收拢后，`git diff --numstat` 实测）：
  - **实现面 = 2 文件 +37/−13（净 +24）**（`git diff --stat` 实测）：
    - `server/deploy/engine-patch/kidpal_display_strip.py`（+23/−5，净 18 行）：`DisplayFenceStateMachine.__init__(self, on_block_closed=None)` 闭合回调构造参数；`feed()` 内 fence 闭合（closer ``` 被消费）的瞬间同步调用一次 `on_block_closed(block)`；`finish()` 内 unclosed 尾恰发一次；`finish().blocks` 保留为观察镜像。模块头 frozen offline contract 增补 `__init__` 语义段（L18-24 adaptive source）。
    - `server/deploy/engine-patch/connection.py`（+14/−8，净 6 行）：`_emit_display_block(block)` 单块发射回调（`display_blocks_message([block])` + `asyncio.run_coroutine_threadsafe(self.websocket.send(...), self.loop)`）；`strip_machine = DisplayFenceStateMachine(on_block_closed=_emit_display_block)` 构造注入；`finish()` 处 `_display_blocks` 下划线化、删除流末重复发射段。
  - **冻结验收件修订 1 文件 +239/−0**：`server/acceptance/test_g_relay_fix.acceptance.py`——REVISION LOG（L185-243，additive 语义扩展声明 + 3 条修订留痕）＋ `DisplayFirstM1Acceptance`（L1192-1362，6 新齿：`test_m1_closure_callback_api_present` / `test_m1_feed_emits_closed_block_immediately` / `test_m1_multiple_blocks_emitted_in_order` / `test_m1_unclosed_tail_emitted_exactly_once_at_finish` / `test_m1_finish_semantics_preserved_with_callback` / `test_m1_connection_wires_closure_callback_into_stream`）。
  - **closeout 制品 5 件**（本次积分收拢落 `changes/dev/260911-display-first-m1/`）：`review-packet.md` / `docs-delta.md` / `existing-red-inventory.md` / `verdict.json` / `verification.json`。
  - dev-loop 执行 ledger/evidence（`.claude/taskpackages/260911-display-first-m1/`）：`taskpackage.json`（活文件）、`plan.md`、`input-brief-{acceptance-author,builder}.md`、`pod-events.jsonl`、`evidence/DF-E1-contract-recheck.md`、`evidence/INCIDENT-260912-git-config-zeroed.md`、`verifier-evidence/rounds/r{1,2,3,4,5}.json`、`verify-runs.jsonl`。
- Scope: `subsystem=server+firmware` 但本次落地**纯 `server/deploy/engine-patch` 内部面**（`docs/contracts/**` 与 `docs/spec.md` **零改动**——`git diff --name-only -- docs/` 空输出，contract-lint 闸 PASS）；`server/gateway/`（front）与 `firmware/` **零改动**（`git diff --name-only -- server/gateway/ firmware/` 空输出）。
- Split recommendation: **不拆**——「strip machine 闭合回调 API + connection 接线」是单一「围栏闭合即发」职责收敛（OQ2=M1 裁定本体），两文件同属 engine-patch 单点剥离状态机 + 其唯一接线的内聚面，拆开任一面都断「单点剥离 + 唯一发射信道」语义。

## Verification Evidence

> **承重 fact 声明（诚实口径）**：final completed-only round = **r5**（`phase:"final"`，identity = changeId+phase+round+invocationId），
> `verifier-evidence/rounds/r5.json`：`ok:false`、`observedGateExit:1`、`authority:"bearing"`、`producedBy:"verify.mjs"`、
> `invocationId da096ccb-165e-47a9-8027-46d95990e790`、`startedAt 2026-09-12T03:08:26.179Z / completedAt 03:08:52.195Z`。
> `ok:false` 的**唯一**成因 = `unit`/`acceptance` 两 full 闸同源红 `test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（SMK-A4，在册 existing-red-inventory R1）；scoped 7 闸**全绿**。此为本包「留红如案」的 machine 记录，非本包自身面红（见 §Residual Risk / §合并门须知）。
> `current/verdict.json` 为同一 `producedBy:"verify.mjs (script fs-write)"` 的投影（non-bearing compatibility projection），closeout 判断以 r5 fact 为准；本相位 `verifier-evidence-check.mjs` 已核验 fact 有效（`ok:true`/exit 0，`factOk:false` 但 evidence 合法）。

- **verifier 逐轮收敛轨迹（r1→r5）**：
  - **r1**（inv `1d5c6d27-…`，scoped，`ok:false`）：唯一红 = `acceptance-coverage`——`missingCoverage:["DF-A3","DF-A4","DF-A5","DF-A6"]`。根因 = 四条目是「真机/部署/跨包承载」类结构齿，未在 taskpackage `acceptance[]` 标 `{id, kind:"structural"}`。逐条判定见下方「structural 豁免审计」；planner finalize 补对象标记（acceptance-author/vult 无断言语义改变）。
  - **r2**（inv `ed7b7503-…`，scoped，`ok:false`）：唯一红 = `mutation-check`——`missingSensitive:["DF-A1","DF-A2"]`。根因 = taskpackage `mutationEvidence.perAcceptance` 尚未登记（acceptance-author 冻结轮已自证，finalize 时补登）。
  - **r3**（inv `8d257a87-…`，scoped，`ok:false`）：唯一红 = `docs-sync`——`docsSyncRequired=false must include docsSyncReason`。根因 = finalize 尚未补 `docsSyncReason`。
  - **r4**（inv `8b8d509f-…`，scoped，`ok:true`）：七闸全绿（`observedGateExit:0`），firmware-build 严格模式真编译（`build-pass`，xiaozhi.bin 0x2c2e00）。
  - **r5**（inv `da096ccb-…`，final）：scoped 7 闸全绿 + `unit`/`acceptance` 两 full 闸红（唯一 SMK-A4）。`unit` 计数 `1 failed/168 passed/6 skipped/45 deselected`、`acceptance` 计数 `1 failed/131 passed/6 skipped/45 deselected`，与本包零新增红。
- **r5 逐闸（实测 ms）**：taskpackage-required 186 / **firmware-build 9499（`build-pass` 真编译，非降级——ninja `[1/5]…[5/5]` 全跑、`xiaozhi.bin 0x2c2e00`、`0xad200 bytes (20%) free`）** / contract-lint 150 / acceptance-coverage 162（acceptance DF-A1..A6 全在册、`structural:["DF-A3","DF-A4","DF-A5","DF-A6"]`、`suiteGates:[]`、files=[test_g_relay_fix.acceptance.py]）/ mutation-check 165 / docs-sync 164 + security 162。
- **6 新齿红→绿（本包自身面核心证据）**：冻结轮红基线实测 `6 failed / 14 passed / 4 skipped`（6 M1 新齿全红：5 红归 kidpal_display_strip.DisplayFenceStateMachine 闭合回调 API 未实现 + 1 红归 connection.py 回调接线不存在；既有 GRF 14 齿全绿）；builder 后 `20 passed / 0 failed / 4 skipped`（4 skipped = GRF-A5 live 齿 conftest 机械 skip，夜间不执行 live，待明早真机轮）。`test_g_relay_fix.acceptance.py` 共 24 个 `def test_`（14 GRF + 6 M1 + 4 GRF-A5 live），20 个可离线执行齿全绿。
- **mutation 敏感自证 2 条（acceptance-author 冻结轮，落 `taskpackage.mutationEvidence.perAcceptance`，均 `sensitive:true` / `turned red`）**：
  - Mutation A（deferred emission）：假实现接受 `on_block_closed` 但把发射推迟到 `finish()`——复刻「API 在但早发没做」缺陷类；`test_m1_closure_callback_api_present` 绿（仅 API 在场桩）、`test_m1_feed_emits_closed_block_immediately` 红——「closer 到达那次 feed() 内即发射」时序被真实钉住。
  - Mutation B（markers-kept payload）：假实现按正确时机在 feed 内发射但载荷保留 ```display 包裹；`test_m1_feed_emits_closed_block_immediately` 与 `test_m1_multiple_blocks_emitted_in_order` 双红——载荷裸化/顺序断言非空过。变异命名纪律遵守（on_block_closed 变异 = 直接移除/换名，未以原字面为子串，KR-A3 教训）。
- **DF-E1 reviewer-first 契约复核（阶段 0，`gateProfile: contract-drift`）**：`evidence/DF-E1-contract-recheck.md` 落盘于 acceptance-author/builder 派发之前，总裁决 `CONFIRM_NO_CONTRACT_CHANGE`——M1 不改 `docs/contracts/interface-contract.md` 的 view schema／mcp 信封／工具白名单／校验权威四个面；`kidpal_display_blocks` 为引擎→front 内部面（契约全库 grep 零命中）；M3 禁采判定维持；时序契约未钉板成立。
- **Security gate**：r5 `ok:true`（`securityReviewRequired:false`）。本包无新公网入口、无新 secrets；`on_block_closed` 回调本身同步、但回调体 `_emit_display_block` 用 `asyncio.run_coroutine_threadsafe(..., self.loop)` 线程安全投送回事件循环，阻断 `feed()` 主链的风险为零（`self.loop` 复用既有发射面的同一引用）。
- **Docs sync**：r5 `ok:true`（`docsSyncRequired:false`）。判定与逐条理由 = 同目录 `docs-delta.md`（canonical 零改动 + `docs/contracts/**`/`docs/spec.md` 零触碰 + DF-E1 CONFIRM_NO_CONTRACT_CHANGE）。**不改 docs/contracts/**。
- **Closeout**：本 phase 跑 `node .claude/scripts/closeout-check.mjs`（默认只读，不传 `--update-index`）→ 结果见 §Closeout。
- **影响面核对（step 11b）**：Pod worktree 内 diff 对索引不可见 → `detect_changes` 如实标注 N/A (worktree)，以 `git diff --stat`（实现面 2 文件 +37/−13）＋ 关键共享符号抽查替代。① `git diff --name-only -- docs/` 空、`-- server/gateway/` 空、`-- firmware/` 空——改动只落 `server/deploy/engine-patch/**`（kidpal_display_strip.py / connection.py）与冻结验收件，零触碰契约面/front/固件；② `DisplayFenceStateMachine` 的 `feed`/`finish` 语义仅在本机内改（`trace_path feed` 单点剥离状态机 + connection.py 唯一接线），不新增跨文件调用者，改动只影响预期符号。

## Simplification Debt

> 本包实现面 = 2 文件 + 1 冻结验收修订，**无新抽象、无新依赖、无 `minimal:` 标记**（`grep` 全仓 `minimal:` 命中 0 处为本包新增）。故本表 **无条目、无 no-trigger 项**——诚实声明「无简化债务」而非伪造一条。本包删的是冗余（`connection.py` 流末 `if display_blocks:` 整块发射段改由回调信道唯一承载，`finish().blocks` 降级为观察镜像、删除重复发射的 `side_msg` 发送段），是 `delete` 语义、不留债。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | judgement-call | final 闸红（留红如案，跨包） | **SMK-A4 final 闸红（不伪造不代修）**：r5 `unit`/`acceptance` 两闸唯一红 = `test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（`server/acceptance/_artifacts/smoke-e2e/screen.md` 缺失 → 无屏上文字证据）。与本包引擎侧发射时机改动零相关（本包不产 display-line 证据） | Basis: final r5 fact `ok:false` stdout（`AssertionError: SMK-A4 RED: …screen.md missing`，`test_smoke_e2e.acceptance.py:422`）+ existing-red-inventory R1（owner=260829-smoke-e2e，`[evidence]` 族先行红）；heuristic: `[evidence]` 族红非本包冒烟面，不越权代采 | 留红如案、提 human 按红证据 ESCALATE 通道裁（红转交 owner=260829-smoke-e2e：重新采集一次真实屏上文字证据或裁「物理冒烟证据以当时记录为准」）。本包不代采不代修 |
| FYI | judgement-call | host runner 基线红（留红如案，跨包） | **host 基线 4 条红（不伪造，引用 PKG-2 inventory）**：WSA-A6:830 / WSA-A4:663 / P2-A1:571 / VRT-A2·E2:878，全部为在册既存跨包红，owner 各归原包；本包实现面零 firmware diff，host 面结论由构造不变承载（不重跑 host 全量） | Basis: `260911-key-route-a1`/`260911-screen-unification` existing-red-inventory R2-R5 同源 + `260829-voice-roundtrip`/`260903-device-ws-switch`/批次③ render-pages owner 登记 + `git diff --name-only -- firmware/` 空；heuristic: firmware 零改动 ⇒ host matrix 结果与 PKG-2 合并基线同态 | 留红如案，基线消解走独立批（launch 明令），不塞本包 |

**Kind 说明**：两行均 `judgement-call`（无一条是本包 repo diff 引入的 hard-violation；两行均为在册留红如案）。无 BLOCKING → 不停合并门；但 final fact `ok:false` 触发红证据 ESCALATE（见 §合并门须知）。

## Five-Axis Review

1. **Correctness（对 PROPOSAL §2.2 M1 裁定 + 冻结离线契约语义）**：逐条兑现「围栏闭合即发」——`feed()` 内 fence 闭合（closer ``` 被消费的瞬间）同步调用一次 `on_block_closed(block)`（`kidpal_display_strip.py:114-115`）；`finish()` 内 unclosed 尾恰发一次（`:138-139`）；不变量「一次完整流后回调载荷序列 == `finish().blocks`（同序同集）」由 `test_m1_finish_semantics_preserved_with_callback` 的 `assertEqual(fired, blocks)` 钉住；`connection.py` 构造注入 + 单块包装 `display_blocks_message([block])`（`connection.py:1168-1178`）+ finish 处撤销重复发射（`connection.py:1454-1456` 注释 + `_display_blocks` 下划线）。6 齿实跑 PASS（`json.loads` 断言回调载荷与 `finish().blocks` 逐字节同形），是「语义兑现」的直接证据，非「跑了没报错」。DF-A4（设备侧语音中渲染）承载于已并入 base 的 PKG-2 冻结齿（`screen_unification.acceptance.c` b1/b3/s2），M1 只改引擎侧发射时机、设备仲裁规则同一套，「中途到达」=「提前到达」，无新行为面——见 REVISION LOG [3]（`test_g_relay_fix.acceptance.py:227-241`）。
2. **Test quality**：6 新齿是**行为级实负载齿**而非「API 在场」浅齿（`json.loads` 断言裸载荷、顺序断言 `fired == blocks`、播报面 `assertNotIn("```", speech)` 负向校验），并配 2 条 mutation 敏感自证（Mutation A deferred-emission / Mutation B markers-kept-payload 全 `turned red`），证齿不是「总是绿的摆设」。既有 GRF 14 齿全绿 = 修订语义不放松自证（REVISION LOG [1] 逐字声明既有齿断言语义零放松）。
3. **Contract / boundary safety**：`docs/contracts/**` 与 `docs/spec.md` 零触碰（`git diff --name-only -- docs/` 空 + contract-lint PASS + DF-E1 `CONFIRM_NO_CONTRACT_CHANGE`）。M3 禁采全程有效（`taskpackage.decisionCoverage`「OQ2-M3-禁采」`landsIn: exitCriteria:DF-E1`）。本包改的是引擎内部「何时发射」而非「发什么格式」——每块仍经 front `build_display_tool_calls` 逐块机械校验+`kidpal/display` mcp 信封下发，设备 wire 三面（view schema / mcp 信封 / 白名单）不变。
4. **Operability / security**：`on_block_closed` 回调默认 `None`（`kidpal_display_strip.py:54`）→ M1 前行为 byte-identical、向后兼容（既有齿 `sm_cls()` 零参数构造继续成立）；回调体走 `asyncio.run_coroutine_threadsafe` 投送回事件循环、不阻塞 `feed()` 主链（`connection.py:1174-1177`，`self.loop` 复用既有发射面引用）；无新公网入口/凭据/依赖。
5. **Maintainability / simplicity**：additive API（默认 None）+ `finish().blocks` 降级为观察镜像，删除流末重复发射段是删冗余（`delete` 语义）；无新抽象、无 `minimal:` 标记债务（§Simplification Debt 已声明）。「每块一条 side-channel message」与设备 transport「多块多帧、no batching」天然对齐（`test_g_relay_fix.acceptance.py:27-28` TRANSPORT OF RECORD），比旧「finish 一次弥发全部 blocks」更贴近设备 mcp 帧型（数字 id 逐信封）。

## S2 Exploratory Scan

TaskPackage 未声明 `s2ExploratoryRequired` / `s2ExploratoryGoals` ⇒ 本相位**未执行** S2 official exploratory scan，无 runId、无 structured `runResult`、无 `knowledgeSuggestion` 待决项。required-by-risk 侧注记：`riskTier=high` 但 planner 未开该面；本包的风险面是「发射时机提前引入的多次侧通道到达对 front 幂等/顺序/回执的影响」（DF-E1 边界注记 3 已在契约面判其为实现面自检、每块已带独立数字 id）与「部署面 engine-patch 在位核验」（DF-A5，human 明早收口轮），非探索式攻击面，planner 已用 DF-E1 reviewer-first 契约复核 + 6 行为齿 + mutation 2 条等价覆盖，本 packet 如实记录「未声明 ⇒ 未执行」，不以 builder debug run 冒充 official evidence。

## UI design-review 证据

本包无 UIUX work（TaskPackage 未声明 uiux 字段；改动为引擎侧 `server/deploy/engine-patch/**` 内部面 + 冻结验收件修订，不新增 UI 面、无 viewport/a11y/design 触及）。设备侧 display 上屏路径只经既有 front `build_display_tool_calls`/`kidpal/display` relay + 已并入 PKG-2 的 `kidpal_render_node` layer 仲裁消费，无新 UI 面。无 UI finding。

## Residual Risk

- Known limitation / 留红如案（均非本包引入、owner 已登记）：
  1. **SMK-A4 final 闸红**（existing-red R1，owner=260829-smoke-e2e）：`screen.md` 证据件缺口，本包不代采、不伪造，提 human ESCALATE 裁。
  2. **host 基线 4 条红**（WSA-A6:830 / WSA-A4:663 / P2-A1:571 / VRT-A2·E2:878，existing-red R2-R5）：在册既存跨包红，owner 各归原包（260903-device-ws-switch / 批次③ render-pages / 260829-voice-roundtrip），本包零 firmware diff，基线消解走独立批。
- Pending（owner 恒 human 明早收口轮，本包不出真机/部署动作）：
  - **DF-A3（时延实测，structural）**：真机「单击问课表」display 页上屏不晚于 TTS 首段后 0.5s（kidpal-live-smoke-debug 三日志源按 correlationId 对齐）——明早 human 持板。
  - **DF-A5（部署合规，structural）**：部署窗口走修正后 apply-xiaozhi-overlay.sh（批次①已并）+ engine-patch 在位核验（启动标记行 `kidpal display-strip patch active` + docker inspect Mounts）+ front 时序复核结论在案——明早 human 部署窗口。
  - **DF-A6（workstream 收口真机轮，structural）**：§5 五条收口标准并集对账（本包承载上屏时延项，其余复核各 owner 包已验项）——明早 human 持板。
- Human decision needed：
  - **红证据 ESCALATE**：final fact r5 `ok:false`（SMK-A4 留红如案），提 human 裁「接受本包合并（红转交 owner=260829-smoke-e2e / 独立批基线消解）或先修 R1 再合并」——由 supervisor-verdict-gate 机械闸 + planner 前置升级闸（`pre-merge-escalation-gate.mjs`）承接，本 integrator 不自判。
  - **risk-accepted 台账**：`taskpackage.decisionCoverage` 六条 decision 的 `landsIn` 全部指向 acceptance/exitCriteria，**无 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头条目** → 本包无 risk-accepted 待追认项（决策「OQ2-显示先行=M1」「OQ2-M3-禁采」「冻结验收面-M1必触」「reviewer-first 契约复核」「PKG-4-部署前置」「workstream级收口标准」六条均承接为 acceptance/exitCriteria，非 dropped）。

## Closeout

`node .claude/scripts/closeout-check.mjs`（本 phase 实跑，默认只读、**不传 `--update-index`**）→ 见 §Closeout 落笔时实跑输出。`requiredFiles:["docs-delta.md","review-packet.md"]` 两件 blocking 制品在本目录在场；`existing-red-inventory.md` 为非 blocking 但必交（红证据 ESCALATE 豁免件，supervisor 机械闸按形状检查）。
advisories（advisory-only，不阻塞按 SSOT）：① `verdict.json`/`verification.json` 投影·advisory（non-bearing，承重 = r5 fact）；② `pod-retro.md` 为 planner 收尾相位产物（非本相位职责，Q5=warn）；③ 若 loop-retro pending 计数触达阈值则 advisory 提示。
post-cutoff 不产 session-digest / loop-health / loop-metrics / pod-journal；`pod-events.jsonl` 在场（advisory 满足，含 1 条 human-decision：`ac5476e8` overnight-deputy-execution-mode —— DF-A3/DF-A6 真机轮 + 合并批准全部留明早 human 在场窗口）。

## Commit 冻结（合并门复核的对象）

- 功能提交 = builder/acceptance-author 已落工作树（`kidpal_display_strip.py` + `connection.py` + 冻结验收件修订），积分收拢提交（本次）把 closeout 制品 5 件 + dev-loop ledger（pod-events / verify-runs / r1-r5 facts / evidence / input-briefs / taskpackage 活文件 / 指针副本）收口。commit 前按 AGENTS.md Hard Rule 用 `git diff --cached --stat` 核对；**不 stage `.codebase-memory/**`（机器再生面常驻脏，Pod 分支禁止私带快照 commit）、不 stage 其它包的 pod-events/taskpackage**。本包 diff 只抬本包 `server/deploy/engine-patch/**` + `server/acceptance/test_g_relay_fix.acceptance.py` + `.claude/taskpackages/260911-display-first-m1/**` + `changes/dev/260911-display-first-m1/**` 与 current/ 指针副本。
- 可回滚路径：全部交付物为 feature 分支 `dev/260911-display-first-m1` 上的本地 commit（**未 push、未动 main**）；回滚 = 分支级 revert（`kidpal_display_strip.py`/`connection.py` 是 additive 单文件面，revert 即恢复「finish() 一次弥发全部 blocks」旧态）。main 零暴露。
- 合并相位：supervisor 机械闸（`supervisor-verdict-gate.mjs`）裁断 → planner 前置升级闸（`pre-merge-escalation-gate.mjs`）→ 放行后 `merge-driver --pod` 本地合并（本包 mergeOrder 20，末位）。本 integrator 不自行判定通过、不执行 merge、不 push、不动 merge-driver。

## 合并门须知（短清单）

1. 无 BLOCKING finding；两行 FYI 均为留红如案、无一阻断本包自身面。
2. 承重 fact = **r5**（phase final，scoped 7 闸绿 + unit/acceptance red = SMK-A4 在册 R1，`ok:false`）；supervisor 复核读 `verifier-evidence/rounds/r5.json`。
3. 6 新齿红基线 6 failed → 全绿 20 passed/0 failed；实施面零 front/固件/docs diff，零新增红。
4. 冻结面（`docs/contracts/**`、`docs/spec.md`、`*.acceptance.*` 及其它包冻结件）git 零触碰（contract-lint PASS + `git diff --name-only -- docs/` 空）。
5. **红证据 ESCALATE**：final fact `ok:false` 触发人类裁决通道（风险面 = SMK-A4 + host 4 红留红如案，owner 均已登记），由 supervisor 机械闸 + planner 前置升级闸承接，riskTier=high 照常升级。
6. human 待办（明早收口轮）：DF-A3 时延实测 / DF-A5 部署合规 / DF-A6 收口真机轮；SMK-A4 留红如案裁定（owner 260829-smoke-e2e）；host 基线 4 红走独立批消解（launch 明令）。均不阻塞本包 merge。