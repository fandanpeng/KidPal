# architect-landing-summary · 260920-gov-review-citation-anchor-residue

packet-id：FROZEN PROPOSAL v1.0 §2 改法集（T1–T3）+ §5 验证集 · 评审轮次 **R1 `BLOCK(F1)`（1 BLOCKING + 4 MAJOR + 2 NIT，无升级 human）→ v0.2 → R2 `BLOCK`（1 MAJOR + 2 NIT，均 R1-F1 折入的残留同步）→ v0.3 → R3 `GREEN`（1 NIT 不拦）** · change-id 260920-gov-review-citation-anchor-residue · OQ1=(a) / OQ2=(a) / OQ3=(c)（human 2026-09-20）

## 1. 落点对照（manifest 3/3）

| manifest 路径 | T 项 | 落了什么 |
|---|---|---|
| `.claude/scripts/landing-fidelity-check.mjs` | T1①②③④ / T2 / T3(a)(c) | ① 候选域改**git 索引面派生**：新增 `gitCitationPrefixes()`（`git ls-files -z` + `--others --exclude-standard` 两式同行，逐段累积祖先目录）、`staticCitationPrefixes()`（回退分支，四项常量 ∪ 两树递归）、`citationPrefixes()` 改为返回 `{ prefixes, source }` 并保留按 dir 缓存；② 失败留痕 `prefixSource` = `'git-index'` / `'fallback-static'`；③ `checkCitationAnchors()` 返回扩为**六列**（`problems` / `resolved` / `abbreviated` / `ambiguousShortForm` / `shortFormUnresolved` / `prefixSource`），六步判定顺序按 §2 T2 钉死，**多命中不再取首个**（旧 `break` 删除）；「一级目录集合」取自同一份派生前缀集的一级段；④ `citationAnchorAdvisory` 观察列同步扩为七字段；⑤ `section2Body()` 改为返回 `{ body, offset, inFence }`、新增 `fenceFlags()`；⑥ `preflightFindings()` 报**文件绝对行号** + 跳围栏内加粗探针；⑦ `runPreflight()` 输出新增 **`anchorUnion`** 并集成员名单（`ok` 判定一字不动）。 |
| `.claude/scripts/lib/governance-gate-shared.mjs` | T3(b) | 新增 `fenceLineFlags()`（三反引号开合跟踪，含开合行自身）；`parseSection2AnchorLists()` 的条目标题扫描与锚行扫描**都跳围栏内**——条目 `lines` 改存 `{ text, fenced }`、锚行 `filter((l) => !l.fenced)`；`runSection2AnchorSelfCheck()` **+6 断言**（围栏带 backtick / 裸写 / 幻影条目不吞锚三族）。**这是本批唯一被门消费的改动**。 |
| `docs/templates/PROPOSAL.template.md` | T3 搭车件 | ① 五段固定式示范块**撤 `〔第五段·锚清单〕` 变形标记**、示范行还原裸 `目标文件：`（与门同形）；② 「读法注」由两条扩为**三条**（第 1 条改写为形态沿革 + 变形标记撤销的完整交代；第 3 条新增「锚行注记不引路径 token」纪律，含 260912 near-miss 与 `anchorUnion` 逐 token 核法）；③ 【起草期自检】① 句补 `anchorUnion` 与绝对行号两句。 |

```landing-manifest
.claude/scripts/landing-fidelity-check.mjs
.claude/scripts/lib/governance-gate-shared.mjs
docs/templates/PROPOSAL.template.md
```

## 2. 验证集执行记录（V1–V8）

