# KidPal direction-B gateway — 部署指引

KidPal 方向 B「唯一公网入口 = 流式网关」（spec 决策 18 / §3.0.0）的服务器端部署承载。本文件是 `server/deploy/` 下部署件的 README 性指引（GWA-A2 / GWA-E2 的对齐文档），指向任务包 `260903-gateway-directionB` 与 `delivery/changes/dev/260903-gateway-directionB/deploy.md`（部署期实证证据）。

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
- **传输腿**：引擎 plain ws:8000 仅暴露于宿主 **LOOPBACK 127.0.0.1:8010**；设备侧公网 wss:8000 由 TLS FRONT 承担——front 终结 TLS + 强制 token 鉴权后转发到 loopback 腿。引擎不外露。（260903-device-ws-switch live 重切：`docker-compose.yml` 改 `network_mode: host`，引擎**自绑** `127.0.0.1:8010`、无 ports 网桥——对外形状不变，见 `delivery/changes/dev/260903-device-ws-switch/`。）
- **脑腿**：OpenClaw 网关 `openclaw.json gateway.bind=loopback`（18789 只绑 `127.0.0.1`，V28 硬前置）；`network_mode: host` 下容器经**真 loopback `127.0.0.1:18789`** 直达宿主 OpenClaw（原 `host.docker.internal:18789` 网桥随 host 模式移除，260903-device-ws-switch live）。

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
- **镜像升级 = 补丁语义漂移风险**：官方镜像升级后镜像内 `core/connection.py` 上游演进，宿主部署副本（0.9.6 基线）可能与新版引擎内部 API 不匹配。升级镜像前先核对上游 diff；标记行仍在但引擎行为异常时优先怀疑此漂移。治理项已登记 `docs/ARCH-REVIEW-QUEUE.md` `ARQ-260907-engine-deploy-patch-drift`（owner=dev-loop 运维面）。
- 已知环境怪癖（非本补丁引入）：容器 healthcheck 显示 `unhealthy`——镜像内无 `ss`/`netstat`，健康检查命令恒失败；引擎实际健康以标记行 + 设备可建连为准。

## OpenClaw system prompt 版本化接线（现状 = v7）

脑腿的 system prompt 按「版本化落盘 + overlay 渲染切换 + 历史可 diff」惯例演进，声明件 =
`OPENCLAW_SYSTEM_PROMPT_CARRIER`——**它同时是版本演进史的唯一全量副本**，本节与其它散文
只留「当前 = vN」指针、不复述沿革（260925-prompt-v7-round-routing 收敛，沿革四重复制
漂移教训见 carrier）。**当前被接线的那一版 = `openclaw-system-prompt.v7.md`**
（回合类型路由表 + 故事段对齐契约禁产 story JSON + 语音「简洁不啰嗦」纪律 + 制品本体
无版本沿革前言；human 260925 拍板）。

**接线不变式：渲染面 == 声明面 == 被测面**。同一切换必须同时改两处并保持一致：

- 渲染面：`apply-xiaozhi-overlay.sh` 的 `PROMPT_SRC` / `PROMPT_DST`（拷进 ECS
  `GATEWAY_DEPLOY_DIR/data/`，与 `.config.yaml` 同目录，并注入 overlay 顶层 `prompt:` 键）；
- 声明面：`provider.config.yaml` 的 `system_prompt.artifact` / `source` / `deploy_target` 三键。

机械承载 = 冻结验收 `test_story_p3.acceptance.py::test_prompt_carrier_v3_landed_and_wired`
（解析 `PROMPT_SRC` 得出版本号 → 断言 ≥3 → 断言该版文件落盘非空且含故事/V31/归档三组指令 →
断言 `provider.config.yaml` 含同名文件）与已重指为版本无关的
`test_char_stroke.acceptance.py::test_e3_prompt_v2_wired_into_deploy`（两面各解析出被接线的
`vN` 文件名 → 两面集合相等 → 该版文件存在且非空 → 正文逐字携带 char C1 锚点）。后者显式
**不认** deploy 目录里的散文提及与 carrier 前瞻注记，只认两面结构化指向——这是版本化制品守卫
应取的形状。

### 已知版本字面漂移（本包未修，归 ARQ-260908）

