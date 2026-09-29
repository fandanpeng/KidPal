# Review Charter · 260929-gov-review-agent-def-alignment

> **冻结声明（2026-09-29）**：本 charter 由 scenario-author 冻结，为本次 governance profile（Track A · full-adversarial）对抗 review 的覆盖判据。challenger / defender 对本 charter 只读；改动须经 review-moderator 重新派发 scenario-author。被审对象 = `delivery/changes/arch/260929-gov-review-agent-def-alignment/PROPOSAL.md`（DRAFT v0.2，human 立项 ack 已获）；本 charter 对被审对象只读，不改 PROPOSAL、不改任何 G1/G2/G3 canonical、不触 `docs/contracts/**`。
>
> **裁决 oracle**：governance profile pass-oracle 六关（`.claude/references/arch-review-protocol.md`「Governance profile pass-oracle」节）——判据覆盖、finding 全结、高敏项标 human、触契约已转门、canaries 全中、decision-packet 完整。

---

## 0. spec 类型判定与选轴理由

**类型判定：governance lane · G2+G3 mixed · 纯治理面变更**。触面 = 12 个 agent def（G2）+ `.claude/scope-map.json`（G3）+ `.claude/references/task-intake-and-slicing.md`（G2 reference）；spec 面（`docs/requirements.md` / `docs/architecture.md` 文件本体）与契约面（`docs/contracts/**` 文件本体）声称零触碰。不属于 arch-scenarios §四四类（技术重 / 领域重 / 内核数据 / 公司级）之任一——因此场景不从该表直取，而是按 arch-scenarios §三 pre-mortem 的「治理跨面不一致」条目 + protocol governance 判据定制；rubric 轴全部经「治理转译」后使用（转译规则在各轴条目注明）。

**批次敏感点（challenger 开局必核）**：批次混含 `architect.md`（self-def，prompt-authoring 与 authoring-agents-skills 双 D-C 硬排除对象）与 4 个 review-loop def（review-moderator / adjudicator / defender / architect-reviewer，第五阀硬排除）+ G3 scope-map——机检硬排除承袭成立（PROPOSAL 头部已自认 Track A），challenger 不必重打路由，但须核验第五阀的**执行形态**（见 SC-5）。

## 1. 适用治理判据轴（governance oracle 关 1，六条全冻结）

1. **authority / owner / writer / approver 一致性**：T1–T8 每一项的目标文件逐一归层——哪个文件是该面的 authority、writer 是否 architect（DEV-GOVERNANCE §6 硬条款 2：agent 定义只归 architect 起草）、approver 是否 human（OQ4 必答即此轴的 G3 腿）。challenger 须逐 T 项核对本批起草者身份与 DEV-GOVERNANCE §6 表一致。
2. **G1/G2/G3 归属**：PROPOSAL §3 影响面表把 12 def 记 G2、scope-map 记 G3、hooks/scripts 记「未触」——challenger 须复核「未触」枚举为真（尤其 T5 删 s2Exploratory 字段句是否牵动任何脚本 / verify-gates 判据；见留空点 B3）。
3. **自改递归**：T7 目标集含 `architect.md`（自改递归主体）——DEV-GOVERNANCE §6 硬条款 3 要求 human-approved governance change。本批已是 Track A + human 门，形态符合；challenger 须核验 human 触点（立项 ack + OQ4/OQ5）**在场且不可被 agent 门替代**，且 architect 未以任何形态自批 / 自冻 / 自 landing。
4. **agent · command · scope-map · hooks · settings 跨面对齐**：本批触 agent + scope-map + references 三面；command / settings / hooks 声称未触——challenger 须验证未触面确实无同构残留、无联动缺失（见留空点 B2、B4——这是本批最容易空过的一轴）。
5. **业务门 / 工程门边界**：PROPOSAL 非目标声明「不改任何门判据、不新增任何 gate」——challenger 须双向核：正向，T1–T8 无一项新增门或改判据；反向，T7 清理未把门句 / 权限句 / 升级句当噪音删（HQ-1 判据：删掉是否改变谁能做什么、何时必须停、哪个门裁断）；T8 透镜行是撰写指引不是新门。
6. **是否触契约、需否转契约门**：OQ3 动的是 scope-map 里 architect 对 `docs/contracts/**` 的**写域**，不是契约文件本体——「动契约写域 ≠ 动契约」判定是否成立、OQ3 是否构成 spec↔contract drift、契约门预审流程是否因此受影响，challenger 须给出显式论证（见留空点 B7），不得默认「零触碰」了事。

