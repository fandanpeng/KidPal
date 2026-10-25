# Review Packet · 261024-m5-k4b-corpus-parser-v2

> integrator 合并前审查包，2026-10-24，final round 2（invocationId `9ceb48f1-c8fe-41f2-89a9-9423be368523`）。等待 supervisor 裁断；不 push、不 merge。上游 = architect 对抗 review `delivery/review/261022-arch-review-device-memory-prompt-overhaul/k4-attribution-fix-review-20261024.md` + human D1-D5 五项裁决；技术方案权威 = review §2.1。

## 1. Change Size

| 维度 | 数字 |
|---|---|
| 解析器 v2 | `server/deploy/honcho/backfill/backfill_honcho.py` +339/−46（逐行双层识别 + 续行归组 + 内嵌 turn 拆分 + created_at 回填 + toolResult 路由 + 报告扩展 additive 键；K4B-E1/E3/E4） |
| 回滚工具 | `server/deploy/honcho/backfill/rollback_honcho_backfill.py` +28/−4（`--source-kind {session-corpus,openclaw-transcript}` 选择性删除，缺省 = 全部旧行为；plan 输出 matched/skipped 逐键；K4B-E2） |
| 单测 | `server/deploy/honcho/backfill/test_backfill_honcho_unit.py` +624/−6（单测 16→64 项） |
| 冻结验收件 | `server/acceptance/test_261022_honcho_backfill.acceptance.py` +752/−2（**锚换代**：CORPUS fixture 增双层机器格式样例、`CORPUS_EXPECTED_MESSAGES` 7→12 显式换代注释留痕、`_StubHonchoV2` additive 桩〔冻结四参旧桩逐字保留供兼容探测两态〕；K4 既有 15 项测试方法体零改动 + K4B 新增 7 件：E1/E3/E2/E4/E5/Gate A3 快照审计/Gate A4 拆分抽样） |
| 部署资产 | `server/deploy/honcho/honcho.env.example` +12（`EMBEDDING_MODEL_CONFIG__MAX_BATCH_SIZE=10` + `LLM_DEFAULT_MAX_TOKENS=8000` 注释口径；K4B-E5，仅仓内模板） |
| 投影 | `delivery/taskpackages/current/` 两件（taskpackage.json 指针换代 + verdict.json final r2 投影，verify.mjs 程序 fs-write） |

总改动约 1760 插入 / 62 删除（7 文件）；零新增第三方依赖（stdlib-only 延续）；零固件 diff（crossEnd exempt：设备端零感知）；含 K4 HOLD 期 fixer 未 commit 补丁（review §1.5 裁 KEEP 为解析器 v2 基底，本 commit 一并收口）。

## 2. Verification Evidence

