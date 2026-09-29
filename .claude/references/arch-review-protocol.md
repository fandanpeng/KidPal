# 架构 Review 协议（arch-review-protocol）

> spec 层对抗 review 的「怎么跑」规则:辩论节奏、收敛/deadlock、finding 结案、pass-oracle、spec↔contract 保障。
> 这是 dev-loop `verify-gates.json` + 升级手册的 spec 层对偶:dev-loop 的 oracle 是测试退出码,本 loop 的 oracle 是**对抗挑战被提出并被解决/记账的记录**(架构无测试退出码)。

> 2026-08-02：路由永久化为「**Track B 常驻轻路径（默认） / 触发式 Track A 重路径**」，SSOT = 下「双轨路由」节（立法来源 `delivery/changes/arch/260802-gov-review-track-b-permanentize/PROPOSAL.md`；2026-08-01 临时路由安排同日退役）。标注「Track A 触发时适用」的各节仅在判据命中时生效。

## 两个 profile（共同机制 + 各自 oracle）

同一 loop(intake+draft → charter → challenger↔defender → adjudicator → pass-oracle)跑两个 profile:

- **spec profile**(`/review-arch`):审 spec(`docs/requirements.md` + `docs/architecture.md`、L0)是否可冻结。低影响 confirmed spec 文本由 moderator 落地。证据落 `delivery/changes/arch/arch-review-<spec>/`。oracle 见下「Pass-oracle」。
- **governance profile**(`/review-governance`):审 G1/G2/G3 开发治理变更是否自洽、可批准。**不落 canonical**,只产证据 + decision packet,human 批后由主线程 packet-bound landing（landing 通道见「双轨路由」节）。证据落 `delivery/changes/arch/gov-review-<id>/`。oracle 见下「Governance profile pass-oracle」。

**统一入口 `/arch-change`**(对偶 dev-loop `/build-feature`):把一个 plan/想法丢给 moderator,先 phase-0 分类再路由进上面某个 profile;`/review-arch`、`/review-governance` 是 profile 已知时的直达入口。**⚠「统一」的所指（本句即「`/arch-change` = 统一入口」这一定位的定义句）**：统一的**不是** Track A 与 Track B 两条轨，而是 **Track A 内部的 spec / governance 两个 profile**——`/arch-change` **是 Track A 的入口**；**Track B 常驻轻路径没有命令入口**，其入口 = 主线程直接起草 PROPOSAL（见「双轨路由」节）。故「命中升级判据 / 声明红队 → `/arch-change`」与「默认 Track B → 主线程起草」两句各指其一、并不冲突。**（边界 · 承 R2-F9）**「统一入口」在本仓是一个**通用短语**而非本命令的专名：`docs/DEV-GOVERNANCE.md` §7 的「**统一入口**」段标题指的是**治理 / spec 变更的入口总览**（同一段同时讲两条轨），`.claude/agents/portfolio-planner.md:25` 的「repair-intent 统一入口」指的是**修复意图的收口处**——二者各由本处上下文定义，**不在本句的覆盖范围内**；本句只定义「用作 `/arch-change` 标签时」的所指。

本文下面各节(intake+分类、辩论内循环、五条要点、finding 结案、渐进式严格、反空过)是**共同机制**,两 profile 通用;差异只在 charter 判据、可写落点与 pass-oracle。

## 路由权归属（author 不得自路由）

> **Track A 触发时适用**（默认路由 = Track B 常驻轻路径，见「双轨路由」节）。

**路由权 = 决定一个 change 走哪条 review flow（spec/governance lane）、什么 depth（lightweight/full）、是否转契约门——此判定独立于变更作者。** 三方承载，皆非作者：

1. **review-moderator** 分类（lane + review-depth；结果必须显式展示，含糊 AskUserQuestion 不猜——见下「Intake + 分类」）；
2. **architect-reviewer** eligibility 四裁断 **+ review-moderator 独立复判**（双锁；复判非 pass-through——见「Review-depth 分类」）；
3. **机器硬排除**（触 G1 指令根 / G3 强制层 / self-def / review-loop def / gate 变更 → 强制 full，机判不靠作者声称——见「Eligibility 判据」；**执行测出的 bug 走 Track B 快路径，见「双轨路由」节**）。

