# PROPOSAL · 260908-gov-review-input-surface-frontload

> **状态：DRAFT v0.1 · 2026-09-08 · 主线程起草完毕，待 human 立项 ack（ack 后送 architect 评审循环）**
> **承载 ARQ**：无既有 ARQ。来源 = human 2026-09-08 指示的 pod 遥测专项分析（12 包 / 72 条 dispatch-completed 事件）；动机件之一 `260907-g-relay-fix` pod-retro「下次改进 1」由本批机械化承载，不另立 ARQ。姊妹项（dispatch-stamp 补记实际生效模型 + token 分项）为已执行测出的机械改进项，走 gov-arq 快路径另行登记，见 §6。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。升级判据核对：不触 `docs/contracts/**`（契约面为零）、非跨系统结构性变更（治理/流程面单侧）→ 不升 Track A。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T7）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（一句话证成：闸向严不向松、存量包零追溯、有 manifest-missing 降级兜底故 downside bounded；但改四个 agent def 的派发/交付行为 + 扩承重闸判定，非纯成文）。
> **review profile**：G1+G2+G3（触面 = `docs/DEV-GOVERNANCE.md` G1 成文 + 四 agent def 与三份 references G2 + 两脚本 G3；G1/契约面关系说明：按 DEV-GOVERNANCE §6 表（`:212`）字面，DEV-GOVERNANCE.md 属 G1 面——batch-2 先例将其成文标 G2 并声明「G1 为零」，本批按存疑向严标 G1，归类请判官复核改判；契约面为零）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
docs/DEV-GOVERNANCE.md
.claude/scripts/brief-inputs.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/agents/planner.md
.claude/agents/portfolio-planner.md
.claude/agents/acceptance-author.md
.claude/agents/builder.md
.claude/references/task-intake-and-slicing.md
.claude/references/dev-loop.md
.claude/references/dev-loop-operating-system.md
.claude/scripts/pod-events-aggregate.mjs
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把 dev-loop 子 agent（acceptance-author / builder）的输入面从「开放自扫」收敛为「起包期派生的封闭清单 + 超读申报」，压缩重派发挂钟长尾，并把 g-relay-fix 实证教训「跨端验收必须读对端解析器源码」变成有闸承载的起包字段。

**现状差距**：TaskPackage 的引用字段已经锚点化（`taskpackage-required-check.mjs` 头注 `:12`：「sourceRefs 必填非空语义收窄为代码范围锚点」），但没有任何一步把它们在派发期展开成封闭清单；agent 定义里写的输入是开放措辞（`builder.md:24`「相关 contracts、events、shared types、system spec」、`acceptance-author.md:24`「相关 `docs/contracts/**` 与系统 spec」），子 agent 每次派发各自重新发现输入面。跨端性在既有机械字段上不可见（§1 P2 全量普查实证），无字段、无闸承载。

**非目标（本批不做）**：
- 不做契约/spec 节选 digest 落 brief（影子副本风险，phase-2 凭 §5 仪表盘数据再议，OQ2）；
- 不动任何模型档位/派发 model 覆盖规则（MODEL-TIER 条款原样；dispatch-stamp 遥测补全另走 ARQ，§6）；
- 不触契约面、不触 arch-review 侧角色（scenario-author/challenger 等的 charter 机制是对偶先例，不是改动对象）;
- 不追溯存量包（闸只读 current taskpackage，历史包天然不触发）。

## 1. 问题与根因（地面真值）

### P1 · 重派发长尾的形态 = 开放输入面自扫

遥测现跑（2026-09-08，命令 = 对 `.claude/taskpackages/*/pod-events.jsonl` ∪ `changes/dev/*/pod-events.jsonl` 按 eventId 去重聚合，python 一次性脚本，本次分析会话实跑输出）：72 条 dispatch-completed 事件（12 包，2026-08-31→09-08），其中 40 条 token 精确。按 actor：

