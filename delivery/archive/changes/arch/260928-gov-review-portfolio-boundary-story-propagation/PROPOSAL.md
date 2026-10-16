# PROPOSAL · 260928-gov-review-portfolio-boundary-story-propagation

> **状态：FROZEN v1.0 · 2026-09-28 · 可进 packet-bound landing**（凭据：**human 立项 ack** 2026-09-28 主线程会话；**architect R1 = `BLOCK(F1,F2,F3,F4)`** → v0.3 逐条闭环 + 建议 F5–F7 采纳 → **architect R2 = `GREEN`**（§7 R2 记录，R2-F1/R2-F2 一行修已落）；**human OQ 裁定** = OQ-1 **(b)** 不设 Pod 侧节 + 细节设计三承载位（acceptance 断言化 / TaskPackage notes·exitCriteria / 派发 prompt）· OQ-2 **(a)** Pod 内两级反查 · OQ-3 **号集相等+降级**。FROZEN = 内容冻结——§0–§6 正文与 §2 改法集自此不可回改，landing 须逐字照此执行）。
> **承载 ARQ**：`ARQ-260927-role-boundary-story-propagation`（根）+ 并批 `ARQ-260924-split-order-granularity-gate-gap`、`ARQ-260914-legacy-sxay-story-id-batch`（三件同批结项）；T9 无 ARQ 承载（human 2026-09-28 立项会话新增，见评审记录区 R2 前状态沿革）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T9）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（治理过程面、每拆包批/每倡议复现；不触契约面与固件/服务端实现面；G3 新增闸齿一处 + 既有脚本扩展一处，闸向严向）。
> **review profile**：G2+G3（G2 = `.claude/agents/**` + `.claude/references/**`；G3 = `.claude/scripts/**` 两件；另触 `delivery/workstreams/README.md` 过程面文档，非 G1/G2/G3 分层物；G1 指令根 = 0、契约面 = 0、自改递归 = 0——review-loop agent 定义零改动，本批动的 planner / portfolio-planner / acceptance-author 全为 dev-loop 面）。

