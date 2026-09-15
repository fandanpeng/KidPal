# PROPOSAL · 260914-gov-review-delivery-restructure

> **状态：FROZEN v1.1 · 2026-09-14 · 凭据：human 立项 ack（2026-09-14：三 OQ 裁定 + R1 扩围裁定 + T6 扩围裁定 + KidPal-claude 不扩裁定）+ architect 第 4 轮终审 GREEN（R17，四轮收口记录见 §7）+ human 裁 OQ 结果：OQ1(c) 契约面改地址不升轨 / OQ2(a) 证据件一并批改 / OQ3(a) 图排除 delivery/。**v1.1 透明小修（human 2026-09-14 讨论澄清）**：§0/§6 演进定位由「整体迁出」改为「收口即迁、活跃态留仓」，零 landing 面变更（T1–T6 / manifest / V1–V6 不动），GREEN 凭据不受影响。landing 等 human 通知——main 侧改动 sync 后再落，避免合并冲突（human 2026-09-14 指示）**。
> **承载 ARQ**：无（human 直接发起；概念讨论脉络见 §0 与 §4 OQ1 前置讨论记录）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。Track A 不触发——OQ1 human 裁 (c)：契约面 3 处指针批改、不升轨（2026-09-14 立项裁定 + 同日扩围裁定，升级判据 ① 由 human 对本件的零语义地址修正显式豁免）；契约门仍走 architect-reviewer 四分类 + human 签字（已预签，见 §4 OQ1 / T6 / §7 预审记录）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T5）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（触点面 142 manifest 锚〔判官 R14 comm 对账口径，R1 扩围后〕为全仓治理结构重组之最，但全部为路径平移、行为保持——脚本逻辑与门语义零变更；契约面触点 = 3 行零语义指针修正，human 已裁不升轨）。
> **review profile**：G2+G3（G2：dev-loop 工作面结构 taskpackages/workstreams；G3：治理机械层 scope-map/hooks/scripts 与治理文档面）。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/scope-map.json
.claude/dev-loop.settings.json
.claude/route-fixtures/surface-alignment.json
.claude/hooks/commit-safety.mjs
.claude/hooks/enforce-scope.mjs
.claude/hooks/planning-sync.mjs
.claude/hooks/portfolio-sync.mjs
.claude/hooks/suggest-codebase-memory.mjs
.claude/hooks/supervisor-gate-check.mjs
.claude/hooks/supervisor-verdict-gate.mjs
.claude/hooks/verify.mjs
.claude/scripts/acceptance-coverage-check.mjs
.claude/scripts/arch-review-handoff-check.mjs
.claude/scripts/brief-inputs.mjs
.claude/scripts/changes-index.mjs
.claude/scripts/closeout-check.mjs
.claude/scripts/delivery-status.mjs
.claude/scripts/devloop-artifact-check.mjs
.claude/scripts/dispatch-stamp.mjs
.claude/scripts/docs-sync-check.mjs
.claude/scripts/journal-append.mjs
.claude/scripts/landing-fidelity-check.mjs
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/loop-health-report.mjs
.claude/scripts/loop-metrics.mjs
.claude/scripts/merge-driver.mjs
.claude/scripts/mutation-check.mjs
.claude/scripts/planning-index.mjs
.claude/scripts/pod-events-aggregate.mjs
.claude/scripts/pod-events-append.mjs
.claude/scripts/portfolio-reconcile.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/scripts/proposal-consolidation-check.mjs
.claude/scripts/render-roadmap.mjs
.claude/scripts/security-check.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/scripts/verifier-evidence-check.mjs
AGENTS.md
.claude/rules/repo-structure.md
.claude/rules/prompt-authoring.md
.claude/agents/adjudicator.md
.claude/agents/architect-reviewer.md
.claude/agents/architect.md
.claude/agents/challenger.md
.claude/agents/defender.md
.claude/agents/integrator.md
.claude/agents/landing-verifier.md
.claude/agents/planner.md
.claude/agents/portfolio-planner.md
.claude/agents/review-moderator.md
.claude/agents/scenario-author.md
.claude/commands/arch-change.md
.claude/commands/build-feature.md
.claude/commands/loop-retro.md
.claude/commands/review-arch.md
.claude/commands/review-governance.md
.claude/commands/triage-change.md
.claude/references/arch-review-protocol.md
.claude/references/arch-scenarios.md
.claude/references/architect-operating-model.md
.claude/references/architecture-decisions.md
.claude/references/change-triage.md
.claude/references/dev-loop-operating-system.md
.claude/references/dev-loop.md
.claude/references/pod-coordination.md
.claude/references/task-intake-and-slicing.md
.claude/references/taskpackage-authoring.md
.claude/references/track-b-verifier-brief.md
.claude/references/plain-file-handoff.md
.claude/skills/arch-doc-restructure/SKILL.md
.claude/skills/brainstorming/SKILL.md
.claude/skills/gov-arq/SKILL.md
.claude/skills/kidpal-pod-launch-prep/SKILL.md
.claude/skills/merge-to-main/SKILL.md
.claude/skills/opendesign-connect/SKILL.md
docs/requirements.md
docs/architecture.md
docs/DEV-GOVERNANCE.md
docs/ARCH-REVIEW-QUEUE.md
docs/ADR.md
docs/notes/ui-implementation-strategy.md
docs/notes/validation-batch-conclusion.md
docs/templates/GOVERNANCE-MIGRATION-GUIDE.md
docs/templates/PROPOSAL.template.md
docs/templates/WORKSTREAM-PROPOSAL.template.md
docs/templates/WORKSTREAM-ROADMAP.template.md
docs/templates/architect-landing-summary.template.md
docs/templates/retro.template.md
firmware/main/test/HOST_TEST_CONVENTIONS.md
server/deploy/README.md
server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md
firmware/main/test/acceptance/red-baseline/red-baseline-summary.char-stroke.md
firmware/main/test/acceptance/red-baseline/red-baseline-summary.md
firmware/main/test/acceptance/red-baseline/schedule-p2-red-baseline-summary.md
firmware/main/test/acceptance/red-baseline/schedule-reminder-red-baseline-summary.md
firmware/main/test/acceptance/red-baseline/screen-unification-red-baseline-summary.md
firmware/main/test/acceptance/red-baseline/story-p3-red-baseline-summary.md
server/evidence/260903-gateway-directionB/cert-lifecycle-accept-risk.md
server/evidence/260903-gateway-directionB/deploy.md
server/evidence/260903-gateway-directionB/v21-multiturn-continuity-probe.md
server/evidence/260903-gateway-directionB/v23-asr-final-selection.md
tools/asset-pipeline/tests/test_art_assets.acceptance.md
tools/simulator/tests/test_lvgl_align.acceptance.md
firmware/main/test/char_stroke.acceptance.c
firmware/main/test/device_ws_switch.acceptance.c
firmware/main/test/happy_flow_capture.test.c
firmware/main/test/hardware_validation.acceptance.c
firmware/main/test/key_route_a1.acceptance.c
firmware/main/test/schedule_reminder_b.acceptance.c
firmware/main/test/screen_unification.acceptance.c
firmware/main/test/ui_art_integration.acceptance.c
firmware/main/test/v3_restore.acceptance.c
firmware/main/test/voice_roundtrip_b.acceptance.c
server/acceptance/test_char_stroke.acceptance.py
server/acceptance/test_gateway_directionB.acceptance.py
server/acceptance/test_g_relay_fix.acceptance.py
server/acceptance/test_schedule_p2.acceptance.py
server/acceptance/test_schedule_reminder.acceptance.py
server/acceptance/test_smoke_e2e.acceptance.py
server/acceptance/test_story_p3.acceptance.py
server/acceptance/test_story_wiring.acceptance.py
server/acceptance/test_voice_roundtrip_b.acceptance.py
.gitattributes
delivery/milestones/delivery-workspace.json
delivery/milestones/milestones.jsonl
delivery/workstreams/README.md
delivery/workstreams/INDEX.md
delivery/workstreams/index.jsonl
delivery/milestones/STATUS.md
delivery/changes/INDEX.md
delivery/taskpackages/current/input-brief-acceptance-author.md
delivery/taskpackages/current/input-brief-builder.md
delivery/taskpackages/current/taskpackage.json
delivery/taskpackages/current/verdict.json
.cbmignore
.claude/.cbmignore
docs/contracts/interface-contract.md
```

> 迁移锚只记迁入路径（四目录根）。OQ2 已裁 (a)：firmware/server/tools 证据件 12 行为无条件锚。`docs/contracts/interface-contract.md` 为 T6 契约面指针锚（OQ1 裁 (c)）。评审 R2 补锚 4 件：`portfolio-planner.md`、`plain-file-handoff.md`、`docs/architecture.md`、`milestones.jsonl`。**R1 已裁扩围（human 2026-09-14「同意推荐」）**：代码面 20 件（firmware 10 `.c` + server 9 `.py` + `.gitattributes`）已补入锚。**landing 期结构修正（2026-09-15，landing-fidelity-check fail-loud 所迫、如实记档于 landing-summary §3-10）**：① 四目录根迁入锚剔出 manifest（git diff 不产目录级变更、不可机判；T1 改整裁腿空锚）；② `docs/CONTRACT-DECISION-QUEUE.md` 剔出（其 census 命中经核全为已结行历史行、零落地件——判官 R14 所述「manifest 侧无命中正当」件之一）；③ 各 T 补「目标文件：」锚清单行（模板硬项 2a，起草期遗漏、四轮评审未捕获，retro 收录）。

---

## 0. 想实现的效果与差距

**目标效果一句话**：把散装在四个顶层（其中两个是隐藏点目录）的交付链路收拢进单一 `delivery/` 屋檐，按「范围怎么拆」组织成四层——milestones（结果面）→ workstreams（拆解面）→ taskpackages（在途执行面）→ changes（归档面）；时间维度不进结构、沉到数据里。

**演进定位（human 2026-09-14 注记 + 同日讨论澄清 v1.1）**：交付工件住进代码库本身是**临时安排**——只因专门管交付的系统尚在开发中。但迁出模式不是「整目录抬走」，而是**收口即迁、活跃态留仓**：`changes/` 等交付面装着两种生命周期不同的东西——① **在途面**（PROPOSAL 草稿、评审记录、gate evidence）是治理流程的工作内存，被仓内机械层实时读写（scope-map 写槽、changes-index、landing-fidelity-check），**必须留仓**，否则 Track B 闭环就得与外部系统强耦合；② **收口面**（闭环后的四件制品、已晋升包、归档 workstream）是审计记忆，闭环最后一步推交付系统归档、仓内收缩为薄指针（INDEX 留 change-id → 系统 ID 映射）。一言以蔽之：**仓库管正在发生的事，系统管发生过的事**；仓内活区恒薄。本变更随之立一条边界纪律：代码面（firmware/server/tools）不得新增对 `delivery/**` 的构建期/运行期依赖（文档指针不计），spec 面对它的引用保持「可替换为系统 ID」的形态。

**现状差距**（2026-09-14 实测）：

- 交付面顶层四散：`.delivery/`（里程碑注册表）、`.planning/`（workstream 工作记忆）、`.claude/taskpackages/`（在途验收包）、`changes/`（收口归档）互不统属，前两个还是隐藏目录，新 agent 无法从目录名读出交付链路。
- 验收包住在 harness 配置面：`.claude/` 按 [repo-structure.md](../../../.claude/rules/repo-structure.md) 顶层地图的定义是「治理机制：agents/rules/hooks/scope-map/settings/scripts/references」，taskpackages 是交付工件却长期寄居其下，且因 `.claude` 整目录被 CodebaseMemory 工具硬排除而意外不入图——位置与属性双错位。
- 概念层有一处待钉死的前置讨论结论：PI（Program Increment，SAFe 时间盒）不适配 AI 开发节奏（速度与范围双输入皆噪声，时间盒无锚点），**不引入 PI 层**；范围维度用 Milestone → Workstream → TaskPackage 三层表达。

**非目标**：不引入 PI / 时间盒结构；不改 workstream 准入判据、front-matter 结构与归档机制行为（planning-sync 钩子只改路径常量）；不改任何脚本 / 门的判定逻辑；不动历史面文件内容（含 `_archive/`、`changes/{arch,dev,retro}/` 既有制品、已晋升 taskpackage 件）；不动 `.codebase-memory/`；不动 `docs/contracts/**`（除非 OQ1 裁 (b)）。

## 1. 问题与根因（地面真值）

**P1 · 结构性错位（根因 = 治理迁移遗留布局，无单点 bug）**

- 证据（2026-09-14 `ls -la` 实测）：顶层交付面 = `.delivery/`（dot-dir）、`.planning/`（dot-dir）、`.claude/taskpackages/`（寄居配置面）、`changes/`（明目录），四者无共同父级。
- 证据（repo-structure.md 顶层地图 `.claude/` 行）：「治理机制：agents / rules / hooks / scope-map / settings / scripts / references」——taskpackages 不在枚举内却实际存在（实测 24 个包目录 + `current/`）。
- 证据（`.delivery/delivery-workspace.json:19` verbatim）：`"planRoot": ".planning"`——交付注册表与规划面互相指认，却分居两个隐藏目录。

**P2 · 引用面广（迁移成本主体，机械可枚举）**

起草期 census（2026-09-14，活文件 = 排除 `_archive/`、`docs/archive/`、`changes/{arch,dev,retro}/`、`.claude/taskpackages/` 既有包件、`.codebase-memory/`、`ESP32-S3-RLCD-4.2-Demo/`）：

```
grep -rl '\.planning' . --include='*.md' --include='*.json' --include='*.mjs' --include='*.jsonl' | grep -vE '_archive|/archive|changes/arch/|changes/dev/|\.codebase-memory|ESP32-S3-RLCD'
grep -rl 'taskpackages' .（同上过滤，另滤 .claude/taskpackages/ 自身）
grep -rlE 'changes/(arch|dev|retro|INDEX)' .（同上过滤）
grep -rl '\.delivery' .（同上过滤）
```

关键计数（评审 R5 复测修正，原稿 27/43 为漏锚前口径；评审 R11 歧义澄清）：单 pattern 口径——`.claude` 机械面 29 件含 `.planning` 形、44 件含 `taskpackages` 形；括号分项（8 hooks、26 scripts、11 agents、6 commands、12 references、6 skills、2 rules，并集 71 件）为**四 pattern 并集的去重清单**，与单 pattern 数有交集、不可相加比较。活文档面命中 AGENTS.md、docs 8 件（含 architecture.md）、templates 6 件、firmware/server 活文档 3 件；firmware/server/tools 证据件 12 件（OQ2 已裁一并批改）；**census 扩展名盲区（评审 R1 发现）**：不限扩展名复跑另命中代码面 20 件——firmware 10 件 `.c`（全注释）、server 9 件 `.py`（docstring/断言消息串 + **功能性路径常量**：`PACKAGE_EVIDENCE_DIR = CHANGES_DEV / "260903-gateway-directionB"`、`LIVE_EVIDENCE_DIR = CHANGES_DEV / "260907-g-relay-fix"`，测试运行时实读证据目录）、`.gitattributes` 1 件（`merge=union` 属性路径）——human 已裁扩围批改（§3）。完整枚举清单 = 本件 landing-manifest（census 命中集与锚清单一一对应，判官 R14 comm 对账：142 锚，sweep 侧仅 ledger.jsonl 一件已声明豁免、manifest 侧 10 件无命中均正当）。

**P3 · 契约面擦边（升级判据 ① 的边界情形，交 OQ1）**

证据（2026-09-14 `grep -n 'changes/' docs/contracts/interface-contract.md`，2 处命中，均为历史审计指针、零活语义）：

- 行 5：>`changes/arch/260902-arch-review-shim-api-field-envelope/CONTRACT-CHANGE-PACKET.v1-http-field-envelope.superseded.md`（被取代信封草案的存档指针）
- 行 96：>（契约变更记录：2026-08-29 经契约门批准新增 `total`，additive 可选字段，见 changes/arch/260829-arch-review-kidpal-spec-stories。）

根因：`changes/` 承载契约变更历史记录的引用，目录迁移会使这两处指针悬空。**human 2026-09-14 裁 (c)：批改地址、不升 Track A**——零语义修正随本变更走轻量契约门（见 T6 / OQ1）。另：architect-reviewer 预审通读时发现行 4 存在更早的过期指针（指向已归档 `docs/spec.md`，属 `changes/` grep 模式之外），human 同日裁定「顺手一起改了」并入 T6。

**P4 · append-only 日志件的行级豁免需求**

证据（实测命中行）：`docs/requirements.md` 行 610 / 675 为活文本（「完成状态核实 = changes/INDEX.md dev 段」「可溯 changes/arch/**」）须批改；行 688 / 729 为版本历史既有行（纪律：行只追加、不覆盖）不动。`docs/ADR.md`、`docs/ARCH-REVIEW-QUEUE.md`、`docs/CONTRACT-DECISION-QUEUE.md` 同为日志件，历史行内旧路径按同一原则不动、活段批改。

## 2. 改法（T1–T5）

**路径映射总表（全部 T 共用）**：

| 旧 | 新 |
|---|---|
| `.delivery/` | `delivery/milestones/` |
| `.planning/` | `delivery/workstreams/` |
| `.claude/taskpackages/` | `delivery/taskpackages/` |
| `changes/` | `delivery/changes/` |

### T1 · 四件目录 git mv 迁移（历史面内容零改动）

- `git mv .delivery delivery/milestones`；`git mv .planning delivery/workstreams`；`git mv .claude/taskpackages delivery/taskpackages`；`git mv changes delivery/changes`。删除 `delivery/taskpackages/.gitkeep`（目录已非空）。
- 历史面 = `delivery/workstreams/_archive/**`、`delivery/changes/{arch,dev,retro}/**`、`delivery/taskpackages/` 下 24 个既有包件（其中 22 个 change-id 已在 changes/dev 有晋升件，实测 `comm` 交集）——**只搬不改**，件内指向旧路径的引用留档。
- 未晋升两包（260829-audio-bargein-hw、260829-shim-skeleton）与 `current/` 同为搬移对象；仅 `current/` 件内容随后由 T4 批改（活工作面），两未晋升包按历史件处理（其包内 `.planning` 引用留档——影响为包内 workstream 指针 stale，读档时按当时语境理解）。
- 前置条件：迁移窗口内无在途 Pod worktree（Pod 分支基于旧路径，rebase 会大面积冲突；landing 前 human/主线程确认）。
- 对应根因：P1。
目标文件：无（目录迁移整裁腿——四目录根为迁入锚、git diff 不产目录级变更不可机判；迁入内容件的活内容锚归 T4，历史面件零改动不产锚）

### T2 · 机械强制层路径批改（scope-map / hooks / scripts / settings，与 T1 同一 commit）

- `scope-map.json`：各 agent 条目 allow/deny glob 全量按映射表替换（如 `.claude/taskpackages/**` → `delivery/taskpackages/**`、`changes/dev/**` → `delivery/changes/dev/**`）；`universal.bashDenyWrite` 增补 `delivery/taskpackages/**`（原 `.claude/**` 红线对验收包的覆盖随迁出而消失，此处平移补齐）；`_bashDenyWrite_doc` 注记措辞同步。
- 8 个 hooks + 26 个 scripts（清单见 manifest）：路径常量、提示文案（enforce-scope.mjs:61 的「过程产物→ .claude/taskpackages/<id>/…」句）、生成目标（planning-index / delivery-status / changes-index 的输出路径）全量按映射表替换；逻辑零变更。
- **分段/字符串字面量形（评审 R10 补 + R15 封口——段值无关、引号无关）**：机械层与代码面路径常量的主流写法是逗号分段/引号拼接字面量，实测全家族：① 单引号逗号形 17 件（verify.mjs、changes-index.mjs、closeout-check、dispatch-stamp、journal-append、pod-events ×2、governance-gate-shared 等——comm 实证全部已在 manifest 锚集内）；② `'changes',` 后接**任意段**（不只 dev/arch/retro——实测 `'changes', 'INDEX.md'` ×4、`'changes', '.changes-index-state.json'` ×1，其中 closeout-check.mjs:147 不在 V1–V6 任何运行路径，漏改即 dev-loop 运行期才炸）；③ Python 双引号 join `"changes" / "dev"` ×8（server 7 件，含全部功能性 `LIVE_EVIDENCE_DIR` / `CHANGES_DEV` 常量）；④ C 双引号实参 `"changes"` ×1（device_ws_switch.acceptance.c:864，`wsa_scan_tree_both` 运行时实扫目录）。判官已核代码面无 `".planning"` / `".delivery"` / `"taskpackages"` 双引号实例——**家族就此封闭、无第五类**。映射规则：凡首段 = `'changes'` / `"changes"`（任意后续段）→ 前插 `'delivery', `；`'.claude', 'taskpackages'` → `'delivery', 'taskpackages'`；`'.planning'` → `'delivery', 'workstreams'`；`'.delivery'` → `'delivery', 'milestones'`。斜杠形漏改在 V2/V3 loudly 失败，字面量形漏改则运行期才炸（verify.mjs 钩子、closeout-check、pod-events、dispatch-stamp、journal-append 不在验证集运行路径上）——故字面量形以 V1 子检 2 兜底（见 §5）。
- `dev-loop.settings.json`、`route-fixtures/surface-alignment.json` 同步替换。
- 顺序敏感：scope-map 批改与 git mv 必须同 commit（避免窗口期 agent 写路径被拦/放行错位）。enforce-scope 不拦顶层目录创建（已核 scope-map `universal.denyCreate` 仅 `contracts/**`，enforce-scope.mjs:53 消费），`delivery/` 可直接创建。
- 对应根因：P1、P2。
目标文件：`.claude/scope-map.json` `.claude/dev-loop.settings.json` `.claude/route-fixtures/surface-alignment.json` `.claude/hooks/commit-safety.mjs` `.claude/hooks/enforce-scope.mjs` `.claude/hooks/planning-sync.mjs` `.claude/hooks/portfolio-sync.mjs` `.claude/hooks/suggest-codebase-memory.mjs` `.claude/hooks/supervisor-gate-check.mjs` `.claude/hooks/supervisor-verdict-gate.mjs` `.claude/hooks/verify.mjs` `.claude/scripts/acceptance-coverage-check.mjs` `.claude/scripts/arch-review-handoff-check.mjs` `.claude/scripts/brief-inputs.mjs` `.claude/scripts/changes-index.mjs` `.claude/scripts/closeout-check.mjs` `.claude/scripts/delivery-status.mjs` `.claude/scripts/devloop-artifact-check.mjs` `.claude/scripts/dispatch-stamp.mjs` `.claude/scripts/docs-sync-check.mjs` `.claude/scripts/journal-append.mjs` `.claude/scripts/landing-fidelity-check.mjs` `.claude/scripts/lib/governance-gate-shared.mjs` `.claude/scripts/loop-health-report.mjs` `.claude/scripts/loop-metrics.mjs` `.claude/scripts/merge-driver.mjs` `.claude/scripts/mutation-check.mjs` `.claude/scripts/planning-index.mjs` `.claude/scripts/pod-events-aggregate.mjs` `.claude/scripts/pod-events-append.mjs` `.claude/scripts/portfolio-reconcile.mjs` `.claude/scripts/pre-merge-escalation-gate.mjs` `.claude/scripts/proposal-consolidation-check.mjs` `.claude/scripts/render-roadmap.mjs` `.claude/scripts/security-check.mjs` `.claude/scripts/taskpackage-required-check.mjs` `.claude/scripts/verifier-evidence-check.mjs`

### T3 · 治理文档与活文档面批改

- 结构 SSOT：`.claude/rules/repo-structure.md` 顶层地图新增 `delivery/**` 行（四子区分别标注拥有者：milestones 主线程 / workstreams 人·主线程·dev-loop / taskpackages dev-loop / changes review-loop·dev-loop），「工作产物去哪」节改写为新链路；AGENTS.md 的 CodebaseMemory 表 `KidPal` 行排除列补 `delivery/`、`.claude` 排除列措辞去 taskpackages。
- agent 定义 11 件（评审 R2 补 `portfolio-planner.md`——三条 census 全命中、原稿漏锚）、commands 6 件、references 12 件（R2 补 `plain-file-handoff.md`，裸 `changes/` 形）、skills 6 件、rules 2 件：路径引用与提示句按映射表替换。
- docs 面：`requirements.md` 仅批改活文本行（610、675），版本历史行不动；`architecture.md`（行 276 仓结构列举句，R2 补锚）、`DEV-GOVERNANCE.md`、`ARCH-REVIEW-QUEUE.md`（活段）、`CONTRACT-DECISION-QUEUE.md`（活段）、`notes/` 2 件、`templates/` 6 件同步。
- 实现面活文档：`firmware/main/test/HOST_TEST_CONVENTIONS.md`、`server/deploy/README.md`、`server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md`。
- 代码面指针批改（评审 R1，human 2026-09-14 裁扩围）：firmware 10 件 `.c`（全注释指针）、server 9 件 `.py`（docstring / 断言消息串 + **功能性路径常量**——`CHANGES_DEV` 派生的 `PACKAGE_EVIDENCE_DIR` / `LIVE_EVIDENCE_DIR` 等，测试运行时实读 `changes/dev/` 证据目录，不改迁移后必红）、`.gitattributes`（`merge=union` 属性行更新至 `delivery/workstreams/_archive/2026/kidpal-mvp/progress.md`，顺修既有归档失配——原行指向归档前的 `.planning/kidpal-mvp/` 早已失配）——全部按映射表平移，代码逻辑零变更。
- 条件锚（OQ2 裁 (a) 时）：firmware 红基线摘要 6 件、`server/evidence/260903-gateway-directionB/` 4 件、tools 验收件 2 件——纯指针批改。
- 对应根因：P2、P4。
目标文件：`AGENTS.md` `.claude/rules/repo-structure.md` `.claude/rules/prompt-authoring.md` `.claude/agents/adjudicator.md` `.claude/agents/architect-reviewer.md` `.claude/agents/architect.md` `.claude/agents/challenger.md` `.claude/agents/defender.md` `.claude/agents/integrator.md` `.claude/agents/landing-verifier.md` `.claude/agents/planner.md` `.claude/agents/portfolio-planner.md` `.claude/agents/review-moderator.md` `.claude/agents/scenario-author.md` `.claude/commands/arch-change.md` `.claude/commands/build-feature.md` `.claude/commands/loop-retro.md` `.claude/commands/review-arch.md` `.claude/commands/review-governance.md` `.claude/commands/triage-change.md` `.claude/references/arch-review-protocol.md` `.claude/references/arch-scenarios.md` `.claude/references/architect-operating-model.md` `.claude/references/architecture-decisions.md` `.claude/references/change-triage.md` `.claude/references/dev-loop-operating-system.md` `.claude/references/dev-loop.md` `.claude/references/pod-coordination.md` `.claude/references/task-intake-and-slicing.md` `.claude/references/taskpackage-authoring.md` `.claude/references/track-b-verifier-brief.md` `.claude/references/plain-file-handoff.md` `.claude/skills/arch-doc-restructure/SKILL.md` `.claude/skills/brainstorming/SKILL.md` `.claude/skills/gov-arq/SKILL.md` `.claude/skills/kidpal-pod-launch-prep/SKILL.md` `.claude/skills/merge-to-main/SKILL.md` `.claude/skills/opendesign-connect/SKILL.md`
目标文件：`docs/requirements.md` `docs/architecture.md` `docs/DEV-GOVERNANCE.md` `docs/ARCH-REVIEW-QUEUE.md` `docs/ADR.md` `docs/notes/ui-implementation-strategy.md` `docs/notes/validation-batch-conclusion.md` `docs/templates/GOVERNANCE-MIGRATION-GUIDE.md` `docs/templates/PROPOSAL.template.md` `docs/templates/WORKSTREAM-PROPOSAL.template.md` `docs/templates/WORKSTREAM-ROADMAP.template.md` `docs/templates/architect-landing-summary.template.md` `docs/templates/retro.template.md`
目标文件：`firmware/main/test/HOST_TEST_CONVENTIONS.md` `server/deploy/README.md` `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md` `firmware/main/test/acceptance/red-baseline/red-baseline-summary.char-stroke.md` `firmware/main/test/acceptance/red-baseline/red-baseline-summary.md` `firmware/main/test/acceptance/red-baseline/schedule-p2-red-baseline-summary.md` `firmware/main/test/acceptance/red-baseline/schedule-reminder-red-baseline-summary.md` `firmware/main/test/acceptance/red-baseline/screen-unification-red-baseline-summary.md` `firmware/main/test/acceptance/red-baseline/story-p3-red-baseline-summary.md` `server/evidence/260903-gateway-directionB/cert-lifecycle-accept-risk.md` `server/evidence/260903-gateway-directionB/deploy.md` `server/evidence/260903-gateway-directionB/v21-multiturn-continuity-probe.md` `server/evidence/260903-gateway-directionB/v23-asr-final-selection.md` `tools/asset-pipeline/tests/test_art_assets.acceptance.md` `tools/simulator/tests/test_lvgl_align.acceptance.md`
目标文件：`firmware/main/test/char_stroke.acceptance.c` `firmware/main/test/device_ws_switch.acceptance.c` `firmware/main/test/happy_flow_capture.test.c` `firmware/main/test/hardware_validation.acceptance.c` `firmware/main/test/key_route_a1.acceptance.c` `firmware/main/test/schedule_reminder_b.acceptance.c` `firmware/main/test/screen_unification.acceptance.c` `firmware/main/test/ui_art_integration.acceptance.c` `firmware/main/test/v3_restore.acceptance.c` `firmware/main/test/voice_roundtrip_b.acceptance.c` `server/acceptance/test_char_stroke.acceptance.py` `server/acceptance/test_gateway_directionB.acceptance.py` `server/acceptance/test_g_relay_fix.acceptance.py` `server/acceptance/test_schedule_p2.acceptance.py` `server/acceptance/test_schedule_reminder.acceptance.py` `server/acceptance/test_smoke_e2e.acceptance.py` `server/acceptance/test_story_p3.acceptance.py` `server/acceptance/test_story_wiring.acceptance.py` `server/acceptance/test_voice_roundtrip_b.acceptance.py` `.gitattributes`

### T4 · 迁入件活内容批改 + 派生索引再生

- `delivery/milestones/delivery-workspace.json`：`planRoot` → `delivery/workstreams`，`milestoneBaseline`/`statusRollup` 路径同步（STATUS.md 选址维持 milestones/ 下不动——delivery-status.mjs 改动最小）。
- `delivery/workstreams/README.md`：边界表三行（`.claude/taskpackages/`、`changes/dev/` 行）改写为新路径，hook 链接与索引重建指令同步。
- `delivery/taskpackages/current/` 4 件（input-brief ×2、taskpackage.json、verdict.json）内路径引用批改。
- `delivery/milestones/milestones.jsonl`（评审 R2 补锚，活数据件）：各里程碑 `planningRefs` 等字段内旧路径按映射表批改（closeout-check 对该字段名 fail-closed，改漏即门红）。
- 派生索引再生（脚本跑、勿手改）：`planning-index.mjs` 重建 `delivery/workstreams/{INDEX.md,index.jsonl}`；`delivery-status.mjs` 再生 `delivery/milestones/STATUS.md`；`changes-index.mjs` **无参全量重建**（评审 R4：实测现 INDEX.md 45 行含旧前缀，仅 `--upsert` 会留 44+ 行死链；脚本无参幂等重建全部行）后再 `--upsert 260914-gov-review-delivery-restructure` 确保本变更行在册。
- 对应根因：P2。
目标文件：`delivery/milestones/delivery-workspace.json` `delivery/milestones/milestones.jsonl` `delivery/workstreams/README.md` `delivery/workstreams/INDEX.md` `delivery/workstreams/index.jsonl` `delivery/milestones/STATUS.md` `delivery/changes/INDEX.md` `delivery/taskpackages/current/input-brief-acceptance-author.md` `delivery/taskpackages/current/input-brief-builder.md` `delivery/taskpackages/current/taskpackage.json` `delivery/taskpackages/current/verdict.json`

### T5 · 图索引面登记

- `.cbmignore`：主索引排除规则增补 `delivery/`（交付工件非代码面，不入 KidPal 图；现状 `.planning/`、`changes/` 本就未被排除规则覆盖属历史疏漏，借此一并对齐 AGENTS.md 表口径）——OQ3 已裁 (a)。
- `.claude/.cbmignore`（KidPal-claude 项目）：删除已失效的 `taskpackages/` 行、注释同步。
- 操作项（非文件锚）：迁移后主索引刷新（`index_repository`）使排除生效。
- 对应根因：P1（taskpackages 意外不入图的错位被显式规则取代）。
目标文件：`.cbmignore` `.claude/.cbmignore`

### T6 · 契约面 3 处指针批改（OQ1 human 裁 (c) + 扩围裁定）

- 行 5、行 96：`docs/contracts/interface-contract.md` 的 `changes/arch/...` 指针按映射表批改为 `delivery/changes/arch/...`；仅地址修正、零语义变更（契约正文条款不动）。
- 行 4（2026-09-14 human 扩围裁定「顺手一起改了」）：更早的过期指针，指向已于 2026-09-13 spec 拆分归档的 `docs/spec.md`。原文（verbatim）：`> 配合 [docs/spec.md](../spec.md) §3.0.2（D3 笔顺格式已定稿）使用。` 替换后完整句：`> 配合 [docs/requirements.md](../requirements.md) F3（查字窗·笔顺演示需求）使用；笔顺 JSON 格式（D3）以本契约为权威。`（旧 §3.0.2 的承接面 = requirements.md F3 story；格式定义权威本就在本契约自身，补充句只是显式化既有事实。）
- 契约门形态（human 2026-09-14 立项裁定 + 同日扩围裁定）：architect-reviewer 四分类预审（行 5/96 已裁「随本治理变更携带（同 PR）· 零语义地址修正」；行 4 同性质顺延——指向承接面的指针刷新，零条款语义变更）+ human 签字（两项裁定即预签）；不升 Track A。
- 对应根因：P3（行 5/96）+ 预审官范围外发现（行 4，human 裁并入）。
目标文件：`docs/contracts/interface-contract.md`

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

census 命令与日期见 §1 P2（2026-09-14 起草期全仓预跑；枚举集 = landing-manifest 锚清单，机械生成、判官 spot-check）。

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 目录迁移 | `.delivery/`、`.planning/`、`.claude/taskpackages/`、`changes/` 四树 | G3 | 历史面内容零改动；`.planning` 顶层实测无活跃 workstream（全在 `_archive/2026/` 3 件），迁移无活跃态损失 |
| 机械强制层 | scope-map、8 hooks、26 scripts、dev-loop.settings、route-fixtures | G3 | 路径平移、逻辑零变更；bashDenyWrite 增补一条为行为**收紧**非放松 |
| 治理文档 | AGENTS.md、rules 2、agents 11、commands 6、references 12、skills 6 | G2+G3 | 提示句与路径引用批改（R2 补锚后计数，评审 R12 核对口径） |
| docs 面 | requirements（活行）、DEV-GOVERNANCE、ARQ、CDQ、ADR、notes 2、templates 6 | G2 | 日志件行级豁免见 P4 |
| 实现面活文档 | HOST_TEST_CONVENTIONS、server/deploy 2 件 | G2 | 指针批改 |
| 实现面证据件 | firmware 红基线 6、server/evidence 4、tools 2 | G2 | OQ2 已裁 (a)：一并批改，无条件锚 |
| 契约面 | `docs/contracts/interface-contract.md` | G3（T6） | 2 行零语义指针批改，human 裁不升轨 |
| CodebaseMemory | `.cbmignore`、`.claude/.cbmignore`、AGENTS.md 表 | G3 | OQ3 已裁 (a)：排除 delivery/ |
| 代码面（R1 已裁扩围） | firmware 10 件 `.c`（全注释）、server 9 件 `.py`（docstring/消息串 + 功能性路径常量 `CHANGES_DEV`）、`.gitattributes` 1 件（`merge=union` 属性路径双重失配——本就指向已归档的 `.planning/kidpal-mvp/`） | G2 | human 2026-09-14 裁全量批改：server 件功能性必改（测试运行时实读 `changes/dev/` 证据目录），firmware 件与 `.gitattributes` 零风险顺改；批改面仅限注释、消息串与路径常量 |
| 未触 | `firmware/**`（实现代码本体）、`server/**`（实现代码本体）、`docs/contracts/**` 正文条款、`.codebase-memory/`、ESP32 参考树 | — | 代码本体零改动；R1 裁扩围时批改面也仅限注释、消息串与路径常量 |

## 4. human 取舍点（OQ）

**OQ1 · 契约面 2 处历史指针（决定 Track B vs 升 Track A）**
- (a) 不动，留根登记：两处均为行内历史审计记录、零活语义，stale 指针随 git 历史与 `delivery/changes/` 树可溯；本变更保 Track B。将来契约面有实质修订时随 spec 改动携带顺手指针刷新。
- (b) 走契约门批改 2 处路径并升 Track A full-adversarial：指针保活但轨成本全付。
- **裁定（2026-09-14，human 立项）**：裁 **(c)**——「契约面只是改地址，不用 Track A」：批改 2 处指针（T6 承载），不升轨；契约门以 architect-reviewer 四分类轻批 + human 预签（本裁定）随本变更走完，评审记录留痕 §7。

**OQ2 · 实现面证据件边界（firmware/server/tools 12 件）**
- (a) 一并批改：活树无死链；批改内容纯为路径指针，不改证据语义。
- (b) 视作历史证据不动：改动面更小，但活树残留 12 件死链，机检豁免名单变长。
- **裁定（2026-09-14，human 立项）**：裁 **(a)**（「同意」）。

**OQ3 · CodebaseMemory 是否排除 `delivery/`**
- (a) 排除：交付工件非代码面，入图只会以 markdown/json 淹没调用图；与 AGENTS.md 表口径对齐。
- (b) 不排除：保留 `delivery/` 可入图能力，代价是图噪声。
- **裁定（2026-09-14，human 立项）**：裁 **(a)**（「排除」）。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**V1 · 残留清扫机检（模式集显式枚举全形态，评审 R3 重写：不限扩展名 + 两段式过滤）**——旧路径四形态：`\.planning`、`\.delivery`、`\.claude/taskpackages`、`changes/`（含 `arch|dev|retro|INDEX` 子形与裸顶层形；正向模式取 `(^|[^a-z])changes/` 兜裸形，`[^a-z]` 排除 `exchanges/` 类词内形，新前缀 `delivery/changes/` 由第二段过滤剔除）：

```bash
grep -rEnI '\.planning|\.delivery|\.claude/taskpackages|(^|[^a-z])changes/' \
  --exclude-dir=.git --exclude-dir=.codebase-memory --exclude-dir=node_modules \
  --exclude-dir=__pycache__ --exclude-dir='ESP32*' --exclude-dir=build \
  --exclude-dir=managed_components . \
  | grep -vE 'delivery/changes/|delivery/workstreams/_archive/|delivery/taskpackages/|docs/archive/|\.claude/verify-logs/'
```

预期：命中仅剩行级豁免件（`docs/requirements.md` 版本历史既有行、`docs/ADR.md` / `docs/ARCH-REVIEW-QUEUE.md` / `docs/CONTRACT-DECISION-QUEUE.md` 历史行、`.claude/loop-retro/ledger.jsonl` append-only 台账既有行——评审 R3 补豁免），逐行人读核对属历史行；其余零命中（含 `docs/contracts/interface-contract.md`——T6 批改后不留残；`.gitattributes` 无扩展名、已被不限扩展名 grep 覆盖）。两个子检（显式目录限定——评审 R10：不做全局「去掉排除项」复跑，会把 24 个历史包的留档引用翻出来）：

```bash
# 子检 1 · 活工作面与活数据件（斜杠形）
grep -rEn '\.planning|\.delivery|\.claude/taskpackages|(^|[^a-z])changes/' \
  delivery/taskpackages/current delivery/milestones/milestones.jsonl
# 子检 2 · 分段/字符串字面量形全家族（评审 R10 补 + R15 封口——段值无关、引号无关、作用域含代码面）
#   a 分支 = 'changes'/"changes" 作首段（逗号后任意段，覆盖 INDEX.md、.changes-index-state.json、dev/arch/retro…）
#   b 分支 = Python join 的 "changes" / "dev" 斜杠拼接形；c 分支 = 特定单引号对
grep -rEn "[\"']changes[\"'], ?|[\"']changes[\"'] */|[\"']\.claude[\"'], ?[\"']taskpackages[\"']|[\"']\.planning[\"']|[\"']\.delivery[\"']" \
  .claude/hooks .claude/scripts .claude/route-fixtures .claude/dev-loop.settings.json \
  firmware/main/test server/acceptance server/deploy tools \
  | grep -vE "[\"']delivery[\"'], ?|[\"']delivery[\"'] */"
```

子检 1、2 均预期零命中（子检 2 第二段剔除已是新前缀的字面量形——`'delivery', …` 与 `"delivery" / …` 两形）。

**V2 · 派生索引再生**：`node .claude/scripts/planning-index.mjs` exit 0 且 `delivery/workstreams/INDEX.md` 无旧路径；`node .claude/scripts/delivery-status.mjs` exit 0 且 STATUS.md 头部链接指向新路径、完工比例数字与迁移前一致（输入未变）；`node .claude/scripts/changes-index.mjs`（无参全量重建）后再 `--upsert 260914-gov-review-delivery-restructure`，断言 INDEX.md 内 `changes/` 旧前缀行数 = 0（迁移前实测 45 行，评审 R4）。

**V3 · 承重脚本自检**：`changes-index`、`landing-fidelity-check`、`proposal-consolidation-check`、`pre-merge-escalation-gate`、`portfolio-reconcile`、`merge-driver`、`mutation-check`、`verifier-evidence-check`、`render-roadmap`、`route-regression-check` 逐一 `--self-check` 全绿（self-check 入口存在性复测：17 件 scripts 含 self-check，2026-09-14 `grep -l | wc -l` 实证——评审 R6 修正原稿 16 件口径）。

**V4 · 结构门**：`landing-fidelity-check.mjs` 过（manifest ≡ 实际落地）；`node .claude/scripts/route-regression-check.mjs` 过（route-fixtures 路径更新后路由行为不变）。

**V5 · git 卫生**：`git diff --cached --stat` 核对后 commit；`git log --follow` 抽查 1 个历史件（如 `delivery/workstreams/_archive/2026/kidpal-mvp/task_plan.md`）确认搬运历史连续。

**V6 · 边界纪律基线（评审 R8 补）**：对构建系统文件一次性 grep 证零 `delivery/` 引用（`firmware/**/CMakeLists.txt`、`firmware/**/idf_component.yml`、`server/**/pyproject.toml|requirements*.txt`、`tools/**/package.json`、根级构建配置），输出留痕 landing-summary；不新增长效门，后续靠 §0 纪律 + code review 目检（留根 §6）。

## 6. 留根清单

- **交付系统迁出预留（human 2026-09-14 注记 + 同日讨论澄清 v1.1）**：迁出模式 = **收口即迁、活跃态留仓**——治理 loop 全程（起草/评审/FROZEN/landing/verifier/retro）照旧在仓内跑、机械层零改动；闭环最后一步把收口制品（四件 + 已晋升包 + 归档 workstream）推交付系统，仓内收缩为薄指针 + INDEX 映射行；在途工作区永远留仓。边界纪律（代码面不新增 `delivery/**` 构建期依赖）随本变更生效，收口推系统的机制届时单独立项、不并入本件。**存量例外登记（R1 裁定连带）**：server 验收测试对 `delivery/changes/dev/**` 证据目录的读取属既有运行期依赖（迁移仅平移路径保测试绿），收口制品推系统后这批测试的证据查找逻辑须一并改指系统——非新增违规、届时对账。
- **stale 指针留根**（日志件行级豁免的既定残值）：requirements/ADR/ARQ/CDQ 历史行、已晋升包件内引用——读档按当时语境理解；将来 spec/契约实质修订时随批携带刷新，不单独立项。
- **PI 概念决策记录**：不引入 PI/时间盒层（速度与范围双输入皆噪声）；范围维度 = Milestone → Workstream → TaskPackage；时间沉数据（front-matter opened/closed、归档年份桶）。将来若实况出现「批间节奏调度」需求，再议轻量批次文件（挂 workstream 下），不复活目录级 PI。
- **Pod worktree 前置**：迁移窗口须无在途 Pod（分支基于旧路径）；launch 面脚本（kidpal-pod-launch-prep）的 worktree/图索引指引已随 T3 批改，下批 launch 前人工复核一次。
- **登记不硬治**：`.planning/`、`changes/` 历史上未被 .cbmignore 排除（图噪声疏漏）由 T5 顺手对齐，不追溯历史图内容。
- **自律件**：landing 用脚本化批量替换 + 逐文件 diff 复读（~140 件不逐一手敲）；替换正则**两种形态都要喂**（斜杠形 + 分段字面量形，评审 R10）——先在 4 个样本件（scope-map、planning-sync.mjs、changes-index.mjs〔分段重度件，R10 补〕、DEV-GOVERNANCE.md）上试跑核对再全量。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 2026-09-14 · architect（Track B 判官，第 1 轮）· 【BLOCK · 升 human】

**census 扩展名盲区：firmware/server 验收代码件与 `.gitattributes` 实测漏扫，§3「未触代码」断言为假。**

§1 P2 四条 census 与 V1 机检的 `--include` 过滤器只扫 `*.md / *.json / *.mjs / *.jsonl`。判官不限扩展名复跑同模式（排除 `_archive / archive / .codebase-memory / node_modules / build` 与四迁移树历史面），活面额外命中：

- `firmware/main/test/` 下 10 件 `.acceptance.c` / `.test.c`：如 `char_stroke.acceptance.c:181`（`.claude/taskpackages/260829-char-stroke/red-baseline-summary.md`）、`happy_flow_capture.test.c:8`（`changes/dev/260913-happy-flow-host-capture/`）、`v3_restore.acceptance.c:5,898`（`changes/dev/260911-render-pages/…`）——均为注释内证据指针。
- `server/acceptance/` 下 9 件 `.py`：如 `test_char_stroke.acceptance.py:93`、`test_smoke_e2e.acceptance.py:61,132,144`、`test_story_wiring.acceptance.py:31,231,294`——含断言失败消息串内旧路径。
- `.gitattributes:12`：`.planning/kidpal-mvp/progress.md merge=union`——merge 属性模式随目录迁移失配，属 G3 邻接机械面。

这直接与 §3 表「未触：firmware/**（代码）、server/**（代码）」矛盾：迁移后这些是活代码面死链，与 OQ2(a)「活树无死链」的裁定理由同构但超出其裁定范围（OQ2 只枚举了 12 件 .md 证据件）。**为何升 human**：扩围批改将首次触代码文件（即便仅注释/消息串，零行为变更），与本件 §0「代码面边界纪律」及 §3 声明冲突，且 `.gitattributes` 模式如何平移（原文件已随 workstream 归档）需拍板；建议二选一——(a) 顺 OQ2(a) 逻辑扩围为「代码注释/消息串纯指针批改 + .gitattributes 模式评估平移」，manifest 补锚；(b) 明确豁免并进 §6 留根（活代码面留 stale 指针，机检豁免名单加长）。裁定前本件不可 FROZEN。

### R2 · 2026-09-14 · architect · 【必改（GREEN 前提），改法机械】

**manifest 漏 4 个活面锚，「census 命中集与锚清单一一对应」硬项断言不成立。**

判官逐条复跑 §1 P2 census（命令原样可复跑，1b 项通过），与 manifest 全量对账，漏列：

1. `.claude/agents/portfolio-planner.md`——`.planning` / `taskpackages` / `.delivery` 三条 census 均命中（:3、:30、:35、:40、:48、:70、:93、:95 等大量活引用）。§2 T3 与 §3 的「agent 定义 10 件」实为 **11 件**（`.claude/agents/` 全量 14 件中命中旧路径引用者 11 件）。改法：manifest 补锚 + 两处计数改 11。
2. `delivery/milestones/milestones.jsonl`（现 `.delivery/milestones.jsonl`）——活数据件，行 1/2 `planningRefs:[".planning/kidpal-mvp"|"kidpal-validation"]` 为旧路径；该文件是里程碑单一事实源（`delivery-status.mjs:2-4` 注明），且同名字段在 closeout 门是 fail-closed 消费（`closeout-check.mjs:333-336` 对 planningRefs 逐个 `existsSync`，缺路径即 fail）。改法：入 manifest + T4 批改清单（planningRefs 改 `delivery/workstreams/...`）。
3. `.claude/references/plain-file-handoff.md:9`——裸 `changes/` 顶层引用（"do not commit/store it under `changes/`"），四形态 census 不覆盖裸顶层形故漏。改法：manifest 补锚 + T3 批改。
4. `docs/architecture.md:276`——canonical spec 的仓库结构描述行（"`changes/`（治理/开发证据）"），同为裸 `changes/` 形漏扫；不改则 spec 面顶层地图与本变更后的实况不符，直接削弱 §0 效果。改法：manifest 补锚 + T3 批改（该行为活文本，非日志件）。

另：`.claude/loop-retro/ledger.jsonl` 亦漏列，但属 append-only 台账（见 R3 第 2 点），进 V1 豁免名单即可、不必批改。`.claude` 面不限扩展名全量对账的其余 69 件命中与 manifest 完全一致（settings/verify-gates/review-loop.settings 均无旧路径引用），此面无其他漏项。

### R3 · 2026-09-14 · architect · 【必改（GREEN 前提），改法机械】

**V1 残留清扫机检按现写法不可判（1c 项不通过），三处缺陷。**

1. **`(^|[^y])` 前瞻实测不区分 `delivery/changes/`**：判官以样例实测，`delivery/changes/arch/foo` 与 `delivery/changes/INDEX.md` 均被正向模式命中（`changes/` 前导字符是 `/` 不是 `y`）——对新路径的判别完全落在 `-vE` 排除列，而排除列只有 `delivery/changes/(arch|dev|retro)/`，**缺 `delivery/changes/INDEX`**。后果：批改正确的活行（requirements 610/675 改后、AGENTS.md、changes-index.mjs 头注等所有含 `delivery/changes/INDEX.md` 的行）会在 V1 误报命中，V1「其余零命中」预期必假。改法：排除列补 `delivery/changes/`（整段新路径引用均合法），或重写正向模式。
2. **豁免名单缺 `.claude/loop-retro/ledger.jsonl`**：行 2 note 含 `changes/retro/2026-09-13-loop-retro-efficiency.html`，为 append-only 台账历史行，按 P4 原则不动——须列入 V1「预期命中仅剩行级豁免件」名单，否则 V1 必红且无法判定属预期残值还是漏扫。
3. **正向模式弱于生成 manifest 的 census**：不覆盖裸 `changes/` 顶层形（正是 R2 第 3/4 点漏项的成因）与无 `.claude/` 前缀的 `taskpackages/...` 形；V1 无法兜底这两类残留。改法：V1 增补裸形子检查（如 `(^|[^y])changes/` 排除 `delivery/changes` 后应零活命中），或明文声明裸形以 manifest 对账为唯一防线。`taskpackages` 裸词形的「命中行须含 delivery/taskpackages」断言写法有解释但无对应命令，建议落成可跑的子命令。V1 的 `--include` 过滤器与 census 同盲区，随 R1 裁定同步修。

### R4 · 2026-09-14 · architect · 【必改（GREEN 前提），改法机械】

**T4 的 changes-index 步骤不完整：`--upsert` 只钉本变更单行，INDEX.md 其余行全量残留旧路径。**

实测 `changes/INDEX.md` 现有 **45 行**含 `changes/dev|changes/arch` 字样（该文件为 changes-index.mjs 幂等生成的投影，行内路径由目录扫描产出）。迁移后仅跑 `--upsert 260914-gov-review-delivery-restructure` 只刷新本变更一行，其余 44+ 行保持旧前缀 = 派生活索引全量死链，且逐行命中 V1 正向模式、不被排除列覆盖（行内无 `delivery/` 前缀）。`changes-index.mjs` 头注自述支持无参幂等全量重建（脚本 :7「node .claude/scripts/changes-index.mjs # 幂等全量重建」）。改法：T4 该步改为「全量重建 + `--upsert` 本变更行」（或 upsert 后全量重建验幂等），V2 第三条断言由「INDEX.md 行路径为 delivery/changes/arch/…」改为「全量行路径均为新前缀」。planning-index / delivery-status 两步本就是全量重建写法，无此问题。

### R5 · 2026-09-14 · architect · 【ADVISORY】

§1 P2 计数口径不自洽：「43 件引用 taskpackages（含 8 hooks、26 scripts、10 agents、6 commands、11 references、6 skills、2 rules）」——括号内合计 69 ≠ 43；判官复跑 `.claude` 面计数为 `.planning` 29 件、`taskpackages` 44 件（含 portfolio-planner）。建议改写为以 manifest 为准的单口径（如「机械面触点 = manifest 所列 scope-map/settings/fixtures + 8 hooks + 26 scripts；治理文档面 = …」），删去双轨数字。

### R6 · 2026-09-14 · architect · 【ADVISORY】

V3 实证计数小误：`grep -ln 'self-check' .claude/scripts/*.mjs` 实测 17 件，PROPOSAL 写 16 件。不影响可判性——V3 具名列出的 10 件逐一核过均有 self-check 入口（route-regression-check.mjs 存在且含 7 处），V3/V4 命令可直接跑。

### R7 · 2026-09-14 · architect · 【ADVISORY】

v0.3 头部残留旧口径：riskTier 行仍写「契约面触点 = 2 行零语义指针修正」、review 轨行仍写「契约面 2 行指针批改」，与 T6 三处口径（行 4/5/96）不一致，建议同步为「3 处」。

### R8 · 2026-09-14 · architect · 【ADVISORY】

§0「代码面不新增 `delivery/**` 构建期/运行期依赖」目前仅 §6 留根、无核验手段（V1 只扫旧路径，无法发现新引用）。建议 landing 时补一次性检查：枚举构建系统文件（firmware CMakeLists/分区表、server 部署脚本与依赖清单、tools 构建脚本）`grep -rn 'delivery/'` 预期零命中；或在 §6 明记「暂无机械门、靠 review 目检」。注意与 R1 裁定互动：若代码注释顺 OQ2(a) 批改为新前缀，检查范围须限定构建系统文件、排除注释与消息串。

### R9 · 2026-09-14 · architect · 【实证复核通过项（1a/2a/3/4/6 中未列打回的部分）】

- **起草期实证全部可复现**：`.delivery/delivery-workspace.json` 的 `planRoot: ".planning"` / `milestoneBaseline` / `statusRollup`（:19 邻域）✓；repo-structure.md 顶层地图与「工作产物去哪」节旧路径引用（:27/:28/:39/:40）✓；enforce-scope.mjs denyCreate 消费（:53）与 docsTopLevel 提示句（:61）✓；scope-map `denyCreate: ["contracts/**"]`（T2「不拦顶层目录创建、delivery/ 可直接创建」论证成立）✓；requirements 610/675 为活文本、688/729 为版本历史行 ✓；契约行 5/96 verbatim ✓；24 包目录 + `current/`（ls 计 25 含 current）✓；22/24 晋升（comm 交集实测 22，changes/dev 共 23 件）✓；`.planning` 顶层无活跃 workstream、`_archive/2026/` 恰 3 件 ✓；`.claude/taskpackages/.gitkeep` 在册（T1 删除指令有对象）✓；T2「bashDenyWrite `.claude/**` 对验收包的覆盖随迁出消失、平移补 `delivery/taskpackages/**`」论证成立（现状值 `["docs/contracts/**", ".claude/**"]`）✓；planning-sync 钩子确为路径常量消费（:36 `startsWith('.planning/')`）✓。
- **OQ 三裁定落实一致（第 4 项）**：OQ1(c) → T6 + manifest `docs/contracts/interface-contract.md` 锚 + §3 契约面行 + V1「T6 批改后不留残」自洽；OQ2(a) → 12 件证据件在 manifest 行 106–117 无条件齐备（T3「条件锚（OQ2 裁 (a) 时）」措辞可改「已裁 (a)，无条件锚」）；OQ3(a) → T5 + manifest 两 cbmignore 锚 + §3 行，`.cbmignore` 现状（docs/ + tools/）与 `.claude/.cbmignore` 现存 `taskpackages/` 行均与 T5 改法对得上。
- **T6 扩围（v0.3）行 4 核过**：原文 verbatim 与 `docs/contracts/interface-contract.md:4` 一致；替换句承接面属实——`docs/requirements.md` F3（:459「F3 查生字…笔顺演示」、:483 S-F3.1 语音查字并看笔顺演示、WF-F3-01 查字窗）确为旧 spec §3.0.2 的承接面；「笔顺 JSON 格式（D3）以本契约为权威」与 AGENTS.md 契约面定义（D3 权威在 docs/contracts）一致，属显式化既有事实，零条款语义变更论证成立；相对链接 `../requirements.md` 解析正确，且替换后不触 V1 任何模式。
- **census 可复跑（1b）**：四条命令原样重跑结果与 manifest 对账，除 R1/R2 所列盲区与漏项外一一对应；manifest 无多列（26 scripts / 8 hooks / 11 references / 6 commands / 6 skills / docs 13 件均与 census 命中精确互证）。
- **迁移顺序与风险（第 3 项）**：T1+T2 同 commit 可操作；Pod worktree 前置已在 T1 与 §6 双登记；`current/` 活工作面处理（T4 批改 4 件）与两未晋升包按历史件处理的边界清晰；派生件再生顺序在 T4 内闭环（除 R4 所指 changes-index 步骤外）。
- **V2/V4/V5 可判（第 6 项）**：三脚本消费点已核（如 delivery-status.mjs:75 读 `.delivery/milestones.jsonl` 常量）；landing-fidelity-check 与 route-regression-check 均存在且有入口；V5 `--follow` 抽查法可判。

### 结论 · 2026-09-14 · architect 第 1 轮

**RED**。BLOCK 项 1 件：**R1**（census 扩展名盲区致 firmware/server 验收代码件 19 件与 `.gitattributes` 漏扫、§3「未触代码」断言为假，扩围与否需 human 裁定）。另 R2/R3/R4 为 GREEN 前必改项（manifest 补 4 锚 + 计数改 11、V1 排除列补 `delivery/changes/` 与豁免名单补 ledger.jsonl、T4/V2 changes-index 改全量重建），改法均机械、无需 human 新裁定，随 R1 裁定一并改后交判官复审；R5–R8 为 ADVISORY 不阻断。

---

**T6 契约预审记录（architect-reviewer，2026-09-14）**：四分类 = **随本治理变更携带（同 PR）· 零语义地址修正**。实核 `docs/contracts/interface-contract.md`：行 5（260902 信封草案存档指针）、行 96（260829 total 字段契约变更记录引用）共 2 处 `changes/arch/...` 指针，全目录 grep（含反斜杠变体与 change-id 裸名）确认无其余 `changes/` 路径依赖；契约正文条款零触及。归「无需改」不成立（迁移后指针必悬空、违背可溯性）、非独立契约变更（无条款/字段/格式/API 语义变化）、非 spec↔contract 漂移（两面无冲突）——路径映射为唯一正解。按映射批改为 `delivery/changes/arch/...`。human 门：OQ1 裁 (c) 立项预签（2026-09-14），不升 Track A。**不备独立 ContractChangePacket**（零语义地址修正，证据随本 PROPOSAL 在册：§0 P3 / §2 T6 / §4 OQ1 / §5 V1；contract-change-evidence.md 全量 ledger 针对 Tier-1 语义变更，本改动不触任何 Tier-1 面）。验证：随 §5 V1（批改后契约文件 `changes/` 残留为零）。

**T6 扩围行 4 的预审补充（主线程记录，2026-09-14）**：预审官通读时另发现行 4 存在更早的过期指针（指向 2026-09-13 spec 拆分时已归档的 `docs/spec.md`，属 `changes/` grep 模式之外），原判「不要求本次处理、建议登记 ARQ」；human 同日裁定「顺手一起改了」并入 T6（替换句已按硬项 2a 钉死于 T6）。行 4 与行 5/96 同性质（指向承接面的指针刷新、零条款语义变更），四分类结论与预签顺延覆盖；判官第 1 轮 R9 已核行 4 替换句承接面属实（requirements F3 :459/:483）、「D3 以本契约为权威」为显式化既有事实，零语义论证成立。

### R10 · 2026-09-14 · architect（第 2 轮）· 【必改（GREEN 前提），机械，无需 human】

**分段字符串字面量形态：路径映射表与 V1 机检双盲区，且该形态是机械层路径常量的主流写法。**

判官复核 v0.5 时实测：机械层脚本/钩子的路径常量大量以分段字面量构造，如 `resolve(projectDir, '.claude', 'taskpackages', ...)`（verify.mjs:101,123,127、pod-events-append.mjs:63,99 等）、`resolve(projectDir, 'changes', 'dev')`（changes-index.mjs:144、closeout-check.mjs:189）、`resolve(projectDir, 'changes', 'INDEX.md')`（changes-index.mjs:395,475,494、closeout-check.mjs:147）、`join(projectDir, '.delivery', 'STATUS.md')`（delivery-status.mjs:90）。全量扫描 `'taskpackages'|'changes'` 独立字面量共命中 **17 件**（verify.mjs、supervisor-gate-check.mjs、supervisor-verdict-gate.mjs、pod-events-aggregate.mjs、pod-events-append.mjs、brief-inputs.mjs、changes-index.mjs、closeout-check.mjs、dispatch-stamp.mjs、journal-append.mjs、lib/governance-gate-shared.mjs、loop-health-report.mjs、loop-metrics.mjs、merge-driver.mjs、pre-merge-escalation-gate.mjs、render-roadmap.mjs、verifier-evidence-check.mjs）。**17 件全部已在 manifest T2 锚集内（comm 实证零缺），无需扩 manifest、无需 human 新裁定。**问题有二：

1. **T2 批改机制覆盖不到**：T2/§6 自律件的替换依据是斜杠形态映射表（`.claude/taskpackages/` → `delivery/taskpackages/` 等），分段形态 `'.claude', 'taskpackages'`、`'changes', 'dev'` 不含斜杠、不被匹配。若 landing 批量替换只按映射表走，这 17 件运行时静默解析旧路径——changes-index / closeout / delivery-status 会在 V2/V3 loudly 失败，但 verify.mjs 钩子、pod-events、dispatch-stamp、journal-append 不在任何验证集运行路径上，dev-loop 运行期才炸，与 §0「行为保持」直接冲突。改法：T2 与 §6 自律件明文增补分段替换规则（`'.claude', 'taskpackages'` → `'delivery', 'taskpackages'`；`'changes', 'X'` → `'delivery', 'changes', 'X'`），样本件试跑清单（现 3 件）补 1 件分段形态重度件（建议 changes-index.mjs）。
2. **V1 机检结构上看不见该形态**：正向模式 `\.claude/taskpackages` 与 `(^|[^a-z])changes/` 都要求斜杠邻接，分段字面量全部漏过——V1 无法对「迁移后无残留旧路径常量」出证。改法：V1 增补分段形态子检（如 `grep -rnE "'taskpackages'|'changes'" .claude/ firmware/ server/ tools/` 批改后仅容 `'delivery'` 前缀形态与历史面豁免件）；顺带把两个子检（current/、milestones.jsonl）落成显式目录限定的可跑命令——v0.5 写法「去掉对应排除项」有歧义：全局去掉 `delivery/taskpackages/` 排除会把 24 个历史包的留档引用一并翻出来，必须限定 grep 范围为 `delivery/taskpackages/current/` 本身。

### R11 · 2026-09-14 · architect（第 2 轮）· 【ADVISORY】

§1 P2 含-句式仍歧义：「44 件引用 taskpackages（含 8 hooks、26 scripts、11 agents、6 commands、12 references、6 skills、2 rules）」——分项和 71 ≠ 44。分项实为 `.claude` 面四 pattern 命中并集口径（判官实测与分项和吻合）。建议改写为「`.planning` 29 件、`taskpackages` 44 件；机械面四形态并集 71 件 = 8 hooks + 26 scripts + 11 agents + 6 commands + 12 references + 6 skills + 2 rules」。

### R12 · 2026-09-14 · architect（第 2 轮）· 【ADVISORY】

两处措辞残留：(a) §3 表「治理文档」行仍写「agents 10、…、references 11」，与 T3 已改的 11 agents / 12 references 不一致（v0.5 漏改此行）；(b) §1 P2 尾句「census 命中集与锚清单一一对应，R1 面除外」——R1 已裁扩围且 20 件已入锚，「除外」措辞过时，建议删。

### R13 · 2026-09-14 · architect（第 2 轮）· 【ADVISORY】

V1 排除列健壮性两处：(a) `--exclude-dir=ESP32-S3-RLCD-4.2-Demo` 为精确名匹配，若在有参考树的 checkout（如主 worktree，五个 `-0x` 后缀目录）跑 V1 则不生效——本 gov worktree 实测无参考树、当前无害，建议改 glob 形态 `--exclude-dir='ESP32-S3-RLCD-4.2-Demo*'` 以防换环境跑；(b) `firmware/build`、`firmware/managed_components` 不在排除列——实测当前零命中且均 gitignored，但构建树再生后可能携带含旧路径注释的源文件副本造成误报，建议顺手补 `--exclude-dir=build --exclude-dir=managed_components`。

### R14 · 2026-09-14 · architect（第 2 轮）· 【第 1 轮各项处理复核——全部核实落地】

- **R1（BLOCK 解除）**：human 扩围裁定留痕完整（manifest 注记「R1 已裁扩围（human 2026-09-14『同意推荐』）」+ T3 代码面条 + §3 表 + §6 存量例外登记）。**20 件枚举与判官 census 精确一致**：firmware 10 件 `.c` 逐文件名对账吻合、server 9 件 `.py` 吻合、`.gitattributes` 在列。功能性路径常量断言实证：`test_gateway_directionB.acceptance.py:158-159`（`CHANGES_DEV = REPO_ROOT / "changes" / "dev"`、`PACKAGE_EVIDENCE_DIR = CHANGES_DEV / "260903-gateway-directionB"`）、`test_g_relay_fix.acceptance.py:274-275`（`LIVE_EVIDENCE_DIR = CHANGES_DEV / "260907-g-relay-fix"`）——「不改迁移后必红」成立。`.gitattributes` 新目标实证：`.planning/_archive/2026/kidpal-mvp/progress.md` 存在，且 `.planning` 顶层确无 kidpal-mvp（原行早已失配的论证成立）。KidPal-claude 图项目不扩的裁定与 T5 原文自洽（delivery/ 迁出 `.claude` 后与该项目无交集），无需改文。
- **R2**：manifest 4 锚齐备（portfolio-planner / plain-file-handoff / docs/architecture.md / milestones.jsonl），T3 计数 11 agents / 12 references、T4 补 milestones.jsonl planningRefs 条（closeout fail-closed 依据引述准确）。
- **R3**：V1 重写实跑通过——命令在本环境机械可跑，两段式过滤行为符合断言（`(^|[^a-z])changes/` 兜裸形成功、`[^a-z]` 正确排除 `exchanges/` 类词内形、第二段剔 `delivery/` 新前缀、`-I` 跳二进制）；豁免名单补 ledger.jsonl；`.gitattributes` 无扩展名被不限扩展名 grep 覆盖。分段形态盲区为新发现，见 R10。
- **R4**：T4 改无参全量重建 + `--upsert`、V2 断言「旧前缀行数 = 0（迁移前实测 45 行）」——与判官实测数字一致，可判。
- **R5–R8**：P2 数字改 29/44（句式残留见 R11）；V3 改 17 件；头部「2 行」旧口径清零（riskTier 行「3 行」、review-轨行「3 处」均已改）；V6 构建系统零依赖基线入册 + §6 存量例外登记。
- **manifest ≡ sweep 机械对账（第 2 轮全量）**：判官以 V1 同模式不限扩展名全量扫描 + 旧→新路径映射后，与 manifest 全部 142 锚 comm 对账——sweep 侧仅 `.claude/loop-retro/ledger.jsonl` 一件不在 manifest（= 已声明豁免件）；manifest 侧 10 件无 sweep 命中均正当（4 个 T1 目录根锚、2 个 T5 cbmignore 锚、`delivery/workstreams/index.jsonl` 再生目标锚、3 件纯分段形态文件 verify.mjs / pod-events-aggregate / pod-events-append——后者正是 R10 所指形态，锚定无误、V1 看不见）。一一对应成立。
- **T6 预审记录入册**：architect-reviewer 四分类结论（随本变更携带·零语义、不备 ContractChangePacket）与判官第 1 轮 R9 行 4 核验互相印证，预签链条完整（立项裁定 + 扩围裁定）。

### 结论 · 2026-09-14 · architect 第 2 轮

**仍 RED（轻）——无 BLOCK、无待 human 项**。第 1 轮 R1–R8 全部核实落地（R14），扩围 20 件枚举精确、manifest 142 锚与全量 sweep 对账干净。唯一 GREEN 前提 = **R10**（分段字符串字面量形态的 T2 替换规则 + V1 子检补齐，17 件受影响文件已全部在锚、改法机械）；R11–R13 为 ADVISORY 顺手项。R10 落地后交判官第 3 轮快审（预期单点核对，可转 GREEN）。

### R15 · 2026-09-14 · architect（第 3 轮）· 【必改（GREEN 前提），机械 3 行级，无需 human】

**R10 修复的形态家族枚举未封口：子检 2 与 T2 分段映射各漏一类实际形态，其中一处正是 R10 自述要兜底的 silent 类。**

判官第 3 轮对分段/字符串字面量形态做全家族枚举（引号无关、段值无关），v0.6 覆盖之外实测另有：

1. **`'changes', 'INDEX.md'` ×4 + `'changes', '.changes-index-state.json'` ×1**：宿主 changes-index.mjs:395,475,494,29 与 **closeout-check.mjs:147**。子检 2 正向模式 `'changes', ?'(arch|dev|retro)'` 与 T2 分段映射（`'changes', 'dev'`（arch/retro 同理））都只列 dev/arch/retro 三段——INDEX.md 与状态缓存件不在内。changes-index 侧漏改会被 V2 loudly 抓住，**closeout-check.mjs:147 不在 V1–V6 任何运行路径上，漏改即 dev-loop closeout 运行期才炸——恰是 R10 条自述「故分段形以 V1 子检兜底」要防的 silent 类**。
2. **Python 双引号 join 形 `"changes" / "dev"` ×8（7 件）**：`test_gateway_directionB.acceptance.py:158`、`test_g_relay_fix.acceptance.py:274`、`test_schedule_p2.acceptance.py:146`、`test_schedule_reminder.acceptance.py:295`、`test_story_p3.acceptance.py:196`、`test_story_wiring.acceptance.py:92,94`、`test_voice_roundtrip_b.acceptance.py:112`——引号（双）与分隔符（` / ` 运算符）都与子检 2 的单引号+逗号模式不同，V1 主命令的斜杠正则也不匹配。T3 已语义性要求功能性常量必改，但无任何机检能看见残留。
3. **C 双引号实参 `"changes"` ×1**：`device_ws_switch.acceptance.c:864`（`wsa_scan_tree_both("changes", "shim.py", …)`——运行时实扫目录，功能性）。

以上全部在既有 manifest 锚内（无扩面）。判官另核：代码面无 `".planning"` / `".delivery"` / `"taskpackages"` / `".claude"` 双引号实例——**形态家族就此封闭，无第四类**。改法：

- T2 分段映射：`'changes', 'X'` 泛化为任意段（明示含 `'INDEX.md'`、`'.changes-index-state.json'`），并补双引号形态注记（Python `"changes" / "dev"` 8 处、C `"changes"` 实参 1 处，映射同喂）。
- 子检 2 正向模式改引号无关 + 任意段：`['\"]changes['\"],`（逗号段，覆盖任意后继段）与 `['\"]changes['\"] */`（Python join）两分支 + 既有 `'\.claude', ?'taskpackages'`、`'\.planning'`、`'\.delivery'`；第二段剔除改为「行含 `['\"]delivery['\"]`」（新形 `'delivery', 'changes', 'X'`、`REPO_ROOT / "delivery" / "changes"…`、`"delivery", "changes"…` 均含 delivery 字面量）；作用域从 `.claude` 四面扩到含 firmware/server/tools（双引号形态宿主）。命中预期仍零（批改正确的新前缀行全被第二段剔除）。

### R16 · 2026-09-14 · architect（第 3 轮）· 【R10–R13 处理核实——除 R15 所指子检 2 形态集外全部落地】

- **R10 结构落地**：T2 分段条（17 件清单、四条分段映射、运行期炸点说明 + V1 子检兜底逻辑）、子检改显式目录限定（不再全局去排除——正确，避免翻出 24 个历史包留档引用）、§6 样本件补 changes-index.mjs + 双形态喂养，均在 v0.6 文本核实。分段映射中 `'.planning'` / `'.delivery'` 两条形态正确（且二者本就被 V1 主命令 `\.planning` / `\.delivery` 覆盖，双保险）。
- **R11**：P2 重写为单 pattern 口径（29 含 `.planning` 形 / 44 含 `taskpackages` 形）与四 pattern 并集 71 件分列、明示不可相加——歧义消除。
- **R12**：§3 治理文档行改 agents 11 / references 12；P2 尾「R1 面除外」清除、改引 R14 comm 对账结论（142 锚、ledger.jsonl 唯一豁免、manifest 侧 10 件无命中正当）——与判官对账记录一致。
- **R13**：V1 主命令排除列改 `--exclude-dir='ESP32*'` glob、补 build / managed_components——本环境实跑复核行为正常（参考树本就不在 gov worktree、两构建目录实测零命中，属换环境保险）。
- riskTier 触点数改 142 锚口径 ✓；manifest 与 T6 面未动（packet 稳定）✓；状态行记 human 指示「GREEN 后不 landing、等 main 侧 sync 通知」✓。

### 结论 · 2026-09-14 · architect 第 3 轮

**仍 RED（轻）——单一必改 R15，无 BLOCK、无待 human 项**。R10–R13 的 v0.6 处理除子检 2 形态集外全部核实落地（R16）；R15 为分段/字符串字面量形态家族封口（子检 2 补任意段与双引号两分支、T2 映射泛化 + 双引号注记、作用域扩 code 面），改法机械 3 行级、全部实例已在锚。**R15 落地后判官即时确认转 GREEN（一行 diff 知会即可，无需第 4 整轮复审）**；GREEN 后按 human 指示标 FROZEN、landing 等 main 侧 sync 通知。

### R17 · 2026-09-14 · architect（第 4 轮 · R15 一行 diff 知会的即时确认）· 【R15 处理核实——全部落地】

- **T2 字面量条重写核实**：四类形态全列（单引号逗号 17 件 / `'changes',` 任意段含 INDEX.md、状态缓存件与 closeout-check.mjs:147 炸点 / Python 双引号 join ×8 含全部功能性常量 / C 双引号实参 :864），引注「家族封闭、无第五类」结论；映射规则泛化「凡首段 = `'changes'` / `"changes"`（任意后续段）→ 前插 `'delivery', `」+ 三个特定对——与判官 R15 改法逐条对应。
- **子检 2 重写实跑双验证**：① 迁移前实跑——命中 27 件宿主文件**全部在 manifest 锚集内**（17 件 JS 分段 + delivery-status.mjs / planning-index.mjs 的 `'.delivery'` / `'.planning'` 独立字面量 + server 7 件 `.py` + device_ws_switch.acceptance.c），零锚外宿主、零作用域意外（route-fixtures / dev-loop.settings / server/deploy / tools 字面量形零命中——该四面仅斜杠形，由 V1 主命令覆盖）；merge-driver.mjs 单件 18 处任意段命中，正是段值泛化的实证价值。② 新前缀形态样例 5 行（`'delivery', 'changes', 'dev'` / `"delivery" / "changes"` / `"delivery", "changes"` / `'delivery', 'taskpackages'` / `'delivery', 'workstreams'`）全部被第二段正确剔除、故意保留的旧形 `'changes', 'INDEX.md'` 正确存活——过滤逻辑与断言精确一致。
- **顺手修正核实**：「三个子检」→「两个子检」已改，与子检 1（current/ + milestones.jsonl 斜杠形）+ 子检 2（字面量形全家族）的实际结构一致。

### 结论 · 2026-09-14 · architect 第 4 轮（终审）

**GREEN（可 FROZEN）**。四轮评审全部收口：第 1 轮 R1（BLOCK，human 已裁扩围 20 件）+ R2–R4（必改，已修）；第 2 轮 R10（必改，已修）；第 3 轮 R15（必改，已修）；R5–R8、R11–R13（ADVISORY）全采。判官随 GREEN 结论核对结构字段：landing-manifest 围栏块（142 锚）与 packet-id 行均在场 ✓。残留清扫（V1 主命令 + 两个子检）、派生索引再生（V2 全量重建口径）、承重脚本自检（V3）、结构门（V4）、git 卫生（V5）、边界纪律基线（V6）机械可判 ✓。T6 契约面 3 处（行 4/5/96）经 architect-reviewer 四分类预审 + human 预签 + 判官两轮核验，零语义论证成立 ✓。无待 human 项、无未决 ADVISORY。按 human 指示：主线程标 FROZEN 后 landing 等 main 侧 sync 通知再落。
