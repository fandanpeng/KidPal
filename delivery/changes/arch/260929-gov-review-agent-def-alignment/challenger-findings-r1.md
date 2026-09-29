# Challenger Findings · R1 · 260929-gov-review-agent-def-alignment

> challenger · 2026-09-29 · 判据 = 冻结 charter（review-charter.md）+ §5 必载蓝图原则。全部 file:line 断言为本次评审实测（grep / 脚本实跑 / 文件实读），适用「搬运引据视同新断言复证」纪律的对手侧：PROPOSAL 自称行号已逐一抽验，偏差如实记。利益相关声明：challenger 自身 def（challenger.md）在本批 T4 目标内（finding F21）。

## 一、Findings

### F1 · BLOCKING · C6/B4 实锤：域外同构残留 20+ 处未入账，sweep 透镜与验证集双窄（轴 2 / 判据轴 4）

**实扫证据**（本次 `grep -rn "docs/systems|apps/sX-|zod|run_inline_check|write_knowledge|knowledgeSuggestions|s2Exploratory|get_run_results" .claude/ --include=*.md/*.json/*.mjs`，排除 agents/scope-map 后全部命中）：

- `.claude/commands/arch-change.md:24/:34/:40`——**本批 Track A 入口命令自身**仍指 spec 草案落 `docs/systems`；
- `.claude/commands/review-arch.md:3/:20/:28`、`.claude/commands/build-feature.md:21/:30`（s2Exploratory 字段判定与 integrator S2 scan 指令全文）；
- `.claude/references/arch-review-protocol.md:12/:37/:138`、`architect-operating-model.md:26/:72/:75/:79/:110/:119`、`dev-loop.md:6/:110/:174/:303`（含裸 `contracts/` + zod）、`security-hardening.md:7`（zod）、`task-intake-and-slicing.md:338`（`docs/systems/<S>/UIUX-DESIGN-BRIEF.md`——**在已列 manifest 的文件内**，T1/T5 均未覆盖此行）/:412（`run_inline_check` 引用，见 F2）；
- `.claude/skills/brainstorming/SKILL.md:59`；
- `.claude/route-fixtures/surface-alignment.json:7/:12`——architect/review-moderator allow 的 **scope-map 镜像**，T2 改 scope-map 后镜像必漂（该 fixture 现无脚本消费，grep scripts/hooks/settings 零命中，属死配置或人读对账件——无论哪种都该入账）。

**问题**：PROPOSAL §3 sweep 透镜复扫模式 = `grep … .claude/agents/ .claude/scope-map.json`，§5 验证集 1/2/4 同域——批头「16 个 agent def 与 scope-map 的文本回到与仓现实一致」只对 agents+scope-map 成立，但 T1 根因自述（「260913 清扫没扫 `.claude/agents/**`」）正是同构漏面复发；charter C6 明示「至少一次性 census 证明域外零命中或把命中入账」。实测非零且 20+ 处。**§3 头部「UNCLASSIFIED=0」声明被上述未分类受影响件证伪。** 处置三选一（elicitation E4）：扩本批 scope / 立 ARQ 同车留根（owner+触发）/ Track A 内追加 T 项——静默不在任何一项 = FROZEN 后欠账即刻复发。

### F2 · BLOCKING · T5 联动清单与实况错位：OQ2(a) 必产半截形态，与批自我目标直接冲突（T 之间 / T ↔ 仓库漂移）

- PROPOSAL T5 称联动「task-intake-and-slicing.md 的 s2Exploratory 字段定义段」——**实读 task-intake 无该「字段定义段」**：s2Exploratory 在该文件仅 :412（起包可行性 convention 的**例证句**，且例文明文引「integrator 经 `run_inline_check` … 见 dev-loop.md §3 / integrator.md Step 5」）与 :245（可省字段清单）。字段定义真身在 `.claude/commands/build-feature.md:21`（拆包侧判定/声明指令整段）与 `.claude/references/dev-loop.md:174`（字段定义 + typed fields 全文）——**两处均不在 T5 目标文件**。
- OQ2(a) 删除 planner:53 字段句 + integrator S2 段后：build-feature.md 仍指示拆包方判定并声明 `s2ExploratoryRequired/Reason/Goals[]`，dev-loop.md 仍定义字段——下游（integrator Step 5）已被删，声明无兑现方，恰是 task-intake:412 自己立的「声明一个兑现不了的要求」禁止形态；同时 :412 例证句指向已删步骤。
- B3 答案（查清，DISMISS 脚本担忧）：`grep -rn "s2Exploratory|run_inline_check" .claude/scripts/ .claude/hooks/ .claude/verify-gates.json` **零命中**——无机械闸消费该字段，删除无闸红风险；风险全部在 command/reference 文本半截。
- 批头自我声明「四根同源，一批清、防半截形态」——T5 现范围恰产半截。修法：T5 联动清单补 build-feature.md:21/:30 + dev-loop.md:174 + task-intake:245/:412，或显式留根（owner+触发）。

