# landing-verdict · 261024-gov-review-retro-efficiency-instrumentation

verdict: PASS + 3 ADVISORY

- 核验官：landing-verifier（Track B，零上下文独立核验）
- 核验时点：2026-10-26 · worktree `gov/kidpal` · 全部命令亲跑、全部 diff 亲读，未采信 landing-summary 自述
- 结构门（`landing-fidelity-check.mjs`，exit 0）：`ok:true` / `frozen:true` / `packetBinding:true` / `unauthorized:[]` / `claimedNotLanded:[]` / `plannedNotLanded:[]` / `gitDegraded:false`；anchorUnion 四成员 ≡ manifest 四行
- preflight 复跑（V1）：`ok:true` / `section2BodyFound:true` / `anchorCount:4` / `findings:[]` / `advisories:[]`
- 本仓无 `cross-surface-check.mjs`（悬空引用已于 260921 批删除），按「无此门」处理，无 ADVISORY
- **resume 记录**：首轮 verdict 为 FAIL(F1)（检查点失效计数两判据合并）；主线程修订 `.claude/scripts/retro-efficiency.mjs` 后按「同 agent resume」范围复跑 F1 修订面 + V5，F1 关闭（复跑证据见「F1 修订复核」节），verdict 升为 PASS + 3 ADVISORY，放行 merge（merge 仍归 human 门）。

## 六项核验结论

### 1. 逐文件 diff 对照 — ok（4/4）

- **`.claude/commands/loop-retro.md`（T2，改）**：步骤 2 增「另跑 `node .claude/scripts/retro-efficiency.mjs --since <周期起点>` 取本周期效率表」；步骤 5 增「效率表两张」输出项，两组列名（九列 Pod 表 / 七列角色表）与 FROZEN 逐字一致；无私货。
- **`.claude/references/dev-loop-operating-system.md`（T2/T3，改）**：§6 增一句指向脚本并声明「口径以脚本为唯一权威」、游标/触发条款/分流纪律不动；§5d 增三键必在场（model / durationMs / tokenUsage + unavailable 语义 + `modelFlagMismatch`）与 `mainThreadUsage` 单调累计 + `cumulativeNote: 'non-increasing'` fail-open 告警；`harness-reported-zero` 统一为 `reported-zero`（FROZEN §5 第 8 条明文要求，见诚实披露核对 #2）。
- **`.claude/scripts/dispatch-stamp.mjs`（T3，改）**：model 键恒在场（`model` + `modelStatus` + 条件 `modelReason` + 条件 `modelFlagMismatch`，不一致 stderr 告警）；`--duration-ms` 接受非负安全整数或字面量 `unavailable`（`durationStatus` + 条件 `durationReason`），0 → `durationNote: 'reported-zero'`；tokenUsage transcript 路径补 `total = input + output` 强制与 exact 零值 `note: 'reported-zero'`；检查点写前读本包最后一条 `mainThreadUsage.total`，不升 → stderr 告警 + `cumulativeNote: 'non-increasing'`，全部 fail-open（try/catch 包裹、不改 exit code、不阻断派发）。**门判据零改动**——diff 只落在 payload 组装与 duration 解析，既有 `--duration-ms` 非整数的 exit 2 行为与改前等价；pre-cutoff legacy 分支零触碰（与诚实注记 #7 一致）。
- **`.claude/scripts/retro-efficiency.mjs`（T1/T3，新增，untracked）**：脚本主体与 T1/T3 规格逐条对上（二分域分类、模型族映射、角色三分类、覆盖标记、检查点有效性、fail-open、14 条自检）；首轮 FAIL 的检查点计数两判据合并问题已按 FROZEN「判据分开写」修订落全（见「F1 修订复核」节）。

### 2. packet-binding 一致 — ok

- summary packet-id 行 = FROZEN §2（T1–T3）+ §5 验证集，与 PROPOSAL 头部 packet-binding 行一致。
- summary 骨架含 `landing-manifest` 围栏块（尾部四行，与 PROPOSAL 头部逐 token 一致）。
- manifest 四行 ≡ §2 三个 T 的「目标文件」并集 ≡ 结构门 proposalTargets/summaryTargets。
- T3 声明的五件组件（model 在场 / durationMs 在场 / tokenUsage 补零值注记 / 检查点告警 / 分析侧四件）在 diff 与脚本中逐一在场。

### 3. 验证集复跑（§5 共 11 条，全部亲跑）

