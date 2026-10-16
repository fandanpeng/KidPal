# PROPOSAL · 261014-gov-review-dev-lane-merge-taskpackages

> **状态：FROZEN v1.0 · 2026-10-14 · 立项 ack（2026-10-14，OQ1 / OQ2 均 (a)）+ R1 / R2 / R3 评审闭环 GREEN。内容冻结（landing 期不可回改）；packet-bound landing 待执行，授权记此行。**
> **承载 ARQ**：无（human 直接发起；human 2026-10-14 两项前置裁定——① 合并方向拍板「收尾文件并入 taskpackages、老重复件以 taskpackages 为活本」；② `changes/` 名称保持不变，本次不改名）。
> **review 轨**：Track B 常驻轻路径（默认路由；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节；`AGENTS.md`「治理路由」节）。不触发升级判据：契约面零触及（2026-10-14 `grep 'changes/dev' docs/contracts/interface-contract.md` 实测零命中）、非跨系统结构变更。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（批改锚 69 件 + 纯搬移约 400 文件，触点面大；但全部为路径平移 / 指针刷新 / 扫描根迁移，机械层判定逻辑零变更——唯二**主**行为变化 = dev INDEX 落址迁移与 closeout 闸查找根迁移，连带项见 T2 连带声明，均为本变更显式设计目标，非意外副作用）。
> **review profile**：G2+G3（G2：dev-loop 工作面结构与实现面证据件；G3：治理机械层 scripts/hooks/scope-map 与治理文档面）。G1 / 契约面 = 零。
>
> **结构字段硬条款**：landing-manifest 围栏块与 packet-id 行是 PROPOSAL 结构字段：**GREEN 结论前必须在场**（最终期限 FROZEN；围栏块置 Status 头部块下方固定位，一行一个目标路径）；判官随 GREEN 结论核对、缺项打回，主线程标 FROZEN 前再人工复核。

```landing-manifest
.claude/agents/integrator.md
.claude/agents/planner.md
.claude/commands/build-feature.md
.claude/commands/loop-retro.md
.claude/dev-loop.settings.json
.claude/hooks/enforce-scope.mjs
.claude/hooks/portfolio-sync.mjs
.claude/hooks/supervisor-gate-check.mjs
.claude/hooks/supervisor-verdict-gate.mjs
.claude/references/architect-operating-model.md
.claude/references/architecture-decisions.md
.claude/references/dev-loop-operating-system.md
.claude/references/dev-loop.md
.claude/references/path-form-family.md
.claude/references/pod-coordination.md
.claude/references/task-intake-and-slicing.md
.claude/rules/repo-structure.md
.claude/scope-map.json
.claude/scripts/brief-inputs.mjs
.claude/scripts/changes-index.mjs
.claude/scripts/closeout-check.mjs
.claude/scripts/devloop-artifact-check.mjs
.claude/scripts/lib/governance-gate-shared.mjs
.claude/scripts/loop-health-report.mjs
.claude/scripts/loop-metrics.mjs
.claude/scripts/merge-driver.mjs
.claude/scripts/pre-merge-escalation-gate.mjs
.claude/scripts/taskpackage-required-check.mjs
.claude/skills/arch-doc-restructure/SKILL.md
.claude/skills/kidpal-pod-launch-prep/SKILL.md
AGENTS.md
docs/DEV-GOVERNANCE.md
docs/requirements.md
docs/notes/validation-batch-conclusion.md
docs/templates/GOVERNANCE-MIGRATION-GUIDE.md
delivery/workstreams/README.md
delivery/workstreams/m1-home-qa/decision-coverage.json
delivery/workstreams/m1-home-qa/pending-human-ruling-260917-m1-k4-rma8-fixbatch.md
delivery/workstreams/m1-home-qa/review.md
delivery/workstreams/m1-home-qa/roadmap.md
delivery/workstreams/m2-char-window/progress.md
delivery/workstreams/m3-schedule/progress.md
delivery/workstreams/m4-star-notebook/decision-coverage.json
delivery/workstreams/m4-star-notebook/progress.md
firmware/main/test/HOST_TEST_CONVENTIONS.md
firmware/main/test/char_window_overlay.acceptance.c
firmware/main/test/keybar_three_slot.acceptance.c
firmware/main/test/key_layer_dispatch.acceptance.c
firmware/main/test/p1_cjk_name.acceptance.c
firmware/main/test/p1_home_battery.acceptance.c
firmware/main/test/reminder_overlay_unify.acceptance.c
firmware/main/test/retract_p4_three_page.acceptance.c
firmware/main/test/voice_round_ui.acceptance.c
firmware/main/test/acceptance/red-baseline/red-baseline-summary.md
firmware/main/test/acceptance/red-baseline/schedule-p2-red-baseline-summary.md
firmware/main/test/acceptance/red-baseline/schedule-reminder-red-baseline-summary.md
firmware/main/test/acceptance/red-baseline/screen-unification-red-baseline-summary.md
firmware/main/test/acceptance/red-baseline/story-p3-red-baseline-summary.md
server/acceptance/test_m3k3_voice_schedule_entry.acceptance.py
server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py
server/deploy/README.md
server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md
server/evidence/260903-gateway-directionB/cert-lifecycle-accept-risk.md
server/evidence/260903-gateway-directionB/deploy.md
server/evidence/260903-gateway-directionB/v21-multiturn-continuity-probe.md
server/evidence/260903-gateway-directionB/v23-asr-final-selection.md
tools/asset-pipeline/tests/test_art_assets.acceptance.md
tools/simulator/tests/test_lvgl_align.acceptance.md
delivery/taskpackages/INDEX.md
```

> manifest 口径：**只记批改件**（T2–T6 内容变更件）+ 再生件（T6）。T1 为纯搬移整裁腿、无内容锚（先例 260914-gov-review-delivery-restructure T1 landing 期修正：目录级迁入不可机判，搬移对账由 §5 V2 承载）；`delivery/changes/dev/INDEX.md` 为退役删除件、非目标锚（V4 判其不存在）。批改锚 69 件 = 仓级 census 命中活面件全集（§1 P2），历史面豁免件不入锚（§4 OQ2）。

---

## 0. 想实现的效果与差距

**目标效果一句话**：撤销 dev lane「晋升」环节——每个 dev-loop 包从生到死只有唯一一个家 `delivery/taskpackages/<id>/`（生前：plan / briefs / verify 证据；身后：review-packet / supervisor-verdict / pod-retro / docs-delta 等收尾制品），`delivery/changes/` 收纯为治理面台账（arch 变更 + intake spine + retro），包的完整故事一目录读齐。

**演进定位（先例衔接）**：本变更是 260914-gov-review-delivery-restructure 所立「taskpackages（短期在途）→ 晋升 delivery/changes/dev/（长期归档）」两段式生命周期的**反转**——晋升环节制造了一包两家：一个包的生前件在 taskpackages、身后件在 changes/dev，读档要跑两个目录拼故事；且晋升是纯机械搬运、不产生任何信息增量。反转后「仓库管正在发生的事，系统管发生过的事」的迁出预留（260914 §6）不受影响——将来收口推交付系统时按包整推 taskpackages/<id>/，反而比跨两目录收集更简单。

