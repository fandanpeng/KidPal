# Review Packet · 261001-openclaw-model-name-overlay-drift

> integrator 收拢制品（2026-10-01），供 supervisor 机械合并门复核；riskTier=low（② 单包 Lite）→ 常规机械门路径，非高/critical。本包 = server 单面**配置/声明值**变更（引擎 `.config.yaml` LLM 段 `model_name` 由 `openclaw/default` 修为 `openclaw/kidpal`，同步四处仓内面）+ 冻结验收重钉 + ADR-008（amend ADR-006）。firmware / 契约 / spec 面零触碰。闭环口径：`MNF-A1`（四处正钉 + 残留扫描）/ `MNF-A2`（PERF-K1 冻结齿重钉）/ `MNF-A3`（部署演练，可选）+ `MNF-E1`~`E3` exitCriteria。

## Change Size

- **Files changed**：`git diff --stat HEAD`（实现+文档面）= **6 files, +33 / −9**；构成：
  - **实现面（四处单行值改，live/声明/探针/provider）**：`server/deploy/apply-xiaozhi-overlay.sh:72`（−1/+1，渲染面/live 真源）、`server/deploy/provider.config.yaml:62`（−1/+1，声明面）、`server/deploy/perf-k1/probe_batch.py:42`（−1/+1，探针默认值）、`server/gateway/provider.py:32`（−1/+1，provider 默认值，**无 live 消费方**）。
  - **冻结验收重钉**：`server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py`（+26/−?，PERF-E1/E2 期望值重钉 + `:917/:939` 失准文案订正 + `:929` docstring 旧值对齐 + REVISION LOG）。
  - **新增冻结验收**：`server/acceptance/test_261001_openclaw_model_name_overlay_drift.acceptance.py`（**新档 237 行**，MNF-A1/A2 承载齿）。
  - **文档面（docs-sync）**：`docs/ADR.md`（+8 行，append-only 新增 `ADR-008`）。
- **Net diff**：实现面四处各 1 行字面（`openclaw/default` → `openclaw/kidpal`）；验收 +26；ADR +8。**无新增/删除源文件、无逻辑分支改动**。
- **Scope**：单一子系统（server）+ 单变量（引擎 LLM 段 `model_name` 值），firmware / 契约 / 需求 / 架构零触碰。
- **Split recommendation**：无需拆分。变更是「渲染面 == 声明面 == 被测面」一条接线不变式（ADR-001）的内聚改动，切开会破坏两面一致。

## Verification Evidence

- **Structural gates**：`contract-lint` **exit 0**（契约 `docs/contracts/interface-contract.md` 未漂移）；`firmware-build` **exit 0**（真编译，`xiaozhi.bin` 0x311920B / app 分区余 11%——本包 firmware 零触碰，编译为回归护栏）。
- **Acceptance（全量 server 面，integrator 真跑锚）**：`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance` → **136 passed / 4 deselected / 9 subtests**（exit 0）。对 redBaselineCommand 重钉前基线（133 passed）→ 设计态红（7 failed = 3 齿级 + 4 subTest）→ 修后全绿：+3 齿级、+4 subTest 全转绿。
- **Acceptance（final r3 scope 内，executor-run）**：`acceptance` 闸 exit 0——`serverAcceptanceFiles=5`、`firmwareAcceptanceFiles=0`，scope 内 **60 passed / 4 deselected / 4 subtests**。
- **Unit（final r3 scope 内，executor-run）**：`unit` 闸 exit 0——scope 内 **60 passed / 4 deselected / 4 subtests**（本包无映射单测，scope 内命中由 acceptance 面文件承载；server/tests 三件不触本包改动面，未入范围——见 test-scope 头注）。
- **Security gate**：`security` 闸 exit 0（`securityReviewRequired:false`，未声明安全轴）。人工安全轴 review 见「Five-Axis Review」第 4 轴。
- **Mutation check**：`mutation-check` **exit 0**（`skipped:true`，如实理由 = 纯声明/配置默认值改动无分支/逻辑可 mutation；敏感性由 MNF-A1/A2 双向断言〔正钉 `assertIn` 目标值 + 负钉 `assertNotIn` 旧值〕承载；acceptance-author 已对 HEAD 版重放 MNF-A2 的 `assertNotIn` 三连实证转红）。
- **Docs sync**：`docs-sync` **exit 0**（`docRefs` = `docs/ADR.md#ADR-006`；`docsNoChangeReason` = 新增 ADR-008，arch/req/契约零改；详见 `docs-delta.md`）。
- **Bearing final fact**：`r3.json`（`ok:true`、`observedGateExit:0`、10 闸全绿）+ checker `verifier-evidence-check.mjs` `factOk:true`（见「Final 相位全闸」节）。

