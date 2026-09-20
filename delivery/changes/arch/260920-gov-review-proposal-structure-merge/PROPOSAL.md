# PROPOSAL · 260920-gov-review-proposal-structure-merge

> **状态：FROZEN v1.0 · 2026-09-20 · 可进 landing（按 human 流程待核验官判后走合并门）**（凭据：human 2026-09-20 会话「立项 ack」+ OQ 全裁〔OQ1 = 保留 §1 空档；OQ2 = 硬项标签去数字〕+ **R2 GREEN**〔§7 `### R2 - 2026-09-20 - architect`，含 2 条 NIT F4/F7——**R2 同时抓到主线程派遣陈述与制品不符两处**（声称已修 F4/F7、实未修），已按实况补修并在本行如实记录〕）。
> **承载 ARQ**：无——human 2026-09-20 会话直接触发（读 260918 批 PROPOSAL 时提出「问题与改法两段分离、阅读成本大」）。收口时按惯例补登记。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。两条升级判据（触契约面 / 跨系统结构性变更）均未命中。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T2）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（一句话证成：纯**文档形态 + 引用标签**改动，**零脚本改动**；机械门 `parseSection2AnchorLists` 的定位三要素——§2 节号、`### T<n>` 条目头、`目标文件：` 裸行——**全部原样保留**）。
> **review profile**：G2（`docs/templates/PROPOSAL.template.md` + `.claude/references/track-b-judge-brief.md` + `.claude/references/residual-scan-patterns.md` + `.claude/references/path-form-family.md` + `.claude/skills/gov-arq/SKILL.md` **五件**，全在 G2 面）。契约面 / G1 / G3 均为零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。
>
> **【本批自指声明】** 本 PROPOSAL **自己就用被提议的新结构写**（§1 空档、§2 五段式条目）——作为形态演示。其机械门相关形态（§2 节号 = 2、条目头 `### T<n>`、`目标文件：` 裸行）与本批改法**同形**，故不自相矛盾：门解析本件与解析改法后的模板，走的是同一条定位链。

```landing-manifest
.claude/references/path-form-family.md
.claude/references/residual-scan-patterns.md
.claude/references/track-b-judge-brief.md
.claude/skills/gov-arq/SKILL.md
docs/templates/PROPOSAL.template.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把 PROPOSAL 的「问题与根因」和「改法」从**两段分离**合成**一段连贯**——每条问题直接跟着它的根因与改法，读者一次读完一条，不必在两节之间来回对位。

**现状差距**：现模板 §1 列问题 1..N（证据 + 根因）、§2 列 T1..Tn（改什么/怎么改 + 目标文件），两段靠读者的**心算对位**连起来。代价在最近的批里可实测：判官简报的评审透镜第 3 条要求逐条核「**根因→改法对应**」（即跨节对位）；人类读者（human 本人 2026-09-20）读 260918 批时明确反馈「两段分开，阅读成本比较大」。

**非目标（本批不做什么）**：不改机械门（`parseSection2AnchorLists` 的定位三要素原样保留）；不改任何脚本；不改 §3–§7 各节的**义务内容**（T2 只改其标签的文字形态，义务语义零变）；不追溯改写既有 29 份 PROPOSAL（它们已 frozen/landed，形态不同不影响——门只在 landing 时解析本批）；不处理「问题 ↔ T 非 1:1」的表达（见 §6 留根③）。

## 2. 问题、根因与改法（T1–T2）

**【硬项 · 起草期实证义务】**（原「硬项 1a」，随 §1 并入本节；标签去数字见 T2）本节的断言纪律：

- 凡 regex / git / 机械门 / 脚本**行为**断言 → 草稿即附实测（命令 + 关键输出 verbatim + 日期）；无实测不写断言，写「未实测」。
- 凡声称「对偶 / 比照 <先例>」→ 先读先例文件，引 file:line verbatim 确认形态再落笔。
- 凡「全 / 均 / 无」量化声明 → 每次改写其覆盖集合时，对新入集成员重新取证并留痕。
- 凡引用 commit SHA → 先 `git rev-parse <sha>` 核实存在，输出留痕。
- 凡数字引用（计数 / 行号 / 版本号等）→ 起草期现跑实测（命令 + 输出留痕）或核对权威原处；不得转引陈旧制品数字，确需转引时显式注出处与复核结果。
- 凡引用脚本 / 闸 / CLI 命令入口（如 `--self-check`、闸调用命令）→ 起草期亲核入口存在（实跑一次，或至少验证该入口在脚本帮助/源码中），不得凭印象构造。
- 搬运 / 改写既有文本中的引据（file:line / 编号 / 数据点）视同新断言复证——版本迭代搬运期的旧引据漂移不得免检。

**【硬项 · 目标文件锚清单】**（原「硬项 2a」；标签去数字见 T2）每 T 末尾显式列「目标文件：<仓根相对路径清单>」（该 T 落地时要改/新增的仓内文件，新增/改分注，条件触发的标触发条件）。packet-binding 完整性断言（landing-manifest ≡ 各 T 目标文件并集）依赖本清单：**缺锚清单 = 门 RED 点名缺锚（fail-loud，不假绿）**。承重 landing 指令（箭头替换的目标文本 / 联动指称）必须原文引用钉死并给出替换后完整句，禁止转述。条目标题一律 `### T<n> · <标题>` 形态——机械门 `parseSection2AnchorLists` 只认 `### T<n>`；landing-manifest 围栏块一行一个裸路径、不带括注（`extractManifestBlock` 逐字取行）。

