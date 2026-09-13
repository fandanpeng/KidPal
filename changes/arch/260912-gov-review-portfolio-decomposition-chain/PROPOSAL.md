# PROPOSAL · 260912-gov-review-portfolio-decomposition-chain

> **状态：DRAFT v0.7 · 2026-09-13 · 讨论中**（260912 拆包→执行链 retro 对话起草，human 未立项 ack、architect 未评审——按治理路由，立项 ack 前本件仅供讨论，不驱动任何落地）。
> **v0.1→v0.2 修订**（human 五点反馈，260912）：① workstream 效果目标升级为业务目的清单硬项；② 方案空间裁定增 spec 关系标注（两源以 spec 为权威）；③ 锚 2 核对者改 planner（agent）、非 human；④ merge 后 portfolio-planner 例行唤起改为状态机械渲染 + 事件驱动唤起；⑤ 移除 deputy 预裁参考（未定型实验，human 指示不作设计依据）。
> **v0.2→v0.3 修订**（260912 human 确认）：⑥ 「下一包 launch 唤起」收窄撤销——常规 ready 由 reconcile 机械计算 + human 照常点火（现行流程不变），**绿路径上 portfolio-planner 沉默**；其再出场条件收窄为「调度异常时由 human 唤起重裁」（锚 3 deviation 影响后续包 / 依赖与实际不符 / WIP·mergeOrder 需重排）。
> **v0.3→v0.4 修订**（260912 human 确认）：⑦ workstream PROPOSAL 内容硬项重钉为「**通用三问 + 分型**」——① 要达成什么（业务目的清单）② 现在在哪（随 workType 三子形，「问题与根因」仅整改型合法）③ 怎么走（方案空间 + 裁定）；frontmatter 增 `workType`（可混合、按业务目的逐条标）。替换 v0.2 隐含的整改型通用骨架（260911 系整改型形态，不具通用性）。
> **v0.4→v0.5 修订**（260912 human 确认）：⑧ `需修订` 类裁定的执行语义钉死为**受影响包的硬 launch 依赖**——spec 修订未冻结（review + human 冻结落地），承载该裁定的包不得 launch；依赖粒度 = 包（经 decision-coverage 映射登记 launchPreconditions，非整 workstream 陪等）；spec 修订在 PROPOSAL 冻结后尽早并行发起（在受影响包关键路径上）。堵死「包先跑、spec 事后补」。
> **v0.5→v0.6 修订**（260912 human 指示 + 纠正）：⑨ workstream PROPOSAL **模板化**——新增 `docs/templates/WORKSTREAM-PROPOSAL.template.md`（三问 + 分型成模板）。**起草方 = 主线程**（输入常为 human 与主线程讨论的产物，起草权不转移至 portfolio-planner——260912 human 纠正，避免下游写上游输入的倒置）；portfolio-planner **intake 增模板符合性核验**（缺业务目的清单 / workType / spec 关系标注 → fail-closed 打回，不猜不代写）。/triage-change ③档起草 PROPOSAL 时按本模板。
> **v0.6→v0.7 修订**（260913 human 指示「requirements 重 review 不影响定拆包规则」+ write-story a118f75 定稿对齐）：⑩ 拆包转写规则落地 = **锚点引用**（story 引用 = `S-Fx.y · 标题` + 可选 AC 号集，不内嵌原文——write-story 已废内嵌口径）；⑪ AC 级回引复合句法全仓首钉 `S-Fx.y-AC<n>`，task-intake sourceRefs 老仓例句（`S1-A1` 系）换新；⑫ plan.md 骨架首项随 workType 分流（原硬锚「问题与根因」在 T1 分型下仅整改型合法）；⑬ roadmap 效果段增回指律（story ID + AC 号集、只投影不改写），锚 2 核验含 AC 回引对账；⑭ 第一问 intake 增两道机查：已砍编号不引用、🔴 story 须先立项处置；⑮ `需修订` 类 spec 修订落点随语义面二分（requirements / architecture），launchPreconditions 按落点核验。
> **承载 ARQ**：T2 承接并收紧 ARQ-260911-split-package-implementation-prose-gap（落地后结清该条）；T3 checklist 消化 ARQ-260911-cross-package-behavior-owner-gap 的拆包期判据（该 ARQ 派单侧义务保留）；与 ARQ-260911-tooth-strength-below-contract / ARQ-260911-verdict-uncovered-surface 相邻不重叠。
> **review 轨**：Track B 常驻轻路径（governance profile）。
> **riskTier**: low-medium（新增制品义务 + 人读流程门，不改任何既有机械闸判定语义、不触契约面）。
> **review profile**: G2（agent def 正文 + references + 模板）+ G3（新增 render-roadmap.mjs，advisory 生成器非门）。
> 配套流程图：`decomposition-chain.html`（本目录内，随批入库——260912 human 指示）。

