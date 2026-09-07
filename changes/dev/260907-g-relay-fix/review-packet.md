# Review Packet — 260907-g-relay-fix（G 包 live relay 缺陷修复回包）

> integrator 产出 · 2026-09-07 · 分支 `dev/260907-g-relay-fix` · 供 supervisor 合并门复核。
> 缺陷本体：包 2 review-packet SHOULD #1——front 真实 ws 代理路径零文本接管，```display fence 混入播报、kidpal/display tools/call 从未送达设备、display text 视图（P3 翻屏语义）从未触发（用户「翻不回上一页」根因）。
> 修复形态（human 批准，pod-events decision `engine-patch-approval` 事件 `37469170`）：**引擎为主剥离**（部署副本补丁：`connection.py` LLM 流式累积段单点 fence 状态机 + `kidpal_display_strip.py` 纯模块，经 compose bind-mount 覆盖镜像 `core/`）、**front 为校验/relay 权威**（侧通道截获 → display_schema V27 → `{"type":"mcp","payload":<tools/call>}` 逐帧下发 + 回执 relay_ack 消费）。front 自身 strip 保留为纵深防御。

## Change Size

- Files changed: 实现面 3（`front_app.py` +211/-26、`mcp_relay.py` +17/-2、`docker-compose.yml` +5）+ 补丁部署副本 2（`engine-patch/connection.py`、`engine-patch/kidpal_display_strip.py`）+ 冻结验收 1（`test_g_relay_fix.acceptance.py`）+ 部署 README 1（+26）+ closeout 制品 3（review-packet / docs-delta / live-smoke-evidence）+ taskpackage 面（`.claude/taskpackages/260907-g-relay-fix/` 全套 + `current/` 投影）
- Net diff: server 面约 +470/-30（不含验收冻结件与证据制品）
- Scope: server 子系统单面（front + 部署件）；firmware 零触碰；`docs/contracts/**`、`docs/spec.md` 零改动
- Split recommendation: 不拆——剥离/校验/relay/回执是一条缺陷修复链，拆开任一环都会出现中间态红；Lite 包单 PR 收口

## Verification Evidence

