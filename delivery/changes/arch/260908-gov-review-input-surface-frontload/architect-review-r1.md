# architect-review-r1 · 260908-gov-review-input-surface-frontload

> **判官身份**：Track B 默认路由 architect 判官（R1 全面审），零上下文独立会话，只读全仓 canonical、只写本文件 + PROPOSAL §7 append。评审发起 2026-09-08，跨日收尾 2026-09-09。
> **评审对象**：`changes/arch/260908-gov-review-input-surface-frontload/PROPOSAL.md`（DRAFT v0.1，human 立项 ack 已给）。
> **纪律依据**：`.claude/references/track-b-judge-brief.md` + `.claude/references/arch-review-protocol.md`「双轨路由」节 + 本批派遣特有焦点清单。

---

## Verdict：NEEDS-WORK（打回）

**BLOCKING ×3（F1 / F3 / F7）· SHOULD ×7（F2 / F4 / F5 / F6 / F8 / F9 / F14）· COULD ×4（F10–F13）**。

一句话形态判断：方向与根因对应成立（三条根因各有承接、census 支撑「申报字段而非硬推导」的闸形态选择、G1/G2/G3 归类与 Track B 降档均站得住），但 §1 P1 遥测表按文中口径**不可复现**（静默排除了 260829-schedule-reminder 整包 + n 列口径混用未披露，且被排除包含有全数据集**最大**长尾），T4 内部存在两处**落地执行者无法同时满足的字面矛盾**（Step 8 硬前置 vs Step 9 brief 生成时点；acceptance-author `:46` 「句保持」 vs V5 负 grep 零命中）。三件 BLOCKING 修法皆为机械修订，不动批次方向，无需升级 human。

**归类裁断**：`docs/DEV-GOVERNANCE.md` = **G1 确认**（§6 表 `:212` 字面列名；batch-2 先例「标 G2 并声明 G1 为零」与 SSOT 表不符，不予沿用）。
**降档裁断**：**Track B 成立**（直接 Track A 两判据零命中；机检硬排除各条均标「Track A 触发时适用」；对照详见下「降档与归类复核」节）。
**升级 human 项**：无方向级升级项。**FROZEN 前置确认项**一件：OQ1/OQ2/OQ3 正文已按推荐 (a) 编入，FROZEN 前须 human 显式裁断落 §7/Status（S4 先例；主线程正在收口，不阻塞 R1）。

---

## A. 遥测可复现性（本批特有焦点 1）——结论：四个量级按文中命令不可复现，对账后归因成立但属未披露排除，F1 BLOCKING

**独立复现方法**：python 对 `.claude/taskpackages/*/pod-events.jsonl` ∪ `changes/dev/*/pod-events.jsonl`（14 个文件）按 eventId 去重聚合，与 PROPOSAL §1 P1 自述命令一致。

**实测 vs 声称**：

| 量 | PROPOSAL 声称 | 实测（全量去重） | 实测（排除 260829-schedule-reminder） |
|---|---|---|---|
| dispatch-completed 事件 | 72 | **91** | **72** ✓ |
| 包数 | 12 | **13** | **12** ✓ |
| token 精确 | 40 | **47** | **40** ✓ |
| 时间跨度 | 2026-08-31→09-08 | 08-31T18:48→09-08T07:53 | 08-31→09-08T00:29 ✓ |
| builder 行 | n=15 / 329min / 中位 12.8 | n=23 / 508.9 / 17.8 | 全事件 n=21；**durationMs 有效子集 n=15** / 328.8 / 12.8 ✓ |
| acceptance-author 行 | n=15 / 210 / 7.4 | n=23 / 285.9 / 8.4 | dur 有效 n=15 / 210.2 / 7.4 ✓ |
| integrator 行 | n=7 / 68 / 8.7 / 长尾「—」 | n=10 / 140.1 / 10.3 | dur 有效 n=7 / 68.2 / 8.7 ✓ |
| fixer 行 | n=3 / 16 / 5.4 | n=6 / 16.3 / 5.4 | dur 有效 n=3 / 16.3 / 5.4 ✓ |

**对账结论**（确定性归因，非猜测）：
1. 起草数据集 = 全量 **减去 260829-schedule-reminder 整包**（该包 19 条 dc 事件、7 条 token 精确，减法后 72/12/40 三量严丝合缝）。时间线支持「非故意」：该包遥测 2026-09-08T16:53 才随 pod merge（e8b73c0）进本分支，PROPOSAL 于 17:27 提交且 commit message 自述「前会话遗留入版本控制」——分析会话跑在 merge 前是合理解释。**但文中未 pin as-of commit/时刻、未声明排除**，按文内命令现跑即不可复现，判官按派遣指令「复现不上即按事实准确性打回」。
2. actor 表 n 列 = **durationMs 有效子集**计数（如 builder 全事件 21、dur 有效 15），与 headline 72（全事件口径）**混用且未披露**；planner（10 条）/verifier（4 条）/architect-reviewer（2 条）行整行未列也未说明取舍标准。
3. **实质后果**：被排除包持有全数据集最大长尾——builder `9de6ab28` schedule-reminder build **109.4min/226 tools**（大于表中 cited 的 `20241a2c` 69.3min）、`6121e6de` builder-verify 70.7min、integrator `f7ea4133` integrate **71.9min/248 tools**（integrator 行「长尾 —」在全量数据下为假）。§1 P1 是整批证据基座且被 V6 引为 pilot 对照基线，失真必须修。

**四条长尾 eventId 前 8 位逐一核验 ✓**（全部精确命中）：
- `20241a2c` = dispatch-completed / builder / 260829-story-p3 / phase=implementation / 69.33min / 109 tools ✓
- `cc79b2fb` = dispatch-completed / acceptance-author / 260829-session-logic / phase=acceptance / 61.17min / 84 tools / tokenUsage total=108585 status=exact ✓
- `1618c89d` = dispatch-completed / acceptance-author / 260829-story-p3 / phase=acceptance-freeze / 51.99min / 113 tools ✓
- `0ebaf461` = **friction** / actor=planner / 260829-story-p3 / frictionType=stall，summary「acceptance-author 18.7min/49tools 读完输入后在动笔前终止，磁盘零落件…」✓（与 P1 引文「对应 friction 事件 `0ebaf461`，stall 型」一致）

`260829-story-p3/pod-retro.md:11` verbatim 摘句亲核 ✓：「它烧了将近 19 分钟、几十次工具调用，把 spec/契约/先例读了个遍，然后回一句『Now I have all the inputs. Let me write the C-side file』就停了——磁盘零落件」（原文加粗「把 spec/契约/先例读了个遍」同在）。

**P3 附带核验**：`node .claude/scripts/pod-events-aggregate.mjs --change-id 260829-story-p3` 实测 eventsTotal=17 ✓（与声称一致；但「默认 current 包 story-p3」现已失真——current/ 指针现指 schedule-reminder，默认裸跑输出 eventsTotal=33。as-of 漂移同 F1 主题）。现输出仅 tokens/escalations/friction 三块、**无 per-actor durationMs/toolUses 分布列** ✓ P3 断言成立。

## B. P2 普查 spot-check（硬项 1b）——结论：三面核验两面精确、一面计数错（F2 SHOULD）；「19 个」口径与主线程「20 个目录」差额已归因

- **差额归因**：`ls -d .claude/taskpackages/*/` = 20 个目录 = 19 个包目录 + `current/` 指针目录；`current/taskpackage.json` 存在（内容 = 260829-schedule-reminder 副本），故 raw glob `*/taskpackage.json` 命中 **20 个文件**。PROPOSAL「19 个」= 排除 current/ 后的包数，口径成立但未注明排除 current/（F2 附带）。
- **双面计数**：按 subsystem 字符串含 firmware+server 判定，去重后实测 = **8/19**（char-stroke、hardware-validation、schedule-p2、schedule-reminder、smoke-e2e、story-p3、voice-roundtrip、device-ws-switch），非声称的 9/19。**9 的成因可确定性归因**：current/ 副本（=schedule-reminder，firmware+server）被重复计入。结论方向不受影响（动机包仍被双面谓词漏掉），但硬项 census 数字必须可复现（F2）。
- **动机包谓词 ✓**：`260907-g-relay-fix` taskpackage.json 实测 `subsystem="server"`、`rigor="lite"`，全字段递归扫描含 "firmware" 的字符串 = **[]（零命中）** ✓「纯路径/subsystem 谓词抓不住它」成立。
- **契约引用无判别力 ✓**：19/19 包 contractRefs/docRefs 均含 `docs/contracts/interface-contract.md` ✓ 实测复现。

## C. 动机包实况（g-relay-fix）——结论：全部引述逐字命中 ✓

