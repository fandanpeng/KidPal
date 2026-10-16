# PROPOSAL · 260929-gov-review-ui-ladder-remap

> **状态：FROZEN v1.0 · 2026-09-29 · 凭据：human 立项 ack（2026-09-29 会话，L4 指路修正 + OQ-4 追加裁定后确认）+ architect R3 GREEN（§7 R3 段；R3-F1/F2/F3 三处微修已随手完成、R2-F2 措辞注意转 landing-summary 留痕）+ OQ 四条已裁（§4）。主线程凭据自冻，授权记 Status 行。**
> **承载 ARQ**：无——human 直接 idea 立项（2026-09-29 会话，紧随 260929-gov-review-req-loop-ui-author landed 之后的后续批）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T7）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
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
.claude/scripts/brief-inputs.mjs
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：UI 设计决策全部上收到需求/治理面（上一批已接线的 requirement-change 档 2 4b），dev-loop 对 UI 只剩三种合法动作——**消费**（L0/L1）、**起包前核 ready**（planner 前置检查触 UI 包的 frozen 页图在位，缺则 fail-closed 停在起 Pod 前）、**兜底升级**（实现期撞出设计缺口 = 停包路由需求面）——不再有「包内翻译」与「包内设计」两个环节。

**现状差距**：UIUX Depth Ladder（`task-intake-and-slicing.md`「UIUX Depth Ladder」节，line 318–343）是「uiux-designer 未接线」时代的产物：L2 教 dev-loop 在包内做交互 brief 翻译（派 uiux-author 模式 A）、L3 教在包内用 OpenDesign 出视觉参照——设计动作被默认留在实现期。上一批（260929-gov-review-req-loop-ui-author，landed）接线了上游供给后，L2 名为设计深度、实为消费动作；L3 出现在实现期本身就是异常信号。且 uiux-author 模式 A 实证到场率趋零（53 包仅 1 包 uiuxRequired=true 且唯一那次 def 缺位直翻降级），其制品 `uiux-brief.md` 的消费面已被 brief-inputs.mjs ①c 透传替代。

**非目标（本批不做）**：

- 不动 requirement-change 档 2 4b 生成模式本体（上游供给链零改动）。
- 不动 L0/L1 语义与 UI-state 验收机制。
- dev-loop「实现期小视觉微调」（不产生新设计语义）仍走 L1 + designReview，不设新通道。

> 注：brief-inputs.mjs ①c 原列非目标（「脚本零改动」），R2 GREEN 后经 human 追加裁定（OQ-4）改为 T7 剥离 uiux-brief.md 子源——设计 refs 主路（designRefs/sharedUiRefs 透传）仍零改动。

## 2. 问题、根因与改法（T1–T7）

**【硬项 · 起草期实证义务】**本节的断言纪律：凡引用 file:line → 起草期现读核对（各 T 证据段所引均为 2026-09-29 实读/实查）；凡声称「比照先例」→ 先读先例文件确认形态再落笔（T2 的 fail-closed 形态比照 `acceptance-author.md:24` 既有「brief 缺失时降级自扫，并在交付报告标 `manifest-missing`（不静默）」句式与 `commands/requirement-change.md` 4.5 步 🟡 未消解不出图句式——引据位置经 R1 判官 grep 核正，R1-F6）。

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

**连带句（判据分形 + 指针化，R1-F2）**：line 330「L2+ requires baseline brief」改为「触 UI 包的 launch 前置判据（UI-ready）**权威 = requirement-change.md『消费保护规则』节 launchPreconditions 句**（T5），本节只指针、不复述正文」；字段说明段「Add them only from Level 1+（some）/ Level 2+（interaction brief）」改为「Level 1+（uiAcceptanceRefs）/ Level 2+（designRefs 逐页 PNG）」。

**旧义清扫（退役语面全形态枚举，R1-F3）**：同节三处超表格残留逐处处置——① :320 节引句「Depth drives whether a `uiux-author` step runs and what design-review evidence integrator owes at closeout」→ 改「Depth drives whether the pre-launch UI-ready check applies and what design-review evidence integrator owes at closeout」；② :338 JSON 示例 `"designRefs": ["docs/ui/UIUX-DESIGN-BRIEF.md#pageId-x"]` → 改 PNG 路径形态 `"designRefs": ["docs/ui/V5/canvas-p1.png"]`；③ :349「designRefs[] primarily point to stable UIUX-DESIGN-BRIEF.md sections/IDs; OpenDesign artifact refs … cannot replace the frozen textual contract」→ 改写为「designRefs[] point to frozen per-page PNGs（400×300，布局坐标定位权威——上一批定例）；brief 文本节仅作口径补充」。**复扫模式完备性**：清扫词形 = 模式 A / 模式A / Mode A / 翻译模式 / 翻译位 / 翻译角色 / uiux-brief / interaction brief / creates an interaction brief / OpenDesign visual reference / uiux-author step（词形变体 × 有/无 backtick）；判官 census 已扫 `.claude/**` 全域（§7 R1 census 段），目标文件内残留 = 上述三处 + T2 末句（R1-F4）+ T3 标题层（R1-F9），本批改法全覆盖。

**L3 停包路由闭环（R1-F5，四件全落不留根）**：① **留痕口径**：停包动作落 pod-events `agent-escalation` 事件（payload 注明缺口页清单与路由去向）——既有事件闭集类型，不新开通道；② **需求 loop 承接形态**：L3 缺口批进 requirement-change = 视为原 story 的**档 2 触 UI 修订批**（human 裁意图入口照常、§9 版本行照常 append）——二次入 loop 的档位判定由此回答：设计缺口 = 结构性牵连，天然档 2；③ **续包判据**：需求批 frozen 后 planner 重跑 T2 UI-ready 前置检查（对齐新版本行）再续包；④ **L4 交界**：单包命中 L4 时，蓝图先行（停包路由 requirement-change 补系统级设计）与拆包归 portfolio-planner 是先后两动作、不互斥——planner 停包、不越权拆包。

目标文件：`.claude/references/task-intake-and-slicing.md`（改）

### T2 · planner Step 6b 重写（UI-ready 前置检查 + 挂图并入起包）

