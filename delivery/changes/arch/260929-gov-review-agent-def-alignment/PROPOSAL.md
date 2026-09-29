# PROPOSAL · 260929-gov-review-agent-def-alignment

> **状态：FROZEN v0.4 · 2026-09-29 · human 盖章（会话内「其他的我没问题了」= integrator 裁定按 v0.4 落字后其余全部确认）。凭据：human 立项 ack（260929）+ architect R1 GREEN（review-packet「可批准（附条件）」）+ human 裁 OQ1–OQ5/E1–E5 十项 + 定稿期 integrator 零 spec 写权裁定（debate-log Step 5 + 定稿修正）**
> **承载 ARQ**：`ARQ-260929-agent-def-reality-drift`（T1–T5）+ `ARQ-260929-agent-def-provenance-noise`（T7）+ `ARQ-260929-competence-anchor-rework-evidence`（T8）+ `ARQ-260923-planner-roster-reference-drift`（T6，同车收口）；四根同源（agent def 面积欠账），一批清、防半截形态。
> **review 轨**：**Track A（full-adversarial，入口 `/arch-change`）**——命中机检硬排除：T2 触 G3 强制层（`.claude/scope-map.json` 写域变更）+ T7 批次混含 architect.md（self-def）与 review-loop def（第五阀）。非快路径，判据见两条 ARQ 条目「风险」段。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T9）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（T2 是 G3 权限面变更——**两腿执法层扩权**〔architect / review-moderator：现拦→后放，修复 260913 重构静默破坏的 spec 起草与低影响落地腿〕+ **integrator 零 spec 写权**〔human 定稿期裁定：spec 文本同步交 architect 承接，见 `ARQ-260929-docs-sync-spec-handoff`〕+ **一腿 deny 惰性**〔adjudicator：allow 非空、docs 根文件本就 allow-miss 被拦，deny 只改报错文案〕；**无收权腿**——OQ3/E3 裁 (b) 留根批次 2，见 §6；OQ4 已按此修正口径 human 放行〔debate-log Step 5 + 定稿期修正〕；其余为 G2 文本零行为变更）。
> **review profile**：G2+G3（触面枚举见 §3；G1 / 契约文件本体为零——architect.allow / review-moderator.allow 的 `docs/contracts/**` 契约写面收窄 OQ3/E3 裁留根批次 2，本批不动）。
> **第五阀安排（E5=(a) 落字）**：本批被改 def 含 architect.md（self-def）与 5 个 review-loop def（review-moderator / challenger / defender / adjudicator / architect-reviewer；scenario-author def 未在本批改动面）——R2 评审与 landing 核验由**不加载被改 def 的独立会话 / harness** 执行（DEV-GOVERNANCE §6 硬条款 4；charter SC-5）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：GREEN 结论前必须在场；条件锚（T5 的 OQ2、T6 的 OQ1）已按裁断解除（OQ2=(a) 全链删除、OQ1=(b) 补 def），本版全部锚为必选锚、manifest 已重算为各 T 目标文件并集。

```landing-manifest
.claude/agents/architect.md
.claude/agents/review-moderator.md
.claude/agents/architect-reviewer.md
.claude/agents/builder.md
.claude/agents/challenger.md
.claude/agents/fixer.md
.claude/agents/integrator.md
.claude/agents/planner.md
.claude/agents/acceptance-author.md
.claude/agents/portfolio-planner.md
.claude/agents/adjudicator.md
.claude/agents/defender.md
.claude/agents/landing-verifier.md
.claude/agents/uiux-author.md
.claude/scope-map.json
.claude/commands/arch-change.md
.claude/commands/review-arch.md
.claude/commands/build-feature.md
.claude/references/task-intake-and-slicing.md
.claude/references/arch-review-protocol.md
.claude/references/architect-operating-model.md
.claude/references/security-hardening.md
.claude/references/dev-loop.md
.claude/references/arch-review-rubric.md
.claude/skills/brainstorming/SKILL.md
.claude/skills/gov-arq/SKILL.md
.claude/route-fixtures/routing.json
.claude/route-fixtures/surface-alignment.json
```

---

## 0. 想实现的效果与差距

目标效果一句话：**agent def 面（16 个既有 def + 本批新增 uiux-author）与 scope-map，连同 commands / references / skills 治理文本与机检 fixture，回到与仓现实一致、无上仓遗留死条文、无内嵌过程修改记录的干净形态**（参照系 = story-author / requirement-reviewer 两个新写 def）。

现状差距（2026-09-29 主线程全量核查，机检 + 逐文件人读；域外命中经 challenger F1 实锤 + 回灌期全量重扫复核，census 见 T9 证据段）：① spec 面去层重构（docs/systems 删除，commit `ed2b353`）未扫进 4 个 def + scope-map 4 处，域外（commands / references / skills / route-fixtures 面）另有 docs/systems 形 19 行、zod 形 4 行、S2/K8 形 4 行、裸 contracts/ 形 12 行同构残留——integrator docs-sync 写 canonical spec 会被 enforce-scope 机械拦（实害）；② builder description 的 `apps/sX-*` 包形状本仓从未存在，description 是 orchestrator 模型级路由信号；③ S2/K8 等 9 处死条文引本仓不存在的工具；④ 9 个 def 内嵌 ≥58 处（census 下界）changeId / T 号 / 实证案例注记，每次派发全量进上下文。

