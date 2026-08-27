# PROPOSAL · 治理体系迁移引导批（档 1）

- **changeId**: 260827-gov-review-migration-bootstrap
- **Status**: LANDED-UNREVIEWED（commit 9d62625，2026-08-27）（主线程按 human 指示「档1 开始往过搬」直接落地；本提案为事后记账，非 frozen-in-advance——原仓 Track B 七步不适用于迁移动作本身，见下「流程豁免说明」）
- **Date**: 2026-08-27

## 背景与目标

把 `c:\AI\AInativecompany` 沉淀的开发治理体系迁入本仓（KidPal / ESP32），按 [GOVERNANCE-MIGRATION-GUIDE.md](../../../docs/templates/GOVERNANCE-MIGRATION-GUIDE.md) 的**档 1 = Spec 治理**裁剪。项目当前处 spec v0.1 草稿期、代码未写，先启用 arch-review 对抗链 + 契约门 + Track B 治理路由，供 spec 冻结前 review 使用。

## 已落地清单

**G2 角色与流程（8 agents + 3 commands + skill + references）**
- `.claude/agents/`：architect / architect-reviewer / review-moderator / scenario-author / challenger / defender / adjudicator / landing-verifier（均按 KidPal 实况适配：契约面 docs/contracts、无 kernel 层并入 systems、dev-loop 角色标注档 2）
- `.claude/commands/`：arch-change / review-arch / review-governance
- `.claude/skills/brainstorming/`
- `.claude/references/`：arch-review-protocol / arch-review-rubric / arch-scenarios / architect-operating-model / architecture-decisions / change-triage / contract-change-evidence / plain-file-handoff / track-b-judge-brief / track-b-verifier-brief
- `.claude/rules/`：contracts.md（ESP32 重写）/ authoring-agents-skills / prompt-authoring

**G3 强制层（3 hooks + 6 scripts + fixtures + configs）**
- `.claude/hooks/`：enforce-scope / deny-main-push / commit-safety（已验证对栈零绑定）
- `.claude/scripts/`：proposal-consolidation-check / landing-fidelity-check / changes-index / lint-descriptions / route-regression-check + lib/governance-gate-shared
- `.claude/scope-map.json`（档 1 净版重写：vendor 面 deny、契约面 bashDenyWrite、8 agent 边界）
- `.claude/settings.json` + `.claude/review-loop.settings.json`（新写）
- `.claude/route-fixtures/routing.json` + `surface-alignment.json`（档 1 净版）

**G1 指令根与治理文档**
- `AGENTS.md` / `CLAUDE.md`（新写：实现面待定登记条款、vendor 只读、CodebaseMemory 降级条款、Track B 路由）
- `docs/DEV-GOVERNANCE.md`（适配：契约面改 docs/contracts、dev-loop 角色标【档 2 未迁】、KidPal 定位注记）
- `docs/ARCH-REVIEW-QUEUE.md`（净版收件箱 + 首个 ARQ 登记）
- `docs/templates/`：PROPOSAL / retro / system-spec / kernel-spec / AGENTS / architect-landing-summary 模板 + 迁移指南

## 流程豁免说明

本次迁移本身是**元级动作**（建立治理机器的动作先于机器存在），无法走「主线程 PROPOSAL → architect GREEN → FROZEN → landing」的既有七步——七步闭环的前提是流程已在本仓生效。human 以「档1 开始往过搬」直接授权本批落地。落地后的首次真实 governance/spec 变更恢复走 Track B 常规路径。诚实记录：本批未经对抗 review，升档 2 或首批 /arch-change 时建议 landing-verifier 事后抽核。

## 冒烟验证证据

- 全部 hooks/scripts `node --check` 通过
- scope-map / settings×2 JSON 合法
- `route-regression-check --self-check`：56 ok, 0 fail
- `lint-descriptions`：7 扫描 1 标警（landing-verifier 描述超长 349>320，告警级非阻塞；与原仓一致水平）
- `changes-index.mjs` 无参重建 ok（0 dev / 0 arch）；upsert 缺目录时正确报错拒绝

## human 取舍点

1. 本提案 Status 记 LANDED-UNREVIEWED 是否接受？或要求补一轮对抗 review（转 /review-governance）？
2. 首次真实使用 = ARCH-REVIEW-QUEUE 已登记的 ARQ-260827-tier1-boot-review（spec.md + interface-contract 冻结前对抗 review），何时启动由 human 定。
