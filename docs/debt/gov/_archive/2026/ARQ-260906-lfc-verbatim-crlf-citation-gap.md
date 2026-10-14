---
id: ARQ-260906-lfc-verbatim-crlf-citation-gap
title: landing-fidelity 字节级 verbatim 对账不识 Windows CRLF 工作树（LF×CRLF 必不命中），citation 解析不识裸文件名引用
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-06
closed: 2026-09-07
closedBy: ""
paths: []
source: ""
---

checkVerbatimQuotes 双侧行尾归一后比对 + CRLF 自检用例（verbatim-quotes 6/6）；checkCitationAnchors 裸文件名前缀重解析（.claude/scripts/ / .claude/scripts/lib/ / .claude/hooks/ 三候选）、resolvedByPrefix 独立观察列消警留痕 + 自检 3 断言（citation-anchors 5/5）。残余：docs/ 裸名候选域缺口另登 ARQ-260907-lfc-citation-docs-prefix。 证据：[260907-gov-review-arq-governance-batch-2](../changes/arch/260907-gov-review-arq-governance-batch-2/)。
