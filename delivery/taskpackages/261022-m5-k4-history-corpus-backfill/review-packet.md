# Review Packet · 261022-m5-k4-history-corpus-backfill

> integrator 合并前审查包，2026-10-23，初版 final round 4（invocationId `4b7f0a2e-9c1d-4e5f-8a3b-2d6c1e0f9a7b`）；**wire 修复轮复收口 final round 6**（invocationId `7c3e9a1f-2b4d-4f6a-9e8c-5d1b0a7f3e2c`，见 §13）。等待 supervisor 裁断；不 push、不 merge。

## 1. Change Size

| 维度 | 数字 |
|---|---|
| 新增实现面 | `server/deploy/honcho/backfill/`：backfill_honcho.py 546 行 + rollback_honcho_backfill.py 144 行 + test_backfill_honcho_unit.py 320 行（单测 16 项） |
| 冻结验收件（新） | `server/acceptance/test_261022_honcho_backfill.acceptance.py` 1076 行（K4-E1..E6 host 面 + K4-E6 structural 项头注申报自检） |
| 冻结验收件（改） | `server/acceptance/test_261022_honcho_memory.acceptance.py` — A4 禁区锚换代（test_k3_e1_no_backfill_subtree_a4_redline → test_k3_e1_no_backfill_in_k3_stack_assets，显式治理动作、头注换代注记留痕，见 redBaselineResult） |
| 部署面 | `server/deploy/manifest.yaml`（backfill 条目 2：两工具 dst `/opt/honcho/backfill/*`）+ `server/deploy/verify-deploy.sh`（backfill 消费位 drift 核验段） |
| TaskPackage 投影 | `delivery/taskpackages/261022-m5-k4-history-corpus-backfill/taskpackage.json` + `current/` 指针两件 |

总代码量约 2086 行新增 + 3 文件修改；无新增第三方依赖（stdlib-only，security gate 绿承载）。

## 2. Verification Evidence

- **final bearing round 4**：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --change-id 261022-m5-k4-history-corpus-backfill --round 4 --invocation-id 4b7f0a2e-9c1d-4e5f-8a3b-2d6c1e0f9a7b` → `observedGateExit=0`、10 闸全绿（taskpackage-required / change-manifest / firmware-build / contract-lint / acceptance-coverage / mutation-check / docs-sync / security / unit / acceptance）。fact = `verifier-evidence/rounds/r4.json`（`ok:true`，2026-10-23T15:14:33Z–15:15:15Z）。
- **fact 校验**：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261022-m5-k4-history-corpus-backfill --phase final --round 4 --invocation-id 4b7f0a2e-9c1d-4e5f-8a3b-2d6c1e0f9a7b` → `factOk:true`，exit 0。
- **test-scope 清单落盘路径**（Step 4 ① 圈定锚行）：`delivery/taskpackages/261022-m5-k4-history-corpus-backfill/test-scope-final-r4.txt`（6 行清单 = 本包 acceptanceTestsFile 2 件 ∪ impactedTests 种子 ∪ 共享 seam 安全余量 2 件〔memory_isolation 资格门口径对偶 + persistent_session transcript_events 源 seam〕 ∪ 单测面 1 件 ∪ verify-deploy.sh 留痕 1 条）。**留痕注记**：r4 fact results 元数据未含 scope 路径行（verify.mjs 未把 `--scope-file` 透传落 fact）——路径以本 packet 与 scope 文件自注承载，差集观察锚在 scope 文件内留痕。
- **圈定方式**：正常圈定（per-Pod 专属图索引 `C-AI-KidPal-pod-m2` detect_changes 可用）——changed_files 13、seed_symbols 74、inbound impacted_total=0（backfill 两工具 + 验收件零仓内调用方，blast radius 自包含；grep 抽查同结论）。主索引 `C-AI-KidPal` 首查对未 commit diff 无符号产出，走降级判定后被 per-Pod 索引补验升回，两次记录均在 scope 文件头注留痕。
- **final 圈定交集实测**：acceptance 闸 `serverAcceptanceFiles=4`，**52 passed / 0 failed / 3 deselected**；unit 闸 **68 passed / 0 failed / 3 deselected / 3 subtests**（structuralCollected 68）。deselected = live 标记件，预期。
- **全量口径参照**（scoped round 3，planner 驱动）：**333 passed / 0 failed / 7 deselected / 33 subtests**；红基线 319 passed / 14 failed（14 红全部 = 本包新件对未实现工具面的设计态红）→ 全绿零回退。
- **round 历史**：r1（scoped，ok:false，5 闸）→ r2（scoped，ok:false，8 闸）→ r3（scoped，ok:true，全量 333 绿）→ r4（final，ok:true，10 闸全绿）。

## 3. demo 对账行（workstream 包增锚 ①）

