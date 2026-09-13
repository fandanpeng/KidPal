# Landing Verdict · 260912-gov-review-portfolio-decomposition-chain

**verdict：GREEN（landing 保真度成立）——merge 放行前置 4 项证据制品格式修正（见偏差清单 D1–D4，均不触 canonical landing 内容，主线程十分钟内可补齐）**

- 核验对象：landing commit `c9a43c7`（相对 FROZEN 基线 `12d23a2`），FROZEN v1.0 PROPOSAL + landing-summary.md。
- 核验方式：`landing-fidelity-check.mjs`（baseRef=12d23a2）+ 逐文件人读 diff（9 个 canonical 文件全核，无抽查跳过）+ 验证集实跑复验。
- 独立性：本核验只读 codebase，只写本 verdict；未改任何 canonical / 证据制品。

---

## 一、结构门（机械）

### 1.1 原始制品跑门 = RED（fail-loud，4 条证据格式项）

```
node .claude/scripts/landing-fidelity-check.mjs <证据目录> 12d23a2  → exit 1
```

| # | 报错 | 根因 | 层级 |
|---|---|---|---|
| R1 | `architect-landing-summary.md is required` | 证据 summary 落盘名为 `landing-summary.md`，脚本硬要求 `architect-landing-summary.md` 文件名 | 证据制品命名 |
| R2 | `缺 landing-manifest 围栏块（architect-landing-summary.md）` | landing-summary 自述引用了「PROPOSAL ```landing-manifest``` 9 文件」但自身无围栏块（B2 格式契约要求两侧各含恰一块） | 证据制品格式 |
| R3 | `缺 packet-binding（须引 packet-id）` | summary 只有「packet-bound 落地」字样，无字面 `packet-id`；被批 review id（260912-gov-review-portfolio-decomposition-chain）与 FROZEN commit 12d23a2 实际都在场——binding 实质成立，仅字面不达标 | 证据制品措辞 |
| R4 | `§2 锚清单：缺锚清单 T1–T4（×4）` + `§2 ≢ manifest：仅 §2 有 .claude/skills/triage-change/SKILL.md` | PROPOSAL 锚行写 `**目标文件**：`（加粗），解析器只认行首裸 `目标文件：`（历史过门 PROPOSAL 均裸写）；另 T1 锚行注记里逐字引用了被撤幻影路径 `` `.claude/skills/triage-change/SKILL.md` ``（「原列 X 不存在」说明文字），解析器把注记内路径形态 backtick token 也当锚消费 | PROPOSAL 锚行格式 |

### 1.2 格式归一后跑门 = GREEN（实质检查全过）

为把机械门的实质判据真实跑完（不改证据目录），在 tmp 复刻件上做四项**纯格式**变换（summary 复制为脚本要求文件名 / 补等价 manifest 块〔逐字取自 summary 表格 9 行，与 PROPOSAL manifest 块一致〕/ 补一行 `packet-id：` / 锚行去粗 + 注记去幻影路径——路径集零改动），重跑：

```json
{ "ok": true, "frozen": true, "packetBinding": true,
  "proposalTargets == summaryTargets": 9 文件一致,
  "unauthorized": [], "claimedNotLanded": [], "plannedNotLanded": [],
  "gitDegraded": false,
  "advisory.changedCanonicalNotInProposal": [] }
```

结论：**结构保真实质全过**——frozen、packet-binding 实质成立、summary ⊆ PROPOSAL（零 scope creep）、claimed/planned 双向零缺、git 改动集内无 manifest 外 canonical 文件。R1–R4 全为证据簿记格式问题，不是 landing 内容问题；但 fail-loud 门对原始制品是红的，故列为 merge 前置修正项（D1–D4）。

设计侧门：Track B 证据集无 decision-packet，`proposal-consolidation-check.mjs` 不跑——N/A（评审记录 §7 R3 GREEN + Status 行 FROZEN v1.0 承载前提，已核在场）。

---

## 二、逐文件人读核对表（9/9 全核）

