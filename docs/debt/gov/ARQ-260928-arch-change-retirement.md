---
id: ARQ-260928-arch-change-retirement
title: /arch-change 与统一 intake（/triage-change 四面探测）并存期入口未定界——双入口都能进跨面变更，/arch-change 开场无「先查既有 spine」条款，治理流量归并或长期并存的定界未做
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下次动 /arch-change、triage 统一路由面或 arch-review-protocol 的治理批；或 human 专项批
opened: 2026-09-28
closed: ""
closedBy: ""
paths: [.claude/commands/arch-change.md, .claude/references/change-intake.md, .claude/references/arch-review-protocol.md]
source: [260928-gov-review-cross-face-intake-spine/PROPOSAL.md §0 非目标① + §6 留根, docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md 结项注, 260928-gov-review-requirement-face/PROPOSAL.md §6 留根]
---

**证据**：

1. **双入口并存是本批的有意留根**：260928-gov-review-cross-face-intake-spine 批把 /triage-change 升格为统一 intake（四面探测 + spine + 分面扇出），但 /arch-change 本体零改动——跨面/治理变更现在两个入口都能进，无建档前互查的机械或文面联动（change-intake.md 单侧承载了「intake 开场查重」，/arch-change 侧条款缺位）。
2. **并存期去重只靠单侧文面**：change-intake.md §3「建档前查重……并存期 /arch-change 侧同此纪律（其文本改动归退役安置批，本文件单侧承载）」——若变更直接从 /arch-change 进，不建 spine、无 faceMap/facePlan/anchors，跨面顺序机查（launchPreconditions spine 源）对其落地的实现包不生效。
3. **先例留根两处同指**：requirement-face 批 §6「/arch-change 退役安置（governance 面 / spec 剩余流量归 triage 统一路由）→ 独立治理变更」+ 本源批 PROPOSAL §6 同项——两批均刻意不改 /arch-change 以控 profile。

**影响面**：并存期每个直接走 /arch-change 的跨面变更复现（无 spine = 无跨面顺序保障、无 anchors 对账）；频率取决于治理流量走哪个入口，暂无实测数据。

**建议改动**（候选，交 review、不自批；两候选存在真设计分叉）：

1. **归并退役**：/arch-change 退役，governance/spec 流量全部归 triage 统一 intake，其 Track A/B profile 与熔断语义由 change-intake.md 吸收；入口单一，去重问题消失。
2. **长期并存定界**：/arch-change 保留为治理/spec 专职入口（直达、免 intake），补开场「先查既有 spine」条款 + 与统一 intake 的分工定界成文（什么该从哪进）。

**风险**：riskTier: medium——路由面、影响每个跨面治理变更；不触契约与实现面。三要件：① yes（上述双入口实况 + 两处留根）/ ② yes（两候选真分叉：归并 vs 定界并存）/ ③ yes（并存期尚短，爆炸半径未被执行实测揭示）→ ③ 命中即升 full-adversarial 倾向，不美化快路径；最终 profile 由 architect-reviewer 机判。
