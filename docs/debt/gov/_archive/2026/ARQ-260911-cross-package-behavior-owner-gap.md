---
id: ARQ-260911-cross-package-behavior-owner-gap
title: 切包时跨包端到端行为无显式 owner——spec 设计被切片后，跨包的落地动作掉进包边界缝，每包在自己边界内绿、合起来与设计不符
domain: gov
status: closed
severity: ""
owner: k3
trigger: ""
opened: 2026-09-11
closed: 2026-09-20
closedBy: ""
paths: []
source: ""
---

260920-gov-review-bearing-strength-align **T1 承接派单侧残余**（拆包期判据早已由 260912 批 T3 消化进门 1 checklist ③）：`task-intake-and-slicing.md` 增 `crossPackageBehaviors` 字段级定义（canonical `{topic, owner, note, carriedBy}` + 两写面收编规则 + 承接包判定统一 `carriedBy ?? owner`）；`brief-inputs.mjs` 增第五路「跨包承接面」，把上游登记的承接项派进 input-brief（自检 +5 夹具；真包干跑 V5 命中 k4 那条 `owner=k3 / carriedBy=k4`）。landing-verdict `PASS + 3 ADVISORY`、commit `e3b19ee`