**作者不得自路由**：起草或提议变更者（architect 起草 subagent、或任何 plan 提出者）**不决定自己变更的 review 路径**；自认「小/轻/无碍」不作数——**存疑向严**（主观残留默认 `ESCALATE_FULL_REVIEW`）、**mixed 交 human**（AskUserQuestion，默认拆两次）。这是 ③ 作者≠判官（[DEV-GOVERNANCE §1.2](../../docs/DEV-GOVERNANCE.md)）在路由面的具体化：判官独立于作者，**路由（决定谁来判、按什么力度判）同样独立于作者**。

## Intake + 分类（`/arch-change` 统一入口）

`/arch-change` 下,moderator 先把 plan 分类再起草;`/review-arch`、`/review-governance` 直达时 profile 已定,跳过本节。

**主 lane(二选一)+ 一个横切 flag**—— 分类器只挑车道,不混两个 oracle:

- **spec lane**:触 `docs/requirements.md` + `docs/architecture.md` 的意图 / 行为 / 拓扑 / 边界 / 阶段(L0 蓝图是 spec 特例,不自动落地)。
- **governance lane**:触 `.claude/{agents,commands,references,rules,skills}`、`scope-map.json`、`*.settings.json`、`verify-gates.json`、`hooks/**`、`scripts/**`、或指令根(README/AGENTS/CLAUDE/DEV-GOVERNANCE)。
- **contract-touching flag(横切,非独立 lane)**:plan 是否实际触 `docs/contracts/**`;命中则在所属 profile 内触发 `architect-reviewer` 四分类 + 人类契约门(原子性:配套 spec/proposal 与契约同一 PR)。
- **mixed(命中 >1 lane)**:AskUserQuestion 展示分类,**默认建议拆两次跑**(spec 一次、governance 一次);强耦合到必须同一 PR 时,同会话先跑完 spec profile 落地再跑 governance profile。
- **纯 `docs/contracts/**`(无 spec/governance 配套)**:不在本 loop 处理 → 直走 `architect-reviewer` + 人类契约门。

分类结果**必须显式展示**;含糊用 AskUserQuestion 确认,不猜。

## Review-depth 分类 + Lightweight path（`/arch-change`，gov-review-architect-landing-mode 2026-07-08 human-approved）

lane 分类之后，`/arch-change` 再判 **review-depth**（**Track A 触发时适用**——默认路由 = Track B 常驻轻路径，见下「双轨路由」节；此处「Track B · 快路径」的「执行测出 bug」语义已并入该节的常驻 Track B）。review-depth 是**双轨**（按「是否执行测出」分流，**不按 G 层**）：**Track A · G 层路由**（新 idea，`lightweight-architect-review` vs `full-adversarial`，下节）+ **Track B · 快路径 / human 知情覆核**（执行测出的 bug，不论 G 几，见「双轨路由」节）。两轨皆是既有「对抗 review 人类拍板」工程门的**具名 depth 子道**（DEV-GOVERNANCE §5 保持 6 工程门，与 6 业务门无关），不是新增第 7 个工程门。

### 双轨路由（默认 Track B 常驻轻路径 / 触发式 Track A 重路径）

**本节为治理 / spec 变更现行路由唯一 SSOT**（2026-08-02 由 2026-08-01 临时路由安排永久化，立法来源 `delivery/changes/arch/260802-gov-review-track-b-permanentize/PROPOSAL.md`）。**默认路由 = Track B 常驻轻路径**；Track A（full-adversarial 机器）不再是默认档，由下方判据**触发式**启用。

**Track B 七步流程（默认）**：

