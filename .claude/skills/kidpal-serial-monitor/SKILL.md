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

## Reading a smoke round (what "good" looks like)
- `with version: 1` (NOT 2 — see [[kidpal-nvs-ops]])
- `WS: Session ID: <uuid>`
- `Application: >> <your STT text>` (ASR up)
- `Application: << <LLM reply>` (LLM answered; NOT「小智现在有点忙」= LLM fallback)
- clean `State: speaking <-> listening`, **no** `WS v2 frame payload_size invalid`, no crash/reconnect.

Related: [[kidpal-idf-env]] [[kidpal-ecs-gateway]] [[kidpal-live-smoke-debug]]