## 2. rubric 适用轴（治理转译；说明挑 / 不挑）

**挑（6 条，均转译）**：

- **轴 4 单一事实源 / 无漂移**（直用）——本批 T1/T4/T5 正是漂移类：spec 面路径漂移、影子路径写法、上仓死条文。challenger 须打「改后是否引入新漂移」而非只验旧漂移清除。
- **轴 1 双向声明**（转译为双向消费方清单）——治理版 expose/consumer = 被改件与其消费方（钩子、机检、派发白名单）的匹配；以 §4 清单承载，留空处即断链点。
- **轴 11 AI-native / 自动化保全**（直用）——T5 删死条文 vs 保留改写、T8 加透镜行 vs 流程步承载、agent 存在性机检缺位是否本批 policy-as-code 化（PROPOSAL §6 留根「交 /loop-retro 或本批评审提级」表述含糊，challenger 须追问归属）。
- **轴 9 不变量 & 失败模式**（转译为 pre-mortem）——以 SC-1/SC-2/SC-4 承载。
- **轴 0 范围 / 层级越权**（转译为批内范围纪律）——OQ3「顺带收窄」是范围蔓延还是合理同车？留根清单（§6）各条是否把该批做的事推给了不存在的后续批次（幽灵 DEFER）？与轴 7 合并核。
- **轴 7 DEFER 纪律**（转译为留根清单核验）——§6 五条留根逐条核：有没有 owner + 触发条件 + 落点（story-author 观察位有触发；「agent 存在性机检缺位……交 /loop-retro 或本批评审提级」无明确 owner 与触发——弱项，challenger 须打）。

**不挑（6 条，说明理由）**：轴 2（相位/依赖）——治理文本变更无跨批次相位依赖；轴 3（命名/词汇冲突）——本批不改命名体系；轴 5（数据所有权/Tier）——无数据面；轴 6（门/编排正确性）——六业务门与开票门语境不适用本仓治理批（工程门边界已由判据轴 5 承载）；轴 8（完整性/非空过）——验证集可证伪性已由判据轴 5 反向腿 + SC-4 承载，不单列稀释注意力；轴 10（安全/权限隔离）——权限面由 G3 归属轴 + SC-1 承载，不重复设轴。

## 3. 冻结场景（ATAM / pre-mortem / 主线 handoff，六场景全冻结）

