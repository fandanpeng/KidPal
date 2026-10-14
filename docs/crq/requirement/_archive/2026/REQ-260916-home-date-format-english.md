---
id: REQ-260916-home-date-format-english
title: 首页辅助条日期显示为中文（真机无中文字形 → 框占位）——human 裁定改英文 `DD Mon` 格式（如 16 Sep），requirements.md S-F1.1 / WF-F1-01 设计稿殿后同步
domain: requirement
status: closed
severity: SHOULD
owner: ""
trigger: human 完成 requirements.md S-F1.1 / WF-F1-01 设计稿同步时销行
opened: 2026-09-16
closed: 2026-10-11
closedBy: docs/requirements.md §9 v1.57 版本行（260922-arch-review-spec-v5-ui-align，2026-09-22 落地）
paths: []
source: 260915-m1-n1b-p1-home-battery PH-A6 真机轮（2026-09-16，主线程代登记；本行 2026-09-16 由 §2 漂移归位）
---

实现已由 260915-m1-n1b-p1-home-battery Pod 按 human 裁定先行落地（pod-events human-decision 在案）；在案已知漂移 = 需求/设计文档尚未同步（agent 无权直改 spec 面，由 human 亲自同步），同步完成前 WF-F1-01 日期元素与实机不一致，不另开 review。

**结项（2026-10-11，主线程销行）**：销行条件已满足——需求面 v1.57（2026-09-22，260922-arch-review-spec-v5-ui-align 批）已把 S-F1.1 AC1 与 WF-F1-01 线框辅助条日期同步为英文简写（`Tue 22 Sep`，无「今天」字样）；docs/ui V5 设计稿母版（kidpal-ui-design-v5.html P1 辅助条）同为英文。账目开于 9-16、条件 9-22 已满足但当时未销，本次人工指示处理 requirement 域两条 REQ 时补销。销行时点滞后不影响实机一致性（实机自 Pod 落地起一直是英文格式）。
