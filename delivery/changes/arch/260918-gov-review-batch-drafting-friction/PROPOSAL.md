# PROPOSAL · 260918-gov-review-batch-drafting-friction

> **状态：FROZEN v1.0 · 2026-09-19 · 可进 landing（按 human 指示暂不 landing，待 human 阅后再落）**（凭据：human 2026-09-18 立项 ack + OQ 全裁；R1 BLOCK(F1/F2) → v0.6 → R2 BLOCK(F7/F8)+F9–F12 → v0.7 → R3 BLOCK(F13)+F14 → **熔断，human 裁定缩圈**（T2 第 3 类移入 §6 留根⑥）→ v0.8 → **R4 GREEN**〔§7 `### R4 - 2026-09-19 - architect`，含 2 条 NIT N-a/N-b，已在标冻前折入〕）。
> **承载 ARQ**：`ARQ-260914-lfc-citation-references-prefix` + `ARQ-260907-lfc-citation-docs-prefix`（残余①）（T1）、`ARQ-260914-landing-preflight-anchor-parse`（T2）、`ARQ-260915-path-form-family-reference`（T3）。不关的根写 §6 留根指针。
> **母批关系**：本批为 `260918` 拆包族之一（原合并草案 `260918-gov-review-loop-friction-sweep` 已按 human 2026-09-18 裁定拆为四批，逐批 review；原草案已删，其证据已按批内聚进各自 §1）。族内另三批 = `-merge-chain-friction` / `-dispatch-chain-friction` / `-closeout-chain-friction`。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节）。两条升级判据（触契约面 / 跨系统结构性变更）均未命中。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T3）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（一句话证成：三项均为**治理批起草 / 核验辅助面**的改动——T1 扩 advisory 前缀候选域（只消假警报、不放松任何判据）、T2 新增**只读干跑**入口（不进任何闸链）、T3 纯新增参考文档。无闸判定语义改动、无行为收紧面、无数据丢失形态）。
> **review profile**：G3（`.claude/scripts/landing-fidelity-check.mjs`）+ G2（`.claude/references/path-form-family.md` 新增 + `docs/templates/PROPOSAL.template.md` 起草 checklist）。契约面 / G1 / agent frontmatter 为零。**本批是本拆包族里零闸语义触点的两批之一**（另一批为 `-dispatch-chain-friction`）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/references/path-form-family.md
.claude/scripts/landing-fidelity-check.mjs
docs/templates/PROPOSAL.template.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把治理批**自身**的三处摩擦修掉——结构门对真实存在的文件喊「找不到」、提案清单写错要到落地期才炸、路径迁移的形态知识不沉淀。

**现状差距**：这三条的共同形态是**同类返工在连批重复支付**：T1 的假警报每批都有，T2 的清单错误连批三次同型返工（含一次 human 格式追认），T3 的那一批为路径形态付了四轮评审 + 落地四轮修复。它们不是设计问题，是**知识没回收 + 检查点放在太后面**。

**非目标（本批不做什么）**：不新增任何工程门（T2 的 `--preflight` 是**只读干跑、不进任何闸链**）；不改 `landing-fidelity-check.mjs` 的既有判据（T1 只扩 advisory 前缀候选域、T2 只加新入口）；不动任何 agent frontmatter；不动 `docs/contracts/**`；不改任何闸的 fail/pass 语义。

## 1. 问题与根因（地面真值）

**【硬项 1a 声明】** 本节断言均为 2026-09-18 在 `gov/kidpal` 工作树现跑实测所得；行号为起草时现行值；解析函数与入口的存在性经亲核。

### 问题 1：`landing-fidelity` citation 前缀重解析候选域偏窄（两处残余）

证据（verbatim 亲核）：`.claude/scripts/landing-fidelity-check.mjs:90` =

```js
      for (const prefix of ['.claude/scripts/', '.claude/scripts/lib/', '.claude/hooks/', 'docs/']) {
```

候选域四前缀——`resolveByPrefix` 认 `.claude/scripts/`、`.claude/scripts/lib/`、`.claude/hooks/`（260907 批 T2 落地）与 `docs/` 根（260907 热修追认）。**两处残余**：

1. `ARQ-260914-lfc-citation-references-prefix`——不含 `.claude/references/`。`260912-gov-review-portfolio-decomposition-chain` 批的 PROPOSAL §7 判官记录引 `dev-loop-operating-system.md:130`（闭集 SSOT 行）报 file-not-exist，而该文件实存于 `.claude/references/` 且行内容逐字相符（该批结构门 2 条 advisory 在册）。
2. `ARQ-260907-lfc-citation-docs-prefix` 残余①——不含 docs **子目录**形态。`PROPOSAL.template.md:49` 类裸名（真身 `docs/templates/`）仍报 advisory（该条 260907 已走过一次一次性热修，只补了 `docs/` 根）。

**起草期现跑实测（2026-09-18）**：

| 裸名 | 真身 | 现候选域命中 |
|---|---|---|
| `dev-loop-operating-system.md` | `.claude/references/` | **未命中** → 报 file-not-exist |
| `PROPOSAL.template.md` | `docs/templates/` | **未命中** → 报 file-not-exist |

**影响面**：凡评审记录 / PROPOSAL 引 references 面或 docs 子目录面文件的批次复现 advisory 假警报，结构门输出信噪比持续走低，每批都要人工定性一遍。**根因**：候选域是**手工维护的清单**，随仓结构增长而漂移——这个漂移形态正是本问题本身的成因（260907 补一次 `docs/` 根、260914 又要补 references，下次还会有新目录）。

### 问题 2：PROPOSAL §2 锚清单无冻结前干跑，形态错到落地核验期才炸

证据（连续三批同型，逐批出处）：

1. `260913-arch-review-spec-split`——PROPOSAL §2 误用**加粗**锚行 `**目标文件**：`（解析器只认裸 `目标文件：`）；human 2026-09-13 追认格式归一。
2. `260912-gov-review-portfolio-decomposition-chain`——同型四处加粗锚行 + T1 锚行注记逐字回引已撤幻影路径 `.claude/skills/triage-change/SKILL.md`；修完加粗形态后该 token 会进 parser 锚并集、造成 §2↔manifest **假失配**（该批 near-miss，靠 landing-verifier D4 处置意见「锚行裸写、注记不引路径 token」抓住）。
3. `260914-gov-review-delivery-restructure`——更彻底：六 T 锚清单**整行缺失**（非形态错、是整行没有）；**四轮评审 + 判官 manifest 对账（以 census 为基准、恰好绕过 `parseSection2AnchorLists`）均未捕获**，`landing-fidelity-check` fail-loud 首跑 6 条「缺锚清单」才炸。

**连带项**（**转引**——出处 = `delivery/changes/arch/260914-gov-review-delivery-restructure/retro.md:42` 的 F5 条，本批未复现该数字）：该批冻结验证集命令未实跑的口径瑕疵——V1 子检 1 按冻结文本跑得 24 命中、逐行核对全为新前缀被自家模式误咬（缺第二段过滤）。即「冻结可判性」停留在纸面。