## Detect Changes / 影响面核对（Step 9b advisory）

- **detect_changes**（project `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch` 池树专属索引，base=main，merge_base `b191efb`）：changed_files=**16**、seed_symbols=**25**、**impacted_total=0**——改动符号（配置默认值字面）的传递调用方集合为空。**与 change surface 自洽**：改动全为**值字面替换**（无既有符号语义变更），故无入向影响面。非 `N/A (worktree)`——池树索引可见本次 diff，核对有效。
- **关键共享符号 trace_path 抽查**：`server/gateway/provider.py` 的 `openclaw_stream_request` / `provider_config`（`OPENCLAW_MODEL` 的两处引用方）——inbound callers_total=**0**（`trace_path` 实测，含 tests）。坐实 plan §7 architect 预评「该模块 LLM 面无 live 消费方」：改 `provider.py:32` = 纯声明一致、零 live 行为变化。live `front_app.py` 只用 `gw_provider.corr_id()` 与 `FEATURE_STREAMING_FORBIDS_MAX_TOKENS`（不动）。
- 结论：blast radius = 0；渲染面 heredoc（`apply-xiaozhi-overlay.sh`）为引擎 `.config.yaml` LLM 段唯一 live 真源（ADR-001 不变式持守）。

## Acceptance 逐锚结果（MNF-A1/A2/A3）

- **MNF-A1（基线与模板一致 · 仓内旧值零残留）**：**绿**。齿①`test_mnf_a1_live_declaration_surfaces_pin_kidpal` 四处显式正钉（每处 `assertIn` 目标值 + `assertNotIn` 旧值，机制级）；齿②`test_mnf_a1_no_stale_model_residue_on_live_surfaces` server 现役代码/配置面残留扫描——offender 集合 `[]`（扫描面 = `server/**` 现役代码/配置，排除验收件与退役面 `shim.py`/`kidpal-shim.service`/`archive/*`；历史记录类 `docs/crq/**`、ADR-006 正文、`PERF-K1-ACCEPTANCE-NOTES.md`、skill 注为 carve-out）。integrator 独立复核 grep：`server/**` 现役面（除验收件/退役面）`openclaw/default` **零命中**；`firmware`/`tools` 零命中。
- **MNF-A2（冻结验收重钉）**：**绿**。齿 `test_mnf_a2_perf_k1_frozen_teeth_repinned` 钉冻结件文本已重钉（`model: "openclaw/kidpal"` / `model_name: openclaw/kidpal` 在场 + 旧期望值不残留 + 失准文案 `§四钉 openclaw` 不残留）。行为侧红基线 = 重钉后 PERF-E1/E2 齿因仓内配置仍为旧值而转红；builder 改配置后转绿（`test_e1_provider_llm_segment_unchanged` / `test_e1_overlay_llm_segment_unchanged` 现绿）。
- **MNF-A3（部署演练）**：**未跑**（标 structural；性质 = 需 ECS + human 授权，非可断言离线齿）——见「未覆盖面申报」。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （无 `minimal:` 标记） | — | — | no-trigger |

