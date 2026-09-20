# PROPOSAL · 260920-gov-review-reader-scripts-align

> **状态：FROZEN v1.0 · 2026-09-20 · 可进 landing（暂压着，待 human 阅后再落）**（凭据：human 2026-09-20 立项 ack + **architect R2 GREEN**〔§7 `### R2 - 2026-09-20 - architect`，附 2 条 NIT F5/F6，已在标冻前折入〕；**本批无 OQ**——两条改法均无真设计分叉，故无 human OQ 裁断项）。
> **承载 ARQ**：`ARQ-260909-precedent-merged-detection-gap`（T1）、`ARQ-260909-dispatch-stamp-telemetry-gaps`（T2，**子项 a；子项 b 经实测缩圈**）。**原 T2〔`ARQ-260914-portfolio-reconcile-launchpreconditions`〕已按 human 2026-09-20 裁定移出本批**——该条须先定**前置清单的数据模型**（见 §6 留根①），并入「拆包 / 输入 / launch prompt 流程化」批。不关的根写 §6 留根指针。
> **本批定位**：两条都是**读侧/写侧与事实不对齐**——脚本按早期形态写死了判定源或字段，形态前进了一步、读侧没跟；且都**不改任何门的判定语义**（改的是一个 advisory 生成器的判定源、一个簿记写入器的可选字段）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。两条升级判据（触契约面 / 跨系统结构性变更）均未命中。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T2）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low（一句话证成：两条均为**读侧/写侧对齐**——T1 给一个 advisory 生成器补一个判定源（方向 fail-closed）、T2 给一个簿记写入器补一个**可选**字段（不传则不带该键）。无闸判定语义改动、无放松面、无数据丢失形态、**无数据模型改动**）。
> **review profile**：G3（`.claude/scripts/` 两个脚本）。契约面 / G1 / G2 / agent frontmatter 均为零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/brief-inputs.mjs
.claude/scripts/dispatch-stamp.mjs
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：让两个脚本**读到/记到的东西与事实对齐**——先例判定认得出本仓的主流合并形态、簿记写入器记得到「这一派发实际跑在哪个模型上」。

**现状差距**：两条形态一样——**脚本按早期形态写死了判定源/字段，形态变了之后没人回写**。后果都是**静默**：不报错、不告警，只是读出来/记下来的不是真的（先例集给陈旧包、模型维度永远空白）。

**非目标（本批不做什么）**：不改任何门的**判定语义**（T1 是 advisory 生成器、T2 只加一个**可选**字段）；不新增工程门；不动 `docs/contracts/**` 与系统 spec 两文件；不动任何 agent frontmatter；**不并入 `ARQ-260909-dispatch-stamp-telemetry-gaps` 的子项 b**（经起草期实测**缩圈移出**，见 T2）；**不并入 `ARQ-260914-portfolio-reconcile-launchpreconditions`**（已移出，见 §6 留根①）。

## 2. 问题、根因与改法（T1…T2）

**【硬项 · 起草期实证义务】** 本节的断言纪律：凡 regex / git / 机械门 / 脚本**行为**断言 → 草稿即附实测（命令 + 关键输出 verbatim + 日期）；凡「全 / 均 / 无」量化声明 → 现跑取证；凡数字引用 → 起草期现跑实测（本批两处 census 均为现跑，命令与输出见各 T 证据段）。

**【硬项 · 目标文件锚清单】** 每 T 末尾显式列「目标文件：<仓根相对路径清单>」。packet-binding 完整性断言（landing-manifest ≡ 各 T 目标文件并集）依赖本清单；**缺锚清单 = 门 RED 点名缺锚**。改法标题一律 `### T<n> · <标题>` 标题形态；landing-manifest 围栏块一行一个裸路径。

**条目形态（五段固定式）**：见 `docs/templates/PROPOSAL.template.md` §2「条目形态」——**问题 / 证据 / 根因 / 改法 / 〔第五段·锚清单〕**。

**【起草期自检两句】** ① FROZEN 标注前跑 `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL 路径>` 并通过 §5 验证集命令实跑；② 本批不涉路径类变更，族谱表不适用。

### T1 · 先例判定认不出本仓的主流合并形态