roadmap 效果段（roadmap.md:89）声明 demo 证据形态 =「host runner 输出（幂等二次跑零增量 + 计数报告 + 回滚脚本离线契约）+ live 运维（human 在 ECS 全量跑 + 计数报告过目 + 清库回滚演练）+ 收口轮抽问旧事」。逐行对账：

| roadmap 声明 | 本次实证形态 | 一致性结论 |
|---|---|---|
| host runner 输出·幂等二次跑零增量 | 冻结验收件 K4-E3 幂等断言（host 桩真跑：二次 apply 恰 ingested=0、skipped=全量、零写调用）+ 单测幂等项，r4 acceptance/unit 闸绿承载 | 一致 |
| host runner 输出·计数报告 | 冻结验收件计数报告 schema 断言（peers/sessions/messages/observations before/after + backfill.ingested/skipped/unparsed/perSource，plan/apply 两态落盘），r4 绿承载 | 一致 |
| host runner 输出·回滚脚本离线契约 | 冻结验收件 K4-E4（标记即键恰删、零标记零删调用、零源侧旗标）+ rollback 单测，r4 绿承载 | 一致 |
| live 运维·human ECS 全量跑 + 计数报告过目 + 清库回滚演练 | **未发生（按设计）**——taskpackage notes ⑥：backfill 执行面归 human，host 绿后停 `awaiting-human:live-deploy`，Pod 不自行部署 | 一致（停门位即声明的分工边界） |
| 收口轮抽问旧事 | 未到收口轮——归 P5 收口轮 owner（roadmap ④：P4 前后计数报告同轮对账、真机配额默认收口轮） | 一致（时序归属正确，非本包缺口） |

## 4. 未覆盖面申报（workstream 包增锚 ②）

对照 acceptance[]（K4-E1..E6）+ 契约条目（interface-contract §四）+ roadmap §3 效果段，逐条一行：

- **REST wire 形状（message metadata 可达性）**：**host 面已落定（r5/r6 wire 修复轮）**——端点与请求/响应形状经 upstream main@7478be29 源码对读 + ECS /opt/honcho 现读特征吻合实测（/v3 + workspace 作用域 + ≤100 分块 + kidpal-bf-<base64url> 资源名编码 + Bearer HONCHO_JWT 名字引用），单测 RestWireCase 十项承载；未定形申报已撤销。剩余未自动断言面 = **message metadata 逐消息可达性**（REST GET 侧是否回传 metadata 属真栈行为）——不可达则按冻结件缺口申报换 session 侧指纹清单方案并走 tighten-only 反审；去向：K4-E6 human 部署轮 live 对账（四项之一，见 §13）。
- **真语料格式适配**：无自动断言——host 面钉最小可测格式（`[<role>] <content>` 行 + transcript_events id/role/content 列），真 session-corpus 语料格式 = live 计数报告对账（unparsed 披露兜底、零丢弃）；去向：K4-E6 human 部署轮。
- **observations 异步增量与回滚逐 session 消息量预测**：无自动断言——deriver 异步增量在 plan 态只能记 after=before 预测、回滚逐 session 消息量不可经客户端协议预知（rollback 头注声明预测半）；去向：K4-E6 live 演练对账。
- **K4-E6 live structural 项**（ECS 全量跑 + 计数报告过目 + 幂等二次跑零增量 + 清库回滚演练 + 画像抽查）：无自动断言（structural）——停门 `awaiting-human:live-deploy`；去向：human 部署轮。
- **契约条目（interface-contract §四）**：none——本包零端云协议面改动（crossEnd exempt 留痕：设备端零感知、固件零 diff，git 事实零 firmware diff 承载）；记忆库形状非契约。
- 其余 exitCriteria：K4-E1/E2/E3/E4/E5 host 断言全绿（r4 承载）；**零额外缺口除上述四条 live 挂账项外为 none**。

## 5. Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | rollback 报告语义 | plan 预演态 `backfill.deleted = len(marked_keys)` 是「预删 session 数」而非已删计数，报告读者若只看 deleted 键可能误读（mode 键与头注已两处披露） | changed-code hunk `rollback_honcho_backfill.py:107-108` + 运维可读性 heuristic；非硬违规 | live 演练对账时先读 mode 再读 deleted；可留待 tighten-only 顺手在 plan 态改键名 |
| NIT | judgement-call | REST wire 假设 | ~~add_message body 为 JSON 数组（batch add 形状）、snapshot_counts 全量拉取建模——wire 形状按 Honcho REST 最佳知识落笔，未经真端点实证~~ **wire 修复轮已收敛（r5/r6）**：形状错误（/v2 + 缺 workspace 上下文）经源码对读修复，原「最佳知识」申报撤销；残留 = metadata 逐消息可达性仍属 live 首验点 | changed-code hunk `backfill_honcho.py:75-91`（minimal: 段含逐端点源码行锚）+ §13 wire 修复轮节 | metadata 可达性入 §13 四项 live 对账点；不需本包动作 |

