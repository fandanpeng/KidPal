# ErrorEnvelope Reference

ErrorEnvelope 是 verifier 与 fixer 之间的唯一失败载体。planner 必须原样传递，fixer 每轮只处理一个 `failingStep`。

## Shape

```json
{
  "ok": false,
  "category": "structural | functional | governance | test-quality | security | ai-output",
  "fingerprint": "stable failure key",
  "failingStep": "gate or workflow step",
  "message": "human-readable failure",
  "cmd": "command if any",
  "logTail": "short relevant tail",
  "affectedFiles": ["optional paths"],
  "remediation": "optional next action"
}
```

## Rules

- verifier 输出失败时保留原始 gate、命令和日志尾部。
- planner 不重写、不合并多个 ErrorEnvelope。
- fixer 先写一句根因假设，再打最小补丁。
- 同一 fingerprint 连续失败时，说明当前假设错了；必须换根因方向或升级。
- 流程缺口类失败（缺验收、红基线无效、docs-sync、契约漂移、安全证据缺失）不由 fixer 硬补实现，应交回 planner。

## test-quality 附加字段

category 为 `test-quality` 时，ErrorEnvelope 可附加：`antiPattern`（命中的反模式）、`affectedTest`（`file:line`）、`example`（建议替换的精确断言）。fixer 据此定位浅断言并补真实断言。