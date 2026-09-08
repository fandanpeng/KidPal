# Defender 答辩 · R1 · 260908-gov-review-merge-chain-hardening

> defender（对抗 challenger findings-r1.md 13 条 + B 系 9 判）。全部 rebut/concede 证据按当日亲核（lib `c099c1b` 落地件在库、main 实现面与 charter 基线 `fae886a` 等价）。立场声明：我是辩论者不是审批者——可机判实证坐实的缺口痛快 CONCEDE 并给修向，不为辩而辩；对 prose-vs-机械闸的取舍（GA-8 张力域）专替克制侧辩，不无脑加管控。

## 一、Findings 逐条

**F1 · CONCEDE（BLOCKING）**
亲核实锤：`.claude/scripts/lib/governance-gate-shared.mjs` 已存在（518 行，`git log` 实证落地于 260907-gov-review-arq-governance-batch-2 commit `c099c1b`），文件头 `:1-16` 自述为 ARQ-097 结构门共享单源，消费方含 landing-fidelity-check / proposal-consolidation-check / taskpackage-required-check / devloop-artifact-check / closeout-check 等（B5 行注记 closeout-check 消费端在案）。PROPOSAL `:99`「（新增）」、`:116`「G3 新增」失实，且与 `:97`「既有先例同名 lib」同段自相矛盾；charter §8.4「lib 是新增文件」前提同失实（charter 冻结不可由我改，erratum 交 moderator）。**修向推荐：并入既有文件，不改名**。论据：① T6 要提取的 `resolveHumanApproval` 等现仅在 gate `:78-106`（lib 内 grep 零命中，未提取），本批动作为「向既有单源**追加**命名导出」，既有导出与函数体零改动——lib 头注 B5 行有「computeFrozenPin 纯新增 + isFrozen 函数体零改动（S8 条件）」的纯新增并入先例，六消费方按 import 语义自动继承、零回退；② 改名新 lib = 两个治理共享 lib 并存，「哪些函数归哪个 lib」即成新双源，GA-6 同族病自我复制；③ 配套义务（ challenger 指出的 sweep 扩面我接受）：PROPOSAL `:97/:99/:116` 措辞改「扩展既有 lib（追加导出，既有导出零改动）」，等价夹具 = 各消费方既有 self-check 全量复跑零回退 + T6 的 lib 入口不变量断言（见 F8 修向，一并覆盖 S-3 失效链②的顶层副作用残留面）。

**F2 · CONCEDE（BLOCKING）**
亲跑实证：`grep -n "docs/contracts" .claude/dev-loop.settings.json` 命中 3 行——`:7`（_doc 注记）、`:18-19`（deny 条目 `Edit(./docs/contracts/**)` / `Write(./docs/contracts/**)` 字面本身）。V4 负断言按字面永假，challenger 成立，charter GA-9「可机证」对 V4 此句不成立。**修向（改写文案候选）**：负断言改为「`permissions.deny` 两行逐字在位（grep -c `"Edit(\./docs/contracts/\*\*)"` = 1 且 `"Write(\./docs/contracts/\*\*)"` = 1）且 `permissions` 段无任何指向 `docs/contracts` 的 allow/ask 条目（T4 ask 条目仅 `Bash(git merge:*)` / `Bash(git rebase:*)` 两形）」——deny 面零删改语义被正向钉死，比「零命中」更强的机证力。

**F3 · CONCEDE（BLOCKING）**
亲核 `parseSection2AnchorLists`（lib `:345-423`）：T7 目标文件行（PROPOSAL `:105`）唯一 canonical 路径 `docs/ARCH-REVIEW-QUEUE.md` 的注记「（versioned-guard 行销账，随批 landing）」不含「迁出」、不含「触发条件|条件」（`:416-417` 两个裁剪分支均不命中）→ 入并集；`compareAnchorSets`（`:425-432`）双向差 → onlyInSection2 非空 → `aligned=false`；消费点实证 landing-fidelity-check.mjs `:307/:312`。V5（PROPOSAL `:140`）明令 landing 时结移该文件 = 它必然在批 → manifest（`:12-19` 六路径）必缺 → 机判必红。challenger 成立。**修向**：FROZEN 前 manifest 补 `docs/ARCH-REVIEW-QUEUE.md` 行——销账文件与 T7 落点三选一无关、无论 A-5 裁谁它都确定在批，可直接补，不依赖裁断；不推荐给锚行加「条件」注记（滥用条件锚裁剪机制粉饰对账，方向违背 fail-loud）。

