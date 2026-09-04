# Review Packet — 260903-gateway-directionB

> integrator 收口。合并门前审查包。任务包 = 方向 B 重切后首包、服务端共享底座（subsystem=server，
> riskTier=high，mergeOrder 3，milestone-affecting）。目的：把 spec v0.2 FROZEN 的方向 B 协议剖面在服务端
> 实现 + 部署期实证出来，作为后面 WS/固件/服务端扩展包的通行证。

## Change Size

- **Files changed**（本包 + closeout，全部相对 feature 分支 base）：
  - 实现面 `server/gateway/*`：`auth.py` `display_schema.py` `provider.py` `fallback.py` `mcp_relay.py` `log.py` + `front/front_app.py`（TLS front，装配 5 模块）
  - 部署件 `server/deploy/*`：`provider.config.yaml` `apply-xiaozhi-overlay.sh` `docker-compose.yml` `kidpal-front.service` `xiaozhi-container.service` `xiaozhi-gateway.service` `cert-renew.sh` `cert-warn-expiry.sh` `secrets.env.example` **`README.md`（新增，deployment-facing 指引）**
  - 验收 `server/acceptance/test_gateway_directionB.acceptance.py`（30 测试 = 20 结构面非 live + 10 `@pytest.mark.live`）
  - 证据暂存 `server/evidence/260903-gateway-directionB/*`（4 件，同晋升 changes/dev）
  - `changes/dev/260903-gateway-directionB/*`：docs-delta + review-packet + 4 件证据（v21/v23/deploy/cert-lifecycle-accept-risk）
  - `docs/ARCH-REVIEW-QUEUE.md`（3 处改动：1 迁 watchlist + 2 新增 deferred）
  - `.planning/kidpal-mvp/progress.md`（closeout 一线追加）
  - `.claude/taskpackages/**`（taskpackage.json + pod-events + verify-runs + verifier-evidence rounds + dispatch-log + current 投影）
  - 闸热修 `.claude/{hooks,scripts}/*`（8 文件 CHANGE_ID_PREFIX_RE `[a-z0-9-]`→`[a-zA-Z0-9-]`，human 批准一次性，见治理）
- **Net diff**：纯新增 + 2 行每文件的正则热修；零改动既有产品代码（server/shim.py 与既有验收不动）。
- **Scope**：单一子系统（server）服务端网关。行为验收 = 冻结验收文件承载 + ECS live（@pytest.mark.live 不 auto-run）。
- **Split recommendation**：不拆。单包内聚，验收与实现 1:1；mergeOrder 3 先并减 7/8 服务端扩展冲突面。

## Verification Evidence

### Bearing machine evidence（closeout 判断来源）

- **final verifier-round fact**：`.claude/taskpackages/260903-gateway-directionB/verifier-evidence/rounds/r4.json`（`schemaVersion:1.0`, `authority:bearing`, `phase:final`, `round:4`, `ok:true`, `observedGateExit:0`, invocation `d5da40d1-28fa-4337-b9e7-00c8f7336447`）。
- **`verifier-evidence-check.mjs` 复验**（integrator，本收口）：`{"ok":true,"gate":"verifier-evidence","factOk":true,"observedGateExit":0,"phase":"final","round":4}`。fact `ok:true` → closeout 判断成立。
- **九闸全绿**（r4.json results）：taskpackage-required / firmware-build（**degraded=true**，`idf.py 不可用`：固件编译未在本机核验，降级放行非假绿，本包纯 server）/ contract-lint / acceptance-coverage / mutation-check / docs-sync / security / unit（executor-skipped=live-only on-target）/ acceptance（oracle-pass + executor-skipped=live-only）。
- **current/verdict.json**：non-bearing 投影（producedBy `verify.mjs (script fs-write)`，ts=final round 完成时点），closeout 判断不以它为准；`verification.json` 未产出（advisory-only，不阻塞，见 closeout-check 输出）。

### 结构面（20/20）

验收文件结构面非 live 测试 **20/20 绿**（`--collect-only` 30 = 20 结构面 + 10 `@pytest.mark.live` deselected 不 auto-run）。redBaseline（2026-09-04）"20 failed / 10 deselected" → 终审时 20 failed 全转绿，覆盖：a2 部署承载、a3 鉴权（auth.py 强制 token）、a4 provider 声明（禁 max_tokens）、a5 display_schema、a6 mcp_relay、a7 fallback、a8 探针证据落盘、a9 证书脚本、e2 部署证据、e3 cert accept-as-risk + 18789 loopback、e4 provider 可插拔。

### Mutation check（9 条 GWA 敏感性）

