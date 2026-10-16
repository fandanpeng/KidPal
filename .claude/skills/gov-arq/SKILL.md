---
name: gov-arq
description: "Use when: 把一个已执行测出的治理改进项（bug 或优化建议）写成快路径就绪的 debt 条目，让 architect-reviewer 能凭条目机判轻/主判档与 Track A 段升级（判据可机判、事实齐全）。触发词: 登记ARQ, gov ARQ, 这个gov bug记一下, register ARQ, fast-path ARQ。边界: 只做执行测出的改进项登记（按域落条目，含技术面与改需求请求）；新 idea 交 /arch-change，cadence 共性批量交 /loop-retro。"
---

# Gov ARQ：把执行测出的改进项写成快路径就绪的 debt 条目

把一个**已执行测出的改进项**（bug 或优化建议）写成一份信息齐全的**条目文件**，按域登记进 debt 账本 `docs/debt/<域>/<ID>.md`。「快路径就绪」= 条目预先携带可机判事实（`paths[]` 供机推 G 层与硬排除、三要件写在正文），让 `architect-reviewer` 不必重新推上下文就能裁 review depth——够格就走轻/主判档（判据见 references/gov-change.md），不够格就**如实**标 `full-adversarial + human`，绝不美化。

本 skill 是**单条、按需**的条目起草器，与 `/loop-retro`（跨 Pod 周期性批量 retro）互补、字段 convention 完全一致；它不是批量 retro，也不是新 idea 入口。登记进账本是**主线程职责**（human-approved 2026-07-28），本 skill 由主线程执行即落该职责。

> **路由注**：本 skill 的下游 = **Track B 默认路由**（`AGENTS.md`「治理路由」节 + `.claude/references/arch-review-protocol.md`「双轨路由」节）：主线程派 architect 起草 PROPOSAL → human 立项 ack → architect-reviewer 评审循环 → human 裁 OQ → FROZEN → packet-bound landing → landing-verifier → retro。命中升级判据（跨系统结构性 / 方向未收敛 / 声明红队）的升所属面 loop Track A 段。步骤 3 的三要件与机检硬排除判断**照常写进条目**（信息完整，Track A 触发时即用）。处理侧配套：PROPOSAL 起草用 `docs/templates/PROPOSAL.template.md`，landing-summary 用 `docs/templates/architect-landing-summary.template.md`，判官/核验官简报 `.claude/references/track-b-{judge,verifier}-brief.md`。

<HARD-GATE>
没有**至少一条可识别的执行记录**（provenance）前，不起草条目——那是 Track B 的客观载体，非作者声称。拿不出执行记录 → 这不是「执行测出的改进项」，停：是新 idea 就交面 loop（`/arch-change` / `/gov-change`），或先去补执行证据再回来。产物只有账本里的**一个条目文件**（或对既有条目的证据注记）；其余边界见「约束」节。
</HARD-GATE>

## Checklist（按顺序，一步一 task）

1. **收 provenance（地面真值）** — 找出至少一条可识别执行记录，引具体路径/行：
   - loop-retro 条目（`.claude/loop-retro/loop-retro-*.md` 某行）/ friction 记录；
   - verifier-evidence round（`delivery/taskpackages/*/verifier-evidence/rounds/*.json`）；
   - architect-review / adjudicator finding（`delivery/review/*/`）；
   - 带执行证据的已登记条目（`docs/debt/*/*.md` 引执行来源）。
   **终点**：一条可点开的执行记录引用。没有 → 走 HARD-GATE 的停法。

2. **定域（三值，判据 = 谁裁 / 落哪个门）** — 见 `docs/templates/debt-item.template.md` 的域表：
   - `gov`（治理机制面：agent / skill / 门 / 脚本 / 纪律文面；裁定人 = architect，落本账 Track B/A 批次）；
   - `tech`（技术相关，**含 contract**：固件 / 工具 / 部署运维 / 契约 / spec 文档；裁定人 = 各面 owner，去向填 `handoff`）；
   - `requirement`（改需求的**请求**与裁定状态；裁定人 = human，落地 = `docs/requirements.md` §9 记账）。
   **终点**：域已定，且 ID 前缀与域一致（`ARQ-` / `TECH-` / `REQ-`；**祖辈前缀 `ARQ-` 落任何域都放行**，只出告警计数）。

