# input-brief · acceptance-author · 260923-m3-k6-boot-auto-connect

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-22T16:28:54.183Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> 来源 TaskPackage: `delivery/taskpackages/260923-m3-k6-boot-auto-connect/taskpackage.json` · subsystem: firmware · rigor: lite

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | `S-F1.1` | S-F1.1 · 常显首页看「接下来干嘛」 | 224 | ok |
| `firmware/main/application.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/application.h` | — | 无锚点（整文件） | — | bare |
| `firmware/main/protocols/websocket_protocol.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc` | — | 无锚点（整文件） | — | bare |
| `server/gateway/front/front_app.py` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/260923-m3-k6-boot-auto-connect/plan.md` | — | 无锚点（整文件） | — | bare |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 本包复用既有 hello 握手零新消息形状，server/front 零改动；建连时机属行为策略非 wire 形状（plan.md 契约核查节 architect CONFIRMED）

豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260922-m3-k5-sntp-time-sync | `firmware/main/test/time_sync.acceptance.c` |
| 260921-m3-k2-p2-week-grid-page | `firmware/main/test/schedule_grid.acceptance.c` |
| 260921-m3-k4-p1-cjk-dynamic-text | `firmware/main/test/p1_cjk_name.acceptance.c` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

`none` —— 两写面（包面 `taskpackage.json` / workstream `decision-coverage.json`）中无「承接包 = 本包」的登记。

`none` 是**结论**、不是省略：若你确信本包该承接某项跨包行为而上游未登记，写进 ④ 超读申报段并回报 planner（那正是本路要消的那类缝）。

