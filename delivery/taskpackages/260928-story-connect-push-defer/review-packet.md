# Review Packet · 260928-story-connect-push-defer

> integrator 收拢制品（2026-09-28，rigor=lite / riskTier=low），供 supervisor 机械合并门复核、高风险时供人类 review。本包 = M4-K1 真机缺陷止血单点摘除：摘除 `ws_gateway` 建连即推 `story_list`（三时点①）调用点一处，②③时点与 wire 形状零变化，firmware / 契约 / spec / prompt 零触碰。闭环口径：`FIX-A1`（负锚齿）+ `FIX-A2`（a3 齿时机语义修订，structural）。

## Change Size

- **实现面（单点摘除，1 文件）**：`server/gateway/front/front_app.py` **+7 / −6**——删除 ws_gateway 连接处理段 `_push_story_list(websocket)` 调用（含 try/except 包裹 + `story_list_connect_push_err` 日志），原位留注释块（摘除理由 + human 方向锚 `b036f554` + ①时机归 M4-K3 + `_push_story_list` 本体与②③调用点保留声明）。**唯一实现改动**。
- **保持在场（铁律，零改动）**：`_push_story_list` 函数本体、②（停录归档后）③（删除执行后）调用点、`story_list` wire 形状（`items` 最新 ≤16 窗口 + `total` 全量现存篇数）、Q7 relay 收紧、Q8 story floor、删除/账本/收尾/超时全链——`git diff` 实证仅上列一处 hunk。
- **冻结验收面**：`server/acceptance/test_m4k1_story_cloud_link.acceptance.py`（26 齿 + FIX-A1 新负锚齿 = 27 齿；sha256 = `78c4e1eac267678c6b51e76550839ba70a0a637198e56c710c6fb1ae79658457`，commit `73b5d3d`，本批只读）——FIX-A2 按**正常改验收通道**（human 方向授权，非 §6 tighten-only）修订 a3 齿时机语义并 append REVISION LOG。
- **Net diff**：工作树 `git diff --stat -- server/` = **1 file, +7 / −6**。
- **Scope**：单一子系统（server）、单调用点摘除，`crossEnd.counterparts = []`（exempt = 仅移除建连时推送调用点一处、零 wire 形状变化、对端固件零触碰）。
- **Split recommendation**：无需拆分（单点止血，无可切面）。

## Verification Evidence

- **Structural gates（scoped r3）**：`firmware-build` **exit 0**（真编译 build-pass，`xiaozhi.bin` 0x30f350B / app 分区余 11%——本包 firmware 零触碰，编译为回归护栏）；`contract-lint` **exit 0**（契约未漂移）。
- **Acceptance/unit（integrator 全量复跑，逐字）**：
  - `python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance` → **`193 passed, 4 deselected, 2 subtests passed in 2.17s`**（exit 0）。
  - 红→绿账 = 冻齿态 `1 failed / 192 passed / 4 deselected（+2 subtests）`（唯一红 = FIX-A1 设计态红）→ 实现后 `193 passed / 0 failed`（192→193 即 FIX-A1 一齿转绿，既有面零新增红）。
- **Bearing 机器件**：scoped 轮 fact = `verifier-evidence/rounds/r3.json`（`authority:bearing`、`observedGateExit:0`、`ok:true`），**7 闸全绿**（`taskpackage-required` / `firmware-build` / `contract-lint` / `acceptance-coverage`〔FIX-A1 functional、FIX-A2 structural 豁免〕/ `mutation-check` / `docs-sync` / `security`）。
- **轮次史（如实）**：r1 红（`taskpackage-required`——`contractRefs` 空，finalize 补正填 §1.③）→ r2 红（`acceptance-coverage`——FIX-A2 structural 标记形态，finalize 补正2 归 structural）→ **r3 全绿**。两轮红均为 TaskPackage 元数据口径、非实现/齿面红，补正全程留痕 taskpackage notes。
- **Mutation 重放（builder 源级第一方 + acceptance-author 冻结期三段，落 `evidence/mutation-replay.md`）**：源级变异（ws_gateway 连接段**加回**建连即推调用）→ `test_fix_a1_connect_path_emits_zero_story_list` **转红**（红名逐字 `AssertionError: 1 != 0 : FIX-A1 RED: 建连路径产生 1 帧 story_list 下行 —— 建连即推未摘除（human 方向锚 b036f554；①时机归 M4-K3 session_ctx 承接）`）+ 修订后 a3 齿保绿；还原 → `193 passed, 4 deselected, 2 subtests passed`。冻结期三段实测（摘除态模拟 GREEN 可满足性 / 原变异 RED / 还原设计态红）齐。
- **本批未跑 final 相位 `verify.mjs --phase=final`**（派单未含 final `round` 与 `invocationId`，与 K1 收口同口径——integrator 不自行生成轮次号）。

