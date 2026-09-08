# PROPOSAL · 260908-gov-review-merge-chain-hardening

> **状态：FROZEN v1.0 · 2026-09-08 · human 冻结确认（「冻结，landing吧」指令；含「补五条」计数合成拍板）。定稿回灌：architect 按 `decision-packet.md` §2.1 十六处回灌点机械回灌 CONFIRMED 改法与 human 拍板（2026-09-08 九项，全按裁决建议批准，见 §4 终裁记录），不夹带新设计；冻结确认归 human，本稿不自标冻结标记。回灌作业记录见 §9。前态 DRAFT v0.2：human 立项 ack 已获（AskUserQuestion 四裁：ack 进评审 / OQ1=并批走 Track A / OQ2=随批 / OQ4=merge 批后即启 escalation Track A）；review 轨按 OQ1 裁断升 Track A，入口 `/arch-change`（review-moderator 编排），评审已收口。**
> **承载 ARQ**：`ARQ-260908-pod-integration-no-legitimate-executor`（主承载，面 A 全量 + 面 B 核验行）+ `ARQ-260908-versioned-artifact-slot-collision`（主承载）+ **`ARQ-260908-versioned-guard-version-literal-coupling`（OQ1 裁并批入内，T7 规约面）** + `ARQ-260907-gate-lib-script-separation`（OQ2 裁随批，T6 确认在批）。**销账簿记（不在本批改法内）**：`ARQ-260907-merge-closeout-bookkeeping-gap` 与 `ARQ-260907-merge-driver-approval-state-desync` 经 2026-09-08 主线程核销确认已被 `260907-gov-review-merge-friction` 批完整落地（merge-driver.mjs T1 `approvalRefUnlock` :915 / T2 Step 0 收口 :777 / T3 投影特判 :828 / T4 `progress.md merge=union`；pod-coordination.md §10 runbook + 簿记口径；五自检 T1/T2/T3/T4/T6 在册），队列 §1 两行属销账滞后，本批随 landing 结移 §2。**明确不入本批**：`ARQ-260907-escalation-cross-system-overfire`（自评 Track A full-adversarial + human，升级闸判据语义变更，入口 `/arch-change`，见 §4 OQ4——human 裁本批 ack 后即启、另行立项）；非 merge 的三条新增闸面 ARQ（tighten-only-unwritten / gate-pytest-import-mode-incompat / known-red-count-no-mechanical-source）留下一批治理批，本批不夹带。
> **review 轨**：**Track A full-adversarial + human（OQ1 human 2026-09-08 裁并批升级；原自评 Track B 作废）**。评审已收口（2026-09-08 adjudicator）：27 条 finding/A 项终态 = CONFIRMED 22 / DISMISSED 4 / DEADLOCK 0，governance pass-oracle 六关全 GREEN（`review-packet.md` §4）；逐条裁定见 `adjudications.md`，裁断正文与 human 拍板清单见 `decision-packet.md`。两处判据适用存疑点已裁：① integrator.md 新节 = agent def 执行语义变更，不触 lightweight 硬排除判据字面、恒 human-approved（A-1，先例四条件归档，human 已批——拍板清单 #2）；② T5 闸内新核验行 = 既有前置升级闸的「门内加项」，非新增 merge gate、非第 7 工程门（A-2）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T7）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（维持——decision-packet §三复核：确认后实际承载面比初稿更宽，但全部 CONFIRMED 改法方向均为收紧或如实申报，无一放宽放行集；无升 high 因素、无降 low 因素。原一句话证成沿用：本批全部落在合并机械链与集成执行面——承载的是「每次并行批必现、已实际导致一次 human 一次性授权 + 流程外绕行」的病根，不修则下批并行 Pod 的 merge 段照付全款；但改动面全部有 fail-closed 默认与五自检护栏，downside bounded）。
> **review profile**：G2+G3（触面 = integrator.md / pod-coordination.md / acceptance-evidence.md G2 + merge-driver.mjs / pre-merge-escalation-gate.mjs / dev-loop.settings.json / **lib 扩展** G3；契约面为零）+ G4 簿记（ARCH-REVIEW-QUEUE.md）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/merge-driver.mjs
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/references/pod-coordination.md
.claude/references/acceptance-evidence.md
.claude/agents/integrator.md
.claude/dev-loop.settings.json
docs/ARCH-REVIEW-QUEUE.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把 dev-loop 的「Pod 分支 → main」集成链从「流程外知识 + 逐次人批 + 单边消解可假绿」修成「合法执行者 + 成文消解规则 + 版本化制品冲突机械 fail-closed」——并行 Pod 的 merge 段不再靠会话读源码自救，丢正文一侧不再可能静默报绿。

**现状差距**（三条 ARQ 逐一实证，详见 §1）：① 集成期冲突消解没有合法执行者（integrator allow 10/10 全覆盖但 charter 明写「不 merge」，实际执行被 harness 权限层拦停过一次，human 一次性授权 `dc9a5c15` 才走通）；② 版本化制品 `.vN` 槽位可被并行 Pod 同时占用，add/add 冲突两种单边消解都不安全，被 needle 齿偶然命中的那侧**假绿**（schedule-reminder 实测：取先入 main 侧 → 本包冻结齿仍 PASS 而 RMD-A8 约定正文已从生产 prompt 消失）；③ 集成期实测陷阱（CRLF 物化翻红承重闸 / graph 缓存拦 merge / 消解后只跑本包档漏对端红 / Pod 分支含 merge commit 与 driver rebase 结构性不兼容）全靠执行会话现查现解。

**非目标（本批不做）**：
- 不给 dev-loop 任何角色开 `docs/spec.md` / `docs/CONTRACT-DECISION-QUEUE.md` / `docs/contracts/**` 写权——面 B **只加核验、不加写权**（ARQ 原文红线：开写权会把契约门侵蚀成「谁发现谁改」）；
- 不动 supervisor 判据 / 不改升级闸 cross-system 判据语义（那是 Track A 的 escalation-cross-system-overfire）；
- ~~不做版本化守卫的规约面改法（版本无关 green-guard 撰写约定归 versioned-guard ARQ，Track A / OQ1）~~——**OQ1 human 2026-09-08 裁 (b) 并批，本条自 v0.2 起作废：规约面由 T7 承载（定稿回灌时显式标注，保留原行供审计追溯）**；
- 不预批任何既有已 merged 包冻结件的修改（schedule-reminder 三陷阱的「对端红复跑」只成文为 runbook 纪律，回头修他人冻结件须逐次授权）；
- 不动 pod-events schema（`versioned-artifact-claim` 用既有 friction/decision 之外的自由事件型，schema 收编留后续——与 human-deputy 批对事件型的同款克制一致）。

## 1. 问题与根因（地面真值）

### P1 · 集成期冲突消解：权限面 ≠ 职责面，执行链零成文

ARQ-260908-pod-integration-no-legitimate-executor 面 A 实测（260829-schedule-reminder，2026-09-08）：与已并 main 的 story-p3 撞 10 条冲突，scope-map 逐 role 实测 builder/fixer 覆盖 6/10、integrator allow 10/10 全覆盖但 integrator.md:74 verbatim「不 push、不合并主干」——第一次 `git merge --no-commit --no-ff main` 被 harness 权限层按该边界拦停，改走只读 `git merge-tree` + 仓外副本探测，直到 human 授权才回落 integrator。⇒ 现态 = 每次并行批的集成段都要现付一次「谁来做、按什么规则做」的人肉裁决。

