# Observability K6 Reference

开发期 loop 的日志与错误形态要与未来 K6 对齐，但不依赖 K6 运行时。

## Required practice

- 所有跨步骤日志使用结构化字段，避免只写自然语言。
- `correlationId` 必须贯穿 API、事件、审计、AI run 和 verifier 输出。
- 错误用 ErrorEnvelope 形态表达，包含 category、fingerprint、failingStep、message、logTail。
- 外部调用、AI 工具调用、成本/审计记录必须可关联到同一 correlationId。
- 日志不得输出 secret、token、password、完整凭证或敏感 PII。

## Evidence examples

- 测试断言事件 payload 含 `correlationId`。
- 审计/成本记录能按 `correlationId` 查询。
- verifier 失败摘要含稳定 fingerprint，便于 fixer 去重。