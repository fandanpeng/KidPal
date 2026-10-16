# review · 260928-gov-review-requirement-face

> 判官评审记录（append-only）。R1 = 2026-09-28，architect（Track B 判官，作者 = 主线程，作者≠判官）。
> 评审对象：本目录 `PROPOSAL.md`（DRAFT v0.1，human 立项 ack 已于 2026-09-28 主线程会话给出，§4 OQ-1~4 裁定在册）。

## R1 - 2026-09-28 - architect

### 评审范围与亲核留痕

- 逐字亲读：PROPOSAL 全文；`docs/requirements.md:670-709`（§9 纪律行 + v1.30–v1.57 版本行）；`docs/templates/crq-item.template.md`；`docs/templates/PROPOSAL.template.md`；`docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md`；`docs/crq/gov/ARQ-260927-role-boundary-story-propagation.md`；`docs/crq/requirement/ARQ-260916-home-date-format-english.md`；`.claude/skills/write-story/SKILL.md`；`.claude/skills/gov-arq/SKILL.md`；`.claude/scope-map.json`（全文）；`.claude/hooks/enforce-scope.mjs`（全文）；`.claude/scripts/route-regression-check.mjs`（全文）；`delivery/changes/arch/260923-gov-review-arq-ssot-restructure/PROPOSAL.md:375-394`；`.claude/references/track-b-judge-brief.md`。
- 机械实跑：`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260928-gov-review-requirement-face/PROPOSAL.md` → `ok: true`，`anchorCount: 6`，anchorUnion 六路径与 landing-manifest 六行逐一吻合、无注记夹带路径 token；Grep `ARQ-260916`（截断形态）与 `ARQ-260916-home-date-format-english`（全 ID）全仓枚举（结果见 F1）；`grep -c "跳过 Track B" docs/requirements.md` = 52、`grep -c "^| v" docs/requirements.md` = 58（§0「50+ 条几乎每条」声明核）；`.claude/agents/` Glob 枚举 14 件、逐件取 `tools:` 行（先例形态核对，见 F4）。

### 硬项核验结论

1. **§2 五段固定式 + 目标文件锚清单**：T1–T5 五段（问题/证据/根因/改法/目标文件）齐全；改法标题均 `### T<n> ·` 形态；锚行 `目标文件：` 起头不加粗；preflight 实跑 ok，manifest ≡ 锚并集（六路径）成立。**过**。
2. **断言实证抽查**：requirements.md:677（§9 纪律行）、:684（v1.55 行尾「（跳过 Track B，如实记录）」）、:688（v1.51 行尾同）、crq-item.template.md:14（域表 REQ- 行，引文省略了 markdown 链接语法，见 F9）、260923 PROPOSAL.md:384（v0.4 不改名裁定 verbatim）、gov-arq SKILL.md:31（祖辈前缀放行句 verbatim）——均属实。**过**（一处引文格式差记 F9）。scope-map.json:102-126「review-agent allow 均含 gov-review token」不成立，记 F8。
3. **sweep UNCLASSIFIED=0 + 未触项证成**：分类表覆盖全部触面，无 UNCLASSIFIED；「scope-map 未触」的机制性核验成立（enforce-scope.mjs 对未登记 agent 明文 `if (!policy) process.exit(0)`，universal 三面红线照常生效，两新 agent 以 tools 白名单无 Write/Edit + 同会话 return 跑通），但证成文的先例引用失准且未披露「首批未登记 agent」事实，记 F4。**实质过、表述欠准**。
4. **landing-manifest ≡ 各 T 锚并集**：preflight 实跑吻合，六路径（T5 迁移只记迁入路径，与模板「迁移锚只记迁入路径」条款一致）。**过**。
5. **OQ 完备性**：OQ-1~4 已裁且与 human 立项对齐记录一致；OQ-5（档 2 review 留痕载体）恰当呈报未裁。无漏报的必裁 human 取舍点（F2 的 changeId 定义属设计澄清义务，判官级，不另立 OQ）。**过**。

### 实质评审（透镜 3/4/5/6）

