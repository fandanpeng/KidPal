# Change Triage（dev 侧变更分流前置门程序）

> `/triage-change` 入口下 architect 遵循的分流程序：判定标准、比例化深度、五档裁决、reviewed plan 就近落地。是 [architect-operating-model.md](architect-operating-model.md) §3/§4 路由的延伸，对偶事实源侧的 `/arch-change`。
> 来源：gov-review-architect-change-triage（FROZEN 2026-07-22）。门性质 = **Part-2 工程门 / bootstrap 临时信任门**（K9 §11 权威），非业务门、不进 DEV-GOVERNANCE §5 六门清单。

## 0. 铁律与地板

- **铁律：只要改自有实现面（固件业务 + 云端 shim）行为，再小也过门。** 入口不分大 feature / 小 feature / bug。（档 1 期间 dev-loop 未迁，本门完整生效待档 2。）
- **地板（免门，形状①直接修）**：仅**不改实现面行为**（docs/typo/只读探索）或人类明确跳过 → 不进此门。
- **四面探测（intake）**：变更进来先按 `change-intake.md` 做跨面探测——触面事实记 spine faceMap（触面 ≥2 建档）；命中触面事实（触 `docs/contracts/**` / systems spec / 跨系统结构性 / 存疑向严）→ 按 facePlan 路由（架构/契约面走 `/arch-change` loop），实现面留本门分档，不再整单踢走。governance 面（`.claude/agents` 定义 / gate）命中 → 路由 `/gov-change`。

## 1. 角色与写域（写域零扩张）

- **architect（子 agent，只读+审）**：判五档 + 出 reviewed plan + 写 rationale。**不写任何文件**（只读 + 审 + 同会话 return）。architect 是 full architect（有判断力、主持 review），**不是 `architect-reviewer`**（后者只读、只做契约影响分类 + lightweight eligibility，不主持收敛、不出 reviewed plan）。
- **主线程（`/triage-change` 编排者 = 人类主线程）**：落盘 reviewed plan。主线程不受 per-agent enforce-scope 限（dev-loop.md §4），可写 `delivery/workstreams`/`taskpackages`。让主线程落、architect 只审，正好绕开扩 architect 写域——**写域零扩张，不触 scope-map**。
- **planner / portfolio-planner**：写各自既有的家（本就是其写域），从家里读 reviewed plan 做 intake/拆包。
- **人类**：三个承重触点（见 §5）；FROZEN 归人类（作者≠判官）。

## 2. 判定 = slicing matrix + riskTier（architect 做人类级判断 + 写 rationale）

判档依据 [task-intake-and-slicing.md](task-intake-and-slicing.md) 的 slicing matrix + riskTier 默认升级规则，architect 做人类级判断并把 rationale 写进裁决。

| 档 | 判据 | 去向 |
|---|---|---|
| **③ 拆包** | 跨多切片 / 需多 Pod / 迁移 / 高风险大 feature | reviewed plan → `delivery/workstreams/<ws>/PROPOSAL.md`（人类冻结）→ portfolio-planner 拆 N 包 |
| **② 单包 Full** | 单切片但 high/critical（触 security 轴 / 真 DB / 状态机 / AI 成本审计）| reviewed plan → `delivery/taskpackages/<id>/plan.md` → planner 单 Pod，Full |
| **② 单包 Lite** | 单一来源/紧耦合小簇 · 单 owning 包 · 无悬挂契约决策 · 无迁移删数 · 一轮收 ⇒ 快路径默认适用（§7） | reviewed plan → `delivery/taskpackages/<id>/plan.md` → planner 单 Pod，Lite |
| **⓪ 档 0 nano** | 改动独立 + 爆炸半径小（每将触符号 inbound ≤5）+ 无验收语义净增 · 排除清单零命中（判据见下方「档 0 nano 判据」节） | reviewed plan（= 意图定义五件，frontmatter `rigor: nano`）→ `delivery/taskpackages/<id>/plan.md` → planner 单 Pod，nano |
| **① 直接修** | 不改 apps 行为 / 人类明确跳过 | 不进 loop（地板） |