顺带实测的三处陷阱（消解规则不成文则下次照踩）：
- **(a) CRLF 物化翻红承重闸**：`git checkout --ours <current/投影>` 按 `core.autocrlf=true` 物化成 CRLF（433 行、字节数差恰为 433），而 `taskpackage-required-check.mjs:143` 用 sha256 逐字节比对 canonical ↔ current ⇒ 闸由绿翻红，需手工归一 LF；merge-driver T3 投影特判走同一 `git checkout --theirs` 机制（merge-driver.mjs:853），同理会踩且自检夹具未覆盖行尾维度。
- **(b) graph 缓存拦 merge**：`.codebase-memory/{graph.db.zst,artifact.json}`（`merge=ours`）内容漂移让 merge 起手即中止；driver 内部 helper `ensureGraphCacheSynced`（:738，三拍）**未导出**，人工/integrator 消解只能自行复刻。
- **(c) 消解后只跑本包档漏对端红**：并集 v3 后 story-p3 冻结齿对 `provider.config.yaml` 注释敏感——只重跑本包档发现不了。
- **并证二补①（结构性）**：Pod 分支含「并 main」的 merge commit 与 driver 的朴素 `git rebase main`（:1197）结构性不兼容（实测 `Rebasing (1/17)` → 撞冲突 → abort + blocked，合并 fail-closed 无损但空转）；线性化后同一命令自证 up to date。⇒「集成期该用 rebase 还是 merge」是驱动契约问题，runbook 必须写死「Pod 分支集成一律 rebase 形态、禁在 Pod 分支引入 merge commit」。
- **并证二补②（双源）**：`approvalRefUnlock`（:915-921，本会话亲核确认仍在）以 `probeWt = findWorktree(tp, projectDir) || projectDir` 作 cwd、gate 参数却传 `--project-dir projectDir`——两源解析到不同主体时 `--source-ref HEAD` 在错误 worktree 求值。可自证的是代码形状，触发条件与修复口径交评审期判。（**已裁，2026-09-08**：A-3 事实层确认与常规路径 Step 1b 同形，唯一缺口 = 回退分支静默落主仓；修复口径 = 维持两形、只改豁免路径回退分支 errorExit 化——见 §2 T1-4。）

### P2 · 版本化制品槽位可并发占用，单边消解方向不对称的假绿

ARQ-260908-versioned-artifact-slot-collision 实测（同包收口集成探测）：两并行包各自把 prompt v2→v3，`git merge-tree` 实测 CONFLICT (add/add) 5 冲突块、接线面共 8 条；仓外副本三消解对照——取先入 main 者**假绿**（needle「备注」在故事版被课表段偶然命中计数 2/1）、取本包者对端 RED、取并集双方全绿（8 failed 既有红同名单 / 155 passed）。既有冲突口径 eventId `5697792c`「以先入 main 者为准」在本案恰产出假绿支——该口径对重复修复成立、对不同正文抢同槽位不成立。**根因**：「谁占哪一版」无机械登记（两包各自看 deploy 目录猜）+ 合并相位无版本化制品特判（交人随手解）+ 消解侧无双包齿复跑纪律。

### P3 · scripts 承重闸无 lib 分离，跨脚本复用只能 spawn

ARQ-260907-gate-lib-script-separation（260907-gov-review-merge-friction retro F3）：gate 无 import.meta 守卫、顶层直跑 + process.exit——merge-driver T1 复用五要素核验只能 spawn（双进程开销 + 以 merge-driver argv 误判 changeId 的语义耦合）。本批大量动 merge-driver 与 gate，是落实分离的顺带时机（OQ2）。

## 2. 改法（T1…T7）

### T1 · merge-driver：版本化制品 add/add 特判 + 投影消解 LF 归一 + approvalRefUnlock 回退分支修复（G3）

1. **版本化制品 add/add fail-closed 特判**：rebase/merge 冲突集检测到路径匹配版本化制品形态（`*.v[0-9]+.md` 及 `.claude/references/pod-coordination.md` §10c 登记表所列路径）且冲突类型为 add/add → 不进入通用 blocked 消息，输出显式指引：「版本化制品 add/add——禁 `--ours/--theirs` 单边消解（丢正文侧可能被 needle 齿偶然命中而假绿），消解 = 取并集 + 复跑**双方** carrier 齿并留 exit code」，置 blocked（消解规则预登记后按 §10c runbook 走）。与投影特判（:828）同层实现，白名单可扩。**补集不变式（A-7 已裁）**：特判只 widen 指引文案、不 widen 放行——登记表外新形态（无 `.vN` 后缀的版本化物）退回通用 blocked fail-closed，由 V1-① 反例夹具机证。
2. **投影消解 LF 归一**：T3 投影特判 `git checkout --theirs` 后对物化文件做 CRLF→LF 归一再落盘（修陷阱 (a) 的 driver 侧半边），自检夹具补行尾维度断言（canonical ↔ current sha256 相等）。
3. **`ensureGraphCacheSynced` 导出**：命名导出（陷阱 (b)），pod-coordination runbook 引用之，人工/integrator 消解不再复刻三拍。
4. **approvalRefUnlock 回退分支修复（A-3 已裁：维持两形、只改豁免路径回退分支）**：现状两路径（`approvalRefUnlock` 与常规路径 Step 1b）均为「git 在 cwd（Pod worktree）求值 source-ref、文件读在 `--project-dir`（主仓）求值」的有意同形语义（评审期亲核：merge-driver.mjs:918-921 与 :1136-1142），**不改两路径传参、不统一口径**；唯一缺口 = 豁免路径 `findWorktree(tp, projectDir) || projectDir` 的回退分支（:919）在 worktree 解析失败时 cwd 静默落主仓 → `--source-ref HEAD` 在 main 求值 → off-main BLOCK 恒空转（方向 fail-closed 无损）。修法 = 把回退分支改为 `findWorktree` 失败即显式 errorExit（与 Step 1b 同款，评审期亲核 :1126-1128），并补一条自检断言钉住「回退分支消除后 worktree 解析失败显式 errorExit 而非静默落主仓」。
5. 五自检全量复跑通过为 landing 前置。

目标文件：`.claude/scripts/merge-driver.mjs`

### T2 · pod-coordination.md：版本化制品槽位预约登记 + §10 runbook 补齐（G2）

