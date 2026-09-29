# PROPOSAL · 260929-gov-review-ui-ladder-remap

> **状态：DRAFT v0.1 · 2026-09-29 · 起草期，human 已立项、PROPOSAL 先呈 human 过目后再派 architect 评审**。
> **承载 ARQ**：无——human 直接 idea 立项（2026-09-29 会话，紧随 260929-gov-review-req-loop-ui-author landed 之后的后续批）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（闸向严：新增 launch 前置检查与 fail-closed 停包路由，不放松任何既有门；无契约触面；收缩一个角色翻译位）。
> **review profile**：G2+G3（触面 = dev-loop 流程 SSOT + agent defs 为 G2；`route-fixtures/routing.json` + `scope-map.json` 为 G3；G1 = 0，契约面 = 0）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：GREEN 结论前必须在场（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/references/task-intake-and-slicing.md
.claude/agents/planner.md
.claude/route-fixtures/routing.json
.claude/agents/uiux-author.md
.claude/references/dev-loop.md
.claude/agents/acceptance-author.md
.claude/references/requirement-change.md
.claude/scope-map.json
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：UI 设计决策全部上收到需求/治理面（上一批已接线的 requirement-change 档 2 4b），dev-loop 对 UI 只剩三种合法动作——**消费**（L0/L1）、**起包前核 ready**（planner 前置检查触 UI 包的 frozen 页图在位，缺则 fail-closed 停在起 Pod 前）、**兜底升级**（实现期撞出设计缺口 = 停包路由需求面）——不再有「包内翻译」与「包内设计」两个环节。

**现状差距**：UIUX Depth Ladder（`task-intake-and-slicing.md`「UIUX Depth Ladder」节，line 318–343）是「uiux-designer 未接线」时代的产物：L2 教 dev-loop 在包内做交互 brief 翻译（派 uiux-author 模式 A）、L3 教在包内用 OpenDesign 出视觉参照——设计动作被默认留在实现期。上一批（260929-gov-review-req-loop-ui-author，landed）接线了上游供给后，L2 名为设计深度、实为消费动作；L3 出现在实现期本身就是异常信号。且 uiux-author 模式 A 实证到场率趋零（53 包仅 1 包 uiuxRequired=true 且唯一那次 def 缺位直翻降级），其制品 `uiux-brief.md` 的消费面已被 brief-inputs.mjs ①c 透传替代。

**非目标（本批不做）**：

- 不动 brief-inputs.mjs ①c（uiux 设计 refs 透传在位才列——`uiux-brief.md` 制品取消后该路自然恒走 none 臂，脚本零改动）。
- 不动 requirement-change 档 2 4b 生成模式本体（上游供给链零改动）。
- 不动 L0/L1 语义与 UI-state 验收机制。
- dev-loop「实现期小视觉微调」（不产生新设计语义）仍走 L1 + designReview，不设新通道。

## 2. 问题、根因与改法（T1–T6）

**【硬项 · 起草期实证义务】**本节的断言纪律：凡引用 file:line → 起草期现读核对（各 T 证据段所引均为 2026-09-29 实读/实查）；凡声称「比照先例」→ 先读先例文件确认形态再落笔（T2 的 fail-closed 形态比照 planner Step 5 既有「brief 缺失降级自扫并标 manifest-missing（不静默）」句式与 Step 4.5 生成模式 🟡 未消解不出图句式）。

**【硬项 · 目标文件锚清单】**每 T 末尾显式列「目标文件」；landing-manifest ≡ 各 T 并集。

### T1 · UIUX Depth Ladder 重排（设计上收、消费留下）

**问题**：梯子 L2/L3 把设计动作留在 dev-loop——与上游供给链（需求档 2 4b 已出设计出图）双头设计，且 L3 在实现期出设计违反「不带病下行」纪律。

