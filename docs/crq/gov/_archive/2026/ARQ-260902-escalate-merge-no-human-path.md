---
id: ARQ-260902-escalate-merge-no-human-path
title: human 批准的 ESCALATE 无机械合并路径
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-02
closed: 2026-09-05
closedBy: ""
paths: []
source: ""
---

治理修复全程跑完：Track B 七步（human 立项 ack 2026-09-04 + OQ 全裁 (a) → architect R1 需修订 → R2 GREEN → FROZEN → packet-bound 落地 T1–T4 → landing-verifier GREEN）——升级闸新增 `--human-approval-ref <eventId>` 五要素核验口（ESCALATE + provenance 完整 + 核验 ok 放行；BLOCK/provenance/off-main/契约永不豁免）、merge-driver 透传 + merge commit message 留痕、planner.md 四处 + 五件 prose 补「批准后机械路径」。证据：[260904-gov-review-escalate-merge-approval](../changes/arch/260904-gov-review-escalate-merge-approval/)。