- **Structural gates**: verifier final round 3 九闸全 PASS（fact `ok:true`，`observedGateExit 0`）——`.claude/taskpackages/260907-g-relay-fix/verifier-evidence/rounds/r3.json`（invocationId `c7505229-5c16-426c-b148-42f1dba91382`）。`verifier-evidence-check.mjs` 复核 PASS：`{"factOk":true,"observedGateExit":0,"phase":"final","round":3}`。注：firmware-build 闸降级放行（idf.py 不可用，本包固件零改动，非假绿；如实记录）。
- **Acceptance**: GRF-A1 结构齿（red baseline 9 failed/1 passed/3 live skipped 起步，修复后结构面全绿）+ GRF-E1 既有验收回归守卫 `test_gateway_directionB.acceptance.py` 结构面保持全绿（20 passed / 10 live skipped）。live 面按 ARQ-260901-g3-server-live-acceptance 口径不 auto-run。
- **Live（真机三轮冒烟）**: `changes/dev/260907-g-relay-fix/live-smoke-evidence.md`（勿动）——2026-09-07 三轮真机 ESP32-S3 ⇄ wss front ⇄ 引擎（engine-patch 部署副本）⇄ OpenClaw：① 启动标记行三轮建连三次打印（补丁真在跑）；② 播报面 + 屏幕面 fence 双清零（240s 串口捕获窗零残留，含《偷月亮的狐狸》story 轮逐字例证）；③ kidpal/display tools/call 下发证明链三段（串口零拒收 + 离线 harness 全链 assert + ④ text 视图弹出目击）；④ text 视图触发成立。诚实边界已录：跨页渲染端到端整合归包 9b（`key_gesture_parser.h:42` SES-E2 注记），本轮不虚报已验翻页。
- **Mutation check**: GRF-A2/A3/A4 三处 mutation 均转红且 sha256 还原核验一致；transport-revision 后（二轮冒烟暴露 display_relay 帧被固件拒 → wire 形态改 mcp 帧 + id 数值化）三处全部重跑：A2 4 red（strip 全语义 + pump 连带）、A3 4 red（信封数断言含侧通道 pump）、A4 1 red（失败点精确命中新增 mcp 解包样本）。A5 证据突变（evidence-based 条目标准 mutation）：`live-smoke-evidence.md` 改名 `.bak` 后 live 4 passed → 4 failed（GRF-A5/E2 RED 断言生效）→ 改回复绿，integrator 实测（RUN_LIVE=1 -m live 口径）。
- **Security gate**: PASS（securityReviewRequired=false）——无 secrets 变更；token 鉴权路径未触碰；引擎补丁纯文本处理、无网络面外扩。
- **Docs sync**: PASS（r3.json docs-sync gate；`docsNoChangeReason: null`）——spec/contract 零改动（见 docs-delta.md），`server/deploy/README.md` 补 engine-patch 运维节（补丁文件/bind-mount/启动标记核验/漂移告警语义）。
- **变更影响面核对（detect_changes N/A (worktree)）**: 本 Pod worktree 支线，CodebaseMemory 主索引 checkout 与本工作树不同步，detect_changes 不可用——按规程以 `git diff --stat` + 关键共享符号抽查替代：`git grep` 确认 `relay_ack`/`display_tool_call`/`alert_tool_call` 全部消费方 = 冻结验收 E1 守卫（绿）+ front 委托（语义零改动，`on_device_receipt` 只做 mcp 帧解包后委托）；`_tool_call` 的 id 数值化波及 `alert_tool_call` 缺省分支（payload 无 id 时 fallback 由 uuid hex 变数值 id）——alert_tool_call 携 reminder id 时仍走自身确定性覆写，行为不变；该边界已核 E1 守卫仍绿。图索引对 `server/gateway` 覆盖不足（graph 查询 0 命中，降级 grep 抽查），如实记录。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （changed 面无 `minimal:` 标记，台账为空） | no-trigger | no-trigger | no-trigger |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | hard-violation | deploy | 引擎部署副本代码补丁为**首例**（此前纪律是镜像零补丁、宿主面承载）；human 已批准（pod-events `engine-patch-approval` 事件 `37469170`），但补丁漂移治理依赖 compose bind-mount + 启动标记行人工核验，镜像更新即丢补丁/补丁语义漂移无机械门 | Basis: 治理授权 pod-events 事件 `37469170`；漂移面已登记 `ARQ-260907-engine-deploy-patch-drift`（本批新增） | 无需本批动作；运维面 owner=dev-loop 跟进 ARQ |
| FYI | judgement-call | firmware/docs | 翻页端到端整合不在本包：text 视图触发已实证，BOOT 键跨页翻页（P1→P4 循环）的端到端验证归包 9b | Basis: changed 证据文件 `live-smoke-evidence.md` 诚实边界段 + `firmware/main/key_gesture_parser.h:42` SES-E2 注记（heuristic: 不虚报已验范围） | 包 9b 承接，无本批动作 |
| NIT | judgement-call | operability | 引擎容器 healthcheck 显示 `unhealthy`：镜像内无 `ss`/`netstat`，健康检查命令恒失败——环境怪癖，非本包回归（engine 实际健康，标记行 + 设备可建连可证） | Basis: changed hunk `docker-compose.yml` healthcheck 注释 + 部署实测；未修因为改 healthcheck 探针属部署面既有形状、超出回归修复包 scope（heuristic: change-scope 纪律） | 已在 `server/deploy/README.md` 补节附记；可选后续包换用镜像内存在的探针命令 |
| SHOULD→已登记 | hard-violation | contract | 契约 §1.5「display 切分」职责措辞与实况存在精度差：物理摘取（fence 状态机）实际落在**引擎部署副本**（human 批准的方案 A），契约只写了网关（front）职责——front 是校验/下发唯一权威不变，但「切分」执行位非网关本体 | Basis: `docs/contracts/interface-contract.md#§1.5` vs changed `engine-patch/connection.py` 实况对照；本包不改契约（契约门纪律） | 已登记 `ARQ-260907-contract-1-5-strip-wording`，下次契约门校准措辞（本批不改 `docs/contracts/**`） |
| FYI | judgement-call | brain-leg | live 引擎 `Intent:nointent` 根因（OpenClaw 拒收 tools 数组）未查——本包 relay 由 front 发起、不依赖 LLM tool calling，不受阻；将来 tools 修复回开 Intent 后引擎侧可能自行 tool-call，front relay 需幂等/去重意识 | Basis: 包 2 roundtrip-live-evidence 遗留 + taskpackage notes（实现注记已写入 front_app 相关注释面）；诊断用开源克隆在 `c:/AI/OpenSourceReference/openclaw-main` | 已登记 `ARQ-260907-openclaw-tools-rejection-rootcause`（owner=architect/dev-loop，B 方案重估 Intent 回开成本） |
| FYI | hard-violation | live-baseline | live A4（ASR）等遗留仍挂 `ARQ-260904-v23-asr-swap-quota-recheck`——与本包无关（本包不触 ASR/provider 面），如实引证避免误记为本包残余 | Basis: 既有 ARQ 条目（260903-gateway-directionB 收口登记） | 无本批动作 |

