# input-brief · acceptance-author · 260928-story-connect-push-defer

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-28T04:06:19.037Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author
> 来源 TaskPackage: `delivery/taskpackages/260928-story-connect-push-defer/taskpackage.json` · subsystem: server · rigor: lite

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/architecture.md` | `§5.2` | 5.2 运行视图（关键场景端到端） | 203 | ok |
| `server/gateway/front/front_app.py` | — | 无锚点（整文件） | — | bare |
| `server/acceptance/test_m4k1_story_cloud_link.acceptance.py` | — | 无锚点（整文件） | — | bare |
| `server/acceptance/M4K1-ACCEPTANCE-NOTES.md` | — | 无锚点（整文件） | — | bare |
| `delivery/changes/dev/260924-m4-k1-story-cloud-link/docs-delta.md` | — | 无锚点（整文件） | — | bare |
| `delivery/workstreams/m4-star-notebook/progress.md` | — | 无锚点（整文件） | — | bare |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 仅移除建连时推送调用点一处、零 wire 形状变化；对端固件零触碰（K2/K3 未开跑），story_page 侧消费语义不变

豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260924-m4-k1-story-cloud-link | `server/acceptance/test_m4k1_story_cloud_link.acceptance.py` |
| 260924-perf-k1-roundtrip-latency | `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py` |
| 260923-m3-k7-schedule-grid-restore | `server/acceptance/test_schedule_grid_restore.acceptance.py` · `firmware/main/test/schedule_grid_restore.acceptance.c` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

`none` —— 两写面（包面 `taskpackage.json` / workstream `decision-coverage.json`）中无「承接包 = 本包」的登记。

`none` 是**结论**、不是省略：若你确信本包该承接某项跨包行为而上游未登记，写进 ④ 超读申报段并回报 planner（那正是本路要消的那类缝）。