mutation-check 闸 PASS，r4 列出 9 条 GWA 全量敏感性（taskpackage.mutationEvidence）。**GWA-A2 sensitivity 偏弱（如实标注）**：presence 子串检查（wss 与 8000 carrier，prose 注释也命中）——真牙齿在 live `test_a2_live_wss_port_reachable`（需公网 8000 绑定后由 verifier 复核）。其余 8 条点即红（auth 短路 / max_tokens 注入 / view 白名单清空 / relay_ack 禁用 / 安抚 4s 阈值打破 / 证据改名 / 证书 10y literal 删除）——teeth 精确。11 文件 sha256 还原核销全绿。

### ECS live 实测（deploy.md + pod-events）

已完成 live 实证（2026-09-04/05，loopback 面）——见 `changes/dev/260903-gateway-directionB/deploy.md` 实测矩阵：
- **TLS front loopback 鉴权三连**：无 token ws 握手 → 400、错 token → 400、带真 token 空文本 `/command`（TLS）→ 400；
- **证书 20 年**：`test_a9_live_cert_validity ≥10y` PASS（notAfter 2046-08-30）；
- **18789 仅 loopback**：`test_e3_live_18789_bound_loopback` PASS（ss 仅 `127.0.0.1:18789`，V28）；
- **V21 多轮探针**：stateless 不续 / 每轮透传历史续 → 承载方案「服务端每轮透传历史」定案；
- **OpenClaw loopback 在线**：`test_a4_live_openclaw_loopback` PASS（401=在线，`/v1/models`）。
未被 live 实际覆盖的 exhaustiveness 以 residual risk 如实列出（公网 8000、ASR/TTS 配额、引擎 ASR provider 换型）。

### Security gate

security 闸 PASS（`securityReviewRequired:false`——本包声明的 gateProfile 含 security，但机器闸按配未触发强 security review 路径，由本 packet 五轴第 4 轴人工复核）。证书/密钥零入仓：grep 复查 `sk-sp-`/OPENCLAW 真值零命中，dev token 仅 ECS `secrets.env`(600)。

## Simplification Debt

（预检 `minimal:` 标记与五轴第 5 轴「过度设计」：无 `delete`/`stdlib`/`native`/`yagni`/`shrink` 命中——gateway 各模块为冻结模块契约要求的纯 Python 独立模块，`front_app.py` 装配 5 模块属组合而非重复，deletion test 判「删 front 装配则复杂度泄漏到部署层/systemd 编排」= 深模组留。）

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| `server/gateway/provider.py` ASR 族 | 按可声明形状注册候选（aliyun_stream/aliyun_nonsream/fun_local→sensevoice），live ASR 凭据缺口下不全量实装 | 全部 ASR 实测 + 流式/VAD 决策 | 阿里云 ASR 凭据开通后补 live 实测（V23 配额重置 2026-09-07） |
| `server/deploy/apply-xiaozhi-overlay.sh` | 只渲染 `.config.yaml`（provider overlay），不承载引擎 boot 依赖闭包 | 官方容器已解 boot 闭包 | （container 拓扑已定型） |

> 无其它简化债务；两行为具唯一 upgrade trigger（配额/凭据），标记 `no-trigger` 项为零。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | 可运维性 | 引擎 boot 的 ASR provider 仍指 `aliyun_stream`（overlay 默认）→ 无凭据时引擎 init crash-loop，未 boot 至 streaming 可用 | changed-code hunk：`server/deploy/docker-compose.yml` / deploy.md 实测「ASR provider 初始化即失败」；容器健康检查只探 ws:8000 端口不探 provider 就绪 | 配额/凭据落实后随 V23 换型核对（见 residual risk ③），非阻塞 |
| FYI | judgement-call | 测试质量 | `test_live_blank_text_rejected_400` 冻结断言用 plain `http://` 读 8000（TLS front 实为 wss 400 语义）；同一 8000 单监听无法同时 plain-HTTP + TLS | changed-code hunk：验收文件 blank_text 探针 vs deploy.md「设备腿 8000 = wss(TLS)」语法 | 非阻断；接受者/planner 决定 `/command` 是否改到引擎 plain-http 8003 或验收对齐，本包 front 已按 TLS 实现并验证 400 正确 |
| FYI | judgement-call | 可运维性 | live 验收 A4 路径错位：`test_a4_live_provider_config_declared_on_ecs` 读 `${GATEWAY_DEPLOY_DIR}/config.yaml`，真实 config 在 `<deploy>/main/xiaozhi-server/config.yaml`（容器内 `/opt/.../data`） | changed-code hunk：验收文件 env 读取 + deploy.md「推荐取值」 | 非阻断；建议跑 live 时置 `GATEWAY_DEPLOY_DIR=/opt/kidpal/xiaozhi-esp32-server/main/xiaozhi-server`（已写进 server/deploy/README.md），不改冻结测试 |

