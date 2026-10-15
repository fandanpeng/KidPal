# architect-landing-summary · 261015-gov-review-face-loop-unify

packet-id：FROZEN PROPOSAL §2 改法集（T1–T24）+ §5 验证集（V1–V11） · 评审轮次 R1 BLOCK(F1..F7) → R2 BLOCK(F1..F3) → R3 GREEN · change-id 261015-gov-review-face-loop-unify

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 architect-reviewer 扩编 | `.claude/agents/architect-reviewer.md` | 全文重写：面 loop 审稿四查 + 契约分类过滤（canonical 五值引用）+ 判档只升不降 + Track B 评审纪律 |
| T2 窄写槽 | `.claude/scope-map.json` | architect-reviewer allow = `delivery/changes/arch/*-{arch,gov}-review-*/**`（landing-verifier 同形）；`_doc` 注记授槽事由 |
| T3 AGENTS.md 治理路由节 | `AGENTS.md` | 七步 cast 更新（architect 起草 / architect-reviewer 评审循环）+ 升级判据重写 + 四面入口行 + 「不动」契约防线句 |
| T4 CLAUDE.md 契约行 | `CLAUDE.md` | 替换为 `/arch-change` loop + 机械凭据闸表述 |
| T5 architect.md 职责归位 | `.claude/agents/architect.md` | description + When to Use + 起草/判官职责 + Boundaries + G4 共七处 |
| T6 operating-model 同步 | `.claude/references/architect-operating-model.md` | boundary summary / G4 / §3 路由表 / 两变更前置门 / Discussion→Draft（重写）/ §5 / review-depth→判档，共九处 |
| T7 /arch-change repurposing | `.claude/commands/arch-change.md` | 全文重写为架构+契约面 loop 入口（七步流水） |
| T8 架构面 SSOT | `.claude/references/arch-change.md` | 新建：立项收敛度 / Track A 段判据 / 四查 / 轻路径判据 / 落地通道 / 触契约分支 / 定稿载体 / 角色表 |
| T9 删 /review-arch | `.claude/commands/review-arch.md` | `git rm` 删除 |
| T10 architecture.md §12 | `docs/architecture.md` | 新增「变更记录」节（纪律行 + 空表首；编号追加不重排） |
| T11 /gov-change 命令 | `.claude/commands/gov-change.md` | 新建（ARQ 消费 + 路径即判据判档） |
| T12 治理面 SSOT | `.claude/references/gov-change.md` | 新建（引用 arch-change 落地通道节，不重复定义） |
| T13 删 /review-governance | `.claude/commands/review-governance.md` | `git rm` 删除 |
| T14 contracts 规则重写 | `.claude/rules/contracts.md` | canonical 五值枚举唯一权威 + 写防线 + 机械凭据闸 + 人签门制品退役注记 |
| T15 合并闸凭据改写 | `.claude/scripts/pre-merge-escalation-gate.mjs` | `hasArchLoopContractEvidence` 三件联立实查（lib 单源解析）+ 通用 1 改写 + self-check 六例 + 函数 fixture 四例 |
| T16 review-loop settings | `.claude/review-loop.settings.json` | 删契约 deny（permissions 空）；`_doc` 更新（防线去向 + dev-loop deny 保留注记） |
| T17 证据骨架收窄 | `.claude/references/contract-change-evidence.md` | 契约包三节并入 PROPOSAL + 状态词修订 + 独立双制品退役 |
| T18 CDQ 列义 | `docs/CONTRACT-DECISION-QUEUE.md` | 头部状态词 + 「human 门」→「loop 凭据」；历史行未动 |
| T19 §10e 退役 | `.claude/references/pod-coordination.md` | 人合 runbook 四步 → 机械凭据闸指针节 |
| T20 引用普查改指 | 见下方清单（19 件） | 逐件按新语义改指；三条件锚亲读结案不改；dev-loop.md 差异注记见 §3 |
| T21 DEV-GOVERNANCE 重写 | `docs/DEV-GOVERNANCE.md` | §4 契约门节 / 对抗 review 节 / 治理变更节（mermaid + 步骤）/ 路由全景 / §5 门表两行 + 折进注 / §7 入口速查 + 入口总览 |
| T22 routing.json | `.claude/route-fixtures/routing.json` | commandEntry 三行全删；review-moderator / architect-reviewer 信号重钉 |
| T23 ARQ-260928 结项 | `docs/debt/gov/ARQ-260928-arch-change-retirement.md` + `docs/debt/gov/INDEX.md` + `docs/debt/gov/_archive/2026/`（debt-render 自动归位） | 第三路径 repurposing 结项注记；frontmatter status=closed |
| T24 repo-structure | `.claude/rules/repo-structure.md` | contracts 行 + 工作产物去哪节 |

T20 明细（19 件改指）：`.claude/agents/{review-moderator,scenario-author,planner,integrator,portfolio-planner,uiux-author}.md`、`.claude/commands/{triage-change,build-feature,loop-retro}.md`、`.claude/references/{arch-review-protocol,change-intake,change-triage,task-intake-and-slicing,dev-loop-operating-system}.md`、`.claude/skills/{brainstorming,gov-arq}/SKILL.md`、`.claude/rules/authoring-agents-skills.md`、`docs/templates/{WORKSTREAM-PROPOSAL.template,GOVERNANCE-MIGRATION-GUIDE}.md`、`docs/glossary.md`。**条件锚三件亲读结案不改**（全为历史 changeId 字面量）：`.claude/scripts/changes-index.mjs`、`.claude/hooks/enforce-scope.mjs`、`.claude/hooks/agent-model-guard.mjs`。