- `grep -rn "minimal:" server/deploy/apply-xiaozhi-overlay.sh server/deploy/provider.config.yaml server/deploy/perf-k1/probe_batch.py server/gateway/provider.py server/acceptance/test_261001_openclaw_model_name_overlay_drift.acceptance.py` 零命中——本包无 `minimal:` 简化债务标记。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | judgement-call | Maintainability | `server/gateway/provider.py:32` 的 `OPENCLAW_MODEL` 默认值**无 live 消费方**——本处改动纯为声明一致（防未来漂移），当前对运行行为零影响。 | changed-code hunk `provider.py:32`；`trace_path` 实测 `openclaw_stream_request`/`provider_config` callers_total=0；plan §7 architect 预评坐实。 | 无需动作。保留声明面一致是 ADR-001「渲染面 == 声明面 == 被测面」不变式的组成部分；若未来清理死配置面，须另起包（不在本包范围）。 |
| NIT | judgement-call | Evidence hygiene | `server/acceptance/PERF-K1-ACCEPTANCE-NOTES.md:63-64` 逐齿映射表仍写旧值（`model openclaw/default` / `model_name openclaw/default`）。 | changed-code 语境：该件 = 承载包 260924 的**冻结设计/红基线/mutation 记录**，改写会伪造红基线与 mutation 证据（plan §8.5）。 | 故意留作 260924 历史快照，**不改写**；已在 `docs-delta.md` + 本包 evidence 显式记「该文件故意留作 260924 历史快照，261001 已重钉冻结件」。无动作。 |

> 无 BLOCKING / SHOULD finding。gate 全绿仍执行完整五轴语义 review（下节）。

## Five-Axis Review

1. **Correctness（是否满足声明 `MNF-Ay`，边界条件）**：`MNF-A1` 四处正钉 + 残留扫描绿、`MNF-A2` 冻结齿重钉绿——见逐锚节。边界：残留扫描范围**有意**限「server 现役代码/配置面」（排除验收件、退役面、历史记录）——全仓扫描会扫进历史/过程文本长期假红（齿头注明示）；`openclaw/kidpal` 点名 agent 定位不随 agent 数量变化（未来新增 agent 仍唯一定位）。`MNF-A3` 未跑为唯一未覆盖项（见未覆盖面申报），非遗漏。
2. **Test quality（是否断言真实行为）**：MNF-A1 齿断言**真实文件内容**（四处 `assertIn`/`assertNotIn` 双钉 + 目录级残留扫描实测 offender 集合）；MNF-A2 齿钉冻结件文本重钉（`assertNotIn` 负钉防回退）；行为侧由 PERF-E1/E2 齿真跑（读 render 源面断言 LLM 段）。负钉敏感性经 acceptance-author 对 HEAD 版重放三连实证转红（回退即红，非空过）。**强于**「没报错就算过」。
3. **Contract/boundary safety（是否漂移契约/spec）**：`docs/contracts/**` 零触碰（`contract-lint` 绿 + `git status --porcelain docs/contracts/` 零输出）；契约 §四对 `model_name`/`openclaw` 值**零命中**（本包从未把 model_name 引入契约面，`:917/:939` 失准文案「契约 §四钉 model_name」已订正）；`docs/requirements.md` / `docs/architecture.md` 零改（无漂移对象）；`docs/ADR.md` append-only（ADR-008 新增，ADR-001~007 未动）。**无漂移**。
4. **Operability/security（ErrorEnvelope/correlationId/权限/输入/secret/日志）**：本包为配置值字面改动，**无新增**权限/输入/secret/日志面。运维收益 = 消除引擎在双 agent 下的 400「no explicit owner」（设备侧对话一律失败）漂移复现路径：改渲染面 heredoc = 下次 overlay 重部署不再把引擎 `.config.yaml` 冲回旧值。未触凭证/网络/错误信封结构。**无安全残余需人类判断**（见 Residual Risk）。
5. **Maintainability/simplicity（局部/易懂/无过早抽象/无重复实现）**：四处均 1 行字面替换（`-1/+1`），**零重复实现、零过早抽象、零新依赖**；渲染面 == 声明面 == 被测面三面同值（ADR-001 不变式）；验收面新档 237 行局部内聚（MNF-A1/A2 承载齿 + 强度申报头注）。Findings #1/#2 为可维护性/证据卫生 minor。

