# DEV-LOOP · 开发期自治循环（用今天的 Claude Code 实现）

> 这是蓝图 §7.3「开发期编排」的**可执行落地**。它使用**当前** Claude Code 原语
> （子 agent + permissions.deny + 钩子 + worktree）运行，**不依赖**尚未建成的
> K9/K6/K3——避免依赖尚未建成的工具来构建工具，形成循环前置。唯一允许共享的依赖是
> `contracts/`（纯 zod，无运行时依赖）。这套脚手架**先于**内核存在，未来才反哺成 K9。
>
> **定位**：本文件是「**Dev Loop OS · Loop 内部层**」的可执行落地（单 Pod 内部 + 门强制）。编排层（RTE / 多 Pod / retro）见 [dev-loop-operating-system.md](dev-loop-operating-system.md)；四流程路由与伞状入口见 [DEV-GOVERNANCE §4](../../docs/DEV-GOVERNANCE.md)。

---

## 0. 强制默认规则

实现机上的正式开发默认必须走 dev loop。适用范围包括 feature、bug fix、行为调整、重构、测试修复、实现侧文档同步，以及任何疑似影响契约的变更。

主 AI 的默认职责是 planner/调度/只读分析，不直接改 `firmware/**` 实现代码。开工入口是：

```powershell
claude --settings .claude/dev-loop.settings.json --agent planner
```

然后用 `/build-feature <story>` 进入循环。缺少验收测试、红基线、TaskPackage 或 verifier 证据时，不允许把工作声明为完成。

允许例外只有：只读探索、计划讨论、架构/契约设计、人类明确要求跳过 loop 的一次性操作。例外不能顺手扩大成正式实现。

dev loop 采用 progressive rigor：低风险小修可以用 Lite TaskPackage，高风险/跨系统/契约相关变更必须使用 Full Change Artifact；两者都不能跳过验收、验证、closeout 和合并门。**三档边界（③拆包 / ②单包Full / ②单包Lite）与 `rigor` 字段级定义见 [task-intake-and-slicing.md](task-intake-and-slicing.md)「rigor 字段与 Lite TaskPackage 字段级定义」；改 `firmware/**` 行为的变更开工前先过 `/triage-change` 分流门（architect 比例化 review + 裁决四档，见 [change-triage.md](change-triage.md)）。**

### 0.1 入口来源与任务切片

`delivery/milestones/` 只记录承诺路线图与里程碑分母；`delivery/workstreams/` 只记录跨切片、跨会话的长周期工作记忆；TaskPackage 才是所有正式实现工作的最小执行合同。架构 review finding、CI 红线、drift report、人类临时任务即使不进入 `delivery/milestones` 或 `delivery/workstreams`，也必须先进入 TaskPackage。

**分流门衔接（gov-review-architect-change-triage）**：改 `firmware/**` 行为的变更开工前先过 `/triage-change` 前置门（architect 比例化 review + 四档裁决，程序见 [change-triage.md](change-triage.md)）。reviewed plan 按成熟度就近落地：大 feature 落 `delivery/workstreams/<ws>/PROPOSAL.md`（人类冻结后由 portfolio-planner 拆包），小 feature/bug 落 `delivery/taskpackages/<id>/plan.md`（单 Pod 跑 dev loop）。planner 跑 `/build-feature <id>` 时从 sibling `plan.md` 读 reviewed plan + frontmatter 裁决（`rigor`/`sourceType`）做 intake；plan.md 是 architect 审过的，planner 只读消费、改需留痕。

planner 在 Draft TaskPackage 前必须按 `.claude/references/task-intake-and-slicing.md` 做 source intake，写入：

- `sourceType`：`delivery-story`、`planning-workstream`、`arch-review`、`human-ad-hoc`、`ci-failure`、`drift-report`、`regression`、`maintenance`、`proposal`。
- `sourceRefs[]`：稳定可追溯来源，如 `S1-A1`、`D-A1`、`ARQ-063-C1`、`TEMP-2026-07-05-*`。对于 `sourceType=proposal`，`sourceRefs[]` 必须包含 proposal `.md` 路径，且该路径必须同时出现在 `docRefs[]` 中。
- `deliveryImpact`：是否影响 `delivery/milestones` 里程碑健康。
- `planningImpact`：`none` / `attach-existing` / `create-workstream` / `needs-triage`。
- `riskTier` 与 `gateProfile[]`：决定必须启用哪些 verifier / integrator / supervisor 证据。

**TaskPackage 撰写约定**：字段级撰写约定见 [taskpackage-authoring.md](taskpackage-authoring.md)——撰写侧单一入口，覆盖每包易复踩的 6 个字段坑（只补约定、不改闸）。

架构 review loop 不能只靠 prose 把 confirmed findings 交给开发。凡 `CONFIRMED` 且需要实现侧修复的 finding，`adjudicator` / `review-moderator` 必须在 `delivery/changes/arch/arch-review-<spec>/dev-loop-handoff.json` 中产出结构化 handoff，包含 `sourceRefs`、`riskTier`、`gateProfile`、`blockingDecisions`、`suggestedTaskPackages`、`planningRecommendation`、`deliveryImpact`、`acceptanceRefs` 与 `verificationHints`。planner 只消费这个 handoff 做切片；没有 handoff，review 只能算已审完，不能算可执行地交给 dev-loop。

---

## 1. 拓扑：主线程是唯一编排者

Claude Code 的**子 agent 不能再派生子 agent**。因此编排者必须是**主线程**：

```
主线程 = planner（claude --settings .claude/dev-loop.settings.json --agent planner）
   ├─ 派生 architect-reviewer  预审疑似契约变更，过滤实现侧误判
   ├─ 派生 uiux-author（条件）  仅当 uiuxRequired && uiuxLevel>=2：从冻结的系统 brief 译出 TaskPackage uiux 块，在 acceptance-author 之前
   ├─ 派生 acceptance-author   先把故事钉成冻结的验收测试（红基线）
  ├─ 派生 builder             单包边界内实现 + 单元测试
  ├─ 内层循环（主线程驱动，非嵌套）：
  │     跑 verify.mjs  →  红则派生 fixer  →  再跑 verify.mjs  →  …直到绿/超预算
   ├─ 派生 integrator          docs-sync + 全量 e2e + delivery/changes/dev/ 制品 + 备可回滚路径
   ├─ planner 收尾（pod-retro） 第一人称热记忆；integrator closeout 之后、supervisor gate 之前（post-cutoff closeout 完整性条件，Q5=warn）
   └─ 跑机械 supervisor-verdict 闸  合并门复核（supervisor-verdict-gate.mjs，planner invoke 不派 agent）；低/中风险+证据完整机器 APPROVE 授权 planner 本地合并（前置升级闸放行后），高/critical 升级人类
```

