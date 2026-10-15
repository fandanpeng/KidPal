# Architecture Decisions Reference

架构决策记录 why，不重复 contracts/spec 的 what。契约变更仍以 `docs/contracts/**` 为事实源，以 architect-reviewer 和人工契约门为准。

## When to record

- 契约变更被批准或拒绝。
- 系统边界、事件 shape、MCP 工具、shared type、数据所有权发生变化。
- 明确选择 adapter 而不是 contract change。
- 接受残余风险或推迟安全/性能/迁移事项。

## Minimal ADR shape

```md
# ADR: <decision>

## Context

## Decision

## Consequences

## Alternatives Considered

## Validation

## Links
```

TaskPackage 可在 `docs-delta.md` 或 `final-summary.md` 中记录 ADR 摘要；integrator 负责把需要长期保留的决策同步到 canonical docs 或 `delivery/taskpackages/<change-id>/`。