# Task Intake and Slicing

This reference is mandatory for planner intake. It decides where a piece of work came from, how large the executable slice should be, whether `.planning/` is needed, and which gates must be enabled before builder work starts.

## Core Rule

Every formal implementation-facing change starts as a TaskPackage. `.delivery/` is the milestone denominator, `.planning/` is long-lived work memory, and `.claude/taskpackages/<change-id>/` is the executable contract for one dev-loop slice.

Do not use `.delivery/` or `.planning/` as a substitute for a TaskPackage.

Acceptance vs exitCriteria authoring convention (A10-type): `acceptance[]` holds only test-assertable items; process/evidence items belong in `exitCriteria`. Non-id-testable entries use explicit object markers — `{ "id": "X", "kind": "structural" }` (non-code-testable) or `{ "id": "X", "suiteGate": true }` (suite-level regression gate; positive list/negative rule apply, and the two markers are mutually exclusive). Acceptance-prose word-washing is removed and forbidden, and bare-suffix / interval-abbreviation mentions (e.g. `A1..A14`) no longer exempt — write full ids or explicit object markers. See `.claude/references/dev-loop.md` §3 (变更制品) for the convention and its red-verdict guidance.

## Required Intake Fields

Each TaskPackage must include:

```json
{
  "sourceType": "arch-review",
  "origin": {
    "upstreamType": "proposal",
    "upstreamRef": ".planning/<ws>/PROPOSAL.md",
    "upstreamFrozen": "<version/sha>"
  },
  "sourceRefs": ["D-A1"],
  "deliveryImpact": "milestone-affecting",
  "planningImpact": "attach-existing",
  "planningRefs": [".planning/devpc-review-remediation/"],
  "riskTier": "high",
  "gateProfile": ["contract-drift", "types", "real-db"]
}
```

> **changeId 命名约束（gov-review-devloop-artifact-lifecycle §3.2）**：强制 `YYMMDD-slug` 形状（`^\d{6}-[a-z0-9-]+$`），如 `260722-fix-lead-dup`。既有 78 目录 + 存量包零追溯（git 首提 < 2026-07-23 = 老包放行，两 G3 脚本共享同一机判谓词）。日期由建包方主线程注入，脚本只校验字面合法（月 01–12、日 01–31）。详见本文件 `### changeId` 小节。

### origin

`origin` 是一等强键（`gov-review-devloop-artifact-lifecycle` §3.1），把「溯源」从 `sourceRefs` 字符串数组中拆出结构化为机器字段。`origin` 与 `sourceType` **至少一个存在**；两者并存时取值须一致（`origin.type=human-confirmed-plan` 例外——该值无 sourceType 镜像，须 sourceType 缺省）。过渡期老包（有 sourceType 无 origin）放行不红——鼓励迁移但不追溯。

TaskPackage 层 `origin` 形状（G3 读取此层，与 workstream 层（task_plan frontmatter）字段名有意不同）：

```jsonc
"origin": {
  "upstreamType": "proposal",
  "upstreamRef": ".planning/<ws>/PROPOSAL.md",
  "upstreamFrozen": "sha1:<16hex>",
  "workstream": "<ws-slug>"
}
```

