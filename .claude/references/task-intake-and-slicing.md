# Task Intake and Slicing

This reference is mandatory for planner intake. It decides where a piece of work came from, how large the executable slice should be, whether `delivery/workstreams/` is needed, and which gates must be enabled before builder work starts.

## Core Rule

Every formal implementation-facing change starts as a TaskPackage. `delivery/milestones/` is the milestone denominator, `delivery/workstreams/` is long-lived work memory, and `delivery/taskpackages/<change-id>/` is the executable contract for one dev-loop slice.

Do not use `delivery/milestones/` or `delivery/workstreams/` as a substitute for a TaskPackage.

Acceptance vs exitCriteria authoring convention (A10-type): `acceptance[]` holds only test-assertable items; process/evidence items belong in `exitCriteria`. Non-id-testable entries use explicit object markers — `{ "id": "X", "kind": "structural" }` (non-code-testable) or `{ "id": "X", "suiteGate": true }` (suite-level regression gate; positive list/negative rule apply, and the two markers are mutually exclusive). Acceptance-prose word-washing is removed and forbidden, and bare-suffix / interval-abbreviation mentions (e.g. `A1..A14`) no longer exempt — write full ids or explicit object markers. See `.claude/references/dev-loop.md` §3 (变更制品) for the convention and its red-verdict guidance.

## Required Intake Fields

Each TaskPackage must include:

```json
{
  "sourceType": "arch-review",
  "origin": {
    "upstreamType": "proposal",
    "upstreamRef": "delivery/workstreams/<ws>/PROPOSAL.md",
    "upstreamFrozen": "<version/sha>"
  },
  "sourceRefs": ["D-A1"],
  "deliveryImpact": "milestone-affecting",
  "planningImpact": "attach-existing",
  "planningRefs": ["delivery/workstreams/devpc-review-remediation/"],
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
  "upstreamRef": "delivery/workstreams/<ws>/PROPOSAL.md",
  "upstreamFrozen": "sha1:<16hex>",
  "workstream": "<ws-slug>"
}
```

- **`upstreamType`**（必填）：取值 = `sourceType` 9 枚举（`delivery-story`、`planning-workstream`、`arch-review`、`human-ad-hoc`、`ci-failure`、`drift-report`、`regression`、`maintenance`、`proposal`）**+ 一个受控扩展 `human-confirmed-plan`**。`human-confirmed-plan` 是过渡性——专为「人确认过、非正式 proposal 的设计权威」（如 s1-nsa design-plan.md）设，其 `origin.type` 不映射到任何 `sourceType`（两者并存→红）。
- **`upstreamRef`**（proposal 型必填，其余 type 可省略）：指向上游设计目录——proposal 型指向 FROZEN PROPOSAL 路径（如 `delivery/workstreams/<ws>/PROPOSAL.md` 或 `delivery/review/<id>/PROPOSAL.md`）。非 proposal 型（regression/ci-failure/…）指向触发来源（issue / drift 报告 / 人类指令记录）。**proposal→docRefs back-check（完整性闸）**——触发判据为**析取** `isProposalByType ∨ isProposalByPath`（260731 Phase 2 T1 路径枢轴：`isProposalByPath` = `origin.upstreamRef ∨ origin.ref` fragment-stripped 走 G3 `isProposalMdPath`；`isProposalByType` = `sourceType/originType === 'proposal'` 字符串判，析取保老包）时 `origin.upstreamRef`（fragment-stripped）**必须** ∈ `docRefs[]`（新包 / 路径枢轴包——origin.ref 接管 back-check；老包（sourceType=proposal、originType 非 proposal、ref 非 proposal 路径）维持 sourceRefs-based back-check 不误红，过渡 carve-out）。**decisionCoverage 承重触发维持字符串判**（`isProposalByType`），不随路径枢轴（OQ4-B：hybrid 不被要求 in-taskpackage DC，其 DC 在 workstream 级）。
- **`upstreamFrozen`**（**仅 origin.upstreamType=proposal 必填**、占位符无效；其余 type 可空、不触发 UPSTREAM_DRIFT）：钉上游 PROPOSAL 被批时间点，用于 `closeout-check` 检测**上游悄改漂移**（UPSTREAM_DRIFT = **warn/告警，非红拦**——convention-only，机器齿记 ARQ-260723-artifact-lifecycle-teeth）。**标准 pin 格式（260731-gov-review-artifact-lifecycle-origin-teeth Phase 1 FROZEN；Phase 2 同批改写 + carry 再确认，H4 批准）**：`sha1:<16hex>`，算法 = `'sha1:' + createHash('sha1').update(fileContent.replace(/\r\n/g, '\n')).digest('hex').slice(0,16)`（**raw file bytes 行尾归一化（`\r\n` → `\n`）后 hash**，G3 `computeFrozenPin` 共享单源，与 `closeout-check.mjs` 消费端一致）。**不是** `git hash-object`（后者含 git blob header `"blob <size>\0" + content`，对同一文件产出不同 hash，结构性不可比）。**过渡期**：prose pin（如 `"frozen v0.2 2026-07-29 (...)"`）及 `git hash-object` 产物均为非标准格式 → convention-only-skip（不判 drift），不追溯为红；新包冻结时应使用标准 raw-content-sha1 格式。
- **`workstream`**（可选）：归属的 `delivery/workstreams` workstream slug，供 `portfolio-planner` 拆包 + 反向反查消费方。

**小 bug 包 origin 填法**：triage 判②单包小 bug → `origin.upstreamType = regression | ci-failure | drift-report`（按触发源，**非 proposal**），`origin.upstreamRef` 指向触发来源（issue / drift 报告 / 人类指令记录），**无需 FROZEN PROPOSAL**——小 bug 上游不是 proposal。`origin` 仍是一等强键，只是 type 非 proposal。

