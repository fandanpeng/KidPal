# PROPOSAL · 260920-gov-review-reader-scripts-align

> **状态：DRAFT v0.1 · 2026-09-20 · 待 human 立项 ack**（FROZEN 行须含凭据：human 立项 ack + architect Rn GREEN + human 裁 OQ 结果）。
> **承载 ARQ**：`ARQ-260909-precedent-merged-detection-gap`（T1）、`ARQ-260914-portfolio-reconcile-launchpreconditions`（T2）、`ARQ-260909-dispatch-stamp-telemetry-gaps`（T3，**子项 a；子项 b 经实测缩圈**）。不关的根写 §6 留根指针。
> **本批定位**：三条都是**读侧脚本与事实不对齐**——脚本读的东西与仓内实际形态脱节，且都**不改任何门的判定语义**（改的是一个 advisory 生成器、一个 launch 检查器、一个簿记写入器的**字段/判定源**）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。两条升级判据（触契约面 / 跨系统结构性变更）均未命中。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T3）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（一句话证成：三条均为**读侧对齐**——T1 给一个 advisory 生成器补一个判定源、T2 给一个 launch 检查器补读一个**已存在**的键（键缺视为无前置，向后兼容）、T3 给一个簿记写入器补一个**可选**字段（不传则不带该键）。无闸判定语义改动、无放松面、无数据丢失形态）。
> **review profile**：G3（`.claude/scripts/` 三个脚本）。契约面 / G1 / G2 / agent frontmatter 均为零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/brief-inputs.mjs
.claude/scripts/dispatch-stamp.mjs
.claude/scripts/portfolio-reconcile.mjs
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：让三个读侧脚本**读到的东西与仓内实际形态对齐**——先例判定认得出本仓的主流合并形态、launch 检查器读得到已经写在册的前置清单、簿记写入器记得到「这一派发实际跑在哪个模型上」。

**现状差距**：三条的形态完全一样——**脚本按早期形态写死了判定源/字段，形态变了之后没人回写**。后果都是**静默**：不报错、不告警，只是读出来的东西不是真的（先例集给陈旧包、前置清单没人核、模型维度永远空白）。

**非目标（本批不做什么）**：不改任何门的**判定语义**（T1 是 advisory 生成器、T2 只让既有检查器**多读一个键**、T3 只加一个**可选**字段）；不新增工程门；不动 `docs/contracts/**` 与系统 spec 两文件；不动任何 agent frontmatter；**不并入 `ARQ-260909-dispatch-stamp-telemetry-gaps` 的子项 b**（经起草期实测**缩圈移出**，见 T3）。

## 2. 问题、根因与改法（T1…T3）

**【硬项 · 起草期实证义务】** 本节的断言纪律：凡 regex / git / 机械门 / 脚本**行为**断言 → 草稿即附实测（命令 + 关键输出 verbatim + 日期）；凡「全 / 均 / 无」量化声明 → 现跑取证；凡数字引用 → 起草期现跑实测（本批三处 census 均为现跑，命令与输出见各 T 证据段）。

**【硬项 · 目标文件锚清单】** 每 T 末尾显式列「目标文件：<仓根相对路径清单>」。packet-binding 完整性断言（landing-manifest ≡ 各 T 目标文件并集）依赖本清单；**缺锚清单 = 门 RED 点名缺锚**。改法标题一律 `### T<n> · <标题>` 标题形态；landing-manifest 围栏块一行一个裸路径。

**条目形态（五段固定式）**：见 `docs/templates/PROPOSAL.template.md` §2「条目形态」——**问题 / 证据 / 根因 / 改法 / 〔第五段·锚清单〕**。

**【起草期自检两句】** ① FROZEN 标注前跑 `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL 路径>` 并通过 §5 验证集命令实跑；② 本批不涉路径类变更，族谱表不适用。

### T1 · 先例判定认不出本仓的主流合并形态

