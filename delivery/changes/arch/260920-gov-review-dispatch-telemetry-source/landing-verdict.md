# landing-verdict · 260920-gov-review-dispatch-telemetry-source

**Verdict：首轮 `FAIL`（1 RED）→ 复核轮 `PASS`（RED 已闭环）**——最终结论见文末「复核轮次（R 轮）」节 §R.7；上文 §0–§7 为首轮留痕。首轮 RED = §5 #1 主线程侧② 夹具在落地实现中缺失（可复现）；§2 改法本身始终忠实落到位。

- 核验官 = landing-verifier（只读 codebase，只写本文件；零上下文独立推导，不采信 landing-summary 自述）。
- 核验时点：2026-09-20 · worktree `C:/AI/KidPal-gov`（branch `gov/kidpal`）。
- 基线 = **FROZEN PROPOSAL v1.0**（`PROPOSAL.md` §2 改法集 T1–T5 + §5 验证集 + landing-manifest 6 路径），**不是任何 commit**。
- 落地形态：6 个 canonical 文件改动均在**工作树、未 commit**；`.codebase-memory/` 两件为机器再生面脏、不计入；`delivery/changes/arch/INDEX.md` 为 packet 外簿记行（承批③/批④ 先例）。
- 轨：**Track B 常驻轻路径**（PROPOSAL「review 轨」行）；review 证据目录无 `decision-packet.md`（`ls` 实证：仅 `PROPOSAL.md` + `architect-landing-summary.md`），故步骤 2 的 `proposal-consolidation-check.mjs` **不跑**。

---

## 0. 两门

### 0.1 结构门（机器）——GREEN

命令（工作树改动未 commit，故以 HEAD 为 baseRef 纳入未提交差异）：

```
node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260920-gov-review-dispatch-telemetry-source HEAD
```

关键输出（verbatim）：

```
  "ok": true,
  "gate": "landing-fidelity",
  "baseRef": "HEAD",
  "frozen": true,
  "packetBinding": true,
  "proposalTargets": [
    ".claude/scripts/lib/transcript-telemetry.mjs",
    ".claude/scripts/dispatch-stamp.mjs",
    ".claude/scripts/pod-events-aggregate.mjs",
    ".claude/agents/planner.md",
    ".claude/references/dev-loop-operating-system.md",
    ".claude/references/dev-loop.md"
  ],
  "summaryTargets": [ ...同上 6 路径，逐字一致... ],
  "unauthorized": [],
  "claimedNotLanded": [],
  "plannedNotLanded": [],
  "gitDegraded": false,
  "citationAnchorAdvisory": { "checked": 28, "problems": [], ... },
  "verbatimQuoteAdvisory": { "checked": [], "missing": [] },
  "advisory": { "changedCanonicalNotInProposal": [], ... }
```

`proposalTargets == summaryTargets == manifest 6 路径`；`unauthorized` / `claimedNotLanded` / `plannedNotLanded` 均空；`frozen:true`、`packetBinding:true`、`gitDegraded:false`。**退 0**。

### 0.2 设计侧门（机器）——N/A

Track B（见上）。「frozen PROPOSAL ↔ 批定一致性」前提由 §7 评审记录段 `### R2 … 结论：GREEN` + Status 行 FROZEN 承载。

---

## 1. 逐文件语义核对（人读 diff == §2 改法）—— 6/6 GREEN