## 未覆盖面申报（对照本包 `acceptance[]` + 契约条目）

- **MNF-A3 部署演练**：性质 = 结构性（需 ECS + human 授权），**非可断言离线齿**（taskpackage 已标 `kind:"structural"` + note「可选 human 授权」）；现状 = **未跑**（活配置已 261001 热修、仓内回流修法已落地）；去向 = 可选，human 授权后 overlay 在 ECS 重跑一次渲染核验。
- **引擎 live 行为（引擎真的以 `openclaw/kidpal` 发起请求）**：性质 = 需 ECS loopback `127.0.0.1:18789` 真发请求，仓内不可达；现状 = 离线齿只证「仓内声明面/渲染面文本已重钉」（齿头注「强度申报」缺口节明示）；去向 = **MNF-A3**。
- **契约条目 ↔ 齿映射**：契约 §四（端云协议剖面）本包**零触碰**、契约对 `model_name` 零命中 → 无「契约条目 ↔ 齿」映射缺口 = **none**（无验收断言挂契约条目）。
- **跨子系统 e2e / correlationId 链路**：性质 = 结构性跨端；现状 = 本包 server 单面、firmware 零触碰、`crossEnd.exempt` 明载「不改 wire、不触固件、不触契约与 spec 面；ECS 活配置已热修，本包只做仓内回流」；去向 = **不适用（none）**。

> 零缺口项明写：契约条目映射 = **none**；跨子系统 e2e / correlationId = **none**（本包 server-only、无跨端对偶面）。

## demo 对账行

- **本包 demo 证据形态** = **冻结验收 pytest 输出（RED→GREEN 轨迹）**，非 UI 截图（本包无 UIUX 改动，`designReviewRequired` 未声明，故无 viewport/a11y/design-drift 证据面）。
- **对账**：redBaselineCommand（`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance`）设计态 = 7 failed（4 subTest + 3 齿级，全为设计态红：四处正钉 4 subTest 红 + 残留扫描命中四处 live 面 + PERF-E1/E2 两行为齿红）→ 修后 **136 passed / 4 deselected / 9 subtests（0 failed）**；final r3 scope 内 60 passed。RED→GREEN 全程留痕、零新增红 ✓。
- **形态差异如实记**：MNF-A3 部署演练为 **ECS live 面**（需 human 授权），仓内 pytest 不可覆盖——对账时按「未跑 + 去向」读，不作已验证据。

## Residual Risk

- **Known limitation**：① MNF-A3 部署演练未跑（需 ECS + human 授权）——离线齿不证引擎 live 真的以 `openclaw/kidpal` 发起请求（齿头注强度申报缺口节明示）；② `PERF-K1-ACCEPTANCE-NOTES.md:63-64` 陈旧逐齿映射故意留作历史快照（Findings #2）。
- **Follow-up recommended**：① 下次 ECS 部署（human 授权）时执行一次 overlay 渲染演练，核验引擎 `.config.yaml` 落 `openclaw/kidpal` 且不覆盖其它段（MNF-A3，可顺带闭合）；② ARQ-261001-openclaw-model-name-overlay-drift 状态 `deferred → closed`（本包落地后转）。
- **Human decision needed**：**none**——riskTier=low、② 单包 Lite、无 BLOCKING finding、无契约/spec 面变更、无新增依赖。合并门判定归 supervisor 机械闸（`supervisor-verdict-gate.mjs`），低风险预期走常规 APPROVE 路径。
- **decisionCoverage dropped/risk-accepted**：本 TaskPackage `decisionCoverage` 字段缺失 → **无** `landsIn === 'dropped' && reason.startsWith('risk-accepted')` 条目——**none**。

