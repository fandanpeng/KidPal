---
id: ARQ-260907-gate-lib-script-separation
title: pre-merge-escalation-gate.mjs 等可执行脚本无「可导入库 / 可执行脚本」分离——顶层直跑 + process.exit，import 即带跑闸逻辑，跨脚本复用（如 merge-driver T1 复用五要素核验）只能 spawn（双进程开销 + 语义耦合字面复用）
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-07
closed: 2026-09-08
closedBy: ""
paths: []
source: ""
---

260908-gov-review-merge-chain-hardening T6 落地：`resolveHumanApproval` 迁入 `.claude/scripts/lib/governance-gate-shared.mjs` 单源、gate 改具名导入；`pre-merge-escalation-gate.mjs:499` `isDirectRun()` import.meta 守卫（被 import 时不跑 CLI）。本行 2026-09-18 由 §1 陈旧行归位。
