# Decision Packet · 260908-gov-review-merge-chain-hardening

> adjudicator 产出 · 2026-09-08 · 交 human 拍板。governance profile：本 packet 是**本次 confirmed 治理改法的 delta 记录**，human 批后由 architect 机械回灌 frozen PROPOSAL（不夹带新设计），再由主线程 packet-bound landing。本 packet 不落 canonical。

## 一、裁定项逐项裁断与归档位置

### A-3 · T1-4 修复口径（Step 1b 同形双源）——建议即地板，human 确认

**事实层（adjudicator 独立亲核，机判先结）**：同形成立。`approvalRefUnlock`（merge-driver.mjs:918-921）= `probeWt = findWorktree(tp, projectDir) || projectDir` 作 cwd + gateArgs 传 `--project-dir projectDir`；常规路径 Step 1b（:1136-1142）= `wt = findWorktree(pod, projectDir)`、`if (!wt) errorExit(...)` 无回退 + 同款 `--project-dir projectDir` 参数 + `cwd: wt`。两路径均为「git 在 cwd（worktree）求值 source-ref、文件读在 --project-dir（主仓）求值」；唯一差异 = 豁免路径回退分支（worktree 解析失败时 cwd 静默落主仓 → `--source-ref HEAD` 在 main 求值 → off-main BLOCK 恒空转，方向 fail-closed 无损）。ARQ 并证二②「常规路径无此形状」对照系失真确认。回退分支触发面真实（blocked Pod 恰可能缺 worktreePath）。

**设计层裁断建议：维持两形、只改豁免路径回退分支**——把 `findWorktree(tp, projectDir) || projectDir` 改为 findWorktree 失败即显式 errorExit（与 Step 1b :1126-1128 同款），不统一改两路径传参。理由：① 现状两路径的形状是有意语义且一致（architect 建议判据：source-ref 必须在 Pod worktree 求值、verdict/PROPOSAL 读主仓——现状两路径均已如此）；② 缺口仅回退分支静默落主仓，改显式 errorExit 消恒空转且 fail-closed 方向不变（canary 10 求值主体零变）；③ 两路径同改会引入无增益的行为面变更。**这是起点，可砍更大**（如 human 判 worktreePath 缺失的 Pod 应有其它恢复路径而非 errorExit，可加恢复指引）。回填点：PROPOSAL :64 按 F5 清理自违候选 + V1-③ 按 F6 条件化改写 + T1-4 补自检断言「回退分支消除后 worktree 解析失败显式 errorExit」。归档：decision-packet 本节 + human 拍板清单 #1。

### A-1 · self-def 存疑点①（integrator.md 新节判 lightweight 硬排除与否）——判据先例归档，human 确认

按 DEV-GOVERNANCE §6 自改递归硬条款与 protocol「Eligibility 判据」机检硬排除清单逐项核：integrator.md 新节 frontmatter（tools/name/model/description）零改动、write scope 零变更（scope-map 不改，canary 12）、authority 语义零转移（architect-notes §T3 候选正文逐字「永不 push」「永不 merge 进 main」「main 侧合并恒归 planner 主线程」与既有 :12/:74 语义叠合唯一赢家）。**裁断：不触 lightweight 硬排除判据的字面**——「agent def 执行语义变更」的硬排除实质指 tools/name/model/write scope/authority 语义变更，本节均不命中。**但本节属 agent def 执行面语义扩展**（把「不 merge」默认禁止显式豁出一角并加前置），判据先例归档为：agent def 正文新节若同时满足 ① frontmatter 零改动、② write scope 零变更、③ 权限语义为显式豁出且带可机判的 fail-closed 前置（本节三前置：逐文件预登记 / eventId 可锚 / 分支亲核——前置不满足即停，非会话自由裁量 prose）、④ authority 零转移，则不自动触发 HUMAN_REQUIRED 硬排除，**恒须 human-approved governance change（approver = human 不可降档）**；任一前置不满足的同类申报，存疑向严默认 ESCALATE。本批已走 Track A full + human，合规。归档：decision-packet 本节 + human 拍板清单 #2（agent def 改动必须 human）。

