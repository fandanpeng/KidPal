# 真机轮证据 · 261010-device-reconnect-on-server-close（2026-10-11，human 持板在场）

> 主线程代记（human 真机轮现场，11:39–11:52 本地）：v9 固件烧录（app 分区 @0x20000，hash verified）+ v9 prompt 已部署 ECS（同轮）。日志源 = 设备串口（serial_passive/tight）+ front journalctl + 引擎 docker logs 三源对账。设备于 11:47:3x 因串口捕获器关闭触发 USB-Serial-JTAG 复位重启一次（观测操作副产物，非缺陷；重启后的行为链反而完整复刻了 boot-即连 → 常驻 → 会话 → 收尾重建全路径）。

## 四场景 + PN-A5 判定

| 齿 | 判定 | 证据（三源时间戳） |
|---|---|---|
| RC-A4 服务端断自发恢复 | **PASS** | 11:46:04 重启 front（设备直连对端）→ 串口 `Websocket disconnected`（uptime 265615ms，0.1s 内察觉）→ 首试撞 front 未起（reset by peer）→ **5.07s 后第二次尝试成功**（TryStartAutoConnect 退避首档 {5,10,20} 实证）→ front `connection open` 11:46:12 → 引擎腿同拍重建（hello 11:46:12、8010 ESTAB）；全程 P1 屏显零 SetDeviceState 搅动 |
| RC-B2 会话收尾后常驻重建 | **PASS** | 会话收尾（第二轮答复毕 11:51:47）→ 串口 `Websocket disconnected` + `listening→idle`（253985ms）→ **259005ms 重连（Δ=5.02s = T 首档 5s 分毫不差）** → 260925ms AutoConnect task exit（成功）；boot 后 11:47:41 建连 → 会话开启前连接持续在场 |
| RC-B4 静置常驻（本日段） | **PASS** | 设备静置 >3.5 分钟：引擎侧零「连接超时/结束对话/超时检查」事件（决策 21 键生效，120s 主动断连已根除）；串口仅 1 次断连记录（= 我们自己触发的 front 重启）；`ss` 实证 front:8000 ⇄ 设备 ESTAB 持续在场。**跨夜段（过夜提醒可达）留长观察，随天补录** |
| RC-B5 常驻下会话回归 | **PASS（附行为注记①）** | 两轮会话在常驻在线状态下全链正常（KEY → 聆听 → ASR「明天有什么课？」11:51:03 → v9 纪律答复 → 第二轮闲聊正常收尾）；键帽/状态机时序全对账 |
| PN-A5（v9 反叙述，同轮顺带） | **PASS** | 11:51 轮 TTS 逐句：「明天是周一」「星玥的课是这样的」「学校里」「第一节语文…第七节还是足球」「课外没有安排」「明天要带的东西备注里没写」——**零回合判定/文件路径/流程步骤独白**（10-10 泄漏形态完全消失，与 9-26 干净轮同形）；引擎无告别语 prompt 触发记录 |

## 行为注记（交 pod/architect 复核，不阻断）

1. **会话启动的「复用」只在 IsTimeout 120s 窗口内成立**：application.cc:885-891 会话启动按 `IsAudioChannelOpened()` 判复用，但 websocket_protocol.cc:77-79 的 IsTimeout 120s 覆盖语义使常驻连接静置 >120s 后被判「未开」→ 长静置后首按 KEY 会先关旧连接再开新连接（实测：断→连 20ms 序列，会话功能零损）。**常驻在线承诺未破**（提醒推送整窗可达——front 按 TCP 事实投递，不读 IsTimeout）；仅「会话零重连开销」只在 120s 窗口内兑现。候选后续：常驻连接的 IsTimeout 语义豁免或会话启动判据改用 TCP 事实（小批）。
2. **front 半开代理缺陷（新发现，独立缺陷已登台账 TECH-261011）**：引擎死但设备空闲无流量时，front 代理泵不收场（pump_dn 阻塞等设备帧、engine 腿已死、gather 永不齐）→ 设备腿挂着、设备全程不知情、alert legs 仍登记指向死引擎腿 → 下次提醒注入必失败（10-10 16:46 `alert_speech_injection_failed` 同机制）。RC-A4 本轮用 front 重启触发（设备直连对端必死）；引擎重启在空闲态不构成设备可感断连。

## 遗留观测（长窗）

- RC-B4 跨夜段：过夜静置后次日提醒可达性（front 无 `alert_skip no_online_device` + 设备渲染回执）——随天补录。
- RC-B6 modem 档位与 keepalive 长稳观测：LOW_POWER 档下常驻连接已实证 >3.5 分钟稳定（本轮即 LOW_POWER 剖面）；长稳结论（ping/pong 过夜）随天补录，倾向冻结「Idle 常驻 = LOW_POWER 档」。
