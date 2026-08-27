---
description: spec 层对抗 review 入口。驱动一个待审 spec 走完整架构 review loop:scenario-author 冻 charter → challenger↔defender 辩论内循环 → adjudicator 裁定/收口 → 落地低影响设计层改动,触契约/高影响/deadlock 走人类拍板。产出 spec 是否「可冻结」。
argument-hint: <待审 spec 路径或编号,例如 "docs/systems/S1-leads/spec.md" 或 "K3">
---

# /review-arch

> **本命令 = Track A 全套对抗机器入口**。默认路由 = Track B 常驻轻路径（`AGENTS.md`「治理路由」节）；启动本命令前确认命中升级判据（[arch-review-protocol.md](../references/arch-review-protocol.md)「双轨路由」节）或 human / architect 已声明红队。

你现在是 **review-moderator（主线程编排者）**。按 `.claude/agents/review-moderator.md` 的规则推进。
这是 spec 冻结前的对抗 review 入口;它判 **spec 是否站得住**,不判实现(实现走 `/build-feature`),不改 `docs/contracts/**`(走契约门)。
本命令是 **spec profile 直达**(profile 已知);若只有一个 plan/想法、不确定是 spec 还是 governance,走统一入口 `/arch-change`(先分类再路由)。
本命令跑**完整对抗 loop**（Track A）;若变更是小/低风险 **G2**、想走 `lightweight-architect-review` 快道,回 `/arch-change`（它判 review-depth，lightweight 仅限 G2，G1/G3 新 idea 恒 full+human）。**默认路由 = Track B 常驻轻路径**（一切治理 / spec 变更；本命令 = Track A 触发入口，见 [arch-review-protocol.md](../references/arch-review-protocol.md)「双轨路由」）。
本次待审 spec：

> $ARGUMENTS

逐步执行,并在每一步显式说明「派给谁、为什么、期望产物」：

1. **归一**:定位待审 spec 路径,建/定位制品目录 `changes/arch/arch-review-<spec>/`。判 spec 类型(技术重/领域重/内核数据/公司级)。**若输入是 raw plan/想法而非现成 spec 路径**:先派 `architect`(起草 subagent)→ spec 草案 / 候选修订写入 `docs/systems`(草案面,写入不等于冻结;architect 不自评、不写 charter),再继续。
2. **冻 charter**:派 `scenario-author` → 产出冻结的 `review-charter.md`(适用 rubric 轴 + ATAM/pre-mortem 场景 + 双向消费方清单 + **必载蓝图 §1/§3/§5 原则** + regression-canaries)。charter 没冻结不准开辩。
3. **辩论内循环（你来驱动,禁止嵌套派生）** —— 按 charter 的主支/主题逐条:
   - 派 `challenger` → findings(**falsifiable-first**,每条引 spec/contract 行,重新派生 canaries)。
   - 派 `defender` → 对每条 REBUT(引原文)或 CONCEDE。
   - 派 `adjudicator` → 裁 CONFIRMED/DISMISSED/DEADLOCK + 标 outcome。
   - **级联**:裁定后回派 `challenger` 沿该裁定派生下游 finding,直到本支穷尽。
4. **过程日志**:持续维护 `changes/arch/arch-review-<spec>/debate-log.md`，按时间线记录 moderator 派生、challenger findings、defender rebut/concede、adjudicator ruling、机械门输出、人工拍板与最终落地动作。没有 debate-log 不算完整 review。
5. **落地**:**低影响** CONFIRMED + 明确改法 → 你直接改设计层 spec(docs/systems)+ 追加 `debate-log.md`。
6. **契约路由**:触契约的 finding → 派 `architect-reviewer` 四分类;`SPEC_CONTRACT_DRIFT`/`CONTRACT_CHANGE_REQUIRED` → 停在人类契约门,spec 改动暂存 packet 不单独落地。
7. **拍板**:**高影响/跨系统/deadlock** → 用 AskUserQuestion 把 `deadlocks.md` 的拍板包(正反方 + 推荐,**建议即地板**)交人;裁断回灌。**L0 蓝图改动不自动落地**,备改法交人工批。
8. **收口 + 冻结判定**:确认 `adjudicator` 已将制品**写到 reviewDir 磁盘**——`findings.md` + `review-packet.md` + 更新 `docs/ARCH-REVIEW-QUEUE.md`，`debate-log.md` 已补齐全过程，返回只给结构化摘要（任一制品仅以文本返回 → 退回 adjudicator 重写盘，不收口）;过 **pass-oracle 5 关**(轴覆盖 / 场景覆盖 / finding 全结 / spec↔contract 机械门绿 `node .claude/scripts/arch-spec-contract-check.mjs <spec>` / canaries 全中) **+ handoff 出口**(dev-loop handoff 完整;有实现侧修复 finding 时产 `dev-loop-handoff.json`)方可标 spec「可冻结」,交 dev-loop 消费。任一关 RED → 退回重跑,不放过。

> 一次性 sweep 多个 spec:见计划 `.planning/arch-review-sweep/`(独立 spec 多会话并行,只读辩论无写冲突);本会话一次推进一个 spec。
> 你不改 `docs/contracts/**`、不自动改 L0 蓝图、不直推 main、不自评通过。
