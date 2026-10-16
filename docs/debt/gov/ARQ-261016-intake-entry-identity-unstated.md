---
id: ARQ-261016-intake-entry-identity-unstated
title: /triage-change 的跨面 intake 入口身份未在任何 canonical 声明——AGENTS.md 入口摘要行未随 260928 批同步、change-intake.md 未自声明入口，读者会得出「triage 只是四面并列入口之一」的错误心智模型
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一治理批（动 AGENTS.md 入口节 / change-intake.md / change-triage.md 任一时并批）；或再出现一次跨面变更被误路由进错入口时立即升格
opened: 2026-10-16
closed: ""
closedBy: ""
paths: [AGENTS.md, .claude/references/change-intake.md]
source: [delivery/intake/261016-p3-story-body-sd-cache.md 的「档位裁定与编排」节（2026-10-16 主线程更正留痕）, 主线程会话 2026-10-16 human 原话「不对，你再去查一下triage跟 intake 之间的关系，你这样设计的话就有个问题：…谁是这个总的编排者呢？」, docs/debt/gov/_archive/2026/ARQ-260928-triage-cross-face-intake-gap.md（前身批；其 paths 未含 AGENTS.md 即为本缺陷的直接实证）]
---

**证据**：

1. **误读事件（2026-10-16，主线程起草会话，活体证据）**：主线程为跨面变更 `261016-p3-story-body-sd-cache` 起草 routing 时，human 问「下一步是应该走 triage 吗？由 triage 去安排改需求、改架构、迭代 loop，是这样吗？」——主线程答**「不是 triage 先行」**，并把 `/triage-change` 描述为「只是实现面的分流前置门，不编排需求面与架构/契约面」，据此把下一步定为 `/requirement-change`。human 当场指出该答有误，主线程复读 `.claude/commands/triage-change.md` + `change-triage.md` 后确认错答并更正（更正留痕见 source 首项）。**误读者 = 主线程（编排者本人），非下游 agent**——连负责编排的一方都会读错入口归属。

2. **权威文本逐字**（含 id 的入口身份，但在读者不会先看的地方）：

   - `.claude/commands/triage-change.md` 首段：「你现在是 **主线程编排者**（人类主线程）」；其步骤 1 = 「**四面探测（intake）**：读 plan/idea，按 `change-intake.md` 程序做跨面探测——触面 ≥2 时主线程落盘 spine」；步骤 6 = 「按 facePlan 顺序打出各面执行命令」。
   - `.claude/references/change-triage.md` §0：「**四面探测（intake）**：变更进来先按 `change-intake.md` 做跨面探测……命中触面事实 → 按 facePlan 路由（架构/契约面走 `/arch-change` loop），实现面留本门分档，**不再整单踢走**。」
   - `.claude/references/change-intake.md` 第 3 行（引用抬头句）：「消费关系：**`/triage-change` 与 change-triage.md 消费本文件做四面探测与分面推进**」。

   即：**入口身份只存在于 triage 侧两个文件 + change-intake.md 的一句抬头**。

3. **读者实际会看的两个地方，都没有这句**：

   - **AGENTS.md**「治理路由」节「**四面入口（每面一个 loop，互为对偶）**」逐字：需求面变更 → `/requirement-change`；架构+契约面变更 → `/arch-change`；治理面变更 → `/gov-change`；**实现面变更 → `/triage-change` 五档分流 → `/build-feature`**。其后另起一段列「跨面 intake：触面 ≥2 的跨面 idea 先过统一 intake 建 spine……」。**读出来的心智模型 = 四个平等的面入口 + 一个独立的跨面路由账本**；四者之一的身份重合（实现面入口同时就是 intake 入口与总编排者）**一字未提**。
   - **`.claude/references/change-intake.md`（intake 程序 SSOT）全文没有「谁是本程序的入口」这句话**——它从「intake 是一个可被消费的程序」的视角写，不绑定入口。

