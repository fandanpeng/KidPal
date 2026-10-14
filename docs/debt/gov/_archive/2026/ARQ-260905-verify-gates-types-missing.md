---
id: ARQ-260905-verify-gates-types-missing
title: verify-gates.json 缺 types/schema 门条目 → merge-driver 运行时无条件 FATAL，机械合并通道自迁移起全死
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-05
closed: 2026-09-05
closedBy: ""
paths: []
source: ""
---

治理修复全程跑完：Track B 七步（human 立项 ack 2026-09-05 + OQ1=(a) 两门案/OQ2=(a) 折入 → R1 需修订（F1/F2+NIT×2）→ R2 GREEN → FROZEN v1.0 → packet-bound 落地 T1–T5 → landing-verifier GREEN）——`QUICK_CHECK_IDS` 对齐两门案 `['contract-lint','acceptance']`（向迁移意图 oracle 对齐；firmware-build 有意不入冒烟）、oracle/warn 文案/豁免注释同批对齐、verify-gates.json `_doc` 补记、升级闸 readGovAnchors frozen 正则放宽（`> ` 前缀双形态命中，execfn ARQ 遗留观察同步归位）、pod-coordination.md:72 门名同步。V1–V5 全绿（自检三件套首次 3/3、dry-run 运行时冒烟 FATAL 零命中）。证据：[260905-gov-review-verify-gates-align](../changes/arch/260905-gov-review-verify-gates-align/)。
