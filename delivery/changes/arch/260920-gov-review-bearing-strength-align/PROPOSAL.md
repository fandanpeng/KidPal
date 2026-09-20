# PROPOSAL · 260920-gov-review-bearing-strength-align

> **状态：DRAFT v0.2 · 2026-09-20 · 已立项、待进 arch 评审循环**（凭据：**human 2026-09-20 立项 ack + OQ 全裁**——OQ1=(a) 按既成先例落 agent def 引用行/携带句；OQ2=(a) 只并入与落点同址的两条搭车项〔见 §4 裁决表〕。起草依据：`C:\tmp\kidpal-gov-remaining-plan.md` §2「P3 · dev-loop 承载 / 验收约定」——该件是临时工作笔记、非 canonical 制品；本件是它的正式化载体，**成稿前已对四条处方逐条实测复证**，复证结果改变了批的构成，详见下「批构成复证」）。
> **承载 ARQ**：`ARQ-260911-cross-package-behavior-owner-gap`（T1，**派单侧残余**——拆包期判据已由 260912 批 T3 消化）、`ARQ-260911-tooth-strength-below-contract`（T2）、`ARQ-260911-verdict-uncovered-surface`（T3，**处方落点经复证更正**）。原计划的第四条 `ARQ-260911-split-package-implementation-prose-gap` **已由 260912 批 T2 结清、本批不做**（复证见下），其队列行的结移随本批落地一并执行。不关的根写 §6 留根指针。
> **批构成复证（起草期现跑，2026-09-20）**：原计划四条中有两条的处方已在本仓被后续批消化——① 第四条（包级实施方案 prose 无耐久家）已由 `260912-gov-review-portfolio-decomposition-chain` T2 落地为 **`plan.md` 全量义务**（「装配型豁免」已全取消，260913 human 裁 OQ3=(a)）：实测 `delivery/taskpackages/` 35 个目录中 15 个带 `plan.md`，**260912 之后的 11 个包 100% 带**（无 `plan.md` 的 20 个全部 ≤ 260911，属存量）；该 ARQ 的队列行仍写「19 个 MVP 拆包全无 `plan.md`/`planRef`」为 2026-09-11 时点值、未随落地更新。② 第一条的**拆包期判据**已由同批 T3 消化进 `task-intake-and-slicing.md:253` 门 1 checklist ③（「decision-coverage 完备（含跨包行为 owner 显式登记）」），**派单侧义务按该批声明保留在册**、即本批 T1 承接的对象。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节）。两条 Track A 硬判据（触契约面 / 跨系统结构性变更）均未命中。**但本批有一个须摊开的判断点：三处改动要动 agent def（T2 的引用行、T3 的正文携带句）**——仓内先例两向并存（落了的：260906/260907/260912；没落的：260918 closeout-chain 主动留根），协议条文本身也两处不一（`arch-review-protocol.md:98-99` vs `:80`）。本批**如实列为 OQ1 交 human 裁**，判官若判其为 `HUMAN_REQUIRED` 即升 human。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T3）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（一句话证成：七处改动全为**成文 / 派生 / 携带句**——零闸判定语义改动、零新增工程门、零 agent frontmatter、零 `docs/contracts/**` 与 spec 面；唯一的 G3 触点是 `brief-inputs.mjs` 增一路**只读派生**（advisory 生成器，不参与任何门判定），唯一的强制层触点是 agent def 的引用行与一句携带句——后者即 OQ1 的判断对象。不报 low 的理由 = 触面含 agent def 与 G3，且判据是本批自己新立的）。
> **review profile**：G2 + G3（G2 = `.claude/references/` 四件 + `.claude/agents/` 两件的引用行/携带句；G3 = `.claude/scripts/brief-inputs.mjs`）。契约面 / G1 / agent frontmatter / scope-map / settings / hooks **均为零**。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/agents/acceptance-author.md
.claude/agents/planner.md
.claude/references/acceptance-evidence.md
.claude/references/dev-loop.md
.claude/references/pod-coordination.md
.claude/references/task-intake-and-slicing.md
.claude/scripts/brief-inputs.mjs
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把 dev-loop 里三处「**设计说了、承载面没跟上**」的缝补上——跨包行为的 owner 登记要有**消费点**（现在只写不读）、验收齿的**强度**要有申报位（现在只管有没有齿、齿敏不敏感）、收口时要有**未覆盖面**的差集申报（现在无人回答「哪些设计性质没齿」）。

**现状差距**：三处形态同一个根——**判据/字段/义务在前一段工序已经存在，但下一段工序没有消费它**：

| 缝 | 上游已有 | 下游缺 |
|---|---|---|
| 跨包行为 | 拆包期门 1 checklist ③ 要求登记 `crossPackageBehaviors`（8 个包已在用） | 派单期没有任何角色/脚本读它——下一个包起包时不会去翻别的包的 `taskpackage.json` |
| 齿强度 | 契约/设计要求写在 spec 与设计稿里；冻结验收件头注已自发写强度要求 | 没有任何角色被指定义务把「契约条目 → 承载齿 → 该齿断言的性质」对齐并申报弱化处 |
| 未覆盖面 | review-packet 有五条必列项 + 锚 3 demo 对账 | 无人给「设计性质 vs 有齿」的差集；acceptance-coverage 只看**已声明的条目**、锚 3 只看**demo 证据形态** |

三者造成的实际代价已由四次跨包 retro 记录在案（四案例 + loop-retro-2026-09-13 三例），共性是**每包在自己边界内全绿、合起来与设计不符**。

