# PROPOSAL · 260928-gov-review-dev-loop-preassess-manifest-scope

> **状态：DRAFT v0.1 · 2026-09-28 · 起草完成，human 预览通过、OQ 已裁（OQ-1=(a) / OQ-2=(a)），进 architect 评审循环**
> **human 凭据**：立项 ack = plan mode 批准（2026-09-28）；OQ-1 裁 (a) 接 scope-file、OQ-2 裁 (a) scoped 每轮（2026-09-28，会话内拍板）。
> **承载 ARQ**：无根——human 2026-09-28 直接立项（立项 ack = plan mode 批准：architect 前置技术评估 + changeManifest + 测试范围收敛三合一）。不关任何既有 ARQ 根。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T11）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（改 dev-loop 全链行为语义 + 新增一个机械闸 + 测试执行面收敛，均为治理文本与机检层；不触 `docs/contracts/**`，不触碰契约门 / landing-verifier / 升级闸三类不动件）。
> **review profile**：G2+G3（触面枚举：dev-loop 流程语义面 = agents/references 文本 G3；机检执行面 = verify-gates.json + hooks/scripts G2；`AGENTS.md` 链描述 G3。G1 / 契约面 = 零。）
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
AGENTS.md
.claude/agents/architect.md
.claude/agents/builder.md
.claude/agents/fixer.md
.claude/agents/integrator.md
.claude/agents/planner.md
.claude/hooks/verify.mjs
.claude/references/dev-loop.md
.claude/references/task-intake-and-slicing.md
.claude/scripts/acceptance-run-check.mjs
.claude/scripts/change-manifest-check.mjs
.claude/scripts/merge-driver.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/scripts/unit-run-check.mjs
.claude/verify-gates.json
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：TaskPackage 进 dev loop 前先经 architect 前置技术评估，产出三件咬合制品——技术方案（补写进 plan.md，留痕）、changeManifest（builder/fixer 的预定改动文件边界，越界走 architect 扩单通道）、impactedTests 种子（integration 测试范围圈定的输入）；integration 与合并收口的测试执行按影响面圈定跑子集，全量回归仅人工触发（如发版前），圈定失败或图不可用时举手升级 human。

**现状差距（三条，均有在册证据）**：

1. **planner 无前置技术评估环节**。planner.md 的 Process 中，architect 类派发只有 Step 1b 的图探索注记（planner.md:44「探索结果只进切片判断与 TaskPackage 字段，不替代 architect-reviewer 的契约判定」）与 Step 2 的契约预审（planner.md:49「对任何疑似契约变更先派 architect-reviewer 预审」）；architect 在 loop 内唯一出场位是 10b「ARCH-ADVISORY 疑难分析档」（planner.md:79），触发闭集以救火为前提（fixer 多轮失败、方向不明等），**未命中闭集不派**——技术方案评估在 loop 前没有系统位置。plan.md 承载位也在 260928 批刚被收窄：拆包侧 plan.md 判据钉死「单包内实现决策（改法取舍、阶段划分、时序细节、渲染接线、包内边角）不进拆包侧 plan.md」（task-intake-and-slicing.md:266），包内技术方案当前没有任何显式承载位与评估者。
2. **integration 每次全量跑测试，随包数线性变贵**。unit / acceptance 两功能闸 timing=full（verify-gates.json:75-85），执行器全仓枚举零筛选——unit-run-check.mjs:31/44（`FW_UNIT_RE` + `collectFiles`）与 acceptance-run-check.mjs:33/39/140-142（`PY_ACC_RE` + `collectFiles`）各自枚举 firmware/ 与 server/ 全部测试文件后整批执行（acceptance-run-check.mjs:238 `python -m pytest ... ${files.join(' ')}`）；唯一筛除是 `-m "not live"` 条目级标记。final 相位铁律逐字「只跑本包档不算完成」（integrator.md:81），Rationalizations 表逐字「集成风险只在收拢后暴露，必须全量验证」（integrator.md:111）。merge-driver quick-check 每次合并另跑一次全仓 acceptance 冒烟（verify-gates.json:26-27「merge-driver quick-check 冒烟子集 = contract-lint + acceptance 两门」）。taskpackage 已有 `acceptanceTestsFile[]` 等影响面记账字段（dev-loop.md:128），但没有任何闸消费它们缩测试范围；detect_changes 在 integrator.md:53（Step 11b）与 merge-driver.mjs:132-136（ARQ-100 advisory，「detect_changes 永不阻塞 merge、永不虚报影响面」）都只有 advisory 记账位。
3. **builder/fixer 改动文件无预定范围约束**。Track B 有 landing manifest（packet-bound 落地清单 + landing-fidelity-check 机检），dev-loop 侧没有对偶：builder 的边界只有包维度（builder.md:12「你在单个包边界内实现 TaskPackage」、:49「Boundaries / 权限边界」节），包内改哪些文件没有预定清单，也无机检对账。

