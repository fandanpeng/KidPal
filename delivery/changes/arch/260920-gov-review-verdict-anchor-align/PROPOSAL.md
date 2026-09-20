# PROPOSAL · 260920-gov-review-verdict-anchor-align

> **状态：DRAFT v0.3 · 2026-09-20 · 已立项 ack · 待 architect R2 复审**（凭据：human 2026-09-20 立项 ack + OQ 全裁；**R1 GREEN（0 BLOCK / 3 SHOULD / 4 NIT）→ 主线程 v0.3 折入 F1–F7**〔F1 green 取最后命中 token / F2 token 集收 {GREEN} / F3 RE 容忍反引号 + T1 正例 / F4 census 时点校准 / F5 pin 数 12 + CONTRACT_WIRE_FILE 先例更正 / F6 显示副作用申报 / F7 backfill 空答复保旧值〕，详见 §7 R1。FROZEN 行待补 R2 GREEN 凭据）。
> **承载 ARQ**：`ARQ-260909-gate-green-anchor-regex-loose`（T1+T2）、`ARQ-260909-index-verdict-anchor-okform-gap`（T2，其「pin」侧）、`ARQ-260920-changes-index-mergedinmain-stale`（T3）。**三条同根**：verdict/状态行的**判据与实际书写形态脱节**。不关的根写 §6 留根指针。
> **本批定位**：三条合一的根治方案 = **verdict 行规范化 + 判据单源化**——判官与核验官的 verdict 行写**固定机器可读形态**（两处简报成文），升级闸与 `deriveLanded` 改用**同一个规范锚**（`VERDICT_STATUS_RE` 迁 lib 单源），`mergedInMain` 守卫收真值型，**6 个存量批按实况 pin 进既有 `LANDED_ADJUDICATION`**（沿既有 7 条先例）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节）。两条升级判据均未命中。**但 T2 触「改 gate 判定」（前置升级闸的 GREEN 锚）**——按 AGENTS.md「存疑向严」与 P1 先例（human 裁「都留 Track B」），本批**如实声明该 gate 事实交判官核**，判官认为须升级即升。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T4）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（一句话证成：T2 改**前置升级闸的判定锚**——属本仓最承重的闸；但方向为**收窄/向严**（从「恒真」恢复为「只认规范 verdict 行」），且**无在途 FROZEN 批受影响**（见 §2 T2 的过渡分析）；T3 为一行守卫收窄；T1/T4 为成文与数据。无放松面、无数据丢失）。
> **review profile**：G3 + G2（G3 = `changes-index.mjs` + `pre-merge-escalation-gate.mjs` + `lib/governance-gate-shared.mjs`；G2 = `.claude/references/track-b-judge-brief.md`）。契约面 / G1 / agent frontmatter 为零。**T2 触「改 gate 判定」= lightweight 机检硬排除项，如实声明交判**。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/references/track-b-judge-brief.md
.claude/references/track-b-verifier-brief.md
.claude/scripts/changes-index.mjs
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：让「verdict 写成什么样」与「机器怎么判 verdict」**同形**——判官与核验官写**固定机器可读形态**的 verdict 行，升级闸与索引的判据改用**同一个规范锚**（lib 单源），存量批按实况 pin 进既有机制。

**现状差距**：三处判据**各自漂移、且互相不知道对方的存在**——升级闸的 GREEN 锚**恒真**（样板行自带 GREEN + 裸分支）；`deriveLanded` 的锚**不识 6 批的实际 verdict 形态**；`mergedInMain` 被 `'no'` 钉死。**三者共享同一个根**：verdict 行的书写形态从未被规范化，而消费它的判据各自为政。

**非目标（本批不做什么）**：不改 `docs/contracts/**` 与系统 spec；不动任何 agent frontmatter；不新增工程门；**不追溯老批的判定**（闸只在被合并批上跑、`deriveLanded` 对老批走既有 `LANDED_ADJUDICATION` 人工 pin）；不改 `landing-fidelity-check.mjs`（那是批② 已落的引用面）。

## 2. 问题、根因与改法（T1…T4）

**【硬项 · 起草期实证义务】** 本节断言均为 2026-09-20 现跑实测（命令 + 输出留痕见各 T 证据段）。

**【硬项 · 目标文件锚清单】** 每 T 末尾显式列「目标文件：<仓根相对路径清单>」。改法标题一律 `### T<n> · <标题>`；landing-manifest 围栏块一行一个裸路径。

**条目形态（五段固定式）**：见 `docs/templates/PROPOSAL.template.md` §2「条目形态」。

**【起草期自检两句】** ① FROZEN 前跑 `--preflight` 并通过 §5 验证集实跑；② 本批不涉路径类变更，族谱表不适用。

### T1 · verdict 行规范成文（判官简报 + 核验官简报两处落点）