4. **反向加固（同文件内自相矛盾）**：`change-intake.md` §8「开场即消费」逐字：「带 changeId 调用的各面流程入口（**triage 实现面** / requirement-change 各档 / /arch-change / /gov-change / build-feature）先读 spine 的 faceMap/facePlan」——这句把 triage **与其余四个并列成"各面流程入口"**、且标注为「实现面」，字面读法即「五个平等入口」，恰好覆盖掉第 3 行抬头那句的权威。误读者的推理链即由此形成（读 §8 → 认定五入口并列 → 认定 triage 只是一个面）。

5. **本缺陷的来源 = 前身批的镜像同步遗漏（可核）**：`docs/debt/gov/_archive/2026/ARQ-260928-triage-cross-face-intake-gap.md`（已结项，closedBy = `260928-gov-review-cross-face-intake-spine`）**正是**把 `/triage-change` 从「只判实现侧切分大小」改造成「跨面 intake 入口」的那一批（其结项行：「四建议全部落地——建议 1/3/4 = 260928-gov-review-cross-face-intake-spine 批 T1/T2/T4」）。**该条目 `paths` = [.claude/commands/triage-change.md, .claude/references/change-triage.md, .claude/skills/write-story/SKILL.md, .claude/references/task-intake-and-slicing.md]——不含 AGENTS.md**。即：那一批改了 `/triage-change` 的**身份**，却没有同步 AGENTS.md 里描述该身份的**摘要行**。本条目与 `ARQ-260928-…` **不同根**——前者治「机制不存在」（已解），本条治「机制建成后入口身份没写进读者会看的地方」；故新立不重开。

**影响面**：

- **命中面 = 所有跨面变更（触面 ≥2）的第一次路由**。AGENTS.md 是**常驻指令**（每次会话加载、经 CLAUDE.md `@AGENTS.md` 导入），其中的错误心智模型会被每一会话默认采信；而「下一步该进哪个入口」正是跨面变更的**第一个决策点**。
- **代价形状 = 静默错路由**：错认入口不会报错，只会让变更从错误的门进入（本例：把编排入口当第三步、直接跳 `/requirement-change`），后续各面 anchors 与 spine 状态顺着错路径推进。漏检成本高于普通文档缺陷——没有机检齿会拦（triage 是 convention-enforced 入口，见 change-triage.md §9）。
- **复现频率**：每个跨面变更一次；且**编排者本人**即已复现一次（证据 1），说明不是"下游 agent 经验不足"，是文本本身需要推。
- **不修的代价**：凡"要从推得出、而非读到"的入口归属，都会被重新推错——今天由主线程复现，日后由任何执行跨面变更的会话复现。

**建议改动**（候选，交 review、不自批）：

1. **AGENTS.md 入口节显式化身份**（最小改动、收益最大，因其为常驻指令）：把「四面入口」节改写为——`/triage-change` 是**跨面变更的编排入口**（step 1 四面探测 + 落 spine，step 6 按 facePlan 打出各面命令），需求 / 架构+契约 / 治理三面由它**按 facePlan 派发**；「四面」改为「一个编排入口 + 三个被派发面」的表述，或至少补一句限定语。**注意**：只改表述，不改任何门权与流程（各面门照旧各归各裁）。
2. **change-intake.md 自声明入口**：在 §1（触发与比例化）或 §5（发起机制）补一句「本程序的入口 = `/triage-change` step 1」——让 intake 程序对它自己的入口负责，而非让读者去 triage 侧推。
3. **change-intake.md §8 消歧**：把「（triage 实现面 / requirement-change 各档 / …）」这一括注改写为「triage 为编排入口、其余为其按 facePlan 派发面」，消除该句与第 3 行抬头之间的字面矛盾。
4. **（可选）check 齿留根**：本缺陷属"表述缺位"类，无天然机检齿；若本批认为值得，可评估一条轻量自检（如 AGENTS.md 入口节须出现「编排」字样及其指向），否则如实记为 convention-only。

**风险**：riskTier: low——纯文档措辞显式化、不触流程与门权、单批可收；但**触 G1（AGENTS.md = 指令根）**，故虽改动小、影响面广，须按治理面流程走。

