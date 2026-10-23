# KidPal direction-B gateway — 部署指引

KidPal 方向 B「唯一公网入口 = 流式网关」（spec 决策 18 / §3.0.0）的服务器端部署承载。本文件是 `server/deploy/` 下部署件的 README 性指引（GWA-A2 / GWA-E2 的对齐文档），指向任务包 `260903-gateway-directionB` 与 `delivery/taskpackages/260903-gateway-directionB/deploy.md`（部署期实证证据）。

## 部署拓扑

```
 设备 (wss://<ECS>:8000/xiaozhi/v1/)
     │  wss (TLS, 自签证书 ≥10y)
     ▼
 TLS FRONT  server/gateway/front  （公网 8000，终结 TLS + 强制设备鉴权）
     │  plain ws -> LOOPBACK 127.0.0.1:8010
     ▼
 容器引擎  xiaozhi-esp32-server（官方镜像 + engine-patch 部署副本补丁）
     │  LLM type:openai -> OpenClaw（host loopback 18789，V28 锁）
```

- **引擎腿**：官方 `ghcr.nju.edu.cn/xinnan-tech/xiaozhi-esp32-server:server_latest`（China mirror；拉不到退 `ghcr.io/...`）。容器镜像本体无改；display fence 剥离补丁以**部署副本 bind-mount** 形态覆盖镜像内 `core/`（见下节「引擎部署副本补丁」）。`data/.config.yaml` overlay + models 卷挂载，keys 不入镜像层、数据跨重建持久。
- **传输腿**：引擎 plain ws:8000 仅暴露于宿主 **LOOPBACK 127.0.0.1:8010**；设备侧公网 wss:8000 由 TLS FRONT 承担——front 终结 TLS + 强制 token 鉴权后转发到 loopback 腿。引擎不外露。（260903-device-ws-switch live 重切：`docker-compose.yml` 改 `network_mode: host`，引擎**自绑** `127.0.0.1:8010`、无 ports 网桥——对外形状不变，见 `delivery/taskpackages/260903-device-ws-switch/`。）
- **脑腿**：OpenClaw 网关 `openclaw.json gateway.bind=loopback`（18789 只绑 `127.0.0.1`，V28 硬前置）；`network_mode: host` 下容器经**真 loopback `127.0.0.1:18789`** 直达宿主 OpenClaw（原 `host.docker.internal:18789` 网桥随 host 模式移除，260903-device-ws-switch live）。

## 部署入口（manifest + deploy.sh · 261013-gov-review-deploy-single-source-loop 起）

ECS 三件套的部署已从手工拷贝切为「GitHub 单源 → ECS 拉取 → manifest 铺放 → 机械核验」闭环：

- **清单**：[manifest.yaml](manifest.yaml) =「什么该在 live 上」的唯一权威（front 源 / overlay 输入与渲染产物 / OpenClaw 创作面 / reminder 消费位 / 漂移 timer / cron 作业核验）。加改部署面 = 改清单进 review。
- **部署**：`cd /opt/kidpal/deploy-clone && bash server/deploy/deploy.sh [--ref origin/main]`——fetch + checkout → 按清单铺放（变更集决定重启面，引擎重启 = 设备全断只在引擎面变更时发生）→ 引擎面变更时跑 `apply-xiaozhi-overlay.sh` 渲染 → 落 `.deployed-commit` 戳 + 快照 → 自检。
- **核验**：[verify-deploy.sh](verify-deploy.sh) 三时机共用——部署前拦截（有未对账漂移拒铺，`--first-run` 例外）、部署后自检、每日 04:17 systemd timer（`kidpal-deploy-drift.timer`）落 `/opt/kidpal/alerts/deploy-drift.json`。
- **一次性 bootstrap**（deploy key / clone / 首铺 / timer 安装）见 [ecs-bootstrap.md](ecs-bootstrap.md)。
- **纪律**：live 紧急热修允许，但下次部署被 verify fail-loud 拦截直至回流仓；OpenClaw 运行态（DREAMS.md / memory / sessions）不入清单、部署不碰；secrets 仅存 ECS。

## 引擎部署副本补丁（engine-patch · 260907-g-relay-fix）

方向 B 的 display fence 剥离以「引擎为主剥离、front 为校验/relay 权威」分工落地（TaskPackage `260907-g-relay-fix`，human 批准首例部署副本代码补丁，pod-events decision `engine-patch-approval` 事件 `37469170`）。补丁形态：官方镜像不动，`server/deploy/engine-patch/` 下两个部署副本文件经 `docker-compose.yml` bind-mount 原位覆盖镜像内 `core/`：

