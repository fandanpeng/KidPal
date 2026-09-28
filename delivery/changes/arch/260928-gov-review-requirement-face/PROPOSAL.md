# PROPOSAL · 260928-gov-review-requirement-face

> **状态：FROZEN v1.0 · 2026-09-28 · 凭据 = human 立项 ack（2026-09-28 主线程会话）+ architect R1 BLOCK→R2 GREEN（[review.md](review.md)）+ human 裁 OQ-1~5（OQ-5 = (a)，判官 R2 后呈裁）**。主线程凭据自冻；M1–M4 已修毕后冻。
> **承载 ARQ**：`ARQ-260928-triage-cross-face-intake-gap`（本批 = 该 ARQ 的需求面基建前置部分；triage intake 四面申报接线 + `/arch-change` 退役安置留根给该 ARQ 后续批，见 §6）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（新增两个 agent 角色 + 一条新需求面变更流程；不触契约面、不动强制层；spec 面仅 §9 纪律注记两处句子级增补）。
> **review profile**：G2+G3（G2 = agents 定义 ×2 + reference ×1；G3 = 无——`scope-map.json` / hooks / settings 未触，见 §3 证成；spec 面一行注记随批携带）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/agents/story-author.md
.claude/agents/requirement-reviewer.md
.claude/references/requirement-change.md
docs/requirements.md
docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md
docs/crq/requirement/REQ-260916-home-date-format-english.md
docs/crq/requirement/INDEX.md
```

---

## 0. 想实现的效果与差距

**目标效果**：需求面（`docs/requirements.md`）从「只有写作纪律、没有变更流程」升级为「有起草/审稿角色对偶、有比例化三档流程、有定稿载体、REQ- 域 CRQ 有闭环」——需求变更不再依赖 human 记忆与人肉顺序，语义撞车在起草期被扫描，登记的改需求请求有接手人和销行条件。

**现状差距**：① `.claude/agents/` 现有 14 个 agent 定义（Glob 枚举：acceptance-author / adjudicator / architect / architect-reviewer / builder / challenger / defender / fixer / integrator / landing-verifier / planner / portfolio-planner / review-moderator / scenario-author），**无一需求面角色**——dev 侧有 acceptance-author 冻验收，需求侧没有对偶的起草与审稿角色；② requirements.md §9 自 v1.2 起 50+ 条变更几乎每条记「跳过 Track B，如实记录」（如 requirements.md:684 v1.55 行、:688 v1.51 行 verbatim「跳过 Track B，如实记录」）——名义有门、实际从未走过，因为门只有一档且过重，无比例化；③ `docs/crq/requirement/` 域唯一条目 `ARQ-260916-home-date-format-english` owner 为空（frontmatter `owner: ""`），处置 = 「由 human 亲自同步」，登记即终点；④ 语义撞车只有读 story 才可见——2026-09-28 主线程 walkthrough 中，human idea「查字页进入时显示完整打字、按 Key 播报」撞三处已钉 AC（S-F3.1 AC1 / S-F3.1 AC4 / S-F3.3 AC1），连走流程的主线程都漏查了架构面（ARQ-260928 证据 2 活体实录）。

**非目标**（防 scope creep）：① 不碰 `/triage-change` 与 `change-triage.md`——triage 升格跨面申报归 ARQ-260928 批；② 不动 `/arch-change` 本体（governance 面 / 契约预审仍由它承载，退役安置归 triage ARQ 批）；③ 不改 `write-story` SKILL 写作纪律（新 agent 正文引用它，不复制）；④ 不新增命令入口（human 已裁 B 方案：判据落 reference，将来 triage 统一路由来消费）；⑤ 不碰 `scope-map.json` / hooks / settings——新角色以同会话 return 交付、不设新写槽（§2 T1/T2 证成）。

## 2. 问题、根因与改法（T1–T6）

**【硬项 · 起草期实证义务】**本节断言纪律：凡 regex / 脚本行为断言 → 草稿即附实测；凡「对偶 / 比照先例」→ 先读先例引 file:line；凡「全 / 均 / 无」量化声明 → 覆盖集合机械枚举。本节引用实证：agents 目录枚举与脚本入口在册见 §0 证据段与 §5（Glob 实跑 2026-09-28）；§9 先例行 requirements.md:684/688 verbatim 已核；`REQ-` 域定义 docs/templates/crq-item.template.md:14（内容引用：requirement 域、前缀 `REQ-`、定义「改需求的请求与裁定状态，裁定人 = human，落地 = requirements.md §9 记账」——markdown 链接语法从略非逐字）；260923 批不改名裁定 delivery/changes/arch/260923-gov-review-arq-ssot-restructure/PROPOSAL.md:384 verbatim（「**保留原 ID `ARQ-260916-home-date-format-english` 不改名**（v0.4 修正：……由起草期自查抓出；`REQ-` 前缀从**下一条新登记**起启用，该条以 `legacy-prefix-off-domain` advisory 在册）」）；scope-map review-agent 写槽：`delivery/changes/arch/*gov-review-*/**` token 多处在册（scope-map.json:88/:102 等；architect-reviewer 条目 allow 为空系纯判官定位使然，判官写槽由 architect 自身条目 :86-90 支撑）。

**【硬项 · 目标文件锚清单】**每 T 末尾显式列目标文件；landing-manifest ≡ 各 T 并集（T5 迁移只记迁入路径，旧路径删除不进 manifest）。

### T1 · 新增 story-author agent（需求面起草角色）

**问题**：需求面无写入角色定义。主线程接到 human 需求意图后即兴起草 story 修订，write-story 纪律（五件结构 / 零机制词 / AC 三段式 / 锚行）靠临场记忆，无角色承载即无 per-agent 纪律约束；相邻 story 语义撞车（本批触发源 walkthrough 撞三处 AC）无人负责扫描。
**证据**：`.claude/agents/` Glob 枚举 14 个定义无需求面角色（2026-09-28 实跑）；ARQ-260928 证据 2 walkthrough 实录；对照先例：acceptance-author 定义（dev 侧「冻结验收测试」角色，`.claude/agents/acceptance-author.md`）——需求面无对偶。
**根因**：dev-loop 立角色时按「实现侧对偶」补齐了 acceptance-author，需求面从未立过角色；需求变更长期走 human 直改通道，角色缺位被通道缺位掩盖。
**改法**：新建 `.claude/agents/story-author.md`。职责边界四条：① **只起草不裁定**——把 human 需求意图原话转成符合 write-story 纪律的五件结构修订稿，不自评通过、不拍板落稿、不改语义裁定；② **起草动作三件**——write-story 完稿自检逐条过、`node .claude/scripts/spec-anchor-check.mjs` 跑绿、全量读相邻 story（同 Feature 节 + 锚行引用所及）出语义撞车报告；③ **档 2 增量**——出牵连面清单（关联 story / §2 共享底座 / 线框 / §6 路线图 / §8 已砍清单是否需同步）；④ **交付形态 = 同会话 return**——修订稿与撞车报告以 return 文本交主线程，不直接写 `docs/requirements.md`（落盘归主线程，见 T3 流程定义）。作者≠判官：与 requirement-reviewer（T2）成对偶，不得互兼。tools 白名单只读 + 机检执行（Read / Grep / Glob / Bash 限 `node .claude/scripts/spec-anchor-check.mjs`），无 Write——不设新写槽、不改 scope-map（riskTier 证成见 §3）。
目标文件：`.claude/agents/story-author.md`（新增）

### T2 · 新增 requirement-reviewer agent（需求面审稿角色）

**问题**：需求面无审稿角色。现有 challenger/defender/adjudicator 是 spec 层对抗判官（对整份 spec 冻结判定），不适配单条 story 修订的轻量审稿；结果 = 需求修订要么无人审（跳 Track B 直改先例）、要么动用整台对抗机器（过重，human 已裁不用）。
**证据**：challenger/defender/adjudicator 定义均自述「spec 层」（`.claude/agents/challenger.md` 等 description）；ARQ-260928 证据 4「需求面无门」。
**根因**：同 T1——需求面从未立角色体系；审稿职能被「human 直改」通道吸收。
**改法**：新建 `.claude/agents/requirement-reviewer.md`。职责边界四条：① **四查**——（a）意图保真：对着 human 意图原话逐条核修订稿是否如实承接；（b）纪律合规：write-story 完稿自检全项 + spec-anchor-check 绿；（c）撞车复核：独立重扫相邻 story 语义撞车（不信 story-author 的报告，独立取证）；（d）牵连面完整：改了 A 处、B 处同步没有（档 2 必查；携带牵连面清单的档 1 同查）。② **否回权**——任一查不过 → 逐条列否回项打回 story-author 重写，循环至过；③ **不代笔**——只出 review 结论与否回项，不改稿；④ **交付形态 = 同会话 return**——结论文本交主线程，不落 canonical 文件。tools 白名单：Read / Grep / Glob / Bash（限跑 `node .claude/scripts/spec-anchor-check.mjs`），无 Write。
目标文件：`.claude/agents/requirement-reviewer.md`（新增）

### T3 · 新增 requirement-change.md reference（三档判据 + 流程 + 定稿载体 + CRQ 闭环纪律）

**问题**：需求变更无比例化判据、无流程定义、无定稿载体——小措辞修订与整条 story 重写同通道（ARQ-260928 证据 4 verbatim「小措辞修订与整条 story 重写同通道」）；story 修订「定稿」无载体，实现包无处引用，②单包路径「上游未冻结不得 launch」无机检挂点。
**证据**：requirements.md §9 先例行形态（:684 等）；ARQ-260928 建议 2/3；CRQ 域定义 crq-item.template.md:14。
**根因**：需求面治理从未成文；唯一存在的 write-story 只管「怎么写」不管「谁写、谁审、写完怎么算定」。
**改法**：新建 `.claude/references/requirement-change.md`，内容五块：
1. **三档判据**——档 0 勘误/措辞（不改语义：错字、排版、表述澄清、锚行格式）；档 1 语义修改（AC 增删改 / 补充规则口径 / 单 story 内行为变更）；档 2 结构性（story 新增 / 重写 / 砍除、跨 story 连锁、NFR 新增/删除/阈值语义变更、Epic 级）。边界消歧两例：跨 story 零语义清扫（锚行格式批改、勘误连带同步）= 档 0；NFR 数值微调 = 档 1。**升档规则**：档 1 起草期发现牵连面（关联 story / §2 共享底座 / 线框需同步）即携带清单；牵连 ≥2 处或含 NFR/Epic → 升档 2。
2. **各档流程**——档 0：主线程直改 + 机检 + §9 记「勘误」行。档 1：human 裁意图（原话即起草依据）→ 主线程派 story-author 起草（修订稿 + 撞车报告以 return 交回）→ 主线程以 dispatch prompt 将「human 意图原话 + 修订稿 + 撞车报告」交 requirement-reviewer 四查 → 不过否回循环至过 → **过即落稿**（主线程写 requirements.md + §9 追加版本行；无 human 终确认——human 已裁，意图事前对齐 + reviewer 意图保真核查已覆盖，§9 append-only 可事后否回）。档 2：human 裁意图与取舍倾向 → story-author 起草（含牵连面清单）→ 派 architect 做可行性判断（可行性三态 ✅/🟡/🔴 + 实现代价 + 影响面，只出意见不裁语义；出场由 human 指定——本批不改 architect.md 入口清单，将其入列归后续批）→ story-author 按可行性修订（🔴/取舍点升回 human，不带病下行）→ requirement-reviewer 四查（重点查牵连面）→ **human 终拍**（architect 结论 + reviewer 结论一并呈）→ 主线程落稿 + §9 版本行；review 留痕 = §9 该行依据列浓缩承载 architect 结论 + reviewer 结论（OQ-5 已裁 (a)）。一句话分工：architect 管「能不能」、reviewer 管「准不准」、human 拍「要不要」。
3. **跳过例外**——human 明确说跳过流程的一次性操作允许直改（对齐 AGENTS.md Hard Rules 既有条款；human 已裁「允许跳过」），§9 记账须写明「跳过 story-author/reviewer」。
4. **定稿载体纪律**——story 修订定稿的标记 = **§9 版本行本身，版本号即 changeId**（不另铸批次 ID：档 1/2 流程不产生 TaskPackage 式批次，且 TaskPackage 已有强语义 changeId——task-intake-and-slicing.md:91-97，撞名禁用）；实现包引用 story 的合法形态（write-story 边界节锚点引用）之上追加「定稿版本号」可引用，为 ARQ-260928 批 launchPreconditions 推广到②路径提供挂点。本批不改 task-intake-and-slicing.md（留根 §6）。
5. **REQ- 域 CRQ 闭环纪律**——`docs/crq/requirement/` 条目必填 owner（指向需求变更流程某档或 human）/ trigger（何种事件触发捡起，禁「human 想起」形态）/ 销行条件（何种状态可销，通常 = 对应需求变更 §9 定稿）；本纪律落本 reference（human 已裁并进，不独立成件）。
目标文件：`.claude/references/requirement-change.md`（新增）

### T4 · requirements.md §9 纪律注记（定稿载体写入 spec 面）

**问题**：T3 定稿载体纪律若只住 reference，requirements.md §9 本体无标注——未来读 §9 的会话（主线程 / Pods）不知道版本行 + changeId 是定稿载体、不知道跳过须标注。
**证据**：requirements.md:677 §9 纪律行现状 verbatim：「纪律：本文件每次改动都在下表追加一行（新行在上，倒序）——版本号、日期、改了什么、依据。story 增删改、NFR 变动、砍项、结构重排必须有行；纯勘误也记（标「勘误」）。行只追加、不覆盖既有行。逐次详情可溯 delivery/changes/arch/** 对应制品。」
**根因**：§9 纪律行写于本批之前，无定稿载体概念。
**改法**：在 §9 纪律引言（requirements.md:677 段）追加两句（追加不覆盖既有句）：「语义修改（档 1）与结构性变更（档 2）的变更行以版本号为 changeId（不另铸 ID），该行即 story 修订的定稿载体，实现包引用以其为准；走 requirement-change.md 三档流程的变更，跳过流程的须在依据列写明『跳过 story-author/reviewer』。流程判据见 `.claude/references/requirement-change.md`。」零 story 语义改动，零正文行变动。
目标文件：`docs/requirements.md`（改——§9 纪律引言两处句子级增补）

### T5 · REQ- 前缀统一（ARQ-260916 改名 REQ-260916）

**问题**：`docs/crq/requirement/` 唯一条目用祖辈前缀 `ARQ-`（id: `ARQ-260916-home-date-format-english`），与域前缀规范 `REQ-`（crq-item.template.md:14）不一致；260923 批 v0.4 裁「迁移不改名、REQ- 自下一条新登记启用」（PROPOSAL.md:384 verbatim 已核），该条以 `legacy-prefix-off-domain` advisory 在册。human 2026-09-28 裁「REQ- 这个一起改」——**本项显式推翻 260923 批 v0.4 的不改名裁定**（human 权限内的新裁定，PROPOSAL 留痕）。
**证据**：frontmatter `id: ARQ-260916-home-date-format-english`（docs/crq/requirement/ARQ-260916-home-date-format-english.md:2 verbatim）；`docs/crq/requirement/` Glob 枚举仅此一条 + INDEX.md（2026-09-28 实跑）。
**根因**：该条先于三域账本存在，260923 迁移期按祖辈规则保留原名；现需求面成批立纪律，域内前缀统一时机成熟且条目量 = 1（改名成本最低点）。
**改法**：`git mv docs/crq/requirement/ARQ-260916-home-date-format-english.md docs/crq/requirement/REQ-260916-home-date-format-english.md` + frontmatter `id` 同步改 `REQ-260916-home-date-format-english` + 正文自引同步 → 跑 `node .claude/scripts/crq-render.mjs` 再生 `docs/crq/requirement/INDEX.md` → 跑 `node .claude/scripts/crq-doctor.mjs` 自检绿。祖辈前缀规则不破坏：gov-arq SKILL.md:31 verbatim「祖辈前缀 `ARQ-` 落任何域都放行，只出告警计数」——反向（REQ- 落 requirement 域）为本域正前缀，doctor 应绿。执行注记：V6 三归类清单中①更新类预计≈空（该条目无在册活引用，render 落地后核对），V6 执行时以实测命中清单为准、不预设命中。
目标文件：`docs/crq/requirement/REQ-260916-home-date-format-english.md`（新增·迁入）· `docs/crq/requirement/INDEX.md`（改·生成件再生）

### T6 · ARQ-260928 条目回写「建议 2 已由本批承载」注记

**问题**：本批 T3 完整承载 ARQ-260928 建议 2（需求面比例化入口），但该 ARQ 条目正文无注记——后续批（triage intake 接线批）只读条目时仍见建议 2 待做，存在跨批重做口子。
**证据**：ARQ-260928-triage-cross-face-intake-gap.md:27-31 建议 1–4 清单现状（建议 2 无承载注记）；append-only 纪律（条目正文只追加不覆盖）。
**根因**：ARQ 登记（2026-09-28 晨）先于本批起草（同日晚），登记时无法预知承载批次。
**改法**：在该条目正文末尾 append 一段：「**2026-09-28 注记**：建议 2（需求面比例化入口）已由 260928-gov-review-requirement-face 批 T3 承载（三档判据 + 流程落 `.claude/references/requirement-change.md`，定稿载体 = §9 版本行即 changeId）；建议 1/3/4 与 /arch-change 退役安置仍归本条目后续批。」只追加、不改既有行。
目标文件：`docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md`（改·append-only 注记）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| agent 定义 | `.claude/agents/story-author.md` | 新增 | G2；消费方 = 主线程 dispatch（档 1/2 起草） |
| agent 定义 | `.claude/agents/requirement-reviewer.md` | 新增 | G2；消费方 = 主线程 dispatch（档 1/2 审稿） |
| reference | `.claude/references/requirement-change.md` | 新增 | G2；消费方 = 主线程（需求变更流程）+ story-author / requirement-reviewer（分工边界）+ ARQ-260928 批（triage 接线消费判据） |
| spec 面 | `docs/requirements.md` | G2（注记级） | §9 纪律引言两处句子级增补；零 story 语义改动；spec-anchor-check 必须保持绿（§5） |
| CRQ 账本 | `docs/crq/gov/ARQ-260928-triage-cross-face-intake-gap.md` | G2（append-only 注记） | T6 回写建议 2 承载；只追加不改既有行 |
| CRQ 账本 | `docs/crq/requirement/REQ-260916-home-date-format-english.md` | 新增（迁入） | T5 迁移；旧 ARQ- 文件删除不进 manifest |
| CRQ 账本 | `docs/crq/requirement/INDEX.md` | G2（生成件再生） | crq-render.mjs 再生，不手改 |
| 指令根 | `AGENTS.md` / `CLAUDE.md` | 未触 | 留根：治理路由节补需求面一行归 ARQ-260928 批（§6） |
| 强制层 | `scope-map.json` / hooks / settings | 未触 | **证成**：story-author / requirement-reviewer 均无 Write、以同会话 return 交付——不需要新写槽。**如实披露**：两 agent 为全仓首批不在 scope-map 登记的 agent 定义（enforce-scope.mjs 对未登记 agent 无 per-agent 约束），其「无 Write / Bash 限跑 spec-anchor-check」系 agent 正文纪律约束、非机检强制——本批接受该约束为约定级（与 triage 门 convention-only 同性质），scope-map 登记归 ARQ-260928 批（§6）。真同形先例 = architect-reviewer.md:4 无 Write 的 tools 行 |
| skill | `.claude/skills/write-story/**` | 未触 | 写作纪律原样；新 agent 正文引用不复制 |

**【硬项 · sweep 透镜义务】**本批不落地新门/新断言脚本（requirement-change.md 是流程 reference，非机检脚本），无判定 pattern 预跑义务。**【硬项 · 复扫模式完备性先验】**T5 改名残留清扫：pattern = **全 ID** `ARQ-260916-home-date-format-english`（禁截断串——截断会撞 4 个无关同日 CRQ 条目及其在 hooks/scripts 注释、pytest.ini、workstreams、dev 制品中的引用，R1-F1 实测在案）。命中清单逐条三归类：① **更新类**——活引用（frontmatter 互引、关联条目正文中的指称）→ 同步改 `REQ-`；② **append-only 历史类**——`docs/crq/**` 既有条目正文历史行、`delivery/changes/**` 既有制品 → 不改写，命中记录进 landing-summary；③ **G3 注记类**——`crq-migrate.mjs:32` 的迁移脚本映射注记 → 不改（脚本属强制层、本批未触面），登记不硬治。**销项 = 三归类清单之外零命中**（①类命中全数更新；②③类命中逐条留痕归类即合规——非「①类之外零命中」）。

## 4. human 取舍点（OQ）

| OQ | 题 | 裁定 |
|---|---|---|
| OQ-1 | 档 1 要不要 human 终确认 | **已裁（2026-09-28 主线程会话）**：不用——human 在改之前应把修改意图对齐，reviewer 核「按意图改的」即可，过审即落稿，§9 append 可事后否回 |
| OQ-2 | REQ- 域 CRQ 纪律落点 | **已裁（同上）**：要落；落点按主线程解读 = 并进 `requirement-change.md`（需求面流程一个 reference 写全）——**立项 ack 时请顺带确认此解读** |
| OQ-3 | 档 0「human 明确跳过流程」例外 | **已裁（同上）**：允许跳过；§9 须记「跳过」 |
| OQ-4 | REQ- 前缀统一 | **已裁（同上）**：一起改（T5）——显式推翻 260923 批 v0.4 不改名裁定 |
| OQ-5 | 档 2 review 留痕载体 | **已裁（2026-09-28，判官 R2 后呈 human）**：(a) §9 依据列承载浓缩结论（零新落点、与先例同形）；独立制品化整体归 ARQ-260928 批 |

## 5. 验证集（landing 前机械前置 + landing 后核验）

| # | 时点 | 命令 | 预期 |
|---|---|---|---|
| V1 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260928-gov-review-requirement-face/PROPOSAL.md` | 形态零错；anchorUnion 与 manifest 七路径逐一吻合、无注记夹带路径 token |
| V2 | pre-commit | `node .claude/scripts/lint-descriptions.mjs` | exit 0（告警级；新增两 agent 定义 description 无流程叙述气味、无超长） |
| V3 | pre-commit | `node .claude/scripts/route-regression-check.mjs` | 绿——保障**既有断言不退化**。如实注记：该脚本断言由 `route-fixtures/routing.json` 驱动，对新增 agent 无既有断言，本命令绿 ≠ 新 agent 路由信号在位（真空绿风险）；新 agent 路由信号的替代保障 = V2 lint-descriptions + landing-verifier 逐文件人读核对 description 纪律（authoring-agents-skills rule 必留路由信号清单）。fixtures 补两新 agent 条目归 ARQ-260928 批（§6） |
| V4 | pre-commit | `node .claude/scripts/spec-anchor-check.mjs` | 绿——T4 改动后 UI 锚 15/15 对账不破 |
| V5 | pre-commit | `node .claude/scripts/crq-render.mjs && node .claude/scripts/crq-doctor.mjs` | render 幂等再生 INDEX 零 diff；doctor 绿（REQ- 前缀落 requirement 域为本域正前缀） |
| V6 | pre-commit | Grep `ARQ-260916-home-date-format-english`（全仓，全 ID） | 命中清单逐条三归类（§3 复扫硬项）；**销项 = 三归类清单之外零命中**（①类全数更新、②③类逐条留痕即合规），归类清单进 landing-summary；①类以实测为准不预设命中 |
| V7 | post-commit | `git status --short docs/crq/requirement/` | 旧 ARQ- 文件已删（git mv 在案）、REQ- 文件与再生 INDEX 在册 |

## 6. 留根清单

- **triage intake 四面申报接线 + 消费本批三档判据** → ARQ-260928 批（该 ARQ 建议 1/3/4）。
- **`/arch-change` 退役安置**（governance 面 / spec 剩余流量归 triage 统一路由）→ 同上，独立治理变更。
- **AGENTS.md 治理路由节补需求面一行** → 同上（本批不碰指令根，控制 profile）。
- **两新 agent 的 scope-map 登记 + route-fixtures/routing.json 断言补条** → ARQ-260928 批（本批两 agent 以正文纪律约束，约定级；机检化随统一路由批一并做）。
- **需求面 review 制品化**（档 2 独立 review 制品 + 需求面 change 目录 + lane 写槽）→ ARQ-260928 批（OQ-5 已裁 (a)：本批以 §9 依据列承载，制品化整体归后续批）。
- **`task-intake-and-slicing.md` 补「定稿版本号引用」合法形态** → 归拆包链治理下批（本批只立载体，不改拆包链文本）。
- **语义撞车机检脚本**（AC 文本相似度 / 键词矩阵）→ 留根 ARQ：先人工（story-author 全量读相邻 story），机检候后续立项。
- **REQ- 域 status 体系与新流程销行的联动细节**（如 accepted-risk 条目是否免走流程直接销）→ 首个新 REQ- 条目时实证再定。
- **architect.md When to Use 入口清单补档 2 可行性判断条目** → 后续治理批（本批档 2 以「human 指定」口径运转，不碰 architect.md 自改递归边界）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
