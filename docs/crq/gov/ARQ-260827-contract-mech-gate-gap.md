---
id: ARQ-260827-contract-mech-gate-gap
title: pass-oracle 第 4 关（spec↔contract 机械门）暂无脚本兜底
domain: gov
status: accepted-risk
severity: SHOULD
owner: ""
trigger: 契约面扩到 ≥2 文件 / 或出现首次 spec↔contract 漂移实例
opened: 2026-08-27
closed: ""
closedBy: ""
paths: []
source: 治理迁移决策记录（2026-08-27，MIGRATION-NOTES §已知缺口）
---

原仓 `arch-spec-contract-check.mjs` 深绑多文件 Prisma contracts 结构，不适用单文件 MD 契约面，未迁。pass-oracle 第 4 关在本仓降级为人工核对，须如实记录「机械门缺失」，不得虚报已机检。将来契约面复杂化（出现第二份契约文件或字段级 drift 高发）时写一个针对 markdown 契约表的一致性检查脚本补位。