- **`upstreamType`**（必填）：取值 = `sourceType` 9 枚举（`delivery-story`、`planning-workstream`、`arch-review`、`human-ad-hoc`、`ci-failure`、`drift-report`、`regression`、`maintenance`、`proposal`）**+ 一个受控扩展 `human-confirmed-plan`**。`human-confirmed-plan` 是过渡性——专为「人确认过、非正式 proposal 的设计权威」（如 s1-nsa design-plan.md）设，其 `origin.type` 不映射到任何 `sourceType`（两者并存→红）。
- **`upstreamRef`**（proposal 型必填，其余 type 可省略）：指向上游设计目录——proposal 型指向 FROZEN PROPOSAL 路径（如 `.planning/<ws>/PROPOSAL.md` 或 `changes/(arch|dev)/<id>/PROPOSAL.md`）。非 proposal 型（regression/ci-failure/…）指向触发来源（issue / drift 报告 / 人类指令记录）。**proposal→docRefs back-check（完整性闸）**——触发判据为**析取** `isProposalByType ∨ isProposalByPath`（260731 Phase 2 T1 路径枢轴：`isProposalByPath` = `origin.upstreamRef ∨ origin.ref` fragment-stripped 走 G3 `isProposalMdPath`；`isProposalByType` = `sourceType/originType === 'proposal'` 字符串判，析取保老包）时 `origin.upstreamRef`（fragment-stripped）**必须** ∈ `docRefs[]`（新包 / 路径枢轴包——origin.ref 接管 back-check；老包（sourceType=proposal、originType 非 proposal、ref 非 proposal 路径）维持 sourceRefs-based back-check 不误红，过渡 carve-out）。**decisionCoverage 承重触发维持字符串判**（`isProposalByType`），不随路径枢轴（OQ4-B：hybrid 不被要求 in-taskpackage DC，其 DC 在 workstream 级）。
- **`upstreamFrozen`**（**仅 origin.upstreamType=proposal 必填**、占位符无效；其余 type 可空、不触发 UPSTREAM_DRIFT）：钉上游 PROPOSAL 被批时间点，用于 `closeout-check` 检测**上游悄改漂移**（UPSTREAM_DRIFT = **warn/告警，非红拦**——convention-only，机器齿记 ARQ-260723-artifact-lifecycle-teeth）。**标准 pin 格式（260731-gov-review-artifact-lifecycle-origin-teeth Phase 1 FROZEN；Phase 2 同批改写 + carry 再确认，H4 批准）**：`sha1:<16hex>`，算法 = `'sha1:' + createHash('sha1').update(fileContent.replace(/\r\n/g, '\n')).digest('hex').slice(0,16)`（**raw file bytes 行尾归一化（`\r\n` → `\n`）后 hash**，G3 `computeFrozenPin` 共享单源，与 `closeout-check.mjs` 消费端一致）。**不是** `git hash-object`（后者含 git blob header `"blob <size>\0" + content`，对同一文件产出不同 hash，结构性不可比）。**过渡期**：prose pin（如 `"frozen v0.2 2026-07-29 (...)"`）及 `git hash-object` 产物均为非标准格式 → convention-only-skip（不判 drift），不追溯为红；新包冻结时应使用标准 raw-content-sha1 格式。
- **`workstream`**（可选）：归属的 `.planning` workstream slug，供 `portfolio-planner` 拆包 + 反向反查消费方。

**小 bug 包 origin 填法**：triage 判②单包小 bug → `origin.upstreamType = regression | ci-failure | drift-report`（按触发源，**非 proposal**），`origin.upstreamRef` 指向触发来源（issue / drift 报告 / 人类指令记录），**无需 FROZEN PROPOSAL**——小 bug 上游不是 proposal。`origin` 仍是一等强键，只是 type 非 proposal。

**hybrid 包 origin 推荐填法（accepted-risk convention，260731-gov-review-artifact-lifecycle-origin-teeth Phase 1 FROZEN）**：多包 regression 经 FROZEN PROPOSAL 拆包（如 260729-s1-chat-interaction-fixes 实例）= **hybrid provenance**（触发源=regression，拆包上游=FROZEN PROPOSAL）。推荐填法：

```jsonc
"sourceType": "regression",
"origin": {
  "type": "regression",            // 与 upstreamType 镜像同值（桥接 ARQ-077 键不对称）
  "upstreamType": "regression",    // 触发源，非 proposal
  "upstreamRef": ".planning/<ws>/PROPOSAL.md#§x.y",  // 仍指向 FROZEN PROPOSAL（fragment 可加）
  "upstreamFrozen": "sha1:<16hex>",  // 标准 pin（见上）；过渡期 prose pin 可接受
  "workstream": "<ws-slug>"
}
```

**Phase 2 新 producer 义务（260731 T1/T2, FROZEN 2026-08-01）**：hybrid 包自本批起触发 proposal 级 back-check（路径枢轴 `isProposalByPath=true`）——**fragment-stripped `origin.upstreamRef` 必须 ∈ `docRefs[]`**，否则 `taskpackage-required-check` hard-fail（新义务）。推荐填法维持上形 + `docRefs[]` 显式含该 PROPOSAL 路径（260729 两实例已满足）。DC 承重不变：hybrid 不被要求 in-taskpackage `decisionCoverage`（OQ4-B）。