1. **§10c 版本化制品 `.vN` 占用登记**（新节）：Pod launch 时在 pod-events 记 `versioned-artifact-claim: <path>@vN`；后到 Pod 见在先 claim 即从 vN+1 起编或等前包合并后重取基线。**双源关系（A-7 已裁）**：登记表为 authority、`*.v[0-9]+.md` 形态正则为兜底——不一致时赢家 = 登记表；正则角色 = merge-driver 特判命中集的形态兜底，登记表外新形态退回通用 blocked fail-closed（GA-7 补集不变式）。**登记表初始清单 = 实测超集（F9 修向）**：既有 `server/deploy/openclaw-system-prompt.vN.md` 一类之外，五类逐条入册——engine-patch/、openclaw-patch/、`OPENCLAW_SYSTEM_PROMPT_CARRIER`（无扩展名）、`provider.config.yaml`、`apply-xiaozhi-overlay.sh`。**维护三句（B-2）**：owner = 批主线程（触碰登记表所列形态的批 landing 时随批更新）；更新触发 = 批版本化制品路径集与登记表 diff 非空；vN 回收 = 不做机械回收、槽位由 forward-bump 语义由后版覆盖（实测需要再另行立项）。**单一权威互指注记（B-0）**：本节为版本化制品消解口径单一权威；merge-driver 特判指引文案与 integrator.md 消解段为引用方，改本节口径须同批核两引用。
2. **§10a runbook 补五条**：① 形态钉死——「Pod 分支集成一律 rebase 形态、禁在 Pod 分支引入 merge commit」（并证二①的驱动契约句）；② graph 缓存三拍引用 T1 导出的 `ensureGraphCacheSynced`；③ current/ 投影取被并包侧**且归一 LF**（sha256 逐字节比对语义提醒）；④ 消解完成后复跑**双方**冻结件 + 全档 + host ELF（文档命令位）+ 设备真编 + 九闸脚本并回报实测数字（陷阱 (c) 对端红纪律）；⑤ **ask 互洽命令形态（N3，A-4 已裁）**——§10a 增一步一次性设置（或由 merge-driver 建 worktree 时代设）Pod worktree 本地 `git config core.editor true`，第 3 步改素形 `git rebase --continue`（以 `git rebase` 起头 → T4 ask 前缀命中；注意 `git -c core.editor=true` 形态不命中前缀、不解决问题，须用 config 落盘形态）。
3. 与 `ARQ-260907-merge-closeout-bookkeeping-gap` 候选④已落地的 §10a/§10b 无缝衔接（本条只补增量，不重写既有 runbook）。

目标文件：`.claude/references/pod-coordination.md`

### T3 · integrator.md「merge-chain execution」节（G2，A-1 已裁：agent def 执行语义变更、恒 human-approved，human 2026-09-08 已批）

正文新增一节，边界钉死（原文写入 def）：**仅 Pod 分支**（分支名匹配 `dev/<changeId>`）、**永不 push**、**永不 merge 进 main**（main 侧合并恒归 planner 主线程 merge-driver 路径——与既有 :12/:74 语义一致，本节是把「集成期 Pod 分支上的 rebase/冲突消解」从默认禁止里显式豁出并加前置）、不改 `*.acceptance.*` / `docs/contracts/**` / `docs/spec.md`、**消解规则必须预登记**（planner 派单逐文件写死 + human 授权事件 id 在场，缺一即停并回报）、集成树验证回报义务（T2 ④ 同款清单）。fs-write 类闸产物（supervisor-verdict.json）的收口提交归属一并写入本节（并证二③）。**定稿正文（整节原样可落）见附录 A.1**——A-1 判据先例四条件核验通过：frontmatter 零改动 / write scope 零变更 / 权限语义为显式豁出且带可机判的 fail-closed 前置 / authority 零转移，不触 lightweight 硬排除判据字面。

目标文件：`.claude/agents/integrator.md`（含 B-4 写作侧对齐：:42 Findings 写作约定补一句「Kind=hard-violation 且 Basis 涉契约/spec 时，Basis 须以 docs/contracts** 或 docs/spec.md 字面前缀起引（格式注记：此括注为散文提及、非锚行——landing-fidelity 或锚判据修正，verdict 转绿五前提之追认 2026-09-08）」——A-8 同场裁，零新增文件）

### T4 · dev-loop.settings.json ask 条目（G3，A-4/OQ3 已裁形态 (a) ask + N3 互洽修法）

`.claude/dev-loop.settings.json` 补 `permissions.ask` 两形（A-4 已裁；ARQ 原文写 `.claude/settings/dev-loop.settings.json`，实际路径 = `.claude/dev-loop.settings.json`，本批按实况修正）：

```json
"ask": [
  "Bash(git merge:*)",
  "Bash(git rebase:*)"
]
```

排除 (b) 条件 allow——settings 权限层不可表达（规则只有「工具 + 参数模式」两维，条件语义唯一通道 = 新 PreToolUse hook = 事实上的新闸，另批走 full）；排除 (c) deny + runbook——拦死 runbook 合法消解路径，与 P1 病根同形倒退。配套 N3 互洽修法（T2-2⑤）落地后，§10a 消解主循环每轮命令均命中 ask 前缀。merge-driver 自身不受影响（内部 git 走 child_process，不经 Bash 权限层）。

**两条诚实边界（A-4 回填义务 ①，随条款落、不只落 landing-summary）**：① 条件语义（Pod worktree 内 / 预登记消解规则 + human 授权事件在场）机械层表达不了，由 T3 正文纪律承载，ask 只机械兜「每次必有人在场」；② 前缀匹配可被复合命令、env 前缀、child_process 三类绕道，airtight 版本需 hook（留根另批，见 §6——等价新设闸，机械硬排除命中）。

**主张限定句（A-4 回填义务 ②）**：ask = 手动 Bash 路径 best-effort；复合命令、env 前缀、child_process 三类绕道在册。

**N6（A-4 回填义务 ③）**：`_doc` 数组（:3-15，现成承载位）加一行「ask 弹窗批准判据：核 human 授权 eventId 在场且可锚到 pod-events 对应事件」。

**B-7 耦合声明（A-4 回填义务 ④，随 §6 hook 留根行并入）**：另批 hook 若核授权事件，import governance-gate-shared.mjs（与 gate / merge-driver 同源）或复刻五要素——形态另批裁，耦合声明随该批 landing 归档。

目标文件：`.claude/dev-loop.settings.json`

### T5 · 面 B：契约/治理 canonical 改动「交回→在册」机械核验（G3，A-2 已裁定性：既有前置升级闸的门内加项，非新增 merge gate、非第 7 工程门）

`pre-merge-escalation-gate.mjs` 加一条核验：review-packet Findings 中 `Kind=hard-violation` 且 Basis 引 `docs/contracts/**` / `docs/spec.md`（或 severity ≥ SHOULD 的 contract/boundary 类）者，MUST 在 `docs/CONTRACT-DECISION-QUEUE.md` 有对应 CDQ 行、或有 human 显式 defer / accepted-risk 记录（eventId 锚），否则红。**只加核验、不加写权**（§0 非目标红线）。

**匹配形态（A-8/B-4 补偿控制①，gate 核验行注释钉）**：Basis 以 `docs/contracts/**` / `docs/spec.md` 字面前缀起引、固定串比对；配套写作侧对齐约定同批落 integrator.md:42（见 T3 目标文件行）。

**未识别形态方向（A-8 已裁 = 放行）**：该闸 fail-closed 语义 = **可识别违规必拦**；未匹配 Basis 形态放行——后果是「漏账不被机械拦」，与批前零核验同水平、不恶化，不产生假绿。闸信噪比理由在案：Findings 是自由 prose（六列约定、无 schema），「未匹配即红」会把写作措辞变化全变假红（对偶教训：ARQ-260906-lfc-verbatim-crlf-citation-gap 字节级严格匹配 → advisory 信噪比走低）。

