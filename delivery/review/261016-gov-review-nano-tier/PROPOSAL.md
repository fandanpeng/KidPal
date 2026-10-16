# PROPOSAL · 261016-gov-review-nano-tier

> **状态：DRAFT v1.0 · 2026-10-16 · human 已裁全部 OQ（OQ1–OQ7，2026-10-16 AskUserQuestion 留痕），定稿待主线程标 FROZEN**
> **承载 ARQ**：无在册 ARQ 直承（human 2026-10-16 两轮口述直接发起，原话逐字在主线程派发记录）。同根先例 = 2026-10-14 loop-retro T1「包元数据/冻结件形状税第四波」（`delivery/retro/2026-10-14-loop-retro.md:16-20`；其关联条目 ARQ-260907-taskpackage-format-expectations-frontload 治的是起包格式期望前置，与本批「整档减重」不同根，不回写该行）。
> **review 轨**：Track B 主路径（机器面恒 Track B，**非**瘦制品轻路径——制品四件 + retro 照常，不误套轻路径 retro 豁免；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节、`AGENTS.md`「治理路由」节、`.claude/references/gov-change.md`「轻路径判据」节）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T7）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（触 G3 承重闸脚本 + 立全会话新档位机制 + G1 与自改递归条件触面，向严取 medium 不取 low-medium）。
> **review profile**：G2+G3（references / agents / commands / skills / scripts / verify-gates.json）+ G1（AGENTS.md、docs/DEV-GOVERNANCE.md，OQ6=(a) human 已裁显式升级）+ architect.md 自改递归触面（OQ1=(a) human 已裁明示批准）。全部 OQ 已于 2026-10-16 裁毕：3 个条件锚转必选锚（architect.md / AGENTS.md / docs/DEV-GOVERNANCE.md），1 个条件锚删除（closeout-check.mjs，OQ4=(b)）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/agents/architect.md
.claude/agents/fixer.md
.claude/agents/planner.md
.claude/commands/triage-change.md
.claude/references/architect-operating-model.md
.claude/references/change-triage.md
.claude/references/dev-loop.md
.claude/references/task-intake-and-slicing.md
.claude/scripts/firmware-build-check.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/skills/kidpal-pod-launch-prep/SKILL.md
.claude/verify-gates.json
AGENTS.md
docs/DEV-GOVERNANCE.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：改动独立、爆炸半径小的实现面微调（典型形态 = 单个 UI 页面元素调整、单个 threshold 常量修改）走新立「档 0 nano」——人工外部发现、triage 落盘意图定义、fixer 直改、architect 事后白盒复核当验收权威、既有测试套件零新增红防回归、human 视觉终裁，使这类改动的流程成本与其体量匹配。

**现状差距**：现行最轻档 = ②单包 Lite，仍要完整跑 intake 仪式（input-brief、crossEnd、1g 预评）+ acceptance-author 冻结验收 + 红基线 + mutation 取证。成本实证（起草期 2026-10-16 现跑复核，详见 T1 证据段）：三个近活 Lite 包子 agent 派发挂钟合计 22 / 54 / 100 分钟，且不含 planner 主线程编排时间；其中 auxbar 包实质改动只是 4 个图标换资产 + 日期全大写，却产出 7 条冻结验收（3 条 kind:structural，mutation 语义本就不适用）+ 双通道红基线 + 4 组 mutation 取证。human 原话：「这个收益跟产出完全就不匹配」。

**非目标（本批不做）**：不动 supervisor 合并门 / 前置升级闸 / merge-driver 合并链 / scope-map 写权边界 / 契约冻结防线 / pod-events 与 INDEX 簿记；nano 不豁免 pod-retro 与 closeout 两 blocking 制品；不承接真机轮硬门成文（ARQ-261001 在册 deferred）；不立独立队列、新目录或新脚本（机械牙齿全走扩承重闸，见 §4 OQ2）。

## 2. 问题、根因与改法（T1…T7）

**【硬项 · 起草期实证义务履行声明】** 本批全部行为断言均附起草期实测（命令 + 关键输出 + 日期 2026-10-16，本 worktree gov/kidpal 分支）：成本数字现跑 `pod-events-aggregate.mjs --change-id` 复核（非转引主线程）；脚本行为断言均以 Grep/Read 到源码行号为准；「四档」残留面以 grep census 机械枚举（命令与命中清单见 T4/§3）；基线自检实跑结果见 §5。无「未实测」断言。

### T1 · triage 判档四档变五档，立「档 0 nano」

**问题**：改自有实现面行为的变更里存在一类「意图完全明确、无验收语义净增」的微改——单个 UI 页面元素布局调整、单个 threshold 常量修改、资产消费点名切换。它们改的是「把现状换成已明确的另一形态」，story 语义不变、无新行为决策，但现行四档里最轻的 ②单包 Lite 仍要求完整 intake 仪式 + 冻结验收链：input-brief 机械投影（对「调个位置」无信息增量）、acceptance-author 派一轮、红基线双通道、mutation 逐条取证。这类变更近三周三连出现（voice 右栏 label 溢出、auxbar 四图标 + 日期大写、tomorrow-row 残线），每个都按 Lite 跑足全程，收益与产出不匹配，且随项目进入小修小改期会越来越高频（human 第二轮裁定④⑤原话：「越往后可能真的就是一些小修小改……Nano 的模式可能会随着项目越往后期，用得越多」）。

**证据**：
- 成本实证（2026-10-16 起草期现跑 `node .claude/scripts/pod-events-aggregate.mjs --change-id <id>`，本 worktree；数值为子 agent 派发挂钟合计，**不含** planner 主线程编排时间）：`261015-voice-rightcol-label-overflow` dispatchesTotal=3、合计 22.2 分钟（builder 9.8 + integrator 9.1 + acceptance-author 3.3）；`261015-p1-auxbar-style-fix` dispatchesTotal=3、合计 53.9 分钟（acceptance-author 31.8 + builder 16.8 + integrator 5.3）；`261013-p1-tomorrow-row-sliver-fix` dispatchesTotal=16、合计 100.5 分钟（acceptance-author 47.6 分 ×5 次 + integrator 26.7 分 ×8 次 + builder 25.4 分 ×2 次 + architect 0.8 分 ×1 次）。
- 形状税活样本（`delivery/taskpackages/261015-p1-auxbar-style-fix/taskpackage.json`，2026-10-16 `wc -l` + `node -e` 实测）：全件 175 行；实质改动 = 4 个图标换资产 + 日期全大写（story 字段 :14）；产出 = acceptance 7 条（:20-40，其中 3 条 `kind:structural` 对象 :24-28、:30-34、:35-39，mutation 语义按 M-4 本就不适用）+ 双通道红基线（redBaselineResult :58）+ mutation 取证 4 组（mutationEvidence.perAcceptance :133-166）+ notes 单字段 3829 字符。
- 同根先例：`delivery/retro/2026-10-14-loop-retro.md:16-20` T1「包元数据/冻结件形状税第四波——闭环建议至今未落，本波 6 包同型红」。
- 半径阈值实测依据（主线程 2026-10-16 现跑 `trace_path` inbound depth 3，project `C-AI-KidPal`；**本起草会话工具面不含 MCP 图工具，未能亲核复跑——按起草期实证义务标「未亲核」，数值以主线程实测为准，建议判官定稿前经 MCP 复跑一次**）：tomorrow-row `p1_draw_card` inbound = 1；auxbar `p1_draw_aux_time` / `p1_draw_aux_temp` / `p1_draw_aux_hum` / `p1_draw_aux_date` 各 = 1、`p1_blit_art` = 4；voice `LcdDisplay::SetupUI` = 3（调用方均同文件子类构造函数）；topbar `kidpal_story_text_parse` = 0、`rn_render_kidpal_page_locked` = 17。分布 = 三个 nano 意图包全部 ≤4 vs 中枢调度符号 17，中间一个数量级断层——阈值钉 5 恰落断层空档；topbar 被拒（中枢调度符号的小修走 lite）是判据生效的设计意图，不是误伤。
- 现行四档表与 Lite 判据锚：`.claude/references/change-triage.md:25-28`（判据表）、`:104-110`（lite 快路径）；`.claude/references/task-intake-and-slicing.md:212-226`（Use a Lite TaskPackage when…）。

**根因**：档位设计按「包大小与风险」切轴（Lite/Full），没有按「验收语义是否净增」切轴。对无验收语义净增的微改，冻结验收链（写新验收件 + 红基线 + mutation）承载的不是新行为的判分，而是用全套仪式重复证明一件 human 一句话已说清的事——成本大头在 planner 编排仪式与证据链，不在 builder/fixer 干活。

**改法**：在 `.claude/references/change-triage.md` §2 判据表加「档 0 nano」行并立其判据节，四档表述全文件同步为五档（:3、:14、:45、:54、:82 及 §2 表）。档 0 判据（全部满足 + 排除清单零命中，存疑向严升档）：

- **改动独立**：单一目标符号 / 单一 UI 页面元素 / 单一常量簇，改动意图一句话说清；
- **爆炸半径小（机器量，不靠人指定、不按代码行数）**：triage 时 architect 用 CodebaseMemory 图工具 `trace_path(direction="inbound")` 量每个将触符号的调用方集，**阈值钉死 = 每将触符号 inbound 调用方 ≤ 5 且同属一个 owning 文件/紧耦合簇**（human 2026-10-16 实测钉死 OQ3，四包实测表与断层分布见证据段）；预估 diff ≤2 个实现面文件作辅助参照；
- **无验收语义净增**：不改 story 语义、不引入新行为决策、不触新契约语义；`sourceType=proposal` 的包不得判 nano（proposal 载荷 = 决策集，decisionCoverage 不豁免，与 task-intake-and-slicing.md:253 现行规则一致）；
- **排除清单（命中任一即升，不判 nano）**：触 wire/契约面、状态机、跨端、安全轴、迁移/删数；目标区域无既有齿覆盖且 human 不愿目检兜底；意图需要新行为验收语义承载（→ 升 Lite）。
- **判档双层形状**：判档预估 = `trace_path` inbound（triage 时）；收口实测 = `detect_changes` 对 actual diff 算实际爆炸半径，结果记进白盒复核件（advisory，见 T3）。
- **判档判注义务（比照 §7 lite 判注）**：architect 判 nano 时 rationale 须含一句「host 齿可闭环」确认（nano 的 suiteGate 回归网 = 既有 host 套件，可在 host_app target 闭环；与 lite 快路径判注义务同构，承接 launch/verify 两侧后置——launch 半边见 T4 skill 扩、verify 半边见 T5 defer 扩）。
- **落盘形状**：nano 裁决 → reviewed plan（= 意图定义五件）落 `delivery/taskpackages/<id>/plan.md`（与 ②单包同家，frontmatter `rigor: nano`；记录落点见 §4 OQ7）；意图定义五件 = ① 改动意图一句（改什么、为什么）② 目标符号/文件锚（即 changeManifest 种子，triage architect 给）③ 预期行为差（before/after 一句）④ 目标区域既有齿覆盖态申报（既有验收/单元齿清单，或「无齿 → human 目检兜底」声明）⑤ 爆炸半径预估（trace_path 结果摘要）。
- **越界升档（nano 无扩单通道）**：nano 包 change-manifest 闸红（diff 越出清单）或白盒复核发现出界 → planner 停，回 `/triage-change` 重判档（典型升 Lite），不得原地扩 manifest 续跑——与 Lite 的 architect 扩单通道（fixer.md:49）有意不同，承载 human「diff 越界自动升档」裁定。
- **触点① nano 形态**：human 确认档 0 时一并确认意图定义五件与覆盖态兜底声明；单向守卫（riskTier overrides rigor）对 nano 同样生效（触契约/security/迁移信号 → 升档）。

