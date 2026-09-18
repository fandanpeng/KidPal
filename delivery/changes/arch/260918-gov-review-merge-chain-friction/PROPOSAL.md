# PROPOSAL · 260918-gov-review-merge-chain-friction

> **状态：DRAFT v0.5 · 2026-09-19 · 已立项 ack · 待 architect R2 复审**（凭据：human 2026-09-18 立项 ack + OQ 全裁；architect R1 BLOCK(F1/F2) 后**按 human 2026-09-19 裁定把 T1〔升级闸 GREEN 机锚收窄〕整条移出本批**，见 §6 留根①。FROZEN 行待补 R2 GREEN 凭据）。
> **承载 ARQ**：`ARQ-260917-step5b-main-append-remerge-tier3`（残余③ + 同族 probe 输出文件）——本批唯一 T。**原承载的 `ARQ-260909-gate-green-anchor-regex-loose` 已移出本批、退回队列**（该 ARQ 的处方经 R1 实测证伪，需重新设计，见 §6 留根①）。
> **母批关系**：本批为 `260918` 拆包族之一（原合并草案 `260918-gov-review-loop-friction-sweep` 已按 human 2026-09-18 裁定拆为四批，逐批 review；原草案已删，其证据已按批内聚进各自 §1）。族内另三批 = `260918-gov-review-dispatch-chain-friction` / `-closeout-chain-friction` / `-batch-drafting-friction`。**另有两项（输入清单先例取旧、launch 前置清单不核）按 human 裁定移出本族、留 ARQ 队列后续单独处理**。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。两条升级判据（触契约面 / 跨系统结构性变更）均未命中。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：**low**（v0.5 降档，理由：T1 移出后本批**零闸判定语义改动**——余下唯一的 T 是 11 件运行产物撤跟踪 + 一行核查结论注释；两者均无数据丢失形态、无行为收紧面、可回滚。R1 的 riskTier=medium 判据随 T1 移出而消失）。
> **review profile**：G3（`.claude/scripts/pre-merge-escalation-gate.mjs`——**仅补一行注释，无判定语义改动**）+ 索引态变更（11 件 `overlap-probe.json` 撤跟踪）。契约面 / G1 / G2 均为零；无 agent def 改动。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/pre-merge-escalation-gate.mjs
delivery/taskpackages/260829-hardware-validation/overlap-probe.json
delivery/taskpackages/260901-ui-art-integration/overlap-probe.json
delivery/taskpackages/260911-display-first-m1/overlap-probe.json
delivery/taskpackages/260911-ecs-deploy-convergence/overlap-probe.json
delivery/taskpackages/260911-key-route-a1/overlap-probe.json
delivery/taskpackages/260911-render-pages/overlap-probe.json
delivery/taskpackages/260911-screen-unification/overlap-probe.json
delivery/taskpackages/260915-m1-k1a-key-layer-dispatch/overlap-probe.json
delivery/taskpackages/260915-m1-k2-keybar-three-slot/overlap-probe.json
delivery/taskpackages/260915-m1-k3-voice-round-ui/overlap-probe.json
delivery/taskpackages/260915-m1-n1b-p1-home-battery/overlap-probe.json
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把合并链上一处「看着修了、其实没修」的机械位修回生效——合并探针产物的忽略配置从其失效的已跟踪态恢复成生效。

**现状差距**：这一条的特殊形态是**修复动作本身是假的**——上一轮修复加了忽略条目，但该条目对**已入库文件**天然不生效，于是病灶原样保留；每一轮受影响的合并都要重付一次 rebase 中止。

**非目标（本批不做什么）**：不改 `merge-driver.mjs` 的合并主流程；不改 `.gitattributes`（`delivery/taskpackages/**/pod-events.jsonl merge=union` 已由 `c16283b` 落地、实测收口，本批原样保留）；不动任何 agent def；不并入同族另三批的任何 T；**不动升级闸的任何判定语义**（v0.5 起，本批对该脚本只加一行注释）。

## 1. 问题与根因（地面真值）