| # | 结果 | 亲核留痕 |
|---|---|---|
| V1 preflight | 绿 | `ok:true` / `anchorCount:4` / `findings:[]` / `advisories:[]`，anchorUnion ≡ manifest |
| V2 self-check | 绿 | exit 0，14/14 具名断言全 ok（`doc-code-directional` … `governance-data-as-doc`，与 §5 第 2 条十四条名单逐一对应）；F1 修订后复跑仍 14/14 |
| V3 `--since 260829` | 绿 | exit 0；表头恰九列含「其中测试行」；coverage.totalPods=92、attributedPods=75、unattributedPods=17；抽查 `260829-audio-bargein-hw` 行数格为空非 0；行集最小前缀即 260829（边界包无漏排） |
| V4 单 Pod 双向核对 | 绿 | 独立手算 `git diff --numstat 88a73633^1 88a73633`（33 文件）按二分域规则逐文件分类 = **代码 2,109 / 测试 1,336 / 文档 1,998**，与脚本输出逐数一致；token 555,102（4/4 exact）与 `pod-events-aggregate.mjs --change-id` 的 `tokenTotals.total` 一致；agentMs 1,812,962 = 30.2 min |
| V5 角色汇总 | 绿 | builder token 21,134,634；token 占比和 100.000%；planner 34/92、token 11,867,898；时长占比分母 = 派发行 366,930,375 ms（planner 时段与脚本行不并入）；verifier/supervisor 成本格空、不进分母；**检查点两计数均在场**（见 F1 修订复核） |
| V6 模型/角色/路径分类 | 绿 | `unmappedActors:[]`、`unmappedModels:[]`；`outsideKnownRoots` 9 文件 / 198 行；`.claude/verify-gates.json`→doc、`server/deploy/**`/`firmware/**/test/**`→code、`AGENTS.md`→doc、`.codebase-memory/artifact.json`→code 且计入 outside（自检 ⑫/⑪ 双向断言在位） |
| V7 缺值夹具 | 绿 | exit 0；事件含 `model:null`+`modelStatus:'unavailable'`+`modelReason:'transcript-and-flag-missing'`、`durationMs:null`+`durationStatus:'unavailable'`+`durationReason:'fixture'`、`tokenUsage.status:'unavailable'`；夹具根跑 `--json` 覆盖计数相应 +1、builder 合计不含 unavailable |
| V8 零值夹具 | 绿 | exit 0；`durationMs:0`+`durationNote:'reported-zero'`；`tokenUsage {status:'exact', total:0, note:'reported-zero'}` |
| V9 检查点夹具 | 绿 | exit 0；stderr 含 `mainThreadUsage.total 50 is not increasing vs previous 100`；新事件带 `cumulativeNote:'non-increasing'`、`mainThreadUsage.total=50`，写入未被阻断 |
| V10 grep 接线 | 绿 | 两文件各 1 处命中 `retro-efficiency.mjs`；步骤 5 含「效率表两张」与两组列名 |
| V11 落地面 ≡ manifest | 绿 | `git diff HEAD --stat` = 3 改 + 1 新增（untracked），与 manifest 四行一致；其余工作树改动仅 `.codebase-memory/*`（机器再生常驻脏，不属 canonical）与 review 证据目录自身（合法） |

### F1 修订复核（resume，全部亲跑）

- **代码面**：`plannerInvalidKind` 区分 `decreasing`（首末为负）与 `decreasing-adjacent`（首末为正但有相邻倒退）；新增 `plannerFlags(cps)`，每 Pod `planner` 增 `firstLastNegative` / `adjacentDecreasing` / `identical` 三布尔（`analyzePod` 以 spread 并入，逐 Pod 进 `--json`）；聚合层改为按三布尔独立计数——`plannerDecreasing` 保留「首末为负」语义、新增 `plannerAdjacentDecreasing`；markdown 数据缺口行同步。修订面限定于检查点计数区块，未波及其它口径（V3/V5 其余数值复跑逐数不变：totals token 71,270,286 / dispatchMs 366,930,375、builder 21,134,634、占比和 100.000%）。
- **`--json --since 260829` 复跑**：`{totalPods:92, plannerValid:34, plannerIdentical:7, plannerDecreasing:5, plannerAdjacentDecreasing:6, plannerInsufficient:2}`——与核验官首轮独立全量复算（34/7/5/6/2）**逐数一致**，与 FROZEN §5 第 5 条「7 首末相同、5 首末为负、相邻倒退 6」逐数一致。
- **关键 Pod 复核**：`261014-voice-round-entry-exit-fixes` 的 `planner` = `{checkpoints:6, delta:72903, invalid:'decreasing-adjacent', firstLastNegative:false, adjacentDecreasing:true, identical:false}`——首末为正可算（计入 34）且相邻倒退计数在场的双判定同时成立。
- **markdown 复跑**：数据缺口行为「可算 34、首末相同 7、首末为负 5、出现相邻倒退 6、检查点不足 2」——两种计数在 markdown 与 `--json` 均在场。
- **self-check 复跑**：14/14、exit 0。
- **summary 披露同步核对**：V5 行已补「首末为负 5、出现相邻倒退 6（两计数均在 `--json`）」；§3 新增第 9 条记录 F1 修订口径——与实际落地一致，无该记未记。

