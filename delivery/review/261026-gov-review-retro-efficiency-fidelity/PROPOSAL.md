# PROPOSAL · 261026-gov-review-retro-efficiency-fidelity

> **状态：FROZEN v0.4 · 2026-10-26 · human 立项 ack + architect-reviewer R2 GREEN**（凭据行：human 2026-10-26 立项 ack + 五项 OQ 裁定——**OQ1=(a) 四项一 batch · OQ2=(a) pod-events 新增 `pod-started`/`pod-closed` · OQ3=(a) 五值闭集 + planner 初判/fixer 申报 · OQ4=(b) `mergeSha` 纳入 T1 · OQ5=(a) 重复盖章并入 T3**；v0.3 = 按 architect-reviewer R1 `BLOCK(F1..F7)` + N1–N3 修订（见「起草注记」第 11 条）；v0.4 = architect-reviewer **R2 GREEN**，并按 R2 三条 NIT N4–N6 落件（见「起草注记」第 12 条）。**主线程 2026-10-26 凭「human 立项 ack + architect-reviewer R2 GREEN」标 FROZEN。**）
> **承载 ARQ**：`ARQ-261026-efficiency-table-package-tier`（T2 直接消费，按本批裁定可结项）；`ARQ-261014-pod-events-booking-telemetry-fidelity`（T1 行数归因保真（含 OQ4=(b) 的 `mergeSha` 写侧根因修法）、T3 生命周期间隔与重复盖章、T4 fixer 根因分类**部分消费**；其六类实据的其余根因修复仍 deferred，边界见 §6 第 6 条）。T1 的两类行数污染此前未立条，属本批新发现，留根见 §6 第 1 条——本批**不自批登记**。
> **review 轨**：Track B 主路径（触 `.claude/scripts/**`、`.claude/commands/**`、`.claude/agents/**`，命中 `gov-change.md`「轻路径判据」排除清单，**非**瘦制品轻路径；SSOT：`.claude/references/gov-change.md`）。Track A 段判据两条均不命中：非跨系统结构性变更、方向已由 human 2026-10-26 立项 ack 与五项裁定收敛。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T4）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：high（OQ4=(b) 把写侧伸进合并链——`merge-driver.mjs` 的 Step 5b 簿记在**每次 Pod 合并**时运行，是合并链写入面；叠加 `dispatch-stamp.mjs` 的派发写入面与 `pod-events-append.mjs` 的事件类型闭集扩张，三处写入面同批改，向严取 high。缓解：三处写入全部 fail-open（Step 5b 既有「簿记失败一律 advisory、绝不 errorExit」语义不动）、零门判据、零 G1、零契约面）。
> **review profile**：G2+G3（`.claude/agents/**`、`.claude/commands/**`、`.claude/references/**`、`.claude/scripts/**`）。G1 触面为零、契约面为零。
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/agents/fixer.md
.claude/agents/integrator.md
.claude/agents/planner.md
.claude/commands/loop-retro.md
.claude/references/dev-loop-operating-system.md
.claude/scripts/dispatch-stamp.mjs
.claude/scripts/merge-driver.mjs
.claude/scripts/pod-events-append.mjs
.claude/scripts/retro-efficiency.mjs
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：让 `/loop-retro` 那张效率表变成一张**读得懂、算得准、拆得开**的表——每行能看出这个 Pod 是什么档位与什么类型的包（Full / Lite / nano / 治理 / 拆包）、行数是这个 Pod 自己的行数（不掺回同步与批量合并）、时间是真实起包到收口的观察窗并且把 agent 工作与等人等待分开、fixer 的返工花在哪一类根因上可以归类聚合。

**现状差距**（2026-10-26 本 worktree `gov/kidpal` 起草期现跑，命令与数值见 §2 开头）：

- **行数会串包**。`retro-efficiency.mjs` 的 `findMergeShas` 接受「命名的包内合并」**或**「任何触及 `delivery/taskpackages/<id>/` 的合并」（`:149`），于是 gov↔main 回同步合并与多包批量合并都被当成包内合并计入。实测 92 个包目录中 **6 个**被这类合污染；最重的 `261013-lang-config-thinking-fix` 表里是 **60,017 代码行 / 12,231 测试行 / 125,046 文档行**，只认包内命名合并时是 **48 实现行 / 0 测试行 / 2,424 文档行**——差三个数量级。
- **表没有规模基准**。`taskpackage.json.rigor` 已在盘上（91 个 taskpackage 中 full 55 / lite 34 / nano 1 / 缺值 1；有事件的 86 个中 full 50 / lite 34 / nano 1 / 缺值 1），但表里一列都没有，单包 Full、Lite、nano 与治理批混排在同一张表里，token 与行数没有可比的分母。
- **时间把三样东西混在一起**。现在的墙钟是首末事件差：`261019-p3-record-dialogue-guide` 墙钟 1,373.5 分钟而 agent 只有 275.1 分钟，`261018-sd-log-persist` 764.2 对 60.8，`261022-m5-k4-history-corpus-backfill` 1,073.7 对 72.1。25 包窗口里有 **4 个包没有任何派发事件**（`261020-p3-rec-frame-visual` 只有 5 条事件，墙钟 10.8 分钟没有意义）。另有一类可观测失真：同一 `dispatchId` 被重复盖章，窗口内因此重复计入 **932,254 token**（占窗口 token 合计 26,980,469 的 3.5%）。
- **fixer 返工没有根因**。窗口内 fixer 11 次派发、2,245,502 token（占窗口 8.3%），`261019` 合计 854,385（4 次派发）、`261021` 合计 851,701（2 次派发），但没有任何字段能区分「验收缺口 / 实现缺陷 / 环境问题 / 规格歧义」。现有 `friction` 事件的 `frictionType` 是 planner 的过程摩擦枚举（窗口 61 条：over-scan 33 / degraded 14 / scan-miss 5 / stall 1 / scope-drift 1，另 **7 条该字段缺失**），不承载 fixer 根因。

**非目标（本批不做，防 scope creep）**：

- **不做**按档位或类型的自动阈值告警、「正常区间」判定、异常打分。表只给维度，判读由人。
- **不做** 10-24 基线 §五 的全量字段（`capabilityEpoch` / `taskClass` / `gateFires` / `escapeRef` / `diffStats` / `baseRef`·`headSha`）——只取支撑「真实起止 + 人等拆分」的最小集，其余在 §6 留根。
- **不做**跨月或跨纪元的比较、按档位的成本基准结论、任何归因框架。
- **不做**行数的全局合计（261024 批 §6 第 5 条已定「表只按 Pod 读、不加总」，本批保持）。
- **不回填历史**：生命周期时间戳只对**新写入**生效，历史包显示 `unknown`；重复盖章只做读侧识别的剔除计数，不追溯改写事件。
- **不删不改**既有九列与既有 `--json` 字段（只增列、只增键）。

## 2. 问题、根因与改法（T1…T4）

**【硬项 · 起草期实证义务履行声明】** 本节全部行为断言均为 2026-10-26 本 worktree `gov/kidpal` 起草期现跑（命令 + 关键输出 verbatim），非转引：

- **行集与归因基线**：`node .claude/scripts/retro-efficiency.mjs --since 261013 --until 261023 --json` → exit 0；`pods: 25`；`coverage` = `{"totalPods":25,"attributedPods":22,"unattributedPods":3,"exactTokenPods":20,"noTokenPods":5,"modelPods":20,"noModelPods":5,"plannerValid":16,"plannerIdentical":2,"plannerDecreasing":2,"plannerAdjacentDecreasing":3,"plannerInsufficient":0,"outsideKnownRootsLines":25,"outsideKnownRootsFiles":4,"malformedLines":2}`。逐 Pod 数值与 `delivery/retro/2026-10-25-loop-retro.md` §八主表**逐格一致**（含 `261013` 60,017 / 12,231 / 125,046 与 `261019` 墙钟 1,373.5 对 agent 275.1）。
- **`--self-check` 基线**：`node .claude/scripts/retro-efficiency.mjs --self-check` → `self-check: 14/14 passed`、exit 0（十四条具名断言全绿，本批在其上追加新夹具）。
- **git 归因实测**（下节 T1 证据逐条给出原文）：`git log --all --merges --format="%H%x09%s" --grep=<id>` 与 `git diff --shortstat <sha>^1 <sha>`；五个 SHA 均经 `git rev-parse --verify <sha>^{commit}` 核实存在。
- **`rigor` / `sourceType` / `subsystem` 全量普查**：`node -e` 遍历 `delivery/taskpackages/*/taskpackage.json`（94 个目录，`current/` 除外；带 taskpackage 91、带 pod-events 87）；`rigor` = full 55 / lite 34 / nano 1 / 缺值 1；`sourceType` = regression 23 / planning-workstream 18 / delivery-story 14 / human-ad-hoc 13 / proposal 10 / arch-review 5 / 未声明 5 / drift-report 2 / maintenance 1。
- **事件面普查**：87 个事件目录、1,133 条事件、457 条 `dispatch-completed`；`merge-completed` 76 条中带 `payload.mergeSha` 的 **1 条**，另有 **3 条** `merged` 类型事件带 `payload.mergedCommit`（三个 261022 包，均指 `017de34c`，值为 12 位短 SHA）；57 条 dispatch 无 `dispatchId`；同一 `dispatchId` 重复组 17 个 = 8 组真重复盖章 + 9 组 id 复用（语料级重复 token 1,361,546）。
- **preflight 实跑结果**见文末「起草注记」。
- 本节所写的「改法」是设计规格，不是对现状的断言；凡「未实测」项已显式标注。

**【硬项 · 目标文件锚清单】** 每个 T 末尾显式列「目标文件：」行；本批锚并集 **9 个路径**，与头部 landing-manifest 九行逐行一致（v0.2 起无条件锚——OQ2=(a) 使 `pod-events-append.mjs` 转正、OQ4=(b) 使 `merge-driver.mjs` 入批）。缺锚清单 = 门 RED 点名。

### T1 · 行数归因保真：只认包内命名合并，批量合并按 changeManifest 切分

**问题**：行数列会把「不属于这个包的行数」算进这个包，且没有上限意识。两类污染：

1. **回同步合并被当成包内合并**：`085c7427`（subject「merge: main→gov 回同步（main 领先 20 笔：261013-lang-config-thinking-fix + p1-tomorrow-row-sliver-fix 两 dev 包…）」）把 main 上 20 笔提交的整体内容当作 `261013-lang-config-thinking-fix` 的产出；同一包的真正包内合并是 `4746cd4f`（「Merge 261013-lang-config-thinking-fix (merge-driver)」）。
2. **多包批量合并把同一份行数复制给每个包**：`017de34c`（「Merge dev/pod-m2: 261022-diag-log-content + 261022-p3-record-chrome-ghost-fix + 261022-p3-record-front-polish-fix（三包真机轮完成…）」）被三个包各记一次全量。

发生频率：全语料 92 个包目录中 **6 个**命中（见证据的 census），占窗口 25 包的 4 个（`261013`、`261022-diag-log-content`、`261022-p3-record-chrome-ghost-fix`、`261022-p3-record-front-polish-fix`）。

**证据**（2026-10-26 现跑）：

`git log --all --merges --format="%H%x09%s" --grep=261013-lang-config-thinking-fix`：

```
085c74277856ad3ec08039d82edebf4f737ba024	merge: main→gov 回同步（main 领先 20 笔：261013-lang-config-thinking-fix + p1-tomorrow-row-sliver-fix 两 dev 包、deploy-firstroll-reconcile 首铺修复 PR#2、release/261013 PR#1、LF-EOL 齿钉等 gov 收尾簿记；gov 独有笔 a99bcad ARQ 入账保留、反向回同步不动 main）
4746cd4fbf47a66919b319fd6f089af5d46d0094	Merge 261013-lang-config-thinking-fix (merge-driver)
```

`git diff --shortstat <sha>^1 <sha>` 逐笔：

```
085c7427  750 files changed, 181582 insertions(+), 3318 deletions(-)
4746cd4f   32 files changed,   2077 insertions(+),  395 deletions(-)
017de34c   74 files changed,   9467 insertions(+),  282 deletions(-)
5412af06   29 files changed,   3974 insertions(+),  122 deletions(-)
f370cf27   18 files changed,    765 insertions(+),   15 deletions(-)
```

按脚本现行分类规则（T1 二分域 + 测试行）逐笔复算：

| 包 | 现行（named∨touches） | 只认命名合并 |
|---|---|---|
| `261013-lang-config-thinking-fix` | 代码 60,017 / 测试 12,231 / 文档 125,046 | 代码 **48** / 测试 **0** / 文档 **2,424** |
| `260920-m2-charwindow-card-fix` | 代码 536 / 测试 461 / 文档 1,224 | 代码 536 / 测试 461 / 文档 **1,205** |
| `260915-m1-n1b-p1-home-battery` | 2,575 / 1,493 / 3,330 | **同值**（回同步那笔的 1 行被命名合并的 max 吸收） |
| `261022-diag-log-content` | 5,084 / 4,041 / 4,665 | —（唯一合并即批量合并，无从只认命名） |
| `261022-p3-record-chrome-ghost-fix` | 5,084 / 4,041 / 4,665 | —（同上） |
| `261022-p3-record-front-polish-fix` | 5,084 / 4,041 / 4,665 | —（同上） |

