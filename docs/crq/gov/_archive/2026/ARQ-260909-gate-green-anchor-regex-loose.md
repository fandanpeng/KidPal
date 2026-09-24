---
id: ARQ-260909-gate-green-anchor-regex-loose
title: 前置升级闸 gov 档「判官 GREEN」机锚含裸 `GREEN` 分支，任何正文出现该字样的 PROPOSAL **恒满足该关**
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-09
closed: 2026-09-20
closedBy: ""
paths: []
source: ""
---

260920-gov-review-verdict-anchor-align T2 落地（升级闸 gov 档 GREEN 锚由「全文 token 匹配（恒真）」改为 lib 单源 `VERDICT_STATUS_RE` 逐行判、**取最后命中 token = GREEN**；裸 `GREEN` 分支删除；`--self-check` +3 夹具）；landing-verdict PASS、commit `7a58398`。**处方证伪的完整分析**（真机制 = `PROPOSAL.template.md` 样板行「GREEN 结论前必须在场」自带 GREEN、16/26 份含它；四候选实测表）在 `260918-gov-review-merge-chain-friction` §6 留根① 与 `260920-gov-review-verdict-anchor-align` §2 T2 证据段。证据：[260920-gov-review-verdict-anchor-align](../changes/arch/260920-gov-review-verdict-anchor-align/)。