**非目标（本批不做什么）**：不改 `docs/contracts/**` 与系统 spec 两文件；不动任何 agent frontmatter（`name`/`tools`/`model` 一字不动）；不新增工程门、不改任何既有闸的判定语义；**不追溯补写存量制品**（34 份存量 review-packet、8 个存量包的字段形态一律不动——义务自本批起对新包生效）；不把判断型申报塞进机器闭集（`verifier-evidence/rounds/*.json` 的 14 字段闭集不动，理由见 §6 留根⑤）；不并入 `ARQ-260914-legacy-sxay-story-id-batch` 这类纯机械批量换例句项（见 OQ2）。

## 2. 问题、根因与改法（T1…T3）

**【硬项 · 起草期实证义务】** 本节的断言纪律：凡 regex / git / 机械门 / 脚本**行为**断言 → 草稿即附实测（命令 + 关键输出 verbatim + 日期）；凡「全 / 均 / 无」量化声明 → 现跑取证；凡数字引用 → 起草期现跑实测或核对权威原处，转引陈旧制品数字时显式注出处与复核结果；凡声称「对偶 / 比照 <先例>」→ 先读先例文件、引 file:line verbatim 确认形态。**本批起草期实际执行了 6 项复证**（见各 T 证据段与 Status 行的「批构成复证」）。

**【硬项 · 目标文件锚清单】** 每 T 末尾显式列「目标文件：<仓根相对路径清单>」。packet-binding 完整性断言（landing-manifest ≡ 各 T 目标文件并集）依赖本清单；**缺锚清单 = 门 RED 点名缺锚**。改法标题一律 `### T<n> · <标题>` 标题形态；landing-manifest 围栏块一行一个裸路径。

**条目形态（五段固定式）**：见 `docs/templates/PROPOSAL.template.md` §2「条目形态」——**问题 / 证据 / 根因 / 改法 / 〔第五段·锚清单〕**。

**【起草期自检两句】** ① FROZEN 标注前跑 `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL 路径>` 并通过 §5 验证集实跑（本件起草期已跑，输出见 §5 V1）；② 本批不涉路径类变更（无文件迁移/改名），族谱表不适用。

### T1 · 跨包行为的 owner 登记只写不读，派单侧没有消费点

**问题**：`crossPackageBehaviors` 这个字段自 2026-09-11 首用起，已经是 M1 批的**事实标准**（8 个包的 `taskpackage.json` 在用），但它**没有任何 canonical 定义、也没有任何消费方**——脚本面 `grep` 零命中。于是它只在两处生效：拆包期门 1 的人读 checklist（判据在），以及各路 prose 的自发引用。**真正出事的环节是派单期**：下一个包的 planner 起包时读的是自己的 TaskPackage 与 input-brief，**不会去翻别的包的 `taskpackage.json`**，所以「上游登记的、owner 指向本包的跨包行为」没有任何机制会到达他手上。四案例里的两个（`GPIO18` 键接线不属于任何包、`art_assets.c` 的「殿后 9b」派单漏列）都不是「没人登记」，而是**登记了/写下了、下游没读到**。

**证据**（全部亲核，2026-09-20）：
- **字段已在用，8 个包**（命令 `grep -rln "crossPackageBehaviors" delivery/taskpackages/*/taskpackage.json`）：`260911-display-first-m1`、`260911-key-route-a1`、`260911-screen-unification`、`260915-m1-k1a-key-layer-dispatch`、`260915-m1-k2-keybar-three-slot`、`260915-m1-k3-voice-round-ui`、`260915-m1-k4-reminder-overlay-unify`、`260915-m1-n1b-p1-home-battery`——**M1 批 5/5 全覆盖**。
- **字段形态（先例 verbatim，`delivery/taskpackages/260911-key-route-a1/taskpackage.json:100-105`）**：`{"behavior": "KEY(GPIO18) 板级接线 + key_gesture_parser_feed 设备侧喂入 + 全局路由端到端", "owner": "260911-key-route-a1", "note": "ARQ-260911-cross-package-behavior-owner-gap 首用试金石：…本批起 owner 归本包独占，杜绝再掉包间缝隙。"}`——形状是 `{behavior, owner, note}`，**但这是从实例反推的，不是成文定义**。
- **无 canonical 定义**：`grep -c "crossPackageBehaviors" .claude/references/task-intake-and-slicing.md` → **0**（该文件是 taskpackage authoring 的 SSOT）；该词在 `.claude/` 全域仅命中队列行与两份 ARQ 引文。
- **无消费方**：`grep -rn "crossPackageBehaviors" .claude/scripts/ .claude/hooks/ | wc -l` → **0**。
- **派单侧无任何核对条文**：`grep -rn "殿后\|转手" .claude/` → **零命中**（该 grep 覆盖 agents / references / scripts / hooks 全域；输出为空）。
- **「殿后项」的真实承载形态是代码注释**（verbatim，`firmware/main/display/art_assets.c:10`）：「…设备上弱默认 -1 = **真机渲染节点集成殿后 9b**」——9b 的承接方没有出现在任何 taskpackage 字段里。
- **拆包期判据确实已在**（`task-intake-and-slicing.md:253` verbatim）：「**checklist 五项**：① 切片逻辑…；② 依赖序与 WIP 合理性；③ **decision-coverage 完备（含跨包行为 owner 显式登记）**；④ 每包意图陈述与 PROPOSAL 冻结裁定一致；⑤ demo 证据形态可行」——判据在拆包期、不在派单期。
- **扩展的现成落点**：`brief-inputs.mjs:243-246` 已经在用 `readdirSync(join(repoRoot,'delivery','taskpackages'))` 遍历兄弟包（第③路「同 subsystem 先例封顶 3」就是这么实现的）⇒ 增一路按 `owner` 过滤的派生**不需要新造扫描面**。基线：`node .claude/scripts/brief-inputs.mjs --self-check` → **23/23 passed**（2026-09-20 现跑）。