**非目标（本批不做）**：不触 `docs/contracts/**`；不改验收测试纪律（host/live 两态、acceptance-author 职责）；不动契约门 / landing-verifier 独立核验 / 升级闸三类不动件；不改 portfolio-planner 拆包与调度逻辑；不引入新的 runner / 测试框架。

---

## 2. 问题、根因与改法（T1…T11）

**【硬项 · 起草期实证义务】**本节的断言纪律：凡 file:line 引据均起草期现核（上节三条差距内的引据已于 2026-09-28 逐条以 Grep 定点复核，复核输出在本会话记录在案）；凡声称「对偶 / 比照先例」已读先例文件（Track B landing manifest 机制见本 PROPOSAL §0 第 3 条差距的对照）。凡「全 / 均 / 无」量化声明（「全仓枚举零筛选」「唯一出场位」「没有任何闸消费」）覆盖集合 = unit-run-check.mjs / acceptance-run-check.mjs / verify.mjs / merge-driver.mjs / verify-gates.json 五件，复核时点 2026-09-28。

### T1 · planner 缺「前置技术评估」环节

**问题**：TaskPackage 进 loop 后直接切片派发，技术影响评估与技术方案确定没有系统环节；方案质量完全押后到 acceptance 断言与 10b 救火档，builder 起步时可能方向已偏。
**证据**：planner.md:79（10b 档原文「dev loop 进行中疑难问题可派 architect……」，触发闭集救火型、「未命中闭集不派」）；planner.md:44/49（architect 类派发仅图探索注记与契约预审）；task-intake-and-slicing.md:266（260928 OQ-1 收窄后包内实现决策无 plan.md 承载位）。
**根因**：dev-loop 设计时把 architect 定位在 loop 前（/triage-change 路由快审，change-triage.md:38「快审：确认判据 + 路由裁决」）与 loop 中（reactive advisory）两端，中间的「单包技术方案评估」真空——triage 快审不做深评，10b 只事后救火。
**改法**：planner.md Step 1b 之后新增 **Step 1c「ARCH-ADVISORY 技术预评档」**：对每个 intake 完成的 TaskPackage，planner 派 architect 做一次前置技术评估（只读、advisory 效力声明与 10b 档同款），产出三件：① 技术方案意见；② changeManifest（预定改动文件/目录清单，T4）；③ impactedTests 种子（T8 消费）。planner 将①补写进 plan.md 技术方案节（T3），②③回填 TaskPackage。契约疑点仍走 Step 2 的 architect-reviewer，不因 1c 档存在而跳过。派发文本锚（planner.md:44 原文，1c 插在其后）：「1b. Intake / 切片需要摸代码面（定 subsystem、绑符号、估影响面）时，用 search_graph / search_code / trace_path / get_architecture 做只读探索（优先于 Grep）；探索结果只进切片判断与 TaskPackage 字段，不替代 architect-reviewer 的契约判定。」
目标文件：`.claude/agents/planner.md`（改）

### T2 · architect 无前置评估职责位