`docs/ARCH-REVIEW-QUEUE.md` 的 `ARQ-260908-versioned-guard-version-literal-coupling`（Track A，
owner=architect + acceptance-author）登记了「语义耦合在版本字面文件名上」这一通用病灶。carrier
切到 v3 后的实测现状，改本节或 bump v4 前须知道：

- **live 探针仍钉旧版字面**：`test_voice_roundtrip_b.acceptance.py:116 SYSTEM_PROMPT_PATH =
  openclaw-system-prompt.v1.md`（VRT-A4 / VRT-A7 live 用它作 prompt 源）与
  `test_char_stroke.acceptance.py:144 PROMPT_V2_PATH`（CHR-A8 / CHR-A9 live 用它作 prompt 源）。
  被接线版为 v3 时这两组 live 齿属「所测 ≠ 生产所用」；本仓无 live 环境，它们由 conftest 机械
  skip、不 gate 任何合并判据，故本包未扩改（重指无法实证），随该 ARQ 一并版本无关化。
- **P2-A6 已成为假绿**：`test_schedule_p2.acceptance.py:583-586` 的「wiring 引用 v2」判据把语料取
  成 `DEPLOY_DIR.rglob("*")` **全部文件**、needle 是完整字面 `openclaw-system-prompt.v2.md`；该字面
  现仅由 `.v2.md` 文件自述头 + `openclaw-schedule.config.yaml:4` 注记 + 本 carrier 历史句维持，
  与被接线的真实渲染源（已 v3）无关——断言仍绿但已不再检验它声称检验的东西。建议按上面
  story/CHR-E3 的两面解析形状重指（同归该 ARQ，勿在本包动冻结文件）。
- **不受本次 bump 影响的守卫**：VRT-E4（`test_e4_system_prompt_wired_into_deploy_overlay`）needle
  是不带版本后缀的裸名 `openclaw-system-prompt`，且语料限 `suffix == ""` 的无扩展名文件（即本
  carrier），故它天然版本无关、继续有效——这是 forward-bump 时唯一不必担心的那条。

### v3 上 ECS 的动作序（本包只产制品，未执行上线）

本包交付的是**仓库侧部署制品 + 接线**；把 v3 推到 ECS 属后续部署触发，不在本包冻结验收面内
（live 端到端意图与真实采音殿后 `9b`，见 P3-E2）。执行时按下列顺序，且任一步失败即停：

1. `bash server/deploy/apply-xiaozhi-overlay.sh`（渲染 v3 到 ECS `data/` + 注入 overlay `prompt:` 键）；
2. 重启 `openclaw-gateway`（prompt 生效面）与 `kidpal-front`（`gateway/story.py` 新模块 + front 侧
   `control/start_recording` 旁路消费在此装载；stories 归档目录由 `KIDPAL_STORIES_DIR` 决定，
   缺省 `/opt/kidpal/stories`）；
3. 复跑版本无关守卫：`cd server && python -m pytest acceptance/test_char_stroke.acceptance.py
   acceptance/test_story_p3.acceptance.py -q --import-mode=importlib`（offline 段应全绿，live 段机械 skip）。

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
- 云端 ASR 凭据（决策 19 云优先 / V23；终选 Qwen3ASRFlash，human 2026-09-06）：`DASHSCOPE_API_KEY` 留空 = live ASR 已标注为 known credential gap（human 待办），结构面仍按可声明形状实现，不伪造绿。
- 本仓 grep 复查零泄漏：`sk-sp-` / 真 token 值全部只出在 ECS `secrets.env`(600)，git 树内无可回显值。

## 证书与轮换（GWA-A9 / ARQ-260902-cert-lifecycle-deadlock）

- `cert-renew.sh renew|force`：`-days 7300`（20 年 ≥ 决策 22 的 ≥10 年），幂等轮换（auto 对未到期有效证书跳过）。
- `cert-warn-expiry.sh`：解析 `notAfter`，30 天窗口内/已到期 → WARN + 非零退出（挂 cron/systemd）。
- 到期死局 = **accept-as-risk 记账**（human 2026-09-04）：验收面不变，不 OTA 提前，watchlist 留待 OTA 建立或证书剩余 <5 年时复议（ARQ-260903 登记）。