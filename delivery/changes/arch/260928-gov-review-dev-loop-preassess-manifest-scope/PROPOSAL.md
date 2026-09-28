# PROPOSAL · 260928-gov-review-dev-loop-preassess-manifest-scope

> **状态：DRAFT v0.3 · 2026-09-28 · R2 BLOCK（R2-F1/R2-F2 条文级小修）修订完成，待 R3 复核**
> **human 凭据**：立项 ack = plan mode 批准（2026-09-28）；OQ-1 裁 (a) 接 scope-file、OQ-2 裁 (a) scoped 每轮（2026-09-28，会话内拍板）。
> **承载 ARQ**：无根——human 2026-09-28 直接立项（立项 ack = plan mode 批准：architect 前置技术评估 + changeManifest + 测试范围收敛三合一）。不关任何既有 ARQ 根。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T12）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
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
.claude/commands/build-feature.md
.claude/hooks/verify.mjs
.claude/references/dev-loop-operating-system.md
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
2. **integration 每次全量跑测试，随包数线性变贵**。unit / acceptance 两功能闸 timing=full（verify-gates.json:75-85），执行器全仓枚举零筛选——unit-run-check.mjs:31/44（`FW_UNIT_RE` + `collectFiles`）与 acceptance-run-check.mjs:33/39/140-142（`PY_ACC_RE` + `collectFiles`）各自枚举 firmware/ 与 server/ 全部测试文件后整批执行（acceptance-run-check.mjs:238 `python -m pytest ... ${files.join(' ')}`）；唯一筛除是 `-m "not live"` 条目级标记。冲突消解复跑清单硬保证逐字「只跑本包档不算完成」（integrator.md:81，位于「Merge-Chain Execution · 消解完成后」节——R1-B6 语境勘误，原 v0.1 误定性为 final 相位铁律），Rationalizations 表逐字「集成风险只在收拢后暴露，必须全量验证」（integrator.md:111）。merge-driver quick-check 每次合并另跑一次全仓 acceptance 冒烟（verify-gates.json:26-27「merge-driver quick-check 冒烟子集 = contract-lint + acceptance 两门」）。taskpackage 已有 `acceptanceTestsFile[]` 等影响面记账字段（dev-loop.md:128），但没有任何闸消费它们缩测试范围；detect_changes 在 integrator.md:53（Step 11b）与 merge-driver.mjs:132-136（ARQ-100 advisory，「detect_changes 永不阻塞 merge、永不虚报影响面」）都只有 advisory 记账位。
3. **builder/fixer 改动文件无预定范围约束**。Track B 有 landing manifest（packet-bound 落地清单 + landing-fidelity-check 机检），dev-loop 侧没有对偶：builder 的边界只有包维度（builder.md:12「你在单个包边界内实现 TaskPackage」、:49「Boundaries / 权限边界」节），包内改哪些文件没有预定清单，也无机检对账。

**非目标（本批不做）**：不触 `docs/contracts/**`；不改验收测试纪律（host/live 两态、acceptance-author 职责）；不动契约门 / landing-verifier 独立核验 / 升级闸三类不动件；不改 portfolio-planner 拆包与调度逻辑；不引入新的 runner / 测试框架。

---

## 2. 问题、根因与改法（T1…T12）

**【硬项 · 起草期实证义务】**本节的断言纪律：凡 file:line 引据均起草期现核（上节三条差距内的引据已于 2026-09-28 逐条以 Grep 定点复核，复核输出在本会话记录在案）；凡声称「对偶 / 比照先例」已读先例文件（Track B landing manifest 机制见本 PROPOSAL §0 第 3 条差距的对照）。凡「全 / 均 / 无」量化声明（「全仓枚举零筛选」「唯一出场位」「没有任何闸消费」）覆盖集合 = unit-run-check.mjs / acceptance-run-check.mjs / verify.mjs / merge-driver.mjs / verify-gates.json 五件，复核时点 2026-09-28。

### T1 · planner 缺「前置技术评估」环节

**问题**：TaskPackage 进 loop 后直接切片派发，技术影响评估与技术方案确定没有系统环节；方案质量完全押后到 acceptance 断言与 10b 救火档，builder 起步时可能方向已偏。
**证据**：planner.md:79（10b 档原文「dev loop 进行中疑难问题可派 architect……」，触发闭集救火型、「未命中闭集不派」）；planner.md:44/49（architect 类派发仅图探索注记与契约预审）；task-intake-and-slicing.md:266（260928 OQ-1 收窄后包内实现决策无 plan.md 承载位）。
**根因**：dev-loop 设计时把 architect 定位在 loop 前（/triage-change 路由快审，change-triage.md:38「快审：确认判据 + 路由裁决」）与 loop 中（reactive advisory）两端，中间的「单包技术方案评估」真空——triage 快审不做深评，10b 只事后救火。
**改法**：planner.md 新增 **Step 1g「ARCH-ADVISORY 技术预评档」**——编号取 1g 是因 1c–1f 槽位已被占用（判官 R1-B1 实查 planner.md:45-48：1c = Intake checklist、1d = crossEnd、1e = 需求权威、1f = 快照保鲜），取 1g 零重编号代价、不断既有 1d 交叉引用（planner.md Step 7/8 与 dev-loop.md:135）。**派发时点钉死 = Step 5 Draft TaskPackage 落盘后、Step 7 brief 生成前**：TaskPackage 已成文故产出可回填，brief 的封闭输入面由此含 changeManifest 语义。对每个 Draft 完成的 TaskPackage，planner 派 architect 做一次前置技术评估（只读、advisory 效力声明与 10b 档同款），产出三件：① 技术方案意见；② changeManifest（预定改动文件/目录清单，T4）；③ impactedTests 种子（T8 消费）。planner 将①补写进 plan.md 技术方案节（T3），②③回填 TaskPackage。**比例化简版（判官 S3 裁定建议，采纳）**：② 单包 Lite / bug 止血包允许简版产出——changeManifest 可目录级粗 glob、技术方案意见可一段、impactedTests 种子可极简；① 直接修不经 loop 天然豁免。契约疑点仍走 Step 2 的 architect-reviewer，不因 1g 档存在而跳过。1g 与既有 10b 救火档边界：1g = planning 期前置、必经（Lite 简版也是评）；10b = 执行期 reactive、触发闭集不放开。插位文本锚（planner.md:44 原文之后、1c 条目前不插，1g 追加在既有 1f 条目之后）：「1f. 快照保鲜（planner.md:48，逐字以落地时实读为准）。」
目标文件：`.claude/agents/planner.md`（改）

### T2 · architect 无前置评估职责位