**F4 · CONCEDE（SHOULD）**
逐字核实：PROPOSAL `:57` = `## 2. 改法（T1…T6）`，与 `:6` packet-binding「T1–T7」及 `:101` `### T7` 两个口径；全文件 grep「T1…T6 / T1–T6 / T1-T6」仅此一处残留（头注互斥两处 challenger 亦确认已修净）。修向：标题改「T1…T7」，一处即净。

**F5 · CONCEDE（SHOULD）**
亲核机制：gate off-main 判定在 `:151-160`（`sourceReachableFromMain` `:46-58`，`ctx.source` = `--source-ref` 值或 fallback `revParse(execFn,'HEAD')` `:223`，execFn 的 cwd = spawn cwd）；若采纳「两者恒用 projectDir」（PROPOSAL `:64`），cwd 落主仓 → HEAD=main HEAD → `source-on-main` → off-main BLOCK 恒命中（`:154-155`）——challenger 的机制链正确，且 charter canary 10 明文「求值主体不得从 Pod worktree 改到主仓」。我方减罪事实仅一条：PROPOSAL `:64` 候选括号内已有半句警示「按 gate 的 off-main 判定语义取正确者」，非完全裸列；且 charter A-3 的裁定选项空间（改两形 vs 只改回退分支）本就不含此侧。但「半句暗示可能取正确者」与「该选项可先验排除」不等价，FROZEN 前留着就是误采纳引线。**修向**：删除「或两者恒用 projectDir」，或原位标注「已被 canary 10 + off-main 语义先验排除（cwd 落主仓 → HEAD=main → off-main BLOCK 恒命中，gate :151-160）」。

**F6 · CONCEDE（SHOULD，轻）**
V1-③（PROPOSAL `:136`）「cwd ≡ --project-dir 解析主体」确实预设 A-3 统一侧。事实层亲核（merge-driver `:919-921` 与 `:1137/:1142`，同 challenger §四）支持 architect §review-bg 1 判据指向的「维持两形、只改回退分支」侧——若裁该侧，cwd 与 --project-dir 是有意异主体，断言文字不可实现。charter GA-10 已为 V3 标注 OQ3 依赖，漏了 V1-③ 的 A-3 依赖，属实。**修向**：V1-③ 改条件化回填——裁「统一两形」则保留现文；裁「只改回退分支」则断言改为「approvalRefUnlock 与 Step 1b 同形（source-ref 在 worktree cwd 求值、文件读走 `--project-dir` 主仓）+ 回退分支消除后 worktree 解析失败显式 errorExit 而非静默落主仓」。归属 = A-3 裁断后回填（decision packet 义务，与 V3 同款机制）。

**F7 · CONCEDE（SHOULD，轻）**
我方辩护事实：S-2 期望的「停 + 回报给谁」在 T3 候选正文已在场（architect-notes §T3 `:19`「前置不满足时停下回报 planner / 升级 human，不自行补条件、不就地改写」）——回报对象 = planner、升级口 = human；反向失效链（就地越权）被「不自行补条件、不就地改写」逐字钉死；结构性 forcing-function 亦在场（缺一即停 = 消解不发生 = blocked Pod 恒卡 board 可见 + merge 链停转，代价由 planner 侧承受，非无声绕过）。但 challenger 打的点是「生产侧义务（planner 派单格式）成文落点」——planner.md 不在 manifest、留根清单（`:143-150`）六条无一涉及，charter S-2 明文「哪怕落点在留根清单也要有」——引不出原文反驳，CONCEDE。**修向**：留根清单增一行「planner 派单格式义务：派单制品必填字段（逐文件消解规则预登记格式 + human 授权 eventId 引用形态 + 停等超时收口人），归下一批 planner.md 修订或首批 T3 演练实测后承载」。不扩本批 manifest（扩批须 moderator 复核，收益不抵）。