**问题**：architect.md 的 dev-loop 职责只有「疑难问题 advisory 分析」（救火型），没有前置评估档位；planner 依 T1 派发时 architect 定义文件无对应 when-to-use 与产出物规范，三件产出（技术方案 / changeManifest / impactedTests 种子）无归属。
**证据**：architect.md:31（原文「**dev loop 中的疑难问题 advisory 分析**：planner 按 `[ARCH-ADVISORY 疑难分析档]` 派你，或 human 显式指定。……**只出意见、不落地、不实现、不改任何文件**」）；architect.md:3（description 枚举同款）。
**根因**：与 T1 同源——前置评估档从未被定义，architect 的 advisory 语义只在救火场景成文。
**改法**：architect.md 新增「**dev loop 前置技术评估**」职责段（与 :31 救火段并列，成对成文）：planner 按 1c 档派发时，评估技术影响、确定技术方案方向，输出固定三件（技术方案意见 / changeManifest / impactedTests 种子）；声明**扩单通道**——builder/fixer 实现中发现 changeManifest 范围不够时，经 planner 带根因假设回评，architect 评估后扩 manifest（扩单留痕：清单 append + 评估依据一句）；advisory 效力声明与 :31 同款照抄。description 行同步追加该档短语。
目标文件：`.claude/agents/architect.md`（改）

### T3 · plan.md 技术方案承载位缺口

**问题**：260928 OQ-1 收窄后，包内技术方案（改法取舍、接线决策、影响面）没有任何 plan.md 承载位；T1 产出的技术方案意见无处落盘，跨会话不可递、不可核。
**证据**：task-intake-and-slicing.md:266（原文「**判据一句钉死：跨包事实 / 共享冻结面约束……留拆包侧；单包内实现决策（改法取舍、阶段划分、时序细节、渲染接线、包内边角）不进拆包侧 plan.md**——包内细节设计承载位 = 冻结 acceptance 断言化 + TaskPackage `notes`/`exitCriteria`/`decisionCoverage` 备注 + planner 派发 prompt（260928 批 OQ-1 裁定）」）；dev-loop.md:129（②单包 plan.md 条目，planner「**只读消费、改需留痕**」）；change-triage.md:30（②单包 plan.md 内容框架仅一句「feature 写“建什么+方案”，bug 写“根因假设+修复+影响面”」）。
**根因**：260928 收窄裁定的动机是防拆包侧 plan.md 重复 planner 派发 prompt 的包内细节；但该裁定把「经 architect 评估的技术方案」一并逐出，与 human 本次立项要求（技术方案写在 plan.md 里）冲突——需要**有意识修订**而非默认沿用。
**改法**：task-intake-and-slicing.md:266 判据句后追加例外条款：「**例外（260928-gov-review-dev-loop-preassess-manifest-scope）**：plan.md 的「技术方案（planner+architect 预评）」节承载 architect 前置评估产出——改法取舍 / 影响面 / 关键接线决策；该节由 planner 在收到 1c 档评估后补写，**须留痕（日期 + architect 档标识）**；包内其余细节承载位判定照旧。」dev-loop.md:129 条目同步扩注 F8 语义：「只读消费、改需留痕」**加注**：技术方案节为唯一例外——planner 可在 architect 前置评估后补写，留痕规则同上；②单包已有 triage plan.md 则追加该节，③拆包由 planner 建包侧 plan.md 时写入。
目标文件：`.claude/references/task-intake-and-slicing.md`（改）· `.claude/references/dev-loop.md`（改）

### T4 · TaskPackage 无改动范围字段（changeManifest）