**机制侧事实（亲核）**：解析函数 `parseSection2AnchorLists` 已从 `.claude/scripts/lib/governance-gate-shared.mjs:345` **具名导出**（同文件另导出 `compareAnchorSets:426`、`extractManifestBlock:124`）；而 `.claude/scripts/landing-fidelity-check.mjs` 现只接受 `process.argv[2]` = reviewDir（`:50`）、`process.argv[3]` = baseRef（`:52`）与 `--self-check`（`:226`），**无任何前置干跑入口**。

**根因**：错误类中**至少两类**（加粗形态 / 无锚行含整行缺失）能被**已导出的纯解析函数 + 一条行首锚定探针**在**起草期**抓获，但它们只在 landing 期才被调用——**检查点放得太靠后**。**另注**：第三类（锚行注记夹带路径 token / 幻影路径）经三轮评审证明**其判据设计不成立**（对合法「不在场」形态持续误报），已按 human 熔断裁定移出本批覆盖面，见 §6 留根⑥。

### 问题 3：路径迁移类变更的「路径文本表示形态族谱」未固化

证据（`260914-gov-review-delivery-restructure` 全环实测，同批四连击）：该环因路径批改付了**四轮评审 + landing 四轮修复**，八种形态逐一现撞（含**成员访问粘连形** `taskPackage.deliveryMilestoneStateChanged` 18 处——四轮评审 + V1–V6 全机检 + `node --check` **均漏**，landing-verifier 首轮 FAIL 才接住；见该批 landing-verdict 首轮 FAIL 红项与 retro §3-F2/F2b、§4-2）。

**承载面实测（亲核）**：`ls .claude/references/` 输出 **24 件**，**无 `path-form-family.md`**。

**根因**：八形态的知识在那一批的 retro 与 verdict 里，**未回收成 reference**；下一批做路径迁移时无据可查、重新撞一遍。

## 2. 改法（T1–T3）

> **同文件落地顺序钉死**：T1 与 T2 都改 `.claude/scripts/landing-fidelity-check.mjs`（T1 改 `:90` 的候选域、T2 加新 CLI 入口），**两者改动区不重叠**；T2 另改 `docs/templates/PROPOSAL.template.md` 起草 checklist（**T3 的族谱指针也由 T2 一并落**，见 T3 说明），故 **T3 不重复改该文件**。

### T1 · `landing-fidelity` citation 候选域补 `.claude/references/` 与 docs 子目录形态

承载 `ARQ-260914-lfc-citation-references-prefix` + `ARQ-260907-lfc-citation-docs-prefix`（残余①）。

改 `.claude/scripts/landing-fidelity-check.mjs:90` 的前缀候选域：在现有四前缀外补 `.claude/references/`（治残余①）**并**补 docs 子目录形态（治残余②——候选域现只有 `docs/` 根，需覆盖 `docs/templates/`、`docs/notes/` 等子目录下的裸名引用）。

**实现取并集形态（R1-F1 采纳——v0.2 的「一级子目录派生」按字面会丢前缀，已修正）**：候选域 = **现有四前缀原样保留**（`.claude/scripts/` / `.claude/scripts/lib/` / `.claude/hooks/` / `docs/`，**防回归**）**∪ 派生集**（`docs/` 与 `.claude/` 树下**递归全部目录**）。**v0.2 的失准**：原写「取目录扫描派生**而非**再加硬编码前缀」，判官按字面实现并全仓 28 份 arch PROPOSAL 端到端复跑，实测**丢两个现有前缀**——`docs/` 根（**根不是子目录**、一级派生取不到）与 `.claude/scripts/lib/`（**两级路径**、一级派生取不到），致**已消警引据回退为 file-not-exist**（R2-F10 计数更正：v0.6 照抄了 R1 的算术 slip「11 条」——**精确清点为纯一级派生 8 条去重（9 次出现）+ 纯递归派生 4 条**（仅 `docs/` 根族，因递归天然含 `lib`）；判官已如实更正，**结论不受影响**：改并集后实测**回退 0**、结构门 problems **68→26**、两处点名残余**均 resolved**），与本 T「**在现有四前缀外补**」「只消假警报」的 riskTier 证成直接冲突。改用并集形态后：现有四前缀**零回退**、派生集覆盖新增目录。**派生前缀集钉字典序排序**（R1-F5 采纳；实测同名文件跨前缀碰撞现为零，排序为确定性兜底）。自检（`runCitationAnchorsSelfCheck`，`:226` 已接线）：**两新增夹具**（`.claude/references/` / `docs/templates/` 裸名）+ **两防回归夹具**——后者与电池**既有**两件同形态（`docs/GOV.md`、`scripts/lib/fixture.mjs`），故此处口径为「**确认保留既有件并具名**」而非「新增」（R2-F12 采纳）；四项均须 resolved 非空、零 problems。

**消警留痕形态保持**：仍经 `resolvedByPrefix` 独立观察列输出，**不静默吞**——扩候选域只减少假警报，不改变「解析到了就留痕」的可见性。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改）

### T2 · `landing-fidelity-check` 增 `--preflight <PROPOSAL>` 冻结前干跑模式

承载 `ARQ-260914-landing-preflight-anchor-parse`。

改 `.claude/scripts/landing-fidelity-check.mjs`：新增 `--preflight <proposal-path>` 入口——**只解析、不对账**。行为 = 对给定 PROPOSAL 调 `parseSection2AnchorLists`（`.claude/scripts/lib/governance-gate-shared.mjs:345` 已具名导出，**本 T 只消费、不改 lib**），把**两类错**在起草期报出来（原第 3 类已按 human 熔断裁定拿掉，见 §6 留根⑥）：

1. **加粗锚行** `**目标文件**：`（解析器只认裸形态）——它在 parser 侧表现为「**缺锚清单**」；干跑**另加一条专用探针**把该形态**具名报出**，免与真缺锚混同。**探针须行首锚定 + 限 §2 节域**（R2-F8 采纳：v0.6 写「正则扫字面」按字面实现会**误咬散文**——本 PROPOSAL 自身即命中 **5 次**、全为**行内引用该错误形态的 prose**（§1 问题 2 的记载、§5 V4 的夹具描述），V3-② 首跑必误红；行首锚定 + 限 §2 域实测 **0 命中**为正确态。**R4-N-b 时点注**：该次数系 R2 时点值，R4 重测本件为 **8 处**（仍是全行内、语义不变）——**判据是「全为行内 / 非行首」而非具体次数**，落地时勿把次数写死进夹具）。自检补一条「散文引用该形态不误报」的反例夹具；
2. **无锚清单**（含**整行缺失**——问题 2 的第三种、也是最彻底的一种）；
**原第 3 类「锚并集含仓内不存在的路径」——已按 human 2026-09-19 熔断裁定整条拿掉**（详见 §6 留根⑥）：该类自 v0.2 起被连轮指出会**对合法情形误报**——F7（新增锚本就不在场）、F13（迁移批的**迁入路径**由落地动作诞生、起草期定义上不在场，而 `preflight` 恰恰是起草期工具，故**任何标准迁移批都会被判红**）；三轮评审每轮各拓一类，说明该类的**判据设计**（对锚并集做仓内存在性检查）建立在「不存在的即幻影」这一**不成立的假设**上——§2 锚清单里的路径**合法地**就可能不在场（新增 / 迁入 / 撤回 / 迁走四形态）。故本 T 缩圈为只保留下面两类**判官三轮验证干净**的错。