| # | 文件 | 动作 | 核对结论 | 证据（PROPOSAL 依据 → diff 实况） |
|---|---|---|---|---|
| 1 | `docs/templates/WORKSTREAM-PROPOSAL.template.md` | 新增 93 行 | **match** | T1：三问骨架（§1 要达成什么 / §2 现在在哪 / §3 怎么走）；frontmatter `workType: [<…>, ...]` 字符串数组形状钉死；三子形节名固定字面「问题与根因」「现状与差距」「结构性痛点」逐一在场且带「仅此形态合法」钳制；§1 枚举式硬项 + 起草侧两查自检行（已砍清单节名定位 m-5 / 🔴 须立项处置）；§3.1 三 token（`within-spec`/`需修订`/`spec-silent`）+ `需修订` 行含 launchPreconditions 执行语义（粒度 = 包、spec 修订未冻结不得 launch、convention + 人读指 pod-coordination §2b）；头注权威序声明（spec = 行为语义权威、本件 = 路径选择记录、不自冻成第二事实源）；起草方 = 主线程注记。无 PROPOSAL 之外私货 |
| 2 | `docs/templates/WORKSTREAM-ROADMAP.template.md` | 新增 44 行 | **match** | T1：四固定节——① 效果目标投影（「投影自 PROPOSAL §1、不复制正文——只引条目号 + 一句判据摘要 + 指针」）；② 列车图 `render-roadmap:begin/end` 标记区块 + 「人工禁改」+ 「渲染源唯一 = board + taskpackage JSON」双声明；③ 每包条目含效果段（人只写机器写不出的一段）+ 回指 `S-Fx.y · <标题>`（AC 号集，新建型必填、只投影不改写）+ demo 证据形态四式 + 真机轮配额默认仅核心包 + 达成状态机械渲染禁改；④ 非目标与批次归属；头注职责划界（task_plan.md / progress.md / 本件只投影不裁决，M-3）。逐字与 T1/R2-n3 对齐 |
| 3 | `.claude/scripts/render-roadmap.mjs` | 新增 447 行 | **match** | T1/T4 + n-15 + m-11 + R3 系列：圈定 = `tp?.origin?.workstream === slug`（:105-106）；board 默认 `git rev-parse --git-common-dir` 下 `devloop/board.json`（:122-127）；deviation = 既有 `human-decision` 型 + `payload.decision === "anchor3-deviation-ruling"` 区分值（:40/:151-155），未触 `pod-events-append.mjs` 闭集（不在 diff）；跨 workstream dependsOn → 带标注外部节点 + 虚线边（:190-194）；全量重算幂等（self-check 断言 12）；`--write-into` 标记区块替换 + 标记外正文保全（:248-264，断言 13–15）；头部明示 advisory 非门。**实跑**：`--self-check` 15 断言全绿 exit 0；`--workstream 260911-display-interaction-sovereignty --json` 圈定恰 3 包（key-route-a1 / screen-unification / display-first-m1，board 均 merged → achieved），ecs-deploy-convergence 只出现在 dependsOn、不入包集——§5-2 双断言成立 |
| 4 | `.claude/agents/portfolio-planner.md` | 改正文 9 行区 | **match** | T1/T2/T3：Step 1 增模板符合性核验（硬项三项缺 → AskUserQuestion 停、打回主线程，fail-closed 不猜不代写）+ 两道 story 引用机查（已砍清单 / 🔴 处置，同一打回出口）；Step 4 改四件套（taskpackage.json + 每包 plan.md 全量义务〔豁免全取消、骨架五段、锚点随 workType 分流〕+ roadmap 产方〔模板引用 + render 命令〕+ decision-coverage.json 含 launchPreconditions）；新 Step 5b 门 1 停等（AskUserQuestion 请 human 发起 → GREEN + human 过目切法方进 Step 6；裁决耐久记 review.md；存疑向严升 Track A）；Step 6 commit 集补 roadmap.md + plan.md；References 增两模板 + 渲染脚本 + task-intake 权威标注。**frontmatter 零改动**：`git diff -U0` 全部 hunk 在 :35 之后，frontmatter（:1-6）未触 |
| 5 | `.claude/agents/planner.md` | 改正文 23 行区 | **match** | T4 + T2/m-8：Step 7 增锚 2 输入（workstream 包 dispatch prompt 携带 `.planning/<ws>/roadmap.md#<changeId>` 路径 + 节锚引用，不整段转抄——R3-n2 择一钉死形态）；新 Step 8b 锚 2 验收方向核（AC 回引对账按 workType 三分流；核对者 = planner 非 human 及角色不冲突理由；对不上 → architect 复核 10b 档 → 既有升级链）；Step 11 增 demo 对账行派发传递义务（M-2 生产者可达性，integrator.md 不改）；TaskPackage 格式例整体换新——`S1-A1` 四处清零（changeId / upstreamRef / sourceRefs / acceptance），`docs/systems` 清零，sourceRefs 示范 `S-F1.4` + `S-F1.4-AC3`，acceptance 保持本地 id `SR-A1/SR-A2` + exitCriteria 同步 `SR-Ax`（R2-n2 换新边界），story 字段 = 锚点引用口径 + 文末口径注记块。**frontmatter 零改动**：全部 hunk 在 :55 之后（:1-6 未触） |
| 6 | `.claude/references/task-intake-and-slicing.md` | 改 41 行区 | **match** | T2/T3 + M-1②：新章「拆包链：workstream 双轨制品、门 1、story 转写」——双轨注记 + 拆包权威四件套经门 1；plan.md 全量义务节（豁免全取消 + 骨架锚点三处节名逐字一致）；story 锚点引用与转写节（AC 复合句法 `S-Fx.y-AC<n>` 全仓首钉 + write-story 分工注记两句并存 + 换新边界 acceptance 本地 id）；门 1 checklist 五项 + 派发形态（frontmatter 无 `Agent()` 也不加、停等）+ workType 分流 + 裁决耐久记录 + 存疑向严；launchPreconditions canonical 定义（顶层键 + 条目形态 + 修订落点二分 + convention + 人读 + 不改 check-launch 判定语义）；sourceRefs 例句换新（`S-F1.4` / `S-F1.4-AC3` / `docs/requirements.md:S-F1.4`）；planRef 节补③拆包路径全量义务注记。一处极小观察见 D5 |
| 7 | `.claude/references/pod-coordination.md` | 改 13 行区 | **match** | T3/T4 + m-8：§2b 增 launchPreconditions convention 条款（`--check-launch` 机械 check 集不读该键、launch 执行者人读核、粒度 = 包、本门判定语义不变、机械齿 ARQ 留根）+ `:Sx-Ay` 例句换新为 `:S-Fx.y` / `:S-Fx.y-ACn`；新 §11 锚 3——对账行义务（review-packet / supervisor-verdict 消费面）+ planner dispatch 派发侧传递 + deviation 事件形态（既有 `human-decision` + 区分值 + `--actor human` + 不扩闭集 + 沿 `approve-escalated-merge` 先例）+ 绿路径 portfolio-planner 沉默 + 刷新时机（merge 后 human 簿记顺跑 render，幂等）+ 再出场条件收窄（仅三条调度异常） |
| 8 | `.claude/references/change-triage.md` | 改 1 行 | **match** | B-1/T1：③档落盘行补「按 docs/templates/WORKSTREAM-PROPOSAL.template.md 三问 + 分型模板起草——三项硬项缺失即 portfolio-planner intake fail-closed 打回」 |
| 9 | `.claude/commands/triage-change.md` | 改 1 行 | **match** | B-1/T1：Step 4③ 同步补模板起草义务句（同一硬项三件 + 打回出口），幻影 skill 路径未出现 |

