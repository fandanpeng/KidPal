---
id: TECH-261010-device-no-reconnect-on-server-close
title: 固件对「服务端主动断连」无自动重连——OnAudioChannelClosed 只进低功耗回 Idle，断线自动重连仅覆盖网络重连事件路径
domain: tech
status: closed
severity: SHOULD
owner: dev-loop（固件 WS 常驻/重连批）
trigger: 下个动 websocket_protocol / application 会话生命周期的包；与 arch §8「闲置约 3 分钟休眠断连」待裁产品决策（同一推送可达性问题面）合并裁定
opened: 2026-10-10
closed: 2026-10-12
closedBy: 主线程（2026-10-12 tech 域 CRQ 盘点，human 指示；证据见各条目收口节）
paths: ["firmware/main/application.cc", "firmware/main/protocols/websocket_protocol.cc"]
source: 2026-10-10 真机使用反馈（设备重启后 WS 没连上、通知推不下来）+ 引擎/front 日志时间窗对账
---

**定性：真 bug（固件缺口），非需求缺口。** requirements §2 错误兜底行明文「断线自动重连，屏幕显示不受影响」；arch §6.4「设备开机即连、断线自动重连」。

**证据（2026-10-10）**：① 固件侧 `application.cc:634-641` `OnAudioChannelClosed` 回调仅 `SetPowerSaveLevel(LOW_POWER) + SetDeviceState(Idle)`，无任何重连动作；唯一自动建连恢复路径 = 网络重连事件（K6 boot-auto-connect，`application.cc:407-408` 注释「网络重连后 Idle 且通道未开 → 自动建连恢复」——网络事件路径有恢复、服务端断连路径没有）。② 日志对账：引擎 16:52:43 主动关连接后，设备直到 17:05:25 才再次出现完整 hello 建连序列（约 13 分钟黑洞，期间 front 侧无任何重连尝试记录）——恢复时点与用户交互（按键/重启）吻合，非固件自发重连。

**影响面**：与 [[TECH-261010-engine-decision21-keys-not-persistent]] 叠加成用户可感症状「重启后 WS 没连上、通知推不下来」：引擎默认 120s 断连（云侧回归）+ 设备被断后不重连（本条）= 静置数分钟后设备即离线、提醒推送黑洞，直到孩子下一次交互。云侧止血（决策 21 键恢复）后，本条仍是残余面：任何服务端/网络侧断连（引擎重启、部署、网络抖动）都会让设备静默离线。另与 arch §8 已登记的「设备闲置约 3 分钟休眠断连 → alert_skip reason=no_online_device【产品决策待裁】」是同一可达性问题族——提醒必达承诺（V20 不对称必达）依赖常驻在线，三方应合并裁定。

**修法候选（dev-loop）**：`OnAudioChannelClosed` 加带退避的自动重连（指数退避 + 上限 + 屏显不受影响口径）；或与「设备休眠唤醒后重连」一揽子做（推送可达性专项）。**riskTier: medium**。依赖：云侧决策 21 键先恢复，否则重连补上了也只是高频重连-被断循环。

---

## 收口（2026-10-12 主线程盘点）

修法已随 261010-device-reconnect-on-server-close 批落地：`firmware/main/application.cc:671-675` OnAudioChannelClosed lambda 区新增 [S8] A 路径——非本地关（服务端/网络侧断）→ `TryStartAutoConnect()`（复用既有 AutoConnect task 守卫与退避，零第二条 task），协议层本地关分类标记 `local_close_pending_` 互斥在世（[S7]），RC-A1 验收齿冻结（`firmware/main/test/boot_auto_connect.acceptance.c`）。live 实证：261012 真机轮 781s 空闲通道超时（服务端侧断）后设备 2.2-4.7s 重建 WS、hello 成功（TECH-261012-first-keypress 证据链）。requirements「断线自动重连」兑现，关闭。残余面（重建后会话失同步 → 首按被吞）已由 TECH-261012-first-keypress-no-listening 独立承接。
