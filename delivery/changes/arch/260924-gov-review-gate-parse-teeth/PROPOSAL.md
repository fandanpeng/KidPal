# PROPOSAL · 260924-gov-review-gate-parse-teeth

> **状态：FROZEN v0.4 · 2026-09-25 · 内容冻结，凭据齐备——主线程凭「立项 ack + GREEN」自冻，授权记本行**。**凭据段（2026-09-24 human 同场四问，AskUserQuestion 回执逐字）**：① 立项 ack + 收圈 = **「全收（Recommended）」**——核心三件 T1–T7 + 候补 4（T8 含 verifier-brief 加固句）+ 候补 5（T9 处方①③）+ T9 合并设计三句（批 B §4-2 围栏承载条款 / §4-5 逐子句时点 / ARQ-260914 (B) 自检第三句）全收；余量复扫清单确认一律不搭。② **OQ-1 = 「(c) 可观测齿先行」**（contextSource + expectCheck 两 additive 键 + 量化触发器：absent ≥1 → 启动闸级强制批 / 累计 30 全 match → settled 结项）。③ **OQ-2 = 「(a) 批准向严（Recommended）」**（verify.mjs 未知 phase fail-loud——判定行为向严变更，本裁定原文即批准凭据，批 B 先例形态）。④ **OQ-3 = 「(b) 退役归档」**（canary 删除 + 条目按退役处置结项 + 覆盖缺口如实记结项注——**非推荐位，human 明确择退役**）。⑤ architect GREEN = §7 R3（2026-09-25，同判官三轮收敛 R1 BLOCK(F1,F2) → v0.3 修订 → R2 BLOCK(F3) → v0.4 单点修订 → R3 GREEN，findings 全清零、未触熔断、升级 human 零项；verdict 行首机锚 `verdict: GREEN` 在 §7，Track B 资格判官独立裁成立〔R1 透镜⑤裁定〕）。packet-bound landing 基准 = 本文件 §2 改法集（T1–T10）+ §5 验证集（landing-verifier 比对基准 = v0.4 §2）。
> **版本沿革**：v0.1 起草（零脚手架、§5 起草期基线全量实跑、preflight 绿、五件承载 ARQ 复证追注已 append 工作树）→ v0.2 立项 ack + OQ 三裁记录（manifest 32→31 行：OQ-3=(b) 裁 SKILL 锚出、三结项件转 `_archive/2026/` 终址；T4/T7 条件分支失效、仅作选项空间记录供判官核 OQ 给全性；V4/V7 按裁定改写现役分支；T4(b) 覆盖缺口核补起草期实跑实证〔enforce-scope 本体无 --self-check：grep 0 命中 + live 跑无输出 exit 0，2026-09-24〕+ canary 活引用 census = 零〔10 命中全为脚本本体/历史制品/账本/本批 PROPOSAL〕）→ **v0.3 = R1 findings 修订**（判官 R1 verdict `BLOCK(F1,F2)` + N1–N3，零升级 human，§7 在案）：F1 T5 census 三处〔§2 T5 证据段 / §3 透镜① / §5 V0〕583/367 → **694/478** 勘正——v0.1 census 脚本字段清单漏计 crossEnd.counterparts[].path 111 条（全在盘零缺失），判官独立重跑与主线程复证逐位一致，逐字段明细入 §2 T5 · F2 三处计数勘正〔review profile scripts「15 件」→「17 件 = 15 改 + lib 新增 1 + canary 删除 1」/ §4 呈报句「19 件」加勘误注不静默改写（human 出示场历史记录）/ riskTier「hooks 面 2 件」→ 4 件〕· N1 零脚手架声明写面句对齐工作树实况〔生成 INDEX 一件 M + crq 侧两件前置 commit 已收〕· N2 T6/留根 5 merge-driver 措辞「ours 策略面」→「投影冲突 --theirs 特判（PROJECTION_CONFLICT_FILES）」〔代码实况 :831-837〕· N3 T7/§4-OQ1 批 A OQ-3(a) 引文改逐字原文〔机械提取自批 A PROPOSAL §4 OQ-3 选项 (a) 理由段〕· **v0.3 补遗（2026-09-25，R2 中间发现归因）**：T5 census 口径注补落——43 为「包目录」数非「历史包」（含在飞 perf-k1），`current/` 指针副本 12 条为排除项（计入即 706/490，与判官 R2 中间复现逐位一致、差额机械归因），§2 T5 / §3 透镜① 两处同步。→ **v0.4 = R2 finding 修订**（判官 R2 verdict `BLOCK(F3)` + N4 随行提醒，F1/F2/N1–N3 全部核验到位、零升级 human，§7 在案）：F3 零脚手架声明写面句勘正——v0.3 曾误书「crq 侧两件 INDEX 随前置 commit `2fce4b6` 收讫」，该 commit 实含一件（`docs/crq/gov/INDEX.md`，git show --stat 亲核）；另一件（`_archive/2026/INDEX.md`）起草期全程零 diff、无收讫事项，措辞已改逐件可验形态 · N4 Status 行滞后同步——DRAFT v0.2 → DRAFT v0.4（阶段说明补 R1/R2 轮次与 verdict 线；FROZEN 转移时标 FROZEN v0.4）。全部修订落 PROPOSAL 文本面，零脚手架态不变。→ **FROZEN v0.4（2026-09-25）**：R3 GREEN 凭据入 Status 凭据段⑤；landing 相位自此启动（T1–T10 packet-bound，零脚手架解除、§5 验证集逐条执行）。
> **承载 ARQ**：`ARQ-260923-gate-tooling-noise-batch`（T1–T4 全承载四处一行级小修）· `ARQ-260923-portfolio-decomposer-drift` 残余③（T5 承载；①②已由批 A T5 落地、本批不重复）· `ARQ-260913-current-pointer-implicit-global` 残余机械齿两件（T6 承载 18 解析点常驻报警、T7 承载 `--expect-change-id` 力度裁定）· `ARQ-260924-landing-summary-template-pointer-gap`（T8，收圈待裁）· `ARQ-260924-landing-gate-dir-anchor-gap` 处方①③（T9，收圈待裁；②触承重闸本批不预批）。**并批设计面**：批 B retro §4-2/§4-5 与 `ARQ-260914-landing-preflight-anchor-parse` 追注 (B) 第三句的 PROPOSAL.template 载体候选并入 T9 合并设计（同一文件面一次改齐，收圈待裁）。**不关的根**：ARQ-260914 §4-1「承重引文 byte 级比对脚本化」机械化候选不在本批（留根 1）。五件条目起草期实测复证追注均已 append（2026-09-24，工作树未 commit，随本批 landing 提交）。
> **review 轨**：Track B 常驻轻路径（SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。**轨道自评（写实，不自我美化）**：核心三件均为执行实测出的病灶（loop-retro-2026-09-23 跨棒实证 + 批 A OQ-3(a) human 裁定明文留批 C + 本批起草期八项只读实测复核）= Track B 快道资格（不论 G 几）；不触 `docs/contracts/**`、非跨系统结构性变更 ⇒ 不升 Track A。**gate 事实显式标注交判官独立裁**：① T2（verify.mjs 未知 phase 静默回落改 fail-loud）为**闸判定行为向严变更**——未知 phase 从「静默绿」变「红」，OQ-2 = (a) human 批准、裁定原文记 Status 凭据段作批准凭据（批 B 先例）；T7 原设计含向严分支 (b)（缺旗标即红），OQ-1 = (c) 裁定后该分支失效、现役改法为判定语义零变更的可观测齿（(b) 转留根 3 触发器记录，命中时另批启动）；② T5/T6 触 gate 脚本本体（taskpackage-required-check / mutation-check 在 `.claude/verify-gates.json` 闸集内）但改法形态 = advisory warnings 键 + stderr 回显，**exit code 与 ok 判定零变更**（V5/V6 含闸行为不变断言）；③ T3 的 unknown-flag 拒绝为用法面向严（changes-index / mutation-check 均非闸判定语义；mutation-check 在闸集内但闸调用姿势 = 裸跑无参数，`.claude/verify-gates.json:59` 实录，不受旗标解析变更影响）。存疑向严：判官认为任一件须升 Track A 即升。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T10）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：low-medium（证成：唯一现役向严件 = T2（未知 phase fail-loud，OQ-2=(a) 已批）且仅命中畸形调用——canonical 调用面零存量依赖〔§2 T2 调用面实扫在案〕；T7 经 OQ-1=(c) 裁定为可观测齿 = 判定语义零变更；T5/T6 advisory/echo 零判定变更；T4 = 非闸集脚本退役删除（活引用实扫零）；全批可整批回滚。不取 low 因触 hooks 面 4 件 + 闸脚本本体多件 + T6 接线面广〔14 文件〕+ T4 属不可逆删件〔git 历史可考古，human 已明知覆盖缺口择退役〕）。
> **review profile**：G2+G3（G3 触面枚举：`.claude/hooks/commit-safety.mjs` · `.claude/hooks/verify.mjs` · `.claude/hooks/supervisor-gate-check.mjs` · `.claude/hooks/supervisor-verdict-gate.mjs`〔hooks〕· `.claude/scripts/` 下 17 件〔= 修改 15（含 landing-fidelity-check.mjs）+ 新增 lib 1 + 退役删除 enforce-scope-canary.mjs 1；v0.3 勘正——v0.2 曾误书 15 件，判官 R1-F2 抓出〕；G2 触面枚举：`.claude/references/arch-review-protocol.md` · `.claude/references/track-b-verifier-brief.md`〔v0.2 收圈 = 加固句收入〕· `docs/templates/PROPOSAL.template.md`〔v0.2 收圈 = 合并设计三句收入，G2 邻接〕）+ 账本写回面（`docs/crq/gov/**`，非 G 层分类面）。**G1 触点为零、契约面触点为零、scope-map.json 零触碰、verify-gates.json 零触碰（T5 warn 齿不改闸 cmd；T2/T6 不改闸集成员）、agent defs 零触碰（planner.md 双必带模板批 A 已落、本批不回头改）、settings 族零触碰（批 B settings-lint 接线不受影响）、`.claude/skills/**` 零触碰（v0.2：T4 退役裁定裁 SKILL 挂接锚出）**。
>
> **零脚手架声明（诚实声明，批 B 先例形态）**：本批起草期对工作树 `.claude/**` 与 `docs/templates/**` **零写入**——T1–T9 全部改动只在「立项 ack + OQ 裁定（= 向严件批准凭据）+ architect GREEN + FROZEN」之后的 landing 相位落工作树。起草期实测全部为只读（源码 Read/grep、各脚本 `--self-check` 只读跑、canary 实跑〔只 spawn 钩子断言退出码、零写入〕、仓外临时样例 preflight 干跑、census 枚举）。起草期写入面 = 本批制品目录 + 五件 ARQ 复证追注 + 生成 INDEX 过程制品一件（`delivery/changes/arch/INDEX.md`，upsert 落 M——工作树现况可逐件验；`docs/crq/gov/INDEX.md` 一件随前置 commit `2fce4b6` 收讫〔commit stat 可验〕；`docs/crq/gov/_archive/2026/INDEX.md` 起草期全程零 diff〔历次 render 输出「未变」实录〕、无收讫事项——v0.4 按 R2-F3 勘正：v0.3 曾误书「两件随 commit 收讫」，该 commit 实含一件〕）（账本登记面，主线程职责非脚手架）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。**manifest 形态记录**：v0.1 = 全量条件形态（32 行，含 T7/T8/T9 条件锚与账本锚）；立项 ack 收圈 + OQ 三裁后 v0.2 按裁断裁剪——T7 裁 (a) 即裁 `dispatch-stamp` 锚、T8 不收即裁 protocol/brief 两锚、T9 不收即裁 landing-fidelity/template 两锚、T4 裁 (b) 退役即裁 SKILL 锚；账本五件按结项与否定稿现址或 `_archive/2026/` 迁址（crq-render 自动归位、人不搬文件——manifest 记**落定后终址**，批 B 先例）；`docs/crq/gov/_archive/2026/INDEX.md` 仅在有结项归档时保留。**过程制品注**：`delivery/changes/arch/INDEX.md`（changes-index upsert 过程制品）不入 manifest、于 B6 类工作树申报中如实认列（批 B F3 教训形态）。**v0.2 裁定裁剪实录**：收圈 = 全收（T7/T8/T9 条件锚全保留、T8 加固收入）；OQ-1=(c) → dispatch-stamp 锚保留；OQ-3=(b) → SKILL 锚裁出（launch-prep 文面零触碰）；结项三件（gate-tooling-noise-batch / portfolio-decomposer-drift / landing-summary-template-pointer-gap，结项判定见 T10）转 `_archive/2026/` 终址；31 行 == §2 锚并集（v0.2 preflight 复跑机械对账，输出留痕）。