**非目标**：不动 K6/correlationId 纪律（已本地化，observability-k6.md）；不收窄 architect.allow 的 G2 五面余量（architect.md 自记「收窄评估留批次 2」，本批留根）；不收窄 architect.allow 与 review-moderator.allow 的 `docs/contracts/**` 契约写面余量（OQ3=(b) / E3=(b)，留根批次 2，见 §6）；不改任何门判据、不新增任何 gate、不动 verifier/supervisor 退役编排；story-author / requirement-reviewer / scenario-author / challenger / landing-verifier 五个干净 def 除 T1/T4 点名行与 T7 的 description 收短（仅当仍超 lint 阈）外不动。

---

## 2. 问题、根因与改法（T1…T9）

**【硬项 · 起草期实证义务】** 本节全部 file:line 断言为 2026-09-29 实测（grep 命令与输出 verbatim 已录 `docs/crq/gov/ARQ-260929-agent-def-reality-drift.md` 证据段）；行号在评审与 landing 前须按「搬运引据视同新断言复证」纪律重核。回灌定稿期（2026-09-29）已按同纪律复证一次，两处复证 delta 如实记（T5 证据的 integrator:46 旁及句、T9 全量重扫 census），ARQ 证据段两处计数/标注偏差以 append 更正注记随批修复（F16）。

### T1 · docs/systems 旧 spec 路径清扫（def 侧）
**问题**：spec 面去层重构后，4 个 def 仍把 spec 草案落点 / 低影响落地目标 / 输出示例指到已删除的 `docs/systems/**`。
**证据**：`grep -rn "docs/systems" .claude/agents/` 共 **10** 行命中（F16 修正：v0.2 记 11 行，自列清单 3+4+3=10；回灌期复证一致）——architect.md:29/:53/:65（:53 同句自相矛盾：`docs/systems/**`（canonical spec = docs/requirements.md + docs/architecture.md））；review-moderator.md:20/:66/:73/:90；architect-reviewer.md:42（将来多系统展开注记）/:128/:146（输出示例）。`ls docs/` 无 systems 目录；`git log -- docs/systems` 顶端 `ed2b353`。
**根因**：260913 去层重构的清扫范围没覆盖 `.claude/agents/**`（重构批以 docs/ 面为主战场）。
**改法**：全部替换为 `docs/requirements.md` + `docs/architecture.md` 平铺表述——architect.md 设计面 / 起草落点、review-moderator spec profile 落地面、architect-reviewer 输出示例与将来展开注记统一改写；:42 的「将来多系统时按 `docs/systems/<sys>/` 展开」改「将来多系统时再立新面（届时先改本 def）」，保证 T1 完成后 grep 零命中。语义 = **同一既有语义换路径**（E1 确认：草案写 canonical 两文件、freeze 归 review-moderator spec profile）；**地板（F4，landing-verifier 核对项）**：architect.md 新路径表述必须原样保留「写入不等于 spec 已冻结」与「正式 spec soundness / 冻结判定归 review-moderator 的对抗 review（spec profile）」两句（HQ-1 门/约束句，T7 不得 prune）。
目标文件：`.claude/agents/architect.md`（改）· `.claude/agents/review-moderator.md`（改）· `.claude/agents/architect-reviewer.md`（改）

### T2 · scope-map spec 面写域对齐（G3）
**问题**：scope-map 四处写域指向已删目录。integrator.allow 不含现行两个 spec 文件（docs-sync 对 spec 面断腿）；architect.allow / review-moderator.allow 同病——spec 起草与 spec profile 低影响落地腿被重构静默砍断。**integrator 腿的处置 = human 定稿期裁定**：integrator 对 `docs/requirements.md` / `docs/architecture.md` **零写权**（需求面单写点纪律：requirements 写权维持 story-author / `/requirement-change` 单链；architecture.md 文本同步交 architect 承接），不授不修——流向登记 `ARQ-260929-docs-sync-spec-handoff`。
**证据**：`grep -n "docs/systems" .claude/scope-map.json` → :83（integrator.allow）/ :88（architect.allow）/ :102（review-moderator.allow）/ :119（adjudicator.deny）；`enforce-scope.mjs:56-66` 实读确认 universal.docsTopLevel 只管 docs/ 根新建白名单、不授 agent 写权。
**根因**：同 T1——重构批没扫 scope-map；adjudicator.deny 的旧路径是无效 deny（现状恰好无害，属碰巧不红）。
**改法**：四处 `docs/systems/**` 按语义替换——integrator.allow 去 `docs/systems/**` 且**不增任何 spec 文件**（零写权裁定；integrator.md 的 docs-sync 语义句在 T5/T7 触面内随批补一句「spec 面〔requirements / architecture〕漂移检出 → 交 architect 更新、零自改」）；architect.allow / review-moderator.allow 换为两个 spec 文件（扩权腿）；adjudicator.deny 换为两个 spec 文件（**deny 腿惰性**，F5 如实呈现：adjudicator.allow 非空、docs 根文件本就 allow-miss 被拦，deny 只改报错文案）。**OQ3/E3 裁定 = (b) 留根批次 2**：architect.allow 与 review-moderator.allow 的 `docs/contracts/**` 余量本批不动（approved-landing 遗留，留根见 §6——含 F18 收窄安全性论证），本批无收权腿。scope-map 镜像 fixture（`.claude/route-fixtures/surface-alignment.json:7/:12`）联动由 T9 承载。
目标文件：`.claude/scope-map.json`（改）

