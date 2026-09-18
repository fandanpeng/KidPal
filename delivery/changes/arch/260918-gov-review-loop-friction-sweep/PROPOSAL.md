# PROPOSAL · 260918-gov-review-loop-friction-sweep

> **状态：DRAFT v0.1 · 2026-09-18 · 待 human 立项 ack**（FROZEN 行待补凭据：human 立项 ack + architect Rn GREEN + human 裁 OQ 结果）。
> **承载 ARQ**：本批为**清账批**，一次承接 14 条在册 ARQ（逐条对应见 §2 各 T 的「承载」行）：`ARQ-260909-gate-green-anchor-regex-loose` / `ARQ-260909-index-verdict-anchor-okform-gap` / `ARQ-260916-acc-gate-archive-skew` / `ARQ-260909-precedent-merged-detection-gap` / `ARQ-260917-step5b-main-append-remerge-tier3`（残余③ + 同族 probe 输出文件）/ `ARQ-260914-portfolio-reconcile-launchpreconditions` / `ARQ-260913-current-pointer-implicit-global` / `ARQ-260914-lfc-citation-references-prefix` / `ARQ-260907-lfc-citation-docs-prefix`（残余①）/ `ARQ-260914-landing-preflight-anchor-parse` / `ARQ-260908-tighten-only-unwritten` / `ARQ-260913-auto-mode-classifier-legit-workflow` / `ARQ-260913-device-round-protocol` / `ARQ-260915-path-form-family-reference`。不关的根写 §6 留根指针。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。两条升级判据（触契约面 / 跨系统结构性变更）均未命中——本批不触 `docs/contracts/**`、不动固件与服务端语义。**但 §4 OQ1 单列一个「重道 vs 快道」判断点**：本批含多处闸判定语义改动，多条承载 ARQ 自陈「属 lightweight 机检硬排除、若 human 判该走重道即转 full-adversarial + human」，按 AGENTS.md「存疑向严」把该判断显式交给 human。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T13）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（一句话证成：触点含前置升级闸与 launch 检查器的**判定语义**，且 T7 引入新的 fail-loud 路径；但各项改动方向均为**收窄 / 向严 / 向后兼容**——T1 收紧恒真锚、T3 收窄枚举面、T6 键缺席即视为无前置、T7 的新硬失败仅在调用方显式传 `--expect-change-id` 时生效。无数据丢失形态、无不可回滚动作）。
> **review profile**：G2 + G3（G3 = `.claude/scripts/**` 七个脚本；G2 = `.claude/references/**` 两件 + `.claude/skills/kidpal-serial-monitor/SKILL.md` + `docs/templates/PROPOSAL.template.md`；**G1 一件 = `docs/DEV-GOVERNANCE.md`「验收测试纪律」新增 §5**——非契约面，承 260907-gov-review-arq-governance-batch-2 T8「新节 + 引用行」同形态先例；契约面为零）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/references/path-form-family.md
.claude/references/pod-coordination.md
.claude/scripts/acceptance-run-check.mjs
.claude/scripts/brief-inputs.mjs
.claude/scripts/changes-index.mjs
.claude/scripts/dispatch-stamp.mjs
.claude/scripts/landing-fidelity-check.mjs
.claude/scripts/portfolio-reconcile.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/skills/kidpal-serial-monitor/SKILL.md
docs/DEV-GOVERNANCE.md
docs/templates/PROPOSAL.template.md
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

**目标效果一句话**：把 dev-loop 与治理环上**「已经查清、改法确定、却还挂在队列里」**的 14 条摩擦一次清掉——机器面让闸的判定与事实对齐（该报的报、不该报的不报、判不了的要看得见），文档面让已经反复付过学费的纪律住进耐久的家。

**现状差距**：这 14 条的共同形态是「病灶已实测、消解方案已收敛到单一路径、却因散在 14 条 ARQ 里而无人承接」。它们不是设计问题，是**排期问题**——单条价值低到不值得为它开一批，累积起来却持续消耗每一次合并与每一次 launch。典型三例：

- 前置升级闸的「判官 GREEN」机锚现**恒真**（T1）——该关的实际安全性只剩 FROZEN 行与 `landing-verdict.md` 存在性两锚把守；
- `delivery/changes/arch/INDEX.md` 现有多批显示 `frozen | 已合main=yes`（T2）——已落地却推不出 landed，正是那条锚形态缺口的活证据；
- `overlap-probe.json` 的 `.gitignore` 条目对**已跟踪文件不生效**（T5）——11 件仍被跟踪，merge-driver 每跑必重写它们，rebase 撞 unstaged 的病灶原样保留。

**非目标（本批不做什么）**：不改任何 agent 的 frontmatter（tools / name / model / description 全不动）；不动 `docs/contracts/**` 与系统 spec 两文件；不动验收冻结件（`*.acceptance.*`）；不新增工程门（T9 的 `--preflight` 是**只读前置自检**、不参与任何门判定）；不改 `merge-driver.mjs` 的合并主流程（T5 只撤跟踪 + 补一行核查结论注释）；不并入 `ARQ-260911-*` 四条承载类设计项（需真设计判断，另批，见 §6）。

## 1. 问题与根因（地面真值）

**【硬项 1a 声明】** 本节全部断言均为 2026-09-18 在 `gov/kidpal` 工作树上现跑实测所得，命令与关键输出逐条留痕；所有 commit SHA 经 `git rev-parse` 核实；所有行号为本批起草时的现行值。

### 问题 1：前置升级闸 gov 档「判官 GREEN」机锚含裸分支，该关恒真

证据（verbatim 亲核）：`.claude/scripts/pre-merge-escalation-gate.mjs:83-84` =

```
  // §7 含判官 GREEN verdict：宽松匹配「GREEN」且「verdict」同段（拒绝仅 history 提及 GREEN）
  const green = /verdict[：:]\s*GREEN|结论[：:]\s*GREEN|GREEN/gi.test(text);
```

上一行注释声明的意图是「**宽松匹配且『verdict』同段**（拒绝仅 history 提及 GREEN）」，而 regex 末支那个**裸 `GREEN` 无上下文备选分支**恰使该意图失效——三支是「或」关系，裸分支令任何正文出现 `GREEN` 字样的 PROPOSAL 恒满足该关。**自证**：`delivery/changes/arch/260908-gov-review-input-surface-frontload/PROPOSAL.md` §7 判官 verdict 行自带「verdict：GREEN」字样即过关（该批 `architect-review-r1.md` 范围外观察与 `architect-review-r4.md` 残余观察 2 两轮判官均已判「本批不触该脚本、另登恰当」）。根因：注释意图与实现脱节——收紧形态写进了注释、没写进 regex。

### 问题 2：`changes-index` 终态锚不识 `ok: true（GREEN）` 标题行，landed 推导静默不动

证据（verbatim 亲核）：`.claude/scripts/changes-index.mjs:177-178` 两条锚定义 =

```js
const VERDICT_STATUS_RE = /^\s*(?:>+\s*)?(?:\*{0,2})\s*(?:verdict|总判|总判定|结论|复验结论|判定)\s*(?:\*{0,2})\s*[:：=]\s*\*{0,2}\s*(PASS|GREEN|RED|FAIL|BLOCK)\b/i;
const VERDICT_HEADING_RE = /^#{1,4}\s*(?:\d{1,3}[.、]\s*)?(?:verdict|总判|总判定|结论|复验结论)\s*[:：]?\s*\*{0,2}\s*(PASS|GREEN|RED|FAIL|BLOCK)?\b/i;
```

两条都要求行以 `verdict|总判|结论|…` 标签起头；`VERDICT_HEADING_RE` 另要求 `#` 标题形态。**`**ok: true（GREEN）——…**` 属加粗行、既无标签前缀也非标题**，两通道皆不命中。