**F8 · CONCEDE（SHOULD，轻）**
charter GA-10② 是冻结判据，明文要求等价断言显式排除「以 merge-driver argv 误判 changeId」语义差；architect §review-bg 6（architect-notes `:133`）已自我指出，PROPOSAL V2（`:137`）确实未落排除句。无可辩。**修向**：V2 补一句「等价断言范围**显式排除** spawn→import 消除的 changeId 误判语义差（消除的是 bug 非 spec 行为，无可证夹具）；以 lib 入口不变量结构断言替代覆盖：governance-gate-shared.mjs 不读 `process.argv`、不 `process.exit`（grep/AST 可机判）」——同时满足 S-3 期望答案的「import 侧入口不变量有夹具或结构断言」。

**F9 · REBUT（定性）/ CONCEDE（清单本体）**
REBUT 部分：「T2-1 初始清单只有单路径」的表述漏了同一行已有的注记——PROPOSAL `:71` 原文「登记表随批维护（当前清单 = `server/deploy/openclaw-system-prompt.vN.md`，**同形态风险 = engine-patch / overlay 配置**）」——同形态风险显式在册，非无视。且「S-1 假绿链在该族原样复发」的严重性定性过重：engine-patch 形态 add/add 落通用 blocked = fail-closed（与批前同水平、不恶化），假绿链要闭合还差「交人随手解→单边消解」一步，而 §10a runbook + T3 已把「禁单边、取并集、复跑双方」成文——残余风险低于批前，非「原样复发」。CONCEDE 部分：亲核 census 复跑属实（server/deploy 下正则不命中的版本化物：`engine-patch/`（connection.py、kidpal_display_strip.py）、`openclaw-patch/`（7 文件）、`OPENCLAW_SYSTEM_PROMPT_CARRIER`（无扩展名）、`provider.config.yaml`、`apply-xiaozhi-overlay.sh` 等 ≥5 类）；若 A-7 裁「登记表为 authority + 初始清单 = 实测超集」，初始清单确实未达标。**修向**：T2-1 初始清单扩至实测超集（上列五类逐条入册）+ V1-① 补反例夹具「engine-patch 形态 add/add → 通用 blocked（非特判、非放行）」——把 GA-7 补集不变式从声明变机证。归属批内。

**F10 · CONCEDE（COULD）**
逐字核实：PROPOSAL `:72`「§10a runbook 补三条」下实列 ①②③④ 四条，且 T3（`:79`）「T2 ④ 同款清单」按四条序引用——计数笔误属实，canary 8 相邻文件增量口径应精确。修向：「补三条」改「补四条」，一处即净。

**F11 · CONFIRMED-NO-IMPACT（charter 侧注记，对 PROPOSAL 零修向）**
亲核属实：enforce-scope.mjs 无 `--self-check` 入口（grep 零命中），仅 deny-main-push.mjs `:494` 有。这是 charter canary 11 措辞失配（enforce-scope 无自检出口可核），不是被审对象的缺口——canary 意图（hook 层零触碰）不受影响，本批 manifest 与 T4 均不含 hooks 面，按「文件零 diff」执行即可。charter 冻结，erratum 交 moderator 注记，不入 decision-packet 修向。

**F12 · CONCEDE（COULD，轻）**
T7「撰写时注释标注类别」（PROPOSAL `:103`）格式未钉属实；VRT-E4 误伤防线全押自由散文注释的脆弱性判断成立。但强制力归属要说清：A-6（charter `:53`）本就把「告警级 lint 夹具先行 / gate 化留根另裁 / 本批是否设门」列为显式裁定项——若 A-6 裁本批不做 lint，标注只需人类可读，格式钉死归 lint 化批；若裁 lint 夹具随批，格式须在本批钉。**修向**：A-6 裁定时**一并**裁最小标注格式（建议固定 token 形态如 `<!-- guard-class: wiring | landing | wording -->`，正则可判、人类可读、不与 prose 冲突）；不阻塞本批其余改法。归属 decision packet 一并裁。

## 二、B 系逐条

