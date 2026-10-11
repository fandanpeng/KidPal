# input-brief · builder · 261010-prompt-v9-schedule-anti-narration

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-11T00:52:10.251Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: builder
> 来源 TaskPackage: `delivery/taskpackages/261010-prompt-v9-schedule-anti-narration/taskpackage.json` · subsystem: server/deploy · rigor: lite

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | `§5.5.5` | 5.5.5 OpenClaw system prompt 版本化 | 295 | ok |
| `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` | — | 无锚点（整文件） | — | bare |
| `docs/crq/tech/TECH-261010-schedule-round-narration-leak.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261010-prompt-v9-schedule-anti-narration/plan.md` | — | 无锚点（整文件） | — | bare |
| `server/deploy/openclaw-system-prompt.v8.md` | — | 无锚点（整文件） | — | bare |
| `server/deploy/provider.config.yaml` | — | 无锚点（整文件） | — | bare |
| `server/deploy/apply-xiaozhi-overlay.sh` | — | 无锚点（整文件） | — | bare |

## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威）

`none` —— uiux block 不在场或无在盘设计 refs（`designRefs[]` / `sharedUiRefs[]`）。本包若实有 UI 面，回报 planner 补挂——不脑补。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 单面实现变更（server/deploy prompt 族 + 验收 + arch 指针 docs-sync）：不改 wire、不触固件、不触契约与需求面；部署面 = ECS overlay 重跑（261010 决策 21 键同款先例，上传前核远端 == 基线）

豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 261001-openclaw-model-name-overlay-drift | `server/acceptance/test_261001_openclaw_model_name_overlay_drift.acceptance.py` · `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

`none` —— 两写面（包面 `taskpackage.json` / workstream `decision-coverage.json`）中无「承接包 = 本包」的登记。

`none` 是**结论**、不是省略：若你确信本包该承接某项跨包行为而上游未登记，写进 ④ 超读申报段并回报 planner（那正是本路要消的那类缝）。