目标文件：`.claude/references/change-triage.md`（改）

### T2 · rigor 字段增 nano 档字段级定义（验收权威置换形态 + 与 lite 边界）

**问题**：`rigor` 字段现行枚举只有 `lite | full`（task-intake-and-slicing.md:240），nano 档在 TaskPackage 字段层无定义——哪些字段不可省、哪些豁免、验收权威用什么形态承载，均无 SSOT。若由各包自行解释，「nano」会退化成什么都可以往里装的筐（滥用引力，human 点名的风险②）。

**证据**：
- `task-intake-and-slicing.md:240`：「TaskPackage 用 `rigor` 字段标档：`"rigor": "lite" | "full"`（缺省按 full 既有逻辑走）」。
- 验收权威置换的机械基础已存在（起草期 Grep 实证 2026-10-16）：`suiteGate:true` 是 260731 批立的一等豁免标记——`acceptance-coverage-check.mjs:143-144`（suiteGates 并入 exempt 集）、`mutation-check.mjs:141-142` 同义豁免 + `:156`（全豁免集时 `ok:true, skipped:true`）、`mutation-check.mjs:67`（M-4 成文：套件级条目 mutation 证据语义 = 不适用）；SSOT 适用判据 = `dev-loop.md:164`（suiteGate 正面清单首条 = 「既有套件零回归」）。
- Lite 字段级定义面：task-intake-and-slicing.md:240-253（不可省/可省清单、crossEnd、planRef、decisionCoverage 与 rigor 关系）。

**根因**：Lite 定义时就明确「省的是方案/决策映射的文牍，不是 loop 的任何一道闸」（task-intake-and-slicing.md:242）；nano 需要的是再省「冻结验收链」这一层——但验收权威不能悬空，必须置换成等价的另一组承载，而不是删除。

**改法**：在 task-intake-and-slicing.md「rigor 字段与 Lite TaskPackage 字段级定义」节内新增 nano 小节：

- `rigor` 枚举扩为 `"nano" | "lite" | "full"`（缺省按 full 既有逻辑不变）。
- **不可省**（守 dev-loop 铁律与簿记统一）：`changeId` / `rigor:"nano"` / `sourceType`（不得为 proposal）/ `sourceRefs` / `deliveryImpact` / `planningImpact` / `riskTier` / `gateProfile` / `story` / `planRef`（= 意图定义载体，nano 必填）/ `subsystem`·scope / `contractRefs` / `docRefs`（两字段在 taskpackage-required-check.mjs:330 数组非空必填行，nano 不豁免——无契约命中时按既有口径填契约面引用或显式说明）/ `changeManifest`（triage architect 给，fixer 文件级硬边界）/ `crossEnd`（二选一照填，exempt ≥12 字符地板不动）/ `acceptance` = **恰一条 `{ "id": "<包前缀>-A1", "suiteGate": true }`**（语义 = 既有测试套件零新增红；复用 260731 一等豁免机制，acceptance-coverage / mutation 两闸零改动即天然闭环——实测引上行证据）/ `exitCriteria`（含白盒复核条目；视觉门不成文，OQ5=(b) 见 T3）/ closeout 两 blocking 制品（docs-delta.md、review-packet.md）/ pod-retro.md（human 已裁①不豁免）/ pod-events 全程簿记 / supervisor 合并门 + 前置升级闸全链；**总括：未在下方豁免清单显式列名的闸必填字段（taskpackage-required-check.mjs:326 与 :330 两必填行）一律照 full 判据，不随 nano 豁免**。
- **豁免**（逐条理由）：`redBaselineCommand`/`redBaselineResult`（验收权威置换为白盒复核 + suiteGate，无新冻结件可跑红基线）；`acceptanceTestsFile`（同上，无新验收文件）；`mutationRequired`/`mutationEvidence`（suiteGate 条目 M-4 语义不适用，闸既有豁免行为承接）；`input-brief`（无 acceptance-author/builder 消费方，fixer 输入 = plan.md 意图定义）；`decisionCoverage`（nano 判据排除 proposal 来源，非 proposal 本就可选，见 task-intake-and-slicing.md:253）；planner Step 1g / 7 / 8 验收件部分 / 8b / 9（编排面豁免理由见 T3）。
- **与 lite 快路径关系**：nano 与 lite 同享快路径两侧——verify 半边 = firmware-build scoped 相位后置（verify-gates.json defer 机制扩 whenRigor，见 T5）；launch 半边 = `kidpal-pod-launch-prep` SKILL 3b 触发条件扩 nano（见 T4；处置 = 扩 skill 对称处理，理由：nano 比 lite 更轻，轻档不享环境后置而重一档的反享 = 无理由倒挂）。lite 快路径其余语义不动（261015-gov-review-lite-fast-path 刚落，本批不回改其 T1–T4 任何裁定）。
- **与 lite 的边界判据（防两档死态互推）**：lite = 有行为决策要钉（新验收语义、story 命中或 bug 根因要冻结齿）但单包一轮可收；nano = 无验收语义净增、意图完全明确的形态替换。判不了 → 向严 lite。判档机判（半径量 + 排除清单）+ 越界升档 + loop-retro 校准三道防滥用（human 裁定的风险②防线）。
- **豁免审计承接**：nano 包 acceptance 恒含 suiteGate → acceptance-coverage 输出 suiteGates 非空 → 既有 D-1 审计通道（review-packet 枚举 + human glance 一句确认，dev-loop.md:168）承接，不新增机械通道。

目标文件：`.claude/references/task-intake-and-slicing.md`（改）

### T3 · dev-loop 编排面 nano 分支（planner 豁免清单 + architect 白盒复核档 + fixer 输入形状）

**问题**：planner 现行 Process 对所有包一刀切跑 Step 1g→12 全程；fixer 的输入契约只认 verifier ErrorEnvelope（fixer.md:20-25「Inputs」）；architect 现行职责里没有「对 actual diff 做事后复核」这档（architect.md:26-32 现行 advisory 档 = 10b 疑难分析 + 1g 技术预评，都是事前/事中意见，不是事后复核）。nano 档的验收权威 = architect 事后白盒复核（human 已裁②），编排面无此派发点。

**证据**：
- planner.md Step 1g（:49，「TaskPackage 落盘后 / brief 生成前」派 architect 预评三件）、Step 7 brief 生成（:62）、Step 8 finalize 硬前置（:63）、Step 8b 验收方向核（:64）、Step 9 派 builder（:65）、Step 10b ARCH-ADVISORY 疑难分析档（:80-84，效力声明原文在 :84）。
- fixer.md:20-25（Inputs = ErrorEnvelope + 根因假设 + TaskPackage）、fixer.md:49（changeManifest 文件级边界 + 扩单唯一通道）。
- architect 无 `delivery/taskpackages/**` 写权（`.claude/scope-map.json:163-174` architect allow 集；`:177` deny architect.md 自身）——复核结论落盘须由 planner 代落，先例 = planner.md:33 派发书三条硬约定②「无权即在派单时预指派代落人」。
- human 视觉门先例（live-manual 停门等 human）：`delivery/taskpackages/261015-p1-auxbar-style-fix/taskpackage.json:36-39`（AB-A7「host 面转绿后停该门等 human，不自行烧录」）。

**根因**：验收权威置换后，编排面需要三个新形状——豁免清单成文（防逐包自由裁量）、复核派发点（验收权威的生产位）、fixer 的非 ErrorEnvelope 输入分支（nano 的活是「外部发现、人工输入」，不是 verifier 红轮驱动）。

**改法**：

- **planner.md**：Step 3 判档认 `rigor: nano`（plan.md frontmatter 裁决消费，与 lite/full 同通道）；新增 nano 分支段，豁免/保留清单成文——豁免：Step 1g（triage 判档 architect 已做影响面 + 半径评估，changeManifest 已随 plan.md 落，同源劳动不重复）、Step 7 brief-inputs（无 acceptance-author/builder 消费方）、Step 8 验收件/红基线 finalize 项（无冻结验收链；TaskPackage 落盘与 current/ 投影纪律照跑）、Step 8b（无冻结验收可核，方向核对由白盒复核承接）、Step 9 builder（执行者 = fixer，human 原话「它就像是 Fixer……处理出来之后再去调 Fixer 把它解决掉」）；保留不动：Step 1 intake（含 1c scope-map 核对、1d crossEnd）、Step 5 Draft 落盘、Step 6 scope-map 预置、Step 10 verifier↔fixer 内循环（verify.mjs scoped/final 照常——既有套件零新增红由它承载）、Step 10b、Step 11 integrator（docs-sync + closeout 两制品 + closeout-check）、Step 12 supervisor 闸 + 前置升级闸 + merge-driver、pod-retro（human 已裁①）、pod-events / dispatch-stamp / journal 全程簿记。**另立显式例外承接**：planner.md Step 5 既有句「`acceptance[]` 必须承载行为决策，不允许整组只有结构条目」（planner.md:53）与 nano 的恰一条 suiteGate 整组形态字面冲突——落地时该句同步加 nano 例外括注（「`rigor=nano` 档为显式例外：整组 = 恰一条 suiteGate 条目，语义 = 既有套件零新增红，字段级定义见 task-intake-and-slicing.md nano 小节」），防下一棒按旧句打回合法 nano 包（判官 F5）。
- **新增 Step 10c〔ARCH-ADVISORY 白盒复核档〕**（验收权威生产位）：nano 包 fixer 落补丁且 verify 绿后、Step 11 integrator 前**必派** architect。dispatch prompt 必含：plan.md 意图定义五件 + actual diff 范围（git diff）+ 效力声明原文——「不豁免 verify.mjs 任何闸、不改 TaskPackage 事实；**本复核即 nano 行为验收判分位，PASS 为 Step 11 前置**」。该声明与 10b 档的 advisory 效力（「不构成通过判据」）**有意不同**：10c 是 nano 档验收权威本体（human 已裁②），不是参考意见；名为 ARCH-ADVISORY 仅指派发通道复用（architect 只读 + 回执文本、不落盘，与 1g/10b 同形）。复核三问 = ① 改的是不是意图定义说的那件事 ② 有没有顺手改出界（diff vs changeManifest vs 意图）③ 覆盖态申报是否属实（无齿区域 → 提示 human 目检兜底未履行不得 PASS）；并用 `detect_changes` 对 actual diff 算实际爆炸半径记入回执（判档预估 vs 收口实测双层形状收口）。结论三值：PASS / 出界（→ 回 `/triage-change` 重判档）/ 非所说事（→ 退 fixer）。**落痕载体 = integrator 在 `review-packet.md` 增「白盒复核」节**（对意图定义逐条核 diff + PASS/偏差结论；OQ4=(b) human 已裁 2026-10-16，不落独立件、closeout-check 不加牙）；10c 判分位语义不变——PASS 仍为 Step 11 前置，变的只是落痕载体。
- **human 视觉终裁**（触 UI 的 nano 包）：**不成文**——不设 exitCriteria 视觉门条目、不设 supervisor 闸前 AskUserQuestion 确认，由既有真机轮/看屏轮惯例兜（human 2026-10-16 裁 OQ5=(b)，理由 = nano 本就生于看屏轮打回场景，人目检是在场前提，再立闸前确认是把已在场的事重收一道税）；supervisor 不恒 ESCALATE 维持现状。human 已裁②末句「视觉终裁仍归 human 持板」由该惯例承载，不另立文面。
- **fixer.md**：Inputs 加 nano 分支（输入 = plan.md 意图定义 + changeManifest + planner 根因假设，无 ErrorEnvelope）；Process 加一句 nano 形状（最小补丁 + 意图对照自查）；Red Flags / 边界节加「nano 无扩单通道——补丁需越 changeManifest → 停手交回 planner 回 triage 升档，不派 architect 扩单」（与 Lite 的 fixer.md:49 扩单通道有意差异，防滥用引力）。
- **dev-loop.md**：§0 progressive rigor 段「三档边界」提法同步为五档（:26，档 0 nano 一句 + 指向 task-intake-and-slicing.md）；「变更制品」节加 nano 制品注记（review-packet.md 增「白盒复核」节 = nano 验收权威落痕位 + nano 豁免/保留清单指针，不复制字段表）。

