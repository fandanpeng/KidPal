# PROPOSAL · 260929-gov-review-req-loop-ui-author

> **状态：FROZEN v1.0 · 2026-09-29 · 凭据：human 立项 ack（2026-09-29 对齐会话四取舍点 + 追问三点增补后确认「立项」）+ architect R2 GREEN（§7 R2 段；R2 附带条件 R2-F1/F2 微修已随手完成、R2-F3 改写注意转 landing-summary 留痕）+ OQ 四条已裁（§4）。主线程凭据自冻，授权记 Status 行。**
> **承载 ARQ**：无——human 直接 idea 立项（2026-09-29 对齐会话四取舍点已裁，见 §4）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（闸向严：新增生成步骤与扩查，不放松任何既有门；无契约触面；行为扩面限需求 loop 与 uiux-author 双角色）。
> **review profile**：G2+G3（触面 = 治理机制面 `.claude/{references,agents,commands}` 为 G2 + `.claude/scope-map.json` 与 `.claude/scripts/brief-inputs.mjs` 为 G3；G1 = 0，契约面 = 0）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：GREEN 结论前必须在场（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/references/requirement-change.md
.claude/agents/uiux-author.md
.claude/scope-map.json
.claude/commands/requirement-change.md
.claude/scripts/brief-inputs.mjs
.claude/agents/story-author.md
.claude/agents/requirement-reviewer.md
.claude/agents/planner.md
.claude/commands/arch-change.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：需求变更触 UI 时，UI 设计在需求 loop 内一次生成并核实（「画的是不是需求说的」），human 终拍看着图拍板；下游 planner / builder 直接消费已随需求 frozen 的设计母版——builder 面向的是每页 400×300 精确画布 PNG（本机应用 = ESP32-S3 RLCD 400×300 1bpp，PNG 即布局坐标的定位权威），不再从 HTML/文字现猜 layout。

**现状差距**：需求变更流程（`.claude/references/requirement-change.md`）只覆盖文字面（requirements.md / architecture.md），UI 面不在 loop 内。UI↔需求对齐目前靠人工设计批（先例：UIUX-DESIGN-BRIEF v4 头部「v3 → v4 全面对齐 requirements.md v1.40」），dev-loop 里 planner/builder 遇到需求批没覆盖的 UI 就要现场画或挂起。编制上「系统级 UI 生成」位空缺——uiux-author def 写明「系统级 UI 蓝图归 `uiux-designer` / governance-lane（该编制尚未接线）」。**且 dev-loop 侧 UI 消费链实测两头断**（证据见 T5）：「UI 做的不对」是交付缺陷高发类，uiux-author 机制到场率趋零（53 包中仅 3 包带 uiuxRequired 字段、true 仅 1 包，且唯一那次判定 L2 时 agent def 尚不存在、直翻降级）。

**非目标（本批不做）**：

- 不新建 uiux-designer 角色（human 已裁：重定义 uiux-author 为双模式）。
- 不改 dev-loop 消费链路——uiux-author 翻译模式（frozen brief → TaskPackage uiux block）与 acceptance-author / builder 的既有消费面零改动；planner def 仅悬空指称同步（T6.3），调度行为不变。
- 不重造出图管线——出图沿用 OpenDesign MCP 既有操作知识（opendesign-connect skill 已载联通排障）。
- `.claude/references/uiux-opendesign-loop.md`（出图 loop 程序 SSOT）本批不建，留根（§6）。
- 本批不改 docs/ui/ 母版内容——母版内容由后续首个触 UI 需求批经新流程产生。

## 2. 问题、根因与改法（T1–T6）

**【硬项 · 起草期实证义务】**本节的断言纪律：

- 凡引用 file:line → 起草期现读核对（本批起草时逐条实读，见各 T 证据段）。
- 凡声称「比照先例」→ 先读先例文件确认形态再落笔（T1 消费保护比照 requirement-change.md「消费保护规则」节原文）。
- 凡引用脚本入口 → 起草期亲核入口存在（§5 验证集命令起草期实跑，见各条标注）。

**【硬项 · 目标文件锚清单】**每 T 末尾显式列「目标文件」；landing-manifest ≡ 各 T 并集。

### T1 · requirement-change SSOT 扩 UI 分支

**问题**：需求面变更流程 SSOT 无 UI 生成与核实的角色与步骤——触 UI 的需求批落稿后，UI 面处于无人接的状态：要么等人工设计批（节奏与需求批脱钩），要么 dev-loop 现场画（绕过设计母版单一事实源）。

**证据**：`.claude/references/requirement-change.md` 全文（2026-09-29 实读）——三档流程、角色分工表（story-author / requirement-reviewer / architect / 主线程 / human 五行）均无 UI 生成角色；「升档规则」句已有「线框需同步」携带清单→升档 2 的接线（line 13），说明起草时已意识到 UI 牵连存在、但只解决了升档、没解决生成。先例：`docs/ui/UIUX-DESIGN-BRIEF.md` line 5「v3 → v4 全面对齐 requirements.md v1.40」——UI↔需求对齐现靠人工批、无 loop 承载。

**根因**：requirement-change 三档流程立项时（260928-gov-review-requirement-face）范围裁定为文字面（requirements.md / architecture.md 两文件），UI 面未纳入起草写面与审稿查项。

**改法**：改 `.claude/references/requirement-change.md` 五处——