| manifest 路径 | 结构 | semanticMatch | expected（来自 FROZEN §2） | actual（来自工作树 diff） |
|---|---|---|---|---|
| `.claude/scripts/lib/transcript-telemetry.mjs`（新增，G3） | untracked 新文件 341 行 | **ok** | T1：`readDispatchTelemetry`（按 agentId 跨全部项目目录定位、四路分项、`total=input+output`、生效模型、sessionId、messageCount、**按 `message.id` 去重**）+ `readMainThreadUsage`（`isSidechain !== true` 过滤 + 去重 + 累计）+ 四条硬约束（①失败降级不抛 ②`KIDPAL_CLAUDE_PROJECTS_DIR` 覆盖 ③无模型白名单 ④谓词钉死顶层 `o.type === 'assistant'`）+ `--self-check` | 逐条落实（见 §3 破坏性自检）。额外导出 `projectsRoot` / `locateDispatchTranscript` / `selfCheck` 为内部辅助，benign |
| `.claude/scripts/dispatch-stamp.mjs`（T2） | +100/-... | **ok** | 取值优先级 transcript→旗标→unavailable；命中置 `status:'exact'` + `source:'transcript'` + 四路；`harnessReported`（`--token-total`/legacy `--tokens`）；`payload.model` 取生效模型 + 新增 `modelSource`（`transcript`\|`flag`）；新增 `payload.mainThreadUsage`（累计）；非 agentId 未命中打 stderr advisory（fail-soft）；pre-cutoff 零变更 | 全部落实；pre-cutoff 分支仅变量改名（`tokenUsage`→`flagTokenUsage`）语义不变 |
| `.claude/scripts/pod-events-aggregate.mjs`（T5） | +64 | **ok** | 只增键：`models`（模型分布 + 有值条数）与 `mainThread`（检查点序列 + 末次累计）；既有键不改 | `aggregateModels` + `aggregateMainThread` 新增，顶层加 `models`/`mainThread`；既有 `tokens`/`actors`/`escalations`/`friction` 四键形状逐一比对**未变**（§2 亲跑，见 §3.5） |
| `.claude/agents/planner.md`（T3） | numstat `2/2` | **ok** | `[DISPATCH-STAMP]`（`:60`）钉 `--dispatch-id` 必须 = harness agentId、说明自动派生、`--model` 降回退；`[MODEL-TIER]`（`:59`）末句改指自动派生；「绝不传 0 充数」原句保留；**不动 frontmatter** | 两行逐字落实；`绝不传 0 充数` 原句在场；hunk（`-U0`）= `@@ -59,2 +59,2 @@` 仅正文两行，**frontmatter 零 diff** |
| `.claude/references/dev-loop-operating-system.md`（T4） | +7/-1 | **ok** | `:132` 按来源分档（transcript 非计费真值 / harness 回退）+ 取值优先级 + 降级纪律；**原句四条实质约束一条不删** | 四条逐字在场（`total = input + output` / `cache 字段不重复计入` / `unknown 绝不写 0` / `harness-reported-zero`）+ 降级纪律句新增 |
| `.claude/references/dev-loop.md`（T5） | +1/-1 | **ok** | `:88` 目录行补 transcript 派生 + 生效模型 + 主线程累计，保持单行 | 单行扩展落实 |

**私货扫描**：`git status --porcelain` 的 canonical 面恰 = manifest 6 路径 + `delivery/changes/arch/INDEX.md`（簿记行，`git diff` 亲读为单行 `frozen` 新增，packet 外）+ 本变更目录（untracked）+ `.codebase-memory/` 两件（机器再生面，批外常态脏）。**scope-map / settings / hooks / contracts 零改动**。无越界、无夹带、无权限/写域扩张。

---

## 2. packet-binding 核对 —— GREEN

- summary packet-id 行 =「FROZEN PROPOSAL v1.0 §2 改法集（T1–T5）+ §5 验证集 · R1 `打回` → v0.2 全折入 → R2 `GREEN` · change-id 260920-gov-review-dispatch-telemetry-source」↔ FROZEN §2+§5 一致。
- landing 实际范围 ⊆ 被批 packet：`unauthorized:[]`（结构门机判）+ 本核验官 diff 逐文件抽查，无超出 §2 的改动。

---

## 3. 破坏性自检记录（命令 + 逐字输出）

### 3.1 库自检基线（V1）

```
$ node .claude/scripts/lib/transcript-telemetry.mjs --self-check
{"selfCheck":"transcript-telemetry","pass":15,"fail":0,"total":15,"ok":true}
EXIT=0
```

### 3.2 反向正控 A · 去重是真控制（非恒真）

把库副本的 `dedupeByMessageId` 改成直通（`return records;`），同夹具复跑：

```
{"selfCheck":"transcript-telemetry","pass":12,"fail":3,"total":15,"ok":false,"failures":[{"name":"dispatch-hit","ok":false},{"name":"dispatch-dedupe-no-inflation","ok":false},{"name":"dispatch-total-is-input-plus-output","ok":false}]}
EXIT=1
```

⇒ 去掉去重后 `dispatch-dedupe-no-inflation` 变红（input 从 30 虚增到 70）——**该夹具是真控制，不是恒真**。