```landing-manifest
.claude/hooks/commit-safety.mjs
.claude/hooks/verify.mjs
.claude/hooks/supervisor-gate-check.mjs
.claude/hooks/supervisor-verdict-gate.mjs
.claude/scripts/changes-index.mjs
.claude/scripts/mutation-check.mjs
.claude/scripts/enforce-scope-canary.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/scripts/brief-inputs.mjs
.claude/scripts/dispatch-stamp.mjs
.claude/scripts/acceptance-coverage-check.mjs
.claude/scripts/closeout-check.mjs
.claude/scripts/devloop-artifact-check.mjs
.claude/scripts/docs-sync-check.mjs
.claude/scripts/journal-append.mjs
.claude/scripts/loop-metrics.mjs
.claude/scripts/pod-events-aggregate.mjs
.claude/scripts/pod-events-append.mjs
.claude/scripts/security-check.mjs
.claude/scripts/lib/current-pointer-shared.mjs
.claude/references/arch-review-protocol.md
.claude/references/track-b-verifier-brief.md
.claude/scripts/landing-fidelity-check.mjs
docs/templates/PROPOSAL.template.md
docs/crq/gov/_archive/2026/ARQ-260923-gate-tooling-noise-batch.md
docs/crq/gov/_archive/2026/ARQ-260923-portfolio-decomposer-drift.md
docs/crq/gov/ARQ-260913-current-pointer-implicit-global.md
docs/crq/gov/_archive/2026/ARQ-260924-landing-summary-template-pointer-gap.md
docs/crq/gov/ARQ-260924-landing-gate-dir-anchor-gap.md
docs/crq/gov/INDEX.md
docs/crq/gov/_archive/2026/INDEX.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：治理机械面的三类「执行测出的齿缺口」一次收口——① 用户面噪声与危险静默（钩子误导提示、未知 phase 静默回落、传参形态不一、canary 漂移无主）消灭在动作点；② 拆包快照的引用腐坏（指向已不存在物的字段）从「纯 planner 习惯肉搏」升级为机械 warn 齿可见；③ `current/` 隐性全局指针的 18 个解析点获得常驻「指错包报警」回显，`--expect-change-id` 的强制力度凭实扫证据交 human 定夺。

**现状差距**（起草期八项只读实测 + census，2026-09-24，证据逐 T 附）：四处一行级小修全部仍在（commit-safety 误导文案本批开工会话内即再现一次；verify.mjs 静默回落仅存 legacy 路径；changes-index 无 --help、mutation-check 旗标被当路径；canary 现跑 3/7）；taskpackage-required-check 对全部引用字段零磁盘存在性消费（mutation-check:111 另有静默跳过新发现）；18 解析点中 14 个无回显齿；gov worktree 的 `current/` 现正指向 dev 包 `260924-perf-k1-roundtrip-latency`（census 实录——在 gov worktree 裸跑任何 current 消费脚本即病例①现场，报警齿缺位的活例）；preflight 对目录 token 静默放行（临时样例干跑实证 ok:true）而 landing 门精确匹配必红（260923 批实录）。

**非目标（本批不做，防 scope creep）**：
- 不做 ARQ-260914 §4-1「承重引文 byte 级比对脚本化」（机械化候选留根 1，本批面已大不扩）。
- 不做 dir-anchor 处方②（landing 门目录前缀展开——触承重闸判定逻辑，须独立批完整评审，本批不预批）。
- 不搭批 D 候补（`ARQ-260923-planner-roster-reference-drift` 含 G1 fan-out、`ARQ-260923-codemem-mode-doc-drift` 触 AGENTS.md=G1）；其余「可搭任意批」在册件全量复扫结果呈立项 ack 场（§4 前言），默认一律不搭、不自行扩批。
- 不触 G1 指令根 / 契约面 / agent defs / settings 族 / scope-map / verify-gates.json；不恢复 headless launch（批 B 留根 6 仍归 human，本批 launch 一律交互式）。
- 不回头改批 A/批 B 已落文面（T8 = protocol 第 4 步**新增**指针句，非回改）。

## 2. 问题、根因与改法（T1…T10）

**【硬项 · 起草期实证义务】** 本节断言纪律照模板执行：行为断言均附起草期实测（命令 + 关键输出 verbatim + 日期 2026-09-24）；数字引用（行号 / 计数）均现扫核对；承重 landing 指令的锚文一律机械提取（grep -o / 单行 Read 后复制）；含反引号/`${}`/转义序列的承重代码行一律围栏代码块承载（批 B R1-F1 教训，禁 blockquote+code-span 转义形态）。

### T1 · commit-safety 钩子「staged 为空」误导提示（噪声①）

**问题**：钩子在 `git add` 触发时恒报「staged 为空——git add 可能未生效或被静默拒绝」，而它读的是 add 执行**前**的快照——add 触发相该文案恒真且恒误导（消耗每一棒注意力，m2-k2 登记一次、m2-k3 一棒三回、card-fix 两回、批 B 会话两次、本批开工会话内 add 相再现一次）；commit 触发相读真快照、同文案是有效异常信号，须保留。

**证据**（`.claude/hooks/commit-safety.mjs` 现读，2026-09-24）：`buildAdvisory` 在 PreToolUse 时点执行 `git diff --cached --stat`（:61），空态文案在 :66：

```js
    stat || '（staged 为空——git add 可能未生效或被静默拒绝，先跑 git status 核对）',
