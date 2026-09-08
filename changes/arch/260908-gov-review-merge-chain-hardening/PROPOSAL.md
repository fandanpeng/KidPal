# PROPOSAL · 260908-gov-review-merge-chain-hardening

> **状态：DRAFT v0.2 · 2026-09-08 · human 立项 ack 已获（AskUserQuestion 四裁：ack 进评审 / OQ1=并批走 Track A / OQ2=随批 / OQ4=merge 批后即启 escalation Track A）；review 轨按 OQ1 裁断升 Track A，入口 `/arch-change`（review-moderator 编排）**
> **承载 ARQ**：`ARQ-260908-pod-integration-no-legitimate-executor`（主承载，面 A 全量 + 面 B 核验行）+ `ARQ-260908-versioned-artifact-slot-collision`（主承载）+ **`ARQ-260908-versioned-guard-version-literal-coupling`（OQ1 裁并批入内，T7 规约面）** + `ARQ-260907-gate-lib-script-separation`（OQ2 裁随批，T6 确认在批）。**销账簿记（不在本批改法内）**：`ARQ-260907-merge-closeout-bookkeeping-gap` 与 `ARQ-260907-merge-driver-approval-state-desync` 经 2026-09-08 主线程核销确认已被 `260907-gov-review-merge-friction` 批完整落地（merge-driver.mjs T1 `approvalRefUnlock` :915 / T2 Step 0 收口 :777 / T3 投影特判 :828 / T4 `progress.md merge=union`；pod-coordination.md §10 runbook + 簿记口径；五自检 T1/T2/T3/T4/T6 在册），队列 §1 两行属销账滞后，本批随 landing 结移 §2。**明确不入本批**：`ARQ-260907-escalation-cross-system-overfire`（自评 Track A full-adversarial + human，升级闸判据语义变更，入口 `/arch-change`，见 §4 OQ4）；`ARQ-260908-versioned-guard-version-literal-coupling`（自评 Track A，与本批 slot-collision 互补——本批治「槽位可并发占用 + 消解机械面」，该条治「守卫耦合版本字面」的规约面，是否并批立项见 OQ1）；非 merge 的三条新增闸面 ARQ（tighten-only-unwritten / gate-pytest-import-mode-incompat / known-red-count-no-mechanical-source）留下一批治理批，本批不夹带。
> **review 轨**：**Track A full-adversarial + human（OQ1 human 2026-09-08 裁并批升级；原自评 Track B 作废）**。入口 `/arch-change`（review-moderator 编排：scenario-author 冻 charter → challenger↔defender 辩论 → adjudicator 裁定 → decision packet 交 human 拍板 → 主线程 packet-bound landing + landing-verifier）。升级归因如实记录：versioned-guard ARQ 并批后触「闸判据/规约落点」真设计分叉面扩大，human 拍板整批走 full。**两处判据适用存疑点仍申报判官复核**：① integrator.md 正文新增「merge-chain execution」节是否触 self-def 硬排除；② T6 闸内新核验行 = gate 事实，判官复核归档。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（一句话证成：本批全部落在合并机械链与集成执行面——承载的是「每次并行批必现、已实际导致一次 human 一次性授权 + 流程外绕行」的病根，不修则下批并行 Pod 的 merge 段照付全款；但改动面全部有 fail-closed 默认与五自检护栏，downside bounded）。
> **review profile**：G2+G3（触面 = integrator.md / pod-coordination.md G2 + merge-driver.mjs / pre-merge-escalation-gate.mjs / dev-loop.settings.json / lib 新文件 G3；契约面为零）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scripts/merge-driver.mjs
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/references/pod-coordination.md
.claude/agents/integrator.md
.claude/dev-loop.settings.json
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：把 dev-loop 的「Pod 分支 → main」集成链从「流程外知识 + 逐次人批 + 单边消解可假绿」修成「合法执行者 + 成文消解规则 + 版本化制品冲突机械 fail-closed」——并行 Pod 的 merge 段不再靠会话读源码自救，丢正文一侧不再可能静默报绿。

**现状差距**（三条 ARQ 逐一实证，详见 §1）：① 集成期冲突消解没有合法执行者（integrator allow 10/10 全覆盖但 charter 明写「不 merge」，实际执行被 harness 权限层拦停过一次，human 一次性授权 `dc9a5c15` 才走通）；② 版本化制品 `.vN` 槽位可被并行 Pod 同时占用，add/add 冲突两种单边消解都不安全，被 needle 齿偶然命中的那侧**假绿**（schedule-reminder 实测：取先入 main 侧 → 本包冻结齿仍 PASS 而 RMD-A8 约定正文已从生产 prompt 消失）；③ 集成期实测陷阱（CRLF 物化翻红承重闸 / graph 缓存拦 merge / 消解后只跑本包档漏对端红 / Pod 分支含 merge commit 与 driver rebase 结构性不兼容）全靠执行会话现查现解。