- **SC-1 · 写域换路径后钩子行为变化（ATAM 一致性/可用性）**：T2 四处换路径 + OQ3 收窄后，逐一回答：integrator docs-sync 写 `docs/requirements.md` / `docs/architecture.md` → enforce-scope **放行**（这是本批唯一声称修复的机械实害，须正向验证而非只 grep 零命中）；adjudicator.deny 换路径后对现行及未来 docs 根文件的拦截面是否与语义声明一致；architect（若 OQ3 裁收窄）不再能写 `docs/contracts/**` 且其契约预审合法写面仍有落点。
- **SC-2 · def 清理后路由信号丢失（pre-mortem 漂移腐蚀）**：T3 改 builder description、T7 清理 12 def 正文后，route-regression-check 绿但实际派发走错——该机检只断言 fixture（`.claude/route-fixtures/routing.json`）内信号，不在 fixture 里的消歧句（「不是契约审批者」类防误路由句）被当噪音删除时**机检不可捕获**。倒推设计因：PROPOSAL T7 底线只锚 prompt-authoring HQ-1，**未锚 authoring-agents-skills 第 2 条「保留路由承重信号」**——challenger 须打这条判据缺口（已补载进 §5 必载原则）。
- **SC-3 · 透镜行引入的 token / 歧义代价（ATAM 成本）**：T8 两行 Leading Words 进 acceptance-author / portfolio-planner 常驻上下文；风险 = 与既有流程步语义重复诱导照抄、措辞含机器消费形态时与门不同形。OQ5 human 裁定为前置；验证 = 复跑两 lint 不劣化 + 反证清单（planner/builder/fixer 不加）被独立复核。
- **SC-4 · manifest 与实际落地漂移（pre-mortem 治理跨面不一致）**：landing-manifest 15 锚中两个条件锚（OQ1(b) 的 `uiux-author.md` 新增、OQ2 的 task-intake 联动）按裁断裁剪前后，manifest↔实际落地双向一致；`landing-fidelity-check.mjs` 的 claimed·planned-but-not-landed 硬失败是否覆盖「manifest 有、没落」与「落了、manifest 没有」两侧；packet-binding（FROZEN PROPOSAL + manifest + landing-summary）三点互证。
- **SC-5 · 自改递归 / 第五阀执行形态（pre-mortem 治理自指）**：批内 architect.md 与 4 个 review-loop def 被改——对它们改动的 re-review / landing 核验须由**不加载被改 description 的独立会话 / harness** 跑，或在新改动 canonical landing 生效前跑（DEV-GOVERNANCE §6 硬条款 4）；defender 若要为本批辩护，其自身 def 在被改清单内——独立性如何保全须显式回答。
- **SC-6 · 主线 handoff（治理七步闭环逐 handoff 双向验证）**：立项 ack（已获）→ charter 冻结（本件）→ challenger↔defender 对抗 → adjudicator 裁决 → governance pass-oracle 六关 → decision-packet → human 逐项批 OQ1–OQ5 → architect 回灌 PROPOSAL 定稿（不夹带新设计）→ human 盖 FROZEN → review-moderator 一致性核验 + `proposal-consolidation-check.mjs` → 主线程 packet-bound landing → landing-verifier 独立核验 → retro（趁热、先于合并）→ `changes-index.mjs --upsert` 钉 INDEX。逐 handoff 验进/出两侧：上游产出是否被下游声明消费、下游期望是否被上游声明供给。

## 4. 双向消费方清单（每个被改件的消费方 + 声明落点；**留空处 = challenger 优先断链点**）

