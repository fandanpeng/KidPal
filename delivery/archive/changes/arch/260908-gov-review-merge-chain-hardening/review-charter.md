# Review Charter — 260908-gov-review-merge-chain-hardening（governance profile · Track A full-adversarial）

> **状态：FROZEN v1.0 · 冻结时间 2026-09-08 · scenario-author 冻结**。本文件对 challenger / defender **只读冻结**；改变轴 / 场景 / canary / 裁定项须经 review-moderator 重新派 scenario-author，辩论中不得自行解读本 charter 未载的判据。
> **待审对象**：`changes/arch/260908-gov-review-merge-chain-hardening/PROPOSAL.md` v0.2（含 §8 architect 修订注记）+ `architect-notes.md` CANDIDATE v0.1（§T3 / §T4 / §T7 / §review-bg——评审输入件，不进 landing-manifest）。
> **承载 ARQ**：ARQ-260908-pod-integration-no-legitimate-executor（面 A 全量 + 面 B 核验行）+ ARQ-260908-versioned-artifact-slot-collision + ARQ-260908-versioned-guard-version-literal-coupling（OQ1 裁并批）+ ARQ-260907-gate-lib-script-separation（OQ2 裁随批）；销账两条 260907 行随 landing 结移（V5）。
> **批类型判定**：governance profile 混合批——G3 合并机械链硬化（merge-driver / gate / settings / lib）为主承重面 + G2 agent 执行边界与规约成文（integrator.md / pod-coordination.md / T7 撰写约定）+ G4 簿记（ARCH-REVIEW-QUEUE 结移）。非 spec profile 的「技术重 / 领域重」二分对象；判据轴按 governance pass-oracle 第 1 关的治理判据集裁剪（见 §2），不用 spec profile 轴 0-11 原文。
> **oracle**：governance profile pass-oracle 六关（arch-review-protocol.md「Governance profile pass-oracle」）——本 charter 的轴集 = 第 1 关覆盖清单，canary 集 = 第 5 关清单，裁定项清单 = 第 3 关（高敏项标 HUMAN-RULED）与第 6 关（decision-packet 完整性）的输入。

---

## 1. 审查对象与范围声明

**审什么**：PROPOSAL §2 改法集 T1–T7 + §5 验证集 V1–V6 的架构自洽性、fail-closed 完备性、跨治理面对齐与可批准性。architect-notes 的候选正文（§T3 节正文 / §T4 形态裁定 / §T7 落点与条款）按候选稿审——「整节原样可落」的声明本身是被审对象，不是已定事实。

**明确排除（不审）**：
- **OQ 已裁项的取舍本身**：OQ1（并批 + 整批 Track A）、OQ2（gate-lib 随批）、OQ4（escalation 批 ack 后即启、另行立项）均为 human 2026-09-08 显式裁断。challenger 不得攻击「该不该这么选」，只可攻击其**实现含义与一致性**（例：并批后 T7 与 T1-1/T2 的登记表口径是否互洽——攻一致性，不攻并批）。
- **escalation-cross-system-overfire 的判据语义**（OQ4 已裁另批，入口 `/arch-change`）；本批唯一相关义务 = 不夹带该面改法（PROPOSAL §0 非目标，可核）。
- **契约改法本身**：T5 只加核验、不加写权，`docs/contracts/**` 零 diff——契约门不触发（GA-9 只做「确未触发」的独立确认，不审契约内容）。
- **销账的历史判断**：「两条 260907 ARQ 已被 merge-friction 批完整落地」是主线程 2026-09-08 核销确认——challenger 不重审该历史判断，但 V5 机械核验（结移 + 承载批引用）必须独立过。

**评审基线**：main tip `fae886a`（PROPOSAL/architect-notes 声明的亲核基线为 `fae886a`；PROPOSAL 提交不改实现面，两基线实现面等价——challenger 引行号时若发现漂移，先核当日 tip 再落 finding）。

---

## 2. 适用治理判据轴（challenger 逐条质询；pass-oracle 第 1 关清单）

> 十条轴。每条给出本批映射与「为什么适用」；架构师 §review-bg 六条关注面与 PROPOSAL 头注两处判据存疑点的归属在表内显式标注，对应的显式裁定项见 §3。