**非目标（本批不做）**：
- 不给 dev-loop 任何角色开 `docs/spec.md` / `docs/CONTRACT-DECISION-QUEUE.md` / `docs/contracts/**` 写权——面 B **只加核验、不加写权**（ARQ 原文红线：开写权会把契约门侵蚀成「谁发现谁改」）；
- 不动 supervisor 判据 / 不改升级闸 cross-system 判据语义（那是 Track A 的 escalation-cross-system-overfire）；
- 不做版本化守卫的规约面改法（版本无关 green-guard 撰写约定归 versioned-guard ARQ，Track A / OQ1）；
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
- **并证二补②（双源）**：`approvalRefUnlock`（:915-921，本会话亲核确认仍在）以 `probeWt = findWorktree(tp, projectDir) || projectDir` 作 cwd、gate 参数却传 `--project-dir projectDir`——两源解析到不同主体时 `--source-ref HEAD` 在错误 worktree 求值。可自证的是代码形状，触发条件与修复口径交评审期判。

### P2 · 版本化制品槽位可并发占用，单边消解方向不对称的假绿

ARQ-260908-versioned-artifact-slot-collision 实测（同包收口集成探测）：两并行包各自把 prompt v2→v3，`git merge-tree` 实测 CONFLICT (add/add) 5 冲突块、接线面共 8 条；仓外副本三消解对照——取先入 main 者**假绿**（needle「备注」在故事版被课表段偶然命中计数 2/1）、取本包者对端 RED、取并集双方全绿（8 failed 既有红同名单 / 155 passed）。既有冲突口径 eventId `5697792c`「以先入 main 者为准」在本案恰产出假绿支——该口径对重复修复成立、对不同正文抢同槽位不成立。**根因**：「谁占哪一版」无机械登记（两包各自看 deploy 目录猜）+ 合并相位无版本化制品特判（交人随手解）+ 消解侧无双包齿复跑纪律。

### P3 · scripts 承重闸无 lib 分离，跨脚本复用只能 spawn

ARQ-260907-gate-lib-script-separation（260907-gov-review-merge-friction retro F3）：gate 无 import.meta 守卫、顶层直跑 + process.exit——merge-driver T1 复用五要素核验只能 spawn（双进程开销 + 以 merge-driver argv 误判 changeId 的语义耦合）。本批大量动 merge-driver 与 gate，是落实分离的顺带时机（OQ2）。

## 2. 改法（T1…T6）

### T1 · merge-driver：版本化制品 add/add 特判 + 投影消解 LF 归一 + approvalRefUnlock 双源修复（G3）

1. **版本化制品 add/add fail-closed 特判**：rebase/merge 冲突集检测到路径匹配版本化制品形态（`*.v[0-9]+.md` 及 `.claude/references/pod-coordination.md` §10c 登记表所列路径）且冲突类型为 add/add → 不进入通用 blocked 消息，输出显式指引：「版本化制品 add/add——禁 `--ours/--theirs` 单边消解（丢正文侧可能被 needle 齿偶然命中而假绿），消解 = 取并集 + 复跑**双方** carrier 齿并留 exit code」，置 blocked（消解规则预登记后按 §10c runbook 走）。与投影特判（:828）同层实现，白名单可扩。
2. **投影消解 LF 归一**：T3 投影特判 `git checkout --theirs` 后对物化文件做 CRLF→LF 归一再落盘（修陷阱 (a) 的 driver 侧半边），自检夹具补行尾维度断言（canonical ↔ current sha256 相等）。
3. **`ensureGraphCacheSynced` 导出**：命名导出（陷阱 (b)），pod-coordination runbook 引用之，人工/integrator 消解不再复刻三拍。
4. **approvalRefUnlock 双源统一**：`probeWt` 与 gate `--project-dir` 参数统一为同一解析主体（评审期定口径：以 probeWt 为准传参，或两者恒用 projectDir——按 gate 的 off-main 判定语义取正确者），并补一条自检断言钉住「cwd 与 --project-dir 同源」。
5. 五自检全量复跑通过为 landing 前置。

目标文件：`.claude/scripts/merge-driver.mjs`

### T2 · pod-coordination.md：版本化制品槽位预约登记 + §10 runbook 三条补齐（G2）

