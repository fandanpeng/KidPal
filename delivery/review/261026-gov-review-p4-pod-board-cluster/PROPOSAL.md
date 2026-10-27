# PROPOSAL · 261026-gov-review-p4-pod-board-cluster

> **状态：FROZEN v0.3 · 2026-10-27 · R3 verdict GREEN（零 findings；R1 五条 + R2 两条全部结案）。评审环 R1 BLOCK(F1,F3) → R2 BLOCK(F1) → R3 GREEN 收敛。human 立项 ack 2026-10-26 四拍、OQ1=(a) / OQ2=(b) 2026-10-27 两裁均在案。**
> **承载 ARQ**：`ARQ-260918-pod-session-settings-identity-unobservable`（SHOULD；**只取低分叉子集**——候选② 设置在场性自检落 pod-events + 候选①后半 非 CLI 形态降级面标注，T1）、`ARQ-260914-portfolio-reconcile-launchpreconditions`（SHOULD；存量 12 条裸 string 迁移，T2）、`ARQ-261025-shared-branch-pod-isolation`（SHOULD；**只取方案①** 文面 + base ref 钉死 + launch 检查项，T3）、`ARQ-261025-wait-state-closed-set`（COULD；调度态枚举与 board 投影 + **OQ2=(b) 同批打通 merge-driver 解锁判据**，T4）、`ARQ-261025-intake-index-retro-scope-hygiene`（COULD；漂移标消费动作 + retro 游标 + 源文件缺口报法，T5）。**不关的根**：260918 候选①前半（260921 批已落）、候选③ 门策略（观察期，见 §6）；shared-branch 方案②（merge-driver `<base>..<head>` 扩参）与 worktree 池化（human 另登记独立批）。
> **review 轨**：**预判 Track B 主路径（非轻路径），最终判档归 architect-reviewer**。依据：T2/T4/T5 落 `.claude/scripts/**`（`portfolio-reconcile.mjs` / `changes-index.mjs` / `loop-health-report.mjs` / `pod-events-append.mjs` / `merge-driver.mjs`），按 `gov-change.md`「轻路径判据」③ 机器面命中即恒 Track B；T1 另触 `.claude/agents/planner.md`（agent 定义）。Track A 段两条判据均不命中：非跨系统结构性变更；五条改法方向由 ARQ 预消化 + human 2026-10-26 立项 ack 四拍 + 2026-10-27 OQ 两裁收敛（OQ1=(a) 保守标注、OQ2=(b) 同批打通）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T5）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：**medium**（v0.2 重估升档，理由：OQ2=(b) 后本批触碰 `merge-driver.mjs` 的**合并链执行面**——`approvalRefUnlock` 解锁判据与 planner ESCALATE 停等取值；失败模式直接落在合并门（解锁误判 = 未批合并；解锁漏判 = 停等卡死），且 driver 是每个 dev Pod 合并必经入口。缓释事实：blocked 路径行为零变化（判据为「blocked ∨ 家族」，只增不减）、放行权仍只在五要素人批事件核验（`resolveHumanApproval` 单源），不放宽任何 RED；其余改动为调度态枚举 / 投影精度 / 申报留痕 / 文面）。
> **review profile**：G2+G3（`agents` / `commands` / `references` / `skills` + `scripts`）；**G1 零触碰**、**契约面零触碰**、**`hooks` / `settings` / `scope-map` / `verify-gates` 零触碰**、**无新增门**。

```landing-manifest
.claude/agents/planner.md
.claude/commands/loop-retro.md
.claude/references/dev-loop-operating-system.md
.claude/references/pod-coordination.md
.claude/scripts/changes-index.mjs
.claude/scripts/loop-health-report.mjs
.claude/scripts/merge-driver.mjs
.claude/scripts/pod-events-append.mjs
.claude/scripts/portfolio-reconcile.mjs
.claude/skills/kidpal-pod-launch-prep/SKILL.md
delivery/workstreams/m1-home-qa/decision-coverage.json
delivery/workstreams/m2-char-window/decision-coverage.json
```

---

## 0. 想实现的效果与差距

**目标效果**（五条 ARQ 并批清偿，按 P4 波立项 ack）：

1. **Pod 会话的设置在场性从「不可见」变「可审计」**：planner 起包时申报本次会话按哪套命令行 / settings 起步（含非 CLI 形态的降级面），结论落 pod-events；启动卡模板显式写明 VS Code 扩展会话形态的降级面与兜底动作。不新增 gate、不裁机械牙。
2. **launch 面有互斥租用禁令与每包 base ref 锚**：pod-coordination §2c 成文「同一 worktree / 同一分支同一时刻至多一个活跃 Pod」+「每包 base ref（= 包首提交父）钉死进 launch-card」+ launch 检查项；包级 diff（终验圈定 / detect_changes / 白盒复核 / 合并范围）恒用该 ref，不用 main。
3. **board 能区分「等什么」**：调度态闭集扩 `awaiting-human[:<reason>]` 受控族，board 原样投影、wip 按原值分桶，不占 merge token；pod-events 仍承载精确事由。**merge-gate 停等（supervisor ESCALATE）改用 `awaiting-human:merge-gate`**（OQ2=(b)），人批解锁判据同批扩族——放行权仍在五要素人批事件核验。
4. **存量 launchPreconditions 迁到对象形态**：`m1-home-qa` / `m2-char-window` 两份 decision-coverage.json 各 6 条裸 string 迁 `{text, scope, status, evidence}`；状态填法诚实——有独立锚的才填 verified，无核验痕迹一律初始 `unverified`、不追认。
5. **intake 漂移标有主、retro 游标不再假阳性**：漂移标指定消费动作与 owner（每轮 retro 逐条判「续期 / 清理 / 升级」并留痕）；`loop-health-report.mjs` 排除 `current/` 指针目录；`changes-index.mjs` 把「spine 源文件缺口」与「status 值域外」分开报。

**现状差距（全部为 2026-10-26～27 起草期实跑或亲核，证据见 §2 各 T 与 §5 基线表）**：

- `dev-loop.settings.json` 独有三件（Stop `supervisor-gate-check.mjs` / PostToolUse `portfolio-sync.mjs` / `permissions.ask` 的 git merge|rebase）只在显式加载时在场；`planner.md` 无「设置在场性自检」步，pod-events 闭集无对应事件型——静默降级零痕迹（260918 实录）。
- 启动卡模板已有命令行两形态（260921 批），但**非 CLI 形态的降级面未成文**（260918 候选①后半 deferred）。
- launch 面只有「空闲判读」（board 记录不是锁），无「同树 / 同分支互斥」口径，也无 base ref 的记录义务——同分支双活跃 Pod 时包级 diff 取错 base（261018 / 261022 实证，四条在 ARQ 条目）。
- `POD_STATUSES`（`portfolio-reconcile.mjs:36-45`）八值闭集无 `awaiting-human`；`--set-pod` fail-loud 拒收（`:141`），`readPods` 把未知值静默归一 `queued`（`:123`）——261016 / 261021 两包只能降级 blocked。
- `launchPreconditions` 两份存量文件仍为裸 string（实测 6+6 条、typeof 全 string）；读侧对象形态早已落地（260921 T6），缺的只是数据。
- `delivery/intake/INDEX.md` 漂移标现存 4 处（261022 未闭环 / 261021 值域外 / 261014 未闭环+滞留 / 261012 未闭环），`loop-retro.md:44` 只写「仍见漂移则升级机械闸另立批」、无逐条处置与 owner；`loop-health-report.mjs --pending` 的 `waitingOnLoopHealth` 仍含 `current`（8 条，实测）；`261021-p3-record-cancel-lifecycle` 的 spine 文件 **frontmatter 未闭合、正文缺失**（`parseSpine` 返回 null 实测），INDEX 只报「⚠值域外」——把文件损坏误标为状态值域外。

**非目标（本批不做，防 scope creep）**：

- **不新增 gate、不改任何门判定语义**：`--check-launch` 判定零改；不改 `verify-gates.json` / settings / hooks / scope-map / `pre-merge-escalation-gate.mjs`。
- **不做 260918 候选③门策略**（身份机械自证 / 升降级判据）——观察期后另裁；不做候选①前半（260921 已落，勿重做）。
- **不做 shared-branch 方案②**（merge-driver 显式 `<base>..<head>` 范围参数）——留给池化批按其形态再定；**不设计 worktree 池**（human 另登记独立 idea，本批零触碰；文面只留互指）。
- **不改前置升级闸**：`pre-merge-escalation-gate.mjs` 的 blocked 探针本批不动（ESCALATE 包的拦截/放行由「非 APPROVE」与 `escalateExempt` 承担，停等态改名不改变放行结果；等价性分析见 §2.0 复证 4 与 §6 留根 3）。若日后要求闸也区分等待态，另立批。
- **不修 261021 的 spine 文件本体**：本批只改「怎么报」，修补属其 owner 主线程的 intake 事务。
- **不追改历史制品**：已落地的旧 PROPOSAL / verdict / launch-card 不回改（append-only）。

## 2. 问题、根因与改法（T1…T5）

### 2.0 五条 ARQ 复证结论（起草期 2026-10-26～27；ARQ 行是待验证假设）

> 纪律：每条结论都对应实跑命令或亲读文件；ARQ 原文的路径 / 行号 / 机制描述按「搬运引据视同新断言复证」逐条核过，漂移处如实更正。

**复证 1 · `ARQ-260918` —— 低分叉子集的两项残余均成立；候选③最低成本形态已在场。**

- 亲核 `planner.md`：全文无「设置在场性」相关步骤——实测 `grep -n "settings" .claude/agents/planner.md` **仅命中 `:12` 一处**（角色说明里的 canonical 命令行，非本义）；`sourceType === 'proposal'` 的「proposal 来源自检」在 Step 5b（`:65`），该行不含 "settings" 字样（R1-F1 更正，原稿误记 `:59` 第二命中）。Step 1g（`:58`）之后即 Step 2（`:59`）。
- 亲核 `dev-loop.settings.json`：三件独有面在场——`Stop` 的 `supervisor-gate-check.mjs`、`PostToolUse`（Edit|Write|MultiEdit）的 `portfolio-sync.mjs`、`permissions.ask` 的 `Bash(git merge:*)` / `Bash(git rebase:*)`；默认 `settings.json` 无这三件（260918 原实录同形）。
- 亲核启动卡模板（`kidpal-pod-launch-prep/SKILL.md:122-134`）：命令行要素两形态在 `:131-133`（260921 批产物），**无非 CLI 形态降级面的任何句子**——候选①后半确实未落。
- 亲核 pod-events 闭集（`pod-events-append.mjs:29-40`）：8 值，无任何「设置在场」观测型；OS §5d（`dev-loop-operating-system.md:130`）同句复述该闭集（单源两处，改动须同批同步）。
- 候选③最低成本形态已在场：`planner.md:111-112` 已有「运行 `supervisor-gate-check.mjs` 确认 provenance」显式步骤——本批不再动它，只把它引作降级形态的兜底动作。

**复证 2 · `ARQ-260914` —— 读侧已落、存量未迁，且 ARQ 的「12 条」计数准确。**

- 实测：两份 `decision-coverage.json` 的 `launchPreconditions` 各 **6 条、typeof 全 string**（命令与输出见 §5 B7）。
- 亲核读侧：`summarizeLaunchPreconditions`（`portfolio-reconcile.mjs:249-263`）对象形态 + 旧形态兼容已在场；canonical 定义（对象四键 + 写入义务分派 + 存量迁移归建包方）在 `task-intake-and-slicing.md:298-311`，`:311` 明写「两在册 workstream 各 6 条裸 string、由建包方下次动该 workstream 时随批迁移」——本批 ack④ 即为该「下次」。
- 两条 base ref 现状亲核：`git cat-file -e base/m1-home-qa:…` / `base/m2-char-window:…` 均报 `invalid object name`（本地无该 ref）——**「commit-before-launch 已履行」不可机械复核**，迁移时不得填 verified（诚实填法见 T2）。

**复证 3 · `ARQ-261025-shared-branch-pod-isolation` —— 病灶与方案①落点成立；「base = 包首提交父」目前只活在 taskpackage 制品里、无 canonical 承载。**

- 亲核：`merge-driver.mjs` 在场；`pod-coordination.md` §2c（`:49-60`）只有换包协议四条，「空闲判读」明写「board 的 worktreePath 是**记录不是锁**」，无互斥口径；§2c 全文无「包首提交父 / 包级 diff base」字样。
- 「包首提交父」概念实测已散落在三份执行制品（`261022-p3-record-chrome-ghost-fix/scope-final.md`、`261022-p3-record-front-polish-fix/{pod-retro.md,scope-final.md}`）与 `delivery/retro/2026-10-25-loop-retro.md`——即实践已发生、canonical 缺口成立。
- 先例亲核：`261018-sd-log-usb-read/launch-card.md:6` 已有「本包 base ref = `dev/pod-m2`」的自由文本行——**形态是分支名**；本批把它固化为「commit SHA（= 包首提交父）+ 分支名」两件（理由见 §4 取舍 C）。
- 方案②不做：`merge-driver.mjs` 的 `<base>..<head>` 显式范围参数不落（该脚本本批仅因 T4 的解锁判据扩族被改，与 T3 无关）。

**复证 4 · `ARQ-261025-wait-state-closed-set` —— 闭集病灶成立，且读侧有一处比 ARQ 记录更隐蔽的隐患（本批一并处理）。**