| 被改件（T 项） | 改动方向（expose 侧） | 声明的消费方 | 消费方声明 / 机制落点 | 状态 |
|---|---|---|---|---|
| architect.md（T1/T7） | spec 路径表述 + 溯源注记清理 | 主线程 dispatch（起草 subagent 派发） | `.claude/agents/architect.md` frontmatter description + route-regression-check | 已声明 |
| review-moderator.md（T1/T7） | spec profile 落地面表述 + 注记清理 | `/arch-change` 编排派发 | route-regression-check fixtures + 该 def 自身 When-to-Use | 已声明 |
| architect-reviewer.md（T1/T7） | 输出示例路径 + 注记清理 | moderator 派 eligibility 四裁断 | route-regression-check fixtures | 已声明 |
| builder.md（T3/T5/T7） | description 包边界重写（模型级路由信号）+ S2 删 | planner Pod Agent() 白名单派发 | route-regression-check 断言白名单不退化 | 已声明；**留空 B2-邻**：description 重写后 fixture 内信号与文本是否仍逐字对得上须核 |
| challenger.md（T4） | 影子路径统一 `docs/contracts/**` | `/arch-change` 对抗派发 | route-regression-check fixtures | 已声明 |
| fixer.md / integrator.md（T5/T7） | S2/K8 死条文删 + 注记清理 | planner 派发 + integrator docs-sync 写域 | route-regression-check + enforce-scope（经 scope-map） | 已声明 |
| planner.md（T5/T6/T7） | s2Exploratory 字段句删 + uiux-author 摘引用 + 注记清理 | Pod 内 Step 6b 派发 + frontmatter Agent() 白名单 | route-regression-check fixtures | **留空 B2**：`.claude/route-fixtures/routing.json` 是否含 uiux-author 期望、T6 摘引用后 fixtures 是否需同批同步——PROPOSAL 未提 fixtures 联动，未列 |
| acceptance-author.md（T6/T7/T8） | uiux-author 引用摘（:23）+ 注记清理 + 透镜行 | planner Step 7 派发（红基线起草） | route-regression-check + §5.9 grep | 透镜行消费方 = 该 agent 自身，无独立机检断言（仅 grep 形态检查）——**留空 B6-邻**：语义正确性无机械承载，靠 OQ5 + 人读 |
| portfolio-planner.md（T7/T8） | 注记清理 + 影响面机械辅证行 | portfolio intake / 拆包派发 | route-regression-check | 同上，透镜行无机检 |
| adjudicator.md（T7） | 散文型注记清理（卡点 D 实证引文） | review 收口派发 | route-regression-check | **留空 B6**：散文型注记无机检 pattern（census 窄 pattern 不覆盖），半机械断言靠人读——验证集第 8 条是否把 ARQ 散文型清单列为核对输入须核 |
| defender.md（T7） | heredoc 历史教训句清理 | 对抗派发 | route-regression-check | 同 B6 |
| landing-verifier.md（T7） | description 收短（363>320 warn） | 主线程核验派发 | lint-descriptions + route-regression-check 必留信号断言 | 已声明；收短后必留信号（packet-binding 消费提示等）是否保留须核 |
| scope-map.json 4 处（T2） | allow/deny 换路径 + OQ3 收窄 | enforce-scope.mjs（机判）+ 各 agent 写行为 | `.claude/hooks/enforce-scope.mjs:56-66`（universal.docsTopLevel 只管 docs 根新建、不授写权） | **留空 B1（最大断链点）**：验证集只有 `python -m json.tool` 解析 + grep 零命中——**没有「换路径后 integrator 真能写两个 spec 文件」的正向钩子行为验证**；换路径 ≠ 语义等价见 SC-1/C5 |
| task-intake-and-slicing.md（T5/T6 联动） | s2Exploratory 字段段删 + L2 行改 | planner / portfolio-planner intake 消费 | task-intake 自身为 SSOT | **留空 B3**：s2Exploratory 是否有脚本 / 闸（taskpackage-required-check 等）消费该字段——删除前未列脚本扫描 |
| review-moderator.allow 的 `docs/contracts/**`（T2 域外） | —（本批未处置） | — | — | **留空 B5**：OQ3 只处置 architect.allow 的契约写面，review-moderator.allow 同含 `docs/contracts/**` 却未入 OQ3 范围——为何不处理 / 留根在哪，PROPOSAL 未答 |
| OQ3 收窄的下游（T2 域外） | architect.allow 去 `docs/contracts/**` | architect 契约预审写落点 | gate evidence 骨架 `.claude/references/contract-change-evidence.md`（实际落 `delivery/changes/arch/**`） | **留空 B7**：收窄后 architect 契约预审各合法写面仍有落点的论证 PROPOSAL 未给出，challenger 须补全或打回 |
| 命令 / settings / hooks / rules 面（全部 T 项域外） | —（声称未触） | — | — | **留空 B4**：本批 sweep 只扫 `.claude/agents/` + scope-map；`.claude/{commands,references,rules,skills}` 与 settings 是否也有 `docs/systems`、`apps/sX-`、裸 `contracts/`、S2/K8 同构残留——未列全面扫描。这正是 T1 根因（260913 清扫漏 `.claude/agents`）的同构复发风险，见 C6 |

## 5. 必载蓝图原则（challenger 判据依据，全程载入）