> **本条目的本条改动**：下列「五段固定式」即 T1 请求模板化的形态，本 PROPOSAL 自 §2 起全部条目已按它书写。

### T1 · 问题与改法两段分离，读一条要来回跳

**问题**：模板把「问题是什么、为什么」与「怎么改」拆在两个章节，读者需要在 §1 与 §2 之间来回对位。对**评审方**（判官逐条核「根因→改法对应」）与**人类读者**（通读一遍判断要不要立项）都是持续成本；且该成本**每份 PROPOSAL 复付一次**。

**证据**（全部亲核，2026-09-20）：
- 模板现状（verbatim）：`docs/templates/PROPOSAL.template.md:31` = `## 1. 问题与根因（地面真值）`、`:45` = `## 2. 改法（T1…Tn）`——两节分离，各自独立成章。
- **对位成本落在判官透镜上**（verbatim，R2-F4 修正：v0.3 原写 `:30`，透镜第 3 条实在 `:29`）：`.claude/references/track-b-judge-brief.md:29` 评审透镜第 3 条 = 「**根因→改法对应**：各 T 是否真治对应问题类；有无错配/漏承接」——该透镜的实现方式就是跨节对位。
- **人类读者反馈**（会话原文，2026-09-20）：human 读 260918 批 PROPOSAL 时提出「现在的结构是『问题根因』…然后到下一个章节又变成『解法』…它俩是分开的…这样我在看的时候，能把问题和解法一块看了。不然两段是分开的，阅读成本比较大」。
- **跨批普遍性**：现存 `PROPOSAL.md` **30 份**（`find . -name PROPOSAL.md` 亲跑，2026-09-20；R1-F5 更正：v0.2 原写 29 份为时点值，且「全部为两段式」**对本件自身不成立**——本件即新结构；故口径应为「**其余 29 份为两段式**」）。

**根因**：模板形制来自「四乘客归纳」（模板 `:6` 形制来源句自陈）。定型时的组织原则是「**先穷举地面真值、再给改法**」——这个顺序对**作者**友好（分两步写：先查清事实，再设计改法），但把**对位**留给了**读者**。而 PROPOSAL 的主要读法是**通读一遍做判断**（立项 ack / 判官评审 / 核验官核对），不是分步查阅——写法顺序与读法顺序错配。

**改法**：
1. 模板 §1 **整体并入 §2**；§2 标题由 `## 2. 改法（T1…Tn）` 改为 `## 2. 问题、根因与改法（T1…Tn）`；**节号保持 `2` 不变**（机械门定位锚之一，改名零代价——门只认 `/^##\s*2[.、]/`）。
2. 每个 `### T<n> · <标题>` 条目改**五段固定式**：

   ```
   ### T<n> · <问题的短名>
   **问题**：<完整陈述——是什么 + 影响面 + 发生频率>
   **证据**：<file:line verbatim / 实测输出 / 制品锚点>
   **根因**：<为什么是这样>
   **改法**：<改什么 + 怎么改>
   〔第五段·锚清单〕目标文件：<裸路径，反引号包裹>（改）· <裸路径>（新增）
   ```

   **读法注（两处形态约束，均系本起草期实测所得）**：
   1. **真实 PROPOSAL 里第五段以 `目标文件：` 起头、不带任何前缀**；上引号块里的 `〔第五段·锚清单〕` 标记是**为避开解析器行为而加**——`parseSection2AnchorLists` **不跳 fenced code block**，若示范行以 `目标文件：` 起头，它会被当作**真锚行**吃进并集、报「歧义锚」（本件首版实测，见 §5 V1）。
   2. **该行不得加粗**——锚行判据是 `/^目标文件\s*[:：]/`（仅 strip 前导 `>` 后按**行首**匹配），写成 `**目标文件**：` 即不被识别 ⇒ 门 fail-loud 报「缺锚清单」。**这正是 `ARQ-260914-landing-preflight-anchor-parse` 记录的原始错误形态**（本件首版把该行写成加粗，`--preflight` 当场报 `bold-anchor-line`）。

   **其余四段加粗只作视觉分隔、不承载机器语义，故可自由。** 这两条合起来是「**形态必须与机械门同形**」的具体落地：**模板给出的示范形态，就是未来每份 PROPOSAL 会照抄的形态**——所以模板里的示范必须与门同形。
3. 条目**标题取问题的短名**（便于扫目录认出是哪条），**正文首行给完整陈述**——两处分工、**不重复**（标题不含影响与频率，正文首行承载它们）。
4. **连带一**：`track-b-judge-brief.md:27` 评审透镜第 1 条「**§1 引述**（编号、次数、数据点）与证据基线逐一核对」→ 引述节号改指 **§2**（合并后 §1 不存在，若不改，该透镜会指向空节）。
5. **连带二（R1-F3 补钉，模板内唯一 §1 残留）**：模板 §2 的引言句（`PROPOSAL.template.md:47`）写「每 T 一节：改什么（文件/段落）+ 怎么改 + 为什么这样改（**对 §1 哪个根因**）。扩承重闸优先、克制新立脚本。」——合并后指向不存在的 §1。**替换后完整句钉死**：`每 T 一节：先写「问题」与「证据」，再写「根因」，再写「改什么 + 怎么改 + 为什么这样改（对哪个根因）」。扩承重闸优先、克制新立脚本。`（该句不含 `目标文件：` 起头，**裸锚行不受影响**）。同批把 §2 T1 的「读法注」两处形态约束（锚行裸起不加粗、解析器不跳引号块）**随五段式一并写进模板正文**，使模板示范与门同形。