1. 主线程起草 PROPOSAL（业务语言 + 初步技术方案 + human 取舍点标注，骨架见 `docs/templates/PROPOSAL.template.md`）→ human **立项 ack**；
2. **architect 评审循环**（只读 canonical、只写评审记录于 PROPOSAL §7；结论二分：GREEN / 打回；影响目标效果的打回项升级 human）至 GREEN；
3. 主线程凭「立项 ack + GREEN」**标 FROZEN**（FROZEN = 内容冻结；授权记 Status 行）；
4. 主线程 **packet-bound 落地**（G1/G2/G3 全落，landing-manifest 绑定，落地后不可回改 PROPOSAL）+ 写 `architect-landing-summary.md`（**骨架见 `docs/templates/architect-landing-summary.template.md`，复制勿手写**——缺 landing-manifest 围栏块即结构门 / landing-verifier 合法打回；unit-gate F4 + 批 A 两实例已在册，同根 ARQ-260924-landing-summary-template-pointer-gap）；
5. 主线程派 **landing-verifier** 独立核验（逐文件 diff + `landing-fidelity-check.mjs`）→ 绿；
6. 主线程补写 **retro report**（骨架见 `docs/templates/retro.template.md`）于提案目录——landing-verifier 绿后**趁热**写、先于合并完成，retro 不含合并锚（合并落点由 `delivery/changes/arch/INDEX.md` `mergedInMain` 列经 `changes-index.mjs` 机械现推承担）；
7. 主线程跑前置升级闸（`pre-merge-escalation-gate.mjs`，含 off-main 校验），放行 → 自动合并本地 main：治理批恒 `git merge --ff-only`（与 `git reflog main` 2026-09-08 起近期实务、`.claude/skills/merge-to-main` 步骤 4 一致；更早 3 例 gov 双亲合并 `8bd4a84`/`8c1cba3`/`33d8009` 系按本条旧文本执行，正是 260909-gov-review-gate-truthfulness T7 的收口对象；main 无独有 commit 时 ff-only 才成立，有分歧即停交 human），回滚口径 = `git revert` 逐 commit 或 human 裁；`--no-ff` 双亲合并仅用于 merge-driver 的 dev Pod 通道（该通道产物是 Pod 分支，需保留双亲以证合入事实）。remote push 仍 human；升级闸拦 → 升级人。

> **前端对齐 + 例外反选（260831-gov-review-auto-merge-local-main 起）**：Track B 的 human 触点 = 立项 ack（前端对齐：需求/方向/量级）；merge 恒由主线程在前置升级闸放行后自动落本地 main，remote 同步仍 human。升级闸只重算 merge 期仍可机判的例外（契约面 diff / off-main / escalation 未 resolve / riskTier high-critical / supervisor APPROVE provenance / architect GREEN 机锚）；跨系统结构性变更、声明红队、自改递归属**路由期**裁决，在 merge 前已引走人，不做 merge 期重算。

**制品 4 件**：PROPOSAL + architect-landing-summary + landing-verdict + retro。

**提案目录命名（起草期硬义务）**：governance-lane 目录 = `<YYMMDD>-gov-review-<slug>`，spec-lane 目录 = `<YYMMDD>-arch-review-<slug>`；目录名必须命中 `.claude/scope-map.json` review-agent 写槽 token（`gov-review` / `arch-review`）——缺 token 时评审/核验期制品写入（review-rN.md / landing-verdict.md 等）被 `enforce-scope.mjs` 机械拦截，只剩事后改名绕行（实证：260817-arch-review-s5-dogfood-first-cut R1-F1、260824-gov-review-pod-worktree-launch-infra R1）。旧目录命名（无日期前缀形态等）为历史制品，不回改。

**批次目录生命周期与 GC（human 门控）**：批次目录的合法终态 = landed（合并入 main）；放弃 / 中断的目录无自动出口——机械 census 提供观察面：`node .claude/scripts/changes-index.mjs --stale-report` 列未落地且首提交超阈（默认 30 日）的陈旧目录（只读、永不删除）；**触发点 = 主线程于每轮队列卫生时手动执行**（human 2026-08-26 裁：纯手动——触发机械化是另一个判断，不搭本批，留根见该批 §6）；human 对每个点名目录三选一：删除 / 移 `docs/archive/` 留档 / 标注保留理由。**删除与归档动作恒为 human 批准后主线程执行，机器只点名不动手。**

**机械面枚举的锚点纪律**：canonical 文档（protocol / reference / agent def）以枚举方式描述机械面（闸清单 / 脚本清单 / agent 名册 / 写面）时，必须指向对应注册表锚点（`.claude/verify-gates.json` / `.claude/scripts/` 目录 / `.claude/agents/` 目录 / `.claude/scope-map.json`），不得硬编码手维护名册；注册表变更的批次须同批核对指向它的 prose 锚点是否仍实（判官评审义务项）。

