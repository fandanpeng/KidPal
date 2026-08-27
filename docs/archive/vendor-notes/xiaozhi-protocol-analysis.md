# 小智(XiaoZhi) 端云协议调研 → 接入 Aliyun+OpenClaw 方案

> 基于 codebase-memory index（项目 `xiaozhi-rlcd`，14341 节点）+ 源码精读。
> 目标：搞清楚小智如何与语音服务器通信，确定在哪些层"切开"接入我们自己的 Aliyun+OpenClaw 后端。

---

## 1. 协议层架构（关键：有干净的抽象层）

小智把端云通信抽象成 `Protocol` 基类（`main/protocols/protocol.h:44`），两套实现：
- `WebsocketProtocol`（`websocket_protocol.cc`）← **推荐我们用它，最适合自建网关**
- `MqttProtocol`（`mqtt_protocol.cc`，音频走 UDP，复杂）

```
        Application（状态机/业务）
              │ 只依赖 Protocol 抽象接口
              ▼
        ┌──────────────┐
        │  Protocol 基类 │  ← 抽象层，接入点在这
        └──────┬───────┘
       ┌───────┴────────┐
  WebsocketProtocol   MqttProtocol
```

**结论：我们不需要改 Application 状态机和音频链路，只需让 WebSocket 连到"我们的服务器"，并实现服务端那一半协议即可。**

---

## 2. Protocol 抽象接口（设备端能力面）

设备主动发送（设备→服务器）：
| 方法 | 含义 |
|---|---|
| `SendAudio(packet)` | 发送 Opus 音频帧（二进制） |
| `SendText(json)` | 发送 JSON 控制消息 |
| `SendWakeWordDetected(word)` | 唤醒词触发 |
| `SendStartListening(mode)` | 开始聆听 |
| `SendStopListening()` | 停止聆听 |
| `SendAbortSpeaking(reason)` | 打断/喊停 |
| `SendMcpMessage(msg)` | MCP 工具调用结果 |

设备接收（服务器→设备，通过回调）：
| 回调 | 含义 |
|---|---|
| `OnIncomingAudio(packet)` | 收到 TTS Opus 音频帧（播放） |
| `OnIncomingJson(root)` | 收到 JSON 控制消息（核心！） |
| `OnAudioChannelOpened/Closed` | 音频通道建立/断开 |
| `OnNetworkError` | 网络错误 |

---

## 3. WebSocket 握手协议

**连接**：WebSocket，请求头带鉴权与设备信息：
```
Authorization: Bearer <token>
Protocol-Version: <1|2|3>
Device-Id: <MAC>
Client-Id: <UUID>
```

**客户端 hello**（`GetHelloMessage`，`websocket_protocol.cc:202`）：
```json
{
  "type": "hello",
  "version": 1,
  "features": { "aec": true, "mcp": true },
  "transport": "websocket",
  "audio_params": { "format": "opus", "sample_rate": 16000, "channels": 1, "frame_duration": 60 }
}
```

**服务端 hello 应答**（`ParseServerHello`，`websocket_protocol.cc:227`）：
```json
{
  "type": "hello",
  "transport": "websocket",
  "session_id": "xxx",
  "audio_params": { "sample_rate": 24000, "frame_duration": 60 }
}
```

---

## 4. 音频二进制帧格式

`BinaryProtocol2`（version=2，带 timestamp 供服务端 AEC）：
```
version(u16) | type(u16: 0=OPUS,1=JSON) | reserved(u32) | timestamp(u32) | payload_size(u32) | payload[]
```
`BinaryProtocol3`（version=3，更精简）：`type(u8) | reserved(u8) | payload_size(u16) | payload[]`

- 音频格式：**Opus**，设备采集 16kHz 上传；服务端 TTS 默认 24kHz 下行。
- 上下行都是 Opus 帧，二进制传输。

---

## 5. 服务端→设备 JSON 消息类型（`application.cc:517` OnIncomingJson）

这是**我们服务端要实现的返回协议**：

