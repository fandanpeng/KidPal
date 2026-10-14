---
id: ARQ-260906-lfc-canonical-anchor-gap
title: landing-fidelity-check 机械门锚正则不识仓根点文件/changes 路径形态，此类落点无法机锚
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

锚提取与未申报改动检测分离：gate-shared 新增 ANCHORABLE_TEST（CANONICAL_TEST ∪ 仓根点文件 ∪ changes/ 前缀），:386 锚过滤切换、:367 检测面不动；自检补两形态用例（section2Anchors 19/19）。本批自身 13 件 manifest 首次实现锚集 ≡ manifest 机判（260906 批 T7/T9 整裁腿缺口根治）。 证据：[260907-gov-review-arq-governance-batch-2](../changes/arch/260907-gov-review-arq-governance-batch-2/)。
