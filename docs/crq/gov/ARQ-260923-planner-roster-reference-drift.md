---
id: ARQ-260923-planner-roster-reference-drift
title: planner.md 可派编制引用与 .claude/agents 实况漂移——frontmatter Agent 列表与正文 Step 6b 仍引 uiux-author / uiux-designer，而编制文件不存在
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一治理批（一文面小修，可搭任意批）
opened: 2026-09-23
closed: ""
closedBy: ""
paths: []
source: "loop-retro-2026-09-23（k3 pod-retro 卡点 5 + 实核 .claude/agents 清单与 planner.md:4/:54 现行文本；报告 delivery/changes/retro/2026-09-23-loop-retro.md）"
---

**证据**（GC1 对账 2026-09-23 实核现行锚点）：`.claude/agents/` 现存 14 个 agent def（acceptance-author 到 scenario-author，无 uiux-author.md）；而 `.claude/agents/planner.md:4` frontmatter `tools` 的 `Agent(...)` 列表含 uiux-author、`:54` 正文 Step 6b 仍写「在派 acceptance-author 之前先派 uiux-author」。260915-m1-k3-voice-round-ui 判定 UIUX Level 2 后实际派不出——直翻降级 + friction degraded 留痕，其 retro 判「编制缺口值得 gov 批收：要么补 .md，要么从可派列表里摘掉」；同构先例 = verifier 退役时的编制同步问题（该角色有退役注记，uiux-author 从未在册退役、属从未落地 + 引用未摘，故走新立而非复现记法）。**影响面**：凡 UIUX Level ≥2 的包（判定在、派发必失败一次）。**建议改动**（候选，交 review，不自批，二选一）：① 补 `.claude/agents/uiux-author.md`（按 6b 节职责面起草）；② 从 planner.md frontmatter 与 6b 摘除引用、UIUX L2 面降级为「直翻 + human design review」成文（k3 实走形态）。现状 = 派发失败后降级，风险面为零行为变更。**riskTier: low**。owner=architect。