**小 feature 与小 bug 合流为同一档（②单包）**：执行形状相同（单 Pod 单 TaskPackage 跑 dev loop），仅以 `sourceType`（feature→`proposal`/`human-ad-hoc`；bug→`regression`/`ci-failure`/`drift-report`；`human-confirmed-plan` 归 artifact-lifecycle origin.type，不预支）与 `plan.md` 内容框架（feature 写"建什么+方案"，bug 写"根因假设+修复+影响面"）区分。

**单向守卫（riskTier 优先于 rigor）—— convention-only（F1 / Q1-A）**：设计意图是单向校验——信号说"这是 Full 级"（security 轴 / 迁移 / 跨包 / core.* / 触契约）而 `rigor:"lite"` → 应强制升 Full。**但本门当前无此机器牙齿**（`taskpackage-required-check.mjs` 只扩了 `isProposalMdPath`，不含 riskTier-overrides-rigor 守卫，记 ARQ 后续 G3）；安全兜底靠**两层人工**：① 人类触点①确认/改判路由；② supervisor 对 riskTier=high 仍升级人类。残余洞需 architect 同时误设 riskTier(low)+rigor(lite) 且人类 rubber-stamp（复合失败）才漏。**诚实标注：这是 convention-only，不是机器保障。** 判断仍归 architect（写进裁决 rationale）。

### 档 0 nano 判据（261016-gov-review-nano-tier FROZEN v1.1 立）

位序在 ①直接修 之上、②单包Lite 之下：仍过门（不像①免门）但不开完整 TaskPackage 验收链仪式（不像②）。面向「意图完全明确、无验收语义净增」的实现面微改——典型形态 = 单个 UI 页面元素调整、单个 threshold 常量修改、资产消费点名切换。**判据（全部满足 + 排除清单零命中；存疑向严升档）**：

- **改动独立**：单一目标符号 / 单一 UI 页面元素 / 单一常量簇，改动意图一句话说清；
- **爆炸半径小（机器量，不靠人指定、不按代码行数）**：triage 时 architect 用 CodebaseMemory 图工具 `trace_path(direction="inbound")` 量每个将触符号的调用方集，**阈值 = 每将触符号 inbound 调用方 ≤ 5 且同属一个 owning 文件/紧耦合簇**（human 2026-10-16 实测钉死：三包 nano 意图包全 ≤4 vs 中枢调度符号 17，阈值落一个数量级断层空档）；预估 diff ≤2 个实现面文件作辅助参照；
- **无验收语义净增**：不改 story 语义、不引入新行为决策、不触新契约语义；`sourceType=proposal` 的包不得判 nano（proposal 载荷 = 决策集，decisionCoverage 不豁免，与 task-intake-and-slicing.md「decisionCoverage 与 rigor 的关系」现行规则一致）；
- **排除清单（命中任一即升，不判 nano）**：触 wire/契约面、状态机、跨端、安全轴、迁移/删数；目标区域无既有齿覆盖且 human 不愿目检兜底；意图需要新行为验收语义承载（→ 升 Lite）。
- **判档双层形状**：判档预估 = `trace_path` inbound（triage 时）；收口实测 = `detect_changes` 对 actual diff 算实际爆炸半径，结果记入白盒复核回执（advisory；落痕载体 = integrator review-packet「白盒复核」节）。
- **判档判注义务（比照 §7 lite 判注）**：architect 判 nano 时 rationale 须含一句「host 齿可闭环」确认（nano 的 suiteGate 回归网 = 既有 host 套件，可在 host_app target 闭环；与 lite 快路径判注义务同构，承接 launch/verify 两侧后置）。
- **落盘形状**：nano 裁决 → reviewed plan（= 意图定义五件）落 `delivery/taskpackages/<id>/plan.md`（与 ②单包同家，frontmatter `rigor: nano`）；意图定义五件 = ① 改动意图一句（改什么、为什么）② 目标符号/文件锚（即 changeManifest 种子，triage architect 给）③ 预期行为差（before/after 一句）④ 目标区域既有齿覆盖态申报（既有验收/单元齿清单，或「无齿 → human 目检兜底」声明）⑤ 爆炸半径预估（trace_path 结果摘要）。
- **越界升档（nano 无扩单通道）**：nano 包 change-manifest 闸红（diff 越出清单）或白盒复核发现出界 → planner 停，回本门重判档（典型升 Lite），不得原地扩 manifest 续跑——与 Lite 的 architect 扩单通道（fixer.md 边界节）有意不同，承载 human「diff 越界自动升档」裁定。
- **触点① nano 形态**：human 确认档 0 时一并确认意图定义五件与覆盖态兜底声明；单向守卫（riskTier overrides rigor）对 nano 同样生效（触契约/security/迁移信号 → 升档）。

