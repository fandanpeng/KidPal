# ARCH-REVIEW-QUEUE · 已迁址（本文件仅为指路 stub）

> **本队列已重构为域账本，原单文件表格已不存在。** 本文件保留路径只为**不断全仓既有引用**（活跃引用面与历史制品引用均以 `grep -rl "ARCH-REVIEW-QUEUE" .` 可枚举；路径不变故全部继续可达）；它是**手写 stub、不是生成件、不再承接任何登记**。

## 去哪里找

| 你要什么 | 去哪里 |
|---|---|
| **Gov 优化账**（治理机制：agent / skill / 门 / 脚本 / 纪律文面的未结项与已结索引） | [`docs/debt/gov/INDEX.md`](debt/gov/INDEX.md) |
| **Tech 账**（技术相关，含 contract：固件 / 工具 / 部署运维 / 契约 / spec 文档） | [`docs/debt/tech/INDEX.md`](debt/tech/INDEX.md) |
| **Requirement 账**（改需求的请求与裁定状态；已落变更仍记在 [`requirements.md`](requirements.md) §9） | [`docs/debt/requirement/INDEX.md`](debt/requirement/INDEX.md) |
| **条目原文**（证据 / 影响面 / 建议改动 / 风险） | `docs/debt/<域>/<条目 ID>.md`，一条目一文件 |
| **契约决策状态**（未变，仍在原处） | [`docs/CONTRACT-DECISION-QUEUE.md`](CONTRACT-DECISION-QUEUE.md) |

## 怎么登记 / 怎么结项（一句话）

- **登记**：按域写一个条目文件 `docs/debt/<域>/<ID>.md`（骨架见 [`docs/templates/debt-item.template.md`](templates/debt-item.template.md)），再跑 `node .claude/scripts/debt-render.mjs`。
- **结项**：把条目文件的 `status` 翻成 `closed` 并填 `closed` / `closedBy`，再跑同一个渲染命令。**不重打 ID、不换结构**；已结条目由渲染脚本**自动归位** `docs/debt/<域>/_archive/<结项年>/`（**人不搬文件**；重开自动搬回），主 INDEX 已结区只留每年一行指针。
- **三份 `INDEX.md` 是生成件，勿手改**；`node .claude/scripts/debt-doctor.mjs` 会报漏跑渲染与结构性错误。

> 本批改动来源：`delivery/changes/arch/260923-gov-review-arq-ssot-restructure/`。