- 源码亲核：`POD_STATUSES`（`portfolio-reconcile.mjs:36-45`）八值；`setPod` 在 `:141` fail-loud；`readPods` 在 `:123` 把未知 status **静默归一 `queued`**（ARQ 未记此点——若只放宽 setPod 而漏读侧，board 会把 `awaiting-human:*` 显示成 queued）。
- token 侧亲核：`computeToken` 的释放判据是 `TERMINAL_OR_RELEASED`（`:175`，= merged/blocked/abandoned）。若只加枚举不动它，Pod 在 `awaiting-human:*` 期间会**持续占 merge token 直到 TTL 回收**（实测复现见 §5 B4：基线把 `awaiting-human:merge-gate` 归一 queued 的同时 token 仍挂在 `a` 上）。
- merge-driver 耦合亲核：`approvalRefUnlock`（`merge-driver.mjs:1101`）判据是 `tp.status !== 'blocked'` ⇒ merge-gate 停等若改 `awaiting-human:merge-gate` 而不同批扩该判据，人批 `--human-approval-ref` 解锁链会 `not-applicable` 空转。human 2026-10-27 OQ2=(b) 裁「同批打通」，T4 第 8 项据此扩族（判据 = blocked ∨ 家族；放行权仍在五要素人批事件核验）。
- 实证在册：261016 / 261021 两包只能降级 blocked（`delivery/retro/2026-10-25-loop-retro.md:56` 与两包 pod-retro）。

**复证 5 · `ARQ-261025-intake-index-retro-scope-hygiene` —— 三项均成立；其中 ③ 的真实病灶比 ARQ 描述更具体（是 spine 文件损坏，不是状态写错值）。**

- ② 实测：`node .claude/scripts/loop-health-report.mjs --pending` 的 `waitingOnLoopHealth` 8 条、末条 = `current`（§5 B6）——`computeRetroScope`（`loop-health-report.mjs:192-208`）遍历 `listChangeDirs(delivery/taskpackages)`，`current/` 是活工作面指针目录（含 `taskpackage.json` / `input-brief.md` / `verdict.json`），非 changeId；`changes-index.mjs:174` 的 `collectDev` 早已同口径排除。
- ③ 实测：`261021-p3-record-cancel-lifecycle.md` 全文 41 行、**无闭合 `---`、无正文**（`parseSpine` 返回 null）；`collectIntake` 因此 status 落 `—`、drift 只报 `⚠值域外`（§5 B5 基线逐字）。ARQ 建议「报源文件缺口而不是只显示 ⚠值域外」成立，且缺口型应压掉值域外标（值域外语义 = status 写了域外值，与本病灶不同）。
- ① 亲核 `loop-retro.md:44`：现文只有「`delivery/intake/INDEX.md` 漂移标核查（T5 复议触发承载点——仍见漂移则升级机械闸另立批）」——无逐条处置动作、无 owner。
- INDEX 漂移标现存 4 处（`delivery/intake/INDEX.md:15/:16/:21/:23`）。

### T1 · 设置在场性自检落 pod-events + 非 CLI 形态降级面成文（260918 低分叉子集）

**问题**：Pod 会话实际加载哪套 settings 无机械可观测面——贴卡起步丢掉 `--settings .claude/dev-loop.settings.json` / `--agent planner` 时，Stop 合并门、board 自动 reconcile、git merge/rebase 的 ask 弹窗三件静默缺席，事后只能靠回读 pod-events / 手比 settings 文件反推（260918 实录：34 条 dispatch-completed、actor 全为子角色，无一条能证明主线程按 dev-loop 设置起步）。影响面 = 每包必现的静默降级；最重缺位面是 Stop 门。

**证据**：见 §2.0 复证 1（planner.md 无自检步、settings 三件、启动卡模板缺降级面、pod-events 闭集 8 值、OS §5d 复述闭集）。

**根因**：两个断点叠加——① 会话无法自证加载了哪套 settings，唯一可承载该事实的 pod-events 闭集里没有观测型事件；② 启动卡只列了两形态命令行，没写「不按命令行起步时损失什么、怎么兜」，降级面只活在人脑里。

**改法**（四件，申报档、零门）：

1. **`pod-events-append.mjs:29-40` 闭集加一值 `settings-presence`**。原文（逐字节，`grep -cF` = 1）：

```js
  'pod-started',
  'pod-closed',
]);
```

   替换为：

```js
  'pod-started',
  'pod-closed',
  // 261026 批 T1（ARQ-260918 低分叉子集·候选②）：设置在场性自检的申报事实——观测、non-bearing、不参与任何门判定。
  'settings-presence',
]);
```

2. **`dev-loop-operating-system.md:130` 同句同步**（该行是闭集的第二落点，不改即双源漂移）。原句里的枚举串 `… | pod-started | pod-closed` 末尾追加 ` | settings-presence`，并在同句后追加一句：

   `settings-presence` 是设置在场性自检的申报事实（payload：`declaredForm` ∈ `cli-interactive|cli-headless|extension-session|unknown`、`basis` ∈ `launch-card|session-observation|none`、`note` 一句）；申报档、non-bearing，不参与任何门判定（ARQ-260918 低分叉子集，261026 批）。

3. **`planner.md` 新增 Step 1h**（插入位置：紧接 Step 1g（`:58`）之后、`2. 对任何疑似契约变更先派…`（`:59`）之前；新节逐字节如下，外层用四反引号围栏仅为本 PROPOSAL 排版、落地时取内层文本）：

````markdown
1h. **设置在场性自检（申报档、非门；ARQ-260918 低分叉子集，261026 批 T1）**：起包时（launch-argument 与自建形状皆适用；Step 1① 只是 launch-argument 形状的探测入口），在 Step 5 首次落盘 TaskPackage 后、Step 7 派 acceptance-author 之前，追加一条 pod-events `settings-presence` 事件，把「本会话按哪套命令行 / settings 起步」从不可见变可审计：

```
node .claude/scripts/pod-events-append.mjs --change-id <changeId> --event-type settings-presence --actor planner --source planner --payload '{"declaredForm":"<cli-interactive|cli-headless|extension-session|unknown>","basis":"<launch-card|session-observation|none>","note":"<bounded 一句>"}'
```

判定依据优先级：① 本包 `launch-card.md` 的命令行要素在场且明确 → `declaredForm` 照录、`basis=launch-card`；② 自建形状 / 无启动卡 → 按会话可观察特征申报（有交互 TTY = `cli-interactive`；`-p` 无 TTY = `cli-headless`；VS Code 扩展会话 = `extension-session`；判不了 = `unknown` + `basis=none` + `note` 写原因）；③ **不得猜测**——申报 `unknown` 是合法结论。申报 `extension-session` / `unknown` 不阻断、不降档、不改任何流程分支：dev-loop.settings.json 三件（Stop `supervisor-gate-check.mjs` / PostToolUse `portfolio-sync.mjs` / `permissions.ask` 的 git merge|rebase）按缺席处理，收尾相位照 Step 12 手工跑 `supervisor-gate-check.mjs`、board 用 `portfolio-reconcile.mjs` 手跑 reconcile（两件本就是本文件显式步骤）。聚合观察面 = loop-retro（按 `declaredForm` 分桶统计降级率）；是否升机械门 = 观察期后再裁（human 2026-10-26 ack②：不新增 gate、不裁机械牙）。写不进去只告警，不阻断起包。
````

4. **`kidpal-pod-launch-prep/SKILL.md` 启动卡模板补降级面**（`:131-133` 命令行要素之后，块内追加；「六要素」改「七要素」的编号变动由 T3 一并做——本 T 只加降级面四行）：

```text
非 CLI 形态（VS Code 扩展会话 / IDE 内会话）降级面（261026 批 · ARQ-260918 候选①后半）：
  该形态不能传 --settings / --agent ⇒ 不加载 dev-loop.settings.json：Stop 的 supervisor-gate-check 门、PostToolUse 的 portfolio-sync 自动 reconcile、permissions.ask 的 git merge/rebase 三件按缺席处理，per-agent allow/deny 对主线程恒不生效（enforce-scope.mjs:9）。
  实测前置未验（两批未测「扩展会话能否传 --settings/--agent」）——在实测前，扩展形态起步须在 pod-events 记 settings-presence（declaredForm=extension-session），并在收尾相位手工跑 supervisor-gate-check.mjs、board 手跑 reconcile。
  若日后实测可传参：本行改写为「扩展形态按 ①② 同法传参」并撤降级申报（升级分支句，见 PROPOSAL OQ1）。
```

**为什么这样改**：对根因「无观测型事件 + 降级面未成文」——申报事件把不可见变可审计（且明确 non-bearing，不给它任何门力），启动卡把降级损失与兜底动作写成可照做的字面。选「新增事件型」而非复用 `friction` 的理由见 §4 取舍 B。

目标文件：`.claude/scripts/pod-events-append.mjs`（改） · `.claude/references/dev-loop-operating-system.md`（改） · `.claude/agents/planner.md`（改） · `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改）

### T2 · launchPreconditions 存量 12 条裸 string 迁对象形态（260914 迁移搭车）

**问题**：`m1-home-qa` / `m2-char-window` 两份 `decision-coverage.json` 的 `launchPreconditions` 各 6 条裸 string——读侧只能走「汇总计数」兼容分支，无法按包过滤、无法显示核过没；两个 workstream 虽已过 launch，但条目仍在册、每次 `--check-launch` 都落一行「尚有 6 条旧形态未迁移」噪音，且旧形态下「核过没」这个位根本不存在。

**证据**：见 §2.0 复证 2（6+6 条 typeof 全 string；读侧与 canonical 定义行号；两条专用 base ref 已不可达）。

**根因**：260921 T6 只落读侧兼容、把数据迁移留给「建包方下次动该 workstream 时随批」，此后两 workstream 未再被打开——防止「强填 status 即自撰凭据」的顾虑（当时原文）与「迁移永远排不上」的现实叠加，形成挂账。

**改法**：两个文件各把 `launchPreconditions` 数组由 `string[]` 逐条替换为 `{text, scope, status, evidence}`，**顺序不变、`text` = 原字符串逐字原样搬运**（不做措辞修订、不截断），`updatedAt` 改为 `2026-10-27`；两份文件现为 CRLF，改写须保持 CRLF（否则整文件行尾翻转污染 diff）。scope / status / evidence 按下表填（`scope` 机械取自条目文本点名的承载包；裸条目不点名包 → `workstream`）：

**m1-home-qa（6 条，文件内顺序 1–6）**

| # | 条目要点（text 按序原样搬） | scope | status | evidence |
|---|---|---|---|---|
| 1 | F-03 commit-before-launch：四件套 commit 到专用 base ref | `workstream` | `unverified` | `""` |
| 2 | 门 1（拆包 review 门）先行 | `workstream` | `verified` | `delivery/workstreams/m1-home-qa/review.md:7（GREEN 裁决，2026-09-15）` |
| 3 | 页点数量 deviation 需 human 过目（点名 keybar-three-slot） | `package:260915-m1-k2-keybar-three-slot` | `verified` | `delivery/workstreams/m1-home-qa/progress.md:11（human 2026-09-16 AskUserQuestion 认可）` |
| 4 | reminder-overlay-unify 额外前置：k1a 与 k3 均已合并（点名承载包） | `package:260915-m1-k4-reminder-overlay-unify` | `verified` | `delivery/taskpackages/INDEX.md:77/:79（两包 closed）+ 两包目录末提交 d5194c94 已在 main（git branch --contains 实测）` |
| 5 | 跨包冻结面扫描已做（launch 前预警义务） | `workstream` | `unverified` | `""` |
| 6 | Pod worktree 图索引（launch 面必办项） | `workstream` | `unverified` | `""` |

**m2-char-window（6 条，文件内顺序 1–6）**

| # | 条目要点（text 按序原样搬） | scope | status | evidence |
|---|---|---|---|---|
| 1 | commit-before-launch：三包四件套 + workstream 制品，milestones 补执行 | `workstream` | `unverified` | `""` |
| 2 | 门 1 先行 | `workstream` | `verified` | `delivery/workstreams/m2-char-window/review.md:7（GREEN 裁决，2026-09-17）` |
| 3 | deviation 拍板：无新增 deviation（页点 4→3 为既定收回） | `workstream` | `verified` | `delivery/workstreams/m1-home-qa/progress.md:11（拍板在案；m2 无再拍项）` |
| 4 | 真机硬依赖：M2K3-A7/A8 与 K1/K2 deferred 真机半挂 K3 设备轮（点名承载包） | `package:260917-m2-k3-char-window-overlay` | `unverified` | `""` |
| 5 | 冻结面修订通道（K2/K3 钉点修订走 acceptance-author 留痕） | `workstream` | `unverified` | `""` |
| 6 | 图索引（复用树随树刷新 / 新建树必办） | `workstream` | `unverified` | `""` |

**诚信判据（硬）**：`verified` 只给「有独立于本条目的耐久锚」的 5 条（m1 #2/#3/#4、m2 #2/#3）；其余 7 条一律 `unverified` + `evidence: ""`——尤其 m1 #1 / m2 #1（base ref 已不可达，**不可**按「当时应该做过」追认）与 m2 #4（「需 human 持板」是持续条件，非已完成事实）。迁移不改任何条目的语义、不删不并。

**为什么这样改**：对根因「迁移永远排不上 + 旧形态没有核验位」——一次把数据补成 canonical 形态，`--check-launch` 从此按包过滤、只报真正未核项；「无痕迹不追认」把当时 T6 的顾虑（强填 = 自撰凭据）在同一批里正面兑现。

目标文件：`delivery/workstreams/m1-home-qa/decision-coverage.json`（改） · `delivery/workstreams/m2-char-window/decision-coverage.json`（改）

### T3 · launch 面互斥租用禁令 + 每包 base ref 钉死（shared-branch 方案①）

**问题**：多 Pod 复用同一 worktree / 同一常驻分支时，launch 面没有任何互斥口径，也没有「本包 base 在哪」的记录义务——包级 diff 取 base=main 会把并联包的改动圈进本包（终验圈定 / 白盒复核 / 合并范围三处同污染），planner 只能临时 cherry-pick 或手工对齐（261018 两包共用 `dev/pod-m2`、261022 切包首提交父重跑才对、261023 并行 `/arch-change` 会话同树留改动，三条实证见 ARQ 条目）。

**证据**：见 §2.0 复证 3（§2c 无互斥口径；「包首提交父」只活在三份执行制品与 retro；261018 launch-card 已有自由文本 base ref 行；merge-driver 整分支语义）。

**根因**：260915 树池化把「一树一常驻分支」定为常态后，**树/分支是共享资源、包是时序租客**——但租用规则只写了「空闲判读 = 树上有会话即占用」（操作层事实），没写下「同一分支同一时刻至多一个活跃 Pod」的禁止性条款，也没规定包与 base 的锚定关系；于是 base 由各包自选，选错无痕。

**改法**（三处，文面 + 检查项，零门）：

1. **`pod-coordination.md` §2c 增两段**（插入位置：§2c 换包协议四条（`:53-58`）之后、`与既有机制的相容：`（`:60`）之前）：

```markdown
**互斥租用禁令与每包 base ref 钉死（261026 批 · ARQ-261025-shared-branch-pod-isolation 方案①）**：

