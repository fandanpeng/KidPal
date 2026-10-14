---
id: ARQ-260913-lfc-anchorable-delivery-gap
title: landing-fidelity §2 锚对账的 ANCHORABLE_TEST 不识 `delivery/milestones/**` 路径形态——含 delivery/milestones 目标的 manifest 恒报对账失配（解析盲点假红）
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-13
closed: 2026-09-15
closedBy: ""
paths: []
source: ""
---

修闸随 260914-gov-review-delivery-restructure landing 完成，且超原范围：`.claude/scripts/lib/governance-gate-shared.mjs:40` ANCHORABLE_TEST 现行实现含 `delivery/(?:workstreams|milestones|taskpackages|changes)/` 与 `firmware|server|tools/` 前缀。§1.1「待 architect 复核收口」项实测消。本行 2026-09-18 由 §1 陈旧行归位。
