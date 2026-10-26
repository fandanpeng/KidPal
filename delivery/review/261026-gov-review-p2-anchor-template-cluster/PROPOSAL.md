# PROPOSAL · 261026-gov-review-p2-anchor-template-cluster

> **状态：FROZEN v0.1 · 2026-10-26 · R1 GREEN（0 BLOCKING / 3 NIT 非阻断，无升级项，判档 Track B 主路径确认）· packet-binding 生效：落地 = 本 §2 改法集（T1–T7）+ §5 验证集；manifest 七文件**
> **承载 ARQ**：`ARQ-261019-anchor-probe-fence-blind`（SHOULD，T1）、`ARQ-261019-verbatim-quote-line-level-false-negative`（NIT，T2）、`ARQ-260928-landing-summary-skeleton-template` + `ARQ-261021-landing-summary-template`（SHOULD ×2，**已并条消费**，T3）、`ARQ-261019-forward-derived-verification-set`（SHOULD，T4）、`ARQ-261025-verify-set-negative-positive-pairing`（SHOULD，T5）、`ARQ-261021-proposal-assert-expectations`（SHOULD，T6）、**搭载** `ARQ-261026-release-pr-gov-lane-zero-pods`（SHOULD，T7；workstream P3 项，随本批动 `.claude/scripts/**` 顺手落）。**不关**：`ARQ-260924-landing-gate-dir-anchor-gap` 残余②（目录锚展开，human 2026-10-26 裁独立批，本批零触碰）。
> **review 轨**：**预判 Track B 主路径（非轻路径），最终判档归 architect-reviewer**。依据：本批 diff 触 `.claude/scripts/**`（G3 门脚本与发布脚本）、`.claude/references/**`、`docs/templates/**`——按 `gov-change.md`「轻路径判据」③，机器面命中任一即恒 Track B。Track A 段两条判据均不命中：非跨系统结构性变更；七条修法方向由 ARQ 预消化 + human 立项 ack 收敛（唯一复证分叉 = T3 前提已过时，见 §2.0 复证 3，改法仍在 human 裁定的「文面义务、不立模板」口径内）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T7）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（一句话证成：三处行为面全部是「判定条件收窄 / 报错精度提高 / 门面文案换形」——不放宽任何 RED；但 `landing-fidelity-check.mjs` 与共享 lib 是**每个 Track B 批 landing 必经**的结构门，`release-pr.mjs` 是**每次发布必经**入口，影响面覆盖全仓，故不按 low 报）。
> **review profile**：G2+G3（`references` + `templates` / `scripts`）；**G1 零触碰**、**契约面零触碰**、**agent 定义零改**、**无新增门、无 settings / verify-gates / hooks 变更**。

```landing-manifest
.claude/references/arch-change.md
.claude/references/gov-change.md
.claude/references/track-b-judge-brief.md
.claude/scripts/landing-fidelity-check.mjs
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/release-pr.mjs
docs/templates/PROPOSAL.template.md
```

---

## 0. 想实现的效果与差距

**目标效果**：

1. **PROPOSAL 起草到 landing 的机械链判得准、说得清**：§2 里用围栏承载「原文钉死」内容（其中含 markdown 节标题）不再让锚解析提前截断、条目静默消失；围栏未闭合这类真模糊态改为 fail-loud 点名；verbatim-quote 观察列不再对「长行中部子句 + 块尾换行」恒报假阴性。
2. **两份高复发格式义务第一次进 face SSOT**：landing-summary 的两件结构义务（packet-id 行、landing-manifest 围栏块）写进 `arch-change.md`（并给 gov lane 一个指针）；验证集的三条撰写规则（实跑回填、白名单从处方正推、负向配正向）写进 `PROPOSAL.template` §5，判官简报同步抽查透镜。
3. **gov 批发布的 PR 门面不再落「0 个 Pod 合并」空话**：release-pr 按 lane 出标题/正文——gov 治理批点名 changeId 并给交付制品目录指针。

**现状差距（全部为 2026-10-26 起草期实跑或亲核，证据见 §2 各 T 与 §5 基线表）**：

- `parseSection2AnchorLists` 的 §2 边界探测不跳围栏：围栏内一行 `## ` 会让 §2 提前截断，其后 T 条目与锚行**静默消失且 problems 为空**（实测：union 只剩 1 项、problems `[]`）。同根切片点在 `landing-fidelity-check.mjs` 还有两处（preflight 的加粗锚探针、verbatim-quote 的 §2 切片）——实测前者漏报围栏后的加粗锚行，后者直接把 §2 钉死块数从 2 读成 0。
- verbatim-quote 比对取围栏块正文（含块尾换行）后做 `includes`：260925 批两条真实假阴性复现——11 个 §2 钉死块里现行报 10 条 missing，`trim()` 变体报 8 条，翻转的恰是 `docs/DEV-GOVERNANCE.md` 与 `arch-review-protocol.md` 那两句（A5 在案）。
- landing-summary 的格式义务只活在门脚本与先例文件里：260928 / 261021 两批各 FAIL 一轮（ARQ 记录）。**注意：summary 模板本身早已存在**（`docs/templates/architect-landing-summary.template.md`，260802 T2 落地，含 packet-id 行 + manifest 块 + 五件组件），protocol 七步第 4 步与 verifier 简报也都有指针——缺的是 **face SSOT（arch-change.md / gov-change.md）没有这两句义务**，见 §2.0 复证 3。
- 验证集三类撰写病在近三批各有实证（261019 docs-root-ledger-rehome 两条 ADVISORY、261025 release-pr 两轮 BLOCK 同根、261021 三处预期失配 + 261024 R8 同根第二实证），而 `PROPOSAL.template` §5 只有一句「命令 + 预期」。
- `release-pr.mjs` 的 PR 标题/正文只看 merge-driver 合并行：gov 批（制品在 `delivery/review/`）恒落「0 个 Pod 合并」+「未从 merge-driver 合并行提取到 changeId」（PR #40 实测，human 点名）。

**非目标（本批不做，防 scope creep）**：

- **不新建 landing-summary 模板**（human 2026-10-26 裁：加两句字面义务，视复发再立；且模板已存在，新建即重复）。
- **不做 orphan-T 启发式点名**（「§2 截断后 §3 及以后出现 T 标题就报错」）——会误伤「§3 按 T 分小节」的合法写法；改为更精确的「围栏未闭合」fail-loud，理由与退路见 §4 取舍 A。
- **不做 release-pr 完整版**（提取 PROPOSAL §0 摘要进 PR 正文）——human 裁最小版。
- **不动 `ARQ-260924` 残余②**（目录锚展开，独立批）。
- **不改任何门判据的 RED 语义**：不新增门、不改退出码契约、不改 `verify-gates.json` / settings / hooks；`pre-merge-escalation-gate.mjs` 零触碰。
- **不追改历史制品**：已落地的旧 PROPOSAL / landing-summary / verdict 不回改（append-only）。

## 2. 问题、根因与改法（T1…T7）

### 2.0 七条 ARQ 复证结论（起草期 2026-10-26；ARQ 行是待验证假设）

> 纪律：每条结论都对应实跑命令或亲读文件；ARQ 原文的路径 / 行号 / 机制描述按「搬运引据视同新断言复证」逐条核过，漂移处如实更正并在 §6 提示主线程结项时订正条目。

**复证 1 · `ARQ-261019-anchor-probe-fence-blind` —— 病灶成立，行号准确；且同根切片点比 ARQ 记录的多两处（本批一并收口）。**

- 源码亲核：`parseSection2AnchorLists`（`governance-gate-shared.mjs:363`）的边界探测在 `:371-374`（以 `/^##\s/` 找下一节标题），`fenceLineFlags` 在 `:375` **之后**才计算；锚行扫描段（`:378-392`）跳围栏——同函数内两处行为不一致，与 ARQ 描述逐字相符。
- 复现（现行实现，实跑）：§2 内围栏含一行 `## OpenClaw ops 段标题（围栏内原文钉死）`、其后 T2 带锚行 → `parseSection2AnchorLists` 返回 `{"union":["docs/a.md"],"problems":[]}`——T2 条目不解析、零 problems（静默）。
- **新增同根点（ARQ paths 未列）**：`landing-fidelity-check.mjs` 有两处等价的 fence-blind §2 切片——`section2Body`（`:214-224`，边界 `:219-221`、flags `:222`）与 `checkVerbatimQuotes`（`:452-480`，切片 `:455-459`）。实测：前者令 preflight 的加粗锚探针对围栏后的 `**目标文件**：` 行报 0 条（现行 `findings: []`）；后者把「围栏含 `## ` 行 + 其后一个钉死块」的 §2 读成 0 块（应为 2）。三处同根、同一修法（算 flags 在前、边界探测 `if (flags[i]) continue;`），本批一并改，理由见 §4 取舍 B。
- 全仓回归（修补版 vs 现行版，76 份 `PROPOSAL.md` / `PROPOSAL.template.md`，含本批稿自身）：union 差异 **0**、problems 差异 **0**、新增 problems **0**——存量文档零漂移。

**复证 2 · `ARQ-261019-verbatim-quote-line-level-false-negative` —— 根因（追加证据）成立；原条早期的「行级匹配」表述不准确，按追加证据更正。**

- 源码亲核：比对在 `landing-fidelity-check.mjs:474` `if (norm(content).includes(norm(block)))`——**本来就是整块子串 includes**，不是行级匹配；真正的病灶 = `block` 取到围栏正文时**含块尾换行**（`:460-463` 正则 `([\s\S]*?)` 截到闭合围栏前），目标句是长行中部子句时目标侧句后还有接续文，`句子\n` 必不在场。
- 真实数据复现（261025 批 PROPOSAL 的 §2 全部钉死块 × 两个目标文件，等价载体逐字复制现行切片与比对逻辑）：11 块 → 现行 missing **10**；`trim()` 变体 missing **8**；翻转的恰是 A5 的两句（`docs/DEV-GOVERNANCE.md:214` 的「human 把关点 = …仍 human**。」、`.claude/references/arch-review-protocol.md:58` 的「merge 恒由主线程…仍 human。」）；其余 8 条 missing 是「旧文已被替换、目标侧应消失」的应有形态，`trim()` 未多翻一条。
- 合成夹具判别（实跑）：目标行 `前缀甲：**。句后接续文、同一行内。` × 块 `前缀甲：**。`（含块尾换行）→ 现行 missing **1**、trim 变体 missing **0**。

**复证 3 · `ARQ-260928` + `ARQ-261021`（并条）—— 修法方向成立，但「无 summary 模板」的前提已过时；本批只落 face SSOT 的义务两句，不新建模板。**