**证据**：`task-intake-and-slicing.md:322-330`——L2 行「`uiux-author` creates an interaction brief from the frozen system brief」、L3 行「OpenDesign visual reference + human design direction as needed」、line 330「L2+ requires at least a baseline `UIUX-DESIGN-BRIEF.md`」。上游先例：requirement-change.md 档 2 4b（本仓 2026-09-29 landed，出图定例 = 400×300 PNG 布局定位权威）。

**根因**：梯子成文时系统级 UI 生成编制未接线（uiux-author def 旧版自述「归 uiux-designer / governance-lane，尚未接线」），设计只能下沉实现期；该前提已被上一批消除。

**改法**：梯子节重排为「上游供给缺多少、下游升多少」语义——

| 级 | 新定义 | 动作 |
|---|---|---|
| L0 / L1 | 不变 | 不变（L1 仍 UI-state 验收 + designReview 容纳小视觉微调） |
| L2 | 包涉 UI 且上游 frozen 页图**齐** | 消费：planner 起包时把逐页 PNG ref 写进 uiux block（`designRefs[]`），不再派翻译角色 |
| L3 | 包涉 UI 且上游 frozen 页图**缺**（新页面/新 IA/新状态无图） | **设计缺口信号**：fail-closed 停在起包前，路由 requirement-change 补设计（human 裁意图 → 档 2 4b 出图 → frozen 后包再续） |
| L4 | 不变（系统导航/shell/跨系统组件） | 系统蓝图**画**的位置 = requirement-change 档 2 4b 生成模式（与其他 UI 同一条供给链，human 设计方向裁定恒在人）；蓝图变更若构成治理面变更（导航模型/shell 语义进治理文档），其**审**走 /arch-change governance profile UIUX 轴集——只审不画 |

连带句：line 330「L2+ requires baseline brief」改为「触 UI 包的 launch 前置 = 冻结页图 PNG 在位且 UIUX-DESIGN-BRIEF 版本行注明对齐的本批 requirements 版本号（T2 机械判据）」；字段说明段「Add them only from Level 1+（some）/ Level 2+（interaction brief）」随 interaction brief 制品取消改为「Level 1+（uiAcceptanceRefs）/ Level 2+（designRefs 逐页 PNG）」。

目标文件：`.claude/references/task-intake-and-slicing.md`（改）

### T2 · planner Step 6b 重写（UI-ready 前置检查 + 挂图并入起包）

**问题**：planner 现行 Step 6b 的动作是「L2+ 派 uiux-author 翻译」——翻译位撤销后，起包动作改为「核 ready + 挂图」，且检查必须在**起 Pod 前**拦住设计缺口，不能等实现期撞墙。

**证据**：`planner.md` Step 6b（line 61，2026-09-29 实读）：「按 task-intake-and-slicing.md 的 UIUX 字段判定 uiuxRequired/uiuxLevel。若 uiuxRequired=true && uiuxLevel>=2，在派 acceptance-author 之前先派 uiux-author……冻结 brief 缺失或仍是 draft → 停（L2+ 需 baseline brief）」——停的判据是 brief 缺失，不含「页图 PNG 缺」；且 stop 动作未定义路由去向。fail-closed 先例：同文件 Step 9 「brief 缺失时降级自扫，并在交付报告标 `manifest-missing`（不静默）」。

**根因**：Step 6b 是旧梯子的投影（同 T1 根因）；UI-ready 判据与路由去向在旧前提下不存在。

**改法**：Step 6b 重写为两段——

1. **判级**（保留）：按新梯子判 L0–L4。
2. **触 UI 包的 UI-ready 前置检查**（新）：逐页核 `docs/ui/` frozen 页图 PNG 在位（文件名对齐 pageId/stateId）+ UIUX-DESIGN-BRIEF 版本行注明对齐的本批 requirements 版本号（机械判据与 requirement-change.md「消费保护规则」节同款）。齐 → L2 消费：planner 把逐页 PNG ref 写进 uiux block `designRefs[]`（翻译并入起包，不再派角色）；缺 → **L3 设计缺口：fail-closed 停在起包前**，向主线程/ human 报缺口页清单并路由 requirement-change 补设计，不带病 launch。
3. dispatch 输入构成相应更新：acceptance-author / builder 的 UI 输入 = uiux block `designRefs[]`（经 brief-inputs ①c 透传），不再是 uiux-brief.md。

