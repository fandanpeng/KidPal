# 烧录与启动观测记录 · 261014-voice-round-entry-exit-fixes

> 执行者 = 主线程（human 于本会话预授权本轮自主烧板）；记录时间 2026-10-14 12:46 +0800。

## 构建

- 命令：`cd firmware && "C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" c:/tmp/idf_run.py build`
- 结果：exit 0；产物 `firmware/build/xiaozhi.bin`（3219856 字节）。
- 注：二进制 mtime = 12:31（Pod 验证期 `VERIFY_FW_BUILD_REQUIRED=1` 构建），本次增量构建无重链接——源面自 12:31 起零改动（`git diff 17e0709 HEAD -- firmware/ server/` 仅簿记文件），镜像与冻结 diff 一致。

## 烧录

- 命令：`"$IDFPY" -m esptool --chip esp32s3 --port COM5 --baud 460800 write_flash 0x20000 firmware/build/xiaozhi.bin`
- 偏移依据：`firmware/partitions/v2/16m.csv`（无 factory 分区，app = ota_0 @ 0x20000）；本包纯代码变更、无 spiffs 资产变更 → 仅烧 app、NVS 保留（flash-asset-partition-rule 口径）。
- 结果：`Wrote 3219856 bytes (1813901 compressed) at 0x00020000 in 18.4 seconds` + `Hash of data verified.` + 硬复位。

## 启动观测（追端口捕获 30s，kidpal-flash-device 判据）

- 命令：`timeout 50 "$IDFPY" c:/tmp/serial_catch_tight.py 30 c:/tmp/boot-check.log`（外层 timeout 兜底——捕获器内层 while 不回检截止、用完必须强杀）。
- 判据核对：**`[[RE-ENUM]]` 计数 = 0** + RX 字节 56794 + uptime 时间戳平滑推进 → **启动成功**（非复位循环）。
- 业务面可见性：`chrome_mid_sync` / `chrome_front layer_top=2` / `show_home: LOCAL_PAGE front` / `schedule_grid_restore corr_id=... courses=39 cached=39` / `rtc_writeback ok rtc=12:46:26 date=10-14`——主循环与显示层正常。
- 收尾：捕获进程已强杀、COM5 已释放（`tasklist python.exe` 空）。

## 未做（按 human 裁定，留待联测轮）

- 真机联测六项（VR-A8：唤醒词入口、连续追问回归、提醒轮、查字轮、驻留面合并回归、两项观察）；engine 侧子项需人工部署 ECS 后闭账——human 回来后开串口联测执行。