**milestone 来源包 origin 填法（260924-gov-review-planner-orchestration-align T5，三包收敛先例成文）**：portfolio-planner 拆包以 milestone 为上游来源（`delivery/milestones/**`）时，`origin.type`/`origin.upstreamType` **必须取闸值域内枚举值**——`milestone` 不在 `ORIGIN_TYPES` 闭集（= sourceType 9 枚举 + `human-confirmed-plan`），写出必红（m2-k1 r1 / m2-k3 intake 裸跑 / m3-k1 r1 三包同雷各修一遍的先例）。收敛填法 = `upstreamType` 取 `delivery-story`，milestone 溯源走 `upstreamRef`（指 `delivery/milestones/milestones.jsonl#<milestone-id>`），`workstream` 照常：

```json
"origin": {
  "type": "delivery-story",
  "upstreamType": "delivery-story",
  "upstreamRef": "delivery/milestones/milestones.jsonl#M3-schedule",
  "upstreamFrozen": "",
  "workstream": "m3-schedule"
}
```

**hybrid 包 origin 推荐填法（accepted-risk convention，260731-gov-review-artifact-lifecycle-origin-teeth Phase 1 FROZEN）**：多包 regression 经 FROZEN PROPOSAL 拆包（如 260729-s1-chat-interaction-fixes 实例）= **hybrid provenance**（触发源=regression，拆包上游=FROZEN PROPOSAL）。推荐填法：

```jsonc
"sourceType": "regression",
"origin": {
  "type": "regression",            // 与 upstreamType 镜像同值（桥接 ARQ-077 键不对称）
  "upstreamType": "regression",    // 触发源，非 proposal
  "upstreamRef": "delivery/workstreams/<ws>/PROPOSAL.md#§x.y",  // 仍指向 FROZEN PROPOSAL（fragment 可加）
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
| `planning-workstream` | The slice is a child of an existing `delivery/workstreams/<workstream>`. |
| `arch-review` | The source is an architecture review finding, ARQ/CDQ row, contract gate condition, or review packet. |
| `human-ad-hoc` | A human gives a one-off task that is not already in delivery or planning. |
| `ci-failure` | A PR/CI/verifier gate failure triggers the task. |
| `drift-report` | A scheduled or manual drift report triggers the task. |
| `regression` | A behavioral regression from manual QA, acceptance, or user feedback triggers the task. |
| `proposal` | 来源为 `delivery/review/**/PROPOSAL.md` 或 `delivery/workstreams/<ws>/PROPOSAL.md`（大 feature 设计权威，gov-review-architect-change-triage §3.6）类设计方案；载荷 = 决策集；蒸馏完整性规则强制适用（**含 Lite，见 decisionCoverage 规则**）。`human-confirmed-plan` **不是** sourceType 枚举值（其正名归 artifact-lifecycle 的 origin.type，本表不预支）。 |

`sourceType` records origin, not impact. If an arch-review task changes milestone health, keep `sourceType: "arch-review"` and set `deliveryImpact: "milestone-affecting"`.

### sourceRefs

`sourceRefs` is an array of stable, grep-able refs from the source system:

- Delivery/spec: `S-F1.4`（story 级）、`S-F1.4-AC3`（AC 级复合句法，见「story 锚点引用与转写」节）、`docs/requirements.md:S-F1.4`
- Architecture review: `ARQ-063`, `D-A1`, `F3`, `ARQ-063-C1`
- Human ad-hoc: `TEMP-YYYY-MM-DD-short-slug`
- CI/drift: `CI-types-2026-07-05`, `DRIFT-D6-CREATE-TABLE-IN-TEST`
- Proposal: `delivery/review/<id>/PROPOSAL.md#§x.y`，或大 feature `delivery/workstreams/<ws>/PROPOSAL.md#§x.y`

For `sourceType=proposal`, `sourceRefs[]` must include the proposal `.md` path(s), and each such path must also be present in `docRefs[]`. A "proposal .md path" is a path ending in `PROPOSAL.md` under `delivery/review/`, `delivery/taskpackages/`, **or `delivery/workstreams/<ws>/`**（大 feature 设计权威；gov-review-architect-change-triage §3.6 把合法落点扩到 `delivery/workstreams`，G3 `isProposalMdPath` 已同步扩认）. Chapter-level refs may use URL fragments (e.g. `path#§x.y`) for human readability; G3 validation strips the fragment before checking `docRefs[]` membership.