**landing 通道（常态规则，非暂停）**：落地通道 = **主线程 packet-bound landing（landing-manifest 绑定 + landing-verifier 核验）** 一条。「主线程」= 真无 agent_type 的人类主会话；architect 恒纯判官 / 起草、**永不 landing**（其 approved-landing mode 已于 2026-08-02 退役）；review-moderator（Track A 编排）只产证据 + 定稿核验、**不 land governance canonical**（spec profile 低影响设计层落地边界不变）。Track A 触发时落地通道亦同（human 批准 + 定稿核验过后，由主线程执行落地腿并派 landing-verifier）。

**直接 Track A（绕过 Track B）判据**——命中任一即走 full-adversarial：

1. 触 `docs/contracts/**`（直走契约门，既有规则不变——契约门是独立机器，非 Track A 族）；
2. 跨系统结构性变更（多系统 spec 语义 / 拓扑同时动）。

**governance 类变更（含 self-def / review-loop agent defs）不再强制升级**——治理形态由 human 定义，Track B 评审循环的对抗力度即为足额；「声明红队」保留为**自愿逃生门**：human 或 architect 任一方随时可声明启动 Track A，但**非路由义务**。Track A 触发时：路由权归属、Eligibility 判据、四裁断、Lightweight 流程、Governance pass-oracle 与定稿序列各节（均标注「Track A 触发时适用」）生效。

**Track B 内熔断升级**：architect 评审循环 **3 轮未 GREEN** → 升级 human 三选一：① 转 Track A（全套对抗）；② 撤回 / 缩圈提案；③ 换新判官实例再战 1 轮——仍不绿则**自动转 Track A**，不再二次升级。**human 不定轮数上限**：方向由 human 裁，数字由机械规则兜底（R1–R3 = 同判官 resume，上下文连续；Track A 内部轮次不设上限，收敛即停）。

**存疑向严**：判据适用存疑时默认升 human（不猜、不向宽）；主线程起草时须在 PROPOSAL 标注轨道自评 + 理由（沿用「路由权归属」作者不得自路由精神）。

**不动清单（本路由不改）**：契约门（contracts Tier-1 → architect-reviewer 四分类 + human 签字）、`landing-fidelity-check.mjs`、landing-verifier 独立核验、机械强制层（scope-map / hooks / verify-gates）。

### Eligibility 判据（机检硬排除 vs 主观残留 —— 关键拆分）

> **Track A 触发时适用**（默认路由 = Track B 常驻轻路径，见「双轨路由」节）。

准入分两档，由 architect-reviewer 判、review-moderator 独立复判：

**机检硬排除（命中任一即不合格；可路径 / frontmatter / diff 机判，不靠判断）：**
- 触 `docs/contracts/**` 或造 spec-contract drift → `CONTRACT_GATE_REQUIRED`。
- 触 **G1 指令根**（README / AGENTS / CLAUDE / DEV-GOVERNANCE / L0 蓝图）→ `HUMAN_REQUIRED`（E1：G1 恒 full + human）。**执行测出的 bug 走 Track B 快路径（不论 G 几），见「双轨路由」节；本条管新 idea 轨。**
- 触 **G3 强制层**（scope-map / *.settings / verify-gates / hooks / scripts）→ `HUMAN_REQUIRED`（E1：G3 恒 full + human）。**执行测出的 bug 走 Track B 快路径（不论 G 几），见「双轨路由」节；本条管新 idea 轨。**
- 改 agent `tools` / `name` / `model` / write scope / authority 语义 → `HUMAN_REQUIRED`（自改递归）。
- 改 self-def / review-loop agent 定义 → `HUMAN_REQUIRED`（自改递归 + 第五阀）。
- 新增 / 删 / 改任何 human / engineering / merge / contract / review gate → 不合格。
- mixed lane / L0 蓝图 → 不合格。

**主观残留（无法机判，architect-reviewer 不得自证为 OK，默认升 full）：**「small non-semantic clarification」「no cross-system meaning / ownership change」「cheap validation plan exists」「no open ambiguity / deadlock」。落到主观判断即 `ESCALATE_FULL_REVIEW`。lightweight 只吃机检能证「确实小」的部分（典型：G2 reference/command 措辞、link、evidence-shape、路由澄清）。

