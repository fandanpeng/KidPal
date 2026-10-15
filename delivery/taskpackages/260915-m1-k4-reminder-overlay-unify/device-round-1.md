# RM-A8 真机确认轮 · 第 1 轮记录（2026-09-17 17:17–17:39）

> human 持板 + 主线程操作（烧录 / 串口 / ECS 网关日志三方证据互证）。
> 固件 = K4 收口 commit `ff93e39`（main tip `944f636` 同代码），全量烧录 COM5（ota_0 @ 0x20000），NVS 未动（url/token/ssid/version=1 读回核实在位）。
> 串口证据：c:/tmp/k4-boot2.log（reset-capture 全 boot）、c:/tmp/k4-demo.log、c:/tmp/k4-popup.log、c:/tmp/k4-popup2.log；网关证据：ECS journalctl kidpal-front（17:26–17:39 段）；演示档注入经 human AskUserQuestion 授权。

## 环境面结论（通过）

- 烧录后启动正常：0 RE-ENUM、uptime 平滑推进；WiFi `701` 连接（192.168.3.36）、激活完成、idle 进 home 页。
- WS 链路正常：唤醒后 `WS: Connecting ... with version: 1` → Session ID → 会话往返（ASR/LLM/TTS 全通）。
- 网关推送链路通：`alert_deliver delivered=True device_frames=1 speech=True`；设备侧 `MCP: alert_dispatch` → `MCP: alert_render raised=1 actions=7`。
- 三件套两件半：提示音 + 语音播报（speech 走引擎腿）可达；弹窗可见但见下 F2。

## 缺陷清单（按严重度）

### F1 · 渲染回执永不发出（根因已钉死）

- **现象**：网关 `alert_deliver delivered=True` 但 `acked=0`；60s 重推 ×3（17:31/32/33 与 17:38 各一轮）后按不追催口径静默作废；每次重推 speech 腿重播 TTS（用户实际听到两遍）。
- **根因**：`kidpal_reminder_mark_rendered` 生产代码**零调用点**（firmware/main 全仓 grep：仅 reminder_page.h 声明、reminder_page.c 实现、测试文件调用）。渲染路径 `McpServer::RaiseReminderQueue`（mcp_server.cc:319-376）画完弹窗直接 `EmitRenderReceipt`，但 `shown.rendered` 恒 0 → `kidpal_reminder_may_emit_receipt`（reminder_page.c:875-884）恒 0 → 回执静默 return。
- **为何 host 齿没抓住**：acceptance 齿自调 `mark_rendered` 桩级模拟渲染完成再验回执武装（reminder_overlay_unify.acceptance.c:811/859/944）——状态机单测绿，但「真实渲染路径未接 mark_rendered」这个集成缺口无齿覆盖。RM-A8 的存在价值实证。
- **修复候选（一行级）**：RaiseReminderQueue 在 `alert_drawn == 1` 后、EmitRenderReceipt 前补 `kidpal_reminder_mark_rendered(state, shown.id)`（state 取 `kidpal_reminder_default()`，与同函数其它调用一致）。

### F2 · 弹窗被语音轮 speaking 态覆盖，不驻留（根因候选，fixer 轮钉）

- **现象**（human 目击 + 串口）：演示档2 `alert_render` 在 65.9s（设备 uptime），TTS 67.1s 起——弹窗仅可见约 1 秒即从屏上消失（「很快自己就关了」）；但 120.4s KEY 按下时 `dismiss_front_overlay` 才把它从栈里关掉（RM-A6 KEY 语义本身正确执行：KEY 回当前页 HOME）。
- **佐证**：65.9s→120.4s 之间**无任何 page 级渲染日志**（kidpal_rn render/show_chat 零输出），排除显式整页重绘；layer 栈方向正确（`kidpal_render_node_begin_reminder` 推 REMINDER(5) 置前，kidpal_render_node.cc:439-452）。
- **候选根因**：语音轮 speaking 态的 chrome/姿态动画（SetStatus/SetEmotion 或 chrome 定时器）在弹窗之上重绘、或 RLCD 面板刷新链不含弹窗帧；归 fixer 轮以「speaking 期间弹窗驻留、不可被 VOICE 层视觉件覆盖」为靶钉死。
- **违反**：RM-A2（最高层覆盖、不被压）、RM-A6（驻留到按键）。演示档1（17:33 第三推）「完全无弹窗只见语音」与本缺陷同根（弹窗即起即被盖）。

### F0 · speaking 态显示锁被持 60s+，按键无响应假死（独立 finding，先于本轮 demo 出现）

- **现象**：语音轮 LLM 回复 speaking 期间连按 KEY，`key_route_dispatch` 有日志无画面反应；`E Display: Failed to lock display` 两条恰好相隔 30000ms（246.5s/276.6s uptime，display.h:61 超时上限）——锁被持 ≥60s；最终「Abort speaking」才恢复 speaking→listening。human 以重启解围。
- **边界**：该次卡死发生在任何提醒推送之前（网关 17:31 才首推），**与 K4 弹窗路径无涉**；疑似 K3 语音轮显示链既有缺陷。不阻塞 RM-A8 判定主流程，但须独立消解（ARQ 或独立修复批）。

## RM-A8 四项清单覆盖进度

| 项 | 状态 |
|----|------|
| 弹窗演出（骨架/三件套） | 部分——骨架复用形态可见、提示音/TTS 通；驻留不成立（F2） |
| 最高层覆盖 | 不成立（F2）——VOICE speaking 视觉件盖过 REMINDER |
| 录音排队例外 | 未测（F2 修复后重测） |
| 模拟器截图缺口 | 未补 |

**判定**：RM-A8 本轮 **不通过**（F1+F2），缺陷证据如上；修复批消解后重跑确认轮。