**问题**：`brief-inputs.mjs` 的「同 subsystem 先例」封顶 3 那条路，靠 `isMerged()` 判断一个包是否已合并。它只认两种痕迹：监督 verdict 判 `APPROVE`、或 pod-events 事件名里带 `merge`。而**本仓的主流合并形态是「监督判 ESCALATE + 人批」**——该形态的事件是 `eventType: "human-decision"` 且 `payload.decision = "approve-escalated-merge"`，**两种痕迹一个都不占**。影响面（R1-F2 校准后的精确口径，2026-09-20 复跑）：起包时给子 agent 生成的输入清单里，「取最新先例」这个目的**部分未达成**——**34** 个候选包（`current/` 指针**不计入**，与先例路的排除语义一致；R2-F5 更正：v0.2/v0.3 原写 35 系把 `current/` 计入分母）中两源已认出 **21** 个（merge-driver 的 `merge-completed` 事件**被第二源 regex 认得**），带 `approve-escalated-merge` 者 **24** 个，故**第三源的净增益 = 7 个存量包**（清单见下证据段）。**R1-F2 更正**：v0.2 原写「先例集**恒**给陈旧包」**属过度陈述**，实际是 7 个包的缺口。**方向是 fail-closed**（少读不误扩阅读面、不放松任何门），故非缺陷、是**目的部分未达**。

**证据**（全部亲核，2026-09-20）：
- 现行两判定源（verbatim）：`.claude/scripts/brief-inputs.mjs:210-211` = 读 `delivery/changes/dev/<id>/supervisor-verdict.json` 的 `verdict`，`if (String(verdict).toUpperCase() === 'APPROVE') return true;`；`:218-219` = 扫 `delivery/taskpackages/<id>/pod-events.jsonl`，`/"eventType"\s*:\s*"merge-(completed|merged)"/` 与 `/"eventType"\s*:\s*"[^"]*merge[^"]*"/`。**两处都不认 `human-decision`**（行号经 R1-F4① 更正：v0.2 原写 `:208-210` / `:219-221`）。
- **主流形态的实据**（现跑枚举）：`grep -rln "approve-escalated-merge" delivery/taskpackages/*/pod-events.jsonl` 命中 **24 个包**（R1 复跑；`260829-char-stroke` / `260829-hardware-validation` / `260829-home-p1` 等）——即该形态**确实是本仓主流**，非孤例。
- **净增益清点**（现跑，逐包核「两源是否已认出」）：候选包 **34**（`current/` 不计入；R2-F5 更正）、两源合计认出 **21**、带 `approve-escalated-merge` 者 **24** ⇒ **净增益 7 个存量包** = `260829-home-p1` / `260829-schedule-p2` / `260829-session-logic` / `260829-smoke-e2e` / `260829-voice-roundtrip` / `260903-device-ws-switch` / `260907-g-relay-fix`。（R1-F2 独立复跑同得 7，并指出该集恰为 `260908-gov-review-input-surface-frontload/landing-verdict.md` **§5 残余风险 (a) `:83`** 与 **§4 表行 4** 点名的那批。）
- **识别率原记**（转引 + 复核）：该 landing-verdict 记「现判只认出 4/19」。**R1-F4⑥ 更正**：v0.2 把该断言引作「**D1**」——该文**无 D1 标签**（按 § 编号），实在 **§4 表行 4** 与 **§5 残余风险 (a) `:83`**；「D1」是 ARQ 行自己的简写。本批起草期复核了其**方向**（两源确不认 human-decision）属实；**唯一数字以本段现跑为准**（4/19 为彼时时点值）。

**根因**：`isMerged` 的判定源按**早期 APPROVE 形态**写死；本仓主合并形态迁到「ESCALATE + 人批」后，**没有回写判定源**。**同类先例**：`ARQ-260920-changes-index-mergedinmain-stale`（`changes-index` 的 `mergedInMain` 守卫因 `'no'` 是真值而失效）——都是「读侧判据与事实脱节且静默」。

**改法**：在 `isMerged()` 现有两源之后**增第三判定源**——读该包 `pod-events.jsonl`，命中 `"eventType"\s*:\s*"human-decision"` **且同行** `payload.decision` 含 `approve-escalated-merge` 即判已合并。**不采用** `git log --merges` 作判定源（该 ARQ 候选里并列的第二形态）：它需 fork git，而 `isMerged` 现为**纯 fs 读**，引入子进程会把「先例路」的失败面从「文件缺失」扩到「git 不可用」，与既有降级纪律冲突；单一文件源已覆盖 ESCALATE + 人批这一**主**形态。`--self-check` 补**双夹具**（沿既有「current/ 排除」臂的同类正控形态）：ESCALATE + 人批形态的 decoy merged 包**须入选**、未 merged 包**须不入选**。方向 fail-closed 不变——第三源只**增加**可识别形态、不减少。

目标文件：`.claude/scripts/brief-inputs.mjs`（改）

### T2 · 簿记写入器记不到「这一派发跑在哪个模型上」（子项 b 经实测缩圈移出）

