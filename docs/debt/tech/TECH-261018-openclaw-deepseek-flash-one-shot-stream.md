---
id: TECH-261018-openclaw-deepseek-flash-one-shot-stream
title: OpenClaw 2026.8.1 下模型 id「deepseek-flash」整段一次性返回（1 个 content delta、首字=总时长），而同一上游的「deepseek-v4-flash」正常渐进流式；本地模型声明 / compat / blockStreamingChunk 均无法修复
domain: tech
status: deferred
severity: COULD
owner: dev 批 / 部署运维
trigger: 下次切换设备模型 id、或升级 OpenClaw 版本时
opened: 2026-10-18
closed: ""
closedBy: ""
paths: []
source: 2026-10-18 ECS 实测——DeepSeek 直连 SSE 对比（两 id）+ OpenClaw loopback 端点分块计数（probe_chunks.py）+ 四次配置试验（reasoning:true / compat 覆盖 / input:text / blockStreamingChunk）
---

**证据（上游 vs OpenClaw 双层实测）**：

- **上游直连**：请求 `deepseek-v4-flash` 与 `deepseek-flash`，响应 `model` 字段**都是 `deepseek-flash`**、`system_fingerprint` 完全相同（同一上游模型）；两者均首包约 0.5s、内容块 18–24 个、约 1s 完成——**上游流式正常**。
- **OpenClaw loopback 端点**（同 prompt、同 agent，只换 model id）：`deepseek-v4-flash` 内容块 28–84、首字 1.5–2.9s；`deepseek-flash` **内容块恒为 1**、首字=总时长 3.7–5.8s。
- **配置试验全部无效**：本地补模型声明 `reasoning: true`、覆盖 `compat`（`thinkingFormat: deepseek` / `supportsReasoningEffort: false`）、把 `input` 改 `text`、设 `blockStreamingChunk`（`breakPreference: sentence`、`minChars: 8`）——仍为 1 块。期间有 1 次 run 意外渐进（首字 2.90s / 总 3.80s），说明是间歇性路径而非确定性配置。

**影响面**：若把设备模型切到 `deepseek-flash` 这个 id，首字从约 2s 变约 5s（语音设备上体感明显），而**模型本身没有任何变化**（同一上游）。另：`deepseek-v4-flash` 已自动路由到最新 flash，切 id 不会换到更强的模型。

**建议改动**：设备侧保持 `deepseek/deepseek-v4-flash` + 思考关（已回滚到该状态）。若将来必须用该 id，先升级 OpenClaw 或向上游报 issue（OpenClaw 社区有症状相似的 reasoning→content 缓冲 issue #95314，报告版本 2026.6.5，本机 2026.8.1 仍可见）。本条不改仓内文件，纯运维知识登记。

**风险**：riskTier: low——不涉及实现改动，仅防后人重复排查。
