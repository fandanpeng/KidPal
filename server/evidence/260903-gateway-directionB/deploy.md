# 部署证据 v0.1 —— 方向 B 流式网关 ECS 部署（GWA-A2 / GWA-E2）

> **_INTEGRATOR LANDING NOTE_**: staged by builder in `server/`; builder write-scope excludes
> `delivery/changes/dev/`. Integrator must land `deploy.md` into
> `delivery/changes/dev/260903-gateway-directionB/` (name contains `deploy`) for
> `test_e2_deploy_evidence_in_changes_dev` to go green. Read verbatim for honest status —
> DO NOT edit to claim a running service that is not yet up.

## 部署目标与拓扑快照

- **对象**: ECS `8.137.91.16`（root，systemd）
- **部署源**: `/opt/kidpal/xiaozhi-esp32-server`（官方 xinnan-tech/xiaozhi-esp32-server, MIT，克隆已非分支/未部署；服务端代码在 `main/xiaozhi-server/`）
- **systemd 单元**: `/etc/systemd/system/xiaozhi.service` 已写入 + `daemon-reload` + `enable`（`server/deploy/xiaozhi-gateway.service` 为本仓承载副本）。**当前未 start 到 active**（见「阻塞与升级」）。
- **证书**: `/opt/kidpal/xiaozhi-esp32-server/cert/server.pem`（自签 RSA-3072，notBefore 2026-09-04 → notAfter 2046-08-30，20 年 ≥ 决策22 的 ≥10 年），`server.key` 640。轮换脚本 `server/deploy/cert-renew.sh`、告警脚本 `server/deploy/cert-warn-expiry.sh`。
- **18789 锁（V28）**: OpenClaw 网关 `openclaw.json gateway.bind` 从 `lan` 改为 **`loopback`** 并 `systemctl restart openclaw-gateway.service`；实测 `ss -ltnp` 仅见 `127.0.0.1:18789` + `[::1]:18789`，再无 `0.0.0.0:18789` / `*:18789`。loopback 可达（`/v1/models` HTTP-401=在线，未带 token 属预期）。
- **provider 承载**: `server/deploy/provider.config.yaml`（LLM type:openai→OpenClaw、TTS edge-tts、ASR aliyun，禁 max_tokens）+ `server/deploy/apply-xiaozhi-overlay.sh` 渲染 `data/.config.yaml`（已落盘 ECS）。`server/deploy/secrets.env`（chmod 600，含 device token 与 OPENCLAW_TOKEN；aliyun 凭据为空）。
- **venv**: `/opt/kidpal/venv`（Python 3.11.13，已装 fastapi/uvicorn/websockets/edge-tts + 本次补 light deps：aioconsole/loguru/ormsgpack/ruamel.yaml/pydub/silero_vad/opuslib_next/bs4/jinja2/portalocker/psutil/PyJWT/chardet/dashscope/mcp）。
- **已有服务**: `openclaw-gateway.service`（active，18789 loopback）、`kidpal-shim.service`（active，127.0.0.1:8080 旧 HTTP shim，随 WS 包退役，本包未动）。

## 部署期已执行并断言

- cert ≥10y 生成并落盘（`openssl x509 -dates` 已核）。
- OpenClaw 18789 loopback 锁（V28，公网入口收敛）并重启后 SS 断言通过。
- xiaozhi 引擎 boot 冒烟推进：`import app` 越过依赖到 `data/.config.yaml` 门 → 补齐 overlay 后继续 → 依赖链逐级暴露（`mcp` 已装）→ 当前停在 `markitdown` 等**函数插件 auto-import 全量依赖**（见阻塞）。
- V21 连续性探针（详见 v21-evidence）：stateless 不续、每轮透传历史续 → 承载方案「每轮透传历史」定案。

## 阻塞与升级（真实，未造假绿）