**问题**：`dispatch-stamp.mjs` 写 `dispatch-completed` 事件时，payload 里**没有任何模型维度**。影响面：`ARQ-260909-dispatch-stamp-telemetry-gaps` 记的「provider 槽位映射失真（260903 期间 opus/sonnet 槽位曾映射 `deepseek-v4-flash`，两包数据模型标签失真）」**无法事后判读**；且 `planner.md:59` MODEL-TIER 条款要求「dispatch-stamp 若可得须补记实际生效模型」——该义务**当前无处落**。（R1-F4⑤ 更正：v0.2 原写 `planner.md:57`，实际在 `:59`；§6 留根④ 同步更正。）

**证据**（全部亲核，2026-09-20；**行号经 R1-F4③④ 更正**）：
- payload 构造（verbatim，`.claude/scripts/dispatch-stamp.mjs:210-217`）：`payload: { dispatchId, phase, durationMs, toolUses, terminalStatus, tokenUsage }`——**枚举中无 model**。（v0.2 原写 `:205-215`。）
- 全文无模型维度（现跑）：`grep -ci "model" .claude/scripts/dispatch-stamp.mjs` = **0**；`grep -c "flagVal('--model')"` = **0**。
- **真实数据普查**（现跑，脚本枚举 `delivery/taskpackages/*/pod-events.jsonl` 全部 `dispatch-completed`）：**192 条，其中带 `model` 字段者 0 条**——缺口是**全覆盖**的、非抽样偏差。
  **R1-F3 时点注记**：该 192 为**起草期时点值**；R1 复跑得 **195 条**（delta = `260920-m2-charwindow-card-fix` 当日 3 条 `dispatch-completed`，其闭环 commit `adc0241` 晚于本 census 时点）。**承重结论不随时点变**：`payload.model` **0 条**、exact 中非零分项**恰 4 条**、样例 `{"status":"exact","source":"harness","input":199198}` 逐字命中。**判据是「model 字段 0 条」这一全覆盖事实，不是具体条数**——后续核验请按此口径，勿按 192 字面复跑。

**子项 b 的缩圈（起草期实测，改法不再包含它）**：该 ARQ 的子项 b =「token input/output/cache-read/cache-write 分项入 payload」。**实测该机制已经存在**——`buildTokenUsage()`（`.claude/scripts/dispatch-stamp.mjs:99-156`，四 flag 支持在 `:105-113`；R1-F4③ 更正：v0.2 原写 `:99-123`）已支持 `--token-input` / `--token-output` / `--token-cache-read` / `--token-cache-write` 四个 flag 并写入 `tu.input` 等键（260909 批落地）。真实数据 192 条中 `status='exact'` 者 **126 条**（R1 复跑 128，同 F3 时点漂移），其中**带非零分项者仅 4 条**（样例：`{"status":"exact","source":"harness","input":199198}`；无分项样例如 `{"status":"exact","source":"harness","total":4284}`）。⇒ **b 的缺口是「调用方未喂值」，不是「stamp 机制缺失」**；而调用方是否有分项数据属 **harness 侧可观测性**（同一 ARQ 的子项 c 已如实标注该分叉、留 full 档）。故 **b 随本批缩圈移出、并入 §6 留根②**，本 T 只做 a。

**根因**：payload 构造写于「只有 duration/toolUses 记账」的年代，**模型维度当时不在记账范围内**；MODEL-TIER 条款（后立）要求补记，**但没落到 stamp 的字段面上**。**同类先例**：T1——都是「形态/义务前进了一步，读侧或写侧没跟」。

**改法**：给 `dispatch-stamp.mjs` 新增**可选** flag `--model <id>`：传了则写入 `payload.model`；**不传则不带该键**（向后兼容——既有调用方行为零变更，且不制造「0 充数」式的伪值）。同批**不在 `planner.md` 加引用行**（不动 agent def，见 §6 留根③）。**本 T 不做**：自动探测当前模型（harness 未回传，属子项 c 的分叉面、不在本批）。

目标文件：`.claude/scripts/dispatch-stamp.mjs`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 输入面生成器 | `.claude/scripts/brief-inputs.mjs` | G3（advisory 生成器，**非门**） | T1 增第三判定源。消费方 = planner Step 7 双 brief、`--self-check` |
| 簿记写入器 | `.claude/scripts/dispatch-stamp.mjs` | G3 | T2 增可选 `--model`。消费方 = planner Step 9 `[DISPATCH-STAMP]` 纪律、`pod-events-aggregate` 聚合面 |
| 契约面 / G1 / G2 / agent frontmatter | — | **未触** | 零改动 |
| launch 检查器 | `.claude/scripts/portfolio-reconcile.mjs` | **未触**（v0.2 移出） | 原 T2〔launchPreconditions〕已移出本批，见 §6 留根① |
| 工程门新增 | — | **零新增** | 两条均只改既有脚本的判定源/字段面，不改任何门 |