**问题**：TaskPackage schema 只有影响面记账字段（`acceptanceTestsFile[]`、`subsystem` 等，dev-loop.md:128），没有「本包预定改动哪些实现文件」的清单字段；T2 的 manifest 产出无处回填，T6 的机检无输入。
**证据**：dev-loop.md:126-128（标准内容字段枚举，逐字「`taskpackage.json`：sourceType、sourceRefs、deliveryImpact、planningImpact、riskTier、gateProfile、story、acceptance、exitCriteria、budget、contractRefs、docRefs」）；dev-loop.md:170（「`taskpackage.json` 可按风险声明附加字段」——schema 扩展的既有惯例位）。
**根因**：dev-loop 从未定义包内文件级改动边界——builder 边界只到包维度，文件维度一直靠自觉。
**改法**：dev-loop.md TaskPackage schema 节新增 `changeManifest[]` 字段：文件/目录 glob 清单（仓根相对），由 architect 前置评估（1c 档）产出、planner 回填；**扩单唯一通道 = architect 评估**（builder/fixer → planner → architect 回评，扩单 append + 评估依据留痕）；字段完备性由 T7 齿机检，内容对账由 T6 闸机检。挂靠位：dev-loop.md:128 标准内容清单追加字段名 + :170 附加字段节写消费细则（谁产出、谁消费、扩单通道、留痕规则）。
目标文件：`.claude/references/dev-loop.md`（改）

### T5 · builder/fixer 无文件级改动边界约束

**问题**：builder 与 fixer 在包边界内可改任意实现文件，改动越出 architect 预估的合理范围时既无停手机制也无扩单流程；Track B 的 packet-bound 纪律在 dev-loop 侧没有对偶。
**证据**：builder.md:12（「你在单个包边界内实现 TaskPackage」）、builder.md:49（「## Boundaries / 权限边界」节，无文件级条款）；fixer.md:50-51（边界仅「不改 canonical docs/spec」「不改 verifier gate、TaskPackage 事实或验收标准」，无文件级条款）。
**根因**：manifest 字段与 architect 前置评估此前不存在，两个 agent 的边界条款自然没有可引用的对象。
**改法**：builder.md「Boundaries / 权限边界」节新增条款：只改 `changeManifest` 范围内的文件；实现中发现需要改清单外文件 → **停手**，带根因假设交回 planner 派 architect 扩单（T2 通道），**不许先改后补、不许自行扩清单**。fixer.md 边界节同步同款条款（fixer 与 builder 同为实现面角色，越界风险同源，本批一并覆盖，不另立 OQ）。Rationalizations 表各加一行（“就改一个文件很快，先改了再说。” → 越界改动绕过了 architect 影响面评估，走扩单通道才是受控路径）。
目标文件：`.claude/agents/builder.md`（改）· `.claude/agents/fixer.md`（改）

### T6 · changeManifest 无机检对账闸

**问题**：T4/T5 的 manifest 纪律若无机检，退化为软约束——与「机械强制层不动摇」的既有治理取向相悖；越界 diff 会在无感知下溜进 commit。
**证据**：本仓机械闸惯例先例：taskpackage-required-check.mjs（TaskPackage 上下文在场机检，verify-gates.json:32-35 timing=scoped）与 landing-fidelity-check.mjs（Track B landing 清单对账）；verify-gates.json 闸挂载格式（:30-86）。
**根因**：manifest 是新概念，对账闸从未存在。
**改法**：新增 `.claude/scripts/change-manifest-check.mjs`：读 TaskPackage `changeManifest[]`（glob 展开），对 git diff（工作树 + 已暂存）涉及文件逐个对账——清单外文件即红（fail-closed）；manifest 缺失/为空/不可解析即红；提供 `--self-check` 自检入口（仿 taskpackage-required-check.mjs 惯例）。挂进 verify-gates.json 为新闸 `change-manifest`（kind=governance、timing=scoped，位置排在 taskpackage-required 之后）——每轮 verifier 都跑，builder 期越界早停早扩单。Phase 语义：scoped 相位按现有 fail-fast 规则遇红即停（verify.mjs:3）。
目标文件：`.claude/scripts/change-manifest-check.mjs`（新增）· `.claude/verify-gates.json`（改）

### T7 · taskpackage-required-check 无 changeManifest 完备性齿