### F3 · BLOCKING · B1：验证集与改法脱节——无任何正向 enforce-scope 行为验证（SC-1 / 判据轴 4）

PROPOSAL §5 第 5 条只有 `python -m json.tool`（解析）+ 各 grep 零命中。本批唯一声称修复的机械实害（integrator docs-sync 写 `docs/requirements.md`/`docs/architecture.md` 被 enforce-scope 拦）没有**正向**证据。charter §4 B1 与 SC-1 均要求「正向验证而非只 grep 零命中」。修法便宜：构造钩子 stdin JSON（`agent_type: integrator`、`tool_input.file_path: docs/requirements.md`）喂 `node .claude/hooks/enforce-scope.mjs`，断言 exit 0；再各喂 architect/review-moderator/adjudicator 用例，把行为矩阵落成可复跑命令。补进 §5 编号条目，否则 T2 的「修复」停留在纸面。

### F4 · BLOCKING · T1 改法语义空洞：spec「草案面」在两文件平铺世界里不存在，blanket 替换 = 授予 architect 直写 canonical spec（轴 0 / SC-1 / 需人类取舍）

- architect.md:53 现行文本：「`docs/systems/**`（canonical spec = docs/requirements.md + docs/architecture.md）。这是 spec 初稿、候选修订与低风险设计维护面，**写入不等于 spec 已冻结**」——旧模型的「草案面」与 canonical 分离：写入 systems ≠ 冻结。T1 改法「全部替换为 `docs/requirements.md` + `docs/architecture.md` 平铺表述——architect.md 设计面 / 起草落点…统一改写」把**起草落点**直接换成两 canonical 文件本体；叠加 T2（architect.allow 增两文件）后机械结果 = architect 获得对 canonical spec SSOT 的直接写权，且 def 文本宣告「写入不等于冻结」——canonical 文件里将出现无 freeze 机制区分的草案内容，与「spec freeze 归 review-moderator 对抗 review / 低影响落地归 moderator」的治理语义冲突（review-moderator.md:20/:73）。
-这不是行号问题而是 T1 改法**未回答**的问题：spec lane 的草案/候选修订现在落哪？（elicitation E1——候选：`delivery/changes/arch/arch-review-<spec>/`、workstreams scratch、或明文允许 architect 直改 canonical 但删「写入不等于冻结」句并说明 freeze 语义如何承载。）T1 定稿前必须答，否则改法文本会把一个未决设计决策固化进三个 def + scope-map。

### F5 · BLOCKING · C5/T2 定性失真：「写域换路径属对齐非扩张」不成立；adjudicator.deny 腿行为惰性使「deny 语义保持」断言空转（SC-1 / OQ4 材料保真）

实读 `enforce-scope.mjs:188-195`：per-agent 语义 = deny 命中拦 → **allow 非空且未命中也拦**（白名单语义；scope-map `_doc` 第 10 行自述同）。据此建行为矩阵：

| 主体 | 换路径前 | 换路径后 | 性质 |
|---|---|---|---|
| integrator.allow | 两 spec 文件被 allow-miss **拦**（实害成立） | 放行 | **扩权**（修复实害所必需，但仍是扩权） |
| architect.allow | 两 spec 文件被拦 | 放行 | **扩权**（并叠 F4 语义问题） |
| review-moderator.allow | 同上 | 放行 | **扩权**（spec profile 低影响落地所需，与 :20/:73 语义一致） |
| adjudicator.deny | `docs/systems/**`（死 glob） | 两文件 | **行为惰性**——adjudicator.allow 非空，任何 docs 根文件本就被 allow-miss 拦；deny 只改报错文案。charter C5① 担心的「未来 docs 根新增白名单文件拦截面收窄」不成立（本来就不许写） |