**根因**：缝不在「有没有登记」，在「**登记的消费点缺位**」——判据落在**拆包期**（门 1 人读），而事故发生在**派单期**（下一个包的 intake）。字段被「写」过一次就再也没有读者：它既不在本包 planner 的输入面（input-brief 只由**本包 TaskPackage 的 refs + crossEnd** 派生），也不在任何脚本的判定链上。「owner 指向别人」这个语义，**没有任何机制把它送回给那个别人**。

**改法**：
1. **成文**：`task-intake-and-slicing.md` 增 `crossPackageBehaviors` 字段级定义（承该文件既有的字段级定义体例，如 `crossEnd` 字段级定义段）——形状 `{behavior, owner, note}`；**必填判据**：行为横跨 ≥2 包时必填，或本包显式不做、须交下游承接的「殿后 / 转手」项**必须**在此登记（不得只留代码注释或 prose——`art_assets.c:10` 即为反例形态）；`owner` 可填本包 `changeId` 或**承接包 `changeId`**；`note` 写清移交判据与验收归属。
2. **给消费点（本 T 的核心）**：`brief-inputs.mjs` 增 **第五路派生「跨包承接面」**——遍历 `delivery/taskpackages/*/taskpackage.json`，取 `crossPackageBehaviors[].owner === 本包 changeId` 的条目，逐条渲染「behavior + 上游包 changeId + note」进两份 input-brief（acceptance-author / builder）。**生成时点快照**语义与该脚本既有四路一致；**零命中时段落明写 `none`、不静默省略**（防「没写」与「没有」混同）。方向为**补可见性**：不改任何门判定、不新增失败面，命中即列、不命中即 none。
3. **自检夹具扩断言**（见 §5 V2）：至少三条——命中（上游 owner 指向本包 → 列出）、零命中（无上游指向本包 → 明写 `none`）、排除（上游 owner 指向**别**包 → 不得出现）。

**为什么这样改**：直接对根因——把「owner 指向别人」这个语义**从写入侧搬到读取侧**，且落在**已有携带臂的输入面上**（input-brief 是 planner Step 7 派生、子 agent 交付报告要回传超读申报的封闭清单，天然被读到）。这条正是 `ARQ-260913-auto-mode-classifier-legit-workflow` 追记里那条教训的正面应用：**只落口径臂不算落地，须同时有携带臂**。

目标文件：`.claude/references/task-intake-and-slicing.md`（改）· `.claude/scripts/brief-inputs.mjs`（改）

### T2 · 齿断言的性质可合法弱于契约要求，且没有申报位

**问题**：验收纪律管住了三件事——**有没有齿**（acceptance-coverage）、**齿敏不敏感**（mutation-check 的 `sensitive`）、**齿是否空过**（vacuity）。但**没有一处管「齿断言的性质够不够契约要求的强度」**：一条断言「这个按钮按下后界面变化了」的齿，与契约要求的「按下的必须是 GPIO18 那条电平路径」之间，**在现有全部绿信号里没有任何差别**。既没有「契约条目 → 承载齿」的映射层，也没有「本条齿弱于契约」的申报位。后果不是红，是**绿得没有信息**：`BOOT≈KEY` 的行为等价使「KEY 根本没接线」原理上不可见（四案例之二），「符号零残留」结构齿被改名绕过（追加证据之二）。