**问题**：若 TaskPackage 可不带 `changeManifest[]` 通过结构闸，T6 闸会对存量/漏填包红得不知所云；字段完备性应在结构闸先验。
**证据**：taskpackage-required-check.mjs 既有先例——AC 回引号集对账齿、引用字段磁盘存在性 warn 齿等逐字段校验结构（该脚本现为 verify-gates.json 第一道 governance 闸，verify-gates.json:32-35）。
**根因**：字段是本批新增，完备性齿未存在。
**改法**：taskpackage-required-check.mjs 新增 changeManifest 完备性齿：在场、非空、元素为合法仓根相对 glob 形态；缺失/空 = 红（fail-closed，与「chat-only TaskPackage 不得过 verifier gates」的脚本头注立场一致）。存量兼容：生效切点用 changeId 日期前缀 cutoff（仿该脚本既有 `AC_BACKREF_CUTOFF = '2026-09-28'` 惯例），cutoff 前包不检查。
目标文件：`.claude/scripts/taskpackage-required-check.mjs`（改）

### T8 · integration 全量跑测试不可持续，无范围圈定

**问题**：final 相位与合并收口每次跑全量功能套件，包数增长下 integration 墙钟线性膨胀（human 立项直接痛点）；已冻结验收测试中大量与本包影响面无关的 case 被反复重跑。
**证据**：integrator.md:38（Step 4 原文含「全量 bearing verification」）；integrator.md:81（逐字「双方冻结件（本包 + 对端包）+ 全档 pytest + host ELF（文档命令位）+ 设备真编 + 九闸脚本。只跑本包档不算完成」）；integrator.md:111（Rationalizations 表逐字「集成风险只在收拢后暴露，必须全量验证。」）；integrator.md:53（Step 11b detect_changes 已在场但仅 advisory 记账，不驱动测试范围）；verify-gates.json:75-85（unit/acceptance timing=full）。
**根因**：final 相位的「全量」是包数少时代的正确默认——彼时全量成本低；「只跑本包档不算完成」防的是**缩到只剩本包档**的偷懒，而非禁止一切圈定；影响面圈定的技术手段（detect_changes blast radius + taskpackage 影响面字段）如今已在仓内可用（T1 种子 + integrator.md:53 既有 detect_changes 位）。
**改法**：integrator.md Step 4 改为两步——① **范围圈定**：跑 detect_changes（把 :53 Step 11b 的 advisory 位前移升格为范围判定依据），结合本包 impactedTests 种子（T1 产出）生成 **test-scope 清单**落盘（TaskPackage 目录 sibling 文件）；② 按清单跑 verify `--phase=final --scope-file <清单>`（T9 消费）。范围构成 = 本包 acceptanceTestsFile + blast radius 映射到的测试 + 共享 seam 相关测试 + 安全余量。**修订逐字铁律**：「只跑本包档不算完成」整句替换为「**范围 = 影响面圈定清单（detect_changes + impactedTests 种子合成）；缺清单、图不可用或圈定失败 → 升级 human 举手，不得默认全量、也不得缩到只剩本包档**」。Rationalizations 表 :111 行同步替换为「圈定失败先举手，不是先全量」；「各包都绿了，全量可以省」的反驳语义改为指回清单。Step 11b 的 advisory 记账职责保留（范围判定 ≠ 影响面核对，两不误）。
目标文件：`.claude/agents/integrator.md`（改）

### T9 · 测试执行闸全仓枚举，不消费 scope 清单

