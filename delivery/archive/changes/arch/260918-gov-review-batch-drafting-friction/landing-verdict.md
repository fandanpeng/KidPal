# landing-verdict · 260918-gov-review-batch-drafting-friction

**verdict：PASS + 2 ADVISORY**

核验官：landing-verifier（Track B，零上下文独立核验——一切判断重新推导，不采信落地者 landing-summary 自述）
日期：2026-09-19
仓库：`C:/AI/KidPal-gov` @ `gov/kidpal`，HEAD = `2492f44`（main → gov/kidpal 回同步 merge）；本批 landing 改动**未 commit**，在工作树。
packet：FROZEN PROPOSAL v1.0 §2 改法集（T1–T3）+ §5 验证集（§7 为 R1–R4 判官 append-only 记录，未改）。
结论：结构门绿、三 manifest 文件语义保真、验证集 V1–V9 亲跑与 summary §2 声称一致；无可升级 human 项、无私货、无漏改、无与批定矛盾。两条非阻断观察见末节 ADVISORY。

---

## 0. 核验面界定（回同步 merge 不计入本批）

承诺核验前先划清边界：落地前有一次 `main → gov/kidpal` 回同步 merge（commit `2492f44`）。核验实证确认为**该 merge 带来、非本批 landing 内容**：

- 结构门 advisory `changedCanonicalNotInProposal` 列出的 3 件（`delivery/workstreams/INDEX.md`、`delivery/workstreams/m2-char-window/progress.md`、`docs/ARCH-REVIEW-QUEUE.md`）**全部**由 `2492f44` 引入（`git diff --name-only 2492f44~1 2492f44` 命中三件），且**工作树中均无未提交改动**（`git status --porcelain -- <三件>` 为空）。
- 工作树未提交改动面 = 4 件 modified（`.claude/scripts/landing-fidelity-check.mjs`、`docs/templates/PROPOSAL.template.md`、`.codebase-memory/artifact.json`、`.codebase-memory/graph.db.zst`）+ 2 件 untracked（`.claude/references/path-form-family.md`、`delivery/changes/arch/260918-gov-review-batch-drafting-friction/architect-landing-summary.md`）。其中 manifest 三路径正是本批 landing；`.codebase-memory/*` 两件为可再生机器面（AGENTS.md 明列常态脏、非 landing）；`architect-landing-summary.md` 为落地者自述件。
- 故该 merge **不计 scope creep**；同时逐件核过工作树无「本批真夹带的私货」（见 §1 逐文件对照）。

## 1. 逐文件 diff 对照（清单 3/3，逐件人读）

对照源 = FROZEN §2 各 T 的改法（Track B 无 decision-packet）。actual 取自 `git diff HEAD -- <file>`（新文件读全文）。

| file | structuralTouched | semanticMatch | expected（FROZEN §2） | actual（diff 实证） |
|---|---|---|---|---|
| `.claude/scripts/landing-fidelity-check.mjs` | 是（M） | **ok** | T1：`:90` 候选域补 `.claude/references/` + docs 子目录形态，取**并集形态**（原四前缀原样保留 ∪ `docs/`、`.claude/` 树下递归全部目录），派生前缀集钉字典序；消警留痕仍走 `resolvedByPrefix`。T2：新增 `--preflight <PROPOSAL>` 只读干跑（只解析、不对账），报**两类错**（加粗锚行 + 无锚清单），与既有位置参数互不干扰，自检电池加接线断言。 | T1：`CITATION_BASE_PREFIXES = ['.claude/scripts/', '.claude/scripts/lib/', '.claude/hooks/', 'docs/']`（原四前缀 verbatim）；`citationPrefixes(dir)` = `new Set(CITATION_BASE_PREFIXES)` ∪ 对 `docs`/`.claude` 递归 `readdirSync` 派生的全部目录，`[...out].sort()` 字典序，按 `resolve(dir)` 缓存；`:90` 改调该函数。**真为并集**（见下核）。T2：`parseCliArgs()` 抽纯函数，`--preflight` 在场时 `{positional:null, baseRef:null}`；`BOLD_ANCHOR_RE`/`section2Body`/`preflightFindings`/`runPreflight` + 入口置于 `--self-check` 块之后。 |
| `docs/templates/PROPOSAL.template.md` | 是（M） | **ok** | T2：硬项 2a 段补两句——① FROZEN 前跑 `--preflight` + §5 验证集命令实跑；② 路径类变更先查 `path-form-family.md` 族谱表（②即 T3 指针，故 T3 不重复改本文件）。 | 硬项 2a 段后新增「**【起草期自检两句】**」段，含①`--preflight` + §5 实跑、②指针 `path-form-family.md`。逐字核对两句话义到位。 |
| `.claude/references/path-form-family.md` | 是（新增） | **ok** | T3：新增八类形态表（各配残留清扫子检模式）+ 三类批量替换风险注记 + 损伤总账（四数分属四类、忌整体归因）+ 第 6 类实现-判据不一致教训 + 完备性边界声明（非穷举）。 | 新增 65 行全文核：八形态行齐（1 斜杠形…8 点号成员访问粘连形）；风险注记①幂等双应用/②点前缀误吃成员访问/③转义形不可见齐；损伤总账表将 `18/10/1/24` 分列粘连形/转义形/幂等双应用/V1 自咬；第 6 类教训（卫兵字符类排除 `/` vs V1 判据 `[^a-z]` 含 `/`）在场；边界声明「该批实测所见八类，非理论穷举」在场。 |