**越界检查**：`git diff 12d23a2..c9a43c7 --stat` 恰 10 件 = manifest 9 canonical + `changes/arch/…/landing-summary.md`（证据制品随 landing commit，属流程制品非 canonical）。manifest 外 canonical 命中 = 0（结构门 advisory `changedCanonicalNotInProposal: []` 与人工核对一致）。`.codebase-memory` 两件脏属机器再生面，未入 commit、不计。

**敏感面核对**：G3 强制层仅 `render-roadmap.mjs`（新增，头部自钉 advisory 非门、不触任何既有门判定语义）；两个 agent def 均正文改动、frontmatter 逐字零改动（U0 hunk 定位实证）；`pod-events-append.mjs` / `portfolio-reconcile.mjs` / hooks / scope-map 零触碰。无权限或写域扩张。

**packet-binding 核对**：landing-summary 引 FROZEN v1.0（commit 12d23a2）+ review id 全称与标题一致；实际落地范围 = manifest 9 文件 ⊆ 被批 packet。binding 实质成立（字面达标见 D3）。

---

## 三、破坏性自检

| 项 | 结果 |
|---|---|
| `git show c9a43c7 \| grep -c '<<<<<<<'` | **0**（无冲突标记残留） |
| `node .claude/scripts/route-regression-check.mjs` 复跑 | **28 ok, 0 fail** exit 0 |
| `node .claude/scripts/lint-descriptions.mjs` 复跑 | exit 0；warn 1 = `landing-verifier.md` over-length（既有、与本批无关——本批两 agent def diff 未触任何 description 行） |
| `render-roadmap.mjs --self-check` | **15 断言全绿** exit 0 |
| 260911 实跑 | 圈定 3 包 + ecs-deploy-convergence 外部前置，与 §5-2 断言一致 |
| key-route-a1 无 plan.md（§5-3 真阳性①） | 亲核 `ls .claude/taskpackages/260911-key-route-a1/`：8 项制品中无 plan.md——T2 全量义务能拦住该真实缺陷，claim 成立 |
| PKG-2 单包承载 4/5 收口标准（§5-3 真阳性②） | 亲核 `.planning/_archive/2026/260911-display-interaction-sovereignty/decision-coverage.json`：单包（display-first-m1）`landsIn` 承载 4 条 decision（coverage 共 14 条）——切片逻辑项命中面存在，claim 成立 |
| launchPreconditions 先例形态（M-1②） | 该文件顶层键在位、数组条目点名承载包与前置对象——canonical 定义与先例一致 |