目标是让**保留下来的两类错各有机制**（两类均已判官实证：第 1 类行首锚定 + 限 §2 域后本件 0 命中、真加粗锚行必命中；第 2 类由 parser 原生 `problems` 直接给出）。

**不做** §2↔manifest 比对（那是 landing 期的事，前置干跑只保证「形态可解析」）。

同批在 `docs/templates/PROPOSAL.template.md` 的**「**【硬项 2a · 目标文件锚清单】」段**（R1-F6 采纳：v0.2 泛称「起草 checklist」，而该文件内的实际可落点即 §2 导语与硬项 2a 段）补两句：① 「FROZEN 标注前跑一次 `--preflight`，并通过 §5 验证集命令实跑」（治问题 2 的**连带项**——验证集命令未实跑的口径瑕疵）；② 「路径类变更先查 T3 的族谱表」（**该项即 T3 的 checklist 落点，故 T3 不重复改本文件**）。

为什么这样改：直击问题 2 的根因——两类错都能被**已导出**的纯解析函数 / 一条行首锚定的探针在起草期抓获，而解析函数今天只在 landing 期被调用；连批三次的返工（含一次 human 格式追认）中，第 1/2 两类可全免。**本 T 不新增工程门**：`--preflight` 是起草期只读自检、**不参与任何门判定、不在任何闸链里被调用**。**缩圈后的覆盖面如实声明**：原设想的第 3 类（幻影路径）**不在本 T 覆盖内**——它需要另行设计（见 §6 留根⑥）。

**入口接线注意**：现 CLI 以 `argv[2]`/`argv[3]` 取位置参数（`:50`/`:52`）、`--self-check` 以 `process.argv.includes` 判（`:226`）。新增 `--preflight` 须与既有位置参数形态**互不干扰**（干跑模式下不消费 `argv[2]` 为 reviewDir），实现时须显式处理优先级并在 `--self-check` 电池里加一条「干跑模式下不误触 reviewDir 解析」的断言。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改）、`docs/templates/PROPOSAL.template.md`（改：起草 checklist 补两句）

### T3 · 新增 `.claude/references/path-form-family.md`（路径文本表示形态族谱）

承载 `ARQ-260915-path-form-family-reference`。

新增 `.claude/references/path-form-family.md`：把 `260914-gov-review-delivery-restructure` 全环实测的**八类形态**各自配对「残留清扫子检模式」——

| # | 形态 | 说明 |
|---|---|---|
| 1 | 斜杠形 | 预期正形 |
| 2 | 分段单引号 | `'changes', 'INDEX.md'` |
| 3 | 分段双引号 | **任意段值**，含 `'changes', 'INDEX.md'` 类非三段值 |
| 4 | Python join | `"changes" / "dev"` |
| 5 | C 双引号实参 | |
| 6 | 相对链接形 | `../../changes/` |
| 7 | 正则转义两子形 | `\.planning` / `changes\/` |
| 8 | **点号成员访问粘连形** | `taskPackage.deliveryMilestoneStateChanged` |

加三类批量替换风险注记：**幂等双应用**（压缩形两轮叠加）、**点前缀规则误吃成员访问**（第 8 类的成因）、**转义形不可见**（第 7 类，正则里看不见、`node --check` 抓不到）。**另附该环损伤总账（R1-F3 采纳更正）**：该环四类损伤合计 `18+10+1+24`，**四数分属四类、不可整体归因给幂等双应用**——`18` = 粘连形 / `10` = 转义形 / `1` = 幂等双应用 / `24` = V1 自咬（v0.2 把四数整体挂在「幂等双应用」下，属误归因）。同时写明**第 6 类的实现-判据不一致教训**（卫兵字符类排除 `/` 而 V1 判据 `[^a-z]` **包含** `/`——实现与判据不一致，是「同一批里两种口径打架」的活例）。

**本 T 不改 `PROPOSAL.template.md`**——其族谱指针由 **T2 一并落地**（见 T2 说明），避免同文件两处改动。

为什么这样改：直击问题 3 的根因——八形态的知识在 retro 与 verdict 里，回收成 reference 后下次路径迁移批不必再付四轮评审。

目标文件：`.claude/references/path-form-family.md`（新增）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 结构门 | `.claude/scripts/landing-fidelity-check.mjs` | G3（**advisory 面**） | T1 候选域扩形、T2 新增只读 `--preflight` 入口。消费方 = landing-verifier、`--self-check`；**二者均不改既有 fail/pass 判据** |
| 路径族谱 | `.claude/references/path-form-family.md` | G2（新增） | T3；无既有消费方，新增后被 `PROPOSAL.template.md` 起草 checklist 引用（T2 落） |
| 提案模板 | `docs/templates/PROPOSAL.template.md` | G2 | T2 checklist 补两句（第二句即 T3 的指针） |
| 契约面 / G1 / agent frontmatter | — | **未触** | 零改动 |
| 工程门新增 | — | **零新增** | T2 的 `--preflight` 是起草期只读自检、不进任何闸链 |

**【硬项 1b · sweep 透镜】** T1 落地新候选域，起草期已预跑原始输出（见 §1 问题 1 的两裸名实测表：`dev-loop-operating-system.md`、`PROPOSAL.template.md` 现候选域均未命中）。**本批不引入新枚举面**，故无 census 需要展开；T2 的新入口为**新增 CLI 形态**，不改变任何既有判定 pattern。

**【硬项 1c · 复扫模式完备性先验】** 本批 T1 涉**路径前缀候选域的完备性**——候选域为**并集形态**（现有四前缀 ∪ `docs/` 与 `.claude/` 树下**递归全部目录**；R2-F9 同步：v0.6 此处仍写「一级子目录派生」、与 §2 的并集/递归形态矛盾，已更正），**完备性论证**：派生源即文件系统实况（递归 `readdirSync` 枚举），不依赖手工清单或词形假设，新增目录自动入集；且**现有四前缀原样保留**保证零回退。T3 的八形态表为**枚举式记录**（非清扫模式集），其完备性边界已在文档内显式声明为「该批实测所见八类，非理论穷举」。

## 4. human 取舍点（OQ）

### OQ1 · T1 的实现形态：目录扫描派生 vs 补两条硬编码前缀

**(a) 目录扫描派生**（**推荐**）。理由：直接治根因——候选域手工清单的漂移正是本问题的成因（260907 补 `docs/` 根、260914 又要补 references，下次还会有新目录）；派生源即文件系统实况，新增目录自动入集。**代价**：候选域变成动态的，判官核验时须理解「派生规则」而非「清单内容」。
**(b) 补两条硬编码前缀**（`.claude/references/` + `docs/templates/`）。理由：改动最小、判官一眼可核、与 260907 热修同形态。**代价**：下次加目录还得手工补——即「漂移形态复发」。
**(c) 折中**：目录扫描派生 + 在文件头注释里写明派生规则与其边界（扫到几级、排除哪些目录）。