目标文件：`.claude/agents/planner.md`（改）· `.claude/route-fixtures/routing.json`（改，见 T2b）

**T2b · routing.json planner 白名单同步**：`route-fixtures/routing.json:91` 的 planner 可派列表含 `"uiux-author"`——翻译位撤销后 planner 不再派该角色，token 删除；route-regression-check 复跑作为验证（若该守卫断言白名单形状，同步断言面）。**tools 高敏字段纪律**：planner def frontmatter `tools: Agent(...)` 白名单同批删 `uiux-author` token，逐字段隔离、不误伤其余 token。

目标文件：`.claude/route-fixtures/routing.json`（改）

### T3 · uiux-author def 收缩为生成模式单模式

**问题**：模式 A（翻译）撤销后，def 的双模式结构、`uiux-brief.md` 制品义务、TaskPackage 目录写面全部过时。

**证据**：`uiux-author.md`（2026-09-29 实读，上一批 FROZEN v1.0 落地版）——「## Role / 职责」双模式句、「## 模式 A · 翻译（dev-loop）」整节、模式 A 步骤 2「产出 `uiux-brief.md`（TaskPackage 目录 sibling）」。实证：53 包仅 3 包带 uiuxRequired 字段、true 仅 1 包且该次 def 缺位（T5 证据同上一批）；其 builder 消费面已被 brief-inputs.mjs ①c 替代（该批 landed）。

**根因**：翻译位的存在前提（系统 brief 与包之间需要独立角色转述）被 ①c 机械透传消除；筛选判断（挑本包相关页/标缺口）并入 planner 起包（T2）后，独立翻译角色无剩余职责。

**改法**：def 重写为**生成模式单模式**——①删「模式 A · 翻译」整节与 `uiux-brief.md` 制品义务；②Role 句改为「需求 loop 档 2 触 UI 批次的设计执行角色」；③When/Inputs/Process 保留模式 B 现文（可行性消解后派、需求修订句唯一设计依据、出图规格单、od_* 归主线程）；④Boundaries 写面只留 `docs/ui/**`；⑤description 重写保路由信号（requirement-change 档 2 触 UI / 档 2 流程 4b / 主线程派），删 Step 6b 翻译信号，控长 ≤320。

目标文件：`.claude/agents/uiux-author.md`（改）

### T4 · dev-loop SSOT 与 acceptance-author 同步句收口

**问题**：dev-loop.md 与 acceptance-author def 各有一句「planner 派 uiux-author 翻译」的旧投影——SSOT 与 def 层不同步即双源漂移（上一批 R1-F1/F2 同源教训）。

**证据**：`dev-loop.md:55`（流程图行「├─ 派生 uiux-author（条件）仅当 uiuxRequired && uiuxLevel>=2：从冻结的系统 brief 译出 TaskPackage uiux 块」）、`dev-loop.md:68`（「`uiux-author` 是条件步骤……它只翻译」整段）、`acceptance-author.md:23`（「L2+ 时由 planner 先派 uiux-author 翻译好后带入」）。

**根因**：同 T1/T2——三处均为旧梯子的投影点，梯子重排后须逐一收口。

**改法**：①`dev-loop.md:55` 流程图行改为「├─ UI-ready 前置检查（条件）触 UI 包：planner 核 frozen 页图在位，缺 → 停包路由 requirement-change（梯子 L3）」；②`:68` 整段改写为 UI-ready 检查说明（判据、fail-closed、路由去向）；③`acceptance-author.md:23` 改为「UI 输入 = TaskPackage uiux block 的 `uiAcceptanceRefs[]` 与 `designRefs[]`（planner 起包期写入，经 brief-inputs ①c 透传）」——acceptance 消费形态不变，仅供给方描述更新。

