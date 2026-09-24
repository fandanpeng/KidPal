---
id: ARQ-260914-legacy-sxay-story-id-batch
title: `Sx-Ay` 老仓 story id 遗产字样在 260912 批 manifest 外多处存活，与新口径（`S-Fx.y` 节锚 + `S-Fx.y-AC<n>` AC 复合句法 + acceptance 本地 id）并存待批量对齐
domain: gov
status: deferred
severity: SHOULD
owner: architect（定对齐范围）+
trigger: 下次动上述任一文件的治理批，或单开批量对齐小批（可与 ARQ-260914-portfolio-reconcile-launchpreconditions 并批）
opened: 2026-09-14
closed: ""
closedBy: ""
paths: []
source: 260912-gov-review-portfolio-decomposition-chain（PROPOSAL §6 留根 + architect-landing-summary 诚实记录）
---

**证据**（260912 拆包链批 landing 核验期全仓清点）：manifest 9 文件内的例句与格式例已全量换新，manifest 外命中面未动——`.claude/agents/integrator.md`、`.claude/commands/build-feature.md`、`.claude/references/minimal-implementation.md`、`acceptance-evidence.md`、`code-review-gate.md`、`.claude/agents/builder.md`、`.claude/references/dev-loop.md`、acceptance-agent description、`.claude/scripts/lib/acceptance-id-shared.mjs`、`mutation-check.mjs` 等（详单见该批 architect-landing-summary 诚实记录节）。**影响面**：新会话读旧例句会学旧口径（story 引用散装 `Sx-Ay` 而非 `S-Fx.y` 锚点引用）；`acceptance-id-shared.mjs` 若参与 id 解析需核对新复合句法的兼容性（`S-F1.4-AC3` 形态是否会被误拆）。**建议改动**（候选，交 review，不自批）：批量把例句换新口径；解析面先跑兼容性夹具再定是否需要代码改动；historical 冻结件/已结 review 记录不动（历史 append-only）。**riskTier: low**。owner=architect（定对齐范围）+ 主线程（批量落地）。
