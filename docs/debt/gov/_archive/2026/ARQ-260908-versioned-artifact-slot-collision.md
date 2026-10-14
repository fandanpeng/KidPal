---
id: ARQ-260908-versioned-artifact-slot-collision
title: 并行 Pod 可**同时占用同一个 `.vN` 版本槽位**（两个包各自把 prompt 从 v2 forward-bump 到 v3），合并时 add/add 冲突且**两种单边消解都不安全**——被「版本字面 carrier 齿」遮蔽的那一种会静默丢约定正文
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-08
closed: 2026-09-08
closedBy: ""
paths: []
source: ""
---

同批 T1（merge-driver `detectVersionedAddAdd` add/add 特判 + 并集指引）+ T2（`.claude/references/pod-coordination.md` §10c 槽位预约与登记表）落地；本行 2026-09-18 核销前实测两者均在位、§10a 消解纪律五条含版本化制品禁单边。本行 2026-09-18 由 §1 陈旧行归位。