→ **合格面 = 仅 G2 + 机检硬排除全过 + 主观残留为空**。

### 四裁断（architect-reviewer 返回）

> **Track A 触发时适用**（默认路由 = Track B 常驻轻路径，见「双轨路由」节）。

| 裁断 | 含义 | 下一步 |
|---|---|---|
| `LIGHTWEIGHT_OK` | 确属小、非契约、仅 G2、硬排除全过、主观残留空 | moderator **独立复判** → 通过后由主线程 packet-bound landing |
| `ESCALATE_FULL_REVIEW` | 有架构语义 / 跨系统 / 歧义 / 信心不足 / 落主观残留 | 走 full 对抗 loop |
| `HUMAN_REQUIRED` | 触 G1 / G3 / 自改递归 / 高敏 | human / full governance |
| `CONTRACT_GATE_REQUIRED` | 触 contracts 或 spec-contract drift | 契约门四分类 + 人类契约门 |

### Lightweight 流程 + 制品

> **Track A 触发时适用**（默认路由 = Track B 常驻轻路径，见「双轨路由」节）。

```text
review-moderator: lane 分类 → review-depth 分类
  → 派 architect-reviewer 只读判 eligibility（4 裁断）
  → LIGHTWEIGHT_OK → review-moderator 独立复判 eligibility（非 pass-through；作者≠判官保全；路由权归属见上节）
  → 复判通过 → 主线程 packet-bound landing（landing-manifest 绑定）
  → 记 lightweight 证据
```

制品（落 `delivery/changes/arch/{arch-review,gov-review}-<id>/`）：`lightweight-intake.md`、`architect-reviewer-verdict.md`、`architect-landing-summary.md`（引被批 packet-id，A9 packet-binding，供人类 merge 门 spot-check）、验证输出 / notes。**不得伪造** full-review 制品（`review-charter.md` / `findings.md` / `review-packet.md`）—— full loop 没跑就没有这些。

### 不达判据 → full-adversarial

任一硬排除命中、主观残留非空、或 moderator 复判存疑 → 走下面的 full 对抗 loop。full review 对以下**强制**：高影响架构、跨系统 spec 语义、L0 蓝图、改 authority/owner/writer/approver 语义 / gates / agent tools·write scope / scope-map allow·deny / hooks·settings·scripts / route enforcement 的治理变更、任何 deadlock 或未决分歧。（**以上为 Track A 新 idea 轨；执行测出的 bug 走 Track B 快路径，见「双轨路由」节——其中真设计分叉 / 爆炸半径未揭示 / 无执行记录 → 回 Track A full。**）

## 起草阶段（architect = 作者 subagent，对偶 builder）

输入是 plan/想法时,moderator 派 `architect` 当**起草 subagent** 产候选(spec lane → `docs/requirements.md` + `docs/architecture.md`,**写入不等于冻结**;governance lane → `delivery/changes/arch/gov-review-<id>/PROPOSAL.md`,**不落 canonical**)。输入已是现成 spec/proposal 则跳过起草。

**作者 ≠ 判官(中立性铁律,对偶 dev-loop builder≠verifier/supervisor)**:architect 只产**候选制品**;判据(charter)由 `scenario-author` 独立冻结,攻与裁由 `challenger`/`adjudicator`。architect 不写冻结 charter、不自评可冻结/可批准、不落 governance canonical、自改递归仍须 human;moderator 编排但不代 architect 起草。**路由权同——作者不得自路由（见上节「路由权归属」）。**

## 角色（详见各 agent 文档）

architect(起草 subagent,仅当输入是 plan/想法,产候选不判)· review-moderator(主线程,编排+落地确认改动)· scenario-author(先冻 charter)· challenger(红队,找洞)· defender(架构师,反驳/承认)· adjudicator(中立判官,裁定+产物)· landing-verifier(governance landing 保真核验,只读 codebase,dev-loop verifier 的治理对偶)。**作者(architect)≠ debater ≠ 判官 ≠ landing 核验者**,靠分离保中立(同 dev-loop builder≠verifier≠supervisor;治理 loop 里 architect≈builder、landing-verifier≈verifier、adjudicator≈supervisor+integrator)。