## 3. 比例化 review 深度

| | 大 feature（③拆包） | 小 feature/bug（②单包） |
|---|---|---|
| review 深度 | 实质单轮：findings 逐条 + 处置 | 快审：确认判据 + 路由裁决 |
| 生成物 | `delivery/workstreams/<ws>/review.md`（findings + architect sign-off） | 裁决写进 `plan.md` frontmatter，不单独成件 |
| 人类确认 | **要**（触点②盖 FROZEN——诞生了跨会话耐久权威） | plan mode 里人类已批过 plan 即够（+ 触点①确认路由） |
| 逃生门 | 见 §6 reviewer-first | 同左 |

**dev 侧为何单轮而非对抗**：dev plan 不碰单一事实源（contracts/spec），错了 verifier 验收门会接住，不值得 challenger/defender 对抗红队。单轮 architect review + 人类确认，与体量对齐。这与 `delivery/review/` 的 spec/governance 对抗 review **不是同一回事**，不互相替代。

**深度纪律（防漂移，accepted-risk）**：小 review 不得漂成重审（过度治理），也不得橡皮图章（审不出问题）——靠五档裁决 + progressive-rigor 约定框住；loop-retro 监控吞吐与深度校准。

## 4. reviewed plan 就近落地（主线程落盘，delivery/review 不碰）

跨会话递不到 planner（planner 是另一会话）。落盘由主线程做，跨会话靠**家里的耐久文件**（plan.md / PROPOSAL.md），不靠返回一段 prose（沿用 dev-loop-handoff 纪律）。

```
/triage-change（主线程=编排者）
  └─ 派 architect（只读+审）→ 返回 reviewed plan + 裁决（同会话 return，非跨会话 prose）
  └─ 主线程 AskUserQuestion 呈现裁决，人类【确认 / 改判五档】（一句话）   ← 人类触点①
  └─ 确认后主线程把 reviewed plan 落进它的家（只搬运、不改内容）：
        ③大 → delivery/workstreams/<ws>/PROPOSAL.md（+review.md，freezeState=pending-human；按 docs/templates/WORKSTREAM-PROPOSAL.template.md 三问 + 分型模板起草——业务目的清单 / workType 数组 / spec 关系标注三项硬项缺失即 portfolio-planner intake fail-closed 打回，260912 拆包链 T1）
        ②   → delivery/taskpackages/<id>/plan.md（frontmatter：rigor/riskTier/sourceType/reviewed-by/date）
  └─（仅大 feature）人类盖 FROZEN（作者≠判官）                       ← 人类触点②
  └─ 报告下一步命令                                                    ← 人类触点③
        ▼（人类起对应会话）
  planner（/build-feature，指 id）/ portfolio-planner（指 ws）→ 从家里读 plan → intake → loop
```

