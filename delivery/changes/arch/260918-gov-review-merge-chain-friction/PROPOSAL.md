# PROPOSAL · 260918-gov-review-merge-chain-friction

> **状态：DRAFT v0.1 · 2026-09-18 · 待 human 立项 ack**（FROZEN 行待补凭据：human 立项 ack + architect Rn GREEN + human 裁 OQ 结果）。
> **承载 ARQ**：`ARQ-260909-gate-green-anchor-regex-loose`（T1）、`ARQ-260917-step5b-main-append-remerge-tier3` 残余③ + 同族 probe 输出文件（T2）。不关的根写 §6 留根指针。
> **母批关系**：本批为 `260918` 拆包族之一（原合并草案 `260918-gov-review-loop-friction-sweep` 已按 human 2026-09-18 裁定拆为四批，逐批 review；原草案已删，其证据已按批内聚进各自 §1）。族内另三批 = `260918-gov-review-dispatch-chain-friction` / `-closeout-chain-friction` / `-batch-drafting-friction`。**另有两项（输入清单先例取旧、launch 前置清单不核）按 human 裁定移出本族、留 ARQ 队列后续单独处理**。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。两条升级判据（触契约面 / 跨系统结构性变更）均未命中。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T2）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（一句话证成：T1 触**前置升级闸的判定语义**——属本仓最承重的一道闸；但改动方向为**收窄 / 向严**（把当前恒真的关恢复成真判），无放松面。T2 为索引面撤跟踪 + 一行注释，无数据丢失形态）。
> **review profile**：G3（`.claude/scripts/pre-merge-escalation-gate.mjs`）+ 索引态变更（11 件 `overlap-probe.json` 撤跟踪）。契约面 / G1 / G2 均为零；无 agent def 改动。
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

**目标效果一句话**：把合并链上两处「看着在守、其实没守」的机械位修回真判——升级闸的判官 GREEN 机锚从恒真恢复成真判；合并探针产物的忽略配置从其失效的已跟踪态恢复成生效。

**现状差距**：这两条的共同形态是**修复动作本身是假的**——T1 的规则注释写对了、代码没跟上，于是该关恒真；T2 上一轮修复加了忽略条目，但该条目对已入库文件天然不生效，于是病灶原样保留。两条都已实测确认，都不是推测。

**非目标（本批不做什么）**：不改 `merge-driver.mjs` 的合并主流程；不改 `.gitattributes`（`delivery/taskpackages/**/pod-events.jsonl merge=union` 已由 `c16283b` 落地、实测收口，本批原样保留）；不动任何 agent def；不并入同族另三批的任何 T。

## 1. 问题与根因（地面真值）

**【硬项 1a 声明】** 本节全部断言均为 2026-09-18 在 `gov/kidpal` 工作树上现跑实测所得，命令与关键输出逐条留痕；所有 commit SHA 经 `git rev-parse` 核实；所有行号为本批起草时的现行值。

### 问题 1：前置升级闸 gov 档「判官 GREEN」机锚含裸分支，该关恒真

证据（verbatim 亲核）：`.claude/scripts/pre-merge-escalation-gate.mjs:83-84` =

```
  // §7 含判官 GREEN verdict：宽松匹配「GREEN」且「verdict」同段（拒绝仅 history 提及 GREEN）
  const green = /verdict[：:]\s*GREEN|结论[：:]\s*GREEN|GREEN/gi.test(text);
```

上一行注释声明的意图是「**宽松匹配且『verdict』同段**（拒绝仅 history 提及 GREEN）」，而 regex 末支那个**裸 `GREEN` 无上下文备选分支**恰使该意图失效——三支是「或」关系，裸分支令任何正文出现 `GREEN` 字样的 PROPOSAL 恒满足该关。

**自证（该批自己的记录）**：`delivery/changes/arch/260908-gov-review-input-surface-frontload/PROPOSAL.md` §7 判官 verdict 行自带「verdict：GREEN」字样即过关；该批 `architect-review-r1.md` 范围外观察与 `architect-review-r4.md` 残余观察 2 **两轮判官均已判「本批不触该脚本、另登恰当」**——病灶在评审侧被两次看见、两次未修。

**起草期现跑实测（2026-09-18）**：

