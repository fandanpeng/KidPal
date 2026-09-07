# ADR · 架构决策记录（Architecture Decision Log）

> 性质：架构决策的长期留档——每条记「选了什么 / 为什么 / 放弃了哪些备选」。区别于 `changes/**`（过程证据，按 change 归档）与 `docs/notes/`（经验与策略记录）：ADR 面向「拍板过就不该丢」的决策本体，新会话 / 新成员不考古即可查。
> 落点凭据：human 2026-09-07 裁「放 docs 根目录、和 spec 同级」（OQ3，260907-gov-review-arq-governance-batch-2 T7）——单文件平铺，承本仓单文件 markdown 注册表惯例（对偶 ARCH-REVIEW-QUEUE.md / DEV-GOVERNANCE.md）。
> 写入者：integrator（dev-loop 收口落新增条目）+ 主线程（治理批 landing 落条目）。**append-only：不覆盖、不改写既有条目**；推翻旧决策 = 新增一条并注明 supersedes ADR-XXX。
> 条目格式：每条 = `## ADR-NNN · <决策一句话>`（NNN 三位递增）+ 固定小节：**日期** / **决策** / **理由** / **备选与放弃原因** / **关联**（change-id 或制品锚）。

（尚无条目——存量散落的决策不做考古搬迁，自 260907 批 landing 起新决策按本格式登记。）