**B-0 · 轻CONCEDE（半断链，challenger 自判半）**
三处引用锚在场：T3 候选正文「消解执行（按 pod-coordination.md §10a runbook + §10c 版本化制品规则）」（architect-notes `:34`）、driver 文案与 T3 逐字同口径（「取并集 + 复跑**双方** carrier 齿并留 exit code」，PROPOSAL `:61` = §T3 `:37`）、V1-① 夹具钉 driver 文案形态。但「半句同文案」靠 FROZEN 时刻对齐、后续批改 runbook 不核 driver = 漂移面真实。修向：T2 §10c 条款内加一行互指维护注记「本节为版本化制品消解口径单一权威；merge-driver 特判指引文案与 integrator.md 消解段为引用方，改本节口径须同批核两引用」（manifest 内文件加注记，零新增文件）。

**B-1 · REBUT（「真断链」定性不成立）**
T2-1 已显式声明消费方与消费规则：`versioned-artifact-claim` 由 planner 于 Pod launch 时写入 pod-events，消费方 = **后到 Pod**，规则 = 「见在先 claim 即从 vN+1 起编或等前包合并后重取基线」（PROPOSAL `:71`）——owner（planner）、触发（launch）、让位规则三者成文，charter P-4 留空点写的也只是「launch 面无**机械**消费方」，即缺 enforcement 挂点而非消费方声明缺位。这属 prose-vs-机械强制力分层（GA-8 域），不是断链。替克制侧辩三点：① 兜底链 fail-closed 在场——claim 漏记的后果是两包撞槽 → T1-1 特判 blocked + 显式指引 → 预登记消解 → 并集，**不假绿**，代价 = 多付一次消解成本，属效率损失非安全破口；② 加机械挂点 = 扩 portfolio-reconcile 或新 hook，本批 manifest 均不含，夹带即越面（canary 11/12 域）；③ 本仓对「确定性机械化」有成文的观测毕业先例——pod-coordination §9 watcher 纪律「便宜档，不建 daemon」「升级只换触发器」「毕业条件量化任一满足即评估升级」（`:104-116`）——claim 挂点正该走同款路径：先跑 prose，实测漏记率/竞态事件达毕业条件再机械化。修向（非认缺陷，是登记）：留根清单增一行「versioned-artifact-claim 机械消费挂点（portfolio-reconcile `--check-claim` 形态候选）按 §9 毕业条件观测后批」。

**B-2 · CONCEDE（真缺口）**
「随批维护」四字确实未名 owner、更新时机、vN 回收三件事，charter GA-2② 已点名，引不出原文反驳。修向：T2-1 补三短句——owner = 批主线程（触碰登记表所列形态的批，landing 时随批更新登记表，与 landing-manifest 流程绑定）；更新触发 = 批版本化制品路径集与登记表的 diff 非空；vN 回收 = 不做机械回收，槽位由 forward-bump 语义由后版覆盖（若实测需要回收另行立项）。归属批内。

**B-3 · CONCEDE（= F7，同修向）**
见 F7。留根清单增行承载 planner 派单格式义务；停 + 回报响应与结构性 forcing 的在场事实一并记档。

**B-4 · REBUT（裁定项待裁状态被误读为缺陷）**
Basis 匹配规则的写作侧对齐不是 PROPOSAL 缺口，是 charter 自己设计的 A-8 裁定流程的内置闭合路径——charter A-8 原文：「若裁放行，须同时裁『写作侧对齐约定』（B-4）作为补偿控制」（charter `:55`）；decision-packet 必须逐项裁 A-8（charter §3 开头明令），裁断即闭合。「真断链」定性把「裁定项的依赖顺序」当成「成文缺口」——A-8 未裁前写作侧无从对齐，恰恰说明这必须在裁断时一起落，而非 PROPOSAL 漏写。附批内承载点：A-8 裁断后 T5 落地时在 gate 核验行注释钉 Basis 匹配形态（前缀/正则 + 两态夹具各一），V2 两态夹具（PROPOSAL `:137` 已列）即对账点。