`verifier↔fixer` 的内层循环由**主线程逐轮 invoke `verify.mjs`**实现，不是 verifier 自行调用 fixer
（它不能）。这点和蓝图 §7.3 的方框图一致，只是把「自验/修复」展开成了可执行的轮次。

`uiux-author` 是**条件步骤**：仅当 TaskPackage 声明 `uiuxRequired && uiuxLevel>=2` 时，在 acceptance-author 之前派生一次，把冻结的系统 `UIUX-DESIGN-BRIEF.md` 译成 TaskPackage 的 `uiux` 块 / `uiux-brief.md`（refs、状态矩阵、验收映射）。它**只翻译**，不发明系统 UI，不写实现或契约。Level 0/1 不派生它。

---

## 2. 文件地图（`.claude/`）

| 文件 | 作用 |
|---|---|
| `dev-loop.settings.json` | 循环专用设置（**不是**默认 settings.json）。`permissions.deny` 把 `docs/contracts/**` 冻结为只读；注册 PreToolUse / Stop 钩子。只在运行循环时显式加载，不影响日常交互式会话。 |
| `scope-map.json` | 每个 agent 的可写路径边界（allow/deny glob）。 |
| `verify-gates.json` | verifier 执行的有序闸门（结构闸 + 功能闸）。 |
| `hooks/enforce-scope.mjs` | PreToolUse 钩子：按 scope-map 物理拦截越界写入（Node，跨平台）。 |
| `hooks/deny-main-push.mjs` | PreToolUse 钩子：禁止直接 `git push` 到 main/master；合并走可回滚路径。 |
| `hooks/supervisor-gate-check.mjs` | Stop 钩子：integrator closeout 后主线程收尾前必须有 supervisor 裁断（post-cutoff 验 `mechanicalSupervisorGate` provenance + 从 immutable 输入第二方重算 riskTier）。 |
| `hooks/supervisor-verdict-gate.mjs` | 机械 supervisor-verdict 闸（planner Bash invoke，非 Stop 钩子）：从 git diff 重派生 riskTier（dev-loop:235 投影）+ 消费 bearing facts（validateFactObject 单源 fail-loud）产 APPROVE/BLOCK/ESCALATE，脚本 fs-write write-once `supervisor-verdict.json`。 |
| `hooks/verify.mjs` | 确定性验证执行器：按 verify-gates 跑闸门，产出与 K6 对齐的 ErrorEnvelope。 |
| `agents/*.md` | 角色 agent 定义。`verifier` 无 .md 定义文件（2026-07-25 gov 批退役）：保留为 **stage 身份**，行为合同由 `verify.mjs` round 证据 + `verifier-evidence-check.mjs` + scope-map 条目机械承载（**不派 agent**，planner Bash 直跑 `verify.mjs`——gov-review-verifier-dispatch-residual）。`supervisor` 同退役且**不再是派发身份**：合并门裁决由 `hooks/supervisor-verdict-gate.mjs`（机械闸，产 verdict）+ `hooks/supervisor-gate-check.mjs`（Stop 门，验 provenance v2 → mechanicalSupervisorGate）机械承载（gov-review-verdict-bearing-dual，2026-07-30）。 |
| `commands/build-feature.md` | `/build-feature <story>` 工作流，把上面这套串起来。 |
| `references/*.md` | 共享工作流知识：任务 intake 与切片、验收证据、测试反模式、ErrorEnvelope、K6 可观测性、安全闸、Review Packet、ADR。 |
| `scripts/arch-review-handoff-check.mjs` | arch-review 收口检查：有实现侧 confirmed finding 时必须有 `dev-loop-handoff.json`。 |
| `scripts/dispatch-stamp.mjs` | `--token-status exact|unavailable` + 可选 input/output/cache/total/source/reason；post-cutoff 只写 pod-events `dispatch-completed`，pre-cutoff 保留 legacy `--tokens` + dispatch-log。**post-cutoff 另按 `--dispatch-id`（= harness agentId）读会话 transcript 自动派生** token 四路分项（`source: 'transcript'`）、**实际生效模型**（`payload.model` + `modelSource`）与主线程累计消耗（`payload.mainThreadUsage`）；取不到时如实降级 `unavailable`（260920-gov-review-dispatch-telemetry-source FROZEN v1.0）。 |
| `scripts/brief-inputs.mjs` | 派发输入面封闭化（260908-gov-review-input-surface-frontload T2）：读 taskpackage 的 refs + `crossEnd`，产 `input-brief-<role>.md`（锚点节定位 / 跨端对端面 / 先例封顶 3 / 超读申报模板）。**planner Step 7 前置一次产双份**；对端 symbol 解析失败 fail-loud 不产空锚；`--dry-run` / `--self-check` 供验证。只读派生、不抽契约正文。 |
| `scripts/journal-append.mjs` | Post-cutoff friction 重定向 pod-events、round-outcome 丢弃；pre-cutoff 保留 pod-journal。 |
| `scripts/pod-events-append.mjs` | pod-events v1 共享 envelope writer；导出唯一 `EVENTS_SCHEMA_CUTOFF`，校验闭集 eventType 并生成 UUID。 |
| `scripts/pod-events-aggregate.mjs` | new-format on-demand 聚合 exact token coverage、escalation 与 friction；malformed 行 fail-open 跳过。**T7（260908 批）加 `actors` 列**：per-actor 挂钟合计/中位、toolUses 中位、长尾（≥40min）计数与 eventId 明细 + 缺值如实计数（不补 0）；纯 advisory，`/loop-retro` 直接消费。只增键、既有输出键不破。 |
| `scripts/loop-metrics.mjs` | Post-cutoff 从 pod-events + verifier-evidence on-demand 输出且不提交文件；pre-cutoff 保留 legacy loop-metrics.json。 |