1. **DEV-GOVERNANCE §6 四元组**：任何治理面区分 Authority（规则事实源文件）/ Owner（提出维护者）/ Writer（实际可改者）/ Approver（批准者）；architect 是开发治理 owner / writer 但**不自批**。
2. **G1–G4 分层**：G1 指令根（AGENTS/CLAUDE/DEV-GOVERNANCE）/ G2 角色与流程定义（`.claude/{agents,commands,references,rules,skills}`）/ G3 强制执行层（scope-map、settings、verify-gates、hooks、scripts）/ G4 证据与队列——各面 Approver 恒 human；本批 G2+G3 mixed。
3. **自改递归（§6 硬条款 3）**：修改自身定义、权限边界、工具、write scope 或影响自身权限的强制层文件 → 必须 human-approved governance change，不得自批、不得凭同一 agent 判断落地。
4. **第五阀（§6 硬条款 4）**：改 review-loop agent（review-moderator / scenario-author / challenger / defender / adjudicator / architect-reviewer）定义时，re-review 须在新改动 canonical landing 生效前跑、或由不加载被改 description 的独立会话 / harness 跑。
5. **landing 通道常态规则**：canonical landing 恒 = 主线程 packet-bound（landing-manifest 绑定 + landing-verifier 独立核验）；architect 恒判官 / 起草、永不 landing（本批 architect.md 自身在 T7 清单内——其「永不 landing」语义不得被 T7 清理弱化）。
6. **prompt-authoring Pruning 硬边界（HQ-1，human 裁 2026-07-24）**：权限边界句、角色约束句、authority/owner/writer/approver 声明、scope allow/deny、工程门/人工门、升级条件、must/never 级约束、契约/合并/验收铁律**不属 prunable**；判据 = 删掉是否改变谁能做什么、何时必须停、哪个门裁断、哪条行为被禁止。T7 的逐文件清理以此为底线。
7. **authoring-agents-skills description 纪律（本 charter 补载——PROPOSAL T7 只锚 HQ-1、漏此条，challenger 须按 SC-2 打）**：① description 只写 when-to-use + 职责边界 + 路由信号，不概括 workflow 正文；② **保留路由承重信号**（子 agent 名 / Agent() 白名单、入口命令、profile 名、调用时机、防误路由消歧句）——去 description 时不得丢；③ 字数预算 + **改 description 只动 description、逐字段隔离**（误伤 `name` 破 enforce-scope 匹配、误伤 `tools: Agent()` 破 dispatch 白名单）。

## 6. regression-canaries（六条；challenger 必须独立重新派生，漏则判浅 → RED）