目标文件：`.claude/references/dev-loop.md`（改）· `.claude/agents/acceptance-author.md`（改）

### T5 · requirement-change.md launchPreconditions 留根落点

**问题**：requirement-change.md「定稿载体」节留根句「launchPreconditions 消费挂点归 ARQ-260928 批（留根）」——本批 T2 的 UI-ready 检查正是该挂点的 UI 面首次落地，SSOT 侧须有对应正式句，否则 dev-loop 侧判据无需求面锚。

**证据**：`requirement-change.md`「定稿载体」节（2026-09-29 实读）：「launchPreconditions 消费挂点归 ARQ-260928 批（留根）」；「消费保护规则」节 frozen 机械判据句（上一批扩写版）。

**根因**：留根句记录的挂点在本批具备落地条件，且两 SSOT 须互引防漂移。

**改法**：「消费保护规则」节末尾加一句：「**launchPreconditions · UI-ready 消费**：dev-loop 起 Pod 前的触 UI 包前置检查（frozen 页图在位 + brief 版本行对齐本批 requirements 版本号）以本节机械判据为权威，判据修订只改本节（防双源）。」story 引用版本号机检齿留根**保持留根不变**（仍等实包摩擦）。「定稿载体」节留根句相应改指「UI 面已落（见消费保护规则节），story 引用版本号机检齿仍留根」。

目标文件：`.claude/references/requirement-change.md`（改）

### T6 · scope-map 写面收紧（随翻译位撤销）

**问题**：uiux-author allow 仍含 `delivery/taskpackages/**`（模式 A 写 uiux-brief.md 的槽）——翻译位撤销后该写槽成为无主授权，违背最小扩权原则。

**证据**：`scope-map.json` uiux-author 条目 `"allow": ["delivery/taskpackages/**", "docs/ui/**"]`（2026-09-29 实读，上一批 T3 扩权后状态）。

**根因**：上一批为模式 A 授的写槽，模式 A 撤销后应同步收紧（扩权与收权同批对称）。

**改法**：allow 列删 `delivery/taskpackages/**`，只留 `docs/ui/**`；deny 列不动。收权不影响模式 B（生成模式只写 docs/ui）。

目标文件：`.claude/scope-map.json`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED = 0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| dev-loop SSOT | `.claude/references/task-intake-and-slicing.md` | G2 | 梯子节重排；消费方 = planner 判级 + triage-change 分档 |
| dev-loop SSOT | `.claude/references/dev-loop.md` | G2 | 角色步骤图与条件步骤段同步（T4）；消费方 = planner / Pod 流程读者 |
| 治理机制 | `.claude/agents/planner.md` | G2 | Step 6b 重写（UI-ready 检查 + 挂图并入）；tools 白名单删 uiux-author token（T2b） |
| 治理机制 | `.claude/route-fixtures/routing.json` | G3 | planner 可派列表删 uiux-author token；消费方 = route-regression-check（守卫复跑） |
| 治理机制 | `.claude/agents/uiux-author.md` | G2 | def 收缩生成单模式；消费方 = 主线程（档 2 4b 派）+ route-regression-check |
| 治理机制 | `.claude/agents/acceptance-author.md` | G2 | 供给方描述句更新（T4）；验收断言机制零改动 |
| 需求面 | `.claude/references/requirement-change.md` | G2 | launchPreconditions 消费句落地 + 留根句改指（T5） |
| 机械强制层 | `.claude/scope-map.json` | G3 | uiux-author allow 收紧（T6）；消费方 = enforce-scope 钩子 |
| 脚本面 | `.claude/scripts/brief-inputs.mjs` | 未触 | ①c「在位才列」——uiux-brief.md 制品取消后恒走 none 臂，零改动；回归由 V5 self-check 承载 |
| 上游供给 | requirement-change 档 2 4b | 未触 | 生成模式本体零改动；uiux-author def 收缩后该派发路径不变 |
| 契约面 | `docs/contracts/**` | 未触 | G1 = 0 |