- **scoped round 1**（invocation `f81ac003-a60e-40f6-8f6b-a87e0687a3f4`）：8 闸绿、`ok:true`——lite 包 firmware-build scoped 相位后置（261015-gov-review-lite-fast-path），fact `verifier-evidence/rounds/r1.json`。
- **final bearing round 2**（invocation `9ceb48f1-c8fe-41f2-89a9-9423be368523`）：`observedGateExit=0`、**10 闸全绿**（taskpackage-required / change-manifest / firmware-build〔真编译，xiaozhi.bin 9% 余量〕/ contract-lint / acceptance-coverage / mutation-check〔豁免声明在册〕/ docs-sync / security / unit / acceptance）、`ok:true`（2026-10-24T09:47:07Z–09:47:28Z），fact = `verifier-evidence/rounds/r2.json`。
- **fact 校验**：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261024-m5-k4b-corpus-parser-v2 --phase final --round 2 --invocation-id 9ceb48f1-c8fe-41f2-89a9-9423be368523` → `factOk:true`、exit 0（integrator 复跑确认）。
- **test-scope 清单落盘路径**（Step 4 ① 圈定锚行）：`delivery/taskpackages/261024-m5-k4b-corpus-parser-v2/test-scope-final-r2.txt`（5 行 = 本包 acceptanceTestsFile 1 件 ∪ impactedTests 种子 2 件 ∪ env.example 1 件 ∪ 回滚工具 1 件，与 changeManifest 5 文件恰同）。**留根注记**：r2 fact results 元数据未含 scope 路径行（verify.mjs 未把 `--scope-file` 透传落 fact），路径以本 packet 与 scope 文件自注承载，差集观察锚在 scope 文件内留痕。
- **圈定交集实测（r2 final）**：unit 闸 **86 passed / 0 failed / 3 subtests**（structuralCollected 86 = 单测 64 + 验收件 22 桩级收集）；acceptance 闸 **22 passed / 0 failed**（serverAcceptanceFiles=1，零 live 标记 deselect——A3/A4 快照审计本机在盘实跑、零 skip）。
- **全量口径参照**（planner 起包红基线回填实测，2026-10-24）：`cd server && python -m pytest -q --import-mode=importlib -m "not live" acceptance` = **340 passed / 7 deselected / 33 subtests**（27.8s）——333 既有全绿零回退 + 本包新增 7 件全绿；单测 64 passed。零 skip。
- **round 历史**：r1（scoped，ok:true，8 闸）→ r2（final，ok:true，10 闸全绿）。无红轮、无 fixer 返工轮。

## 3. demo 对账行（workstream 包增锚 ①）

plan.md demo 证据形态声明 =「host runner 输出：Gate A（A1 单测 / A2 acceptance / A3 快照审计逐值等 / A4 拆分抽样）+ 命令单（供 live 轮照着敲）」。逐项对账：

| plan 声明 | 本次实证形态 | 一致性结论 |
|---|---|---|
| A1 单测 | r2 unit 闸 86 passed / 3 subtests（单测 64 项，含归组/拆分/指纹 v1/created_at/报告键/toolResult 路由族），绿 | 一致 |
| A2 acceptance | r2 acceptance 闸 22 passed；全量 340 passed / 7 deselected / 33 subtests 零回退，绿 | 一致 |
| A3 快照审计逐值等 | `test_k4b_gate_a3_snapshot_audit_frozen_values` 对真语料快照（`C:/tmp/k4-corpus-review/txt` 21 文件，逐件 sha256 冻结于 `K4B_SNAPSHOT_FILES`）本机实跑、零 skip：1652 非空行 → 1007 组 → 2879 段、unparsed=0、perRole user 1580 / assistant 1299 / other 0、指纹 2879 全唯一、无 content 以 `[kidpal/sessions/` 开头 | 一致（与冻结锚逐值等） |
| A4 拆分抽样 | `test_k4b_gate_a4_split_sampling_0930_embedded_turns` 对 2026-09-30.txt（59 行，sha256/行数冻结）：59 行 → 35 组（user 17 / assistant 18）→ 128 段（user 69 / assistant 59）；源消息 440/444/445 类内嵌小伴话术拆出 assistant 段、content 不含外层/切段 `User: `/`Assistant: ` 标记（切段标记被消化，全快照 0 残留），本机实跑、零 skip | 一致 |
| 命令单（live 轮照着敲） | 见本 packet §4 末「live 运维命令单」（回滚选择性删除 + 重灌两段，按冻结 CLI 旗标落笔；执行归主线程经 human 授权） | 一致（制品位 = 本 packet） |
| 冻结锚定数对账 | human D1 / review §2 离线模拟锚 = 1007 组 → 2879 条（user 1580 / assistant 1299），wrapper 标记计数 350（验收件锚按**标记出现次数**计全快照 350、243 个组首段是标载体段——review §1.4 的 246 为带标记**组数**口径，两口径不矛盾、不同单位；保留变体下剥离会使载体段变空、2879 不可达，故不剥只计数）——四值全部冻结进验收件 `K4B_SNAPSHOT_GROUPS/SEGMENTS/PER_ROLE/WRAPPER_MARKERS`（created_at 全部 = 文件日期 T04:00:00+00:00，human D3 口径），A3 断言逐值等 | 一致 |
| 与 review §4 判据一致性 | Gate A 四项判据（A1 单测绿 / A2 acceptance 绿 / A3 unparsed 0 + perRole 逐值等 + 指纹唯一 + 无首层源引用前缀 content / A4 拆分抽样）逐项落成冻结断言，判据无弱化、无缺项 | 一致 |

## 4. 未覆盖面申报（workstream 包增锚 ②）

对照 acceptance[]（K4B-E1..E6）+ review §4 Gate A/B/C/D，逐条一行：

- **live 运维段（Phase 1.5-4：Gate B 试灌读回 / Gate C 硬删完成 / Gate D 派生后验）**：**零 host 断言**（structural，按 taskpackage notes ⑤ 铁律）——执行归主线程经 human 授权（human D5 裁低峰窗口），Pod 不自行部署；rollback `--source-kind`（K4B-E2）与命令单（本节末）已备好工具面，B2 的 role→peer 出网映射真栈验证只能 live 做。
- **A3/A4 机器相关性（快照缺席机器）**：真语料快照 = `C:/tmp/k4-corpus-review/txt` 21 文件只读副本，**孩子语料不入仓**（隐私红线）——快照缺席机器上 A3/A4 显式 `skipTest「快照缺席，审计未跑」`申报（诚实降级，不静默跳过）；在盘但与冻结 sha256/行数不符 → RED。本机（worktree 所在机）快照在盘、实跑零 skip；其它机器跑全量会显式 skip 两件，属申报行为非缺陷。
- **包装标记语义键名 `wrapperMarkersStripped` 与字面不符（双侧注释钉死）**：采用 review §2.1 ③ 保留变体「**不剥、只计数**」——剥离会使 243 个包装标载体段变空、冻结锚定数 2879 不可达且产生空消息，故标记保留在段文本中、键名沿用冻结报告键面而语义 = 协议标签出现数；双侧注释（实现 `backfill_honcho.py:71-76` 与 `:167-170`，验收件 `:265` 与 `:1342`）钉死该语义，防后来读者按字面误判「内容被丢弃」。
- **transcript 源形状漂移不修**：生产表形状 `(session_id, seq, event_json, created_at)` vs 工具/验收契约 `id/role/content` 直读的漂移不在本包修——human D2 裁 transcript 批次一并回滚、不重灌，影子导出器重建（per-session 过滤 + 主键 `(session_id, seq)` + role 含 toolResult + created_at 保留 + 脚本入仓）挂 debt：`docs/debt/tech/TECH-261022-device-memory-prompt-overhaul.md` 2026-10-24 K4b append 条（integrator 本轮登记）。
- **本次无验收断言的设计性质，逐条一行**：
  - Gate B 试灌读回（role→peer 真栈映射、agent-kidpal 自动入会话）——无 host 断言（live 运维，structural 申报）；
  - Gate C 硬删完成核验（sessions/messages/documents/queue 残留 0、workspace 回落 96±live）——无 host 断言（同上）；
  - Gate D 派生后验（queue error 阈值、documents 分布、语义抽检两元钱类/时间线、覆盖度报告）——无 host 断言（同上）；
  - ECS `/opt/honcho/.env` 两键实际写入（EMBEDDING_MODEL_CONFIG__MAX_BATCH_SIZE=10 / LLM_DEFAULT_MAX_TOKENS=8000）——无 host 断言（K4B-E5 明裁仅仓内模板）；
  - deriver EmptyRepresentation 截断残量（D4 允许少量、超阈升级）——无 host 断言（live 后验计数）；
  - **零缺口项：none**——除上述 live 运维挂账外，K4B-E1..E5 host 断言全绿、K4B-E6 Gate A 四项本机实跑全绿，无其它未覆盖面。

### live 运维命令单（主线程经 human 授权后照此执行；非本包动作）

```bash
# Phase 2 · 回滚（ECS，低峰窗开始前）——D2 裁两源一并删；先 plan 后 apply
python /opt/honcho/backfill/rollback_honcho_backfill.py --report-out /tmp/k4b-rollback-plan.json
python /opt/honcho/backfill/rollback_honcho_backfill.py --apply --report-out /tmp/k4b-rollback-apply.json
#   · 需只删单源时加 --source-kind {session-corpus,openclaw-transcript}（K4B-E2；缺省 = 全部）
#   · plan 报告必含 matched/skipped 逐键；apply 后等硬删完成 = Gate C（queue 残留 0、
#     messages 回落 96±live、documents 回落 170 量级；DELETE 202 异步窗）
# Phase 1.5 · 两键写入 /opt/honcho/.env 后 docker compose up -d（restart 不重读 env）：
#   EMBEDDING_MODEL_CONFIG__MAX_BATCH_SIZE=10（仓内模板已同步，K4B-E5）
# Phase 3 · 重灌（Gate B 试灌先行：单文件 2026-09-30.txt 独立 batch apply → 读回核对 → 删试灌 session）
python /opt/honcho/backfill/backfill_honcho.py --corpus-dir <语料目录> --device-id <device_id> \
  --report-out /tmp/k4b-backfill-plan.json            # 先 plan 态对账 perRole/corpusFiles sha256
python /opt/honcho/backfill/backfill_honcho.py --corpus-dir <语料目录> --device-id <device_id> \
  --apply --report-out /tmp/k4b-backfill-apply.json   # 全量 apply；二次跑幂等零增量
# Phase 4 · Gate D 后验（queue 无 pending / 新 batch error ≤ 声明阈值 / 语义抽检）
```

## 5. Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | 报告键语义 | `wrapperMarkersStripped` 键名与「不剥、只计数」语义字面不符——按字面读会误判「内容被剥离」，实际标记保留在段文本、键 = 协议标签出现数（保留变体是冻结锚 2879 可达的充要选择） | changed-code hunk `backfill_honcho.py:71-76`/`:167-170` + 验收件 `:265`/`:1342` 双侧注释 + 可读性 heuristic；非硬违规（键名为冻结报告面，改名反而是破面动作） | 双侧注释已钉死；live 轮读报告时按注释口径读；不改键名 |
| NIT | judgement-call | 单层旧格式理论歧义 | 单层旧格式行 content 恰以 `User: `/`Assistant: ` 起头时会被内嵌拆分改判第二层 role——review §1.5 已定性「真实语料零单层行、属理论风险」，本包维持不加新启发式（review 明示不建议本轮引入） | review `k4-attribution-fix-review-20261024.md` §1.5 结论 + changed-code hunk `_second_layer_role`；judgement-call | 若未来语料出现单层行且 content 起头同形，按 review 预案（外层方括号需似源引用的收紧启发式）走 tighten-only，不热修 |

无 BLOCKING、无 SHOULD、无 hard-violation。结构闸（verify.mjs 10 闸）PASS/FAIL 不复述为 finding，gate 全绿仍完整执行五轴语义 review（见第 6 节）。

## 6. Five-axis semantic review（author 五轴）

1. **安全**：零新增出网通道（REST client 沿用 K4 恰一 `_request` 出入口、loopback 缺省）；created_at 回填与指纹 v1 均为 metadata 侧扩展、零凭据触碰；照记全量不滤第三方姓名 = human 2026-10-22 终拍延续，非本包新裁；rollback 选择性删除缺省保持全量旧行为，`--source-kind` 只是收窄删除面、无放大面；孩子语料快照不入仓红线保持（快照锚只有 sha256 + 行数入冻结件）。绿。
2. **契约**：零契约面 diff（`git status docs/` 空，docs-delta 零 diff 声明承载）；记忆库形状非契约（crossEnd exempt 在册）；duck 协议 `add_message` 增可选 `created_at` 参数 = 向后兼容扩展（缺省行为不变，既有桩/断言零改），非契约变更。绿。
3. **边界**：解析器行为升级全部收在本包 5 文件 manifest 圈定面内；单层旧格式回落逐字保留（fixture 单层 5+2 行逐字保留 + 断言兼容，单测承载）；K4 既有 15 项测试方法体零改动、锚值显式换代（`CORPUS_EXPECTED_MESSAGES` 7→12 等四处，换代注释逐处留痕、`_StubHonchoV2` additive 非替换）——既有冻结件行为零回退（全量 340 绿含 K4 段）；报告扩展全 additive、顶层冻结键零破坏（E3 断言逐键验顶层键集）。绿。
4. **可运维性**：rollback `--source-kind` 使「只回滚 corpus」从不可表达变为单旗标操作，plan 报告 matched/skipped 逐键（运维可预读删除面）；created_at 回填修复 201/521 份 documents 的假时间线（「孩子成长时间线」类推理的前置件）；报告扩展 perRole/corpusFiles sha256 给 live 轮对账逐文件锚；exit 码 0/1/2 分明延续、plan 缺省 / --apply 显式铁律延续。绿。
5. **测试**：单测 64 项 + 验收件 22 项（K4 15 保留 + K4B 7 新增）；真语料快照审计逐值等（1652→1007→2879、1580/1299、wrapper 350、指纹全唯一）是比变异取证更强的实证面——mutation 豁免声明（taskpackage `mutationSkipReason`）在本包成立：解析行为已被冻结锚逐值钉死，变异存活即 RED。零 skip（本机）。绿。

## 7. Simplification debt（`minimal:` 台账）

grep `minimal:` 两处命中，均为 K4 承接件（本包未新增）：

- D1: REST wire 形状按 upstream main@7478be29 源码对读实测落笔、逐端点源码行锚（backfill_honcho.py:116）——`no-trigger`（K4 wire 修复轮已收敛，本包零 wire 改动）。
- D2: 快照/指纹枚举按全量拉取建模（size=100 逐页；现网量级小），metadata 逐消息可达性 = live 全量跑首验点（backfill_honcho.py:541）——`no-trigger`（沿 K4 挂账，去向 K4-E6/K4B live 对账，无升级触发）。

## 8. Residual Risk

- `decisionCoverage` 筛选（`landsIn === 'dropped' && reason.startsWith('risk-accepted')`）：**none**——本包 taskpackage 无 `decisionCoverage` 字段（human D1-D5 五项裁决全部落进 exitCriteria:K4B-E1..E5 + E6 structural 申报，无 risk-accepted dropped 条目）。
- 残余风险清单：① live 运维段（Gate B/C/D）零 host 断言——重灌结果只能 live 首验，失败面 = 回滚路径（`--source-kind` 选择性删除 + OQ-10 重建双源）兜底，且 D2 裁 transcript 不重灌、失败影响面收窄到 corpus 单源；② A3/A4 依赖机外快照（孩子语料不入仓）——其它机器显式 skipTest 申报，冻结 sha256 锚保证快照不可静默漂移；③ wrapperMarkersStripped 语义键名（finding #1）——注释钉死，无行为风险；④ deriver 截断残量（D4 允许少量 EmptyRepresentation）——live Gate D 显式计数、超阈升级，非静默。

## 9. 集成执行回报（Merge-Chain Execution 节）

未触发——本包无 merge-driver 冲突 abort 派单，无 rebase/冲突消解执行；集成树复跑清单 N/A。

## 10. UI design-review 证据

N/A——本包纯云端数据迁移修复轮（解析器 + 回滚工具 + 部署资产模板），无 UIUX 面。

## 11. 架构 / 依赖卫生

零新增第三方依赖（stdlib-only 延续，K4B-E5 两键为 env 模板值非代码依赖）；无跨层调用（backfill 两工具自包含、零仓内 importer——K4 已核 impacted_total=0，本包只在其 3 文件内深化，未新增模块间引用）；无循环依赖；无绕过 contracts 的本地类型（零契约面触碰）；K4 既有冻结断言零放宽（测试方法体零改动；锚换代 = 显式治理动作，换代注记逐处留痕、非静默放宽——验收件锚换代由 acceptance-author 执行并头注申报）。adrRequired=false（解析器 v2 = review §2.1 修正案逐条落地 + human D1-D5 既有裁决消费，无新架构裁决）。

## 12. 制品与 commit 状态

- closeout 制品：`docs-delta.md`（零 diff 声明）+ 本 packet + `test-scope-final-r2.txt`（圈定清单）+ `verdict.json`（**final round 2 fact 晋升投影**，= r2.json 逐字副本，supervisor-compatible shape）+ `verification.json`（advisory 投影，finalScopedRun r2 + priorRounds r1）落 `delivery/taskpackages/261024-m5-k4b-corpus-parser-v2/`。
- debt 登记一处（debt 白名单授权位）：`docs/debt/tech/TECH-261022-device-memory-prompt-overhaul.md` append「transcript 影子导出器重建（D2 裁不重灌）」条。
- workstream 记忆：`delivery/workstreams/device-memory-overhaul/progress.md` append K4b 节。
- commit：dev/pod-m2 分支（含全部实现面 + 验收件 + K4 HOLD 期 fixer 补丁 KEEP 收口 + 投影 + 制品），**不 push**；`.codebase-memory/` 两件不入 commit。
- 后续门：supervisor-verdict-gate（planner 跑）→ pre-merge-escalation-gate → merge-driver --pod（planner 主线程）；live 运维段（Phase 1.5-4）独立于合并门，归主线程经 human 授权（D5 低峰窗口）。
