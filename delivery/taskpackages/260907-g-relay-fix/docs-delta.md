# docs-delta — 260907-g-relay-fix（G 包 live relay 缺陷修复回包）

> 性质：回归修复批（server 子系统，Lite rigor，milestone-affecting，mergeOrder 15 占位）。本包对 canonical docs 的影响。
> 核心结论：**spec/contract 零改动**——本包是纯实现修复（契约 §1.5「display 切分与机械校验是网关职责」在 live ws 代理路径的履职 + 契约 §三/决策 23 mcp 载体语义的 relay 落地），只消费不修改 canonical；`docs/spec.md` 与 `docs/contracts/**` 在本次 closeout 中零写（git diff 确认）。
> 唯一文档落点 = `server/deploy/README.md` 补「引擎部署副本补丁」一节（deployment-facing 指引，见 §C）。verifier 终审 r3.json docs-sync 闸 PASS。

## A. docRefs 现状与无需更新理由

| docRef | 现状 | 本包影响 | 无需更新理由 |
|---|---|---|---|
| `docs/contracts/interface-contract.md#§1.5` | FROZEN：网关职责收敛——display 切分与机械校验是网关（front）职责 | 只读引用，本包即履职 | 契约措辞已覆盖本包行为（live 缺陷是「职责声明了但 ws 代理路径未履职」的实现缺口，非契约缺口）。措辞精度问题（物理摘取实际落在引擎部署副本、校验/下发唯一权威仍在网关）单独登记 `ARQ-260907-contract-1-5-strip-wording`，留下次契约门，本包不改契约 |
| `docs/contracts/interface-contract.md#§三` | FROZEN：决策 23——display 统一走 mcp 工具调用（kidpal/display），设备 JSON-RPC 回执 = 送达确认 | 只读引用 | 本包 relay 链（`build_display_tool_calls` 信封、`on_device_receipt` 回执消费）严格按决策 23 语义实现，`relay_ack` 本体零改动；零契约文本变更 |
| `changes/dev/260829-voice-roundtrip/review-packet.md` | 已归档证据（SHOULD #1 即本包缺陷本体出处） | 只读溯源引用 | 历史证据文件不回写；本包修复结果由本包 closeout 制品（review-packet + live-smoke-evidence）承载 |

**无需更新总述**：本包没有改变任何 canonical 语义。display 的机械校验/relay/回执语义在上游契约与包 2 评审裁决（V27、决策 23）中均已定稿，本包是把「已声明的网关职责」在真实 ws 代理路径上实现并真机实证，不反过来改文档。冻结验收 `server/acceptance/test_g_relay_fix.acceptance.py`（冻结件，不改）与既有 `test_gateway_directionB.acceptance.py`（GRF-E1 回归守卫，结构面保持全绿）共同担保行为与契约语义一致。

## B. 本包落地（docs-sync 相关，非 canonical 文档）

| 目标 | 位置 | 说明 |
|---|---|---|
| `server/gateway/front/front_app.py` | server/ | ws 代理路径文本接管（缺陷本体修复）：`strip_display_fences`（front 纵深防御二次剥离）、`build_display_tool_calls`（display_schema V27 校验 + 降级不静默）、`pump_upstream_to_client` / `pump_client_to_upstream`（双泵接管 + 侧通道截获）、`on_device_receipt`（回执解包委托 `relay_ack`，本体语义不动） |
| `server/gateway/mcp_relay.py` | server/ | `id` 数值化（固件 `mcp_server.cc:473` cJSON_IsNumber wire 事实）：`_next_numeric_id`（ms 时间戳 + 进程内单调 tag）；`alert_tool_call` 保留自身确定性 reminder-id 覆写不动 |
| `server/deploy/engine-patch/*` | server/ | 引擎部署副本补丁（human 批准首例，pod-events `engine-patch-approval` 事件 `37469170`）：`connection.py`（0.9.6 部署副本，LLM 流式累积段单点剥离 + 启动标记行）+ `kidpal_display_strip.py`（纯 Python fence 状态机） |
| `server/deploy/docker-compose.yml` | server/ | 补丁 bind-mount 两行（宿主 `engine-patch/` → 容器 `core/`） |
| `server/acceptance/test_g_relay_fix.acceptance.py` | server/ | 冻结验收（acceptance-author 产出，本包执行不改）：GRF-A1–A5 齿 + TRANSPORT OF RECORD wire 事实钉死 |
| `server/deploy/README.md` | server/ | 新增「引擎部署副本补丁」一节（见 §C）+ 拓扑图/引擎腿描述两处「零补丁」措辞更新 |
| `changes/dev/260907-g-relay-fix/*` | changes/dev/ | closeout/audit 制品：本 docs-delta + review-packet + live-smoke-evidence.md（三轮真机冒烟证据，acceptance 相位已落地） |

## C. deployment-facing 文档指引（server/deploy/README.md 补节）

为部署人补齐引擎部署副本补丁的运维指引，新增「引擎部署副本补丁（engine-patch · 260907-g-relay-fix）」一节，覆盖：

1. **补丁文件与 bind-mount**：`connection.py` / `kidpal_display_strip.py` 两部署副本的宿主→容器落点表 + 职责说明（引擎主剥离 / front 校验-relay 权威）。
2. **核验方式**：启动标记行 `kidpal display-strip patch active`（每次设备建连打一行，`docker logs … | grep` 即核）+ `docker inspect` 核对 bind-mount 在位。
3. **漂移告警语义**：标记行缺失 = 补丁丢失（镜像更新/重建后卷映射或部署副本不在位 → 静默回退原生 `connection.py`，等价 live 缺陷回归）；镜像升级 = 补丁语义漂移风险（部署副本 0.9.6 基线 vs 新版镜像 `core/` 上游演进），治理项 `ARQ-260907-engine-deploy-patch-drift`。附记 healthcheck `unhealthy` 环境怪癖（镜像内无 `ss`/`netstat`，命令恒失败，非补丁引入）。