**问题**：architect.md 的 dev-loop 职责只有「疑难问题 advisory 分析」（救火型），没有前置评估档位；planner 依 T1 派发时 architect 定义文件无对应 when-to-use 与产出物规范，三件产出（技术方案 / changeManifest / impactedTests 种子）无归属。
**证据**：architect.md:31（原文「**dev loop 中的疑难问题 advisory 分析**：planner 按 `[ARCH-ADVISORY 疑难分析档]` 派你，或 human 显式指定。……**只出意见、不落地、不实现、不改任何文件**」）；architect.md:3（description 枚举同款）。
**根因**：与 T1 同源——前置评估档从未被定义，architect 的 advisory 语义只在救火场景成文。
**改法**：architect.md 新增「**dev loop 前置技术评估**」职责段（与 :31 救火段并列，成对成文）：planner 按 **1g 档**派发时，评估技术影响、确定技术方案方向，输出固定三件（技术方案意见 / changeManifest / impactedTests 种子）；**比例化简版**对 Lite / bug 止血包适用（粗 glob + 一段意见 + 极简种子）；声明**扩单通道**——builder/fixer 实现中发现 changeManifest 范围不够时，经 planner 带根因假设回评，architect 评估后扩 manifest（扩单留痕：清单 append + 评估依据一句）；advisory 效力声明与 :31 同款照抄。description 行同步追加该档短语。
目标文件：`.claude/agents/architect.md`（改）

### T3 · plan.md 技术方案承载位缺口

**问题**：260928 OQ-1 收窄后，包内技术方案（改法取舍、接线决策、影响面）没有任何 plan.md 承载位；T1 产出的技术方案意见无处落盘，跨会话不可递、不可核。
**证据**：task-intake-and-slicing.md:266（原文「**判据一句钉死：跨包事实 / 共享冻结面约束……留拆包侧；单包内实现决策（改法取舍、阶段划分、时序细节、渲染接线、包内边角）不进拆包侧 plan.md**——包内细节设计承载位 = 冻结 acceptance 断言化 + TaskPackage `notes`/`exitCriteria`/`decisionCoverage` 备注 + planner 派发 prompt（260928 批 OQ-1 裁定）」）；dev-loop.md:129（②单包 plan.md 条目，planner「**只读消费、改需留痕**」）；change-triage.md:30（②单包 plan.md 内容框架仅一句「feature 写“建什么+方案”，bug 写“根因假设+修复+影响面”」）。
**根因**：260928 收窄裁定的动机是防拆包侧 plan.md 重复 planner 派发 prompt 的包内细节；但该裁定把「经 architect 评估的技术方案」一并逐出，与 human 本次立项要求（技术方案写在 plan.md 里）冲突——需要**有意识修订**而非默认沿用。
**改法**：task-intake-and-slicing.md:266 判据句后追加例外条款：「**例外（260928-gov-review-dev-loop-preassess-manifest-scope）**：plan.md 的「技术方案（planner+architect 预评）」节承载 architect 前置评估产出——改法取舍 / 影响面 / 关键接线决策；该节由 planner 在收到 1g 档评估后补写，**须留痕（日期 + architect 档标识）**；包内其余细节承载位判定照旧。」dev-loop.md:129 条目同步扩注 F8 语义：「只读消费、改需留痕」**加注**：技术方案节为唯一例外——planner 可在 architect 前置评估后补写，留痕规则同上；②单包已有 triage plan.md 则追加该节，③拆包由 planner 建包侧 plan.md 时写入。**与 closeout-check UPSTREAM_DRIFT 的交互（判官 S4，采纳）**：③拆包 plan.md 的该节 planner 补写属 F8 留痕合法改，closeout-check 的 UPSTREAM_DRIFT warn（convention-only）命中时以留痕行解释、不构成漂移——口径写入 dev-loop.md 该条目注。
目标文件：`.claude/references/task-intake-and-slicing.md`（改）· `.claude/references/dev-loop.md`（改）

### T4 · TaskPackage 无改动范围字段（changeManifest）

**问题**：TaskPackage schema 只有影响面记账字段（`acceptanceTestsFile[]`、`subsystem` 等，dev-loop.md:128），没有「本包预定改动哪些实现文件」的清单字段；T2 的 manifest 产出无处回填，T6 的机检无输入。
**证据**：dev-loop.md:126-128（标准内容字段枚举，逐字「`taskpackage.json`：sourceType、sourceRefs、deliveryImpact、planningImpact、riskTier、gateProfile、story、acceptance、exitCriteria、budget、contractRefs、docRefs」）；dev-loop.md:170（「`taskpackage.json` 可按风险声明附加字段」——schema 扩展的既有惯例位）。
**根因**：dev-loop 从未定义包内文件级改动边界——builder 边界只到包维度，文件维度一直靠自觉。
**改法**：dev-loop.md TaskPackage schema 节新增 `changeManifest[]` 字段：文件/目录 glob 清单（仓根相对），由 architect 前置评估（1g 档）产出、planner 回填；**扩单唯一通道 = architect 评估**（builder/fixer → planner → architect 回评，扩单 append + 评估依据留痕）；字段完备性由 T7 齿机检，内容对账由 T6 闸机检。挂靠位：dev-loop.md:128 标准内容清单追加字段名 + :170 附加字段节写消费细则（谁产出、谁消费、扩单通道、留痕规则）。
目标文件：`.claude/references/dev-loop.md`（改）

### T5 · builder/fixer 无文件级改动边界约束

**问题**：builder 与 fixer 在包边界内可改任意实现文件，改动越出 architect 预估的合理范围时既无停手机制也无扩单流程；Track B 的 packet-bound 纪律在 dev-loop 侧没有对偶。
**证据**：builder.md:12（「你在单个包边界内实现 TaskPackage」）、builder.md:49（「## Boundaries / 权限边界」节，无文件级条款）；fixer.md:50-51（边界仅「不改 canonical docs/spec」「不改 verifier gate、TaskPackage 事实或验收标准」，无文件级条款）。
**根因**：manifest 字段与 architect 前置评估此前不存在，两个 agent 的边界条款自然没有可引用的对象。
**改法**：builder.md「Boundaries / 权限边界」节新增条款：只改 `changeManifest` 范围内的文件；实现中发现需要改清单外文件 → **停手**，带根因假设交回 planner 派 architect 扩单（T2 通道），**不许先改后补、不许自行扩清单**。fixer.md 边界节同步同款条款（fixer 与 builder 同为实现面角色，越界风险同源，本批一并覆盖，不另立 OQ）。Rationalizations 表各加一行（“就改一个文件很快，先改了再说。” → 越界改动绕过了 architect 影响面评估，走扩单通道才是受控路径）。**顺手修正（判官 S5，采纳）**：builder.md:51 / fixer.md:49 既有边界节中的旧仓路径 token `apps/sX-*` 改为本仓实况表述（包边界 = TaskPackage subsystem 面，文件写权以 scope-map 为准）——新条款不复制旧 token。
目标文件：`.claude/agents/builder.md`（改）· `.claude/agents/fixer.md`（改）

