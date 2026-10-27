---
# —— 必填 ——
workstream: 治理债清偿（gov debt reduction）
slug: gov-debt-reduction
status: active
opened: 2026-10-26
closed:
summary: docs/debt/gov/ 未结条目的分簇清偿计划——按收益/成本比排 P1–P4 四波，P1 / P1.5 / P2（锚解析门+模板簇）三波均已合 main，剩余 = P3 搭载挂靠与 P4 簇
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

- `ARQ-260918-pod-session-settings-identity-unobservable` + `ARQ-260914-portfolio-reconcile-launchpreconditions` + `ARQ-261025-shared-branch-pod-isolation` + `ARQ-261025-wait-state-closed-set` + `ARQ-261025-intake-index-retro-scope-hygiene`。文面/脚本混杂（pod-coordination / portfolio-reconcile / launch 面分属不同评审强度）。

### P4 复证记录（2026-10-26 立项期，主线程实测）

- 260918：候选① 前半已落（260921 T3，canonical 命令行两形态在启动卡模板 :131-133）；残余 = ① 后半「VS Code 扩展会话降级面标注」（实测前置 = 扩展会话能否传 `--settings`/`--agent`，两批均未实测）+ 候选② 在场性自检落 pod-events（planner.md 无此步，未落）+ 候选③ 门策略。候选③ 的最低成本形态（planner.md :111-112 supervisor-gate-check 显式步骤）已在场。**本条自带设计分叉**（身份要不要机械自证 / 非 CLI 形态如何表达 canonical），原条目 profile 标 full-adversarial 候选。
- 260914：读侧已落（260921 T6，portfolio-reconcile.mjs 17 处消费）；残余 = 存量 12 条数据迁移——实测 `m1-home-qa` / `m2-char-window` 两 workstream 的 `launchPreconditions` 均仍为裸 string，两 workstream 仍在册。
- shared-branch-pod-isolation：落点在场（merge-driver.mjs / pod-coordination.md），未消化。
- wait-state-closed-set：实测 `POD_STATUSES`（portfolio-reconcile.mjs :36）八值闭集、无 awaiting-human 档，未消化。
- intake-index-retro-scope-hygiene：实测 `computeRetroScope`（loop-health-report.mjs :177）无 `current` 排除；intake INDEX 漂移标现存 4 处（且 261012 已挂 ⚠滞留10d——漂移在增长，「下轮 retro 仍见漂移」触发条件已成立）。
- 判档预估：五条中三条落 scripts（portfolio-reconcile / changes-index / loop-health-report）→ 命中轻路径判据③，**整批恒 Track B**（只升不降）。
- **立项 ack（2026-10-26 human 四拍）**：① 五条一批 Track B 主路径（拆批不降评审强度、只加开销）；② 260918 只取低分叉子集——候选② 在场性自检落 pod-events + ① 后半降级面标注，不新增 gate 不裁机械牙，门策略留观察；③ shared-branch 取方案① 文面（pod-coordination §2c 互斥租用禁令 + 每包 base ref 钉死 + launch 检查项），**worktree 池化（Pod pool：空闲工位注册/租用/释放）由 human 另登记为独立 idea 走后续专项批**，方案② merge-driver 扩参留给池化批按其形态再定；④ 260914 存量 12 条迁移搭本批。
- changeId = `261026-gov-review-p4-pod-board-cluster`，architect 起草中。
- 2026-10-27 起草完成（DRAFT v0.1，546 行，preflight 11 锚 ok；draft 已 upsert 进 delivery/review/INDEX.md）。OQ human 前置拍板（2026-10-27）：**OQ1 = (a) 保守标注 + 观察期**（与草案一致）；**OQ2 = (b) 同批打通**——merge-driver approvalRefUnlock（:1101 附近，以 blocked 为键）解锁判据同批扩 awaiting-human:merge-gate，manifest +1，riskTier 重估；wait-state 条目可望整条结项（非部分）。R1 四查进行中，R1 findings + OQ2 修订合并一轮改稿。

### 不动档（触发挂靠，不排波）

- ~12 条「下次动 X 时并批」型（known-red-count ③、versioned-guard-repoint、current-pointer 量化观察期、rename-batch-census、debt-archive-ref-atomic 等）+ 已接受风险 2 条（contract-mech-gate-gap、crossend-consumer-tiers）+ 2 条明确单开批（auto-mode-classifier 双臂批、host-blind-outcome 门）。
- 观察期在册：current-pointer 的 `expectCheck:'absent'` ≥1 实例触发闸级强制批；anchor-check-master「再犯即升格」。

## 决策记录

- 2026-10-26 human 拍板：261018 搭载 P1；260924② 拆出独立批；OQ1 取「允许集从账本拓扑正推」（候选 2 形态）；OQ2 修门+兜底成文；OQ4 闸内点名。
- 2026-10-26 human 指示：261014④ 拆出须登记 governance debt 防忘 → ARQ-261026-final-phase-dispatch-contract-unwritten。
- 2026-10-26 human 反馈：发布 PR 门面「没写改了啥」→ ARQ-261026-release-pr-gov-lane-zero-pods；PR #40 门面已人工重写。
- 本 workstream 走 `/gov-change` 面 loop（不涉 dev-loop 拆包派遣，milestones.jsonl 登记不适用；若后续某波要拆 dev-loop 包，先补里程碑登记再派 portfolio-planner）。