**census（全量机械枚举，命令：`git log --all --merges --grep=<id>` × 92 个包目录 + 逐笔 `git diff --name-only`）**：被「非命名但触及本包目录」的合并命中的包**恰 6 个**（上表六行），无其它包；全语料 76 条 `merge-completed` 事件中带 `payload.mergeSha` 的仅 1 条（见 T1 根因与 OQ4）。

**多笔命名合并的现状口径**（`261021-p3-record-cancel-lifecycle` 有两笔命名合并）：现行「逐文件取 max 再求和」得到 2,133 / 1,910 / 2,739，与「逐文件求和」相差 **4 行**；全语料 7 个有 >1 笔命名合并的包里，求和与 max 的差最大到 **1,903 行**（`260921-m3-k4-p1-cjk-dynamic-text`：6,601 vs 4,698），差值集中在被重写的簿记文件（`review-packet.md` / `supervisor-verdict.json` / `pod-events.jsonl` 等）。

**根因**：`findMergeShas` 的判据是「命名 **或** 触及目录」（`.claude/scripts/retro-efficiency.mjs:149`），而「触及目录」在多包批量合并与 gov↔main 回同步两种形态下都会命中且不区分；同时 commit message 是唯一归因键——`merge-completed` 事件虽有记录，但 76 条里只有 1 条带 `mergeSha`，无法作为归因源。逐文件取 max 是为了避免同一文件在多次合并里被重复计，但代价是把它降成了下界且没人知道它是下界。OQ4=(b) 把 `mergeSha` 补进事件是这条根因的治本动作，但治本只能对**新合并**生效：历史事件没有该键，回退路径（猜 commit message）必须长期并存，不能一次切换。

**改法**：

- **合并分类（纯函数，可自检）**：把 `findMergeShas` 里的判据抽成 `classifyMergeSubject(subject, id)`，返回三值：
  - `named`——subject 以 `Merge <id>` 起头，或含 `merge: dev/<id>`（保持现值域，不动匹配串）；
  - `batch`——非 named，但 subject 中出现 **≥2 个** `\d{6}-` 形态的 changeId（现行实例：`017de34c` 的 subject 出现 3 个）；
  - `backsync`——非 named 且非 batch（现行实例：`085c7427` 只出现 1 个 changeId、`ce71df7c`「Merge main → gov/kidpal（回同步…）」0 个、a3982646「chore(gov): 回同步 main…」0 个）。
- **采纳规则**：`named` 恒采纳；`batch` 采纳但只按第 3 条切分；**`backsync` 不采纳**，进 `--json.mergeAttribution[]` 的诊断（`{sha, subject, verdict, filesTouched, reason}`），不参与任何行数。
- **主判据是 subject 分类器，不是 SHA 来源**（F1 修法①）：无论 SHA 来自哪一路（新 `mergeSha` / 历史 `mergedCommit` / commit-message 回退），一律拿该 SHA 所在 commit 的 subject 跑 `classifyMergeSubject` 决定它是 `named` / `batch` / `backsync`。这样事件路径也不能让批量或回同步蒙混。
- **`changeManifest` glob 方言（单源定义，T1 批量切分与 T2 档位判定两处引用同一实现）**：模式**按仓根锚定**（等价 `^…$`）；`**` 匹配任意层级（`dir/**` 匹配零层及以下，`**/x` 可匹配零层）；`*` 只匹配单段内的任意字符、**不跨 `/`**；大小写敏感。脚本内以 `manifestGlobToRegExp(pattern)` 单一实现导出，两处调用同一函数、不各写一份。
- **批量合并切分**（只对 `batch`）：文件归属按三级判据取第一命中——
  1. 路径在 `delivery/taskpackages/<id>/` 下 → 归本包（本包自己的簿记，天然可归因）；
  2. 路径命中**恰一个**参与包的 `changeManifest[]` glob → 归该包；
  3. 命中 ≥2 个 manifest（歧义）或 0 个（无主）→ **不归任何包**，逐路径进 `--json.batchUnattributedPaths[]`，并计入数据缺口节。
  参与包集合 = 该 `batch` merge 的 subject 中出现的 changeId 集合 ∩ 本表行集。
- **多笔合并仍取逐文件 max**，但把它**显式定性为下界**：`--json` 增 `mergeCount` 与 `multiMerge: true`，表注写明「同一文件在多次合并中被改写时取 max，故多笔合并的口径是下界，不是求和」。
- **归因不到 → 留空**：`unattributedReason` 取值域扩为 `no-merge-commit`（现值）/ `batch-merge-unattributable`（批量合并切分后零文件）/ `merge-diff-unavailable`（现值）。
- **落地效果（起草期按上述规则手算，供判官 spot-check）**：`261013-lang-config-thinking-fix` = 48 / 0 / 2,424；`261022-diag-log-content` = 354 代码 / 0 测试 / 1,096 文档（18 文件）；`261022-p3-record-chrome-ghost-fix` = 280 / 0 / 1,585（20 文件）；`261022-p3-record-front-polish-fix` = 1,228 / 961 / 1,403（19 文件）；三者合计 1,862 / 961 / 4,084，与批量合并整体 5,084 / 4,041 / 4,665 的差额进 `batchUnattributedPaths`。**文件账可平衡**：批量合并共 **74 文件**（`git diff --name-status`：**52 新增 A + 22 修改 M**）= 已归因 **57**（18 / 20 / 19）+ 无主 **11**（0 个 manifest 命中）+ 歧义 **6**。三包合计**不再相等**（现状三行同值），这是本条要的核心可见变化。
- **`--self-check` 新增四条（反转实现必须变红）**：① `named-only-drops-backsync`——给定三条 subject（named / batch / backsync），断言 `backsync` 不计入、`batch` 走切分；若实现回退成 `named || touches`，backsync 被计入 → 断言变红。② `batch-split-ambiguous-dropped`——两个包的 manifest 交集里放一个文件、各自独有里各放一个文件，断言交集文件不进任一包且出现在 `batchUnattributedPaths`；若实现把歧义文件派给任一方 → 断言变红。③ `event-sha-union-not-replace`——夹具里一笔命名合并（`Merge <id>`）加一条 `merge-completed` 事件指向另一笔，断言**两笔都在集合里**（F6 并集语义）；若实现用事件替换整条集合 → 命名那笔丢失 → 断言变红。④ `shared-sha-forces-batch`——同一个 SHA 出现在两个夹具包的候选里，且该 SHA 的 subject 写成 `Merge <id>`（分类器本来会判 `named`），断言两包都被**强制**按 `batch` 切分（guard 盖过 subject 结论）；若实现按 subject 的 `named` 全量计入 → 断言变红。
- **表注与 `--json` 同步**：表注追加一句「行数只归因包内命名合并与按 manifest 可切分的批量合并；回同步合并、批量合并中无主或歧义的文件不计入，见 `--json.mergeAttribution` / `batchUnattributedPaths`」。**现有九列与既有 `--json` 键全部保持**，新增键只是增量。
- **写入侧（OQ4=(b) 采纳，根因修法）**：`merge-driver.mjs` 的 Step 5b 簿记函数 `step5bBookkeeping` 在 `merge-completed` 事件的 payload 里增 `mergeSha` 键——值就是该函数**已经算出**的 `git rev-parse HEAD`（全 40 位，非 12 位缩写；同一次调用里它已用于 summary 与 commit message 的 `merged <sha12>`，本改动只是把这个既有事实再写进 payload 一个键）。其余 payload 键（`summary` 等）与整段 `try/catch` 的既有语义**逐字不动**：簿记失败仍一律 advisory warn、绝不 errorExit、不阻塞合并链。
- **写入侧自检（扩 merge-driver 既有夹具，不新开自检入口）**：`merge-driver.mjs --self-check-merge-friction` 里的 T6 夹具**已经**直调 `step5bBookkeeping` 并断言「事件落行 + pathspec 不夹带 staged 残留」（起草期亲核：`merge-driver.mjs:1264-1280`，`step5bBookkeeping` 在第 1273 行被调用）。在该夹具上追加一条断言：事件 payload 的 `mergeSha` ≡ 该夹具仓库的 `git rev-parse HEAD`，且长度为 40 位。漏写或写 12 位缩写 → 断言变红。
- **候选 SHA 三来源（取并集、按 SHA 去重，不是替换）**（F6 修法，取代 v0.2 的「优先级」叙述）：
  1. `merge-completed.payload.mergeSha`——本批新写的正式通道（全 40 位）；
  2. `merged.payload.mergedCommit`——**历史直写通道，本批明确选择消费**（见下一条裁断）：本语料三包各一条、值为 **12 位短 SHA**，读侧用 `git rev-parse <prefix>` 解析成完整 SHA；
  3. `git log --all --merges --grep=<id>`——commit-message 回退通道（历史与无事件包的唯一路）。
  **并集语义（关键）**：事件 SHA **并入** commit-message 分类出的 SHA 集合、按完整 SHA 去重，**不是替换**。若按替换实现，一个「多笔命名合并 + 一条 `merge-completed`」的包在事件到场后会丢掉其它几笔的行数（语料有 7 个多笔命名合并的包，如 `261021-p3-record-cancel-lifecycle` 两笔、`261013-p1-tomorrow-row-sliver-fix` 三笔）；该语义同时写进 §5 第 16 条的断言语境。
- **`merged` / `payload.mergedCommit` 通道的裁断（明确选择：消费，且只读不追认）**：**读侧消费**——它是本语料里 `017de34c` 这笔多包手工合并的**唯一真实记录机制**（`merge-driver` 每次 invocation 只合并单包、subject 恒为 `Merge <id> (merge-driver)`，`merge-completed` 结构上产不出跨包记录），不消费则 261022 三个批量包完全没有事件侧线索；**写侧不追认**——不把 `merged` 加进 `pod-events-append.mjs` 的闭集、不要求未来继续手写该类型、`.claude/references/dev-loop-operating-system.md` §5d 的事件类型正文不增列它，新的 `mergeSha` 通道是唯一正式通道。`--json.mergeAttribution[]` 每条带 `shaSource: 'merge-completed' | 'merged-legacy' | 'commit-message'`，Pod 级另有 `attributionSources` 去重数组；表注写明「`merged-legacy` 是历史直写、非 canonical 事件类型」。
- **`shaSource` 多通道重遇的优先级（N6）**：同一 SHA 可能同时经多个通道进入候选集。去重后 `mergeAttribution[]` 对该 SHA **只出一条**，`shaSource` 取**通道正式性最高**者，顺序写死为 **`merge-completed` > `merged-legacy` > `commit-message`**；Pod 级 `attributionSources` 保留**全部**实际命中的来源（去重数组，不因覆盖而丢失）。实例：`017de34c` 在三个 261022 包里同时经 `merged-legacy`（`merged` 事件）与 `commit-message`（`git log --all --merges --grep`）两路进入候选集，去重后 `shaSource` 取 `merged-legacy`，`attributionSources` = `['merged-legacy', 'commit-message']`。
- **跨包共享 SHA 降为防御性 guard（不是主判据）**（F1 修法①）：同一 SHA 出现在 **≥2 个包**的候选集里（不论来自哪一路）→ **强制按 `batch` 处理**、盖过 subject 的结论。它拦的是「subject 写成 `Merge <id>` 却其实是多包合并」这种事故形态（`merged-legacy` 与手工合并都可能这样），不是批量识别的主路——主路恒为 subject 分类器的 `batch` 值。
- **本语料实测的路径闭环**：`017de34c` 的 subject 含 3 个 `\d{6}-` changeId → 分类器判 `batch`（主判据命中）；同时三个包的 `merged` 事件都指向同一 `mergedCommit` → guard 复核一致。两条路给出同一结论，故 261022 三包走 manifest 切分而不是各吃整批。
- **历史事实与生效边界**：语料 76 条 `merge-completed` 中带 `payload.mergeSha` 的仅 **1 条**；另有 **3 条** `merged` 事件带 `mergedCommit`（三个 261022 包，均指 `017de34c`）。故**现行绝大多数包仍走 commit-message 回退路径**，本节实测数值不变；新 `mergeSha` 通道从本批落地后的**下一次合并**起才开始产出，不回填历史。这既是「本批不改动已落地数据」的边界，也是 §5 第 15/16 条的断言语境。

目标文件：`.claude/scripts/retro-efficiency.mjs`（改） · `.claude/scripts/merge-driver.mjs`（改）

### T2 · 效率表加「档位」列：让 token 与行数有规模基准

**问题**：Pod 表九列里没有一列说明「这是什么规模、什么类型的包」，于是单包 Full、单包 Lite、nano 与治理批排在同一张表里，跨包判读没有分母：`261020-p3-rec-frame-visual`（nano，49 代码行 / 997 文档行）与 `261019-p3-record-dialogue-guide`（full，5,799 / 4,139）在表上长得一样，只有数字大小不同；拆包是否合适（一个 workstream 拆出的 N 个包各自的量级分布）也无从评估。

**证据**（2026-10-26 现跑，`delivery/taskpackages/*/taskpackage.json` 全量）：

