# PROPOSAL · 260912-gov-review-portfolio-decomposition-chain

> **状态：FROZEN v1.0 · 2026-09-13**（260912 拆包→执行链 retro 对话起草。**立项 ack = 260913 human 指示「commit 后启动 review」+ 同会话 AskUserQuestion 显式确认**（n-16 载体补认）；评审轨迹：R1 打回 17 条 → v0.8；R2 打回 6 条 → v0.9；**R3 GREEN**（§7，23 条 findings 全闭合，熔断未触发）。**FROZEN 依据 = 立项 ack + R3 GREEN**（Track B 第 3 步）；landing 以本版为唯一冻结基线，landing-manifest 9 文件 packet-bound）。
> **v0.1→v0.2 修订**（human 五点反馈，260912）：① workstream 效果目标升级为业务目的清单硬项；② 方案空间裁定增 spec 关系标注（两源以 spec 为权威）；③ 锚 2 核对者改 planner（agent）、非 human；④ merge 后 portfolio-planner 例行唤起改为状态机械渲染 + 事件驱动唤起；⑤ 移除 deputy 预裁参考（未定型实验，human 指示不作设计依据）。
> **v0.2→v0.3 修订**（260912 human 确认）：⑥ 「下一包 launch 唤起」收窄撤销——常规 ready 由 reconcile 机械计算 + human 照常点火（现行流程不变），**绿路径上 portfolio-planner 沉默**；其再出场条件收窄为「调度异常时由 human 唤起重裁」（锚 3 deviation 影响后续包 / 依赖与实际不符 / WIP·mergeOrder 需重排）。
> **v0.3→v0.4 修订**（260912 human 确认）：⑦ workstream PROPOSAL 内容硬项重钉为「**通用三问 + 分型**」——① 要达成什么（业务目的清单）② 现在在哪（随 workType 三子形，「问题与根因」仅整改型合法）③ 怎么走（方案空间 + 裁定）；frontmatter 增 `workType`（可混合、按业务目的逐条标）。替换 v0.2 隐含的整改型通用骨架（260911 系整改型形态，不具通用性）。
> **v0.4→v0.5 修订**（260912 human 确认）：⑧ `需修订` 类裁定的执行语义钉死为**受影响包的硬 launch 依赖**——spec 修订未冻结（review + human 冻结落地），承载该裁定的包不得 launch；依赖粒度 = 包（经 decision-coverage 映射登记 launchPreconditions，非整 workstream 陪等）；spec 修订在 PROPOSAL 冻结后尽早并行发起（在受影响包关键路径上）。堵死「包先跑、spec 事后补」。
> **v0.5→v0.6 修订**（260912 human 指示 + 纠正）：⑨ workstream PROPOSAL **模板化**——新增 `docs/templates/WORKSTREAM-PROPOSAL.template.md`（三问 + 分型成模板）。**起草方 = 主线程**（输入常为 human 与主线程讨论的产物，起草权不转移至 portfolio-planner——260912 human 纠正，避免下游写上游输入的倒置）；portfolio-planner **intake 增模板符合性核验**（缺业务目的清单 / workType / spec 关系标注 → fail-closed 打回，不猜不代写）。/triage-change ③档起草 PROPOSAL 时按本模板。
> **v0.6→v0.7 修订**（260913 human 指示「requirements 重 review 不影响定拆包规则」+ write-story a118f75 定稿对齐）：⑩ 拆包转写规则落地 = **锚点引用**（story 引用 = `S-Fx.y · 标题` + 可选 AC 号集，不内嵌原文——write-story 已废内嵌口径）；⑪ AC 级回引复合句法全仓首钉 `S-Fx.y-AC<n>`，task-intake sourceRefs 老仓例句（`S1-A1` 系）换新；⑫ plan.md 骨架首项随 workType 分流（原硬锚「问题与根因」在 T1 分型下仅整改型合法）；⑬ roadmap 效果段增回指律（story ID + AC 号集、只投影不改写），锚 2 核验含 AC 回引对账；⑭ 第一问 intake 增两道机查：已砍编号不引用、🔴 story 须先立项处置；⑮ `需修订` 类 spec 修订落点随语义面二分（requirements / architecture），launchPreconditions 按落点核验。
> **v0.7→v0.8 修订**（260913 R1 打回逐条修复 + human 三裁定）：**human 裁**——M-1 = convention + 人读（launch 核验不改既有机械闸判定语义，机械齿登 ARQ 留根）；§4 OQ1–OQ4 全部 (a) 定案；立项 ack 确认（载体 = 状态行）。**修复**——⑯ B-1：manifest 幻影目标 `.claude/skills/triage-change/SKILL.md`（实测不存在）换真身 `.claude/references/change-triage.md` + `.claude/commands/triage-change.md`；⑰ M-1：「spec 修订已冻结」launch 核验钉为 convention（执行者人读核 launchPreconditions + 按落点二分核 spec 文件），launchPreconditions 补 canonical 定义（workstream decision-coverage.json 顶层键，沿 260911 先例）；⑱ M-2：锚 3 对账行义务经 planner.md Step 11 派 integrator 的 dispatch prompt 传递（与锚 2 brief 携带同构，不扩 manifest）；⑲ M-3：roadmap ↔ task_plan ↔ progress 职责划界（roadmap 只投影不裁决）；⑳ M-4：门 1 派发者 = human/主线程派 architect，portfolio-planner AskUserQuestion 停等（frontmatter 无 Agent() 也不加）；㉑ m-5～m-11 + n-12～n-15 逐条落（已砍清单位置无关化、⑭ 执行相位钉 portfolio-planner Step 1 + 模板自检行、§5 验证口径如实化、留根枚举补全、锚 2 AC 对账按 workType 分流、§1 R4 引文补逐字、实跑断言口径、句法分工注记、T3 补重构型、workType frontmatter 钉数组形状 + 三子形节名逐字一致、render-roadmap 输入面与 deviation 事件型名钉死）。
> **v0.8→v0.9 修订**（260913 R2 打回逐条修复；本轮无新增 human 裁点——R2-M1 改法由 M-1 既有裁定「convention + 不动既有机械面」精神直接导出）：㉒ R2-M1：锚 3 deviation 事件载体改挂**既有** `human-decision` 事件型 + `payload.decision="anchor3-deviation-ruling"` 区分值（eventType 系 pod-events-append.mjs 机械闭集、未知型即 throw——不扩闭集、不动 G3/SSOT 面，沿 `approve-escalated-merge` 先例形态），render-roadmap 判据同步；㉓ R2-m1：门 1 裁决耐久记录 = append `.planning/<ws>/review.md`「门 1」节（GREEN 不留聊天）；㉔ R2-m2：§5-2 补「跨 workstream dependsOn 目标渲染为带标注外部节点」口径（origin.workstream 圈定的实测边界）；㉕ R2-n1：锚 2 brief 携带效果段钉「路径 + 节锚引用」形态（防双源漂移）；㉖ R2-n2：planner.md 格式例换新边界——acceptance 数组保持本地 id 形态，`S-Fx.y-AC<n>` 只入 sourceRefs 示范位；㉗ R2-n3：§5-4 措辞对齐 T1「投影不复制」+ 补 WORKSTREAM-PROPOSAL 模板内容断言。
> **v0.9→FROZEN v1.0**（260913 R3 GREEN，两条 NOTE 按判官授权 FROZEN 前机械折入、不启新轮）：㉘ R3-n1：锚 3 deviation 的 pod-events append 姿势钉 `--actor human`（五要素核验含 actor==='human'，与合并人批凭据 `approve-escalated-merge` 对称）；R3-n2：㉕ 的实现通道择一钉死 = planner.md 派 acceptance-author 的 dispatch prompt 加行（备选「roadmap.md 入 taskpackage `docRefs` 经 brief-inputs.mjs 自动展开」不用——脚本零改动、不触 manifest 外文件）。
> **格式追认（260914 landing 收口）**：§2 四处锚行加粗形态 `**目标文件**：` 改裸形态 `目标文件：`（`landing-fidelity-check.mjs` 的 `parseSection2AnchorLists` 只认裸形态，B7）+ T1 锚行注记撤幻影路径 token（B-1 修订行已留痕）——纯格式、零语义改动，landing-verifier D4 处置。
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
.claude/references/change-triage.md
.claude/commands/triage-change.md
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
4. **执行无锚（R4）**：无 roadmap、无包级 demo 判据、拆包者 episodic 离场后无常驻列车视角；执行中偏差无预挂裁断通道。**且效果锚从源头无源可引**：workstream 效果目标本身退化为一句话——MVP v0.4 §0「目标效果一句话」实文 =「把 spec §4 全部 story（…）拆成一组排好序、带依赖、模拟优先、硬件殿后的可并行 TaskPackage」（逐字，m-10 补足省略段），即**拆包动作的自述而非业务效果**（孩子能做什么、看到什么，一处未落纸）；260911 批 §0 虽好一档（「听得见的答案、看得见的落点、按得动的按键」），但仍是一句概括、未枚举成可逐条 demo 的业务目的清单。

