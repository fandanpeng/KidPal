# PROPOSAL · 260904-gov-review-escalate-merge-approval

**状态：FROZEN v1.0 · 2026-09-05 · 已立项（human 立项 ack 2026-09-04 + §4 OQ1–OQ3 全裁 (a)）；R1「需修订」（F1/F2）已修订 → R2（2026-09-05）GREEN。主线程凭「立项 ack + GREEN」标 FROZEN，packet-binding 自本标记起生效。本行行首无引用前缀是为命中升级闸 readGovAnchors 机锚正则（`/^\*\*状态：FROZEN v\d+\.\d+/m`）**
> **承载 ARQ**：`ARQ-260902-escalate-merge-no-human-path`（单根全收：闸无人批输入口 + planner 规程断线两个缺口一次补齐；其余 ARQ 不碰）。
> **review 轨**：Track B 常驻轻路径（SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。不触契约面、非跨系统结构变更，无升级判据命中。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T4）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（触强制执行层 G3 闸脚本并新增一条放行路径；但放行以 human 事件机械核验为硬前置、provenance 缺失永不豁免、默认无 flag 行为零变更、self-check 电池正反例覆盖，向严默认保持）。
> **review profile**：G3（`.claude/scripts/` 闸脚本 2 件 + `.claude/agents/planner.md` 规程）+ G2 文档面（references/commands/ARQ 队列）。G1/契约面 = 零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：GREEN 结论前必须在场（最终期限 FROZEN）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/scripts/merge-driver.mjs
.claude/agents/planner.md
.claude/references/dev-loop.md
.claude/references/pod-coordination.md
.claude/references/escalation-playbook.md
.claude/commands/build-feature.md
.claude/references/code-review-gate.md
docs/ARCH-REVIEW-QUEUE.md
changes/arch/260904-gov-review-escalate-merge-approval/PROPOSAL.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：supervisor 机械闸判 ESCALATE（high/critical）且 human 明确批准合并后，存在一条机械可走、全程留痕的合并路径——升级闸凭 `--human-approval-ref <eventId>` 核验 pod-events 里的人批事件后放行，merge-driver 透传执行并把凭据写进 merge commit——不再依赖主线程手工 `git merge` 一次性绕过。

**现状差距**：前置升级闸只认 `verdict=APPROVE`，ESCALATE 包在人批后仍永不自动合并（Pod S 已实证一次，手工绕过，commit `e33278d`）；planner.md 对 ESCALATE 只写「停人类」，人批之后谁执行、如何留痕无定义。方向 B 重切已把本项钉为留根 10：G 包（`260903-gateway-directionB`）与 WS 包（`260903-device-ws-switch`）均 high + 跨系统 + 安全轴，大概率双双 ESCALATE，缺口即将第二次、第三次命中。

**非目标（本批不做）**：
- 不改 `supervisor-verdict-gate.mjs` 的判级逻辑（ESCALATE 语义本身是对的，缺口在判完之后的路）。
- 不改例外清单其余项（契约面人签字制 / off-main / blocked 一般态 / provenance 核验均不动）。
- 不改 pod-events 事件写入侧（复用既有 human-decision 记录习惯，只把 `payload.decision="approve-escalated-merge"` 从事实惯例钉成标准取值）。
- 不建 Track B 治理档的人批豁免（治理合并有 FROZEN + GREEN + landing-verdict 机锚已足，无人批豁免概念）。
- 不动 `docs/contracts/**`、不动 `docs/DEV-GOVERNANCE.md`（其 § supervisor 合并门行「高/critical 升级 human」语义在本批后仍成立，见 §3 分类表）。

## 1. 问题与根因（地面真值）

### P1 · 升级闸与 merge-driver 均无人批输入口（机械层缺口）

证据（起草期实核，2026-09-04）：

