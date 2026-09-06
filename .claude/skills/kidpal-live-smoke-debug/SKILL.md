---
name: kidpal-live-smoke-debug
description: Diagnose a KidPal direction-B live smoke round end-to-end (device ⇄ front ⇄ engine ⇄ OpenClaw) — the three log sources to correlate, a symptom→root-cause→fix table for the failures that actually happened, and why wire capture fails (permessage-deflate) so you use device/engine logs instead of touching the production front. Use when a live device conversation misbehaves or when capturing first-link evidence.
---

# KidPal 方向 B Live 冒烟排障

方向 B 链路（详拓扑见 [[kidpal-ecs-gateway]]）：设备 ⇄ `wss:8000` front ⇄ 引擎 `xiaozhi-esp32-server`(127.0.0.1:8010) ⇄ OpenClaw(127.0.0.1:18789)。一次冒烟要**同时**看三个日志源互相对齐时间窗：

| 源 | 怎么看 | 关键标记 |
|---|---|---|
| 设备串口 COM5 | [[kidpal-serial-monitor]] 的 `serial_passive.py` | `with version: N` / `>> STT` / `<< reply` / `payload_size invalid` |
| 引擎 docker | [[kidpal-ecs-gateway]] 的 `docker logs ... grep` | `识别文本` / `大模型收到` / `发送第一段语音` / `语音生成成功` / `invalid tool configuration` |
| front 服务 | `ssh_helper.py "journalctl -u kidpal-front --since -5m"` | 只记连接事件，**不记帧内容**（见下「别去抓线上报文」） |

「好的一轮」= 设备 `version:1` + `>>STT` + `<<reply`，引擎 `识别文本→大模型收到→发送第一段语音+语音生成成功`，无 `invalid tool configuration`、无 `payload_size invalid`、无崩溃重连。

## 症状 → 根因 → 处置（本轮实测全踩过）

- **设备上屏 STT 文字、却回「小智现在有点忙」** → 引擎日志有 `invalid tool configuration`：`Intent: function_call` 把 tools 数组塞给 OpenClaw，被其 OpenAI 兼容端点拒收 → 改 `selected_module.Intent: nointent` + `docker restart xiaozhi-esp32-server`（见 [[kidpal-ecs-gateway]]）。
- **连上但全程无声 / `bad_alloc` 崩溃 / `payload_size invalid` 刷屏** → 串口 `with version: 2`：NVS 的 ws 协议 version 错了（服务端发裸 opus，设备按 v2 头解）→ 刷成 i32 `1`（见 [[kidpal-nvs-ops]]）。
- **改了配置不生效 / 配置与行为对不上** → 你读的是 docker bind-mount 的**诱饵**配置；活配置在 `deploy/xiaozhi-docker-data/.config.yaml`（见 [[kidpal-ecs-gateway]] 的 config trap）。
- **LLM 404 / 连不上 OpenClaw** → `base_url` 没带 `/v1`，或容器拿不到 loopback 上的 OpenClaw（后者靠 `network_mode: host` 解）。

## 别去抓线上原始报文（permessage-deflate 这堵墙）

引擎↔front 这段 ws 协商了 **permessage-deflate**，stt/tts/hello 等文本帧在线上全被压成二进制，**只读 tcpdump 解不出明文**。所以：
- 取证用**设备串口 + 引擎日志**（它们各自打自己那侧的明文），不要为了「线上 wire 字面量」去动生产前置加 tap——不值，且前置只记连接事件、不记帧内容。
- 给验收选证据标记时，选**本栈自然会落盘**的形态（`识别文本`/`语音生成成功`/`SentenceType`/session_id），别选要为此动生产才能拿到的 wire 字面量（本会话 WSA-A4 即因此由 human 拍板改认已有证据）。

## tcpdump 两个坑（确需抓包时）

- stdout 重定向到文件是**块缓冲**——不加 `-l`（行缓冲）窗口期看不到任何东西。
- `pkill -f tcpdump` 会把正在执行的 bash 外壳也匹配杀掉（报 127）——用 `pkill -x tcpdump`。
- 抓包窗口要和「开始/好了」信号严格对齐（真机冒烟的协调成本，派发时讲明）。

Related: [[kidpal-ecs-gateway]] [[kidpal-serial-monitor]] [[kidpal-nvs-ops]]