1. **触 UI 判定**：story-author 牵连面清单新增显式类目「触 UI」（新增页面 / 改页面布局或交互 / 需出图）——SSOT 加类目的同时 story-author def 自持清单同步加（双源同批改，见 T6），防「SSOT 有、dispatch 拿到的清单没有」的判定输入断链。触 UI 的语义修改按既有牵连升档规则自然落档 2；UI 生成步骤只挂档 2，档 0 / 档 1 不派。
2. **档 2 流程插步**（architect 可行性判断之后、requirement-reviewer 审稿之前）：触 UI → 主线程派 `uiux-author`（生成模式，见 T2）产出本批 UI 修订——UIUX-DESIGN-BRIEF 升版 + 设计 HTML + **出图规格单（逐页 pageId/stateId、画布 400×300、黑白 1bpp 风格、内容要点、对应 brief 锚）**；**出图执行 = 主线程代执行**：主线程按规格单跑 OpenDesign MCP（od_*）生成/回填 PNG 落 `docs/ui/`——uiux-author def 不扩 MCP 工具面（tools 高敏字段不动）。**出图定例：每个受影响页面/状态一张 400×300 精确画布 PNG（文件名对齐 pageId/stateId）**——PNG 是 builder 布局坐标的定位权威，HTML 仅作母版迭代载体不进 builder 视野。产物头部标注「随批 <changeId> 在途」。🟡 可行性或取舍点未被 human 消解前不出图（不带病下行，对齐既有条款）。**否回循环交互**：reviewer 打回的需求修订触及 UI 面 → 主线程重派 uiux-author（生成模式）更新在途产物（含重出图）后再送复审，状态保持 updating。
3. **requirement-reviewer 四查扩五查**：触 UI 批次增查「UI↔需求一致性」——对照 updating 态需求与新落 UI 产物逐项核（页面覆盖、交互口径、键位提示与 §2.3 键位逐语境口径相符）；UI 产物不在 reviewer 写面，恒只读。
4. **消费保护扩展**：触 UI 批次的 docs/ui 在途产物随需求 updating 同护——下游（triage / taskpackage / dev-loop）只认 frozen，机械判据 = §9 版本行在位 **且** UIUX-DESIGN-BRIEF 版本行注明对齐的本批 requirements 版本号。比照对象：「消费保护规则」节既有句「updating 态的 requirements.md 不得被下游引用……机械判据 = §9 对应版本行在位」。
5. **human 终拍材料扩展**：档 2 终拍呈报 = architect 结论 + reviewer 结论 + **UI 产物（设计稿/图）**——human 看图拍「要不要」。

目标文件：`.claude/references/requirement-change.md`（改）

### T2 · uiux-author def 重定义（翻译 / 生成双模式）

**问题**：uiux-author 现被「只翻译不发明」钉死在 dev-loop 翻译位——需求 loop 出现 UI 生成需求时无角色可派。

**证据**：`.claude/agents/uiux-author.md`（2026-09-29 实读）——frontmatter description「只翻译不发明：不发明系统 UI」；正文「不发明系统 UI、不改设计母版、不写实现或契约」（line 12）、「系统级 UI 蓝图归 `uiux-designer` / governance-lane（该编制尚未接线），不归你」（line 17）。

**根因**：编制设计时把「系统级 UI 生成」与「包级 brief 翻译」拆成两个角色，生成位（uiux-designer）一直未接线；需求 loop 新增生成步骤后，空位与新步骤须一次接上。

**改法**：重定义 uiux-author 为**双模式**，description 与正文同步改——

- **模式 A · 翻译（既有不变）**：dev-loop 内，frozen UIUX-DESIGN-BRIEF → TaskPackage uiux block / uiux-brief.md。触发条件、产物、边界全保留。
- **模式 B · 生成（新）**：requirement-change 档 2 内，按已过 architect 可行性的需求修订生成/修订系统级 UI——UIUX-DESIGN-BRIEF 升版、设计 HTML、出图规格单（逐页 400×300 规格，供主线程代执行出图，见 T1.2）。边界：**不发明需求语义之外的 UI**（需求修订句是唯一设计依据，brief 未覆盖且需求未提及的面如实标「需求未覆盖」交回主线程，不脑补）；写面限 `docs/ui/**`（scope-map 扩权见 T3）；在途产物随批 frozen，不单独定稿；frontmatter tools 不扩（od_* 由主线程执行，不走 agent 工具面）。
- def 内删「uiux-designer / governance-lane」悬空指称（编制空位由本批模式 B 承接，指称已失效）。

目标文件：`.claude/agents/uiux-author.md`（改）

### T3 · scope-map 写槽扩权（机械强制层配套）

**问题**：uiux-author 按 scope-map 只有 `delivery/taskpackages/**` 写槽——T2 授予的 `docs/ui/**` 写面会被 enforce-scope 钩子拦截，双模式落地即卡。

**证据**：`.claude/scope-map.json`（2026-09-29 实读）——`"uiux-author"` 条目 `"allow": ["delivery/taskpackages/**"]`（line 125–128），`docs/ui` 在 `universal.docsTopLevel` 白名单（line 42）但不在该 agent allow 列。

**根因**：编制定义 uiux-author 时只有翻译位产物（TaskPackage 目录），无 docs/ui 写面——因为翻译位按边界不改设计母版。

**改法**：`scope-map.json` uiux-author 条目 allow 列增 `"docs/ui/**"`；deny 列不变（contracts / requirements / architecture / .claude / 参考树仍全禁）。**最小扩权**：只授 UI 母版面，需求文字面与契约面继续不可写——「生成者不碰需求原文、审者不碰产物」的分权靠写槽硬隔离兜底。

目标文件：`.claude/scope-map.json`（改）