---

## 3. 契约人工门 + supervisor 合并门 + 防「空过」机制

**门控（不可逾越）**
1. **契约变更门**：循环里没有任何 agent 能改 `docs/contracts/**`（已 deny）。**任何 contracts 修改——包括已获架构师方向批准的补丁——都必须先过 `architect-reviewer`**。
   Reviewer 的职责不限于首次审批：对已批准方向的落地 patch，它验证字段/命名/约束/Tier 分类是否与裁决一致（合规检查）；
   对新请求，它判断是升级人工门还是实现侧可自行解决。只有 reviewer 返回 COMPLIANT，planner 才能提交契约改动。
   若 reviewer 返回 NON_COMPLIANT，planner 停下，把差距交给人类架构师。
2. **supervisor 合并门**：integrator 在 feature 分支 commit 已绿改动并备可回滚路径，不 push、不合并主干。合并门裁决由**机械闸 `supervisor-verdict-gate.mjs`** 产出（planner invoke，不派 supervisor agent——2026-07-25 退役）：从 git diff 机械重派生 riskTier（:235 投影）+ 消费 bearing facts 判证据完整度，产 APPROVE/BLOCK/ESCALATE。APPROVE 只是授权 planner 本地合并（前置升级闸放行后），BLOCK 退 fixer/planner，ESCALATE 停人类；人批后机械路径见 planner.md 合并门段（`--human-approval-ref`）。高/critical（core.*/跨系统/全 mock/安全轴 → high；docs/contracts/**/迁移/删数据 → critical）升级人类；critical/契约/迁移/删数据仍走人类 + architect gate。Stop 钩子 `supervisor-gate-check.mjs` 第二方验 provenance（mechanicalSupervisorGate）+ 从 immutable 输入重算核对（FIX-6）。直接 push main/master 由 hook 拦截。
3. **merge executor（机械执行者）**：不是 review 角色 = planner 主线程。它只在 `supervisor-verdict.json` 为 APPROVE 且前置升级闸（`pre-merge-escalation-gate.mjs`）放行后执行本地合并：`merge-driver --pod` 本地 `--no-ff` 合并；**不 push、不建/更 PR、不 auto-merge remote**；不得直接 push main/master，不得覆盖 supervisor/human 裁断。

**防空过（让「做完」对故事负责）**
1. **验收先行且冻结**：`acceptance-author` 在 builder 之前把故事翻译成可执行验收测试，
   每条断言绑定一个 TaskPackage acceptanceRef/sourceRef（例如 `Sx-Ay`、`D-A1`、`ARQ-063-C1`、`TEMP-*`）；builder/fixer **只读**这些测试（钩子强制）。
2. **反「200=过」**：API 测试必须断言响应体过 zod + 关键取值 + **DB 副作用** + **事件已发**
   +（必要时）审计/成本。UI 测试（仅当子系统声明有 UI）用 Playwright 断言**状态变化与内容**，
   禁止「页面能打开就算过」。
3. **验收覆盖闸**：每个声明的 acceptanceRef/sourceRef 至少被一条断言覆盖，否则红。
4. **破坏性自检（mutation check）**：对实现做一处临时破坏，验收**必须**因此变红；
   若仍绿，说明验收是空的 → 红。
5. **证据优先**：完成状态只认 verifier/integrator 证据，不认 agent 的解释。stdout 保持短摘要，长日志落文件。
6. **文档与 closeout 归档**：integrator 在合并门前同步 canonical docs 并晋升标准 `delivery/changes/dev/<id>` 制品，防止 shipped behavior 与 spec/AGENTS/HANDOVER 以及交付证据漂移。

### 变更制品（Change Artifact）

每个正式变更有两个持久目录，按职责分工、不互拷（gov-review-taskpkg-lifecycle, FROZEN 2026-07-25）：

- **`delivery/taskpackages/<change-id>/`**（执行 ledger）：机器执行态，Git-tracked。
- **`delivery/changes/dev/<change-id>/`**（closeout/audit）：人可读收官/治理审计。

Post-cutoff（changeId date-prefix >= `CHANGE_ID_CUTOFF`）包的 `taskpackage.json` **仅存于** `delivery/taskpackages/<id>/`，不晋升到 `delivery/changes/dev/`。Pre-cutoff 包保持兼容（grandfather clause）。标准内容：