### T3 · builder 包边界表述与 zod 死条文
**问题**：builder.md description（模型级路由信号）与 When-to-Use 写 `apps/sX-*` 包形状——本仓从未存在（实现面 = firmware/server）；:56 zod 校验句非本仓栈。
**证据**：`grep -rn "apps/sX-\|zod" .claude/agents/` → builder.md:3/:17/:56。
**根因**：def 从上仓（原治理模板仓）迁移时按域改写不彻底，frontmatter description 是漏网面。
**改法**：description 与 :17 包边界改「TaskPackage subsystem 面 + changeManifest 清单」表述（与 Boundaries 节既有措辞同形）；:56 删 zod 句（防御纪律的实质已由 minimal-implementation.md 承载，无语义损失）。执行硬后盾（F8）：diff 按 authoring-agents-skills 纪律 3 **逐字段隔离**人读（description 改动只动 description，不误伤 name / tools）；`.claude/route-fixtures/routing.json` 补 builder / planner / landing-verifier 三行断言（F8 裁随批执行——三者原不在 fixtures，route-regression-check 对 builder 为零覆盖，§5 如实标注），补行后纳入断言闭合该盲区。
目标文件：`.claude/agents/builder.md`（改）· `.claude/route-fixtures/routing.json`（改，F8 补三行断言）

### T4 · challenger 影子路径写法统一
**问题**：challenger.md 两处写裸 `contracts/`（无 docs/ 前缀）——恰是 contracts rule 明文禁止的根级影子目录路径写法；challenger 拿它当漂移判据会去查不存在的目录。
**证据**：`grep -n "contracts/" .claude/agents/challenger.md` → :30 / :77（同文件 :22 / :47 已是 `docs/contracts/**` 正形）。
**根因**：影子目录禁令（260827 契约归位期）落 rules 与 scope-map 时未回扫 def 正文。
**改法**：:30 / :77 统一 `docs/contracts/**`，纯替换零语义变化。
目标文件：`.claude/agents/challenger.md`（改）

### T5 · S2/K8 上仓死条文处置
**问题**：builder / fixer / integrator / planner 引 S2 exploratory scan（`run_inline_check` / `get_run_results`）与 K8（`write_knowledge` / `knowledgeSuggestions`）——本仓无对应 MCP 工具，属死条文；agent 实跑时会找不到工具或自适应跳过。
**证据**：builder.md:38/:55、fixer.md:36/:53、integrator.md:39/:40/:41/:46/:102/:128（回灌期复证：:46 的 `knowledgeSuggestion` 旁及句一并入列，v0.2 清单外 delta）、planner.md:53（s2Exploratory* 字段句）与 planner.md:193-195（s2ExploratoryRequired/Reason/Goals 示例字段三行——判官 grep 实证 delta，v0.2 与 challenger 均未列）。K6/correlationId 不在列（已本地化）。
**根因**：同 T3——上仓工具面（S2 扫描、K8 知识库）未随迁移裁剪。
**改法**：**OQ2 裁定 = (a) 删除（全链同删）**——builder/fixer 各 2 行；planner.md:53 的 s2Exploratory 字段句 + :193-195 示例字段三行；integrator S2 三步整段（:39-:41）+ 旁及句（:46/:102/:128）；联动同步：`task-intake-and-slicing.md` :412（起包 convention 例证句）与 :245（可省清单的 S2 重闸 token）、`.claude/commands/build-feature.md` :21（拆包侧 s2 声明指令）/:30（S2 official scan 指令）、`.claude/references/dev-loop.md` :174（字段定义全文）。task-intake:338 复证为 designRefs 示例（docs/systems 形、非 s2 字段段）——归 T9 处置。B3 结论随录（challenger 查清）：s2Exploratory / run_inline_check 无脚本 / 闸消费（scripts / hooks / verify-gates 零命中），删除无闸红风险。
目标文件：`.claude/agents/builder.md`（改）· `.claude/agents/fixer.md`（改）· `.claude/agents/integrator.md`（改）· `.claude/agents/planner.md`（改）· `.claude/references/task-intake-and-slicing.md`（改）· `.claude/commands/build-feature.md`（改）· `.claude/references/dev-loop.md`（改）