| type | 字段 | 设备行为 |
|---|---|---|
| `tts` | `state: start` | 进入 Speaking 态 |
| `tts` | `state: stop` | 说完，回 Listening 或 Idle |
| `tts` | `state: sentence_start`, `text` | 显示 AI 回复文字上屏 |
| `stt` | `text` | 显示识别出的用户语音文字 |
| `llm` | `emotion` | 设置表情（可改造为显示意图/状态） |
| `mcp` | `payload` | MCP 工具调用（**可用来下发课表/查字结构化数据！**） |
| `system` | `command: reboot` | 重启（OTA 用） |
| `alert` | `status,message,emotion` | 弹提醒（**可做课表提醒！**） |
| `custom` | `payload` | 自定义消息（需开 CONFIG_RECEIVE_CUSTOM_MESSAGE） |

---

## 6. 接入 Aliyun+OpenClaw 的三种方案

### 方案 A：自建"小智协议网关"（推荐）
在 Aliyun 上实现一个**兼容小智 WebSocket 协议的服务**，设备固件几乎不改，只改服务器地址。

```
设备 ──小智WS协议──> [Aliyun 网关] ──> 内部转发
                        │ ① Opus→ASR(阿里云NLS)
                        │ ② 文字→OpenClaw+DeepSeek(意图/对话)
                        │ ③ 命中业务(课表/故事/查字)
                        │ ④ 回复→TTS(阿里云NLS)→Opus
                        │ ⑤ 按小智JSON协议回发设备(tts/stt/mcp/alert)
```
**优点**：设备端零改动（用现成小智固件逻辑），所有定制在服务端。
**这是最快路径，强烈推荐 MVP 用。**

### 方案 B：改设备端 Protocol，直连自定义协议
重写 `WebsocketProtocol` 连我们自己的精简协议。
**优点**：协议可按需裁剪；**缺点**：要改设备端、失去小智协议兼容，工作量大。

### 方案 C：用 MCP 通道承载业务数据
小智原生支持 MCP（Model Context Protocol）。课表/查字的结构化数据可通过 `mcp` 类型消息下发，设备端用 `McpServer` 处理。
**可与方案 A 叠加**：网关用 MCP 消息推送笔顺矢量、课表数据。

---

## 7. 对我们三大功能的协议映射（基于方案 A）

| 功能 | 上行（设备→云） | 下行（云→设备） |
|---|---|---|
| 课表查询 | 语音→Opus→ASR | `stt`(识别文字)+`tts`(语音答复) |
| 课表提醒 | —（云端定时触发） | `alert`(状态+文案)+`tts`(语音) |
| 讲故事 | 语音→Opus→ASR(长) | `stt`(文字)+`tts`(确认"已存好") |
| 查生字 | 语音→Opus→ASR("勇怎么写") | `stt`+`tts`(读音)+`mcp/custom`(笔顺矢量数据) |

**笔顺矢量数据**：走 `mcp` 或 `custom` 消息下发 JSON（含笔画路径），设备端解析后用 LVGL 绘制。**这是唯一需要在设备端新增解析逻辑的地方。**

---

## 8. 需要进一步确认的点

1. **鉴权 token 从哪来**：设备 `Authorization: Bearer <token>`，需确定我们网关的鉴权方式（设备绑定/家长账号）。
2. **OTA**：`system/reboot` + 独立 OTA 流程，二期再看。
3. **服务端 AEC**：hello 里 `features.aec` 表示服务端可做 AEC（用 BinaryProtocol2 的 timestamp）。我们 AEC 在本地做，此功能可关。
4. **小智默认服务器配置项**：找到设备端配置服务器 URL/token 的地方（settings/配置），改成指向我们网关。

---

## 9. 下一步行动（M1 前置）

- [ ] 定位设备端"服务器地址/token"配置入口（settings 或 NVS）
- [ ] 确认 WebSocket 版本号默认值（version_，1/2/3）
- [ ] 在 Aliyun 起一个最小 WebSocket 服务，能完成 hello 握手 + 回显 stt/tts
- [ ] 验证设备连我们网关完成一次「说话→收到 stt 文字」闭环
