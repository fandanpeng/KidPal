# Review Packet — 261031-openclaw-cron-trigger-gating

> integrator 产出（合并门前）。supervisor 机械闸复核输入。riskTier=high（scheduler 行为 + 通知面 + ack 去重 + AI 成本审计；门判错的失败形态是静默停摆），rigor=lite（human 2026-10-31 显式裁定「不做 spike、按 Lite 实现」，覆盖 change-triage §2 单向守卫，裁决见 plan.md frontmatter `humanDecision`）。high ⇒ supervisor 合并门预期升级 human（非缺陷）。

## Change Size

- **Files changed（实现面 14 件，changeManifest 全覆盖）**：
  - 新增判定器：`server/deploy/openclaw-patch/reminder_semantic_gate.py`（+138）、`server/deploy/openclaw-patch/schedule_health_gate.py`（+233）。
  - 新增薄壳 + 工具：`server/deploy/openclaw-patch/triggers/reminder-semantic.trigger.js`（+18）、`triggers/schedule-health.trigger.js`（+18）、`server/deploy/openclaw-patch/cron_triggers.py`（+189）。
  - 部署面：`server/deploy/manifest.yaml`（+33/-1）、`server/deploy/verify-deploy.sh`（+62）、`server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md`（+89）、`server/deploy/openclaw-skills/README.md`（+2/-2）、两份 SKILL.md（+16/+12）。
  - 测试：`server/tests/test_semantic_gate_units.py`（+182）、`server/tests/test_schedule_health_gate_units.py`（+221）、`server/acceptance/test_261031_openclaw_cron_trigger_gating.acceptance.py`（+1007，冻结验收 + REVISION LOG R1）。
- **制品面**：`delivery/taskpackages/261031-openclaw-cron-trigger-gating/`（taskpackage / plan / launch-card / input-brief / live-handoff / dispatch-progress-live-handoff / pod-events / docs-delta / review-packet / test-scope-final.txt）。
- **Net diff**：实现面对 merge-base `93b92a6b` 计 +1533/-6（其中新增验收件 1007 行；净逻辑面约 700 行）。
- **Scope**：两个无状态纯只读 python 判定器（单行 JSON 输出契约）+ 两枚 code-mode JS 薄壳 + 一个幂等 apply/clear/status 工具（写入口只许 openclaw CLI，不直写 openclaw.json）+ 部署面静态锚（manifest / verify-deploy / runbook / README / SKILL）。`firmware/**` 零 diff。
- **Split recommendation**：不拆（单包，mergeOrder null；双 cron 同构、共享薄壳模板与 verify 锚，拆开无法独立验收）。

## Verification Evidence

- **红 → 绿对照（本件）**：红基线 25 failed / 0 passed / 2 deselected（@ 93b92a6b + prep，全为设计态红：判定器/薄壳/锚不在位，无 TypeError/AttributeError 冒充）；绿 = 本件单跑 **25 passed, 2 deselected**（`python -m pytest -q --import-mode=importlib -m "not live" acceptance/test_261031_openclaw_cron_trigger_gating.acceptance.py`）。红基线全量对照：25 failed / 370 passed / 9 deselected / 33 subtests（370 passed 恰等于上游基线零回退）。
- **范围圈定（Step 4）**：`detect_changes`（project `C-AI-KidPal-pod-m2`，base=main→merge-base `93b92a6b`，depth 2）**正常圈定**（per-Pod 图索引可用，未降级）；`impacted_total=2` = 仅 `server/tests` 两个新单测模块（changed 件被其 import 命中），**无 inbound 生产调用方**。清单落盘 = `delivery/taskpackages/261031-openclaw-cron-trigger-gating/test-scope-final.txt`（本行为清单路径留根锚）。合成：种子（本包 acceptanceTestsFile）∪ blast radius 映射测试（2 新单测）∪ 共享 seam（`test_reminder_units.py`——`reminder_schedule_lib.py` 是判定器 A 的声明输入④；10 件 manifest/verify-deploy 静态锚消费方 acceptance 件）∪ 安全余量（同门 openclaw trigger/verify 先例 261025-openclaw-dreaming-disable）。floor 注：本包无双包对消解，floor 不适用。
- **final 相位 scoped 实测（2026-10-31 本树）**：
  - `acceptance`（`--phase=final --scope-file=…/test-scope-final.txt`）：oracle-pass serverAcceptanceFiles=11；executor-run structuralCollected=187 / liveDeselected=5 / filesWithLiveMark=3/11；逐字末行 **`187 passed, 5 deselected in 31.72s`**，EXIT=0。
  - `unit`（同 scope-file）：executor-run structuralCollected=248 / liveDeselected=5 / filesWithLiveMark=3/14；逐字末行 **`248 passed, 5 deselected, 5 subtests passed in 32.41s`**，EXIT=0。注：unit 闸 server 面 test_*.py 收集亦覆盖 acceptance 目录，故 248 = 3 件单测 + 10 件 scoped acceptance 件并集，范围仍限于本清单。