## Acceptance 逐锚结果

| 锚 | 内容 | 齿 | 结果 |
|---|---|---|---|
| **FIX-A1** | 建连即推摘除负锚：WS 建连路径零 story_list 下行帧（行为级，离线装机模拟建连流程断言设备腿帧数 = 0）；②③推送保持在场且 wire 形状不变 | `test_fix_a1_connect_path_emits_zero_story_list` | **绿**（设计态红 → 摘除转绿；源级变异回接转红闭环） |
| **FIX-A2**（structural） | a3 齿时机语义修订：移除建连时点断言、保留 `_push_story_list` 命名缝 + 归档后/删除后 wire 断言；REVISION LOG + human 方向锚留痕 | `test_a3_push_seam_and_wire_timepoints`（更名）+ `test_a3_list_stories_window_and_total_independent` | **绿**（语义修订记录齿；行为敏感由 FIX-A1 + 修订后 a3 齿承载） |

## exitCriteria 逐项复核

- **FIX-A1/FIX-A2 全绿 + 既有齿零新增红**：`193 passed / 0 failed`（integrator 复跑逐字）；红基线齿数变化（26→27，a3 更名）如实落账 `redBaselineResult`。**遵**。
- **铁律**：`_push_story_list` 函数本体与②③调用点不删不动（diff 实证）；契约面/spec 面/agent 定义零触碰（`git status --porcelain` 三者零输出）；K1 历史制品不改写（supersession 注记落本包 docs-delta §3，K1 件零输出）。**遵**。
- **docs-sync**：K1 docs-delta §4 H1「WS 建连」三时点措辞 supersession 注记（本包 docs-delta §3，含 §5.1.2 H2 连带短语）+ arch §5.2④ 修订后建议措辞（patch 形态，§4）；`docs-sync` 闸绿（r3 exit 0，`docsNoChangeReason` 逐字在案）。**遵**。

## demo 对账结论（锚 3 ①）

- **本包 demo 证据形态**（plan.md「demo 证据形态」逐字）= 「pytest offline：FIX-A1 负锚齿红→绿全程留痕 + 既有齿零新增红；无 live 面（真机复验归 K3 设备轮，M4K1-A7 口径不变）」。
- **对账（逐项一致性结论）**：① FIX-A1 红→绿全程留痕 ✓（红基线 `1 failed / 192 passed` 落账 + 源级变异回接转红 + 还原复绿，`evidence/mutation-replay.md` 第一方原始输出在案）；② 既有齿零新增红 ✓（192 既有齿全绿，`193 passed / 0 failed`）；③ **无 live 面** ✓——本包不产 live 证据，**设备侧真机复验归 M4-K3 设备轮**（M4K1-A7 口径不变）。
- **结论 = 一致**：本包交付的证据形态与 plan.md 声明逐项吻合；本包效果（按 KEY 开会话不再弹半截 P3 页）的**最终目击 = 真机复验，归 K3**，本包以 offline 负锚齿 + 全套绿承载 host 半。

## 未覆盖面申报（对照齿覆盖轴差集）

