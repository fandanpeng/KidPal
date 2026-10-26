# architect-landing-summary · 261024-gov-review-retro-efficiency-instrumentation

packet-id：FROZEN PROPOSAL §2 改法集（T1–T3）+ §5 验证集 · 评审轮次 R1 BLOCK(F1–F5) → R2 GREEN(v0.2) → R3 BLOCK(F1–F2, v0.3) → R4 BLOCK(F1, v0.4) → R5 GREEN(v0.5) → R6 GREEN(v0.6, 追加角色表与 T3 后按 v0.6 重审) → R7/R8 BLOCK(F1–F2, v0.7/v0.8) → R9 GREEN(v0.9) · change-id 261024-gov-review-retro-efficiency-instrumentation

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 · 只读效率表脚本与固定口径 | `.claude/scripts/retro-efficiency.mjs` | 新增（约 700 行）。九列 Pod 表 + 七列角色汇总表；二分域代码/文档分类；模型族映射；覆盖标记；14 条自检全绿；零写盘 |
| T2 · `/loop-retro` 输出接效率表 | `.claude/commands/loop-retro.md` · `.claude/references/dev-loop-operating-system.md` | 步骤 2 增跑表一句；步骤 5 增「效率表两张」输出项；OS §6 增一句指向脚本并声明口径以脚本为唯一权威 |
| T3 · 支撑效率表的数据规范化 | `.claude/scripts/dispatch-stamp.mjs` · `.claude/scripts/retro-efficiency.mjs` · `.claude/references/dev-loop-operating-system.md` | 写入侧：model / durationMs / tokenUsage 三键恒在场、缺值显式 unavailable、真实零带 note、检查点非单调告警；均为 fail-open。分析侧：模型族映射、角色三分类、覆盖标记、检查点有效性。OS §5d 同步三键语义与单调累计 |

## 2. 验证证据