### A-2 · 存疑点②（T5 归档定性）——agent 层归档，随批改法吸收

**裁断：T5 闸内新核验行 = 既有前置升级闸的「门内加项」**（对偶 DEV-GOVERNANCE §5「closeout 硬化 = verifier 闸族门内加项」折进注范式），非机检硬排除意义上的「新增 merge gate」、非第 7 工程门。依据：T5 只在既有 evaluate 内新增核验行，不删不改既有六条例外条目（canary 5），六工程门不变式（charter §7.5「新门裁决必须折进既有行 + 折进注留痕」）以此形态满足；折进注留痕随本批 landing 落 DEV-GOVERNANCE §5 注（该文件不在本批 manifest——折进注由 DEV-GOVERNANCE 后续批顺带核引，本批先在 PROPOSAL T5 条款内如实声明 gate 事实）。T1 四项行为扩展同理已如实声明交判（PROPOSAL :111 sweep 行）；T4 ask 是 settings 权限层条目非新门。归档：decision-packet 本节（agent 层已够）。

### A-4 · OQ3 严格档（T4 形态三选一）+ GA-8 ask 张力 + N3——human 拍板

**裁断建议：采纳 defender R2 调整后立场「条件保留 ask」，首选先修 runbook 命令形态互洽。** 具体建议包：

1. **形态 = (a) ask**：`permissions.ask` 补 `Bash(git merge:*)` / `Bash(git rebase:*)` 两形。排除 (b) 条件 allow——settings 层不可表达（architect §T4 事实 2：唯一通道 = 新 PreToolUse hook = 事实上的新闸，另批 full）；排除 (c) deny + runbook——拦死 runbook 合法消解路径，与 P1 病根同形倒退。
2. **互洽前提（N3 裁断吸收）**：先修 runbook 命令形态——§10a 增一步一次性设置（或 merge-driver 建 worktree 时代设）Pod worktree 本地 `git config core.editor true`，第 3 步改素形 `git rebase --continue`（以 `git rebase` 起头 → ask 前缀命中；注意 `git -c core.editor=true` 形态不命中前缀、不解决问题）。修后消解主循环每轮真弹窗 = GA-8「真正判断点」论证的自洽形态（人出现在冲突逐个消解的当下，而非仪式性在场）。修法落 T2 §10a 补条范围内，manifest 已含 pod-coordination.md。
3. **降级预案（摩擦实测不可接受时，留根不本批）**：收窄 ask 至 `Bash(git merge:*)` 一条 + rebase 面靠 T3 正文纪律 + airtight hook 留根（诚实降档，另批走 full——新 hook 等价新设闸）。
4. **不推荐**：维持现状 + 留根——runbook 自带逃逸（:126）使 ask 保护主张 day 1 名不副实，违反本批自己的诚实边界纪律。
5. **回填义务（A-4 裁断后批内落地，缺一不齐）**：① 两条诚实边界回填 PROPOSAL T4 正文 + 留根清单（条件语义承载在 T3 正文 / airtight 需 hook 留根另批——不得只落 landing-summary）；② 主张限定句「ask = 手动 Bash 路径 best-effort；复合命令、env 前缀、child_process 三类绕道在册」；③ N6——`dev-loop.settings.json` `_doc` 数组（:3-15）加一行「ask 弹窗批准判据：核 human 授权 eventId 在场且可锚到 pod-events 对应事件」；④ B-7——hook 留根行内并入 hook↔lib 耦合声明一句。归档：human 拍板清单 #3（强制层 settings + GA-8 张力）。

### A-5 · T7 规约落点三选一——human 确认