**问题**：planner 现行 Step 6b 的动作是「L2+ 派 uiux-author 翻译」——翻译位撤销后，起包动作改为「核 ready + 挂图」，且检查必须在**起 Pod 前**拦住设计缺口，不能等实现期撞墙。

**证据**：`planner.md` Step 6b（line 61，2026-09-29 实读）：「按 task-intake-and-slicing.md 的 UIUX 字段判定 uiuxRequired/uiuxLevel。若 uiuxRequired=true && uiuxLevel>=2，在派 acceptance-author 之前先派 uiux-author……冻结 brief 缺失或仍是 draft → 停（L2+ 需 baseline brief）」——停的判据是 brief 缺失，不含「页图 PNG 缺」；且 stop 动作未定义路由去向。fail-closed 先例：`acceptance-author.md:24`「brief 缺失时降级自扫，并在交付报告标 `manifest-missing`（不静默）」（引据位置经 R1 判官 grep 核正——planner.md 无该句，R1-F6）。

**根因**：Step 6b 是旧梯子的投影（同 T1 根因）；UI-ready 判据与路由去向在旧前提下不存在。

**改法**：Step 6b 重写为两段——

1. **判级**（保留）：按新梯子判 L0–L4。
2. **触 UI 包的 UI-ready 前置检查**（新）：逐页核 `docs/ui/` frozen 页图 PNG 在位（文件名对齐 pageId/stateId）+ 版本行判据**分形**（判据权威 = requirement-change.md「消费保护规则」节 launchPreconditions 句，此处指针不复述）：需求批出生的包 = 版本行对齐本批 requirements 版本号；非需求批出生（bug 修复/重构包）= 页图在位且版本行对齐版本不晚于当前 frozen requirements 最新版本行。齐 → L2 消费：planner 把逐页 PNG ref 写进 uiux block `designRefs[]`（翻译并入起包，不再派角色）；缺 → **L3 设计缺口：fail-closed 停在起包前**，pod-events 落 `agent-escalation`（缺口页清单 + 路由去向，留痕口径见 T1 闭环件①），报 human 并路由 requirement-change（承接形态见 T1 闭环件②），不带病 launch。
3. dispatch 输入构成相应更新：**acceptance-author 直读 TaskPackage uiux block**（`uiAcceptanceRefs[]` + `designRefs[]`）；**builder 侧经 brief-inputs ①c 透传**（①c 为 builder 专列——brief-inputs.mjs self-check「uiux-refs 专列 builder」断言在册）——两侧分角表述，不再是 uiux-brief.md。
4. **Step 6b 末句处置**（R1-F4）：现行末句「系统级 UI 生成已接线 = …uiux-author 生成模式…不由你起草」删除——语义不丢：「不由你起草」边界并入判级段 L3 句（设计缺口归需求面）；生成模式指称由 requirement-change.md 档 2 4b 与 uiux-author def 自身承载，planner def 内 uiux-author token 清零（V6 断言据此成立）。

目标文件：`.claude/agents/planner.md`（改）· `.claude/route-fixtures/routing.json`（改，见 T2b）

**T2b · routing.json planner 白名单同步**：`route-fixtures/routing.json:91` 的 planner 可派列表含 `"uiux-author"`——翻译位撤销后 planner 不再派该角色，token 删除；route-regression-check 复跑作为验证（若该守卫断言白名单形状，同步断言面）。**tools 高敏字段纪律**：planner def frontmatter `tools: Agent(...)` 白名单同批删 `uiux-author` token，逐字段隔离、不误伤其余 token。

目标文件：`.claude/route-fixtures/routing.json`（改）

### T3 · uiux-author def 收缩为生成模式单模式

**问题**：模式 A（翻译）撤销后，def 的双模式结构、`uiux-brief.md` 制品义务、TaskPackage 目录写面全部过时。

**证据**：`uiux-author.md`（2026-09-29 实读，上一批 FROZEN v1.0 落地版）——「## Role / 职责」双模式句、「## 模式 A · 翻译（dev-loop）」整节、模式 A 步骤 2「产出 `uiux-brief.md`（TaskPackage 目录 sibling）」。实证：53 包仅 3 包带 uiuxRequired 字段、true 仅 1 包且该次 def 缺位（T5 证据同上一批）；其 builder 消费面已被 brief-inputs.mjs ①c 替代（该批 landed）。

**根因**：翻译位的存在前提（系统 brief 与包之间需要独立角色转述）被 ①c 机械透传消除；筛选判断（挑本包相关页/标缺口）并入 planner 起包（T2）后，独立翻译角色无剩余职责。

**改法**：def 重写为**生成模式单模式**——①删「模式 A · 翻译」整节与 `uiux-brief.md` 制品义务；②Role 句改为「需求 loop 档 2 触 UI 批次的设计执行角色」；③When/Inputs/Process 保留模式 B 现文（可行性消解后派、需求修订句唯一设计依据、出图规格单、od_* 归主线程）；④Boundaries 写面只留 `docs/ui/**`；⑤description 重写保路由信号（requirement-change 档 2 触 UI / 档 2 流程 4b / 主线程派），删 Step 6b 翻译信号，控长 ≤320；⑥标题层去模式化（R1-F9）：H1「UIUX Author（UIUX 翻译 / 生成双模式）」改「UIUX Author（需求 loop UI 生成）」、「## 模式 B · 生成」节标题取消、其下 When/Inputs/Process/Boundaries 平铺上移、Role 段「双模式：」引导词删除。

目标文件：`.claude/agents/uiux-author.md`（改）

### T4 · dev-loop SSOT 与 acceptance-author 同步句收口

**问题**：dev-loop.md 与 acceptance-author def 各有一句「planner 派 uiux-author 翻译」的旧投影——SSOT 与 def 层不同步即双源漂移（上一批 R1-F1/F2 同源教训）。

**证据**：`dev-loop.md:55`（流程图行「├─ 派生 uiux-author（条件）仅当 uiuxRequired && uiuxLevel>=2：从冻结的系统 brief 译出 TaskPackage uiux 块」）、`dev-loop.md:68`（「`uiux-author` 是条件步骤……它只翻译」整段）、`acceptance-author.md:23`（「L2+ 时由 planner 先派 uiux-author 翻译好后带入」）。

