# 硬件速查 · Waveshare ESP32-S3-RLCD-4.2（KidPal 主控板）

> 来源：微雪官方文档 `https://docs.waveshare.net/ESP32-S3-RLCD-4.2/`（2026-09-10 抓取，
> 英文 wiki 同日为空占位，以中文 docs 站为准）+ 官方参考 Demo 代码交叉核对
> （`C:/AI/KidPal/ESP32-S3-RLCD-4.2-Demo/01_Arduino/examples/07_Audio_Test/src/ExternLib/button/button_bsp.c`
> 与 `02_ESP-IDF/07_Audio_Test/components/port_bsp/button_bsp.c`，两套一致）。
> 本文件是**事实参考**，不是 spec；固件侧实现现状随代码漂移，以仓库为准。

## 一、按键（三个物理键，2026-09-10 盘点）

| 键 | GPIO | 有效电平 | 官方定义 | KidPal 固件现状 |
|---|---|---|---|---|
| BOOT | GPIO 0 | 低 | 按住重新上电可强制进入下载模式 | **唯一在用**：单击=语音会话切换（ToggleChatState，故事页内优先消费）/ 双击=四页翻页 / 长按=故事阅读态翻屏（无故事消费时无动作） |
| PWR | —（电源电路，非软件可编程 GPIO） | — | **长按下电、单击上电**（硬件电源管理） | 不在软件面，无按键对象 |
| KEY | GPIO 18 | 低（`GP18_Active 0`） | **自定义功能按键**（官方 Demo 演示单击/双击/长按三手势） | **未使用**——config.h 无定义、板级未初始化（`#define BOOT_BUTTON_GPIO GPIO_NUM_0` 是唯一按键） |

- 结论：三键中真正留给软件的只有 BOOT + KEY 两个；PWR 是硬件电源键。
- **spec↔实现键位对照（2026-09-10 补记）**：spec §4.0 全局手势表（权威）+ §5.3
  设计为**两键分工**——「KEY 主司对话（短按启动/结束/打断、长按回首页）、BOOT
  主司翻页（短按循环翻页、长按页内特例）」，§5.3 注记明言「小智默认 BOOT=
  ToggleChatState，我们改为 KEY 主司对话」。**实现未按 spec 落地**：包 9b 在
  「仅 BOOT 一键」假设下做了过渡方案——BOOT 单击=对话（小智默认）、BOOT 双击=
  翻页、KEY 完全未接。spec 依附 KEY 的页内特例（P2 问课程 / P3 列表光标+双击进
  阅读 / P3 阅读念给我听 / P4 播笔顺 / KEY 长按回首页 / 提醒弹窗 KEY 关闭）全部
  悬空；目前仅「BOOT 长按 P3 阅读翻屏」与 spec 一致。待决：补接 KEY 落地 spec
  两键分工，或 spec 过渡性修订承认一键方案（走 Track B）。
- KEY 接线证据：官方两套 Demo 的 `button_bsp.c` 均 `#define GP18_KEY_PIN 18`、低有效、
  支持 single/double/long-press 三手势（multi_button 库）。
- 接入方法（若启用）：板级 `Button key_button_(GPIO_NUM_18)` + `InitializeButtons`
  注册手势；现有 `boot_button_` 的 esp_button/iot_button 消抖参数
  （`CONFIG_BUTTON_SHORT_PRESS_TIME_MS=180`）对 KEY 同样适用。
- 悬空风险提示：GPIO 18 未初始化时悬空，若未来把它纳入低功耗唤醒源需先配
  上拉/下拉（真机确认电平）。

## 二、核心规格（官方）

- **SoC**：ESP32-S3-WROOM-1-N16R8——Xtensa LX7 双核 240MHz，**16MB Flash + 8MB PSRAM**
  （叠封），内置 512KB SRAM + 384KB ROM，2.4GHz Wi-Fi + Bluetooth 5 (LE) 板载天线。
- **屏幕**：4.2 英寸全反射屏（RLCD），分辨率 **300 × 400**（竖版；KidPal 固件按
  400×300 横向使用），反射成像、无背光，观感近墨水屏、刷新快于墨水屏。
- **音频**：ES8311 低功耗音频 codec（DAC 出声）+ ES7210 ADC（**双麦克风阵列**，
  硬件回声消除）；MX1.25 2PIN 扬声器接口。
- **传感器/时钟**：SHTC3 温湿度 + PCF85063 RTC（支持时间保持，**RTC 独立电源接口
  仅支持 PH1.0 可充电 RTC 电池**）。
- **电源**：18650 锂电池座 + 充放电管理；CHG 充电指示灯（充满熄灭）、WRN 反接警告灯。
- **存储扩展**：Micro SD 卡槽（FAT32）。
- **扩展接口**：2 × 8PIN 2.54mm 排母（预留 UART/I2C/多路 GPIO）；Type-C（烧录+日志）。
- **SKU**：33298（ESP32-S3-RLCD-4.2）/ 33507（ESP32-S3-RLCD-4.2-EN）。

## 三、与 KidPal 固件的对应关系（已接入外设对照）

| 硬件 | 固件消费点 |
|---|---|
| ES8311 / ES7210 / 双麦 | 音频编解码（I2C 总线 SDA13/SCL14，与 RTC/SHTC3 共享——boot hang 修复史见 PROGRESS-SUMMARY §二.1） |
| PCF85063 | RTC 走时（BCD 解码修复 §二.2）→ 渲染节点 `inject_rtc` |
| SHTC3 | 温湿度直读（命令字 0x7866 修复 §二.3）→ `inject_env` |
| 18650 + 充放电路 | 电池电量（ADC）→ 首页电池图标 |
| 4.2" RLCD 400×300 | LVGL XRGB8888 渲染（`kidpal_render_node`）|
| BOOT 键 | 语音会话/翻页（见上表） |

未接入（官方在板、固件未用）：**KEY 键（GPIO 18）**、Micro SD 卡槽（注意：art 资产
用的是内部 Flash 的 SPIFFS art 分区，非 SD 卡）。

## 四、装配/使用注意（官方原文要点）

- Type-C 与 18650 电池插拔**不要以屏幕作为受力点**；屏幕精密易碎，严禁跌落碰撞，
  因操作不当碎屏不保修。
- RTC 备用电池必须是**可充电**（PH1.0 座），不可装纽扣锂原电池。
