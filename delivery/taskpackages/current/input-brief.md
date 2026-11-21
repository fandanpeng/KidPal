# input-brief · 261117-session-end-intent-longretry

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-11-19T14:17:15.098Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> 来源 TaskPackage: `delivery/taskpackages/261117-session-end-intent-longretry/taskpackage.json` · subsystem: firmware control end_session 消费与自动重连策略 + front display 门禁与提示词承载 · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§1.4` | 1.4 控制类指令（OpenClaw 判意图或 front 检出后下发） | 231 | ok |
| `docs/contracts/interface-contract.md` | `§1.5` | 1.5 网关的职责（薄，零自研业务逻辑红线不变） | 252 | ok |
| `docs/contracts/interface-contract.md` | `§1.6` | 1.6 对 OpenClaw 的要求（system prompt 约束） | 262 | ok |
| `delivery/workstreams/261115-session-channel-resident/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/review/261115-arch-review-session-channel-resident/package-split.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | — | 无锚点（整文件） | — | bare |
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/decisions/ADR.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | `§2.1` | 2.1 语音交互模型（已定：按一下 KEY / 喊唤醒词启动会话；KEY 打断停播） | 31 | ok |
| `docs/requirements.md` | `S-F1.4` | S-F1.4 · 日程提醒 | 299 | ok |
| `docs/architecture.md` | `§5.2` | 5.2 运行视图（关键场景端到端） | 219 | ok |
| `docs/architecture.md` | `§6.4` | 6.4 断线重连与常驻连接 | 373 | ok |
| `docs/decisions/ADR.md` | `ADR-019` | ADR-019 · 语音会话生命周期与 WS 通道生命周期解耦：通道常驻 + 带内会话边界（listen start/stop）+ front 心跳保活 + 封顶长跑重连 | 161 | ok |
| `delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md` | `T1` | T1 · 架构 §6.4 整节重写：通道生命周期与会话生命周期解耦 + 常驻保活 + 自愈巡检 + 长重试 | 79 | ok |
| `delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md` | `T2` | T2 · 架构 §5.1.1 / §5.1.2 / §5.2：统一会话结束处理器 + front 每会话态改由带内信号驱动 + 结束三通道接线 | 105 | ok |
| `delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md` | `T5` | T5 · 契约 §1.4 / §1.5 / §1.6：`end_session` 改「front 截获 + 机械自推单源」+ 告别回合 prompt 纪律 + 变更记录条 | 183 | ok |
| `docs/debt/tech/TECH-261014-session-end-channels-unwired.md` | — | 无锚点（整文件） | — | bare |
| `docs/debt/tech/TECH-261014-autoconnect-gives-up-silently.md` | — | 无锚点（整文件） | — | bare |

## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）