### T6 · uiux-author 编制裁定（承接 ARQ-260923）
**问题**：planner 可派编制引用 uiux-author（frontmatter Agent 列表 + Step 6b）与 acceptance-author:23、task-intake L2 行，而 `.claude/agents/uiux-author.md` 从未落地——L2 UI 包派发必失败一次后降级（260915-m1-k3 实证，loop-retro-2026-09-23 在册）。
**证据**：ARQ-260923 条目原文 + 本次核查追注（2026-09-29，复确认 16 个 def 仍无该文件；route-regression-check 只断言白名单不退化、不查存在性，机检不可捕获）。引用全量 7 处（F9 复核，回灌期复证一致）：planner.md:4（frontmatter Agent 列表）/:61（Step 6b）、acceptance-author.md:23、task-intake:320/:326、dev-loop.md:55/:68。
**根因**：编制引用与编制文件双轨维护、无存在性机检——从未落地的引用不会被任何闸拦。
**改法**：**OQ1 裁定 = (b) 补编制**——① 新增 `.claude/agents/uiux-author.md`（按 task-intake UIUX Ladder L2 职责面起草 def 是 landing 期工作，本 PROPOSAL 锚其新增）；② `.claude/scope-map.json` 增 uiux-author 条目（写域随 def 起草定形，landing 期落）；③ 上述 7 处引用**保留并核对指向一致**，`grep -rn "uiux-author" .claude/` 兜底；④ planner.md:61 的 uiux-designer 同句引用带「未接线」限定词（DEV-GOVERNANCE §8「dispatch 尚未接线」在册；G1 编制表行本批不动，留根见 §6）。镜像 fixture（surface-alignment.json）是否随 scope-map 新条目同步，按其「与 scope-map 对齐」语义在 landing 期一并核对（该件已在 T9 锚内）。
目标文件：`.claude/agents/planner.md`（改）· `.claude/agents/acceptance-author.md`（改）· `.claude/references/task-intake-and-slicing.md`（改）· `.claude/agents/uiux-author.md`（新增）· `.claude/references/dev-loop.md`（改，引用核对）· `.claude/scope-map.json`（改，增条目）

### T7 · 过程修改记录噪音清理
**问题**：9 个 def 内嵌 ≥58 处（census 窄 pattern 下界）changeId / T 号 / 实证案例点名注记；agent def 是每次派发全量进上下文的常驻文本——纯 token 债 + 诱导照历史个案抄近路；另有 review-moderator / adjudicator / defender 的散文型注记未被 census 捕获（人工复核在册，ARQ-260929-agent-def-provenance-noise 证据段）。
**证据**：census 逐文件计数（planner 28 / acceptance-author 8 / portfolio-planner 7 / integrator 5 / builder 3 / architect 3 / fixer 2 / review-moderator 1 / architect-reviewer 1）+ 散文型清单，见该 ARQ 条目。
**根因**：每批 landing 在 def 内留「哪次改的、哪个实证」注记成惯例；溯源功能 git blame 与 `delivery/changes/arch/**` 制品已完整承载，def 内属重复。
**改法**：以 story-author / requirement-reviewer 为目标形态逐文件清理——**约束句留、溯源注记删**；底线 = prompt-authoring「Pruning 硬边界（HQ-1）」（权限 / 门 / authority / scope / 升级 / must-never 句一个不删）；**trim-not-delete 条款（F12，执行纪要硬条款）**：defender.md:47 留 must-never 禁令句（「必须用 Write 工具落盘，禁止用 Bash 重定向写」）、只去括注历史教训；review-moderator:12/:104、adjudicator 卡点 D 注记同理逐条按 HQ-1 判据判（删掉是否改变谁能做什么 / 何时必须停）；消歧必需注记保留（如 adjudicator「原 ARCH-REVIEW-QUEUE」指路）；planner.md:67 内 [MODEL-TIER] / [DISPATCH-STAMP] 等承重运行时合同整段**不属于噪音、不动**。landing-verifier 若仍超长（lint 基线 1 条 warn，363>320）顺带收短 description。改后复跑两 lint（§5）。触面 G2 def ×12（本项自身清单；§3 的 def 并集 13 系 T1–T7 全项并集，charter §0「12 个」同误——F16 注记在案，charter 已冻结不改）。
目标文件：`.claude/agents/planner.md`（改）· `.claude/agents/acceptance-author.md`（改）· `.claude/agents/portfolio-planner.md`（改）· `.claude/agents/integrator.md`（改）· `.claude/agents/builder.md`（改）· `.claude/agents/architect.md`（改）· `.claude/agents/fixer.md`（改）· `.claude/agents/review-moderator.md`（改）· `.claude/agents/architect-reviewer.md`（改）· `.claude/agents/adjudicator.md`（改）· `.claude/agents/defender.md`（改）· `.claude/agents/landing-verifier.md`（改）

