# input-brief · 261023-agents-md-static-semantics-alignment

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-23T09:48:27.810Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> 来源 TaskPackage: `delivery/taskpackages/261023-agents-md-static-semantics-alignment/taskpackage.json` · subsystem: server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md（kidpal agent 静态语义单一事实源）+ server/deploy/openclaw-skills/workspace-kidpal/kidpal-schedule-answer/SKILL.md（B1 同口径）+ server/acceptance（新验收件） · rigor: lite

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `delivery/review/261022-arch-review-device-memory-prompt-overhaul/agents-md-advisory-20261023.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `docs/debt/tech/TECH-261023-openclaw-tool-face-prompt-only-readonly.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261023-agents-md-static-semantics-alignment/plan.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | `§5.2` | 5.2 运行视图（关键场景端到端） | 211 | ok |
| `docs/requirements.md` | `S-F2.2` | S-F2.2 · 对话式引导记录（双入口，v1.65 改名，原「开录（双入口）」） | 454 | ok |
| `docs/requirements.md` | `S-F1.3` | S-F1.3 · 语音问课表 | 278 | ok |
| `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md` | — | 无锚点（整文件） | — | bare |
| `server/deploy/openclaw-skills/workspace-kidpal/kidpal-schedule-answer/SKILL.md` | — | 无锚点（整文件） | — | bare |

## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）

`none` —— uiux block 不在场或无在盘设计 refs（`designRefs[]` / `sharedUiRefs[]`）。本包若实有 UI 面，回报 planner 补挂——不脑补。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 纯云端 prompt 文本面：零 wire 形状改动、零帧类型/字段/序列化触碰（display E1 / B1 课表 MD / §三提醒 / §四私有消息帧零触）；设备端零感知、固件零 diff。triage architect 逃生门预查 = 全包 spec-clean 唯 S3 疑似（甲案：architect-reviewer 四分类前置，human 2026-10-23 裁）。

豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

（无同 subsystem 的已 merged 先例包）

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

`none` —— 两写面（包面 `taskpackage.json` / workstream `decision-coverage.json`）中无「承接包 = 本包」的登记。

`none` 是**结论**、不是省略：若你确信本包该承接某项跨包行为而上游未登记，写进 ④ 超读申报段并回报 planner（那正是本路要消的那类缝）。