| 夹具 | 现 regex | 收窄后 |
|---|---|---|
| `260908-gov-review-input-surface-frontload` 的 PROPOSAL 全文（真阳性） | `true` | `true`（**不应误伤**） |
| 合成反例 `本次评审覆盖 GREEN 配色区域，无判定行。` | `true` | `false`（**这就是病灶**：无判定行的正文也能过关） |
| 合成正例（`> verdict：GREEN` 三行块） | `true` | `true` |

根因：注释意图与实现脱节——收紧形态写进了注释、没写进 regex。

### 问题 2：`overlap-probe.json` 的 `.gitignore` 条目对已跟踪文件不生效

证据（亲核）：`.gitignore:46` = `overlap-probe.json`（在场）；`git ls-files | grep -c overlap-probe.json` = **11**（11 件仍被跟踪）——`git ls-files --error-unmatch delivery/taskpackages/260915-m1-k3-voice-round-ui/overlap-probe.json` 退出 0，确认 tracked。`.gitignore` 只对**未跟踪**路径生效，已入库路径不受其约束。

**病灶形态**：`overlap-probe.json` 是合并期闸的**运行产物**——`.claude/references/pod-coordination.md:183` 记「升级闸 `parallelOverlapProbe`（只读 merge-base 交集 + merge-tree 实撞，`--probe-out` 落 overlap-probe.json）」；`merge-driver.mjs:1254` 每次运行读它。而 merge-driver 的内部闸**每跑必重写**它。tracked 态 ⇒ 每次 merge 前工作树带 unstaged 改动 ⇒ rebase 恒撞「local changes would be overwritten」。

**前序消解不完整**：commit `c1bd37b`（`git rev-parse c1bd37b` 核实存在，2026-09-17）做了两件事——加 `.gitignore` 第 46 行、改一件 probe 文件内容——**但未 `git rm --cached` 存量 11 件**，故该条目对它们失效，病灶原样保留。

**残余③核查结论（起草期已完成，非待办）**：`ARQ-260917-step5b-main-append-remerge-tier3` 遗留的「升级闸对 merge=union 文件的探测口径是否需显式豁免」已核——**无需改动**。依据（verbatim 亲核）：`pre-merge-escalation-gate.mjs:199` =

```
      if (!conflictedFiles.length) return 'union-safe'; // 相交但可自动并（不同 hunk git 自并）
```

`merge=union` 文件由 git 自动并、不出现在 `conflictedFiles`，天然落该分支判 `union-safe`（② 级不升人，`:200` 的 `VERSIONED_ARTIFACT_RE_LOCAL` carve-out 是另一条并列路径、非本情形依赖）。同 ARQ 记录 k3 修复轮 r15 合并实测「tier3 评审噪音收口」与此一致。故该项从「待办」降为「已核」，结论写进代码承载点（T2）。

## 2. 改法（T1–T2）

### T1 · 前置升级闸 gov 档判官 GREEN 锚收窄至「verdict/结论 同段」形态

承载 `ARQ-260909-gate-green-anchor-regex-loose`。

改 `.claude/scripts/pre-merge-escalation-gate.mjs:84`：把

```
  const green = /verdict[：:]\s*GREEN|结论[：:]\s*GREEN|GREEN/gi.test(text);
```

替换为**去掉裸分支、并容忍 `**` 与空白**的形态：

```
  const green = /(?:verdict|结论)[：:]?\s*\*{0,2}\s*(?:GREEN|PASS)\b/gi.test(text);
```

为什么这样改：直击问题 1 的根因——**让实现与上一行注释声明的意图一致**（「宽松匹配且『verdict』同段」），把裸 `GREEN` 备选分支去掉。收窄后行为方向为**向严**（原来恒真的关恢复成真判）。同文件自检电池补两夹具：**反例**（合成文本含 `GREEN` 字样但无 `verdict/结论` 标签行 → 判不满足）须 FAIL、**正例**（`> verdict：GREEN` 三行块形态 → 判满足）须 PASS；自检入口 `--self-check` 现存（`:507` 起为 CLI 入口、`:499` `isDirectRun()` 守卫）。

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改）

### T2 · `overlap-probe.json` 撤出跟踪 + 升级闸 union 探测口径核查结论成文

承载 `ARQ-260917-step5b-main-append-remerge-tier3`（残余③ + 同族 probe 输出文件）。