**现状差距**（2026-10-14 起草时点机械对账实测，命令与输出见 §1 P2；**快照时点纪律**：dev lane 在 FROZEN→landing 间仍可能前进，本节数字为起草时点快照、仅作差距论证留痕——R1 评审复测已漂移为 63 目录 / 62 重叠 / 399 独有文件——landing 判据一律以迁移窗口开启时点的迁移快照现跑值为准，见 §5 V2）：

- `delivery/changes/dev/` 59 个包目录中 58 个在 `delivery/taskpackages/` 有同名包目录——两边按生命周期阶段切分而非按包切分，一包两家。
- dev lane 独有文件（taskpackages 侧没有的）合计 **382 件**，即 382 件收尾制品与证据散落在「另一个家」；同名件 byte 对账仅 1 件漂移（260829-char-stroke/pod-events.jsonl，taskpackages 侧为超集，见 T1）。
- 1 个孤儿包 `260913-happy-flow-host-capture` 只在 dev lane 存在、无 taskpackages 侧——晋升机制自身的历史残缺实例。
- 承重机械层（closeout-check / supervisor 双闸 / changes-index / merge-driver 等 16 个脚本与钩子）按「closeout 制品在 changes/dev/<id>/」的旧布局写死查找根，布局与语义错位被机械层固化。

**非目标**：不改 `changes/` 目录名（human 2026-10-14 已拍保持）；不动 `delivery/changes/{arch,intake,retro}/` 三 lane 及其机制；不动契约面（实测零命中）；不改任何脚本 / 门的判定逻辑（仅路径常量与扫描根迁移；唯二行为变化在 T2 显式声明）；不动 `.codebase-memory/`；历史面零改动（豁免清单见 T4 / OQ2）；不新建 spine（单面治理变更）。

## 2. 问题、根因与改法（T1…T6）

**【硬项 · 起草期实证义务】**本节的断言纪律（照模板全文执行）；数字引用均已起草期现跑实测（命令 + 输出留痕如下）。

### T1 · dev lane 全量并入 taskpackages（纯搬移 + 重复件处置，整裁腿）

**问题**：dev lane 59 个包目录与 taskpackages 64 个包目录重叠 58 个，一个包的收尾制品住在「第二个家」，晋升搬运不产生信息增量、只产生两份维护面与孤儿残缺（260913 包无晋升件、pod-events 双份漂移）。

**证据**（2026-10-14 起草时点快照实测；R1 复测 63 目录 / 62 重叠 / 399 独有文件——dev lane 前进所致，终值以迁移快照为准见 V2）：

- 重叠对账：`comm -12`（dev lane 目录名 ∩ taskpackages 目录名）= 58；dev lane 独有目录 = `260913-happy-flow-host-capture`。
- 独有文件计数：逐包 `comm -23`（dev 侧文件 ∖ taskpackages 侧文件）+ 孤儿包全量 `find -type f`，合计 **382 文件**（含子目录内文件递归计入）。
- 同名件漂移对账：全量 `cmp -s` 逐件核对，漂移仅 **1 件** `260829-char-stroke/pod-events.jsonl`（dev 侧 11 行 / taskpackages 侧 16 行）；eventId 子集验证（dev 侧 11 个 eventId 全部含于 taskpackages 侧 16 个）差集 = 0，即 taskpackages 侧为超集（多 5 条后续事件、仅 JSON 序列化格式不同），无信息丢失。
- 目录级对账：`260829-char-stroke/verifier-evidence/` 递归 `diff -rq` 一致（census 初版曾标 DRIFT 为目录比较假阳性，已排除）。

**根因**：260914 重组把「晋升」设计成 dev-loop 包的第二个生命周期阶段，晋升是纯 `git mv` 式搬运——机制上必然产生「同一包两个目录、内容互斥拼合」的布局，且晋升遗漏（孤儿包）与双写漂移（pod-events）是该机制的固有残缺而非操作失误。

**改法**：`git mv` 保全量——58 个镜像包的 382 件 dev-独有文件逐一搬入同名 `delivery/taskpackages/<id>/`；孤儿包 `260913-happy-flow-host-capture/` 整目录搬入 `delivery/taskpackages/`；同名 byte 一致件与已验超集的漂移件（pod-events.jsonl 一件）丢 dev 侧副本，对账证据留 landing-summary；dev lane 目录壳（含旧 `delivery/changes/dev/INDEX.md` 投影件）退役删除。landing 期生成逐件搬移清单（源路径失踪 + 目标路径在场 + 丢弃副本名单）留 review 证据目录。

**前置条件**：迁移窗口内无在途 Pod worktree（同先例 T1；Pod 分支基于旧路径，rebase 会大面积冲突）；landing 前 human / 主线程确认。

**对应根因**：P1。

目标文件：无（整裁腿——纯搬移与退役删除，无内容批改件；对账由 §5 V2 / V4 承载）

### T2 · 机械层路径批改（scripts / hooks / settings / scope-map）

**问题**：16 个脚本与钩子把「closeout 制品在 delivery/changes/dev/&lt;id&gt;/」的旧布局写死为路径常量，包搬家后闸与索引全部失联。

**证据**（2026-10-14 grep 实测，file:line 摘引）：

- closeout 制品查找根：`closeout-check.mjs:183`（`resolve(projectDir, 'delivery', 'changes', 'dev')` 候选根 + `:250` fail 文案）、`supervisor-gate-check.mjs:634`、`supervisor-verdict-gate.mjs:544,564,574`（write-once 写入位 + closeoutArtifacts + verdictRef）。
- main 侧合并后校验：`supervisor-gate-check.mjs:169`（`git cat-file -e main:delivery/changes/dev/<id>/docs-delta.md`）。
- 索引扫描根：`changes-index.mjs:160`（dev 扫描根）与 `:5`（输出路径 `delivery/changes/dev/INDEX.md`）。
- 读取位 / fallback：`brief-inputs.mjs:293`、`pre-merge-escalation-gate.mjs:64,134,139,310,331`、`loop-health-report.mjs:77,129,178`、`loop-metrics.mjs:325,370`、`merge-driver.mjs:190,903,1480`、`taskpackage-required-check.mjs:149,173,381`、`devloop-artifact-check.mjs:167`（均为日期 fallback 或证据读取位）。
- 触发条件与提示句：`portfolio-sync.mjs:2,39`、`enforce-scope.mjs:61`、`dev-loop.settings.json:15`。
- scope-map 写槽：`:26`（doc 注记）、`:55`（integrator allow 第三项 `delivery/changes/dev/**/pod-retro.md`）、`:83`（allow 列表内 `delivery/changes/dev/**`）。
- 自检 fixture 字符串：`supervisor-gate-check.mjs:416-460`、`supervisor-verdict-gate.mjs:685-733`、`merge-driver.mjs:1172-1178`、`lib/governance-gate-shared.mjs:534,537`。

**根因**：路径常量与布局耦合；布局反转后常量必须随迁，判定逻辑本身不动。

**改法**：