- `changes/dev/260907-g-relay-fix/pod-retro.md:23` verbatim ✓：「跨端协议验收：acceptance-author 输入清单固定加『对端解析器源码路径』」（原行为「下次改进」列表第 1 条）。
- `:5` 段（「## 本 Pod 最有价值的教训」，正文 `:7`）✓：「builder 按冻结契约把 relay 帧包成 `display_relay` 自创类型——结构面全绿、离线 harness 全通，真机上被设备固件一句 `Unknown message type` 扔掉」「这轮靠翻 application.cc/mcp_server.cc 才发现数字 id、data 字符串化两个隐藏雷」「这轮是冒烟炸了才补的课」——PROPOSAL 转述与原文一致。
- T5 实证依据 notes 引文 ✓ verbatim：「本包以新建 Lite 包承载并如实标注」逐字在场；且 `rigor:"lite"` 字段本身直接机械佐证（比 notes 引文更强，建议 T5 落地时并引字段）。
- V2 前提 ✓：该包 refs 含 `docs/contracts/interface-contract.md#§1.5` 与 `#§三` 锚点（dry-run 锚点节行断言有料）；`git status --short .claude/taskpackages/260907-g-relay-fix/` 实测空输出（目录清洁，V2 的「不落盘断言」当前无污染项）。

## D. 五条→六条投影完备性——结论：消费面恰好四处、无漏网 ✓；头注替换句 verbatim 成立 ✓；第 1/4 条作 T3 模板成立 ✓