① **撤跟踪 11 件存量**：对 §landing-manifest 列出的 11 个 `overlap-probe.json` 路径执行 `git rm --cached`（**只出索引、工作树文件保留**），使 `.gitignore:46` 的既有条目真正生效。为什么：直击问题 2 的根因——`.gitignore` 对已跟踪路径不生效，`c1bd37b` 只加了条目未撤跟踪，病灶原样保留。撤跟踪后 merge-driver 每跑重写的该文件回到 untracked 态，rebase 撞 unstaged 的形态消除。

② **核查结论成文**：在 `.claude/scripts/pre-merge-escalation-gate.mjs` 的 `:199`（`if (!conflictedFiles.length) return 'union-safe';`）处补一行注释，记录起草期已完成的核查结论——`merge=union` 文件由 git 自动并、不出现在 `conflictedFiles`，天然落该分支判 `union-safe`，**无需显式豁免**（依据 = 该行分支逻辑 + ARQ-260917 记录的 k3 修复轮 r15 合并实测「tier3 评审噪音收口」）。为什么：该结论原为队列里的「待办」，核查后降为「已核」；写进代码承载点比留在队列里耐久，且后人改这条分支时会看到它。

**不改** `merge-driver.mjs` 的合并主流程、不改 `.gitattributes`。

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改：注释一行）、`delivery/taskpackages/260829-hardware-validation/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260901-ui-art-integration/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-display-first-m1/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-ecs-deploy-convergence/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-key-route-a1/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-render-pages/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-screen-unification/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260915-m1-k1a-key-layer-dispatch/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260915-m1-k2-keybar-three-slot/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260915-m1-k3-voice-round-ui/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260915-m1-n1b-p1-home-battery/overlap-probe.json`（撤跟踪）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 前置升级闸 | `.claude/scripts/pre-merge-escalation-gate.mjs` | G3（**判定语义改动，如实声明**） | T1 收窄 GREEN 锚；T2② 补注释。消费方 = merge-driver T1/Step 1b 透传、landing 前置升级闸、`--self-check` |
| 合并运行产物 | 11 × `delivery/taskpackages/*/overlap-probe.json` | **撤跟踪** | T2① 只 `git rm --cached`、工作树文件保留；无源码消费方（merge-driver 每次自写） |
| 契约面 / G1 / G2 / agent frontmatter | — | **未触** | 零改动 |
| 工程门新增 | — | **零新增** | 本批只改既有闸的判定形态，不新增门 |

**【硬项 1b · sweep 透镜】** T1 落地新判定 pattern，起草期已预跑原始输出（见 §1 问题 1 的三夹具表，含正例保全与反例转红两侧）。**本批不引入新枚举面**，故无 census 需要展开。

**【硬项 1c · 复扫模式完备性先验】** 本批 T2① 涉**同族残留清扫**——模式集为**显式枚举 11 条路径**（`git ls-files | grep -c overlap-probe.json` = 11，已逐条列入 manifest），非模式匹配，**完备性论证**：清点以 `git ls-files` 为准（索引态本身即穷举），不依赖任何 glob 或词形变体假设。T1 不涉退役语面。

## 4. human 取舍点（OQ）

### OQ1 · T1 触前置升级闸的判定语义——本批走 Track B 判官判、还是升 full-adversarial + human

**背景（必须给全）**：T1 改的是**前置升级闸**——即「Pod 分支能不能并入 main」这道关卡的判据之一。承载 ARQ 自己写了这句：「**review profile**：Track B（执行测出，不论 G 几）**但机检事实如实声明：此项属「修改 gate 判定」，是 lightweight 机检硬排除**——若 human 判该收紧本身该走重道，按 protocol 转 `full-adversarial + human` 不回标快路径」。而两条先例方向不一致：260907 批把 DEV-GOVERNANCE 新节按 Track B 落了，`ARQ-260908-versioned-guard-version-literal-coupling` 却把「新增/改闸判据」判为硬排除 → full。**按 AGENTS.md「存疑向严」，该判断显式交 human，不自裁。**