**【硬项 1a 声明】** 本节断言均为 2026-09-18/19 在 `gov/kidpal` 工作树上现跑实测所得；所有 commit SHA 经 `git rev-parse` 核实；所有行号为本批起草时的现行值。**本节经 architect R1 独立亲核逐条复证通过**（留痕见 §7 R1「亲核留痕」节）。

### 问题 1：`overlap-probe.json` 的 `.gitignore` 条目对已跟踪文件不生效

证据（亲核）：`.gitignore:46` = `overlap-probe.json`（在场）；`git ls-files | grep -c overlap-probe.json` = **11**（11 件仍被跟踪）——`git ls-files --error-unmatch delivery/taskpackages/260915-m1-k3-voice-round-ui/overlap-probe.json` 退出 0，确认 tracked。`.gitignore` 只对**未跟踪**路径生效，已入库路径不受其约束。

**病灶形态**：`overlap-probe.json` 是合并期闸的**运行产物**——`.claude/references/pod-coordination.md:183` 记「升级闸 `parallelOverlapProbe`（只读 merge-base 交集 + merge-tree 实撞，`--probe-out` 落 overlap-probe.json）」；`merge-driver.mjs:1254` 每次运行读它（写点在 `:1257`）。而 merge-driver 的内部闸**每跑必重写**它（gate `:553-555` 每跑 `--probe-out` 重写）。tracked 态 ⇒ 每次 merge 前工作树带 unstaged 改动 ⇒ rebase 恒撞「local changes would be overwritten」。

**前序消解不完整**：commit `c1bd37b`（`git rev-parse c1bd37b` 核实存在，2026-09-17）做了两件事——加 `.gitignore` 第 46 行、改一件 probe 文件内容——**但未 `git rm --cached` 存量 11 件**，故该条目对它们失效，病灶原样保留。

**残余③核查结论（起草期已完成，非待办；R1 复证维持）**：`ARQ-260917-step5b-main-append-remerge-tier3` 遗留的「升级闸对 merge=union 文件的探测口径是否需显式豁免」已核——**无需改动**。依据（verbatim 亲核）：`pre-merge-escalation-gate.mjs:199` =

```
      if (!conflictedFiles.length) return 'union-safe'; // 相交但可自动并（不同 hunk git 自并）
```

`merge=union` 文件由 git 自动并、不出现在 `conflictedFiles`，天然落该分支判 `union-safe`（② 级不升人，`:200` 的 `VERSIONED_ARTIFACT_RE_LOCAL` carve-out 是另一条并列路径、非本情形依赖）。同 ARQ 记录 k3 修复轮 r15 合并实测「tier3 评审噪音收口」与此一致。故该项从「待办」降为「已核」，结论写进代码承载点（T1）。

## 2. 改法（T1）

### T1 · `overlap-probe.json` 撤出跟踪 + 升级闸 union 探测口径核查结论成文

承载 `ARQ-260917-step5b-main-append-remerge-tier3`（残余③ + 同族 probe 输出文件）。

① **撤跟踪 11 件存量**：对 §landing-manifest 列出的 11 个 `overlap-probe.json` 路径执行 `git rm --cached`（**只出索引、工作树文件保留**），使 `.gitignore:46` 的既有条目真正生效。为什么：直击问题 1 的根因——`.gitignore` 对已跟踪路径不生效，`c1bd37b` 只加了条目未撤跟踪，病灶原样保留。撤跟踪后 merge-driver 每跑重写的该文件回到 untracked 态，rebase 撞 unstaged 的形态消除。

② **核查结论成文**：在 `.claude/scripts/pre-merge-escalation-gate.mjs` 的 `:199`（`if (!conflictedFiles.length) return 'union-safe';`）处补一行注释，记录起草期已完成的核查结论——`merge=union` 文件由 git 自动并、不出现在 `conflictedFiles`，天然落该分支判 `union-safe`，**无需显式豁免**（依据 = 该行分支逻辑 + ARQ-260917 记录的 k3 修复轮 r15 合并实测「tier3 评审噪音收口」）。为什么：该结论原为队列里的「待办」，核查后降为「已核」；写进代码承载点比留在队列里耐久，且后人改这条分支时会看到它。**该改动是纯注释——不触任何判定语义。**