**问题**：`brief-inputs.mjs` 的「同 subsystem 先例」封顶 3 那条路，靠 `isMerged()` 判断一个包是否已合并。它只认两种痕迹：监督 verdict 判 `APPROVE`、或 pod-events 事件名里带 `merge`。而**本仓的主流合并形态是「监督判 ESCALATE + 人批」**——该形态的事件是 `eventType: "human-decision"` 且 `payload.decision = "approve-escalated-merge"`，**两种痕迹一个都不占**。影响面：起包时给子 agent 生成的输入清单里，「取最新先例」这个目的**未达成**（先例集恒给陈旧包）。**方向是 fail-closed**（少读不误扩阅读面、不放松任何门），故非缺陷、是**目的未达**。

**证据**（全部亲核，2026-09-20）：
- 现行两判定源（verbatim）：`.claude/scripts/brief-inputs.mjs:208-210` = 读 `delivery/changes/dev/<id>/supervisor-verdict.json` 的 `verdict`，`if (String(verdict).toUpperCase() === 'APPROVE') return true;`；`:219-221` = 扫 `delivery/taskpackages/<id>/pod-events.jsonl`，`/"eventType"\s*:\s*"merge-(completed|merged)"/` 与 `/"eventType"\s*:\s*"[^"]*merge[^"]*"/`。**两处都不认 `human-decision`**。
- **主流形态的实据**（现跑枚举）：`grep -rln "approve-escalated-merge" delivery/taskpackages/*/pod-events.jsonl` 命中 **≥3 个包**（`260829-char-stroke` / `260829-hardware-validation` / `260829-home-p1` 等）——即该形态**确实是本仓主流**，非孤例。
- **识别率实测**（转引 + 复核）：`delivery/changes/arch/260908-gov-review-input-surface-frontload/landing-verdict.md` D1 记「现判只认出 4/19，而 `git log --merges` 实证 ≥8 包真经 merge-driver 合并」。本批起草期复核了该 D1 的**方向**（两源确不认 human-decision）属实。

**根因**：`isMerged` 的判定源按**早期 APPROVE 形态**写死；本仓主合并形态迁到「ESCALATE + 人批」后，**没有回写判定源**。**同类先例**：`ARQ-260920-changes-index-mergedinmain-stale`（`changes-index` 的 `mergedInMain` 守卫因 `'no'` 是真值而失效）——都是「读侧判据与事实脱节且静默」。

**改法**：在 `isMerged()` 现有两源之后**增第三判定源**——读该包 `pod-events.jsonl`，命中 `"eventType"\s*:\s*"human-decision"` **且同行** `payload.decision` 含 `approve-escalated-merge` 即判已合并。**不采用** `git log --merges` 作判定源（该 ARQ 候选里并列的第二形态）：它需 fork git，而 `isMerged` 现为**纯 fs 读**，引入子进程会把「先例路」的失败面从「文件缺失」扩到「git 不可用」，与既有降级纪律冲突；单一文件源已覆盖 ESCALATE + 人批这一**主**形态。`--self-check` 补**双夹具**（沿既有「current/ 排除」臂的同类正控形态）：ESCALATE + 人批形态的 decoy merged 包**须入选**、未 merged 包**须不入选**。方向 fail-closed 不变——第三源只**增加**可识别形态、不减少。

目标文件：`.claude/scripts/brief-inputs.mjs`（改）

### T2 · launch 前置清单写在册，但检查器不读它

**问题**：带「需修订」标记的 workstream 会在 `decision-coverage.json` 顶层写一份 `launchPreconditions`（launch 前置清单：哪些规范修订须先落地）。而 **`portfolio-reconcile.mjs --check-launch` 完全不读该键**——清单的核验**全靠人读**。影响面：launch 前置缺失或过期时 `--check-launch` **不报红**，兜底只剩人记得去看；凡 workstream 带「需修订」标注的场景均暴露。

