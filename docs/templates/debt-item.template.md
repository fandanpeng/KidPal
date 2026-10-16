# debt-item.template · debt 条目文件骨架（canonical · 260923-gov-review-arq-ssot-restructure T1 落地）

> 用法：复制本件为 `docs/debt/<域>/<ID>.md`，替换 `<尖括号>` 占位，删掉本注与示例正文。
> **字段 schema 的单一事实源是 `.claude/scripts/debt-ledger-shared.mjs`**（闭集与必填项以它为准，本件只教怎么填、不重复定义）。
> 三份 `INDEX.md` 由 `node .claude/scripts/debt-render.mjs` 从条目文件生成，**勿手改**；写完跑一次渲染，再跑 `node .claude/scripts/debt-doctor.mjs` 自检。
> 引用条目文件的路径**必须带目录**（`docs/debt/gov/ARQ-xxxxxx-yyy.md`），仓内共有多个同名 `INDEX.md`。

## 域怎么选（三值，判据 = 谁裁 / 落哪个门）

| 域 | 目录 | ID 前缀 | 什么时候选它 |
|---|---|---|---|
| `gov` | `gov/` | `ARQ-` | 治理机制面：agent 定义 / skill / 门 / 脚本 / 纪律文面。裁定人 = architect，落本账的 Track B/A 批次 |
| `tech` | `tech/` | `TECH-` | 技术相关，**含 contract**：固件 / 工具 / 部署运维 / 契约 / spec 文档。裁定人 = 各面 owner，去向填 `handoff` |
| `requirement` | `requirement/` | `REQ-` | 改需求的**请求**与裁定状态。裁定人 = human，落地 = [`requirements.md`](../requirements.md) §9 记账 |

> **ID**：`<前缀>-<YYMMDD 创建日>-<短英文 kebab>`；**文件名必须与 `id` 相同**。
> **祖辈前缀 `ARQ-`**：历史词汇，落在任何域都放行（只出 advisory 计数）；**新条目请用本域前缀**。

## 骨架

```markdown
---
id: <前缀>-<YYMMDD>-<slug>
title: <一句话人读标题——INDEX 表格 title 列渲染它，缺了即 doctor 红（missing-field）>
domain: <gov | tech | requirement>
status: <blocker | deferred | accepted-risk | closed>
severity: <BLOCKING | MAJOR | SHOULD | COULD | NIT | OPTIONAL | FYI>
owner: <谁承接，如 architect / dev-loop / human>
trigger: <什么时机处理它——「下一治理批」「下个动 X 的包」这类可判触发>
opened: <YYYY-MM-DD，通常 = ID 里的日期>
closed: <已结才填 YYYY-MM-DD>
closedBy: <已结才填：结项依据的链接，如 delivery/review/<id>/landing-verdict.md>
paths: [<待改的确切仓根相对路径，逗号分隔；空则 G 层不可机判，doctor 出 advisory>]
source: [<provenance：可点开的执行记录，如就地的 review-packet / pod-events / retro 行>]
---

**证据**：<地面真值——file:line verbatim / 实测命令与输出 / 制品锚点。**不得写「执行时看到过」这类无法复核的话**>

**影响面**：<爆炸半径：谁会受影响、多久发生一次、不修的代价>

**建议改动**：<候选改法（**交 review、不自批**）>

**风险**：<riskTier: low | medium | high> + 一句话证成
```

`tech` 域另有两个可选字段：`handoff`（去向 + 移交触发，如 `delivery/workstreams/<ws>`；**空则 doctor 出 advisory**）、`cdq`（契约类条目的 CDQ 指针，如 `CDQ-260914-kidpal-session-ctx`；**契约的门与批准状态仍以 CDQ 为唯一权威**）。

`requirement` 域另有三个可选字段：`affects`（受影响的 story·AC 锚，如 `S-F1.1-AC1`）、`decision`（human 裁定原文）、`landedIn`（已落时填 `requirements.md` §9 的版本号）。

## 三条纪律

1. **正文只追加、不改既有段**——历史证据不可被重写（`AGENTS.md` Hard Rules）。要更正，就在尾部追加一段并写明更正理由。
2. **结项 = 原地翻字段**（`status: closed` + `closed` + `closedBy`，正文尾追加一行「**结项**：…」；被变更消费而结项时，结项行写 `consumed-by: <changeId>`——正文约定，change-intake.md §3a，机检兼容性实测零脚本改动），**不重打 ID、不换结构**；已结条目由 `debt-render.mjs` **自动归位** `_archive/<结项年>/`（**人不搬文件**——位置是状态的纯函数，重开自动搬回，`debt-doctor.mjs` 核「位置 == f(状态)」）。
3. **登记前先查重**：`grep -l "<关键词>" docs/debt/*/*.md`——命中即已有条目，**在既例行追加证据注记、不新立**。

## 正文段落名（渲染与检查不依赖，但保持同形便于人读与产物一致）

`**证据**` / `**影响面**` / `**建议改动**` / `**风险**` 四段为现行约定形态；迁移自旧队列的条目四要素原样保留在同一段落体系内。