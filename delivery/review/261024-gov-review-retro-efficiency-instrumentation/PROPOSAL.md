# PROPOSAL · 261024-gov-review-retro-efficiency-instrumentation

> **状态：DRAFT v0.2 · 2026-10-24 · 起草（Track B 主路径；已按 R1 评审修订，待 human 立项 ack）**（FROZEN 时本行换为凭据行：human 立项 ack + architect Rn GREEN + human 裁 OQ 结果。）
> **承载 ARQ**：`ARQ-261014-pod-events-booking-telemetry-fidelity`（部分承载——其修法候选⑥「模型/用量数据失真」的**可见性侧**由本批 T2 承接；①–⑤（escalation 自动闭合 / actor 归属 / 同决定双记 / `refEventId` 单写 / 时间戳倒挂）本批不治，条目保持 deferred，指针见 §6 第 5 条）。同根输入 = human 2026-10-24 两步走决定原话 + 第一步基线报告 `delivery/retro/2026-10-24-pod-efficiency-baseline.md`（无在册 ARQ 直承）。
> **review 轨**：Track B 主路径（机器面恒 Track B，**非**瘦制品轻路径——制品四件 + retro 照常，不误套轻路径 retro 豁免；SSOT：`.claude/references/gov-change.md`、`AGENTS.md`「治理路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T7）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（触 G3 承重脚本四件——`pod-events-append` / `pod-events-aggregate` / `dispatch-stamp` / `merge-driver`——另新增三件脚本与一份新台账；事件 schema 扩项影响全部 Pod 的簿记面。全线只观测、不 blocking、不改任何门判据，故不取 high；但触面广，向严取 medium 不取 low-medium）。
> **review profile**：G2+G3（`.claude/references/**`、`.claude/commands/**`、`.claude/agents/planner.md`、`.claude/scripts/**`、`.claude/loop-retro/**`）。G1 触面为零、契约面为零。
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/agents/planner.md
.claude/commands/loop-retro.md
.claude/loop-retro/epochs.jsonl
.claude/references/dev-loop-operating-system.md
.claude/references/dev-loop.md
.claude/references/task-intake-and-slicing.md
.claude/scripts/dispatch-stamp.mjs
.claude/scripts/lib/diff-buckets.mjs
.claude/scripts/merge-driver.mjs
.claude/scripts/pod-events-aggregate.mjs
.claude/scripts/pod-events-append.mjs
.claude/scripts/pod-stamp.mjs
.claude/scripts/retro-efficiency.mjs
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：让每一个 Pod 在起包与收口两个时点自动留下「能力纪元 × 档位」可复算的效率事实（起止时间、base/head、diff 四桶与验收断言数、门开火汇总、主线程用量、合并 SHA、逃逸归因），使周期 retro 能出一张产出轴 × 时间轴 × 治理开销 × 质量护栏的仪表盘；事实只进**既有** `pod-events.jsonl` 事件与一份追加式纪元台账，聚合恒为**按需输出**——不复活已退役的 committed 每 Pod metrics 文件。

**现状差距**（2026-10-24 起草期本 worktree 现跑普查，命令见 §2 开头）：

- 84 个带 `taskpackage.json` 的包中，80 个有 pod-events、79 个有 verifier round 证据、80 个有 supervisor verdict，但 **0 个**有收口快照事件、**0 个**有逃逸归因字段；69 条 `merge-completed` 事件只有 1 条带 `payload.mergeSha`。
- 事件面共有 51 种 `eventType`、dispatch 事件里共有 155 种 `phase`——闭集校验只约束了「走工具的写入」，没有约束直写，也没有把漂移变成可见信号。
- 唯一的 nano 样本（`261020-p3-rec-frame-visual`）有 5 条事件、0 条 dispatch 事件——它的一次完整生命周期在机器面上是不可见的。
- 第一步基线报告用的统计脚本与四桶路径分类住在仓外一次性目录（`c:/tmp/retro-metrics/collect.mjs`，硬编码仓库路径并把结果写回 `c:/tmp`），任何人重算都得不到同一组数。

**非目标（本批不做，防 scope creep）**：不改任何门的语义与判定（supervisor 合并门、verifier 闸族、契约机械凭据闸、前置升级闸均零改动）；不改分档阈值、不做「每道门是否还值得」的组合复评（human 已定为后续独立批次）；不触 `docs/contracts/**` 与 spec；不新增工程门、不把效率数据变成任何 blocking 判定；不重新引入 committed `loop-metrics.json` 或任何每 Pod metrics 文件；不追溯伪造历史事实（拿不出来的列写 `null` 并注明原因，不写 0）。

## 2. 问题、根因与改法（T1…T7）

**【硬项 · 起草期实证义务履行声明】** 本节全部行为断言均附 2026-10-24 起草期实测（命令 + 关键输出，本 worktree `gov/kidpal`）。普查口径统一为 `node -e` 遍历 `delivery/taskpackages/*/pod-events.jsonl` 与 `*/taskpackage.json`：

- **事件普查**：81 个含事件的包、1070 行事件、51 种 `eventType`（其中 11 行为 `eventType` 缺失的历史行，序列化后呈现为 `undefined`）；`merge-completed` 69 条，带 `payload.mergeSha` 的 1 条。
- **phase 普查**：429 条 `dispatch-completed` 中 155 种 phase，计数最高者 = `(none)` 52 / `acceptance` 47 / `build` 40 / `integrate` 33 / `closeout` 17 / `acceptance-freeze` 17（`fix` 14 紧随其后）。
- **模型普查**：12 种 model 取值（`(none)` 199 条、`glm-5-3-flash-260828` 98、`deepseek-flash` 36、`deepseek-v4-1-flash-260910` 25、`glm-5-3-flash` 21、`deepseek-v4-1-flash-261017` 17、`deepseek-v4-flash-ga-260731` 10、`deepseek-v4.1-flash` 9、`glm-5.3` 9、`deepseek-v4-pro` 3、`kimi-k3` 1、`<synthetic>` 1）。
- **制品覆盖**：84 个带 `taskpackage.json` 的包中，80 有 pod-events、79 有 `verifier-evidence/rounds`、80 有 `supervisor-verdict.json`、0 有 pod-closeout 事件、0 有 `escapeRef` 字段。
- **nano 样本**：`261020-p3-rec-frame-visual` 共 5 条事件（human-decision ×3、merge-completed ×2），0 条 dispatch，无 rounds 目录。
- **required-check 差分测试**：把一份真实 taskpackage 复制到 `C:/tmp/tpfixture` 并注入 `escapeRef` 后跑 `node .claude/scripts/taskpackage-required-check.mjs <夹具路径>`，与剔除该字段的对照组输出同形——唯一 `missing` 均为夹具缺 `input-brief.md`，无「未知字段」类错误，故新增可选字段不撞既有机检。
- **preflight 实跑结果**见文末「起草注记」。
- 本批所有「将新增/将改」的脚本行为属设计规格，不是对现状的断言；对现状的行为断言均以 Read/Grep 到源码行号为准，无「未实测」项。

**【硬项 · 目标文件锚清单】** 每个 T 末尾显式列「目标文件：」行（新增 / 改分注，行首裸起、不加粗）。packet-binding 完整性断言（landing-manifest ≡ 各 T 目标文件并集）依赖本清单：缺锚清单 = 门 RED 点名（fail-loud，不假绿）。本批各 T 的目标文件并集共 13 个路径，与头部 landing-manifest 围栏块逐行一致。

### T1 · 能力纪元台账与起包归属

**问题**：仓内没有能力纪元台账，跨月比较只能拿「模型族」当临时能力代理。模型构成每换一次，跨月对比就会把模型换代红利记账成治理功劳——第一步基线报告已经明确列出这一条不能下结论，而 9→10 月 lite 档 build token 降 55%、治理侧 token 涨 18% 的实测对比也正卡在同一个混杂因子上。同时，dispatch 事件里能观察到的模型取值有 12 种（含 199 条无模型），按字符串归并必然失真，无法替代显式纪元标签。

**证据**：
- `delivery/retro/2026-10-24-pod-efficiency-baseline.md:20`：「| 能力基线标签 | 0 | 87 | 没有纪台账；本报告只能用「模型族」作临时分组键 |」。
- 同件 `:26`：「模型族是现有数据里唯一可用的能力代理……这个归并本身不可靠，第二步必须换成显式纪元标签」；`:78` 的条目名即「治理效率有没有随时间提升」，正文写明「模型构成变了，且没有能力纪元标签，跨月比较会把模型红利记成治理功劳」。
- 同件 `:122`：纪元台账建议形态 = 追加式文件（`.claude/loop-retro/epochs.jsonl`），一行一个纪元，Pod 按起包日期归属。
- 模型取值普查（见本节开头）：12 种取值并存且大量缺失，字符串归并不可靠。