- `taskpackage.json`：sourceType、sourceRefs、deliveryImpact、planningImpact、riskTier、gateProfile、story、acceptance、exitCriteria、budget、contractRefs、docRefs。
- `plan.md`（②单包，可选）：`/triage-change` 分流门 architect 审过的 reviewed plan（sibling 文件，由 taskpackage.json `planRef` 引用，对偶 `acceptanceTestsFile` 引用 `.acceptance.spec.ts`）。planner **只读消费、改需留痕**——完全漂移保护依赖 `origin.upstreamFrozen` + `closeout-check` 的 `UPSTREAM_DRIFT` 检测（已随 artifact-lifecycle 交付，convention-only/warn，ARQ-260723-artifact-lifecycle-teeth）。大 feature 的 reviewed plan 落 `delivery/workstreams/<ws>/PROPOSAL.md`，不在 taskpackage 内。
- `verifier-evidence/rounds/r<round>.json`：**verifier 每轮承重机器事实**，由 `verify.mjs --evidence-mode=round` 程序 fs-write 产出（每轮 scoped/final），非 agent 落盘。identity = `changeId + phase + completed-only round + invocationId`；内部 deterministic gate-child exit 与 fact 配对承重，planner/integrator 只能用 `verifier-evidence-check.mjs` 精确验证该 fact。
- `verdict.json`：**兼容投影**，由 `verify.mjs` best-effort 写入 `current/verdict.json`，可含 `evidenceRef` / identity，但可覆盖、non-bearing；不能满足 FS-EVIDENCE-2、resume、closeout、supervisor 或 merge-readiness。
- `verification.json`：supervisor-compatible closeout 机器件；在 closeout cardinality 中为 advisory-only，缺失不阻塞 `closeout-check`。终审全量闸仍由 immutable verifier-round fact 承重，integrator 只消费 validated final fact，不用 projection 替代判定。
- `docs-delta.md`：需要归档回 canonical docs 的变更，或无需更新理由。
- `review-packet.md`：change size、verification evidence、findings、five-axis review、residual risk、**未覆盖面申报**（260920-gov-review-bearing-strength-align T3）。
- `input-brief-acceptance-author.md` / `input-brief-builder.md`（T6，260908-gov-review-input-surface-frontload）：`scripts/brief-inputs.mjs` 生成的**派生**封闭输入清单，Git-tracked（本目录本就 tracked）。**不晋升到 `delivery/changes/dev/`**（与 post-cutoff `taskpackage.json` 同构：二者都是活执行态，晋升等于把一份带行号的短命快照复制成归档件，反与下文快照语义冲突）。**生成时点 = 拆包侧**（portfolio-planner Step 4 落盘时一次产双份，随 commit-before-launch 进基准 ref）；Pod 内 planner Step 7 **核在场**、refs 有改动才重跑（`--regenerated-because` 留痕）。Step 8 核在场、Step 9 只引用。**证据属性** = 交付侧超读申报（清单外阅读 + 原因）与 wire 节 WARN 复核的锚点。**快照语义（与 §2 表同文，不得只读一半）**：文件内行号 = 生成时点解析结果、带 `generated-at` 时间戳，**非长期事实源**——消费时 symbol 已移位就重跑脚本取新行号，不得照旧行号通读整文件；重跑覆盖事由由脚本写 `regenerated-because` 行落制品自身，不另开记账面。非权威：契约 / spec / 代码各仍是其自身 SSOT，本文件只出指针与节标题、不抽正文。
- `pod-events.jsonl`（post-cutoff）：执行 ledger 的唯一 retro/dispatch 结构化事件面；observational、non-bearing，不进入 verifier/supervisor/merge authority。
- `pod-retro.md`（post-cutoff）：唯一人读 retrospective，只记 root cause、friction/escalation/human-correction themes、unresolved concerns 与 improvement routing；不复述 tokens/gates/rounds/verdict/test results。
- `loop-metrics.json` / `pod-journal.jsonl`：post-cutoff 退役、不提交；pre-cutoff 保留原生产、晋升与读取兼容。

**退役制品（C4，gov-review-devloop-evidence-led FROZEN 2026-07-24）**：
- `test-evidence.md` → **退役**（verifier 报告复述）；机器数据由 `verdict.json` + `verification.json` 承接。
- `final-summary.md` → **退役**（复述类）；判断 prose 由 `review-packet.md` 承接，闭包状态由 `verification.json` 存在性判定。
- `delivery/changes/dev/` 下 `taskpackage.json` 快照 → **退役已落地**（gov-review-taskpkg-lifecycle, FROZEN 2026-07-25）；post-cutoff 包不再复制，活文件 `delivery/taskpackages/<id>/taskpackage.json` 唯一承载。

#### closeout 制品 SSOT（承重 / blocking 状态单一权威）

> gov-review-devloop-closeout-hardening（human-approved 2026-07-27，Q5=warn）。本表是 closeout 制品**承重 / blocking 状态的单一权威**；各处（§3.4 降级 profile、integrator C4、closeout-check 头注、supervisor-gate closeoutFiles）**引用本表、不重述分类**（反漂移约束，F-02）。机器件拆三行（F-02）：承重 / blocking / 投影·advisory 三分。

| 制品 | 状态 | pre-cutoff | post-cutoff | 机器产线未运行的手工替代集 |
|---|---|---|---|---|
| `verifier-evidence/rounds/r<N>.json`（verifier-round fact） | **承重机器件**（closeout 相位承重替代） | 承重 | 承重 | 手工跑机械闸 supervisor-verdict-gate.mjs 补承重 + 留痕（降级 profile，FIX-13；不复活 supervisor agent，守 P-6） |
| `supervisor-verdict.json` | **唯一 blocking 机器件**（supervisor Stop 门 G2 block 判据，`supervisor-gate-check.mjs:667-674` verdict-existence / `:768-770` BLOCK-value；行号实地核正——J4 一次校正 :280-286→:298-305/:371-373 against 编辑前文件，本批 G6 编辑〔2a import/2c validateProvenance/2d recompute〕同批再移→:319-326/:410-412；260814-gov-review-merge-gate-terminal-shape-batch T1-T4 同批再移→:667-674/:768-770） | blocking | blocking | 不可降级（不许降级面，F-11）——supervisor 合并门必产（post-cutoff 由机械闸 supervisor-verdict-gate.mjs fs-write） |
| `verdict.json` / `verification.json` | **投影·advisory**（non-bearing，对引上文 `:125-127`） | advisory | advisory | 缺位时承重 = 手工跑机械闸 supervisor-verdict-gate.mjs 留痕（FIX-13，对偶上行 :146；260726 verdict 先例成文化）；closeout-check 维持 advisory-only、不升 blocking |
| `test-evidence.md` | **退役**（机器产线运行时）；机器产线未运行 → **复活为手工替代件** | 退役 | 退役 | 复活件 = verifier 亲跑命令 + 输出摘要 + exit code（即 260726 形态）。**复活规则 = 程序条款（降级手工替代集）/ pending；`closeout-check.mjs requiredFiles` 逻辑不变**（F-10，避免注释↔代码主动分歧） |
| `delivery/changes/dev/` 下 `taskpackage.json` 快照 | **维持退役** | 兼容保留 | 退役 | 活文件 `delivery/taskpackages/<id>/taskpackage.json` 唯一承载（260725/260726 快照不追溯清理） |
| `final-summary.md` | **退役不替代** | 退役 | 退役 | 判断 prose 归 `review-packet.md`；闭包状态由 `verification.json` 存在性判定 |
| `review-packet.md` / `docs-delta.md` | **维持 blocking 必交** | blocking | blocking | 不可降级（手工产，不可跳过） |
| `pod-retro.md` | **closeout 完整性条件**（planner 收尾相位，第一人称热记忆） | —（legacy 无） | **Q5=warn advisory + 零追溯白名单**（非 blocking；`closeout-check` post-cutoff 存在性牙） | planner 手工著（第一人称热记忆） |

