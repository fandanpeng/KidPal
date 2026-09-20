verdict: PASS

# landing-verdict · 260920-gov-review-citation-anchor-residue

- 独立核验者：landing-verifier（只读 codebase，只写本文件）
- 核验时点：2026-09-20
- PACKET = FROZEN PROPOSAL v1.0（§2 改法集 T1–T3 + §5 验证集）+ landing-manifest（PROPOSAL 头部围栏块 3 路径）+ architect-landing-summary
- 落地者叙述不采信：以下结论一律来自确定性脚本退出码 + 逐文件 diff 实证。
- 结论：**PASS**（放行建议；merge 归 human / 前置升级闸）。附 ADVISORY 4 条（均不改行为、不拦合并）。

---

## 0. 两门

### 0.1 结构门（机器）——GREEN

命令：`node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260920-gov-review-citation-anchor-residue`

关键输出（verbatim）：

```
"ok": true,
"frozen": true,
"packetBinding": true,
"proposalTargets": [
  ".claude/scripts/landing-fidelity-check.mjs",
  ".claude/scripts/lib/governance-gate-shared.mjs",
  "docs/templates/PROPOSAL.template.md"
],
"summaryTargets": [ ...同 3 项... ],
"unauthorized": [],
"claimedNotLanded": [],
"plannedNotLanded": [],
"gitDegraded": false,
```

- `proposalTargets` 三项 = landing-manifest 三行，逐字一致。
- `unauthorized` / `claimedNotLanded` / `plannedNotLanded` 均空 → 无 scope creep、无 claimed-but-not-landed、无 planned-but-not-landed。
- `gitDegraded: false` → 结构门机检未降级。

### 0.2 设计侧门（机器）——N/A

本批为 **Track B 常驻轻路径**（PROPOSAL §「review 轨」）；review 证据目录**无 `decision-packet.md`**（`ls` 实证：仅 `PROPOSAL.md` + `architect-landing-summary.md`）。按本角色步骤 2，Track B 下 `proposal-consolidation-check.mjs` **不跑**；「frozen PROPOSAL ↔ 批定一致性」前提由 §7 评审记录段 R3 `verdict: GREEN` + Status 行 FROZEN 承载。

---

## 1. 逐文件语义核对（人读 diff == §2 改法）

对 landing-manifest 每个 canonical 文件读**实际改动**（`git diff HEAD -- <file>` 全文 + untracked 新文件全读），逐一比对 §2 改法。

| file | structuralTouched | semanticMatch | expected（来自 FROZEN §2） | actual（来自 diff） |
|---|---|---|---|---|
| `.claude/scripts/landing-fidelity-check.mjs` | 是（`229/79`） | **ok** | T1①②③④ + T2 + T3(a)(c) | 见 §1.1 |
| `.claude/scripts/lib/governance-gate-shared.mjs` | 是（`63/3`） | **ok** | T3(b) | 见 §1.2 |
| `docs/templates/PROPOSAL.template.md` | 是（`6/5`） | **ok** | T3 搭车件（示范行还原 + 读法注改写 + 纪律句） | 见 §1.3 |

### 1.1 `landing-fidelity-check.mjs`（G3 面，逐条核）