**review profile = full-adversarial + human**（gov-arq skill 判据：`paths` 含 AGENTS.md ⇒ GateLayer **G1** ⇒ lightweight 机检硬排除，不得美化快路径；硬排除六项——contracts / agentFrontmatter / selfDef / reviewLoopAgentDef / newGate / specContractDrift——**零命中**，排除依据不在硬排除而在 G1）。Track B 三要件：① 执行测出 **yes**（证据 1 误读事件 + 证据 2-5 可复现文本；且前身批 paths 不含 AGENTS.md 为可核实证）/ ② 问题清晰、修复明确 **yes**（三处显式化，无真设计分叉——表述方案的取舍不构成设计分叉）/ ③ 爆炸半径被执行揭示 **弱**（已观测一例，其余为常驻指令加载面推断）→ **③ 不满足 + G1 命中，profile 恒 full**。

**路由**：归 `/gov-change` loop。按 AGENTS.md「治理路由」节，「机器面〔commands / agents / hooks / scripts / scope-map / settings / **G1**〕**恒 Track B**」——本条目触 G1，故 gov-change 侧的**轻路径不适用**（轻路径判据 = 纯文档面且不在机器面清单内）。（注：此「Track B」= gov-change loop 的路径轴，与上文 gov-arq 的 review profile 轴同名异义，两轴独立、一并如实记录。）

**关联**：前身批 `docs/debt/gov/_archive/2026/ARQ-260928-triage-cross-face-intake-gap.md`（机制建立，本条目承接其镜像同步遗漏）；`260928-gov-review-cross-face-intake-spine`（落地批，change-intake.md 出处）；`ARQ-260928-arch-change-retirement`（同批拆出的相邻条目）；本条目由跨面变更 `261016-p3-story-body-sd-cache`（spine `delivery/intake/261016-p3-story-body-sd-cache.md`）的起草期误读派生。

---

**追加（2026-10-16，同日第二次复现，同一条目，证据升级）**：

**第二次误读**：主线程在**读完** `.claude/commands/triage-change.md` + `change-triage.md` **之后**（即本条目证据 1 那次更正之后），仍把 `/triage-change` 编排为「③ 实现面」——即把**容器当成了链上的一环**。human 2026-10-16 当场第二次指出：「triage 现在就是要把需求、架构跟开发都合在一起，它并不是只看开发的，所以你把它放到第三位是不对的」。

**这一条为什么重要**：证据 1 是「没读定义就下结论」（执行失误）；本条是「**读完了定义仍然读错**」——同一份文本、同一个读者、读全之后仍推导出错误结构。这排除了"读者偷懒"的解释，把成因锁在**文本本身**：

- `change-triage.md` §0 的「**实现面留本门分档**」与 AGENTS.md 的「实现面变更 → `/triage-change` 五档分流 → `/build-feature`」，两处都把 triage 描述成「实现面那一环」；
- 而真正点明它是**容器**的那半句在别处、且极易被跳过——`commands/triage-change.md` step 6：「轻面注明**内联执行（本会话继续派发）**」。**「本会话继续派发」＝ triage 会话在自己里面把各面派出去**，这才是"把需求、架构、开发合在一起编"的机制载体。
- **反向旁证被忽视**：前身条目 `ARQ-260928-triage-cross-face-intake-gap` 的触发原话逐字即「现在 triage **只关注到实现侧的拆包大小，这个不够**」——那句正是在宣告 triage 改造后**不再只是实现面**。误读者读过该条目全篇（本次查重时），仍未吸收。

**对建议改动的加固**：原建议 1/2/3 三处显式化之外，须**再加一处**——

5. **点明 `/triage-change` 的容器属性**：AGENTS.md 入口节与 change-triage.md §0 都应写出「triage = 承载整条跨面链的编排会话，各面在它内部按 facePlan 被派发（轻面本会话内联、重面 headless）」；「实现面留本门分档」应补「**分档是它的收尾动作，不是它等于实现面**」这一限定。**不加这条限定，仅补"入口身份"仍不足以阻止第二种误读**（本例已证）。

**证据强度**：本条目现有**两次独立误读**（同一读者、同一文本、两种不同错法），且第二次发生在完整阅读之后——`severity: SHOULD` 是否应升 `MAJOR`，留待 `/gov-change` 评审时裁；本处按纪律只追加、不改既有字段。
