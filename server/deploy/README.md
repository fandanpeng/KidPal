# KidPal direction-B gateway — 部署指引

KidPal 方向 B「唯一公网入口 = 流式网关」（spec 决策 18 / §3.0.0）的服务器端部署承载。本文件是 `server/deploy/` 下部署件的 README 性指引（GWA-A2 / GWA-E2 的对齐文档），指向任务包 `260903-gateway-directionB` 与 `changes/dev/260903-gateway-directionB/deploy.md`（部署期实证证据）。

## 部署拓扑

```
 设备 (wss://<ECS>:8000/xiaozhi/v1/)
     │  wss (TLS, 自签证书 ≥10y)
     ▼
 TLS FRONT  server/gateway/front  （公网 8000，终结 TLS + 强制设备鉴权）
     │  plain ws -> LOOPBACK 127.0.0.1:8010
     ▼
 容器引擎  xiaozhi-esp32-server（官方镜像，零补丁）
     │  LLM type:openai -> OpenClaw（host loopback 18789，V28 锁）
```

- **引擎腿**：官方 `ghcr.nju.edu.cn/xinnan-tech/xiaozhi-esp32-server:server_latest`（China mirror；拉不到退 `ghcr.io/...`）。容器零引擎侧补丁；`data/.config.yaml` overlay + models 卷挂载，keys 不入镜像层、数据跨重建持久。
- **传输腿**：引擎 plain ws:8000 仅暴露于宿主 **LOOPBACK 127.0.0.1:8010**；设备侧公网 wss:8000 由 TLS FRONT 承担——front 终结 TLS + 强制 token 鉴权后转发到 loopback 腿。引擎不外露。（260903-device-ws-switch live 重切：`docker-compose.yml` 改 `network_mode: host`，引擎**自绑** `127.0.0.1:8010`、无 ports 网桥——对外形状不变，见 `changes/dev/260903-device-ws-switch/`。）
- **脑腿**：OpenClaw 网关 `openclaw.json gateway.bind=loopback`（18789 只绑 `127.0.0.1`，V28 硬前置）；`network_mode: host` 下容器经**真 loopback `127.0.0.1:18789`** 直达宿主 OpenClaw（原 `host.docker.internal:18789` 网桥随 host 模式移除，260903-device-ws-switch live）。

## 关键环境变量

| 变量 | 含义 | 典型值 / 说明 |
|---|---|---|
| `GATEWAY_DEPLOY_DIR` | 部署源 | `/opt/kidpal/xiaozhi-esp32-server/main/xiaozhi-server`（CONFIG 在这里；`docker-compose` 卷把 data/.config.yaml 映射进容器 `/opt/xiaozhi-esp32-server/data`） |
| `GATEWAY_SERVICE` | systemd 单元名 | `xiaozhi`（容器引擎） / `kidpal-front`（TLS front） |
| `GATEWAY_CERT_PATH` | 证书路径（frozen） | `/opt/kidpal/xiaozhi-esp32-server/cert/server.pem`（+ 同名 `server.key`，chmod 640） |
| `DAEMON_ENV` 等渲染源 | overlay 渲染输入 | 由 `apply-xiaozhi-overlay.sh` 从 `secrets.env` 注入 provider 配置 |

> live 验收运行期提示：冻结 live 断言 `test_a4_live_provider_config_declared_on_ecs` 读 `${GATEWAY_DEPLOY_DIR}/config.yaml`——跑 live 时显式置 `GATEWAY_DEPLOY_DIR=/opt/kidpal/xiaozhi-esp32-server/main/xiaozhi-server`（或把 provider 声明放该层 config.yaml）。见 review-packet residual risk「live A4 路径错位」。

## Secrets 管理

- **真实 secrets 只存 ECS** `server/deploy/secrets.env`（chmod 600）；**仓库仅保留** `secrets.env.example` 占位模版。
- `apply-xiaozhi-overlay.sh` 渲染 `data/.config.yaml` 时从 `secrets.env` 注入，**绝不硬编码**、不进镜像层。
- **设备共享单 token**（决策 22）：≥22 字符 / ≥128-bit，`auth.authorize` constant-time 比对；**token 未设 = 拒绝服务**（不留 validation-mode 后门）。
- `OPENCLAW_TOKEN`：brain loopback token（与 `server/shim.py` 共用，shim 随 WS 包退役）。
- 阿里云云 ASR 凭据（决策 19 云优先 / V23）：`DASHSCOPE_API_KEY` / `ALIYUN_ASR_APPKEY` / `ALIYUN_ASR_TOKEN` 留空 = live ASR 已标注为 known credential gap（human 待办），结构面仍按可声明形状实现，不伪造绿。
- 本仓 grep 复查零泄漏：`sk-sp-` / 真 token 值全部只出在 ECS `secrets.env`(600)，git 树内无可回显值。

## 证书与轮换（GWA-A9 / ARQ-260902-cert-lifecycle-deadlock）

- `cert-renew.sh renew|force`：`-days 7300`（20 年 ≥ 决策 22 的 ≥10 年），幂等轮换（auto 对未到期有效证书跳过）。
- `cert-warn-expiry.sh`：解析 `notAfter`，30 天窗口内/已到期 → WARN + 非零退出（挂 cron/systemd）。
- 到期死局 = **accept-as-risk 记账**（human 2026-09-04）：验收面不变，不 OTA 提前，watchlist 留待 OTA 建立或证书剩余 <5 年时复议（ARQ-260903 登记）。