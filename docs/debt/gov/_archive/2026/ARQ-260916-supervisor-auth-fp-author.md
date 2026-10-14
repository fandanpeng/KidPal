---
id: ARQ-260916-supervisor-auth-fp-author
title: supervisor-verdict-gate.mjs 安全轴路径正则 `/auth/i` 对 "author" 子串误报（实证两例，均致 riskTier 重算虚高 → ESCALATE、靠 human-approval 凭据放行）
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-16
closed: 2026-09-16
closedBy: ""
paths: []
source: ""
---

human 指示快速处理、不走 Track B（一次性直修，如实记录跳过治理批流程）。修法取候选②并对齐既有单源：securityAxisTriggered 路径环先消费与 content 轴同一份 `ARTIFACT_TEST_EXCLUDE_PATH_RES` 排除闭集（测试/制品面路径不承载生产安全信号——比仅排 `input-brief-*` 稍宽但同族，兼防未来 `input-brief-cost-audit-*` 同型假阳）；生产面 auth 路径命中零改，归因失败回退全量 content 扫描的向严面不减。自检电池 +4 夹具（假阳重放两例、向严保全两例），`--self-check` 73/73 绿、G6 第二方重算 `supervisor-gate-check.mjs --self-check` 10/10 绿；判据 SSOT（dev-loop.md「安全轴触发为高风险」）不变，属投影精度收敛（同 260801/260813 先例）。证据 = 本次落地 diff（dev/260915-m1-k1a-key-layer-dispatch 分支）。
