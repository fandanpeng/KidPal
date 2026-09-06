# PROPOSAL · 260906-gov-review-architect-loop-advisory

> **状态：FROZEN v1.0 · 2026-09-06 · 凭据 = human 立项 ack（「三个问题都同意你的推荐，立项走review」2026-09-06）+ architect R1 GREEN（§7）+ OQ 三题全裁（OQ1=(a) / OQ2=(a) / OQ3=(a)，human 2026-09-06）· R1 四条 NIT 由主线程标 FROZEN 时顺手落字（判官授权，机械修订）**（前端对齐：为什么改 = dev loop 进行中的疑难问题够不到全仓最强档模型（architect = fable），fixer 报「需架构方向」时升级路径只有 human 一档；往哪改 = 三个 agent def + 一份 reference 的 G2 增补，零契约零实现零 G3；改动多大 = 四 T 小件，无新机制、无新闸、无新事件类型，复用既有 dispatch-stamp / pod-events 留痕）。
> **承载 ARQ**：无新承载根——本想法无既有 ARQ 覆盖（2026-09-06 实测 `grep -n "architect" docs/ARCH-REVIEW-QUEUE.md` 命中四段，主题分别为 protocol-version-pinned-constant / lfc-canonical-anchor-gap / escalate-merge-no-human-path / g3-server-live-acceptance，均与本想法无关）；human 2026-09-06 设计讨论直接立项。
> **review 轨**：Track B 常驻轻路径。Track A 判据核查：① 不触 `docs/contracts/**`（manifest 四路径全在 `.claude/` 下，零契约触点）；② 非跨系统结构性变更（纯 G2 角色定义增补，无固件/shim 行为变更）——两判据均否，维持 Track B。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T4）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low——全部触点为 G2 角色/参考定义面；行为保持（architect 恒不 landing、恒不实现，本批只增「只出 advisory 意见」一档新触发）；无新承重闸、无数据迁移、无不可逆操作；机械闸触点仅既有手动验证工具（route-regression-check / lint-descriptions），verify-gates.json 零改动。
> **review profile**：G2——`.claude/agents/planner.md`、`.claude/agents/architect.md`、`.claude/agents/fixer.md`、`.claude/references/architect-operating-model.md`。G1 / G3 / 契约面 / spec 实现面 = 零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行已按硬项 2a 在场。

```landing-manifest
.claude/agents/planner.md
.claude/agents/architect.md
.claude/agents/fixer.md
.claude/references/architect-operating-model.md
```

> manifest 注：4 路径 = T1–T4 §2 目标文件锚并集（一 T 一文件，无共享锚、无新增文件）。

---

## 0. 想实现的效果与差距

**目标效果**：dev loop 进行中遇到疑难问题（fixer 同 fingerprint 多轮失败方向不明、fixer 判断「需架构方向」、planner 自觉棘手的实现分叉、或 human 在 loop 中显式要求）时，planner 能派发 architect（frontmatter 默认 `model: fable`，全仓最高档）出**只读 advisory 分析意见**；意见由 planner 中介进 fixer/builder 的执行链，执行与裁定仍归原 agent（fixer 拥有补丁、verifier 拥有判定），architect 全程不落地、不实现、不改任何验收与门。

**现状差距**：三个断点——① **机械不可达**：planner 的 Agent 派发白名单无 architect（`.claude/agents/planner.md:4`），而 dev loop 正跑在 `claude --agent planner` 主线程身份下，loop 一旦启动即机械上派不出 architect；builder/fixer/integrator 是叶子 agent（frontmatter 无 Agent 工具），升级只能回 planner 或 human。② **路由信号缺失**：architect 的 description 与 When to Use（`.claude/agents/architect.md:3,21-30`）无「疑难分析」入口，自动路由不会命中它。③ **升级路径断**：fixer 的 Red Flag「需要人类判断业务取舍或架构方向」（`.claude/agents/fixer.md:68`）只有 human 一个去向，中间没有「先听最强档分析意见」一档。

**非目标**：① 不让 architect 动手实现或替 builder/fixer/integrator 工作（Boundaries 红线不破，本批只增触发入口）；② 不替 architect-reviewer 的契约预审、不替 review-moderator 的对抗 review（消歧句显式钉死）；③ 不新立事件类型、不扩 pod-events 事件闭集、不改 scope-map、不动 verify-gates.json；④ 不给 builder/integrator/acceptance-author 开直派 architect 的档（失败链经 verifier→fixer 已覆盖，见 §6 留根）；⑤ 不引入机械门——本批全部约束以 prose 钉死，防行为漂移靠 retro 观察项（§6）。