**根因**：同 T1/T2——三处均为旧梯子的投影点，梯子重排后须逐一收口。

**改法**：①`dev-loop.md:55` 流程图行改为「├─ UI-ready 前置检查（条件）触 UI 包：planner 核 frozen 页图在位，缺 → 停包路由 requirement-change（梯子 L3）」；②`:68` 整段改写为 UI-ready 检查说明（判据、fail-closed、路由去向）；③`acceptance-author.md:23` 改为「UI 输入 = TaskPackage uiux block 的 `uiAcceptanceRefs[]` 与 `designRefs[]`（planner 起包期写入；acceptance-author 直读 TaskPackage，builder 侧才经 brief-inputs ①c 透传）」——acceptance 消费形态不变，仅供给方描述更新（R1-F1 分角表述）。

目标文件：`.claude/references/dev-loop.md`（改）· `.claude/agents/acceptance-author.md`（改）

### T5 · requirement-change.md launchPreconditions 留根落点

**问题**：requirement-change.md「定稿载体」节留根句「launchPreconditions 消费挂点归 ARQ-260928 批（留根）」——本批 T2 的 UI-ready 检查正是该挂点的 UI 面首次落地，SSOT 侧须有对应正式句，否则 dev-loop 侧判据无需求面锚。

**证据**：`requirement-change.md`「定稿载体」节（2026-09-29 实读）：「launchPreconditions 消费挂点归 ARQ-260928 批（留根）」；「消费保护规则」节 frozen 机械判据句（上一批扩写版）。

**根因**：留根句记录的挂点在本批具备落地条件，且两 SSOT 须互引防漂移。

**改法**：「消费保护规则」节末尾加一句：「**launchPreconditions · UI-ready 消费**：dev-loop 起 Pod 前的触 UI 包前置检查（frozen 页图在位 + 版本行判据分形：需求批出生的包对齐本批 requirements 版本号，非需求批出生的包对齐版本不晚于当前 frozen requirements 最新版本行）以本节机械判据为权威，判据修订只改本节（防双源，R1-F2）。」story 引用版本号机检齿留根**保持留根不变**（仍等实包摩擦）。「定稿载体」节留根句相应改指「UI 面已落（见消费保护规则节），story 引用版本号机检齿仍留根」。

目标文件：`.claude/references/requirement-change.md`（改）

### T6 · scope-map 写面收紧（随翻译位撤销）

**问题**：uiux-author allow 仍含 `delivery/taskpackages/**`（模式 A 写 uiux-brief.md 的槽）——翻译位撤销后该写槽成为无主授权，违背最小扩权原则。

**证据**：`scope-map.json` uiux-author 条目 `"allow": ["delivery/taskpackages/**", "docs/ui/**"]`（2026-09-29 实读，上一批 T3 扩权后状态）。

**根因**：上一批为模式 A 授的写槽，模式 A 撤销后应同步收紧（扩权与收权同批对称）。

**改法**：allow 列删 `delivery/taskpackages/**`，只留 `docs/ui/**`；deny 列不动。收权不影响模式 B（生成模式只写 docs/ui）。

目标文件：`.claude/scope-map.json`（改）

### T7 · brief-inputs.mjs ①c 剥离 uiux-brief.md 子源（OQ-4 human 追加裁定）

**问题**：模式 A 撤并后 `uiux-brief.md` 制品恒无生产方——①c 的第三路检查成为「永远查空的代码」。

**证据**：`.claude/scripts/brief-inputs.mjs`（2026-09-29 实读，上一批 T5.3 落地版）——①`collectUiuxRefs()` 内 briefPath 检查块（`delivery/taskpackages/<changeId>/uiux-brief.md` 存在性探测）；②builder brief ①c 的 none 消息句含「/ `uiux-brief.md`」字样；③self-check「uiux-brief.md 在位入列」断言 + uxDir temp 夹具块（总断言数 40，2026-09-29 实跑）。

**根因**：同 T3——制品取消后其管线痕迹应同批清零（human 2026-09-29 追加裁定：删、不留，OQ-4）。

**改法**：三处——①`collectUiuxRefs()` 删 briefPath 检查块（返回集合只收 `designRefs[]` / `sharedUiRefs[]`），函数头注释行（:151）同步删「+ uiux-brief.md」段——否则 V8 必红（R3-F1）；②①c 的 none 消息句删「/ `uiux-brief.md`」字样（在位列 / 悬空 advisory / 定位权威句三臂不动）；③self-check 删「uiux-brief.md 在位入列」断言与 uxDir temp 夹具块，总断言数 40 → 39，其余 uiux-refs 断言（在位列 / 专列 builder / 缺席 none / 悬空 advisory / 夹具非空）全保留。

目标文件：`.claude/scripts/brief-inputs.mjs`（改）

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
| 脚本面 | `.claude/scripts/brief-inputs.mjs` | G3 | ①c 剥离 uiux-brief.md 子源（T7，designRefs/sharedUiRefs 主路不动）；消费方 = planner Step 7 前置生成 + builder |
| 上游供给 | requirement-change 档 2 4b | 未触 | 生成模式本体零改动；uiux-author def 收缩后该派发路径不变 |
| 契约面 | `docs/contracts/**` | 未触 | G1 = 0 |

## 4. human 取舍点（OQ）

三件方向性取舍已于 2026-09-29 会话裁毕：

| OQ | 选项 | human 裁定 |
|---|---|---|
| OQ-1 梯子 L2/L3 处置 | (a) 现在立项重排（推荐位）/ (b) 留根等首实跑批摩擦 | **裁 (a)**——梯子现为新旧混合态（L3 仍教包内 OpenDesign），留着重排会误导下一个 UI 包的 planner；摩擦数据留根 retro 补 |
| OQ-2 UI 缺口拦截位 | (a) planner 起 Pod 前置检查 fail-closed（human 提出）/ (b) 实现期升级信号兜底 | **裁 (a)** 为主拦截，(b) 降级为梯子 L3 兜底句——拦截位前移到起包前 |
| OQ-3 模式 A 去留 | (a) 撤并：筛选挂图并入 planner 起包，uiux-author 只留生成模式（推荐位）/ (b) 保留翻译 agent | **裁 (a)**——①c 透传已替代其消费面，实证到场率趋零；角色减一、dispatch 减一 |
| OQ-4 ①c 死代码处置 | (a) 留待顺路清（原推荐位，判官 R1/R2 已核「零改动声明成立」）/ (b) 本批删净：①c 子源检查 + 夹具一并剥离 | **裁 (b)**（2026-09-29 GREEN 后追加）——「永远查空的代码」不留，扩 T7 + manifest 增 brief-inputs.mjs，回判官重裁 |

