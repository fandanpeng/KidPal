# PROPOSAL · 260928-gov-review-cross-face-intake-spine

> **状态：FROZEN v1.0 · 2026-09-28 · 凭据 = human 立项 ack（2026-09-28 主线程会话，OQ-1~4 全裁：Track B / settings(b) / 并入 / 后面验证）+ architect R1 BLOCK→R2 GREEN（[review.md](review.md)，R2-M1/M2 冻结前顺手修毕）+ human 裁 OQ-1~4**。主线程凭据自冻。
> **承载 ARQ**：`ARQ-260928-triage-cross-face-intake-gap`（本批 = 该 ARQ 的主体批：承载建议 1/3/4；建议 2 已由 260928-gov-review-requirement-face T3 先行承载——该批 §6 三件留根〔AGENTS.md 需求面行 / 两 agent scope-map 登记 + fixtures 补条〕由本批 T5/T6 收口）。
> **review 轨**：Track B 常驻轻路径（**human 2026-09-28 裁 OQ-1=(b) 改判**，原话「trackA 只有在全新、非常不清晰的情况下使用，都很明确了就不要用了」——ARQ 自评 Track A 被 human 裁定覆盖，留痕于此；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T7）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（触强制层 scope-map + 指令根 AGENTS.md + 新增 headless 会话拉起机制；但契约面零触、hooks/settings 零触、门体系零改动——只加 intake 前置，四面门照旧；headless 三铁律禁跨门、禁递归、身份显式）。
> **review profile**：G2+G3（G2 = references ×3 + commands ×2 + rules ×1 + 指令根 ×1 + CRQ append ×1，共 8 件；G3 = scope-map.json ×1 + route-fixtures ×1；G1/契约面为零，见 §3）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/references/change-intake.md
.claude/commands/triage-change.md
.claude/references/change-triage.md
.claude/rules/repo-structure.md
.claude/references/task-intake-and-slicing.md
.claude/commands/build-feature.md
AGENTS.md
.claude/scope-map.json
.claude/route-fixtures/routing.json
docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md
```

---

## 0. 想实现的效果与差距

**目标效果**：跨面 idea（同时触需求 / 架构 / 契约 / 实现中 ≥2 面的变更）从「human 在 3-4 个窗口以 3-4 种留痕格式重复同一意图」变为「**一个入口、一次陈述、一份 spine 耐久载体**」——`/triage-change` 升格为跨面 intake：human 说一次意图，agent 对照四面 canonical 文本 + CRQ 未结账做触面探测，落一份 spine 制品（`delivery/changes/intake/<changeId>.md`），分面计划驱动各面门照旧各归各裁（统一 intake ≠ 统一门），各面定稿逐面回填 anchors 使跨面顺序可查；面间接力由主线程 headless 拉起下游编排会话完成，human 只在各面既有的人类触点出场。

**现状差距**：① triage 判据面只有实现侧四档（slicing matrix + riskTier，change-triage.md §2），需求/架构/契约触面仅在第一刀当排除条件（碰 → 踢走 /arch-change），不进 intake、不当分面计划；② 意图无耐久载体——第一份耐久制品（plan.md / PROPOSAL）在 review 之后且已实现形，原始意图只以「投影进派生 plan」方式幸存，plan 没承载的面（需求语义、架构关系、契约影响）永久丢失；③ ② 单包路径无跨面顺序机制——260912 链 launchPreconditions 只覆盖 ③/proposal 路径，② 路径靠人肉顺序 + 事后逃生门；④ 面间接力靠 human 手工开窗口 + 手工组织启动 prompt，ARQ-260928 证据 2 walkthrough 实录连走流程的主线程都漏查架构面。②③④ 的机制根源同源：没有一份跨面制品可指。

**非目标**（防 scope creep）：① `/arch-change` 本体不动、退役安置不做（前批 260928-gov-review-requirement-face §6 已留根为独立治理变更；本批只把 triage 升格为可与之并存的统一 intake 入口）；② 契约面 `docs/contracts/**` 零触——契约门程序照旧，本批只把 faceMap 契约行接为四分类的输入；③ hooks / settings 零改动——headless 拉起按面显式传匹配的**既有**面专属 settings（OQ-2 已裁 (b)：`dev-loop.settings.json` / `review-loop.settings.json` 在册复用、零文件改动）+ 显式 `--agent` 钉身份；④ 不在本批造 spine 活体实例（机制批；首个实例随下一个真实跨面 idea，OQ-4）；⑤ 不改 requirement-change.md 三档判据本体与 write-story（只消费，requirement-change.md:3 留根句 verbatim「triage intake 跨面接线消费本文件判据」即本批 T1/T2 的消费授权）；⑥ showme 讨论稿 `docs/showme/show-me-triage-intake-spine.html` 保持讨论稿定位，不转正、不进 manifest。

## 2. 问题、根因与改法（T1–T7）

**【硬项 · 起草期实证义务】**本节断言纪律逐条已执行（2026-09-28）：agents 目录实跑枚举 16 个定义（14 既有 + story-author + requirement-reviewer，`ls .claude/agents/` 实跑在案）；`.claude/scripts/` 全清单实跑在案——landing-fidelity-check.mjs / lint-descriptions.mjs / route-regression-check.mjs / changes-index.mjs / taskpackage-required-check.mjs 均在册；route fixtures 实径 = `.claude/route-fixtures/routing.json`（find 实跑核实；其 `_doc` 尾句 verbatim「档2 迁入 dev-loop 时补 build-feature/planner 行」）；「第一刀」census（`grep -rn "第一刀" .claude/ AGENTS.md docs/ delivery/workstreams/`，2026-09-28 复跑）= 9 处：`.claude` 流程面 3 处（triage-change.md:13 / :21、change-triage.md:10，全在 T2 目标集内）+ 集外 6 处**描述性引用**（docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md:13 / :19 / :20 为 append-only 历史行；docs/showme/README.md:13、show-me-gov-triage.html:108 / :122 为讨论稿）——六处显式豁免、机检 scope 收窄（R1-F1，详见 §3 复扫硬项与 §5 V4）；build-feature.md:16 launch 面预检「四件」原文已核（verbatim 引于 T4）；task-intake-and-slicing.md:272-285 launchPreconditions canonical 定义节已核；AGENTS.md 治理路由节 :10-:27 已核（T5 插入锚 verbatim 引于 T5）；requirement-change.md:3 留根句已核（见 §0 非目标 ⑤）；两 agent description 信号实文已核（T6 fixtures 条目逐信号对 description 核过，2026-09-28）。

**【硬项 · 目标文件锚清单】**每 T 末尾显式列目标文件；landing-manifest ≡ 各 T 并集（T7 为 append-only 注记，无迁移项）。

### T1 · 新建 change-intake.md reference（跨面 intake 流程 SSOT）

**问题**：跨面变更没有统一 intake 程序文本——四面探测怎么做、spine 什么形态、面间怎么接力、子流程疑问找谁，全无 canonical 承载；不立 SSOT，triage 升格（T2）就没有可引用的程序权威。
**证据**：ARQ-260928 建议 1（intake 四面申报）/ 3（launchPreconditions 推广 ②）/ 4（grilling 收敛一次）均无落点；requirement-change.md:3 已为「triage intake 跨面接线」留根但接线文本不存在。
**根因**：triage 门立门时（gov-review-architect-change-triage，FROZEN 2026-07-22）按纯实现侧分流设计，跨面 intake 是其后才被 walkthrough 实测暴露的缺口。
**改法**：新建 `.claude/references/change-intake.md`，八块内容：
1. **触发与比例化**——触面 ≥2 的变更强制建 spine；单面 ① 直接修免门（地板不变）、单面 ②③ 现状不动（plan.md / PROPOSAL 本身已载 intent，无重复陈述问题）。
2. **spine 制品形态**——`delivery/changes/intake/<changeId>.md`，一变更一文件，lane 中立（arch/dev/retro 第四 sibling，结构登记见 T3）；frontmatter 七字段：`changeId`（全局 changeId，YYMMDD-slug；各面锚回填时映射为各面既有格式——req = §9 版本号、contract = packet、arch = changeId、imp = taskpackage id；单包 ② 直接复用不另铸）、`sources`（human 原话逐字 + CRQ / §9 / retro 引用 id 列表，词汇复用 TaskPackage `sourceRefs`）、`faceMap`（四面触面事实 + governance 探测命中行：动不动 / 动哪 / 依据行号，R2-M2）、`facePlan`（各面谁裁 + 执行顺序 + grilling 一次收敛的裁定）、`anchors`（per-face 回填槽，初始空）、`openQ`（子流程疑问与裁定留痕）、`status`（含 `awaiting-human:<gate>` / `awaiting-upstream` 态）。
3. **四面探测程序**——主线程派 architect（只读 + 同会话 return），语料 = requirements.md（含 §9 账 + story AC）+ architecture.md + docs/contracts/** + CRQ 未结账三域（gov / tech / requirement）；触面事实是**查出来的**不是申报出来的（walkthrough 实证自我申报不可靠）；产出 faceMap + facePlan 交主线程落盘。语料另含 governance 面（`.claude/agents/**` 定义 / gate 登记）——命中记入 faceMap 并路由 /arch-change 治理 profile，不建实现面档（R1-F4）。**建档前查重**：intake 开场先查 `delivery/changes/intake/` 既有 spine（按意图与触面相近判），命中 → 消费既有件不重复建档；并存期 /arch-change 侧同此纪律——其文本改动归退役安置批，本批以 change-intake.md 单侧承载（R1-F5）。
4. **立项 ack**——intake 收尾唯一必停人类触点：四面对齐「为什么改 / 往哪改 / 改动多大」；升级判据（触契约 / 跨系统结构性 / 存疑向严）随 ack 一并裁定路由。
5. **发起机制**——两姿势：轻面（需求档 0/1、单步活）同会话内联一级派发；重面（架构 Track A/B、契约 packet 起草、③ 拆包）主线程经 Bash 后台 headless 拉起 `claude -p`（它是完整顶层会话、非子 agent，可 `--agent` 跑完整编排循环）。**三铁律**：禁跨 human 门（到门即停，`status` 写 `awaiting-human` + 退出报告，human `claude --resume <session-id>` 进同一会话拍板，上下文无损）；禁递归（headless 子会话不得再 `claude -p` 拉孙会话）；身份三件套显式（命令行显式钉 `--agent` + 按面显式传匹配 settings——`dev-loop.settings.json` / `review-loop.settings.json` 在册复用、零文件改动，OQ-2 已裁 (b)；ARQ-260918 贴卡起步丢身份先例）。长循环必须后台执行 + 通知回调（Bash timeout 实测上限 10 分钟）；子会话 stdout 只是进度，状态一律以 spine 落盘为准。**anchors 回填写者恒 = 主线程**——headless 子会话完成面工作后以退出报告交回，主线程回填（写域零扩张：headless 子会话的 per-agent 写域不含 intake/，review-moderator 现形态即拦，R1-F6）；**单写者规则**——同一 spine 同一时刻仅一个主线程会话持写权，跨会话接力经 openQ / 退出报告交接，禁并行双写（R1-F6）。三铁律第一版为 prompt / 文面纪律，机检齿留根（§6）。
6. **疑问三级处置**——L0 子流程就地解决（执行歧义 + canonical 有依据可引 + 不涉门语义 / 不越权限 / 不创先例 / 影响限于本变更内 → 直接办，裁定 + 依据写 `openQ` 留痕）；L1 主线程答疑（子会话写 `openQ` 退出 → 主线程凭 faceMap / facePlan / 立项 ack 取舍倾向作答——跨面上下文只在主线程完整存在，能答的写回 `openQ` 续跑，不消耗 human 出场）；L2 呈 human（判据存疑向严 / 枚举外新情形 / 需要取舍——对齐触点①预授权「枚举外一律回问」与 Track B「存疑向严」既有纪律，从触点①推广到全部子流程疑问）；L1 答不了的**去向**：openQ 标 `escalated` + status 写 `awaiting-human:<openQ>`，主线程呈人，答复写回 openQ 后 headless 续跑，不得悬置（R1-F6）。
7. **spine 免冻 + facePlan 修订权限**——spine 是路由账本不是设计文档，不设 FROZEN；各面的冻结语义由各面制品承载（§9 版本行 / 契约 packet / FROZEN PROPOSAL / origin.frozen），spine anchors 记录这些冻结事实。facePlan 修订：加面 / 改「谁裁」= L2 呈人；调执行顺序 / 细节 = L0/L1。
8. **与各面门接缝**——faceMap 契约行（动不动 / 动哪 / 依据行号）直接作为 architect-reviewer 四分类的输入（契约门开场带 intake 取证，human 第二次出场只答签不签）；**升级判据 ≠ 契约门**消歧（升级判据 = 路由门，答「走哪套流程」，触面疑似即可命中、判错可挽回；契约门 = 审批门，答「这份 diff 签不签」，针对具体 packet、判错污染 SSOT；两门独立可达——升级后四分类可判「无需改」、未升级也可能被逃生门踢进契约门）；下游会话**开场即消费**纪律：带 changeId 调用的各面流程入口先读 spine 的 faceMap/facePlan，不得要求 human 重述意图。

目标文件：`.claude/references/change-intake.md`（新增）

### T2 · triage 门升格跨面 intake（第一刀改四面探测）

**问题**：triage 第一刀把契约/架构触面当**排除条件**（碰 → 踢走 /arch-change，根本不进本门），而不是 intake 的一部分——跨面 idea 被切碎进不同门，意图重复陈述 + 顺序人肉的根源就在这一刀。
**证据**：triage-change.md:13 verbatim「**与 `/arch-change` 的分区（第一刀）**：变更进来先问**碰不碰单一事实源**（`docs/contracts/**` / kernel·systems spec / `.claude/agents` 定义 / gate）——碰 → 跟大小无关，一律直走 `/arch-change`，**根本不进本门**；不碰（纯 firmware/server/docs）→ 进本门由 architect 判四档。」；change-triage.md:10 同语义；triage-change.md:21 步骤 1 verbatim「**第一刀（事实源分区）**：读 plan，判它碰不碰单一事实源。碰 → 告知用户直走 `/arch-change`，本门收尾。不碰 → 继续。」；触点③（triage-change.md:32-35）现为「打出人类要起的下一段会话命令」由 human 手工开窗。
**根因**：立门时按纯实现侧分流设计；「碰 SSOT」当时唯一的处理手段是整体上交，没有 faceMap/facePlan 概念，无法「分面处置」。
**改法**：两文件协同改，**判四档的判定本体（change-triage.md §2 表 + 单向守卫）与逃生门（§6 reviewer-first）一字不动**——逃生门保留为探测有漏时的兜底（两道门独立可达）。改动三处：
1. **第一刀 → 四面探测**（triage-change.md:13 替换后完整句）：「- **四面探测（intake，程序 SSOT 见 `change-intake.md`）**：变更进来先做跨面探测——对照 requirements / architecture / contracts + CRQ 未结账 + governance 面逐面查触面事实（枚举以 SSOT 为准，R2-M1），主线程落盘 spine（触面 ≥2 时）；命中升级判据（触 `docs/contracts/**` 或跨系统结构性）→ 按 facePlan 把该变更的架构/契约面路由 Track A/契约门，实现面留本门分档，不再整单踢走；纯实现面变更 → 照旧判四档。」change-triage.md:10 与 triage-change.md:21 同语义改写（含「第一刀」措辞退役，配合 §5 V4 流程面 census 清零）。
2. **触点①与四档判定保留**——实现面档位裁决仍是本门核心产物；facePlan 的实现面行 = 四档裁决。
3. **触点③ → runbook 接力**（triage-change.md:32-35 **整块替换**，替换后完整句）：「6. **报告下一步（runbook）**：按 facePlan 顺序打出各面执行命令——轻面注明内联执行（本会话继续派发），重面给出 headless 拉起命令（含显式 `--agent`、匹配 settings 与 changeId）；下游会话开场读 spine，不要求 human 重述意图；单包/拆包的下一步命令映射（②单包 → planner `/build-feature <id>`；③大 feature → portfolio-planner `<ws>`）保留为 runbook 条目。」原 :33-34 的命令映射语义并入上句保留；**原 :35「planner/portfolio 必须是自己的主线程会话——子 agent 不能再派子 agent，本门拉不起那个会话；dev loop 本就该人类在场」随块删除**——「本门拉不起那个会话」被 headless 接力取代（R1-F3），其人类在场语义由 change-intake.md 三铁律「禁跨门」承接（dev loop 关键门 human 仍在场），「子 agent 不能再派子 agent」约束不变（headless 拉起的是完整顶层会话、非子 agent）。
目标文件：`.claude/commands/triage-change.md`（改）· `.claude/references/change-triage.md`（改）

### T3 · repo-structure.md 结构登记两行（intake 新行 + retro 漂移修复）

**问题**：① `delivery/changes/intake/**` 将成为 spine 落点，但结构 SSOT 无此行——不登记则首份 spine 落盘即成「白名单外产物」；② 现存漂移：`delivery/changes/retro/` 目录已存在（2026-09-13 / 2026-09-23 两份 loop-retro 制品在册，ls 实跑核实）而 repo-structure 顶层地图无此行——归口靠口口相传。
**证据**：repo-structure.md:32-33 现有 verbatim「| `delivery/changes/arch/**` | review-loop | 架构 review 证据：PROPOSAL / landing-summary / verdict / retro | 当契约 SSOT 用 |」「| `delivery/changes/dev/**` | dev-loop | 实现变更证据归档 | 当 live 工作区用（live 在 `delivery/taskpackages/`） |」——两行之间即 intake 行插入位，retro 行随后追加。
**根因**：retro 目录是 loop-retro skill 落地时顺手建的，未走结构登记；intake 是本批新面。
**改法**：在 :32-33 两行后追加两行（追加不覆盖）。两新行**逐字行文**钉死如下（landing-summary 逐字承载供核验官对 diff，R1-F8）：

```
| `delivery/changes/intake/**` | 主线程（跨面 intake 落盘） | spine 制品：`delivery/changes/intake/<changeId>.md`（触面 ≥2 的变更入口记录；程序 SSOT = `.claude/references/change-intake.md`） | 当各面门制品的家（各面制品落各自既有家，spine 只记 anchors）；手改各面 anchors 以外的历史字段 |
| `delivery/changes/retro/**` | 主线程（loop-retro cadence） | cadence retro 制品归档（loop-retro 产出） | 当 live 工作区用；当各面门制品用 |
```
目标文件：`.claude/rules/repo-structure.md`（改）

### T4 · launchPreconditions 推广 ② 单包路径（spine anchors 前置）

**问题**：跨面顺序机制只覆盖 ③/proposal 路径（workstream `decision-coverage.json` 顶层键 `launchPreconditions` + `portfolio-reconcile.mjs --check-launch` 读侧），② 单包路径的「上游面未冻结不得实现」无机检载体，靠人肉顺序——ARQ-260928 建议 3 的原文缺口。
**证据**：task-intake-and-slicing.md:272-285 canonical 定义节（`### launchPreconditions（canonical 定义）`）全部条目以 workstream 决议为源，无 spine 源；build-feature.md:16 launch 面预检 verbatim「先核 launch 面**四件**——① board 条目…；② 启动卡…；③ 运行时 worktree…；④ 专属图索引…。**四件齐 → 原样进 Step 1 intake；任一缺 → 不进 planner intake**」——预检骨架现成，只差 spine 源一件。
**根因**：launchPreconditions 设计时只有 ③ 路径形态（workstream 决议），spine 当时不存在；② 路径的顺序保障随本批 spine 出现才第一次有可机查的挂点。
**改法**：两文件：
1. task-intake-and-slicing.md launchPreconditions 节追加一条（append 到 :285 存量迁移条目之后）：「**spine 源（② 单包路径，260928-gov-review-cross-face-intake-spine 起）**：带 spine 的变更（`delivery/changes/intake/<changeId>.md` 在场），spine `facePlan` 中**先于实现面**的各面 anchors 未回填 = 上游面未冻结 → 承载包不得 launch（与 ③ 路径『未冻结 → 承载该裁定的包不得 launch』同判、粒度同为包）；核验 = build-feature launch 面预检第五件（文面路由，非机械闸），机检齿留根。」
2. build-feature.md:16 预检四件扩五件——「四件」改「五件」，④ 与句号间插入「；⑤ spine 上游面（`delivery/changes/intake/<changeId>.md` 在场时核——其 facePlan 中先于实现面的各面 anchors 已回填；未回填 → 不进 intake，报 `awaiting-upstream`，先完成上游面）」，末句「**四件齐 → 原样进 Step 1 intake；任一缺 → 不进 planner intake**」对应改「**五件齐 → …**」（逐字替换其余不动）。
目标文件：`.claude/references/task-intake-and-slicing.md`（改）· `.claude/commands/build-feature.md`（改）

### T5 · AGENTS.md 治理路由节补两行（需求面 + 跨面 intake）

**问题**：① 需求面三档流程已立（前批）但 AGENTS.md 治理路由节无需求面行——前批 §6 显式留根「AGENTS.md 治理路由节补需求面一行归 ARQ-260928 批」；② 本批跨面 intake 立新路由事实，指令根无行则路由不完整（各会话只靠 CLAUDE.md 导入的 AGENTS.md 建立路由认知）。
**证据**：AGENTS.md:24 升级判据段 verbatim 尾句「**存疑向严**：判据适用存疑默认升 human。」；:26 起始 verbatim「**landing 通道（常态规则）**：恒 = 主线程 packet-bound 一条；architect 恒纯判官 / 起草、永不 landing。」——插入位即两段之间。
**根因**：前批为控 profile 不碰指令根；本批本就触 AGENTS.md（G2），留根两行一并收口，避免第三次单独碰指令根。
**改法**：在 :24 段与 :26 段之间插入两段（追加，不覆盖任何既有行）：「**需求面变更**：走 requirement-change.md 三档流程（story-author / requirement-reviewer 对偶起草与审稿，260928-gov-review-requirement-face 立；SSOT `.claude/references/requirement-change.md`）。」「**跨面 intake**：触面 ≥2 的跨面 idea 先过统一 intake 建 spine（`delivery/changes/intake/<changeId>.md`；程序 SSOT `.claude/references/change-intake.md`，本批立）——统一 intake ≠ 统一门，各面门照旧各归各裁；spine 免冻，anchors 记录各面自己的冻结态。CLAUDE.md 经 `@AGENTS.md` 导入传导，不另改。」
目标文件：`AGENTS.md`（改）

### T6 · scope-map 登记两 agent + route fixtures 补条（前批留根收口）

**问题**：story-author / requirement-reviewer 是全仓首批不在 scope-map 登记的 agent 定义（前批 §3 如实披露：「其『无 Write / Bash 限跑 spec-anchor-check』系 agent 正文纪律约束、非机检强制」）；route fixtures 亦无两 agent 断言（前批 V3 自曝真空绿：「该命令绿 ≠ 新 agent 路由信号在位」）。
**证据**：前批 PROPOSAL §3 强制层行 + §5 V3 注记 verbatim 已核；`.claude/route-fixtures/routing.json` 现有 `agentRequiredSignals` 六条目（review-moderator / scenario-author / challenger / defender / adjudicator / architect-reviewer，实读核实）；两 agent description 实文已核（信号逐条对得上，见改法）。
**根因**：前批刻意控 profile 不触强制层，留根本批；本批本就触 scope-map（spine 落点归主线程写、无需新写槽——写者 = 主线程不受 per-agent 限，与现行 plan.md 落盘同构），顺手收口。
**改法**：① scope-map.json `agents` 段新增两条目，形态同 architect-reviewer（纯判官 / return 型写域零授予）：`story-author` 与 `requirement-reviewer` 均 `allow: []`、`deny: ["**"]`（enforce-scope 管写不管读，return 型交付不受影响）；② routing.json `agentRequiredSignals` 补两断言（信号逐条摘自 description 实文）：`story-author: ["requirement-change.md", "requirement-reviewer", "对偶", "无 Write"]`、`requirement-reviewer: ["story-author", "四查", "作者≠审者", "不审契约"]`。两 agent 无 `tools:Agent()` 白名单，`agentRequiredToolsAgents` 不动。
目标文件：`.claude/scope-map.json`（改）· `.claude/route-fixtures/routing.json`（改）

### T7 · ARQ-260928 条目回写承载注记（append-only）

**问题**：本批承载建议 1/3/4 后，ARQ 条目正文仍显示三建议待做——后续读者（含 /arch-change 退役安置批）会重做或误判范围。
**证据**：条目末注记现只有一条（「**2026-09-28 注记**：建议 2（需求面比例化入口）已由 260928-gov-review-requirement-face 批 T3 承载……建议 1/3/4（intake 四面申报 / launchPreconditions 推广②路径 / grilling 收敛）与 /arch-change 退役安置仍归本条目后续批。」）；append-only 纪律（AGENTS.md Hard Rules「历史记录类文件一律 append」）。
**根因**：同前批 T6——登记先于承载，承载批负责回写。
**改法**：条目正文末尾 append 一段：「**2026-09-28 注记（第二批）**：建议 1/3/4（intake 四面申报 / launchPreconditions 推广②路径 / grilling 收敛一次）已由 260928-gov-review-cross-face-intake-spine 批承载——统一 intake 程序 SSOT 落 `.claude/references/change-intake.md`，triage 第一刀改四面探测，spine = `delivery/changes/intake/<changeId>.md`；/arch-change 退役安置仍留根独立批。另：本条目风险节自评 review profile = Track A，经 human 2026-09-28 裁 OQ-1=(b) 改判 Track B（改判原话留痕本批 PROPOSAL §4），本批按 Track B 执行。」只追加、不改既有行。
目标文件：`docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md`（改·append-only 注记）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| reference | `.claude/references/change-intake.md` | 新增 | G2；消费方 = 主线程（intake 编排）+ triage 门（程序引用）+ 各面下游会话（开场即消费） |
| reference | `.claude/references/change-triage.md` | G2 | 第一刀改四面探测；§2 判定表与 §6 逃生门不动 |
| reference | `.claude/references/task-intake-and-slicing.md` | G2 | launchPreconditions 节 append spine 源一条；既有 canonical 定义零改动 |
| command | `.claude/commands/triage-change.md` | G2 | 第一刀/触点③ 改写；消费方 = 主线程 |
| command | `.claude/commands/build-feature.md` | G2 | launch 面预检四件扩五件；消费方 = planner |
| rules | `.claude/rules/repo-structure.md` | G2 | T3 结构登记两行（intake 新行 + retro 漂移修复）；消费方 = 全会话（结构边界认知） |
| 指令根 | `AGENTS.md` | G2（追加两段） | 治理路由节插入两段；Hard Rules 与既有行零改动；CLAUDE.md 经导入传导不另改 |
| 强制层 | `.claude/scope-map.json` | G3 | agents 段新增两条目（return 型零写域）；universal 段与既有条目零改动 |
| 机检 fixtures | `.claude/route-fixtures/routing.json` | G3 | agentRequiredSignals 补两断言；commandEntry / agentRequiredToolsAgents 零改动 |
| CRQ 账本 | `docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md` | G2（append-only 注记） | T7；只追加不改既有行 |
| 契约面 | `docs/contracts/**` | 未触 | 契约门程序照旧；faceMap 契约行只是四分类的输入，不改契约面文本 |
| 强制层其余 | hooks / settings | 未触 | 无新 hook；headless 复用既有面专属 settings（OQ-2 已裁 (b)），零 settings 文件改动 |
| 讨论稿 | `docs/showme/show-me-triage-intake-spine.html` | 未触 | 保持讨论稿定位（§0 非目标 ⑥） |

**【硬项 · sweep 透镜义务】**本批不落地新门/新断言脚本（change-intake.md 是流程 reference；build-feature 预检第五件与 task-intake spine 源条款均为文面路由、非机检判定，与前批同性质），无判定 pattern 全仓预跑义务。**【硬项 · 复扫模式完备性先验】**「第一刀」语面退役残留清扫：pattern = 全词 `第一刀`（grep 已含反引号包裹与裸写两形态），census 全量 = **9 处**（2026-09-28 复跑）：① `.claude` 流程面 3 处（triage-change.md:13 / :21、change-triage.md:10）——全在 T2 目标集内、全数改写；② 集外 6 处**描述性引用**——docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md:13 / :19 / :20（ARQ 条目 append-only 历史行，AGENTS.md Hard Rules 禁改写既有行）与 docs/showme/README.md:13、show-me-gov-triage.html:108 / :122（showme 讨论稿，非目标⑥不动）——六处**显式豁免**：均为对该门术语的历史描述性引用，非本批改写对象语面，机检销项按收窄 scope 判（V4）。完备性论证 = census 全量已列、无第三类形态（该词仅此两种用法：门内术语定义与对它的转述）。销项 = ①类 3 处全数改写 + ②类 6 处逐条留痕豁免（R1-F1）。

## 4. human 取舍点（OQ）

| OQ | 题 | 裁定 |
|---|---|---|
| 已裁-1 | spine 落点 = 候选 C（`delivery/changes/intake/`，lane 中立第四 sibling） | **已裁（2026-09-28 讨论轮次二）**：同意 |
| 已裁-2 | changeId 命名空间：spine id = 全局 changeId，各面锚回填映射既有格式，单包 ② 复用不另铸 | **已裁（同上）**：同意 |
| 已裁-3 | intake 不进 changes-index 的 lane INDEX，可见性由 spine frontmatter 承载 | **已裁（同上）**：同意 |
| 已裁-4 | 比例化阈值：触面 ≥2 才建 spine；单面 ①②③ 现状不动 | **已裁（同上）**：同意 |
| 已裁-5 | 发起机制：轻面内联 + 重面 headless 接力，gate 停机 + `--resume` 接住 | **已裁（human 提议，讨论轮次三）**：同意 |
| 已裁-6 | 疑问三级处置：L0 子流程就地解决 / L1 主线程答疑 / L2 呈人 | **已裁（human 原则，讨论轮次三）**：同意；判据细化见 T1 第 6 块 |
| 已裁-7 | FROZEN 不盖 spine（spine = 路由账本免冻）；facePlan 修订权：加面/改「谁裁」= L2，调顺序细节 = L0/L1 | **已裁（讨论轮次四）**：同意 |
| 已裁-8 | 升级判据 ≠ 契约门（路由 vs 审批两道题）；faceMap 契约行作四分类输入，契约门开场带取证 | **已裁（讨论轮次四）**：同意 |
| OQ-1 | review 轨：维持 Track A（ARQ 自判）还是讨论轮次收敛设计分叉后降 Track B | **已裁（2026-09-28 human）**：**(b) Track B**——human 原话「trackA 只有在全新、非常不清晰的情况下使用，都很明确了就不要用了」；ARQ 自评 Track A 被 human 裁定覆盖，Status 行已同步；改判留痕随 T7 回写 ARQ 条目（R1-F7） |
| OQ-2 | headless 第一版 settings 策略 | **已裁（2026-09-28 human）**：**(b) 复用既有面专属 settings**——headless 拉起按面显式传匹配件（`dev-loop.settings.json` / `review-loop.settings.json` 已在册，零文件改动），两件都不匹配的面用项目默认 settings；「身份三件套（`--agent` + settings + changeId）显式钉死」成文进 T1 发起机制 |
| OQ-3 | 前批三件留根（AGENTS.md 需求面行 / scope-map 两 agent 登记 / fixtures 补条）并入本批 vs 拆独立小批 | **已裁（2026-09-28 human）**：**(a) 并入**——T5/T6 现状即并入形态 |
| OQ-4 | spine 首个活体实例：本批不带实例（机制批）vs 立即找一个 pending idea 跑首例验证 | **已裁（2026-09-28 human，原话「后面验证吧」）**：**(a) 机制批不带实例**——首例随下一个真实跨面 idea，验证实录随例 retro |

## 5. 验证集（landing 前机械前置 + landing 后核验）

| # | 时点 | 命令 | 预期 |
|---|---|---|---|
| V1 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260928-gov-review-cross-face-intake-spine/PROPOSAL.md` | 形态零错；anchorUnion 与 manifest 十路径逐一吻合、无注记夹带路径 token、无目录 token advisory |
| V2 | pre-commit | `node .claude/scripts/route-regression-check.mjs` | 绿——T6 补条后两新 agent 信号断言在 fixtures 且与 description 实文一致（本批补条使前批 V3 的真空绿缺口闭合，非真空绿） |
| V3 | pre-commit | `node -e "JSON.parse(require('fs').readFileSync('.claude/scope-map.json','utf8'));console.log('scope-map ok')"` | 输出 `scope-map ok`（T6 JSON 合法性） |
| V4 | pre-commit | `grep -rn "第一刀" .claude/ AGENTS.md delivery/workstreams/` | 零命中（T2 改写后流程面 census 清零；scope 较起草期**收窄**——剔除 docs/，集外 6 处描述性引用为 append-only 历史行与讨论稿、显式豁免，R1-F1） |
| V5 | post-commit | `grep -n "需求面变更\|跨面 intake" AGENTS.md` | 治理路由节两新增段各 ≥1 命中 |
| V6 | post-commit | `grep -c "260928-gov-review-cross-face-intake-spine" docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md` | = 1（T7 append 恰一段） |
| V7 | post-commit | `grep -n "changes/intake\|changes/retro" .claude/rules/repo-structure.md` | 顶层地图两新行各 ≥1 命中 |

## 6. 留根清单

- **/arch-change 退役安置**（governance 面剩余流量归并、与统一 intake 的入口关系定稿、其开场查重条款落文本）→ 独立治理变更（前批留根 + 本 ARQ 注记双锚；查重纪律本批已单侧承载于 change-intake.md，R1-F5）。
- **headless 机检齿**（三铁律的机器强制：递归拉起检测、`awaiting-human` 越门拦截、`--agent` 身份核验）→ G3，随首次真实 headless 面的实况立项。
- **面专属 headless settings 文件** → OQ-2 裁 (a) 后留根，首个真实 headless 面实证权限需求再立。
- **spine 机检渲染 / INDEX 可见性**（intake 版 crq-render、anchors 齐套机判）→ G3（已裁-3 的后续自然延伸）。
- **grilling 收敛的 L0 判定口径细化**（「有依据可引」的边界案例）→ 随首个 spine 实例的 openQ 实录实证再成文。
- **`task-intake-and-slicing.md` 补「定稿版本号引用」合法形态** → 拆包链治理下批（前批留根原样保留，本批未触）。
- **glossary.md 补术语条目**（spine / faceMap / facePlan / anchors / openQ）→ 下次 glossary 批顺手收。
- **ARQ-260928-triage-cross-face-intake-gap 结项** → 本批 landing + landing-verifier 绿 + retro 齐后，评估建议 1/3/4 全落地 + 退役安置留根清晰，由主线程裁是否结项（退役安置未做前倾向留根不结）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-28 - architect

