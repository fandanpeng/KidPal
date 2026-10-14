---
id: ARQ-260909-precedent-merged-detection-gap
title: `brief-inputs.mjs` 先例路 `isMerged()` 两判定源在本仓主合并形态下**同时落空**，先例集恒给陈旧包
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-09
closed: 2026-09-20
closedBy: ""
paths: []
source: ""
---

260920-gov-review-reader-scripts-align T1 落地（`isMerged` 增第三判定源：`human-decision` 事件 + `payload.decision` 含 `approve-escalated-merge` 双条件命中；`--self-check` 补正控双夹具 3 条断言，含 ESCALATE+人批 decoy 须入选 / 其他 decision 不入选 / 无事件文件 → false；landing-verdict PASS、commit `e0306f0`）。2026-09-20 主线程结移前复证：`node .claude/scripts/brief-inputs.mjs --self-check` 23/23 GREEN；第三判定源独立 census 复跑 = 候选 35 / 命中 25（批① FROZEN 时点口径 34/24，其后新增 1 包）。结移 §2 由本批簿记动作执行（changes/arch/260920-gov-review-reader-scripts-align/）。