**这项为什么列 OQ**：选 (a)/(c) 会让该脚本的候选域从「静态清单」变成「按仓结构派生」，属**行为形态变更**（虽仍在 advisory 面）；判官核验的锚点也随之从「清单内容」变成「派生规则」。方向无争议，但形态该由 human 定。

### OQ2 · T3 的八形态表是否封闭

**(a) 显式声明非穷举**（**推荐**）：「该批实测所见八类，非理论穷举；遇新形态时 append」。理由：封闭清单会让第 9 类无据可依，而路径形态确实可能随工具链演进增长。
**(b) 声明为封闭八类**。理由：边界更清晰、更好判官核，但需人工维护且在遇新形态时立刻过时。

### OQ 裁决（human 2026-09-18）

| OQ | 裁决 | 备注 |
|---|---|---|
| OQ1 | **(a) 改成自动派生** | 候选域从实际目录结构派生，不再手工维护清单。(b) 补两条硬编码、(c) 派生 + 文件头注释两案均未采。**human 在裁决前先问清了实际读者**——结构门的执行者是 `landing-verifier`（Track B 第 5 步 landing 核验期，零上下文、只读、只写 verdict），故「核验锚点由清单内容变为派生规则」这一代价已明示并接受。 |
| OQ2 | **(a) 显式声明非穷举** | 八形态表写明「该批实测所见八类，非理论穷举；遇新形态时 append」。 |

## 5. 验证集（landing 前机械前置 + landing 后核验）

**【真空绿规约】** V1/V2 为 `--self-check` 电池，新增夹具须在输出中**具名出现**（防「加了夹具但没接线」的真空绿）；V3 的干跑为**新入口首跑**，须先断言输入非空（被测 PROPOSAL 存在且 §2 非空）。

| 项 | 命令 | 预期 |
|---|---|---|
| V1 | `node .claude/scripts/landing-fidelity-check.mjs --self-check` | 全绿 + 新增 citation 两夹具（`.claude/references/` 与 `docs/templates/` 裸名）具名出现 |
| V2 | 同上电池内的「干跑模式不误触 reviewDir 解析」新断言 | 具名出现且通过（防 T2 新入口与 `:50`/`:52` 位置参数互扰） |
| V3 | `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>` | **两项全清**（缩圈后；R4-N-a 归一：v0.8 原列 ①②③ 中 ① 与 ③ 系同一断言两说）：**① 无缺锚清单**——即 parser `problems` 为空（§2 锚清单形态可解析的等价表述）；**② 无加粗锚行**——专用探针**行首锚定 + 限 §2 域**（R2-F8）零命中。**本件 T3 的新增锚 `.claude/references/path-form-family.md` 不再被任何检查点名**（第 3 类已拿掉，R3 熔断裁定） |
| V4 | **两反例夹具**（缩圈后覆盖保留的两类）：① 对**故意注入加粗形态 `**目标文件**：` 的副本**跑 `--preflight`；② 对**故意抽掉某 T 锚清单的副本**跑 `--preflight` | ① 须报「加粗锚行」（**且散文引用形态不误报**——R2-F8 的反例夹具）；② 须报「缺锚清单」——两者均防干跑恒绿 |
| V5 | **四夹具**现跑（对照 `:90` 现候选域；R1-F4 计数归正：v0.2 写「三夹具」却只点名两条） | **新增两条**：`dev-loop-operating-system.md` / `PROPOSAL.template.md` —— 现候选域**均未命中**、扩形后**均 resolved**。**防回归两条**：`DEV-GOVERNANCE.md`（`docs/` 根族）/ `governance-gate-shared.mjs`（`.claude/scripts/lib/` 族）—— **现候选域本已命中、扩形后须仍命中**（四项前后各留完整输出） |
| V6 | `grep -c "path-form-family" docs/templates/PROPOSAL.template.md` | ≥1（T2 落的族谱指针在场） |
| V7 | `ls .claude/references/path-form-family.md` + `grep -c "^| [1-8] " .claude/references/path-form-family.md` | 文件在场 + 八形态行齐（各 1） |
| V8 | `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` | 全绿；lint 只允许既有告警（`landing-verifier.md` 超长，本批未触该文件） |
| V9 | `landing-fidelity-check.mjs <本批>` 结构门 | §2 锚集 ≡ manifest 机判通过（本批 3 路径） |

## 6. 留根清单

**不关的根**：

1. **`ARQ-260907-lfc-citation-docs-prefix` 的「候选域维护策略」子项**——本批 T1 取**并集形态**（现有四前缀保留 ∪ 递归派生）已实质回应：**新增目录不再需要手工补前缀**（派生自动覆盖）；但**现有四前缀仍是显式保留的**（防回归），故「**完全**不再手工维护清单」在并集形态下表述过强（R2-F11 采纳更正）——残余的手工面仅剩「何时可从并集里摘除已无用的旧前缀」。
2. **验证集命令「实跑」的机械化**——T2 的 checklist 第二句要求「冻结前通过 §5 验证集命令实跑」，但**「是否真跑过」无机械判据**（仍是纪律句）。**建议下个动 `--preflight` 的批评估**：是否可让 `--preflight` 顺带实跑 §5 命令并留输出（本批不做，避免新入口承担执行职责）。
3. **`ARQ-260914-lfc-citation-references-prefix` 的 `.claude/scripts/lib/` 子目录形态**——现候选域含 `.claude/scripts/lib/` 一层；若将来出现更深层（如 `.claude/scripts/lib/sub/`）的裸名引用，目录扫描派生应天然覆盖，但**本批未构造该场景的夹具**。随 T1 落地后观察。
4. **拆包族另三批**（`-merge-chain-friction` / `-dispatch-chain-friction` / `-closeout-chain-friction`）——本批不承接。
5. **移出本族的两个 launch 面 ARQ**（输入清单先例取旧、launch 前置清单不核）——留 ARQ 队列后续单独处理。