**closeout-check 调用时机（SSOT · 候选③纯文档、零代码）**：closeout-check 是 integrator **产出 closeout 制品后**跑的终检（`integrator.md:52` 第 11 步已规定产出后跑；`--update-index` 写调用见下方硬要求段）。**planner 复核 / 探针调用**用默认只读姿势：在制品产出**前**拿它当「我该产什么」探针会**首红**——闸行为正确（缺制品就该红），**非 soundness bug，是人机工程/顺序摩擦**；探针解读应读其 fail 清单作**产出指引，不作终检判定**。终检 hard-fail 语义**不变**（候选③零力度、不弱化；候选① advisory `--dry-run`/`--pre` API 已裁否，留独立 Track A）。

**acceptance vs exitCriteria 编写惯例（A10 型，gov-review-verifier-gate-infra-fixes 成文；260731-gov-review-acceptance-coverage-structural-exemption FROZEN v1.0 修订）**：`acceptance[]` 只放**测试可断言**条目（可被冻结验收测试断言覆盖、被 coverage marker 命中）；过程/证据型条目（如「逐项走查清单并在 evidence 文件签字」）放 `exitCriteria`，由 integrator/supervisor 按证据判读。

**非逐 id 可测条目的唯一一等豁免通道 = 显式对象标记**（260731 起，coverage/mutation 两闸同义）：非代码可测条目（typecheck/schema/compile 等 structural 门）写 `{ "id": "X", "kind": "structural" }`；套件级回归门（「既有套件不新增红」等）写 `{ "id": "X", "suiteGate": true }`。两标记**互斥**（同条目双标 = 类别矛盾红）；未知 kind 值 / `suiteGate` 非 true = fail-safe 红（taskpackage-required-check 形状校验一线成文）。**suiteGate 适用判据**——正面清单：既有套件零回归 / `firmware-build`（idf.py build）干净 / `contract-lint` 类可枚举形态；反面规则：**可断言行为条目贴 suiteGate = A10 型违规，退 planner 重派**（D-3 裁）。mutation 证据对这两类条目**不适用**（套件级回归门非逐 AC 敏感判定对象，M-4）。

**两条豁免通道逐字切开**（OQ3 硬条件）：① `exitCriteria[]` 的「structural 词 + **全 id 提及**」豁免路径**保留**——准则含 structural 类词（typecheck/firmware-build/idf.py/contract-lint/compile 等）且**完整提及**该 acceptance id 时豁免（K7-S5 形合法）；② `acceptance[]` 条目 **prose 词表洗白已移除并明令禁止**——措辞含 "integrator"/"checklist"/"sign off" 等词不再豁免任何条目（工具侧 governanceWords 通道已删，工具↔规范一致，CS-7 形 gaming 闭合）；③ **裸后缀 / 区间缩写 / prose 同物提及自此不豁免**（力度变更成文辨析：非纯 bug 修复）——「S1-A1..A14 全绿」不再豁免 P1-A14，准则须写**全 id** 或用显式对象标记；存量样本 s1-nsa P1-A14 / k1-p1 / k1-user-person / k1-session K1-A13 / k8 碰撞四条全部闭包、零追溯，新写法只面向未来包。

**红判读（三路）**：① acceptance-coverage 红且缺失条目为过程/证据型（A10 型）= 编写惯例违规——退 planner 重派 acceptance-author 把该条目移入 exitCriteria；不得由 fixer 补 marker、不得以任何词表/prose 途径洗白。② 真缺 coverage（可断言条目无测试）→ builder/fixer 补测试路径。③ **标记存在但可疑**（如可断言行为条目贴了 kind/suiteGate 标记）→ verifier **不退 builder**，退 planner 重派 / 升级 supervisor，verify-runs 留痕（F-01/F-13 质疑路由，D-1 裁）。**豁免审计消费（D-1 (a)+(b)）**：supervisor 放行判据——coverage 闸输出 `suiteGates:[...]` 非空 → **human glance 一句确认**；integrator closeout 的 review-packet 必枚举本轮 `structural:[...]`/`suiteGates:[...]` 清单（逐条一句「为何豁免」），supervisor 合并门核其存在。

`taskpackage.json` 可按风险声明附加字段：

- `securityReviewRequired` / `securityAxes` / `securityEvidence`：启用安全闸并记录证据。
- `adrRequired`：提示 integrator 把架构/边界/契约取舍归档成 ADR 摘要或 `delivery/changes/dev/<change-id>/`。
- `s2ExploratoryRequired` / `s2ExploratoryReason` / `s2ExploratoryGoals[]`：按风险要求 S2 official exploratory scan；planner 声明目标，integrator 在全量 verify/e2e 绿后执行并把摘要写入 Review Packet。Official scan 必须使用 `run_inline_check` typed fields：`targetPersona` 必填（匿名用 `anonymous`），`metadata.changeId/taskPackageId/goalId/triggeredBy="dev-loop-integrator"` 必填，`allowDestructive=false` 默认；若允许破坏性探索，必须有 `destructiveScope` 与 `safetyReason`。`maxSteps` / `budget` 可省略（Phase 1 bootstrap 默认不限），official scan 默认 `wait=true` 且调用层等待上限 600 秒。

变更包不是新事实源；`contracts/` 和 canonical specs 仍是事实源。它的作用是保留每次 loop 的上下文和证据，避免聊天记录成为唯一记忆。

**硬要求**：TaskPackage 必须落盘。planner 在 Draft 阶段创建 `delivery/taskpackages/<change-id>/taskpackage.json`，并刷新 `delivery/taskpackages/current/taskpackage.json`；Finalize 后必须再次写回。verifier 第一闸会检查当前 TaskPackage 是否存在且字段完整；integrator 必须运行 `node .claude/scripts/closeout-check.mjs --update-index` 确认 `delivery/changes/dev/<id>` 两个 blocking 标准制品（`docs-delta.md`、`review-packet.md`；TaskPackage 从 `delivery/taskpackages/<id>/taskpackage.json` 解析，post-cutoff 包不再要求 delivery/changes/dev/ 内副本）、`delivery/milestones` STATUS、`delivery/workstreams` workstream 收口，并显式执行 best-effort `delivery/changes/dev/INDEX.md` projection。**post-cutoff 包 closeout 完整性条件另含 `pod-retro.md`**（planner 收尾相位，第一人称热记忆；**Q5=warn**：closeout 相位 warn advisory + 零追溯白名单，**非 blocking**——见「closeout 制品 SSOT」表）。planner/supervisor 的复核调用使用默认只读 `node .claude/scripts/closeout-check.mjs`。没有文件记录的实现不能声明 loop 完成。