**问题**：判官与核验官的 verdict 行**书写形态从未被规范化**——起草期实测（26 份 arch PROPOSAL + 19 份 landing-verdict；**R1-F4 时点校准**：现况 29 份 PROPOSAL / 25 份 landing-verdict），verdict 行至少 **8 种写法**（`verdict：GREEN` / `**GREEN（可 FROZEN）**` / `**结论：GREEN（可冻结）。**` / `### R2 · architect verdict：GREEN（…）` / `判定：PASS——…` / `## 总结论：绿（…）` / `**verdict：APPROVE**` / Status 行轮次指针 `R2 GREEN〔…〕`），**没有一种被两处判据共同认可**。影响面：升级闸的 GREEN 锚**恒真**（见 T2）、`deriveLanded` 对 6 批推不出 landed（见 T3）。

**证据**（全部亲核，2026-09-20）：
- 判官简报（verbatim）：`.claude/references/track-b-judge-brief.md:21` = `**verdict 格式**：\`GREEN\` 或 \`BLOCK(F1..Fn)\`；每个 finding 附证据…`——**只定义了 token，没定义行的书写形态**（行首？要不要标签？加不加粗？）。
- 核验官简报（verbatim）：`.claude/references/track-b-verifier-brief.md:30-32` = `## verdict 格式` 段——`\`landing-verdict.md\` 含：verdict 行（\`PASS\` / \`FAIL(F1..Fn)\` / \`PASS + N ADVISORY\`）+ 六项逐项结论…`——同样**只定义 token、没定义行形态**。
- **两处简报都没说「verdict 行长什么样」** ⇒ 每个判官/核验官自由发挥，形态发散。

**根因**：两处简报的 verdict 格式段定义了**语义**（什么算 GREEN / 什么算 FAIL）但**没定义行的机器可读形态**——而下游有两个消费方（升级闸、deriveLanded）各自猜形态。

**改法**：
1. 改 `.claude/references/track-b-judge-brief.md:21`：verdict 格式段**追加行形态义务**——「verdict 行必须写为**行首** `verdict: <TOKEN>`（可带 `>` 引用前缀；TOKEN ∈ `GREEN` / `BLOCK(F1..Fn)`），**append 到 PROPOSAL §7**」。**正例（R1-F3 采纳）**：批① reader-scripts-align §7 的 `**Verdict：`GREEN`**` 形态（token 被反引号包裹）**也须被锚认得**——该形态为真实判官输出、现行 RE 不认（该批仅靠次级行兜住）⇒ T2 的 RE 须容忍 token 前后的反引号，配 V2 夹具。
2. 改 `.claude/references/track-b-verifier-brief.md:30-32`：verdict 格式段**追加行形态义务**——「`landing-verdict.md` 必须含一行**行首** `verdict: <TOKEN>`（TOKEN ∈ `PASS` / `FAIL(F1..Fn)` / `PASS + N ADVISORY`）」。

**为什么这样改**：直击根因——**下游两个消费方需要的「机器可读 verdict 行」从未被定义**；定义一次（简报成文），两个消费方（升级闸、deriveLanded）就能**共用同一个锚**。

目标文件：`.claude/references/track-b-judge-brief.md`（改）、`.claude/references/track-b-verifier-brief.md`（改）

### T2 · 升级闸 GREEN 锚改用 `VERDICT_STATUS_RE`（迁 lib 单源）

承载 `ARQ-260909-gate-green-anchor-regex-loose`。

**问题**：升级闸的 GREEN 锚（`.claude/scripts/pre-merge-escalation-gate.mjs:84`）**恒真**——**恒真的真机制**（本批起草期实测）= `PROPOSAL.template.md` 头部样板行 `**结构字段硬条款**：…：**GREEN 结论前必须在场**` **自带 GREEN**，实测存在于 **16/26** 份 arch PROPOSAL（**R1-F4 时点校准**：16/26 为 260918 批时点值，现跑 **19/29**〔PROPOSAL 总数亦由 26 增至 29〕）且**单独即令现行 regex 为真**，与判官判没判无关。**裸 GREEN 分支**只是让恒真雪上加霜。

**证据**（现跑，2026-09-20）：
- 样板行实测：`grep -l "GREEN 结论前必须在场" delivery/changes/arch/*/PROPOSAL.md` = **16 / 26** 份。
- 样板行单独测试：对该行文本跑现行 regex → `true`（即使整份 PROPOSAL 无任何判官 verdict）。
- **`VERDICT_STATUS_RE` 的剖面**（对 26 份 arch PROPOSAL 逐行测，即 `deriveLanded` 的真实用法）：FROZEN 24 批中命中 **17**、未命中 **7**（均为非规范形态批）；未命中的 7 批**均已合并**（闸不会再跑它们）⇒ **零实际影响**；非 FROZEN 批中被命中 **1**（`260830`，其 verdict `**结论：GREEN。**` 为**真判官 GREEN**、其 FROZEN_RE 不匹配仅因 Status 行无版本号）⇒ **零假阳性**。

