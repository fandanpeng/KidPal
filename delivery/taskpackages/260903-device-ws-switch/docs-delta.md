# docs-delta — 260903-device-ws-switch（设备切 WS + 方向 B 冒烟，tracer-bullet 设备片）

> 性质：方向 B 重切后的设备侧切换包（subsystem=firmware+server，riskTier=high，mergeOrder=4，
> dependsOn=260903-gateway-directionB）。把设备从 Pod S 的同步 HTTP shim 形态切到
> 「配网直写 NVS + 自签证书 pinning + WS 握手 Bearer + 原生 WS+Opus 流式」，并退役 HTTP 台阶。
> 本文件记录 docs-sync 的 **dist（哪些已改）** 与 **处置（哪些由 planner/主线程/契约门承接，本包不动手）**。
> 边界注：integrator 的 scope-map 写面 = `firmware/** server/** docs/systems/** docs/templates/**
> docs/ARCH-REVIEW-QUEUE.md changes/dev/** .claude/taskpackages/** .planning/**`——
> `docs/notes/**`、`docs/spec.md`、`docs/contracts/**` 不在写面，相关项如实留给主线程（见 C / F 段）。

## A. 已改（本包落地）

| 目标 | 位置 | 改动 | 理由 |
|---|---|---|---|
| WS 协议直选 + 读缝保留 | `firmware/main/protocols/websocket_protocol.cc` | 原生 WS 协议读 NVS `websocket` 命名空间（url/token/version），建连走自签 CA pinning | WSA-A1/A3 |
| 配网直写入口（新增） | `firmware/main/provisioning/ws_provision.{cc,h}` + `ws_policy.h` | 非 ota.cc 的配网直写：写 url/token/version + 短路激活；token 由 CSPRNG `esp_fill_random` 生成（24 字节 = 192-bit，决策 22 ≥128-bit 双满足），不嵌真实 token | WSA-A2 |
| 自签 CA 嵌入 + 加载（新增） | `firmware/main/certs/kidpal_ca.pem` + `ws_tls_ca.{cc,h}` | 自签 CA（公钥证书块，无私钥）经 `CONFIG_MBEDTLS_CUSTOM_CERTIFICATE_BUNDLE_PATH` 收进 esp_crt_bundle 信任面（V10，自签 pinning） | WSA-A3 |
| 协议切换 + 激活门短路 | `firmware/main/application.{cc,h}` | 移除 `CONFIG_USE_SHIM_PROTOCOL` 分支与 `ShimProtocol`/`ota_->Activate()`/`ShowActivationCode`（tenclass 激活环），直选 WebsocketProtocol + 调 `WsProvision::EnsureWebsocketToken()` | WSA-A1/A2/A5 |
| HTTP 台阶死代码消除（删） | 删 `firmware/main/protocols/shim_protocol.{cc,h}`、`firmware/main/audio/mp3_decoder.{cc,h}`；`audio/audio_service.{cc,h}` 去 `PlayMp3`；`CMakeLists.txt`/`idf_component.yml`/`Kconfig.projbuild` 去 `shim_protocol`/`mp3_decoder`/`USE_SHIM_PROTOCOL`/`SHIM_*` | 同步 HTTP  shim 链路 + mp3 下行 + 4 个 Kconfig 项整体退役；tenclass/xiaozhi.me 激活门字符串归零 | WSA-A5 |
| 设备侧冻结验收（新增） | `firmware/main/test/device_ws_switch.acceptance.c` | WSA-A1..A7 + boundary/security 齿，host_app linux-target ELF 跑（HOST_TEST_CONVENTIONS C4.4） | WSA-A1..A7 |
| 单元测试（新增） | `firmware/main/test/ws_provision.test.cpp` | ws_policy HexEncode / IsValidWsUrl / token 生成形状 | 单测面 |
| 验收编入闸 runner | `firmware/main/test/host_app/main/CMakeLists.txt` | `HOST_ACCEPTANCE_SRCS` 加 `device_ws_switch.acceptance.c` + `ws_provision.test.cpp`（builder 接线 C4.2） | E1 |
| 方向 B 网关 live 适配 | `server/gateway/front/front_app.py` | websockets v17 兼容 import（`websockets.asyncio.client` / `additional_headers`，legacy 退 `extra_headers`）+ 设备白名单头透传（authorization/device-id/client-id/protocol-version，token 不入日志） | 方向 B 首链串通 |
| 引擎部署 live 重切 | `server/deploy/docker-compose.yml` | 改 `network_mode: host`（引擎自绑 127.0.0.1:8010、经真 loopback 达 OpenClaw 18789；去 host-gateway/extra_hosts/ports 网桥）；healthcheck 改探 `127.0.0.1:8010` | live 实证必要（见 E 段偏差②） |
| provider 配置 live 修正 | `server/deploy/provider.config.yaml` | LLM `base_url` 补 `/v1`（缺则引擎 404）；TTS `provider` 由 `edge-tts`→`edge`（须匹配引擎 provider 模块文件名） | live 实证必要（见 E 段偏差） |
| shim.py 归档退役 | `server/shim.py` | 头加 ARCHIVED 墓碑注记（代码逻辑零改动，`py_compile` 通过）；承接指针 = 方向 B 网关 | WSA-A6 |
| 部署文档 docs-sync | `server/deploy/README.md` | 同步 network_mode: host 后的真实拓扑（引擎自绑 8010、真 loopback 达 OpenClaw，去 host.docker.internal 网桥表述）——本包 live 重切引入的文档漂移，本包收口 | docs-sync（本包引入的漂移） |
| 冻结验收 supersede 注记 | `server/acceptance/test_cloud_link.acceptance.py` + `test_smoke_e2e.acceptance.py` | 头注 SUPERSEDED（superseded-by=260903-device-ws-switch，coverage WSA-A7）；断言语义零变更、测试 id 与 coverage map 不动 | WSA-A7（acceptance-author 执行） |
| Pod 证据 + closeout | `.claude/taskpackages/260903-device-ws-switch/`（taskpackage/pod-events/verify-runs/verifier-evidence r1-r4/red-baseline-summary）+ `changes/dev/260903-device-ws-switch/` | 执行 ledger + 承重 round fact + 收口制品 | 归档 |

