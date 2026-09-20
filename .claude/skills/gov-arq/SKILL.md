---
name: gov-arq
description: "Use when: 把一个已执行测出的治理改进项（bug 或优化建议）写成快路径就绪的 ARQ，让 architect-reviewer 能凭 ARQ 行机判 review depth（Track B 三要件 或 G2 lightweight 机检事实齐全）。触发词: 登记ARQ, gov ARQ, 这个gov bug记一下, register ARQ, fast-path ARQ。边界: 只做执行测出的 gov 改进项 ARQ；新 idea 交 /arch-change，cadence 共性批量交 /loop-retro。"
---

# Gov ARQ：把执行测出的治理改进项写成快路径就绪的 ARQ

把一个**已执行测出的治理改进项**（bug 或优化建议）写成一行信息齐全的 ARQ，登记进 `docs/ARCH-REVIEW-QUEUE.md`。「快路径就绪」= ARQ 行预先携带可机判事实，让 `architect-reviewer` 不必重新推上下文就能裁 review depth——够格就走快路径（**Track B** 执行测出快道，或纯 G2 的 **Track A lightweight**），不够格就**如实**标 `full-adversarial + human`，绝不美化。

本 skill 是**单条、按需**的 ARQ 起草器，与 `/loop-retro`（跨 Pod 周期性批量 retro）互补、字段 convention 完全一致；它不是批量 retro，也不是新 idea 入口。登记进 queue 是**主线程职责**（human-approved 2026-07-28），本 skill 由主线程执行即落该职责。

> **路由注**（2026-08-02 常驻化）：本 skill 的下游 = **Track B 默认路由**（`AGENTS.md`「治理路由」节 + `.claude/references/arch-review-protocol.md`「双轨路由」节）：主线程起草 PROPOSAL → human 立项 ack → architect 评审循环 → human 裁 OQ → FROZEN → packet-bound landing → landing-verifier → retro。命中升级判据（触 `contracts/**` / 跨系统结构性变更 / 声明红队）的交 `/arch-change`（Track A）。步骤 2/3 的三要件与机检硬排除判断**照常写进行**（行内信息完整，Track A 触发时即用）。处理侧配套：PROPOSAL 起草用 `docs/templates/PROPOSAL.template.md`（硬项 · 起草期实证 / sweep 透镜 / 复扫先验 / 目标文件锚清单），landing-summary 用 `docs/templates/architect-landing-summary.template.md`，判官/核验官简报 `.claude/references/track-b-{judge,verifier}-brief.md`。

<HARD-GATE>
没有**至少一条可识别的执行记录**（provenance）前，不起草 gov 改进项 ARQ 行——那是 Track B 的客观载体，非作者声称。拿不出执行记录 → 这不是「执行测出的 gov 改进项」，停：是新 idea 就交 `/arch-change`（cadence 共性批量 retro 的 `/loop-retro` 属档 2 未迁，本仓暂由主线程人工归纳），或先去补执行证据再回来。产物只有 queue 里的一行（或对既例行的证据注记）；其余边界见「约束」节。
</HARD-GATE>

## Checklist（按顺序，一步一 task）

1. **收 provenance（地面真值）** — 找出至少一条可识别执行记录，引具体路径/行：
   - loop-retro 条目（`.claude/loop-retro/loop-retro-*.md` 某行）/ friction 记录；
   - verifier-evidence round（`delivery/taskpackages/*/verifier-evidence/rounds/*.json`）；
   - architect-review / adjudicator finding（`delivery/changes/arch/*/`）；
   - 带执行证据的已登记 ARQ（`docs/ARCH-REVIEW-QUEUE.md` 某行引执行来源）。
   **终点**：一条可点开的执行记录引用。没有 → 走 HARD-GATE 的停法。

