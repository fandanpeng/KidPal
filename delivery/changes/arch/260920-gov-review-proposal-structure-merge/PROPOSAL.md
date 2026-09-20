# PROPOSAL · 260920-gov-review-proposal-structure-merge

> **状态：DRAFT v0.2 · 2026-09-20 · 已立项 ack · 待 architect R1 评审**（凭据：human 2026-09-20 会话「立项 ack」+ OQ 全裁〔OQ1 = 保留 §1 空档；OQ2 = 硬项标签去数字〕。FROZEN 行待补 architect Rn GREEN 凭据）。
> **承载 ARQ**：无——human 2026-09-20 会话直接触发（读 260918 批 PROPOSAL 时提出「问题与改法两段分离、阅读成本大」）。收口时按惯例补登记。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。两条升级判据（触契约面 / 跨系统结构性变更）均未命中。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T2）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（一句话证成：纯**文档形态 + 引用标签**改动，**零脚本改动**；机械门 `parseSection2AnchorLists` 的定位三要素——§2 节号、`### T<n>` 条目头、`目标文件：` 裸行——**全部原样保留**）。
> **review profile**：G2（`docs/templates/PROPOSAL.template.md` + `.claude/references/track-b-judge-brief.md` + `.claude/references/residual-scan-patterns.md` 三件，全在 G2 面）。契约面 / G1 / G3 均为零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。
>
> **【本批自指声明】** 本 PROPOSAL **自己就用被提议的新结构写**（§1 空档、§2 五段式条目）——作为形态演示。其机械门相关形态（§2 节号 = 2、条目头 `### T<n>`、`目标文件：` 裸行）与本批改法**同形**，故不自相矛盾：门解析本件与解析改法后的模板，走的是同一条定位链。

```landing-manifest
.claude/references/residual-scan-patterns.md
.claude/references/track-b-judge-brief.md
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
- **对位成本落在判官透镜上**（verbatim）：`.claude/references/track-b-judge-brief.md:30` 评审透镜第 3 条 = 「**根因→改法对应**：各 T 是否真治对应问题类；有无错配/漏承接」——该透镜的实现方式就是跨节对位。
- **人类读者反馈**（会话原文，2026-09-20）：human 读 260918 批 PROPOSAL 时提出「现在的结构是『问题根因』…然后到下一个章节又变成『解法』…它俩是分开的…这样我在看的时候，能把问题和解法一块看了。不然两段是分开的，阅读成本比较大」。
- **跨批普遍性**：现存 `PROPOSAL.md` **29 份**（`find . -name PROPOSAL.md` 亲跑）全部为两段式。

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
4. **连带**：`track-b-judge-brief.md:27` 评审透镜第 1 条「**§1 引述**（编号、次数、数据点）与证据基线逐一核对」→ 引述节号改指 **§2**（合并后 §1 不存在，若不改，该透镜会指向空节）。

目标文件：`docs/templates/PROPOSAL.template.md`（改：§1 并入 §2 + 五段式条目模板）、`.claude/references/track-b-judge-brief.md`（改：透镜第 1 条引述节号 §1→§2）

### T2 · 「硬项」编号绑死节号，节号一动即失真

**问题**：四个【硬项】的标签写作 `1a / 1b / 1c / 2a`——数字部分是**旧节号的镜像**。T1 把 §1 并入 §2 之后，原「硬项 1a」与「硬项 2a」**同落 §2**，编号自相矛盾（`1a` 与 `2a` 并列在同一节内）。若不处理，模板自身即示范了一次「标识符绑死结构位置」的漂移。

**证据**（全部亲核，2026-09-20）：
- 现模板（verbatim）：`:35` = `**【硬项 1a · 起草期实证义务】**`（该条原属 §1）、`:49` = `**【硬项 2a · 目标文件锚清单】**`（属 §2）——`1a` 与 `2a` 的数字分别锚在 §1 与 §2。
- **外部引用面**（`grep -rn "硬项 1a\|硬项 1b\|硬项 1c\|硬项 2a" --include="*.md" .claude/ docs/` 亲跑）：
  - 「硬项 1b」**1 处** = `.claude/references/track-b-judge-brief.md:28`；
  - 「硬项 1c」**3 处** = `.claude/references/residual-scan-patterns.md:3` / `:16` / `:18`；
  - 「硬项 1a」「硬项 2a」**0 处**（无外部引用）。
- **同型先例**：本仓已有「标识符绑死会漂移的位置 → 位置一动即失真」的实例——在册 `ARQ-260909-gate-green-anchor-regex-loose` 的病灶即「注释声明的意图与实现脱节」，其追记里判官的原话是「与 `:83` 注释声明该契约」类的位置耦合。

**根因**：标签的设计意图是「第几节的第几个硬项」——把**位置**编进了**名字**。凡名字含位置信息者，位置变则名字失真；而修复（改数字）会**逐个波及引用点**，引用点越多越贵。

**改法**：四个硬项**去掉数字前缀**，改为纯功能性标签，使名字只描述**义务内容**、不含位置：

| 现 | 改后 |
|---|---|
| 【硬项 1a · 起草期实证义务】 | **【硬项 · 起草期实证义务】** |
| 【硬项 2a · 目标文件锚清单】 | **【硬项 · 目标文件锚清单】** |
| 【硬项 1b · sweep 透镜义务】 | **【硬项 · sweep 透镜义务】** |
| 【硬项 1c · 复扫模式完备性先验】 | **【硬项 · 复扫模式完备性先验】** |

并同步 **4 处外部引用**（judge-brief ×1 + residual-scan-patterns ×3）。**义务语义零变**，只动标签文字。

目标文件：`docs/templates/PROPOSAL.template.md`（改：四标签去数字）、`.claude/references/track-b-judge-brief.md`（改：硬项 1b → 硬项）、`.claude/references/residual-scan-patterns.md`（改：硬项 1c ×3 → 硬项）

**【起草期自检两句】**（260918-gov-review-batch-drafting-friction T2 钉死，随 §2 保留）① **FROZEN 标注前跑一次 `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL 路径>`**——只读干跑，报两类**形态**错（加粗锚行、无锚清单含整行缺失），不参与任何门判定、不进任何闸链；**并通过 §5 验证集命令实跑**。② **路径类变更先查 `.claude/references/path-form-family.md` 的形态族谱表**。

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 提案模板 | `docs/templates/PROPOSAL.template.md` | G2 | §1 并入 §2 + 五段式条目模板 + 四标签去数字。消费方 = 每一份新 PROPOSAL 的作者 |
| 判官简报 | `.claude/references/track-b-judge-brief.md` | G2 | 透镜第 1 条引述节号 §1→§2 + 硬项 1b 去数字。消费方 = 每环判官派遣 prompt |
| 扫描模式参考 | `.claude/references/residual-scan-patterns.md` | G2 | 硬项 1c ×3 去数字。消费方 = 涉退役语面清扫的批次作者 |
| **机械门** | `.claude/scripts/lib/governance-gate-shared.mjs` | **未触** | `parseSection2AnchorLists` 定位三要素（`/^##\s*2[.、]/` + `/^###\s+(T\d+…)/` + `目标文件：` 裸行）**原样保留**；其自检夹具 11 处 `'## 2. 改法'` 字面**不受影响** |
| **脚本面 / 契约面 / G1 / agent frontmatter** | — | **未触** | 零改动 |