## B. docs-sync 判定（docsNoChangeReason，已生效）

- `docsSyncRequired` 未声明，TaskPackage 以 `docsNoChangeReason` 生效、docs-sync 闸（r4）PASS。
- docRefs 全为本包**只读消费的上游冻结权威**：`.planning/kidpal-mvp/PROPOSAL.md`（受 `upstreamFrozen sha1:88a5bdb83abdd867` pin，禁改防 UPSTREAM_DRIFT）、`docs/spec.md#§3.0.0`/`#§6.3`、`docs/contracts/interface-contract.md#§四`、`changes/arch/260902-arch-review-kidpal-spec-v0.2/dev-loop-handoff.json`。V10→WSA-A3 等决策映射由 `decisionCoverage.landsIn` 承载，非 docRefs 内容职责。
- **canonical spec 已先行冻结方向 B**：`docs/spec.md` §3.0.0「迁移与既有资产」（HTTP 链路为台阶、方向 B 落地后退役）、§9 决策 9/18（方向 B 转官方小智服务端网关、HTTP shim 退役）、§9 端云通信（WebSocket + Opus 流式）均先于本包存在。本包是把已冻结的方向 B 落到设备侧——**spec 无漂移、不触发 spec 改动**（PROPOSAL 非目标条款）。

## C. 笔记面检查（docs-sync note surface）

- `docs/notes/feature-inventory.md` OC-7 行仍描述「shim 薄桥（FastAPI /v1/asr /v1/chat /v1/tts /v1/alerts）已建（TP-A）✅ 全路径跑通」——本包已退役该 HTTP shim（`server/shim.py` 归档 + ECS 停服），OC-7 行已过时。
- **处置**：`docs/notes/**` 不在 integrator 写面 → 留**主线程**更新 OC-7 行（标注 shim 已退役、承接 = 方向 B 网关 `server/gateway/`），并把「设备切 WS（配网直写 NVS + 自签 pinning + WS 握手）」补进设备侧能力清单。本包如实登记、不越面直改。

## D. ADR 落点说明