## 辩论内循环

- 每条 finding:challenger ↔ defender ≤ N 轮(默认 2–3)。moderator 主线程驱动,**禁止嵌套派生**(子 agent 不能再派子 agent)。
- **收敛**:defender 承认 → CONFIRMED;或 defender 引具体 spec/contract 原文完整反驳 → DISMISSED。adjudicator 认证。
- **deadlock(吵不清)**:满预算仍争 → adjudicator 打包正反双方最强论点 + 利害 + 各方案代价 + 推荐 → 人类拍板。
- **熔断**:同一 finding 反复 deadlock / 同一 spec deadlock 堆积超阈值 → 升级人类,不外环硬磕(镜像 dev-loop 2 轮 BLOCK 熔断)。

## 五条协议要点

1. **falsifiable-first 排序** — challenger 开局先打可证伪项:spec 内部矛盾 / spec↔contract 漂移 / 跨 spec 矛盾(可机判),再打软领域判断。(S1 grilling 实证:最强开局是 K7 渠道矛盾,不是领域算法。)
2. **级联续问** — finding 是**依赖树**不是平表。每次裁定后 challenger 重新派生下游 finding 并续问,直到该分支穷尽。charter 按**一条主支/主题**播种,不是整 spec 拍平。(S1:删 Teams → 连带 5+ 下游裁定。)
3. **建议即地板** — 高影响的 范围/边界/架构 finding,拍板包把 AI 裁断当**起点**呈现,显式邀请「砍更大」;这类**即便正反方一致也不自动结案**。(S1 Q1:AI 建议「Teams 只留摄取」,人砍成「整体退出」。)
4. **elicitation 提问** — challenger 同时抛开放式知识抽取问题(不只 gotcha),让人注入隐性领域知识。loop 是知识抽取引擎,不替代人。(S1:人注入 QQ/163、AI-native 原则、公司级愿景。)
5. **造概念前先查重** — 提「这缺一个新关注点」前,先 grep 仓库确认它不存在(避免重设计已存在的东西)。

## finding 结案方式（outcome）

| outcome | 含义 |
|---|---|
| FIX-SPEC | CONFIRMED + 明确 spec 文本改法 → moderator 落地(低影响)或入拍板包(高影响) |
| FIX-DEV / FIX-IMPLEMENTATION | CONFIRMED + 需要实现侧修复 → 写入 `dev-loop-handoff.json`，交 dev-loop planner intake；不是靠 review prose 手工转述 |
| DISMISS-WITH-GROUNDING | defender 引 spec/contract 原文完整反驳 → 记理由,关闭 |
| ACCEPT-AS-RISK | 真缺口但有意接受 → 记入 ADR/decision-log,标 owner+相位 |
| HUMAN-RULED | deadlock/高影响 → 人类拍板,裁断回灌 |
| SPAWN-NEW-CONCERN | finding 揭示缺一个内核/公司级关注点 → 登记**延迟占位**(forcing-function 追踪),不在本 spec 过度设计;入 CRQ 账本按域落条目(`docs/crq/{gov,tech,requirement}/`,一条目一文件 + 跑 `crq-render.mjs`)(S1 Inbound Router 即此类) |

## 严重度 → 可冻结性

任何未结的 BLOCKING/SHOULD ⇒ spec 不可冻结(severity 定义见 `arch-review-rubric.md`)。

## 渐进式严格（谁落地 vs 谁升级）

- **低影响** CONFIRMED + 明显 spec 文本改法 → moderator 主线程直接落地 + 记 debate-log。
- **实现侧修复** CONFIRMED → adjudicator/review-moderator 必须生成 `delivery/changes/arch/arch-review-<spec>/dev-loop-handoff.json`，包含 sourceRefs、riskTier、gateProfile、blockingDecisions、suggestedTaskPackages、planningRecommendation、deliveryImpact、acceptanceRefs 与 verificationHints；planner 只消费 handoff 做 TaskPackage 切片。
- **高影响 / 跨系统 / 触契约 / deadlock** → 人类拍板(镜像 supervisor 自动放行 vs ESCALATE + 契约门)。