**问题**：unit/acceptance 两闸执行器硬编码全仓枚举（collectFiles），没有按清单筛选的入口；integrator 想圈范围也无机械通道，T8 的清单只能停在纸面。
**证据**：unit-run-check.mjs:31/44（`FW_UNIT_RE` / `collectFiles` 定义）；acceptance-run-check.mjs:33/39（`PY_ACC_RE` / `collectFiles`）、:140-142（fwAccFiles/pyAccFiles 全仓收集）、:238（`python -m pytest -q --import-mode=importlib -m "not live" ${files.join(' ')}`）；verify.mjs:82（「--phase=final 时跑全部；否则只跑 timing=scoped 的闸」）、:96（未知 --phase fail-loud 惯例）。
**根因**：run-check 脚本设计时无圈定需求，「全量」被写死在枚举层而非配置层。
**改法**：三个脚本加 `--scope-file <path>`：① unit-run-check.mjs / acceptance-run-check.mjs——scope-file 在场时，collectFiles 结果与之求交（相对路径归一后过滤），交集空时如实报「scope 内 0 条可跑」红（防假绿，语义仿 acceptance-run-check.mjs:230 的 live 全 deselected 处置）；scope-file 不在场且 phase=final → **红闸举手**（报错指引升级 human，不退全量）；scoped 相位不消费（维持现状，功能闸本就只在 final 跑）。② verify.mjs——透传 `--scope-file` 给 timing=full 闸的 child 进程（verify.mjs:275 gate-child spawn 处）；无 scope-file 的 final 相位在闸层红（同 fail-closed）。`-m "not live"` 条目级筛除逻辑原样保留（scope-file 只做文件级求交，不做 live 豁免）。全量回归 = 人工触发：scope-file 显式传全仓清单文件，或未来由人手工跑 `--scope-file` 之外的显式全量入口——本批不新增 --all 旗标（留根 §6 记观察项）。
目标文件：`.claude/scripts/unit-run-check.mjs`（改）· `.claude/scripts/acceptance-run-check.mjs`（改）· `.claude/hooks/verify.mjs`（改）

### T10 · timing 语义与 merge-driver quick-check 未随范围收敛更新

**问题**：verify-gates.json `_doc` 的 timing 语义仍写「full = 仅终审 merge-readiness 跑一次（全量功能套件）」，与 T8/T9 的新语义冲突；merge-driver quick-check 每次合并全仓跑 acceptance 冒烟，与「全量仅人工触发」的 human 裁决方向不一致（是否收敛见 OQ-1）。
**证据**：verify-gates.json:14-17（timing 字段定义原文「scoped = 每轮 verifier 都跑（结构闸 + 受影响子集功能闸）。full = 仅终审 merge-readiness 跑一次（全量功能套件）。」）、:26-27（quick-check 冒烟子集定义）；merge-driver.mjs:132-136（ARQ-100 detect_changes advisory，「detect_changes 永不阻塞 merge、永不虚报影响面」）。
**根因**：timing 语义成文于圈定机制存在之前；quick-check 子集是 260905 对齐产物，彼时「acceptance 全仓冒烟」尚可承受。
**改法**：verify-gates.json `_doc` timing 段更新为：「full = 终审 merge-readiness 按 scope-file 圈定跑（scope-file 缺失红闸举手）；全量功能套件 = 人工触发（如发版前），不在任何自动相位」+ scope-file 语义一句；:26-27 quick-check 条目按 **OQ-1 已裁 (a)** 更新。merge-driver.mjs：按 (a) 落地——QUICK_CHECK_IDS 的 acceptance 门执行透传 scope-file（按合并 diff 的 blast radius 圈定，与 integrator final 相位同规），图/detect_changes 不可用时 fail-closed 举手；detect_changes 的 advisory 非阻塞语义（:136）原样保留。起草期已核 merge-to-main skill 文内无 quick-check / 全量 acceptance 语义表述（grep 2026-09-28 零命中），该文件不入本批 manifest，语义同步义务记留根 §6。
目标文件：`.claude/verify-gates.json`（改）· `.claude/scripts/merge-driver.mjs`（改）

### T11 · AGENTS.md dev-loop 链描述未含前置评估位