6. **T2 原第 3 类「锚并集含仓内不存在的路径」——移出本批覆盖面，需重新设计**（human 2026-09-19 熔断裁定；三轮评审实证其判据设计不成立）

   本条的实质产出，须完整交接，否则下一批会重复本批的错误设计：

   - **目标**：治 260912 那条 near-miss——T1 锚行注记逐字回引**已撤**的幻影路径 `.claude/skills/triage-change/SKILL.md`，该 token 进 §2 锚并集后造成 §2↔manifest 假失配（靠 landing-verifier D4 处置意见抓住）。
   - **三轮被证伪的过程**（每轮各拓一类合法误报）：

     | 轮 | finding | 被误报的合法形态 |
     |---|---|---|
     | R2 | F7 | **新增锚**——本批 T3 目标 `.claude/references/path-form-family.md` 是**本批要创建的**文件，起草期定义上不在场 |
     | R2 | F8 | **散文引用**——探针全文正扫 `**目标文件**：` 会命中正文里**引用该错误形态**的 prose（本 PROPOSAL 自身命中 5 次） |
     | R3 | F13 | **迁移锚的迁入路径**——按 parser 语义「迁出不入并集、**迁入入并集**」，而迁入路径由**本批 landing 动作诞生**、起草期不在场；故**任何标准迁移批跑 `--preflight` 都会对全部迁入锚判红**，而迁移批恰是 `preflight` 的目标场景（实证：260913 迁入锚 `docs/archive/spec.md` 即该批落地产物） |

   - **根因定性（关键，勿照抄原设计）**：该类的判据「对锚并集做仓内存在性检查」**建立在一个不成立的假设上**——「不存在的即幻影」。实际上 §2 锚清单里的路径**合法地**就可能不在场，至少四形态：**新增**（本批创建）/ **迁入**（本批落地时诞生）/ **撤回**（已在历史批删撤）/ **迁走**（已迁移到新址）。三轮评审每轮各撞一类，说明**枚举补丁不可收敛**——正确方向应是**从锚注记的语义出发**（parser 已解析出的「新增 / 迁移 / 改 / 无注记」分类若存在则可用），或**放弃存在性检查、改治更上游的因**（如要求锚行注记不引路径 token——这正是 260912 那条 D4 处置意见的原始建议）。
   - **交接给下一批的动作**：先核 parser 是否**已**产出锚注记的分类信息（若否，本类可能需要先扩 parser，那就不是一条 `--preflight` 子检查的量级）；再在其他四形态**各造一条夹具**的前提下设计判据。
   - **本批已落地的部分不受影响**：T2 保留的第 1/2 两类由三轮评审实证干净（第 1 类行首锚定 + 限 §2 域后本件 0 命中、真加粗锚行必命中；第 2 类由 parser 原生 `problems` 直接给出）。

**交日落复盘项**：本批 T2 的 `--preflight` 落地后，建议在 `/loop-retro` 观察——**是否还有 landed 期才暴露的 PROPOSAL 形态错**（即 `--preflight` 未覆盖的第 4 类错）。若有，即该干跑面仍有缺口。

**自律件**：起草期已按硬项 1a 亲核（§1 全部断言带命令与日期）；两裸名实测表为现跑、非转引；`parseSection2AnchorLists` 与 `compareAnchorSets` 的导出行号经 `grep -n` 核实。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-19 - architect

**verdict：BLOCK(F1, F2)**（F3–F6 为 SHOULD/NIT，随修订一并处理）。**无升级 human 项**——F1/F2 均为规格完备性缺陷，修订不动 OQ1(a)/OQ2 裁决方向、不触契约面、不改 review 轨。

**亲核留痕（R1 全量）**：

- 行号断言全对：`landing-fidelity-check.mjs:50/:52/:90/:226`（argv[2]/argv[3]/候选域四前缀/`--self-check` includes 接线）与 `governance-gate-shared.mjs:124/:345/:426`（extractManifestBlock/parseSection2AnchorLists/compareAnchorSets 具名导出）逐行核对相符；`:90` 候选域四前缀 verbatim = `['.claude/scripts/', '.claude/scripts/lib/', '.claude/hooks/', 'docs/']`。
- `ls .claude/references/` = 24 件、无 `path-form-family.md`：实测相符。
- §1 两裸名现状复现（判官自实现 `checkCitationAnchors` 同逻辑对全仓 28 份 arch PROPOSAL 复跑：237 条 citation、现行前缀下 65 条 file-not-exist）：`dev-loop-operating-system.md:130`（260912 批 2 条在册）与 `PROPOSAL.template.md:49`（260907 批 1 条在册）均 file-not-exist——问题 1 两条残余属实；`.claude/references/dev-loop-operating-system.md:130` 与 `docs/templates/PROPOSAL.template.md:49` 实存、行内容逐字相符（前者 = EVENTS_SCHEMA_CUTOFF 闭集 SSOT 行）。
- T3 溯源：八形态表逐条对上 `260914-gov-review-delivery-restructure/retro.md:38`（F2 四连击 + 八形态列举）、`:48`（§4-2 八类 + 清扫子检配对建议）、`:55`（四轮评审 + 首轮 FAIL 接住第 8 形）；第 8 类硬数字 = retro:38「3 脚本 + 1 C 件 18 处」+ landing-verdict:34（`closeout-check.mjs:317` 实例）；「转义形 10 处」= verdict:20/:50；「24 命中自咬」= retro:42（F5）；第 6 类教训（r4 卫兵字符类排除 `/` 而 V1 判据 `[^a-z]` 含 `/`）= retro:38① verbatim。
- 结构字段：landing-manifest 围栏块（3 路径）与 packet-id 行在场；以真实 `parseSection2AnchorLists` + `compareAnchorSets` 对本 PROPOSAL 干跑 = 零 problems、§2 union ≡ manifest（3 路径互恰）。
- T2 接线可行性：`--preflight` 会被 `:50` 的 argv[2] 无条件消费（`reviewDir = resolve(projectDir, '--preflight')` → `:271` existsSync fail），故新入口须在 `:266` 前拦截（`--self-check` 在 `:226` 的先例位置可行）——PROPOSAL「入口接线注意」已如实表述，方向可行。V4 可证伪：加粗形态副本可机械构造（`目标文件：` → `**目标文件**：`），parser 对该形态报「缺锚清单」→ 反例夹具成立。

**Findings**：

**F1（BLOCKING）· T1 派生规则按字面会丢现有四前缀中的两项，回退三类已消警引据**。判官自实现派生逻辑（「`docs/` 与 `.claude/` 下实际存在的一级子目录」→ 前缀集）实测：派生集 = `docs/{archive,contracts,notes,templates,ui}/` + `.claude/{agents,commands,hooks,loop-retro,references,route-fixtures,rules,scripts,skills,verify-logs}/`，**不含 `docs/` 根、不含 `.claude/scripts/lib/`**（一级派生只产 `<root>/<sub>/` 形态）。端到端复跑（28 份 arch PROPOSAL：现行四前缀 65 条 problems → 纯一级派生前缀 35 条 problems）实测**新增回退 11 条已消警引据**：`DEV-GOVERNANCE.md:280/:293/:291/:22`（docs 根裸名，260907 热修所治形态复发）、`governance-gate-shared.mjs:34/:386/:410`、`acceptance-id-shared.mjs:36`（lib 裸名，260907 批 T2 所治形态复发）。这与 T1 正文「**在现有四前缀外补**」、riskTier 证成「T1 只扩 advisory 前缀候选域（只消假警报、不放松任何判据）」、§3 sweep 表「二者均不改既有 fail/pass 判据」直接冲突，并证伪 §6 留根 3「更深层……目录扫描派生应天然覆盖」（一级规则下不成立）。**修法方向**：派生规则改为保持现有覆盖面的形态——「`docs/` 与 `.claude/` 树下全部目录（递归）∪ `docs/` 根」或「现有四前缀 ∪ 派生集」并集（后者仍含手工清单成分，与 OQ1(a)「不再手工维护清单」精神需对齐，判官倾向前者）；`runCitationAnchorsSelfCheck` 补防回归夹具：docs 根裸名 + lib 裸名各一条（现行电池里已有 docs 根夹具——纯一级派生若真实现，该夹具本会自曝，须确认不被顺手删掉）。