**根因**：`readGovAnchors` 的 green regex 是「**全文 token 匹配**」——而 PROPOSAL 是**充满 GREEN 字样的治理文档**（样板行、census、讨论），全文匹配**必然恒真**。**正确形态是「行首判据」**——而 `changes-index.mjs:177` 的 `VERDICT_STATUS_RE` 已经是**行首判据**（`^\s*…verdict…:…TOKEN`）且已被 `deriveLanded` 验证过。

**改法**：
1. `VERDICT_STATUS_RE`（+ `VERDICT_HEADING_RE`）从 `changes-index.mjs:177-178` **迁到 `lib/governance-gate-shared.mjs`**（export 单源），`changes-index.mjs` 改 import——**防派生判据双源分叉**（单源先例 = `CONTRACT_WIRE_FILE`〔在 `brief-inputs.mjs` export、R1-F5 更正：非 lib〕与 `closeoutRequiredFiles`〔在 lib〕——**两种形态都算单源**，本批取 lib 形态因两个消费方都在 `.claude/scripts/`）。
2. `pre-merge-escalation-gate.mjs:84` 的 green 改为：对 PROPOSAL 文本**逐行**跑 `VERDICT_STATUS_RE`，**取最后命中的 token**（后出者胜，与 `deriveLanded` 的 `:189` 覆写语义一致——R1-F1 采纳，防「GREEN 后重开」场景 fail-open），`green = (最后命中 token === 'GREEN')`——**即 `deriveLanded` 对 PROPOSAL 的同款判定**。**token 集收为 {GREEN}**（R1-F2 采纳：删 PASS——260918-gov-review-merge-chain-friction R1 `:176` 已论证否决该 token〔spoof 面、判官语法无 PASS〕）。**RE 须容忍 token 前后的反引号**（R1-F3：批① §7 的 `**Verdict：` + 反引号 GREEN + 反引号 + `**` 形态为真实判官输出）。
3. **裸 GREEN 分支删除**（ARQ 原病灶）。

**为什么这样改**：① 恒真的根因是**全文 token 匹配**而非裸分支本身——把锚改成**行首判据**后，样板行（GREEN 在行中）与散文（GREEN 在行中）**都不命中**，只有**行首 verdict 行**命中 ⇒ 恒真根治；② 单源化 = 升级闸与 `deriveLanded` **共用同一条判据**（不再各自猜形态）；③ **老批不追溯**——闸只在被合并批上跑，24 批 FROZEN 中 7 批非规范形态的**均已合并**（零实际影响）；④ **新批**由 T1 的简报义务保证写规范形态。

**过渡分析（无在途受影响批）**：本批立项时点（2026-09-20）**无 FROZEN 未合并的批**——260918 四批 + 批① 均已 landed + merged。故锚收窄**不产生任何过渡期假阴性**。

**自检**：gate 的 `--self-check` 补三夹具——样板行文本 → **不命中**（恒真根因消除）；`verdict：GREEN` 行 → 命中；`BLOCK(F1)` 行 → 不命中（GREEN 锚不认 BLOCK）。

目标文件：`.claude/scripts/lib/governance-gate-shared.mjs`（改：VERDICT_* 迁入 + export）、`.claude/scripts/changes-index.mjs`（改：改 import）、`.claude/scripts/pre-merge-escalation-gate.mjs`（改：green 锚换源 + 删裸分支）

### T3 · `mergedInMain` 守卫收真值型（`if (merged)` → `=== 'yes'`）

承载 `ARQ-260920-changes-index-mergedinmain-stale`。

**问题**：`changes-index.mjs:371` 的守卫 `if (merged) { …cache[id] = {…mergedInMain: merged} }`——`gitContainedInMain` 返回 `'yes'` / `'no'` / `''`（`:118`），**`'no'` 在 JS 中为真值** ⇒ 「未合并」时守卫照样通过、把 `'no'` 钉进缓存；而全量重建**缓存优先**（`:240`）⇒ 合并后该列**不翻 `yes`**。实测：本族四批合并后全量重建仍显示 `landed | no`，须 `--backfill` 才翻（commit `5a01585` 记录）。

**根因**：`:371` 的 `if (merged)` 读起来像「**如果已合并**才写」，实际是「**只要 git 给了确定答复就写**（无论答案）」——**守卫与意图脱节**（与 `ARQ-260909-gate-green-anchor-regex-loose` 的「注释意图 vs 实现」同型）。

**改法**：`:371` 的 `if (merged)` 改为 `if (merged === 'yes')`——**一行**，使「未合并」不再钉缓存（合并后跑 `--upsert` 即正确翻 `yes`）。**显示副作用（R1-F6 采纳，如实申报）**：未合并批的「已合main」列由 `'no'` 变 `'—'`（空 = 无确定答复）——显示形态变化、非行为变更。**同批一并收窄 `--backfill` 的无条件写入**（R1-F7 采纳）：`:414` `cache[name] = { …mergedInMain: merged }` 在 `merged === ''`（git 不可用）时会**抹掉已钉的 `'yes'`**——改为 `''` 分支**保旧值**（不覆盖既有缓存项的 mergedInMain）。