| 轴 | 本批映射 | 为什么适用 |
|---|---|---|
| **GA-1 · G 层归属与 review profile 一致性** | 逐目标文件核 G 层：T1/T4/T5/T6 = G3（scripts / settings / lib），T2/T3 = G2（references / agents 正文），T7 = G2 成文 + G4 簿记（ARCH-REVIEW-QUEUE.md 结移）。陷阱两处：① T7 落点三选一中 **DEV-GOVERNANCE.md 是 G1**——若裁它落点，approver 语义与触面成本全变（architect-notes §T7 已以「G1 触面成本升级无对应增益」为由排除，该排除论证本身是被审对象）；② PROPOSAL 头注 packet-binding 行写「§2 改法集（T1–T6）」而 §2 实含 T7——packet-id 定义与改法集不自洽，manifest 六文件不含 T7 落点（三选一未裁无法列） | G1/G2/G3/G4 归属错一档 = approver 错人。本批全落 human approver，但「落点选择改变 G 层」的判定必须显式 |
| **GA-2 · authority / owner / writer / approver 四元组一致性** | 逐改法核四元组：owner=architect、landing=主线程 packet-bound、approver=human 恒成立？重点三处：① T7 他人冻结件重指——writer 实际将是 acceptance-author（dev-loop 侧），authority = 各冻结件作者域，授权链 = human 逐件（PROPOSAL 承诺「只登记不预批」）；② T2 §10c 登记表本体的 owner 与更新时机（「随批维护」——随哪个批、谁写，PROPOSAL 未名之，见 §5 B-2）；③ `versioned-artifact-claim` 事件的写入方（Pod launch 面 planner）与读取方 | 四元组错位是治理批头号病灶；本批跨 dev-loop / review-loop 两 loop 交接面多，最易「authority 在此、writer 在彼」 |
| **GA-3 · 自改递归 / agent def 执行边界（存疑点①归此）** | T3 给 integrator.md 加「merge-chain execution」节 = 把 :74「不 push、不合并主干」的默认禁止显式豁出一角（Pod 分支 rebase/消解），实质扩执行面。三问：① PROPOSAL 头注存疑点①——「agent def 正文新节」是否触 lightweight 硬排除判据，判官须显式裁并归档（本批已在 Track A full，裁断意义 = 归档该判据的适用边界，供后续同类申报引用）；② 非自改（architect 起草 + human 批）成立，但新节的前置三条件（逐文件预登记 / eventId 在场 / 分支亲核）是否**可机判的 fail-closed**，还是可被会话自由裁量绕过的 prose；③ 新节与 integrator.md 既有边界句 :73-74、:12 的语义叠合是否唯一赢家 | architect §review-bg 关注面 3 归此。agent def 执行面扩展 = 权限语义变更的成文形态，前置不 fail-closed 则豁出的一角就是新破口 |
| **GA-4 · 工程门边界 / gate 事实归档（存疑点②归此）** | 三处 gate 事实逐一定性：① T5 闸内新核验行——是既有前置升级闸的**门内加项**（对偶 DEV-GOVERNANCE §5 折进注「closeout 硬化 = verifier 闸族门内加项、维持 warn 先例」），还是机检硬排除意义上的「新增 merge gate」——判官显式裁并归档（存疑点②）；② T1 四项行为扩展 = 合并机械链行为变更（PROPOSAL 已声明交判）；③ T6 闸结构重构（薄壳化 + lib 分离）声称「判定语义零变更」——零变更声明由谁证、证到什么粒度。另核：六工程门不变式（不得另立第 7 工程门）——T5 核验行与 T4 ask 均不得被叙述成新门 | 前置升级闸本身是 260831 批「折进注」先例产物；本批在其上叠改，「门内加项 vs 新门」的定性直接决定 approver 力度与后续 ARQ 的援引先例 |
| **GA-5 · 跨治理面对齐（agent def ↔ settings ↔ runbook ↔ scripts ↔ scope-map ↔ hooks）** | 五面表述不得打架：T3（integrator.md 新节）↔ T4（dev-loop.settings.json ask）↔ pod-coordination §10a runbook（:124 第 1 步就是在 Pod worktree 手动 `git rebase`）↔ merge-driver 行为（特判 / 导出 / 双源）↔ scope-map（integrator allow 10/10 现状，本批不改）。重点：① **Step 1b「同形双源」事实裁定**（A-3，归此轴）——`approvalRefUnlock`（merge-driver.mjs:915-921，cwd=probeWt :919 / 参数 `--project-dir projectDir` :920）与常规路径 Step 1b（:1137 gateArgs 传 `--project-dir projectDir`、:1142 `cwd: wt`）实为**同形**（cwd=worktree、参数=主仓），ARQ 并证二②「常规路径无此形状」的对照系失真已由 architect §review-bg 1 亲核申报；真实差异仅在 `findWorktree(tp, projectDir) \|\| projectDir` 回退分支（:919）——worktree 解析失败时 cwd 落主仓 → `--source-ref HEAD` 在 main 上求值 → off-main BLOCK，方向 fail-closed 无损但恒空转。改两路径还是只改豁免路径 = §3 A-3；② T4 ask 的绕道面诚实性（architect §review-bg 关注面 4）——前缀规则可被复合命令绕过（仓内对策先例 = deny-main-push.mjs :68 起的引号感知切分），「ask 是 best-effort 层、airtight 需 hook」的边界声明必须回填 PROPOSAL（architect-notes §T4 两条诚实边界），不得只落 landing-summary | governance profile 的机械对偶（pass-oracle 第 1 关明列「跨面对齐」人工核查）。本批改法每条都横跨 ≥2 面，单面自洽≠全局自洽 |
| **GA-6 · 单一事实源 / 注册表锚点纪律** | ① §10c 版本化制品登记表（文档侧 authority）与 T1-1 `*.v[0-9]+.md` 形态正则（脚本侧白名单）是**双源**——两源清单不一致时谁赢？arch-review-protocol「机械面枚举的锚点纪律」要求枚举指向注册表锚点、不得硬编码手维护名册——正则形态兜底 + 登记表扩展的互补关系须显式定义（A-7）；② `versioned-artifact-claim` 用自由事件型不入 pod-events schema（PROPOSAL 已声明，与 human-deputy 批同款克制）——「自由事件型」的命名/可发现性由谁保证；③ T7 落点三选一裁后，撰写约定的单一权威在哪、其他两落点是否要加指回引用防漂移 | 登记表/正则双源与 ARQ-260827-contract-mech-gate-gap 同族病：两个真相源，漂移无声 |
| **GA-7 · fail-closed 不变量与失败模式** | ① T1-1 add/add 特判的 widen 方向（architect §review-bg 关注面 2）：特判只应 widen **指引文案**、不得 widen **放行**——命中集与放行集的补集关系不变式（登记表外的版本化物如 engine-patch 无 `.vN` 后缀形态，须退回通用 blocked fail-closed）；② T5 解析面（关注面 5）：Findings 是 review-packet prose 六列（integrator.md:42），Basis 自由文本无 schema——机械匹配对未识别形态**放行还是拦**的方向声明必须显式裁（A-8；「放行为默认、只拦可识别形态」与 fail-closed 姿态相反，须 adjudicator 显式裁而非默认带过）；③ kept-blocked / Step 0 不吞（见 §6 canaries C3/C4） | 本批所有改法的安全性论证都建立在「默认 blocked / 红方向不变」上；任何一处 widen 放行 = 整批 riskTier 论证失效 |
| **GA-8 · 自动化保全 vs 人工在场张力（spec 轴 11 对偶）** | T4 ask = 每次消解现场命令弹 human 确认（architect 推荐 (a)）；T3 前置 2 = human 授权 eventId 在场。与「前端对齐 + 例外反选」原则（human 只在真正判断点出场，260831-gov-review-auto-merge-local-main 起）对撞：集成消解是「真正判断点」（每案冲突集不同、真需人裁）还是「被 ask 摩擦稀释的仪式门」？challenger 与 defender 对撞；defender 替自动化方辩（消解规则已预登记时人只是盖章？），取舍交拍板。不预设结论 | 关卡纪律的对偶面：六门以外默认 policy-as-code、不造第七人工门——ask 是把人拉回环路的机制，其正当性必须靠「每次冲突集真不同」论证，不能靠「多一道更保险」 |
| **GA-9 · 触契约判定（转门确认）** | 独立确认 sweep 表「契约面未触（零）」：T1–T7 目标文件集不含 `docs/contracts/**`；T5 只读消费 CDQ、不开写权（ARQ 红线：开写权会把契约门侵蚀成「谁发现谁改」）；V4 负断言（`grep -n "docs/contracts" .claude/dev-loop.settings.json` 零命中）可机证 | pass-oracle 第 4 关（触契约已转门）的确认项；challenger 须独立跑核，不采信 sweep 表自述 |
| **GA-10 · 验证集完备性 / 可证伪** | V1–V6 逐条判「可机判 + 真空绿先行」：① V2 两态夹具须**非空断言先行**（gate self-check 真空绿规约同款，pre-merge-escalation-gate.mjs :255 注释）；② T6 等价断言范围须**显式排除**「以 merge-driver argv 误判 changeId」语义差——spawn→import 消除的是 bug 非 spec 行为，无可证夹具，等价声明若宣称覆盖它即虚报（architect §review-bg 关注面 6）；③ V3 依赖 OQ3 裁断值，裁前不可执行；④ V6 e2e 模板 = schedule-reminder review-packet §Integration Probe 实测数据——模板数据与夹具环境的差异（真实仓 vs 临时仓）是否削弱断言力 | 验证集是 landing-verifier 的对照清单；含虚报或不可执行项 = 保真核验空转 |

