---
id: ARQ-260928-landing-summary-skeleton-template
title: landing-summary 制品无骨架模板——格式义务（manifest 围栏块 / packet-id 字样）靠会话记忆承载，本批首验即因此 FAIL 一轮
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一治理批（动 docs/templates/** 或 PROPOSAL.template 的治理批时并批；或与 ARQ-260914-landing-preflight-anchor-parse 同族可并批）
opened: 2026-09-28
closed: ""
closedBy: ""
paths: [docs/templates/]
source: [260928-gov-review-portfolio-boundary-story-propagation/retro.md §4 建议 1, landing-verdict.md FAIL→PASS 演进记录]
---

**证据**（260928 批 landing-verifier 首验 FAIL 实测）：`architect-landing-summary.md` 首版缺 ```landing-manifest 围栏块（B2 fail-loud）+ 全文无 `packet-id` 字面 token——结构门 exit 1、verdict FAIL 一轮。PROPOSAL 有 `docs/templates/PROPOSAL.template.md` 骨架（硬项清单 + 读法注 + 起草期自检三句），summary 是 packet 第三件却无对应骨架——格式义务全靠会话记忆承载， Track B 每批复现面。

**影响面**：每个 Track B / Track A 治理批的 summary 制品；重复代价 = 每批一次 verifier FAIL 返工轮 + 判官复验轮。

**建议改动**（候选，交 review、不自批）：① 新建 `docs/templates/ARCHITECT-LANDING-SUMMARY.template.md`（硬项：manifest 围栏块逐字 == PROPOSAL §2 锚并集、packet-id 行、逐文件落地表骨架、验证集执行记录表、排除清单段、留根移交段）；② 或在 PROPOSAL.template 增一节「summary 制品义务摘要」由起草者复制。两案交 review 辩。

**风险**：riskTier: low——纯模板件，无行为面。改动面 = docs/templates（过程面文档）。owner: architect。