- **T1 改法 1（候选域改由 git 索引面派生）**：新增 `gitCitationPrefixes()`，`for (const args of [['ls-files', '-z'], ['ls-files', '-z', '--others', '--exclude-standard']])`（第 120 行）——**一行两式齐备**；`addPath` 逐段累积祖先目录（`segs.pop()` 去文件名）。**落到位**。
- **T1 · `CITATION_BASE_PREFIXES` 去留**：常量**未删**（第 90 行定义），仅被 `staticCitationPrefixes`（回退分支）消费（第 95 行 `new Set(CITATION_BASE_PREFIXES)`）；正常 git 派生路径不引用它。与改法 1「不得整条删除、保留于回退分支」**一致**。
- **T1 改法 2（失败即留痕）**：`citationPrefixes()` 返回 `{ prefixes, source }`，正常 `'git-index'` / 回退 `'fallback-static'`；`gitCitationPrefixes` 任一调用 `status !== 0` 或 stdout 非 string → 返回 `null` → 转回退并留痕。`prefixSource` 真接进观察列（advisory 第 7 字段，见 §1.1-观察列）。**落到位**。
- **T1 改法 3（按 dir 缓存保留）**：`citationPrefixCache` 保留，`key = resolve(dir)`，命中即返回缓存 entry（含 source）。**落到位**。
- **T1 改法 4（多命中不再取首个——承重连带项）**：旧 `for (...) { if (exists) { resolvedBy = prefix; break; } }` **已整段删除**；新实现 `const hits = prefixes.filter(...)` **收集全部命中**，`hits.length === 1` → `resolvedByPrefix`，`hits.length > 1` → `ambiguousShortForm`。**落到位**（自检 `ambiguousShortForm` 命中数 ≥3 断言佐证，见 §2 V2）。
- **T2 六步判定顺序（钉死）**：代码顺序 = ① `if (existsSync(abs))`（字面直中，含 `line-out-of-range` / `file-unreadable`，原样保留）→ ② `/[…»]|\.\.\./` **`abbreviated`（在试探之前）** → ③ `hits.length === 1` `resolvedByPrefix` → ④ `hits.length > 1` `ambiguousShortForm` → ⑤ `relPath.includes('/') && topLevelDirs.has(relPath.split('/')[0])` 落 `problems` 的 `file-not-exist` → ⑥ 其余 `shortFormUnresolved`（附补救句）。**`abbreviated` 确在 `attemptPrefixes` 之前**。（②⑤ 两处的 delta 见 §7 复验。）
- **T2 · `problems` 语义**：`line-out-of-range`（`if (line > lineCount)` 分支）与 `file-unreadable`（`catch` 分支）**原样保留**，只在「字面直中」分支内产生，不进六步判定。与第 101 行 §2 更正表述一致。
- **T2 · 「一级目录集合」同源**：`const topLevelDirs = new Set(prefixes.map((p) => p.split('/')[0]))`，`prefixes` 取自 `citationPrefixes(projectDir)`——**与候选域同源同缓存**，未另造事实源。
- **观察列输出形态**：`checkCitationAnchors` 返回 7 字段 `{ checked, problems, resolved, abbreviated, ambiguousShortForm, shortFormUnresolved, prefixSource }`；`citationAnchorAdvisory` = `{ checked, problems, resolvedByPrefix, abbreviated, ambiguousShortForm, shortFormUnresolved, prefixSource }`——**字段名 `problems` / `resolvedByPrefix` 保持不变**。7 字段齐备。
- **T3 子项 a（文件绝对行号）**：`boldLines.push(i + 1 + sec.offset)`；`section2Body()` 返回 `{ body, offset, inFence }`，`offset = start + 1`。**落到位**（自检断言 `行号·报文件绝对行号` 期望 6、`行号·与环境真身行号一致` 期望 6，均绿）。
- **T3 子项 b（跳围栏）**：`preflightFindings()` 对 `sec.inFence[i]` 的加粗探针 `return`；`section2Body()` 与 `parseSection2AnchorLists()` 两处都跳围栏。**落到位**（lib 侧见 §1.2）。
- **T3 子项 c（anchorUnion 可见面）**：`runPreflight()` 新增 `anchorUnion`（排序成员名单），`ok: findings.length === 0` **一字未动**、`anchorCount = union.length`。**纯可见面、非判据**成立。
- **私货扫描**：diff 内**未见**把任何门判据从 advisory 升 fail（`citationAnchorAdvisory` 仅进输出 JSON，**从未进 `errors.push`**——见 §3）；**未见** §2 未写的判定分支。

### 1.2 `governance-gate-shared.mjs`（T3(b)——本批唯一被门消费的改动）

