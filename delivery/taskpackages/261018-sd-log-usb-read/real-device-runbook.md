# 真机持板操作单 · 261018-sd-log-usb-read

> 写给 human（持板人）。Pod 已在真机门前停等（status=`awaiting-human:real-device`）；代码面（host 齿 7/7 + 既有套件零新增红 + 设备编译闸）已全绿，本单执行完把结果回给 planner（Pod 会话 `bgrpw9j1f` resume 或新会话 `/build-feature 261018-sd-log-usb-read` 续跑）即可继续收口。

## 前置状态

- 固件 = persist 包（`261018-sd-log-persist`）+ 本包实现，已在分支 `dev/pod-m2` 编译通过（final r2 firmware-build 真编译绿）。
- 需要一张已格式化、有日志的 TF 卡（先跑过 persist 的真机轮或正常使用产生 `/sdcard/logs/KPLOGnnn.TXT`）。
- COM5 = 原生 USB-Serial/JTAG（`VID:PID=303A:1001`）；**跑脚本前关闭其它串口监控/烧录工具**（单进程独占）。
- 脚本：`tools/diag/kidpal_logs.py`（用 IDF python env 跑，需 pyserial）。

## 第 0 步 · 单命令 smoke（先行，验证 `/dev/secondary` 可读）

```
python tools/diag/kidpal_logs.py --port COM5 --cmd list --out list.txt
```

- **成功**（list.txt 有 `KPLOGnnn.TXT <bytes>` 行 + `KPLOG TOTAL` 行）→ 继续 1–9。
- **失败**（收帧超时 / 无响应）→ 按 plan 降级链：降级 A = HAL 直读 `usb_serial_jtag_ll_read_rxfifo`（把失败输出原样交回 planner，回 loop 补丁）；降级 B = console primary 切 USB-SERIAL-JTAG（须回 `/triage-change` 升档重判）。**不要自行改 sdkconfig 硬试。**

## 真机轮九项（LUR-A7，逐项留痕——截图/命令输出/一句结论都行，逐条判据）

1. **静默无自动输出**：烧录本包固件后插 USB（COM5），除正常启动日志外**零自动输出**；不插线时正常启动。
2. **list**：`--cmd list` → 文件名 + 字节数 + `KPLOG TOTAL <bytes> <n> files`；当前写入文件行尾有 `*`。
3. **tail 50**：`--cmd tail --n 50` → 恰 50 行、含当轮 STT 转写 / LLM 回复原文片段、`=== KPLOG BEGIN tail ===` / `=== KPLOG END ===` 帧完整。
4. **cat 0**：`--cmd cat --index 0 --out cat0.txt` → 全文逐字节正确、中文 UTF-8 不乱码（打开 cat0.txt 核对头尾）。
5. **读不干扰写**：正常对话产生新日志后再跑 list / tail → 字节数增长、tail 含新行。
6. **非法命令**：`--cmd cat --index 99`（脚本侧应拦截）与 `--cmd tail --n abc` → 设备侧错误帧（bad-index/bad-cmd）、设备不崩、无多余输出。
7. **拔卡**：运行中拔卡 → `--cmd list` 回 `KPLOG ERROR no-card` 错误帧、设备不崩、语音主链路不受影响；插回重启后恢复。
8. **烧录/监控零回归**：关脚本后 esptool 正常烧录；串口监控（idf.py monitor / 既有链路）正常。
9. **中文与边界**（并入 3/4 观察即可）：tail/cat 的中文内容不乱码、超长行不产生假输出。

## 回传与后续（planner resume 相位，human 无需操作）

1. 九项结果 + smoke 输出交回 planner → planner 落 pod-events + 更新 review-packet LUR-A7 判定。
2. **OQ-1 合并顺序**：本包结果与 persist 真机四项一起验 → human 批 persist 合 main → 本包 rebase 到新 main 后立合（persist 未验完前两包都不合）。
3. resume 后 planner 相位：closeout index projection（INDEX 行）→ supervisor-verdict-gate 机械闸 → pre-merge-escalation-gate → merge-driver。
