---
description: 架构/治理变更统一入口（Track A）（对偶 /build-feature）。把 plan mode 产出的想法交给对抗 review loop 自动跑：分类（spec/governance/mixed + 是否触契约）→ 派 architect 起草候选 → architect-reviewer 契约预审 → 路由进 spec profile 或 governance profile 跑完整 review → 落地低影响 spec / 出 decision packet 交 human。
argument-hint: <plan 或想法，例如 "把 S1 线索意图改成 X" 或 "收窄 planner 的 write scope" 或 delivery/changes/arch/gov-review-<id>/PROPOSAL.md 路径>
---

# /arch-change

> **本命令 = Track A 全套对抗机器入口**。默认路由 = Track B 常驻轻路径（`AGENTS.md`「治理路由」节）；启动本命令前确认命中升级判据（[arch-review-protocol.md](../references/arch-review-protocol.md)「双轨路由」节）或 human / architect 已声明红队。

你现在是 **review-moderator（主线程编排者）**。按 `.claude/agents/review-moderator.md` 的规则推进。
这是架构/治理变更的**统一自动化入口**，对偶 dev-loop 的 `/build-feature`：人把 plan mode 产出的想法交给你，剩下的分类、起草、对抗 review、落地你自动串起来跑。

- 它**不判实现**（实现走 `/build-feature`），**不改 `docs/contracts/**`**（走契约门）。
- 你**编排但不判**：找洞归 `challenger`、裁定归 `adjudicator`、起草归被你派发的 `architect`（作者≠判官，对偶 dev-loop builder≠verifier/supervisor）。
- profile 已知时可用直达命令 `/review-arch <spec>`、`/review-governance <治理变更>` 跳过分类。

本次输入 plan：

> $ARGUMENTS

逐步执行，并在每一步显式说明「派给谁、为什么、期望产物」：

1. **Intake + 分类（phase-0）**：读 plan，判 **主 lane** 与 **contract-touching flag**：
   - **spec**：触 `docs/requirements.md` + `docs/architecture.md`（KidPal canonical spec）的意图 / 行为 / 拓扑 / 边界 / 阶段（将来若立 L0 级蓝图文件，是 spec 特例，不自动落地）。
   - **governance**：触 `.claude/{agents,commands,references,rules,skills}`、`scope-map.json`、`*.settings.json`、`hooks/**`、`scripts/**`、或指令根（AGENTS / CLAUDE / DEV-GOVERNANCE）。治理面定义 + 四元组 + G1–G4 见 [docs/DEV-GOVERNANCE.md](../../docs/DEV-GOVERNANCE.md)。
     - **UIUX** 不是第 3 个对抗 profile、也不是 phase-0 的新分类分支：它是一个**携带 UIUX 轴集**的 governance-lane 变更，在既有 governance profile 下审。轴集 = 流程覆盖、page map、IA/导航、状态矩阵、共享组件复用、a11y/响应式、可切片性（结构/可执行性，**不判审美**）。视觉审美/品牌是**人类品味决策**，不由机器裁定。系统级 UIUX blueprint 的起草执行角色是 `uiux-designer`（design-execution role）；把它接进 review-moderator 派发链是**待办的第五阀 follow-up**，当前 UIUX 轴集直接在既有 governance lane 下审，本命令**不**声称 moderator 已派发 `uiux-designer`。
   - **contract-touching flag**：plan 是否实际触 `docs/contracts/**`（shim API / display 块 / 笔顺 JSON / 课表 MD）—— 这是**横切标记**，不是独立 lane；命中则在所属 profile 内触发 `architect-reviewer` 路由。
   - **mixed**（命中 >1 lane）：用 AskUserQuestion 呈现分类，**默认建议拆成两次 `/arch-change` 跑**（spec 一次、governance 一次），除非强耦合到必须同一 PR 原子落地；由 human 定。
   - **纯契约面**（`docs/contracts/**`，无 spec/governance 配套）**：不在本命令处理 —— 直接派 `architect-reviewer` + 人类契约门，告知用户走契约门，本命令收尾。
   分类结果**必须显式展示给用户**；含糊时用 AskUserQuestion 确认，别猜。
   - **review-depth（stage 2，lane 定后判；本段 = Track A 内部 G 层路由）**：`lightweight-architect-review` vs `full-adversarial`。**lightweight 仅限小/低风险 G2**（G1 指令根 + G3 强制层新 idea 恒 full-adversarial + human，永不 lightweight）。lightweight 快道：派 `architect-reviewer` 判 eligibility（4 裁断 LIGHTWEIGHT_OK / ESCALATE_FULL_REVIEW / HUMAN_REQUIRED / CONTRACT_GATE_REQUIRED，机检硬排除 vs 主观残留）→ 若 `LIGHTWEIGHT_OK` 则你**独立复判 eligibility**（非 pass-through，作者≠判官保全）→ 通过后**由主线程 packet-bound landing（landing-manifest 绑定）** → 记 lightweight 证据（`lightweight-intake.md` / `architect-reviewer-verdict.md` / `architect-landing-summary.md` / 验证）。不达判据 → `full-adversarial`（下面步骤 3–7）。详见 [arch-review-protocol.md](../references/arch-review-protocol.md)「Review-depth 分类 + Lightweight path」（Track A 触发时适用；默认路由 = Track B，见「双轨路由」节）。