## 2. 改法（T1…T4）

> 总形：一条「意图钉死 → 方向受审 → 执行对锚 → 完成对账」的链。门 1 治 R1+R2，T2 治 R3，锚 2/锚 3 治 R4。

### T1 · 制品双轨分离：PROPOSAL 回归裁决记录，新增 workstream roadmap 活文档

PROPOSAL（.planning 通道）只管「三问 + 裁定」（要达成什么 / 现在在哪 / 怎么走），维持冻结；「列车怎么跑、走到哪了」独立为活文档 roadmap.md，落 `.planning/<workstream>/roadmap.md`。

**workstream PROPOSAL 内容硬项 = 通用三问 + 分型**（**落新模板 `docs/templates/WORKSTREAM-PROPOSAL.template.md`**；`task-intake-and-slicing.md` 注记引用；frontmatter 增 `workType`——**形状钉死 = 字符串数组**（`workType: [remediation, new-capability]`，取值 new-capability / remediation / refactor，可混合是常态先例：260911 = 三个整改目的 + A2 新建），逐条业务目的的类型归属在清单正文内标注；三子形节名固定字面「问题与根因」/「现状与差距」/「结构性痛点」，与 T2 plan.md 骨架锚名、roadmap ① 节投影引用**逐字一致**（§5-4 断言，n-14））：

- **起草方与 intake 核验（v0.6 human 指示 + 纠正）**：**起草方 = 主线程**（/triage-change ③档路径）——输入常为 human 与主线程讨论的产物（260912 本批 retro 即实例形态），主线程按模板起草 → human grilling（AskUserQuestion 裁 OQ）→ architect 分流 review → FROZEN，起草权不转移至 portfolio-planner（下游写上游输入是倒置）。**portfolio-planner intake 增模板符合性核验**：业务目的清单 / workType / spec 关系标注缺失或不合规 → fail-closed 打回主线程补齐，不猜、不代写。

- **第一问 · 要达成什么**——业务目的清单：枚举式、每条含可观察判据（人能看到/听到/按到什么），禁一句概括了事（R4 MVP 实证：目标句写成拆包动作自述，业务效果无处落纸）。清单是 roadmap 每包效果段的**唯一上游源**。每条目的来源随分型：整改型引缺陷证据，新建型引 spec story（只加判据、不改语义）。story 引用形态一律**锚点引用**（`S-Fx.y · 标题` + 可选 AC 号集，合法形态以 write-story 为准）——story 文本随 spec review 演进（requirements v1.10 起仍在重 review），锚点引用不受其扰。新建型引 story 另有两道 intake 机查（fail-closed；**执行方与相位 = portfolio-planner Step 1**，与 ⑨ 模板符合性核验同一打回出口、打回对象 = 主线程，不猜不代写同律，m-6）：① 引用编号不得命中 requirements.md「已砍清单」节——**位置无关定位**（该节自 requirements v1.2 起在文末 §8，按节名 grep、不按行号/文件头，m-5）；② 可行性 = 🔴 的 story 须先有立项处置记录（write-story 🔴 挂账纪律：Pod 无权自裁），否则该目的不得入清单。起草侧预检：WORKSTREAM-PROPOSAL.template.md 自带这两查的自检行，主线程 FROZEN 前自查，降低冻结后打回。
- **第二问 · 现在在哪**——形态随 workType 分三子形：**新建型** = 现状盘点（已就绪基础、可复用资产、前置批次状态）+ 差距 = 目标能力 − 现状（「还没建」，非「坏了」）；**整改型** = 问题行为 + 证据锚（file:line / 实测）+ 根因——「问题与根因」标题仅此形态合法；**重构型** = 结构性痛点证据 + 为什么现在动。现状盘点即 RTE 拆包的原料输入（供给 R1/R4 的缺口面）。
- **第三问 · 怎么走**——方案空间 + OQ 裁定 + 非目标 + 逃生门申报。
- **方案空间裁定标注 spec 关系**：PROPOSAL §方案空间每条裁定必须带标注，写明它与 spec 的关系是三种里的哪一种——`within-spec`（spec 已定此语义，裁定只选实现路径，不动语义——标注时引 spec 条款号）/ `需修订`（裁定要改 spec 已钉死的语义 → **必须先走 spec 修订流程**：Track B `/arch-change` → 对抗 review → human 冻结；spec 改完这条裁定才生效，**不得在 PROPOSAL 内自冻成第二事实源**。**执行语义 = 受影响包的硬 launch 依赖**：spec 修订未冻结，承载该裁定的包不得 launch——依赖登记到该包 `launchPreconditions`（粒度 = 包，经 decision-coverage 映射，非整 workstream 陪等；先例形态：260911 PKG-4 前置挂批次① overlay 修复）；spec 修订在 PROPOSAL 冻结后尽早并行发起，与拆包 / 门 1 / 其它包执行同时进行。堵死「包先跑、spec 事后补」）/ `spec-silent`（spec 未载此事——留根登记，由门 1 或收口定是否补进 spec）。两源并存时的权威序恒为：**spec = 行为语义权威，PROPOSAL = 路径选择记录**——260911 批 §7 申报机制（OQ4=A 判 spec 零修订、OQ3=c 判无语义变更）是本标注的先例形态，本项将其升格为逐条硬项。
- **workType 往下游传**：整改型必有红基线（有既有行为可红）、门 1 侧重「根因 ↔ 改法」对得上；新建型无红基线、门 1 侧重切片与覆盖；重构型门 1 侧重结构性质可验收。

**roadmap 模板**：新增 `docs/templates/WORKSTREAM-ROADMAP.template.md`，固定节：① workstream 效果目标与现状差距（引用投影自 PROPOSAL §0 的业务目的清单，不复制）；② 列车图（依赖 + 批次，由 render-roadmap.mjs 从 taskpackage.json 的 dependsOn/mergeOrder 机械渲染，人工禁改该节）；③ 每包条目——changeId、mergeOrder、一段「完成后人能看到什么」的业务效果（**人只写机器写不出的这一段**，从 workstream 业务目的清单分解而来）+ **回指本包承载的 story ID + AC 号集**（新建型；只投影判据、不改写不复述 story 语义——与第一问「只加判据、不改语义」同律，防效果段长成 story 的第二释义层）、demo 证据形态（模拟器截图 / host runner 输出 / 串口日志 / 真机轮；真机轮配额默认仅核心包）、达成状态（pending / achieved / deviation+裁定锚，由脚本从 board/merge 状态机械渲染）；④ 非目标与批次归属。新增 `.claude/scripts/render-roadmap.mjs`（读 workstream 下 taskpackage JSON 集 → 产出 mermaid 依赖图块 + 达成状态渲染，含 `--self-check`）。**与既有 workstream 制品的划界（M-3）**：task_plan.md frontmatter 维持 `planning-index.mjs` 机器索引源地位（本批不动该脚本），progress.md 维持进度记录；roadmap.md = **效果对账 + 列车图投影**活文档——包集与达成状态的权威恒 = board + taskpackage JSON，重叠处以投影侧为准（roadmap 只渲染不裁决，不成第二事实源）；模板 ② 节「人工禁改」处同句声明渲染源唯一。**render-roadmap.mjs 输入面钉死（n-15）**：taskpackage JSON 集按 `origin.workstream` 圈定 + board.json + deviation 事件载体 = 各包 pod-events.jsonl 的**既有事件型** `human-decision` + `payload.decision="anchor3-deviation-ruling"` 区分值（锚 3 裁定时写入，**append 姿势钉 `--actor human`**——五要素核验含 actor==='human'，与合并人批凭据对称，R3-n1。R2-M1：eventType 是 `pod-events-append.mjs` 机械闭集、未知型即 throw，不扩闭集、不动 SSOT——沿合并人批凭据 `approve-escalated-merge` 的先例形态；render-roadmap 的 deviation 判据按此区分值渲染）；全量重算、幂等；达成状态刷新时机 = 包 merge 后由 human 合并簿记顺跑一次（绿路径 portfolio-planner 沉默），门 1 与收口相位亦可调用。`task-intake-and-slicing.md` 补注记：PROPOSAL §拆包预判降级为意图提示，拆包权威 = 四件套经门 1 冻结。

目标文件：`docs/templates/WORKSTREAM-PROPOSAL.template.md`（新增）、`docs/templates/WORKSTREAM-ROADMAP.template.md`（新增）、`.claude/scripts/render-roadmap.mjs`（新增）、`.claude/references/task-intake-and-slicing.md`（改）、`.claude/references/change-triage.md`（改，③档程序源：起草按模板）、`.claude/commands/triage-change.md`（改，Step 4③ 补一句按模板起草）、`.claude/agents/portfolio-planner.md`（改，Step 1 intake 增模板符合性核验 + ⑭ 两道 story 引用机查）

### T2 · plan.md 全量义务（收紧 ARQ-260911-split-package-implementation-prose-gap）

