---
id: ARQ-260907-taskpackage-format-expectations-frontload
title: taskpackage/验收的机械闸格式期望没有前置对齐，多个 Pod 靠闸红反推再回派/补字段，同型红在 ≥3 包复现（跨 4 包 5 子项）
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-07
closed: 2026-09-07
closedBy: ""
paths: []
source: ""
---

DEV-GOVERNANCE 新节「## 起包格式期望前置」五条清单（mutationEvidence 骨架 / 断言格式 / 验收 ID 禁括号后缀 / 降级声明前置 / 新约定包闸判据自查）+ 四件 agent def 引用行（planner/portfolio-planner/builder/acceptance-author，合计 +6，OQ2=(a) human 已批）。 证据：[260907-gov-review-arq-governance-batch-2](../changes/arch/260907-gov-review-arq-governance-batch-2/)。**复现（loop-retro-2026-09-13，已结项未被行为闭合）**：260911 批三包仍同型红——display-first-m1 verifier 三趟「补一行 JSON 再重验」（DF-A3~A6 未标 structural / mutationEvidence acceptanceId 缩写失配 / docsSyncRequired=false 缺 reason，retro 原话「四轮能压成一轮」）、render-pages 三轮 scoped 红全为 marker/字段类（coverage map 缺 5 id、mutationSkipReason 缺字段、docs/notes 缺锚点）、screen-unification 元数据面反复卡包（验收条目无 kind 标记 / mutation 证据空 / docs 同步理由缺）。并暴露五条清单未覆盖的新子项：① hasCoverageMarker 只认 `coverage: <id>` 前缀形态而 HOST_TEST_CONVENTIONS 未钉机器可识别形态（key-route-a1 r1 红一轮实证）；② structural 豁免须对象条目 kind:'structural' 编码、验收作者交付结论未接闸契约（display-first-m1 实证）；③ docs/notes 锚点 append 通道「每次都靠人肉记住」（render-pages 第三次应验）。按 GC1 记复现不新立，新子项随本条后审一并处置。