1. **设备侧实际停弹页面** — 性质：真机行为复验项（本包 offline 齿只证「建连路径零 story_list 帧」，不证设备渲染结果）；现状：未跑（单真机，K2/K3 未开跑）；去向：**真机复验归 M4-K3 设备轮**（M4K1-A7 口径不变）。
2. **三时点①恢复时机（设备进 P3 即见列表 + RAM 缓存）** — 性质：行为策略悬置面——摘除后设备进 P3 在②③触发前无列表推送；现状：human 方向锚明示止血优先、①归 K3 重裁（`crossPackageBehaviors` 承接条在案，K3 需一并设计「session_ctx 触发时点 vs 录音引导态」的冲突）；去向：**M4-K3**（planner/integrator 消费本包 docs-delta §3.3 指引）。
3. **K1 行为等价假设②的 live 复证** — 性质：已被本包证伪并处置（「建连」在真机拓扑 = 每会话，假设不成立）；现状：K1 review-packet 未覆盖面第 2 条闭环（本包 docs-delta §3.2 申报，K1 件不改写）；去向：**已闭环**。
4. **K2 板端消费面** — 性质：板端归属（story_page 收 story_list 即渲染/缓存的语义由 K2 设计承载）；现状：②③时点与 wire 形状不变、K2 消费面不受影响；去向：**K2**（`260924-m4-k2-p3-star-notebook-ui`，未开跑）。
5. **跨子系统 e2e / correlationId 链路验证** — 性质：结构性跨端；现状：本包 server 单端、`crossEnd.counterparts = []`、对端固件零触碰（exempt 在案）；去向：**不适用（none）**。

> **零缺口项明写 none**：跨子系统 e2e / correlationId 链路 = **none**（本包单点摘除、零 wire 形状变化、无跨端对偶面）。

## Detect Changes / 影响面核对