每个 TaskPackage 拆包时必备 `.claude/taskpackages/<changeId>/plan.md`，taskpackage.json `planRef` 指向；「装配型豁免」**全取消**（§4 OQ3 = (a)，260913 human 裁——豁免判断权在拆包 agent 自身即利益冲突，PKG-1 误豁免实证在册）。plan.md 骨架：**背景摘录（锚点随 workType 分流——整改型锚 PROPOSAL「问题与根因」节、新建型锚「现状与差距」、重构型锚「结构性痛点」）**、改法取舍、阶段划分、铁律约束、demo 证据形态复述（与 roadmap 一致）。

**story 转写规则（write-story 边界指给本链的落点，a118f75 新口径）**：story 在 plan.md / taskpackage 中**只以锚点引用出现**——`S-Fx.y · 标题` + 可选 AC 号集，不内嵌原文、不改写、不复述语义；story 语义以 requirements.md 现文为唯一权威，taskpackage `story` 字段承锚点引用（技术描述归 plan.md，治 260829 期 story 字段写成技术方案的真实翻车）。**AC 级回引复合句法全仓首钉**：`S-Fx.y-AC<n>`（如 `S-F1.4-AC3`），验收齿 / sourceRefs 的 AC 粒度回引按此形态填。**句法分工注记（n-12）**：prose 锚点引用形态的权威 = write-story（其 description「拆包引用 story 的合法形态以本 skill 为准」管 prose 层）；AC 粒度机器引用的复合 token 句法 = task-intake 转写小节首钉（write-story 边界节已把 TaskPackage 承载/转写规则委派本链，R1 判官核为委派范围内、无冲突）——两句注记同落该小节，防未来会话拿 description 括注反判复合句法违规。

`portfolio-planner.md` Step 4 补全量条款，`task-intake-and-slicing.md` planRef 节同步 + 新增 story 锚点引用转写小节（上形规则 + 分工注记 + sourceRefs 老仓例句 `S1-A1`、`docs/systems/S1-leads/spec.md:S1-A1` 换为本仓新形态）。**manifest 内遗产字样同批换新（m-8）**：`planner.md` TaskPackage 格式例（`S1-A1` 四处 + 「all declared Sx-Ay acceptance tests pass」）与 `pod-coordination.md` §2b 例句（「`:Sx-Ay` 等锚点」）——两文件均已在 manifest，纯 additive 小改顺手闭环；manifest 外命中面维持留根另登 ARQ（§6）。**换新边界（R2-n2）**：planner.md 格式例的 acceptance 数组保持本地 id 形态（如 `KR-A1`，实测现行），`S-Fx.y-AC<n>` 只出现在 sourceRefs 示范位——防例子教出「acceptance id = story AC 号」的混淆。

目标文件：`.claude/agents/portfolio-planner.md`（改）、`.claude/references/task-intake-and-slicing.md`（改）

### T3 · 拆包 review 门（门 1）

拆包四件套齐备（roadmap.md + taskpackage.json × N + plan.md × N + decision-coverage.json）后、commit-before-launch 前，插一道 architect 轻量 review：固定 checklist 五项——① 切片逻辑（按行为切还是按页面/问题切，图上见，偏离垂直切片须有显式理由）；② 依赖序与 WIP 合理性；③ decision-coverage 完备（含跨包行为 owner 显式登记）；④ 每包意图陈述与 PROPOSAL 冻结裁定一致；⑤ demo 证据形态可行（真机轮配额仅核心包）。checklist 侧重随 workType 分流（T1 workType 下传：整改型核根因↔改法 + 红基线在位；新建型核切片与覆盖；重构型核结构性质可验收，n-13）。**门 1 派发形态（M-4）**：同 /triage-change 派发姿势——human / 主线程派 architect（只读审 + 返回裁决、零写入 canonical）；portfolio-planner frontmatter 无 `Agent()` 也不加（authoring 纪律：加派发白名单属 frontmatter 变更），其 Step 6 前插步骤 = 「四件套齐备 → AskUserQuestion 请 human 发起门 1 → 收到 GREEN + human 切法过目后方可 commit-before-launch」（停等，不绕行）。**`需修订` 类裁定的 launch 核验 = convention + 人读（M-1，260913 human 裁）**：spec 修订 = 受影响包 launch 前置依赖（见 T1）；launch 执行者（human 点火前）人工核 workstream decision-coverage.json 顶层 `launchPreconditions` + 按落点二分核对应 spec 文件冻结状态（story 语义 → requirements.md、支撑语义 → architecture.md）——**不改 `portfolio-reconcile.mjs --check-launch` 既有判定语义**（机械齿另登 ARQ 留根，§6）；条款落 `pod-coordination.md` §2b + `task-intake-and-slicing.md` 门 1 checklist 节（均已在 manifest）。**launchPreconditions canonical 定义（M-1②）**：落点 = workstream decision-coverage.json 顶层键，条目形态沿 260911 先例（包 id + 前置描述 + 核验落点），定义落 `task-intake-and-slicing.md`。GREEN 后 human 过目切法（立项 ack 的组成部分：**方向 + 切法一次前端对齐**——切法自此有票），方可 commit-before-launch。**门 1 裁决耐久记录（R2-m1）**：verdict + checklist 逐项结论 + 派发/裁决时点 append 到 `.planning/<ws>/review.md`「门 1」节——GREEN 不得只留在聊天（本批自己反对的形态）。checklist 落 `task-intake-and-slicing.md` 新节，`portfolio-planner.md` Step 6 前插「门 1」步骤引用。存疑向严：触契约面 / 跨系统 / 判官不绿 → 升 Track A（`/arch-change`）。

目标文件：`.claude/agents/portfolio-planner.md`（改）、`.claude/references/task-intake-and-slicing.md`（改）

### T4 · Pod 内两锚：验收方向核（锚 2）+ demo 对账（锚 3）

- **锚 2 · 验收方向核**：acceptance 冻结后、builder 动工前，**planner（agent）**对照 roadmap.md 本包效果段核一遍验收方向一致，含 **AC 回引对账（按 workType 分流，m-9）**——新建型（效果段带 story 回指）：验收齿回引的 AC 号集与效果段回指的 AC 号集对得上（多引、少引都是方向偏差信号）；整改型：核红基线 ↔ 根因方向；重构型：核结构性质可验收（与 T1/T2/T3 三处分流对齐）——核对者是 planner 而非 human（验收语义方向不是 human 能逐条核清的，260912 human 反馈裁定），planner 持有 roadmap 效果段原文与派发上下文，且核对对象是 acceptance-author 的产出而非自己的，角色不冲突；对不上或存疑 → 升 architect 复核，仍不清 → 走既有升级链。`planner.md` intake 流程补条款；派 acceptance-author 的 brief 必须携带 roadmap 本包效果段（pod-retro 教训 2 的稳定落点）——**携带形态 = 路径 + 节锚引用**（`.planning/<ws>/roadmap.md#<changeId>` 的效果段节），不整段转抄，防双源漂移（roadmap 经 F-03 在基准 ref 可读，R2-n1）；**实现通道择一钉死（R3-n2）** = planner.md 派 acceptance-author 的 dispatch prompt 加行（备选「roadmap.md 入 taskpackage `docRefs` 经 brief-inputs.mjs 自动展开」不用——脚本零改动、不触 manifest 外文件）。
- **锚 3 · demo 对账**：包收口（integrator review-packet / supervisor 消费）必含 demo 对账行——demo 证据（形态按 roadmap 拆包时定的）+ 与本包效果段的一致性结论；不一致 = 走既有 escalation 通道就地升裁（裁断记 pod-events——事件载体按 T1 钉的 `human-decision` + `payload.decision="anchor3-deviation-ruling"` 形态、roadmap 状态标 deviation+裁定锚），禁攒到集成夜。升级通道的具体形态不在本批定义（deputy 影子预裁系未定型实验，260912 human 指示不作为设计依据）。`pod-coordination.md` 补约定；**生产者可达性（M-2）**：义务经 `planner.md` Step 11 派 integrator 的 dispatch prompt 显式携带「review-packet 必含 demo 对账行」——与锚 2 brief 携带效果段同构（派发侧传递），integrator.md 不在本批 manifest、不扩面。
- **列车视角**：roadmap 达成状态由 render-roadmap.mjs 从 board/merge 状态 + 锚 3 对账记录**机械渲染**（包 merge 即 achieved、有 deviation 记录即标注）；**下一包 ready 由 reconcile 机械计算**（依赖已满足 + WIP 未满 + mergeOrder 最靠前），human 照常点火（worktree add + 起 planner，现行流程不变）——**绿路径上 portfolio-planner 沉默，不 per-merge、不 per-launch 例行出场**；与 roadmap 的匹配判断已在锚 3 于合并前完成，不后置到 merge 之后。portfolio-planner 仅在**调度异常**时由 human 唤起重裁：锚 3 deviation 影响后续包（依赖它的包是否照原计划跑）、依赖关系与实际不符、WIP / mergeOrder 需重排。

目标文件：`.claude/agents/planner.md`（改）、`.claude/references/pod-coordination.md`（改）