| actor | n | 挂钟合计 | 挂钟中位 | 长尾实例（eventId 前 8 位） |
|---|---|---|---|---|
| builder | 15 | 329 min | 12.8 min | `20241a2c` story-p3 implementation 69.3min/109 tools |
| acceptance-author | 15 | 210 min | 7.4 min | `cc79b2fb` session-logic acceptance 61.2min/84 tools/108,585 tok；`1618c89d` story-p3 acceptance-freeze 52.0min/113 tools |
| integrator | 7 | 68 min | 8.7 min | — |
| fixer | 3 | 16 min | 5.4 min | — |

长尾派发的过程体感与「自扫输入面」直接对应：`260829-story-p3/pod-retro.md:11`（verbatim 摘句）「它烧了将近 19 分钟、几十次工具调用，**把 spec/契约/先例读了个遍**，然后回一句『Now I have all the inputs…』就停了——磁盘零落件」（对应 friction 事件 `0ebaf461`，stall 型）。

**根因**：refs 锚点已存在但无机械展开步骤；「相关 X」的开放措辞把输入面发现成本压给每次派发的子 agent 会话。

### P2 · 跨端对端源码不在输入面，机械字段推不出跨端性

动机实证：`changes/dev/260907-g-relay-fix/pod-retro.md:23`（verbatim）「跨端协议验收：acceptance-author 输入清单固定加『对端解析器源码路径』」——该包 builder 按冻结契约自创 `display_relay` 帧类型，离线全绿、真机被固件 `Unknown message type` 拒（同文件 `:5` 段），冒烟炸后靠翻 `application.cc`/`mcp_server.cc` 才补上课。

全量普查（2026-09-08 现跑，python 对 19 个 `.claude/taskpackages/*/taskpackage.json` 的 subsystem/sourceRefs/contractRefs/docRefs/acceptanceTestsFile 判定）：
- subsystem 含 firmware+server 双面的包 9/19；
- **动机包 g-relay-fix 自身 `subsystem=server`，全部字段无任何 firmware 路径**——纯路径/ subsystem 谓词抓不住它；
- 19/19 包 refs 均引 `docs/contracts/interface-contract.md`——「引契约」无判别力，不能当跨端谓词。

**根因**：跨端性是设计判断不是可推导属性；既有 schema 无申报字段，闸无承载，教训只活在 retro 散文里（= 五条清单头注 `:291` 说的「期望只活在闸的判定代码里、起草侧零成文」的镜像形态：这次连闸都没有）。

### P3 · 超读/探索行为无记账面、无仪表盘

friction 枚举 canon（`dev-loop-operating-system.md:122`，verbatim）「`{frictionType: stall|scope-drift|scan-miss|degraded, summary}`」——无 over-scan 形态；`pod-events-aggregate.mjs` 现输出 tokens/coverage/escalation/friction 汇总（实跑 `node .claude/scripts/pod-events-aggregate.mjs` 确认，2026-09-08，默认 current 包 story-p3 输出 eventsTotal=17），**无 per-actor 挂钟/toolUses 分布列**——本次专项分析只能手写一次性 python，cadence 复盘（/loop-retro）无现成消费面。

**根因**：观测面缺位 → 「哪些派发在超读」不可见 → 输入面治理没有反馈回路。

### 先例论证（对偶迁移，已读先例原文确认形态）

- **arch 侧 charter 机制**：`scenario-author.md:31`（verbatim）「建**双向消费方清单**:把 spec 里每个 expose/喂给/只读投影/事件 producer 列出,标『匹配的消费方声明在哪』—— 留空处就是 challenger 要打的断链点」；`:64`「单列才暴露留空的断链点(毛利缺口教训)」；`:74` charter 产物含「双向消费方清单(含留空点)」。本批 input-brief = 该冻结阅读面机制在 dev-loop 侧的对偶（acceptance-author/builder 消费，planner/脚本产出）。
- **五条清单同型**：`docs/DEV-GOVERNANCE.md:289-297`「起包格式期望前置」五条（mutationEvidence 骨架 / 断言格式 / 验收 ID / 降级声明前置 / 闸判据自查），四方对象恰为本批同一组角色；本批第六条与之同质（起包期把派发期期望前置成文 + 字段 + 闸）。第 1 条「空骨架也行，字段必须在场」与第 4 条「降级声明（不适用 + 理由）在 Draft 阶段写字段」是 T3 闸形态的直接模板。