### 3.3 反向正控 B · `status:'exact'` 承重词真命中（真派发端到端）

用**本核验官自身这次派发的 agentId `a700c78ac3289d5ee`**（`meta.json` 逐字：`{"agentType":"landing-verifier","description":"Landing 保真度独立核验","requestShape":"background",...}`）跑真实派发（`CLAUDE_PROJECT_DIR` 指向仓外 temp，避免污染在册包）：

```
$ CLAUDE_PROJECT_DIR=c:/tmp/kidpal-verify node .claude/scripts/dispatch-stamp.mjs --task-package-id 260920-scratch-telemetry-verify --dispatch-id a700c78ac3289d5ee --actor landing-verifier --phase verify --duration-ms 4200 --tool-uses 12 --terminal-status ok --token-total 99999 --model sonnet
dispatch-stamp: resolved changeId = 260920-scratch-telemetry-verify (from --task-package-id)
[dispatch-stamp] a700c78ac3289d5ee: landing-verifier/verify 4200ms -> pod-events.jsonl (tokens: exact/transcript, model: deepseek-flash (transcript))
EXIT=0
```

产出行（verbatim，节选 payload）：

```
"tokenUsage":{"status":"exact","source":"transcript","input":62100,"output":0,"cacheRead":689664,"cacheWrite":0,"total":62100,"harnessReported":99999},"model":"deepseek-flash","modelSource":"transcript","mainThreadUsage":{"source":"transcript","input":143416,"output":107542,"cacheRead":16379264,"cacheWrite":0,"total":250958,"model":"deepseek-flash","messageCount":106}
```

逐项确认：`status:"exact"` ✓（R1-S1 承重词命中）；`source:"transcript"` ✓；四路分项 ✓；`total == input+output`（62100）✓；`harnessReported:99999`（= 我传的 `--token-total`，与四路旗标无关）✓；**生效模型优先 transcript**——我显式传了 `--model sonnet` 仍得 `model:"deepseek-flash"` + `modelSource:"transcript"` ✓；`mainThreadUsage` 在场 ✓。

**人读核对①（派发侧，稳定 transcript 复算）**：取同源稳定 transcript `agent-aaecbb47025725868.jsonl`，库值 vs 独立重算（不经库）：

```
INDEPENDENT  : {"input":36776,"output":53,"cacheRead":65024,"cacheWrite":0,"total":36829,"messageCount":3}
LIBRARY      : {"input":36776,"output":53,"cacheRead":65024,"cacheWrite":0,"total":36829,"messageCount":3}
MATCH: true
raw(no-dedupe)= 6  deduped= 3  ratio= 2.00
```

⇒ 库值与独立重算**逐数相同**；raw 6 → 去重后 3（恰 2×，E3 陷阱实证）。

**人读核对②（主线程侧，独立重算）**：对同一 sessionId（`63ffe407-030f-4de3-9301-51e57a7a95e8`）主会话文件按同口径（`o.type==='assistant'` + `isSidechain!==true` + `message.id` 去重 + 累计）独立现算：

```
INDEPENDENT main-thread: {"input":143416,"output":107542,"cacheRead":16379264,"cacheWrite":0,"total":250958,"messageCount":106}
EMITTED (from stamp):    {"input":143416,"output":107542,"cacheRead":16379264,"cacheWrite":0,"total":250958,"messageCount":106}
```

⇒ 与产出的 `mainThreadUsage` **逐数相同**。（注：对**仍在增长**的自身派发 transcript 复算会因时点漂移得直读值 ≠ 该 stamp 值，如 62100→71603——属 transcript 单调增长，非缺陷，与 PROPOSAL E6 注记同源。）

### 3.4 反向正控 C · 谓词陷阱（真红）

把库副本谓词改成 `o.message.type === 'assistant'`（值恒为 `"message"`）：

```
{"selfCheck":"transcript-telemetry","pass":7,"fail":8,"total":15,"ok":false,"failures":[{"name":"dispatch-hit",...},{"name":"dispatch-dedupe-no-inflation",...},{"name":"dispatch-total-is-input-plus-output",...},{"name":"dispatch-effective-model-surface",...},{"name":"dispatch-sessionid-from-record",...},{"name":"main-thread-hit-and-sidechain-excluded",...},{"name":"main-thread-total-and-model",...},{"name":"predicate-rejects-message-type-trap","ok":false}]}
EXIT=1
```