PROPOSAL 头部 riskTier 理由「写域换路径属对齐非扩张」对 integrator/architect/review-moderator 三腿**字面为假**（现拦→后放 = 扩权）；「deny 语义保持」对 adjudicator 腿平凡为真（因 deny 惰性）而非行为分析成果。OQ4 的 human 过目材料必须按「三腿扩权 + 一腿惰性 + OQ3 一腿收权」如实呈现，否则 human 在失真口径上放行 G3 权限面。不改变 OQ4 必答的结论，但改变陈述义务。

### F6 · SHOULD · PROPOSAL 内部矛盾：非目标声明 vs T7 目标集（landing-verifier）

§0 非目标：「story-author / requirement-reviewer / scenario-author / challenger / landing-verifier 五个干净 def **除 T1/T4 点名行外不动**」——landing-verifier 不在 T1/T4 任何点名行内，但 T7 目标文件明列 `.claude/agents/landing-verifier.md（改）`（description 收短）。同一文件两处打架，FROZEN 前须改口径（如「除 T1/T4 点名行与 T7 description 收短外不动」）。

### F7 · SHOULD · 条件锚机制自相矛盾：OQ2=(b) 时 task-intake 锚「裁掉」不可能成立（SC-4 / manifest 一致性）

T5 改法：「联动同步 task-intake-and-slicing.md…（已列入 manifest，**若 OQ2 裹保留则该锚裁掉**）」。但 T6 目标文件**无条件**含 task-intake（L2 行 :326 同步）。task-intake 因此恒在 §2 锚并集内；若按 T5 条款裁掉 manifest 中该锚，`landing-fidelity-check.mjs` 第 8 类错（§2 锚并集 ≢ manifest，实读 ：599-614）必红。条件锚条款须改为「OQ2=(b) 时 task-intake 锚保留、由 T6 承载」或同形修正。

### F8 · SHOULD · 验证集第 6 条覆盖虚标：route-regression-check 根本不含 planner / builder / landing-verifier（C2 / B2 / SC-2）

实读 `route-regression-check.mjs` + `route-fixtures/routing.json`：`agentRequiredSignals` 仅 8 角色（review-moderator / scenario-author / challenger / defender / adjudicator / architect-reviewer / story-author / requirement-reviewer），`agentRequiredToolsAgents` 仅 review-moderator。**builder（T3 description 重写——模型级路由信号，charter 自认高风险位）、planner（T6 frontmatter `Agent()` 列表编辑——误伤即破 dispatch 白名单）、landing-verifier（T7 收短）均无任何 fixture 断言**。§5 第 6 条「0 fail（T3/T6/T7 后路由信号不退化）」对这三件是零保护的假安心；T6 摘 uiux-author 后即便误删 `Agent(architect,…)` 任何成员，该机检也绿。修法：routing.json 补 builder/planner/landing-verifier 行（本批或立 ARQ），§5 第 6 条如实标注覆盖域；T3/T6 diff 按 authoring-agents-skills 纪律 3 逐字段隔离人读为唯一后盾。SC-2 的机检盲区比 charter 预设更大：不止「fixture 外消歧句」，连 fixture 机制本身都未覆盖这三个文件。

### F9 · SHOULD · C4 复派生 delta：uiux-author 引用实际 ≥7 处，「三处全摘」清单不全（T6 / OQ1）

实测引用清单：planner.md:4（frontmatter Agent 列表）、**:61**（Step 6b，ARQ-260923 记 ：54 已漂）、acceptance-author.md:23、task-intake:320（prose「Depth drives whether a `uiux-author` step runs」）与 :326（L2 行）、**`.claude/references/dev-loop.md:55` 与 ：68**（Pod 流程图 + 条件步骤定义段）。charter C4 与 PROPOSAL T6 均只列三处/四处；dev-loop.md 两处与 task-intake:320 在 OQ1(a) 摘引用后成为残留引用（「派生 uiux-author」指令仍在流程 SSOT 里）——正是批标题要防的半截形态。OQ1(a) 执行清单须按 7 处重列 + grep 兜底。旁及：planner.md:61 同句还引 `uiux-designer`（def 亦不存在，DEV-GOVERNANCE §8 已知缺口「dispatch 尚未接线」在册）——OQ1(a) 重写 6b 时对 uiux-designer 的表述方式须一并定（引用带「未接线」限定词或去引用），G1 编制表行本批不动须留根。