## 2. 改法（T1…T7）

### T1 · DEV-GOVERNANCE 五条清单扩六：「输入面期望前置」

改 `docs/DEV-GOVERNANCE.md`「## 起包格式期望前置」节：头注 `:291` 末句「五条前置清单：」→「六条前置清单：」（承重 landing 指令，原文引用钉死，替换后完整句 = 「…跨 4 包 5 子项实证）。六条前置清单：」）；第 5 条后追加第 6 条，要素：
1. planner 派 acceptance-author / builder 时附 **input-brief**（`brief-inputs.mjs` 从 TaskPackage refs 派生的封闭输入清单：锚点节定位 + 跨端对端面 + 先例封顶 3 个）；
2. 子 agent 清单外阅读允许，但交付报告必须列「超读文件 + 原因」段——激励措辞钉死：**超读是常态动作，不申报才是问题**（防止为过闸藏超读、申报数据失效）；
3. `crossEnd` 申报字段 Draft 必在（空骨架也行）+ finalize 时 counterparts 非空 **或** exempt+理由 二选一（与第 1/4 条同型）。

对应 §1 P1/P2 根因：把「输入面期望」从散文教训升为条文 + 字段 + 闸。

目标文件：`docs/DEV-GOVERNANCE.md`

### T2 · 新脚本 `brief-inputs.mjs`（派生清单，不新造事实源）

`node .claude/scripts/brief-inputs.mjs --change-id <id> --role <acceptance-author|builder> [--dry-run] [--self-check]`，读 `.claude/taskpackages/<id>/taskpackage.json`，输出 `.claude/taskpackages/<id>/input-brief-<role>.md`（`--dry-run` 输出 stdout 不落盘，供历史包 fixture 验证）。四路展开：
- **① 契约/spec 锚点**：`contractRefs`/`docRefs`/`sourceRefs` 的 `#锚点` → 定位所属标题节，列「路径 + 锚点 + 节标题」；**不抽正文**（避免契约影子副本，digest 是 OQ2 的 phase-2）；
- **② 跨端对端面**：`crossEnd.counterparts[]` 的 `{path, symbol}` → 生成时点 grep 解析 symbol 行号区间 + 生成时间戳（**行号不落 taskpackage 库**，防腐化）；
- **③ 先例封顶**：同 subsystem 已 merged 包的 `acceptanceTestsFile`，按时间倒序封顶 3 个（治 P1「把先例读了个遍」）；
- **④ 超读申报段模板**：交付报告的「清单外阅读 + 原因」空表。

advisory（非门）：包 refs 引 `interface-contract.md` 的 wire 节（§1.5/§三）且 `crossEnd.counterparts` 空且无 exempt → stderr WARN 提示 planner 复核申报。`--self-check` 用内置 fixture 断言四路输出形状（真空绿规约：fixture 非空断言先行）。对应 P1/P2。

目标文件：`.claude/scripts/brief-inputs.mjs`（新增）

### T3 · `taskpackage-required-check.mjs` 扩三条判定（扩承重闸，不新立）

1. `crossEnd` 字段在场性：对象 `{counterparts: [], exempt?: string}`，Draft 空骨架放行（同 mutationEvidence 条）；
2. finalize 态（`acceptanceTestsFile` 非空）：`counterparts` 非空（条目须含 path+symbol）**或** `exempt` 非空理由，二选一，双缺 = RED 点名；
3. finalize 态：`input-brief-acceptance-author.md` 与 `input-brief-builder.md` 在 taskpackage 目录存在（acceptance-author 的 brief 在 Step 7 派发前已生成，时序上先于首次 scoped verify 轮，不产生鸡生蛋）。