⇒ 误取 `message.type` 会**静默得 0**（8 项红）——实现用的是顶层 `o.type === 'assistant'`（源码 `transcript-telemetry.mjs:41-43` `isAssistantRecord`），**正确**。

### 3.5 反向正控 D · 降级契约（不抛、exit 0、不阻断）

用历史自造标签跑同一命令：

```
$ CLAUDE_PROJECT_DIR=c:/tmp/kidpal-verify node .claude/scripts/dispatch-stamp.mjs --task-package-id 260920-scratch-telemetry-verify --dispatch-id builder-r1 --actor builder --phase impl --duration-ms 100 --tool-uses 1 --terminal-status ok
[dispatch-stamp] advisory: --dispatch-id "builder-r1" 不像 harness agentId（17 位十六进制）且未命中 transcript（transcript-not-found）——本次遥测将不可归因。planner.md [DISPATCH-STAMP] 要求传 harness 回传的 agentId。
[dispatch-stamp] builder-r1: builder/impl 100ms -> pod-events.jsonl (tokens: unavailable/-)
EXIT=0
```

产出行：`"tokenUsage":{"status":"unavailable","reason":"no-token-flags-provided"}`，**无 `mainThreadUsage` 键**（仅命中时带键，见 §4）。⇒ 不抛、exit 0、stderr advisory 在场、不阻断；未命中时不带 `mainThreadUsage`。

### 3.6 V9 真机可读性（token 账一行取齐）

对上 temp 包（含 3.3 的命中行 + 3.5 的降级行）跑聚合：

```
$ CLAUDE_PROJECT_DIR=c:/tmp/kidpal-verify node .claude/scripts/pod-events-aggregate.mjs --change-id 260920-scratch-telemetry-verify
"tokens": { "dispatchesTotal": 2, "dispatchesWithTokenData": 1, "dispatchesUnavailable": 1,
            "tokenTotals": { "input": 62100, "output": 0, "cacheRead": 689664, "cacheWrite": 0, "total": 62100 } },
"models": { "dispatchesTotal": 2, "withModel": 1, "byModel": { "deepseek-flash": 1 }, "bySource": { "transcript": 1, "(none)": 1 } },
"mainThread": { "checkpointsTotal": 1, "first": {...}, "last": { "input": 143416, ..., "total": 250958, "model": "deepseek-flash" }, "cumulativeDelta": null, "checkpoints": [ ... ] },
"escalations": {...}, "friction": {...}
EXIT=0
```

⇒ 一次聚合输出里**一行取齐**：四路分项（`tokens.tokenTotals`）+ 生效模型（`models.byModel`）+ 主线程累计（`mainThread.last`）。**判定边界 = token 账，不含挂钟归因**（PROPOSAL §0 口径界定 / §5 #9）。`bySource` 亦在场为 transcript 通道失效的机械观察点。

### 3.7 V5 既有键兼容（老包复跑）

对在册包 `260920-m2-charwindow-card-fix` 跑聚合：顶层键 = `ok,changeId,eventsTotal,malformedLines,tokens,actors,models,mainThread,escalations,friction`；`tokens` 键 = `dispatchesTotal,dispatchesWithTokenData,dispatchesUnavailable,tokenTotals`，`tokenTotals` 键 = `input,output,cacheRead,cacheWrite,total`——**四既有键一个不少、形状不变**。

进一步：对 3 个数据较富的包，用 `git show HEAD:.claude/scripts/pod-events-aggregate.mjs` 的旧版 vs 新版逐一比对**剔除 `models`/`mainThread` 后的全部输出**：

```
== 260915-m1-n1b-p1-home-battery ==  existing-keys-identical: true  esc: true fric: true tok: true act: true
== 260911-screen-unification ==      existing-keys-identical: true  esc: true fric: true tok: true act: true
== 260918-gov-review-closeout-chain-friction == existing-keys-identical: true  esc: true fric: true tok: true act: true
```

⇒ 既有输出**逐字节相同**。

### 3.8 V7 形态门干跑