**defer / accepted-risk 锚格式（B-6，枚举值随 human 批 A-8 确认）**：= pod-events `human-decision` 事件，payload.decision ∈ {defer-contract-change, accept-risk}；核验复用 resolveHumanApproval 五要素、decision 值参数化为上列。

**gate 事实定性（A-2）**：本条为既有前置升级闸的门内加项（对偶 DEV-GOVERNANCE §5「closeout 硬化 = verifier 闸族门内加项」折进注范式），不删不改既有六条例外条目（canary 5），六工程门不变式以此形态满足；折进注留痕随本批 landing 落 DEV-GOVERNANCE §5 注（该文件不在本批 manifest，由后续批顺带核引，本条先如实声明 gate 事实）。与 `ARQ-260827-contract-mech-gate-gap`（pass-oracle 第 4 关无脚本兜底，人工核对降级）同族——本条是该缺口在 dev-loop 侧的第一块机械补丁，不宣称补齐第 4 关。

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`

### T6 · gate 可导入库分离（OQ2 human 裁随批，确认在批）

gate 五要素核验共享函数（`resolveHumanApproval` 等）**并入既有** `.claude/scripts/lib/governance-gate-shared.mjs`（F1 终裁：**扩展既有 lib——追加命名导出、既有导出与函数体零改动，不改名**；lib 头注 B5「computeFrozenPin 纯新增 + isFrozen 函数体零改动」纯新增并入先例在案。改名新 lib = 两个治理共享 lib 并存，「哪些函数归哪个 lib」成新双源，GA-6 同族病自我复制，不采）。gate 薄壳化 + `import.meta` 守卫；merge-driver T1 的 spawn 复用改为 import（消双跑 + 语义耦合字面复用）。

**N5 申报句（已裁选项 2）**：解锁预核（approvalRefUnlock）收窄为五要素事件核验，全闸由 Step 1b 同 invocation 背书（merge-driver :1132-1150，评审期亲核）——解锁后同 invocation 内 Step 1b 无条件再 spawn 全闸，任一违例即 BLOCK + setPod blocked，端到端「没有全闸放行就没有 merge」不变式零变化；canary 4 kept-blocked 字面与意图不动（:927 起 catch 块亲核在案）；ready-to-merge 瞬态自纠于回落 blocked，board 状态属信息面。

gate 判定语义零变更（纯结构重构；等价断言显式排除「以 merge-driver argv 误判 changeId」语义差——旧形态是 bug 非 spec 行为，GA-10②），自检夹具双路径复跑 + 七消费方名册（N2：含 portfolio-reconcile.mjs:34 具名导入，六改七）全量复跑钉住等价（见 V2）。

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`、`.claude/scripts/lib/governance-gate-shared.mjs`（**既有 lib 扩展，非新增文件**）、`.claude/scripts/merge-driver.mjs`

### T7 · 版本化制品 green-guard 撰写约定（OQ1 裁并批承载 versioned-guard ARQ；A-5 已裁落点）

按 ARQ-260908-versioned-guard-version-literal-coupling 落三条：① **版本无关接线守卫约定**成文——「版本化制品的接线类 green-guard 必须：解析当前被接线/渲染的那一版文件（从结构化指向读——渲染面如 `apply-xiaozhi-overlay.sh` 的 `PROMPT_SRC`、声明面如 `provider.config.yaml` 的 `system_prompt.*`，不从目录猜）→ 断言该文件确实落盘（防孤儿）→ 断言其正文逐字携带本包语义锚点（防未来版丢内容）；禁在接线类断言里硬写版本字面文件名」。**落点钉死（A-5 已裁）= `.claude/references/acceptance-evidence.md`**：「必要证据」段（:5-11）末尾追加条款 + 同文件「反例」节（:31-39）挂「硬写版本字面」反例；被排除两落点（task-intake-and-slicing.md / DEV-GOVERNANCE.md）不需要指回引用——两者从未承载该约定，单一权威 = acceptance-evidence.md，无 B-0 式漂移源。**类别标注格式（A-6/F12 已钉死）**：撰写守卫时以固定 token 注释标注类别 `<!-- guard-class: wiring | landing | wording -->`（正则可判、人类可读、不与 prose 冲突）——接线 vs 落盘/措辞二分以该 token 为类别判据；即便 lint 留根另批，格式本批先钉（撰写当下即用，防自由散文防线）。定稿条款正文见附录 A.2。② **全仓既有字面耦合守卫清点**随批执行：`grep -rn "openclaw-system-prompt.v[0-9]"` 于验收断言字面处逐条判「接线守卫（该版本无关化）vs 落盘/措辞守卫（钉死某版合理）」——在册实例清单（含处置候选与防误伤注记）见附录 A.3。③ **既有守卫重指**触他人冻结件 → 逐件 human 授权，本批只登记清单与规约、不预批任何冻结件改动（与 §0 非目标同款纪律）。**B-8 队列行**：T7③ 落地时（或 V5 结移动 ARCH-REVIEW-QUEUE 的同一动作里）在 `docs/ARCH-REVIEW-QUEUE.md` 增一条排队行引用重指清单，行内注明「发起 = 下一批触碰对应冻结件时（对齐 P7）或 human 专项批」——零新增文件。**工程门判据侧（A-6 已裁）**：本批不设新门（六工程门不变式，不另立枚举、不新增机检判据）——告警级 lint 夹具（「字面版本名出现在新接线类 green-guard 断言里 → warn」，lint-descriptions.mjs RC-2 告警级先例；lint 化时以 guard-class token 为信号而非「同文件出现 .vN 字面」粗粒度规则）留根另批，gate 化（判红）留根另裁。

目标文件：`.claude/references/acceptance-evidence.md`（A-5 裁定落点）+ `docs/ARCH-REVIEW-QUEUE.md`（versioned-guard 行销账 + B-8 排队行，随批 landing）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明（消费方） |
|---|---|---|---|
| 合并机械链 | `.claude/scripts/merge-driver.mjs` | G3 行为扩展（add/add 特判 + LF 归一 + helper 导出 + 回退分支修复）= gate 事实，如实声明交判（已判，A-3/A-2） | planner 主线程 merge 段；§10a runbook |
| 协调 runbook | `.claude/references/pod-coordination.md` | G2 新节 §10c + §10a 补五条 | planner / integrator / human 消解现场 |
| agent def | `.claude/agents/integrator.md` | G2 新节（A-1 已裁：执行语义变更、恒 human-approved；含 B-4 Findings 写作侧对齐一句） | integrator 集成段；planner 派单 |
| dev-loop 平台设置 | `.claude/dev-loop.settings.json` | G3 ask 条目（A-4 已裁；T4 ask = settings 权限层条目非新门） | harness 权限层 |
| 升级闸 | `.claude/scripts/pre-merge-escalation-gate.mjs` | G3 新核验行（gate 事实；A-2 已裁门内加项定性） | 合并门前置；CDQ 台账（只读消费） |
| 共享 lib | `.claude/scripts/lib/governance-gate-shared.mjs` | G3 **扩展既有 lib**（追加导出，既有导出零改动；F1 终裁；OQ2 已裁随批） | gate / merge-driver / portfolio-reconcile 等七消费方 |
| 版本守卫规约（T7） | `.claude/references/acceptance-evidence.md`（A-5 裁定落点）+ 既有守卫重指清单在册（附录 A.3） | G2 成文 + 他人冻结件重指逐件 human 授权（不预批） | acceptance-author 撰写约定；A-6 已裁本批不设门、lint 留根、guard-class 格式钉死 |
| 契约面 | `docs/contracts/**` | **未触（零）** | — |
| `docs/spec.md` / CDQ | — | **未触（零写权）** | T5 只读核验 |
| pod-events schema | — | **未触** | `versioned-artifact-claim` 用自由事件型；defer/accepted-risk 复用既有 human-decision 事件（B-6，payload.decision 参数化，schema 零改） |
| 队列销账 | `docs/ARCH-REVIEW-QUEUE.md` | 簿记（两条 260907 行结移 §2 + versioned-guard 行销账 + B-8 排队行，随批 landing 执行；F3 后入 manifest） | — |