## 1. 问题与根因（地面真值）

**问题类 A：loop 内机械不可达（T1）**。证据（2026-09-06 实测 `grep -n "^tools:" .claude/agents/*.md`）：全仓 14 个 agent def 中**仅两处** tools 行含 `Agent(`——`.claude/agents/planner.md:4`（白名单 = `architect-reviewer, uiux-author, acceptance-author, builder, fixer, integrator`，**无 architect**）与 `.claude/agents/review-moderator.md:4`（白名单含 architect——先例在册，architect 作可派发 subagent 的形态已被 review loop 使用）。builder/fixer/integrator/portfolio-planner 等 12 个 def 的 tools 行均无 Agent 工具（同次 grep 全输出在案）。根因 = dev loop 编排者白名单按「loop 内既定角色集」封闭设计，architect 从未被设想为 loop 内咨询角色；而 planner 自述「务必主线程运行（`claude --agent planner`）——只有主线程能派生子 agent」（planner.md:3,12），故 loop 中连「human 显式指定」都会被白名单机械拦下（真 human 会话无 agent_type 不受白名单限，但那是退出 loop 的形态）。

**问题类 B：路由信号缺失（T2）**。证据（2026-09-06 人读）：architect description（`.claude/agents/architect.md:3`）列的 Use when 场景为系统设计、流程定义、contract gate 路由、spec 初版/修订、ARQ/CDQ/交接、合并治理——无「疑难问题分析」；When to Use（:21-30）八条同状。既有近似物均不在 loop 中：`/triage-change` 前置门是 loop **开工前**的 architect 比例化 review（`.claude/references/dev-loop.md:32`），review-moderator 派 architect 起草是 `/arch-change` **对抗 review 内**的形态。根因 = 能力（fable 档 + 只读分析不触 Boundaries）一直在，缺的只是路由入口。

**问题类 C：升级路径断 + 留痕形态未定（T1/T3）**。证据（2026-09-06 人读 + 实测）：fixer Red Flag（`.claude/agents/fixer.md:68`）「需要人类判断业务取舍或架构方向」的直接去向只有升级 human；pod-events 事件类型闭集（`.claude/scripts/pod-events-append.mjs:28-34`）= `dispatch-completed / agent-escalation / agent-escalation-resolved / human-decision / human-correction / friction` 六件，无「分析意见」专用类型。根因 = 既有机制里 advisory 类输入靠 dispatch prompt 人际携带（先例：verifier 给 fixer 的「一句根因假设」，fixer.md:22-23），但 architect 档的派发-留痕链路从未被定义。

**设计定型（human 2026-09-06 设计讨论裁）**：architect 只出 advisory 意见、执行归原链。理由三条，均对齐既有铁律而非新增原则：① author ≠ judge（architect.md:66）+ 机械验证链不腐蚀——执行必落 diff + 过 verify.mjs，意见不豁免任何门；② 与 verifier→fixer「根因假设」dispatch 契约同构——architect 意见 = 更高档的根因假设输入，fixer 照样拥有补丁；③ 模型经济学——意见是有界输出、执行是无界循环，fable 用在密度低杠杆高处，与 planner MODEL-TIER 纪律（planner.md:57）同思路。**效力钉死**：意见为 advisory，fixer 可拿证据反驳；意见与冻结验收冲突时以验收为判分标准；意见不改 TaskPackage 事实、不改验收、不改门——若 architect 认为验收/任务本身有问题，走既有升级（planner Red Flag / needs-triage），不就地修。

## 2. 改法（T1…T4）

### T1 · planner 开 architect 疑难分析档（planner.md）

**改法**：① frontmatter tools 行白名单加 `architect`——由 `Agent(architect-reviewer, uiux-author, acceptance-author, builder, fixer, integrator)` 改为 `Agent(architect, architect-reviewer, uiux-author, acceptance-author, builder, fixer, integrator)`（**逐字段隔离**：只动 tools 行内白名单，`name` / `model` / `description` 三字段零触碰，防误伤 enforce-scope 匹配与路由信号）；② Process 节新增一段（位置 = Step 10 内循环段之后、Step 11 integrator 之前），段头钉死 = **`[ARCH-ADVISORY 疑难分析档]`**，内容：

