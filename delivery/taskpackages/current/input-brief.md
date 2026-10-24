# input-brief · 261024-oneshot-user-uniformity

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-24T00:59:17.163Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> 来源 TaskPackage: `delivery/taskpackages/261024-oneshot-user-uniformity/taskpackage.json` · subsystem: server/gateway/front（一次性回合调用链 _openclaw_chat_once / _organize_and_archive / _story_guide_round / _story_finish_session）+ server/gateway/provider.py（payload 单源）+ server/deploy/261022-openclaw-honcho-plugin/honcho-plugin-declaration.json（声明工件换代）+ server/acceptance（锚换代件）+ server/tests（桩签名同步） · rigor: lite

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261023-honcho-memory-wiring-restoration/plan.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | `§6.7` | 6.7 记忆写入范围与画像 | 356 | ok |
| `server/acceptance/test_261022_memory_isolation.acceptance.py` | — | 无锚点（整文件） | — | bare |
| `server/gateway/front/front_app.py` | — | 无锚点（整文件） | — | bare |
| `server/gateway/provider.py` | — | 无锚点（整文件） | — | bare |
| `server/deploy/261022-openclaw-honcho-plugin/honcho-plugin-declaration.json` | — | 无锚点（整文件） | — | bare |

## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）

`none` —— uiux block 不在场或无在盘设计 refs（`designRefs[]` / `sharedUiRefs[]`）。本包若实有 UI 面，回报 planner 补挂——不脑补。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 纯 server 云内改动：front→OpenClaw loopback HTTP payload 的 user 键是云端运行态请求字段，非跨端 wire（帧类型/字段类型与承载/回执语义/序列化形态零触）；设备端零感知、固件零 diff、WS device-id 头本就存在（engine websocket_server 既有要求），本包不改任何设备面协议。

豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 261023-honcho-memory-wiring-restoration | `server/acceptance/test_261022_memory_isolation.acceptance.py` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

`none` —— 两写面（包面 `taskpackage.json` / workstream `decision-coverage.json`）中无「承接包 = 本包」的登记。

`none` 是**结论**、不是省略：若你确信本包该承接某项跨包行为而上游未登记，写进 ④ 超读申报段并回报 planner（那正是本路要消的那类缝）。