### T6 · changeManifest 无机检对账闸

**问题**：T4/T5 的 manifest 纪律若无机检，退化为软约束——与「机械强制层不动摇」的既有治理取向相悖；越界 diff 会在无感知下溜进 commit。
**证据**：本仓机械闸惯例先例：taskpackage-required-check.mjs（TaskPackage 上下文在场机检，verify-gates.json:32-35 timing=scoped）与 landing-fidelity-check.mjs（Track B landing 清单对账）；verify-gates.json 闸挂载格式（:30-86）。
**根因**：manifest 是新概念，对账闸从未存在。
**改法**：新增 `.claude/scripts/change-manifest-check.mjs`：读 TaskPackage `changeManifest[]`（glob 展开），对 git diff（工作树 + 已暂存）涉及文件逐个对账——**对账面（判官 R1-B2 钉死）** = 自有实现面（`firmware/**` + `server/**`，以 scope-map 实现面定义为准）∪ TaskPackage 显式声明的 docs-sync 面；治理 / 交付 / 机械簿记面（`delivery/**`、`.claude/**`、`docs/**` 等执行 ledger 与 fs-write 制品）**不在对账面**——各自由既有纪律管，scoped 每轮不因 pod-events / verifier-evidence / brief / taskpackage.json 本体 / test-scope sibling 等簿记写入常态红。清单外实现面文件即红（fail-closed）；manifest 缺失/为空/不可解析即红，**但与 T7 共享同一 cutoff 谓词**（同一 changeId 日期前缀 cutoff 常量，pre-cutoff 在途包两闸同豁免）。两场景钉死：① 活跃 TaskPackage worktree 内的「human 跳流程例外」改动 → 红 = 逼扩单/留痕（红信息指引补 architect 扩单或落留痕记录，不静默豁免）；② Pod worktree 内 TaskPackage 定位沿用 taskpackage-required-check.mjs 既有定位机制（`delivery/taskpackages/current/` 投影优先）。提供 `--self-check` 自检入口（仿 taskpackage-required-check.mjs 惯例）。挂进 verify-gates.json 为新闸 `change-manifest`（kind=governance、timing=scoped，位置排在 taskpackage-required 之后）——每轮 verifier 都跑（OQ-2 已裁 (a)），builder 期越界早停早扩单。Phase 语义：scoped 相位遇红即停（成文位 verify-gates.json `_doc`:3「scoped 相位遇到第一个失败即停并产出 ErrorEnvelope」；实现位 verify.mjs:250-258——R1-N1 引据勘误，原引 verify.mjs:3 有误）。
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
**改法**：integrator.md **拆两语境改（判官 R1-B6 勘误承接）**——① 常态 final 相位（Step 4，含 :38「全量 bearing verification」句本体）改为两步：**范围圈定**（跑 detect_changes，把 :53 Step 11b 的 advisory 位前移升格为范围判定依据）+ 结合本包 impactedTests 种子（T1 产出）生成 **test-scope 清单**落盘（TaskPackage 目录 sibling 文件），再按清单跑 verify `--phase=final --scope-file <清单>`（T9 消费）。范围构成 = 本包 acceptanceTestsFile + blast radius 映射到的测试 + 共享 seam 相关测试 + 安全余量。**语义铁律（Step 4 落点新增）**：「范围 = 影响面圈定清单（detect_changes + impactedTests 种子合成）；缺清单、图不可用或圈定失败 → 升级 human 举手，不得默认全量、也不得缩到只剩本包档」。**「图不可用」三分法定案（R2-F2 消解，与 :53 N/A 降级路径的边界钉死）**：① Pod worktree 内 per-Pod 专属图索引可用（launch 面必办项，repo-structure.md 既有治理）→ 正常圈定，:53 的「N/A (worktree)」降级路径**收窄删除**；② 索引不可用但 trace_path 抽查可用 → 降级圈定（git diff --stat + 关键共享符号 trace_path 抽查）**算合法圈定**，铁律例外注 + 降级如实留痕；③ detect_changes 与 trace_path 均不可用 → 真不可用，举手升级 human。② 冲突消解复跑清单（:79-81 Merge-Chain Execution 节）**不整句替换**：保留「双方冻结件（本包 + 对端包）必跑」为圈定 scope **下限**（floor：圈定清单 ∪ 双方冻结件——消解副作用只在对端冻结齿上显形，是实测陷阱，硬保证不弱化），「只跑本包档不算完成」句保留并加 floor 语义注。③ :111 Rationalizations 行照 v0.1 计划替换（反驳语义指回清单）；description/:3/:8/:12/:39/:118/:125 等语义级「全量」残留随本 T 同批改新语义（R1-B7 清单 integrator 面；:39 为 R2-F1 补录「在全量 verify/e2e 绿后」句）。④ **顺手修陈旧锚（判官 R1-B4 附带）**：:53「索引 checkout（C:/AI/AInativecompany）」改为本仓图索引口径（主 worktree = `C-AI-KidPal`，Pod worktree = 按路径派生条目），N/A (worktree) 降级路径按上述三分法收窄。⑤ **留根 1 可执行化（判官 S1，采纳）**：test-scope 清单落盘路径一行记入 final verifier fact / review-packet（results 元数据必含行），留根 1 的「圈定 vs 事后全 blast radius」差集观察由此有锚可查。Step 11b 的 advisory 记账职责保留（范围判定 ≠ 影响面核对，两不误）。
目标文件：`.claude/agents/integrator.md`（改）

### T9 · 测试执行闸全仓枚举，不消费 scope 清单