### T4 · /requirement-change 命令编排接线

**问题**：loop 入口命令的编排步骤无 UI 步骤——SSOT 扩了、命令不接，主线程跑 loop 时不会派生成。

**证据**：`.claude/commands/requirement-change.md`（2026-09-29 实读）——第 4 步（architect 可行性，line 17）与第 5 步（requirement-reviewer 四查，line 18）之间无 UI 生成步骤；description 亦无 uiux-author 路由信号。

**根因**：同 T1——命令是 SSOT 的编排投影，SSOT 缺 UI 分支则命令无从投影。

**改法**：命令改四处——① 第 4 步后插「4.5 UI 生成（仅档 2 且触 UI）」：主线程判触 UI（依据 = author 牵连面清单显式类目，def 侧同批加类目见 T6）→ 派 `uiux-author`（生成模式）产 brief 升版 + HTML + 出图规格单 → 主线程代执行出图回填 PNG 落 docs/ui/ 标在途；② 第 5 步「四查」改「四查（触 UI 批次扩五查）」并在 dispatch 输入构成中显式含 UI 产物路径；③ 第 6 步终拍呈报清单加 UI 产物；④ description 同步增路由信号（uiux-author / 生成模式 / 触 UI 判定）。

目标文件：`.claude/commands/requirement-change.md`（改）

### T5 · builder 像素参照定例（400×300 页图）与设计 refs 入封闭输入面

**问题**：「UI 做的不对」是交付缺陷高发类。根子是 builder 拿不到像素级视觉参照——layout 坐标从文字 spec 现猜；且 uiux 设计 refs 根本进不了 builder 的封闭输入面，即便 uiux block 挂了 designRefs 也无人消费。

**证据**（2026-09-29 起草期实查）：

1. `delivery/changes/dev/260829-schedule-p2/review-packet.md` line 31：「布局常量按 host seam 400x300 定坐标（列表纵向排布）」——schedule_page.c:622 坐标是估算值。
2. `delivery/changes/dev/260829-hardware-validation/review-packet.md` line 208：「阅读态分页行高按 400x300 最小估算」——mcp_server.cc:728 同类；该 packet 并记「已知殿后项：故事页排版」。
3. `delivery/changes/dev/260915-m1-k3-voice-round-ui/pod-retro.md` line 15：「判定 L2 后发现 .claude/agents/ 根本没有 uiux-author.md……直翻降级 + friction 留痕走通」。
4. 全仓 `delivery/taskpackages/*/taskpackage.json` 实查（grep uiuxRequired，2026-09-29）：53 个包目录中仅 **3 包**带该字段（260915-m1-k3 = true；260829-smoke-e2e = false；260917-m2-k3 = false 附判定注记），其余缺省（task-intake-and-slicing.md:332 明文 UIUX 字段可选）——`uiuxRequired=true` 仅 1 包，机制到场率趋零。
5. `.claude/scripts/brief-inputs.mjs` 实查（grep designRefs/uiux，2026-09-29）：**0 处命中**——builder 封闭输入面派生脚本不含任何 uiux 设计 refs 透传。
6. 页级 400×300 PNG 先例已在母版：`docs/ui/V5/` 下 canvas-p1–p3、state-*、char-window 等十余张——出图形态已被人工设计批验证可行，缺的是把它变成规则与管线。

**根因**：设计产物形态（HTML + 文字 brief）与实现面（LVGL 1bpp 400×300 坐标常量）之间没有像素级交接物；uiux refs 未接进 `brief-inputs.mjs` 派生管线——设计侧与 builder 的交接断在两处。

**改法**：三处——

1. **出图定例**（落 T1 生成步骤与 T2 模式 B 正文）：每页/每状态一张 400×300 精确画布 PNG，文件名对齐 pageId/stateId，落 `docs/ui/`；PNG 是 layout 坐标定位权威，HTML 仅母版迭代载体。
2. **模式 A 挂图**（落 T2 模式 A 正文）：uiux-brief.md 逐页挂 PNG ref；TaskPackage uiux block 的 `designRefs[]` 含对应 PNG 路径。母版已有的（如 V5 先例）直接引用，缺的如实标「无对应页图」交回 planner。
3. **管线透传**：`brief-inputs.mjs` 派生 builder 封闭输入面时增列 uiux block 的 `designRefs[]` / `sharedUiRefs[]` 与 `uiux-brief.md`（在位才列，缺席不脑补）；builder 按清单读图、以 PNG 定 layout 常量。builder def 零改动——封闭输入面语义下，refs 入清单即入视野。**self-check 夹具义务**（真空绿规约）：新 uiux 路增夹具 + 反真空断言（在位列 / 缺席不列两臂），对齐 brief-inputs.mjs 既有①b/⑤路惯例；V6 预期随之改（见 §5）。

目标文件：`.claude/references/requirement-change.md`（改，T1 生成步骤携出图定例）· `.claude/agents/uiux-author.md`（改，模式 A/B 挂图句）· `.claude/scripts/brief-inputs.mjs`（改）

### T6 · 周边角色 def / 命令同步批（判定输入与悬空指称收口）

**问题**：SSOT 扩 UI 分支后，四处周边文件的自持枚举 / 悬空指称不同步即假陈述——① story-author def 档位增量段自持牵连面清单，SSOT 加「触 UI」类目而 def 不加，4.5 步判定输入断链；② requirement-reviewer def 的「四查」路由信号 / 查项枚举 / 读面边界三处过时，五查无 def 承载；③ planner.md 与 /arch-change 命令里的「uiux-designer 尚未接线」句在模式 B 落地后即为假陈述。