- **触发时机（闭集三 + human）**：(a) fixer 同一 fingerprint 多轮失败且方向不明；(b) fixer 报「需架构方向 / 业务取舍」类升级（fixer Red Flag 的架构向子集，不替代 human 门）；(c) planner 自觉实现分叉疑难、继续盲烧 fixer 预算前；(d) human 在 loop 中显式要求。**未命中闭集不派**（缺完美疑难断言不是触发条件）。
- **派发契约**：派 architect（其 frontmatter `model: fable` 默认档生效，**派发不带 model 覆盖**）；dispatch prompt 必含——当前 ErrorEnvelope / verifier evidence 摘要、TaskPackage 边界与相关 acceptanceRefs、疑难点一句钉死、本段效力声明原文；architect 只读、只回 advisory 意见文本，**不落盘、不改任何文件**。
- **中介与留痕（复用既有机制，零 G3 变更）**：意见由 planner 携带进下一轮 fixer（或 builder）dispatch prompt 当根因假设输入；派发事实由既有 `[DISPATCH-STAMP]` 纪律（planner.md:58，写 `dispatch-completed` 事件）自动留痕；planner 判断意见实质构成 blocker 时，才按既有 ESCALATION-BRIDGE（planner.md:66）落 `agent-escalation`——advisory 档常规不落 escalation 事件，防语义滥用。
- **效力声明（原文钉死）**：「architect 意见为 advisory 输入：不豁免任何门、不改验收与 TaskPackage 事实、不构成通过判据；fixer 可拿证据反驳；意见与冻结验收冲突时以验收为准。」

**为什么这样改**：直接拆 §1 问题类 A（白名单机械不可达）与问题类 C（留痕用既有 dispatch-stamp，不新立事件类型）；消歧与效力用 prose 钉死而非机械门，因本批 riskTier low 且防「意见被实践当权威」属行为观察面（retro 核对，§6）。
目标文件：`.claude/agents/planner.md`（改：tools 行 + Process 新增 `[ARCH-ADVISORY 疑难分析档]` 段）。

### T2 · architect 补「疑难分析」路由入口 + 消歧（architect.md）

**改法**：① description 的 Use when 枚举**句内追加**一项（保持既有枚举与防误路由消歧句不动，逐字段隔离）：在「ADR/rationale」后插入「dev-loop 内疑难问题 advisory 分析（planner 派或 human 显式；只出意见、不执行、不替契约预审/对抗 review）」；② When to Use 列表新增一条：「**dev loop 中的疑难问题 advisory 分析**：planner 按 `[ARCH-ADVISORY 疑难分析档]` 派你，或 human 显式指定。你只读相关 evidence / spec / contract / 实现，回传 advisory 分析意见文本（根因假设、方向取舍、风险点），**只出意见、不落地、不实现、不改任何文件**；意见为 advisory，不豁免任何门、不构成通过判据；实现变更若需要，转 planner + TaskPackage，不就地动手。」；③ Boundaries 节**不新增条目**（既有「不实现固件/shim 代码」「不绕过 dev-loop」「恒不 landing」已全覆盖），只在 When to Use 条内自带消歧（见②引文）。**本 T 触自改递归面**（architect.md:53——修改自身定义须显式 human-approved、不得自批）：由 OQ1 human 立项裁夺承载，Track B human ack 即显式批准载体。

**为什么这样改**：拆 §1 问题类 B（路由信号缺失）；description 增补保持短枚举形态（authoring 纪律：只写 when-to-use + 消歧，不搬运 workflow 正文）；消歧句显式划开 architect-reviewer（契约预审）与 review-moderator（对抗 review）的既有地盘。
目标文件：`.claude/agents/architect.md`（改：description 行 + When to Use 新增一条）。

### T3 · fixer 升级路径补 architect 档（fixer.md）

**改法**：Red Flags 节「需要人类判断业务取舍或架构方向。」（fixer.md:68）句内扩写为：「需要人类判断业务取舍或架构方向——**架构方向类先由 planner 按 `[ARCH-ADVISORY 疑难分析档]` 派 architect 出 advisory 意见；意见拿不住或涉业务取舍时仍升级 human**。」其余 Red Flag 条目零触碰。