```landing-manifest
docs/templates/WORKSTREAM-PROPOSAL.template.md
docs/templates/WORKSTREAM-ROADMAP.template.md
.claude/scripts/render-roadmap.mjs
.claude/agents/portfolio-planner.md
.claude/agents/planner.md
.claude/references/task-intake-and-slicing.md
.claude/references/pod-coordination.md
.claude/skills/triage-change/SKILL.md
```

---

## 0. 想实现的效果与差距

目标效果：portfolio-planner 拆出的每个 TaskPackage，**方向在落地前受审、包级设计耐久在案、Pod 执行全程有意图锚可依、每包完成有可见的业务效果对账**——「拆错方向、写到最后集成夜才爆」不再可能无声发生。

现状差距（对应 §1 四根因）：

1. 拆包产物（taskpackage × N + decision-coverage + mergeOrder + plan.md）无任何独立 review，直接 commit-before-launch；
2. 包级实施方案（plan.md）本批才首用且带豁免判据，MVP 全量 19/19 缺位；
3. 每包业务效果目标（「完成后人能看到什么」）无耐久载体，验收意图由 acceptance-author 在 Pod 内现造，本批造错过一次（SU-RULING-01）；
4. workstream 无活体 roadmap，拆包者离场后无人持有列车视角，整体效果只在末位包收口轮重新有人看。

**非目标**（防 scope creep）：模型档位策略——挂起（260912 human 裁后移，待按 pod retro 的返工轮次/token 数据按档位统计后另行定夺；统计前注意 ARQ-260909-dispatch-stamp-telemetry-gaps 的 token 口径缺口）；门 1 的机械化为 launch 前置闸——现阶段为人读 + checklist，不新建 PreToolUse 闸；260911 批次回溯补件——自下批 workstream 起适用。

## 1. 问题与根因（地面真值）

1. **切法无票（R1）**：workstream 方向经 grilling 由 human 逐条裁定，但切包方案随 PROPOSAL §4「拆包预判」一并冻结、未经任何裁断——260911 grilling 四 OQ 全是方案空间选择题，无一问「这么切包你认不认」。实况：MVP 19 包按页面/技术层切（home_p1 / schedule_p2 / story_p3 / session-logic / host-test-infra / shim-skeleton，目录名实录），260911 批按问题切（问题 #4 → PKG-2，致单包承载五条收口标准中的四条，plan.md 内再切阶段 0–5 自救）——两代切法均非按行为垂直切片，portfolio-planner.md 垂直切片原则在场但无任何环节检查。MVP 跨包掉缝（KEY 端到端无人接线、殿后 9b 转手漏列、故事归档三段各自绿拼起来不可达）系此根因的结构性后果（RETRO-VERIFICATION-GAP-260911 §二）。
2. **拆包无审（R2）**：review 发生在拆包**前**（/triage-change 对 PROPOSAL 的分流 review，260911 批 F1–F9），拆包产物本身零门——产完即 commit-before-launch。本批两次核心语义偏差（SU-RULING-01 验收优先级钉反、SU-RULING-02 只交付机制未做物理 reparent）均靠 planner 事后逐字读冻结验收 + 屏幕模型代码才抓出（pod-retro §这一轮最深的体感）。
3. **包级设计缺位（R3）**：2026-09-12 全量机械清点（命令：python 遍历 `.claude/taskpackages/` 25 目录，核 plan.md 文件与 taskpackage.json planRef 字段）：**21 无 plan.md**；MVP 期 19 包（260829 批 12 + 260831/260901/260903 批 6 + 260907-g-relay-fix，sourceType = planning-workstream / regression）plan.md 与 planRef **双双缺位，无一例外**；有 plan.md 的 4 包全部产自 2026-09-11（②单包路径 2 + ③拆包 ARQ 首用 2）。本批 PKG-1（key-route-a1）被「装配型豁免」判掉，而它恰是跨包行为 owner 承载包——豁免判断本身是拆包 agent 的判断，判错即裸奔。
4. **执行无锚（R4）**：无 roadmap、无包级 demo 判据、拆包者 episodic 离场后无常驻列车视角；执行中偏差无预挂裁断通道。**且效果锚从源头无源可引**：workstream 效果目标本身退化为一句话——MVP v0.4 §0「目标效果一句话」实文 =「把 spec §4 全部 story 拆成一组排好序、带依赖、可并行的 TaskPackage」，即**拆包动作的自述而非业务效果**（孩子能做什么、看到什么，一处未落纸）；260911 批 §0 虽好一档（「听得见的答案、看得见的落点、按得动的按键」），但仍是一句概括、未枚举成可逐条 demo 的业务目的清单。

