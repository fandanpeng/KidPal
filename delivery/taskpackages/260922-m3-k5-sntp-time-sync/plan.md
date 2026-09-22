# Plan · 260922-m3-k5-sntp-time-sync

```yaml
rigor: lite
riskTier: medium
sourceType: null          # origin.type = human-confirmed-plan（无 sourceType 镜像，缺省）
reviewed-by: architect（triage 四档裁决 2026-09-22，②单包 Lite，human 拍板范围 F1=(a)/F2=随包后锁定）
date: 2026-09-22
```

## 来源与立项

挂账：`delivery/workstreams/m3-schedule/progress.md` K3 设备轮段登记的独立缺口「设备时间错误」（当时记的根因 = 网关未下发 server_time——**主线程 2026-09-22 实测修正**：引擎 OTA 端点确会返回正确的 `server_time {timestamp(UTC ms), timezone_offset:480}`，真因是**设备侧根本不去要**——方向 B 短路激活后 `ActivationTask()`（application.cc:330-348）不再调用 `Ota::CheckVersion()`，而它是全固件唯一解析 server_time 并 `settimeofday` 的地方（ota.cc:186-204），固件无任何 SNTP）。真机实证：串口刷 `System time is not set, tm_year: 70`，显示层守门（lvgl_display.cc:129 `tm_year >= 2025-1900`）拒绝渲染时钟。human 立项 ack = 2026-09-22 本会话（效果要求：「设备显示真实时间；断电重新联网后能同步到正确时间」→ 对应范围裁定 F1=(a)）。

## 目标效果（一句话）

设备全部常显时间面（XiaoZhi chrome 状态栏 HH:MM + P1 辅助条时间/日期）显示真实本地时间：联网后自动 NTP 对时、对好的时间写回 PCF85063 硬件 RTC（断电由纽扣电池续航、重新上电未联网也显示正确时间）、P1 时间消费面周期 re-inject 修掉「boot 快照冻结」既有缺陷（F2）。

## 范围裁定（triage F1，human 拍板 2026-09-22）

- **F1=(a)（选定）**：SNTP 校时 + 写回 PCF85063 + 周期 re-inject——唯一同时满足「常显时间正确 + 断电续航（对齐 S-F1.1 AC4 / NFR-OFF-1）+ 不动渲染消费面」的形状；保持 Lite。
- F1=(b)（P1 消费面切系统 localtime）未选——该变体连带 `p1_home_battery.acceptance.c` 的 `s_rtc_hour`/`inject_rtc` guard 齿、按单向守卫升 Full；F1=(c)（只修状态栏）不满足 human 效果要求。
- **F2=随包一并修**：`s_rtc_hour/minute` 是 boot 快照（waveshare-s3-rlcd-4.2.cc:255 `ReadRtcTime` 仅 `InitializeSensors` 时读一次、全固件无第二次调用）——(a) 的周期 re-inject 顺带收口，最经济。

## 关键接缝

- **SNTP 挂点**：WiFi got IP 后异步启动（`HandleNetworkConnectedEvent` 或 activation done 后），不阻塞主流程。server 列表 = ntp.aliyun.com 主 + ntp1.aliyun.com / pool.ntp.org 备（esp_netif_sntp 多 server）。
- **写回 RTC**：新增 PCF85063 I2C **写**路径（现只有读），UTC+8 转本地 BCD 写 0x04–0x09 寄存器；注意 BCD 编码与 OS/CV 标志位语义（先读 OS 位判晶振停振）。SNTP 成功事件驱动写回。
- **周期 re-inject**：修 F2——P1 消费面从「boot 一次 `ReadRtcTime` → `kidpal_render_node_inject_rtc`」（waveshare-s3-rlcd-4.2.cc:255-280,325-345,598 → kidpal_render_node.cc:88-100,433-439 → home_p1.c:299-343,880-902）改为周期重读重注入；inject 家族保持在世（`p1_home_battery.acceptance.c` 的 `inject_rtc`/`s_rtc_hour` guard 齿零偏移）。
- **TZ**：POSIX TZ 字符串 `CST-8`（UTC+8）；**勿照抄** emote_display.cc:488 的 `setenv("TZ","GMT+0")` 错误先例。走 RTC 本地 BCD 路线时 TZ 与芯片存值解耦。
- **sdkconfig 前提**：核实 `CONFIG_LWIP_SNTP*` 启用（esp_netif_sntp 组件前提），缺则补。