```

头注 :11-12 已自述时序：「PreToolUse 显示的是**命令执行前**的 staged 状态；`git add` 静默拒绝的实际捕获点在后续 `git commit` 命令到来时（commit 触发已覆盖该场景）」。本批开工会话实录：`git add` 三件后钩子输出「（staged 为空——git add 可能未生效或被静默拒绝，先跑 git status 核对）」，而同命令内 `git diff --cached --stat` 显示 3 files changed（噪声实证）。

**根因**：空态文案不区分触发相——add 相的空是时序常态（快照在命令前），commit 相的空才是异常；一个文案服务两个语义相反的时点。

**改法**：`buildAdvisory` 的空态分支按 `trigger` 分流——纯函数抽出（self-check 可测，本文件既有 `classify`/`graphCacheNotice` 纯函数先例）：

```js
export function statLine(trigger, stat) {
  if (stat) return stat;
  if (trigger === 'add') return '（add 执行前快照：此刻 staged 为空属正常时序，见头注 :11-12；add 后以 git diff --cached --stat 为准核对，commit 触发相将显示 add 后快照）';
  return '（staged 为空——git add 可能未生效或被静默拒绝，先跑 git status 核对）';
}
```

`buildAdvisory` :66 行改为调用 `statLine(trigger, stat)`。一切路径 exit 0 的不变量 I1 不动、判定语义零变更（advisory 级非门）；self-check 增 `statLine` 三态判别 case（add 空→新文案 / commit 空→旧文案 / 非空→原样返回；反转实现必须变红）。头注 :11-12 时序注随改法补一句「add 相空态文案已按时序分流（本批 T1）」。

目标文件：`.claude/hooks/commit-safety.mjs`（改）

### T2 · verify.mjs 未知 phase 值静默回落 scoped 改 fail-loud（噪声②，向严变更 → OQ-2）

**问题**：legacy 路径下 `--phase` 传任何非 `final` 值（含 K7 实烧的 `--phase=full`——timing 词汇误用为 phase 值）都静默按 `scoped` 跑，scoped 绿可能被当 final 绿（差点假绿，K7 retro 判「参数不认时静默降级为默认相位是危险形态」）。

**证据**（`.claude/hooks/verify.mjs` 现读，2026-09-24）：:83-85 全文：

```js
function phaseFromArgs(args = parseArgs()) {
  return args.phase === 'final' ? 'final' : 'scoped';
}
```

bearing 路径已 fail-loud——`normalizeExpected`（`.claude/scripts/verifier-evidence-check.mjs:114`）：`if (!['scoped', 'final'].includes(phase)) throw new Error('phase must be scoped or final');`。**缺口仅存 legacy 路径**。调用面实扫（grep `--phase` 全 `.claude/agents` `.claude/skills` `.claude/references` `docs/DEV-GOVERNANCE.md`，2026-09-24）：canonical 调用模板仅 planner.md:71（`--evidence-mode=round --phase=scoped|final`）与 integrator.md:38（`--phase=final`），均 bearing 模式、均只传合法值——**无存量调用方依赖静默回落**。缺省语义（不传 `--phase` → scoped、只跑 timing=scoped 闸）系 :79-81 注释（「--phase=final 时跑全部；否则只跑 timing=scoped 的闸」）与 phaseFromArgs 现实现的成文设计，保留不动。

**根因**：`phaseFromArgs` 把「值缺失」（合法缺省）与「值非法」（调用方错误）折叠进同一回落分支。

**改法**（**OQ-2 = (a) 批准向严已裁**，v0.2 凭据段——裁定原文即批准凭据；(b) 维持回落分支自此失效）：入口校验先于一切分派（含 `--gate-child` 分支——父进程只传合法值，子进程复校验幂等无害），非法值 stderr + exit 2：

```js
const VALID_PHASES = ['scoped', 'final'];
function assertPhaseArg(args) {
  if (args.phase === undefined) return; // 缺省 → scoped（fail-safe 默认，:80-81 成文设计，保留）
  if (typeof args.phase !== 'string' || !VALID_PHASES.includes(args.phase)) {
    process.stderr.write(`[verify.mjs] 未知 --phase 值 ${JSON.stringify(args.phase)} —— 只认 scoped|final（fail-loud，不静默回落；bearing 路径 normalizeExpected 同判据）\n`);
    process.exit(2);
  }
}
```

接线点 = 现 :378-382 尾部派发段之前（`if (isGateChild) runGateChild();` 之前），对 `parseArgs()` 结果调用一次。`--phase` 裸旗标（无值，parseArgs 产 `true`）同判非法。**同步补 `--self-check` 分支**（verify.mjs 现无 self-check，grep 0 命中实录；260813 批 argv 触发型义务先例、mutation-check.mjs:136-137 结构注「self-check 分支先于早退」同款）：case 集 = 缺省→放行 / scoped→放行 / final→放行 / full→exit 2 / true（裸旗标）→exit 2 / garbage→exit 2（spawn 子进程断言退出码，enforce-scope-canary 形态）。**valid 值的 legacy 全量实跑不入 §5 live 验证**——legacy 跑闸会写 `current/verdict.json` 与 current 所指包的 `verify-runs.jsonl`（:112-115/:135），gov worktree 的 current/ 现指 dev 包 perf-k1（census 实录），裸跑即簿记污染，故 valid 相位覆盖由 self-check 纯 case 承担。

目标文件：`.claude/hooks/verify.mjs`（改）

### T3 · changes-index / mutation-check 传参形态对齐 + 补 --help（噪声③）

**问题**：changes-index.mjs 无 `--help`——想看用法的调用落入 else 分支触发全量重建（rma8 整合期实烧，顺带纠偏 4 行历史 INDEX）；mutation-check.mjs 只认位置参数，`--change-id <id>` 旗标被当路径 resolve 后报「TaskPackage file does not exist」（m2-k1 踩），两脚本传参形态不统一（m2-k1 retro 判「治理脚本传参形态不统一是系统性小税」）。

**证据**（现读，2026-09-24）：changes-index.mjs :528-544 分支 = `--self-check` / `--backfill` / `--stale-report` / `--upsert`，else 直落 `main()`（:542-543），无 help 分支；mutation-check.mjs :13-18：

```js
function taskPackagePath() {
  const explicit = process.argv[2] || process.env.DEV_LOOP_TASKPACKAGE || process.env.TASKPACKAGE_PATH;
  if (explicit) return resolve(projectDir, explicit);
```

`--change-id=x` 作 argv[2] 被 resolve 成路径、:89 existsSync 失败即 `fail('TaskPackage file does not exist')`。

**根因**：两脚本各自生长，无统一旗标解析约定；「未知输入」与「合法缺省」同落默认行为分支。

**改法**：① changes-index.mjs 增 `--help`/`-h` 分支（打印头注 :9-13 用法块 + 四分支语义，exit 0）；未知 `--*` 旗标改 usage 错误 exit 2（用法面轻度向严——不再静默全量重建；**裸跑无参数仍全量重建，行为不变**）。② mutation-check.mjs `taskPackagePath()` 增 `--change-id <id>` 旗标（解析为 `delivery/taskpackages/<id>/taskpackage.json`；优先级 = `--change-id` > 位置参数 > env > current/ 回退）+ `--help` + 未知 `--*` 旗标 usage 错误 exit 2（根治「旗标被当路径」病灶）；位置参数向后兼容保留。③ 闸安全性：`.claude/verify-gates.json:59` 实录 mutation-check 闸调用姿势 = `node .claude/scripts/mutation-check.mjs` 裸跑——无参数路径零变更，闸判定语义零变更。④ mutation-check `--self-check` 增判别 case（`--change-id` 解析 / 未知旗标 exit 2 / 位置参数兼容；现基线 8/8 绿，2026-09-24 实跑）。changes-index `--self-check` 现基线 29 例 fail 0（实跑），增 help/unknown-flag case。

目标文件：`.claude/scripts/changes-index.mjs`（改）· `.claude/scripts/mutation-check.mjs`（改）

### T4 · enforce-scope-canary 退役归档（噪声④；OQ-3 = (b) 已裁）

**问题**：canary 现跑 **3/7、4 FAIL**（2026-09-24 实跑，红基线在案）——期望侧过期无主：非闸集脚本漂移后无人被提醒复跑。

**证据**（现跑输出 verbatim，2026-09-24）：

```
FAIL  [exit 0, 期望 2]  architect subagent 写 .claude/** → 拦
FAIL  [exit 0, 期望 2]  大小写变体 "Architect" 写 .claude/** → 归一后仍拦（F3）
FAIL  [exit 0, 期望 2]  大写+后缀 "ARCHITECT-foo" 写 .claude/** → 归一+回退基础角色仍拦（F3）
FAIL  [exit 2, 期望 0]  architect 写 docs/systems/** → 放行（起草面 allow）
```

归因（scope-map.json 现读）：architect allow 已含 `.claude/agents/**`（authoring-agents-skills owner=architect 治理变更；review-loop 自定义另列个别 deny）→ 前三条期望「拦」过期；`docs/systems/**` 虽在 architect allow，但 docs 平铺后 universal 层拦未登记子目录（stderr 实录「docs/ 下未登记的子目录 docs/systems/」）→ 第四条期望「放行」过期。与 m2-k2 登记（四条、3/7）总数一致；细分与条目转引「2+2」有出入，现扫 3+1，复证追注已回写。**退役前置核（起草期已实跑，2026-09-24）**：① 活引用 census = **零**——`grep -rln "enforce-scope-canary" .claude/ docs/ delivery/ AGENTS.md CLAUDE.md` 10 命中全为脚本本体 / 历史制品（delivery/changes、delivery/taskpackages、loop-retro 报告）/ 账本条目 / 本批 PROPOSAL，无任何 SKILL/agents/hooks/settings 接线；② 覆盖缺口核——enforce-scope.mjs 本体**无 --self-check**（grep `self-check|selfCheck` 0 命中 + `--self-check` 实跑无输出 exit 0），F1/F3 行为契约逻辑仍在钩子本体（agent_type 读取 :174、大小写归一 :178-180「runtime 若传大小写变体（如 "Architect"）」），但 canary 退役后该两契约**零等价回归覆盖**——缺口如实记条目结项注（不掩盖、不另立条目重开——human 明知代价择退役，裁定回执原文含「覆盖缺口核记录」）。

**根因**：canary 期望钉的是 2026-08 时代的授权面快照；授权面经治理批演进后无人复跑（不在 verify 闸集、无复跑清单挂接）。

**改法**（**OQ-3 = (b) 退役归档已裁**，v0.2 凭据段；(a) 修复期望 + 挂 launch-prep 分支自此失效、仅作选项空间记录供判官核 OQ 给全性）：删除 `.claude/scripts/enforce-scope-canary.mjs`（git 历史留档可考古）；`ARQ-260923-gate-tooling-noise-batch` 条目 ④ 按「退役处置」承载（条目处方原文即「挂进某复跑清单**或声明退役归档**」两选，退役 = 处方第二支兑现，随 ①②③ 全承载结项，见 T10）；结项注逐字记录覆盖缺口（上方 ② 核结果原文）。**F1/F3 契约逻辑本体零触碰**（enforce-scope.mjs 不改一行；退役的只是其回归测试件）。

目标文件：`.claude/scripts/enforce-scope-canary.mjs`（删）

### T5 · 引用字段磁盘存在性 warn 齿（decomposer 残余③，advisory 不拦门）

**问题**：机械闸只查字段形态、查不出「引用指向的东西还在不在」（m2-k2 四处引用腐坏实证：验收文件路径磁盘不存在 / 冻结件已归档 / elf 路径 cwd 基准错 / refs 不含真实改动面）；批 A T5 已把 intake 人工四问成文进 planner.md（1f 步），机械 warn 齿按其 OQ-3(a) 裁定留本批。**起草期实测新发现同族件**：mutation-check.mjs:111 对不存在的验收测试文件静默跳过——验收文件路径全错的包可绿过 mutation 闸。

**证据**（现读 + census，2026-09-24）：taskpackage-required-check.mjs 对 docRefs/sourceRefs/contractRefs/planningRefs/acceptanceTestsFile/origin.ref(upstreamRef)/crossEnd.counterparts[].path **零磁盘存在性消费**（全文 existsSync 仅 :121 本体、:145 canonical 比对、:390 input-brief 双份三处）；mutation-check.mjs:111：

```js
    if (!existsSync(absolutePath)) continue;
```

brief-inputs.mjs 已有半齿——renderAnchorSections :125-127 对缺失文件产 `status:'missing'` 行（brief 内可见），但 advisories 通道只覆盖 bare-anchor（:131），missing 无 advisory。**sweep 透镜预跑**（census 命令 = inline python 机械枚举，2026-09-24；v0.3 按 R1-F1 勘正：v0.1 版 census 脚本字段清单漏计 crossEnd.counterparts[].path，下为七类字段全映射重跑数，判官独立重跑与本复证逐位一致）：本 worktree 43 个包目录（= 全部 `delivery/taskpackages/*/` 目录，含在飞包 perf-k1；**排除 `current/` 指针副本**——副本 changeId 同为 perf-k1、七类字段 12 条且全在盘，若计入即 706/490，判官 R2 中间复现与此逐位一致、差额 12/12 全落「在场」桶，口径归因机械可解释）**694 条**引用字段条目（七类含 crossEnd.counterparts[].path）——按 `ANCHORABLE_TEST`（governance-gate-shared.mjs:40，前缀正则）路径形状过滤后：**478 条在场 / 87 条路径形缺失（分布 23 包，主因 = `docs/spec.md` 已退役归档与 260829 时代测试文件迁移）/ 129 条非路径 token**（story id `S-F1.1` 类、`.planning/` 仓外前史工件——齿必须跳过，否则纯噪声）；**crossEnd.counterparts[].path 计 111 条、全部在盘（零缺失零非路径）**——逐字段明细：docRefs 63 在场/32 缺失/34 非路径 · sourceRefs 192/19/47 · contractRefs 43/1/0 · planningRefs 21/1/23 · acceptanceTestsFile 30/34/0 · origin.upstreamRef 18/0/25 · crossEnd 111/0/0。census 同时证明：**历史包普遍带陈旧引用 ⇒ warn 齿必须继承本闸零追溯姿势**（只查 current 默认读取或显式传入的当前包，不追溯历史——taskpackage-required-check 头注 :15「零追溯 = 已 merge 历史包（本闸只读 current taskpackage）」既有原则）。

**根因**：闸的字段校验与磁盘现实解耦；缺失态在三个脚本里分别被静默跳过（mutation-check）、无 advisory（brief-inputs）、根本不查（taskpackage-required）。

**改法**（三处 warn 齿，全部 advisory——exit code 与 ok 判定零变更）：① taskpackage-required-check.mjs 增纯函数 `collectRefWarnings(tp, existsFn)`（依赖注入可测）：对七类引用字段取 fragment-stripped 路径、过 `ANCHORABLE_TEST` 形状过滤（非路径 token 跳过）、`existsSync` 不在场者入 `warnings[]`（`{field, ref, reason:'path-not-on-disk'}`）；ok 与 fail 两路输出均带 `warnings` 键；**增 `--self-check` 分支**（现无，grep 0 命中实录）——判别夹具：假 ref→warnings 非空 / 在场 ref→空 / story-id 非路径→不产 warn（反转实现必须变红）。② brief-inputs.mjs renderAnchorSections missing 分支补 advisory push（:131 bare 同款形态）；self-check 增 missing-advisory case（现基线 28/28 绿）。③ mutation-check.mjs :111 静默 continue 改为收集进 `warnings[]` 随 ok payload 输出（fail 路径行为不变）；self-check 增 case（现基线 8/8 绿）。**闸集安全**：verify-gates 裸跑姿势下 warnings 为纯增量键——verify.mjs 对闸 stdout 只解析 notExecuted 三信号（:160-170 实录），附加键不影响；gate log 截断面 boundedText 承载。

目标文件：`.claude/scripts/taskpackage-required-check.mjs`（改）· `.claude/scripts/brief-inputs.mjs`（改）· `.claude/scripts/mutation-check.mjs`（改）

### T6 · current/ 解析点常驻「指错包报警」回显齿（current-pointer 残余①机械齿）

**问题**：经 `delivery/taskpackages/current/` 解析包上下文的脚本群在指针指错时静默按错包跑（三包历史实证：home-p1 九闸全绿跑错包 / display-first-m1 簿记写进上一批 / schedule-reminder 投影不刷静默假绿；k4-reminder 漏刷两条 dispatch-completed 误落他包）。dispatch-stamp 已有回显齿（260918 T2）、verifier-evidence-check 已有 mismatch fail-loud 齿（:258-260），其余解析点裸奔。**起草期活例**：gov worktree 的 `current/` 现指 dev 包 `260924-perf-k1-roundtrip-latency`（census 实录）——在 gov worktree 裸跑任何 current 消费脚本即病例现场，无任何报警。

**证据**（机械枚举，2026-09-24；grep pattern = `taskpackages[/'].*current` 全 `.claude/scripts` `.claude/hooks`，逐件计数）：**现扫 18 件**（条目/部分承载注称 17，差 ±1 = 当时 grep 口径，复证追注已回写、以现扫为准；18 件全部于 260918 批前即存在，git 首提日期核过）。分类：**待接回显齿 14 件** = hooks 3（verify.mjs:123〔persistVerifyRuns，病例①承载者〕/ supervisor-gate-check.mjs:618 / supervisor-verdict-gate.mjs:468）+ scripts 11（acceptance-coverage-check:25 / closeout-check:51 / devloop-artifact-check:42 / docs-sync-check:22 / journal-append:51 / loop-metrics:64 / mutation-check:17 / pod-events-aggregate:29 / pod-events-append:99 / security-check:13 / taskpackage-required-check:29）；**已有齿 2 件** = dispatch-stamp（:215 始终回显 resolved changeId + source，260918 落）/ verifier-evidence-check（:258-260 `current TaskPackage changeId mismatch` throw，bearing 模式硬齿）；**非解析消费 2 件** = brief-inputs（:254/:318 census 时**排除** current 目录，非经其解析上下文）/ merge-driver（:831-837 current 投影列入投影冲突 **--theirs 特判**白名单 `PROJECTION_CONFLICT_FILES`〔rebase 语境 theirs = 被重放的 Pod 侧，恒为被合并包赢〕+ :796-797 防 current 拼入白名单的守卫，均非解析点——v0.3 按 R1-N2 勘正：v0.2 曾误书「ours 策略面」，代码实况为 theirs 特判）。

**根因**：current/ 是隐性全局状态，解析发生时零可见性——错与对在输出上无差别（dispatch-stamp R1-F6 评审已确立「始终回显来源」纪律，但未推广到其余解析点）。

**改法**：新增共享 lib `.claude/scripts/lib/current-pointer-shared.mjs`（单源收敛，changeid-shared.mjs 先例形态）：导出 `resolveCurrentTaskPackage(scriptName, projectDir)` → `{ changeId, source }` 并**始终** stderr 回显（dispatch-stamp :215 同款纪律，错与对不再无差别）：

```js
export function echoResolution(scriptName, changeId, source) {
  process.stderr.write(`[${scriptName}] TaskPackage 上下文解析：changeId=${changeId || '(none)'}（来源：${source}）\n`);
}
```

14 件待齿文件在各自 current/ 回退解析点接入（显式传参路径同样回显、source 如实区分——`--change-id 旗标` / `位置参数` / `current/ 指针` 三态，dispatch-stamp `resolveChangeIdWithSource` :69-81 形态对齐）；逐件改动 = 1-3 行（import + 解析点替换/回显插入），**exit code 与判定语义零变更**（纯 stderr 可见性齿）。lib 自带 `--self-check`（isMain 守卫，acceptance-id-shared.mjs:182-185 对偶形态）。钩子侧接线注：verify.mjs 回显落 persistVerifyRuns 解析点（:123-127，fail-open try 内不影响不变量）；supervisor 两件在各自 changeId 回退解析处。闸 stdout/stderr 采集面：回显行进 gate log（boundedText 截断承载，verify.mjs:194-196 实录）——纯增量文本、不触 notExecuted 三信号 pattern（`degraded`/`executor-skipped`/`allLiveDeselected` 字面不含）。**已有齿 2 件与非解析 2 件不动**（理由如上分类）。

目标文件：`.claude/scripts/lib/current-pointer-shared.mjs`（新增）· `.claude/hooks/verify.mjs`（改）· `.claude/hooks/supervisor-gate-check.mjs`（改）· `.claude/hooks/supervisor-verdict-gate.mjs`（改）· `.claude/scripts/acceptance-coverage-check.mjs`（改）· `.claude/scripts/closeout-check.mjs`（改）· `.claude/scripts/devloop-artifact-check.mjs`（改）· `.claude/scripts/docs-sync-check.mjs`（改）· `.claude/scripts/journal-append.mjs`（改）· `.claude/scripts/loop-metrics.mjs`（改）· `.claude/scripts/mutation-check.mjs`（改）· `.claude/scripts/pod-events-aggregate.mjs`（改）· `.claude/scripts/pod-events-append.mjs`（改）· `.claude/scripts/security-check.mjs`（改）· `.claude/scripts/taskpackage-required-check.mjs`（改）

### T7 · dispatch-stamp `--expect-change-id` 可观测齿（current-pointer 残余②；OQ-1 = (c) 已裁）

**问题**：旗标现为**可选**硬校验——在场即 fail-loud（不符/无上下文均 exit 2），缺旗标静默跳过校验；批 A T6 已把「双必带」落进 planner.md 调用模板（纯文面），批 A OQ-3(a) 原裁（该批 PROPOSAL §4 OQ-3 选项 (a) 理由段逐字）「本批文面纪律先跑一段，实测「文面成文后是否仍复现」再定机械齿的形态与力度」，闸级强制（缺旗标即红）留本批交 human 拍。

**证据**（现读 + 实扫，2026-09-24）：dispatch-stamp.mjs :217-219 注释自述「`--expect-change-id` 为**可选**硬校验开关——传了则『不符』与『无上下文』两态均 fail-loud……**不传则行为与改前完全一致**」；:220-230 实装核对相符。**批 A 落地后调用侧漏带实扫**（批 A ff 落 main = 10:16 UTC；实扫面 = 全四 worktree 的 pod-events.jsonl `dispatch-completed` 事件 953 条时间线 + m4-k1 pod-retro + verify-logs）：批 A 落地后仅 m4-k1 一包有派发活动——5 次 dispatch（12:38–14:24 UTC），其 worktree 于 11:38 UTC rebase 后已含批 A 双必带模板（`git rev-list` 基点核过），5 事件全落对包、零污染、pod-retro 零摩擦。**但事件面结构性不可审计**：`dispatch-completed` payload（:296-311）不记旗标在场与否、不记 resolved source；:215 回显瞬态——「零漏带实例」实为**零污染症状**而非「旗标恒带」证明，且样本仅 1 包/5 dispatch/~6h。planner.md:66 批 A T6 条款实录：「`--task-package-id` 与 `--expect-change-id` 双必带」。调用方全量面 = planner.md [DISPATCH-STAMP] 模板（post-cutoff + terminal-status 两形态），无脚本级调用方（grep 全 `.claude` 核过；loop-metrics:293-294 仅字符串引用）。

**根因**：旗标可选 + 事件不记旗标态 ⇒ 批 A 原裁所设「文面纪律先跑一段、实测是否仍复现」的观察期**没有观察面**——漏带只能靠污染症状倒推，无症状漏带不可见。

**改法**（**OQ-1 = (c) 可观测齿先行已裁**，v0.2 凭据段；(a)/(b) 分支自此失效、仅作选项空间记录供判官核 OQ 给全性）：
- **(a) 维持文面观察**〔失效记录〕：本 T 全裁不动脚本；批 A 原裁继续有效；条目残余②保留（复证追注的取证记录即本批贡献）。
- **(b) 闸级强制（缺旗标即红）**〔失效记录；量化触发器命中时按留根 3 另批启动，届时本段为其起草起点〕：`--expect-change-id` 改必带——缺失即 stderr + exit 2（`required()` 同款待遇，指向 planner.md [DISPATCH-STAMP] 模板）；旗标恒在场后 :222-225 无上下文态已 fail-loud，:232-235「no-context → exit 0」静默跳过分支对 post-cutoff 调用不可达（保留代码 + 注释标注可达性变化，删否交评审）。pre-cutoff legacy 分支（:317-334）同受必带约束——现存包全部 post-cutoff（cutoff 2026-07-25，2609 时代包实录），legacy 调用面已死，影响 = 零。**判定行为向严**：缺旗标从绿（exit 0 跳过）变红（exit 2）。
- **(c) 可观测齿先行〔裁定现役〕**：payload 增两 additive 键——`contextSource: resolved.source`（三态来源）+ `expectCheck: 'match' | 'mismatch' | 'absent' | 'no-context'`；消费面安全 = pod-events-aggregate:76 / loop-metrics:242 只读 `tokenUsage.status` 分桶键（dispatch-stamp :253-256 注释实录「唯一分桶键」），附加键零影响（additive 先例 = model/modelSource/mainThreadUsage 三键）；量化触发器成文入条目追注（T10 落）：`expectCheck:'absent'` 出现 ≥1 次 → 即启动 (b) 闸级强制批；累计 30 次 dispatch 全 `match` → 文面强制 settled、条目残余②按「观察期完结」结项。**判定语义零变更**（纯事件面增量）。裁定理由（human 回执原文见凭据段）：批 A 原裁「实测复现再定力度」的前提是复现可观测——(c) 恰好补上观察面而不预设结论。

目标文件：`.claude/scripts/dispatch-stamp.mjs`（改）

### T8 · landing-summary 模板指针补进 protocol 第 4 步（候补 4；v0.2 收圈 = 收入，含加固句）

**问题**：Track B 七步第 4 步只说「写 `architect-landing-summary.md`」不指模板，主线程手写漏 landing-manifest 围栏块致核验 FAIL——同根已炸两次（unit-gate F4 + 批 A FAIL），模板 :6 自注早已固化该教训，但 landing 时没人被指向模板。

**证据**（现读 grep verbatim，2026-09-24）：`.claude/references/arch-review-protocol.md:58` 全文「4. 主线程 **packet-bound 落地**（G1/G2/G3 全落，landing-manifest 绑定，落地后不可回改 PROPOSAL）+ 写 `architect-landing-summary.md`；」——无模板指针；对照 :55 第 1 步（「骨架见 `docs/templates/PROPOSAL.template.md`」）与 :60 第 6 步（「骨架见 `docs/templates/retro.template.md`」），缺位形态确认。批 A/批 B 两批靠起草纪律清单（任务书第 2 条）人工兜住，未炸第三实例——但纪律载体是批任务书（一次性文面），protocol 常驻面仍缺。

**根因**：第 1/6 步的指针句式在第 4 步漏装；教训固化进了模板本体、没固化进「指向模板的路标」。

**改法**：:58 行尾「+ 写 `architect-landing-summary.md`」扩为「+ 写 `architect-landing-summary.md`（骨架见 `docs/templates/architect-landing-summary.template.md`，复制勿手写——缺 landing-manifest 围栏块即结构门/核验官合法打回，unit-gate F4 + 批 A 两实例）」——与第 1/6 步指针同款形态、属**新增句非回改**（批 A/批 B 已落文面不动）。**可选加固（v0.2 收圈裁定 = 收入）**：`track-b-verifier-brief.md` 核验清单侧同句提醒（防 protocol 被跳读）。零门判定语义变更（G2 文面）。

目标文件：`.claude/references/arch-review-protocol.md`（改）· `.claude/references/track-b-verifier-brief.md`（改）

### T9 · preflight 目录 token advisory + PROPOSAL.template 纪律句合并设计（候补 5 处方①③ + 批 B §4-2/§4-5 + ARQ-260914 (B) 第三句；v0.2 收圈 = 全收）

**问题**：preflight 锚提取接受目录 token（起草期绿）而 landing 门逐文件精确匹配不消费目录锚（收口期必红）——错误反馈延迟到 FROZEN 之后，修 manifest 即格式例外（260923 批实录：`docs/crq/` 目录简写 → FROZEN 后首跑结构门 RED 两条 → human 追认 + 机械展开 99 行）。同文件面（PROPOSAL.template）另积三处纪律句候选：批 B §4-2 围栏承载条款（near-miss：转义伪影差一点 byte 照抄进 merge-driver）、批 B §4-5 §5 断言逐子句时点、ARQ-260914 (B) 起草期自检第三句（锚文机械提取 + byte 对账）——分批改同节即两次返工同一文件面。

**证据**（现行为干跑实证，2026-09-24）：仓外临时样例（锚行含 `docs/crq/` 与 `.claude/scripts/landing-fidelity-check.mjs` 两 token）跑 `landing-fidelity-check.mjs --preflight` 输出实录：`anchorUnion` 含 `docs/crq/`、`"ok": true`、`"findings": []`、exit 0——目录 token 静默通过。机制 = `ANCHORABLE_TEST`（governance-gate-shared.mjs:40）为前缀正则，`docs/crq/` 必命中。landing 门精确匹配 :613/:615：

```js
const claimedNotLanded = [...summaryTargets].filter((t) => !changed.has(t));
```

（:615 plannedNotLanded 同款）——`changed` 来自 `git status --porcelain -uall` 逐文件行，目录路径永不命中。模板面三候选的实例证据 = 批 B retro §3-F1/near-miss/§3-F2 与 ARQ-260914 追注 (1)(3)(5)（均在案）。

**根因**：两门对「锚 token 的合法形态」口径不一致且无前移反馈；模板纪律句靠 retro 建议载体排队、无合并设计。

**改法**（处方①③ + 模板面三句一次改齐；②不做——触承重闸判定逻辑，条目自注「不预批」维持）：
- **① preflight advisory 齿**：`runPreflight` 输出增 `advisories[]` 键——对 `anchorUnion` 中以 `/` 结尾的目录 token 逐个产 advisory（文案要点：landing 门不消费目录锚〔精确匹配 changed.has〕，manifest 须逐件枚举成员，迁移批先例 260914 批 139 行）；**`ok` 与 exit code 零变更**（advisories 不入 findings——入 findings 会把 preflight 变红、超「advisory 不拦门」处方，向严与否留判官评）；self-check 增判别 case（含目录 token 样例 → advisories 长度 1 / 纯文件样例 → 0，反转实现必须变红；现基线全绿 2026-09-24 实跑）。
- **③ 模板 manifest 纪律句**：PROPOSAL.template.md :23 围栏块说明行「<一行一个目标路径；恰 = §2 各 T 目标文件锚清单并集（条件锚按裁断裁剪；迁移锚只记迁入路径）>」扩为含「一行一个**文件**路径（目录须展开为成员枚举——landing 门不消费目录锚，目录简写收口期必红）」。
- **合并设计三句（v0.2 收圈裁定 = 并批）**：(i) 读法注新增第 4 条**围栏承载条款**——含反引号/`${}`/转义序列的承重代码行必须围栏代码块承载、禁 blockquote+code-span 转义形态（byte 保真 + 渲染稳；批 B R1-F1 实例注）；(ii) 「起草期自检两句」①句尾补 **§5 逐子句时点**——双时点标注的验证集逐子句注明归属时点（批 B A2 实例注）；(iii) 「起草期自检两句」扩**第三句**（ARQ-260914 (B) 建议原文候选）——承重 landing 指令的引文/锚文一律机械提取 + 落笔前后各一次 byte 对账，禁长输出目视转录；工具负结果先第二信号源交叉再归因。三句均纯模板读法注面、零机械变更（不并批分支〔留 ARQ-260914 追注待后续批〕自 v0.2 失效）。

目标文件：`.claude/scripts/landing-fidelity-check.mjs`（改）· `docs/templates/PROPOSAL.template.md`（改）

### T10 · 账本结项与状态回写（landing 相位收口腿）

**问题**：本批承载的五件 ARQ 条目须按落地结果结项/部分承载回写，两件生成 INDEX 重渲染；结项形态依赖 OQ 裁定与收圈结果。

**证据**：批 A/批 B 先例（T4 账本结项 + render；结项条目由 crq-render 自动归位 `_archive/2026/`、人不搬文件）。

**根因**：账本状态是落地的一部分——条目 trigger 行「留批 C」指针须兑现为「已由本批承载」，否则下批复扫读到过期处方。

**改法**（v0.2 按四裁定定稿）：`ARQ-260923-gate-tooling-noise-batch`——①②③ 修复落地 + ④ 退役处置（条目处方原文两选之第二支）= 四处全承载 → **结项**（closedBy 本批，render 归位 `_archive/2026/`）；结项注逐字含 T4 覆盖缺口核记录。`ARQ-260923-portfolio-decomposer-drift`——T5 落地 = ①②（批 A T5）+ ③（本批）全承载 → **结项**（归位同上）；mutation-check:111 新发现件随 ③ 同承载（复证追注在案）。`ARQ-260913-current-pointer-implicit-global`——T6 落地 = 残余① 承载；T7=(c) 落地 = 残余② 转「可观测齿已落 + 量化触发器成文」→ **部分承载不结**（现址留，trigger 改指触发器：absent ≥1 → 闸级强制批 / 30 全 match → settled 结项；触发器原文逐字入追注，留根 3）。`ARQ-260924-landing-summary-template-pointer-gap`——T8 全落（含加固句）→ **结项**（归位同上）。`ARQ-260924-landing-gate-dir-anchor-gap`——T9 处方①③ 落地、② 维持不预批 → **部分承载不结**（现址留，trigger 改「② 须独立批完整评审」）。结项三件由 crq-render 自动归位 `_archive/2026/`（manifest 已记终址，v0.2）；`changes-index.mjs --upsert` 本批 id 于各状态转移点跑（起草已跑〔v0.1 inserted 实录〕/FROZEN/landed/mergedInMain 四时点，AGENTS.md 纪律）。

目标文件：`docs/crq/gov/_archive/2026/ARQ-260923-gate-tooling-noise-batch.md`（改后归位）· `docs/crq/gov/_archive/2026/ARQ-260923-portfolio-decomposer-drift.md`（改后归位）· `docs/crq/gov/ARQ-260913-current-pointer-implicit-global.md`（改）· `docs/crq/gov/_archive/2026/ARQ-260924-landing-summary-template-pointer-gap.md`（改后归位）· `docs/crq/gov/ARQ-260924-landing-gate-dir-anchor-gap.md`（改）· `docs/crq/gov/INDEX.md`（生成件重渲染）· `docs/crq/gov/_archive/2026/INDEX.md`（生成件重渲染）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| hooks | `.claude/hooks/commit-safety.mjs` | G3 | T1 文案分流；advisory 级非门、I1 不变量（一切 exit 0）不动；消费方 = PreToolUse 钩子链 |
| hooks | `.claude/hooks/verify.mjs` | G3 | T2 fail-loud（向严，OQ-2）+ T6 回显（persistVerifyRuns 点）；消费方 = planner Bash 直跑（bearing/legacy）+ gate-child 自派发 |
| hooks | `.claude/hooks/supervisor-gate-check.mjs` / `supervisor-verdict-gate.mjs` | G3 | T6 回显接线（各 1 解析点）；判定语义零变更；消费方 = planner 合并门链 |
| 闸集脚本 | `.claude/scripts/taskpackage-required-check.mjs` | G3 | T5 warnings 键 + T6 回显；**exit/ok 零变更**；消费方 = verify-gates 闸集（裸跑姿势）|
| 闸集脚本 | `.claude/scripts/mutation-check.mjs` | G3 | T3 旗标解析 + T5 warnings + T6 回显；闸调用姿势 = 裸跑（verify-gates.json:59）零影响；位置参数向后兼容 |
| 非闸脚本 | `.claude/scripts/changes-index.mjs` | G3 | T3 help/unknown-flag；INDEX 生成逻辑零变更；消费方 = 主线程状态转移 upsert + AGENTS.md 纪律句 |
| 非闸脚本 | `.claude/scripts/enforce-scope-canary.mjs` | G3 | T4 = (b) 退役删除（v0.2 OQ-3 裁定）；非闸集、活引用实扫零（10 命中全为脚本本体/历史制品/账本/本批 PROPOSAL）；退役后消费方 = 零，F1/F3 契约逻辑本体（enforce-scope.mjs）零触碰 |
| 非闸脚本 | `.claude/scripts/brief-inputs.mjs` | G3 | T5 missing advisory；brief 生成行为不变；消费方 = planner Step 7 前置双份生成 |
| 非闸脚本 | `.claude/scripts/dispatch-stamp.mjs` | G3 | T7 = (c) 可观测齿（v0.2 OQ-1 裁定）：payload 两 additive 键、判定语义零变更；消费方 = planner [DISPATCH-STAMP] 模板 + pod-events 读者群（只读 status 分桶键，附加键零影响） |
| 非闸脚本 | `.claude/scripts/landing-fidelity-check.mjs` | G3 | T9 preflight advisories 键；**主模式（承重结构门）零触碰**、preflight ok/exit 零变更；消费方 = 起草期自检 + landing 结构门 |
| T6 接线脚本 ×11 | acceptance-coverage-check / closeout-check / devloop-artifact-check / docs-sync-check / journal-append / loop-metrics / pod-events-aggregate / pod-events-append / security-check（+上表 mutation-check、taskpackage-required-check） | G3 | 各 1-3 行回显接线；判定语义零变更；消费方 = verify 闸集（coverage/security/closeout 等）与 planner 簿记链 |
| lib 新增 | `.claude/scripts/lib/current-pointer-shared.mjs` | G3 | T6 单源 helper（changeid-shared 先例形态）；自带 --self-check |
| references | `.claude/references/arch-review-protocol.md` | G2 | T8 第 4 步指针新增句；消费方 = Track B 全批主线程 + 判官简报 |
| references | `.claude/references/track-b-verifier-brief.md` | G2 | T8 加固句（v0.2 收圈裁定收入）；消费方 = 核验官派遣 prompt（现读再粘纪律） |
| 模板 | `docs/templates/PROPOSAL.template.md` | G2 邻接 | T9 纪律句（manifest 说明 + 读法注第 4 条 + 自检句扩充，v0.2 合并设计裁定收入）；消费方 = 每批起草复制起点 |
| 账本 | `docs/crq/gov/` 五件 + INDEX 两件 | 账本面 | T10 结项三件（归位 `_archive/2026/`）+ 部分承载两件 + render 再生 |
| 未触 | `.claude/verify-gates.json` / `.claude/scope-map.json` / settings 族 / agent defs / `.claude/skills/**`（v0.2 T4 退役裁定裁 SKILL 挂接锚出）/ `docs/contracts/**` / G1 四件 | 未触 | 零 diff（B6 类申报时机械核对） |

**【硬项 · sweep 透镜义务】** 本批新增断言的判定 pattern 全仓预跑：① T5 存在性齿 pattern（fragment-stripped + ANCHORABLE 过滤 + existsSync，七类字段含 crossEnd.counterparts[].path）对 43 个包目录（含在飞 perf-k1、排除 current/ 指针副本——口径注见 §2 T5 证据段）694 条引用预跑 = 478 在场/87 路径形缺失（23 包）/129 非路径跳过（v0.3 按 R1-F1 勘正重跑数，命令与逐字段输出实录见 §2 T5 证据段，2026-09-24）——历史包噪声实证支持「零追溯 + 形状过滤」两设计约束；② T6 解析点 census = grep 机械枚举 18 件逐件计数（命令实录见 §2 T6 证据段），分类 14 待齿/2 已有齿/2 非解析；③ T9 目录 token pattern（`/$/` 结尾 union 成员）对现存批 PROPOSAL 面预跑：本批 v0.1 manifest 32 行全为文件路径、零目录 token（自查），260923 批实例（`docs/crq/`）为唯一在册病例。

**【硬项 · 复扫模式完备性先验】** 本批不涉退役语面/残留清扫（T4 canary 期望重写属夹具更新非语面清扫；docs/systems 退役残留面归 ARQ-260914-legacy-sxay 族、不在本批）。

## 4. human 取舍点（OQ）

**已裁（2026-09-24 同场四问，AskUserQuestion 回执 verbatim 记 Status 凭据段）**：收圈 = 「全收（Recommended）」；OQ-1 = 「(c) 可观测齿先行」；OQ-2 = 「(a) 批准向严（Recommended）」；OQ-3 = 「(b) 退役归档」（非推荐位）。以下各 OQ 选项原文保留作选项空间记录（含推荐位与理由标注），供判官核 OQ 给全性；失效分支已随 v0.2 在各 T 改法段标注。

**立项 ack + 收圈裁定（前端对齐，与 OQ 同场）**——为什么改：三类齿缺口全部执行测出（loop-retro 跨棒实证 + 批 A OQ 裁定明文留批 C + 本批八项实测复核），噪声与静默形态在批 B/批 C 会话内仍在现身；往哪改：四处小修 + warn 齿 + 回显齿 + 两件文面指针/纪律句，向严件全部单独 OQ；改动多大：G3 脚本 19 件（含 lib 新增 1）+ hooks 4 件 + G2 文面 2-4 件 + 账本 7 件，manifest 全量 32 行、按裁定裁剪。**〔v0.3 勘误注（判官 R1-F2(b)，本段系 v0.1 human 出示场历史记录、不静默改写）：「19 件」系起草笔误无可复现来源；权威值 = v0.2 manifest 31 行，其构成为 `.claude/scripts/` 面 17 件（15 改 + lib 新增 1 + canary 退役删除 1）+ hooks 4 件 + G2 文面 3 件（protocol + verifier-brief + template）+ 账本 7 件；「G2 文面 2-4 件」亦为裁定前条件式表述，裁定后定为 3 件〕****收圈子项**（逐项圈定）：① 候补 4（T8）收不收；② T8 可选加固（verifier-brief 同句）收不收；③ 候补 5（T9 处方①③）收不收；④ T9 合并设计三句（围栏承载/逐子句时点/自检第三句）并不并批；⑤「可搭任意批」余量复扫结果确认——全量 open 条目 trigger 复扫（2026-09-24，15 件 deferred/accepted-risk 逐条核）结论 = 其余在册件与本批触面**零自然交点**（known-red-count 残余③交点 = acceptance-evidence 文面 / portfolio-reconcile 交点 = portfolio-reconcile.mjs / legacy-sxay = 独立批量对齐批为宜 / ARQ-260918 候选②③涉 launch 形态未裁项 / versioned-guard 触发 = 触碰对应冻结件 / auto-mode-classifier 须双臂单开批），**建议一律不搭**；批 D 候补两件（planner-roster / codemem-doc）按任务书维持不入。

**OQ-1 · `--expect-change-id` 强制力度（T7 三态）**——背景：批 A OQ-3(a) 原裁（逐字，出处同 §2 T7）「本批文面纪律先跑一段，实测「文面成文后是否仍复现」再定机械齿的形态与力度」；起草期实扫 = 批 A 落地后仅 m4-k1 一包 5 次 dispatch（全落对包、零污染），但事件面不记旗标态——**零实例 = 零症状而非零漏带证明**，且观察期缺观察面。代价：(a) 零改动、观察期继续盲测；(b) 缺旗标即红 = 预防性向严，畸形调用（历史重放/手工补记）需补旗标，与「实测复现再定」原判据相悖；(c) payload 两 additive 键 = 零判定变更 + 补上审计面，代价是事件 schema 增量与「加牙与否」再等一个数据期。为什么该 human 定：闸级强制命中「新增/改门判据」敏感面、机械强制层不得 agent 自批（批 B 先例）；(c) 的量化触发器（absent ≥1 → 启动 (b) 批；30 全 match → settled）= 把下一次裁定的证据面钉死，也须 human 认可。**(a) 维持文面观察 / (b) 闸级强制缺旗标即红 / (c) 可观测齿先行 + 量化触发器（Recommended——忠于批 A「实测复现再定」原判据且修复其观察面缺口）**。

**OQ-2 · verify.mjs 未知 phase fail-loud（T2 向严批准）**——背景：静默回落仅存 legacy 路径（bearing 已 fail-loud），K7 实烧差点假绿；调用面实扫零存量依赖（canonical 模板只传合法值且恒 bearing）。代价：向严后畸形调用从「静默按 scoped 跑」变 exit 2 红——手工重放历史命令带错 phase 值者需改命令；缺省→scoped 的 fail-safe 不动，合法调用零影响。为什么该 human 定：判定行为向严（未知 phase 从绿变红）= 机械强制层变更不得 agent 自批。**(a) 批准向严（Recommended——零存量依赖实证在案、危险静默消灭）/ (b) 维持静默回落（条目 ② 保留待后续）**。

**OQ-3 · enforce-scope-canary 去留（T4 两选）**——背景：现跑 3/7、四条期望过期（授权面演进 + docs 退役），非闸集无主漂移；脚本用途 = 钉死 enforce-scope 钩子的 F1/F3 行为契约（大小写逃逸归一 + agent_type 硬门 + 主线程 carve-out）。代价：(a) 修期望 + 挂 launch-prep 步骤 2 = 每次 launch 多跑一次 ~2s 的 spawn 断言，钩子行为契约获得常驻回归网；(b) 退役 = 消灭漂移面，但 F1/F3 契约回归覆盖回到无主状态（enforce-scope 本体是否有等价 self-check 覆盖，退役前核一次、缺口如实记结项注）。为什么该 human 定：条目明文两选交裁；(a) 触碰 launch 流程文面（SKILL 步骤 2）、(b) 是不可逆删件（git 历史可考古但意图上属退役决策）。**(a) 修复期望 + 挂 launch-prep 复跑清单（Recommended——契约覆盖价值仍在、挂接成本低、settings-lint 先例同款）/ (b) 退役归档**。

**熔断预案**：评审 3 轮未 GREEN → human 三选一（转 Track A / 撤回·缩圈 / 换新判官实例再战 1 轮——仍不绿自动转 Track A）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> 时点纪律（批 B §4-5 细则）：每条标注评估时点；双时点子句逐条拆分归属。「反转实现必须变红」= 判别性义务（memory：fixtures-must-discriminate）。

**V0 · 起草期基线（已实跑，2026-09-24，全记录 verbatim 在 §2 证据段与本节）**：commit-safety --self-check 12/12 绿 · mutation-check --self-check 8/8 绿 · changes-index --self-check 29 例 fail 0 · brief-inputs --self-check 28/28 绿 · landing-fidelity --self-check 全套绿（verbatim-quotes 6/6、isFrozen 19/19 尾段实录）· settings-lint --self-check 6/6 绿（邻接基线）· canary 实跑 **3/7 = 红基线**（4 FAIL 输出 verbatim 在 T4 证据段）· preflight 目录 token 样例干跑 ok:true（T9 证据段）· T5 census 数字（43 包/694 条/478 在场/87 路径形缺失〔23 包〕/129 非路径；crossEnd 111 条全在盘——v0.3 勘正数，v0.1 曾误记 583/367，漏计 crossEnd，判官 R1-F1 抓出）· T6 枚举 18 件。断言：taskpackage-required-check / dispatch-stamp / verify.mjs 三件现无 --self-check（grep 0 命中实录）——T2/T5 新增分支的「新增面」属性由此坐实。【时点：起草期，已完成】

**V1 · T1 判别**：`node .claude/hooks/commit-safety.mjs --self-check` 全绿且输出含新增 statLine case 名（pass 数 > 12 基线）；判别性 = 新增 case 断言 add 相空态 → 新文案、commit 相空态 → 旧文案（对调实现必须变红）。live 佐证：landing commit 的 `git add` 触发相钩子输出含新文案、无「可能未生效」误导句。【self-check：pre-commit；live 佐证：landing 期如实记录】

**V2 · T2 判别（OQ-2=(a) 时）**：`node .claude/hooks/verify.mjs --self-check` 全绿（新增面，case 集 ≥6：缺省/scoped/final 放行 + full/裸旗标/garbage exit 2）；live：`node .claude/hooks/verify.mjs --phase=full` → exit 2 + stderr 含「未知 --phase 值」（校验先于一切派发 ⇒ 零副作用，T2 改法明文）；`--phase=scoped`/`--phase=final` 的 live 实跑**禁止**（legacy 全量跑闸写 current 所指包簿记 = 污染 dev 包，T2 证据段明文）——合法值覆盖由 self-check case 承担 + bearing 路径 normalizeExpected 既有判据同文（:114 verbatim 在案）。【全部 pre-commit 可验】

**V3 · T3 判别**：`node .claude/scripts/changes-index.mjs --help` → exit 0 + 用法输出含四分支；`node .claude/scripts/changes-index.mjs --bogus` → exit 2 + usage 错误（**不触发重建**：跑前跑后 `git diff --stat -- delivery/changes/` 为空）；`node .claude/scripts/mutation-check.mjs --help` → exit 0；`node .claude/scripts/mutation-check.mjs --change-id 260829-char-stroke` → 解析到该包 taskpackage.json（输出 taskPackage 路径字段含该 id）；`node .claude/scripts/mutation-check.mjs --bogus x` → exit 2；两脚本 --self-check 全绿且 pass 数超基线（8/8、29 例）。【全部 pre-commit；--change-id 用历史包 = 只读零副作用】

**V4 · T4 退役断言（OQ-3=(b) 已裁）**：`test -f .claude/scripts/enforce-scope-canary.mjs` → 非零（已删）；`grep -rln "enforce-scope-canary" .claude/ docs/ delivery/ AGENTS.md CLAUDE.md` → `.claude/` 下命中 **零**（V0 前置 census 实录：删除前 `.claude/` 唯一命中 = 脚本本体；docs/delivery 命中 = 账本结项注 + 历史制品 + 本批制品，逐面枚举留痕）；条目结项注 grep 断言含覆盖缺口记录关键词（`--self-check` 缺失 + F1/F3 零等价回归覆盖）；`git diff --stat -- .claude/hooks/enforce-scope.mjs` 为空（退役不触契约逻辑本体）。【全部 pre-commit】

**V5 · T5 判别**：三脚本 --self-check 全绿且含新增判别 case（taskpackage-required 新增面 ≥3 case：假 ref 产 warn / 在场 ref 零 warn / story-id 跳过；brief-inputs 28 基线 + missing-advisory case；mutation-check 8 基线 + missing-file warning case）；**闸行为不变断言**：选一历史包显式跑 `node .claude/scripts/taskpackage-required-check.mjs delivery/taskpackages/<id>/taskpackage.json` landing 前后 exit code 相同（只读、零副作用；warnings 键为增量）；census 对照 = 该包 warnings 数与 V0 census 该包缺失数一致（机械对账）。【self-check 与显式跑：pre-commit；「前后 exit 相同」的「后」子句：pre-commit（landing 后立即跑、commit 前）】

**V6 · T6 判别**：`node .claude/scripts/lib/current-pointer-shared.mjs --self-check` 全绿（新增面）；live 回显实证 = gov worktree 裸跑 `node .claude/scripts/taskpackage-required-check.mjs`（只读安全）→ stderr 含「TaskPackage 上下文解析：changeId=260924-perf-k1-roundtrip-latency（来源：current/ 指针）」——**指错包现场报警活证**（V0 census 实录 current/ 指向）；接线完备性 = grep 断言 14 件全部 import current-pointer-shared（机械枚举对账，零命中容忍 = 0）；既有 self-check 全家回归绿（mutation-check/brief-inputs/changes-index/commit-safety/landing-fidelity/settings-lint 六件，V0 基线对照）。verify.mjs/supervisor 两件钩子的回显由 self-check case 或 spawn 断言覆盖（live 跑 = 写簿记污染，禁止，同 V2 理由）。【全部 pre-commit；grep 对账命令与输出留痕】

**V7 · T7 判别（OQ-1=(c) 已裁）**：temp fixture 场（`CLAUDE_PROJECT_DIR` 指向 mkdtemp 夹具目录 + 夹具包，零仓内写入）live 调 dispatch-stamp 带 / 不带 `--expect-change-id` 各一次 → 事件 JSON 含 `contextSource` 与 `expectCheck` 两键、值分别为 match / absent（判别性：不传旗标必须产 `absent` **值**而非缺键；反转实现必须变红）；带旗标时事件既有字段与改前同参数运行逐字段一致（additive 断言：对照输出 diff 仅新增两键）；消费面零影响断言 = `node .claude/scripts/pod-events-aggregate.mjs` 对夹具事件跑通、输出与改前对同夹具一致；`git diff --stat -- .claude/agents/planner.md` 为空（批 A 模板已落、零回改）。【全部 pre-commit（夹具场内）】

**V8 · T8 判别（收圈收入时）**：grep 断言 protocol 第 4 步行含模板路径指针 + 「复制勿手写」关键词（替换后完整句与 FROZEN §2 逐字一致——byte 对账，批 B 教训 6 形态）；`node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>` 复跑仍绿（指针句不触解析面）；可选加固收入时 track-b-verifier-brief grep 同句断言。【pre-commit】

**V9 · T9 判别（收圈收入时）**：仓外临时样例两枚（含目录 token / 纯文件 token）跑 preflight → 前者 `advisories` 长度 ≥1 且 `ok:true` exit 0 不变、后者 `advisories` 空（**反转实现必须变红**：若 advisory 误入 findings 则前者 ok:false = 判别信号）；landing-fidelity --self-check 全绿含新 case；模板三句（或单句，按收圈）grep 断言逐句在场；**承重断言**：本批 PROPOSAL 自身 preflight 复跑仍绿（模板改动不回咬本批）。【pre-commit】

**V10 · 账本与状态机**：`node .claude/scripts/crq-render.mjs` 输出与结项裁定一致（结项件出现在归位宣告行）；render 复跑幂等——**post-commit 子句**：commit 后 `node .claude/scripts/crq-render.mjs && git diff --exit-code -- docs/crq/` → exit 0（工作树对 index 无 diff；批 B A2 教训形态，pre-commit 恒假不入 pre 集）；**pre-commit 等效子句**：landing 后 commit 前 render 复跑输出五 INDEX 全「未变」；`node .claude/scripts/crq-doctor.mjs` RED 零命中；`node .claude/scripts/changes-index.mjs --upsert 260924-gov-review-gate-parse-teeth` 于 FROZEN/landed 状态转移点各跑一次、`delivery/changes/arch/INDEX.md` 行状态随之翻。【时点逐子句标注如上】

**V11 · 结构门与工作树申报**：landing 后 `node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/260924-gov-review-gate-parse-teeth` → 绿（manifest ↔ 改动集对账，条件锚已按裁断在 v0.2 裁剪）；B6 类工作树全量申报 = `git status --porcelain` 全量粘贴逐行注性质（批 B §4-4 自律件形态：机械枚举不手写清单），过程制品 `delivery/changes/arch/INDEX.md` 与常驻脏 `.codebase-memory/` 两件如实认列。【pre-commit（申报）+ landing 后（结构门）】

## 6. 留根清单

1. **ARQ-260914 §4-1「承重引文 byte 级比对脚本化」机械化候选不在本批**——同根已四实例、够格机械化，但本批面已大（manifest 32 行）不扩；候选留 ARQ-260914 追注 (2)，后续批承接（起草侧 + landing 侧双钩形态设计已在追注内）。
2. **dir-anchor 处方②（landing 门目录前缀展开）维持不预批**——触承重闸判定逻辑，若做须独立批完整评审（条目自注 + T9 改法明文双重钉）。
3. **T7=(c) 量化触发器（已裁，现役）**——`expectCheck:'absent'` 出现 ≥1 实例 → 即启动 (b) 闸级强制批（§2 T7 失效记录段 (b) 为该批起草起点，设计已给全）；累计 30 次 dispatch 全 `match` → 文面强制 settled、`ARQ-260913-current-pointer-implicit-global` 残余② 按「观察期完结」结项。触发器原文随 T10 逐字写入条目追注（不依赖记忆）。
4. **批 B 留根 6（headless launch 恢复）仍归 human**——本批 launch 一律交互式；T2 live 验证的副作用禁区（valid phase 裸跑污染 current 所指包簿记）在 headless 恢复评估时须一并复核（自动化跑闸的 phase 传参面）。
5. **gov worktree current/ 残留观察项（登记不硬治）**——census 实录 gov worktree current/ 指 dev 包 perf-k1：属 main→gov 回同步 merge 带入的投影漂移（merge-driver 投影冲突 theirs 特判面在 dev 通道，gov 通道无等价齿；v0.3 按 R1-N2 勘正措辞）；T6 回显齿落地后该状态在裸跑输出中可见（报警齿恰是其可见性答案）；是否给 gov worktree 加 current/ 清理纪律 → 观察一批再议，不入本批。
6. **canary 退役覆盖缺口为已知处置（v0.2）**——OQ-3=(b) 系在选项说明中如实呈报缺口后所裁（F1/F3 契约零等价回归覆盖；enforce-scope 本体无 --self-check，起草期实测在案 T4 证据段）；缺口逐字入条目结项注。若判官认为缺口记录不足为打回项；未来若需回归覆盖，路径 = 给 enforce-scope 本体补 --self-check（另批承接，本批不重开——human 已明知代价择退役）。
7. **mutation-check:111 静默跳过系起草期新发现**——已并入 T5 ③（同根不另立条目，复证追注在案）；若判官认为超 ③ 处方面（条目原文点名两脚本、mutation-check 是第三落点），按打回项裁：缩圈出批则登记新条目走 gov-arq。
8. **changes-index unknown-flag 拒绝为用法面轻度向严**——超出条目 ③ 处方字面（「补 help + 传参对齐」）半步；判官若认为超「零门判定语义变更」自述，降格为仅补 --help（unknown flag 行为保留原样）——两形态均满足 rma8 病灶（想看不触发重建）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-09-25 - architect（Track B 评审循环，全面审）

**亲跑复核命令清单与关键输出**（全部本轮实测，非采信申报；关键节选）：

- **canary 实跑**：`node .claude/scripts/enforce-scope-canary.mjs` → **3/7、4 FAIL**（4 条 FAIL 行逐字 == §2 T4 证据段围栏；stderr 含「docs/ 下未登记的子目录 docs/systems/」）✓ 红基线复现。
- **preflight 对本批**：`node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>` → `ok:true / anchorCount 31 / findings:[]`；anchorUnion 31 成员与 landing-manifest 31 行**独立对账**：onlyUnion=[]、onlyManifest=[]——结构字段在场核对 + 轮次焦点②（manifest ≡ §2 锚并集集合恒等）独立复核成立 ✓。
- **preflight 目录 token 干跑**（仓外临时样例，锚行含 `docs/crq/` + `.claude/scripts/landing-fidelity-check.mjs`）：`ok:true`、exit 0、`anchorUnion` 收 `docs/crq/`——T9 现行为（目录 token 静默通过）复现 ✓。
- **六脚本 self-check 基线**：commit-safety 12/12 · mutation-check 8/8 · changes-index 29 例 fail 0 · brief-inputs 28/28 · settings-lint 6/6 · landing-fidelity 套件绿（verbatim-quotes 6/6、isFrozen 19/19 尾段实录）——与 §5 V0 申报全对 ✓。
- **承重围栏引文逐字比对**（Read/grep 亲核磁盘）：T1 commit-safety :66（含 4 空格缩进）✓ · T2 verify.mjs :83-85 ✓ · T3 mutation-check :13-15 ✓ · T5 mutation-check :111 ✓ · T9 landing-fidelity :613 ✓——五处承重围栏引文 byte 级全同。
- **file:line 逐点亲核**（全部命中、无一扑空）：verify.mjs :79-81（缺省→scoped 成文设计注）/:83-85/:112-115/:123（persistVerifyRuns current 解析）/:135/:160-170（notExecuted 三信号）/:194-195（boundedText）/:210（liftNotExecuted(stdout) 只吃 stdout）/:378-382（尾部派发段，接线点可行）+ parseArgs :35-53（裸旗标 → `args[key]=true`，T2 裸旗标判非法设计成立）✓；verifier-evidence-check :114（normalizeExpected throw）/:260（mismatch throw）✓；changes-index :9-13（用法块）/:528-544（四分支，else 直落 main() 在 :542-543）+ help 零命中 ✓；mutation-check :89/:111/:136-137（「self-check 分支先于早退」结构注）✓；verify-gates.json :59（mutation-check 闸裸跑）✓；taskpackage-required-check existsSync 仅 :121/:145/:390 三处 + 头注 :15（零追溯）+ **self-check 零命中**（新增面坐实）✓；brief-inputs :125-127（missing 行）/:131（bare-anchor 专属 advisory）/:254/:318（census 排除 current）/:543（self-check 夹具，非解析点）✓；governance-gate-shared :40（ANCHORABLE_TEST 前缀正则）✓；landing-fidelity :613/:615（changed.has 精确匹配）+ preflightFindings/runPreflight :227-266 ✓；enforce-scope :174（agent_type）/:178-180（大小写归一注）+ 无 --self-check（grep 零命中 + 实跑无输出 exit 0）✓；scope-map architect allow 含 `.claude/agents/**`、review-loop 八 def 另列 deny ✓；dispatch-stamp :69-81/:215（始终回显）/:217-219（可选硬校验自述）/:220-230/:222-225/:232-235（no-context → exit 0）/:253-256（唯一分桶键注，内引 loop-metrics:242 / pod-events-aggregate:76）/:296-311（payload 无旗标态/来源键；additive 先例 model/modelSource/mainThreadUsage :307-309）/:317-334（pre-cutoff legacy）+ EVENTS_SCHEMA_CUTOFF='2026-07-25'（:91）✓；acceptance-id-shared isMain 守卫 ✓；planner.md:66（双必带）/:71（scoped|final 模板）+ integrator.md:38（final）✓；protocol :55（PROPOSAL 模板指针）/:58（第 4 步无模板指针——缺位形态确认）/:60（retro 模板指针）✓；PROPOSAL.template :23（manifest 说明行）+ 读法注 1–3 条（第 4 条为新增）+ 起草期自检两句 ✓；architect-landing-summary.template :6（unit-gate F4 教训自注）✓；track-b-verifier-brief 六项核验清单在场（T8 加固句落点可达）✓。
- **T6 census 独立重跑**：grep `taskpackages[/'].*current` 全 `.claude/scripts`+`.claude/hooks` → **18 件**（hooks 3 + scripts 15）；14 件待齿行号逐件全对（acceptance-coverage:25 / closeout:51 / devloop-artifact:42 / docs-sync:22 / journal-append:51 / loop-metrics:64 / mutation-check:17 / pod-events-aggregate:29 / pod-events-append:99 / security-check:13 / taskpackage-required:29 / verify:123 / supervisor-gate-check:618 / supervisor-verdict-gate:468）；放宽口径（`'current'`/`/current/`）文件集与 18 件**完全一致**——census 无漏点 ✓；已有齿 2 件等价性成立（dispatch-stamp :215 始终回显 = T6 所推广纪律本源；verifier-evidence-check :260 mismatch throw 硬于回显齿）；非解析 2 件排除理由成立（brief-inputs = census 排除 + 夹具；merge-driver = :836-837 投影冲突特判面 + :796-797 白名单守卫，:1093-1095/:1181 均 self-check 夹具串）；「18 件全部 260918 批前即存在」spot-check 4 件首提日期均 2026-08-29 ✓。
- **T5 census 独立重跑**：43 包 ✓ / 缺失 87（23 包）✓ / 非路径 129 ✓ **全部复现**；总数与在场数**不复现**（见 F1）。
- **T4 退役前置核**：活引用 census `grep -rln "enforce-scope-canary"` → **10 命中** = 脚本本体 1 + 账本 1 + 本批 PROPOSAL 1 + 历史制品 7（spec-split PROPOSAL / 批 A PROPOSAL / m2-k2 pod-retro / review-packet / pod-events / taskpackage / loop-retro），零 SKILL/agents/hooks/settings 接线 ✓；条目处方原文两选（「挂进某复跑清单**或声明退役归档**」）在场 ✓。
- **T7 取证复核**：m4-k1 = `260924-m4-k1-story-cloud-link`，5 条 dispatch-completed（12:38:34 / 13:19:12 / 13:45:45 / 14:01:43 / 14:24:43 UTC，跨 main + m1-k1a 两 worktree 双份）✓；m1-k1a worktree rebase finish = 19:38:09 +0800（= 11:38 UTC）✓；批 A retro commit `cf9db09` = 18:16:13 +0800（= 10:16 UTC）✓；申报「953 事件」与我现扫 958 原始行差 = m4-k1 事件双份复制（5 条 × 2 worktree）之时点漂移、差异精确可解释，「零污染症状 ≠ 旗标恒带证明」论证的证据链完好 ✓。消费面安全亲核：pod-events-append payload 仅要求 object（无闭集键校验）；dispatch-completed 消费方（loop-metrics :234→:242 只分桶 status、pod-events-aggregate :60/:119/:165/:186 只读 tokenUsage 数值子键）均不触新增键——additive 安全论证成立，焦点⑤通过 ✓。
- **T2 调用面实扫**：`--phase` 全 canonical 命中 = planner.md:66（dispatch-stamp 自由标签）/ :71（scoped|final，均合法值）/ :76（verifier-evidence-check bearing 路径，:114 已 fail-loud）/ integrator.md:38（final）——verify.mjs 的 --phase 调用模板仅 :71 与 :38、均 bearing、均合法 ⇒ 「无存量调用方依赖静默回落」确认 ✓。
- **闸集成员核**：taskpackage-required / mutation-check / acceptance-coverage / docs-sync / security 在 verify-gates 闸集；changes-index、enforce-scope-canary 不在 ✓——§2 各「闸/非闸」自述全对。
- **零脚手架核**：`git status --porcelain` = 五件 ARQ 追注 M + `delivery/changes/arch/INDEX.md` M（+1 行 draft 状态行）+ 本批目录 untracked + `.codebase-memory` 常驻脏 ×2——**`.claude/**` 与 `docs/templates/**` 零写入实测成立**（焦点⑧核心主张过；枚举措辞见 N1）。

**Eligibility 独立核**（作者自评不作数）：不触 `docs/contracts/**`（manifest + grep 双零）✓；非跨系统结构性变更 ✓；Track B 快道资格成立——三类病灶全部执行实测在案（loop-retro-2026-09-23 T6/T2/T7 跨棒证据 + 批 A §4 裁定记录「OQ-3 = (a) 全部留根批 C」明文 + 本批起草期八项实测——canary 3/7、preflight 目录 token、census、事件扫描、六脚本基线均被我独立复现）；**向严面 human 批准凭据形态 = 批 B 先例同款**（OQ-1/OQ-2/OQ-3 裁定原文记 Status 凭据段；OQ-2=(a) 的「未知 --phase 值 fail-loud」措辞覆盖 T2 全部向严分支——非法值、`--phase=full`、裸旗标 true 均属「未知/非法值」类，未越裁定语）✓；T4 退役 = 非推荐位、选项文本明示 F1/F3 覆盖缺口与「退役前核一次、缺口如实记结项注」义务、human 择 (b) 凭据段如实记「非推荐位」——焦点④凭据完备性通过 ✓；T5/T6/T7(c)/T9① 判定语义零变更（advisory 键 / stderr 回显 / additive 事件键 + V5/V6/V7 闸行为不变断言在案）✓。未发现真设计分叉或未揭示爆炸半径。**Track B 资格成立、零升级 human。**

**判官应裁项裁定**（PROPOSAL 留判官裁的三项）：

- **T9① advisories 入不入 findings**：裁 = **维持独立 `advisories[]` 键（ok/exit 零变更）**——入 findings 即 preflight 门判定向严，无 OQ 批准且超条目处方①「不动承重门判定」；现设计正确。
- **留根 8（changes-index unknown-flag 拒绝半步）**：裁 = **维持**——changes-index 非闸集（INDEX 幂等投影工具，全量重建是再生副作用非判定），unknown-flag 红 = 正打 rma8 病灶（想看用法触发重建），两形态均满足病灶、且 PROPOSAL 已透明标注并给降格选项；不属机械强制层向严。
- **留根 7（mutation-check:111 并入 T5③）**：裁 = **不超处方、维持并批**——ARQ-260923-portfolio-decomposer-drift 批 C 复证追注已明文同根判定与「不另立条目」（账本登记义务闭合），落点为同一 warn 机制（advisory 零判定变更）。

**Findings**：

- **F1（BLOCKING）T5 census 总数/在场数按文内映射不可复现**：§2 T5 证据段、§3 sweep 透镜①、§5 V0 三处引用 43 包 / 583 条 / 367 在场 / 87 缺失（23 包）/ 129 非路径。判官独立重跑（同规则：七类字段 = docRefs/sourceRefs/contractRefs/planningRefs/acceptanceTestsFile/origin.ref(upstreamRef)/crossEnd.counterparts[].path → fragment 剥离 → ANCHORABLE_TEST → existsSync）：**43 包 ✓ / 87 缺失（23 包）✓ / 129 非路径 ✓，但总数 = 694、在场 = 478**——差额 111 恰等于 crossEnd.counterparts[].path 条目数（111 条、全部 ANCHORABLE、全部在盘、零缺失零非路径；结构实样：260829-hardware-validation → `server/gateway/front/front_app.py` 类对象含 `path` 键）。即：文内字段映射含 crossEnd，但所录输出从未计及它。影响：§5 V0 把「43/583/367/87/129/23」钉为冻结基线，landing 后复跑将得 694/478——冻结基线不可复现，违本批 §2 硬项纪律（「数字引用均现扫核对」+ census 输出实录 verbatim）——恰是本批主张要治的证词失真类。修法方向（机械、不动设计）：按显式映射重跑 census，三处同步改数——either 694/478（census 改为真含 crossEnd），or T5 证据段明文 crossEnd 排除及其理由 + 字段清单描述相应改六类；87/129/23/43 不动（已复现）；V5 的逐包对照子句（逐包 warnings 数 vs census 逐包缺失数）不受影响。不影响目标效果、无须升级 human。
- **F2（SHOULD）冻结文本内部三处计数/措辞与 manifest（31 行权威值）矛盾**：(a) review profile 行「15 件脚本〔含新增 lib 1 件〕」——manifest 脚本面实为 17 件（15 件修改中既有 + lib 新增 1 + canary 删除 1），「含」字面读法少 2（可改「17 件 = 15 改 + 1 新增 + 1 删除」）；(b) §4 立项 ack 段「G3 脚本 19 件（含新增 lib 1）+ hooks 4 件」——manifest 脚本 17 件，19 无可复现来源（疑并入 2 件 G2 references 计数）；§4 是 human 出示场历史记录，建议加一句如实勘误注、不静默改写；(c) riskTier 证成「触 hooks 面 2 件」——manifest hooks 面 4 件（commit-safety / verify / supervisor-gate-check / supervisor-verdict-gate），若意为「主改动 2 件」须写出限定语。三处均不影响 manifest 权威值与 §3 sweep 表逐行枚举（逐行枚举与 manifest 对账零差），但 profile / riskTier 枚举是判官与核验官消费的结构性主张，FROZEN 前应消解内部矛盾。不影响目标效果、无须升级 human。
- **N1（NIT）零脚手架声明「两件生成 INDEX」措辞与工作树证据含混**：git status 仅 1 件 INDEX M（`delivery/changes/arch/INDEX.md`）；若指 crq-render 幂等再生两件 crq INDEX（内容零 diff），应写明是哪两件 + 零 diff 事实——写面逐件可验（批 A R1-F4 / 批 B R1-F3 申报 vs 实况先例形态）。
- **N2（NIT）T6 对 merge-driver 的措辞**：「merge ours 策略面」与代码实况（:830-833 注释「投影冲突 --theirs 特判（rebase 语境 theirs = 被重放 Pod 侧，恒为被合并包赢）」）术语不符，建议改「投影冲突 theirs 特判面」；分类结论（非解析消费）不变，判官亲核成立。
- **N3（NIT）T7/§4 对批 A OQ-3(a) 的引文为压缩转述而带引号呈现**：批 A §4 原文 = 「让本批文面纪律先跑一段，实测『成文后是否仍复现』再定机械齿的形态与力度」；PROPOSAL 引作「文面先跑一段、实测复现再定机械齿力度」非逐字——去掉引号改转述、或换逐字引文（本批自身硬项纪律对引文逐字性的要求）。

**随行提醒（非 finding）**：V6 live 回显断言钉死 changeId=260924-perf-k1-roundtrip-latency——current/ 属残留投影漂移（留根 5 在案），landing 前若经 main→gov 回同步再漂移，该断言会因夹具漂移而非缺陷变红（fail-loud 方向安全）；执行者遇红先复读 current/ 判因。

**结构字段核对（随行义务）**：Status 行 / packet-binding 行 / landing-manifest 围栏块（31 行）三件在场；manifest ≡ §2 锚并集（preflight 机证 onlyUnion=[] onlyManifest=[]）✓。

verdict: BLOCK(F1,F2)

**结论**：F1（BLOCKING）须 FROZEN 前修复——census 按显式映射重跑改数（或明文 crossEnd 排除理由），三处引用 + V0 基线同步；F2（SHOULD）三处措辞对齐 manifest 权威值（§4 加勘误注）。两项均为机械修复、修法逐条在案；**零项影响目标效果、零升级 human**。N1–N3 随 F 项修订顺带处置或如实保留。R2 复核范围 = F1 三处改数点 + V0 基线同步 + F2 三处措辞 + 回归确认（承重围栏引文重跑 byte 比对 + preflight 复跑 + 未引入新偏差）。

### R2 - 2026-09-25 - architect（Track B 评审循环，同判官 resume）

**复核对象**：DRAFT v0.3（R1 findings F1/F2 + N1–N3 修订）+ 2026-09-25 v0.3 补遗（T5 census 口径归因）。

**亲跑复核命令清单与关键输出**（全部本轮独立实测）：

- **F1 census 排除 current 口径逐字段重跑**（判官独立脚本，七类字段 + fragment 剥离 + ANCHORABLE_TEST + existsSync，glob 排除 `current/`）：docRefs 63 在场/32 缺失/34 非路径 · sourceRefs 192/19/47 · contractRefs 43/1/0 · planningRefs 21/1/23 · acceptanceTestsFile 30/34/0 · origin.upstreamRef 18/0/25 · crossEnd 111/0/0；**TOTAL 478 在场 / 87 缺失（23 包）/ 129 非路径，43 个包目录**——与 §2 T5 申报逐字段明细**逐位一致** ✓。
- **current/ 指针副本归因实证**：`delivery/taskpackages/current/taskpackage.json`（changeId = 260924-perf-k1-roundtrip-latency）七类字段恰 **12 条、12 在盘、零缺失零非路径**——判官 R2 中间复现的 706/490 差额 12/12 全落「在场」桶，与主线程双口径对照逐位吻合 ✓。**口径接受性裁定（coordinator 委判）**：**接受「排除 current/ 指针副本」口径**——current/ 是指针副本非包目录，其 canonical 本体（`260924-perf-k1-roundtrip-latency/`）已在 43 目录集内，计入即同包双计；排除理由机械成立，不构成新 finding。「43 个历史包」→「43 个包目录（含在飞 perf-k1）」勘正如实（perf-k1 系在飞包）。
- **F1 三处同步核**：§2 T5 证据段（含口径注 + 逐字段明细 + crossEnd 全在盘注）✓ · §3 透镜①（「43 个包目录（含在飞 perf-k1、排除 current/ 指针副本——口径注见 §2 T5 证据段）」）✓ · §5 V0（694/478 + v0.1 误记勘正注）✓ · 版本沿革 v0.3 补遗（归因实录）✓。旧数 583/367 残留仅三处合法历史提及（版本沿革勘正文 / V0 勘正注 / §7 R1 判官原文），grep 逐处核对 ✓。
- **F2 三处核**：review profile「17 件〔= 修改 15（含 landing-fidelity-check.mjs）+ 新增 lib 1 + 退役删除 enforce-scope-canary.mjs 1〕」✓（判官重数 manifest 脚本面 17 件吻合）；§4 呈报句勘误注（原文未静默改写 + 权威值注「17 + hooks 4 + G2 3 + 账本 7 = 31」+「G2 文面 2-4 件」条件式表述裁定后定值说明）✓；riskTier「触 hooks 面 4 件」✓。
- **N2 核**：T6 证据段「投影冲突 **--theirs 特判**白名单 `PROJECTION_CONFLICT_FILES`〔rebase 语境 theirs = 被重放的 Pod 侧，恒为被合并包赢〕」+ 勘正注 ✓；留根 5「投影冲突 theirs 特判面」✓；行号 :831-837 与磁盘实测（注释块 :831 起 + 数组 :835-838 区域）相符 ✓。
- **N3 核 + 判官自我勘正**：`grep -o "本批文面纪律先跑一段，实测「文面成文后是否仍复现」再定机械齿的形态与力度" <批 A PROPOSAL>` → **命中**（exit 0）——v0.3 两处引文（T7 问题段 + §4 OQ-1 背景段）与批 A §4 OQ-3 选项 (a) 理由段**逐字一致** ✓；T7 根因句转述已去引号 ✓。**判官自我勘正（诚实记录）**：R1-N3 所引「原文」首字「让」系判官侧误记——批 A 原文实为「本批文面纪律先跑一段…」（本轮 grep -o 亲证，`让本批…` 形态零命中）；R1-N3 的 finding 实质（引文须逐字）成立且已被正确修复，但 R1 记录中给出的「原文」本身不逐字，特此勘正。
- **回归**：`node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>` → `ok:true / anchorCount 31 / findings:[]`，anchorUnion 31 ≡ landing-manifest 31（onlyUnion=[] onlyManifest=[]，判官独立复跑）✓；R1 记录 append-only 保全（未改一字）✓。
- **N1 修复核查 → 发现 F3**：`git show --stat 2fce4b6` → 该 commit 文件集 = retro.md（+1）+ ARQ-260914 追注（6 行）+ **`docs/crq/gov/INDEX.md`（2 行）——仅一件 INDEX**；`docs/crq/gov/_archive/2026/INDEX.md` 不在该 commit。

**Findings**：

- **F3（SHOULD，R2 新增——N1 修复引入的申报不实）**：零脚手架声明（:11）现文「`docs/crq/gov/` 侧两件 INDEX 已随前置 commit `2fce4b6` 载体兑现 render 收讫、起草期零 diff」——`git show --stat 2fce4b6` 实况该 commit 只含**一件** INDEX（`docs/crq/gov/INDEX.md`）；`_archive/2026/INDEX.md` 不在 commit 内（若经 render 幂等再生则内容零 diff、git 无记录，但「随 commit 收讫」的字面归属不实）。R1-N1 的修复要求正是「写面逐件可验」，现文对第二件 INDEX 仍不可验且与 commit 实况不符——零脚手架声明是本批最敏感的诚实声明面（B6 类申报的冻结基准），冻结件不应携带可机械证伪的申报。修法（一句话）：改为「`docs/crq/gov/INDEX.md` 一件随前置 commit `2fce4b6` 收讫（git show 可验）；`_archive/2026/INDEX.md` 起草期零 diff 维持（该 commit 未触）」。不影响目标效果、无须升级 human。
- **N4（NIT · 随行提醒）**：Status 行（:3）仍「DRAFT v0.2 · …待进 architect 评审循环」，版本沿革已至 v0.3 补遗——同文件版本号不同步。FROZEN 转移时 Status 行必然重写，届时须**直接标 FROZEN v0.3**（沿版本沿革现值；机锚形态 `FROZEN v<大>.<小>`，先例教训在案）并在凭据段补 architect GREEN 轮次——不单独打回，随 F3 修订顺带处置。

**结论**：F1/F2/N1/N2/N3 修订全部到位（census 逐位复现、口径归因机械成立且判官接受、三处计数对齐、引文逐字），v0.3 补遗如实；**新增 F3 一项**——N1 修复引入的「两件 INDEX 随 2fce4b6 收讫」申报与 commit 实况不符，须 FROZEN 前改为逐件可验事实句。R3 复核范围 = F3 一处措辞 + Status 行 FROZEN 形态（N4 随行项）+ 回归（preflight 复跑 + 修订处 diff 核对），预计单点收敛。

verdict: BLOCK(F3)

### R3 - 2026-09-25 - architect（Track B 评审循环，同判官 resume · R2 交接口径单点复核）

**复核对象**：DRAFT v0.4（R2 finding F3 + 随行项 N4 修订）。

**亲跑复核**（全部本轮独立实测）：

- **F3 ✓**：零脚手架声明（:11）现文 = 「`docs/crq/gov/INDEX.md` 一件随前置 commit `2fce4b6` 收讫〔commit stat 可验〕；`docs/crq/gov/_archive/2026/INDEX.md` 起草期全程零 diff〔历次 render 输出「未变」实录〕、无收讫事项——v0.4 按 R2-F3 勘正：v0.3 曾误书「两件随 commit 收讫」，该 commit 实含一件」——逐件可验形态达成，且与判官实测双向吻合：`git show --stat 2fce4b6` = 该 commit 仅含 `docs/crq/gov/INDEX.md`（2 行）一件 ✓；`git status --porcelain -- docs/crq/gov/_archive/2026/INDEX.md` = 空（工作树 clean，「起草期全程零 diff」有 git 实况佐证）✓；勘正注如实（v0.3 误书原文保留于版本沿革历史段、v0.4 段明文勘正 = 合法历史提及形态）✓。
- **N4 ✓**：Status 行（:3）= 「DRAFT v0.4 · 2026-09-25 · R1/R2 两轮评审在案（BLOCK(F1,F2)→BLOCK(F3)，三轮内），待 R3 复核」——版本号与版本沿革同步、轮次与 verdict 线如实；版本沿革 v0.4 行（F3 勘正 + N4 处置记录）在场 ✓。
- **回归 ✓**：`node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL>` → `ok:true / anchorCount 31 / findings:[]`，anchorUnion 31 ≡ landing-manifest 31（onlyUnion=[] onlyManifest=[]，判官独立复跑）✓；旧措辞「两件 INDEX 随 commit 收讫」残留 grep 逐处核对 = 版本沿革 v0.3 历史段（合法历史提及）+ T10「两件生成 INDEX 重渲染」（landing 相位 render 再生义务、与起草期写面申报无关）+ §7 判官 R1/R2 原文（append-only 保全，未改一字）——无未勘正的现役残留 ✓。
- **无新矛盾**：v0.4 修订仅触 :3（Status 行）、:4（版本沿革 v0.4 行）、:11（零脚手架写面句）三处，均与 git 实况 / 评审记录一致；§2/§5/manifest 零变动（preflight 机证 manifest 31 行不变）。

**结构字段核对（GREEN 结论随行义务）**：Status 行 / packet-binding 行 / landing-manifest 围栏块（31 行）三件在场；manifest ≡ §2 锚并集（preflight 机证）✓。

**结论**：R2 交接口径全部落实——F3 措辞改逐件可验且双向实测吻合，N4 Status 行同步，回归零新偏差。R1–R3 三轮收敛（未触熔断），全批承重证据链经三轮独立亲核（census 逐位复现、五处围栏引文 byte 级一致、T6 18 件 census 逐行号核对、向严件 human 批准凭据形态成立、零脚手架核心主张 git 实证）。**无任何项须升级 human。**

verdict: GREEN

**随行提醒（非 finding）**：① 标 FROZEN 时 Status 行机锚形态须为 **`FROZEN v0.4`**（`FROZEN v<大>.<小>` 样式，勿写「FROZEN（v0.4）」——升级闸 isFrozen 判读形态，先例教训在案），并在凭据段补 architect R3 GREEN 轮次；② landing 相位按 §5 V1–V11 与 T10 顺序执行，landing-verifier 比对基准 = 本 v0.4 §2；③ R1 判官记录中 N3 所引批 A「原文」首字「让」系判官侧误记（R2 已自我勘正在案）——landing-verifier 对 T7 引文做 byte 对账时以批 A 磁盘原文为准，勿以 R1 记录内引文为基准。