**治理件引用时序约定**（260824-gov-review-pod-worktree-launch-infra T3，ARQ-260813-cross-worktree-ref-firstclass 降级成文）：dev 包只引用已合进 main 的治理件。若需引用尚未合并的 gov FROZEN proposal，先完成该 proposal 的 main 合并、同步本 checkout，再于 docRefs/origin 引用；不得以 `_note` 散文、origin 类型改写等方式绕过 docs-sync 存在性检查。

### 非 Claude harness 降级 profile

> gov-review-devloop-closeout-hardening §3.4（human-approved 2026-07-27）。本 loop 的机械保全默认假设 Claude Code harness；在其它 harness（Kimi CLI 等）上按下表降级。**降级是证据事实、不是违规**，但承重闸与 human 触点不可跳过。机器件承重 / blocking 分类引「closeout 制品 SSOT」表、不重述（F-02/F-11）。operating-system 指针见 [dev-loop-operating-system.md](dev-loop-operating-system.md) §1。

- **必然失效面**（harness 不提供，须显式应对）：
  - hooks / verify-gates 不自动触发——`taskpackage-required-check` / `supervisor-gate-check` / `dispatch-stamp` 须**手工跑**；
  - 机器产线 `verdict.json` / `verification.json` 不产；
  - **`verify.mjs` round-fact 产线不产**（G6 provenance 产线 = verify.mjs round 证据，`supervisor-gate-check.mjs:228-280` validateVerifierEvidenceRefs〔含 260814-gov-review-merge-gate-terminal-shape-batch T1 红证据 ESCALATE 豁免分支：ESCALATE + 红 fact + `existing-red-inventory.md` 形状检查 → 合法停止等 human 裁，APPROVE 永不入豁免〕；行号实地核正——J4 一次校正 :154-183→:166-195 against 编辑前文件，本批 G6 编辑〔2a import +3 行〕同批再移→:169-198；260814 批 T1-T4 同批再移→:228-280；非 Claude harness 不跑——或显式声明降级 G6 手工替代形态，F-11）；
  - scope-map 无机械强制（靠结构闸 + 纪律兜底）；
  - token telemetry `unavailable`；
  - 触点③「起下一段会话」语义须重释（同会话子 agent 派出，须留痕说明）。
- **约定替代**：G3 闸手工跑——命令 + exit code + 输出摘要记入 `review-packet.md` / `test-evidence.md`（SSOT 手工替代件）；**机械闸 `supervisor-verdict-gate.mjs` 手工跑补承重**（非 Claude harness 不自动触发，纳入 :172 手工跑清单；命令/exit code/输出摘要记 review-packet，FIX-13）——承重闸是机械闸本体，**不复活 supervisor agent**（守 P-6）；brief 注入角色语义且**不引用退役 agent def**（对偶 planner brief 纪律）。
- **不许降级面**（可手工跑，**不可跳过**）：红基线、冻结验收、neverTouch、human 触点（①②③ 或其 §3.6 成文预授权）、supervisor 合并门、`taskpackage-required-check` 与 `closeout-check` 两闸、**`supervisor-verdict.json` 存在（G2 block 判据，F-11 补承重机器件）**、**v2 provenance 手工可核（`closeoutArtifactsChecked` / `verifyLogRefs` 手工比对，F-11 补）**。**注（B5，去自指）**：承重闸 = `taskpackage-required-check`——closeout 相位**程序化调用**它（单源，见收口闸表），非 Claude harness 手工跑它兜底承重；`closeout-check` 本体**不再**自实现 decisionCoverage/story 浅版终检（F-04 删），故「不许降级」两闸 = taskpackage-required-check（承重）+ closeout-check（仅 pod-retro 牙）。

### 轻量 scope 修正通道

> gov-review-devloop-closeout-hardening §3.6（human-approved 2026-07-27）。loop 内对冻结 scope 的**轻量**修正走本通道；超界一律正式 escalation。

- **条件**（全部满足才可走本通道）：① 同包边界内；② 零新依赖；③ neverTouch 零触；④ 不改冻结验收条目。
- **程序**：verifier 定性（judgement-call 归类 + basis 引机器核对结果，**不凭当场主观判断**）→ planner 带时间戳补登记 taskpackage → `review-packet.md` findings 留痕 → supervisor verdict `degradationsConfirmed` 承接。
- **机器核对**（不留当场判断，四条件各自机器可判）：同包边界 / neverTouch 零触 = `git diff` 核对 `scope.files` 外零触 + neverTouch 清单零触；零新依赖 = `package.json` diff 无新增条目；不改冻结验收 = 验收文件 diff 为空。先例锚点 = 260726 verdict「scope.files 恰 14 文件 + neverTouch 零触 + package.json 无条目（零新依赖机器确认）」+ 活文件/快照字节一致核对。
- **超界**：任一条件不满足 → 正式 agent-escalation（planner / human），不得走本通道。

---

## 4. 路径冻结的真实机制（已对照官方文档纠偏）

- 子 agent 的 `tools` 字段是**工具级**允许列表（能不能用 Edit），**不是路径级**。
  仅靠 `tools` **无法**冻结具体路径。
- 真正的路径冻结靠两层：
  1. **`permissions.deny`**（`dev-loop.settings.json`）：如 `Edit(./docs/contracts/**)`，
     精度高、`deny > ask > allow`、托管层不可被覆盖。
  2. **PreToolUse 钩子**（`enforce-scope.mjs`）：从钩子输入读 `agent_type` 与
     `tool_input.file_path`，按 `scope-map.json` 判定，越界 `exit 2` 拦截，理由回灌给模型。

本仓库已对钩子做过行为验证（builder→contracts 拦截、builder→apps 放行、
acceptance-author→验收放行、verifier→全量禁写、未登记 agent/主线程不干预）。

---