**选项**：
- **(a) 留 Track B**（判官按 `track-b-judge-brief` 全面审；判官或 human 任一方随时可启「声明红队」升级）。**推荐**。理由：(1) 不命中 AGENTS.md 的两条 Track A 判据；(2) 改动方向是**收窄 / 向严**，无放松面；(3) 半径已由起草期三夹具**照出**（正例保全、反例转红两侧都有实测）；(4) 拆包后本批只剩**一处**闸语义触点，正是判官最该盯且能盯透的量。
- **(b) 升 full-adversarial + human**（走 `/arch-change` full 档）。理由：闸语义的**闭合性**——收窄后会不会漏掉某种合法的 verdict 书写形态——不是单轮判官能穷尽的，红队更稳。
- **(c) 折中**：Track B 但**加派一个独立复核人**（第二判官实例复核 T1 单点），其余照常。理由：用最小代价给唯一的闸语义触点加一道独立眼看。

### OQ2 · T2 的撤跟踪范围

**(a) 11 件全撤**（推荐；`git rm --cached`，工作树文件保留、历史不变）。理由：清点以索引态为穷举、一次撤干净，避免「同族形态可复发」的教训再留尾巴。**(b) 只撤近三批**（260915 三件，早期批次已收口、其 probe 不再被重写）。理由：改动面最小。**(c) 不撤，改 merge-driver 落点**（把 probe 输出移出被合并路径）。理由：从形态上根治，但要动合并主流程、半径大于本批定位，与本批 §0 非目标冲突。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**【真空绿规约】** V1 为 `--self-check` 电池，其**反例夹具须先自证输入确实落在目标形态之外**（合成反例须断言正文确无 `verdict`/`结论` 标签行，否则「反例不红」可能只是夹具没造对）；自检输出须含**新增夹具名**（防「加了夹具但没接线」的真空绿）。

| 项 | 命令 | 预期 |
|---|---|---|
| V1 | `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` | 全绿，且**新增两夹具**（GREEN-裸字样反例 / `verdict：GREEN` 正例）在输出中具名出现 |
| V2 | 三夹具现跑（合成反例 / 合成正例 / `260908` 批 PROPOSAL 真阳性） | 反例：现 `true` → 收窄后 `false`；正例与真阳性两侧均 `true`（**前后各留一次输出**） |
| V3 | `git ls-files \| grep -c overlap-probe.json` | 撤跟踪后 = **0** |
| V4 | `git status --porcelain delivery/taskpackages/*/overlap-probe.json` | 显示为 untracked、**工作树文件仍在盘**（`ls` 逐条确认 11 件在场） |
| V5 | `git log --oneline -1 -- delivery/taskpackages/260829-hardware-validation/overlap-probe.json` | 该文件的历史仍在（撤跟踪不改历史） |
| V6 | `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` | 全绿；lint 只允许既有告警（`landing-verifier.md` 超长，本批未触该文件） |
| V7 | `landing-fidelity-check.mjs <本批>` 结构门 | §2 锚集 ≡ manifest 机判通过（本批 12 路径） |

## 6. 留根清单

**不关的根**：

1. **`ARQ-260917-step5b` 的 ①②③ 三条处置状态**——①（`pod-events.jsonl` 配 `merge=union`）已由 `c16283b` 落地、r15 实测收口；②（Step 5b 簿记落点迁出被合并路径）**未做**，本批不承接（改合并主流程，半径过大）；③ 本批 T2② 已核并成文。**② 留根**：若将来仍出现簿记类 EOF 双追加的新形态，再评估。
2. **同族「追踪产物文件」的系统性形态**——本批只治 `overlap-probe.json` 这一件。是否还有其他「merge-driver / 闸每次运行重写、却被误入库」的产物，本批未做 census。**建议下个动 `delivery/taskpackages/**` 治理面的批顺手清点**。
3. **拆包族另三批**（`-dispatch-chain-friction` / `-closeout-chain-friction` / `-batch-drafting-friction`）——本批不承接。
4. **移出本族的两个 launch 面 ARQ**（`ARQ-260909-precedent-merged-detection-gap` 输入清单先例取旧、`ARQ-260914-portfolio-reconcile-launchpreconditions` launch 前置清单不核）——按 human 2026-09-18 裁定留在 ARQ 队列、后续单独处理。

**交日落复盘项**：本批 T1 收窄后，建议在 `/loop-retro` 观察——是否出现「因收窄而漏过的合法 verdict 书写形态」（即该关的新假阴性）。若出现，即收窄过头的信号，届时补形态而非回退。

**自律件**：起草期已按硬项 1a 亲核（§1 全部断言带命令与日期）；三夹具为现跑实测、非构造描述。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）