**convention-only 不变量（本门当前无机器牙齿，靠约定 + 多层人工兜底；机器守卫记 ARQ 后续 G3）**：

1. **主线程只搬运 architect 返回的 reviewed plan + 裁决，不改内容（F7）**。主线程旁路 per-agent enforce-scope 是真实治理面，靠此约定 + universal deny 兜结构红线。
2. **plan.md 是 architect 审过的，planner 只读消费、改需留痕（F8）**。完全漂移保护依赖 artifact-lifecycle 的 origin.frozen（本提案缺位，convention-only）。
3. **单向守卫（riskTier overrides rigor）convention-only（F1/Q1）**：多层兜底 = 人类触点① + supervisor riskTier 升级。
4. **FROZEN 拆包门 convention-only（F6/Q5）**：portfolio-planner 只拆 FROZEN 的 PROPOSAL（无 freezeState 机器校验，靠自觉）；未冻 → 先走 architect review + 人类冻结。
5. **逃生门 reviewer-first（F10）**：见 §6。

**大 feature FROZEN**：PROPOSAL.md 由**人类**盖 FROZEN（版本/sha）；机器判据 = `review.md`（architect sign-off + findings）存在 + human confirmation record。**architect 不能自冻**——scope-map architect allow 不含 `delivery/workstreams`，物理写不进（作者≠判官由 scope **机器兜住**）。

**大 feature `delivery/workstreams/<ws>/` 由谁建（F4 / Q4）**：triage 时由**人类/主线程**创建 `delivery/workstreams/<ws>/` 装 reviewed PROPOSAL。这把"workstream 创建"拆成两半：**设计权威落家**（triage 时人类/主线程建目录 + PROPOSAL/review）与**调度态建立**（portfolio-planner 拆包时 attach 补 `task_plan`/`pod-board`/`progress`）——portfolio-planner 的"workstream 权限"读作**拆包+调度**权，不垄断建目录。**这是对 ARQ-065/066 RTE 集权的有意放宽（deliberate relaxation，human-confirmed 2026-07-22，非回归）**，与 architect-operating-model §4 / task-intake create-workstream 补注一致。

**落地顺序（②单包，F11）**：主线程先建 `delivery/taskpackages/<id>/plan.md`（含 frontmatter 裁决）→ planner 跑 `/build-feature <id>` 时建 `taskpackage.json` 做 intake（sourceRefs/origin 回指 plan.md，`rigor` 取自 frontmatter）。两次写跨人类触发的独立会话顺序发生，无竞态。

## 5. 三个承重人类触点（落盘本身全自动，人类只保留三下，每下承重、非帮机器填表）

| 触点 | 何时 | 人类做什么 | 为何承重 |
|---|---|---|---|
| ① 确认/改判路由 | architect 返回裁决后、落盘前 | 一句话确认五档，或改判（AskUserQuestion） | Lite/Full 是安全相关判断，architect 误判需人类 backstop |
| ② 冻结 PROPOSAL | 仅大 feature，落盘后 | 盖 FROZEN（版本/sha） | 作者≠判官：architect 起草/主持 review，判官是人类 |
| ③ 起下一段会话 | 落盘（+冻结）后 | 跑主线程打出的命令（planner `/build-feature <id>` 或 portfolio-planner `<ws>`） | planner/portfolio 必须是自己的主线程会话——子 agent 不能再派子 agent |

小 feature 动线：plan mode 批准 → `/triage-change` →（自动 architect 审）→ 触点①确认 →（自动落盘 plan.md）→ 触点③跑 `/build-feature`。大 feature 在①与③之间多触点②（盖 FROZEN），触点③跑 portfolio-planner。

### 触点①预授权（gov-review-devloop-closeout-hardening §3.6，human-approved 2026-07-27）

人类可对触点①给**一次性预授权**（architect 在声明的档位集合内自行确认落盘，不每次回问）。预授权是 backstop 的**例外**，从严解释：