### T8 · 能力透镜锚（返工实测驱动，两处过线）
**问题**：agent def 普遍只有流程义务、无专业方法透镜；Pod 实际执行模型经降档（glm-5.3-flash 等）后，「模型天然懂」假设不成立。但按 human 260929 裁定判据「该角色的返工实测集中在哪个能力段，就锚哪段」，全量返工普查（57 条 friction + 25 条 escalation + verifier 轮次红数 + retro 主题对读，证据录 `ARQ-260929-competence-anchor-rework-evidence`）只有**两处过线**。
**证据**：acceptance-author 三段复发——冻结验收断言两向皆红/自相矛盾 ×3（schedule-reminder 钢琴课时间簇、m4-k1 fixture 缺 feed、screen-unification 验收↔PROPOSAL 冲突）、seam 闭环漏钉（m2-k3 设备侧驱动零验收断言）、host/真机覆盖差集（m1-n1b，5 红/12 轮）；portfolio-planner 影响面枚举漏扫（m2-k3 sourceRefs 漏四处 + retro T2 快照腐坏跨棒复发）。反证：planner / builder / fixer 的对应症状已有流程承载（Step 8b / C5 / 2c），不加防重复。
**根因**：def 撰写纪律（prompt-authoring）只管删废话与留权限句，没有「专业透镜何时值得写」的判据；经验上专业方法一直靠模型自带，降档后缺口放大。
**改法**：**OQ5 裁定 = (a) 两处都加**——Leading Words 一行式，不写方法论正文，按 moderator 已呈 human 的原文落锚：① acceptance-author.md **Process 第 6 步旁**加「**断言设计透镜**」行：两向可满足性推演（红基线为何红 + 转绿路径存在；验收断言自含性显式声明）+ seam 闭环（每个行为 seam 列驱动方与消费方，缺驱动方 = 盲区申报）+ host/真机近似假设显式化；② portfolio-planner.md **Step 2 旁**加「**影响面机械辅证**」行：sourceRefs / docRefs 枚举须 trace_path / get_architecture 交叉验证，不信脑内清单。**显式去重（F15）**：两行行尾均加「语义权威仍以 acceptance-evidence.md 与 DEV-GOVERNANCE 验收测试纪律为准，引用不复制」——与 acceptance-author 既有承载（:36 红基线步、:45-46 引验收纪律 §2/§3）引用或收编、不并列两套。
目标文件：`.claude/agents/acceptance-author.md`（改）· `.claude/agents/portfolio-planner.md`（改）

### T9 · 域外同构残留清扫（commands / references / skills 面 + 机检 fixture）
**问题**：T1–T5 的同构残留不止 agents + scope-map 两面——commands / references / skills 面与 route-fixtures 镜像同样命中（challenger F1 实锤，v0.2 §3「UNCLASSIFIED=0」被证伪）；若只清 agents + scope-map，T1 根因（260913 清扫漏面）同构复发，FROZEN 后欠账即刻复发。
**证据**（回灌期全量重扫 census，2026-09-29，全 `.claude` 面，grep verbatim 计数）：
- `docs/systems` 形共 33 行，agents 面 10 行（T1）+ scope-map 4 行（T2）之外**域外 19 行**：`.claude/commands/arch-change.md`:24/:34/:40、`.claude/commands/review-arch.md`:3/:20/:28、`.claude/references/arch-review-protocol.md`:12/:37/:138、`.claude/references/architect-operating-model.md`:26/:72/:75/:79/:110/:119、`.claude/references/task-intake-and-slicing.md`:338（designRefs 示例的 `docs/systems/<S>/UIUX-DESIGN-BRIEF.md`——**地板件**，改指本仓现实路径 `docs/ui/` 设计母版面）、`.claude/skills/brainstorming/SKILL.md`:59、`.claude/route-fixtures/surface-alignment.json`:7/:12（**地板件**，scope-map 镜像，T2 改后必漂）。
- `zod` 形域外 4 行：`.claude/references/dev-loop.md`:6/:110/:303、`.claude/references/security-hardening.md`:7（agents 面 builder.md:56 归 T3）。
- S2/K8 形（run_inline_check / get_run_results / write_knowledge / knowledgeSuggestions / s2Exploratory）域外 4 行：`.claude/commands/build-feature.md`:21/:30、`.claude/references/dev-loop.md`:174、`.claude/references/task-intake-and-slicing.md`:412（agents 面各行归 T5）。
- 裸 `contracts/` 形（非 docs/ 前缀路径引用）域外 12 行：`.claude/references/dev-loop.md`:6/:177/:235/:303/:305、`.claude/references/arch-review-protocol.md`:188/:193、`.claude/references/arch-review-rubric.md`:21（`contracts/README.md` 上仓遗留）、`.claude/skills/gov-arq/SKILL.md`:12/:44/:76/:88。**非清理命中如实注记**：hooks / scripts / rules 面的裸 `contracts/` 命中（enforce-scope.mjs:85 影子绕过注释、supervisor-verdict-gate.mjs:87-88 critical 闭集注记、contract-lint.mjs / pre-merge-escalation-gate.mjs 注释、repo-structure.md:40 docs 子目录名枚举、scope-map denyCreate `contracts/**` 影子拦截）均为合法形态、不在本批 T 面；planner.md:55 / challenger.md:27 / acceptance-author.md:30 / change-triage.md:43 / architect-operating-model.md:57 的「contracts/spec」为 prose 泛指、非路径引用，不入清理。
**根因**：260913 去层重构与上仓迁移的清扫范围都没扩到全 `.claude` 面（T1 根因的结构性复发面，challenger F1 / charter C6）。
**改法**：按各 T 项已裁定语义在域外面同构执行——docs/systems 形改 `docs/requirements.md` + `docs/architecture.md` 平铺表述（E1 确认语义：草案写 canonical 两文件、freeze 归 review-moderator spec profile；def 侧冻结语义两句由 T1 地板承载）；zod / 上仓栈形按 T3 语义改写或删死句；S2/K8 形按 T5 语义（OQ2=(a)）全链删除；裸 `contracts/` 形按 T4 语义统一 `docs/contracts/**` 正形（`contracts/README.md` / `contracts/shared-types` 等上仓遗留例证改指本仓现实 `docs/contracts/interface-contract.md` 形态）；`surface-alignment.json`:7/:12 随 T2 换路径同步镜像（`docs/contracts/**` 两腿按 OQ3=(b) 留根、维持现状不动）。**终清单判定依据 = human E4=(a) 裁定所列文件 + challenger F1 清单 + 本 census 重扫新增两件**（arch-review-rubric.md / gov-arq SKILL.md——F1 的 grep 未扫裸 contracts/ 形，按 human「先全量重扫取证再定终清单」指令入账；两件均 commands/references/skills 面同构命中，moderator 核验时如认为超界可划除并相应缩 manifest）。非斜杠形（裸 contracts/、点前缀粘连等 path-form-family 八类形态）由 §5.1 族谱复扫硬前置逐类覆盖（F13）。
目标文件：`.claude/commands/arch-change.md`（改）· `.claude/commands/review-arch.md`（改）· `.claude/commands/build-feature.md`（改）· `.claude/references/arch-review-protocol.md`（改）· `.claude/references/architect-operating-model.md`（改）· `.claude/references/security-hardening.md`（改）· `.claude/references/dev-loop.md`（改）· `.claude/references/task-intake-and-slicing.md`（改）· `.claude/references/arch-review-rubric.md`（改）· `.claude/skills/brainstorming/SKILL.md`（改）· `.claude/skills/gov-arq/SKILL.md`（改）· `.claude/route-fixtures/surface-alignment.json`（改，T2 镜像联动）

