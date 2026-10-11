# input-brief · builder · 261010-device-reconnect-on-server-close

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-11T01:55:26.383Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: builder
> 来源 TaskPackage: `delivery/taskpackages/261010-device-reconnect-on-server-close/taskpackage.json` · subsystem: firmware · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | `§3` | 3. 架构目标与约束 | 104 | ok |
| `docs/architecture.md` | `§6.4` | 6.4 断线重连与常驻连接 | 321 | ok |
| `docs/architecture.md` | `§8` | 8. 风险·技术债·开放问题 | 346 | ok |
| `docs/architecture.md` | `§10` | 10. 决策记录 | 388 | ok |
| `docs/crq/tech/TECH-261010-device-no-reconnect-on-server-close.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261010-device-reconnect-on-server-close/plan.md` | — | 无锚点（整文件） | — | bare |
| `firmware/main/application.cc` | — | 无锚点（整文件） | — | bare |
| `firmware/main/protocols/websocket_protocol.cc` | — | 无锚点（整文件） | — | bare |

## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威）

`none` —— uiux block 不在场或无在盘设计 refs（`designRefs[]` / `sharedUiRefs[]`）。本包若实有 UI 面，回报 planner 补挂——不脑补。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 单面固件实现变更：server 面零改动、WS wire 形状零变化（重连复用既有 hello 建连序列）；常驻在线改变每会话重开 WS 拓扑 → 复用通道 seam（application.cc:1014-1020 / front ctx_push_edge front_app.py:212）以既有 story ctx / reminder 齿回归覆盖，front 零改

豁免理由留痕，integrator / loop-retro 可事后复核；如复核认定实有对端面，回灌 counterparts 条目并重跑本脚本。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 261001-m4-k3-record-session-live-repair | `firmware/main/test/record_session_repair.acceptance.c` |
| 260924-m4-k2-p3-star-notebook-ui | `firmware/main/test/story_page_v2.acceptance.c` |
| 260924-m4-k3-record-session-live | `firmware/main/test/record_session_live.acceptance.c` · `server/acceptance/test_m4k3_record_session_live.acceptance.py` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

## ⑤ 跨包承接面（上游登记的、**承接包 = 本包**的跨包行为）

`none` —— 两写面（包面 `taskpackage.json` / workstream `decision-coverage.json`）中无「承接包 = 本包」的登记。

`none` 是**结论**、不是省略：若你确信本包该承接某项跨包行为而上游未登记，写进 ④ 超读申报段并回报 planner（那正是本路要消的那类缝）。

