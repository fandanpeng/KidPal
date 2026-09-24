---
id: ARQ-260915-changes-index-lane-split
title: delivery/changes/INDEX.md 单文件混装 dev / arch 两段——Pod 合并流（dev 段刷行）与 gov 合并流（arch 段 upsert）每次各改同一文件，跨 lane 合并冲突逐次必现（merge-to-main skill 曾列其为仓内热点冲突文件）
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-15
closed: 2026-09-15
closedBy: ""
paths: []
source: ""
---

human 指示直改、跳过 Track B（如实记录）：INDEX 按 lane 拆分——`delivery/changes/dev/INDEX.md`（dev 段，closeout-check 增量刷行指向新路径）+ `delivery/changes/arch/INDEX.md`（arch 段，`--upsert` 指向新路径），旧单文件删除；`changes-index.mjs` 全量重建 / backfill / upsert 改写双 lane 文件（self-check 29/29、upsert 幂等验证、行内链接改为同目录相对形态 `<id>/`）；活跃引用面 11 处同步改指（AGENTS.md / DEV-GOVERNANCE.md / ARCH-REVIEW-QUEUE.md §0 状态轨迹行 / retro.template / GOVERNANCE-MIGRATION-GUIDE / pod-coordination / dev-loop / arch-review-protocol / merge-to-main skill / m1-home-qa decision-coverage / requirements §6 背景）；历史制品内旧引用按 append-only 不追改。证据 = 本次落地 diff（gov/kidpal 分支）+ 自检与重建输出。
