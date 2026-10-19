---
id: TECH-261011-spec-v5-reconcile-note-drift
title: requirements v1.61 补充规则内 v1.57 时代 ⚠️ V5 版式对账注与 home_p1.c 实现已漂移（大字+倒计时贴纸+中文日期描述过时）
domain: tech
status: deferred
severity: COULD
owner: 主线程（需求面勘误批，档 0 勘误通道）
trigger: 下个需求面变更批顺带（requirement-change 档 0 勘误通道；不阻塞任何实现面在办批）
opened: 2026-10-11
closed: ""
closedBy: ""
paths: ["docs/requirements.md"]
source: "spine openQ q4（delivery/changes/intake/261011-p1-home-v5-rework.md，architect triage 取证 2026-10-11）+ 261011-p1-home-v5-rework architecture-gate-evidence.md §9 Follow-up"
cdq: CDQ-261011-p1-home-v5-rework
---

**证据**：requirements v1.61 补充规则内保留了 v1.57 时代的 ⚠️ V5 版式对账注（描述为「大字 + 倒计时贴纸 + 中文日期」），而 home_p1.c 实现自 260926 真机对齐轮（6ed6b67/efa8513）与 ui-v5-align 阶段 2（70ade0b）起已是行流 14px / 英文日期 / 黑帽标，倒计时贴纸已退役（architect triage 取证 2026-10-11）。**该注与实现漂移，属需求文本历史残留**。

**处置（human 已裁）**：本批不动需求面（v1.61 已 frozen 定稿、human 明确本批零改 requirements），登记为后续勘误批候选（档 0 勘误通道），不阻塞本批。勘误内容 = 删除或改写过时对账注，使补充规则与在役实现一致。

**cdq 指针**：本条目随契约批 261011-p1-home-v5-rework 登记（gate authority = CDQ-261011-p1-home-v5-rework，docs/CONTRACT-DECISION-QUEUE.md）；本条目本身不涉契约语义，仅作该批 tech 侧记账的 follow-up 承载（gate evidence §8「记 tech 侧条目 + cdq: 指针」义务）。

**riskTier: low**（纯文档勘误、零行为面）。

**追注（2026-10-19，261019-gov-review-docs-root-ledger-rehome 批）**：上文所引 gate authority 所指 CDQ-261011-p1-home-v5-rework 已迁 `docs/debt/contract/_archive/2026/`（closed）；`cdq:` 指针（ID 形）不受影响。
