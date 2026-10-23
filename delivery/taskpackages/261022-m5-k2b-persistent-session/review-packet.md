# Review Packet · 261022-m5-k2b-persistent-session

- date: 2026-10-23 · author: integrator · branch: `dev/pod-m2`（worktree `C:/AI/KidPal-pod-m2`，未 push、未合并）
- workstream: device-memory-overhaul P2（rigor=full，riskTier=high，mergeOrder=52，dependsOn=K1 已合并；P5 亦已合并部署）
- gateProfile: evidence-integrity / test-quality · sourceType: proposal（PROPOSAL FROZEN v1.0 + arch PROPOSAL FROZEN v1.1）

## Change Size

- Files changed: 15（实现面 3 件改动 + 1 件新增，全在 changeManifest 6 件面内——`connection.py` +124/-7（切片接线 + 探针门控 marker）、新增 `engine-patch/openai.py` 216 行（stock 基线 + keyword-only user + 零 max_tokens）、`docker-compose.yml` +6（openai.py bind-mount 行 + 人工生效步注释）、`manifest.yaml` +4/-2（新资产条目 + marker 清单补第四条）、新增冻结验收件 `test_261022_persistent_session.acceptance.py` 1039 行 14 断言；`verify-deploy.sh` 零 diff（预期内，docs-delta 注记 3）；其余 10 件 = 簿记面：taskpackage 回填 / current/ 投影 / verifier evidence / test-scope / workstream progress）
- Net diff: server 实现面 4 件 +136/-7 + 新增 2 件 1255 行；firmware 零 diff（crossEnd exempt 成立——additive 请求字段 user，设备端零感知）
- Scope: server/deploy/engine-patch（持久会话补丁与切片装配）+ server/deploy 部署登记（compose/manifest）+ server/acceptance（新冻结件）
- Split recommendation: 不拆——K2-E1..E5 五条 exitCriteria 共享同一「只发本回合 + user 键 + 漂移护栏」不变式；护栏（E3）与接线（E1/E2）拆开即制造无报警锚的过渡态

## Verification Evidence

- Structural gates: firmware-build PASS（final 相位真编译，xiaozhi.bin 0x3242f0 / 9% 空闲）；contract-lint PASS（1 条先存 warning：260923 归档件相对链接缺失，非本包引入，K1/P5 同款）
- **final bearing round 2**（invocation `2b7d9e4a-5c31-4f8a-9e62-d01c4471ab55`，evidence `verifier-evidence/rounds/r2.json`，fact `ok:true`，observedGateExit 0，**10 闸全绿**：taskpackage-required / change-manifest / firmware-build / contract-lint / acceptance-coverage / mutation-check / docs-sync / security / unit / acceptance）：
  - acceptance 闸按 scope-file 圈定跑 = **89 passed / 4 subtests**（serverAcceptanceFiles=7，与清单 7 件 acceptance 恰合）；unit 闸按同清单求交 = 89 passed / 4 subtests（unit 闸口径 = 清单内可收集测试件，交集语义，docs-delta 注记 2）。
  - verifier-evidence-check PASS（exit 0，factOk=true）。