- `rigor` 值域 = **full 55 / lite 34 / nano 1 / 缺值 1**（91 个 taskpackage）；限定「有 pod-events 的 86 个」= **full 50 / lite 34 / nano 1 / 缺值 1**；无 taskpackage 的 1 个事件目录（`260920-gov-review-dispatch-telemetry-source`）rigor 不可得。
- 窗口 25 包的档位分布 = full 10 / lite 14 / nano 1（逐个已在 T1 证据的包表列出）。
- **`sourceType` 不是治理信号**（这条改变 ARQ 候选规则的输入前提）：17 个 `sourceType` 取治理侧值（proposal 10 / arch-review 5 / drift-report 2）的包里，**全部 17 个都是业务实现包**（subsystem 前缀 firmware / server / tools，改的是 prompt 承载、honcho 部署、SD 日志等业务面）。`sourceType` 记的是「请求从哪来」，不是「这个包的变更属治理还是业务」——与 human 2026-10-26 原话「其实不是用 source type」一致。
- 91 个 taskpackage 的 `subsystem` 前缀**没有任何一个**以 `.claude` / `docs` / `delivery` 起头 → 当前行集里**治理包命中数为 0**。治理变更走 `/gov-change` loop，产物住 `delivery/review/**`，本来就不进 `delivery/taskpackages/` 的行集。
- 拆包维度可得：`origin.workstream` 非空（即「某 workstream 拆出来的一个包」）在全语料有实据，窗口内如 `261022-m5-n2a-schedule-mechanical-injection`（workstream = `device-memory-overhaul`）。
- `changeManifest` 不能当类型判据：91 个 taskpackage 里 **46 个没有该字段**（早期包早于该字段）。

**根因**：表自建立起只有「值」没有「基准」——261024 批定的九列全部是成本与产出度量，没有任何一列承载「规模/类型」维度；而 `rigor` 一直在 `taskpackage.json` 里，只是没被消费。ARQ 候选规则把 `sourceType` 当治理判据，实测该判据会把 17 个业务包错标成治理批，必须在落地规则里换掉。

**改法**：

- **新增第十列「档位」**，插在 `changeId` 之后（第二列）。取值域固定为五种 + 空：
  - `full` / `lite` / `nano` —— 直接读 `taskpackage.json.rigor` 原值（不做映射、不做大小写归一）；
  - `governance` —— **路径性质判据（F5 后写死的精确措辞）**：`subsystem` 非空**且其全部取值命中治理根**，**或** `changeManifest` 非空**且其全部条目命中治理根**（`changeManifest` 的 glob 按 T1 定义的 `manifestGlobToRegExp` 方言匹配），任一成立即 `governance`；**字段缺失或空一律视为不命中**（不按 `sourceType` 猜、不按 id 猜）。治理根 = `.claude/`、`docs/`、`delivery/`，以及仓根 `AGENTS.md`、`CLAUDE.md`。命中时档位列显示 `governance` 并覆盖第 2 条的 rigor 值（治理批不走五档分流）。**当前行集命中数为 0**：91 个 taskpackage 的 `subsystem` 无一以治理根起头，其中 **46 个连 `changeManifest` 字段都没有**（缺字段按不命中处理，不会误判）——该值是为未来把治理批纳入行集预留，不推测、不硬凑；
  - `full·拆包` / `lite·拆包` / `nano·拆包` —— `origin.workstream` 非空时在档位后加 `·拆包` 后缀（「这是某 workstream 拆出来的一包」），与 `/triage-change` 的「③拆包」同义；
  - **空** —— 无 taskpackage（`260920-gov-review-dispatch-telemetry-source` 型）或 `rigor` 缺值；**不写 `unknown` 字符串、不写 `?`**，留空并在数据缺口节计数。
- **`--json` 增 `tier` 字段**（每 Pod 一对象：`{rigor, governance, slice, workstream, source}`，`source` ∈ `'taskpackage'|'absent'|'missing-rigor'`），既有键不动。
- **数据缺口节（表三）增一行**：`档位：full N / lite N / nano N / governance N / 拆包 N / 留空 N`。
- **`.claude/commands/loop-retro.md` 步骤 5 的列清单同步**：九列改十列，写明「档位」列（`changeId / 档位 / 模型 / 代码行 / 其中测试行 / 文档行 / 文档代码比 / token / agent 分钟 / 墙钟分钟`），并写明档位取值域与「留空不推测」。
- **不做**：按档位分组小计、按档位的「正常区间」、按档位的自动告警、治理维度以外的类型标签（如「业务 / 平台」细分）——human 只要维度，不要判读机器。

目标文件：`.claude/scripts/retro-efficiency.mjs`（改） · `.claude/commands/loop-retro.md`（改）

### T3 · 真实起包/收口时间与「agent 工作 vs 等人等待」拆分（含重复盖章识别）

**问题**：三件相关但不同的失真：

1. **墙钟把 agent 工作与等待混成一个数**。现值 `wallMs` = 首末事件差（`.claude/scripts/retro-efficiency.mjs:219-226`），既包含 agent 工作时长，也包含串行等待与等人。窗口内三处典型：`261019` 墙钟 1,373.5 分钟 / agent 275.1 分钟；`261018-sd-log-persist` 764.2 / 60.8；`261022-m5-k4` 1,073.7 / 72.1。表注虽写了「含人等」，但没有任何一列把「等待」单独拿出来。
2. **事件稀疏时墙钟失去意义**。窗口内 **4 个包零派发事件**（`261020-p3-rec-frame-visual` 5 条事件全是 human-decision + merge-completed，墙钟 10.8 分钟；`261022-diag-log-content` / `-chrome-ghost-fix` / `-front-polish-fix` 各 1 条 `merged` 事件，墙钟为空）。另有 **5 个包无 exact token**（上列四个 + `261013-qq-schedule-write-closure`）。这些格子现在的形状让人以为「这个小包很便宜」，实际是「没数据」。
3. **重复盖章把 token 重复计入**。同一 `dispatchId` 出现多行时，`sumTokens` 把每一行的 exact token 都加起来。

**证据**（2026-10-26 现跑）：

- 墙钟与 agent 的对比：上列三包数值取自 `--since 261013 --until 261023 --json` 逐 Pod 输出（wall 与 agentMs 两列，与 §八主表一致）。
- 零派发 / 无 exact token 的包：逐 Pod 事件枚举，`261020` = `{"human-decision":3,"merge-completed":2}`、三个 `261022` 包各为 `{"merged":1}`；窗口 `exactTokenPods: 20 / noTokenPods: 5`。
- **短派发普查**：全语料 457 条 `dispatch-completed` 中 `0 < durationMs < 60000` 19 条、`durationMs === 0` 15 条（合计 **7.4%**）；`261018`–`261025` 窗口 100 条中 12 条（**12.0%**），逐 Pod 最高 50%（`261022-m5-n2a` 3/6）。**与派发说明的「9%–18%」不一致**：全语料是 7.4%、窗口是 12.0%，逐 Pod 区间 0%–50%——本节以实测为准。
- **重复盖章普查**：同一 `dispatchId` 的多行组全语料 **17** 组；其中「存在 `durationMs===0` 且 token 合计与同 id 非零时长行**完全相同**」的组 = **8** 组、重复计入 token **1,361,546**；**其余 9 组为 id 复用**（同 id 多行 token 各不相同，属另一类失真，见下条）。窗口内重复盖章 = **6** 组、重复计入 **932,254**（= 窗口 token 合计 26,980,469 的 **3.5%**）。典型：`261022-m5-n2a` 的 `aed70f23…` 两行 `[0, 589880]ms` token 均为 277,323。
- **另一类同 id 不同值**（不是重复盖章，是 id 复用）：`261021` 的 `aa7525e5…` 三行 `[2245646, 0, 425770]ms` token 分别 298,710 / 314,239 / 582,909——同一 id 记了三次不同的派发。本批**不合并**这类（合并会真丢数据），只计数上报。
- **57 条 dispatch 完全没有 `dispatchId`**（早期手写事件），本批不参与去重。
- **planner 检查点窗口内 16/25 可算**（首末相同 2、首末为负 2、相邻倒退 3、检查点不足 0），其余 5 包无检查点。

**根因**：三处同源——**没有「包的生命周期」这一等事实**。起包与收口没有落盘的时间戳，只能拿事件首末当代理；而事件密度取决于这个包装了多少 procedure，不取决于包本身跑了多久。重复盖章的根因是 `dispatch-stamp` 对同一 `dispatchId` 无幂等保护（`appendPodEvent` 只 append，不查重），调用侧重试或补记就会多一行。

**改法**：

- **写入侧（最小集两个时点，其余全不做；存储形态按 OQ2=(a) 定为 pod-events 两种新事件）**：
  - **起包时点**：planner 在 `delivery/taskpackages/<id>/taskpackage.json` 首次落盘（Step 5 Draft 落盘）之后，经 `pod-events-append.mjs` 追加一条 `pod-started` 事件（`actor/source = planner`，payload 可为空对象）；**收口时点**：integrator 在 Step 9 closeout 完成（`closeout-check.mjs` 通过）之后、交 planner 跑 supervisor 闸之前，追加一条 `pod-closed` 事件（`actor/source = integrator`）。时间戳用 append 的默认 `new Date().toISOString()`，**不接受调用方自报时间**（自报时间可伪造、且与事件顺序解耦）。
  - **闭集扩张**：`pod-events-append.mjs` 的 `VALID_EVENT_TYPES`（`:29-36`）增 `pod-started` / `pod-closed` 两值；`.claude/references/dev-loop-operating-system.md` §5d 的 eventType 闭集正文（`:130`）同步补这两个类型，并一句写明「起包取最早一条、收口取最晚一条；两者都是观测事实、non-bearing」。
  - **fail-open**：两处写入写不进去只告警，绝不阻断起包、收口或合并；目录不存在时沿用 `appendPodEvent` 既有的 `skipped` 语义（exit 0）。
  - **语义**：起包时间取**最早**一条 `pod-started`、收口时间取**最晚**一条 `pod-closed`（Pod 被 BLOCK 后重开时会有多条收口记录，取最后一条）；同一 Pod 多次记录不改写历史。
- **读侧（`retro-efficiency.mjs`）**：
  - 新增 `podStartAt` / `podCloseoutAt`（ISO 字符串或 null）、`observationWindowMs`、`waitingMs`、`lifecycleSource`（`'event' | 'absent'`——OQ2=(a) 后只有「事件」与「没有」两态）。
  - `observationWindowMs` = 收口 − 起包；任一端缺失 → `null` + `unknownReason`，**判定顺序写死（F7）**：两端全缺 → `no-lifecycle-data`；仅缺起包 → `no-start`；仅缺收口 → `no-closeout`。**绝不回退到首末事件差**——两者语义不同，回退会把旧近似伪装成新事实。
  - `waitingMs` = `observationWindowMs − agentMs`；两者都可得且差 ≥0 才输出，差为负 → `null` + `unknownReason: 'negative-window'`（说明事件时长和超过观察窗，属数据异常，不写 0）。
  - **既有 `wallMs` 与「墙钟分钟」列原样保留**（向后兼容），表注改写为「墙钟 = 首末事件差（旧近似，事件稀疏时低估）；观察窗 = 真实起包→收口，历史包无该数据」。
- **重复盖章识别与剔除**（并入本 T，不单列子项；见 OQ5）：
  - **判据**（严格三条件，全满足才算重复盖章）：同一 `dispatchId`；一行 `durationMs === 0`；该行的 `tokenUsage.status === 'exact'` 且 `total` 与同 id 中某个 `durationMs > 0` 行的 `total` **相等**。
  - **处置**：把该行从 token 合计、`totalDispatches`、`durationCount` 三处剔除，**角色汇总（第二张表）与主表同口径**同步剔除（否则两表 token 合计对不上），并在 `--json.dedupe` 里记 `{restampedRows, restampedToken, affectedIds[]}`；剔除量与剔除后合计一并进数据缺口节。
  - **不满足三条件的同 id 多行不合并**：进 `--json.dedupe.ambiguousIdGroups`（`{dispatchId, changeId, rows, reason: 'distinct-values'}`），只计数、原文保留——那属 id 复用，归 ARQ-261014（`refEventId`/dispatchId 单写）面，本批不裁。
  - **无 `dispatchId` 的行恒不参与去重**（57 条历史事件），在 `--json` 里计 `withoutDispatchId`。
- **输出**：markdown 增**第三张表「时间拆分」**（列固定为 `changeId / 观察窗分钟 / agent 分钟 / 等待分钟 / 数据来源`；`数据来源` = `lifecycle` / `wall-clock-legacy` / `unknown`），而不是把主表加宽到十二列——主表已因 T2 增列，继续加列会让表失去可读性；`agent 分钟` 在该表按 T1 口径保留原值，`--json` 是唯一口径权威。
- **`.claude/commands/loop-retro.md`**：步骤 5 输出清单增「**效率表三张**」（原两张 + 时间拆分表），并写明「观察窗缺失显示 unknown，不显示 0，不倒推」。
- **`.claude/references/dev-loop-operating-system.md`**：§6 现句只说「取四指标 Pod 表与按 Agent 角色汇总表」（`:147`），与 T2 的档位列、T3 的时间拆分表、T4 的根因小表脱节，须同步为「取 Pod 表（含档位列）、按 Agent 角色汇总表、时间拆分表与 fixer 根因小表；口径以脚本为唯一权威」，并一句写明「观察窗数据缺失显示 unknown、不回退到首末事件差」；§5d 的 eventType 闭集正文（`:130`）按 OQ2=(a) 补 `pod-started` / `pod-closed` 两值，并写明起包取最早、收口取最晚、两者 non-bearing。
- **`--self-check` 新增三条（反转必红）**：① `lifecycle-window-unknown-not-zero`——起包有、收口无的夹具断言 `observationWindowMs === null` 且 `unknownReason === 'no-closeout'`（若实现回退到首末事件差即红）；② `waiting-negative-unknown`——agentMs > 窗口的夹具断言 `waitingMs === null` + `negative-window`（若实现 clamp 成 0 即红）；③ `restamp-dedupe-triple-condition`——造三组：真重复（零时长 + 同 token，应剔除）、同 id 不同 token（应保留）、无 dispatchId（应保留），断言三组各走各路（若实现按「同 id 就剔」一刀切，第二组会被误剔 → 红）。