- 亲核：`docs/templates/architect-landing-summary.template.md` **已存在**（文件头自注「260802-gov-review-track-b-hardening T2 落地」），内含 packet-id 行（`:12`）、五件组件义务（`:5`）、`landing-manifest` 围栏块（`:30-32`）与验证证据 / 诚实注记节——正是两条 ARQ 想要立的最小骨架。
- 指针已在两处：`arch-review-protocol.md:53`（七步第 4 步「骨架见 …，复制勿手写——缺 landing-manifest 围栏块即结构门 / landing-verifier 合法打回」）、`track-b-verifier-brief.md:22`（核验清单第 2 项同句）；两者同根 `ARQ-260924-landing-summary-template-pointer-gap`（**已结项归档**，2026-09-25）。
- 残余缺口因此收窄为：**face SSOT（`arch-change.md` / `gov-change.md`）没有这两句义务**——主线程走 `/arch-change` / `/gov-change` 时读的是这两个流程文件，两批 FAIL 的现场正是「先例文件当事实标准、义务未成文」。T3 据此只加义务两句 + 模板指针（human 裁「不立模板」与实况一致；ARQ-260928 候选①「新建模板」已不再适用）。
- ARQ-261021 的机判判据（跟踪两批 summary 格式 FAIL）保留为观察项，**口径改写**：模板已存在的前提下，再 FAIL 应查「指针可见性 / 是否被跳读」而不是「立模板」，见 §6 留根 5。

**复证 4 · `ARQ-261019-forward-derived-verification-set` —— 处方成立，落点取 `PROPOSAL.template` §5（ARQ 给的二选一之一）。**

- 证据两条 ADVISORY 在案（261019 docs-root-ledger-rehome 的 §5.7 白名单与 §5.3 计数）；本批不重跑那批数据，只把撰写规则成文。落点选模板而非 arch-change 的理由：规则作用于**验证集这件制品本身**，模板是每份 PROPOSAL 必抄的载体，单源不漂移。

**复证 5 · `ARQ-261025-verify-set-negative-positive-pairing` —— 处方成立；「是否升硬项」本批裁定 = 硬项（judge 可 challenge，退路见 §4 取舍 D）。**

- 证据（261025 批 R1-F1 / R2-F10 两轮同根）在案；本批把规则同时落 `PROPOSAL.template` §5（作者义务）与 `track-b-judge-brief.md` 透镜第 6 条（判官抽查）。

**复证 6 · `ARQ-261021-proposal-assert-expectations` —— 处方成立；human 2026-10-26 裁适用范围扩到「PROPOSAL 内所有实测数字」。**

- 原条 §5 三处失配 + 追加证据 261024 R8（数字由原型代跑、与成文规则现算不一致）。T6 的义务句按扩展口径写（§2 T6），落三处：arch-change.md 义务节、模板 §5 占位、判官透镜抽查。

**复证 7 · `ARQ-261026-release-pr-gov-lane-zero-pods`（搭载）—— 证据成立；gov lane 无 merge-driver 合并行是结构性的，不是偶发。**

- 亲核 main 史：gov 批落 main 的形态是普通 commit（如 `009eb70f` feat(gov) 落地 + `47888d5b` retro/记账），不是 `Merge <id> (merge-driver)` 行——`extractChangeIds` 对 gov lane 恒空；`buildPrBody` 的兜底字面（`release-pr.mjs:168-182`）与标题模板（`:257-260`）即 PR #40「0 个 Pod 合并」的来源。
- 检测设计（最小版，human 裁）：不解析 commit 主题、不做摘要提取，改从 `git diff --name-only origin/main..main -- delivery/review/` 派生治理批 changeId（`<YYMMDD>-(gov|arch)-review-<slug>` 目录前缀）——制品路径是本仓治理批的稳定锚，`delivery/review/INDEX.md` 等非批目录自动排除。两线皆空时诚实报「变更线未识别」，不再落「0 个 Pod」。

### T1 · §2 边界探测跳围栏（共享解析器 + 两处同根切片点 + 围栏未闭合点名）

**问题**：PROPOSAL §2 用围栏承载「原文钉死」内容（新节插入、长段替换是常见形态）且内容含 markdown 节标题时，锚解析把围栏内的 `## ` 行当节边界、§2 提前截断：其后 T 条目与锚行静默消失（零 problems），起草者要么付一轮形态规避返工（261019 批自曝过一次），要么在 manifest 对账期才被「仅 manifest 有」间接点出。同根缺陷在 landing-fidelity-check 的两处 §2 切片上复现：preflight 的加粗锚探针漏报、verbatim-quote 把钉死块数读少。

**证据**：见 §2.0 复证 1（三处源码行号、现行实现 `problems: []` 复现、preflight findings `[]`、verbatim 0 vs 2 块、全仓 76 份零漂移预跑）。

**根因**：边界探测与锚行扫描在同一函数内用了两套围栏语义——扫描段先算 `fenceLineFlags` 再跳，边界段在 flags 计算之前按裸正则找 `## `。`landing-fidelity-check.mjs` 的两处切片各写了一套同形循环（`section2Body` 先边界后 flags；`checkVerbatimQuotes` 用字符串 `search` 完全不认围栏），三处缺陷同源：**围栏语义没有作为单一半成品被所有 §2 切片点复用**。

**改法**（三处，均为函数内局部改；flags 先算、边界探测跳围栏）：

1. **`parseSection2AnchorLists`（`governance-gate-shared.mjs:371-375`）**：原文（逐字节，实跑逐串核对 = 恰 1 次；多行串用脚本 `split(anchor).length - 1` 计数，见文末自检留痕第 3 条）：

```js
  let end = lines.length;
  for (let i = start + 1; i < lines.length; i++) {
    if (/^##\s/.test(lines[i])) { end = i; break; }
  }
  const flags = fenceLineFlags(lines); // 全文件算一次，再按节域切片（下标与 lines 对齐）
```

   替换为（逐字节）：

```js
  const flags = fenceLineFlags(lines); // 全文件算一次，再按节域切片（下标与 lines 对齐）
  // 261026 批 T1（ARQ-261019-anchor-probe-fence-blind）：边界探测与锚行扫描同机制——跳围栏。
  // 围栏内以「## 」起头的行是原文钉死内容、不是节边界；旧行为在此提前截断 §2，其后 T 条目与锚行静默消失。
  let end = lines.length;
  let boundaryFound = false;
  for (let i = start + 1; i < lines.length; i++) {
    if (flags[i]) continue;
    if (/^##\s/.test(lines[i])) { end = i; boundaryFound = true; break; }
  }
  // 261026 批 T1：围栏未闭合（fence 定界行总数为奇数）且找不到非围栏节边界 → §2 扫描域吞至文末，其后条目不可见——fail-loud 点名。
  if (!boundaryFound && lines.reduce((n, l) => n + (/^\s*```/.test(l) ? 1 : 0), 0) % 2 === 1) {
    problems.push('围栏未闭合：fence 定界行为奇数且找不到非围栏节边界，§2 扫描域吞至文末、其后 T 条目与锚行不可见（fail-loud）');
  }
```

2. **同函数「无 T 条目」早退不吞既有 problems（`governance-gate-shared.mjs:393-395`）**：原文（`grep -cF` = 1）：

```js
    return { union: [], problems: ['§2 无任何 T 条目（### T<n> 标题缺失）：无法对账锚清单（fail-loud）'] };
```

   替换为：

```js
    return { union: [], problems: problems.concat(['§2 无任何 T 条目（### T<n> 标题缺失）：无法对账锚清单（fail-loud）']) };