**【硬项 · sweep 透镜义务】** 本批**不引入新门、不引入新判定 pattern**（T1 是给既有判据**增源**、T2 是给既有 payload **增可选字段**），故无 pattern 全仓预跑义务。**但 census 义务已履行**（两处，均现跑 + 留痕）：① T1 的净增益清点（**34** 候选〔`current/` 不计入〕/ 两源认出 21 / 带 `approve-escalated-merge` 24 / **净增益 7**，见 T1 证据段）；② T2 的 `dispatch-completed` 全量普查（**192 条**〔起草期时点；R1 复跑 195，见 T2 的 R1-F3 时点注记〕/ model **0** 条 / exact **126** 条 / 分项非零 **4** 条）。

**【硬项 · 复扫模式完备性先验】** 本批**不涉退役语面 / 残留清扫**，无清扫模式集。

## 4. human 取舍点（OQ）

**本批无 OQ。** 两条的改法均**无真设计分叉**：T1 是「加一个判定源、方向 fail-closed」，T2 是「加一个可选字段、不传则不带键」。

**v0.2 的 OQ 清零说明**：v0.1 的 OQ1（「T2〔launchPreconditions〕的前置未落如何判」）**随该 T 移出而作废**——该问题在起草期被暴露为**比本批定位更大的设计问题**：前置清单是**无字段的字符串数组**，既**没有「核过没」位**（脚本会永久重复报已办条目）、也**没有「属哪个包」位**（而 `--check-launch` 是 per-package，会把 workstream 级、甚至关于别包的条目录给每个包）。⇒ 它须**先定数据模型**，见 §6 留根①。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**【真空绿规约】** V1 为 `--self-check`，其**新增夹具须在输出中具名出现**（防「加了夹具没接线」）；V2 为探针类，须覆盖「传 flag / 不传 flag」两态。

| 项 | 命令 | 预期 |
|---|---|---|
| V1 | `node .claude/scripts/brief-inputs.mjs --self-check` | 全绿 + 新增双夹具具名（ESCALATE + 人批 decoy 须入选 / 未 merged 须不入选） |
| V2 | **三路探针 · temp projectDir 模式**（**R1-F1 采纳**——v0.2 原设计「用不匹配的 `--expect-change-id` 触发提前退出」**是假绿**：该 fail-loud 在 `dispatch-stamp.mjs:188-191`、**先于** `appendPodEvent` **调用**（`:206-218`；R2-F6② 更正：v0.3 把 `:210-217` 挂在调用上，该区间实为**payload 字面量**、调用在 `:206-218`，二者「先于」判定不受影响），事件根本不落盘 ⇒ ②③ 无可观察对象。**修法取 temp projectDir**：在临时目录建 `delivery/taskpackages/<探针包 id>/`——**探针包 id 须带 ≥`260725` 的 YYMMDD 日期前缀**（R2-F6① 钉死：`isPostEventsCutoff` 在 `:81-85` 按 `CHANGE_ID_PREFIX_RE` 取前缀、`EVENTS_SCHEMA_CUTOFF` = `2026-07-25`；**前缀晚于该日才走 pod-events 分支**，否则落回 legacy `dispatch-log.jsonl` 路径、探针读错文件），**前缀选错会响亮失败而非假绿**；以 `CLAUDE_PROJECT_DIR=<tempDir>` 跑**真实调用**、读 tempDir 下的 `pod-events.jsonl`，用毕 `rm -rf` 并**断言目录已不存在**。**起草期已实证该形态可行**——temp dir 建包 → 事件落 temp dir → 清理成功，故非纸面设计） | ① `grep -c "flagVal('--model')" .claude/scripts/dispatch-stamp.mjs` ≥1（入口在场）；② 带 `--model <id>` 跑 → **读 tempDir 下 `pod-events.jsonl` 的该条 `dispatch-completed`**，其 `payload` **含** `model`；③ 不带 `--model` 跑 → 同法读该条，`payload` **不带** `model` 键（向后兼容）。三路各留完整输出 |
| V3 | `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` | 全绿；lint 只允许既有告警（`landing-verifier.md` 超长，本批未触该文件） |
| V4 | `git diff --stat .claude/scripts/` | 恰为本批 manifest 两路径，无夹带 |
| V5 | `landing-fidelity-check.mjs <本批>` 结构门 | §2 锚集 ≡ manifest 机判通过（本批 2 路径） |

## 6. 留根清单