目标文件：`.claude/scripts/changes-index.mjs`（改）

### T4 · 6 个存量批 pin 进 `LANDED_ADJUDICATION`（沿既有机制）

承载 `ARQ-260909-index-verdict-anchor-okform-gap`（其「存量失真」侧）。

**问题**：6 个已合并批的 landing-verdict **verdict 行形态不被 `deriveLanded` 认出**（六成因：`verdict：APPROVE` token 不在集 / `### 7.0 Verdict：` 点分编号 / `**绿（GREEN）**` 无标签 / `**最终 verdict：GREEN**` 前缀挡路 / `**绿（放行建议）**` 同类 / `## 0. Verdict：**RED**` 后续转绿为散文无 token 行）。INDEX 恒显示 frozen。**为什么用 pin 而非补锚**：六形态**各自不同**，补锚 = 打地鼠（P4 批已实证）；而 **`LANDED_ADJUDICATION` 就是既有机制**（`changes-index.mjs:36` 起 **12 条**——R1-F5 更正：v0.2 原写 7 条；注释「发散散文旧批…人工核 verdict 结论为通过态」）——**沿它就是沿设计**。

**证据**（现跑，2026-09-20；逐批核 landing-verdict 的实际 verdict 行）：
- `260907-architect-dispatch-model-override`：`:3` = `**verdict：APPROVE**`——token 不在集。
- `260908-escalation-conflict-ladder`：`:95` = `### 7.0 Verdict：**GREEN（放行建议…）**`——点分编号 `7.0` 不被 `\d{1,3}[.、]\s*` 认。
- `260908-merge-chain-hardening`：`:5` = `## 0. Verdict：**RED（…）**`，`:164` = 散文「五前提全部满足…全绿」——**转绿无 token 行**。
- `260907-merge-friction`：`:98` = `**绿（GREEN）**——…`——无标签。
- `260904-escalate-merge-approval`：`:77` = `**最终 verdict：GREEN。**`——`最终` 挡在标签前。
- `260830-ui-impl-strategy`：`:9` = `**绿（放行建议）——…**`——无标签。

**改法**：`changes-index.mjs` 的 `LANDED_ADJUDICATION` map **增 6 条**（沿既有 7 条格式：id → true + 人读理由注释），逐条注明明实核的 verdict 行与出处——**pin 是「人工核 verdict 结论为通过态」的既有机制**，6 条均为本批起草期逐批实核（verdict 行 verbatim 已列上）。

**为什么这样改**：直击根因——**沿既有机制**而非打地鼠补锚；且 6 条 pin 后 INDEX 对它们的显示**恢复真实**（landed）。

目标文件：`.claude/scripts/changes-index.mjs`（改：LANDED_ADJUDICATION +6 条）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 判官简报 | `.claude/references/track-b-judge-brief.md` | G2 | T1① verdict 行形态义务。消费方 = 判官（每批） |
| 核验官简报 | `.claude/references/track-b-verifier-brief.md` | G2 | T1② verdict 行形态义务。消费方 = landing-verifier（每批） |
| 索引生成器 | `.claude/scripts/changes-index.mjs` | G3 | T2 VERDICT_* 改 import（迁 lib）；T3 LANDED_ADJUDICATION +6 条 pin。消费方 = `--upsert` / 全量重建 / `--backfill` / `--self-check` |
| 共享 lib | `.claude/scripts/lib/governance-gate-shared.mjs` | G3 | T2 VERDICT_* 迁入 + export。消费方 = `changes-index` + `pre-merge-escalation-gate` |
| **前置升级闸** | `.claude/scripts/pre-merge-escalation-gate.mjs` | G3（**gate 判定锚变更，如实声明**） | T2 green 锚改 `VERDICT_STATUS_RE` 逐行判 + 删裸分支。消费方 = merge-driver 透传、landing 前置升级闸、`--self-check` |
| 契约面 / G1 / agent frontmatter | — | **未触** | 零改动 |
| 工程门新增 | — | **零新增** | T2 是**改既有闸的锚形态**（向严），非新增门 |

**【硬项 · sweep 透镜义务】** T2 落地新判定 pattern（VERDICT_STATUS_RE 用于升级闸），**起草期已对全量 26 份 arch PROPOSAL 预跑**（剖面见 T2 证据段：FROZEN 24 中命中 17 / 未命中 7〔均已合并、零实际影响〕/ 非命中 1 属 FROZEN_RE 版本号瑕疵非锚问题）。**T4 的 6 条 pin 逐批实核 verdict 行**（verbatim 已列）。

**【硬项 · 复扫模式完备性先验】** 本批不涉退役语面 / 残留清扫，无清扫模式集。

## 4. human 取舍点（OQ）

### OQ1 · T2 的过渡策略