目标文件：`.claude/scripts/retro-efficiency.mjs`（改） · `.claude/references/dev-loop-operating-system.md`（改） · `.claude/agents/planner.md`（改） · `.claude/agents/integrator.md`（改） · `.claude/commands/loop-retro.md`（改） · `.claude/scripts/pod-events-append.mjs`（改）

### T4 · fixer 派发补可选触发根因分类

**问题**：fixer 是唯一「花了钱但说不清为什么花」的角色。窗口内 fixer 11 次派发、2,245,502 token（占窗口 token 合计 8.3%），其中 7 个包有 exact 值；`261019-p3-record-dialogue-guide` **合计** 854,385（4 次派发）、`261021-p3-record-cancel-lifecycle` **合计** 851,701（2 次派发）——这两个包占了窗口 fixer token 的 76%，但没有任何字段能说明这两笔是「验收缺口」「实现缺陷」「环境问题」还是「规格/契约歧义」。下一次复盘只能重读 pod-retro 猜。

**证据**（2026-10-26 现跑）：

- `node .claude/scripts/retro-efficiency.mjs --since 261013 --until 261023 --json` → `roles` 中 `{"role":"fixer","dispatches":11,"token":2245502,"durationMs":11903014,"podsWithToken":7,"podsWithDuration":7}`；逐 Pod 明细 `261019` 854,385 / 4 次、`261021` 851,701 / 2 次。
- **现有字段不承载根因**：`friction` 事件的 `frictionType` 闭集是**五值** `stall | scope-drift | scan-miss | degraded | over-scan`（SSOT `.claude/references/dev-loop-operating-system.md:122`，**闭集里没有 `unknown`**）；窗口 61 条实测 = over-scan 33 / degraded 14 / scan-miss 5 / stall 1 / scope-drift 1，另 **7 条 `frictionType` 字段缺失**（是落账缺字段，不是枚举值——retro 报告把它归入 `unknown` 桶只是聚合器的兜底显示）。全部取值都是「planner 观察到什么过程现象」，不是「这次 fixer 为什么被派」。
- **dispatch payload 无分类槽**：`dispatch-stamp.mjs:366-392` 的 payload 键集为 `dispatchId / phase / durationMs / durationStatus / toolUses / terminalStatus / tokenUsage / model / modelStatus / contextSource / expectCheck`（+ 条件键），没有任何根因位；457 条 dispatch 中带 fixer 根因的 **0 条**。
- **历史形态不统一**：语料里另有 2 条 `fixer-round` 类型事件（`260911-render-pages`），该类型不在 `pod-events-append.mjs` 的闭集里，属早期直写；本批不追认该类型、也不复用。

**根因**：派发信封只记「谁、什么时候、花了多少」，不记「为什么」。要区分根因只能在派发那一刻问一句，事后从 pod-retro 散文反推不可机械聚合。另有一个必须守住的设计约束：**不能为此新开第二个账本**——`friction` 已经承载 planner 的过程摩擦，若再加一个「fixer 原因」事件类型，同一次返工会在两处各记一笔、口径还会打架。

**改法**：

- **写入侧（`dispatch-stamp.mjs`）**：新增**可选**旗标 `--root-cause <enum>`。
  - 枚举闭集（脚本内单源导出 `FIXER_ROOT_CAUSES`）：`acceptance-gap`（验收缺口：验收没钉住或钉反了）· `implementation-defect`（实现缺陷）· `environment`（环境/工具/设备问题）· `spec-ambiguity`（规格或契约歧义）· `other`。
  - 传入合法值 → payload 写 `rootCause: <value>` + `rootCauseStatus: 'exact'`；传入非法值 → **fail-open**：stderr 一条 advisory、payload 写 `rootCause: null` + `rootCauseStatus: 'invalid'` + `rootCauseNote`，**不阻断派发、不改 exit code**（与 `--model` / `--duration-ms` 的既有 fail-open 姿势一致）；不传 → 不带 `rootCause` 键（向后兼容，既有调用方零变更）。
  - **仅在 `--actor fixer` 时记录**：非 fixer 派发即使传了也只写 `rootCauseStatus: 'ignored-non-fixer'`，防止把根因标签滥用到 builder / integrator 上。
- **写入侧（`.claude/agents/planner.md`）**：在 `[DISPATCH-STAMP]` 条与 Step 10 的 fixer 派发纪律里各加一句——「派 fixer 时按本轮怀疑的根因传 `--root-cause`；拿不准时不传（留空优于猜）」；并把枚举取值列进派发模板一行。**不新增门、不新增必填项**。
- **写入侧（`.claude/agents/fixer.md`）**：`Verification / 交付证据` 节加一句——「返回时申报本轮根因类别（取 planner 派单给出的枚举，或 `other` 并一句说明）；这是申报不是判分，planner 可据证据改判」。这样 producer 闭合成「planner 派发时初判 + fixer 返回时申报 + planner 落章」一条链，不需要第二个账本。
- **读侧（`retro-efficiency.mjs`）**：新增 `fixerRootCause` 聚合（只统计 `actor === 'fixer'` 且 `payload.rootCauseStatus === 'exact'` 的行，按 `rootCause` 分组：`{rootCause, dispatchCount, token, durationMs, pods}`）；markdown 增一张**小表「fixer 根因分布」**（列：`根因 / 派发次数 / token / agent 分钟 / Pod 数`），**仅当至少一行有根因时打印**；无数据时打印一行「fixer 根因：0/11 派发带根因（本批之前的历史派发无此字段）」——历史缺失显示为「无数据」，不写 0。
- **读侧（`.claude/commands/loop-retro.md`）**：步骤 5 输出清单增「fixer 根因分布小表（无数据时打印缺失行数，不补 0）」。
- **与 `friction` 事件的关系（必须写进脚本注释与表注，避免双记）**：`friction` = planner 视角的**过程摩擦**，逐条叙事、跨角色；`rootCause` = **单次 fixer 派发**的归因标签，一派发一值。两者粒度不同、生产者不同、不互相对撞、禁止 join 计数（不得把「有个 acceptance-gap 的 fixer 派发」折算成一条 friction，反之亦然）。若某一轮 fixer 同时被记了 friction，两处各记各的语义，复盘时并列读、不相加。
- **不做**：不给根因加门、不做根因分布的时间趋势、不做「哪类根因最贵」的结论、不引入自由文本标签（自由文本只允许出现在 `rootCause === 'other'` 时的 `rootCauseNote`，且不进聚合）。
- **`--self-check` 新增一条（反转必红）**：`root-cause-enum-closed`——合法值写 `exact`、非法值写 `invalid` 且不写 `rootCause`、非 fixer actor 传值写 `ignored-non-fixer`；若实现把非法值原样写进 payload 或对非 fixer 也记录根因 → 断言变红。

