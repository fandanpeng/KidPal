# Device Round Record — 260915-m1-k1a-key-layer-dispatch（KD-A5 真机确认轮）

> **状态：PASS（r3，2026-09-15）**。human 持板轮已完成，三行为 PASS 由 human 口述裁定（pod-events `83ddeb08`）；本件 = 证据互证记录。

## 执行事实

- 烧录：planner 经 esptool `write_flash 0x20000 xiaozhi.bin`（COM5，460800，app 分区单烧、NVS 保留），r1 与 r3 各一次，均 `Hash of data verified` + 硬复位；r3 固件 = 修复版（fixer round 1 级联补丁后整编产物）。
- 持板：human；捕获：planner 被动串口窗（300s，COM5 115200，无复位）；裁定：human 口述 + 日志互证，PASS 归 human。

## 轮次记录

- **r1**（`device-round-keylayer-r1.log`）：① 路由半边正确（BOOT → action=4 OVERLAY_CLOSE → 关层回原页不翻页）但**会话未级联结束**（状态机滞留 listening，human 仍可对话）；②③ PASS。human 裁定「修复后重跑」（pod-events `bf04686f`）→ fixer round 1（板级 OVERLAY_CLOSE case 补 VOICE 覆盖层 → ToggleChatState 级联 + `front_is_voice` 判据 + 回归锁齿）。
- **r2**（`device-round-keylayer-r2-gateway-flap.log`）：异常轮——**定性 = 网关间歇 TLS 握手超时**（`wss://8.137.91.16:8000` code=0x8006 连续失败后重试连上；ECS front 侧健康、19:17:05 连接在案）。r2 中关层路由 action=4 零触发、fixer 补丁未参与链路，**非补丁回归**。连接失败兜底（`voice_end` 弹层）后重连成功、会话隐形存活无覆盖层 = 既有重连行为与层模型的错位，owner 归会话逻辑面（非本包 seam），登记 review-packet 备查。
- **r3**（`device-round-keylayer-r3.log`）：复位干净态后重跑，**三行为全 PASS**：
  1. 覆盖层 BOOT = `action=4 → dismiss_front_overlay + listening→idle 同毫秒`（95632 / 233382 / 251422 三处实证，关层 + 结束会话 + 回原页不翻页）；
  2. 主页面 BOOT = `action=3`（PAGE_NEXT）翻页正常；
  3. KEY 长按 = `action=2 → show_home` 回首页正常（覆盖层内实例见 r1/r3 行为①同径）。

## 门口径

- KD-A5 三行为 PASS（human 裁，pod-events `83ddeb08` 锚定）；fixer 修复有效性终判 = r3 日志级联三处实证。
- 本包 structural 义务全部闭合，无后置项、无风险接受项。