## 反空过守卫（mutation-check 对偶,非破坏性）

scenario-author 在 charter 里播种 **regression-canaries** = 已知历史 finding(从仓库决策日志/CONTRACT-DECISION-QUEUE/既往 grilling 提炼)。challenger 若没能独立重新派生出来 → 辩论太浅 → RED,换更锋利的 challenger 重跑。**绝不改真 spec 来造洞。**

## Spec↔Contract 一致性保障（铁律）

本 loop 改 spec(docs/,设计层),但 `docs/contracts/**` 是单一事实源、全程冻结(同 dev-loop 的 `permissions.deny` + enforce-scope 钩子),**永不**拥有契约权威 → 挂到 dev-loop 同一个契约门。

1. **冻结**:所有 review agent 不能写 `docs/contracts/**`。
2. **单一门**:对每条 CONFIRMED 的 spec 改动按 `architect-reviewer` 四分类定性 —— `NO_CONTRACT_CHANGE`(纯措辞/阶段/边界,或 spec 漂了改 spec 消除漂移)/ `IMPLEMENTATION_ADAPTER` / `SPEC_CONTRACT_DRIFT` / `CONTRACT_CHANGE_REQUIRED`。后两类 → moderator 主线程派 architect-reviewer → ContractChangePacket → 人类契约门;spec 改动**暂存在 packet 里,不单独落地**。
3. **原子性**:触契约的 spec 改动与契约改动**同一个 PR** 落地(monorepo 原则),不先合 spec;契约未批前 spec 不可冻结。
4. **机械门**:`node .claude/scripts/arch-spec-contract-check.mjs <spec>` —— 不复制 schema(铁律,硬失败);`core.*` 引用必须在 `docs/contracts/**` 解析(硬失败);`<system>.*` 引用未解析记为 advisory(可能是 Tier-2 私有/未契约化事件,challenger 按轴 4 核),`--strict` 下升为硬失败。这是 pass-oracle 第 4 关。

## Pass-oracle（5 关 + handoff 出口全过才可冻结）

1. **轴覆盖** — charter 冻结的每条适用轴都被 challenger 走过。
2. **场景覆盖** — charter 冻结的每个 ATAM + pre-mortem 场景都被走过(见 `arch-scenarios.md`)。
3. **finding 全结** — 每条 finding 落到上面某个 outcome;无未结 BLOCKING/SHOULD。
4. **spec↔contract 机械门绿** — 无 schema 复制 + `core.*` 引用解析(`<system>.*` 未解析为 advisory,Tier-2 交 challenger 按轴 4 核;见 A7 脚本)。
5. **canaries 全中** — 所有 regression-canary 被 challenger 独立重新派生。
6. **dev-loop handoff 完整** — 有实现侧修复 finding 时，`node .claude/scripts/arch-review-handoff-check.mjs delivery/changes/arch/arch-review-<spec>` 必须绿；没有此类 finding 时，review-packet 说明 handoff not required。

## Governance profile pass-oracle（全过才可标「可批准」）

> **Track A 触发时适用**（默认路由 = Track B 常驻轻路径，见「双轨路由」节）。

governance profile(`/review-governance`)不用上面的 spec 冻结 oracle,换成这六关:

1. **治理判据覆盖** — charter 冻结的每条治理判据(authority/owner/writer/approver 一致性、G1/G2/G3 归属、自改递归、agent·command·scope-map·hooks·settings 跨面对齐、业务门/工程门边界、是否触契约)都被 challenger 走过。
2. **finding 全结** — 每条 finding 落到某个 outcome;无未结 BLOCKING/SHOULD。
3. **高敏项已标 human** — 触 agent 定义 / 自改递归 / 强制执行层 / 指令根的 confirmed 项一律标 HUMAN-RULED,写进 decision-packet,不在 loop 内落 canonical。
4. **触契约已转门** — 若治理变更实际触 `docs/contracts/**`,已派 architect-reviewer 转契约门。
5. **canaries 全中** — 治理 regression-canaries(历史治理踩坑)被 challenger 独立重新派生。
6. **decision-packet 完整** — 每条 confirmed 治理改法有:目标文件、改法、影响面、是否触自改递归/契约/强制层、推荐,供 human 批。