**Review 定稿记录**（architect 回灌 · 2026-09-29）

- **回灌来源**：`decision-packet.md`（R1 CONFIRMED delta + human 问题集裁决表）+ `findings.md`（F1–F21 逐条裁定：CONFIRMED 20 / DISMISSED 1 / DEADLOCK 0）+ `debate-log.md` Step 5（human 拍板：OQ4 放行〔F5 修正口径〕/ E4=(a) / OQ3=(b) / E3=(b) / OQ1=(b) / OQ2=(a) / OQ5=(a) / E5=(a) / E1 确认默认 / E2=(a)；human 写作纪律 = 散文禁用「齿」，本 PROPOSAL 全文含新写段遵守，引既有制品原文照录不改）。
- **回灌动作**：E4 追加 T9（含回灌期全量重扫 census 与终清单判定）；manifest 重算 = 各 T 目标文件并集（28 锚）；T5/T6 条件锚解除（OQ2=(a) / OQ1=(b)，全部锚为必选锚）；OQ3/E3 转留根（§6 两项，F18 论证随录）；F5 riskTier 口径改写 + OQ4 放行落字；F1/F2/F3/F6/F7/F8/F9/F12/F13/F15/F16/F18/F19 逐条落实（§0 / §3 / §5 / §6 与各 T 项内）；E5 第五阀安排头部落字；F16 计数修正（T1 证据 10 行、§3 def 并集 13、ARQ 证据段两处更正注记随批 append 执行）。
- **定稿边界**：只机械回灌已批改动与判官裁定 delta，不夹带新设计；FROZEN 待 human 盖章（architect 不自冻）；盖章后由 review-moderator 收口核验（frozen PROPOSAL ↔ decision-packet ↔ findings ↔ human 裁决）+ `proposal-consolidation-check.mjs`，再交主线程 packet-bound landing。

---