## Five-Axis Review

1. **Correctness**: 五验收齿全覆盖——A1 结构齿（pump 接线计数齿：`pump_upstream_to_client`/`pump_client_to_upstream` 在 ws_gateway 内各被 create_task 调用一次，源计数断言防「定义了不接线」回归）；A2 strip 纯语法语义（canonical + inline opener 两形态、多块、脏体、无 fence 直通）；A3 校验/降级（合法块信封逐块等长、脏块降级友好 text 视图不静默、data 字符串化 wire 形态）；A4 回执闭环（mcp 帧解包后委托冻结 `relay_ack`，等价性抽检齿防分叉实现）；A5 live 四事实。边界条件：流式跨 delta 的部分 backtick 由引擎状态机 pending 承载（front 纵深防御不做跨帧重组，架构注记明确）；未闭合 fence 不入 TTS（宁静默不念 JSON）。波及面：`_tool_call` id 数值化同时改了 alert_tool_call 的 fallback id（E1 守卫仍绿，见 Findings 前 Verify 段）。
2. **Test quality**: 冻结验收断言真实 wire 事实（application.cc:492/:532 帧路由、mcp_server.cc:473 数值 id、:105 双 String 属性）而非自证；red baseline 起步（无 import/语法假红）；mutation 四处含 A5 证据突变；既有验收回归守卫并行保持。
3. **Contract/boundary safety**: `docs/contracts/**` 与 spec 零触碰；决策 23 语义零分叉（front 只消费回执不重发）；§1.5 措辞精度差已登记 ARQ 走契约门，不在 loop 内改。
4. **Operability/security**: 无 secrets 变更；token 鉴权路径未动；补丁文件含启动标记行作为部署可观测锚（README 补核验步骤）；healthcheck unhealthy 怪癖已附记；错误路径（引擎不可达 1011、非 JSON 帧直通、脏块降级提示）均有承载。
5. **Maintainability/simplicity**: 剥离单点化（引擎状态机单实例、TTS delta 与 response_message 共用同一输出流，防两处状态机漂移）；front 侧函数短小、职责单一（strip/build/relay/receipt 四 callable + 双泵）；无新依赖引入；重复实现检查：`on_device_receipt` 明确委托 `relay_ack` 不自造语义（验收有等价性齿）。无投机抽象（deletion test：删掉 strip/pump 任一函数缺陷即回归，复杂度不会转移到调用方）。

## S2 Exploratory Scan

TaskPackage 未声明 `s2ExploratoryRequired`——未执行 S2 official scan，无 S2 evidence、无 knowledgeSuggestion 持久化项。builder/fixer 真机冒烟与 Portal 手动操作按规程不作为 official evidence（live 证据以冻结验收 RUN_LIVE 口径 + `live-smoke-evidence.md` 承载）。

## Residual Risk

- Known limitation:
  - 引擎部署副本补丁为部署面新形态：镜像更新即丢补丁（bind-mount 不随镜像走时静默回退原生 `connection.py`）、镜像升级有补丁语义漂移风险——治理项 `ARQ-260907-engine-deploy-patch-drift`（owner=dev-loop 运维面），短期缓释 = 启动标记行核验纪律 + README 漂移告警语义。
  - front 纵深防御 strip 是单帧正则剥离，不做跨帧重组——架构上由引擎单点剥离承担流式语义（架构注记明确，非缺陷）。
  - OpenClaw tools 数组拒收根因未查（Intent 回开成本待重估）——`ARQ-260907-openclaw-tools-rejection-rootcause`。
  - healthcheck unhealthy 环境怪癖（非本包回归）如 Findings 表。
- Follow-up recommended: 包 9b 承接跨页翻页端到端整合（SES-E2）；reminder-side 包承接 alert_tool_call id 数值化注记；运维面跟进补丁漂移 ARQ。
- Human decision needed: 无本批新增。`decisionCoverage` 三条 decision 全部 `landsIn` acceptance 齿（GRF-A2/A3/A4），无 `landsIn: dropped && reason: risk-accepted` 条目，故本包无 risk-accepted 台账项。live A4（ASR）遗留挂既有 `ARQ-260904-v23`，此前已由 human 记账，非本包引入。

## 合并门前状态

- Review Packet 无 BLOCKING finding——不触发停门。
- 可回滚路径：feature 分支 `dev/260907-g-relay-fix` 单 commit（见 commit hash 记录），未 push、未动 main；回滚 = 分支级 revert/删除，引擎侧回滚 = 移除 compose 两行 bind-mount + 重启容器（标记行消失即回滚核验锚）。
- 等待 supervisor 裁断；本 integrator 不自行判定通过、不执行 merge。