**T1「并集形态」专项核**（FROZEN §2 T1 核心风险点）：独立重算派生并集 = **43 项**，四项原前缀**全部在场**（含 `docs/` 根——根非子目录、只能来自硬编码保留，故其命中即证并集未丢前缀）；`.claude/references/`、`docs/templates/`、`.claude/scripts/lib/` 均在集。派生源=文件系统实况（递归 `readdirSync`），非手工清单，符 OQ1(a) 裁决与 §6 留根1 的并集口径。**未放松任何判据**：citation 检查全程 advisory-only（`problems` 只进 `citationAnchorAdvisory`，不入 `errors`），故不产生任何 fail→pass 的语义移动。

**T2「`--preflight` 不误触 reviewDir/baseRef」专项核**（FROZEN §2 T2 核心风险点）：`parseCliArgs` 在 `--preflight` 在场时把 `positional`/`baseRef` 置 `null`，与既有 `argv[2]`/`argv[3]` 位置参数分支互斥；常规分支 `positional=argv[2]`、`baseRef=argv[3]` 与改前逐字一致（含「`--self-check` 会被当 positional」的既有小瑕，未顺带修改）。`runPreflight` **只调** `parseSection2AnchorLists().problems` + 加粗探针，**不调 `compareAnchorSets`**，符「只解析、不对账」。干跑入口置于 `--self-check` 块之后。

**逐件私货核**：三文件改动全部落在 §2 T1/T2/T3 授权面内；无 G1 指令根、G3 强制层、`architect.md` self-def、review-loop agent 定义、`docs/contracts/**`、agent frontmatter 的任何触碰（本批 FROZEN §3 亦声明零接触）。零私货、零权限/写域扩张。

## 2. packet-binding 一致（清单 2/2）

- **summary packet-id 行**（`:3`）：`packet-id：FROZEN PROPOSAL v1.0 §2 改法集（T1–T3）+ §5 验证集 · 评审轮次 R1 BLOCK → R2 BLOCK → R3 BLOCK（熔断）→ R4 GREEN · change-id 260918-gov-review-batch-drafting-friction` ↔ FROZEN §2（T1–T3）+ §5 + §7 R4 GREEN 结论**逐项相符**。被批 review id 在场（结构门 packetBinding=true）。
- **manifest ↔ §2 目标文件锚清单并集**：结构门亲跑 `proposalTargets = summaryTargets =` 且 §2 锚集 ≡ manifest（3 路径互恰、`compareAnchorSets` aligned、零 problems）。三路径 = §2 各 T「目标文件：」行并集：T3（`path-form-family.md`）+ T1/T2（`landing-fidelity-check.mjs`）+ T2（`PROPOSAL.template.md`）。
- **T3 条目清单核**：§2 T3 的八类形态为绑定面——落地表 8 行齐、各配清扫子检模式（`grep -c "^| [1-8] "` = 8）。
- 简报模板的「T2 五件组件」子句系通用占位、**本批 §2 T2 未声明组件清单**（其绑定面 = `--preflight` 行为 + 模板两句），不构成核验项。

## 3. 验证集复跑（V1–V9 逐条亲跑）