目标文件：`.claude/scripts/dispatch-stamp.mjs`（改） · `.claude/scripts/retro-efficiency.mjs`（改） · `.claude/agents/planner.md`（改） · `.claude/agents/fixer.md`（改） · `.claude/commands/loop-retro.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G3 scripts | `.claude/scripts/retro-efficiency.mjs` | 改（T1–T4） | 只读聚合器：合并分类与批量切分、档位列、生命周期窗口与等待、重复盖章剔除、fixer 根因聚合；仍 stdout / 零写盘 |
| G3 scripts | `.claude/scripts/dispatch-stamp.mjs` | 改（T4） | 派发写入面：新增可选 `--root-cause`；非法值 fail-open 告警 |
| G3 scripts | `.claude/scripts/pod-events-append.mjs` | 改（T3） | `VALID_EVENT_TYPES` 增 `pod-started` / `pod-closed` 两值（OQ2=(a)） |
| G3 scripts | `.claude/scripts/merge-driver.mjs` | 改（T1） | Step 5b `merge-completed` payload 增 `mergeSha`（OQ4=(b)）；既有 advisory 语义与 try/catch 逐字不动 |
| G2 commands | `.claude/commands/loop-retro.md` | 改（T2/T3/T4） | 步骤 5 输出契约：九列→十列、增时间拆分表与 fixer 根因小表 |
| G2 references | `.claude/references/dev-loop-operating-system.md` | 改（T2/T3） | §6 的 `retro-efficiency` 输出指针句同步（现为「Pod 表与角色汇总表」，与新增的时间拆分表 / 根因小表 / 档位列脱节）；§5d 事件类型枚举（`:130`）补两个生命周期类型 |
| G2 agents | `.claude/agents/planner.md` | 改（T3/T4） | 起包时点写入（Step 5 后）+ fixer 派发传根因 |
| G2 agents | `.claude/agents/integrator.md` | 改（T3） | 收口时点写入（Step 9 closeout 后） |
| G2 agents | `.claude/agents/fixer.md` | 改（T4） | 返回时申报根因类别（申报，不判分） |
| 承重未触 | `.claude/hooks/verify.mjs`、`supervisor-verdict-gate.mjs`、`supervisor-gate-check.mjs`、`pre-merge-escalation-gate.mjs`、`closeout-check.mjs` | 未触 | 门判据与合并链判定零改动；不新增工程门、不加牙 |
| 承重未触 | `.claude/scope-map.json`、`.claude/*.settings.json`、`.claude/verify-gates.json`、`.claude/hooks/**` | 未触 | 写域零扩张（新增写入面全部落在 `delivery/taskpackages/**`，planner / integrator 的既有 allow 面内） |
| 未触 | `docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md` | 未触 | 契约面与 spec 零触 |
| 历史制品 | `delivery/taskpackages/**`、`delivery/retro/**`、`docs/debt/**` | 未触 | 不回填历史、不改既有事件、不登记 ARQ（§6 第 1 条留根） |

**【硬项 · sweep 透镜义务】** 本批**不新增门、不新增工程断言**——新增的四类判定 pattern 都是只读分析面或 fail-open 告警：① 合并分类（`named` / `batch` / `backsync`，主判据 = subject 分类器，三路 SHA 共用）与批量合并的文件切分，辅以跨包共享 SHA 的防御性 guard；② 档位与类型归类；③ 重复盖章三条件判据；④ fixer 根因枚举合法性。起草期已用与规格同源的一次性脚本对**全语料**预跑六类 census（命令与计数见 §2 开头与各 T 证据段）：合并污染 6 包、`rigor` 分布 55/34/1/1、重复盖章 8 组 / 1,361,546 token、id 复用 9 组、根因覆盖 0/457、`mergeSha` 覆盖 76 条中 1 条带值（另 3 条 `merged` 事件带 `mergedCommit`）——判官可逐项 spot-check。三处写入面（`dispatch-stamp` 根因旗标、`pod-events-append` 事件类型、`merge-driver` Step 5b payload）全部 fail-open，且都不改变任何门的判据。词表型枚举（根因五值、档位五值、事件类型八值）在正文已全域列出、无省略显示。

**【硬项 · 复扫模式完备性先验】** 本批**不退役、不改名**任何既有术语、列名或事件类型：九列原样保留（只增列）、既有 `--json` 键原样保留（只增键）、`friction` 枚举不动、`unattributedReason` 只扩值域不删旧值（`no-merge-commit` / `merge-diff-unavailable` 仍是合法取值）。故无复扫对象，本节不适用。

## 4. human 取舍点（OQ）

> 五项均已于 2026-10-26 由 human 拍定（立项 ack 同批），以下为**裁定记录**；备选项与代价保留作依据，正文（§0、§2、§3、§5、§6）已按裁定改写。

**OQ1 · 是否四项一 batch → 裁定 (a) 四项一 batch**。备选 (b) 拆成「读侧 T1+T2 / 写侧 T3+T4」两批、(c) 只拆 T3。裁定理由：读侧三处（T1/T2/T3 的读侧）改的是同一个函数的同一段输出，拆开必然两次同文件改动 + 两次全量复算；写侧各项已是可选字段 + fail-open。

**OQ2 · T3 生命周期时间戳的存储形态 → 裁定 (a) pod-events 新增 `pod-started` / `pod-closed` 两种事件**。备选 (b) `taskpackage.json` 两字段（非 append-only、需同步 current/ 投影）、(c) sidecar `pod-lifecycle.jsonl`（多一个文件与写入实现）。裁定理由：起包/收口本就是 Pod 生命周期事件，与 dispatch / merge 同属一个 ledger 才读得齐；已有 `pod-events-append.mjs` 的 CLI 写入通道，零新脚本、零新文件。**连带影响已落正文**：`pod-events-append.mjs` 由条件锚转为**无条件锚**（manifest 加行、§2 T3 锚行去「条件」注记）、`dev-loop-operating-system.md` §5d 的 eventType 闭集正文（`:130`）同步补两值。

**OQ3 · fixer 根因的粒度与生产者 → 裁定 (a) 五值闭集 + planner 派发初判、fixer 返回申报**。取值 = `acceptance-gap` / `implementation-defect` / `environment` / `spec-ambiguity` / `other`；备选 (b) 四值、(c) 受控标签 + 自由文本进聚合（会让表变成不可机判的散文区）。裁定理由：五值覆盖 2026-10-25 轮 fixer 的全部可见形态，`other` 是诚实兜底；自由文本只放在 `other` 的 `rootCauseNote`、不进聚合。

**OQ4 · `mergeSha` 是否纳入本批 → 裁定 (b) 纳入 T1（治本）**。备选 (a) 不纳入、留根 ARQ-261014。裁定理由：与其长期靠 commit message 猜归因，不如把「合并事实」写进事件。**连带影响已落正文**（下句为 v0.2 **裁定时的快照**，读侧机制的现行规格以 §2 T1 为准）：T1 增写侧（`merge-driver.mjs` Step 5b payload 补 `mergeSha`）+ 读侧机制（v0.2 记为「事件优先、批量合并按 SHA 跨包识别、commit message 回退」；**v0.3 起按 R1-F1 修订为「subject 分类器为主判据 + 跨包共享 SHA 降为防御性 guard + 三来源并集」，见 §2 T1**）；manifest 加 `.claude/scripts/merge-driver.mjs`；§3 该行由「未触」改「改」；§6 第 2 条改写；§5 增第 15/16 条；**riskTier 由 medium 上调 high**（见头部证成）。

**OQ5 · 重复盖章的处理是否并入本批 → 裁定 (a) 并入 T3**。备选 (b) 单列 T5、(c) 只计数不剔除。裁定理由：识别规则已实测（三条件、8 组、1,361,546 token），读侧改动与 T3 同文件同函数；剔除只作用于三条件全满足的 8 组，id 复用的 **9** 组只计数不动。

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. **（pre-commit，起草期已跑）** `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261026-gov-review-retro-efficiency-fidelity/PROPOSAL.md` → 预期 `ok:true` ∧ `findings:[]` ∧ `section2BodyFound:true` ∧ `anchorCount:9`，`anchorUnion` ≡ landing-manifest 九行（v0.2 起无条件锚；四跑轨迹见「起草注记」第 1 条）。
2. **（pre-commit，起草期已跑）** `node .claude/scripts/retro-efficiency.mjs --self-check` → exit 0，基线 `14/14 passed`（本批前状态，作为新夹具的对照组）。
3. **（post-commit）** `node .claude/scripts/retro-efficiency.mjs --self-check` → exit 0，输出含**二十三条**具名断言 = 既有十四 + 新增九：`named-only-drops-backsync`（回退成 `named || touches` 必红）、`batch-split-ambiguous-dropped`（歧义文件派给任一方必红）、`event-sha-union-not-replace`（事件替换整条集合必红）、`shared-sha-forces-batch`（按 subject 的 `named` 全量计入必红）、`tier-column-classification`（`rigor` 原值 / `governance` 覆盖 / 缺值留空三种各走各路）、`lifecycle-window-unknown-not-zero`（回退到首末事件差必红）、`waiting-negative-unknown`（clamp 成 0 必红）、`restamp-dedupe-triple-condition`（按「同 id 就剔」一刀切必红）、`root-cause-enum-closed`（非法值原样写入必红）。夹具先断言非空（真空绿规约）。
4. **（post-commit）** `node .claude/scripts/retro-efficiency.mjs --since 261013 --until 261023` → exit 0；主表表头恰为 **十列** `changeId / 档位 / 模型 / 代码行 / 其中测试行 / 文档行 / 文档代码比 / token / agent 分钟 / 墙钟分钟`；行集 25 行；档位列抽查四个方向——`261016-p3-story-body-sd-cache` = **`full`**（subsystem 与 changeManifest 全在 `firmware/main`，按 T2 规则不进 governance 分支）、`261020-p3-rec-frame-visual` = **`nano`**、`261022-m5-n2a-schedule-mechanical-injection` = **`lite·拆包`**（rigor=lite ∧ origin.workstream 非空）、以及**全表不得出现任何 `governance` 值**（当前行集治理包命中数为 0，出现即规则误判）。
5. **（post-commit）** T1 数值核对：`--json` 下 `261013-lang-config-thinking-fix` 的 `codeLines === 48` ∧ `testLines === 0` ∧ `docLines === 2424`；`261022-diag-log-content` / `-chrome-ghost-fix` / `-front-polish-fix` 三者的 `codeLines` **互不相等**（起草期手算 354 / 280 / 1,228）；三者的 `mergeAttribution[]` 各含一条 `017de34c`、`verdict: 'batch'`、`shaSource: 'merged-legacy'`；`261013` 的 `mergeAttribution[]` 含 `085c7427` 且 `verdict: 'backsync'`（**不计入**）；`batchUnattributedPaths` 条目数 = **11（无主）+ 6（歧义）共 17**，且「已归因 57 + 17 = 74」与 `git diff --name-status 017de34c^1 017de34c` 的文件总数吻合（52 A + 22 M；以 FROZEN 时点重跑为准）。
6. **（post-commit）** T3 时间拆分：`--since 261013 --until 261023` 的「时间拆分」表对全部 25 行输出 `观察窗分钟` 为空、`数据来源` = `unknown`（历史包无生命周期数据）；`261020-p3-rec-frame-visual` 两端全缺，`unknownReason` 应为 **`no-lifecycle-data`**（优先级第一档），不得出现 `no-start` / `no-closeout` 或任何臆造值；`--json` 中 `podStartAt` / `podCloseoutAt` 全为 `null`。
7. **（post-commit）** T3 重复盖章：`--json.dedupe` 在窗口内 `restampedRows === 6`（以 FROZEN 时点重跑为准；起草期实测 6 行 / 932,254 token），且窗口 `totals.token` 比改前同命令**恰好少 932,254**（起草期基线 26,980,469 → 预期 26,048,215）；`ambiguousIdGroups` 非空（至少含 `261021` 的两组）且**未被剔除**。
8. **（post-commit）** T4 无数据路径：`--json.fixerRootCause` 为空对象/空数组，markdown 打印「0/11 派发带根因」一行，**不出现 token 为 0 的根因行**。
9. **（post-commit）** 生命周期写入夹具（`CLAUDE_PROJECT_DIR` 指向临时夹具根）：`node .claude/scripts/pod-events-append.mjs --change-id <夹具 id> --event-type pod-started --actor planner --source planner --payload '{}'` → exit 0，夹具 `pod-events.jsonl` 增一行且 `eventType === 'pod-started'`；同法验证 `pod-closed`；再以闭集反证——传入一个不在八值内的类型（如 `pod-open`）→ 非 0 退出（`appendPodEvent` 抛错、CLI catch → exit 1）且报文列出现行八值，证明闭集扩张到位而未被放开成任意值。
10. **（post-commit）** 根因旗标夹具：`dispatch-stamp.mjs ... --actor fixer --root-cause acceptance-gap` → 事件 `payload.rootCause === 'acceptance-gap'` ∧ `rootCauseStatus === 'exact'`；`--root-cause nope` → exit 0 ∧ stderr 含 advisory ∧ `rootCauseStatus === 'invalid'` ∧ 无 `rootCause` 键；`--actor builder --root-cause environment` → `rootCauseStatus === 'ignored-non-fixer'`。
11. **（post-commit）** `grep -n "档位" .claude/commands/loop-retro.md`、`grep -n "时间拆分" .claude/commands/loop-retro.md`、`grep -n "根因" .claude/commands/loop-retro.md` 各至少一处命中；步骤 5 输出清单写「效率表三张」。
12. **（post-commit）** `grep -n "pod-started\|收口时点\|起包时点" .claude/agents/planner.md .claude/agents/integrator.md` 各至少一处命中；`grep -n "root-cause\|根因" .claude/agents/planner.md .claude/agents/fixer.md` 各至少一处命中。
13. **（post-commit）** 向后兼容：`--json` 输出仍含既有全部顶级键（`ok / generatedAt / scope / coverage / totals / pods / roles / scriptRoles / unmappedActors / unmappedModels`），`pods[]` 每项仍含既有全部键（`changeId / models / codeLines / testLines / docLines / ratio / ratioDocImpl / outsidePaths / unattributedReason / token / tokenExactDispatches / tokenUnavailableDispatches / totalDispatches / agentMs / durationCount / wallMs / planner / actors / malformed / hasEvents`）；主表九个旧列名与顺序不变（新列插在 `changeId` 之后）。
14. **（post-commit）** `git diff --cached --stat` 对照 landing-manifest：落地文件集 ≡ manifest 围栏块**九行**；无夹带、无 scope creep。
15. **（post-commit）** `node .claude/scripts/merge-driver.mjs --self-check-merge-friction` → exit 0；T6 夹具新增断言生效：**在调用 `step5bBookkeeping` 之前**先取 `git rev-parse HEAD` 存为期望值（该函数自身会 commit 簿记并推进 HEAD，调用后取会漂），再断言事件 `payload.mergeSha` ≡ 该期望值且长度 40（若实现写 12 位缩写、漏写、或在调用后取值 → 该自检红）。
16. **（post-commit，合并后观察）** 本批落地后的下一次真实 Pod 合并在 main 上产生的新 `merge-completed` 事件，其 `payload.mergeSha` ≡ 该次合并后 `git rev-parse HEAD`；同时断言历史不被改写——全语料 `merge-completed` 中带 `mergeSha` 的条数仍为「1 + 新增合并数」。**并集语义断言（F6）**：对一个历史多笔命名合并的包（如 `261021-p3-record-cancel-lifecycle`，两笔命名合并）在注入一条指向其中一笔的 `merge-completed` 夹具事件后，`mergeCount` 仍为 2（事件 SHA 并入而不替换集合）；再断言该包 `attributionSources` 同时含 `commit-message` 与 `merge-completed` 两值。

## 6. 留根清单

1. **T1 的两类行数污染未立 ARQ**（本批新发现）：回同步合并污染与多包批量合并污染，实测影响 6 个包、最重处差三个数量级。本批以改法直接收口；**建议主线程按批次终了登记一条 ARQ（或并入 ARQ-261014 追加证据），须 human 批**——architect 不做登记动作。
2. **`mergeSha` 已入批（OQ4=(b)），留根的是历史回填**：写侧（`merge-driver` Step 5b payload 补 `mergeSha`）与读侧（三来源并集 + 逐 SHA 标 `shaSource`）都落在 T1；存量 76 条 `merge-completed` 中仅 1 条带该键，另有 3 条 `merged` 事件带 `mergedCommit`（走 `merged-legacy` 只读通道），**历史包永远走回退路、不追溯补写**，两条路长期并存而不是一次性切换——这是有意的设计，不是未完成项。
3. **10-24 基线 §五 的其余字段不进本批**：`capabilityEpoch` / `taskClass` / `gateFires` / `escapeRef` / `diffStats` / `baseRef`·`headSha` 全部留根——它们分别属于能力纪元台账、任务分类、门开火记录、逃逸归因、四桶行数统计，各自是独立批次，不在本脚本上叠加。
4. **治理批不在行集**：行集仍为 `delivery/taskpackages/**`，治理变更（`delivery/review/**` 的 gov-review / arch-review 批）没有 pod-events、没有 rigor，想给治理批出效率数据需要**扩行集**并定义治理批的成本口径——另立批，本批只在档位列预留 `governance` 取值（当前命中 0）。
5. **`wallMs` 语义不变**：仍为首末事件差、仍单列保留，作为历史包的旧近似；本批不删不改、也不让它冒充观察窗。
6. **ARQ-261014 的消费边界**：本批消费其 ②（行数归因失真——T1 的两类污染 + OQ4=(b) 的 `mergeSha` 根因修法）、⑥（模型/用量失真——T3 重复盖章识别与剔除属读侧；写侧幂等仍留该条）、以及「派发可归因性」的 T4 面；**仍留在该条**：① escalation 闭合链、③ 同决定双记、④ `refEventId` 单写、⑤ 时间戳倒挂、⑥ 的快照刷新根因与 dispatchId 幂等写入。
7. **同 id 不同值的 9 组不合并**：`--json.dedupe.ambiguousIdGroups` 只计数原文保留——这类是 dispatchId 被复用到多次不同派发（如 `261021` 的 `aa7525e5` 三行 token 各不同），合并会真丢数据；修法是写入侧幂等与 id 唯一性，归 ARQ-261014。
8. **历史包不回填**：92 个包目录的生命周期时间戳一律 `null`（`unknown`），不追溯、不猜测、不拿首末事件差补。
9. **多笔合并取 max 是下界**：不改为逐文件求和——求和会把「同一文件被重写」的内容重复计入（实测 7 个多合并包求和比 max 多 4 行到 1,903 行，差值全在簿记文件）。表注与本条共同固定这个口径。
10. **引据时点（自律件）**：本 PROPOSAL 的实测数字与行号引据以 2026-10-26 本 worktree `gov/kidpal` 为准；landing 期若目标文件已被并行批改动（尤其 `retro-efficiency.mjs`、`dispatch-stamp.mjs`、`merge-driver.mjs`），以 FROZEN 时点重核为准，数字漂移须如实订正。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-26 - architect-reviewer

轮次：R1 全面审。判官纪律履行：全仓 canonical 只读，本段为唯一写入面；下列一切事实断言均亲核实测（命令与数值见「亲核留痕」），未采信 PROPOSAL 转述。

verdict: BLOCK(F1..F7)

**Findings**（F1 BLOCKING · F2-F7 SHOULD · N1-N3 NIT 不阻冻结）：

- **F1（BLOCKING · 派单焦点 1 预committed 判定成立）——OQ4=(b) 的批量合并机械识别判据在真实数据上不可达，规格须显式声明主次并裁断 merged/mergedCommit 通道的处置**。
  证据：① 亲核 017de34c（语料唯一多包批量合并）在 delivery/taskpackages/*/pod-events.jsonl 的落点——命中三个包（261022-diag-log-content / -p3-record-chrome-ghost-fix / -p3-record-front-polish-fix），但事件形态是 eventType "merged"、payload 键为 "mergedCommit": "017de34c"——不是 merge-completed、键名不是 mergeSha，且 merged 不在 pod-events-append.mjs:29-36 的闭集内（planner 手工直写）。T1 读侧优先级 1-2 只扫 merge-completed + payload.mergeSha，对本语料唯一真实批量合并完全不触发。② 结构性论证：merge-driver.mjs 每次 invocation 只合并单个 Pod、subject 恒为 "Merge <id> (merge-driver)"（:1554）、Step 5b 每包各落一条事件（:1590）——merge-completed+mergeSha 通道结构上产不出跨包共享 SHA；生产中的批量合并只会走手工合并路径（017de34c 正是 merge-driver 升级后 human 指示的手工合并），落 merged 事件。③ 后果：批量识别实际全靠 subject 分类器（batch = subject 含 >=2 个六位日期前缀 changeId；017de34c subject 含 3 个，分类正确），而夹具 ④ merge-sha-shared-batch-split 与读侧优先级 2 的措辞把一条生产中不会发生的路径当成了批量识别机制来陈述。
  修法方向：① T1 读侧改写为「批量识别主判据 = subject 分类器的 batch 值（事件路径与回退路径共用）；同一 mergeSha 出现于 >=2 包的检测降为事件路径的防御性校验（guard）」；② 显式裁断 merged/mergedCommit 通道：要么读侧增第三来源（扫 merged 事件的 payload.mergedCommit——这恰是本语料真实的批量记录机制），要么一句话写明不消费及原因；③ 夹具 ④ 与 §5 第 3 条措辞随 ①② 对齐。