1. **`ARQ-260914-portfolio-reconcile-launchpreconditions`——本批移出，须先定数据模型**（human 2026-09-20 裁定：并入「**拆包 / 输入 / launch prompt 流程化**」批）

   起草期把它写成 OQ 后，人类读者两问暴露其**真实规模**（均经实测成立）：

   - **问一「已经核了，脚本知道吗？会再报吗？」→ 不知道、会永远重复报。** 实测：`launchPreconditions` 是**纯字符串数组**（`delivery/workstreams/m1-home-qa/decision-coverage.json`，6 条，每条一整段散文），**没有任何「已核」位**；而门 1 的核验结果落在**另一个文件** `review.md`（`## 门 1（拆包 review 门）—— 2026-09-15` + 五项逐项结论），脚本不读。⇒ 单做「报 N 条待核」会产**永久噪音**。
   - **问二「人拍的事可能不是本包的，会显示出来吗？」→ 会，且是全量。** 实测：`launchPreconditions` 是 **workstream 级**（一份、5 包共享），而 `--check-launch <changeId>` 是 **per-package**；看实际内容，【3】页点 3/4 是 **keybar-three-slot 那个包**的事、【4】制约的是 **m1-k4**（另一个包）。⇒ 每个包点火都会看到全部 6 条、其中几条与自己无关。
   - **结论**：两缺口指向**清单缺两个位**——「核过没」与「属哪个包」。**同文件已有先例**：`decisionCoverage` 的每条都带 `landsIn: "package:<id>"`（正是问二需要的位）——`launchPreconditions` 只是没跟上。**故这不是「脚本读不读」的问题，是前置清单的数据模型问题**；改模型要动在册 workstream（迁移或兼容），超出本批「读侧对齐、零设计分叉」的定位。
   - **包 → workstream 的链接已具备**（实测：`taskpackages/260915-m1-k2-keybar-three-slot/taskpackage.json` 的 `origin.workstream = "m1-home-qa"`）⇒ 按包过滤在技术上可行，缺的只是逐条的 `scope`/`status` 位。
   - **交接给下一批的动作**：先定数据模型（`launchPreconditions` 由 `string[]` 改为 `{text, scope, status}[]`，或加平行已核位），再落 `--check-launch` 的读取与过滤；**与用户的「拆包 / 输入 / launch prompt 流程化」一并做**。

2. **`ARQ-260909-dispatch-stamp-telemetry-gaps` 的子项 b（token 分项）**——**本批缩圈移出**：机制已在（`buildTokenUsage` 支持四 flag、126/192 为 exact），缺口是**调用方未喂值**；而调用方有无分项数据属 harness 侧可观测性。**留根**：随子项 c 一并处置，或由 harness 侧能力变化触发。
3. **`ARQ-260909-dispatch-stamp-telemetry-gaps` 的子项 c（主线程自身消耗观测）**——原 ARQ 已如实标「**含真设计分叉**」（harness 无回传，估算 vs 不采 待定）。本批不触；触及时该子项转 `full-adversarial + human`。
4. **MODEL-TIER 条款的引用行**——`planner.md:59`（R1-F4⑤ 更正：v0.2 原写 `:57`）要求「dispatch-stamp 若可得须补记实际生效模型」，本批只落**字段面**（T2），**未在 `planner.md` 加引用行**（动 agent def 触 self-def 判断点）。**留根**：待该判断点摊开时随批补。
5. **T1 的 `git log --merges` 判定源形态**——ARQ 候选并列给出的第二形态，本批**未采用**（理由见 T1 改法）。**留根**：若将来出现「pod-events 文件缺失但包确已合并」的实况，再评估补该源。
6. **本批与 `ARQ-260909-dispatch-stamp-telemetry-gaps` 原行的口径差**——该 ARQ 行记「97 次派发仅 44 次 exact」，本批现跑为「**192 条 dispatch-completed，exact 126 条**」（口径与包范围均不同：该行是 13 包复盘时点、本批是全仓现跑）。**该 ARQ 行已按其原口径在册，本批不动它**；如需对齐口径，随 T2 落地时在队列行追加实测注记。

**交日落复盘项**：T1 落地后，建议 `/loop-retro` 观察——同 subsystem 先例集是否真变新（可对某包跑 `--dry-run` 看先例选取结果与人工判断是否一致）。

**自律件**：起草期两处 census 均为现跑（命令与输出见 §2 各 T 证据段）；**T2 的子项 b 缩圈、T2〔launchPreconditions〕移出**均为实测驱动，非作者判断。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-20 - architect