> 无 BLOCKING / SHOULD finding。三条均为实现已就位、待外因（凭据/配额/human 拍板）收口的已知受限项，已进 Residual Risk 或 deploy 指引，不进合并门前退回。不把 gate 结果（r4 九闸 PASS）复述为 finding（约定级约束）。

## Five-Axis Review

1. **Correctness**：结构面 20/20 逐条对 GWA-A1~A9 + E1~E4（acceptance-coverage 闸验证全部 functional acceptance 有 coverage marker + mutation 红）。auth 强制 token（未设=拒绝服务，机械短路非仅条件分支，历史教训 ARQ-260902-shim-401-gate-bypass）；display_schema view 白名单 + data shape/大小上限 + 脏 JSON 降级；fallback 4s 安抚 / 错误兜底 / V32 截断；mcp_relay kidpal/display+alert JSON-RPC 回执。live 三连（无/错/空 token）在 TLS front 面实证。
2. **Test quality**：验收断言真实行为（10 live 条目不 auto-run、演进文件 `--import-mode=importlib` 显式；20 结构面各断言 HTTP 状态/存在性/脚本输出，非「没报错」，V8 净化后不再直读供应商路径）。mutation 9 条真 teeth（8 条单点即红），A2 敏感性如实标注偏弱。
3. **Contract/boundary safety**：契约面零触碰（`docs/contracts/**` 只读引用 §四/§1.5）。`openclaw_stream_request` 严格禁带 max_tokens（契约 §四 V9 硬约束）；display 白名单 view 集合与契约对齐。frozen 验收文件 `*.acceptance.*` 未改。
4. **Operability/security**：log.py 结构化日志带 correlationId（K6）；secret 只经 ECS secrets.env(600) 注入，不入镜像层/仓库（grep 零泄漏）；18789 锁 loopback；证书 20y + 轮换 + 到期告警；`0.0.0.0:8000` 公网绑定**未执行**（需 human 本人授权，系统权限闸要求）——部署已验证 loopback 面，公网暴露为显式待 consent 项。
5. **Maintainability/simplicity**：5 个 gateway 纯 Python 模块各自单一职责（冻结模块契约：import 不依赖 fastapi/edge_tts/httpx，可独立单测）；front 装配为组合非重复；`docker-compose` 零引擎补丁把 boot 依赖闭包交官方镜像；无过早抽象（provider registry 是决策 18/19 明确要求的可插拔底座）。deletion test：删任一模块复杂度即外泄到调用方或部署层 → 深模组，留。

## Residual Risk

> decisionCoverage 残险（`landsIn=dropped` 且 `reason` 以 `risk-accepted` 开头）——见文末专项段（本包 decisionCoverage 无 dropped 条目；cert accept-as-risk 为 ARQ 记账非 decisionCoverage dropped）。

- **Known limitation**：
  - **live 面待办（human 外因，不阻塞结构面/合并门，阻塞的是完整 live 实证）**：
    ① **公网 `0.0.0.0:8000` 绑定待 human 本人一句话授权**（builder/coordinator 转述的 consent 不被视为用户授权；系统权限闸要求）——授权后 verifier 在 `8.137.91.16:8000` 复核 `test_a2_live_wss_port_reachable` + 鉴权三连。
    ② **ASR/TTS 实测待 token-plan 配额重置**（2026-09-07 14:19 UTC；key 已认证通过、端点已锁定 `token-plan.cn-beijing.maas.aliyuncs.com/compatible-mode/v1`，当前 429 `insufficient_quota` 阻断）——重置后补 live 实测（转写正确性/延迟/流式/VAD 切句）。
    ③ **引擎 ASR provider 换型待 human 拍板**：当前 overlay `aliyun_stream` 无凭据 crash-loop；可行换型 = token-plan Qwen via `OpenaiASR`（`type:openai`，改 `base_url` 指向 token-plan `/audio/transcriptions` 即可不改引擎，需验路径存在） vs `Qwen3ASRFlash`（SDK 直连，无 base_url 覆写需引擎改动）。决定权在 human（V23 报告只陈述可行性）。
    ④ **V23 VAD 切句 / 流式兼容性待配额后实测**（AliyunStreamASR `max_sentence_silence` 与本地 AFE VAD 两级不冲突；Qwen3-ASR-Flash 非经典逐句，若引入需网关侧补流式切句累积）。
  - 引擎 boot 未达「active/running 服务」全态（依赖闭包由容器解、但 ASR provider 就绪受凭据门），deploy.md 如实诚实记录、不冒充。