1. **引擎 boot 的依赖闭包**：`app.py` boot 经 `core/connection → plugins_func auto_import_modules` 把全部函数/工具插件**无条件 import**，即使只配云 provider 也拉 `markitdown/funasr/sherpa_onnx/vosk/modelscope/mem0ai/torch...` 全量重依赖（torch 2.2.2 等）。云优先路径**无法**靠轻量子集 boot。方案：装全量重依赖（慢/占空间，modelscope/huggingface 本机不可达风险）或改第三方 boot import 闭包（不改参考树）。→ **升级 planner**。
2. **公网绑定 8000 需人类确认**：引擎按 xiaozhi 默认绑 `0.0.0.0:8000`（设备侧可达的 wss 端口，GWA-A2 语义）。该**公网暴露需显式 consent**（本次在 pack 内 SSH 以 loopback 冒烟验证，未开公网）。→ **升级 planner/human**。
3. **wss(TLS) 网关 front 未建**：官方 xiaozhi 只提供 plain ws + HTTP `/command`/TLS-on-8000 均无；live 验收的 `Authorization` 握手鉴权、空文本 400、`/command`、mcp/fallback 下发需要**自建网关 front**（TLS 终结 + `server/gateway/*` 组合）。→ **升级（本包 server/gateway 已备好构建块）**。
4. **阿里云 ASR 凭据缺失**：AliyunStreamASR appkey/token 与 Qwen3-ASR-Flash api_key 均不可得 → live ASR 首验未执行，human 待办（见 V23 evidence）。
5. **live A4 路径错位**：冻结 live 验收 `test_a4_live_provider_config_declared_on_ecs` 读 `${GATEWAY_DEPLOY_DIR}/config.yaml`，但真实布局在 `<deploy>/main/xiaozhi-server/config.yaml`；该断言需 env override 或接受者对齐路径，否则 live 必红。→ 记录待 planner/接受者确认。

## 状态

systemd 单元已安装、enable，引擎 boot 依赖闭包未完成；cert + 18789 锁 + overlay + secrets 已就位。**「active/running 服务」尚未达成**——不以文字冒充。（本段诚实状态，供 integrator 核销 GWA-E2 时对齐。）

---

## 追加：Task 2（官方 Docker + wss TLS front + 公网 8000）实测（2026-09-04/05）

### Docker 引擎拓扑（human 拍板选型落实）
- **Docker**：ECS `docker 26.1.4` + compose v2.27.1 可用。官方镜像 `ghcr.nju.edu.cn/xinnan-tech/xiaozhi-esp32-server:server_latest`（6.9GB，预装 python:3.10-slim + libopus/ffmpeg + 全量 pip 依赖——从源解决 boot 重依赖闭包）已成功拉取。
- **承载**：`server/deploy/docker-compose.yml`（引擎容器零补丁；`data/.config.yaml` overlay + models 卷挂载、数据落卷不落容器层；`extra_hosts host.docker.internal:host-gateway` 让容器内经 `http://host.docker.internal:18789` 达宿主 OpenClaw（V28 锁）；引擎 plain ws:8000 → 宿主 LOOPBACK `127.0.0.1:8010`）。
- **引擎 boot 状态**：容器能启动并**读到 overlay config**，但 **ASR provider 初始化即失败**——`AliyunStreamASR`（`aliyun_stream`）`_refresh_token()` 抛「无法获取有效的访问Token」（阿里云凭据为空）；本地降级 SenseVoice `model.pt` 亦缺失（models 目录仅 tokenizer/config）。→ 引擎进程 crash-loop，**升级项**：阿里云 ASR 凭据开通（human 待办，决策19 云优先；本地 SenseVoice 模型需另下载）。