---

## 3. 显式裁定项清单（adjudicator 裁定 / decision-packet 必须逐项给出裁断与归档位置）

| # | 裁定项 | 归属轴 |
|---|---|---|
| A-1 | **存疑点①**：integrator.md 正文新增「merge-chain execution」节（frontmatter 零改动）是否触「agent def 执行语义变更」的 lightweight 硬排除判据——显式裁并归档，作为后续同类申报的判据先例 | GA-3 |
| A-2 | **存疑点②**：T5 闸内新核验行的归档定性——「既有升级闸门内加项」（折进注范式）还是「新增 merge gate」——显式裁并归档 | GA-4 |
| A-3 | **Step 1b 同形双源**：T1-4「统一口径」改**两条路径**（approvalRefUnlock + 常规 Step 1b，消除两形）还是**只改豁免路径回退分支**（`findWorktree \|\| projectDir`，消恒空转、维持两形现状）——裁定前提 = 先裁 gate 的「git 在 cwd（worktree）求值、文件读在 --project-dir（主仓）求值」哪些是有意语义（architect 建议判据：source-ref 必须在 Pod worktree 求值、verdict/PROPOSAL 读主仓；现状两路径均已如此，缺口仅回退分支）。这是事实+设计双层面裁定：事实层（两形同形）可机判先结，设计层（改法范围）交裁 | GA-5 / GA-4 |
| A-4 | **OQ3（human 未裁，归评审期/decision packet）**：T4 形态三选一——architect 推荐 (a) ask 附两条诚实边界（条件语义承载在 T3 正文 / airtight 需 hook 留根另批）。裁断后两条边界注记须**回填 PROPOSAL T4 正文与留根清单**，不得只落 landing-summary | GA-5 / GA-8 |
| A-5 | **T7 落点三选一**：architect 推荐 acceptance-evidence.md「必要证据」段（:5-11）+ 反例节（:31-39）挂反例——裁断须同时定「被排除两落点是否需要指回引用」 | GA-1 / GA-6 |
| A-6 | **T7 工程门判据侧是否设门**：architect 候选 = 告警级 lint 夹具先行（RC-2 告警先例）、gate 化留根另裁——裁「本批不设新门」须与 GA-4 结论一致 | GA-4 / GA-10 |
| A-7 | **§10c 登记表与 `*.v[0-9]+.md` 正则的双源关系**：登记表为 authority + 正则为兜底，还是正则为 authority + 登记表为 extension？不一致时的赢家与发现通道 | GA-6 |
| A-8 | **T5 未识别形态的方向**：Basis 自由文本未匹配机械规则时**放行（只拦可识别形态）**——与 fail-closed 姿态相反，须显式裁；若裁放行，须同时裁「写作侧对齐约定」（B-4）作为补偿控制 | GA-7 |