**活证据（本批起草期现跑，命令 = `cat delivery/changes/arch/INDEX.md`）**：现行 INDEX 中以下批次显示 `frozen | 已合main = yes`——已落地却推不出 landed：

| changeId | INDEX 状态 | 实际 |
|---|---|---|
| `260908-gov-review-merge-chain-hardening` | frozen | landed（architect-landing-summary manifest 8/8 全落） |
| `260907-gov-review-merge-friction` | frozen | landed |
| `260908-gov-review-escalation-conflict-ladder` | frozen | landed |
| `260904-gov-review-escalate-merge-approval` | frozen | landed |
| `260907-gov-review-architect-dispatch-model-override` | frozen | landed（`agent-model-guard.mjs` 在场） |
| `260830-arch-review-ui-impl-strategy` | frozen | landed |

（另有三条 `draft | yes` 同族形态。）根因：`deriveLanded` 的三通道（标签+冒号 / 编号标题 / JSON 行锚）是对既有 verdict 书写习惯的**事后归纳**，未覆盖 `ok: true（GREEN）` 这一已被使用的形态；ARQ-260909-index-verdict-anchor-okform-gap 已在 260909-gov-review-gate-truthfulness 合并簿记期实测记录同一现象，本批 §1 的表格是其**跨批复现**（非孤例），故按该 ARQ 候选①（补第四锚）处置。

### 问题 3：`acceptance-run-check` 枚举不跳 `archive` 退役面，且退役保留名约定无处成文

证据（verbatim 亲核）：`.claude/scripts/acceptance-run-check.mjs:34` =

```js
const SKIP_DIRS = new Set(['node_modules', 'build', 'dist', 'coverage', '.git', 'managed_components', '__pycache__', 'ESP32-S3-RLCD-4.2-Demo']);
```

`archive` 不在集内；同文件 `FW_ACC_RE = /\.acceptance\.(c|cpp|cc)$/`（`:31`）按扩展名形状匹配、不限深度，故 `firmware/main/test/acceptance/archive/` 下的归档 `.acceptance.c` 仍被枚举为「在盘验收文件」。

**本批起草期现跑实测（按闸同款 `collectFiles` 逻辑复现）**：`firmware/main/test/acceptance/` 现枚举命中 **13 件，且 13 件全部位于 `archive/` 下**——该面**现役件为 0**，即该闸固件面的 oracle 输入集当前**只剩退役件**。逐件清单：`char_stroke` / `device_ws_switch` / `hardware_validation` / `home_p1` / `key_route_a1` / `schedule_p2` / `schedule_reminder_b` / `screen_unification` / `session_logic` / `story_p3` / `ui_art_integration` / `v3_restore` / `voice_roundtrip_b`（各 `.acceptance.c`）；补 `archive` 进 `SKIP_DIRS` 后同口径枚举 = **0 件**。

**【硬项 1a · 数字更正申报】** `ARQ-260916-acc-gate-archive-skew` 原文记该目录下「**9 件**归档 `.acceptance.c`」，本批现跑为 **13 件**。两者不符，本批以现跑实测为准，并把该偏差如实登记（成因未追：或为 9-15 固件归档批之后又有 4 件于后续包并入 archive，起草期未回溯 git 历史）。**该更正不影响改法**（T3 的修法是集合补一个成员、与件数无关），但件数差 4 说明该 ARQ 的取证时点已陈旧。

**对偶闸已修而本闸未修**：`unit-run-check.mjs` 的 `SKIP_DIRS` 已于 260916 补 `archive`（ARQ-260916-unit-gate-archive-recursion 闭账 commit `4fc0969`，`git rev-parse 4fc0969` 核实存在），两闸退役口径现不同步。

**当前不产红**（architect 对 ARQ-260916 落地的复核有保留 GREEN 保留项 R3/R4 已记录）：acceptance 闸固件面因 host_app runner 证据在场，只影响 oracle 输入集与统计；但将来 host_app runner 判据变更或撤除时会被退役件打红，复刻 unit 闸病灶（ARQ 原文）。**第二条缺口**：`grep -n archive docs/DEV-GOVERNANCE.md` **零命中**——「任意深度名为 archive 的目录 = 退役保留名」这一约定现仅存于 `unit-run-check.mjs:34-38` 与 `server/pytest.ini:2-5` 两处**代码注释**，包作者不可见；将来现役测试误落该名会静默不跑。根因：退役机制由 260916 一次性直修引入，只改了代码、未成文；acceptance 闸是对偶面漏改。

### 问题 4：`brief-inputs` 先例路 `isMerged()` 两判定源在本仓主合并形态下同时落空

证据（verbatim 亲核）：`.claude/scripts/brief-inputs.mjs:205-220` 的 `isMerged()` 只认两源——① `delivery/changes/dev/<id>/supervisor-verdict.json` 的 `verdict === 'APPROVE'`；② `delivery/taskpackages/<id>/pod-events.jsonl` 行内 `eventType` 匹配 `/merge-(completed|merged)/` 或含 `merge` 字样。

本仓主合并形态是**监督 verdict 判 ESCALATE + 人批事件**（`eventType = human-decision`、`payload.decision = "approve-escalated-merge"`）——既非 APPROVE、eventType 也不含 `merge`，两源同时落空。证据出处：`delivery/changes/arch/260908-gov-review-input-surface-frontload/landing-verdict.md` D1（核验官独立实测：现判只认出 4/19，而 `git log --merges` 实证 ≥8 包真经 merge-driver 合并）。**影响面**：仅第③路「同 subsystem 先例封顶 3」与其 `--self-check` 臂；方向 **fail-closed**（少读不误扩阅读面、不放松任何门），危害是「取最新先例」目的未达、V6 pilot 对照数据失真。根因：`isMerged` 的判定源是按早期 APPROVE 形态写的，ESCALATE + 人批形态落地后未回写。

### 问题 5：`overlap-probe.json` 的 `.gitignore` 条目对已跟踪文件不生效

证据（亲核）：`.gitignore:46` = `overlap-probe.json`（在场）；`git ls-files | grep -c overlap-probe.json` = **11**（11 件仍被跟踪）——`git ls-files --error-unmatch delivery/taskpackages/260915-m1-k3-voice-round-ui/overlap-probe.json` 退出 0，确认 tracked。`.gitignore` 只对**未跟踪**路径生效，已入库路径不受其约束。

**病灶形态**：`overlap-probe.json` 是合并期闸的**运行产物**（`.claude/references/pod-coordination.md:183`：升级闸 `parallelOverlapProbe` 只读 merge-base 交集 + merge-tree 实撞，`--probe-out` 落该文件；`merge-driver.mjs:1254` 每次运行读它），而 merge-driver 的内部闸**每跑必重写**它。tracked 态 ⇒ 每次 merge 前工作树带 unstaged 改动 ⇒ rebase 恒撞「local changes would be overwritten」。

**前序消解不完整**：commit `c1bd37b`（`git rev-parse c1bd37b` 核实存在，2026-09-17）做了两件事——加 `.gitignore` 第 46 行、改一件 probe 文件内容——**但未 `git rm --cached` 存量 11 件**，故该条目对它们失效，病灶原样保留。同 commit 亦未处理同族形态（任何未来的追踪产物文件同款）。

**残余③核查结论（本批起草期已完成，非待办）**：ARQ-260917 遗留的「升级闸对 merge=union 文件的探测口径是否需显式豁免」已核——**无需改动**。依据（verbatim 亲核）：`pre-merge-escalation-gate.mjs:199` = `if (!conflictedFiles.length) return 'union-safe'; // 相交但可自动并（不同 hunk git 自并）`。`merge=union` 文件由 git 自动并、不出现在 `conflictedFiles`，天然落该分支判 `union-safe`（② 级不升人）；同 ARQ 记录 k3 修复轮 r15 合并实测「tier3 评审噪音收口」与此一致。故该项从「待办」降为「已核（结论成文于 T5）」。

