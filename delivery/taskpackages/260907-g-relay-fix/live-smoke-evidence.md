# GRF live 冒烟证据 · G 包 relay 修复（GRF-A5 四事实 + GRF-E2）

> TaskPackage 260907-g-relay-fix · 2026-09-07 · 真机 ESP32-S3（WAVESHARE_S3_RLCD_4_2）⇄ wss://8.137.91.16:8000（front）⇄ 引擎 xiaozhi-esp32-server（engine-patch 部署副本 bind-mount）⇄ OpenClaw。
> 证据形态 = 栈自然留存物（设备串口摘录 / 引擎 docker logs 摘录 / 本证据 md），不复录 wire 字面——front↔引擎 ws 协商 permessage-deflate 致 wire 抓包不可读（排障口径 kidpal-live-smoke-debug skill 既载）。

## 背景

G 包 260903-gateway-directionB 的 live relay 缺陷（包 2 review-packet SHOULD #1，defect 轮 session `95ccf407-fdf7-497f-bd53-9d64d11faa6a`，见 `changes/dev/260829-voice-roundtrip/roundtrip-live-evidence.md`「新发现（G 包 live 面缺陷）」段）：引擎剥离未做 + front relay 断链——```display fence 混入播报文本、kidpal/display tools/call 从未送达设备、display text 视图（P3 翻屏语义）从未触发（用户报告「上一页看不到」即此因）。本包按方案 A（pod-events 决策 `engine-patch-approval`，human 批「引擎部署副本代码补丁」）修复：引擎侧单点剥离（`connection.py` 部署副本补丁 + `kidpal_display_strip` fence 状态机）为主，front 为机械校验 + relay 权威（`kidpal_display_blocks` 侧通道 → display_schema V27 → mcp 帧型 relay，display_relay wrapper 废止）。

2026-09-07 三轮真机冒烟：一轮（14:36，修复前缺陷态对照）、二轮（修复前最后一环断点对照，display_relay wrapper 帧被设备固件拒收）、三轮（wire 形态修复后验证，14:37:35 / 14:37:46 / 14:39:05 三次设备建连）。

## ① 引擎补丁标记行（GRF-A5⓪：补丁真在跑）

docker logs xiaozhi-esp32-server 原文摘录三处（连接级日志，每次设备建连打印一次）：

- `260907 14:37:35 … [core.connection]-INFO-kidpal display-strip patch active`
- `260907 14:37:46 … [core.connection]-INFO-kidpal display-strip patch active`
- `260907 14:39:05 … [core.connection]-INFO-kidpal display-strip patch active`（session `SiQwOpEdnonoOp`）

三轮建连三次打印同一标记行 = 部署副本补丁在容器内真实运行（非陈旧容器假绿）。

## ② session id + fence 双面清零（GRF-A5①）

- 引擎侧会话：`SiQwOpEdnonoOp`；设备侧 ws 会话（串口逐字，`c:/tmp/g_relay_fix_smoke_5.log` 重连段）：`WS: Session ID: c7f45056-4a27-4266-889c-0d9cc3d0ae5e`。
- **播报面清零**：串口捕获（`c:/tmp/g_relay_fix_smoke_5.log`，约 240s 量级捕获窗，设备时间轴 440s–728s）内 `Application: <<` 行全部为自然文本，零 fence 字样、零 display JSON 尾段。story《偷月亮的狐狸》轮（引擎 docker logs 14:39:32「发送第一段语音」，session `SiQwOpEdnonoOp`）逐字例证：
  - `I (451677) Application: << 好呀，那我给你讲一个新故事，名字叫《偷月亮的狐狸》。从前有只小狐狸，它觉得月亮太亮了，亮得它夜里睡不着，就悄悄爬上山顶，想把月亮摘下来藏进树洞里`
  - `I (588457) Application: << 我正接着讲《偷月亮的狐狸》呢——月亮给小狐狸讲了一颗小星星的故事，听着听着小狐狸就要睡着啦`

  引擎侧「发送第一段语音」文本零 fence（14:39:32 轮）。对照缺陷态一轮（14:36:39）：引擎 TTS 念出裸 ```display 开头与 `text":"…}}` JSON 尾段、串口帧尾截断——修复后播报面实证 fence 清零。
- **屏幕面清零**：屏幕文字面与播报面走同一剥离后输出流（引擎单点剥离：TTS delta 与 response_message 过同一状态机；front 纵深防御二次兜底），上述串口 `Application: <<` 同步文字即屏幕 chat 面渲染源——屏幕面同证零 fence 残留、零 display JSON 混入。

## ③ kidpal/display tools/call 实际下发且设备接受（GRF-A5②）

下发的 wire 帧无自然日志可摘（排障口径：front 只记连接事件、设备 mcp 成功路径不打日志、front↔引擎 ws 协商 permessage-deflate 致 wire 抓包不可读——kidpal-live-smoke-debug skill 既载），下发证明链三段：

- (a) **串口零拒收**：三轮全程（含 240s 串口捕获窗）设备侧零 MCP 拒收记录——无未知消息类型（Unknown-message-type 形态）拒收、无无效 id（Invalid-id 形态）拒收。对照二轮实证拒收形态：display_relay wrapper 帧被设备固件按未知消息类型丢弃（application.cc:532 丢弃一切非 mcp 类型；该行串口逐字记录见冻结验收 `server/acceptance/test_g_relay_fix.acceptance.py` docstring「TRANSPORT OF RECORD」段——GRF-A5② 齿断言本证据语料零拒收字面，故此处不复录该行），wire 形态修复后该断点证消。
- (b) **离线 harness 全链 assert**（GRF-A3/A4 齿，standalone runner 全绿）：引擎状态机产出 → `kidpal_display_blocks` 侧通道帧 → front 截获（不透传设备）→ display_schema 校验（脏块降级友好 text 视图，不静默）→ 每块一信封以 `{"type":"mcp","payload":<tools/call>}` mcp 帧逐帧下发、numeric id（mcp_server.cc:473 cJSON_IsNumber）、data 字符串化（:105 双 String 属性）；设备渲染回执 mcp 帧解包后经 relay_ack 消费（回执闭环）。
- (c) **下发效果目击** = ④ text 视图弹出。

## ④ text 视图触发 + 翻屏语义（GRF-A5③）

- human 目击 story 轮 **text 视图弹出**——display text 视图面渲染成功即 kidpal/display tools/call 送达并被设备 McpServer 执行的直接效果；「上一页看不到」根因（relay 断链、视图从未触发）消除。
- 翻页按键 = **BOOT 键**（`firmware/main/key_gesture_parser.h:14`：「职责分离：KEY 主司会话（短按启动/结束会话、长按回首页）、BOOT 主司翻页（短按 P1→P2→P3→P4 循环）」；:42 四页循环页 id 定义）。
- **诚实边界**：跨页渲染端到端整合归包 9b（`key_gesture_parser.h:42` 注记原文：「跨页渲染/端到端整合由包 9b 兜底，SES-E2」）——本包证据记录翻屏语义承载现状（text 视图触发成立），BOOT 翻页的跨页端到端验证未在本轮执行，不虚报已验翻页。
