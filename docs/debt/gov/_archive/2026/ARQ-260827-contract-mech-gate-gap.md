---
id: ARQ-260827-contract-mech-gate-gap
title: pass-oracle 第 4 关（spec↔contract 机械门）暂无脚本兜底
domain: gov
status: closed
severity: SHOULD
owner: ""
trigger: 契约面扩到 ≥2 文件 / 或出现首次 spec↔contract 漂移实例
opened: 2026-08-27
closed: 2026-10-27
closedBy: human 直接指示（主线程代记，261027 P7.5 会话）
paths: []
source: 治理迁移决策记录（2026-08-27，MIGRATION-NOTES §已知缺口）
---

原仓 `arch-spec-contract-check.mjs` 深绑多文件 Prisma contracts 结构，不适用单文件 MD 契约面，未迁。pass-oracle 第 4 关在本仓降级为人工核对，须如实记录「机械门缺失」，不得虚报已机检。将来契约面复杂化（出现第二份契约文件或字段级 drift 高发）时写一个针对 markdown 契约表的一致性检查脚本补位。

**结项追注（2026-10-27，human 直接指示，主线程代记）**：human 裁本条**已过时/被消化**，结项。结项时工作树曾出现本文件裸删（无归位痕迹），human 确认删除意图后按正规记账通道补记：还原文件 → status 翻 closed → 本追注 append → 渲染归位。`.claude/rules/contracts.md` 的路径指针已随归档位同步刷新；`contract-lint.mjs` / `verify-gates.json` / M4K1 验收注记为 ID 级引用、归档后仍有效，不动。