**Verdict：`BLOCK(F1–F4)`**——F1 BLOCKING、F2 SHOULD、F3/F4 NIT。**无「影响目标效果」级方向性错误，故无升级 human 项**：两条改法方向均成立、两处缩圈均复核站得住、无 OQ 成立；唯一 BLOCKING 项是验证集 V2 的探针设计不可执行，主线程修 V2 措辞（或补自检入口）即可，不动 T1/T2 改法本体。

**Findings：**

- **F1（BLOCKING）· V2 探针机制自相矛盾，②③ 无可观察对象。** V2 括注指定「为避写副作用，用不匹配的 `--expect-change-id` 触发提前退出」，但亲核 `dispatch-stamp.mjs` 源码：`--expect-change-id` 不符的 fail-loud 在 `:188-191`（`process.exit(2)`），**先于** `appendPodEvent` 调用（`:206-218`，payload 字面量在内）——提前退出意味着事件**根本不落盘**，故 ②「该次事件 payload 含 `model`」与 ③「payload 不带 `model` 键」两判据没有任何可检查的落点；照字面跑只能得到「exit 2、stderr 无 model 字样」，形同假绿——真空绿规约在本项失效。修法方向（任一，V2 验证目标「传/不传两态」不变）：① 沿 `260908-gov-review-input-surface-frontload/landing-verdict.md` §6 破坏性自检的 temp 模式——临时 projectDir + **匹配的** `--expect-change-id` + 用毕清理并断言不存在；② 给 `dispatch-stamp.mjs` 补 `--self-check`（或 `--dry-run`，stderr echo payload、不落盘），与 `brief-inputs.mjs --self-check` 模式对齐。
- **F2（SHOULD）· T1 识别率叙述与现况脱节，「恒给陈旧包」绝对化。** 本判官现跑重算（34 候选包 = `acceptanceTestsFile` 非空）：现行两源已判 **21 包** merged——merge-driver 落 `merge-completed` 事件（实测样例 `260829-char-stroke` / `260915-m1-k2-keybar-three-slot` / `260917-m2-k3-char-window-overlay` 均有 `merge-completed actor=merge-driver`）已被第二源 regex `[^"]*merge[^"]*` 认得；**第三源净增益 = 7 个历史包**（`home-p1 / schedule-p2 / session-logic / smoke-e2e / voice-roundtrip / 260903-device-ws-switch / 260907-g-relay-fix`——恰为 landing-verdict §5(a) 点名集，即 merge 事件盖章惯例成形前的存量）。**改法方向仍成立**（fail-closed 增源、净增益真实、ARQ 点名的 7 包正是要治的），但 §0「先例集恒给陈旧包」与 T1 问题段的隐含链（主流形态 ⇒ 包不被识别）应按现跑数字校准：缺口是「7 个存量包」，非「主流形态整体不可识别」。`4/19` 是 260908 时点转引（PROPOSAL 已如实标注「转引 + 复核方向」），保留可以，但应并记现况数字（本判官方数：两源 21/34、第三源 24/34、并 28/34）。
- **F3（NIT）· T2 census 数字不复现（时点漂移）。** R1 复跑得 **195 条 / exact 128**（PROPOSAL：192/126）。delta = `260920-m2-charwindow-card-fix` 当日 3 条 `dispatch-completed`（ts `2026-09-20T04:58–05:35Z`，该包闭环 commit `adc0241` 13:39 +0800，晚于起草 census 时点）。承重结论全部成立：`payload.model` **0 条**（全覆盖缺口）、exact 中非零分项**恰 4 条**、样例 `{"status":"exact","source":"harness","input":199198}` 逐字命中（在 `260915-m1-k4-reminder-overlay-unify`）。建议在 T2 证据段或 §6 留根⑥加一句时点注记，防后续核验官按 192 字面复跑报红。
- **F4（NIT）· 引用精度漂移（6 处；verbatim 引文本身全部逐字吻合）。** ① `brief-inputs.mjs` APPROVE 判定实际 `:210-211`（文称 `:208-210`）；② merge 扫描实际 `:218-219`（文称 `:219-221`）；③ `buildTokenUsage()` 函数体实际 `:99-156`（文称 `:99-123`；四 flag 支持在 `:105-113`，语义成立）；④ payload 构造实际 `:210-217`（文称 `:205-215`）；⑤ `planner.md` MODEL-TIER 条款实际 `:59`（文称 `:57`，§6 留根④同）；⑥ T1 证据段引作「landing-verdict.md **D1**」——该文无 D1 标签（按 § 编号，4/19 断言实在 §4 表行 4 与 §5 残余风险 (a) `:83`；「D1」是 ARQ 行自己的简写）。落地前更正，防核验官按行号/标签机械对不上。