**为什么这样改**：拆 §1 问题类 C 的升级路径断点——fixer 报「需架构方向」不再只有 human 一档，中间插入「听最强档分析意见」一跳；但业务取舍仍归 human，advisory 档不构成 human 门的替代。只改一句，不动 fixer 职责/工具面结构。
目标文件：`.claude/agents/fixer.md`（改：Red Flags 节一行扩写）。

### T4 · operating-model §3 路由表对齐（architect-operating-model.md）

**改法**：§3 Core Role Routing 表 Architect 行的 Owns 列（`architect-operating-model.md:56`）追加一项：「in-dev-loop advisory analysis（opinions only; execution stays with fixer/builder）」；该行 Does not own 列零触碰（既有「App implementation, contract approval, merge approval, all canonical landing」已完备）。其余章节零触碰。

**为什么这样改**：architect 开工必读此文件（architect.md:14-19），§3 路由表是角色职责单源——新职责只写 architect.md 而不更新此表，下一轮 architect 会话读到的是漂移表；一格对齐是最便宜的防漂移。不扩 §8（Dev-Loop Governance To Preserve）——该节保的是 dev-loop 既有治理不变量，本批恰恰不动那些不变量。
目标文件：`.claude/references/architect-operating-model.md`（改：§3 表 Architect 行 Owns 格）。

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G2 角色定义 | `.claude/agents/planner.md`（T1） | 改 | 消费方 = 全部 planner 派生实例（每个 dev-loop Pod 的编排者）；tools 白名单扩张一成员 + Process 新增一段；派发面扩张属**授权向**（多一个可派对象），不缩任何既有权限 |
| G2 角色定义 | `.claude/agents/architect.md`（T2） | 改 | 消费方 = architect 派生实例 + orchestrator 路由判分；description/When to Use 增补入口；**自改递归面，OQ1 human 裁**；消歧句护 architect-reviewer / review-moderator 地盘 |
| G2 角色定义 | `.claude/agents/fixer.md`（T3） | 改 | 消费方 = fixer 派生实例；Red Flag 一行扩写，升级路径多一档、human 门不被替代 |
| G2 参考定义 | `.claude/references/architect-operating-model.md`（T4） | 改 | 消费方 = architect/planner/主线程的开工必读面；§3 表一格对齐防职责漂移 |
| G3 强制执行层 | — | 未触 | verify-gates.json / scope-map.json / hooks / scripts 零改动；lint / route-regression-check 是手动验证工具、本次仅作验证集运行对象 |
| G1 / 契约面 / spec 实现面 | — | 未触 | 零触点 |

**sweep 透镜义务核对**：本批**新立判定 pattern 数 = 0**——无新门、无新正则、无新脚本；四个触点全是既有文件的 prose/白名单修订。受影响集合机械枚举 = manifest 四件（T1–T4 一一对应，无共享锚）。pod-events 事件闭集、dispatch-stamp 脚本、MODEL-TIER 段均为**消费**不**修改**（T1 文本引用其机制）。

## 4. human 取舍点（OQ）

**OQ1 · architect.md 自改递归承载**：human 2026-09-06 裁 **(a)** 本批含 architect.md 自改、立项 ack 即显式批准载体（architect.md:53 要求的「显式 human-approved」由本 ack 承载）；(b) 拆两批；(c) 不落 architect.md（想法残废，不推荐）为落选项。

**OQ2 · advisory 意见留痕形态**：human 2026-09-06 裁 **(a)** planner 中介：意见进下一轮 fixer/builder dispatch prompt + 既有 dispatch-stamp 自动留痕，architect 全程零落盘（零 scope-map 变更、零新事件类型、author≠judge 最干净）；(b) architect 自写分析文件落 taskpackage 目录（须扩 scope-map + 新增路径治理，本批不配比）为落选项。

**OQ3 · 触发通道范围**：human 2026-09-06 裁 **(a)** 双通道——planner 自主判（闭集三触发）+ human 显式指定（触发闭集机械可判、每派必留 dispatch-stamp 痕可 retro 审计）；(b) 仅 human 显式指定首跑（痛点只解一半）为落选项。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**landing 前（起草期基线已实测，2026-09-06；landing 后复跑比对）**：

