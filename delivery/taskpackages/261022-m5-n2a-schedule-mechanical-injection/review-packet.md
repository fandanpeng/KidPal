# Review Packet · 261022-m5-n2a-schedule-mechanical-injection

- date: 2026-10-23 · author: integrator · branch: `dev/pod-m2`（worktree `C:/AI/KidPal-pod-m2`，未 push、未合并）
- workstream: device-memory-overhaul P5（rigor=lite，riskTier=medium，mergeOrder=51，dependsOn=K1 已合并）

## Change Size

- Files changed: 15（实现面 3 件改动 + 1 件新增验收，全在 changeManifest 6 件面内——`connection.py` +83/-3、`front_app.py` +142、`manifest.yaml` +1/-1、新增 `test_261022_schedule_injection.acceptance.py` 10 断言；`schedule.py` 零 diff 只读复用；`verify-deploy.sh` 零 diff；其余 11 件 = 簿记面：taskpackage 回填 / current/ 投影 / verifier evidence / test-scope / workstream progress / INDEX / debt 结项件）
- Net diff: server 实现面 3 件 +226/-4；firmware 零 diff（crossEnd exempt 成立——纯云端装配与单源复用，设备端零感知）
- Scope: server/deploy/engine-patch（占位填充缝 + 帧认领）+ server/gateway/front（单源派生 + 会话建立注入）+ server/acceptance（新验收件）
- Split recommendation: 不拆——E1–E6 六条 exitCriteria 共享同一「单源派生 → 内部通道 → 占位填充」不变式，拆开任一条都制造过渡态（注入通道在而内容缺源，或反之）

## Verification Evidence

- Structural gates: firmware-build PASS（final 相位真编译，xiaozhi.bin 0x3242f0 / 9% 空闲）；contract-lint PASS（1 条先存 warning：260923 归档件相对链接缺失，非本包引入，K1 同款）
- Acceptance/unit:
  - **final bearing round 2**（invocation `becb85f0-afdb-4d4d-a281-a40b99fb0325`，evidence `verifier-evidence/rounds/r2.json`，fact `ok:true`，observedGateExit 0，10 闸全绿）：acceptance 闸按 scope-file 圈定跑 = **160 passed / 5 subtests**（serverAcceptanceFiles=10，与清单 10 件恰合）；unit 闸 = **220 passed / 5 subtests**（13 件 unit 文件，零红）。
  - **scoped round 1**（invocation `86204f35-0898-47bc-bf79-85f3c2223028`，evidence `verifier-evidence/rounds/r1.json`，`ok:true`）：结构/治理 8 闸绿。
  - builder 全量（收拢前实测）：acceptance **278 passed / 4 deselected / 33 subtests**（10 断言全绿 + m3k1/m3k3 冻结件零新增红 = K5-E6 达成）；builder 申报 unit 115 passed 与 final 闸 220 passed 存在计数口径差异，以 final fact 为准（docs-delta 注记 2 已申报）。final 圈定跑 160 与全量 278 之差 = 未入影响面的 3 件 acceptance（圈定语义，非缩水——清单由 detect_changes blast radius + impactedTests 4 件种子 + 共享 seam 合成，见下）。
  - **test-scope 清单落盘路径**：`delivery/taskpackages/261022-m5-n2a-schedule-mechanical-injection/test-scope-final.txt`（本行即 Step 4 要求的 results 元数据必含行）。
  - 范围圈定来源：detect_changes（项目 `C-AI-KidPal-pod-m2`，base=main，merge_base `fdd643b3`，seed_symbols=45）impacted_total=66，测试面映射 6 件 + 种子 4 件 + 共享 seam 4 件（`{{schedule_summary}}` 占位载体 = K1 上游验收件、prompt 载体既有守卫件）。图可用性：per-Pod 专属图索引可用，正常圈定、无降级。