2. **归一制品目录**：spec profile → `delivery/changes/arch/arch-review-<slug>/`；governance profile → `delivery/changes/arch/gov-review-<id>/`。
3. **派 architect 起草候选**（作者阶段，对偶 dev-loop builder）：派 `architect`（起草 subagent）→ 产出**候选**：
   - **spec lane** → spec 草案 / 候选修订写入 `docs/requirements.md` + `docs/architecture.md`（**写入不等于 spec 已冻结**）。
   - **governance lane** → `delivery/changes/arch/gov-review-<id>/PROPOSAL.md`（**不落 G1/G2/G3 canonical**）。
   - architect 只起草候选、准备 review 背景 / 候选关注点 / 历史 canary 线索；**不写冻结 charter、不自评 spec 可冻结、自改递归仍须 human**。
4. **契约预审（仅当 contract-touching）**：派 `architect-reviewer` 四分类；`SPEC_CONTRACT_DRIFT` / `CONTRACT_CHANGE_REQUIRED` → 停在人类契约门，配套 spec / proposal 暂存 packet 不单独落地（原子性：与契约改动同一 PR）。
5. **路由进 profile 跑完整 loop**：按分类进入 spec profile 或 governance profile，按 `.claude/agents/review-moderator.md` Process 推进：`scenario-author` 冻 charter → `challenger`↔`defender` 辩论内循环（你驱动，**禁止嵌套派生**）→ `adjudicator` 裁定 + 标 outcome → 级联续问穷尽。全程维护 `debate-log.md`。
6. **落地 / 收口（profile 分叉）**：
   - **spec profile**：低影响 CONFIRMED + 明确改法 → 你直接落设计层 spec（`docs/requirements.md` / `docs/architecture.md`）；高影响 / 跨系统 / deadlock / L0 → AskUserQuestion 交 human（**建议即地板**）。过 pass-oracle 5 关（+ handoff 出口）方可标 spec「可冻结」；需实现侧修复 → 产 `dev-loop-handoff.json` 交 `/build-feature`。
   - **governance profile**：**不落任何 canonical**；confirmed 治理改法写进 `decision-packet.md`（目标文件 / 改法 / 影响面 / 是否触自改递归·契约·强制层 / 推荐）；治理默认高影响，AskUserQuestion 交 human；改 agent 定义 / 自改递归 / 强制执行层 / 指令根**必 human**。过 governance pass-oracle 6 关方可标「可批准」，human 批后**先定稿冻结 PROPOSAL（architect 回灌改法 → human 冻结 → 你核验 + `proposal-consolidation-check.mjs`）再由主线程 packet-bound canonical landing → 主线程派 `landing-verifier` 独立核验落地保真（`landing-fidelity-check.mjs` + 逐文件 diff，写 `landing-verdict.md`），verdict 绿才 human merge**（见 `arch-review-protocol.md`「Governance profile 定稿」）。
7. **收尾**：确认 `adjudicator` 已将制品**写到 reviewDir 磁盘**——`findings.md` + `review-packet.md`（governance 另加 `decision-packet.md`）+ 按域写出的 CRQ 账本条目（`docs/crq/{gov,tech,requirement}/`，写完跑过 `node .claude/scripts/crq-render.mjs`），`debate-log.md` 齐备，返回只给结构化摘要；任一制品仅以文本返回 → 退回 adjudicator 重写盘，不收口。任一 pass-oracle 关 RED → 退回重跑，不放过。

> mixed 强耦合必须同一 PR 时：同一会话**先跑完 spec profile 落地，再跑 governance profile**；否则拆两次会话。
> 你不改 `docs/contracts/**`、不自动改 L0 蓝图、不落 G1/G2/G3 canonical、不直推 main、不自评通过、不替 human 拍板。