**复判采纳 architect 荐：`.claude/references/acceptance-evidence.md`「必要证据」段（:5-11）末尾 + 同文件「反例」节（:31-39）挂「硬写版本字面」反例。** 复判理由：① 消费者对位——撰写约定唯一消费者 = acceptance-author 写验收断言的当下，acceptance-evidence.md 已是同类撰写约定集合，同构同位；② task-intake-and-slicing.md 是 planner 拆包 reference，acceptance-author 非主消费者，断言格式权威落在撰写者读不到的文件 = 漂移源；③ DEV-GOVERNANCE.md 是 G1 指令根——裁它落点则触面成本升级且 profile 须重报 moderator（GA-1 陷阱），G2 reference 足额承载，避开。**被排除两落点不需要指回引用**：两者从未承载该约定、无既有指向往回改，单一权威 = acceptance-evidence.md，无 B-0 式漂移源。回填点：PROPOSAL T7 目标文件行落点钉死 + manifest 补该路径（与 F3 的 ARCH-REVIEW-QUEUE 行一并）。归档：human 拍板清单 #4。

### A-6 · 字面耦合检查是否设门——human 拍板（闸判据域）

**裁断建议：本批不设新门**——与 GA-4 / A-2 结论一致（六工程门不变式，本批不另立枚举、不新增机检判据）。architect 候选采纳：**告警级 lint 夹具先行留根**（「字面版本名出现在新接线类 green-guard 断言里 → warn」，lint-descriptions.mjs RC-2 告警级先例），gate 化（判红）留根另裁另批。**随裁钉死标注格式（F12 吸收）**：T7「撰写时注释标注类别」钉最小固定 token 形态 `<!-- guard-class: wiring | landing | wording -->`（正则可判、人类可读、不与 prose 冲突）——即便 lint 留根另批，格式仍本批先钉（撰写当下即用，防自由散文防线）。**VRT-E4 误伤防线联动**：S-4 类别判据（接线 vs 落盘/措辞二分）在条款正文内可操作化 = guard-class 注释即类别判据，lint 化时以该 token 为信号而非「同文件出现 .vN 字面」的粗粒度规则。归档：human 拍板清单 #5。

### A-7 · §10c 登记表与正则双源关系——agent 层归档 + 整包批确认

**裁断：登记表为 authority + `*.v[0-9]+.md` 形态正则为兜底。** 不一致时赢家 = 登记表（文档侧注册表锚点，pod-coordination.md §10c）；正则角色 = merge-driver 特判命中集的形态兜底，登记表外新形态（无 `.vN` 后缀的版本化物）退回通用 blocked fail-closed——GA-7 补集不变式（特判只 widen 指引文案、不 widen 放行）以此成立。发现通道双轨：① V1-① 反例夹具「engine-patch 形态 add/add → 通用 blocked」把不变式从声明变机证；② B-2 修向的 diff 触发（批版本化制品路径集与登记表 diff 非空 = 更新触发）。**登记表初始清单 = 实测超集**（F9 修向）：`server/deploy/openclaw-system-prompt.vN.md` + engine-patch/ + openclaw-patch/ + OPENCLAW_SYSTEM_PROMPT_CARRIER（无扩展名）+ provider.config.yaml + apply-xiaozhi-overlay.sh 五类逐条入册。此裁断同时满足 protocol「机械面枚举的锚点纪律」——枚举指向注册表锚点（§10c），正则不反向硬编码名册。归档：decision-packet 本节 + PROPOSAL T2-1 回填（随整包批确认）。

### A-8 · T5 未识别形态方向 + B-4 写作侧对齐（同场裁）——human 拍板（闸判据域）

**裁断建议：放行（只拦可识别形态）为默认方向。** 理由：① T5 匹配对象是 review-packet Findings 自由 prose（integrator.md:42 六列约定，无 schema）；「未匹配即红」把写作措辞变化全部变成假红，闸信噪比崩塌（对偶教训：ARQ-260906-lfc-verbatim-crlf-citation-gap 字节级严格匹配 → advisory 信噪比走低、每批人工定性）；② fail-closed 不变量不受损——该闸的 fail-closed 语义 = **可识别违规必拦**，未识别形态放行的后果是「漏账不被机械拦」，与批前零核验同水平、不恶化；放行不产生任何假绿。**B-4 写作侧对齐约定（同场裁，补偿控制，R2 A-2 设定的 packet 完整性义务在此履约）**：① T5 落地时在 gate 核验行注释钉 Basis 匹配形态（`docs/contracts/**` / `docs/spec.md` 字面前缀串匹配、固定串比对）+ V2 两态夹具各一（正例：有 CDQ 行 / defer 锚放行；负例：hard-violation + Basis 引契约面 + 无锚 → 红）；② 写作侧同批对齐——integrator.md:42 Findings 写作约定补一句「Kind=hard-violation 且 Basis 涉契约/spec 时，Basis 须以 `docs/contracts/**` 或 `docs/spec.md` 字面前缀起引」（manifest 已含 integrator.md，零新增文件）；③ B-6 defer/accepted-risk eventId 锚格式 = pod-events `human-decision` 事件、payload.decision ∈ {defer-contract-change, accept-risk}（枚举值随 human 本项批一并确认），核验复用 resolveHumanApproval 五要素、decision 值参数化。归档：human 拍板清单 #6。