**F2（BLOCKING）· T2 第三类错（锚行注记夹带路径 token）在「只解析、不对账」约束下无检测机制，V3 预期输出与声称行为矛盾**。亲核 `parseSection2AnchorLists`（lib `:345-423`）：problem 族仅 缺锚清单 / 歧义锚 / 或锚未决 / §2 未找到 四类；锚行注记区 backtick 路径 token 会被当正常多路径锚并入 union、零 problem（260824 批正例自检的「多路径锚」即合法形态）。故 T2 声称的三类错里，第 1 类（加粗）在 parse-only 下只能以「缺锚清单」面目出现（非独立「加粗」报错），第 3 类**完全无机制**；而 V3 预期「报……无加粗锚行、无缺锚清单、注记零路径 token」三项中两项无指定可实现机制。260912 实案是幻影路径（文件不存在），可用「union 各路径存在性检查」抓；但真实存在的路径写进注记同样污染 union、只有对账能抓——而 T2 明文排除对账。**修法方向**（三选一，主线程定）：① preflight 增 union 路径存在性检查（治幻影路径）+ 对起草期已在场的 manifest 块做 **advisory 级**差集报告（「不做比对」收窄为「不因失配 fail」）；② 加粗形态补独立探针（锚行正则 `**目标文件**：` 形态点名）；③ 从 T2 声称面撤掉第 3 类、如实留给 landing 期。当前文本自相矛盾，冻结前必须归一。

**F3（SHOULD）· T3 风险注记把「18+10+1+24」误归因给幂等双应用**。原文「幂等双应用（压缩形两轮叠加，实测 18+10+1+24 处损伤）」——该四数实为整环四类损伤/误咬之和（ARQ-260915 原文「landing 四轮修复（18+10+1+24 处损伤/误咬）」）：18 = 成员访问粘连（retro:38）、10 = 转义形（verdict:20/:50）、1 = 双应用（retro:38②）、24 = V1 自咬（retro:42 F5）。挂在「幂等双应用」名下会让未来读者把 43 处全当双应用防。修法：数字族挪到八形态表的环级损伤汇总处，或改注「整环四类损伤/误咬合计，其中双应用 1 处」。

**F4（SHOULD）· V5「三夹具」计数与所名两裸名不符**。V5 行写「三夹具现跑」但只点名 `dev-loop-operating-system.md` 与 `PROPOSAL.template.md` 两条；T1 自检新增夹具也是两条。冻结前归一（改「两夹具」或点名第三条是什么）。

**F5（NIT）· 派生前缀集建议钉确定性排序**。实测当前 12 个前缀池内 .md/.mjs/.json **同名碰撞为零**（first-hit 语义现无实际歧义）；但派生集随 readdirSync 顺序不定，`resolvedAs` 输出可复现性建议字典序钉死。

**F6（NIT）· T2 模板落点措辞**。「起草 checklist」在 `docs/templates/PROPOSAL.template.md` 里无同名节；实际落点应点名「硬项 2a · 目标文件锚清单」段（`:49`）。V6 的 grep 判据不受影响，但落点精确到段可免 landing-verifier 逐文件 diff 对照时的两读。

**同族透镜结论**（本批特别提示的「该 T 是否真能覆盖目标面」三问）：T1 对两条点名残余**确能消警**（派生集含 `.claude/references/` 与 `docs/templates/`，端到端实测两条均 resolved）——但派生规则本体按字面引入回归（F1）；T2 入口接线可行、V4 可证伪，但三类错的声称面与机制不匹配（F2）；T3 八形态表逐条可溯源、硬数字相符，唯数字归因一处失准（F3）。

### R2 - 2026-09-19 - architect

**verdict：BLOCK(F7, F8)**（F9–F12 为 SHOULD/NIT；F1/F3–F6 六项 R1 findings 全部到位）。**无升级 human 项**——F7/F8 均为 F2 机制化改写的精度修订（新增锚豁免 / 探针扫描域钉死），不动 OQ 裁决、不触契约面、不改轨。

**修订到位性核对（R2 聚焦，逐条）**：

- **F1 → 到位**。① 归因核对：v0.6 正文「`docs/` 根不是子目录、`.claude/scripts/lib/` 是两级路径 → 一级派生取不到」与 R1 实测一致（判官 R2 重测：纯一级派生集确实两项皆缺）。② **并集形态实测消回退**：判官按 v0.6 文本自实现（现行四前缀 ∪ `docs/`+`.claude/` 树下递归全部目录，字典序）对全仓 28 份 arch PROPOSAL 复跑——递归派生集 42 项（含 `.claude/scripts/lib/`、不含 `docs/` 根，故并集里 `docs/` 根由硬编码四前缀承担）；**并集形态回退 = 0**（problems 68 → 26），两点名残余 `dev-loop-operating-system.md:130` / `PROPOSAL.template.md:49` 并集下均 resolved。③ 四夹具可证伪：纯派生回归时任一防回归夹具自曝（现行电池里已有同形态两件——260907 热修 `docs/GOV.md` 夹具与 260907 批 T2 `.claude/scripts/lib/fixture.mjs` 夹具，见 F12 注）。
- **F2 → 机制方向对、两处新精度缺陷**（见 F7/F8）。① 可实现性：存在性检查 = 纯 fs 检查、不涉 manifest 对账（约束内 ✓）；加粗专用探针可行（约束内 ✓）。② V3 四项项项有机制——但 **④ 的预期值对本 PROPOSAL 自身不可满足**（F7）、**② 的探针按字面全文跑会误咬本 PROPOSAL 自身**（F8）。③ 假阳性实核（判官对全仓锚并集跑存在性检查）：**有**——本批 1/3 锚为新增目标（`.claude/references/path-form-family.md` 现不存在）；另 260908-gov-review-human-deputy-shadow（撤回批，2 锚不存在）、260830-arch-review-ui-impl-strategy（`docs/spec.md` 已迁 `docs/archive/`、旧路径不在场）、260914 批（19 锚为 firmware/server 面路径、不在 gov worktree 分支——`firmware/main/test/` 在本 worktree 存在但那 19 件具体文件不在本分支）。「合法引用当前不存在路径」的形态真实且多样，须分形态处置（F7 修法）。
- **F3 → 到位**：四数分属（18 粘连 / 10 转义 / 1 幂等双应用 / 24 V1 自咬）与 R1 溯源逐条一致；幂等双应用注记已不带数字族。
- **F4 → 到位**：V5 改四夹具并点名两防回归（`DEV-GOVERNANCE.md` / `governance-gate-shared.mjs`）+「现候选域本已命中、扩形后须仍命中」前后输出留痕。
- **F5 → 到位**：字典序钉死已写入 T1。
- **F6 → 到位**：`docs/templates/PROPOSAL.template.md:49`「**【硬项 2a · 目标文件锚清单】**」段实存、为可落点；commit 4abb1c9 实查仅改本 PROPOSAL.md 一件（模板未动，其头部「R1/R2 评审修订」字样系 260802 批既有文本）。
- **结构核对**：manifest（3 路径）仍 ≡ §2 锚并集（真实 parser 干跑零 problems、`compareAnchorSets` aligned=true）；§7 R1 记录原样保全；§2 导语「同文件落地顺序钉死」与 v0.6 无新矛盾。