## 4. human 取舍点（OQ）

三件方向性取舍已于 2026-09-29 会话裁毕：

| OQ | 选项 | human 裁定 |
|---|---|---|
| OQ-1 梯子 L2/L3 处置 | (a) 现在立项重排（推荐位）/ (b) 留根等首实跑批摩擦 | **裁 (a)**——梯子现为新旧混合态（L3 仍教包内 OpenDesign），留着重排会误导下一个 UI 包的 planner；摩擦数据留根 retro 补 |
| OQ-2 UI 缺口拦截位 | (a) planner 起 Pod 前置检查 fail-closed（human 提出）/ (b) 实现期升级信号兜底 | **裁 (a)** 为主拦截，(b) 降级为梯子 L3 兜底句——拦截位前移到起包前 |
| OQ-3 模式 A 去留 | (a) 撤并：筛选挂图并入 planner 起包，uiux-author 只留生成模式（推荐位）/ (b) 保留翻译 agent | **裁 (a)**——①c 透传已替代其消费面，实证到场率趋零；角色减一、dispatch 减一 |

无遗留 OQ——不涉挂载面扩张、不涉全会话新机制（前置检查是既有 launchPreconditions 留根的落地）、不涉不可逆操作（写槽收紧可逆）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

| # | 时点 | 命令 | 预期 |
|---|---|---|---|
| V1 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260929-gov-review-ui-ladder-remap/PROPOSAL.md` | exit 0；anchorUnion 成员 = manifest 八路径、无注记夹带路径 token |
| V2 | pre-commit | `node .claude/scripts/route-regression-check.mjs` | exit 0——planner 白名单删 uiux-author 后路由信号不退化（本批关键守卫：白名单形状变化必须过它） |
| V3 | pre-commit | `node .claude/scripts/lint-descriptions.mjs` | exit 0 · flagged 0——uiux-author description 重写后 ≤320 且路由信号在场 |
| V4 | post-commit | `node -e "JSON.parse(require('fs').readFileSync('.claude/scope-map.json','utf8'));console.log('ok')"` | 输出 ok |
| V5 | post-commit | `node .claude/scripts/brief-inputs.mjs --self-check` | 40/40 PASS——①c 零改动回归绿（uiux-refs 夹具含 none 臂，制品取消后该臂即常态路） |
| V6 | post-commit | `grep -c "uiux-author" .claude/agents/planner.md` | 输出 0——planner def 内派发句与白名单 token 清零（模式 A 撤销的负向断言） |
| V7 | post-commit | `grep -c "uiux-author\|uiux-brief" .claude/references/task-intake-and-slicing.md .claude/references/dev-loop.md .claude/agents/acceptance-author.md` | 各 ≥0 且 dev-loop.md 中「派生 uiux-author」句不复现（grep -c "派生 uiux-author" dev-loop.md = 0） |

真空绿规约：V1–V3 跑前先确认脚本入口在位；环境红先立基线再归因。V6/V7 为负向断言（撤并收口的机检形态），预期值即断言。

## 6. 留根清单

- **摩擦回收（OQ-1 裁定留根）**：首实跑批回收三问——① L3 设计缺口实际发生频率与停包路由成本；② UI-ready 检查的判据摩擦（版本行对齐人工核是否够）；③ 小视觉微调走 L1 + designReview 是否兜得住。交 loop-retro cadence。
- **taskpackage-required-check 机检齿**：UI-ready 检查现为 def 层义务、无脚本兜底——若首实跑批出现「检查跳过」，立项把检查落进 launch 前机检（比照 story 引用版本号机检齿留根句法）。
- **L4 与 /arch-change UIUX 轴集的接线**：梯子 L4 只改指路，governance profile 轴集审的 dispatch 细节未动——出现 L4 实例时再校准。
- **登记不硬治项**：planner 挂图动作（写 designRefs 逐页 PNG）无独立验证——由 ①c 透传的「悬空 ref advisory」间接兜底（挂错路径会报警），不另设门。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
