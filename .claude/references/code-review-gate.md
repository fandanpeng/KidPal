# Code Review Gate Reference

integrator 在合并门前产出 Review Packet。它先供 supervisor 自动复核；高风险、critical 或 ESCALATE 时再供人类 review。Review Packet 不替代契约门，也不允许跳过可回滚路径（本地 merge 或 PR）。

## Severity

| Severity | Meaning |
|---|---|
| BLOCKING | 必须修；不修不能进入合并门 |
| SHOULD | 强烈建议本 PR 修；若不修必须写理由 |
| NIT | 小问题，不应阻塞 |
| OPTIONAL | 长期维护建议，可另开后续 |
| FYI | 背景信息，不要求动作 |

### UI 严重度解读（不是新轴）

有 UI 的改动，按用户任务影响解读**既有** BLOCKING/SHOULD/NIT 阶梯——这是解读，不是新增严重度轴、也不是新增闸：

- `BLOCKING`：用户无法完成任务；数据/状态误导；关键流程断裂；基础 a11y 不可用；硬违反 `UIUX-DESIGN-BRIEF` 或 `docs/ui/design.md` 规则。UI-BLOCKING 由**既有 supervisor 合并门**消费（DEV-GOVERNANCE §5 已载对应 NOTE），**不引入第 7 个工程闸、不新增 design/UIUX 闸**。
- `SHOULD`：可用但明显更差——响应式/布局/文案问题降低效率或清晰度。
- `NIT`：纯视觉微调——非关键间距、颜色、动画、小打磨。

## Five axes

1. Correctness: 是否满足每个声明的 `Sx-Ay`，边界条件是否缺失。
2. Test quality: 是否断言真实行为，包括 DB、事件、审计、UI 状态、schema、错误路径。
3. Contract/boundary safety: 是否漂移 `docs/contracts/**`、events、tools、shared types 或系统 spec。
4. Operability/security: ErrorEnvelope、correlationId、权限、输入、secret、日志是否正确。
5. Maintainability/simplicity: 改动是否局部、易懂、没有过早抽象或重复实现。

## Simplicity tags / 过度设计检查（第 5 轴细化）

第 5 轴命中过度设计时，用统一 tag 给出“删什么、换成什么”。默认 severity 落在 SHOULD/NIT/OPTIONAL，不自动 BLOCKING（除非同时违反硬规则）：

| Tag | 命中 | 替换 |
|---|---|---|
| `delete` | 死代码、投机灵活性、没人用的特性 | 删掉，不替换 |
| `stdlib` | 手写了标准库已有的东西 | 点名标准库函数 |
| `native` | 依赖 / 代码在做平台或框架原生能做的事 | 点名原生特性 |
| `yagni` | 单实现的抽象、没人设的 config、单调用方的层 | 内联，等第二个用例再抽 |
| `shrink` | 同逻辑能更短 | 给出更短形式 |

单个 smoke 或 `assert` 自检是最小实现下限，不是冗余，永远不标删除。

**深/浅模组 judgement 透镜（deletion test）**：「删掉这个模块后，复杂度会集中泄漏到调用方，还是直接消失？」复杂度只会转移/扩散 = 深模组信号（留）；复杂度随模块一起消失 = 空壳浅模组信号（内联/删）。deletion test 只是 finding `Basis` 中的判断证据，**不新增 `shallow-module` tag**；命中后仍映射到上表 `delete` / `yagni` / `native` / `shrink` 中最贴切的一个，同一代码问题只产一条 finding。

## Gate evidence 与 Findings 写法（Kind / Basis）

- **不把结构闸证据重新包装成 code-review finding**：tsc/lint/contract-lint 等 gate 的 PASS/FAIL 归 verifier evidence；gate failure 直接阻断，不能进入 integrator 后再被 code review「跳过」或重报。进入 integrator 时即使 gate 全绿，仍须完整执行五轴语义 review——PASS 只证明已配置检查通过，不证明类型/规范/契约/安全领域已被穷尽。advisory、未执行、降级或配置未覆盖的边界照常 review。去掉的是 Review Packet 对 gate 结果的复述，不是任何 review 轴。**此为约定级（convention）约束，与既有五轴纪律同一保全层级，无 G3 verification。**
- **每条 finding 标 Kind = `hard-violation` vs `judgement-call`**：硬违反文档规则/契约边界 = hard-violation；smell / simplicity 提示 = judgement-call。既有 severity 阶梯不变，只加这层标注。
- **每条 finding 写 `Basis`**：hard-violation 必须引稳定规则/spec/contract/`Sx-Ay`；judgement-call 必须引 changed-code hunk + heuristic/tag（如 `Feature Envy` / `yagni`，或 `deletion test` 作为判断证据），不得为没有规范来源的判断伪造 rule citation。
- 五轴与 simplicity tags 全保留、不减、不新增第 6 轴。

## Review Packet

```md
# Review Packet

## Change Size
- Files changed:
- Net diff:
- Scope:
- Split recommendation:

## Verification Evidence
- Structural gates:
- Acceptance/unit/api/ui/e2e:
- Security gate:
- Mutation check:
- Docs sync:

## Simplification Debt
| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
（从 `minimal:` 标记收集；无 upgrade trigger 的标 no-trigger）

## Findings
| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|

## Five-Axis Review

## Residual Risk
- Known limitation:
- Follow-up recommended:
- Human decision needed:
```

任何 BLOCKING finding 都必须让 integrator 停在合并门前，交回 planner、fixer 或人类。supervisor APPROVE 才能继续自动合并路径；ESCALATE 才进入人类合并门。人批后机械路径见 planner.md 合并门段（`--human-approval-ref`）。