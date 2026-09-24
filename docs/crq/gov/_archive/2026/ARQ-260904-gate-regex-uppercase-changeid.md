---
id: ARQ-260904-gate-regex-uppercase-changeid
title: 闸脚本 `CHANGE_ID_PREFIX_RE` 不认大写字符，混合大小写 changeId 被误判 pre-cutoff（热修追认 + 永久化）
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-04
closed: 2026-09-06
closedBy: ""
paths: []
source: ""
---

changeId 正则收敛 lib 单源：新增 `.claude/scripts/lib/changeid-shared.mjs`（`[a-zA-Z0-9-]`），8 消费方（closeout-check / taskpackage-required-check / devloop-artifact-check / dispatch-stamp / journal-append / loop-health-report / loop-metrics / supervisor-gate-check）删本地常量、import 单源；自检 4/4 + 全仓 census 仅剩 lib 一处定义（[260906-gov-review-arq-governance-batch](../changes/arch/260906-gov-review-arq-governance-batch/)，T1）。