### F10 · SHOULD · B5：review-moderator.allow 的 `docs/contracts/**` 未入 OQ3 范围且无留根

scope-map:102 实况：review-moderator.allow = `[..., "docs/systems/**", "docs/contracts/**"]`。与 architect 同性质 approved-landing 遗留（review-moderator.md:90 自述「不改 docs/contracts/**」）。OQ3 只裁 architect 腿；review-moderator 腿为何不同批处置 / 留根在哪，PROPOSAL 未答。同批收窄是零成本顺车（收权方向、同一次 human 过目），不处置须给理由并留根（owner+触发），否则 OQ3 的「一次清干净」推荐理由自我削弱。

### F11 · SHOULD · SC-5 / 第五阀执行形态：PROPOSAL 只字未提独立性安排

被改清单含 review-moderator / adjudicator / defender / architect-reviewer / challenger（五阀角色）+ architect.md（D-C 硬排除）。charter SC-5 明确要求显式回答：对它们改动的 re-review / landing 核验由「不加载被改 description 的独立会话 / harness」还是「新 canonical 生效前跑」承担。PROPOSAL 通篇无此安排；defender（及 challenger）自身 def 在改清单内，其对抗独立性如何保全须在 decision-packet 前落字。

### F12 · SHOULD · T7 散文型清单执行风险：defender.md:47 是约束句+教训句合体，整句删 = HQ-1 违规（B6 / SC-2）

实读 defender.md:47 全文：「**必须用 Write 工具落盘，禁止用 Bash 重定向写**（`cat >`/`tee` 等）：Bash 写既绕 scope 边界，又会被正文里的撇号/引号打破 shell 解析（历史教训：heredoc 正文撇号不成对 → `unexpected EOF …`）。…」——ARQ-260929-provenance-noise 把该行整体记为「heredoc 历史教训句」清理对象，但行首是 must-never 级行为禁止（HQ-1 不可 prune 类）。T7 执行须 trim-not-delete（留禁止句、去括注教训）。同时验证集第 8 条只复跑 census 窄 pattern，**未把 ARQ 的散文型人工清单列为核对输入**（charter §4 B6 与轴 5 反向腿均要求）——review-moderator:12/:104、adjudicator 卡点 D、defender:47 全在 census 盲区，第 8 条须补「按 ARQ 散文清单逐条销账」。

### F13 · SHOULD · C1：形态族谱复扫留根在 §6 而非 §5 硬前置，且 §5 全部为斜杠形模式

§6 第 3 条留根「path-form-family.md 形态族谱复扫——landing 前按该表逐类子检一遍」——但它不在 §5 验证集编号条目内：留根清单语义 = 可延后项，「landing 前」时点约束与留根载体自相矛盾，按本仓惯例大概率被跳过（260914 批正是四轮机检全漏后靠 landing-verifier 接住）。须升为 §5 编号硬前置。另：§5 第 1-3 条 grep 全为斜杠形（`docs/systems`），按族谱教训，引号分段形、转义形（`docs\/systems`）、相对链接形不在覆盖内；「清扫模式与验收判据同一条模式的两次使用」目前满足但**同盲**——同源不等于全覆盖。F1 的域外命中里 `dev-loop.md:303`（`contracts/`（zod））即是裸形实例。

### F14 · SHOULD · 轴 7 弱项确认：「agent 存在性机检缺位」留根无 owner、触发二义（C4 / §6）

§6 第 4 条：「是否加牙交 /loop-retro **或本批评审提级**，不在本批硬治」——两个候选去向并列、无 owner、无触发条件，正是 charter 判据轴 7 点名打掉的形态。对照第 1 条（story-author 观察位有明确触发）即可见差距。elicitation E2：归属二选一，落 owner。

### F15 · OPTIONAL · T8/SC-3 透镜行与既有承载的重复度及验证自缚

acceptance-author.md:36（Process 第 6 步红基线）与 ：45-46（已引 DEV-GOVERNANCE 验收测试纪律 §2 probe≠exercise / §3 冻结即冒烟）构成既有承载；T8 透镜行三件事中「host/真机近似假设显式化」与 §2/§3 部分同域——OQ5 措辞须显式与既有引用去重（引用或收编，不并列两套）。另：验证集第 9 条 grep「透镜」把机械验证与措辞字面同源——OQ5 若裁定的措辞不含「透镜」二字，验证自缚失败；建议验证锚改为「Step 6 旁新增行在场」的行计数或 OQ5 定稿措辞的固定子串。

### F16 · NIT · 计数与标注偏差（复证纪律下如实记）

- §3「12 个（T1/T3/T4/T5/T6/T7 并集）」与 charter §0「12 个 agent def」均误：并集实为 **13**（T7 的 12 个 + challenger），manifest 本身正确列了 13 个 def 文件。
- T1「共 11 行命中」实为 **10**（其自列清单 3+4+3=10）；ARQ-260929-reality-drift「共 16 行命中」实为 **14**（agents 10 + scope-map 4）。
- ARQ-260929-reality-drift :88 标注「architect.deny」——scope-map:88 实在 **architect.allow** 块内（:87 `"allow": [`）；PROPOSAL T2 的标注（architect.allow）正确，ARQ 证据段有一处错标。
- ARQ-260923「planner.md:54 Step 6b」现漂至 ：61。

### F17 · FYI · SC-4 实证答案：landing-fidelity 对「落了、manifest 没有」方向无检

实读 `landing-fidelity-check.mjs:628-637`：claimed-but-not-landed（summary→git）与 planned-but-not-landed（manifest→git）两方向硬失败在场；**反向（git changed ⊄ manifest）无检**——落地超出 manifest 且 summary 未声称时静默绿（仅 summary 声称越权时 :626 unauthorized 兜底）。对条件锚场景的推论：OQ1(b) 裁补 def 后若 landing 落了 `uiux-author.md` 而 manifest 未同步增锚，不会红。SC-4 的「双向一致」只有单向机械保障；packet 三点互证的人读腿须明示承担该方向。

### F18 · FYI · B7 查清（PROPOSAL 补录义务，非 OQ3 反对票）

OQ3(a) 收窄后 architect 契约预审合法写面仍有落点：gate evidence 落 `delivery/changes/arch/*gov-review-*/**` / `*arch-review-*/**`（architect.allow:88 保留，OQ3 只去 `docs/contracts/**`）；authoring 面 `.claude/references/**`（contract-change-evidence.md 骨架）在 allow 五面内不动；契约本体恒由 human 签字 + 主线程 atomic PR 落地（AGENTS.md 契约冻结硬规则），architect 恒不 landing。结论：OQ3(a) 安全；但该论证 PROPOSAL 未载，须补录进 OQ3 条目供 human 裁断时在案。

### F19 · FYI · B2/B6 邻的机检实况补充

route-regression-check 只读 frontmatter description + tools 行（实读 ：24-37），正文消歧句、散文型注记全盲——charter B6 判断证实。T7 的 12 个文件中仅 6 个 review-loop 角色在 fixtures 内（见 F8），lint-descriptions 是唯一全量机检（warn 级、exit 0，非门）。T7 质量实质承载 = 逐文件人读 diff + landing-verifier，PROPOSAL §5 第 8 条已如实标注「半机械」——但须按 F12 补散文清单输入。

### F20 · FYI · C3 复派生结果：目标形态母版当前无实症

story-author / requirement-reviewer 实测 grep（changeId/gov-review/FROZEN/实证 pattern）0 命中——母版干净，批量照抄风险当前无实症；scenario-author 的 2 处命中为合法路径引用（:43/:53 output 落点），非残留。T8 措辞与消费方同形由 OQ5 + F15 承载。C3 判「已派生、无实症、防护靠 OQ5+人读」。

### F21 · FYI · 利益相关声明

challenger.md 在本批 T4 目标内（:30/:77 影子路径统一，实测确认两行存在、:22/:47 已是正形）；本 finding 记录不因自身 def 在改动清单而改变立场，但 SC-5 独立性安排（F11）对 challenger 同样适用。

## 二、Canaries 重新派生结果（pass-oracle 关 5）

| Canary | 状态 | 派生落点与 delta |
|---|---|---|
| C1 路径形态族谱 | 已重新派生 | F13——复扫滞留 §6 非硬前置；§5 全斜杠形同盲；同源纪律满足但同源≠全覆盖。delta：F1 的 `dev-loop.md:303` 裸 `contracts/` 为非斜杠形实例 |
| C2 description 误伤 name/tools | 已重新派生 | F8——delta 超 charter 预设：builder/planner/landing-verifier 根本不在 fixtures，逐字段隔离无任何机械后盾；复跑两 lint 对这三件是假安心 |
| C3 变形标记照抄 | 已重新派生 | F20——母版（story-author/requirement-reviewer）实测 0 残留，无实症；T8 措辞同形由 OQ5+F15 承载 |
| C4 verifier 退役同构 / 存在性机检 | 已重新派生 | F9+F14——delta：引用处实为 ≥7（charter 记 3）；route-regression-check 实读证实无存在性检查且 planner 不在 fixtures；ARQ-260923 行号 :54 已漂 :61 |
| C5 换路径 ≠ 语义等价 | 已重新派生 | F3+F5——delta 超 charter 预设：allow-list 语义下 deny 腿惰性（charter C5① 担心的拦截面收窄不成立）、三腿实为扩权而非「对齐非扩张」 |
| C6 清扫漏面同构复发 | 已重新派生 | F1——delta：不是「风险」，是实锤 20+ 处域外命中（含本批入口命令自身与已列 manifest 文件内行） |

## 三、Charter 断链点 B1–B7 处置

- **B1** → F3（BLOCKING，正向钩子验证缺位）。
- **B2** → F8（fixtures 实况比 charter 预设更弱：builder/planner/landing-verifier 无 fixture 行）。
- **B3** → 查清后部分 DISMISS：无脚本/闸消费 s2Exploratory（scripts/hooks/verify-gates 零命中）；残余风险转入 F2（command/reference 定义面半截）。
- **B4** → F1（BLOCKING，实锤命中清单）。
- **B5** → F10（review-moderator.allow 契约面未处置无留根）。
- **B6** → F12+F19（散文型清单须入验证集第 8 条；机检盲区实况）。
- **B7** → F18（查清 + 论证补录义务；OQ3(a) 方向安全）。

## 四、Elicitation（需 human 领域知识 / 取舍，非 gotcha）

- **E1**（F4）：两文件平铺世界里，spec lane 的草案 / 候选修订落点是什么？（architect 起草面与 canonical 的关系须显式重定义——候选：arch-review 证据目录、workstreams scratch、或明文允许直改 canonical 并重述 freeze 语义。）这决定 T1 三个 def 的正确改写方向与 T2 architect.allow 腿的语义。
- **E2**（F14）：agent 存在性机检缺位的归属——本批评审提级（作为 G3 附带小项）还是 /loop-retro？owner 谁？
- **E3**（F10）：review-moderator.allow 的 `docs/contracts/**` 与 OQ3 同性质——同批收窄、还是留根（owner+触发）？
- **E4**（F1）：域外 20+ 处残留的入账方式——扩本批 scope / 立 ARQ 同车 / Track A 追加 T 项？
- **E5**（F11）：第五阀执行形态——对被改 review-loop def 的 re-review 用独立会话/harness，还是安排在新 canonical 生效前跑？由谁执行？

## 五、场景覆盖对账（charter §3 六场景）

- **SC-1**：F3/F4/F5（正向验证缺位、起草落点语义、行为矩阵）。
- **SC-2**：F8/F12/F19（fixtures 覆盖实况、消歧句与约束句保护、散文清单）。
- **SC-3**：F15（透镜行重复度与验证自缚；OQ5 前置无异议）。
- **SC-4**：F7+F17（条件锚裁剪矛盾、fidelity 单向覆盖实证）。
- **SC-5**：F11（PROPOSAL 静默，须补）。
- **SC-6**：本批流转正常（立项 ack 已获、charter 已冻、R1 对抗在场）；F11 是 handoff 链上唯一缺环。

## 六、摘要

Findings 21 条：**BLOCKING 5**（F1-F5）/ SHOULD 9 / NIT 1 / OPTIONAL 1 / FYI 5。Canaries 6/6 已独立重新派生（其中 C2/C5/C6 三条派生结果超出 charter 预设强度）。B1–B7 全部处置（5 转 finding、2 查清部分 DISMISS 并转 finding）。Elicitation 5 问。最需 human 拍板前解决的：F1（域外入账）、F2（T5 半截）、F4（草案落点，E1）——三者不解，PROPOSAL 不宜进 OQ1–OQ5 批阅与 FROZEN。