| 仓库文件（宿主） | 容器内落点 | 内容 |
|---|---|---|
| `server/deploy/engine-patch/connection.py` | `/opt/xiaozhi-esp32-server/core/connection.py` | 0.9.6 部署副本改：LLM 流式累积段（`tts_text_queue` 入队前）接 fence 状态机；对话历史 `dialogue.put` 收剥离前原文；连接建立时打补丁标记行 |
| `server/deploy/engine-patch/kidpal_display_strip.py` | `/opt/xiaozhi-esp32-server/core/kidpal_display_strip.py` | 纯 Python fence 状态机（流式 `feed()`/`finish()`），剥离出的 display 原始载荷经 `kidpal_display_blocks` 侧通道哑管道透传 front |

front 侧对应职责（`server/gateway/front_app.py`）：截获侧通道帧 → `display_schema` 机械校验（脏块降级友好提示，不静默）→ 逐块以 `{"type":"mcp","payload":<kidpal/display tools/call>}` mcp 帧下发设备。

### 核验方式（每次部署/重启后应做）

1. **启动标记行**：`docker logs xiaozhi-esp32-server 2>&1 | grep "kidpal display-strip patch active"`——补丁后的 `connection.py` 在**每次设备建连**时打一行该标记；有标记 = bind-mount 的补丁副本在容器内真实运行（排除陈旧容器假绿）。标记行缺失即补丁未生效（见漂移告警）。
2. **bind-mount 在位**：`docker inspect xiaozhi-esp32-server --format '{{json .Mounts}}'` 应含上表两个宿主→`core/` 映射。

### 漂移告警语义

- **标记行缺失 = 补丁丢失或未装载**：镜像更新/重建后若 compose 卷映射被改动或部署副本文件不在位，容器会静默回退到镜像原生 `connection.py`（fence 重新混入播报文本、display relay 断链）——唯一的机械报警锚就是标记行不再出现，等价于 live 缺陷回归，应立即对照本节排查。
- **镜像升级 = 补丁语义漂移风险**：官方镜像升级后镜像内 `core/connection.py` 上游演进，宿主部署副本（0.9.6 基线）可能与新版引擎内部 API 不匹配。升级镜像前先核对上游 diff；标记行仍在但引擎行为异常时优先怀疑此漂移。治理项已登记 `docs/debt/tech/ARQ-260907-engine-deploy-patch-drift.md`（owner=dev-loop 运维面；原 `docs/ARCH-REVIEW-QUEUE.md` 261019 批删除）。
- 已知环境怪癖（非本补丁引入）：容器 healthcheck 显示 `unhealthy`——镜像内无 `ss`/`netstat`，健康检查命令恒失败；引擎实际健康以标记行 + 设备可建连为准。

## OpenClaw system prompt 承载（静态语义 = AGENTS.md · 动态 context 块）

261022-m5-k1-prompt-carrier-migration 起 prompt 承载反转（arch §5.5.5 / ADR-001 重定义）：

- **静态语义（主承载，缓存边界之上）**：单一事实源 = `openclaw-agents/workspace-kidpal/AGENTS.md`
  （manifest 登记、`deploy.sh` 铺放、`verify-deploy.sh` 每日 md5 核验）。回合路由、各域纪律、
  底线三不变量全部住这里，不再进引擎注入面。
- **动态 context 块（每轮，缓存边界之下）**：模板 = `openclaw-agents/kidpal-dynamic-context.v1.txt`，
  由 `apply-xiaozhi-overlay.sh` 以 2 空格缩进注入引擎 `.config.yaml` 顶层 `prompt:` 键；内容有界
  （本回合时间 / 日期 + 课表摘要占位），不含静态纪律。
- **冻结终点版存档**：`openclaw-system-prompt.v11.md` 原地冻结、不再 bump；`PROMPT_SRC` /
  `PROMPT_DST` 仍把它拷进 ECS `data/`（历史可 diff、版本无关守卫可解析），但**不再注入
  `prompt:` 键**。版本演进史唯一全量副本 = `OPENCLAW_SYSTEM_PROMPT_CARRIER`。

**承载不变式（ADR-001 重定义）**：① 静态面：渲染面（live workspace `AGENTS.md`）== 声明面
（`OPENCLAW_SYSTEM_PROMPT_CARRIER` + `provider.config.yaml` 的 `static_prompt` 三键）== 被测面
（探针直读同一 `AGENTS.md`）；② 动态面：渲染面（`.config.yaml` `prompt:` 值）== 声明面
（`provider.config.yaml` 的 `dynamic_context` 三键，装配壳 `prompt_template` 不计第二模板件）
== 被测面（探针请求同源）。

