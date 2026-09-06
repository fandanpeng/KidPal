---
name: kidpal-idf-env
description: ESP-IDF build/flash environment for KidPal firmware — the IDF python env path, the MSYSTEM trap, the idf_run.py wrapper, esptool read/flash, and the WSL CRLF shell-script gotcha. Use whenever building firmware, flashing, running esptool, or running repo shell scripts under WSL on the ESP32-S3 board.
---

# KidPal ESP-IDF Environment & Flashing

## Paths (this machine, verified)
- IDF root: `C:\esp\v5.5.1\esp-idf`
- IDF_TOOLS_PATH: `C:\Users\15802\.espressif`
- **IDF python env (USE THIS for any esptool/serial/partition work):**
  `C:\Users\15802\.espressif\python_env\idf5.5_py3.10_env\Scripts\python.exe`
  — has `esptool 4.12.0`, `pyserial`, `yaml`, `zlib` etc. The system `python` does NOT have these.

## The MSYSTEM trap (cost real time)
Git Bash stamps `MSYSTEM` on every child process; `idf_tools.py` hard-rejects it and `env -u` can't help (MSYS re-stamps each child). Fix: run inside a native python process that pops `MSYSTEM` in-process, applies `idf_tools.py export --format=key-value`, then execs. That wrapper already exists:
- **`c:/tmp/idf_run.py`** — call as: `"<idf python>" c:/tmp/idf_run.py build` from `firmware/`. Do NOT pass `idf.py` as an arg (it already execs idf.py).

## esptool (module form, no separate install)
```
IDFPY="C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe"
"$IDFPY" -m esptool --chip esp32s3 --port COM5 --baud 460800 read_flash  0x9000 0x4000 out.bin
"$IDFPY" -m esptool --chip esp32s3 --port COM5 --baud 460800 write_flash 0x9000 out.bin
```
- Board auto-enters download mode via the USB-Serial/JTAG DTR/RTS circuit (no manual BOOT hold needed). Chip: ESP32-S3 QFN56 rev v0.2, 8MB PSRAM, USB mode USB-Serial/JTAG.
- esptool source (if needed): `C:/esp/v5.5.1/esp-idf/components/esptool_py/esptool/esptool.py`
- **Only ONE process may hold COM5** — kill any serial monitor before esptool, and vice versa.

## Build
```
cd firmware && "C:/Users/15802/.espressif/python_env/idf5.5_py3.10_env/Scripts/python.exe" c:/tmp/idf_run.py build
```
Board target: WAVESHARE_S3_RLCD_4_2 (see `firmware/main/sdkconfig`).

## WSL + CRLF shell-script trap (host acceptance runs under WSL)
Repo shell scripts checked out on Windows may carry **CRLF** line endings; WSL bash then dies with `$'\r': command not found`. Do NOT edit the repo file in place — generate an LF temp copy and run that:
```
sed 's/\r$//' espidf_wsl.sh > /tmp/espidf_wsl.lf.sh && bash /tmp/espidf_wsl.lf.sh <args>
```
Root fix (pin `*.sh` to LF via `.gitattributes`) is tracked as governance debt — see `docs/ARCH-REVIEW-QUEUE.md` ARQ-260906-shell-scripts-lf-gitattributes.

Related: [[kidpal-serial-monitor]] [[kidpal-nvs-ops]] [[kidpal-live-smoke-debug]]
