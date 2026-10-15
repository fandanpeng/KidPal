# Review Packet — 260829-voice-roundtrip

> integrator 收口 · 2026-09-07。合并门前的审查包。本包为**方向 B 语音回环 + display 消费链路**
> （subsystem=firmware+server，riskTier=high，mergeOrder 5，dependsOn=260903-device-ws-switch /
> 260829-host-test-infra 均已 merged）：设备流式语音会话验收 + display mcp 消费端（view 路由
> 共享底座，包 4/5/6/8 扩展面）+ display 格式遵循度统计性复验（契约待确认点 1）+ 网关兜底
> 设备侧消费 + abort 云端取消 + OpenClaw 基础 system prompt 版本化落盘与部署接线（VRT-E4）。

## Change Size

- **Files changed**（working-tree diff，git status 自查）：
  - 固件实现（新增）：`firmware/main/display/view_router.{c,h}`（纯逻辑 C linkage，无芯片依赖）
  - 固件改动：`firmware/main/mcp_server.cc`（kidpal/display 工具注册 + string 参数收对象/数组序列化分支 + cJSON 送达确认回执）、`firmware/main/CMakeLists.txt`、`firmware/main/test/host_app/main/CMakeLists.txt`（host runner 接线）
  - 固件验收（新增，冻结只读）：`firmware/main/test/voice_roundtrip_b.acceptance.c`（9 TEST_CASE：A1/A3×2/A2/A5/A6/A2-E2 evidence/boundary + weak-symbol frozen contract）
  - 服务端（新增）：`server/deploy/openclaw-system-prompt.v1.md`（87 行版本化 prompt）、`server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER`（接线声明件）、`server/acceptance/test_voice_roundtrip_b.acceptance.py`（11 test：offline 8 + live 3）、`server/acceptance/conftest.py`（live 机械 skip 闸）
  - 服务端改动：`server/deploy/apply-xiaozhi-overlay.sh`（真 mount 路径修复 + prompt 渲染注入）、`server/deploy/provider.config.yaml`（system_prompt 声明块）
  - 治理/证据：`.claude/taskpackages/260829-voice-roundtrip/*`、`changes/dev/260829-voice-roundtrip/*`、`.planning/kidpal-mvp/progress.md`（append 收口记）、`firmware/main/test/acceptance/red-baseline/red-baseline-summary.md`
- **Net diff**：tracked 修改 10 文件 +301/−132，另有新增实现/验收/prompt/证据等未入 tracked diff 的新文件。净新增为主（本包是能力新增包，无台阶退役）。
- **Scope**：双子系统。固件只加 display 消费底座与 mcp 工具；服务端只加 prompt 落盘/渲染与验收——不触碰 G 包已 landed 面（fallback.py / display_schema.py / mcp_relay.py 零改动）。
- **Split recommendation**：不拆。验收与实现 1:1（VRT-A1~A7/E1~E4 全落两份冻结验收文件），单包内聚。

## Verification Evidence

### 承重事实（verify.mjs round，唯一 blocking 机器件）

- **final r1（invocation `205433b2-c329-4fcd-8869-2936d2cbe392`）= ok:true / observedGateExit 0**，phase=final、round=1，九闸全绿，2026-09-07。
- 承重事实文件：`.claude/taskpackages/260829-voice-roundtrip/verifier-evidence/rounds/r1.json`（authority=bearing）。
- checker：`verifier-evidence-check.mjs --phase final --round 1` → **PASS（factOk:true，EXIT=0）**。
- 投影·advisory：`.claude/taskpackages/current/verdict.json`（verify.mjs 程序 fs-write，bearing 投影，legacyNonBearing=false）。本包未手写 `changes/dev/verdict.json` / `verification.json`（R11）。
- 演进留痕：verify-runs.jsonl 记录 verifier 两轮 scoped（r1 初轮 mutation-check 红——mutationEvidence 未齐 → 补齐后 7 闸绿）→ integrator final r1 九闸绿。

### 逐闸状态（final r1，诚实口径）