- **三档判据与流程**：档 0/1/2 划分与各档流程主体成立；档 1「过即落稿 + §9 append 可事后否回」的轻量化与 human OQ-1 裁定一致。两处边界注记（跨 story 零语义清扫、NFR 数值微调）见 F7；档 1 牵连面义务缺口见 F6。
- **作者≠审者对偶闭环**：story-author 与 requirement-reviewer 互斥声明在册、双双无 Write、同会话 return 交付；与 architect advisory 不重叠（档 2 分工「能不能 / 准不准 / 要不要」三值清晰，对齐 architect 定义既有 advisory 形态）。**过**（档 2 派 architect 的入口措辞见 F10）。
- **与 ARQ-260927 互查咬合**：本批定稿载体（§9 版本行 + 定稿版本引用）与该条「story 投影对账」**不冲突、不重复**——该条治拆包链下游投影保真（acceptance/锚对账），本批治链最上游的定稿标记；ARQ-260927 建议 2① 若落地，恰可用本批「定稿版本」钉投影基准（互补）。**过**。
- **与 ARQ-260928 建议 2 覆盖关系**：PROPOSAL §6 已写明后续批只做建议 1/3/4（即建议 2 由本批 T3 承载），批内自洽；但承载 ARQ 条目本体无注记，跨批防重做缺一环，记 F5。

### Findings

