---
id: ARQ-260907-friction-event-payload-schema
title: pod-events friction 事件 payload 无最小 schema 校验：空 payload 证据丢失、非标键归 unknown、ledger=0/retro=N——retro 覆盖比对的机器侧失真
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-07
closed: 2026-09-07
closedBy: ""
paths: []
source: ""
---

pod-events-aggregate.mjs aggregateFriction 新增 malformedFrictions 显式告警列（空 payload / 缺 frictionType / 缺 summary 三判据，unknown 兜底保留双轨）；schema 成文进 DEV-GOVERNANCE「## friction 事件最小 schema」+ planner.md 引用行。真数据重放：voice-roundtrip 两条 empty-payload 与 ws-switch 两条缺键全命中（55a43813 类证据丢失病灶兜住后车）。 证据：[260907-gov-review-arq-governance-batch-2](../changes/arch/260907-gov-review-arq-governance-batch-2/)。