### 问题 6：`portfolio-reconcile --check-launch` 不读 `launchPreconditions`

证据（亲核）：`grep -n launchPreconditions .claude/scripts/portfolio-reconcile.mjs` **零命中**；launch 可达性检查函数为 `checkLaunchReachability()`（`:243`）。而 260912 拆包链批落的 convention 通道要求 workstream 的 `decision-coverage.json` 顶层携带 `launchPreconditions` 键（`需修订` spec 落点的 launch 前置清单）+ `pod-coordination.md` §2b convention 核验条款——**该键的核验现全靠人读**。影响面：launch 前置清单缺失或过期时 `--check-launch` 不报红，兜底只剩人读；凡 workstream 带 `需修订` 标注的场景均暴露。根因：260913 human 三选一裁定「机械齿不做、留根」，本批承接该留根。

### 问题 7：`current/` 指针是隐性全局状态，经它解析的脚本不回声、错了也静默

证据（verbatim 亲核）：`.claude/scripts/dispatch-stamp.mjs:60-68` 的 `resolveChangeId()` 从 `delivery/taskpackages/current/taskpackage.json` 读 `changeId` 并返回，**全程无任何输出**；调用点 `:165` = `const changeId = resolveChangeId();` 随后直接以该 changeId 写 `pod-events.jsonl`。**失效形态（loop-retro-2026-09-13，3 包 retro 实证）**：① home-p1 worktree bootstrap 把上一包的 `current/` 残留带入，九闸跑在错误包上下文全绿（docRefs 全是上一包 id），靠顺手瞄一眼才识破；② display-first-m1 dispatch-stamp/journal-append 经 `current/` 解析 changeId，指针一天没切到本包，簿记写进上一批的 pod-events，还得 `git checkout` 还原再重灌；③ schedule-reminder intake 时 `current/taskpackage.json` 投影指着上一个包，不刷就静默假绿。根因：`current/` 是设计上的**单指针**，但每个经它解析的调用点都把它当**可信输入**，且解析结果不可见——错与对在输出上无差别。

### 问题 8：`landing-fidelity` citation 前缀重解析候选域偏窄（两处残余）

证据（verbatim 亲核）：`.claude/scripts/landing-fidelity-check.mjs:90` =

```js
      for (const prefix of ['.claude/scripts/', '.claude/scripts/lib/', '.claude/hooks/', 'docs/']) {
```

候选域四前缀——`resolveByPrefix` 认 `.claude/scripts/`、`.claude/scripts/lib/`、`.claude/hooks/`（260907 批 T2 落地）与 `docs/` 根（260907 热修追认）。**两处残余**：① `ARQ-260914-lfc-citation-references-prefix`——不含 `.claude/references/`，PROPOSAL §7 判官记录引 `dev-loop-operating-system.md:130`（闭集 SSOT 行）报 file-not-exist，而该文件实存于 `.claude/references/` 且行内容逐字相符（260912 拆包链批结构门 2 条 advisory 在册）；② `ARQ-260907-lfc-citation-docs-prefix` 残余①——不含 docs **子目录**形态，`PROPOSAL.template.md:49` 类裸名（真身 `docs/templates/`）仍报 advisory。影响面：凡评审记录 / PROPOSAL 引 references 面或 docs 子目录面文件的批次复现 advisory 假警报，结构门输出信噪比持续走低。根因：候选域是手工清单，随仓结构增长而漂移。

### 问题 9：PROPOSAL §2 锚清单无冻结前干跑，形态错到 landing 核验期才炸

证据（连续三批同型，逐批出处）：① `260913-arch-review-spec-split` 的 PROPOSAL §2 误用**加粗**锚行 `**目标文件**：`（解析器只认裸 `目标文件：`），human 2026-09-13 追认格式归一；② `260912-gov-review-portfolio-decomposition-chain` 同型四处加粗锚行 + T1 锚行注记逐字回引已撤幻影路径 `.claude/skills/triage-change/SKILL.md`——修完加粗形态后该 token 会进 parser 锚并集、造成 §2↔manifest 假失配（本批 near-miss，靠 landing-verifier D4 处置意见「锚行裸写、注记不引路径 token」抓住）；③ `260914-gov-review-delivery-restructure` 更彻底——六 T 锚清单**整行缺失**（非形态错、是整行没有），四轮评审 + 判官 manifest 对账（以 census 为基准、恰好绕过 `parseSection2AnchorLists`）均未捕获，`landing-fidelity-check` fail-loud 首跑 6 条「缺锚清单」才炸。

**机制侧事实（亲核）**：解析函数 `parseSection2AnchorLists` 已从 `.claude/scripts/lib/governance-gate-shared.mjs:345` **具名导出**（同文件另导出 `compareAnchorSets:426`、`extractManifestBlock:124`）；而 `.claude/scripts/landing-fidelity-check.mjs` 现只接受 `process.argv[2]` = reviewDir（`:50`）、`process.argv[3]` = baseRef（`:52`）与 `--self-check`（`:226`），**无任何前置干跑入口**。根因：三个错误类（加粗形态 / 无锚行含整行缺失 / 注记夹带路径 token）都能被已导出的纯解析函数在**起草期**抓获，但该函数只在 landing 期被调用。

### 问题 10：tighten-only 修订纪律只活在页脚与散落先例里

证据（亲核）：`grep -n tighten docs/DEV-GOVERNANCE.md` **零命中**——「## 验收测试纪律」节现存 §1（server host/live 两态，`:268`）/ §2（probe ≠ exercise，`:279`）/ §3（冻结即冒烟，`:283`）/ §4（C 类冻结验收实编，`:287`）四小节，**无 tighten-only 条**。而该纪律已实际行使多次：260829-schedule-reminder 包两次行使——`7c6d7c5`（acceptance-author 修自己冻结件两处 fixture/采集缺陷，断言行数 179→180 净增一条自洽断言）、`7ab2d47`（tighten-only 补两齿，`git diff --numstat` = C 档 `101/0`、py 档 `200/0`，`git diff -U0` 删除行计数 **0**）。**授权链全程靠一次性 advisory**：planner 须 human 显式要求另开 ARCH-ADVISORY 档（pod-events `0e55cfab`）才敢动冻结件。

**既成事实面（亲核）**：仓内 7 份冻结件自带 `REVISION LOG (acceptance-author only; tighten-only discipline)` 页脚（`schedule_reminder_b.acceptance.c:1700` / `schedule_p2.acceptance.c:979` / `home_p1.acceptance.c:214` / `voice_roundtrip_b.acceptance.c:111` / `char_stroke.acceptance.c:190` / `device_ws_switch.acceptance.c:78` / `test_char_stroke.acceptance.py:95`）。**影响面**：① 每包遇 tighten 场景都要现付一次 advisory/escalation 成本（该包实测 = 一次 architect 派发 20,374 tokens + 一次 human 触点）；② 更坏的是**反向漂移**——无成文边界时「补齿」与「改验收」的区分只靠执行者自律，放宽断言可以伪装成 tighten。

### 问题 11：auto-mode 分类器误伤 dev-loop 合法工作流已成成本模式