**Findings（新）**：

**F7（BLOCKING）· F2 第 3 类机制未豁免「新增」锚——V3-④ 对本 PROPOSAL 自身不可满足**。判官以 v0.6 机制模拟（并集每条路径做仓内存在性检查、无豁免）对本 PROPOSAL 干跑：并集 3 路径中 `.claude/references/path-form-family.md`（T3 新增目标、现不存在）即被点名——V3-④「锚并集零『仓内不存在路径』」**首跑必红**。这不是边缘例：preflight 是**起草期**工具，而 PROPOSAL 锚清单的常规形态就含「新增」目标文件（本批即 1/3）；全仓实核另见撤回批、已迁移旧路径（260830 `docs/spec.md`→archive）、跨分支锚（260914 的 19 件 firmware/server 面）三类合法「当前不在场」形态。**修法**：存在性检查按锚注记分形态——「新增」锚**反向断言**（已存在 = 冲突报错，向严；或至少豁免不报）；「改」锚不存在即报（真错——260830 旧路径类即此）；跨分支不在场降 advisory（如实留痕、不 fail）。V3-④ 预期同步改写为「新增锚按反向断言、改锚零不存在」。parser 已有「迁出/条件」注记裁剪先例，注记键「新增」可循同款形态接入。

**F8（BLOCKING）· 加粗专用探针未钉扫描域——按字面全文正扫会误咬散文引用（260914 F5 自咬家族同型）**。v0.6 写「正则扫 `**目标文件**：` 字面」：判官实测，全文本正扫 `\*\*目标文件\*\*\s*[:：]` 在**本 PROPOSAL 自身**命中 5 次、**全部为散文行内引用该错误形态**（§1 问题 2 证据 1、T2 第 1 类等处——即 V3-②「无加粗锚行」对本 PROPOSAL 首跑即误报红）；行首锚定（`^\s*>\s*\*\*目标文件\*\*\s*[:：]`，镜像 parser 锚行识别形态）实测 0 命中 = 正确。**修法**：探针钉**行首锚定 + 限 §2 节域**（与 `parseSection2AnchorLists` 的解析域一致），并在 `--self-check` 电池里加一条「散文行内引用错误形态不误报」的反例夹具（本 PROPOSAL 自身即现成素材）。

**F9（SHOULD）· §3 硬项 1c 未随 T1 并集形态同步**。1c 仍写「模式集为**目录扫描派生**（从 `docs/` 与 `.claude/` 下实际存在的一级子目录派生）……`readdirSync` 一级子目录枚举」——与 §2 T1 v0.6「现有四前缀 ∪ 递归全部目录」直接矛盾（一处一级派生、一处递归并集）。冻结前把 1c 的模式集描述同步改为并集形态，否则 landing-verifier 逐文件 diff 对照时两读。

**F10（SHOULD）· 回退计数「11 条」失准（判官 R1 自身算术错误、v0.6 照抄传播，如实更正）**。判官 R2 重测精确清点：**纯一级派生**回退 = **8 条去重 / 9 次出现**（`DEV-GOVERNANCE.md:280/:293/:291/:22` ×4、`governance-gate-shared.mjs:34/:386/:410` ×3 去重〔`:34` 出现 2 次〕、`acceptance-id-shared.mjs:36` ×1）；**纯递归派生**回退 = **4 条**（仅 DEV-GOVERNANCE docs 根族——递归天然含 `.claude/scripts/lib/`，lib 族零回退）。R1 finding 枚举了 8 条却写「11 条」，v0.6 正文与 commit message 均照抄。结论不受影响（回退真实存在、并集形态实测归零），但数字须更正入文（建议注「R2 计数勘误，原记 11」）。

**F11（NIT）· §6 留根 1「不再手工维护清单」表述过强**。并集形态下现有四前缀仍是硬编码保留（防回归锚）；实际语义 = 派生集自动生长、手工核冻结为不再增长的防回归锚。建议校准措辞，免与 OQ1 裁决备注「不再手工维护清单」形成口径漂移。

**F12（NIT）· 防回归两夹具与既有电池夹具的关系未写明**。现行 `runCitationAnchorsSelfCheck` 电池里已有同形态两件：260907 热修 `docs/GOV.md` 夹具（docs 根族）与 260907 批 T2 `.claude/scripts/lib/fixture.mjs` 夹具（lib 族）——纯派生回归时任一自曝，断言可证伪 ✓。建议 v0.6 的「补两防回归夹具」写明为「**确认保留既有两件并具名防回归语义**」（沿临时目录 hermetic 形态），避免落地时重复造轮或误删；若改用真实仓名（`DEV-GOVERNANCE.md` / `governance-gate-shared.mjs`）做夹具则须走 `projectDir`（非 hermetic、依赖仓实况），判官不推荐。

### R3 - 2026-09-19 - architect

**verdict：BLOCK(F13)**（F14 为 NIT）。R2 的 F7/F8 修订对本批**到位**、V3 四项对本 PROPOSAL 已全部可满足；但 F7 分形态改写**新引入一处对迁移批的保证性假阳性**（F13），须冻结前归一。无升级 human 项——单条款规格修订，不动 OQ 裁决、不触契约面、不改轨。

**修订到位性核对（R3 聚焦）**：

- **① F7 分形态 → 对本批到位**。判官按 v0.8 规则模拟本 PROPOSAL：两改锚（`landing-fidelity-check.mjs`、`PROPOSAL.template.md`）在场零报；T3 新增锚 `path-form-family.md` 走反向断言、不在场不点名——**V3-④「本 PROPOSAL 的 T3 新增锚不得被点名」可满足** ✓。三类实测误报形态处置核：新增锚 → 反向断言消 ✓；撤回批 260830 类 → preflight 只跑起草批、moot ✓；跨分支 → advisory，判据机械可实现（worktree 不在场 ∧ `git cat-file -e main:<path>` 在场——refs 跨 worktree 共享，判官实测该原语可用；措辞见 F14）。260912 幻影路径（注记区引用、无注记）按「无注记 → 不存在即报」仍判红 ✓ 真阳性保持。
- **② F8 探针 → 到位**。行首锚定 + 限 §2 节域实测：本 PROPOSAL 全文 5 处 `**目标文件**：` 全为**行内** prose 引用、行首锚定 0 命中 = 散文不误报 ✓；真加粗锚行（§2 内行首形态）必命中探针、且 parser 侧同时报「缺锚清单」双保险，V4 反例夹具覆盖 ✓；自检补「散文引用该形态不误报」反例夹具已写入 ✓。
- **③ F9–F12 → 全部到位**。F9：§3 硬项 1c 已改并集/递归形态并自注更正出处 ✓。F10：精算数字逐字一致（纯一级派生 8 条去重〔9 次出现〕+ 纯递归 4 条仅 docs 根族、标注 R1 算术 slip、结论不受影响）✓。F11：留根 1 已弱化为「新增目录不再需手工补前缀；现有四前缀仍显式保留；残余手工面仅剩摘除旧前缀」✓。F12：防回归夹具口径改「确认保留既有件并具名」（`docs/GOV.md`、`scripts/lib/fixture.mjs`）✓。
- **④ 结构与新矛盾**：manifest（3 路径）仍 ≡ §2 锚并集（真实 parser 干跑零 problems、aligned）；§7 R1/R2 原样保全；commit 4b735e9 实查仅改本 PROPOSAL.md 一件。新矛盾 = F13（下）。