**根因**：纪元是**决策事实**（人类选定的模型阵容、harness 版本、派发信封形态），不是可从数据推导的属性；仓里从来没有一处把「当时用的是哪套能力底座」记下来，事件里的 `model` 只是逐 dispatch 的观测值、且历史大量缺失。按日期归属又要求纪元区间互不重叠，历史模型混用的时段（例如 9 月下旬 deepseek 与 glm 并存）根本无法事后切出干净边界——这是「拿不出来就先空着」的典型场景。

**改法**：新增追加式台账 `.claude/loop-retro/epochs.jsonl`，与既有 `.claude/loop-retro/ledger.jsonl` 同目录同纪律（版本控制、append-only、一行一条 JSON）：

- 每行字段：`epochId`（如 `E1`）、`openedAt`（开纪元日期）、`models`（当期模型阵容数组）、`harness`（harness 版本或描述）、`dispatchEnvelope`（派发信封版本描述，如对应治理批 changeId）、`reconstructed`（布尔）为**六项必填**；`note` **可选**（在场时须为字符串）。
- **开纪元判据**：模型阵容 / harness / 派发信封任一变化 → 开新纪元；**治理规则变化不开新纪元**。
- **写入者 = 主线程**（模型阵容本就是 human 的决定，随决定落一行；本批 landing 只植入一条种子行，见 OQ1）。
- **归属规则**：Pod 的纪元 = `pod-start` 事件的 `startedAt` 日期（T2）落在 `openedAt ≤ 该日` 的最新一行；`pod-start` 缺失时回退 changeId 日期前缀；仍无则 `null`。归属来源记入 `epochSource`（`podStartAt` / `changeId-date` / `none`），不做静默猜测。
- 消费面只有 T6 仪表盘；不进任何闸。

目标文件：`.claude/loop-retro/epochs.jsonl`（新增） · `.claude/references/dev-loop-operating-system.md`（改）

### T2 · Pod 生命周期事件与主线程用量检查点

**问题**：Pod 没有真实的起止时间锚。墙钟只能拿「首末事件时间差」近似——事件稀疏的 Pod 会被系统性低估；base ref 与 head SHA 没有机械记录，行数只能从历史 merge 提交反推。主线程自身的 token 消耗也只寄生在 dispatch 事件上：transcript 命中才带一次累计快照，整条链上没有任何与 dispatch 无关的检查点，于是没有 dispatch 的 nano 包在机器面上完全不可见。

**证据**：
- `delivery/retro/2026-10-24-pod-efficiency-baseline.md:16-19`：起止时间戳 0/87、nano 档成本 0/1、「现在只有事件时间，没有起包/收口时间」。
- 第一步脚本 `C:/tmp/retro-metrics/collect.mjs:123-126`：墙钟 = `timestamps[0]` 与末条事件之差——稀疏事件必低估。
- `.claude/scripts/dispatch-stamp.mjs:286-292`：`mainThreadUsage` 只在派发 transcript 命中时随事件带出（`telemetry.found ? ... : null`），没有独立入口。
- nano 样本实测（见本节开头）：0 条 dispatch，主线程用量与墙钟都无锚。
- 事件闭集现状：`.claude/scripts/pod-events-append.mjs:29-36`（六值），`:48-50`（未知类型直接抛错）。

**根因**：pod-events 的骨架事件只有「事件发生时」，缺「Pod 开始 / Pod 结束」这一对生命周期锚；主线程用量检查点按设计寄生在 dispatch 上，于是「没有派发」与「没有消耗」被混为一谈。

**改法**：

- **事件闭集扩两项**：`pod-start`、`pod-closeout`（落到 `.claude/scripts/pod-events-append.mjs` 的 `VALID_EVENT_TYPES`，与 T7 的收敛同一落点、同批原子生效）。
- **新增 `.claude/scripts/pod-stamp.mjs`**（复用 `resolveCurrentTaskPackage` 与 `--change-id` 回显纪律，形态比照 `dispatch-stamp.mjs` 的 post-cutoff 分支）：
  - `--phase start --change-id <id> [--started-at <iso>] [--base-ref <ref>]`：写 `pod-start`，payload = `{startedAt, baseRef, capabilityEpochId, epochSource, rigor, sessionId, mainThreadUsage}`。`startedAt` 缺省取当前时刻，`--started-at` 允许起包侧回填真实起点（OQ2）；`baseRef` 缺省 = `git merge-base HEAD main` 结果，失败写 `null` + reason。
  - `--phase closeout --change-id <id>`：写 `pod-closeout`，payload 见 T3/T4 字段（`{closedAt, startedAt, baseRef, headSha, rigor, riskTier, diffStats, acceptanceCount, acceptanceKinds, gateFires, escapeRef, sessionId, mainThreadUsage, dirtyFiles}`）。`headSha` = `git rev-parse HEAD`；`dirtyFiles` = 收口时 `git status --porcelain` 行数（记账，不判红）。
  - `mainThreadUsage`：经 `lib/transcript-telemetry.mjs` 的 `readMainThreadUsage(sessionId, projectDirName)` 取**主线程会话累计值**，随事件带 `sessionId` 与 `sessionSource`。会话发现 = 当前工作目录对应项目目录下 mtime 最新的会话文件（可 `--session-id` 覆盖），失败写 `{status:'unavailable', reason}` 不阻断。跨会话的累计值不可相减——T6 只在两端 `sessionId` 相同时做差，否则记 `null`。
  - 全程 fail-open：任何子计算失败只让对应字段 `null` + reason，事件照写；脚本除用法错误外恒 exit 0，**不阻断任何相位**。
- **planner.md 接线（两处）**：Step 5 Draft TaskPackage 落盘后跑 `--phase start`（起包锚）；Step 11 的 `[FS-EVIDENCE-3]` 通过后、写 `pod-retro.md` 前跑 `--phase closeout`（收口锚；此时 integrator 已提交，工作树干净）。选 planner 主线程而非 integrator 的原因：`mainThreadUsage` 的会话锚就是 Pod 主线程会话，integrator 是独立子会话，由它取不到同一累计序列。nano 分支（3n）的保留清单加一句「pod-stamp 起止两侧必跑」，其余不变。
- **OS §5d 同步**：闭集枚举、两个新事件类型、`pod-start` / `pod-closeout` 的 payload schema、「只观测、fail-open、不进任何闸」一句。**另须给 `:139` 现行句加 carve-out**——该句原文「Verifier rounds、gate outcomes、test results、supervisor verdict 不进入 pod-events，仍以 immutable verifier evidence / supervisor verdict 为权威」与 T4 的 `gateFires` 段字面冲突，同步修订为：「原始 gate outcomes / test results 不落 pod-events，权威仍在 immutable 制品；`pod-closeout` 的 `gateFires` 是收口时点自这些 immutable 制品派生的**快照**（advisory derived），不构成第二事实源、不进任何判定链」。语义与 T4「不造第二账本」一致。

目标文件：`.claude/scripts/pod-stamp.mjs`（新增） · `.claude/scripts/pod-events-append.mjs`（改） · `.claude/agents/planner.md`（改） · `.claude/references/dev-loop-operating-system.md`（改）

### T3 · 收口 diff 四桶与验收断言数快照

**问题**：产出轴目前没有 canonical 口径。四桶（impl / test / doc / 治理制品）行数只有一个住在仓外的一次性脚本会算，脚本硬编码仓库路径、把结果写回 `c:/tmp`，换一台机器、换一个 worktree 就得到另一套数；而「验收断言数」在整个数据面上根本不存在——taskpackage 没有任何字段承载它，任何既有制品都推不出来。

**证据**：
- `C:/tmp/retro-metrics/collect.mjs:10`（`const REPO = 'c:/AI/KidPal-gov'` 硬编码）、`:21-32`（`classify()` 四桶规则本体，内部还分出 `govConfig` 第五桶）、`:175`（`writeFileSync('c:/tmp/retro-metrics/pods.json', ...)` 写仓外）。
- `delivery/retro/2026-10-24-pod-efficiency-baseline.md:126`：「脚本和四桶分类目前不在仓内；第二步落地时应把它们正式收进 `.claude/scripts/`，否则下次复算会得到另一套数字」。
- `delivery/retro/2026-10-24-pod-efficiency-baseline.md:22`：明确留空列含「验收断言数」。
- 起草期实测：真实 taskpackage.json 的顶层键（`changeId` … `notes` 共 36 个）中无任何验收计数或断言计数字段。

