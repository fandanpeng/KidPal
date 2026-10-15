# `delivery/workstreams/` — 长周期工作记忆

本目录承载**跨切片、长周期**的工作记忆（为什么 / 发现 / 进展）。它是 workstream（里程碑 / 命名倡议）级的，区别于单切片验收包（`delivery/taskpackages/`）与实现变更归档（`delivery/taskpackages/`）。

> 本仓是治理迁移的采用方（自 AInativecompany 迁入），非 dogfood 参考实例；本文件只保留与栈无关的通用结构与纪律。

## 结构

```text
delivery/workstreams/
  README.md                       # 本治理文件（准入 / 归属 / 生命周期 / 结构）
  INDEX.md                        # 人读总览（hook 自动生成，勿手改）
  index.jsonl                     # 机读 feed：工具只读消费（hook 自动生成，勿手改）

  <workstream-slug>/              # 顶层 = 只放“活着的”（status: active | blocked）
    task_plan.md                  # front-matter（状态单一源）+ 目标 / phase / 决策
    findings.md                   # 调研发现、风险、偏差
    progress.md                   # 执行日志（追加式）：改了什么、测了什么、失败/修复
    ledger.jsonl                  # 可选 append-only 过程日志

  _archive/                       # 完结的（status: done | abandoned），按收口年份分桶；不删=耐久记忆
    <year>/
      <workstream-slug>/
        task_plan.md
        findings.md
        progress.md
```

`<workstream-slug>`：kebab-case、稳定、不带 change-id / 日期，对应一个里程碑或命名倡议。

## 什么样的工作才配建目录（准入判据）

> **半年后接手的 agent 要靠“为什么 / 发现 / 进展”才能继续 → 配目录。
> 需要的一切都在 diff + taskpackage + commit message 里 → 不配目录。**

自上而下套这个梯：

1. 带 `<change-id>` 的单切片 dev-loop 变更 → **不建**，归 `delivery/taskpackages/<change-id>/`（晋升后进 `delivery/taskpackages/`）。
2. 已存在 workstream 的子任务 / 续作 → **不建**，追加进那个 workstream 的 `progress.md` / `findings.md`。
3. 跨多切片 / 多会话，且带未来 agent 必须读的 why / findings / deviation → **建** `delivery/workstreams/<slug>/`。
4. 其余（琐碎一次性、纯探讨、无耐久产出）→ **不建**，靠 commit message 或不留。

粒度是 **1 workstream : N changes**：N 个变更挂同一个 slug 下（taskpackage 引用它），workstream 目录只累加 progress，所以 change 数飞涨、目录数缓涨。

## 谁建（创建者归属）

> **谁写代码，谁建 workstream。**

| 时机 | 谁 | 动作 |
|---|---|---|
| 立项里程碑 / 倡议 | 人 或 主线程 planner | 套准入判据 → 建 `<slug>/` + `task_plan.md` |
| 跑动中 | planner / builder / fixer | 追加 `progress.md` / `findings.md` |
| 收口 | integrator | 填 `closed:` + `status: done` → 触发归档 |
| 始终 | verifier、architect-reviewer | 只读，不建不改 |

写权限以 [.claude/scope-map.json](../.claude/scope-map.json) 各 agent 条目的 `delivery/workstreams/**` allow 为准。

## task_plan.md front-matter（状态单一事实源）

```yaml
---
# —— 必填 ——
workstream: <人读标题>
slug: <= 目录名；kebab-case；稳定；无 change-id/日期>
status: active            # active | blocked | done | abandoned
opened: YYYY-MM-DD
closed:                   # done/abandoned 时必填 → 决定落进 _archive/<year>/
summary: <一句话，进 INDEX>
# —— 选填 ——
owner: main-thread        # 人 / 主线程 planner / integrator…
area: governance          # kernel | systems | governance（仅供 INDEX 分组）
changes:                  # 挂在本 workstream 下的 change-id（体现 1:N）
  - <change-id>
---
```

## 拆包顺序链（260928-gov-review-portfolio-boundary-story-propagation T7，M1–M4 先例链成文）

两条顺序纪律（agent 侧的 fail-closed 停等 = `portfolio-planner.md` Step 1「拆包顺序链停等三件」，与本节互为表里——本节管人读纪律、intake 停等管 agent 行为）：

1. **task_plan 的 `phase` 节为 workstream 必备节**——建 workstream 即落目标 / phase（分波交付计划），不后补；无 phases = 拆包派遣的前置件缺失。
2. **里程碑登记（`delivery/milestones/milestones.jsonl`）先于拆包派遣**——workstream 建目录、派 portfolio-planner 之前，本倡议必须已在 milestones.jsonl 登记（含 nextAction）；ui-v5-align 事故（2026-09-24，21 包建毁、3 轮 human 复裁）即「建 workstream 当日直接派遣拆包」的实证。

## 生命周期

1. 人 / planner 在顶层建 `<slug>/`，`status: active`。
2. planner / builder / fixer 追加 `progress.md` / `findings.md`。
3. 卡住 → `status: blocked`（仍留顶层，INDEX 高亮）。
4. 收口 → integrator **最后一步**填 `closed:` + `status: done`；PostToolUse 钩子（[.claude/hooks/planning-sync.mjs](../.claude/hooks/planning-sync.mjs)）随即把目录搬进 `_archive/<closed 年份>/` 并重建 `INDEX.md` / `index.jsonl`。

> `status: done` 是收口的**最后一个写动作**——它一落，目录立即被归档搬走，之后别再往旧路径写。

## 边界（别跟其它层重复）

| 层 | 是什么 | 谁写 |
|---|---|---|
| `delivery/workstreams/<workstream>/` | 跨切片、长周期工作记忆（为什么 / 发现 / 进展） | 人 · 主线程 · dev-loop agent |
| `delivery/taskpackages/<change-id>/` | 单个正式实现切片的机器可读验收包（exit criteria / 预算 / 证据） | dev-loop |
| `delivery/taskpackages/<change-id>/` | 晋升后的长期实现变更归档 | integrator |

## 约定

- `progress.md` 追加式、短句，记真实动作与状态，不写空话。
- 一个 workstream 完结后保留归档（搬进 `_archive/`），不删（这是耐久记忆）。
- `INDEX.md` / `index.jsonl` 由 hook 自动生成，**勿手改**；要改内容去改各 `task_plan.md` 的 front-matter。
- 手动重建索引：`node .claude/scripts/planning-index.mjs`。
- `delivery/workstreams/` 入库版本控制（"代码库是 planning 源头"），不进 `.gitignore`。