- **同一 worktree / 同一常驻分支同一时刻至多一个活跃 Pod**。board 的 worktreePath 是**记录不是锁**（本节第一条，human 2026-09-15 裁）——本禁令是 **launch 面口径 + launch 检查项**，不是门：launch 执行者点火前扫 `$(git rev-parse --git-common-dir)/devloop/pods/*.json`，目标树路径或目标常驻分支上已有活跃条目（`queued` / `running` / `verifier-green` / `ready-to-merge` / `merging` / `awaiting-human*`）→ 停，改换空闲树或等该包收口。理由 = 同分支双活跃 Pod 下包级 diff 取错 base，会把并联包的改动背进本包（261018 两包共用 dev/pod-m2、261022 detect_changes base 取错、261023 同树并行会话留改动，三实证在 ARQ 条目）。
- **每包 base ref（= 包首提交父）钉死进 launch-card**：点火时在目标树跑 `git rev-parse HEAD`，把该 SHA 与常驻分支名写进启动卡（要素见 `kidpal-pod-launch-prep` skill 步骤 7）。**包级 diff 的 base 恒 = 该 ref，不用 main**——终验 scope 圈定（`detect_changes`）、白盒复核的实际爆炸半径、合并范围三处同污染面共用这一个锚（261022 实证：base=main 圈进并联包全部符号，切包首提交父 `80bc2027` 重跑才对）。互斥租用保证「包首提交的父 = 点火时 HEAD」这一等式成立。
- **池化另批**：worktree 池（空闲工位注册 / 租用 / 释放）由 human 另登记为独立专项批，本批不设计池；方案②（merge-driver 显式 `<base>..<head>` 范围参数）留给池化批按其形态再定。
```

2. **`kidpal-pod-launch-prep/SKILL.md` 步骤 3 增 launch 检查项**（插入位置：步骤 3 第一个 bullet「空闲判读…」（`:66`）之后，同为四条的列表项）：

```markdown
- **互斥租用检查（261026 批 · 语义 SSOT = pod-coordination §2c）**：点火前扫 `devloop/pods/*.json`——目标树路径或目标常驻分支上已有活跃 Pod（`queued` / `running` / `verifier-green` / `ready-to-merge` / `merging` / `awaiting-human*`）→ 停，换空闲树或等该包收口（同分支双活跃 Pod = 包级 diff 背别人改动的根因）。
```

3. **`kidpal-pod-launch-prep/SKILL.md` 步骤 7 启动卡模板**（同文件两处：① `:122` 的 `——**数行启动卡，六要素**：` 改为 `——**数行启动卡，七要素**：`；② 模板内 `基线锚：…` 行之后插入一行）：

```text
本包 base ref：<点火时目标树 git rev-parse HEAD 的 SHA>（= 包首提交父；常驻分支 <分支名>）——包级 diff（终验 scope 圈定 / detect_changes / 白盒复核 / 合并范围）恒用此 ref，不用 main。
```

**为什么这样改**：对根因「共享资源缺租用规则、包与 base 无锚定」——禁令写成 launch 检查项（不设机械锁，与「记录不是锁」的既有裁定相容），base ref 用「点火时 HEAD 的 SHA」而非分支名（分支会前移，SHA 不会；261018 先例的分支形态在 `<base>..<head>` 圈定时仍需再解析，退路见 §4 取舍 C）。

目标文件：`.claude/references/pod-coordination.md`（改） · `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改）

### T4 · 调度态闭集扩 `awaiting-human[:<reason>]` 受控族（wait-state）

**问题**：`POD_STATUSES` 八值闭集没有「等人操作」一等语义——261016 真机轮与 261021 合并门/真机轮停等都只能降级 `blocked`，board 无法区分「等真机 / 等合并批准 / 等裁决」，要精确语义得翻 pod-events；`--set-pod --status awaiting-human:real-device`（launch-card 与 exit-report 里已经这么写）直接 fail-loud 拒收。**v0.2（OQ2=(b) human 2026-10-27 裁：同批打通）**：merge-gate 停等一并改用新族——即「等合并批准」这一档也进 board 可见面，代价是 `merge-driver.mjs` 的人批解锁判据同步扩族。

**证据**：见 §2.0 复证 4（`:36-45` 闭集、`:141` 拒收、`:123` 静默归一 queued、`:175` token 释放集合、`merge-driver.mjs:1101` 解锁判据；261016/261021 实证）。另亲核 merge-driver 全链：解锁判据 `tp.status !== 'blocked'`（`:1101`）是唯一 status 键；`resolveHumanApproval`（`lib/governance-gate-shared.mjs:644-673`）的放行核验不读 status；前置升级闸的 blocked 探针（`pre-merge-escalation-gate.mjs:501-504`）只认 `blocked`——ESCALATE 包在无人批时仍由「非 APPROVE」（`:448`）拦、有人批时由 `escalateExempt`（`:382-386`）放行，故停等态改名不改变闸的放行判据（见 §6 留根 3）。

**根因**：调度态闭集当初只按「Pod 生命周期阶段」设计，没给「等人」留位；下游三处消费（读侧归一、token 释放、driver 人批解锁）都按旧闭集写死，故单纯加值会在读侧被吞、在 token 侧泄漏、在解锁侧空转（`not-applicable`）。

**改法**（`portfolio-reconcile.mjs` 五处 + `pod-coordination.md` 两处 + `merge-driver.mjs` 五处 + `planner.md` 一处；**OQ2=(b) 已裁，driver 不再零触碰**）：

1. **`portfolio-reconcile.mjs` 新增受控族判据**（插入位置：`const TERMINAL_OR_RELEASED = …`（`:46`）之后）：

```js
// 261026 批 T4（ARQ-261025-wait-state-closed-set）：等人操作停等的受控枚举族 awaiting-human[:<reason>]。
// 只扩调度态投影面；放行权不在 status 档位，在各自门的人批凭据核验（见 pod-coordination §3）。
const AWAITING_HUMAN_RE = /^awaiting-human(:[a-z0-9][a-z0-9-]*)?$/;
export function isAwaitingHumanStatus(status) {
  return typeof status === 'string' && AWAITING_HUMAN_RE.test(status);
}
export function isValidPodStatus(status) {
  return POD_STATUSES.has(status) || isAwaitingHumanStatus(status);
}
```

2. **读侧归一**（`:123`）：`      status: POD_STATUSES.has(data.status) ? data.status : 'queued',` → `      status: isValidPodStatus(data.status) ? data.status : 'queued',`
3. **写侧校验**（`:141`）：`  if (!POD_STATUSES.has(status)) throw new Error(…` → `  if (!isValidPodStatus(status)) throw new Error(…`（仅替换判据调用，错误消息文本不变）。
4. **token 释放**（`:175`）：`    if (!h || TERMINAL_OR_RELEASED.has(h.status)) {` → `    if (!h || TERMINAL_OR_RELEASED.has(h.status) || isAwaitingHumanStatus(h.status)) {`（等待态不占 token，与 §8「不占 token」对偶）。
5. **自检增 6 条**（插入位置：`selfCheck()` 内 `const pass = cases.filter(...)` 之前）：

```js
  // ⑦ 261026 批 T4：等人操作停等族（判别力：去掉 family 判据时后两条"非法"变红；放宽成任意冒号串时同样变红）
  t('pod-status: awaiting-human 裸值合法', isValidPodStatus('awaiting-human'), true);
  t('pod-status: awaiting-human:<reason> 合法', isValidPodStatus('awaiting-human:real-device'), true);
  t('pod-status: reason 大写/空格非法', isValidPodStatus('awaiting-human:Real Device'), false);
  t('pod-status: 空 reason 非法', isValidPodStatus('awaiting-human:'), false);
  t('pod-status: 既有八值仍合法', isValidPodStatus('ready-to-merge'), true);
  t('pod-status: 拼写错误仍非法', isValidPodStatus('blockedd'), false);
```

6. **`pod-coordination.md` §3 状态机**：状态图第二行补一支（原图两行不动，追加一行 `\-> awaiting-human[:<reason>]` 对齐 `blocked` 行），并在「它是持久标记…」句后追加：

```markdown
**等人操作停等 = `awaiting-human` / `awaiting-human:<reason>`（261026 批 · ARQ-261025-wait-state-closed-set，OQ2=(b) human 2026-10-27 裁）**：`POD_STATUSES` 扩一个受控族——`awaiting-human` 或 `awaiting-human:<reason>`（reason 取 `[a-z0-9-]+`），board 原样投影、wip 分桶按原值计；pod-events 的 `agent-escalation` 仍承载精确事由（权威面不变）。设值者 = planner / human：**merge-gate 停等（supervisor ESCALATE）= `awaiting-human:merge-gate`**（planner 合并门 ESCALATE 分支设值），真机轮 / 人等裁决等场景按 `<reason>` 自述；**真 BLOCK（supervisor BLOCK / 闸 BLOCK）仍用 `blocked`**——它不是等人，是退回修。放行权不在 status 档位：人批合并仍由 `approvalRefUnlock` 五要素人批事件核验把关（`merge-driver.mjs`，OQ2=(b) 同批扩族），无合法人批事件一律 `kept-blocked`。awaiting-human* 不占 merge token（reconcile 释放 holder，见 §8）。
```

7. **`pod-coordination.md` §8 token 表增一行**（插入位置：`| supervisor **ESCALATE** | …` 行之后）：

```markdown
| Pod 挂 `awaiting-human*`（等人操作停等） | `holder=null` 释放，不占 token；人批 / 人操作完成后按场景回 `running` 或 `ready-to-merge` |
```

8. **`merge-driver.mjs` 解锁判据扩族（五处）**：

   a. import 行（`:41` 前后）：`import { setPod, reconcile } from './portfolio-reconcile.mjs';` → `import { setPod, reconcile, isAwaitingHumanStatus } from './portfolio-reconcile.mjs'; // 261026 批 T4：解锁判据与 reconcile 共用同一受控族单源`
   b. `approvalRefUnlock` 上方注释（`:1097`）原句 `// 核验通过 → setPod(ready-to-merge) + reconcile + 刷新内存 board 视图（R1-F1）；失败 fail-closed 保持 blocked。` 之后追加三行注释（逐字节）：

```js
// 261026 批 T4（OQ2=(b) human 2026-10-27 裁：同批打通）：可解锁判据 = blocked ∪ awaiting-human[:<reason>] 族
// （merge-gate 停等改用新族后，人批 --human-approval-ref 仍走同一条解锁链）；放行权不在 status 档位、在五要素
// 人批事件核验（runner 内 resolveHumanApproval），族内任一 reason 无合法人批事件一律 kept-blocked。blocked 路径行为零变化。
```

   c. 解锁判据（`:1101`）：`  if (!tp || tp.status !== 'blocked') return 'not-applicable';` → `  if (!tp || !(tp.status === 'blocked' || isAwaitingHumanStatus(tp.status))) return 'not-applicable';`
   d. 解锁提示语（`:1121`）：`` warnFn(`${changeId}: approval-ref 解锁 blocked → ready-to-merge (ref=${humanRef})`); `` → `` warnFn(`${changeId}: approval-ref 解锁 ${tp.status} → ready-to-merge (ref=${humanRef})`); ``（如实显示原停等态，便于审计）
   e. 自检增 2 条（插在 T1c 断言行之后、T1 夹具块内；`t1` 仓与 `setPod` 均为该块既有变量/import）：

```js
      // 261026 批 T4（OQ2=(b)）：awaiting-human 族可解锁——放行权在五要素人批事件、不在 status 档位
      setPod(t1, { changeId: '260901-t1s', status: 'awaiting-human:merge-gate' });
      const rD = approvalRefUnlock(t1, { pods: [{ changeId: '260901-t1s', status: 'awaiting-human:merge-gate' }] }, {
        changeId: '260901-t1s', humanRef: 'self-check-event', runner: () => {}, reconcileFn: () => ({}),
        refreshBoard: () => ({ pods: [{ changeId: '260901-t1s', status: 'ready-to-merge' }] }), warnFn: () => {},
      });
      const fileD = JSON.parse(readFileSync(join(t1, '.git', 'devloop', 'pods', '260901-t1s.json'), 'utf8'));
      assert('T1d awaiting-human:merge-gate + 有效 ref → 解锁（merge-gate 停等改用新族后不卡死）', rD === 'unlocked' && fileD.status === 'ready-to-merge', `r=${rD} file=${fileD.status}`);
      setPod(t1, { changeId: '260901-t1t', status: 'awaiting-human:real-device' });
      const rE = approvalRefUnlock(t1, { pods: [{ changeId: '260901-t1t', status: 'awaiting-human:real-device' }] }, {
        changeId: '260901-t1t', humanRef: 'self-check-event',
        runner: () => { throw Object.assign(new Error('gate block'), { stderr: '五要素不符' }); },
        reconcileFn: () => ({}), warnFn: () => {},
      });
      const fileE = JSON.parse(readFileSync(join(t1, '.git', 'devloop', 'pods', '260901-t1t.json'), 'utf8'));
      assert('T1e 族内任一档无有效人批 → fail-closed 保持 waiting（语义档不承担放行权）', rE === 'kept-blocked' && fileE.status === 'awaiting-human:real-device', `r=${rE} file=${fileE.status}`);
```