无遗留 OQ——不涉挂载面扩张、不涉全会话新机制（前置检查是既有 launchPreconditions 留根的落地）、不涉不可逆操作（写槽收紧可逆）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

| # | 时点 | 命令 | 预期 |
|---|---|---|---|
| V1 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260929-gov-review-ui-ladder-remap/PROPOSAL.md` | exit 0；anchorUnion 成员 = manifest 九路径、无注记夹带路径 token（R3-F2 改数） |
| V2 | pre-commit | `node .claude/scripts/route-regression-check.mjs` | exit 0——planner 白名单删 uiux-author 后路由信号不退化（本批关键守卫：白名单形状变化必须过它） |
| V3 | pre-commit | `node .claude/scripts/lint-descriptions.mjs` | exit 0 · flagged 0——uiux-author description 重写后 ≤320 且路由信号在场 |
| V4 | post-commit | `node -e "JSON.parse(require('fs').readFileSync('.claude/scope-map.json','utf8'));console.log('ok')"` | 输出 ok |
| V5 | post-commit | `node .claude/scripts/brief-inputs.mjs --self-check` | **39/39 PASS**——①c 子源剥离后自检绿（总断言数 40 → 39，T7③；designRefs 主路五条 uiux-refs 断言在场） |
| V6 | post-commit | `grep -c "uiux-author" .claude/agents/planner.md` | 输出 0——派发句、白名单 token、Step 6b 末句（T2 改法 4 删除）三项合计清零（负向断言；撤并前基线计数 = 2，2026-09-29 实查） |
| V7 | post-commit | `grep -cE "uiux-author\|uiux-brief" .claude/references/task-intake-and-slicing.md .claude/references/dev-loop.md .claude/agents/acceptance-author.md` | 三文件各 **0**（判官 R1 census 判定旧义只住这三处 + planner def / uiux-author def；撤并收口后清零；撤并前基线计数 = 2/2/1，2026-09-29 实查。「各 ≥0」恒真断言已废——R1-F7） |
| V8 | post-commit | `grep -c "uiux-brief" .claude/scripts/brief-inputs.mjs` | 输出 0——uiux-brief.md 子源痕迹清零（负向断言；撤并前基线 = 6 处，2026-09-29 实查；R3-F3 并表） |

真空绿规约：V1–V3 跑前先确认脚本入口在位；环境红先立基线再归因。V6–V8 为负向断言（撤并收口的机检形态），预期值即断言；基线计数已于起草期实查在案。

## 6. 留根清单

- **摩擦回收（OQ-1 裁定留根）**：首实跑批回收三问——① L3 设计缺口实际发生频率与停包路由成本；② UI-ready 检查的判据摩擦（版本行对齐人工核是否够）；③ 小视觉微调走 L1 + designReview 是否兜得住。交 loop-retro cadence。
- **taskpackage-required-check 机检齿**：UI-ready 检查现为 def 层义务、无脚本兜底——若首实跑批出现「检查跳过」，立项把检查落进 launch 前机检（比照 story 引用版本号机检齿留根句法）。
- **L4 与 /arch-change UIUX 轴集的接线**：梯子 L4 只改指路，governance profile 轴集审的 dispatch 细节未动——出现 L4 实例时再校准。
- **登记不硬治项**：planner 挂图动作（写 designRefs 逐页 PNG）无独立验证——由 ①c 透传的「悬空 ref advisory」间接兜底（挂错路径会报警），不另设门。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 2026-09-29 · architect（Track B 判官 · 新实例，与 260929-gov-review-req-loop-ui-author 批相互独立）

**总裁定：回返（非 GREEN）**——1 条 BLOCKING（R1-F1）+ 4 条 SHOULD（R1-F2–F5）+ 4 条 MINOR（R1-F6–F9）。本批方向（设计上收需求/治理面、fail-closed 拦截位前移到起包前、模式 A 撤并、写槽对称收紧）判官无异议，回返项全部是完备性、引据准确性与验证集内部一致性，不涉方向推翻。

**已核项（复核通过面）**：

1. **逐 T 引据亲读复核通过**：T1——`task-intake-and-slicing.md` 梯子节 L2/L3 行（:326/:327）、:330 baseline brief 句、:332 字段说明句逐字属实；上游先例（requirement-change.md 档 2 流程 4b，:45）属实。T2——`planner.md:61` Step 6b 现文逐字属实（停的判据只含 brief 缺失、不含页图 PNG 缺、无路由去向）。T3——`uiux-author.md` 双模式结构、模式 A 整节、`uiux-brief.md` 制品义务（:14–30/:58）属实；实证口径（53 包 / 3 包带字段 / true 仅 1）与上一批 R1-F7 判官实查一致。T4——`dev-loop.md:55/:68`、`acceptance-author.md:23` 三处引据逐字属实。T5——`requirement-change.md:54` 留根句与 :58「消费保护规则」节机械判据句属实。T6——scope-map uiux-author allow 现值（:125–128）属实。T2 证据段的 fail-closed 先例引据错位单独立 R1-F6（先例本体存在、引据位置错）。
2. **packet-binding 抽查通过**：landing-manifest 8 行 ≡ T1–T6 目标文件锚并集（T1=task-intake-and-slicing；T2+T2b=planner+routing.json；T3=uiux-author；T4=dev-loop+acceptance-author；T5=requirement-change；T6=scope-map），形态合规（裸路径、Status 头部块下方固定位、无注记夹带路径 token）；判官独立实跑 preflight 绿（见已核项 7）。
3. **轨道/profile 裁定：Track B 恰当、G2+G3 自评准确**。G1=0（DEV-GOVERNANCE 未触，uiux-designer roster 行系上一批已裁的 G1 留根面）、契约面=0、无跨系统结构性变更；`route-fixtures/routing.json` + `scope-map.json` 属 G3，但治理类变更默认 Track B（arch-review-protocol「双轨路由」节：governance 类不再强制升级），且 human 立项 ack 的 OQ-3（模式 A 撤并）已覆盖 tools 白名单删 token 与写槽收紧这两个机械后果的授权面——与上一批 scope-map 扩权走 Track B 的先例一致。无应升 Track A 的触面。
4. **T2b 守卫形状兼容性（焊点核）**：`route-regression-check.mjs:71–86` 按 routing.json 自身 `agentRequiredToolsAgents` 列表断言各 agent frontmatter `tools:Agent()` 白名单成员在场——routing.json:91 与 planner frontmatter 两侧同删 `uiux-author` token 后守卫仍绿（基线 54 ok → 删后预期 53 ok，成员 -1）。守卫按设计不检测「从 required 列表移除」本身（它只防丢必留成员），移除侧的负向保护由 V6 承载——形状兼容，草案 §5 V2「exit 0」预期成立。uiux-author 无 routing.json 必留信号条目，T3 description 重写不受信号约束（仅 lint 约束）。
5. **犀利问题裁定**：① 模式 A 撤并后 planner 自产自销（起包期亲自写 designRefs 筛选/挂图、无独立审者）——**判可接受**：筛选挂图本属编排权范畴，挂错路径由 brief-inputs ①c 悬空 ref advisory 机械兜底（§6 已登记「登记不硬治项」）、acceptance-author 消费 designRefs 形成下游校验位、且判级（uiuxLevel）本就是 planner 既有自持判断，非新引入的自评面；② L3 二次入 loop 的档位与留痕、③ L4 命中单包时与 portfolio-planner 的交界——见 R1-F5（未闭环，须补句或显式留根）。
6. **连带面清扫（census 详见下段）**：`change-triage.md` 无梯子消费（grep L2/L3/L4/UIUX/ladder 零命中——分档逻辑不依赖梯子，T1 改义无需同步）；`portfolio-planner.md` 无旧梯子/L4 旧指路（grep 零命中）；`requirement-reviewer.md:24`、`commands/requirement-change.md:2/:18–19`、`commands/arch-change.md:26` 均为生成模式引用，本批落地后仍为真陈述；`integrator.md:44` design-review 证据句不触旧义；`brief-inputs.mjs` ①c「零改动」声明成立（V5 基线 40/40 PASS 独立实跑绿）。
7. **独立实跑留痕（2026-09-29，判官本机）**：
   - V1：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260929-gov-review-ui-ladder-remap/PROPOSAL.md` → `"ok": true, "anchorCount": 8`，anchorUnion 八路径与 manifest 逐一相符、`advisories: []`、`findings: []`。
   - V2 基线：`node .claude/scripts/route-regression-check.mjs` → `route-regression-check: 54 ok, 0 fail`，exit 0。
   - V3 基线：`node .claude/scripts/lint-descriptions.mjs` → `scanned 16, exempt 0, excluded 1 (architect.md), flagged 0`，`✓ clean`，exit 0。
   - V5 基线（附带）：`node .claude/scripts/brief-inputs.mjs --self-check` → `40/40 passed · SELF-CHECK GREEN`——§5 V5 的 40/40 预期与本基线一致（脚本零改动前提成立）。