**【硬项 · sweep 透镜义务】** 本批**不新增门、不改判定 pattern**，故无 pattern 预跑义务。**但枚举义务已履行**（T2 改的是标识符，其引用面正是 census 类）：两个标识符族的**全仓引用面**已机械枚举，命令与结果逐处列在 T2 的「证据」段（`grep -rn` 亲跑）。另枚举了本批**不受影响**的关联面以证「未触」：`parseSection2AnchorLists` 的消费方实测**仅 `landing-fidelity-check.mjs` 一个**（`:39` import + `:173`/`:192`/`:458` 三处调用；通查 `.claude/scripts/proposal-consolidation-check.mjs` **零命中**，故本批无第二个消费方要改）。

**【硬项 · 复扫模式完备性先验】** 本批涉**标签改名**（一种残留清扫）。模式集显式枚举全形态：`硬项 1a` / `硬项 1b` / `硬项 1c` / `硬项 2a` × 有 / 无空格 × 裸文本 / backtick 包裹。**完备性论证**：四标签是**固定字面**——不涉词形变体（无中英转换、无单复数、无缩略），故 grep 模式 `硬项 1[a-c]\|硬项 2a` 的命中面**即**完备面；实测命中 **4 处**，已逐处列出。

## 4. human 取舍点（OQ）

**两处 OQ 已由 human 2026-09-20 会话裁定「同意推荐位」**，记录如下（随立项 ack 一并确认）：

| OQ | 选项 | 裁决 | 理由 |
|---|---|---|---|
| OQ1 · 合并后节号出现 `0 → 2 → 3…` 空档，如何处理 | (a) **保留空档**（§0 当导语）；(b) 把 §0 改名 §1 消掉空档 | **(a) 保留空档** | 两者机械代价相同（都要改 judge-brief 一行）；而「§0 = 导语」是本仓既有惯例（`dev-loop.md` / `change-triage.md` / `ARCH-REVIEW-QUEUE.md` / `ui-implementation-strategy.md` 均以 §0 起）。为一个纯粹观感空档而破惯例不划算 |
| OQ2 · 硬项标签是否去数字 | (a) **去数字**（纯功能标签）；(b) 保留数字并重排 | **(a) 去数字** | 数字前缀把「位置」编进了「名字」——T1 的合并正撞在这上面（`1a` 与 `2a` 同落 §2 即自相矛盾）。去数字一次性解除耦合；外部引用面仅 4 处，成本已实测 |

**无待裁 OQ。**