9. **`planner.md` 多 Pod 合并门 ESCALATE 分支改设值**（`:117` 整行替换，`grep -cF` = 1；**不得全局替换**——`--status blocked` 在 `planner.md` 现行共 **3** 处：`:115`（多 Pod APPROVE 后升级闸拦分支）、`:116`（BLOCK 分支）、`:117`（ESCALATE 分支，本项唯一应改处）；`:115` / `:116` 均不动，`:115` 拦的是真闸 BLOCK、`:116` 是 supervisor BLOCK，与 T4 第 6 项「真 BLOCK 仍用 blocked」一致）。原文（逐字节）：

```markdown
      - ESCALATE → `node .claude/scripts/portfolio-reconcile.mjs --set-pod <changeId> --status blocked`；停 human。human 批准合并后：planner 落一条 pod-events `human-decision` 事件（`payload.decision="approve-escalated-merge"`，eventId 即人批凭据）→ 直跑 `node .claude/scripts/merge-driver.mjs --pod <changeId> --human-approval-ref <eventId>`（仍经 merge token 串行化；blocked 停等态由闸内联动豁免消解，OQ2=(a)）；人批不豁免 off-main/契约/provenance，闸照拦。
```

   替换为（逐字节；只有两处字面变化）：

```markdown
      - ESCALATE → `node .claude/scripts/portfolio-reconcile.mjs --set-pod <changeId> --status awaiting-human:merge-gate`；停 human。human 批准合并后：planner 落一条 pod-events `human-decision` 事件（`payload.decision="approve-escalated-merge"`，eventId 即人批凭据）→ 直跑 `node .claude/scripts/merge-driver.mjs --pod <changeId> --human-approval-ref <eventId>`（仍经 merge token 串行化；merge-gate 停等态由闸内联动豁免消解，OQ2=(a)）；人批不豁免 off-main/契约/provenance，闸照拦。
```

   BLOCK 分支（`:116`）与单 Pod 分支（`:113`，无 board 条目）不动。

**为什么这样改**：对根因「闭集无位 + 三处消费写死」——受控族（冒号后 reason 限小写字母/数字/连字符）既让 board 能区分等什么，又保持闭集可校验；读侧归一、写侧校验、token 释放三处同批改齐，board 投影不再吞值、token 不再被等待态占死。**OQ2=(b) 的打通设计**：解锁面只扩「可被检查的集合」（blocked ∨ 家族），**不新增任何放行条件**——五要素人批事件核验（`resolveHumanApproval` 单源）仍是唯一放行判据，族内任一无有效人批一律 `kept-blocked`（T1e 夹具钉死）；因此不存在「族内某个 reason 不应释放」的分档需求——reason 只管 board 可读性，不承担权限语义（该判据写进夹具与 §6 留根 3）。前置升级闸的 blocked 探针不改：ESCALATE 包的拦截由「非 APPROVE」reason 承担、人批由 `escalateExempt` 承担，停等态改名不改变闸的放行结果（§6 留根 3 记录该等价性分析）。

目标文件：`.claude/scripts/portfolio-reconcile.mjs`（改） · `.claude/references/pod-coordination.md`（改） · `.claude/scripts/merge-driver.mjs`（改） · `.claude/agents/planner.md`（改）

### T5 · intake 漂移标消费动作 + retro 游标排除 current + 源文件缺口报法（intake/index 卫生）

**问题**：三个投影卫生病灶——① `delivery/intake/INDEX.md` 的漂移标是「检测不拦截」设计，但没有 owner、没有动作，连续多轮 retro 只把它抄进小结、无人逐条处置（现存 4 处，滞留天数还在涨）；② `loop-health-report.mjs --pending` 的 `waitingOnLoopHealth` 把 `current/` 指针目录当成「旧格式、缺 loop-health」的包，每轮 waiting 计数 +1 假阳性；③ spine 文件损坏（frontmatter 未闭合 / 缺 status）时，`changes-index.mjs` 只报「⚠值域外」，把「文件坏了」误标成「状态写了域外值」。

**证据**：见 §2.0 复证 5（三项实测；`261021` spine 41 行无闭合分隔符；`current` 在 waiting 桶；`loop-retro.md:44` 无处置动作）。

**根因**：① 漂移标是渲染器的旁注，规则里只写了「复议触发」，没写「谁在何时做什么」——旁注没有闸力也就没有消费方；② `computeRetroScope` 的扫描根 = `delivery/taskpackages/*/`，而 `current/` 是同一目录下的活工作面投影（非 changeId），`collectDev` 早排除了它、`computeRetroScope` 没同步；③ 状态解析失败与状态值域外共用同一兜底字面串。

**改法**（三件，零门）：

1. **`loop-retro.md` 步骤 5 漂移标核查句替换**（`:44` 原句为行内片段；原文片段，`grep -cF` = 1）：
   原文：`` `delivery/intake/INDEX.md` 漂移标核查（T5 复议触发承载点——仍见漂移则升级机械闸另立批） ``
   替换为：

```markdown
`delivery/intake/INDEX.md` 漂移标逐条处置（261026 批：消费动作与 owner 成文）——owner = 主线程（本轮 retro 执行者）；每条漂移标必须在本轮 retro 报告固定节「intake 漂移处置」逐条给三裁之一：**续期**（写明理由与新期限）/ **清理**（封闭条件已达成或 human 授权）/ **升级**（转 architect-owned ARQ 或立机械闸批；`⚠源文件缺口` 型升级 = 主线程修补 spine 文件）。逐条留痕视为本项完成；「仍见漂移」本身不再是升级判据——升级判据 = 同一条目连续两轮被裁「续期」仍无动作。
```

2. **`loop-health-report.mjs` `computeRetroScope` 排除 `current`**（`:192` 之后，for 循环体首行插入）：

```js
    // 261026 批 T5②（ARQ-261025-intake-index-retro-scope-hygiene）：current/ 是活工作面指针目录、不是 changeId，
    // 不进 retro 游标任何桶（changes-index.mjs collectDev 同口径排除）。
    if (id === 'current') continue;
```

3. **`changes-index.mjs` 缺口与值域外分流**（四处）：

   a. `intakeDrift`（`:397-406`）整函数替换为（逐字节）：

```js
function intakeDrift({ statusBase, hitCount, filled, lastCommitDate, today, spineOk = true, hasStatus = true }) {
  const marks = [];
  // 261026 批 T5③（ARQ-261025-intake-index-retro-scope-hygiene）：源文件缺口与值域外分开报——
  // frontmatter 未闭合 / 缺 status 字段时，旧行为只落「⚠值域外」（原文语义是 status 写了域外值），
  // 掩盖真实病灶（spine 文件损坏）。缺口型先报缺口、不再叠加值域外；已解析且 status 在场的域外值仍走值域外。
  if (!spineOk) { marks.push('⚠源文件缺口（frontmatter 未闭合）'); return marks; }
  if (!hasStatus) { marks.push('⚠源文件缺口（缺 status 字段）'); return marks; }
  if (!INTAKE_STATUS_ENUM.test(statusBase) && statusBase !== 'closed') marks.push('⚠值域外');
  if (hitCount > 0 && filled >= hitCount && statusBase !== 'closed') marks.push('⚠未闭环');
  if (/^awaiting-/.test(statusBase) && lastCommitDate) {
    const d = Math.floor(daysBetweenIso(lastCommitDate, today));
    if (!Number.isNaN(d) && d > 7) marks.push(`⚠滞留${d}d`);
  }
  return marks;
}
```

   b. `collectIntake` 的 drift 调用行（`:425`）改为：

```js
    const drift = intakeDrift({ statusBase, hitCount: hits.length, filled: prog.filled, lastCommitDate: last.slice(0, 10), today, spineOk: fields !== null, hasStatus: statusRaw.trim().length > 0 });
```

   c. INDEX 头注（`:501`）的漂移标说明句追加 `；⚠源文件缺口 = spine frontmatter 未闭合 / 缺 status 字段（261026 批：与值域外分开报，缺口型不叠加值域外）`。
   d. 自检增 3 条（`:696` 之后，`const pass = …` 之前）：

```js
  // 261026 批 T5③：源文件缺口与值域外分流（判别力：不传 spineOk/hasStatus 时缺口 fixture 落 ⚠值域外 变红）
  t('intake: frontmatter 未闭合 → ⚠源文件缺口', JSON.stringify(intakeDrift({ statusBase: '—', hitCount: 0, filled: 0, lastCommitDate: '', today: '2026-10-16', spineOk: false, hasStatus: false })), JSON.stringify(['⚠源文件缺口（frontmatter 未闭合）']));
  t('intake: 已解析但缺 status → ⚠源文件缺口', JSON.stringify(intakeDrift({ statusBase: '—', hitCount: 2, filled: 0, lastCommitDate: '2026-10-01', today: '2026-10-16', spineOk: true, hasStatus: false })), JSON.stringify(['⚠源文件缺口（缺 status 字段）']));
  t('intake: 值域外 status → 不误标缺口', JSON.stringify(intakeDrift({ statusBase: 'landed', hitCount: 3, filled: 3, lastCommitDate: '2026-10-01', today: '2026-10-16', spineOk: true, hasStatus: true })), JSON.stringify(['⚠值域外', '⚠未闭环']));
```

   注：`intakeDrift` 的既有调用（含自检）不传新参数时走默认 `true/true`，行为与现状逐字一致（§5 A2 实测 41 条旧断言全绿）。

**为什么这样改**：对三个根因——① 把「检测」补上「消费」：owner + 三裁 + 留痕载体，并把升级判据从「仍见漂移」改为「连续两轮续期无动作」（原判据在漂移长期存在时恒真，等于没有判据）；② 与 `collectDev` 同口径排除指针目录，单源传导给三个消费方；③ 两个不同病灶用两个不同字面报，避免诊断误导。

目标文件：`.claude/commands/loop-retro.md`（改） · `.claude/scripts/loop-health-report.mjs`（改） · `.claude/scripts/changes-index.mjs`（改）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 / 消费方 |
|---|---|---|---|
| 强制执行层 G3 | `.claude/scripts/portfolio-reconcile.mjs` | 改（T4） | 调度态受控族 + 读侧归一 + token 释放 + 自检 6 条；消费方 = `--set-pod` / `--check-launch` / reconcile（`--check-launch` 判定零改）与 `portfolio-sync` 钩子 |
| 强制执行层 G3 | `.claude/scripts/merge-driver.mjs` | 改（T4，OQ2=(b)） | `approvalRefUnlock` 可解锁判据 = blocked ∪ awaiting-human 族（import 单一受控族判据）+ 解锁提示语如实带原状态 + 自检 2 条；消费方 = 每个 dev Pod 的 `--pod` 合并路径（人批路径）与 `--self-check-merge-friction` |
| 强制执行层 G3 | `.claude/scripts/loop-health-report.mjs` | 改（T5②） | `computeRetroScope` 排除 `current`；消费方 = closeout-check / retro-staleness-check / merge-driver（同一函数三消费方，单源传导） |
| 强制执行层 G3 | `.claude/scripts/changes-index.mjs` | 改（T5③） | 缺口/值域外分流 + 自检 3 条；消费方 = 三 lane INDEX 全量重建与 `--upsert` |
| 强制执行层 G3 | `.claude/scripts/pod-events-append.mjs` | 改（T1②） | eventType 闭集加 `settings-presence`；消费方 = 全部 pod-events 写入口（dispatch-stamp / journal-append / planner 升级桥）与聚合器（未知型本就不计数，新增型不改变既有聚合） |
| 流程定义 G2 | `.claude/agents/planner.md` | 改（T1②） | 新增 Step 1h 申报步；消费方 = 每个 planner Pod 会话 |
| 流程定义 G2 | `.claude/references/dev-loop-operating-system.md` | 改（T1②） | §5d 闭集句同步（闭集第二落点，双源同批改） |
| 流程定义 G2 | `.claude/references/pod-coordination.md` | 改（T3 / T4） | §2c 互斥租用与 base ref；§3 状态机与等待态；§8 token 表一行 |
| 流程定义 G2 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | 改（T1①后半 / T3） | 启动卡七要素 + 降级面 + 互斥检查项；消费方 = launch 执行者（human 主线程） |
| 流程定义 G2 | `.claude/commands/loop-retro.md` | 改（T5①） | 步骤 5 漂移标处置动作与 owner |
| 执行状态 / 拆包制品 | `delivery/workstreams/m1-home-qa/decision-coverage.json` · `delivery/workstreams/m2-char-window/decision-coverage.json` | 改（T2） | 12 条裸 string → 对象形态；消费方 = `portfolio-reconcile --check-launch` 的 launchPreconditions 读侧 |
| 指令根 G1 | `AGENTS.md` / `CLAUDE.md` / `docs/DEV-GOVERNANCE.md` | 未触 | 零 diff |
| 契约面 | `docs/contracts/**` | 未触 | 零 diff |
| 其余强制层 | `.claude/hooks/**` / `.claude/scope-map.json` / `*.settings.json` / `.claude/verify-gates.json` / `pre-merge-escalation-gate.mjs` | 未触 | 零 diff；无新增门、无门判定语义变化 |
| 规范 spec | `docs/requirements.md` / `docs/architecture.md` | 未触 | 零 diff |

**【硬项 · sweep 透镜义务】新字面 / 新事件型全仓预跑（起草期 2026-10-27，命令 + 结果）**：

