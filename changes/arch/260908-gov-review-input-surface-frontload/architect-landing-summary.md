# architect-landing-summary · 260908-gov-review-input-surface-frontload

> 主线程 packet-bound landing 执行回报（2026-09-09）。蓝本 = **FROZEN v1.0** `PROPOSAL.md`（§2 改法集 T1–T7 + §5 验证集 V1–V6）；落点 = **11 个 manifest 路径（11/11）**；判官轨迹 R1(14) → R2(6) → R3(1，熔断) → **R4 新判官实例 GREEN**。
> **packet-id** = `260908-gov-review-input-surface-frontload` §2 改法集（T1–T7）+ §5 验证集（V1–V5 为 landing 门，V6 交日落复盘）。

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

## 1. 落点对照（manifest 11/11）

| manifest 路径 | T 项 | 落了什么 |
|---|---|---|
| `docs/DEV-GOVERNANCE.md` | T1 | 「## 起包格式期望前置」头注末句计数词 **五条 → 六条**（承重替换，替换后完整句 = 「…跨 4 包 5 子项实证）。六条前置清单：」，与 PROPOSAL 钉死的原文逐字一致）；第 5 条后追加**第 6 条「输入面期望前置（input-brief + 超读申报 + crossEnd）」**三要素：① planner Step 7 前置一次产双份 brief、派发只引路径不转抄正文；② 超读允许但必申报，激励措辞钉死「**超读是常态动作，不申报才是问题**」；③ `crossEnd` Draft 骨架必在 + finalize 二选一（`exempt` ≥12 字符），并写明「跨端性是设计判断不是可推导属性」与两案实证（story-p3 stall / g-relay-fix 帧类型） |
| `.claude/scripts/brief-inputs.mjs` | T2 | **新增**（**474 行**，C1 更正：初稿自报「约 340 行」不实，以 landing-verifier 实测为准）。四路展开：① 锚点 → 所属标题节定位（只出路径+锚点+节标题+节首行，**不抽正文**；裸路径如实标「无锚点（整文件）」+ ADV；定位不到标 not-found 不猜）；② `crossEnd.counterparts` 的 `{path,symbol}` 生成时点解析行号区间（brace 平衡求块界、病态块封顶）、写 `generated-at`，**解析失败 fail-loud 非零退出点名条目、不静默产空锚**（§8-B1），越界档（非 `firmware/**`·`server/**`）给 ADV 不硬红；③ 先例封顶 3 —— 三判定源钉死：merged = `supervisor-verdict.json` verdict=APPROVE ∨ pod-events merge 事件（**排除 board**）、subsystem = **token 化包含**、时间轴 = **changeId 日期前缀**，`current/` 指针副本显式排除；④ 超读申报段空表模板。advisory WARN 谓词 = `isWireRef`（**锚点绑定契约文件名** ∧ 节 ∈ `WIRE_SECTIONS`）；`WIRE_SECTIONS = ['§1.5','§三','§四']` 为脚本顶部显式常量 + 注释钉维护义务与 §1.2/§1.3/§二 待评清单，`export` 供普查单源复用（R2-F1 教训落地）。`--dry-run` 只出 stdout 不落盘；`--self-check` 19 条断言（fixture 非空先行） |
| `.claude/scripts/taskpackage-required-check.mjs` | T3 | 扩三条判定（向严、零新门）：① `crossEnd` 骨架必在（空 `counterparts` 数组亦算在场）；② finalize 态（`acceptanceTestsFile` 非空 = 既有代理）二选一——`counterparts` 非空且每条含 path+symbol ∨ `exempt` 非空且 **≥12 字符**（`CROSS_END_EXEMPT_MIN_CHARS`，对齐同闸 `landsIn=dropped` reason 地板，§8-B2），双缺红点名并带「判断不了按存疑向严填 counterparts」指引；③ finalize 态两份 `input-brief-<role>.md` 必在 taskpackage 目录（红消息内嵌 Step 7 生成命令）。头注补本批修订段 + 过渡口径。**配置零改动**：`verify-gates.json:32-34` 与 `closeout-check.mjs:309` 单源调用自动传导 |
| `.claude/agents/planner.md` | T4 | 新步 **1d 跨端判断**（含「判据不是 subsystem 写没写两面」与 `260907-g-relay-fix` 反例、向严填 counterparts 指引）；**Step 7 前置一次产双 brief**（明写 builder 那份不得留到 Step 9、脚本 fail-loud 即修再派、派发 prompt 只引路径 + 申报纪律一句、不转抄清单）；Step 8 硬前置两条；Step 9 **只引用**（refs 后改则重跑覆盖，事由落 brief 头部 `regenerated-because` 行，不另开记账面）；`:109` 引用行五条 → 六条并点名第 ⑥ 条 |
| `.claude/agents/portfolio-planner.md` | T4 | `:58` 引用行「五条」→「六条」+ **点名 `input-brief`**（R2-F2 取建议 (i)：「`crossEnd` 骨架与 `input-brief` 同源——预填 counterparts 即 planner Step 7 brief 第②路输入」），使 V5④ 四臂由冻结文本必然产绿 |
| `.claude/agents/acceptance-author.md` | T4 | `:24` Inputs 开放措辞 → `input-brief-acceptance-author.md` 封闭清单（+ `manifest-missing` 降级）；Step 1 读取面改指 brief 并加**对端解析器必读**义务（含「按本侧契约想象」反面教训）；Step 7 交付加超读申报表回传；`:46` 引用行「起包清单**六**条中的②③」+ 第 ⑥ 条引用（R1/F7 消歧落地：计数词替换而非「句保持」） |
| `.claude/agents/builder.md` | T4 | `:24` Inputs 开放措辞 → `input-brief-builder.md` 封闭清单（+ `manifest-missing`）+ 申报条；`:46` 引用行追加第 ⑥ 条（该句原不含「五条」字样，故为追加而非替换） |
| `.claude/references/task-intake-and-slicing.md` | T5 | `crossEnd` 字段级定义段（形状 / Draft 骨架 / finalize 二选一 / **`exempt` ≥12 地板显式投影进起草侧文档**（R2-F5）/ 向严指引 / 本批覆盖档 = 自有实现面 + ②③档留 ARQ / advisory WARN 行为 / 消费方），并列入 **Lite 不可省清单**（注明动机包本身即 Lite） |
| `.claude/references/dev-loop.md` | T6 | §2 文件地图新增 `scripts/brief-inputs.mjs` 行；`pod-events-aggregate.mjs` 行补 T7 新列说明；taskpackages 标准内容节新增 `input-brief-<role>.md` 条（生成时点 / 证据属性 / **快照语义同文重申** / 非权威只出指针） |
| `.claude/references/dev-loop-operating-system.md` | T7 | `:122` friction 枚举扩 **`over-scan`**（OQ1=(a)）+ **生产者触发定义**（R1/F14：交付报告超读段非空 → planner 落一条，summary 引行数与代表文件）+ 与 `scan-miss` 的反向语义 + 维持「纯 advisory、绝不当门」（DP-D4）不破 |
| `.claude/scripts/pod-events-aggregate.mjs` | T7 | 新增 `aggregateActors`：**只增 `actors` 键**（既有 `tokens`/`escalations`/`friction` 键不破），per-actor `dispatches`/`withDurationMs`/`durationMsMissing`（**缺值如实计数、不补 0**）/ 挂钟合计/中位 / toolUses 中位 / 长尾（**≥40min，常量 `LONGTAIL_MIN_MS` 与 §1 P1 同源**）计数与 eventId·phase 明细，按挂钟合计倒序 |