```landing-manifest
.claude/agents/portfolio-planner.md
.claude/agents/planner.md
.claude/agents/acceptance-author.md
.claude/agents/integrator.md
.claude/agents/builder.md
.claude/commands/build-feature.md
.claude/references/task-intake-and-slicing.md
.claude/references/minimal-implementation.md
.claude/references/acceptance-evidence.md
.claude/references/code-review-gate.md
.claude/references/dev-loop.md
.claude/scripts/brief-inputs.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/scripts/lib/acceptance-id-shared.mjs
.claude/scripts/mutation-check.mjs
.claude/scripts/acceptance-coverage-check.mjs
.claude/skills/kidpal-pod-launch-prep/SKILL.md
delivery/workstreams/README.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：拆包链各角色守各自权限边界（RTE 只管切与序、不管怎么写；acceptance 唯一写点在 Pod 内 acceptance-author；planner 从写手改型为核对者），且 story 语义从 requirements 到冻结验收全程有机械承载、下游不再盲，拆包顺序链与粒度有前置判据。

**现状差距**（三条 ARQ 的证据面，详 §2 各 T）：① portfolio-planner 越界在 plan.md 写包级实现设计（改法取舍 / 阶段划分 / 边角），门 1 又不审这些内容——RTE 职责臃肿且内容无审；② story 语义经五跳投影、每跳 lossy，下游（acceptance-author / builder）拿到的 input-brief 里 story 只有裸编号，`S-F1.4` 在 brief 里落成 missing + advisory（`brief-inputs.mjs` 不识别 story id）；③ acceptance 双写点（拆包侧骨架 vs planner Step 1e 蒸馏）字面张力无对账，且 acceptance 冻结后无人核其对 story 的忠实度；④ 拆包顺序链（里程碑登记 / roadmap 先于 taskpackages）只存在于 M1–M4 先例自觉，无前置判据（ui-v5-align 21 包建毁实证）；⑤ `Sx-Ay` 老仓 story id 遗留字样在新口径定型批（260912）manifest 外多处存活。

**非目标**：③ 候选「verifier/supervisor 加 story↔acceptance 语义对账机械牙」**不做**（human 2026-09-28 裁：真语义对账机械做不了，①+② 已堵主要面）；不改 `docs/requirements.md` / `docs/architecture.md` / `docs/contracts/**`；不改 review-loop agent 定义（architect / challenger / defender / adjudicator / review-moderator 零触碰）；不动 scope-map 与 settings 权限面；不动 `portfolio-reconcile.mjs` / `merge-driver.mjs` 等既有 G3 闸判定逻辑（T3 仅核对既有闸对「拆包期 acceptance 空骨架」的判定面，见证据）。

## 2. 问题、根因与改法

### T1 · 拆包侧 plan.md 收窄（角色权限划分）

**问题**：portfolio-planner（RTE）在拆包落盘的 plan.md 里写包级实现设计——改法取舍（实现级决策）、阶段划分（Pod 标准 loop 的重述）、包内实现边角，这些是「怎么写」不是「切到哪」；RTE 越界做包级设计，且门 1 checklist 五项无一审这些内容，越界产物没有任何一道审。
**证据**：`delivery/taskpackages/260924-m4-k3-record-session-live/plan.md`（commit `498e3c2` 拆包落盘，`git rev-parse 498e3c2` 实测存在，`chore(portfolio): 260924 M4-star-notebook 里程碑拆包`）§改法取舍 8 条实现级决策（「发送原语复用 K2 缝」「时序 = 首帧音频前、SendStartListening 之前」等）+ §阶段划分 5 步（= planner 标准 loop 重述）+ §边角 6 条 builder 易漏；`.claude/references/task-intake-and-slicing.md:251` plan.md 骨架义务 = 「背景摘录 + 改法取舍 + 阶段划分 + 铁律约束 + demo 证据形态复述」（「改法取舍」「阶段划分」为拆包义务的条文源）；`.claude/agents/portfolio-planner.md:40`（Step 4 ②「每包必备 sibling plan.md，骨架 = 背景摘录 + 改法取舍 + 阶段划分 + 铁律约束 + demo 证据形态复述」）；门 1 checklist 五项（`task-intake-and-slicing.md:267`）无改法/阶段审项。
**根因**：260912 拆包链批定 plan.md 全量义务骨架时把「实现设计」维度一并塞进拆包义务，未划「切（RTE 权）」与「写（Pod 权）」的界；越界无审是同一枚硬币——门 1 按「拆包 review」设计，自然不覆盖越界塞进来的实现内容。
**改法**：按 human 裁定「判据切开」收窄拆包侧 plan.md 骨架为五件——**背景摘录（锚点随 workType 分流，不变）+ 可行性判据 + 跨包事实与共享冻结面约束（含跨切片边角）+ 铁律约束 + demo 证据形态复述**；删除「改法取舍」「阶段划分」两项拆包义务。判据一句钉死：**跨包事实 / 共享冻结面约束（只有 RTE 看全局才知道的跨切片知识，如「唤醒词路径也过会话发起入口」「发送原语复用 K2 冻结缝」）留拆包侧；单包内实现决策（时序细节、渲染接线、包内边角）不进拆包侧 plan.md**。落点：`task-intake-and-slicing.md`「plan.md 全量义务」节骨架改写 + 判据句；`portfolio-planner.md` Step 4 ② 同步收窄。门 1 checklist **不加**「审改法」项（改法已不在拆包侧，越界物消失即无审的对象）；包内实现决策的承载形态 = human OQ-1 裁定（§4：不设 Pod 侧节，细节设计断言化进 acceptance + TaskPackage notes/exitCriteria + 派发 prompt 三承载位）。既有铁律约束项保留（跨包冻结面「只消费零修改」类本就是共享冻结面约束的特例）。
目标文件：`.claude/agents/portfolio-planner.md`（改）· `.claude/references/task-intake-and-slicing.md`（改）

### T2 · story 整节照录进 input-brief（下游不再盲）

**问题**：input-brief 不承载 story 定位——`brief-inputs.mjs` 只识别 `path#§锚点` 形态的契约/spec 引用；sourceRefs 里的裸 story id（`S-F1.4`）被当路径处理落成 missing（文件不存在）+ advisory，下游 acceptance-author / builder 的「封闭阅读面」里 story 只有编号、语义全靠散文纪律兜底。
**证据**：`brief-inputs.mjs:62-67`（`splitRef` 只按 `ANCHOR_RE` 拆 `path#anchor`，裸 id 即 path 且盘上不存在）+ `:122-148`（`renderAnchorSections`：文件不存在 → missing 态 + 引用腐坏 advisory；无锚 → bare 态）；ARQ-260927 证据 5。**起草期实测**（2026-09-28，`grep -n` 面检 + 源码通读）：`collectRefs`（`:78-93`）三桶（contractRefs / docRefs / sourceRefs）统一走 `splitRef`，无任何 story id 特判——裸 `S-F1.4` 必落 missing，行为断言成立。
**根因**：input-brief 输入面的锚语法只设计了「文件路径 + 节锚」一族，story 锚点引用（`S-Fx.y`，定义于 `task-intake-and-slicing.md`「story 锚点引用与转写」节）是后来定型的新句法，脚本没跟上。
**改法**：`brief-inputs.mjs` 新增 **story 照录路**——① 识别 sourceRefs / docRefs 中命中 `^S-F\d+\.\d+`（及 `S-Fx.y-AC<n>` 复合句法）的 ref，映射到 `docs/requirements.md`，定位 `##### S-Fx.y · <标题>` 标题节；② 命中即在 brief 新增「story 语义权威」段：**整节照录**（AC + 补充规则 + 线框 + 锚 + 可行性行逐字），头部标注「machine-derived 投影，源 = `docs/requirements.md:S-Fx.y`，语义权威仍在 requirements 现文」；③ 节定位失败走 **advisory 通道（R1-F6 钉死语义）**：照录段如实标 not-found + stderr 提示，**不 exit 1、不阻断 brief 生成**（与第①路 missing/not-found 态同构，不猜）；④ 无 story ref 的包该段显示 none，零影响；⑤ `selfCheck` 增对应夹具。与既有「防双源漂移、不整段转抄」纪律的关系：那条管 agent 手抄（会漂移），本条是脚本每次生成时从源文件现抓的机械投影（源变投影随变），不冲突。AC 级复合句法（`S-F1.4-AC3`）照录整节、AC 号加亮标注。
目标文件：`.claude/scripts/brief-inputs.mjs`（改）

### T3 · acceptance 写点下沉 + planner 改型核对（唯一写点 = acceptance-author）

**问题**：acceptance 字段两个写点、无机械对账——拆包侧（`portfolio-planner.md:37` Step 2「N 个包的 acceptance 并集必须覆盖 proposal 决策集」/ `:40` Step 4 写 taskpackage 含 acceptance 骨架）先写一遍；Pod 内（`planner.md:47`，Step 1e「通读 story 全节、蒸馏进 acceptance/decisionCoverage」）再写一遍——同一字段两处产、两侧都没被要求与对方对账。同时 acceptance 冻结后**无人核对**它对 story 的忠实度（写好后没核验点）。
**证据**：`portfolio-planner.md:37/:40/:95`（Step 2 并集句 / Step 4 骨架义务 / Verification 节并集句复述——**删点共两处三行**）与 `planner.md:47`（Step 1e 蒸馏句）均为起草期 Read 实测（2026-09-28）；`planner.md:39` Step 1① 权威句现文**不含 acceptance**（现文 = 「以其 sourceType/origin/rigor/riskTier/gateProfile/decisionCoverage 为权威」——ARQ-260927 证据 4 对该句的转引失真，R1-F2 勘误，双写点结论不受影响：问题由 `portfolio-planner.md:37/:40` ↔ `planner.md:47` 两写点独立支撑）。
**根因**：两个写点是两批治理各自长出来的——拆包侧骨架为门 1「并集覆盖」核查提供对象（Pod 未启动时得有东西可核），Step 1e 为堵「需求已写明却当未定项请示」的事故形态（260924 批 T2），两批未对齐写点归属。
**改法**（human 裁定：写点下沉 + 写者换人）：① **拆包侧不再写 acceptance 骨架**——`portfolio-planner.md` 删两处并集句（Step 2 `:37`「N 个包的 acceptance 并集必须覆盖 proposal 决策集」与 Verification 节 `:95` 复述句，R1-F5 补列后者），承载核挂到 decisionCoverage 映射上（每个决策有承载包 / exitCriteria / 跨包 `package:` 指针 / 显式 dropped）；Step 4 ① taskpackage 写盘义务中 acceptance 字段在拆包期为**空骨架**（与 `crossEnd` / `mutationEvidence` 的 Draft 态空骨架在场先例同形）。**门 1 checklist ③ 零改动**——`task-intake-and-slicing.md:267` 现文本就核「decision-coverage 完备（含跨包行为 owner 显式登记）」，无 acceptance 并集字样（R1-F5 核实），acceptance 在门 1 期不存在故 checklist 本就不核它。② **acceptance 唯一写点 = Pod 内 acceptance-author**——`acceptance-author.md` 补「唯一写点 + 冻结后由 planner 核对忠实度」定位；Pod 内反查义务（acceptance-author Step 1 的 decisionCoverage 强制反查）保留不变。③ **planner 改型两半明钉相位（R1-F3）**——`planner.md` **Step 1e 保留通读义务**：finalize 前通读 story 全节、当**行为权威**带进派发上下文喂 acceptance-author（「需求已写明禁止推荐替代请示」三条升级面原样保留）；**核对三件明钉 Step 8b**（与 T4 扩 8b 为全 story 命中包同址、同口径互引）：AC 回引号集与 sourceRefs AC 号集一致（对齐 Step 8b 既有「多引、少引都是方向偏差信号」口径）；story 补充规则逐条有无落点（acceptance / exitCriteria / 显式 dropped+reason）；断言方向不弱于、不钉反 story 语义。核对不通过 → 退回 acceptance-author 重钉，planner 不代写。④ **`planner.md:39` Step 1① 与 `task-intake-and-slicing.md:89`（F-06 同句）核对确认（R1-F2）**——两处现文已不含 acceptance 权威句，无「改为」对象；实际动作 = 起包核验时确认一致 + 视需要在 Step 1① 补一句「acceptance 为 Pod 内产物、拆包期为空骨架」澄清（澄清句为建议件，landing 时视上下文是否需要而落）。⑤ **既有闸判定核对（起草期实测，R1 独立复核成立）**：`taskpackage-required-check.mjs:224` 对 acceptance 非空的硬要求**只在 Pod finalize / closeout 相位被调用**（`closeout-check.mjs:205/300-307` 程序化调用 + `verify-gates.json:34` Pod verifier 轮 + planner Step 8 手工跑；全仓 grep 调用点实测**无拆包期 / launch 期程序化调用点**）——拆包期空骨架只多一条红名单项、不改红绿判定（m4-k3 实包 `acceptanceTestsFile: []` Draft 态闸本就红为证），本 T 零脚本改动、零新闸。
目标文件：`.claude/agents/portfolio-planner.md`（改）· `.claude/agents/planner.md`（改）· `.claude/agents/acceptance-author.md`（改）· `.claude/references/task-intake-and-slicing.md`（改）

### T4 · AC 回引机械化（缺则闸红）

**问题**：锚 2（planner Step 8b 的 AC 号集对账）只是 workstream 包的散文纪律，非 workstream 的 story 命中包连散文纪律都没有；「AC 引用着但断言比 AC 弱 / 钉反」全漏，结构漂移无机械牙。
**证据**：`planner.md:63`（Step 8b「仅 workstream 包」「多引、少引都是方向偏差信号」）；`task-intake-and-slicing.md:257-259`（`S-Fx.y-AC<n>` 复合句法首钉 + acceptance 本地 id 换新边界——AC 回引只出现在 sourceRefs / 验收齿回引位）。
**根因**：AC 复合句法定型（260912 批）时只落了句法与散文对账纪律，没落机械判定；判据本可机判（号集一致性是闭集比对）。
**改法**：`taskpackage-required-check.mjs` 增**AC 回引齿**——finalize 态（`acceptanceTestsFile` 非空）且 sourceRefs 命中 `^S-F\d+\.\d+` 时：acceptance 齿 / sourceRefs 必须带 `S-Fx.y-AC<n>` 回引，且回引 AC 号集 == sourceRefs 声明的 AC 号集（sourceRefs 未带 AC 号集 = 只核「至少一条 AC 回引在场」，不判多引少引；Draft 态放行）。缺则闸红点名。**存量 carve-out 谓词（FROZEN 前钉死，R1-F4）**：对偶 DP-2 / `CHANGE_ID_CUTOFF` 先例——**包目录（`delivery/taskpackages/<changeId>/`）git 首提日期 < 本批 landing commit 日期 = 存量放行**（首提日期机判复用 `taskpackage-required-check.mjs:108` `gitFirstCommitDate` 既有函数，零新依赖）；cutoff 及之后建包 = 新齿适用。landing-summary 只记抽查处置结果、不做闸设计。`selfCheck` 增两条自检（story 命中包 Draft 态放行 + finalize 态缺回引红）。planner 核对步（T3 ③，Step 8b）与本齿口径对齐引用（机械齿管号集、人读核管语义方向，二者不互替）。回引齿消费 `acceptance-id-shared.mjs` 的 id 解析（复合句法兼容性由 T8 夹具先行验证）。
目标文件：`.claude/scripts/taskpackage-required-check.mjs`（改）· `.claude/agents/planner.md`（改——Step 8b 扩为全 story 命中包 + 指向机械齿的口径对齐句）

### T5 · 拆包顺序链 intake 停等三件拒拆

**问题**：建 workstream 后直接派遣拆包无前置判据——milestones.jsonl 未登记、roadmap / phases 未建、PROPOSAL 未冻结时 portfolio-planner 照拆不误，ui-v5-align 事故以 21 包建毁、3 次派遣、3 轮 human 复裁为代价实测显形；M1–M4 的先例顺序链（milestones 登记 → workstream phases → taskpackages）只存在于自觉。
**证据**：ARQ-260924 证据 ①②④（`delivery/workstreams/ui-v5-align/findings.md` §7 执行记录逐时点；`portfolio-planner.md:36` Step 1 fail-closed 核验条件 = 「输入为 `delivery/workstreams/<ws>/PROPOSAL.md` 时」才核模板硬项，非 PROPOSAL 输入不触发——起草期 Read 实测）；门 1 在 Step 5b（四件套齐备后才停等），拦不住「无序就开始拆」。
**根因**：intake 核验的触发条件绑死了输入文件形态（PROPOSAL），把「拆包前置顺序」当成模板符合性问题的子集；顺序链从未被条文承载。
**改法**（ARQ-260924 建议 ①）：`portfolio-planner.md` Step 1 intake fail-closed 从「输入为 PROPOSAL.md 才核验」扩为**拆包类派遣三件停等拒拆**——`delivery/milestones/milestones.jsonl` 已登记本倡议 / 目标 workstream 已有 roadmap（`roadmap.md` 或 task_plan phases 节）/ 已有 PROPOSAL 或等价业务目的清单；缺任一 → AskUserQuestion 停、打回主线程补齐，不猜、不代写（与既有 PROPOSAL 模板符合性核验同一打回出口）。单切片不建 workstream 的派遣不适用（该路径无顺序链前提）。
目标文件：`.claude/agents/portfolio-planner.md`（改）

### T6 · Slicing Matrix 增尺寸维 + 视觉对齐切法判例

**问题**：portfolio-planner 切法判据只有垂直切片维（能力完整性）+ 冲突面 + 跨包冻结面扫描，无尺寸维（行数 / 文件面 / 单包验证预算 / N 上下界）；ui-v5-align 两轮粒度实质等于派遣方喂入的批次提案（15 包 ← 批次 0-6 加半包；6 包 ← P-A..F），human 三轮复裁充当缺失的粒度门。
**证据**：ARQ-260924 证据 ③（planner Step 2 判据枚举 = `portfolio-planner.md:37` 起草期 Read 实测，无尺寸维）；260924 human 三包终裁判据三条（包间文件所有权不相交 / 门禁依赖包内同质 / 单包验证预算一 Pod 可消化）。
**根因**：Slicing Matrix（`task-intake-and-slicing.md`）只有「按什么轴切」没有「切多细」；视觉对齐类工作与功能切片逻辑不同构（页单位 vs 功能点），无判例可依。
**改法**（ARQ-260924 建议 ②）：`task-intake-and-slicing.md` Slicing Matrix 增**尺寸维**四判据（改动行数量级 / 文件面 / 单包验证预算一 Pod 可消化 / N 上下界提示）+ **视觉对齐类切法判例**三条（页单位粗包、包间文件面不相交、跨页串面项走合并后串行追加轮；视觉批不套功能切片细粒度逻辑）——判据引据源 = `delivery/workstreams/ui-v5-align/` 终裁记录（human 三轮复裁的三包判据）。
目标文件：`.claude/references/task-intake-and-slicing.md`（改）

### T7 · workstreams README 顺序链成文

**问题**：「task_plan phases 为必备节；里程碑登记先于拆包派遣」的顺序链只存在于 M1–M4 先例，workstreams README 未承载，新倡议无从读到。
**证据**：ARQ-260924 证据 ②（milestones.jsonl M4 行 nextAction 时序 = 先例链实例）；`delivery/workstreams/README.md` 未含顺序链条款（起草期未逐字核对——**未实测**，landing 时先读原文再落笔）。
**根因**：同 T5——先例自觉未成文。
**改法**（ARQ-260924 建议 ③）：`delivery/workstreams/README.md` 明写两条——task_plan phases 为 workstream 必备节；里程碑登记（milestones.jsonl）先于拆包派遣。与 T5 的 agent 前置条件互为表里（README 管人读纪律、intake 停等管 agent 行为）。
目标文件：`delivery/workstreams/README.md`（改）

### T8 · Sx-Ay 例句批量换新 + 复合句法兼容性夹具

**问题**：`Sx-Ay` 老仓 story id 遗留字样在 260912 批 manifest 外多处存活，新会话读旧例句会学旧口径（散装 `Sx-Ay` 而非 `S-Fx.y` 锚点引用）；`acceptance-id-shared.mjs` 参与 id 解析，`S-Fx.y-AC<n>` 复合句法是否被误拆未验证。
**证据**（起草期机械 census，2026-09-28 实测；命令 `grep -rnoE "S[0-9]-A[0-9]+|Sx-Ay" .claude/agents .claude/commands .claude/references .claude/scripts` 去重统计（R1-F1 勘误：转录时丢 `-E`，判官以同命令独立重跑 34 命中与本详单逐文件逐行一致），**本 census 为权威详单**，ARQ 条目中的旧详单以此为准）：`.claude/scripts/mutation-check.mjs:202-203`、`.claude/scripts/lib/acceptance-id-shared.mjs`（:92-172 多处）、`.claude/scripts/acceptance-coverage-check.mjs:58/69`、`.claude/references/minimal-implementation.md:28`、`.claude/references/dev-loop.md:37/109/166/274`、`.claude/references/code-review-gate.md:25/51`、`.claude/references/acceptance-evidence.md:7`、`.claude/commands/build-feature.md:3`、`.claude/agents/integrator.md:42`、`.claude/agents/builder.md:79`、`.claude/agents/acceptance-author.md:3`（description）。**census 较 ARQ 详单多两件**（`acceptance-coverage-check.mjs`、`contract-change-evidence.md`）——后者经逐点核对**排除**：`:29` 的 `S5-A2` 是指向历史真实制品（`s1-nsa-integration-e2e` 验收测试）的引用、非格式例句，改了就是伪造引用。
**根因**：260912 批 manifest 只覆盖当批九文件，manifest 外命中面留根未清。
**改法**（ARQ-260914 建议 + census 权威化）：① 文面例句换新口径（`S-Fx.y` 节锚 + `S-Fx.y-AC<n>` 复合句法 + acceptance 本地 id），逐点以 census 行号为锚；② scripts 内命中点**逐点分流**——注释 / 示例位直接换新；selfCheck 夹具内的 id token 与断言预期**连动换新**（换 token 必换预期值，夹具实跑判红为准）；③ `acceptance-id-shared.mjs` 先跑兼容性夹具验证 `S-F1.4-AC3` 解析形态（不被误拆为路径 token、AC 号可提取），**再定解析面是否需要代码改动**（夹具绿则零代码改动，只动例句）；④ historical 冻结件 / 已结 review 记录 / 指向历史制品的真引用（如 `contract-change-evidence.md:29`）一律不动。
目标文件：`.claude/agents/integrator.md`（改）· `.claude/agents/builder.md`（改）· `.claude/agents/acceptance-author.md`（改，description 例句位）· `.claude/commands/build-feature.md`（改）· `.claude/references/minimal-implementation.md`（改）· `.claude/references/acceptance-evidence.md`（改）· `.claude/references/code-review-gate.md`（改）· `.claude/references/dev-loop.md`（改）· `.claude/scripts/lib/acceptance-id-shared.mjs`（改，例句 + 视夹具结论）· `.claude/scripts/mutation-check.mjs`（改）· `.claude/scripts/acceptance-coverage-check.mjs`（改）

### T9 · input-brief 生成时点归还 Pod 内 planner（翻转 260921 T5 裁定）

**问题**：input-brief 的消费方全在 Pod 内（acceptance-author / builder），但生成时点被 260921-gov-review-launch-pipeline T5（OQ-1 裁 (a)）定在拆包侧——portfolio-planner 对下游形成三重交接（planner / acceptance-author / builder），而它的合同本应只有 planner 一个；与 T1 治的「RTE 越界做包级设计」同族：拆包侧该管「切到哪与切多细」，不该管「Pod 内角色读什么」。且拆包期生成有两处实质错位：① brief 内容是**时点快照**（②路 symbol 行号区间、③路先例封顶，脚本自注「生成时点快照，非长期事实源」），拆包期生成到 Pod 真消费间隔整个 launch 周期、快照最陈旧，「核在场 + refs 有改动才重跑」补丁机制本身是生成时点错位的气味；② ②路输入 `crossEnd` 的权威判断在 **planner Step 1d（起包期设计判断）**，拆包侧只预填骨架——拆包期生成等于用骨架值生成正式输入面，planner 改判后靠重跑兜底。
**证据**：`portfolio-planner.md:40`（Step 4 ⑤「同批跑 brief-inputs.mjs 双份，生成时点归拆包侧」）；`planner.md:61`（Step 7「核在场 + refs 有改动才重跑」——260921 T5 改判后的补丁形态）；`planner.md:46`（Step 1d crossEnd 权威判断在起包期）；`brief-inputs.mjs:155`（resolveSymbolRange 注释「生成时点快照，非长期事实源」）；260921 批 PROPOSAL T5（`delivery/changes/arch/260921-gov-review-launch-pipeline/PROPOSAL.md:186-223`，(a) 裁定与三条理由原文）；launch 前置门机械 check 集不含 brief（`pod-coordination.md:36-39`：check 集 = taskpackage blocking + PROPOSAL blocking + decision-coverage warn，**brief 不在**）。
**根因**：260921 批治的是「三方生成时点打架、无单一事实源」，(b) 归 Pod 侧同样能达成单一产方，但当时以「闸现文位置 + human 点火前看输入面」为由裁了 (a)；角色边界代价与快照陈旧代价当时未被权衡。
**改法**（human 2026-09-28 裁：归还 Pod 侧，历史已生成的不改）：① `portfolio-planner.md` Step 4 删 ⑤（双 brief 生成句），交接物回归四件套；② `planner.md` Step 7 改回**生成**形态——crossEnd（Step 1d）与 refs finalize 之后、派 acceptance-author 之前，跑 `brief-inputs.mjs` 双份（260908 原形）；Step 7/9 的「按需重跑」机制保留（Pod 内 refs 中途仍可能改）；③ `dev-loop.md` 生成时点文面回改（260921 T5 承重指令位逐字回退）；④ `kidpal-pod-launch-prep/SKILL.md` 步骤 8「双 brief 在场」项删（launch 侧不再核 Pod 内输入面）；⑤ `taskpackage-required-check.mjs` 的 finalize 态 brief 在场查**不动**（查的本来就是 Pod 侧产物，与生成时点归还后自洽）；⑥ `pod-coordination.md` §2b 机械 check 集**零改动**（brief 本不在集内）；⑦ **存量不追溯**：已生成 brief 的存量包目录不清理、不重生成、不改时点断言，新包起算。与 T2（story 照录路）零冲突——脚本内容不变，只换调用时点与调用者。
目标文件：`.claude/agents/portfolio-planner.md`（改）· `.claude/agents/planner.md`（改）· `.claude/references/dev-loop.md`（改）· `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G2 | `.claude/agents/portfolio-planner.md` | 改（T1/T3/T5） | 消费方 = 主线程派发的 RTE 会话；Step 1/2/4 条文收窄与前置停等 |
| G2 | `.claude/agents/planner.md` | 改（T3/T4/T9） | 消费方 = Pod planner 会话；Step 1e 通读义务保留、核对步钉 8b、Step 7 改回生成形态、Step 1① 视需要补澄清 |
| G2 | `.claude/agents/acceptance-author.md` | 改（T3/T8） | 消费方 = acceptance-author；唯一写点定位 + description 例句 |
| G2 | `.claude/agents/integrator.md`、`builder.md` | 改（T8 例句） | 例句口径，零行为语义 |
| G2 | `.claude/references/task-intake-and-slicing.md` | 改（T1/T3/T6） | 权威条文：plan.md 骨架、门 1 checklist、story 转写节、Slicing Matrix；消费方 = 两 planner + 门 1 判官 |
| G2 | `.claude/references/minimal-implementation.md`、`acceptance-evidence.md`、`code-review-gate.md`、`dev-loop.md` | 改（T8 例句） | 例句口径；`dev-loop.md` 额外核 `:166/:274` 上下文非闸判据位 |
| G2 | `.claude/commands/build-feature.md` | 改（T8 例句） | 参数示例位 |
| G3 | `.claude/scripts/brief-inputs.mjs` | 改（T2） | 新 story 照录路 + selfCheck 夹具；消费方 = 双 input-brief 生成 |
| G3 | `.claude/scripts/taskpackage-required-check.mjs` | 改（T4） | 新 AC 回引齿 + selfCheck；承重闸，closeout 相位程序化调用 |
| G3 | `.claude/scripts/lib/acceptance-id-shared.mjs`、`mutation-check.mjs`、`acceptance-coverage-check.mjs` | 改（T8） | 例句 / 夹具 token 连动；`acceptance-id-shared` 兼容性夹具先行 |
| G2 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | 改（T9） | launch 侧人工 checklist 删「双 brief 在场」项（launch 侧不再核 Pod 内输入面）；机械 check 集（pod-coordination §2b）零改动 |
| 过程面 | `delivery/workstreams/README.md` | 改（T7） | workstream 准入与顺序链纪律，人读面 |
| 未触 | `docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md` | 零改动 | 契约面与 spec 面零触碰；`brief-inputs.mjs` 只**读** requirements（新增读路径，非写） |
| 未触 | scope-map.json、*.settings.json、hooks/**、review-loop agents | 零改动 | 权限面与对抗链零触碰；既有闸判定逻辑零改动（T3 ⑤ 实测核对） |

## 4. human 取舍点（OQ）

- **OQ-1 · Pod 侧实现设计承载形态（已裁 2026-09-28）**：human 裁 **(b) 不设 Pod 侧 plan.md 节**，并附关键补语——「拆包给的是概要设计，他调查的也主要是为了怎么拆包；但 Pod 是要实现的，可能会有细节设计，不能没有地方放」。据此明钉**包内细节设计承载位**（防 (b) 被读成「细节设计无处放」）：可断言的部分**断言化进冻结 acceptance**（时序、接线、边界形态类——m4-k3 的「时序 = 首帧音频前」本就该是 A1 齿的断言而非散文）；不便断言化的约束与取舍落 **TaskPackage `notes` / `exitCriteria` / `decisionCoverage` 备注**；派发期上下文细节落 **planner 派发 prompt**。三个承载位全在 Pod 内、零回拆包侧；plan.md 定型为「拆包侧可行性卡」（背景摘录 + 可行性判据 + 跨包事实与共享冻结面约束 + 铁律 + demo 形态）。
- **OQ-2 · 门 1 覆盖核查后移落点**（T3 余留分叉：「acceptance 并集覆盖 proposal 决策集」的 acceptance 级核查移到哪）：
  - (a) **Pod 内两级反查（推荐）**——acceptance-author 既有 decisionCoverage 强制反查（本包）+ planner 核对步（T3 ③）兜底；跨包视角靠 workstream decisionCoverage 的 `package:` 指针与既有跨包共享冻结面扫描条款；零新机械闸；
  - (b) launch 前置机械汇总——扩 `portfolio-reconcile.mjs --check-launch` 读 workstream decisionCoverage 做「每决策有承载包」核（新增 G3 齿）。
  - 推荐理由：决策→承载包映射在拆包期即可核（门 1 checklist ③ 保留），acceptance 级覆盖天然只能在接受包后核，(a) 已有两级且零新闸；(b) 若后续实测漏检可按量化触发器再立（对齐 ARQ-260913 触发器模式），本批不预批机械齿。
- **OQ-3 · AC 回引齿的严格度**（T4 细节，倾向已定、human 可改）：回引 AC 号集 == sourceRefs 声明的 AC 号集（对齐 Step 8b「多引、少引都是方向偏差信号」）；sourceRefs 未带 AC 号集时降为「至少一条 AC 回引在场」。若 human 要更严（sourceRefs 强制带 AC 号集）→ 加重拆包侧义务，本批不推荐。
- **OQ-4 · input-brief 生成归属（已裁，记录形态）**：human 2026-09-28 立项会话裁定 **T9 = 归还 Pod 内 planner**，翻转 260921 批 OQ-1(a)。撤回理由对照当时三条裁 (a) 理由——①「闸现文位置不用改」：finalize 态 brief 在场查保留，位置未动，此理由与 (b) 兼容；②「F-03 保证 Pod 天然拿到」：Pod 侧生成自产自用，F-03 不再需要覆盖 brief；③「human 点火前看输入面」：human 2026-09-28 原话「前置闸里面的那些校验，我觉得都不应该在这个阶段去判断」——该理由被 human 主动撤回。评审循环的攻点与缓解注记（R1-F7 自答）：① **fail-loud 后移的 Pod 预算代价有界**——Step 1d crossEnd 权威判断与 Step 1f 拆包快照保鲜核验均在 Step 7 生成**之前**完成，残余失败面仅剩拆包预填 counterparts 腐坏一类，fail-loud 在派发前炸、不烧 builder 轮；② **launch 侧输入面可观测性实质损失有限**——human 点火前仍可见 taskpackage refs + crossEnd + plan.md（brief 只是它们的派生快照），信息面未消失、消失的只是派生视图。

## 5. 验证集（landing 前机械前置 + landing 后核验）

| # | 时点 | 命令 | 预期 |
|---|---|---|---|
| 1 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260928-gov-review-portfolio-boundary-story-propagation/PROPOSAL.md` | exit 0；`anchorUnion` 成员名单 == landing-manifest 18 路径，注记零夹带、零目录 token |
| 2 | pre-commit | `node .claude/scripts/brief-inputs.mjs --self-check` | ok:true（含新增 story 照录路夹具：story ref 解析 + 整节提取 + not-found advisory（不阻断、brief 仍产出）——R2-F2 勘误：原词「fail-loud」与 T2 ③ advisory 语义打架） |
| 3 | pre-commit | 探测夹具：临时包 refs 带 `S-F1.4` 跑 `node .claude/scripts/brief-inputs.mjs --change-id <probe> --role builder --dry-run` | brief 含 story 整节照录段，节文字与 `docs/requirements.md` §S-F1.4 byte 一致（探测包用后即删、不入 commit） |
| 4 | pre-commit | `node .claude/scripts/taskpackage-required-check.mjs --self-check` | ok:true（含新自检：story 命中包 Draft 态放行 / finalize 态缺 AC 回引红 / AC 号集不等红） |
| 5 | pre-commit | `node .claude/scripts/lib/acceptance-id-shared.mjs --self-check`（flag 形态以 `:176` 输出面为准，**起草期未实测**，landing 先跑定形态） | ok:true 且含 `S-F1.4-AC3` 复合句法解析夹具（不被误拆、AC 号可提取） |
| 6 | post-commit | `node .claude/scripts/route-regression-check.mjs` | 绿（acceptance-author description 换新后路由信号未退化） |
| 7 | post-commit | `node .claude/scripts/lint-descriptions.mjs` | 无新增告警（基线 = landing 前实跑一次留痕） |
| 8 | post-commit | `grep -rEn "S[0-9]-A[0-9]+|Sx-Ay" .claude/agents .claude/commands .claude/references .claude/scripts` | 命中仅余 T8 证据段声明的排除项（historical 真引用，含 `contract-change-evidence.md:29` 的 `S5-A2`——**该排除项必须出现在命中里**，缺失 = 命令转义退化空转假绿，R1-F1），例句位零残留 |
| 9 | post-commit | 既有包回归抽查：任取一存量 story 命中包跑 `node .claude/scripts/taskpackage-required-check.mjs` | 既有包不因新齿误红（存量 carve-out 按 T4 钉死的 gitFirstCommitDate < landing cutoff 谓词放行，抽查处置记录进 landing-summary） |
| 10 | post-commit | `grep -n "brief-inputs" .claude/agents/portfolio-planner.md` | 零命中（Step 4 ⑤ 生成句已删） |
| 11 | post-commit | `grep -nE "^7\. .*brief-inputs" .claude/agents/planner.md` | **Step 7 行内含 `brief-inputs.mjs` 调用 = 生成形态在位**（R2-F1 勘误：原 pattern token 与 260908 原形文面不咬合，改用「brief-inputs 出现在 Step 7 行」正向判据；「核在场」残留仅允许出现在描述拆包侧交接或按需重跑语义的句子，不得作为 Step 7 主语义） |
| 12 | post-commit | `grep -n "input-brief" .claude/skills/kidpal-pod-launch-prep/SKILL.md` | 步骤 8 的「双 brief 在场」项已删（若 grep 余命中，仅允许是无关上下文，逐处核对进 landing-summary） |

## 6. 留根清单

- **story↔acceptance 语义对账机械牙（ARQ-260927 候选 ③）**：human 2026-09-28 裁不做；ARQ 结项时记「③ 不做 + 理由」，非留根重开项。
- **brief 照录路的 `docs/requirements.md:S-F1.4` 冒号形态兼容**：task-intake 示例句法含冒号形态，story 照录路首轮只保证裸 `S-Fx.y` 与 `S-Fx.y-AC<n>` 两形态；冒号形态是否同路解析，landing 夹具实测后定，不进本批验收。
- **launch 前置机械汇总齿（OQ-2b）**：不预批；量化触发器 = Pod 内反查实测漏检 ≥1 例即立批。
- **crossEnd 黑盒/参考树两档消费方（ARQ-260909）**：与本批无关，留根不动。
- **交 retro 项**：T8 census 与 ARQ 详单的差集处置（排除 `contract-change-evidence.md` 的理由）是否需要在 ARQ 条目回写勘误。
- **260921 T5 裁定翻转记录**：本批 T9 翻转 `260921-gov-review-launch-pipeline` OQ-1(a)——该批 FROZEN 正文不回改（历史 append-only），翻转凭据 = 本 PROPOSAL §4 OQ-4 + 本批 landing-verdict；retro 时核对该批 workstream / review.md 是否需要补一行指针。
- **登记不硬治项（human 2026-09-28 立项会话裁：下一批再说）**：dev-loop landing-manifest——TaskPackage 文件级改动面预告 + closeout/合并门对账实际 diff。真设计分叉三条待裁（硬闸 diff⊆manifest vs advisory、实现期新增文件的 manifest 修订留痕机制、挂点 closeout-check vs supervisor-gate）；触发 = 下次动 taskpackage 格式 / closeout-check / supervisor-gate 闸面的治理批，或攒到一次实测证据（Pod 实际 diff vs 预告面差集）后；CRQ 条目 = `ARQ-260928-devloop-landing-manifest`。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-28 - architect
verdict: BLOCK(F1,F2,F3,F4)（无设计致命项；四条打回项均为证据精确性 / packet-binding 精度修，闭环后可转 GREEN）

**F1 [打回项] 验证集 #8 命令转义错误 → 空转校验**。§5-8 命令 `grep -rEn "S[0-9]-A[0-9]+\|Sx-Ay" ...` 中 `\|` 在 ERE 下是**字面竖线**（非 alternation），判官实测该命令形态全仓**零命中**（连应保留的排除项 `contract-change-evidence.md:29` 的 `S5-A2` 都不匹配）——照文执行会得到「零残留」假绿。T8 证据段所记 census 命令 `grep -rno "S[0-9]-A[0-9]+|Sx-Ay"` 无 `-E`，BRE 下 `+`/`|` 均为字面量、同样匹配不到任何东西，与 census 实际产出的 34 处命中矛盾（实跑必带 `-E`，转录丢失）。判官以 `grep -rnoE "S[0-9]-A[0-9]+|Sx-Ay"` 独立重跑：命中面与 PROPOSAL 详单**逐文件逐行一致**（census 本身准确）。改法：两处命令统一为 `-E` + 非转义竖线；#8 预期补一句「排除项 `contract-change-evidence.md:29` 须出现在命中里」（防转义回归后空转绿）。

**F2 [打回项] T3 ④ 的 Step 1① 引文失真，「改为」指令无对象**。T3 问题/证据引「`planner.md:39`（Step 1①）又说『以其 acceptance/decisionCoverage 为权威、不回退重归一』」——实测 planner.md:39 现文 =「命中已存在 portfolio taskpackage → 以其 `sourceType`/`origin`/`rigor`/`riskTier`/`gateProfile`/`decisionCoverage` 为权威，…不回退重归一、不覆盖 sourceType」，**不含 acceptance**；`grep -rn "acceptance/decisionCoverage 为权威" .claude/` 零命中（沿袭 ARQ-260927 证据 4 的误引）。后果：①「双写点字面张力」的 Step 1① 一腿建立在不存在的引文上（双写点问题本身仍由 `portfolio-planner.md:37/:40` ↔ `planner.md:47` 支撑，结论不倒）；② T3 ④ 目标文与现文几乎逐字相同（仅差 `rigor`），landing-fidelity 对「改为」找不到源文。改法：T3 ④ 改写为「对 `planner.md:39` 与 `task-intake-and-slicing.md:89`（F-06 同句）现文核对——现文已不含 acceptance 权威句，实际动作 = 核对确认 + 视需要补一句『acceptance 为 Pod 内产物、拆包期为空骨架』澄清」，删除失真引文。

**F3 [打回项] T3 ③ 核对步的相位落点未钉死**。核对三件（AC 回引号集一致 / story 补充规则落点 / 断言方向不弱于不钉反）的核对对象是**冻结后的 acceptance**（Step 8 才存在），但 T3 ③ 把「通读 story 全节 + 核对」整体挂在 **Step 1e**（intake 相位、acceptance 尚不存在）——照文落地会把「acceptance 冻结后核对」写成 intake 步指令，复刻本批要治的相位错位字面张力。§3 sweep 表虽列 8b，§2 正文未明说核对步落点。改法：明钉两半——Step 1e 保留「finalize 前通读 story 全节、当行为权威带进派发上下文」（喂 acceptance-author）；核对三件明钉 **Step 8b**（与 T4 扩 8b 为全 story 命中包同址、同口径互引），「核对不通过 → 退回 acceptance-author 重钉、planner 不代写」挂 8b。

**F4 [打回项] T4 存量 carve-out 谓词未设计，留到 landing = 违 packet-binding**。§5-9 只留策略名「Draft 态放行 / 存量白名单」、处置「记录进 landing-summary」——白名单的机判谓词（哪些包豁免新齿）属**闸设计**，FROZEN 后 landing 不得新增设计。改法：在 §2 T4 钉死谓词，建议对偶 DP-2 / CHANGE_ID_CUTOFF 先例（包目录 git 首提日期 < 本批 landing cutoff = 存量放行；cutoff 及之后建包 = 新齿适用），landing-summary 只记抽查处置结果。另注：Draft 态放行一臂与既有闸行为自洽（`taskpackage-required-check.mjs:224` 无条件必填下 Draft 态闸本就红——m4-k3 实包 `acceptanceTestsFile: []` 为证，acceptance 空骨架只加红名单项、不改红绿判定），这半句可留、须与白名单谓词分开写清。

**F5 [建议] T3 修订点漏列 portfolio-planner.md:95（Verification 节）**。「N 包 acceptance 并集覆盖 proposal 决策集」句在两处：Step 2（:37，已列删）与 Verification 节（:95，未列）——后者照 manifest 落地会留陈迹句、与 T3 矛盾。另：门 1 checklist ③ 现文（`task-intake-and-slicing.md:267`）=「decision-coverage 完备（含跨包行为 owner 显式登记）」，**本无 acceptance 并集字样**——T3 ①「删 acceptance 并集核」对 checklist 是无对象操作，实际删点只在 :37/:95。建议 T3 ① 文字对齐实况。

**F6 [建议] T2 ③ fail-loud 语义二义**。「节定位失败 fail-loud（stderr 报警 + 照录段如实标 not-found，不猜）」与脚本既有 fail-loud（exit 1、不产 brief——crossEnd 路 §8-B1）语义不一致：story not-found 走 advisory（产 brief 带 not-found 段）还是阻断（exit 1）？建议明写「advisory 通道、不 exit 1」（与第①路 missing 态同构），防 landing 实现二选一。

**F7 [建议] OQ-4 自标攻点宜自答**。① fail-loud 后移的 Pod 预算代价：可注缓解面——Step 1d crossEnd 权威判断 + Step 1f 拆包快照保鲜核验均在 Step 7 生成**之前**，残余风险仅剩拆包预填 counterparts 腐坏，代价有界；② launch 侧输入面可观测性：human 点火前仍可见 taskpackage refs + crossEnd + plan.md（brief 是其派生快照），实质损失有限。各补一句注记即可，非阻塞。

**核过无问题面（留痕一行清单）**：五段固定式齐（T7 证据诚实标未实测）；anchor 清单 18 == manifest 18；T2 行为断言亲证（`brief-inputs.mjs:62-67` splitRef 无 story 特判 + `:122-148` missing 态，源码通读成立；`docs/requirements.md` `##### S-Fx.y · <标题>` 形态 13 处在册、定位可行）；T3 ⑤ 闸调用相位独立复核成立（全仓调用点 = `verify-gates.json:34`〔Pod verifier 轮〕+ `closeout-check.mjs:303-307` 程序化 + planner Step 8 手工 + 文面引用，确无拆包期 / launch 期程序化调用点）；T8 census 权威性独立重跑逐文件逐行一致、`contract-change-evidence.md:29` 排除理由核实（真历史制品引用）；T9 与 260921 T5 落地面四处翻转位全在 manifest 内（`dev-loop.md:135` / `portfolio-planner.md:40` Step 4 ⑤ / `planner.md:61` Step 7 / SKILL.md:21 步骤 8 项），`brief-inputs.mjs:14` 头注与 `portfolio-planner.md:61` references 句现文本就是 Step 7 口径（260921 未改到、T9 后无需动、不在 manifest 亦无残留冲突面）；`--regenerated-because` 在 T9 后仍有语义（Step 7/9 按需重跑）；finalize 态 brief 在场查时序成立（Pod Step 7 生成 → Step 8 查在场）；sweep UNCLASSIFIED=0；OQ 完备（OQ-1/2/3 留裁、OQ-4 已裁记录形态）；验证命令入口在册、#5 flag 形态诚实标未实测；T5/T6/T7 与 ARQ-260924 建议 ①②③ 逐条对齐；T1 判据切开句与门 1 checklist 五项实文核对一致；Track B 路由成立（两 ARQ 原判 Track A、human 同场改裁且升级判据不命中，状态行留「存疑向严回升」）。

### R2 - 2026-09-28 - architect
verdict: GREEN（R1 四打回项 F1–F4 全部真闭环、三建议 F5–F7 全采纳；闭环动作引入两条新建议 R2-F1/R2-F2，非阻塞、建议主线程标 FROZEN 前顺手一行修掉，不构成再打回理由）

**R1 闭环复核（逐条）**：
- **F1 闭环 ✓**：T8 census 命令改 `-rnoE` + 转录丢 `-E` 勘误注记；§5-8 命令改非转义竖线形态，判官实跑 30 命中、排除项 canary（`contract-change-evidence.md:29` 的 `S5-A2`）确实在命中里——「缺失 = 空转假绿」反向判据可机判；§5-11 第二 grep 同步 `-nE`。
- **F2 闭环 ✓**：T3 问题/证据段删除失真引文、如实记「Step 1① 现文不含 acceptance（ARQ 证据 4 转引失真，R1-F2 勘误）」，双写点结论改由 :37/:40 ↔ :47 独立支撑；T3 ④ 改为核对确认 + 视需要补澄清句（澄清句显式标「建议件、landing 视上下文」——可选落件已明示为可选，不违 packet-binding）。
- **F3 闭环 ✓**：T3 ③ 两半明钉——Step 1e 保留通读义务（喂 acceptance-author、三条升级面原样）、核对三件明钉 Step 8b 且与 T4 扩 8b 同址互引、退回重钉挂 8b；§3 sweep 表 planner.md 行同步更新。相位自洽。
- **F4 闭环 ✓**：存量 carve-out 谓词 FROZEN 前钉死 = 包目录 `gitFirstCommitDate`（判官核实 `taskpackage-required-check.mjs:108` 既有函数在册、零新依赖成立）< 本批 landing commit 日期 → 放行；cutoff 常量属 landing 时点事实、谓词语义已完整钉死，对偶 DP-2 先例成立；§5-9 同步改引该谓词、landing-summary 只记抽查结果。
- **F5 闭环 ✓**（T3 ① 补列 :95 删点——判官核实 :37/:95 两处并集句均在册；门 1 checklist ③ 改记「现文本无 acceptance 并集字样、零改动」——:267 现文核实吻合）；**F6 闭环 ✓**（T2 ③ 钉死 advisory 通道、不 exit 1、与第①路 missing 态同构）；**F7 闭环 ✓**（OQ-4 两条缓解注记入 §4）。

**R2-F1 [建议] §5-11 第二 grep 的 pattern 与「260908 原形文面在位」预期不咬合**。`grep -nE "产双份|生成双 brief"` 的两个备选 token 均不出现在 260908 原形 Step 7 文面（原形 token = 「前置一次**产双 brief**」「两份都在本步**生成**」，见 260921 批 PROPOSAL T5 的 verbatim 引文）——若 landing 逐字恢复原形，该 grep **零命中**、#11 的「Step 7 为生成形态」正向证据落空（判官现跑亦零命中，无法区分「未落地」与「落地了但 token 不匹配」）。建议 pattern 换成与生成语义必然咬合的形态（如 `brief-inputs\.mjs` 出现在 Step 7 行内，或 `产双|双份.*生成`），或预期改写为「`brief-inputs.mjs` 在 Step 7 行在场即证生成形态」。一行修、非阻塞。

**R2-F2 [建议] §5-2 预期仍写「not-found fail-loud」，与 F6 闭环后的 advisory 语义打架**。T2 ③ 已钉死「不 exit 1、advisory 通道」，但 §5-2 夹具预期句保留旧词「not-found fail-loud」——本仓脚本词汇里 fail-loud = exit 1 阻断（crossEnd 路 §8-B1），landing 实现者照 §5-2 写自检断言会与 §2 T2 ③ 矛盾。建议改为「not-found advisory（不阻断、brief 仍产出）」。一词修、非阻塞。

**R2 核过无新问题面（留痕）**：v0.3 全文重读，§0/§1（无 §1 编号位，固定式无恙）/§2/§3/§4/§5/§6 与 v0.2 差集逐处核对——改动面恰为 R1 闭环七项 + 状态行升 v0.3，无夹带新设计；manifest 18 路径零变动；T3 ⑤ 补记的 `verify-gates.json:34` 调用相位与 R1 独立复核一致；T4 谓词不与 Draft 态放行臂混淆（两臂已分开写清）；sweep / OQ / 留根清单与 v0.2 语义零漂移。
