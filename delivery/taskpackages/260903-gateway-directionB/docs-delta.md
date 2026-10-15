# docs-delta — 260903-gateway-directionB（方向 B 流式网关 bring-up）

> 性质：实现批（server 子系统，milestone-affecting，mergeOrder 3）。本包对 canonical docs 的影响。
> 核心结论：**spec/contract 零改动**——方向 B 已在 spec v0.2 FROZEN + 契约方向 B 重写（§四端云协议剖面）原子落地（2026-09-03 契约门四题 + human 放行），本包纯实现、只消费不修改 canonical。
> 本包 docs-sync 闸（r4.json gate docs-sync）已 PASS：docRefs 四条均消费引用，`docsNoChangeReason: null`；docs-delta 制品在 integrator closeout 相位落地（docsOwner=integrator）。

## A. docRefs 现状与无需更新理由

| docRef | 现状 | 本包影响 | 无需更新理由 |
|---|---|---|---|
| `.planning/kidpal-mvp/PROPOSAL.md` | FROZEN v0.4（重切 15 包），本包 `origin.upstreamRef`，pin `sha1:88a5bdb83abdd867` | 只读（origin ref） | 受 `upstreamFrozen` sha1 pin 约束禁改（防 UPSTREAM_DRIFT）；本包实现与其 G 包定义逐条对齐，逆向验证通过 |
| `docs/spec.md#§3.0.0` | v0.2 FROZEN：方向 B（WebSocket + Opus 流式，官方小智网关）为唯一公网入口 | 只读章节锚点引用 | 本包承载其「服务端网关 + 强制鉴权（决策22）+ provider 适配」语义，零 spec 文本变更 |
| `docs/spec.md#§6.1` | v0.2 FROZEN：端云协议剖面（hello/Opus 帧/stt/tts/abort/mcp/鉴权/会话连续性） | 只读章节锚点引用 | 本包 `server/gateway/*` 按 §6.1 语义实现（auth 握手、display 切分、mcp 下发），零 spec 文本变更 |
| `docs/contracts/interface-contract.md#§四` | 方向 B 重写版：端云协议剖面 + `type:openai` 接入禁带 max_tokens（V9 硬约束）+ §1.5 网关职责收敛（强制鉴权 + display 机械校验） | 只读引用 | 本包 `provider.openclaw_stream_request` 严格遵循禁 max_tokens；`display_schema`/`auth` 按 §四/§1.5 实现；契约面零触碰（separate 只读，改动走契约门） |

**无需更新总述**：本包没有改变任何 canonical 语义；全部 `docs/contracts/**` 与 `docs/spec.md` 在本次 closeout 中零写（git diff 确认无 docs 根白名单外文件、无影子目录）。spec/contract 的 28 条决策与方向 B 协议已在上游冻结时定稿，本包是把它们实现/实证出来，不反过来改文档。

## B. 本包落地（docs-sync 相关，非 canonical 文档）

| 目标 | 位置 | 说明 |
|---|---|---|
| `server/gateway/*` | server/ | 实现面（auth / display_schema / provider / fallback / mcp_relay / front）——纯 Python 模块，import 不依赖 fastapi/edge_tts/httpx（冻结模块契约），承载 §1.5/§四语义 |
| `server/deploy/*` | server/ | 部署件：`provider.config.yaml`（provider 声明）/ `apply-xiaozhi-overlay.sh` / `docker-compose.yml` / `kidpal-front.service` / `xiaozhi-container.service` / `xiaozhi-gateway.service` / `cert-renew.sh` / `cert-warn-expiry.sh` / `secrets.env.example` / **`README.md`（新增，deployment-facing 指引，见 §C）** |
| `server/evidence/260903-gateway-directionB/*` | server/ | builder 证据落盘暂存（write-scope 对齐项，同内容晋升 `changes/dev/`） |
| `changes/dev/260903-gateway-directionB/*` | changes/dev/ | closeout/audit 制品：本 docs-delta + review-packet + 4 件证据（v21 / v23 / deploy / cert-lifecycle-accept-risk） |

## C. deployment-facing 文档指引（server/deploy/README.md）

为部署人补齐 `server/deploy/` 下 README 性说明（新增 `README.md`），覆盖：

- **部署拓扑**：官方镜像容器引擎（`ghcr.nju.edu.cn/xinnan-tech/xiaozhi-esp32-server:server_latest`，零引擎补丁）+ 宿主 LOOPBACK 转发腰（引擎 plain ws:8000 → 宿主 `127.0.0.1:8010`）+ **TLS front**（`server/gateway/front` 在公网 wss:8000 终结 TLS + 强制设备鉴权）→ 转发到 loopback 腿。设备侧唯一公网入口 = wss front（决策 18/§3.0.0）。
- **关键环境变量**：
  - `GATEWAY_DEPLOY_DIR`：部署源 = `/opt/kidpal/xiaozhi-esp32-server/main/xiaozhi-server`（container 引擎 → 容器内 `/opt/xiaozhi-esp32-server/data`；live 验收 `test_a4_live_provider_config_declared_on_ecs` 需在跑 live 时置 `GATEWAY_DEPLOY_DIR` 指向该层 config.yaml，见 review-packet residual risk「live A4 路径错位」）。
  - `GATEWAY_SERVICE= xiaozhi`、`GATEWAY_CERT_PATH=/opt/kidpal/xiaozhi-esp32-server/cert/server.pem`（证书/脚本路径 frozen）。
  - provider 渲染依赖 secrets.env（见下）。
- **secrets 管理**：真实 secrets 只存 ECS `server/deploy/secrets.env`（chmod 600），仓库仅保留 `secrets.env.example`（占位）；`apply-xiaozhi-overlay.sh` 渲染时从 env 注入，绝不硬编码。本仓 grep 复查零 key 泄漏（`sk-sp-`/`OPENCLAW_TOKEN` 真值零入仓）。token 共享单 token ≥128-bit + constant-time 比对（决策 22）。

## D. 治理追认（docs 队列级，append 不覆盖）

- `docs/ARCH-REVIEW-QUEUE.md`：① ARQ-260902-cert-lifecycle-deadlock 自 §1.1 迁至 §1.3 watchlist（human 2026-09-04 accept-as-risk 记账）；② 新增 ARQ-260904-gate-regex-uppercase-changeid（§1.2，一次性热修后审固化）；③ 新增 ARQ-260904-v23-asr-swap-quota-recheck（§1.2，配额 2026-09-07 14:19 UTC 后补验）。
- `.planning/kidpal-mvp/progress.md`：追加本包 closeout 一线（G 收口）。

## E. 无漂移确认

- `docs/contracts/interface-contract.md`：零写（只读引用）。
- `docs/spec.md`：零写（章节锚点引用）。
- 无新增 `docs/` 根白名单外文件、无影子目录（影子 `contracts/` 由 scope-map denyCreate 拦截）。
- `changes/arch/**` 零改（本包无架构裁决需归档，G 固件侧裁决属 ADR 面留待 WS/固件包）。