**B-5 · REBUT（执行者身份三处在场，「未名」不成立）**
① sweep 表（PROPOSAL `:112`）pod-coordination.md 行的声明消费方 = 「planner / integrator / human 消解现场」——执行者身份在 sweep 层已声明；② integrator 分支由 T3 候选正文完全钉死（触发条件 = merge-driver abort + blocked 且 planner 派单指名，architect-notes `:19`）；§10a:124 步骤的执行者 = 消解现场会话，两执行者场景分别承载于 T3（被指名 integrator）与 §10a（planner 主线程现场，`git merge` 放行前默认路径，pod-coordination `:70`）；③ ask 弹给谁：harness 权限层 ask 的唯一接收方 = 交互会话的 human 操作者——dev-loop 会话本就是 human 启动监督的会话（dev-loop.settings.json `:6` `claude --settings … --agent planner`），「谁裁授权在场」= 弹窗 human 本身核对 eventId。三面合看，B-5 指控的「ask 弹给谁 / 执行者是谁无声明」不成立；残余只是 runbook 行文无主语的风格问题，不值得修。

**B-6 · CONCEDE（真缺口）**
defer eventId 锚格式未定义属实：gate 现仅核 `human-decision` / `approve-escalated-merge` 五要素（gate `:100-104`），defer/accepted-risk 的 decision 值形态缺位；architect-notes 起草注记（`:51`）自己把形态钉死归到「T4/T5 的机械核验面」而 T5 正没落。修向：T5 条款补最小形态声明——「defer / accepted-risk 记录 = pod-events `human-decision` 事件，payload.decision ∈ {defer-contract-change, accept-risk}（枚举值交 decision packet 裁），核验复用 `resolveHumanApproval` 五要素、decision 值参数化为上列」。归属批内。

**B-7 · 轻CONCEDE（半断链，challenger 自判半）**
architect 已自我指出耦合（architect-notes `:70` 排除 (b) 时「hook 内核验授权事件要么复刻五要素逻辑（与 T6 刚提取的 lib 形成新耦合点）、要么 import 治理 lib（把 dev-loop 平台 hook 与治理 lib 钉死）」），PROPOSAL 留根清单未登记属实。修向：A-4 裁断回填时（charter A-4 原文本就要求两条边界注记回填留根清单）在 hook 留根行内并入一句「另批 hook 若核授权事件，import governance-gate-shared.mjs（与 gate / merge-driver 同源）或复刻五要素——形态另批裁，耦合声明随该批 landing 归档」。不单独立项，随 A-4 回填零成本承载。

**B-8 · REBUT（「无主工作项」定性）/ 轻CONCEDE（队列化挂载）**
REBUT：重指流程已有完整仓内先例——CHR-E3 首例重指 = human 裁 `c54bbcde`（architect-notes §T7 表 #1 在册、载改判缘由），记录落点 = pod-events human-decision + commit，发起 = human 逐件授权——charter §9 P7 已把「逐件 human 授权、本批只登记」钉为已裁前提，challenger 亦不攻取舍本身；「清单如何变成有主的工作项」的 owner（architect 登记 + human 逐件授权）与记录形态（先例在案）并非空白。轻CONCEDE：触发时机与队列化确实未与 ARCH-REVIEW-QUEUE 挂钩。修向：T7③ 落地时（或 V5 结移动 ARCH-REVIEW-QUEUE 的同一动作里）增一条排队行引用重指清单，行内注明「发起 = 下一批触碰对应冻结件时（对齐 P7）或 human 专项批」——ARCH-REVIEW-QUEUE.md 本就在 T7 目标文件行内（PROPOSAL `:105`），零新增文件。

## 三、CONCEDE 汇总表

