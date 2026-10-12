---
id: ARQ-260831-p4-stroke-dasharray-visual-check
title: P4 笔顺「未写笔画的虚线点链」dasharray 观感与契约 D3 canvas 折线渲染需视觉对齐核验
domain: tech
status: closed
severity: COULD
owner: ""
trigger: P4 dev-loop 包验收阶段
opened: 2026-08-31
closed: 2026-10-12
closedBy: 主线程（2026-10-12 tech 域 CRQ 盘点，human 指示；证据见各条目收口节）
paths: []
source: 260830-arch-review-ui-impl-strategy（PROPOSAL §2 T3）
---

设计稿用 dasharray 描边实现虚线点链、允许「自然淡网格」观感；契约 D3 定的是 canvas 折线逼近贝塞尔，未定虚线渲染细则。两者之间需一次目测对齐；若确认需要契约字段，单独走契约门（不预批）。

---

## 收口（2026-10-12 主线程盘点）

**核验对象已消失**：P4 查生字页已由 M2-K2（260917-m2-k2-retract-p4-three-page）撤出常驻页循环——`firmware/main/display/kidpal_render_node.h` 枚举现为 `KP_PAGE_HOME / KP_PAGE_SCHEDULE / KP_PAGE_STORY` 三页、`KP_PAGE_CHAR` 整值撤出（2026-10-12 grep 实证，头注释载 2026-09-17 M2-K2 撤页记录）。笔顺 dasharray 虚线点链的视觉对齐核验失去承载对象，登记作废关闭。