无 BLOCKING、无 SHOULD；无 hard-violation。结构闸（verify.mjs 10 闸）PASS/FAIL 不复述为 finding，gate 全绿仍完整执行五轴语义 review（见第 6 节）。

## 6. Five-axis semantic review（author 五轴）

1. **安全**：出网仅 `--api` 缺省 Honcho loopback（DEFAULT_API=127.0.0.1:18100，单 `_request` 出入口）；stdlib urllib、零内联 secret、零凭据触碰；照记全量（不滤第三方姓名）= human 2026-10-22 终拍 R6/OQ-3 裁定，非实现自裁，零过滤旗标负锚有冻结断言；rollback 恰删 batch 标记 session、零源侧旗标（NFR-PRIV-1 三保留）。绿。
2. **契约**：零契约面 diff；记忆库形状非契约（crossEnd exempt 在册）；§四端云协议剖面零触碰。绿。
3. **边界**：K4-E5 两条口径不互串——grep 核实 backfill 树零资格门配置引用（仅 docstring 声明豁免），P3 插件工作台单源未动（git 状态零该目录改动）；身份口径按 ADR-016 现行版（session 键族设备级 + 家庭单 owner peer），K4-E2 措辞已由 planner 1f 保鲜权对齐，decisionCoverage OQ-7 历史原文保留不改。绿。
4. **可运维性**：plan 干跑缺省、--apply 显式写/删（迁移先预演铁律）；计数报告 plan/apply 两态落盘（M5 收口对账件）；exit 码 0/1/2 分明、不裸崩；manifest 登记 + verify-deploy.sh 消费位 drift 核验段。绿。
5. **测试**：单测 16 项（幂等/计数/回滚三态）+ 冻结验收 host 面（红基线 14 设计态红 → 全绿）；K4-E6 structural 项显式申报挂 human 轮；mutation 取证豁免有理由（数据证据面非分支逻辑面）。绿。

## 7. Simplification debt（`minimal:` 台账）

grep `minimal:` 两处命中，均集中在 HonchoRestClient wire 假设（r5/r6 后状态）：

- D1: ~~REST 端点/wire 按最佳知识落笔、live 一次校准~~ **已收敛**（backfill_honcho.py:75——形状经 upstream main@7478be29 源码对读实测落笔，`minimal:` 段改承载逐端点源码行锚；残留校准点仅 metadata 可达性，入 §13 live 对账）——`no-trigger`（wire 形态错误已在实现期消灭，无升级触发）。
- D2: snapshot/list 按全量拉取（size=100 逐页）建模，量级增大后换聚合端点（backfill_honcho.py:307）——`no-trigger`（现网量级小，live 对账确认后无动作）。

## 8. Residual Risk

- `decisionCoverage` 筛选（`landsIn === 'dropped' && reason.startsWith('risk-accepted')`）：**none**——本包 5 条 decision 全部落 exitCriteria:K4-E1..E5，无 risk-accepted dropped 条目。
- 残余风险清单：① live wire/真语料两处首验点（第 4 节前两条）——失败面 = 迁移工具校准，回滚路径（rollback 件 + OQ-10 重建双源）兜底；② 换设备 = 新 session 家族、画像不变（ADR-016 已知后果，arch §1.3 在案）；③ rollback plan 态 deleted 语义可读性（finding #1）。

## 9. 集成执行回报（Merge-Chain Execution 节）

未触发——本包无 merge-driver 冲突 abort 派单，无 rebase/冲突消解执行；集成树复跑清单 N/A。

## 10. UI design-review 证据

N/A——本包纯云端数据迁移与运维脚本，无 UIUX 面（input-brief ①c：uiux refs = none）。

## 11. 架构 / 依赖卫生

零新增第三方依赖（stdlib-only）；无跨层调用（backfill 工具自包含，零仓内 importer，per-Pod detect_changes impacted_total=0 承载）；无循环依赖；无绕过 contracts 的本地类型；K3 姊妹件锚换代为显式治理动作（acceptance-author 执行、头注留痕），非静默放宽。adrRequired=false（无新架构裁决；ADR-016 由面 C 既有批承载，本包消费）。

## 12. 制品与 commit 状态

- closeout 制品：`docs-delta.md`（零 diff 声明）+ 本 packet + `test-scope-final-r4.txt`（初轮）+ `test-scope-final-r6.txt`（wire 修复轮复收口）+ `verdict.json`（**final round 6 fact 晋升投影**，supervisor-compatible shape）+ `verification.json`（advisory 投影，finalScopedRun r6 + priorRounds r1..r5）落 `delivery/taskpackages/261022-m5-k4-history-corpus-backfill/`。
- commit：dev/pod-m2 分支（含全部实现面 + 验收件 + 投影 + 制品，wire 修复轮复收口二次 commit），**不 push**；`.codebase-memory/` 两件不入 commit。
- 后续门：supervisor-verdict-gate（planner 跑）→ pre-merge-escalation-gate → merge-driver --pod（planner 主线程）；K4-E6 human 部署轮独立于合并门（stop 位 awaiting-human:live-deploy）。