### 4. 结构门复跑 — ok

亲跑 exit 0，输出见文首；untracked 新文件被正确识别（`proposalTargets`/`summaryTargets` 均含 `retro-efficiency.mjs`，`claimedNotLanded`/`plannedNotLanded` 为空）。

### 5. 真空绿规约 — ok

- 重放类：`retro-efficiency.mjs` 用 `execFileSync('git', [..., \`${sha}^1\`, sha])`（参数数组不走 shell，`^` 无 cmd 转义问题）；`findMergeShas` 对 `--grep` 命中先做 subject/触碰双判据采用，未命中即空集且空集显式落 `unattributedReason`，不产生空输入绿。
- 接线类：V10 核了 grep 命中 + 步骤 5 输出清单含两张表（不止「自检跑了」）。
- 环境红基线：本批未发现涉闸执行层的红，无需基线对比；三个 dispatch-stamp 夹具在干净夹具根（空 `KIDPAL_CLAUDE_PROJECTS_DIR`）复现成功，非真空绿（V9 夹具预置 100 写 50，缺告警实现必红；V7 缺值形态若被静默吞则覆盖计数不 +1）。

### 6. 诚实披露核对 — ok

- 注记 #1（93.3h vs 91.2h）：`--json` planner `spanMs` = 5,599.9 分钟 = 93.33 小时，与 summary 如实一致；表内标注「主线程时段，含人等」、不并入时长占比（markdown planner 行亲读确认）；PROPOSAL 91.2h 为起草期快照，差异来自新增事件拉长墙钟——披露成立。
- 注记 #2（reported-zero 统一）：全 `.claude/**` grep `harness-reported-zero` 零命中；命中仅在 `delivery/archive/changes/**` 历史归档文书，与 summary 声称一致；OS §5d 原句已同步改为 `reported-zero`。
- 注记 #3–#8 逐一核对成立（outsideKnownRoots 口径 9/198、畸形行 2 条如实在「数据缺口」节计数、`<synthetic>` 映射使 `unmappedModels` 空、历史值不追溯、pre-cutoff 分支零触碰、验证范围与实际相符）。
- 注记 #9（F1 修订后新增）：双计数口径披露与实际落地一致（见 F1 修订复核末条）。

## 派遣方特别核对项（①–⑥）

① 单 Pod 手算 2,109/1,336/1,998 与独立逐文件分类**逐数一致**；② planner 34/92、token 11,867,898、占比和 100.000% 复现；③ 三个 dispatch-stamp 夹具全部真实复现（V7–V9）；④ 诚实注记 #1/#2 如实；⑤ dispatch-stamp 改动确为 fail-open、零门判据改动（payload 只增键、既有消费方 `typeof` 守卫与 `|| 0` 兼容）；⑥ untracked 新文件被结构门正确识别。

## ADVISORY（非阻断）

- A1：`no-write-side-effects` 自检对夹具跑的是解析/聚合代表路径而非完整 CLI 管线；内容级 sha256 快照机制本身机械可判，非真空绿，仅记录覆盖面注记。
- A2：`loop-retro.md` 步骤 2 落地句在 FROZEN 字面句之外多带括注「（口径以脚本为唯一权威，本节不复述列定义）」——与 T2 对 OS 侧的要求同语义、方向一致，判为忠实的充实而非私货。
- A3：夹具根（非 git 目录）下跑 `retro-efficiency` 时 git stderr 原样漏到控制台（`fatal: not a git repository`），fail-open 不受影响、真实仓内不触发，属观感噪声。

## 处置

PASS + 3 ADVISORY：F1 已按 FROZEN §5 第 5 条「两种计数都进 `--json`、判据分开写」修订落全，复跑逐数复现；六项核验全过。**放行 merge（merge 仍归 human 门）。**
