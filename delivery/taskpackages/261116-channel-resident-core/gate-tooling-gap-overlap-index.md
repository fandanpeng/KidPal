# gate-tooling-gap · parallel-overlap 对生成件 `delivery/workstreams/INDEX.md` 误判 tier-3

> 发现相位：`261116-channel-resident-core` 真机轮 A 修复轮收尾（planner）。性质 = **规则文面列举不全 + `.gitattributes` 缺 union 条目**，非本包缺陷。
> 状态：已由 architect 做 §10d ③ 消解评审（dispatch `a7c1ad6c73ea2da9f`，advisory）；本文件供主线程 `/gov-change` 修复与合并期复核。

## 现象

`node .claude/scripts/pre-merge-escalation-gate.mjs 261116-channel-resident-core --source-ref HEAD` 报：

```
- parallel-overlap-tier3：并行 Pod 相交有取舍，需 architect 消解评审（pod-coordination.md §10d）——实撞 1 件（delivery/workstreams/INDEX.md）
```

## 盘上事实（architect 实测，与闸叙述有出入）

- `delivery/workstreams/INDEX.md` 是纯生成件（头注「自动生成……勿手改」）；权威渲染器 = `node .claude/scripts/planning-index.mjs`（`reconcile()`，无参数直跑重建；数据源 = 各 workstream `task_plan.md` front-matter），hook = `.claude/hooks/planning-sync.mjs`。
- `git merge-tree` 实撞的**唯一 hunk = 头部「最后更新 <date>」一行**（我方 2026-11-16 / main 2026-11-17）；我方追加的 workstream 行与 `Active (6)→(7)` 计数**已被 git 自动并**，合并结果树里完整在场。
- 该日期是渲染产物，哪侧赢都在下次重渲染被覆盖 → **无语义可取舍**。按 `pod-coordination.md` §10d 末句「生成件由 union 免冲突 + 重跑渲染归位」类规则应判 **②（机械消解）**。

## 根因

1. `pod-coordination.md` §10d 末句的**列举**只写「三条 lane 的 INDEX.md（渲染器 changes-index.mjs）」，未列 `delivery/workstreams/INDEX.md`（planning-index.mjs）与 `docs/debt/*/INDEX.md`（debt-render.mjs）——类规则覆盖、列举不全。
2. `.gitattributes` 未给 `delivery/workstreams/INDEX.md`（及 `index.jsonl`）配 `merge=union`；仓内已有四例同形先例（`kidpal-mvp/progress.md`、`delivery/taskpackages/**/pod-events.jsonl`、`docs/debt/*/INDEX.md`、三 lane INDEX）。
3. 闸的 `DERIVED_STATE_CARVE_OUT`（`pre-merge-escalation-gate.mjs:380-383`）刻意与 `merge-driver.mjs` 的 `PROJECTION_CONFLICT_FILES` 同形，**不建议只扩闸侧**（会造「闸放行、driver abort」错位）。

## 建议修法（architect advisory，交主线程 `/gov-change`）

1. **主修（推荐）**：`.gitattributes` 增 `delivery/workstreams/INDEX.md merge=union`；同批裁 `delivery/workstreams/index.jsonl` 是否一并入（architect 倾向入：`reconcile()` 全量重写、无门禁消费）。union 落地后该文件不再进 `conflictedFiles` → 闸自然 `union-safe`，merge-driver 也不再撞非特判文件。
2. `pod-coordination.md` §10d 末句列举改为**类级表述 + 逐族点渲染器**（changes-index 三 lane / debt-render / planning-index），让类规则与列举自洽。
3. 不建议只扩 `DERIVED_STATE_CARVE_OUT`；若要闸侧兜底，须与 driver 侧处置同批改。
4. 建议主线程在 `docs/debt/gov/` 登记本次实例。

## 对合并的影响（承重）

- **`parallel-overlap-tier3` 不在 `escalateExempt` 覆盖内**（闸 `:702-707` 无条件 push；riskTier / 非 APPROVE 两条都带 `&& !escalateExempt`）——**human 批准（`--human-approval-ref`）豁免不了本条**。
- 合并前必须让冲突消失，二选一：
  1. 先落 `.gitattributes` union 条目（推荐）；
  2. 或主线程手工 rebase/merge 消解那行日期并重跑 `planning-index.mjs`，使 main 成为祖先 → `intersectingFiles` 空 → `no-overlap`。
- 本项**不构成独立停等理由**（本包停等实质 = riskTier high ESCALATE + 真机轮 B 未跑），但合并动作前必须先消解，不能只靠人批放行。

## 复现

```bash
cd <worktree>
node .claude/scripts/pre-merge-escalation-gate.mjs 261116-channel-resident-core --source-ref HEAD
# 期望：parallel-overlap-tier3（实撞 delivery/workstreams/INDEX.md）
git merge-tree --write-tree --name-only HEAD main | head
# 期望：唯一冲突 hunk 为「最后更新」日期行
```