## 2. 改法（T1…T4）

> 总形：一条「意图钉死 → 方向受审 → 执行对锚 → 完成对账」的链。门 1 治 R1+R2，T2 治 R3，锚 2/锚 3 治 R4。

### T1 · 制品双轨分离：PROPOSAL 回归裁决记录，新增 workstream roadmap 活文档

PROPOSAL（.planning 通道）只管「三问 + 裁定」（要达成什么 / 现在在哪 / 怎么走），维持冻结；「列车怎么跑、走到哪了」独立为活文档 roadmap.md，落 `.planning/<workstream>/roadmap.md`。

**workstream PROPOSAL 内容硬项 = 通用三问 + 分型**（**落新模板 `docs/templates/WORKSTREAM-PROPOSAL.template.md`**；`task-intake-and-slicing.md` 注记引用；frontmatter 增 `workType: new-capability / remediation / refactor`，可混合——按业务目的逐条标新建/整改，混合是常态先例：260911 = 三个整改目的 + A2 新建）：

- **起草方与 intake 核验（v0.6 human 指示 + 纠正）**：**起草方 = 主线程**（/triage-change ③档路径）——输入常为 human 与主线程讨论的产物（260912 本批 retro 即实例形态），主线程按模板起草 → human grilling（AskUserQuestion 裁 OQ）→ architect 分流 review → FROZEN，起草权不转移至 portfolio-planner（下游写上游输入是倒置）。**portfolio-planner intake 增模板符合性核验**：业务目的清单 / workType / spec 关系标注缺失或不合规 → fail-closed 打回主线程补齐，不猜、不代写。

- **第一问 · 要达成什么**——业务目的清单：枚举式、每条含可观察判据（人能看到/听到/按到什么），禁一句概括了事（R4 MVP 实证：目标句写成拆包动作自述，业务效果无处落纸）。清单是 roadmap 每包效果段的**唯一上游源**。每条目的来源随分型：整改型引缺陷证据，新建型引 spec story（只加判据、不改语义）。story 引用形态一律**锚点引用**（`S-Fx.y · 标题` + 可选 AC 号集，合法形态以 write-story 为准）——story 文本随 spec review 演进（requirements v1.10 起仍在重 review），锚点引用不受其扰。新建型引 story 另有两道 intake 机查（fail-closed）：① 引用编号不得命中 requirements.md 文件头「已砍清单」（grep 即判）；② 可行性 = 🔴 的 story 须先有立项处置记录（write-story 🔴 挂账纪律：Pod 无权自裁），否则该目的不得入清单。
- **第二问 · 现在在哪**——形态随 workType 分三子形：**新建型** = 现状盘点（已就绪基础、可复用资产、前置批次状态）+ 差距 = 目标能力 − 现状（「还没建」，非「坏了」）；**整改型** = 问题行为 + 证据锚（file:line / 实测）+ 根因——「问题与根因」标题仅此形态合法；**重构型** = 结构性痛点证据 + 为什么现在动。现状盘点即 RTE 拆包的原料输入（供给 R1/R4 的缺口面）。
- **第三问 · 怎么走**——方案空间 + OQ 裁定 + 非目标 + 逃生门申报。
- **方案空间裁定标注 spec 关系**：PROPOSAL §方案空间每条裁定必须带标注，写明它与 spec 的关系是三种里的哪一种——`within-spec`（spec 已定此语义，裁定只选实现路径，不动语义——标注时引 spec 条款号）/ `需修订`（裁定要改 spec 已钉死的语义 → **必须先走 spec 修订流程**：Track B `/arch-change` → 对抗 review → human 冻结；spec 改完这条裁定才生效，**不得在 PROPOSAL 内自冻成第二事实源**。**执行语义 = 受影响包的硬 launch 依赖**：spec 修订未冻结，承载该裁定的包不得 launch——依赖登记到该包 `launchPreconditions`（粒度 = 包，经 decision-coverage 映射，非整 workstream 陪等；先例形态：260911 PKG-4 前置挂批次① overlay 修复）；spec 修订在 PROPOSAL 冻结后尽早并行发起，与拆包 / 门 1 / 其它包执行同时进行。堵死「包先跑、spec 事后补」）/ `spec-silent`（spec 未载此事——留根登记，由门 1 或收口定是否补进 spec）。两源并存时的权威序恒为：**spec = 行为语义权威，PROPOSAL = 路径选择记录**——260911 批 §7 申报机制（OQ4=A 判 spec 零修订、OQ3=c 判无语义变更）是本标注的先例形态，本项将其升格为逐条硬项。
- **workType 往下游传**：整改型必有红基线（有既有行为可红）、门 1 侧重「根因 ↔ 改法」对得上；新建型无红基线、门 1 侧重切片与覆盖；重构型门 1 侧重结构性质可验收。