- **C1 · 路径形态族谱**（来源：`.claude/references/path-form-family.md`；实证批 `delivery/changes/arch/260914-gov-review-delivery-restructure/`——路径批改最隐蔽一类〔点前缀粘连形〕18 处损伤、四轮评审 + V1–V6 全机检 + node --check 全漏，靠 landing-verifier 首轮 FAIL 才接住）。challenger 须派生：PROPOSAL §5 验证集的 grep 模式只覆盖**斜杠形**；批量换路径须按八类形态补子检（尤其引号分段形、转义形如 `docs\/systems`、相对链接形），且「清扫模式与验收判据必须是同一条模式的两次使用」（族谱 §四教训）——§5 的改法描述与验证 grep 是否同源须核。PROPOSAL §6 已留根「landing 前按族谱补一轮复扫」——challenger 验其是否升为硬前置而非可选项。
- **C2 · description 误伤 name/tools**（来源：`.claude/rules/authoring-agents-skills.md` 纪律 3——该纪律的由来即误伤 `name` 破 enforce-scope 匹配、误伤 `tools: Agent()` 破 dispatch 白名单）。challenger 须派生：T3 改 builder description、T7 收短 landing-verifier description 的 diff 须逐字段隔离核验，且改后 route-regression-check + lint-descriptions 复跑（PROPOSAL 验证集 6/7 已列——challenger 验其覆盖「逐字段」维度，非只跑脚本）。
- **C3 · 变形标记被当正形照抄**（来源：`docs/templates/PROPOSAL.template.md` §2 读法注 1；实证批 260920-gov-review-citation-anchor-residue——为绕开解析器加的变形标记只存在两批就被起草者当正形照抄，`--preflight` 三条锚行全落空）。challenger 须派生：T7 以 story-author / requirement-reviewer 为「目标形态」时，**参照系自身**是否含偶有变形 / 本仓特化残留会被批量照抄进 12 个 def（参照系本批不动〔非目标清单〕，但它是复制品的母版）；且本批新写入的任何示范性文本（T8 透镜行措辞）必须与门 / 消费方同形。
- **C4 · verifier 退役时的编制同步缺口（T6 同构先例）**（来源：ARQ-260923 追注「同构先例 = verifier 退役时的编制同步问题」+ DEV-GOVERNANCE §2 现行编制注——supervisor / verifier 定义 2026-07-25 退役、以 stage 身份存续）。challenger 须派生：编制引用与编制文件双轨维护、无存在性机检（route-regression-check 只断言白名单不退化、不查被引 agent 存在性——已核实该脚本只读 fixtures 与 def 文本）→ OQ1 裁 (a) 摘引用时，引用摘除须**三处全摘**（planner.md frontmatter Agent() 列表 + Step 6b、acceptance-author.md:23、task-intake L2 行）+ grep 兜底，否则正是本批标题要防的「半截形态」；「存在性机检缺位」留根（§6）的 owner / 触发含糊，challenger 须打（同轴 7 弱项）。
- **C5 · scope-map 换路径 ≠ 语义等价（收窄 ≠ 换语义）**（来源：ARQ-260929-agent-def-reality-drift 证据段对 enforce-scope.mjs:56-66 的实读——universal.docsTopLevel 只管 docs/ 根新建白名单、不授 agent 写权）。challenger 须派生：换路径须建「谁、写哪个文件、钩子放行 / 拦截」行为矩阵——① adjudicator.deny 从目录 glob `docs/systems/**` 换两个具体文件，在「未来 docs 根新增白名单文件」场景拦截面收窄（现状 deny 覆盖任何 systems 下文件，换后只覆盖两文件）；② integrator.allow 换后对 docs 根**其他**白名单文件（glossary.md、ADR.md、CONTRACT-DECISION-QUEUE.md 等）的覆盖与换前差异；③ 「deny 语义保持」这句 PROPOSAL 断言须被逐场景证伪或证实。PROPOSAL 验证集 5（json.tool 解析）不构成行为等价证据。
- **C6 · 清扫范围漏面的同构复发**（来源：T1 根因本身——260913 去层重构清扫范围没覆盖 `.claude/agents/**`，即本批要修的欠账成因）。challenger 须派生：本批 sweep 若同样只扫 agents + scope-map，则 `.claude/{commands,references,rules,skills,settings}` 的同构残留（docs/systems、apps/sX-、zod、裸 contracts/、S2/K8、changeId 注记）构成**与根因同构的复发**——留空 B4。打点：PROPOSAL §3 的复扫模式与 §5 验证集是否应扩至全 `.claude` 面（至少一次性 census 证明域外零命中或把命中入账）。

## 7. challenger 执行注记

- 开局 falsifiable-first：先打 §4 留空点 B1（正向钩子行为验证缺位——可机判）与 C5（行为矩阵），再打 B2/B3/B4（消费方联动缺列——可机判补扫），最后打软判断（OQ5 锚点集措辞、T8 语义重复风险）。
- 每条 finding 落到具体 file:line（PROPOSAL §2 硬项已声明行号在评审期须按「搬运引据视同新断言复证」纪律重核——challenger 复核行号时适用同一纪律）。
- 场景覆盖判定：§3 六场景逐一走过并在 findings 记录；canary 六条逐一独立重新派生（pass-oracle 关 5）。
- 结案：CONFIRMED 高敏项（scope-map 写域、architect.md、review-loop def）一律 HUMAN-RULED 进 decision-packet，不在 loop 内落 canonical（pass-oracle 关 3）。

---

*冻结：scenario-author · 2026-09-29 · 输入 = PROPOSAL v0.2 + 四条 ARQ + rubric/scenarios/protocol + DEV-GOVERNANCE + prompt-authoring + authoring-agents-skills + path-form-family + scope-map/enforce-scope/route-regression-check 实读核实。*