- Security gate: PASS（`securityReviewRequired=false`）。注入面人工安全核对：内部帧 `kidpal_schedule_summary` 为引擎内部指令、认领后绝不当孩子话语送 LLM；摘要文本只来自 front 单源派生的 schedule.md 内容，无用户可控注入路径扩大；缺席占位文本零编造。
- Mutation check: PASS（skipped，理由 = taskpackage `mutationSkipReason`：Lite 包、改动面小且为装配接线，单源反证 + 受控轮次为主，变异取证 ROI 不成立）。
- Docs sync: PASS（docsSyncRequired=false，spec 两文件与 docs/contracts/** 零 diff）；两条声明性注记登记于 `docs-delta.md`（P1 遗留占位观察 + unit 层口径），不改任何 docs 文件（docs/ 唯一改动 = TECH-261018 结项件，属 debt 账本结项授权路径）。
- verifier-evidence-check: PASS（exit 0，factOk=true）。

### 合并收口前影响面核对（Step 9b · advisory 记账）

detect_changes 已跑（见上）：15 changed files 全部为簿记面（delivery/**、docs/debt 结项件）或 changeManifest 登记面（server/** 4 件）。blast radius 映射：schedule.py 符号为 inbound 消费面命中（本包零改动、只读复用——正是设计意图）；front_app 命中符号全部是本包新增 `build_schedule_summary` / `_inject_schedule_summary` 及其邻接既有符号（schedule push / session legs 接线）；`firmware/main` 2 条为 usage 边噪声（固件零 diff）。无未预期生产符号波及，核对通过。

### 集成执行回报

本包未触发 merge-driver 冲突消解路径（单包在途、无对端包冲突），Pod 分支 rebase/merge 未执行——该节不适用，无实测数字可报。

## Demo 对账行（workstream 包硬性两条之一）

demo 证据 = **host runner 输出**（与 plan.md「demo 证据形态」节一致）：pytest 验收套件（final 圈定 160 passed / builder 全量 278 passed）——K5-E1 三断言（派生接缝单源符号 / 引擎零第二解析器 / 占位填充缝在引擎部署副本补丁内）、K5-E2 三断言（有界 3 日 ≤2048 字节 / 零意图门控 / 会话建立接线锚）、K5-E3 两断言（同日派生逐项一致含学校节次名 + 备注列逐字 / 窗口外零泄漏）、K5-E4 受控 21 轮课表提问零工具调用有料可答（钉行 ≥20）；变异敏感性以冻结件头注 RED 基线节承载（窗口越 4 日 / 内容抽空分别新爆 6/7 红，冻结相位在案）。与 roadmap 效果段**一致**：host 面 E1–E4 exitCriteria 全部有对应绿断言。**live 观测项（K5-E4 live 段：真机 ≥20 轮课表提问零工具调用零编造）如实标「待 human 部署轮」**——本包 host 面转绿后停 `awaiting-human:live-deploy`，为 launch-card 钉死的停等位，不是缺口。

## 未覆盖面申报（对照 acceptance[] 六条 + 契约 §1.5 + roadmap 效果段）

| 设计性质 | 自动断言现状 | 现状 | 去向 |
|---|---|---|---|
| K5-E4 live 段：真机 ≥20 轮课表提问零工具调用零编造 | 无（host 受控 21 轮已绿，live 段 docstring 申报挂部署轮） | host 面承载前提已达成（注入块含相对日标签 + 绝对日期双锚） | human 部署轮：deploy.sh 重启引擎（connection.py engine-face，设备断连重连 K1 同款）后真机抽问今天/明天/后天 |
| TECH-261018 结项状态（K5-E5） | `test_k5_e5_e6_declarations_exist` 守申报在位 | 结项已落：frontmatter status: closed + closedBy = 本包 + 结项记 append + debt-render 归位 `_archive/2026/` | 闭合（本包 integrator 收口路径完成） |
| P1 遗留：`{{today_date}}` 等日期/天气占位可能字面进 live prompt | 无（builder 遗留②，跨包观察项） | 本包 `{{schedule_summary}}` 已填不受影响；其余占位疑字面透传 | K5-E4 live 轮顺带实证；确认后归 P1 收尾另批小修（候选落点 connection.py `change_system_prompt` 同缝）——见 docs-delta 注记 1 |
| 部署轮 engine 断连窗口 | 无（部署面属性，非代码缺陷） | connection.py 重启 = 设备断连重连，已知部署属性 | 部署轮执行须知（manifest note 已载三条生效标记行核验锚） |
| 会话内课表变更不追（跨天最迟下一会话生效） | 边界记录（taskpackage K5-E2 条目明示「记录为边界」） | 相对日标签（今天/明天/后天）在跨天存活会话内会漂移，绝对日期头兜底 | 无去向（既定边界，T5 语义②原文） |
| 契约 §1.5 单源红线 | K5-E1 三断言（引擎零解析 / 单源符号 / 填充缝） | 契约字面零 diff，实现与红线口径一致 | 闭合 |

## Five-axis semantic review（author 五轴，结构闸 PASS/FAIL 外独立执行）

1. **安全**：内部帧认领先于绑定门（`_route_message` 首位），帧体只取 `text` 字段且 strip 后存会话态，非 dict / 非本 type 静默放行走既有路由——无新攻击面；摘要内容源 = 仓内 schedule.md（与现状读文件出网等价，TECH-261018 建议改动节已裁等价性）。缺席填「（本次会话未提供）」不编造。无 finding。
2. **契约**：单源红线三重守（派生函数只调 `gw_schedule.items_for_date`，零 MD 区块名常量、零表格切分；引擎侧零 import gateway、零 B1 区块名；K5-E1 断言机械守）。B1 / display E1 / 提醒 / 私有消息帧零改动。无 finding。
3. **边界**：字节预算整日粒度截断（不掐半条事实，首日超预算整行裁 + 显式省略标记）；帧先到/后到/二次到达三时序覆盖（`_kidpal_schedule_prompt_base` 占位原形快照回填）；非 str prompt 原样返回。judgement-call 一条见 Findings。
4. **可运维性**：启动标记行 `kidpal schedule-summary patch active` + manifest note 三标记核验锚；注入失败 warning 降级不阻断建连（`_push_schedule_connect` 同位先例）；日志 len 记账。无 finding。
5. **一致性/卫生**：无重复实现（派生唯一走 schedule.py）、无跨层调用（front→engine 单向内部帧，kidpal_speak 同型）、无循环依赖、零新增依赖、无绕过 contracts 的本地类型。无 finding。

## Simplification Debt

`minimal:` 标记 sweep：仓内现存 2 处（`server/shim.py`、`server/gateway/front/front_app.py:1831` reminder 轮询常量注）均为先存、非本包改动 hunk，本包零触碰。本包新增代码无 `minimal:` 标记。

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （本包无新增） | — | — | no-trigger |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | front_app.py `build_schedule_summary` | 相对日标签（今天/明天/后天）按注入时刻硬编码偏移 0..2，跨天仍存活的会话内标签会漂移（摘要头绝对日期兜底，事实不假但相对词可能错位） | changed-code hunk（`_DAY_LABELS` 常量 + 窗口循环）+ 边界启发：与 taskpackage K5-E2 已记录边界「会话内不追」同源，属已裁边界的表现层细节 | no-trigger（K5-E2 边界既定；live 轮若实证跨天长会话误答，随 P1 收尾小批一并裁） |
| NIT | judgement-call | front_app.py `_fit_summary_to_budget` | 首日块自身超预算时按整行裁进 2048 字节，首日事实可能不完整进注入块（其余日整块丢弃语义正确） | changed-code hunk + 有界启发：窗口内数据量极端时降级为部分内容 + 显式省略标记，不静默、不超限，与 T5 有界语义一致 | no-trigger（真实课表体量远小于 2KB；省略标记在案，模型可走读文件补全） |

无 BLOCKING / SHOULD finding。结构闸（closeout-check 等）结果不复述为 finding；gate 全绿仍完整执行了上述五轴语义 review。

## Residual Risk（decisionCoverage `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目）

taskpackage.json `decisionCoverage` 五条全部 `landsIn` 到 exitCriteria 或上游包，无 `dropped` 条目——本包无 risk-accepted 残余风险项。

## 制品与账本

- docs-delta：`delivery/taskpackages/261022-m5-n2a-schedule-mechanical-injection/docs-delta.md`（两条声明性注记）
- verdict.json（final fact 投影晋升）+ verification.json（advisory 投影）：同目录
- TECH-261018 结项：`docs/debt/tech/_archive/2026/TECH-261018-schedule-read-compliance-residual.md`（debt-render 已跑，tech 域 INDEX 收敛）
- 回滚方式：`git revert` 本 commit 即整体回退（单 commit、无数据迁移）；部署面回滚 = deploy.sh 回滚 connection.py 上一版（占位填「（本次会话未提供）」不影响既有回合路径）
- Merge packet / supervisor 输入：本 packet + verdict.json + verification.json + verifier-evidence r1/r2 + test-scope-final.txt；交 planner 跑 supervisor-verdict-gate 机械闸，本 agent 不自判通过、不执行 merge