**【硬项 1b · sweep 透镜义务】** 承载面现跑核验（2026-09-08 主线程）：① `approvalRefUnlock` 双源形状亲核（merge-driver.mjs:915-921 `probeWt` vs `--project-dir projectDir` 在场）；② `.claude/settings/` 目录不存在、dev-loop 设置实为 `.claude/dev-loop.settings.json`（ARQ 路径笔误已修正）；③ `.gitattributes` 现有两条（`*.sh eol=lf` / `progress.md merge=union`），本批无新增 attributes 需求；④ integrator 现行边界句 :12/:54/:73/:74 四处亲核，T3 为增量新节、不改既有句。

**【硬项 1c · 复扫模式完备性先验】** 不适用：本批无退役语面/残留清扫（全部为增量新节/新分支/新条目）。

## 4. human 取舍点（OQ）

- **OQ1 · versioned-guard-version-literal-coupling（Track A）是否并批立项**：(a) 分批；(b) 并批走 Track A。—— **human 2026-09-08 裁 (b)**：并批，整批升 Track A，T7 承载。
- **OQ2 · gate-lib-script-separation 是否随批**：(a) 随批；(b) 留下批。—— **human 2026-09-08 裁 (a)**：随批，T6 确认在批。
- **OQ3 · T4 settings 条件化 git 的严格档**：(a) ask（推荐）；(b) 条件 allow；(c) deny + runbook。—— **human 2026-09-08 裁 (a) ask + N3 互洽修法**（decision-packet A-4）：ask 条件保留，先修 runbook 命令形态互洽（T2-2⑤）；排除 (b)（settings 层不可表达，唯一通道 = 新 PreToolUse hook = 事实上的新闸，另批走 full）与 (c)（拦死 runbook 合法消解路径，与 P1 病根同形倒退）；降级预案留根（§6）。
- **OQ4 · escalation-cross-system-overfire（Track A）启动时机**：(a) merge 批 ack 后即启；(b) 攒样本后再开；(c) 接受恒批记 watchlist。—— **human 2026-09-08 裁 (a)**：本批 ack 后即启，单独 Track A 批另行立项（不并入本批改法面）。

**human 拍板终裁（2026-09-08，九项全按 decision-packet §四裁决建议批准）**：

1. **A-3 T1-4 修复口径**：维持两形、只改豁免路径回退分支（errorExit 化）。
2. **A-1 判据先例归档 + integrator.md 新节生效**：不触 lightweight 硬排除判据字面、恒 human-approved（approver = human 不可降档）；先例四条件归档（frontmatter 零改动 / write scope 零变更 / 权限语义为显式豁出且带可机判 fail-closed 前置 / authority 零转移；任一不满足的同类申报存疑向严默认 ESCALATE）。
3. **A-4/OQ3**：ask 条件保留，先修 runbook 命令形态互洽；降级预案留根。
4. **A-5 T7 规约落点**：`.claude/references/acceptance-evidence.md`；两排除落点不加指回引用。
5. **A-6**：本批不设新门，lint 夹具留根；guard-class 标注格式本批钉死。
6. **A-8**：放行（只拦可识别形态）+ 三件补偿控制同批到位（Basis 匹配形态注释 + V2 两态夹具 / integrator.md:42 写作侧对齐 / B-6 defer 锚格式）；B-6 枚举值 {defer-contract-change, accept-risk} 一并确认。
7. **N5 解锁核验集收窄**：选项 2——收窄 + Step 1b 背书申报句 + V2 背书夹具。
8. **B-1 机械消费挂点**：留根观测后批（按 §9 毕业条件）。
9. **整包批准**：decision-packet 全部裁断 + riskTier medium 复核 + §2.1 十六处回灌点授权 architect 定稿回灌——批准后进定稿（本稿 v1.0 候选）→ human 冻结确认 → 主线程 packet-bound landing 序列。

## 5. 验证集（landing 前机械前置 + landing 后核验）