| 项 | 亲跑命令 | 亲跑实测 | 与 summary §2 比对 |
|---|---|---|---|
| V1 | `node .claude/scripts/landing-fidelity-check.mjs --self-check` | exit 0；`citation-anchors 14/14`（含 `references-prefix-*`/`templates-prefix-*` 具名）、`landing-preflight 9/9`、其余八套零回退 | 一致 ✓ |
| V2 | 同上电池内 | 「干跑模式 positional/baseRef 均为 null」两断言通过 | 一致 ✓ |
| V3 | `--preflight delivery/changes/arch/260918-gov-review-batch-drafting-friction/PROPOSAL.md` | `ok:true` / `findings:[]` / `anchorCount:3` / `section2BodyFound:true` / exit 0 | 一致 ✓ |
| V4① | 注入行首加粗锚（`目标文件：`→`**目标文件**：`）副本 → `--preflight` | exit 1；findings=`[bold-anchor-line(行4), missing-anchor-list(T1)]` | 一致 ✓ |
| V4② | 抽掉 T2 整条锚清单副本 → `--preflight` | exit 1；findings=`[missing-anchor-list(T2)]` | 一致 ✓ |
| V4③ | §2 域内散文**行内引用**该错误形态副本 → `--preflight` | exit 0；findings=`[]`（不报 bold） | 一致 ✓ |
| V5 | 合成 frozen PROPOSAL（复用本批 3 真实 manifest 路径）载入四引据，过**真实** gate 的 `checkCitationAnchors` | 派生并集 43 项；`DEV-GOVERNANCE.md:280`→`docs/`、`governance-gate-shared.mjs:345`→`.claude/scripts/lib/`（防回归两条仍 resolved）；`dev-loop-operating-system.md:130`→`.claude/references/`、`PROPOSAL.template.md:49`→`docs/templates/`（新增两条 resolved）；problems=[] | 一致 ✓ |
| V6 | `grep -c "path-form-family" docs/templates/PROPOSAL.template.md` | `1`（≥1） | 一致 ✓ |
| V7 | `ls .claude/references/path-form-family.md` + `grep -c "^\| [1-8] "` | 文件在场；八形态行 = 8 | 一致 ✓ |
| V8 | `route-regression-check.mjs` + `lint-descriptions.mjs` | route `28 ok, 0 fail` exit 0；lint exit 0（仅既有 `landing-verifier.md` 超长告警，本批未触该文件） | 一致 ✓ |
| V9 | `landing-fidelity-check.mjs <本批>` | 见 §4 | 由本核验官亲跑 |

## 4. 结构门复跑（亲跑退出码）

```
node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260918-gov-review-batch-drafting-friction
→ exit 0
{ "ok": true, "frozen": true, "packetBinding": true,
  "proposalTargets": [3 路径], "summaryTargets": [同 3 路径],
  "unauthorized": [], "claimedNotLanded": [], "plannedNotLanded": [], "gitDegraded": false,
  "citationAnchorAdvisory": { "checked": 18,
     "problems": [{"citation":"260914-gov-review-delivery-restructure/retro.md:38","reason":"file-not-exist"}],
     "resolvedByPrefix": [含 dev-loop-operating-system.md→.claude/references/、PROPOSAL.template.md→docs/templates/、acceptance-id-shared.mjs→.claude/scripts/lib/ 等] },
  "advisory": { "changedCanonicalNotInProposal": [delivery/workstreams/INDEX.md, delivery/workstreams/m2-char-window/progress.md, docs/ARCH-REVIEW-QUEUE.md] } }
```

结构门绿（exit 0）。advisory 一条 `retro.md:38` file-not-exist = summary §4 已如实披露的残引；`changedCanonicalNotInProposal` 三件经 §0 归因为回同步 merge、非本批。

（`.claude/scripts/cross-surface-check.mjs` 本仓未迁入，该子门按脚本在场才跑、跳过——非本批引入。）

## 5. 真空绿规约核对