## 3. 影响面 sweep 分类（UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 模板 | docs/templates/WORKSTREAM-PROPOSAL.template.md | 新增（G2） | 三问 + 分型骨架；消费方：主线程（起草）、human grilling、portfolio-planner（intake 核验）、门 1 判官 |
| 模板 | docs/templates/WORKSTREAM-ROADMAP.template.md | 新增（G2） | 消费方：portfolio-planner（产）、门 1 判官 + human（审）、planner/acceptance-author（锚） |
| 脚本 | .claude/scripts/render-roadmap.mjs | 新增（G3） | advisory 生成器非门；消费方：portfolio-planner、roadmap 复核 |
| agent def 正文 | .claude/agents/portfolio-planner.md | G2 | Step 1 intake 增模板符合性核验（fail-closed）+ ⑭ 两道 story 引用机查（同一打回出口）+ Step 4 全量条款 + Step 6 前门 1 停等步骤（AskUserQuestion 请 human 发起）；不动 frontmatter（authoring 纪律逐字段隔离） |
| agent def 正文 | .claude/agents/planner.md | G2 | 锚 2 条款（AC 对账按 workType 分流）+ brief 携带效果段义务 + Step 11 派 integrator dispatch 携带 demo 对账行义务（M-2）+ TaskPackage 格式例 `S1-A1`/`Sx-Ay` 换新（⑪/m-8）；不动 frontmatter |
| reference | .claude/references/task-intake-and-slicing.md | G2 | 双轨注记 + 模板引用 + planRef ③路径注记 + 门 1 checklist 新节（含 M-1 convention 核验条款）+ story 锚点引用转写小节（句法首钉 + 分工注记 + 例句换新）+ launchPreconditions canonical 定义 |
| reference | .claude/references/pod-coordination.md | G2 | 锚 3 demo 对账约定 + §2b launchPreconditions 人读核验条款（M-1）+ reconcile 再唤起 + 偏差升级走既有 escalation 通道 + `:Sx-Ay` 例句换新（m-8） |
| reference | .claude/references/change-triage.md | G2 | ③档程序源：起草 PROPOSAL 按 WORKSTREAM-PROPOSAL.template.md（B-1 换真身） |
| command | .claude/commands/triage-change.md | G2 | Step 4③ 补一句按模板起草（B-1 换真身；原列幻影 skill 路径废弃） |

## 4. human 取舍点（OQ）

| OQ | 选项 | 推荐 | 理由 |
|---|---|---|---|
| OQ1 门 1 深度 | (a) architect 轻量 checklist review；(b) 完整对抗 review（challenger↔defender） | (a) | 一次派发 vs 每 workstream 一场重流程；存疑向严升级通道已有，「拆包是方向性决策」可支持 (b)，取决于愿为方向错误付多少保险费 |
| OQ2 demo 对账消费 | (a) human 每包过一眼（约五分钟/包）；(b) planner 产对账、human 仅核心包必看 + 抽查 | (a) | show demo 的价值在有人真看；全交 agent 会退化成又一层绿灯 |
| OQ3 plan.md 豁免 | (a) 全取消；(b) 保留极窄例外（装配型自证 + 门 1 复核） | (a) | 豁免判断即拆包 agent 判断，判错裸奔（本批 PKG-1 误豁免实例在册）；装配型包 plan.md 约十行，成本可付 |
| OQ4 门 1 加入后，冻结前 architect 分流 review 去留 | (a) 保留独立 review（起草 → grilling → architect 审 → 冻结）；(b) 并入门 1（拆包后一次审 PROPOSAL 意图 + 切法） | (a) | 260911 的 F6/F7/F8 均为冻结前 review 喂进 PROPOSAL 的实质改进——裁定冻结前有 architect 对抗，价值已证；(b) 省一次派发，但裁定冻结后再翻案成本高、且门 1 责任面过载 |

