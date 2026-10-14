---
id: ARQ-260831-cross-surface-check-missing
title: landing-fidelity-check 的 cross-surface 子门子脚本未随迁出
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-08-31
closed: 2026-08-31
closedBy: ""
paths: []
source: ""
---

human 指示直修（项目初期不走 review）：子门裁剪为「脚本在场才跑」，`landing-fidelity-check.mjs` GAR-001 段已加 existsSync 守卫与实况注记；若将来 dev-loop 面（route-fixtures / dev-loop.settings.json）迁齐，从原仓补迁 `cross-surface-check.mjs` 即自动恢复启用。