## 1b. 落地中的偏差与补位（如实申报，交 landing-verifier 复核）

1. **packet 外补位 3 处（同一理由：不改则 canonical 内部自相矛盾）**——均在 manifest 内文件、无新文件、无新门：
   - `planner.md` TaskPackage 格式示例块（`:~188`）加一行 `"crossEnd": { "counterparts": [], "exempt": "" }`。T4 只列了 Step 1/7/8/9 与 `:109`，但第 6 条要求 Draft 骨架必在，而 planner 的格式示例是起草侧唯一模板源；不补则 planner 起草时看不到形状（与 mutationEvidence 骨架同病灶）。
   - `planner.md` `[JOURNAL-APPEND]` 行（`:66`）friction 枚举同步加 `over-scan` + 标「闭集 SSOT = operating-system §friction，本行只投影不另立」+ 生产者义务。**T7 生产者定义的执行主体正是 planner**，def 内仍列四型会让 over-scan 永无产出者（= R1/F14 判的「死 canon」在 def 面的复现）。实测 `journal-append.mjs:99/162` 对 `--friction-type` 取自由串、**无闭集校验**，故新值可直接落账、不破机械面。
   - `dev-loop.md` §2 表 `pod-events-aggregate` 行补 T7 新列说明（同文件同类投影一致性）。