## 验收方向（architect reviewed plan 机判锚）

1. **正锚·时间被设置**：真机串口出现 SNTP 同步成功事件日志；`System time is not set, tm_year: 70` 真机日志**清零**（负锚点）；lvgl 状态栏渲染出正确 HH:MM（守门 `tm_year >= 125` 通过）。
2. **正锚·常显面**：P1 辅助条时间/日期随真实本地时间正确显示；长开机 ≥N 分钟后 P1 时间继续走、非 boot 快照冻结（F2 收口证据）。
3. **正锚·断电续航**：SNTP 写 PCF85063 成功后断电重启 + 保持断网 → RTC 恢复路径显示正确时间（S-F1.1 AC4 / NFR-OFF-1 离线口径）。
4. **负锚·失败降级**：NTP 不可达（域名置错）→ 主流程不卡（激活/对话照常）、无 crash、守门拒绝渲染维持、`UpdateStatusBar` 的 W 日志节流不刷屏（现每 10s 一条，需降噪）。
5. **回归**：既有 acceptance 齿全绿——尤其 `p1_home_battery.acceptance.c` 的 `inject_rtc`/`s_rtc_hour` guard 齿（(a) 路线 inject 家族保持在世）+ `schedule_grid.acceptance.c` KSG-A2 同查；device build 闸（xiaozhi.bin 构建过 + 体积余量记录）。
6. **host runner**：可离线机判的部分（BCD 编解码、SNTP 成功→写回→re-inject 的注入缝逻辑）尽量落 host 齿；SNTP 真联网半挂真机窗口（单真机不撞车纪律）。

## 非目标

不改协议、不改 server/front、不动 `docs/contracts/**`（时间同步不在任何契约内，architect F7 核查确认）；不动 spec 冻结面（spec 不规定校时机制，NFR-OFF-1 被方案 (a) 更好满足）；不动渲染版式/帧缓冲链路（包 260915-m1-n1b-p1-home-battery 的电池链路 owner 边界不触碰）；提醒触发链不改（云端 cron 推送不依赖设备本地时间，architect F6）；OTA 校时通道不复活（ota.cc server_time 路径保持现状死代码）。

## 边角（review 发现，builder 易漏）

- **F2 冻结语义**：P1 时间消费面只认 inject 进 render_node 的快照值——只 settimeofday 不 re-inject 修不了 P1；re-inject 频率与 P1 重绘节拍协调（避免半分钟内跳字/撕裂）。
- **F4 TZ 细节**：POSIX `CST-8` = UTC+8（符号反转易错）；RTC 寄存器存本地时间 BCD，读出后直接喂 inject，不经 TZ。
- **F5 挂点与节流**：SNTP 起在 WiFi got IP 后异步；失败不重试风暴（退避）；`System time is not set` W 日志节流。
- **断电重启离线场景**（F3）：ESP32 系统时间不持久——重启后须从 PCF85063 恢复系统时间（现有 boot 读 RTC 路径已注入 P1，系统时间 settimeofday 恢复与否要补齐），否则断网重启后状态栏时钟仍 1970。
- **OS 位**：PCF85063 OS（oscillator stop）标志置位 = 晶振曾停振（如纽扣电池耗尽）→ 时间不可信，走「不写回、等 SNTP」降级而非把坏时间当权威。

## 调度注记

常驻树 `C:/AI/KidPal-pod-m1-k1a-key-layer-dispatch`、分支 `dev/260915-m1-k1a-key-layer-dispatch`，K4 已合并收尾（7a5d909）、树面 clean——本包可立即排入该树第 9 棒。真机半（锚 1/2/3）挂设备轮窗口，human 在场协调按真机轮协议（单一长捕获 + 就绪信号开窗）。