- TaskPackage **无 `adrRequired`**，且本包架构裁决（方向 B WS 切换、HTTP 台阶退役、自签 pinning、配网直写 NVS、设备弱 token 鉴权）全部已在上游冻结：PROPOSAL v0.4 + arch-review handoff（V10）+ spec §6.3/§3.0.0/决策 22。`decisionCoverage` 五条把 V10/spec-§6.3/决策22/§3.0.0/OQ-R4 逐一映射到 acceptance（无 `dropped`）。
- 故**不新增 ADR**——why 已由上游冻结件承载，本包无新增架构裁权。

## E. 须如实记录的 live 偏差与验收修订（reviewer 必问）

### E.1 WSA-A4 证据形态修订（human 授权「验收改认已有证据」）

- 冻结验收 WSA-A4 齿 3 原要求**字面 wire 字符串** `stt`/`tts`/`sentence_start`（契约 §四）；但引擎↔front 的 WS 腿协商 **permessage-deflate**，这些帧在线上被 DEFLATE 压缩、任何日志里都不出现字面量——钉死字面量会逼一次纯为字符串匹配的生产日志改动。
- human 授权把 A4 改认**本栈真实输出的标记**：session_id 保持字面；ASR 事实 ← `识别文本`/`>>`/`stt`；TTS 事实 ← `语音生成成功`/`发送第一段语音`/`tts`；流式事实 ← `SentenceType`/`发送第一段语音`/`sentence_start`。**四个协议事实仍全为强制**，由真实拼装的 `smoke-roundtrip.log` 实证。
- 这是**证据形态的改革，非严格度的放松**；由 acceptance-author 落（修订记录见 `device_ws_switch.acceptance.c` 头注 2026-09-06 行，源 = 真实 2026-09-06 冒烟 engine session `40024357-2e41-4bb4-9ae9-7f07327e3295`）。integrator 未触碰验收文件。

### E.2 live 相对原计划的偏差（均必要、留 retro）

| # | 偏差 | 原因 |
|---|---|---|
| ① | 引擎 `Intent: nointent`（工具调用关闭） | tracer-bullet 冒烟：OpenClaw 拒绝 tools 数组（`invalid tool configuration`），为串通首链先关意图/工具 |
| ② | ASR 用 `qwen3_asr_flash` + 用户 dashscope key（非 V23 终选 `aliyun_stream`） | G 包始终未取得 ALIYUN 凭据；用已有 dashscope key 跑通 |
| ③ | TTS `type: edge` | provider 模块文件名须为 `edge`（`edge-tts.py` 不存在 → 「不支持的TTS类型」） |
| ④ | 引擎容器 `network_mode: host` | 达 loopback 18789 上的 OpenClaw（原 host-gateway 网桥与 V28 loopback-only 锁互相矛盾） |
| ⑤ | front `websockets v17` 兼容 import + 设备头透传（device-id 等） | 引擎 `websocket_server.py:83` 要求 device-id；v17 改 `additional_headers` |

### E.3 假阳性教训（留 retro）

- 早前一个 NVS「patch version→1」脚本 Gate 在**错误的页魔数**（`\xfe\x83` vs 真实 `\xfe\xff`）上，静默 no-op；version=1 实际靠后续一次烧录才落。**已记入 ops skills**——闸的判据务必对真实载体（魔数/指纹）核验，杜绝「脚本跑过就当生效」。

## F. 无漂移确认

- `docs/contracts/interface-contract.md` 本包**只读引用**（`contractRefs`），零写；设备 WS 切换不改任何契约定义（笔顺 JSON / display 统一响应块 / 课表 MD / shim API 面未动）。
- 无新增 `docs/` 根文件、无影子目录（根级 `contracts/**` 零触碰）。
- 证据与报告落 `changes/dev/260903-device-ws-switch/` 与 `.claude/taskpackages/260903-device-ws-switch/`，不散写。
- `server/acceptance/_artifacts/device-ws-switch/`（flash.log / provision.log / smoke-roundtrip.log / screen1.jpg / ecs-teardown-evidence.md）按 `server/acceptance/.gitignore` 的 `_artifacts/` 规则**本地-only、by design 不入库**——WSA-A4/A6 证据齿读该目录（env `WSA_ARTIFACT_DIR` 可覆写），未 force-add。
