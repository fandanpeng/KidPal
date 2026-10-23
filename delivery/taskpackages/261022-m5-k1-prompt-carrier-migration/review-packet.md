# Review Packet · 261022-m5-k1-prompt-carrier-migration

- date: 2026-10-23 · author: integrator · branch: `dev/pod-m2`（worktree `C:/AI/KidPal-pod-m2`，未 push、未合并）
- workstream: device-memory-overhaul P1（rigor=full，riskTier=medium，mergeOrder=50）

## Change Size

- Files changed: 33（19 tracked modified + 14 新增/新增目录展开；实现面 16 件全在 changeManifest 内，簿记面 9 件 = taskpackage 回填 / current/ 投影 / verifier evidence / test-scope / workstream progress / n2a plan 注记）
- Net diff: server 面 16 件 810 insertions / 473 deletions（`git diff --stat server/`）；firmware 零 diff（crossEnd exempt 成立）
- Scope: server/deploy（prompt 承载反转声明件 + overlay 渲染面 + manifest + verify-deploy + tools 声明式 patch 工作台）+ server/acceptance（新增两件 + 六件 supersede 留痕）+ perf-k1 probe resolver
- Split recommendation: 不拆——E1–E6 六条 exitCriteria 共享同一承载反转不变式（ADR-001 三面集合相等），拆开任一条都会制造过渡态双源

## Verification Evidence

- Structural gates: firmware-build PASS（真编译，xiaozhi.bin 0x3242f0 / 9% 空闲）；contract-lint PASS（1 条先存 warning：260923 归档件相对链接缺失，非本包引入）
- Acceptance/unit/api/ui/e2e:
  - **final bearing round 2**（invocation `28f4ca43-2380-403a-860d-e31b037d16d9`，evidence `verifier-evidence/rounds/r2.json`，fact `ok:true`，observedGateExit 0，10 闸全绿）：acceptance 闸按 scope-file 圈定跑 = **122 passed / 4 deselected / 4 subtests**（serverAcceptanceFiles=9，与清单 9 件恰合）；unit 闸同圈定集 122 passed。
  - **scoped round 1**（invocation `aa4e8924-f0eb-4975-8126-4e988fa0932f`，evidence `verifier-evidence/rounds/r1.json`，`ok:true`）：结构/治理 8 闸绿。
  - builder 全量（冻结验收件全档，收拢前实测）：acceptance **268 passed / 4 deselected / 33 subtests**；unit 115 passed。final 圈定跑 122 与全量 268 之差 = 未入影响面的 12 件 acceptance（圈定语义，非缩水——清单由 detect_changes blast radius + impactedTests 8 件种子合成，见下）。
  - **test-scope 清单落盘路径**：`delivery/taskpackages/261022-m5-k1-prompt-carrier-migration/test-scope-final.txt`（本行即 Step 4 要求的 results 元数据必含行）。
  - 范围圈定来源：detect_changes（项目 `C-AI-KidPal-pod-m2`，base=main，merge_base `180732a9`，seed_symbols=168）impacted_total=2 全落 `test_prompt_v7_round_routing.acceptance.py`（已在种子内）；共享 seam 补查 server/tests/ 六件 unit 与本包零 seam（server 源码零 diff）。