---

## 4. 冻结场景（challenger 逐场景走；spec 对某场景「答案」缺失或与其余改法冲突 = finding，落 BLOCKING/SHOULD 阶梯）

**S-1 · 并行双 Pod 抢 v3 槽位全链演练（一致性 / 并发，ATAM）**【charter 必载】
- 触发：A 包 bump prompt v2→v3（含齿 X）→ 并 main；B 包同时 bump v2→v3（含齿 Y）→ driver 跑。
- 失效链：两包 launch 时均未查或未记 `versioned-artifact-claim`（B-1 机械消费方缺位）→ 双方各自落 v3 → add/add 5 冲突块 → 若 T1-1 特判未命中（登记表/正则漏登记该路径形态）→ 通用 blocked 交人随手解 → 单边消解 → 被丢侧 needle 齿偶然命中 → 假绿（schedule-reminder 实测先例：eventId `5697792c`「以先入 main 者为准」口径在本案恰产出假绿支）。
- 期望答案：全链各环节（launch 登记 → 后到 Pod 让位 → driver 特判 → 并集消解 → 双方齿复跑）的 owner、触发、失败归属逐一显式；特判未命中形态的退路 = 通用 blocked fail-closed（GA-7 补集不变式）；消解规则预登记与 §10c 的衔接点在哪一文件哪一步。

**S-2 · integrator 按新节消解但派单漏预登记（pre-mortem：幽灵授权）**【charter 必载】
- 触发：merge-driver abort + blocked，planner 派单指名 integrator，但派单制品**漏写**某冲突文件的消解规则，或引用的 eventId 锚不到 pod-events 对应事件。
- 失效链：integrator 按 T3 前置「缺一即停」停下回报 → 若 planner 派单格式无成文义务（B-3：planner.md 不在本批 manifest），停等变成两 agent 互相等待或 planner 口头补条件 → 消解口径回退「会话现商」= P1 病根（流程外知识 + 逐次人批）在**新成文的豁免口内复发**；反向失效链：integrator 自行补条件就地消解 = 越权，且 T4 ask 若被复合命令绕过则无机械拦。
- 期望答案：前置不满足的**每一条**（预登记缺失 / eventId 锚不到 / 分支不符）都有设计好的响应（停 + 回报给谁、超时谁来收口），不是「不会发生」；前置 1/2 的生产侧义务（planner 派单格式）有落点——哪怕落点在留根清单，也要有 forcing-function。

**S-3 · gate lib 分离后某自检语义漂移（可改性 / 等价性，pre-mortem）**【charter 必载】
- 触发：T6 把 `resolveHumanApproval` 等提取至 `.claude/scripts/lib/governance-gate-shared.mjs`，gate 薄壳化 + import.meta 守卫；merge-driver T1 spawn 复用改 import。
- 失效链：① spawn 与 import 的模块级状态差异（import 单例缓存 vs spawn 每次冷启）在某条核验路径上产生首次/后续调用行为差 → 五自检仍全绿（夹具未覆盖该路径）而生产调用语义已漂；② gate 顶层副作用（env 读取、argv 解析、`--human-approval-ref` FATAL exit :377-380）残留在 lib 侧，merge-driver import 时带上 gate 的 argv 语义 = ARQ-260907-gate-lib-script-separation 证据原文的「语义耦合字面复用」换个方向复发；③ self-check case（:256 起）双路径复跑只证 gate 侧等价，merge-driver import 侧无等价夹具。
- 期望答案：等价断言的**覆盖范围声明**（含显式排除项：changeId 误判语义差，GA-10②）+ import 侧的入口不变量（lib 不读 process.argv / 不 process.exit）有夹具或结构断言可证。