机械承载 = 冻结验收 `test_261022_m5_k1_prompt_carrier_migration.acceptance.py`（K1-E1/E2/E3：
静态语义迁移锚、动态件有界、离线渲染产物反解内容源恰一、marker 锚集 ↔ 渲染产物一致性）与
`test_261022_openclaw_kidpal_tools.acceptance.py`（K1-E4 工具面收窄）。版本无关接线守卫
（`test_char_meaning_merge.acceptance.py` 等）继续从 `PROMPT_SRC` 与 `system_prompt` 三键解析
终点版文件——两处结构化指向保持不变。

### v11 上 ECS 的动作序（本包只产制品，未执行上线）

1. `bash server/deploy/apply-xiaozhi-overlay.sh`（把 v11 拷进 ECS `data/` 存档 + 把动态 context
   块注入 overlay `prompt:` 键）；
2. 重启 `openclaw-gateway`（prompt 生效面）与 `kidpal-front`（装配壳与 stories 旁路消费；stories
   归档目录由 `KIDPAL_STORIES_DIR` 决定，缺省 `/opt/kidpal/stories`）；
3. `deploy.sh` 铺放仓内 `AGENTS.md` 到 live workspace（静态语义主承载；铺放后 `verify-deploy.sh`
   的 md5 漂移消失）；
4. 复跑版本无关守卫与 K1 验收（offline 段应全绿，live 段机械 skip）。


## 关键环境变量

| 变量 | 含义 | 典型值 / 说明 |
|---|---|---|
| `GATEWAY_DEPLOY_DIR` | 部署源 | **live 引擎配置真身在 `deploy/xiaozhi-docker-data/`**（bind-mount 进容器 `data/`；`main/xiaozhi-server` 是 stale decoy，勿读勿写——261013 批修正本行旧注记）；manifest 化后该变量仅 overlay 渲染期内部使用 |
| `GATEWAY_SERVICE` | systemd 单元名 | `xiaozhi`（容器引擎） / `kidpal-front`（TLS front） |
| `GATEWAY_CERT_PATH` | 证书路径（frozen） | `/opt/kidpal/xiaozhi-esp32-server/cert/server.pem`（+ 同名 `server.key`，chmod 640） |
| `DAEMON_ENV` 等渲染源 | overlay 渲染输入 | 由 `apply-xiaozhi-overlay.sh` 从 `secrets.env` 注入 provider 配置 |

> live 验收运行期提示：冻结 live 断言 `test_a4_live_provider_config_declared_on_ecs` 读 `${GATEWAY_DEPLOY_DIR}/config.yaml`——跑 live 时显式置 `GATEWAY_DEPLOY_DIR=/opt/kidpal/xiaozhi-esp32-server/main/xiaozhi-server`（或把 provider 声明放该层 config.yaml）。见 review-packet residual risk「live A4 路径错位」。

## Secrets 管理

- **真实 secrets 只存 ECS** `server/deploy/secrets.env`（chmod 600）；**仓库仅保留** `secrets.env.example` 占位模版。
- `apply-xiaozhi-overlay.sh` 渲染 `data/.config.yaml` 时从 `secrets.env` 注入，**绝不硬编码**、不进镜像层。
- **设备共享单 token**（决策 22）：≥22 字符 / ≥128-bit，`auth.authorize` constant-time 比对；**token 未设 = 拒绝服务**（不留 validation-mode 后门）。
- `OPENCLAW_TOKEN`：brain loopback token（与 `server/shim.py` 共用，shim 随 WS 包退役）。
- 云端 ASR 凭据（决策 19 云优先 / V23；终选 Qwen3ASRFlash，human 2026-09-06）：`DASHSCOPE_API_KEY` 留空 = live ASR 已标注为 known credential gap（human 待办），结构面仍按可声明形状实现，不伪造绿。
- 本仓 grep 复查零泄漏：`sk-sp-` / 真 token 值全部只出在 ECS `secrets.env`(600)，git 树内无可回显值。

## 证书与轮换（GWA-A9 / ARQ-260902-cert-lifecycle-deadlock）

- `cert-renew.sh renew|force`：`-days 7300`（20 年 ≥ 决策 22 的 ≥10 年），幂等轮换（auto 对未到期有效证书跳过）。
- `cert-warn-expiry.sh`：解析 `notAfter`，30 天窗口内/已到期 → WARN + 非零退出（挂 cron/systemd）。
- 到期死局 = **accept-as-risk 记账**（human 2026-09-04）：验收面不变，不 OTA 提前，watchlist 留待 OTA 建立或证书剩余 <5 年时复议（ARQ-260903 登记）。