- **V1 · merge-driver 五自检复跑**：`--self-check-retro-advisory / quick-checks / detect-changes / rebase-target / worktree-topology` 全绿；新增四夹具——①版本化制品 add/add → 显式指引 + blocked（非通用 blocked 文案），**含反例夹具（F9/A-7 补集不变式机证）**：engine-patch 形态（登记表外、无 `.vN` 后缀的版本化物）add/add → 通用 blocked（非特判、非放行）；②投影消解后 canonical ↔ current sha256 相等（LF 归一断言，夹具含 autocrlf 场景）；③**approvalRefUnlock 形状断言（F6 按 A-3 裁断条件化改写）**：与 Step 1b 同形（source-ref 在 worktree cwd 求值、文件读走 `--project-dir` 主仓）+ 回退分支消除后 worktree 解析失败显式 errorExit 而非静默落主仓；④`ensureGraphCacheSynced` 导出可 import。
- **V2 · gate 核验行两态夹具 + T6 等价夹具**（OQ2 已裁 (a) 随批）：① **T5 两态（A-8 补偿控制，真空绿规约：fixture 非空断言先行）**——正例：有 CDQ 行 / human defer eventId 锚（human-decision 事件、payload.decision ∈ {defer-contract-change, accept-risk}）→ 放行；负例：`Kind=hard-violation` + Basis 以 `docs/contracts/**` / `docs/spec.md` 字面前缀起引 + 无 CDQ 行无锚 → exit 1 红。② **T6 lib-import 等价**：spawn 与 import 双路径同结果；**import 探针主断言（N1/F8 修向终稿）**——`node -e "import('<lib>').then(m => { if (typeof m.resolveHumanApproval !== 'function') process.exit(2); }).catch(() => process.exit(3))"` 须 exit 0（一个探针同时机证 import 时进程存活 = 顶层零副作用零 exit + 提取导出可调）；**辅断言** `grep -c "process.argv" <lib>` = 0（实测零命中，持续可判）；弃「不 process.exit」全称负断言（函数体归属需 AST、grep 不可判，写成断言就是下一个 F2——「process.exit 仅存在于 run*SelfCheck 体内」只作注记不作断言）。③ **七消费方名册**（N2：portfolio-reconcile.mjs:34 具名导入在册，六改七）各既有 self-check 全量复跑零回退（含 portfolio-reconcile `--self-check`，入口 :329 可跑）。④ **N5 背书夹具**：unlock 收窄形态下，off-main / provenance 违例 Pod 解锁后仍在同 invocation 被 Step 1b BLOCK 并回落 blocked。⑤ 等价断言显式排除「以 merge-driver argv 误判 changeId」语义差（GA-10②：旧形态是 bug 非 spec 行为）。
- **V3 · settings ask 探针**（OQ3 已裁 (a) ask + N3 互洽修法）：Pod worktree 内 `git merge` / `git rebase`（含修后 §10a 素形 `git rebase --continue`）→ ask 前缀命中弹窗；「授权事件在场」条件语义机械层不判（T3 正文纪律承载）；merge-driver 自身（child_process 路径）不受影响。
- **V4 · 投影 grep（正负双断言，F2 修向 = -cF 固定串正向钉死）**：正：`grep -c "merge-chain execution" .claude/agents/integrator.md` ≥1；`grep -c "10c" .claude/references/pod-coordination.md` ≥1；`grep -c "versioned-artifact-claim" .claude/references/pod-coordination.md` ≥1；`grep -c "guard-class" .claude/references/acceptance-evidence.md` ≥1（T7 落点条款）。deny 面正向钉死：`grep -cF 'Edit(./docs/contracts/**)' .claude/dev-loop.settings.json` = 1 且 `grep -cF 'Write(./docs/contracts/**)' .claude/dev-loop.settings.json` = 1（契约面 deny 两行逐字在位）。ask 白名单逐字：`grep -cF 'Bash(git merge:*)' .claude/dev-loop.settings.json` = 1 且 `grep -cF 'Bash(git rebase:*)' .claude/dev-loop.settings.json` = 1。permissions 段无指向 docs/contracts 的 allow 条目。integrator def 中「永不 push / 永不 merge 进 main」逐字在场。
- **V5 · 队列销账核验**：landing 时 `docs/ARCH-REVIEW-QUEUE.md` 中两条 260907 merge ARQ 行已结移 §2、§2 行含 `260907-gov-review-merge-friction` 承载批引用；同一动作里增 B-8 排队行（引用附录 A.3 重指清单，行内注明发起时机——见 T7③）；`node .claude/scripts/changes-index.mjs --upsert 260908-gov-review-merge-chain-hardening` 单行钉入 INDEX。
- **V6 · end-to-end 演练（landing 前置）**：在临时仓夹具复刻「双包抢 v3 槽位」最小场景——A 包 bump v3（含齿 X）→ 并 main → B 包 bump v3（含齿 Y）→ driver 跑 → 断言：add/add 特判触发、并集消解后 X 与 Y 双齿绿、全程无 `--ours/--theirs` 单边提交。（模板 = schedule-reminder review-packet §Integration Probe 实测数据，断言以临时仓夹具实测为准。）

## 6. 留根清单

- **escalation-cross-system-overfire Track A 批**（OQ4 裁本批 ack 后即启）：升级闸 cross-system 判据收紧 vs 接受恒批，另行立项走 `/arch-change`，不占本批改法面。
- **pass-oracle 第 4 关完整兜底**：T5 只补 dev-loop 侧核验，spec↔contract 全量机械门仍缺（ARQ-260827-contract-mech-gate-gap 主体未销）。
- **HOST_ACCEPTANCE_SRCS 顺序承重根治**：seam reset() + boundary 齿自建新鲜 seam，属固件测试基建，归 dev-loop 包承接（closeout-bookkeeping ARQ 的「结构性根治另线」原文保留）。
- **pod-events `versioned-artifact-claim` 事件型收编**：自由事件型先跑，schema 最小校验留观测后批（friction-event-payload-schema ARQ 同族）。
- **settings 条件表达力核实**：已随 A-4 裁断收口（条件 allow 在 settings 层不可表达，裁 (a) ask 形态；探针矩阵见 findings-r2 C-①；airtight hook 留根见下行）。
- **hook airtight 形态 + B-7 耦合声明（A-4）**：复合命令 / env 前缀 / child_process 三类绕道的 airtight 版本需新 PreToolUse hook（等价新设闸，另批走 full）；hook 若核授权事件，import governance-gate-shared.mjs（与 gate / merge-driver 同源）或复刻五要素——形态另批裁，耦合声明随该批 landing 归档。
- **planner 派单格式义务（F7/B-3，S-2 forcing-function 唯一落点）**：派单制品必填字段（逐文件消解规则预登记格式 + human 授权 eventId 引用形态 + 停等超时收口人），归下一批 planner.md 修订或首批 T3 演练实测后承载。
- **versioned-artifact-claim 机械消费挂点（B-1，DISMISS 成立前提 = 本行真落）**：portfolio-reconcile `--check-claim` 形态候选，按 §9 毕业条件观测后批（pod-coordination.md:104-116「便宜档，不建 daemon」「毕业条件量化任一满足即评估升级」先例在案）。
- **字面耦合 lint 夹具 + gate 化（A-6）**：告警级 lint（「字面版本名出现在新接线类 green-guard 断言里 → warn」，lint-descriptions.mjs RC-2 告警级先例；以 `<!-- guard-class: -->` token 为信号）留根，gate 化（判红）另裁另批。
- **ask 收窄降级预案（A-4）**：摩擦实测不可接受时，收窄 ask 至 `Bash(git merge:*)` 一条 + rebase 面靠 T3 正文纪律 + airtight hook 留根（诚实降档，另批走 full——新 hook 等价新设闸）。
- **销账动作执行面**：两条 260907 行的结移由本批 landing 步机械执行（V5），不改写其证据链。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

> adjudicator 收口（2026-09-08）：27 条 finding/A 项终态 = CONFIRMED 22 / DISMISSED 4 / DEADLOCK 0；governance pass-oracle 六关全 GREEN（可冻结，条件 = CONFIRMED 改法清单全部回灌 PROPOSAL 冻结稿 + human 拍板清单 9 项裁断后——两项均已履行，见 §4 终裁记录与 §9 回灌作业记录）。逐条裁定见同目录 `adjudications.md`，裁断正文与 human 拍板清单见 `decision-packet.md`，收口汇总见 `review-packet.md`。

## 8. architect 修订注记（2026-09-08，起草位核正）

只做事实性核正（路径 / 行号 / 机制描述），不改 §4 OQ 已裁内容、不改 §7 判官区、不夹带新改法：