`none` —— uiux block 不在场或无在盘设计 refs（`designRefs[]` / `sharedUiRefs[]`）。本包若实有 UI 面，回报 planner 补挂——不脑补。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `server/gateway/display_schema.py` | `validate_display` | :6-8 | 128 |
| `server/gateway/front/front_app.py` | `pump_upstream_to_client` | :2060-2171 | 3118 |
| `server/gateway/front/front_app.py` | `_push_display_payload` | :786-795 | 910, 1075, 1323, 1327, 1401, 1565, 1570, 1596, 1647 |
| `firmware/main/mcp_server.cc` | `McpServer::DoTool` | :2900-2968 | — |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

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
| `package:261116-channel-resident-core` | 云端结束意图（end_session 三段补链） | `261117-session-end-intent-longretry` | `261117-session-end-intent-longretry` | P1 只建立统一处理器与 P1 内触发源；end_session 的 prompt / 门禁 / 设备 case 0 三段补链归 P2，P1 不得提前改 mcp_server.cc 与 display_schema.py。 |
| `package:261116-channel-resident-core` | 自动重连封顶长跑与人工意向位 | `261117-session-end-intent-longretry` | `261117-session-end-intent-longretry` | P1 只保留既有 TryStartAutoConnect 3 次退避与守卫三条件；长跑改形归 P2。 |
| `package:261116-channel-resident-core` | SCR-A2 in-flight 守卫断言的重锚 | `261116-channel-resident-core` | `261117-session-end-intent-longretry` | P1 冻结件 session_channel_resident.acceptance.c 的 SCR-A2「in-flight 守卫逐字在世」将被 P2 的 SEI-A4 重锚为「守卫在场 + 置意向位 + 审计行」；P2 须把该文件列入 impactedTestsSeed。 |
| `package:261116-channel-resident-core` | 自愈巡检使 3 次退避事实上变成长跑重试（≈35s 一轮、无上限） | `261117-session-end-intent-longretry` | `261117-session-end-intent-longretry` | P1 只保留既有 TryStartAutoConnect 3 次退避与守卫三条件；但通道常驻 + 每秒巡检使离线期事实形态 = 每 ≈35s 一轮 TLS 握手、无上限。P2 的 SEI-A2/A3 封顶长跑改形时，行号锚与形态基线以 P1 合并后的 main 为准（P1 review-packet 与轮 A A3/A12 观测项均记此事实）。 |
| `package:261116-channel-resident-core` | mcp_server.cc DISCARD_RECORD case 的注释双源 | `261117-session-end-intent-longretry` | `261117-session-end-intent-longretry` | mcp_server.cc:1011-1024 现注「严禁走收尾保存面（listen stop 上行会触发 front stop 归档）」。P1 后 discard 路径恰恰会发 listen stop（靠 SendKidpalStoryDiscard 先行把 routing 打回 chat 才不归档，契约 §四 2026-11-16 已把发送时机改锚到 listen stop 之前）。P1 不改该文件（守 P2 边界），P2 改 control case 0 时同批改写该注释。 |
| `workstream:261115-session-channel-resident` | 统一会话结束处理器（六触发源收口） | `261116-channel-resident-core` | `261117-session-end-intent-longretry` | P2 的 end_session 触发源必须收口到 P1 建立的处理器，不另起收尾路径；约束随 P2 的 input-brief 下发。 |
| `workstream:261115-session-channel-resident` | 云端结束意图三段补链 | `261117-session-end-intent-longretry` | `261117-session-end-intent-longretry` | P1 不得提前改 mcp_server.cc / display_schema.py / prompt 制品。 |
| `workstream:261115-session-channel-resident` | 自动重连封顶长跑与人工意向位 | `261117-session-end-intent-longretry` | `261117-session-end-intent-longretry` | P1 保留既有 3 次退避与守卫三条件；长跑改形归 P2。 |
| `workstream:261115-session-channel-resident` | 常驻模型下的守卫三条件语义与 SCR-A2 重锚 | `261116-channel-resident-core` | `261117-session-end-intent-longretry` | P1 冻结的 session_channel_resident.acceptance.c SCR-A2「in-flight 守卫逐字在世」将被 P2 的 SEI-A4 重锚为「守卫在场 + 置意向位 + 审计行」；P2 须把该文件列入 impactedTestsSeed。 |

上述条目 = **本包承接的跨包行为**（上游起包期 / 拆包期登记）。验收面须覆盖它们，或在本包 review-packet 的「未覆盖面申报」段写明去向（`dev-loop.md`「### 合并门 Review Packet」）。

## ⑥ 非阻断 advisory 明细（终端只打汇总；此处保留定位）

- **info · whole-document** · ref=`delivery/workstreams/261115-session-channel-resident/PROPOSAL.md`：Markdown 整文件引用（不自动猜章节）：delivery/workstreams/261115-session-channel-resident/PROPOSAL.md
- **info · whole-document** · ref=`delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md`：Markdown 整文件引用（不自动猜章节）：delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md
- **info · whole-document** · ref=`delivery/review/261115-arch-review-session-channel-resident/package-split.md`：Markdown 整文件引用（不自动猜章节）：delivery/review/261115-arch-review-session-channel-resident/package-split.md
- **info · whole-document** · ref=`docs/requirements.md`：Markdown 整文件引用（不自动猜章节）：docs/requirements.md
- **info · whole-document** · ref=`docs/architecture.md`：Markdown 整文件引用（不自动猜章节）：docs/architecture.md
- **info · whole-document** · ref=`docs/contracts/interface-contract.md`：Markdown 整文件引用（不自动猜章节）：docs/contracts/interface-contract.md
- **info · whole-document** · ref=`docs/decisions/ADR.md`：Markdown 整文件引用（不自动猜章节）：docs/decisions/ADR.md
- **info · whole-document** · ref=`docs/debt/tech/TECH-261014-session-end-channels-unwired.md`：Markdown 整文件引用（不自动猜章节）：docs/debt/tech/TECH-261014-session-end-channels-unwired.md
- **info · whole-document** · ref=`docs/debt/tech/TECH-261014-autoconnect-gives-up-silently.md`：Markdown 整文件引用（不自动猜章节）：docs/debt/tech/TECH-261014-autoconnect-gives-up-silently.md