1. `node .claude/scripts/route-regression-check.mjs` → exit 0、`0 fail`（基线实测 = `28 ok, 0 fail`）；白名单扩张不得回退既有路由信号。
2. `node .claude/scripts/route-regression-check.mjs --self-check` → 绿（必留信号可证伪机制自证不退化）。
3. `node .claude/scripts/lint-descriptions.mjs` → 无新增 flag（基线实测 = 仅 landing-verifier.md over-length 一条既有 warn；architect.md 本就 excluded，T2 改 description 不引入 lint 面变化）。
4. `grep -c "Agent(architect," .claude/agents/planner.md` → 1（T1 白名单恰加一处）。
5. `grep -c "^model: opus$" .claude/agents/planner.md && grep -c "^name: planner$" .claude/agents/planner.md` → 均 1（frontmatter 逐字段隔离，model/name 零误伤）。
6. 消歧短语 census（T1/T2 引文钉死短语逐字比对）：`grep -c "advisory" .claude/agents/planner.md` ≥7（基线 4 + T1 新段新增 ≥3，判官 F4 精确实测口径）、`grep -c "ARCH-ADVISORY" .claude/agents/planner.md` ≥1、`grep -c "advisory" .claude/agents/architect.md` ≥1、`grep -c "ARCH-ADVISORY" .claude/agents/fixer.md` ≥1。
7. 结构门：`node .claude/scripts/landing-fidelity-check.mjs`（本 change）→ 绿（manifest 4 行 ≡ T1–T4 锚并集）。

**landing 后（核验侧）**：

8. 人读 diff 四件：planner.md 只动 tools 行 + 新增 `[ARCH-ADVISORY 疑难分析档]` 段；architect.md 只动 description 行 + When to Use 新增一条；fixer.md 只动 Red Flags 一行扩写；architect-operating-model.md 只动 §3 表 Architect 行 Owns 格。
9. Track A 判据复核：manifest 四路径全 `.claude/` 面，零 `docs/contracts/**`、零实现面、零 G3——机械枚举断言。
10. 空白名单语义冒烟：人读 T1 新段确认未给 planner 自身新增写面、未动合并门/契约门链条任何一环。

## 6. 留根清单

**不关的根（登记不硬治 / 归口他处）**：

- builder / integrator / acceptance-author 不开 architect 直派档——失败链经 verifier→fixer 已覆盖（fixer 档是 builder 失败的自然汇聚点）；若 retro 发现 integrator 收口期也有疑难分析需求，下批再评。
- pod-events 事件闭集未扩（T1 走既有 dispatch-stamp + 条件性 agent-escalation）；若 retro 发现 advisory 派发成高频且审计诉求强，再评估专用 eventType（届时触 G3 另批）。
- MODEL-TIER 与 fable 默认档联动观察项：dispatch-stamp 已记实际生效模型，retro 核对 architect 档是否被滥用（派发行数/疑难命中率），防「凡难就派 fable」烧档。
- ARQ-260906-lfc-canonical-anchor-gap（CANONICAL_TEST 不识仓根点文件 / `changes/**`）：本批四落点全 `.claude/` 面、机械门可识，不踩缺口，无需整裁腿。

**交日落复盘项**：retro 核对此机制首次实际使用（预计包 2 或其后 dev-loop 包）是否满足「意见 advisory 不腐蚀验证链」——具体看有无「architect 说了所以算过」的实践变形；若出现，下批考虑机械提示（如 fixer dispatch prompt 强制带 advisory 头）。

**自律件（不进 landing）**：planner 首次实际派 architect 档时，pod-retro 记一笔使用体感（触发是否准、意见是否有用、是否省 fixer 轮）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 2026-09-06 · architect 判官 · 结论：GREEN

评审范围：硬项 2a 锚清单逐比对 + 硬项 1a 证据断言逐项亲跑亲读 + §5 验证集逐条亲核（含真空绿排查）+ 改法技术合理性（含 T1 插段行号漂移专项）+ Track A 判据独立复核 + Track B 三要件与机检硬排除 + §6 留根对照全队列。以下实证均为本判官 2026-09-06 亲跑/亲读。

**透明披露**：T2 触本判官自身定义文件（architect.md）——自改递归面由 OQ1 human 2026-09-06 立项裁夺承载（ack 即显式批准载体），判官按既有 Track B 职责评审落字一致性与内部自洽，OQ 选择本身不在评审范围（派遣方背景已钉）。