③ **跨工作树过渡预期（R1-F5 采纳，如实申报）**：撤跟踪后 ff 合并进 main（`C:/AI/KidPal`）时，这 11 件会从 **main 工作树盘上消失**（tracked-deleted）；Pod 工作树若恰带本地脏 probe，会在过渡期撞一次「local changes would be overwritten」——**正是本批要治的形态，属过渡期一次性代价、非新增缺陷**。文件为**可再生运行产物**（gate `:553-555` 每跑 `--probe-out` 重写、merge-driver `:1257` 传入该路径），**无数据丢失**。本条如实写进 landing summary 的诚实注记段。

**不改** `merge-driver.mjs` 的合并主流程、不改 `.gitattributes`、不改升级闸的判定语义。

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改：注释一行）、`delivery/taskpackages/260829-hardware-validation/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260901-ui-art-integration/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-display-first-m1/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-ecs-deploy-convergence/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-key-route-a1/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-render-pages/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-screen-unification/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260915-m1-k1a-key-layer-dispatch/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260915-m1-k2-keybar-three-slot/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260915-m1-k3-voice-round-ui/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260915-m1-n1b-p1-home-battery/overlap-probe.json`（撤跟踪）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 前置升级闸 | `.claude/scripts/pre-merge-escalation-gate.mjs` | G3（**纯注释，无判定语义改动**） | T1② 补一行核查结论注释。消费方 = merge-driver T1/Step 1b 透传、landing 前置升级闸、`--self-check`；**行为零变更** |
| 合并运行产物 | 11 × `delivery/taskpackages/*/overlap-probe.json` | **撤跟踪** | T1① 只 `git rm --cached`、工作树文件保留；**无 git 内消费者**（R1 消费方 sweep 亲核：merge-driver 读写工作树副本、gate 自检夹具本地构造 JSON、adjudication 文档仅作历史证据引用） |
| 契约面 / G1 / G2 / agent frontmatter | — | **未触** | 零改动 |
| 工程门新增 | — | **零新增** | 本批不新增门、不改任何闸的判定 |

**【硬项 1b · sweep 透镜】** 本批**不引入新判定 pattern、不引入新枚举面**，故无判定面预跑义务。**清点类义务已履行**：11 件清单经 `git ls-files` 机械枚举（索引态本身即穷举）、R1 独立复跑并核「清单与 manifest 逐条 1:1」✓。

**【硬项 1c · 复扫模式完备性先验】** 本批 T1① 涉**同族残留清扫**——模式集为**显式枚举 11 条路径**（非模式匹配），**完备性论证**：清点以 `git ls-files` 为准（索引态即穷举），不依赖任何 glob 或词形变体假设。本批不涉退役语面，无实体清扫模式集。

## 4. human 取舍点（OQ）

### OQ1 · T1 的撤跟踪范围

**(a) 11 件全撤**（human 2026-09-18 已裁）。理由：清点以索引态为穷举、一次撤干净，避免「同族形态可复发」的教训再留尾巴。**(b) 只撤近三批**（260915 三件，早期批次已收口、其 probe 不再被重写）。理由：改动面最小。**(c) 不撤，改 merge-driver 落点**（把 probe 输出移出被合并路径）。理由：从形态上根治，但要动合并主流程、半径大于本批定位，与本批 §0 非目标冲突。

### OQ 裁决（human 2026-09-18，v0.5 状态复核）

| OQ | 裁决 | v0.5 状态 |
|---|---|---|
| ~~原 OQ1「T1 判官 GREEN 机锚走哪条轨道」~~ | — | **随 T1 移出而作废**：本批已零闸判定语义改动（见下） |
| OQ1（撤跟踪范围） | **(a) 11 件全撤** | 维持，仍是本批唯一 OQ |

**v0.5 的 OQ 清零说明**：v0.2 时的 OQ1（闸语义触点走 Track B / full-adversarial）随「T1 移出」而**前提消失**——本批现在对升级闸只加一行注释，无判定语义改动，故该判断点不再存在。**human 2026-09-18 的「都留 Track B」裁决对族内其余批次（P3 的验收闸枚举 + 新 fail-loud）仍然有效**。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**【真空绿规约】** V1 为清单类断言，须先断言**输入非空**（撤跟踪前 `git ls-files` 命中数 > 0，否则「撤后 = 0」是真空绿）；V3 为 `--self-check`，须核输出**无新增失败**（本批为纯注释改动，预期**零差异绿**——这正是「未破坏闸行为」的正面证据）。

