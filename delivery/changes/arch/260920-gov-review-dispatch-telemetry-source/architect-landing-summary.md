# architect-landing-summary · 260920-gov-review-dispatch-telemetry-source

packet-id：FROZEN PROPOSAL v1.0 §2 改法集（T1–T5）+ §5 验证集 · 评审轮次 **R1 `打回`（3 SHOULD + 3 COULD + 2 NIT，无升级 human）→ v0.2 全折入 → R2 `GREEN`（2 advisory 不阻，标冻前折入 N3/N4）** · change-id 260920-gov-review-dispatch-telemetry-source · human 立项三裁：范围 a+b+c 一并 / 通道采 transcript / **轨级改判全批 Track B**（human 2026-09-20）

## 1. 落点对照（manifest 6/6）

| manifest 路径 | T 项 | 落了什么 |
|---|---|---|
| `.claude/scripts/lib/transcript-telemetry.mjs` | T1（新增） | 新建只读库，导出 `projectsRoot()` / `locateDispatchTranscript()` / `readDispatchTelemetry()` / `readMainThreadUsage()` / `selfCheck()`。跨**全部**项目目录按 agentId 定位（E8：不假设同目录）；按 `message.id` 去重（E3 的 2× 陷阱）；四路分项 + `total = input + output`；生效模型透传。四条硬约束落实现：① 失败即降级不抛出（`{found:false, reason}`）② `KIDPAL_CLAUDE_PROJECTS_DIR` 可覆盖 ③ 无模型名白名单 ④ **谓词钉死顶层 `o.type === 'assistant'`**（R2-N4，注释写明 `message.type` 恒为 `"message"`、误用即静默得 0）。自带 `--self-check`，**15 条夹具**（含「混入 `isSidechain:true` 干扰项」与「误取 `message.type` 的反向正控」）。 |
| `.claude/scripts/dispatch-stamp.mjs` | T2 | 接入该库，**只在 post-cutoff 分支**（pre-cutoff legacy 路径零变更）。取值优先级 transcript → 旗标 → `unavailable`；命中时置 **`status:'exact'` + `source:'transcript'`**（R1-S1 承重词）；`harnessReported` 交叉核对（`--token-total` / legacy `--tokens`，**不是四路旗标**，R2-N3）；`payload.model` = 生效模型（transcript 优先、`--model` 降为回退）+ 新增 `modelSource`；新增 `payload.mainThreadUsage`（累计检查点，**仅命中时带该键**——见 §3 落地期发现 1）；新增 `harnessReportedSingle()` / `AGENT_ID_SHAPE` / `mainThreadUsagePayload()` 三个辅助；`--dispatch-id` 不具 agentId 形态且未命中时打 **stderr advisory（fail-soft、exit 0 不变）**。 |
| `.claude/scripts/pod-events-aggregate.mjs` | T5 | 新增 `aggregateModels()`（`withModel` / `byModel` / **`bySource`——transcript 通道失效的机械观察点**，R2-C3）与 `aggregateMainThread()`（累计检查点序列 + **`cumulativeDelta`** = 首尾做差）。**只增键**，`tokens` / `actors` / `escalations` / `friction` 四键一字未动。 |
| `.claude/agents/planner.md` | T3 | `[DISPATCH-STAMP]`（`:60`）追加：**`--dispatch-id` 必须传 harness 回传的 agentId**、三项遥测自动派生、**不需也不应手喂 `--token-input` 系列**、`--model` 降为回退、取不到即降级不阻断；`[MODEL-TIER]`（`:59`）末句由「若可得须补记」改指**自动派生**并注明该人工欠账已机械化。原「**绝不传 0 充数**」原句一字未改。**frontmatter 零 diff**（hunk 仅 `@@ -59,2 +59,2 @@`）。 |
| `.claude/references/dev-loop-operating-system.md` | T4 | `:132` 单源由「只接受 harness exact 值」改为**按来源分档**（`transcript` = 仓内归因最忠实记录 / **非计费真值**；`harness` = 回退值与交叉核对）+ 取值优先级 + **降级纪律**（不得因遥测缺失阻断或延迟派发）。**原句四条实质约束逐字保留**（R1-C2 核定数：`total = input + output`、`cache 字段不重复计入`、`unknown 绝不写 0`、`harness-reported-zero`）。 |
| `.claude/references/dev-loop.md` | T5 | `:88` 脚本目录行补上 transcript 派生、生效模型（`model` + `modelSource`）与主线程累计三项能力，保持单行格式。 |

