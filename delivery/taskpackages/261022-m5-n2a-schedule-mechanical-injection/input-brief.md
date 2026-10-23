# input-brief · 261022-m5-n2a-schedule-mechanical-injection

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-10-23T05:06:43.837Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: acceptance-author + builder（单份共用）
> 来源 TaskPackage: `delivery/taskpackages/261022-m5-n2a-schedule-mechanical-injection/taskpackage.json` · subsystem: server/deploy/engine-patch（动态 context 装配：课表摘要拼入每轮动态块）+ server/gateway/schedule.py（只读复用，单源） · rigor: lite
> **改动指令源 = plan.md**：`delivery/taskpackages/261022-m5-n2a-schedule-mechanical-injection/plan.md`（bug 类包——本 brief ①b 为指针形态，真实改动指令在此）。

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | `§1.5` | 1.5 网关的职责（薄，零自研业务逻辑红线不变） | 180 | ok |
| `delivery/workstreams/device-memory-overhaul/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/workstreams/device-memory-overhaul/task_plan.md` | — | 无锚点（整文件） | — | bare |
| `delivery/review/261022-arch-review-device-memory-prompt-overhaul/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `delivery/taskpackages/261022-m5-n2a-schedule-mechanical-injection/plan.md` | — | 无锚点（整文件） | — | bare |
| `docs/requirements.md` | — | 无锚点（整文件） | — | bare |
| `docs/architecture.md` | — | 无锚点（整文件） | — | bare |
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `docs/debt/tech/TECH-261018-schedule-read-compliance-residual.md` | — | 无锚点（整文件） | — | bare |
| `TECH-261018-schedule-read-compliance-residual` | — | （文件不存在） | — | missing |
| `delivery/workstreams/device-memory-overhaul/PROPOSAL.md` | `§3.1` | 3.1 方案空间与裁定 | 53 | ok |
| `delivery/workstreams/device-memory-overhaul/PROPOSAL.md` | `§4` | 4. 拆包预判（意图提示，非权威） | 92 | ok |
| `delivery/review/261022-arch-review-device-memory-prompt-overhaul/PROPOSAL.md` | `T5` | T5 · 课表机械注入：front 单源派生 → 内部通道 → 动态 context 块 | 345 | ok |
| `server/gateway/schedule.py` | — | 无锚点（整文件） | — | bare |
| `server/deploy/engine-patch/connection.py` | — | 无锚点（整文件） | — | bare |

## ①b · story 语义权威（bug 类包 · 指针形态——改动指令源在 plan.md）

### S-F1.3 · 源 = `docs/requirements.md` 节首行 :278 · 本包声明 AC 粒度回引：`S-F1.3-AC1`

> 本包 sourceType 为 bug 类且 planRef 在场：**不照录整节**——语义权威 = `docs/requirements.md` 现文（`S-F1.3 · 语音问课表`），按需自读；story 节对本包仅作背景，真实改动指令 = plan.md（见头部改动指令源行）。


## ①c · UI 设计 refs（uiux block 透传 · 页级 PNG = 布局坐标定位权威 · 本节主要消费方 = builder）

`none` —— uiux block 不在场或无在盘设计 refs（`designRefs[]` / `sharedUiRefs[]`）。本包若实有 UI 面，回报 planner 补挂——不脑补。

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

本包声明 **exempt**（不适用跨端对端阅读）：

> 本包纯云端动态 context 装配与单源复用：设备端零感知、固件零 diff；不新增或改变任何端→网关消息、帧类型、字段与序列化形态，B1 课表 MD 结构与 display E1 零改动。

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
| `workstream:device-memory-overhaul` | 动态 context 块装配契约（有界 / 不含纪律 / 每轮在场） | `261022-m5-k1-prompt-carrier-migration` | `261022-m5-n2a-schedule-mechanical-injection` | P1 建机制与模板；P5 在模板占位内注入课表摘要，不得另建第二动态块；P5 可先于 P2 合并，P2 rebase 须保留 P5 装配段。 |
| `workstream:device-memory-overhaul` | M5 收口轮（跨包并集验收 + live 真机轮 + P4 计数报告对账） | `261022-m5-n2a-schedule-mechanical-injection` | `261022-m5-n2a-schedule-mechanical-injection` | P5 为最后一个行为切片的收口轮 owner；P4 前後计数报告同轮对账；真机配额默认仅收口轮（P1–P4 不单独占轮）。 |

上述条目 = **本包承接的跨包行为**（上游起包期 / 拆包期登记）。验收面须覆盖它们，或在本包 review-packet 的「未覆盖面申报」段写明去向（`dev-loop.md`「### 合并门 Review Packet」）。