| 项 | 命令 / 方法 | 结果 |
|---|---|---|
| V1 | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260920-gov-review-citation-anchor-residue/PROPOSAL.md` | **`ok:true` / `anchorCount: 3` / `findings: []`**（起草期、标冻前、落地后各一次）✓。首跑为红——三处锚行照抄了模板的变形示范形态，撤前缀后转绿（该实例已记入 §2 T3 读法注，是 T3 立案理由的第七个实例） |
| V2 | `node .claude/scripts/landing-fidelity-check.mjs --self-check` | **十路全绿、断言总数 112 → 131（+19）**。`citation-anchors` **14 → 19**、`landing-preflight` **9 → 17**、`section2Anchors` **19 → 25**；其余七路与基线同值 ✓ |
| V3 | `node .claude/scripts/route-regression-check.mjs` | **`28 ok, 0 fail`**——与起草期基线同值（本批不动任何 agent def / command，属纯回归）✓ |
| V4 | **批③ 端到端复放**：`node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260920-gov-review-bearing-strength-align 173d734` | **`ok:true`**；`citationAnchorAdvisory` = `prefixSource: git-index` / `problems: []` / `resolvedByPrefix` **raw 12 条（distinct 6；其中属本批新解析的 delta = 2 条，两条均 → `delivery/workstreams/`）**〔A3 更正：原写「2」把 delta 当成了数组长度，核验官实测 raw=12〕 / `abbreviated` 1（`…/PROPOSAL.md:126`）/ `ambiguousShortForm` 1（`progress.md:18`，**5 命中**：`_archive/2026/{260911-display-interaction-sovereignty,kidpal-mvp,kidpal-validation}` + `m1-home-qa` + `m2-char-window`）/ `shortFormUnresolved` 0。**与 FROZEN §2 T1 的「4 条 file-not-exist 归零」预言逐条吻合**（旧实现同命令跑出 `problems` 恰为那 4 条，核验官已独立复证）✓ |
| V4b | 本件自身复放 | 改前 `resolvedByPrefix` **0**、全部进 `problems`；改后去重切分 = `resolvedByPrefix` 2 / `abbreviated` 8 / `ambiguousShortForm` 4 / `file-not-exist` 2（**1 条真信号 `docs/spec.md:252` + 1 条 R1 判官记录逐字回引的自检夹具名**）。raw 计数（checked / problems）随本文档增删漂移，**不作断言**——序列见 §5 V4b ✓ |
| V5 | **反空过**：`CLAUDE_PROJECT_DIR=/tmp/v5repo node .claude/scripts/landing-fidelity-check.mjs <批③ 目录>`（临时副本**非 git 仓** → 逼入回退分支） | `prefixSource` = **`fallback-static`**（留痕生效）✓；`resolvedByPrefix` **0**（**不静默假绿**）✓；**分类漂移如文档所述**——`progress.md:18` 由歧义落 `shortFormUnresolved`（回退域无 `delivery/` ⇒ 0 命中）、`…/PROPOSAL.md:126` 仍 `abbreviated` ✓。**已知局限如实记录**：该夹具是隔离临时目录、**仅含批③ 制品**，故连「写全仓相对路径」的引据也不在场——这是沙箱的口径限制，不影响本条要走的三项断言；完整域下的对照见 V4 |
| V6 | `git status --porcelain`（剔 `.codebase-memory/`）+ `git diff --numstat` | **恰 3 路径 = manifest 3/3**，零多零少；numstat：`229/79`（landing-fidelity-check）、`63/3`（governance-gate-shared）、`6/5`（PROPOSAL.template）✓ |
| V7 | 语义面复核 grep | `ls-files` **1 行**（同行含两式——**落地实况校准了起草期「两处调用」的措辞**）；`CITATION_BASE_PREFIXES` **2 命中且全在「常量定义 + 回退分支」**（正常 git 路径不引用，非「零命中」）；四类字段齐备 **30** 命中；`fenceLineFlags` **2**（定义 + 调用）；模板 `〔第五段·锚清单〕` **1 命中且仅在形态沿革说明句内**（无活体使用——同为落地实况校准）；`锚行注记不引路径 token` **1** ✓ |
| V8 | `landing-verifier` 独立核验 | 见 `landing-verdict.md` |

**真空绿规约核对**：V2 的引据夹具**先断夹具输入非空**（`git init -q` + `git add -A` 后派生前缀集非空、`checkCitationAnchors` 的 `prefixSource` 断言为 `'git-index'`）再断分类输出；**gitignore 正控**（`build/sub/gen.mjs`）刻意排除「字面直中」捷径——引的是**只有 `build/` 成为候选前缀时才解析得到**的短形态 `sub/gen.mjs`，故「resolved 为空 + 落 `shortFormUnresolved`」是**真控制**而非恒真；围栏三族夹具**先断言围栏内 token 确实存在**（`.replace()` 前后串不同）再断不入并集。V5 的临时副本有前置事实（批③ 制品在盘）。

## 3. 落地期发现（起草期未预见，如实记录）

1. **首次自检红**：gitignore 正控夹具初版为 `build/gen.mjs`——该文件**物理存在**，走「字面直中」分支、根本不进候选域，断言恒 0（假测试）。改为 `build/sub/gen.mjs` + 引 `sub/gen.mjs` 后才成为真控制。
2. **模板自身跑 `--preflight` 报 fail-loud**（`§2 无任何 T 条目`）——**这是正确行为**：canonical 模板本就不是一份 PROPOSAL（其 §2 只在围栏块里示范五段固定式）。**该结果本身是 T3 子项 b 的活体验证**：落此改动前，围栏内那两行会被吃成幻影条目；现在被正确跳过，于是报告回到诚实的「没有真条目」。
3. **V7 两条断言按落地实况校准**：`ls-files` 是「一行两式」非「两处调用」；模板 `〔第五段·锚清单〕` 命中 1 处（沿革说明里的逐字回引）而非 0。两处均为**断言校准、不扩不改任何改法内容**，已记入 PROPOSAL Status 行的「FROZEN 后 packet 内修订」。
4. **判官三轮回跑暴露的量纲事实**：本件自身的 raw 引据计数（checked / problems）**随本文档每次增删而变**（`problems` 29 → 34 → 39 → 41），故已降级为「不作断言」；去重 17 与判官数的 16 之差经查为**分母口径不同**（17 = 1 条字面直中 + 16 条进入分类），非笔误。
5. **核验后修正 2 处（landing-verdict A1 / A4，均在对方 PASS 之后、commit 之前）**：
   - **A1（实质未授权项，已改）**：`abbreviated` 正则原为 `/[…»]|\.\.\./`——**多带一个 `»`（U+00BB）**，§2 T2 改法 2 只授权 `…` / `...`，其上注释亦只写这两式。核验官实证 `»` 在全仓 PROPOSAL / `delivery/**` **零出现**、对真实输入无行为影响（且该分类 advisory-only），但仍属**代码与冻结稿不一致**——本批要治的正是这个病，故清理为 `/[…]|\.\.\./`。
   - **A4（冗余前置，已删）**：第 5 步原为 `if (relPath.includes('/') && topLevelDirs.has(...))`，`includes('/')` 恒不改变结果（`CITATION_RE` 要求扩展名、一级目录名不可能等于裸文件名）。删去后与 §2 T2 改法 5 逐字同形。
   - **两处均为「使代码 == 冻结稿」的收敛、零行为变化**；改后 `node --check` 通过、`--self-check` 十路仍全绿（总数 **131**，含本批相关的 `citation-anchors` 19 / `landing-preflight` 17 / `section2Anchors` 25，三项与改前同值）。**该 delta 已请核验官复验**。
   - 其余两条 ADVISORY 无动作：A2 = 模板两处 doc 同步（§1 已声明、无行为影响）；A3 = 本 §2 表 V4 行措辞（已按核验官口径更正）。

## 4. 留根与后续

- 簿记结移与 ARQ 登记**待落地后独立 commit**执行（PROPOSAL §6 留根③）：`ARQ-260907-lfc-citation-docs-prefix` 结移、`ARQ-260920-anchor-parse-fence-lineno` 结移、`ARQ-260914-landing-preflight-anchor-parse` 第 3 类残余标部分结、T2 家族新登一行。
- 其余留根（中段省略不解析 / 存量历史引据不追改 / 非 git 回退面 / 第 3 类判据仍不机械化 / `/loop-retro` 三项观察）见 PROPOSAL §6。