## 3. 影响面 sweep 分类（回灌定稿版；域外命中已全量重扫入账，见 T9 census）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G2 def | .claude/agents/ 下 13 个既有 def（T1/T3/T4/T5/T6/T7 并集）+ uiux-author.md 新增（T6） | G2 | 消费方 = 主线程派发 / planner / review-moderator 的 Agent() dispatch；文本变更零行为语义（T2 写域与 T6 编制补全除外）。def 并集 = 13（F16 修正；charter §0「12 个」同误，注记在案）；并入 T6 新增件后本批 def 触面共 14 |
| G3 强制层 | .claude/scope-map.json | G3 | 消费方 = enforce-scope.mjs 钩子（机判）；两腿扩权（architect / review-moderator）+ integrator 零 spec 写权（定稿期 human 裁定，ARQ-260929-docs-sync-spec-handoff）+ 一腿 deny 惰性（OQ4 放行口径 v2），无收权腿 |
| G2 command | .claude/commands/arch-change.md / review-arch.md / build-feature.md | G2 | T5/T9 联动（s2 指令全链删除 + spec 草案落点平铺表述）；消费方 = 主线程入口编排 |
| G2 reference | .claude/references/task-intake-and-slicing.md / arch-review-protocol.md / architect-operating-model.md / dev-loop.md / security-hardening.md / arch-review-rubric.md | G2 | T5/T6/T9 联动（s2 字段定义、uiux-author 引用核对、域外同构清扫）；消费方 = planner / portfolio-planner intake 与 review-loop 编排 |
| G2 skill | .claude/skills/brainstorming/SKILL.md / gov-arq/SKILL.md | G2 | T9（docs/systems 形 / 裸 contracts/ 形） |
| G3 机检 fixture | .claude/route-fixtures/routing.json / surface-alignment.json | G3 | F8 补三行断言 / T2 镜像同步；消费方 = route-regression-check.mjs 与人读对账 |
| G1 / 契约面 / hooks·scripts·rules | 无 | 未触 | 契约文件本体零触碰；hooks / scripts / rules 面重扫命中均为合法形态（T9 census 注记），不入本批 |

**sweep 透镜**：本批无新门新断言，无 census 新增义务；复扫模式扩至**全 `.claude` 面**（T9 census 同款四模式）——`grep -rn "docs/systems" .claude/`、`grep -rn "apps/sX-\|zod" .claude/`、`grep -rn "run_inline_check\|get_run_results\|write_knowledge\|knowledgeSuggestions\|s2Exploratory" .claude/`、裸 `contracts/` 形态扫描；path-form-family 八类形态复扫升 §5 编号硬前置（F13，见 §5.1）。

## 4. human 取舍点（OQ）——已全裁（debate-log Step 5，2026-09-29）

- **OQ1 · uiux-author 编制 = (b) 补 def**（human 选补编制，非推荐项）：T6 改为「补编制」——新增 uiux-author.md + scope-map 增条目 + 7 处引用核对指向一致。
- **OQ2 · S2/K8 死条文 = (a) 全链删除**：T5 联动清单按判官真身重列（含 planner.md:193-195 示例字段三行 delta）。
- **OQ3 · architect 契约写面余量 = (b) 留根批次 2**：本批不收窄；§6 留根立项（owner = architect、触发 = 批次 2 / 下一批触 scope-map 契约写面的治理批），F18 收窄安全性论证随录在案。
- **OQ4 · G3 写域变更确认 = 放行**：按 F5 修正口径——「三腿执法层扩权（integrator/architect/review-moderator：现拦→后放，修复 260913 重构静默破坏的 docs-sync 成文职责）+ 一腿 deny 惰性（adjudicator）」，无收权腿；riskTier 理由句已按此改写（头部）。**定稿期修正（human 盖章前审阅裁定）**：integrator 腿移除——integrator 对两 spec 文件零写权、spec 文本同步交 architect 承接（ARQ-260929-docs-sync-spec-handoff），口径收敛为「两腿扩权 + integrator 零写权 + 一腿惰性」；riskTier 句已同步改写。
- **OQ5 · 能力透镜锚点集 = (a) 两处都加**：T8 按 moderator 已呈 human 的原文落锚，与既有承载显式去重（F15）。
- **E1 · spec 草案落点 = 维持现行成文语义**（确认项）：草案写 canonical 两文件、freeze 归 review-moderator spec profile；T1 保留冻结语义两句（F4 地板）。
- **E2 · agent 存在性机检归属 = (a) 立 ARQ**：owner = architect、触发 = 下一批触 planner / Agent() 白名单的治理批（§6 留根登记；ARQ 登记动作归主线程）。
- **E3 · review-moderator 契约写面 = (b) 留根**：与 OQ3 同批留根（§6 单列一项、触发同款）。
- **E4 · 域外残留入账 = (a) 同批追加 T 项**：新增 T9——地板两件必入（task-intake:338 + surface-alignment.json:7/:12），commands / references / skills 面按 E1 语义与各 T 同构语义改写，全量重扫 census 落 T9 证据段。
- **E5 · 第五阀执行形态 = (a) 独立会话 / harness**：R2 评审与 landing 核验由不加载被改 def 的独立会话 / harness 执行（头部「第五阀安排」落字）。

## 5. 验证集（landing 前机械前置 + landing 后核验；时点均 post-commit）