1. **§2 T7 行号核正（2 处）**：① live 齿 `PROMPT_V2_PATH` 读点原引 `:618/:657` 为 ARQ 在 story-p3 评审时点的值；story-p3 + schedule-reminder 落地后行移，当前 main（`fae886a`）实测 = CHR-A8 live `test_a8_live_block_matches_makemeahanzi`（def :678）读点 :683-685、CHR-A9 live `test_a9_live_char_latency_5s_band`（def :718）读点 :722-724、常量 :144。② P2-A6 wiring 齿 `:583-586` 精确为 `:584-587`（`self.assertIn` 块，needle 字面 :585，常量 :144）。
2. **§2 T7 实例清单补全（1 处）**：voice_roundtrip 的 `.v1` 字面耦合消费面显式化——除常量 :116 外，live 探针 A4/A7 读点 :509-512 与落盘齿 :378 同源（carrier 声明文件 `server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER` 已注记该断裂）；VRT-E4 反例精确到接线齿 `test_e4_system_prompt_wired_into_deploy_overlay` :410（同号落盘齿钉 `.v1` 属措辞守卫、合理保留，避免「VRT-E4 免改」被误读为整号免改）。
3. **一处对照系失真申报（不改 §1 正文，供评审裁）**：ARQ-260908-pod-integration-no-legitimate-executor 并证二②称「常规路径 Step 1b（:1137-1143）cwd 与参数同用 wt」——亲核 merge-driver.mjs:1136-1142 实际 = `cwd: wt` + gate 参数仍传 `--project-dir projectDir`，与 `approvalRefUnlock`（:915-921）同形；真实差异仅在 `findWorktree(…) || projectDir` 回退分支（:919，worktree 解析失败时 cwd 落主仓 → `--source-ref HEAD` 求值成 main → off-main BLOCK，方向 fail-closed 无损但恒空转）。T1-4 的「统一口径」裁定需以此为前提（两路径同改 vs 只改豁免路径会成两形），详见 architect-notes §review-bg 关注面 1。（**评审已裁**：adjudicator 独立亲核确认同形成立，ARQ 对照系失真确认；A-3 裁维持两形、只改豁免路径回退分支——T1-4 已按裁断回灌。）

## 9. Review 定稿记录（2026-09-08 · architect 机械回灌）

本稿 v0.2 → v1.0 候选 = architect 按 `decision-packet.md` §2.1 十六处回灌点 + human 拍板（2026-09-08 九项，全按裁决建议）机械回灌 CONFIRMED 改法；只回灌已批改动、不夹带新设计；冻结确认归 human（review-moderator 核验 `proposal-consolidation-check.mjs` 后由主线程标 FROZEN）。十六处回灌点落位对照：

| # | 回灌点 | 落位 |
|---|---|---|
| 1 | manifest 补 `docs/ARCH-REVIEW-QUEUE.md`（F3）+ `acceptance-evidence.md` 落点行（A-5） | 头部 landing-manifest 块 |
| 2 | F1 lib 措辞改「扩展既有 lib（追加导出，既有导出零改动）」 | 头注 review profile、T6 条款、§3 sweep 表 lib 行 |
| 3 | V2 补 F8/N1（import 探针主断言 + argv 零命中辅断言 + changeId 排除句）+ N2 名册六改七 + N5 背书夹具 + A-8 两态夹具 | §5 V2 |
| 4 | V4 负断言按 F2 改写（-cF 固定串 deny 两行 = 1 + ask 白名单两形） | §5 V4 |
| 5 | V1-③ 按 A-3/F6 条件化改写 + V1-① 补 engine-patch 反例夹具（F9） | §5 V1 |
| 6 | §2 标题改 T1…T7（F4） | §2 标题 |
| 7 | :64 删「两者恒用 projectDir」候选（F5），钉 A-3 裁断（维持两形、只改回退分支 errorExit 化）+ 自检断言（F6） | T1-4 |
| 8 | 「补三条」计数订正（F10；与 N3 新增一条合成「补五条」，见下注） | T2-2 |
| 9 | T2-1 初始清单扩实测超集五类（F9/A-7）+ B-2 三句 + B-0 互指注记 | T2-1 |
| 10 | T2 §10a 增 core.editor=true 一次性设置 + 第 3 步改素形 continue（N3，A-4 裁后） | T2-2 ⑤ |
| 11 | T5：A-8 放行方向声明 + Basis 匹配形态 + B-6 defer 锚格式 + A-2 门内加项定性如实声明 | T5 条款 |
| 12 | T6：N5 申报句 + F1 扩展既有 lib 措辞 | T6 条款 |
| 13 | T7：A-5 落点钉死 + A-6/F12 guard-class 格式 + N4 齿名订正（本文件 :160 即 §8 注 1，与 architect-notes :117 两处）+ B-8 队列行 | T7 条款 + §8 注 1 + 附录 A.2/A.3 |
| 14 | T4：A-4 裁断回填四件（两条诚实边界 + 主张限定句 + N6 `_doc` 批准判据行 + B-7 耦合声明） | T4 条款 |
| 15 | §6 留根清单增行：planner 派单格式义务（F7/B-3）、versioned-artifact-claim 挂点（B-1）、hook airtight + B-7（A-4）、lint 夹具 + gate 化（A-6）、ask 收窄预案（A-4 降级） | §6 |
| 16 | T7 目标文件行落点路径回填（A-5 裁后） | T7 目标文件行 |

计数合成说明（F10 × N3）：decision-packet #8 字面为「补三条」改「补四条」（原列 ①②③④），#10 又按 N3 在 §10a 增第 ⑤ 条——冻结稿如实计「补五条」以避免复刻 F10 计数失配。随批顺带的事实性核正（非新设计）：§0 非目标第三行按 OQ1 已裁标注作废；§1 并证二补②与 §8 注 3 补「已裁」指针；§7 追加 adjudicator 收口指针一行；§8 注 1 齿名按 N4 订正为 makemeahanzi。

---

## 附录 A · 定稿正文（architect-notes 候选正文经裁定与 human 拍板回灌，landing 时逐字取用）

> 来源 = architect-notes §T3/§T7 候选正文（评审输入件），经裁定（A-1/A-5/A-6/F12）与 human 拍板（2026-09-08）后整段回灌本附录，使冻结 packet 自含 landing 正文；architect-notes 本体保持评审时点原样（仅 N4 齿名订正），不进 landing-manifest。

### A.1 T3 · integrator.md「merge-chain execution」节定稿正文（插入位置：`## Process / 步骤` 之后、`## References / 共享规则` 之前）