**硬项 2a（锚清单 ≡ manifest）——通过**。T1→`.claude/agents/planner.md`、T2→`.claude/agents/architect.md`、T3→`.claude/agents/fixer.md`、T4→`.claude/references/architect-operating-model.md`，一 T 一文件、无共享锚、无新增文件；landing-manifest 围栏块（PROPOSAL:12-17）四行与并集逐一相等 ✓。packet-id 行（PROPOSAL:6）与围栏块在场，结构字段硬条款满足 ✓。

**硬项 1a（证据断言亲核）**：

- ① 亲跑 `grep -n "^tools:" .claude/agents/*.md`：14 个 def（`ls | wc -l` = 14 ✓），tools 行含 `Agent(` 恰两处——planner.md:4 白名单 `architect-reviewer, uiux-author, acceptance-author, builder, fixer, integrator`（无 architect ✓）与 review-moderator.md:4（白名单含 architect，先例在册 ✓）。planner.md:3「务必主线程运行（claude --agent planner）——只有主线程能派生子 agent。」与 planner.md:12「你必须作为主线程运行……子 agent 不能再派生子 agent」逐字在案 ✓。
- ② architect.md:3 description 的 Use when 枚举与 When to Use（:21-30）亲读：无「疑难分析」入口 ✓（条目数见 F2）。
- ③ fixer.md:68 逐字 = 「需要人类判断业务取舍或架构方向。」✓；`.claude/scripts/pod-events-append.mjs:28-34` 亲读：`VALID_EVENT_TYPES` 闭集恰六件（dispatch-completed / agent-escalation / agent-escalation-resolved / human-decision / human-correction / friction），:47-48 拒绝非法 type ✓。
- ④ planner.md:57 `[MODEL-TIER`、:58 `[DISPATCH-STAMP]`、:66 `[ESCALATION-BRIDGE]`、fixer.md:22-23「verifier 的一句根因假设」、architect.md:53 自改递归段、architect.md:66「作者≠判官」、dev-loop.md:32「分流门衔接……开工前先过 `/triage-change` 前置门」——行号与引文逐字命中 ✓。architect-operating-model.md 锚见 F3。

**§5 验证集亲核**：1 亲跑 `route-regression-check.mjs` = `28 ok, 0 fail` exit 0 ✓（基线与 PROPOSAL 一致）。2 亲跑 `--self-check` = `56 ok, 0 fail`、`all required signals/whitelists falsifiable` ✓。3 亲跑 `lint-descriptions.mjs` = `scanned 13, excluded 1 (architect.md), flagged 1`，唯一 flag = landing-verifier.md over-length ✓（architect.md 硬排除在案，T2 不引入 lint 面变化 ✓）。4 模式 `Agent(architect,` 精度亲核：逗号消歧——现 tools 行 `Agent(architect-reviewer,` 不被命中，基线 `grep -c` = 0、landing 后预期恰 1，非真空 ✓。5 `^model: opus$` / `^name: planner$` 行锚定模式精确，基线各 = 1；planner.md:57 内联 `` `model: opus` `` 非行首不误命中 ✓；逐字段隔离（name/model/description 零触碰）由此二式 + 人读 diff 机判可行 ✓。6  census 三条非真空（`ARCH-ADVISORY` planner.md 基线 0、`advisory` architect.md 基线 0、`ARCH-ADVISORY` fixer.md 基线 0），planner.md `advisory ≥4` 一条真空，见 F4。7 landing-fidelity-check.mjs 在册、`CANONICAL_TEST`（governance-gate-shared.mjs:33）认 `.claude/` 前缀，本批四落点均可机锚 ✓。8-10 人读/复核项与改法自洽 ✓。

**改法技术合理性**：

- T1 插段行号漂移专项（评审清单点查项）：插入点 = Step 10 段（planner.md:62-70）之后、Step 11（:71）之前；T1 新段自引锚 `planner.md:58` / `planner.md:66` 均**先于**插入点，落地后不失锚 ✓；全仓 `grep -rn "planner\.md:" .claude/ docs/` 零行号级外部引用，无第二失锚面 ✓——本项判**无需改法显式处理**，实测结论留痕。
- T2 description 增补符 authoring 纪律：短枚举句内追加（插点「ADR/rationale」在 architect.md:3 逐字在案 ✓）+ 自带防误路由消歧（划开 architect-reviewer 契约预审 / review-moderator 对抗 review）+ 不搬运 workflow 正文 ✓。
- T3 只扩写 fixer.md:68 一句，human 门不被替代（「业务取舍仍升级 human」钉死）✓。
- T4 与 architect.md:14-19 开工必读清单一致（operating-model 为必读件，:17 在案），§3 表一格对齐是最小防漂移 ✓；Does not own 列引文「App implementation, contract approval, merge approval, all canonical landing」与 :56 逐字一致 ✓。
- 效力声明四钉（不豁免门 / 不改验收与 TaskPackage 事实 / 不构成通过判据 / 可拿证据反驳）与 author≠judge、验证链不腐蚀铁律同构 ✓；零新事件类型、零 scope-map 变更、零 verify-gates 触点与非目标声明一致 ✓。