目标文件：`.claude/agents/planner.md`（改） · `.claude/agents/fixer.md`（改） · `.claude/references/dev-loop.md`（改）

### T4 · 入口、镜像与牵连面文面同步（四档提法残留清扫 + launch 面快路径扩 nano）

**问题**：「四档」提法散在入口 command 与 operating-model 镜像段；档位 SSOT 改了而镜像不同步，就是下一批的撞车源（本仓「别把看到的一处当全部」教训在册：拟 canonical 前先全写面 fan-out）。

**证据**：复扫 census（2026-10-16 起草期实跑 `grep -rn "四档\|判四\|三档边界\|Full / Lite / 直接修" --include="*.md" .claude docs AGENTS.md CLAUDE.md`，豁免 docs/debt、docs/showme、docs/archive、delivery 历史制品）：canonical 面命中 = `.claude/commands/triage-change.md:2`（description「判四档（③拆包 / ②单包Full / ②单包Lite / ①直接修）」）、`:13`、`:22`、`:26`（「呈现 architect 的四档裁决」——判官 F3 补漏，R1 修订复跑 grep 复证 4 处命中）；`.claude/references/architect-operating-model.md:67`（「返回四档裁决」）；`.claude/references/change-triage.md`（T1 处理）；`.claude/references/dev-loop.md:26`（T3 处理）；`.claude/agents/architect.md:26`（T6 处理）；`AGENTS.md:30` 与 `docs/DEV-GOVERNANCE.md:128`、`:235`（T7 处理）。牵连面另一件（判官 F4 指出，R1 修订补）：`.claude/skills/kidpal-pod-launch-prep/SKILL.md` 3b lite 快路径（SKILL.md:77-84，触发 = 包 `rigor=lite` 且 triage 未声明「快路径不适用」；步骤 4 头注「lite 包新树：整步骤后置真机轮前」同只认 lite）——nano 若不扩则 launch 面环境后置不生效，与 verify 半边（T5 defer 扩）形成无理由不对称。需求面「三档」语境（requirement-change.md / story-author / requirement-reviewer / write-story 等）是另一根轴（0/1/2 档判据），词面相似、语义不相干，不在本批面内（复扫模式完备性论证见 §3）。

**根因**：档位数字被多处镜像引用而非单源——这是既成结构，本批不做单源化重构，只做同步清扫（与 261015 批 lite 快路径同口径：改判据先改 SSOT 再同步镜像）。

**改法**：`.claude/commands/triage-change.md` description 与 :13、:22、:26 的「四档」提法 → 五档，档位枚举加「档 0 nano」；`.claude/references/architect-operating-model.md:67`「返回四档裁决（③拆包 / ②单包Full / ②单包Lite / ①直接修）」→ 五档枚举同步。command description 改动后复跑 route-regression-check（§5 验证集第 7 条）。牵连面扩一件：`.claude/skills/kidpal-pod-launch-prep/SKILL.md` 3b lite 快路径触发条件扩为 `rigor ∈ {lite, nano}`（标题与触发句同步），步骤 4 头注「lite 包新树」同步为「lite/nano 包新树」——处置选择 = **扩 skill 对称处理**（判官 F4 二选一），理由：nano 比 lite 更轻，轻档不享 launch 面环境后置而重一档的反享 = 无理由倒挂；triage 判注义务在 T1 已比照 §7 立 nano 版（「host 齿可闭环」确认）。