**退役语面 census（判官 2026-09-29 实跑，词形变体 × 有/无 backtick：模式 A / 模式A / Mode A / 翻译模式 / 翻译位 / 翻译角色 / uiux-brief / creates an interaction brief / OpenDesign visual reference / 派生 uiux-author / interaction brief，扫 `.claude/**`）**：T1–T4 目标文件之外仅命中 `brief-inputs.mjs` ①c 相关行（:151/:164/:469 及 self-check 夹具 :600–602）——属「零改动」非目标面，声明成立、无需处理。目标文件内**超出 T 改法枚举的残留**三组：`task-intake-and-slicing.md:320/:338/:349`（R1-F3）、`planner.md:61` Step 6b 末句（R1-F4）、`uiux-author.md:8/:32` 标题层（R1-F9）。`docs/DEV-GOVERNANCE.md` 的 uiux-designer roster 行为 G1 历史留根（上一批 R1-F3 已裁独立批收口），本批不触、无新假陈述引入。

**Findings**：

**R1-F1 · BLOCKING · acceptance-author 输入机制假陈述（T2 改法 3 / T4③）**。T2 改法 3 写「acceptance-author / builder 的 UI 输入 = uiux block `designRefs[]`（经 brief-inputs ①c 透传）」，T4③ 给 `acceptance-author.md:23` 的替换句同样带「（planner 起包期写入，经 brief-inputs ①c 透传）」——但 ①c 是 **builder 专列**：`brief-inputs.mjs:593` self-check 断言「uiux-refs 专列 builder（acceptance-author brief 不含 ①c）」在册；acceptance-author 的 UI 输入现形 = 直读 TaskPackage uiux block。按替换句落地即在 def 落一句假机制陈述（上一批 R1-F3「尚未接线」假陈述 BLOCKING 同源）。**建议**：T4③ 改为「UI 输入 = TaskPackage uiux block 的 `uiAcceptanceRefs[]` 与 `designRefs[]`（planner 起包期写入；acceptance-author 直读 TaskPackage，builder 侧经 brief-inputs ①c 透传）」；T2 改法 3 同步分角表述。

**R1-F2 · SHOULD · 非需求批 UI-ready 判据误拦风险**。T1 连带句与 T2 判据均要求「UIUX-DESIGN-BRIEF 版本行注明对齐的**本批** requirements 版本号」——非需求批出生的触 UI 包（bug 修复包、重构包）没有「本批 requirements 版本号」可对齐，严格读法会把这类包误判 L3 停包（误拦）。**建议**判据分形并三处（T1 连带句 / T2 Step 6b / T5 消费句）同款：需求批出生的触 UI 包 = 版本行对齐本批版本号；非需求批出生 = 冻结页图在位 + brief 版本行对齐的版本号不晚于当前 frozen requirements 最新版本行（或明示非需求批仅核页图在位）。顺带焊点：该判据将在三处重述，T5 的「以本节机械判据为权威、判据修订只改本节」是防漂移正解，建议 Step 6b 与梯子连带句尽量指针化（「判据权威 = requirement-change.md『消费保护规则』节」），减少复制正文。