**roadmap 模板**：新增 `docs/templates/WORKSTREAM-ROADMAP.template.md`，固定节：① workstream 效果目标与现状差距（引用投影自 PROPOSAL §0 的业务目的清单，不复制）；② 列车图（依赖 + 批次，由 render-roadmap.mjs 从 taskpackage.json 的 dependsOn/mergeOrder 机械渲染，人工禁改该节）；③ 每包条目——changeId、mergeOrder、一段「完成后人能看到什么」的业务效果（**人只写机器写不出的这一段**，从 workstream 业务目的清单分解而来）+ **回指本包承载的 story ID + AC 号集**（新建型；只投影判据、不改写不复述 story 语义——与第一问「只加判据、不改语义」同律，防效果段长成 story 的第二释义层）、demo 证据形态（模拟器截图 / host runner 输出 / 串口日志 / 真机轮；真机轮配额默认仅核心包）、达成状态（pending / achieved / deviation+裁定锚，由脚本从 board/merge 状态机械渲染）；④ 非目标与批次归属。新增 `.claude/scripts/render-roadmap.mjs`（读 workstream 下 taskpackage JSON 集 → 产出 mermaid 依赖图块 + 达成状态渲染，含 `--self-check`）。`task-intake-and-slicing.md` 补注记：PROPOSAL §拆包预判降级为意图提示，拆包权威 = 四件套经门 1 冻结。

**目标文件**：`docs/templates/WORKSTREAM-PROPOSAL.template.md`（新增）、`docs/templates/WORKSTREAM-ROADMAP.template.md`（新增）、`.claude/scripts/render-roadmap.mjs`（新增）、`.claude/references/task-intake-and-slicing.md`（改）、`.claude/skills/triage-change/SKILL.md`（改，③档起草按模板）、`.claude/agents/portfolio-planner.md`（改，Step 1 intake 增模板符合性核验）

### T2 · plan.md 全量义务（收紧 ARQ-260911-split-package-implementation-prose-gap）

每个 TaskPackage 拆包时必备 `.claude/taskpackages/<changeId>/plan.md`，taskpackage.json `planRef` 指向；「装配型豁免」取消（豁免范围按 §4 OQ3 裁定）。plan.md 骨架：**背景摘录（锚点随 workType 分流——整改型锚 PROPOSAL「问题与根因」节、新建型锚「现状与差距」、重构型锚「结构性痛点」）**、改法取舍、阶段划分、铁律约束、demo 证据形态复述（与 roadmap 一致）。

**story 转写规则（write-story 边界指给本链的落点，a118f75 新口径）**：story 在 plan.md / taskpackage 中**只以锚点引用出现**——`S-Fx.y · 标题` + 可选 AC 号集，不内嵌原文、不改写、不复述语义；story 语义以 requirements.md 现文为唯一权威，taskpackage `story` 字段承锚点引用（技术描述归 plan.md，治 260829 期 story 字段写成技术方案的真实翻车）。**AC 级回引复合句法全仓首钉**：`S-Fx.y-AC<n>`（如 `S-F1.4-AC3`），验收齿 / sourceRefs 的 AC 粒度回引按此形态填。

`portfolio-planner.md` Step 4 补全量条款，`task-intake-and-slicing.md` planRef 节同步 + 新增 story 锚点引用转写小节（上形规则 + sourceRefs 老仓例句 `S1-A1`、`docs/systems/S1-leads/spec.md:S1-A1` 换为本仓新形态）。

**目标文件**：`.claude/agents/portfolio-planner.md`（改）、`.claude/references/task-intake-and-slicing.md`（改）

### T3 · 拆包 review 门（门 1）