governance profile **不复制** spec↔contract 机械门(那是 spec profile 第 4 关);它的机械对偶是「跨治理面一致性」人工核查(agent 定义 ↔ scope-map ↔ command 对齐)。

## Governance profile 定稿 → 冻结 → 核验 → landing（human 批准后 / canonical landing 前）

> **Track A 触发时适用**（默认路由 = Track B 常驻轻路径，见「双轨路由」节）。

governance profile 在 human 批准 decision-packet **之后**、主线程 packet-bound canonical landing **之前**，必须走定稿阶段。补的就是一个真实缺口：confirmed 改法若只留在 `decision-packet.md` / `findings.md`，不回灌 PROPOSAL，则 PROPOSAL 起草后即「孤儿」——停在初稿、可能与批定设计矛盾，而 canonical landing 又绑定 decision-packet，于是**没有任何一步核验「落地的 == 批定的」**（gov-review-proposal-consolidation-gate 修复）。

1. **定稿（consolidate，architect）**：把所有 CONFIRMED 改法（来自 `decision-packet.md` / `findings.md`）+ human 裁决**回灌进 `PROPOSAL.md`**，标 `FROZEN / APPROVED`，并加一段「Review 定稿记录」（review 摘要 + finding/改法映射 + 裁决）。PROPOSAL 成为**单一权威设计**；`decision-packet.md` 退化为「本次 delta 记录」。定稿必产 ```landing-manifest 围栏块（恰 canonical 目标路径集，一行一条）作为回灌的机械组成部分（非可选）——两结构门 fail-loud 消费此块，缺块即格式不合格 RED（authoring 格式契约 SSOT 见 architect-operating-model §5）。architect 只机械回灌**已批**改动，**不夹带新设计、不自冻**（作者≠判官）。
2. **冻结（freeze，human）**：human 盖 FROZEN（版本/sha）。architect 不能自冻——作者≠判官，起草/定稿归 architect，判官是 human。
3. **核验（verify，review-moderator 收口 + 机械门）**：moderator（**非作者**，保 author≠judge）核对 frozen PROPOSAL ↔ decision-packet ↔ findings ↔ human 裁决一致——**无漏改法、无新引入矛盾、canonical 目标逐一匹配**；并跑 `node .claude/scripts/proposal-consolidation-check.mjs delivery/changes/arch/gov-review-<id>`（机器兜底：PROPOSAL 已冻结 + 有定稿段 + decision-packet 每条 canonical 目标都在 frozen PROPOSAL 出现）。脚本是**结构一致性**门，不证语义正确——语义正确是 moderator 人读核验的事，两层互补。核验 RED → 退回 architect 定稿，**不放行 landing**。
4. **landing（主线程）**：核验过后由主线程（无 agent_type 会话）packet-bound canonical landing，`architect-landing-summary.md` 引被批 packet-id + frozen PROPOSAL 版本（A9 packet-binding）。
5. **landing 保真核验（landing-verifier，独立于落地者）**：landing 后由主线程派 `landing-verifier`（治理 loop 的 verifier 对偶，补 builder≠verifier 缺槽）**独立核验「实际落的 == frozen PROPOSAL」**：跑 `node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/gov-review-<id>`（结构门：未冻结 / 无 packet-binding / scope creep / claimed·planned-but-not-landed 硬失败）+ 逐文件人读 diff 核对改法 == PROPOSAL/decision-packet（语义保真，脚本不判），写 `landing-verdict.md`。**verdict 绿才交 human merge，RED 退回主线程修 landing**。landing-verifier 只读 codebase、只写自己 verdict——改了被验文件就塌独立性。补的缺口：此前 landing 只靠 architect 自写 landing-summary「供人类 merge 门 spot-check」，无独立核验（gov-review-landing-fidelity-gate 修复）。

> 这一段是 governance profile 专属（spec profile 的候选直接是 `docs/` 里的 spec，confirmed 改法直接落进 spec，无「孤儿 PROPOSAL」问题）。定稿核验过后 landing 恒 = 主线程 packet-bound（见「双轨路由」节；architect 的 approved-landing mode 已于 2026-08-02 退役）。
