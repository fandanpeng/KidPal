---
id: ARQ-260914-legacy-sxay-story-id-batch
title: `Sx-Ay` 老仓 story id 遗产字样在 260912 批 manifest 外多处存活，与新口径（`S-Fx.y` 节锚 + `S-Fx.y-AC<n>` AC 复合句法 + acceptance 本地 id）并存待批量对齐
domain: gov
status: closed
severity: SHOULD
owner: architect（定对齐范围）+
trigger: （已结项）原触发条款由 260928-gov-review-portfolio-boundary-story-propagation 承接完毕
opened: 2026-09-14
closed: 2026-09-28
closedBy: 260928-gov-review-portfolio-boundary-story-propagation（T8：census 权威详单 + 例句换新 10 文件 + acceptance-id-shared 兼容夹具钉「S-F1.4-AC3 不被误拆、解析面零代码改动」结论；排除 2 条 historical 真引用）
paths: []
source: 260912-gov-review-portfolio-decomposition-chain（PROPOSAL §6 留根 + architect-landing-summary 诚实记录）
---

**证据**（260912 拆包链批 landing 核验期全仓清点）：manifest 9 文件内的例句与格式例已全量换新，manifest 外命中面未动——`.claude/agents/integrator.md`、`.claude/commands/build-feature.md`、`.claude/references/minimal-implementation.md`、`acceptance-evidence.md`、`code-review-gate.md`、`.claude/agents/builder.md`、`.claude/references/dev-loop.md`、acceptance-agent description、`.claude/scripts/lib/acceptance-id-shared.mjs`、`mutation-check.mjs` 等（详单见该批 architect-landing-summary 诚实记录节）。**影响面**：新会话读旧例句会学旧口径（story 引用散装 `Sx-Ay` 而非 `S-Fx.y` 锚点引用）；`acceptance-id-shared.mjs` 若参与 id 解析需核对新复合句法的兼容性（`S-F1.4-AC3` 形态是否会被误拆）。**建议改动**（候选，交 review，不自批）：批量把例句换新口径；解析面先跑兼容性夹具先定是否需要代码改动；historical 冻结件/已结 review 记录不动（历史 append-only）。**riskTier: low**。owner=architect（定对齐范围）+ 主线程（批量落地）。

**结项记录（2026-09-28 append）**：并批进 260928-gov-review-portfolio-boundary-story-propagation T8 结项——起草期机械 census（`grep -rnoE`）较本条旧详单多发现 `acceptance-coverage-check.mjs`、排除 `contract-change-evidence.md:29`（真历史制品引用）；例句换新 10 文件 + scripts 夹具 token 连动换新；兼容性夹具（acceptance-id-shared selfCheck 夹具⑨）实测 `S-F1.4-AC3` 复合句法不被误拆，**解析面零代码改动**结论钉夹具防回归；残留 census 恰余 2 条声明排除项（contract-change-evidence.md:29 + dev-loop.md:166 的 260731 力度变更叙事句——后者为本条旧详单外新增裁断，排除理由记 architect-landing-summary）。historical 冻结件 / 已结记录零触碰。
