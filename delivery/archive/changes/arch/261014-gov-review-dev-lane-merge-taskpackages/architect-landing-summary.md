# architect-landing-summary · 261014-gov-review-dev-lane-merge-taskpackages

packet-id：FROZEN v1.0 PROPOSAL §2 改法集（T1–T6）+ §5 验证集（V1–V7）· 评审轮次 R1（打回 7 项）→ R2（打回 2 项）→ R3 GREEN（同判官实例 abb307efd8e8bce8f，2026-10-14）· change-id 261014-gov-review-dev-lane-merge-taskpackages

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 纯搬移整裁腿 | delivery/changes/dev/ → delivery/taskpackages/ | 399 件 dev 独有文件逐件 `git mv` 入同名包目录（含孤儿包 260913-happy-flow-host-capture 整迁）；9 件同名副本丢弃（8 件 byte 一致 + pod-events.jsonl 一件，超集复验 dev 11 eventId ⊆ tp 16、差集 0）；dev lane 目录壳与旧 INDEX 退役删除；逐件对账清单 `move-inventory-399.txt` 留本目录 |
| T2 机械层路径批改（16 件） | `.claude/scripts/*` ×11、`.claude/hooks/*` ×4、`.claude/dev-loop.settings.json`、`.claude/scope-map.json` | 路径常量平移 + 判定逻辑零变更；唯二主行为变化落地：changes-index dev 扫描根与 laneIndexPath('dev') 改指 delivery/taskpackages、collectDev/changeIdArchiveCandidates 排除 `current/`；portfolio-sync 触发条件删 changes/dev 分支；scope-map planner allow 的 pod-retro 专项条目与 integrator allow dev-lane 项剔除；closeout-check 双 fallback 塌缩件清理 |
| T3 治理文档面批改（17 件） | repo-structure.md 四处改写 + agents 2 + commands 2 + references 7 + skills 2 + AGENTS.md + DEV-GOVERNANCE.md + GOVERNANCE-MIGRATION-GUIDE.md | repo-structure taskpackages 行翻转（包全程唯一家）、原 changes/dev 行删除、同名物消歧表 dev INDEX 落址更新、晋升句改写；其余指针与提示句平移；职责边界零变更 |
| T4 docs 与 workstreams 活文本批改（11 件） | requirements.md、validation-batch-conclusion.md、workstreams README + 8 活件 | 指针刷新；历史面按 OQ2(a) 全豁免（T4 豁免清单全集零改动） |
| T5 实现面批改（24 件） | server acceptance 2 `.py`（DEVICE_ROUND_RECORD / LIVE_EVIDENCE_DIRS 功能常量改指 taskpackages）+ deploy 2 + evidence 4 + firmware `.c` 8 + HOST_TEST_CONVENTIONS + red-baseline 5 + tools 2 | 功能常量离线可导入（V5 绿）；注释 / 消息串 / 证据摘要指针平移；2 处「enforce-scope 拦 changes/dev 写入」陈旧机制从句改「证据面 = 包内收尾制品」（见 §3 注记 3） |
| T6 派生索引再生 | delivery/taskpackages/INDEX.md（新增） | `changes-index.mjs` 无参全量重建 + `--upsert 261014-gov-review-dev-lane-merge-taskpackages`；dev 段 67 行 = V2 迁移快照推导值（快照 taskpackages 非 current 66 + 孤儿 1）；旧 `delivery/changes/dev/INDEX.md` 未复活 |

## 2. 验证证据