- **新字面碰撞基线**（`grep -rlF`，语料 = 全仓 `.md` / `.mjs` / `.json`，排除本批 `delivery/review/261026-gov-review-p4-pod-board-cluster/`）：`settings-presence` = **0** 文件；`⚠源文件缺口` = **0** 文件；`isAwaitingHumanStatus` = **0**；`isValidPodStatus` = **0**；`本包 base ref` = **1**（`delivery/taskpackages/261018-sd-log-usb-read/launch-card.md:6`，先例，非撞车）；`互斥租用` = **1**（`delivery/workstreams/gov-debt-reduction/task_plan.md:63`，本批立项 ack 记录）；`设置在场性自检` = **2**（`docs/debt/gov/ARQ-260918…md` 与本批任务书先在，非撞车）；**v0.2 追加**：`awaiting-human:merge-gate` = **0**、`isAwaitingHumanStatus(tp.status)` = **0**、`T1d awaiting-human:merge-gate` = **0**；`awaiting-human` 家族字面已在 launch-card / exit-report / intake spine 多处在用（语义同向，本批是把 pod 调度态对齐到既有用词，不是新造词）。
- **`POD_STATUSES` 消费方 census（v0.2 更新）**：`.claude/scripts/*.mjs` / `.claude/hooks/*.mjs` 全仓 grep，除 `portfolio-reconcile.mjs` 自身（`:36/:123/:141/:175`）外无第二处消费闭集常量；`merge-driver.mjs` 的 status 键共两处——`:1101` 解锁判据（T4 改为 import 同一受控族单源 `isAwaitingHumanStatus`）与 `:1731` 的 `ready-to-merge` 过滤（零变动）⇒ 解锁面扩族、ready 过滤与其他合并逻辑零变化。
- **`intakeDrift` 消费者 census**：函数仅在本脚本内被 `collectIntake` 与 `--self-check` 调用（无导出消费方）；新参数带默认值，旧调用行为不变。
- **`computeRetroScope` 消费者 census**：`closeout-check.mjs` / `retro-staleness-check.mjs` / `merge-driver.mjs` 三处单源消费；排除 `current` 对三者一致为正收益（`current` 从不是真实包）。
- **`settings-presence` 聚合面**：`pod-events-aggregate.mjs` 按 eventType 白名单聚合（dispatch / escalation / friction 等），未知型不计数、不报错——新增型不改变任何既有聚合数值；retro 若需按 `declaredForm` 分桶，走既有事件读取面（本批不加聚合器代码）。

**【硬项 · 复扫模式完备性先验】**：本批无退役语面、无重命名、无目录搬家、无路径改落——`path-form-family.md` 的形态族谱无消费面（新文本均为新增句/新增值，不替换旧路径写法）。唯一「替换型」文本 = `planner.md` 的插入（不改既有句）、`loop-retro.md` 的漂移标句（旧句被新句整体替换、旧句不残留）、`changes-index.mjs` 的 `intakeDrift` 函数（整体替换，旧函数体不残留）——三处均以 §5 V6 的字面 grep 正反配对断言兜底（旧句 = 0 / 新句 = 1）。

## 4. human 取舍点（OQ）

> **裁定记录（2026-10-27，human 两裁已归档）**：OQ1 = **(a) 保守标注 + 观察期**（与草案一致，T1 零改动）；OQ2 = **(b) 同批打通**（T4 据此扩 `merge-driver.mjs` 解锁判据，riskTier 升 medium）。

**OQ1 · 260918 扩展形态降级面：保守标注 vs 先实测 vs 写禁则 —— human 2026-10-27 裁 (a)**

（a）**保守标注 + 观察期（推荐，已裁）**：启动卡写「扩展形态不能传参 ⇒ 三件缺席，按降级申报 + 手工兜底」，并留「若实测可传参则改写」升级分支句。理由 = 两批均未实测、把未验事实写成禁则会误伤合法用法；申报事件已让降级率可统计，观察期后再决定是否升门或放行。
（b）**本批先实测再定文面**：需 human 起一次 VS Code 扩展会话、尝试带 `--settings` / `--agent` 起步（成本一轮人工），实测结果决定文面正反。若选此项，T1 第 4 件与 OQ 结论同批落。
（c）**写成禁则**：扩展形态一律不得起步 Pod，必须走 CLI。理由最省心，但当前无实测支撑、且可能在扩展形态本可传参时过度限制。

**OQ2 · `awaiting-human:merge-gate` 的归属：本批留根 vs 同批打通 driver —— human 2026-10-27 裁 (b)**

（a）**本批留根**：merge-gate 停等继续用 `blocked`——`merge-driver.mjs:1101` 的人批解锁判据以 `blocked` 为键，打通需同批改 driver（超出 ack「只动调度态枚举与 board 投影」的边界，manifest +1，评审面扩大）。本批先解决真机轮 / 人等裁决两类等待的可见性；merge-gate 迁移登记 §6 留根。
（b）**同批打通（已裁）**：扩 `approvalRefUnlock` 判据接受 `awaiting-human:*` 族（+ 自检夹具），merge-gate 停等改用新族、token 与解锁链均不卡死；同时把 planner 合并门 ESCALATE 分支的设值改为 `awaiting-human:merge-gate`。代价 = 触合并链脚本，§5 已增 driver 自检项、riskTier 升 medium（T4 第 8/9 项）。

**非 OQ 但提请评审注意的技术取舍（已给理由，judge 可 challenge）**：

- **取舍 A · T2 的 verified 只给 5 条**：只认「独立于条目自身的耐久锚」（review.md GREEN 行 / progress.md human 拍板行 / INDEX closed + 末提交在 main）；base ref 已不可达的两条与「需 human 持板」的持续条件一条一律 unverified。宁可留 warn，不自撰凭据（原则**源出 `ARQ-260914` / 260921 T6 的顾虑「强填 status 即自撰凭据」**，与 ack④ 兼容；R1-F5 更正原稿「ack 红线」归属）。**退路**：判官若认为 migration 应一律 unverified（更保守），把 5 条降回 unverified 即可，其余不动。
- **取舍 B · 新增 `settings-presence` 事件型而非复用 `friction`**：复用会让「设置缺席」混进 friction 计数与 retro 的摩擦主题，语义错位（friction = 流程磨难，本型是常态申报）；且 `friction` 事件有必填报文 schema，申报在场（正常态）时没有 friction 可写。新增型 non-bearing、不进任何聚合口径，风险面 = 闭集 +1、OS §5d 同步句 +1。**退路**：判官若认为不该扩闭集，改为「仅申报降级态时才写 friction（frictionType=environment）」——但会牺牲「在场也可统计」这一半。
- **取舍 C · base ref 记 commit SHA 而非分支名**：分支会前移，`<base>..<head>` 圈定与 detect_changes 都需要稳定锚；261018 先例的分支形态在跨包共享分支时无法直接当 base 用。启动卡同记分支名，供人读定位。**退路**：判官若要求兼容旧先例，可写「SHA（分支名）」两件并列（本批已按此写）。
- **取舍 D · 不修 261021 spine 文件本体**：本批只改「怎么报」；spine 修补属其 owner（主线程）的 intake 事务，且修文件会把这批的 diff 混进别人家账。**退路**：判官若要求同批补，须先在 ack 内扩 scope（本批不预留实现）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> **断言纪律（T 自守）**：脚本面预期值 = 「按本稿 §2 逐字节文本装配进 pristine 副本」的实跑输出（副本在仓外 `C:/tmp/p4-assembly`，装配脚本从 §2 文本逐字节装配、不手抄，起草后即弃）；文档面预期值 = **处方正推**（基线 `grep -cF` 实测 + 处方新文在该文件恰插入一次 ⇒ 预期 = 基线 + 1），基线数字均为起草期实跑。落地期实跑若与本节有字节差异，以落地实跑为准覆写 §5 并在 §7 留痕。
> **时点定义**：pre-commit = 落地文件在盘、未 commit 即可跑（迁移对照的 HEAD 仍指落地前提交）；post-commit = 须落地提交后才成立。

**基线（现行 HEAD，起草期 2026-10-26～27 实跑，本 gov worktree）**：

| # | 命令 | 实跑输出（逐字 / 摘要） |
|---|---|---|
| B1 | `node .claude/scripts/portfolio-reconcile.mjs --self-check` | `{"selfCheck":"changeid-prefix","pass":4,"fail":0,"total":4,"ok":true}` + `{"selfCheck":"portfolio-reconcile","ok":true,"pass":6,"fail":0,"total":6}`，exit 0 |
| B2 | `node .claude/scripts/changes-index.mjs --self-check` | `{"selfCheck":"changeid-prefix","pass":4,"fail":0,"total":4,"ok":true}` + `{"selfCheck":"changes-index","ok":true,"pass":41,"fail":0,"total":41}`，exit 0 |
| B3 | `node .claude/scripts/portfolio-reconcile.mjs --set-pod selfcheck-probe --status awaiting-human:real-device`（现行） | `Error: setPod: invalid status "awaiting-human:real-device"`（抛出点 `:141`，exit≠0） |
| B4 | 临时 git 仓夹具（仓外）：pods/a.json status=`awaiting-human:merge-gate` + board holder=a → 现行 `reconcile` | `{"baselineWip":{"queued":1},"baselineHolder":"a"}`——未知值被静默归一 `queued`，且 token 仍挂在 `a`（泄漏面实证） |
| B5 | `CLAUDE_PROJECT_DIR=<repo> node --input-type=module -e "import {collectIntake} from './.claude/scripts/changes-index.mjs'; …"` | 4 条漂移：`261012 ⚠未闭环` / `261014 ⚠未闭环 ⚠滞留11d` / `261021 ⚠值域外`（status=`—`）/ `261022 ⚠未闭环`（注：INDEX 文件里 261014 仍显示 10d，因生成件是 10-26 快照） |
| B6 | `node .claude/scripts/loop-health-report.mjs --pending` | `waitingOnLoopHealth` **8 条**、末条 = `current`；`[loop-health] --pending: 6 to analyze, 8 waiting on loop-health.md, 0 excluded …, 81 already analyzed across 5 retro run(s)` |
| B7 | 两份 decision-coverage.json 解析 | `items= 6 typeof= ["string"]`（m1）/ `items= 6 typeof= ["string"]`（m2） |
| B8 | 新字面碰撞基线（`grep -rlF` 全仓，排除本批目录） | `settings-presence`=0 文件 / `⚠源文件缺口`=0 / `isAwaitingHumanStatus`=0 / `isValidPodStatus`=0 / `本包 base ref：`（SKILL 内）=0 / `互斥租用禁令与每包 base ref 钉死`=0 / `settings-presence`（OS 内）=0 |
| B9 | 二十处被替换 / 插入锚的 `split(anchor).length - 1` 逐条核（v0.2 扩含 T4 新增面） | 全部 = **1**（初版十三处：planner.md Step 2 行 / OS 闭集句 / pod-coordination §2c 标题、§3 标题、ESCALATE 行 / SKILL `六要素` 句、空闲判读行、命令行句 / loop-retro 步骤 5 句 / changes-index drift 调用行 / loop-health for 行 / portfolio-reconcile `:123` 行 / pod-events-append 闭集尾；v0.2 新增七处：merge-driver import 行 / `approvalRefUnlock` 上方原注释句 / `:1101` 解锁判据行 / `:1121` 解锁提示语行 / T1c 断言行 / planner.md 多 Pod ESCALATE 行 / pod-coordination「它是持久标记…」句） |
| B10 | **装配版**（§2 逐字节文本装进 `C:/tmp/p4-assembly/scripts` 副本）跑脚本 `--self-check` | `portfolio-reconcile` = `{"pass":12,"fail":0,"total":12,"ok":true}`；`changes-index` = `{"pass":44,"fail":0,"total":44,"ok":true}`；`merge-driver --self-check-merge-friction` = `{"selfCheck":"merge-driver-merge-friction","pass":25,"fail":0,"total":25,"ok":true}`（T1d / T1e 逐条 PASS） |
| B11 | 装配版 `collectIntake` 跑真仓（`CLAUDE_PROJECT_DIR=<repo>`） | 4 条漂移，其中 `261021` = `⚠源文件缺口（frontmatter 未闭合）`，另三条与 B5 逐字相同 |
| B12 | 装配版 `loop-health-report.mjs --pending` | `waitingOnLoopHealth` **7 条**（无 `current`；`260829-audio-bargein-hw` 仍在列 = 正向配对）；`6 to analyze, 7 waiting …` |
| B13 | 装配版 `pod-events-append` 两向夹具（仓外临时项目） | 非法型 `settings-presense` → `throws-invalid`；合法型 `settings-presence` → 写出 envelope `{"schemaVersion":1,…,"eventType":"settings-presence","actor":"planner","source":"planner","payload":{"declaredForm":"extension-session","basis":"launch-card","note":"selfcheck"}}` |
| B14 | 装配版 `reconcile` 令牌矩阵（仓外临时 git 仓） | `ready-to-merge` → wip `{ready-to-merge:1}`、holder=`a`（保留，正控）；`awaiting-human:merge-gate` → wip `{awaiting-human:merge-gate:1}`、holder=null；`awaiting-human` → wip `{awaiting-human:1}`、holder=null；`blocked` → holder=null（既有行为不变） |
| B15 | **现行** `node .claude/scripts/merge-driver.mjs --self-check-merge-friction` | `{"selfCheck":"merge-driver-merge-friction","pass":23,"fail":0,"total":23,"ok":true}`，exit 0（T1a / T1b / T1c 三代解锁夹具在列） |
| B16 | 现行 `approvalRefUnlock` 对 `awaiting-human:merge-gate` 的判据（基线） | 判据 `tp.status !== 'blocked'` ⇒ 家族档一律 `not-applicable`（静态判读：`:1101` 逐字在场；装配版 T1d/T1e 夹具在现行实现下必红——同族形态下 `r='not-applicable'`） |

**落地验证集**：