目标文件：`.claude/commands/triage-change.md`（改） · `.claude/references/architect-operating-model.md`（改） · `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改）

### T5 · G3 机械面：required-check nano profile + firmware-build defer 扩（扩承重闸，克制新立脚本）

**问题**：nano 包过不了现行机械闸的第一关——`taskpackage-required-check.mjs:326` 无条件必填 `redBaselineCommand` / `redBaselineResult`，finalize 态另要 input-brief 在场（:515）与 mutationEvidence 骨架（:513 注）；firmware-build 的 scoped 后置（lite 快路径）不认 nano。若不加机械面，nano 档只能纸面存在。

**证据**（2026-10-16 起草期 Grep/Read 实证）：
- `.claude/scripts/taskpackage-required-check.mjs:326`（必填字段行，含 redBaselineCommand/Result）、`:334-350`（acceptance 对象条目 kind/suiteGate 形状校验与互斥判——nano 的单条 suiteGate 形态校验可挂在同族）、`:515`（finalize 态 input-brief 在场）。该脚本现行不校验 rigor 取值闭集（Grep 无 rigor 校验逻辑），新增 `"nano"` 枚举值不撞既有校验。
- defer 机制：`.claude/hooks/verify.mjs:116-128`（`shouldDeferGate` 读 verify-gates.json 的 `defer.whenRigor`/`phases`）、`:249-261`（defer 命中时注入 `VERIFY_GATE_DEFER=1` + `VERIFY_GATE_DEFER_REASON`，reason 文案含真实 rigor 值）；`.claude/verify-gates.json` firmware-build 条目 `defer: {"whenRigor":["lite"],"phases":["scoped"]}`。
- reason 文案断点：`.claude/scripts/firmware-build-check.mjs:64-66`（deferred 臂 reason 硬编码 `'rigor=lite 包 scoped 相位后置…'`）、`:122`（只读 `VERIFY_GATE_DEFER`、不读 `VERIFY_GATE_DEFER_REASON`）——whenRigor 加 nano 后，nano 包的证据文案会假称 lite，必须顺手修（扩 env 消费 + self-check 断言同步）。
- 基线（2026-10-16 实跑）：`taskpackage-required-check.mjs --self-check` = 18 pass / 0 fail（+ changeid-prefix 4/0）；`verify.mjs --self-check` = 16/0；`firmware-build-check.mjs --self-check` 绿；`route-regression-check.mjs` = 52 ok / 0 fail。

**根因**：机械闸的字段契约是按「验收链必有」的世界观写的；nano 是合法的字段形状新档，闸要认档而不是被绕开（绕开 = nano 包不进 verify.mjs，等于废掉「既有套件零新增红」这道唯一的机器防回归）。

**改法**（机械牙齿落点 OQ2=(a) human 已裁 2026-10-16：全部扩承重闸、零新立脚本）：

- `taskpackage-required-check.mjs` 加 `rigor==='nano'` 分支：免 :326 行 `redBaselineCommand`/`redBaselineResult` 必填、**免 :330 行 `acceptanceTestsFile` 数组非空必填**（判官 F1：该字段与 contractRefs/docRefs 同列 :330 无条件必填行，漏此豁免则合法 nano 包必在 :330 红）、免 mutationEvidence 骨架、免 finalize 态 input-brief 在场；新增 nano 专属校验 = `planRef` 非空 ∧ `changeManifest` 非空 ∧ `acceptance` 恰 1 条且为 `{id, suiteGate:true}` 纯形态（挂 :334-350 同族形状校验）∧ `sourceType !== 'proposal'`；nano 包不依赖 acceptanceTestsFile 判 finalize（直接按 nano profile 全量校验）。`--self-check` 加 nano 夹具 ≥3 条（合法 nano 包过 / 缺 planRef 红 / acceptance 非单条 suiteGate 红——反转必红，真空绿），自检名含 nano 字样的输出进 §5 验证集。
- `verify-gates.json`：firmware-build 条目 `defer.whenRigor` 加 `"nano"`（scoped 后置、final 恒跑，与 lite 同待遇；机制 verify.mjs 零改动）。**落地注记**：同文件 `_doc` 两处「rigor=lite 包…后置」措辞（:36 头注数组内、:59 firmware-build 条目 `_doc` 字段，2026-10-16 grep 复证）顺手同步为 lite/nano 双档表述（判官 F7）——只改文档措辞，不改任何判定语义。
- `firmware-build-check.mjs`：deferred 臂 reason 改为优先消费 `VERIFY_GATE_DEFER_REASON` env（verify.mjs 已注入含真实 rigor 的文案），env 缺席回退现行硬编码文案；self-check 的 `deferred-arm-executor-skipped` 断言同步加 env 消费分支（防证据文案假称 lite）。

目标文件：`.claude/scripts/taskpackage-required-check.mjs`（改） · `.claude/verify-gates.json`（改） · `.claude/scripts/firmware-build-check.mjs`（改）

### T6 · architect 白盒复核职责立编（OQ1=(a) human 已裁；自改递归面已获明示批准）

**问题**：architect 事后白盒复核是新编制职责——architect 现行不做实现 diff 复核（architect.md 的 When to Use / 职责段无此档；Boundaries 明载不替 builder/fixer/integrator 工作）。只改 planner.md 派发契约而不改 architect.md，职责描述即与实践漂移（1g/10b 档都是 planner.md 定义 + architect.md 镜像段双写，先例在册）。

**证据**：`.claude/agents/architect.md:26`（triage 段「判四档」镜像）、`:31-32`（10b 疑难分析档 + 1g 技术预评档的职责镜像段——双写先例本体）；自改递归边界 = `docs/DEV-GOVERNANCE.md:212`（「修改自身定义、权限边界、工具、write scope……必须升级为 human-approved governance change；不得自批」）+ `.claude/scope-map.json:177`（architect deny 含自身 def 文件）。

**根因**：职责的机读路由信号（description / When to Use）住在 agent def；派发契约住 planner.md。两处缺一，orchestrator 路由与角色自我认知必有一个过期。

**改法**（OQ1=(a) human 已裁 2026-10-16，自改递归面获明示批准）：architect.md 的 When to Use 加一档「dev-loop nano 档事后白盒复核（planner 按 Step 10c 派：fixer 落补丁后拿 plan.md 意图定义对 actual diff 复核三问 + detect_changes 收口半径记录；只读 + 回执文本、不落盘、不改任何文件；**效力声明引 T3 Step 10c 声明原文、逐字一致**——「不豁免 verify.mjs 任何闸、不改 TaskPackage 事实；本复核即 nano 行为验收判分位，PASS 为 Step 11 前置」，两落点单源）」，职责段同步镜像，:26 triage 段「判四档」→「判五档」同步。**禁止回植 1g/10b 的「不构成通过判据」尾句**——10c 是 nano 档验收权威本体（human 已裁②），自我否定的声明会把 R1 F2 的病从镜像段原样带回（R2-F1）。Boundaries / Write Scope 不动（复核只读 + planner 代落，写域零扩张）。

目标文件：`.claude/agents/architect.md`（改）

### T7 · G1 档数同步（OQ6=(a) human 已裁；G1 触面显式升级已批）

**问题**：两处 G1 指令根引用档数——`AGENTS.md:30`「实现面变更：走 `/triage-change` 四档分流」、`docs/DEV-GOVERNANCE.md:128`「比例化分档（拆包 / Full / Lite / 直接修）」与 `:235` 入口速查表「比例化四档」。SSOT 五档化后 G1 镜像不同步 = 指令根与流程 SSOT 常驻矛盾。G1 触面按模板纪律必须显式升级，不得默携带。

**证据**：`AGENTS.md:30`；`docs/DEV-GOVERNANCE.md:128`（§4 Dev Loop OS 怎么做 ☐）、`:235`（§7 入口速查表「自有实现面」行）。复扫 census 见 T4 证据段（同一 grep 命令输出）。

**根因**：同 T4——档数被多处镜像引用；G1 因其指令根地位单列显式升级，不走一般镜像同步。

**改法**（OQ6=(a) human 已裁 2026-10-16，G1 触面显式升级已批）：AGENTS.md:30「四档分流」→「五档分流」；DEV-GOVERNANCE.md:128 分档枚举加 nano、:235「比例化四档」→「比例化五档」。措辞级同步，不引新语义（五档裁定本身 human 已裁④，本 T 只是 G1 镜像）。

目标文件：`AGENTS.md`（改） · `docs/DEV-GOVERNANCE.md`（改）

**【起草期自检三句】**（落盘前履行，结果进 §5 第 1 条与回执）：① `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261016-gov-review-nano-tier/PROPOSAL.md` 干跑核 anchorUnion 成员名单（逐 token 核注记无夹带路径记号）；② 本批无路径形态变更（无搬迁/改名/路径串改写），`.claude/references/path-form-family.md` 形态族谱表查表义务不适用，声明在此；③ 本 PROPOSAL 全部 file:line 引据均由 Read/Grep 输出直接复制（未长输出目视转录），承重引文（change-triage.md:25-28、planner.md:49、fixer.md:49、taskpackage-required-check.mjs:326 等）落笔前逐条对照源文件行。

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G2 references | `.claude/references/change-triage.md` | 改（T1） | 五档化 + nano 判据节；消费方 = `/triage-change` 主线程 + architect |
| G2 references | `.claude/references/task-intake-and-slicing.md` | 改（T2） | rigor=nano 字段级定义；消费方 = planner intake + 闸作者 |
| G2 references | `.claude/references/dev-loop.md` | 改（T3） | 档数提法 + 制品注记；消费方 = dev-loop 全体 |
| G2 references | `.claude/references/architect-operating-model.md` | 改（T4） | :67 镜像同步；消费方 = architect 路由 |
| G2 agents | `.claude/agents/planner.md` | 改（T3） | nano 分支 + Step 10c；消费方 = planner Pod |
| G2 agents | `.claude/agents/fixer.md` | 改（T3） | nano 输入分支 + 无扩单通道；消费方 = fixer |
| G2 agents | `.claude/agents/architect.md` | 改（T6） | 白盒复核职责；自改递归面，OQ1=(a) human 已裁明示批准 |
| G2 commands | `.claude/commands/triage-change.md` | 改（T4） | description + 步骤文面五档化（:2/:13/:22/:26 四处）；消费方 = 主线程入口 |
| G2 skills | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | 改（T4） | 3b 触发条件 + 步骤 4 头注扩 nano（判官 F4，对称处理）；消费方 = launch 面主线程 |
| G3 scripts | `.claude/scripts/taskpackage-required-check.mjs` | 改（T5） | rigor=nano profile + self-check 夹具；消费方 = verify.mjs 第一闸 + closeout 相位 |
| G3 scripts | `.claude/scripts/firmware-build-check.mjs` | 改（T5） | defer reason env 消费；消费方 = verify.mjs defer 链 |
| G3 config | `.claude/verify-gates.json` | 改（T5） | firmware-build defer.whenRigor 加 nano；消费方 = verify.mjs |
| G1 | `AGENTS.md` | 改（T7） | :30 档数同步；G1 显式升级，OQ6=(a) human 已裁 |
| G1 | `docs/DEV-GOVERNANCE.md` | 改（T7） | :128、:235 档数同步；G1 显式升级，OQ6=(a) human 已裁 |
| 承重未触 | `.claude/scope-map.json`、`.claude/hooks/enforce-scope.mjs`、`.claude/dev-loop.settings.json` | 未触 | 写域零扩张：architect 复核回执 planner 代落（先例 planner.md:33②），fixer 既有 allow 覆盖实现面 |
| 承重未触 | `.claude/hooks/supervisor-verdict-gate.mjs`、`supervisor-gate-check.mjs`、`.claude/scripts/pre-merge-escalation-gate.mjs`、`merge-driver.mjs` | 未触 | 合并链零改动；nano 包合并走同一链 |
| 承重未触 | 契约冻结防线（contracts.md / Bash 重定向恒拦 / per-agent deny） | 未触 | nano 判据排除清单含触契约即升档 |
| 承重未触 | `.claude/scripts/acceptance-coverage-check.mjs`、`.claude/scripts/mutation-check.mjs` | 未触 | suiteGate 既有豁免行为承接 nano 验收形态（:143-144 / :141-142,:156 实证），零改动 |
| 承重未触 | `.claude/scripts/brief-inputs.mjs`、`pod-events-append.mjs`、`pod-events-aggregate.mjs`、`changes-index.mjs` | 未触 | brief 生成步对 nano 豁免（脚本本身不动）；簿记/聚合面无新 eventType（loop-retro 消费面统一，human 已裁①） |
| 历史制品 | `docs/showme/README.md:13`、`docs/debt/**`、`delivery/**` 历史包/PROPOSAL/retro | 未触 | 「四档」时点快照不回改（append-only 纪律） |
| 邻面相干不动 | 需求面「三档」语境（requirement-change.md / story-author / requirement-reviewer / write-story / change-intake.md 引用） | 未触 | 另一根档轴（0/1/2 判据），词面相似语义不相干；复扫不误伤 |

**【硬项 · sweep 透镜义务】** 本批落地的新判定 pattern = ① 「四档→五档」复扫（模式集与命令见下「复扫模式完备性先验」，原始命中清单见 T4 证据段，2026-10-16 实跑）；② suiteGate 豁免行为 census（命令 `grep -n "suiteGate" .claude/scripts/acceptance-coverage-check.mjs .claude/scripts/mutation-check.mjs`，命中 6 行全列于 T2 证据）；③ 成本 census（`pod-events-aggregate.mjs --change-id` × 3 包，输出全列于 T1 证据）。枚举均由机械命令生成，判官可 spot-check。

**【硬项 · 复扫模式完备性先验】** 退役语面 = 「四档」提法。模式集显式枚举：`四档` ×（裸 / backtick 包裹）∪ `判四` ∪ `三档边界` ∪ `Full / Lite / 直接修` ∪ `②单包Lite / ①直接修`；检索命令 `grep -rn "<模式>" --include="*.md" --include="*.json" --include="*.mjs" .claude docs AGENTS.md CLAUDE.md`（2026-10-16 实跑，输出见 T4 证据段）。完备性论证：档数提法的可能词形 = 「四档」（含包裹变体）、「判四」（动词紧缩）、「三档边界」（dev-loop.md 独有异形）、档位枚举句（「Full / Lite / 直接修」），四者并集覆盖语义全集；需求面「三档」属另一根轴（判据枚举 0/1/2），经逐条 Read 确认语义不相干后显式排除，不构成漏扫。JSON/MJS 面零命中（档位枚举不在脚本字符串面，required-check 不校验 rigor 闭集，Grep 实证）。

## 4. human 取舍点（OQ）

**以下四点 human 已裁（2026-10-16 两轮口述，照录不 reopen）**：

1. **pod-retro 正常记（human 已裁）**：nano 包不豁免 pod-retro.md，pod-events 簿记同理保留——保持 loop-retro 消费面统一。
2. **验收权威 = architect 事后白盒复核（human 已裁）**：nano 不写冻结验收测试、不跑红基线、不做 mutation 取证；architect 在 fixer 落补丁后拿开工落盘的意图定义对 actual diff 白盒复核（改的是不是说的那件事、有没有顺手改出界）；防回归 = 既有测试套件零新增红（verify.mjs）；视觉终裁仍归 human 持板。
3. **判档机器化（human 已裁）**：不靠人指定、不按代码行数；判据 = 改动独立性 + 爆炸半径小，用图工具量（判档预估 = `trace_path` inbound；收口实测 = `detect_changes` 对 actual diff）。
4. **五档，nano 独立成档（human 已裁）**：triage 四档变五档；本批命名「档 0 nano」，位序在 ①直接修 之上、②单包Lite 之下——命名理由：nano 仍过门（不像①免门）但不开完整 TaskPackage 仪式（不像②），数字 0 表达「比最小包档更小一档」的位序，且与既有①②③编号不产生重排（不改既有档号 = 历史制品引用零漂移）。

**以下待裁**：

- **OQ1 · architect.md 触面（自改递归，必须 human 明示）**：白盒复核职责定义落哪？
  (a) 同批改 `.claude/agents/architect.md`（When to Use + 职责段镜像，T6）——与 1g/10b 档「planner.md 定义 + architect.md 镜像」双写先例一致，一次批清防职责描述漂移；
  (b) 不动 architect.md，职责只落 change-triage.md + planner.md 派发契约，architect.md 镜像段殿后下一批——自改递归面触面最小，但 description 路由信号期内过期。
  **推荐 (a)**：双写先例在册（architect.md:31-32），且本批本就是 human-approved 治理变更，批准成本不新增。
  **human 已裁 2026-10-16 = (a)**：同批改 architect.md；T6 条件锚已转必选锚。
- **OQ2 · 机械牙齿形态**：
  (a) 全部扩承重闸——required-check 加 nano profile、verify-gates.json defer 扩、firmware-build-check reason 修、closeout-check 条件 warn 牙；判档半径与 detect_changes 收口实测全走 advisory（T5 本形）；
  (b) 新立 `nano-tier-check.mjs` 独立脚本硬判半径阈值——判档机判更硬，但新立脚本 + 新闸接线，违背「扩承重闸优先、克制新立脚本」模板纪律；
  (c) 扩 `change-manifest-check.mjs` 内嵌「nano 越界自动升档」语义——越界判定已在该闸（红即停），但「升档」是流程语义不是闸语义，写进脚本会把编排逻辑塞进判定闸。
  **推荐 (a)**：越界硬边界 = change-manifest 既有红（fail-closed 已在），升档动作归 planner 流程（T3），判档半径在样本来前不配硬齿（见 OQ3）。
  **human 已裁 2026-10-16 = (a)**：全扩既有闸、零新立脚本、半径 advisory——维持原设计不变。
- **OQ3 · 判档半径阈值标定方法**：
  (a) PROPOSAL 直接钉死数值（如 inbound ≤5）——可判性最强，但零样本定数 = 空判；
  (b) 建议区间起步（inbound 调用方 ≤5 且同 owning 文件、预估 diff ≤2 实现面文件，写进 change-triage.md 作判档指引），loop-retro 攒三轮 nano 样本后校准钉死（留根 §6 第 1 条）；
  (c) 不给数值指引，全交 architect 裁量——与 human 已裁③「判档机器化」精神不符，且滥用引力无第一道防。
  **推荐 (b)**：阈值先有指引形态、机判齿待样本校准，与 lite 快路径「复议触发器（YAGNI）」先例同构。
  **human 已裁 2026-10-16 = 实测钉死 inbound ≤ 5（不采纳推荐 (b)）**：主线程现跑四包爆炸半径（trace_path inbound depth 3，project C-AI-KidPal），分布 = 三个 nano 意图包全 ≤4 vs 中枢调度符号 17，一个数量级断层，阈值钉 5 恰落断层空档（实测表与「未亲核」标注见 T1 证据段）；T1 判据已按钉死口径改写。
- **OQ4 · 白盒复核件载体与牙**：
  (a) 独立件 `delivery/taskpackages/<id>/whitebox-review.md`（planner 代落）+ closeout-check 加 warn 级存在性牙（对偶 pod-retro Q5=warn 先例，closeout-check.mjs:277-284）；
  (b) 不立新件，复核结论只作 review-packet 一节——零新件零新牙，但复核可被静默跳过且 integrator 产出时复核上下文已隔一层；
  (c) 独立件但不加牙（convention-only）——件在但漏产无信号。
  **推荐 (a)**：复核是 nano 唯一的验收权威替代，无牙 = 可空过；warn 级力度与 pod-retro 先例对齐，不构成新 blocking。
  **human 已裁 2026-10-16 = (b)**：不落独立件、不加牙——复核落痕 = integrator 在 review-packet.md 增「白盒复核」节；T3 落痕载体、T5 锚清单、landing-manifest、§3 sweep、§5 验证集已同步删除 whitebox-review.md 与 closeout-check.mjs 相关条目。
- **OQ5 · human 视觉门形态**（触 UI 的 nano 包）：
  (a) exitCriteria 条目 + planner 在 supervisor 闸前 AskUserQuestion 请 human 目检确认（模拟器截图或真机），与 AB-A7 live-manual 停门先例同构（auxbar taskpackage.json:36-39）；
  (b) 不成文，靠既有真机轮惯例兜——但 nano 档面向的恰是视觉微调，终裁位不成文则验收权威三件套缺一条腿；
  (c) 并入 supervisor ESCALATE 恒升级 human——机械闸 riskTier 重派生并不识「视觉微改」，会把 nano 全部打成 high 升级，税比 lite 还重。
  **推荐 (a)**：先例同构、触点便宜（一句确认），且 human 已裁②末句「视觉终裁仍归 human 持板」需要落点。
  **human 已裁 2026-10-16 = (b)**：视觉终裁不成文、靠既有真机轮/看屏轮惯例兜——理由 = nano 本就生于看屏轮打回场景，人目检是在场前提，再立闸前确认是把已在场的事重收一道税；T3 视觉门条款已改写，supervisor 不恒 ESCALATE 维持。
- **OQ6 · G1 触面批准**（AGENTS.md:30 + DEV-GOVERNANCE.md:128/:235 档数同步，T7）：
  (a) 同批同步（措辞级，五档镜像）；
  (b) 本批不动 G1，档数镜像殿后下一批——SSOT 与指令根常驻矛盾期延长。
  **推荐 (a)**：五档裁定 human 已裁④，G1 同步是该裁定的镜像携带面；显式列出即满足模板「G1 必须显式升级」纪律。
  **human 已裁 2026-10-16 = (a)**：G1 同批同步；T7 两条件锚已转必选锚。
- **OQ7 · nano 意图定义记录落点**：
  (a) `delivery/taskpackages/<id>/plan.md`（frontmatter `rigor: nano` + 意图定义五件正文），与 ②单包同家；
  (b) 不落 plan.md，意图直接写 taskpackage.json 字段（story/notes）——机读近但 prose 塞 JSON 正是形状税第四波病灶；
  (c) 独立新件（如 nano-intent.md）——新制品类别，簿记面再分化。
  **推荐 (a)**：triage「reviewed plan 就近落地」形状统一（change-triage.md §4 现行即此），planner 只读消费规则零改动，planRef 机判可挂（T5）。
  **human 已裁 2026-10-16 = (a)**：维持 plan.md 落点设计不变。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> 双时点标注逐条给出；pre-commit 条目起草期已实跑，结果照录。自检类遵守真空绿规约（夹具非空 + 反转必红断言）；环境基线先立（2026-10-16 实跑：required-check self-check 18/0、verify.mjs 16/0、firmware-build-check self-check 绿、route-regression 52/0，全绿）。

1. **（pre-commit，起草期已跑）** `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261016-gov-review-nano-tier/PROPOSAL.md` → 预期 `ok:true` ∧ `findings:[]` ∧ `advisories:[]` ∧ `section2BodyFound:true` ∧ `anchorCount:14`（v1.0 定稿态：全部 14 锚均为必选锚——OQ1/OQ6 裁 (a) 两个+一个条件锚已转必选、OQ4 裁 (b) 已删 closeout-check.mjs 锚，条件锚裁剪语义不再适用）；anchorUnion 成员名单逐 token 核对 ≡ landing-manifest 围栏块 14 行、注记无夹带路径 token（实跑结果见文末起草注记）。
2. **（post-commit）** 五档文面在场：`grep -c "档 0" .claude/references/change-triage.md` ≥ 1 ∧ `grep -c "nano" .claude/references/task-intake-and-slicing.md` ≥ 1 ∧ `grep -n '"nano" | "lite" | "full"' .claude/references/task-intake-and-slicing.md` 命中 rigor 枚举行 ∧ `grep -c "nano" .claude/skills/kidpal-pod-launch-prep/SKILL.md` ≥ 1（launch 面 3b 触发扩，R1 修订新增）。
3. **（post-commit）** 复扫零残留：`grep -rn "四档\|判四\|三档边界" .claude AGENTS.md CLAUDE.md docs/DEV-GOVERNANCE.md`（需求面三档语境与 docs/debt、docs/showme、docs/archive 豁免）→ 预期 0 命中。
4. **（post-commit）** `node .claude/scripts/taskpackage-required-check.mjs --self-check` → exit 0 ∧ 输出含新增 nano 自检名（≥3 条夹具：合法 nano 包过 / 缺 planRef 红 / acceptance 非单条 suiteGate 红）∧ 既有自检计数不减（基线 18+4）；调用顺序审查：nano 自检接入既有 self-check 序列、不绕开。
5. **（post-commit）** `node .claude/scripts/firmware-build-check.mjs --self-check` → exit 0 ∧ 输出含 reason env 消费自检（VERIFY_GATE_DEFER_REASON 在场时 reason 取 env 文案、缺席回退硬编码——两臂都有断言）。
6. **（post-commit）** `node .claude/hooks/verify.mjs --self-check` → exit 0 ∧ 既有 16 项不减（defer 机制回归，whenRigor 匹配逻辑零改动断言）。
7. **（post-commit）** `node .claude/scripts/route-regression-check.mjs` → exit 0（triage-change command description 与 planner/fixer/architect def 改动后，必留路由信号不退化；基线 52 ok）。
8. **（post-commit）** `git diff --cached --stat` 对照 landing-manifest 14 行（v1.0 定稿态全为必选锚）：落地文件集 ≡ manifest 集合（landing-fidelity 结构门同判据，本条为 landing 后人读复核位）。

## 6. 留根清单

1. **判档半径硬齿**：`detect_changes` 实际半径进闸（如 change-manifest-check 或独立脚本）交 loop-retro 攒三轮 nano 样本校准后另批；判档阈值本身已实测钉死（OQ3：inbound ≤ 5，四包实测表见 T1 证据段），本条只管收口实测侧的硬齿化。
2. **supervisor 面 suiteGate glance**：nano 包恒触发 D-1 审计通道（review-packet 枚举 suiteGates + human glance 一句确认，dev-loop.md:168 现行 convention 承接）；若 nano 高频后 glance 成税，另批评机械消费。
3. **历史快照不回改**：`docs/showme/README.md:13`、docs/debt 归档、delivery 历史包/PROPOSAL/retro 中的「四档」时点表述保持原样（append-only 纪律）。
4. **真机轮硬门成文**不归本批：ARQ-261001-host-blind-outcome-device-gate 在册 deferred；nano 视觉终裁按 OQ5=(b) 不成文、由既有真机轮/看屏轮惯例兜，不预断该条目裁定。
5. **nano 校准复盘项（交 loop-retro，不硬治）**：nano 包单独聚合白盒复核发现率 / 越界升档率 / 判档误判率，三轮后回评 OQ3 钉死阈值（inbound ≤ 5）的有效性与 OQ2 牙齿力度。
6. **自律件（环内纪律，不进模板）**：本 PROPOSAL 引据行号以 2026-10-16 工作树为准；landing 期若目标文件已被并行批改动，行号引据以 FROZEN 时点重核为准（引用精确纪律）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-16 - architect

**四查**：① 意图保真——human 四点裁决逐条对照：pod-retro 不豁免（T3 保留清单 + T2 不可省均承接）、验收权威 = architect 事后白盒复核（T3 Step 10c + T6 镜像）、判档机器化（T1 trace_path inbound 预估 + detect_changes 收口实测双层，显式「不按代码行数」）、五档 nano 独立成档（T1，档 0 位序论证成立）——四点忠实承接，未见悄悄变形；T1 新增「无验收语义净增」轴与排除清单系 risk② 防线的合理操作化，非加料。注：human 口述原话引述（第二轮裁定④⑤等）我侧无派发记录可独立核verify，按输入契约采信、如实留痕。② 纪律合规——契约零触、scope-map 写域零扩张（architect 复核 planner 代落，先例 planner.md:33② 亲核在场）、supervisor 合并门 / 前置升级闸 / merge-driver 零触（grep 证实 supervisor-verdict-gate / supervisor-gate-check / pre-merge-escalation-gate / merge-driver / closeout-check / changes-index 均不消费 rigor，nano 走同一合并链无需感知）。③ 撞车复核——suiteGate 豁免机制亲核成立：`acceptance-coverage-check.mjs:143-144`（exempt 并集）、`mutation-check.mjs:141-142` + `:155-156`（全豁免集 `ok:true,skipped:true`）+ `:67`（M-4 成文）、`dev-loop.md:164`（正面清单首条）+ `:168`（D-1 审计通道）——T2「两闸零改动天然闭环」断言成立；`mutation-check.mjs:129` 仅在 `mutationRequired===false` 分支要求 skip 理由，字段缺席不触闸，nano 免 mutationRequired 可行。defer 机制亲核：`verify.mjs:119-126`（shouldDeferGate）+ `:249-263`（env 注入含真实 rigor）+ `verify-gates.json:58`；`firmware-build-check.mjs:64-66`（deferred 臂 reason 硬编码 'rigor=lite 包…'）+ `:122`（只读 VERIFY_GATE_DEFER）——T5「证据文案假称 lite」断点属实。条件锚 parser 语义亲核：`lib/governance-gate-shared.mjs:346`（规则文）+ `:443`（实现 `/触发条件|条件/` 裁剪），PROPOSAL 引行号实（文件实路径在 `.claude/scripts/lib/` 下，裸名引用可解析）。route-regression-check 无「四档」字面断言（grep 证实），T4 改 description 不破该检查；JSON/MJS 面四档 census 零命中证实。需求面「三档」另根轴排除成立（`requirement-change.md:1/:5` 三档判据 0/1/2）。④ 牵连面——发现两处缺口（F3/F4，见 findings）。

**判档结论（只升不降）**：本批 landing 面触 `.claude/agents/**`（planner/fixer/条件锚 architect）、`.claude/scripts/**` ×3、`.claude/commands/**`、`.claude/verify-gates.json`、`AGENTS.md`、`docs/DEV-GOVERNANCE.md`——`gov-change.md`「轻路径判据」③排除清单多条命中 → **恒 Track B 主路径全件**（PROPOSAL + landing-summary + landing-verdict + retro；retro 不豁免），非瘦制品轻路径。PROPOSAL Status 头「Track B 常驻轻路径（默认路由）」系沿 AGENTS.md 对默认路由的命名（「Track B 常驻轻路径——七步闭环」），与 gov-change.md 瘦制品义「轻路径」同名异义，见 F6。

**Track A 段判定**：不升级——方向已收敛（human 四点 2026-10-16 已裁），OQ1–OQ7 均为实现层取舍非方向级分叉；判据①跨系统结构性不命中（单面治理变更）。riskTier=medium 成立（触 G3 承重闸 + G1 + 自改递归条件锚，向严合理）。

**Findings**：

- **F1（BLOCKING）· T5 闸改法规格漏 `acceptanceTestsFile` 豁免点 + T2 字段级 SSOT 不完整**：`taskpackage-required-check.mjs:330` 将 `acceptanceTestsFile` 与 `acceptance/exitCriteria/contractRefs/docRefs/sourceRefs/gateProfile` 同列**无条件非空数组必填**（与 finalize 代理 `:526 isFinalizedState` 是两回事）；T2 豁免清单豁免了 `acceptanceTestsFile`，但 T5 nano 分支改法只列「免 redBaselineCommand/Result、免 mutationEvidence 骨架、免 finalize 态 input-brief」，未列「免 :330 acceptanceTestsFile 非空必填」——按 T5 字面落地，合法 nano 包（无验收文件，:T2 豁免语义）必在 :330 红，nano profile 无法闭环；而若改填既有验收文件凑非空，则 `isFinalizedState=true` 反向触发 input-brief 在场检查（:545-551）与 AC 回引齿（:165，story 命中包必红——nano 单条 suiteGate 无 AC 回引），矛盾更深。同族：T2「不可省」清单漏 `deliveryImpact`/`planningImpact`（:326 必填）与 `contractRefs`/`docRefs`（:330 必填），按 T2 字面建包会撞闸。修法方向：T5 nano 分支显式加「免 :330 acceptanceTestsFile 非空必填（nano 恒缺席）」；T2 不可省清单补齐四字段（或加一句「未列字段按 full 既有闸判据照常必填」总括）。§5 第 4 条自检夹具（合法 nano 包过）大概率能在实现期兜住此缺口，但 FROZEN 文本即落地权威，规格须先补全。
- **F2（BLOCKING）· Step 10c 效力声明照抄 10b 与「验收权威」角色自相矛盾**：T3 规定 10c dispatch prompt 必含「与 10b 同款」效力声明，原文含「**不构成通过判据**」；T6 镜像同款。但 human 裁② 明定白盒复核 = nano 的**验收权威**（T3 自述「验收权威生产位」，结论三值 PASS/出界/非所说事，OQ4(a) 称「复核是 nano 唯一的验收权威替代」）——照抄该句等于验收权威自我否定，落地后可被援引为跳过判分的口实（防空过语义削弱）。10b 的「不构成通过判据」成立的前提是门与冻结验收仍是权威；nano 恰无冻结验收，此句不可平移。修法方向：10c 效力声明改写为区分两义——「不豁免 verify.mjs 任何闸、不改 TaskPackage 事实；本复核即 nano 行为验收的判分位（human 2026-10-16 裁②），PASS 为 Step 11 前置、出界回 triage、非所说事退 fixer」。
- **F3（SHOULD）· 四档 census 漏一处命中**：判官实测 `grep -n "四档" .claude/commands/triage-change.md` = 4 处命中（:2 / :13 / :22 / **:26**）；T4 证据清单只列 :2/:13/:22，漏 ：26「呈现 architect 的四档裁决」。§3 sweep 声明「命中清单见 T4 证据段」故引据不完备；§5 第 3 条 post-commit grep 会兜住残留，但 census 引据须补 ：26、T4 改法范围随之补该行。
- **F4（SHOULD）· 牵连面漏消费方：launch 面 lite 快路径只认 rigor=lite**：`.claude/skills/kidpal-pod-launch-prep/SKILL.md:77-84`（3b 分支）触发条件 = `rigor=lite`，nano 不享 launch 面固件环境等价化后置；§3 sweep 未列该 skill，T2「与 lite 快路径关系」只声明 verify 半边（defer 扩 whenRigor）同待遇、对 launch 半边沉默。若不对称系有意（nano 即便新树也在 launch 即备环境）须在 T2/§3 记裁断与理由；若无意则需扩 skill 分支（加 manifest 行）。建议以显式声明收口（nano 典型为 UI 微改、final 相位 firmware-build 恒跑终需环境，launch 侧后置收益有限——但该判断应落在 FROZEN 文本里，不落在我这里）。
- **F5（SHOULD）· planner Step 5 既有条文与 nano 验收形态冲突未承接**：planner.md:53「`acceptance[]` 必须承载行为决策，不允许整组只有结构条目」——nano 恰一条 `{id, suiteGate:true}` 即「整组只有豁免条目」。机械面无此断言（判官 grep required-check / acceptance-coverage-check 证实），但 planner 自身条文在 nano 包上自相矛盾；T3 的 nano 豁免清单应显式承接该句例外（注「nano 例外由 T2 恰一条 suiteGate 形态承载」）。
- **F6（SHOULD）· Status 头轨道命名建议消歧**：头部「review 轨：Track B 常驻轻路径」建议改写为「Track B 主路径（机器面恒 Track B，非瘦制品轻路径）」——防落地段误套瘦制品五件 + retro 豁免。
- **F7（NIT）· verify-gates.json _doc 同步**：`_doc` 两处「rigor=lite 包…后置」字样（firmware-build 条目 _doc 与 _doc 头 defer 说明段）在 whenRigor 加 nano 后变不完备；该文件本在 manifest 内，T5 落地时顺手同步措辞即可，非扩面。
- **F8（NIT）· 行号引据微漂**：auxbar taskpackage.json 三条 kind:structural 对象实为 ：24-28 / :30-34 / :35-39（PROPOSAL 记 ：30-35 / :36-39，各差一行）；定稿时顺手校正（引用精确纪律）。

**亲核留痕（判官实测，2026-10-16，本 worktree gov/kidpal）**：① 成本 census 三包 `pod-events-aggregate.mjs --change-id` 实跑——voice dispatchesTotal=3（builder 590251ms≈9.8min + integrator 547785ms≈9.1min + acceptance-author ≈3.2min ≈ 22.2min）、auxbar =3（acceptance-author 1907807ms=31.8min + builder/integrator 与申报吻合 ≈53.9min）、tomorrow-row =16（acceptance-author 2856489ms=47.6min×5 + integrator 1599902ms=26.7min×8 + builder 1526005ms=25.4min×2 + architect ×1 ≈100.5min）——T1 数值全数吻合；② 形状税——`wc -l` = 175、notes 3829 字符、acceptance 7 条、structural 3 条（node 实测）；③ 基线自检实跑——required-check self-check 18/0（+changeid-prefix 4/0）、verify.mjs 16/0、firmware-build-check 绿、route-regression 52 ok/0 fail、landing-fidelity preflight `ok:true / findings:[] / anchorCount:10` 且并集与起草注记逐 token 一致；④ 其余 file:line 引据（change-triage.md:25-28/:104-110、task-intake-and-slicing.md:240/:242/:253/:212-226、planner.md:33/:49/:62-65/:80-84、fixer.md:20-25/:49、architect.md:26/:31-32、scope-map architect allow 不含 delivery/taskpackages + deny 自身、dev-loop.md:26/:164/:168、AGENTS.md:30、DEV-GOVERNANCE.md:128/:212/:235、triage-change.md:2/:13/:22、architect-operating-model.md:67、closeout-check.mjs:277-284、verify-gates.json:58、brief-inputs.mjs 无 rigor 分支依赖〔:460 仅展示、:573 self-check 夹具〕）逐条 Read/Grep 核实为真；⑤ 结构字段在场核对——landing-manifest 围栏块（14 行 = 10 必选 + 4 条件）与 packet-id 行均在场，§5 第 1 条定稿义务（FROZEN 前按 OQ 裁断改写条件锚并复核并集 ≡ manifest）已载。

verdict: BLOCK(F1,F2)

---

**起草注记（非判官区）**：§5 第 1 条 preflight 实跑结果——v1.0 定稿后复跑（2026-10-16，`node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261016-gov-review-nano-tier/PROPOSAL.md`）：`ok:true`、`section2BodyFound:true`、`findings:[]`、`advisories:[]`、`anchorCount:14`，anchorUnion 成员名单 = `.claude/agents/architect.md`、`.claude/agents/fixer.md`、`.claude/agents/planner.md`、`.claude/commands/triage-change.md`、`.claude/references/architect-operating-model.md`、`.claude/references/change-triage.md`、`.claude/references/dev-loop.md`、`.claude/references/task-intake-and-slicing.md`、`.claude/scripts/firmware-build-check.mjs`、`.claude/scripts/taskpackage-required-check.mjs`、`.claude/skills/kidpal-pod-launch-prep/SKILL.md`、`.claude/verify-gates.json`、`AGENTS.md`、`docs/DEV-GOVERNANCE.md`——与 landing-manifest 围栏块 14 行逐 token 一致（§2 并集 ≡ manifest 核对通过）。v1.0 定稿动作：human 裁毕全部 OQ——OQ1=(a) 与 OQ6=(a) 三条件锚转必选、OQ4=(b) 删 closeout-check.mjs 锚与 §5 warn 牙验证条（复核落痕改 review-packet「白盒复核」节）、OQ5=(b) 视觉终裁不成文（T3 改写）、OQ3=实测钉死 inbound ≤ 5（T1 判据 + 四包实测表入证据段，起草会话无 MCP 图工具、数值标「未亲核」以主线程实测为准）。

### R2 - 2026-10-16 - architect

**轮次焦点**：R1 八条 findings 修订到位性逐条核验 + 修订引入的新矛盾扫描；R1 已亲核且未被修订触动的引据不重跑。

**R1 findings 逐条核验**：

- **F1 修订到位**。T5 现文显式列「**免 :330 行 `acceptanceTestsFile` 数组非空必填**」并补「nano 包不依赖 acceptanceTestsFile 判 finalize（直接按 nano profile 全量校验）」；T2 不可省清单补齐 `deliveryImpact` / `planningImpact` / `contractRefs` / `docRefs` 四字段 + 总括句「未在下方豁免清单显式列名的闸必填字段（taskpackage-required-check.mjs:326 与 :330 两必填行）一律照 full 判据」。判官亲核两必填行实际内容：:326 for 循环清单 = `changeId/subsystem/story/redBaselineCommand/redBaselineResult/deliveryImpact/planningImpact/riskTier`；:330 = `acceptance/acceptanceTestsFile/exitCriteria/contractRefs/docRefs/sourceRefs/gateProfile`——T5 豁免点名（redBaseline×2 + acceptanceTestsFile）与两行未豁免字段集合对得上，T2 不可省清单与未豁免字段一一对应，总括句兜底完备：按 T5 字面落地时合法 nano 包（acceptance 恰一条 suiteGate、acceptanceTestsFile 缺席、无验收文件）不再在 :330 红，亦不再经 :526 isFinalizedState 代理触发 input-brief 在场检查与 AC 回引齿。contractRefs/docRefs 的「无契约命中时按既有口径填契约面引用或显式说明」措辞与闸行为（仅查非空数组）相容。
- **F2 T3 主体改写到位，但第二载体残留（见 R2-F1）**。T3 10c 效力声明原文已重写为「不豁免 verify.mjs 任何闸、不改 TaskPackage 事实；本复核即 nano 行为验收判分位，PASS 为 Step 11 前置」，与 10b advisory 效力的辨析（「有意不同」+「名为 ARCH-ADVISORY 仅指派发通道复用」）清楚。全 PROPOSAL grep 复证：T 段生效措辞无「不构成通过判据」残留（:107 为引述辨析、:288 为 R1 append-only 记录，均非生效措辞）。**T6 镜像句未同步，见 R2-F1。**
- **F3 修订到位**。T4 证据补 ：26（「呈现 architect 的四档裁决——判官 F3 补漏，R1 修订复跑 grep 复证 4 处命中」），改法范围扩为「description 与 ：13、:22、:26」，§3 sweep 行同步「:2/:13/:22/:26 四处」。判官亲核 `triage-change.md` 四档提法实为 ：2/:13/:22/:26 四处，与修订覆盖集一致。
- **F4 修订到位**。处置 = 扩 skill 对称，四处落位齐全：landing-manifest 加 `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（manifest :24）、§2 T4 改法（3b 触发条件扩 `rigor ∈ {lite, nano}` + 步骤 4 头注 lite/nano 同步）、§3 sweep 加 G2 skills 行、§5 第 2 条加 `grep -c "nano" SKILL.md` 断言。T1 判档判注义务已立（「host 齿可闭环」确认，明示 launch 半边见 T4、verify 半边见 T5）。判官亲核比照对象与倒挂论证：`SKILL.md:77-84` 3b 触发 = `rigor=lite` 且步骤 4 头注「lite 包新树：整步骤后置真机轮前」、`change-triage.md:108` lite 判注义务原文在场（T1 比照同构成立）、`verify-gates.json:58` `defer.whenRigor:["lite"]`——lite 两半皆享后置，nano 更轻却两半皆无 = 倒挂成立，扩 skill 对称处置逻辑自洽。
- **F5 修订到位**。T3「另立显式例外承接」段载明 planner.md:53 原句与例外括注措辞（「`rigor=nano` 档为显式例外：整组 = 恰一条 suiteGate 条目…」）；planner.md:53 原句「`acceptance[]` 必须承载行为决策，不允许整组只有结构条目」亲核在场。
- **F6 修订到位**。Status 头 :5 现文「**review 轨**：Track B 主路径（机器面恒 Track B，**非**瘦制品轻路径——制品四件 + retro 照常，不误套轻路径 retro 豁免…）」。
- **F7 修订到位**。T5 verify-gates.json 落地注记载明 `_doc` 两处（「:36 头注数组内、:59 firmware-build 条目 `_doc` 字段」）；判官亲核两处实际内容（:36「rigor=lite 包后置 final 相位——」、:59「rigor=lite 包 scoped 相位后置到 final…」）与注记吻合，只改措辞不动判定语义的边界声明在场。
- **F8 修订到位**。T1 证据校正为 :24-28/:30-34/:35-39；判官亲核 auxbar taskpackage.json：AB-A4 结构对象 :24-28、AB-A6 :30-34、AB-A7 :35-39，三段行号实。

**新矛盾扫描 findings**：

- **R2-F1（BLOCKING）· T6 镜像句「效力声明与 1g/10b 同款」与 T3 改写后 10c 声明自相矛盾（R1 F2 修订未覆盖其第二载体）**：PROPOSAL T6 改法现文「…只读 + 回执文本、不落盘、不改任何文件；**效力声明与 1g/10b 同款**）」，而 T3 已钉死 10c 声明与 10b advisory 效力**有意不同**。1g/10b 效力声明尾句原文亲核 = planner.md:49「不豁免任何门、不改验收与 TaskPackage 事实、不构成通过判据」、planner.md:84「…不构成通过判据；fixer 可拿证据反驳…」——若 OQ1 裁 (a) 按 T6 字面落地，architect.md 镜像段会把 10b 的「不构成通过判据」写成 10c 的效力声明 = R1 F2 点名的「验收权威自我否定、可被援引为跳过判分的口实」在第二载体原样回植；且 PROPOSAL 自身 T3「有意不同」与 T6「同款」并存，FROZEN 文本内部矛盾（本批 T6 根因自述「两处缺一…必有一个过期」恰被本句自证）。修法方向：T6 镜像句改为「效力声明 = T3 Step 10c 改写后原文（与 1g/10b 有意不同）」或直引同一声明原文，两落点单源。升级 human：否——修法唯一且机械，无方向分叉。
- **R2-F2（NIT）· §5 第 9 条 manifest 行数计数未随 F4 修订同步**：现文「`git diff --cached --stat` 对照 landing-manifest 14 行」，但 F4 修订 manifest 加 SKILL.md 后围栏块现为 15 条目（判官亲核 manifest :13-27 = 15 行 = 11 必选 + 4 条件锚，与 §5 第 1 条 `anchorCount:11` 及起草注记一致；R1 亲核留痕的「14 行 = 10 必选 + 4 条件」系修订前时点快照，append-only 不回改）。修法方向：「14 行」→「15 行」，或改写为「对照 landing-manifest 条目集（11 必选 + 4 条件锚按 OQ 裁断裁剪）」防行数再漂。

**锚一致性亲核**：判官亲跑 `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261016-gov-review-nano-tier/PROPOSAL.md` = `ok:true` ∧ `findings:[]` ∧ `advisories:[]` ∧ `section2BodyFound:true` ∧ `anchorCount:11`，anchorUnion 成员名单与起草注记逐 token 一致；manifest 围栏块 15 行 = 11 必选 + 4 条件锚，一致。

**判档与升级（只升不降）**：判档不变——Track B 主路径全件（R1 判档结论的触面集未被修订扩出边界，SKILL.md 属 G2 skills 原本就在机器面集合内）；Track A 段判定维持不升级（R2-F1/R2-F2 均为实现层文本修订，无方向级分叉）。

**亲核留痕（判官实测，2026-10-16，本 worktree gov/kidpal）**：① `taskpackage-required-check.mjs:326/:330` 两必填行字段清单 Read 亲核（见 F1 条）；② `triage-change.md` 全文 Read——四档提法 :2（description「判四档（③拆包 / ②单包Full / ②单包Lite / ①直接修）」）、:13（「照旧判四档」）、:22（「判四档 + 出 reviewed plan」）、:26（「呈现 architect 的四档裁决」）四处实；③ `SKILL.md:77-84` Read——3b 标题「lite 快路径（`rigor=lite` 包…）」、触发句、步骤 4 头注「lite 包新树：整步骤后置真机轮前」实；④ `change-triage.md:108` Grep——lite 判注义务「host 齿可闭环」原文实；⑤ `verify-gates.json:36/:58/:59` Read——头注数组内「rigor=lite 包后置 final 相位」、firmware-build `defer.whenRigor:["lite"]`、`_doc`「rigor=lite 包 scoped 相位后置到 final」实；⑥ `planner.md:49/:53/:84` Read——1g 效力声明尾句「不构成通过判据」、Step 5 「不允许整组只有结构条目」、10b 效力声明原文实；⑦ auxbar `taskpackage.json:24-39` Read——三条 kind:structural 对象行号 :24-28/:30-34/:35-39 实；⑧ preflight 实跑输出见上「锚一致性亲核」；⑨ PROPOSAL v0.1→v0.2 全量 diff（git diff abb769c d4b39e1）通读——修订范围与起草注记自述一致，无夹带改动；T6 :156 与 §5 第 9 条 :262 的两处残留即 diff 后仍存于 v0.2 的原文。

verdict: BLOCK(R2-F1)

### R3 - 2026-10-16 - architect

**轮次焦点**：R2 两条 findings 修订到位性核验 + v0.3 新矛盾扫描 + GREEN 前结构字段核对；R1/R2 已亲核且未被 v0.3 触动的引据不重跑。

**R2 findings 逐条核验**：

- **R2-F1 修订到位**。T6（:156）现文「**效力声明引 T3 Step 10c 声明原文、逐字一致**——「不豁免 verify.mjs 任何闸、不改 TaskPackage 事实；本复核即 nano 行为验收判分位，PASS 为 Step 11 前置」，两落点单源」+ 显式禁令「**禁止回植 1g/10b 的「不构成通过判据」尾句**——10c 是 nano 档验收权威本体（human 已裁②），自我否定的声明会把 R1 F2 的病从镜像段原样带回（R2-F1）」——「与 1g/10b 同款」表述已删（grep `与 1g/10b 同款|10b 同款` 仅命中 :288/:321 两处 §7 append-only 评审记录，非生效措辞）。逐字比对（node 字节级提取两行引文）：T3 :107 引文 = 「不豁免 verify.mjs 任何闸、不改 TaskPackage 事实；**本复核即 nano 行为验收判分位，PASS 为 Step 11 前置**」，T6 :156 引文去除 markdown 加粗标记后与之**逐字符相等**——T3 引文内的 `**` 系该行排版强调（包住后半句 punchline），非声明内容差异；判定「逐字一致」在字符内容层成立，landing 时两落点取同一去标记纯文本即可，不构成 finding。「不构成通过判据」生效面命中仅两处元语句：T3 :107（「与 10b 档的 advisory 效力（「不构成通过判据」）**有意不同**」辨析引述）与 T6 :156（禁令句本身引用尾句字面以禁止回植）——均为引述/禁令，非 10c 声明本体；其余命中（:288/:311/:321/:328）全在 §7 评审记录，无生效面残留。
- **R2-F2 修订到位**。§5 第 9 条（:262）现文「`git diff --cached --stat` 对照 landing-manifest 15 行（11 必选锚 + 4 条件锚，条件锚按 OQ 裁断裁剪后）」——行数修正与条目集表述双采纳（R2 修法方向给出的两个选项都落了）。判官亲核 manifest 围栏块 :13-27 = **15 条目**（sed 实测逐行打印核对），= 11 必选锚（preflight anchorUnion 同集）+ 4 条件锚（architect.md / closeout-check.mjs / AGENTS.md / docs/DEV-GOVERNANCE.md），与 §5 第 1 条 `anchorCount:11` 及起草注记联立一致。

**v0.3 新矛盾扫描**：`git diff bcbd307 7f9d13e -- <本PROPOSAL>` 全量通读——改动恰三处（Status 状态行 v0.2→v0.3、T6 改法镜像句、§5 第 9 条），与 R2 否回的修法方向一一对应、无夹带改动；T6 新文与 T3 :107「有意不同」辨析、§5 第 1 条 anchorCount 断言、起草注记均无冲突。preflight 亲跑（2026-10-16 本 worktree）= `ok:true` ∧ `section2BodyFound:true` ∧ `anchorCount:11` ∧ `findings:[]` ∧ `advisories:[]`，anchorUnion 11 成员与起草注记逐 token 一致。

**GREEN 前结构字段核对**：landing-manifest 围栏块在场（:12-28，15 条目）；packet-id 行在场（:6「packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T7）+ §5 验证集」）；§5 第 1 条末句定稿义务在场（:254「**定稿义务**：FROZEN 前按 OQ1/OQ4/OQ6 裁断改写条件锚（裁 (a) 则去条件注记转必选锚、裁 (b) 则删锚并裁剪 manifest），复跑本预跑核「§2 并集 ≡ manifest 围栏块」」）。

**判档与升级（只升不降）**：判档维持 Track B 主路径全件（v0.3 触面集与 R1 判档结论一致，未扩边界）；Track A 段判定维持不升级——R2-F1/R2-F2 修订均为实现层文本收敛，v0.3 未引入方向级分叉。无升级 human 项。

**亲核留痕（判官实测，2026-10-16，本 worktree gov/kidpal）**：① node 字节级提取 :107/:156 两行「不豁免…」引文比对——输出 `verbatim-equal (ignoring md bold): true` 且 T3 引文含 `**` 标记、T6 无（判定见 R2-F1 条）；② grep `与 1g/10b 同款|10b 同款` = :288/:321 两行（均 §7 记录）；③ `sed -n '13,27p'` manifest 围栏块 = 15 条目逐行打印核对（11 必选 + 4 条件，与 R2 亲核留痕⑧同一集合）；④ `node .claude/scripts/landing-fidelity-check.mjs --preflight <本PROPOSAL>` 实跑 = ok:true / anchorCount:11 / findings:[] / advisories:[] / section2BodyFound:true；⑤ `git diff bcbd307 7f9d13e` 全量通读 = 三 hunk 与 R2 否回项一一对应；⑥ 结构字段 grep 定位——packet-id :6、manifest 围栏 :12-28、§5 第 1 条定稿义务 :254、§5 第 9 条 :262。

verdict: GREEN

### R4 - 2026-10-16 - architect

**轮次焦点**：定稿保真专项（R3 GREEN 后 human 裁毕全部 OQ、v1.0 定稿改写）——七项逐核：§4 裁断保真、OQ4(b)/OQ5(b) 落地保真、条件锚三转一删、T1 阈值与实测表、定稿 diff 全量通读（`git diff 9476bdb..fdfd2e3`）、结构字段与凭据链。R1–R3 已亲核且未被 v1.0 触动的引据不重跑。

**七项核验**：

1. **§4 裁断保真：七条全对**。OQ1=(a)（:217「同批改 architect.md；T6 条件锚已转必选锚」）、OQ2=(a)（:223「全扩既有闸、零新立脚本、半径 advisory——维持原设计不变」）、OQ3=实测钉死 inbound ≤ 5、明示不采纳推荐 (b)（:229「实测钉死 inbound ≤ 5（不采纳推荐 (b)）」）、OQ4=(b) 裁在非推荐位（:235，推荐原为 (a)）、OQ5=(b) 裁在非推荐位（:241，推荐原为 (a)）、OQ6=(a)（:246）、OQ7=(a)（:252）——七条标注与 human 裁断逐字一致；OQ4/OQ5 确认 T 段按 (b) 改写而非残留 (a) 设计（到位面见第 2/3 项，残留面见 findings）。
2. **OQ4(b) 落地保真：主体到位，词汇/机制残留见 R4-F1/R4-F2**。到位面：T3 :107 落痕载体 = integrator 在 review-packet.md 增「白盒复核」节 +「不落独立件、closeout-check 不加牙」+「10c 判分位语义不变——PASS 仍为 Step 11 前置」三要素齐；T5 改法与目标文件行（:144）已无 closeout-check 条目（定稿 diff 实删条件牙 bullet 与目标文件第四项）；§5 原 OQ4(a) 专属验证条（旧第 8 条 closeout-check warn 牙夹具）已删、条目重编号连续（现第 8 条 = git diff --cached --stat 对照 manifest，无断号）；全 PROPOSAL grep `whitebox-review` 字面：生效面零命中（命中仅 §4 OQ4 选项/裁断 ：231/:235 与 §7 历史记录）。
3. **OQ5(b) 落地保真：到位**。T3 :108「不成文——不设 exitCriteria 视觉门条目、不设 supervisor 闸前 AskUserQuestion 确认……supervisor 不恒 ESCALATE 维持现状」；T2 :84 exitCriteria 括注同步为「视觉门不成文，OQ5=(b) 见 T3」；T3 证据 ：100 live-manual 先例引证保留（先例引证属举证材料非视觉门条款，不成文裁定不受影响）；AskUserQuestion 无生效面设计残留（:3 为 Status 留痕注记、:237 为 §4 选项文本）。
4. **条件锚三转一删：核过**。manifest 围栏块 ：12-27、条目 ：13-26 恰 14 行亲点；architect.md / AGENTS.md / docs/DEV-GOVERNANCE.md 三锚在场且 §2 目标文件行条件注记已去（T6/T7 标题与目标文件、§3 sweep 行、Status 头 ：8 四处同步）；closeout-check.mjs 不在 manifest、不在 §2 锚清单。判官亲跑 preflight = `ok:true ∧ section2BodyFound:true ∧ anchorCount:14 ∧ findings:[] ∧ advisories:[]`，anchorUnion 14 成员与 manifest 围栏块逐 token 一致。
5. **T1 阈值与实测表：在场 + 侧面抽查成立**。判据钉死句 :59「阈值钉死 = 每将触符号 inbound 调用方 ≤ 5 且同属一个 owning 文件/紧耦合簇」；实测表 ：51 四包数值齐全且含「topbar 被拒……是判据生效的设计意图，不是误伤」设计意图说明；「未亲核」标注如实（主线程 trace_path 现跑、起草会话无 MCP）。判官侧面抽查（Grep 实测，不精确复算）：`p1_blit_art` 在 `firmware/main/display/home_p1.c` 直接调用点 = 4（:1143/:1217/:1273/:1341，定义 :573 与注释不计）——与申报 = 4 恰合；`rn_render_kidpal_page_locked` 直接调用 4 处（kidpal_render_node.cc :656/:999/:1037/:1150）+ 其四公共入口（kidpal_render_node_cycle_next / refresh_story / dismiss_front_overlay / voice_end）设备侧外部调用点 ≥9（application.cc:1088、waveshare 板 ：138/:160/:185、mcp_server.cc :240/:300/:390/:1079/:1116），depth-3 传递闭包 ≈17 量级可信、明显 >5——断层分布成立，「阈值钉 5 恰落断层空档」经抽查无反证。
6. **定稿 diff 全量通读：无夹带**。`git diff 9476bdb..fdfd2e3 -- <本PROPOSAL>` 13 个 hunk 逐个核对——全部映射到七条 OQ 裁断及其联动同步（Status/review profile、manifest 删 closeout-check 行、T1 判据钉死+实测表、T2 exitCriteria 括注、T3 三 bullet、T4 条件注记去条件化、T5 删牙+标题、T6/T7 标题与目标文件、§3 四行、§4 七条裁断行、§5 第 1/3/7/8 条、§6 第 1/4/5 条、起草注记），无无关改动混入；§6 留根与 OQ3/OQ5 同步（第 1 条现文「判档阈值本身已实测钉死……本条只管收口实测侧的硬齿化」——无「阈值待校准」残留表述；第 4/5 条同 sync）；§5 各条（pre-commit）/(post-commit) 时点标注 8/8 在场。
7. **结构字段与凭据链：齐**。manifest 围栏块 ：12-27 与 packet-id 行 ：6 在场；Status 行 ：3 载 OQ 裁毕留痕与「定稿待主线程标 FROZEN」，立项 ack 由 ：4「human 2026-10-16 两轮口述直接发起」承载——R4 GREEN 后凭据链（立项 ack + OQ 裁毕 + R3 GREEN + R4 GREEN）完整，可标 FROZEN。

**Findings**：

- **R4-F1（BLOCKING）· T1 判据句残留 OQ4=(a) 落痕载体词汇「白盒复核件」，与 T3/OQ4=(b) 矛盾**：T1 判档双层形状句（:62）现文「收口实测 = `detect_changes` 对 actual diff 算实际爆炸半径，结果记进白盒复核件（advisory，见 T3）」——「白盒复核件」在本 PROPOSAL 语汇中是 OQ4 的定义术语（OQ4 标题「白盒复核件载体与牙」即 whitebox-review.md 载体问题），OQ4=(b) 已裁「不落独立件」，T3 :107 生效文为「记入回执」+「落痕载体 = integrator 在 review-packet.md 增『白盒复核』节」。T1 字面落地会把收口实测落点写进一个 (b) 设计下不存在的独立件——而 T1 判据节恰是落进 change-triage.md 的 canonical 内容，该悬空指称会随 landing 进入档位 SSOT，未来读者按 T1 找「白盒复核件」即回植被拒的 (a) 设计。与 R2-F1 同判：FROZEN 文本内部矛盾 + 字面落地回植被拒设计 → BLOCKING。修法方向：:62 改为「……结果记入白盒复核回执（advisory，见 T3；落痕载体 = integrator review-packet『白盒复核』节，OQ4=(b)）」。升级 human：否——修法唯一且机械，无方向分叉。
- **R4-F2（SHOULD）· 「planner 代落」落痕链三处 (a) 时代残留**：OQ4=(a) 的落盘机制是 planner 代落 whitebox-review.md；(b) 下落痕 = integrator 写 review-packet 节，planner 代落不再是机制一环。残留三处：T3 证据 :99「复核结论落盘须由 planner 代落」（(b) 下落盘者 = integrator，「须由 planner 代落」为过期断言）；T6 改法尾句 ：154「（复核只读 + planner 代落，写域零扩张）」（结论「写域零扩张」在 (b) 下依然成立——architect 恒只读、integrator 写自家制品——但括注机制名过期）；§3 sweep :190「architect 复核回执 planner 代落（先例 planner.md:33②）」。三处均不承载闸语义、结论不受影响，故 SHOULD；修法方向：三处统一改为「architect 恒只读零写；落痕 = integrator review-packet『白盒复核』节（OQ4=(b)）」。

**判档与升级（只升不降）**：判档维持 Track B 主路径全件（v1.0 触面集与 R1 判档结论一致，OQ4=(b) 删 closeout-check.mjs 触面属收窄非扩张）；Track A 段判定维持不升级——R4-F1/R4-F2 均为实现层文本残留修订，无方向级分叉。无升级 human 项。

**亲核留痕（判官实测，2026-10-16，本 worktree gov/kidpal）**：① preflight 亲跑输出 = ok:true / section2BodyFound:true / anchorCount:14 / findings:[] / advisories:[]，anchorUnion 14 token 与 manifest 逐一对上；② 全文 grep `whitebox-review|closeout-check`——命中 ：8/:106/:107/:219/:231/:235/:258/:282/:299/:305/:342（:106 为 Step 11 既有 closeout-check 保留项、:107 为「不加牙」否定句，其余为 Status 头裁断注记、§4 选项/裁断文本与 §7 append-only 历史；生效面零残留）；③ 全文 grep `白盒复核件|视觉门|AskUserQuestion|代落|第 9 条`——定位 R4-F1 :62 与 R4-F2 :99/:154/:190；其余命中（:3 Status 留痕、:100/:108 T3、:230/:236/:237 §4、:325/:342/:344 §7 历史）均豁免；④ 定稿 diff 13 hunk 全量通读（逐 hunk ↔ 七条 OQ 映射核对）；⑤ `p1_blit_art` home_p1.c 调用点 grep = 4；⑥ `rn_render_kidpal_page_locked` 全固件 grep（kidpal_render_node.cc 四调用点 + 四公共入口外部设备侧调用点 grep 实测 ≥9）；⑦ manifest :13-26 亲点 14 行、packet-id :6、Status :3/:4、§5 八条时点标注在场；⑧ T5 目标文件行 ：144 = 三文件无 closeout-check、§5 现第 8 条 = manifest 对照条（原 warn 牙验证条已删、无断号残留）。

verdict: BLOCK(R4-F1)
