# PROPOSAL · 261015-gov-review-changes-taxonomy

> **状态：FROZEN v0.4 · 2026-10-16 · 凭据三件联立：human 立项 ack（2026-10-15 主线程会话）+ architect R4 GREEN（熔断后新判官实例，§7 R4 记录，verdict 行行首机器锚可消费）+ human 裁 OQ 结果（OQ-A 选 (b) / 两波程序变体 / 滞留阈值 7 天，§4 裁定行）；主线程凭据自冻，授权记本 Status 行**（版本演进：v0.1 起草 → v0.2 R1 修订〔F1 sweep 漏件 7 类模式 + 7 件入面入锚 + F2–F4 顺修〕→ v0.3 R2 修订〔architecture.md :278 入改写面 + V4 设计保留位三类并集 + 行号回改 + 两 NIT〕→ v0.4 R3 修订〔7 件补入 §2 T6 目标文件行 + preflight 注记现态口径 + isProposalMdPath 函数名勘误〕；熔断留痕：R1–R3 三轮未 GREEN 触发 human 三选一，已裁「换新判官实例再战 1 轮」→ R4 GREEN）。
> **承载 ARQ**：无（human 2026-10-15 主线程会话直接发起；human 原话四段见 §0 证据锚，改判记录见 §4 第 5 条）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。Track A 段判据只看收敛度——本批方向已由 human 2026-10-15 四裁定收敛（§4 照录），不命中升级判据；判档终裁归 architect-reviewer，只升不降。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium——平移面大（整树 309 件 + 活引用约 50 件），但改动纯机械（git mv + 路径常量与文面替换 + 索引再生），261014-gov-review-dev-lane-merge-taskpackages 同类 dev lane 迁移已绿先例；双齿 + 合并闸结构检查兜底防复发；契约面零写；行为变化有界（新增闸检查只拦废止路径、intake INDEX 只检测不拦截）。
> **review profile**：G1+G2+G3——G1 = `AGENTS.md` + `docs/DEV-GOVERNANCE.md`（路径与 lane 措辞机械刷新，零语义变更）；G2 = agents 9 件 / commands 5 件 / references 14 件 / rules 1 件 / skills 5 件；G3 = scope-map / hooks 4 件 / scripts 11 件（含 lib 1）/ route-fixtures 1 件；另含文档面（`docs/templates/**`、`docs/notes/**`、`docs/showme/README.md`）与 spec 面两处指针勘误（`docs/requirements.md` §9 头注 + `docs/architecture.md` :278 仓结构行，同型同通道——均走档 0 勘误通道）；契约面零写（实测 3 处历史链接的处置归 §4 OQ-A）。派发书建议按 G2/G3 圈——起草期 census 实测 G1 两文件在改写面内，如实升记，判官终裁。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。
> manifest 口径：只记内容批改件 + 可枚举平移迁入件（intake 9 件 / retro 3 件 / T1 归位 4 件逐件入锚）+ 新增件 + 再生件（两份 INDEX 与状态缓存）；**archive 整树（约 300 件）与 review/ 在途批前移（落地窗口定名单）为目录级搬移，不入锚**（先例 261014 T1 / 260914 T1：目录级迁入不可机判，搬移对账由 §5 V7/V8 承载）。