| # | 时点 | 命令 | 预期（实跑回填 / 正推） | 判别力 / 配对 |
|---|---|---|---|---|
| V1 | pre-commit | `node .claude/scripts/portfolio-reconcile.mjs --self-check`；`node .claude/scripts/changes-index.mjs --self-check` | 前者 `pass:12 / fail:0`（含 6 条新 pod-status 断言逐条 PASS）；后者 `pass:44 / fail:0`（含 3 条新 intake 断言） | 反转实现（不扩族）→ B3 抛错、B4 wip 归一 queued 且 token 泄漏，新组断言不可满足；不传 `spineOk/hasStatus` → 缺口 fixture 落 `⚠值域外`、新第 1/2 条变红 |
| V2 | pre-commit | 同 B5 的 `collectIntake` 一行（仓内脚本） | 4 条漂移：`261021` = `⚠源文件缺口（frontmatter 未闭合）`；`261012` / `261022` = `⚠未闭环`；`261014` = `⚠未闭环 ⚠滞留11d`（日期随当日重算） | 与 B5 对拍（同一条目报法改变、其余三条逐字不变）；260921 已落的对象形态 fixture 由 V1 的第二条自检兜住 |
| V3 | pre-commit | `node .claude/scripts/loop-health-report.mjs --pending` | `waitingOnLoopHealth` **7 条**，不含 `current`；`260829-audio-bargein-hw` 仍在列 | 负向（不含 current）× 正向（真实 wait 仍列出）配对；与 B6 对拍 = 8 → 7 |
| V4 | pre-commit | 仓外临时 git 仓跑 `--set-pod --status awaiting-human:real-device` 与四态 token 矩阵（同 B14 脚本，用落地后脚本） | `--set-pod` 接受并写 pods json；四态 wip / holder 与 B14 逐值相同 | 与 B3/B4 对拍（现行拒收 + 泄漏） |
| V4b | pre-commit | `node .claude/scripts/merge-driver.mjs --self-check-merge-friction` | `{"pass":25,"fail":0,"total":25,"ok":true}`；`T1d awaiting-human:merge-gate + 有效 ref → 解锁` 与 `T1e 族内任一档无有效人批 → fail-closed 保持 waiting` 逐条 PASS；T1a/T1b/T1c 与基线 B15 逐字同 | 与 B15/B16 对拍（23 → 25；现行实现下家族档 `not-applicable` ⇒ T1d / T1e 双红）；blocked 路径零变化由 T1a/T1b/T1c 三代夹具守住 |
| V5 | pre-commit | 迁移对照脚本（下附）：对两份文件逐条比 `text` ≡ `git show HEAD:<file>` 的原字符串、键集 ≡ `{text,scope,status,evidence}`、verified/unverified 计数 = 3/3（m1）、2/4（m2） | 全部断言 true | 负向（不出现裸 string、不出现 `verified` 而 evidence 为空）× 正向（下标顺序与原文逐字相同、5 条 verified 各有 evidence）配对 |
| V6 | pre-commit | 白名单正推清单（下附 20 条 `grep -cF`） | 逐条 = 表列预期（基线 B8 = 0 的条目落地后 = 1；`六要素` 基线 1 → 落地后 **0**，与 `七要素` = 1 配对；T4 新增字面基线亦全 0） | 处方正推：基线实测 + 处方恰插入一次 ⇒ 预期 = 基线 + 1 |
| V7 | pre-commit | `git diff --name-only -- docs/contracts/ .claude/hooks/ .claude/scope-map.json .claude/*.settings.json .claude/verify-gates.json .claude/scripts/pre-merge-escalation-gate.mjs AGENTS.md CLAUDE.md docs/DEV-GOVERNANCE.md docs/requirements.md docs/architecture.md` | 空输出 | 负向（禁触面零 diff；v0.2 起 `merge-driver.mjs` 已出禁触列、入 manifest）；正向配对 = `git diff --name-only` 恰等于 manifest 十二行（landing-verifier 逐件核） |
| V8 | pre-commit | `node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261026-gov-review-p4-pod-board-cluster/PROPOSAL.md` | `ok:true`、`anchorCount` = 12、`anchorUnion` 与 manifest 十二行逐条一致、`findings:[]`、`advisories:[]` | 起草期自检实跑留痕见文末 |
| V9 | post-commit | 复跑 V1 / V2 / V3 / V4 / V4b / V5 / V6 | 与上表逐值相同 | **未实测**（依赖落地提交）；由主线程 / landing-verifier 执行 |
| V10 | post-commit | `node .claude/scripts/pre-merge-escalation-gate.mjs 261026-gov-review-p4-pod-board-cluster --source-ref HEAD` | `PASS (…, kind=gov)`，exit 0 | **未实测**（依赖 FROZEN + landing-verdict）；本批无契约 diff，契约凭据分支不触发 |

**V5 迁移对照脚本**（pre-commit；仓根执行；HEAD 仍指落地前提交）：

```sh
node --input-type=module -e "
import { execSync } from 'node:child_process';
import { readFileSync } from 'node:fs';
const spec = [
  ['delivery/workstreams/m1-home-qa/decision-coverage.json', { verified: 3, unverified: 3 }],
  ['delivery/workstreams/m2-char-window/decision-coverage.json', { verified: 2, unverified: 4 }],
];
let bad = 0;
for (const [f, want] of spec) {
  const oldArr = JSON.parse(execSync('git show HEAD:' + f, { encoding: 'utf8' })).launchPreconditions;
  const arr = JSON.parse(readFileSync(f, 'utf8')).launchPreconditions;
  const assert = (c, msg) => { if (!c) { bad++; console.log('FAIL ' + f + ': ' + msg); } };
  assert(arr.length === oldArr.length, 'length ' + arr.length + ' != ' + oldArr.length);
  const counts = { verified: 0, unverified: 0 };
  arr.forEach((it, i) => {
    assert(JSON.stringify(Object.keys(it).sort()) === JSON.stringify(['evidence', 'scope', 'status', 'text']), 'keys@' + i);
    assert(typeof it.text === 'string' && it.text === oldArr[i], 'text@' + i + ' not verbatim');
    assert(it.status === 'verified' || it.status === 'unverified', 'status@' + i);
    assert(it.scope === 'workstream' || /^package:.+/.test(it.scope), 'scope@' + i);
    assert(typeof it.evidence === 'string', 'evidence@' + i);
    assert(!(it.status === 'verified' && !it.evidence), 'verified-without-evidence@' + i);
    counts[it.status]++;
  });
  assert(counts.verified === want.verified && counts.unverified === want.unverified, 'counts ' + JSON.stringify(counts));
  console.log(f + ' => ' + JSON.stringify(counts));
}
process.exit(bad ? 1 : 0);"
```

**V6 白名单正推清单**（字面逐字取自 §2 处方新文；命令形态 = `grep -cF '<字面>' <文件>`；基线列 = B8 / 起草期实测）：

| # | 文件 | 字面（逐字） | 基线 | 预期 |
|---|---|---|---|---|
| 1 | `.claude/agents/planner.md` | `1h. **设置在场性自检` | 0 | 1 |
| 2 | `.claude/agents/planner.md` | `--event-type settings-presence` | 0 | 1 |
| 3 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | `非 CLI 形态（VS Code 扩展会话 / IDE 内会话）降级面` | 0 | 1 |
| 4 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | `**数行启动卡，七要素**` | 0 | 1 |
| 5 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | `六要素`（旧句残留面） | 1 | **0** |
| 6 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | `本包 base ref：` | 0 | 1 |
| 7 | `.claude/skills/kidpal-pod-launch-prep/SKILL.md` | `互斥租用检查（261026 批 · 语义 SSOT = pod-coordination §2c）` | 0 | 1 |
| 8 | `.claude/references/pod-coordination.md` | `互斥租用禁令与每包 base ref 钉死` | 0 | 1 |
| 9 | `.claude/references/pod-coordination.md` | `awaiting-human[:<reason>]` | 0 | 1 |
| 10 | `.claude/references/pod-coordination.md` | `\| Pod 挂 `（表行首片段；grep 时去掉反斜杠转义） | 0 | 1 |
| 11 | `.claude/commands/loop-retro.md` | `intake 漂移处置` | 0 | 1 |
| 12 | `.claude/references/dev-loop-operating-system.md` | `settings-presence` | 0 | 1（该段单行） |
| 13 | `.claude/scripts/pod-events-append.mjs` | `'settings-presence',` | 0 | 1 |
| 14 | `.claude/scripts/portfolio-reconcile.mjs` | `export function isAwaitingHumanStatus` | 0 | 1（装配实测 B10 同源） |
| 15 | `.claude/scripts/loop-health-report.mjs` | `if (id === 'current') continue;` | 0 | 1（装配实测） |
| 16 | `.claude/scripts/changes-index.mjs` | `spineOk: fields !== null` | 0 | 1（装配实测） |
| 17 | `.claude/scripts/merge-driver.mjs` | `isAwaitingHumanStatus(tp.status)` | 0 | 1（装配实测 B10 同源） |
| 18 | `.claude/scripts/merge-driver.mjs` | `T1d awaiting-human:merge-gate`（新夹具断言行） | 0 | 1（装配实测） |
| 19 | `.claude/agents/planner.md` | `--status awaiting-human:merge-gate` | 0 | 1 |
| 20 | `.claude/references/pod-coordination.md` | `awaiting-human:merge-gate` | 0 | 1（该段单行；`awaiting-human[:<reason>]` 形式另计 #9） |

（注：#5 是**负向**条目，与 #4 正向配对；#13–#16 的预期来自装配版 `grep -cF` 实测——`settings-presence`=1 行、`isAwaitingHumanStatus`=3 行（含两处调用）、`⚠源文件缺口`=5 行；如以整句锚核，请用表中字面在本稿 §2 的落地形态为准，勿用单 token 计数代替整句断言。）

## 6. 留根清单

**本批结项（landing 后由主线程执行登记 / 状态变更，architect 不做）**：`ARQ-260918-pod-session-settings-identity-unobservable`（**部分结项**：候选② + 候选①后半已落；候选③门策略与「实测前置」按 OQ1=(a) 留根，结项时须写明「未闭合面 = 候选③ + 扩展形态实测」）、`ARQ-260914-portfolio-reconcile-launchpreconditions`（存量 12 条已迁；读侧 260921 已落 ⇒ 本条念闭）、`ARQ-261025-shared-branch-pod-isolation`（**部分结项**：方案①已落；方案②与池化按 human 裁归后续批）、`ARQ-261025-wait-state-closed-set`（**整条结项**：枚举族 + board 投影 + merge-gate 停等与 driver 解锁判据均落——OQ2=(b) 同批打通，无留根项）、`ARQ-261025-intake-index-retro-scope-hygiene`（三项全落）。

**留根与复议触发**：

1. **260918 候选③门策略不做**：是否给「设置在场性」加机械牙（如 Stop 钩子自检 settings 加载）留观察期。复议触发 = `settings-presence` 事件在 retro 统计中出现 `extension-session` / `unknown` 占比 ≥ 一个周期且伴随一次「未跑 supervisor-gate-check 即欲收尾」实况。
2. **扩展形态能否传参未实测**（OQ1）：若实测可传参，启动卡降级面改写为「按 ①② 同法传参」，并把 `settings-presence` 的 `declaredForm` 枚举语义同步。复议触发 = OQ1 结论或首次实测。
3. **`awaiting-human` 族的 reason 只是 board 可读性、不承担权限语义**（OQ2=(b) 已打通）：解锁放行权在五要素人批事件核验（`resolveHumanApproval`），族内任一 reason 无有效人批一律 `kept-blocked`；前置升级闸的 blocked 探针不改（stop 态改名不改变放行结果——ESCALATE 包无人批由「非 APPROVE」拦、有人批由 `escalateExempt` 放行，等价性分析见 §2.0 复证 4）。复议触发 = 出现「某 reason 档被误当权限档」的实例（如 planner 把真 BLOCK 写成 awaiting-human），届时评估给 reason 加受控枚举白名单。
4. **`scanAllChanges`（`--all` 便捷扫）仍会把 `current` 当作一个 change 扫描**：本批只按 ARQ 范围改 `computeRetroScope`（游标三消费方单源）；`--all` 是观察面便捷扫、非游标。复议触发 = `--all` 输出因 `current` 造成误读。
5. **261021 spine 文件本体未修**（frontmatter 未闭合）：本批只改报法；修补归主线程 intake 事务。复议触发 = 该 spine 被实际消费（如跨面续跑）仍解析失败。
6. **`settings-presence` 不进聚合器**：本批零改 `pod-events-aggregate.mjs`；retro 若需机器分桶再立。复议触发 = 连续两轮 retro 手工统计降级率。
7. **启动卡 base ref 的落地形态**：本批规定 SHA（= 包首提交父）+ 分支名两件；若池化批采用不同形态（如注册表登记），以池化批 SSOT 为准并回收本条。
8. **`awaiting-human` 与 intake spine 的 `awaiting-human:<gate>` 同名不同域**：pod 调度态用本批受控族（reason 限 `[a-z0-9-]`），intake status 沿用 change-intake 值域（可含中文 reason，如 `立项-ack`）——两域互不消费。复议触发 = 出现跨域混读。

**环内自律**：本批起草期有仓外装配副本（`C:/tmp/p4-assembly`，含五个脚本副本，v0.2 起加 `merge-driver.mjs`；外加仓外临时 git 仓与 pod-events 夹具），全部不进仓、不 stage；§2 的五个脚本改法以装配实跑为准回填 §5。仓内 `delivery/review/261026-gov-review-p4-pod-board-cluster/_scratch/` 的装配脚本起草结束后删除（不随批 commit）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-27 - architect

verdict: BLOCK(F1,F3)