### N5 · T6 解锁核验集三选项排序——human 确认（合并机械链 gate 事实）

**裁断：选项 2（收窄 + Step 1b 背书补偿断言 + T6 申报句）**。裁决依据（adjudicator 独立亲核）：approvalRefUnlock 调用点唯一在 merge-driver 入口 :1368-1370（`SINGLE_POD && HUMAN_APPROVAL_REF && !DRY_RUN` 守卫），解锁成功置 ready-to-merge 后，同 invocation 内 ready 集收集 → processPod（:1117）→ Step 1b（:1136-1150）**无条件再 spawn 全闸**（gateArgs 含 `--human-approval-ref` :1137-1138），off-main / 契约面 / riskTier / provenance / blocked 任一违例即 BLOCK + setPod blocked + return false（:1145-1150）——**端到端「没有全闸放行就没有 merge」不变式由 Step 1b 背书，收窄不产生新 merge 级破口**；DRY_RUN 下 unlock 本被 :1368 守卫跳过，无「解锁后跳过 Step 1b」旁路；唯一瞬态 = 解锁与 Step 1b 之间 board 短暂显示 ready-to-merge（自纠于 :1148），board 状态属信息面。canary 4 kept-blocked 字面与意图均不变（:927 起 catch 块亲核）。三选项排序：**选项 2 > 选项 1（保留 spawn——「消双跑」名不副实、T6 对 merge-driver 侧收益归零、argv 语义耦合 wart 原样，且 T6 立项理由 ARQ-260907-gate-lib-script-separation 正是消这个）> 选项 3（全闸六条 + 豁免全部共享化——提取面扩到 evaluate 全域 + ctx 装配（含 execSync 副作用面需注入化），等价夹具负担 ×6 条规则 ×两档、批内重构风险上升，成本收益倒挂）**。落地点：T6 补申报句「解锁预核收窄为五要素事件核验，全闸由 Step 1b 同 invocation 背书（merge-driver :1132-1150）」+ V2 增背书夹具「unlock 收窄形态下，off-main / provenance 违例 Pod 解锁后仍在同 invocation 被 Step 1b BLOCK 并回落 blocked」。归档：human 拍板清单 #7。

### OQ3 严格档与 GA-8 合并说明

OQ3（human 未裁项）由 A-4 承载，两者同场给建议；challenger R2 分歧清单第 2 项（GA-8 ask 张力附 N3 输入）与 defender R2 分歧第 2 项同指 A-4，不再单列。B-1 挂点取舍（challenger R2 分歧第 3 项）见下「B-1 机械挂点」。

### B-1 · versioned-artifact-claim 机械消费挂点取舍——human 拍板（GA-8 域）

**裁断建议：顶 defender 克制侧——留根观测后批。** 理由：① fail-closed 兜底链在场（claim 漏记 → 两包撞槽 → T1-1 特判 blocked + 显式指引 → 预登记消解 → 并集，失败态 board 机械可见，非静默假绿；代价 = 效率损失非安全破口）；② §9 毕业先例逐字在案（pod-coordination.md:104-116「便宜档，不建 daemon」「毕业条件量化任一满足即评估升级」）；③ 本批 manifest 不含 portfolio-reconcile.mjs，夹带即越面（canary 11/12 域）。落地点：留根清单增行「versioned-artifact-claim 机械消费挂点（portfolio-reconcile `--check-claim` 形态候选）按 §9 毕业条件观测后批」——该行须真落留根清单，否则 R1 对 B-1 的 DISMISS 作废。**这是起点，可砍更大**（human 可判挂点即建）。归档：human 拍板清单 #8。