**F1（BLOCKING）· V6 / §3 复扫的清扫模式错误：截断 pattern 撞 4 个无关同日 CRQ 条目，「零命中」不可达、字面执行会改写无关治理记录。**
证据（本判官 2026-09-28 全仓 Grep 实测）：
- V6（PROPOSAL.md:117）与 §3 复扫段（PROPOSAL.md:96）写的 pattern 是截断的 `ARQ-260916`，而全仓以此串开头的 ID 除本批目标 `ARQ-260916-home-date-format-english` 外还有 4 个**无关条目**：`ARQ-260916-unit-gate-archive-recursion`、`ARQ-260916-supervisor-auth-fp-author`、`ARQ-260916-acc-gate-archive-skew`、`ARQ-260916-server-mvp-teeth-sweep`。它们的引用散布在 `.claude/hooks/supervisor-verdict-gate.mjs`（:145/:202/:241/:735 注释）、`.claude/scripts/unit-run-check.mjs`（:34/:188）、`.claude/scripts/acceptance-run-check.mjs`（:176）、`.claude/scripts/crq-migrate.mjs`（:54/:55/:56/:63）、`server/pytest.ini:2`、`server/acceptance/archive/README.md:40`、`delivery/workstreams/**`、dev-lane 制品等多处——**「零命中」按字面永不可达**；§3 的「命中处逐个更新为 `REQ-260916`」若被字面执行，会把这 4 个无关条目的全部在册引用改写成 REQ- 前缀，等于静默篡改无关治理记录（其中 hooks/scripts 注释还是 G3 面——本批自证 G3 未触）。
- 即使改用全 ID `ARQ-260916-home-date-format-english`，V6 声明的两类排除（「本 PROPOSAL」「docs/crq/** 既有条目正文与 260923 批制品」）仍不覆盖实测在册的类外命中：`.claude/scripts/crq-migrate.mjs:32`（OVERRIDES 历史映射表，G3 面、本批不可触）、`delivery/changes/dev/260915-m1-n1b-p1-home-battery/`（review-packet.md:32/:51/:106/:117、docs-delta.md:9、device-round-record.md:39——dev-lane append-only 历史证据，不应改写）、`docs/crq/requirement/INDEX.md:24`（生成件，render 再生自然消解）。类外命中不清零，V6 作为一个 pre-commit 验证项无法绿。
- 这同时违反 §2 硬项「凡『全 / 均 / 无』量化声明 → 覆盖集合机械枚举」：PROPOSAL.md:96 断言「改名后旧前缀全仓残留 = 引用该条目的链接」未附机械枚举，且按其写出的 pattern 该断言为假。
修法方向：V6 与 §3 复扫段重写为——pattern = 全 ID `ARQ-260916-home-date-format-english`；起草期机械枚举命中清单并逐条归类（更新类：INDEX 经 render 再生；历史不改写类：260923 批制品、dev-lane / taskpackages / workstreams 过程制品、crq-migrate.mjs 一次性迁移脚本 OVERRIDES 表〔G3 不触、注记进 landing-summary〕）；销项定义 = 「类外零命中 + 命中清单全归类进 landing-summary」。本判官实测清单可直接复用（见上）。

**F2（BLOCKING）· 「changeId」铸造来源未定义，且与仓内既有 changeId 强语义撞名——却要经 T4 落成 requirements.md §9 的永久纪律。**
证据：T3 §4（PROPOSAL.md:62）「story 修订定稿的标记 = §9 版本行 + changeId（档 1/2 必带）」、T4（PROPOSAL.md:71）将「变更行必带 changeId」写进 §9 纪律引言；但档 1/档 2 流程（T3 §2 改法第 2 块）从头到尾**不产生任何批次 ID**——无 PROPOSAL、无强制 REQ- 条目登记（T3 §5 的 REQ- 域条目定位是「改需求的请求」待办账，非全量变更 intake）。仓内 changeId 另有强既定语义：TaskPackage 唯一标识、强制 `YYMMDD-slug` 形状（`.claude/references/task-intake-and-slicing.md:91-97` verbatim「`changeId` 是 TaskPackage 的唯一标识符，**强制 `YYMMDD-slug` 形状**」）。Pods / 未来会话读 §9「必带 changeId」时无从铸造、且大概率误读为 dev-loop changeId。定稿载体是本批目标效果的承重件（实现包「定稿版本引用」挂点、ARQ-260928 批 launchPreconditions 推广的基座），承载词未定义即冻结，等于把歧义钉进 canonical。
修法方向（二选一，写死在 T3 §4 + T4 措辞里）：(a) changeId := §9 版本号本行（如 `v1.58`）——零新铸造机制，推荐；(b) 档 1/2 强制先登记 REQ- 域条目、changeId := REQ- ID——则 T3 流程与 §5 纪律须同步改为全量 intake 口径（显著加重档 1，与轻量意图相悖，需回问 human）。

**F3（SHOULD）· V3 期望与 route-regression-check.mjs 实际行为不符（假覆盖）。**
证据：route-regression-check.mjs 的断言集完全由 fixture 驱动（源码 :16 `readFileSync(... 'route-fixtures', 'routing.json')`，三类循环遍历 `CFG.commandEntry / CFG.agentRequiredSignals / CFG.agentRequiredToolsAgents`）——story-author / requirement-reviewer 不在 fixture 内，命令对其**零断言**；V3（PROPOSAL.md:114）却预期「绿——新增两 agent 定义路由信号在位（orchestrator 派谁 / 入口 / 消歧句）」。命令会绿，但绿的不是它声称的内容（真空绿反例：验证项证不了它标称验证的东西）。修法方向：改写 V3 预期为「既有 agent 断言不退化（新 agent 无 fixture 断言）」；若真要机检新 agent 路由信号，须并更 `.claude/route-fixtures/routing.json`——该文件未进 §3 sweep 表，若选此路须补行登记。

**F4（SHOULD）· §3「未触」证成的先例引用失准，且未如实披露两新 agent = 全仓首批 scope-map 未登记 agent。**
证据：§3（PROPOSAL.md:93）「机检脚本均在既有 allow 面（只读 + node 脚本执行与 architect/triage 先例同形）」——(a) 仓内没有 triage agent 定义（triage 是 command + reference），先例引用失准；真正的同形先例是 `architect-reviewer.md:4` `tools: Read, Grep, Glob, Bash`（无 Write/Edit，全仓唯一无写工具的 agent 定义）。(b) 「Bash 限 `node .claude/scripts/spec-anchor-check.mjs`」（T1/T2）没有机检形态——tools 白名单是工具级不是命令级，该限制只能由 agent 正文纪律承载（architect 自身 Bash 纪律同此），证成应如实写「正文纪律级，非机检」。(c) 未披露的关键事实：enforce-scope.mjs 对未登记 agent 明文零 per-agent 约束（`if (!policy) process.exit(0); // 未登记的 agent：本钩子不强加任何约束`），本批两新 agent 将是**全仓 14+2 个 agent 中头两个未登记者**——写面纪律完全由 tools 白名单（无 Write/Edit）+ 正文承载；Bash 重定向写 `docs/requirements.md` 的残余面对**所有** agent 今天就开放（universal.bashDenyWrite 只守 contracts/.claude/taskpackages 三面），非本批新增弱化，但证成应写明这一强度等同（与 architect-reviewer 同级）而非笼统「既有 allow 面」。硬项「凡『比照先例』→ 引 file:line verbatim」在此项未兑现。修法方向：证成句改为引 architect-reviewer.md:4 为先例 + 披露未登记事实与残余面；不要求登记 scope-map（那会升 G3 触面，非必要）。

**F5（SHOULD）· 承载 ARQ（ARQ-260928）建议 2 的「已由本批承载」未回写条目——跨批互相以为对方做了的口子没关死。**
证据：PROPOSAL §6（PROPOSAL.md:122）写明后续批只做「该 ARQ 建议 1/3/4」，即建议 2（需求面比例化）由本批 T3 承载——批内自洽；但 canonical 账本条目 `docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md` 正文建议清单原样、无任何注记，后续批起手只读 ARQ 条目（CRQ 纪律：同根在既有条目追加注记不新立）时看到的建议 2 仍是「待做」。修法方向：本批加一小项（或并进 T5 落地动作）——对 ARQ-260928 正文 **append-only 追加一段注记**「建议 2 已由 260928-gov-review-requirement-face T3 承载（三档判据 + 流程 + 定稿载体）」，manifest 相应补该文件；不动既有行。

**F6（SHOULD）· 档 1 无牵连面自查义务——单 story 语义改动常需 §2/线框同步，§9 先例实证此为常态。**
证据：T1 起草动作③（PROPOSAL.md:42）把牵连面清单定为「档 2 增量」；T2 四查(d)（PROPOSAL.md:50）「牵连面完整……（档 2 适用）」。而 §9 先例中单 story 语义改动携带 §2 表格 / 线框 / §1 同步是常态：v1.48（S-F2.1 AC4 + §2.3 矩阵 + F2 线框清单三面同步）、v1.56（S-F3.2 扩释义 + §2.2 行 + WF-F3-01 同步）、v1.38（S-F3.2 AC2 改 + WF-F3-01 同步）均为**档 1 量级**的单 story 语义改动 + 跨节牵连。按现文这些走档 1 无牵连面清单、reviewer 也不查牵连面——漏同步风险恰是本批想治的「人肉顺序」病。修法方向：T3 判据补一句——「档 1 起草发现牵连面（§2 共享底座 / 线框 / §1 / §6 / §8 任一）即在修订稿携带牵连面清单；牵连面 ≥2 处或含 NFR/Epic 者升档 2」。

**F7（NIT）· 三档边界两处示例消歧。**
(a) 跨 story 但零语义的机械清扫（如全文章节术语统一）按现文落「跨 story 连锁」= 档 2 全流程（含 architect 可行性 + human 终拍），对零语义清扫明显过重；(b) NFR 数值微调恒档 2（「NFR 变动」不分语义变动 vs 数值定案）——可接受（NFR 值本属 human 实验拍板域），但判据各补一句示例可免执行期反复。非缺陷，向严方向的模糊，落 reference 时顺手补。

**F8（NIT）· scope-map.json:102-126「review-agent allow 均含 gov-review token」不成立。**
证据：:121-124 `architect-reviewer` 条目 `allow: []`、`deny: ["**"]`——不含该 token。「均」为假（硬项量化声明未机械核）。判官写槽事实由 architect 自身条目（:86-90，allow 含 `delivery/changes/arch/*gov-review-*/**`）支撑，本判官本次写入未被拦即实证。修法：改引 architect 条目行号。

**F9（NIT）· crq-item.template.md:14 引文非严格 verbatim。**
实际行为 `` [`requirements.md`](../requirements.md) ``（markdown 链接），PROPOSAL 引文写平文「requirements.md §9 记账」。语义无损，引文标注「verbatim」则应逐字。

**F10（NIT）· 档 2 派 architect 的入口不在 architect.md When to Use 清单内。**
architect.md 的 advisory 入口现有 =「planner 按 `[ARCH-ADVISORY 疑难分析档]` 派你，或 human 显式指定」；requirement-change.md 档 2 新增第三种 dispatch 来源（主线程按流程派）未经 architect.md 登记。本批不动 architect.md（自改递归敏感面，合理），建议 requirement-change.md 档 2 措辞对齐既有口径——「human 裁意图与取舍倾向时同步**指定** architect 做可行性判断（经主线程派发，对齐 architect advisory 形态）」——把派发权锚回 human 指定，免开未登记入口。

### 结论

verdict: BLOCK(F1,F2)

- F1（V6/复扫清扫模式错误）与 F2（changeId 未定义即入 canonical）为 BLOCKING：一个使 pre-commit 验证集按字面不可绿且字面执行有改写无关治理记录的实际危险，一个把未定义且撞名的承载词钉进 requirements.md §9 永久纪律。均须主线程修订 PROPOSAL 后送 R2。
- F3–F6（SHOULD）建议随 R2 一并修订；F7–F10（NIT）landing 前顺手修即可。
- 无升级 human 项：riskTier medium、review profile G2、Track B 轨道判定均成立；未发现该走 Track A 的判据命中（不触契约面、非跨系统结构性变更——requirements.md 仅两句纪律注记）。
- 认可项（R2 无需重证）：T1–T5 改法方向与各自根因对应成立；对偶设计（作者≠审者、architect 三值分工）闭环；与 ARQ-260927 不冲突不重复（互补）；T5 推翻 260923 v0.4 裁定有 human 2026-09-28 明示依据且 PROPOSAL 留痕，程序正当；preflight 形态全绿。

## R2 - 2026-09-28 - architect

### 评审范围

聚焦 R1 十项 findings 的 v0.2 修订到位性 + 修订是否引入新矛盾（轮次纪律；不做无的放矢的全量重核）。亲核：v0.2 全文逐字重读；preflight 复跑（`ok: true`，`anchorCount: 7`，anchorUnion 七路径与 manifest 七行逐一吻合、无 advisory）；`grep -c "260916" docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md` = 0（M3 证据）；scope-map.json:88/:102 亲核（两行均含 `gov-review` token，F8 修订引据属实）。

### R1 findings 处置核验

- **F1（BLOCKING）已解**：§3 复扫硬项（PROPOSAL.md:106）与 V6（:127）pattern 均改全 ID `ARQ-260916-home-date-format-english`，并明写「禁截断串——截断会撞 4 个无关同日 CRQ 条目……R1-F1 实测在案」；三归类覆盖 R1 实测全部命中（①活引用更新 / ②append-only 历史含 260923 批制品与 dev-lane 制品 / ③crq-migrate.mjs:32 G3 注记类）；销项可判。残余两处文字级问题记 M2/M3，不构成洞。
- **F2（BLOCKING）已解**：T3 §4（:63）「§9 版本行本身，版本号即 changeId（不另铸批次 ID……TaskPackage 已有强语义 changeId——task-intake-and-slicing.md:91-97，撞名禁用）」——采纳推荐 (a)，撞名依据引对；T4 句子（:72）同步「以版本号为 changeId（不另铸 ID）」，与 T6 注记口径一致。铸造来源闭环。
- **F3 已解**：V3（:124）如实注记 routing.json 驱动、新 agent 零断言的真空绿风险，替代保障（V2 + landing-verifier 人读）与 fixtures 补条留根（§6:135）成套。真空绿反照通过。
- **F4 已解**：§3 强制层行（:103）如实披露两 agent 为全仓首批 scope-map 未登记者、约束系正文纪律非机检（「约定级」定性诚实）、真同形先例改引 architect-reviewer.md:4、scope-map 登记留根 §6。披露不改变 profile（G3 文件面零改动这一事实未变）与 riskTier（medium 成立；约定级弱化已披露且有界——tools 无 Write + universal 三面红线仍在）。
- **F5 已解**：T6（:83-89）append-only 注记设计正确（只追加、内容与 R1 建议一致、留痕「建议 1/3/4 与退役安置仍归后续批」防重做口子关死）；目标文件进 manifest（七锚之一）；执行者 = 主线程（docs/crq 登记动作归主线程，repo-structure.md 权限表一致）。条目 owner=architect 不冲突——注记系 landing 动作非本判官自写。
- **F6 已解**：T3 判据（:60）升档规则补齐（档 1 发现牵连面即携带清单；牵连 ≥2 处或含 NFR/Epic 升档 2）。一处配套缺口记 M4。
- **F7 已解**：两例边界消歧入判据（跨 story 零语义清扫 = 档 0；NFR 数值微调 = 档 1，档 2 收窄为「NFR 新增/删除/阈值语义变更」）——与 R1 建议方向一致且更细。
- **F8 已解**：§2 硬项引据（:34）改「token 多处在册（scope-map.json:88/:102 等；architect-reviewer 条目 allow 为空系纯判官定位使然，判官写槽由 architect 自身条目 :86-90 支撑）」——三处行号亲核属实。
- **F9 已解**：crq-item.template.md:14 改标「内容引用……markdown 链接语法从略非逐字」，诚实标注。
- **F10 已解**：T3 档 2 措辞（:61）「出场由 human 指定——本批不改 architect.md 入口清单，将其入列归后续批」+ §6 留根（:140）——锚回既有 advisory 口径，不碰自改递归边界。

### 新引入问题（均 MINOR，landing 时顺手修，不阻断）

- **M1（MINOR）· V1 预期列文字陈旧**：V1（:122）仍写「anchorUnion 与 manifest 六路径逐一吻合」——T6 加入后实为七路径（preflight 实测 anchorCount 7）。机械门不看这句文字，但 FROZEN 制品内自相矛盾的计数该改。
- **M2（MINOR）· 销项措辞「①类外零命中」两读**：§3（:106）与 V6（:127）的销项公式「**销项 = ①类外零命中**」字面可读作「①类之外零命中」（则 ②③ 类命中不应存在，与括注矛盾）；实意 = 「三归类清单之外零命中」。括注（「①类全数更新、②③类逐条留痕归类」）已消歧，但 landing-verifier 执行 V6 时按字面会判不出——建议 landing 前改为「销项 = 三归类清单之外零命中」。
- **M3（MINOR）· ①类示例含不存在的命中**：§3 复扫段 ①类示例「如 ARQ-260928 条目正文关联句」——亲核 `grep -c 260916 docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md` = 0，该条目正文不引用本批目标 ID（T6 注记也不引）。示例系起草期未机械枚举的推想；按实测，T5+render 落地后 ①类实际≈空（INDEX 经再生自然消解、条目自身 id 随 T5 改）。无害（示例标「如」），但 V6 执行时不应期待该命中存在。
- **M4（MINOR）· T2 四查(d) 未随 F6 升档规则配套**：T2 四查(d)（:51）仍写「牵连面完整……（档 2 适用）」——F6 升档规则下，携带牵连面清单的档 1（牵连 = 1 处、未达升档线）也交了清单，reviewer 按现文不查它。落地写 requirement-reviewer.md 时把 (d) 扩为「档 2 及携带牵连面清单的档 1 适用」即可闭合。

### 结论

verdict: GREEN

- 两项 BLOCKING（F1 清扫模式、F2 changeId 铸造）均真解，SHOULD/NIT 六项全部到位且未引入新矛盾；T6 与 append-only 纪律、§3 新披露均不改变 riskTier medium / review profile（G3 文件面零触）判定；轨道维持 Track B。
- M1–M4 为 MINOR，主线程 landing 时顺手修（M2 的销项措辞与 M4 的 reviewer 查项扩围建议在标 FROZEN 前一并改进 PROPOSAL/落地文本；M1/M3 一行文字勘正）。无升级 human 项。
- 结构字段核对：landing-manifest 围栏块七行与 packet-id 行（T1–T6）均在场、preflight 实跑零 advisory——GREEN 前置结构义务满足。
