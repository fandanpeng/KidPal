---
id: TECH-261014-autoconnect-gives-up-silently
title: 自动重连任务固定 3 次尝试后静默退出——服务端重启/断网窗口超过约 15 秒即设备离线且不自愈，须用户动作（KEY/唤醒）才重建连；后果：离线期间提醒推送（MCP 下发）全丢
domain: tech
status: deferred
severity: SHOULD
owner: dev-loop（固件面）
trigger: 下一次动 application.cc 连接/重连面的批顺带评估；或排期后的可靠性小包（判据 = 是否要求「持续重试到连上」的产品口径先定）
opened: 2026-10-14
paths: ["firmware/main/application.cc"]
source: 2026-10-14 真机联测（主线程重启 kidpal-front 造服务端侧断，/c/tmp/live-261014-round2-r2.log：三次 `WS: Connecting` 均 `Connection reset by peer` → `AutoConnect task exit` 后零重试，设备离线约 2 分钟直至 human 按 KEY 才重建连成功，`WS: Session ID: 5a255d44-...`）
handoff: 独立小项；判「是否为缺陷」需先定产品口径（K6 原设计是否即「3 次即止、其余靠用户动作」）
---

**现象（2026-10-14 真机联测发现，非本日 261014 包引入）**：主线程重启 `kidpal-front.service` 制造服务端侧断连，设备正确识别为服务端侧断并启动自动重连（`AutoConnect` task 起），但 **3 次尝试（退避 5s/10s）全部落在 front 重启窗口内失败后，任务直接退出、此后零重试**——设备静默离线约 2 分钟，直到 human 按 KEY 触发 `OpenAudioChannel()` 才重建连成功。

**代码锚**：`firmware/main/application.cc:527-541` — `for (int attempt = 0; attempt < 3; attempt++) { ... if (attempt < 2) vTaskDelay(kAutoConnectBackoffS[attempt] * 1000); }`，退避表 `:77 kAutoConnectBackoffS[3] = {5, 10, 20}`，循环结束即 `AutoConnect task exit`（:541）。触发点 = `:682`（`OnAudioChannelClosed` lambda 内 `TryStartAutoConnect()`）与 `:423/:477`（网络事件、boot 自动建连）。

**影响面**：

- **可靠性**：服务端短时重启、家庭网络瞬断等超过约 15 秒的窗口 → 设备离线不自愈；孩子/家长下次用之前不会恢复（语音问话时才走 `HandleWakeWordDetectedEvent` → `OpenAudioChannel()` 建连，可用但用户感知为「喊了没反应一次」）。
- **提醒必达性**：设备离线期间，front 经 MCP 下发的考试/上课提醒（`kidpal/alert`）**全部丢失**（无重投、无补发）——这是本条目最重的后果，直接冲 S-F1.4 提醒必达语义。2026-10-14 实测窗口恰好印证：设备离线期间提醒推送无面。
- 该行为**不是** 2026-10-14 `261014-voice-round-entry-exit-fixes` 引入——该包 A2 只改 `websocket_protocol.cc` 的旧通道拆除分类，自动重连逻辑（K6 批 260915 建）逐字未动；真机第 4 项回归已验「合法服务端断仍被正确识别并触发重连」，A2 无误伤。

**修法方向（归包计划期裁，含产品口径取舍）**：① 拉长/改为持续重试（如指数退避封顶后按固定间隔长期重试，或至少重试次数与时长覆盖常见服务端重启窗口）；② 保留短预算但补「周期性兜底重试」（如每分钟一次），避免与省电冲突；③ 维持现状 + 改提醒链路（离线留待补发）——③ 面更大、不推荐。取舍点 = 重试频率 vs 功耗，以及"离线期间提醒补发"要不要另做。

**风险**：riskTier 预估 low-medium——纯固件侧、改动面小（重试预算/策略），但需真机验证功耗与重连风暴（多次失败下的退避行为），且判「是否缺陷」依赖产品口径先定。