**背景**：锚收窄后，**新批**须写规范 verdict 行（T1 的简报义务保证）、**老批**不追溯（闸只跑被合并批）。但有一个**过渡期缝隙**：若某批在**本批落地之后**、判官**还没读到新简报**时被合并，闸会对它判红（假阴性）。

**(a) 接受该缝隙**（推荐）：缝隙窗口 = 本批 landing 到判官下次读到新简报之间，**实际为零**——本批立项时点无在途 FROZEN 批（260918 四批 + 批① 均已合并），而下一批的判官派遣会在本批 landing 之后。**风险 = 下一批的判官若沿用旧习惯写非规范 verdict 行，其批会被闸拦**——但那正是 T1 义务要教的，拦一次即纠正。
**(b) 过渡宽限**：闸在收窄后 N 批内对非规范形态**降 advisory 不拦**。理由：更平滑；代价：**宽限期内的恒真没治**，且 N 的选取又是新参数。
**(c) 不收窄，只加 T4 pin**：维持现状、只修 INDEX 显示。理由：零风险；代价：升级闸的恒真**原样保留**（`ARQ-260909-gate-green-anchor-regex-loose` 不关）。

**这项为什么该 human 裁**：T2 改的是**前置升级闸**的判定锚，缝隙的容忍度是**流程风险偏好**问题。

### OQ2 · T4 的 6 条 pin 是否由本批落

**(a) 本批落**（推荐）：6 条的 verdict 行 verbatim 已实核（§2 T4 证据段），pin 是机械动作；不落则 INDEX 对它们继续显示 frozen。
**(b) 留给 human 逐条核后另落**。理由：pin 是「人工核 verdict 结论为通过态」的**人工判断**的固化——严格说该由 human 逐条确认。**折中**：本批落 pin，但**在 pin 注释里逐条注明「依据 = 该批 landing-verdict 的 verdict 行 verbatim」**，供 human 事后抽查。

**这项为什么该 human 裁**：pin 的实质是**把「人工核过」写成代码常量**——谁有权做这个断言，是授权边界。

### OQ 裁决（human 2026-09-20）

| OQ | 裁决 | 备注 |
|---|---|---|
| OQ1 | **(a) 接受缝隙** | 过渡窗口实际为零（无在途 FROZEN 批）；下一批判官若沿用旧习惯会被闸拦一次——**那是 T1 义务要教的，拦一次即纠正**。 |
| OQ2 | **(a) 本批落 + 注明依据** | 6 条 pin 逐条注明「依据 = 该批 landing-verdict 的 verdict 行 verbatim」供事后抽查。 |

## 5. 验证集（landing 前机械前置 + landing 后核验）

**【真空绿规约】** V2/V3 为 `--self-check`，**新增夹具须具名**；V4 的「样板行不命中」须**先断样板行文本确实含 GREEN**（防夹具没造对）；V5 的 6 条 pin 须**逐条带 verdict 行 verbatim**（非只写 id）。

| 项 | 命令 | 预期 |
|---|---|---|
| V1 | `node .claude/scripts/changes-index.mjs --self-check` | 全绿（VERDICT_* 迁 lib 后零回退——原有 29 条断言 + 新增） |
| V2 | `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` | 全绿 + **新增三夹具**具名（样板行文本 → 不命中 / `verdict：GREEN` 行 → 命中 / `BLOCK(F1)` → 不命中） |
| V3 | `node .claude/scripts/pre-merge-escalation-gate.mjs 260918-gov-review-merge-chain-friction`（**已合并批**） | 结构上仍 `PASS`——**过渡分析（无在途受影响批）的实证**：该批已合并，闸的重跑不应因锚收窄而翻转结论 |
| V4 | 对 6 条 pin 后的批跑 `deriveLanded`（经 `--backfill` 或 `--upsert`） | 6 批 INDEX 状态由 `frozen` 翻 `landed` |
| V5 | `git diff .claude/scripts/` 逐行归类 | T3 的 diff = **1 行改动**（`if (merged)` → `=== 'yes'`）；T2 的 diff = **VERDICT_* 定义删除 + import 增 + green 换源**，均落在声称范围 |
| V6 | `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` | 全绿 |
| V7 | `landing-fidelity-check.mjs <本批>` 结构门 | §2 锚集 ≡ manifest 机判通过（本批 5 路径） |

## 6. 留根清单