```

   （否则「围栏未闭合 → 无任何条目」的现场只报后者、吞掉围栏点名，fail-loud 失效。）

3. **`section2Body`（`landing-fidelity-check.mjs:217-222`）**。原文（逐字节，多行串用脚本计数 = 1）：

```js
  let end = lines.length;
  for (let i = start + 1; i < lines.length; i++) {
    if (/^##\s/.test(lines[i])) { end = i; break; }
  }
  const flags = fenceFlags(lines);
```

   替换为（逐字节）：

```js
  const flags = fenceFlags(lines);
  let end = lines.length;
  for (let i = start + 1; i < lines.length; i++) {
    if (flags[i]) continue; // 261026 批 T1：边界探测跳围栏（与 parseSection2AnchorLists 同机制）
    if (/^##\s/.test(lines[i])) { end = i; break; }
  }
```

4. **`checkVerbatimQuotes`（`landing-fidelity-check.mjs:455-459`）**。原文（逐字节，多行串用脚本计数 = 1）：

```js
  const secStart = proposalText.search(/^##\s*2[.、]/m);
  if (secStart === -1) return { checked, missing };
  const secRest = proposalText.slice(secStart);
  const secEnd = secRest.search(/\n##\s/m);
  const secText = secEnd === -1 ? secRest : secRest.slice(0, secEnd);
```

   替换为（逐字节；围栏正则与比对逻辑不动——trim 属 T2）：

```js
  // 261026 批 T1：§2 切片跳围栏（围栏内以「## 」起头的行不是节边界；旧行为会提前截断、其后钉死块不被检）
  const vqLines = proposalText.split(/\r?\n/);
  const vqStart = vqLines.findIndex((l) => /^##\s*2[.、]/.test(l));
  if (vqStart === -1) return { checked, missing };
  const vqFlags = fenceFlags(vqLines);
  let vqEnd = vqLines.length;
  for (let i = vqStart + 1; i < vqLines.length; i++) {
    if (vqFlags[i]) continue;
    if (/^##\s/.test(vqLines[i])) { vqEnd = i; break; }
  }
  const secText = vqLines.slice(vqStart, vqEnd).join('\n');
```

5. **自检夹具（判别力已实测）**：三处自检段各增夹具，**逐字节如下**（插入位置：`runSection2AnchorSelfCheck` 的 `console.log(JSON.stringify({ selfCheck: 'section2Anchors' …}))` 汇总行之前；`runPreflightSelfCheck` 的 `console.log(JSON.stringify({ selfCheck: 'landing-preflight' …}))` 汇总行之前）。**反转实现（边界不跳围栏）时这些断言逐条变红**——现行值实测：union 缺 `docs/b.md`、围栏未闭合无 problem、加粗行 0 条。

```js
  // 261026 批 T1（ARQ-261019-anchor-probe-fence-blind）夹具：判别力 = 反转实现（边界探测不跳围栏）时
  // T2 锚行被截断出域，union 缺 docs/b.md、本组断言变红。
  const fenceBoundary = [
    '## 2. 改法（T1–T2）',
    '### T1 · 甲',
    '正文一句。',
    '目标文件：`docs/a.md`（改）',
    '',
    '```md',
    '## 围栏内原文钉死的节标题（不得作 §2 边界）',
    '```',
    '',
    '### T2 · 乙',
    '目标文件：`docs/b.md`（改）',
    '',
    '## 3. 影响面',
  ].join('\n');
  const fenceBoundaryRes = parseSection2AnchorLists(fenceBoundary);
  ok(fenceBoundaryRes.problems.length === 0, 'fence-boundary-no-problems');
  ok(JSON.stringify([...fenceBoundaryRes.union].sort()) === JSON.stringify(['docs/a.md', 'docs/b.md']), 'fence-boundary-union-keeps-post-fence-entry');
  // 反向正控：真节标题（非围栏）仍终止 §2——§3 内的 T9 标题不造条目、其锚行不入并集（防「跳围栏」被扩成「不认边界」）
  const genuineBoundary = [
    '## 2. 改法（T1）',
    '### T1 · 甲',
    '目标文件：`docs/a.md`（改）',
    '## 3. 影响面',
    '### T9 · §3 内的小节标题',
    '目标文件：`docs/z.md`（改）',
  ].join('\n');
  const genuineBoundaryRes = parseSection2AnchorLists(genuineBoundary);
  ok(genuineBoundaryRes.problems.length === 0 && JSON.stringify(genuineBoundaryRes.union) === JSON.stringify(['docs/a.md']), 'genuine-boundary-still-terminates');
  // 围栏未闭合 fail-loud 点名（判别力：反转实现无此 problem）
  const unclosedFence = [
    '## 2. 改法（T1）',
    '### T1 · 甲',
    '目标文件：`docs/a.md`（改）',
    '```md',
    '正文……（围栏未闭合）',
  ].join('\n');
  const unclosedRes = parseSection2AnchorLists(unclosedFence);
  ok(unclosedRes.problems.some((x) => x.includes('围栏未闭合')), 'unclosed-fence-fail-loud');
```

```js
  // 261026 批 T1：边界探测跳围栏——围栏内「## 」行不得截断 §2 域，其后加粗锚行仍须报出（绝对行号 9）。
  // 判别力：反转实现（section2Body 边界不跳围栏）时该行被截断出域、boldCls = 0 变红。
  const fenceBoundaryBold = ['# 夹具', '## 2. 改法（T1）', '### T1 · 甲', '目标文件：`.claude/scripts/a.mjs`（改）', '```md', '## 围栏内节标题', '```', '### T2 · 乙', '**目标文件**：`docs/b.md`（改）', '## 3. 影响面'].join('\n');
  const fbb = (preflightFindings(fenceBoundaryBold).findings.find((f) => f.class === 'bold-anchor-line') || {}).lines || [];
  check('跳围栏·围栏后加粗锚行仍报（绝对行号 9）', fbb[0], 9);
```

**为什么这样改**：对根因「三处各自一套围栏语义」——统一为「先算 flags、所有 §2 切片都跳围栏、只认非围栏的 `## ` 作边界」；截断诱因消失后，剩余唯一会吞条目的形态是围栏未闭合，用一条精确的 fail-loud 补上（不引入启发式误报）。

目标文件：`.claude/scripts/lib/governance-gate-shared.mjs`（改） · `.claude/scripts/landing-fidelity-check.mjs`（改）

### T2 · verbatim-quote 子句假阴性（比对前对块 trim）

**问题**：`checkVerbatimQuotes` 对「替换句嵌在长行中部、句后仍有接续文」的形态恒报 `not-found-verbatim-in-any-target`（即使逐字在场）——核验者每次都要手工 byte 复核，观察列可信度被稀释（狼来了效应）；两次实证（260925 批 A5 两条、260928 批一条）。

**证据**：见 §2.0 复证 2（真实数据 11 块：现行 10 missing / trim 8 / 翻转恰 A5 两句；合成夹具现行 1 → trim 0）。

**根因**：围栏块正文恒带块尾换行；`includes(block)` 要求目标侧句后紧跟换行。目标文件里句后接续文 ⇒ 恒假阴性。方向不是改匹配粒度（本来就是子串），而是先剥掉块的首尾空白。

**改法**（`landing-fidelity-check.mjs:474`）：原文（`grep -cF` = 1）：

```js
      if (norm(content).includes(norm(block))) { found = true; break; }
```

替换为：

```js
      // 261026 批 T2（ARQ-261019-verbatim-quote-line-level-false-negative）：围栏块正文恒带块尾换行，
      // 子句替换形态里目标句常嵌在长行中部、句后仍有接续文——比对前对块 trim()（首尾空白不进判据，正文逐字仍在）。
      if (norm(content).includes(norm(block).trim())) { found = true; break; }
```

自检增 4 条，**逐字节如下**（插入位置：`runVerbatimQuotesSelfCheck` 内**紧接** `check('verbatim-crlf-checked', crlfRes.checked.length, 1);` 行之后——注意该文件里 `rmSync(fixtureDir, …)` 共出现两处（citation 自检段与 verbatim 自检段各一），**不得**以它为锚，否则夹具会落进 `runCitationAnchorsSelfCheck` 而 `block` 未定义、整个 `--self-check` 崩在运行期；`block` / `fixtureDir` 为 verbatim 自检段的既有作用域变量）：

```js
  // 261026 批 T2 夹具：子句嵌长行中部 + 块尾换行（判别力：反转实现不 trim 时 missing = 1 变红）
  const clauseDir = mkdtempSync(join(tmpdir(), 'vq-clause-'));
  writeFileSync(join(clauseDir, 'long.md'), '前缀甲：**。句后接续文、同一行内。\n', 'utf8');
  const clauseProposal = ['## 2. 改法', '### T1 · 子句', '```markdown', '前缀甲：**。', '```', '## 3. 影响面'].join('\n');
  const clauseRes = checkVerbatimQuotes(clauseProposal, clauseDir, ['long.md']);
  check('verbatim-clause-midline-trailing-newline-missing', clauseRes.missing.length, 0);
  check('verbatim-clause-checked', clauseRes.checked.length, 1);
  // 尾换行变体正控：块内容确实不在目标 → 仍报 missing（trim 不放宽判据）
  const clauseDrift = clauseProposal.split('前缀甲：**。').join('前缀甲：**。改');
  check('verbatim-clause-drift-still-missing', checkVerbatimQuotes(clauseDrift, clauseDir, ['long.md']).missing.length, 1);
  // 261026 批 T1 夹具：§2 切片跳围栏——围栏内含「## 」行后，其后钉死块仍须被检（反转实现截断 → checked 1 变红）
  const boundaryProposal = ['## 2. 改法', '### T1 · 甲', '```md', '## 围栏内节标题', '```', '', '### T2 · 乙', '```js', block, '```', '## 3. 影响面'].join('\n');
  check('verbatim-fence-boundary-checked', checkVerbatimQuotes(boundaryProposal, fixtureDir, ['target.mjs']).checked.length, 2);
  rmSync(clauseDir, { recursive: true, force: true });
```

方向 = **观察列向准**，不改任何 RED/exit 语义（该检查本就 advisory-only）。

**为什么这样改**：对根因「块尾换行被当成正文一部分」——trim 是外科最小动作；逐行 trim 会额外忽略缩进差异，对「原文钉死」语义过宽，不取。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改）

### T3 · landing-summary 结构两件义务进 face SSOT（arch-change.md 新增义务节 + gov-change.md 指针）

**问题**：landing-summary 的两件结构义务（字面 `packet-id` + 被批 review id；`landing-manifest` 围栏块）是 `landing-fidelity-check.mjs` 的硬失败判据（`:584-586` / `:591-597`），却没有任何 face SSOT 承载——主线程凭先例对照补齐，260928 / 261021 两批各 FAIL 一轮。

**证据**：见 §2.0 复证 3（模板存在与内容行号、两处指针行号、指针缺口 ARQ 已结；两批 FAIL 实录在各自 verdict）。

**根因**：义务只活在门脚本与先例文件里；`/arch-change`、`/gov-change` 的流程 SSOT 不提这两件，主线程写 summary 时没有可见的义务句。模板与指针都在，但都在「七步后端」层，face SSOT 层是空的。

**改法**：

1. `arch-change.md` 在「## 轻路径落地通道（R1-F3 / R2-F1）」节之后、「## 触契约分支」之前插入新节。**新节标题行（逐字，行首）**：`## Track B 制品义务（landing-summary 结构两件 + 验证集实跑回填）`。节体（围栏内按字节落地）：

```markdown
> 本节 = 两面 lane（`/arch-change` 与 `/gov-change`）Track B 落地的**制品义务**单源；落地恒由主线程 packet-bound 执行。轻/主路径同义务——轻批的瘦 summary 不豁免本节两件。

**① landing-summary 结构两件**（`landing-fidelity-check.mjs` 硬失败条目 3 / 7，缺一即核验 FAIL）：

- 引字面 `packet-id` token，且同文引被批 review id（`gov-review-<id>` / `arch-review-<id>`；A9 packet-binding）。
- 含 `landing-manifest` 围栏块：一行一个 canonical 目标路径，恰 = FROZEN PROPOSAL §2 各 T 目标文件锚清单并集（条件锚按裁断裁剪）。

骨架 = `docs/templates/architect-landing-summary.template.md`（复制勿手写；七步第 4 步与本文件同指针）。两件原只活在门脚本与先例文件里——260928 / 261021 两批 summary 首跑各 FAIL 一轮（ARQ-260928 / ARQ-261021），成文即消。

**② 验证集预期值实跑回填（ARQ-261021-proposal-assert-expectations）**：human 2026-10-26 裁适用范围扩至本 PROPOSAL 内所有实测数字。

§5 每条断言的预期值 = 起草期**实跑输出**回填（命令 + 关键输出 verbatim 留痕），禁止推算；适用范围 = 本 PROPOSAL 内所有实测数字（§5 断言、脚本与表格数字、census 计数一律现算，不得由另一套原型代跑）。落地期实跑若与预期有字节差异，以落地实跑为准覆写 §5 并在 PROPOSAL §7 留痕。
```

2. `gov-change.md` 的「## Track B 后端」节末句（`:30`，逐字）`= 现行 Track B 七步（AGENTS.md「治理路由」节），cast 已统一：主线程派 architect 起草 PROPOSAL → human 立项 ack → architect-reviewer 评审循环至 GREEN → 主线程标 FROZEN → packet-bound landing → landing-verifier → retro → 升级闸 → 合并本地 main。` **句尾追加**（同一行接续）：

```markdown
 落地制品义务（landing-summary 结构两件 + 验证集实跑回填）见 `arch-change.md`「Track B 制品义务」节（单源，本文件不复制）。
```

**为什么这样改**：对根因「义务不在 face SSOT 层」——两句字面义务 + 既有模板指针落进两个 face 流程文件（gov-change 只放指针防双源）；不新建模板（已存在）、不复制五件组件清单（模板是唯一细节源）。

目标文件：`.claude/references/arch-change.md`（改） · `.claude/references/gov-change.md`（改）

### T4 · 验证集白名单 / 计数从处方正推

**问题**：「命中 ≡ 白名单」「残留 = 0」「计数 ≤ N」型验证条目按**现状命中**枚举，而处方新文本身会带来新命中（新句逐字回引旧路径等）——落地与处方一致即必然「白名单外命中」；计数承诺与数据来源表两处独立撰写、不交叉对账（261019 docs-root-ledger-rehome 的 §5.7 / §5.3 两条 ADVISORY 实录）。

**证据**：ARQ-261019-forward-derived-verification-set 证据段（两条 ADVISORY 同族：处方新文 9 行自带旧路径字面；advisory 承诺 ≤195、实测 196）。本批不重跑那批数据，只成文规则。

**根因**：验证集从现状枚举、不从处方正推；计数与映射表两个数据源没对账。

**改法**（`PROPOSAL.template.md:84-86` 之后追加，与 T5 / T6 的模板插入**合一为同一块**「验证集撰写三规则」；本 T 对应其第 2 条）：

```markdown
**【硬项 · 验证集撰写三规则】**（261026 批合写；ARQ-261021 / ARQ-261019 / ARQ-261025 各一）

1. **预期值现跑回填（ARQ-261021-proposal-assert-expectations）**：每条断言的预期值 = 起草期实跑输出回填（命令 + 输出摘要留痕于本节「基线」表或文末自检留痕段），禁止推算；适用范围 = 本 PROPOSAL 内所有实测数字（§5 断言、脚本与表格数字、census 计数一律现算，不得由另一套原型代跑）。占位写法：`预期：<起草期实跑输出摘要>（命令见行内；无实跑留痕不得标 FROZEN）`。
2. **白名单 / 计数从处方正推（ARQ-261019-forward-derived-verification-set）**：凡「命中 ≡ 白名单」「残留 = 0」「计数 ≤ N」型条目，白名单须从处方新文正推——处方文本本身自带的字面（新句里回引的旧路径等）也计入命中集合，先列进白名单再写断言；计数承诺须与数据来源表（映射表 / 配置行 / 计数表）交叉对账后落数字。
3. **负向断言须配正向 happy-path（ARQ-261025-verify-set-negative-positive-pairing）**：负向断言（不含 / 只允许 / ⊆ / = 0）必须配一条正向断言（恰一次 / 相邻 flag 对 / 集合相等），否则判官可按真空绿打回。
```

本批自身即按第 2 条执行：§5 的白名单型断言（新增字面句 grep、自检分组计数）逐条从 §2 处方新文正推列出，见 §5「白名单正推清单」。

**为什么这样改**：对根因「从现状枚举」——规则钉在验证集唯一载体（模板 §5），起草期即须正推；不新增机器检查（口径是人判 + 判官抽查）。

目标文件：`docs/templates/PROPOSAL.template.md`（改）

### T5 · 负向断言须配正向 happy-path（模板硬规则 + 判官透镜第 6 条）

**问题**：负向白名单 / 零字符串 / ⊆ 允许集型断言「看起来最机械，实际是真空绿的反面形态」——可被满足，但满足方式无意义（261025 批两轮 BLOCK 同根：零字符串与自检断言互斥、gh 调用集合 ⊆ 白名单而一次 PR 都不开也满足）。

**证据**：ARQ-261025 证据段（R1-F1 BLOCKING、R2-F10 同根二次发现；修法 `publish-happy-path` 夹具 + argv 白名单在 v0.4 落地）。

**根因**：验证集撰写只规约「每条机械可判」，没规约「负向断言必须同时证明正向路径真的发生过」。

**改法**（同一「验证集撰写三规则」块的第 3 条，见 T4 改法围栏内的第 3 项；另改判官简报透镜）：

`track-b-judge-brief.md:32` 原文（逐字节，`grep -cF` = 1）：

```markdown
6. **验证集可执行性**：每条机械可判；用真空绿规约反照（锚点 grep 是否「看起来像验证、实则证不了什么」）。
```

替换为（逐字节）：

```markdown
6. **验证集可执行性**：每条机械可判；用真空绿规约反照（锚点 grep 是否「看起来像验证、实则证不了什么」）。**配对与实跑抽查（261026 批，ARQ-261025 / ARQ-261021）**：负向断言（不含 / 只允许 / ⊆ / = 0）须配正向 happy-path 断言，缺配可按真空绿打回；抽查每条预期值旁有无起草期实跑留痕（命令 + 输出摘要），无留痕的推算值可打回。
```

**为什么这样改**：对根因「只规约可判、不规约证到什么」——配对是能被判官机械判的（有没有对应正向行），落作者侧（模板）与判者侧（透镜）两处、文字同一句意避免双源分歧。

目标文件：`docs/templates/PROPOSAL.template.md`（改） · `.claude/references/track-b-judge-brief.md`（改）

### T6 · 验证集预期值实跑回填（适用范围 = PROPOSAL 内所有实测数字）

**问题**：§5 预期值由起草人推算：口径错用（`grep -c` 行计数当次数）、自引未计入（新增 T 条目自身回引目标词）、原型代跑与成文规则不一致（261021 三处失配 + 261024 R8 第二次实证）；近失手会把正确落地判成夹带。

**证据**：ARQ-261021-proposal-assert-expectations 事实段 + 追加证据（261024 R8：成文规则写「JSON/数据与配置 → 文档」，原型数字按 `.txt → 代码` 生成，claimed 中位 1.07 不复现、按成文规则现算 1.29）；human 2026-10-26 裁适用范围扩至「PROPOSAL 内所有实测数字」。

**根因**：预期值被当作可推算的纸面数字；没有把「实跑留痕」规定为预期值的合法来源。

**改法**（三处，同一条义务）：

1. `arch-change.md` 新节「Track B 制品义务」的 **②** 项（见 T3 改法围栏内的 ② 段，逐字含「实跑输出回填」「所有实测数字」「不得由另一套原型代跑」）。
2. `PROPOSAL.template.md` §5 的「验证集撰写三规则」第 1 条（见 T4 改法围栏内第 1 项，含占位写法 `预期：<起草期实跑输出摘要>`）。
3. `track-b-judge-brief.md` 透镜第 6 条的「实跑留痕」抽查句（见 T5 替换文本「抽查每条预期值旁有无起草期实跑留痕（命令 + 输出摘要），无留痕的推算值可打回」）。

**为什么这样改**：对根因「推算被当合法来源」——把实跑留痕写成预期值的唯一合法来源，并在作者 / 起草流程 / 判官三面各留一句；三处文字同源（T3 的 ② 段为定义处，另两处为义务投影，不复制举例细节）。

目标文件：`.claude/references/arch-change.md`（改） · `docs/templates/PROPOSAL.template.md`（改） · `.claude/references/track-b-judge-brief.md`（改）

### T7 · release-pr 的 PR 门面按 lane 出标题 / 正文（最小版）

**问题**：gov 治理批发布时 PR 标题落「0 个 Pod 合并」、正文落「未从 merge-driver 合并行提取到 changeId」——对评审者零信息量，每次都要主线程事后 `gh pr edit` 手写（PR #40 实测 + human 点名）。

**证据**：见 §2.0 复证 7（main 史形态 + 源码行号 + PR #40 实录）。

**根因**：发布脚本的「本批改了什么」提取只有 dev 一条腿（merge-driver 合并行）；gov 批的制品锚（`delivery/review/<changeId>/`）没被消费，两条腿都空时脚本还落了一句**看起来像说明、实际零信息**的兜底。

**改法**（函数级，四个落点 + 自检）：

1. 新增纯函数 `extractGovChangeIds`（插在 `extractChangeIds` 之后、`assertPushArgv` 注释块之前），**逐字节**：

```js
/**
 * gov lane changeId 提取（261026 批 T7，ARQ-261026-release-pr-gov-lane-zero-pods）：从
 * `git diff --name-only origin/main..main -- delivery/review/` 输出取治理批目录前缀
 * （<YYMMDD>-(gov|arch)-review-<slug>），按序去重。gov 批无 merge-driver 合并行，changeId 只能从交付制品路径派生。
 */
export function extractGovChangeIds(diffNamesText) {
  const out = [];
  const seen = new Set();
  for (const line of String(diffNamesText || '').split(/\r?\n/)) {
    const m = /^delivery\/review\/(\d{6}-(?:gov|arch)-review-[A-Za-z0-9._-]+)\//.exec(line.trim());
    if (m && !seen.has(m[1])) { seen.add(m[1]); out.push(m[1]); }
  }
  return out;
}
```

2. 新增纯函数 `derivePrTitle`（紧接上者之后），**逐字节**：

```js
/**
 * PR 标题（261026 批 T7）：dev lane = merge-driver 合并行（>0）→「N 个 Pod 合并」；
 * gov lane = delivery/review 治理批目录（dev 行为 0）→ 点名治理批；两线皆空 → 诚实报「变更线未识别」，
 * 不再落「0 个 Pod 合并」空门面（PR #40 实测病灶：gov 批发布时标题对评审者零信息量）。
 */
export function derivePrTitle({ ymd, suffix, changeIds, govChangeIds }) {
  if (changeIds.length) return `release(${ymd}${suffix}): ${changeIds.length} 个 Pod 合并`;
  if (govChangeIds.length) return `release(${ymd}${suffix}): gov 治理批 ${govChangeIds.join(', ')}`;
  return `release(${ymd}${suffix}): 变更线未识别（dev 合并行与 gov 交付制品均空）`;
}
```

3. `buildPrBody`（`:168-182`）整函数替换。原文（逐字节；多行串用脚本计数 = 1）：

```js
export function buildPrBody({ changeIds, baseSha, tipSha, branch }) {
  const pods = changeIds.length
    ? changeIds.map((id) => `- ${id}`)
    : ['- （未从 merge-driver 合并行提取到 changeId）'];
  return [
    `发布分支：\`${branch}\``,
    `基线（origin/main）：\`${baseSha}\``,
    `release tip（= 本地 main）：\`${tipSha}\``,
    '',
    '本批已合并 Pod：',
    ...pods,
    '',
    '仓库仅允许 merge commit。human 在网页合并后，请在主 worktree 跑 `node .claude/scripts/release-pr.mjs --sync` 追平本地 main。',
  ].join('\n');
}
```

   替换为（逐字节；发布分支 / 基线 / tip / 末尾提醒四行逐字保持不变）：

```js
export function buildPrBody({ changeIds, govChangeIds = [], baseSha, tipSha, branch }) {
  const section = changeIds.length
    ? ['本批已合并 Pod：', ...changeIds.map((id) => `- ${id}`)]
    : govChangeIds.length
      ? ['本发布为 gov 治理批：', ...govChangeIds.map((id) => `- gov 治理批 ${id}（制品见 delivery/review/${id}/）`)]
      : ['本变更线未识别（dev 的 merge-driver 合并行与 gov 交付制品均为空）：请在合并前于 PR 描述补充本批变更说明。'];
  return [
    `发布分支：\`${branch}\``,
    `基线（origin/main）：\`${baseSha}\``,
    `release tip（= 本地 main）：\`${tipSha}\``,
    '',
    ...section,
    '',
    '仓库仅允许 merge commit。human 在网页合并后，请在主 worktree 跑 `node .claude/scripts/release-pr.mjs --sync` 追平本地 main。',
  ].join('\n');
}
```

4. `publishFlow` 第 8 段（`:257-260`）与 body 调用行（`:264`）。原文（逐字节，各 1 次）：

```js
  const changeIds = extractChangeIds(deps.mergeLog());
  const prTitle = title || `release(${deps.today()}${suffix}): ${changeIds.length} 个 Pod 合并`;
```

   替换为：

```js
  const changeIds = extractChangeIds(deps.mergeLog());
  const govChangeIds = extractGovChangeIds(deps.reviewDiffNames ? deps.reviewDiffNames() : '');
  const prTitle = title || derivePrTitle({ ymd: deps.today(), suffix, changeIds, govChangeIds });
```

   body 调用行原文 `    body: buildPrBody({ changeIds, baseSha: originSha, tipSha: mainSha, branch: picked.name }),` 替换为 `    body: buildPrBody({ changeIds, govChangeIds, baseSha: originSha, tipSha: mainSha, branch: picked.name }),`；第 8 段注释原文 `  // 8. 开 PR（标题默认 release(<YYMMDD>[-n]): <N> 个 Pod 合并）。` 替换为 `  // 8. 开 PR（261026 批 T7：lane 感知标题/正文——dev lane 记 Pod，gov lane 点名治理批与制品目录）。`

5. deps 两处（各 1 次）：live deps 在 `mergeLog` 行之后追加 `    reviewDiffNames: () => git(['diff', '--name-only', 'origin/main..main', '--', 'delivery/review/'], { cwd }).stdout || '',`；`makeFakePublishDeps` 的 `mergeLog` 行之后追加 `    reviewDiffNames: () => '',`。

6. 自检新增 7 条夹具（`runSelfCheck`，插在 `const pass = results.filter((r) => r.pass).length;` 之前），**逐字节**：

```js
  // 20（261026 批 T7）：gov lane changeId 提取
  {
    const ids = extractGovChangeIds([
      'delivery/review/260101-gov-review-x/PROPOSAL.md',
      'delivery/review/260101-gov-review-x/retro.md',
      'delivery/review/260102-arch-review-y/PROPOSAL.md',
      'delivery/review/INDEX.md',
      'delivery/review/.changes-index-state.json',
      'delivery/intake/z.md',
    ].join('\n'));
    check('extract-gov-changeids', JSON.stringify(ids) === JSON.stringify(['260101-gov-review-x', '260102-arch-review-y']), `ids=${JSON.stringify(ids)}`);
  }
  // 21（261026 批 T7）：gov lane 横幅——标题点名治理批、正文给制品指针（正向），且不落「0 个 Pod」空门面（负向配对）
  {
    let captured = null;
    const { deps } = makeFakePublishDeps({
      mergeLog: () => '',
      reviewDiffNames: () => 'delivery/review/260101-gov-review-x/PROPOSAL.md\n',
      ghCreatePr: (payload) => { captured = payload; return { ok: true, url: 'https://example.test/pull/88' }; },
    });
    const r = publishFlow(deps);
    check('gov-lane-title', r.exit === 0 && !!captured && captured.title === 'release(261025): gov 治理批 260101-gov-review-x', `title=${captured && captured.title}`);
    check('gov-lane-body-pointer', !!captured && captured.body.includes('gov 治理批 260101-gov-review-x（制品见 delivery/review/260101-gov-review-x/）'), 'body missing pointer');
    check('gov-lane-no-zero-pod', !!captured && !captured.title.includes('个 Pod') && !captured.body.includes('个 Pod') && !captured.body.includes('未从 merge-driver 合并行提取到 changeId'), 'zero-pod facade still present');
  }
  // 22：dev lane 正向配对——合并行在场时标题仍记 Pod 数
  {
    let captured = null;
    const { deps } = makeFakePublishDeps({ ghCreatePr: (payload) => { captured = payload; return { ok: true, url: 'u' }; } });
    publishFlow(deps);
    check('dev-lane-title', !!captured && captured.title === 'release(261025): 2 个 Pod 合并', `title=${captured && captured.title}`);
  }
  // 23：两线皆空——诚实报「变更线未识别」
  {
    let captured = null;
    const { deps } = makeFakePublishDeps({ mergeLog: () => '', reviewDiffNames: () => 'delivery/review/INDEX.md\n', ghCreatePr: (payload) => { captured = payload; return { ok: true, url: 'u' }; } });
    publishFlow(deps);
    check('unknown-lane-title', !!captured && captured.title.includes('变更线未识别') && !captured.title.includes('个 Pod'), `title=${captured && captured.title}`);
    check('unknown-lane-body', !!captured && captured.body.includes('本变更线未识别') && !captured.body.includes('未从 merge-driver 合并行提取到 changeId'), 'body fallback mismatch');
  }
```

**为什么这样改**：对根因「只有 dev 一条提取腿」——gov 腿用交付制品路径（本仓最稳定锚），不做摘要提取（human 最小版）；两线皆空改成诚实标签，防止下一次以另一种形态重演「零信息门面」。

目标文件：`.claude/scripts/release-pr.mjs`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 / 消费方 |
|---|---|---|---|
| 强制执行层 G3 | `.claude/scripts/lib/governance-gate-shared.mjs` | 改（T1） | §2 边界探测跳围栏 + 围栏未闭合 fail-loud + 早退不吞 problems；消费方 = `landing-fidelity-check.mjs`（`parseSection2AnchorLists` 唯一消费者，实证见下） |
| 强制执行层 G3 | `.claude/scripts/landing-fidelity-check.mjs` | 改（T1 / T2） | 两处 §2 切片点跳围栏 + verbatim 比对 trim + 自检夹具；消费方 = 主线程 landing 结构门 + 起草期 `--preflight` |
| 强制执行层 G3 | `.claude/scripts/release-pr.mjs` | 改（T7） | 新增两个纯函数 + `buildPrBody` 入参扩展 + live deps 一个新字段 + 自检 7 夹具；消费方 = 主线程发布面（`--publish` / `--self-check`），无程序解析标题/正文 |
| 流程参考 G2 | `.claude/references/arch-change.md` | 改（T3 / T6） | 新增「Track B 制品义务」节（summary 两件 + 验证集实跑回填）；消费方 = `/arch-change` 主线程与起草者 |
| 流程参考 G2 | `.claude/references/gov-change.md` | 改（T3） | 「Track B 后端」节句尾追加一行指针（单源不复制）；消费方 = `/gov-change` 主线程 |
| 判定简报 G2 | `.claude/references/track-b-judge-brief.md` | 改（T5 / T6） | 透镜第 6 条加「配对与实跑抽查」句；消费方 = Track B 判官派遣 prompt |
| 制品模板 G2 | `docs/templates/PROPOSAL.template.md` | 改（T4 / T5 / T6） | §5 追加「验证集撰写三规则」硬项块；消费方 = 每份新 PROPOSAL（复制模板） |
| 指令根 G1 | `AGENTS.md` / `CLAUDE.md` / `docs/DEV-GOVERNANCE.md` | 未触 | 零 diff |
| 契约面 | `docs/contracts/**` | 未触 | 零 diff（§5 V9 断言） |
| 角色定义 / review-loop 定义 | `.claude/agents/**` | 未触 | 零改；无自改递归、无第五阀问题 |
| 其余强制层 | `.claude/hooks/**`、`.claude/scope-map.json`、`*.settings.json`、`.claude/verify-gates.json` | 未触 | 零改；无新增门 |
| 现有 landing-summary 模板 | `docs/templates/architect-landing-summary.template.md` | 未触（只被指针指向） | 已存在（260802 T2），本批不新建、不改内容 |

**【硬项 · sweep 透镜义务】新判据 / 新语义全仓预跑（起草期 2026-10-26，命令 + 结果）**：

- **修补后解析器 vs 现行解析器全仓回归**：扫描 `delivery/review`、`delivery/archive`、`docs/templates` 下全部 `PROPOSAL.md` / `PROPOSAL.template.md`（**76 份**，含本批稿自身），逐份比较 union 与 problems——`{"files":76,"unionDiff":0,"problemSetDiff":0,"newProblems":0}`。即：新边界语义对存量文档**零漂移**，「围栏未闭合」新问题**零存量命中**（不会把已落地批次翻红）。
- **`parseSection2AnchorLists` 消费者 census**：`.claude/scripts/*.mjs` 具名导入 `governance-gate-shared.mjs` 共 9 个脚本；其中 `parseSection2AnchorLists` 只被 `landing-fidelity-check.mjs` 消费（其余脚本用的是 isFrozen / extractTargets / TARGET_PATTERNS / resolveOrigin / resolveHumanApproval / VERDICT_* 等）——改解析器只影响 landing 门与 preflight。
- **`release-pr` 函数消费者 census**：`buildPrBody` / `extractChangeIds` 仅脚本内部调用（全仓 grep 无第二消费方）；`release-pr` 不在 `verify-gates.json` / settings 任何闸链内；PR 标题/正文无程序解析方（`gh pr list --json headRefName,headRefOid` 只取这两字段）。
- **新字面句碰撞基线**：§5 将断言的 13 条新增字面（见 §5「白名单正推清单」）在现行全仓（`.md` / `.mjs` / `.json`，排除本批目录）实测命中文件数**均为 0**——新句不与既有文本撞车。
- **`§2 外 T 标题` census（orphan-T 取舍的依据）**：`delivery/review`、`delivery/archive`、`docs/templates` 下 147 份 PROPOSAL 族 md（含模板与 landing-summary / verdict 类，含本批稿）逐份比较「围栏感知 §2 边界之外的 `### T<n>` 标题」与「被围栏吞掉的 `### T<n>`」——两列**均 0**。存量无触发实例，但该启发式对「§3 按 T 分小节」的未来合法写法有误报面，故不取（§4 取舍 A）。
- **围栏未闭合 census**：76 份 PROPOSAL §2 内 fence 定界行奇数的文件 **0 份**——新 fail-loud 无存量误伤。

**【硬项 · 复扫模式完备性先验】**：本批无退役语面、无重命名、无目录搬家。唯一「清扫型」面 = T7 的旧兜底字面（`（未从 merge-driver 合并行提取到 changeId）`）——它在 `release-pr.mjs` 中仅 `buildPrBody` 一处，本批以替换实现删去，已在 §5 V7 以「负向：源码与自检输出均不再出现该字面」+「正向：gov lane 指针行恰在场」配对断言。不涉路径搬家，故 `path-form-family.md` 形态族谱无消费面。

## 4. human 取舍点（OQ）

**本批无待裁 OQ**：human 2026-10-26 立项 ack 已把三个真分叉裁完——① summary 义务承载 = `arch-change.md` 加两句字面义务、不立模板；② 实跑回填纪律适用范围 = PROPOSAL 内所有实测数字；③ release-pr = 最小版。T3 / T6 / T7 按裁定无条件落地。

**非 OQ 但提请评审注意的技术取舍（已给理由，judge 可 challenge）**：

- **取舍 A · orphan-T 启发式不取，改「围栏未闭合」fail-loud（T1）**：ARQ 建议的「报错侧对条目消失增显式点名」若实现为「§2 之后出现 `### T<n>` 就报错」，会对「§3 按 T 分小节」的合法结构误报（存量 0 实例、但误报面真实存在）。改取更精确的条件：**找不到非围栏节边界 + fence 定界行数为奇数** → 报「围栏未闭合」。它恰好覆盖「边界修好后唯一还会吞条目的形态」（未闭合围栏会令其后条目全部不可见），且零存量命中（§3 census）。**退路**：判官若认为必须实现 orphan-T，可在 R 轮要求追加为 advisory 级（不阻断），本批不预留实现。
- **取舍 B · 同根三处一并修（T1 含 landing-fidelity 两处）**：ARQ paths 只列共享 lib 一处；复证发现 `landing-fidelity-check.mjs` 另有两处同根切片（preflight 加粗探针、verbatim §2 切片），且 T2 本就动该文件。只修 lib 会让起草期 preflight 与 verbatim 观察列对同一形态继续失明——正是本 ARQ 记录的病灶面。**退路**：判官若判范围溢出，可裁「lib 一处落地 + 另两处转留根」，改法可分离（三处各自独立、互不依赖），manifest 相应去 `.claude/scripts/landing-fidelity-check.mjs`（但 T2 仍需要该文件，故该退路实际只在「另两处不动」维度成立）。
- **取舍 C · T3 只加义务句、不加模板副本**：模板已存在（§2.0 复证 3）；face SSOT 加「两句义务 + 模板指针」，gov lane 只加一行指针防双源。**退路**：若判官认为 gov-change.md 一行指针溢出，删该行即可（T3 主体仍成立，gov lane 经 protocol 七步第 4 步的既有指针可达）。
- **取舍 D · T5 的规则标【硬项】（ARQ 留「是否升硬项由下一批裁」）**：本批裁定升硬项，理由 = 真空绿反照规律（负向断言无配对即可被打回，与「看起来像验证、实则证不了什么」同族）；模板内【硬项】标签在当前体制下等于「判官 BLOCK 的合法理由」，给的是判据可见性不是新门。**退路**：判官/human 若要求保持建议性，把第 3 条前的【硬项】框架取消、降为普通规则段即可，其余不动。
- **取舍 E · T7 两线皆空也改诚实标签（超出「gov lane」最小版半句）**：human 裁最小版针对 gov lane；两线皆空若保留旧兜底，下一次以「非 dev 非 gov」形态发布仍会重演零信息门面。改动只是同一函数内一行分支文案，负向断言与正向配对已给。**退路**：判官若判超范围，删该分支、保留 `release(<ymd>): 0 个 Pod 合并` 现行为，其余不动。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> **断言纪律（T6 自守）**：本节每条预期值 = 起草期实跑输出回填，**不留推算值**。来源两类：① 现行仓库实跑（基线 B 系列）；② **装配版实跑**——把 §2 的逐字节补丁文本机械抽取、装配到 `.claude/scripts/**` 的 pristine 副本（仓外临时目录，不进仓），逐条跑同一命令取输出（B10 / V1 / V2 直接引用）；落地期若实跑与本节有字节差异，以落地实跑为准覆写本节并在 §7 留痕。
> **时点定义**：pre-commit = 落地文件在盘、未 commit 即可跑；post-commit = 须落地提交后才成立。
> **原型的诚实注记**：B10 / V1 / V2 的最终数字**取自「以本 PROPOSAL §2 逐字节文本装配进 pristine 副本」的运行**（装配脚本从本稿 js 围栏块机械抽取补丁文本，不手抄）——不是另一个手工原型。早期探索版（scratch 副本上手工打补丁）先跑出同一组数字，用于确认方向；最终稿以装配版为准。`buildPrBody` 的发布分支 / 基线 / tip / 末尾提醒四行在装配版里逐字沿用原文，未改。

**基线（现行 HEAD，起草期实跑 2026-10-26，本 gov worktree）**：

| # | 命令 | 实跑输出（逐字 / 摘要） |
|---|---|---|
| B1 | `node .claude/scripts/landing-fidelity-check.mjs --self-check` | 10 组自检全 `ok:true`、exit 0；分组 pass：`changeid-prefix 4` · `extractTargets 6` · `section2Anchors 25` · `resolveOrigin 29` · `computeFrozenPin 4` · `citation-anchors 19` · `landing-preflight 19` · `lint-descriptions-trigger 2` · `verbatim-quotes 6` · `isFrozen 19`（合计 133） |
| B2 | `node .claude/scripts/release-pr.mjs --self-check` | `{"selfCheck":"release-pr","pass":19,"fail":0,"total":19,"ok":true}`，exit 0 |
| B3 | 现行 `parseSection2AnchorLists` 对「围栏内含 `## ` 行 + 其后 T2 锚行」夹具 | `{"union":["docs/a.md"],"problems":[]}`——T2 锚静默消失、零 problems |
| B4 | `--preflight` 对同形夹具（T2 锚正常行 / T2 锚加粗行各一份） | 正常行版 `anchorCount:1`、`anchorUnion:["docs/a.md"]`、`findings:[]`；加粗行版同样 `findings:[]`（围栏后的 `**目标文件**：` 漏报） |
| B5 | 等价载体：现行 `checkVerbatimQuotes` 切片 + 比对，跑 261025 批 PROPOSAL 的 §2 全部钉死块 × 两目标文件 | `{"s2FenceBlocks":11,"currentMissing":10,"trimMissing":8,"flips":[DEV-GOVERNANCE 句, arch-review-protocol 句]}`——trim 恰翻转 A5 两句、不多翻 |
| B6 | 等价载体：现行 vs 修补解析器，逐份跑 76 份 `PROPOSAL.md` / `PROPOSAL.template.md`（含本批稿） | `{"files":76,"unionDiff":0,"problemSetDiff":0,"newProblems":0}` |
| B7 | census（语料 = 起草期现行仓，含本批稿） | orphan-T：147 份 PROPOSAL 族 md 两列均 0；围栏未闭合：76 份中 0 份 |
| B8 | 亲核：`docs/templates/architect-landing-summary.template.md` 存在且含 packet-id 行 / manifest 块；`arch-review-protocol.md:53`、`track-b-judge-brief.md:22` 各含模板指针 | 三处均 `grep -cF` = 1 |
| B9 | 新字面碰撞基线：§5「白名单正推清单」13 条逐条 `grep -rlF`（排除本批目录） | 13 条全部 = 0 个文件命中 |
| B10 | **以本 PROPOSAL §2 逐字节文本装配进 pristine 副本**后，跑 `landing-fidelity-check.mjs --self-check` 与 `release-pr.mjs --self-check`（装配脚本从本稿 js 围栏块抽取补丁文本，不手抄） | 前者 10 组全 `ok:true`、分组 = 基线 B1 但 `section2Anchors 29` / `landing-preflight 20` / `verbatim-quotes 10`（合计 142）、exit 0；后者 `{"pass":26,"fail":0,"total":26,"ok":true}` |

**落地验证集**：

| # | 时点 | 命令 | 预期（实跑回填） | 判别力 / 配对 |
|---|---|---|---|---|
| V1 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --self-check` | exit 0；逐组 `ok:true`，且 `section2Anchors` pass = **29**、`landing-preflight` = **20**、`verbatim-quotes` = **10**，其余组与基线 B1 逐字相同 | 反转实现逐条变红：边界不跳围栏 → `fence-boundary-union-keeps-post-fence-entry`（现行 union 缺 `docs/b.md`，B3）与 `跳围栏·围栏后加粗锚行仍报`（现行 0 条，B4）；不 trim → `verbatim-clause-midline-trailing-newline-missing`（现行 missing 1）；切片不跳围栏 → `verbatim-fence-boundary-checked`（现行 0 块，B5 同根） |
| V2 | pre-commit | `node .claude/scripts/release-pr.mjs --self-check` | `{"pass":26,"fail":0,"total":26,"ok":true}`，exit 0；末段逐条 PASS 含 `extract-gov-changeids` / `gov-lane-title` / `gov-lane-body-pointer` / `gov-lane-no-zero-pod` / `dev-lane-title` / `unknown-lane-title` / `unknown-lane-body` | 反转实现（现行逻辑）在 gov 夹具上标题 = `release(261025): 0 个 Pod 合并`（`:258` 模板 × 空 changeIds，PR #40 同形）→ `gov-lane-title` / `gov-lane-no-zero-pod` 双红 |
| V3 | pre-commit | 见下「V3 直探脚本」（`node --input-type=module` 直调共享解析器） | `{"union":["docs/a.md","docs/b.md"],"problems":[]}` | 与 B3 直接对拍（负向：`docs/b.md` 不在现行 union；正向：落地后恰在） |
| V4 | pre-commit | 两份夹具各跑 `node .claude/scripts/landing-fidelity-check.mjs --preflight <夹具路径>`（正常锚行版 / 加粗锚行版；两份内容见本节「V4 夹具」块） | **装配版实跑**：正常版 `ok:true`、`anchorCount:2`、`anchorUnion:['.claude/scripts/a.mjs','docs/b.md']`、`findings:[]`；加粗版 `ok:false`、`anchorCount:1`、`findings` 含 `bold-anchor-line lines:[9]` 与 `missing-anchor-list`（点名「T2 · 乙 无「目标文件：」行」） | 与 B4 对拍（现行两版均 `findings:[]`、正常版 `anchorCount:1`）；夹具勾呈现字形与 §2 T1 改法 5 的 `fenceBoundaryBold` 同形 |
| V5 | pre-commit / post-commit | 见下「V5 语料脚本」（以在仓的**落地后**解析器跑语料；零漂移的对照基线 = 起草期 B6 的现行 vs 修补同 run 逐份比对，已实测 0/0/0） | **落地后固定语料**的起草期实测：同一语料（76 份，含本批稿）= `{"files":76,"filesWithProblems":6}`——6 份为存量既有 problems（4 份归档旧批：无 §2 / 缺锚清单；2 份模板骨架：§2 只有围栏内示范、无真 T 条目），**全部在现行解析器下同样存在**（B6 problemSetDiff = 0），非本批引入；语料随新批次增长，落地期以实跑为准 | 若新边界误吞节内内容，problems 面会相对 B6 同语料多出文件（B6 已证 newProblems 0）；**强判别已由 B6 双实现同 run 对拍承担**，V5 是落地面常驻 sanity |
| V6 | pre-commit | 见下「白名单正推清单」：13 条 `grep -cF`（逐文件） | 13 条逐条 = **1**（基线 B9 均为 0） | 处方正推白名单（T4 第 2 条自身的执行）；每条字面均出自 §2 处方新文 |
| V7 | pre-commit | 配对断言（T5 自身的执行）：`grep -cF "includes(norm(block))" .claude/scripts/landing-fidelity-check.mjs`；`grep -cF "includes(norm(block).trim())" .claude/scripts/landing-fidelity-check.mjs`；`grep -cF "（未从 merge-driver 合并行提取到 changeId）" .claude/scripts/release-pr.mjs`；`grep -cF "（制品见 delivery/review/" .claude/scripts/release-pr.mjs` | 旧比对 = **0**（基线 1）/ 新比对 = **1**（基线 0）；旧兜底字面 = **0**（基线 1）/ 新 gov 指针片段 = **2**（起草期预期 1，落地实跑覆写——见 §7 留痕） | 负向 ×2 各配一条正向（同文件同面），任一单独漂移即红 |
| V8 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261026-gov-review-p2-anchor-template-cluster/PROPOSAL.md` | `ok:true`、`anchorCount:7`、`anchorUnion` = manifest 七行逐条一致、`findings:[]`、`advisories:[]` | 起草期自检实跑记录见文末「起草期自检留痕」 |
| V9 | pre-commit | `git diff --name-only -- docs/contracts/ .claude/agents/ .claude/hooks/ .claude/scope-map.json .claude/*.settings.json .claude/verify-gates.json` | 空输出 | 契约 / agent 定义 / hooks / settings / verify-gates 全零触碰（负向）；正向配对 = `git diff --name-only` 恰等于 §2 七文件锚并集（landing-verifier 逐件核） |
| V10 | landing 期 | `node .claude/scripts/landing-fidelity-check.mjs delivery/review/261026-gov-review-p2-anchor-template-cluster <landing 前 ref>` | `ok:true`、`proposalTargets` / `summaryTargets` 各 7、`unauthorized:[]`、`claimedNotLanded:[]`、`plannedNotLanded:[]` | **未实测**（依赖 summary 与 git 落地面，结构上不可能在起草期跑）；形态由 V1 的自检与既有门行为背书 |
| V11 | post-commit | 复跑 V1 / V2 / V5 / V6 / V7 | 与上表逐值相同 | **未实测**（依赖落地提交）；落地提交后由主线程 / landing-verifier 执行 |
| V12 | post-commit | `node .claude/scripts/pre-merge-escalation-gate.mjs 261026-gov-review-p2-anchor-template-cluster --source-ref HEAD` | `PASS (…, kind=gov)`，exit 0 | **未实测**（依赖 FROZEN + landing-verdict）；本批无契约 diff，闸的契约凭据分支不触发 |

**V3 直探脚本**（pre-commit；bash 下逐行执行即可，反引号与换行用 charCode 构造以免嵌套转义）：

```sh
node --input-type=module -e "import { parseSection2AnchorLists } from './.claude/scripts/lib/governance-gate-shared.mjs';
const B = String.fromCharCode(96), N = String.fromCharCode(10);
const fx = ['## 2. 改法','### T1 · 甲','目标文件：'+B+'docs/a.md'+B+'（改）','',B+B+B+'md','## 围栏内节标题',B+B+B,'','### T2 · 乙','目标文件：'+B+'docs/b.md'+B+'（改）','','## 3. 影响面'].join(N);
console.log(JSON.stringify(parseSection2AnchorLists(fx)));"
```

**V4 夹具**（两份单行数组，各自写入临时 .md 后喂 `--preflight`；加粗版与本稿 §2 T1 改法 5 的 `fenceBoundaryBold` 逐字相同）：

```js
// 正常锚行版（装配版实跑：ok:true、anchorCount:2）
const v4Normal = ['# 夹具', '## 2. 改法（T1）', '### T1 · 甲', '目标文件：`.claude/scripts/a.mjs`（改）', '```md', '## 围栏内节标题', '```', '### T2 · 乙', '目标文件：`docs/b.md`（改）', '## 3. 影响面'].join('\n');
// 加粗锚行版（装配版实跑：ok:false、bold-anchor-line lines:[9] + missing-anchor-list 点名 T2）
const v4Bold = ['# 夹具', '## 2. 改法（T1）', '### T1 · 甲', '目标文件：`.claude/scripts/a.mjs`（改）', '```md', '## 围栏内节标题', '```', '### T2 · 乙', '**目标文件**：`docs/b.md`（改）', '## 3. 影响面'].join('\n');
```

**V5 语料脚本**（pre-commit / post-commit 均可跑；输出全仓语料在**落地后**解析器下的 problems 面）：

```sh
node --input-type=module -e "import { readFileSync, readdirSync } from 'node:fs';
import { join } from 'node:path';
import { parseSection2AnchorLists as parse } from './.claude/scripts/lib/governance-gate-shared.mjs';
function walk(d, out = []) { for (const e of readdirSync(d, { withFileTypes: true })) { const p = join(d, e.name); if (e.isDirectory()) walk(p, out); else if (e.name.endsWith('.md')) out.push(p); } return out; }
const files = []; for (const d of ['delivery/review', 'delivery/archive', 'docs/templates']) walk(d, files);
const targets = files.filter((f) => /PROPOSAL(\.template)?\.md$/.test(f));
let filesWithProblems = 0;
for (const f of targets) if (parse(readFileSync(f, 'utf8')).problems.length) filesWithProblems++;
console.log(JSON.stringify({ files: targets.length, filesWithProblems }));"
```

**白名单正推清单（T4 第 2 条的执行；字面逐字取自 §2 处方新文，命令形态 = `grep -cF '<字面>' <文件>`）**：

| # | 文件 | 字面（行首的 `## ` 标题行亦按整串计） | 预期 |
|---|---|---|---|
| 1 | `.claude/references/arch-change.md` | `## Track B 制品义务（landing-summary 结构两件 + 验证集实跑回填）` | 1 |
| 2 | `.claude/references/arch-change.md` | ``Track B 落地的**制品义务**单源`` | 1 |
| 3 | `.claude/references/arch-change.md` | ``引字面 `packet-id` token，且同文引被批 review id`` | 1 |
| 4 | `.claude/references/arch-change.md` | ``含 `landing-manifest` 围栏块：一行一个 canonical 目标路径`` | 1 |
| 5 | `.claude/references/arch-change.md` | `验证集预期值实跑回填（ARQ-261021-proposal-assert-expectations）` | 1 |
| 6 | `.claude/references/gov-change.md` | `落地制品义务（landing-summary 结构两件 + 验证集实跑回填）见` | 1 |
| 7 | `docs/templates/PROPOSAL.template.md` | `【硬项 · 验证集撰写三规则】` | 1 |
| 8 | `docs/templates/PROPOSAL.template.md` | `预期值现跑回填（ARQ-261021-proposal-assert-expectations）` | 1 |
| 9 | `docs/templates/PROPOSAL.template.md` | `白名单 / 计数从处方正推（ARQ-261019-forward-derived-verification-set）` | 1 |
| 10 | `docs/templates/PROPOSAL.template.md` | `负向断言须配正向 happy-path（ARQ-261025-verify-set-negative-positive-pairing）` | 1 |
| 11 | `.claude/references/track-b-judge-brief.md` | `配对与实跑抽查（261026 批，ARQ-261025 / ARQ-261021）` | 1 |
| 12 | `.claude/scripts/release-pr.mjs` | `export function extractGovChangeIds(diffNamesText) {` | 1 |
| 13 | `.claude/scripts/release-pr.mjs` | `export function derivePrTitle({ ymd, suffix, changeIds, govChangeIds }) {` | 1 |

（各条在基线 B9 实测 0 命中；表内不含「处方新文回引旧路径」类命中——本批新文未回引任何旧路径字面，故白名单即全表。若第 1 条因行首判定需要，`grep -cF` 可直接以整串匹配，无需行首锚。）

## 6. 留根清单

**本批结项（landing 后由主线程执行登记 / 状态变更，architect 不做）**：`ARQ-261019-anchor-probe-fence-blind`、`ARQ-261019-verbatim-quote-line-level-false-negative`、`ARQ-260928-landing-summary-skeleton-template`、`ARQ-261021-landing-summary-template`（两条同根并条，结项时注「前提已过时：模板 260802 已存在，本批落 face SSOT 义务句」）、`ARQ-261019-forward-derived-verification-set`、`ARQ-261025-verify-set-negative-positive-pairing`、`ARQ-261021-proposal-assert-expectations`、`ARQ-261026-release-pr-gov-lane-zero-pods`（搭载）。**不改动**：`ARQ-260924-landing-gate-dir-anchor-gap`（残余②独立批）。

**留根与复议触发**：

1. **orphan-T 启发式不做**（取舍 A）：以「围栏未闭合」fail-loud 替代。复议触发 = 再出现一次「§2 被非围栏 `## ` 标题截断、其后条目静默消失且 §2↔manifest 对账未被走到」的实例（例如起草期只跑 preflight、landing 门被跳过）。
2. **「围栏未闭合」是新增 fail-loud**：存量零命中（B7），但未跟踪出「合法用法下 fence 定界行奇偶」的例外（例如正文里以行为单位书写的四反引号围栏）。复议触发 = 出现一次被该 problem 误拦的真实 PROPOSAL。
3. **其余 §2 类切片点未动**：`.claude/scripts/spec-anchor-check.mjs`（spec 文档的 `## 2.` 面）与 `contract-lint.mjs`（节计数）不做围栏语义同步——它们面向 spec / 契约文档，不是 PROPOSAL §2。复议触发 = 出现同类 fence-blind 误判实例。
4. **T7 检测面 = `delivery/review/` 路径**：只改 workstream / debt 账本、无 review 目录的 gov 形态发布 → 落「变更线未识别」（诚实标注，非零信息「0 个 Pod」）。复议触发 = 观察到主线程仍需人工重写 PR 描述的 gov 批。
5. **ARQ-261021 的机判判据改口径**：模板已存在，跟踪两批 summary 格式 FAIL；若再 FAIL，先查「模板指针是否被跳读 / face SSOT 义务句是否够醒目」，不立新模板。复议触发 = 连续两批 summary 首跑 FAIL。
6. **义务句的多落点漂移风险**：summary 义务现出现在五处（门脚本判据 / landing-summary 模板 / protocol 七步第 4 步 / verifier 简报 / arch-change 新节），gov-change 只放指针。SSOT 取「模板 = 细节、arch-change 新节 = 义务定义、门脚本 = 判据」。复议触发 = 五处表述出现语义分歧。
7. **T5 硬项标签**：本批裁定为【硬项】（取舍 D）；若后续评审体制调整【硬项】语义，同步复核模板 §5 与该标签。复议触发 = 模板【硬项】语义变更。
8. **T7 两线皆空的诚实标签**（取舍 E）：若判官删该分支，`unknown-lane-*` 两条自检夹具同步删除、V2 计数相应减 2（26 → 24），其余不变。

**环内自律**：本批有 scratch 等价载体（`.claude/scripts/**` 三份临时副本，置于仓外临时目录，不进仓、不 stage）。落地实现若与 §2 文本有字节差异，以落地实跑为准覆写 §5 预期并在 §7 留痕；不把起草期读数当既有事实转引。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-26 - architect-reviewer

**轮次**：R1 全面审（四查 + 评审透镜逐项过，未裁剪条目）。
**判档确认**：Track B 主路径（非轻路径）——本批 diff 触 `.claude/scripts/**`，`gov-change.md`「轻路径判据」③ 机器面命中即恒 Track B（亲核 `:18` 原文）；Track A 段两判据（跨系统结构性 / 方向未收敛）均不命中，判档成立。

verdict: GREEN

**Findings（0 BLOCKING；3 NIT 记录在案，不阻断，无需改法轮）**：

- **F1（NIT · 事实表述精度）**：复证 1 / §3 census 的「76 份 PROPOSAL.md / PROPOSAL.template.md」按文件名精确口径实为 **75** 份（review 19 + archive 55 + templates 1）；第 76 份 = `docs/templates/WORKSTREAM-PROPOSAL.template.md`（basename 以 `PROPOSAL.template.md` 结尾，被 §5 V5 语料脚本的文件名尾锚正则命中）。计数与 V5 脚本口径一致、V5 断言不受影响；建议主线程结项订正 ARQ 时把 prose 口径改写为「匹配 V5 语料正则的 76 份」。
- **F2（NIT · census 括注）**：§3「147 份 PROPOSAL 族 md（含模板与 landing-summary / verdict 类）」实测可复现口径 = proposal 族 + summary 族（19+19+55+51+2+1 = 147）；若再计 verdict 族为 218。数字按最自然读法可复现，仅括注列举与实际过滤集不完全一致，不承载任何断言。
- **F3（NIT · fail-loud 条件边界，留根已覆盖）**：T1「围栏未闭合」判据以**全文件** fence 定界行计数为准（非 §2 域内计数）——两个未闭合围栏跨节互相配对（全文件偶数）时该 fail-loud 不触发。PROPOSAL §6 留根 2 已登记残余风险并给复议触发；主修复（边界探测跳围栏）不依赖该判据，无阻断。

**亲核留痕（ground truth 实测，本会话独立复现，非转述采信）**：

1. **源码引据 byte 级核对**：`governance-gate-shared.mjs:363/:371-375/:393-395`、`landing-fidelity-check.mjs:214-224/:452-480/:455-459/:474`、`release-pr.mjs:102/:120/:168-182/:257-260/:264/:393/:433-446/:644`——§2 全部「原文」引据与盘上逐字一致。
2. **基线命令复跑**：B1 = 10 组全 `ok:true`（section2Anchors 25 / landing-preflight 19 / verbatim-quotes 6，合计 133）、exit 0；B2 = `{"pass":19,"fail":0,"total":19,"ok":true}`。V3 直探（现行解析器）= `{"union":["docs/a.md"],"problems":[]}`（B3 逐字复现）；V8 preflight = `anchorCount:7`、union 与 manifest 七行逐条一致、findings / advisories 空、exit 0（与文末留痕逐值一致）。
3. **B6 独立复现**：按 §2 T1 改法 1/2 逐字节装配 lib 副本（仓外临时目录，不入仓），现行 vs 修补双实现对跑 76 份语料 = `{"files":76,"unionDiff":0,"problemSetDiff":0,"newProblems":0}`；行为夹具四条（修补版 union 收进 docs/b.md / 真节边界仍终止 §2 / 围栏未闭合 fail-loud 触发 / 早退不吞围栏 problem）全部与 PROPOSAL 声称一致。
4. **B10 独立复现**：装配版 `landing-fidelity-check.mjs --self-check` = section2Anchors **29** / landing-preflight **20** / verbatim-quotes **10**、10 组全 `ok:true`、exit 0；装配版 `release-pr.mjs --self-check` = `{"pass":26,"fail":0,"total":26,"ok":true}`（`extract-gov-changeids` / `gov-lane-*` / `dev-lane-title` / `unknown-lane-*` 七条新夹具逐条 PASS）。
5. **V5 语料实测**（现行解析器）：`{"files":76,"filesWithProblems":6}`，6 份 = 4 归档旧批（无 §2 / 缺锚）+ 2 模板骨架，与 V5 预期逐值一致、全部存量既有 problems。
6. **census 抽查**：`parseSection2AnchorLists` 唯一消费方 = `landing-fidelity-check.mjs` ✓；`buildPrBody` / `extractChangeIds` 全仓无第二消费方 ✓；`rmSync(fixtureDir` 恰 2 处（T2 插入位警告属实）✓；`spec-anchor-check.mjs` / `contract-lint.mjs` 在场（留根 3 成立）✓；fence 奇数 census 按修补后精确条件复算 76 份 = **0** 命中（B7 ✓，且 261023 批 PROPOSAL 的跨节 ADR 围栏为真实 fence-blind 样本、修补版下零漂移）；B9 白名单 13 条基线逐条 `grep -rlF` = **0** ✓。
7. **插入点与指针在场**：`arch-change.md`「轻路径落地通道」:27 /「触契约分支」:31（T3 插入域成立，八节标题无同名撞车）；`gov-change.md` :30 末句逐字 ✓；`PROPOSAL.template.md` :84-86（插入落 §5 末、`## 6` 之前，无歧义）；`track-b-judge-brief.md` :32 透镜第 6 条逐字 ✓；B8 三处指针（summary 模板 :5 / :12 / :30-32，`arch-review-protocol.md:53`，`track-b-verifier-brief.md:22`）✓；复证 2 引据 `docs/DEV-GOVERNANCE.md:214` / `arch-review-protocol.md:58` ✓。
8. **意图保真**：human 2026-10-26 三取舍（两句字面义务不立模板 / 实跑回填扩至全部实测数字 / release-pr 最小版）在 T3 / T6 / T7 逐条落位；搭载面 = workstream P3 项 ✓；`ARQ-260924` 残余②零触碰（manifest 不含其路径）✓；七文件锚并集 = manifest = preflight union 三方一致 ✓。
9. **结构字段在场**：landing-manifest 围栏块（:10-18）与 packet-binding 行（:6）在场 ✓；riskTier low-medium 证成（结构门 + 发布入口全仓必经）与 profile G2+G3、G1 / 契约 / agents / hooks / settings 零触碰声明与 §3 sweep 表自洽 ✓。

**升级 human 项**：无。

## 起草期自检留痕（模板四句自检的落地记录，2026-10-26）

1. **preflight 实跑**（只读干跑，V8）：见本节末「preflight 输出」块。

2. **路径形态族谱**（`path-form-family.md`）：本批不改任何路径的落点与文件名（无搬家、无改名），新增的是两个函数名（`extractGovChangeIds` / `derivePrTitle`）与一个新问题串（「围栏未闭合」），都已按 §3 硬项段做碰撞 census（新字面 13 条基线 0 命中；函数名全仓 0 命中）。

3. **承重引文 byte 对账**：§2 引用的每一处被替换原文 / 锚句，起草期以脚本 `split(anchor).length - 1` 逐条核过恰 1 次（**15 处全部 = 1**，从本稿围栏块机械抽取、不手抄）：lib 的边界段与早退行；lfc 的 section2Body 段、verbatim 切片段、比对行；release-pr 的 buildPrBody 整段、第 8 段两行、body 调用行、第 8 段注释、live mergeLog 行、汇总行；模板 §5 骨架句；判官简报透镜第 6 条；arch-change 轻路径落地通道末句；gov-change Track B 后端整行。

4. **长文落地自检**：本文件分四块写入，写后机检 = `wc -l`、U+FFFD 替换字符扫描零命中、`### T<n>` 标题计数 = 7、`目标文件：` 锚行计数 = 7、§2 围栏内以 `## ` 起头的行计数 = 0（规避本批正在修的 fence-blind 缺陷；T3 的新节标题行以行内钉死形态承载）。

5. **落地文本装配核验（B10 的执行方式）**：把本稿 §2 的 js 围栏块机械抽取、装配进三份 pristine 副本后实跑——① `landing-fidelity-check.mjs --self-check` 10 组全绿（section2Anchors **29** / landing-preflight **20** / verbatim-quotes **10**，exit 0）；② `release-pr.mjs --self-check` **26/26**（含 7 条新夹具逐条 PASS）；③ 本稿 §2 T7 三个函数文本单独抽成模块直调，9 条行为断言全 PASS（gov changeId 提取、三态标题、三态正文、公共行不变）。装配期实测抓到一个落地陷阱并已写进 T2 的插入位置说明：`landing-fidelity-check.mjs` 内 `rmSync(fixtureDir, …)` 出现两处，以它为锚会把 verbatim 夹具插进 citation 自检段并令 `--self-check` 运行期崩溃——正确锚是 `check('verbatim-crlf-checked', …)` 行之后。

**preflight 输出**（V8 实跑，2026-10-26，exit 0）：

```
$ node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261026-gov-review-p2-anchor-template-cluster/PROPOSAL.md
{"ok":true,"gate":"landing-preflight","proposal":"delivery/review/261026-gov-review-p2-anchor-template-cluster/PROPOSAL.md",
 "section2BodyFound":true,"anchorCount":7,
 "anchorUnion":[".claude/references/arch-change.md",".claude/references/gov-change.md",
   ".claude/references/track-b-judge-brief.md",".claude/scripts/landing-fidelity-check.mjs",
   ".claude/scripts/lib/governance-gate-shared.mjs",".claude/scripts/release-pr.mjs",
   "docs/templates/PROPOSAL.template.md"],
 "advisories":[],"findings":[]}
exit=0
```

`anchorUnion`（7 项）与头部 landing-manifest 围栏块逐条一致；`advisories` 空 = 锚行未夹带路径记号；`findings` 空 = 无加粗锚行、无缺锚（围栏内 `## ` 行 0 条——本批正在修的缺陷未在自身稿触发，规避动作见本节第 4 条）。

### Landing 留痕（主线程 2026-10-26，packet-bound 落地期）

- **落地实跑覆写一处（本文件冻结纪律「落地期实跑若与预期有字节差异，以落地实跑为准覆写 §5 并在 §7 留痕」的执行）**：V7 第 4 条「新 gov 指针片段」起草期预期 1，落地实跑 = **2**。根因 = 该字面 `（制品见 delivery/review/` 在 §2 处方新文中出现两处（`buildPrBody` 实现行 + `gov-lane-body-pointer` 自检夹具行），起草期白名单只计了实现行、漏计夹具行——恰是本批 T4 立的「白名单从处方正推」规则应抓到的形态，落地期按正推规则覆写为 2。非语义偏差：两处都出自处方新文、方向 = 断言更严（多计一处命中锚点）。
- **pre-commit 验证集实跑汇总（落地后、commit 前，全绿）**：V1 `landing-fidelity --self-check` 10 组全 `ok:true`，`section2Anchors 29` / `landing-preflight 20` / `verbatim-quotes 10`（与预期逐值一致）；V2 `release-pr --self-check` `{"pass":26,"fail":0,"total":26,"ok":true}`（7 条新夹具逐条 PASS）；V3 直探 `{"union":["docs/a.md","docs/b.md"],"problems":[]}`；V4 正常版 `ok:true anchorCount:2` / 加粗版 `ok:false` + `bold-anchor-line lines:[9]` + `missing-anchor-list 点名 T2 · 乙`；V5 `{"files":76,"filesWithProblems":6}`（全部存量既有）；V6 白名单 13 条逐条 = 1；V8 preflight `ok:true anchorCount:7` union 与 manifest 逐条一致；V9 禁触面 diff 空。
