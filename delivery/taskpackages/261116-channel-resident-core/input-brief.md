# input-brief · 261116-channel-resident-core

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-11-16T14:04:36.402Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> 来源 TaskPackage: `delivery/taskpackages/261116-channel-resident-core/taskpackage.json` · subsystem: firmware 语音会话生命周期与通道保活（application.cc / protocols）+ front 心跳与每会话收尾（front_app.py） · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§四-listen` | （未定位到标题节） | — | not-found |
| `docs/contracts/interface-contract.md` | `§四-kidpal_keepalive` | （未定位到标题节） | — | not-found |
| `docs/contracts/interface-contract.md` | `§四-kidpal_story_discard` | （未定位到标题节） | — | not-found |
| `docs/contracts/interface-contract.md` | `§四-kidpal_session_ctx` | （未定位到标题节） | — | not-found |
| `delivery/workstreams/261115-session-channel-resident/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/review/261115-arch-review-session-channel-resident/package-split.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | — | 无锚点（整文件） | — | bare |
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/decisions/ADR.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | `§2.1` | 2.1 语音交互模型（已定：按一下 KEY / 喊唤醒词启动会话；KEY 打断停播） | 31 | ok |
| `docs/requirements.md` | `§2.3` | 2.3 按键手势（3 键，无触摸；本节为权威键位定义） | 131 | ok |
| `docs/requirements.md` | `S-F1.4` | S-F1.4 · 日程提醒 | 299 | ok |
| `docs/requirements.md` | `S-F2.2-AC4` | （未定位到标题节） | — | not-found |
| `docs/architecture.md` | `§6.4` | 6.4 断线重连与常驻连接 | 373 | ok |
| `docs/architecture.md` | `§5.1.1` | 5.1.1 设备端组件（firmware/main/） | 171 | ok |
| `docs/architecture.md` | `§5.1.2` | 5.1.2 服务端组件（server/） | 194 | ok |
| `docs/architecture.md` | `§5.2` | 5.2 运行视图（关键场景端到端） | 219 | ok |
| `docs/decisions/ADR.md` | `ADR-019` | ADR-019 · 语音会话生命周期与 WS 通道生命周期解耦：通道常驻 + 带内会话边界（listen start/stop）+ front 心跳保活 + 封顶长跑重连 | 161 | ok |
| `delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md` | `T1` | T1 · 架构 §6.4 整节重写：通道生命周期与会话生命周期解耦 + 常驻保活 + 自愈巡检 + 长重试 | 79 | ok |
| `delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md` | `T2` | T2 · 架构 §5.1.1 / §5.1.2 / §5.2：统一会话结束处理器 + front 每会话态改由带内信号驱动 + 结束三通道接线 | 105 | ok |
| `delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md` | `T3` | T3 · 架构 §8.1 风险表 + §5.1.1 会话状态机行 ⚠️ 对账 + §3.3 假设清单行 + §10 决策 29 取代注记：账实同步（四处引用「常驻重建」全数对账） | 139 | ok |
| `delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md` | `T4` | T4 · 契约 §四：新增 `listen` 帧节（会话边界带内信号）+ 新增下行 `kidpal_keepalive` 心跳帧 + `kidpal_story_discard` 发送时机措辞改 + 变更记录条 | 159 | ok |
| `docs/debt/tech/TECH-261114-overlay-close-channel-rebuild-churn.md` | — | 无锚点（整文件） | — | bare |
| `docs/debt/tech/TECH-261012-first-keypress-no-listening.md` | — | 无锚点（整文件） | — | bare |

## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）

`none` —— uiux block 不在场或无在盘设计 refs（`designRefs[]` / `sharedUiRefs[]`）。本包若实有 UI 面，回报 planner 补挂——不脑补。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `server/gateway/front/front_app.py` | `ws_gateway` | :2446-2462 | — |
| `server/gateway/front/front_app.py` | `pump_client_to_upstream` | :1782-1784 | 2505 |
| `server/gateway/front/front_app.py` | `_consume_kidpal_control` | :992-1045 | 1817 |
| `firmware/main/protocols/protocol.cc` | `Protocol::SendStopListening` | :88-91 | — |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

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

## ⑥ 非阻断 advisory 明细（终端只打汇总；此处保留定位）

- **warning · anchor-not-found** · ref=`docs/contracts/interface-contract.md#§四-listen`：ref 的显式锚点未定位到标题节：docs/contracts/interface-contract.md#§四-listen（起包期核对锚点或目标文档是否已变更）
- **warning · anchor-not-found** · ref=`docs/contracts/interface-contract.md#§四-kidpal_keepalive`：ref 的显式锚点未定位到标题节：docs/contracts/interface-contract.md#§四-kidpal_keepalive（起包期核对锚点或目标文档是否已变更）
- **warning · anchor-not-found** · ref=`docs/contracts/interface-contract.md#§四-kidpal_story_discard`：ref 的显式锚点未定位到标题节：docs/contracts/interface-contract.md#§四-kidpal_story_discard（起包期核对锚点或目标文档是否已变更）
- **warning · anchor-not-found** · ref=`docs/contracts/interface-contract.md#§四-kidpal_session_ctx`：ref 的显式锚点未定位到标题节：docs/contracts/interface-contract.md#§四-kidpal_session_ctx（起包期核对锚点或目标文档是否已变更）
- **info · whole-document** · ref=`delivery/workstreams/261115-session-channel-resident/PROPOSAL.md`：Markdown 整文件引用（不自动猜章节）：delivery/workstreams/261115-session-channel-resident/PROPOSAL.md
- **info · whole-document** · ref=`delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md`：Markdown 整文件引用（不自动猜章节）：delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md
- **info · whole-document** · ref=`delivery/review/261115-arch-review-session-channel-resident/package-split.md`：Markdown 整文件引用（不自动猜章节）：delivery/review/261115-arch-review-session-channel-resident/package-split.md
- **info · whole-document** · ref=`docs/requirements.md`：Markdown 整文件引用（不自动猜章节）：docs/requirements.md
- **info · whole-document** · ref=`docs/architecture.md`：Markdown 整文件引用（不自动猜章节）：docs/architecture.md
- **info · whole-document** · ref=`docs/contracts/interface-contract.md`：Markdown 整文件引用（不自动猜章节）：docs/contracts/interface-contract.md
- **info · whole-document** · ref=`docs/decisions/ADR.md`：Markdown 整文件引用（不自动猜章节）：docs/decisions/ADR.md
- **warning · anchor-not-found** · ref=`docs/requirements.md#S-F2.2-AC4`：ref 的显式锚点未定位到标题节：docs/requirements.md#S-F2.2-AC4（起包期核对锚点或目标文档是否已变更）
- **info · whole-document** · ref=`docs/debt/tech/TECH-261114-overlay-close-channel-rebuild-churn.md`：Markdown 整文件引用（不自动猜章节）：docs/debt/tech/TECH-261114-overlay-close-channel-rebuild-churn.md
- **info · whole-document** · ref=`docs/debt/tech/TECH-261012-first-keypress-no-listening.md`：Markdown 整文件引用（不自动猜章节）：docs/debt/tech/TECH-261012-first-keypress-no-listening.md