verdict: BLOCK

按模板四硬项逐核 + T1–T7 实质审查：V1 preflight 实跑绿（anchorUnion 十路径与 manifest 逐一吻合、形态零错）；起草期实证义务大头为真（16 agents 枚举、五脚本在册、全部 verbatim 引据亲核无误）；T6 先例形态（architect-reviewer `allow:[], deny:["**"]` 同形）与 8 条路由信号逐条对 description 实文全中；V2/V3 现跑绿、「非真空绿」表述准确；T4 fail 语义与既有预检同口径；riskTier medium + G2+G3 证成成立。BLOCK 2 项：**F1**「第一刀」census 断言失实（实跑 9 处非 3 处——docs/crq 3 处 + docs/showme 3 处为断言称零的集外命中），且致 V4 按现 scope landing 后必红不可判（ARQ append-only、showme 非目标，6 处描述性引用永久命中）——修法 = V4 收窄至 `.claude/ AGENTS.md delivery/workstreams/` + §2/§3 census 改全量如实并显式豁免描述性引用；**F2** §3 sweep 表漏 classify manifest 目标文件 `.claude/rules/repo-structure.md`（UNCLASSIFIED=0 失实）+ review profile 枚举漏 rules ×1——修法 = 补一行 + 补一枚举。两项均文面修订级，不动 T1–T7 改法本体与 OQ 裁定。另附 MINOR 六条（F3 T2 触点③ 替换块 :33-35 去留两读、F4 四面探测漏 governance 面分支、F5 双入口并存期无 spine 查重纪律、F6 anchors 回填写者未定 + 并发写无防护 + L1 答不了的去向、F7 OQ-1「Status 行已同步」与 ARQ 实况不符、F8 T3 两新表格行未钉逐字行文），其中 F3/F7 属落地两读/失实留痕类建议必改。逐 finding 证据与实测留痕见本目录 `review.md` R1 段。