- **change-manifest**：显式路径 `change-manifest-check.mjs delivery/taskpackages/261031-openclaw-cron-trigger-gating/taskpackage.json` → `ok:true`，对账面工作树零改动；已提交 diff 手工核对 `git diff --name-only 93b92a6b..HEAD -- firmware server` = **14 件**，全部落在 changeManifest 9 条 glob 内，firmware 零 diff。（默认读 `current/` 会因指针文件首提日期早于 cutoff 误判 pre-cutoff 跳过——已在 pod-events friction 留根，本闸一律显式路径调。）
- **docs-sync**：`node .claude/scripts/docs-sync-check.mjs` → `ok:true`；docRefs 五件在盘并承载 CRG-A1..A6 锚。逐面判定见 `docs-delta.md`。
- **security**：任务记录 securityReviewRequired=false，闸绿（本包纯只读判定器 + 部署文本锚，无输入解析面扩张、无新增依赖，stdlib only）。
- **mutation check**：`mutationRequired=false`，skipped 且有 reason（无状态机/迁移/并发；本仓未接 Python 变异工具；25 条冻结验收逐条钉住谓词两侧边界，判别力由边界断言承担）。
- **firmware-build**：build-pass（零 firmware diff；lite 包 scoped 后置、final 恒跑）。
- **承重机器件缺口（如实申报）**：本包目录**无** `verifier-evidence/rounds/rN.json`（bearing round fact）与 `verdict.json` / `verification.json` 投影——本轮 verifier 以各闸脚本直跑（遗留 `verify-logs` 可见）而非 `verify.mjs --evidence-mode=round` 的 bearing 路径，故无 round fact。integrator 未伪造 round/invocationId 补产（role 要求 round 由 planner/verifier 线提供）；本轮以 final 相位 scoped 实测独立复核。closeout-check 对 machineEvidenceFiles 按 advisory 处理（非 blocking）。若 supervisor 门要求 bearing fact，交 planner 以 `verify.mjs --evidence-mode=round --phase=final --scope-file … --change-id … --round 1 --invocation-id <uuid>` 补产（round 1 为 next completed ordinal，本包目录当前 0 轮，preflight 无缺口）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （无 `minimal:` 标记命中——台账为空） | — | — | no-trigger |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | 验收判别力（live 面） | 两条 live 验收（`test_live_evidence_covers_l1_l7` / `test_live_evidence_records_exec_shape_and_rollback`）的回退链 `_evidence_text()` 在没有 `live-evidence.md` 时回退读 `live-handoff.md`；而 `live-handoff.md` 正文恰含 `L1`..`L7`、`aggregated`/`text`、`--clear-trigger` 字样。实测 `RUN_LIVE=1 -m live` **2 passed** ——在零真实 ECS 证据下即绿，live 门不区分「交班清单」与「真证据」，`CRG-A5/CRG-E4` 的 live 面失去机械判别力。 | changed-code hunk：`server/acceptance/test_261031_openclaw_cron_trigger_gating.acceptance.py`:`_evidence_text` 的 `for name in ("live-evidence.md", "live-handoff.md")` 回退；对撞 `live-handoff.md` 首行「本文件是交班清单，不是证据」与验收 docstring「只读证据文件」。 | 交回 planner/fixer：live 验收应只认专用 `live-evidence.md`（或加「证据文件不得等于 handoff / 须含真实 L1..L7 逐段输出 + 回滚 exit code」的结构断言）；**integrator 不自行改冻结验收**。不阻断 host 合并，但在真机轮回灌前不得据此谓「live 已过」。 |
| NIT | judgement-call | 制品卫生 | `dispatch-progress-live-handoff.md` 与 `live-handoff.md` 内容逐字重复（同为 8115 bytes，builder scope 绕行产物 + 主线程物化后的双份）。 | changed-code hunk：impl 提交 99325093 同时新增两文件。 | 可选：live 相位前删 `dispatch-progress-live-handoff.md` 保留 canonical `live-handoff.md`；不阻断。 |

> 结构闸 PASS/FAIL 不复述为 finding（本包各结构/功能/治理闸均绿）。

## Five-Axis Review