**R1-F3 · SHOULD · 梯子节旧义清扫不完整（退役语面硬项）**。T1 改法枚举了表格 L2/L3 行、:330 连带句、:332 字段说明句，但同节还有三处旧义未入枚举：① :320 节引句「Depth drives whether a `uiux-author` step runs and what design-review evidence integrator owes at closeout」——「uiux-author step runs」在新语义下为假陈述；② :338 JSON 示例 `"designRefs": ["docs/ui/UIUX-DESIGN-BRIEF.md#pageId-x"]`——与新「designRefs = 逐页 PNG」口径不一致（示例应改 PNG 路径形态）；③ :349「`designRefs[]` primarily point to stable `UIUX-DESIGN-BRIEF.md` sections/IDs; OpenDesign artifact refs are supporting visual context and cannot replace the frozen textual contract」——与新 L2 语义（designRefs 逐页 PNG、PNG = 布局坐标定位权威，上一批定例）正面矛盾，必须改写或明示保留理由。修订稿按模板「退役语面复扫」硬项把全形态枚举（词形变体 × 有/无 backtick）写进 T1 改法并逐处落处置。

**R1-F4 · SHOULD · V6 与 Step 6b 末句互斥（验证集内部一致性）**。现行 `planner.md:61` Step 6b 末句「系统级 UI 生成已接线 = requirement-change loop 档 2 的 `uiux-author` 生成模式（主线程派，见 requirement-change.md 档 2 流程 4b），不由你起草」含 uiux-author token；T2 重写规格（判级段 + UI-ready 检查段 + dispatch 输入构成）未明示该句处置。落地若保留该句 → V6（`grep -c "uiux-author" .claude/agents/planner.md` = 0）必红；若删 → 须说明语义不丢（L3 路由句已含「路由 requirement-change」，「不由你起草」边界可并入判级段）。二选一须落进修订稿，V6 预期才成立。

**R1-F5 · SHOULD · L3 停包路由的流程闭环三缺（含 L4 交界）**。T1 L3 行与 T2 改法定义了「fail-closed 停在起包前、路由 requirement-change」，但三件未定义：① **留痕口径**——停包动作落哪个既有闭集（pod-events `agent-escalation`/`friction` 或 planner 交付报告 Red Flag），修订稿一句钉死（此件属本批落地面、不宜留根）；② **需求 loop 承接形态**——L3 缺口批进 requirement-change 的身份：建议明示「视为原 story 的档 2 触 UI 修订批（human 裁意图入口照常、§9 版本行照常 append）」，同时回答「human 已批过原需求批后的二次入 loop 档位判定」；③ **续包判据**——「frozen 后包再续」的再核动作（planner 重跑 UI-ready 前置检查、对齐新版本行）。另含 L4 交界一句：单包命中 L4（系统导航/shell）时，planner 停包路由 requirement-change 补蓝图与既有 Step 3「需多包/新 workstream → portfolio-planner」的边界须一句消歧（蓝图先行、拆包归 portfolio-planner，两动作可先后发生不互斥）。②③ 若嫌重可整体留根，但须显式写进 §6。

**R1-F6 · MINOR · T2 证据段先例引据错位**。T2 引「同文件 Step 9『brief 缺失时降级自扫，并在交付报告标 `manifest-missing`（不静默）』」——`planner.md` 全文无 manifest-missing/降级自扫（判官 grep 实证零命中）；该句式实在 `acceptance-author.md:24`。§0 硬项段的「planner Step 5 既有……句式」同错。「Step 4.5 生成模式 🟡 未消解不出图句式」在 `commands/requirement-change.md:18`，属实。修订稿两处引据改指 `acceptance-author.md:24` 与 `commands/requirement-change.md:18`。

**R1-F7 · MINOR · V7「各 ≥0」恒真**。负向断言只剩「grep -c "派生 uiux-author" dev-loop.md = 0」一条有牙；「各 ≥0」对任何输出恒真（vacuous）。建议改为逐文件明确计数预期（三文件 uiux-author|uiux-brief 计数写死，含 0 值）。

**R1-F8 · MINOR · §0 非目标「①c 恒走 none 臂」表述失准**。①c 收三源（`designRefs[]` / `sharedUiRefs[]` / `uiux-brief.md`，`brief-inputs.mjs:151–165`）；模式 A 撤并后只有 uiux-brief.md 子源恒缺席，designRefs 主路由 planner 起包写入（T2 改法 2）常态在场。建议改口径为「uiux-brief.md 子源恒缺席、①c 主路（designRefs 透传）不变」，防「①c 已死」误读。

**R1-F9 · MINOR · T3 def 重写的标题层未枚举**。`uiux-author.md:8` H1「UIUX Author（UIUX 翻译 / 生成双模式）」、:32「## 模式 B · 生成」节标题、:12 Role 段「双模式：」引导词在「重写为生成模式单模式」语义下均需去模式化改写；T3 改法五点未显式列（「When/Inputs/Process 保留模式 B 现文」保留的是内容，标题与引导词仍须改）。并入 R1-F3 的退役语面复扫枚举即可。

**R1 结论：回返。** 修订清单：① T1 扩清扫三处旧义 + 判据分形（R1-F3 / R1-F2）；② T2 改法 3 分角表述 + Step 6b 末句处置明示（R1-F1 / R1-F4）；③ T4③ 替换句改写（R1-F1）；④ T1 L3 行或 T5 补路由闭环（R1-F5，至少留痕口径必落）；⑤ §0 引据与口径修正（R1-F6 / R1-F8）；⑥ V6/V7 断言修正（R1-F4 / R1-F7）；⑦ T3 补标题层枚举（R1-F9）。**manifest 无需扩**——全部修订落在既有八目标文件内。修订后重跑 V1 preflight + V2/V3 基线复跑，送 R2。

### R2 · 2026-09-29 · architect（Track B 判官 · v0.2 复审）