- **detect_changes**（project `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，`since=73b5d3d`）：changed_files 12（**含他会话脏件 6 面**——`delivery/changes/arch/INDEX.md`、`.changes-index-state.json`、`260926-arch-review-char-window-key-triggered-play/`、`docs/crq/tech/TECH-260928-school-merge-inprogress.md` 等，builder 已申报、本包不触碰不 stage）；**seed_symbols = 1**（`front_app.py` 摘除 hunk 内符号）、**impacted_total = 1** = `M4K1ConnectPushDeferAcceptance::test_fix_a1_connect_path_emits_zero_story_list`（本包新冻负锚齿自身）。**blast radius = 零既有符号受影响**——摘除的是无下游依赖的调用语句，`_push_story_list` 本体与②③调用点未动。
- 结论：单点摘除、影响面 = 1（新齿自身）；全量 `193 passed / 0 failed` 与之自洽。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （无 `minimal:` 标记） | — | — | no-trigger |

- `grep -rn "minimal:" server/gateway/front/front_app.py` **零命中**（exit 1）。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | Product/behavior | 三时点①摘除后**「设备进 P3 即见列表」语义暂缺**：在②（归档后）/③（删除后）触发前，设备 P3 列表无数据源。 | changed-code hunk `front_app.py:961-967`（建连调用摘除）+ human 方向锚 `b036f554`（止血优先）+ `crossPackageBehaviors` 承接条；heuristic = 止血换功能缺失的显式取舍 | **交 M4-K3 重裁**（①恢复时机随 `kidpal_session_ctx` 设备半一并设计，注意「session_ctx 触发时点 vs 录音引导态」冲突——plan.md 警示 + docs-delta §3.3 已留交接锚）。已获 human 方向授权，不阻断本包 |
| NIT | judgement-call | Worktree hygiene | 工作树含**他会话脏件 5 面**（`delivery/changes/.changes-index-state.json`、`delivery/changes/arch/INDEX.md`、`delivery/changes/arch/260926-arch-review-char-window-key-triggered-play/`、`docs/crq/tech/TECH-260928-school-merge-inprogress.md`、`delivery/taskpackages/current/verdict.json`）——与本包无关。 | `git status --porcelain` 实读 + builder 终态自检申报（mutation-replay §4）；heuristic = 并发会话残余混树 | 本包**不触碰不 stage 不还原**（派单排除清单）；处置归 planner / 他会话 owner |
| NIT | judgement-call | Test history | 冻结件 a3 齿经**正常改验收通道**更名并移除建连时点断言（`test_a3_push_seam_connect_and_wire_timepoints` → `test_a3_push_seam_and_wire_timepoints`）——K1 冻结齿语义被修订。 | 冻结件 REVISION LOG + commit `73b5d3d` + taskpackage `FIX-A2`（structural，human 方向授权、非 §6 tighten-only）；heuristic = 验收语义修订须留 human 锚（已留） | 无动作（通道合规、留痕齐）；K1 taskpackage `mutationEvidence` A3 条齿名 supersession 已由 `73b5d3d` 连带落账 |

> 结构闸 PASS/FAIL 不复述为本表 finding；本批**无 hard-violation finding**（全闸绿、零契约/spec 漂移、铁律全遵）。

## Five-Axis Review

1. **Correctness（是否满足声明 FIX-Ax，边界条件）**：FIX-A1 断言**真实行为**（离线装机模拟建连流程、断言设备腿收到 story_list 帧数 = 0——非「没报错就算过」）；②③在场与 wire 形状由修订后 a3 齿 + 既有 a4 齿承载；红→绿账清晰（1F/192P → 193P/0F，唯一红即 FIX-A1 设计态红）。
2. **Test quality**：mutation 打在**真实调用点**（源级回接建连即推 → 负锚齿转红 `1 != 0`；还原复绿）+ 冻结期三段运行期实测（可满足性 / 敏感性 / counterEvidence）双口径闭环；FIX-A2 structural 豁免定性在案（语义修订记录齿，与 M4K1-A7 同机制）。**强**。
3. **Contract/boundary safety**：`docs/contracts/**` 零触碰（`contract-lint` 绿）；wire 形状零变化（②③推送与 `story_list` 形状齿全绿）；`_push_story_list` 本体与②③调用点不删不动（diff 实证）；K1 历史制品不改写（supersession 双向注记落本包）。**无漂移**。
4. **Operability/security**：摘除 = 减面（少一次每次会话的无条件下行帧与日志噪声）；无新增输入/权限/secret 面；注释块保留根因与去向（后人可追溯）；零配置变更。**无安全残余需人类判断**。
5. **Maintainability/simplicity**：单点摘除、原位注释承载 why（比删注释更可维护）；未引入新抽象/新分支；K1 冻结齿语义修订走正常改验收通道并留 human 锚。Findings #1（①悬置）为产品面显式取舍、#2（脏件混树）为环境事实。

## Residual Risk

- **Known limitation**：① 「进 P3 即见列表」暂缺（Findings #1，human 方向授权的止血取舍，K3 重裁）；② 设备侧停弹效果未真机目击（未覆盖面第 1 条，归 K3）；③ 他会话脏件混树（Findings #2，本批排除在外）。
- **Human decision needed**：**无**（本包方向已由 human 锚 `b036f554` 裁定；①恢复时机的重裁归 K3 立项时点，非本批决策）。
- **decisionCoverage dropped/risk-accepted**：本 TaskPackage `decisionCoverage` 为空数组 `[]`，**无** `landsIn === 'dropped' && reason.startsWith('risk-accepted')` 条目——**none**。

## 回滚方式

- **一步回滚**：`git revert <本批 commit>`——恢复 ws_gateway 建连即推调用（含 try/except + 日志），既有齿面零破坏（该调用点即 `73b5d3d` 冻结基线态，FIX-A1 齿届时转红 = 如实反映建连即推回接，非假绿）。
- **本仓改动回滚面**：`server/gateway/front/front_app.py` 单文件回到 `73b5d3d` 态 + 丢弃本批 closeout/簿记新增件；无运行时/配置/数据面（`.seq.json` 账本、stories 目录零触碰）。
- **合并门**：本批 pending supervisor 机械闸复核；supervisor APPROVE + 前置升级闸放行后由 planner 主线程 `merge-driver --pod` 本地合并（integrator 不 push、不 merge）。