拆包四件套齐备（roadmap.md + taskpackage.json × N + plan.md × N + decision-coverage.json）后、commit-before-launch 前，插一道 architect 轻量 review：固定 checklist 五项——① 切片逻辑（按行为切还是按页面/问题切，图上见，偏离垂直切片须有显式理由）；② 依赖序与 WIP 合理性；③ decision-coverage 完备（含跨包行为 owner 显式登记）；④ 每包意图陈述与 PROPOSAL 冻结裁定一致；⑤ demo 证据形态可行（真机轮配额仅核心包）。checklist 侧重随 workType 分流（T1 workType 下传：整改型核根因↔改法 + 红基线在位；新建型核切片与覆盖）。`需修订` 类裁定的 spec 修订 = 受影响包 launch 前置依赖（见 T1），由 check-launch 核验「spec 修订已冻结」才放行 launch——落点随语义面二分：story 语义修订看 requirements.md、支撑语义修订看 architecture.md（spec 260913 已拆两文件，核验按落点对号）。GREEN 后 human 过目切法（立项 ack 的组成部分：**方向 + 切法一次前端对齐**——切法自此有票），方可 commit-before-launch。checklist 落 `task-intake-and-slicing.md` 新节，`portfolio-planner.md` Step 6 前插「门 1」步骤引用。存疑向严：触契约面 / 跨系统 / 判官不绿 → 升 Track A（`/arch-change`）。

**目标文件**：`.claude/agents/portfolio-planner.md`（改）、`.claude/references/task-intake-and-slicing.md`（改）

### T4 · Pod 内两锚：验收方向核（锚 2）+ demo 对账（锚 3）

- **锚 2 · 验收方向核**：acceptance 冻结后、builder 动工前，**planner（agent）**对照 roadmap.md 本包效果段核一遍验收方向一致，含 **AC 回引对账**——验收齿回引的 AC 号集与效果段回指的 AC 号集对得上（多引、少引都是方向偏差信号）——核对者是 planner 而非 human（验收语义方向不是 human 能逐条核清的，260912 human 反馈裁定），planner 持有 roadmap 效果段原文与派发上下文，且核对对象是 acceptance-author 的产出而非自己的，角色不冲突；对不上或存疑 → 升 architect 复核，仍不清 → 走既有升级链。`planner.md` intake 流程补条款；派 acceptance-author 的 brief 必须携带 roadmap 本包效果段（pod-retro 教训 2 的稳定落点）。
- **锚 3 · demo 对账**：包收口（integrator review-packet / supervisor 消费）必含 demo 对账行——demo 证据（形态按 roadmap 拆包时定的）+ 与本包效果段的一致性结论；不一致 = 走既有 escalation 通道就地升裁（裁断记 pod-events、roadmap 状态标 deviation+裁定锚），禁攒到集成夜。升级通道的具体形态不在本批定义（deputy 影子预裁系未定型实验，260912 human 指示不作为设计依据）。`pod-coordination.md` 补约定。
- **列车视角**：roadmap 达成状态由 render-roadmap.mjs 从 board/merge 状态 + 锚 3 对账记录**机械渲染**（包 merge 即 achieved、有 deviation 记录即标注）；**下一包 ready 由 reconcile 机械计算**（依赖已满足 + WIP 未满 + mergeOrder 最靠前），human 照常点火（worktree add + 起 planner，现行流程不变）——**绿路径上 portfolio-planner 沉默，不 per-merge、不 per-launch 例行出场**；与 roadmap 的匹配判断已在锚 3 于合并前完成，不后置到 merge 之后。portfolio-planner 仅在**调度异常**时由 human 唤起重裁：锚 3 deviation 影响后续包（依赖它的包是否照原计划跑）、依赖关系与实际不符、WIP / mergeOrder 需重排。

**目标文件**：`.claude/agents/planner.md`（改）、`.claude/references/pod-coordination.md`（改）