---

## 四、残余偏差清单

| # | 偏差 | severity | 是否 PROPOSAL 允许 | 处置 |
|---|---|---|---|---|
| D1 | landing-summary 落盘名 `landing-summary.md` ≠ 结构门硬要求名 `architect-landing-summary.md` | minor（证据簿记，非 landing 内容） | 否（门格式契约） | **merge 前置**：主线程更名/补一份规范名副本 |
| D2 | landing-summary 自身缺 ```landing-manifest``` 围栏块（B2 要求两侧各含恰一块；本次门检的 scope/claimed/planned 机查因此只能经 verifier 归一后跑通） | minor-moderate（格式契约 fail-loud 项） | 否 | **merge 前置**：补 9 行围栏块（内容 = PROPOSAL manifest 逐字） |
| D3 | summary 缺字面 `packet-id`（只有「packet-bound」；binding 实质在场） | minor | 否 | **merge 前置**：补一行 `packet-id：260912-gov-review-portfolio-decomposition-chain` |
| D4 | PROPOSAL §2 锚行写 `**目标文件**：` 加粗形态，机械解析器不认；T1 锚行注记逐字引用被撤幻影路径致 §2↔manifest 对账假失配 | minor（PROPOSAL 锚行格式；路径集实质与 manifest 恰好 9=9 一致） | 否（authoring 格式） | 本批不阻断（FROZEN 稿不宜回改正文，格式修正不保真）；**未来 PROPOSAL 起草纪律**：锚行裸写 `目标文件：`、注记不引路径形态 token——建议登 ARQ 一条 |
| D5 | task-intake sourceRefs 例句行把 `K7-P0` 一并换掉（T2 正文仅点名 `S1-A1` 系两例） | trivial（同一行例句换新、in-manifest 文件、无语义规则变化；`S-F1.4` 系新形态与 PROPOSAL 指定一致） | 是（「例句换新」行级授权内） | 记录即可，无需动作 |

PROPOSAL §6 授权内的留根（M-1 机械齿不做、`Sx-Ay` manifest 外命中面批量对齐另登 ARQ）均如实记录于 landing-summary「诚实记录」节，非偏差。

---

## 五、结论

实际落的 diff == FROZEN v1.0 PROPOSAL 的 landing 清单与改法：9/9 canonical 文件语义逐项对上、零越界、零漏落、零夹带、敏感面（G3 脚本 / 两 agent def frontmatter）干净、验证集关键断言亲测成立。**landing 保真度 = GREEN**；merge 放行前置 = D1–D3（证据簿记三处补齐，使 fail-loud 结构门对原始制品可复现 GREEN）+ D4 登 ARQ 留根。merge 裁定归 human manual-merge 门。