2. **形式差异 1 处**：T4「Step 1 intake 加跨端判断」落为**新步 `1d`**（Step 1 已有 ①②子块与 1b/1c 平行步，塞进步内会糊掉两条既有子结构）；语义与 PROPOSAL 逐要素一致。
3. **`--regenerated-because` 为本批新 flag**：T4/R2-F6 要求「重跑事由落制品自身」，实现只能在生成器侧，故 `brief-inputs.mjs` 新增该可选 flag（写 brief 头部一行）。未扩 manifest、未加新事实源。
4. **先例封顶的 merged 判定源在实测中有两种落点**：`supervisor-verdict.json` 的 verdict 位置存在 `verdict` / `decision.verdict` / `result.verdict` 三形态（脚本三取，全缺即判未 merged，fail-closed 少收先例不误扩阅读面）。
5. **已知过渡态（F4 落地实况，非新缺陷）**：本批 landing 后裸跑承重闸 = **RED 3 项**（`crossEnd` / 两份 brief），因 `current/` 指向已 merge 的 `260829-schedule-reminder`（无新字段）。当下无在飞 pod，故无实际阻塞；**补救按 T3「过渡注记」执行** = 若下个包在飞，planner 补跑 `brief-inputs.mjs` 双份 + 补填 `crossEnd` 一次即转绿。不设过渡豁免（PROPOSAL 已裁）。
6. **未产出任何真实 brief 文件**：全程只用 `--dry-run`（V2）与仓外 temp fixture（V3），故 `.claude/taskpackages/` 下无新增文件——留给首个真包按 Step 7 生成。

## 2. 验证集执行结果（V1–V5 landing 门全绿；V6 交日落复盘）

| 验证 | 结果 | 实测摘录 |
|---|---|---|
| **V1** 脚本自检 | **GREEN** | `--self-check` → exit 0，**19/19** passed（含 fixture 非空先行断言；四路形状各臂；`isWireRef` 三例含「异文件同节必判 false」= R2-F1 误计实例；fail-loud 反例点名 `NoSuchSymbol`；三判定源抽样；`current/` 排除臂） |
| **V2** 历史包 dry-run | **GREEN** | `--change-id 260907-g-relay-fix --role acceptance-author --dry-run` → stdout 含 `interface-contract.md` 两条锚点节行（`§1.5` → 「1.5 网关的职责…」:138、`§三` → :205）+ ④ 空表模板；并**实际触发 wire WARN**（该包 crossEnd 不在场，正是本批要抓的形态）与 3 条裸路径 ADV；`git status --short .claude/taskpackages/260907-g-relay-fix/` → **空**（dry-run 不落盘） |
| **V3** 闸负正两态 | **GREEN** | 负①（缺 `crossEnd`）exit 1 且点名 `crossEnd(骨架必在…)` + 两 brief；负②（`exempt:"—"` 1 字）exit 1 点名 `exempt must be >= 12 chars (got 1)`；正态（counterparts 含 `firmware/main/mcp_server.cc#McpServer` + 两份 brief + 全部既有必填）→ **exit 0**。fixture 落 `C:/Users/15802/AppData/Local/Temp/kidpal-v3-fixture`（**仓外**，未污染 `.claude/taskpackages/` census），用后 `rm -rf` 并 `ls` 断言不存在 ✓ |
| **V4** 聚合新列 | **GREEN** | `--change-id 260829-story-p3` → `actors` 键在场；acceptance-author `toolUsesMedian = 18.5`（原值 113/8/18/19 逐条对得上，113-tools 派发在场）、builder `69.3min/109tools` 长尾 1 条；既有 `tokens`/`escalations`/`friction` 三键仍在（向后兼容）✓ |
| **V5** 投影 grep 六臂 | **GREEN** | ① `六条前置清单` = 1、`五条前置清单` in DEV-GOVERNANCE = **0**；② 内容锚 `input-brief` = 1、`crossEnd` = 1；③ `.claude/agents/` 对 `五条前置清单\|五条中的` 命中 = **0**；④ 四 def 含 `input-brief` = **4/4**（计数 4/1/3/2）；⑤ operating-system 含 `degraded\|over-scan` = 1；⑥ dev-loop.md 含 `input-brief` = 2（表行 + 内容节） |
| **V6** pilot 回看 | **不在本门** | 交日落复盘；基线已在 FROZEN 文本钉死（acceptance 52.0–61.2min、builder 上限 109.4min/226tools、integrator 71.9min/248tools；守护指标口径 = 冻结后再派 story-p3 = 3 次、fixer 13 包共 3 次） |

## 3. 契约与强制层零触碰自证

`git status` 全量 diff 面 = 11 个 manifest 文件（含 1 新增脚本）+ 治理簿记（`changes/INDEX.md`、`changes/.changes-index-state.json`、本批 PROPOSAL/评审件）+ 会话前既存的两项（`.codebase-memory/*` 工具快照、`.claude/skills/show-me/` 未纳管目录，**均非本批产物、不随本批 commit 混入**）。**`docs/contracts/**` 零改动；`.claude/scope-map.json`、`*.settings.json`、`.claude/verify-gates.json`、`hooks/**` 零改动；四个 agent def 的 frontmatter（`tools`/`name`/`model`/write scope）零改动**——D-2 landing 纪律注记遵守，未触自改递归面。