**证据**（全部亲核，2026-09-20）：
- 检查器不读（现跑）：`grep -c "launchPreconditions" .claude/scripts/portfolio-reconcile.mjs` = **0**。
- 检查器现读什么（verbatim，`.claude/scripts/portfolio-reconcile.mjs:243-255`）：`checkLaunchReachability(projectDir, { changeId, baseRef, exists, show })` 依次核 `delivery/taskpackages/<id>/taskpackage.json`（blocking）、其 `origin.upstreamRef` 解析出的 PROPOSAL 路径（blocking）——**枚举中无 `decision-coverage.json`**。
- **该键真实存在**（现跑）：`delivery/workstreams/m1-home-qa/decision-coverage.json` 顶层带 `launchPreconditions`，实测为**数组、2 条**（首条为「F-03 commit-before-launch…」）——即「有键在册而无人读」**是实况、非假设**。

**根因**：260912 拆包链批落的 convention 通道**要求写该键 + `pod-coordination.md` §2b 核验条款**，但**未落读取面**（260913 human 三选一裁定「机械齿不做、留根」）。**同类先例**：T1——都是「**写在册了、但读侧没有对应的判定**」。

**改法**：改 `checkLaunchReachability()`——在既有枚举之后**增读** workstream 的 `decision-coverage.json` 顶层键 `launchPreconditions`：**键在**则逐条核验对应 spec 修订已落，缺即报红；**键不在**视为无前置（**向后兼容既有 workstream**，本仓现存不带该键者行为零变更）。**边界**：本 T 只让既有检查器**多读一个键**，不改 `--check-launch` 的既有判据、不新增门。`--self-check` 补两夹具：带键且前置未落 → 报红；带键且前置已落 / 无键 → 不报。

目标文件：`.claude/scripts/portfolio-reconcile.mjs`（改）

### T3 · 簿记写入器记不到「这一派发跑在哪个模型上」（子项 b 经实测缩圈移出）

**问题**：`dispatch-stamp.mjs` 写 `dispatch-completed` 事件时，payload 里**没有任何模型维度**。影响面：`ARQ-260909-dispatch-stamp-telemetry-gaps` 记的「provider 槽位映射失真（260903 期间 opus/sonnet 槽位曾映射 `deepseek-v4-flash`，两包数据模型标签失真）」**无法事后判读**；且 `planner.md:57` MODEL-TIER 条款要求「dispatch-stamp 若可得须补记实际生效模型」——该义务**当前无处落**。

**证据**（全部亲核，2026-09-20）：
- payload 构造（verbatim，`.claude/scripts/dispatch-stamp.mjs:205-215`）：`payload: { dispatchId, phase, durationMs, toolUses, terminalStatus, tokenUsage }`——**枚举中无 model**。
- 全文无模型维度（现跑）：`grep -ci "model" .claude/scripts/dispatch-stamp.mjs` = **0**；`grep -c "flagVal('--model')" ` = **0**。
- **真实数据普查**（现跑，脚本枚举 `delivery/taskpackages/*/pod-events.jsonl` 全部 `dispatch-completed`）：**192 条，其中带 `model` 字段者 0 条**——缺口是**全覆盖**的、非抽样偏差。

**子项 b 的缩圈（起草期实测，**改法不再包含它**）**：该 ARQ 的子项 b =「token input/output/cache-read/cache-write 分项入 payload」。**实测该机制已经存在**——`buildTokenUsage()`（`.claude/scripts/dispatch-stamp.mjs:99-123`）已支持 `--token-input` / `--token-output` / `--token-cache-read` / `--token-cache-write` 四个 flag 并写入 `tu.input` 等键（260909 批落地）。真实数据 192 条中 `status='exact'` 者 **126 条**，其中**带非零分项者仅 4 条**（样例：`{"status":"exact","source":"harness","input":199198}`；无分项样例如 `{"status":"exact","source":"harness","total":4284}`）。⇒ **b 的缺口是「调用方未喂值」，不是「stamp 机制缺失」**；而调用方是否有分项数据属 **harness 侧可观测性**（同一 ARQ 的子项 c 已如实标注该分叉、留 full 档）。故 **b 随本批缩圈移出、并入 §6 留根②**，本 T 只做 a。

