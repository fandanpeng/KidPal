---
id: ARQ-281028-mutation-fixture-consumer-layer
title: 反转变红判别夹具默认打在判据计算层，消费/拦截层零判别——闸自检电池全绿而闸层带洞
domain: gov
status: deferred
severity: SHOULD
owner: architect（验证集义务文面）+ 判官（评审时核层别）
trigger: 下一个带 mutation/反转判别的治理批或 dev-loop 验证集义务文面批
opened: 2026-10-28
closed: ""
closedBy: ""
paths: ["docs/templates/PROPOSAL.template.md", ".claude/references/gov-change.md"]
source: [delivery/review/261028-gov-review-p8-gate-semantics-cluster/retro.md §3 F1 / §7 R2 段]
---

**证据（P8 批实测）**：P8 v0.2 改稿后自检电池 85/85 + mutation 6/6 全绿，但设备门「最懒路径」（launch-card 照抄模板 reason=none + diff 触登记 host 不可编路径）在闸消费层照旧放行——两个新负例与 mutation 判别 case 全部打在 assess（判据计算）层，evaluate/闸消费层零判别。判官对装配副本 `evaluate` 五案直调实测 `blocked=false`（A/B 两案）才接住；判官修正时以结构性不变量（`probed ∧ ok=false ⟹ required=true` 三条 return 路径）钉死。第二次实证：上一次同类坑为「判官+核验官各抓一次」（见 workstream 记忆 fixtures-must-discriminate）。

**影响面**：凡「计算层与消费/拦截层分离」的闸判据批——自检电池与 mutation 电池可以全绿而消费层静默带洞；带洞 FROZEN 落地 = 本批要治的「带病合并」同型风险回流。

**建议改动**（候选，交 review、不自批）：
1. 验证集义务加一句：判别 case 须落在**消费/拦截层**；计算层与消费层分离时两层各至少一个判别（正反配对）；
2. mutation-check 类设施对多文件闸脚本按消费层出口断言（如对 `evaluate` 直调）而非仅对内部函数变异；
3. 判官简报同步：R 轮核「判别 case 层别」为固定检查项（P8 R2 先例 = evaluate 五案直调）。

**风险**：riskTier: **low**——纯验证集义务文面（PROPOSAL.template + gov-change.md 判官简报注记），零脚本、零门判据改动；便宜验证 = 下批评审观察层别核验在位。review profile = **Track B**（三要件：① 执行测出 yes——P8 retro §3 F1 + §7 R2 实测；② 修法明确 yes——层别义务一句话，无设计分叉；③ 爆炸半径已被执行揭示 yes——R2 实测洞 + 电池全绿并存）。owner = architect（文面）+ 判官（评审项）。