> **260913 human 裁定：OQ1–OQ4 全部 (a) 定案**（AskUserQuestion 显式确认；判官 R1 逐项同意推荐，理由见 §7 R1 分焦点 E）。OQ3 定案 (a) = 装配型豁免全取消，已落 T2 正文。

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` 复跑绿（agent def 正文改动后）。
2. `node .claude/scripts/render-roadmap.mjs --self-check` 绿；对 260911-display-interaction-sovereignty 实跑（**归档位** `.planning/_archive/2026/`，m-11）：产出依赖图与 decision-coverage.json 的 **launchPreconditions commit 清单**一致（key-route-a1 / screen-unification / display-first-m1 + 批次①前置 ecs-deploy-convergence；「包集」判据取 launchPreconditions 面、**非 landsIn 全集**——后者另含 render-pages 共 4 包），达成状态可从 board/merge 状态渲染。**圈定口径（R2-m2）**：taskpackage 集按 `origin.workstream` 圈定（实测 ecs-deploy-convergence / render-pages 的 origin.workstream = kidpal-mvp，不在本 workstream 圈定内）；批次①前置经 dependsOn 进图——断言口径 = **跨 workstream 的 dependsOn 目标渲染为带标注外部节点**，不计入本 workstream 包集。
3. 回溯演练（真阳性检查，一次性）：以门 1 checklist 核 260911 拆包产物，应点名 key-route-a1 缺 plan.md（T2 命中）与 PKG-2 单包承载 4/5 收口标准（T3 切片逻辑项命中）——判据 = 本批已发生的真实缺陷能被新门拦住。
4. WORKSTREAM-ROADMAP.template.md 存在且含 **① 效果目标投影节**（投影自 PROPOSAL 业务目的清单、不复制——措辞与 T1 划界句对齐，R2-n3）+ 效果段回指律（story ID + AC 号集）；WORKSTREAM-PROPOSAL.template.md 含三问 + 分型三子形 + workType 数组形状 + 起草侧两查自检行（内容断言，R2-n3）；task-intake-and-slicing.md 含 spec 关系标注约定 + story 锚点引用转写小节（`S-Fx.y-AC<n>` 句法 + 分工注记 + 例句换新）+ launchPreconditions canonical 定义；**模板三子形节名与 T2 plan.md 骨架锚名、roadmap ① 节投影引用逐字一致**（n-14 断言）。**机械/人读口径如实化（m-7）**：`route-regression-check.mjs` 的 agentRequiredSignals 只覆盖 review-loop 六 agent，portfolio-planner / planner 无必留信号——复跑 = description 回归保护（本批不动 description，无新信号断言）；正文引用行在位 = **landing-verifier 逐文件人读核对项**，不声称机械覆盖。

## 6. 留根清单

- 模型档位策略：挂起（260912 human 裁后移）；待按 pod-events / verify-runs 的返工轮次 + token 数据按档位统计后定夺。
- 门 1 机械化为 launch 前置闸：若人读 + checklist 反复漏检再机械化（先例：check-launch 门）。
- 「spec 修订已冻结」launch 核验机械齿（`portfolio-reconcile.mjs --check-launch` 读 launchPreconditions）：本批 convention + 人读（260913 human 裁 M-1，守住「不改既有机械闸判定语义」的 riskTier 声明）；若人读核验反复漏检再机械化——另登 ARQ 留根。
- 260911 批次不回溯补 roadmap（自下批适用）。
- `Sx-Ay` / `S1-A1` 老仓遗产字样（grep 实测 13 文件命中，m-8 补全枚举）：**manifest 内三处本批顺手换新**——task-intake sourceRefs 例句（⑪）、planner.md 格式例四处 + exitCriteria 句、pod-coordination §2b 例句；**manifest 外**（integrator.md / build-feature.md / minimal-implementation.md / acceptance-evidence.md / code-review-gate.md / builder.md / dev-loop.md / acceptance-author description / acceptance-id-shared.mjs / mutation-check.mjs 等）不扩 landing，按 ⑪ 钉的 `S-Fx.y-AC<n>` 句法另登 ARQ 批量对齐。
- ARQ 结清关系：T2 落地后结清 ARQ-260911-split-package-implementation-prose-gap；ARQ-260911-cross-package-behavior-owner-gap 拆包期判据由 T3 checklist 消化、派单侧义务保留该条在册。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 2026-09-13 · architect 评审

**verdict：打回**（BLOCKING 1 / MAJOR 4 / MINOR 7 / NOTE 5，共 17 条。打回主因 = B-1 幻影 manifest 目标 + M-1～M-4 四件落点/可达性/派发者缺口；均为可修复项，不动摇 T1–T4 主体设计与 §4 推荐，修复成 v0.8 后可径入下一轮。）

**地面真值抽查（先证后审，均属实，小偏差单列）**：§1 R3 机械清点复跑一致——`.claude/taskpackages/` 25 目录（含 `current`）、plan.md 仅 4 份且全为 260911-*、MVP 期 19 包（260829×12 + 260831/260901/260903×6 + 260907-g-relay-fix）plan.md/planRef 双缺、key-route-a1 无 plan.md；260829-schedule-reminder taskpackage `story` 字段逐字命中技术方案化实文（「脑算网关推：OpenClaw cron…」，T2 翻车实例成立）；`S-F1.4` = 日程提醒、AC3 实存（requirements.md:166，⑪ 例句 `S-F1.4-AC3` 合法）；260911 workstream decision-coverage.json 顶层实存 `launchPreconditions` 三条（含 PKG-4 批次① overlay 前置，先例形态属实）；MVP/260911 两批 §0 效果句与 §1 R4 定性相符（逐字性小差见 m-10）；承载 ARQ 四条均在 ARCH-REVIEW-QUEUE §1.2 在册——T2 对 ARQ-260911-split-package-implementation-prose-gap 的承接属**收紧**（原建议分级豁免 → 本件全量义务 + OQ3 裁豁免）、T3 checklist ③ 消化 ARQ-260911-cross-package-behavior-owner-gap 拆包期判据（260911 decision-coverage.json 已首用 `crossPackageBehaviors`，owner=key-route-a1，判据有先例可核）、与 tooth-strength / verdict-uncovered 两条「相邻不重叠」声明成立（该两条治齿强度与 verdict 申报面，本件治拆包链承载面）。

**B-1 [BLOCKING]**（landing-manifest 第 8 行 + §3 sweep「skill」行 + T1 目标文件段）幻影 manifest 目标：`.claude/skills/triage-change/SKILL.md` **不存在**——实测 `.claude/skills/**/SKILL.md` 22 件无 triage-change；`/triage-change` 真身 = `.claude/commands/triage-change.md`（入口命令，Step 4③ 即「③大 feature → .planning/<ws>/PROPOSAL.md」落盘点）+ `.claude/references/change-triage.md`（③档程序源，:25/:56）。⑨「③档起草 PROPOSAL 按本模板」义务因此无落点；packet-bound 落地时 landing-fidelity-check 将对不存在目标失败，或诱使误建影子 skill 与 command 并存。改法：manifest 与 §3 行换真文件——建议 `.claude/references/change-triage.md`（程序源，必列）+ `.claude/commands/triage-change.md`（Step 4③ 补一句按模板起草），T1 目标文件段同步。

**M-1 [MAJOR]**（T1「需修订」段 + T3 + 头部⑮）「spec 修订已冻结」核验链不闭合。check-launch 真身 = `portfolio-reconcile.mjs --check-launch`（pod-coordination §2b 机械承载，脚本 :327/:354 实核），现 check 集 = taskpackage.json / PROPOSAL / decision-coverage，**不读 launchPreconditions**——该字段全 scripts/references/agents/hooks 零命中，仅活在 260911 workstream decision-coverage.json 顶层键与包 notes prose，无 canonical 定义；portfolio-reconcile.mjs 亦不在本批 manifest。两难：若做机械核验 = 改既有门判定语义，与头部 riskTier「不改任何既有机械闸判定语义」自相矛盾且文件未列；若为 convention = 人读核验，则条款文字落点未指明（T3 目标文件与 §3 pod-coordination 行均未含 §2b 改动）。改法：① 显式钉死为 convention——launch 执行者人工核 workstream decision-coverage.json 的 launchPreconditions + 按⑮落点二分核对应 spec 文件（requirements / architecture）冻结状态，条款落 pod-coordination §2b（已列）或 task-intake 门 1 checklist 新节（已列）；② 同批给 launchPreconditions 一句 canonical 定义（落点 = workstream decision-coverage.json 顶层；条目形态沿 260911 先例），落 task-intake（已列）；③ 若要机械齿，另登 ARQ 留根或列 portfolio-reconcile.mjs 入 manifest 并如实升格 gate-fact 声明。**[需 human 裁]**：机械齿本批做不做（做 = 改门语义须升声明；不做 = convention + 留根）。

**M-2 [MAJOR]**（T4 锚 3 + §3 pod-coordination 行）锚 3 义务对生产者不可达：demo 对账行 = 「integrator review-packet / supervisor 消费必含」，落点仅 pod-coordination.md；integrator.md References 节（:87-95 实核七件）**不含 pod-coordination.md**——正文 :73 内联指针只覆盖 §10a 冲突消解 runbook，且 integrator.md 不在 manifest。按现引用面，义务在 integrator 收口相位不可发现，锚 3 会空转。改法二选一：① manifest 增 integrator.md，References 只加一行引用（承 260906/260907 批「agent def 只加引用、结构零改动」先例）；② 在 planner.md（已列）Step 11 派 integrator 的 dispatch prompt 钉死携带 demo 对账行要求（与锚 2 brief 携带条款同构，派发侧传递）。

**M-3 [MAJOR]**（T1 roadmap 段）roadmap.md 与既有 task_plan.md 职责边界未划：现行 `.planning/<ws>` 的列车视角由 task_plan.md 承载（260911 归档实核：frontmatter workstream/status/changes 列表 + 目标 + 设计权威节；`planning-index.mjs` 机械消费产 .planning/INDEX.md + index.jsonl），进度另有 progress.md。roadmap.md 的列车图/达成状态/每包条目与二者在列车面重叠，全文未声明三者关系（替代/并存/分工）——这正是本件自己要防的第二事实源形态。改法：T1 补划界句——task_plan frontmatter 维持 planning-index 机器索引源地位（本批不动 planning-index.mjs）；roadmap = 效果对账 + 列车图**投影**活文档，包集与达成状态权威恒 = board + taskpackage JSON；重叠处以投影侧为准（roadmap 只渲染不裁决）。roadmap 模板 ② 节「人工禁改」处同句声明渲染源唯一。

**M-4 [MAJOR]**（T3）门 1 的派发者未指明：portfolio-planner frontmatter tools 无 `Agent()`（实核：Read/Edit/Write/Grep/Glob/Bash/AskUserQuestion/mcp 图工具），技术上派不出 architect 做门 1 review，而本批声明「不动 frontmatter」（也不应动——加派发白名单属 authoring 纪律的 frontmatter 变更）。T3 只写「插一道 architect 轻量 review」，未钉谁派发、portfolio-planner 在门 1 期间停等还是走开。改法：钉门 1 形态 = 同 /triage-change 派发姿势（human / 主线程派 architect，architect 只读审 + 返回裁决、零写入）；portfolio-planner Step 6 前插步骤 = 「四件套齐备 → AskUserQuestion 请 human 发起门 1 → 收到 GREEN + human 切法过目后，方可 commit-before-launch」，落 portfolio-planner.md（已列）+ task-intake 门 1 checklist 节（已列）。

**m-5 [MINOR]**（T1 第一问 ⑭-①）「requirements.md **文件头**『已砍清单』」位置指针过时——requirements v1.2（2026-09-13）起已砍清单自文件头移至**文末 §8**（requirements.md:395 实核 + :423 变更记录行）。requirements 正处重 review 期，位置描述应免腐化：改「requirements.md『已砍清单』节（现 §8）」或索性位置无关化。

**m-6 [MINOR]**（T1 ⑭ + §3 portfolio-planner 行）⑭ 两道机查的执行方与相位未钉：链上已定义的 intake = portfolio-planner Step 1（⑨ 模板符合性核验），但「否则该目的不得入清单」语气又约束起草侧；§3 sweep portfolio-planner 行只列「模板符合性核验」未含两查。改法：钉 ⑭ = portfolio-planner Step 1 机查两项（与 ⑨ 同一 fail-closed 打回出口、打回对象 = 主线程，不猜不代写同律），§3 行补字；同时 WORKSTREAM-PROPOSAL.template.md（已列）自带起草侧自检行（含已砍编号 / 🔴 处置两查），让主线程在 FROZEN 前预检，降低冻结后打回。

**m-7 [MINOR]**（§5-4）「portfolio-planner.md / planner.md 引用行在位（route-regression 必留信号核对）」与机械闸实际覆盖不符：`.claude/route-fixtures/routing.json` 的 agentRequiredSignals 实核只覆盖 review-loop 六 agent（review-moderator / scenario-author / challenger / defender / adjudicator / architect-reviewer），portfolio-planner 与 planner **无必留信号**；且该脚本断言 frontmatter description 与 command 路由，不断言正文引用行。引用行在位核对实为 landing-verifier 人读项。改法：§5-4 如实改写（route-regression 复跑 = description 回归保护，本批不动 description 故无新信号断言；正文引用行在位 = landing-verifier 逐文件核对），或给 routing.json 增补信号——后者属 G3 变更须入 manifest，二选一写明。

**m-8 [MINOR]**（§6 留根第 4 条）「Sx-Ay 老仓遗产字样散在五处、**非本批 manifest 面**」被实证证伪：grep `S1-A1|Sx-Ay` 命中 13 文件，其中 **planner.md**（TaskPackage 格式例 S1-A1 四处 + 「all declared Sx-Ay acceptance tests pass」）、**pod-coordination.md** §2b（「`:Sx-Ay` 等锚点」例句）、**task-intake-and-slicing.md**（⑪ 已承接其 sourceRefs 例句）均在 manifest 内；另有 integrator.md / build-feature.md / minimal-implementation.md / acceptance-id-shared.mjs / mutation-check.mjs 未列入枚举。改法：补全枚举并点名 manifest 内文件的遗产字样处置——planner.md 格式例建议直接纳入 ⑪ 换新（已列文件、纯 additive 小改，顺手闭环）；pod-coordination §2b 例句同判；manifest 外文件维持另登 ARQ 批量对齐。

**m-9 [MINOR]**（T4 锚 2 vs T1 ⑬）AC 回引对账未按 workType 分流：⑬ 回指律只约束**新建型**（效果段回指 story ID + AC 号集），T4 的 AC 对账却写成无条件义务——整改/重构型包效果段无 AC 号集回指，执行时无对账对象。改法：T4 补「AC 回引对账适用承载 story 回指的新建型包；整改型核红基线 ↔ 根因方向、重构型核结构性质可验收」（与 T1/T2/T3 三处分流对齐，消 B 焦点咬合缝）。

**m-10 [MINOR]**（§1 R4）MVP §0 引文非逐字：实文 = 「把 spec §4 全部 story（…）拆成一组排好序、带依赖、**模拟优先、硬件殿后**的可并行 TaskPackage」（kidpal-mvp PROPOSAL:15），引文脱「模拟优先、硬件殿后」而自称「实文 =」。定性不受影响（仍是拆包动作自述、无业务效果），按仓输出标准「证据引用保持逐字精确」补足或加省略标记。

**m-11 [MINOR]**（§5-2）实跑断言口径两点不精：① 260911-display-interaction-sovereignty 已归档 `.planning/_archive/2026/`（实核），实跑路径应指归档位；② 「decision-coverage.json 包集（3 包 + 1 批次①前置）」= launchPreconditions commit 面口径（key-route-a1 / screen-unification / display-first-m1 + 前置 ecs-deploy-convergence），但 landsIn 目标含 render-pages（4 包）——应写明「包集」判据取 launchPreconditions commit 清单而非 landsIn 全集，否则 render-roadmap 实跑产出与断言可能对不上号。

**n-12 [NOTE]**（⑪/T2 vs write-story，A 焦点结论）句法归属**现状已自洽、无冲突**：write-story 边界节显式把「TaskPackage 怎么承载/转写 story（含验收齿如何挂接）」委派拆包链治理，其 description「拆包引用 story 的合法形态以本 skill 为准」管的是 prose 锚点引用层（`S-Fx.y · 标题` + 可选 AC 号集）；复合 token `S-Fx.y-AC<n>` 属验收齿/sourceRefs 的 AC 粒度机器引用形态，在委派范围内，task-intake 首钉合法。建议加固一句显式分工注记落 task-intake 新小节：「prose 引用形态权威 = write-story；AC 粒度机器引用复合 token 句法 = 本节首钉」——防未来会话拿 description 括注反判复合句法违规（write-story a118f75 同日定稿，本批不动它）。

**n-13 [NOTE]**（T3）workType 分流括注漏重构型（T1 有「重构型门 1 侧重结构性质可验收」，T3 括注只列整改/新建两型），落地时对齐字面即可。

**n-14 [NOTE]**（T1 frontmatter + 两模板）workType「可混合」的 frontmatter 形状未定（数组？主类型 + 正文逐条标？），模板须钉死一种机器可读形态；模板三子形节名（「问题与根因」/「现状与差距」/「结构性痛点」）须与 T2 plan.md 骨架锚名、roadmap ① 节投影引用逐字一致——建议 §5-4 断言补「模板节名三处一致」一条，防锚点引用漂移。

**n-15 [NOTE]**（T1/T4 render-roadmap.mjs）输入面与 deviation 判据未钉：「锚 3 对账记录」的耐久载体（review-packet 固定节？pod-events 事件型名？）未定义，「有 deviation 记录即标注」无机器判据；建议脚本条款钉死输入面（taskpackage JSON 集按 origin.workstream 圈定 + board.json + deviation 事件载体与类型名）与全量重算幂等语义，并写明达成状态刷新时机与执行者（绿路径 portfolio-planner 沉默下，由谁在 merge 后跑——human 合并簿记顺跑或 hook 触发，一句钉死）。

**n-16 [NOTE]**（头部状态行，程序项）状态行仍写「human 未立项 ack」——Track B 第 3 步 FROZEN 前置 = 立项 ack + GREEN。判官无权改头部，注记在此：主线程 FROZEN 前补认立项 ack 的记录载体与日期（若 v0.6→v0.7 的 human 指示已构成立项 ack，应把载体钉进状态行）。

**分焦点结论**：**A**（⑩–⑮ vs write-story 新口径）方向一致、无实质冲突，残余 = n-12（加固注记）/ m-5（位置指针）/ m-6（相位）/ M-1（⑮ 核验链）；**B**（T1–T4 咬合）三子形 ↔ ⑫ ↔ T3 ↔ ⑬ 主体咬合，缝 = m-9（锚 2 未分流）/ n-13（重构型漏列）/ n-14（节名一致性），⑭ fail-closed 语义与 ⑨ 打回出口同构、相容，需钉相位（m-6）；**C**（manifest 完整性）⑩⑫⑬ 有落点，⑨ 目标幻影（B-1）、⑮ 无 manifest 内落点（M-1）、⑭ Step 不明（m-6）、锚 3 生产者不可达（M-2）、门 1 派发者缺（M-4）；**D**（既有治理面）Track B 路由自洽（governance 类、不触契约面、非跨系统结构变更；G3 新增仅 advisory 生成器——以 M-1 钉死 convention 为前提，riskTier 声明方成立）；与 A10 / decisionCoverage / landsIn 闭集文法相容（planRef、story 字段沿既有约定；launchPreconditions 需 canonical 定义，并入 M-1）；§5 验证集两处覆盖声明与实况不符（m-7 / m-11）；**E**（§4 OQ）四项推荐全部同意——OQ1(a)：门 1 是方向与切法的人读审，五项 checklist + 存疑向严升 Track A 通道已在，完整对抗 per-workstream 的派发成本不对称；OQ2(a)：show demo 的价值在 human 真看，全交 agent 即 R2「事后才抓出」的复发土壤，约五分钟/包成本与「集成夜才爆」的代价相称；OQ3(a)：PKG-1（key-route-a1）误豁免实证在册——豁免判断权在拆包 agent 自身即利益冲突，装配型 plan.md 约十行成本可付；OQ4(a)：260911 F6/F7/F8 证明冻结前对抗喂进实质改进，并入门 1 = 裁定冻结后再翻案 + 门 1 责任面过载，推荐与本轮 findings 无冲突；**F**（非目标与留根）无 scope creep（T1–T4 均可回指 R1–R4，非目标三项划界清楚、模型档位挂起有 260912 human 裁依据），留根枚举需修（m-8）。

**[需 human 裁] 汇总**：① M-1 分叉——check-launch「spec 修订已冻结」机械齿本批做不做（做 = 改门判定语义、须升格 riskTier 声明并列 portfolio-reconcile.mjs 入 manifest；不做 = convention + 人读，机械化登 ARQ 留根）；② §4 OQ1–OQ4（判官意见 = 四项均同意推荐，裁断权在 human）；③ n-16 立项 ack 载体补认（程序事实确认）。

### R2 · 2026-09-13 · architect 评审

**verdict：打回**（MAJOR 1 / MINOR 2 / NOTE 3，共 6 条新 findings。R1 的 17 条：**16 条真修复、1 条（n-15）半修复且衍生新 MAJOR**——`anchor3-deviation` 事件型撞 pod-events 机械闭集。三条新 findings 均一句话可修，不动摇 T1–T4 主体设计与 human 三裁定落记；修成 v0.9 后 R3 复审面收窄 = R2-M1 修复核对 + R2-m1/m2 落点抽查，预计单轮收敛。未触熔断（本件第 2 轮）。）

**复审方法**：v0.8 修订文本（⑯–㉑）逐条对照 R1 findings 核真修复（非字面应付），并对修复引入的新句法/新事件型/新 canonical 定义做机检亲核——`pod-events-append.mjs` 闭集与 throw 行为、`dev-loop-operating-system.md:130` 闭集 SSOT、260911 四包 `origin.workstream`/`dependsOn` 实际值、`acceptance-id-shared.mjs:36` extractShortId 正则、全仓 pod-events.jsonl 对 `versioned-artifact-claim` 的命中。

**一、R1 十七条逐条核验**

- **B-1 ✓ 真修复**：manifest（:25-26）幻影路径已除、换真身 `.claude/references/change-triage.md` + `.claude/commands/triage-change.md`，§3 表（:108-109）与 T1 目标段（:71）同步且如实注记「原列…不存在」。承载力亲核：change-triage.md :25（③档行「reviewed plan → .planning/<ws>/PROPOSAL.md（人类冻结）」）+ :56（③大落盘行）、command Step 4③（:28）均为 ⑨「按模板起草」义务的自然落点，落点实在。
- **M-1 ✓ 真修复**（承 human 260913 裁 = convention + 人读）：T3（:85）钉「launch 执行者人工核 workstream decision-coverage.json 顶层 launchPreconditions + 按落点二分核 spec 文件冻结状态」+ 显式「不改 `portfolio-reconcile.mjs --check-launch` 既有判定语义」；条款落点 pod-coordination §2b + task-intake 门 1 节（均在 manifest，§3 :106-107 同步）；launchPreconditions canonical 定义落 task-intake（:85/:106）；机械齿留根 §6（:133）。riskTier「不改任何既有机械闸判定语义」声明由此自洽。
- **M-2 ✓ 真修复**（取 R1 改法②）：T4（:92）锚 3 义务经 planner.md Step 11 派 integrator 的 dispatch prompt 显式携带，planner.md 在 manifest、§3（:105）同步。与 planner.md Step 7「不转抄正文」纪律无冲突——该纪律绑 acceptance-author 双 brief 派发面，Step 11 是另一派发面（携带形态建议见 R2-n1）。
- **M-3 ✓ 真修复**：T1（:69）划界句四要素齐——task_plan frontmatter 维持 planning-index.mjs 机器索引源（本批不动该脚本）、progress.md 维持进度、权威恒 = board + taskpackage JSON、roadmap 只渲染不裁决 + 模板 ② 节渲染源唯一声明。
- **M-4 ✓ 真修复**：T3（:85）钉派发形态（human/主线程派 architect，只读审 + 返回裁决零写入）+ portfolio-planner Step 6 前插停等步骤（AskUserQuestion 请 human 发起 → GREEN + 切法过目 → commit-before-launch）。相容性亲核：portfolio-planner frontmatter（:4）tools 含 AskUserQuestion、不加 Agent()（authoring 纪律守住）；插步位置在 Step 5 board 初始化之后——无害（reconcile 全量重算幂等，pod-coordination §4 :59；门 1 打回重排可覆盖 board 态）。残余派生 = R2-m1（裁决耐久载体）。
- **m-5 ✓**：T1（:63）位置无关定位（「按节名 grep、不按行号/文件头」+ 注明现 §8）。**m-6 ✓**：执行方与相位 = portfolio-planner Step 1（:63），模板自检行 + 打回出口同一（:63），§3（:104）与 T1 目标段（:71）三处同步。**m-7 ✓**：§5-4（:127）机械/人读口径如实化（六 agent 无必留信号 + 正文引用行 = landing-verifier 人读项，不声称机械覆盖）。**m-8 ✓**：§6（:135）枚举补全（manifest 内 3 处 + 外 10 处 = 13 文件，与 R1 grep 实测吻合），manifest 内同批换新义务落 T2（:79）+ §3（:105/:107）同步。**m-9 ✓**：T4（:91）锚 2 AC 对账三形分流，与 T1/T2/T3 对齐。**m-10 ✓**：§1 R4（:49）引文补逐字 + 省略标记。**m-11 ✓**：§5-2（:125）归档位 + launchPreconditions commit 清单口径 + landsIn 含 render-pages 注记（与 n-15 输入面交互派生 R2-m2，见下）。
- **n-12 ✓**：T2（:77）分工注记两句同落转写小节。**n-13 ✓**：T3（:85）补重构型。**n-14 ✓**：T1（:59）workType 钉字符串数组 + 三子形节名固定字面 + §5-4（:127）逐字一致断言。**n-16 ✓**：头部状态行（:3）立项 ack 载体钉死。
- **n-15 半修复**：输入面圈定（origin.workstream）/ 幂等全量重算 / 刷新时机（human 合并簿记顺跑，绿路径 portfolio-planner 沉默）✓；但首钉事件型 `anchor3-deviation` 撞机械闭集 → 升级 R2-M1；圈定方式与 §5-2 断言交互出 R2-m2。

**二、human 三裁定落记核对（如实、无越权代裁痕迹）**：① M-1 = convention + 人读——T3 :85「（M-1，260913 human 裁）」+ §6 :133 + 头部 ⑰，裁定主体归 human、载体 AskUserQuestion ✓；② OQ1–OQ4 全 (a)——§4 :120 裁定行（AskUserQuestion 显式确认 + 判官意见仅引为意见）+ T2 :75 正文随 OQ3=(a) 改「全取消」并注明裁定来源 ✓；③ 立项 ack——头部 :3 载体钉死、状态行同步「评审中（R1 打回已修，待 R2）」与实况相符 ✓。判官侧未代裁任何 human 取舍点；⑯–㉑ 修订行与正文改动逐条对得上。

**三、新 findings（v0.8 修复引入 / 派生）**

**R2-M1 [MAJOR]**（T1 :69 n-15 钉死段 + T4 :92 锚 3「裁断记 pod-events」）首钉事件型 `anchor3-deviation` **写不进去**——pod-events eventType 是机械强制闭集：`pod-events-append.mjs` :28-35 `VALID_EVENT_TYPES` 仅六型（dispatch-completed / agent-escalation / agent-escalation-resolved / human-decision / human-correction / friction），:47-48 未知型**即 throw**；闭集 SSOT 成文 = `dev-loop-operating-system.md:130`。「自由事件型」无落地先例——pod-coordination §10c :148 的 `versioned-artifact-claim` 虽声明「自由事件型」，实测全仓 pod-events.jsonl **零命中**（该约定从未以独立事件型落地，不足为凭）。按 v0.8 原文 landing 只有两条出路：标准 append 通道被脚本拒（条款不可执行），或扩闭集（`pod-events-append.mjs` + `dev-loop-operating-system.md` 两文件 G3/SSOT 变更，**均在 manifest 外**，且与 human M-1 裁定「不动既有机械面」精神及 riskTier 声明相抵）。**改法（推荐，manifest 内零扩面）**：deviation 裁定记录改挂既有闭集型 **`human-decision` + payload.decision 钉区分值**（如 `anchor3-deviation-ruling`）——精确先例 = 合并人批凭据 `payload.decision="approve-escalated-merge"`（planner.md :93，eventId 即凭据、消费模式现成）；render-roadmap.mjs（manifest 内新增）判据 = `eventType==='human-decision' && payload.decision` 匹配该值；T1 n-15 钉死段与 T4 锚 3「裁断记 pod-events」处各同步一句。备选（不推荐）：扩闭集并把两文件入 manifest + 如实升格声明——若选择即属翻转 M-1 裁定面，须另升 human。

**R2-m1 [MINOR]**（T3 :85）门 1 裁决的**耐久记录载体**未钉——M-4 修复钉了派发者/停等/放行条件，但 architect 返回的门 1 裁决（GREEN/打回 + findings）落哪份耐久制品未定义；「收到 GREEN」若无审计锚，即本批自己反对的「聊天记录当记忆」。改法：T3 补一句 = 门 1 裁决记录 append `.planning/<ws>/review.md`「门 1」节（architect 返回、主线程落盘——与 ③档 review.md 承载分流 review 同构，作者≠判官；转录者不改判词）。

**R2-m2 [MINOR]**（T1 :69 n-15 输入面 + §5-2 :125）跨 workstream dependsOn 目标的渲染口径缺——实测：批次①前置包 ecs-deploy-convergence 与 render-pages 的 `origin.workstream` 均 = **kidpal-mvp**（非 260911-display-interaction-sovereignty），「按 origin.workstream 圈定」输入面只得 3 包；批次①前置仅经 display-first-m1 `dependsOn`（实测 = [screen-unification, ecs-deploy-convergence]）以**外部节点**身份进图。§5-2 断言「依赖图与 launchPreconditions commit 清单一致（3 包 + 1 批次①前置）」成立的前提 = 外部节点渲染口径已钉，否则实跑对不上或脚本作者现造口径。改法：n-15 钉死段补「跨 workstream dependsOn 目标渲染为带标注外部节点、不计入本 workstream 包集」+ §5-2 口径注（= 3 域内节点 + 1 外部前置节点）。

**R2-n1 [NOTE]**（T4 :91 锚 2）「brief 必须携带 roadmap 本包效果段」的携带形态未钉——建议钉路径 + 节锚引用（roadmap.md 经 commit-before-launch 在基准 ref 可读，F-03 commit 集含 `.planning/<ws>/**`，portfolio-planner.md :46），与 planner.md Step 7「不转抄正文、防双源漂移」纪律同律；若确需内文携带，写明漂移防护理由。

**R2-n2 [NOTE]**（T2 :79 / §6 :135，m-8 换新执行细节）acceptance 本地 id 与 AC 回引 token 的区分——本仓实测 acceptance id = 包前缀本地 id（KR-A1 / SU-A1 / DF-A1，story AC 回引在齿文内承载）；机检：extractShortId 正则 `[A-Za-z0-9][A-Za-z0-9._-]*`（acceptance-id-shared.mjs :36）接受 `S-Fx.y-ACn` token，⑪ 复合句法机器相容 ✓。建议 planner.md 格式例换新时 acceptance 数组保持本地 id 形态、sourceRefs 示范复合句法，防例子教出「acceptance id = story AC 号」的混淆形态。

**R2-n3 [NOTE]**（§5-4 :127）「WORKSTREAM-ROADMAP.template.md 存在且含**业务目的清单硬项节**」与 T1 roadmap ① 节「引用投影自…不复制」（:69）措辞张力——字面会要求 roadmap 模板含清单本体。建议改「含 ① 效果目标投影节（引用业务目的清单、不复制）」，并趁势补 WORKSTREAM-PROPOSAL.template.md 内容断言行（三问 + workType 数组形状 + spec 关系标注 + ⑭ 自检行），补全两模板断言对称性。

**四、结论**：v0.8 对 R1 的修复质量高——16/17 真修复且落点全部在 manifest 内文件、human 三裁定落记如实；唯一新 MAJOR（R2-M1）是 n-15 修复首钉事件型时未核 pod-events 闭集所致，推荐改法（human-decision + payload.decision 区分值）有精确先例、零扩面、与 M-1 裁定精神一致。**[需 human 裁]：本轮无新增项**——R2-M1 推荐改法由 human 260913 M-1 裁定（convention + 不动既有机械面）精神直接导出；仅当选择「扩闭集」备选时才属翻转该裁定、需另升 human。R2-m1/m2/n1-n3 均判官职权内修复建议，不触目标效果。

### R3 · 2026-09-13 · architect 评审

**verdict：GREEN**（新增 findings：BLOCKING 0 / MAJOR 0 / MINOR 0 / NOTE 2。R2 的 6 条全部真修复并经机检亲核；至此 R1 17 条 + R2 6 条共 23 条全部闭合。**熔断状态：本件第 3 轮，GREEN 收敛，未触发**（熔断条件 = 3 轮未 GREEN）。按 Track B 第 3 步，主线程可凭「立项 ack（头部状态行在册）+ GREEN（本节）」标 FROZEN，进入第 4 步 packet-bound 落地。）

**复审面**（按 R2 预告收窄）：㉒–㉗ 逐条真修复核验 + ㉒ 与 pod-events 闭集/五要素人批路径的严格相容性机检 + v0.9 修复引入面扫描（git diff 51cb211..6e48113 = 仅本文件 +44/-8，无越界改动；㉒–㉗ 修订行与正文逐条对得上；状态行如实更新「R2 打回已修，待 R3」）。

**一、六条修复逐条核验**

- **㉒（R2-M1）✓ 真修复，机检三重全过**：T1 :70 deviation 事件载体改挂**既有** `human-decision` 型 + `payload.decision="anchor3-deviation-ruling"` 区分值，T4 :93 锚 3 正文同步对齐。相容性机检：① **可写入**——`pod-events-append.mjs` 对 payload 只查 `typeof === 'object'`（:59-61），无 payload.decision 白名单；eventType 走闭集内 `human-decision`，不触 :47-48 throw，**不扩闭集、不动 G3/SSOT 面** ✓；② **先例严格同构**——planner.md :93/:97 合并人批凭据 `payload.decision="approve-escalated-merge"`（eventId 即凭据）✓；③ **与合并门机械隔离且踩在预留扩展轨道上**——`governance-gate-shared.mjs` `resolveHumanApproval`（:532-560）五要素核验（eventType / actor / payload.decision / changeId / schemaVersion），`expectedDecision = deps.decision || 'approve-escalated-merge'`（:534）：合并人批路径对 `anchor3-deviation-ruling` 事件在 :557 payload.decision 比对处**机械拒绝**（无误消费可能）；且 :522-523 原注明文该参数化正是为「defer/accept-risk 锚核验复用同一五要素路径、decision 枚举值参数化」而设——㉒ 形态是既有治理设计**预留的扩展模式**，非权宜绕行。render-roadmap（manifest 内新增）判据按区分值渲染，闭合 R2-M1 全部要求。
- **㉓（R2-m1）✓ 真修复**：T3 :86 钉「门 1 裁决耐久记录」——载体 = `.planning/<ws>/review.md`「门 1」节（③档 review.md 先例，change-triage.md :56），内容要素 = verdict + checklist 逐项结论 + 派发/裁决时点，「GREEN 不得只留在聊天」。writer 可由相邻钉死句导出（M-4 条款「architect 只读审 + 返回裁决、零写入 canonical」→ 转录归主线程/portfolio-planner，二者 `.planning/**` 可写，portfolio-planner.md :67）。
- **㉔（R2-m2）✓ 真修复**：§5-2 :126 圈定口径与 R2 实测严格一致（ecs-deploy-convergence / render-pages 的 origin.workstream = kidpal-mvp，实测复核属实）；「跨 workstream dependsOn 目标渲染为带标注外部节点、不计入本 workstream 包集」使实跑断言可判（3 域内节点 + 1 外部前置节点，与 display-first-m1 实测 dependsOn 吻合）。
- **㉕（R2-n1）✓ 真修复**：T4 :92 携带形态钉「路径 + 节锚引用」（`.planning/<ws>/roadmap.md#<changeId>`），roadmap 模板 ③ 每包条目含 changeId（T1 :70）→ 节锚可解析；与 planner.md Step 7「不转抄正文、防双源漂移」纪律同构。派生实现通道细节见 R3-n2（NOTE，非阻塞）。
- **㉖（R2-n2）✓ 真修复**：T2 :80 换新边界——planner.md 格式例 acceptance 数组保持本地 id 形态（KR-A1 系，实测现行）、`S-Fx.y-AC<n>` 只入 sourceRefs 示范位。范围限定于**格式例**，与 ⑪ 主句「验收齿 / sourceRefs 的 AC 粒度回引按此形态填」不冲突（真实包的齿级回引在齿文/decisionCoverage 承载，非 acceptance id 位）——机检 extractShortId 正则相容性已于 R2 核过，不重复。
- **㉗（R2-n3）✓ 真修复**：§5-4 :128 改「① 效果目标投影节（投影自…不复制——措辞与 T1 划界句对齐）」，消解与 T1 :70 的字面张力；补 WORKSTREAM-PROPOSAL.template.md 内容断言行（三问 + 分型三子形 + workType 数组形状 + 起草侧两查自检行），两模板断言对称，n-14 节名逐字断言保留在位。

**二、修复引入面扫描（新缝）**：未发现 BLOCKING/MAJOR/MINOR 级新缝。扫描面 = ㉒ 事件形态对 pod-events 闭集 / 五要素人批路径 / render-roadmap 消费三面（机检如上，全相容）；㉓ review.md 追加对既有 ③档 review.md 语义的影响（同文件分节承载两类 review 记录，append-only，无冲突）；㉔ 外部节点口径对 n-15 输入面钉死段的一致性（两处口径互引一致）；㉕ 节锚对 roadmap 模板结构的依赖（changeId 条目首字段，在位）；㉖ 对 ⑪ 主句的作用域隔离（格式例限定词在场）；㉗ 对 §5 其余断言的无回归。两条落地相位细节降为 NOTE：

**R3-n1 [NOTE]**（T1 :70 / T4 :93，㉒ 派生）append 姿势的 **actor 要素**未钉——五要素核验含 `actor === 'human'`（governance-gate-shared.mjs :555），合并人批凭据即按 `--actor human` 落事件；`anchor3-deviation-ruling` 同为 human 裁断凭据，建议 landing 条款钉 append 姿势 `--actor human --source <裁断通道>`（与凭据形态对称，且未来若有消费方按 `deps.decision='anchor3-deviation-ruling'` 参数化复用五要素路径即天然兼容）。render-roadmap 判据（eventType + payload.decision）不含 actor，渲染不受影响——本条纯属凭据质量与复用兼容，不钉也能跑。

**R3-n2 [NOTE]**（T4 :92，㉕ 派生）锚 2「brief 携带效果段（路径 + 节锚）」的**实现通道**有两条 manifest 内合法路径未择一：① planner 派 acceptance-author 的 dispatch prompt 加一行 roadmap 路径 + 节锚引用（planner.md 条款面，已列）；② roadmap.md 路径进 taskpackage `docRefs`，经 `brief-inputs.mjs` 第②路 refs 驱动自动展开进 brief 必读清单（脚本零改动——其展开逻辑本就消费 docRefs）。建议 landing 时钉死一条，防执行者误读为需改 brief-inputs.mjs（manifest 外）。

**三、NOTE 处置与 GREEN 依据**：R3-n1/n2 均为落地相位操作细节，不触设计、不触目标效果、不动冻结判据——主线程可选择 FROZEN 前顺手折入正文（机械转录判官建议文字，不启新评审轮），或保持现文冻结、由 landing 执行者按本记录消费 + landing-verifier 对 §7 R3 两条 NOTE 的处置留痕核备；两种处置均不阻塞 FROZEN。GREEN 依据：23 条历史 findings 全闭合、manifest 9 文件落点全部真实存在且义务条款分布完备（B-1 修复后经 R2/R3 两轮复核）、human 三裁定落记经 R2 核如实且 v0.9 无新增裁点（㉒ 推导如实标注由 M-1 裁定精神导出）、riskTier / review profile / Track B 路由声明与修复后文本自洽（不改任何既有机械闸判定语义——㉒ 机检亲核为真）。

**[需 human 裁]：本轮无新增项。**