**根因**：payload 构造写于「只有 duration/toolUses 记账」的年代，**模型维度当时不在记账范围内**；MODEL-TIER 条款（后立）要求补记，**但没落到 stamp 的字段面上**。**同类先例**：T1/T2——**都是「形态/义务前进了一步，读侧或写侧没跟」**。

**改法**：给 `dispatch-stamp.mjs` 新增**可选** flag `--model <id>`：传了则写入 `payload.model`；**不传则不带该键**（向后兼容——既有调用方行为零变更，且不制造「0 充数」式的伪值）。同批在 `planner.md` MODEL-TIER 条款处**不加引用行**（不动 agent def，见 §6 留根③）。**本 T 不做**：自动探测当前模型（harness 未回传，属子项 c 的分叉面、不在本批）。

目标文件：`.claude/scripts/dispatch-stamp.mjs`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 输入面生成器 | `.claude/scripts/brief-inputs.mjs` | G3（advisory 生成器，**非门**） | T1 增第三判定源。消费方 = planner Step 7 双 brief、`--self-check` |
| launch 检查器 | `.claude/scripts/portfolio-reconcile.mjs` | G3（**检查器扩读，如实声明**） | T2 读 `launchPreconditions`。消费方 = `kidpal-pod-launch-prep` skill 的 `--check-launch` 步、`--self-check` |
| 簿记写入器 | `.claude/scripts/dispatch-stamp.mjs` | G3 | T3 增可选 `--model`。消费方 = planner Step 9 `[DISPATCH-STAMP]` 纪律、`pod-events-aggregate` 聚合面 |
| 契约面 / G1 / G2 / agent frontmatter | — | **未触** | 零改动 |
| 工程门新增 | — | **零新增** | 三条均只改既有脚本的判定源/字段面，不改任何门 |

**【硬项 · sweep 透镜义务】** 本批**不引入新门、不引入新判定 pattern**（T1/T2 是给既有判据**增源**、T3 是给既有 payload **增可选字段**），故无 pattern 全仓预跑义务。**但 census 义务已履行**（三处，均现跑 + 留痕）：① T1 的 `approve-escalated-merge` 包枚举（`grep -rln`，≥3 包）；② T2 的 `launchPreconditions` 消费者枚举（`grep -c` = 0）与供给者实例（`m1-home-qa/decision-coverage.json` 实测带键）；③ T3 的 `dispatch-completed` 全量普查（192 条 / model 0 条 / exact 126 条 / 分项非零 4 条）。

**【硬项 · 复扫模式完备性先验】** 本批**不涉退役语面 / 残留清扫**，无清扫模式集。

## 4. human 取舍点（OQ）

### OQ1 · T2 的「前置未落」如何判

**背景**：`launchPreconditions` 是**自由文本数组**（实测：m1-home-qa 的两条各是一段完整散文，含 `F-03`、`门 1` 等内部指称），**不是结构化字段**。判「该前置是否已落」需要从文本里取出可判据的锚（如 `F-03` 这类编号），而**该取法本身是个设计选择**。

**(a) 只核「键在不在」**（推荐）：`--check-launch` 只报「该 workstream 有 N 条 launch 前置待人工核」，**不判逐条落没落**。理由：把「判前置是否已落」留给读清单的人——脚本只负责**让清单不可被忽略**（治的正是「没人读」）；零假判风险。
**(b) 逐条机械核**：从每条前置文本里抽取可识别锚（如 `F-03`）并核其对应 spec 修订是否已落。理由：更接近「机械齿」；**代价**：抽锚规则要对自由文本作假设，**规则本身会成为新的失真源**（自由文本的形态不受控）。
**(c) 折中**：脚本报「键在 + 条数」，并**列出各条首 40 字**供人扫读，不做任何判定。

