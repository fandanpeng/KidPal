# input-brief · 261022-m5-k1-prompt-carrier-migration

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-23T03:43:37.433Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> 来源 TaskPackage: `delivery/taskpackages/261022-m5-k1-prompt-carrier-migration/taskpackage.json` · subsystem: server/deploy（prompt 静态/动态承载反转 + kidpal agent 工具面收窄；OpenClaw workspace AGENTS.md / carrier / provider.config / overlay / manifest / verify-deploy / openclaw.json 声明式 patch） · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§1.6` | 1.6 对 OpenClaw 的要求（system prompt 约束） | 189 | ok |
| `delivery/workstreams/device-memory-overhaul/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/workstreams/device-memory-overhaul/task_plan.md` | — | 无锚点（整文件） | — | bare |
| `delivery/review/261022-arch-review-device-memory-prompt-overhaul/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261022-m5-k1-prompt-carrier-migration/plan.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | — | 无锚点（整文件） | — | bare |
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/debt/tech/TECH-261022-device-memory-prompt-overhaul.md` | — | 无锚点（整文件） | — | bare |
| `TECH-261022-device-memory-prompt-overhaul` | — | （文件不存在） | — | missing |
| `delivery/workstreams/device-memory-overhaul/PROPOSAL.md` | `§2` | 2. 现在在哪（混合型：并列三节） | 31 | ok |
| `delivery/workstreams/device-memory-overhaul/PROPOSAL.md` | `§3.1` | 3.1 方案空间与裁定 | 53 | ok |
| `delivery/review/261022-arch-review-device-memory-prompt-overhaul/PROPOSAL.md` | `T3` | T3 · prompt 承载反转：AGENTS.md 静态语义单一事实源 + 动态 context + ADR-001 不变式重定义 | 225 | ok |
| `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md` | — | 无锚点（整文件） | — | bare |
| `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` | — | 无锚点（整文件） | — | bare |
| `server/deploy/provider.config.yaml` | — | 无锚点（整文件） | — | bare |
| `server/deploy/apply-xiaozhi-overlay.sh` | — | 无锚点（整文件） | — | bare |
| `server/deploy/manifest.yaml` | — | 无锚点（整文件） | — | bare |

## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）

`none` —— uiux block 不在场或无在盘设计 refs（`designRefs[]` / `sharedUiRefs[]`）。本包若实有 UI 面，回报 planner 补挂——不脑补。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 本包纯云端部署声明面与引擎 prompt 装配面改动：设备端零感知、不新增或改变任何端→网关消息、帧类型、字段与序列化形态（display E1 / B1 / §三提醒 / §四私有消息帧零触碰），固件零 diff；crossEnd 对端面不存在。

豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

（无同 subsystem 的已 merged 先例包）

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

| 来源 | 行为 | 语义 owner | 承接包（`carriedBy ?? owner`） | note |
|---|---|---|---|---|
| `workstream:device-memory-overhaul` | manifest .config.yaml marker 锚集（撤 v11 静态锚、增动态块锚） | `261022-m5-k1-prompt-carrier-migration` | `261022-m5-k1-prompt-carrier-migration` | P1 同批换锚（否则 verify-deploy 必红）；P3 若经 overlay 增 Honcho 配置，只能在 P1 新锚集上追加，不得回退。 |
| `workstream:device-memory-overhaul` | 冻结验收语义锚升级（三面接线 / v11 语义 / 声明式 patch 守卫） | `261022-m5-k1-prompt-carrier-migration` | `261022-m5-k1-prompt-carrier-migration` | test_prompt_v7_round_routing / test_261010 / test_261017 由 P1 经 acceptance-author 留痕 supersede；P2/P3/P5 的 impactedTests 只做追加，不静默改写既有冻结断言。 |

上述条目 = **本包承接的跨包行为**（上游起包期 / 拆包期登记）。验收面须覆盖它们，或在本包 review-packet 的「未覆盖面申报」段写明去向（`dev-loop.md`「### 合并门 Review Packet」）。