**证据**（2026-09-29 起草期 + R1 判官复核）：

1. `.claude/agents/story-author.md:24–25`——档位增量段自持清单枚举（「关联 story / §2 共享底座 / 线框需同步」；档 2 另枚举六类），无「触 UI」类目。
2. `.claude/agents/requirement-reviewer.md`——description 含「四查」路由信号；`.claude/route-fixtures/routing.json:57` 的 `agentRequiredSignals.requirement-reviewer` 必留信号含「四查」（改 description 丢该子串则 V2 RED）；正文四查枚举节、硬边界读面句（未含 docs/ui 产物）同待扩。
3. `.claude/agents/planner.md:61`——「系统级 UI 蓝图归 `uiux-designer` / governance-lane（该编制尚未接线，DEV-GOVERNANCE §8 在册），不由你起草」。
4. `.claude/commands/arch-change.md:26`——「起草执行角色是 `uiux-designer`……moderator dispatch 尚未接线」。

**根因**：角色 def 自持操作清单与流程 SSOT 是两个面，SSOT 扩分支时 def 侧若无同批同步项即漂移（与 R1-F1/F2/F3 同源）。

**改法**：四处最小同步——

1. `.claude/agents/story-author.md`：档位增量两处清单各增「触 UI（新增页面 / 改页面布局或交互 / 需出图）」类目，与 T1.1 同措辞。
2. `.claude/agents/requirement-reviewer.md`：description 改「四查（触 UI 批次扩五查：UI↔需求一致性）」——**保「四查」子串**（routing.json 必留信号约束）；正文四查节加第五查条件句（触 UI 批次生效）；硬边界读面扩「触 UI 批次只读 docs/ui 在途产物」（恒只读不变）。
3. `.claude/agents/planner.md:61`：「该编制尚未接线」句改指「系统级 UI 生成已接线 = requirement-change loop 档 2 uiux-author 生成模式；planner 不另派」——职责边界（不由 planner 起草）语义不变。
4. `.claude/commands/arch-change.md:26`：同款改指模式 B 承接，dispatcher 提示同步。