- **重放/夹具类先断言输入非空**：V4 三例夹具均非空、断言精确计数（`boldCls(boldTrue)===1`、`resolved.length===1` 等），非恒绿；`runPreflight` 对缺失 PROPOSAL 显式 `exit 1`（实测 `--preflight <不存在>` 逻辑在场）。空输入的「绿」不构成证据。
- **接线类 = 输出含新增自检名 + 顺序**：V1 输出**具名**含 `citation-anchors 14/14` 与 `landing-preflight 9/9`（防「加了夹具没接线」）；接线顺序核——终端式 `runIsFrozenSelfCheck`（无条件 `process.exit`）**仍在最后**（源码顺序 + V1 输出末位为 `isFrozen` 双证）。
- **入口优先级**：实测 `--self-check --preflight <f>` 同给时，自检先跑并以自检退出码为准（exit 0），与 summary §3-2 一致；`--preflight` 缺参时给出显式错误并 exit 1。
- **环境红基线**：本批涉闸执行层无红——所有亲跑均退出码 0（除刻意构造的反例夹具按预期 exit≠0）。

## 6. 诚实披露核对（无该记未记——除 ADVISORY-1 的一处数字）

- summary §3 四条事实性偏差：① 自检计数已增——**方向属实、基线数字失准**（见 ADVISORY-1）；② `--preflight` 入口置于 `--self-check` 之后——**属实**（源码 + 实测双证）；③ `citationPrefixes` 按 dir 缓存（防夹具复用真实仓派生集致假绿）——**属实**（源码 `resolve(dir)` 缓存键）；④ `parseCliArgs` 常规分支保持改前逐字一致、未顺带修 `--self-check` 当 positional 的既有小瑕——**属实**。
- summary §4 残余：留根⑥（第 3 类未覆盖、覆盖面为**两类**非三类）↔ FROZEN §6 留根⑥ 及缩圈裁定一致；留根②（验证集实跑无机械判据）一致；结构门残留 advisory `retro.md:38` file-not-exist ↔ 结构门输出**逐字相符**、且该引据在 §7 append-only 面（落地者不可改）——**属实**。
- **无未披露的私货/偏差**：工作树除三 manifest + 两可再生件 + 自述件外无其它改动；未发现应记未记的落地期偏差。

---

## ADVISORY（非阻断观察；不阻 merge，建议主线程顺手校正）

- **ADVISORY-1 · summary §3-1 基线计数失准**：summary 写「`citation-anchors` 由 **5 条**增至 14 条……+9 条 citation 断言」；实测**改前为 8 条**（`git show HEAD:…` 逐条数：checked/problems/prefix×3/docs×3），本批新增 **6 条**（references 组 3 + templates 组 3），即 **8 → 14（+6）**，非 5 → 14（+9）。方向（"数量多于字面"）正确、落地本身无误；仅该数字应更正为「8 → 14」，免与「两新增夹具」的字面口径产生 +9 的误读。属披露精度问题，非 landing 保真问题。
- **ADVISORY-2 · `BOLD_ANCHOR_RE` 未镜像 parser 的 `>` 去引用前缀**：探针 `/^\s*\*{1,2}\s*目标文件\s*\*{0,2}\s*[:：]/` 为行首锚定（符 FROZEN §2 T2「行首锚定」字面要求），但不覆盖 `> **目标文件**：` 这类引用前缀形态——而 parser 会先 `strip /^\s*>\s*/` 再判。实测该形态下探针**不具名** `bold-anchor-line`，但 parser 仍报 `missing-anchor-list`（**无假绿**，仍 red）。属与 R2-F8 括注「镜像 parser 锚行识别形态」的极窄偏差，FROZEN §2 T2 未硬性要求引用前缀镜像；可选（非必须）同款 strip 以对齐。

## 破坏性自检（核验浅度自查）

- 三 manifest 文件**全核**（非抽样）：`landing-fidelity-check.mjs` 163 行 diff 逐行读、`PATH-form-family` 新文件 65 行全文读、模板 2 行 diff 读。
- 敏感面**已核**：G3 脚本（本批承载面）逐字核；G1/self-def/review-loop agent 定义/contracts 本批**零触碰**（已用工作树改动面反证）。
- 未躲大 diff、未只挑好看文件核、未采信 landing-summary 自述（三处 summary 声称均以独立命令复核）。
- 只读边界：核验全程未改任何 canonical / PROPOSAL（含 §7）；夹具只写系统临时目录并即清；唯一落盘 = 本 verdict。

**结论：PASS + 2 ADVISORY。** landing 语义保真（改法落到位、无漏改、无私货、与批定一致），两机械门与 V1–V9 与声称相符。merge 仍归 human manual-merge 门。