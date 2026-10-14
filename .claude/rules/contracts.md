# 契约面纪律（碰 `docs/contracts/**` 时自动加载）

> 本仓契约面 = `docs/contracts/**`（单一事实源：笔顺 JSON 格式 D3 / display 统一响应块 E1 / 课表 MD 结构 B1 / shim API）。自 AInativecompany 迁移时按「单文件 markdown 契约」实况重写；原仓 Prisma 多文件结构的规则不适用。

## 结构红线

- `docs/contracts/**` 对 review-loop / 所有 agent **只读**（review-loop.settings.json permissions.deny + scope-map universal.bashDenyWrite 双拦）。
- 改契约 = **architect-reviewer 四分类预审 + 人类契约门**，任何 loop 内不得自行改。四分类：随 spec 改动携带（同 PR）/ 独立契约变更 / spec↔contract 漂移 / 无需改。
- 契约变更的 gate evidence 骨架见 `.claude/references/contract-change-evidence.md`。
- 影子目录禁令：根级 `contracts/**` 是影子（scope-map universal.denyCreate 拦截新建）；真契约面唯一在 `docs/contracts/**`。

## 本仓实况注记

- 契约已归位为 `docs/contracts/interface-contract.md`（2026-08-27 主线程按 human 指示归位，spec.md 链接同步改指新址）。
- pass-oracle 第 4 关（spec↔contract 机械门）本仓暂无脚本兜底（原仓脚本绑 Prisma 结构未迁），降级为人工核对并如实记录——见 debt 账本条目 `docs/debt/gov/ARQ-260827-contract-mech-gate-gap.md`。
