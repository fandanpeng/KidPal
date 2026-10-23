---
id: TECH-261022-openclaw-prep-tax-memory-pressure
title: OpenClaw embedded run prep 固定税 ~7s（bootstrap-context ~6.5s）+ 内存压力增长趋势——整理回合靠 front 60s 超时兜住，网关侧改善为长线候选
domain: tech
status: deferred
severity: MAJOR
owner: 主线程（OpenClaw 网关运维面；front 侧容错已由 261021 包承载）
trigger: 整理/引导回合再出现 ClientDisconnectError 超时形态；或 OpenClaw rss 再触内存压力告警；或下个动 OpenClaw 部署/升级的批
opened: 2026-10-22
closed: ""
closedBy: ""
paths: ["server/gateway/front/front_app.py", "docs/debt/tech/_archive/2026/TECH-261021-story-session-cancel-lifecycle.md"]
source: 2026-10-22 真机轮三源日志（front journalctl story_llm_round_unreachable TimeoutError ×2；OpenClaw liveness warning event_loop_delay 38s / rss 600MB 超阈 125%；trace:embedded-run totalMs=7052 其中 bootstrap-context 6538ms）
handoff: 与 OpenClaw 升级 / ECS 规格调整 / 记忆库治理批同批；front 侧容错（60s 超时 + 失败安抚路径）已在 261021-p3-record-cancel-lifecycle 落地
---

**证据（2026-10-22 真机轮，三源）**：

1. **prep 固定税**：`trace:embedded-run` 实测 `totalMs=7052`，分项 = workspace-sandbox 24ms / skills 10ms / core-plugin-tools 267ms / **bootstrap-context 6538ms** / bundle-tools 56ms / system-prompt 41ms / session-resource-loader 24ms / agent-session 78ms / stream-setup 14ms——每个 embedded run（含 front 的整理/引导回合）都要先付这笔，然后模型才开始生成。
2. **环境约束**：ECS 2 核 / 2GB（可用 ~955MB）；Node 网关单进程 rss 600MB（当日超阈值 125% 告警一次）；记忆库 = 共享 openclaw.sqlite 60MB + kidpal agent 库 26MB，bootstrap 期查询；重启后冷缓存放大首回合延迟。
3. **front 侧现状**：整理回合 `_ORGANIZE_LLM_TIMEOUT_S = 60.0`（2160fef7）已把 prep 税 + 生成长尾包住，真机复验整理成功；引导回合 10s 维持（短 prompt 实测可通过）。

**影响面**：整理回合端到端延迟下限 ≈ 7s prep + 生成时间（孩子按收尾后要等 10s+ 才见正文）；内存压力若持续增长会复发 event loop 退化（今天 38s 退化 + 整理超时 ×2 即该形态），front 60s 超时只能兜住不困死、兜不住体验。

**建议改动（候选，交 review 不自批）**：① OpenClaw 侧查 bootstrap-context 6.5s 的构成（memory-core 查询 / 插件装载 / 会话资源恢复），评估会话资源缓存或降载——OpenClaw 升级批内做；② ECS 内存规格评估（2GB 对 600MB rss + 引擎容器共置偏紧）；③ front 侧整理回合失败重试一次（60s 内二次尝试）作为体验兜底——若做属 front 实现面小批；④ 记忆库治理（26MB agent 库增长趋势观察）。

**风险**：riskTier: medium——候选改动全部在网关运维 / front 容错面，不动契约与回合路由；② 有停机窗口成本。