## 回滚方式

- **仓内回滚**：四处实现面各一行 `openclaw/kidpal` → `openclaw/default`（`git revert` 本包 commit 即可；无数据面/无迁移）。本 Pod 分支未 push、未 merge；弃用 = 丢弃分支 commit。
- **ECS 活配置回滚锚**（若需退回旧态，**不建议**——会复现 400）：备份 `.config.yaml.bak-260930-modelname-clobber`。
- **本包不 push、不合并主干**；合并门归 supervisor 复核，planner 跑 `pre-merge-escalation-gate.mjs` 放行后 `merge-driver --pod` 本地合并。

## Final 相位全闸（`verify.mjs --phase=final`）

实跑（`round=3`，`invocationId=f49b13d2-e99b-4a9e-84df-488d70c12977`）：

- `node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --scope-file delivery/taskpackages/261001-openclaw-model-name-overlay-drift/test-scope-final-r3.txt --change-id 261001-openclaw-model-name-overlay-drift --round 3 --invocation-id f49b13d2-e99b-4a9e-84df-488d70c12977` → **exit 0**，`{"ok":true,"phase":"final","round":3,"observedGateExit":0}`，产 `verifier-evidence/rounds/r3.json`。
- `node .claude/scripts/verifier-evidence-check.mjs --change-id 261001-openclaw-model-name-overlay-drift --phase final --round 3 --invocation-id f49b13d2-e99b-4a9e-84df-488d70c12977` → **exit 0**，`{"ok":true,"gate":"verifier-evidence","factOk":true,"observedGateExit":0}`。

**final 相位 10 闸全绿**（r3.json，`observedGateExit=0`）：

| 闸 | kind | exit | ms | timing | 关键读数 |
|---|---|---|---|---|---|
| `taskpackage-required` | governance | 0 | 261 | scoped | ok |
| `change-manifest` | governance | 0 | 398 | scoped | pre-cutoff 存量放行（gitFirstCommitDate 2026-09-11 < MANIFEST_CUTOFF 2026-09-28） |
| `firmware-build` | structural | 0 | 8516 | scoped | 真编译 build-pass（xiaozhi.bin 0x311920，app 分区余 11%；本包 firmware 零触碰） |
| `contract-lint` | structural | 0 | 128 | scoped | 契约未漂移 |
| `acceptance-coverage` | functional | 0 | 136 | scoped | MNF-A1~A3 coverage 齐；`structural:["MNF-A3"]` / `suiteGates:[]` |
| `mutation-check` | functional | 0 | 138 | scoped | skipped（纯声明/配置值，如实理由） |
| `docs-sync` | governance | 0 | 134 | scoped | docRefs 全提及验收 id |
| `security` | security | 0 | 121 | scoped | `securityReviewRequired:false` |
| `unit` | functional | 0 | 3071 | full | 60 passed / 4 deselected / 4 subtests（scope 内，executor-run） |
| `acceptance` | functional | 0 | 2960 | full | 60 passed / 4 deselected / 4 subtests（scope 内 5 文件 = serverAcceptanceFiles 5 / firmwareAcceptanceFiles 0） |

- **test-scope 清单落盘路径**：`delivery/taskpackages/261001-openclaw-model-name-overlay-drift/test-scope-final-r3.txt`（范围 = 本包 acceptanceTestsFile ∪ blast radius 映射测试 ∪ 共享 seam 相关测试 ∪ 安全余量；详见该文件头注）。

> **承重判定**：validated final fact（r3.json，`ok:true`，checker `factOk:true`）——本包 final 证据有效，可进入 closeout。**等待 supervisor 机械闸裁断**（integrator 不自行判定通过、不 merge）。