| 闸 | 态 | 要点 |
|---|---|---|
| taskpackage-required | PASS | changeId 一致 |
| firmware-build | PASS·**degraded** | idf.py 本机不可用，固件编译未在本机 re-verify（builder/fixer 侧已真编译 + 真机烧录 Hash verified——roundtrip-live-evidence.md 头注）。非假绿，re-verification 降级放行 |
| contract-lint | PASS | `docs/contracts/interface-contract.md` 零触碰 |
| acceptance-coverage | PASS | VRT-A1~A7 全识别（A1 structural，A2..A7 functional） |
| mutation-check | PASS | 6 条 functional acceptance 各有 mutationEvidence（turnedRed=true，sensitive=true） |
| docs-sync | PASS | docsOwner=integrator；判定见 docs-delta B 段 |
| security | PASS | securityReviewRequired=false；diff 与新文件密钥扫描零命中（见 Five-Axis #4） |
| unit | PASS·executor-skipped | live-only on-target 测试（@pytest.mark.live 手动触发） |
| acceptance | PASS·oracle-pass/executor-skipped | 结构面 oracle（firmware 4 / server 4 验收文件）；on-target 实证由 host ELF + live 承载 |

### Host ELF 验收（闸上 runner 判据）

- 冻结验收经 builder 接线进 host_app linux-target ELF（`HOST_ACCEPTANCE_SRCS` + `voice_roundtrip_b.acceptance.c` + `view_router.c` 强符号覆盖 weak 哨兵）：**32 Tests 0 Failures**（含本包 9 TEST_CASE + 包 0/2/3 既有测试；mutation 变异还原后回绿 32T0F）。
- 红→绿演进：acceptance-author 红基线 8 Tests 5 Failures exit=5（C 侧 stub runner 同文件实测，BUILD_CLEAN）→ builder 落地后 weak 被覆盖、行为真执行 → 绿。python 侧红基线 5 failed 3 passed 3 skipped → 落地后 offline 全绿 + live 齿由证据文件满足。

### live 实证（2026-09-07 真机，planner 采集）

