---
paths:
  - ".claude/agents/**"
  - ".claude/skills/**"
  - ".claude/commands/**"
  - ".claude/references/**"
  - "!.claude/agents/architect.md"
---

# Prompt / agent-def 正文撰写卫生（碰 `.claude/{agents,skills,commands,references}/**` 时自动加载）

> 对标 Matt Pocock `writing-great-skills` 三原则的一次性吸收与本地化（gov-review-devloop-pocock-adoption C3，FROZEN 2026-07-25）；获批后本仓 canonical 独立演进，不跟随上游。
> **硬排除**：本 rule **不适用于 `.claude/agents/architect.md`**（自改递归边界；architect self-def 改动走独立 human-approved 治理变更，见 [DEV-GOVERNANCE.md](../../docs/DEV-GOVERNANCE.md) §6 自改递归段）。review-loop agent 定义仍受第五阀约束。本 rule 只改变 authoring 指引，不改变这些治理边界。

## 职责分界（引用不复制）

本 rule 是 agent / skill / command / reference **正文与其中 dispatch prompt** 的通用撰写 SSOT，只约束撰写风格/密度，不改任何角色职责、控制流或闸门，不与 CLAUDE.md / AGENTS.md 的 G1 常驻指令**内容**冲突（那是内容不是风格）。其它面各有 SSOT，本 rule 不复述：

- **description / frontmatter authoring**（路由信号、字数预算、lint 守卫）→ [authoring-agents-skills.md](authoring-agents-skills.md)。**description 撰写在本 rule 范围之外（out-of-scope）**；两个 rule 在 agent/skill/command 路径可同时加载，各管一面。
- **TaskPackage `acceptance` / `exitCriteria` 的工程语义**（A10 约定）→ [task-intake-and-slicing.md](../references/task-intake-and-slicing.md)（只引用，不复制）。

## 三原则

### 1. Pruning（修剪）

删所有废话与 AI 已知的指令；每句须有实际作用——多写一句废话 = AI 多一分分心。

适用例：删「请认真、仔细地」「记住要」类无操作语义的副词与客套；同一约束只说一次；长解释先看能否换成 Leading Words。

**Pruning 硬边界（HQ-1，human 裁定 2026-07-24）**：权限边界句、角色约束句、authority/owner/writer/approver 声明、scope allow/deny、工程门/人工门、升级条件、must/never 级约束、契约/合并/验收铁律**不属于 prunable 内容**，即便其语义 AI 已知。判断标准：删掉这句话是否会改变谁能做什么、何时必须停、哪个门负责裁断、或哪条行为被禁止；若会，必须保留。可以压缩表达，但不能删除语义。

本硬边界是覆盖面内权限类 reference（如 `.claude/references/architect-operating-model.md`）的保全机制：paths 不做 glob 扩大排除（HQ-1 方案 A），由本正文声明硬保全。

### 2. Leading Words（引导词）

用经典专业术语替代长解释——术语深植训练数据，一词胜百字。

适用例：写「Feature Envy」「Data Clumps」「深/浅模组」「tracer bullet」即达意，不必各配一段定义；`code-review-gate.md` 的 deletion test 透镜是此写法实例。

### 3. Completion Criteria（完成标准）

给明确终点、防无限发散：每个步骤写出可检查终点——什么制品、落在哪个路径、由谁消费/检查。

适用例：「TaskPackage 立即写入 `delivery/taskpackages/<change-id>/taskpackage.json`，不能只留在聊天里」是可检查终点；「完成拆包」「准备好文档」不是。TaskPackage 级 `acceptance` / `exitCriteria` 的写法与工程语义归 `task-intake-and-slicing.md`，本 rule 不重复。