**判档复核**：Track B 主路径成立——轻路径判据③命中（T2/T4/T5 落 `.claude/scripts/**`，路径即判据恒 Track B，只升不降、无降档事由）；Track A 段两条判据均不命中（非跨系统结构性变更；方向由 ARQ 预消化 + human 2026-10-26 四拍 ack 收敛）。riskTier low-medium 证成成立：零门判定语义变更亲核（`--check-launch` 判据零改、token 仅扩释放面、merge-driver 零触碰）。无升级 human 项——OQ1/OQ2 已把该 human 裁的给全。

**findings**：

- **F1 · SHOULD · 复证 1 的 grep 引据不实**：§2.0 复证 1 称「`grep -n "settings"` 只命中 `:12` 的角色说明与 `:59` 的 proposal 来源自检」。亲核实测 `grep -n "settings" .claude/agents/planner.md` 仅命中 `:12` 一处；`:59`（Step 2 契约变更行）不含 "settings"，"proposal 来源自检" 实际在 Step 5b（`:65`）且亦不含该词。结论本身（planner.md 无设置在场性自检步）亲核成立、不受影响。修法：更正该句引据（删 `:59` 命中声明或如实改单命中）。
- **F2 · NIT · spine 行数差一**：复证 5 称 261021 spine「全文 42 行」，实测 `awk 'END{print NR}'` = 41（文件以 CRLF 收尾）。实质断言亲核成立：`grep -c '^---'` = 1（仅开头围栏、无闭合）、无正文、parseSpine 返回 null。随 F1 顺手更正。
- **F3 · SHOULD · T1 Step 1h 触发句与判定依据②内部张力**：Step 1h 开头限定「launch-argument 形状起包时（Step 1①）」，而判定依据②又写「自建形状 / 无启动卡 → 按会话可观察特征申报」——字面读来自建形状会话可判 1h 不适用，恰漏掉 ARQ 观测面「每包必现」的自建一半。修法方向：触发句改为两形状皆适用（如「起包时（launch-argument 与自建形状皆适用）」），落地前更正。
- **F4 · NIT · V6 计数头句与表列不符**：§5 V6 头句「下附 12 条 `grep -cF`」与白名单表 #1–#16 共 16 条不符；更正计数即可。
- **F5 · NIT · 取舍 A 引据归属**：「无核验痕迹不得自撰 verified」被标为「ack 红线」，立项 ack 四拍（task_plan.md:63）无此字面；原则源出 ARQ-260914（260921 T6「强填 status 即自撰凭据」顾虑）。归属更正即可，原则本身与 ack④ 兼容、执行正确（5 条 verified 锚逐条亲核成立，见留痕 2）。

**亲核留痕（R1 实测，本 gov worktree）**：

1. **行号与引据**：portfolio-reconcile.mjs `:36-45`（八值闭集）/:46/:123/:141/:175/:249-263、pod-events-append.mjs `:29-40`（8 值，闭集尾锚 grep -cF=1）、merge-driver.mjs `:1101`（`tp.status !== 'blocked'` 逐字在场）与 `:1731`（ready-to-merge 过滤）、changes-index.mjs `:174`（collectDev 排除 current）/:324/:397-406/:425/:501/:696、loop-health-report.mjs `:177-208`（for 行 `:192`，确无 current 排除）、SKILL.md `:66`/:122/:131-133、pod-coordination.md `:49-60`/:73-80/:126、dev-loop-operating-system.md `:130`（枚举串锚 grep -cF=1）、loop-retro.md `:44`（替换句原文 grep -cF=1）、planner.md `:12`/:58/:59/:111-112（supervisor-gate-check 显式步在场）、task-intake-and-slicing.md `:298-311`（`:311` 存量迁移句逐字在场）、enforce-scope.mjs `:9`——全部亲读在场，与 PROPOSAL 引用一致（F1 的 grep 计数除外）。
2. **T2 五条 verified 锚逐条亲核成立**：m1 review.md:7 = 「**裁决：GREEN**」（门 1 · 2026-09-15）；m2 review.md:7 = 「**裁决**：**GREEN**」（2026-09-17）；m1 progress.md:11 = 「human 2026-09-16 AskUserQuestion 认可」；`delivery/taskpackages/INDEX.md:77/:79` = k1a / k3 两行 closed；d5194c94 = 两包目录各自 `git log -1` 末提交且 `git branch --contains` 含 main（实测输出含 `dev/260915-m1-k1a-key-layer-dispatch` 等分支族）。base/m1-home-qa 与 base/m2-char-window 实测 `git cat-file -e` 均报 `Not a valid object name`（exit 128）——「不可填 verified」判据成立。unverified 7 条（含 m2 #4 持续条件）分类正确。
3. **基线复跑逐值同**：B1 self-check = changeid-prefix 4 + portfolio-reconcile 6（exit 0）；B2 = 4 + 41（exit 0）；B3 实跑 `Error: setPod: invalid status "awaiting-human:real-device"`（抛出点 :141，exit 1）；B4 仓外临时 git 仓 + 预置 board holder 复现逐字同 `{"baselineWip":{"queued":1},"baselineHolder":"a"}`（静默归一 queued + token 泄漏双实证）；B5 collectIntake 实跑 4 条漂移逐字同（261014 = `⚠未闭环 ⚠滞留11d` 与当日重算一致；261021 = `⚠值域外` status=`—`）；B6 waitingOnLoopHealth 实测 8 条、末条 = `current`，汇总行 8 waiting / 81 analyzed / 5 runs 逐字同。
4. **census 抽查**：POD_STATUSES 全 `.claude/scripts/` + `.claude/hooks/` grep 仅 portfolio-reconcile.mjs 一文件（「无第二处消费」成立）；新字面碰撞全 0 实测（settings-presence / ⚠源文件缺口 / isAwaitingHumanStatus / isValidPodStatus / `本包 base ref：`SKILL 内 / `awaiting-human[:<reason>]`pod-coordination 内）；「互斥租用」全仓唯一命中 = task_plan.md:63（与 PROPOSAL 声明同）；「六要素」SKILL 内 = 1（其余命中均在 archive 历史制品，非 V6 #5 scope）。
5. **V8 preflight 复跑**：`ok:true, anchorCount:11`，anchorUnion 十一项与头部 landing-manifest 围栏逐条一致，`findings:[]/advisories:[]`，exit 0。
6. **结构字段在场**：landing-manifest 围栏块（11 行）+ packet-binding 行均在场；两份 decision-coverage.json 实测 CRLF（grep -qU $'\r' 命中）——T2 行尾保持义务有据。
7. **ack 四拍 → 文本对照**：①五条一批 Track B 主路径 ✓（review 轨行）；②260918 低分叉子集（候选②+①后半、零新门）✓（T1 + 非目标 + OQ1）；③shared-branch 方案① + 池化另批 + 方案②留池化批 ✓（T3 + 非目标 + 留根 7）；④260914 迁移搭批 ✓（T2，verified=5/unverified=7 与 V5 计数 m1 3/3、m2 2/4 自洽）。无不忠实表述。
8. **T4 设计自洽性**：读侧归一/写侧校验/token 释放三处同批改齐 ✓；自检 6 条判别力成立（放宽 family 或去掉 family 判据均致后两条变红）；与 merge-driver `blocked` 键的耦合经 OQ2(a) 显式留根（§6 留根 3）非漏接；T3 活跃态清单含 `awaiting-human*` 与 T4 前向一致；wip 原值分桶不占 merge token 与 §8 增行对偶 ✓。
9. **T5③ 兼容性亲核**：现行 intakeDrift 41 条旧自检断言无 statusBase='—' 用例；生产路径 statusBase='—' 必伴 statusRaw 空 ⇒ hasStatus=false 提前返回缺口分支，`&& statusBase` 条件的删除不改变任何可达行为。
10. **「包首提交父」散落实证**：grep 全仓命中 261022-p3-record-chrome-ghost-fix/scope-final.md、261022-p3-record-front-polish-fix/{pod-retro.md,scope-final.md} 与 delivery/retro/2026-10-25-loop-retro.md（复证 3 所列三制品 + retro 全部在场）；261018-sd-log-usb-read/launch-card.md:6 「本包 base ref = `dev/pod-m2`」逐字在场。

**结论**：无方向级缺口、无门语义漂移、无漏承接根因、无撞车（intake spine `awaiting-human:<gate>` 同名异域已由留根 8 显式消歧）。F1/F3 为证据引据与落地文本措辞级修正（应修）；F2/F4/F5 为 NIT 随手更正。R2 聚焦 F1/F3 修订到位性 + 修订是否引入新矛盾，无需全量重核。

### R2 - 2026-10-27 - architect

verdict: BLOCK(F1)

**轮次纪律**：R2 聚焦复审——只核 R1 findings 修订到位性 + OQ2=(b) 扩改面 + 作者自报两点 + 修订是否引入新矛盾；R1 已核面不重复跑，仅在受 OQ2=(b) 波及处补测。

**findings**：

- **F1 · BLOCKING · R1 记录拼接处逐字损坏 + 「零改动」声明不实**：机械 diff（R1 节现文 vs 判官 R1 原文全节比对）唯一偏差在 R1 亲核留痕第 6 项——拼接复原时丢失 `$'` 两字符并引入一个换行：判官原文为单行「…实测 CRLF（grep -qU $'\r' 命中）——T2 行尾保持义务有据。」，现文断为两行且成「（grep -qU ⏎ \r' 命中）」。修订记录 #8 自称「R1 文字零改动（`（grep -qU ` 与 `\r' 命中）` 两段逐字保留）」——两片段确各自逐字保留，但「零改动」在拼接处不成立。R1 的 verdict 行、findings、结论经 diff 证实全部逐字未动（唯一偏差即此一处），机器锚完好；但 §7 是评审 loop 的审计轨迹、本仓「证据引用逐字精确」纪律下，判官记录的 verbatim 损坏 + 不实的零改动声明须修复后方可冻结。修法：留痕 6 复原为单行并补回 `$'\r'`；修订记录 #8 改为如实表述（「拼接处 `$'` 两字符曾丢失，已复原」）。
- **F2 · NIT · T4 第 9 项「不得全局替换」的枚举不全**：`:117` 整行锚 `grep -cF` = 1 亲核成立；但 `--status blocked` 在 planner.md 现行共 **3** 处（`:115` 多 Pod APPROVE→升级闸拦分支、`:116` BLOCK 分支、`:117` ESCALATE 分支），原文只点名 `:116`「亦出现一次」。操作指令（整行替换、不得全局替换）正确无误，`:115` 本就不该动（闸 BLOCK = 真 BLOCK，与 T4 第 6 项「真 BLOCK 仍用 blocked」一致）；仅枚举补全即可。

**R1 findings 修订到位性（逐条亲核）**：F1 ✓（复证 1 改「仅命中 `:12` 一处」+ Step 5b `:65` 不含该词，与实测一致）；F2 ✓（复证 5 与 T5 证据均改 41 行）；F3 ✓（Step 1h 触发句改「起包时（launch-argument 与自建形状皆适用；Step 1① 只是 launch-argument 形状的探测入口）」，内部张力消除）；F4 ✓（V6 头句 20 条与表列 #1–#20 对齐）；F5 ✓（取舍 A 归属改 `ARQ-260914` / 260921 T6 并注 R1-F5 更正）。五条均改净、未改出新错。

**OQ2=(b) 扩改面亲核**：

1. **blocked 路径零变化**：解锁判据改动纯加法（`blocked` ∨ 家族）；T1a/T1b/T1c 三代夹具不动且判据仍成立（T1c `ready-to-merge` → `not-applicable` 扩族后依旧）；B15 实跑现行 `--self-check-merge-friction` = `{"selfCheck":"merge-driver-merge-friction","pass":23,"fail":0,"total":23,"ok":true}` exit 0 逐字同基线。B16 静态判读成立（`:1101` 逐字在场 ⇒ 现行实现下家族档必 `not-applicable`，T1d/T1e 在现行实现下双红——判别力成立）。
2. **放行权单源**：`resolveHumanApproval`（`lib/governance-gate-shared.mjs:644-673`）五要素核验（eventType / actor / payload.decision / changeId / schemaVersion）只读 pod-events、**不读 pod status**，亲核成立；T1d（族内 + 有效 ref → `unlocked`）与 T1e（族内 + runner 抛错 → `kept-blocked` + 文件态不变）判别力成立，且与既有 T1 夹具同构（`t1` / `setPod` / `readFileSync` / `join` / `assert` 均为该块既有名，插入位「T1c 断言行之后」可行——T1c 断言行 `grep -cF` = 1）。
3. **判据实现面**：T4 第 8c 项单点替换 `:1101`，`isAwaitingHumanStatus` 从 `portfolio-reconcile.mjs` import 单源、无正则复制 ✓；`:1121` 提示语改如实带原停等态 ✓（`:1097` 注释锚、import 行锚 `grep -cF` 均 = 1）。
4. **前置升级闸等价性链——独立复核成立**：`escalateExempt`（`pre-merge-escalation-gate.mjs:380-386`）要求 verdict === 'ESCALATE' + provenance 完整 + approval.ok；「非 APPROVE」拦截（`:448`）不看 pod status；blocked 探针（`:501-504`，闸内唯一读 pods json status 处）消费于 `:415-416`（`ctx.blocked && !escalateExempt` 才拦）。故停等态改 `awaiting-human:merge-gate` 后：无人批 → 非 APPROVE 照拦（探针假阴性不开放任何新路径）；有人批 → escalateExempt 放行——放行结果两态同旧。一处界外观察（非流内状态、不构成 finding）：若 pod 停等期间 verdict 被重跑为 APPROVE，旧码会因 blocked 探针拦、新码放行——但 planner 流程不产生该状态（ESCALATE 停等期不重跑 verdict），且 merge-driver Step 1b 同 invocation 无条件重跑全闸背书，记录在案备查。
5. **planner.md 改动面**：`:117` 整行锚唯一（grep = 1）；替换仅两处字面（`--status awaiting-human:merge-gate` + 「merge-gate 停等态」），`OQ2=(a)` 字样保留正确（指 planner.md 既有承诺、非本批 OQ）；`:113`（单 Pod，无 board 条目）与 `:116`（真 BLOCK）不动正确。`--status blocked` 计数 3 处的枚举疏漏见 F2。
6. **census / 基线**：V6 #17–#20 基线全 0 实测 ✓；B9 新增七锚（merge-driver import 行 / `:1097` 注释句 / `:1101` 行 / `:1121` 提示语行 / T1c 断言行 / planner 多 Pod ESCALATE 行 / pod-coordination「它是持久标记」句）实测各 = 1 ✓；pod-coordination §3 标题 / §8 ESCALATE 行锚各 = 1 ✓。