### R2 - 2026-09-28 - architect

verdict: GREEN

R1 BLOCK(F1/F2 + MINOR F3–F8) 修订核验完毕：两项 BLOCKING 均真解——F1 census 改全量 9 处如实（与 R1 实测逐处比对零漂移）、V4 收窄 scope 复跑恰 3 处全在 T2 目标集内（landing 后清零可判）、六处描述性引用显式豁免附证成；F2 §3 补 rules 行 + profile 补「共 8 件」（逐件对 manifest 重数 8+2=10 全集，UNCLASSIFIED=0 成立）。MINOR 六条处置到位：F3 :32-35 整块替换钉死 + :35 删除语义去向写明；F4 governance 面语料与路由落点补齐；F5 建档前查重 + 退役安置留根补条款落点；F6 anchors 写者恒主线程 + 单写者规则 + L1 答不了去向全落（review-moderator 拦写断言复证为真）；F7 改判留痕随 T7 回写 ARQ；F8 T3 两行围栏钉死且不入锚集。修订面新矛盾扫描零命中；preflight 复跑绿（anchorUnion 十路径不变）、V6 基线复跑 0。残留 R2-M1/M2 两条措辞同步级 MINOR（T2 替换句语料枚举未含 governance 面、T1 第 2 块 faceMap「四面」定义未随第 3 块 governance 记入同步），一词一句级补写、不构成 packet 漂移，landing 前可顺手修或不修随首例 retro 收口，不阻断。逐 finding 证据与 R2 实测留痕见本目录 `review.md` R2 段。