- **V1 残留清扫（post-commit 判定，landing commit `d5194c9` 树上执行）**：
  - 子检 A（斜杠形）：残量 **810 行、全部位于 `delivery/taskpackages/**` 包内历史行**（豁免类 9），非豁免面残量 = **0**；子检 A 剔除列外逐目录归类见下「逐类计数」。
  - 逐类计数（-c 口径，行数）：taskpackages 包内历史行 810；arch 制品目录 148（`grep -c` 于 `delivery/changes/arch/**`，含本 PROPOSAL 自身）；intake spine / debt / showme / ADR / ledger / acceptance-archive / workstreams-archive / notes 各类命中均为剔除列内文件、未计入残量面。子检 B（.claude 分段字面量形）= **0 命中**；子检 C（server/acceptance Python join 形）= **0 命中**；子检 D（转义形，经 **Grep 工具字节保真通道**执行——R2-F8 纪律）唯一命中 = 本 PROPOSAL 自身（模式注记自指，属「本批 arch 制品目录」预期豁免）。
  - `docs/notes/validation-batch-conclusion.md` 已批改、剔除列冗余无害（与 V1 预期一致）。
- **V2 搬移对账（pre-commit）**：`test ! -d delivery/changes/dev` → lane 目录不复存在 ✓；`ls -d delivery/taskpackages/*/ | grep -v current | wc -l` = **67** = 迁移快照（taskpackages 非 current 66）+ dev 独有目录（孤儿 1）✓；逐件对账 399/399「源失踪 + 目标在场」零失败 ✓；迁移快照（2026-10-14 landing 窗口现跑）：taskpackages 非 current **66** / dev lane **63** / 重叠 **62** / 孤儿 `260913-happy-flow-host-capture` / dev 独有文件 **399**（与 R1 复测一致）。
- **V3 承重脚本自检（pre-commit）**：`changes-index` / `supervisor-gate-check` / `supervisor-verdict-gate` / `merge-driver` / `pre-merge-escalation-gate` / `landing-fidelity-check` 六件 `--self-check` 全 OK；`closeout-check` 无 self-check 旗（R1-F4），以 `--read-only` 干跑替代 exit 0 ✓。
- **V4 INDEX 再生（pre-commit）**：`node .claude/scripts/changes-index.mjs` 无参重建 exit 0（`devCount: 67, archCount: 57`）；`delivery/taskpackages/INDEX.md` 在册、dev 段 67 行、含本变更行（`--upsert` 回显 action=replaced、status=frozen）；`test ! -e delivery/changes/dev/INDEX.md` ✓。
- **V5 功能常量离线验证（pre-commit）**：`python -m pytest server/acceptance/test_m3k3_voice_schedule_entry.acceptance.py --collect-only -q --import-mode=importlib` → exit 0、14 collected；`test_perf_k1_roundtrip_latency` 同 → exit 0、27 collected；`test -f delivery/taskpackages/260921-m3-k3-voice-schedule-entry-live/device-round-record.md` → 在场 ✓。**注记**：conftest.py:11 载明该目录测试收集须 `--import-mode=importlib`（G 包先例）；FROZEN V5 命令文本缺该旗时 exit 2（ModuleNotFoundError，收集机制层面、与常量改动无关——改动前同命令同败），验证实质（导入期常量构造成功）以带旗命令承载。
- **V6 git 卫生（post-commit）**：`git diff --cached --stat` 先核后 commit（483 files：400 R / 72 M / 10 D / 1 A，与 manifest 锚集 + 搬移面一致）；`git log --follow` 抽查 2 件（镜像包收尾件 `260829-char-stroke/supervisor-verdict.json` + 孤儿包件 `260913-happy-flow-host-capture/review-packet.md`）均连续回溯至搬迁前 commit（`bd81f57` / `ceb25b6`）✓。
- **V7 自检干跑（FROZEN 前置）**：`landing-fidelity-check.mjs --preflight` 报 `ok: true`、`anchorCount: 69`、`anchorUnion` 与 manifest 逐 token 一致、`advisories: []`、`findings: []`；R1/R2/R3 三轮附录后均复跑绿。

## 3. 诚实注记

