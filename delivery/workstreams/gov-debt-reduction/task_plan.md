---
# —— 必填 ——
workstream: 治理债清偿（gov debt reduction）
slug: gov-debt-reduction
status: active
opened: 2026-10-26
closed:
summary: docs/debt/gov/ 未结条目的分簇清偿计划——按收益/成本比排 P1–P4 四波，P1 合并闸簇与 P1.5 final 相位派发契约批均已合 main，下一波 = P2 锚解析门+模板簇
# —— 选填 ——
owner: main-thread
area: governance
changes:
  - 261026-gov-review-merge-gate-cluster-fix
  - 261026-gov-review-final-phase-dispatch-contract
  - 261026-gov-review-p2-anchor-template-cluster
---

# task_plan · 治理债清偿（gov-debt-reduction）

## 目标

把 `docs/debt/gov/` 的未结条目从「散点触发挂靠」收敛成**分簇清偿波次**：同触发面/同脚本/同文面的条目并批处理，降低每个治理批的立项与评审边际成本；按「复购率 × 阻断度 ÷ 修法成本」排波次。

## phase（分波交付计划）

### P1 · 合并闸/强制执行层簇（✅ 2026-10-26 已落）

- 六条一次收口：ARQ-261017-merge-gate-contract-evidence-ref + ARQ-261021（同根合并消费）+ ARQ-261017-scope-unstated + ARQ-261014-tooling-hotfix + ARQ-261011-selfquote + ARQ-261018（MAJOR 搭载，human 裁）。
- changeId = `261026-gov-review-merge-gate-cluster-fix`；全环闭环（R3 GREEN → PASS+7A → 已合 main → PR #40 已合并）。
- 详见 `progress.md`。

### P1.5 · final 相位派发契约批（✅ 2026-10-26 已落，MAJOR）

- 主条目：`ARQ-261026-final-phase-dispatch-contract-unwritten`（八包实证每包烧半轮到一轮；修法 = G2 文面几句）。
- 并批（同面）：`ARQ-260923-codemem-mode-doc-drift`（同动 launch-prep 文面）+ `ARQ-261025-interrupted-dispatch-progress-note`（同动 planner 派发步/brief 模板）。
- changeId = `261026-gov-review-final-phase-dispatch-contract`；全环闭环（R2 GREEN → PASS → 已合 main `47888d5b`）。详见 `progress.md`。

### P2 · 锚解析门 + summary 模板簇（✅ 2026-10-26 已落）

- `ARQ-261019-anchor-probe-fence-blind`（governance-gate-shared §2 边界探测不跳围栏）+ `ARQ-261019-verbatim-quote-line-level-false-negative`（NIT）。
- `ARQ-260928-landing-summary-skeleton-template` 与 `ARQ-261021-landing-summary-template` **先并条再消费**（同一件事：summary 缺模板承载；低成本案 = arch-change.md 加两句字面义务）。
- §5 验证集纪律三条纯文面：`ARQ-261019-forward-derived-verification-set` + `ARQ-261025-verify-set-negative-positive-pairing` + `ARQ-261021-proposal-assert-expectations`。
- **搭载**：`ARQ-261026-release-pr-gov-lane-zero-pods`（P3 项，本批动 `.claude/scripts/**` 顺手落）。
- **拆出不动**：`ARQ-260924-landing-gate-dir-anchor-gap` 残余②（目录锚展开，human 2026-10-26 裁独立批——触承重闸判定逻辑，单独评审）。
- **立项 ack（2026-10-26）**：① summary 义务 = arch-change.md 加两句字面（不立模板，视复发再立）；② 实跑回填纪律扩到 PROPOSAL 全部实测数字；③ release-pr gov-lane 取最小版；④ 六条并批 + 搭载，Track B 主路径。changeId = `261026-gov-review-p2-anchor-template-cluster`。

### P3 · release-pr gov-lane 门面（搭载项，不独立开批）

- `ARQ-261026-release-pr-gov-lane-zero-pods`：发布脚本对 gov 批提取不到 changeId，PR 门面落「0 个 Pod 合并」。搭载 P2 或下个动 `.claude/scripts/**` 的批。

### P4 · Pod / board 面簇（排后，摩擦型非阻断）

- `ARQ-260918-pod-session-settings-identity-unobservable` + `ARQ-261014-portfolio-reconcile-launchpreconditions` + `ARQ-261025-shared-branch-pod-isolation` + `ARQ-261025-wait-state-closed-set` + `ARQ-261025-intake-index-retro-scope-hygiene`。文面/脚本混杂（pod-coordination / portfolio-reconcile / launch 面分属不同评审强度）。

### 不动档（触发挂靠，不排波）

- ~12 条「下次动 X 时并批」型（known-red-count ③、versioned-guard-repoint、current-pointer 量化观察期、rename-batch-census、debt-archive-ref-atomic 等）+ 已接受风险 2 条（contract-mech-gate-gap、crossend-consumer-tiers）+ 2 条明确单开批（auto-mode-classifier 双臂批、host-blind-outcome 门）。
- 观察期在册：current-pointer 的 `expectCheck:'absent'` ≥1 实例触发闸级强制批；anchor-check-master「再犯即升格」。

## 决策记录

- 2026-10-26 human 拍板：261018 搭载 P1；260924② 拆出独立批；OQ1 取「允许集从账本拓扑正推」（候选 2 形态）；OQ2 修门+兜底成文；OQ4 闸内点名。
- 2026-10-26 human 指示：261014④ 拆出须登记 governance debt 防忘 → ARQ-261026-final-phase-dispatch-contract-unwritten。
- 2026-10-26 human 反馈：发布 PR 门面「没写改了啥」→ ARQ-261026-release-pr-gov-lane-zero-pods；PR #40 门面已人工重写。
- 本 workstream 走 `/gov-change` 面 loop（不涉 dev-loop 拆包派遣，milestones.jsonl 登记不适用；若后续某波要拆 dev-loop 包，先补里程碑登记再派 portfolio-planner）。