**这项为什么该 human 裁**：选 (b) 会让脚本**对自由文本作判定**——这是「机械齿」与「文本脆弱性」之间的取舍，属判据设计选择，不是实现细节。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**【真空绿规约】** V1/V2/V3 为 `--self-check`，其**新增夹具须在输出中具名出现**（防「加了夹具没接线」）；V4 为探针类，须覆盖「传 flag / 不传 flag」两态。

| 项 | 命令 | 预期 |
|---|---|---|
| V1 | `node .claude/scripts/brief-inputs.mjs --self-check` | 全绿 + 新增双夹具具名（ESCALATE + 人批 decoy 须入选 / 未 merged 须不入选） |
| V2 | `node .claude/scripts/portfolio-reconcile.mjs --self-check` | 全绿 + 新增两夹具具名（带键未落 → 报 / 无键 → 不报） |
| V3 | **三路探针**（`dispatch-stamp.mjs` **现无 `--self-check` 入口**，起草期已核实，故不写自检、改实跑探针；为避写副作用，用不匹配的 `--expect-change-id` 触发提前退出） | ① `grep -c "flagVal('--model')"` ≥1；② 带 `--model <id>` 跑 → 该次事件 payload **含** `model`；③ 不带 → payload **不带** `model` 键（向后兼容） |
| V4 | `grep -c "launchPreconditions" .claude/scripts/portfolio-reconcile.mjs` | 由 **0** 变为 ≥1 |
| V5 | `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` | 全绿；lint 只允许既有告警（`landing-verifier.md` 超长，本批未触该文件） |
| V6 | `git diff --stat .claude/scripts/` | 恰为本批 manifest 三路径，无夹带 |
| V7 | `landing-fidelity-check.mjs <本批>` 结构门 | §2 锚集 ≡ manifest 机判通过（本批 3 路径） |

## 6. 留根清单

1. **`ARQ-260909-dispatch-stamp-telemetry-gaps` 的子项 b（token 分项）**——**本批缩圈移出**：机制已在（`buildTokenUsage` 支持四 flag、126/192 为 exact），缺口是**调用方未喂值**；而调用方有无分项数据属 harness 侧可观测性。**留根**：随子项 c（主线程自身观测，已如实标 full-adversarial 分叉）一并处置；或由 harness 侧能力变化触发。
2. **`ARQ-260909-dispatch-stamp-telemetry-gaps` 的子项 c（主线程自身消耗观测）**——原 ARQ 已如实标「**含真设计分叉**」（harness 无回传，估算 vs 不采 待定）。本批不触；触及时该子项转 `full-adversarial + human`。
3. **MODEL-TIER 条款的引用行**——`planner.md:57` 要求「dispatch-stamp 若可得须补记实际生效模型」，本批只落**字段面**（T3），**未在 `planner.md` 加引用行**（动 agent def 触 self-def 判断点）。**留根**：待该判断点摊开时随批补。
4. **T1 的 `git log --merges` 判定源形态**——ARQ 候选并列给出的第二形态，本批**未采用**（理由见 T1 改法）。**留根**：若将来出现「pod-events 文件缺失但包确已合并」的实况，再评估补该源。
5. **本批与 `ARQ-260909-dispatch-stamp-telemetry-gaps` 原行的口径差**——该 ARQ 行记「97 次派发仅 44 次 exact」，本批现跑为「**192 条 dispatch-completed，exact 126 条**」（口径与包范围均不同：该行是 13 包复盘时点、本批是全仓现跑）。**该 ARQ 行已按其原口径在册，本批不动它**；如需对齐口径，随 T3 落地时在队列行追加实测注记。

**交日落复盘项**：T1 落地后，建议 `/loop-retro` 观察——同 subsystem 先例集是否真变新（可对某包跑 `--dry-run` 看先例选取结果与人工判断是否一致）。

**自律件**：起草期三处 census 均为现跑（命令与输出见 §2 各 T 证据段）；**T3 的子项 b 是起草期实测**缩圈**的结果**（机制已在、缺口在调用方），非作者判断。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）