```landing-manifest
.claude/scripts/lib/transcript-telemetry.mjs
.claude/scripts/dispatch-stamp.mjs
.claude/scripts/pod-events-aggregate.mjs
.claude/agents/planner.md
.claude/references/dev-loop-operating-system.md
.claude/references/dev-loop.md
```

## 2. 验证集执行记录（§5 九项）

| 项 | 命令 / 方法 | 结果 |
|---|---|---|
| V1 库自检非空绿 | `node .claude/scripts/lib/transcript-telemetry.mjs --self-check` | **`{"pass":16,"fail":0,"total":16,"ok":true}`、exit 0** ✓（首轮落地时为 15 条，**缺主线程侧去重真控制**——该缺项经独立核验判 **RED**，已补并复证，见 §5）。含派发侧五条 + **主线程侧五条（含去重真控制）** + 谓词反向正控 + 空目录哨兵 |
| V2 端到端命中 | `CLAUDE_PROJECT_DIR=<临时> node .claude/scripts/dispatch-stamp.mjs --task-package-id 260920-scratch-telemetry-test --dispatch-id aaecbb47025725868 …`（**真实 agentId**） | ✓ 产出含 **`status:'exact'` + `source:'transcript'`** + 四路 `input 36776 / output 53 / cacheRead 65024 / cacheWrite 0` + `total 36829` + `harnessReported 36560` + `model: deepseek-v4-flash` + `modelSource:'transcript'` + `mainThreadUsage`（累计 `input 133642 / output 89841 / cacheRead 11371520 / total 223483`、`messageCount 83`）。**人读核对①**：四路值与库对同一 agentId 直接现算**逐数相同**（`36776/53/65024/0`）✓ |
| V2b 人读核对②（R1-S2） | 同上 + 复跑一次比对 `mainThreadUsage` | ✓ 另一时点得 `input 131568 / output 83227 / cacheRead 10149120 / total 214795`、`messageCount 77`——**后一次 ≥ 前一次（单调不减成立）**，且两值均 == 对同一 sessionId 同口径现算值 |
| V3 降级不阻断 | `--dispatch-id builder-r1`（历史自造标签） | ✓ `tokenUsage.status === 'unavailable'` + reason 在场；**exit 0**；stderr 出 advisory（点名「不像 harness agentId 且未命中 transcript」） |
| V4 向后兼容重放 | 同命令、新参数零传、`--dispatch-id legacy-style-id` | ✓ 产出 payload 键集 = `["dispatchId","durationMs","phase","terminalStatus","tokenUsage","toolUses"]`，与在册历史同形态事件**逐键相同**（键集比对，非「看着像」）；`tokenUsage` 值同为 `{status:'unavailable',reason:…}` |
| V5 聚合消费方兼容 | `node .claude/scripts/pod-events-aggregate.mjs --change-id 260920-m2-charwindow-card-fix` | ✓ 顶层新增 `models` / `mainThread`；**既有 `tokens` / `actors` / `escalations` / `friction` 四键全在、`tokens` 形状未变**（`dispatchesTotal/dispatchesWithTokenData/dispatchesUnavailable/tokenTotals`） |
| V6 政策↔实现一致 | grep 四条约束 + `git diff --numstat` | ✓ 四条逐字各命中 1 处；`planner.md` numstat `2/2`、hunk 仅 `@@ -59,2 +59,2 @@`（**frontmatter 未触**） |
| V7 形态门干跑 | `node .claude/scripts/landing-fidelity-check.mjs --preflight <PROPOSAL>` | ✓ `ok:true` / `anchorCount 6` / `findings: []`；`anchorUnion` 六路径 == manifest 六路径；锚行注记无路径 token（起草期 / 标冻前 / 落地后各一次，均绿） |
| V8 无门污染 | `git status --porcelain` + `git diff --stat` | ✓ 改动集**恰 = manifest 六路径** + `delivery/changes/arch/INDEX.md`（簿记行，**在 packet 外**，承批③ `589c35e` 先例）+ 本变更目录。**scope-map / settings / hooks / contracts 零改动** |
| V9 真机可读性 | 落地后**真实派发**（核验官本次，agentId `a700c78ac3289d5ee`）stamp 进本变更自身 pod-events → `node .claude/scripts/pod-events-aggregate.mjs --change-id 260920-gov-review-dispatch-telemetry-source` | ✓ **一条命令取齐三项**：`tokens.tokenTotals {input:82540, output:0, cacheRead:2349824, cacheWrite:0, total:82540}`（四路）+ `models.byModel {deepseek-flash:1}` 且 `bySource {transcript:1}`（生效模型 + 来源）+ `mainThread.last {input:147116, output:112087, cacheRead:18036864, total:259203, model:deepseek-flash}`（主线程累计）。**判定边界 = token 账，不含挂钟归因**（见 PROPOSAL §0 口径界定） |