- `.claude/scripts/pre-merge-escalation-gate.mjs:126-127`：dev-loop 档 `riskTier === 'high' || 'critical'` → push BLOCK 理由；`:133-134`：`verdict !== 'APPROVE'` → push BLOCK 理由。两处均无豁免分支。
- 同文件 `:22` 用法行实核：`node .claude/scripts/pre-merge-escalation-gate.mjs <changeId> [--source-ref <ref>] [--project-dir <dir>]`——无 approval 类参数。
- `.claude/scripts/merge-driver.mjs:617-632`（Step 1b）：合并前以 `execFileSync` 复跑同一升级闸，拦下即 `setPod blocked` 返回——人批后重跑结果不变。
- 实测：`node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` → `{"selfCheck":"pre-merge-escalation-gate","pass":10,"fail":0,"total":10,"ok":true}`（2026-09-04）——现有电池无任何 approval 用例。

根因：ESCALATE 的处置语义只建模了「停下来给人看」的前半程，「人看完之后放行」从未建模进机械层。

### P2 · planner 规程在人批节点断线（规程层缺口）

证据（2026-09-04 实核行号与原文）：

- `.claude/agents/planner.md:87`（单 Pod 模式）：「BLOCK → 退 fixer/planner；ESCALATE → 停人类。」——句号之后没有下文。
- `.claude/agents/planner.md:91`（多 Pod 模式）：「ESCALATE → `node .claude/scripts/portfolio-reconcile.mjs --set-pod <changeId> --status blocked`；停 human。」
- `.claude/agents/planner.md:109` 与 `:137` 同形态：「只有 ESCALATE 才停人类 review + merge」「ESCALATE 停在人类合并门」。
- 周边 prose 同形态（§3 census 全枚举）：`dev-loop.md:103`、`pod-coordination.md:99`、`escalation-playbook.md:15`、`build-feature.md:31`。

根因：同上——流程图在「human 批准」节点没有出边。

### P3 · 已实证后果：一次性手工绕过（Pod S，2026-09-02）

- `changes/dev/260829-smoke-e2e/supervisor-verdict.json` 实核：`verdict: "ESCALATE"`、`riskTier: "high"`，理由 `cross-system:impl=.gitignore,main,acceptance,shim.py` + `security-axis-diff:auth`——机械判据正确。
- `.claude/taskpackages/260829-smoke-e2e/pod-events.jsonl` 实核：human 批准事件 `86c73079-89f7-41c2-8c88-38e5a679b09d`，`eventType: "human-decision"`、`actor: "human"`、`payload.decision: "approve-escalated-merge"`（承接特殊放行事件 `c2682f8c`）。
- 机械路径不存在 → 主线程手工合并。实核 `git show e33278d --stat`（2026-09-04）：commit message 自承「human 批准特殊放行 + ESCALATE 合并,事件 c2682f8c/86c73079;一次性人批操作,机械闸无人批入口已记治理缺口」。
- 另查实：merge-driver 已有同形先例参数 `--human-approved-exemption <ref>`（`merge-driver.mjs:40-43`、`:677-699`，acceptance 门红豁免，shape-only 校验）——「human token flag + 预存制品核验」是本仓既有范式，本批沿用并加强（核验到具体事件而非仅制品在场）。

## 2. 改法（T1–T4）

### T1 · `pre-merge-escalation-gate.mjs` 加人批核验口 `--human-approval-ref <eventId>`

改法（对 P1）：

1. **CLI**：入口参数解析段（现 `:267-276` 形态）新增 `--human-approval-ref <eventId>`；用法行（`:22`）补一行：
   `node .claude/scripts/pre-merge-escalation-gate.mjs <changeId> --human-approval-ref <eventId>  # ESCALATE 人批放行（核验 pod-events human-decision 事件）`。