证据（loop-retro-2026-09-13，3 包 retro 实证）：① char-stroke 变异探针（`mutation-check` 闸要求真实变异→齿红→还原）被分类器判成「造假绿」，**方向完全颠倒**，AskUserQuestion 拍板后才继续；② story-p3 mutation 取证被拦「不可逆破坏 builder 产物」——分类器看不到「下一步就 checkout 还原」的意图，拦得其实合理但成本同付；③ g-relay-fix builder 的 ECS 部署被 Credential Leakage / Data Exfiltration 两形态终身盯防（transcript 出现过内联凭据痕迹后），处置 = 主线程代执行。**影响面**：凡「看起来像攻击」的合法验证动作（变异探针 / ECS 操作 / mutation 取证）都会复发；char-stroke retro 已总结成模式——「派发/执行前先落 human-decision 事件背书」。根因：分类器判的是**动作形态**，而预防成本最低的补充信息（该动作的合法性与还原路径）现无处固定承载，每次靠现场解释。

### 问题 12：真机轮协调协议三件套未固化，每次真机轮重复交学费

证据（loop-retro-2026-09-13，4 包 retro 实证）：① key-route-a1 第一次 300s 捕获窗口**开早了**（checklist 还在读），human 操作落在窗口外、口述与日志互证不上，按「completion 不凭解释宣布」重捕一轮；② screen-unification 合并夜三次设备重启与串口捕获 **DTR/RTS 关端口复位伪影**秒级吻合（21:15 / 22:50 实测）——正确姿势 = 单一长捕获覆盖全窗口、中途绝不关端口；同夜提前 15 分钟分发的提醒时间表落空（fire 时刻设备离线），改实时口令「现在按 BOOT！」一次成功；③ hardware-validation / render-pages 同族（烧录-复烧-目视等待是主循环周期；render-pages 的 boot 死锁烧录后才现形，开机冒烟已成必经步）。

**承载面实测（亲核）**：`.claude/skills/kidpal-serial-monitor/SKILL.md` 全文 31 行，三节（Tools / Rules that cost time when forgotten / Reading a smoke round）。**DTR/RTS 只在 `:10` 出现过一次**，且是工具说明形态（「resets the board via DTR/RTS then reads」），**未记录伪影模式**；「单一长捕获」「human 就绪信号开窗」「实时口令钉动作点」三条**零承载**。根因：该 skill 成文于被动读日志年代，真机轮协调知识散在 4 包 retro 里未回收。

### 问题 13：路径迁移类变更的「路径文本表示形态族谱」未固化

证据（260914-gov-review-delivery-restructure 全环实测，同批四连击）：该环因路径批改付了**四轮评审 + landing 四轮修复**，八种形态逐一现撞——① 斜杠形（唯一预期形态）；② 分段单引号；③ 分段双引号（任意段值，含 `'changes', 'INDEX.md'` 类非三段值）；④ Python join `"changes" / "dev"`；⑤ C 双引号实参；⑥ 相对链接形（`../../changes/` 被 r4 卫兵字符类排除而 V1 判据 `[^a-z]` 包含 `/`——**实现与判据不一致**）；⑦ 正则转义两子形（`\.planning` 孤儿反斜杠致 changes-index SyntaxError + `changes\/` 静默漏网）；⑧ **成员访问粘连形**（`taskPackage.deliveryMilestoneStateChanged` 点号属性被点前缀规则误吃，18 处、四轮评审 + V1–V6 全机检 + `node --check` 均漏，landing-verifier 首轮 FAIL 才接住——保险丝开火实录）。

**承载面实测（亲核）**：`ls .claude/references/` 输出 24 件，**无 `path-form-family.md`**。根因：八形态的知识在那一批的 retro（§3-F2/F2b + §4-2）与 landing-verdict（首轮 FAIL 红项）里，未回收成 reference。

## 2. 改法（T1…T13）

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