**Findings（新）**：

**F13（BLOCKING）· 分形态把「迁移锚」归入「不存在即报」组——对迁移批构成保证性假阳性**。v0.8 第 3 类写「**改锚 / 迁移锚 / 无注记** → 不存在即报」。按 parser 语义（lib `:416`：迁出路径不入并集、**迁入路径入并集**），class-3 检查域里「迁移锚」实际能碰到的只有**迁入路径**——而迁入路径由**本批 landing 动作诞生**、起草期定义上不在场。实证：`260913-arch-review-spec-split` 的迁入锚 `docs/archive/spec.md` 即该批归档动作的产物（落地才存在），其 `.delivery/milestones.jsonl` 等三条迁入锚现亦不在场（后续 260914 再迁移）；260914 批自身就是迁移批。故按 v0.8 规则，**任何标准迁移批跑 `--preflight` 会对其全部迁入锚判红**——恰是 T2/T3 的目标场景（「下次路径迁移批」）。**修法**：迁入锚改走**新增锚同款反向断言**（迁入路径已存在 = 目标位被占冲突，报；不在场 = 正常，不报）；「改锚 / 无注记」保持不存在即报不动（260912 幻影即无注记形态，真阳性不受影响）。

**F14（NIT）· 跨分支判据措辞校准**。v0.8 括注「同批未并 main 的路径」与机械可实现形态有出入——可实现判据 = **worktree 不在场 ∧ `main` ref 同字面路径在场** → advisory。另注（判官 R2 表述自校）：R2 所称 260914 ×19「跨分支锚」经 main 树抽查实为**后续再迁移**（现住 `firmware/main/test/acceptance/archive/` 等字面路径，main tip `210bb6e` 实查）——该家族只存在于历史 landed 批，preflight 只跑起草批不会遇到；起草批真正会遇到的形态是「main 已有、gov 未回同步」的路径，main-ref 查询判据对它成立。建议括注改写为「不在本工作树、在 `main` ref 的路径」。

### R4 - 2026-09-19 - architect

**verdict：GREEN**（熔断缩圈路径，F13 随第 3 类整条移除而消解；两 NIT 级措辞注记随 FROZEN 前顺手归一即可，不构成门）。

**缩圈到位性核对（R4 聚焦）**：

- **第 3 类痕迹清扫**：`grep` 全文「三类错/三个错误类」——§0–§6 **零残留**（§1 根因改「至少两类」+ 另注移出声明、§2 T2 导语/「为什么这样改」/「缩圈后的覆盖面如实声明」齐、V3/V4 已收）；仅存命中在 §7 R1/R2 **append-only 历史评审记录**（:244/:254，按纪律不可改、正当保留）。第 3 类全文以「已拿掉 + 指向留根⑥」的单一口径叙述，无两读。
- **留根⑥ 交接质量**：目标（260912 near-miss 幻影路径）+ 三轮证伪过程表（F7/F8/F13 各一行，与判官记录逐条一致）+ 根因定性 + 建议方向 + 下一批动作 + 已落地部分申明，六件齐。「枚举补丁不可收敛」这层**已讲透**——定性明确指出判据建立在「不存在的即幻影」这一不成立假设上、四形态（新增/迁入/撤回/迁走）合法不在场、并给出两条替代方向（锚注记语义 / 改治上游因——即 260912 D4 原始建议）；「先核 parser 是否已产出注记分类」的下一步动作**措辞诚实**（判官亲核：parser 现只以「迁出/条件」为裁剪关键字读注记、**未产出**「新增/改」分类——留根⑥的「若存在则可用」式对冲是对的，下一批不会拿假前提开工）。
- **保留两类自洽**：V3 收两项后**每条预期有机制**——① parser 原生 `problems`（判官对 v0.8 干跑：零 problems ✓）；② 加粗探针行首锚定 + 限 §2 域（判官对 v0.8 §2 域实测**行首 0 命中**、全文行内 8 处全为 prose 引用 = 散文不误报 ✓）。V4 两反例夹具均可机械构造且各覆盖一类（注入加粗副本 → 探针 + parser「缺锚清单」双报；抽掉锚行副本 → parser「缺锚清单」），「散文引用不误报」反例保留 ✓——干跑不恒绿、可证伪。
- **§3 一致性**：硬项 1b 无「三类」叙述、1c 仍为并集/递归形态，与 §2 一致 ✓。
- **结构字段**：landing-manifest 围栏块（3 路径）与 packet-id 行在场；真实 `parseSection2AnchorLists` + `compareAnchorSets` 对 v0.8 干跑 = 零 problems、§2 union ≡ manifest（aligned=true）✓。§7 R1/R2/R3 原样保全（append-only 保持）✓。commit 7915c94 实查仅改本 PROPOSAL.md 一件 ✓。
- **riskTier/profile 复核**：缩圈后（扩 advisory 候选域 + 只读两类干跑入口 + 新增参考文档）与 low / G3+G2 声明相称，升级判据（契约面 / 跨系统）未命中 ✓。

**NIT 注记（两处，FROZEN 前顺手归一、不构成门）**：

- **N-a**：V3 写「**两项全清**」却列 ①②③，其中 ①（「parser 零 problems」）与 ③（「无缺锚清单——即 parser problems 为空」）为同一断言两说。建议收成两项（parser 零 problems / 探针零命中）或把 ① 标为总括项；预期输出本身无歧义，不影响可判性。
- **N-b**：T2 第 1 类引「本 PROPOSAL 自身即命中 **5 次**」系 R2 时点实测值；v0.8 文本生长后现值为 **8 处**（判官重测，仍全为行内 prose、行首 0 命中——语义主张不变）。建议改为「（R2 实测 5 次，现文本 8 处、均行内）」或去具体数字防再漂。

**结论**：缩圈到位、无新矛盾、结构字段齐、验证集每条可机判可证伪——**GREEN，可 FROZEN**。判官未自评冻结（作者≠判官）；FROZEN 与落地走主线程 packet-bound 常规链。