- **F2（SHOULD）——batchUnattributedPaths 条目数断言错误且内部自相矛盾**。§5 第 5 条「= 54（无主）+ 6（歧义）共 60」与 T1 落地效果「无主 54 文件」实测不可复现：判官按 PROPOSAL 三级判据独立复算 017de34c（diff 74 文件 = 52 A + 22 M），归因 57 文件（18/20/19，与 PROPOSAL per-package 文件数逐个一致）+ 无主（0 manifest 命中）11 文件 + 歧义 6 文件 = 17，57+17=74 吻合；而 PROPOSAL 的 57 归因 + 60 无主 = 117 > 74，内部不可能。歧义 6 与 PROPOSAL 一致。修法：54 -> 11（或 FROZEN 时点重跑订正），§5 第 5 条与 T1 落地效果同步改。
- **F3（SHOULD）——「id 复用的 11 组」计数错误**。OQ5 与 §6 第 7 条均写 11 组；实测（per-package 同 dispatchId 多行组逐组枚举）= 17 组中三条件真重复 8 组 -> 歧义 9 组（逐组列表见亲核留痕 6）。修法：两处 11 -> 9。
- **F4（SHOULD）——frictionType 闭集引述失真**。T4 证据段写闭集含 unknown 六值并引 dev-loop-operating-system.md:122；亲核 OS:122 实文为五值 {frictionType: stall|scope-drift|scan-miss|degraded|over-scan}，无 unknown；窗口普查的「缺值 7」是字段缺失、非枚举值。修法：引述改五值、「缺值」单列；T4 改法本体不动。
- **F5（SHOULD）——T2 governance 判据两可读 + 空/缺失边界未定义；changeManifest glob 方言未钉死**。「subsystem 或 changeManifest[] 全部落在 …」未定义「全部」是字段级全称还是任一字段整体命中即算；subsystem 缺失 / changeManifest 缺失（91 包中 46 个无该字段）或空数组时算不算命中也未定义。实测按「任一字段整体命中即算（OR）、空/缺失不命中」全语料 governance 命中 0，§5 第 4 条「全表不得出现 governance」两种读法今天都成立——是未来行集扩张时的误判源，今天不是。另：changeManifest glob 的匹配方言（锚定根、** 跨段、* 单段）在 T1 批量切分与 T2 各消费一次，判官用标准方言复算 T1 切分逐格吻合（证明起草用了等价方言），但实现须钉死语义。修法：改写为「subsystem 非空且全部取值命中治理根，或 changeManifest 非空且全部条目命中治理根，任一成立即 governance；字段缺失或空视为不命中」；glob 方言一处定义、两处引用。

- **F6（SHOULD）——读侧事件/回退两路的合并集语义未定义**。T1 读侧优先级 1「带该键的 SHA 直接当作本包的合并事实」未说明事件 SHA 是替换整条合并集还是并入（与 git log 分类结果按 SHA 去重合并）。全语料 7 个多笔命名合并的包（如 261021-p3-record-cancel-lifecycle 两笔）在落入首条 merge-completed 事件后若走替换语义会丢历史合并行数。修法：明确「事件 SHA 并入 commit-message 分类 SHA、按 SHA 去重」或等价语义，并写进 §5 第 16 条断言语境。
- **F7（SHOULD）——unknownReason 优先级未定义且 §5 第 6 条期望值可疑**。T3 列出 no-start / no-closeout / no-lifecycle-data 三值但未给判定顺序（两端都缺归哪个？）；§5 第 6 条对 261020-p3-rec-frame-visual（历史包、两端全缺）写「不得是 no-closeout 以外的臆造值」——按三值语义两端全缺的自然值是 no-lifecycle-data，按该断言字面却要求 no-closeout。修法：定义优先级（如两端缺 -> no-lifecycle-data；仅缺起 -> no-start；仅缺收 -> no-closeout），§5 第 6 条期望值随改。
- **N1（NIT）——行号引据微漂**：retro-efficiency「:144（named || touches 判据）」——:144 是 named 定义行，表达式实际在 :149；merge-driver「:1063-1072（envelope 与 payload）」实际 envelope 起于 :1060、「:1074-1076（try/catch advisory）」catch 体实际至 :1078。语义均对，落件时顺手校。
- **N2（NIT）——§5 第 15 条夹具断言取值时点**：「payload.mergeSha ≡ 夹具仓库 git rev-parse HEAD」须在调用 step5bBookkeeping 之前取 HEAD——该函数自身会 commit 簿记文件、推进 HEAD（T6 夹具 :1273 调用后 :1275 已在 show HEAD）。实现时先取后调即无歧义，建议把「调用前」写进断言文本。
- **N3（NIT）——§0 措辞**：「261019 一次 854,385、261021 一次 851,701」的「一次」与 T4 证据段「854,385（4 次派发）/ 851,701（2 次派发）」不一致，建议 §0 改「合计」。

**派单焦点逐项结论**（不另立 finding 的项）：

- 焦点 2（新事件类型牵连面）：manifest 未漏消费方。亲核 sweep：pod-events-aggregate.mjs（仅等值过滤 dispatch-completed / agent-escalation / friction）、loop-health-report.mjs（零 eventType 引用）、closeout-check.mjs（零 eventType 引用）、loop-metrics.mjs（仅 dispatch-completed）、render-roadmap.mjs / governance-gate-shared.mjs / pre-merge-escalation-gate.mjs（仅 human-decision）——全部按 eventType 等值过滤、无「未知类型告警 / 计数 / 状态机」假设，pod-started / pod-closed 落地对它们零影响。
- 焦点 3（Step 5b 实际形态）：:1059 全 40 位 git rev-parse HEAD、payload 现仅 summary 一键、try/catch advisory 语义（:1049 注释与 :1076-1078 实现一致：「簿记失败一律 advisory、绝不 errorExit」）、T6 夹具 :1264-1280 直调 step5bBookkeeping 在 :1273——均与 PROPOSAL 一致；--self-check-merge-friction 入口在 :1647 在场。增 mergeSha 键不破坏 advisory 语义；PROPOSAL 行号微漂见 N1。
- 焦点 4（重复盖章三条件误剔风险）：评估为可接受残余风险——误剔需「同一 dispatchId 内某零时长行 token 总数与某非零时长行逐位相等」，作为瞬时失败的自然重合概率极低；且剔除全程留痕 --json.dedupe、角色表同口径同步（两表不会对不上）、id 复用组只计数不动。不判 finding。
- 焦点 6（T4 fail-open 同款性）：亲核 dispatch-stamp.mjs 既有姿势——modelFlagMismatch advisory（:337）、telemetry 未命中 fail-soft 不改 exit code（:357-364）、条件键「不传则不带」（:343 注释）——--root-cause 的 invalid / ignored-non-fixer / 不带键三态与既有姿势一致。
- 焦点 7（验证集可执行性）：第 9 条 exit code 亲核为 1（非法 eventType -> appendPodEvent throw -> CLI catch -> process.exit(1)，:145-148；缺旗标/坏 payload 是 exit 2，两值不混）。第 7 条「恰好少 932,254」亲核成立（窗口三条件重复行 exact token 合计恰 932,254）。第 5 条的 354/280/1,228 与互不相等成立，60 见 F2。第 16 条当前基线「带 mergeSha 1 条」成立。第 3/8/10-13/15 条机械可判；第 13 条键清单与实跑逐键一致。
- 焦点 9（结构字段）：manifest 围栏块九行在场且与 §2 四条「目标文件：」并集逐路径一致（判官自行取并集核对）；packet-id 行在场；Status 行五项裁定与 human 2026-10-26 凭据逐项一致（OQ1=(a) / OQ2=(a) / OQ3=(a) / OQ4=(b) / OQ5=(a)）；§7 本段 append 前为空；landing preflight 复跑 ok:true / section2BodyFound:true / anchorCount:9 / findings:[] / advisories:[]。

**亲核留痕**（判官实测，2026-10-26 本 worktree gov/kidpal）：

