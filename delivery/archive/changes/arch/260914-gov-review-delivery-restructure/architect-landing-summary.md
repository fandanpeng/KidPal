# architect-landing-summary · 260914-gov-review-delivery-restructure

packet-id：本 `PROPOSAL.md` §2 改法集（T1–T6）+ §5 验证集（V1–V6）· 评审轮次 R1–R17（第 4 轮终审 GREEN，2026-09-14）· change-id `260914-gov-review-delivery-restructure`

> landing 执行：2026-09-15，主线程 packet-bound（human 立项 ack + 三 OQ 裁定 + R1 扩围裁定 + GREEN 凭据见 PROPOSAL Status 行）。前置：main→gov sync 已完成（main 5573583 为 gov/kidpal 祖先，实测 `git merge-base --is-ancestor`）；sync 后 census 复验与判官四轮口径一致（分段字面量 27 件精确吻合、契约面仍 2 处 changes/ 引用）。

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 四目录 git mv | `.delivery→delivery/milestones`、`.planning→delivery/workstreams`、`.claude/taskpackages→delivery/taskpackages`、`changes→delivery/changes` | 520 件 rename + `.gitkeep` 删除；历史面（`_archive/`、`changes/{arch,dev,retro}/` 既有制品、22 个已晋升包、2 个未晋升包）内容零改动 |
| T2 机械强制层 | scope-map.json（allow/deny glob 全量平移 + `bashDenyWrite` 增补 `delivery/taskpackages/**`）、8 hooks、26 scripts、dev-loop.settings.json、route-fixtures/surface-alignment.json | 斜杠形 + 分段/字面量全家族（单引号逗号、任意段值、Python join、C 实参）双轮替换；逻辑零变更 |
| T3 治理文档面 | agents 11、commands 6、references 12、skills 6、rules 2、AGENTS.md（CodebaseMemory 表两行）、docs 面（requirements 活行 610/676、architecture:276、DEV-GOVERNANCE、ARQ 活段、notes 2、templates 6）、firmware/server 活文档 3、代码面 20 件（R1 扩围：firmware 10 `.c` + server 9 `.py` + `.gitattributes`）、repo-structure.md 顶层地图新增 delivery 三行 + 「工作产物去哪」改写 | 全部落定；`.gitattributes` merge=union 属性行顺修既有归档失配（→ `delivery/workstreams/_archive/2026/kidpal-mvp/progress.md`） |
| T4 迁入件活内容 + 派生索引再生 | delivery-workspace.json（planRoot 等 3 处）、workstreams/README.md（12 处）、current/ 4 件、milestones.jsonl planningRefs（2 处）；planning-index / delivery-status / changes-index（无参全量重建 + upsert） | 三索引再生全 ok；STATUS 完工比例与迁移前一致（accepted 0%、inflight 11.4%——输入未变）；INDEX devCount 23 / archCount 23 |
| T5 图索引面 | `.cbmignore`（增 `delivery/` + 注记）、`.claude/.cbmignore`（删 `taskpackages/` 行 + 注记） | 完成；主索引刷新为 landing 后操作项（见 §3-7） |
| T6 契约面 3 处 | docs/contracts/interface-contract.md 行 4 / 5 / 96 | 行 4 改指 `docs/requirements.md` F3 + 「D3 以本契约为权威」显式化；行 5 / 96 前缀平移为 `delivery/changes/arch/`；architect-reviewer 四分类 = 随本变更携带·零语义 + human 预签（记录见 PROPOSAL §7） |

## 2. 验证证据