**总裁定：GREEN，可标 FROZEN**——R1 全部 9 条 finding 核销通过（逐项见下），packet-binding 重验通过，新增 1 条 MINOR 文字层残留 + 1 条落地措辞注意（R2-F1/R2-F2），均为非阻塞、不触 manifest 与改法语义，主线程标 FROZEN 前顺手修正即可，**无需 R3**；landing-verifier 按 R2-F1 核对 FROZEN 文本已含微修。

**R1 逐项核销**：

| Finding | 修订落点（v0.2 实文） | 核销 |
|---|---|---|
| R1-F1（acceptance-author 输入假陈述） | T2 改法 3 分角表述（「acceptance-author 直读 TaskPackage uiux block；builder 侧经 brief-inputs ①c 透传（①c 为 builder 专列——self-check 断言在册）」）+ T4③ 替换句同款分角——与判官建议句式一致 | ✓ |
| R1-F2（非需求批判据误拦） | 三处同款分形：T1 连带句指针化（权威 = requirement-change.md「消费保护规则」节 launchPreconditions 句，本节只指针）+ T2 改法 2 分形（需求批 = 对齐本批版本号 / 非需求批 = 页图在位且对齐版本不晚于当前 frozen 最新版本行）+ T5 消费句落分形全句（权威承载位） | ✓ |
| R1-F3（梯子节旧义清扫） | T1 新增「旧义清扫」段：:320 节引句改「pre-launch UI-ready check」、:338 示例改 PNG 路径形态（`docs/ui/V5/canvas-p1.png`——判官 `ls` 实核该文件在场）、:349 矛盾句改写（designRefs = frozen per-page PNGs 定位权威、brief 文本仅口径补充）+ 复扫模式完备性枚举（词形 × backtick，引判官 census，残留清单覆盖 T2 末句与 T3 标题层） | ✓ |
| R1-F4（V6 与末句互斥） | T2 新增改法 4：末句删除明示、「不由你起草」边界并入判级段 L3 句、生成模式指称由 requirement-change.md 档 2 4b 与 uiux-author def 承载、planner def token 清零——V6 据此成立 | ✓ |
| R1-F5（L3 路由闭环） | T1 新增「L3 停包路由闭环」四件全落不留根：① pod-events `agent-escalation` 留痕（既有事件闭集、不新开通道，category 落 `other` 即可承载）；② 承接形态 = 原 story 档 2 触 UI 修订批（二次入 loop 档位判定一并回答：设计缺口 = 结构性牵连天然档 2）；③ 续包判据 = frozen 后重跑 UI-ready 对齐新版本行；④ L4 交界 = 蓝图先行与拆包归 portfolio-planner 先后两动作不互斥——T2 改法 2 同步引用①② | ✓ |
| R1-F6（先例引据错位） | §0 硬项段与 T2 证据段均改指 `acceptance-author.md:24`（注明经判官 grep 核正）与 `commands/requirement-change.md` 4.5 步 | ✓ |
| R1-F7（V7 恒真断言） | V7 改三文件各 **0** 明确计数 + 撤并前基线 2/2/1 实查值——判官独立复算相符（task-intake :320/:326 两行、dev-loop :55/:68 两行、acceptance-author :23 一行；grep -c 按行计） | ✓ |
| R1-F8（①c 口径失准） | §0 非目标改「仅 uiux-brief.md 子源恒缺席、designRefs 主路由 planner 起包写入、常态在场」——但 §3/:150 与 §5 V5 括注两处同源旧口径未同步（R2-F1） | ✓（附残留） |
| R1-F9（T3 标题层） | T3 新增⑥：H1 改「UIUX Author（需求 loop UI 生成）」、「## 模式 B · 生成」节标题取消平铺上移、「双模式：」引导词删除 | ✓ |

**R2 独立复核**：① packet-binding 重验——manifest 8 行不变 ≡ T1–T6 锚并集（修订全部落在既有八目标文件内，manifest 未扩，与 R1 结论「无需扩」一致）；判官独立实跑 V1 preflight 绿（`"ok": true, "anchorCount": 8`、`advisories: []`、`findings: []`，2026-09-29）。② 基线独立复跑（2026-09-29，判官本机）：V2 `route-regression-check: 54 ok, 0 fail` exit 0；V3 `lint-descriptions: scanned 16, flagged 0, ✓ clean` exit 0——与主线程报数一致。③ V6 基线独立复算：`grep -c "uiux-author" .claude/agents/planner.md` = 2（frontmatter tools 行 + Step 6b 行），与 v0.2 声明的「撤并前基线计数 = 2」相符；V7 基线 2/2/1 相符（见核销表 R1-F7 行）。④ T1 示例 PNG `docs/ui/V5/canvas-p1.png` 判官 `ls` 实核在场。⑤ 轨道/profile 维持 R1 裁定：Track B 恰当、G2+G3、G1=0、契约面=0；R1 census 的目标文件外命中（brief-inputs.mjs ①c 相关行）维持「零改动非目标面」处置，无新假陈述引入。⑥ R1 已核项（T2b 守卫形状兼容、planner 自产自销裁定、triage-change/portfolio-planner 无梯子消费）在本轮无反证，维持。

**R2 Findings**：

- **R2-F1 · MINOR · §3/:150 与 §5 V5 括注两处残留旧口径**：R1-F8 只修了 §0 非目标句，§3 sweep 表脚本面行仍写「uiux-brief.md 制品取消后恒走 none 臂」、§5 V5 括注仍写「制品取消后该臂即常态路」——两处与修正后口径（仅 uiux-brief.md 子源恒缺席、designRefs 主路〔planner 起包写入〕常态在场）不一致，且会助长「①c 已死」误读。主线程标 FROZEN 前两处顺手同步为 §0 同款口径；纯文字层，不触 manifest、锚清单、改法语义与任何门。
- **R2-F2 · 建议注意（非 finding）· T2 改法 2 落地措辞**：修订文「此处指针不复述」之后仍给出分形全文——作为改法规格无碍（告诉落地者写什么），但落地 Step 6b 时建议取「权威指针句 + 两分支各半句摘要」形态：纯指针会逼 planner 判级时跨文件跳读 requirement-change.md，纯复述又增三处漂移面；T5 权威句已含分形全句，摘要形态两者兼顾。随 landing-summary 留痕。

