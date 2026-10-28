---
name: kidpal-device-logs
description: Retrieve and read the KidPal device's on-board diagnostic logs (KPLOG segments on TF card) over USB — the kidpal_logs.py pull commands (list/tail/cat), the log format and five semantic seams (wall-clock prefix / PAGE_SWITCH / state_reject / key semantic names / noise down-throttle), the boot-success signature, and reset-loop vs clean-boot discrimination. Use when analyzing what the device actually did (story sessions, state jumps, crashes, reboots), when pulling logs after a device round, or when the user says 看日志/拉日志/查设备日志.
---

# KidPal Device Logs（TF 卡诊断日志拉取与判读）

设备把诊断日志落到 TF 卡（261018 批落卡管线 + 261022-diag-log-content 内容增强）。**事后取证首选这条通道**——重启不丢、带墙上时间、无串口捕获的窗口盲区。实时交互窗口仍走 [[kidpal-serial-monitor]]。

## 拉取（COM5 USB 串口直读 TF 卡，需 IDF env python）

系统 python 缺 pyserial，**必须用 IDF env 全路径**：

```
"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" tools/diag/kidpal_logs.py --cmd list
"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" tools/diag/kidpal_logs.py --cmd tail --n 400 --out c:/tmp/kplog-tail.log
"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" tools/diag/kidpal_logs.py --cmd cat --index 5 --out c:/tmp/kplog005_full.txt
```

- `list` = 列分段与大小，`*` 标当前段；`tail/cat` 落 `--out` 文件（附行数回执）。
- 拉取窗口期独占 COM5（one-process-holds-COM5 规则同 [[kidpal-serial-monitor]]）——拉完即释放。

## 分段与格式

- **KPLOG000..031 轮转**，每段 512KB 封顶；`list` 里 `*` = 当前段。要全量取证就 `cat --index <N>` 当前段 + 前一段拼接。
- 每行 `[YYYY-MM-DD HH:MM:SS] I (uptime-ms) TAG: message`——**墙上时间前缀自 261022 内容增强起每行自带**（SNTP/RTC 未同步时降级 `BOOT+<ms>ms`）；括号内是开机毫秒。跨段拼接按墙上时间对齐。
- 引擎/front 侧对齐：[[kidpal-ecs-gateway]]（引擎 docker logs / front journalctl）+ [[kidpal-live-smoke-debug]] 三源对齐法。

## 五类语义缝（判读入口，diag-log 包 DLG 齿承载）

| 缝 | 日志形态 | 判读 |
|---|---|---|
| 墙上时间前缀 | `[2026-10-22 21:49:31] I (187025)` | 与 front/engine 日志对时间窗；未同步 = `BOOT+ms` 降级标注 |
| 页面跳转 | `kidpal_rn: PAGE_SWITCH from=X to=Y trigger=key/timer/net` | 页面轨迹主缝；`trigger` = 归因 |
| 状态机 | `StateMachine: State: A -> B` + 被拒转移 `state_reject from→to` | 会话/音频通道生命周期 |
| 按键语义名 | `key_route_dispatch action=4(overlay_close)` / `key_gesture_parser key=1(boot) gesture=3(long_press)` | human 操作还原 |
| 噪声降频 | `SystemInfo` 每 60s 一条 | 不再刷屏，但判读时注意它是节流产物非停摆 |

## 启动成功 / 复位循环判据（烧板验收用，[[kidpal-flash-device]] 首选通道）

- **启动成功** = 尾部出现完整 boot 序列推进：`rtc_writeback ... ok`（t≈2s）→ `StateMachine: State: unknown -> starting → activating → idle`，时间戳单调。
- **复位循环** = 尾部**反复同构 boot 序列**（`unknown -> starting` + t≈2s rtc 行多次、间隔 ~7s）；一次看不清隔几秒再 tail，对比尾部是否推进。

## 记录头垃圾行（历史注记）

KPLOG005（memcpy 修复前固件写的段）里记录头带 22 字节栈垃圾（二进制乱码行）——`44793227` 已修；修复后固件写的段不再有。判读老段时跳过乱码行即可。

## 作为设备轮合并证据

KPLOG 可以独立作为真机目击佐证，不必另写「人工在场目检」。在包内 `device-round-evidence.md` 的 observation 写 `method: "kplog"`，并同时给：① `KPLOG000..031` 段号；② 覆盖观察行为的墙上时间窗（未同步时标 `BOOT+ms` 窗口）；③ 包内 `device-evidence/` 的最小脱敏语义摘录路径；④ 该摘录的 SHA-256。摘录必须能判别所声明的 acceptanceRef（如 PAGE_SWITCH / state_reject / key_route_dispatch / StateMachine 转移），只拉 tail 而不指到具体行为不算证据。

**隐私与保留**：完整 KPLOG 可含儿童语音转写，原文件留仓外；公开仓只提交完成判定所需的最小语义行，不提交整段对话原文。原始件在结论复核期不得随当轮收尾即删。

Related: [[kidpal-flash-device]] [[kidpal-serial-monitor]] [[kidpal-ecs-gateway]] [[kidpal-live-smoke-debug]]