| 项 | 命令 | 预期 |
|---|---|---|
| V1 | 撤前 + 撤后各跑 `git ls-files \| grep -c overlap-probe.json` | 撤前 = **11**（>0，非空基线）；撤后 = **0** |
| V2 | `git status --porcelain delivery/taskpackages/*/overlap-probe.json` | 显示为 untracked；**工作树文件仍在盘**（`ls` 逐条确认 11 件在场） |
| V3 | `git log --oneline -1 -- delivery/taskpackages/260829-hardware-validation/overlap-probe.json` | 该文件的历史仍在（撤跟踪不改历史） |
| V4 | `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` | **全绿且与改动前逐格一致**（纯注释改动 → 零差异，证明未触判定语义） |
| V5 | `git diff .claude/scripts/pre-merge-escalation-gate.mjs` | **全部改动落在注释行内**（无任何可执行语句变更）——这是 riskTier=low 与 profile 声明的机械证据 |
| V6 | `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` | 全绿；lint 只允许既有告警（`landing-verifier.md` 超长，本批未触该文件） |
| V7 | `landing-fidelity-check.mjs <本批>` 结构门 | §2 锚集 ≡ manifest 机判通过（本批 12 路径） |

## 6. 留根清单

**① T1（升级闸 GREEN 机锚收窄）——移出本批、退回 ARQ 队列，需重新设计**（human 2026-09-19 裁定；R1-F1 实测触发）

这是本批 R1 的实质产出，须完整交接，否则下一批会重复本批的错误设计：

- **原处方被证伪**：`ARQ-260909-gate-green-anchor-regex-loose` 的处方是「regex 收窄至『verdict/结论 同段』形态与 :83 注释意图对齐」。R1 + 主线程起草后复测证明**该描述不成立**——真实判官 verdict 形态 **≥8 种、跨两套惯例**（§7 内联 append / 独立 `review-*.md` 文件），其中 `**GREEN（可 FROZEN）**`（行首加粗独立形态，6 处、跨 4 批）与 Status 行轮次指针 `R2 GREEN〔review-r2-architect.md〕` **都不含 `verdict/结论` 标签**。
- **「恒真」的真机制与 §1 原归因不同**（v0.5 已更正）：`PROPOSAL.template.md` 头部的样板行「**结构字段硬条款**：…：**GREEN 结论前必须在场**」**自带 GREEN**，实测存在于 **16 / 26** 份 arch PROPOSAL，**它单独一个就让现行 regex 为真**——闸对这些批恒真，与判官判没判无关。（v0.1 曾把恒真归因于判官 verdict 行命中裸分支，该归因**不完整**，以本条为准。）
- **四个候选设计全部实测不成立**（对全部 26 份 arch PROPOSAL 逐份实测）：

  | 候选 | 假阴性（**错拦合法合并**） | 假阳性（闸仍虚设） |
  |---|---|---|
  | A · 行首/引导形 | **3 个已冻结批被漏** | — |
  | B · 行内宽松 | 0 | 太松（连当时的四份草案都判真） |
  | C · 判定陈述位置 | **3 个已冻结批被漏** | 治理类草案仍误判真 |
  | 只排样板行 | **0** | 仍有 5–6 份非 GREEN 文本判真——**包括治理类正文自己讨论 GREEN 时** |

  最后一行是关键：治理批本来就最可能正文提 GREEN，「只排样板行」对**恰恰是治理批**这一类几乎无改善。