## 13. wire 修复轮（r5/r6）——复收口简记

**实锤缘由**：初轮收口（commit fe89b6fa，r4 全绿）后，主线程对照审查在 ECS `/opt/honcho` 现读实锤 builder 首轮 REST 适配的 wire 形态错误——REST client 按「最佳知识」落笔用了 `/v2` 端点且缺 workspace 上下文（friction 记录 pod-events `157cc490`，frictionType=scan-miss）；host 桩验收因鸭型协议自洽而全绿，未拦截形态错误。

**修法（源码对读）**：builder 按 upstream `plastic-labs/honcho` main@7478be29 本地克隆（C:/tmp/honcho-upstream-ref/honcho）逐端点对读修复，并与协调者 ECS `/opt/honcho` 现读特征吻合校验（全 router prefix `/v3`、routers 全 workspace 作用域）；`minimal:` 段承载逐端点源码行锚（sessions/peers/messages/conclusions 四 routers + schemas/api.py RESOURCE_NAME_PATTERN + utils/pagination.py + security.py Bearer）。

**实现期新适配（IMPLEMENTATION_ADAPTER 级，非契约变更）**：canonical 键含 `:` 不合 Honcho `RESOURCE_NAME_PATTERN`（`^[a-zA-Z0-9_-]+$`）→ 出网资源名编码 = `kidpal-bf-` 前缀 + base64url（去 padding），解码可逆（单测 roundtrip 断言承载）；其余修复项：`/v3` + workspace 作用域端点族、消息批量 ≤100 分块（MessageBatchCreate max_length=100）、列举 POST /list 分页（size ≤ 100）、凭据改 Bearer `HONCHO_JWT` 环境变量名字引用（token ECS 侧注入不入仓）。四分类判定：现契约无 memory wire 表达，纯实现侧适配，`IMPLEMENTATION_ADAPTER`（原地指导级），不触发 `/arch-change`。

**acceptance-author 申报收敛（tighten-only）**：冻结件头注「缺口申报」节收敛——原「REST wire 形状 = 最佳知识、未定形」申报撤销，诚实申报收窄为四项 live 对账点；断言、鸭型协议、coverage 标记零改动（冻结全量 333 passed 零变化）。

**收敛后 live 对账四项（K4-E6 human 部署轮，stop 位 awaiting-human:live-deploy）**：① message metadata 逐消息可达性（REST GET 侧真栈行为；不可达则换 session 侧指纹清单方案 + tighten-only 反审）；② 真 session-corpus 语料格式适配（unparsed 披露兜底、零丢弃，计数报告对账）；③ observations 异步增量（plan 态 after=before 预测）；④ 回滚逐 session 消息量预测（rollback 头注预测半）+ conclusions 异步派生等待窗观测。

**复跑数字**：
- scoped round 5（builder 修复 + 申报收敛后，invocation `baf02b5c`）：8 闸绿（fact `r5.json` ok:true，2026-10-23T15:45:25Z–15:45:37Z）。
- **final round 6（本复收口 bearing，invocation `7c3e9a1f`）**：`node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --scope-file delivery/taskpackages/261022-m5-k4-history-corpus-backfill/test-scope-final-r6.txt --change-id 261022-m5-k4-history-corpus-backfill --round 6 --invocation-id 7c3e9a1f-2b4d-4f6a-9e8c-5d1b0a7f3e2c` → `observedGateExit=0`、10 闸全绿、`ok:true`（2026-10-23T15:46:50Z–15:47:32Z）；**test-scope 清单落盘路径** = `delivery/taskpackages/261022-m5-k4-history-corpus-backfill/test-scope-final-r6.txt`（圈定方式 = 正常圈定：per-Pod 索引 `C-AI-KidPal-pod-m2` detect_changes，changed_files 26、seed_symbols 74、impacted_total=0，清单件数与 r4 相同、floor 不缩）。fact 校验：`verifier-evidence-check.mjs` 同参 → `factOk:true`、exit 0。
- 圈定交集实测：acceptance **52 passed / 0 failed / 3 deselected**（serverAcceptanceFiles=4）；unit **78 passed / 0 failed / 3 deselected / 3 subtests**（structuralCollected 68→78，builder 扩容 RestWireCase 十项 wire 形状断言）。
- 留根注记：r6 fact results 元数据仍未含 scope 路径行（verify.mjs 未透传落 fact），路径以本节与 scope 文件自注承载。