3. **判 Track B 三要件**（`arch-review-protocol.md` Track B 段）——逐条 yes/no：
   - ① 执行测出（有上一步 provenance 承载）？
   - ② 问题清晰、修复明确（**无真设计分叉**）？
   - ③ 爆炸半径已被执行揭示（不是推测）？
   **终点**：三条全 yes → `review profile = Track B（不论 G 几）`。任一 no → **不是轻/主可机判形态**，标 `Track A 段 + human`；skill 仍可起草，但 profile 必须写实，不得回标 Track B。

4. **填 `paths[]` 与机检硬排除事实**（即便走 Track B 也写清，供 architect-reviewer 判）：
   - **列出要改的确切仓根相对路径** → 写进 frontmatter 的 `paths[]`。**G 层与硬排除结论由 `node .claude/scripts/debt-doctor.mjs` 从路径机推**——不再手写六个布尔（这是旧形态 6 495 字符格子的主要体积来源）。
   - 若要自报 G 层（可选字段 `gateLayers`），**必须与机推一致**，不一致 doctor 报红。
   - 手感对照（机推规则见 `.claude/scripts/debt-ledger-shared.mjs` 的 `GATE_LAYER_RULES`）：`.claude/{agents,commands,references,rules,skills}/` = G2；`.claude/{scope-map.json,hooks/,scripts/,*.settings.json}` = G3；`AGENTS.md`/`CLAUDE.md`/`README.md`/`docs/DEV-GOVERNANCE.md` = G1；`delivery/{changes,taskpackages,workstreams}/` = G4。
   - **便宜验证计划**：例如复跑 `route-regression-check.mjs` / `lint-descriptions.mjs` / `landing-fidelity-check.mjs`。
   **终点**：`paths[]` 已列全 + 验证计划点名 + 触 G1/G3/契约面/gate/self-def/mixed 者 profile 已标 full。

5. **查重（登记前纪律）** — `grep -l "<根因关键词>" docs/debt/*/*.md` 找同根项。
   - 有同根已登项 → **在既有条目正文尾部追加证据注记，不新立**（引本次 provenance）。
   - 无 → 新立。**ID 格式** = `<前缀>-<YYMMDD 创建日>-<english-kebab-slug>`（slug：ASCII `[a-z0-9-]`、≤~5 词/~40 字符、不收尾 `-`）；**文件名必须与 `id` 相同**。同日不重即可。旧 `ARQ-NNN` 冻结共存、不迁移。
   **终点**：确定「追加注记」还是「新立」+ 目标条目路径 / 新 ID。

6. **写条目** — 复制 `docs/templates/debt-item.template.md` 为 `docs/debt/<域>/<ID>.md`，填 frontmatter + 四段正文：
   - **frontmatter 必填**：`id` / `title` / `domain` / `status` / `severity` / `opened`（已结项豁免 `severity`）；建议填 `owner` / `trigger` / `paths` / `source`。**`title` 是 INDEX 表格的 title 列来源，漏填即 doctor 红（missing-field）——与 lib `REQUIRED_FIELDS` 同集，改一处必改另一处。**
   - `severity` 从**七值并集**选（`BLOCKING / MAJOR / SHOULD / COULD / NIT / OPTIONAL / FYI`）——**注意**：其中 `MAJOR` 与 `COULD` 不在 `.claude/references/arch-review-rubric.md` 的五档内，用了会出 advisory（队列历史用词的漂移在册，见 `/loop-retro` 与队列留根）；**新条目请尽量用 rubric 五档**。
   - **正文四段**（快路径信息的落点，不可省）：`**证据**`（provenance，步骤 1）/ `**影响面**`（爆炸半径，步骤 3③）/ `**建议改动**`（候选，交 review、不自批，步骤 3②）/ `**风险**`（含 `riskTier: low|medium|high` 一句话证成 + `review profile` + owner）。
   - **正文只追加、不改既有段**（`AGENTS.md` Hard Rules）。
   **终点**：条目文件落盘，frontmatter 与四段齐全。