## 3. 影响面 sweep 分类（UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 模板 | docs/templates/WORKSTREAM-PROPOSAL.template.md | 新增（G2） | 三问 + 分型骨架；消费方：主线程（起草）、human grilling、portfolio-planner（intake 核验）、门 1 判官 |
| 模板 | docs/templates/WORKSTREAM-ROADMAP.template.md | 新增（G2） | 消费方：portfolio-planner（产）、门 1 判官 + human（审）、planner/acceptance-author（锚） |
| 脚本 | .claude/scripts/render-roadmap.mjs | 新增（G3） | advisory 生成器非门；消费方：portfolio-planner、roadmap 复核 |
| agent def 正文 | .claude/agents/portfolio-planner.md | G2 | Step 1 intake 增模板符合性核验（fail-closed）+ Step 4 全量条款 + Step 6 前门 1 步骤；不动 frontmatter（authoring 纪律逐字段隔离） |
| agent def 正文 | .claude/agents/planner.md | G2 | 锚 2 条款 + brief 携带效果段义务；不动 frontmatter |
| reference | .claude/references/task-intake-and-slicing.md | G2 | 双轨注记 + 模板引用 + planRef ③路径注记 + 门 1 checklist 新节 + story 锚点引用转写小节（句法首钉 + 例句换新） |
| reference | .claude/references/pod-coordination.md | G2 | 锚 3 demo 对账约定 + reconcile 再唤起 + 偏差升级走既有 escalation 通道 |
| skill | .claude/skills/triage-change/SKILL.md | G2 | ③档起草 PROPOSAL 按 WORKSTREAM-PROPOSAL.template.md |

## 4. human 取舍点（OQ）

| OQ | 选项 | 推荐 | 理由 |
|---|---|---|---|
| OQ1 门 1 深度 | (a) architect 轻量 checklist review；(b) 完整对抗 review（challenger↔defender） | (a) | 一次派发 vs 每 workstream 一场重流程；存疑向严升级通道已有，「拆包是方向性决策」可支持 (b)，取决于愿为方向错误付多少保险费 |
| OQ2 demo 对账消费 | (a) human 每包过一眼（约五分钟/包）；(b) planner 产对账、human 仅核心包必看 + 抽查 | (a) | show demo 的价值在有人真看；全交 agent 会退化成又一层绿灯 |
| OQ3 plan.md 豁免 | (a) 全取消；(b) 保留极窄例外（装配型自证 + 门 1 复核） | (a) | 豁免判断即拆包 agent 判断，判错裸奔（本批 PKG-1 误豁免实例在册）；装配型包 plan.md 约十行，成本可付 |
| OQ4 门 1 加入后，冻结前 architect 分流 review 去留 | (a) 保留独立 review（起草 → grilling → architect 审 → 冻结）；(b) 并入门 1（拆包后一次审 PROPOSAL 意图 + 切法） | (a) | 260911 的 F6/F7/F8 均为冻结前 review 喂进 PROPOSAL 的实质改进——裁定冻结前有 architect 对抗，价值已证；(b) 省一次派发，但裁定冻结后再翻案成本高、且门 1 责任面过载 |

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` 复跑绿（agent def 正文改动后）。
2. `node .claude/scripts/render-roadmap.mjs --self-check` 绿；对 260911-display-interaction-sovereignty 实跑：产出依赖图与 decision-coverage.json 包集一致（3 包 + 1 批次①前置），达成状态可从 board/merge 状态渲染。
3. 回溯演练（真阳性检查，一次性）：以门 1 checklist 核 260911 拆包产物，应点名 key-route-a1 缺 plan.md（T2 命中）与 PKG-2 单包承载 4/5 收口标准（T3 切片逻辑项命中）——判据 = 本批已发生的真实缺陷能被新门拦住。
4. WORKSTREAM-ROADMAP.template.md 存在且含业务目的清单硬项节 + 效果段回指律（story ID + AC 号集）；task-intake-and-slicing.md 含 spec 关系标注约定 + story 锚点引用转写小节（`S-Fx.y-AC<n>` 句法 + 例句换新）；portfolio-planner.md / planner.md 引用行在位（route-regression 必留信号核对）。

## 6. 留根清单

- 模型档位策略：挂起（260912 human 裁后移）；待按 pod-events / verify-runs 的返工轮次 + token 数据按档位统计后定夺。
- 门 1 机械化为 launch 前置闸：若人读 + checklist 反复漏检再机械化（先例：check-launch 门）。
- 260911 批次不回溯补 roadmap（自下批适用）。
- `Sx-Ay` 老仓遗产字样：散在 acceptance-evidence.md / code-review-gate.md / builder.md / dev-loop.md / acceptance-author description——非本批 manifest 面、不扩 landing，按 ⑪ 钉的 `S-Fx.y-AC<n>` 句法另登 ARQ 批量对齐。
- ARQ 结清关系：T2 落地后结清 ARQ-260911-split-package-implementation-prose-gap；ARQ-260911-cross-package-behavior-owner-gap 拆包期判据由 T3 checklist 消化、派单侧义务保留该条在册。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