- **结论**：这不是「一行 regex 替换」能解决的，是**判据设计问题**——本闸该验的是什么（严格 token / verdict 区扫描 / 机器锚 / 判官 verdict 行规范化），需要在「错拦合法合并」与「闸继续虚设」之间做一次明确取舍。
- **建议下一批的处置方向（不自批，交 review）**：优先评估「判官 verdict 行规范化 + 机器锚」——即要求判官 verdict 写成固定机器可读形态，锚只认该形态；老批已合并、不追溯（闸只在**被合并批**上跑）。该向零假阴性、判据可机判、与 `deriveLanded` 的 JSON 行锚同思路；代价是要动 `PROPOSAL.template.md` + 判官简报（**跨 G2 面**，R1-F1 注已提示「不得顺手夹带」）。
- **队列状态**：`ARQ-260909-gate-green-anchor-regex-loose` **仍在册未结**，已在 `docs/ARCH-REVIEW-QUEUE.md` 补注本批实测结论（处方证伪 + 真机制 + 四候选实测），供下一批直接引用，不必重付本批的诊断成本。

**② 同族「追踪产物文件」的系统性形态**——本批只治 `overlap-probe.json` 这一件。是否还有其他「merge-driver / 闸每次运行重写、却被误入库」的产物，本批未做 census。**建议下个动 `delivery/taskpackages/**` 治理面的批顺手清点**。

**③ `ARQ-260917-step5b` 的 ①②③ 三条处置状态**——①（`pod-events.jsonl` 配 `merge=union`）已由 `c16283b` 落地、r15 实测收口；②（Step 5b 簿记落点迁出被合并路径）**未做**，本批不承接（改合并主流程，半径过大）；③ 本批 T1② 已核并成文。**② 留根**：若将来仍出现簿记类 EOF 双追加的新形态，再评估。

**④ 拆包族另三批**（`-dispatch-chain-friction` / `-closeout-chain-friction` / `-batch-drafting-friction`）——本批不承接。

**⑤ 移出本族的两个 launch 面 ARQ**（`ARQ-260909-precedent-merged-detection-gap` 输入清单先例取旧、`ARQ-260914-portfolio-reconcile-launchpreconditions` launch 前置清单不核）——按 human 2026-09-18 裁定留在 ARQ 队列、后续单独处理。

**交日落复盘项**：本批 T1① 撤跟踪后，建议在 `/loop-retro` 观察——是否仍出现「merge 前 rebase 撞 unstaged」的同族事故。若出现，即清点未穷举或有新的追踪产物形态。

**自律件**：起草期已按硬项 1a 亲核；v0.5 的 T1 移出为 human 裁定 + R1 实证双重驱动，非主线程自裁；R1 记录（§7）保持 append-only 原样。
## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-19 - architect

**verdict：BLOCK(F1, F2)**——T1 收窄方向（向严）、T2 两侧（11 件清点穷举、union 结论成文）、§1 地面真值引述全部亲核成立；但 T1 候选 regex 有一处 BLOCKING 闭合缺口（漏掉两类**在册**合法 verdict 书写形态）与一处未申报放宽面，须修订后复审。F3–F5 为 NIT 不阻塞。**无升级 human 项**：F1/F2 均为可机械修订的闭合/申报缺口，非方向性错误、非 riskTier/profile 误判，与 OQ1/OQ2 已裁事项（Track B / 11 件全撤）无实质冲突——改法集恰为 T1+T2、撤跟踪无未申报影响（跨工作树过渡预期见 F5）。

#### Findings

**F1 · BLOCKING · T1 闭合性缺口——收窄 regex 漏掉两类在册合法 verdict 形态**

亲核（2026-09-18/19 现跑：node 对全部 25 份 `delivery/changes/arch/*/PROPOSAL.md` 逐份做现行 regex（:84）与候选 regex 双对照）：**8 份 flip（cur=true → nar=false）**，其中三份是**真实 GREEN 判定在册、却无任何 `verdict/结论` 标签形态**的近期批：