1. **批② 的 `ARQ-260909-index-verdict-anchor-okform-gap`**——本批的 T1/T2/T4 合起来关它：**新批**走规范形态（T1 成文 + T2 锚认）、**存量批**走 pin（T4）。**关它的条件 = T1–T4 全部落地**。
2. **`ARQ-260909-gate-green-anchor-regex-loose`**——本批 T2 关它（锚收窄 + 裸分支删除）；**处方证伪的完整分析**（四候选实测表 + 真机制）已在 P1 批 §6 留根① 与队列行追记。
3. **`ARQ-260920-changes-index-mergedinmain-stale`**——本批 T3 关它的「守卫真值陷阱」侧；**「重建不重算」侧由 T4 的 pin 缓解**（6 批 pin 后 INDEX 恢复真实），但**「合并后跑 upsert 前仍显示 no」的时序问题仍在**——随该 ARQ 的收口一并评估是否需要「合并流程强制 backfill」。
4. **`260830` 的 FROZEN_RE 版本号瑕疵**——其 Status 行 `**状态：FROZEN · 2026-08-31` 无 `vN.M`，`readGovAnchors` 的 frozen 锚不命中（本批起草期实测）。**不修**（该批已合并、闸不再跑它）；如实登记为既有瑕疵。
5. **判官/核验官对新形态的遵从率**——T1 的简报义务落地后，判官是否**每次**都写规范行，需 `/loop-retro` 观察。若遵从率低，T2 的锚会对它们判红——**那是有意的**（逼规范），但若误伤率高则须回头评估。

**交日落复盘项**：T2 落地后，`/loop-retro` 观察——下一批的判官 verdict 行是否规范（遵从率）、升级闸是否有新假阴性。

**自律件**：起草期四处 census（样板行 16/26、VERDICT_STATUS_RE 剖面 17/24+7、净增益、6 批 pin 的 verdict 行 verbatim）均为现跑；**本批的处方证伪教训已吸收**——所有「改法」都先做了全量剖面验证（非照 ARQ 原文）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-20 - architect

**verdict：GREEN（0 BLOCK / 3 SHOULD / 4 NIT，无升级 human 项）**——方向与根因诊断全部成立：恒真机制（样板行 + 裸分支）、`mergedInMain` 真值陷阱、六批存量失真均亲核复现；T2 全量剖面（17/7/1）独立复跑逐格吻合。三条 SHOULD 均为改法②/T1 措辞级收口（一句话量级），**属 FROZEN 前应折入项而非可选**——F1/F2 若不折入会把一个 fail-open 面落进闸（见下），不折请回本轮再确认；折入后无需再轮，可直接凭「立项 ack + 本 GREEN」标 FROZEN。四条 NIT 可随冻结顺手改或留根。

**亲核留痕**（2026-09-20 现跑，仓库根 `C:/AI/KidPal-gov`）：

