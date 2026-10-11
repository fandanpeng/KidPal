# 本地关 8 调用点归类表（RC-E4 验收前提证据）

> builder 逐点复核语义后归类（2026-10-11），planner 代落。行号 = 本包实现后实况（与 plan 快照行号位移 = 文件上部新增常量 + helper 所致，语义一一对应）。

| # | 行号（实况） | 语义 | 归类 | 依据 |
|---|---|---|---|---|
| 1 | application.cc:429 HandleNetworkDisconnectedEvent | 网络断时关通道 | 零重建 | 网络没了重建必刷日志；恢复走既有 K6 触发点 2（HandleNetworkConnectedEvent Idle 支路 :416），不新增触发 |
| 2 | application.cc:832 Application::CloseAudioChannel | K3 [S5] 录音态丢弃（BOOT 手势）；caller = mcp_server KSPV2_ACT_DISCARD_RECORD（mcp_server.cc:347） | B 重建 | 丢弃后回 Idle → 常驻在线（:835 接 helper） |
| 3 | application.cc:878 HandleToggleChatEvent Listening 分支 | 会话 toggle 收尾 | B 重建 | 会话收尾 → T 秒常驻重建（:880） |
| 4 | application.cc:1112 Reboot | 重启前关通道 | 零重建 | 重启流程接管（protocol_.reset + esp_restart） |
| 5 | application.cc:1131 UpgradeFirmware | OTA 升级前关 | 零重建 | 升级流程接管（成功即 Reboot） |
| 6 | application.cc:1220 WakeWordInvoke Listening 分支 | 已在聆听态再唤醒 → 关通道 | B 重建 | builder 复核：分支只关不重开 = 会话收尾，走 T 秒常驻重建（:1223），与 plan :1189 归类一致 |
| 7 | application.cc:1287 SetAecMode | AEC 模式切换关 | 零重建 | builder 复核：功能性；切换后下个会话重开通道，既有语义零偏移，与 plan :1254 一致 |
| 8 | application.cc:1300 ResetProtocol | protocol 重置关 | 零重建 | 功能性；protocol_.reset() 后由激活路径接管，与 plan :1267 一致 |

plan 归类表 :1189/:1254 语义复核结论：两处归类与 plan 一致（:1189→B 重建、:1254→零重建）。

## 遗留行为分叉（builder HIGH 观察，adjudication in flight）

[S7] 抑制本地关 `on_audio_channel_closed_` 回调后，既有 `SetDeviceState(kDeviceStateIdle)` 收尾（application.cc A 区 lambda 承载）对 B 路径失去触发 → 设备态停留 Listening/Speaking → helper 守卫三条件（state == Idle）不过 → 5s 后 TryStartAutoConnect 直接 return → B 常驻重建不触发（与 story「Idle 态恒持活 WS」/ RC-B4 相抵）。:429 网络断关点存在同族次生面（TCP 未察觉时回调亦被抑制）。fix 待 architect advisory 裁定后由 builder 补钉（host 齿允许面内，TEST 8/11 未禁各区 SetDeviceState(）。