**S-4 · T7 重指清单误伤 VRT-E4 类天然版本无关守卫（pre-mortem：清点机械化误伤）**【charter 必载】
- 触发：T7② 全仓清点按 `grep -rn "openclaw-system-prompt.v[0-9]"` 执行，后续批机械化成 lint（A-6 告警级）。
- 失效链：VRT-E4 接线齿 `test_e4_system_prompt_wired_into_deploy_overlay`（:410，needle = 裸名 `SYSTEM_PROMPT_NAME` :117 + 无扩展名 carrier 文件）**天然版本无关**——若 lint 规则以「同文件/同常量块出现 `.vN` 字面」为信号（而非逐断言判接线 vs 落盘/措辞类别），裸名齿会被误报为耦合 → acceptance-author「重指」它 → 反而把天然健壮齿改脆（解析逻辑引入新断链面）；对称地，P2-A6（:584-587，needle 字面 :585）**现属假绿**，若被类别判成「落盘守卫」则漏改。
- 期望答案：T7 条款正文的「接线 vs 落盘/措辞」二分判据可操作（撰写时注释标注类别——该标注对 lint 可读吗）；清单表 8 实例 + 清单外防误伤段（`test_schedule_reminder.acceptance.py:294` `PROMPT_CARRIER_V3` 是受害侧、不在清单）的分类判据逐条可复核。

**S-5 · ask 门的绕道面与覆盖盲区（安全 / 隔离，ATAM）**
- 触发：消解现场用 `cd <dir> && git merge …` 复合命令；或经 node child_process 跑（merge-driver 自身走 execFileSync，:1143 先例——不经 Bash 工具权限层）。
- 失效链：T4 ask 前缀匹配不命中复合命令 → 「机械兜底」声称的「每次必有人在场」出现静默盲区；merge-driver 内部 git 操作天然不经权限层（architect §T4 事实 4 已自认）→ ask 只覆盖手动路径。
- 期望答案：T4 的保护主张被诚实限定在「手动 Bash 路径的 best-effort」并在 PROPOSAL 回填（A-4）；残余风险有主（T3 正文纪律承载 + hook 留根），不是无声豁口。

**S-6 · 消解完成后只跑本包档，对端红漏检（完整性，pre-mortem）**
- 触发：并集消解后赶时间，integrator 只复跑本包冻结件 + 全档，跳过对端包档。
- 失效链：陷阱 (c) 实测——对端齿对本包改写的 `provider.config.yaml` 注释敏感（story-p3 侧 RED 先例）；对端红只在对端冻结齿显形 → 合并进 main 后才爆 → 违反「merge 段 fail-closed 无损」的整批前提。
- 期望答案：「复跑双方」是 prose 纪律（T2-2④ + T3 回报义务）——它的强制力等级是什么？无机械闸时，靠什么让它不退化为「建议」（回报义务入 review-packet 是唯一锚，consumed by 谁核？见 §5 B 系留空点）。

---

## 5. 双向消费方清单（每个改法产物的消费方 + 「匹配的消费方声明在哪」；**留空点 = challenger 的断链打点**）

| # | 产物（改法） | 生产者 | 声明的消费方 | 消费方声明位置 | 断链 / 留空点 |
|---|---|---|---|---|---|
| P-1 | T1-1 add/add 显式指引文案 + blocked | merge-driver 特判分支 | blocked 现场的 integrator / planner（消解执行者） | T2 §10a runbook + T3 新节消解执行段（引 §10c 规则） | 指引文案与 §10a/§10c 的**措辞口径**两处成文（driver 输出 vs runbook 步骤），漂移无同步机制——留空点 B-0 |
| P-2 | T1-3 `ensureGraphCacheSynced` 命名导出 | merge-driver | 人工 / integrator 消解（不再复刻三拍） | T2-2② runbook 引用 | 已声明；核 runbook 引用形态（文档引用 JS 导出名的可执行性——runbook 读者怎么「调」它，命令位在哪） |
| P-3 | T1-4 双源统一后的 gate 求值 | approvalRefUnlock / Step 1b | gate off-main 判定（source-ref 求值主体） | 无独立声明——语义藏在代码 | A-3 裁定项即消费方声明；裁后应有一句注释钉住「source-ref 在 worktree 求值是有意语义」 |
| P-4 | T2 §10c `versioned-artifact-claim` 事件 | Pod launch（planner 写 pod-events） | **后到 Pod**（见在先 claim 即从 vN+1 起编） | T2-1 | **B-1 留空点**：launch 面无机械消费方——「后到 Pod 见在先 claim」靠会话自查 pod-events，无 `--check-launch` 类机械挂点；claim 漏记 / 迟记（两包同日 launch 竞态）无报警 |
| P-5 | T2 §10c 登记表本体（`*.vN` 路径清单） | 「随批维护」（未名之） | T1-1 特判白名单 + 后续 Pod | T2-1 | **B-2 留空点**：维护 owner 与更新时机未声明（哪个角色、哪类批、merge 后回收 vN 槽位吗）；与 GA-6 双源问题叠加 |
| P-6 | T3 新节前置 1/2 的输入（逐文件预登记 + eventId） | planner 派单制品 | integrator（缺一即停的前置检查对象） | T3 前置段 | **B-3 留空点**：planner 侧派单格式义务无成文落点——planner.md 不在本批 landing-manifest（六文件），派单制品的必填字段（预登记格式、eventId 引用形态）无 consumer 侧可核对的声明面 |
| P-7 | T4 ask 条目 | dev-loop.settings.json | harness 权限层（Bash 工具调用） | T4 / architect §T4 | **B-5 留空点**：ask 的承载主体与 §10a:124 第 1 步的执行者身份未对齐声明——runbook 手动重放的执行者是 planner 还是 integrator？ask 弹给谁、弹出的确认由谁裁「授权引用在场」 |
| P-8 | T5 新核验行的输入 | review-packet Findings（integrator 写，六列 prose） | pre-merge-escalation-gate.mjs 新核验行 | T5 | **B-4 留空点**：Findings 写作侧（integrator.md:42 约定）与机械匹配规则无对齐约定——Basis 措辞变化（如「spec.md」vs「docs/spec.md」写法）直接改变匹配命中面；匹配规则单侧声明 |
| P-9 | T5 核验消费的 CDQ 行 / defer 记录 | human / architect（契约门流程写 CDQ） | gate（只读消费） | T5（eventId 锚形态） | defer/accepted-risk 的 **eventId 锚格式**未定义（pod-events 何种事件型、gate 怎么核「显式」）——留空点 B-6 |
| P-10 | T6 lib（governance-gate-shared.mjs） | gate 薄壳 import + merge-driver import | 本批两消费方已声明 | T6 | **B-7 留空点**：前向消费方——T4 留根的 airtight hook 形态若另批落地将 import 该 lib，dev-loop 平台 hook ↔ 治理 lib 的耦合声明缺位（architect §T4 排除 (b) 时已指出，留根未登记此耦合） |
| P-11 | T7① 撰写约定条款 | acceptance-evidence.md（A-5 裁后） | acceptance-author 撰写验收断言的当下 | T7 / architect §T7 落点论证 | 已声明（唯一消费者 = 撰写当下）；「撰写时注释标注类别」的标注格式对 A-6 lint 的可读性未定义（S-4 失效链） |
| P-12 | T7② 重指清单（8 实例 + 防误伤段） | architect（本批登记） | 逐件 human 授权流程 → acceptance-author 重指 | T7③ | **B-8 留空点**：授权流程的发起者 / 触发时机 / 记录落点（pod-events？ADR？逐件 decision packet？）未声明——「只登记不预批」之后，清单如何变成有主的工作项 |