**「无 OQ」成立性核对**：T1 判定源选型（fs 源 vs `git log --merges`）有明确取舍理由（亲核 `brief-inputs.mjs` imports 仅 `node:fs`/`node:path` `:12-13`，无 child_process——「不采用」理由与源码相符）且留根⑤承接弃选形态；T2 可选 flag 无分叉。**无漏 OQ。**

**两处缩圈判断复核：均站得住。** ① T2 子项 b 缩圈——`buildTokenUsage` 四 flag 机制确在（`:105-113`）、192（现 195）条中 exact 126（现 128）条、非零分项仅 4 条 ⇒「缺口在调用方未喂值、非机制缺失」成立；② launchPreconditions 移出——亲核 `delivery/workstreams/m1-home-qa/decision-coverage.json:143-150` 实为 **6 条纯字符串数组**（无「已核」/「属哪个包」位）；门 1 结论在另一文件 `review.md:5`（`## 门 1（拆包 review 门）—— 2026-09-15`），脚本不读；同文件 `decisionCoverage` 条目带 `landsIn`（15 处，`package:<id>` 形态实测在）；`taskpackages/260915-m1-k2-keybar-three-slot/taskpackage.json` 的 `origin.workstream = "m1-home-qa"` 实测命中；`--check-launch` per-package（`portfolio-reconcile.mjs:327,354-362`）。「先定数据模型再动」的裁定有实测地基。

**其余亲核留痕（全部 2026-09-20 本 worktree 现跑）**：T1 两判定源 verbatim 与源码吻合（`isMerged()` `:205-223`）；`approve-escalated-merge` 包枚举重跑 `grep -rln` 命中 **24 包**（「≥3」成立、例举三包均在）；T2 全文无 model 维度（`grep -ci "model"` = 0、`flagVal('--model')` = 0）确认；消费面双向核对——`pod-events-append.mjs:59-60` payload 仅验「是 object」不验键集，`pod-events-aggregate.mjs:71,128-137` 与 `loop-metrics.mjs:239-243` 均守卫式属性访问 ⇒ 增/缺 `model` 键均无破坏，「不传则不带该键」向后兼容成立；planner `[DISPATCH-STAMP]` 纪律（Step 9）flag 集不含 `--model`，改动零影响；结构字段在场（landing-manifest 围栏块 2 路径 + packet-id 行），`landing-fidelity-check.mjs --preflight` 现跑 `{"ok":true,"anchorCount":2,"findings":[]}`，§2 锚集 ≡ manifest 两路径自洽；基线命令现跑——V1 `--self-check` 19/19 GREEN、V3 `route-regression-check` 28 ok 0 fail + `lint-descriptions` 仅既有 `landing-verifier.md` 超长告警（363>320），与 V3 预期相符；ARQ 两行原文（`docs/ARCH-REVIEW-QUEUE.md:49/51`）与承载关系、留根⑥口径差注记一致。

**结论**：修 F1（BLOCKING）+ 顺手 F2–F4 后可交 R2 聚焦复核（只核修订到位性与是否引入新矛盾，不重全量）。

### R2 - 2026-09-20 - architect

**Verdict：`GREEN`**（附 2 条 NIT 非阻塞更正，F5/F6——均为单行量级口径钉死，建议主线程标 FROZEN 前顺手改，改后无需再轮；不改判 GREEN。）R1 四 findings 修订到位性逐条核毕，T1/T2 改法本体两轮未动，无升级 human 项。

**R1 修订到位性（逐条亲核，全部 2026-09-20 现跑）：**