- **scoped round 1**（invocation `7322233d-491c-498d-bbcf-aecd5aa11da4`，evidence `verifier-evidence/rounds/r1.json`，`ok:true`）：结构/治理 8 闸绿。
- builder 全量（收拢前实测）：acceptance **292 passed / 0 failed / 4 deselected / 33 subtests**（冻结件 14 断言全绿 + 既有冻结件零新增红 = K2-E5 达成）；unit 115 passed 零新增红。final 圈定跑 89 与全量 292 之差 = 未入影响面的 4 件 acceptance（圈定语义，非缩水——清单见下）。判法目标数 281 vs 实数 292 的口径说明见 docs-delta 注记 1。
- **test-scope 清单落盘路径**：`delivery/taskpackages/261022-m5-k2b-persistent-session/test-scope-final.txt`（本行即 Step 4 要求的 results 元数据必含行）。
- 范围圈定来源：detect_changes（项目 `C-AI-KidPal-pod-m2`，base=main，merge_base `5cb5bae9`，changed_files=15，seed_symbols=58）impacted_total=7——影响集全为 python-builtins / firmware gifdec usage 边噪声（本包变更面 = 部署副本补丁 + compose/manifest 登记行，不产生产产符号入边，blast radius 未映射出 server 测试文件）；测试面 = acceptanceTestsFile 1 件 ∪ impactedTests 种子 3 件 pytest（第 4 件 verify-deploy.sh 非 pytest 件、零 diff，清单头注记）∪ 共享 seam 2 件（connection.py 为 P5 共享冻结面——P5 动态装配段回退守卫；K1 prompt 载体承载不变式）∪ 安全余量 2 件（R9 意图/工具零触碰 + 模型名漂移守卫，K2-E5 语义）。图可用性：per-Pod 专属图索引可用，正常圈定、无降级。
- Security gate: PASS（`securityReviewRequired=false`）。注入面人工安全核对：`user` 字段值恒 = 引擎侧 `device_id`（设备标识，非用户可控文本），无注入路径扩大；`user` 缺省/空串回落旧行为（契约 §四向后兼容负锚）；探针失败打 ERROR 不抛、不拖垮建连；零新增依赖。
- Mutation check: PASS（skipped，理由 = taskpackage `mutationSkipReason`：补丁文件改动小而接线性质强，离线 payload 断言 + 连续性探针 + 降级路径覆盖，变异取证 ROI 不成立；同 260907-g-relay-fix engine-patch 先例口径。builder 冻结相位已做 9 项变异自检全抓，双写禁区 / 切片丢史 / 丢 user 注入 / max_tokens 禁带 / marker 门控均能杀）。
- Docs sync: PASS（docsSyncRequired=false，spec 两文件与 docs/contracts/** 零 diff）；四条声明性注记登记于 `docs-delta.md`（281/292 口径 + unit 层口径 + verify-deploy.sh 零改动 + 部署轮人工生效步），不改任何 docs 文件。

### 合并收口前影响面核对（Step 9b · advisory 记账）

detect_changes 已跑（见上）：15 changed files 全部为簿记面（delivery/**）或 changeManifest 登记面（server/** 4 件 + 验收件）。blast radius 映射：impacted 7 条全为 builtins / firmware gifdec usage 边噪声（固件零 diff），无未预期生产符号波及，核对通过。当前工作树与本仓图索引一致（per-Pod 专属索引 `C-AI-KidPal-pod-m2` 在场、detect_changes 与 trace_path 均可用，非降级路径）。

### 集成执行回报

本包未触发 merge-driver 冲突消解路径（单包在途、dependsOn=K1 已合并、P5 已合并部署，无对端包冲突），Pod 分支 rebase/merge 未执行——该节不适用，无实测数字可报。

## Demo 对账行（workstream 包硬性两条之一）

demo 证据 = **host runner 输出**（与 plan.md「demo 证据形态」节一致）：pytest 验收套件（final 圈定 89 passed / builder 全量 292 passed）——K2-E1 五断言（openai.py 补丁在位与两腿签名 / payload 形状携 user / 省略 user 旧行为负锚 / connection 切片路径携 user / `_kidpal_turn_messages` 纯函数零历史）、K2-E2 断言（RAM 副本保留 + 发送路径零重放）、K2-E3 三断言（compose bind-mount / marker 探针门控 / manifest 资产登记）、K2-E4 两断言（两轮追问连续性 loopback 探针 + 省略 user 降级 + 双写形态负例）、K2-E5 三断言（R9 意图/思考档零触碰 + P5 接线零回退 + 申报自检）。与 roadmap 效果段**一致**：host 面 E1–E5 exitCriteria 全部有对应绿断言。**live 观测项（K2-E6：真机跨 WS 新会话仍记得此前讲过的事实 + 持久会话增长/auto-compaction 成本观察）如实标「待 human 部署轮」**——本包 host 面转绿后停 `awaiting-human:live-deploy`，为 launch-card 钉死的停等位，不是缺口。

## 未覆盖面申报（对照 acceptance[] 六条 + 契约 §四 + arch §8.1 风险 14）

| 设计性质 | 自动断言现状 | 现状 | 去向 |
|---|---|---|---|
| K2-E6 live 观测项：真机跨 WS 回忆 + 持久会话增长/auto-compaction 成本 | 无（kind=structural，零自动断言申报） | host 面承载前提已达成（user 键 + 只发本回合 + 探针门控报警锚） | human 部署轮：先 `docker compose up -d` 重建容器（新增 bind-mount 首次生效 = 设备全断人工窗口），再真机新 WS 会话问此前讲过的事实；成本只记观察、不在本包定阈值（arch §8.1 风险 14） |
| `_kidpal_prefix_count` few-shot 悬空补位假设 | minimal: 注记在码（connection.py:1444） | nointent 部署面 few-shot 段本就为空；既有两条示例链恒闭合 | 若未来引入悬空示例链，升级路径 = 改用与引擎同源的补位计数（minimal: 注记已写明） |
| OpenClaw 会话键 `agent:kidpal:openai-user:<device_id>` 形态 | host 面不可见（服务端在 ECS 部署版 bundle 内） | 部署版 bundle 实读已确认 user 能力在场（plan 可行性判据） | live 轮实证；OQ-7 device→peer 映射归 P3/K3（decisionCoverage landsIn 已钉） |
| 引擎 RAM Dialogue 无限增长 | 无（RAM 副本照常 put 记账） | 副本非权威、不重放，增长只占引擎内存 | live 轮与 auto-compaction 一并观察（K2-E6 观察位） |
| 部署轮 engine 断连窗口 | 无（部署面属性，非代码缺陷） | connection.py 重启 = 设备断连重连，已知部署属性；新增 bind-mount 另需容器重建 | 部署轮执行须知（manifest 两条 note + compose 注释三处同文） |
| 契约 §四 省略 user 向后兼容 | K2-E1 负锚断言（None/空串 → 键缺席） | 契约字面零 diff，实现与定案口径一致 | 闭合 |

## Five-axis semantic review（author 五轴，结构闸 PASS/FAIL 外独立执行）

1. **安全**：`user` 字段值恒 = `self.device_id`（设备标识，非用户可控文本），additive 字段无注入面扩大；探针失败走 ERROR 日志不抛、绝不拖垮建连；`_kidpal_turn_messages` 非法下标 ValueError fail-loud（切片错误不会静默发错历史）。无 finding。
2. **契约**：契约 §四定案逐条落——`user` 字段名冻结、真值才入请求体、缺省/None/空串回落旧行为（负锚断言在场）；对 OpenClaw 流式请求禁带 max_tokens（stock 语义维持、仅显式给出才进请求）；thinking 维持关闭（`_apply_thinking_disabled` 钩子原样）；display E1 / B1 / 提醒 / 私有消息帧零改动（设备端零感知）。无 finding。
3. **边界**：切片边界三态覆盖——keep_tail_count==0 仅前缀、非法下标 ValueError、尾段与前缀重叠 ValueError；在途 tool 链保留（keep_tail_count 覆盖本回合 user + assistant(tool_calls) + tool，工具链未持久化到 OpenClaw，截断即失上下文）；`_kidpal_turn_start` 在 `dialogue.put` **之前**记（时序正确——之后追加的本回合消息进尾部）；探针反射失败 False 不抛。judgement-call 一条见 Findings。
4. **可运维性**：第四条启动标记行由 provider 探针**门控**——openai.py bind-mount 丢失时 stock provider 静默吞 user kwarg（持久会话全功能回退为每请求随机 session），无门控 marker 只是假绿报警锚，本实现探针失败打 ERROR 点名 bind-mount 缺失/漂移（ARQ-260907 同族护栏加强版）；manifest 新资产条目 md5/restart=engine + note 三处同文写明 compose up -d 人工生效步。无 finding。
5. **一致性/卫生**：无重复实现（切片唯一装配点 = `_kidpal_turn_messages`，两发送腿共用一个中转变量）；无跨层调用（connection→provider 单向，P5 schedule 段零回退——圈定跑含 P5 冻结件全绿）；无循环依赖；零新增依赖（inspect = 标准库）；无绕过 contracts 的本地类型。无 finding。

## Simplification Debt

`minimal:` 标记 sweep：本包改动面现存 1 处——`connection.py:1444` `_kidpal_prefix_count` docstring 内 `minimal: few-shot 段按其「悬空 tool_calls 补位」规则`（builder 落，nointent 部署面该段为空、升级路径已写明）。仓内其余 2 处先存标记（`server/shim.py`、`front_app.py:1831`）非本包 hunk、零触碰。新验收件与 openai.py 无 `minimal:` 标记。

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| server/deploy/engine-patch/connection.py:1444 | few-shot 补位计数按「示例链恒闭合」假设简化 | 与引擎同源的悬空 tool_calls 补位计数 | 引入悬空示例链时改用同源补位计数（nointent 部署面现状该段为空，不触发） |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | connection.py `_kidpal_prefix_count` | 前缀计数含隐式假设：system 恒至多 1 条、few-shot 示例链恒闭合（无悬空 tool_calls 补位）；与 `get_llm_dialogue_with_memory` 的段序若未来漂移，切片 actual_start 会错位（错位方向 = 误伤历史或漏切，探针/marker 均不报警） | changed-code hunk（connection.py:1444 minimal: 注记）+ 边界启发：部署面 intent_type=nointent 时 few-shot 段为空、system 恒 1 条，现状成立 | no-trigger（minimal: 注记已写升级路径；live 轮若见回忆异常优先核对此假设） |
| NIT | judgement-call | openai.py 两发送腿 | `if user:` 真值判据把非字符串真值（如 dict）也视为携 user 入请求体；connection 侧恒透传 `self.device_id` 字符串，现状不可达 | changed-code hunk（openai.py:143/:193）+ 边界启发：唯一调用方透传字符串 device_id，契约只定义字符串形态 | no-trigger（单调用方受控；若未来引入第二调用方，加 isinstance 防御一行即可） |

无 BLOCKING / SHOULD finding。结构闸（closeout-check 等）结果不复述为 finding；gate 全绿仍完整执行了上述五轴语义 review。

## Residual Risk（decisionCoverage `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目）

taskpackage.json `decisionCoverage` 六条全部 `landsIn` 到 exitCriteria 或上游包（OQ-7 落 K3 包），无 `dropped` 条目——本包无 risk-accepted 残余风险项。

## 制品与账本

- docs-delta：`delivery/taskpackages/261022-m5-k2b-persistent-session/docs-delta.md`（四条声明性注记）
- test-scope 清单：`delivery/taskpackages/261022-m5-k2b-persistent-session/test-scope-final.txt`（Step 4① 圈定，final r2 消费）
- verdict.json（final fact 投影晋升，verify.mjs fs-write）+ verification.json（advisory 投影）：同目录
- 回滚方式：`git revert` 本 commit 即整体回退（单 commit、无数据迁移）；部署面回滚 = 撤 compose openai.py bind-mount 行 + 回滚 connection.py 上一版 + `docker compose up -d`，OpenClaw 侧既有持久会话键不影响旧行为（省略 user = 每请求随机 session，向后兼容）
- Merge packet / supervisor 输入：本 packet + verdict.json + verification.json + verifier-evidence r1/r2 + test-scope-final.txt；交 planner 跑 supervisor-verdict-gate 机械闸（riskTier=high → ESCALATE 停人类合并门为 launch-card 钉死的预期路径），本 agent 不自判通过、不执行 merge
