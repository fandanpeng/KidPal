---
id: ARQ-260831-p4-stroke-dasharray-visual-check
title: P4 笔顺「未写笔画的虚线点链」dasharray 观感与契约 D3 canvas 折线渲染需视觉对齐核验
domain: tech
status: deferred
severity: COULD
owner: ""
trigger: P4 dev-loop 包验收阶段
opened: 2026-08-31
closed: ""
closedBy: ""
paths: []
source: 260830-arch-review-ui-impl-strategy（PROPOSAL §2 T3）
---

设计稿用 dasharray 描边实现虚线点链、允许「自然淡网格」观感；契约 D3 定的是 canvas 折线逼近贝塞尔，未定虚线渲染细则。两者之间需一次目测对齐；若确认需要契约字段，单独走契约门（不预批）。