1. **形态族谱复扫（F13，硬前置——先于下列 grep 断言跑）**：按 `.claude/references/path-form-family.md` 八类形态逐类子检本批全部换路径面——引号分段形 / 转义形（如 `docs\/systems`）/ 相对链接形至少覆盖（260914 教训：路径批最隐蔽一类四轮机检全漏）。
2. `grep -rn "docs/systems" .claude/` → 零命中（T1/T2/T9；全 .claude 面）。
3. `grep -rn "apps/sX-\|zod" .claude/` → 零命中（T3/T9）。
4. `grep -n "contracts/" .claude/agents/challenger.md` → 命中行全部为 docs/contracts/ 形态（T4）。
5. `grep -rn "run_inline_check\|get_run_results\|write_knowledge\|knowledgeSuggestions\|s2Exploratory" .claude/` → 零命中（T5/T9，OQ2=(a) 全链删除——含 planner.md:193-195 示例字段三行、task-intake:412 例证句与 build-feature / dev-loop 域外行）。
6. `python -m json.tool .claude/scope-map.json` → 解析通过（T2）。
7. **enforce-scope 行为矩阵（F3，正反双向）**：构造钩子 stdin JSON（agent_type / tool_input.file_path）喂 `node .claude/hooks/enforce-scope.mjs`——① architect → `docs/architecture.md` → exit 0（扩权腿正向）；② review-moderator → `docs/architecture.md` → exit 0（扩权腿正向）；③ integrator → `docs/requirements.md` 或 `docs/architecture.md` → block（零写权裁定实证）；④ integrator → `docs/contracts/**` 任一契约文件 → exit 2（deny 腿仍咬）；⑤ adjudicator → 任一 docs 根文件 → block（allow-miss 惰性实证）。OQ3 收窄用例（architect → `docs/contracts/**` → exit 2）随 OQ3=(b) 留根批次 2，不在本批。
8. `node .claude/scripts/route-regression-check.mjs` → 0 fail。**覆盖域如实标注（F8/F19）**：builder / planner / landing-verifier 原不在 fixtures（对 builder 为零覆盖），本批 routing.json 补三行后纳入断言；正文消歧句与散文注记仍盲——由第 10 条人读腿承载。
9. `node .claude/scripts/lint-descriptions.mjs` → flag 数 ≤ 基线 1（T7 后 landing-verifier 超长 warn 应消除，目标 0）。
10. T7 census 复跑（同 ARQ 条目 pattern）→ 承重运行时合同段与消歧注记外零命中；半机械断言，逐文件人读 diff + landing-verifier 核对承载；**核对输入补「按 ARQ-260929-agent-def-provenance-noise 散文清单逐条销账」（F12/F19）**。
11. T8 验证锚（F15，OQ5 定稿措辞固定子串）：`grep -n "断言设计透镜" .claude/agents/acceptance-author.md` → ≥1 行命中；`grep -n "影响面机械辅证" .claude/agents/portfolio-planner.md` → ≥1 行命中；两 lint 复跑不劣化。

## 6. 留根清单

- **story-author 能力透镜观察位**——按「返工实测」判据零数据不加锚；触发 = 首个 requirement-change 批次 retro 后，若起草返工集中在某能力段（如歧义点识别 / 牵连面枚举），按本批 T8 同形加一行透镜（/loop-retro 分流）。
- **architect.allow G2 五面余量收窄**（`.claude/agents/**` 等 approved-landing 遗留）——owner = architect、触发 = 批次 2（architect.md:69 自记在案）。
- **architect.allow 的 `docs/contracts/**` 契约写面收窄（OQ3=(b)）**——owner = architect、触发 = 批次 2 / 下一批触 scope-map 契约写面的治理批。**F18 论证在案（供下批裁断）**：收窄后 architect 契约预审合法写面仍有落点——`delivery/changes/arch/*arch-review-*/**` 与 `*gov-review-*/**` 保留（scope-map:88）、authoring 面 `.claude/references/**` 在 G2 五面内、契约本体恒 human 门 + 主线程 atomic PR；收窄方向安全。
- **review-moderator.allow 的 `docs/contracts/**` 契约写面收窄（E3=(b)）**——与上一条同性质 approved-landing 遗留（review-moderator.md:90 自述不改契约）；owner = architect、触发 = 与 OQ3 同批（批次 2 / 下一批触 scope-map 契约写面的治理批）。
- **agent 存在性机检缺位（E2=(a) 立 ARQ）**——route-regression-check 不查 Agent() 白名单被引 agent 存在性（T6 根因）；owner = architect、触发 = 下一批触 planner / Agent() 白名单的治理批；ARQ 登记动作归主线程（本批 landing 期落条目）；policy-as-code 方向 = route-regression-check 补牙，与 routing.json `_doc` 在册 DEFER 对齐。
- **uiux-designer 编制接线与 G1 编制表行**——DEV-GOVERNANCE §8「dispatch 尚未接线」在册；本批 T6 只以「未接线」限定词在 planner.md:61 在案、G1 编制表行不动；owner = architect、触发 = UI 批真正需要设计审查编制或下批触 DEV-GOVERNANCE 编制面时。
- **K6/correlationId**——已本地化，不在清理范围。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