> **DP-1 过渡 carve-out**：上述规则仅适用于既有 sourceType=proposal 老包（无 origin 或其 type 非 proposal）。新包（`origin.upstreamType=proposal`）的 proposal→docRefs back-check 由 **`origin.upstreamRef` 接管**——`origin.upstreamRef`（fragment-stripped）**必须** ∈ `docRefs[]`，`sourceRefs` 不再承载 proposal 路径（退本职为代码范围锚点）。详见 [task-intake-and-slicing.md](#origin) `### origin` 节。

### deliveryImpact

Use one of:

| deliveryImpact | Meaning |
|---|---|
| `none` | Does not change milestone state or committed scope. |
| `milestone-affecting` | May block, unblock, or advance a `delivery/milestones` milestone. |

Changing `delivery/milestones/milestones.jsonl` is still an integrator/human closeout act. `deliveryImpact` only tells the loop to inspect milestone health.

### planningImpact

Use one of:

| planningImpact | Meaning |
|---|---|
| `none` | Single slice; do not create `delivery/workstreams/`. |
| `attach-existing` | Append progress to an existing `delivery/workstreams/<workstream>`. `planningRefs[]` is required. |
| `create-workstream` | A new workstream is needed. Creating the top-level workstream is `portfolio-planner`'s authority (the cross-Pod RTE), not planner; a single-Pod planner only uses `attach-existing`. `planningRefs[]` is required. |
| `needs-triage` | Planner lacks enough information. Stop, ask the human or do read-only discovery. Do not send to builder. |

Create `delivery/workstreams/<workstream>` only when future agents need durable why/findings/progress across multiple slices. If the diff plus TaskPackage is enough, do not create a workstream. Creating a top-level workstream (and decomposing work into multiple TaskPackages) is `portfolio-planner`'s authority; a single-Pod planner only attaches to an existing workstream. If multi-Pod decomposition is needed, route to `portfolio-planner`.

> **workstream 创建权的两半（gov-review-architect-change-triage §3.4 / F4 / Q4，human-confirmed 2026-07-22）**：上面"create-workstream = portfolio-planner 权限"指的是**拆包+调度态建立**（把 workstream 拆成 N 个 TaskPackage、建 `task_plan`/`pod-board`/`progress`）。另有**设计权威落家**一半：`/triage-change` 的人类/主线程可在 triage 时创建 `delivery/workstreams/<ws>/` **仅用于装 reviewed FROZEN PROPOSAL（+ review.md）**，供 portfolio-planner 后续拆包消费。portfolio-planner 的拆包+调度权**不垄断建目录**。这是对 ARQ-065/066 RTE 集权的**有意放宽（deliberate relaxation，非回归）**，与 `architect-operating-model.md` §4「human or planner main thread creates `delivery/workstreams/<workstream>`」对齐。

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

### 尺寸维与视觉对齐判例（260928-gov-review-portfolio-boundary-story-propagation T6）

垂直切片维管**包边界的形状**，本维管**大小**——ui-v5-align 21 包建毁实证的缺口面（两轮粒度实质等于派遣方喂入的批次提案、human 三轮复裁充当缺失的粒度门）。四判据：

- **改动行数量级**：单包改动行数量级应在一个 Pod 的验证预算内可消化；量级失衡（一包覆盖其余包总和）= 切法失衡信号，重切。
- **文件面**：包间文件所有权不相交——同文件被多包认领 = merge order 冲突面 + 冻结面互踩，须显式 blocking edges 或重切。
- **单包验证预算**：一包的验收面（齿数 + live/真机半）一 Pod 在 fixBudget 内可消化；超出 = 拆细，或降验收密度升 human。
- **N 上下界**：单 workstream 包数无机械硬限，但 N 超过 Pod 并行度（WIP limit 量级）须显式论证——只按派遣方批次提案定粒度而无独立论证 = 粒度门缺失。

**视觉对齐类切法判例**（260924 ui-v5-align human 终裁判据成文，引据源 = `delivery/workstreams/ui-v5-align/` 终裁记录；视觉批不套功能切片细粒度逻辑）：

- **页单位粗包**：视觉对齐批按页切粗包，不按功能点/AC 逐条细切（粒度错配）。
- **包间文件面不相交**：页 → 资产/渲染文件映射保持单包独占；同文件跨页复用时认领关系显式登记。
- **跨页串面项走合并后串行追加轮**：横跨多页的系统性调整（字体/间距/主题级）不塞进任何单页包，等页包合并后单开追加轮，防每包都碰共享面。

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

Create or attach `delivery/workstreams/<workstream>` when any are true:

- multiple TaskPackages will be needed;
- findings form a remediation campaign;
- work spans multiple systems or multiple sessions;
- decisions/progress must be durable for future agents;
- milestone state will be blocked/unblocked over time.

Stop with `planningImpact: "needs-triage"` when the planner cannot determine the slice, owner, risk, or required gate profile.

## rigor 字段与 Lite TaskPackage 字段级定义（gov-review-architect-change-triage §3.5）

`dev-loop.md` §0 声明 progressive rigor，本节给出字段级定义。TaskPackage 用 `rigor` 字段标档：`"rigor": "nano" | "lite" | "full"`（缺省按 full 既有逻辑走；存量无 `rigor` 的包不追溯，校验白名单放行）。

**Lite 省的是"方案/决策映射的文牍"，不是 loop 的任何一道闸。** 验收先行、红基线、verifier 证据、closeout、supervisor 合并门——一道都不跳。`rigor=lite` 另承载**快路径**执行语义（launch 面环境后置 + verify 面 firmware-build scoped 相位后置，final 恒跑）——资格判注与例外声明见 [change-triage.md](change-triage.md) §7「lite 快路径」，launch 面机制见 `kidpal-pod-launch-prep` SKILL 的 lite 分支；本节字段级定义本体不受影响。

- **不可省**（守 dev-loop 铁律）：`changeId` / `rigor` / `sourceType` / `sourceRefs` / `riskTier` / `gateProfile` / `story` / `planRef` / `acceptance`（含红基线）/ `exitCriteria` / `crossEnd` 骨架（T5，260908-gov-review-input-surface-frontload：动机包本身即 Lite，Lite 豁免这条就等于放过动机案例）/ `scope` / 合并门。
- **可省/简化**：**仅 bug 类（非 proposal 来源）**的 `decisionCoverage`（**proposal 来源含 Lite 仍必填不豁免，见下**）、design-review/security 重闸（按 riskTier 关）、多包依赖图（`mergeOrder`/`dependsOn`/`acceptanceIdMap`）、`fixBudget` 调小。
- **`crossEnd` 字段级定义（T5，260908-gov-review-input-surface-frontload）**：形状 = 对象 `{ "counterparts": [ { "path": "...", "symbol": "..." } ], "exempt": "<可选，不适用理由>" }`。**Draft 态：字段必须在场，空骨架 `{"counterparts": []}` 也算在场**（同 mutationEvidence 条）。**finalize 态（`acceptanceTestsFile` 非空）：`counterparts` 非空（每条含 path+symbol）∨ `exempt` 非空理由，二选一，双缺 = 闸红点名**；用作不适用分支时 **`exempt` 长度地板 ≥12 字符**（对齐闸内 `decisionCoverage.landsIn=dropped` 的 reason 地板，防「—」混关）——起草侧就要看到地板，否则短 exempt 撞闸红再反推，正是本清单要消灭的形态。**语义**：跨端性是设计判断、不是可推导属性（实测反例 `260907-g-relay-fix` 标 `subsystem=server`、全字段零 firmware 路径，却在改设备侧要解的帧），故走申报 + 留痕而非硬谓词；**判断不了按存疑向严填 counterparts，不填 exempt**。**本批覆盖档 = 自有实现面消费方**（`firmware/**`、`server/**`，`{path,symbol}` 可被 `brief-inputs.mjs` grep 解析；解析不到 = 脚本 fail-loud 非零退出、不静默产空锚）；只读参考树与无源码黑盒消费方（OpenClaw 引擎、物理声/显）两档 **留 ARQ 未覆盖**，越界填法脚本给 advisory。**advisory（非门）**：包 refs 引契约 wire 节（`interface-contract.md` 的 §1.5/§三/§四，常量单源在 `brief-inputs.mjs`）而 `counterparts` 空且无 exempt → stderr WARN 提示 planner 复核。消费方 = `brief-inputs.mjs` 第②路展开成对端面必读清单 + `taskpackage-required-check.mjs` 判定。
- **`planRef` 指向 sibling `plan.md`**：taskpackage.json 是机器解析的，reviewed plan 是长 prose，prose 归 markdown（与验收是 sibling `.acceptance.spec.ts`、由 `acceptanceTestsFile` 引用同模式）。plan.md 是 `/triage-change` 分流门 architect 审过的，planner **只读消费、改需留痕**。**③拆包路径下 plan.md = 全量义务**（「装配型豁免」已全取消），骨架与门 1 checklist 见下文「拆包链」节。
- **`crossPackageBehaviors` 字段级定义（260920-gov-review-bearing-strength-align T1，FROZEN v1.0）**：形状 = 数组，每项 `{ "topic": "...", "owner": "...", "note": "...", "carriedBy": "..." }`，四项语义——**`topic`** = 行为名（**包面存量键名为 `behavior`**）；**`owner`** = **语义 owner**（谁定这个行为的语义 / 裁决）；**`carriedBy`** = **实际承接包 `changeId`**（缺省回落 = `owner`）；**`note`** = 移交判据与验收归属。**「承接包」的判定统一为 `carriedBy ?? owner`**——「谁承接」这个位在两写面用的是不同的键（workstream 面 = `carriedBy`、包面 = `owner`），读侧**不得按 `owner` 单键判承接**（否则在 workstream 面把「谁定语义」当成「谁承接」，实测反例见下）。**写侧归一 canonical 四键；读侧兼容 `behavior` ∨ `topic` 作行为名**（包面存量 8 包全写 `behavior`，不回改）。
- **`crossPackageBehaviors` 的两写面（语义同源，登记位二选一）**：**包面** = 本包 `delivery/taskpackages/<changeId>/taskpackage.json`（拆包时登记）；**workstream 面** = `delivery/workstreams/<ws>/decision-coverage.json` 顶层键（拆包期登记）。**必填判据**：① 行为横跨 ≥2 包时必填；② **本包显式不做、须交下游承接的「殿后 / 转手」项必须登记在此**——不得只留代码注释或 prose（反例形态：`firmware/main/display/art_assets.c:10` 的「真机渲染节点集成殿后 9b」只活在代码注释里，承接方未出现在任何字段中）。**先例**：包面 = `delivery/taskpackages/260911-key-route-a1/taskpackage.json:100-105`（8 包在用）；workstream 面 = `delivery/workstreams/m1-home-qa/decision-coverage.json:116-141`，其中第 2 条（`:123-128`）`owner: 260915-m1-k3-voice-round-ui` / `carriedBy: 260915-m1-k4-reminder-overlay-unify` 即「语义 owner ≠ 承接包」的活语义。**消费方** = `brief-inputs.mjs` 第五路「跨包承接面」（扫两写面、按 `carriedBy ?? owner` 过滤、命中项派进 input-brief；该路是本报文落地时一并新增的读侧）。

**riskTier 优先于 rigor（单向守卫）—— convention-only（F1 / Q1-A）**：设计意图是单向校验——若信号说"这是 Full 级"（security 轴 / 迁移 / 跨包 / core.* / 触契约）而 `rigor:"lite"` → 应强制升 Full。**但该机器守卫当前不落地**（`taskpackage-required-check.mjs` 本批只扩 `isProposalMdPath`，不含 riskTier-overrides-rigor 校验，记 ARQ 后续 G3）；landing 时靠**两层人工**兜底：① `/triage-change` 人类触点①确认/改判路由；② supervisor 对 riskTier=high 仍升级人类。判断归 architect（写进裁决 rationale）。

**decisionCoverage 与 rigor 的关系（F2）**：`sourceType=proposal` 的包——**包括 Lite**——`decisionCoverage` 必填不豁免。「Lite 省 decisionCoverage」仅适用于非 proposal 的 bug 类小改动（regression/ci-failure/drift-report，无 proposal 决策集）。G3 `taskpackage-required-check.mjs` 对 `sourceType==='proposal'` 强制 decisionCoverage 必填，与本节一致。

### nano 档字段级定义（261016-gov-review-nano-tier FROZEN v1.1 立）

nano 面向「无验收语义净增、意图完全明确」的实现面微改（判据与排除清单见 [change-triage.md](change-triage.md)「档 0 nano 判据」节）。**Lite 省的是方案/决策映射的文牍；nano 再省「冻结验收链」——但验收权威不删除，置换成等价承载**：architect 事后白盒复核（行为判分位）+ 既有套件零新增红（防回归）+ human 视觉终裁（触 UI 包，不成文靠真机轮惯例）。

- **不可省**（守 dev-loop 铁律与簿记统一）：`changeId` / `rigor:"nano"` / `sourceType`（**不得为 proposal**）/ `sourceRefs` / `deliveryImpact` / `planningImpact` / `riskTier` / `gateProfile` / `story` / `planRef`（= 意图定义载体，nano 必填）/ `subsystem`·scope / `contractRefs` / `docRefs`（两字段在 taskpackage-required-check.mjs 数组非空必填行，nano 不豁免——无契约命中时按既有口径填契约面引用或显式说明）/ `changeManifest`（triage architect 给，fixer 文件级硬边界）/ `crossEnd`（二选一照填，exempt ≥12 字符地板不动）/ `acceptance` = **恰一条 `{ "id": "<包前缀>-A1", "suiteGate": true }`**（语义 = 既有测试套件零新增红；复用 260731 一等豁免机制，acceptance-coverage / mutation 两闸零改动即天然闭环）/ `exitCriteria`（含白盒复核条目）/ closeout 两 blocking 制品（docs-delta.md、review-packet.md）/ pod-retro.md（human 已裁不豁免）/ pod-events 全程簿记 / supervisor 合并门 + 前置升级闸全链；**总括：未在下方豁免清单显式列名的闸必填字段一律照 full 判据，不随 nano 豁免**。
- **豁免**（逐条理由）：`redBaselineCommand`/`redBaselineResult`（验收权威置换为白盒复核 + suiteGate，无新冻结件可跑红基线）；`acceptanceTestsFile`（同上，无新验收文件）；`mutationRequired`/`mutationEvidence`（suiteGate 条目 M-4 语义不适用，闸既有豁免行为承接）；`input-brief`（无 acceptance-author/builder 消费方，fixer 输入 = plan.md 意图定义）；`decisionCoverage`（nano 判据排除 proposal 来源，非 proposal 本就可选）；planner Step 1g / 7 / 8 验收件部分 / 8b / 9（编排面豁免理由见 planner.md nano 分支段）。
- **与 lite 快路径关系**：nano 与 lite 同享快路径两侧——verify 半边 = firmware-build scoped 相位后置（verify-gates.json defer 机制 whenRigor 已扩 nano）；launch 半边 = `kidpal-pod-launch-prep` SKILL 3b 触发条件已扩 nano（处置 = 扩 skill 对称处理，理由：nano 比 lite 更轻，轻档不享环境后置而重一档的反享 = 无理由倒挂）。lite 快路径其余语义不动。
- **与 lite 的边界判据（防两档死态互推）**：lite = 有行为决策要钉（新验收语义、story 命中或 bug 根因要冻结齿）但单包一轮可收；nano = 无验收语义净增、意图完全明确的形态替换。判不了 → 向严 lite。判档机判（半径量 + 排除清单）+ 越界升档 + loop-retro 校准三道防滥用。
- **豁免审计承接**：nano 包 acceptance 恒含 suiteGate → acceptance-coverage 输出 suiteGates 非空 → 既有 D-1 审计通道（review-packet 枚举 + human glance 一句确认，dev-loop.md 验收惯例节）承接，不新增机械通道。

## 拆包链：workstream 双轨制品、门 1、story 转写（260912-gov-review-portfolio-decomposition-chain FROZEN v1.0）

### 双轨制品与拆包权威

- workstream **PROPOSAL**（`delivery/workstreams/<ws>/PROPOSAL.md`，模板 [`WORKSTREAM-PROPOSAL.template.md`](../../docs/templates/WORKSTREAM-PROPOSAL.template.md)）= **裁决记录**（三问 + 裁定），冻结后不改；**roadmap**（`delivery/workstreams/<ws>/roadmap.md`，模板 [`WORKSTREAM-ROADMAP.template.md`](../../docs/templates/WORKSTREAM-ROADMAP.template.md)）= **活文档**（效果对账 + 列车图投影）。
- PROPOSAL §拆包预判 = **意图提示、非权威**；拆包权威 = 四件套（`roadmap.md` + `taskpackage.json` × N + `plan.md` × N + `decision-coverage.json`）经**门 1** 冻结 + human 过目切法。
- **职责划界（防第二事实源）**：`task_plan.md` frontmatter 维持 `planning-index.mjs` 机器索引源地位，`progress.md` 维持进度记录，roadmap **只投影不裁决**——包集与达成状态的权威恒 = board + taskpackage JSON。roadmap ② 节（列车图）由 `.claude/scripts/render-roadmap.mjs` 机械渲染、人工禁改。

### plan.md 全量义务

- 每个 TaskPackage 拆包时必备 `delivery/taskpackages/<changeId>/plan.md`，`planRef` 指向；**「装配型豁免」全取消**（OQ3=(a)，260913 human 裁——豁免判断权在拆包 agent 自身即利益冲突，PKG-1 误豁免实证在册）。
- plan.md 骨架（260928 批 T1 收窄）：**背景摘录**（锚点随 workType 分流——整改型锚 PROPOSAL「问题与根因」节、新建型锚「现状与差距」、重构型锚「结构性痛点」；三处节名**逐字一致**）+ 可行性判据 + **跨包事实与共享冻结面约束（含跨切片边角）** + 铁律约束 + demo 证据形态复述（与 roadmap 一致）。**判据一句钉死：跨包事实 / 共享冻结面约束（只有 RTE 看全局才知道的跨切片知识，如「唤醒词路径也过会话发起入口」「发送原语复用 K2 冻结缝」）留拆包侧；单包内实现决策（改法取舍、阶段划分、时序细节、渲染接线、包内边角）不进拆包侧 plan.md**——包内细节设计承载位 = 冻结 acceptance 断言化 + TaskPackage `notes`/`exitCriteria`/`decisionCoverage` 备注 + planner 派发 prompt（260928 批 OQ-1 裁定）；阶段划分 = `planner.md` Pod 标准 loop 重述、不重复成文。**例外（260928-gov-review-dev-loop-preassess-manifest-scope T3）**：plan.md 的「技术方案（planner+architect 预评）」节承载 architect 前置评估（planner Step 1g 档）产出——改法取舍 / 影响面 / 关键接线决策；该节由 planner 在收到 1g 档评估后补写，**须留痕（日期 + architect 档标识）**；包内其余细节承载位判定照旧。

### story 锚点引用与转写

- story 在 plan.md / taskpackage 中**只以锚点引用出现**：`S-Fx.y · <标题>`（可选带 AC 号集）。**不内嵌原文、不改写、不复述语义**；story 语义权威 = `docs/requirements.md` 现文。
- **acceptance 唯一写点（260928-gov-review-portfolio-boundary-story-propagation T3）**：story 命中包的 acceptance 由 Pod 内 acceptance-author 唯一产出（input-brief 带 `brief-inputs.mjs` 的 story 整节照录投影，见该脚本「story 语义权威」段）；拆包侧**不写 acceptance 骨架**（taskpackage `acceptance` 拆包期为空骨架 `[]`）；planner 在 **Step 8b** 核对忠实度（AC 回引号集与 sourceRefs AC 号集一致 + story 补充规则逐条有无落点 + 断言方向不弱于、不钉反），核对不通过退回 acceptance-author 重钉、planner 不代写；机械号集对账由 `taskpackage-required-check.mjs` AC 回引齿承载（T4，两道不互替）。
- taskpackage `story` 字段承**锚点引用**，技术描述归 plan.md（治 260829 期 story 字段写成技术方案的真实翻车——write-story 纪律 1 反例在册）。
- **AC 级回引复合句法（全仓首钉）**：`S-Fx.y-AC<n>`（如 `S-F1.4-AC3`）——验收齿 / `sourceRefs` 的 AC 粒度回引按此形态填。
- **句法分工注记**：prose 锚点引用形态的权威 = skill `write-story`（其 description「拆包引用 story 的合法形态以本 skill 为准」管 prose 层）；AC 粒度机器引用的复合 token 句法 = **本节首钉**（write-story 边界节已把「TaskPackage 怎么承载/转写 story」委派拆包链治理）。两句并存、各管一层，不互为违规依据。
- **换新边界**：`acceptance[]` 保持**本地 id 形态**（如 `KR-A1`、`SU-A3`），`S-Fx.y-AC<n>` 只出现在 `sourceRefs` / 验收齿回引位——防「acceptance id = story AC 号」混淆。
- **两道 intake 机查（fail-closed，执行方 = portfolio-planner Step 1）**：① 引用的 story 编号不得命中 `docs/requirements.md`「已砍清单」节（按节名定位，位置随 spec 演进）；② 可行性 = 🔴 的 story 须先有立项处置记录（write-story 🔴 挂账纪律：Pod 无权自裁）。命中即打回主线程补齐，不猜、不代写。

### 门 1（拆包 review 门）checklist

四件套齐备后、commit-before-launch 前，一道 **architect 轻量 checklist review**（OQ1=(a)：轻量、非完整对抗）。

- **派发形态**：human / 主线程派 architect（只读审 + 返回裁决、零写入 canonical）。portfolio-planner frontmatter 无 `Agent()` **也不加**（authoring 纪律）——其 Step 6 前以 AskUserQuestion 请 human 发起门 1，随后**停等**，不绕行。
- **checklist 五项**：① 切片逻辑（按行为切还是按页面/问题切，图上见；偏离垂直切片须有显式理由）；② 依赖序与 WIP 合理性；③ decision-coverage 完备（含跨包行为 owner 显式登记）；④ 每包意图陈述与 PROPOSAL 冻结裁定一致；⑤ demo 证据形态可行（真机轮配额默认仅核心包）。
- **workType 分流**：整改型核根因↔改法 + 红基线在位；新建型核切片与覆盖；重构型核结构性质可验收。
- **裁决耐久记录**：verdict + checklist 逐项结论 + 派发/裁决时点 append 到 `delivery/workstreams/<ws>/review.md`「门 1」节——GREEN 不得只留在聊天。
- **存疑向严**：触契约面 / 跨系统 / 判官不绿 → 升所属面 loop 的 Track A 段（架构/契约面 = `/arch-change`；治理面 = `/gov-change`）。

### launchPreconditions（canonical 定义）

- **落点** = workstream `delivery/workstreams/<ws>/decision-coverage.json` **顶层键**（数组；先例形态 = 260911 批）。**条目形态 = 对象**（260921-gov-review-launch-pipeline T6 承载 `ARQ-260914`；旧裸 `string` 形态向后兼容、不再新写）：

  | 键 | 取值 | 写入义务 |
  |---|---|---|
  | `text` | 前置描述字符串，须点名承载包与前置对象；由 `需修订` 类 spec 裁定产生的条目须写明**修订落点**（story 语义 → `docs/requirements.md`、支撑语义 → `docs/architecture.md`） | 建包方 |
  | `scope` | `"workstream"` ∨ `"package:<changeId>"`（**复用同文件 `decisionCoverage` 条目的 `landsIn` 取值形态**，不新造约定） | **建包方**（portfolio-planner 落四件套时填——条目文本本就点名承载包，机械可判） |
  | `status` | `"unverified"` ∨ `"verified"` | **launch 执行者**（human 点火前核后回写） |
  | `evidence` | 凭据锚（`progress.md` 行 / human 裁决 ref）；`status === "verified"` 时应在场 | launch 执行者 |

- **读侧**（`portfolio-reconcile.mjs --check-launch`，260921-gov-review-launch-pipeline T6 落地）：脚本**读**该键——**按包过滤**（只取 `scope === "workstream"` ∨ `scope === "package:<本包>"`，其余折叠为一行计数、不逐条列）、**只报** `status !== "verified"` 的条目，落 **warn 通道（blocking 语义不变）**；裸 `string` 旧形态按 `{scope:"workstream", status:"unverified"}` 视之并走汇总行、不逐条报（防永久噪音，且不静默丢失）。**本改动不改任何门的判定语义、不新增工程门。**
- **核验 = 机械读 + 人读**（260921-gov-review-launch-pipeline T6 取代原「convention + 人读，本门不含机械齿」口径）：**人读义务不变**——launch 执行者（human 点火前）逐条核 `launchPreconditions` + 按落点核对应 spec 文件**已冻结**（review + human 冻结落地）；未冻结 → 承载该裁定的包不得 launch（粒度 = 包，非整 workstream 陪等）。**核验结果回写 `status` / `evidence`**，使 `--check-launch` 下次不再重复报同一条。
- **存量迁移不在同批**：两个在册 workstream（`m1-home-qa` / `m2-char-window`，各 6 条裸 `string`）的迁移由建包方在下次动该 workstream 时随批完成（`scope` 机械填、`status` 逐条引源行填）；过渡期由上述旧形态兼容行为承接。
- **spine 源（② 单包路径，260928-gov-review-cross-face-intake-spine 起）**：带 spine 的变更（`delivery/intake/<changeId>.md` 在场），spine `facePlan` 中**先于实现面**的各面 anchors 未回填 = 上游面未冻结 → 承载包不得 launch（与 ③ 路径「未冻结 → 承载该裁定的包不得 launch」同判、粒度同为包）；核验 = build-feature launch 面预检第五件（文面路由，非机械闸），机检齿留根。

### TaskPackage 显示编号（承重/游离，机械推导 · 2026-09-15 定，ARQ-260915-taskpackage-display-numbering）

**形态** `<WS>-<KN><波次><并行字母?>`，如 `M1-K2a`：

- `<WS>` = workstream 短码，建 workstream 时定（与里程碑一对一时 = 里程碑号，如 `M1`；一号多 workstream 加建序字母 `M1a`/`M1b`）。机械侧：`render-roadmap.mjs` 从 slug 里程碑前缀推导（`m1-home-qa` → `M1`），`--ws-code` 显式覆盖。
- `<KN>` = **K（承重）/ N（游离），恒由机器从 dependsOn 推导、人工不赋值**：K ⟺ 被任一 in-scope 下游包 dependsOn，或位于主干终点（依赖深度达本 workstream 最大）；N ⟺ 无人依赖且不在主干终点。跨 workstream 外部前置不参与 K/N 与波次推导（已并/跨流包不占本流波次）。
- 波次数 = in-scope 依赖深度 + 1；并行字母 = 同波次内按 mergeOrder 的占位序（同波互不依赖、可同批 launch，字母序 ≠ 合并序），单元素波次省略字母。

**判读保证**（推导性质，非人工约定）：① 任何包的 dependsOn 目标必为 K（承重闭合——被依赖者必承重）；② N 包无人等待：延后/重排不阻塞任何包（全浮动侧线）；③ 编号不编码完整依赖边——波次给最早开工层级、K/N 给主干/侧线，确切边以 dependsOn 与列车图为准。

**机制**：编号是**渲染态、非 canonical 字段**——由 `render-roadmap.mjs` 从 dependsOn + mergeOrder 推导、随 roadmap ② 节列车图渲染（图例随区块输出），`taskpackage.json` 不存编号、人工不赋值。依赖变更后编号随重渲染自动重算，不构成拆包制品变更、不重过门 1。

**changeId 嵌编号 token（常设规则，2026-09-15 定）**：拆包建包时（portfolio-planner Step 4 落盘四件套），changeId 在日期前缀后、语义 slug 前嵌入编号 token——形态 `YYMMDD-<ws小写>-<kn小写><波次><字母?>-<语义slug>`（如 `260915-m1-k2-keybar-three-slot`；单元素波次无字母，如 `260915-m1-n1b-p1-home-battery`）。编号 token = **建包时点依赖图的镜像**：后续依赖变更时渲染态编号随重渲染重算，changeId **不追溯改名**（目录 / taskpackage / dependsOn / board / merge token 均以 changeId 为键，键名冻结）；token 与渲染态漂移时判读以渲染态为准，后续批次新增包用当次推导的编号嵌名。

## UIUX Depth Ladder and Optional TaskPackage Fields

When a slice touches user-visible UI, planner detects a UIUX depth level and records UIUX context on the TaskPackage. Depth drives whether the **pre-launch UI-ready check** applies (T2 · planner Step 6b) and what design-review evidence integrator owes at closeout.

| Level | Use when | Required action |
|---|---|---|
| L0 | Pure backend/contract/script/migration/internal service; no user-visible UI | `uiuxRequired=false` (or key absent); no UIUX author or design review |
| L1 | Existing blueprint + component pattern; small status/field/button/message change | UI-state acceptance only（小视觉微调——不产生新设计语义——亦在此档走 designReview） |
| L2 | Package touches UI and upstream frozen per-page PNGs **exist** for every affected page/state | Consume: planner writes per-page PNG refs into the TaskPackage `uiux` block `designRefs[]` at packaging time（no translation role dispatched — upstream supply = requirement-change 档 2 流程 4b 生成模式） |
| L3 | Package touches UI and upstream frozen per-page PNGs are **missing**（new page / new IA / new state without art） | **Design-gap signal**: fail-closed **before Pod launch** — stop the package, log pod-events `agent-escalation`（缺口页清单 + 路由去向），route to `requirement-change`（承接形态 = 原 story 的档 2 触 UI 修订批；human 裁意图入口照常、§9 版本行照常 append）；after the requirement batch freezes, planner re-runs the UI-ready check（对齐新版本行）then resumes the package |
| L4 | System navigation model, shell, shared UI, cross-system components, multiple pages | System blueprint first — **drawn** via requirement-change 档 2 流程 4b 生成模式（same supply chain；human 设计方向裁定恒在人）；a blueprint change that constitutes a governance change is **reviewed** via `/gov-change`（UIUX 轴集随治理变更携带，只审不画）; then `portfolio-planner` slices TaskPackages（蓝图先行与拆包是先后两动作、不互斥——planner 停包、不越权拆包） |

Existing systems without a full blueprint may continue L1 work. The **UI-ready launch preconditions** for any UI-touching package（frozen per-page PNGs present + brief version-row alignment，分形判据：需求批出生的包对齐本批 requirements 版本号，非需求批出生的包对齐版本不晚于当前 frozen requirements 最新版本行）are **authoritatively defined in requirement-change.md「消费保护规则」节 launchPreconditions 句** — this section points, it does not restate.

The UIUX fields are an **OPTIONAL extension**, not part of the required intake field set. `taskpackage-required-check.mjs` is unchanged and its hardcoded required list excludes them; a Level-0 package needs no `uiuxRequired` key at all. Add them only from Level 1+ (some: `uiAcceptanceRefs`) / Level 2+ (`designRefs` per-page PNGs):

```json
{
  "uiuxRequired": true,
  "uiuxLevel": 2,
  "designRefs": ["docs/ui/V5/canvas-p1.png"],
  "sharedUiRefs": ["docs/ui/design.md#component-action-card"],
  "localDesignArtifactRefs": ["delivery/review/uiux-<system>-blueprint/_artifacts/<snapshot>"],
  "artifactPackageRef": "<external zip path or future S5 artifactId>",
  "uiAcceptanceRefs": ["pageId-x.stateId-y"],
  "designReviewRequired": true,
  "designReviewEvidenceRequired": true,
  "repoVisualArtifactRequired": false
}
```

`designRefs[]` point to frozen per-page PNGs（400×300，布局坐标定位权威——builder layout 常量按图定，brief 文本节仅作口径补充）. If `UIUX-DESIGN-BRIEF.md` changes **semantically** while a UI TaskPackage is in flight, affected packages pause and update their design refs/acceptance; unaffected packages continue.

## Distillation Integrity (decisionCoverage)

`decisionCoverage` is an optional TaskPackage field that becomes **mandatory** for `sourceType=proposal`. It records how each decision/constraint from the source was carried into the loop: landed in an acceptance test, landed in an exit criterion, deferred to another package, or explicitly dropped with a reason. It is a **mapping layer**, not a replacement for the A10 acceptance-vs-exitCriteria convention: `acceptance[]` still holds only test-assertable items, and `exitCriteria[]` still holds process/evidence items. `decisionCoverage` proves the **completeness** of the acceptance/exitCriteria set against the source decision set; A10 proves the **assertability** of each item.

Shape:

```json
"decisionCoverage": [
  {
    "decision": "D1",
    "summary": "Polish only runs when substantive content changes (three-way check).",
    "sourceRefs": ["delivery/taskpackages/s1-ai-summary-projection-polish/PROPOSAL.md#§4.2"],
    "landsIn": "acceptance:POL-B2"
  },
  {
    "decision": "D2",
    "summary": "Unit tests cover debounce merge and failure fallback.",
    "sourceRefs": ["delivery/taskpackages/s1-ai-summary-projection-polish/PROPOSAL.md#§5"],
    "landsIn": "exitCriteria:E2"
  },
  {
    "decision": "D3",
    "summary": "Cross-slice delay to another package.",
    "sourceRefs": ["delivery/taskpackages/s1-ai-summary-projection-polish/PROPOSAL.md#§4.1"],
    "landsIn": "package:s1-ai-summary-polish"
  },
  {
    "decision": "D4",
    "summary": "A dropped decision.",
    "sourceRefs": ["delivery/taskpackages/s1-ai-summary-projection-polish/PROPOSAL.md#§6"],
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

TaskPackage 声明执行层能力需求时（布尔声明字段，以及 prose 中声明的能力要求），建包方须在起包期把每个声明映射到当前仓内现存的兑现方（角色 + 工具 / 闸路径），逐一列明；映射不上的声明不得裸交——要么不声明，要么在包内显式写明执行期降级预案（降级路径 + 「降级不亏证据」条款）。降级本身合法，「声明一个兑现不了的要求且无预案」不合法。（先例：loop-retro-2026-07-28 R8——曾有声明的执行层能力无对应工具，执行期才撞缺口、靠预案降级真 DB 证据；相关声明字段已随 260929 对齐批退役，先例留作纪律出处。）本条为起包 convention、不入机械门（声明字段面小且开放新增；声明漂移复发时升机检，见 ARQ-260728-taskpackage-declaration-feasibility 留根）。

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