```markdown
## Merge-Chain Execution / 集成链执行（Pod 分支冲突消解，条件触发）

> 触发条件：merge-driver 因冲突 abort + blocked（含版本化制品 add/add 特判 blocked）后、planner 派单指名你执行消解时本节才生效。本节把「集成期 Pod 分支上的 rebase / 冲突消解」从既有「不 merge」默认禁止中显式豁出并加前置；main 侧合并权不因此转移（铁律 3）。前置不满足时停下回报 planner / 升级 human，不自行补条件、不就地改写。

### 前置（缺一即停）

1. planner 派单制品内**逐文件预登记消解规则**——冲突集里出现无预登记口径的文件 = 停。
2. **human 授权事件 id 在场**（pod-events 内可锚定的 eventId，planner 派单引用之）——id 缺失或锚不到对应事件 = 停。
3. worktree 当前分支亲核为 `dev/<changeId>`（`git rev-parse --abbrev-ref HEAD`）——不在本 Pod 分支上 = 停。

### 边界铁律（逐字）

- 仅在 Pod 分支 `dev/<changeId>` 上执行 `git rebase` / `git merge`（消解重放）；**禁在 Pod 分支引入 merge commit**（驱动契约：merge-driver Step 3 rebase 与 merge commit 结构性不兼容）。
- **永不 push**。
- **永不 merge 进 main**；main 侧合并恒归 planner 主线程 merge-driver 路径（supervisor APPROVE + 前置升级闸放行后），不是你。
- 消解中**不改** `*.acceptance.*`（冻结验收件）、`docs/contracts/**`（恒只读）、`docs/spec.md`——冲突集含这些文件 = 停下升级，消解口径由 planner / human 裁，不就地改写。

### 消解执行（按 pod-coordination.md §10a runbook + §10c 版本化制品规则）

- current/ 投影冲突恒取被合并包侧，且物化后**归一 LF**（`core.autocrlf` 物化 CRLF 会让 sha256 逐字节比对承重闸由绿翻红）。
- 版本化制品（§10c 登记表所列 `*.vN` 槽位）add/add 冲突**禁 `--ours` / `--theirs` 单边消解**——单边会让被丢正文一侧的 needle 齿偶然命中而假绿；消解 = 取并集 + 复跑**双方** carrier 齿并留 exit code。
- graph 缓存冲突调 merge-driver 导出的 `ensureGraphCacheSynced`，不自行复刻三拍。

### 消解完成后（集成树复跑清单，实测数字回报入 review-packet「集成执行回报」）

双方冻结件（本包 + 对端包）+ 全档 pytest + host ELF（文档命令位）+ 设备真编 + 九闸脚本。只跑本包档不算完成——对端红漏检是实测陷阱（消解副作用只在对端冻结齿上显形）。

### fs-write 闸产物收口提交归属

`supervisor-verdict.json` 等机械闸 fs-write 产物，由你以 chore commit 收口进本 Pod 分支（对齐步骤 12 的 feature 分支 commit 职责），不留 unstaged 的 blocking 机器件——merge-driver Step 0 白名单收口是兜底，不是常态路径。收口后 `portfolio-reconcile.mjs --set-pod <id> --status ready-to-merge`，交 planner 重跑 merge-driver。
```

frontmatter（name / tools / model / description）零改动、write scope 零变更（scope-map 不改，canary 12）。

### A.2 T7 · acceptance-evidence.md 条款定稿正文（落入「必要证据」段末尾；「反例」节同批挂「硬写版本字面」反例）

```markdown
- 版本化制品（`*.vN` 落盘物，如 `server/deploy/openclaw-system-prompt.vN.md`、engine-patch、overlay 配置）的**接线类 green-guard 必须版本无关**，三步：① 解析当前被接线 / 渲染的那一版文件名——从结构化指向读（渲染面如 `apply-xiaozhi-overlay.sh` 的 `PROMPT_SRC`、声明面如 `provider.config.yaml` 的 `system_prompt.artifact|source|deploy_target`），不从目录猜；② 断言该文件确实落盘（防孤儿）；③ 断言其正文逐字携带本包语义锚点（防未来版丢内容）。**禁在接线类断言里硬写版本字面文件名**：前序包 forward-bump 后字面陈旧，守卫要么假红（断言字面陈旧非行为破损、需 human ruling 才放行），要么假绿（needle 在别的版本正文里偶然命中，丢正文侧静默报绿——schedule-reminder 实测先例）。**落盘 / 措辞类守卫钉死某版合理**（如断言某版基座结构、措辞收紧），撰写时以固定 token 注释标注类别 `<!-- guard-class: wiring | landing | wording -->`，与接线类区分（guard-class token = 接线 / 落盘措辞二分的类别判据）。重指母版 = `test_e3_current_prompt_wired_and_carries_char_c1`（human 裁 `c54bbcde`）；清点范围 = 全仓 grep `openclaw-system-prompt.v[0-9]` 于验收断言字面处逐条判「接线守卫（该版本无关化）vs 落盘 / 措辞守卫（钉死某版合理）」。
```

### A.3 T7 · 既有守卫重指清单表（在册实例逐条，行号按 main tip `fae886a` 亲核）

| # | 实例 | 位置（亲核） | 类别 | 处置候选 |
|---|---|---|---|---|
| 1 | char-stroke CHR-E3 接线齿 `test_e3_current_prompt_wired_and_carries_char_c1` | `server/acceptance/test_char_stroke.acceptance.py:467`（解析 PROMPT_SRC / provider 声明两面得版本再断言，docstring :468-482 载改判缘由） | 接线守卫 | **已重指**（human 裁 `c54bbcde`）——版本无关形态母版，免动 |
| 2 | char-stroke CHR-E3 措辞齿 `test_e3_prompt_v2_landed_with_discipline` | 同文件 :423（`PROMPT_V2_PATH` 读点 :432-435，常量 :144） | 落盘 / 措辞守卫 | 免改——钉 v2 措辞面合理（ARQ 原文「姊妹齿保留」） |
| 3 | schedule_p2 P2-A6 wiring 齿 | `server/acceptance/test_schedule_p2.acceptance.py:584-587`（needle 字面 :585，常量 :144） | 接线守卫 | **现属假绿**——绿由 v2 文件自身 + config 历史句维持、与真接线（已 v3）无关；待重指，触他人冻结件须逐件 human 授权（本批只登记不预批） |
| 4 | voice_roundtrip VRT 落盘齿 `test_e4_system_prompt_versioned_landed` | `server/acceptance/test_voice_roundtrip_b.acceptance.py:378`（`SYSTEM_PROMPT_PATH` 常量 :116 钉 `.v1`） | 落盘 / 措辞守卫 | 免改——钉 v1 基座结构合理 |
| 5 | voice_roundtrip VRT 接线齿 `test_e4_system_prompt_wired_into_deploy_overlay` | 同文件 :410（needle = `SYSTEM_PROMPT_NAME` 裸名 :117，corpus 只读 deploy 无扩展名文件含 `OPENCLAW_SYSTEM_PROMPT_CARRIER`） | 接线守卫（天然版本无关） | **免改——反例母版**（ARQ 原文「裸名 + 无扩展名 carrier」即此齿） |
| 6 | voice_roundtrip VRT live 探针（A4 / A7） | 同文件 :509-512（读 `SYSTEM_PROMPT_PATH` = `.v1`；live 齿类 :423 起） | live 接线（所测 .v1 ≠ 生产 v3） | 待重指——carrier 声明文件已注记该断裂；无 live env 重指无法实证（ARQ 原文），随本 ARQ 落地时一并走版本无关重指 + 核 `PROMPT_V1_PATH`/`PROMPT_V2_PATH` 常量语义与 `OPENCLAW_SYSTEM_PROMPT_CARRIER` 口径统一 |
| 7 | char-stroke CHR-A8/A9 live 探针 | `server/acceptance/test_char_stroke.acceptance.py:683-685`（`test_a8_live_block_matches_makemeahanzi`，def :678）与 :722-724（`test_a9_live_char_latency_5s_band`，def :718）；常量 :144 | live 接线（所测 .v2 ≠ 生产 v3） | 待重指——同 #6 语义（ARQ 原行号 :618/:657 为 story-p3 评审时点值，落地后行移，§8 注 1 已核正；齿名按 N4 订正为 makemeahanzi） |
| 8 | schedule_p2 `PROMPT_CARRIER_V2` 常量 | `server/acceptance/test_schedule_p2.acceptance.py:144` | 常量（被 #3 消费） | 随 #3 重指一并处置 |

清单外防误伤：`test_schedule_reminder.acceptance.py:294` `PROMPT_CARRIER_V3` 与 :1416 的 RMD-A8 齿是本批 P2 假绿问题的**受害侧**（其 v3 字面合理——它是当前接线版的事实锚点，且已由同包新齿 `test_a8_prompt_carrier_v3_reminder_section_anchors` 补闭假绿，carrier 声明文件注记在案），不在重指清单内。