2. **判 Track B 三要件**（`.claude/references/arch-review-protocol.md` Track B 段）——逐条 yes/no：
   - ① 执行测出（retro / dev-loop / architect review loop 反馈，有上一步 provenance 承载）？
   - ② 问题清晰、修复明确（**无真设计分叉**）？
   - ③ 爆炸半径已被执行揭示（不是推测，是执行已照出影响面）？
   **终点**：三条全 yes → `review profile = Track B（不论 G 几）`。任一 no → **不是快路径**，`review profile = full-adversarial + human（Track A）`；skill 仍可起草，但 profile 必须写实，不得回标 Track B。

3. **判 G 层 + 机检硬排除事实**（即便走 Track B 也写清，供 architect-reviewer 判；表见 `docs/DEV-GOVERNANCE.md` §6，硬排除清单见 `.claude/agents/architect-reviewer.md`）：
   - **列出要改的确切路径** → 机判 G 层：G1（README/AGENTS/CLAUDE/blueprint/DEV-GOVERNANCE）、G2（`.claude/{agents,commands,references,rules,skills}/**`）、G3（scope-map / *.settings / verify-gates / hooks / scripts）、G4（delivery/changes/** / 队列）。**所列路径即后续 Track B PROPOSAL 硬项 ·「目标文件锚清单」的种子**（`docs/templates/PROPOSAL.template.md`——登记侧写清路径，处理侧直接锚定）。
   - 逐项声明（可机判，不靠判断）：触 `docs/contracts/**` 或造 spec-contract drift？改 agent `tools`/`name`/`model`/write scope/authority 语义？改 self-def（architect.md）或 review-loop agent 定义？新增/删/改任何 human/engineering/merge/contract/review gate？mixed lane / 触 L0 蓝图？
   - **便宜验证计划**：例如复跑 `route-regression-check.mjs` / `lint-descriptions.mjs` / `landing-fidelity-check.mjs`。
   **终点**：G 层标注 + 上述每一项的 yes/no + 验证计划名。触 G1/G3/contracts/gate/self-def/mixed 任一项 → 标 `full-adversarial + human`（这些是 lightweight 机检硬排除；Track B 可吃 G3，但仍须把这些事实写明供判）。

4. **查重（登记前纪律）** — grep `docs/ARCH-REVIEW-QUEUE.md` 找同根项（按 slug/根因关键词）。
   - 有同根已登项 → **在既例行追加证据注记，不新立**（引本次 provenance）。
   - 无 → 按 `ARQ-<YYMMDD>-<english-kebab-slug>` 新立（YYMMDD=创建日；slug=ASCII `[a-z0-9-]`、≤~5 词/~40 字符、不收尾 `-`；人读中文标题留 title 列；同日不重即可）。子项用 `ARQ-<YYMMDD>-<slug>-<子项>`。旧 `ARQ-NNN` 冻结共存、不迁移。
   **终点**：确定「追加注记」还是「新立」+ 目标行 / 新 ID。

5. **填行** — 按 queue 现行 row contract（`docs/ARCH-REVIEW-QUEUE.md` §0 维护规则）：
   - §1.1/§1.2 列：`| ID | 严重度 | title | 未结原因 | 处理时机 | 来源 |`。
   - severity 从五档选（`.claude/references/arch-review-rubric.md`）：`BLOCKING / SHOULD / NIT / OPTIONAL / FYI`。
   - **未结原因格按活 ARQ convention 内嵌**（这是快路径信息的落点，不可省）：
     `**证据**（<provenance ref，步骤1>）…  **影响面**：<爆炸半径，步骤2③>…  **建议改动**（候选，交 review，不自批）：<明确修复，步骤2②>…  **riskTier: low|medium|high**。**review profile**：<Track B（不论G几） | G2 lightweight-eligibility 候选 | full-adversarial+human（Track A）> + <G层 + 机检事实一句话，步骤3>。owner=architect。`
   - 来源格：provenance 链接（loop-retro 行 / round JSON / finding 路径）。
   **终点**：一行 6 列齐全、未结原因格六要素（证据/影响面/建议改动/riskTier/review profile/owner）齐全。

6. **登记** — 写进 `docs/ARCH-REVIEW-QUEUE.md` 对应节：需立即拍板 → §1.1 Blocker；待阶段/触发 → §1.2 Deferred；刻意接受的风险 → §1.3 Accepted-risk（列换为 `| ID | 严重度 | title | 保留原因 | 复议触发 | 来源 |`）。
   **不**改 §0 维护规则、**不**动 §2 已结索引（除非本 skill 之外另行收口）、**不**手写节标题计数「（X 项）」、**不**往行里复制 debate log / review-packet / canary 长证据（只链接 `delivery/changes/arch/**`）。
   **终点**：queue 文件里该行已落盘对应节。

7. **交接下游** — 报告：已登 `ARQ-id` + review profile + 下一步命令。
   **终点**：报告三样齐全（ARQ-id / profile / 下游命令）；下游路由细则见末节。

## 快路径就绪自检（登记前过一遍，缺一即未就绪）

- [ ] provenance：≥1 条可点开的执行记录（非作者声称）。
- [ ] Track B 三要件逐条标注；任一 no 则 profile 已是 `full-adversarial+human`，未回标 Track B。
- [ ] G 层由确切 touched paths 机判得出。
- [ ] 机检硬排除逐项声明（contracts / agent tools·name·model·scope / self-def / review-loop agent def / 任何 gate / mixed lane / L0）——有任一 yes 且未标 full，即矛盾。
- [ ] 便宜验证计划点名。
- [ ] 未结原因格六要素齐全；review profile 写实。
- [ ] 查重已做：同根追加、异根新立；ID 合规。

## 约束（must / never，不可省）

- **MUST**：每条 gov 改进项 ARQ 带至少一条可识别 provenance——Track B 资格的客观载体。
- **MUST**：review profile 写实。三要件任一不满足、或触 G1/G3/contracts/gate/self-def/mixed，就标 `full-adversarial + human`，绝不美化成快路径。
- **MUST**：查重在前；同根在既例行追加证据注记，不新立。
- **MUST**：字段 convention 与 `/loop-retro` 产出的 ARQ 完全一致（证据/影响面/建议改动/riskTier/review profile/owner=architect），保证 queue 内同质。
- **NEVER**：不自批、不开 TaskPackage、不改任何 canonical（`.claude/{agents,commands,references,rules,skills}/**`、`contracts/**`、docs 蓝图与指令根）——本 skill 只写 queue 行 / 既例行注记。
- **NEVER**：不把「新增/改工程门」当快路径塞——新增或修改任何 gate 本身就是 lightweight 机检硬排除，须标 full（或按 Track B 写明 G3 + 该 gate 事实交判）。

## 交接下游（terminal state）

本 skill 的 terminal state = 「ARQ 行登记进 queue」（边界即「约束」节 NEVER，不重复）。按 review profile 交下游：

- **已登记的 ARQ（默认）** → 走 **Track B 默认路由**（主线程起草 PROPOSAL 走起，见文首路由注；PROPOSAL 起草用 `docs/templates/PROPOSAL.template.md`）。
- **命中升级判据（触 `contracts/**` / 跨系统结构性变更）或声明红队** → 交 **`/arch-change`**（Track A：review-moderator 分类 review depth——G2 lightweight 4 裁断 / full-adversarial；architect-reviewer 凭行内机检事实判 `LIGHTWEIGHT_OK / ESCALATE_FULL_REVIEW / HUMAN_REQUIRED / CONTRACT_GATE_REQUIRED`，LIGHTWEIGHT_OK 还须 review-moderator 独立复判〔非 pass-through〕）。
- **provenance 不足 / 其实是新设计 idea** → 不登记，交 **`/arch-change`**（新 idea 走 Track A）。
- **是跨 Pod 周期性共性、应批量 retro** → 交 **`/loop-retro`**（其 triage 第 4 步同样产出 ARQ candidate，字段一致）。