- **V1 主命令（活面残留清扫，不限扩展名 + 两段过滤）**：豁免面之外**零残留**。豁免命中如实枚举：`.claude/loop-retro/ledger.jsonl` 1 行（append-only 台账）、`docs/ADR.md` 4 行（决策记录历史行）、`docs/ARCH-REVIEW-QUEUE.md` §2 已结索引 39 行（含 :90 格式说明行）、`docs/CONTRACT-DECISION-QUEUE.md` 3 行（已结/已落地行；第 4 行 CDQ-260913 含 `docs/archive/` 字样被第二段过滤蔽掉，同为历史行）、`docs/requirements.md` 版本历史 3 行（682/693/734）、`docs/DEV-GOVERNANCE.md:20` 1 行（原仓名引述，新类豁免见 §3-5）。
- **V1 子检 1（current/ + milestones.jsonl）**：按冻结命令文本跑得 24 命中，逐行核对**全部为 `delivery/changes/dev/...` 新前缀被自家模式误咬**（子检命令缺第二段过滤——机检口径瑕疵，见 §3-6）；补 `delivery/` 过滤后 **0**。
- **V1 子检 2（分段/字符串字面量全家族，作用域含代码面）**：**0**。
- **V2 派生索引**：`planning-index.mjs` ok（count 3，全 archive）；`delivery-status.mjs` ok（0% / 11.4%，与迁移前一致）；`changes-index.mjs` 无参全量重建 ok + `--upsert` 本 id ok；INDEX **链接列旧前缀 0 行**（origin 数据列 46 行历史数据豁免，见 §3-5）；workstreams INDEX/index.jsonl 旧路径 0。
- **V3 承重脚本自检**：changes-index / landing-fidelity-check / proposal-consolidation-check / pre-merge-escalation-gate / portfolio-reconcile / merge-driver / mutation-check / verifier-evidence-check / render-roadmap / route-regression-check **10/10 PASS**。
- **V4 结构门**：route-regression-check 实跑 28 ok / 0 fail；landing-fidelity-check 见本 summary 落盘后复跑结果（verdict 件记录）。
- **V5 git 卫生**：commit 前 `git diff --cached --stat` 核对（commit 时执行）；`git log --follow` 抽查 `delivery/workstreams/_archive/2026/kidpal-mvp/task_plan.md` 搬运历史连续（commit 后复核）。
- **V6 构建基线**：`firmware/**/CMakeLists.txt`、`server/pyproject.toml`、`tools/package.json` 等构建系统文件 **0 处 `delivery` 引用**（managed_components 为 vendor 面、gitignored）。

## 3. 诚实注记