留空点计数：B-0 至 B-8 共 **9 处**（B-0 为文案口径漂移面，B-1/B-2/B-3/B-4/B-5/B-6/B-7/B-8 为消费方声明缺位）。

---

## 6. regression-canaries（challenger 必须独立重新派生——没派生出来 = 辩论太浅判 RED；每条标来源）

> 本清单由 scenario-author 独立从 PROPOSAL / ARQ 原文 / 机制行号亲核派生，非照抄 architect §review-bg 候选清单（重叠处属独立收敛）。

1. **merge-driver 五自检全绿、夹具名单只增不改**：六 flag `--self-check-{retro-advisory,quick-checks,detect-changes,rebase-target,worktree-topology,merge-friction}` 入口 :1329-1349；T1 新增四夹具只增不改既有断言语义。来源：260907-gov-review-merge-friction 落地件五自检在册（ARQ-260907-merge-closeout-bookkeeping-gap 销账声明原文）。
2. **投影特判 --theirs 取侧方向零变**：current/ 投影冲突恒取被合并 Pod 侧（merge-driver.mjs:853-855，注释「rebase 语境 theirs = 被重放的 Pod 侧」）——T1-2 LF 归一只能加在 checkout **之后**，不得改变取侧方向、不得顺手改成按内容择优。来源：ARQ-260907-merge-closeout-bookkeeping-gap 候选②「被合并包赢，语义恒正确」+ 260907 批 T3 落地。
3. **Step 0 fail-closed 不吞**：白名单外变更在场即 warn + 保持 fail（:813-815），绝不吞非治理面变更。来源：同上 ARQ 候选① + pod-coordination.md §10b:136「白名单外变更不吞、维持 fail」。
4. **approvalRefUnlock fail-closed 方向**：核验失败 kept-blocked（:925-929 catch 块）、核验成功才 setPod ready-to-merge（:930）——T1-4 双源修复只消「回退分支恒空转」，不得把 kept-blocked 改成任何形式的解锁。来源：ARQ-260907-merge-driver-approval-state-desync 兑现承诺原文「核验失败 fail-closed 保持 blocked」。
5. **gate 例外清单六条判定语义零变**：头注 :10-19 + evaluate :136 起；T5 只新增核验行，不删不改既有条目；`--human-approval-ref` dev-loop 档限定（:377-380）不得扩到 gov 档；`hasContractGateEvidence: false` 向严默认（:249）不动。来源：260831-gov-review-auto-merge-local-main FROZEN 例外清单（DEV-GOVERNANCE §5 折进注尾行）。
6. **gate self-check 既有 case 全数通过、名单只增不减**：selfCheck :256 起（真空绿规约注释在 :255），含 T1 人批豁免 8 例。来源：gate 文件自述规约 + 260831 批验证计划。
7. **settings deny 面结构不变**：dev-loop.settings.json permissions.deny 两契约条目（:17-20 Edit/Write deny）+ hooks 挂点结构（:22 起）零删改；T4 只增 `ask` 数组。来源：AGENTS.md Hard Rules 契约冻结 + `.claude/rules/contracts.md` 结构红线（review-loop deny + bashDenyWrite 双拦）。
8. **§10a 既有 5 步 + §10b 簿记口径逐字零变**：pod-coordination.md:118-136 不重排——步骤编号是外部引用锚点（DEV-GOVERNANCE §4「怎么做」步骤编号同款纪律）；T2 只补增量（PROPOSAL T2-3 承诺原文）。来源：260907-gov-review-merge-friction T5 落地件 + PROPOSAL §6「销账动作执行面……不改写其证据链」。
9. **.gitattributes 现有两条零变**：`*.sh text eol=lf`（:6）+ `.planning/kidpal-mvp/progress.md merge=union`（:12）；本批无新增 attributes 需求（PROPOSAL 硬项 1b③）。来源：260907 批 closeout 落地 + PROPOSAL 硬项 1b 现跑核验。
10. **off-main BLOCK 语义与求值主体**：gate 通用 2 off-main 判定（evaluate 内 sourceReachableFromMain，BLOCK 文案「源 commit 已含在 main 内」:150-156 区）语义零变；双源修复后 `--source-ref HEAD` 的求值主体不得从 Pod worktree 改到主仓（求值主体变更 = 判定语义变更，不是修复）。来源：升级闸通用 2 条款 + architect §review-bg 1「现状两路径均已如此，缺口仅回退分支」亲核结论。
11. **hook 层零触碰**：deny-main-push.mjs / enforce-scope.mjs 行为零变（各自 `--self-check` 输出零变化）；T4 不以新 hook 实现条件语义（新 hook = 事实上的新闸，另批走 full）。来源：architect §T4 事实 2「新增一道 G3 hook = 事实上的新闸」+ ARQ-260907-gate-lib-script-separation「涉及承重闸脚本须独立治理批」纪律。
12. **scope-map 零改**：本批 landing-manifest 不含 `.claude/scope-map.json`——integrator「allow 10/10 全覆盖」的权限事实不由本批改法消解（消解走 T3 成文面 + T4 settings 面）；scope-map 被夹带改动 = 越面 landing。来源：PROPOSAL §3 sweep 表（scope-map 未列入）+ manifest 围栏块六路径。