## 5. 验证阶梯（`verify-gates.json`）

| 类别 | 闸门 | 含义 |
|---|---|---|
| 上下文闸 | `taskpackage-required` | dev-loop 必须有已落盘 TaskPackage；缺 `changeId/sourceType/sourceRefs/deliveryImpact/planningImpact/riskTier/gateProfile/story/acceptance/红基线/exitCriteria` 等关键字段直接红。 |
| 结构闸（必要非充分） | `firmware-build`（idf.py build）→ `contract-lint` | 编不过/类型错/契约漂移直接红，不进功能闸。 |
| 功能闸（对故事负责） | `acceptance-coverage` → `test:unit` / `test:acceptance`（声明则加 `api`/`ui`） | 必须断言真实结果；空过等于没测。 |
| 安全闸（按风险启用） | `security` | `securityReviewRequired=true` 时必须记录输入、权限、数据隔离、secret、AI tools 等安全证据。 |
| 文档/治理闸 | `docs-sync`（TaskPackage 上下文中启用） | 实现行为变化时，相关 spec/AGENTS/HANDOVER 必须同步或记录无需更新理由。 |
| 收口闸 | `closeout-check`（integrator 显式 `--update-index`；planner/supervisor 默认只读复核） | 合并门前必须有 `delivery/changes/dev/<id>` 两个 blocking 标准制品（`docs-delta.md`、`review-packet.md`）、`delivery/milestones` STATUS 无漂移、`delivery/workstreams` workstream 记录完整；TaskPackage 从 `delivery/taskpackages/<id>/` 解析；**承重字段/decisionCoverage 校验由 closeout 相位程序化调用 `taskpackage-required-check` 单源承重**（F-04，本体不浅版重实现）；**post-cutoff 包 closeout 完整性条件含 `pod-retro.md`（Q5=warn advisory + 零追溯白名单，非 blocking）**；verdict/verification、loop metrics、pod journal 缺失为 advisory。 |

判定**确定**（来自退出码），LLM 只把失败解读成 ErrorEnvelope + 根因假设。
`verify.mjs` 产出的 immutable verifier-round fact 以本地 closed schema 承载 ErrorEnvelope 关系；ErrorEnvelope 形状与 `contracts/shared-types`（K6）方向对齐，
但**不 import K6 运行时**——保持循环对内核零依赖。
`fixer` 修复真实缺陷后必须补 guard 测试/断言；如果无权修改该测试层，则升级回 planner。

### 合并门 Review Packet

integrator 在 supervisor 合并门前必须产出 Review Packet，辅助 supervisor/人类 review：

- change size：文件数、diff 规模、scope、是否建议拆分。
- verification evidence：结构闸、功能闸、安全闸、mutation check、docs-sync。
- design-review evidence（若 TaskPackage 声明 `designReviewRequired`）：integrator 在 bootstrap 期只落**文本**证据——viewport 覆盖摘要、a11y 摘要、design-drift 摘要、外部 artifact 引用（若有）、residual risk；**不**把 screenshot / video / HTML 证据提交进仓库。
- findings：按 `BLOCKING` / `SHOULD` / `NIT` / `OPTIONAL` / `FYI` 分级。
- five-axis review：correctness、test quality、contract/boundary safety、operability/security、maintainability/simplicity。
- **未覆盖面申报（260920-gov-review-bearing-strength-align T3，FROZEN v1.0）**：对照本包 `acceptance[]` 集合 + 本包引用的契约条目 + roadmap §3 效果段，列出**本次无齿 / 未跑的设计性质**，逐条一行写「性质 + 现状 + 去向（deferred 到哪包 / `structural` 豁免 id / 由真机轮或 human 持板兜 / 明确接受不覆盖）」。**零缺口时明写 `none`**（防「没写」与「没有」混同）。本段管**齿覆盖轴**，与锚 3 demo 对账行（管**证据形态轴**）正交、同址（口径见 `pod-coordination.md` §11）；**义务的生产者可达性由 planner Step 11 的 dispatch prompt 携带**（与锚 3 同构，`integrator.md` 不因此改）。既存实况：本仓 33 份 dev review-packet 中 23 份出现过未闭环类措辞，但**位置与粒度各异**（如 `260920-m2-charwindow-display-fix/review-packet.md:43` 的 MCD-A6「未做、deferred」写在 Demo 对账行表内）——本段把该申报固定到统一位置。
- residual risk：已知限制、后续建议、人类决策点。
- S2 exploratory evidence（若 TaskPackage 声明）：required/executed、runId、goals、终态 status、structured runResult summary、integrator triage、采纳进 findings 的项目、knowledge persisted / not persisted with reason，以及可选 artifact refs；不复制 raw screenshots / traces / logs。`queued` / `running` 不得作为 pending evidence 进入 supervisor；`cancelled` / `interrupted` 默认 evidence incomplete，除非 integrator 采纳其本身为 finding。