- **有效条件**（全部满足才可当触点①履行）：① 裁决落预授权声明的档位集合内；② findings 全文本级回灌 PROPOSAL/plan；③ 无升档；④ 未触 security 轴；⑤ 方案形状未被 CONFIRMED 改变；⑥ **仅本次有效、不创先例**（每次预授权独立成立，不得援引既往先例扩大适用范围）。
- **必须回问枚举**（命中任一即回问人类，不自行推定有效）：升档（如 ②→③）；触 security 轴；方案形状被 CONFIRMED 改变；出现 BLOCKING 级分歧；**出现条文未覆盖的新情形**（枚举外情形一律回问）。
- **留痕形式**：`human-decision` pod-event + taskpackage notes 记采纳项 + `origin.upstreamFrozen` 钉**回灌后**版本（260726 形态成文化）。

## 6. 逃生门（reviewer-first，F10）

分流 review 中发现**疑似**触契约/spec/agent 定义/gate → **先派 `architect-reviewer` 四分类**（对偶 dev-loop 的 reviewer-first 路由）→ `ACCOMPANIES_SPEC_CHANGE`/`SPEC_CONTRACT_DRIFT`/`CONTRACT_CHANGE_REQUIRED` 转 `/arch-change` loop（契约变更 = 架构变更的一种；本门无权批准）。dev 分流门无权批准事实源变更，不越权。

## 7. Lite TaskPackage 字段级定义

Lite 省的是"方案/决策映射的文牍"，不是 loop 的任何一道闸。字段级定义（不可省 / 可省、`rigor` 字段、`planRef`、decisionCoverage 与 rigor 关系）见 [task-intake-and-slicing.md](task-intake-and-slicing.md)「rigor 字段与 Lite TaskPackage 字段级定义」。

### lite 快路径（261015-gov-review-lite-fast-path T1）

`rigor=lite` ⇒ **快路径默认适用**（launch 面环境后置 = `kidpal-pod-launch-prep` SKILL lite 分支；verify 面 firmware-build scoped 相位后置 = verify-gates.json defer 字段、final 相位恒跑）。机器只读 rigor，无独立资格字段。

**triage 判注义务**：architect 判 lite 时，rationale 须含一句「host 齿可闭环」确认（验收内循环可在 host_app target / server pytest 闭环）；若判 lite 但设备编译面深（host_app target 盖不住的 seam，如触设备专有编译单元），须注明「快路径不适用、按 full 环境备」——launch/verify 快路径对注明的包不生效。

**复议触发器（YAGNI）**：出现首个真实「full 但 host 可闭环」的包时，再评是否立独立资格字段（落新 ARQ 条目），本批不预设。

## 8. proposal 合法路径

`sourceType=proposal` 的合法 proposal 路径为 `delivery/review/**/PROPOSAL.md` **或 `delivery/workstreams/<ws>/PROPOSAL.md`**（大 feature 设计权威）。G3 `taskpackage-required-check.mjs` 的 `isProposalMdPath` 已同步扩认 `delivery/workstreams`（本提案落地的机械前提；artifact-lifecycle 的 origin.ref 校验复用之，sequencing 上本提案先于/同批）。

## 9. 强制力说明（honest，F9）

`/triage-change` 当前是**约定入口**（像 dev-loop 本身是"强制默认"靠约定），无 hook 强制牙齿——可被跳过，但下游 supervisor/verifier 门兜底。机械强制牙齿（如 closeout-check 查 plan.md + reviewed-by）列后续 G3，不阻塞本程序。铁律"只要改 apps 行为再小也过门"是**convention-enforced**。

## 10. 强制默认与例外

- **默认**：改自有实现面（固件/shim）行为的变更开工前过本门。
- **例外（形状①）**：不改 apps 行为（docs/typo/只读探索）、或人类明确跳过。例外不能顺手扩大成正式实现。