1. **§10c 版本化制品 `.vN` 占用登记**（新节）：Pod launch 时在 pod-events 记 `versioned-artifact-claim: <path>@vN`；后到 Pod 见在先 claim 即从 vN+1 起编或等前包合并后重取基线；登记表随批维护（当前清单 = `server/deploy/openclaw-system-prompt.vN.md`，同形态风险 = engine-patch / overlay 配置）。
2. **§10a runbook 补三条**：① 形态钉死——「Pod 分支集成一律 rebase 形态、禁在 Pod 分支引入 merge commit」（并证二①的驱动契约句）；② graph 缓存三拍引用 T1 导出的 `ensureGraphCacheSynced`；③ current/ 投影取被并包侧**且归一 LF**（sha256 逐字节比对语义提醒）；④ 消解完成后复跑**双方**冻结件 + 全档 + host ELF（文档命令位）+ 设备真编 + 九闸脚本并回报实测数字（陷阱 (c) 对端红纪律）。
3. 与 `ARQ-260907-merge-closeout-bookkeeping-gap` 候选④已落地的 §10a/§10b 无缝衔接（本条只补增量，不重写既有 runbook）。

目标文件：`.claude/references/pod-coordination.md`

### T3 · integrator.md「merge-chain execution」节（G2，self-def 存疑点申报见头注）

正文新增一节，边界钉死（原文写入 def）：**仅 Pod 分支**（分支名匹配 `dev/<changeId>`）、**永不 push**、**永不 merge 进 main**（main 侧合并恒归 planner 主线程 merge-driver 路径——与既有 :12/:74 语义一致，本节是把「集成期 Pod 分支上的 rebase/冲突消解」从默认禁止里显式豁出并加前置）、不改 `*.acceptance.*` / `docs/contracts/**` / `docs/spec.md`、**消解规则必须预登记**（planner 派单逐文件写死 + human 授权事件 id 在场，缺一即停并回报）、集成树验证回报义务（T2 ④ 同款清单）。fs-write 类闸产物（supervisor-verdict.json）的收口提交归属一并写入本节（并证二③）。

目标文件：`.claude/agents/integrator.md`

### T4 · dev-loop.settings.json 条件化 git 条目（G3）

`.claude/dev-loop.settings.json` 补 `git merge` / `git rebase` 的边界条目：integrator 在 Pod worktree 内、有预登记消解规则 + human 授权事件引用时 ask/allow；无引用即 deny。使 T3 的成文边界有机械层呼应，不再「靠分类器放行侥幸」。（ARQ 原文写 `.claude/settings/dev-loop.settings.json`，实际路径 = `.claude/dev-loop.settings.json`，本批按实况修正。settings 具体形态——ask vs 条件 allow 的表达力边界——交评审期按 hooks 能力定，形态若不可表达则降级为「deny + runbook 指引」并在留根注明。）

目标文件：`.claude/dev-loop.settings.json`

### T5 · 面 B：契约/治理 canonical 改动「交回→在册」机械核验（G3，gate 事实申报）

