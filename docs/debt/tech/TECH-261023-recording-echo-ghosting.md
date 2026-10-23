---
id: TECH-261023-recording-echo-ghosting
title: P3 录音回显滚动瞬间的面板响应叠印——传输层/LVGL 层已三重排除，定性为反射屏液晶响应窗物理特性，缓解与产品选项待拍板
domain: tech
status: deferred
severity: SHOULD
owner: 主线程（产品选项拍板 = human；面板驱动实验 = 需 hardware validation 面）
trigger: human 对回显显示形态拍板（单行化 / 接受现状 / 驱动实验三选一）；或下个动 P3 录音演出面 / 显示驱动面的包
opened: 2026-10-23
closed: ""
closedBy: ""
paths: ["firmware/main/boards/waveshare-s3-rlcd-4.2/custom_lcd_display.cc", "firmware/main/display/story_page.c", "firmware/main/mcp_server.cc"]
source: 2026-10-22/23 真机轮 human 照片两轮（KPLOG005/006/023a）+ architect 评审（IDF v5.5.1 源码证据链）+ 三轮缓解实验（FULL 模式 / 渲染节流 / 作用域修正）
handoff: 产品选项拍板后：单行化走 /requirement-change 档 1（改 v7 回显滚动设计）；驱动实验走 hardware validation；诊断锚已埋可直取
---

**现象**：P3 录音页右侧回显列滚动瞬间（新句入列、四行上移），回显区域出现新旧两帧文本叠印，约 0.3-0.5s 后自行清晰。human 两轮照片实证（2026-10-22 橡皮泥篇 / 2026-10-23 复测），且明确「重影只在文字向上滚动时出现、滚完消失」。

**已排除面（三轮实验 + architect IDF 源码证据链，勿重走）**：

1. **传输层 DMA 竞态——已证伪**。初判 `esp_lcd_panel_io_tx_color` 异步排队（`spi_device_queue_trans` 即返回）+ DMA 后台读 PSRAM 镜像 `DispBuffer`，下一帧镜像写踩在途读窗 = 撕裂。architect 源码复核推翻：S3 上 `esp_ptr_dma_capable` 区间（soc.h:203-204）**不含 PSRAM**，SPI 驱动 `setup_priv_desc`（spi_master.c:1157-1208）对 PSRAM 发送缓冲做**内部 RAM 弹跳拷贝**，DMA 从不读 `DispBuffer`；且 tx_color 的命令相位无条件先回收全部 inflight 事务（esp_lcd_panel_io_spi.c:213-221），相邻帧传输天然串行。「同步等待 trans_done」缓解方案据此**否决不落**（零效果 + 每帧 10-13ms 阻塞）。
2. **LVGL PARTIAL 分块撕裂——已证伪**。切 `LV_DISPLAY_RENDER_MODE_FULL`（整帧单次 flush）上板验证（2026-10-22 23:47），重影形态不变。
3. **渲染帧率——已证伪（部分缓解保留）**。渲染节流 10fps→3.3fps（秒数/回显行数/闪点变化或每 3 拍才渲染，mcp_server.cc StoryRecTimerCb）上板验证，重影依旧只在滚动瞬间。节流保留（CPU/功耗收益），但已实锤非根因；遗留小缺陷如实记录：`flash`（REC 闪点）每 ~350ms 翻转仍击穿 `content_changed` 条件，实际渲染 ~2.8fps 而非 3.3——修法 = flash 移出 content_changed（blink 变半频），未落。

**定性（当前最优解释）**：反射屏液晶响应窗。回显滚动瞬间 216×88px 区域全体像素同时旧→新翻转，液晶重定向需数百毫秒，窗内新旧叠印，稳态后清晰。与「滚完消失」的 human 观察一致；与「静止内容（气泡/猫咪/REC 数字）不叠印」一致（同值重驱无光学响应）。诊断锚已埋：`rlcd mirror ptr=... dma_capable=...`（custom_lcd_display.cc 构造期，2026-10-23 烧板固件起每 boot 打一条）。

**待拍板选项（human 2026-10-23 指示「先不改了」暂挂）**：
- 回显单行化（只显示最新一句、替换式更新，变化面积缩小 4/5）——显示形态变更，走 /requirement-change 档 1 改 v7 回显滚动设计；
- 接受现状（每句滚动一次的短暂叠印）；
- 面板驱动电压/响应实验（story_page init 序 Gate Voltage 0xC0 / VSHP 0xC1 现值可调）——硬件风险，需 hardware validation 面承接。

**相邻洞（architect 评审发现，单独立项评估，勿并入）**：story_list 帧在 `page == ORGANIZING` 时到达 → view_init 打回 LIST + 清 archive_pend + 前推渲染盖掉整理演出（与 2026-10-23 已修的「自动收尾无徽章」同族——archive_pend 现已在 start_organizing 消费时武装，此洞的实害待重评）。

**验证方式**：真机录一篇 + 滚动瞬间照片/目检；诊断锚日志逐 boot 核对。面板物理定性的对照实验 = 官方 Demo（`ESP32-S3-RLCD-4.2-Demo/09_LVGL_V9_Test`，flush 写法与本板逐字同构）跑动画屏看是否同型残影。