- **F1（BLOCKING）→ 已修，机制实证可行。** v0.3 V2 改 temp projectDir 探针。本判官**独立实跑**该形态（非采信自述）：`mktemp -d` 建临时目录 + `mkdir delivery/taskpackages/260990-r2-probe-pkg` + `CLAUDE_PROJECT_DIR=<tempDir> node .claude/scripts/dispatch-stamp.mjs … --task-package-id 260990-r2-probe-pkg` → 事件**确实落在 tempDir 的 pod-events.jsonl**，envelope 完整可读（`payload` 键集直接可判含/不含 `model`）——① 可观察性成立。② 残留写副作用 = 零：亲核 `pod-events-append.mjs:23`（`projectDir = process.env.CLAUDE_PROJECT_DIR || process.cwd()`，与 `dispatch-stamp.mjs:30` 同源）与 `:63/:83-84`（唯一写点 = `resolve(projectDir, 'delivery', 'taskpackages', changeId)/pod-events.jsonl` 的 `appendFileSync`，无别处状态/索引/锁文件）；实跑后仓内 `delivery/taskpackages/260990-r2-probe-pkg` 不存在、`git status --porcelain delivery/taskpackages/` 干净；`rm -rf` + `[ ! -d ]` 断言成立。
- **F2（SHOULD）→ 实质已修，分母引入一处小漂移（→F5）。** v0.3 口径「两源认出 **21** / 带 `approve-escalated-merge` **24** / **净增益 7** + 7 包逐个列名」与本判官 R2 复跑**逐数吻合**，7 包清单与 R1 独立清点**逐字一致**（`home-p1 / schedule-p2 / session-logic / smoke-e2e / voice-roundtrip / 260903-device-ws-switch / 260907-g-relay-fix`）；「恒给陈旧包→部分未达 + 过度陈述更正」的校准到位。唯「候选包 **35**」不复现：本判官按先例路同口径（`acceptanceTestsFile` 非空、排除 `current/`）复跑得 **34**——35 疑为把 `current/` 指针目录计进分母（`ls delivery/taskpackages | wc -l` = 35 = 34 + current；current/ 无 pod-events.jsonl，故探测数不受它影响、恰可解释 21/24/7 吻合而分母多 1）。见 F5。
- **F3（NIT）→ 已修。** T2 证据段与 §3 sweep 段均加时点注记（192 = 起草期时点、R1 复跑 195、delta = `260920-m2-charwindow-card-fix` 当日 3 条、闭环 commit `adc0241`），并显式声明「**判据是『model 字段 0 条』这一全覆盖事实，不是具体条数』——后续核验请按此口径，勿按 192 字面复跑」。该声明足以防字面复跑报红。子项 b 段的「126（R1 复跑 128，同 F3 时点漂移）」注记亦在场。
- **F4（NIT）→ 六处均已更正、未引入新漂移（V2 括注一处微标注 →F6 顺带）。** 逐处复核：① `:210-211` ✓（实际 verdict 取值 `:210`、APPROVE 判 `:211`）；② `:218-219` ✓（两 merge regex 实际行）；③ `:99-156` + 四 flag `:105-113` ✓（函数体实际 `:99-156`）；④ payload `:210-217` ✓（字面量实际 `:210-217`）；⑤ `planner.md:59` 于 T2 问题段与 §6 留根④ 双处同步 ✓；⑥ 「D1」→「§4 表行 4 与 §5 残余风险 (a) `:83`」 ✓（亲核该文无 D1 标签、断言实在此两处）。

**R2 新 findings（均 NIT、非阻塞）：**

- **F5（NIT）·「候选包 35」分母口径漂移。** 见上 F2 核对：可复现口径为 **34**（`acceptanceTestsFile` 非空、排除 `current/`——与 `findPrecedents` 的 current/ 排除语义一致）。建议更正为 34（或如确要写 35，须声明分母含 `current/` 指针，但那与先例路语义不一致、不推荐）。承载数字 21/24/7 与 7 包清单不受影响。
- **F6（NIT）· V2 括注两处微标注。** ① 探针包 id **须带 ≥ `260725` 的日期前缀**（如 `260990-probe-xxx`）才走 post-cutoff pod-events 分支——本判官用 `260990-` 前缀实证通过；V2 未钉此前置，若核验官选了无日期前缀/前缀过小的 id 会落到 legacy `dispatch-log.jsonl` 分支（该失败是**响亮**的——temp pod-events.jsonl 不存在即被发现，非假绿，故仅 NIT）。建议 V2 补一句「探针包 id 须带 ≥260725 日期前缀」。② V2 写「先于 `appendPodEvent`（`:210-217`）」——`:210-217` 实为 payload 字面量，`appendPodEvent` 调用在 `:206-218`；「先于」的判定不受影响，标注宜对齐（T2 证据段同号引用是对的，仅 V2 此处挂错了对象）。

**另核项（R2 清单）**：manifest（2 路径）仍 ≡ §2 锚清单并集（T1 `brief-inputs.mjs` + T2 `dispatch-stamp.mjs`）✓；`landing-fidelity-check.mjs --preflight` 复跑 `{"ok":true,"anchorCount":2,"findings":[]}` ✓；§7 R1 原样保全（diff 核对 `c3ba367→63dd6a9` 仅本 PROPOSAL 一文件、R1 全文以追加形态入 commit、无既有行改写）✓；「无 OQ」仍成立（两改法文本两轮未动，无新分叉）✓；无新引入矛盾（除 F5/F6 两 NIT）。

**结论**：GREEN。主线程可凭「立项 ack + 本 R2 GREEN」标 FROZEN（F5/F6 两条 NIT 建议冻结前顺手更正，单行量级、无需再轮）；随后走 packet-bound 落地 + landing-verifier 独立核验。