**问题**：AGENTS.md Hard Rules 的 dev-loop 链描述「实现动作走 dev-loop（planner Pod 派 acceptance-author → builder → verifier↔fixer → integrator → supervisor）」不含 architect 前置评估位；AGENTS.md 是全会话路由根，链描述与 planner.md 实际流程漂移会误导后续会话。
**证据**：AGENTS.md「Hard Rules」节逐字「主 AI 不直改自有实现面做正式工作——实现动作走 dev-loop（planner Pod 派 acceptance-author → builder → verifier↔fixer → integrator → supervisor）」。
**根因**：链描述成文时前置评估位不存在。
**改法**：该句链描述扩为「planner Pod 派 architect（前置技术预评）→ acceptance-author → builder → verifier↔fixer → integrator → supervisor」；一句话改动，不加新节（Instruction Budget 纪律）。
目标文件：`AGENTS.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 消费方 |
|---|---|---|---|
| dev-loop 流程语义 | `.claude/agents/planner.md` | G3 | planner Pod 全链（/build-feature 驱动） |
| dev-loop 流程语义 | `.claude/agents/architect.md` | G3 | planner 1c 档派发 / human 显式 |
| dev-loop 流程语义 | `.claude/agents/builder.md` · `.claude/agents/fixer.md` | G3 | Pod 内实现角色 |
| dev-loop 流程语义 | `.claude/agents/integrator.md` | G3 | integrator 收拢相位 |
| dev-loop 流程语义 | `.claude/references/dev-loop.md` · `.claude/references/task-intake-and-slicing.md` | G3 | dev loop 全体角色（程序 SSOT） |
| 机检执行面 | `.claude/scripts/change-manifest-check.mjs`（新增） | G2（新闸） | verify.mjs 按 verify-gates.json 派发 |
| 机检执行面 | `.claude/scripts/taskpackage-required-check.mjs` · `.claude/scripts/unit-run-check.mjs` · `.claude/scripts/acceptance-run-check.mjs` | G2 | verify.mjs |
| 机检执行面 | `.claude/hooks/verify.mjs` | G2 | verifier 相位（planner 直跑）+ merge-driver quick-check |
| 机检执行面 | `.claude/verify-gates.json` | G2 | verify.mjs / merge-driver.mjs 单源 |
| 机检执行面 | `.claude/scripts/merge-driver.mjs` | G2 | 合并流（merge-to-main） |
| 会话路由根 | `AGENTS.md` | G3 | 全会话（经 CLAUDE.md @导入传导） |

**【硬项 · sweep 透镜义务】**本批落地新门 change-manifest（判定 pattern = changeManifest glob 对 git diff 文件集的全覆盖断言）。全仓预跑：当前仓内活跃 TaskPackage（`delivery/taskpackages/` 下）**均无** `changeManifest` 字段——本批以 changeId 日期前缀 cutoff（T7）豁免存量，故不产生存量红；机检落地后以 `--self-check` fixture 实跑两态（越界红 / 清单内绿），输出进 §5 验证集。新断言（scope-file 求交）对既有执行器的侵入面：仅 final 相位 + 传参在场时生效，scoped 相位与 quick-check（视 OQ-1）之外的行为路径逐字节不变——自查方式 = T9 改法所述条件分支 + §5 回归自检（verify.mjs --self-check 全绿含既有自检名）。

**【硬项 · 复扫模式完备性先验】**本批不涉退役语面 / 残留清扫：新增字段与新增旗标均为全新标识符（changeManifest / change-manifest / scope-file / --scope-file），无既有同义词形需要清扫；「全量」语义修订为**改写既有句**（integrator.md:81/111、verify-gates.json:14-17），替换目标文本已在各 T 改法中原文钉死，不留旧语义残留句——落地时逐句 grep「必须全量验证」「全量功能套件」「只跑本包档」核清零（§5 验证集含此断言）。

## 4. human 取舍点（OQ）

**OQ-1 · merge-driver quick-check 的 acceptance 门是否一并接 scope-file**
- (a) **接（推荐）**——quick-check 的 acceptance 门按合并 diff 的 blast radius 圈定跑，图/detect_changes 不可用时 fail-closed 举手；与「全量仅人工触发」的裁决方向一致，合并频率 = 每包一次，随包数增长的痛点与 integration 同源。
- (b) 不接——quick-check 维持全仓 acceptance 冒烟不动，作为本地 main 前最后一道固定防线；代价是每包一次合并仍付全仓 pytest 成本。
- 推荐理由：全量防线已由「人工触发位（发版前）」承接，quick-check 维持全仓会把收敛效果折损一半。
- **human 裁定（2026-09-28）：(a) 接。** T10 按 (a) 落地。

**OQ-2 · change-manifest 闸挂载相位**
- (a) **scoped 每轮跑（推荐）**——builder 期越界早停早扩单，扩单成本最低；每轮多一次 glob 对账（毫秒级）。
- (b) 仅 final 跑——verifier 轮次更轻，但越界改动平均晚发现一整轮 builder/fixer 周期。
- 推荐理由：对账为纯文件集运算无外部依赖，早停收益远大于毫秒级开销；本 PROPOSAL T6 按 (a) 起草，OQ-2 若裁 (b) 则 FROZEN 前调整挂载位。
- **human 裁定（2026-09-28）：(a) scoped 每轮。** T6 挂载位定案。

## 5. 验证集（landing 前机械前置 + landing 后核验）

1. **PROPOSAL 形态门**（pre-commit）：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260928-gov-review-dev-loop-preassess-manifest-scope/PROPOSAL.md` → 形态零错；`anchorUnion` 成员名单与 manifest 围栏块 16 行逐 token 核对一致（注记无夹带路径记号）。
2. **INDEX 登记**（pre-commit）：`node .claude/scripts/changes-index.mjs --upsert 260928-gov-review-dev-loop-preassess-manifest-scope` → arch lane INDEX 出现本 changeId 单行。
3. **verify 自检回归**（post-commit）：`node .claude/hooks/verify.mjs --self-check` → 既有自检名全绿 + 新增 scope-file 相关自检名在场（phase-final-missing-scope-file-reds 类）。
4. **change-manifest 两态 fixture**（post-commit）：`node .claude/scripts/change-manifest-check.mjs --self-check` → 内置 fixture 两态断言全绿（越界 diff → 红；清单内 diff → 绿；manifest 缺失 → 红）。
5. **旧语义残留清零**（post-commit）：`grep -rn "只跑本包档不算完成" .claude/` 与 `grep -rn "必须全量验证" .claude/` → 仅命中 Rationalizations 表的新反驳句（指回清单），原句 0 残留；`grep -n "全量功能套件" .claude/verify-gates.json` → 仅命中新 timing 语义句。
6. **quick-check 行为保持**（post-commit，若 OQ-1 裁 (b)）：`node .claude/scripts/merge-driver.mjs --self-check`（若该入口不存在则以 dry-run fixture 替代，落地期亲核后择一钉死）→ quick-check 门集与 OQ-1 裁断一致。