1. **恒真机制复现**：对模板样板行（`docs/templates/PROPOSAL.template.md` 头部「结构字段硬条款」行，与本 PROPOSAL `:11` 同文）逐字跑 `VERDICT_STATUS_RE`（`changes-index.mjs:177` 原文正则）→ **false**；对同一行跑现行闸 green regex（`pre-merge-escalation-gate.mjs:84`）→ **true**。「样板行不命中」与「样板行单独即令现行 regex 为真」双证成立。
2. **全量剖面独立复跑**（用升级闸自己的 FROZEN 锚 `/^[>\s]*\*\*状态：FROZEN v\d+\.\d+/m` 作口径，对 29 份 arch PROPOSAL 逐行跑 `VERDICT_STATUS_RE`）：FROZEN 24 批中命中 **17**、未命中 **7**；非 FROZEN 命中 **1** = `260830-arch-review-ui-impl-strategy`，命中行 `:255` = `**结论：GREEN。**`（亲读上下文为真判官终轮 GREEN；其 Status 行 `> **状态：FROZEN · 2026-08-31` 无 `vN.M`，闸 frozen 锚不命中——与 §6 留根④ 一致）。**17/7/1 与 PROPOSAL T2 证据段声称逐格吻合**；分母出入见 F4。
3. **未命中 7 批均已合并**：逐批取目录末提交（`git log -1`）+ `git branch --contains`（用脚本同款 `--format=%(refname:short)` 精确比较）→ 260829-arch-review-kidpal-spec-stories、260906-index-status-truth-source、260907-merge-friction、260908-escalation-conflict-ladder、260908-merge-chain-hardening、260914-delivery-restructure、260915-worktree-pool-reuse **全部在 main**——「零实际影响」成立。（环境注记：裸 `git branch` 装饰输出在本机带 `*`/`+` 标记，逐字比较会假阴；`gitContainedInMain` 本身用 `--format`，不受影响。）
4. **过渡分析复证**：对全部 FROZEN 批（lib `isFrozen` 口径）逐批核末提交在 main → **零个不在 main**；立项时点确无在途 FROZEN 批，OQ1(a)「窗口实际为零」成立。非 frozen 存量仅 260827-migration-bootstrap 与 260829-migration-tier23（LANDED-UNREVIEWED 记账）、260908-human-deputy-shadow（DRAFT 停滞）与本批自身，均不构成过渡面。
5. **V3 基线实测**：`node .claude/scripts/pre-merge-escalation-gate.mjs 260918-gov-review-merge-chain-friction` → `PASS (changeId=…, kind=gov) — 无例外命中`、exit 0（现行锚基线）。该批 §7 的 `VERDICT_STATUS_RE` 命中行为 `:155` BLOCK 与 `:202` GREEN（末位 GREEN），T2 收窄后 green 仍真 → V3 预期可执行且方向正确。
6. **T4 六条 pin 逐条亲核**（超出「抽 2–3 条」要求，全 6 条 + 成因亲证）：`260907-architect-dispatch-model-override/landing-verdict.md:3`、`260908-escalation-conflict-ladder:95`、`260907-merge-friction:98`、`260908-merge-chain-hardening:5` 与 `:164`、`260904-escalate-merge-approval:77`、`260830-ui-impl-strategy:9` 六处 verdict 行 **verbatim 与 §2 T4 证据段逐字一致**；六成因逐一实证（例：`### 7.0 Verdict：…` 对 `VERDICT_HEADING_RE` 实测不匹配——`\d{1,3}[.、]` 吃掉 `7.` 后余 `0` 挡住标签位；`最终` 前缀、`绿（…）` 无标签、`## 0. Verdict：RED` 后散文转绿无 token 行，同类亲证）。pin 的注释审计支撑充分（OQ2(a) 裁「注明依据 = verdict 行 verbatim」+ V5 逐条带 verbatim）。
7. **T3 根因代码亲核**：`changes-index.mjs:371` = `if (merged) { entry.mergedInMain = merged; cache[id] = { ...(cache[id] || {}), date: entry.date, mergedInMain: merged }; }`；`:118` = `return branches.includes('main') ? 'yes' : 'no';`（`''` 哨兵在 `:112`/`:120`）；`:240` 全量重建缓存优先——`'no'` 真值钉缓存 + 重建不重算的链路成立；commit `5a01585`（2026-09-19，`chore(gov): INDEX --backfill —— 四批 mergedInMain 翻 yes`）在场。
8. **简报引文亲核**：判官简报 `:21`、核验官简报 `:30-32` 与 T1 证据段引文逐字一致；两处确实只定义 token、未定义行的书写形态。
9. **迁移面干净**：全仓 grep `VERDICT_STATUS_RE` 唯一消费方 = `changes-index.mjs`；正则无 `/g` flag（逐行 `exec` 与 `test` 均无 lastIndex 态，changes-index 逐行用法与升级闸拟改的逐行用法兼容）；两脚本均已从 lib 具名导入（`changes-index.mjs:27`、`pre-merge-escalation-gate.mjs:32`），迁移动作顺理。V1 声称的「原有 29 条断言」实测吻合（`selfCheck` 现有 29 个用例）。
10. **ARQ 承接核实**：T2 承接的是 `ARQ-260909-gate-green-anchor-regex-loose` 2026-09-19 证伪追记后的建议方向（「判官 verdict 行规范化 + 机器锚、老批不追溯、闸只在被合并批上跑」）而非原处方 ✓；T3 = `ARQ-260920` 子项 a，子项 b 留根（与该 ARQ「子项 b 触及时转 human」一致）✓；T4 = `ARQ-260909-index-verdict-anchor-okform-gap` 追记建议 ①+③（建议②「点分编号补锚」被弃——存量已由 pin 覆盖、未来已由 T1 规范形态覆盖，弃之成立；建议 T4 加一句明示弃②的 reason，非义务）。P1 先例（human 2026-09-19「都留 Track B」）在 `260918-gov-review-merge-chain-friction/PROPOSAL.md:211` 在场 ✓。
11. **结构字段核对**：landing-manifest 围栏块在场（5 路径）≡ §2 各 T 目标文件锚清单并集（T1 两件 + T2 三件，T3/T4 为其子集）；packet-id 行在场（`:7`）。✓

**Findings**：