### F1 修向终裁——agent 层已够（随批改法吸收）

并入既有 `governance-gate-shared.mjs`（追加命名导出、既有导出与函数体零改动），**不改名**。裁决依据：① T6 要提取的 resolveHumanApproval 等现仅在 gate 内（lib 内 grep 零命中），本批动作 = 向既有单源追加，既有消费方按 import 语义自动继承零回退；② lib 头注 B5 行「computeFrozenPin 纯新增 + isFrozen 函数体零改动（S8 条件）」纯新增并入先例在案；③ 改名新 lib = 两个治理共享 lib 并存，「哪些函数归哪个 lib」成新双源，GA-6 同族病自我复制。challenger R2 B-Ⅰ 抽验自洽 + N2 名册六改七一并吸收。challenger 抽验无异议，终裁即此。

## 二、CONFIRMED 改法清单

### 2.1 进 PROPOSAL 冻结稿（architect 定稿回灌点，逐条列）

| # | 回灌点 | 改法 |
|---|---|---|
| 1 | manifest（:12-19） | 补 `docs/ARCH-REVIEW-QUEUE.md` 行（F3）；A-5 裁后补 `acceptance-evidence.md` 落点行 |
| 2 | PROPOSAL :97/:99/:116 + sweep 表 lib 行 | 措辞改「扩展既有 lib（追加导出，既有导出零改动）」，「新增」改「扩展」（F1） |
| 3 | V2（:137） | 补显式排除 changeId 误判差句 + import 探针主断言 + `grep -c "process.argv"` = 0 辅断言（F8+N1）+ 等价夹具名册六改七（N2）+ N5 背书夹具 + A-8 两态夹具 |
| 4 | V4（:139） | 负断言按 F2 修向改写（-cF 固定串 deny 两行 = 1 + ask 白名单两形） |
| 5 | V1（:136） | V1-③ 按 A-3 裁断条件化改写（F6）；V1-① 补 engine-patch 反例夹具（F9） |
| 6 | PROPOSAL :57 | 标题改 T1…T7（F4） |
| 7 | PROPOSAL :64 | 删「或两者恒用 projectDir」或标注先验排除（F5）+ T1-4 按 A-3 裁断改写 |
| 8 | PROPOSAL :72 | 「补三条」改「补四条」（F10） |
| 9 | T2-1（:71） | 初始清单扩实测超集五类（F9/A-7）+ owner/更新触发/vN 回收三句（B-2）+ §10c 单一权威互指注记（B-0） |
| 10 | T2 §10a | 增 core.editor=true 一次性设置 + 第 3 步改素形 continue（N3，A-4 裁后） |
| 11 | T5 条款 | A-8 放行方向声明 + Basis 匹配形态注释 + defer/accepted-risk 最小形态声明（B-6）+ gate 事实门内加项定性如实声明（A-2） |
| 12 | T6 条款 | N5 申报句（解锁预核收窄 + Step 1b 背书）+ 落点措辞改「扩展既有 lib」 |
| 13 | T7 条款 | 落点钉死 acceptance-evidence.md（A-5）+ guard-class 标注格式（F12/A-6）+ 齿名订正 makemeahanzi（N4，:160 与 architect-notes :117 两处）+ B-8 队列行 |
| 14 | T4 条款 | A-4 裁断回填：两条诚实边界 + 主张限定句 + N6 `_doc` 批准判据行 + B-7 耦合声明 |
| 15 | §6 留根清单 | 增行：planner 派单格式义务（F7/B-3）、versioned-artifact-claim 机械消费挂点（B-1）、hook airtight 形态 + B-7 耦合声明（A-4）、lint 夹具 + gate 化（A-6）、ask 收窄预案（A-4 降级预案） |
| 16 | PROPOSAL :105 | T7 目标文件行落点路径回填（A-5 裁后） |