- **头注实文**：`docs/DEV-GOVERNANCE.md:291`（verbatim）：「> 成文：260907-gov-review-arq-governance-batch-2（T6）；承载 ARQ-260907-taskpackage-format-expectations-frontload。格式期望只活在闸的判定代码里、起草侧零成文 = 靠闸红反推再回派（跨 4 包 5 子项实证）。五条前置清单：」——T1 的承重替换句「…跨 4 包 5 子项实证）。六条前置清单：」与原文尾部严丝合缝 ✓。五条正文在 `:293-297`，第 5 条后追加第 6 条的落点成立 ✓。
- **消费面 grep 自跑**（pattern `五条前置清单\|五条中的`，全仓 *.md/*.mjs/*.json，排除参考树）：canonical 命中恰 = §3 表首行所列四处（`planner.md:109`、`portfolio-planner.md:58`、`acceptance-author.md:46`、`DEV-GOVERNANCE.md:291`）+ PROPOSAL 自身（改动集，非消费面）。**无漏网** ✓。宽 pattern `五条` 的其余命中均为异形同词或冻结历史证据，不需也不应改：`arch-review-protocol.md`「五条协议要点」（另一概念）、`pod-coordination.md:130`「消解纪律五条」（另一概念）、`DEV-GOVERNANCE.md:22`「五条设计思想」（另一概念）、`changes/dev/**` 各 pod-retro/docs-delta/review-packet 的「decisionCoverage 五条」「DEV-GOVERNANCE 五条」（append-only 历史证据，不回改）。注意 `builder.md:46` 引用同节但**不含**「五条」字样——T4 已将其纳入改动集（引用行同步），V5「四 def 各含 input-brief 引用」兜住该面 ✓。
- **第 1/4 条模板资格 ✓**：`:293`「Draft TaskPackage 必须带 mutationEvidence 字段骨架（空骨架也行，字段必须在场）」、`:296`「降级声明（不适用 + 理由）在 Draft TaskPackage 阶段写字段，不等到 verifier 红才补」——T3 的「骨架必在 + finalize 二选一」确与该两条同型，模板论证成立。
- **四方对象 ✓**：节头 `:289`「（planner / portfolio-planner / acceptance-author / builder 四方）」= T4 同一组角色。

## E. T3 时序与单源红利——结论：闸侧无鸡生蛋 ✓；单源红利两断言亲核成立 ✓；finalize 代理 = 既有约定 ✓；**但 T4 prose 侧 Step 8 硬前置与 Step 9 生成时点矛盾（F3 BLOCKING）**，in-flight 包过渡未成文（F4 SHOULD）

- **闸现读字段**：`taskpackage-required-check.mjs:163-169` 无条件要求 `acceptanceTestsFile` 等七数组非空 + 标量必填 + origin/DC 深校验。**「acceptanceTestsFile 非空」作 finalize 代理 = 既有约定**：planner.md Step 8（`:55`）「Finalize TaskPackage：验收文件、coverage marker、红基线、docs/security 标记缺一不可」+ 闸本来就只在验收文件在场时才可能绿。本批新造的只是把它用作闸内**分支条件**——自洽：Draft 态闸本来就红（acceptanceTestsFile 缺），T3.1 的 Draft 容忍分支只影响红名单内容不影响过/红结果；闸绿仅在 finalize 态可达，故 T3.2/T3.3 的 finalize 判定恒有定义。
- **T3.3 闸侧时序 ✓**：闸的实际调用点 = verifier scoped 轮（Step 10 起，verify-gates.json timing=scoped）与 closeout（Step 11，closeout-check.mjs `:309-315` 程序化传 canonical 路径）。两处均晚于 Step 9（builder brief 生成点），两份 brief 届时必在场——**闸层面不产生鸡生蛋**。PROPOSAL `:108` 括注只论证了 acceptance-author 侧，builder 侧经上述调用点枚举补证成立。
- **T4 prose 侧矛盾（F3）**：T4 一面把「两份 input-brief 在场」设为 **Step 8 finalize 硬前置**，一面把 builder brief 生成钉在 **Step 9 派发前置**——照字面时序执行的 planner 在 Step 8 永远等不到 builder brief（其唯一定义生成点在其后），规则跟随者死锁，只能靠临场发挥提前生成（= 冻结 def 内的即兴，正是要防的形态）。技术上双 brief 完全可在 Step 7 前一次产出：brief-inputs 只消费 contractRefs/docRefs/sourceRefs/crossEnd（Draft 即齐）+ 他包先例，不依赖 finalize-only 字段。修法一句话：钉死「Step 7 派发前双 brief 一并生成；Step 8 核在场；Step 9 只引用」。
- **单源红利亲核 ✓**：`dev-loop.md:229`（收口闸行）verbatim 含「**承重字段/decisionCoverage 校验由 closeout 相位程序化调用 `taskpackage-required-check` 单源承重**（F-04，本体不浅版重实现）」；closeout-check.mjs `:309` `const bearingGatePath = resolve(projectDir, '.claude', 'scripts', 'taskpackage-required-check.mjs')` 实证程序化调用；`verify-gates.json:32-34` = taskpackage-required 闸条目（id/kind/cmd 三行）——扩脚本判定、两处配置零改动即传导 ✓ 断言成立。
- **in-flight 过渡（F4）**：「存量零追溯：闸只读 current taskpackage，历史包不触发」对历史包为真，但 **landing 时在飞的 pod**（已过 Step 7/9、无 brief、无 crossEnd）是 current 包，下一轮 scoped verify 即撞 T3.3/T3.2 红。当下无在飞 pod（current/ 指向已 merge 的 schedule-reminder），风险窗口 = 现在至 landing 间新起的 pod。补救可行（planner 补跑 brief-inputs + 补填 crossEnd，脚本不依赖相位）但未成文——riskTier 证成句「存量包零追溯」应如实补一句过渡摩擦与补救动作，或设过渡豁免。

## F. T2 advisory 与 T3 硬红分工 + 派生物语义——结论：分工不重叠不抵消 ✓；WARN 宿主已指定 ✓；wire 节谓词未封闭（F5 SHOULD）；先例封顶判定源缺（F8 SHOULD）；行号防腐措辞过强（F11 COULD）；无锚点 refs 行为未定（F12 COULD）

- **分工核验**：T2 WARN 条件（refs 引 wire 节 ∧ counterparts 空 ∧ 无 exempt）与 T3.2 硬红（finalize ∧ counterparts 空 ∧ 无 exempt，**不限 wire 节引用**）不同相位、不同宽度：WARN 在 brief 生成期（Draft 即可触发，闸此时按 T3.1 容忍）提前提示，硬红在 finalize 后闸轮兜底全量包。二者不互相抵消（WARN 是早信号、RED 是晚执法）、不在同一相位重复告警。exempt 非空即抑制 WARN——「假 exempt」由 OQ3(a) 已声明的 integrator/loop-retro 事后复核承接，属已记账取舍，成立。
- **WARN 宿主 ✓**：planner 在 Step 7/9 前置 Bash 跑 brief-inputs，stderr 直接进 planner 会话输出——消费者与时点均有主（对照「无人消费的 advisory 等于无」及格）。但注意 WARN 的下游动作（复核后改填 counterparts 还是补 exempt 理由）只有「提示 planner 复核申报」一句，advisory 性质下可接受。
- **wire 节谓词（F5）**：`interface-contract.md` 实测节结构：§1.5 =「网关的职责」、§三 =「提醒推送与出队语义」，两节存在 ✓；g-relay-fix refs 恰引此二锚（谓词显然由动机包 refs 反推）。但 **§四「端云协议剖面（小智协议消息子集）」才是 wire 形状核心节**（mcp kidpal/display 载荷、E1 JSON 承载均在此定义），§1.2/§1.3（display 顶层结构/view 类型）同理在列——未来包只引 §四 时 WARN 静默不触发。且节号硬编码进 G3 脚本 = 与契约文档结构耦合，契约经契约门改节号后 WARN 静默失效（vacuous advisory）。修法：wire 节集合显式为脚本常量 + 注释钉判定源与维护义务（契约节号变更同批同步），并论证/扩充集合（至少评估 §四、§1.2/§1.3）。
- **T2① 影子契约面 ✓（带 F12）**：「路径+锚点+节标题」是指针不是正文，brief 明示非权威派生物（T6 证据属性）、按派发再生、OQ2 把真 digest 风险显式递延且不预授权——phase-1 封闭性论证成立。缺口：无锚点 refs（实测 g-relay-fix `contractRefs=["docs/contracts/interface-contract.md"]` 即裸路径）在 brief 中如何呈现未定——封闭清单对其退化为整文件引用，应标「无锚点（整文件）」并 advisory 提示 planner 补锚。
- **T2② 行号腐化（F11）**：「行号不落 taskpackage 库，防腐化」对 taskpackage.json 为真，但行号**落进 brief 且 brief Git-tracked**，同样随代码漂移。设计实际自洽——brief 是带生成时间戳、按派发再生的短命快照，权威窗口 = 本次派发消费期——但 PROPOSAL 未把这层快照语义说出来，「防腐化」措辞过强。修法：T2/T6 补一句 brief 行号快照语义（非长期事实源、消费时 symbol 已移则重新 grep）。
- **T2③ 先例封顶（F8）**：「同 subsystem 已 merged 包的 acceptanceTestsFile，按时间倒序封顶 3 个」三个判定源均未钉：**merged 判定源**（候选：`changes/dev/<id>/supervisor-verdict.json` verdict=APPROVE / pod-events merge-completed 事件 / `$(git-common-dir)/devloop/pods/<id>.json` board——board 不入版本控制、新 worktree 不可依赖，应排除）；**同 subsystem 匹配规则**（字符串相等则 firmware 包与 firmware+server 包互不为先例，token 化包含更合理，需钉）；**时间倒序的时间轴**（changeId 日期前缀 / git merge 时刻 / 文件 mtime）。脚本无法无追加决策实现。
- **T7 over-scan 生产侧（F14）**：OQ1(a) 扩枚举值后，**谁在什么条件下落 over-scan friction 事件未定义**——canon 扩型而无记账义务，仪表盘 over-scan 列可能恒空（与「无人消费的 advisory 等于无」同型：无生产者的枚举值等于无）。既有对照：stall 在 `:122` 有启发式触发注记。修法一句话：钉触发（如「子 agent 交付报告超读申报段非空时，planner 落一条 over-scan friction，summary 引申报行数」），同时把 T1 第 2 条激励措辞（「超读是常态动作，不申报才是问题」）与记账动作接上。

## G. 验证集反真空绿照——结论：V1/V2/V4 机械可判有判别力 ✓；V5 判别力不足（F6）+ 与 T4 字面矛盾（F7 BLOCKING）；V3 落点/清理/字段清单缺（F9）；V6 基线受 F1 传染 + 计数口径未钉（F13）

- **V1 ✓**：self-check exit 0 + 四路形状断言名 + fixture 非空断言先行——机械可判、有判别力（fixture 非空前置反照已内置）。
- **V2 ✓**：g-relay-fix refs 含 `#§1.5`/`#§三` 锚点（stdout 锚点节行断言有料）；该目录 git 实测清洁（scoped `git status --short` 空输出断言当前不被污染）。注意断言范围已 scoped 到该目录，规避了全仓脏项污染——写法合格。
- **V3（F9）**：负正两态设计对（exit 1 点名 crossEnd → 补骨架+两 brief+acceptanceTestsFile → exit 0；fixture 非空前置 ✓）。三缺：① **fixture 落点路径未指定**（闸对显式 argv 路径 `resolve(projectDir, explicit)`，绝对路径可用——仓外 temp 或本批 changes/arch 目录皆可，但须钉死并附**清理义务**，否则 fixture taskpackage.json 落进 `.claude/taskpackages/` 会污染未来的包数 census——本批 F2 正是 census 污染实例）；② 正态 fixture 须满足闸**全部既有必填**（changeId YYMMDD 前缀、origin/sourceType、七数组、budgets、docsOwner=integrator 等），不注明则执行者被 crossEnd 之外的红项绊住误判；③ 「两份 brief fixture 文件」的落点 = fixture taskpackage 同目录（T3.3 语义），应写明。
- **V4 ✓**：实测 story-p3 acceptance-author 有 4 条 dispatch（113/8/18/19 tools，中位 18.5），「toolUses 中位与 pod-events 原值一致」是可判别的一致性核（113-tools 派发在场使 spot-check 可辨）✓。
- **V5（F6 + F7）**：① `grep -c "六条前置清单" ≥1` 只证计数词换了，**证不了第 6 条正文被追加**（换头注不追加条目照样绿）；负 grep 只扫 `.claude/agents/`，**不扫 DEV-GOVERNANCE.md 自身**残留。补两齿：`grep -c "五条前置清单" docs/DEV-GOVERNANCE.md` = 0 + 第 6 条内容锚（如 `grep -c "input-brief" docs/DEV-GOVERNANCE.md ≥1`）。② **F7 矛盾**：T4 acceptance-author 条目「`:46` 引用行『五条中的②③』句保持」按字面 = 保留「五条中的」字样，而 V5 负 grep `五条前置清单\|五条中的` 要求 `.claude/agents/` 零命中——`:46` 实文「（起包清单五条中的②③）」恰含该 pattern，两硬项**不可同时满足**，落地执行者按 T4 字面落即 V5 红、按 V5 改即偏离冻结 T4 字面（landing-fidelity 两难）。意图显然是「②③ 选集语义保持、计数词五→六」，冻结文本必须钉死到无第二读法。
- **V6（F13 + F1 传染）**：交日落复盘、非 landing 门 ✓ 定位诚实。守护指标基线两处待修：「acceptance 缺陷再派次数（基线：story-p3 单包 4 次）」——pod-events 实测 story-p3 冻结后 acceptance-author 再派 = **3 次**（b65a58bd harness-repair / b45eb47f repoint / e1a4aaef live-tooth-tighten），含冻结派发本身才 = 4，计数口径须钉死；「fixer 轮数（基线：12 包共 3 次）」实测两种口径下均 = 3 ✓ 但包数随 F1 刷新变 13。基线表整体随 F1 数据刷新联动更新。

## H. 结构字段与影响面 sweep——结论：landing-manifest 与 packet-id 在场且与 T1–T7 精确闭合 ✓；sweep 分类无 UNCLASSIFIED ✓；scope-map 断言亲核 ✓

- **结构字段** ✓：landing-manifest 围栏块在 Status 头部块下方固定位，11 行 = T1(1)+T2(1)+T3(1)+T4(4)+T5(1)+T6(1)+T7(2) 目标文件**精确闭合**（无孤儿、无缺项）；packet-id 行在场（§2 改法集 + §5 验证集）。
- **sweep** ✓：九行全分类、UNCLASSIFIED=0 声称成立；「未触」面亲核：landing-manifest 无 `docs/contracts/**` ✓、无 scope-map/settings/verify-gates.json ✓；新脚本落 `.claude/scripts/`（既有平台面）✓；scope-map 实测 planner allow 已含 `.claude/taskpackages/**`（brief 写面在既有槽内，「无新写槽需求」成立；脚本 fs 写与 dispatch-stamp/pod-events-append 同型先例）✓。
- **taskpackage schema 行「存量 19 包零追溯」**：随 F4 修正措辞（in-flight 非零）。

## I. OQ 完备性——结论：三 OQ 选项互斥、各带代价与推荐 ✓；正文按推荐 (a) 编入 → FROZEN 前置确认项（不阻塞 R1）

OQ1（a/b/c 互斥，代价各表，推荐带语义相反性论证——scan-miss=该扫没扫 vs over-scan=不该扫扫了，独立型才可分向统计，论证成立）；OQ2（a/b 互斥，推荐 (a) 与影子目录禁令精神的相容性论证成立）；OQ3（a/b/c 互斥，推荐 (a) 直接被 §1 P2 普查实证支撑——硬谓词抓不住动机包，(b) 的「为可见的一半设闸」批评经本审 census 复核为真）。正文依赖：T7 按 OQ1 裁断值落、T2① 假设 OQ2(a)、T3/T5 假设 OQ3(a)——主线程正在向 human 收口；**FROZEN 前须显式裁断落 §7/Status**（S4 先例），若改选 (b)/(c) 则 T7/T2/T3 对应重写。

---

## Findings 表

| # | severity | kind | 位置 | 摘要（最强证据） | 修法方向 |
|---|---|---|---|---|---|
| F1 | **BLOCKING** | 事实准确性/可复现性 | PROPOSAL §1 P1（`:44-53`）、§5 V6 | 按文中命令实测 = 91 事件/13 包/47 exact，非 72/12/40；对账确定归因 = 静默排除 260829-schedule-reminder（16:53 才 merge 进本分支，分析先跑但未 pin as-of）+ actor 表 n 列 = durationMs 有效子集口径未披露；被排除包含全数据集最大长尾（builder `9de6ab28` 109.4min/226tools、integrator `f7ea4133` 71.9min/248tools → integrator 行「长尾—」全量下为假） | 全量数据重跑刷新 §1 P1 表与 V6 基线（推荐），或 pin as-of commit + 显式声明排除与 n 口径；长尾实例列补真实最大值 |
| F2 | SHOULD | census 计数（硬项 1b） | PROPOSAL §1 P2（`:61-64`） | 双面实测去重 = **8/19** 非 9/19；9 = current/ 指针（schedule-reminder 副本）重复计入；「19 个 taskpackage.json」raw glob 实为 20（含 current/） | 数字改 8/19 并注明排除 current/ 口径；结论方向不受影响、无需改闸形态论证 |
| F3 | **BLOCKING** | 内部时序矛盾 | PROPOSAL T4（`:116`）vs T3（`:108`） | Step 8 finalize 硬前置「两份 input-brief 在场」vs builder brief 唯一定义生成点在 Step 9 前置——照字面执行的 planner 在 Step 8 死锁；T3 括注只论证 acceptance-author 侧 | 钉死「Step 7 派发前双 brief 一并生成（refs Draft 即齐、派生不依赖 finalize-only 字段），Step 8 核在场，Step 9 只引用」 |
| F4 | SHOULD | 过渡面未成文 | PROPOSAL T3（`:110`）、riskTier 行（`:7`） | 「存量零追溯」漏 in-flight 包：landing 时在飞 pod（已过 Step 7/9、无 brief）下一轮 scoped verify 撞 T3.2/T3.3 红；当下无在飞 pod（current/=已 merge），窗口 = 至 landing 间新起 pod | 如实注记一次性过渡摩擦 + 补救动作（planner 补跑 brief-inputs + 补填 crossEnd），或设过渡豁免 |
| F5 | SHOULD | 谓词未封闭 | PROPOSAL T2 advisory（`:100`） | wire 节集合「(§1.5/§三)」由动机包 refs 反推；实测契约 §四「端云协议剖面」才是 wire 核心节未入集合；节号硬编码 G3 脚本，契约改节号 → WARN 静默失效 | 集合作脚本显式常量 + 注释钉判定源与维护义务；评估纳入 §四/§1.2/§1.3 或改锚点 pattern 谓词 |
| F6 | SHOULD | 验证判别力 | PROPOSAL §5 V5（`:176`） | `grep -c "六条前置清单" ≥1` 证不了第 6 条正文被追加（换头注即绿）；负 grep 只扫 .claude/agents/、不扫 DEV-GOVERNANCE 自身残留 | 补 `grep -c "五条前置清单" docs/DEV-GOVERNANCE.md` = 0 + 第 6 条内容锚（如 grep "input-brief" ≥1） |
| F7 | **BLOCKING** | 硬项互相矛盾 | PROPOSAL T4 acceptance-author 条（`:118`）vs V5（`:176`） | 「`:46` 引用行『五条中的②③』句保持」按字面保留「五条中的」，而 `:46` 实文含该 pattern、V5 负 grep 要求零命中——两硬项不可同时满足，落地者按 T4 字面落即 V5 红 | T4 改写为「『②③』选集语义保持、计数词五→六（『起包清单六条中的②③』），另追加第六条引用」 |
| F8 | SHOULD | 机械可判性缺口 | PROPOSAL T2 ③（`:97`） | 「同 subsystem 已 merged 包…按时间倒序」三判定源未钉：merged 判定源（board 不入版本控制不可依赖）、subsystem 匹配规则（相等 vs token 包含）、时间轴（changeId 前缀/merge 时刻/mtime） | T2 钉死三判定源（建议：merged = changes/dev/<id>/supervisor-verdict.json APPROVE ∨ pod-events merge 事件；subsystem = token 化包含；时间 = changeId 日期前缀） |
| F9 | SHOULD | fixture 规约缺口 | PROPOSAL §5 V3（`:174`） | fixture 落点路径与清理义务未指定（落进 .claude/taskpackages/ 会污染包数 census——F2 即实例）；正态 fixture 须满足闸全部既有必填未注明 | 钉 fixture 目录（仓外 temp 绝对路径可用——闸 resolve 支持，或本批目录 + 用后清理）+ 最小字段清单（可引 planner.md TaskPackage 格式示例） |
| F10 | COULD | 行号漂移 | PROPOSAL §0（`:32`） | 引 `taskpackage-required-check.mjs` 头注「`:12`」，实测该句在 `:11`（文本逐字准确） | 行号改 `:11` |
| F11 | COULD | 措辞过强 | PROPOSAL T2 ②（`:96`） | 「行号不落库，防腐化」——行号落进 Git-tracked brief 同样漂移；实际防腐机制 = 时间戳 + 按派发再生 + 非权威快照，未成文 | T2/T6 补一句 brief 行号快照语义（消费时 symbol 已移则重新 grep） |
| F12 | COULD | 规格缺口 | PROPOSAL T2 ①（`:95`） | 无锚点 refs（实测 g-relay-fix contractRefs 即裸路径）在 brief 中呈现行为未定，封闭清单对其退化为整文件 | 标「无锚点（整文件）」+ advisory 提示 planner 补锚 |
| F13 | COULD | 基线口径 | PROPOSAL §5 V6（`:177`） | 「story-p3 单包 4 次」计数口径未钉：pod-events 实测冻结后再派 = 3（harness-repair/repoint/tighten），含冻结派发才 = 4 | V6 写明计数口径（phase 集合），随 F1 刷新 |
| F14 | SHOULD | 生产者缺位 | PROPOSAL T7（`:139`）/ T1 第 6 条第 2 项（`:85`） | OQ1(a) 扩 over-scan 枚举值后无记账触发定义——无生产者的枚举值 = 死 canon，仪表盘列可能恒空（对照 stall 在 `:122` 有触发注记） | 一句钉触发（如「交付报告超读申报段非空 → planner 落 over-scan friction，summary 引申报行数」） |

---

## 降档与归类复核（PROPOSAL 显式请判官裁，镜像上批 D 节写法）

### D-1 · DEV-GOVERNANCE.md 归类裁断：**G1 确认，不改判**

`docs/DEV-GOVERNANCE.md` §6 表 `:212` 实文（verbatim）：「| **入口与指令根（G1）** | `AGENTS.md`、`CLAUDE.md`、`docs/DEV-GOVERNANCE.md` | architect | **human** |」——DEV-GOVERNANCE.md 在 G1 行**字面列名**，归类无解释空间。batch-2 先例「将其成文标 G2 并声明 G1 为零」与 SSOT 表直接矛盾，属先例错误，不予沿用；PROPOSAL 按存疑向严自标 G1 **正确，维持**。归类的轨道后果 = 零：G1 不是直接 Track A 判据（判据只有契约面 + 跨系统结构性变更），Track B landing 通道明文「G1/G2/G3 全落」（arch-review-protocol 双轨路由节），§6 表 Approver=human 由 Track B 的立项 ack + FROZEN 授权 + 升级闸机锚（FROZEN/§7 GREEN/landing-verdict）承载。本批 profile 定 **G1+G2+G3** 成立。

### D-2 · Track B 降档独立复核：**成立，足额**

按三要件逐一独立核：
1. **契约面零触碰** ✓：landing-manifest 11 文件无 `docs/contracts/**`；T2 脚本对契约只读派生指针（不抽正文），契约门机械判据是 diff（pre-merge-escalation-gate `hasContractsDiff`），本批 diff 恒不含契约面。
2. **非跨系统结构性变更** ✓：改动全落治理/流程面（dev-loop 输入面机制），不同时动固件协议与服务端语义；crossEnd 字段治理的是「跨端判断的申报」，不改任何系统 spec 语义。
3. **声明红队** ：两造均未声明（自愿逃生门，非义务）。

对照「机检硬排除」段（G1 → HUMAN_REQUIRED、G3 → HUMAN_REQUIRED、gate 变更 → 不合格、自改递归条）：该段整体标注「**Track A 触发时适用**」（管 Track A 内 lightweight vs full 的 depth 路由，不管 Track B vs A 的轨道路由），且双轨路由节明文「governance 类变更（含 self-def / review-loop agent defs）**不再强制升级**——治理形态由 human 定义，Track B 评审循环的对抗力度即为足额」。本批触的四件 agent def 是 **dev-loop 角色**（非 review-loop 六件、非 architect self-def），T4 改的是正文 Inputs/Process/References（不动 frontmatter tools/name/model/write scope——不触自改递归面的实质）；扩承重闸判定 = 向严 + 零追溯（F4 过渡注记补齐后成立）+ 新脚本无 canonical 写面。Track B 力度组成：R1 全面审（本文）+ human 立项 ack + FROZEN 授权 + landing-verifier 独立核验 + 前置升级闸 gov 档机锚——对该风险形态足额。**结论：降档成立，无需升 Track A。**
**landing 纪律注记**（给落地执行者）：T4 限于四 def 正文；若实施中发现需动任何 frontmatter 字段，即触自改递归高敏面（机检硬排除精神），须停下另行升级，不得顺手改。

### D-3 · riskTier 复核：low-medium 成立

向严不向松（闸只加严）+ 存量零追溯（F4 注记补齐后）+ brief 缺失降级兜底（manifest-missing 申报）+ 新脚本 fail 面有界（advisory/self-check 先行）。downside 主要形态 = T3 扩展若有 bug 则 fail-loud 拦全部新 pod——该风险由 V3 负正两态 fixture + 单源传导（一处扩、两处零改动）约束，medium 以下合理。注：前置升级闸不解析 PROPOSAL riskTier（gov 档只验 FROZEN/GREEN/landing-verdict 机锚），此字段为描述性，无机械耦合。

---

## FROZEN 前置确认项（非 finding）

- **OQ1/OQ2/OQ3 human 显式裁断**：正文已按推荐 (a) 编入（T7 依赖 OQ1 裁断值、T2① 按 OQ2(a) 不预授权、T3/T5 按 OQ3(a) 骨架必在+二选一）。主线程正向 human 收口——按 S4 先例，FROZEN 前须显式裁断并记录；若任一 OQ 改选非 (a)，对应 T 项重写后再送审。

## 范围外观察（不入本批 findings，建议主线程另行登记）

- **前置升级闸 green 机锚宽松**：`pre-merge-escalation-gate.mjs:83-84` 注释意图为「GREEN 且 verdict 同段（拒绝仅 history 提及）」，但实装 regex `/verdict[：:]\s*GREEN|结论[：:]\s*GREEN|GREEN/gi` 的裸 `GREEN` 分支令**任何**含 "GREEN" 字样的 PROPOSAL 文本（如本批 `:10`「GREEN 结论前必须在场」结构字段条款句）即满足机锚——gov 档第 6 关在 DRAFT 文本上已恒真，向宽失真。本批不触该脚本、非本批 finding；建议登记 ARQ 或并入 260908 merge-chain 邻批（该闸 T5 正在动此文件）。

## Coverage · 未审面诚实列

1. `brief-inputs.mjs` 为新增脚本，无可审实现——只审了接口 spec 与四路展开语义（F5/F8/F11/F12 即其规格缺口）。
2. T7 聚合列的具体实现未审（脚本未写）；只实测了现输出形状确无 per-actor 列。
3. `.claude/scripts/lib/`（governance-gate-shared.mjs / changeid-shared.mjs）未逐行审——T3 未声明改 lib，落地时确认 crossEnd 判定不需 lib 扩展或同批核。
4. §6 留根「provider 槽位映射（260903 期间 deepseek-v4-flash）」历史事实未机核（g-relay-fix retro:18「deepseek 代理」旁证一致，采信为留根动机、不作本批承证据）。
5. `scope-map.json` 只核了 planner/acceptance-author/builder allow 面与 universal 键名，未全量审（本批声称零触碰，与 manifest 一致）。
6. V6 pilot 回看为交日落项，本审只核了其基线数据可追溯性（F13/F1 传染），不预审 pilot 结论。

## 亲核留痕（主要实测命令与输出摘要）

- 遥测：python 全量去重聚合（14 文件 → 172 unique events → 91 dc / 13 包 / 47 exact）+ 排除 schedule-reminder 重算（72/12/40，actor 表逐行吻合）+ 四 eventId 逐条打印。
- census：python 20 glob 命中 / 19 非 current 包 / dual-subsystem=8 / g-relay-fix 全字段 firmware 扫描=[] / refIC=19/19 / current 指针=schedule-reminder。
- git 时间线：PROPOSAL 首提 014adfe（09-08T17:27，「前会话遗留」）vs schedule-reminder merge e8b73c0（09-08T16:53）。
- grep：`五条前置清单|五条中的` 全仓（canonical 恰四处）；宽 `五条`（异形同词与历史证据枚举）；friction 全量枚举（environment 漂移实证 9003e855）。
- 实跑：`pod-events-aggregate.mjs`（裸跑 + --change-id story-p3）、`git status --short .claude/taskpackages/260907-g-relay-fix/`（空）。
- Read 逐字核：DEV-GOVERNANCE（:59-60/:212/:289-297）、planner（:45/:54-58/:109）、acceptance-author（:24/:29/:46）、builder（:24/:46）、portfolio-planner（:58）、scenario-author（:31/:64/:74）、task-intake-and-slicing（:211/:217）、dev-loop（:72/:122/:125/:229）、dev-loop-operating-system（:122/:130-136）、taskpackage-required-check 全文、verify-gates.json、closeout-check（:16/:211-214/:306-315）、pre-merge-escalation-gate（:77-87/:255-285）、interface-contract 节结构、两份 pod-retro、g-relay-fix taskpackage.json、scope-map allow 面。

---

# R2 · 复核记录（对 v0.2，2026-09-09）

> **轮次纪律**：R2 = 聚焦 R1 findings 修订到位性 + 修订是否引入新矛盾 + 协调者点名四题，不做无的放矢的全量重核。回灌来源核对：§8-A 十四条映射 + §8-B 两件加码 + §8-C human 五裁登记均在册；**landing-manifest 11 行 v0.1→v0.2 零改动**（git diff + Read 逐行比对）、packet-id 行在场 ✓。

## R2 Verdict：NEEDS-WORK（SHOULD ×2 / COULD ×4；R1 十四条全部闭合；无升级 human 项）

残余全部为机械修订且修法已给到可直接誊写的程度；修后 R3 仅聚焦复核该六条，不全量重核。**v0.2 的主干修订质量高**：F1 刷新表与判官以 §1 P1 复现命令原文逐字重跑的输出**逐行一致**（91/13/47；八 actor 行双 n = builder 23/17、aa 23/18、integrator 10/8、fixer 6/3、planner 22/2、verifier 4/0、architect 1/1、arch-reviewer 2/1；挂钟合计/中位/toolUses 中位 43/18.5/42.0/24/15/21；长尾四例 `9de6ab28` 109.4min/226tools、`6121e6de` 70.7/50、`20241a2c` 69.3/109、`e4b70402` gateway-directionB token-plan-probe 42.2/97 phase 亲核 ✓；aa 两例、integrator 一例、余 actor 无长尾 ✓）；as-of `c1e26a2` = 2026-09-08T23:40:50+08:00 ✓；§6 派生算术（44 条无精确 token = 91−47、builder 23−17=6 条无 durationMs）✓；F3/F7 两处 BLOCKING 矛盾消除干净；F9 的 V3 四补与闸 `:161-251` 必填面逐项核对无缺；F8「pod-events merge 类事件」臂经实测可判（`merge-completed` eventType 实存：schedule-reminder `52b7d378`，merge-driver Step 5b 簿记；与 supervisor-verdict APPROVE 臂为析取，历史 merged 包由第一臂全覆盖）。打回原因集中两处：v0.2 **新增**的 wire 节定量辩护按 T2 自身谓词不可复现（R2-F1）、V5④ 与 T4 portfolio-planner 条不闭合（R2-F2，系 R1 漏审项，诚实自记）。

## 一、R1 十四条闭合核验（逐条）

| R1 finding | v0.2 落点 | 闭合判定 |
|---|---|---|
| F1 BLOCKING 遥测 | §1 P1 全表刷新 + 口径披露段 + 修订注 + 复现命令入节 + V6/承载 ARQ 行联动 | **✓ 闭合**（复跑逐行一致，见上；v0.1→v0.2 修订注的差额归因与判官 R1 对账结论一致） |
| F2 SHOULD census | §1 P2 8/19 + current/ 口径 + 八包名单；§3 硬项 1b 同步 | ✓ 闭合（八包名单与判官实测逐一相同）。**但同段新增的 wire 节定量另起 R2-F1** |
| F3 BLOCKING Step8/9 自锁 | T2 头部 / T3.3 / T4 planner / T6 / §3 sweep 五处 | ✓ 闭合——五处均为「Step 7 前置一次产双 brief、Step 8 核在场、Step 9 只引用」，无旧读法残留；「refs 后改则重跑覆盖」与 Step 8「核在场」**不构成新两义**（重跑=覆盖非删除，在场断言单调；闸 T3.3 只查存在性，新鲜度是 Step 9 纪律）；唯一松点 =「记一行」落点未指定（R2-F6） |
| F4 SHOULD in-flight | T3「追溯口径与过渡」段 + riskTier 行 + 非目标 4 + §3 schema 行 | ✓ 闭合（四处同步；「不设过渡豁免」裁量合理：窗口 = FROZEN→landing、补救 = 补跑 + 补填一次即转绿，第二口径代价确大于一次补跑） |
| F5 SHOULD wire 谓词 | T2 advisory：脚本顶部常量 + 维护义务注释 + 纳入 §四 + §1.2/§1.3 实现期评估；WARN 宿主成文 | ✓ 闭合（常量集合 {§1.5,§三,§四} 与契约实测节结构相符；宿主 = planner Step 7/9 前置 Bash stderr 有主）。**但 §1 P2 新增定量用了另一套谓词——R2-F1（谓词单源问题）** |
| F6 SHOULD V5 判别力 | V5 重编 ①–⑥ | ✓ 闭合（负齿 DEV-GOVERNANCE 自身 ==0、内容锚 input-brief/crossEnd 各 ≥1、四 def、:122 over-scan、dev-loop 文件地图俱全）。④ 臂另起 R2-F2 |
| F7 BLOCKING T4↔V5 互斥 | T4 acceptance-author 条改写 + 理由入文 + V5③ 联动注 | ✓ 闭合（「五条中的②③」→「六条中的②③」计数词替换 + 选集语义保持，无第二读法；V5③ 改后可满足——三 def 计数词全换、builder 本无该字样实测确认） |
| F8 SHOULD 三判定源 | T2③ 钉死 + 排除 board + V1③ 三子断言 | ✓ 闭合（merged 第二臂可判性实测：`merge-completed` 事件实存；subsystem token 化包含弃用字符串相等的理由成立——本仓 subsystem 实测 5 种取值〔firmware / server / tools / firmware+server / firmware+tools〕，相等判据确会碎成互不相干先例池；时间轴 = changeId 前缀跨 checkout 稳定 ✓） |
| F9 SHOULD V3 规约 | V3 ①落点/清理 ②最小字段清单 ③brief 同目录 ④exempt 地板负例 | ✓ 闭合（字段清单与闸逐项核对无缺：标量八件、七数组、origin∨sourceType、双预算、docsOwner=integrator；planningImpact=none + planningRefs=[] 由「照抄 planner.md 示例」覆盖；正态例引用 `firmware/main/mcp_server.cc` 实存 ✓；仓外绝对路径经闸 `resolve(projectDir, explicit)` 可用 ✓） |
| F10 COULD 行号 | §0 `:11` + 校正注 | ✓ 闭合 |
| F11 COULD 快照语义 | T2②(b) 改述 + T6 同文重申 | ✓ 闭合（「防的是把旧行号当长期事实源」措辞准确）。§8-A 映射行 V1 交叉引用错位另起 R2-F3 |
| F12 COULD 无锚点 refs | T2① 标记 + advisory + V1② | ✓ 闭合（g-relay-fix 裸路径 contractRefs 实测在场，断言有料） |
| F13 COULD V6 口径 | V6 钉「冻结后再派」+ 基线 3 次三 eventId + 13 包 | ✓ 闭合（`b65a58bd`/`b45eb47f`/`e1a4aaef` 与判官实测一致；fixer 13 包 3 次 ✓） |
| F14 SHOULD over-scan 生产者 | T7 触发定义 + T1 条 2 闭环 + stall 同型对照 | ✓ 闭合（「申报段非空 → planner 落 over-scan friction，summary 引申报行数与代表文件」——生产者、触发条件、summary 形状三件齐，死 canon 风险消除） |

**human 五裁去向核对**（§8-C ↔ 正文）：OQ1→T7 ✓、OQ2→T2①/非目标 1 ✓、OQ3 连批→T1 连批理由段（三格 Step 5 骨架→Step 8 二选一→Step 10 兜底红，与 planner.md Step 5「Draft 必须包含 intake 字段」时序自洽）+ T4 ✓、F1 修法→§1 P1 ✓、②③档→非目标 5 + T2② + §6 首条（g-relay-fix notes「live 引擎 Intent:nointent」引文 R1 已 verbatim 亲核）✓。R1「FROZEN 前置确认项」闭合 ✓。§6 新增 GREEN 机锚 ARQ 条目（判官范围外观察被采纳登记、修法方向含 self-check 反例，且如实标注不入本批 manifest）——处置得当 ✓。

## 二、§8-B 两件加码定性（协调者问 3）——**均属 OQ3(a) 必要配套，维持，非须回退的新设计**

- **B1 counterpart 解析失败 fail-loud = (a) 的牙的实现面保底**。(a) 二选一的全部价值系于 counterparts 申报为真：若 path/symbol 解析失败静默产空锚，则 brief 对端面为空而闸全绿——(a) 被空洞满足，「牙全泄」判断准确。fail-loud 把申报错误在生成时点（Step 7）推回 planner 修正、或转为诚实 exempt+理由，是 (a) 二选一语义在解析相位的**直接机械投影**：不加新字段、不立新门、不扩写面，未扩 human 裁断范围（该细节在 human 裁 (a) 的解释现场已在册，未单独裁 ≠ 越权，属实现语义补全）；与仓内 fail-loud 既有实践同型（landing-manifest 消费 fail-loud、闸 fail-safe 红）。边界对称性 ✓：只打**非空** counterparts 条目的解析失败，空骨架+exempt 路径不受触碰；V1① 必测 ✓。**裁：维持。**
- **B2 exempt ≥12 字符地板 = (a) 文本中「理由」二字的可判化**。无地板则 `exempt:"是"` 过闸、二选一退化为一选；地板数源非拍数——同闸 `:292-293` 既有同型判据 `landsIn=dropped requires reason >= 12 chars`（R1 verbatim 亲核），属同闸既成约定向同构新字段的一致性投影。V3④ 负例在场 ✓。附带投影缺口见 R2-F5。**裁：维持。**
- 两件「human 未单独裁断」的来源标注诚实合规（作者≠判官：主线程提出、判官定性、human 在 FROZEN 授权时天然保留否决位）。

## 三、R2 新 findings

| # | severity | 来源 | 位置 | 摘要（最强证据） | 修法方向 |
|---|---|---|---|---|---|
| **R2-F1** | **SHOULD** | v0.2 新增 | §1 P2 末条 + §3 硬项 1b 行 + §4 OQ3(b) 注 | 「wire 节引用 4/19（schedule-p2/schedule-reminder/simulator-lvgl-align/g-relay-fix），非双面 2/19」按 **T2 自身集合 {§1.5,§三,§四} + 锚点绑定 interface-contract.md** 不可复现。判官独立普查实测 = **5/19**：schedule-reminder（§三，双面）、voice-roundtrip（§一+§四，双面）、device-ws-switch（§四，双面）、**gateway-directionB（§四，server 非双面）**、g-relay-fix（§1.5+§三，server 非双面）。差额确定归因：① 普查谓词未绑文件名——simulator-lvgl-align 的 `§1.5` 锚实指 `docs/notes/feature-inventory.md#§1.5`（其 ic ref 仅裸路径、零 ic 锚点），误计；② 节集合与 T2 常量不一致——计入 §二（schedule-p2 唯一 ic 锚 = `#§二`，B1 课表结构，不在 T2 集合）、漏 §四（三个真引者全丢）。**净增覆盖 = 2/19 数值不变、成员应为 gateway-directionB + g-relay-fix**——恰与 v0.1 OQ3(b) 注自己点名的两包吻合（gateway-directionB 同为 subsystem=server 的真跨端包），(a) 裁断不受动摇、论据反而更强 | P2 末条与硬项 1b 行按 T2 谓词改写（5/19、双面 3、非双面 2 = gateway-directionB + g-relay-fix）；**声明 census 谓词与 T2 常量单源**（防御数字与上线 advisory 谓词再分叉——正是本批自警的双源漂移）；§二（云端写、端侧解析的 B1 结构）可并入 §1.2/§1.3 的实现期评估清单 |
| **R2-F2** | **SHOULD** | R1 漏审（诚实自记），v0.1 即在 | §5 V5④ vs T4 portfolio-planner 条 | V5④ 要求**四** def 各含 `input-brief`（grep -l 四件 wc -l == 4），但 T4 portfolio-planner 条只指定 crossEnd 预填 + 计数词同步——实测该 def 现文 `grep -c input-brief` = **0**，冻结 T4 文本不产出 V5④ 强制元素。非 F7 型互斥（V5④ 在 packet 内〔packet-id = §2+§5〕，执行者读 V5 可补 mention 满足之、无逻辑矛盾），但过与不过依赖执行者自由发挥 = landing-verifier 对账噪声 | 二择一钉死：**(i) 推荐**——T4 portfolio-planner 条补「`:58` 第六条引用点名 input-brief（其 crossEnd 预填的下游消费方 = planner Step 7 brief 生成，点名有实质语义非纯过 grep）」；(ii) 或 V5④ 收窄为三 def（planner/acceptance-author/builder，brief 生命周期三角色） |
| R2-F3 | COULD | v0.2 新增（§8-A） | §8-A F11/F12 行 | 映射表 V1 交叉引用错位：F11 行称「V1② 加断言」，实测 V1② 是 F12 的裸路径断言（F11 快照语义无 V1 断言，落点 T2②(b)+T6 已足）；F12 行称「V1① 加子断言」，实测 V1① 是 §8-B1 的 fail-loud 断言。映射表是 R2/landing-verifier 对账面，错位制造对账噪声 | 更正两行：F12→V1②；F11→「无 V1 断言（措辞修订 T2②(b)+T6）」或补时间戳在场断言后如实引 |
| R2-F4 | COULD | v0.2 回灌残留 | §2 T7 末（`:182-184`） | 重复句：「对应 P3，并给 OQ2（digest phase-2，已裁不预授权）提供决策数据面。」之后紧跟 v0.1 旧句「对应 P3，并给 OQ2（digest phase-2）提供决策数据面。」——回灌时旧句未删 | 删旧句，留带裁断注的新句 |
| R2-F5 | COULD | v0.2 新增（B2 连带） | T5 括注 vs T3.2 | B2 的 ≥12 字地板进了闸（T3.2）与 V3④，但 T5 字段定义括注（形状/二选一语义/advisory WARN 行为/覆盖档）未显式投影——起草侧读 task-intake-and-slicing 起包时不知地板，短 exempt 撞闸红反推 = 复刻本批自己要消灭的「期望只活在闸里」病灶的微形态 | T5 括注补「exempt 理由 ≥12 字符（闸地板同 T3.2）」字样 |
| R2-F6 | COULD | v0.2 新增（F3 修法连带） | T4 planner Step 9 | 「若 Step 8 之后 refs 有改动则重跑覆盖并**记一行**」——记录落点未指定（taskpackage notes？pod-events？planner 交付报告？）。非矛盾：重跑=覆盖非删除，Step 8 在场断言与闸 T3.3 存在性检查均单调不受影响 | 钉落点（建议 taskpackage notes 一行或 pod-events 注记，与超读申报记账面同源） |

## 四、协调者四题答案汇总

1. **生成时点三处（实为五处）一致** ✓：T2 头部 / T3.3 / T4 planner / T6 注册文 / §3 sweep 行全部为「Step 7 前置一次产双 brief、Step 8 核在场、Step 9 只引用」，无旧读法残留；「refs 后改则重跑覆盖」不构成新两义（在场单调），唯一松点 =「记一行」落点（R2-F6）。
2. **V3/V5 与 T3/T4 可满足性**：V3 负正两态 + 四补全后可满足 ✓；V5 ①②③⑤⑥ 闭合 ✓，**④ portfolio-planner 臂不闭合 = R2-F2**——F7 型互斥未复现，R2-F2 是弱形态（V 强制元素未被 T 文本钉死，可同时满足但靠执行者自觉）。
3. **§8-B 定性**：B1、B2 均 = OQ3(a) 必要配套，**维持不回退**（论证见第二节）；附 R2-F5（B2 地板的 T5 投影缺口）。
4. **F1 新数字自洽** ✓：复现命令原文重跑逐行一致、as-of commit ✓、派生算术 ✓、V6 基线三 eventId ✓。**唯一不复现 = P2 wire 节定量（R2-F1）**，修正后 OQ3(a) 覆盖差论据数值不变、成员更强。

## 五、结构字段核对（随本轮 verdict 执行）

landing-manifest 围栏块在场、11 行、v0.1→v0.2 **零改动** ✓（与 T1–T7 目标文件仍精确闭合；②③档与 GREEN 机锚双双留 §6/ARQ、未扩 manifest，回灌纪律「无自添设计」在 manifest 面成立）；packet-id 行在场 ✓。

## 六、R3 预告

主线程落 R2-F1–F6 六条（SHOULD 两条必落，COULD 四条建议同批落免三度开卷）后，R3 仅核：① P2 末条/硬项 1b/OQ3(b) 三处 wire census 改写与判官实测五元组（5/19、双面 3、非双面 2 = gateway-directionB+g-relay-fix）一致 + census↔T2 常量单源声明在场；② V5④-T4 闭合方式（(i) 或 (ii)）自洽；③ 四条 COULD 落点；④ 无新引入矛盾。不全量重核。

---

# R3 · 复核记录（对 v0.3，2026-09-09 · 熔断前最后一轮）

> **轮次纪律**：按 R2 预告四条聚焦 + 协调者加核一项（WARN 宿主 Step 7/9→7 的同类残留全扫），不做全量重核。照实判，不为免熔断放宽严重度分级（亦不加重）。

## R3 Verdict：NEEDS-WORK（SHOULD ×1）→ 触发 Track B 熔断条款，上报 human 三选一

**唯一打回项 = R3-F1**：v0.3 在 §4 OQ3(b) 括注**新引入**一处史实错置（「gateway-directionB（OpenClaw 拒 tools 数组那次）」），与同文 §6 引文内部矛盾。**其余全部复核面绿**：预告四条中 ①②③ 全过、④ 的 WARN 宿主校正本身正确且逻辑自洽、「Step 9 生成」读法零残留——错置恰是在执行 ④ 的新矛盾扫描时扫出。残余为一个短语的 verbatim 替换（替换句已给），修法零判断量。

**熔断上报**：R1/R2/R3 三轮未 GREEN，按 `arch-review-protocol.md`「Track B 内熔断升级」条款升 **human 三选一**（① 转 Track A / ② 撤回·缩圈 / ③ 换新判官实例再战 1 轮——仍不绿自动转 Track A）。判官供决策事实、不预判 human 选择：残余项单一且极小（SHOULD ×1、一个短语、替换句已备），四项预告复核中三项全绿、第四项仅此一疵；若选 ③，新判官复核面 = 该短语落点 + 无新矛盾，预期分钟级。

## 一、预告四条复核结果

### ① wire census 单源改正 —— **✓ 过**

判官以 T2 同源谓词（锚点绑定 `interface-contract.md` ∧ 节 ∈ {§1.5,§三,§四}，字段面 = sourceRefs/contractRefs/docRefs/acceptanceTestsFile，排除 current/）于 R3 当场**复算**，与文内三处（§1 P2 末条 `:101-102`、§3 硬项 1b `:201`、§4 OQ3(b) `:211`）**逐字一致**：

```
wire hits: 5 / 19
  260829-schedule-reminder   firmware+server  [§三]        dual=True
  260829-voice-roundtrip     firmware+server  [§四]        dual=True
  260903-device-ws-switch    firmware+server  [§四]        dual=True
  260903-gateway-directionB  server           [§四]        dual=False
  260907-g-relay-fix         server           [§1.5, §三]  dual=False
dual: 3   non-dual: [gateway-directionB, g-relay-fix]
```

- 单源声明在场 ✓（T2 advisory `:139`：「本常量集合即 §1 P2『净增覆盖 5/19、非双面 2/19』census 的唯一谓词源……锚点还须绑定 `interface-contract.md` 文件名，不得只匹配节号文本」+ 硬项 1b「脚本实现期须引同一常量、不得各写一套」）。
- §二 并入实现期评估清单 ✓（T2 `:139`，并如实标注「schedule-p2 的唯一契约锚，实测落 T2 集合外」）。
- v0.2→v0.3 修订归因段（`:102`）与判官 R2 归因**逐点一致**（谓词未绑文件名 → simulator-lvgl-align 的 `§1.5` 实指 `docs/notes/feature-inventory.md#§1.5`；节集合分叉 → 误纳 §二、漏 §四），且把教训自指为本批单源纪律的实证——诚实、到位。

### ② V5④ ↔ T4 闭合 —— **✓ 过**（取判官建议 (i)）

T4 portfolio-planner 条（`:158`）补「`:58` 第六条引用须同时点名 `input-brief`」+ 最小句形「`crossEnd` 骨架与 `input-brief` 同源——预填的 counterparts 即 planner Step 7 生成 brief 第②路的输入」：
- **四臂可绿**：句形含 `input-brief` 字面 → landing 后 `grep -l input-brief` 命中 portfolio-planner.md，V5④ == 4 由冻结文本**必然产出**，执行者自由发挥位消除 ✓。
- **不越拆包职责**：句形纯描述下游消费关系（crossEnd 预填 → brief 第②路输入），未给 portfolio-planner 派任何 brief 生成/校验职责，其 Boundaries（不驱动内层 loop、不写实现）零触碰 ✓。语义准确性 ✓（counterparts 确为 T2② 跨端对端面路的输入）。
- 措辞微注（非 finding）：「同源」严格说是「派生关系」（crossEnd 是 brief 第②路之源），破折号从句已把关系说清，无第二读法，不要求改。

### ③ 四条 COULD 落点 —— **✓ 全过**

- **R2-F3**（§8-A 交叉引用）：F11 行改「V1 无 F11 专属断言——快照语义由 V5⑥ + T6 注册文人读承载，不硬造 grep 齿」、F12 行改「V1② 加子断言（V1① 实属 §8-B1）」——两行与 V1 实文①②③对位精确 ✓。「F11 无专属断言」的处理**可接受，判官不要求补齿**：快照语义的承重面本就是 T6 注册文（landing-verifier 人读 diff 核对），对「时间戳在场」硬造 grep 齿需要 fixture 化验证、成本收益不成比例——R2 原修法即给了两个选项，主线程择一且如实标注，合规。
- **R2-F4**（T7 重复句）：`:183` 现为单句「对应 P3，并给 OQ2（digest phase-2，已裁不预授权）提供决策数据面。」，v0.1 旧句已删 ✓。
- **R2-F5**（地板投影进 T5）：`:168` 括注显式载「`exempt` 理由长度地板 ≥12 字符（对齐闸 `:292` 同型判据，见 T3.2 与 §8-B2）」+ 理由句（起草侧看不到地板 → 闸红反推 = 病灶自复现）✓——起草侧读 task-intake-and-slicing 即见地板，投影链闭合。
- **R2-F6**（「记一行」落点）：`:157` 钉为**落制品自身**——brief 头部 `generated-at` 刷新 + 覆盖再生时追加 `regenerated-because: <refs 改动一句>`，Git-tracked、`git diff` 可审、planner 不另写 journal/prose ✓。一致性核对：与 T2②「写生成时间戳」（同字段，T4 给了字段名）、T6 快照语义注册（时间戳 + 按派发再生）无矛盾；「避免第二事实源」的取舍与本批纪律同向。

### ④ WARN 宿主校正 + 同类残留全扫 —— **校正本身 ✓ 过；扫描扫出一处新矛盾 = R3-F1**

- **WARN 宿主「Step 7/9」→「Step 7」**（主线程自查项，`:139`）：校正正确，且**逻辑自洽有证**——Step 9 的条件重跑（refs 后改）**不可能**再触发 WARN：WARN 前提 = `counterparts 空 ∧ 无 exempt`，而 Step 8 finalize 硬前置已强制 crossEnd 二选一（T3.2 同判据），故 Step 8 之后 WARN 前提恒不可满足；「WARN 唯一宿主时点 = Step 7」为真命题，非约化表述 ✓。
- **「Step 9 生成 builder brief」读法残留扫描**：PROPOSAL 内 `Step 9` 全 8 处逐一核——规范性 4 处（T2 `:139` 校正句、T3.3 `:147`、T4 `:157`、§3 `:193`）全部为「Step 9 只引用」语义；其余 4 处（`:242/:267/:286/:327`）系 §7/§8 评审与回灌记录对旧 finding 的历史引述，append-only 不改属正当。**规范性文本零残留** ✓。canonical 面（def/references）现文尚无 input-brief 字样（本批才落地），不存在仓内残留面 ✓。
- **但新矛盾扫描命中一处**：见 R3-F1。

## 二、R3 新 finding

| # | severity | 来源 | 位置 | 摘要（最强证据） | 修法方向 |
|---|---|---|---|---|---|
| **R3-F1** | **SHOULD** | v0.3 新增（R2-F1 修法执行时带入） | §4 OQ3(b) 括注（`:211`） | 「(a) 净增覆盖 2/19 = g-relay-fix（真炸过的那次）+ **gateway-directionB（OpenClaw 拒 tools 数组那次）**」——后半括注**史实错置**：① 「OpenClaw 拒绝 tools 数组」的 finding 记录源 = `changes/dev/260903-device-ws-switch/review-packet.md:76` SHOULD 行（verbatim：「引擎 `Intent: nointent`（工具调用关闭）——OpenClaw 拒绝 tools 数组（`invalid tool configuration`），tracer-bullet 为串通首链先关意图/工具」），即 g-relay-fix notes 所引「**WS 包 SHOULD #2**」；② `changes/dev/260903-gateway-directionB/` 全目录 grep「tools 数组」**零命中**，且其 review-packet `:73` verbatim「无 BLOCKING / SHOULD finding。三条均为实现已就位、待外因（凭据/配额/human 拍板）收口的已知受限项」；③ 事件链实测 = voice-roundtrip live 遗留（review-packet `:109`）→ device-ws-switch SHOULD #2（缩域处置）→ g-relay-fix FYI + `ARQ-260907-openclaw-tools-rejection-rootcause`（根因未查、挂 ARQ）——三环均不系 gateway-directionB；④ **同文内部矛盾**：§6 留根 (c) 档自己引 g-relay-fix notes「live 引擎 Intent:nointent，OpenClaw 拒 tools 数组，WS 包 SHOULD #2」，与 OQ3(b) 括注的同事件异归属互斥。**census 成员资格不受影响**：gateway-directionB 入净增覆盖 2 凭「引 §四 ∧ subsystem=server」机判成立（判官复算在场），错的只是着色短语。**严重度论证（防放宽/加重双向质疑）**：非 COULD——虚假史实将随 FROZEN 入 human 裁断记录（且该括注系 v0.3 新文本，human 09-09 五裁时未见），文内不可自恢复、会作「单面标签真跨端包」先例传播；与 R1/F2 同级（SHOULD：防御论据内的事实错、结论不受动摇），重于 R1/F10（COULD：行号漂移、实质 verbatim 准、文内可恢复） | 括注换为有据描述，判官给 verbatim 替换句：「gateway-directionB（引擎方向 B 定向包，refs 引 §四端云协议剖面——`subsystem=server` 单面标签、业务实跨端）」；若想保留 OpenClaw 事件作色彩，其记录归属应写 g-relay-fix 自身（FYI + ARQ）或指 device-ws-switch SHOULD #2，不得挂 gateway-directionB |

## 三、结构字段核对（随本轮 verdict 记录）

landing-manifest 围栏块在场、**11 行、v0.1→v0.2→v0.3 零改动** ✓（与 T1–T7 目标文件精确闭合不变）；packet-id 行在场 ✓（`:6` 未动）。§8-D 回灌表与实文逐条对位 ✓，主线程自查附加项如实单列 ✓（回灌纪律「已裁来源 + 如实标注」三版一贯）。

## 四、熔断上报（照实、不放宽）

按 `arch-review-protocol.md`「Track B 内熔断升级」：评审 3 轮（R1/R2/R3，同判官 resume、上下文连续）未 GREEN → **升 human 三选一**：① 转 Track A（全套对抗）；② 撤回 / 缩圈提案；③ 换新判官实例再战 1 轮——仍不绿则自动转 Track A。判官义务性说明（供 human 裁量、不代裁）：本批三轮打回项均非方向级（R1 三 BLOCKING = 数据可复现性 + 两处冻结文本自洽性；R2 两 SHOULD = census 谓词单源 + V5④ 闭合；R3 一 SHOULD = 一个短语的史实错置），批次方向、归类（G1 确认）、降档（Track B 足额）自 R1 起三轮稳定未动摇；R3 残余项修法为 verbatim 替换、零判断量。human 若选 ③，新判官复核面 = R3-F1 落点 + 无新矛盾两项。