**accepted-risk 披露（Phase 2 FROZEN 2026-08-01 更新）**：hybrid 包的 FROZEN PROPOSAL 上游**已获 proposal 级 back-check + UPSTREAM_DRIFT 保全**（T1/T2 路径枢轴 + leg ③ FROZEN 守卫 warn-only）。**唯 decisionCoverage 承重维持字符串判**（OQ4-B：hybrid DC 在 workstream 级）。ARQ-260729 Phase 1 闭合；ARQ-260723 本批闭合。**过渡噪声**：260726 git-hash-object pin → UPSTREAM_DRIFT 永久假阳 advisory（warn 不阻塞）。

**origin 带来三个机器能力**：① portfolio-planner 读一份 FROZEN PROPOSAL 即拆，不扫字符串；② 结构化字段 → 脚本可从 PROPOSAL **反查所有消费方**（免费补 backref 缺口）；③ `upstreamFrozen` 钉版本 → 上游悄改可报 `UPSTREAM_DRIFT`（closeout warn，非红拦；convention-only）。

**已存在 portfolio taskpackage → 权威消费（F-06，对偶 planner.md Step 1 F-02）**：intake 命中已 commit 的 portfolio taskpackage（`origin.upstreamType`/`origin.workstream` 已定、`rigor`/`riskTier` 已定）→ planner **以其 `sourceType`/`origin`/`rigor`/`riskTier`/`gateProfile`/`decisionCoverage` 为权威**，只补红基线/验收落盘与 loop 驱动，**不回退重归一、不覆盖 sourceType**。重归一成 `human-ad-hoc` 会丢整个决策集（decisionCoverage）+ riskTier 错配（Full/high→Lite/low）绕开本该触发的 supervisor 人类升级门——本规则即 governance `ARQ-260729-portfolio-handoff-failclosed` 治理的事故根因修复。origin 填法口径与 `.claude/references/change-triage.md` 单消费对齐（挂 ARQ-077 `.claude/**` 一致性面）。

### changeId

`changeId` 是 TaskPackage 的唯一标识符，**强制 `YYMMDD-slug` 形状**（`^\d{6}-[a-z0-9-]+$`，如 `260722-s1-nsa-engine-hardening`）。既与新强键 `origin` 配合落地，也与 `sourceType` 保持向前兼容。

- **日期前缀**：`YYMMDD` 由建包方（portfolio-planner 拆包 / `/triage-change` 路由 / planner 建包）**人手注入**。脚本只校验日期字面合法（月 01–12、日 01–31），不推断建包时刻。`YYMMDD` 日期是近似主张——字典序=时间序，非精确建包时戳。
- **零追溯（DP-2 git 首提日期 cutoff）**：既有 78 目录 + 存量 `sourceType` 包不强制前缀。白名单谓词（单一、机判，两 G3 脚本 **convention 级同步同一语义**）：**包所落目录的 git 首提日期 < landing cutoff（2026-07-23）= 老包放行**；cutoff 及之后建包 = 新包强制前缀。禁用「不匹配前缀正则即放行」。git 不可用时降级：有前缀则验，无前缀则放行（convention 兜底）。INDEX 回填 git 首提日期为近似（与建包时刻可能不符）。
- **跨包引用**：`decisionCoverage` `package:<changeId>` 语法 `[a-z0-9-]+`——`260722-slug` 天然满足，前缀方案不破跨包引用。

### sourceType

Use one of:

| sourceType | Use when |
|---|---|
| `delivery-story` | Planner creates the TaskPackage directly from a committed delivery milestone or canonical acceptance item that is small enough for one loop slice. |
| `planning-workstream` | The slice is a child of an existing `.planning/<workstream>`. |
| `arch-review` | The source is an architecture review finding, ARQ/CDQ row, contract gate condition, or review packet. |
| `human-ad-hoc` | A human gives a one-off task that is not already in delivery or planning. |
| `ci-failure` | A PR/CI/verifier gate failure triggers the task. |
| `drift-report` | A scheduled or manual drift report triggers the task. |
| `regression` | A behavioral regression from manual QA, acceptance, or user feedback triggers the task. |
| `proposal` | 来源为 `changes/(arch|dev)/**/PROPOSAL.md` 或 `.planning/<ws>/PROPOSAL.md`（大 feature 设计权威，gov-review-architect-change-triage §3.6）类设计方案；载荷 = 决策集；蒸馏完整性规则强制适用（**含 Lite，见 decisionCoverage 规则**）。`human-confirmed-plan` **不是** sourceType 枚举值（其正名归 artifact-lifecycle 的 origin.type，本表不预支）。 |