2. **核验函数**（新增纯函数，可注入 reader，对齐既有 `readSupervisorVerdict` 形态）：`resolveHumanApproval(dir, changeId, eventId)`——读 `<dir>/.claude/taskpackages/<changeId>/pod-events.jsonl`；该文件不在场时回退读 pods 记录 `.git/devloop/pods/<changeId>.json` 的 `worktreePath` 下同路径（合并前事件可能只在 worktree 侧落盘；`worktreePath` 字段实核存在于 `.git/devloop/pods/260829-smoke-e2e.json`）。逐行 JSON 解析，找 `eventId === <eventId>` 的记录，核验五要素全过才算 ok：`eventType === "human-decision"`、`actor === "human"`、`payload.decision === "approve-escalated-merge"`、记录 `changeId === 本 changeId`、`schemaVersion === 1`。任一不符 → `{ ok:false, reason:<点名哪要素不符> }`。
3. **evaluate 豁免语义**（仅 dev-loop 档）：当 `verdict === 'ESCALATE'` 且 provenance 完整（`mechanicalSupervisorGate.ran && producedBy` 在）且 `humanApproval.ok` → 不推 `:126-127`（riskTier）与 `:133-134`（verdict 非 APPROVE）两条理由，放行输出注明 `human-approval verified (eventId=...)`。**blocked 态联动（OQ2 已裁 (a)）**：同一豁免条件成立时，通用 3 的「change 处 blocked 态」理由亦不推——该 blocked 是 ESCALATE 停等人类的驻留标记，人批事件即其 resolve；off-main/契约/provenance/quick-check 等真安全判照常复跑。**永不豁免**：provenance 缺失（`:130-132`）、off-main、契约面、`verdict === 'BLOCK'`（证据缺失不是风险接受能抹平的形态，人批豁免不适用）。
4. **flag 误用防护**：`--human-approval-ref` 给了但 `humanApproval.ok === false` → BLOCK 且理由点名核验失败项（fail-closed）；kind 为 `gov`/`unknown` 时带此 flag → exit 2 用法错（治理档无人批豁免概念，防误用）。
5. **self-check 增补 8 例**（见 §5 V1），既有 10 例保持不动。

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改）

### T2 · `merge-driver.mjs` 透传 + 留痕

改法（对 P1 的链路闭环）：

1. 入口参数段（现 `:36-43` `SINGLE_POD`/`HUMAN_EXEMPTION` 形态）新增 `HUMAN_APPROVAL_REF` 解析；用法注释（`:9-13` 段）补一行。
2. Step 1b 调用（`:621-625` 的 `execFileSync` 参数数组）：`HUMAN_APPROVAL_REF` 非空时追加 `'--human-approval-ref', HUMAN_APPROVAL_REF`；PASS 后 log 行带 `human-approval-ref=<eventId>`。
3. merge commit message（现 `:731` 形态 `Merge ${changeId} (merge-driver)`）：带 ref 时追加 `human-approval-ref=<eventId>`（OQ3 已裁 (a)，强制进 message）——对齐 Pod S 手工合并 message 的留痕形态（事件号进 message）。

目标文件：`.claude/scripts/merge-driver.mjs`（改）

### T3 · `planner.md` 四处 ESCALATE 分支补「批准后机械路径」

改法（对 P2）：四处（`:87` 单 Pod、`:91` 多 Pod、`:109` 合并门段、`:137` 最终交付段）的「停人类」句尾统一补同义路径定义，落地时按各处文体裁剪，语义锚钉死为：

> ESCALATE → 停人类。human 批准合并后：planner 落一条 pod-events `human-decision` 事件（`payload.decision="approve-escalated-merge"`，eventId 即人批凭据）→ 直跑 `node .claude/scripts/merge-driver.mjs --pod <changeId> --human-approval-ref <eventId>` 走标准 Step 1b→合并链（blocked 停等态由闸内联动豁免消解，OQ2=(a)）；人批不豁免 off-main/契约/provenance，闸照拦。

目标文件：`.claude/agents/planner.md`（改）

### T4 · 周边 prose 指针同步 + ARQ 移已结

改法（防文档陈旧，对 §3 census）：

1. `.claude/references/dev-loop.md:103`「ESCALATE 停人类」句尾补「人批后机械路径见 planner.md 合并门段（--human-approval-ref）」。
2. `.claude/references/pod-coordination.md:99` ESCALATE 行补同义指针（人批后重占 token 走 merge-driver 人批路径）。
3. `.claude/references/escalation-playbook.md:15` ESCALATE 行补同义指针。
4. `.claude/commands/build-feature.md:31`「ESCALATE 才停人类 review/合并」句尾补同义指针。
5. `.claude/references/code-review-gate.md:89`「ESCALATE 才进入人类合并门」句尾补同义指针（R1 F1：与 build-feature.md:31 同语义同形态，落地后不补即陈旧文档）。
6. landing 完成 + landing-verifier 绿后：`docs/ARCH-REVIEW-QUEUE.md` 把 `ARQ-260902-escalate-merge-no-human-path` 从 §1.3 移除、§2 已结索引加一行（evidence 指本目录），并跑 `node .claude/scripts/changes-index.mjs --upsert 260904-gov-review-escalate-merge-approval`。