- 新增 `fenceLineFlags(lines)`：三反引号开合跟踪（`/^\s*```/.test(line)` 行切换 `inFence`，**开合行自身**标记为 true）。
- `parseSection2AnchorLists()`：条目 `lines` 改存 `{ text, fenced }`；锚行扫描 `filter((l) => !l.fenced)`。条目标题扫描对 `flags[i]` 行 `continue`（围栏内不产条目头，防幻影条目吞其后真锚行）。
- **重点核：有没有把真锚行一起误跳**：围栏状态机只在 ``` 行翻转，真锚行（在围栏外）`flags=false` 不被跳。**反向正控夹具**（`fencedPhantom`：围栏内 `### T1 · 围栏内示范条目` + 围栏后真 `### T1 · 真条目` + 真锚行）断言 `fence-phantom-no-swallow`（union = `['.claude/scripts/real.mjs']`）**绿**。另一活体验证：`--preflight docs/templates/PROPOSAL.template.md` → `ok:false` + `anchorCount:0` + `anchorUnion:[]` + 仅 1 条 `missing-anchor-list（§2 无任何 T 条目）`——canonical 模板 §2 的 fenced 示范块**未造幻影条目**（§2 无真条目，报告回到诚实结论）。**未把真锚行一起误跳**。
- 自检 +6 断言（带 backtick / 裸写 / 幻影不吞锚三族），`section2Anchors` 19 → 25。

### 1.3 `docs/templates/PROPOSAL.template.md`（T3 搭车件）

- **示范行还原裸 `目标文件：`**：fenced 示范块行 `-〔第五段·锚清单〕目标文件：<裸路径...>` → `+目标文件：<裸路径...>`。**撤销变形标记、与门同形**。
- **读法注 1 改写**：改为「形态沿革」完整交代（变形标记的来由 + 由本批 T3 子项 b 修复 + 撤销 + 250911 教训）。**落到位**。
- **纪律句（读法注第 3 条）**：新增 `锚行注记不引路径 token`（`grep` 命中 1，见 §2 V7）。与 §5 V7「读法注第 3 条」预期一致。
- **`〔第五段·锚清单〕` 残留**：`grep` 命中 **1 处**，**仅在形态沿革说明句内**（逐字回引该标记的历史），**无活体使用**。与 §5 V7 的「落地实况校准」表述一致。
- 另含 2 处超 §2 字面的 doc 编辑（读法注标题重标 + 【起草期自检两句】① 补 `anchorUnion`/绝对行号）——见 ADVISORY A2。

---

## 2. 验证集独立复跑（V1–V7，逐项实测 vs landing-summary）

| 项 | 命令 | 我的实测值 | landing-summary 所写 | 一致 |
|---|---|---|---|---|
| V1 | `--preflight <本件>` | `ok:true` / `section2BodyFound:true` / `anchorCount:3` / `anchorUnion:3`（= manifest 三行）/ `findings:[]` | 同 | ✓ |
| V2 | `--self-check` | 十路全绿，**总 131**（citation-anchors **19** / landing-preflight **17** / section2Anchors **25** / 其余 7 路 = 基线：extractTargets 6 / resolveOrigin 29 / computeFrozenPin 4 / changeid-prefix 4 / lint-descriptions-trigger 2 / verbatim-quotes 6 / isFrozen 19） | 112 → 131（+19），19 / 17 / 25 | ✓ |
| V3 | `route-regression-check.mjs` | `28 ok, 0 fail` | 同 | ✓ |
| V4 | `<批③ 目录> 173d734` | `ok:true` / `prefixSource:git-index` / `problems:[]` / `abbreviated:[…/PROPOSAL.md:126]`（1）/ `ambiguousShortForm:[progress.md:18]`（1，**5 命中**：3 个 `_archive/2026/*` + `m1-home-qa` + `m2-char-window`）/ `shortFormUnresolved:0`；`resolvedByPrefix` **raw 12 / distinct 6**（= 旧实现 10 + 本批新解析 2，两条均 → `delivery/workstreams/`）。**旧实现同命令：`problems` = 恰那 4 条**（`progress.md:18` / `m1-home-qa/decision-coverage.json:116` / `_archive/2026/260911.../decision-coverage.json:113` / `…/PROPOSAL.md:126`），**新实现归零** | 「resolvedByPrefix 2（两条均 → delivery/workstreams/）」；其余同 | ✓（见注） |
| V4b | 本件自身；旧代码 = `git show HEAD` 版 | **改前（旧实现）**：`problems` raw **41** / distinct **16**、`resolvedByPrefix` **0**（承载断言复现）；**改后（新实现）**：去重切分 `resolvedByPrefix 2` / `abbreviated 8` / `ambiguousShortForm 4` / `file-not-exist 2`（去重 16）+ 1 条字面直中 = 去重 17 | 「改前 resolvedByPrefix 0、全部进 problems」「改后 2 / 8 / 4 / 2」 | ✓ |
| V5 | `CLAUDE_PROJECT_DIR=<非 git 临时副本> <批③ 目录>` | `prefixSource:**fallback-static**`（留痕生效）/ `resolvedByPrefix:[]`（**不静默假绿**）/ `abbreviated:[…/PROPOSAL.md:126]`（仍缩写）/ `progress.md:18` 及两条 decision-coverage 型由分类落 `shortFormUnresolved`（回退域无 `delivery/` ⇒ 0 命中） | 同（fallback-static / resolvedByPrefix 0 / 分类漂移如文档） | ✓ |
| V6 | `git status --porcelain`（剔 `.codebase-memory/`）+ `git diff --numstat` | **恰 3 路径 = manifest 3/3**；numstat `229/79`、`63/3`、`6/5` | 同 | ✓ |
| V7 | 语义面 grep | `ls-files` **1 行**（同行两式）；`CITATION_BASE_PREFIXES` **2 命中**（定义行 90 + 回退分支 95）；四类字段 **30** 命中；`fenceLineFlags` **2**（定义 350 + 调用 372）；模板 `〔第五段·锚清单〕` **1 命中**（仅形态沿革句内）；`锚行注记不引路径 token` **1 命中** | 同 | ✓ |