**真空绿规约核对**：V1 的 `fixture-nonempty` 夹具**先断夹具真落盘**再跑断言，并有 `selfcheck-fixture-actually-empty-sentinel`（空目录必得 `found:false`）防「夹具其实是空的也全绿」；去重夹具**先断不去重会得 70、去重得 30**（真控制而非恒真）；谓词夹具**先断误取 `message.type` 得 false** 再断正取得 true。V4 有前置事实（在册历史事件键集可查）。

## 3. 落地期发现（起草期未预见，如实记录）

1. **§5 #4 与 T2 的自相矛盾被验证集自己抓住**：T2 原文写「新增 `payload.mainThreadUsage`」，未写明是否无条件带键；而 §5 #4 要求老调用序列产出**逐字段一致**。落地时若无条件写 `mainThreadUsage`，老形态事件会凭空多出一个 `{status:'unavailable', reason:'no-dispatch-transcript'}` 段，**使 §5 #4 字面不成立**。**处置 = 只在命中派发 transcript 时带该键**——未命中时连 `sessionId` 都无从得知（连接键就是 agentId），带一个恒 unavailable 的段只是添噪。该处置**不是改法变更**，而是取本件**既有约定**的同一形态：`--model` 注释原文即「不传则 **不带该键**（向后兼容…）」。处置后 V4 逐键相同实测通过。**已记入 PROPOSAL Status 行「FROZEN 后 packet 内修订」**（不扩改法内容、六条目标路径一字未变）。
2. **`harnessReported` 的在场条件是「调用方仍传了单值」**：V2 传了 `--token-total 36560` 故在场；V3/V4 未传故整个键缺失。与既有一致——**不制造伪值**。
3. **主线程累计值的两个时点差**：V2 与 V2b 相隔数分钟，`total` 由 214,795 增至 223,483（+8,688）。**这是该字段设计语义的直接体现**（累计检查点随会话推进单调增长），同时提示：**该数本身的绝对值不对应任何「某次派发的消耗」**，只有**同包首尾做差**（`cumulativeDelta`）才有归因意义——retro 使用时勿误读绝对值。
4. **本批自身即为「仓外未文档化通道」的第一个真实样本**：落地后 `mainThreadUsage` 的首批数据即由 gov worktree 会话自身产出；`model` 字段实测为 `deepseek-flash`（主会话）与 `deepseek-v4-flash`（子 agent）两种写法并存——**再次坐实「不做模型名白名单」是必要约束**，任何映射表都会在第一次遇到命名漂移时就产生错值。

## 4. 落地后的簿记项（packet 外）

- ARQ 行 `ARQ-260909-dispatch-stamp-telemetry-gaps` 三条子项 a / b / c **全结** → 结移 §2（随本批 commit 执行，承批③ `589c35e` 先例，**不进 landing-manifest**）。子项 b 需在结移行注明**其实况与原建议不符**（旗标面 / 文档 / 校验三处早已在位，真缺口在上游数据源）。
- `changes-index.mjs --upsert` 状态行已随标冻翻 `frozen`；合 main 后须跑 `--backfill`（「已合main」列被状态缓存钉死，承 §8 第 5 条经验）。

## 5. 独立核验与 RED 闭环（`landing-verifier`，2026-09-20）

