# PROPOSAL · 261027-gov-review-p6-landing-gate-cluster

> **状态：FROZEN v0.2 · 2026-10-27（凭 human 立项 ack + R2 GREEN 标冻结；N1 括注收口后无未处置 findings。沿革：DRAFT v0.1 → R1 BLOCK(F1,F2,F3,F4) → DRAFT v0.2 → R2 GREEN → N1 微调。Track B 主路径；三条一批，dir-anchor ② 批内独立 T 全强度评审）。**
> **承载 ARQ**：`ARQ-260914-landing-preflight-anchor-parse`（SHOULD，T1；机械化判据两子项，条目维持 deferred——③ 连带项非机械化半仍待排期）、`ARQ-261026-verbatim-extract-backtick-cut`（NIT，T2，整条结项）、`ARQ-260924-landing-gate-dir-anchor-gap`（SHOULD，T3，整条结项）。三条同批清偿；不关的根见 §6。
> **review 轨**：**Track B 主路径**（human 2026-10-27 立项 ack 定；三条 diff 全触 `.claude/scripts/**`，按 `gov-change.md` 轻路径判据③「机器面命中即恒 Track B」）。Track A 段两条判据均不命中：非跨系统结构性变更（不改固件 / shim / 契约 / spec 两文件），方向由 human 两拍收敛（载体口径 + 目录锚口径）。**T3 按 human 裁在批内作独立 T 全强度评审**（触承重闸判定逻辑）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T3）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：**medium**（一刀证成：T3 改承重闸的 claimed/planned-but-not-landed 判定语义——目录 token 由「精确匹配必假 RED」改为「前缀展开兜底」；T1/T2 为只读自查载体与提取精度，不改任何 RED/exit 语义。失败模式与缓释见 §2 T3 末）。
> **review profile**：**G2+G3**（G3 = `.claude/scripts/landing-fidelity-check.mjs`；G2 = `docs/templates/PROPOSAL.template.md` 治理制品骨架；另触 `docs/debt/gov/**` 三条目记账）。**G1 零触碰、契约面零触碰、hooks / scope-map / settings / verify-gates 零触碰、无新增门。**
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段，GREEN 结论前必须在场；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/landing-fidelity-check.mjs
docs/debt/gov/ARQ-260914-landing-preflight-anchor-parse.md
docs/debt/gov/ARQ-260924-landing-gate-dir-anchor-gap.md
docs/debt/gov/ARQ-261026-verbatim-extract-backtick-cut.md
docs/templates/PROPOSAL.template.md
```

---

## 0. 想实现的效果与差距

**目标效果**（P6 批，human 2026-10-27 立项 ack 两拍；复证记录与拍板原文见 `delivery/workstreams/gov-debt-reduction/task_plan.md` P6 节）：

1. **承重引文 / 插入文本的 byte 对账从「每批判官与核验官现写 node 脚本」变成一条可复用子命令**：`landing-fidelity-check.mjs --cite-check <PROPOSAL> [--phase draft|landing]`——起草期对 §2「原文」引文对磁盘现文自跑，FROZEN 后对「改后 / 替换后 / 插入 / 追加」文本对落地文件自跑；把「目视转录形近而误 / 转义伪影 / 插入段漏 1 字符」这类**提交前即可自证**的 finding 前置拦截（载体按 human 拍定 ① 挂子命令、否独立脚本）。
2. **围栏提取精确化**：块内容中的 ``` 字面（js 夹具串形态）不再被当作块边界——观察列伪 missing 归零，advisory 可信度恢复（261026 批 landing 观察列 24 条伪 missing，landing-verdict ADVISORY-1 在案）。
3. **landing 门目录锚口径收敛**：按 human 拍定 ②，目录 token 由「逐文件精确匹配必假 RED」改为**前缀展开兜底**（门不再对目录简写假 RED），同时保持「仍劝阻」——advisory 文案与模板 manifest 纪律句同步改写为「可用但劝阻、逐件枚举是正形、展开是兜底不是许可」，迁移批写作习惯不变。
4. **双信号源规约成文（正读数镜像）**：工作树形态类断言（CRLF/LF、BOM、尾空白）须两个独立信号源一致方可写入正式制品；msys awk/sed 剥 `\r` 的已知工具伪影随句承载（模板起草期自检句 ③ 扩写）。
5. **三条 ARQ 如实收口**：260914 部分结（两子项落、余项留根）、261026 结项、260924 结项；由 `debt-render.mjs` 按 status 自动归位，人不搬文件。

**现状差距**（全部为起草期 2026-10-27 实跑或亲核，命令与输出见 §2.0 复证与 §5 基线）：

- `--cite-check` 不存在：`grep -c "cite-check\|citeCheck" .claude/scripts/landing-fidelity-check.mjs` = **0**，`.claude/scripts/` 下无独立 cite 脚本。判官 / 核验官每次现写 node byte 比对（260924 批 retro §4-1 代价面：判官 R1 六项 finding 四项属「提交前可自证」类，估计可省 R2/R3 两轮 ~455k tokens + 核验一轮 ~142k）。
- 双信号源规约无承载：`PROPOSAL.template.md:69` 的起草期自检句 ③ 只有「工具**负**结果先交叉第二信号源」半边（原文三实例在册）；**正读数镜像**（CRLF/LF、BOM、尾空白断言）与 msys 剥 `\r` 伪影清单零字面。
- 围栏提取正则原样：`landing-fidelity-check.mjs:472` 仍为 `/```[a-zA-Z]*\n([\s\S]*?)```/g`，无独立成行判定。
- 目录锚两门口径差仍在：`:656` / `:658` 仍 `changed.has(t)` 精确成员判定；`:247-251` 的 `dirTokenAdvisories` 文案与 `PROPOSAL.template.md:23` 的 manifest 说明行仍写「landing 门不消费目录锚 / 目录简写收口期必红」——T3 落地后该句成为假话，必须同步改写。
- 台账：三条目 frontmatter 均 `status: deferred`。

**非目标（本批不做，防 scope creep）**：