任何 `BLOCKING` finding 都意味着不能进入合并门，必须交回 planner / fixer / 人类裁决。supervisor 合并门（机械闸 `supervisor-verdict-gate.mjs`）的低/中风险自动放行需要同时满足：真库/真实结果验收可信、mutation 逐 AC 敏感、closeout 全绿。全 mock、跨系统、core.*、安全轴触发为高风险；docs/contracts/**、迁移、删数据为 critical。**本行是 riskTier 机械判据的 SSOT**：`supervisor-verdict-gate.mjs` 是本判据的机械投影（非第二权威），改判据先改本行再同步该脚本（反漂移 C-5）。

### 抽检

supervisor 自动放行的是低/中风险变更。人类不逐单卡住这些变更，但每个统计周期按 **10% 且不少于 3 单** 做事后抽检，用来校准机械闸；发现漏判，下周期升到 **25%**；连续 2 个周期无漏判可降到 **5%**。高风险和 critical 不抽检，直接升级。**抽检执行主体 + 载体 + 计数（FIX-9，auto-merge 随 260831-gov-review-auto-merge-local-main 落地即恢复）**：执行主体 = loop-retro cadence（周期机械触发）；机械载体 = loop-metrics `terminalOutcome` 枚举 verdict=APPROVE 包（`loop-metrics.mjs:322/:347`）；确定性抽样 = `hash(diff) mod N`（supervisor-gate-check:11 R10 lineage）取 10% 且 ≥3 → human 复核质量层；计数 = loop-retro cadence + GC5 式增量计数（漏判→25%，连续 2 周期无漏判→5%）。

### ADR / 决策归档

ADR 只记录 why，不复制 contracts/spec 的 what。以下情况需要 `adrRequired=true` 或 integrator 在 docs-delta 中标记：契约变更批准/拒绝、系统边界变化、事件/tool/shared type 取舍、选择 adapter 而非 contract change、接受残余风险或推迟迁移/安全事项。

> 这些命令面向 monorepo 合并完成后的形态（见 `MERGE-MONOREPO.md`）。脚手架阶段
> 某些包级测试命令（如 `test:acceptance`）在脚手架阶段可能尚未就绪，按子系统就绪情况增删 `gates`；`contract:lint`、`acceptance-coverage`、`mutation-check`、`docs-sync` 必须是真闸，不能用桩脚本空过。

---

## 6. 运行方式

**单子系统（顺序循环）：**

```powershell
# 在子系统对应的工作树根目录
claude --settings .claude/dev-loop.settings.json --agent planner
# 然后在会话里：
/build-feature S1-A1 销售提交线索后能在列表中看到该线索
```

planner 会按 `commands/build-feature.md` 推进，并在契约人工门或 supervisor ESCALATE 时停下等你。

**真并行（多子系统）= 多 worktree + 多会话**，而不是一个会话里同时派多个 builder：

```powershell
git worktree add ../wt-s3 -b feat/s3-quote
git worktree add ../wt-s5 -b feat/s5-delivery
# 两个终端分别：
cd ../wt-s3; claude --settings .claude/dev-loop.settings.json --agent planner
cd ../wt-s5; claude --settings .claude/dev-loop.settings.json --agent planner
```

每棵 worktree 是独立 checkout，verifier 看到的是同会话 builder 刚写入的树；
跨包冲突留给 integrator 收口。要更强的**单包硬隔离**，复制 `agents/builder.md` 为
`builder-s3-quote.md` 并在 `scope-map.json` 加一条
`"builder": { "allow": ["firmware/**", "server/**"], "deny": [...] }`（钩子会精确到包）。

> 跨平台：钩子用 Node 写（`*.mjs`），Windows/macOS/Linux 通用，无需区分 bash/powershell。

---

## 7. Bootstrap 阶梯（先有循环，后有内核）

| 层 | 形态 | 依赖 |
|---|---|---|
| **L0** | 人手在终端跑 `/build-feature`，主线程编排 | 只需 Claude Code + `contracts/`（zod） |
| **L1** | 薄编排：本文件这套 `.claude/` 脚手架 | 同上 |
| **L2** | 独立的循环引擎（自带任务队列/并发，脱离单会话） | 仍只依赖 `contracts/` |
| **L3** | 接入自家 K6：把 `verify.mjs` 的 ErrorEnvelope/日志写进 `LogEntry`，按 correlationId 串链 | 需要 K6 已建成（**后期奖励**，非前置） |

K6/K9 是这条循环的**产物**，不是它的前置依赖。L3「吃狗粮」是锦上添花，不是起点。

---

## 8. 与产品侧的同构

这套开发期范式与产品侧「K9 编排 + 事件驱动 + 6 人工门控 + K6 可观测性」**同构**：

- planner ↔ K9 orchestrator；契约变更门 ↔ 6 人工门控的「事先冻结」；
- verify 闸门 ↔ S2 AI Tester 的判分；ErrorEnvelope ↔ S2 failure-analyzer 的公司级提升；
- 多 worktree 并行 ↔ 模块按所有权边界并行。

未来内核成形后，可把 L2 引擎并入 K9，把 `verify.mjs` 的输出灌进 K6——届时「公司建造自己」
和「公司运营业务」就跑在同一套基础设施上。

---

## 9. 机器校验格式契约（输出标准的豁免面索引）

AGENTS.md「输出标准」节要求散文通顺完整，下表所列格式由脚本校验或由约定钉死，是通顺化的豁免面——散文可改，格式逐字节不动（2026-08-14 gov-review-unified-output-standard 批建立，清单随脚本演化以本表为维护面）。

| 契约 | 校验脚本:锚点 | 校验内容 |
|---|---|---|
| landing-manifest 围栏块 | `landing-fidelity-check.mjs:16,:24` | PROPOSAL 与 landing-summary 必须含 ```landing-manifest 围栏块（B2 fail-loud） |
| Status 行 FROZEN 凭据 | `landing-fidelity-check.mjs:11,:119` | Status 行含 FROZEN 标记（仅 Status 行有效） |
| packet-binding 行 | `landing-fidelity-check.mjs:12,:121-122` | landing-summary 含 packet-id + 被批 review id |
| verifier-round-fact 字段闭集 | `verifier-evidence-check.mjs:280-291` | 14 字段闭集 + artifactType/authority/phase/ok 类型断言 |
| supervisor-verdict 字段 | `supervisor-gate-check.mjs:51,:312-320` | verdict 枚举 + provenanceVersion='2.0' + mechanicalSupervisorGate 字段 |
| pod-events 事件 envelope | `pod-events-append.mjs:42-48` | eventType 闭集 + envelope 校验 |
| taskpackage 必填字段 | `taskpackage-required-check.mjs:140,:185` | 8 字段必填 + changeId YYMMDD- 前缀 |
| CodebaseMemory marker 块 | `AGENTS.md` 首部（工具方 re-index 重写面） | marker 块内文由工具托管，非手写面 |
| 队列表格列结构 | `ARCH-REVIEW-QUEUE.md` §1 表头（6 列约定） | 行内列数错位即坏行（约定非机检，列结构为簿记纪律） |

**单源注**：围栏块与 Status 行校验的字节级单一源在 `.claude/scripts/lib/governance-gate-shared.mjs`（extractManifestBlock / extractTargets / isFrozen），表内锚点指向 landing-fidelity-check.mjs 的描述与消费面；Status 行契约另有消费方 closeout-check.mjs 与 proposal-consolidation-check.mjs（同一 isFrozen 单源）。修式样者以 lib 单源为准。