- **扫描根 / 查找根迁移**（显式行为变化 ①②）：changes-index dev 扫描根与 INDEX 输出改指 `delivery/taskpackages`（扫描排除 `current/` 活工作面）与 `delivery/taskpackages/INDEX.md`（落址裁决见 OQ1）；closeout-check 候选根同步迁移（同样排除 `current/`），supervisor 双闸的 closeout 查找、写入、main 侧校验全部改指 `delivery/taskpackages/<id>/`。
- **行为变化连带声明**：INDEX 覆盖面从起草快照 59 涨到**迁移时点全量**（起草快照 65 / R1 复测 67；终值 = 迁移快照时点 taskpackages 非 current 目录数 + dev lane 独有目录数，landing 期现跑 census 对账，见 V2）。从未晋升包（起草快照 6 件：`260829-audio-bargein-hw`、`260829-shim-skeleton`、`260920-gov-review-dispatch-telemetry-source`、`260924-ops-openclaw-family-user-mapping`、`261013-lang-config-thinking-fix`、`261013-qq-schedule-write-closure`；R1 复测剩 4——后两件已随 dev/pod-m2 合并晋升，集合随 dev lane 前进继续收敛）迁移时点全量首次入册，状态按既有 origin / 状态文件推导，landing 期核验。此为设计目标（全量台账）而非副作用。
- 其余文件按「旧根 → 新根」平移路径常量、提示句、fixture 字符串；scope-map integrator allow 的 dev-lane pod-retro 专项条目剔除（taskpackages/** 已覆盖）、allow 列表剔除 dev-lane 项；portfolio-sync 触发条件删 changes/dev 分支。**逻辑零变更**。
- 落地按 path-form-family.md 八形态清点：本批机械面命中形态 = 斜杠形 + 分段单引号 + 分段双引号（`'delivery', 'changes', 'dev', …` → 删中段）+ 自检 fixture 字符串形；Python join / C 实参形仅涉 T5 server 件。

**对应根因**：P2。

目标文件：`.claude/scripts/changes-index.mjs`（改）· `.claude/scripts/closeout-check.mjs`（改）· `.claude/hooks/supervisor-gate-check.mjs`（改）· `.claude/hooks/supervisor-verdict-gate.mjs`（改）· `.claude/scripts/brief-inputs.mjs`（改）· `.claude/scripts/pre-merge-escalation-gate.mjs`（改）· `.claude/scripts/loop-health-report.mjs`（改）· `.claude/scripts/loop-metrics.mjs`（改）· `.claude/scripts/merge-driver.mjs`（改）· `.claude/scripts/taskpackage-required-check.mjs`（改）· `.claude/scripts/devloop-artifact-check.mjs`（改）· `.claude/scripts/lib/governance-gate-shared.mjs`（改）· `.claude/hooks/portfolio-sync.mjs`（改）· `.claude/hooks/enforce-scope.mjs`（改）· `.claude/dev-loop.settings.json`（改）· `.claude/scope-map.json`（改）

### T3 · 治理文档面批改（结构 SSOT + agent / command / reference / skill）

**问题**：结构单一事实源与治理文档仍把「晋升 changes/dev」描述为正形，agent 与 loop 按旧指引工作。

**证据**：`.claude/rules/repo-structure.md` 顶层地图 `delivery/taskpackages/` 行（「永不允许当长期归档用（长期在 delivery/changes/dev/）」）、`delivery/changes/dev/**` 行、同名物消歧表 INDEX 落址行、「工作产物去哪」晋升句；`AGENTS.md` 治理路由节 lane 拆分句（dev 段地址）；DEV-GOVERNANCE.md 6 处（R1-F7 复测 `grep -c` 6 行）；agents 2 件、commands 2 件、references 7 件、skills 2 件（清单 = manifest 对应段，grep file:line 逐处落地期核对）。

**根因**：文档面描述跟随旧布局。

**改法**：repo-structure.md 四处改写——`delivery/taskpackages/` 行翻转（包全程唯一家，收尾制品同目录）、`delivery/changes/dev/**` 行删除、同名物消歧表 dev 段 INDEX 落址更新、「工作产物去哪」晋升句删除；AGENTS.md lane 拆分句 dev 段改指新 INDEX 落址；其余文件路径引用与提示句平移。逻辑与职责边界零变更。

**对应根因**：P2、P3。

目标文件：`.claude/rules/repo-structure.md`（改）· `.claude/agents/integrator.md`（改）· `.claude/agents/planner.md`（改）· `.claude/commands/build-feature.md`（改）· `.claude/commands/loop-retro.md`（改）· `.claude/references/architect-operating-model.md`（改）· `.claude/references/architecture-decisions.md`（改）· `.claude/references/dev-loop-operating-system.md`（改）· `.claude/references/dev-loop.md`（改）· `.claude/references/path-form-family.md`（改）· `.claude/references/pod-coordination.md`（改）· `.claude/references/task-intake-and-slicing.md`（改）· `.claude/skills/arch-doc-restructure/SKILL.md`（改）· `.claude/skills/kidpal-pod-launch-prep/SKILL.md`（改）· `AGENTS.md`（改）· `docs/DEV-GOVERNANCE.md`（改）· `docs/templates/GOVERNANCE-MIGRATION-GUIDE.md`（改）

### T4 · docs 与 workstreams 活文本批改 + 历史面豁免

**问题**：活文档与 workstream 工作记忆引用旧地址；搬家后相对链接断链。

**证据**：`docs/requirements.md:622`（INDEX 地址引用，活文本行）；`docs/notes/validation-batch-conclusion.md:5,57`（指向 dev lane 证据的相对链接，搬家即断）；workstreams 活文件 9 件（README 边界表 + 4 个活跃 workstream 的 progress / roadmap / decision-coverage / pending-human-ruling）。

**根因**：文档引用与布局耦合。

**改法**：上述活文本地址批改（指针刷新、语句语义不动）；**历史面豁免清单**（零改动，读档按当时语境理解）：`delivery/changes/arch/**` 全部 37 批历史评审制品、`delivery/changes/intake/261011-p1-home-v5-rework.md`（spine 的 anchors 为冻结态记录）、`docs/ADR.md` 历史行（append-only 日志）、`docs/debt/**` 条目正文（append-only 散文；原 `docs/crq/`，261014-gov-review-crq-debt-rename 批于 2026-10-14 改名，R1 复测 15 文件命中——路径是当时事实，批改即篡改记录）、`docs/showme/` 2 件（一次性可视化存档）、firmware / server `acceptance/archive/` 退役测试与 README、`delivery/workstreams/_archive/**`、`.claude/loop-retro/ledger.jsonl`、taskpackages 包内历史行（pod-events / verifier-evidence rounds / taskpackage.json origin 字段 / plan / briefs / launch-card 等事件记录与冻结制品——路径是当时事实，批改即篡改记录；计数以迁移快照为准：起草快照 16 系计数基错、R1 复测 taskpackages 侧 169 件，迁入的 dev lane 包内件 R1 复测 179 件同归本类）。

**对应根因**：P2、P3。

目标文件：`docs/requirements.md`（改）· `docs/notes/validation-batch-conclusion.md`（改）· `delivery/workstreams/README.md`（改）· `delivery/workstreams/m1-home-qa/decision-coverage.json`（改）· `delivery/workstreams/m1-home-qa/pending-human-ruling-260917-m1-k4-rma8-fixbatch.md`（改）· `delivery/workstreams/m1-home-qa/review.md`（改）· `delivery/workstreams/m1-home-qa/roadmap.md`（改）· `delivery/workstreams/m2-char-window/progress.md`（改）· `delivery/workstreams/m3-schedule/progress.md`（改）· `delivery/workstreams/m4-star-notebook/decision-coverage.json`（改）· `delivery/workstreams/m4-star-notebook/progress.md`（改）

### T5 · 实现面批改（功能常量 + 注释指针 + 证据件）

**问题**：活树内 2 处**功能性路径常量**（测试运行时实读目录）与若干注释 / 消息串 / 证据摘要指向 dev lane，搬家后 offline 也会红。

**证据**（2026-10-14 实测）：

- `server/acceptance/test_m3k3_voice_schedule_entry.acceptance.py:192-194`：`DEVICE_ROUND_RECORD = (REPO_ROOT / "delivery" / "changes" / "dev" / "260921-m3-k3-voice-schedule-entry-live" / "device-round-record.md")`——运行时实读。
- `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py:203-206`：`LIVE_EVIDENCE_DIRS` 首项指向 dev lane（第二项已是 taskpackages 侧、保留）；`:106` 文档句与 `:1030` 失败消息串同步。
- 注释 / 指针面：firmware 活测试 8 件 `.c`（char_window_overlay / keybar_three_slot / key_layer_dispatch / p1_cjk_name / p1_home_battery / reminder_overlay_unify / retract_p4_three_page / voice_round_ui，全注释内证据指针）+ `HOST_TEST_CONVENTIONS.md`；red-baseline 摘要 5 件；server deploy 2 件；`server/evidence/260903-gateway-directionB/` 4 件；tools 2 件。

**根因**：先例 260914 批 R1 的同一形态——census 不限扩展名才能看见；功能常量不改必红。

**改法**：功能常量改指 taskpackages 对应路径（`DEVICE_ROUND_RECORD` 改 `delivery/taskpackages/<id>/device-round-record.md`；`LIVE_EVIDENCE_DIRS` 首项改指 taskpackages 包根）；注释 / 消息串 / 证据摘要指针平移。**先例同构裁定**：证据件批改沿 260914 批 OQ2(a) 逻辑（活树无死链、批改内容纯指针），本次不再单列 OQ。firmware / server `archive/` 目录豁免（退役测试，见 T4 豁免清单）。

**对应根因**：P2。

目标文件：`server/acceptance/test_m3k3_voice_schedule_entry.acceptance.py`（改）· `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py`（改）· `server/deploy/README.md`（改）· `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md`（改）· `server/evidence/260903-gateway-directionB/cert-lifecycle-accept-risk.md`（改）· `server/evidence/260903-gateway-directionB/deploy.md`（改）· `server/evidence/260903-gateway-directionB/v21-multiturn-continuity-probe.md`（改）· `server/evidence/260903-gateway-directionB/v23-asr-final-selection.md`（改）· `firmware/main/test/HOST_TEST_CONVENTIONS.md`（改）· `firmware/main/test/char_window_overlay.acceptance.c`（改）· `firmware/main/test/keybar_three_slot.acceptance.c`（改）· `firmware/main/test/key_layer_dispatch.acceptance.c`（改）· `firmware/main/test/p1_cjk_name.acceptance.c`（改）· `firmware/main/test/p1_home_battery.acceptance.c`（改）· `firmware/main/test/reminder_overlay_unify.acceptance.c`（改）· `firmware/main/test/retract_p4_three_page.acceptance.c`（改）· `firmware/main/test/voice_round_ui.acceptance.c`（改）· `firmware/main/test/acceptance/red-baseline/red-baseline-summary.md`（改）· `firmware/main/test/acceptance/red-baseline/schedule-p2-red-baseline-summary.md`（改）· `firmware/main/test/acceptance/red-baseline/schedule-reminder-red-baseline-summary.md`（改）· `firmware/main/test/acceptance/red-baseline/screen-unification-red-baseline-summary.md`（改）· `firmware/main/test/acceptance/red-baseline/story-p3-red-baseline-summary.md`（改）· `tools/asset-pipeline/tests/test_art_assets.acceptance.md`（改）· `tools/simulator/tests/test_lvgl_align.acceptance.md`（改）

### T6 · 派生索引再生

**问题**：旧 dev INDEX 是旧扫描根的投影；扫描根迁移后必须全量重建，否则全表死链（先例 260914 批 R4 教训）。

**证据**：`delivery/changes/dev/INDEX.md` 头注自述「AUTO-GENERATED 勿手改（投影，源 = 各包 origin / 目录）」；`delivery/changes/.changes-index-state.json` 实测不含路径常量（grep `changes/dev` 零命中），随扫描根迁移无需批改。

**根因**：INDEX 是生成件，源变则再生。

**改法**：landing 期 `node .claude/scripts/changes-index.mjs` 无参全量重建（新落址 `delivery/taskpackages/INDEX.md`）+ `--upsert 261014-gov-review-dev-lane-merge-taskpackages`；旧 `delivery/changes/dev/INDEX.md` 已随 T1 退役不复活。预期行数 = V2 迁移快照推导目录数（起草快照 65 / R1 复测 67；T2 行为变化连带声明的核验位）。

**对应根因**：P1、P2。

目标文件：`delivery/taskpackages/INDEX.md`（新增）

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

census 命令（2026-10-14 起草期全仓预跑，不限扩展名——先例 R1 盲区教训内建）：

```bash
grep -rlE 'changes/dev' \
  --exclude-dir=.git --exclude-dir=.codebase-memory --exclude-dir='ESP32*' \
  --exclude-dir=build --exclude-dir=managed_components --exclude-dir=node_modules \
  --exclude-dir=__pycache__ .
```

命中集 = manifest 批改件 69 ∪ T4 豁免清单全集（arch 制品 / intake spine / ADR / debt / showme / archive 测试 / workstreams _archive / ledger / taskpackages 包内历史件），两类并集与 census 命中一一对应、判官 spot-check；契约面 `grep 'changes/dev' docs/contracts/interface-contract.md` 零命中。

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| 目录迁移 | dev lane 起草快照 59 包目录、382 文件（终值以迁移快照为准）→ taskpackages | G2 | T1 整裁腿纯搬移；重复件处置规则与对账证据见 T1 |
| 机械强制层 | scripts 11、hooks 4、settings 1、scope-map | G3 | 路径常量平移 + 扫描根迁移；判定逻辑零变更；唯二主行为变化（INDEX 落址与覆盖面扩至迁移时点全量、closeout 查找根）+ 连带声明显式落 T2 |
| 治理文档 | AGENTS.md、repo-structure、agents 2、commands 2、references 7、skills 2、DEV-GOVERNANCE、templates 1 | G2+G3 | 指针与提示句批改；职责边界零变更 |
| docs 与 workstreams 活文本 | requirements 1 行、notes 1 件、workstreams 活件 9 | G2 | 指针刷新；历史面豁免见 T4 清单 |
| 实现面 | server 2 `.py`（功能常量）+ deploy 2 + evidence 4；firmware `.c` 8 + conventions 1 + red-baseline 5；tools 2 | G2 | 功能常量改后 offline 齿仍绿（V5）；批改面仅常量 / 注释 / 消息串 / 指针 |
| 契约面 | — | 未触 | grep 零命中 |
| CodebaseMemory | — | 未触 | delivery/ 已整树排除（260914 批 T5 既有裁定）；本项目索引不涉 |
| `.changes-index-state.json` | 1 | 未触 | 无路径常量（实测零命中）、状态缓存随扫描根自然迁移 |

**【硬项 · 复扫模式完备性先验】**残留清扫模式集：斜杠形 `changes/dev`（主命令）；分段字面量形 `['"]changes['"]\s*,\s*['"]dev` 与双引号 join 形 `"changes" / "dev"`（.claude 机械面 + server 活测试面各一子检）；转义形 `changes\/` 单独扫。豁免件按 T4 清单逐类排除——清扫模式与验收判据同源（path-form-family 第四节教训），V1 子检与落地期清点用同一条模式。

## 4. human 取舍点（OQ）

**OQ1 · dev INDEX 落址（dev lane 消失后台账住哪）**
- (a) `delivery/taskpackages/INDEX.md`（**推荐**）：台账跟着包走，taskpackages 自带完整台账；changes/ 只剩 arch 一份 INDEX，职责更纯；与「一包一家」目标同构。
- (b) 保留 `delivery/changes/dev/INDEX.md`：changes/ 留一个无 lane 内容的空壳目录只为放一份投影，与撤销 dev lane 的目标自相矛盾。
- (c) dev 段并回 `delivery/changes/INDEX.md` 单文件：推翻 ARQ-260915-changes-index-lane-split 的 lane 拆分裁定，且把 dev-loop 状态机行混进治理台账。
- **裁定**：（a）taskpackages/INDEX.md——human 2026-10-14 立项 ack 时拍定；T2 / T6 按 (a) 书写，与此一致。

**OQ2 · 历史面豁免清单确认**
- (a) 全豁免 + 留根（**推荐**）：arch 历史评审制品 / intake spine / ADR 与 crq 日志行 / showme / archive 退役测试 / taskpackages 包内事件记录，均为 append-only 或冻结态记录，批改即篡改历史；stale 指针读档按当时语境理解（260914 批 P4 原则成例）。
- (b) 全量批改：活树零旧引用、机检豁免名单最短，但要重写 37 批历史评审制品与大量事件记录，违背 append-only 纪律。
- **裁定**：（a）全豁免 + 留根——human 2026-10-14 立项 ack 时拍定；豁免清单全文见 T4，起草按 (a) 书写，与此一致。

## 5. 验证集（landing 前机械前置 + landing 后核验）

**V1 · 残留清扫机检（与落地期清点同一条模式，双时点标注：本条为 post-commit 判定）**——主命令 = §3 census 命令的 `-n` 内容版，预期命中 = T4 豁免清单全集 + 本 PROPOSAL 自身所在 arch 制品目录；逐类计数核对（豁免类逐类 `-c` 计数与 landing-summary 记录一致；计数基 = 迁移快照 census 现跑值，起草 / R1 计数为时点留痕、不作闸）。子检：

```bash
# 子检 A · 斜杠形主命令（预期命中仅豁免清单 + 本批 arch 制品）
grep -rEnI 'changes/dev' \
  --exclude-dir=.git --exclude-dir=.codebase-memory --exclude-dir='ESP32*' \
  --exclude-dir=build --exclude-dir=managed_components --exclude-dir=node_modules \
  --exclude-dir=__pycache__ . \
  | grep -vE 'delivery/changes/arch/|delivery/changes/intake/|docs/debt/|docs/showme/|docs/ADR\.md|loop-retro/ledger\.jsonl|acceptance/archive/|workstreams/_archive/|^\./docs/notes/validation-batch-conclusion\.md'
# 子检 B · 分段字面量形（.claude 机械面，无过滤直断零命中——R1-F2）
grep -rEn "['\"]changes['\"]\s*,\s*['\"]dev|['\"]delivery['\"]\s*,\s*['\"]changes['\"]\s*,\s*['\"]dev" .claude/hooks .claude/scripts .claude/dev-loop.settings.json .claude/scope-map.json
# 子检 C · Python join 形（server 活测试，预期零命中）
grep -rEn "['\"]changes['\"] */ *['\"]dev" server/acceptance --include='*.py' | grep -v archive
# 子检 D · 转义形（预期零命中，承 §3「转义形单独扫」）
# ⚠ 执行通道（R2-F8 探针实证）：agent Bash 工具通道会吞一层反斜杠（grep 实收 changes\/dev、退化成 plain 形）
#   ——会误红数百行 plain 命中、转义形真残留反逃检。须经字节保真通道执行：
#   ① Grep 工具（ripgrep，模式 changes\\/dev），或 ② 本命令经 Write 落脚本文件（exact bytes）再 bash 执行。勿直跑。
grep -rEn 'changes\\/dev' \
  --exclude-dir=.git --exclude-dir=.codebase-memory --exclude-dir='ESP32*' \
  --exclude-dir=build --exclude-dir=managed_components --exclude-dir=node_modules \
  --exclude-dir=__pycache__ .
```

子检 B / C / D 预期零命中（R1-F2：子检 B 原尾接 `-v "['\"]delivery['\"]"` 把完整三段形 `resolve(p, 'delivery', 'changes', 'dev')` 全部吞掉、对该形成空断言，已去除——新前缀行本就不匹配 A|B 模式，无需剔除；R1-F6：pattern `changes/dev` 不匹配带反斜杠的 `changes\/dev` 文本，转义形须独立扫，故设子检 D；子检 D 须经字节保真通道执行（Grep 工具或 Write 落脚本再 bash）——agent Bash 工具通道实测吞一层反斜杠（R2-F8 探针两路复现），直跑即误红 plain 形、转义形反逃检）。子检 A 预期命中仅剩：豁免清单内文件行 + 誤差应为零（`docs/notes/validation-batch-conclusion.md` 已批改故列入剔除列防误报，批改后其行内已无旧串、剔除列冗余无害）。

**V2 · 搬移对账（pre-commit，期望值 = 迁移快照现跑推导，不硬编码起草数）**：迁移窗口开启时点先现跑 census 定迁移快照（taskpackages 非 current 目录数、dev lane 目录数与重叠数、dev 独有文件数），记录于 landing-summary；`test ! -d delivery/changes/dev`（lane 目录不复存在）；`ls -d delivery/taskpackages/*/ | grep -v current | wc -l` = 快照 taskpackages 非 current 目录数 + 快照 dev lane 独有目录数（起草快照 64 + 1 / R1 复测 66 + 1）；landing 期生成的逐件搬移清单（快照 dev 独有文件全集：起草快照 382 / R1 复测 399）逐项核「源失踪 + 目标在场」，丢弃副本名单（同名 byte 一致件 + pod-events.jsonl 一件，快照时点重验超集）在 landing-summary 记录并附 eventId 超集验证输出。

**V3 · 承重脚本自检（pre-commit）**：`changes-index`、`supervisor-gate-check`、`supervisor-verdict-gate`、`merge-driver`、`pre-merge-escalation-gate`、`landing-fidelity-check` 逐一 `--self-check`（或各件既有自检入口）全绿；`closeout-check` 无 self-check 入口（旗面仅 `--update-index` / `--read-only`，R1-F4 起草期亲核失守、R1 复核确认），以 `--read-only` 对迁移后目录干跑替代——exit 0 且 fail 文案指向新根。

**V4 · INDEX 再生（pre-commit）**：`node .claude/scripts/changes-index.mjs` 无参重建 exit 0；`delivery/taskpackages/INDEX.md` 在册且 dev 段行数 = V2 迁移快照推导目录数（起草快照 65 / R1 复测 67）、含本变更行；`test ! -e delivery/changes/dev/INDEX.md`。

**V5 · 功能常量离线验证（pre-commit）**：`pytest server/acceptance/test_m3k3_voice_schedule_entry.acceptance.py --collect-only -q` exit 0（导入期常量构造成功）；`pytest server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py --collect-only -q` exit 0；`test -f delivery/taskpackages/260921-m3-k3-voice-schedule-entry-live/device-round-record.md`（DEVICE_ROUND_RECORD 新指向在场）。

**V6 · git 卫生（post-commit）**：`git diff --cached --stat` 先核后 commit（Hard Rules）；`git log --follow` 抽查 2 件搬移件（1 件镜像包收尾文件 + 1 件孤儿包文件）确认历史连续。

**V7 · 自检干跑（FROZEN 前置，pre-commit）**：`node .claude/scripts/landing-fidelity-check.mjs --preflight <本 PROPOSAL 路径>` 报告 anchorUnion 与 manifest 逐 token 一致（69 锚、无注记夹带路径 token）。

## 6. 留根清单

- **历史面 stale 指针**：T4 豁免清单全集——读档按当时语境理解；将来对应文件实质修订时随批携带刷新，不单独立项。
- **INDEX 覆盖面扩容的状态推导核验**：从未晋升包（迁移时点全量，起草快照 6 / R1 复测 4，随 dev lane 前进继续收敛）首次入册，其状态行（closed / running）由 changes-index 既有推导逻辑得出——landing 期人工复核迁移快照清单各行状态与实际相符，失真则登记 ARQ（推导逻辑问题归脚本，不归本布局变更）。
- **交付系统迁出预留**（260914 §6 既有）：收口推系统时按包整推 `delivery/taskpackages/<id>/`——本变更使推系统收集面从一个目录完成，预留路径更简；推系统机制届时单独立项。
- **Pod worktree 前置**：迁移窗口须无在途 Pod（T1 前置条件）；下批 launch 前人工复核 kidpal-pod-launch-prep skill 指引已随 T3 批改。
- **登记不硬治**：孤儿包晋升残缺与 pod-events 双写漂移是旧晋升机制的固有产物，随本变更消亡，不追溯归因。
- **自律件**：搬移用脚本化批量 + 逐件对账清单（起草快照 382 件、终值以迁移快照为准，不逐一手搬）；批改按 path-form-family 八形态清点，样本件（changes-index.mjs、closeout-check.mjs、repo-structure.md）试跑核对再全量；锚行注记不夹带路径 token（起草期已按模板读法注 3 自检）。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 · 2026-10-14 · Track B 第 2 步 architect 评审判官（独立实例 R1）

**核验方法**：现读 PROPOSAL 全文 + `arch-review-protocol.md`「双轨路由」节 + `PROPOSAL.template.md` 后动工；跑了 `landing-fidelity-check.mjs --preflight` 只读干跑、§3 census 命令全量复跑、契约面 grep 复证、目录与文件对账复算（comm / find / wc）、file:line 引据抽验 15 处、两处探针实证（子检 B 过滤行为、pod-events 超集）。

**通过项**（正面确认，不改判）：

1. **结构硬条款合格**：preflight 干跑 `ok: true`、`anchorCount: 69`、`anchorUnion` 69 token 与 manifest 围栏块逐 token 一致、`advisories: []`、无注记夹带路径 token；围栏块置 Status 头部块下方固定位、一行一个裸路径。T1 锚行「目标文件：无（整裁腿……）」无夹带，T6 锚行干净。
2. **升级判据自查成立、Track B 合轨**：契约面 `grep 'changes/dev' docs/contracts/interface-contract.md` 复证零命中（exit 1）；固件 / 服务端触面经抽验均为注释指针与路径常量（`server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py:203-206` 的 `LIVE_EVIDENCE_DIRS` 首项、`firmware/main/test/char_window_overlay.acceptance.c:2311` 注释等），无协议 / 语义变更，非跨系统结构变更。
3. **OQ 裁定回灌一致**：OQ1(a) 下 T2 扫描根与输出落址、T6 目标文件、V2 `test ! -e delivery/changes/dev/INDEX.md`、V4 新 INDEX 判据均指 `delivery/taskpackages/INDEX.md`；OQ2(a) 下 T4 全豁免 + 留根与裁定一致（但见 F3 的豁免面失真）。
4. **file:line 引据抽验 15 处全部属实**：`closeout-check.mjs:183`（`resolve(projectDir, 'delivery', 'changes', 'dev')`）与 `:250` fail 文案、`changes-index.mjs:5` 与 `:160`、`supervisor-gate-check.mjs:169`（main 侧 cat-file）与 `:634`、`supervisor-verdict-gate.mjs:544/564/574`、`scope-map.json:26/55/83`、两个 acceptance py 共 6 处、`requirements.md:622`、`docs/notes/validation-batch-conclusion.md:5,57`、`portfolio-sync.mjs:2,39`、fixture 行号段四处（`supervisor-gate-check.mjs:416-420`、`supervisor-verdict-gate.mjs:685-687`、`merge-driver.mjs:1172-1178`、`lib/governance-gate-shared.mjs:534,537`）。
5. **pod-events 超集断言复证成立**：dev 侧 11 行 eventId 全含于 taskpackages 侧 16 行（差集 0），tp 侧多 5 条；「仅 JSON 序列化格式不同」属实（tp 侧带空格、dev 侧紧凑形）。
6. **V1 子检 B / C 的范围限定经独立普查成立**：分段字面量形（`['\"]changes['\"]\s*,\s*['\"]dev`）在 `.claude/` 与 `server/acceptance/` 之外全仓实测零命中，子检范围限定无盲区。
7. **判定逻辑零变更声明核验通过**：portfolio-sync 触发条件删 changes/dev 分支（`:39`）、scope-map allow 剔除 dev-lane 项、changes-index 扫描排除 `current/`，均已在本 PROPOSAL T2 显式声明且迁移后效果不构成判定逻辑变化（迁后制品均在 `delivery/taskpackages/` 前缀下、portfolio-sync 首条件仍覆盖；taskpackages/** 已覆盖被剔除的 allow 面）。未发现未声明的行为变化。

**打回项**（须逐条落结后才可 GREEN）：

- **F1（BLOCKING）· 数字集与仓库现态漂移，V2 / V4 冻结判据会机械误红**。PROPOSAL 声称 dev lane 59 目录 / 重叠 58 / 独有 382 文件 / taskpackages 64 / 迁后 65 目录 / INDEX 覆盖面 59→65 / 6 个从未晋升包。评审时点（21:05 `dev/pod-m2` 合并 + PR #5 之后）实测：dev lane 63 目录、重叠 62、独有 399 文件、taskpackages 非 current 66、迁后应为 67；`261013-lang-config-thinking-fix` 与 `261013-qq-schedule-write-closure` 两包已晋升（现从未晋升包 = 4）。V2 的 `wc -l = 65` 与 V4 的「dev 段 65 行」是 FROZEN 后 landing 闸判据，照单冻结则正确 landing 也会 fail-loud 误红；§6「6 行状态复核」的复核对象已错 2 行。处置：FROZEN 前把硬编码计数改为「迁移前 census 快照」同源推导语义（landing 期现跑 census、期望值 = 快照计数 + 孤儿包 1，逐断言与快照对账），并对「dev lane 在 FROZEN→landing 间仍可能前进」显式留一句快照时点纪律。不升级 human：设计方向与两项 OQ 裁定均不受影响，属起草事实刷新。
- **F2（BLOCKING）· V1 子检 B 自噬过滤，完整三段形成为清扫盲区**。子检 B 管道 `grep -E "A|B" | grep -v "['\"]delivery['\"]"` 中，`-v` 剔除的是「任何含 quoted delivery token 的行」——而模式 B（`['\"]delivery['\"]\s*,\s*['\"]changes['\"]\s*,\s*['\"]dev`）的全部命中天然含该 token，故完整三段形永远逃检。探针实证：行 `x = resolve(p, 'delivery', 'changes', 'dev');` 被 -v 后过滤消失、仅剩无 delivery 前缀的裸两段形命中。V1 辩解句「新前缀行含 delivery 段被第二段剔除」不成立——新前缀行（taskpackages）本就不匹配 A|B，无需剔除；被剔除的唯一对象恰是本该抓的残留三段形，而该形是 `.claude` 机械面真实主流写法（`closeout-check.mjs:183` 即是）。正中 260914 批「最隐蔽形态四轮机检全漏」教训。处置：去掉 `-v`，或改为精确剔除新前缀 token（如 `-v "taskpackages"`），预期零命中断言须让任何形态匹配都 fail-loud。
- **F3（BLOCKING）· 豁免面路径过期 + 类计数失真，V1 剔除列与逐类计数基错**。其一：`docs/crq` 已于本日 14:51 由 261014-gov-review-crq-debt-rename 批落地改名 `docs/debt`（早于本 PROPOSAL 起草提交 21:04），T4 豁免清单与 V1 剔除列仍写 `docs/crq/`；实测 `docs/debt` 下 15 件含 `changes/dev`，post-commit V1 子检 A 将对它们报非豁免命中（误红）。其二：T4「taskpackages 包内历史行……实测 16 文件命中」与 §3 同一条 census 命令实测 169 件（无一在 `current/`）相差一个数量级；16 对不上任何真实豁免类（真实 16–21 量级的是 acceptance/archive 17 件 + workstreams/_archive 4 件一类）。T1 迁入的 dev lane 包内件（census 内 179 件）迁后同归 taskpackages 历史件类，豁免类语义本身覆盖，但计数锚错则 V1「逐类 -c 计数与 landing-summary 一致」失去基。处置：crq→debt 全量改写；taskpackages 历史件类计数重测落锚（评审时点 169 + 迁入 179 的快照口径一并写清）。
- **F4（SHOULD）· V3 引用 closeout-check 的自检入口不存在**。`closeout-check.mjs` 旗面仅 `--update-index` / `--read-only`（未知旗拒绝，`:65-67`），无任何 self-check 入口；V3 将其列入「逐一 `--self-check`（或各件既有自检入口）」不可执行，触模板硬条款「凡引用脚本入口 → 起草期亲核存在」。其余 6 件（changes-index / supervisor 双闸 / merge-driver / pre-merge-escalation-gate / landing-fidelity-check）含 self-check 字样属实。处置：V3 为 closeout-check 指定可执行替代（如 fixture 干跑 `--read-only`）或移出清单并注明。
- **F5（NOTE）· 「唯二行为变化」措辞偏紧**。INDEX 覆盖面扩容、扫描排除 `current/` 新语义、portfolio-sync 触发条件删支、scope-map allow 剔除均为已显式声明的连带，不构成误导；建议措辞改为「唯二主行为变化 + 连带声明」以免与 T2 自身的连带声明段形成两读。
- **F6（NOTE）· 转义形论证句技术上不成立但结论实测成立**。V1 称「转义形 `changes\/` 经子检 A 的 `-E` 模式天然覆盖——`changes/dev` 中 `/` 无需转义即可匹配」——该推理把「pattern 无需转义」错当「文本带反斜杠也能命中」：pattern `changes/dev` 不匹配文本 `changes\/dev`。全仓实测（ripgrep 字面反斜杠匹配）转义形零命中，结论碰巧成立；且本句与 §3「转义形 `changes\/` 单独扫」自相矛盾。处置：删错误论证句，保留实测零命中留痕。
- **F7（NOTE）· DEV-GOVERNANCE.md 计数差 1**。T3 称「DEV-GOVERNANCE.md 5 处」，`grep -c` 实测 6 行（`:30/:138/:160/:217/:234/:252`）；T3 已声明落地期逐处核对，修订时顺手改数即可。

**处置总表**：F1 / F2 / F3 须 FROZEN 前修订并回 R2 复审；F4 一并修；F5 / F6 / F7 随批顺手改。三项 BLOCKING 均为验证集与起草事实失真，不触目标效果（设计方向、OQ1(a) / OQ2(a) 裁定、「一包一家」目标效果均不动摇），不升级 human；修订后由主线程派新评审轮核销。

R1 结论：打回（7 项：3 BLOCKING / 1 SHOULD / 3 NOTE）

### R2 · 2026-10-14 · Track B 第 2 步 architect 评审判官（同判官实例续审 R2）

**核验方法**：现读 v0.3 全文；`git diff HEAD` 逐 hunk 核对修订面（排除 §7 R1 追加）以查夹带；preflight 复跑（`ok: true` / 69 锚 / `advisories: []`）；子检 B / D 现跑判别力探针（fixtures-must-discriminate：B 当前应命中、迁移后应零）；V2 快照推导公式语义复核。

**F1–F7 逐条核销**：

- **F1 ✓ 落结**：§0 快照时点纪律句 + R1 漂移数字（63 / 62 / 399）标注在案；T1 证据标时点；T2 连带声明改「迁移时点全量 + 快照推导公式 + 6 / 4 收敛」；T6 / V2 / V4 / §6 全部同源改写。V2 / V4 可机判复核通过：迁后目录总数 = 快照 taskpackages 非 current 目录数 + 快照 dev lane 独有目录数（起草 64+1 / R1 复测 66+1，公式判官复核语义正确——重叠包按定义已在 taskpackages 侧、仅独有目录为净增量）；快照值记录于 landing-summary 且可由 pre-landing commit 树机械复算，falsifiable 成立。硬编码计数已全部从闸判据中清除。
- **F2 ✓ 落结**：子检 B 去 `-v` 过滤、直断零命中，辩解句重写如实（R1-F2 成因记录在案）。判别力探针通过：现跑命中 19 行（旧分段形在场），迁移后改写完应零——模式非空断言。
- **F3 ✓ 落结**：T4 豁免清单 crq→debt（含改名溯源与 R1 复测 15 件）、V1 子检 A 剔除列 `docs/debt/`、§3 命中集句 debt——三处全改；taskpackages 历史件计数改快照口径并落锚（起草 16 系计数基错声明 + R1 复测 169 / 迁入 179 同归本类）。V1 计数基句同步（「计数基 = 迁移快照 census 现跑值，起草 / R1 计数为时点留痕、不作闸」）。OQ2(a) 选项描述内「crq 日志行」保留为裁定时点原文，T4 改名注记已消歧——判官认可不改（裁定凭据原文不应回写）。
- **F4 ✓ 落结**：closeout-check 移出 `--self-check` 清单、以 `--read-only` 对迁移后目录干跑替代，旗面事实（仅 `--update-index` / `--read-only`）如实注明；其余 6 件含 self-check 入口 R1 已验属实。
- **F5 ✓ 落结**：riskTier 行与 §3 机械强制层行均改「唯二主行为变化 + 连带项见 T2 连带声明」。
- **F6 ✓ 落结**（但派生 F8，见下）：错误论证句已删；「转义形 `changes\/` 单独扫」的 §3 承诺经增设子检 D 兑现——方向正确。
- **F7 ✓ 落结**：T3 改「DEV-GOVERNANCE.md 6 处（R1-F7 复测 `grep -c` 6 行）」。

**夹带核查**：`git diff HEAD` 全 hunk 逐一对号——所有修订均映射到 F1–F7 处置面（时点标签 / 快照语义 / 过滤修复 / 计数勘正 / 措辞收紧），landing-manifest 与 §2 锚集零变动（preflight 69 锚复证），OQ 裁定语义零改写（仅回灌裁定行，v0.2 已在场）。未发现夹带设计变更。

**新发现**：

- **F8（SHOULD）· 子检 D 命令经 agent Bash 执行通道实测退化，冻结后会在主执行通道误红且转义形在该通道逃检**。探针实证（本判官两路独立复现）：子检 D 命令文本 `grep -rEn 'changes\\/dev' …` 经 Claude Code Bash 工具通道执行时，模式被吞一层反斜杠、grep 实收 `changes\/dev`，在 `-E` 下即字面 `/`——只命中 plain 形探针行、miss 转义形探针行；同通道落盘的探针文件本身也少一层反斜杠（通道层吞反斜杠、非 bash 单引号行为——POSIX 单引号字面保真，退化系工具通道所致）。后果：landing-verifier（主执行通道即此通道）post-commit 跑子检 D 会报全量 plain 命中（数百行），「预期零命中」误红正确 landing；同时转义形若真残留反在该通道逃检。无假绿路径（最坏是误红 + 通道困惑），且转义形当前经 ripgrep 通道实测全仓零命中，故定 SHOULD 不定 BLOCKING。处置（R3 机械采纳即可，无需再派评审轮——本判官给出可执行形态）：子检 D 注明执行通道为字节保真通道——① Grep 工具（ripgrep，判官实测该通道反斜杠语义保真：`changes\\/dev` 模式全仓零命中而 plain 形近 500 件在册、退化必然可见），或 ② 先经 Write 落脚本文件（exact bytes）再 `bash` 执行原命令文本；并把本探针结论（Bash 通道吞反斜杠）一句话注记在 V1，防后来者直跑踩坑。
- **F9（NOTE）· 起草数字时点标签未铺满**：§3 表行 1「dev lane 59 包目录、382 文件」与 §6 自律件行「382 件不逐一手搬」仍带起草数字、无「起草快照」前缀——V 闸已不消费这两处（纯描述面），建议主线程标 FROZEN 时顺手加时点标签（机械改写、无需回评审）。

**总评**：R1 三项 BLOCKING 全部落结且修法正确；F8 为 F6 兑现过程中新引入的执行通道缺陷，属验证命令形态问题、不触目标效果与 OQ 裁定，不升级 human。主线程按上述可执行形态机械修订子检 D（并顺手 F9）后，可标 FROZEN（F8 修法由判官本轮给出、按机械采纳处理；若主线程选择非上述两种形态的其它改法，须回 R3 复核）。

R2 结论：打回（2 项：0 BLOCKING / 1 SHOULD / 1 NOTE）

### R3 · 2026-10-14 · Track B 第 2 步 architect 评审判官（同判官实例续审 R3，机械采纳核验）

**核验方法**：git diff 定点核对 F8 / F9 落点 + preflight 复跑（`ok: true` / 69 锚 / `advisories: []` / findings 空）。

**逐条核销**：

- **F8 ✓ 采纳与 R2 给定形态逐句一致**：子检 D 注记块照给定两种字节保真通道原文写入（① Grep 工具 ripgrep 模式 `changes\\/dev`，② Write 落脚本文件 exact bytes 再 bash 执行），命令文本本体保留原样（即形态②的执行对象）、并加「勿直跑」警示与通道后果说明（误红 plain 形 + 转义形逃检）；V1 解释段同步注记句含「R2-F8 探针两路复现」与直跑后果。未偏离给定形态，按 R2 总评约定（机械采纳免再评审轮）放行。
- **F9 ✓ 两处时点标签按给定语义落笔**：§3 表行 1「dev lane 起草快照 59 包目录、382 文件（终值以迁移快照为准）」、§6 自律件行「起草快照 382 件、终值以迁移快照为准」。

**夹带核查**：v0.3→v0.4 增量 diff 仅为 Status 行版本升级与上述两处 + R2 记录本体；manifest / §2 锚集 / OQ 裁定 / T 段结构零变动。Status 行「免 R3」指 R2 约定的免完整评审轮，本记录即该约定的机械采纳核验，两者不冲突。

**GREEN 前提清单核对**（模板结构字段硬条款）：landing-manifest 围栏块在场（69 行）、packet-id 行在场、preflight 干跑全绿。R1 三项 BLOCKING、R2 一项 SHOULD 与一项 NOTE 全部落结。

R3 结论：GREEN

Verdict：GREEN（机锚补行 · 2026-10-14 · 本行为 R3 结论「GREEN」的 VERDICT_STATUS_RE 行首形态归位，不构成新裁决——闸 pre-merge-escalation-gate.mjs 行首判据消费此行）