`sourceType` records origin, not impact. If an arch-review task changes milestone health, keep `sourceType: "arch-review"` and set `deliveryImpact: "milestone-affecting"`.

### sourceRefs

`sourceRefs` is an array of stable, grep-able refs from the source system:

- Delivery/spec: `S1-A1`, `K7-P0`, `docs/systems/S1-leads/spec.md:S1-A1`
- Architecture review: `ARQ-063`, `D-A1`, `F3`, `ARQ-063-C1`
- Human ad-hoc: `TEMP-YYYY-MM-DD-short-slug`
- CI/drift: `CI-types-2026-07-05`, `DRIFT-D6-CREATE-TABLE-IN-TEST`
- Proposal: `changes/(arch|dev)/<id>/PROPOSAL.md#§x.y`，或大 feature `.planning/<ws>/PROPOSAL.md#§x.y`

For `sourceType=proposal`, `sourceRefs[]` must include the proposal `.md` path(s), and each such path must also be present in `docRefs[]`. A "proposal .md path" is a path ending in `PROPOSAL.md` under `changes/arch/`, `changes/dev/`, **or `.planning/<ws>/`**（大 feature 设计权威；gov-review-architect-change-triage §3.6 把合法落点扩到 `.planning`，G3 `isProposalMdPath` 已同步扩认）. Chapter-level refs may use URL fragments (e.g. `path#§x.y`) for human readability; G3 validation strips the fragment before checking `docRefs[]` membership.