```landing-manifest
.claude/scope-map.json
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/scripts/brief-inputs.mjs
delivery/taskpackages/261015-voice-rightcol-label-overflow/review-packet.md
delivery/taskpackages/261015-voice-rightcol-label-overflow/supervisor-verdict.json
delivery/taskpackages/261015-voice-rightcol-label-overflow/pod-retro.md
delivery/taskpackages/261015-voice-rightcol-label-overflow/docs-delta.md
.claude/rules/repo-structure.md
.claude/references/change-intake.md
docs/templates/debt-item.template.md
.claude/scripts/changes-index.mjs
.claude/agents/integrator.md
.claude/commands/build-feature.md
.claude/commands/arch-change.md
.claude/commands/gov-change.md
.claude/commands/requirement-change.md
.claude/commands/loop-retro.md
.claude/references/requirement-change.md
.claude/scripts/landing-fidelity-check.mjs
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/portfolio-reconcile.mjs
.claude/scripts/proposal-consolidation-check.mjs
.claude/scripts/settings-lint.mjs
.claude/scripts/arch-review-handoff-check.mjs
.claude/scripts/devloop-artifact-check.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/hooks/commit-safety.mjs
.claude/hooks/enforce-scope.mjs
.claude/hooks/suggest-codebase-memory.mjs
.claude/hooks/supervisor-verdict-gate.mjs
.claude/route-fixtures/surface-alignment.json
.claude/agents/adjudicator.md
.claude/agents/architect.md
.claude/agents/challenger.md
.claude/agents/defender.md
.claude/agents/landing-verifier.md
.claude/agents/planner.md
.claude/agents/review-moderator.md
.claude/agents/scenario-author.md
.claude/references/arch-review-protocol.md
.claude/references/arch-scenarios.md
.claude/references/architect-operating-model.md
.claude/references/change-triage.md
.claude/references/dev-loop-operating-system.md
.claude/references/dev-loop.md
.claude/references/plain-file-handoff.md
.claude/references/path-form-family.md
.claude/references/pod-coordination.md
.claude/references/task-intake-and-slicing.md
.claude/references/taskpackage-authoring.md
.claude/references/track-b-verifier-brief.md
.claude/skills/brainstorming/SKILL.md
.claude/skills/gov-arq/SKILL.md
.claude/skills/kidpal-idf-env/SKILL.md
.claude/skills/merge-to-main/SKILL.md
.claude/skills/opendesign-connect/SKILL.md
AGENTS.md
docs/DEV-GOVERNANCE.md
docs/requirements.md
docs/architecture.md
docs/notes/ui-implementation-strategy.md
docs/showme/README.md
docs/templates/PROPOSAL.template.md
docs/templates/architect-landing-summary.template.md
docs/templates/retro.template.md
docs/templates/GOVERNANCE-MIGRATION-GUIDE.md
delivery/intake/260928-char-window-key-triggered-play.md
delivery/intake/260929-char-window-v5-trim.md
delivery/intake/261001-m4-k3-record-session-live-repair.md
delivery/intake/261010-p3-storylist-cold-start.md
delivery/intake/261011-p1-home-v5-rework.md
delivery/intake/261012-kidpal-realtime-facts.md
delivery/intake/261013-gov-review-lf-eol-teeth-pin.md
delivery/intake/261014-single-tier-bringlist-home.md
delivery/intake/261014-single-tier-bringlist-home.design.md
delivery/intake/INDEX.md
delivery/retro/2026-09-13-loop-retro-efficiency.html
delivery/retro/2026-09-23-loop-retro.md
delivery/retro/2026-10-14-loop-retro.md
delivery/review/INDEX.md
delivery/review/.changes-index-state.json
delivery/archive/README.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：`delivery/` 下的变更制品按「生命周期 × 面」平铺成六个家——发现的问题（诉求层）进 `docs/debt/`、在途变更路由进 `delivery/intake/`、三面 loop 评审证据进 `delivery/review/`、dev-loop 包全程进 `delivery/taskpackages/`（不动）、复盘进 `delivery/retro/`、历史整树进 `delivery/archive/`；`delivery/changes/` 伞解散，任意一个 idea 从冒头到结项的记录链（debt 暂缓 → spine 在途 → companion 设计稿 → 各面定稿）有图可查，旧布局回流有双齿拦截。

**human 意图原话**（2026-10-15 主线程会话逐字，经派发书转交，本批的方向边界）：

1. 「我们在 dev 目录下放的是发现的问题，不管是需求的、技术的还是 governance 的问题……然后 changes 下面，基本上是我们主动发现或主动提起的一些诉求。还有就是我们在 dev 下边做的时候，比如说 governance 做一些实现的时候，会把过程里的东西记录在 changes 下边，这个逻辑看起来挺不顺的。」
2. 「idea 过程中的记录结果到底是放在哪？我咋没看出来呢？」
3. 「我还是想改一下 delivery 下边的 changes，这个名字就很别扭，我甚至想把它也改了。」
4. 「我想能不能把历史的那堆东西全部放到一个 archive 文件夹里，我们就不再改它了，直接换成新的架构……这样可不可行？」

**现状差距**（证据详见 §2 各 T）：① `delivery/changes/` 伞下混住四种语义不同的东西（arch 评审证据 292 件 / intake 路由 spine 9 件 / retro 复盘 3 件 / dev 残件 4 件），名字本身不承载任何语义；② 「触发源（主动提的 vs 发现的问题）」被错当目录轴，与 debt / intake 两账的真实分工错位，且 261014 批撤销 dev lane 后仍有一个并发 Pod 分支按旧布局收口的残件漏网（T1 实证）；③ idea 从冒头到结项的记录链没有成文地图——debt 条目、spine sources/openQ、companion 设计稿、spine anchors 各存一段，但没有任何文件告诉读者「讨论过程去哪看」（human 原话②）；④ debt 与 intake 两账平行无桥——「发现的问题先记 debt，后来立项做了」没有晋升规则与结项互引；⑤ intake spine 有 frontmatter status 七字段状态机但无投影面，漂移（status 出值域、anchors 长期待回填）无人可见。

**非目标（防 scope creep）**：不改 dev-loop 流程语义与任何闸的既有判定逻辑（新增的结构检查只拦废止路径）；不动 `delivery/taskpackages/`、`delivery/milestones/`、`delivery/workstreams/` 三目录的结构与机制；不改契约面任何文件（含其内 3 处历史链接，处置归 §4 OQ-A）；不改写任何 append-only 历史面（豁免清单见 §3 与 T6 改法）；intake INDEX 漂移标本批只检测、不拦截（复议触发 = 下轮 loop-retro 仍见漂移，human 裁定②照录）；「统一 intake ≠ 统一门」不变，各面门照旧各归各裁。

**判档依据（如实记录）**：本批触 `.claude/scope-map.json`、`.claude/hooks/**`、`.claude/scripts/**`、`.claude/agents/**`、`.claude/commands/**`、`AGENTS.md`、`docs/DEV-GOVERNANCE.md`——按 `gov-change.md` 轻路径判据（路径即判据，命中任一恒 Track B，哪怕只改一个字符），**轻路径不适用**，走 Track B 主路径五段式全件（PROPOSAL + landing-summary + landing-verdict + retro 四件；波段拆分见下，retro 不豁免）。Track A 段判据（只看收敛度）：human 2026-10-15 已四裁定收敛方向（方案 B 归档切换 / T5 检测不拦截 / idea 只留裁定+依据 / intake+retro 整体前移），不命中；判官评审中升级与声明红队保留为逃生门。

**落地排序与依赖（两波落地，程序变体，§4 第 6 条随立项 ack 确认）**：

- **波段一 = 仅 T1**（残件归位 + 防复发双齿）：独立、紧急——齿二（合并闸结构检查）必须抢在在途 Pod 旧布局分支合并前进 main，否则残件类漏网会重演（T1 根因）。
- **波段二 = T2 + T3 + T4 + T5 + T6**：同波原子落地，内部顺序 = 语义基座（T2/T3）→ 规范化与整改（T4/T5）→ 容器解散（T6 殿后）。波段二的执行前置 = 在途 Pod 两分支（`dev/pod-m2`、`dev/260915-m1-k1a-key-layer-dispatch`）合并窗口过后，且在途 gov 批（`261013-gov-review-lf-eol-teeth-pin`）处置裁定留痕（T6 改法第 8 条）。波段二内 T2/T5/T6 同一批提交序列，T2/T5 文面直接按新树路径书写，无旧路径中间态进 main。
- 每波各自走「packet-bound landing → landing-verifier 独立核验 → 前置升级闸 → 合并本地 main（可回滚）」；制品增量 = 波段一 `architect-landing-summary-w1.md` + `landing-verdict-w1.md`（中间证据件），波段二 `architect-landing-summary.md` + `landing-verdict.md`（终态件，核验范围 = 波段二落地 + 全 manifest 终态）+ retro 一件（波段二后趁热写、先于波段二合并的后续 upsert，覆盖两波）。**核验范围如实声明**：结构门 `landing-fidelity-check.mjs` 的全量 manifest 对账只在波段二终态跑——波段一期间 manifest 有 75 件未落地属波段设计态（v0.3 口径：manifest 82 − 波段一 7 件），`landing-verdict-w1.md` 的核验范围 = 波段一 7 件 + §5 V1–V3 机械证据，verdict 文本逐字注明范围、不冒充全量保真（不假绿）；波段二 verdict 恢复全量口径（届时 `ANCHORABLE_TEST` 已随本批 T6 第 5 条扩根，新树锚可机辨）。INDEX upsert 状态机在 draft / frozen / landed 三转移点各跑一次不变（landed 在波段二合并后翻）。`changes-index.mjs` 的 landed 派生只认 `landing-verdict.md` 终态件，零逻辑改动。

## 2. 问题、根因与改法（T1…T6）

> 以下 file:line 行号与计数均为 2026-10-15 本 worktree（`C:/AI/KidPal-gov`，分支 gov/kidpal）起草时点实测快照，搬运者自负复证责任。grep census 命令统一形态：`grep -rl "<pattern>" --exclude-dir=.git --exclude-dir=node_modules --exclude-dir=ESP32-S3-RLCD-4.2-Demo --exclude-dir=build .`

### T1 · changes/dev 残件归位 + 防复发双齿

**问题**：261014-gov-review-dev-lane-merge-taskpackages 批已撤销 dev lane（包全程唯一家 = `delivery/taskpackages/<id>/`），但 `delivery/changes/dev/261015-voice-rightcol-label-overflow/` 仍残存四件收尾制品，与 taskpackages 同 id 目录并存——「一包一家」裁定被一枚漏网残件破坏，且没有任何机制阻止下一个旧布局分支再漏一次。
**证据**：`find delivery/changes/dev -type f`（2026-10-15 实测）= 4 件：`review-packet.md` / `supervisor-verdict.json` / `pod-retro.md` / `docs-delta.md`；`ls delivery/taskpackages/261015-voice-rightcol-label-overflow/`（同日实测）= `exit-report.md` / `input-brief-acceptance-author.md` / `input-brief-builder.md` / `mutation-evidence` / `plan.md` / `pod-events.jsonl` / `taskpackage.json` / `verifier-evidence` / `verifier-scope.json` / `verifier-scope.txt` / `verify-runs.jsonl`——与残件四件**零文件名冲突**。成因链三个 commit 经 `git log --oneline` 核实存在：`82c0a65`（chore(closeout)：真机轮 PASS 收口）、`6d199f8`（chore(supervisor)：机械闸 APPROVE + 前置升级闸 PASS）、`d827064`（Merge branch 'main' into dev/pod-m2）。`delivery/changes/dev/INDEX.md` 已不存在（dev INDEX = `delivery/taskpackages/INDEX.md`，261014 批已迁）；该包 dev INDEX 行在场、状态 `open`（`delivery/taskpackages/INDEX.md` :13，2026-10-15 实测——行状态刷新归既有 closeout/upsert 机制，非本批，见 §6 留根）。复发面实测：在途分支 `dev/pod-m2` 与 `dev/260915-m1-k1a-key-layer-dispatch`（`git branch` 同日实测在册）均为旧布局时期拉出的 Pod 分支。
**根因**：并发 Pod 分支（dev/pod-m2 线）按 261014 批前的旧布局完成收口（closeout 四件落 `delivery/changes/dev/`），merge 进 main 时无人对账新旧布局差异；261014 批的迁移只覆盖迁移时点 main 上既有的 58 包目录，后到的分支残件没有拦截机制——scope-map 只管 agent 会话写入，管不到 git merge 把旧路径树带进来。
**改法**：

1. **归位**：四件 `git mv` 进 `delivery/taskpackages/261015-voice-rightcol-label-overflow/`（零内容改动，文件名零冲突已实测）；删空树 `delivery/changes/dev/`。landing 期逐件核「源失踪 + 目标在场」留 landing-summary。
2. **齿一（scope-map 废止位）**：`universal.denyCreate` 增 `delivery/changes/dev/**` + `_doc` 注记「废止位：dev lane 已由 261014 批撤销，任何写入即违规」。机制核实（`enforce-scope.mjs` :56-59，2026-10-15 实读）：`checkUniversal` 对所有会话、所有写入操作生效，命中 denyCreate 即拦「禁止写」——**目录已删的前提下任何写入 = 新建，denyCreate 单机制即等效全覆盖**，universal 层本无独立 denyWrite 机制，任务书「denyCreate+denyWrite」以本机制承载（如实注记，不少拦）。
3. **齿二（合并闸结构检查）**：`pre-merge-escalation-gate.mjs` 增结构检查——复用既有 `git merge-tree --write-tree` 调用面（:244，同日实读在册）产出合并结果树，`git ls-tree <结果树> -- delivery/changes/dev` 非空 → BLOCK 升级人（消息逐条点名残件路径 + 指引「归位 taskpackages 同 id 目录后重跑」）。专拦旧布局分支 merge 带入（齿一拦不住的 git 通道）。self-check 增两臂：残件分支 BLOCK / 干净分支 PASS（闸为纯函数输入可注入设计，:36 注释在册，夹具注入 execFn 模拟两态）。
4. **fixture 顺修**：`brief-inputs.mjs` :758（同日实读）self-check 夹具字符串 `planRef: 'delivery/changes/dev/x/PROPOSAL.md'` 改为现行合法形态（落地时随 T6 定稿为 `delivery/review/x/PROPOSAL.md`，与该夹具断言的 proposal 来源语义一致——`architect-operating-model.md` §4 sourceType=proposal 定义句同步在 T6 改写面内）。

目标文件：`delivery/taskpackages/261015-voice-rightcol-label-overflow/review-packet.md`（新增=归位迁入）· `delivery/taskpackages/261015-voice-rightcol-label-overflow/supervisor-verdict.json`（新增=归位迁入）· `delivery/taskpackages/261015-voice-rightcol-label-overflow/pod-retro.md`（新增=归位迁入）· `delivery/taskpackages/261015-voice-rightcol-label-overflow/docs-delta.md`（新增=归位迁入）· `.claude/scope-map.json`（改）· `.claude/scripts/pre-merge-escalation-gate.mjs`（改）· `.claude/scripts/brief-inputs.mjs`（改）

### T2 · 生命周期三层语义 + 触发源判据 + idea 记录链地图（语义基座，落 repo-structure.md）

**问题**：变更制品的存放逻辑分散在五个文件的表格行里，没有任何一处成文回答三个根本问题：「一个东西算诉求、证据还是复盘？」「主动提的与发现的问题按什么分家？」「一个 idea 从冒头到结项，讨论过程记录在哪？」——human 原话②「idea 过程中的记录结果到底是放在哪？我咋没看出来呢？」是该缺口的一手证词。原话①指出的「不顺」同根：dev 目录按触发源（发现的问题）分轴、changes 按另一套含混口径收主动诉求，两套轴叠加后谁也说不通。
**证据**：`.claude/rules/repo-structure.md` 顶层地图（2026-10-15 快照）有 `delivery/changes/arch/**`、`delivery/changes/intake/**`、`delivery/changes/retro/**`、`delivery/taskpackages/`、`docs/debt/` 五行，各自只答「放什么」，无一处答「这些家之间的生命周期关系」；「触发源」散见 `architect-operating-model.md` §4 sourceType 七值（元数据形态）与 debt 三域分工（`docs/templates/debt-item.template.md` 域判据表），但无任何条文声明「触发源不作目录轴」。需求面定稿留痕的落点已核实是设计如此：`.claude/references/requirement-change.md` :47「review 留痕 = §9 该行依据列浓缩承载 architect 结论 + reviewer 结论」（同日实读）。
**根因**：结构 SSOT 是按「目录」组织的事实登记表，缺一层「生命周期」语义模型把目录串成一条线；触发源与生命周期两个维度从未被显式分离，旧 dev/changes 分家正是两维混淆的遗留。
**改法**（全部落在 `repo-structure.md`，顶层地图节后新增一节，波段二内直接按 T6 新树路径书写）：

1. **三层语义模型**：**诉求层** = `docs/debt/`（发现的问题·暂缓账，一条目一文件 + 状态机）+ `delivery/intake/`（在途变更路由账，spine 一变更一文件）；**证据层** = `delivery/review/`（需求/架构+契约/治理三面 loop 的批证据：PROPOSAL / 评审记录 / landing-summary / verdict / retro）+ `delivery/taskpackages/`（dev-loop 包全程唯一家）+ `docs/requirements.md` §9（需求面定稿与评审留痕，设计如此，锚 `requirement-change.md` :47）；**复盘层** = `delivery/retro/`（cadence retro 制品）+ `delivery/workstreams/`（跨包长周期工作记忆）。
2. **触发源判据条文**（成文，防下一轮回摆）：「触发源（主动发起 / 发现的问题）= 元数据，不作目录轴」——spine `sources`（human 原话 + refs）、TaskPackage `sourceType` + `sourceRefs`、debt 条目 frontmatter（域 + severity + trigger）各自承载触发源；**理由同条成文**：进 loop 后制品同构（主动与发现的批走同一套 Track B 七步、产同一套制品），且主动/被动边界天然模糊（一个发现的 bug 常牵出主动改进），按目录分轴必制造双头记账与搬家成本（dev lane 撤销 + 本批残件即代价实证）。
3. **idea 记录链纵向地图**（表格四行，直接回答 human 原话②）：冒头暂缓 → `docs/debt/<域>/<ID>.md` 条目正文（append-only）；在途讨论 → spine `sources`（human 原话逐字）+ `openQ`（问答通道，裁定 + 依据留痕）；成形设计 → `delivery/intake/<changeId>.design.md` companion 设计稿（需要判据见 T4）；推进状态 → spine `facePlan` / `anchors` / `status`。**比例化原则同条成文**：留裁定 + 依据，不留推演全程 transcript（human 2026-10-15 裁定③照录）。

目标文件：`.claude/rules/repo-structure.md`（改）

### T3 · debt → intake 晋升规则（落 change-intake.md + debt 模板）

**问题**：「发现的问题先记 debt，后来立项要做」这条最常见的路径没有成文桥——debt 条目与 spine / 各面入口之间无互引约定，结果是要么双头记账（debt 一件、spine 一件互不知情），要么 debt 条目结项时写不清「被哪个变更消费了」。
**证据**：`.claude/references/change-intake.md` §3（2026-10-15 快照 :27）触面探测语料已含「debt 未结账三域」，但全文件无「debt → intake 晋升」字样（同日 `grep -n "debt" .claude/references/change-intake.md` 实测仅命中 :27 语料行一行——:29 查重行不含 debt 字串，v0.2 按 R1-F4 顺修；实质结论不变）；§3 建档查重（:29）查重范围只有「既有 spine」，不含 debt 未结条目。`docs/templates/debt-item.template.md` :53 结项形态 = 「原地翻字段（status: closed + closed + closedBy）+ 正文尾追加结项行」，无「被哪个 changeId 消费」字段。机检兼容性已实读判定（任务书预留的判定项）：`.claude/scripts/debt-ledger-shared.mjs` 的必填闭集 = `REQUIRED_FIELDS = ['id','title','domain','status','severity','opened']`（:51），校验只查必填缺失与 status/severity/ID 前缀闭集（:214/:237/:239/:226），**无未知字段拦截**；渲染器不消费 `consumed-by`——故正文约定即足，**debt-render.mjs / debt-ledger-shared.mjs / debt-doctor.mjs 三脚本零改动**。
**根因**：debt 账（260923 批立）与 intake spine（260928 批立）分两批建成，接缝处只铺了「探测语料含 debt」半座桥，晋升方向（debt 条目被立项消费）从未铺。
**改法**：

1. `change-intake.md` 新增「debt → intake 晋升」节（位置随 §3 后）：发现的问题先记 `docs/debt/` 条目（登记 = 主线程，域判据见 debt 模板）；**立项要做时**——触面 ≥2 → 按 §1 建 spine，`sources.refs` 引 debt 条目 id；触面 =1 → 直接进各面既有入口（需求 `/requirement-change` / 架构+契约 `/arch-change` / 治理 `/gov-change` / 实现 `/triage-change`），各面制品的 refs/sourceRefs 引 debt 条目 id；**主动 idea 跳过 debt**，直接按触面判（§1 既有程序不变）。debt 条目随立项结项：`status` 翻 `closed` + 正文结项行带 `consumed-by: <changeId>` 约定（正文约定，理由 = 机检兼容性实测如上 + 与 append-only 散文同型）。
2. `docs/templates/debt-item.template.md` :53 结项说明行补 `consumed-by` 约定句（被变更消费而结项时，结项行写 `consumed-by: <changeId>`）。
3. `change-intake.md` §3 建档查重义务扩一句：查重范围 = 既有 spine **+ docs/debt 三域未结条目**（防「debt 一件、spine 一件」双头）；命中 debt 未结条目 → 消费既有条目（晋升路径第 1 条），不重复建档。

目标文件：`.claude/references/change-intake.md`（改）· `docs/templates/debt-item.template.md`（改）

### T4 · intake companion 设计稿规范化 + 261014 当即应用

**问题**：跨面变更的成形设计稿已有事实先例（261014 批的立项设计稿），但没有成文规范——什么时候该有、叫什么名字、要不要冻结、跟谁评审，全靠当批口头裁定；human 原话②的「记录结果放在哪」在「成形设计」这一环同样无答案。
**证据**：`delivery/changes/intake/261014-single-tier-bringlist-home.PROPOSAL.md` 在场（2026-10-15 实读，头 5 行 verbatim：「角色：spine（…）的立项对齐 companion——承载『为什么改 / 往哪改 / 改动多大』与 T1–T4 设计；…本稿不设 FROZEN-landing（无独立评审批，2026-10-14 human 裁，见 OQ-1）」）；`change-intake.md` 全文无 companion 设计稿节（同日 grep 实证零命中）。该 spine（`261014-single-tier-bringlist-home.md`）还存两处 261015 批前旧口径：`faceMap.contract` 行（:24）「走 architect-reviewer 四分类 + 人类契约门（随批 packet）」与 `facePlan` 契约行（:31）「architect-reviewer 四分类 → ContractChangePacket → human 契约门」——人签门已由 261015-gov-review-face-loop-unify 批撤销，现行 = 契约变更走 `/arch-change`、契约包三节随批原子落、无人签门、合并过 `pre-merge-escalation-gate.mjs` 机械凭据闸。
**根因**：companion 先例生于 261014 批的路由裁定（openQ 记录：降为立项稿、不建独立评审批），裁定留在了 spine openQ 里，没有回灌进程序 SSOT。
**改法**：

1. `change-intake.md` 新增「companion 设计稿」节：**需要判据** = 跨面 / 需求档 2 / 含 human 设计取舍点——即立项 ack 需要实质对齐物时；**命名** = `<changeId>.design.md`，与 spine 同目录；**免冻、不设独立评审批**（设计对抗审归下游面 loop 既有环节——261014 先例：由 requirement-change 档 2 的 architect 必派承担）；spine 的「往哪改」行引用之；比例化原则同 T2 第 3 条（留裁定 + 依据）。
2. **当即应用 261014**：`261014-single-tier-bringlist-home.PROPOSAL.md` 改名 `261014-single-tier-bringlist-home.design.md`（波段二随平移段落终态 `delivery/intake/`）；spine `261014-single-tier-bringlist-home.md` 引用同步（openQ 既有记录行与「立项对齐」节的 companion 文件名）。
3. **同块刷旧口径**：该 spine 的 `faceMap.contract` 行与 `facePlan` 契约行刷新为现行契约口径（上文证据段末句）。权限路径注记：facePlan 改「谁裁」属 `change-intake.md` §7 的 L2 呈人级修订——human 已在本批任务书裁定此项（2026-10-15），L2 已满足；landing 由主线程执行 + spine `openQ` 追加留痕。

目标文件：`.claude/references/change-intake.md`（改）· `delivery/intake/261014-single-tier-bringlist-home.design.md`（新增=平移+改名）· `delivery/intake/261014-single-tier-bringlist-home.md`（改+平移）

### T5 · intake lane INDEX 投影 + status 收敛 + 收口回写清单齿

**问题**：intake spine 有 frontmatter 状态机（七字段之 `status`），但无任何投影面——九份 spine 的真实状态要逐份打开才知道；现测即有两例漂移在册，且各面收尾点（integrator closeout / arch·gov landing 记账 / requirement §9 记账）只有零散一句「回填 anchors」，status 回写义务完全缺席，漂移只会继续累积。
**证据**：① `changes-index.mjs` 现有扫描根只有 arch lane（:245/:260/:284/:309/:426，2026-10-15 实读），无 intake lane；状态缓存常量 `STATE_CACHE_PATH = delivery/changes/.changes-index-state.json`（:32）。② 漂移实例一：`delivery/changes/intake/261010-p3-storylist-cold-start.md` :8 `status: landed`——值域外（枚举 = `change-intake.md` §2 第 7 字段：`intake → in-flight → awaiting-human:<gate> / awaiting-upstream / awaiting-human:<openQ> → closed`，:23）；其 anchors 三面已回填、契约门 HUMAN_APPROVED 2026-10-12（spine :6 实测）。③ 漂移实例二：`261001-m4-k3-record-session-live-repair.md` anchors 三面「待回填」（:27-29 实测）+ `status: in-flight`（:68）；其 Package A dev INDEX 行已 `closed`（`delivery/taskpackages/INDEX.md` :25 实测）——imp 面事实已推进、spine 未跟。④ 既有回写句实测：`.claude/commands/gov-change.md` :19、`.claude/commands/arch-change.md` :20、`.claude/commands/requirement-change.md` :22 均只有「回填 anchors」半句，无 status；dev 面收尾（integrator closeout / build-feature）全句缺席。⑤ INDEX 计数实测：`find . -name INDEX.md`（同日）= **9 份**（arch 1 + taskpackages 1 + workstreams 1 + debt 三域 3 + debt 归档年 3），repo-structure 消歧表「现共 8 份」已是漂移（加 intake INDEX 后应为 10 份，计数句随批顺修）。
**根因**：spine 状态机（260928 批立）只定义了字段、没建投影与回写义务；「写者 = 主线程」（`change-intake.md` :15）的单写者设计正确，但主线程在四个收尾点没有清单齿提醒，全靠记性好。
**改法**（human 裁定②照录：清单齿 + INDEX 漂移检测标记，检测不拦截）：

1. **intake lane INDEX**：`changes-index.mjs` 扩 intake lane——扫 `delivery/intake/*.md` frontmatter 七字段（波段二同波落地，扫描根直接取 T6 新址，无旧根中间态），生成 `delivery/intake/INDEX.md`（生成件勿手改，头注同 arch INDEX 既有形态）。列 = changeId / 触面（faceMap 命中面缩写）/ status / anchors 回填进度（已回填面数 ÷ 命中面数）/ 日期 / 漂移标。同批顺带（T6 合流）：arch lane 扫描根改 `delivery/review/`、lane 键名 `arch`→`review`（代码与文面同步）、`STATE_CACHE_PATH` 迁 `delivery/review/.changes-index-state.json`。
2. **漂移检测标记（检测不拦截）**：两条规则——① anchors 命中面全回填但 status ≠ `closed`；② `awaiting-*` 状态滞留超 **7 天**（酌定理由：对偶 loop-retro 日历兜底的周量级观察窗；短于 7 天会把正常等 human 的周末档打成噪音——首个真实误报/漏报实例出现时重估，留 §6）。命中 → INDEX 行漂移列打标（⚠未闭环 / ⚠滞留Nd 形态），不拦任何流程。**复议触发**（human 裁定②照录）：下一轮 loop-retro 仍见漂移标 → 届时另立批升级机械闸。
3. **status 值域收敛**：INDEX 生成器按 `change-intake.md` §2.7 枚举解析；值域外值原样显示 + 打漂移标（不静默归一）。**现存漂移整改两件**（landing 期主线程执行）：261010 `status: landed` → `closed`（anchors 三面已回填、实现面同批已收口，openQ 追加整改依据留痕）；261001 全面实审回填——imp anchor 更新为 Package A 收口实态（dev INDEX closed 实证如上）+ req/contract/arch 三面如实填「Track A 待跑」实况 + status 实判 + openQ 留痕。
4. **收口回写清单齿**（五处各加一行，语义 = 「回写 spine anchors + status；写者恒 = 主线程」）：`.claude/agents/integrator.md` closeout 步骤（退出报告须含 imp anchor 事实与建议 status）；`.claude/commands/build-feature.md` 收尾步；`.claude/commands/arch-change.md` :20 与 `.claude/commands/gov-change.md` :19 既有「回填 anchors」句扩 status；`.claude/references/requirement-change.md` 档 1 第 4 步（:37）与档 2 第 6 步（:47）的 §9 记账步。另：`.claude/commands/loop-retro.md` 分析清单加一行「核 `delivery/intake/INDEX.md` 漂移标」——复议触发（第 2 条）的承载点。
5. **消歧表登记**：`repo-structure.md`「三种同名物消歧」表 intake INDEX 入列（索引什么 = 在途变更路由状态；划分轴 = lane；谁写 = changes-index.mjs 生成）+ 计数句按实测顺修（证据⑤）。

目标文件：`.claude/scripts/changes-index.mjs`（改）· `.claude/agents/integrator.md`（改）· `.claude/commands/build-feature.md`（改）· `.claude/commands/arch-change.md`（改）· `.claude/commands/gov-change.md`（改）· `.claude/commands/requirement-change.md`（改）· `.claude/commands/loop-retro.md`（改）· `.claude/references/requirement-change.md`（改）· `.claude/rules/repo-structure.md`（改）· `delivery/intake/261010-p3-storylist-cold-start.md`（改+平移）· `delivery/intake/261001-m4-k3-record-session-live-repair.md`（改+平移）· `delivery/intake/INDEX.md`（新增=生成件）

### T6 · changes/ 容器解散（切换式迁移，殿后；任务书代号 T6″）

**问题**：`delivery/changes/` 这个名字本身不承载语义（human 原话③「这个名字就很别扭」），伞下四种语义不同的制品混住（arch 评审证据 292 件 / intake 9 件 / retro 3 件 / dev 残件 4 件，同日 `find` 实测，整树 309 件）；而历史批次的引用面遍布全仓，手工逐件改写历史既不可行也违反 append-only 纪律。human 原话④给出的方向 = 历史整树归档冻结、新架构切换。
**证据**：引用面 census（2026-10-15 实测，命令形态见节首注）：`changes/arch` 288 文件、`changes/intake` 63、`changes/retro` 23、`changes/dev` 442（含 delivery/ 自引用与全部历史面；派发书快照 284/62/23 与本实测 288/63/23 的差 = 本批新产生的引用与统计口径差异，以本实测为准）。活面（delivery/ 之外）构成已逐件分类（§3 表）：机械消费面 = scripts 11（arch-review-handoff-check / brief-inputs / changes-index / devloop-artifact-check / landing-fidelity-check / lib/governance-gate-shared / portfolio-reconcile / pre-merge-escalation-gate / proposal-consolidation-check / settings-lint / taskpackage-required-check）+ hooks 4（commit-safety / enforce-scope / suggest-codebase-memory / supervisor-verdict-gate）+ scope-map 写槽 8 处（:165-166、:195-196、:209-210、:220-221、:231-232、:242-243、:257-258、:293-294 实测）+ route-fixtures 1；流程文面 = agents 8（review-loop 六件 + landing-verifier + planner）+ commands 1（build-feature :16 spine 路径）+ references 13 + rules 1 + skills 5（v0.2 口径对齐注记：原 census 按四子路径 pattern 计，R1 修订后按 §3 七类复扫模式全量计——scripts +2（devloop-artifact-check / taskpackage-required-check，交替形消息串）、hooks +1（supervisor-verdict-gate，裸伞维护注）、agents +1（planner，交替形 prose）、references +1（plain-file-handoff，裸伞存址指引；原 T6 记 12 = 四 pattern 口径、§3 原记 13 = manifest 口径含无路径命中的 requirement-change 清单齿件、宽 pattern 实测 13 含 plain-file-handoff，三方已对齐）、skills +2（brainstorming / merge-to-main，裸伞）；integrator（agents 面）与 references/requirement-change.md 无 `changes/` 路径命中、纯 T5 清单齿内容改入 manifest，不计入本路径 census；commands 面同型——manifest 5 件 vs 本路径 census 1 件（build-feature），差 4 件 = arch-change / gov-change / loop-retro / requirement-change（commands 面四件 `changes/` 命中 2026-10-16 实测均 0，纯 T5 清单齿入 manifest，v0.3 R2-NIT-2 补记））；G1 = AGENTS.md（治理路由节 INDEX 路径 + 跨面 intake 行 spine 路径）+ docs/DEV-GOVERNANCE.md（§1 铁律 / 思想④ / §4 治理变更 step 1·step 3 / §4 对抗 review 证据行 / §6 G4 表行 / §7 速查行）；模板 5 + notes 1 + showme README 1（活索引表 4 行指向 changes/ 内，:27-30 实测）+ requirements.md §9 头注指针行（:690）+ architecture.md §5.5.1 仓结构行（:278，裸伞形 spec 面活指针——四子路径 pattern 对该文件零命中，v0.3 R2-F1 增列）。机械闸对旧路径的硬编码实证：`pre-merge-escalation-gate.mjs` :89-90（PROPOSAL / landing-verdict 路径拼接）与 :132（契约凭据 PROPOSAL 正则 `delivery\/changes\/arch\/\d{6}-(?:gov|arch)-review-`）；`changes-index.mjs` :32/:245/:260/:309（扫描根与缓存）；`enforce-scope.mjs` :66（docsTopLevel 拦截提示句引旧路径）。**锚识别白名单实证（起草期 preflight 发现）**：`governance-gate-shared.mjs` :40 `ANCHORABLE_TEST` 的 delivery 根枚举只有 `workstreams|milestones|taskpackages|changes` 四根——本稿初跑 `--preflight` 实测 anchorUnion 58 件 vs manifest 74 件（v0.1 起草时点快照；v0.2 R1 增列 7 件后 manifest = 81 件、v0.3 R2 增列 1 件后 manifest = 82 件；**v0.4 R3 修订时点真机复测**（node 直调 `parseSection2AnchorLists` + `extractManifestBlock` + `compareAnchorSets`）anchorUnion = 59 / manifest = 82 / 缺口 onlyInManifest = 23 = 16 件新树路径锚 + 7 件 §2 未枚举件，**两条归零路径并述**——16 件锚性缺口由本批 T6 第 5 条 ANCHORABLE_TEST 扩根闭合、7 件枚举缺口由本批 R3-F1 补列 §2 T6 目标文件行闭合，两路归零后 §2 并集 66 + 扩根回归 16 = 82 ≡ manifest，V6 全等预期不变；landing 期复跑以当日实测为准），被静默丢弃的 16 件恰为全部新树路径锚（intake / retro / review / archive）；同文件 :110 `isProposalMdPath`（:106-113）内的 PROPOSAL 形态正则 `delivery\/changes\/(arch|dev)\/` 同型（v0.4 R3-NIT：函数名勘误，原误植 `resolveOriginRef`，行号不动）。先例支持（整树归档 + 历史零改写）：`docs/archive/spec.md`（AGENTS.md 系统 spec 面行在册）、`delivery/workstreams/_archive/<年>/`、`docs/debt/<域>/_archive/<年>/`（repo-structure.md debt 行「位置 = 状态的纯函数」）。**契约面实测**（任务书同法）：`grep -rn "changes/" docs/contracts/` = **3 处命中**（`interface-contract.md` :5、:90、:111，均为历史契约变更记录行指向 arch 旧批目录）——非零，处置归 §4 OQ-A。
**根因**：`delivery/changes/` 是治理证据面的第一代替物，在 dev lane 撤销（261014）与 intake/retro 加入（260928/loop-retro）之后，伞名与内容物的错位已经不可修复；逐件改写历史引用违反 append-only 纪律（261014 批 T4 豁免先例：「路径是当时事实，批改即篡改记录」），唯一自洽的出路 = 整树切换。
**改法**（human 裁定①照录：方案 B 归档切换；④照录：intake/retro 量小整体前移）：

1. **新树**：`delivery/intake/`、`delivery/review/`、`delivery/retro/` 三目平移至 `delivery/` 根（milestones / workstreams / taskpackages 不动）。intake 全 9 件（8 spine + 1 companion 改名件）与 retro 全 3 件整体前移（逐件入 manifest 锚）；arch lane 只前移「落地窗口实态判定为活跃在途」的批——判据 = 落地窗口先跑 `changes-index.mjs` 全量重建刷新 INDEX，**仍要向 main 交付制品的批**整批前移 `delivery/review/`；起草时点实测候选 = 本批自身一例（波段二期间 landed 未翻，波段二制品写新家）；派发书候选名单的实态复核（2026-10-15 INDEX 实测）：261015-gov-review-face-loop-unify 已 `landed/yes`、261015-gov-review-lite-fast-path 已 `landed/yes`（两批随归档）；261013-gov-review-lf-eol-teeth-pin `frozen/—` 与 260926-arch-review-char-window-key-triggered-play `draft/—` 的制品均不在 main（无可前移物，处置见第 8 条）；`frozen/yes` 停滞异常行 5 行（260922-arch-review-spec-v5-ui-align / 260923-arch-review-graph-index-name-errata / 260928-gov-review-requirement-face / 260928-gov-review-requirement-loop / 260929-gov-review-agent-def-alignment）随归档冻结、落地窗口逐行核注留 landing-summary（任务书预裁「落地时再核」照录；日后重启 = 新立 changeId 引旧批）。
2. **归档**：`delivery/changes/` 整树 `git mv` → `delivery/archive/changes/`（历史文件零改写，合 append-only 纪律；整树移动保树内相对链接拓扑）；新增 `delivery/archive/README.md` 路标——旧→新映射四行（arch → review、intake → intake、retro → retro、dev → taskpackages〔261014 已迁〕）+ 「归档内不再改写」声明 + 断链自救指引（按同名文件到新树/归档内找）。
3. **活引用改写（两分类判据）**：**改写类** = 活 canonical 与活索引（不受 append-only 纪律约束的文件）：`.claude/**` 全部命中件（agents/commands/references/rules/skills/hooks/scripts/scope-map/route-fixtures）、G1 两文件、docs/templates 5 件、docs/notes/ui-implementation-strategy.md :3 来源行（指向 260830 批，改写为归档内路径）、docs/showme/README.md :27-30 活索引 4 行（3 行改指归档、1 行改指 delivery/retro/）、docs/requirements.md §9 头注指针行（走档 0 勘误通道 + §9 追加勘误行）、docs/architecture.md :278 仓结构行（同型 spec 面活指针——`delivery/changes/`（治理/开发证据），走档 0 勘误通道 + §12 变更记录追加勘误行；v0.3 R2-F1 增列）。**豁免类**（零改写，逐类枚举 = §5 V4 的豁免清单基线）：归档整树内部；delivery/taskpackages/** 包内历史件；spine/retro 平移件内部的历史叙述行与 anchors 回填文本（T4/T5 指定的编辑除外）；delivery/workstreams/** 与 delivery/milestones/**；docs/ADR.md、docs/ARCH-REVIEW-QUEUE.md、docs/CONTRACT-DECISION-QUEUE.md、docs/debt/**、docs/archive/spec.md、docs/showme/*.html 快照；docs/requirements.md §9 既有版本行；docs/contracts/interface-contract.md（OQ-A）；firmware acceptance 退役面（archive/** 与 red-baseline/**——red-baseline 面实测命中 `screen-unification-red-baseline-summary.md` :118/:167 两处历史链接，v0.3 R2-NIT-1 扩列）与 server acceptance/archive/** 退役面（server 侧实测无 red-baseline 目录）；server/acceptance/test_m4k1_story_cloud_link.acceptance.py :1844 注释（冻结验收件，验收冻结纪律不改；provenance 注释按同名 spine 文件可解）；server/evidence/**；.claude/loop-retro/ledger.jsonl；.codebase-memory/**。**第三类「设计保留位」与本豁免类分列、不混入本清单**——post-landing 活树按本批自身设计仍保留旧路径字面量的位置（齿、闸检查路径与夹具、锚识别保留分支、归档路标与索引指针、改指归档的活文面、本批证据件引文），逐件枚举与判据见 §5 V4；豁免类 = 零改写历史面，设计保留位 = 活文件中有意的旧路径书写，两者均非残留漏改（v0.3 R2-F2 增列）。
4. **INDEX 切换**：旧 `delivery/changes/arch/INDEX.md` 随归档冻结；`changes-index.mjs` 扫描根改 `delivery/review/` + `delivery/intake/`（与 T5 合流，同波无中间态）；新 `delivery/review/INDEX.md` 全量重建 + 头部指针行指向归档内旧 INDEX（「2026-10-15 前历史见 delivery/archive/changes/arch/INDEX.md」）；消歧表随 T5 第 5 条更新；lane 键名 `arch`→`review`（代码、scope-map 写槽、AGENTS.md lane 措辞、`changes-index.mjs` :374 命名检查提示句同步）。
5. **锚识别白名单前置（起草期 preflight 实证发现，漏扩即静默丢锚）**：`governance-gate-shared.mjs` 的 `ANCHORABLE_TEST`（:40）delivery 根枚举扩 `intake|retro|review|archive` 四根（`changes` 根保留——归档内历史 PROPOSAL 仍可被 preflight 解析）；同文件 `isProposalMdPath`（:106-113）内的 PROPOSAL 形态正则（:110-111）增 `delivery/review/` 分支（旧 `delivery/changes/(arch|dev)/` 分支保留，归档引用可解析）。`CANONICAL_TEST`（:34）按 :39 注释的既有分工**不动**——新四根与 `delivery/changes/` 同为 G4 证据面，本就不在 scope-creep 检测面（landing-fidelity-check :723 消费点核对一致）。本项是波段二内**时序前置**：manifest 锚识别的机械门消费（landing-fidelity / 判官 preflight 复跑）依赖它，落地序列中脚本批改先于 landing-fidelity 终验；§5 V6 的 preflight 复跑预期随之升为 anchorUnion ≡ manifest 全等（现态口径见 T6 证据段 preflight 注记：v0.4 实测缺口 23 = 16 件新树锚 + 7 件 §2 未枚举件——本条扩根闭合 16 件、R3-F1 §2 补列闭合 7 件，两路归零）。
6. **双齿扩展**：齿一 scope-map `universal.denyCreate` 的 `delivery/changes/dev/**` 条目波段二扩为 `delivery/changes/**`（注记更新：伞已解散，真家见 `delivery/archive/README.md` 映射）；齿二 `pre-merge-escalation-gate.mjs` 结构检查同型扩为「合并结果树 `git ls-tree <结果树> -- delivery/changes` 非空 → BLOCK」（精确路径检查，`delivery/archive/changes/` 是不同路径、不误伤——专拦旧布局分支 merge 把旧伞带回；拦截消息指引归档映射）。闸内契约凭据正则（:132）与 PROPOSAL/verdict 路径拼接（:89-90）同步改 `delivery/review/`。
7. **scope-map 写槽迁移**：8 处 `delivery/changes/arch/*arch-review-*/**`、`*gov-review-*/**` 写槽 → `delivery/review/` 同型（目录命名 token 规约不变：`<YYMMDD>-gov-review-<slug>` / `<YYMMDD>-arch-review-<slug>`，仅父路径 arch→review）。
8. **时机与在途处置（殿后条款）**：波段二执行前置 = ① 在途 Pod 两分支（`dev/pod-m2`、`dev/260915-m1-k1a-key-layer-dispatch`）合并窗口已过（齿二已拦截或归位完成）；② 在途 gov 批 `261013-gov-review-lf-eol-teeth-pin`（其 spine 自述已 landing GREEN、分支未合 main）与 arch 批 `260926-arch-review-char-window-key-triggered-play`（draft）处置经主线程裁定留痕——先合 main 再随归档（推荐：状态机完整闭环）或窗口后按新布局重落；未按期处置的旧布局分支未来 merge 时撞齿二 BLOCK → 升级 human（设计态，非死锁）。前置确认命令与留痕位置进 §5 V8。
9. **自改递归与第五阀注记（如实申报）**：本批改写面含 `.claude/agents/architect.md`（architect 自身定义的路径引用）与 review-loop agent 六件 + landing-verifier——均为路径引用机械替换、零职责/权限语义变化；自改递归批准 = 本批 human 立项 ack + 判官四查（意图保真核查覆盖「路径替换不夹带语义变化」）；第五阀场景（被改 def 生效后的 re-review）不涉语义、无需特别处置，如实留此注记。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改）· `.claude/scripts/lib/governance-gate-shared.mjs`（改）· `.claude/scripts/portfolio-reconcile.mjs`（改）· `.claude/scripts/proposal-consolidation-check.mjs`（改）· `.claude/scripts/settings-lint.mjs`（改）· `.claude/scripts/arch-review-handoff-check.mjs`（改）· `.claude/scripts/devloop-artifact-check.mjs`（改——错误消息串交替形 stale 刷新，行为经共享 isProposalMdPath 自动更新；R1 增列、R3-F1 补入本行）· `.claude/scripts/taskpackage-required-check.mjs`（改——同型错误消息串交替形 stale 刷新；R3-F1 补入本行）· `.claude/hooks/commit-safety.mjs`（改）· `.claude/hooks/enforce-scope.mjs`（改）· `.claude/hooks/suggest-codebase-memory.mjs`（改）· `.claude/hooks/supervisor-verdict-gate.mjs`（改——维护注裸伞 closeout 根刷新；R3-F1 补入本行）· `.claude/route-fixtures/surface-alignment.json`（改）· `.claude/agents/adjudicator.md`（改）· `.claude/agents/architect.md`（改）· `.claude/agents/challenger.md`（改）· `.claude/agents/defender.md`（改）· `.claude/agents/landing-verifier.md`（改）· `.claude/agents/review-moderator.md`（改）· `.claude/agents/scenario-author.md`（改）· `.claude/agents/planner.md`（改——交替形 prose 旧树双路解析描述刷新；R3-F1 补入本行）· `.claude/references/arch-review-protocol.md`（改）· `.claude/references/arch-scenarios.md`（改）· `.claude/references/architect-operating-model.md`（改）· `.claude/references/change-triage.md`（改）· `.claude/references/dev-loop-operating-system.md`（改）· `.claude/references/dev-loop.md`（改）· `.claude/references/path-form-family.md`（改）· `.claude/references/pod-coordination.md`（改）· `.claude/references/task-intake-and-slicing.md`（改）· `.claude/references/taskpackage-authoring.md`（改）· `.claude/references/track-b-verifier-brief.md`（改）· `.claude/references/plain-file-handoff.md`（改——裸伞存址指引刷新；R3-F1 补入本行）· `.claude/skills/gov-arq/SKILL.md`（改）· `.claude/skills/kidpal-idf-env/SKILL.md`（改）· `.claude/skills/opendesign-connect/SKILL.md`（改）· `.claude/skills/brainstorming/SKILL.md`（改——裸伞活指令「成形设计落 delivery/changes/」刷新；R3-F1 补入本行）· `.claude/skills/merge-to-main/SKILL.md`（改——裸伞 INDEX lane 拆分描述顺修；R3-F1 补入本行）· `AGENTS.md`（改）· `docs/DEV-GOVERNANCE.md`（改）· `docs/requirements.md`（改——档 0 勘误通道）· `docs/architecture.md`（改——档 0 勘误通道）· `docs/notes/ui-implementation-strategy.md`（改）· `docs/showme/README.md`（改）· `docs/templates/PROPOSAL.template.md`（改）· `docs/templates/architect-landing-summary.template.md`（改）· `docs/templates/retro.template.md`（改）· `docs/templates/GOVERNANCE-MIGRATION-GUIDE.md`（改）· `delivery/intake/260928-char-window-key-triggered-play.md`（新增=平移）· `delivery/intake/260929-char-window-v5-trim.md`（新增=平移）· `delivery/intake/261011-p1-home-v5-rework.md`（新增=平移）· `delivery/intake/261012-kidpal-realtime-facts.md`（新增=平移）· `delivery/intake/261013-gov-review-lf-eol-teeth-pin.md`（新增=平移）· `delivery/retro/2026-09-13-loop-retro-efficiency.html`（新增=平移）· `delivery/retro/2026-09-23-loop-retro.md`（新增=平移）· `delivery/retro/2026-10-14-loop-retro.md`（新增=平移）· `delivery/review/INDEX.md`（新增=生成件）· `delivery/review/.changes-index-state.json`（新增=再生件）· `delivery/archive/README.md`（新增）——archive 整树与在途批前移为目录级搬移（成员随落地窗口实态），不入锚，对账归 §5 V7/V8

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

> census 命令（2026-10-15 本 worktree 实测）：`grep -rl "<pattern>" --exclude-dir=.git --exclude-dir=node_modules --exclude-dir=ESP32-S3-RLCD-4.2-Demo --exclude-dir=build . | wc -l` → `changes/arch` 288 · `changes/intake` 63 · `changes/retro` 23 · `changes/dev` 442（442 中 350 在 delivery/taskpackages 包内历史件）。**自引用漂移注记（v0.2 · R1-F3）**：四 pattern 计数含本 PROPOSAL 自身（`grep -rl` 计未跟踪文件），批内每增一处 pattern 字面引用计数 +1 属正常——R1 复测已见 289/64/24/443（各 +1）；绝对计数不承重，§5 V4 落地期按当日实测跑、以「命中集 == 豁免清单 ∪ 本批自身引用」并集判据对账。活面逐件分类如下；命中文件均经 Glob/Grep 机械枚举生成，判官 spot-check 即可。

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| scope-map / hooks / scripts / route-fixtures | `.claude/scope-map.json`、`hooks/` 4 件、`scripts/` 11 件（含 lib 1）、`route-fixtures/surface-alignment.json` | G3 | 机械消费面：写槽 8 处、闸路径常量与正则、扫描根、fixture 字符串、拦截提示句；v0.2 R1 增列：`devloop-artifact-check.mjs` :137 与 `taskpackage-required-check.mjs` :492 错误消息串（交替形 `expect delivery/changes/(arch|dev)/<id>/PROPOSAL.md`——行为经共享 `isProposalMdPath`〔governance-gate-shared.mjs :106，已在改写面〕自动更新，仅消息 stale）、`supervisor-verdict-gate.mjs` :207 维护注（裸伞 `delivery/changes/**` closeout 根） |
| agent 定义 | `.claude/agents/` 9 件（review-loop 6 + landing-verifier + integrator + planner） | G2 | 路径引用替换 + T5 清单齿 1 行（integrator）；v0.2 R1 增列 planner.md :39（交替形 prose 引旧树双路解析，行为同由共享 `isProposalMdPath` 承载）；architect.md 自改递归注记见 T6 第 9 条 |
| commands | `.claude/commands/` 5 件（build-feature / arch-change / gov-change / requirement-change / loop-retro） | G2 | T5 清单齿 + T6 路径（build-feature :16） |
| references / rules / skills | references 14 件 + rules/repo-structure.md + skills 5 件 | G2 | repo-structure 为 T2 语义基座主落点；change-intake 为 T3/T4 主落点；v0.2 R1 增列：`plain-file-handoff.md` :9（裸伞「勿存 `delivery/changes/`」指引）、`brainstorming/SKILL.md` :59/:62（裸伞活指令「成形设计落 `delivery/changes/`」——伞解散后直指废止树，齿一虽拦、指令本身错须改）、`merge-to-main/SKILL.md` :18（裸伞 INDEX lane 拆分描述，arch INDEX 随 T6 迁 `delivery/review/` 顺修） |
| G1 | `AGENTS.md`、`docs/DEV-GOVERNANCE.md` | G1 | 路径与 lane 措辞机械刷新，零语义变更 |
| spec 面 | `docs/requirements.md`、`docs/architecture.md` | spec（档 0 勘误） | requirements §9 头注指针行 1 行 + 勘误行 append、§9 既有行豁免；architecture :278 仓结构行 1 行 + §12 变更记录勘误行 append（v0.3 R2-F1 增列，与 requirements 同型对偶） |
| 文档面 | `docs/templates/` 5 件、`docs/notes/ui-implementation-strategy.md`、`docs/showme/README.md` | 文档 | 模板用法行 / 来源行 / 活索引 4 行 |
| 证据面（迁移） | `delivery/taskpackages/261015-voice-rightcol-label-overflow/` 4 件、`delivery/intake/` 9 件、`delivery/retro/` 3 件、`delivery/archive/`（整树 + README） | G4 证据 | 平移/归位/归档；3 件 spine 内容改（T4/T5 指定） |
| 再生件 | `delivery/intake/INDEX.md`、`delivery/review/INDEX.md`、`delivery/review/.changes-index-state.json` | 再生 | changes-index.mjs 生成，勿手改 |
| 历史豁免（零改写） | 归档整树、taskpackages 包内历史件、workstreams/milestones、ADR/ARQ/CDQ/debt/**、archive/spec.md、showme HTML、requirements §9 既有行、firmware acceptance 退役面（archive/** + red-baseline/**）与 server acceptance/archive/**、server/evidence、loop-retro/ledger.jsonl、冻结验收件注释 1 处 | 豁免 | 判据与枚举 = T6 改法第 3 条豁免类 |
| 契约面 | `docs/contracts/interface-contract.md`（3 处历史链接） | 未触 | 本批零写；断链处置归 §4 OQ-A |

**【硬项 · 复扫模式完备性先验】**（涉退役语面残留清扫，模式集显式枚举；已查 `.claude/references/path-form-family.md` 形态族谱表）：

1. 斜杠形：`changes/arch`、`changes/intake`、`changes/retro`、`changes/dev`（裸写 / 反引号 / 引号包裹同一 grep 覆盖）；
2. 分段字面量形：`['"]changes['"]\s*,\s*['"](arch|intake|retro|dev)['"]`（resolve/join 多段调用，scripts/hooks 机械面子检）；
3. 转义形：`changes\/`（正则字面量内转义形态，如闸 :132——独立子检，对偶 261014 批 R1-F6 教训：普通 pattern 不匹配转义形）；
4. 裸 token 形：`"changes"` 作为独立路径段出现在数组/枚举字面量中（如 `['delivery', 'changes', 'arch']`——分段数组形子检）；
5. 目录名 token 形：`(arch|gov)-review-`（命名规约 token 不变，但拼接父路径处须逐一核——scope-map 写槽与闸正则已枚举）；
6. 交替形：`changes/(arch|dev)`（正则 / 消息串 / prose 字面量内的括号交替形态——四子路径 pattern 结构上不可能命中此形字面量，v0.2 R1-F1 增列；实测新增命中 3 件：devloop-artifact-check.mjs :137、taskpackage-required-check.mjs :492、planner.md :39，已在改写面文件内的同形命中随件覆盖）；
7. 裸伞形：`delivery/changes/` 后不接四子路径（无子路径的落址指引 / 通配 / 存址提醒，子检形态 = `delivery/changes/` 全量命中减去四子路径命中，v0.2 R1-F1 增列；实测新增命中 5 件：supervisor-verdict-gate.mjs :207、brainstorming/SKILL.md :59/:62、merge-to-main/SKILL.md :18、plain-file-handoff.md :9（四件 .claude 面，已在改写面文件内的同形命中随件覆盖）+ docs/architecture.md :278（spec 面活指针，v0.3 R2-F1 补列——R1 复扫漏件：该文件四子路径 pattern 零命中、裸伞形宽扫孤出，已入 §3 spec 面行 + manifest + T6 第 3 条改写类））。

完备性论证：path-form-family.md 族谱表列一仓路径的全部文本写法族（裸斜杠 / 包裹变体 / 分段 join / 转义 / 数组段），本批四族目标词 × 前五类形态一一配子检，另增交替形与裸伞形两类独立子检（v0.2 R1 增列，覆盖四子路径 pattern 的结构性盲区——字面量内无子路径字符可锚的两类形态）；豁免件按 T6 第 3 条清单逐类排除——清扫模式与验收判据同源（261014 批 T5 范式照用），§5 V4 子检与落地期清点用同一条模式。

## 4. human 取舍点（OQ）

**立项 ack 待确认（已裁四点照录 + 改判记录 + 程序变体一项）**（2026-10-15 human 立项 ack：第 1–4 点照录确认、第 6 点两波程序变体确认、滞留阈值 7 天确认、OQ-A 裁 (b)——裁定行见下）：

1. **方案 B「归档切换」**（照录）：解散 `delivery/changes/` 伞，`delivery/` 平铺六目录（milestones / workstreams / taskpackages 不动 + intake / review / retro 新树）；历史整树归档不逐件改写。
2. **T5 status 回写形态**（照录）：清单齿 + INDEX 漂移检测标记（**检测不拦截**）；复议触发 = 下一轮 loop-retro 仍见漂移则升级机械闸。随附酌定项：滞留阈值 7 天（理由见 T5 改法第 2 条），随 ack 一并确认。
3. **idea 讨论过程只留「裁定 + 依据」**（照录），不留推演全程 transcript——落 T2 第 3 条比例化原则。
4. **intake / retro 量小整体前移**（照录，不全归档）。
5. **改判记录（义务留痕）**：261014-gov-review-dev-lane-merge-taskpackages 批 human 曾裁「`changes/` 名称保持不变，本次不改名」（该批 PROPOSAL :4 实测）；本批 human 2026-10-15 改判推翻（原话③④）。改判先例：v1.61 翻转 v1.57 OQ2（明天区口径）。
6. **两波落地程序变体（本 PROPOSAL 新增设计，需确认）**：Track B 的 landing 拆两波执行（波段一 = T1 牙齿抢先；波段二 = T2–T6 在 Pod 合并窗口后原子落地），制品增量 = 波段一 summary/verdict 带 `-w1` 后缀的中间件两份 + 波段二终态件两份 + retro 一份（§0「落地排序与依赖」节）；结构门全量 manifest 对账只在波段二终态跑，波段一 verdict 为范围限定核验（如实注明、不假绿）。**推荐确认**——T1 齿二必须抢在在途 Pod 旧布局分支合并前进 main，单波落地会迫使全批等窗口、牙齿失效。

**OQ-A · 契约内 3 处历史链接断链处置**（起草期实测发现，`grep -rn "changes/" docs/contracts/` = 3 命中：`interface-contract.md` :5 / :90 / :111，均为历史契约变更记录行指向 arch 旧批目录；归档迁移后相对链接失效）：

- (a) **不改契约**【推荐】：三处链接成为指向归档前路径的断链，`delivery/archive/README.md` 的旧→新映射兜底（按批目录名可解）；断链事实登记留根（§6）。理由：三行属历史变更记录（append-only 纪律面），且本批红线 = 契约面零写——gov 批没有契约合并凭据链，碰契约会被 `pre-merge-escalation-gate.mjs` 通用 1 机械 BLOCK（闸判据 :305-307 实测）。
- (b) 另起一个 `/arch-change` 轻批（文档面措辞级）专改三处链接指向归档内路径：断链清零，但多走一批、且改动的是历史记录行。
- (c) 本批顺带改：不可行（同 (a) 理由，机械闸必拦），列出仅为完备。

> **裁定（human 2026-10-15）**：选 **(b)**——本批红线「契约面零写」不变；本批波段二合并后由主线程另起一个 `/arch-change` 轻批专改三处链接指向归档内路径（纯契约措辞级，随批自有凭据链过合并闸）。断链窗口 = 波段二合并至轻批落地之间，窗口内由 `delivery/archive/README.md` 旧→新映射兜底；时序与留根见 §6。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> 双时点标注（pre-commit / post-commit）；重放类先断言输入非空（V4 豁免清单非空、V5 spine 文件非空）；基线已立（2026-10-15 本 worktree 实测：`pre-merge-escalation-gate --self-check` 43/43 + changeid-prefix 4/4 绿；`changes-index --self-check` 31/31 绿；`settings-lint` 3 files 0 RED 0 WARN；`spec-anchor-check` ok:true errors:[]）。

- **V1 · T1 搬移对账（post-commit 波段一）**：`for f in review-packet.md supervisor-verdict.json pod-retro.md docs-delta.md; do test -f "delivery/taskpackages/261015-voice-rightcol-label-overflow/$f" || echo "MISSING $f"; done` 无 MISSING 输出；`test ! -d delivery/changes/dev`。
- **V2 · deny 双态实测（pre-commit，波段一/二各一次）**：落地会话试写探针文件（波段一 = `delivery/changes/dev/probe.md`；波段二 = `delivery/changes/probe.md`）被 enforce-scope 拦截，拦截信息含 `universal.denyCreate` 与废止注记字样；两段拦截原文进 landing-summary（对偶 261014 批 deny 实测先例；探针文件不落盘）。
- **V3 · 合并闸 BLOCK/PASS（pre-commit 两波）**：`node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` 绿且输出含新增结构检查两臂（残件分支 BLOCK / 干净分支 PASS；波段二追加旧伞分支 BLOCK 臂）；波段一 landing 期对一个真实旧布局分支（`dev/pod-m2` 或其等价夹具）实跑一次闸取 BLOCK 证据留痕。
- **V4 · 活树零残留（post-commit 波段二）**：§3 复扫模式七类子检逐条跑（斜杠 / 分段字面量 / 转义 / 裸 token / 目录 token / 交替形 / 裸伞形），命中集 == T6 第 3 条豁免清单枚举 ∪ 设计保留位清单（下枚举，v0.3 R2-F2 增列第三类）∪ 本批 PROPOSAL 自身引用；三类分列——豁免类 = 零改写历史面（append-only 纪律），设计保留位 = post-landing 活树按本批自身设计仍保留旧路径字面量的位置（其存在目的就是拦旧路径或指向归档，非残留漏改），逐件枚举：
  - ① scope-map 双齿：`.claude/scope-map.json` `universal.denyCreate` 的 `delivery/changes/dev/**`（T1 第 2 条）与 `delivery/changes/**`（T6 第 6 条）两条目及其 `_doc` 废止注记；
  - ② 合并闸结构检查：`.claude/scripts/pre-merge-escalation-gate.mjs` 的 `git ls-tree <结果树> -- delivery/changes/dev` / `-- delivery/changes` 精确路径参数、BLOCK 消息内旧路径指引文、自检两臂（残件分支 / 旧伞分支）夹具内旧路径树字面量（T1 第 3 条 / T6 第 6 条）；
  - ③ 锚识别保留位：`.claude/scripts/lib/governance-gate-shared.mjs` ANCHORABLE_TEST（:40）保留的 `changes` 根枚举项 + PROPOSAL 形态正则（:110-111）保留的旧 `delivery\/changes\/(arch|dev)\/` 分支（T6 第 5 条明文「`changes` 根保留 / 旧分支保留，归档引用可解析」）；
  - ④ 归档路标与索引指针：`delivery/archive/README.md` 旧→新映射四行（书写旧路径即其功能，T6 第 2 条）+ `delivery/review/INDEX.md` 头部指针行（指向 `delivery/archive/changes/arch/INDEX.md`，T6 第 4 条）；
  - ⑤ 改指归档的活文面：`docs/notes/ui-implementation-strategy.md` :3 来源行与 `docs/showme/README.md` :27-30 改指归档的 3 行——改写终点 = 归档内路径，字面量含 `changes/` 子路径属设计产物（T6 第 3 条改写类的归档指向行）；
  - ⑥ 本批落地证据件引文：landing-summary（w1 + 终态）与 landing-verdict（w1 + 终态）中 V2 拦截原文 / V3 夹具输出 / V7-V8 对账记录引用的旧路径字面量（与「本批 PROPOSAL 自身引用」同类，证据件非 PROPOSAL 本体故单列）。
  豁免清单与设计保留位清单全文随 landing-summary 落盘，判官 spot-check；并集与 census 命中一一对应（261014 批 V1 范式）；census 绝对计数含本批自身引用漂移（§3 注记），landing 期按当日实测跑、以并集对账为判、不以绝对计数为判。
- **V5 · intake INDEX 与 frontmatter 实扫一致（post-commit 波段二）**：`node .claude/scripts/changes-index.mjs --self-check` 绿（含 intake lane 新臂）；`ls delivery/intake/*.md | wc -l` == INDEX 数据行数；抽 2 份 spine 人工对 frontmatter status / 触面与 INDEX 行一致。
- **V6 · 机检绿（post-commit 波段二）**：`node .claude/scripts/settings-lint.mjs` 0 RED；`node .claude/scripts/spec-anchor-check.mjs` ok:true errors:[]；`node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL 路径>`（pre-FROZEN 必跑一次，锚形态 + anchorUnion 逐 token 核注记无夹带路径；**波段二终态复跑预期升为 anchorUnion ≡ manifest 全等**——T6 第 5 条扩根生效后，当前 16 件新树锚的可机辨性缺口闭合）。
- **V7 · INDEX 切换对账（post-commit 波段二）**：`test -f delivery/archive/changes/arch/INDEX.md`（旧 INDEX 随归档冻结、零改写）；`test -f delivery/review/INDEX.md` 且其数据行集 == 落地窗口前移批名单（T6 第 1 条留痕）+ 头部指针行指向归档；`node .claude/scripts/changes-index.mjs` 连跑两次，第二次 `git diff --stat` 对两份新 INDEX 与缓存零变化（幂等）。
- **V8 · 波段二时机前置（波段二 landing 前）**：`git branch --contains` 核 `dev/pod-m2`、`dev/260915-m1-k1a-key-layer-dispatch`、`gov/261013-gov-review-lf-eol-teeth-pin` 与 main 的合并实态；任一未合 → 主线程按 T6 第 8 条裁定（等窗口 / 先合 / 豁免）并留痕 landing-summary；未留痕不进波段二 landing。
- **V9 · 契约零写（post-commit 两波）**：两波各自合并 diff 跑 `git diff --name-only <base>..HEAD -- docs/contracts/` 均为空（与 OQ-A 裁定互证）。

## 6. 留根清单

- **契约 3 处历史链接改向（OQ-A 裁 (b)，2026-10-15）**：本批不碰契约；波段二合并后主线程另起 `/arch-change` 轻批专改三处链接指向归档内路径。断链窗口内由 `delivery/archive/README.md` 映射兜底（按批目录名可解）；轻批落地前断链在册、不另立 debt 条目（窗口有界、处置已裁）。
- **261015-voice-rightcol-label-overflow 包 dev INDEX 行 `open`**（`delivery/taskpackages/INDEX.md` :13）：行状态刷新归既有 closeout/upsert 机制，非本批；本批只归位四件制品。
- **frozen/yes 停滞异常行 5 行**（T6 第 1 条名单）：随归档冻结，落地窗口逐行核注留 landing-summary；日后重启 = 新立 changeId 引旧批。
- **漂移标 7 天阈值**：首个真实误报 / 漏报实例出现时重估（T5 第 2 条注记）；复议机械闸触发 = 下轮 loop-retro 仍见漂移（human 裁定②）。
- **261013 gov 批与 260926 arch 批的窗口处置**：V8 留痕为准；未合 main 的旧布局分支未来撞齿二 BLOCK 属设计态。
- **showme HTML 快照内嵌的旧路径文字**：一次性视觉存档内容不改；未来重生成时再刷。
- **spec 面两文件指针勘误**（`docs/requirements.md` §9 头注 + `docs/architecture.md` :278 仓结构行）：均走档 0 勘误通道——requirements §9 追加勘误行、architecture §12 变更记录追加勘误行（append-only 合规；architecture :278 为 v0.3 R2-F1 增列）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-15 - architect

**四查**：① 意图保真——human 原话四段（§0）逐字核对派发链在册，四裁（方案 B / 检测不拦截 / 只留裁定+依据 / intake+retro 前移）与 §4 照录一致；改判记录（261014 批 PROPOSAL :4「`changes/` 名称保持不变，本次不改名」）verbatim 实测吻合。② 纪律合规——G1 改写面限定路径与 lane 措辞机械刷新、零语义变更，符合 G1 纪律；architect.md 自改递归申报（T6 第 9 条）如实；append-only 豁免清单（T6 第 3 条）与 261014 批 T4 先例（「路径是当时事实，批改即篡改记录」，:177 verbatim 实测）同型。③ 撞车复核——与 261014 dev-lane 批、261015 face-loop-unify 批、change-intake/debt 既有 SSOT 无冲突；INDEX 现态（261013 frozen/—、260926 draft/—、frozen/yes 5 行）实测与 T6 第 1 条名单一致。④ 牵连面——见 F1（sweep 漏件）。

**判档终裁**（只升不降）：自判「轻路径不适用、走 Track B 主路径」**成立**——改写面触 `.claude/scope-map.json`、`.claude/hooks/**`、`.claude/scripts/**`、`.claude/commands/**`、`.claude/agents/**`、`AGENTS.md`、`docs/DEV-GOVERNANCE.md`，命中 `gov-change.md` 轻路径判据③任一即恒 Track B；Track A 段判据（只看收敛度）不命中——human 2026-10-15 四裁定方向已收敛，无升级项。本批按 Track B 主路径全件推进，retro 不豁免。

**亲核留痕**（地面真值实测，非转述采信）：T1 四件残件与 taskpackages 同 id 目录零文件名冲突、`delivery/changes/dev/INDEX.md` 缺席、commits `82c0a65`/`6d199f8`/`d827064` 在册、dev INDEX 行 :13 `open`、:25 `closed`、在途两分支在册——全吻合。机械面 file:line：`enforce-scope.mjs` checkUniversal :56-59（denyCreate 单机制、无 universal denyWrite）✓；gate :36 注释 / :244 merge-tree / :132 契约正则 / :305-307 通用 1 ✓；`brief-inputs.mjs` :758 planRef ✓；`governance-gate-shared.mjs` :34/:39/:40（ANCHORABLE_TEST 四根枚举）/ :110-111 ✓；`changes-index.mjs` :245/:260/:309/:426/:374 ✓；契约 3 处链接 `interface-contract.md` :5/:90/:111 ✓；scope-map 写槽 8 对行 ✓；`requirements.md` :690 ✓；showme README :27-30 ✓；requirement-change :47/:37/:22 ✓；change-intake :15/:23/:27/:29 ✓（全文无 companion，grep 实测 0）；spine 261010 :6/:8、261001 :27-29/:68、261014 :24/:31 与 companion 头 5 行 ✓；debt 模板 :53、debt-ledger-shared :51/:214/:226/:237/:239（无未知字段拦截）✓；INDEX 9 份 ✓；261015 两批 landed/yes ✓。V6 基线复跑：gate --self-check 43/43 + changeid-prefix 4/4、changes-index --self-check 31/31、settings-lint 3 files 0 RED 0 WARN、spec-anchor-check ok:true——全绿。preflight 复跑 `--preflight` 实测 anchorCount = 58，manifest 围栏块实测 74 行，58+16 与「静默丢弃 16 件新树锚」论断算术吻合。结构字段：landing-manifest 围栏块与 packet-id 行在场 ✓。OQ-A 裁 (b) 与 §0 非目标（契约零写）、§6 留根、V9 自洽 ✓。

**findings**：

- **F1 · SHOULD · sweep 漏件（UNCLASSIFIED≠0）**：§3 四 pattern（changes/arch|intake|retro|dev）漏两类文本形态——`(arch|dev)` 交替形与裸伞形 `delivery/changes/`（无子路径）；判官以宽 pattern `changes/` 复扫实测 7 件活面命中既不在改写面/manifest 也不在豁免清单：`.claude/scripts/devloop-artifact-check.mjs` :137 与 `.claude/scripts/taskpackage-required-check.mjs` :492（错误消息串硬编码 `expect delivery/changes/(arch|dev)/<id>/PROPOSAL.md`——行为经共享 `isProposalMdPath`〔governance-gate-shared.mjs :106，已在改写面〕自动更新，仅消息 stale）、`.claude/agents/planner.md` :39（prose 引双路解析旧树，行为同由共享函数承载）、`.claude/hooks/supervisor-verdict-gate.mjs` :207（维护注引 `delivery/changes/**` closeout 根）、`.claude/skills/brainstorming/SKILL.md` :59/:62（**活指令**「成形设计落 `delivery/changes/`」——伞解散后直接指向废止树，齿一会拦但指令本身错）、`.claude/skills/merge-to-main/SKILL.md` :18（INDEX lane 拆分描述，T6 后 arch INDEX 迁 `delivery/review/`）、`.claude/references/plain-file-handoff.md` :9（「勿存 `delivery/changes/`」指引）。V4 五类模式同不含此两形态 → 命中集对账不会 fail-loud，缺口静默。连带：T6 census「references 12」与 §3「references 13 件」与宽 pattern 实测 13 件（含 plain-file-handoff）三方不齐。修法方向：§3 模式集增交替形与裸伞形两类子检；7 件逐件分类（改写入 manifest 或豁免+理由）；计数句随批顺修。改法机械、不涉方向，不动判档。
- **F2 · NIT · file:line 个别偏移**：`changes-index.mjs` STATE_CACHE_PATH 记 :32 实 :31、扫描根记 :284 实 :283；gate readGovAnchors 路径拼接记 :89-90 实 :88-89；`enforce-scope.mjs` docsTopLevel 提示句记 :66 实 :67。PROPOSAL 节首已声明「搬运者自负复证责任」，不承重，落地时以实测行为准。
- **F3 · NIT · census 自引用漂移**：四 pattern 本轮复测 289/64/24/443，较 §3 记录 288/63/23/442 各 +1——成因 = 本 PROPOSAL 自身含 pattern 字串且起草中途增长（grep -rl 含未跟踪文件）。不承重（V4 按豁免集对账非绝对计数），建议 §3 注记「census 计数含本 PROPOSAL 自身，随批内容漂移」，landing 期按当日实测跑 V4。
- **F4 · NIT · T3 证据措辞**：「grep -n debt 仅命中语料行与查重行」——实测仅命中 :27 一行（:29 查重行不含 debt 字串）；实质结论（全文件无晋升条文）不变。

verdict: BLOCK(F1)

### R2 - 2026-10-15 - architect

**轮次定位**：聚焦审——R1 findings 修订到位性 + 修订是否引入新矛盾；R1 已亲核面不重复全量重核。

**F1（R1）到位性核验**：七类复扫模式入 §3 硬项（斜杠/分段字面量/转义/裸 token/目录 token/交替形/裸伞形）✓；R1 点名 7 件逐行 verbatim 实测在场——`devloop-artifact-check.mjs` :137、`taskpackage-required-check.mjs` :492、`planner.md` :39（交替形 `expect delivery/changes/(arch|dev)/<id>/PROPOSAL.md`）、`supervisor-verdict-gate.mjs` :207、`brainstorming/SKILL.md` :59/:62、`merge-to-main/SKILL.md` :18、`plain-file-handoff.md` :9（裸伞形）✓；7 件逐件入 §3 表且分类有据（G3 三件 / agents 一件 / references·skills 三件）✓；7 件全部入 manifest（围栏块 :38/:39/:43/:50/:59/:65/:68 对应行）✓；行为由共享 `isProposalMdPath`（governance-gate-shared.mjs :106 实测在册）自动更新、仅消息 stale 的论断与源码一致 ✓。**但**裸伞形复扫申报 4 件、实测活面尚有第 5 件漏网（见 R2-F1）——UNCLASSIFIED=0 硬项未闭合。

**计数联动核验**：manifest 围栏块 awk 实测 81 行 ✓；§0「manifest 81 − 波段一 7 件」算术吻合（波段一 7 件 = manifest :14-20：scope-map + gate + brief-inputs + taskpackage 四件）✓；T6 第 5 条 preflight 注记「+7 全为 .claude 可机辨锚、16 新树锚缺口不变」与 81 口径自洽（81 − (58+7) = 16）✓；review profile 计数（agents 9 / commands 5 / references 14 / rules 1 / skills 5 / scripts 11 含 lib 1 / hooks 4 / route-fixtures 1）与 manifest 逐类点数吻合 ✓；T6 census 双口径注记（references 13 vs 14、agents 8 vs 9）实测成立——`integrator.md` 与 `references/requirement-change.md` 的 `changes/` 命中 grep 实测均为 0、纯 T5 清单齿入 manifest ✓；四 pattern census 复跑 = 289/64/24/443，与 F3 注记申报值逐字吻合 ✓；状态行 DRAFT v0.2 修订摘要如实 ✓；结构字段：landing-manifest 围栏块与 packet-id 行在场 ✓。

**R1-F2/F3/F4 顺修核验**：F3 自引用漂移注记在册且机制属实 ✓；F4 T3 证据句已收敛、实质结论不变（`grep -n "debt" .claude/references/change-intake.md` 实测仅 :27 一行）✓；**F2 顺修四处行号全部反向**（见 R2-F3）。

**findings**：

- **R2-F1 · SHOULD · F1 修订的裸伞形复扫仍漏 1 件活面命中（UNCLASSIFIED≠0）**：`docs/architecture.md` :278 verbatim：「- \`docs/\`（spec 面 + contracts + notes）、\`delivery/changes/\`（治理/开发证据）、\`.claude/\`（治理机制）。」——裸伞形命中（后不接四子路径，四子路径 pattern 对该文件零命中、`grep -c "changes/arch"` = 0，故 v0.1 四 pattern 口径与 R1 复扫均漏）；该文件不在 §3 表任何行、不在 manifest、不在 T6 第 3 条豁免清单。性质 = spec 面活指针，与已入改写面的 `docs/requirements.md` §9 头注指针行（:690）同型对偶——T6 伞解散后 :278 指向已废止树，属目标效果缺口（记录链地图的 spec 侧指针断链）。修法方向：与 requirements.md 同型——入 §3 spec 面行 + manifest + T6 第 3 条改写类（档 0 勘误通道）；机械补列，不动判档、不动方向。
- **R2-F2 · SHOULD · V4 并集判据缺「设计保留位」类，落地期按字面跑必红**：post-commit 波段二活树按本批自身设计仍保留旧路径字面量，均非豁免类（零改写历史面）也非本批自身引用——① scope-map denyCreate 两齿（T1 `delivery/changes/dev/**` + T6 第 6 条扩 `delivery/changes/**`）；② `pre-merge-escalation-gate.mjs` 结构检查路径 `-- delivery/changes/dev` / `-- delivery/changes` 及自检两臂夹具（T1 第 3 条/T6 第 6 条，齿的存在本身就是为拦旧路径）；③ `governance-gate-shared.mjs` :110 保留分支 `/^delivery\/changes\/(arch|dev)\/.+\/PROPOSAL\.md$/`（T6 第 5 条明文「旧分支保留，归档引用可解析」，实测在册）；④ 新增件 `delivery/archive/README.md` 旧→新映射四行与 `delivery/review/INDEX.md` 头部指针行（指向 `delivery/archive/changes/arch/INDEX.md`）——其存在目的就是书写旧路径；⑤ 本批落地证据件（landing-summary/verdict）引用的拦截原文与夹具输出。V4 判据「命中集 == T6 第 3 条豁免清单枚举 ∪ 本批 PROPOSAL 自身引用」对以上任一项均不可满足，落地期要么误红、要么被迫临场扩清单破「清扫模式与验收判据同源」。修法方向：V4 判据补第三类「设计保留位」逐件枚举（齿、闸检查路径与夹具、governance-gate-shared 保留分支、archive README 映射行、review INDEX 指针行、本批证据件引文），与豁免类（零改写历史面）分列、不混入 T6 第 3 条清单。
- **R2-F3 · SHOULD · F2 顺修四处行号全部反向（评审驱动修订引入错误）**：三法交叉实测（grep -n / awk NR / head|tail / node split）一致——`changes-index.mjs` `STATE_CACHE_PATH` 实 **:32**（v0.2 记 :31）、staleReport 扫描根实 **:284**（记 :283）、`pre-merge-escalation-gate.mjs` readGovAnchors PROPOSAL/landing-verdict 路径拼接实 **:89-90**（记 :88-89）、`enforce-scope.mjs` docsTopLevel 拦截提示句实 **:66**（记 :67）。v0.1 原值本正确，R1-F2 实测整体偏一、v0.2 照单改致反向。行号不承重（节首「搬运者自负复证责任」在册），但冻结稿不应携带经评审「顺修」反而变错的引用——四处回改实测值；T6 证据段同引 :31/:283 随同改。
- **NIT-1 · firmware red-baseline 退役面未入豁免枚举**：`firmware/main/test/acceptance/red-baseline/screen-unification-red-baseline-summary.md` :118/:167 含 `delivery/changes/arch/260908-gov-review-human-deputy-shadow/...` 历史链接（四子路径口径本就命中、在 census 内）；T6 第 3 条豁免类只枚举「acceptance/archive/\*\*」。建议扩为「firmware/server acceptance 退役面（archive/\*\* 与 red-baseline/\*\*）」或逐件点名。历史冻结记录零改写语义自明，不入 BLOCK。
- **NIT-2 · T6 双口径注记未覆盖 commands**：manifest commands 5 件 vs 路径 census 1 件（build-feature），差 4 件（arch-change / gov-change / loop-retro / requirement-change——`changes/` 命中实测均 0、纯 T5 清单齿入 manifest）；v0.2 注记只点名 integrator 与 requirement-change。建议补全点名，消歧「路径 census 口径 vs manifest 口径」在全类型上的适用。

**判档与升级**：无判档变动——R2-F1/R2-F2/R2-F3 均为机械补列或判据补类，不改方向、不触 human 已裁四点、不触 OQ-A 裁定；无「影响目标效果」级方向性问题，不升级 human。

**亲核留痕**（地面真值实测，非转述采信）：R1 点名 7 件逐行 sed/grep 实读 ✓；manifest `awk` 计数 81 ✓；四 pattern census 复跑 289/64/24/443 ✓；裸伞宽扫 `grep -rln "delivery/changes/"`（排除树内/归档/taskpackages/debt/showme-html/firmware·server 退役面）逐件过类，孤出 `docs/architecture.md` :278 ✓；`grep -c "changes/arch" docs/architecture.md` = 0 ✓；commands 六件 `changes/` 计数实测 1/0/0/0/0/0、integrator 0、references/requirement-change 0 ✓；行号三法交叉（`head -n N | tail -n 1`、`awk NR`、node split）✓；gate :36（纯函数注）/ :132（契约正则）/ :244（merge-tree）、governance-gate-shared :34/:40/:106-111、enforce :56-59（checkUniversal）复核 ✓；`grep -n debt change-intake.md` 仅 :27 ✓；firmware red-baseline :118/:167 实读 ✓；VERDICT_STATUS_RE（governance-gate-shared.mjs :629）token 形态核对，本 verdict 行可被机锚消费 ✓。

verdict: BLOCK(F1,F2,F3)

### R3 - 2026-10-16 - architect

**轮次定位**：R3——聚焦 R2 findings 修订到位性 + 修订引入新矛盾 + architect 越界申报裁断；R1/R2 已亲核面不重复全量重核。

**R2 findings 到位性核验**（逐项实测）：

- **R2-F1 到位 ✓**：`docs/architecture.md` :278 verbatim 实测吻合（「- \`docs/\`（spec 面 + contracts + notes）、\`delivery/changes/\`（治理/开发证据）、\`.claude/\`（治理机制）。」）；五联动点齐——manifest :73、§3 spec 面行、§3 硬项第 7 类、T6 第 3 条改写类（档 0 勘误通道 + §12 勘误行）、review profile :8、§6 留根末条；`grep -c "changes/arch" docs/architecture.md` = 0 与申报一致。
- **R2-F2 到位 ✓**：V4 三类并集成文（豁免 ∪ 设计保留位 ∪ 本批自身引用），设计保留位六条逐件枚举有据（① scope-map 双齿 / ② 闸结构检查路径与自检夹具 / ③ ANCHORABLE_TEST `changes` 根与 `isProposalMdPath` 旧分支保留位 / ④ archive README 映射四行 + review INDEX 头部指针行 / ⑤ 改指归档活文面（notes :3 + showme :27-30 三行）/ ⑥ 本批证据件引文）；T6 第 3 条「第三类与本豁免类分列、不混入本清单」cross-ref 如实。
- **R2-F3 终裁 ✓**：判官独立实测（`grep -n` + `awk NR`）——`STATE_CACHE_PATH` **:32**、staleReport 扫描根 **:284**、gate `readGovAnchors` 路径拼接 **:89-90**、`enforce-scope.mjs` docsTopLevel 提示句 **:66**，与 v0.3 现稿四处全部一致。**终裁声明：R1-F2 当时实测整体偏一、R2 论断成立**——v0.1 原值本正确，v0.2 照 R1 改致反向，v0.3 回改到位；本判官以自身实测为终，与 R1 判官记录（F2「记 :32 实 :31」等四处）相悖处以本实测为准。§7 外无旧值残留（`:31`/`:283`/`:88-89`/`:67` grep 零命中）。
- **NIT-1 到位 ✓（裁可）**：豁免类扩列成文且带实测证据（`screen-unification-red-baseline-summary.md` :118/:167 两处历史链接实测在册）；server 侧「无 red-baseline 目录」经 `find server -type d -name red-baseline` 实测为空——architect 按事实分列而非对称写法，符合事实优先，裁可。
- **NIT-2 到位 ✓**：commands 四件（arch-change / gov-change / loop-retro / requirement-change）点名入 T6 证据段注记；四件 `changes/` 命中 grep 实测均 0（build-feature 非 0 为路径 census 面）、integrator 0、references/requirement-change 0——双口径注记事实成立。
- **修订引入新矛盾扫描**：§0「75 件未落地」= manifest 82 − 波段一 7 件，算术吻合（manifest 围栏块 awk 实测 82 行；波段一 7 件 = 围栏块 :14-20）✓；T6 census 双口径注记时点标注（2026-10-15 / 2026-10-16）如实、§7 外无残留旧值 ✓；结构字段（manifest 围栏块 + packet-id 行）在场 ✓。**但** T6 第 5 条 preflight 注记「16 件新树锚缺口论断不变」与现实脱钩（并入 R3-F1）。

**architect 越界申报裁断（本轮焦点）**：

- **申报属实**：7 件（devloop-artifact-check.mjs / taskpackage-required-check.mjs / supervisor-verdict-gate.mjs / planner.md / plain-file-handoff.md / brainstorming SKILL / merge-to-main SKILL）在 §3 表与 manifest（:38/:39/:43/:50/:59/:65/:68），不在 §2 任何 T 的目标文件行（逐件对照 T1/T5/T6 目标文件行实测）。定性：R1-F1 修法方向本就指令「7 件逐件分类改写入 manifest 或豁免+理由」，入 §3/manifest 是判官指令的执行、**非越界夹带**；缺口是 §2 传播未完成。
- **机门实测（真机函数，非推演）**：node 直调 `governance-gate-shared.mjs` 的 `parseSection2AnchorLists` + `extractManifestBlock` + `compareAnchorSets` 跑本 PROPOSAL——**§2 并集 59、manifest 82、onlyInManifest = 23 = 16 新树锚 + 7 申报件、onlyInSection2 = 0**。承重消费点 = `landing-fidelity-check.mjs` :604-615：闸前置链内 B7 对账失配即 errors（RED）。T6 第 5 条扩根后 16 件新树锚回归并集（其在 §2 各 T 目标文件行内、仅被现 ANCHORABLE_TEST 过滤），但 7 件不在 §2 解析域、扩根后仍 onlyInManifest = 7 → **波段二 landing 必红**；preflight 的 anchorUnion（landing-fidelity-check :260/:270，同源 §2 解析并集）同理，V6「anchorUnion ≡ manifest 全等」预期按同法必不达成。
- **裁决：不采「manifest 为准、§2 不必枚举尽」口径**——B7 是双向集合等式硬门，改写 V6 预期行救不了闸红；且 packet-id 绑定「§2 改法集（T1–T6）」，§2 漏枚举 = packet 授权面与 manifest 落地面不一致，landing-verifier 逐文件对账失去 §2 侧依据。**修法 = 7 件全部补入 §2 T6 目标文件行**（归 T6 活引用改写类：T6 第 3 条 catch-all「`.claude/**` 全部命中件」已语义覆盖，无一件属 T1 fixture〔brief-inputs.mjs 已在 T1 行〕或 T5 清单齿）；同步 T6 第 5 条 preflight 注记改现态实测口径（现态 anchorUnion 59 / 缺口 23 = 16 新树 + 7 未枚举；两条归零路径 = ANCHORABLE_TEST 扩根 + 7 件补入 §2）。补入后 §2 并集 66，扩根后 82 ≡ manifest，V6 预期行无需改写。

**findings**：

- **R3-F1 · BLOCKING · 7 件 §2 未枚举致 B7 机门波段二 landing 必红**：证据与裁断见上节。修法方向：architect 修订轮把 7 件补入 §2 T6 目标文件行（逐件带注记，形态与既有行一致）+ T6 第 5 条注记算术改现态实测口径。机械补列、不动判档、不动 human 已裁点；但 B7 为 landing 硬门，未补前本 PROPOSAL 不可 FROZEN 进 landing。

**判档与升级**：无判档变动（R3-F1 为机械补列）；无方向性问题，不升级 human 方向裁断。**熔断提示（如实申报）**：R1–R3 三轮未 GREEN，按 `AGENTS.md`「治理路由」熔断条款触发 human 三选一；余量如实——仅剩单条机械补列（7 行 + 一处注记），无方向性分歧，修法已给明。

**亲核留痕**（地面真值实测，非转述采信）：architecture.md :278 verbatim ✓；manifest 围栏块 awk 82 行、波段一 :14-20 七件 ✓；四处行号独立实测 :32/:284/:89-90/:66 ✓；brief-inputs.mjs :758 planRef verbatim ✓；gate :36/:132/:244、governance-gate-shared :40（ANCHORABLE_TEST 四根）/ :106-113（isProposalMdPath）/ :110-111 ✓；firmware red-baseline :118/:167 ✓、`find server -type d -name red-baseline` 空 ✓；commands 四件 0 命中 / build-feature 非 0 / integrator 0 / references-requirement-change 0 ✓；§7 外旧行号残留 grep 零命中 ✓；真机 B7 实测（parseSection2AnchorLists union 59 / manifest 82 / onlyInManifest 23 逐件名单 / onlyInSection2 0）✓；B7 消费点 landing-fidelity-check.mjs :604-615（失配 errors）与 preflight anchorUnion :260/:270 同源 ✓；新树 16 件在 §2 各 T 目标文件行内（T4 二 + T5 三 + T6 十一）✓；结构字段在场 ✓。

verdict: BLOCK(F1)

### R4 - 2026-10-16 - architect

**轮次定位**：R4——熔断后换新判官实例再战 1 轮（human 三选一裁定留痕见状态行）。聚焦 R3-F1 修订到位性 + 修订是否引入新矛盾；前轮已终裁事实（R2-F3 四处行号 v0.3 回改、判档 Track B 主路径、riskTier medium）不重开，本轮以独立实测复核，与在案记录一致，无推翻项。

**R3-F1 到位性核验**（逐项实测）：

- **7 件补列 ✓**：§2 T6 目标文件行逐件 verbatim 实测在场、各带注记——`.claude/scripts/devloop-artifact-check.mjs`（改——错误消息串交替形 stale 刷新，行为经共享 isProposalMdPath 自动更新；R1 增列、R3-F1 补入本行）、`.claude/scripts/taskpackage-required-check.mjs`（改——同型错误消息串交替形 stale 刷新；R3-F1 补入本行）、`.claude/hooks/supervisor-verdict-gate.mjs`（改——维护注裸伞 closeout 根刷新；R3-F1 补入本行）、`.claude/agents/planner.md`（改——交替形 prose 旧树双路解析描述刷新；R3-F1 补入本行）、`.claude/references/plain-file-handoff.md`（改——裸伞存址指引刷新；R3-F1 补入本行）、`.claude/skills/brainstorming/SKILL.md`（改——裸伞活指令「成形设计落 delivery/changes/」刷新；R3-F1 补入本行）、`.claude/skills/merge-to-main/SKILL.md`（改——裸伞 INDEX lane 拆分描述顺修；R3-F1 补入本行）。分组 scripts 2 / hooks 1 / agents 1 / references 1 / skills 2 = 7，与 dispatch 申报一致 ✓。
- **T6 第 5 条 preflight 注记现态口径 ✓**：T6 第 5 条现态注记（「现态口径见 T6 证据段 preflight 注记：v0.4 实测缺口 23 = 16 件新树锚 + 7 件 §2 未枚举件——本条扩根闭合 16 件、R3-F1 §2 补列闭合 7 件，两路归零」）+ T6 证据段 v0.4 真机复测快照（anchorUnion 59 / manifest 82 / onlyInManifest 23），与本轮实测自洽——现态并集 66 = 59 + 7，缺口 23 = 16 + 7，两条归零路径算术闭合。
- **V6 预期行未改写成立 ✓**：实测 §2 并集 66 + 扩根回归 16 = 82 ≡ manifest；V6 行「T6 第 5 条扩根生效后，当前 16 件新树锚的可机辨性缺口闭合」描述精确——16 件缺口属可机辨性（ANCHORABLE_TEST 过滤）、由扩根闭合，7 件缺口属 §2 枚举、已由本补列闭合（R3 终裁「V6 预期行无需改写」经真机复测维持）。

**真机对账终裁**（node 直调 `governance-gate-shared.mjs` 的 `parseSection2AnchorLists` + `extractManifestBlock` + `compareAnchorSets`，独立复测现态 §2）：problems 空；§2 并集 = **66**；manifest = **82**；onlyInSection2 = **0**；onlyInManifest = **16**，逐件核验全部为新树根路径（intake 10 / retro 3 / review 2 / archive 1）——与 architect 自报（66 / 82 / 16 = 新树锚）一致。7 件全部在 §2 并集内、均被现 ANCHORABLE_TEST（governance-gate-shared.mjs :40 实读，delivery 根枚举 `workstreams|milestones|taskpackages|changes` 四根未扩）识别为可机辨锚；16 件新树锚在 §2 各 T 目标文件行内、被解析器 canonical 过滤（:416 ANCHORABLE_TEST.test 实读确认）排除在并集外，T6 第 5 条扩根后即回归——「两路归零后 82 ≡ manifest」的机械论断经真机复测成立，B7 双向集合等式可达。

**NIT 勘误核对 ✓**：`isProposalMdPath` 源码 :106-113 实读在册（PROPOSAL 形态正则 :110-111 = `/^delivery\/changes\/(arch|dev)\/.+\/PROPOSAL\.md$/`）；`resolveOriginRef` 为同文件 :275 的另一函数，勘误方向正确；PROPOSAL 两处落点（T6 证据段、T6 第 5 条）均已用 `isProposalMdPath`，残留 `resolveOriginRef` 字样仅存于勘误注记自身引述（状态行「原误植 resolveOriginRef」与 T6 证据段勘误句），非误指。

**修订引入新矛盾扫描 ✓**：状态行 DRAFT v0.4 修订摘要与熔断裁定留痕如实；manifest 围栏块 82 行（awk 与 extractManifestBlock 双口径一致）；§0 波段设计态算术「manifest 82 − 波段一 7 件 = 75 件未落地」吻合、波段一 7 件逐件在 manifest（:14-20）；review profile 计数逐类复核（agents 9 / commands 5 / references 14 / rules 1 / skills 5 / scripts 11 含 lib 1 / hooks 4 / route-fixtures 1）与 manifest 一致；§3 表、§4 human 已裁点（立项 ack 四点 + OQ-A (b) + 两波程序变体 + 7 天阈值）未被本轮编辑触及。

**结构字段核对 ✓**：landing-manifest 围栏块（:13-96）与 packet-id 行（:6）在场。

**findings**：无——R3-F1 修订到位，未发现新 finding。

**判档与升级**：无判档变动（维持前轮终裁：Track B 主路径、riskTier medium）；无方向性问题，不升级 human。熔断后本轮 GREEN，评审循环收敛。

**亲核留痕**（地面真值实测，非转述采信）：governance-gate-shared.mjs :40（ANCHORABLE_TEST 四根枚举）/ :106-113（isProposalMdPath）/ :110-111（正则 verbatim）/ :275（resolveOriginRef 另函数）/ :360-445（解析器与 compareAnchorSets 源码）/ :416（canonical 过滤）实读 ✓；node 直调三函数：union 66 / manifest 82 / onlyInSection2 0 / onlyInManifest 16 逐件名单全为 intake|retro|review|archive 新树根 / problems 空 ✓；7 件 ∈ 并集且 ANCHORABLE_TEST 全 true ✓；16 件被现测试过滤（并集含 0 件）✓；isProposalMdPath 现态行为实测（`delivery/review/x/PROPOSAL.md` → false、`delivery/changes/arch/x/PROPOSAL.md` → true，扩根前旧分支保留语义与 T6 第 5 条一致）✓；manifest awk 计数 82、波段一 7 件在册、82−7=75 ✓；T6 目标文件行 7 件注记逐件实读 ✓；T6 第 5 条注记、T6 证据段 v0.4 快照、V6 行、状态行 v0.4 逐字实读 ✓；结构字段在场 ✓。

verdict: GREEN