**问题**：unit/acceptance 两闸执行器硬编码全仓枚举（collectFiles），没有按清单筛选的入口；integrator 想圈范围也无机械通道，T8 的清单只能停在纸面。
**证据**：unit-run-check.mjs:31/44（`FW_UNIT_RE` / `collectFiles` 定义）；acceptance-run-check.mjs:33/39（`PY_ACC_RE` / `collectFiles`）、:140-142（fwAccFiles/pyAccFiles 全仓收集）、:238（`python -m pytest -q --import-mode=importlib -m "not live" ${files.join(' ')}`）；verify.mjs:82（「--phase=final 时跑全部；否则只跑 timing=scoped 的闸」）、:96（未知 --phase fail-loud 惯例）。
**根因**：run-check 脚本设计时无圈定需求，「全量」被写死在枚举层而非配置层。
**改法**：三个脚本加 `--scope-file <path>`：① unit-run-check.mjs / acceptance-run-check.mjs——scope-file 在场时，collectFiles 结果与之求交（相对路径归一后过滤）。**交集空判据（判官 R1-B3 勘误承接）**：交集空 = **诚实绿跳过**（报告注明「scope 内 0 条可跑」及原因，对偶既有 state4 no-files 绿与 allLiveDeselected 绿两先例——:230 的 live 全 deselected 处置本就是绿不是红，v0.1 引据反向有误）；**scope-file 在场但其引用的文件在盘上全不存在 = 红**（防 typo 假绿）——两判据分开写。scope-file 不在场且 phase=final → **红闸举手**（报错指引升级 human，不退全量）；scoped 相位不消费（维持现状，功能闸本就只在 final 跑）。② verify.mjs——透传 `--scope-file` 给 timing=full 闸的 child 进程（verify.mjs:275 gate-child spawn 处）；无 scope-file 的 final 相位在闸层红（同 fail-closed）。`-m "not live"` 条目级筛除逻辑原样保留（scope-file 只做文件级求交，不做 live 豁免）。全量回归 = 人工触发：本批不新增 --all 旗标（留根 §6 记观察项）。
目标文件：`.claude/scripts/unit-run-check.mjs`（改）· `.claude/scripts/acceptance-run-check.mjs`（改）· `.claude/hooks/verify.mjs`（改）

### T10 · timing 语义与 merge-driver quick-check 未随范围收敛更新

**问题**：verify-gates.json `_doc` 的 timing 语义仍写「full = 仅终审 merge-readiness 跑一次（全量功能套件）」，与 T8/T9 的新语义冲突；merge-driver quick-check 每次合并全仓跑 acceptance 冒烟，与「全量仅人工触发」的 human 裁决方向不一致（是否收敛见 OQ-1）。
**证据**：verify-gates.json:14-17（timing 字段定义原文「scoped = 每轮 verifier 都跑（结构闸 + 受影响子集功能闸）。full = 仅终审 merge-readiness 跑一次（全量功能套件）。」）、:26-27（quick-check 冒烟子集定义）；merge-driver.mjs:132-136（ARQ-100 detect_changes advisory，「detect_changes 永不阻塞 merge、永不虚报影响面」）。
**根因**：timing 语义成文于圈定机制存在之前；quick-check 子集是 260905 对齐产物，彼时「acceptance 全仓冒烟」尚可承受。
**改法**：verify-gates.json `_doc` timing 段更新为：「full = 终审 merge-readiness 按 scope-file 圈定跑（scope-file 缺失红闸举手）；全量功能套件 = 人工触发（如发版前），不在任何自动相位」+ scope-file 语义一句；:26-27 quick-check 条目按 **OQ-1 已裁 (a)** 更新。merge-driver.mjs 四点（OQ-1=(a) 落地）：① **QUICK_CHECK_IDS 的 acceptance 门执行透传 scope-file**（按合并 diff 的 blast radius 圈定，与 integrator final 相位同规）；② **改锚（判官 R1-B4 承重项）**：:138 `DETECT_CHANGES_PROJECT = 'C-AI-Ainativecompany'` 为旧仓索引锚，advisory 时代错锚仅 warn 无害，(a) 落地后每次合并会误判「图不可用」fail-closed 举手、收敛整体失效——同批改为本仓 `C-AI-KidPal`（主 worktree 合并流 = 常量；Pod worktree = 按路径派生条目动态解析——与 T8④ 同一口径，落地期钉死一种并给自检断言，R2-F2 附带对齐）；③ **fail-closed 分面（判官 R1-B5 消解）**：ARQ-100「detect_changes 永不阻塞 merge、永不虚报影响面」（:132-136）**收窄为 retro/影响面记账消费面**（F1 catch 墙照旧），quick-check 范围判定消费面允许 fail-closed 举手——两消费面在 :132-136 注释与自检断言中显式分写，不再并存矛盾；④ **F4 oracle 保持（判官 S2，采纳）**：scope-file 参数在 runQuickCheck 调用点追加，不动 buildQuickChecks cmd 表——EXPECTED_QUICK_CHECK_CMDS 字面 oracle（merge-driver.mjs:418-422）与 runQuickChecksSelfCheck 不破。起草期已核 merge-to-main skill 文内无 quick-check / 全量 acceptance 语义表述（grep 2026-09-28 零命中），该文件不入本批 manifest，语义同步义务记留根 §6。
目标文件：`.claude/verify-gates.json`（改）· `.claude/scripts/merge-driver.mjs`（改）

### T11 · AGENTS.md dev-loop 链描述未含前置评估位

**问题**：AGENTS.md Hard Rules 的 dev-loop 链描述「实现动作走 dev-loop（planner Pod 派 acceptance-author → builder → verifier↔fixer → integrator → supervisor）」不含 architect 前置评估位；AGENTS.md 是全会话路由根，链描述与 planner.md 实际流程漂移会误导后续会话。
**证据**：AGENTS.md「Hard Rules」节逐字「主 AI 不直改自有实现面做正式工作——实现动作走 dev-loop（planner Pod 派 acceptance-author → builder → verifier↔fixer → integrator → supervisor）」。
**根因**：链描述成文时前置评估位不存在。
**改法**：该句链描述扩为「planner Pod 派 architect（前置技术预评）→ acceptance-author → builder → verifier↔fixer → integrator → supervisor」；一句话改动，不加新节（Instruction Budget 纪律）。
目标文件：`AGENTS.md`（改）

### T12 · 「全量」语义残留面超 §3 先验声明（判官 R1-B7 打回项独立成 T）