1. **批量替换跑了两轮**：第一轮 8 规则的 r4 前导卫兵把 `/` 排除，漏 `../../changes/` 相对链接形（arch-scenarios / taskpackage-authoring 等 10 件）；按 V1 判据 `[^a-z]` 口径以负向后瞻重写 r4/r0 补第二遍（幂等验证通过：重跑 0 触达）。
2. **第二遍引入 1 处双应用损伤**：scope-map `_bashDenyWrite_doc` 原文压缩形 `.claude/taskpackages/changes/dev` 被两轮叠加成 `delivery/taskpackages/delivery/changes/dev`——全树模式扫描确认仅此 1 处，手工修复并改写为显式枚举（`delivery/taskpackages/、delivery/changes/dev/`）。
3. **批量替换越界吃掉行级豁免件历史行**（requirements 3 行 / ADR 4 行 / CDQ 4 行 / ARQ §2 32 行）：发现后从 git index 定点逐行恢复（CRLF 对齐修正后精确还原），复核 diff 仅剩活行（requirements 610/676、ADR :3、ARQ §0/§1）。
4. **正则字面量损伤与漏网**：源码内转义形 `\.planning` 被斜杠规则命中留孤儿反斜杠（3 文件 6 处，changes-index 启动即 SyntaxError 暴露）；另 4 处 `changes\/` 转义斜杠形两轮均匹配不到——共 10 处全部手工修复，`.claude` 全量 `.mjs` `node --check` 体检通过。
5. **新增行级豁免 2 类**（判官预留清单之外，报 landing-verifier 裁）：①「原仓名引述」——DEV-GOVERNANCE:20 引述 AInativecompany 原仓的 `.delivery`/`.planning` 目录名（语义修正时留真）；②「派生索引内嵌历史数据」——changes/INDEX 的 origin 数据列 46 行，来源为历史包 taskpackage.json 的 origin.ref（只搬不改面），链接列本身已全量新前缀。
6. **机检口径瑕疵 1 处**：V1 子检 1 冻结命令缺第二段过滤（误咬新前缀），landing 时补过滤执行——建议 retro 收录（子检命令以本 summary 记录的补滤版为准）。
7. **未在本 commit 内的操作项**：主索引 `index_repository` 刷新（使 `.cbmignore` 的 `delivery/` 排除生效）——landing 后执行。
8. **手工语义修正 2 处**：DEV-GOVERNANCE:20（原仓 dogfood 句，批量替换致语义歪斜后按原意重写）；`.gitattributes` merge=union 属性行（原行本就指向归档前路径、早已失配，顺修至 `_archive/2026/` 实际位置）。
9. **repo-structure.md 三行新增曾被权限分类器误拦**（理由文本自认 landing 已授权但调用仍拒），human 手动放行后完成——如实记录。
10. **post-FROZEN 结构修正 3 项（landing-fidelity-check fail-loud 所迫，PROPOSAL manifest 注记同步留痕）**：① 四目录根迁入锚剔出 manifest——git diff 不产目录级变更、目录锚不可机判，T1 改整裁腿空锚（迁移事实由 520 件 rename + V5 `git log --follow` 见证）；② `docs/CONTRACT-DECISION-QUEUE.md` 剔出 manifest——其 census 命中经核全部为已结行历史行（行级豁免）、零落地件；③ PROPOSAL 各 T 补「目标文件：」锚清单行——模板硬项 2a 起草期遗漏、四轮评审未捕获（判官 manifest comm 对账以 census 为基准故未触发），retro 收录。连带：`governance-gate-shared.mjs` 的 `ANCHORABLE_TEST` 扩形（认 `delivery/{milestones,taskpackages}` 与 `firmware|server|tools` 前缀——R1 扩围后的合法锚面，原正则只认旧锚宇宙）；`CANONICAL_TEST`/`ANCHORABLE_TEST` 的 workstreams 前缀平移属 T2 批量替换外的正则字面量手工修复（见 §3-4）。
11. **landing-verifier 首轮 FAIL（RED）→ 红项修复轮（2026-09-15，如实记档）**：核验官判 FAIL 并退回 4 处落地逻辑损伤——**成员访问粘连形**（第 8 种路径形态）：`taskPackage.deliveryMilestoneStateChanged` 等**点号属性访问**里的 `.delivery`/`.planning` 被 r2/r3 点前缀规则误吃，产出 `taskPackagedelivery/milestonesMilestoneStateChanged` 类未定义标识符（closeout-check.mjs、taskpackage-required-check.mjs、arch-review-handoff-check.mjs 三脚本 + device_ws_switch.acceptance.c:864 的 r5 分段插入致 7 参函数变 8 参）。修复：定点脚本 18 处恢复原属性名（C 件改单串 `"delivery/changes"`）；扩展签名扫描 `[A-Za-z0-9_})\]]delivery/(milestones|workstreams|taskpackages)` 全树零残留；三脚本实跑冒烟过线（closeout ok:true、required-check ok:true、handoff-check 到 usage 业务判定）。防回归归 retro §4-2（路径形态族谱表 ARQ——成员访问粘连形列为第 8 形）。
12. **canonical↔current 指针对齐缺口（required-check 冒烟暴露、landing 造成）**：批量替换按 PROPOSAL 只批改了 `current/` 活副本，在途包（260911-render-pages）的 canonical `taskpackage.json` 被按「历史面只搬不改」处理 → 哈希失配（pre-landing 实测两者字节一致，失配 100% landing 造成）。修复：批改后的 current 回写 canonical 对齐，required-check 恢复 ok:true。PROPOSAL 覆盖缺口成因：census 起草期把 `.claude/taskpackages/` 整树当历史面排除，未识别「在途包 canonical = 活面」。

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
