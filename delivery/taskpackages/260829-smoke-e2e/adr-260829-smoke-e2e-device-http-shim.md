# ADR — 设备侧弃小智 websocket/mqtt、改用最小同步 HTTP shim 协议（260829-smoke-e2e）

> 状态：已落地（tracer bullet 冒烟包 S，`changeId=260829-smoke-e2e`，subsystem=firmware+server）。
> 触发：spec §3.0.0 已定「MVP = 同步 HTTP（device→shim→OpenClaw 三层链路）」，本 ADR 记录该架构在**设备侧**的实现形态与取舍（架构裁决在 spec 治理层已定，此处只记实现事实）。
> 落点注记：`adrRequired=true`，原拟落 `docs/notes/`，但该目录不在 integrator 的 scope-map 写面（enforce-scope 实测拦截，与 TP-A 先例一致），故归档于 `changes/dev/260829-smoke-e2e/`。若需晋升到 `docs/notes/`（经验/策略类文档面），请主线程迁移，正文可整体搬用。

## 背景

- 小智基座把端云通信抽象成 `Protocol` 基类，默认两套流式实现：`WebsocketProtocol`、`MqttProtocol`（音频 Opus 帧，24kHz 下行）。
- KidPal MVP 为三层链路串通（KEY→麦克风→shim→OpenClaw→TTS→喇叭），spec §3.0.0 裁决：设备侧**不**沿用小智 websocket/mqtt 流式协议，改用**最小同步 HTTP shim 协议**（请求/响应，一次会话 = 一段完整音频上/下行），复用验证批已并入 main 的 `server/shim.py` 四端点。

## 决策

设备侧新增 `firmware/main/protocols/shim_protocol.{cc,h}`（`ShimProtocol : public Protocol`），实现同步 HTTP 请求/响应闭环，取代默认的小智流式协议。此为本包的 tracer-bullet 设备腿（`SMK-A1..A4`）。

## 实现形态（逐点）

1. **协议分支（编译期）**：`application.cc` `InitializeProtocol()` 用 `#if CONFIG_USE_SHIM_PROTOCOL` 分支——开启时实例化 `ShimProtocol`，否则回退既有 MQTT/Websocket 选择逻辑。协议选择层只落一个 Kconfig 编织位，不侵入状态机其余部分。

2. **mp3 解码选型**：下行 TTS 是 edge-tts 产物 MP3（小智音频管线 Opus-only）。新增 `firmware/main/audio/mp3_decoder.{cc,h}`，封装 `esp-libhelix-mp3`（`chmorgan/esp-libhelix-mp3 ^1.0.3`，Helix 定点解码），把 `/v1/chat` 返回的 base64 mp3 解成 int16 PCM 送喇叭 codec。`idf_component.yml` 追加该依赖。

3. **配网载体**：Kconfig（`USE_SHIM_PROTOCOL` / `SHIM_BASE` / `SHIM_DEVICE_TOKEN` / `SHIM_CAPTURE_MS`）承载编译期配置；运行时 `Settings("shim")` NVS 命名空间（`shim_base` / `device_token`）覆盖 Kconfig 缺省。设备 token 不硬编码进固件（安全轴 secrets-not-in-firmware）。

4. **上行攒帧组 WAV**：`ShimProtocol::SendAudio` 收 Opus 帧、攒满 `SHIM_CAPTURE_MS`（默认 3000ms）窗口后 `FinalizeSession`——Opus 解码回 WAV（PCM16 16kHz mono）→ base64 → `POST /v1/asr {audio}`。

5. **下行闭环**：`/v1/asr` → `{text}` 空则回 idle（屏显「shim: empty asr text」占位）；非空 → `POST /v1/chat {text}` → `{audio(base64 mp3), display}` → `PlayMp3` 出声 + `tts sentence_start` 上屏。

6. **K6 cid 透传**：每次 HTTP 请求带 `X-Correlation-Id`（`esp_random` 生成的 32-hex）+ `Device-Id`（MAC）+ `Client-Id`（UUID），与 server shim 的 correlationId 链路对齐。

7. **激活门编译期旁路 + 死代码消除**：`ActivationTask` 中 `CheckNewVersion()`（官方 OTA 激活，api.tenclass.net 重试）用 `#if !CONFIG_USE_SHIM_PROTOCOL` 旁路——shim tracer bullet 无官方后端，设备须直达 `InitializeProtocol()`；旁路后 tenclass/xiaozhi.me 字符串在镜像级归零（`SMK-A2−` 禁词守卫 + 死代码消除实证）。

## 取舍与后果

- **同步 HTTP 换流式**：整段音频一次性上/下行，无流式 ASR/TTS 增量；MVP 段可接受（spec §3.0.0 已定），M2 若需流式再议。
- **新增 mp3 解码路径**：下行从「Opus」变为「MP3」，代价是设备端多一个 Helix 定点解码器；收益是与 edge-tts 产物直通、无需服务端转 Opus。
- **上行 Opus→WAV 转换**：设备采集仍走小智 Opus 管线，`FinalizeSession` 解码回 WAV 再送 faster-whisper（PCM16 16kHz mono 可解）。

## 安全关联

- 设备只触达 shim（禁 18789/openclaw 直连，`SMK-A2` 守卫 + 语料零命中）；设备 token 鉴权走 shim `require_device`（Bearer）；OpenClaw token 仅 ECS 本机 loopback 转发（对偶 TP-A finding #1）。
- 未测面（声学往返）豁免见 review-packet §未测面与特殊放行声明。