存量零追溯：闸只读 current taskpackage，历史包不触发（与 CHANGE_ID cutoff 同约定）。单源红利：closeout 相位程序化调用本脚本（`dev-loop.md:229`，verbatim 摘句「承重字段/decisionCoverage 校验由 closeout 相位程序化调用 `taskpackage-required-check` 单源承重（F-04）」），扩展自动传导，`verify-gates.json` 与 `closeout-check.mjs` 零改动。对应 P2。

目标文件：`.claude/scripts/taskpackage-required-check.mjs`

### T4 · 四 agent def 修订（派发指令 + Inputs 封闭化 + 引用行同步）

- **planner.md**：Step 7（`:54` 派 acceptance-author）前置「先跑 `brief-inputs.mjs --role acceptance-author`，派发 prompt 引用 brief 文件路径 + 超读申报纪律一句，**不转抄清单正文**（防双源漂移）」；Step 9（`:56`）builder 派发同理；Step 8 finalize（`:55`）硬前置加「两份 input-brief + crossEnd 二选一」；Step 1 intake 加跨端判断（设计判断字段：本包是否断言/改变跨端 wire 形状？判断不了按存疑向严填 counterparts 而非 exempt）；`:109` 引用行「五条」→「六条」。
- **portfolio-planner.md**：拆包草案预填 `crossEnd` 骨架（`:58` 引用行同步「五条」→「六条」+ crossEnd 预填点名——与 mutationEvidence 骨架教训同型：起草侧不带，闸红回派）。
- **acceptance-author.md**：`:24` Inputs「相关 `docs/contracts/**` 与系统 spec。」→「`input-brief-acceptance-author.md`（封闭输入面，脚本从 TaskPackage refs 派生）；brief 缺失时降级自扫并在交付标 `manifest-missing`。」；`:29` Step 1「读取 TaskPackage、contracts、系统 spec」同步改指 brief；Step 7 交付输出加超读申报段；`:46` 引用行「五条中的②③」句保持、追加第六条引用。
- **builder.md**：`:24` Inputs「相关 contracts、events、shared types、system spec。」同型改法（brief-builder）；`:46` 引用行同步。

对应 P1（封闭输入面）+ P2（intake 判断点）+ 五条→六条投影一致性。

目标文件：`.claude/agents/planner.md`、`.claude/agents/portfolio-planner.md`、`.claude/agents/acceptance-author.md`、`.claude/agents/builder.md`

### T5 · `task-intake-and-slicing.md` 字段级定义：crossEnd 入 Lite 不可省清单

`:211` 节（rigor 与 Lite TaskPackage 字段级定义）加 `crossEnd` 字段定义（形状 / 二选一语义 / advisory WARN 行为），并把 `crossEnd` 骨架列入 Lite 不可省清单（`:217`）——实证依据：**动机包 g-relay-fix 本身是 Lite 包**（其 taskpackage notes verbatim「本包以新建 Lite 包承载并如实标注」），Lite 豁免即放过动机案例。对应 P2。

目标文件：`.claude/references/task-intake-and-slicing.md`

### T6 · `dev-loop.md` 文件地图注册 input-brief

§2 文件地图（`:72`）与 taskpackages 标准内容节（`:125` 段）注册 `input-brief-<role>.md`：脚本生成派生物、Git-tracked（taskpackages 目录本就 tracked）、证据属性（超读申报与 WARN 复核的锚点）。对应 P3 的制品面。

目标文件：`.claude/references/dev-loop.md`

### T7 · 观测面：friction over-scan 型（按 OQ1）+ 聚合仪表盘列

- `dev-loop-operating-system.md:122` friction 枚举按 OQ1 裁断落值（推荐 (a) 扩 `over-scan`）；维持 `:122` 既有定性「纯 advisory、绝不当门」（DP-D4）——over-scan 只记账不拦截；
- `pod-events-aggregate.mjs` 加 per-actor `durationMs`/`toolUses` 中位 + 长尾（>40min）计数列，`/loop-retro` cadence 直接消费；pilot 2–3 包后凭该仪表盘回看长尾收敛（阈值从数据定，本批不预设「≤60 tools」类拍数目标）。

