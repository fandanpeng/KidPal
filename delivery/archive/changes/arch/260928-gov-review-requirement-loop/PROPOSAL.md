# PROPOSAL · 260928-gov-review-requirement-loop

> **状态：FROZEN v1.0 · 2026-09-29 · 凭据 = human 立项 ack（2026-09-28 主线程会话，OQ-1~4 同批裁定全按推荐位，见 §4）+ architect R1 BLOCK(F1,F2)→R2 BLOCK(F8)→R3 GREEN（[review.md](review.md)，三轮内收口未触熔断）+ human 裁 OQ-1~4 已记 §4**。主线程凭据自冻，授权记本 Status 行。
> **承载 ARQ**：无独立 ARQ 根——本批 = human 2026-09-28 需求面全流程实跑复盘（260928-char-window-key-triggered-play spine 走毕）后的直接立项；与 260928-gov-review-requirement-face 批（三档流程 v1.0）为同面迭代关系，见 §0。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T7）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（触强制层 scope-map 写槽扩张一次 + 指令根单行 + spec 面状态行一行；不动 hooks / scripts / verify-gates；新命令为 command 文面流程、非机检闸）。
> **review profile**：G2 + G3 + G1 单行显式呈报（G2 = agents ×2 + reference ×1 + command ×1；G3 = scope-map.json；G1 = AGENTS.md 需求面变更节单行，OQ-4 裁 (a) 时入，裁 (b) 时 profile 收窄为 G2+G3——契约面恒为零）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/references/requirement-change.md
.claude/agents/story-author.md
.claude/agents/requirement-reviewer.md
.claude/scope-map.json
.claude/commands/requirement-change.md
docs/requirements.md
AGENTS.md
```

---

## 0. 想实现的效果与差距

**目标效果**：需求面变更从「主线程按 reference 手工编排的文档流程」升级为「有命令入口、有盘上状态机、有机械写槽强制的可执行 loop」——story-author 直接在 `docs/requirements.md` 上以 updating 态起草，requirement-reviewer 审盘上在途稿，主线程只做状态翻转与记账；档 2 的 architect 可行性判断必派且产出「逐 story/AC 技术锚 + 影响面概貌」，供下游 triage / planner 前置评估消费；updating 态的 requirements.md 不得被下游（triage / taskpackage / launchPreconditions）引用。

**现状差距**（260928 实跑实录 + 文件取证，2026-09-28）：

1. **无 loop 入口**：需求面流程 SSOT 是 `.claude/references/requirement-change.md`（三档判据 + 各档流程），但执行全靠主线程读文推进——`.claude/commands/` Glob 枚举 6 个命令（arch-change / loop-retro / review-arch / review-governance / triage-change / build-feature，2026-09-28 实跑），无一需求面入口；headless 启动的主 agent 无命令可挂，每次编排质量取决于当次会话对 reference 的理解。
2. **story-author 无 Write，落稿靠主线程代笔**：`requirement-change.md:26` 档 1 流程 verbatim「story-author 起草——修订稿 + 完稿自检 + spec-anchor-check + 相邻 story 撞车报告，同会话 return 交主线程」、`:58` 角色分工表 verbatim「不裁定、不自评、无 Write」；`story-author.md:29` verbatim「同会话 return（本 agent 无 Write，不落任何文件）……主线程负责转交 requirement-reviewer 与落盘」。修订稿经主线程转手落盘，既多一次中转，也产生「审的稿 ≠ 落的稿」的漂移口子；architecture.md 镜像句也由主线程代写（260928 spine 实录）。
3. **无在途态标记**：requirements.md 头部只有 `:7` verbatim「> 当前版本 **v1.59**；版本历史与逐次变更见文末 §9「变更记录」。」——起草期、审稿期、终拍期的文件状态不可见，第三方（另一个会话、一个 Pod）读盘无法判断 requirements.md 当前是否处于变更在途，也没有任何机制阻止下游引用一份正在被改写的 spec 面。
4. **档 2 architect 可出场可不出场**：`requirement-change.md:34` verbatim「3. **architect 可行性判断**（出场由 human 指定）」——260928 spine 实跑即触发免派（openQ Q4：human 未指定出场，主线程判定在先 PROPOSAL 已覆盖）。技术可行性锚（能不能实现、影响哪些面）目前只能在 dev-loop 侧 planner Step 1g 档才首次产出，需求面与实现面的影响面判断脱节。
5. **审稿对象是转交文本而非盘上文件**：`requirement-reviewer.md:3` description verbatim「story-author 交回修订稿后要四查」——审的是 return 文本；落稿时主线程再写盘，两份载体无人对账。

**与 260928-gov-review-requirement-face 批的关系**：该批立了三档流程、两角色、定稿载体（§9 版本行即 changeId）与 REQ- 域 CRQ 纪律——方向全部保留；本批不改三档判据、不改定稿载体、不改 REQ- 纪律，只把「交付与落稿机制」从 return 文本换成盘上状态机，并补 loop 入口、写槽强制与 architect 必派。该批留根「两新 agent 的 scope-map 登记」「AGENTS.md 治理路由节补需求面一行」恰由本批承接（前者 scope-map.json:125-128 已在册但 allow 为空——`"story-author": { "allow": [], "deny": ["**"] }` verbatim，2026-09-28 实读；后者见 T7）。

**非目标**（防 scope creep）：① 不改三档判据与升档规则（requirement-change.md §三档判据 原样）；② 不改定稿载体（§9 版本行即 changeId，状态行只是可见态、不是第二定稿标记）；③ 不动 `write-story` skill 写作纪律；④ 不碰 `docs/contracts/**`（零触碰）；⑤ 不新增机检脚本、不改既有脚本（消费保护齿落点见 OQ-3，推荐本批只立规则成文）；⑥ 架构面独立变更 loop 不在本批（human 已裁分步走：需求面 loop 先跑实，架构档复用同骨架后续挂入，留根 §6）；⑦ 固件 runner 选齿 + known-red 清单化不在本批（human 2026-09-28 裁「固件的先不管了」）。

## 2. 问题、根因与改法（T1–T7）

**【硬项 · 起草期实证义务】**本节断言纪律：凡 file:line / verbatim 引据 → 起草期实读留痕；凡「全 / 无」枚举声明 → 机械枚举附命令与日期。本节引用实证均在 2026-09-28 实读取得：commands 目录 Glob 枚举 6 命令（§0 差距 1）；`requirement-change.md:19/:26/:28/:34/:58`、`story-author.md:3/:29`、`requirement-reviewer.md:3/:13/:25`、`scope-map.json:125-128`、`requirements.md:7`、`AGENTS.md:26` 各 verbatim 见对应 T 证据段。

**【硬项 · 目标文件锚清单】**每 T 末尾显式列目标文件；landing-manifest ≡ 各 T 并集（T7 为条件锚，OQ-4 裁 (b) 时从 manifest 剪除）。

### T1 · requirement-change.md 流程改版：盘上状态机 + 盘上审稿 + architect 必派

**问题**：三档流程的交付机制是「return 文本 + 主线程落盘」，无在途状态、无消费保护、档 2 architect 可免派——需求面变更在盘上不可见、下游可引用在途 spec、技术可行性锚缺位（§0 差距 2/3/4/5）。
**证据**：`requirement-change.md:26`（档 1 story-author 同会话 return）、`:28`（过即落稿=主线程写）、`:34`（architect 出场由 human 指定）、`:58`（角色分工表 story-author 无 Write）、`:61`（主线程「派发 + 落盘 + §9 记账」）——均 2026-09-28 实读 verbatim。
**根因**：260928-gov-review-requirement-face 批立项时以「return 交主线程」换取零 scope-map 触碰（该批 §3 强制层行如实披露「两 agent 无 Write、约定级约束」）；状态机与消费保护当时不在问题域内，实跑一单后摩擦显形。
**改法**：改 `.claude/references/requirement-change.md` 五处，三档判据与定稿载体两节不动：
1. **头部状态机**（新节）：requirements.md 头部版本区设状态行（形态见 T6），值 ∈ {`frozen`, `updating`}。story-author 动笔即置 `updating` 并注在途版本号与日期；翻 `frozen` 只归主线程——档 1 = requirement-reviewer PASS 后（沿用该批 OQ-1 既有裁定：档 1 无 human 终确认），档 2 = human 终拍后；翻转同时 append §9 版本行（版本行仍是定稿唯一标记，状态行只是盘上可见态）。档 0 勘误不走状态机（主线程直改即 frozen，勘误无在途态）。**边界场景两句成文**（R1 F5）：① updating 在途期档 0 勘误不并行执行——勘误等 frozen 后补做，或并入在途变更随本批落（主线程裁其一，§9 依据列留痕）；② updating 会话中断的弃/续由主线程在下次入口裁——弃 = 在途改动段经 git 还原、状态行恢复 frozen、不 append §9 版本行；续 = 重新派 author 接着改、状态保持 updating。
2. **档 1/档 2 流程步骤改写**：story-author 起草 = 直接编辑 requirements.md（updating 态）+ 自检 + 撞车报告以 return 交主线程（稿在盘上，return 只带报告与自检结果）；requirement-reviewer 四查对象 = 盘上 updating 态文件（不再审转交文本），主线程 dispatch prompt 的输入构成**显式含 author 产出的改前/改后句级对照**（R1 F2，见 T2 ②）——reviewer 靠对照与盘上文件两相核对做四查 1 意图保真的夹带检测；否回循环 = story-author 再改盘上文件，状态保持 updating；落稿 = 主线程翻 frozen + §9 记账。主线程职责由「派发 + 落盘 + §9 记账」改「派发 + 状态翻转 + §9 记账」（`:61` 分工表同步改）。
3. **档 2 architect 必派**：`:34`「出场由 human 指定」改「默认必派」；产出由「可行性三态 + 实现代价 + 影响面」扩为固定三件——可行性三态（✅/🟡/🔴）、**逐 story/AC 技术锚**（每个受影响 story/AC 的可实现性判据与实现要点一句话）、**影响面概貌**（牵连的实现面文件/子系统清单，供下游 triage 分档与 planner Step 1g 前置评估消费）。免派例外保留一条：human 明示可行性已有在先评估承载时免派，免派事由记 §9 该行依据列（260928 spine openQ Q4 形态成文化）。
4. **消费保护规则**（新节）：updating 态的 requirements.md 不得被下游引用——triage 分流、taskpackage 撰写（story 引用）、launchPreconditions 消费一律只认 frozen 态（机械判据 = §9 对应版本行在位）；本批该规则成文 + §5 V5 grep 齿，引用校验机检齿落点归 OQ-3。
5. **角色分工表**：story-author 行「不裁定、不自评、无 Write」改「不裁定、不自评、写面限 requirements.md + architecture.md（scope-map 强制）」；requirement-reviewer 行审稿对象同步注「盘上 updating 态文件」；human 行增「档 1 事后否回权不变」。
目标文件：`.claude/references/requirement-change.md`（改）

### T2 · story-author agent 定义改版：交付形态改盘上写

**问题**：story-author 无 Write、交付靠 return 文本转交——多一跳中转、多一个漂移口子（§0 差距 2）。
**证据**：`story-author.md:3` description verbatim「修订稿以同会话 return 交主线程，本 agent 无 Write、不直接改 spec 面」；`:29` verbatim「同会话 return（本 agent 无 Write，不落任何文件）：修订稿全文 + 自检结果 + 撞车报告 + （档 2）牵连面清单。主线程负责转交 requirement-reviewer 与落盘。」
**根因**：同 T1 根因——260928 批以无 Write 换零强制层触碰；当时 Write 的风险（落稿先于审稿、author 自行发布）由「无 Write」单点承接，状态机（T1）建立后该风险改由「updating 态不可消费 + frozen 翻转只归主线程」承接，Write 可以归还。
**改法**：改 `story-author.md` 四处：① description 交付形态句改为「修订直接写入 docs/requirements.md（updating 态），审稿归 requirement-reviewer 审盘上在途稿，本 agent 写面限 requirements.md 与 architecture.md（scope-map 强制）、frozen 翻转不归本 agent」；② 「交付形态」节改为「盘上交付：修订直接编辑 requirements.md（头部状态行置 updating），return 交主线程的内容 = 自检结果 + 撞车报告 + （档 2）牵连面清单 + 已改锚行清单 + **改前/改后句级对照**（逐修订段「原句 → 新句」两行式，R1 F2——reviewer 四查 1 意图保真夹带检测的输入，return 不再携带修订稿全文但对照不可省）」；③ 「硬边界」节「不改 docs/requirements.md」句改为「只改 docs/requirements.md 与 docs/architecture.md（镜像句同批；scope-map allow 之外零写），不改 docs/contracts/**，不改 write-story skill」——落稿裁定（frozen 翻转、§9 记账）仍不归本 agent；④ frontmatter `tools` 行 `Read, Grep, Glob, Bash` 改 `Read, Edit, Write, Grep, Glob, Bash`（R1 F1——enforce-scope 的 per-agent 拦截只对 Edit/Write/MultiEdit 生效，tools 无写工具则 T4 的路径级写槽不可达，盘上写机制整体失效；工具级授出 × scope-map 路径级圈定双层合围。**Bash 旁路如实表述（R2 F8 勘正）**：两 allow 路径的 Bash 重定向写不在 universal.bashDenyWrite 覆盖内（scope-map.json:25 该清单仅 contracts / .claude / taskpackages 三项），旁路约束 = agent def「Bash 仅限跑 spec-anchor-check」白名单（约定级，本改法不触碰该 bullet、落地后仍在）；Bash 旁路机拦属新齿，归 OQ-3 留根同流）。职责边界「只起草不裁定」「不自评通过」两条不动。
目标文件：`.claude/agents/story-author.md`（改）

### T3 · requirement-reviewer agent 定义小改：审稿对象改盘上文件

**问题**：reviewer 审的是转交文本而非盘上文件，落稿载体无人对账（§0 差距 5）。
**证据**：`requirement-reviewer.md:3` description verbatim「Use when: story-author 交回修订稿后要四查」；`:25` verbatim「同会话 return（本 agent 无 Write）：结论 PASS / FAIL。」
**根因**：同 T1——上游交付形态是 return 文本，审稿输入只能跟着是文本。
**改法**：改 `requirement-reviewer.md` 两处：① description 的「story-author 交回修订稿后」改「story-author 在 docs/requirements.md 落出 updating 态修订后」；② 审稿输入节（四查前置）加一句「审稿对象 = 盘上 updating 态的 requirements.md（以盘上文件为准，不审转交文本）」。「只审不写」「不拍板落稿」「同会话 return 结论 PASS / FAIL」三条不动——reviewer 仍无 Write，不因本批获得写权。
目标文件：`.claude/agents/requirement-reviewer.md`（改）

### T4 · scope-map 登记 story-author 写槽

**问题**：T2 归还 Write 后，若 scope-map 不登记写槽，story-author 的写面约束仍停留在 agent 正文纪律（约定级）——enforce-scope 钩子对 scope-map 条目按 allow/deny 机判，allow 为空 + deny `**` 意味着任何写都被拦，agent 正文允许写盘会被钩子物理拦截，流程走不通；反之不登记则约束无机检。
**证据**：`scope-map.json:125-128` verbatim：
```
"story-author": {
  "allow": [],
  "deny": ["**"]
},
```
（2026-09-28 实读；260928-gov-review-requirement-face 批 §3 强制层行自述「两 agent 为全仓首批不在 scope-map 登记的 agent 定义……本批接受该约束为约定级，scope-map 登记归 ARQ-260928 批」——实际登记已由该批后续落地为 deny-all 占位条目，本批是首次授 allow。）
**根因**：无 Write 的 agent 不需要写槽；T2 授 Write 后写槽从「可选」变「必办」。
**改法**：`scope-map.json` story-author 条目改为 `allow: ["docs/requirements.md", "docs/architecture.md"]`、`deny: ["docs/contracts/**"]`（契约 deny 在 universal 层已有，条目内显式重申防漏；两 allow 路径与 OQ-1 裁定联动——裁 (b) 时剪除 architecture.md 一项）。requirement-reviewer 条目不动（维持 deny `**`，reviewer 恒无 Write）。
目标文件：`.claude/scope-map.json`（改）

### T5 · 新增 /requirement-change 命令（loop 入口）

**问题**：需求面流程无命令入口——headless 启动的主 agent 无命令可挂，每次编排靠当次会话读 reference 临场发挥；与 dev-loop 的 `/build-feature` 入口形态不对称（§0 差距 1）。
**证据**：`.claude/commands/` Glob 枚举 6 命令无需求面入口（2026-09-28 实跑，清单见 §0）；对照先例 `build-feature.md` 命令形态（「你现在是 planner（主线程编排者）」开场 + 逐步指令 + 显式派发说明）。
**根因**：需求面流程立批时 human 已裁「不新增命令入口」——**本项显式推翻 260928-gov-review-requirement-face 批非目标④**（推翻标签按先例形态：该批 T5 对 260923 批不改名裁定的推翻写法；human 权限内的新裁定，PROPOSAL 留痕）。原裁定语境：判据落 reference、将来 triage 统一路由消费——彼时流程尚未实跑；loop 化诉求来自 260928 实跑复盘，属新裁定。
**改法**：新建 `.claude/commands/requirement-change.md`。工作流照 `/build-feature` 形态写：开场定「你现在是主线程编排者，按 `.claude/references/requirement-change.md` 推进需求变更」+ 逐步指令——① 档位判定（0/1/2，按三档判据；存疑向严升档）；② 档 0 直改 + 机检 + §9 记「勘误」行收束；③ 档 1：派 story-author（盘上 updating 起草）→ 派 requirement-reviewer 审盘上文件 → 否回循环至 PASS → 主线程翻 frozen + §9 记账；④ 档 2：同档 1 且 architect 必派（可行性三态 + 逐 story/AC 技术锚 + 影响面概貌）插在起草与终拍之间、🔴/取舍点升 human → human 终拍 → 主线程翻 frozen + §9 记账；⑤ 每次翻转/落稿跑 `node .claude/scripts/spec-anchor-check.mjs` 须绿；⑥ updating 态不得进入下游消费（triage / taskpackage / launchPreconditions）。`$ARGUMENTS` 承接 human 意图原话；缺意图时 fail-closed 回问 human，不脑补。命令文面流程、非机检闸（对齐 build-feature.md 先例）；description 按 authoring-agents-skills 纪律写（when-to-use + 路由信号，不叙述流程正文）。
目标文件：`.claude/commands/requirement-change.md`（新增）

### T6 · requirements.md 头部状态行 + §9 纪律句增补（spec 面携带）

**问题**：状态机（T1）需要 spec 面有一个承载点——头部无状态行，在途态在盘上不可见（§0 差距 3）。
**证据**：`requirements.md:7` verbatim「> 当前版本 **v1.59**；版本历史与逐次变更见文末 §9「变更记录」。」（头部引用区当前 blockquote 五行，:3–:7，均为溯源/范围/版本说明行——R1 F6 计数勘正）。
**根因**：spec 面从未有在途态概念——变更直改直落，无需状态；盘上状态机是新机制，需要 spec 面一行承载。
**改法**：两处句子级增补（零 story 语义改动、零既有行覆盖）：① `:7` 当前版本行之后追加一行 blockquote：`> 状态：frozen · v1.59 · 2026-09-28（frozen = 可被下游引用；updating = 需求变更在途、下游禁引——流程见 .claude/references/requirement-change.md）`；② §9 纪律引言（260928-gov-review-requirement-face 批 T4 增补的定稿载体句之后）追加一句：「头部状态行是盘上在途态标记：story-author 动笔置 updating、主线程在落稿时翻 frozen，翻 frozen 以 §9 版本行在位为准；updating 态本文件不得被实现包 / triage / launchPreconditions 引用。」落稿时点（本批 landing）状态行即以 frozen 态落。
目标文件：`docs/requirements.md`（改——头部状态行一行新增 + §9 纪律句一句增补）

### T7 · AGENTS.md 需求面变更节入口行更新（条件项：OQ-4）

**问题**：AGENTS.md「需求面变更」节只指向 requirement-change.md 三档流程，无 loop 入口——指令根与流程实态漂移（会话读指令根不知道 /requirement-change 存在）。
**证据**：`AGENTS.md:26` verbatim「**需求面变更**：走 requirement-change.md 三档流程（story-author / requirement-reviewer 对偶起草与审稿，260928-gov-review-requirement-face 立；SSOT `.claude/references/requirement-change.md`）。」
**根因**：260928-gov-review-requirement-face 批留根「AGENTS.md 治理路由节补需求面一行」（其 §6）；本批落地 loop 入口后，该行更新时机成熟。
**改法**：`:26` 行改写为（追加不删除既有凭据信息）：「**需求面变更**：走 `/requirement-change` loop（260928-gov-review-requirement-loop 立；流程 SSOT `.claude/references/requirement-change.md`，story-author 盘上 updating 起草 + requirement-reviewer 审稿 + 档 2 architect 必派〔免派例外见 SSOT〕；260928-gov-review-requirement-face 立三档判据与定稿载体）。」G1 面单行触碰，OQ-4 显式呈报 human。
目标文件：`AGENTS.md`（改——需求面变更节单行改写；OQ-4 裁 (b) 时本 T 剪除、AGENTS.md 从 manifest 移除）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| reference | `.claude/references/requirement-change.md` | G2 | 流程 SSOT 改版（状态机 + 盘上审稿 + architect 必派 + 消费保护）；消费方 = 主线程（/requirement-change 推进依据）+ story-author / requirement-reviewer（分工边界） |
| agent 定义 | `.claude/agents/story-author.md` | G2 | 交付形态改盘上写（tools 行增 Edit/Write——R1 F1；写面由 T4 scope-map 路径级圈定）；消费方 = 主线程 dispatch |
| agent 定义 | `.claude/agents/requirement-reviewer.md` | G2 | 审稿输入改盘上文件；消费方 = 主线程 dispatch |
| 强制层 | `.claude/scope-map.json` | G3 | story-author 写槽授 allow 两路径（enforce-scope.mjs 消费）；requirement-reviewer 条目不动 |
| command | `.claude/commands/requirement-change.md` | G2 | 新增 loop 入口；消费方 = human / headless 主 agent |
| spec 面 | `docs/requirements.md` | G2（注记级） | 头部状态行 + §9 纪律句；零 story 语义改动；spec-anchor-check 必须保持绿（§5 V4） |
| 指令根 | `AGENTS.md` | G1（单行，显式呈报） | 需求面变更节入口行改写；OQ-4 裁 (b) 时剪除 |
| 契约面 | `docs/contracts/**` | 未触 | 本批零触碰（需求面流程批，无契约改动形状） |
| hooks / scripts / verify-gates | `.claude/hooks/**`、`.claude/scripts/**`、`.claude/verify-gates.json` | 未触 | **证成**：消费保护本批成文 + grep 齿（OQ-3 推荐 (a)），不新增脚本、不改 spec-anchor-check——状态行在 `>` 引用区，anchor-check 扫描面为锚行与 §2 结构，头部 blockquote 行不在其判定 pattern 内（V4 实跑证伪点） |

**【硬项 · sweep 透镜义务】**本批不落地新门/新断言脚本，无判定 pattern 预跑义务。**【硬项 · 复扫模式完备性先验】**本批无退役语面清扫；唯一残留风险 = 「无 Write / 同会话 return」旧表述残留——复扫模式集：`同会话 return`、`无 Write`、`不落任何文件`、`交回修订稿`（词形 × 有/无 backtick），目标文件仅三个（requirement-change.md / story-author.md / requirement-reviewer.md），逐文件 grep 核改后仅新语义句命中（§5 V6）；requirement-change.md 保留的「无 Write」句（reviewer 行、跳过例外段）为合法存活，逐条归类留痕 landing-summary。

## 4. human 取舍点（OQ）

| OQ | 题 | 裁定 |
|---|---|---|
| OQ-1 | story-author 写面是否含 `docs/architecture.md`（镜像句同批） | **已裁（2026-09-28 主线程会话，随立项 ack）**：(a) 含——镜像句是 requirements 口径的逐字镜像，author 一次写齐、消灭主线程代写中转（260928 spine 实录的摩擦点）；scope-map 同槽授两路径 |
| OQ-2 | 档 1 frozen 翻转是否沿用「reviewer PASS 即翻、无 human 终确认」 | **已裁（同上）**：(a) 沿用——状态机不改变 260928-gov-review-requirement-face 批 OQ-1 既有裁定（意图事前对齐 + reviewer 意图保真核查已覆盖；§9 append-only 可事后否回） |
| OQ-3 | 消费保护机检齿落点 | **已裁（同上）**：(a) 本批只立规则成文 + V5 grep 齿——克制新齿，先跑实包看摩擦再机检化 |
| OQ-4 | AGENTS.md 需求面变更节更新是否本批携带 | **已裁（同上）**：(a) 携带——指令根与流程 SSOT 漂移防护，单行成本；G1 触碰已在本 Status 头显式呈报 |

## 5. 验证集（landing 前机械前置 + landing 后核验）

| # | 时点 | 命令 | 预期 |
|---|---|---|---|
| V1 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260928-gov-review-requirement-loop/PROPOSAL.md` | 形态零错；anchorUnion 与 manifest 路径逐一吻合、无注记夹带路径 token |
| V2 | pre-commit | `node .claude/scripts/lint-descriptions.mjs` | exit 0（告警级）；story-author / requirement-reviewer 两 description 改后无流程叙述气味、无超长；新命令 description 同查 |
| V3 | pre-commit | `node .claude/scripts/route-regression-check.mjs` | 绿——既有断言不退化（真空绿风险同 260928 批 V3 注记：fixtures 无新命令断言，路由信号由 V2 + landing-verifier 人读核对兜） |
| V4 | pre-commit | `node .claude/scripts/spec-anchor-check.mjs` | 绿——T6 头部状态行与 §9 句增补后锚对账不破（60/46/13/31 基线维持） |
| V5 | pre-commit | `grep -n "^> 状态：" docs/requirements.md` | 恰一行，`frozen` 态在位——**名实注记（R1 F3）**：本项是状态行在场与形态自检；消费保护（updating 下游禁引）按 OQ-3 裁 (a) 本批为约定级成文（requirement-change.md 新节 + §9 纪律句），不由本 grep 承载 |
| V6 | pre-commit | 复扫三文件 grep（模式集见 §3 硬项：`同会话 return`、`无 Write`、`不落任何文件`、`交回修订稿`） | 仅新语义句与归类存活句命中（reviewer 无 Write 句、跳过例外段等），逐条归类进 landing-summary |
| V7 | post-commit | enforce-scope 行为探针（landing 时实测留痕）：story-author 写 `docs/requirements.md` / 写 `docs/contracts/**` 各一发 | 前者放行、后者拦截（scope-map T4 生效的行为验证，对齐 dev-loop.md §4 行为验证先例） |

## 6. 留根清单

- **消费保护机检齿**（taskpackage-required-check 校验引用版本号在 §9 在位）→ OQ-3 裁 (a) 时留根：实包跑 2–3 单后按摩擦决定机检化批次。
- **route-fixtures/routing.json 补 /requirement-change 断言** → 下批治理批顺带（同 260928 批 V3 留根形态）。
- **架构档挂入同一 loop 骨架**（架构面独立变更的 loop 化，author 侧是否从主线程拆独立 agent）→ 需求面 loop 实跑 2–3 单后立项（human 2026-09-28 裁分步走）。
- **launchPreconditions 消费 updating/frozen 状态的接线** → 归 ARQ-260928-triage-cross-face-intake-gap 批既有留根（launchPreconditions 挂点），本批状态机为其提供判据。
- **architect.md When to Use 入口清单补「档 2 需求面可行性判断」条目** → 后续治理批（architect self-def 边界，不碰）。
- **状态机与 intake spine anchors 的联动**（spine anchors.req 记录 frozen 版本号还是状态）→ 首个跨面 spine 走新流程时实证再定。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-29 - architect

verdict: BLOCK(F1,F2)

R1 全面审毕。引据抽验 22 项全部亲读吻合（requirement-change.md:19/:26/:28/:34/:58/:61、story-author.md:3/:29、requirement-reviewer.md:3/:25、scope-map.json:125-128、requirements.md:7、AGENTS.md:26、commands 六命令枚举、build-feature.md:8 先例形态、§9:680 定稿载体句、requirement-face 批 §3/非目标④/OQ-1、spine openQ Q4 均 verbatim 核对）；manifest ≡ 七 T 目标锚并集逐一吻合；V1 preflight 实跑 ok（anchorCount 7）、V3 实跑 36 ok / 0 fail、V4 实跑基线 60/46/13/31 逐项吻合、V2 基线 exit 0。两项 BLOCKING：F1 = T2 遗漏 story-author frontmatter `tools` 行补 Write/Edit（现行 `tools: Read, Grep, Glob, Bash` 无写工具，enforce-scope per-agent 拦截仅对 Edit/Write/MultiEdit 生效——scope-map 授了路径、工具级不可达，盘上写整体失效，须 T2 增第 ④ 点改 tools 行）；F2 = 审稿「改动前后对照」机制缺失（return 不再携修订稿全文 + reviewer 只审盘上 updating 态 + Bash 限 spec-anchor-check → 改前文本仅在 git HEAD 不可取，四查 1 意图保真「夹带检测」较现状回退，须明示 dispatch 输入构成含 before/after 对照）。两项均为已 ack 方向内机械补全，无升级 human 项。另 F3–F7 MINOR（V5「消费保护齿」名实不符、T7 文案丢免派例外、状态机 updating 期档 0/中断恢复未成文、T6 blockquote 计数四行实为五行、T5 推翻前批裁定建议加显式标签），landing 前顺手修。逐 finding 证据与实跑留痕见本目录 [review.md](review.md) R1 段。

### R2 - 2026-09-29 - architect

verdict: BLOCK(F8)

R2 聚焦复审（按轮次纪律不重做全量抽验）。F1–F7 修订全数到位：F1 = T2 增第 ④ 点（tools 行 `Read,Edit,Write,Grep,Glob,Bash`）+ §3 sweep 行注记，双层合围语义成立；F2 = T2 ② 增「改前/改后句级对照（原句 → 新句 两行式）」+ T1 第 2 步 dispatch 输入构成句，产出方/形态/消费方三点闭合；F3–F7（V5 名实注记、T7 补〔免派例外见 SSOT〕、T1 边界场景两句、T6 计数勘正五行、T5 显式推翻标签）逐项核过均到位。Status/manifest/packet-id/profile 无漂移，R1 记录 append-only 保留。**新引入一项 BLOCKING（F8）**：T2 ④ 末句「Bash 重定向写仍由 universal.bashDenyWrite 恒拦、无绕行口」为事实性错述——scope-map.json:25 实读 `bashDenyWrite = ["docs/contracts/**", ".claude/**", "delivery/taskpackages/**"]`，不含 docs/requirements.md 与 docs/architecture.md，enforce-scope Bash 分支只按该清单拦；两 allow 路径的 Bash 重定向写不被任何钩子机拦，真实旁路约束 = agent def「Bash 仅限跑 spec-anchor-check」白名单（约定级）。照此冻结会留下「Bash 旁路已机械封死」的假保证，属 260909-gov-review-gate-truthfulness 批收口同类门真话失真。修法一句改写（如实标注约定级 + Bash 旁路机拦归 OQ-3 留根同流）或删该断言；不改 F1 改法本身。无升级 human 项，修订后报 R3 收口。逐项复核表与 F8 证据见本目录 [review.md](review.md) R2 段。

### R3 - 2026-09-29 - architect

verdict: GREEN

R3 收口复审。F8 勘正到位：v0.4 T2 ④ Bash 旁路句与 scope-map.json:25 三项清单实况逐点吻合（不在覆盖内 / agent def 白名单约定级且本改法不触碰该 bullet / 机拦归 OQ-3 留根同流），无残留错述；v0.3 → v0.4 全文比对除该句与 Status 升版外零改动，R1/R2 记录 append-only 保留，无新矛盾。GREEN 前结构字段核对：landing-manifest 围栏块与 packet-id 行均在场；manifest 七路径 ≡ T1–T7 目标文件锚并集逐一吻合（OQ-4 (a) 下 AGENTS.md 在册正确）；V1 preflight 复跑 ok（anchorCount 7、anchorUnion 与 manifest 吻合、零 advisory）。八项 findings（R1 F1/F2 BLOCKING + F3–F7 MINOR、R2 F8 BLOCKING）全数修订到位；riskTier medium / G2+G3+G1 单行显式呈报 / Track B 轨道自评维持成立，契约面零触，OQ-1~4 已裁全按推荐位。主线程可凭「human 立项 ack（2026-09-28）+ 本 GREEN」标 FROZEN；三轮内收口（R1/R2 BLOCK → R3 GREEN），未触发熔断线。逐点核对与结构字段留痕见本目录 [review.md](review.md) R3 段。