**V4 注**：任务给定期望「`resolvedByPrefix` 2」，landing-summary 同写 2。实测 **raw 数组 = 12 / distinct = 6**（旧实现同命令为 10）。差异**非失配**——「2」是「本批**新解析**的两条前缀省略型」这个 **delta**（旧实现 `problems` 恰含这 2 条、新实现它们转 `resolvedByPrefix`），非数组长度。§2 T1 的「4 条 file-not-exist 归零」预言**逐条吻合**（旧实现那 4 条：2 → `resolvedByPrefix`〔`delivery/workstreams/`〕、1 → `abbreviated`、1 → `ambiguousShortForm` 5 命中）。**唯一精度瑕疵在 landing-summary 的措辞**（把 delta 当数组值写），属 ADVISORY A3；改法本身无失配。

**V4b 注**：任务列 V4b 于 V8 独立核验范围，已复跑。**改前** raw 计数（41 / distinct 16）与 PROPOSAL §5 V4b 所载末值**逐字吻合**；承载断言 `resolvedByPrefix = 0`（改前）与去重切分 `2/8/4/2`（改后）**均复现**。

---

## 3. 破坏性自检 / 私货扫描

- **核验范围**：三个目标文件**全文 diff 逐条核**（未抽样、未跳敏感面）。敏感面 = 本批 G3 两脚本 + G2 模板；**未触** G1 指令根 / self-def（architect.md）/ review-loop agent def / settings / hooks / scope-map / `docs/contracts/**`。
- **advisory → fail 静默升级**：**无**。`grep` 实证 `citationAnchorAdvisory`（及其 7 字段）**只出现在输出 JSON 块**（第 687–723 行），**从不进 `errors.push`**；`errors.push` 全在既有结构门路径（未冻结 / 缺 packet-binding / 锚清单 problems / scope creep / claimed / planned）。引据面 **advisory-only 不变**。
- **§2 未写的判定分支**：**无**。`abbreviated` / `ambiguousShortForm` / `resolvedByPrefix` / `problems.file-not-exist` / `shortFormUnresolved` 五支全为 §2 T2 六步所载。
- **claimed-but-not-landed**：landing-summary §1 声称的每项改动均在真实 diff 找到对应（§1.1–1.3 逐条）。**无**声称落而实未落。
- **自己是否只挑好看的文件核**：否——3/3 文件全文核。

---

## 4. packet-binding 核对