## 2. 验证证据

- **V1 路由回归**：基线 53 ok 0 fail（2026-10-15 FROZEN 前）→ 落地后 `route-regression-check` **52 ok 0 fail**（差 1 = commandEntry 三行全删所致断言数变化，符合 T22 钉死形态）；`--self-check` **104 ok 0 fail**（全部必留信号可证伪）。
- **V2 spec-anchor**：`spec-anchor-check.mjs` → `"errors": [], "ok": true`（§12 计入 archSectionCount，零悬空）。
- **V3 lint-descriptions**：`scanned 16, flagged 0`（新增/改写 description 无流程叙述气味）。
- **V4 JSON 形态**：scope-map / routing / review-loop.settings 三件 `JSON.parse` ok。
- **V5 合并闸自检**：`--self-check` **42 pass 0 fail**（含六个契约凭据用例 + 四个函数 fixture 例；既有 33 例不回退）。
- **V6 引用普查**：`grep -rn "/review-arch\|/review-governance"`（含 showme/archive 排除）→ 仅剩 2 处合法残留：`arch-review-protocol.md` 内 Track A 段历史定义句一处（已修，复扫归零）+ debt `_archive` 结项注记历史引用（封存合规）；`changes-index.mjs` 2 处 = changeId 字面量（封存）。
- **V7 debt 归位**：`debt-render.mjs` 跑毕，ARQ-260928 归 `_archive/2026/`，INDEX 已结指针更新。
- **V8 preflight**：FROZEN 前实跑，findings `[]` / advisories `[]`。
- **V9 三场景走查**：① 轻路径 = 审 → 瘦制品五件过闸 → 合并 → 变更记录节（SSOT `arch-change.md`「轻路径落地通道」逐步可追）；② 主路径+契约 = 契约包三节 + T15 三件联立凭据（gate 通用 1 新文案可追）；③ Track A 段 = 判据三条 → 对抗机器 → human 拍方向 → 流回（SSOT `arch-change.md` Track A 段节可追）。
- **V10 破坏性自检**：`git diff HEAD -- docs/contracts/` **空**（契约内容零触碰）；dev-loop.settings.json 契约 deny 在场未动；scope-map 除 architect-reviewer 条目 + `_doc` 一行外零改动；hooks 两件零改动；`.codebase-memory/` 两件为工具自刷新派生物，按其治理纪律不 stage 不追交。

## 3. 诚实注记

1. **差异注记 A1 · `dev-loop.md` 在册未改**：manifest 45 件中该件经亲读定性为「字面碰撞无实义命中」（唯一 `review-arch` 命中 = changeId 字面 `gov-review-architect-change-triage` 的子串；其「预审疑似契约变更，过滤实现侧误判」表述在新设计下仍准确——过滤器职责保留），零改动。留 manifest 内作 T20 普查覆盖凭证。
2. **差异注记 A2 · ARQ-260928 文件路径迁移**：manifest 声明 `docs/debt/gov/ARQ-260928-arch-change-retirement.md`，实际落位 `_archive/2026/` 同名件——`debt-render.mjs` 按「位置 = 状态的纯函数」自动归位（repo-structure 明文「人不搬文件」），非人工迁移。
3. **ADVISORY · V9 为文审走查**：三场景走查系对 SSOT 文本的逐段可追性核查，非实跑 loop——首三个真实变更批（各面一单）落地时按走查路径实际执行，偏差回写 retro。
4. **未落项（FROZEN §6 留根 6 条）**：需求面形态差统一评估 / reviewer 大一统评估 / 变更记录节机械消费齿 / 凭据闸补强评估 / CDQ 存废评估 / commandEntry 形态对齐复核——全部按留根清单去向登记，不在本批。

```landing-manifest
.claude/agents/architect-reviewer.md
.claude/agents/architect.md
.claude/agents/review-moderator.md
.claude/agents/scenario-author.md
.claude/agents/planner.md
.claude/agents/integrator.md
.claude/agents/portfolio-planner.md
.claude/agents/uiux-author.md
.claude/scope-map.json
.claude/review-loop.settings.json
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/route-fixtures/routing.json
.claude/commands/arch-change.md
.claude/commands/review-arch.md
.claude/commands/gov-change.md
.claude/commands/review-governance.md
.claude/commands/triage-change.md
.claude/commands/build-feature.md
.claude/commands/loop-retro.md
.claude/references/arch-change.md
.claude/references/gov-change.md
.claude/references/architect-operating-model.md
.claude/references/arch-review-protocol.md
.claude/references/contract-change-evidence.md
.claude/references/pod-coordination.md
.claude/references/change-intake.md
.claude/references/change-triage.md
.claude/references/task-intake-and-slicing.md
.claude/references/dev-loop.md
.claude/rules/contracts.md
.claude/rules/repo-structure.md
.claude/rules/authoring-agents-skills.md
.claude/skills/brainstorming/SKILL.md
.claude/skills/gov-arq/SKILL.md
AGENTS.md
CLAUDE.md
docs/DEV-GOVERNANCE.md
docs/architecture.md
docs/CONTRACT-DECISION-QUEUE.md
docs/templates/WORKSTREAM-PROPOSAL.template.md
docs/templates/GOVERNANCE-MIGRATION-GUIDE.md
docs/glossary.md
docs/debt/gov/ARQ-260928-arch-change-retirement.md
docs/debt/gov/INDEX.md
```
