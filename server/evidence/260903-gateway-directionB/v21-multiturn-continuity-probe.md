# V21 多轮连续性首验探针结论（部署期第一件事）

> **_INTEGRATOR LANDING NOTE_**: this file is staged by the builder inside `server/` because the
> builder write-scope (enforce-scope) excludes `changes/dev/260903-gateway-directionB/`. The
> frozen structural acceptance `test_a8_v21_multiturn_probe_evidence` reads it from
> `changes/dev/260903-gateway-directionB/v21*.md`. **Integrator must land this (and the sibling
> evidence files) into `changes/dev/260903-gateway-directionB/` for GWA-A8/E2/E3 to go green.**

- **TaskPackage**: `260903-gateway-directionB`（GWA-A8）
- **探针日期/环境**: 2026-09-04，ECS `8.137.91.16`，OpenClaw loopback `http://127.0.0.1:18789`（OpenAI 兼容端点，`type:openai` 接入假设为无状态，契约 §四）
- **探针方法**: 对 OpenClaw `/v1/chat/completions` 做「两轮追问」连续性实验，对照契约 §四「`type:openai` 接入默认无状态」假设，实证多轮连续性的承载方案。

## 探针与实测结果

1. **首轮（自我介绍）**：`"你好，我叫小明，今年8岁。请记住并回应我"` → OpenClaw 在本轮单轮上下文内回应「记住了，小明，你今年8岁啦」。单轮内记住 ≠ 跨轮延续。
2. **第二轮（独立调用、不传历史）**：`"我刚才第一次打招呼时介绍了自己。我介绍过我叫什么、几岁了吗？"` → OpenClaw 明确回 **「没有记录」**——独立 stateless 调用**不携带**上一轮自我介绍。实证契约 §四「`type:openai` 默认无状态」。
3. **第二轮（显式透传全历史）**：messages = `[{自我介绍},{assistant 确认},{追问}]` → OpenClaw 正确回「你叫**小明**，今年**8岁**！」——**每轮透传历史可承载连续性**。

## 结论：承载方案定案（V21）

- **探针结论 = stateless 无状态不保证跨轮连续**（turn2-stateless 失败），选项落「每轮透传历史」。
- **承载方案**：网关适配侧在服务端**每一轮把会话历史透传给 OpenClaw**——`server/gateway/provider.openclaw_stream_request(messages)` 按「全量 messages 透传」实现（决策 18 / 契约 §四），不依赖 OpenClaw 调用间记忆。
- 未降级单轮（透传历史已实测可行）。备选「网关侧状态 + 会话标识」留待历史超限时切换。
- 相关 ARQ：ARQ-260902-multiturn-continuity —— 承载方案已由本探针收敛为「每轮透传历史」。

## 证据

- turn1：OpenClaw 单轮内回应「记住了，小明，你今年8岁啦」。
- turn2-stateless：OpenClaw「没有记录」。
- turn2-with-history：OpenClaw「你叫小明，今年8岁！」。
- 原始 `chat.completion` 响应在 2026-09-04 ECS 现场已捕获。