7. **渲染 + 自检 + 交接下游** — 跑 `node .claude/scripts/debt-render.mjs`（三份 `INDEX.md` 是**生成件，勿手改**）→ `node .claude/scripts/debt-doctor.mjs`（RED 须零命中；advisory 可留但要知道有哪些）。
   报告：新条目路径 + 域 + review profile + 下一步命令。
   **终点**：渲染已跑、doctor 已跑、报告四样齐全；下游路由细则见末节。

## 快路径就绪自检（登记前过一遍，缺一即未就绪）

- [ ] provenance：≥1 条可点开的执行记录（非作者声称）。
- [ ] 域已定且 ID 前缀与域一致（祖辈 `ARQ-` 例外，见步骤 2）。
- [ ] Track B 三要件逐条标注；任一 no 则 profile 已是 `full-adversarial+human`，未回标 Track B。
- [ ] `paths[]` 已列全（G 层与硬排除由 doctor 机推，非手写）。
- [ ] 便宜验证计划点名。
- [ ] frontmatter 必填齐全；正文四段齐全；review profile 写实。
- [ ] 查重已做：同根追加、异根新立；ID 与文件名一致。
- [ ] `debt-render.mjs` 与 `debt-doctor.mjs` 已跑（RED 零命中）。

## 约束（must / never，不可省）

- **MUST**：每条改进项带至少一条可识别 provenance——Track B 资格的客观载体。
- **MUST**：review profile 写实。三要件任一不满足、或触 G1/G3/契约面/gate/self-def/mixed，就标 `full-adversarial + human`，绝不美化成快路径。
- **MUST**：查重在前；同根在既有条目追加证据注记，不新立。
- **MUST**：写完跑 `debt-render.mjs`（否则 生成的 INDEX 与账本不一致，doctor 会报 `index-stale`）。
- **NEVER**：不自批、不开 TaskPackage、不改任何 canonical（`.claude/{agents,commands,references,rules,skills}/**`、`docs/contracts/**`、docs 蓝图与指令根）——本 skill 只写条目文件与既有条目注记。
- **NEVER**：不把「新增/改工程门」当快路径塞——新增或修改任何 gate 本身就是 lightweight 机检硬排除，须标 full（或按 Track B 写明 G3 + 该 gate 事实交判）。
- **NEVER**：不手改三份 `INDEX.md`（生成件）；不在 `docs/debt/` 下建条目以外的文件（说明的单一源在 lib / template / index 头部）。

## 交接下游（terminal state）

本 skill 的 terminal state = 「条目写进 `docs/debt/<域>/` + 渲染已跑」（边界即「约束」节 NEVER，不重复）。按 review profile 交下游：

- **已登记条目（默认）** → 走 **Track B 默认路由**（主线程起草 PROPOSAL 走起，见文首路由注）。
- **命中升级判据（跨系统结构性变更 / 方向未收敛）或声明红队** → 升所属面 loop 的 **Track A 段**。
- **provenance 不足 / 其实是新设计 idea** → 不登记，交面 loop（**`/arch-change` / `/gov-change`**）。
- **是跨 Pod 周期性共性、应批量 retro** → 交 **`/loop-retro`**（其 triage 第 4 步同样产出 ARQ candidate，字段一致）。
- **tech 域条目**：登记后工作本身交出去（`handoff` 字段写去向 + 移交触发）——账本存的是「已交 X 面、去向 Y、触发 Z」，不是「活在我这儿」。