### 2.2 进留根（§2.1 第 15 行已列，此处记义务属性）

F7/B-3（planner 派单格式——S-2 forcing-function 唯一落点）、B-1（claim 挂点——DISMISSED 成立前提）、A-4（hook airtight + 耦合声明）、A-6（lint + gate 化）四行均为**有主留根**（下一批触发条件已写明），非无主占位。

### 2.3 归后续批

escalation-cross-system-overfire Track A 批（OQ4 已裁本批 ack 后即启）、pass-oracle 第 4 关完整兜底、pod-events 事件型收编、settings 条件表达力核实（hooks 表达力）、B-1 挂点机械化、A-6 gate 化、F7 planner.md 修订——均已在既有留根清单或本 packet 留根行内，无新增无主项。

## 三、riskTier 复核与 review profile 终态

**riskTier：维持 medium。** 复核依据：本批确认后实际承载面比初稿更宽（lib 扩展至七消费方、ask 互洽修复、N5 收窄申报、A-8 补偿控制），但 ① 全部 CONFIRMED 改法方向均为收紧或如实申报，无一放宽放行集（GA-7 补集不变式由 V1-① 夹具机证）；② 三条 BLOCKING 均为文本/夹具/清单级修向，非行为面重设计；③ downside 护栏在场（五自检 + fail-closed 默认 + N5 Step 1b 背书 + canary 12 条全数重派）；④ 无删数据、无契约触面、无跨系统 spec 语义。无升 high 因素，无降 low 因素（合并机械链 + 强制层 settings + gate 核验行的真实病根承载不变）。

**review profile 终态：governance · Track A full-adversarial + human（OQ1 裁并批升级）· G2+G3 混合 + G4 簿记，approver 恒 human。** 批内未发生把触面挪进 G1 的改法（A-5 复判明确避开 DEV-GOVERNANCE.md 落点，GA-1 陷阱未命中），profile 无需重报 moderator。

## 四、human 拍板清单（8 项显式 + 1 项整包批准）

按 DEV-GOVERNANCE 必须 human 的域：agent def 改动 / self-def / 强制层 / 指令根 / 闸判据。以下各项均为**建议即地板**——裁断可砍更大。

| # | 项 | 域 | 建议 |
|---|---|---|---|
| 1 | A-3 修复口径（合并机械链行为变更，gate 事实） | 闸判据 / 合并链 | 维持两形、只改豁免路径回退分支（errorExit 化） |
| 2 | A-1 判据先例归档 + integrator.md 新节生效 | agent def / self-def 边界 | 不触硬排除字面、恒 human-approved；先例四条件归档 |
| 3 | A-4 / OQ3：T4 ask 形态 + N3 runbook 互洽修法 | 强制层 settings + GA-8 | ask 条件保留，先修 runbook 命令形态；降级预案留根 |
| 4 | A-5 T7 规约落点 | G2 落点选择 | acceptance-evidence.md；两排除落点不加指回 |
| 5 | A-6 字面耦合检查设门与否 + guard-class 格式 | 闸判据 | 本批不设门，lint 留根；格式本批钉死 |
| 6 | A-8 T5 未识别形态方向 + B-4 写作侧对齐 + B-6 defer 枚举值 | 闸判据 | 放行（只拦可识别形态）+ 三件补偿控制同批到位 |
| 7 | N5 解锁核验集收窄（合并级 fail-closed 由 Step 1b 背书的申报） | 闸核验面 | 选项 2：收窄 + 申报句 + V2 背书夹具 |
| 8 | B-1 机械消费挂点取舍 | GA-8 / 挂点域 | 留根观测后批（§9 毕业条件）；可砍更大 |
| 9 | 整包批准：decision-packet 全部裁断 + riskTier medium 复核 + §2.1 十六处回灌点授权 architect 定稿回灌 | 治理整批 | 批准后进定稿 → FROZEN → landing 序列 |

agent 层已够（不占拍板带宽、随批改法吸收）：F1–F6、F8–F10、N1、N2、N4、B-0、B-2、B-3、B-7、B-8、A-2、A-7 的批内修向与归档。
