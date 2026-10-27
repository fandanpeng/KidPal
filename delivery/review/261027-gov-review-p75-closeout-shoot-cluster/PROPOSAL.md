# PROPOSAL · 261027-gov-review-p75-closeout-shoot-cluster

> **状态：FROZEN v0.2 · 2026-10-27 · R2 GREEN（verdict 见 §7 R2：F1–F3 + NIT-1/2 五处修订全到位、机检复跑逐值相符、判档维持 Track B）；凭「立项 ack（task_plan P7.5 节，human 2026-10-27）+ OQ-1 已拍 (a) + R2 GREEN」标冻**
> **承载 ARQ**：`ARQ-261014-closeout-obligation-write-authority-vacuum`（④ 收口写点映射：修法取 ①+②，候选③ 列 §4 OQ-1 单独拍，2610-25 子项① 留根 §6-1）· `ARQ-261019-opendesign-shoot-script-precedent`（⑤ 出图先例：条目三改进项全取，整条结项候选，结项记账见 T6）。立项 = workstream `gov-debt-reduction` P7.5 节（`delivery/workstreams/gov-debt-reduction/task_plan.md:112-116`，human 2026-10-27 立项 ack：两条并批、恒 Track B 主路径、修法照各条目建议改法）。
> **review 轨**：**Track B 主路径**（起草期自评，判档归 architect-reviewer、**只升不降**）。逐条判据见 §1；Track A 段判据不命中——方向由两条 ARQ 的既有建议改法 + P7.5 立项 ack 收敛，无声明红队。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6，共 17 处编辑）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：**low-medium**（一句话证成：全部是文面——G1 新增一节映射、G2 三处定义 / 流程 / 技能增补与两处消费者句对齐，零脚本、零门、零写权矩阵变更；但 planner finalize 新增一条核对步、uiux-author 定义扩大写入纪律，属行为面文面化，故不取 low）。
> **review profile**：**G1+G2**（G1 = `docs/DEV-GOVERNANCE.md`；G2 = `.claude/agents/**` 两件、`.claude/skills/**` 一件、`.claude/references/**` 一件、`.claude/commands/**` 一件）；另 `docs/debt/gov/**` 两条目记账。**G3 零触碰**（scope-map / settings / verify-gates / hooks / scripts 全零 diff）、契约面零触碰、spec 两文件零触碰、实现面零触碰、`docs/ui/**` 图夹零 diff。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
docs/DEV-GOVERNANCE.md
.claude/agents/planner.md
.claude/skills/opendesign-connect/SKILL.md
.claude/agents/uiux-author.md
.claude/references/requirement-change.md
.claude/commands/requirement-change.md
docs/debt/gov/ARQ-261014-closeout-obligation-write-authority-vacuum.md
docs/debt/gov/ARQ-261019-opendesign-shoot-script-precedent.md
```

---

## 0. 想实现的效果与差距

**目标效果**（P7.5 两条，逐条对应一个 ARQ 条目）：

1. **收口义务的写点单源**（T1 + T2）：把「收口期该落的制品由谁写」从每包收口期现场即兴找写点，变成 `docs/DEV-GOVERNANCE.md` §6 一张固定映射表；planner 在 finalize 前按表逐行核对本包会产出的收口义务，并按**持会话 agent 粒度**核写权（Pod 主会话是 planner agent，不是裸主线程）。
2. **出图管道成文 + 母版单写者**（T3 + T4 + T5）：出图 = 仓内 `tools/asset-pipeline/shoot_design.mjs` 管道，Open Design 失联只挡设计生成、不挡出图；requirement-change 流程与角色表、命令入口、团队表四处消费者同批对齐，不再留「出图跑 od_*」的反向指引；uiux-author 定义补「母版 / 增量页单写者」约定，防全文件 Write 覆盖主线程合并的完整母版。

**现状差距**（起草期 2026-10-27 实跑或亲核，命令与输出见 §5 基线表）：

- `DEV-GOVERNANCE.md` 现行与「改什么 / 谁批准」相关的表只有 §7「入口速查」（`:224-240`，列义 = 我想改什么 / 入口 / 批准）——它是**入口与批准路由**，不是收口写点映射；全仓 `收口义务 → 承载写点映射` 字面 **0 文件**（§5 B7，排除本批目录）。
- `planner.md` 全文无「收口写点核对」类 finalize 前置项；最近邻接物是「派发书三条硬约定」②（逐派发核写权，不是包级收口义务核对），且该条只覆盖派发那一刻，覆盖不到 closeout 相位。
- `opendesign-connect/SKILL.md` 全文无 `shoot_design` 字面（§5 B7 census：`shoot_design` 全仓 11 文件命中，SKILL 不在其中）。
- `uiux-author.md:34` 原文仍是「- 不执行出图（od_* 归主线程）；OpenDesign 失联 → 如实标「出图 pending」交回主线程挂 human-decision」——把出图挂在 OD 联通上，与本仓 2026-10-19 起的实况（OD 失联、`shoot_design.mjs` 全链成功）相反。
- `references/requirement-change.md:45`（4b）与 `commands/requirement-change.md:18`（4.5）都写「按规格单跑 OpenDesign MCP（od_*）回填 PNG」——这是「出图依赖 OD」误解的现行程序载体；`references/requirement-change.md:75` 角色表同口径（`od_* 归主线程`）。

**非目标（本批不做，防 scope creep）**：

- **不改 `scope-map.json` 写权矩阵**：候选③（账本登记类机械通道）不进默认改法集，单列 §4 OQ-1 交 human 拍；本批 scope-map 零 diff。
- **不新增门、不改任何闸判据**：hooks / scripts / settings / verify-gates 零 diff；T2 的核对步是**惯例非门**（漏写不判红）。
- **不改 spec 两文件、契约面、实现面**：`docs/requirements.md` / `docs/architecture.md` / `docs/contracts/**` / `firmware/**` / `server/**` 零 diff。
- **不动 `docs/ui/**` 图夹**：零搬图、零 PNG 增删；T3 / T4 只引用 P5 落地的声明面口径，不写第二套版本语义。
- **不把 uiux-author 加进 Pod cast**：per-session cast 属 harness 会话级配型，留根 §6-1。
- **不修 SKILL description 的死指针**（指向不存在的 `.claude/references/uiux-opendesign-loop.md`）：留根 §6-3，本批不动 description 边界句。
- **不建 `.claude/references/uiux-opendesign-loop.md`**：出图程序知识本批收在 SKILL「出图管道」节。

## 1. 判档依据（Track B 主路径）

**为什么恒 Track B**（gov-change 轻路径判据逐条）：

- **判据①（path 面）**：轻路径要求 diff 全部落在 `.claude/references/**`、`.claude/skills/**`、`docs/notes/**`、`docs/templates/**`。本批触 `docs/DEV-GOVERNANCE.md`、`.claude/agents/**`、`.claude/commands/**`、`docs/debt/gov/**` —— 判据①即不满足。
- **判据③（排除集）**：明确点命 `docs/DEV-GOVERNANCE.md`、`.claude/agents/**`、`.claude/commands/**`——**命中任一 → 恒 Track B，哪怕只改一个字符**。本批三项全中。
- **性质**：不是措辞 / 排版级（新增映射表一节、新增 finalize 核对步、新增 skill 一节、agent 定义五处改写），判据②亦不满足。

**Track A 段判据（逐条不命中）**：

- ① human 立项时方向未收敛——不命中：两条 ARQ 已自带建议修法，P7.5 立项 ack 已裁「修法照各条目建议改法」（④ 取 ①+②，⑤ 取条目三改进项）。
- ② 评审中发现方向级缺口——待判官；若判官认为映射表的写点裁定本身存在未裁的根本分叉（例如「设计稿母版写点是否该换授给某 agent」），按只升不降升 Track A 段。
- ③ 声明红队——无。

**判档只升不降**；本稿自评 Track B 主路径，判官可改判升档。

## 2. 改法清单（T1…T6）

| T | ARQ | 落点 | 编辑数 |
|---|---|---|---|
| T1 | `ARQ-261014` 修法①（映射成文） | `docs/DEV-GOVERNANCE.md` §6 新增映射表 + §7 分工句 | 2 |
| T2 | `ARQ-261014` 修法②（finalize 核对） | `.claude/agents/planner.md` Step 8c | 1 |
| T3 | `ARQ-261019` 改进项①（skill 先例段） | `.claude/skills/opendesign-connect/SKILL.md` 新增「出图管道」节 | 1 |
| T4 | `ARQ-261019` 改进项②③（定义增补 + 单写者） | `.claude/agents/uiux-author.md` 五处 | 5 |
| T5 | `ARQ-261019` 真因记录（消费者对齐） | `docs/DEV-GOVERNANCE.md` §2 · `references/requirement-change.md` 两处 · `commands/requirement-change.md` 两处 | 5 |
| T6 | 两条目结项 / 追注记账 | `docs/debt/gov/` 两件 | 3 |

**装配与自证**：17 处编辑以旧串锚定装配（`node .claude/scripts/assemble-edits.mjs`，清单 = `delivery/review/261027-gov-review-p75-closeout-shoot-cluster/_scratch/edits.json`；该 `_scratch/` 目录落地 commit 前删除），仓外副本 `C:/tmp/p75-assembly4`。装配结果：`ok:true`、8 件；17/17 旧串在现盘恰命中 1 次、17/17 新文在副本恰命中 1 次且在现盘 0 命中（§5 B1–B3）。本 §2 引文块与清单逐条对账（对账脚本按「原文块 = `oldStr`、改后块 = `newStr`」归一比对，17/17 配对、未配对 0），非手抄转录。

### 2.0 复证结论（起草期 2026-10-27；ARQ 行是待验证假设）

**复证 A · `ARQ-261014` —— 根因成立；CDQ 子项确已闭合；2610-25 三子项逐条可裁。**

- **CDQ 子项已闭合**：条目 2026-10-19 追注在案——CDQ 单文件队列已并入 `docs/debt/contract/` 域并删原文件，新写点落在 integrator / adjudicator 既有写槽内。映射表把它收编成文（T1 第一行），不重复立。
- **三条义务仍无载体**：`DEV-GOVERNANCE.md` 无写点映射（§0 已举）；`planner.md` 无 finalize 核对（同）；`delivery/intake/**` 与 `docs/notes/**` 在 `.claude/scope-map.json` 的**任何 agent allow 里都不出现**（实测 `grep -n "notes\|intake" .claude/scope-map.json`：`docs/notes` 仅命 `universal.docsTopLevel.allowDirs` 一行 `:46`，`intake` 仅命一条 `_doc` 注释 `:23`——均非 per-agent 授权）。
- **2610-25 三子项处置**（逐条裁，如实标）：②「Pod 主会话 ≠ 裸主会话写权口径」→ **收编**（T1 映射表边界句 + T2 同句）；③「acceptance-author taskpackage 元数据回填被拦」→ **收编**（T1 taskpackage 行：写点 = planner，派发书不要求其回写）；①「Pod 会话派集无 uiux-author」→ **半收编**（映射表钉「Pod cast 缺省不含 → 母版 / 出图归 Pod 外主线程」），**cast 本身留根 §6-1**。
- **修法候选③ 未取**：账本登记类机械通道涉及 scope-map 写权矩阵，按 P7.5 立项记录单列 §4 OQ-1。

**复证 B · `ARQ-261019` —— 三条改进项全部仍在位；脚本用法以头注实读为准。**

- **落点状态**：`grep -F "shoot_design" .claude/skills/opendesign-connect/SKILL.md` = 0；`uiux-author.md:34` 反向指引在位（原文见 §0）；母版覆盖事故两次有 261019 批实测记录（条目 trigger :8）。
- **脚本实读（`tools/asset-pipeline/shoot_design.mjs:1-6` 头注 + 通读 73 行）**：用法 `node shoot_design.mjs <html 绝对路径> <输出目录>`；playwright-core + `chromium.launch({ channel: 'msedge', headless: true })`；逐 `.dev[data-od-id]` 元素与整数 clip 精确截图；**只截 400×300 整幅画布**、`comp-*` 等自定义小尺寸跳过；输出 `<outDir>/<id>.png`；`sharp` 灰度 + `threshold(128)`。**条目原文「每批只捡变更帧」不是脚本行为**（脚本每次全画布全出），是入库纪律——T3 文面按此分开写。
- **P5 口径咬合**：母版现行形态以 `docs/ui/UI-DESIGN-VERSIONS.md` 的 `current:` 为准（v7），活图夹 / 冻结快照语义该声明面已载；T3 只留指针、不复述，避免第二套口径。
- **入库帧先例实测**：`docs/ui/V6/` = 1 个 PNG（`p3-body-unavailable.png`），`docs/ui/V7/` = 3 个 PNG（`p3-recording-guide.png` / `p3-recording.png` / `state-organizing.png`）；v6 / v7 母版唯一 `data-od-id` 数（`grep -o` 去重计数）= 61 / 64。

**复证 C · 写点映射的 scope-map 依据（逐条亲读 `.claude/scope-map.json`，本批零改动）**：planner allow = `delivery/taskpackages/**` + `delivery/workstreams/**`、deny `docs/**`（`:60` 起）；integrator allow 含 `docs/debt/**`、`docs/templates/**`、`docs/decisions/ADR.md`（`:146` 起）；adjudicator allow 含 `docs/debt/**`（`:241` 起）；uiux-author allow = `docs/ui/**`（`:264` 起）；architect allow 含两 spec 文件、不含 `docs/debt/**`；无任何 agent allow 覆盖 `delivery/intake/**`、`docs/notes/**`。

### T1 · 收口义务 → 承载写点映射成文（DEV-GOVERNANCE §6 新增节 + §7 分工句）

**问题**：收口期该落的制品——设计稿母版内容级同步、出图、spine 回填、spec 挂法句、账本条目登记、`docs/notes` 经验记录、taskpackage 元数据回填——没有「谁写」的单源。每包收口期现场即兴找写点，找不齐就升级 human 裁「主线程直落」；同型复发四包（2026-10-14 loop-retro T4）后登记 ARQ，又复发三包（2026-10-25 T10）。更深一层：Pod 主会话是 planner agent，受 per-agent 写权约束（`deny docs/**`），plan 里写的「主线程通道代落」在 Pod 内根本不成立。

**证据**：① `DEV-GOVERNANCE.md:224-240`（§7「入口速查」）= 「我想改… / 入口 / 批准」三列，是入口与批准路由，不含写点；全仓 `收口义务 → 承载写点映射` 字面 0 命中（§5 B7）。② 2610-25 三包实例：261015-p1-auxbar（Pod 可派集无 uiux-author，真母版 `docs/ui` 无人可写）、261016-sd-card-mount（`docs/notes/**` 无任何 agent allow，且 `--agent planner` 起的 Pod 主会话受 per-agent 约束）、261022-m5-k1（acceptance-author 写槽不含 taskpackage，元数据回填被拦）。③ scope-map 逐条亲读见复证 C：`delivery/intake/**` 与 `docs/notes/**` 在全部 agent allow 中零覆盖。

**根因**：`DEV-GOVERNANCE.md` §6 的权威分布只到「治理面（G1–G4）+ authority / owner / writer / approver」这一层，没有下沉到「具体收口义务 → 承载写点」；执行侧只能每包临场判断，而 per-agent 写权是机械的——临场判断必然撞墙，撞墙后唯一出路是升级 human。

**改法**（两处；并落理由：映射表本体放 §6，§7 只加一句分工边界，不复述表内容）：

一、§6 末尾、§7 标题之前插入映射小节（锚串 = §7 标题行 `## 7. 入口速查`，现盘恰 1 次命中；插入内容包括表体与「为什么成文」段）。原文（`DEV-GOVERNANCE.md:224`，逐字节）：

```text
## 7. 入口速查
```

改后（逐字节）：

```text
**收口义务 → 承载写点映射（261027-gov-review-p75-closeout-shoot-cluster · 承载 ARQ-261014）**

**这张表回答「收口期该落的制品由谁写」。** 与 §7「入口速查」分工：§7 管「从哪个入口改、谁批准」，本表管「收口落盘时写点在哪」；同一义务两表都出现时，批准看 §7、写点看本表，两表不得互相扩写。边界：本表只收**收口期落盘、且 §7 未给具体写点**的义务——ADR 条目、`delivery/review/**` 记账等已由 §7 载明的义务不并入；§7 末行的「ARQ / CDQ｜按来源流程记账」只指入口与账本归属，具体写点由本表第一行补全。

| 收口义务 | 承载写点 | 依据 / 边界 |
|---|---|---|
| 账本条目登记（`docs/debt/<域>/`，原 CDQ 行） | **主线程**（登记动作）；包内收口可落 = integrator（`docs/debt/**` 写槽）、review 收口可落 = adjudicator（同写槽） | 261019 批 CDQ 已并入 `docs/debt/contract/`，写权真空闭合；planner / architect 无该写槽 |
| 设计稿母版内容级同步（`docs/ui/kidpal-ui-design-vN.html`） | **主线程**（Pod 外的真主线程会话）；需求面 loop 档 2 在场时由 uiux-author 增量修订 | Pod cast 缺省不含 uiux-author，且 Pod 主会话受持会话 agent 的 per-agent 写权约束——「主线程通道代落」在 Pod 内不成立；母版升版与声明面纪律见 `docs/ui/UI-DESIGN-VERSIONS.md` |
| 出图（400×300 PNG 画布帧） | **主线程** | 管道 = `node tools/asset-pipeline/shoot_design.mjs <母版绝对路径> <输出目录>`，不经 Open Design；用法见 `.claude/skills/opendesign-connect/SKILL.md`「出图管道」节 |
| spine 回填（`delivery/intake/<changeId>.md` 的 `anchors` / `status`） | **主线程** | 无任何 agent 的 scope-map allow 覆盖 `delivery/intake/**` |
| spec 挂法句（`docs/requirements.md` / `docs/architecture.md` 的同步句） | **architect**（各自面 loop 内）；dev 包检出 spec 漂移 → 交 architect、零自改 | `ARQ-260929-docs-sync-spec-handoff` 已结裁定：integrator 对两份 spec 文件零写权 |
| 经验记录（`docs/notes/**`） | **主线程**（Pod 外） | 无 agent allow 覆盖 `docs/notes/**`；Pod 主会话 = planner agent，`deny docs/**` |
| taskpackage 元数据回填 | **planner** | acceptance-author 写槽不含 taskpackage；派发书不要求其回写，交付内容由 planner 预指派代落（planner「派发书三条硬约定」②） |

**为什么成文**：这类义务原先每包收口期现场即兴找写点，找不齐就升级 human 裁「主线程直落」，同型复发四包（2026-10-14 loop-retro T4）、三包（2026-10-25 T10）。写点在这里一次钉死；planner 的 finalize 核对（`.claude/agents/planner.md` Step 8c）按本表执行。**本表是文面映射，不构成任何 scope-map 换授**——写权矩阵以 `.claude/scope-map.json` 为准，二者不一致时以 scope-map 为真、并按治理变更修本表。

## 7. 入口速查
```

二、§7 开头索引句 append 分工边界（锚串 = 该句，现盘恰 1 次命中）。原文（`DEV-GOVERNANCE.md:226`，逐字节）：

```text
本表是 §1–§6 的索引，不是规则本身。
```

改后（逐字节）：

```text
本表是 §1–§6 的索引，不是规则本身。**写点 / 写权类问题不在本表**——收口期该由谁落盘见 §6「收口义务 → 承载写点映射」，本表只答「从哪个入口改、谁批准」，两表不得互相扩写。
```

**咬合裁定（落点二选一，取 §6）**：试过把写点并进 §7 当第四列——§7 是「入口 / 批准」两轴表，加写点轴会让一表承担三种语义，且 §7 的四类「我想改」粒度（contracts / spec / agent 定义 / 命令）与收口义务粒度（母版 / 出图 / 账本行）不匹配，硬并只能写成含糊映射。取 §6：§6 本就是 authority / owner / writer / approver 的所在节，写点下沉到具体义务落这里语义顺；§7 只留一句边界句防漂移。**边界句为什么必须有**：两表并存的最大风险是下一批把同一义务在两表各写一份、然后一边改了另一边没改——分工句把「批准看哪、写点看哪、不得互相扩写」三件事钉在同一处。

**为什么这样改（对哪个根因）**：根因是「权威分布不下沉到具体义务」——映射表把七类高频义务逐行钉死写点，是直接补下沉层；§7 边界句与「本表不构成 scope-map 换授」句共同封住两个次生根因（双表漂移、映射被读成换授）。表只收「§7 未给写点」的义务，避免与 §7 重复。

目标文件：`docs/DEV-GOVERNANCE.md`（改）

### T2 · planner finalize 写点核对前置项（Step 8c）

**问题**：T1 的映射表若要生效，执行侧得有一处「按表核本包」的动作。现有 `planner.md` 没有：Step 8 finalize 核的是验收件 / brief / crossEnd，不是收口写点；「派发书三条硬约定」② 只在每次派发前核该次派发的写权，覆盖不到 closeout 相位的收益类义务（账本行 / spine / docs-notes / 母版）。于是 Pod 收口期才知道「这个我写不了」，只能升级。

**证据**：`planner.md:85`（Step 8 Finalize 全文，无写点核对）；`:86`（Step 8b，核验收方向与 story 忠实度）；`:87`（Step 9，直接派 builder）。2026-10-25 T10 三包实例同 T1 证据②。

**根因**：写权知识散在 scope-map（机械）与个人经验（非机械）之间，planning 相位缺一个把两者对齐到**本包具体收益义务**的检查点；且执行者习惯把「主线程」当成一个到处可用的通道，而 Pod 内根本没有裸主线程。

**改法**（一处；插在 8b 与 Step 9 之间，不并进 Step 8 正文——8b 是同类独立子步，编号顺延最贴近既有形态）：原文（`planner.md:87` 行首，逐字节）：

```text
9. 派 `builder` 在目标包内实现 + 单元测试：
```

改后（逐字节）：

```text
8c. **收口写点核对（finalize 前置项；纯纪律步、零新门；261027 批 · ARQ-261014）**：Step 8 finalize 收尾前，对照 `docs/DEV-GOVERNANCE.md` §6「收口义务 → 承载写点映射」逐行核本包会产出的收口义务（设计稿母版 / 出图 / spine 回填 / spec 挂法句 / 账本条目 / docs-notes 记录 / taskpackage 元数据）：命中 → 在 taskpackage `notes` 写明「义务项 + 承载写点 + 在哪个相位由谁写」；未命中写一句 `none`。**按持会话 agent 粒度核写权**——本 Pod 主会话是 planner agent（`deny docs/**`），不是裸主线程，「主线程通道代落」在 Pod 内不成立：写点不在本 Pod cast 的义务，明写「出 Pod 后由主线程直落」或「合并后补」，不得留到 closeout 现场即兴找写点。惯例非门：漏写不判红，但 closeout 撞 enforce-scope 时以本条为返工判据。
9. 派 `builder` 在目标包内实现 + 单元测试：
```

**与既有条目的分界（防双头）**：① 「派发书三条硬约定」② 核的是**单次派发**的写权（谁落盘、落哪个目录），8c 核的是**本包收口义务**的写点（谁在哪个相位写）——输入不同、时点不同，互补不重叠，派发书②不代 8c。② 8c 不进 Step 8 正文、也不进 Step 3n 的 nano 豁免清单：nano 包同样适用（收口义务一般为零，一行写 `none` 即闭合）；若判官认为 nano 应豁免，须显式写进 3n 并说明「nano 包不产生收口义务」的判据（本批不预做）。

**为什么这样改（对哪个根因）**：把映射表接进 planning 相位，且在同一句里钉死「持会话 agent 粒度」——这是 2610-25 子项② 的直接处方，避免同类误判再靠 human 现场纠偏。惯例非门（不改闸）符合 ARQ 原条「纯文面，不改闸」的定性。

目标文件：`.claude/agents/planner.md`（改）

### T3 · 出图管道先例段（opendesign-connect SKILL 新增节）

**问题**：本仓出图（设计 HTML → 逐画布 PNG 帧）自 2026-10-19 起实际走仓内 `shoot_design.mjs`，但这条先例只活在个别批的 intake / retro 里，skill 面零字面。后续批遇到 OD 失联时，能读到的指引只有「排障 OD」，于是出图被误判成堵死、挂 human-decision——261019-p3 批实测过这条反径（OD daemon 失联，按旧指引本应挂 human-decision，human 当场指出「出图应该不用 Open Design」）。

**证据**：① `grep -F "shoot_design" .claude/skills/opendesign-connect/SKILL.md` = 0。② 脚本实读（`tools/asset-pipeline/shoot_design.mjs` 73 行）：用法 `node shoot_design.mjs <html 绝对路径> <输出目录>`；无头 Edge 逐 `.dev[data-od-id]` 元素截图、只截 400×300 整幅画布、`<id>.png` 命名、灰度 128 阈值化。③ 全仓 `shoot_design` 11 文件命中，其中 canonical 消费者 = 3 个母版 HTML、声明面 `docs/ui/UI-DESIGN-VERSIONS.md`、`docs/requirements.md` 变更记录——出图早已走管道，只有 skill 面没写。

**根因**：`opendesign-connect` skill 的边界句自设为「只做联通诊断与恢复指引」，随后把「出图」误归入 OD 联通问题；但出图是纯本地渲染，与 OD daemon 无依赖关系。知识错位导致「OD 失联 → 出图 pending → human 门」的连锁。

**改法**（一处；插在「配置更新与生效」与「约束」之间——先给管道事实，再守约束）：原文（`SKILL.md:26`，逐字节）：

```text
## 约束（must / never）
```

改后（逐字节）：

```text
## 出图管道（出图不走 OD；261027-gov-review-p75-closeout-shoot-cluster 成文 · 承载 ARQ-261019）

**出图 = 把设计 HTML 母版渲染成逐画布 PNG 帧，不经过 Open Design。** od_* 失联只挡设计生成，不是出图堵点；出图不必先确认 OD 联通，也不必挂 human-decision。

- **命令**：`node tools/asset-pipeline/shoot_design.mjs <母版 html 绝对路径> <输出目录>`
- **行为**（以脚本头注为准）：playwright-core 起无头 Edge，逐 `.dev[data-od-id]` 元素精确截图；只截布局恰为 400×300 的整幅画布（自定义小尺寸的 `comp-*` 组件预览自动跳过）；按 `data-od-id` 命名输出 `<id>.png`；灰度 128 阈值化、输出 1bpp 纯黑白（真机观感的唯一真相版）。
- **输入与落点**：母版与落点一律以声明面 `docs/ui/UI-DESIGN-VERSIONS.md` 为准（`current:` 指针 +「活图夹与冻结快照」节）；版本语义、活图夹目录形态与快照口径均在该声明面，本 skill 不复述、不另立。
- **只捡变更帧入库**：脚本每跑一次全画布全出，入库只捡本批变更帧（先例：`261016-p3-story-body-sd-cache` 母版 61 个 `data-od-id` 中入库 1 帧 = `docs/ui/V6/`；`261019-p3-record-dialogue-guide` 母版 64 个 `data-od-id`、出图 15 画布、入库 3 帧 = `docs/ui/V7/`），未变帧不进版本控制。
- **先例**：261019-p3-record-dialogue-guide 批 OD daemon 失联，按本管道全链成功（15 画布出图、三帧入库），出图未因 OD 缺席挂 human-decision。

## 约束（must / never）
```

**为什么不改 skill 的 description**：description 的边界句「不做设计生成规约（那在 `.claude/references/uiux-opendesign-loop.md`）」指向一个从未建成的 reference（260929 批留根不建），是预置死指针——修它属 description 面改动、会牵路由信号；本批留根 §6-3，不夹带。

**为什么这样改（对哪个根因）**：把「出图 = 本地渲染管道」写成 skill 可读事实，且输入 / 落点 / 入库纪律三件齐（缺任一，执行侧仍会回落到「问 OD」或「全量入库」）。版本语义只留指针到 P5 声明面，不写第二套。

目标文件：`.claude/skills/opendesign-connect/SKILL.md`（改）

### T4 · uiux-author 定义增补（出图分工 + 母版 / 增量页单写者）

**问题**：uiux-author 的定义把「出图」与「OD」绑死（`:3` description、`:12` Role、`:27` Step 3、`:34` Boundaries 四处 `od_*`），且没有写「母版只有一个写者」——261019-p3 批实测其全文件 Write 两次把主线程合并的完整 v7 母版覆盖回 3 帧增量页（R1 回归）。

**证据**：`uiux-author.md:34` 原文「- 不执行出图（od_* 归主线程）；OpenDesign 失联 → 如实标「出图 pending」交回主线程挂 human-decision」；`:12`「出图（OpenDesign MCP od_*）由主线程代执行」；`:3` description「不执行出图（od_* 归主线程）」；`:27` Step 3「交主线程代执行出图（od_*）」。母版覆盖事故两例在 `ARQ-261019` trigger :8 与 261019 批 intake / retro 记录。

**根因**：出图通道换代（OD → 本地脚本）后，agent 定义里残留旧通道描述；同时「谁写母版」没有约定，全文件 Write 语义让「增量页」作者天然能整份覆盖他方合并结果。

**改法**（五处；同一定义内逐处最小改，不重写段落）：

一、description 出图口径（`uiux-author.md:3`）。原文（逐字节）：

```text
不执行出图（od_* 归主线程）；dev-loop 包内不派本 agent
```

改后（逐字节）：

```text
不执行出图（出图归主线程）；dev-loop 包内不派本 agent
```

二、Role 段出图口径（`uiux-author.md:12`）。原文（逐字节）：

```text
出图（OpenDesign MCP od_*）由主线程代执行，不走本 agent 工具面。
```

改后（逐字节）：

```text
出图（`tools/asset-pipeline/shoot_design.mjs` 管道，不经 OpenDesign）由主线程代执行，不走本 agent 工具面。
```

三、Step 2 末尾追加单写者约定（`uiux-author.md:26` 段末）。原文（逐字节）：

```text
机检在现行稿相对前序版本塌缩过半时另会显式报「母版完整性」并中止，而不是把母版缺口报成 story 锚漂移。
```

改后（逐字节）：

```text
机检在现行稿相对前序版本塌缩过半时另会显式报「母版完整性」并中止，而不是把母版缺口报成 story 锚漂移。
   **母版 / 增量页单写者约定（261027 批 · ARQ-261019）**：同一时刻 `kidpal-ui-design-vN.html` 完整母版只有一个写者。主线程已把完整母版合并落盘时，你的修订只能「盘上现行母版基座 + 你的增量」合成后整份 Write（本 agent 工具面无 Edit）——Write 前先读盘上现行文件，确认其内容是你的产出基线；盘上母版含非你产出的内容（典型 = 主线程合并的完整版本）→ 停，把增量页 / 补丁说明交回主线程合并，不得用自己手里的增量页整份覆盖。整份覆盖曾两次把主线程合并的完整母版打回增量页（261019-p3 实测）。
```

四、Step 3 出图交付句（`uiux-author.md:27`）。原文（逐字节）：

```text
交主线程代执行出图（od_*）
```

改后（逐字节）：

```text
交主线程按 `tools/asset-pipeline/shoot_design.mjs` 管道出图（不经 OpenDesign）
```

五、Boundaries 出图与失联口径（`uiux-author.md:34`）。原文（逐字节）：

```text
- 不执行出图（od_* 归主线程）；OpenDesign 失联 → 如实标「出图 pending」交回主线程挂 human-decision，不自行排障重试超过 skill 指引范围。
```

改后（逐字节）：

```text
- 不执行出图（出图管道归主线程，不经 OpenDesign）；OpenDesign 失联只挡设计生成：如实标「生成 pending」交回主线程，不自行排障重试超过 skill 指引范围。
```

**与 P5 批落地的咬合（不写第二套口径）**：Step 2 的母版命名族 / 声明面纪律、Step 3 的活图夹 / 冻结快照语义都是 P5 批（261027-gov-review-p5-spike-anchor-cluster）刚落地的现行文面，本批只在旁边追加单写者约定与出图通道口径，**不改写、不复述** P5 的版本语义；单写者约定与 P5「分块用临时命名、构建完整后一次重命名落位」不冲突——P5 管升版落盘原子性，本约定管「母版已有他方内容时不得整份覆盖」。

**为什么这样改（对哪个根因）**：通道换代残留与写者缺位是两件事，但都落在同一份定义上——五处最小改各答一问（通道是什么、谁执行、增量如何落、覆盖前读什么、失联怎么标），不重写段落以保既有 P5 文面逐字不动。

目标文件：`.claude/agents/uiux-author.md`（改）

### T5 · 出图消费者对齐（四处现行程序文面）

**问题**：T3 / T4 把「出图不走 OD」写进 skill 与 agent 定义后，流程 SSOT 仍写着「跑 OpenDesign MCP（od_*）回填 PNG」——同一动作两套指引，执行侧按哪份都说得通，等于没改。

**证据**：`references/requirement-change.md:45`（4b）：「**出图执行 = 主线程代执行**：按规格单跑 OpenDesign MCP（od_*）回填 PNG 落 `docs/ui/`，文件名对齐 pageId/stateId。」；`commands/requirement-change.md:18`（4.5）：「**主线程代执行出图**：按规格单跑 OpenDesign MCP（od_*）回填 PNG 落 `docs/ui/`（…）」「od_* 失联挂 human-decision 事件、产物标「出图 pending」」；`references/requirement-change.md:75` 角色表：「不执行出图（od_* 归主线程）」；`DEV-GOVERNANCE.md:54` 团队表：「出图 od_* 归主线程」。

**根因**：261019 批的出图换代发生在执行面（主线程实测脚本），未回灌流程文面；261027 P5 批只改版本语义，未触出图通道句。DEV-GOVERNANCE 治理变更步骤 4 要求「改源时同批同步本文件内的全部投影处」——本批按此把四处投影同批收齐。

**改法**（五处；三份文件）：

一、`DEV-GOVERNANCE.md:54` 团队表 uiux-author 行。原文（逐字节）：

```text
出图 od_* 归主线程
```

改后（逐字节）：

```text
出图归主线程（`shoot_design.mjs` 管道）
```

二、`references/requirement-change.md:45`（4b）出图执行句。原文（逐字节）：

```text
**出图执行 = 主线程代执行**：按规格单跑 OpenDesign MCP（od_*）回填 PNG 落 `docs/ui/`，文件名对齐 pageId/stateId。
```

改后（逐字节）：

```text
**出图执行 = 主线程代执行**：按规格单从设计 HTML 母版跑 `node tools/asset-pipeline/shoot_design.mjs <母版绝对路径> <输出目录>` 回填 PNG 落 `docs/ui/`，文件名对齐 pageId/stateId（出图不经 OpenDesign——od_* 失联不阻塞出图，管道与用法见 `.claude/skills/opendesign-connect/SKILL.md`「出图管道」节）。
```

三、`references/requirement-change.md:75` 角色表。原文（逐字节）：

```text
不执行出图（od_* 归主线程）；不碰需求文字面与契约面
```

改后（逐字节）：

```text
不执行出图（出图管道归主线程）；不碰需求文字面与契约面
```

四、`commands/requirement-change.md:18`（4.5）出图执行句。原文（逐字节）：

```text
**主线程代执行出图**：按规格单跑 OpenDesign MCP（od_*）回填 PNG 落 `docs/ui/`（文件名对齐 pageId/stateId，PNG = builder 布局坐标定位权威）
```

改后（逐字节）：

```text
**主线程代执行出图**：按规格单从设计 HTML 母版跑 `node tools/asset-pipeline/shoot_design.mjs <母版绝对路径> <输出目录>` 回填 PNG 落 `docs/ui/`（文件名对齐 pageId/stateId，PNG = builder 布局坐标定位权威；出图不经 OpenDesign——od_* 失联不阻塞出图，管道见 `.claude/skills/opendesign-connect/SKILL.md`「出图管道」节）
```

五、`commands/requirement-change.md:18`（4.5）失联句。原文（逐字节）：

```text
od_* 失联挂 human-decision 事件、产物标「出图 pending」，不阻塞需求文字面先行。
```

改后（逐字节）：

```text
设计生成走 od_* 时失联挂 human-decision 事件、产物标「生成 pending」，不阻塞需求文字面先行。
```

**咬合裁定（为什么连命令面一起改）**：4.5 是命令入口里可被直接执行的步骤文本，4b 是流程 SSOT 的对应步骤——两者是同一动作的两处投影，只改一处会造成「入口说 od_*、流程说脚本」的分裂。命令面这处只是**程序步骤文本的通道句**替换，不改命令参数、不改任何门与路由，review 仍走本批 Track B（判官可 challenge 是否应按「commands 面」另加审；本稿认为属于既有步骤的消费者对齐，不新立行为）。

**为什么这样改（对哪个根因）**：根因是「换代未回灌」——五处替换把旧通道字面清零，并统一指回 skill「出图管道」节（单源），不在四处各写一份用法细节。

目标文件：`docs/DEV-GOVERNANCE.md`（改） · `.claude/references/requirement-change.md`（改） · `.claude/commands/requirement-change.md`（改）

### T6 · 两条目结项 / 追注记账

**问题**：ARQ-261019 的三条改进项本批全部落地，条目却仍是 `status: deferred` 且无 `closed` / `closedBy` 字段——下一轮 loop-retro 会把已消化面当未治面重复立项；ARQ-261014 修法 ①+② 落地、子项①与候选③留根，若不写明处置，同样会被读成「整条未治」。

**证据**：`ARQ-261019-...md:5`（`status: deferred`，frontmatter 无 `closed:` / `closedBy:` 行）；`ARQ-261014-...md:24` 末段（三次追注的最后一段，止于 `delivery/retro/2026-10-25-loop-retro.md §二 T10。`）。

**根因**：账本条目 append-only，状态与处置只能靠 frontmatter 状态字段 + 文末追注承载；修法落地批不记账，条目状态就与实况脱节（`ARQ-261014` 已两次靠追注纠偏即先例）。

**改法**（三处；结项 / 追注文本逐字给定，落地归主线程 packet-bound）：

一、`ARQ-261019` frontmatter 结项翻转（`:5`）。原文（逐字节）：

```text
status: deferred
```

改后（逐字节，保留文件原 CRLF 行尾）：

```text
status: closed
closed: 2026-10-27
closedBy: 261027-gov-review-p75-closeout-shoot-cluster
```

二、`ARQ-261019` 文末结项追注（append 在正文末段之后，空一行）。原文（逐字节）：

```text
3. **真因记录**：OD daemon 端口动态 + 应用拉起失败是常态场景，「出图依赖 OD 应用」是历史误解，skill 既有文本会误导后续批挂等 OD。
```

改后（逐字节）：

```text
3. **真因记录**：OD daemon 端口动态 + 应用拉起失败是常态场景，「出图依赖 OD 应用」是历史误解，skill 既有文本会误导后续批挂等 OD。

**结项追注（2026-10-27，261027-gov-review-p75-closeout-shoot-cluster 批；R1 F1 修订）**：三条改进项全部落地——① `opendesign-connect/SKILL.md` 增补「出图管道」节（命令 / 行为 / 落点 / 只捡变更帧 / 先例）；② `uiux-author` 定义增补出图与母版写入分工（description / Role / Step 3 / Boundaries 四处通道句同步）；③ 真因记录随 ① ② 成文消解——skill「出图管道」节与定义四处均明写「OD 失联只挡设计生成」，条目所述「skill 既有文本会误导后续批挂等 OD」的口径已清除。另落条目 **trigger ③**（正文改进项编号之外的 261019 实测追加项）：母版 / 增量页单写者约定，挂 `uiux-author` Step 2 末尾。连带对齐出图消费者四处（`DEV-GOVERNANCE.md` §2 团队表、`references/requirement-change.md` 4b 与角色表、`commands/requirement-change.md` 4.5）——出图一律走 `shoot_design.mjs` 管道，od_* 只承载设计生成。
```

三、`ARQ-261014` 文末追注（append 在末段之后，空一行；条目维持 `deferred`）。原文（逐字节）：

```text
delivery/retro/2026-10-25-loop-retro.md §二 T10。
```

改后（逐字节）：

```text
delivery/retro/2026-10-25-loop-retro.md §二 T10。

**追注（2026-10-27，261027-gov-review-p75-closeout-shoot-cluster 批）**：修法候选 ①+② 落地，修法候选 ③（CDQ 类机械通道）本批未取。① 收口义务→承载写点映射 = `docs/DEV-GOVERNANCE.md` §6「收口义务 → 承载写点映射」表（与 §7 入口表分工句同批写明，防双表漂移）；② finalize 前置核对 = `planner.md` Step 8c（纯纪律步、零新门）。2610-25 三子项处置逐条：② Pod 主会话 ≠ 裸主会话写权口径 → **收编**（映射表「按持会话 agent 粒度」边界句 + Step 8c 同句）；③ acceptance-author taskpackage 元数据回填被拦 → **收编**（映射表 taskpackage 行：写点 = planner，派发书不要求 acceptance-author 回写）；① Pod per-session cast 无 uiux-author → **半收编、留根**：映射表已钉「Pod cast 缺省不含 uiux-author → 母版 / 出图归 Pod 外主线程」，但「把 uiux-author 加进 Pod 会话 cast」属 harness 会话级配型问题、非写点映射，本批不治。条目维持 deferred，剩余面 = 子项① 与修法候选③。
```

**为什么这样改（对哪个根因）**：结项与追注都写「哪条落到哪、哪条留根、留根理由」——下一轮 retro 读到的是可判的处置记录，不是一坨「已修 / 未修」二选一。记账归主线程（本批只起草文本），`debt-render.mjs` 会在落地后把已结条目自动归位 `docs/debt/gov/_archive/2026/`（纯函数位移，§6-5）。

目标文件：`docs/debt/gov/ARQ-261019-opendesign-shoot-script-precedent.md`（改） · `docs/debt/gov/ARQ-261014-closeout-obligation-write-authority-vacuum.md`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 / 消费方 |
|---|---|---|---|
| 治理文档 G1 | `docs/DEV-GOVERNANCE.md` | 改（T1 / T5） | §6 新增收口写点映射表 + §7 分工句 + §2 团队表通道句；消费方 = planner 起包 / 收口、全体 agent 读写点、判官 |
| 流程定义 G2 | `.claude/agents/planner.md` | 改（T2） | 新增 Step 8c 收口写点核对；消费方 = planner 自身、判官 |
| 技能 G2 | `.claude/skills/opendesign-connect/SKILL.md` | 改（T3） | 新增「出图管道」节；消费方 = 主线程出图、uiux-author、本表其他消费者 |
| 角色定义 G2 | `.claude/agents/uiux-author.md` | 改（T4） | 通道口径四处 + 单写者约定一处；消费方 = 需求面档 2 批次 |
| 流程定义 G2 | `.claude/references/requirement-change.md` | 改（T5） | 4b 出图句 + 角色表；消费方 = 需求面 loop 全链 |
| 命令 G2 | `.claude/commands/requirement-change.md` | 改（T5） | 4.5 两处通道句；消费方 = 需求面 loop 入口 |
| 账本 G4 | `docs/debt/gov/ARQ-261019-…md` · `docs/debt/gov/ARQ-261014-…md` | 改（T6） | 结项翻转 + 两条目追注 |
| 生成件 | `docs/debt/gov/INDEX.md`（+ 归档 INDEX） | 再生 | 落地后 `node .claude/scripts/debt-render.mjs` 刷新；主线程跑，禁手改 |
| 生成件 | `delivery/review/INDEX.md` | 再生 | draft / FROZEN / landed 三转移点 `changes-index.mjs --upsert` |
| 强制执行层 G3 | `.claude/scope-map.json` · `*.settings.json` · `verify-gates.json` · `hooks/**` · `scripts/**` | 未触 | 零 diff；无新增门、无闸判据变化 |
| 契约面 / spec | `docs/contracts/**` · `docs/requirements.md` · `docs/architecture.md` | 未触 | 零 diff |
| 实现面 | `firmware/**` · `server/**` · `tools/**` | 未触 | 零 diff；`tools/asset-pipeline/shoot_design.mjs` 只读引用 |
| 设计面 | `docs/ui/**` | 未触 | 零搬图、零 PNG 增删；T3 / T4 只引声明面口径 |

**【硬项 · sweep 透镜义务】**：本批**不落地新门、不落地新断言 pattern**（无脚本读取新文面），故无「新判定 pattern 全仓预跑」项。新字面碰撞 census（起草期 2026-10-27 实跑，语料 = 全仓文本，排除本批目录与 `.git` / `node_modules`）：

- `收口义务 → 承载写点映射`：**0 文件**（本批为首钉）。
- `出图管道`：**3 文件**——`delivery/review/261027-gov-review-p5-spike-anchor-cluster/PROPOSAL.md`（P5 留根申明「出图管道属 ARQ-261019」，同义不冲突）、`docs/debt/gov/ARQ-261019-opendesign-shoot-script-precedent.md`（处方源文）、`docs/debt/gov/INDEX.md`（由条目 trigger 生成的薄行）。三者都是立项材料 / 历史证据，不构成碰撞语义。
- `shoot_design`：**11 文件**——3 个母版 HTML、声明面、`docs/requirements.md` 变更记录、1 条 intake、1 条 workstream findings、1 条 P5 PROPOSAL、ARQ 条目与 INDEX、脚本本体。本批不改任何既有命中件（只新增引用）。

**【硬项 · 复扫模式完备性先验】**：本批唯一的退役语面 = 「出图走 OpenDesign MCP（od_*）」口径。模式集显式枚举全形态（词形 × 有无 backtick × 目标词）：

- `od_* 归主线程`（含 backtick 词形；本批改 3 个现行载体文件——`DEV-GOVERNANCE.md`、`uiux-author.md` ×2、`references/requirement-change.md` ⇒ 落地后现行载体 0）：全仓现命中 **7 文件**，另 4 件 = `delivery/archive/changes/arch/260929-gov-review-req-loop-ui-author/{architect-landing-summary,landing-verdict}`、`delivery/archive/changes/arch/260929-gov-review-ui-ladder-remap/PROPOSAL.md`、`delivery/review/261019-gov-review-docs-root-drift-cleanup/PROPOSAL.md`——**全部为归档 / 历史批证据，append-only 不回改**，本批不动。
- `按规格单跑 OpenDesign MCP（od_*）`：现命中 **3 文件**，现行载体 2（`references/requirement-change.md` / `commands/requirement-change.md`，本批改）；另 1 = 260929 归档 PROPOSAL，不回改。
- `od_* 失联挂 human-decision`：现命中 **1 文件**（`commands/requirement-change.md`，本批改）。
- `出图 pending`（不含 od_* 的裸词形）：现命中 **6 文件**，现行载体 2（`uiux-author.md` / `commands/requirement-change.md`，本批改）；另 4 = 260929 归档三件 + `delivery/workstreams/gov-debt-reduction/task_plan.md:115`（立项前的复证记录，历史陈述，不回改）。
- 完备性论证：退役语义的三要素是「通道名（od_* / OpenDesign MCP）」「动作（出图 / 规格单 / 回填 PNG）」「失败态（失联 / pending）」；上四条模式覆盖全部「动作 + 失败态」组合，通道名单独出现的合法语境（OD 联通排障、生成期事实）不属退役面，本批保留（SKILL 前半篇与 `od_generate_design` 等条目）。

## 4. human 取舍点（OQ）

**OQ-1 · 账本登记类义务要不要现在开机械通道？（`ARQ-261014` 修法候选③）**

**已裁：human 2026-10-27 拍 (a)**（记录 = `delivery/workstreams/gov-debt-reduction/task_plan.md:117` 与 §7 R1 节）；本稿保留三选项与理由供追溯，FROZEN 时 OQ-1 视为已决——改法集与 (a) 一致（零 scope-map 触碰、候选③ 留根 §6-2）。

背景：账本条目登记（`docs/debt/<域>/`）是收口期高频代落动作。261019 批已把 CDQ 并进 `docs/debt/contract/`、写点落进 integrator / adjudicator 的既有写槽，但「谁登记」在多数包仍是主线程手工补。候选③ 提议给机械通道或专用写槽——**这要动 `scope-map.json` 写权矩阵**，属 G3 变更，爆炸半径与新批相当；P7.5 立项记录已预裁「修法取 ①+②，候选③ 列 OQ」。本批映射表先按现状把写点钉成「主线程（登记动作）+ 既有写槽可落」。

- **(a) 维持文面映射，不开通道**（推荐）：成本零、本批 profile 不变。收益 = 先让映射表落地，积累「哪些义务仍须 human 兜」的真实数据；风险 = 若登记仍频繁回流主线程，主线程仍是瓶颈（但这是既有事实，不是本批新增）。
- **(b) 另批开机械通道**（如给 planner / acceptance-author 一类收口侧角色补窄 allow、或脚本化登记）：收益 = 减一次 handoff；成本 = scope-map 写权矩阵变更 + 可能的脚本批（须过脚本拆分纪律），要独立批 + 独立评审 + human 批准。
- **(c) 本批扩面一起做**：不推荐——把文面批变成写权变更批，review profile 从 G1+G2 扩到 G3，判档与核验成本都会变。

**推荐 (a)（已拍）**：映射表是候选③ 的观察前提；没有它，讨论「机械通道该给谁」只能凭印象。若后续复议，可转 (b)，通道另批、本批落地不变。

**实现层取舍（判官可 challenge，非 human 拍板）**：

- **取舍 A · 映射表落 §6，不并进 §7**：理由与退路见 T1「咬合裁定」；判官若要求并入 §7，该表会被读成第三种语义，本稿不建议。
- **取舍 B · T2 独立编号 8c，不并入 Step 8 正文**：Step 8 是 finalize 动作本体，8c 是核对前置；合并会使 Step 8 一句话承载两件事。退路：并入 Step 8 末句即可，判据不变。
- **取舍 C · T5 连命令面一起改（含 4.5 两处）**：只改 reference 会留下「命令入口说 od_*、流程说脚本」的分裂；命令面改的是步骤文本的通道句，不改参数、不改路由。退路：判官若认为命令面应另批，本批减 T5 中两处、留根同步。
- **取舍 D · T4 五处最小改、不重写段落**：保 P5 批刚落地的版本语义逐字不动；单写者约定作为追加块挂在 Step 2 段末。退路：若判官要求单写者约定提到 Boundaries，可加一条指针句。
- **取舍 E · SKILL description 死指针本批不修**：`opendesign-connect` 的 description 边界句指向从未建成的 `.claude/references/uiux-opendesign-loop.md`；修它属 description 面（路由信号）改动。留根 §6-3。
- **取舍 F · riskTier 取 low-medium 而非 low**：T2 新增一条 finalize 核对步、T4 给 agent 定义加写入纪律，属行为面文面化；取 low 会把「行为面变化」读成「纯措辞」。

## 5. 验证集

> **断言纪律**：所有预期值 = 起草期实跑输出回填（命令 + 关键输出见基线与留痕），不推算；落地期实跑若与本表有字节差异，以落地实跑为准覆写本表并在 §7 留痕。
> **装配**：17 处编辑由共享锚文装配器装配（`node .claude/scripts/assemble-edits.mjs delivery/review/261027-gov-review-p75-closeout-shoot-cluster/_scratch/edits.json C:/tmp/p75-assembly4`，仓外副本；`p75-assembly` / `p75-assembly2` / `p75-assembly3` 是修订过程中的中间副本）；清单 `oldStr` = 本 §2 各段原文块、`newStr` = 改后块。
> **时点**：pre-commit = 落地文件在盘、未 commit 即可跑；post-commit = 须落地提交后才成立。

**基线（起草期 2026-10-27 实跑）**：

| # | 命令 | 实跑输出（逐字 / 摘要） |
|---|---|---|
| B1 | `node .claude/scripts/assemble-edits.mjs delivery/review/261027-gov-review-p75-closeout-shoot-cluster/_scratch/edits.json C:/tmp/p75-assembly`（首跑，17 条） | `ok:true`、8 件；17/17 旧串各恰 1 次命中（0 次 / 多次即中止，本跑无中止） |
| B2 | 同命令、输出根换 `C:/tmp/p75-assembly4`（R1 修订后复跑） | `ok:true`、8 件；sha256（前 → 后，前 16 位）：DEV-GOVERNANCE `3ea84feee6c9a996…→cc2960c6bade0dce…`（332 行）· planner `c97c134fb6328488…→372d17f780022ef7…`（225）· SKILL `a571111082a297a4…→6b54eec333a99052…`（46）· requirement-change `df1d9ec87bccf48a…→50f8dd67bd8bb3e6…`（80）· command `9d9adacc170e1040…→8edfb351af5a260c…`（25）· uiux-author `1fe183fcb0fba3ad…→2bc3d2033ebc43a6…`（38）· ARQ-261019 `5fa8d5ec46ce941d…→048c306239810c03…`（36）· ARQ-261014 `e66312e6574862a9…→522e9b1ba946f834…`（27） |
| B3 | `node delivery/review/261027-gov-review-p75-closeout-shoot-cluster/_scratch/verify.mjs`（逐条 `split(oldStr)` / `split(newStr)` 计数） | 17/17：`oldRepo=1 newCopy=1 newRepo=0`，`bad: 0` |
| B4 | `git diff --no-index --numstat`（仓内 ↔ 副本，逐件） | DEV-GOVERNANCE **18+/2−** · planner **1+/0−** · SKILL **10+/0−** · requirement-change **2+/2−** · command **1+/1−** · uiux-author **5+/4−** · ARQ-261019 **5+/1−** · ARQ-261014 **2+/0−** |
| B5 | 副本读回计数（Node：`\r\n` 计数 + 裸 LF 计数 + U+FFFD + 散文禁用词计数） | DEV-GOVERNANCE LF 332 行（CRLF 0）· planner LF 225 · SKILL LF 46 · requirement-change LF 80 · command LF 25 · uiux-author LF 38 · ARQ-261019 **CRLF 35 / 裸 LF 0** · ARQ-261014 **CRLF 20 / 裸 LF 6**（混合态为原文件既有形态，新插段用 LF、与文末邻行同源）。八件 U+FFFD = 0；新插文本散文禁用词计数 = 0（17 条 newStr 全扫） |
| B6 | 结构抽查（副本 `sed` 读段） | 映射表落在 §6「三类制品」表之后、`## 7. 入口速查` 之前，表头三列齐、7 行数据；planner 8c 插在 8b 与 `9. 派 builder` 之间；SKILL「出图管道」节在「生效（需 human）」与「约束」之间；uiux Step 2 追加块以三空格缩进挂在原列表项下 |
| B7 | 新字面 census（`grep -rlF`，排除本批目录 / `.git` / `node_modules`） | `收口义务 → 承载写点映射` **0 文件**；`出图管道` **3 文件**（P5 PROPOSAL / ARQ-261019 条目 / `docs/debt/gov/INDEX.md`，均立项材料或历史证据）；`shoot_design` **11 文件**（3 母版 HTML / 声明面 / requirements 变更记录 / intake / workstream findings / P5 PROPOSAL / ARQ 条目 / INDEX / 脚本本体） |
| B8 | 退役语面 census（同口径） | `od_* 归主线程` **7 文件**（现行载体 3 = `DEV-GOVERNANCE.md` / `uiux-author.md` ×2 / `references/requirement-change.md`；归档 / 历史 4）；`按规格单跑 OpenDesign MCP（od_*）` **3 文件**（现行 2 + 260929 归档 1）；`od_* 失联挂 human-decision` **1 文件**（命令面）；`出图 pending` **6 文件**（现行 2 + 历史 4） |
| B9 | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261027-gov-review-p75-closeout-shoot-cluster/PROPOSAL.md` | `ok:true`、`section2BodyFound:true`、`anchorCount:8`；`anchorUnion` 八件与 manifest 逐 token 一致；`findings:[]`、`advisories:[]`、exit 0 |
| B10 | `node .claude/scripts/landing-fidelity-check.mjs --cite-check …/PROPOSAL.md --phase draft` | `ok:true`、`phase:draft`、`frozen:false`、`targetCount:8`、**checkedCount 17 / missing 0 / notChecked 17（十七块改后文）/ unverified 0**；16 条 `matched` 指向对应目标文件，1 条**归属歧义**（ARQ-261019 frontmatter 翻转块，oldStr = `status: deferred` 在两件 ARQ 条目各恰 1 命中，checker 将其归到 `ARQ-261014-…md`）——歧义非未命中，串唯一性由装配器「恰 1 命中」断言（本批 17/17 ok）与 V3 哨行（ARQ-261014 `status: deferred` 1→1）兜住 |
| B11 | 落地相位沙盘：八件副本 + 本稿复本落 `C:/tmp/p75-landsim`（同仓根相对路径），`CLAUDE_PROJECT_DIR=C:/tmp/p75-landsim node .claude/scripts/landing-fidelity-check.mjs --cite-check …/PROPOSAL.md --phase landing` | `ok:true`、`phase:landing`、`targetCount:8`、**checkedCount 17 / missing 0 / notChecked 17（十七块原文）/ unverified 0**；17 条 `matched` 逐条指向对应目标文件——V2 的终态预演 |
| B12 | 字面正推（仓内 ↔ 沙盘，Node `split` 计数） | `DEV-GOVERNANCE`：`出图 od_* 归主线程` **1→0**、`收口义务 → 承载写点映射` **0→2**；`## 7. 入口速查` **1→1**；planner：`收口写点核对` **0→1**、Step 9 行首 `9. 派` **1→1**、`8b. **锚 2` **1→1**；SKILL：`## 出图管道` **0→1**、`shoot_design` **0→1**、`## 约束（must / never）` **1→1**；reference：`按规格单跑 OpenDesign MCP（od_*）` **1→0**、`不执行出图（od_* 归主线程）` **1→0**、`**出图执行 = 主线程代执行**` **1→1**；command：`按规格单跑 OpenDesign MCP（od_*）` **1→0**、`od_* 失联挂 human-decision` **1→0**、`4.5. **UI 生成` **1→1**；uiux：`不执行出图（od_* 归主线程）` **2→0**、`母版 / 增量页单写者约定` **0→1**、`机检在现行稿相对前序版本塌缩过半` **1→1**；ARQ-261019：`status: deferred` **1→0**、`status: closed` **0→1**、`结项追注（2026-10-27` **0→1**；ARQ-261014：`status: deferred` **1→1**（哨：条目维持 deferred）、`追注（2026-10-27，261027-gov-review-p75` **0→1** |

**落地验证集**：

| # | 时点 | 命令 | 预期（实跑回填 / 正推） | 判别力 / 配对 |
|---|---|---|---|---|
| V1 | pre-commit | ① 落地前复跑装配到全新输出根：`node .claude/scripts/assemble-edits.mjs delivery/review/261027-gov-review-p75-closeout-shoot-cluster/_scratch/edits.json C:/tmp/p75-prelaunch`；② 落地后对仓内八件跑 sha256（Node `createHash('sha256').update(content,'utf8')`） | ① `ok:true`、8 件、逐件 `afterSha256` 与 B2 逐值相同；② 八件仓内 sha256 与 B2 `afterSha256` 逐值相同（cc2960c6… / 372d17f7… / 6b54eec3… / 50f8dd67… / 8edfb351… / 2bc3d203… / 048c3062… / 522e9b1b…） | 反向：任一锚漂移即 exit 1（0 命中）或 count≠1（多命中）且零写盘；正向：八件集合 = manifest 集合 |
| V2 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --cite-check delivery/review/261027-gov-review-p75-closeout-shoot-cluster/PROPOSAL.md`（FROZEN 后自动 phase=landing） | `ok:true`、`phase:landing`、`targetCount:8`、**checkedCount 17 / missing [] / notChecked 17（原文块）/ unverified []**；起草期沙盘预演 = B11（逐值相同） | 负向（missing 为空）× 正向（checked 非空且逐条 `matched != null`） |
| V3 | pre-commit | 字面正推（`grep -cF` / Node `split` 逐项，仓内落地文件） | 逐项 = B12 的 `after` 列（1→0 / 0→1 / 哨 1→1 全对） | 每条退役字面配「旧=0」、每条新字面配「新=1」；哨行（`## 7. 入口速查` / Step 9 行首 / `8b. **锚 2` / `## 约束（must / never）` / `机检在现行稿…` / `4.5. **UI 生成` / ARQ-261014 `status: deferred`）各恒 1，防误删邻行 |
| V4 | pre-commit | `git diff --name-only -- docs/contracts/ docs/requirements.md docs/architecture.md docs/ui/ firmware/ server/ tools/ .claude/hooks/ .claude/scripts/ .claude/scope-map.json .claude/*.settings.json .claude/verify-gates.json AGENTS.md CLAUDE.md` | 空输出 | 正向配对：落地未 stage 时 `git diff --name-only` 恰 = manifest 八件；已 stage 则改跑 `git diff --cached --name-only` 对账同一集合（PROPOSAL 与 `_scratch/` 按 staging 时序另计，`_scratch/` 必须不在集合内）；并发改动逐件核性质并记入 landing-summary |
| V5 | pre-commit | `git status --porcelain` 中的 `_scratch/` | 无（`delivery/review/261027-gov-review-p75-closeout-shoot-cluster/_scratch/` 在落地 commit 前删除；起草期产物副本全在仓外 `C:/tmp`） | 负向（仓内无残留）× 正向（V4 的「diff 恰等于 manifest」） |
| V6 | pre-commit / post-commit | `node .claude/scripts/debt-render.mjs` | `docs/debt/gov/INDEX.md` 中 ARQ-261019 行消失、条目移入 `docs/debt/gov/_archive/2026/`（status=closed 的纯函数归位）；ARQ-261014 行仍在未结段、措辞不变 | 生成件，禁手改；归位后 landing-verifier 主门的 `verbatimQuoteAdvisory` 可能对旧路径 miss（P5 先例：`landing-verdict` 已解释同形），人工核归档位逐字节一致 |
| V7 | post-commit | `node .claude/scripts/changes-index.mjs --upsert 261027-gov-review-p75-closeout-shoot-cluster` | `delivery/review/INDEX.md` 出现该 id 行；须在落地 commit **之前**跑（主线程 chore） | 生成件，禁手改 |
| V8 | post-commit | `node .claude/scripts/pre-merge-escalation-gate.mjs 261027-gov-review-p75-closeout-shoot-cluster --source-ref HEAD` | 预期 `PASS (…, kind=gov)`、exit 0 | **未实测**（依赖 FROZEN + landing-verdict 在场）；本批无契约 diff |
| V9 | post-commit | `node .claude/scripts/route-regression-check.mjs` · `node .claude/scripts/lint-descriptions.mjs` | route：`52 ok, 0 fail`（起草期基线实测同值；本批 description 改动不触任何必留路由信号）；lint：`flagged 0` / `✓ clean` | 未实测说明：两项读仓内 canonical，须落地后跑；基线值已回填 |

**起草期自检留痕（模板五句的落地记录，2026-10-27）**：

1. **preflight 实跑**：见 B9（`ok:true`、anchorCount 8、findings / advisories 空、exit 0）；通过后才给判官。`--cite-check --phase draft` 见 B10，落地相位沙盘见 B11。
2. **路径形态族谱**：本批零搬家、零重命名、零存量路径改落；新增字面（`收口义务 → 承载写点映射` / `## 出图管道` / `收口写点核对` / `母版 / 增量页单写者约定`）已按 §3 census 预跑（B7），`path-form-family.md` 不适用。
3. **承重引文 byte 对账**：§2 十七组「原文 / 改后」块与 `_scratch/edits.json` 逐条机械对账（对账脚本按文档序配对），17/17 配对、未配对 0、双侧行尾归一后逐字节相等——非手抄转录；无命中长输出目视抄写。
4. **长文落地自检**：本稿分块写入（五段：头部与 §0–§2 前置、T1–T2、T3–T4、T5–T6 与 §3、§4–§7），R1 后逐点修订（v0.2：F1/F2/F3 文本级修正 + NIT-1/NIT-2 搭车），每次写后机检——终稿 **566 行**（含 §7 R1 评审节）、LF（CRLF 0）、U+FFFD 0、围栏定界行 **70**（偶数）、`### T<n>` 标题 6、`目标文件：` 锚行 6、manifest 块 8 行。
5. **脚本批拆分自查**：本批 `.claude/scripts/**` 零 diff（V4），拆分纪律不适用；`_scratch/verify.mjs` 是起草期一次性核对件、非仓内工具，落地前删除。

## 6. 留根清单

1. **Pod per-session cast 的 uiux-author 可用性**（`ARQ-261014` 2610-25 子项①）：本批只把「Pod cast 缺省不含 uiux-author → 母版 / 出图归 Pod 外主线程」钉进映射表；「让 Pod 会话能派出 uiux-author」属 harness 会话级配型（`--agent planner` 起的会话可派集），不是写权映射能解的。复议触发 = 再出现「Pod 内需要需求面式 UI 同步」的实例，或 harness 支持按 Pod 配 cast。
2. **账本登记类机械通道**（`ARQ-261014` 修法候选③）：§4 OQ-1 若裁 (a)，本条留根，复议触发 = 映射表落地后账本登记仍高频回流主线程。
3. **`opendesign-connect` description 的死指针**：其边界句指向从未建成的 `.claude/references/uiux-opendesign-loop.md`（260929 批立项时留根不建）。本批不改 description（路由信号面）；建议修法 = 边界句改指 `references/requirement-change.md` 4b 或本 skill「出图管道」节。复议触发 = 该 reference 立项，或指针清扫批。
4. **Step 8c 无机械闸**：写点核对执行方是 planner 本人，靠文面纪律与 `notes` 留痕；复议触发 = 出现「漏核、closeout 又撞 enforce-scope」的新实例（届时再议机检，不预设）。
5. **生成件与归档位移**：`docs/debt/gov/INDEX.md` 由 `debt-render.mjs` 再生；`ARQ-261019` 结项后自动归位 `docs/debt/gov/_archive/2026/`（位置 = status 的纯函数）；`delivery/review/INDEX.md` 由 `changes-index.mjs --upsert` 再生。三者都不进 manifest、禁手改。落地核验时若 `verbatimQuoteAdvisory` 对旧路径 miss，按 P5 先例核归档位逐字节一致。
6. **`_scratch/` 清理**：`delivery/review/261027-gov-review-p75-closeout-shoot-cluster/_scratch/`（`edits.json` + `verify.mjs` + `reconcile.mjs`）是起草期装配清单与核对件，**落地 commit 前删除**；装配副本在仓外 `C:/tmp/p75-assembly4`（中间副本 `p75-assembly` / `p75-assembly2` / `p75-assembly3`），不入仓。
7. **P5 留根的交接**：`261027-gov-review-p5-spike-anchor-cluster` §6 留根 5（「母版写者分工 / 出图管道属 ARQ-261019」）由本批消化；P5 §6 留根 4（活图夹机制 / 存量 V5–V7 迁移）仍是需求面另立的动作，本批不碰。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

---

### R1 · 2026-10-27 · architect-reviewer（四查 + 判档）

**评审方式**：只读 canonical 亲核 + 机检复跑（preflight / cite-check / assemble-edits 到全新输出根 / 全仓 census 复扫），不采信转述。

**① 意图保真**：改法集与两条件目建议 + P7.5 立项 ack（`task_plan.md:112-116`：两条并批、恒 Track B、④ 取①+②、⑤ 取三改进项）一致；候选③ 留 OQ-1、子项①留根，与 ack 同构。**OQ-1 human 已拍 (a)（2026-10-27，task_plan.md:117）**：改法集与 (a) 一致——零 scope-map 触碰、候选③ 留根 §6-2、无 (b)/(c) 隐含语义；拍板结果在此正式记入 §7，FROZEN 时 OQ-1 视为已决。T6 结项追注对改进项③ 的表述错位见 F1。

**② 纪律合规**：Track B 全件齐备（manifest 围栏块 + packet-id 行在场，preflight ok:true / anchorCount 8 = manifest 8 件一致）；G3 面 / 契约 / spec / docs/ui / 实现面零 diff 断言与 manifest 及 V4 命令互证成立；agents 定义变更逐处未超条目语义（T4 五处、T2 一处均最小改，route-regression 必留信号不触——uiux-author 不在 `route-regression-check.mjs` CFG 信号表，脚本 grep 零命中）。B10 摘要句与实跑不符见 F3。

**③ 撞车复核**：新 §6 映射表与 §7「我想改…」入口表的分工由 T1 边界句 + §7 分工句双面钉死，ADR / delivery 记账已显式排除不并入；T2 Step 8c 与「派发书三条硬约定」② 的分界（单次派发写权 vs 本包收口义务写点）成立，且显式不进 3n nano 豁免清单（nano 一行 `none` 闭合）；T3 / T4 与 P5 落地的 `docs/ui/UI-DESIGN-VERSIONS.md` 声明面（`current:` / 单一活图夹 / 冻结快照 / V<N> 过渡惯例）同语不写第二套，P5 §6 留根 5 交接关系成立；T4 未改写 P5 刚落的 Step 2 / Step 3 版本语义。无与既有节冲突项。

**④ 牵连面完整**：装配复现通过——assemble-edits 复跑到全新输出根 `ok:true`、8 件、8 个 afterSha256 与 B2 逐值相同（V1① 预演通过）；复证 C（scope-map 写权矩阵）亲读证实（planner deny `docs/**`、integrator/adjudicator allow `docs/debt/**`、uiux-author allow 仅 `docs/ui/**`、无 agent allow 覆盖 `delivery/intake/**` 与 `docs/notes/**`——映射表各行写点与现状一致、零换授）；四条退役语面 + 三条新字面 census 判官全仓复跑与 B7/B8 逐项相同（`od_* 归主线程` 7 文件 / `按规格单跑 OpenDesign MCP（od_*）` 3 / `od_* 失联挂 human-decision` 1 / `出图 pending` 6 / `收口义务 → 承载写点映射` 0 / `出图管道` 3 / `shoot_design` 11）；消费者普查穷尽性复扫——现行面残留仅 SKILL.md（排障语境，本批有意保留）与 `story-author.md:24-25`（「需出图」为触 UI 类目定义、通道中立，不载退役语义，T5 未列属正确豁免）；V6=1 PNG / V7=3 PNG、`ARQ-260929-docs-sync-spec-handoff` 已结（closed 字段形态与 T6 一致）亲核属实。F2 为本轮新抓牵连缺口。

**判档结论**：**Track B 主路径成立**（核其依据，不改判）——gov-change.md:19 判据③排除集命中 `docs/DEV-GOVERNANCE.md`、`.claude/agents/**`、`.claude/commands/**` 三处（亲核原文），判据①路径面亦不满足，判据②（新增映射表一节 / finalize 核对步 / skill 新节 / agent 定义五处改写）非措辞级——恒 Track B 成立；Track A 段判据逐条不命中：方向由两条 ARQ 建议改法 + 立项 ack 收敛、OQ-1 已拍、评审未发现方向级缺口、无声明红队。riskTier low-medium 自评合理（T2 / T4 属行为面文面化，不取 low 的证成成立）。

**Findings**：

- **F1（SHOULD · 意图保真）**：T6 结项追注（PROPOSAL:404）对 ARQ-261019「改进项③」表述错位——条目正文改进项③ = **真因记录**（ARQ-261019-...md:26「3. **真因记录**：OD daemon 端口动态 + 应用拉起失败是常态场景……」），追注却写「③ 同款指引随 ② 落定义正文（Role / Step 3 / Boundaries 三处同步）」；「同款指引」是改进项② 的原文措辞（条目 :25），③ 在追注中无对应承载说明，且单写者约定的归属（条目 trigger ③，非正文改进项编号）未写清。追注是条目永久处置记录，错位映射会误导下轮 retro 对账。改法：③ 句改为真因记录的承载说明（修正后的 skill「出图管道」节 + 定义三处通道句已清除「出图依赖 OD」误导口径），单写者约定归属注 trigger ③。
- **F2（SHOULD · 牵连面 / authoring）**：T4 处三单写者约定指示「做最小 Edit」「改走增量 Edit」（PROPOSAL:260），但 uiux-author frontmatter `tools: Read, Write, Grep, Glob`（uiux-author.md:4）**无 Edit 工具**——该指令对本 agent 不可执行，可执行路径只剩「交回主线程合并」分支；违反 prompt-authoring「每句须有实际作用」。改法：改写为 Write 语义判据（「全文件 Write 仅当盘上现行文件内容即你的产出基线；盘上含非你产出内容 → 停，把增量页 / 补丁说明交回主线程合并」），或显式注「Edit 不在本 agent 工具面」。
- **F3（SHOULD · 验证集如实回填）**：§5 B10 摘要句「17 条 `matched` 各指向对应目标文件」与实跑不符——判官复跑 `--cite-check --phase draft`：第 381 行块（ARQ-261019 frontmatter 翻转，oldStr = `status: deferred`）的 matched 归到 `ARQ-261014-...md`（该串在两件 ARQ 条目各恰 1 命中，checker 归属歧义）。串唯一性由装配器闸（0/多命中即中止，判官复跑 ok:true）与 V3 哨行（ARQ-261014 `status: deferred` 1→1）兜住，不构成落地风险；但验证集预期值须如实回填（arch-change.md「Track B 制品义务」②），摘要句改为如实描述（16/17 指向对应目标 + 1 条归属歧义说明）。
- **NIT-1**：T3「输入与落点」句前半声明「本 skill 不复述」、后半仍复述「输出落单一活图夹——机制落地前维持 docs/ui/V<N>/ 惯例」；活图夹机制批（P5 §6 留根 4）落地后此句需同步，可收窄为纯指针或在该机制批消费者清单记一笔。
- **NIT-2**：§4 OQ-1 仍以「推荐 (a)」在途形态呈现；建议本轮修订在 OQ-1 题头补一行「human 2026-10-27 已拍 (a)」，使 FROZEN 制品自含拍板记录（事实已记本 §7 与 task_plan.md:117）。

三条 SHOULD 均为文本级修正、不改 T1–T6 结构与落点，修订后 R2 聚焦复验 F1–F3 到位性即可。

verdict: BLOCK(F1,F2,F3)

---

### R2 · 2026-10-27 · architect-reviewer（聚焦复验：R1 findings 到位性，不重开四查）

**复验方式**：逐字人读五处修订 + 机检复跑（装配到全新输出根 / cite-check draft / 全新沙盘 landing 相位）；R1 GREEN 面经 sha 比对确认未受 v0.2 影响（manifest、落点、T1/T2/T5 改文零变化——五件未涉改文件 afterSha256 与 R1 复现值逐值相同）。

**逐条到位性**：

- **F1 ✓ 到位**：T6 结项追注重写后与条目编号一一对应——① = skill「出图管道」节、② = 定义四处通道句（description / Role / Step 3 / Boundaries）、③ = 真因记录随 ①② 成文消解（与条目 :26 改进项③ 原义相符）、单写者约定单列为「条目 **trigger ③**（正文改进项编号之外的 261019 实测追加项）」——F1 指出的错位映射与归属不清两项均消除。
- **F2 ✓ 到位**：T4 处三「做最小 Edit」「改走增量 Edit」全清；新文为 Write 语义判据——「你的修订只能『盘上现行母版基座 + 你的增量』合成后整份 Write（**本 agent 工具面无 Edit**）——Write 前先读盘上现行文件，确认其内容是你的产出基线；盘上母版含非你产出的内容 → 停，把增量页 / 补丁说明交回主线程合并」——对 `tools: Read, Write, Grep, Glob` 完全可执行，且显式声明工具面限制；判据落点（何时可整份 Write / 何时停 / 停后去向）三件齐。
- **F3 ✓ 到位**：B10 摘要句改为「16 条 matched 指向对应目标文件，1 条归属歧义（ARQ-261019 frontmatter 翻转块，oldStr = `status: deferred` 在两件 ARQ 条目各恰 1 命中，checker 将其归到 `ARQ-261014-…md`）——歧义非未命中」——与判官复跑输出逐字相符（判官复跑：ok:true、17 checked / 0 missing / 17 notChecked；line 381 块 matched = ARQ-261014，为唯一歧义条）。
- **NIT-1 ✓ 到位**：T3「输入与落点」bullet 收窄为纯指针——「母版与落点一律以声明面 `docs/ui/UI-DESIGN-VERSIONS.md` 为准（`current:` 指针 +「活图夹与冻结快照」节）；…本 skill 不复述、不另立」——「单一活图夹 + V<N> 惯例」复述已删，无双口径残留。
- **NIT-2 ✓ 到位**：§4 OQ-1 题头补「**已裁：human 2026-10-27 拍 (a)**（记录 = `task_plan.md:117` 与 §7 R1 节）；…FROZEN 时 OQ-1 视为已决——改法集与 (a) 一致」，推荐句改「推荐 (a)（已拍）」并留 (b) 复议路径——FROZEN 制品自含拍板记录。

**机检复跑**：① 装配复跑到全新输出根 `C:/tmp/p75-review-r2` = ok:true、8 件，8 个 afterSha256 与 v0.2 B2 逐值相同（SKILL `6b54eec3…` / uiux-author `2bc3d203…` / ARQ-261019 `048c3062…` 三件为新值；DEV-GOVERNANCE / planner / requirement-change / command / ARQ-261014 五件与 R1 值相同），afterLines（332/225/46/80/25/38/36/27）同 B2；② cite-check `--phase draft` 复跑 = ok:true、17/17 原文块命中、missing 0，歧义条归属与 B10 新描述一致；③ 落地相位沙盘（判官自建 `C:/tmp/p75-landsim-r2`，八件副本 + 本稿复本）= ok:true、phase:landing、17 checked / 0 missing / 17 notChecked / unmatched 0——B11 复现；④ 修订处装配产物逐字人读（SKILL 节 / uiux Step 2 追加块含三空格缩进 / ARQ-261019 追注）与 §2 围栏块一致。

**判档**：维持 R1 结论 Track B 主路径（只升不降已核；v0.2 未引入新语义成分——五处修订均为 R1 findings 指向的文本级修正）。

verdict: GREEN