**证据**（全部亲核，2026-09-20；跨批案例如实标注复核结果）：
- **现状无此约定**：`acceptance-evidence.md` 全文 **50 行 / 五节**（`## 必要证据`、`## UI 验收 vs 设计 review`、`## 红基线`、`## 破坏性自检`、`## 反例`），`grep -ci "强度\|契约条目\|映射"` → **1**（且非该义）⇒ 无强度/映射约定。
- **实况案例一已闭合，如实标注**：ARQ 原文引 `ksy_label`（`story_page.c:1062`）不设宽不 wrap 致横向裁切「一路绿」。**复核结果：该强度缺口已由 2026-09-12 的 RP-A2/F2 机制齿补上、引据行号亦已漂移**——现跑 `grep -n ksy_label firmware/main/display/story_page.c` → **`:1122`**（原引 `:1062`）；该处实现现已设宽度 + `LV_LABEL_LONG_WRAP`（`story_page.c:1265-1270` verbatim 注释含「段落设宽度 + WRAP 消横向裁切 (#9 排版缺陷)」）；机制齿在 `firmware/main/test/acceptance/archive/story_p3.acceptance.c:2071-2075`（verbatim：「P3-A5/F2 — READER「当前屏正文」机制齿（RP-A2/F2, REVISION LOG 2026-09-12 新产）。既有齿 ④「屏 1 vs 屏 2 帧差异」由屏位指示条产生、**不锁正文切片**; 本齿锁机制本体」）。**⇒ 本案例作为「缺口存在」的论据已失效，仅作「强度缺口是可被发现、可被补的」的正面样本保留。**
- **实况案例二（KEY 全行为级）**：`smoke-e2e SMK-A3`、`device-ws-switch`、`story-p3`、`reminder RMD-A5`、`bargein BRG-A2` 的 KEY 验收全是行为级断言（按键 → 对话），机制级（哪根 GPIO 电平路径）零齿；`BOOT≈KEY` 行为等价使「KEY 未接线」原理上不可见（ARQ 原文，出处 = `delivery/changes/dev/260829-hardware-validation/RETRO-VERIFICATION-GAP-260911.md` §二收窄③；**本项未逐齿复核，转引为待判行**）。
- **实况案例三 / 四**：`schedule_p2` 齿 6 first-match 定位歧义（齿解析器弱于齿意图，该包 review-packet F2）；`WSA-A4/A6` 证据门齿设计成 evidence-based 却无任何包生产所需制品（同上转引）。
- **追加证据（loop-retro-2026-09-13，三例，出处 = `delivery/changes/retro/2026-09-13-loop-retro-efficiency.html`）**：① render-pages 的 `v3_restore` 文本齿被冻结件注释文本偶然糊绿（三字体零运行时接线全程无信号，human 肉眼戳破）；② screen-unification「符号零残留 + 单处创建点」结构齿被「改名 + 捕获屏」双路径绕过；③ key-route-a1 变异命名保留原字面子串致探针不红、改 `_STUB` 才红。**同族累计命中 ≥6 包**。
- **机械面已到顶的证据**：`mutation-check.mjs` 的判定只有 `record.sensitive === true`（`:54`）与文案匹配（`:57`），`:130` 的失败文案为「Mutation evidence is missing or **insensitive**」——**齿再弱，只要变异能让它转红，它照样过闸**。这正是「绿信号对行为等价假设无免疫」的机械形态。

**根因**：**强度是设计判断，不是可推导属性**，所以它从未被写成义务。机械面能证的只有「有齿且敏感」；「这条齿断言的性质是否等于契约要求的性质」需要有人把**契约条目与齿并排读**——而现行分工里没有任何角色被指派做这件事：planner 读 story/spec 但不产齿、acceptance-author 产齿但输入面是**已经转写过的 acceptanceRefs**（不是契约原文的逐条强度）、verifier 只消费齿的绿信号。缺的不是新工具，是一个**申报位**（把自己知道弱在哪说出来）。

**改法**：`acceptance-evidence.md` 增一节「**契约条目 ↔ 齿 的强度申报**」（承该文件既有撰写约定体例），四件：
1. **映射申报**：产齿时列「契约 / 设计条目 → 承载齿 → **该齿断言的性质**」三类之一：**机制级**（钉具体接线 / 资产 / live 配置 / 符号路径）/ **行为级**（钉可观察行为）/ **源锚代理**（钉决策代理物，如源码文本锚，**非行为本体**）。
2. **弱于契约处显式声明**：凡承载齿的强度档低于契约要求者，逐条写明缺口与理由；**「行为等价假设」必须显式化**（如 `BOOT≈KEY`、`同名符号即接线`）——**不写即默认不成立**（现行实况恰相反：等价假设是隐性默认）。
3. **机制断言优先**：能钉「哪根 GPIO / 哪个资产 / 哪个 live 配置」的，不断「行为看起来对」；源锚代理型齿须自陈「这是代理、非行为本体」。
4. **声明落点 = 冻结验收件头注**——这是**既成先例的自发形态**（`story_p3` 冻结头注 ⑤「当前屏正文」即该体例），本改法只是把它从自发升为义务、并要求**逐条写缺口**（先例的病灶恰是「头注写了强度要求、却无齿兑现、且事后无回写」）。
另：`acceptance-author.md` References 节加一行引用行指向新增节（承 260906/260907 批「agent def 只加引用、结构零改动」先例；该文件已有 3 条同形引用行，见 §3 影响面表）。

**T2 的同址搭车两件（OQ2 裁 (a) 并入——二者都落在本 T 已有的目标文件里，landing-manifest 一字不变）**：

5. **`ARQ-260908-tighten-only-unwritten` 残余**：`acceptance-author.md` References 节**再一条**引用行——指向 `docs/DEV-GOVERNANCE.md`「验收测试纪律」§6「tighten-only 修订窗口」（该 §6 已由 `260918-gov-review-closeout-chain-friction` T3 落地，**只差这条引用行**；原批 §6 留根② 因「不值得为一条引用行把本批拖进 self-def 争议」而留根——那个争议正是本批 OQ1 的对象，human 已裁 (a)，故该留根自然解）。
6. **`ARQ-260908-known-red-count-no-mechanical-source` 子项②**：`acceptance-evidence.md` **再一节**「evidence 类扫描齿的 root 注入」——约定 evidence 类齿（如 VRT live 探针、WSA-A4/A6 证据门齿）**显式接收仓根 / 由 runner 注入 root，禁依赖调用 cwd**（实证：同一 ELF 在文档命令位置跑出 3 条既有红、在仓根跑出 2 条，判读口径随 cwd 漂移）；子项③（host runner 报告头写执行目录）**不并入**（属 host 测试基建面，见 §6 留根④）。

**为什么这样改**：把「强度」从**隐性默契**变成**可核对的显式申报**——它不生成任何新闸（判断型，无法机判），但让「齿绿而契约未兑现」从「事后靠人肉眼撞见」变成「事先写在一处、谁都能读」。**刻意不做的**：不把强度做成 acceptance 字段（那要给判断型数据造机器形状，属过度机械化）、不设门。

目标文件：`.claude/references/acceptance-evidence.md`（改）· `.claude/agents/acceptance-author.md`（改）

### T3 · 收口时无人回答「哪些设计性质没有齿」

**问题**：包收口现有三道「未覆盖面」的**近亲**，但没有一道回答「**哪些设计性质没有齿**」：① `acceptance-coverage` 只对**已经声明进 `acceptance[]` 的条目**给豁免清单（`structural` / `suiteGates`）；② 锚 3 demo 对账（`pod-coordination.md` §11）问的是 **demo 证据形态 vs 本包效果段**；③ verifier 侧没有任何 agent（该角色 2026-07-25 已退役，只剩 stage 身份，round fact 是 `verify.mjs` 产出的 14 字段机器闭集）。于是「设计面 vs 验证面」这条轴上**不存在独立信号源**：acceptance-author 与 verifier 共享同一套验证面假设（同一 testHost、同一包边界、同一断言语言），绿信号对双方一致——唯一在这条轴上独立的是真机轮 + human 持板目视（两次「界面还是老的」即由它戳破）。

**证据**（全部亲核，2026-09-20）：
- **review-packet 必列项无此段**（`dev-loop.md:242-247` verbatim，逐条）：「`- change size：…`／`- verification evidence：结构闸、功能闸、安全闸、mutation check、docs-sync。`／`- design-review evidence（若 TaskPackage 声明 \`designReviewRequired\`）：…`／`- findings：…`／`- five-axis review：…`／`- residual risk：已知限制、后续建议、人类决策点。`」——**五轴是 correctness / test quality / contract-boundary safety / operability-security / maintainability-simplicity，其中没有一条问「有没有齿」**。
- **ARQ 处方的落点不存在**（`dev-loop.md:84` verbatim）：「`verifier` 无 .md 定义文件（2026-07-25 gov 批退役）：保留为 **stage 身份**，行为合同由 `verify.mjs` round 证据 + `verifier-evidence-check.mjs` + scope-map 条目机械承载（**不派 agent**…）」⇒ ARQ 原文「落点候选 = verifier agent def / verdict 模板约定」的**前半不存在**；后半「verdict 模板」在 dev-loop 包内也已退役（`verdict.json` + `verification.json` 是**机器产线件**，由 `verify.mjs`/机械闸 fs-write，不能承载判断型申报）。**处方须重新落点**，本 T 的改法即重新落点结果。
- **实况：申报在自发发生，但无统一位置**——`grep -rln "半闭环\|未执行\|deferred\|未做" delivery/changes/dev/*/review-packet.md | wc -l` → **23**（分母 `ls delivery/changes/dev/*/review-packet.md | wc -l` → **33**）⇒ 约七成的包收口 prose 里出现过未闭环类措辞，**但都是作者自发的散文**，位置各异、粒度各异。
- **最近一例（verbatim，`delivery/changes/dev/260920-m2-charwindow-display-fix/review-packet.md:43`）**：「| 真机/live 腿（human 持板，MCD-A6） | **未做、deferred**——MCD-A6 为 structural 条目在册：① 真实遮挡消除（层序渲染只有上屏可证，源锚齿只是决策代理）… | **一致但未闭环**——… |」——该申报写在 **「Demo 对账行」表内**（即 260912 锚 3 的对账位），且同文件 `:47-57` 的「本包特有事项」另有自由发挥段。**它是「锚 3 恰好覆盖到这一条」的副产品，不是「本包有没有未覆盖面」的回答**（MCD-A6 是 structural 豁免条目，属①的口径；若某设计性质**根本没进 `acceptance[]`**，三道近亲全部沉默）。
- **携带形态的现成先例**（`pod-coordination.md:196` verbatim）：「**义务的生产者可达性由 planner Step 11 派 integrator 的 dispatch prompt 显式携带**（与锚 2 brief 携带同构，派发侧传递；**integrator.md 不因此改**）」——即 260912 批给出了一条**不改 integrator.md 也能把 review-packet 新义务送达**的通路。

**根因**：**设计 vs 验证面这条轴上没有独立信号源，也没有申报义务**。三道近亲都在「已经被承认的条目」内部打转（coverage 看条目、锚 3 看效果段），而问题恰在**条目集合本身可能漏了设计面**——`acceptance[]` 是 planner/acceptance-author 写出来的，写漏了什么，任何消费该集合的机器都不会知道。要补的不是新门，是一份**对照设计面给出的差集**：谁来给、什么时候给、写在哪 —— 现在三项全缺。

**改法**：
1. **`dev-loop.md`「### 合并门 Review Packet」必列项增一条**（置 `residual risk` 之前，逐字形态见下）：**未覆盖面申报**——对照本包 `acceptance[]` 集合 + 本包引用的契约条目 + roadmap §3 效果段，列出**本次无齿 / 未跑的设计性质**，逐条一行写「性质 + 现状 + 去向（deferred 到哪包 / structural 豁免 id / 由真机轮或 human 持板兜 / 明确接受不覆盖）」。**零缺口时明写 `none`**（防「没写」与「没有」混同）。
2. **`pod-coordination.md` §11 增一小节**（与 demo 对账行**同址**，理由：二者是同一次收口的两个正交轴——**demo 对账行管证据形态轴**（有什么证据、与效果段一致否）、**未覆盖面申报管齿覆盖轴**（设计面上有没有齿），同址可让写的人一次想两轴）。
3. **`planner.md` Step 11 的携带句扩一档**：现句只携带「review-packet 必含 demo 对账行」，扩为「**必含 demo 对账行 + 未覆盖面申报**」，其余照旧（不一致 = 走既有 ESCALATION-BRIDGE）。**`integrator.md` 不动**——承 260912 锚 3 的「派发侧传递、integrator.md 不因此改」形态（该形态已落地并经核验）。

**为什么这样改**：① **最小可判**——不新增门、不新增字段、不改任何机器件，只在**已必交的 review-packet** 上加一段、在**已有携带位**上加一句；② **对根因**——让「设计面差集」有一个强制出现的位置和一个能看到设计面的人（integrator 收口时会读 roadmap 效果段与 acceptance 集合，是链上最后一个同时看得到两端的人）；③ **避开处方失效面**——不像 ARQ 原文那样依赖不存在的 verifier agent def。

目标文件：`.claude/references/dev-loop.md`（改）· `.claude/references/pod-coordination.md`（改）· `.claude/agents/planner.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 拆包/起包 SSOT | `.claude/references/task-intake-and-slicing.md` | G2 | T1 增 `crossPackageBehaviors` 字段级定义（canonical）。消费方 = portfolio-planner（拆包期登记）、门 1 判官（人读）、本批新派生路 |
| 输入面派生器 | `.claude/scripts/brief-inputs.mjs` | G3（**只读派生，非门**） | T1 增第五路「跨包承接面」。消费方 = planner Step 7 派发、两份 input-brief 的子 agent、`--self-check`。**明示不参与任何门判定** |
| 验收证据约定 | `.claude/references/acceptance-evidence.md` | G2 | T2 增「契约条目 ↔ 齿 的强度申报」节（本件 4）+ **搭车件 6 增「evidence 类扫描齿的 root 注入」节**（OQ2 裁 (a)）。消费方 = acceptance-author（Inputs 已含本文件）+ 该 agent 的交付面 |
| 验收作者定义 | `.claude/agents/acceptance-author.md` | G2（**agent def，OQ1 对象**） | T2 只加 References 引用行（结构零改动；该文件现有 3 条同形引用行：`:45` 260906 T3/T8、`:46` 260907 T5、`:47` 260907 T6/260908 T4）+ **搭车件 5 再一条**（tighten-only §6 引用行）——共两条引用行，**两条之外零改动** |
| 合并门制品约定 | `.claude/references/dev-loop.md` | G2 | T3 增 review-packet 必列项一条。消费方 = integrator（收口产出）、supervisor 合并门（核存在）、human |
| Pod 协作约定 | `.claude/references/pod-coordination.md` | G2 | T3 增 §11 一小节（与 demo 对账行同址）。消费方 = integrator / planner |
| 单 Pod 编排者定义 | `.claude/agents/planner.md` | G2（**agent def，OQ1 对象**） | T3 扩 Step 11 携带句一句（`planner.md:78` 现有同形态句即 260912 锚 3 落地物） |
| 契约面 / G1 / hooks / scope-map / settings | — | **未触** | 零改动；`docs/contracts/**` 与 spec 两文件零触碰 |
| 工程门新增 | — | **零新增** | 三条改法均不新增门、不改任何既有闸判定语义 |
| agent frontmatter | — | **未触** | `name`/`tools`/`model` 一字不动（OQ1 若裁 (b)，则上表两行 agent def 条目一并移出本批） |

**【硬项 · sweep 透镜义务】** 本批不落地新判定 pattern（零新 gate、零新判据），故无 pattern 全仓预跑义务。**但落一条新「段名」进 review-packet 义务**：起草期已现跑核该段名**在机制面（`.claude/**`）零命中**、全仓仅 2 处出现且都在本条来源侧（`grep -rn "未覆盖面申报" .claude/ delivery/ | grep -v <本件>` → 2 条，均落在 `delivery/changes/dev/260829-hardware-validation/RETRO-VERIFICATION-GAP-260911.md:99/:139`——即本条的来源 retro，非既有义务）⇒ 不构成与既有段撞名。**T1 的字段名 `crossPackageBehaviors` 的消费面 census 已现跑**（8 个包实例、脚本面 0 命中，见 T1 证据段），判官只需 spot-check。

**【硬项 · 复扫模式完备性先验】** 本批不涉退役语面 / 残留清扫，无清扫模式集。**但含一处「同一判据两处消费」的形态**（承批② F1/F7 教训）：T1 新增的派生路与 T3 新增的必列段**都在消费「owner / 承接」这一族语义**——起草期已逐点列出消费方（字段写入方 = portfolio-planner；读取方 = `brief-inputs.mjs` 第五路；人读方 = 门 1 checklist ③ / review-packet 申报段），**四者共用「owner 指向承接包 changeId」这一条语义**，落地时须同步核对、不得只改一处。

## 4. human 取舍点（OQ）

### OQ1 · agent def 触面：本批两条改法要不要动 agent def

**背景**：三条改法里有两条要落 agent def——T2 的一条 **References 引用行**（`acceptance-author.md`）、T3 的一句**正文携带句**（`planner.md` Step 11）。仓内先例**两向都在**，且都不是孤例：

- **落了的（Track B 落地，已合并）**：`acceptance-author.md:45-47` 已载 **3 条**同形态引用行（260906 批 T3/T8、260907 批 T5、260907 批 T6 + 260908 批 T4）；`planner.md:78` 已载 **260912 锚 3 的正文携带句**（「workstream 包的 dispatch prompt 显式携带『review-packet 必含 demo 对账行』」）。两次都是 Track B、都经核验 PASS。
- **没落的**：`260918-gov-review-closeout-chain-friction` §6 留根② 明确把**一条** acceptance-author 引用行留根，理由原文「动 agent def 触 self-def 判断点，**不值得为一条引用行把本批拖进该争议**」（该批 §0 已声明不动任何 frontmatter）。
- **协议条文本身两处不一**：`arch-review-protocol.md:98-99`（Track A 新 idea 轨的机检硬排除）把「改 agent `tools`/`name`/`model`/write scope / authority 语义」与「改 self-def / review-loop agent 定义」列为 `HUMAN_REQUIRED`；同一文件 `:80` 又写「**governance 类变更（含 self-def / review-loop agent defs）不再强制升级**——治理形态由 human 定义，Track B 评审循环的对抗力度即为足额」。

> **(a) 按既成先例落（推荐）**：本批两处改动**是 References 引用行 + 一句携带句**——零 frontmatter、零 write scope、零 authority 语义、零工具面；且两者都是**对既有已落地形态的重演**（同一文件、同一节、同一句法）。载体历史处置（260906 / 260907 / 260912）均为 Track B 落地。**推荐理由一句话**：本批的价值之一正是把这个判断点摊开谈，而 (a) 把谈的结果固定成「有先例、按先例走」。
> **(b) 只落 references、agent def 一侧全留根**：承 closeout-chain 的谨慎形态。**代价**：T3 的携带臂缺失——义务退回「靠 integrator 自发翻阅 references」，这正是 `ARQ-260913-auto-mode-classifier-legit-workflow` 追记里记的「只落口径臂」教训；T2 的引用行缺失则少一次注意力提示（该文件本身已在 acceptance-author 的 Inputs 里，代价略轻）。
> **(c) 先由 human 定通则、再落**：本批 agent def 一侧全部留根，human 另立一批定「agent def 的引用行 / 正文句是否算 self-def」的通则。**代价**：本批三条改法里两条的半边悬空；**收益**：未来所有批不再逐批撞这个点。

**这项为什么该 human 裁**：它是一条**通则级**判断（「哪些 agent def 改动算触 self-def」），不是三条改法的技术内容；且协议条文自身不一致（`:98-99` vs `:80`），按「存疑向严」须由人定，agent 不得自裁。

### OQ2 · 搭车项是否并入本批

**背景**：剩余计划里列了四条「可与 P3 同判 / 同址 / 同判断点」的在册 ARQ 残余：

| 候选搭车项 | 内容 | 与本批的关系 |
|---|---|---|
| `ARQ-260908-tighten-only-unwritten` 残余 | `acceptance-author.md` **一条** References 引用行（其 DEV-GOVERNANCE §6 一侧已由 260918 closeout-chain 落地） | **同文件、同形态**（且正是 OQ1 的第二个实例） |
| `ARQ-260908-known-red-count-no-mechanical-source` 子项② | `acceptance-evidence.md` 增一节「evidence 类扫描齿显式接收仓根、禁依赖调用 cwd」撰写约定（子项③属 host 测试基建面） | **同文件**（T2 的落点文件），不同主题 |
| `ARQ-260913-current-pointer-implicit-global` 残余①③ | ① planner 起包 checklist 核 `current/` 指向（触 agent def）；③ 指针副本刷新走字节级复制 | ① **同判断点**（agent def），③ 不同面 |
| `ARQ-260914-legacy-sxay-story-id-batch` | 批量把老口径 `Sx-Ay` 例句换成 `S-Fx.y`（跨 10 个文件，含 3 个 agent def） | 仅「都碰文件」层面的搭车，非同一问题类 |

> **(a) 只并入前两条（推荐）**：省一轮评审与一次 landing，且二者**不扩半径**——都落在本批已有的目标文件里（`acceptance-author.md` / `acceptance-evidence.md`），**landing-manifest 一字不变**。**推荐理由一句话**：(a) 拿到的节省是实打实的（判官每轮半径不变），而 (c) 里的 legacy-sxay 是纯机械批量换例句、跨 10 文件，并进来会把「半径小到判官能逐行核完」这条本批唯一的比较优势吃掉。
> **(b) 四条全并**：一轮做完所有「agent def 面 + 引用行」的债。**代价**：legacy-sxay 的 10 文件批量把核对面从「七处逐行核」变成「十七处 + 一句例句替换的完备性论证」，与本仓 260918/260920 两轮「拆小批」的实证结论相反。
> **(c) 全不并**：本批只 T1–T3，最干净；两条同址项的节省留到各自批。

**这项为什么该 human 裁**：它决定的是**批的半径**（挂载面扩张），而半径直接换取判官逐行核对的深度——是本仓反复付过学费的取舍点。

### OQ 裁决（human 2026-09-20）

| OQ | 裁决 | 备注 |
|---|---|---|
| OQ1 | **(a) 按既成先例落** | 两处 agent def 改动（T2 的引用行、T3 的正文携带句）按 260906/260907/260912 先例走 **Track B**；零 frontmatter / 零 write scope / 零 authority 语义。**判官若判其为 `HUMAN_REQUIRED`（协议 `:98-99` 一侧），即升 human —— 本裁决不预支该判定。** |
| OQ2 | **(a) 只并入与落点同址的两条** | 并入 = T2 的搭车件 5（tighten-only 残余引用行）+ 6（known-red 子项②）；**landing-manifest 维持七路径不变**。`ARQ-260913-current-pointer-implicit-global` 残余①③ 与 `ARQ-260914-legacy-sxay-story-id-batch` 不并入，留各自批（§6 留根④）。 |

## 5. 验证集（landing 前机械前置 + landing 后核验）

**V1 · 起草期干跑（FROZEN 前）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260920-gov-review-bearing-strength-align/PROPOSAL.md` → 预期 `ok:true`（两类形态错零命中）。**本件起草期已跑，结果 verbatim（2026-09-20）**：`{"ok": true, "gate": "landing-preflight", "section2BodyFound": true, "anchorCount": 7, "findings": []}`（`anchorCount` = 7，与 landing-manifest 七路径一致）。
**V2 · 派生器自检（含新增路夹具，真空绿规约）**：`node .claude/scripts/brief-inputs.mjs --self-check` → 预期全绿，**断言数由基线 23 增至 ≥26**（基线 23/23 为 2026-09-20 现跑值）。新增三条夹具须**先断言夹具输入非空**（被指向包确实存在且 `crossPackageBehaviors` 非空）再断言输出：① 命中（owner == 本包 → 列出）；② 零命中（无指向本包 → 段落明写 `none`）；③ 排除（owner == 别包 → 不得出现）。
**V3 · 路由回归（planner.md 改动后必跑）**：`node .claude/scripts/route-regression-check.mjs` → 绿；`--self-check` 亦绿（证必留路由信号可证伪）。
**V4 · description lint**：`node .claude/scripts/lint-descriptions.mjs` → exit 0（本批不动 description，属回归）。
**V5 · 派生面端到端干跑（真包）**：对一件**有上游指向本包**的真包跑 `node .claude/scripts/brief-inputs.mjs --change-id <id> --role builder --dry-run` → 输出含「跨包承接面」段且逐条 `owner == <id>`；对一件无上游指向的包 → 该段明写 `none`。**命令与输出摘要进 landing summary**。
**V6 · 闭集核对**：`git status --porcelain` + `git diff --numstat` 逐路径核对 = landing-manifest 七路径，零多零少。
**V7 · 语义面复核（人读一遍式）**：改后 `grep -rn "crossPackageBehaviors" .claude/references/task-intake-and-slicing.md` ≥1；`grep -rn "未覆盖面申报" .claude/references/dev-loop.md .claude/references/pod-coordination.md .claude/agents/planner.md` 三处命中；**搭车件复核**：`grep -n "tighten-only\|§6" .claude/agents/acceptance-author.md` 命中新引用行、`grep -n "root\|cwd" .claude/references/acceptance-evidence.md` 命中新增节。
**V8 · 反空过（对偶 mutation 思路）**：T1 的派生路落地后，**故意把一件真包的 `owner` 改成不存在的 changeId** → 预期该条目**不再出现在** input-brief（证明过滤真生效、非「一律列出」）；还原后复跑一致。命令/输出进 landing summary（本条为**证明力**检查，非门）。

**landing 后核验**：`landing-verifier` 独立重跑 V1/V2/V3/V5/V6/V7，并逐文件人读 diff 核对改法 == §2（尤其 OQ 裁决后的裁剪）。

## 6. 留根清单

- **留根① · `crossPackageBehaviors` 的机器形状校验未落**：`taskpackage-required-check.mjs` 对 present-but-malformed 的校验本批**不做**——理由：本批已动一个 G3 脚本（`brief-inputs.mjs`），再动一个**承重校验**脚本会让「同批两个 G3 面」成为半径扩大的新口子；且该字段眼下有 门 1 人读 checklist 兜着。**触发条件**：出现第一个字段写错却不被任何环节发现的实例，或该字段消费方增至 ≥3。
- **留根② · 簿记结移**（落地时随 batch 执行，非改法）：`ARQ-260911-split-package-implementation-prose-gap` 结移 §2（已由 260912 批 T2 结清，证据 = `task-intake-and-slicing.md:234-236`「plan.md 全量义务…『装配型豁免』全取消（OQ3=(a)，260913 human 裁）」+ 本批 Status 行的现跑 census）；同时按同批体例**订正**该行原记的「19 个 MVP 拆包全无 `plan.md`/`planRef`」（时点值，现况：35 包中 15 有、260912 之后 11/11）。
- **留根③ · 存量制品不追溯**：34 份存量 review-packet 不补「未覆盖面申报」段；8 个存量包的 `crossPackageBehaviors` 形态不回改（含 260911 三包的早期形状）——义务自本批起对新包生效。
- **留根④ · 未并入的搭车项**（OQ2 裁 (a) 后确定）：`ARQ-260908-known-red-count-no-mechanical-source` 子项③（host runner 报告头写执行目录——属 host 测试基建面）、`ARQ-260913-current-pointer-implicit-global` 残余①③（① 起包 checklist 核 `current/` 指向〔触 agent def〕+ ③ 指针副本刷新走字节级复制）、`ARQ-260914-legacy-sxay-story-id-batch`（`Sx-Ay` → `S-Fx.y` 例句批量换新，跨 10 文件、含 3 处 agent def）——各自留原批。
- **留根⑤ · 判断型申报不机器化**：`verifier-evidence/rounds/*.json` 的 14 字段闭集、`verdict.json`/`verification.json` 一律不动——「未覆盖面」「齿强度」是判断型数据，塞进机器闭集会造成「机器件承载了不可重算的东西」（该闭集的权威性正建立在可重算上）。**若未来要将本 T 的两段申报机械化，须单开一批并重新论证可判性。**
- **留根⑥ · 交日落复盘项**：`/loop-retro` 观察两件事——① 新包 review-packet 的「未覆盖面申报」段是否真写、是否退化为一句话套话；② `acceptance-author` 的强度申报是否在冻结件头注真出现（遵从率）。**退化为套话即说明本条选错了承载位**（届时评估是否改为 planner 派发期派生的封闭清单，回收 T1 的同款形态）。
- **自律件（不进模板的环内纪律）**：起草期对四条 ARQ 处方做了 6 项复证，**两条被证伪/收窄**（第四条已结清、第一条只剩派单侧、T3 处方落点不存在）——**「ARQ 的建议改动是待验证假设」这条纪律本批再得一次实证**，落地后建议在 `gov-arq` skill 侧重申（与在册 `ARQ-260908-*` 同族的「处方验证状态标注」建议合流）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