- landing-summary 头部引 `packet-id：FROZEN PROPOSAL v1.0 §2 改法集（T1–T3）+ §5 验证集` + `change-id 260920-gov-review-citation-anchor-residue` → **== 被批 review id**。
- landing 实际范围 ⊆ 被批 packet：结构门 `unauthorized: []` + 自身 diff 抽查（V6 恰 3 路径 = manifest）**双向印证**。
- review 轨 = Track B（PROPOSAL §「review 轨」明载；两条 Track A 硬判据未命中）；OQ1=(a) / OQ2=(a) / OQ3=(c)（human 2026-09-20）。

---

## 5. ADVISORY（均不改行为、不拦合并；建议 human / architect 判是否收紧）

- **A1（未授权字符 · inert）**：`abbreviated` 判据正则实为 `/[…»]|\.\.\./`（第 175 行）——含 **`»`（U+00BB）**，而 §2 T2 改法 2 只授权「路径段含 **`…` 或 `...`**」，且其上方代码注释亦仅写「`…` / `...`」。实证 **`»` 在全仓 PROPOSAL / `delivery/**` 零出现**（`grep` 0 命中），故**对全部真实输入无行为影响**，且该分类 advisory-only、不进 `errors`。建议清理为与 §2 同形（如 `[…](\.\.\.)`）。**〔2026-09-20 复验后：已消解，见 §7〕**
- **A2（doc 超 §2 字面）**：模板除 §2 T3 搭车件明列的两处外，另改 2 处——「读法注」标题重标批次归属、【起草期自检两句】① 补 `anchorUnion` / 绝对行号说明。两处均为**与 T3(a)/(c) 一致的文档同步**、landing-summary §1 已声明，无行为影响。
- **A3（landing-summary 措辞精度）**：V4 行 `resolvedByPrefix 2` 实为**新解析 delta**，raw 数组 = 12（见 §2 V4 注）。建议改为「raw 12（其中新解析 2）」以免读者误读为数组长度。
- **A4（观察项 · 冗余条件）**：T2 第 5 步多一个 `relPath.includes('/')` 前置条件（§2 字面为「路径首段是仓内实存的一级目录」）。因 CITATION_RE 要求路径带扩展名，纯裸名不可能命中 `topLevelDirs`，该条件**恒不改变结果**（无害），且与「路径首段」的措辞相合，仅记录不拦。**〔2026-09-20 复验后：已消解，见 §7〕**

---

## 6. 最终判定

- 结构门 GREEN（`ok:true`，无 scope creep / claimed / planned 失配）。
- 逐文件语义核对 3/3 `semanticMatch: ok`（§2 改法 T1①②③④ + T2 + T3(a)(b)(c) + 搭车件**全落到位、无漏改、无与批定矛盾**）。
- 无 advisory→fail 升级、无 §2 未写的判定分支、无权限 / 写域扩张。
- 验证集 V1–V7 独立复跑**逐项与 landing-summary 实质一致**（含 V4 的 delta 口径差，已定性为措辞精度、非失配）。
- 未授权项仅 A1（inert 单字符）、A2（doc 同步），均不改任何门 / 观察列输出。

⇒ **verdict: PASS**。放行建议：可进 commit 分支 / 前置升级闸；merge 仍归 human。附 4 条 ADVISORY 供其判断是否要求落前清理（均非拦阻项）。

---

## 7. 复验（核验后 delta）· 2026-09-20

**背景**：§6 PASS 之后、commit 之前，落地者对本件 `.claude/scripts/landing-fidelity-check.mjs` 内 `checkCitationAnchors()` 做了**两处收敛改动**，针对 ADVISORY A1 / A4。本节为**独立复验**该 delta（不采信落地者叙述，只认脚本退出码 + diff 实证）。§0–§6 为 delta 前时点的记录，保持原样不动。

**delta 内容（逐字符）**：

1. **A1**：第 175 行 `if (/[…»]|\.\.\./.test(relPath))` → `if (/[…]|\.\.\./.test(relPath))`（删未授权字符 `»` U+00BB）。
2. **A4**：第 179 行 `if (relPath.includes('/') && topLevelDirs.has(relPath.split('/')[0]))` → `if (topLevelDirs.has(relPath.split('/')[0]))`（删冗余前置条件）。

### 复验项与证据