为什么这样改：直击问题 1 的根因——**让实现与上一行注释声明的意图一致**（「宽松匹配且『verdict』同段」），把裸 `GREEN` 备选分支去掉。收窄后行为方向为**向严**（原来恒真的关恢复成真判）。同文件自检电池补两夹具：**反例**（正文含 `GREEN` 字样但无 `verdict/结论` 标签行 → 判不满足）须 FAIL、**正例**（`260908-gov-review-input-surface-frontload` 的 `verdict：GREEN` 行形态 → 判满足）须 PASS；自检入口 `--self-check` 现存。

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`（改）

### T2 · `changes-index` `deriveLanded` 增第四锚：`ok: true（GREEN/PASS）` 行

承载 `ARQ-260909-index-verdict-anchor-okform-gap`。

改 `.claude/scripts/changes-index.mjs`：在 `:177-178` 两条锚旁新增第三条锚正则（第四通道），识 `ok: true（GREEN）` / `ok:true(GREEN)` / `ok: true（PASS）` 等变体，接线进 `deriveLanded()`（`:179`）与既有的 ①②③ 通道并列。为什么这样改：直击问题 2 的根因——三通道是对既有书写习惯的事后归纳、漏了已被实际使用的形态；补锚后 §1 表中 6 条 `frozen | yes` 批次经 `--upsert` / 全量重建即翻 `landed`（**这是本 T 的可执行验收面**）。自检电池（`--self-check`，`:512`）补两夹具：`ok: true（GREEN）` → true、`ok: false（RED）` → false。**不改** verdict 模板的书写要求、不强制单一形态（多形态并存是 `deriveLanded` 容差设计的本意，本条只是补一个漏掉的形态）。

目标文件：`.claude/scripts/changes-index.mjs`（改）

### T3 · `acceptance-run-check` 枚举跳 `archive` + 退役保留名成文

承载 `ARQ-260916-acc-gate-archive-skew`。

① 改 `.claude/scripts/acceptance-run-check.mjs:34`：`SKIP_DIRS` 集合补 `'archive'`——镜像 `unit-run-check.mjs` 已于 260916 落地的同款修法（该闸 `SKIP_DIRS` 现含 archive，commit `4fc0969`），使两闸退役口径同步。自检电池补**实盘断言**（沿 unit 闸 `real-repo-no-archive-collected` 先例）：枚举命中 `archive` 路径即 FAIL，防 `SKIP_DIRS` 回退静默复发。

② 改 `docs/DEV-GOVERNANCE.md`「## 验收测试纪律」节**新增 §5「退役保留目录名」**（与 ① 同批，使代码与约定同源）：正文钉死「`archive` = 两面（firmware / server）退役保留目录名，现役测试禁落该名」+ 写明两处代码承载点（`.claude/scripts/unit-run-check.mjs` 与 `.claude/scripts/acceptance-run-check.mjs` 的 `SKIP_DIRS`、`server/pytest.ini` 的 `norecursedirs`）+ 写明「归档件按定义非现役面」的对偶语义（承 260916-unit-gate-archive-recursion）。为什么这样改：直击问题 3 的第二条缺口——约定现只活在代码注释里，包作者不可见。

目标文件：`.claude/scripts/acceptance-run-check.mjs`（改）、`docs/DEV-GOVERNANCE.md`（改）

### T4 · `brief-inputs` `isMerged()` 增第三判定源

承载 `ARQ-260909-precedent-merged-detection-gap`。

改 `.claude/scripts/brief-inputs.mjs:205-220` 的 `isMerged()`：在现有两源之外增第三源——读该包 `pod-events.jsonl`，命中 `"eventType"\s*:\s*"human-decision"` 且同行 `payload.decision` 含 `approve-escalated-merge` 即判 merged。**不采用** `git log --merges` 作为判定源（该 ARQ 候选里并列的第二形态）：它需要 fork git、而 `isMerged` 现为纯 fs 读，引入子进程会把「先例路」的失败面从「文件缺失」扩到「git 不可用」，与既有降级纪律冲突；单一文件源已覆盖 ESCALATE + 人批这一本仓**主**形态。`--self-check` 补**双夹具**（沿既有「current/ 排除」臂的同类正控形态）：ESCALATE + 人批形态的 decoy merged 包**须入选**、未 merged 包**须不入选**。为什么这样改：直击问题 4 的根因——判定源按早期 APPROVE 形态写死，主形态变了未回写。方向 fail-closed 不变（第三源只**增加**可识别形态，不减少）。

目标文件：`.claude/scripts/brief-inputs.mjs`（改）

### T5 · `overlap-probe.json` 撤出跟踪 + 升级闸 union 探测口径核查结论成文

承载 `ARQ-260917-step5b-main-append-remerge-tier3`（残余③ + 同族 probe 输出文件）。

① **撤跟踪 11 件存量**：对 §landing-manifest 列出的 11 个 `overlap-probe.json` 路径执行 `git rm --cached`（**只出索引、工作树文件保留**），使 `.gitignore:46` 的既有条目真正生效。为什么：直击问题 5 的根因——`.gitignore` 对已跟踪路径不生效，`c1bd37b` 只加了条目未撤跟踪，病灶原样保留。撤跟踪后 merge-driver 每跑重写的该文件回到 untracked 态，rebase 撞 unstaged 的形态消除。

② **核查结论成文**：在 `.claude/scripts/pre-merge-escalation-gate.mjs` 的 `:199`（`if (!conflictedFiles.length) return 'union-safe';`）处补一行注释，记录本批起草期已完成的核查结论——`merge=union` 文件由 git 自动并、不出现在 `conflictedFiles`，天然落该分支判 `union-safe`，**无需显式豁免**（依据 = 该行分支逻辑 + ARQ-260917 记录的 k3 修复轮 r15 合并实测「tier3 评审噪音收口」）。为什么：该结论原为「待办」，核查后降为「已核」；写进代码承载点比留在队列里耐久。

**不改** `merge-driver.mjs` 的合并主流程、不改 `.gitattributes`（`delivery/taskpackages/**/pod-events.jsonl merge=union` 已由 `c16283b` 落地、实测收口，本批原样保留）。

目标文件：`delivery/taskpackages/260829-hardware-validation/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260901-ui-art-integration/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-display-first-m1/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-ecs-deploy-convergence/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-key-route-a1/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-render-pages/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260911-screen-unification/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260915-m1-k1a-key-layer-dispatch/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260915-m1-k2-keybar-three-slot/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260915-m1-k3-voice-round-ui/overlap-probe.json`（撤跟踪）、`delivery/taskpackages/260915-m1-n1b-p1-home-battery/overlap-probe.json`（撤跟踪）、`.claude/scripts/pre-merge-escalation-gate.mjs`（改：注释一行）

### T6 · `portfolio-reconcile --check-launch` 增读 `launchPreconditions`

承载 `ARQ-260914-portfolio-reconcile-launchpreconditions`。

改 `.claude/scripts/portfolio-reconcile.mjs` 的 `checkLaunchReachability()`（`:243`）：读 workstream 的 `decision-coverage.json` 顶层键 `launchPreconditions`——**键在**则逐条核验对应 spec 修订已落，缺即报红；**键不在**视为无前置（**向后兼容既有 workstream**，本仓现存 workstream 不带该键者行为零变更）。为什么这样改：直击问题 6——该键由 260912 批 convention 通道引入，但核验全靠人读；`--check-launch` 是本仓 launch 面的机械入口，把它接上成本最低。**边界**：本 T 只让既有检查器**多读一个键**，不改 `--check-launch` 的既有判据、不新增门。

目标文件：`.claude/scripts/portfolio-reconcile.mjs`（改）

### T7 · `dispatch-stamp` 回显所解析 changeId + 可选 `--expect-change-id` 硬校验

承载 `ARQ-260913-current-pointer-implicit-global`。

改 `.claude/scripts/dispatch-stamp.mjs`：① `resolveChangeId()`（`:60`）的返回值在调用点（`:165`）**始终回显一行到 stderr**（形态如 `dispatch-stamp: resolved changeId = <id> (from delivery/taskpackages/current/taskpackage.json)`）——使解析结果从不可见变为可见；② 新增**可选** flag `--expect-change-id <id>`：传了且与解析值不符即 **fail-loud 退出**（打印两值 + 指向 `current/` 路径），不传则行为与今天**完全一致**。为什么这样改：直击问题 7 的根因——错与对在输出上无差别；让每个经 `current/` 解析的调用点**看得见**解析值，并给调用方一个**零成本升级到硬校验**的开关。选 `--expect-change-id` 而非无条件 fail-loud：后者会打断跨包簿记的合法用法（如收口期为上一包补记），而前者把是否收紧留在调用方手里。**本批只改 `dispatch-stamp.mjs`**；`journal-append` / `verify.mjs` 等同族调用点只在本 T 的说明里点名、不在本批改（见 §6 留根）。

目标文件：`.claude/scripts/dispatch-stamp.mjs`（改）

### T8 · `landing-fidelity` citation 候选域补 `.claude/references/` 与 docs 子目录形态

承载 `ARQ-260914-lfc-citation-references-prefix` + `ARQ-260907-lfc-citation-docs-prefix`（残余①）。

改 `.claude/scripts/landing-fidelity-check.mjs:90` 的前缀候选域：在现有四前缀外补 `.claude/references/`（治残余①）**并**补 docs 子目录形态（治残余②——候选域现只有 `docs/` 根，需覆盖 `docs/templates/`、`docs/notes/` 等子目录下的裸名引用）。实现取**目录扫描派生**而非再加两条硬编码前缀：从 `docs/` 与 `.claude/` 下实际存在的一级子目录派生候选前缀集，使候选域随仓结构增长自动跟进，免去「每加一个 docs 子目录就手工补一行」的漂移形态（该漂移正是本问题的成因，见问题 8 根因）。自检（`runCitationAnchorsSelfCheck`，`:226` 已接线）补两夹具：裸名指向 `.claude/references/` 下实存文件 → resolved 非空、零 problems；裸名指向 `docs/templates/` 下实存文件 → 同上。**消警留痕形态保持**：仍经 `resolvedByPrefix` 独立观察列输出，不静默吞。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改）

### T9 · `landing-fidelity-check` 增 `--preflight <PROPOSAL>` 冻结前干跑模式

承载 `ARQ-260914-landing-preflight-anchor-parse`。

改 `.claude/scripts/landing-fidelity-check.mjs`：新增 `--preflight <proposal-path>` 入口——**只解析、不对账**。行为 = 对给定 PROPOSAL 调 `parseSection2AnchorLists`（`.claude/scripts/lib/governance-gate-shared.mjs:345` 已具名导出，本 T 只消费、不改 lib），把三类错**在起草期**报出来：① 加粗锚行 `**目标文件**：`（解析器只认裸形态）；② **无锚清单**（含整行缺失——问题 9 的第三种、也是最彻底的一种）；③ 锚行注记夹带路径 token（会污染 §2↔manifest 并集、造成假失配）。**不做** §2↔manifest 比对（那是 landing 期的事，前置干跑只保证「形态可解析」）。同批在 `docs/templates/PROPOSAL.template.md` 的起草 checklist 补一句「FROZEN 标注前跑一次 `--preflight`，并通过 §5 验证集命令实跑」（**同时补一句路径类变更先查 T13 的族谱表**——与该 ARQ 的并批建议一致）。

为什么这样改：直击问题 9 的根因——三类错都能被**已导出**的纯解析函数在起草期抓获，而它今天只在 landing 期被调用；连批三次的返工（含一次 human 格式追认）全部可免。**本 T 不新增工程门**：`--preflight` 是起草期只读自检、不参与任何门判定、不在任何闸链里被调用。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改）、`docs/templates/PROPOSAL.template.md`（改）

### T10 · `DEV-GOVERNANCE` 验收测试纪律新增 §5「tighten-only 修订窗口」

承载 `ARQ-260908-tighten-only-unwritten`。

改 `docs/DEV-GOVERNANCE.md`「## 验收测试纪律」节，在 §4 之后新增 **§5 tighten-only 修订窗口**，正文直接采纳 advisory `0e55cfab` 里那份**防滥用五件**（逐字为据，非重写）：

1. **纯 additive 机器可判**：`git diff -U0` 删除行数 = 0；
2. **新齿必须重跑当初暴露 vacuity 的原变异**并证明转红、`sha256` 还原核销（不许另造易过变异自证）；
3. **三件记录**：冻结件 `REVISION LOG` append 带 `Reason` + `mutationEvidence` 该条 `sensitive:false→true` + 红基线摘要补一段；
4. **范围帽**：同 Pod、同既有冻结件、不新建文件；
5. **意图锚定**：新齿须钉既有 `acceptanceRef` / `decisionCoverage` 原意图；若需新 acceptance id 或改行为定义即属 re-spec、**升 human**。

**同批补 `.claude/references/acceptance-evidence.md` 引用行**？——**本 T 不做**：§6 非目标已声明不改 agent def，而 `acceptance-evidence.md` 不在本批 landing-manifest 内；该 ARQ 原文的落地面为「`docs/DEV-GOVERNANCE.md` 新增 §5 + acceptance-author agent def 引用行」，本 T **只落 DEV-GOVERNANCE 一侧**，引用行一侧留 §6 留根（理由：动 agent def 触 self-def 判断点，不值得为一条引用行把本批拖进该争议）。为什么这样改：直击问题 10 的两条影响面——① 消灭每包现付的 advisory 成本（该纪律从此有据可依）；② 堵住「放宽断言伪装成 tighten」的反向漂移（五件里 1/2 两条是机器可判的硬闸）。

目标文件：`docs/DEV-GOVERNANCE.md`（改）

### T11 · 派发契约补「高风险合法动作预防声明」

承载 `ARQ-260913-auto-mode-classifier-legit-workflow`。

改 `.claude/references/pod-coordination.md`：在派发契约面（与 §11「demo 对账行」同层，即「义务的生产者可达性由派发侧传递」的那一族条款）新增一条**常设条款**——凡派发/执行清单内的高风险合法动作（**变异探针 / ECS 操作 / 部署动作 / mutation 取证**，即 auto-mode 分类器已实证误伤的四类），dispatch prompt **固定携带两句**：①「本动作的合法性与还原路径」声明（做什么、为什么合法、如何还原，如变异探针的「真实变异→齿红→`checkout` 还原」三步）；②「被拦就停、主线程代执行」兜底句（承 g-relay-fix 第二轮已实证的省一轮硬碰形态）。**不做** ARQ 候选②（给分类器加白名单）：那动 harness 面、须独立治理批，本登记不预批（见 §6）。为什么这样改：直击问题 11 的根因——分类器判**动作形态**，而预防成本最低的补充信息无处固定承载，每次靠现场解释；把它钉成派发契约的常设一句，成本一次付、收益每包复得。选 `pod-coordination.md` 而非 agent def：派发契约的既有承载点就在这里（§11 同族先例），且避开 agent def self-def 判断点。

目标文件：`.claude/references/pod-coordination.md`（改）

### T12 · `kidpal-serial-monitor` skill 固化真机轮协调协议

承载 `ARQ-260913-device-round-protocol`。

改 `.claude/skills/kidpal-serial-monitor/SKILL.md`（现 31 行）：① 新增一节「Device round protocol / 真机轮协调协议」，三条——**单一长捕获**（一次开窗覆盖全窗口、中途绝不关端口；附 DTR/RTS 关端口复位伪影的**模式识别**：设备重启时刻与捕获启停时刻秒级吻合即为伪影，非真重启）；**human 就绪信号开窗**（捕获窗口不由 agent 单方面开播，由 human 就绪信号开启——key-route-a1 窗口开早、操作落在窗外重捕一轮的实证）；**实时口令钉动作点**（动作点不提前分发时间表，改实时口令——screen-unification 提前 15 分钟分发、fire 时刻设备离线落空的实证）；② 在既有「Rules that cost time when forgotten」节补一行指向新节。同批在 `.claude/references/pod-coordination.md` 的真机轮相关处补**一行引用指针**（指向该 skill 新节，不转抄正文——防双源漂移，沿本仓「引用不复制」惯例）。为什么这样改：直击问题 12——三条零承载的知识散在 4 包 retro 里，skill 是它们的耐久家；同吃 260915 批「环境事实归 skills」的口径。

目标文件：`.claude/skills/kidpal-serial-monitor/SKILL.md`（改）、`.claude/references/pod-coordination.md`（改：引用指针一行）

### T13 · 新增 `.claude/references/path-form-family.md`（路径文本表示形态族谱）

承载 `ARQ-260915-path-form-family-reference`。

新增 `.claude/references/path-form-family.md`：把 260914-gov-review-delivery-restructure 全环实测的**八类形态**各自配对「残留清扫子检模式」——

1. 斜杠形（预期正形）；2. 分段单引号 `'changes', 'INDEX.md'`；3. 分段双引号（**任意段值**，含非三段值）；4. Python join `"changes" / "dev"`；5. C 双引号实参；6. 相对链接形 `../../changes/`；7. 正则转义两子形 `\.planning` / `changes\/`；8. **点号成员访问粘连形** `taskPackage.deliveryMilestoneStateChanged`

加三类批量替换风险注记：**幂等双应用**（压缩形两轮叠加，实测 18+10+1+24 处损伤）、**点前缀规则误吃成员访问**（第 8 类的成因）、**转义形不可见**（第 7 类，正则里看不见、`node --check` 抓不到）。同时写明**第 6 类的实现-判据不一致教训**（卫兵字符类排除 `/` 而 V1 判据 `[^a-z]` 包含 `/`）。`docs/templates/PROPOSAL.template.md` 的起草 checklist 一句由 **T9 一并落地**（「路径类变更先查族谱表」，与该 ARQ 的并批建议一致），本 T 不重复改该文件。为什么这样改：直击问题 13——八形态的知识在 retro 与 verdict 里，回收成 reference 后下次路径迁移批不必再付四轮评审。

目标文件：`.claude/references/path-form-family.md`（新增）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 前置升级闸 | `.claude/scripts/pre-merge-escalation-gate.mjs` | G3（**判定语义改动，如实声明**） | T1 收窄 GREEN 锚（恒真 → 真判，向严）；T5② 补一行注释。消费方 = merge-driver T1/Step 1b 透传、landing 前置升级闸 |
| arch INDEX 生成器 | `.claude/scripts/changes-index.mjs` | G3（生成器，非门） | T2 补第四锚。消费方 = `--upsert` / 全量重建 / `--self-check`；INDEX 非拦阻依据（文件头自陈） |
| 验收闸 | `.claude/scripts/acceptance-run-check.mjs` | G3（**枚举面改动，如实声明**） | T3① 跳 archive。消费方 = `verify.mjs` 的 acceptance 相位、`--self-check` |
| 输入面生成器 | `.claude/scripts/brief-inputs.mjs` | G3（advisory 生成器，非门） | T4 增第三判定源。消费方 = planner Step 7 双 brief、`--self-check` |
| launch 检查器 | `.claude/scripts/portfolio-reconcile.mjs` | G3（**检查器扩读，如实声明**） | T6 读 `launchPreconditions`。消费方 = kidpal-pod-launch-prep skill `--check-launch` 步 |
| 簿记写入器 | `.claude/scripts/dispatch-stamp.mjs` | G3 | T7 回声 + 可选硬校验。消费方 = planner Step 9 `[DISPATCH-STAMP]` 纪律；`journal-append` / `verify.mjs` 同族**本批不改** |
| 结构门 | `.claude/scripts/landing-fidelity-check.mjs` | G3（advisory 面） | T8 候选域扩形、T9 新增 `--preflight`。消费方 = landing-verifier、`--self-check` |
| 治理规则正文 | `docs/DEV-GOVERNANCE.md` | **G1** | T3② 新增 §5 退役保留名、T10 新增 §5 tighten-only。承 260907 批 T8 同形态先例（新节非改判据） |
| 派发契约 | `.claude/references/pod-coordination.md` | G2 | T11 新常设条款、T12 引用指针一行 |
| 路径族谱 | `.claude/references/path-form-family.md` | G2（新增） | T13；无既有消费方，新增后被 PROPOSAL.template checklist 引用（T9 落） |
| 串口 skill | `.claude/skills/kidpal-serial-monitor/SKILL.md` | G2 | T12；消费方 = 真机轮派发 |
| 提案模板 | `docs/templates/PROPOSAL.template.md` | G2 | T9 checklist 补两句 |
| 合并运行产物 | 11 × `delivery/taskpackages/*/overlap-probe.json` | **撤跟踪** | T5① 只 `git rm --cached`、工作树文件保留；无源码消费方（merge-driver 每次自写） |
| 契约面 | — | **未触** | 零 `docs/contracts/**` 改动 |
| 系统 spec 面 | — | **未触** | 零 `docs/requirements.md` / `docs/architecture.md` 改动 |
| agent frontmatter | — | **未触** | tools / name / model / description 全不动 |
| 工程门新增 | — | **零新增** | T9 的 `--preflight` 是起草期只读自检、不进任何闸链 |

**【硬项 1b · sweep 透镜】** 本批 T1 / T3① / T8 落地新判定 pattern / 新枚举口径 / 新候选域，起草期预跑原始输出：

- **T1**（现跑实测，2026-09-18）：① 对 `260908-gov-review-input-surface-frontload` 的 PROPOSAL 全文求值——现 regex = `true`、收窄后 = `true`（该 §7 确有 `verdict：GREEN` 行，收窄后**不应**误伤真阳性）；② **合成反例**（`本次评审覆盖 GREEN 配色区域，无判定行。`）——现 regex = `true`（裸分支命中）、收窄后 = `false`（**这正是病灶**：无判定行的正文也能满足该关）；③ **合成正例**（`> **7. 评审记录**` / `> R1 - 2026-09-07 - architect` / `> verdict：GREEN`）——现 = `true`、收窄后 = `true`。
- **T3①**（现跑实测，2026-09-18）：现口径枚举 `firmware/main/test/acceptance/` = **13 件命中、13 件全在 `archive/` 下**（逐件清单见 §1 问题 3）；补 `archive` 进 `SKIP_DIRS` 后同口径 = **0 件**。
- **T8**（现跑实测，2026-09-18）：现候选域对裸名 `dev-loop-operating-system.md`（真身在 `.claude/references/`）与 `PROPOSAL.template.md`（真身在 `docs/templates/`）**均未命中**（→ 报 file-not-exist）；补前缀后两者分别命中 `.claude/references/` 与 `docs/templates/`。

**【硬项 1c · 复扫模式完备性先验】** 本批涉**退役语面残留清扫**的项为 T3①（archive 目录名）。模式集显式枚举全形态：`archive`（英文词形，目录名精确匹配）× 有 / 无 backtick × `SKIP_DIRS` 集合成员 / `norecursedirs` 配置项两种消费形态；**完备性论证**：`SKIP_DIRS.has(entry)` 按**目录名精确相等**匹配（`acceptance-run-check.mjs:46` = `if (SKIP_DIRS.has(entry) || entry.startsWith('.')) continue;`，entry 为 `readdirSync` 的单段名），故无需考虑大小写变体或路径形态——`archive` 一词即完备。T5 的撤跟踪面为**显式枚举 11 条路径**（`git ls-files | grep -c overlap-probe.json` = 11，已逐条列入 manifest），非模式匹配，无完备性缺口。

## 4. human 取舍点（OQ）

### OQ1 · 本批的闸判定语义触点，走 Track B 判官判、还是升 full-adversarial + human

**背景（必须给全）**：本批 13 个 T 里有 5 个触到闸 / 检查器的**判定语义**——T1（前置升级闸判定 regex 收窄）、T3（验收闸枚举面）、T5（升级闸 union 探测口径核查）、T6（launch 检查器扩读）、T9（结构门新增前置自检入口）。其中三条的承载 ARQ **自己就写了这句**：「属 lightweight 机检硬排除——若 human 判该收紧本身该走重道，按 protocol 转 `full-adversarial + human`」。同时 `ARQ-260908-versioned-guard-version-literal-coupling` 把「新增/改闸判据」判为硬排除 → full，而 260907 批却把 DEV-GOVERNANCE 新节按 Track B 落了。两条先例方向不一致。

**选项**：
- **(a) 留 Track B**（判官按 `track-b-judge-brief` 全面审，判官或 human 任一方随时可启「声明红队」升级）。**推荐**。理由：(1) 本批不命中 AGENTS.md 的两条 Track A 判据（不触契约、非跨系统结构性变更）；(2) 五项改动方向**全部是收窄 / 向严 / 向后兼容**，无一是放松；(3) 每项改动的半径已由本批 §1 的现跑实测**照出**（含反例夹具），符合 Track B 三要件；(4) 判官在本批的硬约束是逐条亲核——闸语义改动正是它最该盯的面，快道不等于轻审。
- **(b) 升 full-adversarial + human**（走 `/arch-change` 的 full 档，challenger↔defender 对抗 + 人类拍板）。理由：闸语义的**闭合性**（如 T1 收窄后会不会漏掉某种合法的 verdict 书写形态）不是单轮判官能穷尽的，红队更稳。
- **(c) 拆分**：闸语义五项（T1/T3/T5/T6/T9）走 full、其余八项走 Track B。理由：把重道花在真正影响门判定的面上。

### OQ2 · T5 的撤跟踪范围

**(a) 11 件全撤**（推荐；`git rm --cached`，工作树文件保留、历史不变）。(b) 只撤近三批（260915 三件）。理由：早期批次已合并收口、其 probe 文件不再被重写，撤它们只为统一口径；但**不撤**会让「同族形态可复发」的教训又留一条尾巴。**(c) 不撤，改 merge-driver 落点**（把 probe 输出移出被合并路径）。理由：从形态上根治，但要动合并主流程、半径大于本批定位，与本批 §0 非目标冲突。

### OQ3 · T10 的 §5 落点与强度

**(a) 按五件原样落 `docs/DEV-GOVERNANCE.md`「验收测试纪律」§5**（推荐；承 260907 批 T8 同形态先例）。(b) 降强度：只落「纯 additive 机器可判 + 三件记录」两条，其余三条（原变异重放 / 范围帽 / 意图锚定）留参考面。理由：五件里只有前两条是机器可判的硬闸，后三条是判断，成文后仍需人读。**(c) 落 `.claude/references/acceptance-evidence.md`（G2）而非 DEV-GOVERNANCE（G1）**，理由：G1 的挂载面更宽。

**这项为什么该 human 裁**：§5 是**新增一条适用于全部未来包的验收纪律条款**（挂载面扩张），且它同时是「给冻结件开后门」的授权来源——授权边界该由 human 划。

### OQ4 · T11 与 T12 的落点

两 T 各有两处候选承载面，影响面不同：

- **T11**：(a) `.claude/references/pod-coordination.md`（推荐——派发契约既有承载点，避开 agent def self-def 判断点）vs (b) `.claude/agents/planner.md` 正文（离派发动作更近，但触 self-def 判断点）。
- **T12**：(a) `.claude/skills/kidpal-serial-monitor/SKILL.md` + `pod-coordination.md` 一行引用指针（推荐——同吃 260915 批「环境事实归 skills」口径）vs (b) 只落 skill、不加引用指针（更克制，但真机轮派发侧可能想不到去翻该 skill）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**【真空绿规约】** 下列自检 / 重放类条目，重放前先断言输入非空——V1/V3/V5/V7/V11 五条为 `--self-check` 电池，其**反例夹具须先自证其输入确实落在目标形态之外**（如 V1 的合成反例须断言正文确无 `verdict`/`结论` 标签行，否则「反例不红」可能只是夹具没造对）；自检验证输出须含**新增夹具名**（防「加了夹具但没接线」的真空绿）。

| 项 | 命令 | 预期 |
|---|---|---|
| V1 | `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` | 全绿，且**新增两夹具**（GREEN-裸字样反例 / `verdict：GREEN` 正例）在输出中具名出现 |
| V2 | 现跑：对 `260908-gov-review-input-surface-frontload` PROPOSAL 按新 regex 求值 | `green = true`（该批 §7 有 `verdict：GREEN` 行，收窄后应仍满足）；反例（删标签留正文 `GREEN`）→ `false` |
| V3 | `node .claude/scripts/changes-index.mjs --self-check` | 全绿 + 新增两夹具（`ok: true（GREEN）` → true、`ok: false（RED）` → false）具名出现 |
| V4 | `node .claude/scripts/changes-index.mjs`（全量重建）+ `cat delivery/changes/arch/INDEX.md` | §1 表中 6 条 `frozen \| yes` 批次翻 `landed`；`git diff` 仅 INDEX 两文件的该列变化 |
| V5 | `node .claude/scripts/acceptance-run-check.mjs --self-check` | 全绿 + 新增实盘断言（`archive` 路径零枚举）具名出现 |
| V6 | 按闸同款 `collectFiles` 逻辑枚举 `firmware/main/test/acceptance/` | 补 `archive` 前实测 **13** 条命中（全在 `archive/` 下）、补后 **0** 条（前后各留一次输出） |
| V7 | `node .claude/scripts/brief-inputs.mjs --self-check` + `--dry-run` | 全绿 + 新增双夹具（ESCALATE + 人批 decoy 须入选 / 未 merged 须不入选）具名出现；`--dry-run` 先例新鲜度可读 |
| V8 | `git ls-files \| grep -c overlap-probe.json` | 撤跟踪后 = **0**；`git status --porcelain delivery/taskpackages/*/overlap-probe.json` 显示为 untracked、工作树文件仍在盘 |
| V9 | `node .claude/scripts/portfolio-reconcile.mjs --self-check` + 对**不带** `launchPreconditions` 键的现存 workstream 跑 `--check-launch` | 自检全绿；不带键者行为零变更（不报红）——**向后兼容的正面证据** |
| V10 | 三路探针（**起草期已核实 `dispatch-stamp.mjs` 现无 `--self-check` 入口**，故本项不写自检、改实跑探针）：① `git grep -c "resolved changeId" .claude/scripts/dispatch-stamp.mjs`；② 在 `current/` 指向某包的临时状态下跑一次真调用；③ 带 `--expect-change-id <不同值>` 再跑一次 | ① ≥1（回显行在场）；② stderr 出现 `resolved changeId = <该包 id>`；③ **非零退出** + 打印解析值与期望值两值；**不带该 flag 时与今日行为一致**（exit 0） |
| V11 | `node .claude/scripts/landing-fidelity-check.mjs --self-check` | 全绿 + 新增 citation 两夹具（`.claude/references/` 与 `docs/templates/` 裸名）具名出现 |
| V12 | `node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>` | 报「§2 锚清单形态可解析、无加粗锚行、无缺锚清单、注记零路径 token」；对**故意注入加粗形态的副本**跑 → 须报错（反例夹具） |
| V13 | `node .claude/scripts/route-regression-check.mjs` + `node .claude/scripts/lint-descriptions.mjs` | 全绿；lint 只允许既有告警（`landing-verifier.md` 超长，本批未触该文件） |
| V14 | `node .claude/scripts/proposal-consolidation-check.mjs <本批>` | FROZEN 后复跑：`frozen:true`、`missingTargets:[]` |
| V15 | `landing-fidelity-check.mjs <本批>` 结构门 | §2 锚集 ≡ manifest 机判通过（本批 23 路径） |
| V16 | `grep -c "tighten" docs/DEV-GOVERNANCE.md` / `grep -c "archive" docs/DEV-GOVERNANCE.md` | 两条均 ≥1（原为 0，本批后成文在场） |

## 6. 留根清单

**不关的根（本批明确不承接）**：

1. **`ARQ-260911-*` 四条承载类设计项**（`cross-package-behavior-owner-gap` / `tooth-strength-below-contract` / `verdict-uncovered-surface` / `split-package-implementation-prose-gap`）——共享同一土壤（切包 / 验收承载面），需真设计判断（字段形状、申报段格式、分级判据），命中「便宜验证不成立」故不并入本批。**建议下一批单开**，且四条宜并批同判。
2. **`ARQ-260909-dispatch-stamp-telemetry-gaps`**——其子项 c（主线程自身消耗观测）含**真设计分叉**（harness 无回传，估算 vs 不采），该子项须 `full-adversarial + human`；子项 a/b 即便走快道也与 T7 同文件，但本批只做 T7 的回显与校验口，telemetry 字段面留根以免同文件两处改动互相拖累。
3. **`ARQ-260908-known-red-count-no-mechanical-source` 子项②③**——子项①已由 260909 T5 承接；②（evidence 齿 root 注入）落 `.claude/references/acceptance-evidence.md`，触侧与本批 §0 非目标（不动验收撰写约定）冲突；③（runner 报告头写执行目录）属 host 测试基建面。两项留根。
4. **`ARQ-260907-lfc-citation-docs-prefix` 的「候选域维护策略」子项**——本批 T8 取「目录扫描派生」已实质回应（不再手工维护清单），但若判官认为该派生的**边界**（扫到几级、哪些目录排除）需要单独成文，可另登。本批不预留该条款。
5. **`ARQ-260904-v23-asr-swap-quota-recheck`**——时间敏感项：ARQ 记的 token-plan 配额重置点是 2026-09-07 14:19 UTC，今日 2026-09-18 已过 11 天，**补验条件已具备**（另需阿里云 ASR 凭据开通）。属产品 / 部署面非 dev-loop 摩擦，不在本批范围内，**但建议 human 尽快另开**（拖久了会与引擎换型判断脱节）。

**交日落复盘项**：本批 §1 问题 2 的「6 条 frozen\|yes」是 `deriveLanded` 锚缺口的**跨批复现**（非孤例），T2 落地后建议在 `/loop-retro` 观察：INDEX 全量重建后是否仍有推不出 landed 的批次——若有，即该锚面仍有未枚举形态。

**登记不硬治项**：`journal-append.mjs` / `verify.mjs` 的 `current/` 解析点（本批只改 `dispatch-stamp.mjs`，见 T7 边界）——待观察 T7 的 `--expect-change-id` 实际被调用方采用率，再决定是否推广为常设硬校验。

**自律件（不进模板的环内纪律）**：起草期已按硬项 1a 逐条亲核（本 §1 全部断言带命令与日期）；本批 §1 的 §1 表（6 条 frozen 批次）与问题 5 的 11 件清点均为起草期现跑枚举、非转引。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）