> **DP-1 过渡 carve-out**：上述规则仅适用于既有 sourceType=proposal 老包（无 origin 或其 type 非 proposal）。新包（`origin.upstreamType=proposal`）的 proposal→docRefs back-check 由 **`origin.upstreamRef` 接管**——`origin.upstreamRef`（fragment-stripped）**必须** ∈ `docRefs[]`，`sourceRefs` 不再承载 proposal 路径（退本职为代码范围锚点）。详见 [task-intake-and-slicing.md](#origin) `### origin` 节。

### deliveryImpact

Use one of:

| deliveryImpact | Meaning |
|---|---|
| `none` | Does not change milestone state or committed scope. |
| `milestone-affecting` | May block, unblock, or advance a `.delivery` milestone. |

Changing `.delivery/milestones.jsonl` is still an integrator/human closeout act. `deliveryImpact` only tells the loop to inspect milestone health.

### planningImpact

Use one of:

| planningImpact | Meaning |
|---|---|
| `none` | Single slice; do not create `.planning/`. |
| `attach-existing` | Append progress to an existing `.planning/<workstream>`. `planningRefs[]` is required. |
| `create-workstream` | A new workstream is needed. Creating the top-level workstream is `portfolio-planner`'s authority (the cross-Pod RTE), not planner; a single-Pod planner only uses `attach-existing`. `planningRefs[]` is required. |
| `needs-triage` | Planner lacks enough information. Stop, ask the human or do read-only discovery. Do not send to builder. |

Create `.planning/<workstream>` only when future agents need durable why/findings/progress across multiple slices. If the diff plus TaskPackage is enough, do not create a workstream. Creating a top-level workstream (and decomposing work into multiple TaskPackages) is `portfolio-planner`'s authority; a single-Pod planner only attaches to an existing workstream. If multi-Pod decomposition is needed, route to `portfolio-planner`.

> **workstream 创建权的两半（gov-review-architect-change-triage §3.4 / F4 / Q4，human-confirmed 2026-07-22）**：上面"create-workstream = portfolio-planner 权限"指的是**拆包+调度态建立**（把 workstream 拆成 N 个 TaskPackage、建 `task_plan`/`pod-board`/`progress`）。另有**设计权威落家**一半：`/triage-change` 的人类/主线程可在 triage 时创建 `.planning/<ws>/` **仅用于装 reviewed FROZEN PROPOSAL（+ review.md）**，供 portfolio-planner 后续拆包消费。portfolio-planner 的拆包+调度权**不垄断建目录**。这是对 ARQ-065/066 RTE 集权的**有意放宽（deliberate relaxation，非回归）**，与 `architect-operating-model.md` §4「human or planner main thread creates `.planning/<workstream>`」对齐。

### riskTier

Use one of `low`, `medium`, `high`, `critical`.

Default escalation rules:

- `critical`: `docs/contracts/**`, migrations, data deletion, irreversible data movement.
- `high`: `core.*`, cross-system behavior, security axes, all-mock acceptance, AI cost/audit correctness, schedulers/state machines with user/business impact.
- `medium`: single subsystem behavior with real acceptance and bounded blast radius.
- `low`: docs/tooling/cleanup with no runtime behavior change.

### gateProfile

Use a non-empty list. Suggested values:

- `types`
- `contract-drift`
- `real-db`
- `migration`
- `test-quality`
- `evidence-integrity`
- `docs-sync`
- `security`
- `ai-cost-audit`
- `scheduler`
- `state-machine`
- `mcp-tool-contract`
- `notification`
- `none-runtime`

The verifier/integrator may map these profiles to concrete scripts as the repo matures.

## Slicing Matrix

### 垂直切片维（gov-review-devloop-pocock-adoption C1）

Lite/Full 讲大小与风险；本维讲**包边界的形状**：每个 TaskPackage 默认应承载一个完整能力/行为结果，并且完成后可独立演示或验证（tracer-bullet 垂直切片）。`verifiable` 是完成结果的证据，不是把任意技术层横切包装成合法 slice 的逃生口。有 UI 的 slice 可从用户任务演示；L0 backend/kernel/contract/script/migration/internal-service slice 可用 API 响应、DB 状态、事件流、CLI 输出或敏感测试独立验证，**不要求用户可见 UI**。

- 默认不按 DB→API→UI 这种不能独立闭合能力/行为的技术层横切。这里约束的是**包边界**，不是要求每包必须同时改 schema/API/UI；只触一层但该层改动本身就是完整交付结果（例如修正一个 API 错误响应、一个事件投递 bug、一个可运行 CLI 行为）的 bugfix/internal-service/tooling change 仍是合法垂直切片。仅仅「这批 DB/API/UI 改动各自有测试」不足以证明它是垂直切片。
- **wide refactor 例外**：rename/retype shared symbol、schema/contract evolution 等单次机械改动若 blast radius 使任何 tracer bullet 都无法独立保持 green，可按 `expand → migrate batches → contract` 拆包。expand/migrate 中间包不必 user-visible，但必须有独立可验证状态并声明 blocking edges。若连 migrate batch 都无法独立保持 green，本维不定义额外分支/集成机制；需另开治理变更设计集成策略。

Use a Lite TaskPackage when all are true:

- one source ref or a small tightly-coupled cluster;
- one owning subsystem/package;
- no contract/human decision is pending;
- no migration or data deletion;
- expected to close in one verifier/fixer loop.

Use a Full TaskPackage when any are true:

- high/critical risk;
- multiple gates beyond normal unit/acceptance;
- real DB, scheduler, state machine, AI cost/audit, security, or MCP contract behavior;
- multiple filesystems or packages;
- closeout evidence needs claim verification.

Create or attach `.planning/<workstream>` when any are true:

- multiple TaskPackages will be needed;
- findings form a remediation campaign;
- work spans multiple systems or multiple sessions;
- decisions/progress must be durable for future agents;
- milestone state will be blocked/unblocked over time.

Stop with `planningImpact: "needs-triage"` when the planner cannot determine the slice, owner, risk, or required gate profile.

## rigor 字段与 Lite TaskPackage 字段级定义（gov-review-architect-change-triage §3.5）

`dev-loop.md` §0 声明 progressive rigor，本节给出字段级定义。TaskPackage 用 `rigor` 字段标档：`"rigor": "lite" | "full"`（缺省按 full 既有逻辑走；存量无 `rigor` 的包不追溯，校验白名单放行）。

**Lite 省的是"方案/决策映射的文牍"，不是 loop 的任何一道闸。** 验收先行、红基线、verifier 证据、closeout、supervisor 合并门——一道都不跳。

- **不可省**（守 dev-loop 铁律）：`changeId` / `rigor` / `sourceType` / `sourceRefs` / `riskTier` / `gateProfile` / `story` / `planRef` / `acceptance`（含红基线）/ `exitCriteria` / `scope` / 合并门。
- **可省/简化**：**仅 bug 类（非 proposal 来源）**的 `decisionCoverage`（**proposal 来源含 Lite 仍必填不豁免，见下**）、S2/design-review/security 重闸（按 riskTier 关）、多包依赖图（`mergeOrder`/`dependsOn`/`acceptanceIdMap`）、`fixBudget` 调小。
- **`planRef` 指向 sibling `plan.md`**：taskpackage.json 是机器解析的，reviewed plan 是长 prose，prose 归 markdown（与验收是 sibling `.acceptance.spec.ts`、由 `acceptanceTestsFile` 引用同模式）。plan.md 是 `/triage-change` 分流门 architect 审过的，planner **只读消费、改需留痕**。

**riskTier 优先于 rigor（单向守卫）—— convention-only（F1 / Q1-A）**：设计意图是单向校验——若信号说"这是 Full 级"（security 轴 / 迁移 / 跨包 / core.* / 触契约）而 `rigor:"lite"` → 应强制升 Full。**但该机器守卫当前不落地**（`taskpackage-required-check.mjs` 本批只扩 `isProposalMdPath`，不含 riskTier-overrides-rigor 校验，记 ARQ 后续 G3）；landing 时靠**两层人工**兜底：① `/triage-change` 人类触点①确认/改判路由；② supervisor 对 riskTier=high 仍升级人类。判断归 architect（写进裁决 rationale）。

**decisionCoverage 与 rigor 的关系（F2）**：`sourceType=proposal` 的包——**包括 Lite**——`decisionCoverage` 必填不豁免。「Lite 省 decisionCoverage」仅适用于非 proposal 的 bug 类小改动（regression/ci-failure/drift-report，无 proposal 决策集）。G3 `taskpackage-required-check.mjs` 对 `sourceType==='proposal'` 强制 decisionCoverage 必填，与本节一致。

## UIUX Depth Ladder and Optional TaskPackage Fields

When a slice touches user-visible UI, planner detects a UIUX depth level and, from Level 2 up, records UIUX context on the TaskPackage. Depth drives whether a `uiux-author` step runs and what design-review evidence integrator owes at closeout.

| Level | Use when | Required action |
|---|---|---|
| L0 | Pure backend/contract/script/migration/internal service; no user-visible UI | `uiuxRequired=false` (or key absent); no UIUX author or design review |
| L1 | Existing blueprint + component pattern; small status/field/button/message change | UI-state acceptance only |
| L2 | New local user flow or complex states; visual direction follows existing system | `uiux-author` creates an interaction brief from the frozen system brief |
| L3 | New page skeleton, key dashboard/workbench, important IA change, or human needs to see it first | OpenDesign visual reference + human design direction as needed |
| L4 | System navigation model, shell, shared UI, cross-system components, multiple pages | System blueprint first; then `portfolio-planner` slices TaskPackages |

Existing systems without a full blueprint may continue L1 work. L2+ requires at least a **baseline** `UIUX-DESIGN-BRIEF.md`, which may be reverse-engineered from current implementation/spec — no full OpenDesign recreation required.

The UIUX fields are an **OPTIONAL extension**, not part of the required intake field set. `taskpackage-required-check.mjs` is unchanged and its hardcoded required list excludes them; a Level-0 package needs no `uiuxRequired` key at all. Add them only from Level 1+ (some) / Level 2+ (interaction brief):

```json
{
  "uiuxRequired": true,
  "uiuxLevel": 2,
  "designRefs": ["docs/systems/<S>/UIUX-DESIGN-BRIEF.md#pageId-x"],
  "sharedUiRefs": ["docs/ui/design.md#component-action-card"],
  "localDesignArtifactRefs": ["changes/arch/uiux-<system>-blueprint/_artifacts/<snapshot>"],
  "artifactPackageRef": "<external zip path or future S5 artifactId>",
  "uiAcceptanceRefs": ["pageId-x.stateId-y"],
  "designReviewRequired": true,
  "designReviewEvidenceRequired": true,
  "repoVisualArtifactRequired": false
}
```

`designRefs[]` primarily point to stable `UIUX-DESIGN-BRIEF.md` sections/IDs; OpenDesign artifact refs are supporting visual context and cannot replace the frozen textual contract. If `UIUX-DESIGN-BRIEF.md` changes **semantically** while a UI TaskPackage is in flight, affected packages pause and update their design refs/acceptance; unaffected packages continue.

## Distillation Integrity (decisionCoverage)

`decisionCoverage` is an optional TaskPackage field that becomes **mandatory** for `sourceType=proposal`. It records how each decision/constraint from the source was carried into the loop: landed in an acceptance test, landed in an exit criterion, deferred to another package, or explicitly dropped with a reason. It is a **mapping layer**, not a replacement for the A10 acceptance-vs-exitCriteria convention: `acceptance[]` still holds only test-assertable items, and `exitCriteria[]` still holds process/evidence items. `decisionCoverage` proves the **completeness** of the acceptance/exitCriteria set against the source decision set; A10 proves the **assertability** of each item.

Shape:

```json
"decisionCoverage": [
  {
    "decision": "D1",
    "summary": "Polish only runs when substantive content changes (three-way check).",
    "sourceRefs": ["changes/dev/s1-ai-summary-projection-polish/PROPOSAL.md#§4.2"],
    "landsIn": "acceptance:POL-B2"
  },
  {
    "decision": "D2",
    "summary": "Unit tests cover debounce merge and failure fallback.",
    "sourceRefs": ["changes/dev/s1-ai-summary-projection-polish/PROPOSAL.md#§5"],
    "landsIn": "exitCriteria:E2"
  },
  {
    "decision": "D3",
    "summary": "Cross-slice delay to another package.",
    "sourceRefs": ["changes/dev/s1-ai-summary-projection-polish/PROPOSAL.md#§4.1"],
    "landsIn": "package:s1-ai-summary-polish"
  },
  {
    "decision": "D4",
    "summary": "A dropped decision.",
    "sourceRefs": ["changes/dev/s1-ai-summary-projection-polish/PROPOSAL.md#§6"],
    "landsIn": "dropped",
    "reason": "risk-accepted: human accepted on 2026-07-19 that X is out of scope for this slice."
  }
]
```

`landsIn` grammar (only these four forms are valid):

- `acceptance:<shortId>` — the decision is carried by an acceptance test; `<shortId>` must match an `id` declared in `acceptance[]`（对象形态条目 `{ "id": "X", "kind": "structural" }` / `{ "id": "X", "suiteGate": true }` 同样声明其 id——taskpackage-required-check 提取对象条目 id 进 acceptanceIds，landsIn 不误报 not-found）。
- `exitCriteria:<id>` — the decision is carried by a process/evidence item; `<id>` must match an `id` declared in `exitCriteria[]`.
- `package:<changeId>` — the decision is deferred to another TaskPackage; `<changeId>` is the target package directory name (shape `[a-z0-9-]+`). The cross-package union is checked at the workstream level by `portfolio-planner`.
- `dropped` — the decision is intentionally dropped; must include a `reason` of at least 12 characters. For "notify"-class drops the reason should start with `risk-accepted:` and include the human acceptance context/date.

Rules:

- For `sourceType=proposal`, `decisionCoverage` is mandatory and non-empty — **包括 Lite TaskPackage**（gov-review-architect-change-triage F2：「Lite 省 decisionCoverage」仅适用于非 proposal 的 bug 类小改动；proposal 来源含 Lite 必填不豁免）。
- Every `sourceRefs[]` entry that is a proposal `.md` path must also appear in `docRefs[]` (fragment-stripped for G3 validation).
- `dropped` entries must have a `reason` of at least 12 characters.
- For other `sourceType` values `decisionCoverage` is optional but recommended; if present, the same back-check and validation rules apply.

### 机检口径分层（两闸，gov-review-devloop-closeout-hardening §3.5 / Q6 / F-05）

decisionCoverage / 必填字段的机检是**两闸分层**，口径不同、不可互代：

- **`taskpackage-required-check.mjs` = 字段完整性 + decisionCoverage 承重闸**：深校验必填字段 + decisionCoverage（含 `landsIn` acceptance/exitCriteria 回查）。这是承重单源；closeout 相位程序化调用它（见 [dev-loop.md](dev-loop.md) 收口闸表，F-04）。
- **`devloop-artifact-check.mjs` = origin/changeId 形状闸**：只查 `origin.type` 值域 / `origin.ref` 合法性 / changeId 前缀形状，**不查 decisionCoverage / 必填字段**。其 `ok:true` **不作字段完整性证据**（Q6：只加分层说明、不加 decisionCoverage 检查，避免两闸逻辑重复维护漂移）。
- **键名收敛（ARQ-077，260731 Phase 2 T2 landing, 2026-08-01）**：三门统一读 `origin.upstreamType ∨ origin.type`（类型键）与 `origin.upstreamRef ∨ origin.ref`（路径谓词键解析）——`devloop-artifact-check.mjs` 原「只读 `origin.type`」本批收敛。原不对称消除；教训（逻辑重复维护漂移）留档。
- **F-05 键纪律（通用）**：本束任何**新增** proposal-条件逻辑判据键统一读 `origin.upstreamType ∨ origin.type`（与 `taskpackage-required-check.mjs` 的 `isProposalByType` / `closeout-check.mjs` 对齐），不再自产第三个键不对称。（`devloop-artifact-check.mjs` 只读 `type` 风格已于 Phase 2 T2 收敛；路径谓词键解析统一 `origin.upstreamRef ∨ origin.ref`。）

## 声明可兑现性（执行层能力声明须有兑现方）

TaskPackage 声明执行层能力需求时（`s2ExploratoryRequired` 类布尔声明字段，以及 prose 中声明的能力要求），建包方须在起包期把每个声明映射到当前仓内现存的兑现方（角色 + 工具 / 闸路径，如 `s2ExploratoryRequired` → integrator 经 `run_inline_check` 执行 S2 official scan，见 dev-loop.md §3 / integrator.md Step 5），逐一列明；映射不上的声明不得裸交——要么不声明，要么在包内显式写明执行期降级预案（降级路径 + 「降级不亏证据」条款）。降级本身合法，「声明一个兑现不了的要求且无预案」不合法。（先例：loop-retro-2026-07-28 R8——260727-chat 声明 `s2ExploratoryRequired=true` 而当时 integrator 工具集无 S2 explorer，执行期才撞缺口、靠预案降级真 DB 证据。）本条为起包 convention、不入机械门（声明字段面小且开放新增；声明漂移复发时升机检，见 ARQ-260728-taskpackage-declaration-feasibility 留根）。

## Arch Review Handoff

Architecture review does not hand work to dev-loop by prose alone. For each `CONFIRMED` finding that requires implementation-side remediation, the review closeout must produce `dev-loop-handoff.json` under the review folder.

Recommended shape:

```json
{
  "schemaVersion": 1,
  "sourceType": "arch-review",
  "reviewId": "arch-review-S1-leads",
  "sourceRefs": ["F3"],
  "findings": [
    {
      "findingId": "F3",
      "sourceRefs": ["F3", "ARQ-024"],
      "severity": "BLOCKING",
      "riskTier": "high",
      "gateProfile": ["types", "real-db", "test-quality"],
      "affectedSurfaces": ["firmware/main", "docs/contracts/interface-contract.md"],
      "blockingDecisions": [],
      "suggestedTaskPackages": [
        {
          "changeIdHint": "s1-k7-emit-declaration",
          "title": "Close S5 to K7 emit declaration",
          "sourceRefs": ["F3"],
          "planningImpact": "attach-existing",
          "deliveryImpact": "milestone-affecting"
        }
      ],
      "planningRecommendation": "attach-existing",
      "deliveryImpact": "milestone-affecting",
      "acceptanceRefs": ["F3"],
      "verificationHints": ["typecheck", "real DB acceptance for emitted notification"]
    }
  ]
}
```

Planner intake consumes this file, then decides whether to keep suggested packages, merge findings into fewer packages, split them further, or block on human/contract decisions.
