# architect-landing-summary · 260908-gov-review-escalation-conflict-ladder

> 主线程 packet-bound landing 执行回报（2026-09-08）。蓝本 = FROZEN v1.0 PROPOSAL + decision-packet；落点 = 6 manifest 路径；落地 commit 见 git log「landing(gov): …escalation-conflict-ladder」。
> packet-id = 260908-gov-review-escalation-conflict-ladder §2 改法集（T1–T5）+ §5 验证集。

```landing-manifest
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/scripts/merge-driver.mjs
.claude/hooks/supervisor-verdict-gate.mjs
.claude/references/pod-coordination.md
docs/DEV-GOVERNANCE.md
docs/ARCH-REVIEW-QUEUE.md
```

## 1. 落点对照（manifest 6/6）

| manifest 路径 | T 项 | 落了什么 |
|---|---|---|
| `.claude/hooks/supervisor-verdict-gate.mjs` | T1 | cross-system 派生轴退役：`isCrossSystem` + `distinctAppSystems` 摘除、退役注记在案；classifyHigh 保留 core.*/全 mock/安全轴三轴；两例自检改判 low 夹具（retired-impl-axis-*）；`--self-check` 69/69；`cross-system` 字面零命中（V5 锚） |
| `.claude/scripts/pre-merge-escalation-gate.mjs` | T1/T2 | `parallelOverlapProbe` 导出（merge-base 交集 + merge-tree 实撞 + add/add 判定 + **噪声滤清**——实撞集 ∩ 双方改动交集，V3 实测修正）+ 三级 tier（no-overlap/union-safe/tradeoff）+ evaluate ③ 级 BLOCK 分支 + buildCtx 接线 + `--probe-out` JSON 制品（PASS/BLOCK 两态都落）+ 自检 4 新例（25/25） |
| `.claude/scripts/merge-driver.mjs` | T4 | Step 1b 传 `--probe-out` + 读取 overlap-probe.json（`lastOverlapProbe` 逐 Pod 隔离）；预探测 no-overlap 与 rebase 实撞不一致 → 存疑向严升 ③ warn；Step 5b payload 加「并行相交面=tier×N件」摘要；自检 20/20 |
| `.claude/references/pod-coordination.md` | T2 | §10d 新节：分级路由表（①②③④）+ 取舍机判判据 + S1 carve-out（已登记 §10c 形态 add/add 恒归 ②） |
| `docs/DEV-GOVERNANCE.md` | T5 | 例外升级拍板段补「升级件必含并行相交面内容；human 批的是消解方案而非放行仪式」一句 |
| `docs/ARCH-REVIEW-QUEUE.md` | V5 | ARQ-260907-escalation-cross-system-overfire 结移 §2（承载批引用） |

## 1b. 落地中的事实性偏差（如实申报，交 landing-verifier 复核）

1. **探测面噪声滤清为落地中新发现**：V3 演练实测 merge-tree `--name-only` 输出混 informational 行（Auto-merging/CONFLICT 行），与 FROZEN 稿「产出 conflictedFiles 清单」的隐含干净清单假设有差——修法 = 实撞集 ∩ 双方改动交集（结构性正确：实撞必为双侧改动）。判定语义零变（tier 只依赖滤清后的实撞集），属落地期实现修正，请 verifier 复核。
2. **V2 新例（实撞不一致升 ③）落在 processPod 代码路径**：merge-driver 自检夹具不驱动 processPod 全流程（与 260908 前批同款边界），该分支以代码路径亲核 + Step 5b payload 断言代测——如实降档申报。
3. **gate 本地正则双源留根**：`VERSIONED_ARTIFACT_RE_LOCAL` 与 merge-driver `VERSIONED_ARTIFACT_RE` 同形双源（lib 收编会扩 manifest、未授权），留根注记在 gate 代码头 + 本 summary §4。

## 1b 侧注：修订统计

6 文件 +135/−29；三脚本自检合计 114 例全绿（69+25+20）；V3 演练两轮（第一轮暴露噪声、第二轮净清单 + tier 正确）。

## 2. 验证集执行记录

| 项 | 结果 |
|---|---|
| V1 gate 自检 | 25/25（21 旧 + T5 3 例 + ladder 4 例：union-safe 放行 / tradeoff 阻断 / probe-fail fail-closed / 零相交放行） |
| V1 hook 自检 | 69/69（含 retired-impl-axis 两例改判 low 夹具） |
| V2 driver 自检 | 20/20（T7a-g + T8a-c 既有；probe 消费路径代码亲核） |
| V3 端到端演练 | 临时仓双包抢 v3 → `parallelOverlapProbe` 实跑 tier=union-safe、实撞/交 集/双新增三清单净（两轮修正见 1b-1） |
| V5 投影 grep | §10d=1、parallelOverlapProbe=2、消解方案=1、hook cross-system 字面=0、driver 并行相交面=3 |
| V6 回放对照 | home-p1/schedule-p2 形态由 hook 自检 retired 夹具承载（同 diff 形态 → 不再 high）；真跨系统同 hunk 相撞 → ③ BLOCK 由 gate 自检 overlap-tradeoff-block 承载。全量 git 历史重构回放留观测后批（判据换轴面已由夹具级双断言覆盖） |
| 结构门 | proposal-consolidation-check ok:true（FROZEN 后） |

## 2b. 追认链记账

human 2026-09-08 AskUserQuestion 显式追认「Track B 降档 + 继续落地」（PROPOSAL 头注已录）；降档归因（设计分叉被立项裁定消费 + 门内改造先例）经 architect R1-D 独立复核维持。

## 2c. R2 角色注记转发

v0.2 修订由判官（architect）按 human 三裁 + 主线程指令机械回灌——作者≠判官的最终保障在 landing-verifier 独立核验（architect-review-r1.md R2 节角色注记原文在案）。