---

## 7. 必载治理原则（challenger 判 GA-1/2/3/4/8 的根据）

1. **四元组（DEV-GOVERNANCE §6）**：描述任何治理面必须区分 Authority（规则事实源文件）/ Owner（谁提出维护）/ Writer（谁实际改）/ Approver（谁批）；architect 是开发治理 owner/writer 但**不自批**、不替代契约门与合并门。challenger 逐改法核四元组，writer≠approver 是底线。
2. **G1–G4 分层（§6 表）**：G1 指令根（AGENTS/CLAUDE/DEV-GOVERNANCE）与 G3 强制层（scope-map/settings/verify-gates/hooks/scripts）恒 full + human；G2 = `.claude/{agents,commands,references,rules,skills}/**`；G4 = changes/** 与队列文件（按流程分工）。本批 G2+G3 混合 + G4 簿记，approver 恒 human——charter 冻结该 profile，评审中任何改法把触面挪进 G1（如 A-5 裁 DEV-GOVERNANCE.md）须即时重报 moderator 复核 profile。
3. **自改递归（§6 硬条款 3）**：修改自身定义、权限边界、工具、write scope 或影响自身权限的强制层文件 → 必须 human-approved governance change、不得自批。本批的 review-loop agent 定义零触碰（architect-notes/PROPOSAL 均不动 challenger/defender 等 def）；若发现任何改法实质改到 review-loop 自身行为（如 T4 ask 拦到 review 角色的 Bash），按第五阀与存疑向严升级。
4. **第五阀（§6 硬条款 4）**：改 review-loop agent 自身定义时 re-review 须在新 canonical 生效前跑或由不加载被改 description 的会话跑——本批预告为零触碰，challenger 核实（V4 负断言族可扩一条：diff 不触 `.claude/agents/{challenger,defender,adjudicator,review-moderator,scenario-author,architect-reviewer}.md`）。
5. **六工程门不变式（§5）**：工程门 = 表内 6 行，新门裁决必须折进既有行 + 折进注留痕，不得另立枚举——T5 核验行与 T4 ask 的定性（A-2 / GA-4）以此为锚。
6. **landing 恒主线程 packet-bound + author≠judge**：architect 恒纯判官/起草、永不 landing；本批 architect-notes 候选稿是输入件不是 canonical——评审产出的 confirmed 改法经 decision packet → human 批 → architect 机械回灌 PROPOSAL（不夹带新设计）→ human 盖 FROZEN → 主线程 landing。
7. **前端对齐 + 例外反选（260831 起）**：human 只在真正判断点出场（立项 ack + 例外升级），其余 agent + 机械闸自动——GA-8 张力轴的评判基线；T4 ask 的正当性论证必须对着这条原则做，不能只说「多一道更保险」。
8. **存疑向严 + 锚点纪律**：判据适用存疑默认升 human；canonical 以枚举描述机械面必须指向注册表锚点（GA-6 依据）。

---

## 8. falsifiable-first 攻击优先级提示（给 challenger 的方向，不预设结论）

**第一梯队（可机判，先打）**：
1. **PROPOSAL 头注自相矛盾**：头注第 4 行同时载「ARQ-260908-versioned-guard-version-literal-coupling（OQ1 裁并批入内，T7 规约面）」与「明确不入本批：ARQ-260908-versioned-guard-version-literal-coupling（……是否并批立项见 OQ1）」两段互斥表述（后者系 OQ1 裁前残留未清）——同一文件对同一 ARQ 两个入批状态，packet-binding 完整性直接存疑。同类：头注 packet-binding 行「§2 改法集（T1–T6）」vs §2 实含 T7。
2. **A-3 事实层**：`approvalRefUnlock`（:919-920）与 Step 1b（:1137/:1142）「同形双源」亲核即判——challenger 须独立跑 `git grep -n "project-dir" .claude/scripts/merge-driver.mjs` 复核两路径形状，不被 ARQ 原文「常规路径无此形状」的失真对照带偏。
3. **GA-6 双源可证**：§10c 登记表（尚不存在）与 `*.v[0-9]+.md` 正则的关系——先 grep 全仓 `.v[0-9]` 落盘物清单（含 engine-patch / overlay 配置等无 `.vN` 后缀的版本化物），数出「正则漏、登记表须收」的实例，直接给 A-7 供弹。
4. **GA-9 机证**：V4 负断言（settings 零 docs/contracts 引用）+ manifest 六路径与 §3 sweep 表逐行对账（有无未列文件被 T6 lib 新增捎带——lib 是新增文件，sweep 表已列，核一致性）。

**第二梯队（夹具可证 / 半机判）**：
5. **GA-7 补集不变式**：构造「登记表外版本化物 add/add」反例（engine-patch 形态）打 T1-1 特判的漏拦退路是否真落通用 blocked（S-1 失效链）。
6. **T5 解析面**：构造 Basis 变体（`docs/spec.md` / `spec.md` / 「契约 §三」三种写法）打匹配策略的漏报面（S-2 同族的幽灵漏账方向）；对偶打误报面（judgement-call 类 finding 引了 spec 路径会不会被误拦）。
7. **GA-10 T6 等价范围**：逼 defender 给出等价断言的显式排除清单（changeId 误判语义差），拒绝则标 SHOULD。

**第三梯队（软判断，后打，取舍交拍板）**：
8. **GA-8 ask 张力**：S-5 覆盖盲区 + 「ask 弹给谁」（B-5）——defender 替自动化方辩，adjudicator 按「真正判断点」原则裁。
9. **T7 落点与类别判据可操作性**（A-5 / S-4）：VRT-E4 误伤链走一遍。
10. **S-6 双方复跑纪律的强制力等级**：prose 纪律无机械闸——是 ACCEPT-AS-RISK 记账还是补机械锚，交裁。

---

## 9. 已接受前提（human / 既有流程显式裁定，不重审取舍本身；可攻其一致性与实现含义）

| # | 前提 | 出处 |
|---|---|---|
| P1 | 并批 + 整批 Track A full-adversarial + human（OQ1） | PROPOSAL 头注 / §4 OQ1 |
| P2 | gate-lib 分离随批（OQ2） | PROPOSAL §4 OQ2 |
| P3 | escalation-cross-system-overfire 另批立项、本批 ack 后即启（OQ4） | PROPOSAL §4 OQ4 |
| P4 | OQ3（T4 形态三选一）**未裁**、归评审期/decision packet——不在前提清单内，见 A-4 | PROPOSAL §4 OQ3 |
| P5 | 非目标红线：不给 dev-loop 任何角色开 spec/CDQ/契约写权；不动 supervisor 判据与升级闸 cross-system 语义；不预批任何既有冻结件修改；pod-events schema 不动 | PROPOSAL §0 |
| P6 | 两条 260907 merge ARQ 已被 merge-friction 批完整落地（销账核销 2026-09-08），V5 机械核验随批 | PROPOSAL 头注销账段 |
| P7 | T7 既有守卫重指逐件 human 授权、本批只登记清单与规约 | PROPOSAL T7③ / §0 非目标 |

---

## 10. 主题支划分（供 moderator 逐支驱动辩论；pass-oracle 逐支过）

| 支 | 主题 | 覆盖轴 / 场景 / 裁定项 |
|---|---|---|
| T-A | 合并机械链 fail-closed 面（T1 全部 + V1/V6） | GA-5 / GA-7 / GA-10；S-1、S-5；A-3 |
| T-B | 消解执行者边界与跨面对齐（T2 + T3 + T4） | GA-3 / GA-5 / GA-8 / GA-2；S-2、S-6；A-1、A-4 |
| T-C | 升级闸演化与工程门边界（T5 + T6） | GA-4 / GA-7 / GA-10；S-3；A-2、A-8、B-4/B-6 |
| T-D | 版本化规约面与清点纪律（T7） | GA-1 / GA-2 / GA-6；S-4；A-5、A-6、A-7、B-2/B-8 |
| T-E | 全局簿记与消费方断链收口（§5 全清单 + V5） | GA-1 / GA-2 / GA-6 / GA-9；B 系留空点逐条走 |

---

## 11. 冻结声明

本 charter 一经写盘即对 challenger / defender **只读冻结**。任何一方要改轴 / 场景 / canary / 裁定项 / 前提，必须经 review-moderator 重新派 scenario-author；不得在辩论中自行解读本 charter 未载的判据。行号引用以冻结时亲核为准（基线见 §1 评审基线行）；辩论中如发现基线漂移，先报 moderator 核实再落 finding。

---

*charter 冻结：2026-09-08 · scenario-author · v1.0*