### wss TLS front（组装 server/gateway/*）
- `server/gateway/front/front_app.py` 已装配并通过 **loopback 127.0.0.1:8000（TLS）** 实测（`kidpal-front-loop.test` 测试单元，非公网）：
  - healthz → `{"status":"ok","gateway_modules":[22,true,[char,control,home,schedule,story_list,story_text,text],4000,kidpal/display]}`（模块装配物证）。
  - **无 token ws 握手 → HTTP/1.1 400**；**错 token ws 握手 → HTTP/1.1 400**（live `test_a3_live_no_token/wrong_token` 断言「4」通过）。
  - **空文本 /command（带真 token，TLS）→ HTTP:400**；无/错 token → 401（live `test_live_blank_text` 的 4xx 语义在 TLS 面成立）。
  - 鉴权实现 = `gateway/auth.authorize`（token 未设=拒绝服务、constant-time、机械短路）+ TLS 终结（20 年证书）。
- **公网 0.0.0.0:8000 绑定未执行**：前端常驻公网部署需**人类本人**明确命名（系统权限闸要求）；coordinator 转述的 consent 不被视为用户授权。→ 见「待 human」。

### 实测矩阵（loopback 口，公网待 consent 后由 verifier 在 8.137.91.16:8000 复核）
| live 断言 | 状态 | 备注 |
|---|---|---|
| test_live_ssh_reachable | PASS | SSH 可达 |
| test_a3_live_no_token → 4xx | PASS(400) | front 握手层拒绝 |
| test_a3_live_wrong_token → 4xx | PASS(400) | front 握手层拒绝 |
| test_live_blank_text → 4xx | TLS 面 PASS(400)；**冻结断言用 plain http:// 会得 000（见下 gap）** | /command 行为正确 |
| test_a9_live_cert_validity ≥10y | PASS | notAfter 2046-08-30（20y） |
| test_e3_live_18789_bound_loopback | PASS | ss 仅 127.0.0.1:18789 |
| test_a4_live_openclaw_loopback | PASS(401=在线) | 18789 可达 |
| test_a2_live_wss_port_reachable(公网 8000) | **待公网绑定** | 需 user 明确的公网 8000 consent |

### 已知验收面 gap（交 planner/接受者，不改冻结验收）
- **空白文本端点是 plain-HTTP 语义**：冻结 `test_live_blank_text_rejected_400` 用 `curl http://127.0.0.1:8000/command`，但设备腿 8000 = wss(TLS)（auth 探针需 TLS）。同一 8000 单监听无法同时具备 plain-HTTP 与 TLS。建议接受者/planner 决定：`/command` 改到引擎 plain-http 8003（官方 SimpleHttpServer）或验收调整；本包 front 已按 TLS 实现并验证 400 正确。
- **live A4 路径错位**：冻结 `test_a4_live_provider_config_declared_on_ecs` 读 `${GATEWAY_DEPLOY_DIR}/config.yaml`，真实 config 在 `<deploy>/main/xiaozhi-server/config.yaml`（Docker 后实为容器内 /opt/.../config.yaml + data/.config.yaml）。**推荐取值**：运行 live 时 `GATEWAY_DEPLOY_DIR=/opt/kidpal/xiaozhi-esp32-server/main/xiaozhi-server`（或把 provider 声明放该层 config.yaml）；不改冻结测试。

### 待 human
1. **公网 0.0.0.0:8000 绑定**：human 本人明确「在 ECS 8.137.91.16 把 gateway front 绑公网 0.0.0.0:8000」→ verifier 复跑 wss 可达 + 鉴权三连。
2. **阿里云 ASR 凭据**（AliyunStreamASR appkey/token；或本地 SenseVoice model.pt）→ 引擎方能 boot 至 streaming 可用。
3. **token-plan qwen-audio 配额**（正确基址已定位，见 v23 evidence）：该套餐 1 周配额耗尽，重置 09-07 14:19 UTC → 重置后补 live ASR/TTS 实测 + 引擎 ASR provider 换型（OpenaiASR 指向 token-plan）拍板。

### 安全
- 设备 token 仅存 ECS `deploy/secrets.env`(600)；本仓 `git status` 无 key 泄漏（grep 零命中）。