对应 P3，并给 OQ2（digest phase-2）提供决策数据面。

目标文件：`.claude/references/dev-loop-operating-system.md`、`.claude/scripts/pod-events-aggregate.mjs`

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明（消费方） |
|---|---|---|---|
| 治理成文 | `docs/DEV-GOVERNANCE.md` | G1 成文（五条→六条 + 头注计数） | 四 agent def 引用行（T4 同批同步）；grep 「五条」全仓消费面现跑（2026-09-08）：`planner.md:109`、`portfolio-planner.md:58`、`acceptance-author.md:46`、`DEV-GOVERNANCE.md:291` 四处，全部入 T1/T4 改动集，无漏网 |
| 闸脚本 | `.claude/scripts/taskpackage-required-check.mjs` | G3 扩判定（向严） | `verify-gates.json:32-34` taskpackage-required 闸（配置零改动）；`closeout-check.mjs` F-04 单源调用（`dev-loop.md:229`）自动传导 |
| 新脚本 | `.claude/scripts/brief-inputs.mjs` | G3 新增（只读 taskpackage + 写 taskpackages 目录内 brief，无 canonical 写面） | planner Step 7/9 调用；`--dry-run`/`--self-check` 供验证集 |
| 聚合脚本 | `.claude/scripts/pod-events-aggregate.mjs` | G3 加列（向后兼容，新列只增不改既有输出键） | `/loop-retro` cadence；本 PROPOSAL §5 V4 |
| agent def | planner / portfolio-planner / acceptance-author / builder 四件 | G2 行为修订 | 派发/交付合同变化；arch-review 侧角色零触碰 |
| references | task-intake-and-slicing.md / dev-loop.md / dev-loop-operating-system.md | G2 成文 | 字段定义 / 文件地图 / friction 枚举的三个 SSOT 投影 |
| 契约面 | `docs/contracts/**` | **未触（零）** | — |
| scope-map / settings / verify-gates.json | — | **未触** | 新脚本落点 `.claude/scripts/` 为既有平台面，无新写槽需求 |
| taskpackage schema | `crossEnd` 新字段 | G2/G3 边界（字段定义在 references，判定在闸） | 存量 19 包零追溯（闸只读 current） |

**【硬项 1b · sweep 透镜义务】** 新门判定 pattern 全仓预跑已完成：cross-end 谓词对 19 个存量 taskpackage 的普查（命令与输出摘要见 §1 P2，2026-09-08 现跑 python 一次性脚本，逐包打印 subsystem + 双路径判定 + interface-contract 引用判定，无截断省略形态）；结果直接塑造了 T3 闸形态（纯机械谓词抓不住动机包 → 申报字段 + 二选一，而非硬推导）。机械枚举 census：「五条」消费面 grep 命令与四处命中已列本表首行。

**【硬项 1c · 复扫模式完备性先验】** 不适用：本批无退役语面/残留清扫（「五条」→「六条」为计数词更新，消费面已全量枚举于上表，非模式复扫）。

## 4. human 取舍点（OQ）

- **OQ1 · over-scan friction 型归属**：(a) `:122` 枚举扩 `over-scan` 新型（**推荐**——与 `scan-miss`（该扫没扫）语义相反（不该扫扫了），独立型才可分向统计；枚举是关键词形态、扩型零迁移成本）；(b) 复用 `scan-miss` 靠 summary 措辞区分（省一个枚举值，但聚合统计混向）；(c) 不入 friction，只留交付报告段（零 canon 改动，但 cadence 复盘无机器可聚合信号）。
- **OQ2 · phase-2 digest 是否预授权**：(a) **不预授权**（推荐）——phase-1 封闭清单落地后，凭 T7 仪表盘量 2–3 包，若整文件通读仍占长尾大头再提案；digest = 契约节选副本，与影子目录禁令精神相抵，须凭数据立项；(b) 本批预授权（全节抽取 + sha256 钉源 + 非权威标注），省一轮提案但先造副本面后验需求。
- **OQ3 · crossEnd 闸形态**：(a) **骨架必在 + 非空或豁免二选一**（推荐——§1 P2 普查实证硬谓词抓不住动机包；豁免理由留痕可被 integrator/loop-retro 复核，形态照抄五条清单第 1/4 条既成模式）；(b) 仅 subsystem 含双面时硬红（会漏 g-relay-fix/gateway-directionB 类单面标签跨端包，等于为可见的一半设闸）；(c) 纯 convention 不设闸（回到「期望零成文零承载」老路，与本批立项理由自相矛盾）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