```
$ node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260920-gov-review-dispatch-telemetry-source/PROPOSAL.md
"ok": true, "anchorCount": 6, "anchorUnion": [ ...6 路径，== manifest... ], "findings": []
```

---

## 4. RED · §5 #1 主线程侧② 夹具在落地实现中缺失（可复现）

- **failingFile**：`.claude/scripts/lib/transcript-telemetry.mjs`
- **expected**：FROZEN §5 #1 主线程侧② 逐字为「**`message.id` 去重后累计正确**」——`--self-check` 夹具须在**主线程侧**注入重复 `message.id` 并断言去重后累计正确（§5 为 packet 成员：本 PROPOSAL 的 packet-id 行 =「§2 改法集（T1–T5）+ §5 验证集」）。landing-summary §2 V1 亦自述「含派发侧五条 + **主线程侧四条**」。
- **actual**：库的自检**主线程夹具无任何重复 `message.id`**（`transcript-telemetry.mjs:220-224` 的 `p1`/`p2`/`sc1` 各 `blocks=1`、id 互异），故主线程侧去重**零覆盖**。可复现证明：**只**把 `readMainThreadUsage` 的去重调用去掉（`transcript-telemetry.mjs:157` `const records = dedupeByMessageId(principal);` → `const records = principal;`），自检仍**全绿**：

```
$ node <仅主线程去重被移除的库副本> --self-check
{"selfCheck":"transcript-telemetry","pass":15,"fail":0,"total":15,"ok":true}
EXIT=0
```

- **category**：批定验证集没落全（**§5 #1 主线程侧② 遗漏**）+ landing-summary 的 V1「主线程侧四条」覆盖断言与实际不符（**断言过声明**）。
- **根因假设**：主线程侧的「去重」与「累计求和」共用同一把 `message.id` 阈值，实现期把去重夹具**误以为**由派发侧的 `dispatch-dedupe-no-inflation` 覆盖（二者共用 `dedupeByMessageId`），故主线程夹具只补了 `isSidechain` 过滤 / `累计求和` / 降级三条，**独漏去重**；而 R1-S2 折入该条的**本意**正是「主线程侧 `readMainThreadUsage` 的核心计算（过滤 + 去重 + 累计）零覆盖」，去重恰是其三要素之一。
- **最小复现步骤**：1) `cp .claude/scripts/lib/transcript-telemetry.mjs c:/tmp/x.mjs`；2) 将 `c:/tmp/x.mjs:157` 的 `const records = dedupeByMessageId(principal);` 改为 `const records = principal;`；3) `node c:/tmp/x.mjs --self-check` → 得 `pass:15, ok:true`、exit 0（应有红而无红）。

**后果**：`readMainThreadUsage` 的主线程去重一旦回归（漏去重 → 主线程累计虚增），本批自建的自检**不报红、不降级、静默给错值**——正是本批主题（遥测静默失真）与 R1-S2 要根治的那类「看起来对」。子项 c（主线程自身观测）的落点正确性因此少一道本批指定的守卫。

**处置建议**：在主线程夹具注入一条与 `p1` 同 `message.id` 的重复块（或一条 `blocks:2` 记录）并断言去重后累计不虚增；同步校正 landing-summary §2 V1 的「主线程侧四条」措辞。修毕复跑 `--self-check` 应仍 `15/15`（或按新增夹具数）且**移除主线程去重调用须变红**。

> **注（本核验官判断的边界）**：本 RED 只针对**§5 验证集**。**§2 改法（T1–T5）本身逐条落到位、是 GREEN**（§1 表 + §3 破坏性自检为证）；即本批的**行为**是对的（主线程去重实现正确、经独立重算逐数吻合），缺的是 FROZEN §5 指定、R1-S2 明确要求的那**一条守卫夹具**。若人类/编排判 §5 夹具条目为非阻断的加固项而非「须」，可下调为 ADVISORY——但 §5 已被本 PROPOSAL 明文纳入 packet（packet-id = §2 + §5），且 §5 #1 用「夹具**须**覆盖」，故本核验官按 RED 报。

---

## 5. landing 期修订独立判断（§4 问题委托）—— 判为「断言校准」，非「改法偏离」

**被判断的对象**：PROPOSAL Status 行记的「FROZEN 后 packet 内修订 1 处」——`payload.mainThreadUsage` 在场条件由隐含的「无条件带键」明确为**「仅命中派发 transcript 时带该键」**（实现：`dispatch-stamp.mjs:283-285,309` 的 `telemetry.found ? … : null` + 条件展开）。