**R2 结论：GREEN，可标 FROZEN。** 附带条件仅一条程序性要求：主线程标 FROZEN 前按 R2-F1 完成两处文字层微修（§3/:150、§5 V5 括注，不触 manifest、锚清单、改法语义），修后直接进 FROZEN、无需 R3；R2-F2 作为 T2 落地时的措辞注意随 landing-summary 留痕。R1 已核项与裁定在本轮无反证，维持。

### R3 · 2026-09-29 · architect（Track B 判官 · v0.3 重裁——OQ-4 范围追加）

**总裁定：GREEN（有条件），可标 FROZEN**——OQ-4 范围追加（T7 + manifest 第 9 行）裁定恰当、T7 改法主体完备、packet-binding 扩后重验通过；R2-F1 核销确认；新增 1 条 SHOULD + 2 条 MINOR（R3-F1–F3），全部为文字层枚举/数字/排版修正、修法唯一无设计分叉，主线程标 FROZEN 前完成即可，**无需 R4**；landing-verifier 按 R3-F1 核对 FROZEN 文本已含注释行清扫。

**OQ-4 范围追加裁定：恰当**。① 轨道不变——brief-inputs.mjs 属 G3 脚本面，human 追加裁定（OQ-4，§4 在案）即授权触点，与 OQ-3 撤并同批对称（撤角色、撤制品、撤管线痕迹三件一套），Track B 继续成立；② review profile 自评 G2+G3 仍准确（脚本面 §3 已改 G3 实触）；③ 「删净不留」优于「留待顺路清」——判官 R1/R2 核「零改动声明成立」的前提是死代码无害，human 裁定消除该无害假设的存续期，方向判官认可。

**T7 改法完备性核**（判官亲读脚本实况）：

1. **三处引据属实**：① `collectUiuxRefs()` briefPath 检查块（:164–165）；② ①c none 消息句「/ `uiux-brief.md`」字样（:469）；③ self-check「uiux-brief.md 在位入列」断言 + uxDir temp 夹具块（:598–:604，含 mkdirSync/rmSync），块内恰 1 条 ok()——总断言数 40 → 39 预期成立。
2. **none 臂语义自洽性：通过**——剥离子源后 none 触发条件回归「uiux block 不在场或无在盘设计 refs（designRefs[]/sharedUiRefs[]）」，与 ①c 主路语义一致；在位列 / 悬空 advisory / 定位权威句三臂不动，主路行为零变化。
3. **真空绿规约：仍满足**——被删断言的 subjects（uiux-brief.md 在位列）随子源消失而不可再验，非 vacuous green；剩余五条 uiux-refs 断言（在位列 :590 / 专列 builder :593 / 缺席 none :594 / 悬空 advisory :596 / 夹具非空 :603——:603 用 projectDir、不依赖 uxDir）全保留、覆盖主路两臂；§5 V5 括注「designRefs 主路五条 uiux-refs 断言在场」与实况相符。
4. **V8 基线独立复核**：`grep -c "uiux-brief" .claude/scripts/brief-inputs.mjs` = 6（:151/:164/:469/:600/:601/:602），与 v0.3 声明相符——**但 T7 三处改法只覆盖其中 5 行**：:151 函数头注释（「uiux block 的设计 refs + uiux-brief.md」）未入枚举，若随检查块保留则 V8（计数 = 0）不成立——R3-F1。

**packet-binding 扩后重验**：manifest 9 行 ≡ T1–T7 目标文件锚并集（+ brief-inputs.mjs）；判官独立实跑 V1 preflight 绿（`"ok": true, "anchorCount": 9`、`advisories: []`、`findings: []`，2026-09-29）；packet-id 行已同步 T1–T7；§0 非目标改注记（OQ-4 改道记录）形态诚实。

**R2-F1 核销确认**：§3 脚本面行（:164）已改 G3 实触 + 新口径（「designRefs/sharedUiRefs 主路不动」），§5 V5 括注已重写（39/39 + 主路五断言）——两处旧口径「恒走 none 臂 / 该臂即常态路」均被 T7 改写吸收，无残留。R2-F2（Step 6b 落地措辞注意）维持随 landing-summary 留痕。

**基线独立复跑（2026-09-29，判官本机）**：V2 `route-regression-check: 54 ok, 0 fail` exit 0；V3 `lint-descriptions: scanned 16, flagged 0, ✓ clean` exit 0——与主线程报数一致。R1/R2 已核项与裁定（轨道、守卫形状、census、planner 自产自销裁定等）在本轮无反证，维持。

**Findings**：

- **R3-F1 · SHOULD · T7① 清扫范围漏 :151 函数头注释**。`brief-inputs.mjs:151` 注释行「// uiux 设计 refs 透传（…T5.3）：uiux block 的设计 refs + uiux-brief.md」含 uiux-brief token，T7 三处改法（:164 检查块 / :469 none 句 / :600–602 夹具断言）未枚举该行——落地若保留注释，V8（`grep -c "uiux-brief" .claude/scripts/brief-inputs.mjs` = 0）必红，与 R1-F4 同类（改法与负向断言互斥）。**修法唯一**：T7① 补一句「:151 函数头注释同步改写为『uiux block 的设计 refs』（删『+ uiux-brief.md』段）」。
- **R3-F2 · MINOR · V1 预期数字未随 manifest 扩同步**。§5 V1 预期仍写「anchorUnion 成员 = manifest 八路径」，manifest 已 9 行（+ brief-inputs.mjs）——改「九路径」（前一批 R1-F8②「四→五路径」同款文面漂移）。
- **R3-F3 · MINOR · V8 行脱离主验证表**。V8 行前有空行隔断（:192），成无表头孤行——并入 §5 主表末行（删隔断空行）即可；纯排版，不影响断言语义与任何门。

**R3 结论：GREEN（有条件），可标 FROZEN。** 附带条件：主线程标 FROZEN 前完成 R3-F1（T7① 补 :151 注释行清扫句）、R3-F2（八→九路径）、R3-F3（V8 并表）三处文字层修正——均不触 manifest、锚清单、改法语义与任何门，修后直接进 FROZEN、无需 R4；landing-verifier 核对 FROZEN 文本已含三处微修（重点 R3-F1：T7① 含注释行清扫句）。R1/R2 全部裁定与已核项维持。