- `260914-gov-review-delivery-restructure/PROPOSAL.md:504`（verbatim）：`**GREEN（可 FROZEN）**。四轮评审全部收口：……`——判官终审 verdict 为**行首加粗独立形态**，全文无 `verdict：`/`结论：` 标签行。该批已 landed（INDEX:19），但形态出自 2026-09-18 前五天的真判官输出，非构造例。
- `260915-gov-review-worktree-pool-reuse/PROPOSAL.md:3`（verbatim 摘）：`……主线程回填 v0.5 → R2 GREEN〔review-r2-architect.md，N3/N4 NIT 已在冻结前收进 T1②/V8〕）。`——GREEN 证据住在**兄弟评审文件**（review-r2-architect.md），PROPOSAL 正文仅 Status 行**轮次指针形态** `R2 GREEN〔…〕`。已 landed（INDEX:17）。
- 旁证：`260830-arch-review-ui-impl-strategy/PROPOSAL.md:215` `### R2 · architect 复审 · 2026-08-31 · **GREEN**`（标题内嵌 GREEN、无标签）——该批仅因 :227 `**R2 结论：GREEN。**` 在场才未 flip。
- 另一自证：`260908-gov-review-human-deputy-shadow` 的 green 现值**仅由** :10 样板句「**GREEN 结论前必须在场**」的裸字样支撑（§1「恒满足」断言的又一实证——该样板句存在于近年每份 PROPOSAL 头部）。

后果（消费链亲核）：`pre-merge-escalation-gate.mjs:286` `else if (!a.green) push('Track B：§7 无 architect GREEN verdict');`——未来任何 FROZEN 批若判官按上述在册形态落 verdict，前置升级闸即**假阴性 BLOCK**（fail-closed 错拦合法形态 + 例行升级噪音）。本批 §0 自我定位是治合并链摩擦，该缺口恰会生成**新的**合并链摩擦；§6 交日落复盘项把「收窄过头的假阴性」设为落日后观察对象，但 census 证明缺口**起草期即已存在**，V2 三夹具未覆盖、V1 也无对应闭合性夹具。修法方向（主线程择一，均可机械落）：
- (a) 候选 regex 增补两支在册形态：轮次指针形态（如 `\bR\d+\s*GREEN\b`，覆盖 `R2 GREEN〔…〕`/`R3 GREEN ✓`/Status 凭据行）与行首加粗独立形态（如 `^\s*\*{0,2}GREEN\b` 配 `m` 标志；放宽面须如实申报）；
- (b) 或维持候选 regex 不动、把「接受形态语法」成文进 §2 + V1 增补上述两形态为**闭合性夹具**（明确断言它们判不满足是**设计决定**而非疏漏）+ 同步改 :83 注释声明该契约；
- 注：改 track-b-judge-brief 强制 verdict 行带标签属 G2 面、不在本批 manifest——若走该向需跨批协调，不得顺手夹带。

**F2 · SHOULD（打回）· 候选 regex 含两处 §2 未申报的放宽：`|PASS` 分支与冒号可选化**

§2 改法理由只申报了「去掉裸分支、并容忍 `**` 与空白」，但候选 regex `/(?:verdict|结论)[：:]?\s*\*{0,2}\s*(?:GREEN|PASS)\b/gi` 相对现行还做了两件 §2 未写的事：
1. **新增 `PASS` 为可接受 token**——全仓 25 份 PROPOSAL 无任何真实判官 `verdict: PASS` 形态（唯一字面命中 = `260906-gov-review-index-status-truth-source/PROPOSAL.md:168` 对 landing-verifier 规范行 `> verdict: PASS/GREEN` 的 **prose 引述**——该字符串本身即能命中候选 regex，等于新增一条 spoof 面）；判官 verdict 语法（track-b-judge-brief）= GREEN / BLOCK(F1..Fn)，:83 注释也仍写「判官 GREEN verdict」。本批的根因论点就是「注释意图与实现脱节」，候选 regex 又引入一处同病（注释说 GREEN、实现收 PASS）。
2. **冒号可选化**（`[：:]?`；现行为必选 `[：:]`）——接受 `verdict GREEN` 无冒号形态，全仓无此形态需求，同样未申报。

修法：删 `|PASS` 并恢复必选冒号，或两处放宽面在 §2 如实申报 + 同步改 :83 注释 + V1 补对应夹具（现 V1 两夹具不含 PASS、不含无冒号形态）。

