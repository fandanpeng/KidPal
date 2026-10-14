---
id: ARQ-260905-escalation-gate-live-execfn-gap
title: 升级闸 live 路径 off-main 判定恒 fail-closed（buildCtx 缺 execFn，self-check 假绿）
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

human 2026-09-05 拍板同一批热修（先例 ARQ-260904-gate-regex-uppercase-changeid）：`buildCtx` 补 `execFn` 字段一行修（260904 批 gov 分支内，commit 随合并入 main）；复验 self-check 18/18 + V4 真实重放 off-main 恢复真判（Pod S 分支正确出 `source-on-main`、diverged 分支正确无理由）。附观察：`readGovAnchors` frozen 正则要求行首 `**状态：FROZEN vX.Y`，与本仓 PROPOSAL 惯用 `> ` 前缀形态错位（机锚从未命中过实际 PROPOSAL），260904 批状态行已适配，格式统一化留观察。证据：[260904-gov-review-escalate-merge-approval](../changes/arch/260904-gov-review-escalate-merge-approval/)（landing summary §4 合并门前闸校准）。
