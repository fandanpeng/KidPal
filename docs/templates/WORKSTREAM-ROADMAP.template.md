# WORKSTREAM-ROADMAP.template · workstream 活文档 roadmap 模板（canonical · 260912-gov-review-portfolio-decomposition-chain T1 落地，FROZEN v1.0）

> 用法：复制本件为 `.planning/<workstream-slug>/roadmap.md`。**产方 = portfolio-planner**（拆包时），**审方 = 门 1 判官 + human**，**消费方 = planner / acceptance-author（锚 2）+ integrator / supervisor（锚 3）**。
> 本件是**活文档**：与冻结的 `PROPOSAL.md`（裁决记录）双轨分离。
> **职责划界（防第二事实源）**：`task_plan.md` frontmatter 维持 `planning-index.mjs` 机器索引源地位；`progress.md` 维持进度记录；本件 = **效果对账 + 列车图投影**。包集与达成状态的权威恒 = board（`$(git rev-parse --git-common-dir)/devloop/`）+ `.claude/taskpackages/<changeId>/taskpackage.json`——**本件只投影不裁决**，重叠处以权威侧为准。
> 文体：③ 节每包效果段是唯一需要人写的散文（机器写不出的那一段）；②④ 节与达成状态列由脚本渲染。

---

# <workstream 标题> · roadmap

> workstream：`<slug>` ｜ 设计权威：`PROPOSAL.md`（FROZEN <vX.Y>，pin `<sha1:…>`）｜ 本件由 `render-roadmap.mjs` 投影渲染，②节与达成状态列人工禁改。

## ① 效果目标投影

**（投影自 `PROPOSAL.md` §1 业务目的清单，不复制正文——只引条目号 + 一句判据摘要 + 指针。）**

| 目的# | 判据摘要（一行） | 权威指针 |
|---|---|---|
| 1 | <人可观察判据摘要> | `PROPOSAL.md#§1` 目的 1 |

## ② 列车图（依赖 + 批次）

<!-- render-roadmap:begin -->
<!-- 本区块由 node .claude/scripts/render-roadmap.mjs --workstream <slug> --write-into <本文件> 机械渲染。
     人工禁改本区块；渲染源唯一 = board + taskpackage JSON（dependsOn / mergeOrder / 达成状态）。
     改依赖或排序 = 改 taskpackage JSON / board，再重跑脚本全量重算（幂等）。 -->
<!-- render-roadmap:end -->

## ③ 每包条目

**（每包一小节。效果段是唯一人工散文位：从 ① 的业务目的清单分解而来，写「完成后人能看到什么」。）**

### <changeId>

- **mergeOrder**：<N（脚本渲染列同步）>
- **完成后人能看到什么**：<一段业务效果——人只写机器写不出的这一段；从 §① 目的清单分解，不新立目的>
- **回指**：`S-Fx.y · <标题>`（AC 号集：<AC1、AC3…>）——**新建型必填**；只投影判据、不改写不复述 story 语义（story 语义权威 = `docs/requirements.md`）。整改型回指缺陷证据锚、重构型回指结构性痛点锚。
- **demo 证据形态**：<模拟器截图 / host runner 输出 / 串口日志 / 真机轮>（真机轮配额默认仅核心包）
- **达成状态**：<pending | achieved | deviation（裁定锚 <eventId>）>——脚本从 board/merge 状态 + 锚 3 对账记录机械渲染，人工禁改

## ④ 非目标与批次归属

（投影自 `PROPOSAL.md` §3.3；跨批次前置/后继在此点名，含跨 workstream 外部前置包。）
