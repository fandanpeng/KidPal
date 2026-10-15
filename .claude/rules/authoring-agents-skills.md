---
paths:
  - ".claude/agents/**"
  - ".claude/skills/**"
  - ".claude/commands/**"
  - "!.claude/agents/architect.md"
---

# Agent / Skill / Command 的 authoring 纪律（碰 `.claude/{agents,skills,commands}/**` 时自动加载）

> 借鉴 obra/superpowers 的 description 纪律省常驻 token + 防 orchestrator 抄近路。
> **D-C 硬排除**：本 rule **不适用于 `.claude/agents/architect.md`**（自改递归边界；改 architect.md authoring 走独立 human-approved 治理变更，见 [DEV-GOVERNANCE.md](../../docs/DEV-GOVERNANCE.md) §6 自改递归段）。

## 职责分界（引用不复制，避免与既有 rule 重叠）

本 rule 只管 **agent / skill / command 的 `description` / authoring 纪律**。其它面各有 SSOT，本 rule 不复述：
- 文件**位置 / 结构边界** → [repo-structure.md](repo-structure.md)。
- **契约** authoring → [contracts.md](contracts.md)。
- 最小实现 / code-review 纪律 → [.claude/references/minimal-implementation.md](../references/minimal-implementation.md) / [code-review-gate.md](../references/code-review-gate.md)（引用，不搬运正文）。

## description 纪律（常驻 token + 路由信号）

`description` 每会话都载入、且是 orchestrator 的**模型级路由信号**——写坏会又费 token 又诱导 agent 照 description 抄近路、跳过正文流程。因此：

1. **只写「何时用（when-to-use）+ 职责边界 + 路由信号」，不概括 workflow 正文**。流程链（`A → B → C`、「先…再…然后」、`X + Y + Z` 输出清单）留正文，别进 description。
2. **保留路由承重信号**：orchestrator 派谁（子 agent 名 / `tools: Agent(...)` 白名单）、入口命令（`/build-feature`、`/arch-change`、`/gov-change`、`/requirement-change`）、profile 名、调用时机、防误路由消歧句（如 defender「不是契约审批者」、architect-reviewer「准备 ContractChangePacket 交人」）。去 description 时**不得**丢这些——去了会破自动路由。
3. **字数预算**：description 尽量短（气味阈见 lint）；`name`/`tools`/`model` 与 `description` 同 frontmatter 块，改 description **只动 description、逐字段隔离**（误伤 `name` 破 `enforce-scope` 匹配、误伤 `tools: Agent()` 破 dispatch 白名单）。
4. **thin 封装 skill**（`disable-model-invocation: true`，如 `grill-me`/`grill-with-docs`）：description 是**人读可读性**用途，**非模型自动路由信号**——给它们补「触发词提升自动路由」无效（模型本不自动调）。

## 机械守卫（告警级 policy-as-code，非门）

- `node .claude/scripts/lint-descriptions.mjs`：flag 流程叙述气味 + 超长，**告警级（exit 0）**、点名文件；`architect.md` 硬排除、`supervisor` 显式豁免。**不是**逐条人工门（RC-2）。
- `node .claude/scripts/route-regression-check.mjs [--self-check]`：静态断言必留路由信号 + `tools:Agent()` 白名单不退化；`--self-check` 证每条必留信号可证伪（删任一→RED）。改 description 后须复跑仍绿。
- owner=architect（G3）；无 CI/pre-commit，作 landing-time / 手动验证工具（human 2026-07-08 决定 manual-merge 工作流）。