`pre-merge-escalation-gate.mjs` 加一条核验：review-packet Findings 中 `Kind=hard-violation` 且 Basis 引 `docs/contracts/**` / `docs/spec.md`（或 severity ≥ SHOULD 的 contract/boundary 类）者，MUST 在 `docs/CONTRACT-DECISION-QUEUE.md` 有对应 CDQ 行、或有 human 显式 defer / accepted-risk 记录（eventId 锚），否则红。**只加核验、不加写权**（§0 非目标红线）。与 `ARQ-260827-contract-mech-gate-gap`（pass-oracle 第 4 关无脚本兜底，人工核对降级）同族——本条是该缺口在 dev-loop 侧的第一块机械补丁，不宣称补齐第 4 关。

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`

### T6 · gate 可导入库分离（OQ2 human 裁随批，确认在批）

gate 五要素核验共享函数（`resolveHumanApproval` 等）提取至 `.claude/scripts/lib/governance-gate-shared.mjs`（既有先例同名 lib），gate 薄壳化 + `import.meta` 守卫；merge-driver T1 的 spawn 复用改为 import（消双跑 + 语义耦合字面复用）。gate 判定语义零变更（纯结构重构），自检夹具双路径复跑钉住等价。

目标文件：`.claude/scripts/pre-merge-escalation-gate.mjs`、`.claude/scripts/lib/governance-gate-shared.mjs`（新增）、`.claude/scripts/merge-driver.mjs`

### T7 · 版本化制品 green-guard 撰写约定（OQ1 裁并批承载 versioned-guard ARQ；规约面骨架，正文归 architect 评审循环细化）

按 ARQ-260908-versioned-guard-version-literal-coupling 候选改法落两条：① **版本无关接线守卫约定**成文——「版本化制品的接线类 green-guard 必须：解析当前被接线/渲染的那一版文件 → 断言该文件确实存在（防孤儿）→ 断言其正文逐字携带本包语义锚点（防未来版丢内容）；禁在断言里硬写版本字面文件名」，落点候选 = `.claude/references/acceptance-evidence.md`「必要证据」段 / `task-intake-and-slicing.md` 验收断言格式段 / `docs/DEV-GOVERNANCE.md` 验收测试纪律节（三选一归评审期裁）；② **全仓既有字面耦合守卫清点**随批执行：`grep -rn "openclaw-system-prompt.v[0-9]"` 于验收断言字面处逐条判「接线守卫（该版本无关化）vs 落盘/措辞守卫（钉死某版合理）」——在册待判实例：char-stroke CHR-E3 已首例重指（human 裁 `c54bbcde`）、schedule_p2 P2-A6 `:583-586` 现属假绿、voice_roundtrip VRT `:116` 钉 `.v1`、live 齿 `PROMPT_V2_PATH` 常量（:618/:657）；反例免误改：`VRT-E4`（裸名 + 无扩展名 carrier，天然版本无关）。③ **既有守卫重指**触他人冻结件 → 逐件 human 授权，本批只登记清单与规约、不预批任何冻结件改动（与 §0 非目标同款纪律）。工程门判据侧（architect-reviewer 预审/verifier 增列「字面版本耦合」检查）是否设门归评审期裁。

目标文件：三选一规约落点（评审期定）+ `docs/ARCH-REVIEW-QUEUE.md`（versioned-guard 行销账，随批 landing）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明（消费方） |
|---|---|---|---|
| 合并机械链 | `.claude/scripts/merge-driver.mjs` | G3 行为扩展（add/add 特判 + LF 归一 + helper 导出 + 双源修复）= gate 事实，如实声明交判 | planner 主线程 merge 段；§10a runbook |
| 协调 runbook | `.claude/references/pod-coordination.md` | G2 新节 §10c + §10a 补三条 | planner / integrator / human 消解现场 |
| agent def | `.claude/agents/integrator.md` | G2 新节（self-def 存疑点，判官复核） | integrator 集成段；planner 派单 |
| dev-loop 平台设置 | `.claude/dev-loop.settings.json` | G3 条件化条目 | harness 权限层 |
| 升级闸 | `.claude/scripts/pre-merge-escalation-gate.mjs` | G3 新核验行（gate 事实） | 合并门前置；CDQ 台账（只读消费） |
| 共享 lib | `.claude/scripts/lib/governance-gate-shared.mjs` | G3 新增（OQ2 已裁随批） | gate / merge-driver 双消费 |
| 版本守卫规约（T7） | 落点三选一待评审期裁（acceptance-evidence.md / task-intake-and-slicing.md / DEV-GOVERNANCE.md）+ 既有守卫重指清单在册 | G2 成文 + 他人冻结件重指逐件 human 授权（不预批） | acceptance-author 撰写约定；verifier/architect-reviewer 判据侧是否设门归评审期裁 |
| 契约面 | `docs/contracts/**` | **未触（零）** | — |
| `docs/spec.md` / CDQ | — | **未触（零写权）** | T5 只读核验 |
| pod-events schema | — | **未触** | `versioned-artifact-claim` 用自由事件型 |
| 队列销账 | `docs/ARCH-REVIEW-QUEUE.md` | 簿记（两条 260907 行结移 §2，随批 landing 执行） | — |

**【硬项 1b · sweep 透镜义务】** 承载面现跑核验（2026-09-08 主线程）：① `approvalRefUnlock` 双源形状亲核（merge-driver.mjs:915-921 `probeWt` vs `--project-dir projectDir` 在场）；② `.claude/settings/` 目录不存在、dev-loop 设置实为 `.claude/dev-loop.settings.json`（ARQ 路径笔误已修正）；③ `.gitattributes` 现有两条（`*.sh eol=lf` / `progress.md merge=union`），本批无新增 attributes 需求；④ integrator 现行边界句 :12/:54/:73/:74 四处亲核，T3 为增量新节、不改既有句。

**【硬项 1c · 复扫模式完备性先验】** 不适用：本批无退役语面/残留清扫（全部为增量新节/新分支/新条目）。

## 4. human 取舍点（OQ）

- **OQ1 · versioned-guard-version-literal-coupling（Track A）是否并批立项**：(a) 分批；(b) 并批走 Track A。—— **human 2026-09-08 裁 (b)**：并批，整批升 Track A，T7 承载。
- **OQ2 · gate-lib-script-separation 是否随批**：(a) 随批；(b) 留下批。—— **human 2026-09-08 裁 (a)**：随批，T6 确认在批。
- **OQ3 · T4 settings 条件化 git 的严格档**：(a) ask（推荐）；(b) 条件 allow；(c) deny + runbook。—— **未裁，归评审期/decision packet**（head 判定需 hooks 表达力核实，先随批进评审）。
- **OQ4 · escalation-cross-system-overfire（Track A）启动时机**：(a) merge 批 ack 后即启；(b) 攒样本后再开；(c) 接受恒批记 watchlist。—— **human 2026-09-08 裁 (a)**：本批 ack 后即启，单独 Track A 批另行立项（不并入本批改法面）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

- **V1 · merge-driver 五自检复跑**：`--self-check-retro-advisory / quick-checks / detect-changes / rebase-target / worktree-topology` 全绿；新增四夹具——①版本化制品 add/add → 显式指引 + blocked（非通用 blocked 文案）；②投影消解后 canonical ↔ current sha256 相等（LF 归一断言，夹具含 autocrlf 场景）；③approvalRefUnlock 双源统一断言（cwd ≡ --project-dir 解析主体）；④`ensureGraphCacheSynced` 导出可 import。
- **V2 · gate 核验行两态夹具**（真空绿规约：fixture 非空断言先行）：packet 含 contract 类 SHOULD finding 且 CDQ 无对应行 → exit 1 点名；有 CDQ 行 / human defer eventId 锚 → 放行。若 OQ2=(a) 再跑 gate 五要素核验的 lib-import 等价夹具（spawn 与 import 双路径同结果）。
- **V3 · settings 条件化探针**：按 OQ3 裁断值模拟——Pod worktree 内 `git rebase`（无授权引用）→ deny/ask；有授权引用 → 按裁断值放行。
- **V4 · 投影 grep（正负双断言）**：正：`grep -c "merge-chain execution" .claude/agents/integrator.md` ≥1；`grep -c "10c" .claude/references/pod-coordination.md` ≥1；`grep -c "versioned-artifact-claim" .claude/references/pod-coordination.md` ≥1。负：`grep -n "docs/contracts" .claude/dev-loop.settings.json` → 零命中（写权红线）；integrator def 中「永不 push / 永不 merge 进 main」逐字在场。
- **V5 · 队列销账核验**：landing 时 `docs/ARCH-REVIEW-QUEUE.md` 中两条 260907 merge ARQ 行已结移 §2、§2 行含 `260907-gov-review-merge-friction` 承载批引用；`node .claude/scripts/changes-index.mjs --upsert 260908-gov-review-merge-chain-hardening` 单行钉入 INDEX。
- **V6 · end-to-end 演练（landing 前置）**：在临时仓夹具复刻「双包抢 v3 槽位」最小场景——A 包 bump v3（含齿 X）→ 并 main → B 包 bump v3（含齿 Y）→ driver 跑 → 断言：add/add 特判触发、并集消解后 X 与 Y 双齿绿、全程无 `--ours/--theirs` 单边提交。（模板 = schedule-reminder review-packet §Integration Probe 实测数据。）

## 6. 留根清单

- **escalation-cross-system-overfire Track A 批**（OQ4 裁本批 ack 后即启）：升级闸 cross-system 判据收紧 vs 接受恒批，另行立项走 `/arch-change`，不占本批改法面。
- **pass-oracle 第 4 关完整兜底**：T5 只补 dev-loop 侧核验，spec↔contract 全量机械门仍缺（ARQ-260827-contract-mech-gate-gap 主体未销）。
- **HOST_ACCEPTANCE_SRCS 顺序承重根治**：seam reset() + boundary 齿自建新鲜 seam，属固件测试基建，归 dev-loop 包承接（closeout-bookkeeping ARQ 的「结构性根治另线」原文保留）。
- **pod-events `versioned-artifact-claim` 事件型收编**：自由事件型先跑，schema 最小校验留观测后批（friction-event-payload-schema ARQ 同族）。
- **settings 条件表达力核实**：T4 若 hooks 条件匹配表达不了「授权事件 id 在场」语义，降级形态与理由在 landing-summary 如实记录。
- **销账动作执行面**：两条 260907 行的结移由本批 landing 步机械执行（V5），不改写其证据链。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）