- **F1 · SHOULD · 改法②「任一行命中即 green」与 `deriveLanded`「后出者胜」语义不一致，「即 deriveLanded 对 PROPOSAL 的同款判定」表述不准确。** 亲核 `changes-index.mjs:189`（`if (s) { finalToken = s[1].toUpperCase(); … }`，循环内后出覆写；`selfCheck` f3/f4 夹具即「翻判取最终」语义）——`deriveLanded` 取**最后**命中 token；改法②写「任一行 token ∈ {GREEN, PASS} 即 `green = true`」。差异是 fail-open 面：§7 早轮 GREEN、后轮 BLOCK（GREEN 后重开评审的场景）时任一行语义误放行，与 T2「向严」方向相逆。修法：改法②改为「逐行取**最后**命中 token，token = GREEN 才 green」——此形态下「同款判定」表述即准确。
- **F2 · SHOULD · §7 green 锚纳入 PASS token，重复了 260918 批 R1 已论证否决的设计。** `260918-gov-review-merge-chain-friction/PROPOSAL.md:176`（R1 finding 原文）：「**新增 `PASS` 为可接受 token**——全仓 25 份 PROPOSAL 无任何真实判官 `verdict: PASS` 形态（唯一字面命中 = …prose 引述…等于新增一条 spoof 面）」。§7 是判官 append 区，判官 verdict 语法只有 GREEN / BLOCK(F1..Fn)（判官简报 `:21`）；PASS 属 landing-verifier 面（另一文件 `landing-verdict.md`，由闸的 `landingVerdict` 存在性检查与 `deriveLanded` 承载）。本批未引未驳即复用该设计。修法：§7 锚 token 集收为 {GREEN}（与 F1 合并修成一句）。
- **F3 · SHOULD · T1 成文与 T2 锚有一处实测形态缺口：反引号包 token 不被认，且恰是最近一批的现行习惯。** 实测：批① `260920-gov-review-reader-scripts-align/PROPOSAL.md:151` 形态为 ``**Verdict：`GREEN`**``（token 反引号包裹），对 `VERDICT_STATUS_RE` → **false**；该批能进 17 命中集仅靠次级行 `:167`（`**结论**：GREEN。`）兜住。T1 现文「行首 `verdict: <TOKEN>`（可带 `>` 引用前缀…）」未给正例句、未排除反引号包 token——下一判官沿用批①习惯即被闸假阴拦一次（OQ1(a) 已裁「拦一次即纠正」，但该拦本可由成文一次消掉）。修法（择一，均一句话量级）：① T1 两处简报文本各加正例（如「例：`> **verdict：GREEN**` / `> **verdict：BLOCK(F1..F2)**`；TOKEN 裸写、不用反引号包裹」）；② 迁 lib 时把 token 前的 `\*{0,2}` 放宽为容忍反引号。任一方向须在 V2 补对应夹具。
- **F4 · NIT · census 数字混用两个时点快照，「均为现跑」声明不完全准确。** 「样板行 16/26」「26 份 arch PROPOSAL + 19 份 landing-verdict」是 260918 时点数（该批 R2 `:225` 亲核 16/26；本批起草 commit `1d0e184` 时点盘上已有 28 份 PROPOSAL——proposal-structure-merge 与批① 均先于本批落地）；本判官现跑 = 样板行 **19/29**（含本批自身）、landing-verdict **25** 份、剖面 17/7/1（此三项为现况口径、逐格复现）。定性结论（恒真、零实际影响、零假阳性）全部维持。建议 FROZEN 前统一改写为现况数或注明快照时点——本批自律件恰以「处方须实测复证」为主题，数字口径应自洽。
- **F5 · NIT · 两处计数/先例引据精度。** ① T4 与 Status 头两处称「既有 7 条」pin——实测 `LANDED_ADJUDICATION` 现有 **12** 条（`changes-index.mjs:38-50`，`grep -c "': true,"` = 12）；+6 动作不受影响，但「沿既有机制」的规模陈述失准（「7」疑转引自 `ARQ-260909-index-verdict-anchor-okform-gap` 追记原文，该行同样写 7）。② 「沿 CONTRACT_WIRE_FILE / closeoutRequiredFiles 的 lib 单源先例」半失准：`CONTRACT_WIRE_FILE` 在 `brief-inputs.mjs:23`（脚本内 export、非 lib）；`closeoutRequiredFiles` 在 lib ✓。建议只引后者，或改引 `isFrozen` / `resolveOriginRef` 等 lib 真先例。
- **F6 · NIT · T3 的显示面副作用未申报。** `=== 'yes'` 收窄后未合并批不再钉 `'no'`，INDEX「已合main」列对它们显示 `—`（原显示 `no`）。可选修法（保显示、不保钉）：守卫内仍 `entry.mergedInMain = merged`、仅 `merged === 'yes'` 时写缓存；或维持现改法、在 T3 申报一句显示语义变化。
- **F7 · NIT · backfill 的无条件覆写在 git 失败分支会抹掉已钉 'yes'。** `changes-index.mjs:415` = `cache[name] = { date, mergedInMain: merged, landed };`——`merged === ''`（git 不可用/无历史）时会把 immutable 的 `'yes'` 覆写为 `''`。`ARQ-260920` 记 backfill 为「该列唯一自动刷新路径」，重算本身不能窄化；建议 `''` 分支保留旧值（如 `merged || cache[name]?.mergedInMain || ''` 形态），fail-closed 显示。

**升级判断**：无「影响目标效果」级问题，不升 human、不升 Track A。T2 触「改 gate 判定」属实，但：① 两条 Track A 硬判据（契约面 / 跨系统结构性变更）均未命中；② 方向为收窄向严、无放松面、无在途批受影响（亲核全 24 FROZEN 批均在 main）；③ human P1 先例（2026-09-19「都留 Track B」）与本批立项 ack 均已就该 gate 事实作出 Track B 裁定；④ 三条 SHOULD 均为措辞级收口、不动改法方向。维持 Track B。

**修订建议汇总**：F1+F2 合并修（改法②一句：逐行取最后命中 token、token = GREEN 才 green）；F3 择一方向加正例或放宽并配 V2 夹具；F4/F5 FROZEN 前顺手改数字与引据；F6/F7 可留根或顺手。F1–F3 折入后无需再轮。