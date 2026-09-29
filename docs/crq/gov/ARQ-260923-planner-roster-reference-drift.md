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

---

**追注（2026-09-29，主线程 agent def 全量核查复确认）**：本次 16 个 def 逐文件核查复确认本条仍未解——`.claude/agents/` 现存 16 个 def 仍无 uiux-author.md，planner.md:4 frontmatter `Agent(...)` 列表与 Step 6b、acceptance-author.md:23、task-intake-and-slicing.md:326（UIUX Ladder L2 行）仍引用。route-regression-check.mjs 通过（该机检只断言白名单不退化、不查被引 agent 存在性），故机检不可捕获、仍靠人读对账。处置并入 260929 对齐批：与本核查新立的 `docs/crq/gov/ARQ-260929-agent-def-reality-drift.md`、`docs/crq/gov/ARQ-260929-agent-def-provenance-noise.md` 同车，二选一裁定（补 def vs 摘引用降级）列为该批 human 取舍点。

**更正注记（2026-09-29，260929-gov-review-agent-def-alignment 回灌定稿期 F16 修复集 append；不改上方既有段）**：正文「`:54` 正文 Step 6b」的行号已漂移至 **:61**（challenger F16/F9 复核、adjudicator CONFIRMED，回灌期复证一致；引用全量实为 7 处 = planner.md:4/:61、acceptance-author.md:23、task-intake:320/:326、dev-loop.md:55/:68）。human 已裁 **OQ1=(b) 补 def**（非摘引用）。按账本纪律 append 更正、既有行不覆盖。