- **不新增或改动任何门**：不改 `hooks` / `scope-map` / `settings` / `verify-gates` / `pre-merge-escalation-gate.mjs`；`--cite-check` 不接入任何闸链（只读自查子命令，退出码自管）。
- **不改契约面与 spec 两文件**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md` 零 diff。
- **不动实现面**：`firmware/**` / `server/**` / `tools/**` 零 diff。
- **不覆盖 blockquote 承载的插入段**：`--cite-check` v1 覆盖面 = §2 围栏块 + 行内 code-span 两种形态；blockquote 插入段与多 span 混合行内形态列入 §6 留根（宁可如实申报不覆盖，也不做会误报的解析）。
- **不改围栏近似的宽松态**：`fenceFlags`（本脚本）与 `parseSection2AnchorLists` 的 `fenceLineFlags`（`governance-gate-shared.mjs`）仍用「行首 ``` 即翻转」，本批不改（只用于节域近似，与提取器职责不同；见 §6 留根 2）。
- **不改迁移批 manifest 写作习惯**：逐件枚举仍是正形；目录锚展开只作兜底（human 拍定 ② 原话口径）。
- **不动历史制品**：已归档批的 PROPOSAL / verdict / retro 零改；三条 ARQ 只 append 结项 / 部分结注记。

## 2. 问题、根因与改法（T1…T3）

### 2.0 复证结论（起草期 2026-10-27；ARQ 行是待验证假设）

> 纪律：每条结论对应实跑命令或亲读文件；ARQ 原文的路径 / 行号 / 机制描述按「搬运引据视同新断言复证」逐条核过，漂移处如实更正。

**复证 1 · `ARQ-260914` 残余两子项确认未消化；既有机械化半边边界亲核。**
- **byte 比对脚本化未消化**：`grep -c "cite-check\|citeCheck" .claude/scripts/landing-fidelity-check.mjs` = 0；`.claude/scripts/` 目录下无独立 cite 脚本（`ls | grep -i cite` 零命中、exit 1）。
- **既有覆盖边界亲核**：`checkVerbatimQuotes`（`:458`）只做「§2 围栏块正文 → landing-manifest 目标文件」单向对账（landing 侧半边），且以 advisory 列挂主门（`:748`）；**起草侧「PROPOSAL 引文 vs 磁盘现文」无任何载体**——起草期只有 `--preflight`（`:545` 起）查锚清单**形态**，不查引文内容。故本批不重复造已有机能：新子命令复用同一提取器与同一逐字判据（行尾归一 + trim），只补相位与角色两个维度。
- **双信号源规约无承载**：`grep -n "第二信号源" docs/templates/PROPOSAL.template.md` = **:69 一处**，内容是「工具**负**结果（Edit not found / 探针 -1）先用第二信号源交叉再归因」——正读数镜像（CRLF/LF、BOM、尾空白两源一致）与工具伪影清单零字面。追注 §4-4 的实例链在条目正文（批 B F2：msys awk 剥 `\r` 致 bareLF=75 假读数，单源写入 summary 成披露不实）。
- **同条目另三子项已在场**（不重做）：围栏承载条款 `:65`、评估时点逐子句 `:69①`、目录 token advisory `:245-250`。

**复证 2 · `ARQ-261026` 判定成立；噪声规模以 261026 批判定现场为准。**
- **正则原样在场**：`:472` = ``  const fenceRe = /```[a-zA-Z]*\n([\s\S]*?)```/g; ``（逐字），无独立成行判定；块内容中的行内 ``` 必被当边界提前闭合。
- **立案现场**：`261026-gov-review-p2-anchor-template-cluster` 的 landing-verdict ADVISORY-1 记述：「verbatim-quote 观察列 24 条 missing，其中大量为伪 missing——PROPOSAL §2 的 js 夹具串内含 ``` 字面（如 `'```md'` 数组元素）… 块被错切」；该批核验官独立核为 advisory 语义缺陷、非落地偏差。
- **相邻两形态已修、不重开**：`:461-471`（§2 切片跳围栏，261026 批 T1）与 `:486-488`（块尾换行 trim，261026 批 T2）在场；本批只修第三条形态（闭合边界）。
- **判别力实测（新分类器接入后）**：对 P2 的 FROZEN PROPOSAL 复跑新版 `--cite-check --phase landing` 得 checked 17 / missing **1**——恰为该批 ADVISORY-2 记录的注释 byte 偏差（`governance-gate-shared.mjs:380`「fence 定界**行为**奇数」vs FROZEN 处方「fence 定界行**总数**为奇数」），即旧 24 条伪 missing 噪声里唯一的真偏差；新工具在历史批上自动复现了判官人读发现（§5 B7）。

**复证 3 · `ARQ-260924` 残余② 判定成立；①②③ 三分与 2026-09-24/25 两段实测原样有效。**
- **精确匹配在场**：`:656` 与 `:658` 仍 `changed.has(t)`；`gitChangedFiles`（`:574-589`）逐文件枚举（`-uall`），目录路径永不命中。
- **处方①③ 已落地（本批不改语义、只改文案口径）**：preflight `advisories` 键在 `:272`、`dirTokenAdvisories` 在 `:247-251`；模板 `:23` 纪律句在场。
- **两门口径差机制面**：preflight 锚提取用 `ANCHORABLE_TEST`（`governance-gate-shared.mjs:42`，前缀正则，目录 token 必命中）；landing 门用精确集合成员——同一 token 起草期绿、收口期 RED（260923 批 `docs/crq/` 实录：human 追认 + 机械展开 99 行）。
- **口径改写必要性亲核**：human 拍定 ②「仍劝阻、展开作兜底」落地后，现行两句成为假话——`:250`「landing 门不消费目录锚（逐文件精确匹配 changed.has）」与模板 `:23`「目录简写收口期必红」。全仓现行承载点精确定位见 §3 复扫段（其余命中均为归档批历史制品与 ARQ 条目自身正文，append-only 不改）。

**复证 4 · 本批基线（供给侧，全部起草期实跑）。**
- **现行 `--self-check` 全绿 10 组**：`changeid-prefix 4/4` / `extractTargets 6/6` / `section2Anchors 29/29` / `resolveOrigin 29/29` / `computeFrozenPin 4/4` / `citation-anchors 19/19` / `landing-preflight 20/20` / `lint-descriptions-trigger 2/2` / `verbatim-quotes 10/10` / `isFrozen 19/19`（另有 isFrozen 的 conflicting-status-lines 自证 advisory，属既有预期输出）。
- **装配副本**：按本批 §2 装配后 1129 行、`node --check` 过、`--self-check` 全绿 12 组（新增 `cite-check 21/21`、`dir-anchor-expansion 7/7`，`verbatim-quotes` 10 → 13）（§5 B2）。
- **行尾**：五件目标文件 `git ls-files --eol` 均 `i/lf w/lf`（本 gov worktree 实况；落地逐文件保持 LF）。
- **仓内无 `_scratch` 残留纪律**：本批起草期脚手架在 `delivery/review/261027-gov-review-p6-landing-gate-cluster/_scratch/`，落地 commit 前删除（V5 正向断言机械兜底）。

### T1 · 承重引文 byte 对账脚本化（`--cite-check` 子命令）+ 双信号源规约成文（ARQ-260914 残余两子项）

**问题**：承重引文与插入文本的逐字性，今天唯一的网是判官 R1 与核验官各自**现写**的 node byte 比对脚本——作者侧没有任何可复用自检。后果是三批四实例的同型事故：锚文目视转录「形近而误」（260924 批 A 批 R1-F1/F2：planner.md「对端面」误作「对端条目」、`` `origin` `` 误作 `**origin**`）、转义伪影（B 批 R1-F1 near-miss：blockquote+code-span 转义形态若 byte 照抄进 merge-driver 即 SyntaxError）、插入段漏 1 字符（B 批核验 F1：490 vs 489 codepoints）。每一实例都靠判官 / 核验官一轮人读兜住，成本口径 = 两轮评审 + 一轮核验（该批 retro 估 ~455k + ~142k tokens）。另一半问题是**正读数**的同类事故：批 B F2 用 msys awk 数裸 LF 得 bareLF=75 假读数、单源写入 summary 成披露不实（实况六件纯 CRLF 全程未变），模板自检句只覆盖「负结果先交叉」半边。

**证据**：§2.0 复证 1（`--cite-check` 零字面、既有覆盖边界 `:458`/`:748`、模板 `:69` 只有负结果半边）；实例与代价链见 `ARQ-260914` 追注 (1)(2)(4) 与 260924 批 retro §3-F1/F2、§4-1、§4-3。

**根因**：① 对账能力留在**评审者手里**而不是**生产者手里**——作者无法在提交前自证承重文本逐字，判官只能当第一道机检；② 逐字判据在系统里已有（`checkVerbatimQuotes` 的行尾归一 + trim + includes），但它只挂在 landing 门、只有「落地后」一个相位、且不分引文角色（原文块落地后本应消失，混在同一 missing 列里制造噪声）；③ 正读数没有被当成断言——「我数出来是这样」直接成文，没有第二信号源义务，工具伪影（msys 文本模式）也没有成文清单。

**改法**（脚本六处 + 模板一处 + ARQ 一处；承载按 human 拍定 ① = `landing-fidelity-check.mjs` 子命令、不立独立脚本）：

0. **头注 import 增 `relative`**（`:31` 整行替换；供 `findQuoteInTargets` 回报实际命中文件路径——归档位兜底命中时读者看得见真实落点）：

原文（`:31`，逐字节）：
```js
import { basename, resolve, join } from 'node:path';
```

改后（逐字节）：
```js
import { basename, relative, resolve, join } from 'node:path';
```

1. **头注用法块扩写**（`:24-28` 整块替换；同块末六行记 T3 的目录锚兜底口径——同一次连续编辑，语义归 T3）：

原文（`:24-28`，逐字节）：
```text
// 用法：node .claude/scripts/landing-fidelity-check.mjs delivery/review/gov-review-<id> [baseRef]
// 「已落地」判定 = 工作树改动 ∪ 已提交改动；默认相对 HEAD~1，给 baseRef 则相对该 ref（含未提交）。
// landing 清单来源：读 PROPOSAL.md / architect-landing-summary.md 里的 ```landing-manifest 围栏块
// （一行一个 canonical 路径，authoring 格式契约见 architect-operating-model §5）。
// B2 fail-loud：任一侧无该块 → 显式 RED（格式不合格），不再正则兜底。
```

改后（逐字节）：
```text
// 用法：node .claude/scripts/landing-fidelity-check.mjs delivery/review/gov-review-<id> [baseRef]
// 「已落地」判定 = 工作树改动 ∪ 已提交改动；默认相对 HEAD~1，给 baseRef 则相对该 ref（含未提交）。
// landing 清单来源：读 PROPOSAL.md / architect-landing-summary.md 里的 ```landing-manifest 围栏块
// （一行一个 canonical 路径，authoring 格式契约见 architect-operating-model §5）。
// B2 fail-loud：任一侧无该块 → 显式 RED（格式不合格），不再正则兜底。
//
// 目录锚兜底（261027 批 T3 · ARQ-260924 残余②，human 2026-10-27 裁「仍劝阻、展开作兜底」）：manifest 里以 /
// 结尾的目录 token 按前缀展开为成员判定（任一下辖文件有改动即视为该行已落），展开明细进输出 dirAnchorExpansions。
// 仍劝阻使用——逐件枚举是正形（展开只验「有成员改动」、逐件漏落不再被点名）；纯文件路径行为不变（changed.has 精确判定）。
//
// 引文自查子命令（261027 批 T1 · ARQ-260914 残余「锚文逐字性 byte 比对脚本化」）：
//   node .claude/scripts/landing-fidelity-check.mjs --cite-check <PROPOSAL.md> [--phase draft|landing]
// 只读自查、不参与主门判定、不进任何闸链：draft（缺省按 Status 自动判）= §2「原文」引文 vs 磁盘现文；
// landing = §2「改后 / 替换后 / 插入 / 追加」及未标注围栏块 vs 落地文件。exit 0 全命中 / 1 有未命中 / 2 用法或输入缺失。
```

2. **CLI 解析扩 `--cite-check` / `--phase`**（`:53-60` 整块替换；与 `--preflight` 同法抽成纯函数、子命令参数不被位置参数消费）：

原文（`:53-60`，逐字节）：
```js
function parseCliArgs(rawArgv) {
  const argv = rawArgv.slice(2);
  const idx = argv.indexOf('--preflight');
  if (idx >= 0) return { preflight: argv[idx + 1] || null, positional: null, baseRef: null };
  return { preflight: null, positional: argv[0] || null, baseRef: argv[1] || null };
}
const CLI = parseCliArgs(process.argv);
const preflightArg = CLI.preflight;
```

改后（逐字节）：
```js
function parseCliArgs(rawArgv) {
  const argv = rawArgv.slice(2);
  const idx = argv.indexOf('--preflight');
  if (idx >= 0) return { preflight: argv[idx + 1] || null, citeCheck: null, phase: null, positional: null, baseRef: null };
  // 引文自查入口（261027 批 T1 · ARQ-260914）：与 --preflight 同为只读子命令，选项不被位置参数消费。
  const ci = argv.indexOf('--cite-check');
  if (ci >= 0) {
    const pi = argv.indexOf('--phase');
    return { preflight: null, citeCheck: argv[ci + 1] || null, phase: pi >= 0 ? argv[pi + 1] || null : null, positional: null, baseRef: null };
  }
  return { preflight: null, citeCheck: null, phase: null, positional: argv[0] || null, baseRef: argv[1] || null };
}
const CLI = parseCliArgs(process.argv);
const preflightArg = CLI.preflight;
const citeCheckArg = CLI.citeCheck;
const citeCheckPhase = CLI.phase;
```

3. **新增 `--cite-check` 实现与自检**（插入位置：`runVerbatimQuotesSelfCheck` 收尾 `:536-537` 之后、自检接线注释 `:539` 之前；含 `targetCandidates` 归档位兜底 / `findQuoteInTargets` 逐字判据单源 / `collectCiteQuotes` 相位与角色分列 / `runCiteCheck` 入口 / `runCiteCheckSelfCheck` 判别夹具）：

插入位置锚（原文 `:536-537`，逐字节）：
```js
  if (fail > 0) process.exit(1);
}
```

插入段（逐字节，落地照抄）：
```js

// ── 引文自查子命令 --cite-check（261027 批 T1 · ARQ-260914 残余「锚文逐字性 byte 比对脚本化」）──
// 定位：把判官 R1 / 核验官每次现写的 node byte 比对固化为可复用件（载体 = 本脚本子命令，human 2026-10-27 立项 ack ①）。
// 覆盖形态（v1，判据交评审裁）：
//   - 围栏块：label 行（围栏开行之上最近的非空行）含「原文」= 原文引文（draft 期须在盘）；含「改后 / 替换后 /
//     插入 / 追加」= 改后文本（landing 期须在盘）；两族以 label 中**最后出现的 cue** 定性（「…原文…→ 替换后…」
//     单行混合式按后者走）。两相位各自把「不检」的块列进 notChecked——覆盖态如实申报，不静默略过。
//   - 行内引文：cue 之后最后一个冒号之后恰一个 code-span 且行尾无其它内容（如 原文（`:13`，逐字节）：`…`）——
//     draft 期查原文、landing 期查改后族。
//   - 目标解析走 targetCandidates：债务条目已归位 docs/debt/<域>/_archive/<年>/ 时按归档位补找（260924 批核验
//     教训：debt-render 归位后 manifest 旧路径失效，旧 advisory 在 P5 批报 3 条归档位预期 miss）。
// 不覆盖（输出 coverage / notChecked 如实申报）：blockquote 承载的插入段、多 span / 混合散文行内形态。
// 效力：只读自查，**不参与主门判定、不进任何闸链**；退出码自管（0 全命中 / 1 有未命中 / 2 用法或输入缺失）。
const CITE_ORIGINAL_CUE_RE = /原文/;
const CITE_RESULT_CUE_RE = /改后|替换后|插入|追加/;

// label 定性：取最后一个 cue（「替换前原文」只命中原文 → original；「…原文…→ 替换后…」→ result）。
function classifyCueLabel(label) {
  const hits = [...String(label).matchAll(/原文|改后|替换后|插入|追加/g)];
  if (!hits.length) return 'unlabeled';
  return hits[hits.length - 1][0] === '原文' ? 'original' : 'result';
}

// 围栏块的 label 行 = 围栏开行之上最近的非空行（索引相对传入的 §2 节域行数组）。
function labelLineForBlock(lines, openLine) {
  for (let i = openLine - 1; i >= 0; i--) {
    if (!lines[i].trim()) continue;
    return { index: i, text: lines[i].trim() };
  }
  return { index: -1, text: '' };
}

// 行内引文：cue 与候选用 code-span 遮罩后的文本定性（引文正文里的「原文」等字面不参与定性）——
// 候选 = 行内最后一个 code-span；要求 cue 之后、候选之前出现冒号（排除「原文（`grep -cF` = 1）：」这类
// 注记 span），且该区间内无第二个 cue 字面（排除「`a` → 替换后：`b`」混合式，交 unverified 不猜）。
function maskCodeSpans(label) {
  return String(label).replace(/`[^`\n]+`/g, (m) => ' '.repeat(m.length));
}

function inlineQuoteAfterCue(label, cueRe) {
  const masked = maskCodeSpans(label);
  const cue = cueRe.exec(masked);
  if (!cue) return null;
  const spans = [...label.matchAll(/`([^`\n]+)`/g)];
  if (!spans.length) return null;
  const last = spans[spans.length - 1];
  if (last.index <= cue.index + cue[0].length) return null;
  const between = masked.slice(cue.index + cue[0].length, last.index);
  if (!/[：:]/.test(between)) return null;
  if (/原文|改后|替换后|插入|追加/.test(between)) return null;
  if (!/^[\s，。；、）)】]*$/.test(masked.slice(last.index + last[0].length))) return null;
  return last[1];
}

// 从 PROPOSAL 收集本轮时点应检的引文（纯函数，供 --cite-check 与自检共用）。
// 返回 { quotes, notChecked, unverified }：quotes = 须逐个对账的引文；notChecked = 本相位不检的围栏块（含定性）；
// unverified = 定性歧义或行内形态无法无歧义提取的线索（不猜、不静默略过）。
function collectCiteQuotes(text, phase) {
  const lines = text.split(/\r?\n/);
  const start = lines.findIndex((l) => /^##\s*2[.、]/.test(l));
  if (start === -1) {
    return { quotes: [], notChecked: [], unverified: [{ line: 0, cue: '§2 改法节未找到', reason: 'no-section2' }] };
  }
  const flags = fenceFlags(lines);
  let end = lines.length;
  for (let i = start + 1; i < lines.length; i++) {
    if (flags[i]) continue;
    if (/^##\s/.test(lines[i])) { end = i; break; }
  }
  const secLines = lines.slice(start, end);
  const quotes = [];
  const notChecked = [];
  const unverified = [];
  const labelIdx = new Set();
  for (const b of extractFencedBlocks(secLines)) {
    const label = labelLineForBlock(secLines, b.openLine);
    if (label.index >= 0) labelIdx.add(label.index);
    if (!b.block.trim()) continue;
    const role = classifyCueLabel(label.text);
    const lineNo = start + b.openLine + 1;
    const cue = label.text.slice(0, 120);
    if (phase === 'draft') {
      if (role === 'original') quotes.push({ role, form: 'fence', line: lineNo, cue, text: b.block });
      else notChecked.push({ line: lineNo, role, form: 'fence', cue });
    } else if (role === 'original') {
      // landing 期：「原文」块落地后应已消失或被替换，不作判据（P5 批 3 条预期 miss 即此形态）。
      notChecked.push({ line: lineNo, role, form: 'fence', cue });
    } else {
      quotes.push({ role: role === 'unlabeled' ? 'result' : role, form: 'fence', line: lineNo, cue, text: b.block });
    }
  }
  // 行内引文：围栏外、且不是某围栏块 label 的行。draft 只查原文；landing 只查改后族。
  const cueRe = phase === 'draft' ? CITE_ORIGINAL_CUE_RE : CITE_RESULT_CUE_RE;
  for (let i = 0; i < secLines.length; i++) {
    if (flags[start + i] || labelIdx.has(i)) continue;
    const l = secLines[i];
    if (!cueRe.test(l)) continue;
    const q = inlineQuoteAfterCue(l, cueRe);
    if (q) {
      quotes.push({ role: phase === 'draft' ? 'original' : 'result', form: 'inline', line: start + i + 1, cue: l.trim().slice(0, 120), text: q });
    } else if (/原文[（(：:]|改后[（(：:]|替换后[（(：:]|插入[（(：:]|追加[（(：:]/.test(l) && l.includes('`')) {
      unverified.push({ line: start + i + 1, cue: l.trim().slice(0, 120), reason: 'inline-ambiguous' });
    }
  }
  return { quotes, notChecked, unverified };
}

function runCiteCheck(proposalPath, phaseArg) {
  const abs = resolve(projectDir, proposalPath);
  if (!existsSync(abs)) {
    print({ ok: false, gate: 'landing-cite-check', message: 'PROPOSAL 不存在', proposal: proposalPath });
    process.exit(2);
  }
  const text = readText(abs);
  const frozen = isFrozen(text);
  const phase = phaseArg || (frozen ? 'landing' : 'draft');
  if (phase !== 'draft' && phase !== 'landing') {
    print({ ok: false, gate: 'landing-cite-check', message: `--phase 仅接受 draft / landing（实得 ${phaseArg}）`, proposal: proposalPath });
    process.exit(2);
  }
  let targets;
  try {
    targets = extractTargets(text, { source: 'PROPOSAL.md' });
  } catch (error) {
    print({ ok: false, gate: 'landing-cite-check', message: error.message, proposal: proposalPath });
    process.exit(2);
  }
  const { quotes, notChecked, unverified } = collectCiteQuotes(text, phase);
  const checked = [];
  const missing = [];
  for (const q of quotes) {
    const hit = findQuoteInTargets(q.text, targets, projectDir);
    checked.push({ role: q.role, form: q.form, line: q.line, cue: q.cue, matched: hit });
    if (!hit) missing.push({ role: q.role, form: q.form, line: q.line, cue: q.cue, excerpt: q.text.trim().slice(0, 120) });
  }
  print({
    ok: missing.length === 0,
    gate: 'landing-cite-check',
    proposal: proposalPath,
    phase,
    frozen,
    targetCount: targets.length,
    checkedCount: checked.length,
    checked,
    missing,
    notChecked,
    unverified,
    coverage: {
      fenced: true,
      inline: true,
      blockquote: false,
      note: 'blockquote 承载的插入段与多 span / 混合散文行内形态不在 v1 覆盖面（见本批 PROPOSAL §6 留根）。',
    },
    note: '只读自查：引文 ↔ 目标文件逐字对账（双侧行尾归一 + trim）；不参与任何门判定、不进任何闸链。',
  });
  process.exit(missing.length === 0 ? 0 : 1);
}

// 引文自查自检（261027 批 T1）：覆盖 CLI 解析、draft / landing 两相位分列、行内形态、label 定性、归档位兜底。
// 判别力：反转 classifyCueLabel（原文 / 改后混淆）→ 两相位 quotes 角色断言变红；放宽行内判据 → 行内组变红；
// 去掉归档兜底 → 归档组变红。
function runCiteCheckSelfCheck() {
  let pass = 0;
  let fail = 0;
  const check = (label, got, want) => {
    if (got === want) { pass++; } else { fail++; console.error(JSON.stringify({ selfCheck: 'cite-check', label, got, want })); }
  };
  const c1 = parseCliArgs(['node', 'x', '--cite-check', 'p.md', '--phase', 'landing']);
  check('cli·citeCheck 取值', c1.citeCheck, 'p.md');
  check('cli·phase 取值', c1.phase, 'landing');
  check('cli·positional 为 null（不误触 reviewDir）', c1.positional, null);
  check('cli·baseRef 为 null', c1.baseRef, null);
  check('cli·phase 缺省为 null（按 Status 自动判）', parseCliArgs(['node', 'x', '--cite-check', 'p.md']).phase, null);
  const dir = mkdtempSync(join(tmpdir(), 'cite-'));
  const target = 'target.md';
  writeFileSync(join(dir, target), '首行\n原文甲：逐字在场。\n改后乙：落地在场。\n末行\n', 'utf8');
  const proposal = [
    '## 2. 改法（T1）',
    '### T1 · 甲',
    '原文（逐字节）：',
    '```text',
    '原文甲：逐字在场。',
    '```',
    '',
    '改后（逐字节）：',
    '```text',
    '改后乙：落地在场。',
    '```',
    '',
    '目标文件：`target.md`（改）',
    '## 3. 影响面',
  ].join('\n');
  const draftRes = collectCiteQuotes(proposal, 'draft');
  check('draft·只检原文块', JSON.stringify(draftRes.quotes.map((q) => q.role)), JSON.stringify(['original']));
  check('draft·改后块进 notChecked', draftRes.notChecked.length, 1);
  check('draft·原文命中', findQuoteInTargets(draftRes.quotes[0].text, [target], dir), target);
  const landingRes = collectCiteQuotes(proposal, 'landing');
  check('landing·只检改后块', JSON.stringify(landingRes.quotes.map((q) => q.role)), JSON.stringify(['result']));
  check('landing·原文块进 notChecked', landingRes.notChecked.length, 1);
  check('landing·改后命中', findQuoteInTargets(landingRes.quotes[0].text, [target], dir), target);
  const drift = proposal.replace('原文甲：逐字在场。', '原文甲：逐字在场！');
  check('draft·漂移未命中', findQuoteInTargets(collectCiteQuotes(drift, 'draft').quotes[0].text, [target], dir), null);
  const inlineProposal = ['## 2. 改法（T1）', '### T1 · 乙', '原文（`:13`，逐字节）：`原文甲：逐字在场。`', '目标文件：`target.md`（改）', '## 3. 影响面'].join('\n');
  const inlineRes = collectCiteQuotes(inlineProposal, 'draft');
  check('draft·行内原文入检', inlineRes.quotes.length === 1 && inlineRes.quotes[0].form === 'inline', true);
  check('draft·行内原文命中', findQuoteInTargets(inlineRes.quotes[0].text, [target], dir), target);
  const inlineMiss = inlineProposal.replace('逐字在场。', '逐字在场?');
  check('draft·行内漂移未命中', collectCiteQuotes(inlineMiss, 'draft').quotes.length === 1 && findQuoteInTargets(collectCiteQuotes(inlineMiss, 'draft').quotes[0].text, [target], dir) === null, true);
  const mixed = ['## 2. 改法（T1）', '### T1 · 丙', '原文（逐字）：`甲` → 替换后（逐字）：`乙`', '目标文件：`target.md`（改）', '## 3. 影响面'].join('\n');
  check('draft·混合行进 unverified（不猜）', collectCiteQuotes(mixed, 'draft').unverified.length, 1);
  check('label·替换前原文仍判原文', classifyCueLabel('替换前原文（逐字）：'), 'original');
  check('label·混合行取最后 cue', classifyCueLabel('原文（逐字）：`a` → 替换后（逐字）：`b`'), 'result');
  check('label·无 cue 判未标注', classifyCueLabel('全文逐字节如下：'), 'unlabeled');
  // 归档位兜底：目标不在 manifest 旧路径、在 docs/debt/gov/_archive/2026/ —— 命中；非债务路径不回退。
  mkdirSync(join(dir, 'docs', 'debt', 'gov', '_archive', '2026'), { recursive: true });
  writeFileSync(join(dir, 'docs', 'debt', 'gov', '_archive', '2026', 'ARQ-x.md'), '归档原文行。\n', 'utf8');
  check('归档兜底·命中', findQuoteInTargets('归档原文行。', ['docs/debt/gov/ARQ-x.md'], dir), 'docs/debt/gov/_archive/2026/ARQ-x.md');
  check('归档兜底·非债务路径不回退', findQuoteInTargets('归档原文行。', ['docs/other/ARQ-x.md'], dir), null);
  rmSync(dir, { recursive: true, force: true });
  console.log(JSON.stringify({ selfCheck: 'cite-check', pass, fail, total: pass + fail, ok: fail === 0 }));
  if (fail > 0) process.exit(1);
}

// 目录锚前缀展开兜底（261027 批 T3 · ARQ-260924 残余②，human 2026-10-27 裁「仍劝阻、展开作兜底」）：
// manifest 目录 token（以 / 结尾）不再按 changed.has 精确匹配判假 RED——展开为其下成员判定（任一成员有改动
// 即视为该行已落），展开明细进 dirAnchorExpansions；逐件枚举仍是正形（展开放宽「静默漏落」检测，只作兜底）。
// 纯文件 token 行为不变（changed.has 精确成员判定，与改前逐字节同）。
function computeNotLanded(targets, changed) {
  const notLanded = [];
  const dirExpansions = [];
  for (const t of targets) {
    if (String(t).endsWith('/')) {
      const matched = [...changed].filter((f) => f.startsWith(t)).length;
      if (matched === 0) notLanded.push(t);
      else dirExpansions.push({ token: t, matched });
      continue;
    }
    if (!changed.has(t)) notLanded.push(t);
  }
  return { notLanded, dirExpansions };
}

// 目录锚自检（261027 批 T3）：两向判别——目录锚样例展开后不假 RED（改前形态 = changed.has 精确匹配必判未落）；
// 纯文件路径行为与改前一致（命中不报 / 未落照报 / 零展开明细）。尾 / 是目录锚唯一形态判据（无 / 走文件 token）。
function runDirAnchorExpansionSelfCheck() {
  let pass = 0;
  let fail = 0;
  const check = (label, got, want) => {
    if (got === want) { pass++; } else { fail++; console.error(JSON.stringify({ selfCheck: 'dir-anchor-expansion', label, got, want })); }
  };
  const changed = new Set(['docs/crq/a.md', 'docs/crq/sub/b.md', '.claude/scripts/x.mjs']);
  const dirHit = computeNotLanded(['docs/crq/'], changed);
  check('目录锚·有成员改动不判未落（改前假 RED 形态）', JSON.stringify(dirHit.notLanded), JSON.stringify([]));
  check('目录锚·展开明细', JSON.stringify(dirHit.dirExpansions), JSON.stringify([{ token: 'docs/crq/', matched: 2 }]));
  const dirMiss = computeNotLanded(['docs/none/'], changed);
  check('目录锚·零成员改动仍判未落', JSON.stringify(dirMiss.notLanded), JSON.stringify(['docs/none/']));
  check('目录锚·零命中无展开明细', dirMiss.dirExpansions.length, 0);
  const files = computeNotLanded(['.claude/scripts/x.mjs', 'docs/crq/a.md', '.claude/scripts/y.mjs'], changed);
  check('纯文件路径·命中不报 / 未落照报（行为不变）', JSON.stringify(files.notLanded), JSON.stringify(['.claude/scripts/y.mjs']));
  check('纯文件路径·无展开明细', files.dirExpansions.length, 0);
  const noSlash = computeNotLanded(['docs/crq'], changed);
  check('尾斜杠唯一判据·无尾 / 按文件 token 处理', JSON.stringify(noSlash.notLanded), JSON.stringify(['docs/crq']));
  console.log(JSON.stringify({ selfCheck: 'dir-anchor-expansion', pass, fail, total: pass + fail, ok: fail === 0 }));
  if (fail > 0) process.exit(1);
}
```

4. **自检接线扩两组**（`:539-541` 整块替换；`runCiteCheckSelfCheck` 插在 `runPreflightSelfCheck` 后、`runDirAnchorExpansionSelfCheck` 插在 `runVerbatimQuotesSelfCheck` 后、终端式 `runIsFrozenSelfCheck` 仍列最后）：

原文（`:539-541`，逐字节）：
```js
// 自检接线顺序审查（真空绿规约）：runIsFrozenSelfCheck 为终端式（无条件 process.exit），必须列最后；
// 其余非终端式自检（含 B7 新增 section2Anchors、T2 新增 verbatim-quotes、260918 批新增 landing-preflight）排在其前，否则永不执行。
if (process.argv.includes('--self-check')) { runExtractTargetsSelfCheck(); runSection2AnchorSelfCheck(); runResolveOriginSelfCheck(); runComputeFrozenPinSelfCheck(); runCitationAnchorsSelfCheck(); runPreflightSelfCheck(); runLintDescriptionsTriggerSelfCheck(); runVerbatimQuotesSelfCheck(); runIsFrozenSelfCheck(); }
```

改后（逐字节）：
```js
// 自检接线顺序审查（真空绿规约）：runIsFrozenSelfCheck 为终端式（无条件 process.exit），必须列最后；
// 其余非终端式自检（含 B7 新增 section2Anchors、T2 新增 verbatim-quotes、260918 批 landing-preflight、
// 261027 批 cite-check / dir-anchor-expansion）排在其前，否则永不执行。
if (process.argv.includes('--self-check')) { runExtractTargetsSelfCheck(); runSection2AnchorSelfCheck(); runResolveOriginSelfCheck(); runComputeFrozenPinSelfCheck(); runCitationAnchorsSelfCheck(); runPreflightSelfCheck(); runCiteCheckSelfCheck(); runLintDescriptionsTriggerSelfCheck(); runVerbatimQuotesSelfCheck(); runDirAnchorExpansionSelfCheck(); runIsFrozenSelfCheck(); }
```

5. **子命令派发入口**（插入位置：`--preflight` 派发块收尾 `:549-551` 之后；与 preflight 同置于主流程之前，只读 exit 2 自管）：

插入位置锚（原文 `:549-551`，逐字节）：
```js
  }
  runPreflight(preflightArg);
}
```

插入段（逐字节，落地照抄）：
```js
// 引文自查入口（261027 批 T1 · ARQ-260914）：只读、不参与主门判定；与 --preflight 同置于主流程之前。
if (process.argv.indexOf('--cite-check') >= 0) {
  if (!citeCheckArg) {
    print({ ok: false, gate: 'landing-cite-check', message: '--cite-check 需要一个 PROPOSAL 路径参数' });
    process.exit(2);
  }
  runCiteCheck(citeCheckArg, citeCheckPhase);
}
```

6. **模板起草期自检句 ③ 扩写**（`PROPOSAL.template.md:69` 的行内插入；插在 `四实例在册）。④` 的 ④ 之前——`grep -cF "四实例在册）。④"` = 1，旧句逐字见下）：

原文（`:69` 行内片段，逐字节，`grep -cF` = 1）：
```text
四实例在册）。④
```

改后（逐字节，整段插入文本，④ 顺延）：
```markdown
 **正读数镜像**：工作树形态类断言（CRLF/LF、BOM、尾空白）同样须两个独立信号源一致方可写入正式制品（如 `git ls-files --eol` + CR 字节计数 `tr -dc '\r' | wc -c`），单源读数不得成文——msys（Git Bash）的 `awk` / `sed` 处理 CRLF 记录时会剥 `\r`，用它数裸 LF 会得假读数（批 B F2 实例：单源报裸 LF=75、实况六件纯 CRLF 全程未变；已知工具伪影即此一处、随句承载，不另立文件）。**机判形态**：`node .claude/scripts/landing-fidelity-check.mjs --cite-check <本 PROPOSAL 路径>` 一次跑完 §2 围栏块与行内原文引文（起草期查原文 vs 磁盘现文，FROZEN 后查改后文本 vs 落地文件；261027 批 T1）。
```

7. **`ARQ-260914` 部分结记账**（frontmatter 不动——条目维持 deferred；文末 append）：

文末尾行（原文 `:20`，逐字节）：
```text
**批 B retro §4 载体兑现追注（2026-09-24，260924-gov-review-settings-deny-contract-merge；gov-arq 查重结论 = §4-1/2/3/5 四条均与本条既有子面同根，按「同根追注、不新立」落本条，不新立条目）**：(1) **逐字性子面第二/三实例**——① R1-F1 转义伪影（该批 near-miss、最险一刻）：PROPOSAL 初稿把将 byte 照抄进 G3 门脚本（merge-driver）的 JS 源码行承载在 blockquote+code-span 内并以反斜杠+反引号转义——若未在 FROZEN 前抓出，landing 照抄即 SyntaxError（dev Pod 合并链全断），且 preflight 与结构门均不查引文逐字性、判官 byte 级比对是唯一网（判官 R1 记录 :309-311）；② 核验 F1 插入段漏 1 字符：SKILL.md:48 插入段「非 TTY 形态」落盘、FROZEN §2 基准为「非 TTY 两形态」（490 vs 489 codepoints，字符级 diff 三重机证）——批 A 教训 4「锚文机械提取 + 落笔前 byte 对账」只执行了提取半边、**插入文本侧落盘后对账缺位**。该子面累计三批四实例（批 A R1-F1/F2、批 B R1-F1、批 B 核验 F1），**够格机械化**。(2) **§4-1 机械化候选 = 承重引文 byte 级自证脚本化（起草侧 + landing 侧双钩）**：把判官 R1 与核验官各自现写的 node byte 比对脚本固化为可复用件（如 `landing-fidelity-check.mjs --cite-check` 或独立脚本）：起草期对 PROPOSAL 全部「锚定原文（逐字）」引文 vs 磁盘自跑；landing 后对全部插入/替换文本 vs FROZEN §2 自跑。批 B 实测代价面：判官 R1 六项 finding 中四项属「提交前可自证」类，自证到位估计可省 R2/R3 两轮（~455k tokens 报告口径）+ 核验一轮（~142k）。(3) **§4-2 同根改法候选 = PROPOSAL.template 读法注「围栏承载条款」**：含反引号/`${}`/转义序列的承重代码行禁用 blockquote+code-span 转义形态、必须围栏代码块承载（byte 保真 + 渲染稳；实例 = 上述 near-miss，读法注现文未覆盖此形态）。(4) **§4-3 双信号源规约 = 本条「工具负结果先交叉第二信号源」条款的正读数镜像形态**：工作树形态类断言（CRLF/LF、BOM、尾空白）须两个独立信号源一致方可写入正式制品（如 `git ls-files --eol` + `tr -dc` 字节计数）；实例 = 批 B F2——msys awk 对 CRLF 记录剥 `\r` 的工具行为致 bareLF=75 假读数、单源写入 summary §3.1 成披露不实（实况六件纯 CRLF 全程未变，verifier 复跑 census 证伪）；msys awk/sed 的剥 `\r` 行为列入已知工具伪影清单。(5) **§4-5 = ③ 连带项「评估时点」子面细则**：双时点标注的验证集须**逐子句注明归属时点**——批 B A2 与该子面第三实例同型（「render 复跑后 git diff --exit-code → 0」在 pre-commit 恒假），批 B 已于 commit 后复跑实证 exit 0（该批 retro §6-2）。**载体合批提示**：(2)(3)(5) 的改法候选落点均在 PROPOSAL.template 读法注/起草期自检面，与 ARQ-260924-landing-gate-dir-anchor-gap 候选 ③（manifest 纪律句）同文件面——未来批次可合并设计、同一文件面一次改齐（批 C 立项 ack 的收圈取舍面之一）。
```

append 段（逐字节，插在尾行之后、空一行）：
```markdown
**2026-10-27 · 部分结（`261027-gov-review-p6-landing-gate-cluster`，T1）**：本批消费机械化判据的两子项，条目维持 deferred（余项见末）——① **byte 比对脚本化**：`landing-fidelity-check.mjs` 新增 `--cite-check <PROPOSAL> [--phase draft|landing]` 只读子命令（载体按 human 2026-10-27 立项 ack ① 定 = 本脚本子命令、不立独立脚本）：起草期对 §2「原文」引文（围栏块 + 行内 code-span 两形态）对磁盘现文自跑；FROZEN 后对「改后 / 替换后 / 插入 / 追加」及未标注围栏块对落地文件自跑；label 按**最后出现的 cue** 定性、混合行入 unverified 不猜；目标解析含债务条目归档位兜底（`docs/debt/<域>/_archive/<年>/`）。判别力实测（起草期 2026-10-27）：对 `261026-gov-review-p2-anchor-template-cluster` 的 FROZEN PROPOSAL 复跑，旧观察列 24 条伪 missing（该批 landing-verdict ADVISORY-1 在案）降为 **1 条真偏差**——恰为该批 ADVISORY-2 记录的注释 byte 偏差（「行总数为奇数」落成「行为奇数」，`governance-gate-shared.mjs:380`），工具在历史批上自动复现了判官人读发现。② **双信号源规约（正读数镜像）**：`PROPOSAL.template.md` 起草期自检句 ③ 扩写——工作树形态类断言（CRLF/LF、BOM、尾空白）须两个独立信号源一致方可写入正式制品；msys awk/sed 剥 `\r` 的已知工具伪影随句成文（伪影清单不另立文件）。**仍留根**：③ 连带项非机械化半（§5「零命中 / 全 / 均」断言与 §2 / §4 保留面交叉对账）无排期；blockquote 承载的插入段与多 span 混合行内形态不在 `--cite-check` v1 覆盖面（见该批 PROPOSAL §6 留根）。
```

**为什么这样改**：对三个根因一一对应——① 对账能力下放到生产者：作者在 DRAFT 期跑一条命令即可自证全部原文引文（`--phase draft` 或自动按 Status 判），核验官在落地后跑 `--phase landing` 自证全部插入 / 替换文本；② 复用既有逐字判据而不是新造第二套：`findQuoteInTargets` 与主门 advisory 共用同一函数（行尾归一 + trim + includes 一字不改），新子命令只是在它外面加「相位 × 角色」两层过滤——原文块在 landing 期不作判据（落地后本应消失）、改后文本在起草期不作判据（尚未落地），这正是 P5 批 6 条观察列 miss 里 3 条的解释口径；③ 正读数成文：模板自检句 ③ 补「两个独立信号源一致方可成文」义务与 msys 伪影清单，把批 B F2 的教训从 retro 抬到起草 checklist。**为什么载体是子命令而非独立脚本**：human 拍定 ① 原话——判据、目标集、提取器都住在 `landing-fidelity-check.mjs`（manifest 提取、§2 切片、围栏提取皆在此），另立脚本要么复制这些要么反向依赖，二者都比子命令贵。**为什么 draft/landing 按 `isFrozen` 自动判又有显式覆盖**：默认零参数即可用；显式 `--phase` 给「FROZEN 前想预演 landing 侧」与「对历史 FROZEN PROPOSAL 做起草侧审计」留口，避免猜。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改） · `docs/templates/PROPOSAL.template.md`（改） · `docs/debt/gov/ARQ-260914-landing-preflight-anchor-parse.md`（改）

### T2 · 围栏提取独立成行（块内容中的 ``` 字面不再腰斩块）（ARQ-261026）

**问题**：`checkVerbatimQuotes` 的围栏提取正则把**任意位置**出现的 ``` 当块边界。凡 §2 的围栏块里出现 ``` 字面（js 夹具串的最常见形态：`` '/^\s*```/.test(l)' ``、`'```md'` 数组元素），块在首个行内 ``` 处被提前闭合：前半段成「截断块」、其后散文与剩余代码被当成新块逐个报 missing。观察列因此被伪 missing 稀释——261026 批实测 24 条 missing 里「大量为伪 missing」，每批要靠人逐条排除，观察列可信度下降（与已结的 ARQ-261019 假阴性同属「狼来了」族）。修门脚本的批**必写**含 ``` 的夹具（本批自身 §2 就是活例），故复发面稳定为每个门脚本批。

**证据**：§2.0 复证 2（`:472` 正则逐字在场；261026 landing-verdict ADVISORY-1 原文；相邻两形态已修不重开）；判别力实测（P2 FROZEN PROPOSAL 复跑：旧 24 条伪 missing → 新 1 条真偏差，§5 B7）。

**根因**：正则式提取器无法区分「块边界」与「块内容里的同形字面」——闭合判定缺少「独立成行」这一 Markdown 围栏语义约束。行内出现的 ``` 在 Markdown 里本就不是围栏（须行首、闭合行更只允许空白），提取器却按子串匹配。

**改法**（脚本两处 + ARQ 记账）：

1. **围栏提取改逐行状态机 + 逐字判据抽单源**（`:455-494` 整块替换；同块含 T1 的 `targetCandidates` / `findQuoteInTargets`——两 T 共用一次连续编辑，归属本 T 的提取器为主）：

原文（`:455-494`，逐字节）：
```js
// ── §2 钉死引号块 ↔ 落地文件机械逐字对账（260826-gov-review-g3-infra-convergence T2，advisory-only）──
// 提取 PROPOSAL §2 各 T 节内围栏代码块（```）正文（作者约定「原文钉死」最可机辨形态），逐块在 landing-manifest
// 目标文件集中做字节级 includes 对账；问题只入观察列、不产 fail（力度 = OQ-2 推荐 (a) advisory 起步）。
function checkVerbatimQuotes(proposalText, projectDir, targetFiles) {
  const checked = [];
  const missing = [];
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
  const fenceRe = /```[a-zA-Z]*\n([\s\S]*?)```/g;
  let m;
  while ((m = fenceRe.exec(secText)) !== null) {
    const block = m[1];
    if (!block.trim()) continue;
    let found = false;
    for (const target of targetFiles) {
      const abs = resolve(projectDir, target);
      if (!existsSync(abs)) continue;
      let content;
      try { content = readFileSync(abs, 'utf8'); } catch { continue; }
      // T2（260907 批）：双侧行尾归一后比对——仓内 .mjs 工作树全量 CRLF（autocrlf 实况）、PROPOSAL 经 Write 落盘 LF，
      // 逐字节 includes 必假（260906-gov-review-index-status-truth-source 批 2 条 advisory 假失配实证）。
      const norm = (s) => s.replace(/\r\n/g, '\n');
      // 261026 批 T2（ARQ-261019-verbatim-quote-line-level-false-negative）：围栏块正文恒带块尾换行，
      // 子句替换形态里目标句常嵌在长行中部、句后仍有接续文——比对前对块 trim()（首尾空白不进判据，正文逐字仍在）。
      if (norm(content).includes(norm(block).trim())) { found = true; break; }
    }
    checked.push(block);
    if (!found) missing.push({ block: block.trim().slice(0, 120), reason: 'not-found-verbatim-in-any-target' });
  }
  return { checked, missing };
}
```

改后（逐字节）：
```js
// ── 目标文件解析（261027 批 T1 · ARQ-260914）：manifest 路径 → 实际可读路径候选 ──
// 债务条目归位兜底：已结 debt 条目由 debt-render 移入 docs/debt/<域>/_archive/<年>/（位置 = status 的纯函数），
// 落地核验时 manifest 旧路径已不存在——按归档位补找一次；找不到仍按原路径返回（后续读失败自然判 missing）。
function targetCandidates(target, projectDir) {
  const exact = resolve(projectDir, target);
  if (existsSync(exact)) return [exact];
  const m = String(target).match(/^(docs\/debt\/[A-Za-z]+\/)([^/]+)$/);
  if (!m) return [exact];
  const archiveRoot = resolve(projectDir, `${m[1]}_archive`);
  let years = [];
  try { years = readdirSync(archiveRoot, { withFileTypes: true }).filter((e) => e.isDirectory()).map((e) => e.name); } catch { return [exact]; }
  const hits = years.map((y) => resolve(archiveRoot, y, m[2])).filter((p) => existsSync(p));
  return hits.length ? hits : [exact];
}

// ── 引文 ↔ 目标文件逐字对账单源（261027 批 T1）：既有 advisory 与本批 --cite-check 共用同一判据 ──
// T2（260907 批）：双侧行尾归一后比对——仓内 .mjs 工作树全量 CRLF（autocrlf 实况）、PROPOSAL 经 Write 落盘 LF，
// 逐字节 includes 必假（260906-gov-review-index-status-truth-source 批 2 条 advisory 假失配实证）。
// 261026 批 T2（ARQ-261019-verbatim-quote-line-level-false-negative）：围栏块正文恒带块尾换行，
// 子句替换形态里目标句常嵌在长行中部、句后仍有接续文——比对前对引文 trim()（首尾空白不进判据，正文逐字仍在）。
// 返回**实际命中的文件**（仓根相对路径，Windows 反斜杠归一为 /）——归档位兜底命中时读者能看见真实落点，未命中 = null。
function findQuoteInTargets(quote, targetFiles, projectDir) {
  const norm = (s) => s.replace(/\r\n/g, '\n');
  const needle = norm(quote).trim();
  if (!needle) return null;
  for (const target of targetFiles) {
    for (const abs of targetCandidates(target, projectDir)) {
      let content;
      try { content = readFileSync(abs, 'utf8'); } catch { continue; }
      if (norm(content).includes(needle)) return relative(projectDir, abs).split('\\').join('/');
    }
  }
  return null;
}

// ── §2 围栏块提取（261027 批 T2 · ARQ-261026-verbatim-extract-backtick-cut）──
// 旧实现 /```[a-zA-Z]*\n([\s\S]*?)```/g 把块内容中的 ``` 字面（js 夹具串形态，如 '/^\s*```/.test(l)'）
// 当块边界提前闭合——块被腰斩、其后散文被误当钉死块（261026 批 landing 观察列 24 条伪 missing 实证）。
// 新实现 = 逐行状态机：开行 = 行首可选空白 + ``` + 可选 info 串；闭行须**独立成行**（``` 后只允许空白）。
// 未闭合块不入列（围栏未闭合由 parseSection2AnchorLists fail-loud 点名，本检查不重复判）。
function extractFencedBlocks(lines) {
  const blocks = [];
  let open = null;
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    if (open) {
      if (/^\s*```\s*$/.test(line)) {
        blocks.push({ block: open.content.join('\n'), openLine: open.openLine, closeLine: i, info: open.info });
        open = null;
      } else {
        open.content.push(line);
      }
      continue;
    }
    const m = line.match(/^\s*```(.*)$/);
    if (m) open = { info: m[1].trim(), openLine: i, content: [] };
  }
  return blocks;
}

// ── §2 钉死引号块 ↔ 落地文件机械逐字对账（260826-gov-review-g3-infra-convergence T2，advisory-only）──
// 提取 PROPOSAL §2 各 T 节内围栏代码块正文（作者约定「原文钉死」最可机辨形态），逐块在 landing-manifest
// 目标文件集中做字节级 includes 对账；问题只入观察列、不产 fail（力度 = OQ-2 推荐 (a) advisory 起步）。
function checkVerbatimQuotes(proposalText, projectDir, targetFiles) {
  const checked = [];
  const missing = [];
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
  for (const { block } of extractFencedBlocks(vqLines.slice(vqStart, vqEnd))) {
    if (!block.trim()) continue;
    const hit = findQuoteInTargets(block, targetFiles, projectDir);
    checked.push(block);
    if (!hit) missing.push({ block: block.trim().slice(0, 120), reason: 'not-found-verbatim-in-any-target' });
  }
  return { checked, missing };
}
```

2. **verbatim 自检增 block 完整性夹具**（插入位置：`check('verbatim-fence-boundary-checked', …)` 行 `:531` 之后、`rmSync(clauseDir, …)` 之前）：

插入位置锚（原文 `:531`，逐字节）：
```js
  check('verbatim-fence-boundary-checked', checkVerbatimQuotes(boundaryProposal, fixtureDir, ['target.mjs']).checked.length, 2);
```

插入段（逐字节，落地照抄）：
```js
  // 261027 批 T2 夹具（ARQ-261026-verbatim-extract-backtick-cut）：块内容含行内 ``` 字面（js 夹具串形态），
  // 块须完整提取（未腰斩）、逐字对账仍命中。判别力：反转实现（闭合不要求独立成行）时块被切在首个行内 ``` 处，
  // checked[0] 变成截断串「const re = /^\s*」→ 本组 block-intact 断言变红（该截断串仍是目标文件子串，missing 不会变红，
  // 故判别以「块完整」断言为准——这是本条 bug 的真实形态：伪 missing 与错切散文并存，而非命中翻转）。
  const tickDir = mkdtempSync(join(tmpdir(), 'vq-backtick-'));
  const tickBlock = "const re = /^\\s*```/.test(l);\nconst arr = ['```md', 'x'];";
  writeFileSync(join(tickDir, 'f.mjs'), `head\n${tickBlock}\ntail\n`, 'utf8');
  const tickProposal = ['## 2. 改法', '### T1 · 反引号夹具', '```js', tickBlock, '```', '## 3. 影响面'].join('\n');
  const tickRes = checkVerbatimQuotes(tickProposal, tickDir, ['f.mjs']);
  check('verbatim-backtick-in-content-checked', tickRes.checked.length, 1);
  check('verbatim-backtick-in-content-block-intact', tickRes.checked[0] === tickBlock, true);
  check('verbatim-backtick-in-content-missing', tickRes.missing.length, 0);
  rmSync(tickDir, { recursive: true, force: true });
```

3. **`ARQ-261026` 结项记账**（frontmatter 三字段翻转 + 文末 append）：

frontmatter 原文三行逐条（各 `grep -cF` = 1；三行在文件 frontmatter 里不相邻，逐行引避免造假连续块）：

原文（逐字节）：
```yaml
status: deferred
```

原文（逐字节）：
```yaml
closed: ""
```

原文（逐字节）：
```yaml
closedBy: ""
```

改后：`status: closed` / `closed: 2026-10-27` / `closedBy: 261027-gov-review-p6-landing-gate-cluster`。

文末尾行（原文 `:24`，逐字节）：
```text
**关联**：261026 批已修的「块尾换行假阴性」（ARQ-261019-verbatim-quote-line-level-false-negative，已结）与「§2 切片跳围栏」（T1）同检查面；本条是其相邻新形态，登记时检查面刚被 261026 批动过，落点行号以动工时复测为准。
```

append 段（逐字节，插在尾行之后、空一行）：
```markdown
**2026-10-27 · 结项（`261027-gov-review-p6-landing-gate-cluster`，T2）**：`checkVerbatimQuotes` 的围栏提取改为逐行状态机（`extractFencedBlocks`）——开行 = 行首可选空白 + ``` + 可选 info 串，闭行须**独立成行**（``` 后只允许空白）；块内容中的行内 ``` 字面（js 夹具串形态，如 `'/^\s*```/.test(l)'`、`'```md'` 数组元素）不再作边界。自检新增 `verbatim-backtick-in-content-*` 三条（块完整提取 + 命中 + 无 missing），判别键 = 块完整断言（反转实现把块切在首个行内 ``` 处、checked[0] 变截断串即红——截断串仍是目标文件子串，missing 不翻转，故以块完整为准）。同批的 `--cite-check` 主门 advisory 共用新提取器。留根：`fenceFlags` / `parseSection2AnchorLists` 的 `fenceLineFlags` 仍用「行首 ``` 即翻转」的宽松态（只用于节域近似，未随本批改——若块内容出现**行首** ``` 仍可能错切节域，见该批 PROPOSAL §6 留根）。
```

**为什么这样改**：闭合围栏的 Markdown 语义就是「独立成行 + 后随只允许空白」，状态机按语义扫描即根除子串误判；夹具判别键刻意选**块完整断言**而非 missing 数——本 bug 的真实形态是「截断块仍是目标文件子串、missing 不翻转」，只有块完整性断言能在反转实现下变红（§5 B3 m1 实测）。**为什么不动 `fenceFlags` / `fenceLineFlags`**：它们只用于「§2 节域近似」与锚行跳围栏，宽松态带来的是保守方向的小概率边界误差，而改动要动 `governance-gate-shared.mjs` 的共享解析面、扩两倍评审面；本批按「扩承重闸优先、克制新立」只修真出事的提取器，余下形态写进 §6 留根 2。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改） · `docs/debt/gov/ARQ-261026-verbatim-extract-backtick-cut.md`（改）

### T3 · landing 门目录锚前缀展开兜底 + 口径改写（ARQ-260924 残余②，human 拍定「仍劝阻、展开作兜底」）

**问题**：landing 门的「已落地」判定是 `changed.has(t)` 逐文件精确成员匹配，而 preflight 的锚提取接受目录 token——同一份 manifest，起草期干跑绿、FROZEN 后首跑结构门 RED（260923 批 `docs/crq/` 实录：human 追认 + 机械展开 99 行才过门）。错误反馈延迟到 FROZEN 之后，此时修 manifest 已构成格式例外。凡迁移 / 新建目录类批次（本仓 260914 批 139 行、260923 批 99 行）都会遇到两个门的口径差，频率 = 每迁移批一次，成本 = 一次 FROZEN 后例外流程 + 判官 / 核验官各多核一轮。

**证据**：§2.0 复证 3（`:656`/`:658` 精确匹配、`:247-251` 与模板 `:23` 文案、两门机制面、口径改写必要性）；ARQ 条目 2026-09-24 / 09-25 两段实测复证（`ANCHORABLE_TEST` 前缀正则、advisory 落点与判别性）原样有效。

**根因**：两个门对「manifest 行」的语义假设不一致——preflight 把目录 token 当合法锚（前缀正则天然收），landing 门把 manifest 行当**文件集合成员**。差异在 FROZEN 两侧各验一半，谁都不错、合起来必炸；且地雷文案「目录简写收口期必红」把错误反馈留在最贵的时点。

**改法**（脚本四处 + 模板一处 + ARQ 一处；落法按 human 拍定 ②：「可用但劝阻、展开是兜底不是许可」，迁移批写作习惯不变）：

1. **目录锚判定收敛为 `computeNotLanded`（前缀展开）+ 自检**（`:655-658` 整块替换；claimed / planned 两侧同源调用，展开明细去重后进输出 `dirAnchorExpansions`；纯文件 token 仍 `changed.has` 精确判定）：

原文（`:655-658`，逐字节）：
```js
// 5. summary 声称落的 → git 里有改动。
const claimedNotLanded = [...summaryTargets].filter((t) => !changed.has(t));
// 6. PROPOSAL 清单 → git 里有改动（无静默漏落）。
const plannedNotLanded = [...proposalTargets].filter((t) => !changed.has(t));
```

改后（逐字节）：
```js
// 5. summary 声称落的 → git 里有改动（目录 token 按前缀展开兜底，261027 批 T3；纯文件路径行为不变）。
const summaryLanded = computeNotLanded([...summaryTargets], changed);
// 6. PROPOSAL 清单 → git 里有改动（无静默漏落）。
const proposalLanded = computeNotLanded([...proposalTargets], changed);
const claimedNotLanded = summaryLanded.notLanded;
const plannedNotLanded = proposalLanded.notLanded;
const dirAnchorExpansions = [...new Map([...summaryLanded.dirExpansions, ...proposalLanded.dirExpansions].map((x) => [x.token, x])).values()];
```

2. **失败输出携带展开明细**（`:675` 整行替换）：

原文（`:675`，逐字节）：
```js
  fail('governance landing 保真度核验未过', { reviewDir: relDir, errors, gitDegraded: degraded, hint });
```

改后（逐字节）：
```js
  fail('governance landing 保真度核验未过', { reviewDir: relDir, errors, gitDegraded: degraded, dirAnchorExpansions, hint });
```

3. **成功输出携带展开明细**（插入位置：`:764` `plannedNotLanded: …` 行之后）：

插入位置锚（原文 `:764`，逐字节）：
```js
  plannedNotLanded: degraded ? 'skipped-git-degraded' : [],
```

插入段（逐字节，落地照抄）：
```text
  dirAnchorExpansions,
```

4. **preflight advisory 文案改口径**（`:244-251` 整块替换；`ok` / exit / advisories 键位零变更）：

原文（`:244-251`，逐字节）：
```js
// T9①（260924-gov-review-gate-parse-teeth）：目录 token advisory——preflight 接受目录锚、
// landing 门逐文件精确匹配（changed.has）不消费目录锚（260923 批实录：`docs/crq/` FROZEN 后首跑结构门 RED
// 两条 → human 追认 + 机械展开 99 行）。本 advisory 把反馈前移回起草期；**不入 findings、不动 ok/exit**。
function dirTokenAdvisories(union) {
  return [...(union || [])]
    .filter((t) => String(t).endsWith('/'))
    .map((t) => `锚并集含目录 token「${t}」：landing 门不消费目录锚（逐文件精确匹配 changed.has），manifest 须逐件枚举成员（迁移批先例：260914 批 139 行全枚举）`);
}
```

改后（逐字节）：
```js
// T9①（260924-gov-review-gate-parse-teeth，261027 批 T3 改口径）：目录 token advisory——preflight 接受目录锚；
// landing 门自 261027 批起对目录 token 按前缀展开兜底（261027 批前 = 逐文件精确匹配 changed.has，260923 批
// `docs/crq/` FROZEN 后首跑结构门 RED 两条 → human 追认 + 机械展开 99 行即旧口径实录）。human 2026-10-27 裁：
// 目录锚**可用但劝阻、展开是兜底不是许可**——逐件枚举是正形（展开只验「有成员改动」，逐件漏落不再被点名）。
// 本 advisory 把口径与迁移批先例回起草期；**不入 findings、不动 ok/exit**。
function dirTokenAdvisories(union) {
  return [...(union || [])]
    .filter((t) => String(t).endsWith('/'))
    .map((t) => `锚并集含目录 token「${t}」：可用但劝阻——manifest 逐件枚举成员是正形；landing 门对目录 token 仅按前缀展开兜底（任一下辖文件有改动即视为该行已落，逐件漏落不再被点名；迁移批先例：260914 批 139 行全枚举）`);
}
```

5. **模板 manifest 说明行改口径**（`PROPOSAL.template.md:23` 整行替换；`grep -cF "目录须展开为成员枚举"` = 1）：

原文（`:23`，逐字节）：
```text
<一行一个目标路径；恰 = §2 各 T 目标文件锚清单并集（条件锚按裁断裁剪；迁移锚只记迁入路径）；一行一个**文件**路径——目录须展开为成员枚举（landing 门不消费目录锚，目录简写收口期必红）>
```

改后（逐字节）：
```text
<一行一个目标路径；恰 = §2 各 T 目标文件锚清单并集（条件锚按裁断裁剪；迁移锚只记迁入路径）；一行一个**文件**路径——逐件枚举是正形；目录简写可用但劝阻：landing 门对目录 token 仅按前缀展开兜底（任一下辖文件有改动即视为该行已落，逐件漏落不再被点名），展开是兜底不是许可（261027 批改口径；迁移批先例：260914 批 139 行全枚举）>
```

6. **`ARQ-260924` 结项记账**（frontmatter 三字段翻转 + 文末 append）：

frontmatter 原文三行逐条（各 `grep -cF` = 1；三行在文件 frontmatter 里不相邻，逐行引避免造假连续块）：

原文（逐字节）：
```yaml
status: deferred
```

原文（逐字节）：
```yaml
closed: ""
```

原文（逐字节）：
```yaml
closedBy: ""
```

改后：`status: closed` / `closed: 2026-10-27` / `closedBy: 261027-gov-review-p6-landing-gate-cluster`。

文末尾行（原文 `:28`，逐字节）：
```text
**部分承载（2026-09-25，260924-gov-review-gate-parse-teeth T9，本条不结）**：处方①③ 落地——preflight 输出增 `advisories` 键（`dirTokenAdvisories` 纯函数：锚并集中以 `/` 结尾的目录 token 逐个点名，文案含「landing 门逐文件精确匹配 changed.has、manifest 须逐件枚举成员」与 260914 批先例）；**ok/exit 零变更实证**：目录样例 ok:true / exit 0 / advisories 1，纯文件样例 advisories 0（判别性双向），landing-fidelity `--self-check` 19/19（含新增 2 判别 case）。PROPOSAL.template.md manifest 说明行补纪律句「一行一个**文件**路径——目录须展开为成员枚举（landing 门不消费目录锚，目录简写收口期必红）」（与批 B §4-2 围栏承载条款 / §4-5 逐子句时点 / ARQ-260914 追注 (B) 第三句同批合并设计、同一文件面一次改齐）。**处方② 维持不预批**（触承重闸判定逻辑，须独立批完整评审）。证据：本批 PROPOSAL §2 T9 + §5 V9 + landing-summary。
```

append 段（逐字节，插在尾行之后、空一行）：
```markdown
**2026-10-27 · 结项（`261027-gov-review-p6-landing-gate-cluster`，T3；human 2026-10-27 裁「仍劝阻、展开作兜底」）**：处方② 按裁落地——`landing-fidelity-check.mjs` 的 claimed / planned-but-not-landed 判定收敛为 `computeNotLanded`：manifest 目录 token（以 / 结尾）按前缀展开为成员判定（任一下辖文件有改动即视为该行已落），展开明细进输出 `dirAnchorExpansions`；纯文件 token 仍 `changed.has` 精确判定、行为逐字节不变。**口径随裁改写**：`dirTokenAdvisories`（preflight advisory）文案与 `PROPOSAL.template.md` manifest 说明行由「landing 门不消费目录锚、目录简写收口期必红」改为「可用但劝阻、逐件枚举是正形、展开是兜底不是许可」——本条原述「两门口径不一致」的病灶就此消解（preflight 与 landing 门同接受目录 token，landing 门按前缀展开、不再对目录 token 假 RED）；迁移批写作习惯不变（逐件枚举仍是正形，先例 260914 批 139 行全枚举）。落地后本条目由 debt-render 按 status 归位 `docs/debt/gov/_archive/2026/`。
```

**为什么这样改**：对根因——目录 token 的语义在门内显式化（前缀展开 = 「这一行断言该目录下有落地」），两门口径就此对齐；同时**不把展开升格为许可**：展开只回答「该目录下有成员动了」，不回答「清单成员逐个都落了」，逐件漏落检测因此从「逐行点名」降为「整目录一票」——这正是 human 拍「仍劝阻」的技术理由，故写入输出 `dirAnchorExpansions` 让核验者看得见哪些行走了兜底、各匹配了几个成员；模板句同步改为「逐件枚举是正形、可用但劝阻、展开是兜底不是许可」，迁移批先例（139 行全枚举）保留为写作范例。**为什么不展开 `unauthorized` 与 §2↔manifest 对账**：前者判「summary 是否越权」，目录简写与成员枚举本就是不同断言、展开会把授权检查变成集合近似（放宽授权面，不取）；后者是「同一 token 语义核对」，两侧同形写法才可判，展开会让对账失去对象。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改） · `docs/templates/PROPOSAL.template.md`（改） · `docs/debt/gov/ARQ-260924-landing-gate-dir-anchor-gap.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 / 消费方 |
|---|---|---|---|
| 强制执行层 G3 | `.claude/scripts/landing-fidelity-check.mjs` | 改（T1 ⓪-⑤ / T2 ①-② / T3 ①-④） | 新增 `--cite-check` 子命令（只读，不接闸链）；围栏提取改状态机；claimed/planned 判定收敛为 `computeNotLanded`（目录 token 前缀展开）并新增 `dirAnchorExpansions` 输出；三组自检（`cite-check` / `verbatim-quotes` 扩充 / `dir-anchor-expansion`）。消费方 = 主线程 landing 步骤、landing-verifier、本批后各治理批起草者与判官 |
| 治理制品骨架 G2 | `docs/templates/PROPOSAL.template.md` | 改（T1⑥ / T3⑤） | 起草期自检句 ③ 扩写（正读数镜像 + 伪影清单 + `--cite-check` 机判形态）；manifest 说明行改目录锚口径。消费方 = 后续全部 PROPOSAL 起草者与判官 |
| 账本 / 证据 G4 邻接 | `docs/debt/gov/ARQ-260914-landing-preflight-anchor-parse.md` · `docs/debt/gov/ARQ-261026-verbatim-extract-backtick-cut.md` · `docs/debt/gov/ARQ-260924-landing-gate-dir-anchor-gap.md` | 改（T1⑦ / T2③ / T3⑥） | 前两条 status 翻转（261026 / 260924 → closed）+ 三条 append 注记；落地后由 `debt-render.mjs` 自动归位 `docs/debt/gov/_archive/2026/`（生成动作，不进 manifest，见 §5 V7） |
| 生成件（不在 manifest） | `docs/debt/gov/INDEX.md` · `docs/debt/gov/_archive/2026/INDEX.md` · `delivery/review/INDEX.md` | 再生 | 前者由 `debt-render.mjs` 重渲染；后者由 `changes-index.mjs --upsert` 单行钉入（禁手改） |
| 指令根 G1 | `AGENTS.md` / `CLAUDE.md` / `docs/DEV-GOVERNANCE.md` | 未触 | 零 diff |
| 契约面 | `docs/contracts/**` | 未触 | 零 diff |
| 规范 spec | `docs/requirements.md` / `docs/architecture.md` | 未触 | 零 diff |
| 实现面 | `firmware/**` / `server/**` / `tools/**` | 未触 | 零 diff |
| 其余强制层 | `.claude/hooks/**` / `.claude/scope-map.json` / `*.settings.json` / `.claude/verify-gates.json` / `.claude/scripts/pre-merge-escalation-gate.mjs` / `.claude/scripts/lib/governance-gate-shared.mjs` | 未触 | 零 diff；无新增门、无门判定链变化（`--cite-check` 不接闸链；`fenceLineFlags` 不动） |

**【硬项 · sweep 透镜义务】新字面 / 新判定 pattern 全仓预跑（起草期 2026-10-27，命令 + 结果）**：

- **新字面碰撞基线**（`grep -rlF`，语料 = 全仓 `.md` / `.mjs` / `.json` / `.txt` / `.yaml` / `.yml`，单根 `.`，排除本批 `delivery/review/261027-gov-review-p6-landing-gate-cluster/`）：**机器标识符 15 项实测全 0**——`--cite-check`（全仓仅归档 retro 有「或独立脚本」的候选讨论，不含该旗标字面）/ `citeCheck` / `extractFencedBlocks` / `targetCandidates` / `findQuoteInTargets` / `collectCiteQuotes` / `classifyCueLabel` / `inlineQuoteAfterCue` / `maskCodeSpans` / `labelLineForBlock` / `computeNotLanded` / `dirAnchorExpansions` / `landing-cite-check` / `dir-anchor-expansion` / `verbatim-backtick-in-content`。**话术三项按豁免面申报、不作「= 0」断言（R1-F3 更正）**：`正读数镜像` = 4 文件，全部为豁免面且语义同向——术语出处 `delivery/archive/changes/arch/260924-gov-review-settings-deny-contract-merge/retro.md`、立项材料 `delivery/workstreams/gov-debt-reduction/task_plan.md`、处方源文 `docs/debt/gov/ARQ-260914-landing-preflight-anchor-parse.md`（追注 §4-3，正本批消费项）、生成件 `docs/debt/gov/INDEX.md`；`可用但劝阻` = 1、`展开是兜底不是许可` = 1（均 `task_plan.md` 立项记录）。三项是话术非标识符、本无碰撞语义，「正读数镜像」一词的出处恰是本批消费的 ARQ 追注——收窄为豁免面 + 出处注记（逐项数值见 §5 B8 复跑）。
- **新判定 pattern 全仓预跑**：`--cite-check` 对三个真实 PROPOSAL 实跑——本批 DRAFT（§5 B10）、P5 FROZEN（B6：checked 20 / missing 0 / notChecked 4 原文块）、P2 FROZEN（B7：checked 17 / missing 1 = 该批已知 ADVISORY-2 byte 偏差）。`computeNotLanded` 对真仓与四组夹具实跑（§5 B4/B5）：目录锚不假 RED、纯文件漏落照 RED。
- **`debt-render` 消费面**：三条目 status 翻转后 `--check` 应报待归位（本批落地前为干净基线，§5 B9）；渲染后自动归位；主 INDEX 与归档 INDEX 均为生成件，本批零手改。

**【硬项 · 复扫模式完备性先验】**：本批无目录搬家、无路径改落（`path-form-family.md` 形态族谱无消费面）。**退役语面一处**：两句「landing 门不消费目录锚 / 目录简写收口期必红」——模式集显式枚举全形态（词形 × 有/无 backtick × 文件面）：`目录须展开为成员枚举`、`不消费目录锚`、`目录简写收口期必红`、`逐件枚举`（保留词）。实跑定位（`grep -rn`，单根 `.`）：现行承载 = `PROPOSAL.template.md:23` + `landing-fidelity-check.mjs:245-250`（本批改）；其余命中为**豁免面**——本批 §2 自身的原文引文（围栏内）、ARQ-260924 条目正文与 `docs/debt/gov/INDEX.md`（条目标题/摘要，append-only 历史留痕，随结项归位）、归档批 PROPOSAL（`delivery/archive/**` 与 `delivery/review/261026-*` 历史制品，零改）。**「替换 / 插入型」文本共十四处**：脚本八处整块 / 整行替换（`:31` 单行、`:24-28` 块、`:53-60` 块、`:539-541` 块、`:455-494` 块、`:244-251` 块、`:655-658` 块、`:675` 单行）+ 脚本四处纯插入（`--cite-check` 实现段、派发段、verbatim 夹具、成功输出行）；文档两处（模板 `:23` 整行替换、模板 `:69` 子句插入）；另三条 ARQ append（纯 append，不动既有行）——全部以 §5 V6 的字面 grep 正反配对断言兜底。

## 4. human 取舍点（OQ）

**本批无待 human 拍板项**：两个方向性分叉已由 human 2026-10-27 两拍收敛（① `--cite-check` 载体 = 挂子命令；② 目录锚口径 = 仍劝阻、展开作兜底——决策原话见 `delivery/workstreams/gov-debt-reduction/task_plan.md` P6 节），起草按裁落地、不重开。以下为实现层技术取舍，判官可 challenge，任一被推翻都有明确退路：

- **取舍 A · `--cite-check` 覆盖面 = 围栏块 + 行内 code-span，不含 blockquote 插入段**：blockquote 承载的 prose 插入（如 260924 批 SKILL.md 插入段）在 Markdown 里没有可靠边界语义，机械提取会误报；v1 宁可如实申报不覆盖（输出 `coverage` + `notChecked`）。退路：判官若要求覆盖，须设计 blockquote 提取契约（引号前缀剥除规则 + 多段拼接规则），本批不做。复议触发见 §6 留根 1。
- **取舍 B · 角色定性用「label 中最后出现的 cue」**：`替换前原文（逐字）：` 只命中「原文」→ original；`原文（…）：`a` → 替换后（逐字）：`b`` 最后 cue = 替换后 → result；无法无歧义提取的行进 `unverified`（不猜）。退路：判官若认为应改「首个 cue」或强制 label 词表，本批的自检断言（`label·混合行取最后 cue`）随之改；代价是「替换前原文」形态会被误判为 result。
- **取舍 C · 目标集合 = manifest 目标文件（含债务归档位兜底）**：原文引文按定义来自被改文件，manifest 即被改文件集；归档位兜底治「落地后 debt-render 已归位、manifest 旧路径失效」的预期 miss（P5 批 observed 3 条），且命中路径**如实回报**（如 `docs/debt/gov/_archive/2026/ARQ-….md`）。退路：判官若认为兜底过宽，可撤 `targetCandidates` 的 `_archive` 分支——代价是每批 ARQ append 引文在落地后必报 missing、核验者需人工豁免。
- **取舍 D · `--cite-check` 不接主门、不进闸链**：它是生产者自查工具，主门既有 advisory（全块覆盖、advisory-only）不动；把两个口径混进一个出口会让「门的观察列」与「作者自检」互相污染。退路：判官若要求门内也按角色分列，须改主门 advisory 语义（本批不取，见 §6 留根 5）。
- **取舍 E · 目录锚展开只用于 claimed / planned 判定**：`unauthorized`（summary ⊆ proposal）与 §2↔manifest 对账仍精确 token 口径——它们判的是「断言是否同形 / 是否越权」，展开会让判据变成集合近似。退路：判官若要求三处一致，须先定义「目录 vs 枚举」的集合等价语义，属新机制，不在本批。
- **取舍 F · `dirAnchorExpansions` 按 token 去重**：summary 与 proposal 是同一条 token 时只报一条（member 数同源）；同 token 不同核不可发生（同一 changed 集）。退路：判官若要求两侧分列，可改输出键形状。
- **取舍 G · 提取器只管 `checkVerbatimQuotes` 线，不动 `fenceFlags` / `fenceLineFlags`**：见 T2「为什么这样改」（共享解析面扩评审不划算，余下形态留根）。
- **取舍 H · 伪影清单随模板句承载、不另立文件**：目前已知伪影只一条（msys awk/sed 剥 `\r`），立文件属过度建设；模板句同时是纪律载体与清单载体，读者在义务旁边就看到伪影。退路：伪影增至三条以上时改立 `.claude/references/` 小节（留根 6 的复议触发）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> **断言纪律（T 自守）**：脚本面预期值 = 「按本稿 §2 逐字节装进 pristine 副本」的实跑输出（装配副本在仓外 `C:/tmp/p6-assembly`，装配脚本 `_scratch/assemble-script.mjs` 从本稿 §2 的片段文件装配、不手抄，每处旧串先断言恰 1 次）；文档面预期值 = 仓外副本实跑（`_scratch/assemble-docs.mjs`，旧串 `split(old).length-1 === 1` fail-loud）。落地期实跑若与本表有字节差异，以落地实跑为准覆写 §5 并在 §7 留痕。
> **时点定义**：pre-commit = 落地文件在盘、未 commit 即可跑；post-commit = 须落地提交后才成立。
> **夹具配方**（任何人可重建）：① 判别夹具 = `_scratch/verify-fixtures.mjs`（起草结束删除；断言已固化进脚本自检，永久载体 = B2/V1）；② 端到端夹具 = 临时 git 仓 + FROZEN PROPOSAL/summary 最小件（正文见 B5 说明）；③ 引文夹具 = 临时目录三文件（target.md + DRAFT/FROZEN PROPOSAL）。

**基线（现行 HEAD，起草期 2026-10-27 实跑）**：

| # | 命令 | 实跑输出（逐字 / 摘要） |
|---|---|---|
| B1 | `node .claude/scripts/landing-fidelity-check.mjs --self-check`（**现行脚本**） | 10 组全绿：`changeid-prefix 4` / `extractTargets 6` / `section2Anchors 29` / `resolveOrigin 29` / `computeFrozenPin 4` / `citation-anchors 19` / `landing-preflight 20` / `lint-descriptions-trigger 2` / `verbatim-quotes 10` / `isFrozen 19`，exit 0 |
| B2 | 装配副本 `--self-check` | **12 组**全绿：`cite-check 21` / `verbatim-quotes 13` / `dir-anchor-expansion 7`，其余九组与 B1 逐值同；exit 0 |
| B3 | `_scratch/verify-fixtures.mjs`（判别 + 端到端，18 断言） | **18/18**：m1（闭合不要求独立成行）→ `verbatim-backtick-in-content-block-intact` 红；m2（cue 取首个）→ `label·混合行取最后 cue` 红；m3（撤归档兜底）→ `归档兜底·命中` 红；m4（目录锚不展开）→ `目录锚·有成员改动不判未落` 红；其余 14 条全绿 |
| B4 | 端到端 · 目录锚（临时 git 仓，FROZEN PROPOSAL 含目录 manifest） | `dir-anchor-hit`：`ok:true` + `dirAnchorExpansions:[{"token":"docs/crq/","matched":1}]`；`dir-anchor-miss`：RED 且 errors 点名 `docs/none/`；`file-token-unchanged`：RED 且点名 `.claude/scripts/y.mjs`、expansions `[]`；`file-token-hit`：`ok:true`、expansions `[]` |
| B5 | 端到端 · 引文自查（临时目录） | draft 自动判：`phase:draft`、`checkedCount:2`（围栏原文 + 行内原文）、`notChecked:1`（改后块）、exit 0；漂移版 exit 1、missing 2；FROZEN 自动判 landing：`phase:landing`、只检改后块 1 条、exit 0；PROPOSAL 不存在 → exit 2；非法 `--phase` → exit 2 |
| B6 | 真仓 · `--cite-check delivery/review/261027-gov-review-p5-spike-anchor-cluster/PROPOSAL.md` | `ok:true`、`phase:landing`、targetCount 13、**checked 20 / missing 0** / notChecked 4（原文块 `:238`/`:249`/`:285`/`:386`）/ unverified 0；三条 ARQ append 块经归档位兜底命中且**命中路径如实回报**（`:170` → `docs/debt/gov/_archive/2026/ARQ-261024-new-mechanism-spike-first.md`；`:433` → `…/ARQ-261017-anchor-check-master-unvalidated.md`；`:454` → `…/ARQ-260908-versioned-guard-repoint-execution.md`）。**旧观察列同批 6 条 missing（P5 landing-verdict 在案）→ 新工具 0** |
| B7 | 真仓 · `--cite-check delivery/review/261026-gov-review-p2-anchor-template-cluster/PROPOSAL.md` | `ok:false`、checked 17 / **missing 1** = `:112` 块（`governance-gate-shared.mjs:380` 已知 byte 偏差，ADVISORY-2），notChecked 8 / unverified 0。**旧观察列同批 24 条伪 missing（ADVISORY-1）→ 新工具 1 条真偏差** |
| B8 | 新字面碰撞 census（`grep -rlF`，排除本批目录） | 机器标识符 15 项（`--cite-check` / `citeCheck` / `extractFencedBlocks` / `targetCandidates` / `findQuoteInTargets` / `collectCiteQuotes` / `classifyCueLabel` / `inlineQuoteAfterCue` / `maskCodeSpans` / `labelLineForBlock` / `computeNotLanded` / `dirAnchorExpansions` / `landing-cite-check` / `dir-anchor-expansion` / `verbatim-backtick-in-content`）**全 0 文件**；话术三项为豁免面（R1-F3 更正）：`正读数镜像` 4 文件（归档 retro 术语出处 / task_plan 立项材料 / ARQ-260914 处方源文追注 / debt INDEX 生成件）、`可用但劝阻` 1（task_plan）、`展开是兜底不是许可` 1（task_plan） |
| B9 | `node .claude/scripts/debt-render.mjs --check`；`… --self-check` | `--check` exit 0（基线干净）；`--self-check` 见 V7 复跑 |
| B10 | 装配副本 `--cite-check delivery/review/261027-gov-review-p6-landing-gate-cluster/PROPOSAL.md`（**本稿 DRAFT**） | `ok:true`、`phase:draft`、**checked 23 / missing 0 / notChecked 17 / unverified 0**（23 = 原文切片 16〔脚本 12 + 模板 1 + ARQ 末行 3〕+ frontmatter 单行 6 + 模板 `:69` 片段 1） |
| B10b | 装配副本 + **post-landing 沙盘**（仓外 `C:/tmp/p6-post`：装配版脚本 + 本批文档副本 + 261026/260924 已按 debt-render 归位到 `_archive/2026/`），`--phase landing` 跑本稿 | `ok:true`、**checked 19 / missing 0 / notChecked 23 / unverified 0**；`_archive/2026/` 路径如实回报（`:695`/`:825`/`:833` → 260924 归档件、`:703` → 261026 归档 append）——V2 的终态预演 |
| B11 | 装配副本 `--preflight` 本稿 | `ok:true`、`anchorCount:5`、anchorUnion 与 manifest 五行逐条一致、`findings:[]`、`advisories:[]` |
| B12 | 行尾 / 语法 | 装配副本 **1129 行**、`node --check` 通过；五件目标文件 `git ls-files --eol` 均 `i/lf w/lf`（本 worktree LF，落地保持） |

**落地验证集**：

| # | 时点 | 命令 | 预期（实跑回填 / 正推） | 判别力 / 配对 |
|---|---|---|---|---|
| V1 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --self-check`（落地后脚本） | 与 B2 **逐值相同**（12 组、`cite-check 21` / `verbatim-quotes 13` / `dir-anchor-expansion 7`，exit 0） | 真空绿：自检输出含三个新增自检名；调用顺序 `runIsFrozenSelfCheck` 仍最后。判别力由 B3 变异实测背书（四处反转各自变红） |
| V2 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --cite-check delivery/review/261027-gov-review-p6-landing-gate-cluster/PROPOSAL.md`（落地后脚本 + 本稿） | `phase:landing`（本稿届时 FROZEN）、`missing:[]`；原文块进 `notChecked`；改后 / 插入段全部命中（含三条 ARQ append——届时 260924 / 261026 已归位 `_archive/2026/`，由归档位兜底命中并如实回报路径；起草期沙盘预演 = B10b：checked 19 / missing 0） | 负向：`missing` 为空 × 正向：`checked` 非空且逐条 `matched != null`。若文本有任一字符漂移，missing 非空即红 |
| V3 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261027-gov-review-p6-landing-gate-cluster/PROPOSAL.md` | `ok:true`、`anchorCount:5`、anchorUnion = manifest 五行、`findings:[]`、`advisories:[]`、exit 0 | 形态门：五条锚行逐 token 对照 manifest；`advisories:[]` 证明无目录锚混入（本批 manifest 全文件路径） |
| V4 | pre-commit | `git diff --name-only -- docs/contracts/ docs/requirements.md docs/architecture.md server/ firmware/ tools/ .claude/hooks/ .claude/scope-map.json .claude/*.settings.json .claude/verify-gates.json .claude/scripts/pre-merge-escalation-gate.mjs .claude/scripts/lib/ AGENTS.md CLAUDE.md docs/DEV-GOVERNANCE.md` | 空输出 | 负向（禁触面零 diff）；正向配对 = `git diff --name-only`（debt-render 之前）恰等于 manifest 五行。**并发改动口径**：若落地时同工作树另有主线程账本登记或生成件在途（如起草期已见的债务条目登记 + `docs/debt/gov/INDEX.md` 重渲染），逐件核性质后如实记入 landing-summary 的 `changedCanonicalNotInProposal` 说明——不是静默豁免、不并入本批 manifest |
| V5 | pre-commit | `ls delivery/review/261027-gov-review-p6-landing-gate-cluster/_scratch/` | 目录不存在（起草期脚手架已删） | 负向（无残留）+ 正向配对 V4 的「diff 恰等于 manifest」会机械拦住漏删 |
| V6 | pre-commit | 文档字面正推（`grep -cF`，文件 = manifest 五件） | `正读数镜像`：template = 1；`可用但劝阻`：template = 1、脚本 = 2（T3④ 函数注释 + advisory 文案）；`目录须展开为成员枚举`：template = **0**；`目录简写收口期必红`：template = **0**；`landing 门不消费目录锚`：脚本 = **0**；`--cite-check`：脚本 ≥ 3（头注、CLI、派发、输出 gate 名）；三条 ARQ：`status: deferred` 261026/260924 各 = **0**、`status: closed` 各 = 1、`closed: 2026-10-27` 各 = 1、`closedBy: 261027-gov-review-p6-landing-gate-cluster` 各 = 1；ARQ-260914 `status: deferred` = 1（维持） | 每条负向（旧句 = 0）配正向（新句 ≥ 1）；frontmatter 正反配对防「只 append 不翻转」。**frontmatter 字面形态与 T2③ / T3⑥ 改后行逐字对齐（`closed: 2026-10-27`，冒号后空格、无引号；R1-F1 更正——v0.1 曾误写 `closed:"2026-10-27"`）** |
| V7 | post-commit | `node .claude/scripts/debt-render.mjs`（渲染归位）→ `--check` → `--self-check` | 261026 / 260924 落 `docs/debt/gov/_archive/2026/`、主 INDEX 转已结指针、归档 INDEX 出现薄行；`--check` exit 0；`--self-check: ok` | 归位是 status 的纯函数；V2 的归档位兜底在归位后复跑仍 `missing:[]`（兜底生效的终态证明） |
| V8 | post-commit | `node .claude/scripts/pre-merge-escalation-gate.mjs 261027-gov-review-p6-landing-gate-cluster --source-ref HEAD` | `PASS (…, kind=gov)`，exit 0 | **未实测**（依赖 FROZEN + landing-verdict）；本批无契约 diff |
| V9 | pre-commit（主线程 chore） | `node .claude/scripts/changes-index.mjs --upsert 261027-gov-review-p6-landing-gate-cluster` | `delivery/review/INDEX.md` 出现该 id 行、state 缓存同步；须在落地 commit 之前跑 | 生成件，禁手改 |

## 6. 留根清单

**本批结项（landing 后由主线程执行登记 / 状态变更；architect 不做）**：`ARQ-261026-verbatim-extract-backtick-cut`（整条结项）、`ARQ-260924-landing-gate-dir-anchor-gap`（整条结项）；`ARQ-260914-landing-preflight-anchor-parse` **部分结、维持 deferred**（见留根 3）。

**留根与复议触发**：

1. **`--cite-check` v1 覆盖面不含 blockquote 插入段与多 span 混合行内形态**：两份不覆盖都如实申报在输出 `coverage` / `unverified` 里（不静默略过）。复议触发 = 出现第一例「blockquote 承载的插入段 byte 漂移逃过起草自检与核验」（任何角色发现），届时按 §4 取舍 A 设计 blockquote 提取契约。
2. **`fenceFlags` / `fenceLineFlags` 的「行首 ``` 即翻转」宽松态未改**：只影响 §2 节域近似与锚行跳围栏，本批提取器已不依赖它做块边界。复议触发 = 首次出现「块内容含行首 ``` 导致节域错切」实例。
3. **`ARQ-260914` ③ 连带项非机械化半（§5「零命中 / 全 / 均」断言与 §2 / §4 保留面交叉对账）仍待排期**：机械化判据两子项已由本批落地，余项无机械载体；条目维持 deferred——**frontmatter 按 T1⑦「frontmatter 不动」条款维持现文**（其中 trigger 的「剩余（机械化判据）无排期」等字面为开具时点状态；部分结事实由文末 append 承载，ARQ 消费侧有「处方须实测复证」纪律兜底；R1-F4 的口径折中）。复议触发 = 该形态第四实例。
4. **`unauthorized` 与 §2↔manifest 对账仍精确 token 口径**：目录简写在 summary / proposal 两侧不同形时仍 RED（政策一致，见 T3「为什么这样改」）。复议触发 = 出现「两侧同形目录简写仍被拒」实例。
5. **`--cite-check` 未接任何闸链**：复议触发 = 连续两批出现「本可被 `--cite-check` 拦住的自证类 finding」，届时评估挂进 landing 门或 dev-loop verify 面（须另立批 + 全强度评审）。
6. **工具伪影清单只此一条（msys awk/sed 剥 `\r`）且随模板句承载**：复议触发 = 出现第二个已知工具伪影（届时改立 `.claude/references/` 小节，见 §4 取舍 H）。
7. **归档位兜底只覆盖 `docs/debt/<域>/_archive/<年>/<同名文件>`**：债条目未来若改归位形态（改名 / 分层），`targetCandidates` 需同步；复议触发 = debt-render 归位规则变更批。
8. **生成件不进 manifest**：`docs/debt/gov/INDEX.md` / `_archive/2026/INDEX.md` / `delivery/review/INDEX.md` 均为再生件（V7/V9），landing-verifier 按最终路径核内容。

**环内自律**：起草期脚手架全在 `delivery/review/261027-gov-review-p6-landing-gate-cluster/_scratch/`（装配、夹具、片段文件），落地 commit 前删除、不随批 commit（V5 正向断言机械兜底）；仓外装配副本在 `C:/tmp/p6-assembly` / `C:/tmp/p6-docs`，不进仓。

## 起草期自检留痕（模板四句自检的落地记录，2026-10-27）

1. **preflight 实跑**（只读干跑，V3 预跑）：见本节末「preflight 输出」块。
2. **路径形态族谱**（`path-form-family.md`）：本批无新增路径、无搬家、无改名；新增符号（`--cite-check` / `--phase` / `extractFencedBlocks` / `targetCandidates` / `findQuoteInTargets` / `collectCiteQuotes` / `classifyCueLabel` / `inlineQuoteAfterCue` / `maskCodeSpans` / `labelLineForBlock` / `computeNotLanded` / `dirAnchorExpansions` / `landing-cite-check`）、自检组名（`cite-check` / `dir-anchor-expansion` / `verbatim-backtick-in-content-*`）与话术（`正读数镜像` / `可用但劝阻` / `展开是兜底不是许可`）已按 §3 硬项做碰撞 census（B8：机器标识符 15 项全 0；话术三项按豁免面申报——出处 / 立项材料 / 生成件，见 §3）。
3. **承重引文 byte 对账**：§2 全部原文引文由 `_scratch/inject-slices.mjs` 从磁盘**机械切片**注入（`<<<SLICE:path:from:to>>>` 标记逐行替换为文件对应行，无手抄）；改后 / 插入文本由 `<<<PIECE:…>>>` 从片段文件注入（同一份片段被装配脚本消费，PROPOSAL ↔ 装配产物同源）；装配脚本对每处旧串先断言恰 1 次（B12 前身）。**未手抄长输出**。
4. **长文落地自检**：本文件写入与注入后机检 = `wc -l` = **1035**（v0.2 收口时点，含 §7 R1 / R2 判官记录）、U+FFFD 替换字符扫描零命中、`### T<n>` 标题计数 = 3、`目标文件：` 锚行计数 = 3、注入标记零残留（整行形态计 0）、围栏定界行配平（偶数）、manifest 围栏块恰 5 行——以上由 `_scratch/check-consistency.mjs` 结构化复跑（40/40）。

**preflight 输出**：见下方（落地期复跑同值）。

```
$ node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261027-gov-review-p6-landing-gate-cluster/PROPOSAL.md
{
  "ok": true,
  "gate": "landing-preflight",
  "proposal": "delivery/review/261027-gov-review-p6-landing-gate-cluster/PROPOSAL.md",
  "section2BodyFound": true,
  "anchorCount": 5,
  "anchorUnion": [
    ".claude/scripts/landing-fidelity-check.mjs",
    "docs/debt/gov/ARQ-260914-landing-preflight-anchor-parse.md",
    "docs/debt/gov/ARQ-260924-landing-gate-dir-anchor-gap.md",
    "docs/debt/gov/ARQ-261026-verbatim-extract-backtick-cut.md",
    "docs/templates/PROPOSAL.template.md"
  ],
  "advisories": [],
  "findings": [],
  "note": "只读干跑：只保证 §2 锚清单「形态可解析」，不做 §2↔manifest 对账；不参与任何门判定。"
}
exit=0
```

**起草期 `--cite-check` 实跑（本工具自身 dogfood，§5 B10 / B10b）**：`phase:draft` → `checked 23 / missing 0 / notChecked 17 / unverified 0`；post-landing 沙盘 `--phase landing` → `checked 19 / missing 0 / notChecked 23 / unverified 0`（归档位命中路径如实回报）。**一致性核验**（`_scratch/check-consistency.mjs`）：片段 17 件 + 切片 16 处逐字节在场、结构自检全过，**40/40**。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-27 - architect-reviewer

**R1 全面审（四查 + 判档 + 验证集可执行性 + 实跑抽查）。** 一切事实断言亲核实测，留痕见末。

**四查结论**：

1. **意图保真 · 过**。human 立项 ack 两拍（`delivery/workstreams/gov-debt-reduction/task_plan.md:90` 原文亲读）逐字落实：拍① 载体 = `landing-fidelity-check.mjs --cite-check` 子命令、否独立脚本——T1 全部改法落在既有脚本六处 + 模板一处 + ARQ 一处，manifest 无新脚本行；拍② 目录锚「仍劝阻、展开作兜底」在 T3④ advisory 文案、T3⑤ 模板纪律句、T1① 头注三处口径一致（「可用但劝阻、逐件枚举是正形、展开是兜底不是许可」逐字在改后文本），迁移批写作习惯未动。三条 ARQ 处方承接与残余行精确吻合（260914 两子项落、余项留根维持 deferred；261026 / 260924 整条结）。
2. **纪律合规 · 过（门语义面重点核）**。T3 对承重闸 claimed/planned 判定语义的变更 = human 拍② 授权的变更本身、非文本漂移；**「纯文件 token 行为逐字节不变」断言亲核成立**——`computeNotLanded` 对无尾 `/` token 走与改前相同的 `changed.has` 精确分支，自检「纯文件路径·命中不报 / 未落照报（行为不变）」+ 端到端 file-token-hit / file-token-unchanged 双向覆盖（B3 实跑过）。`--cite-check` 不接主门不进闸链核实：派发段独立于主流程、exit 0/1/2 自管、主门 `print` / `fail` 调用点除 T3②/T3③ 加输出键外零改动。写权边界：`_scratch/` 22 件为起草期脚手架、不在 manifest、V5 断言落地前删除——口径自洽；本评审只写本 §7。
3. **撞车复核 · 过**。T1 复用既有逐字判据（行尾归一 + trim）未造第二套：主门 advisory 与新子命令在 T2① 改后共用 `findQuoteInTargets` 单源；261026 批已修两形态（`:461-471` 切片跳围栏 / `:486-488` 块尾 trim）在改后块逐行保留；旧正则闭合模式 = 裸 ```（无独立成行约束）亲读 `:472` 核实——T2 问题陈述与 m1 变异（闭合改「含 ``` 即闭合」）机制吻合，截断串「const re = /^\s*」与夹具注释一致。P5 批 spec-anchor-check.mjs 异脚本零交集。新符号全仓碰撞 = 0（机器标识符类亲测，见 F3 中保留项）。
4. **牵连面完整 · 过（F3 除外）**。退役语面两句定位准确（模板 `:23` + 脚本 `:245-250`），豁免面列举成立（归档批制品 / ARQ 正文 / debt INDEX 生成件）；三条 ARQ 的文末尾行引文（`:20` / `:24` / `:28`）与磁盘现文逐字核对一致；frontmatter 三行引文逐条在场；判别夹具四变异 m1–m4 实跑 18/18 各自变红。并发改动隔离：当前在途集 = `task_plan.md` / `docs/debt/gov/INDEX.md` / `delivery/review/INDEX.md` + state 缓存 / ARQ-261027 登记 / `.codebase-memory` 两件——V4 口径句（「逐件核性质后如实记入 landing-summary 的 changedCanonicalNotInProposal 说明——不是静默豁免、不并入本批 manifest」）可处理全部在途项，口径成立。

**判档结论**：**Track B 主路径成立、只升不降核对无升**。diff 触 `.claude/scripts/**` → gov-change.md 轻路径判据③排除面命中、恒 Track B；riskTier medium / profile G2+G3 与 §3 实况相符（G1 / 契约面 / hooks / scope-map / settings / verify-gates 零触碰亲核 manifest 与 §3 表一致）；Track A 段两条判据均不命中（非跨系统结构性、方向由 human 两拍收敛）；T3 触承重闸但方向清晰 → Track B + 批内独立 T 全强度评审（本评审已按全强度对待）。无升级 human 项。

**findings**：

- **F1 · BLOCKING · V6 断言字面与改后 frontmatter 实际形态不匹配**：V6 写 `closed:"2026-10-27"` 各 = 1，而 T2③ / T3⑥ 改后明确 `closed: 2026-10-27`（冒号后空格、无引号）——`grep -cF 'closed:"2026-10-27"'` 对落地文件恒得 0 ≠ 预期 1，验证集按冻结文本不可执行（恰属 ARQ-261021「预期值现跑可判」要防的形态）。修法：V6 该字面改为 `closed: 2026-10-27`（与 T2③/T3⑥ 改后行逐字对齐）；`closedBy` 字面已核对无误。
- **F2 · SHOULD · 复证 4 装配副本行数为陈旧数字**：§2.0 复证 4 写「按本批 §2 装配后 **1121** 行」，实测装配副本（`C:/tmp/p6-post/.claude/scripts/landing-fidelity-check.mjs`）`wc -l` = **1129**，与 §5 B12「1129 行」同稿互斥。修法：复证 4 更正为 1129（自检组数 12 / 21 / 13 / 7 亲测无误、不必动）。
- **F3 · SHOULD · B8/§3 新字面 census 对话术短语的「= 0」断言与实测不符**：按 §3 声明语料（排除本批目录）实测——`正读数镜像` = **4 文件**（`delivery/archive/changes/arch/260924-gov-review-settings-deny-contract-merge/retro.md`＝术语出处、`delivery/workstreams/gov-debt-reduction/task_plan.md`＝立项材料、`docs/debt/gov/ARQ-260914-landing-preflight-anchor-parse.md`＝处方源文追注 (4)、`docs/debt/gov/INDEX.md`＝生成件）、`可用但劝阻` = **1**（task_plan.md）、`展开是兜底不是许可` = **1**（task_plan.md）。三者为话术非标识符、本无碰撞语义，且「正读数镜像」一词的出处恰是本批消费的 ARQ 追注——按「= 0」申报既不实也无意义。机器标识符类逐项实测 = 0 成立（`--cite-check` / `citeCheck` / `extractFencedBlocks` / `targetCandidates` / `findQuoteInTargets` / `collectCiteQuotes` / `classifyCueLabel` / `inlineQuoteAfterCue` / `maskCodeSpans` / `labelLineForBlock` / `computeNotLanded` / `dirAnchorExpansions` / `landing-cite-check` / `dir-anchor-expansion` / `verbatim-backtick-in-content`）。修法：话术短语三项改「豁免面 + 出处注记」或从 census 断言中移出、收窄到机器标识符。
- **F4 · NIT · 留根 3 与 T1⑦ 对 ARQ-260914 frontmatter 的口径不一**：留根 3 写「条目维持 deferred，trigger 已如实追加」，T1⑦ 写「frontmatter 不动——条目维持 deferred」——landing 后 trigger 字段仍写「剩余（机械化判据）无排期……待排期」，与「两子项已落地」产生时点差；「trigger 已如实追加」若读作 post-batch 状态则不成立。修法：留根 3 改写为「trigger 字段按 T1⑦ frontmatter 不动条款维持现文，部分结事实由文末 append 承载」，或在 T1⑦ append 末补一句 trigger 陈旧性注记（ARQ 消费侧有「处方须实测复证」纪律兜底，故定 NIT）。

**亲核留痕（2026-10-27 实测）**：B1 现行脚本 `--self-check` 复跑 10 组逐值同（4/6/29/29/4/19/20/2/10/19，exit 0，含 conflicting-status-lines advisory）。B2 装配副本 1129 行、`node --check` 过、`--self-check` 12 组全绿（cite-check 21 / verbatim-quotes 13 / dir-anchor-expansion 7）。B3 `_scratch/verify-fixtures.mjs` 18/18（m1-verbatim-close-anywhere / m2-cue-first-not-last / m3-no-archive-fallback / m4-dir-anchor-no-expansion 各自变红；目录锚端到端 4 组 + 引文端到端 8 组全过）。B6 复跑（装配副本对 P5 FROZEN）：ok:true / phase landing / targetCount 13 / checked 20 / missing 0 / notChecked 4（`:238`/`:249`/`:285`/`:386`）/ unverified 0，归档命中 `:170`→ARQ-261024、`:433`→ARQ-261017、`:454`→ARQ-260908——与 B6 行逐值同。B7 复跑：ok:false / checked 17 / missing 1（`:112` 块）/ notChecked 8 / unverified 0——同。B10 对**现行 966 行 DRAFT** 复跑：checked 23 / missing 0 / notChecked 17 / unverified 0——同（B10 拆账 16+6+1 与评审独立计数吻合）。B10b 沙盘 `--phase landing`：checked 19 / missing 0 / notChecked 23 / unverified 0，归档命中 `:695`/`:825`/`:833`→260924 归档件、`:703`→261026 归档 append——同。B9 `debt-render --check` exit 0；B11 `--preflight` 复跑 ok:true / anchorCount 5 / advisories [] / findings []；`_scratch/check-consistency.mjs` 40/40；五件目标文件 `git ls-files --eol` 均 `i/lf w/lf`；`.claude/scripts/` 无独立 cite 脚本（`ls | grep -i cite` exit 1）；`governance-gate-shared.mjs` `:42`（ANCHORABLE_TEST）与 `:380`（「fence 定界行为奇数」）引据核实。**沙盘注记（不计 finding）**：`C:/tmp/p6-assembly` 现为空目录、装配副本实体在 `C:/tmp/p6-post`（§6 环内自律与 B3 默认路径仍写 p6-assembly——仓外临时件、非落地面）；`C:/tmp/p6-post` 内 PROPOSAL 副本为 945 行早期快照，与现行稿差异全部在 §2 之外，B10 已对现行稿复跑闭合。结构字段：landing-manifest 围栏块（5 行）与 packet-id 行在场核对通过；manifest 不含 `_scratch/`、不含生成件——口径自洽。

verdict: BLOCK(F1,F2,F3,F4)

## 修订记录（作者侧 · v0.1 → v0.2，2026-10-27）

> 本节是作者侧修订留痕，不替代 §7 判官记录；R1 记录逐字未动（「起草期自检留痕」小节物理归位到 §7 之前后，R1 恢复挂在 §7 之下）。

**依据**：R1 verdict BLOCK(F1,F2,F3,F4)（§7）。

| # | 来源 | 修订 |
|---|---|---|
| 1 | R1-F1 | §5 V6 的 frontmatter 字面 `closed:"2026-10-27"` → `closed: 2026-10-27`（冒号后空格、无引号），与 T2③ / T3⑥ 改后行逐字对齐；`closedBy` 字面经 R1 核对无误、未动。 |
| 2 | R1-F2 | §2.0 复证 4 装配副本行数 1121 → **1129**（与 §5 B12 同值；自检组数 12 / 21 / 13 / 7 不动）。 |
| 3 | R1-F3 | §3 硬项 census、§5 B8 与「起草期自检留痕」第 2 条对话术三项改「豁免面 + 出处注记」：`正读数镜像` 4 文件（归档 retro 术语出处 / task_plan 立项材料 / ARQ-260914 处方源文追注 / debt INDEX 生成件）、`可用但劝阻` 1、`展开是兜底不是许可` 1（均 task_plan）；机器标识符 15 项「全 0」成立、保留为断言。 |
| 4 | R1-F4 | §6 留根 3 改写：删「trigger 已如实追加」，改述「frontmatter 按 T1⑦「frontmatter 不动」条款维持现文；部分结事实由文末 append 承载；ARQ 消费侧有『处方须实测复证』纪律兜底」。 |
| 5 | — | 「起草期自检留痕」小节物理归位到「## 7. 评审记录」之前（R1 由此恢复挂在 §7 之下、逐字未动）；小节内自指元数据按 v0.2 复跑刷新（版本标签 / `wc -l` / preflight 与 cite-check 复跑值）。 |
| 6 | — | **manifest 不变（5 行整）**；**判档与 riskTier 不变**（Track B 主路径 / medium）：四项均为文面级，零围栏、零脚本、零门、零路径变更，不改爆炸半径。 |

**未纳入（作者判断，judge 可 challenge）**：R1 留痕段「沙盘注记（不计 finding）」所述 `C:/tmp/p6-assembly` 为空、`C:/tmp/p6-post` 内 PROPOSAL 副本为 945 行早期快照——两处均为仓外临时件、非落地面，v0.2 只补一条现状注记（`C:/tmp/p6-assembly` 与 `C:/tmp/p6-post` 现存在、装配副本 1129 行自检可跑，见下复跑留痕），不改任何正文断言；B10 / B10b 已对现行稿复跑闭合。

**v0.2 收口微调（R2-N1 顺手项，2026-10-27；不改版本号）**：§5 V6 判别列「脚本 = 2」的括注由「（advisory 文案 + 头注）」改为「（T3④ 函数注释 + advisory 文案）」——装配副本实测两处 = `:264`（`dirTokenAdvisories` 函数注释）与 `:269`（advisory 文案字符串），头注（T1① 用法块）用「仍劝阻使用」不含该字面；计数 = 2 与断言不变，仅括注位置描述更正。连带刷新「起草期自检留痕」第 4 条的 `wc -l` 自指值（同轮实测）。


### R2 - 2026-10-27 - architect-reviewer

**R2 聚焦轮（R1 findings 修订到位性 + 新矛盾扫描 + 结构不变量 + 复跑抽查；不做全量重核）。**

**四项修订逐项核到位**：

1. **R1-F1 · 到位**。§5 V6 frontmatter 字面已改 `closed: 2026-10-27` 各 = 1（冒号后空格、无引号），与 T2③/T3⑥ 改后行逐字对齐（v0.2 文 :695/:825 改后行亲读核对）；判别列补出处注记（「R1-F1 更正——v0.1 曾误写 `closed:"2026-10-27"`」）；`closedBy` 字面未动（R1 已核无误）。修法方向与 R1 建议一致。
2. **R1-F2 · 到位**。§2.0 复证 4 装配副本行数 1121 → **1129**（v0.2 文 :76 亲读），与 §5 B12（:898）同值；自检组数 12 / 21 / 13 / 7 未动（R1 实测本就无误）。全稿残留扫描：`1121` / `966` 现仅存在于 §7 R1 历史记录内（历史留痕、应保留），正文零残留。
3. **R1-F3 · 到位**。三处同步核过：§3 硬项（:857）改「机器标识符 15 项实测全 0」+「话术三项按豁免面申报、不作『= 0』断言（R1-F3 更正）」，豁免面四文件与 R1 实测逐项一致（归档 retro / task_plan / ARQ-260914 / debt INDEX）；§5 B8（:893）同口径；「起草期自检留痕」第 2 条（:934）同步。机器标识符 15 项保留为断言——与 R1 实测（15 项全 0）吻合，收窄合法。择案 = R1 给出的修法方向一（豁免面 + 出处注记）。
4. **R1-F4 · 到位**。§6 留根 3（:922）删「trigger 已如实追加」，改述「frontmatter 按 T1⑦『frontmatter 不动』条款维持现文（其中 trigger 的『剩余（机械化判据）无排期』等字面为开具时点状态；部分结事实由文末 append 承载，ARQ 消费侧有『处方须实测复证』纪律兜底）」——与 T1⑦ 口径闭合，时点差显式化。择案 = R1 给出的修法方向一。

**新矛盾扫描 · 未发现**：① Status 行升 v0.2 并如实注 R1 修订轮（机器锚 isFrozen 消费面不受影响，仍 DRAFT）；② 修订记录段（:992-1007）为作者侧新增尾段，自注「不替代 §7 判官记录」、不触任何机器锚（manifest / packet-id / riskTier / §2 / §5 判定行零变更），row 6「manifest 不变（5 行整）/ 判档与 riskTier 不变」亲核属实；「零围栏、零脚本、零门、零路径变更」与四项修订实际相符；③「起草期自检留痕」小节物理归位到 §7 之前（:931，§6 之后）——§7 判官区自此只含判官记录，归位合理；留痕 4 自指元数据按 v0.2 刷新（`wc -l` = **1008**，亲测吻合）；④「未纳入」判断（沙盘注记两处仓外临时件不改正文断言）可接受——且作者已重建 `C:/tmp/p6-assembly`（`.claude/scripts/landing-fidelity-check.mjs` 1129 行、与 p6-post 副本 diff 逐字节相同、`node --check` 过、`--self-check` 12 组全绿），v0.2 的「现存在、自检可跑」注记现为真。

**结构不变量亲核**：R1 段（:966-990）与 verdict 行（:990 `verdict: BLOCK(F1,F2,F3,F4)` 行首形态）逐字未动；landing-manifest 围栏块仍 5 行（:12-18，与 §2 目标文件并集一致）；packet-binding 行 / riskTier medium / profile G2+G3 / Track B 主路径判档全部未动。

**复跑抽查（作者自报复跑值的独立复核）**：`--preflight` v0.2 复跑 ok:true / anchorCount 5 / advisories [] / findings [] ✓；`--cite-check`（装配副本对 v0.2 稿）checked 23 / missing 0 / notChecked 17 / unverified 0 ✓；`_scratch/check-consistency.mjs` 40/40 ✓；V6 字面干跑可判性——`可用但劝阻` 脚本 = 2 亲测吻合（见下 NIT）、template = 1（T3⑤ 改后片）✓、`正读数镜像` template = 1（T1⑥ 改后片）✓、frontmatter 四字面与 T2③/T3⑥ 改后行逐字对齐 ✓、负向字面（`目录须展开为成员枚举` / `目录简写收口期必红` / `landing 门不消费目录锚`）在 T3④/T3⑤ 改后文本中确认归零 ✓。

**findings**：

- **N1 · NIT · V6「脚本 = 2」的位置注记「（advisory 文案 + 头注）」与实况不符**：装配副本实测 `可用但劝阻` 两处分别在 `:264`（T3④ `dirTokenAdvisories` 函数注释）与 `:269`（advisory 文案字符串）；脚本头注（T1① 用法块）用的是「仍劝阻使用」、不含该字面。计数 = 2 正确、断言机械可判、判定不受影响——纯描述性括注不准。处置建议：不必另开评审轮；主线程标 FROZEN 前顺手把括注改为「（T3④ 函数注释 + advisory 文案）」即可，或保持现状（判读以 grep 实测为准）。

**判档复核**：v0.2 修订全部为文面级，riskTier / profile / 判档 / manifest 零变更——R1 判档结论（Track B 主路径、无升档、无升级 human 项）维持。

verdict: GREEN