| 条目 | verdict | 修向 | 归属 |
|---|---|---|---|
| F1 | CONCEDE（BLOCKING） | 并入既有 lib（追加导出、既有导出零改动）；PROPOSAL :97/:99/:116 措辞改「扩展既有」；六消费方 self-check 复跑零回退 | 批内 |
| F2 | CONCEDE（BLOCKING） | V4 负断言改写：deny 两行逐字在位 + permissions 无 contracts 相关 allow/ask 条目 | 批内 |
| F3 | CONCEDE（BLOCKING） | manifest 补 `docs/ARCH-REVIEW-QUEUE.md`（与 A-5 裁断无关，可直接补）；不加「条件」注记粉饰 | 批内 |
| F4 | CONCEDE（SHOULD） | `:57` 标题改 T1…T7 | 批内 |
| F5 | CONCEDE（SHOULD） | 删「或两者恒用 projectDir」或标注已被 canary 10 + off-main 语义先验排除 | 批内 |
| F6 | CONCEDE（轻） | V1-③ 条件化，按 A-3 裁断回填断言文字 | A-3 裁后回填 |
| F7 / B-3 | CONCEDE（轻） | 留根清单增行：planner 派单格式义务（预登记格式 + eventId 形态 + 超时收口） | 留根 |
| F8 | CONCEDE（轻） | V2 补显式排除 changeId 误判差 + lib 入口不变量结构断言（不读 process.argv / 不 process.exit） | 批内 |
| F9 | REBUT（定性）+ CONCEDE（清单） | T2-1 初始清单扩实测超集（五类）+ V1-① 补 engine-patch 反例夹具 | 批内 |
| F10 | CONCEDE（COULD） | 「补三条」改「补四条」 | 批内 |
| F11 | CONFIRMED-NO-IMPACT | 无 PROPOSAL 修向；charter erratum 交 moderator | charter erratum |
| F12 | CONCEDE（轻） | A-6 裁定时一并钉标注格式（`guard-class` 固定 token 形态） | decision packet |
| B-0 | 轻CONCEDE | §10c 加「单一权威 + 引用方同批核」互指注记 | 批内 |
| B-1 | REBUT | 非断链；挂点按 §9 毕业条件留根观测（留根增一行） | 留根 |
| B-2 | CONCEDE | T2-1 补 owner / 更新触发 / vN 回收三句 | 批内 |
| B-4 | REBUT | A-8 裁断时同裁写作侧对齐约定（charter 内置路径），裁后 T5 注释钉匹配形态 | A-8 裁断闭合 |
| B-5 | REBUT | 无修向（执行者身份三处在场：sweep 表 :112 / T3 / ask 弹窗机制） | — |
| B-6 | CONCEDE | T5 补 defer/accepted-risk 最小形态声明（human-decision + decision 枚举 + 五要素参数化） | 批内 |
| B-7 | 轻CONCEDE | A-4 回填时并入 hook↔lib 耦合声明一句 | A-4 回填 |
| B-8 | REBUT（定性）+ 轻CONCEDE（队列化） | ARCH-REVIEW-QUEUE 排队行引用重指清单（零新增文件） | 批内 |

计数：F 系 12 条 = REBUT 全额 0 + REBUT/CONCEDE 部分各半 1（F9）+ 确认零影响 1（F11）+ CONCEDE 10（其中 4 条为轻量文案/回填级）；B 系 9 条 = REBUT 3（B-1/B-4/B-5）+ 部分各半 2（B-0/B-8）+ CONCEDE 4（B-2/B-3/B-6/B-7）。

## 四、交 decision-packet 的张力点

1. **F1 修向二选一（并入 vs 改名）**：我推荐并入既有 `governance-gate-shared.mjs`（追加导出、B5 纯新增先例、避免双 lib 双源），adjudicator 裁；无论裁哪侧，PROPOSAL 措辞与 sweep 必须在 FROZEN 前修正。
2. **A-3 联动（F5/F6）**：事实层证据（gate `:151-160` + merge-driver `:919-921` / `:1137/:1142`）一致支持「维持两形、只改回退分支」；V1-③ 断言文字须按裁断回填，裁断时一并清掉 PROPOSAL `:64` 的自违候选。
3. **B-1 / GA-8 张力**：versioned-artifact-claim 的机械消费挂点 vs prose 纪律 + fail-closed 兜底——我顶克制侧（§9 毕业条件先例：先 prose、观测后机械化），不预设结论，交拍板。
4. **F9 / A-7 联动**：登记表 authority + 初始清单 = 实测超集 + engine-patch 反例夹具，A-7 裁「登记表为 authority」时三者须一并裁。

*defender · 2026-09-08 · 基线亲核：lib 落地件 c099c1b 在库、实现面与 charter 基线 fae886a 等价*