**结论：仍在冻结文本的语义内，属「断言校准」而非「未授权的改法变更」。** 理由：

1. **冻结文本从未明写「无条件带键」**。§2 T2 原文只写「新增 `payload.mainThreadUsage`：主线程截至本次 stamp 的累计四路分项 + 生效模型」，未规定在场条件；「无条件带键」是**未写出的默认读法**，非被冻结的改法内容。改的是**一个从未成文的默认**，不是成文的改法条文。
2. **该修订消除的正是冻结文本的自身矛盾**。§2 T2 向后兼容段逐字要求「不传新参数的老调用序列产出与改前**逐字段一致**」，§5 #4 亦要求重放「产出字段集与改前**逐字段一致**（键集比对）」。若**无条件**带 `mainThreadUsage`，老形态事件会凭空多出一个键——**无条件读法会使 §5 #4 字面不成立**。两个成文条款相冲突时，取「使另一条可满足」的读法是**校准而非偏离**（landing-summary §3 发现 1 与 Status 行已如实记录该推理，本核验官独立复推一致）。
3. **形态取自本件既有约定**。`dispatch-stamp.mjs` 同一 `payload` 内 `--model` 注释即「不传则**不带该键**（向后兼容…）」；`mainThreadUsage` 缺席时连 `sessionId` 都无从得知（连接键就是 agentId），带一个恒 `unavailable` 段只是添噪。取同一形态**自洽**。
4. **不扩不改任何改法内容、六条目标路径一字未变**（结构门 `proposalTargets == summaryTargets == 6 路径` 实证）。
5. **有实测支撑**：§3.6 证明命中时键在场、§3.5 证明未命中时键缺席，且 §3.7 证明既有消费方输出逐字节不变。

**残留注记（ADVISORY，非 RED）**：「仅命中时带键」下，一条老调用序列若其 `--dispatch-id` **恰是真 agentId 且命中 transcript**（PROPOSAL §3 census 里 91/142 命中），仍会多出 `mainThreadUsage` 键——故 §5 #4 的「逐字段一致」严格而言只对**自造 id 那部分**重放成立，不对「真 agentId 的老序列」成立。此为冻结文本残留的**作用域边界**，非本处修订引入的新偏离；如实登记。

---

## 6. ADVISORY 清单（不拦 PASS 但须如实记）

1. **【RED 关联】** §5 #1 主线程侧② 夹具缺失（见 §4）——已按 RED 报；若下调为 advisory，须在主线程去重回归时能变红方可闭合。
2. **`pod-events-aggregate.mjs` 一处纯空白改动**：`aggregateEscalations` 的函数首行与 `const escalations = …` 被并成一行（`pod-events-aggregate.mjs:219`）。JS 语义等价（§3.7 旧版 vs 新版既有输出**逐字节相同**为证），不违「只增键」、无行为影响；属插入块伴生的空白折叠，建议后续顺手恢复换行。（承 reader-scripts-align verdict 之「插入性空行」同类。）
3. **landing-summary §2 V9 行的「见 §2 尾注」为悬引**：summary §2 表 V9 行末写「见 §2 尾注（…）」，但 §2 节内并无「尾注」段（`architect-landing-summary.md` §2 表后仅「真空绿规约核对」段）。V9 实况见本 verdict §3.6。建议 summary 补该尾注或改指本 verdict。
4. **库导出面大于 §2 T1 措辞**：T1 写「导出**两个**只读函数」，实现另导出 `projectsRoot` / `locateDispatchTranscript` / `selfCheck`。均为内部辅助 / 既有 `--self-check` 形态（`acceptance-run-check.mjs` 等 10+ 脚本同型），benign、不改变任何行为；仅记以备后来者勿误读「只两个」。

---

## 7. 结论

实际落地 diff == FROZEN PROPOSAL §2 改法集（T1–T5）：**6/6 文件语义保真**，无私货、无越界、无与批定矛盾；落地期那 1 处修订经独立判断属**语义内的断言校准**（非改法偏离）。结构门 / 形态门绿，破坏性四控（去重真控制、`status:'exact'` 承重词、谓词陷阱、降级契约）**均真红/真命中**，V9 一行取齐成立。