**问题**：v0.1 §3 复扫先验称「不涉残留清扫」，判官实查语义级「全量」残留至少散布 14 处，含 manifest 外文件与 G1 面（docs/DEV-GOVERNANCE.md）——v0.1 声明与实况不符。
**证据**：判官 R1-B7 实查清单——integrator.md:3/:8/:12/:38/:118/:125（已并入 T8 ④同批改）；planner.md:84（Step 11「docs-sync、全量验证」）；dev-loop.md:60/:132；dev-loop-operating-system.md:38/:55（「终审 full（全量 monorepo 功能套件一次）」，该文件 v0.1 不在 manifest）；build-feature.md:30（不在 manifest）；DEV-GOVERNANCE.md:47/:137（G1 面）；merge-driver.mjs:408 注释。
**根因**：起草期复扫先验只按「整句替换目标」枚举，未按语义族全仓 grep——模式集不完备。
**改法**：**扩 manifest 同批清 `.claude` 面残留**（判官给的二选一处置中选此项，DEV-GOVERNANCE 除外）：planner.md:84、planner.md:65/:70（R2-F1 补录「首次全量复验」两处，改「final 复验（scope 圈定）」）、dev-loop.md:60/:132/:174（:174 为 R2-F1 补录「在全量 verify/e2e 绿后」）、dev-loop-operating-system.md:38/:55、build-feature.md:30（R2-F1 勘误：同行含「全量 e2e」与「全量 verify/e2e」**两实例**，落地按 §3 全形词表逐形清、防只改一词过 grep）、merge-driver.mjs:408 注释——逐处改为新语义（scope 圈定 / 人工触发位口径），逐处处置留痕进 landing-summary；`docs/DEV-GOVERNANCE.md:47/:137` **豁免 + 留根**（G1 面不入本批 profile——review profile 保持 G1=零，该文档为治理叙述文档非程序 SSOT，留根 §6-7 登记下批 docs 修订顺带）。§5 验证集 item 5 grep 词表同步扩。
目标文件：`.claude/agents/planner.md`（改）· `.claude/references/dev-loop.md`（改）· `.claude/references/dev-loop-operating-system.md`（改）· `.claude/commands/build-feature.md`（改）· `.claude/scripts/merge-driver.mjs`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 消费方 |
|---|---|---|---|
| dev-loop 流程语义 | `.claude/agents/planner.md` | G3 | planner Pod 全链（/build-feature 驱动） |
| dev-loop 流程语义 | `.claude/agents/architect.md` | G3 | planner 1g 档派发 / human 显式 |
| dev-loop 流程语义 | `.claude/agents/builder.md` · `.claude/agents/fixer.md` | G3 | Pod 内实现角色 |
| dev-loop 流程语义 | `.claude/agents/integrator.md` | G3 | integrator 收拢相位 |
| dev-loop 流程语义 | `.claude/references/dev-loop.md` · `.claude/references/task-intake-and-slicing.md` · `.claude/references/dev-loop-operating-system.md` | G3 | dev loop 全体角色（程序 SSOT） |
| dev-loop 流程语义 | `.claude/commands/build-feature.md` | G3 | /build-feature 入口（T12 残留清） |
| 机检执行面 | `.claude/scripts/change-manifest-check.mjs`（新增） | G2（新闸） | verify.mjs 按 verify-gates.json 派发 |
| 机检执行面 | `.claude/scripts/taskpackage-required-check.mjs` · `.claude/scripts/unit-run-check.mjs` · `.claude/scripts/acceptance-run-check.mjs` | G2 | verify.mjs |
| 机检执行面 | `.claude/hooks/verify.mjs` | G2 | verifier 相位（planner 直跑）+ merge-driver quick-check |
| 机检执行面 | `.claude/verify-gates.json` | G2 | verify.mjs / merge-driver.mjs 单源 |
| 机检执行面 | `.claude/scripts/merge-driver.mjs` | G2 | 合并流（merge-to-main） |
| 会话路由根 | `AGENTS.md` | G3 | 全会话（经 CLAUDE.md @导入传导） |

**【硬项 · sweep 透镜义务】**本批落地新门 change-manifest（判定 pattern = changeManifest glob 对 git diff 文件集的全覆盖断言）。全仓预跑：当前仓内活跃 TaskPackage（`delivery/taskpackages/` 下）**均无** `changeManifest` 字段——本批以 changeId 日期前缀 cutoff（T7）豁免存量，故不产生存量红；机检落地后以 `--self-check` fixture 实跑两态（越界红 / 清单内绿），输出进 §5 验证集。新断言（scope-file 求交）对既有执行器的侵入面：仅 final 相位 + 传参在场时生效，scoped 相位与 quick-check（视 OQ-1）之外的行为路径逐字节不变——自查方式 = T9 改法所述条件分支 + §5 回归自检（verify.mjs --self-check 全绿含既有自检名）。

**【硬项 · 复扫模式完备性先验】**（R1-B7 修订版 + R2-F1 补形）本批涉「全量」语义残留清扫，模式集显式枚举全形态：**「全量验证」「全量 e2e」「全量 bearing」「全量 monorepo」「全量端到端」「全量 verify」「全量闸门」「全量闸」「全量门套」「全量复验」「只跑本包档」「全量功能套件」**（词形 × 有/无 backtick × 同行多实例（build-feature.md:30 实证）× 标题/正文/description/注释位）。处置：`.claude` 面内命中 = T8 ③（integrator.md 七处）+ T12（planner/dev-loop/dev-loop-operating-system/build-feature/merge-driver.mjs 注释）同批改新语义；`docs/DEV-GOVERNANCE.md:47/:137` = 豁免留根（G1 面不入 profile，§6-7）。落地时逐词 grep 核：`.claude` 面内仅新语义句命中、DEV-GOVERNANCE 命中 = 预期豁免（§5 item 5 含此断言）。新增标识符（changeManifest / change-manifest / scope-file）为全新词形，无退役清扫面。

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

