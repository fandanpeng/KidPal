# docs/debt/ —— 未结决策项账本（门面）

> 账本 = 一条目一文件（YAML frontmatter 机读状态 + 正文 append-only 散文）+ 每域一份**生成件** `INDEX.md`。
> 字段 schema 单一事实源 = `.claude/scripts/debt-ledger-shared.mjs`；条目骨架 = [`docs/templates/debt-item.template.md`](../templates/debt-item.template.md)。

## 四域（判据 = 谁裁 / 落哪个门）

| 域 | 目录 | ID 前缀 | 什么时候选它 |
|---|---|---|---|
| `gov` | [`gov/`](gov/INDEX.md) | `ARQ-` | 治理机制面：agent 定义 / skill / 门 / 脚本 / 纪律文面。裁定人 = architect |
| `tech` | [`tech/`](tech/INDEX.md) | `TECH-` | 技术相关：固件 / 工具 / 部署运维 / spec 文档（契约**决策账**已独立成 `contract` 域；tech 域契约类条目以 `cdq:` 指针挂接）。裁定人 = 各面 owner |
| `requirement` | [`requirement/`](requirement/INDEX.md) | `REQ-` | 改需求的**请求**与裁定状态。裁定人 = human，落地 = [`docs/requirements.md`](../requirements.md) §9 记账 |
| `contract` | [`contract/`](contract/INDEX.md) | `CDQ-` | 契约面变更的决策状态与剩余条件（原 `docs/CONTRACT-DECISION-QUEUE.md` 单文件队列，261019 批并入）。登记 = integrator 收口落条目 / 主线程治理批落条目；四分类五值权威 = [`.claude/rules/contracts.md`](../../.claude/rules/contracts.md) |

## 去哪里找 / 怎么登记 / 怎么结项

- **找未结**：各域 `INDEX.md`（生成件）的「待拍板 / 待触发 / 已接受风险」三节；**找条目原文**：`docs/debt/<域>/<条目 ID>.md`，或按 ID grep（`grep -rl "<ID>" docs/debt/`）。
- **登记**：按域写一个条目文件（骨架见模板），再跑 `node .claude/scripts/debt-render.mjs`。
- **结项**：把条目文件的 `status` 翻成 `closed` 并填 `closed` / `closedBy`，再跑同一个渲染命令。**不重打 ID、不换结构**；已结条目由渲染脚本**自动归位** `docs/debt/<域>/_archive/<结项年>/`（**人不搬文件**；重开自动搬回），主 INDEX 已结区只留每年一行指针。
- **各域 `INDEX.md` 是生成件，勿手改**；`node .claude/scripts/debt-doctor.mjs` 会报漏跑渲染与结构性错误。

## 迁址记（261019-gov-review-docs-root-ledger-rehome 批）

- `docs/ARCH-REVIEW-QUEUE.md`（指路 stub，260923 批降 stub）**已删除**——条目按 ID grep 即得（`grep -rl "<ARQ-id>" docs/debt/`）。
- `docs/CONTRACT-DECISION-QUEUE.md` **已删除**——12 行迁为 contract 域 12 条目（5 条已结在 `_archive/2026/`）。
- `docs/ADR.md` **已迁** [`docs/decisions/ADR.md`](../decisions/ADR.md)（条目编号与内容零改）。
- **历史制品（`delivery/**`、`docs/archive/**`、`docs/debt/<域>/_archive/<年>/`）与实现面散文里的旧路径引用悬空 = 已知代价**，以本节为指路兜底。

## 生成件纪律

各域 `INDEX.md`（含 `_archive/<年>/INDEX.md`）勿手改——`merge=union` + 渲染自愈：`debt-doctor.mjs` 报 `index-stale` 红 → 重跑 `node .claude/scripts/debt-render.mjs` 即归位。