1. **V5 命令旗缺**（ADVISORY）：FROZEN V5 命令文本未带 `--import-mode=importlib`，字面执行 exit 2（收集机制层面 ModuleNotFoundError，与本次常量改动无关、改动前同命令同败）；验证实质以 conftest 载明的带旗命令承载（见 §2 V5）。后续同类验证沿用带旗命令。
2. **子检 A 残量 810 行的口径**（ADVISORY）：taskpackages 包内历史行（豁免类 9）以**行数**计 810；T4 快照数字（169 + 179）为**文件数**口径（census `-l` 文件清单），两口径不同、不矛盾；非豁免面残量 = 0 是 V1 的实质判据。
3. **firmware 2 处陈旧机制从句改写**（ADVISORY）：`reminder_overlay_unify.acceptance.c` / `voice_round_ui.acceptance.c` 各 1 处「enforce-scope 拦 changes/dev 写入」——该机制随 lane 退役消亡，原从句无指针可平移；按语义最小改写为「证据面 = 包内收尾制品」，指令内容（本文件不承载真机证据、planner 已转写包内）不变。属 FROZEN T5「指针平移」边界的语义最小裁量，登记备核。
4. **path-form-family.md:17 保留**：taxonomy 表的 Python join 形**示例** `"changes" / "dev"` 是形态教学泛例、非仓内路径指针，未批改（T3 只平移路径引用与提示句）。
5. **OQ2(a) 裁定行内「crq 日志行」保留**：裁定凭据原文不应回写（R2 判官认可），T4 豁免清单已带改名溯源注记消歧。
6. **`.changes-index-state.json` 随批变化**（PROPOSAL §3 判「未触」成立）：文件无路径常量；本批变化 = 全量重建 + upsert 的状态缓存自然写盘（含本变更行 mergedInMain=yes——机械推导，本变更目录已随 PR #5 进 main，属实记账）。
7. **迁移窗口前置条件**：landing 时点两 Pod worktree 均无在途工作（pod-m2 = 3fedba4 已合并零脏；pod-m1-k1a 为 260915 旧分支遗留，仅 2 行机生面脏：`.changes-index-state.json` 缓存 + 未跟踪生成件 `lang_config.h`），T1 前置条件成立。

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
delivery/taskpackages/INDEX.md
delivery/workstreams/README.md
delivery/workstreams/m1-home-qa/decision-coverage.json
delivery/workstreams/m1-home-qa/pending-human-ruling-260917-m1-k4-rma8-fixbatch.md
delivery/workstreams/m1-home-qa/review.md
delivery/workstreams/m1-home-qa/roadmap.md
delivery/workstreams/m2-char-window/progress.md
delivery/workstreams/m3-schedule/progress.md
delivery/workstreams/m4-star-notebook/decision-coverage.json
delivery/workstreams/m4-star-notebook/progress.md
docs/DEV-GOVERNANCE.md
docs/notes/validation-batch-conclusion.md
docs/requirements.md
docs/templates/GOVERNANCE-MIGRATION-GUIDE.md
firmware/main/test/HOST_TEST_CONVENTIONS.md
firmware/main/test/acceptance/red-baseline/red-baseline-summary.md
firmware/main/test/acceptance/red-baseline/schedule-p2-red-baseline-summary.md
firmware/main/test/acceptance/red-baseline/schedule-reminder-red-baseline-summary.md
firmware/main/test/acceptance/red-baseline/screen-unification-red-baseline-summary.md
firmware/main/test/acceptance/red-baseline/story-p3-red-baseline-summary.md
firmware/main/test/char_window_overlay.acceptance.c
firmware/main/test/key_layer_dispatch.acceptance.c
firmware/main/test/keybar_three_slot.acceptance.c
firmware/main/test/p1_cjk_name.acceptance.c
firmware/main/test/p1_home_battery.acceptance.c
firmware/main/test/reminder_overlay_unify.acceptance.c
firmware/main/test/retract_p4_three_page.acceptance.c
firmware/main/test/voice_round_ui.acceptance.c
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
```

**manifest 口径注记**：上围栏块 = FROZEN PROPOSAL 围栏块逐 token 拷贝（69 锚）；对账以 landing-fidelity-check.mjs 消费 PROPOSAL 为源。