目标文件：`.claude/agents/story-author.md`（改）· `.claude/agents/requirement-reviewer.md`（改）· `.claude/agents/planner.md`（改）· `.claude/commands/arch-change.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED = 0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 治理机制 | `.claude/references/requirement-change.md` | G2 | 流程 SSOT 扩 UI 分支；消费方 = /requirement-change 命令 + story-author（牵连面类目）/ requirement-reviewer（扩查）/ uiux-author（生成模式）/ 主线程 |
| 治理机制 | `.claude/agents/uiux-author.md` | G2 | def 重定义双模式；消费方 = planner dispatch（模式 A 不变）+ 主线程（模式 B 新派发） |
| 治理机制 | `.claude/scope-map.json` | G3 | uiux-author allow 扩 `docs/ui/**`；消费方 = enforce-scope 钩子（机械强制层） |
| 治理机制 | `.claude/commands/requirement-change.md` | G2 | loop 入口编排接线；消费方 = 主线程（human 触发 /requirement-change 时） |
| 治理机制 | `.claude/scripts/brief-inputs.mjs` | G3 | builder 封闭输入面派生增列 uiux 设计 refs；消费方 = planner Step 7 前置生成 + builder（按清单读图） |
| 治理机制 | `.claude/agents/story-author.md` | G2 | 牵连面自持清单加「触 UI」类目（T6.1）；消费方 = story-author 起草报告 + 主线程触 UI 判定 |
| 治理机制 | `.claude/agents/requirement-reviewer.md` | G2 | 五查承载 + 读面扩 docs/ui 在途产物只读（T6.2）；description 保「四查」子串（routing.json 必留信号）；消费方 = 主线程 dispatch + route-regression-check |
| 治理机制 | `.claude/agents/planner.md` | G2 | 「uiux-designer 尚未接线」悬空句改指模式 B（T6.3）；消费方 = planner dispatch 判读 |
| 治理机制 | `.claude/commands/arch-change.md` | G2 | 同款悬空句改指（T6.4）；消费方 = review-moderator / 主线程入口 |
| 治理面 | `docs/DEV-GOVERNANCE.md` | G1 · 未触 + 留根 | 本批不动（G1=0）；:54/:245/:264 的 roster 行与留根句同步走独立 human-approved 批（§6 留根）；消费方 = governance 编制读者 |
| 需求面 | `docs/requirements.md` / `docs/architecture.md` | 未触 | 本批不改需求文字面；后续触 UI 需求批按新流程正常走 write-story 写面 |
| UI 母版 | `docs/ui/**` | 未触 | 本批不改母版内容；首个触 UI 需求批经新流程产生（出图定例先例 = 既有 V5 页图） |
| dev-loop | acceptance-author / builder / verifier | 未触 | agent def 零改动；builder 视野经封闭输入面内容变化扩展（refs 在位才列）；模式 A 消费链形态不变。planner def 仅悬空句同步（上行），调度行为不变 |
| 契约面 | `docs/contracts/**` | 未触 | G1 = 0；display 响应块等契约定义不受 UI 设计稿生成影响 |

## 4. human 取舍点（OQ）

四条取舍点已于 2026-09-29 立项对齐会话裁毕（AskUserQuestion 四问，human 逐条裁定）：

| OQ | 选项 | human 裁定 |
|---|---|---|
| OQ-1 UI 生成角色 | (a) 启用空位 uiux-designer（推荐位）/ (b) 重定义 uiux-author 为双模式 | **裁 (b)**——接受双职责换少一个角色；T2 按此落，边界句同步改防职责混 |
| OQ-2 UI 核实位 | (a) requirement-reviewer 四查扩五查（推荐位）/ (b) story-author 自核 / (c) 新建 uiux-reviewer | **裁 (a)**——复用否回循环，作者≠审者纪律不破 |
| OQ-3 产物落点 | (a) 更新 docs/ui 设计母版（推荐位）/ (b) 落需求变更批次目录 | **裁 (a)**——下游消费链零改动，母版单一事实源不生双源 |
| OQ-4 生成时机 | (a) architect 可行性后、human 终拍前（推荐位）/ (b) 终拍后执行出图 / (c) 起草期并行出图 | **裁 (a)**——终拍看图拍「要不要」，可行性未消解不出图 |

无遗留 OQ——本批不涉挂载面扩张（docs/ui 已在 docsTopLevel 白名单）、不涉全会话新机制（角色派发是既有机制新实例）、不涉不可逆操作。

## 5. 验证集（landing 前机械前置 + landing 后核验）

| # | 时点 | 命令 | 预期 |
|---|---|---|---|
| V1 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/changes/arch/260929-gov-review-req-loop-ui-author/PROPOSAL.md` | exit 0；anchorUnion 成员 = manifest 九路径、无注记夹带路径 token |
| V2 | pre-commit | `node .claude/scripts/route-regression-check.mjs` | exit 0——uiux-author description 改后路由信号不退化（authoring rule 机械守卫，起草期已亲核脚本入口在位） |
| V3 | pre-commit | `node .claude/scripts/lint-descriptions.mjs` | exit 0（告警级守卫，无超长/流程叙述气味 flag） |
| V4 | post-commit | `node -e "JSON.parse(require('fs').readFileSync('.claude/scope-map.json','utf8'));console.log('ok')"` | 输出 ok——scope-map 改后 JSON 可解析 |
| V5 | post-commit | `grep -c "uiux-author" .claude/references/requirement-change.md .claude/commands/requirement-change.md` | 两文件各 ≥1——SSOT 与命令均含生成模式接线句 |
| V6 | post-commit | `node .claude/scripts/brief-inputs.mjs --self-check` | exit 0 **且输出含新增 uiux 路自检名**——透传改动后脚本自检绿、新路径非 vacuous（自检入口起草期已亲核在位，brief-inputs.mjs:44/505） |

真空绿规约：V1–V3 跑前先确认脚本入口在位（`ls .claude/scripts/` 三脚本名在列）；环境红先立基线再归因。

## 6. 留根清单

- **`.claude/references/uiux-opendesign-loop.md` 不建**：出图 loop 的程序 SSOT 暂缺，出图操作知识现住 opendesign-connect skill（联通排障）。首个模式 B 实跑批若出图步骤出现可复用程序（图页映射、命名、母版插锚），再立项成文。**od_* 失联升级路径**：联通恢复末步（/mcp 重连）需 human 操作（skill 明文）——模式 B 实跑批出图遇失联时主线程挂 human-decision 事件、按 skill 排障，产物状态标「出图 pending」不阻塞需求文字面先行。
- **docs/DEV-GOVERNANCE.md roster 同步（G1 面）**：:54/:245/:264 的 uiux-designer roster 行与留根句在本批后即为历史表述——G1 面不随本批动，走独立 human-approved 治理变更收口（R1-F3 裁定）。
- **brief↔requirements 版本对齐机检齿**：T1 的 frozen 判据把「brief 版本行注明对齐版本号」升为规则，但无脚本校验对齐——先例（v4↔v1.40）靠人核。实跑 2–3 个触 UI 批后按摩擦立项（比照「消费保护规则」节既有留根句法）。
- **出图粒度未拍**：每批全量出图 vs 仅新增/变更页出图——首实跑批按成本与摩擦定例。
- **登记不硬治项**：uiux-author 双模式后，「何时派生成模式 vs 翻译模式」靠 description 路由信号 + dispatch 显式指名（主线程 dispatch prompt 写明模式），不建独立机械判别。
- **L0/L1 包不强制挂图**：挂图义务现只落模式 A（L2+ 派 agent 才产 uiux-brief）；L1 包对母版既有页图（V5 先例）的引用靠 planner 判定与 designRefs 自由挂载，不设强制——实跑后若「UI 做的不对」在 L1 包仍高发，再复议门槛下移（T5 证据 4 的到场率问题一并看）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 2026-09-29 · architect（Track B 判官）

**总裁定：回返（非 GREEN）**——4 条 BLOCKING（R1-F1–F4）+ 2 条 SHOULD（R1-F5–F6）+ 3 条 MINOR（R1-F7–F9）。主线程按各 finding 修订 §0–§6 并同步扩 landing-manifest 后送 R2 复审；R2 起点核对 = manifest 扩后重跑 `node .claude/scripts/landing-fidelity-check.mjs --preflight <本文件>` 且 V1 预期数字同步改正（见 R1-F8②）。本批方向（UI 生成接入需求 loop、双模式重定义、PNG 定位权威）判官无异议，回返项全部是完备性与落地卡点，不涉方向推翻。

**已核项（复核通过面，先记录）**：

1. **逐 T 引据亲读复核通过**：T1——`.claude/references/requirement-change.md:13` 升档规则句「线框需同步」在场属实，`docs/ui/UIUX-DESIGN-BRIEF.md:5`「v3 → v4 全面对齐 requirements.md v1.40」属实；T2——`.claude/agents/uiux-author.md:12/:17` 两句悬空指称逐字属实；T3——scope-map `docs/ui` 在 `universal.docsTopLevel.allowDirs`（line 42）属实，uiux-author 条目 allow 仅 `delivery/taskpackages/**` 属实（行号小漂见 R1-F8③）；T4——`.claude/commands/requirement-change.md:17/:18` 第 4/5 步之间确无 UI 步骤；T5——六条证据逐一亲读/实查：证据 1（`delivery/changes/dev/260829-schedule-p2/review-packet.md:31`）、证据 2（`260829-hardware-validation/review-packet.md:208`）、证据 3（`260915-m1-k3-voice-round-ui/pod-retro.md:15`）逐字属实；证据 5（brief-inputs.mjs 全文无 designRefs/uiux 透传、`collectRefs` 只收三桶 line 135）属实；证据 6（`docs/ui/V5/` 下 12 张 PNG：canvas-p1–p3、state-*、char-window、p3-* 等）经 `ls` 亲核在场；V6 自检入口 `brief-inputs.mjs:44/:505` 属实。证据 4 的量化表述失准，单独立 R1-F7。
2. **packet-binding 抽查通过**：landing-manifest 5 行 ≡ T1–T5 目标文件锚并集（T5 三锚中两枚复用 T1/T2 目标，unique = 5），形态合规（裸路径、无注记夹带路径 token）。回返修订扩 manifest 后此结论失效、须重验。
3. **轨道裁定：Track B 恰当**。无契约触面（G1/契约 = 0 属实）、无跨系统结构性变更（触面全在治理机制面）、governance 类变更不强制升级（protocol「双轨路由」节）；无应升 Track A 的触面。但 profile 自评标签失准（R1-F9）。
4. **T3 扩权最小性判定：通过**。allow 只增 `docs/ui/**`，deny 列不动（含 `docs/requirements.md` / `docs/architecture.md` / `docs/contracts/**` / `.claude/**`）——「生成者不碰需求原文、审者不碰产物」的写槽硬隔离成立，模式 B 读 updating 态 requirements 不受写面 deny 影响（scope-map 管写不管读）。
5. **T5 管线兼容性判定：通过**。brief-inputs.mjs 增列不破既有 ①–⑤ 路（新列「在位才列」、不新增 fail-loud 错误类）；planner Step 7/9 的消费形态（只引 brief 路径）不变；`task-intake-and-slicing.md:349`「brief 语义变更时在途包暂停」规则与 T1.4 消费保护互补不冲突。
6. **连带面「不需要改」的裁定与理由**：acceptance-author def 不需要（`acceptance-author.md:23` 只消费模式 A 产物，形态不变）；builder def 不需要（封闭输入面语义下 refs 入清单即入视野，PROPOSAL 理由成立）；landing-verifier 不需要（本批 manifest 全是 `.claude/**` 既有类型文件，docs/ui 产物由后续需求批经各自 loop 落地、不经 gov landing-verifier）。story-author / requirement-reviewer / planner / arch-change 四处判定「需要」，见 R1-F1–F3。

**Findings**：

**R1-F1 · BLOCKING · story-author def 同步缺位（改法完备性）**。T1.1 把「触 UI」类目加进 SSOT 牵连面清单，且 T4 的 4.5 步触 UI 判定依据明写「author 牵连面清单显式类目」——但 `.claude/agents/story-author.md:24–25` 档位增量段**自持清单枚举**（「关联 story / §2 共享底座 / 线框需同步」、档 2「关联 story / §2 共享底座 / 线框 WF-* / §6 路线图 / §8 已砍清单 / NFR」），SSOT 加类目而 def 不加，则主线程拿到的清单里没有「触 UI」标记、4.5 步判定输入断链；反之只改 def 则双源漂移。**建议**：`.claude/agents/story-author.md` 入 landing-manifest，T1（或单立 T6）加最小同步句——档位增量两处清单各增「触 UI（新增页面 / 改页面布局或交互 / 需出图）」类目；§3 sweep 表 story-author 行由「未触」改实类。

**R1-F2 · BLOCKING · requirement-reviewer def 同步缺位（改法完备性 + 路由信号交互）**。SSOT 扩五查后 def 三处过时：① description 路由信号「四查」（`uiux-author.md` 对偶句同款）——且 `.claude/route-fixtures/routing.json:57` `agentRequiredSignals.requirement-reviewer` 必留信号含「四查」，改写 description 时若丢「四查」子串，V2 `route-regression-check.mjs` 即 RED；② 正文「## 四查」枚举节须加触 UI 批次的第五查条件句；③ 硬边界「只读 `docs/requirements.md` 与相邻制品」未含 docs/ui 在途产物——五查要对照 UI 产物逐项核，读面须显式扩（scope-map `requirement-reviewer` allow=[]/deny=["**"] 是写面语义、不拦读，但 def 硬边界句会拦）。**建议**：`.claude/agents/requirement-reviewer.md` 入 manifest；description 保「四查」子串（如「四查（触 UI 批次扩五查：UI↔需求一致性）」）；正文与硬边界同步三处。若选择改 routing.json 信号（不建议），该文件属 G3 面须入 manifest。

**R1-F3 · BLOCKING · uiux-designer 悬空指称清扫不全 + sweep 表缺类**。T2 改法第 3 条只清 `uiux-author.md` 自身两处（line 17/34），但同一悬空指称还住在：`.claude/agents/planner.md:61`（「系统级 UI 蓝图归 `uiux-designer` / governance-lane（该编制尚未接线，DEV-GOVERNANCE §8 在册），不由你起草」）、`.claude/commands/arch-change.md:26`（「起草执行角色是 `uiux-designer`……moderator dispatch 尚未接线」）、`docs/DEV-GOVERNANCE.md:54/:245/:264`（roster 行 + 留根行）。本批落 G1=0（不动 DEV-GOVERNANCE）判正确，但：① §3 sweep 表完全没给 DEV-GOVERNANCE 行——ARQ-096 义务 UNCLASSIFIED=0 被破，须补「G1 · 未触 + 留根：roster 行与 §264 留根句的同步走独立 human-approved 批」；② planner.md:61 与 arch-change.md:26 的「尚未接线」句在模式 B 落地后即为假陈述（编制已由 uiux-author 模式 B 承接），G2 面可同批最小同步，须入 manifest，或在 §3/§6 显式给「不同步」理由。现状 §3 表把 planner 标「未触、agent def 零改动」与 line 61 实况直接冲突。**建议**：planner.md + arch-change.md 入 manifest 做最小同步（「尚未接线」句改指模式 B / requirement-change loop）；DEV-GOVERNANCE 补 sweep 行 + §6 留根。

**R1-F4 · BLOCKING · 模式 B 出图工具面缺口（落地即卡）**。出图走 OpenDesign MCP `od_*` 工具（`.claude/skills/opendesign-connect/SKILL.md:33–34`：`od_generate_design` 单页 5–10 分钟、`od_save_project_file` 覆盖存），而 `uiux-author.md:4` frontmatter `tools: Read, Write, Grep, Glob`——无任何 MCP 工具。T2 改法未裁「谁执行出图」：若 agent 自出图，tools 行须扩（authoring 纪律：逐字段隔离、误伤即破 dispatch 白名单；且 tools 变更属高敏字段——Track A 语境恒 HUMAN_REQUIRED，本批 human 立项 ack 的四取舍点未覆盖 tools 面，修订后须回 human 补 ack，存疑向严）；若主线程代执行出图（agent 产 brief 升版 + HTML + 出图指令清单，主线程跑 od_* 后回填 PNG），则 T1.2 流程句与 T2 模式 B 正文须如实写、不得写成「派 uiux-author 产出图」。二选一必须落进修订稿，不能悬空。另注：skill 明文联通恢复末步（`/mcp` 重连）需 human——模式 B 实跑批遇失联的升级路径应在 T2 或 §6 留根提一句。

**R1-F5 · SHOULD · 触 UI 批次否回循环的 UI 产物再生成未定义**。档 2 否回循环（reviewer 打回 → story-author 改盘上 → 复审）里，若需求修订触及 UI 面，在途 UI 产物即过期；T1.3 五查会抓到不一致，但流程没有「重派 uiux-author 更新在途产物再复审」的步骤定义，否回循环与 UI 生成的交互是空白。**建议**：T1.2 补一句——否回循环中需求修订触及 UI 面 → 主线程重派 uiux-author（生成模式）更新在途产物后再送 reviewer。

**R1-F6 · SHOULD · T5.3 无 self-check 夹具义务（真空绿规约）**。模板 §5 明文「自检验证输出含新增自检名」；brief-inputs.mjs 既有惯例 = 每路带夹具 + 反真空断言（①b 路、⑤ 路先例在册）。T5.3 增 uiux 路若无夹具，V6 的「exit 0」对新代码路径是 vacuous green。**建议**：T5.3 改法补一句 self-check 夹具义务（在位列 / 缺席不列两臂），V6 预期改为「exit 0 且新增 uiux 路自检名在场」。

**R1-F7 · MINOR · T5 证据 4 量化表述失准**。「30+ 包中 `uiuxRequired=true` 仅 1 包」实况（2026-09-29 判官实查）：`delivery/taskpackages/` 共 53 目录，仅 **3 包** taskpackage.json 带该字段（260915-m1-k3 = true；260829-smoke-e2e = false；260917-m2-k3 = false 附判定注记），其余字段缺省（`task-intake-and-slicing.md:332` 明文 UIUX 字段可选、L0 包无需带键）。结论「机制到场率趋零」反而更强，但按模板硬项「凡量化声明 → 现跑实测留痕」须改写为实数口径。

**R1-F8 · MINOR · 文面硬项三小错**：① §2 节标题「（T1–T4）」应为「（T1–T5）」（packet-id 行已是 T1–T5，标题漂移）；② V1 预期「manifest 四路径」应为**五**路径（manifest 实为 5 行；回返扩 manifest 后按新数改）；③ T3 引据「line 125–127」实为 line 125–128（allow 行在 126；引述内容本身准确，纯行号漂）。

**R1-F9 · MINOR · review profile 自评失准**。触面含 `.claude/{references,agents,commands}/**`（G2 面）+ `scope-map.json` / `scripts/brief-inputs.mjs`（G3 面），按模板 Status 行规格应标 **G2+G3** 而非 G3。不影响轨道裁定（见已核项 3），Status 行修订时一并改。

**R1 结论：回返。** 修订清单汇总：manifest 扩至 ≥9 文件（原 5 + story-author.md + requirement-reviewer.md + planner.md + arch-change.md，若 R1-F4 裁 agent 自出图则 uiux-author.md 的 tools 决定写入 T2 并回 human 补 ack）；T1/T2/T4 按各 finding 补句；§3 sweep 补 DEV-GOVERNANCE 行与 planner/arch-change 实类；§5 V1/V6 预期修正；Status 行 profile 改 G2+G3。修订后重跑 preflight + V1–V3，送 R2。

### R2 · 2026-09-29 · architect（Track B 判官 · v0.3 复审）

**总裁定：GREEN，可标 FROZEN**——R1 全部 9 条 finding 核销通过（逐项见下），packet-binding 重验通过，新增 2 条 MINOR 文字层残留 + 1 条落地措辞注意（R2-F1–F3），均为非阻塞、不触 manifest 与改法语义，主线程在标 FROZEN 前的人工复核窗口顺手修正即可，**无需 R3**；landing-verifier 按本段 R2-F1/F2 核对 FROZEN 文本已含该两处微修。

**R1 逐项核销**：

| Finding | 修订落点 | 核销 |
|---|---|---|
| R1-F1（story-author 同步） | T6.1 + manifest + §3 行 162 实类 G2 + T1.1 补「双源同批改」句 | ✓ |
| R1-F2（requirement-reviewer 同步） | T6.2 三处（description 保「四查」子串并点明 routing.json:57 约束 / 正文第五查条件句 / 硬边界读面扩 docs/ui 只读）+ manifest + §3 实类 | ✓ |
| R1-F3（悬空指称清扫 + sweep 缺类） | T6.3/T6.4（planner.md:61、arch-change.md:26 改指模式 B）+ §3 补 DEV-GOVERNANCE「G1·未触+留根」行 + §6 留根条 + §0 非目标补 planner 说明 | ✓ |
| R1-F4（出图工具面缺口） | 裁**主线程代执行出图**：T1.2 出图规格单 + 主线程跑 od_* 回填、T2 模式 B「tools 不扩」、T4 ① 同步、§6 留根 od 失联升级路径（挂 human-decision 事件、出图 pending 不阻塞文字面）——四处一致、无悬空 | ✓ |
| R1-F5（否回循环交互） | T1.2 补否回循环句（触 UI 面 → 重派生成模式更新在途产物含重出图，再送复审） | ✓ |
| R1-F6（self-check 夹具） | T5.3 补夹具义务（两臂 + 反真空，对齐①b/⑤路惯例）+ V6 预期改「exit 0 且含新增 uiux 路自检名」 | ✓ |
| R1-F7（证据 4 口径） | T5 证据 4 改实数口径（53 目录 / 3 包带字段 / true 仅 1 / 其余缺省），与判官 R1 实查一致 | ✓ |
| R1-F8（文面三小错） | ①§2 标题 T1–T6 ✓ ②V1「九路径」✓ ③T3 行号 125–128 ✓ | ✓ |
| R1-F9（profile 标签） | Status 行改 G2+G3，触面枚举正确 | ✓ |

**R2 独立复核**：① packet-binding 重验——T1–T6 锚并集 unique = 9（T5 三锚中两枚复用 T1/T2 目标）≡ manifest 9 行，形态合规（裸路径、锚行注记无路径 token）；主线程报 preflight 绿（anchorUnion 9 = manifest 9）+ route-regression 54 ok + lint clean，采信（脚本实跑属主线程腿，判官抽查形态即可）。② T6 五段固定式齐、`### T6` 标题形态可机辨、四条证据引据（story-author.md:24–25 / requirement-reviewer.md 三处 / routing.json:57 / planner.md:61 / arch-change.md:26）均经判官 R1 期亲读复核属实。③ R1-F4 的裁定形态（主线程代执行）与既有实况一致（历史出图即主线程侧跑 od_*），不触 tools 高敏字段、不越 human 立项 ack 范围。④ 轨道/profile 维持 R1 裁定：Track B 恰当、G2+G3、G1=0（DEV-GOVERNANCE 留根处置正确）、契约面 = 0。

**R2 Findings（均非阻塞）**：

- **R2-F1 · MINOR · §3 sweep 表类别列与 Status 行自相矛盾**：Status 行（v0.3）已正确枚举「`.claude/{references,agents,commands}` 为 G2」，但 sweep 表前三行（requirement-change.md / uiux-author.md / requirement-change 命令）类别列仍标 G3（v0.1 遗留）——应改 G2；scope-map.json 与 brief-inputs.mjs 两行 G3 正确保留。纯类别标签修正，不影响落地动作与任何门。
- **R2-F2 · MINOR · §0 残留旧口径**：§0「现状差距」段仍写「30+ 包仅 1 包 uiuxRequired=true」——T5 证据 4 已改实数口径（53 目录 / 3 包带字段），§0 同一量化声明的概述句未同步。顺手统一为实数口径或「字段大多缺省、true 仅 1 包」表述。
- **R2-F3 · 建议注意（非 finding）· T6.4 落地措辞**：arch-change.md:26 的原句承载两层语义——「UIUX 轴集在 governance lane 下审」（本批不动）与「系统级 blueprint 起草执行角色 = uiux-designer、dispatch 未接线」（本批改指模式 B）。落地改写时只改第二层指称、保全第一层「UIUX 轴集仍走 /arch-change governance profile」语义，勿把轴集 review 的路由句一并改掉。

**R2 结论：GREEN，可标 FROZEN。** 附带条件仅一条程序性要求：主线程标 FROZEN 前按 R2-F1/R2-F2 完成两处文字层微修（不触 manifest、锚清单、改法语义），修后直接进 FROZEN、无需 R3；R2-F3 作为 T6.4 落地时的措辞注意随 landing-summary 留痕。R1 已核项（T3 扩权最小性、T5 管线兼容性、连带面「不需要改」三处裁定）在本轮无反证，维持。