目标文件：`.claude/references/dev-loop.md`（改）、`.claude/references/pod-coordination.md`（改）、`.claude/references/escalation-playbook.md`（改）、`.claude/commands/build-feature.md`（改）、`.claude/references/code-review-gate.md`（改）、`docs/ARCH-REVIEW-QUEUE.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

census 命令（2026-09-04 实跑）：`grep -rn 'ESCALATE.*停\|停.*ESCALATE\|停在人类\|停人类\|停，交人类' .claude/ docs/ --include='*.md'` + 既有 `grep ESCALATE` 全仓文件清单。逐条分类：

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G3 闸脚本 | `.claude/scripts/pre-merge-escalation-gate.mjs` | G3 改 | T1，加人批核验口 |
| G3 合并执行 | `.claude/scripts/merge-driver.mjs` | G3 改 | T2，透传 + 留痕 |
| G3 规程 | `.claude/agents/planner.md` | G3 改 | T3，四处 ESCALATE 分支（:87/:91/:109/:137） |
| G2 文档 | `.claude/references/dev-loop.md` | G2 改 | T4，:103 指针；:274「停下等你」语义仍成立不动 |
| G2 文档 | `.claude/references/pod-coordination.md` | G2 改 | T4，:99 指针 |
| G2 文档 | `.claude/references/escalation-playbook.md` | G2 改 | T4，:15 指针 |
| G2 文档 | `.claude/commands/build-feature.md` | G2 改 | T4，:31 指针 |
| G2 文档 | `.claude/references/code-review-gate.md` | G2 改 | T4，:89「ESCALATE 才进入人类合并门」指针（R1 F1 升类：与 build-feature.md:31 同语义，原判「未触」不成立） |
| 工作队列 | `docs/ARCH-REVIEW-QUEUE.md` | 改 | T4，landing 后 ARQ 移已结 |
| 治理地图 | `docs/DEV-GOVERNANCE.md` | 未触 | :188「高/critical 升级 human」语义仍成立（升级动作不变，本批只补人批后路径）；:204 升级闸出处注不动 |
| 机械 provenance 闸 | `.claude/hooks/supervisor-gate-check.mjs` | 未触 | :266「merge requires human ruling」——人批事件即 human ruling，本批后语义仍真（R1 F2 补录） |
| 观测脚本 | `.claude/scripts/loop-health-report.mjs` | 未触 | :113 仅 verdict 聚合计数，非合并路径语义（R1 F2 补录） |
| skill 文档 | `.claude/skills/gov-arq/SKILL.md` | 未触 | :83 为 Track A 路由裁断 `ESCALATE_FULL_REVIEW` 语境，与合并路径无关（R1 F2 补录） |
| 审查规程 | `.claude/references/code-review-gate.md:89` | （见上 G2 改行，R1 F1 升类） | 原判「未触」不成立，已并入 T4 |
| 契约门图 | `docs/DEV-GOVERNANCE.md:82` | 未触 | 契约门 DRIFT 停人，与本题无关 |
| arch-review 侧 | `review-moderator.md:74` / `arch-change.md:37` / `review-arch.md:29` / `architect-operating-model.md` / `architect-reviewer.md` / `arch-review-protocol.md` | 未触 | 均为契约路由语境的 ESCALATE，非合并路径 |
| 判级闸 | `.claude/hooks/supervisor-verdict-gate.mjs` | 未触 | ESCALATE 判级逻辑本批明确不动 |
| 既有豁免先例 | `merge-driver.mjs --human-approved-exemption`（:677-699） | 未触 | acceptance 门红豁免是独立机制，本批沿用其范式不改其行为 |
| 本批制品 | `changes/arch/260904-gov-review-escalate-merge-approval/` | 新增 | PROPOSAL + 后续 landing-summary / landing-verdict / retro（流程制品随七步在同目录产出） |

## 4. human 取舍点（OQ）

> **裁决结果（human，2026-09-04，随立项 ack 同场裁断）：OQ1=(a)、OQ2=(a)、OQ3=(a)。** §2 各 T 按裁断落地：T1 复用 human-decision 事件五要素核验；T1/T3 的 blocked 态按闸内联动豁免（`humanApproval.ok` 时通用 3 的 blocked 理由不推，且仅限该情形）；T2 merge commit message 强制带 `human-approval-ref=<eventId>`。

### OQ1 · 人批凭据的表达形态

- **(a)（推荐）** 复用既有 pod-events `human-decision` 事件，`payload.decision="approve-escalated-merge"` 钉为标准取值，`--human-approval-ref` 引 eventId。理由：Pod S（事件 `86c73079`）已是事实惯例，零新机制、零新文件，核验五要素全部落在既有 schema 内。
- (b) 新立事件类型 `human-merge-approval`：语义更专，但要动事件 schema 约定与写入侧习惯，本批「不改写入侧」的非目标会被打破。
- (c) 签名文件（如 `changes/dev/<id>/human-merge-approval.md`）：多一处真相源，与 pod-events 双写漂移风险。

### OQ2 · ESCALATE 停等留下的 Pod blocked 态如何解

ESCALATE 停人类时 Pod 被置 `blocked`（多 Pod 由 planner 置，单 Pod 由 merge-driver Step 1b 拦下时置）；升级闸通用 3「change 处 blocked 态 → BLOCK」会挡住人批后的重跑。

- **(a)（推荐）** 闸内联动：`humanApproval.ok` 视同 resolve 本次 ESCALATE 停等的 blocked 态，该条理由不推。理由：blocked 在这里只是「停等人类」的驻留标记而非独立安全判——合并期真正的安全判（off-main/契约/provenance/quick-check/mutation）全部照常复跑，风险没有出口；路径最短，planner 零额外步骤。
- (b) planner 先 `portfolio-reconcile --set-pod <id> --status ready-to-merge` 翻状态再跑 merge-driver：语义更显式，但多一步且单 Pod 模式该 CLI 路径未实证过，失败多一个坑位。

### OQ3 · merge commit message 留痕强度

- **(a)（推荐）** 强制：带 ref 合并时 message 追加 `human-approval-ref=<eventId>`。理由：对齐 Pod S 手工合并 message 已做的事（事件号进 message），事后从 git log 即可追溯人批凭据。
- (b) 只进 merge-driver 日志（stdout）不进 message：留痕弱，CI/日志易散。

## 5. 验证集（landing 前机械前置 + landing 后核验）

- **V1 · 升级闸 self-check 扩电池**：`node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` → `ok:true` 且 `total ≥ 18`。新增 8 例：`escalate-no-approval-still-block`（ESCALATE 无 flag 行为零变更）/ `escalate-with-approval-pass` / `approval-wrong-eventid-block` / `approval-actor-not-human-block` / `approval-wrong-decision-block` / `approval-wrong-changeid-block` / `block-verdict-not-waived`（BLOCK 不可人批豁免）/ `escalate-provenance-missing-not-waived`（provenance 缺失永不豁免）。真空绿规约：电池重放断言输入非空、逐 case 有预期。
- **V2 · 语法闸**：`node --check .claude/scripts/pre-merge-escalation-gate.mjs && node --check .claude/scripts/merge-driver.mjs` 零输出通过。
- **V3 · merge-driver 既有自检回归**：`--self-check-quick-checks` / `--self-check-detect-changes` / `--self-check-rebase-target` 全绿（行为保持）。
- **V4 · 真实重放（Pod S 数据）**：`node .claude/scripts/pre-merge-escalation-gate.mjs 260829-smoke-e2e --source-ref <Pod S 分支 ref> --human-approval-ref 86c73079-89f7-41c2-8c88-38e5a679b09d` → 预期 BLOCK 理由中**不再含** `riskTier=` 与 `非 APPROVE`；若源 ref 已含 main 则 off-main 理由仍在（向严保持的证明）。对照组：同命令去掉 flag → 两条理由回来。
- **V5 · landing 结构门**：`node .claude/scripts/landing-fidelity-check.mjs`（或现行等价入口）对 packet 跑绿 + landing-verifier 逐文件人读 diff 核对 == 本 §2 清单。
- **V6 · 索引钉行**：`node .claude/scripts/changes-index.mjs --upsert 260904-gov-review-escalate-merge-approval` 单行进 `changes/INDEX.md` arch 段。

## 6. 留根清单

- **人批事件写入侧仍是 LLM 可写**：pod-events 由 planner 落盘，`actor:"human"` 字段防呆不防恶意——对称 `supervisor-verdict-gate.mjs` H5 既有残余（不抗恶意内部人，防漂移/防意外），本批同级接受、登记不硬治。
- **升级闸 `escalationUnresolved` 聚合**（`pre-merge-escalation-gate.mjs:196` 注「pod-events 聚合待接入」）与本批的 pod-events 读取部分重叠：将来做统一 pod-events 聚合器时两处并轨，本批不预设。
- **Track B 治理档不适用本 flag**：若将来出现治理合并人批豁免需求，另行立项，不在本批半径。
- **方向 B 留根 10 销账**：本批 landing 后，G/WS 两包 ESCALATE 合并路径就绪，重切 PROPOSAL 留根 10 在 G 包 merge 时实证销账。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 2026-09-05 · architect（Track B 判官，governance profile）

**结论：需修订（2 条打回项，均不触 T1–T3 核心设计；主线程修订后送 R2 复核）。**

**证据实核**（抽核范围 = 全部主线程预核项独立复核 + 任务书指定重心项，逐一对照现仓行号）：

1. `pre-merge-escalation-gate.mjs`：:22 用法行无 approval 类参数 ✓；:126-127 riskTier 理由 ✓；:130-132 provenance 理由 ✓；:133-134 非 APPROVE 理由 ✓；:120 blocked 理由 ✓；:196 escalationUnresolved「待接入」注 ✓；:267-276 参数解析段形态 ✓。
2. self-check 实跑（2026-09-05）：`{"pass":10,"fail":0,"total":10,"ok":true}`——现有 10 例、无任何 approval 用例，与 §1 一致 ✓。
3. `merge-driver.mjs`：:36-43 SINGLE_POD/HUMAN_EXEMPTION 解析形态 ✓；:617-632 Step 1b execFileSync 复跑升级闸 + BLOCK 即 setPod blocked ✓；:677-699 `--human-approved-exemption` shape-only 豁免先例 ✓；:731 merge message `Merge ${changeId} (merge-driver)` 形态 ✓。
4. `planner.md` 四处：:87（「ESCALATE → 停人类。」句号后无下文）/:91/:109/:137 原文逐字命中 ✓。
5. 四件 prose：dev-loop.md:103「ESCALATE 停人类」✓；pod-coordination.md:99「Pod 挂 `blocked`（等人类）」✓；escalation-playbook.md:15「ESCALATE | 停，交人类」✓；build-feature.md:31「ESCALATE 才停人类 review/合并」✓。
6. Pod S 证据链：事件 `86c73079-89f7-41c2-8c88-38e5a679b09d` 五要素逐字段实核（schemaVersion 1 / eventType human-decision / actor human / payload.decision approve-escalated-merge / changeId 260829-smoke-e2e）✓；supervisor-verdict.json verdict=ESCALATE、riskTier=high、理由 cross-system + security-axis-diff:auth、provenance.mechanicalSupervisorGate.ran=true + producedBy 在 ✓；commit e33278d message 与 §1 引文逐字一致 ✓；`.git/devloop/pods/260829-smoke-e2e.json` worktreePath 字段在 ✓。

**Findings**：

- **F1（SHOULD，census 定性错误）**：§3 把 `.claude/references/code-review-gate.md:89` 判「未触」，理由「泛 BLOCKING 停合并门语义，非 ESCALATE 合并路径」不成立——该行末句「ESCALATE 才进入人类合并门」与 build-feature.md:31（已入 T4 第 4 项）同语义同形态，本批落地后若不补指针即为 T4 所要防的陈旧文档；且该文件不在 landing-manifest 内，landing-fidelity-check 不会兜住此漏面。修订：T4 增补该项（句尾补同义指针）+ §3 该行类别改「G2 改」+ landing-manifest 围栏块加 `.claude/references/code-review-gate.md` 一行。
- **F2（MINOR，census 记录完整性）**：§3 声称「grep ESCALATE 全仓文件清单」逐条分类，但实测另有 3 个引用 ESCALATE 的文件未获分类行：`.claude/hooks/supervisor-gate-check.mjs`（:266「merge requires human ruling」——本批后仍真，人批事件即 human ruling）、`.claude/scripts/loop-health-report.mjs`（:113 仅 verdict 聚合计数，非合并路径语义）、`.claude/skills/gov-arq/SKILL.md`（:83 为 Track A 路由裁断 ESCALATE_FULL_REVIEW 语境）。判官已代核三者均实质不受本批影响；修订时在 §3 表补三行「未触」分类即可，不改 T1–T4。

**其余专项核查（无 finding）**：

- 豁免语义自洽：五要素全过才 ok（fail-closed）；BLOCK 不豁免、provenance 永不豁免、off-main/契约/quick-check 照拦；gov/unknown 带 flag exit 2 防误用。与既有 `--human-approved-exemption`（shape-only、acceptance 门）无冲突亦不重复——命名平行、作用门不同、本批核验强度更高（核到具体事件）。
- V4 重放可行性预核：事件与 provenance 均在主仓可读，预期「BLOCK 理由减 riskTier 与非 APPROVE 两条」可机判；off-main 理由保持符合向严。V1 计数自洽：10 既有 + 8 新增 = total ≥ 18。
- §0 非目标与 §3 一致：DEV-GOVERNANCE.md:188「高/critical 升级 human」升级动作本批后仍成立（:204 出处注、:82 契约门图亦实核未触）；判级闸 supervisor-verdict-gate.mjs 未动，论证成立。
- 观察项（不构成打回，供 §6 留根参考）：五要素不含事件时序绑定——若同一 changeId 将来出现「合并后再开、旧人批事件重放」路径，闸仍由 off-main/契约等真安全判兜底，且该残差类已被 §6 第 1 条（LLM 可写事件、H5 对称）覆盖，本批无需处理。

**结构字段核对**：landing-manifest 围栏块与 packet-id 行在场、与 T1–T4 改法集一致；F1 修订须同步增补 manifest 一行后再冻结。

### R2 · 2026-09-05 · architect（Track B 判官，governance profile）

**结论：GREEN（可冻结）。**

**修订复核**（对 R1 F1/F2，逐条比对修订后 PROPOSAL 与现仓实况）：

1. **F1 三处一致性 ✓**：T4 新增第 5 项（`code-review-gate.md:89`「ESCALATE 才进入人类合并门」句尾补同义指针）——引文与现仓 ：89 原文一致；T4 目标文件行已增补 `.claude/references/code-review-gate.md`（改）；§3 原行升类为「G2 改」并保留历史注记行（「见上 G2 改行，R1 F1 升类」——审计痕迹保留，认可）；landing-manifest 围栏块已增补该文件一行。
2. **F2 补录 ✓**：§3 新增三行「未触」——`supervisor-gate-check.mjs`（:266 human ruling 语义本批后仍真）、`loop-health-report.mjs`（:113 聚合计数）、`gov-arq/SKILL.md`（:83 Track A 路由语境），三条说明与 R1 判官代核结论逐字相符。
3. **头部状态行 ✓**：DRAFT v0.2，待 R2 记录已如实更新。

**无新引入问题**：T1–T4 改法集、OQ 裁决、§5 验证集、§6 留根与 R1 版本零漂移；§3 表重排后各「未触」行定性无变化；manifest 无多行漏行。

**GREEN 附带核对（结构字段硬条款）**：

- landing-manifest 围栏块在场（10 行：T1 一件 + T2 一件 + T3 一件 + T4 五件 prose + ARQ 队列 + PROPOSAL 本体），与 §2 改法集逐一对应、无多无漏 ✓。
- packet-id 行在场（§2 改法集 T1–T4 + §5 验证集）✓。

**遗留提示（不构成打回）**：R1 观察项（人批事件五要素不含时序绑定）仍由 §6 第 1 条留根覆盖，冻结与落地无需处理。