- **V1 · 脚本自检**：`node .claude/scripts/brief-inputs.mjs --self-check` → exit 0，stdout 含四路形状断言名（锚点节/对端面/先例封顶/申报段）；重放真空绿规约：self-check 先断言内置 fixture 非空。
- **V2 · 历史包 dry-run fixture**：`node .claude/scripts/brief-inputs.mjs --change-id 260907-g-relay-fix --role acceptance-author --dry-run` → stdout 含 `interface-contract.md` 锚点节行 + 超读申报段模板；随后 `git status --short .claude/taskpackages/260907-g-relay-fix/` → 空输出（dry-run 不落盘断言）。
- **V3 · 闸负正两态**：fixture taskpackage（refs 引 wire 节、缺 `crossEnd`）→ `taskpackage-required-check.mjs <fixture路径>` exit 1 且 message 点名 `crossEnd`；补 `{counterparts:[],exempt:"<理由>"}` 骨架 + 两份 brief fixture 文件 + `acceptanceTestsFile` 非空 → exit 0。先断言 fixture 文件非空（真空绿规约）。
- **V4 · 聚合新列**：`node .claude/scripts/pod-events-aggregate.mjs --change-id 260829-story-p3` → 输出 JSON 含 per-actor 挂钟/toolUses 中位键，且 acceptance-author 行 toolUses 中位与 pod-events 原值一致（story-p3 有 113-tools 派发，spot-check 可辨）。
- **V5 · 投影 grep**：`grep -c "六条前置清单" docs/DEV-GOVERNANCE.md` ≥1；`grep -rn "五条前置清单\|五条中的" .claude/agents/` → 零命中（计数词投影全同步）；四 def 各含 `input-brief` 引用；`dev-loop-operating-system.md` `:122` 枚举含 OQ1 裁断值；`dev-loop.md` 文件地图含 `input-brief`。
- **V6 · pilot 回看（交日落复盘，非 landing 门）**：本批 landing 后前 2–3 个新包，凭 T7 仪表盘对比基线（§1 P1 表：acceptance 长尾 52–61min、builder 长尾 69min、story-p3 stall 形态）；守护指标 = 冻结后 acceptance 缺陷再派次数（fix/amendment/harness-repair 类，基线：story-p3 单包 4 次）与 fixer 轮数（基线：12 包共 3 次）**不得恶化**——恶化即清单切漏必要输入，回补 brief 展开规则而非废机制。

## 6. 留根清单

- **dispatch-stamp 遥测补全**（另走 gov-arq，不入本批）：`planner.md:57` MODEL-TIER 条款「dispatch-stamp 若可得须补记实际生效模型」欠账未实现 + token input/output/cache 分项缺失 + provider 槽位映射（260903 期间 opus/sonnet 槽位曾映射 deepseek-v4-flash，两包数据模型标签失真）无字段钉——三项一并登记 ARQ。
- **frictionType `environment` 漂移**：实测数据已出现 `environment` 值（260829-session-logic）但不在 `:122` 枚举——本批不夹带（防 scope creep），留 ARQ 或下批治理批收编。
- **planner 主线程自身 token 无遥测**：本批仪表盘只覆盖子 agent 派发；主线程消耗（可能是 Pod 最大单项）观测另议。
- **交日落复盘项**：V6 pilot 回看（2–3 包后）；OQ2 digest 立项与否凭该数据。
- **自律件（不进模板）**：planner 派发 prompt 只贴 brief 文件路径 + 纪律一句，不转抄清单正文；brief 生成失败时降级派发须标 `manifest-missing`，不静默。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
