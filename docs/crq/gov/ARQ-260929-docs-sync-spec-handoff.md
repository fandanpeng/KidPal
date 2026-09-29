---
id: ARQ-260929-docs-sync-spec-handoff
title: docs-sync 对 spec 面写权缺口——human 裁 integrator 对 requirements/architecture 零写权，spec 文本同步交 architect 承接
domain: gov
status: blocker
severity: SHOULD
owner: architect
trigger: 260929-gov-review-agent-def-alignment 批 landing 时随 T2 落字；下次 dev 包触 spec 文档同步时首次实走新流向
opened: 2026-09-29
closed: ""
closedBy: ""
hardExclusions: true
paths: [.claude/scope-map.json, .claude/agents/integrator.md]
source: "260929 批 FROZEN 前 human 定稿裁定（AskUserQuestion 原话「这个缺口让 architect 去更新，你记一条 ARQ，这两个都不能有 integrator 更新」，2026-09-29）；背景辩论 = 同批 F3/F5（delivery/changes/arch/260929-gov-review-agent-def-alignment/findings.md）"
---

**证据**：260913 去 systems 层重构后 integrator.allow 的 `docs/systems/**` 匹配不到任何文件——integrator docs-sync 对 spec 面断腿（F5/F3 实证链，判官亲证 enforce-scope.mjs:188-195 白名单语义）。原 PROPOSAL T2 拟授 integrator 两 spec 文件写权修复断腿；human 定稿期审阅时质疑「integrator 为什么要写 requirements.md / architecture.md」，经 moderator 举例说明两文件性质差异（architecture=怎么造、requirements=该做什么且语义权威=human）后裁定：**integrator 对两份 spec 文件均零写权**。

**影响面**：① 需求面单写点纪律保住——requirements.md 写权维持 story-author / `/requirement-change` 单链，不产生 integrator 第二写点；② docs-sync 实际写 spec 文本的场景（如拓扑变更后同步 architecture.md 部署节）由 architect 承接——architect.allow 经 T2 扩权腿授两 spec 文件（E1 语义：草案写 canonical 两文件、freeze 归 review-moderator spec profile）；③ 不修复的残余 = dev 包 docs-sync 检出 spec 漂移时多一跳转手（integrator → planner/architect）。

**建议改动**（随 260929 批 T2 落字，已入 PROPOSAL v0.4）：① integrator.allow 去 `docs/systems/**` 且**不增任何 spec 文件**；② integrator.md 的 docs-sync 语义句随批（T5/T7 触面内）补一句「spec 面（requirements / architecture）漂移检出 → 交 architect 更新、零自改」；③ architect.allow / review-moderator.allow 换授两 spec 文件；④ 本条目即流向登记，批 landing 后翻 closed。

**风险**：riskTier: low——流向裁定已由 human 当场给出；落字随批、无新增闸。review profile：随 260929 批 full-adversarial + human（批内既有硬排除承袭）。
