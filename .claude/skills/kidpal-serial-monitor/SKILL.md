---
name: kidpal-serial-monitor
description: Serial capture/monitoring for the KidPal ESP32-S3 device on COM5 — passive watch vs reset-and-capture, which tool for which job, and the one-process-holds-COM5 rule. Use whenever reading device boot logs, watching a smoke round, or debugging on-device behavior.
---

# KidPal Serial Monitor (COM5 @ 115200)

## Tools (already on disk, in c:/tmp)
- **`c:/tmp/serial_passive.py <seconds> <out>`** — opens COM5, **NO reset**, just reads. Use when the device is already running and you want to watch a smoke round / live interaction without disrupting it.
- **`c:/tmp/serial_capture.py <seconds> <out>`** — **resets the board via DTR/RTS** then reads. Use to capture a clean boot sequence (boot → WiFi → activation → idle).
- **`c:/tmp/serial_log.py` / `serial_reset_log.py`** — older variants.

Run ALL of them with the IDF python env (has pyserial):
```
"C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" c:/tmp/serial_passive.py 120 c:/tmp/out.log
```
The system `python` lacks pyserial. Decode with `errors="replace"` — device prints Chinese.

## Rules that cost time when forgotten
- **Only ONE process holds COM5 at a time.** A running capture blocks esptool and vice versa. Check `tasklist | grep -i python` and stop background captures before flashing.
- Passive (no reset) is the default for watching smoke rounds; reset capture only when you specifically need the boot log.
- The device only opens the WS channel (prints `WS: Connecting ... with version: N`) **on wake** (wake word「你好小智」or KEY). A passive boot stays `idle` and will NOT show the version line — to verify protocol version you must wake it during the capture.
- **真机轮的协调姿势不在上述三条里** —— 捕获窗口何时开、串口怎么抓、动作点怎么下，各有硬规矩，且违反的代价当场就付（重捕一轮 / 假重启误判 / human 看不到动作点）。跑真机轮**之前**先读下面「Device round protocol / 真机轮协调协议」一节。

## Reading a smoke round (what "good" looks like)
- `with version: 1` (NOT 2 — see [[kidpal-nvs-ops]])
- `WS: Session ID: <uuid>`
- `Application: >> <your STT text>` (ASR up)
- `Application: << <LLM reply>` (LLM answered; NOT「小智现在有点忙」= LLM fallback)
- clean `State: speaking <-> listening`, **no** `WS v2 frame payload_size invalid`, no crash/reconnect.

## Device round protocol / 真机轮协调协议

真机轮 = 含真实硬件、真实人、**不可重放**的一环：协调失败的代价最高，却最没有说明书。以下三条固化自 4 个包的 retro 实证（key-route-a1 / screen-unification / hardware-validation / render-pages），跑真机轮前逐条过。

- **单一长捕获** —— 一次开窗覆盖**全窗口**，中途**绝不关端口**。
  **DTR/RTS 伪影识别**：`serial_capture.py` 开端口时会拉 DTR/RTS 复位板子；若设备重启时刻与捕获**启停时刻秒级吻合**，那是**端口复位伪影、不是真重启**（screen-unification 合并夜三次重启实测，21:15 / 22:50 两处可考）。把伪影当真重启会误导整个排查方向——**判据是「与捕获启停是否秒级吻合」**。
- **human 就绪信号开窗** —— 捕获窗口**不由 agent 单方面开播**，由 human 的就绪信号开启。
  实证：key-route-a1 第一次 300s 窗口**开早了**（checklist 还在读），human 操作落在窗口外、口述与日志互证不上，按「completion 不凭解释宣布」**重捕了一轮**。开窗前先确认人已就位、checklist 已读完。
- **实时口令钉动作点** —— 动作点**不提前分发时间表**，改**实时口令**。
  实证：screen-unification 提前 15 分钟分发提醒时间表，fire 时刻设备离线、human 看不到 agent 发的表格 → 改实时口令「现在按 BOOT！」**一次成功**。

Related: [[kidpal-idf-env]] [[kidpal-ecs-gateway]] [[kidpal-live-smoke-debug]]