- **会话一 fa104e0c（流式回环多轮，VRT-A2/E2）**：五事实全命中（wss 会话 / ASR 识别回传 / TTS 语音生成 / 句级下发 / 文字同步上屏 state-speaking），状态机 listening↔speaking 全程正确轮转，4 轮往返正常。
- **会话二 4aebae34（abort，VRT-A6）**：播报中 KEY 打断即时停播（本地主地位）+ 引擎同秒收到 abort 消息 + 该轮后续零 TTS 产出（云端取消生成）。
- **display round 95ccf407（第三轮）**：引擎日志证 prompt live 生效（`prompt成功 # KidPal OpenClaw 基础 System Prompt — v1`）；月亮盈亏问答回复含口语 + ```display text view，设备收到全文并全屏显示。
- 证据文件：`changes/dev/260829-voice-roundtrip/roundtrip-live-evidence.md`（五事实逐字摘录 + KEY 裁定 + display 消费链路诚实边界 + 环境摩擦留痕）。`server/acceptance/_artifacts/**` gitignore 本地-only，未 force-add。

### A4 遵循度复验（契约待确认点 1，统计性口径）

- 冻结 harness 对真 OpenClaw 8 样本（`RUN_LIVE=1`，OPENCLAW_TOKEN 经 ECS 配置取用不落 transcript）：**8/8 PASS**（fence + view=text 白名单内），不复用 HTTP 期 12/12 旧结论。
- 诚实处置：首跑 0/8 全空回复 = loopback 401 探针通路问题，**非遵循度结论**，样本作废重跑（planner 观察补记入 a4-adherence-samples.md）。
- 失败模式登记：sample 1 口语部分实测 4 句 > 三句纪律——统计口径（fence+view）判 PASS，超限观察按失败模式登记（处理 = 网关 V32 截断/分屏 G 已 landed + 包 4 prompt 增量可收紧），非 gate 项（契约待确认点 1 = 失败包内 resolve）。
- 证据文件：`changes/dev/260829-voice-roundtrip/a4-adherence-samples.md`。

### Mutation check

6 条 functional acceptance（VRT-A2..A7）各有 mutationEvidence、turnedRed=true、sensitive=true，还原 sha256 与基线一致：A2/E2 证据三态（五组标记全量移除→红）、A3 text 路由错置 HOME（行为齿真执行双红）、A4 处置行移除→红、A5 UNKNOWN 降级改直通（降级混流即红）、A6 abort 调用点删除（计数 6→2 过阈值）、A7 prompt 三句行删除→红。粒度局限如实记录（A2 单点移除不红、A6 注释式探针不红——齿粒度见 taskpackage notes）。操作勘误留痕：A3 首次误用设备 target 陈旧 ELF 假绿，改走 host_app 独立工程后正确转红。

## 架构 / 依赖卫生

- **共享底座 additive 设计**：`view_router` 为 7 view 注册表纯函数（枚举数值码冻结于验收文件头），包 4/5/6/8 扩展 = 注册表加行，不改核心；`mcp_server.cc` 消费端仅调用三契约函数，无跨包耦合。
- **调用面核查**：`kidpal_display_*` 三符号全仓调用点仅 `mcp_server.cc`（生产）+ 冻结验收（测试）——图工具 trace_path 对 C linkage 跨文件调用解析为 0（工具局限，如实记录），改以 Grep 全仓核查；本包 worktree 图索引（`C-AI-KidPal-pod-voice-roundtrip`）已含新文件。
- **G 包零破坏**：`fallback.py` / `display_schema.py` / `mcp_relay.py` 零改动；G 包 pytest 基线 30 collected 零破坏；新增 conftest 的 live skip 与 G 包 live 验收 docstring 语义一致（显式 `-m live` 不受影响）。
- **依赖**：零新增第三方依赖（固件用既有 cJSON，服务端用既有 pytest/os）。
- **回滚路径**：分支 `dev/260829-voice-roundtrip` 单点 commit，revert 即整体回退；合并走 `merge-driver --pod`（--no-ff 语义），合并前前置升级闸 + supervisor-verdict-gate 机械闸。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| —（无新 `minimal:` 标记） | — | — | no-trigger |

> 本包 diff 内无新增 `minimal:` 标记。复用原生面：mcp 工具走小智原生 AddTool/DoToolCall 机制（仅加 string 参数序列化分支）、文字渲染复用原生 SetChatMessage、显示分页为纯函数不建新抽象层；prompt 版本化用文件后缀而非发明版本管理机制。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | 集成/G 包 live 面 | display 端到端 relay 缺口：引擎 display 切分未剥离 fence 即随文字同步下发（设备串口/引擎日志可见 ` ```display ` 字样混入播报文本），且未见 kidpal/display tools/call 中转下发——用户所见全文来自文字同步面（SetChatMessage）而非 display text 视图面，P3 翻屏语义未触发（用户报告「上一页看不到」即此因） | changed-code 之外的真机日志证据（roundtrip-live-evidence.md「新发现（G 包 live 面缺陷）」段，session 95ccf407）+ 本包齿只覆盖设备消费面（C host 实测 + 工具注册 live 实证），relay 归 G 包（260903-gateway-directionB）live 集成 | G 包 live 集成修复（fence 剥离 + mcp_relay 中转下发）；建议登记 ARQ 留根，planner 回包处置 |
| SHOULD | judgement-call | 生成端纪律 | A4 sample 1 口语部分 4 句 > F4 三句纪律（其余 7 样本达标）——生成端约束偶发漂移 | a4-adherence-samples.md planner 观察 + taskpackage VRT-A7（「超限按有处理的失败模式」） | 非 gate（契约待确认点 1 = 失败包内 resolve）：机械兜底 V32 截断/分屏已 landed；包 4 system prompt 增量扩展时收紧措辞 |
| FYI | judgement-call | 齿粒度 | mutation 齿粒度局限：A2/E2 证据齿为文件级五组标记搜索（单点移除不红）；A6 计数齿对注释式探针不红（宽容单点缺失，升级删除式才红） | taskpackage mutationEvidence notes（诚实注记 + 操作勘误留痕） | 无代码动作；后继包设计证据齿时可参考收窄粒度 |
| FYI | judgement-call | 过程/环境 | 环境摩擦两则：① `firmware/sdkconfig` 为 gitignore 机器本地文件，launch 镜像过期副本致首烧跑错板型（friction fb2c70c9，已取 WS worktree 修正件重建重烧）；② a4 首跑 loopback 401 作废重跑（planner 观察已补记） | roundtrip-live-evidence.md 环境摩擦留痕段 + a4-adherence-samples.md | 无代码动作；launch 面 sdkconfig 陷阱已被 ops skill 记载（kidpal-ecs-gateway / kidpal-idf-env），后续 Pod 注意取活 worktree 副本 |

> **无 BLOCKING finding。** 结构闸证据不重复包装为 finding（code-review-gate 约定）；gate 全绿仍完整执行五轴语义 review（下）。Finding #1 为跨包留根升级项，不阻断本包（本包验收范围 = 设备侧消费面，已由 host 齿 + live 工具注册实证覆盖）。

## Five-Axis Review

1. **Correctness**：VRT-A1~A7 语义逐条落地——消费面装配结构（A1：路由模块在设备树 + CMakeLists 引用 + 会话面调用点 + 原生 mcp 分支守卫）、view 路由行为 + text 全文分页（A3：7 view 白名单 + UNKNOWN 降级 + pages×lines≥行数全文不丢）、流式回环设备面（A2：原生 stt/tts 状态机缝 + live 五事实）、兜底消费（A5：UNKNOWN 独立降级绝不盲渲染 + 错误兜底 text 视图当轮结束）、abort（A6：4 调用点 + live 云端取消实证）、A4 统计复验 8/8、A7 问答链路 live 探针绿。host 32/0 与冻结判定逐条吻合。
2. **Test quality**：weak-symbol frozen contract 设计让红基线红在真实缺失行为（「frozen contract NOT implemented」哨兵断言）而非链接/编译错误；行为齿真执行（变异 text 路由→双红）；证据齿三态（缺即红/满足形状即绿/破坏载体即红）；齿粒度局限如实记录不粉饰。live 断言读逐字摘录的真实日志证据。
3. **Contract/boundary safety**：契约面零触碰（contract-lint PASS）；view/control 数值码与语义以冻结验收文件头为准，实现与消费端不得偏移（头注钉死）；boundary 齿守卫「网络出小差了」兜底文案不硬入固件（生成归 G）、设备无 openclaw 直连引用；spec §4.0/§4.4 消费侧归属与 G 生成侧切分在验收断言中落实。
4. **Operability/security**：kidpal/display 注册于设备 MCP server——工具为显示渲染 + 白名单 control 动作（end_session/stop_speaking/start_recording 均映射到既有原生 Application 方法），无敏感操作面；system prompt 为公开声明内容（儿童话术 + 格式约定，无密钥无内部信息）；密钥零入库——OPENCLAW_TOKEN 走环境变量（`os.environ.get`），secrets 在 gitignore/ECS 侧，全 diff 与新文件密钥扫描零命中；apply-xiaozhi-overlay.sh prompt 注入幂等（grep prompt 键跳过），真 mount 路径修复消除了「渲染到陈旧诱饵路径」的 config trap。
5. **Maintainability/simplicity**：纯逻辑底座单文件（view_router.c 103 行）+ 注册表模式；mcp 消费端复用原生工具机制；prompt 版本化 = 文件后缀 + 声明件，无投机抽象；增量扩展区为包 4/7 预留而不预实现。

## Residual Risk

- **Known limitation**：
  - display 端到端 relay 缺口（finding #1）——live 链路中显示内容经文字同步面到达，display text 视图面（含翻屏）未被端到端 exercised；设备消费面本身已 host 实测。
  - firmware-build 闸 degraded（idf.py 本机不可用，未本机 re-verify；builder/fixer 侧真编译 + 真机烧录 Hash verified）。
  - 引擎 Intent:nointent（OpenClaw 拒 tools 数组）——意图驱动的语音触发 end_session 无法 live 验证（intake 裁定：fence 路径不依赖 tools，A3/A4 不受影响；fail-closed 评估后未卡死任何断言）。
  - 兜底 live 面（安抚句 4s / 错误兜底文案）以冻结 harness offline 对齐（a5 齿）承载，未单独 live 触发真实故障注入。
- **Follow-up recommended**：
  - G 包 live relay 修复 + ARQ 留根（finding #1）。
  - 包 4 prompt 增量时收紧三句纪律措辞（finding #2）。
  - 后继包设计证据齿时参考本包齿粒度教训（finding #3）。
- **Human decision needed**：无。本包无待人类拍板项；finding #1 归 G 包处置路径由 planner/主线程安排。

## decisionCoverage 残险（landsIn=dropped / risk-accepted）

- 本包 `decisionCoverage` 五条（PROPOSAL-v0.4-§2→VRT-A2、contract-§一-E1→VRT-A3、contract-待确认点1→VRT-A4、spec-§4.0→VRT-A5、spec-§4.4→VRT-A7）均 `landsIn: acceptance:*`，**无 `dropped` / `risk-accepted` 条目** → 无此列表。

## 未测面与特殊放行声明

- **live 三齿按 ARQ-260901-g3 口径不 auto-run**（conftest 机械闸）：A4/A7 live 已由 planner 冻结 harness 显式执行并留证据；live SSH 可达齿为环境前置。显式 `RUN_LIVE=1` / `-m live` 可复跑。
- **firmware-build 降级放行**：非假绿，本机工具链缺失的 re-verification 降级（builder 侧真编译 + 烧录实证）。
- **本包如实载明**：final r1 九闸 ok:true、host 32/0、mutation 6/6 变红还原、live 两会话一 display round、A4 8/8。**不粉饰为「display 端到端全通」——relay 缺口已如实留根（finding #1），设备消费底座（VRT-E3）与 prompt 承载（VRT-E4）是本包交付物。**
