---
id: ARQ-260901-closeout-check-delivery-status-gap
title: closeout-check.mjs 依赖 delivery-status.mjs 缺失致机械 closeout 闸崩溃
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-01
closed: 2026-09-01
closedBy: ""
paths: []
source: ""
---

迁移补 `delivery-status.mjs`（`export checkStatus`）+ `.delivery/` 骨架（STATUS.md / delivery-workspace.json / milestones.jsonl 三件入仓）即恢复。`closeout-check.mjs:319-330` 的 delivery-milestone 子闸是条件路径（仅 `taskPackage.deliveryMilestoneStateChanged===true` 才 checkStatus）+ `:322` existsSync 守卫，非无条件崩。冒烟：本仓现有包 `node .claude/scripts/closeout-check.mjs` 输出 `ok:true`（不再崩）。注：DEV-GOVERNANCE §1.1「.delivery 未迁」句已过时（.delivery 骨架现存在于仓），dogfood 数据面是否启用属另一议题、不影响本闸。