但 **§5 #1 主线程侧②（主线程侧 `message.id` 去重夹具）在落地实现中缺失**，且可复现（§4：移除 `readMainThreadUsage` 的去重调用后 `--self-check` 仍 `15/15` 绿）。§5 经本 PROPOSAL 明文纳入 packet 且用「夹具**须**覆盖」，故本核验官按 **FAIL** 报——**不放行**下一步（commit 分支 / retro / 前置升级闸 / human merge），退主线程修 landing（补该夹具 + 校正 summary V1 措辞）后重核。

merge 拍板归 human；本核验官只给 verdict。

---

# 复核轮次（R 轮）· 2026-09-20 · 同一核验官实例，上下文连续

> **最终 Verdict：PASS**（首轮 FAIL 的唯一 RED 已闭环；其余各面复核无回归）。**上文 §0–§7 为首轮留痕**，结论以本轮为准。

## R.1 RED 闭环复验（独立重跑，不采信主线程自述）

主线程按本核验官首轮给的最小复现修毕。本核验官**独立重跑**：

**正常态**（`.claude/scripts/lib/transcript-telemetry.mjs` 现态）：

```
$ node .claude/scripts/lib/transcript-telemetry.mjs --self-check
{"selfCheck":"transcript-telemetry","pass":16,"fail":0,"total":16,"ok":true}
EXIT=0
```

**最小复现重跑**（注入 mutant：把 `readMainThreadUsage` 里 `const records = dedupeByMessageId(principal);` 改成 `const records = principal;`）：

```
$ node <mutant> --self-check
{"selfCheck":"transcript-telemetry","pass":13,"fail":3,"total":16,"ok":false,"failures":[{"name":"main-thread-hit-and-sidechain-excluded","ok":false},{"name":"main-thread-dedupe-no-inflation","ok":false},{"name":"main-thread-total-and-model","ok":false}]}
EXIT=1
```

⇒ **RED 已闭**：移除主线程去重**现在会红**（3 条断言，含新增的 `main-thread-dedupe-no-inflation`），正常态仍全绿。夹具 `p2` 现落 **3 个块（同 `message.id`）**（`transcript-telemetry.mjs:224`），去重后 `input 10 / messageCount 2`、不去重则 `16 / 4`。

## R.2 生产逻辑零回归（逐面复核）

- **库生产函数未动**：`transcript-telemetry.mjs:30-173`（`projectsRoot` / `isAssistantRecord` / `dedupeByMessageId` / `readDispatchTelemetry` / `readMainThreadUsage` 等）与首轮**逐字节相同**；本轮改动只落在 `selfCheck()` 夹具区（`:219-290` 一带）。
- **`dispatch-stamp.mjs` 未动**：`status:'exact'`（`:259`）、`source:'transcript'`（`:260`）、`harnessReportedSingle`（`:168`）、`AGENT_ID_SHAPE`（`:178`）、`mainThreadUsagePayload`（`:181`）、`telemetry.found` 门（`:257/:283/:289`）、`modelSource`（`:277`）均在原位，与首轮一致。
- **`planner.md` / `dev-loop-operating-system.md` / `dev-loop.md` 未动**：numstat 仍 `2/2`、`7/1`、`1/1`，与首轮一致。
- **三脚本 `node --check` 全过**（dispatch-stamp OK / aggregate OK / library OK）。
- **结构门复跑仍 GREEN**：`{"ok":true,"frozen":true,"packetBinding":true,"unauthorized":[],"claimedNotLanded":[],"plannedNotLanded":[],"gitDegraded":false}`、`citationAnchorAdvisory.problems:[]`，exit 0。
- **`pod-events-aggregate.mjs` 既有键兼容复跑仍成立**：旧版 vs 新版在 `260915-m1-n1b-p1-home-battery`、`260918-gov-review-closeout-chain-friction` 上**剔除 `models`/`mainThread` 后逐字节相同**（`existing-keys-identical: true`）。
- **改动面仍恰 = manifest 6 路径**（canonical 面）+ `INDEX.md` 簿记行（packet 外）+ 本变更目录 + 本变更自身 `delivery/taskpackages/260920-gov-review-dispatch-telemetry-source/`（V9 所需，见 R.3）；scope-map / settings / hooks / contracts 零改动。