**根因**：分类规则活在一次探索性脚本里、从未成文，仓里没有「哪条路径进哪一桶」的单一事实源；「验收断言」在本仓也从未被定义成机械可数单位——验收体系的机械单位一直是 `acceptance[]` 条目（acceptance-coverage 与 mutation 逐条取证都以它为单位），但收口时没人数它。

**改法**：

- **新增 `.claude/scripts/lib/diff-buckets.mjs`**（纯库，无副作用）：
  - `classifyPath(path)`：四桶 = `test` / `governance` / `doc` / `impl`，判定顺序照搬 `collect.mjs:21-32`（`delivery/**` 与 `.claude/**` → 治理制品；其余 `docs/**`、任意 `*.md` / `*.mdx`、`README*` → doc；测试形态（`tests/`、`__tests__/`、`*.test.*`、`*.spec.*`、`*_test.py`、`test_*.py`、`*.acceptance.*`、`fixtures/`、`tests/data/`）→ test；其余 → impl）。治理制品桶 = 原脚本 `govArtifact` + `govConfig` 的并集，与基线报告「治理制品行」列同口径，历史数字可直接对比。
  - `diffStats(baseRef, headRef, {cwd})`：跑 `git diff --numstat <base> <head>`，二进制行（`-`）计 0，逐文件归桶后汇总 `lines` 与 `files` 两组计数。
- **pod-stamp 收口快照**（T2 的 `pod-closeout`）：`diffStats` 用该库对 `baseRef..headSha` 计算；`acceptanceCount` = taskpackage `acceptance[]` 长度（主口径，OQ4），`acceptanceKinds` = 按 `kind` / `suiteGate` 字段分 `behavioral` / `structural` / `suiteGate` 三档计数。
- **口径注记写进 OS §5d**：收口快照只覆盖 integrator closeout 时**已提交**的面；`pod-retro.md` 与合并期簿记发生在快照之后，不计入四桶——与基线报告「按 merge commit 反推」的口径本就有这点差异，T6 输出说明会写清。
- 历史 Pod 不补造快照；需要历史数字时由 T6 的 `--recompute-diff` 走 merge commit 回填（同一 `classifyPath` 规则，保证同一组数）。

目标文件：`.claude/scripts/lib/diff-buckets.mjs`（新增） · `.claude/scripts/pod-stamp.mjs`（改）

### T4 · 门开火汇总与合并后逃逸归因

**问题**：每道门的开火记录分散在各自制品里：verifier 红轮在 round fact、supervisor 裁决在 `supervisor-verdict.json`、mutation 发现埋在 taskpackage 的取证数组、契约机械凭据闸的输出甚至不落盘。周期 retro 想回答「哪道门真拦住了东西」时，没有一处可汇总的入口。更关键的是，一个 Pod 如果是某个早期 Pod 的返修，机器面上完全看不出这层关系——没有任何字段指向「本包修的是谁、本应由哪道门拦住」。

**证据**：
- verifier round fact 的权威位置与命名：`.claude/scripts/verifier-evidence-check.mjs:126-131`（`roundsDir` = `delivery/taskpackages/<id>/verifier-evidence/rounds`，`evidenceRef` = `.../rounds/r<N>.json`）；实测样本 `261022-m5-k4-history-corpus-backfill` 六个 round 中 r1/r2 为 `ok:false, observedGateExit:1`——红轮可机械计数。
- `supervisor-verdict.json` 为 **write-once**：`.claude/hooks/supervisor-verdict-gate.mjs:433-467`（`linkSync` 建 final，EEXIST 即 fail-loud），只保留最终裁决，历史上 BLOCK 轮次不落第二处。
- 契约机械凭据闸无持久开火记录：`.claude/scripts/pre-merge-escalation-gate.mjs:775` 只在 `probeOutPath` 写并行相交面探针，不写 BLOCK 判定。
- mutation 发现可数：`delivery/taskpackages/261015-p1-auxbar-style-fix/taskpackage.json` 的 `mutationEvidence.perAcceptance[]` 每条带 `sensitive` 布尔——`sensitive:false` 即一次「变异未被抓到」的发现。
- 逃逸归因：84 个 taskpackage 实测 `escapeRef` 命中 0。

**根因**：各闸的输出面是为自己服务的（判红、放行、留证），从未按「汇总消费」设计；收口事件缺位（T2）又让汇总没有天然落点。逃逸是**跨 Pod 关系**，既没有字段也没有生产者——起包时知道、收口后就丢。

**改法**：

- **`pod-closeout` 的 `gateFires` 段**（机械从既有权威制品派生，不造第二账本；OS §5d:139 的派生快照 carve-out 见 T2 的 OS 同步项）：
  - `verifier: {rounds, redRounds, failedByGate}`——扫 `verifier-evidence/rounds/*.json`，`rounds` = 文件数，`redRounds` = `ok:false` 数，`failedByGate` = 各闸名在红轮里的失败次数。
  - `supervisor: {verdict, riskTier, recordedAt, blockRounds, blockRoundsReason}`——读 `supervisor-verdict.json` 的最终裁决；`blockRounds` 本批恒 `null` + `blockRoundsReason: 'write-once-final-only'`（OQ5）。
  - `mutation: {insensitiveCount, entries}`——`mutationEvidence.perAcceptance[]` 中 `sensitive !== true` 的条数。
  - `contractGate: {blocked, blockedReason}`——本批恒 `null` + `blockedReason: 'gate-output-not-persisted'`（OQ5）。
  - **null 不写 0**：与既有「unknown 绝不写 0」的遥测纪律同源，拿不出来就空着并给原因。
- **`escapeRef` 字段**：TaskPackage 新增**可选**字段 `escapeRef: {originChangeId, missedGate, note}`，生产者 = triage / planner（Step 5 填，返修包必填、非返修包缺省）；文档落 `.claude/references/task-intake-and-slicing.md` 的 TaskPackage 字段表 + planner Step 5 一句指向。pod-stamp 收口时原样抄进 `pod-closeout` 事件，T6 据此算逃逸率与「本该拦住它的门」分布。字段为可选——required-check 无顶层未知键校验（差分测试见 §2 开头），不撞既有机检。
- 历史缺口不补造：旧 Pod 的契约闸 BLOCK / supervisor BLOCK 轮本批记不到，留根 §6 第 1 条；将来若要机械可数，由各闸落持久件另批处理。

目标文件：`.claude/scripts/pod-stamp.mjs`（改） · `.claude/references/task-intake-and-slicing.md`（改） · `.claude/agents/planner.md`（改）

### T5 · `merge-completed` 事件机械携带 `mergeSha`

**问题**：合并事实的机器字段缺失。69 条 `merge-completed` 事件里只有 1 条带 `payload.mergeSha`，其余只能从 `payload.summary` 的散文里正则抠出前 12 位——而 `mergeSha` 在写事件的那个函数里本来就已经算好了，只是没落字段。

**证据**：
- 事件普查（见 §2 开头）：69 条 `merge-completed`，带 `payload.mergeSha` 的 1 条。
- `.claude/scripts/merge-driver.mjs:1051`：`const mergeSha = execSync('git rev-parse HEAD', ...)`——值已存在。
- 同件 `:1052-1063`：事件 payload 只有 `summary`（散文内嵌 `main tip ${mergeSha.slice(0, 12)}` 与 human-approval-ref）。
- 同件 `:1066`：commit message 写 `merged ${mergeSha.slice(0, 12)}`（历史回退匹配的可用锚，见 T6）。

**根因**：Step 5b 簿记事件按「人读摘要」设计，没有给机器字段；`mergeSha` 已在函数作用域内，属于纯粹的落字段遗漏，不是数据不可得。

**改法**：`step5bBookkeeping` 的 payload 增 `mergeSha`（40 位全 SHA）与 `mode`（`single` / `board` / `self-check`），`summary` 原样保留。同步扩 `.claude/scripts/merge-driver.mjs:1255-1275` 的 T6 自检夹具：在既有「簿记落行 + pathspec 不夹带残留」断言之外，加一条 `payload.mergeSha === HEAD`（反转必红）。历史事件不改写（append-only）；旧事件缺字段时由 T6 从 commit subject 的 `merged <sha12>` 前缀与 merge commit 列表回退匹配。