- **Follow-up recommended**：
  - ARQ-260902-cert-lifecycle-deadlock 迁移 watchlist（OTA 建立或证书 <5 年时复议）；本包证书 20y + 轮换 + 告警已就位。
  - gate-regex 热修后审定是否将「闸脚本须接受混合大小写 changeId」固化为永久规则（ARQ-260904-gate-regex-uppercase-changeid）。
  - V23 ASR 换型 + 配额重置后补验（ARQ-260904-v23-asr-swap-quota-recheck）。
  - live A4 路径错位与环境变量指引已写进 server/deploy/README.md，供后续 live 验收复用。
- **Human decision needed（全部为 live 待办而非结构面 blocking）**：
  - 公网 8000 绑定授权（同步安全面收敛确认）；
  - 阿里云 ASR 凭据开通 或 token-plan 引擎 provider 换型拍板（③）；
  - token-plan 配额重置后补 live 实测（②④）。

## S2 探索 / UI design-review / 其他

- **S2 探索**：TaskPackage 未声明 `s2ExploratoryRequired`，按指令跳过，不产生 S2 official scan evidence。
- **UI design-review**：本包纯 server 网关，无 UI 工件（UIUX 设计稿/渲染均在固件/设计包），viewport/a11y/design-drift 不适用——记录为「无 UI 面，N/A」。
- **S2 knowledge decisions**：不适用（无 scan）。

## 治理事项与诚实记录

- **gate-regex 热修（human 批准一次性）**：8 个闸脚本 `CHANGE_ID_PREFIX_RE` 原 `[a-z0-9-]` 不认大写字母，`260903-gateway-directionB`（大写 B）被误判 pre-cutoff（dispatch-stamp 首次落错通道）→ human 2026-09-04 AskUserQuestion 批准一次性热修 `[a-z0-9-]`→`[a-zA-Z0-9-]`，跳过 architect 治理批、如实记录（pod-events `human-decision gate-regex-hotfix`，eventId `e9b8c34c-4339-4591-8a7f-c67b204978e4`；agent-escalation→resolved human-approved-hotfix）。已落 8 文件 + ARQ 登记（见下）。
- **A2 结构面敏感性偏弱（如实标注）**：presence 子串检查，prose 注释也命中；真实牙齿在 live wss 可达，mutationEvidence 已如实标注 notes，未虚报 teeth 强度。
- **机器产线 shape**：本包走了完整 verify.mjs round 产线（r1~r4）→ 本文档（closeout）为 blocking 判断 prose，`verdict.json`（current 投影）non-bearing；不产 session-digest/loop-health/loop-metrics/pod-journal（post-EVENTS_SCHEMA_CUTOFF 退役），pod-events.jsonl 存在（closeout 作 advisory 检查通过）。

## decisionCoverage 残险（landsIn=dropped / risk-accepted）

- 无 `landsIn=dropped` 条目：本包 `taskpackage.json.decisionCoverage` 九条全部 `landsIn` 到 `acceptance:GWA-A2~A9`（决策18→A2、决策22→A3、决策19→A4、contract§四→A4、handoff V27→A5、决策23→A6、spec§4.0→A7、handoff V21→A8、handoff V25→A9），无 dropped / risk-accepted。
- 注：cert 到期死局（ARQ-260902-cert-lifecycle-deadlock）的 accept-as-risk 记账是 **ARQ watchlist 层面**，非 decisionCoverage dropped 字段——按格式不列于此段，见上文 Follow-up 与 docs-delta。

## 里程碑判定

（dogfood）**本包不触发里程碑阶段跳变**。依据：`.delivery/milestones.jsonl` **无头注释**（文件直接以 JSON lines 起首，未见任务描述所称的「头注释」）；`milestones.jsonl` 的 M0/M1 恒在 `in_delivery`、本包（server 网关底座，mergeOrder 3）是方向 B 阶段 A「服务端就绪」/ 后续 M2 全链路的前提，不单独收口任一 M0–M6 里程碑；且 taskpackage 未置 `deliveryMilestoneStateChanged:true`（closeout 里程碑子闸不触发）。故不改 `.delivery/milestones.jsonl`、不重跑 `delivery-status.mjs` 改 STATUS.md（`.delivery/**` 亦不在 integrator 写面）。此判断如实记录——若「首刀进 in_delivery」的预期存在，其已由 M0/M1 既有 `in_delivery` 状态承载，本包不重复跳变；留待 planner/主线程在后续里程碑（如 M2）阶段收口时按既定口径 iterate 更新。