**F3 · NIT · 全文扫描残余假阳性**：green 锚测的是整份 PROPOSAL 文本而非 §7 切片，prose 引述标签形态仍可过关——自证：本 PROPOSAL 当前 nar=true 而 §7 在本轮 append 前为空（命中点 = §1 `「verdict：GREEN」` 引述与 §2/V2 夹具文本）；260908-input-surface:300 范围外观察行同样命中。:83 注释「同段」意图只部分实现。可选修法：text 切 §7 段或行锚化；不改不阻塞（FROZEN 行 + landing-verdict 存在性双锚仍把守）。

**F4 · NIT · `g` 标志挂在 `.test()` 上**：产线安全（readGovAnchors 每次调用新建 regex 字面量），但对 V2 式复测是 stateful 脚枪——本轮亲核首跑即因 `lastIndex` 残留得到一次假 `false`，换新实例复跑才对。建议去掉 `g`（`.test()` 不需要）或自检夹具内注明该险。

**F5 · NIT · T2 撤跟踪的跨工作树过渡力学未申报**：V4 只核 gov 侧「untracked + 在盘」；ff 合并进 main（`C:/AI/KidPal`）时这 11 件会从 main 工作树**盘上删除**（tracked-deleted），Pod 工作树若带本地脏 probe 恰会在过渡期撞一次「local changes would be overwritten」——正是本批要治的形态。文件为可再生运行产物（gate `:555` 每跑 `--probe-out` 重写、merge-driver `:1257` 传入该路径），无数据丢失；建议 landing summary 一句申报过渡预期。

#### 亲核留痕（地面真值）

- `pre-merge-escalation-gate.mjs:83-84` verbatim 与 §1 引文逐字一致 ✓；`:199`/`:200` 分支逻辑与 T2② 描述相符（merge=union 文件 git 自动并不入 conflictedFiles → :199 union-safe；:200 `VERSIONED_ARTIFACT_RE_LOCAL` carve-out 为另一并列路径）✓；`:499`/`:507-508` self-check CLI 入口 ✓；`:283-286` green 消费链 ✓；`:553-555` probe-out 每跑重写 ✓。
- 三夹具独立复跑（每测新建 regex 实例，规避 F4 的 lastIndex 态）：260908-input-surface PROPOSAL 全文（真阳性）nar=true（命中 :288 `### R4 · architect verdict：GREEN…`）；合成反例 nar=false、现行 true（裸分支）；合成正例 true/true——与 §1 三夹具表逐格一致 ✓。
- `.gitignore:46` = `overlap-probe.json` ✓；`git ls-files | grep -c overlap-probe.json` = **11**、清单与 manifest 11 路径**逐条 1:1** ✓；`c1bd37b`/`c16283b` 经 `git rev-parse` 核实存在 ✓；`pod-coordination.md:183`、`merge-driver.mjs:1254`（及 :1257/:1265-1268 读点）verbatim ✓。
- ARQ 队列两原文在场：`ARQ-260909-gate-green-anchor-regex-loose`（queue:49）、`ARQ-260917-step5b-main-append-remerge-tier3`（queue:32，含 r15「tier3 评审噪音收口」实测句）✓——T2②「无需豁免」结论与代码分支 + ARQ 实测双证一致，**维持**。
- 消费方 sweep：overlap-probe.json 无 git 内消费者——merge-driver 读写工作树副本、gate 自检夹具本地构造 JSON（:483-488）、adjudication 文档仅作历史证据引用；撤跟踪不破坏任何读取点 ✓（盘面过渡预期见 F5）。
- 260908-input-surface 自证链：`architect-review-r1.md:162`（「本批不触该脚本、非本批 finding；建议登记 ARQ」）+ 该批 PROPOSAL:300（r4 范围外观察「另登 ARQ…处置恰当」）——「两轮已见未修」属实 ✓。
- 结构字段：landing-manifest 围栏块在场（12 路径 = 1 脚本 + 11 probe，与 §2 目标文件集精确闭合）✓；packet-id 行在场 ✓。
- V1–V7 逐条机械可执行 ✓（V1 现有 gov 档夹具 :404-408 可扩；V7 锚集 = manifest 12 路径自洽）。

**结论**：T2 两侧无保留；T1 方向与夹具方法正确，但候选 regex 的闭合性（F1）与申报完整性（F2）须修订。预期 R2 聚焦 F1/F2 修订到位性 + 是否引入新矛盾，无需全量重核。