1. 基线全跑 --since 261013 --until 261023 --json，exit 0：coverage 逐字段、totals.token 26,980,469、fixer 11 次 / 2,245,502 token / 11,903,014ms、261013=60,017/12,231/125,046、261019 wall 82,408,948ms=1,373.5min / agent 16,504,464ms=275.1min、261018=764.2/60.8、261022-m5-k4=1,073.7/72.1、261020=49/997 / wall 10.8min / 0 派发——与 §0/§2 逐格一致。
2. --self-check -> 14/14 passed、exit 0。
3. 五 SHA shortstat 逐字一致（750/181582/3318、32/2077/395、74/9467/282、29/3974/122、18/765/15）；subject 逐字一致（:73 的「齿钉」系 commit subject 原文，逐字证据保留正确）。
4. 261013 只认命名合并 = 48/0/2,424：判官以脚本同款分类规则独立复算 4746cd4f numstat，逐格一致。
5. 批量切分复算（标准 glob 方言）：354/1,096/18 文件、280/1,585/20、1,228/961(test)/1,403/19 逐格一致；无主/歧义计数见 F2。
6. 重复盖章：per-package 同 id 多行组 17、三条件真重复 8、歧义 9（260829-schedule-reminder / 260829-session-logic / 260831-ui-art-assets / 260911-render-pages / 260920-m2-charwindow-display-fix / 260924-m4-k1-story-cloud-link 两条 / 261021-p3-record-cancel-lifecycle 两条）；窗口 6 组 / 932,254 token（占 3.46%）；aed70f23 两行 [0, 589880]ms token 均 277,323。
7. rigor 普查：91 包 full 55 / lite 34 / nano 1 / 缺值 1；261016-p3-story-body-sd-cache=full（subsystem firmware+server、manifest 全 firmware/server）、261020=nano、261022-m5-n2a=lite+workstream device-memory-overhaul；无 changeManifest 46 包；governance 命中 0；无 taskpackage 的事件目录确为 260920-gov-review-dispatch-telemetry-source 恰 1 个（另两个无 taskpackage 目录无 pod-events）。
8. 事件普查：1,133 事件 / 457 dispatch / 57 无 dispatchId / durationMs=0 15 条、0<d<60000 19 条（合计 7.4%）；merge-completed 76 条中带 payload.mergeSha 恰 1 条；merged 型事件恰 3 条（见 F1）；friction 窗口 61 条分布 over-scan 33 / degraded 14 / 缺值 7 / scan-miss 5 / stall 1 / scope-drift 1；261020 事件 human-decision 3 + merge-completed 2。
9. merge-driver Step 5b 与 pod-events-append / retro-efficiency 各锚行亲核见焦点 3 与 N1；pod-events-append.mjs :29-36 六值闭集、:64-67 skipped 语义、非法 eventType exit 1 均实。
10. 写域：planner / integrator scope-map allow 均含 delivery/taskpackages/**，universal.bashDenyWrite 不拦 node 脚本写入路径；「写域零扩张」成立；scope-map review 写槽 token（gov-review）在册、批次目录名命中。
11. OS 锚行：:122 frictionType（见 F4）、:130 eventType 六值闭集、:147「四指标 Pod 表与按 Agent 角色汇总表」；gov-change.md 轻路径排除清单实文核对一致。

**判档结论（只升不降；判据条目引用）**：

- 面 = 治理面（触 .claude/{scripts,agents,commands,references}/**）。
- 轻/主判档：gov-change.md「轻路径判据」③ 排除清单明列 .claude/scripts/**、.claude/agents/**、.claude/commands/**——本批 manifest 直接命中三项，恒 Track B 主路径、非瘦制品轻路径（与 PROPOSAL 头部自评一致，判档认可）。
- Track A 段判据：gov-change.md「Track A 段判据」① 方向未收敛——不命中（human 2026-10-26 立项 ack + 五项 OQ 裁定，方向与量级均已裁）；②③ 不适用，不升 Track A 段。
- riskTier high：认可（向严自估成立：merge-driver.mjs Step 5b 属每次 Pod 合并运行的合并链写入面，叠加 dispatch-stamp 派发写入面与 pod-events-append 事件类型闭集扩张，三写入面同批，取严合理）。机械面注记：pre-merge-escalation-gate.mjs 的 riskTier BLOCK 分支（:396-400）读 supervisor-verdict.json、属 dev-loop 档；治理档 PROPOSAL 的 riskTier 无机械消费点——high 在本批是标注与自律性质，不引入额外人签或红队义务。
- 升级 human 项：无——F1-F7 均为规格文本与数值修正，不触方向分叉、不改目标效果定义；按 Track B 评审循环回 architect 修订后复审（R2 聚焦 F1-F7 修订到位性）。

### R2 - 2026-10-26 - architect-reviewer

轮次：R2 复审（轮次纪律：只聚焦 R1 findings F1-F7 + N1-N3 的修订到位性 + 修订是否引入新矛盾，不做全量重核）。判官纪律同 R1：全仓 canonical 只读，本段为唯一写入面；修订到位性逐条亲核，未采信修订声明转述。

verdict: GREEN

**R1 findings 修订到位性逐条核对**（全部落正文，非只落起草注记）：

- **F1（BLOCKING）→ 已按修法方向落实**：① 主判据改写落在 T1 改法正文（「主判据是 subject 分类器，不是 SHA 来源」，三路 SHA 一律跑 classifyMergeSubject）；v0.2 的「读侧优先级 = 批量识别机制」陈述已删，grep 旧叙述仅剩 §4 OQ4 历史裁定记录与起草注记（见 N5）。② guard 定位专条在场（「跨包共享 SHA 降为防御性 guard（不是主判据）」，拦「subject 写成 Merge <id> 却其实是多包合并」的事故形态）+「本语料实测的路径闭环」段。③ merged/mergedCommit 裁断专条在场：消费、只读不追认、写侧不入 VALID_EVENT_TYPES / OS §5d、表注写明「merged-legacy 是历史直写、非 canonical 事件类型」；shaSource 三值与 attributionSources 去重数组定义在场。④ 夹具 ③ 改 event-sha-union-not-replace、④ 改 shared-sha-forces-batch，§5 第 3 条断言名同步、第 5 条补 shaSource 期望——旧夹具名 merge-sha-event-preferred / merge-sha-shared-batch-split 已不在 §2/§5 正文（grep 残留仅在 §7 判官记录与起草注记的历史记述中，合法）。
- **F2 → 落实**：T1 落地效果补文件账「74 = 52 A + 22 M = 已归因 57（18/20/19）+ 无主 11 + 歧义 6」，§5 第 5 条改「11（无主）+ 6（歧义）共 17」并加「57 + 17 = 74 与 git diff --name-status 文件总数吻合」自洽断言——与判官 R1 独立复算逐数一致。
- **F3 → 落实**：T3 证据段补「其余 9 组为 id 复用」、OQ5 改「id 复用的 9 组」、§6 第 7 条改「9 组」——三处全改，17 = 8 + 9 自洽。
- **F4 → 落实**：T4 证据段改五值闭集并明写「闭集里没有 unknown」、缺值 7 单列为字段缺失；§0 同步（去掉 unknown、单列缺失 7）。
- **F5 → 落实**：T2 governance 判据按建议措辞写死（subsystem 非空且全部取值命中，或 changeManifest 非空且全部条目命中，任一成立即 governance；字段缺失或空一律不命中），46 个无 changeManifest 写死为不命中；glob 方言以 manifestGlobToRegExp 单源 bullet 定义（仓根锚定 / ** 跨段 / * 单段 / 大小写敏感），T2 明写引用 T1 的同一实现。
- **F6 → 落实**：「候选 SHA 三来源（取并集、按 SHA 去重，不是替换）」专条在场，替换语义的危害实例（7 个多笔命名合并包）写明；§5 第 16 条增并集断言（注入事件后 mergeCount 仍为 2 + attributionSources 含两来源），与正文「该语义同时写进 §5 第 16 条」互指一致。
- **F7 → 落实**：T3 读侧写死判定顺序（两端全缺 no-lifecycle-data；仅缺起 no-start；仅缺收 no-closeout）；§5 第 6 条期望值改 no-lifecycle-data 并明写不得出现另两值；夹具 ①（起包有、收口无 → no-closeout）与新优先级自洽。
- **N1 → 落实**：:149 三处（§0 / T1 根因 / 起草注记 6，并注明 :144 是 named 定义行）；merge-driver :1060-1071 / :1076-1078 / :1647 均改。
- **N2 → 落实**：§5 第 15 条写明「在调用 step5bBookkeeping 之前先取 git rev-parse HEAD 存为期望值（调用后取会漂）」。
- **N3 → 部分落实**：§0 已改「合计」（两处）；**T4 正文（:212）仍写「261019-p3-record-dialogue-guide 一次 854,385（4 次派发）」**——见 N4。

**修订是否引入新矛盾**（重点亲核）：

1. **merged 通道只读不追认的依赖声明**：已在 T1 专条显式声明（读侧消费 + 写侧不追认 + 表注标注非 canonical），OS §5d 与 pod-events-append 闭集均不增列——依赖是声明过的、有界的，不构成未声明依赖。manifest 九行不变与该裁断自洽（merged 消费在 retro-efficiency.mjs 读侧实现，已在锚内）。
2. **guard 误判风险**：同一 SHA 出现于 ≥2 包候选集才能触发 guard。commit-message 路径下两笔独立单包合并不可能共享 SHA（每包 grep 只命中含本包 id 的 subject）；事件路径共享 SHA 恰是 guard 要拦的事故形态。guard 强制 batch 后，manifest 覆盖不到的文件落 batchUnattributedPaths（不归任何包）——出错方向是「少计」而非「双计」，与 T1 目标效果同向。夹具 ④ 的构造（subject 写 named、靠 guard 翻转）机械可判。不判 finding。
3. **新事实断言亲核**：「261013-p1-tomorrow-row-sliver-fix 三笔」——实测 git log 恰三笔命名合并（d308bd62 / 506b7c3c / d56e1ed6，subject 均 Merge 261013-p1-tomorrow-row-sliver-fix (merge-driver)），成立。
4. **OQ4 裁定记录的连带影响描述仍写 v0.2 机制**（「批量合并按 SHA 跨包识别」）——属 §4 历史裁定记录、非 live 规格，§2 为唯一口径权威，不构成规格矛盾（见 N5）。
5. **与 261024 批 / ARQ-261014 / ARQ-261026**：shaSource / attributionSources / mergeCount / multiMerge 均为新增键，既有键零动（向后兼容第 13 条不受影响）；261024 批的十四夹具为基线、本批叠加不变；ARQ 边界表述（§6 第 2/6/7 条）与 R1 结论一致，无新撞车。

**R2 新增 NIT**（不阻冻结）：

- **N4（NIT）**：T4 正文 :212「`261019-p3-record-dialogue-guide` 一次 854,385（4 次派发）」——「一次」与括注「4 次派发」仍不自洽（§0 同位已改「合计」，此处漏改）。修法：改「合计」，与 §0 对齐。
- **N5（NIT）**：§4 OQ4 的「连带影响已落正文」句仍描述 v0.2 机制（「批量合并按 SHA 跨包识别」），v0.3 正文已改 subject 主判据 + guard。属历史裁定记录、非 live 规格；建议落件时顺手加半句「（读侧机制 v0.3 起按 R1-F1 修订为 subject 主判据 + guard，见 §2 T1）」，或保持原样作为裁定记录的历史快照。
- **N6（NIT）**：shaSource 的多通道重遇优先级未定义——017de34c 经 merged-legacy 与 commit-message 两路同时进入三个包的候选集（去重后留一条），§5 第 5 条期望其 shaSource = 'merged-legacy'，隐含「legacy 事件 > commit-message」但正文未写死。修法：一句话写死优先级（建议 merge-completed > merged-legacy > commit-message，按通道正式性排序），§5 第 5 条断言即完全机械可判。

**结构字段复核**：Status 行 = DRAFT v0.3 且五项 OQ 凭据行原样保留；manifest 围栏块九行未动、与 §2 四条「目标文件：」并集仍逐行一致（判官复核）；packet-id 行在场；§7 的 R1 记录经归一化逐行比对与本判官 R1 append 版本 byte 一致（作者未改一字）；preflight 复跑 ok:true / section2BodyFound:true / anchorCount:9 / findings:[]。

**判档结论**：沿 R1 不变——Track B 主路径（轻路径判据 ③ 命中）、Track A 段不触发（方向已收敛）、riskTier high 认可。升级 human 项：无。

**结论**：F1-F7 全部按修法方向落实且未引入 BLOCKING/SHOULD 级新矛盾；N4-N6 为 NIT、随落件顺手处理即可。R2 通过，主线程可凭「立项 ack + 本 GREEN」标 FROZEN。


---

**起草注记（非判官区）**：

1. **preflight 实跑（四跑：v0.1 首跑 → v0.1 修正 → v0.2 复跑 → v0.3/v0.4 复核，2026-10-26）**：命令 `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261026-gov-review-retro-efficiency-fidelity/PROPOSAL.md`。
   - **v0.1 首跑**：`ok:true / section2BodyFound:true / anchorCount:6 / findings:[] / advisories:[]`。**六而非八的原因**：机门 `parseSection2AnchorLists` 对锚行注记含「条件」的路径按设计裁剪（`lib/governance-gate-shared.mjs:443`「条件锚裁剪：manifest 反映裁断」；规则说明见同件 `:346`），当时两条条件锚的注记都带「条件」→ 被裁掉。
   - **v0.1 修正**：把 `.claude/references/dev-loop-operating-system.md` 转无条件锚、只留 `pod-events-append.mjs` 一个条件锚并置于锚行末位；manifest 收为七行 → 复跑 `anchorCount:7`，七成员与 manifest 逐 token 一致。
   - **v0.2（human 五项裁定后）**：OQ2=(a) 使 `pod-events-append.mjs` 转正、OQ4=(b) 使 `merge-driver.mjs` 入批 → 条件锚归零、manifest 九行。**复跑结果**：`ok:true / section2BodyFound:true / anchorCount:9 / findings:[] / advisories:[]`，`anchorUnion` = `.claude/agents/fixer.md`、`.claude/agents/integrator.md`、`.claude/agents/planner.md`、`.claude/commands/loop-retro.md`、`.claude/references/dev-loop-operating-system.md`、`.claude/scripts/dispatch-stamp.mjs`、`.claude/scripts/merge-driver.mjs`、`.claude/scripts/pod-events-append.mjs`、`.claude/scripts/retro-efficiency.mjs`，与 manifest 九行逐 token 一致、无双向失配。
   - **v0.3（R1 修订后）**：结构未变（无增删目标文件），复跑仍为 `ok:true / section2BodyFound:true / anchorCount:9 / findings:[] / advisories:[]`，`anchorUnion` 与 manifest 九行逐 token 一致——修订只落在 §0–§6 的正文与本节，不改锚集。
   - **v0.4（R2 GREEN + N4–N6 落件后）**：同样 `ok:true / section2BodyFound:true / anchorCount:9 / findings:[] / advisories:[]`，`anchorUnion` 与 manifest 九行逐 token 一致——N4–N6 全部落在正文措辞与新增 `shaSource` 优先级条款上，不触锚集、不改 manifest。
2. **§5 实跑/未跑清单**（起草期）：**已实跑**——第 1 条（preflight，三跑见上）、第 2 条（`--self-check` → `14/14 passed`、exit 0）、以及第 5/6/7 条的**基线侧**（`--since 261013 --until 261023 --json` 全量输出，本文引用的每个数都出自它或与同源一次性脚本的复算）。**未实跑 + 原因**——第 3 条（新夹具尚不存在，须先实现）、第 4 条的列头与新列值（脚本未改）、第 5 条的目标值（`261013` = 48/0/2424 等为起草期**手算**，须实现后由脚本复现）、第 8–13 条（写入面与文面同步未落地）、第 15 条（`merge-driver` 夹具断言尚未加）、第 16 条（须等落地后的下一次真实合并，属**合并后观察**项，不是 landing 前置）。手算与实现若不一致，以 FROZEN 时点实测为准并如实订正。
3. **与派发说明的数字差异（以本 worktree 现跑为准，逐条列出，不采信转述）**：① 「3 个包没有派发事件」→ 实测 **4 个**（`261020-p3-rec-frame-visual`、`261022-diag-log-content`、`261022-p3-record-chrome-ghost-fix`、`261022-p3-record-front-polish-fix`）；② 「1 个包没有 exact token」→ 实测窗口 **5 个**（上列四个 + `261013-qq-schedule-write-closure`，与脚本 `coverage.noTokenPods: 5` 一致）；③ 「Oct18–25 有 9%–18% 的 dispatch durationMs < 1 分钟」→ 实测全语料 **7.4%**（34/457）、`261018`–`261025` 窗口 **12.0%**（12/100）、逐 Pod 区间 0%–50%；④ 重复盖章的实际危害不是「拉低时长中位数」，而是**重复计入 token**（窗口 932,254）并抬高派发计数——`durationMs === 0` 的行对求和贡献为零，但对计数贡献一；⑤ `merge-completed` 事件现为 **76 条**（ARQ-261014 记 69 条为旧时点），带 `payload.mergeSha` 的仍是 **1 条**。以上差异已在 §0 / §2 相应段落按实测改写。
4. **全量普查命令留痕**（均为 `node --input-type=module` 一次性脚本 + `git`，2026-10-26，本 worktree）：行集与 `rigor`/`sourceType`/`subsystem` 普查（94 个目录 / 91 个 taskpackage / 87 个事件目录）；事件面普查（1,133 条事件 / 457 条 dispatch / 76 条 merge-completed / 3 条 merged / 57 条无 dispatchId）；合并归因 census（92 个包 × `git log --all --merges --grep`，命中「非命名但触及目录」的包恰 6 个）；逐笔 `git diff --shortstat` 与逐笔分类复算；批量合并的 manifest 切分试验（74 文件 = 52 A + 22 M：归因 57 / 无主 11 / 歧义 6）；多笔命名合并的 sum-vs-max 对比（7 个包）；重复盖章三条件复算（17 组 = 8 组真重复 + 9 组 id 复用）。上述脚本为起草期一次性分析件，**不入仓、不进 manifest**。
5. **路径形态族谱（`path-form-family.md`）**：本批无搬迁 / 改名 / 路径串改写，查表义务不适用，在此声明。
6. **承重引文 byte 对账**：`.claude/scripts/retro-efficiency.mjs` 的 `:149`（`named || touches` 判据；`:144` 是 `named` 定义行）、`:219-226`（`wallMs`）、`:113-131`（`numstatSum` 逐文件取 max）、`:195-207`（`sumTokens`）；`.claude/scripts/dispatch-stamp.mjs:366-392`（payload 键集）；`.claude/scripts/pod-events-append.mjs:29-36`（`VALID_EVENT_TYPES` 六值闭集）、`:64-67`（目录不存在时 `skipped` 语义）；`.claude/scripts/merge-driver.mjs:1059`（`mergeSha = git rev-parse HEAD`，全 40 位）、`:1060-1071`（`merge-completed` envelope 与 payload，现仅 `summary` 一键）、`:1076-1078`（`try/catch` 的 advisory 语义）、`:1264-1280`（T6 夹具直调 `step5bBookkeeping`，调用在第 1273 行）、`:1647`（`--self-check-merge-friction` 入口）；`.claude/references/dev-loop-operating-system.md:130`（事件类型闭集正文）、`:122`（`frictionType` 五值闭集：`stall|scope-drift|scan-miss|degraded|over-scan`，无 `unknown`）、`:147`（`retro-efficiency` 的输出指针句）；`.claude/references/gov-change.md:18`（轻路径排除清单）。均系起草期逐行 Read 所得，非转述；v0.3 按 R1-N1 复核并订正行号漂移。
7. **长文与自身产物机检**：全文 443 行 = 正文 §0–§6 共 307 行 + §7 判官区 99 行（R1 与 R2 由判官 append，本批作者未改一字）+ 分隔线 1 行 + 起草注记 36 行；`### T<n>` 四条、`目标文件：` 行四条、manifest 围栏块九行、分块哨兵零残留；`**目标文件**` 加粗误形零处。**唯一一处「齿」字在 :73 的 `git log` 逐字输出块内**——那是 commit subject 原文（`LF-EOL 齿钉等 gov 收尾簿记`），属逐字证据、非散文，按「证据引用保持逐字精确」保留，未改写。
8. **判档自估（最终归 architect-reviewer）**：按 `gov-change.md`「轻路径判据」，本批 diff 落在 `.claude/scripts/**`、`.claude/agents/**`、`.claude/commands/**`、`.claude/references/**`——命中排除清单 ⇒ **恒 Track B 主路径，非瘦制品轻路径**。riskTier 自估 **high**（v0.1 为 medium，随 OQ4=(b) 上调）：`merge-driver.mjs` 的 Step 5b 在**每次 Pod 合并**时运行、属合并链写入面，叠加 `dispatch-stamp.mjs`（每次派发写入）与 `pod-events-append.mjs`（事件类型闭集扩张），三处写入面同批改，向严取 high。缓解面：三处写入全部 fail-open（Step 5b 既有「簿记失败一律 advisory、绝不 errorExit、不阻塞合并链」语义逐字不动）、零门判据、零 G1、零契约面、写域零扩张。
9. **scope concern（起草期自查）**：① 每个目标文件在落地期都有合法写者——landing 恒为主线程 packet-bound（主线程不受 per-agent allow 限制，只受 universal 与 settings deny），`delivery/taskpackages/**` 的新写入点落在 planner / integrator 既有 allow 面内；② 与 261024 批不撞车——该批已 FROZEN 落地（`retro-efficiency.mjs` 的十四夹具为基线），本批只在其增量上叠加；③ 与 ARQ-261014 的边界——`mergeSha` 写侧是 OQ4=(b) 明确并入本批的部分，dispatchId 幂等写入与 id 复用仍留根（§6 第 6、7 条）；④ 与 ARQ-261026 的关系是直接消费，若本批 GREEN 且落地，该条可结项（结项登记动作归主线程）；⑤ `merge-driver` 的改动是 payload **加键**，既有消费方零影响（起草期核对：`retro-efficiency.mjs` 现在根本不读 `merge-completed` 的 payload，`pod-events-aggregate.mjs` 只按 `dispatch-completed` 聚合）。
10. **v0.2（human 2026-10-26 立项 ack + 五项裁定，2026-10-26）**：① Status 升 DRAFT v0.2 并写凭据行（五裁 + 待 architect-reviewer 四查）；② **OQ2=(a)** → `pod-events-append.mjs` 由条件锚转无条件锚、T3 写侧改为「`pod-started` / `pod-closed` 两事件 + 闭集扩张 + OS §5d 枚举同步」、`lifecycleSource` 收为 `'event' | 'absent'` 两态；③ **OQ4=(b)** → T1 增写侧（`merge-driver` Step 5b payload 补 `mergeSha`，全 40 位）+ 读侧优先级（事件优先 / 共享 SHA 识别批量 / commit message 回退 / `attributionSource` 留痕）+ 新增两条 read-side 夹具与一条 merge-driver 夹具断言，§3 该行由「未触」改「改」，§5 增第 15/16 条，§6 第 2 条由「未纳入」改写为「历史回填留根」；④ **riskTier medium → high**（合并链写入面）；⑤ 修正 v0.1 的一处规则误用——§5 第 4 条原把 `261016-p3-story-body-sd-cache` 的档位写成 `governance`，但它的 subsystem 与 changeManifest 全在 `firmware/main`，按 T2 规则应为 `full`；已改为 `full` 并补一条「全表不得出现 `governance`（当前命中 0）」的机械断言，同时复核其余两个抽查值（`261020` = nano、`261022-m5-n2a` = lite·拆包）均正确；⑥ 条件锚机制随之退场，manifest 收为九行、§2 锚并集九路径逐行一致。
11. **v0.3（按 R1 `BLOCK(F1..F7)` + N1–N3 修订，2026-10-26）——逐条处置**（§7 判官区一字未动，本轮只改 §0–§6 与本节）：
    - **F1（BLOCKING）**：① **主判据改为 subject 分类器**——新增 bullet「主判据是 subject 分类器，不是 SHA 来源」，明确三路 SHA 一律跑 `classifyMergeSubject` 出 `named`/`batch`/`backsync`；删掉 v0.2「读侧优先级第 2 条 = 批量识别机制」的错误陈述。② **跨包共享 SHA 降为防御性 guard**——新增 bullet，定位为「拦『subject 写成 `Merge <id>` 却其实是多包合并』的事故形态」，明写它不是批量识别主路；并补一段「本语料实测的路径闭环」说明 `017de34c` 是主判据（subject 三个 changeId）命中、guard 复核一致。③ **`merged` / `payload.mergedCommit` 通道的裁断 = 消费（只读不追认）**——新增专条：读侧消费（它是该笔手工合并的唯一真实记录机制，不消费则 261022 三包无事件侧线索），写侧不追认（不入 `VALID_EVENT_TYPES`、不入 OS §5d 枚举、不要求未来续写），`mergeAttribution[]` 每条带 `shaSource: 'merge-completed' | 'merged-legacy' | 'commit-message'`。④ **夹具对齐**——③ 由 `merge-sha-event-preferred` 改为 `event-sha-union-not-replace`（并集语义）、④ 由 `merge-sha-shared-batch-split` 改为 `shared-sha-forces-batch`（subject 写成 named、靠 guard 强制 batch）；§5 第 3 条断言名同步，第 5 条补 `shaSource: 'merged-legacy'` 期望。
    - **F2**：无主文件数 **54 → 11**（T1 落地效果与 §5 第 5 条两处），并补文件账「74 = 52 A + 22 M = 已归因 57（18/20/19）+ 无主 11 + 歧义 6」，使 57+17=74 自洽（判官独立复算的数）。
    - **F3**：id 复用组 **11 → 9**（§4 OQ5 与 §6 第 7 条两处）；T3 证据段补一句「其余 9 组为 id 复用」，把 17 = 8（真重复）+ 9（复用）写清。
    - **F4**：T4 证据段的 `frictionType` 引述改为**五值** `stall | scope-drift | scan-miss | degraded | over-scan`（明写闭集无 `unknown`），「缺值 7」单列为**字段缺失**、并说明 retro 报告的 `unknown` 桶只是聚合器兜底显示；T4 改法本体未动。
    - **F5**：T2 的 `governance` 判据改写为精确措辞——「`subsystem` 非空且全部取值命中治理根，或 `changeManifest` 非空且全部条目命中治理根，任一成立即 `governance`；字段缺失或空一律不命中」，并写死边界「91 个 taskpackage 中 46 个无 `changeManifest`，缺字段按不命中」；**glob 方言单源定义**新增为 T1 的一条 bullet（仓根锚定 / `**` 跨段 / `*` 单段 / 大小写敏感 / `manifestGlobToRegExp` 单一实现），T1 批量切分与 T2 档位判定两处引用同一函数。
    - **F6**：新增「候选 SHA 三来源（取并集、按 SHA 去重，不是替换）」bullet，并写明若按替换实现会丢多笔命名合并的行数（语料 7 个多笔包的实例）；§5 第 16 条增并集断言（注入事件后 `mergeCount` 仍为 2、`attributionSources` 同时含两来源）。
    - **F7**：T3 读侧写死优先级「两端全缺 → `no-lifecycle-data`；仅缺起 → `no-start`；仅缺收 → `no-closeout`」；§5 第 6 条对 `261020-p3-rec-frame-visual` 的期望值改为 **`no-lifecycle-data`** 并明写不得出现另外两值。
    - **N1**：`retro-efficiency.mjs` 的 `named || touches` 引据 `:144 → :149`（§0、T1 根因、本节第 6 条三处；并注明 `:144` 是 `named` 定义行）；`merge-driver.mjs` 的 envelope 引据改 `:1060-1071`、catch 体改 `:1076-1078`，另补 `:1647`（自检入口）。
    - **N2**：§5 第 15 条写明「**在调用 `step5bBookkeeping` 之前**先取 `git rev-parse HEAD` 存为期望值（该函数自身 commit 簿记并推进 HEAD）」，并说明在调用后取值会漂。
    - **N3**：§0 的「`261019` 一次 854,385、`261021` 一次 851,701」改为「**合计**」，与 T4 的「4 次派发 / 2 次派发」对齐；同段的 friction 分布同步按 F4 改写（去掉 `unknown`、单列缺失 7）。
    - **未随修订改动的部分**：T1 写侧（`mergeSha` 全 40 位）、T3 生命周期事件与重复盖章三条件、T4 五值闭集与 producer 链、riskTier high（判官已认可）、manifest 九行与 §2 锚并集（结构未变）。
12. **v0.4（按 R2 `GREEN` 的三条 NIT N4–N6 落件，2026-10-26）——逐条处置**（§7 判官区一字未动；本轮只改正文与本节）：
    - **N4**：T4「问题」段的「`261019…` **一次** 854,385（4 次派发）」改为「**合计**」，`261021…` 同句补「合计」，与 §0 的「合计」口径统一。
    - **N5**：§4 OQ4 的「连带影响已落正文」句改为**显式标注为 v0.2 裁定时的快照**，并补指向现行规格的半句——「v0.3 起按 R1-F1 修订为『subject 分类器为主判据 + 跨包共享 SHA 降为防御性 guard + 三来源并集』，见 §2 T1」。这样既保留裁定当时的记录，也不会让读者把已被替换的机制当成现状。
    - **N6**：T1 新增专条「`shaSource` 多通道重遇的优先级」——同一 SHA 可能同时经多路进入候选集，`mergeAttribution[]` 对该 SHA **只出一条**，`shaSource` 按通道正式性取最高：**`merge-completed` > `merged-legacy` > `commit-message`**；Pod 级 `attributionSources` 保留**全部**实际命中来源。实例写明：`017de34c` 在三个 261022 包里同时经 `merged-legacy` 与 `commit-message` 两路进入，去重后 `shaSource` 取 `merged-legacy`、`attributionSources` = `['merged-legacy', 'commit-message']`。
    - **本轮未改**：目标文件、manifest、锚集、riskTier、验证集条目数（23 条断言）与 §7 判官区；preflight 复跑结果见第 1 条末段。