- Security gate: PASS（`securityReviewRequired=false`）；K1-E4 机判齿 `test_k1_e4_no_openclaw_json_in_repo_and_no_secret_in_declaration` 绿——`openclaw.json` 整文件（含 gateway 凭据）零入仓，declaration 文件人工复核无 secret。
- Mutation check: PASS（skipped，理由 = taskpackage `mutationSkipReason`：声明件/渲染脚本/清单多为字面锚与接线一致性，apply/rollback 按 261017 先例以幂等/漂移/回滚三态验收覆盖）。
- Docs sync: PASS（docsSyncRequired=false，docs/** 零 diff）；两条声明性注记登记于 `docs-delta.md`（§5.5.5 窄漂移 + v11 声明面保留），不改任何 docs 文件。
- verifier-evidence-check: PASS（exit 0，factOk=true）。

### 合并收口前影响面核对（Step 9b · advisory 记账）

detect_changes 已跑（见上）：33 changed files 全部为簿记面（delivery/**）或 changeManifest 登记面（server/** 16 件），blast radius 映射仅命中 `test_prompt_v7_round_routing` 内既有符号的测试方法，无未预期生产符号波及。本仓图索引可用（per-Pod 专属条目 `C-AI-KidPal-pod-m2`），无降级。改动只影响预期符号，核对通过。

### 集成执行回报

本包未触发 merge-driver 冲突消解路径（单包在途、无对端包），Pod 分支 rebase/merge 未执行——该节不适用，无实测数字可报。

## Demo 对账行（workstream 包硬性两条之一）

demo 证据 = **host runner 输出**（与 plan.md「demo 证据形态」节一致）：pytest 验收套件（final 圈定 122 passed / 全量 268 passed）+ 静态接线/锚集断言（三面接线 / manifest marker 锚集「本回合时间」「课表摘要」替换「实时事实回合」/ carrier 声明）+ tools 工作台四态真跑（apply 幂等 SKIP / drift_guard exit3 / UTC 时间戳备份 / rollback dry-run+restore，与 261017 skills 组合共存真跑）。与 roadmap 效果段（host 面三面接线 + tools 收窄四态）**一致**：E1–E6 六条 host 面 exitCriteria 全部有对应绿断言。**live 观测项（K1-E7 token 体积 / cache 命中 / 工具 4 项 57KB schema 消失、K1-E8 SOUL/USER 清理）如实标「待 human 部署轮」**——本包 host 面转绿后停 `awaiting-human:live-deploy`，为 launch-card 钉死的停等位，不是缺口。

## 未覆盖面申报（对照 acceptance[] 六条 + 契约条目 + roadmap 效果段）

| 设计性质 | 自动断言现状 | 现状 | 去向 |
|---|---|---|---|
| K1-E7 单轮 DeepSeek prompt_tokens ≤11,000（基线 22,116）+ cache hit 覆盖稳定前缀 | 无（docstring 申报 live 观测项） | host 面承载拓扑前提已达成（静态语义进 stable project context） | human 部署轮：deploy.sh 后复测 |
| K1-E7 工具面 audit 可见工具 = 4 项、57KB schema 消失 | 无（同上） | 仓内 declaration 四项 + apply 脚本四态验收绿 | human 部署轮：openclaw 工具列表核验 |
| K1-E8 workspace-kidpal/SOUL.md 与 USER.md live 清理 | 无（host 面零断言，launch card 人工步骤） | 仓内无镜像（ECS 生产 live 在场），本包 changeManifest 不含 | human 部署轮：随 deploy.sh 铺放批次处理；不存在则记 moot |
| 真机轮回归 | 无（plan.md：真机轮默认不在本包，核心包配额留 P5 收口轮） | 本包 server/deploy 纯云端面、设备端零感知（crossEnd exempt） | 默认不加轮；human 要求时仅抽问回归 |
| openclaw.json 实机 apply 效果（live 运行态） | 仓内仅四态机判（临时副本真跑） | apply/drift/rollback 对真 openclaw.json 的效果未实测（文件不入仓） | 部署轮 observability：apply 后 audit 工具列表 + drift_guard 复跑 |
| 契约 §1.6 承载对账注记 | 零改（arch 批已落稿） | 契约字面零 diff，实现与注记口径一致 | 无去向（闭合） |

## Simplification Debt

`minimal:` 标记 sweep：仓内现存 2 处（`server/shim.py:295`、`server/gateway/front/front_app.py:1698`）均为先存、非本包改动面文件，本包零触碰。本包新增代码无 `minimal:` 标记。

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （本包无新增） | — | — | no-trigger |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | docs/architecture.md §5.5.5 | 实现定形「原地改写」（内容源 = kidpal-agent-prompt.txt + 装配壳 = kidpal-dynamic-context.v1.txt）与 §5.5.5「装配壳经 overlay 渲染进 prompt 键」的动态模板描述存在命名归属级措辞漂移 | changed-code hunk：`server/deploy/provider.config.yaml` dynamic_context 三键 + `server/deploy/apply-xiaozhi-overlay.sh` DYNAMIC_CONTEXT_SRC 注入段；heuristic/tag = spec-face-drift（声明性、非行为） | 随 P5（同触动态块面）小批措辞修正 spec 措辞；本包已登记 docs-delta.md 注记 1，P5 plan.md 已落注入落点注记——无需本包动作 |
| NIT | judgement-call | server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER | 承载反转节（新增）与下文「声明面/渲染面/消费面」历史段（v11 现行表述）之间存在少量时态混排（历史段仍称「当前冻结版本化文件，v11」） | changed-code hunk：carrier 文件新增反转节 vs 既有 v11 沿革段；heuristic/tag = doc-internal-consistency（读者需上下文两节对读才得全貌） | 随 P5 spec 措辞批顺手统一 carrier 历史段时态；不阻塞 |

结构闸 PASS/FAIL 不复述为 finding；gate 全绿，五轴语义 review 独立执行如下。

## Five-Axis Review

1. **安全**：`openclaw.json`（含 gateway 凭据）整文件不入仓——机判齿 + 人工复核 declaration 无 secret；apply 脚本写前 UTC 时间戳备份、drift_guard 现状锚（未设覆写时才 apply）、rollback 一步还原，无 destructive 默认路径。spec 面零自改，契约零 diff。无新增依赖、无凭据硬编码。绿。
2. **契约**：display E1 线格式零触碰（本包纯 server/deploy 声明面 + 引擎 prompt 装配面）；契约 §1.6 承载对账注记与实现一致（静态承载 = AGENTS.md、动态注入面 = 有界 context 块）。绿。
3. **边界**：动态块有界（`kidpal-agent-prompt.txt` 仅回合内事实占位 6 行，零静态纪律——`test_k1_e2_dynamic_context_template_present_and_bounded` 绿）；静态纪律唯一件 = AGENTS.md（自声明单一事实源齿绿）；二选一定形「原地改写」由冻结判法组合唯一自洽解背书（planner 追认留痕 progress.md）。无跨层调用、无循环依赖、无绕过 contracts 的本地类型。绿。
4. **可运维性**：manifest.yaml 新增 4 条目（装配壳 + tools 三件）全部 verify: md5 / restart 语义明示；marker 锚集同批撤换（「实时事实回合」→「本回合时间」「课表摘要」）防部署核验必红（plan.md 可行性判据末条闭合）；AGENTS.md md5 漂移已知窗口纪律在场未弱化（K1-E6 齿绿）；perf-k1 probe resolver 同批改指 AGENTS.md，探针与生产承载不脱钩。绿。
5. **语义**：v1–v11 历史件原地冻结零回改（K1-E1 sha256 齿绿）；六件既有冻结验收 supersede 留痕、零静默改写（K1-E5 齿绿，扩面三项 planner 授权 + 追认留痕 progress.md）；m4k3 换代新增「≤12 字标题」锚随 v11 语义一并迁入 AGENTS.md。绿。

## Residual Risk

- Known limitation: live 面（token 体积 / cache 命中 / 工具列表 / SOUL-USER 清理）未观测——承载拓扑的结构性前提已由 host 面断言，但收益数字须部署轮实证；若 prompt_tokens 未达 ≤11,000 目标，需回头查 OpenClaw `extraSystemPrompt` 装配路径是否仍挂载静态内容（engine-patch/connection.py 归 P2）。
- Follow-up recommended: ① §5.5.5 措辞修正随 P5 批；② v11 声明面（provider 三键 / overlay PROMPT_SRC / carrier 沿革段）为刻意保留，P2–P5 勿当废弃清理（docs-delta.md 注记 2）；③ P3 落 Honcho drift_guard 时现状锚须以本包 tools patch 已应用态为准（plan.md 跨包事实节）。
- Human decision needed: 无 BLOCKING；合并门后部署轮（deploy.sh + K1-E7/E8 观测）由 human 执行，Pod 停 `awaiting-human:live-deploy` 不自行上 ECS。
- decisionCoverage 复核：本包 7 条 decision 全部 `landsIn` = exitCriteria（K1-E1/E2/E3/E4/E7）或姊妹包（n2a / k2b），**无 `landsIn: dropped` 条目**，risk-accepted 台账为空。

## Status 与回滚

- 建议包 status：`awaiting-human:live-deploy`（host 面全绿 + final bearing r2 绿；live 观测项停等，launch-card 钉死位）。spine 回写归主线程。
- 可回滚路径：单 commit revert 即回滚全部实现面（未 push、未合并；merge 后 main 侧回滚 = revert 同 commit）。tools patch live 侧另有 `rollback_agent_tools.py` 一步还原运行态。
- 里程碑：M5 state 仍 `planning`（P1 未 merged、停 live 门）；`planning → in_delivery` 翻账按 M4 先例归 human / 主线程，integrator 不改 milestones.jsonl。