1. **逐字同形核对 —— PASS**
   - `grep -c '»' .claude/scripts/landing-fidelity-check.mjs` → **0**（零命中）。
   - `grep -c 'relPath.includes' .claude/scripts/landing-fidelity-check.mjs` → **0**（零命中）。
   - 第 175 行 byte dump：`if ( / [ 342 200 246 ] | \ . \ . \ . / . t e s t ( relPath ) )` —— 字符类内**只剩 U+2026 `…`**（`342 200 246`），`»`（`302 273`）已消。**与 §2 T2 改法 2「路径段含 `…` 或 `...`」逐字同形**。
   - 第 179 行 = `if (topLevelDirs.has(relPath.split('/')[0])) {` —— **与 §2 T2 改法 5「路径首段是仓内实存的一级目录」逐字同形**。

2. **语法 / 自检 —— PASS**
   - `node --check .claude/scripts/landing-fidelity-check.mjs` → **SYNTAX OK**（exit 0）。
   - `node .claude/scripts/landing-fidelity-check.mjs --self-check` → **十路全绿、总数 131**（`citation-anchors` **19** / `landing-preflight` **17** / `section2Anchors` **25**；其余七路 = 基线：extractTargets 6 / resolveOrigin 29 / computeFrozenPin 4 / changeid-prefix 4 / lint-descriptions-trigger 2 / verbatim-quotes 6 / isFrozen 19）。**与 delta 前逐项同值**。

3. **无其它未授权改动 —— PASS**
   - `git diff --numstat -- .claude/scripts/landing-fidelity-check.mjs` → **`229/79`**，**与 delta 前逐字相同**（两处收敛均为「改既有新增行的内容」，未增删任何行）。
   - `git diff` 的 added-lines 计数 = **229**（不变）；全 diff 内 `»` 与 `relPath.includes` **零出现**。即：diff 除上述两行内容外**无新增**（两处仅改既有新增行）。
   - **自伤修复核实**：落地者自陈「改动 1 时误删换行、把 `abbreviated` 行与 `const hits` 行并成一行，已恢复」——末态 `sed -n '170,186p'` 实证第 175(`abbreviated`) / 176(`const hits`) / 177 / 178 / 179 各为独立行，**无合并痕迹**；numstat 亦保持 `229/79`（若末态仍合并，added-lines 会 −1）。

4. **零行为变化 —— PASS（独立确认，非采信）**
   - **A1 侧**：`»` 在本件 PROPOSAL（`grep -c '»' PROPOSAL.md` → 0）与全仓 `delivery/**`（grep 0）**零出现**，故删 `»` 对任何真实输入的分类结果无影响。
   - **A4 侧**：独立复跑**前后对照**——
     - V4（批③ 复放 `173d734`）delta 前 = `problems 0 / resolvedByPrefix 12 / abbreviated 1 / ambiguous 1(5 命中) / shortFormUnresolved 0 / prefixSource git-index`；delta 后 = **完全同值**。
     - V4b（本件自身）delta 前 = `checked 44 / problems 5（2 distinct）/ resolvedByPrefix 10（2 distinct）/ abbreviated 12（8 distinct）/ ambiguous 14（4 distinct）/ shortFormUnresolved 0`；delta 后 = **完全同值**。
   - ⇒ 两处收敛**确为零行为变化**（`includes('/')` 因 CITATION_RE 要求路径带扩展名、纯裸名不可能命中 `topLevelDirs`，恒不改变结果）。

5. **对先前 PASS 的影响**：**不改变**。两处收敛恰为**消解 A1 / A4**，其余（结构门、逐文件语义、V1–V7）已在前证且不受影响。

### 复验结论

- **delta 复验：PASS**；**维持整体 verdict: PASS**。
- ADVISORY 状态更新：**A1 已消解**、**A4 已消解**；**余 A2**（模板 doc 超 §2 字面 2 处，与 T3(a)/(c) 一致的同步、已声明、无行为影响）、**A3**（landing-summary V4 行「resolvedByPrefix 2」措辞，建议改「raw 12（新解析 2）」）。
- 仍无 advisory→fail 升级、无 §2 未写的判定分支、无权限 / 写域扩张；merge 仍归 human。