| 编号 | 命令 | 实际输出关键行 | 结果 |
|---|---|---|---|
| V1 | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261024-gov-review-retro-efficiency-instrumentation/PROPOSAL.md` | `ok:true` / `section2BodyFound:true` / `anchorCount:4` / `findings:[]` / `advisories:[]` | 绿 |
| V2 | `node .claude/scripts/retro-efficiency.mjs --self-check` | 14 条具名断言全 `ok`；末行 `self-check: 14/14 passed` | 绿 |
| V3 | `node .claude/scripts/retro-efficiency.mjs --since 260829` | 92 行 Pod 表；表头九列含「其中测试行」；行数可归因 75、归因不到 17；`--since` 按 changeId 前缀过滤 | 绿 |
| V4 | 单 Pod 双向核对 `261023-honcho-memory-wiring-restoration` | 代码行 2,109 / 其中测试 1,336 / 文档行 1,998；token 555,102（4/4 exact）；agent 30.2 min；与 `pod-events-aggregate.mjs` 与 `git diff --numstat` 手算一致 | 绿 |
| V5 | `node .claude/scripts/retro-efficiency.mjs --json --since 260829` | builder token 21,134,634；角色 token 合计 71,270,286，占比和 100.000%；planner 34/92、token 11,867,898；首末为负 5、出现相邻倒退 6（两计数均在 `--json`）；verifier/supervisor 不在角色行、不计分母 | 绿 |
| V6 | 同上 `--json` | `unmappedActors: []`；`unmappedModels: []`（11 个具名模型 + `<synthetic>` 均映射，另 199 条无记录）；`outsideKnownRoots` 9 文件 / 198 行；所有路径有确定归类 | 绿 |
| V7 | 缺值夹具：transcript 空目录 + `--duration-ms unavailable --token-status unavailable` | 事件含 `model:null` + `modelStatus:'unavailable'` + `modelReason`；`durationMs:null` + `durationStatus:'unavailable'` + `durationReason`；`tokenUsage.status:'unavailable'`；exit 0 | 绿 |
| V8 | 零值夹具：`--duration-ms 0 --token-status exact --token-total 0` | `durationMs:0` + `durationNote:'reported-zero'`；`tokenUsage: {status:'exact', total:0, note:'reported-zero'}`；exit 0 | 绿 |
| V9 | 检查点夹具：预置 `mainThreadUsage.total=100`，再以累计 50 的 fixture transcript 跑 stamp | stderr 含非单调告警；新事件带 `cumulativeNote:'non-increasing'`；`mainThreadUsage.total=50`；exit 0 | 绿 |
| V10 | `grep -c "retro-efficiency.mjs" .claude/commands/loop-retro.md .claude/references/dev-loop-operating-system.md` | 两文件各 1 处命中；命令步骤 5 含「效率表两张」与两组列名 | 绿 |
| V11 | `git diff --stat` 对照 landing-manifest | 4 个目标文件：3 改 + 1 新增；无夹带 | 绿 |

真空绿规约遵守情况：self-check 的 `no-write-side-effects` 先对非空夹具做内容级快照（`path:size:sha256`）再比对；`token-exact-only` 的 unavailable 夹具带 999999 的 total，错实现一并求和必红；`governance-data-as-doc` 为反向断言。

## 3. 诚实注记

1. **Planner 主线程时段与起草期快照有差**：本次全量运行为 93.3 小时（5,599.9 分钟），PROPOSAL §2 起草期快照为 91.2 小时；planner token（11,867,898）与覆盖（34/92）完全一致，差异来自周期内新增事件拉长墙钟。该列已在表内标注「主线程时段，含人等」，不并入时长占比，不作为纯工作时长解读。
2. **零值 note 统一为 `reported-zero`**：原 `dispatch-stamp.mjs` 旗标路径写 `harness-reported-zero`，FROZEN §5 第 8 条要求 `reported-zero`；本次两路径统一为 `reported-zero`，OS §5d 原句同步。全仓无脚本消费旧字符串（grep 仅命中历史归档文档）。
3. **`outsideKnownRoots` 计数口径**：文件数取全量唯一路径（9），行数按各 Pod 归因 diff 求和（198）；同一路径在多个 Pod 出现时行数可重复计入，属诊断计数、不影响分类。
4. **畸形事件行**：本次运行跳过 2 条 malformed 行，已在「数据缺口」节如实计数；未追溯改写。
5. **模型族映射**：`<synthetic>` 归 `synthetic`，使 `unmappedModels` 为空；将来新模型名会先落 `unmappedModels`，需在脚本内补一行映射（分析侧数据，不改结构）。
6. **历史值不追溯**：199 条缺 model、57 条缺 durationMs、57 条缺 tokenUsage、11 条缺时间戳的历史事件按原值处理，只在覆盖标记与 `unmappedModels` / `outsideKnownRoots` 中反映；本批只约束未来写入。
7. **pre-cutoff legacy 面**：`dispatch-stamp.mjs` 的 pre-cutoff 分支仍写 `dispatch-log.jsonl`，`durationMs` 可能为 null；该路径不在两张表的行集内，行为零变更（仅共用解析）。
8. **未跑全量回归**：本批不触 `verify-gates.json`、不新增门；验证范围 = §5 十一条 + 三个 dispatch-stamp 夹具 + 一次全量只读运行。
9. **V5 双计数口径（landing-verifier F1 修订）**：`plannerDecreasing` 保留「首末为负」语义（5），新增 `plannerAdjacentDecreasing`（出现任一相邻倒退，6）；markdown 数据缺口同步为「首末为负 5、出现相邻倒退 6」。两类计数都进 `--json`。

```landing-manifest
.claude/commands/loop-retro.md
.claude/references/dev-loop-operating-system.md
.claude/scripts/dispatch-stamp.mjs
.claude/scripts/retro-efficiency.mjs
```