**Track A 判据独立复核**：manifest 四路径全 `.claude/` 面，零 `docs/contracts/**`、零 G3（verify-gates.json / scope-map / hooks / scripts 零改动，两脚本仅作验证运行对象）、零实现面、非跨系统结构性变更——两判据均否成立，Track B 维持 ✓。

**Track B 三要件 + 机检硬排除**（同款 260906 批 R1）：三要件全 yes（human 立项 ack 2026-09-06 在凭 + OQ 三题全裁记录一致；PROPOSAL §0–§7 骨架齐备；landing-manifest 围栏块 + packet-id 行在场）。机检硬排除逐项 no：不触 contracts、不改 verify-gates 门语义、非 mixed lane、不触 L0、无 gate 新增删改、无新事件类型 ✓。

**§6 留根对照**：亲读 `docs/ARCH-REVIEW-QUEUE.md`——ARQ-260906-lfc-canonical-anchor-gap 在册（queue:37），本批四落点全 `.claude/` 面、不踩缺口 ✓；builder/integrator 不开直派档、pod-events 闭集未扩、MODEL-TIER/fable 烧档 retro 观察项与队列实况无冲突 ✓。§0「承载 ARQ」行的覆盖核查方向正确（命中行主题均与本想法无关），惟计数见 F1。

**Findings**：

- **F1（NIT）** §0 承载 ARQ 行「实测 `grep -n "architect" docs/ARCH-REVIEW-QUEUE.md` 命中五段」——本判官亲跑同式与 `grep -in` 均 = **4 行**（queue:35 protocol-version-pinned-constant / :37 lfc-canonical-anchor-gap / :64 escalate-merge-no-human-path / :78 g3-server-live-acceptance），与 PROPOSAL 枚举四主题一一对应，实质结论「无既有 ARQ 覆盖」不受影响。修订：「五段」→「四段」（删「等」或保留均可，数字须按实测）。
- **F2（NIT）** §1 问题类 B「When to Use（:21-30）十条同状」——实测条目 = **8 条**（architect.md:23-30；:21 为节头、:22 空行）。修订：「十条」→「八条」。
- **F3（NIT）** T4 锚「architect-operating-model.md:54」——:54 为 §3 表头行（`| Role | Owns | Does not own |`），Architect 行 = **:56**。修订：「:54」→「:56」。
- **F4（NIT）** §5 验证 6 `grep -c "advisory" .claude/agents/planner.md` ≥4 为**真空绿**——基线实测已 = 4（planner.md:64/66/73/82 既有 advisory 行），不改一个字也通过。修订：阈值升 **≥7**（基线 4 + T1 新段预期新增 ≥3 行：派发契约「只回 advisory 意见文本」、中介留痕「advisory 档常规不落 escalation」、效力声明「architect 意见为 advisory 输入」；注意 `ARCH-ADVISORY` 大写不被小写 `advisory` 模式命中），landing 后复跑比对。

**结论：GREEN**。四条 NIT 均为 verbatim 证据计数/锚位/阈值精度类，不触目标效果与改法设计；照 260906 批 R2-F13/F14/F15 同款先例，**授权主线程在标 FROZEN 时顺手机械落字 F1–F4**（五→四、十→八、54→56、≥4→≥7，其余字不动），落字后无需再审一轮。

**后续序列指引**：主线程凭「立项 ack + 本 GREEN」标 FROZEN（同步落 F1–F4）→ upsert changes-index → packet-bound 落地四件 → landing-verifier 独立核验（结构门 + 人读 diff + §5 复跑，注意 F4 修订后阈值）→ retro → 前置升级闸 → 合并本地 main。