- **类型/编译轴**：判定器纯 stdlib、显式 3.6-safe 写法（`fromisoformat` 有 `hasattr` 回退 + `strptime` 形态、`reconfigure` 有 `getattr` 守卫）；`_env_int` 对数值型 env 非数字 fail-loud（禁静默回落默认值）；`firmware-build` build-pass、零 firmware diff。无新增依赖（无 requirements 改动）。
- **规范/一致性轴**：判定器 A 四输入（schedule/skill/parser/lib）+ 60s 竞态余量、边界含等于，与 runbook §8.3d 与 SKILL「新鲜度合同」文字一致；判定器 B 四分支 + 指纹逐字 = `error|detail` + 坏 checked_at 退 mtime + 冷却窗，与 SKILL「四分支/去重权威=ack 件」一致。输出契约严格单行 JSON、异常 exit≠0 不吐半截 JSON。
- **契约轴**：`docs/contracts/**` 零 diff（data 形状不变，仅改调度成本模型）；`interface-contract.md` 仅作 contractRefs 引用锚；无新 wire 帧、无新数据类。
- **安全轴**：纯只读、无网络、不写盘；写入口只许 openclaw CLI（`cron edit --trigger-script` / `--clear-trigger`），`cron_triggers.py` 不 open 写模式、不 write_text 任何配置件；`verify-deploy.sh` 的 `trigger.once`/`cron.triggers.enabled` 守卫与内联文本逐字比对形成回滚可观测面。
- **可运维性轴**：`cron_triggers.py status/apply/clear`（含 `--dry-run`）给出一键核验与一步回滚；runbook §8.3d/§8.6/§9 三步（铺放、回滚、升级核对）可照抄执行；`verify-deploy.sh` DRIFT 输出用于定位内联失配。

## 集成执行回报

本轮未触发 Merge-Chain Execution（无跨包冲突 aborted + 无 planner 派单消解）；圈定 floor 注不适用（无双包对消解场景）。

## 白盒复核（explosion 半径）

- 判定器 / 薄壳 / `cron_triggers.py` 均为**叶子只读脚本**：`detect_changes` impact `impacted_total=2`，只命中两个新单测模块；`server/` 应用代码无 import 它们（`grep` 实证：仅 3 件测试引用）。触发链的唯一消费端 = OpenClaw 运行时（第三方黑盒，仓内无源码，`crossEnd.exempt` 已申报），消费端观测落 live L1–L7。
- `verify-deploy.sh` 新增区段嵌在既有 `for job in manifest.get("cron_jobs", [])` 循环体内（同 `every_min` 缩进），循环后追加全局开关守卫；`import os` 已补。未改动既有导出/函数签名。
- manifest 新增 5 条 entries + 2 条 `expect_trigger_file`，均为**追加**，不改既有条目；他包静态锚（10 件 scoped acceptance）本轮绿，证明追加未破邻域。

## Residual Risk

- **Known limitation（live 面未执行）**：L1–L7 **未执行**（ECS 写操作归主线程/human，`live-handoff.md` 已列命令与判据）。四条未跑 spike 的未知项（exec 返回字段实形 aggregated vs text / `once` 默认 / `trigger.state` 持久化 / 会话分类）在 L2/L5 收口；薄壳按 `res?.aggregated ?? res?.text` 容错，若实测皆空须按实测字段修正薄壳并重跑 `cron_triggers.py apply`。**不得在真机轮回灌前宣称「行为已验」**。
- **live 门判别力缺陷**：见 Findings SHOULD——在真机轮前 live 门会因 `live-handoff.md` 假绿，须先修验收判别力再采信 live 结果。
- **decisionCoverage**：taskpackage 无 `decisionCoverage` 字段 ⇒ 无 `landsIn: dropped` / `risk-accepted` 条目可列。
- **回滚路径（两条独立，任一可单独生效）**：① 代码 revert——本包实现 commit 及后续 integrator 制品 commit 整体 revert（无数据迁移、无 schema、server 应用面零 diff 影响）；② ECS 侧 `cron_triggers.py clear`（等价 `openclaw cron edit <name> --clear-trigger`）一步回到无条件 15m 形态，payload / 频率零改动。仓内文件未被消费即无害，manifest 条目留待后续批清理。
- **Follow-up recommended**：① Findings SHOULD（live 验收判别力收紧，交 planner/fixer）；② `change-manifest` 闸取包目录自身 taskpackage.json 首提日期而非 `current/` 投影（pod-events friction 已记 ARQ 建议）。
- **Human decision needed**：supervisor APPROVE 后合并门按 riskTier=high 升级 human（预期路径）；live L1–L7 真机轮由主线程/human 执行并回灌 `live-evidence.md`。

## UI design-review 证据

不适用（本包无 UIUX 工作；改动为调度层判定脚本与部署文本锚，无视图外观变化）。

## 制品与投影记账

- blocking 三件：`taskpackage.json`（在盘）+ `docs-delta.md`（本包目录）+ `review-packet.md`（本件）；范围圈定清单 `test-scope-final.txt`（本包目录）。
- 承重机器件：**本包无 verifier-round fact**（见 Verification Evidence 缺口申报）；`verdict.json` / `verification.json` 未在包目录（advisory-only，缺失不阻断）。
- post-cutoff 确认：不产 session-digest / loop-health / loop-metrics / pod-journal；`pod-events.jsonl` 在盘（advisory 检查通过）。
- 9b 记账（advisory）：`detect_changes` 已在 per-Pod 图索引 `C-AI-KidPal-pod-m2` 上实跑（本树未 rebase，索引与工作树一致），改动只影响预期符号（两个新单测模块），无预期外实现面影响。
