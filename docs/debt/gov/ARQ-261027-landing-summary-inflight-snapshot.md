---
id: ARQ-261027-landing-summary-inflight-snapshot
title: landing-summary 的在途件登记是写盘时点快照、无「实跑时点」口径句——结项等后落动作被 verifier 读成时序差 ADVISORY
domain: gov
status: deferred
severity: NIT
owner: 主线程（summary 出具口径）
trigger: 下一治理批 landing-summary 出具时并批消费（architect-landing-summary.template.md 一句增补，文面级）
opened: 2026-10-27
closed: ""
closedBy: ""
paths: ["docs/templates/architect-landing-summary.template.md"]
source: ["delivery/review/261027-gov-review-p7-devloop-doc-cluster/landing-verdict.md ADVISORY A1", "delivery/review/261027-gov-review-p7-devloop-doc-cluster/retro.md F2"]
---

**证据**：261027-gov-review-p7-devloop-doc-cluster 批——主线程 landing-summary §2/V4 登记在途件三条（INDEX / task_plan / .codebase-memory），随后执行 FROZEN §6 留根 1 授权的 ARQ 结项账本动作（docs/debt/gov 八件）；landing-verifier 实跑 `git status` 时点晚于 summary 写盘，读出多出的八件，因 summary 无「在途件登记以写盘时点实跑为准」口径句，只能标 ADVISORY A1 提示「防读者误读为漏报」（verdict 原文）。

**影响面**：每个 Track B 批的 landing-summary 与 landing-verifier 时点差——主线程在 summary 出具后总会再动账本 / 生成件（ARQ 结项、INDEX upsert），时序差每批复现，每批多一条可免的 ADVISORY。

**建议改动**（交 review、不自批）：`architect-landing-summary.template.md` §3（诚实注记）或用法注记加一句字面义务——在途件登记须注「以本 summary 写盘时点实跑为准；其后主线程账本 / 生成件动作以 landing-verifier 实跑为最终口径」，一句即可，不改结构门脚本。

**风险**：riskTier: low——纯模板文面，零行为语义变化。