## 4. 遗留与另登（FROZEN 文本 §6 在册，此处登记执行状态）

- 待另登 ARQ ×3（本批不落，逐条按 `gov-arq` 快路径登记）：① `crossEnd` 消费方②③档（只读参考树路径语义 / 黑盒 live oracle 锚）；② `pre-merge-escalation-gate.mjs:83-84` gov GREEN 机锚裸 `GREEN` 分支向宽（R1 范围外观察 + R3/R4 两轮复述，处置 = 另登已判恰当）；③ 既存的 dispatch-stamp 遥测补全（含 91 事件里 44 条无 token 精确值的实测规模）。
- 交日落复盘：V6 pilot 回看（前 2–3 包，凭 `actors` 新列对比 §1 P1 基线）；R4 残余观察 3（Step 9 重跑触发词未含「crossEnd 迟填」分支）随该轮回看一并裁。

## 5. 核验后更正与残余（landing-verifier verdict = **GREEN、无 RED**，2026-09-09）

独立核验者 `landing-verdict.md` 结论：结构门 `ok:true`、11/11 语义保真（承重替换句 byte 级命中、`isWireRef` 绑文件名经独立探针四例复核）、敏感面零越界（四 def frontmatter 前 12 行逐字节相同 + 无 `tools:`/`model:` 增删）、§1b 三处补位**判为冻结 packet 的必然投影、接受、无需回退无需补报**、六项破坏性探针无一空转；对主线程的 `citationAnchorAdvisory` 归因**独立复核成立**（14 条 `file-not-exist` 逐条实查文件与引文俱在 = 工具短式解析局限）。

主线程自报 vs 其独立复跑：**唯二不符如下，均已就地处理**——

| # | 发现 | 处置 |
|---|---|---|
| **C1** | summary 称新脚本「约 340 行」，实测 474 行 | 已更正（§1 表内）。自报不实，功能无差 |
| **C2** | `--self-check` 的「②③档越界」臂含**恒真合取** `advisories.length + advisoriesGlobal.length >= 0`，只验了「不硬红」半边、漏验「ADV 实发」 | 已修：改为断言 ADV 文本实发。**修后首跑即 RED（18/19）→ 反证原臂确在空转，且暴露 `renderBrief` 返回值未含全局 ADV 的真缺陷**；再把全局 ADV 并入返回值后 19/19 GREEN。这是本批「真空绿规约」在自家脚本上的一次现场印证，随 §6/retro 上交 |
| **D1** | **实质设计级发现（非 landing 失真）**：T2③ 先例路的 `isMerged()` 只认出 4/19 merged 包，git 实证 ≥8 包真经 merge-driver 合并（schedule-p2 / home-p1 / session-logic / host-test-infra / voice-roundtrip / device-ws-switch / gateway-directionB / g-relay-fix）。根因确定：本仓主合并形态 = supervisor verdict **ESCALATE** + 人批事件 `eventType=human-decision`（`payload.decision="approve-escalated-merge"`），既非 APPROVE、eventType 也不含 "merge" → **两个冻结判定源皆失**，brief 给的是陈旧先例 | **本批不回改**（判定源文本由 FROZEN packet 钉死，改即偏离冻结；且 fail-closed 方向 = 少读先例、不放松任何门，危害是「取最新先例」的设计目的未达而非失真）。→ **另登 ARQ-01**，属「已执行测出的治理改进项」走 gov-arq 快路径；修法方向已明：第三判定源 = `human-decision` 事件（decision 含 approve-escalated-merge）∨ git  merge-commit 存在性（`git log --merges` 命中该包落点），并配 `--self-check` 正控（decoy merged 包须入选） |
| **D2** | `exempt ≥12 字符` 地板的**适用面**比 packet 措辞略严（packet 写「作不适用分支时」，实现对所有非空 exempt 生效） | 向严方向、接受；措辞对齐留后续件（不回改冻结文） |
| **D3** | `isWireRef` 用 `startsWith`，故 `#§1.5abc` 亦判 true | advisory-only、无门效应；并入 ARQ-01 同批或后续件收口 |
| **D4** | 冻结 PROPOSAL 内的**行锚**在 landing 后位移（`planner.md:109→110`、`acceptance-author.md:46→47`、`builder.md:46→47`、`dev-loop.md:229→231`） | **不回改冻结文本**（落地后不可回改）。教训回流：后续治理件引用自家 canonical 时应改**短语锚**（引文 + 定位谓词）而非行号——正是本批对 T2② 行号快照语义同一条纪律的作者版复现，随 retro 上交 |

**边界声明**：合并门与前置升级闸非本 summary / 非 verdict 权限；本批按 Track B 第 6（retro 趁热）→ 第 7 步（前置升级闸放行后自动落本地 main，remote push 仍 human）序列继续。