目标文件：`docs/templates/PROPOSAL.template.md`（改：§1 并入 §2 + 五段式条目模板）、`.claude/references/track-b-judge-brief.md`（改：透镜第 1 条引述节号 §1→§2）

### T2 · 「硬项」编号绑死节号，节号一动即失真

**问题**：四个【硬项】的标签写作 `1a / 1b / 1c / 2a`——数字部分是**旧节号的镜像**。T1 把 §1 并入 §2 之后，原「硬项 1a」与「硬项 2a」**同落 §2**，编号自相矛盾（`1a` 与 `2a` 并列在同一节内）。若不处理，模板自身即示范了一次「标识符绑死结构位置」的漂移。

**证据**（全部亲核，2026-09-20）：
- 现模板（verbatim）：`:35` = `**【硬项 1a · 起草期实证义务】**`（该条原属 §1）、`:49` = `**【硬项 2a · 目标文件锚清单】**`（属 §2）——`1a` 与 `2a` 的数字分别锚在 §1 与 §2。
- **全仓引用面 census**（命令 `grep -rn "硬项[[:space:]]*1a\|硬项[[:space:]]*1b\|硬项[[:space:]]*1c\|硬项[[:space:]]*2a" --include="*.md" .claude/ docs/` 亲跑）：**共 12 处，分布 5 个文件**，逐处列出：

  | # | 位置 | 命中标签 |
  |---|---|---|
  | 1 | `docs/templates/PROPOSAL.template.md:35` | 硬项 1a（**标签本体**） |
  | 2 | `docs/templates/PROPOSAL.template.md:49` | 硬项 2a（**标签本体**） |
  | 3 | `docs/templates/PROPOSAL.template.md:59` | 硬项 1b（**标签本体**） |
  | 4 | `docs/templates/PROPOSAL.template.md:61` | 硬项 1c（**标签本体**） |
  | 5 | `docs/templates/PROPOSAL.template.md:6` | 硬项 2a（形制来源句内的引用） |
  | 6 | `.claude/references/track-b-judge-brief.md:28` | 硬项 1b |
  | 7 | `.claude/references/residual-scan-patterns.md:3` | 硬项 1c |
  | 8 | `.claude/references/residual-scan-patterns.md:16` | 硬项 1b |
  | 9 | `.claude/references/residual-scan-patterns.md:18` | 硬项 1c |
  | 10 | `.claude/references/path-form-family.md:64` | 硬项 2a |
  | 11 | `.claude/skills/gov-arq/SKILL.md:12` | 硬项 1a **/** 1b **/** 1c **/** 2a（同句四标签） |
  | 12 | `.claude/skills/gov-arq/SKILL.md:34` | 硬项 2a |

- **非本批目标（如实排除并说明）**：`docs/templates/retro.template.md` 的「硬项 R1…R5」**是另一套编号**（retro 的硬项按 R 编号、**不绑节号**），不属本批；`docs/ARCH-REVIEW-QUEUE.md` 内的「硬项 R2」同类。二者的存在恰好说明：**R 编号因不绑节号而从未漂移，1a/2a 因绑节号而在 T1 的合并上即刻自相矛盾**——这是本 T 的直接对照证据。
- **同型先例**：本仓已有「标识符绑死会漂移的位置 → 位置一动即失真」的实例——在册 `ARQ-260909-gate-green-anchor-regex-loose` 的病灶即「注释声明的意图与实现脱节」。

**【硬项 · sweep 透镜 · 本批自证】** 上表首版 census **写错过一次，如实记录**：初稿用 `grep … | grep -v "PROPOSAL.template.md"` 想排除**模板文件**——但 `grep -v` 过滤的是**行内容**，凡行文提及 `PROPOSAL.template` 的**整行**都被滤掉，于是 `path-form-family.md:64`、`gov-arq/SKILL.md:12`/`:34` 三行（各自正文里都引了模板路径）**被静默吞掉**，初稿因此误报「外部引用 4 处 / 2 文件」。改用不带 `-v` 的完整模式重跑得 12 处 / 5 文件。**这正是本仓「枚举 census 须注明命令、遇省略须逐条展开」（硬项 · sweep 透镜义务）要防的形态——过滤口径写错 = 静默漏项。**

**作用域声明（R1-F6）**：本 census 的作用域 = `.claude/` + `docs/`（`--include="*.md"`）；`delivery/changes/arch/**` 历史制品内的旧标签（数十处）为 frozen/landed 记录、**不追溯改**——排除是有意处置而非遗漏。

**根因**：标签的设计意图是「第几节的第几个硬项」——把**位置**编进了**名字**。凡名字含位置信息者，位置变则名字失真；而修复（改数字）会**逐个波及引用点**，引用点越多越贵。

**改法**：四个硬项**去掉数字前缀**，改为纯功能性标签，使名字只描述**义务内容**、不含位置：

| 现 | 改后 |
|---|---|
| 【硬项 1a · 起草期实证义务】 | **【硬项 · 起草期实证义务】** |
| 【硬项 2a · 目标文件锚清单】 | **【硬项 · 目标文件锚清单】** |
| 【硬项 1b · sweep 透镜义务】 | **【硬项 · sweep 透镜义务】** |
| 【硬项 1c · 复扫模式完备性先验】 | **【硬项 · 复扫模式完备性先验】** |

并同步**上表列出的全部 12 处**（跨 **5 个文件**：模板本体 4 处标签 + 模板 `:6` 引用 + judge-brief + residual-scan-patterns ×3 + path-form-family + gov-arq ×2）。**义务语义零变**，只动标签文字。**gov-arq 的 `:12` 一行含四个标签**，须逐处改（不是整行替换）。

目标文件：`docs/templates/PROPOSAL.template.md`（改：四标签去数字 + `:6` 引用去数字）、`.claude/references/track-b-judge-brief.md`（改：硬项 1b → 硬项）、`.claude/references/residual-scan-patterns.md`（改：硬项 1b ×1 + 硬项 1c ×2 → 硬项）、`.claude/references/path-form-family.md`（改：硬项 2a → 硬项）、`.claude/skills/gov-arq/SKILL.md`（改：`:12` 四标签 + `:34` 一标签 → 硬项）

**【起草期自检两句】**（260918-gov-review-batch-drafting-friction T2 钉死，随 §2 保留）① **FROZEN 标注前跑一次 `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL 路径>`**——只读干跑，报两类**形态**错（加粗锚行、无锚清单含整行缺失），不参与任何门判定、不进任何闸链；**并通过 §5 验证集命令实跑**。② **路径类变更先查 `.claude/references/path-form-family.md` 的形态族谱表**。

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 提案模板 | `docs/templates/PROPOSAL.template.md` | G2 | §1 并入 §2 + 五段式条目模板 + 四标签去数字。消费方 = 每一份新 PROPOSAL 的作者 |
| 判官简报 | `.claude/references/track-b-judge-brief.md` | G2 | 透镜第 1 条引述节号 §1→§2 + 硬项 1b 去数字。消费方 = 每环判官派遣 prompt |
| 扫描模式参考 | `.claude/references/residual-scan-patterns.md` | G2 | 硬项 1b ×1 + 1c ×2 去数字（`:3`/`:16`/`:18`）。消费方 = 涉退役语面清扫的批次作者 |
| 路径形态族谱 | `.claude/references/path-form-family.md` | G2 | `:64` 的「硬项 2a」去数字。消费方 = 路径类变更批次作者（该文件 260919 刚由 P4 落地） |
| 治理 ARQ skill | `.claude/skills/gov-arq/SKILL.md` | G2 | `:12`（同句四标签）+ `:34` 去数字。消费方 = 登记 ARQ 的主线程。**注意 `:12` 一行含四个标签、须逐处改** |
| **机械门** | `.claude/scripts/lib/governance-gate-shared.mjs` | **未触** | `parseSection2AnchorLists` 定位三要素（`/^##\s*2[.、]/` + `/^###\s+(T\d+…)/` + `目标文件：` 裸行）**原样保留**；其自检夹具 11 处 `'## 2. 改法'` 字面**不受影响** |
| **脚本面 / 契约面 / G1 / agent frontmatter** | — | **未触** | 零改动 |

**【硬项 · sweep 透镜义务】** 本批**不新增门、不改判定 pattern**，故无 pattern 预跑义务。**但枚举义务已履行**（T2 改的是标识符，其引用面正是 census 类）：两个标识符族的**全仓引用面**已机械枚举，命令与结果逐处列在 T2 的「证据」段（`grep -rn` 亲跑）。另枚举了本批**不受影响**的关联面以证「未触」：`parseSection2AnchorLists` 的消费方实测**仅 `landing-fidelity-check.mjs` 一个**（`:39` import + `:173`/`:192`/`:458` 三处调用；通查 `.claude/scripts/proposal-consolidation-check.mjs` **零命中**，故本批无第二个消费方要改）。

**【硬项 · 复扫模式完备性先验】** 本批涉**标签改名**（一种残留清扫）。模式集显式枚举全形态：`硬项 1a` / `硬项 1b` / `硬项 1c` / `硬项 2a` × 有 / 无空格 × 裸文本 / backtick 包裹。**完备性论证**：四标签是**固定字面**——不涉词形变体（无中英转换、无单复数、无缩略），故 grep 模式 `硬项 1[a-c]\|硬项 2a` 的命中面**即**完备面；实测命中 **4 处**，已逐处列出。

## 4. human 取舍点（OQ）

**两处 OQ 已由 human 2026-09-20 会话裁定「同意推荐位」**，记录如下（随立项 ack 一并确认）：

| OQ | 选项 | 裁决 | 理由 |
|---|---|---|---|
| OQ1 · 合并后节号出现 `0 → 2 → 3…` 空档，如何处理 | (a) **保留空档**（§0 当导语）；(b) 把 §0 改名 §1 消掉空档 | **(a) 保留空档** | 两者机械代价相同（都要改 judge-brief 一行）；而「§0 = 导语」是本仓既有惯例（`dev-loop.md` / `change-triage.md` / `ARCH-REVIEW-QUEUE.md` / `ui-implementation-strategy.md` 均以 §0 起）。为一个纯粹观感空档而破惯例不划算 |
| OQ2 · 硬项标签是否去数字 | (a) **去数字**（纯功能标签）；(b) 保留数字并重排 | **(a) 去数字** | 数字前缀把「位置」编进了「名字」——T1 的合并正撞在这上面（`1a` 与 `2a` 同落 §2 即自相矛盾）。去数字一次性解除耦合；外部引用面（模板外）**7 处 / 4 文件**，成本已实测。**R1-F2 更正**：v0.2 原写「仅 4 处」系首版 `grep -v` 误计值，按更正后 census 更正（裁决实质不受影响） |

**无待裁 OQ。**

## 5. 验证集（landing 前机械前置 + landing 后核验）

| 项 | 命令 | 预期 |
|---|---|---|
| V1 | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260920-gov-review-proposal-structure-merge/PROPOSAL.md` | `ok:true` / `findings:[]` / exit 0——**本件即新结构，须自证形态可解析** |
| V2 | `node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260920-gov-review-proposal-structure-merge` | `ok:true`；`frozen`/`packetBinding` 真；`proposalTargets` ≡ `summaryTargets`（**5 路径**——R1-F1 更正，v0.2 原写 3 为 census 更正前陈旧值）；`unauthorized`/`claimedNotLanded`/`plannedNotLanded` 全空 |
| V3 | `node .claude/scripts/landing-fidelity-check.mjs --self-check`（**R1-F8 更正**：`governance-gate-shared.mjs` 无 CLI 入口、不可直接复跑；改经 landing-fidelity 的自检电池承载） | `section2Anchors` 电池**全绿且与改动前零差异**（R1 判官基线 = **19/19**）——证机械门未受本批影响 |
| V4 | `grep -rn "硬项[[:space:]]*1a\|硬项[[:space:]]*1b\|硬项[[:space:]]*1c\|硬项[[:space:]]*2a" --include="*.md" .claude/ docs/` | 落地后 **零命中**（T2 证据表列的 **12 处**全改完）。**注意勿加 `grep -v` 过滤**——本件首版即因该过滤按行内容误吞三行（见 T2 的「本批自证」段） |
| V5 | `grep -c "§1 引述" .claude/references/track-b-judge-brief.md` + `grep -c "§2 引述" …` | `§1 引述` = **0**、`§2 引述` = **1** |
| V6 | `grep -c "^## 1\." docs/templates/PROPOSAL.template.md` + `grep -c "^## 2\." …` | `## 1.` = **0**（§1 已并入）、`## 2.` = **1**（节号保留） |
| V6b | `grep -n "§1" docs/templates/PROPOSAL.template.md` | **零命中**（R1-F3 补：模板内唯一 §1 残留 = :47 引言句，随 T1 连带二替换后归零） |
| V7 | `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` | 全绿；lint 只允许既有告警（`landing-verifier.md` 超长，本批未触该文件） |

**真空绿规约核对**：V3 为「**零差异**」型断言（工程门未受影响的正面证据，而非「跑过了就算」）；V4/V5 为**改名类**——须断言**旧标识符零命中**，不能只断言新标识符在场（否则改名没改净也是绿）；V1 为**自指**型——本件用新结构写，故它同时充当形态夹具。

## 6. 留根清单

- **留根① · 既有 29 份 PROPOSAL 不追溯改**：它们形态为旧两段式。机械门只在 landing 时解析**本批**，故不影响功能；但**判官读旧件**时仍会看到两段式。**交日落复盘项**：观察下一批判官是否因形态不同而产生误读或额外对位成本。
- **留根② · §5 验证集与 §3 sweep 对「T 编号」的耦合未动**：本批只改 §2 条目**内部**布局，未动 T 编号本身；§5 的 V 编号仍按 T 编号。**若将来把条目单位从 `T<n>` 换成别的形态**（本轮讨论中称为「乙′」的那条路），§3/§5 要一并改键——**本批不预批**。
- **留根③ · 「问题 ↔ T 非 1:1」的表达未处理**：实测 260918 四批**冻结终态**均为 1:1；但起草期一度出现 2:1（那是「T 被移出后重编号」的过渡态，非常态）。若将来出现真 2:1，需定「一条 T 写两段问题」还是「拆成两条 T」——**本批不预设**。
- **留根④ · 起草期自检工具的两处精度缺陷（本批起草期实测发现，不承接）**：

  1. **`--preflight` 报的行号是 §2 域内相对行号，非文件行号**：本件首版被报 `bold-anchor-line, lines:[38]`，而该行在**文件**第 68 行——`preflightFindings()` 的 `boldLines.push(i + 1)` 里 `i` 是 **§2 节域切片内的下标**，未加节域起始偏移。后果：作者按报出的数字去文件里找**找不到**（要心算 `文件行 − §2 起始行`）。
  2. **`parseSection2AnchorLists` 不跳 fenced code block**：本件首版在 §2 的引号块里示范 `目标文件：` 行，被解析器当作**真锚行**吃进并集、报「歧义锚」（`T1 … 「目标文件：」行无 backtick 包裹路径`）。后果：**PROPOSAL 的 §2 内无法示范锚行的字面形态**——本件最终改用 `〔第五段·锚清单〕目标文件：…` 前缀标记绕开（见 §2 T1 的「读法注」）。**R2-F7 补第二形态（判官夹具实测）**：§2 围栏内**带数字**的 `### T<n>` 行（如示范「五段式」时写 `### T1 · …`）会被条目头扫描当成**真条目**、造出**幻影条目**并**吞掉其后真锚行**（fail-loud RED、非假绿）——即本件首版若在示范块里写带数字的 T 标题，同样会被抓。**子项 b 的修法须同时处理两类**（锚行过滤 + 条目头扫描）。

  两处修法都极小（① 加节域偏移；② 给解析加 fenced-block 跳过或给探针同款），但**本批不承接**——它们触 `.claude/scripts/lib/governance-gate-shared.mjs` 与 `.claude/scripts/landing-fidelity-check.mjs`（G3，且后者刚经 P4 落地并通过核验，事后改动会使 verdict 失效）。**已登记 `ARQ-260920-anchor-parse-fence-lineno`**，建议随下一批 gov 快道修。
- **自律件**：本批 PROPOSAL 自用新结构（头部自指声明在案）——这是**形态演示**，不是「先斩后奏」：机械门相关形态与本批改法同形，不自相矛盾。**起草期实测（本条自证的价值）**：首版五段式把「目标文件」行写成加粗，`--preflight` **当场报红**——即：本批提议的新结构**已被它自己的自检工具检验过一遍**，且抓出了一处真错。这正是本批要推广的「起草期自检」的实证价值。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-20 - architect

**verdict：BLOCK(F1..F3)（3 SHOULD + 5 NIT；无 BLOCKING、无升级 human 项）**。方向性判断先行：T1（§1 并入 §2、节号保 2、五段固定式）与 T2（硬项标签去数字）的改法方向均成立，机械门兼容性已由本判官**实跑改法后形态的夹具**证实（见亲核留痕 1，非仅读本件描述）；打回项全部是**计数回灌不彻底**（260920 census 更正只回灌了 T2 与 §3，漏了 V2 与 OQ2 理由行）与**一处承重指令未钉死**（模板 `:47` 的「对 §1」残留指称）——均为机械修订，修完可直接进 R2 聚焦复核。

**findings**：

- **F1 · SHOULD · §5 V2 预期「3 路径」为陈旧计数**。判官亲跑（`parseSection2AnchorLists` + `extractManifestBlock` 于本件全文）：problems=[]、union = 5 路径、manifest 围栏块 = 同 5 路径、`compareAnchorSets` aligned（双向差空）。「3 路径」是 census 更正前首版 manifest（模板 + judge-brief + residual-scan-patterns 三件）的残影。落地核验若按 V2 字面「3 路径」核对会误判。修法：V2 改「5 路径」。
- **F2 · SHOULD · §4 OQ2 理由行「外部引用面仅 4 处」与自身 census 矛盾**。按 T2 更正后 census：12 处中模板外引用 = judge-brief 1 + residual-scan-patterns 3 + path-form-family 1 + gov-arq SKILL 2 = **7 处 / 4 文件**；「4 处」即 T2「本批自证」段记录的首版 `grep -v` 误计值（外部 4 处 / 2 文件）。OQ2 裁决（去数字）**不受影响**——7 处成本仍低、且选项 (b) 保留数字重排同样要动全部引用点、还留再漂移隐患——故不构成 OQ 重开、不升级 human；但 frozen 记录的理由行数字须与 census 一致。修法：理由行改「外部引用面 7 处 / 4 文件（12 处中的模板外部分）」并注计数更正来源。
- **F3 · SHOULD · T1 连带漏一处 §1 残留消费点：模板 `:47`「对 §1 哪个根因」**。`docs/templates/PROPOSAL.template.md:47`（§2 引言句）verbatim = 「每 T 一节：改什么（文件/段落）+ 怎么改 + 为什么这样改（**对 §1 哪个根因**）。扩承重闸优先、克制新立脚本。」——§1 并入后该指称指向空节。判官全文 grep 核过：这是模板内唯一一处「§1」字样引用（`:31` 是被并掉的节标题本体、`:33` 是随迁正文，均随 T1 消解）。T1 改法四条（并入 / 改标题 / 五段式 / judge-brief 连带）未列此句改写；「改：§1 并入 §2」的目标文件注记语义上可涵盖，但按本批自己的硬项标准（「承重 landing 指令必须原文引用钉死并给出替换后完整句」）该句属承重指称、未钉死；且验证集无一条能抓「§1」字样残留（V4 扫硬项数字、V6 数节标题行）。修法：T1 补钉死 `:47` 替换后完整句（如「每 T 一节：……为什么这样改（对应本条目「根因」段）。」），并补一条验证（`grep -n "§1" docs/templates/PROPOSAL.template.md` 期望零命中）；同时确认 T1 改法 2 的两条读法注（锚行裸起、不得加粗）随五段式一并写入模板正文，不得只留本 PROPOSAL。
- **F4 · NIT · T1 证据引述 `track-b-judge-brief.md:30` 偏一行**。实测评审透镜第 3 条（根因→改法对应）在 `:29`；`:30` 是第 4 条（硬项可执行性）。同文件另两处引用（`:27` 透镜第 1 条、`:28` census 硬项 1b）均准确。citation 机检只查行界不查内容、抓不住此类偏移，恰是「行号引用须现跑实测」硬项要防的形态。修法：改 `:29`。
- **F5 · NIT · 「现存 PROPOSAL.md 29 份」计数过时且口径未注**。实测 `find . -name PROPOSAL.md`（排 ESP32 参考树）= **30 份**：`delivery/changes/arch/**` 27 份（含本件）+ `delivery/workstreams/_archive/2026/` 3 份。起草期时点（本件未落盘）= 29 成立；但冻结时点该句已过时，且「全部为两段式」对本件自身不成立（本件即新形态）。修法：改「其余 29 份（本件除外）」并注明统计口径（是否含 workstream 归档件）。
- **F6 · NIT · census「全仓」措辞与 delivery/ 历史面排除未显式声明**。T2 称「全仓引用面 census」但命令作用域 = `.claude/ docs/`；实测 `delivery/changes/arch/**` 历史制品（已 frozen 的 PROPOSAL / verdict / retro / summary）中「硬项 1a/1b/1c/2a」引用数十处——按「不追溯改写既有 PROPOSAL」非目标保留是正确处置，但未像 retro.template R 编号那样给出显式排除说明。修法：T2「非本批目标」或 §6 留根补一句「delivery/changes/arch/** 历史制品内旧标签为 frozen 记录、不追溯改」。
- **F7 · NIT · 围栏幻影条目形态（已登记 ARQ 的补充观察，不要求本批改）**。判官夹具实测出 fence 缺陷的第二形态：§2 围栏内出现**带数字**的 `### T<n>` 行会创建幻影条目并吞掉其后第一条真锚行（真 T 报缺锚、围栏内路径入并集）——fail-loud RED、非假绿，根因与已登记缺陷同一（解析器不感知 fence）；但 ARQ-260920-anchor-parse-fence-lineno 行与 §6 留根④ 的表述只点名「目标文件： 行被吃」形态。将来修该 ARQ 子项 b 时若只给锚行过滤加 fence 跳过、不给条目头扫描加，幻影条目仍在。建议：处理该 ARQ 时把本形态纳入子项 b 修复面（ARQ 行注记由主线程裁量；本批 T1 改法 2 的围栏示范用 `T<n>` 占位符〔非数字〕，本件自身安全，已实测）。
- **F8 · NIT · V3 命令列非可直接复跑形态**。实测 `governance-gate-shared.mjs` 无 CLI 入口（直跑零输出、exit 0），可直接复跑的命令是 `node .claude/scripts/landing-fidelity-check.mjs --self-check`（该电池含 section2Anchors，判官已跑全绿）。修法：V3 命令列改写为该命令 + 断言 section2Anchors 电池行。判官已采基线：`section2Anchors pass:19 fail:0 total:19`（2026-09-20），供「零差异」断言对照。

**亲核留痕（关键项，2026-09-20 判官亲跑）**：

1. **机械门兼容性（最承重项，按改法后模板形态实跑夹具）**：构造样例 = §0 导语 + **无 §1** + `## 2. 问题、根因与改法（T1…T2）` 五段式条目（裸 `目标文件：` 行）+ §3–§7 → `parseSection2AnchorLists` 返 problems=[]、union = 5 预置路径；`compareAnchorSets(union, manifest)` = aligned（双向差空）；§2 定位：parser 正则与 preflight 探针（`section2Body` 同款 `/^##\s*2[.、]?\s/`）均正确命中（§1 缺席不破定位链）。**结论：T1 改法与机械门同形，兼容性成立。**
2. **本件自证（V1 当下即绿）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight <本件>` → `ok:true / findings:[] / anchorCount:5`；本件全文跑 `parseSection2AnchorLists` + `extractManifestBlock` 对账 → aligned（5 ≡ 5）。
3. **自检电池**：`landing-fidelity-check.mjs --self-check` 全绿（section2Anchors 19/19、landing-preflight 9/9、citation-anchors 14/14、isFrozen 19/19 等）。
4. **census 全量复跑（无排除过滤）**：`grep -rn "硬项[[:space:]]*1a\|硬项[[:space:]]*1b\|硬项[[:space:]]*1c\|硬项[[:space:]]*2a" --include="*.md" .claude/ docs/` → **12 行 / 5 文件**，与 T2 证据表 12 行逐一吻合（SKILL `:12` 同行含四标签——按行计 12、按标签计 15，T2 已自行注明「须逐处改」）。
5. **行号引述逐处核对**：模板 `:31` / `:45` / `:35` / `:49` / `:59` / `:61` / `:6`、judge-brief `:27` / `:28`（`:30` 除外，见 F4）、residual-scan-patterns `:3` / `:16` / `:18`、path-form-family `:64`、gov-arq SKILL `:12` / `:34` —— 全部 verbatim 吻合。
6. **消费方 census**：`parseSection2AnchorLists` 通查 `.claude/scripts/` = 仅 `landing-fidelity-check.mjs`（import `:39` + 调用 `:173` / `:192` / `:458`）；`proposal-consolidation-check.mjs` 零命中——§3「仅一个消费方」属实。
7. **§1 节号其他消费方排查**：track-b-verifier-brief 仅引 §2（改法集 / 锚清单并集），无 §1 引用、无数字硬项标签——T1/T2 连带无需触它；architect-landing-summary.template `:31` 的「§1 表文件列」指 summary 自身 §1，不受影响；WORKSTREAM 两模板与 portfolio-planner 的「PROPOSAL §1 业务目的清单」指 workstream 模板族，与本模板无关。**模板面唯一漏网 = F3 的 `:47`。**
8. **§0 起始惯例（OQ1 理由）**：dev-loop.md（`## 0. 强制默认规则`）、change-triage.md（`## 0. 铁律与地板`）、ARCH-REVIEW-QUEUE.md（`## 0. AI 维护规则`）、docs/notes/ui-implementation-strategy.md（`## 0. 一句话定论`）——四件属实。
9. **ARQ 在册**：ARQ-260920-anchor-parse-fence-lineno（全文读：两缺陷 + `--preflight` 输出 verbatim 凭据 + 子项 b 如实标分叉，登记质量合格）、ARQ-260914-landing-preflight-anchor-parse、ARQ-260909-gate-green-anchor-regex-loose 均在册。
10. **V5 / V6 / V7 基线**：judge-brief `§1 引述`=1 / `§2 引述`=0；模板 `^## 1.`=1 / `^## 2.`=1；`route-regression-check.mjs` 28 ok 0 fail；`lint-descriptions.mjs` 仅 landing-verifier.md 超长告警（与 V7 预期一致，本批未触该文件）。
11. **留根④ 两条的代码级复核**：`preflightFindings()` 的 `boldLines.push(i + 1)` 中 `i` 确为 §2 切片下标（相对行号缺陷属实）；锚行过滤仅 strip `>` + trim（不感知 fence，属实）。
12. **retro.template R 编号排除**：retro.template `:14`–`:22` 硬项 R1–R5 + ARCH-REVIEW-QUEUE 行内「硬项 R2」实测在册——R 编号不绑节号，排除理由成立。
13. **自检夹具字面计数**：`governance-gate-shared.mjs` 内 `'## 2. 改法'` 子串出现 11 处（含 `:443` 的 `'## 2. 改法（T1…T2）'`）——§3 sweep 表「11 处字面不受影响」按子串口径成立。

**升级 human**：无。F1–F3 均为机械修订（计数回灌 + 指令钉死），不触改法方向、不触 OQ 裁决实质（F2 已注明裁决不受影响）。

**结构字段在场核对（透镜 7）**：landing-manifest 围栏块（5 路径）与 packet-id 行均在场，且与 §2 锚并集实测对账 aligned。

### R2 - 2026-09-20 - architect

**verdict：GREEN**（R1 三条 SHOULD 全部收口且亲核属实；附 2 条未结 NIT〔F4/F7——均系「派遣方声称已修、制品未见」项〕+ 1 处引文粗体微瑕，均不触机械门、不触 OQ 裁决、不影响目标效果，故按严重度规约不阻 GREEN）。主线程可凭「立项 ack + 本 GREEN」标 FROZEN；建议标 FROZEN 时顺手清 F4′（一字之差）并裁量补 F7′ 的 ARQ 行注记。

**F1–F8 逐条复核（v0.3，判官亲核）**：

- **F1 ✓ 已修**：§5 V2 现写「**5 路径**——R1-F1 更正，v0.2 原写 3 为 census 更正前陈旧值」——与判官实测（manifest = union = 5 路径）一致。
- **F2 ✓ 已修**：§4 OQ2 理由行现写「外部引用面（模板外）**7 处 / 4 文件**」+ 更正注记（v0.2 原值系 `grep -v` 误计、裁决实质不受影响）——与 R1 census 复算（judge-brief 1 + residual-scan-patterns 3 + path-form-family 1 + gov-arq 2 = 7 / 4 文件）一致。
- **F3 ✓ 已修（三要素齐）**：① 连带二钉死了替换后完整句——原句（模板 `:47`）与替换句逐字给出，替换句语义到位（「先写问题与证据，再写根因，再写改法」与五段式同构）、不含 `目标文件：` 起头（裸锚行不受影响）；② 「读法注两处形态约束随五段式一并写进模板正文」已作为义务钉进连带二；③ V6b `grep -n "§1" docs/templates/PROPOSAL.template.md` → 零命中**真能抓 §1 残留**：判官实测模板现含「§1」字样**恰一处**（`:47`），替换后归零；节标题残留由 V6（`^## 1.` = 0）覆盖，V6+V6b 合起来完备。
- **F4 ✗ 未修（派遣方声称已改 `:29`，制品未改）**：§2 T1 证据第 2 条（本件 `:54`）仍写 `track-b-judge-brief.md:30`；`git show f526707` 全量 diff 亦无该行改动。透镜第 3 条实仍在 `:29`。维持 NIT——一字之差、不触任何门；建议标 FROZEN 时顺手改。
- **F5 ✓ 已修**：T1 证据现写「30 份（含本件与 3 份 workstream 归档件）」+「口径应为『其余 29 份为两段式』，本件即新结构」的更正注记——与判官 `find` 实测（30 份 = arch 27〔含本件〕+ workstream 归档 3）一致；判官抽验 2 份归档件（260911-display-interaction-sovereignty、kidpal-mvp）均有独立的地面真值节，与「问题/改法分离」的普遍性断言相容。
- **F6 ✓ 已修**：census 段新增「作用域声明（R1-F6）」——作用域 `.claude/` + `docs/`、arch 历史制品旧标签为 frozen 记录不追溯改、排除是有意处置。
- **F7 ✗ 未落（派遣方声称已补，制品未改；本项在 R1 即明示为裁量项）**：§6 留根④ 两条仍只覆盖「目标文件行被吃 + 行号偏移」两缺陷，未含「围栏内 `### T<n>` 条目头造幻影条目吞锚行」形态；ARQ-260920 行实测亦无「条目头/幻影条目」字样（grep 亲核）。该形态目前仅存于本件 §7 R1-F7 文字——将来处理该 ARQ 时子项 b 若只给锚行过滤加 fence 跳过、不给条目头扫描加，幻影条目仍在。维持 NIT + 建议主线程在 ARQ 行追加一句注记（登记动作归主线程，本判官不代写）。
- **F8 ✓ 已修**：V3 命令改为 `node .claude/scripts/landing-fidelity-check.mjs --self-check` + 「shared lib 无 CLI 入口」更正注记 + 判官基线 19/19——可直接复跑。

**新引入矛盾核查**：未见。① manifest（5 路径）≡ §2 目标文件并集（T2 扩围后新增 path-form-family.md 与 gov-arq/SKILL.md 两路径）——判官对 v0.3 全文复跑 `parseSection2AnchorLists` + `extractManifestBlock` 对账：problems=[]、union 5、aligned；② `--preflight` 复跑 ok:true / findings:[] / anchorCount:5；③ riskTier low（零脚本改动）与 review profile G2（五件全 G2 面）维持成立——§3 sweep 表未变且判据未变；④ OQ1/OQ2 裁决内容未被修订触碰；⑤ §7 R1 记录经 `git show f526707` diff 核对为 append 原样保全。

**新观察（NIT 级，随 F4′ 顺手清，不另立编号）**：连带二所引「原句」把「对 §1 哪个根因」加了粗体——模板 `:47` 原文**无粗体**（判官 grep 亲核，该行唯一命中、无 `**`）。前后文足以唯一定位替换目标，钉死效力不受影响，但「原文引用钉死」的 verbatim 精度可顺手对齐。

**升级 human**：无。

**结构字段在场核对（透镜 7）**：landing-manifest 围栏块（5 路径）与 packet-id 行在场，与 §2 锚并集实测对账 aligned——满足 GREEN 前置。