# 真机持板操作单 · 261018-sd-log-persist（human 持板）

> 本包已收口到真机门前：scoped r1 与 final r2（圈定 27 条测试范围）全绿；设备编译闸真编译通过（`xiaozhi.bin` = `0x322180` 字节，最小 app 分区余量 9%）；integrator closeout 完成、commit `0dc0447e`。**本 Pod 未烧录、未连真机**——以下四项由 human 持板执行，完成后 resume 续跑。

## 一、烧录（只烧 app 分区；本包无资产变更）

- 产物：`firmware/build/xiaozhi.bin`
- **app 偏移 = `0x20000`（`ota_0`，不是 0x10000）**；本板分区表无 factory 分区，错偏移会 7 秒复位循环 + 黑屏。
- 二选一：
  - 全量（推荐）：在 `firmware/` 下
    `"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" c:/tmp/idf_run.py flash`
  - 仅 app（保 NVS 配置）：
    `"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" -m esptool --chip esp32s3 --port COM5 --baud 460800 write_flash 0x20000 firmware/build/xiaozhi.bin`
- 烧录前确认 TF 卡在位、FAT32；串口观察 COM5（先关其它串口工具，单进程独占）。

## 二、真机观察项（SLP-A6，逐条记录 PASS/FAIL + 现象）

| # | 观察项 | 判据 | 结果 |
|---|---|---|---|
| 1 | **落卡与内容** | `/sdcard/logs/` 出现 `KPLOG*.TXT`；随机做一轮真实对话（如问「今天有什么课」「今天天气」），日志里能看到 `<<`（STT 转写）与 `>>`（LLM 回复）原文片段。读完卡后请把日志文件片段贴回或指路径。 | 待填 |
| 2 | **串口不中断** | 烧录后 COM5 启动日志持续输出，装钩子前后没有「日志忽然停住/缺一段」；应用运行期串口仍有正常 `ESP_LOGI` 行。 | 待填 |
| 3 | **循环覆盖与目录封顶** | 文件名严格 `KPLOG000.TXT` … `KPLOG031.TXT`；单文件约 512KB；32 个文件写满后最旧被覆盖；`/sdcard/logs/` 目录总量 ≤16MB。若不便长时间跑，可只核「文件按序推进 + 单文件接近 512KB 时切档」并如实标注未跑满轮。 | 待填 |
| 4 | **拔卡/坏卡降级（若可执行）** | 运行中拔卡：设备不崩、不复位循环，语音主链路继续可用；恢复卡后 30s 内重新出现新日志（重试恢复）。**若卡不方便拔**，如实写「本机不可执行」即可，不要伪造观察。 | 待填 |

> 隐私提醒：日志含儿童语音转写与 AI 回复原文（human 已拍板接受）。诊断完成后可直接删除 `/sdcard/logs/` 或整卡处置；本包不加加密/脱敏。

## 三、完成后请回什么

逐条回 **PASS / FAIL + 现象**（含串口日志片段或卡内文件片段路径）。若有 FAIL，给编号与现象即可，planner 会在同 changeId 内派 fixer 续跑，不重开包。

## 四、resume 后的机械路径

1. 你的真机结果落 pod-events `human-decision`（含四条结论与证据路径）。
2. planner 跑机械合并门 `supervisor-verdict-gate.mjs`；**riskTier=high，预期 ESCALATE 到 human**（这是设计行为，不是异常）。
3. ESCALATE 经你批准后：落 `human-decision`（`payload.decision="approve-escalated-merge"`）→ 前置升级闸 `pre-merge-escalation-gate.mjs` → `merge-driver.mjs --pod 261018-sd-log-persist` 本地 `--no-ff` 合并（可回滚）。不 push。