## 6. 留根清单

1. **圈定召回质量无自动兜底**：全量回归改为人工触发后，scope 圈定若漏召回只能靠实机/线上暴露——首三个实包 integration 记录「圈定清单 vs 事后 detect_changes 全 blast radius」差集，交 loop-retro cadence 观察；若连续漏召回，回补「batch 末位全量」或扩安全余量规则。
2. **全量人工触发位无入口与 checklist**：本批不新增 `--all` 旗标，全量目前 = 手工传全仓 scope 清单；「发版前全量」未挂任何 checklist/脚本，登记 ARQ 候选（留根不硬治）。
3. **① 直接修 / human 跳流程例外不经 manifest**：changeManifest 只约束 TaskPackage 流；human 明确说跳过流程的一次性操作按既有铁律（诚实记录「跳过流程」）覆盖，不新增拦截。
4. **fixer.md 同批覆盖未立 OQ**：fixer 与 builder 同为实现面角色，边界条款一并落地；若评审认为 fixer 期扩单应有更轻通道，升级判官裁。
5. **② 单包 Lite 档的 1c 档成本**：小 bug 包也走 architect 前置评估会增加一次派发成本；比例化裁剪（Lite 档是否免评或简化评）交评审循环裁，倾向保留但允许判官按比例化原则收窄。
6. **merge-to-main skill 语义同步义务**：起草期已核该 skill 文内无 quick-check / 全量 acceptance 表述（grep 2026-09-28 零命中），本批不触；若该 skill 后续增写测试范围语义，需带上「全量 = 人工触发位」新语义——登记不硬治。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
