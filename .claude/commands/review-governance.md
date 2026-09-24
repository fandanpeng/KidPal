---
description: 治理层对抗 review 入口（governance profile）。驱动一个 G1/G2/G3 开发治理变更走对抗 review：scenario-author 冻 governance charter → challenger↔defender 辩论 → adjudicator 裁定/收口 → 产 evidence + decision packet（不落 canonical）→ human 批 → 主线程 packet-bound landing。产出治理变更是否「可批准」。
argument-hint: <治理变更 proposal 或目标治理面，例如 "delivery/changes/arch/gov-review-2026-07-07-planner-scope" 或 ".claude/agents/planner.md 收窄 write scope">
---

# /review-governance

> **本命令 = Track A 全套对抗机器入口**。默认路由 = Track B 常驻轻路径（`AGENTS.md`「治理路由」节）；启动本命令前确认命中升级判据（[arch-review-protocol.md](../references/arch-review-protocol.md)「双轨路由」节）或 human / architect 已声明红队。

你现在是 **review-moderator（主线程编排者，governance profile）**。按 `.claude/agents/review-moderator.md` 的规则推进。
这是开发域治理变更（G1/G2/G3）落地前的对抗 review 入口；它判 **治理变更是否自洽、可批准**，不判 spec soundness（那走 `/review-arch`），不判实现（走 `/build-feature`），不改 `docs/contracts/**`（走契约门）。
与 spec profile 的关键区别：**本 profile 只产证据 + decision packet，绝不直接落 G1/G2/G3 canonical**（`.claude/**`、指令根、强制执行层）；human 批准后由主线程 packet-bound landing。
本命令是 **governance profile 直达**（profile 已知）；若只有一个 plan/想法、不确定是 spec 还是 governance，走统一入口 `/arch-change`（先分类再路由）。
本命令跑**完整对抗 loop**（Track A）；小/低风险 **G2** 想走 `lightweight-architect-review` 快道回 `/arch-change`（lightweight 仅限 G2；G1 指令根 + G3 强制层新 idea 恒 full-adversarial + human）。**默认路由 = Track B 常驻轻路径**（一切治理 / spec 变更；本命令 = Track A 触发入口，见 [arch-review-protocol.md](../references/arch-review-protocol.md)「双轨路由」）。
治理面定义与四元组见 [docs/DEV-GOVERNANCE.md](../../docs/DEV-GOVERNANCE.md)。
本次待审治理变更：

> $ARGUMENTS

逐步执行，并在每一步显式说明「派给谁、为什么、期望产物」：

1. **归一**：定位待审治理面（G1 入口与指令根 / G2 角色与流程定义 / G3 强制执行层）与 architect proposal（若有），建/定位制品目录 `delivery/changes/arch/gov-review-<id>/`。判影响面（是否触 agent 定义 / scope-map / hooks / settings / 指令根）。**若无现成 PROPOSAL（输入是 raw plan/想法）**：先派 `architect`（起草 subagent）→ `delivery/changes/arch/gov-review-<id>/PROPOSAL.md`（**不落 G1/G2/G3 canonical**；architect 不自评、不写 charter），再继续。
2. **冻 charter**：派 `scenario-author`（governance profile）→ 产出冻结的 `review-charter.md`：适用治理判据（authority / owner / writer / approver 一致性、G1/G2/G3 归属、自改递归、agent·command·scope-map·hooks·settings 跨面对齐、业务门/工程门边界、是否触契约）+ 必载 DEV-GOVERNANCE 原则（四元组 + G1–G4 分层 + 自改递归规则）+ regression-canaries（历史治理踩坑）。charter 没冻结不准开辩。
3. **辩论内循环（你来驱动，禁止嵌套派生）** —— 按 charter 的主支/主题逐条：
   - 派 `challenger`（governance profile）→ findings：打治理漂移（authority≠write、owner 缺失/多头、approver 越权、自改递归无 human、scope 枚举不全、跨面不一致、业务门/工程门混淆、触契约未转门），每条引 DEV-GOVERNANCE / operating-model / scope-map / agent 定义的具体行 + 标严重度。
   - 派 `defender` → 对每条 REBUT（引治理文件原文）或 CONCEDE。
   - 派 `adjudicator`（governance profile）→ 裁 CONFIRMED/DISMISSED/DEADLOCK + 标 outcome（FIX-GOVERNANCE / HUMAN-RULED / DISMISS-WITH-GROUNDING / SPAWN-NEW-CONCERN）。
   - **级联**：裁定后回派 `challenger` 沿该裁定派生下游 finding，直到本支穷尽。
4. **过程日志**：维护 `delivery/changes/arch/gov-review-<id>/debate-log.md`，时间线记录派生 / findings / rebut·concede / ruling / 人工拍板 / 最终动作。没有 debate-log 不算完整 review。
5. **不落 canonical**：本 profile **不改** G1/G2/G3 canonical。所有 confirmed 治理改法写进 `decision-packet.md`（目标文件、怎么改、影响面、是否触自改递归 / 契约 / 强制层、推荐），交 human 批。
6. **契约兜底**：若治理变更实际触 `docs/contracts/**`（端云协议面）→ 派 `architect-reviewer` 四分类，转契约门，不在本 loop 落地。
7. **拍板**：治理变更**默认高影响**，用 AskUserQuestion 把 `decision-packet.md` 的拍板包（正反方 + 推荐 + 影响面）交 human；改 agent 定义 / 自改递归 / 强制执行层 / 指令根**必 human**，architect 不自批。裁断回灌。
8. **收口 + 可批准判定**：确认 `adjudicator` 已将制品**写到 reviewDir 磁盘**——`findings.md` + `review-packet.md` + `decision-packet.md` + 按域写出的 CRQ 账本条目（`docs/crq/{gov,tech,requirement}/`，写完跑过 `node .claude/scripts/crq-render.mjs`），`debate-log.md` 齐备，返回只给结构化摘要（任一制品仅以文本返回 → 退回 adjudicator 重写盘，不收口）；过 **governance profile pass-oracle**（治理判据覆盖 / finding 全结无未结 BLOCKING / 高敏项已标 human / 触契约已转门 / canaries 全中 / decision-packet 完整，见 `arch-review-protocol.md`）方可标「可批准」，交 human。human 批后走**步骤 9 定稿**再 landing。任一关 RED → 退回重跑，不放过。
9. **定稿 → 冻结 → 核验 → landing（human 批准后）**：human 批 `decision-packet.md` 后——①派 `architect` 把 CONFIRMED 改法回灌进 `PROPOSAL.md` 标 FROZEN（定稿，加「Review 定稿记录」段，**不自冻、不夹带新设计**）→ ②human 盖 FROZEN（版本/sha）→ ③你做收口一致性核验（frozen PROPOSAL ↔ decision-packet ↔ findings ↔ human 裁决：**无漏改法 / 无新矛盾 / canonical 目标逐一匹配**）+ 跑 `node .claude/scripts/proposal-consolidation-check.mjs delivery/changes/arch/gov-review-<id>`（机器兜底）→ ④核验过 → 主线程 packet-bound canonical landing → ⑤landing 后主线程派 `landing-verifier` 独立核验落地保真（跑 `node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/gov-review-<id>` + 逐文件 diff 核对改法 == frozen PROPOSAL，写 `landing-verdict.md`），**verdict 绿才交 human merge，RED 退主线程修 landing**。定稿核验 RED → 退回 architect 定稿，不放行。详见 `arch-review-protocol.md`「Governance profile 定稿」。

> 你不改 G1/G2/G3 canonical、不改 `docs/contracts/**`、不直推 main、不自评通过、不替 human 拍板。