目标文件：`.claude/scripts/merge-driver.mjs`（改）

### T6 · 效率仪表盘脚本与 `/loop-retro` 消费节

**问题**：第一步的统计能力只活在一次探索性脚本里，不可复算；`/loop-retro` 的输出契约里也从来没有效率轴——它只消费 token coverage、escalation 与 friction 主题，产出轴、单位成本、门开火与逃逸率都不在视野内。

**证据**：
- `C:/tmp/retro-metrics/collect.mjs:10`（硬编码仓库）、`:175`（输出写 `c:/tmp`）——脚本不是仓库制品，任何一次重算都可能换一套数。
- `.claude/commands/loop-retro.md:26-44`：步骤 1–6 的输出清单只含 top findings、升级主题、分流、advisory、waiting、token coverage；无效率仪表盘。
- `.claude/references/dev-loop-operating-system.md:145`：「New-format 用 on-demand aggregation 提取 token coverage、escalation 与 friction themes」——同样没有产出轴。

**根因**：第一步刻意停在「用现有数据跑通流程」，脚本与口径都还没产品化；retro 命令的输出契约从建立起就只面向「摩擦与流程债」，效率视角是新引入的第二个消费轴，需要显式成文。

**改法**：

- **新增 `.claude/scripts/retro-efficiency.mjs`**（只读、按需、零写盘副作用）：
  - 参数：`--since <YYMMDD|YYYY-MM-DD>` / `--until` / `--epoch <id>` / `--rigor <tier>` / `--recompute-diff` / `--json` / `--self-check`；缺省输出 markdown 表到 stdout。
  - 数据源优先级：`pod-closeout` 事件 → （字段缺失或 `--recompute-diff` 时）merge commit 回填（沿用 `collect.mjs:35-78` 的 `findMergeCommits` / `diffStatsFor` 算法搬入）→ `null`。四桶分类与行数恒走 `lib/diff-buckets.mjs`，与收口快照单源。`mergeSha` 缺失时从 commit subject `merged <sha12>` 前缀回退匹配（T5）。
  - 分组：**纪元 × 档位**。每格输出：Pod 数、token（`{dispatchesTotal, dispatchesWithTokenData, dispatchesUnavailable}` 三计数随行）、主线程 token（仅同会话首尾差可算）、agent 分钟、墙钟分钟、等待占比、dispatch 数、治理 token 占比、返工派发占比、impl / test / doc / 治理制品四桶行数、文书代码比、token / 千行、验收断言数、gate 开火（verifier 红轮 / supervisor 裁决分布 / mutation insensitive 数）、逃逸数与「本该拦住它的门」分布。
  - 末尾固定「数据覆盖」节：逐字段列可用 / 不可用 Pod 计数与原因（无收口快照、无 exact token、无 merge 锚、`escapeRef` 缺失等）——拿不出来的列显式空着，不补 0，也不假装是全量。
  - `--self-check`：临时目录夹具断言（至少含：纪元按日期解析、同会话差可算 / 跨会话记 `null`、四桶判定对反转实现会变红、收口快照优先于 merge 回填、缺字段不写 0）。
- **`.claude/commands/loop-retro.md`**：步骤 2 增一句「new-format 包之外，跑 `node .claude/scripts/retro-efficiency.mjs --since <周期起点>` 取本周期效率仪表盘」；步骤 3 的共性提炼增「效率轴共性（单位成本恶化 / 治理占比异常 / 逃逸集中）」；步骤 5 输出增「效率仪表盘（按纪元 × 档位）+ 数据覆盖缺口」两节。游标、触发条款、分流纪律一律不动。
- **OS §6** 增一段效率消费面：指命令与输出口径，不复述列清单、不改游标与触发条款。

目标文件：`.claude/scripts/retro-efficiency.mjs`（新增） · `.claude/commands/loop-retro.md`（改） · `.claude/references/dev-loop-operating-system.md`（改）

### T7 · 事件类型与 phase 收敛（告警不拦截，fail-open）

**问题**：事件面已经在漂移。全量 1070 条事件里出现 51 种 `eventType`，dispatch 事件里出现 155 种 `phase`——闭集校验形同虚设；而现行 append 工具对未知类型**抛错**，把本应纯观测的簿记面变成了硬门：想记一笔新事件却不在闭集里的调用方，要么直接失败，要么绕开工具直写（`merge-driver` 就是直写的），于是闭集越收越假。

**证据**：
- 普查（见 §2 开头）：51 种 eventType、155 种 phase（计数最高者 = `(none)` 52 / acceptance 47 / build 40 / integrate 33 / closeout 17 / acceptance-freeze 17，fix 14 紧随其后）。
- `.claude/scripts/pod-events-append.mjs:29-36`（六值闭集）、`:48-50`（`throw new Error('invalid eventType ...')`）。
- `.claude/references/dev-loop-operating-system.md:130`：「eventType 闭集：`dispatch-completed | agent-escalation | agent-escalation-resolved | human-decision | human-correction | friction`」。
- `.claude/scripts/merge-driver.mjs:1052-1064`：`merge-completed` 事件由 merge-driver 直接 `appendFileSync` 写入，不经过闭集校验——闭集从未约束过直写方。
- `delivery/retro/2026-10-24-pod-efficiency-baseline.md:120`：「事件类型收敛成枚举：现有 45 种自造类型，建议在 append 时告警而不是拦截，保持 fail-open」（该报告记 45 种、本次全量普查为 51 种，两次普查时点与口径不同；本批的断言只取「漂移存在」这一事实，不以任一绝对数为判据）。

**根因**：闭集只约束了「走工具的写入」，没有约束直写；而工具的抛错语义把观测面变成了硬门，与 pod-events「observational、non-bearing、fail-open」的既有定性（OS §5d）自相矛盾——观测设施不该有让调用方失败的权力。

**改法**：

- **`pod-events-append.mjs`**：未知 `eventType` 改为 **stderr 告警后照常 append**（告警文本给出规范集合）；同时导出 `VALID_EVENT_TYPES` 供聚合消费。闭集扩为九值：`dispatch-completed` / `agent-escalation` / `agent-escalation-resolved` / `human-decision` / `human-correction` / `friction` / `merge-completed` / `pod-start` / `pod-closeout`。`changeId` / `actor` / `source` / `payload` 的结构校验维持原样（那是 envelope 完整性，不是类型收敛）。
- **`dispatch-stamp.mjs`**：`--phase` 不在 canonical 集合时 stderr 告警并照常写事件；该集合由本件导出 `CANONICAL_PHASES`，**权威落点 = 代码导出**（聚合方从该导出消费）。本批初值十三值：`triage` / `intake` / `acceptance` / `build` / `verify` / `fix` / `mutation` / `integrate` / `docs-sync` / `closeout` / `merge` / `retro` / `advisory`（覆盖 planner 步骤与 advisory 档的既有用词；历史异形词留作漂移观测对象，不做映射、不改写）。
- **`pod-events-aggregate.mjs`**：只增键、不改既有键，新增 `eventTypes: {counts, nonCanonical}`、`phases: {counts, nonCanonical}`、`lifecycle`（`pod-start` / `pod-closeout` 摘要）、`closeout`（收口快照摘要）四段，让漂移在聚合输出里直接可见。
- **文档同步**：OS §5d 的闭集枚举改为九值，补一句「未知类型 / 非 canonical phase 告警不拦截，保持 fail-open」，并写明 canonical phase 集合的权威 = `dispatch-stamp.mjs` 导出的 `CANONICAL_PHASES`（**不在 OS 复制名册**——机械面枚举按锚点纪律指向代码导出）；`.claude/references/dev-loop.md:91`（「校验闭集 eventType」）与 `:334`（「eventType 闭集 + envelope 校验」）两处措辞同步为「闭集 + 未知类型告警（fail-open）」。`journal-append.mjs` 自身的 `round-outcome | friction` 闭集不动（那是 journal 车道的输入枚举，不是 pod-events 事件类型）。
- 历史事件不改写、不做类型映射：旧异形词就是漂移证据，保留原样在 `nonCanonical` 里显形。

