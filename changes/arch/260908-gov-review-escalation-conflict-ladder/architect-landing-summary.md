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
2. **V2 新例未落为 driver 自检夹具（降档如实申报，verdict 转绿条件相关）**：「预探测 no-overlap 与 rebase 实撞不一致 → 升 ③」分支在 processPod 代码路径内，merge-driver 自检夹具不驱动 processPod 全流程（与 260908 前批同款边界）——该分支以**代码路径亲核承载**（merge-driver.mjs rebase 冲突 catch 块 warn 分支），driver 自检 20 例**零新增**；FROZEN V2「新增两例」的差距在此如实申报，不在 §2 冒充已测。
3. **gate 本地正则双源留根**：`VERSIONED_ARTIFACT_RE_LOCAL` 与 merge-driver `VERSIONED_ARTIFACT_RE` 同形双源（lib 收编会扩 manifest、未授权），留根注记在 gate 代码头注 + PROPOSAL §6 留根。
4. **V1 第五例（目录面不再触发升级的 gate 级宿主缺位）**：FROZEN V1 第五例「cross-system 目录面不再触发升级（home-p1 形态夹具）」的机械宿主在 **hook 自检**（retired-impl-axis 两例，改判 low 断言）而非 gate 自检——判据宿主随 B1 修正移到了 hook，夹具随之在 hook；gate 侧无对应第五例，由 §10d 路由表 prose 承载（verdict 转绿条件 3 前半）。
5. **登记表 authority 档未机械化（方向差申报，verdict 转绿条件 3 后半）**：§10c 登记表 = authority、`*.vN.md` 正则 = 兜底（A-7 已裁），但 gate/driver 实现里**只有正则兜底在场、登记表无机械读取**——形态判定方向上正则从严兜底（未登记形态 → ③ fail-closed），不会误放行；登记表机械化（gate 读表）留观测后批。

## 1b 侧注：修订统计

6 文件 +135/−29；三脚本自检合计 114 例全绿（69+25+20）；V3 演练两轮（第一轮暴露噪声、第二轮净清单 + tier 正确）。

## 2. 验证集执行记录

| 项 | 结果 |
|---|---|
| V1 gate 自检 | **26/26**（21 旧 + T5 3 例 + ladder 4 例 + **V4 转绿补件 1 例**：overlap-probeout-payload-keys-ok——升级件宿主 = overlap-probe.json，断言相交清单/实撞集/双新增/tier 四键序列化形状防回退） |
| V1 hook 自检 | 69/69（含 retired-impl-axis 两例改判 low 夹具） |
| V2 driver 自检 | 20/20（T7a-g + T8a-c 既有；probe 消费路径代码亲核——**20 例无新增**，与 FROZEN V2「新增两例」的差距见 1b-② 申报） |
| V3 端到端演练 | 临时仓双包抢 v3 → `parallelOverlapProbe` 实跑 tier=union-safe、实撞/交集/双新增三清单净（两轮修正见 1b-1） |
| **V4 升级件内容面** | **降档申报（verdict 转绿条件 1，冻结层追认见 §9 追认行）**：③/④ 级升级件的「逐文件相交清单」宿主 = `overlap-probe.json`（gate 自检 overlap-probeout-payload-keys-ok 断言其形状）；但「双方版本正文摘录锚」与「architect 意见槽」为 ③/④ **流程面**字段——③ 级意见由派发 architect 产、④ 级内容由 planner 组装升级件时拼装，**本批无独立机械宿主/schema**（超本批 G2/G3 面，schema 化留观测后批——对齐 friction-event-payload-schema ARQ 病族克制）。V4 由「probe-out 形状断言 + DEV-GOVERNANCE 内容义务句 + §10d 升级件表」三层 prose+机械承载，非全机判 |
| V5 投影 grep | §10d=1、parallelOverlapProbe=2、消解方案=1、hook cross-system 字面=0、driver 并行相交面=3 |
| V6 回放对照 | home-p1/schedule-p2 形态由 hook 自检 retired 夹具承载（同 diff 形态 → 不再 high）；真跨系统同 hunk 相撞 → ③ BLOCK 由 gate 自检 overlap-tradeoff-block 承载。全量 git 历史重构回放留观测后批（判据换轴面已由夹具级双断言覆盖；git 2.46 满足版本前提——verifier 首验项可销） |
| 结构门 | proposal-consolidation-check ok:true（FROZEN 后）；landing-fidelity-check ok:true（baseRef 4c38ba9） |

## 2b. 追认链记账

human 2026-09-08 AskUserQuestion 显式追认「Track B 降档 + 继续落地」（PROPOSAL 头注已录）；降档归因（设计分叉被立项裁定消费 + 门内改造先例）经 architect R1-D 独立复核维持。

## 2c. R2 角色注记转发

v0.2 修订由判官（architect）按 human 三裁 + 主线程指令机械回灌——作者≠判官的最终保障在 landing-verifier 独立核验（architect-review-r1.md R2 节角色注记原文在案）。