## 5. 验证集（landing 前机械前置 + landing 后核验）

| 项 | 命令 | 预期 |
|---|---|---|
| V1 | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260920-gov-review-proposal-structure-merge/PROPOSAL.md` | `ok:true` / `findings:[]` / exit 0——**本件即新结构，须自证形态可解析** |
| V2 | `node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260920-gov-review-proposal-structure-merge` | `ok:true`；`frozen`/`packetBinding` 真；`proposalTargets` ≡ `summaryTargets`（3 路径）；`unauthorized`/`claimedNotLanded`/`plannedNotLanded` 全空 |
| V3 | `node .claude/scripts/lib/governance-gate-shared.mjs` 经 landing-fidelity 的 `--self-check` 电池 | `section2Anchors` 电池**全绿且与改动前零差异**——证机械门未受本批影响 |
| V4 | `grep -rn "硬项 1a\|硬项 1b\|硬项 1c\|硬项 2a" --include="*.md" .claude/ docs/` | 落地后 **零命中**（4 处引用全改完） |
| V5 | `grep -c "§1 引述" .claude/references/track-b-judge-brief.md` + `grep -c "§2 引述" …` | `§1 引述` = **0**、`§2 引述` = **1** |
| V6 | `grep -c "^## 1\." docs/templates/PROPOSAL.template.md` + `grep -c "^## 2\." …` | `## 1.` = **0**（§1 已并入）、`## 2.` = **1**（节号保留） |
| V7 | `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` | 全绿；lint 只允许既有告警（`landing-verifier.md` 超长，本批未触该文件） |

**真空绿规约核对**：V3 为「**零差异**」型断言（工程门未受影响的正面证据，而非「跑过了就算」）；V4/V5 为**改名类**——须断言**旧标识符零命中**，不能只断言新标识符在场（否则改名没改净也是绿）；V1 为**自指**型——本件用新结构写，故它同时充当形态夹具。

## 6. 留根清单

- **留根① · 既有 29 份 PROPOSAL 不追溯改**：它们形态为旧两段式。机械门只在 landing 时解析**本批**，故不影响功能；但**判官读旧件**时仍会看到两段式。**交日落复盘项**：观察下一批判官是否因形态不同而产生误读或额外对位成本。
- **留根② · §5 验证集与 §3 sweep 对「T 编号」的耦合未动**：本批只改 §2 条目**内部**布局，未动 T 编号本身；§5 的 V 编号仍按 T 编号。**若将来把条目单位从 `T<n>` 换成别的形态**（本轮讨论中称为「乙′」的那条路），§3/§5 要一并改键——**本批不预批**。
- **留根③ · 「问题 ↔ T 非 1:1」的表达未处理**：实测 260918 四批**冻结终态**均为 1:1；但起草期一度出现 2:1（那是「T 被移出后重编号」的过渡态，非常态）。若将来出现真 2:1，需定「一条 T 写两段问题」还是「拆成两条 T」——**本批不预设**。
- **留根④ · 起草期自检工具的两处精度缺陷（本批起草期实测发现，不承接）**：

  1. **`--preflight` 报的行号是 §2 域内相对行号，非文件行号**：本件首版被报 `bold-anchor-line, lines:[38]`，而该行在**文件**第 68 行——`preflightFindings()` 的 `boldLines.push(i + 1)` 里 `i` 是 **§2 节域切片内的下标**，未加节域起始偏移。后果：作者按报出的数字去文件里找**找不到**（要心算 `文件行 − §2 起始行`）。
  2. **`parseSection2AnchorLists` 不跳 fenced code block**：本件首版在 §2 的引号块里示范 `目标文件：` 行，被解析器当作**真锚行**吃进并集、报「歧义锚」（`T1 … 「目标文件：」行无 backtick 包裹路径`）。后果：**PROPOSAL 的 §2 内无法示范锚行的字面形态**——本件最终改用 `〔第五段·锚清单〕目标文件：…` 前缀标记绕开（见 §2 T1 的「读法注」）。

  两处修法都极小（① 加节域偏移；② 给解析加 fenced-block 跳过或给探针同款），但**本批不承接**——它们触 `.claude/scripts/lib/governance-gate-shared.mjs` 与 `.claude/scripts/landing-fidelity-check.mjs`（G3，且后者刚经 P4 落地并通过核验，事后改动会使 verdict 失效）。**已登记 `ARQ-260920-anchor-parse-fence-lineno`**，建议随下一批 gov 快道修。
- **自律件**：本批 PROPOSAL 自用新结构（头部自指声明在案）——这是**形态演示**，不是「先斩后奏」：机械门相关形态与本批改法同形，不自相矛盾。**起草期实测（本条自证的价值）**：首版五段式把「目标文件」行写成加粗，`--preflight` **当场报红**——即：本批提议的新结构**已被它自己的自检工具检验过一遍**，且抓出了一处真错。这正是本批要推广的「起草期自检」的实证价值。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）