## R.3 V9 真机可读性复验（真派发实测，逐字）

主线程已把本核验官这次派发 stamp 进本变更自身 pod-events（`delivery/taskpackages/260920-gov-review-dispatch-telemetry-source/pod-events.jsonl`，`dispatchId` = `a700c78ac3289d5ee`，`actor` = landing-verifier）。本核验官**独立重跑**聚合，逐字核 summary V9 的数字：

```
$ node .claude/scripts/pod-events-aggregate.mjs --change-id 260920-gov-review-dispatch-telemetry-source
tokens.tokenTotals  = {"input":82540,"output":0,"cacheRead":2349824,"cacheWrite":0,"total":82540}
models.byModel      = {"deepseek-flash":1}   models.bySource = {"transcript":1}
mainThread.last     = {"input":147116,"output":112087,"cacheRead":18036864,"total":259203,"model":"deepseek-flash"}
```

⇒ **一条命令取齐**四路分项 + 生效模型 + 主线程累计，且与 summary §2 V9 行**逐数吻合**；该行「见 §2 尾注」的悬引已消除（现为实际命令 + 逐字结果）。**判定边界 = token 账，不含挂钟归因**。

## R.4 两处 ADVISORY 复核

- **ADVISORY 2（`aggregateEscalations` 首行并行）已修**：`pod-events-aggregate.mjs:219-220` 现为 `function aggregateEscalations(events) {` + 换行 + `  const escalations = …`——格式已复原；既有输出逐字节不变（R.2 复跑为证）。
- **ADVISORY 3（V9 悬引）已修**：见 R.3。

## R.5 ADVISORY 4 判定（委托：良性扩张 vs 改法偏离）——**判为良性扩张，可接受**

库实际导出 5 项，多于 §2 T1「导出**两个**只读函数」措辞。本核验官判定**不构成改法偏离**，理由：

1. `selfCheck` **非扩张**——§2 T1 逐字要求「**自带 `--self-check`**（本仓既有形态：`acceptance-run-check.mjs` / `landing-fidelity-check.mjs` / `merge-driver.mjs` 等 10+ 脚本均有该入口）」，导出 `selfCheck` 是该硬性要求的落地形态。
2. `projectsRoot` / `locateDispatchTranscript` 是**内部辅助函数**，因可测性导出；二者正是 §2 T1 硬约束②（`KIDPAL_CLAUDE_PROJECTS_DIR` 覆盖）与 E8（跨目录定位）的实现体，导出只**增加可见性、不增加行为**。
3. **不扩权限、不改写域、不动任何目标路径**；不改任何行为断言（六文件 diff 未含它们，manifest 六路径一字未变）。
4. 属「实现细化」，非「改法变更」——同 reader-scripts-align verdict 对「插入性空行」的处置同族（记载而不阻断）。

⇒ 主线程「接受并如实记录（写进 landing-summary §5）」的处置**可接受**。

## R.6 landing 期修订判定不变

「`payload.mainThreadUsage` 仅命中时带键」仍判为**「断言校准」而非「未授权改法变更」**（首轮 §4 四项理由不变；本轮 `dispatch-stamp.mjs` 该逻辑逐字未动）。残留 ADVISORY 保留：真 agentId 的老序列仍会多出该键，故 §5 #4「逐字段一致」严格只对自造 id 重放成立。

## R.7 最终结论

**PASS**。首轮唯一 RED（§5 #1 主线程侧② 主线程去重夹具缺失）已按本核验官最小复现闭合，且**移除该去重现会变红**（R.1）；RED 之外首轮已判 GREEN 的各面（结构门 / 逐文件 6/6 / 四破坏性控制 / V5 / V9）**无回归**（R.2–R.4）；landing 期修订判定不变（R.6）；ADVISORY 4 判为良性扩张（R.5）。

实际落地 diff == FROZEN PROPOSAL §2 改法集（T1–T5）+ §5 验证集，无私货、无漏改、无与批定矛盾。**放行建议：满足「可进 commit 分支 → retro → 前置升级闸 → 合并本地 main」的前置**（结构门 GREEN + 本 verdict PASS + §5 九项 V1–V9 齐）；merge 拍板归 human manual-merge 门。