**其余核对**：manifest 12 行 ✓（merge-driver 入列）；V8 preflight 复跑 `ok:true, anchorCount:12`、exit 0 ✓；riskTier 升 medium 如实（合并链执行面 + 失败模式点名 + 缓释三条具体，成立）；V7 禁触列已移出 merge-driver ✓；§6 wait-state 改整条结项与 OQ2=(b) 一致、留根 3 重写为族语义 + 闸不改等价性记录 ✓；OQ2=(b) 不与 ack③ 冲突（ack③ 留给池化批的是方案② `<base>..<head>` 扩参，本批动的是解锁判据，两事无关）；§3 sweep 表 merge-driver 行补齐 ✓。

**结论**：OQ2=(b) 扩改设计成立（加法判据 + 放行权单源 + 双向夹具 + 闸等价链复核通过），R1 五条 findings 全部改净；唯一否回项 F1 为 R1 记录拼接处两字符复原 + 修订记录措辞更正（纯记录保真，不动任何改法与验证集）。F2 为 NIT 随手更正。R3 只核 F1 复原到位，无需重核其它。

### R3 - 2026-10-27 - architect

verdict: GREEN

**轮次纪律**：R3 只核 F1 复原到位性 + F2 枚举补全 + R2 判官记录复原一致性；其余面不重核（R1 / R2 已核）。

**F1 复原到位**：R1 亲核留痕第 6 项已恢复判官原文单行「…实测 CRLF（grep -qU $'\r' 命中）——T2 行尾保持义务有据。」——`$'\r'` 两字在位、无换行；R1 全节与判官 R1 原文机械 diff 逐字一致（verdict 行 / findings / 留痕 / 结论 / 机器锚未动，diff 零输出）。修订记录 #8 改述如实（「片段逐字保留 ≠ 拼接零改动」，与 R2-F1 判定一致）；#9 复原记录与实况相符。

**F2 补全到位**：T4 第 9 项枚举改为 3 处——`:115`（多 Pod APPROVE 后升级闸拦分支）/ `:116`（BLOCK 分支）/ `:117`（ESCALATE 分支，本项唯一应改处）——与判官实测一致；`:115` / `:116` 不动理由（真闸 BLOCK / supervisor BLOCK = 真 BLOCK）与 T4 第 6 项「真 BLOCK 仍用 blocked」口径自洽；修订记录 #10 如实。

**R2 判官记录复原**：入档时 R2 节 F1 段被 `$'` 转义吞字两处（缺口以〈缺字〉标记）、节头带作者侧修复注。复原前机械 diff 证实：除两处吞字缺口、作者修复注与尾部两个多余空行外，R2 节其余文字与判官 R2 原文逐字一致。本判官持原文，已将 R2 节整体重贴复原为判官原文逐字（verdict 行 / findings / 亲核留痕 / 结论全节一致）；作者修复注与〈缺字〉标记随复原消除，尾部空行归一。无无法复原处。

**并发写入核查**：入档前实测 `### R1` / `### R2` / `## 修订记录` / `## 起草期自检留痕` 各恰 1 处（grep -c 全 = 1）——作者报告的「尾段重复五份」在本判官入档时点已不存在，无残留异常。

**结论**：R2-F1 / R2-F2 全部收口，复原未引入新矛盾；R1 五条（F1–F5）+ R2 两条（F1–F2）findings 全部结案，无未结 BLOCKING / SHOULD。verdict GREEN——本批可进 FROZEN（判档维持 Track B 主路径、riskTier medium、制品与验证集义务照常）。

## 修订记录（作者侧 · v0.1 → v0.3，2026-10-27）

> 本节是作者侧修订留痕，不替代 §7 判官记录；R1 / R2 判官文字除下第 8–11 行所述的两处拼接复原与结构修复外，未作改动。

**依据**：R1 verdict BLOCK(F1,F3) + R2 verdict BLOCK(F1)（§7）+ human 2026-10-27 OQ1=(a) / OQ2=(b) 两裁。

| # | 来源 | 修订 |
|---|---|---|
| 1 | R1-F1 | §2.0 复证 1 的 grep 引据更正：删「`:59` 第二命中」，如实记 `settings` 仅命中 `:12` 一处；proposal 来源自检在 Step 5b（`:65`）且不含该词 |
| 2 | R1-F2 | 复证 5 与 T5 证据的 spine 行数 42 → 41 |
| 3 | R1-F3 | T1 Step 1h 触发句改「起包时（launch-argument 与自建形状皆适用；Step 1① 只是 launch-argument 形状的探测入口）」——两形状均须申报，消内部张力 |
| 4 | R1-F4 | §5 V6 头句计数与表列对齐（现 20 条） |
| 5 | R1-F5 | §4 取舍 A 的归属更正为 `ARQ-260914` / 260921 T6 顾虑，非「ack 红线」 |
| 6 | OQ2=(b) | T4 扩：`merge-driver.mjs` 解锁判据扩族（import 受控族单源 / 解锁行 / 提示语如实带原状态 / 自检 T1d+T1e），planner.md 多 Pod ESCALATE 设值改 `awaiting-human:merge-gate`，pod-coordination §3 同步；manifest 11 → 12；riskTier low-medium → **medium**；§5 增 B15 / B16 / V4b / V6 #17–#20；§6 wait-state 改**整条结项**、留根 3 重写；§0 / §3 / §2.0 复证 4 同步 |
| 7 | OQ1=(a) | §4 记裁定；T1 零改动 |
| 8 | 结构修复（v0.2） | §7 R1 第 6 项在入档时被一次追加截断（跨断并夹带重复自检段）：当时把两段拼接复原、删除重复段。**该次拼接有偏差**（R2-F1 指出）：丢失原文 `$'` 两字符并引入换行；两片段各自逐字保留 ≠ 拼接零改动。原文为单行 `…实测 CRLF（grep -qU $'\r' 命中）——T2 行尾保持义务有据。` |
| 9 | R2-F1 | 按 R1 原文把亲核留痕第 6 项**复原为单行并补回 `$'\r'`**（v0.3）；本条 #8 的「零改动」表述同步改述为「片段逐字保留、拼接处曾有偏差、本轮复原」。R1 的 verdict 行 / findings / 结论经判官 diff 证实逐字未动，机器锚完好，未动 |
| 10 | R2-F2 | T4 第 9 项枚举补全：`--status blocked` 在 `planner.md` 现行共 **3** 处（`:115` 多 Pod APPROVE→升级闸拦分支、`:116` BLOCK 分支、`:117` ESCALATE 分支），仅 `:117` 应改；`:115` / `:116` 不动；整行锚唯一性与「不得全局替换」指令不变 |
| 11 | 结构修复（v0.3） | §7 R2 记录在入档时被 `$'` 转义在 F1 段吞字两处，且其后尾段（本修订记录 + 自检留痕）被重复写入五份；作者侧纯结构修复：R2 片段按序逐字保留、缺口以〈缺字〉标记、重复段删除。R2 完整原文由主线程持存，可整体重贴替换本节 |
| 12 | R3 记录（主线程 FROZEN 时补记） | #11 末句「可整体重贴替换本节」为 v0.3 时点历史行——R2 记录已由判官在 R3 环整体重贴复原（机械 diff 证实全节与判官原文一致，见 R3 记录），本表各节恰 1 处、无 〈缺字〉 残留 |

**未纳入（作者判断，judge 可 challenge）**：前置升级闸的 blocked 探针不扩族（等价性分析见 §2.0 复证 4 / §6 留根 3）；`scanAllChanges` 不改（§6 留根 4）。


## 起草期自检留痕（模板四句自检的落地记录，2026-10-27）

1. **preflight 实跑**（只读干跑，V8）：见本节末「preflight 输出」块。

2. **路径形态族谱**（`path-form-family.md`）：本批不改任何路径的落点与文件名（无搬家、无改名、无新文件进仓），新增的是事件型名（`settings-presence`）、状态族（`awaiting-human[:<reason>]`）、漂移标字面（`⚠源文件缺口…`）与三个函数名（`isAwaitingHumanStatus` / `isValidPodStatus` / 无新导出函数于 changes-index），已按 §3 硬项做碰撞 census（B8 基线全 0）。唯一「路径文本」新增 = 启动卡模板的 `本包 base ref：<SHA>` 行（不是仓内路径）。

3. **承重引文 byte 对账**：§2 引用的每一处被替换原文 / 锚句，起草期以脚本 `split(anchor).length - 1` 逐条核过恰 1 次（B9 二十处全部 = 1——初版十三处 + v0.2 新增七处，从本稿围栏块机械抽取、不手抄）。脚本面补丁另以「锚计数 ≠ 1 即抛错」的装配脚本施加于 pristine 副本（B10–B14 + v0.2 的 merge-driver 补丁），锚不匹配当场失败。

4. **长文落地自检**：本文件分块写入（v0.3 复跑），写后机检 = `wc -l`、U+FFFD 替换字符扫描零命中、`### T<n>` 标题计数 = 5、`目标文件：` 锚行计数 = 5、§2 围栏内以 `## ` 起头的行计数 = 0；`## 修订记录` 与 `## 起草期自检留痕` 各恰 1 处、`### R1` / `### R2` 各 1 处。另：本稿 T1 的 planner.md 插入段含嵌套围栏，外层用四反引号承载（不是三反引号）——落地时取内层文本，勿把外层反引号带进目标文件。

**preflight 输出**（V8 实跑，2026-10-27，exit 0）：

```
$ node .claude/scripts/landing-fidelity-check.mjs --preflight delivery/review/261026-gov-review-p4-pod-board-cluster/PROPOSAL.md
{"ok":true,"gate":"landing-preflight","proposal":"delivery/review/261026-gov-review-p4-pod-board-cluster/PROPOSAL.md",
 "section2BodyFound":true,"anchorCount":12,
 "anchorUnion":[".claude/agents/planner.md",".claude/commands/loop-retro.md",
   ".claude/references/dev-loop-operating-system.md",".claude/references/pod-coordination.md",
   ".claude/scripts/changes-index.mjs",".claude/scripts/loop-health-report.mjs",
   ".claude/scripts/merge-driver.mjs",
   ".claude/scripts/pod-events-append.mjs",".claude/scripts/portfolio-reconcile.mjs",
   ".claude/skills/kidpal-pod-launch-prep/SKILL.md",
   "delivery/workstreams/m1-home-qa/decision-coverage.json",
   "delivery/workstreams/m2-char-window/decision-coverage.json"],
 "advisories":[],"findings":[]}
exit=0
```

`anchorUnion`（12 项）与头部 landing-manifest 围栏块逐条一致；`advisories` 空 = 锚行未夹带路径记号；`findings` 空 = 无加粗锚行、无缺锚。

**写后机检（2026-10-27 v0.3 实跑）**：`wc -l` = 694；U+FFFD = 0；`### T<n>` = 5；`目标文件：` 锚行 = 5；围栏行共 48（含 1 对四反引号，为 T1 嵌套围栏承载）；§2 围栏内以 `## ` 起头的行 = 0，§2 结束时围栏闭合；`### R1` / `### R2` / `## 修订记录` / `## 起草期自检留痕` 各 1 处。
\r' 命中）——T2 行尾保持义务有据。
7. **ack 四拍 → 文本对照**：①五条一批 Track B 主路径 ✓（review 轨行）；②260918 低分叉子集（候选②+①后半、零新门）✓（T1 + 非目标 + OQ1）；③shared-branch 方案① + 池化另批 + 方案②留池化批 ✓（T3 + 非目标 + 留根 7）；④260914 迁移搭批 ✓（T2，verified=5/unverified=7 与 V5 计数 m1 3/3、m2 2/4 自洽）。无不忠实表述。
8. **T4 设计自洽性**：读侧归一/写侧校验/token 释放三处同批改齐 ✓；自检 6 条判别力成立（放宽 family 或去掉 family 判据均致后两条变红）；与 merge-driver `blocked` 键的耦合经 OQ2(a) 显式留根（§6 留根 3）非漏接；T3 活跃态清单含 `awaiting-human*` 与 T4 前向一致；wip 原值分桶不占 merge token 与 §8 增行对偶 ✓。
9. **T5③ 兼容性亲核**：现行 intakeDrift 41 条旧自检断言无 statusBase='—' 用例；生产路径 statusBase='—' 必伴 statusRaw 空 ⇒ hasStatus=false 提前返回缺口分支，`&& statusBase` 条件的删除不改变任何可达行为。
10. **「包首提交父」散落实证**：grep 全仓命中 261022-p3-record-chrome-ghost-fix/scope-final.md、261022-p3-record-front-polish-fix/{pod-retro.md,scope-final.md} 与 delivery/retro/2026-10-25-loop-retro.md（复证 3 所列三制品 + retro 全部在场）；261018-sd-log-usb-read/launch-card.md:6 「本包 base ref = `dev/pod-m2`」逐字在场。

**结论**：无方向级缺口、无门语义漂移、无漏承接根因、无撞车（intake spine `awaiting-human:<gate>` 同名异域已由留根 8 显式消歧）。F1/F3 为证据引据与落地文本措辞级修正（应修）；F2/F4/F5 为 NIT 随手更正。R2 聚焦 F1/F3 修订到位性 + 修订是否引入新矛盾，无需全量重核。