目标文件：`.claude/scripts/pod-events-append.mjs`（改） · `.claude/scripts/dispatch-stamp.mjs`（改） · `.claude/scripts/pod-events-aggregate.mjs`（改） · `.claude/references/dev-loop-operating-system.md`（改） · `.claude/references/dev-loop.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G2 agents | `.claude/agents/planner.md` | 改（T2/T4） | Step 5 起包 stamp + `escapeRef` 填报；Step 11 收口 stamp；3n nano 保留清单加一句 |
| G2 commands | `.claude/commands/loop-retro.md` | 改（T6） | 步骤 2/3/5 增效率仪表盘消费；游标与触发条款不动 |
| G2 references | `.claude/references/dev-loop-operating-system.md` | 改（T1/T2/T3/T6/T7） | §5d 闭集与 payload schema、§5d:139 派生快照 carve-out、纪元台账、四桶口径、§6 效率消费面 |
| G2 references | `.claude/references/dev-loop.md` | 改（T7） | :91 / :334 两处闭集措辞同步为 fail-open 语义 |
| G2 references | `.claude/references/task-intake-and-slicing.md` | 改（T4） | TaskPackage 可选字段 `escapeRef` 文档化 |
| G3 scripts | `.claude/scripts/pod-events-append.mjs` | 改（T2/T7） | 闭集九值 + fail-open 告警 + 导出 `VALID_EVENT_TYPES` |
| G3 scripts | `.claude/scripts/pod-events-aggregate.mjs` | 改（T7） | 只增键：eventTypes / phases / lifecycle / closeout 四段 |
| G3 scripts | `.claude/scripts/dispatch-stamp.mjs` | 改（T7） | 导出 `CANONICAL_PHASES` + 非 canonical phase 告警，判定语义零变更 |
| G3 scripts | `.claude/scripts/merge-driver.mjs` | 改（T5） | Step 5b payload 加 `mergeSha` / `mode` + 自检断言 |
| G3 scripts | `.claude/scripts/pod-stamp.mjs` | 新增（T2/T3/T4） | 生命周期与收口快照生产者，只读计算 + 程序 fs 追加事件 |
| G3 scripts | `.claude/scripts/lib/diff-buckets.mjs` | 新增（T3） | 四桶分类与 diff 统计单源库 |
| G3 scripts | `.claude/scripts/retro-efficiency.mjs` | 新增（T6） | 只读按需仪表盘 |
| G3 台账 | `.claude/loop-retro/epochs.jsonl` | 新增（T1） | 追加式纪元台账，与 ledger.jsonl 同目录纪律 |
| 承重未触 | `.claude/scope-map.json`、`.claude/*.settings.json`、`.claude/verify-gates.json`、`.claude/hooks/**` | 未触 | 写域零扩张：新脚本由既有角色经 Bash 调用、程序 fs 追加事件（与 dispatch-stamp 同通道）；无新 agent 写权 |
| 承重未触 | `.claude/scripts/closeout-check.mjs`、`supervisor-verdict-gate.mjs`、`pre-merge-escalation-gate.mjs`、`supervisor-gate-check.mjs`、`.claude/hooks/verify.mjs` | 未触 | 门判据与合并链零改动；不新增工程门、不加任何牙（OQ3） |
| 承重未触 | `.claude/scripts/taskpackage-required-check.mjs` | 未触 | 可选 `escapeRef` 不撞校验（差分测试见 §2） |
| 未触 | `docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md` | 未触 | 契约面与 spec 零触 |
| 历史制品 | `delivery/taskpackages/**` 既有 pod-events、PROPOSAL、retro | 未触 | append-only：历史事件不改写、不回溯补字段、不补造快照 |

**【硬项 · sweep 透镜义务】** 本批**不新增任何门或断言**；唯一新增的判定 pattern 是「eventType / phase 是否 canonical」，其效果为 stderr 告警与聚合输出列，不拦截任何写入。起草期已用该 pattern 对全仓事件面预跑（§2 开头的普查段，命令为 `node -e` 遍历 `delivery/taskpackages/*/pod-events.jsonl`），原始计数全列于 T1/T2/T6/T7 证据段。census 声明均为机械枚举生成，判官可 spot-check。

**【硬项 · 复扫模式完备性先验】** 退役语面 = 「pod-events eventType 闭集」的旧表述（六值 + 抛错语义）。模式集显式枚举：`eventType 闭集` ×（有/无 backtick）∪ `闭集 eventType` ∪ `eventType：闭集` ∪ `dispatch-completed | agent-escalation | agent-escalation-resolved | human-decision | human-correction | friction`（六值枚举串）∪ `VALID_EVENT_TYPES`。检索命令 `grep -rn "<模式>" .claude docs AGENTS.md CLAUDE.md`（2026-10-24 起草期实跑）。命中与处置：OS `:130`（T7 改）、`dev-loop.md:91` / `:334`（T7 改）、`pod-events-append.mjs:29-36` / `:48-50`（T2/T7 改）、`journal-append.mjs:60`（journal 车道自有枚举，语义不同、不改）、`planner.md:42`（F-08 指 human-decision / human-correction 两类，仍在九值内、不改）。完备性论证：闭集的可能词形 = 「闭集」+ 枚举串 + 常量名三族，并集覆盖全部机器与文面引用点；`pod-journal` 的 `round-outcome | friction` 与 escalation category 闭集属另两根轴（词面相似语义不相干），逐条 Read 确认后显式排除。JSON / 配置面零命中。

## 4. human 取舍点（OQ）

**OQ1 · 纪元台账的启动种子**：
(a) landing 时植入一条 `E0-pre-instrumentation` 种子行（`openedAt` = 现存最早 Pod 日期、`models: []`、`reconstructed: true`），全部历史 Pod 归入该桶；真实纪元从本批之后第一次模型阵容变化起记；
(b) 台账留空，历史 Pod 的纪元记 `null`、仪表盘单列「未归属」桶；
(c) 用 dispatch 遥测里的模型字符串事后切出 3–4 个历史纪元（如 deepseek 期 / glm 期）。
**推荐 (a)**：历史边界已不可考——9 月下旬 deepseek 与 glm 是混用并存的，按日期切不出互不重叠的干净区间，(c) 等于伪造边界；而 (b) 会让历史 Pod 完全落在分组外，(a) 至少给出一个诚实、可比较的「前计量时代」基准桶。
**推荐理由一句话**：E0 桶把「不知道」显式记成「前计量时代」，既保留历史对比的锚，又不冒充事后知道当年的模型阵容。

**OQ2 · 起包时间的机械取点**：
(a) planner Step 5（Draft TaskPackage 落盘时）跑 `pod-stamp --phase start`，`--started-at` 允许起包侧回填更早的真实起点；
(b) planner Step 1 intake 一开始就 stamp——但「自建形状」此时还没有 changeId，需要先建一个临时占位，违背簿记纪律；
(c) 由 launch 面主线程写进 launch-card，planner 读取——依赖 launch-card 覆盖率，历史有包没有启动卡。
**推荐 (a)**：Step 5 是 changeId 与台账目录都已存在的最早时点，单点接线、对所有档位（含 nano）统一；`--started-at` 留出回填真实起点的口子。其已知代价是「triage → intake 探索」这段不计入起包时间，会如实写进 OS 注记。
**推荐理由一句话**：机械可保证的单点时点优先于覆盖率不均的多点回填，回填口子只作可选增强。

**OQ3 · 收口快照是否加牙**：
(a) 不加牙：只落 planner 步骤义务 + T6 仪表盘的覆盖率展示（漏跑在周期 retro 的数据覆盖节显形）；
(b) 在 `closeout-check.mjs` 加一条 warn 级存在性 advisory（post-cutoff、零追溯，先例同 pod-retro / loop-metrics 的 advisory）；
(c) blocking。
**推荐 (a)**：本批定位是「先补记录」，效率数据一旦带牙就违背「不把效率数据变成 blocking 判定」的硬约束；覆盖率缺口在仪表盘里已足够显形，等观测几轮后再评估是否加 warn。
**推荐理由一句话**：记录先落地、牙后评估，符合 human「先把数据流程做掉」的指令；加牙会让新簿记变成新的流程税。

**OQ4 · 「验收断言数」的口径**：
(a) 主口径 = taskpackage `acceptance[]` 条目数（附 `behavioral / structural / suiteGate` 三档计数）；
(b) 正则数验收测试文件里的断言行数（`assert` / `TEST_ASSERT` / `EXPECT_` 等，语言相关、易漂）；
(c) 两者都记。
**推荐 (a)**：本仓验收体系的机械单位一直是 `acceptance[]` 条目——acceptance-coverage 与 mutation 取证逐条都以它为单位，用它作「验收断言数」与既有审计面同轴；代码级断言行数（b）跨 C / Python / mjs 三种语言且随写法漂移，留根 §6 第 2 条等有明确消费方再立。
**推荐理由一句话**：用与既有验收审计同轴的单位，避免造一个只有仪表盘自己相信的第二口径。

**OQ5 · 不可机械恢复的门记录如何处置**（契约机械凭据闸 BLOCK、supervisor BLOCK 轮数）：
(a) 收口快照记 `null` + 原因（`gate-output-not-persisted` / `write-once-final-only`），留根等后续批次；
(b) 本批顺手让 `pre-merge-escalation-gate.mjs` 与 `supervisor-verdict-gate.mjs` 各落一份持久开火件；
(c) 从 `human-decision` 事件与 pod-retro 散文里推断。
**推荐 (a)**：这两处输出面是承重闸，动它们属于「改闸」半径，超出本批；推断（c）不可靠且会把散文变成伪机器数据。空着并写明原因，正是 human「拿不出来的先空着」的原话落点。
**推荐理由一句话**：诚实空值加留根，比为了凑一列而扩承重闸半径更符合本批边界。

**OQ6 · 主线程用量检查点机制**：
(a) 显式 checkpoint：`pod-start` / `pod-closeout` 各带一次主线程会话累计值 + `sessionId`，只在同会话首尾间做差，跨会话记 `null`；
(b) 沿用现状（只有 dispatch 命中时附带累计值）——nano 与无 dispatch 相位不可见；
(c) 依赖 harness 暴露的会话 id 环境变量做精确锚定（本会话未实测该变量存在，若存在则比 mtime 启发式更可靠）。
**推荐 (a)**：它把「有没有派发」与「有没有消耗」解耦，直接补上 nano 的盲区；(c) 作为后续增强写进留根，不在未实测的前提下写进规格。
**推荐理由一句话**：显式检查点用现有 transcript 通道即可闭环，且同会话约束保证数字不会被跨会话累计值污染。

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. **（pre-commit，起草期已跑）** `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261024-gov-review-retro-efficiency-instrumentation/PROPOSAL.md` → 预期 `ok:true` ∧ `findings:[]` ∧ `section2BodyFound:true` ∧ `anchorCount:13`；`anchorUnion` 成员名单 ≡ landing-manifest 围栏块 13 行，注记无夹带路径记号（实跑结果见文末「起草注记」）。
2. **（post-commit）** `node .claude/scripts/pod-stamp.mjs --self-check` → exit 0，输出含自检名且覆盖：纪元按日期解析（早于首行的日期 → `null`）、同会话首尾差可算 / 跨会话 → `null`、四桶分类反转必红、缺 round 目录时 `gateFires.verifier` 为 `null` + reason 而非 0、`escapeRef` 缺省 → `null`。夹具先断言非空（真空绿规约）。
3. **（post-commit）** `node .claude/scripts/retro-efficiency.mjs --self-check` → exit 0，断言：分组按纪元 × 档位、收口快照优先于 merge 回填、同会话差与跨会话 `null` 两臂、覆盖节对缺口计数非零。
4. **（post-commit）** `node .claude/scripts/retro-efficiency.mjs --since 260829` → exit 0；输出含 E0 桶与真实 Pod 行；抽两个 Pod 与 `git diff --numstat <merge>^1 <merge>` 手算逐桶核对一致；「数据覆盖」节列出无收口快照的 Pod 计数（预期 = 当期全部 Pod，收口快照从本批之后才开始有）。
5. **（post-commit）** fail-open 冒烟（临时夹具，`CLAUDE_PROJECT_DIR` 指向夹具根）：`node .claude/scripts/pod-events-append.mjs --change-id <夹具 id> --event-type made-up-type --actor t --source t --payload '{}'` → exit 0 ∧ stderr 含 canonical 集合提示 ∧ 事件行已追加；`dispatch-stamp.mjs` 同夹具带 `--phase not-a-phase` → exit 0 ∧ stderr 告警。
6. **（post-commit）** `node .claude/scripts/pod-events-aggregate.mjs --change-id 261023-honcho-memory-wiring-restoration` → exit 0；既有键（tokens / actors / models / mainThread / escalations / friction）逐键保留，新增 `eventTypes` / `phases` / `lifecycle` / `closeout` 四键在场；**`eventTypes.nonCanonical` 预期为空**（该包五类事件 human-decision / friction / dispatch-completed / agent-escalation / merge-completed 均在九值内），**`phases.nonCanonical` 非空且点名 `acceptance-freeze` / `acceptance-marker-fix`**（两条非 canonical phase，起草期实测各 1 条）。
7. **（post-commit）** `node .claude/scripts/merge-driver.mjs --self-check-merge-friction` → exit 0，含新增断言「`payload.mergeSha` = 夹具 HEAD 全 SHA」（反转必红），且既有 T6 簿记断言不减。
8. **（post-commit）** 回归面零破：`node .claude/hooks/verify.mjs --self-check`（基线 16 项）、`node .claude/scripts/taskpackage-required-check.mjs --self-check`（基线 23 项 + changeid-prefix 4 项，2026-10-24 起草期与 R1 判官均实测 `pass:23,total:23`）、`node .claude/hooks/supervisor-verdict-gate.mjs --self-check`、`node .claude/scripts/route-regression-check.mjs` 全 exit 0 且计数不减。
9. **（post-commit）** `node -e` 逐行 `JSON.parse` `.claude/loop-retro/epochs.jsonl`，断言每行含 `epochId / openedAt / models / harness / dispatchEnvelope / reconstructed` **六必填字段**非缺失、`note` 可选（在场时须为字符串），且 `openedAt` 可解析为日期。
10. **（post-commit）** `git diff --cached --stat` 对照 landing-manifest 13 行：落地文件集 ≡ manifest 集合（landing 后人读复核位）。

## 6. 留根清单

1. **不可机械恢复的门记录持久化**：契约机械凭据闸 BLOCK 与 supervisor BLOCK 轮数（OQ5=(a) 下记 `null` + reason）。若将来确认需要机械可数，由各自闸落持久开火件另批处理，本批不扩承重闸半径。
2. **代码级验收断言计数**：本批只到 `acceptance[]` 条目级（OQ4=(a)）；按语言数断言行数（C / Python / mjs）留待有明确消费方时再立，避免为仪表盘自证而造第二口径。
3. **任务类标签 `taskClass`**：基线报告建议过「固件 UI / 服务端 / 真机轮 / 治理」的分轴，但 TaskPackage 现有 `subsystem` + `rigor` + `sourceType` 已覆盖对比所需的三个维度，再加一层自由文本口径反而制造归并歧义——本批不立；若将来「真机轮 vs host 包」需要独立分轴，再在 `pod-start` payload 上追加。
4. **会话发现的 harness 原生锚**：`pod-stamp` 的会话发现当前用「项目目录下 mtime 最新会话文件」的启发式（OQ6=(c) 未实测 harness 是否暴露会话 id 环境变量）；若后续确认存在该变量，替换启发式即可，字段形态不变。
5. **ARQ-261014-pod-events-booking-telemetry-fidelity 的 ①–⑤ 保持 deferred**：escalation 自动闭合、actor 归属、同决定双记、`refEventId` 单写、时间戳倒挂五项本批不治；本批只动其⑥的可见性侧（主线程检查点显式化 + `suspiciousZeroDelta` 标记）。条目保持 deferred；指针注记由主线程在**独立 chore commit** 中追加——`docs/debt/gov/ARQ-261014-pod-events-booking-telemetry-fidelity.md` 不在本批 manifest 与 §3 sweep 表内，随 landing packet 同 commit 会被 landing-fidelity 门按 scope-creep 判红（与 INDEX backfill 独立补提先例同通道）。
6. **历史快照不补造**：本批不回填历史 `pod-closeout` 事件、不改写既有 `merge-completed` 事件、不改写任何 `eventType` 异形词；历史数字只能经 T6 的 merge 回填路径近似，输出说明必须标注该口径差异（收口快照覆盖已提交面、不含 `pod-retro` 与合并期簿记）。
7. **第一步基线报告的归属**：`delivery/retro/2026-10-24-pod-efficiency-baseline.md` 当前未被 git 跟踪，本批 landing-manifest 不含它；由主线程在合适批次补提交为 retro 证据（append-only、不改正文）。
8. **效率数据永不 blocking（自律件，不进模板）**：本批新产的一切数据不进任何 pass/fail/verdict 判定链；若未来出现「效率阈值拦截合并」的动议，须走完整治理批并显式反转本条。
9. **引据时点（自律件）**：本 PROPOSAL 的全部行号以 2026-10-24 本 worktree `gov/kidpal` 为准；landing 期若目标文件已被并行批改动，行号引据以 FROZEN 时点重核为准。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-24 - architect

**亲核留痕（全部本 worktree `gov/kidpal` 实测，非转述采信）**：
- 普查复算（`node -e` 全量遍历 `delivery/taskpackages/*/pod-events.jsonl` 与 `*/taskpackage.json`）：dirs 88（含 current）→ 带 taskpackage.json 85（含 current）/ 84（不含，与 §0 一致）；pod-events 81；tp∧events 交集 80（与 §0「84 中 80」吻合；差 1 = `260920-gov-review-dispatch-telemetry-source` 有 events 无 tp，81 = 80+1 自洽）；rounds 79、supervisor-verdict 80、pod-closeout 0、escapeRef 0；事件 1070 行（undefined eventType 11 行）、eventType 51 种、merge-completed 69 条中带 payload.mergeSha 1 条、dispatch-completed 429 条中 phase 155 种、model 12 种取值（含 (none) 199）；nano 样本 5 事件（human-decision×3、merge-completed×2、0 dispatch）。以上与 §2 普查段逐项吻合，唯两处措辞偏差见 F3/F6。
- 引据逐条亲核：`pod-events-append.mjs:26`（EVENTS_SCHEMA_CUTOFF）/`:29-36`（六值）/`:48-50`（throw）✓；`dispatch-stamp.mjs:290`（`const mainThreadUsage = telemetry.found`，PROPOSAL 记 :286-292 区间 ✓）；`merge-driver.mjs:1051`（mergeSha）/`:1052-1063`（payload 仅 summary）/`:1066`（commit message `merged <sha12>`）✓；`supervisor-verdict-gate.mjs:434/:447`（writeVerdictOnce + linkSync EEXIST write-once）✓；`pre-merge-escalation-gate.mjs:775`（仅 probeOutPath 落盘、不写 BLOCK 判定）✓；`verifier-evidence-check.mjs:126-131`（roundsDir/evidenceRef）✓；OS `:130`（六值闭集）/`:139`（见 F1）/`:145`（on-demand aggregation 无效率轴）✓；`dev-loop.md:91`/`:334` ✓；`journal-append.mjs:60`（round-outcome|friction 自有枚举）✓；`planner.md:42`（F-08）/`:53`（3n 保留清单）/`:55`（Step 5）/`:89`（[FS-EVIDENCE-3]）✓；`loop-retro.md:26-44`（输出契约无效率轴）✓；`collect.mjs`（c:/tmp/retro-metrics）：REPO 硬编码、classify 四+1 桶、墙钟首末事件差、govArtifact+govConfig 求和口径、写回 c:/tmp ✓。
- 结构门复跑：`landing-fidelity-check.mjs --preflight` → `ok:true`、`section2BodyFound:true`、`findings:[]`、`advisories:[]`、`anchorCount:13`，anchorUnion 13 成员与 landing-manifest 围栏块逐 token 一致 ✓。各 T 目标文件并集独立手算 = 13 路径，与 manifest 一致 ✓。
- 差分测试独立复现（非采信起草者结论）：`C:/tmp/tpfixture/delivery/taskpackages/261023-fixture-x` 注入 `escapeRef` 前后各跑 `node .claude/scripts/taskpackage-required-check.mjs <tp>/taskpackage.json`，两轮输出同形，唯一 `missing` 均为 `input-brief.md`，无未知键报错（复测后夹具已还原为无 escapeRef 态）✓。源码侧旁证：required-check 无顶层未知键白名单校验，`acceptance-coverage`/manifest 校验均只读具名字段。
- 反向影响面亲核：`verify.mjs`、`supervisor-gate-check.mjs`、`supervisor-verdict-gate.mjs` 对 `eventType`/pod-events 零读取（grep 实测零命中）；`closeout-check.mjs:269-274` 对 pod-events 仅 advisory 存在性检查；`loop-metrics.mjs:237` 只过滤 `dispatch-completed`（不受新类型影响）。EVENTS_SCHEMA_CUTOFF 消费方（closeout-check/dispatch-stamp/journal-append/loop-health-report/loop-metrics）全部只把它当日期谓词用，扩事件类型不破坏其语义。「只观测、不 blocking、不改门」两条在改法层守住（T7 告警不拦截、OQ3=(a) 不加牙、§6-8 自律件、scope-map/verify-gates/settings 零触）。
- scope-map 亲核：planner allow 含 `delivery/taskpackages/**`，pod-stamp 程序 fs 追加与 dispatch-stamp 同通道、无新写权需求 ✓；`escapeRef` 全仓 grep 零撞名 ✓；`.claude/loop-retro/` 现存 ledger.jsonl，T1 同目录纪律成立 ✓；`merge-driver.mjs:1629` `--self-check-merge-friction` 在场 ✓。

**Findings**：

- **F1 · SHOULD** —— OS §5d:139 现行句「Verifier rounds、gate outcomes、test results、supervisor verdict 不进入 pod-events，仍以 immutable verifier evidence / supervisor verdict 为权威」与 T4 的 `pod-closeout.gateFires` 段（verifier 红轮数、supervisor 最终裁决、mutation 计数写入 pod-events payload）直接冲突，而 T2 的「OS §5d 同步」清单（闭集枚举、两新事件 payload schema、fail-open 一句）未列对本句的修订。落地后 canonical 自相矛盾，后手实现者可据 :139 拒绝 gateFires 载荷。修法方向：T2 的 OS §5d 同步范围补一条——:139 句加 carve-out（「`pod-closeout` 的 `gateFires` 为收口时点自 immutable 制品的派生快照，权威仍在原制品；本句指原始 gate outcomes 不落 pod-events」即可，语义与 T4「不造第二账本」一致）。
- **F2 · SHOULD** —— 验证集第 8 条基线计数错误：实测 `node .claude/scripts/taskpackage-required-check.mjs --self-check` → `{"selfCheck":"taskpackage-required-check","pass":23,...,"total":23,"ok":true}`（changeid-prefix 4 项无误），PROPOSAL 写「基线 18 项」。核验官按 18 对表会误判。修法：改 23，或统一写「以 FROZEN 时点实测计数为准」并如实记 23。
- **F3 · SHOULD** —— T1 证据句「模型字符串普查（见本节开头）：13 种写法并存」与 §2 普查「12 种 model 取值」自相矛盾；实测 12 种（11 具名 + `(none)`）。修法：T1 证据句改「12 种」。
- **F4 · SHOULD** —— 验证集第 6 条断言「`nonCanonical` 列表非空（该包含历史异形类型）」对 `261023-honcho-memory-wiring-restoration` 在 eventType 轴不成立：实测该包事件类型 = human-decision×2 / friction×3 / dispatch-completed×4 / agent-escalation×1 / merge-completed×1，T7 九值闭集后 `eventTypes.nonCanonical` 预期为**空**；非 canonical 仅在 phase 轴成立（实测 `acceptance-freeze`×1、`acceptance-marker-fix`×1）。断言不点名轴，核验时要么误红要么空过。修法：第 6 条改为「`phases.nonCanonical` 非空（acceptance-freeze / acceptance-marker-fix 两值）；`eventTypes.nonCanonical` 预期为空」。
- **F5 · SHOULD** —— 验证集第 9 条自相矛盾：写「七字段非缺失」但只枚举六个字段名（epochId / openedAt / models / harness / dispatchEnvelope / reconstructed）；T1 字段清单为七项（另有 `note`）。`note` 必填与否两处不一致，机械断言无法照写。修法：钉死 `note` 可选，断言改「六必填字段非缺失 + note 可选」，或七字段全列。
- **F6 · NIT（不阻冻结）** —— phase 普查「前五 = acceptance 47 / build 40 / integrate 33 / closeout 17 / fix 14」排名错位：实测并列第五为 `acceptance-freeze` 17，`fix` 14 实为第七（前六 = (none) 52 / acceptance 47 / build 40 / integrate 33 / closeout 17 / acceptance-freeze 17）。所引各计数值本身准确、§2 与 T7 同源，仅「前五」措辞失真。修法：改「前六」并补 acceptance-freeze，或删排名只留计数值。
- **F7 · NIT（不阻冻结）** —— 两处落地执行面模糊：① §6 第 5 条「主线程在该条目追加本批指针注记」指向 `docs/debt/gov/ARQ-261014-…md`，该文件不在 manifest 13 行内、不在 §3 sweep 表内——若随 landing packet 同 commit，landing-fidelity-check 会以 scope-creep 误红；② T7 的 13 值 canonical phase 集合只存在于代码导出（CANONICAL_PHASES），OS §5d 同步只加 fail-open 句未写明集合的权威落点（按「机械面枚举的锚点纪律」应指向脚本导出而非在 OS 复制名册）。修法方向：① §6-5 钉死「独立 chore commit、不进本批 manifest」（与 INDEX backfill 先例同通道）；② OS §5d 补一句指向 `dispatch-stamp.mjs` 导出的 CANONICAL_PHASES。

**判档与升级**：治理面机器面（scripts / agents / commands / references）恒 Track B，本批轨道自评（Track B 主路径、非轻路径）成立；riskTier medium 成立（触 G3 承重脚本四件 + 新增三件，全线只观测不改门；G1 零触、契约面零触亲核无误）；Track A 段判据两条均不命中（非跨系统结构性、方向已由 human 两步走决定收敛）。无升级 human 项——上列 findings 均为起草层修订，不动改法方向。

verdict: BLOCK(F1..F5)


### R2 - 2026-10-24 - architect

**轮次纪律**：只核 R1 findings（F1–F7）修订到位性 + 是否引入新矛盾，不做全量重核。修订后版本 = DRAFT v0.2（Status 行实测）。

**逐条核验（行号为 v0.2 现行行号，均亲读原文）**：

- **F1（修订到位）**：T2 `:101` 补 :139 carve-out——替换句明确「原始 gate outcomes / test results 不落 pod-events，权威仍在 immutable 制品；`pod-closeout` 的 `gateFires` 是收口时点自这些 immutable 制品派生的**快照**（advisory derived），不构成第二事实源、不进任何判定链」。核：① 原句的过度枚举（「supervisor verdict 不进入 pod-events」字面）被撤换，字面冲突消解；② gateFires 上游 = write-once 制品（supervisor-verdict.json EEXIST fail-loud / verifier rounds immutable，R1 已亲核），快照无漂移源，「不构成第二事实源」成立；③「不进任何判定链」与 OQ3=(a)（`:261-265`）、§6-8 自律件一致，无新增牙。T4 `:143` 首条已加交叉引用；§3 `:223` OS 行说明已补 carve-out。三处落点齐。
- **F2（修订到位）**：验证集第 8 条 `:297` 改「基线 23 项 + changeid-prefix 4 项」并附双实测注记；与本判官 R1 实测 `pass:23,total:23` 一致。
- **F3（修订到位）**：T1 证据 `:65` 改「12 种取值并存」，与 §2 `:48`、T1 问题段 `:59` 单一口径；全文 grep 无「13 种」残留。
- **F4（修订到位，机械可判）**：`:295` 改两轴点名——eventTypes.nonCanonical 预期为空（五类事件逐类列举，与本判官 R1 普查实测逐类吻合）；phases.nonCanonical 非空且点名 `acceptance-freeze` / `acceptance-marker-fix`（各 1 条，与实测一致）。断言从「列表非空」含糊态变为两轴各可判真/假。
- **F5（修订到位）**：T1 `:71` 钉「六项必填 + `note` 可选（在场时须为字符串）」；验证集第 9 条 `:299` 同步为「六必填字段非缺失、note 可选」。两处口径闭合，断言可照写。
- **F6（修订到位）**：§2 `:47` 与 T7 `:199` 两处同源改「计数最高者 = (none) 52 / acceptance 47 / build 40 / integrate 33 / closeout 17 / acceptance-freeze 17（fix 14 紧随其后）」，删「前五」排名；与 R1 实测排序逐值一致。
- **F7①（修订到位）**：§6-5 `:307` 钉死「独立 chore commit」并写明 scope-creep 误红机理与 INDEX backfill 先例通道。
- **F7②（修订到位，源码归属核成立）**：T7 `:209` 改为 pod-events-append.mjs 只导出 `VALID_EVENT_TYPES`；`:210` `CANONICAL_PHASES` 归 dispatch-stamp.mjs 导出、权威落点 = 代码导出；`:212` OS 指向导出、明写「不在 OS 复制名册」；§3 `:226` / `:228` 同步。亲核现行源码：`VALID_EVENT_TYPES` 现居 `pod-events-append.mjs:29`（事件类型写权归属正确）；`CANONICAL_PHASES` 为新增导出（设计规格），dispatch-stamp 是 phase 唯一写方、归属成立；§3 复扫先验 `:242` 的模式集仍指向 pod-events-append 旧常量面，与修订不冲突。

**新矛盾扫描（聚焦修订面）**：① carve-out「不进任何判定链」↔ T6 仪表盘消费——仪表盘是只读输出非判定链，无矛盾；② carve-out 撤换后 :139 剩余语面（human decision/correction append 句）不受影响；③ F7② 的导出分工与 `:211` 聚合消费面（eventTypes/phases 两段各取所辖导出）对齐；④ preflight 复跑（本判官，v0.2）：`ok:true`、`section2BodyFound:true`、`findings:[]`、`anchorCount:13`，anchorUnion 与 manifest 13 行一致——行号位移未破坏结构字段；⑤ Status 行已升 DRAFT v0.2、landing-manifest 与 packet-id 行在场（GREEN 前结构字段核对通过）。

**判档与升级**：维持 Track B 主路径、riskTier medium；无新升级项。

verdict: GREEN

---

**起草注记（非判官区）**：

1. **preflight 实跑（2026-10-24，本 worktree `gov/kidpal`）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261024-gov-review-retro-efficiency-instrumentation/PROPOSAL.md` → `ok:true`、`section2BodyFound:true`、`findings:[]`、`advisories:[]`、`anchorCount:13`；`anchorUnion` 13 成员 = `.claude/agents/planner.md`、`.claude/commands/loop-retro.md`、`.claude/loop-retro/epochs.jsonl`、`.claude/references/dev-loop-operating-system.md`、`.claude/references/dev-loop.md`、`.claude/references/task-intake-and-slicing.md`、`.claude/scripts/dispatch-stamp.mjs`、`.claude/scripts/lib/diff-buckets.mjs`、`.claude/scripts/merge-driver.mjs`、`.claude/scripts/pod-events-aggregate.mjs`、`.claude/scripts/pod-events-append.mjs`、`.claude/scripts/pod-stamp.mjs`、`.claude/scripts/retro-efficiency.mjs`，与 landing-manifest 围栏块逐 token 一致；各 T 锚行注记无路径记号夹带。
2. **路径形态族谱（`path-form-family.md`）**：本批无搬迁 / 改名 / 路径串改写，形态族谱表查表义务不适用，在此声明。
3. **承重引文 byte 对账**：本 PROPOSAL 全部 file:line 引据由 Read / Grep 输出直接复制或经 `node -e` 逐条 `includes` 比对（20 条引文核对，初跑 2 条因省略源文里的 markdown 加粗标记与大小写不严格匹配，已改为逐字片段并复跑通过）。普查数字（事件 / 类型 / phase / 模型 / 制品覆盖）均为 2026-10-24 现跑，命令与数值见 §2 开头。
4. **长文分块落地**：本件分三次落笔（头部 + T1–T2、T3–T5、T6–T7 + §3–§7），落盘后已核行数、抽读首尾并 grep 噪音特征（见下一条）。
5. **自身产物机检**：`wc -l` 与 `grep -c "目标文件："` 核对 = T 条目 7 条、锚行 7 条；`grep -n "<!-- PART"` 零命中（分块哨兵已清除）；全文无机器串接残留。preflight 于上述第 1 条复跑为最终态（本注记属非判官区，不进入 §2 锚扫描）。
6. **R1 修订（2026-10-24，v0.2）**：F1 → T2「OS §5d 同步」补 `:139` 派生快照 carve-out，T4 首条加交叉引用，§3 sweep 该行同步；F2 → §5 第 8 条基线改 23（起草期复跑 `taskpackage-required-check --self-check` 实测 `pass:23,total:23`）；F3 → T1「模型取值普查」改 12；F4 → §5 第 6 条改为点名两轴（eventTypes 预期为空、phases 点名 `acceptance-freeze` / `acceptance-marker-fix`，两值经 261023 包实测）；F5 → T1 字段清单钉死「六必填 + `note` 可选」，§5 第 9 条同步；F6 → §2 与 T7 两处排名措辞改为「计数最高者」并列全列；F7 → §6 第 5 条钉死独立 chore commit、T7 canonical phase 权威指向 `dispatch-stamp.mjs` 导出的 `CANONICAL_PHASES`（OS 不复抄名册），§3 对应两行同步。§7 评审记录（判官 append-only 区）未改动。
