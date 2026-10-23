---
name: kidpal-flash-device
description: Flash the KidPal ESP32-S3 app firmware over COM5 — the app offset comes from the partition table (ota_0 @ 0x20000, NOT 0x10000), the wrong-offset boot-loop symptom table (7s reset loop, black screen, USB chime), NVS-preserving flash, and boot-success verification via the device SD log (kidpal_logs.py tail) with serial capture as fallback. Use whenever flashing xiaozhi.bin, a device boot-loops after flash, or you're about to guess a flash offset.
---

# KidPal Device Flash (COM5)

## The offset rule (cost a whole device round, 260915-m1-k2 实证)

**App offset = read the partition table, never guess.** 事实源 = `firmware/partitions/v2/16m.csv`（sdkconfig `CONFIG_PARTITION_TABLE_CUSTOM_FILENAME` 实指）：

- 该表 **无 factory 分区**，app 在 **ota_0 @ 0x20000**。
- ✅ 正确烧录（二选一）：
  - 全量（推荐，bootloader/partition/ota_data/art/assets 一并按表落位）：在 `firmware/` 下 `"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" c:/tmp/idf_run.py flash`
  - 仅 app（保 NVS 配置）：`"$IDFPY" -m esptool --chip esp32s3 --port COM5 --baud 460800 write_flash 0x20000 firmware/build/xiaozhi.bin`
- ❌ 惯例猜测 `0x10000`（ESP32 常见 factory 偏移）在本板 = 镜像整体右移、头部缺失 → bootloader 校验失败。

## Symptom table: wrong-offset boot loop

烧录后出现以下组合 = 先怀疑偏移，**不要**先怀疑代码：

- 设备 ~7 秒周期复位循环；屏幕全黑；Windows 持续播 USB 接入音（USB-Serial/JTAG 每次复位重枚举）。
- 串口打开即 `ClearCommError failed (PermissionError(13, WinError 22))`，任何姿势都抓不到字节。
- 幸运抓到时：只有 ROM bootloader 行 + `RTC_SW_SYS_RST` + `Saved PC 0x403ccced`，零 app 日志（图像校验 loop 伪影）。

**A/B 判定法**（隔开代码与偏移两个变量）：同一段代码分别用错误/正确偏移各烧一次，错误偏移必循环、正确偏移启动稳定 → 偏移伪影实锤，代码无罪。

## Boot-success observation（串口不配合时）

设备重启循环中普通被动捕获抓不到输出。用追端口捕获器（毫秒级直扑 COM5）：

```
"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" c:/tmp/serial_catch_tight.py 30 c:/tmp/boot-check.log
```

判据：**0 个 `[[RE-ENUM]]` + 有 RX 字节 + uptime 时间戳平滑推进 = 启动成功**；有 RE-ENUM = 仍在复位循环。

⚠️ 该脚本内层 `while True` 不回检截止时间——端口稳定后**永不自行退出且锁死 COM5**，用完必须 kill（`tasklist //FI "IMAGENAME eq python.exe"` 核对）。

## Boot-success via SD log（首选；261022 human 指示更新——diag-log 包后每行日志自带墙上时间）

**烧完板子不用拉串口，看设备 SD 卡日志即可**（诊断日志落 TF 卡，261018 批管线 + 261022 内容增强）：

```
"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" tools/diag/kidpal_logs.py --cmd tail --n 80 --out c:/tmp/boot-check.log
```

判据（读尾部）：**出现本轮新 boot 序列**（`rtc_writeback ... ok` 在 t≈2s 处 + `StateMachine: State: unknown -> starting → activating → idle` 推进到 idle，且时间戳单调推进）= 启动成功；**复位循环 = 尾部反复出现同构 boot 序列**（`unknown -> starting` + t≈2s 的 rtc 行多次、间隔 ~7s）——一次看不清就隔几秒再 tail 一次对比尾部 mtime 与序列是否推进。详见 [[kidpal-device-logs]]。

串口捕获降级为 fallback：SD 卡读不出来（挂载失败本身）或需要抓复位循环的第一现场时才用上面 serial_catch_tight（锁端口的坑照旧）。

## Procedure (put together)

1. 构建：`cd firmware && "$IDFPY" c:/tmp/idf_run.py build`（venv python 全路径，[[kidpal-idf-env]] 的 MSYSTEM 陷阱）。
2. 查端口独占：`tasklist //FI "IMAGENAME eq python.exe"`——一个进程持 COM5（kill 残留捕获进程）。
3. 烧录：按上面的偏移规则（默认走 `idf_run.py flash` 全量；仅 app 更新走 `write_flash 0x20000` 保 NVS）。
4. 观测：**首选 SD 卡日志 tail 确认启动**（上文判据）；串口捕获是 fallback。之后再开被动捕获（`c:/tmp/serial_passive.py`）看业务轮。
5. USB-Serial/JTAG 硬复位后会重枚举——COM5 消失再出现是常态，重开句柄即可（`serial.tools.list_ports` 核对还在）。

Related: [[kidpal-device-logs]] [[kidpal-idf-env]] [[kidpal-serial-monitor]] [[kidpal-nvs-ops]]