**首轮判定：`FAIL`（1 RED）** —— **本批唯一一处真实缺陷，由核验官独立抓出，不重叠于判官两轮**（判官抓的是论证与条款，核验官抓的是实现与夹具）。

### RED · §5 #1 主线程侧「`message.id` 去重」夹具缺失（可复现）

- **expected**：§5 #1 明文要求主线程侧覆盖「`message.id` 去重后累计正确」（§5 经 packet-id 行纳入 packet，措辞为「夹具**须**覆盖」）。
- **actual**：首轮夹具 `p1` / `p2` / `sc1` 各 `blocks=1`、id 互异 ⇒ **主线程侧去重零覆盖**。核验官给出的最小复现：把 `readMainThreadUsage` 里的 `const records = dedupeByMessageId(principal);` 改成 `const records = principal;`，自检**仍 15/15 全绿**——即该路径的核心计算「真错了也不会红」。
- **根因（主线程自陈）**：实现期误以为主线程去重由派发侧共用同一函数覆盖，故独漏该夹具；而 R1-S2 的本意恰是「主线程侧 过滤 + 去重 + 累计 **三要素**零覆盖」。**这正是本仓「真空绿规约」要防的『看起来对』**——本条缺陷与批次主题（遥测静默失真）同族。
- **处置（已闭环）**：① 主线程夹具 `p2` 改落 **3 个块（同 `message.id`）**，新增断言 `main-thread-dedupe-no-inflation`（不去重会得 input 16 / messageCount 4，去重后为 10 / 2），并把 `messageCount === 2` 并入既有主线程断言；② **复跑核验官的最小复现**——注入 mutant 后自检**确已变红**：`{"pass":13,"fail":3,"total":16,"ok":false}`、exit 1、`failures` 含 `main-thread-dedupe-no-inflation`；③ 修复后正常自检 `{"pass":16,"fail":0,"total":16,"ok":true}`、exit 0。
- **§2 V1 措辞校正**：首轮 summary V1 自称「主线程侧四条」属**覆盖断言过声明**，已改为「五条（含去重真控制）」并注明该缺项经核验判 RED、已补并复证。

### 其余核验结论（摘要，全文见 `landing-verdict.md`）

- **结构门 GREEN**：`proposalTargets == summaryTargets == manifest` 六路径，`unauthorized: []`、`claimedNotLanded: []`、`plannedNotLanded: []`。
- **逐文件语义核对 6/6 GREEN**：六文件改动逐条 == FROZEN §2 T1–T5，无私货、无越界。
- **landing 期修订（`mainThreadUsage` 仅命中时带键）被判为「断言校准」而非「未授权改法变更」**（四项理由见 verdict §4）。**残留 ADVISORY（如实记录）**：真 agentId 的老序列仍会多出该键，故 §5 #4 的「逐字段一致」**严格只对自造 id 重放成立**。
- **四破坏性控制均真红/真命中**：去重真控制（移除即 3 fail）、`status:'exact'` 承重词（真派发实测命中）、谓词陷阱（改 `message.type` 即 8 fail）、降级契约（exit 0 + 不带 `mainThreadUsage`）。
- **V5 兼容**：旧版 vs 新版聚合在 3 包上**剔除 `models`/`mainThread` 后逐字节相同**。

### 已处置的 ADVISORY

| # | advisory | 处置 |
|---|---|---|
| 1 | §5② 夹具缺失 | **已闭**（上文 RED 闭环） |
| 2 | `pod-events-aggregate.mjs` `aggregateEscalations` 首行被并成一行（纯空白，语义等价） | **已修**（该行是主线程早前一次误操作留下的格式损伤，非有意改动；恢复后 `node --check` 通过、既有输出不变） |
| 3 | summary §2 V9 行「见 §2 尾注」为悬引 | **已修**（V9 行改为给出实际命令与逐字结果） |
| 4 | 库导出面大于 §2 T1「导出两个只读函数」措辞 | **接受并记录**：实际导出 5 项（多出 `projectsRoot` / `locateDispatchTranscript` / `selfCheck`）。属**实现细化的良性扩张**——三者均为可测性所需（核验官正是靠导入原函数做破坏性自检），不改变任何行为断言、不动任何目标路径。**如实记录，不静默** |