1. **PROPOSAL 形态门**（pre-commit）：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260928-gov-review-dev-loop-preassess-manifest-scope/PROPOSAL.md` → 形态零错；`anchorUnion` 成员名单与 manifest 围栏块逐 token 核对一致（**17 个路径 token**，R1-N2 口径改按 token 数钉死；注记无夹带路径记号）。
2. **INDEX 登记**（pre-commit）：`node .claude/scripts/changes-index.mjs --upsert 260928-gov-review-dev-loop-preassess-manifest-scope` → arch lane INDEX 出现本 changeId 单行。
3. **verify 自检回归**（post-commit）：`node .claude/hooks/verify.mjs --self-check` → 既有自检名全绿 + 新增 scope-file 相关自检名在场（phase-final-missing-scope-file-reds 类）。
4. **change-manifest 两态 fixture**（post-commit）：`node .claude/scripts/change-manifest-check.mjs --self-check` → 内置 fixture 两态断言全绿（越界 diff → 红；清单内 diff → 绿；manifest 缺失 → 红）。
5. **旧语义残留清零**（post-commit）：`grep -rn "只跑本包档不算完成" .claude/` 与 `grep -rn "必须全量验证" .claude/` 与 `grep -rnE "全量 e2e|全量 bearing|全量 monorepo|全量验证|全量端到端|全量 verify|全量闸门|全量闸|全量门套|全量复验" .claude/`（R1-B7 词表 + R2-F1 补四形）→ `.claude` 面内仅命中新语义句（scope 圈定 / 人工触发位口径；「只跑本包档不算完成」仅保留于冲突消解 floor 注），原句 0 残留、同行多实例逐一清（build-feature.md:30 型）；`grep -n "全量功能套件" .claude/verify-gates.json` → 仅命中新 timing 语义句。
6. **quick-check 行为**（post-commit，OQ-1 已裁 (a)）：`node .claude/scripts/merge-driver.mjs --self-check`（若该入口不存在则以 dry-run fixture 替代，落地期亲核后择一钉死）→ scope-file 透传生效 + F4 oracle（EXPECTED_QUICK_CHECK_CMDS）自检不破 + DETECT_CHANGES_PROJECT 锚断言命中本仓 `C-AI-KidPal`（R1-B4 自检义务）。

## 6. 留根清单

1. **圈定召回质量无自动兜底**：全量回归改为人工触发后，scope 圈定若漏召回只能靠实机/线上暴露——首三个实包 integration 记录「圈定清单 vs 事后 detect_changes 全 blast radius」差集，交 loop-retro cadence 观察；若连续漏召回，回补「batch 末位全量」或扩安全余量规则。
2. **全量人工触发位无入口与 checklist**：本批不新增 `--all` 旗标，全量目前 = 手工传全仓 scope 清单；「发版前全量」未挂任何 checklist/脚本，登记 ARQ 候选（留根不硬治）。
3. **① 直接修 / human 跳流程例外不经 manifest**：changeManifest 只约束 TaskPackage 流；human 明确说跳过流程的一次性操作按既有铁律（诚实记录「跳过流程」）覆盖，不新增拦截。
4. **fixer.md 同批覆盖未立 OQ**：fixer 与 builder 同为实现面角色，边界条款一并落地；若评审认为 fixer 期扩单应有更轻通道，升级判官裁。
5. **② 单包 Lite 档的 1g 档成本**：已按判官 R1-S3 裁定裁**简版**（粗 glob + 一段意见 + 极简种子，T1/T2 条文已载），不全免；简版产出质量交首三个实包 retro 观察。
6. **merge-to-main skill 语义同步义务**：起草期已核该 skill 文内无 quick-check / 全量 acceptance 表述（grep 2026-09-28 零命中），本批不触；若该 skill 后续增写测试范围语义，需带上「全量 = 人工触发位」新语义——登记不硬治。
7. **docs/DEV-GOVERNANCE.md「全量」语义豁免**（R1-B7 处置）：:47/:137 语义级残留不入本批（G1 面保持 profile 外），登记下批 docs 修订顺带清；本批 §5 item 5 的 grep 断言对其预期豁免命中。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-28 - architect
verdict: BLOCK(B1–B7)（方向与骨架成立：三件咬合制品 + scoped 红闸 + fail-closed 圈定的设计意图清楚，OQ-2=(a) 承接忠实；七条打回项集中在机检语义漏洞、编号/相位错位与残留清扫声明不实，均为可闭环修，无方向性推翻）

评审范围：PROPOSAL 全文 + 引据抽验 18 处（agents / references / scripts / hooks / json 各面覆盖，逐字核对）+ 漏改面 grep（「全量」语义全仓扫描 + merge-runner 锚实查）。引据抽验结果：16 处逐字命中（planner.md:44/:49/:79、architect.md:3/:31、task-intake-and-slicing.md:266、builder.md:12/:49、fixer.md:49-51、integrator.md:38/:53/:111、dev-loop.md:128/:129/:170、verify-gates.json:14-17/:26-27/:32-35/:75-85、unit-run-check.mjs:31/:44、acceptance-run-check.mjs:33/:39/:140-142/:238、verify.mjs:82/:96/:275、merge-driver.mjs:132-136、change-triage.md:30/:38、taskpackage-required-check.mjs:129 AC_BACKREF_CUTOFF='2026-09-28'、AGENTS.md Hard Rules 链句）；2 处错位见 N1 与 B6。写入面纪律核验：本批零触 `docs/contracts/**`、零动三类不动件（契约门 / landing-verifier / 升级闸均不在 manifest）——合格；`.claude/agents/architect.md` 属 self-def 面，human 立项 ack（plan mode 批准）覆盖、T2 不动 tools/name/model/write scope，边界内。

**B1 [打回项] T1 新档编号撞号 + 派发时序错位**。planner.md 的 1c/1d/1e/1f 四个槽位已全部占用（1c = Intake checklist C5，planner.md:45；1d = crossEnd，:46；1e = 需求权威，:47；1f = 快照保鲜，:48）——「Step 1b 之后新增 Step 1c」要么撞号、要么全链重编号（planner.md Step 7/8 的「Step 1d `crossEnd`」引用与 dev-loop.md:135 同引会断，两文件虽均在 manifest，但 PROPOSAL 未列重编号义务）。且插入位（1b 后、Step 2 契约预审前）早于 TaskPackage 构造点（Step 5 Draft / Step 8 finalize），与 T1 自身「②③回填 TaskPackage」「对每个 intake 完成的 TaskPackage」的时序矛盾——1c 产出 changeManifest/impactedTests 种子的输入对象在该位置尚不存在。改法：新档改号（建议 **1g**，零重编号代价），派发时点明钉 = **Step 5 Draft TaskPackage 落盘后、Step 7 brief 生成前**（brief 的封闭输入面应含 changeManifest 语义，T5 条文与 brief-inputs 的关系同批写明或留根）。

**B2 [打回项] T6 对账闸的 diff 面未定义，scoped 每轮常态红**。change-manifest 闸「对 git diff（工作树 + 已暂存）涉及文件逐个对账——清单外文件即红」：scoped 每轮跑时，工作树 diff 恒含执行 ledger 与机械 fs-write 制品（pod-events.jsonl 追加、verifier-evidence/rounds、verdict.json/current 投影、input-brief、taskpackage.json 本体、T8 的 test-scope sibling 文件），changeManifest 若只列实现文件（T4 语义 = builder/fixer 改动边界）则这些簿记写入全部落「清单外」→ 常态红。必须定义 diff 过滤面或豁免集（建议：对账面限定自有实现面 `firmware/**`+`server/**` + TaskPackage 显式声明的 docs-sync 面；ledger/机械产物面枚举豁免，豁免集写进 T6 条文）。存量兼容同理只定义在 T7（cutoff），T6 自身「manifest 缺失/为空即红」对 pre-cutoff 在途包无豁免——两闸 cutoff 必须共享同一谓词。另两场景须写明：①「human 跳过流程例外」（留根 3 只说「不新增拦截」）在**活跃 TaskPackage worktree 内**的跳流程改动会撞红闸，行为应是「红 = 逼扩单/留痕」还是豁免，钉一句；② Pod worktree 内 TaskPackage 路径解析（current/ 投影 vs change-id 直读）沿用哪条既有惯例，钉一句。

**B3 [打回项] T9「交集空时红」自相矛盾且制造假红**。T9 写「交集空时如实报『scope 内 0 条可跑』红（防假绿，语义仿 acceptance-run-check.mjs:230 的 live 全 deselected 处置）」——但 :230 的 allLiveDeselected 处置是**绿**（executor-skipped 诚实跳过，非红），所引先例与所述判据相反；且合法交集空态真实存在（纯固件包 scope 内 0 条 server unit 测试 / 纯 server 包 scope 内 0 条固件验收），红会把正常 final 相位逼成升级噪音、直接折损本批目标效果。改法：交集空 → 诚实报告 + 绿跳过（对偶既有 state4 no-files 绿 与 allLiveDeselected 绿两先例）；「scope-file 在场但其引用的文件在盘上全不存在」才红（防 typo 假绿）——两判据分开写。

**B4 [打回项] T10/OQ-1=(a) 的承重依赖 detect_changes 项目锚指向旧仓，未核**。merge-driver.mjs:138 `DETECT_CHANGES_PROJECT = 'C-AI-Ainativecompany'`（root `C:/AI/AInativecompany`，本日 list_projects 实查在册）——非本仓索引 `C-AI-KidPal`（AGENTS.md CodebaseMemory 表权威）。advisory 时代错锚只 warn N/A 无害；OQ-1=(a) 把 quick-check acceptance 门的范围圈定压在「合并 diff 的 blast radius」上后，错锚 = 每次合并落「图不可用 fail-closed 举手」，收敛目标整体失效。T10 必须同批改锚（主 worktree 合并流用 `C-AI-KidPal`，或按 Pod worktree 条目动态解析）并给自检断言。附带同款陈旧锚：integrator.md:53「索引 checkout（C:/AI/AInativecompany）」——本批 integrator.md 已在 manifest，宜顺手改或明文留根。

**B5 [打回项] T10 fail-closed 与 ARQ-100「detect_changes 永不阻塞 merge」冻结语义矛盾未消解**。T10 一句写「图/detect_changes 不可用时 fail-closed 举手」，另一句写「detect_changes 的 advisory 非阻塞语义（:136）原样保留」——在 quick-check 消费面上互斥（不可用即举手 = 阻塞 merge 的一种形态）。需显式拆面：advisory 非阻塞语义**收窄为 retro/影响面记账消费面**（merge-driver.mjs:132-136 注释 + F1 catch 墙照旧），quick-check 范围判定消费面允许 fail-closed（相应修订 :132-136 注释文字与其自检断言）；或定义 quick-check 降级路径（如降级全仓冒烟）。三选一钉死，不得两句并存。

**B6 [打回项] T8 对 integrator.md:81 的语境错置——该句在冲突消解复跑清单节，非 final 相位铁律**。:81 逐字命中，但它位于「Merge-Chain Execution · 消解完成后（集成树复跑清单）」节（integrator.md:79-81），管**冲突消解后**的复跑口径（「双方冻结件（本包 + 对端包）」必跑），§0 把它定性为「final 相位铁律」与 T8 整句替换的落点选择均基于错语境：替换后「对端包冻结件必跑」的冲突消解硬保证被弱化为 blast radius 圈定（scope 清单不保证含对端冻结件——消解副作用恰是对端红漏检的实测陷阱，:81 原文自述）。改法：拆开两语境——常态 final 相位（Step 4）按新语义改；冲突消解复跑清单保留「双方冻结件必跑」为 scope **下限**（floor：圈定清单 ∪ 双方冻结件），或明文论证为何可并入圈定。

**B7 [打回项] §3/§5「全量」残留清扫声明不实——残留面远超所列三句，且含 G1 面**。复扫先验称「不留旧语义残留句」，但语义级「全量」残留实查至少还有：integrator.md:3（description「跑全量端到端」）/:8（标题「全量 e2e」）/:12（Role「全量验证」）/:38（Step 4「全量 bearing verification」——T8 改法只提 :81/:111，Step 4 本体句未列）/:118（Red Flag「全量 verify 或 e2e 红」）/:125（Verification「全量闸门结果」）、planner.md:84（Step 11「docs-sync、全量验证」）、dev-loop.md:60/:132、dev-loop-operating-system.md:38/:55（「终审 full（全量 monorepo 功能套件一次）」——该文件不在 manifest）、build-feature.md:30（不在 manifest）、DEV-GOVERNANCE.md:47/:137（**G1 面**，manifest 未含且 review profile 自声明 G1=零）、merge-driver.mjs:408 注释。处置二选一钉死：扩 manifest 同批清（DEV-GOVERNANCE 入 manifest = profile 增面，须回改 Status 头 review profile 行）；或逐条列残留 + 每条一句豁免理由 + 留根登记下批。§5 验证集 item 5 的 grep 词表同步扩（至少加「全量 e2e」「全量验证」「全量 bearing」「全量 monorepo」）。当前「清零」声明与实况不符，不得带病 FROZEN。

**S1 [建议] final 证据不锚 scope-file 路径，留根 1 的召回质量观察无锚可查**。留根 1 要记「圈定清单 vs 事后 detect_changes 全 blast radius」差集，但 T8/T9 未要求 final fact 或 review-packet 引用 scope-file 落盘路径。建议：T8 的 test-scope 清单路径记入 final verifier fact 的 results 元数据或 review-packet 必含行（一行成本，换留根 1 可执行）。

**S2 [建议] T10 透传须保 merge-driver F4 字面 oracle 不破**。quick-check 的 scope-file 参数须在 runQuickCheck 调用点追加，不动 buildQuickChecks 的 cmd 表——否则 EXPECTED_QUICK_CHECK_CMDS 字面 oracle（merge-driver.mjs:418-422）与 runQuickChecksSelfCheck 自检红。T10 条文写明「调用点追加、F4 oracle 不动」。

**S3 [建议·比例化，承接留根 5] ② Lite / bug 止血包裁简版而非全免**。1c 档对 Lite 包保留但允许简版产出：changeManifest 可目录级粗 glob、技术方案意见可一段、impactedTests 种子可极简——写进 T1/T2 条文一句。①直接修不经 loop 天然豁免（留根 3 已覆盖），维持。此为判官按比例化原则的裁定建议，主线程可采纳或升级 human。

**S4 [建议] T3 未提与 closeout-check UPSTREAM_DRIFT（convention-only/warn）的交互**。③拆包 plan.md 由 planner 追加技术方案节会触发 drift 检测 warn；T3 应写明「该节追加属登记例外、warn 命中按留痕解释」或与 closeout-check 口径对齐，防落地期误判。

**S5 [建议] T5 所改边界节仍是旧仓 `apps/sX-*` 路径 token**（builder.md:51、fixer.md:49，本仓实况 = `firmware/**`+`server/**`，真实写权由 scope-map 管）。T5 既改同节，宜顺手修正或至少不把旧 token 复制进新条款；存量漂移若不在本批治，留根一句。

**N1 [nit] T6 引据 verify.mjs:3 错位**。scoped fail-fast 的成文位 = verify-gates.json `_doc`:3（「scoped 相位遇到第一个失败即停」），实现位 = verify.mjs:250-258（final 遇红不停 / scoped exit(1)）；verify.mjs:3 是 legacy verdict.json 注释。改引据两处。

**N2 [nit] §5 item 1「manifest 围栏块 16 行」实为 15 行**（15 个路径 token，逐 token 数过）。核对口径按 token 数钉死，防 landing 期对不上。

R1 结论：**BLOCK**。打回项 B1–B7 闭环（改 PROPOSAL 对应 T 条文与 §3/§5 声明）后可进 R2 复核；B4/B5 涉 quick-check 承重语义，R2 将重验锚点与 fail-closed 分面文字。建议 S1–S5 采纳成本均低，随打回项一并处理可省一轮。

### R2 - 2026-09-28 - architect
verdict: BLOCK(R2-F1, R2-F2)（B1–B7 + S1–S5 + N1/N2 闭环承接逐条核验**全部到位**，修订未引入新设计矛盾；两条新打回项均为 B7 闭环的残余缺口与 B5 同型的并存语义未消解，改动量小、R3 可绿）

复核范围（R2 纪律 = 前轮 findings 修订到位性 + 修订引入新矛盾，不做无的放矢重核）：v0.2 全文通读 + B1–B7 逐条对核 + preflight 独立实跑（`landing-fidelity-check.mjs --preflight` 零 findings、anchorUnion 17 token 与围栏块逐 token 一致）+ 残留词形复核 grep。闭环确认：B1→T1（1g 改号含占用实据、时点钉死 Step 5 后/Step 7 前、1g vs 10b 边界「planning 期必经 / 执行期闭集」二分清楚、S3 简版入 T1/T2）✓；B2→T6（对账面 = 实现面 ∪ 声明 docs-sync 面、簿记面显式出局、双闸共享 cutoff 谓词、跳流程例外 = 红逼留痕、worktree 定位沿 current/ 惯例）✓；B3→T9（交集空 = 诚实绿跳过 / scope-file 引用全不存在 = 红，两判据分开且勘误 v0.1 引据反向）✓；B4→T10 ②（改锚 C-AI-KidPal + 自检断言义务，§5-6 含锚断言；integrator:53 顺手修入 T8 ④）✓；B5→T10 ③（两消费面显式分写，F1 catch 墙归记账面）✓；B6→T8（两语境拆开：Step 4 新铁律 / :79-81 保留双包冻结件为 scope floor 不整句替换；§0 定性句勘误）✓；B7→T12 独立成 T + manifest 扩 17（build-feature.md、dev-loop-operating-system.md 入册）+ DEV-GOVERNANCE 豁免留根 §6-7 + §3 先验重写 + §5 词表扩——**闭环不全，见 R2-F1**。S1→T8 ⑤ ✓；S2→T10 ④（调用点追加、F4 oracle 不动）✓；S4→T3 ✓；S5→T5（apps/sX-* 顺手修，写权归 scope-map 表述正确）✓；N1→T6 引据改 _doc:3 + verify.mjs:250-258 ✓；N2→§5-1 改 17 token ✓。OQ 承接复核：OQ-1=(a) → T10 四点忠实；OQ-2=(a) → T6 scoped 每轮 + 排序不变，忠实。写入面纪律复验：manifest 17 文件零触 docs/contracts/**、零动三类不动件；新增两面（commands/build-feature.md、references/dev-loop-operating-system.md）均在 G2/G3 profile 内，review profile「G1=零」与 DEV-GOVERNANCE 豁免自洽 ✓。

**R2-F1 [打回项 · B7 闭环残余] §3 模式集「全形态」声明仍漏四个在册词形，行清单漏三处实例、一处有带病过 grep 的具体失败模式**。v0.2 §3 先验枚举七词形（全量验证 / 全量 e2e / 全量 bearing / 全量 monorepo / 全量端到端 / 只跑本包档 / 全量功能套件），但 R2 复核 grep 实证还有四形在册且不在词表：**「全量 verify」「全量闸（门）」「全量门套」「全量复验」**。后果三件：① 行清单漏列——integrator.md:39（Step 5「在全量 verify/e2e 绿后」）、dev-loop.md:174（s2Exploratory 条「在全量 verify/e2e 绿后」）两处不在 T8 ③ / T12 任何行清单；planner.md:65/:70（MODEL-TIER「首次全量复验」两处）全 PROPOSAL 无处置——本批后 final 是 scope 圈定跑，「全量复验」措辞成陈迹；② grep 盲区有具体失败模式——build-feature.md:30 同行含「全量 e2e」与「全量 verify/e2e」两实例，§5 item 5 词表只命中前者，落地若只改前者即带病过 grep 断言；③ §3 硬项「模式集显式枚举全形态」声明与实况不符——与 R1-B7 同病，不得带病 FROZEN。改法（三处小修）：§3 模式集与 §5 item 5 词表补「全量 verify｜全量闸｜全量门套｜全量复验」四形；T8 ③ 行清单补 integrator.md:39、T12 行清单补 dev-loop.md:174；planner.md:65/:70 二选一处置（改「final 复验（scope 圈定）」口径，或明文豁免一句 + 理由）。

**R2-F2 [打回项 · B5 同型] T8 新铁律「图不可用 → 举手」与 integrator.md:53 既有「Pod worktree 内 diff 对索引不可见 → N/A (worktree) + trace_path 替代」降级路径的关系未消解**。:53 的 N/A 降级是既有成文纪律（Pod worktree 内 detect_changes 对索引不可见时，以 git diff --stat + trace_path 抽查替代）；T8 ① 把 detect_changes 升格为范围判定依据并立铁律「缺清单、图不可用或圈定失败 → 升级 human 举手」，T8 ④ 只修 :53 的 checkout 路径锚、不动 N/A 语义——两句并存即 B5 同型矛盾：若 N/A (worktree) 属「图不可用」则 Pod 内 final 相位常态举手（本批目标失效）；若 N/A 的 trace_path 替代算合法圈定则铁律须写明该例外；若锚修正后 per-Pod 索引（`C-AI-KidPal-pod-*` 条目实存、watched 自动刷新）令 Pod 内 detect_changes 可用，则 N/A 路径应收窄或删除。三选一钉死并写进 T8 ①/④，不留两句并存。附带核对：T10 ② 改锚只写主 worktree 合并流用 `C-AI-KidPal`，Pod worktree 侧（若有 detect_changes 消费）用 per-Pod 条目——与 T8 ④ 的「按路径派生条目」口径对齐一句即可。

R2 结论：**BLOCK**。仅 R2-F1 / R2-F2 两条，均为条文级小修（词表 + 行清单 + 一句语义消解），无结构返工；闭环后 R3 直接收口。其余全部核验通过——若主线程愿将两条修订并入 v0.3 且无新引入矛盾，R3 预期 GREEN。
