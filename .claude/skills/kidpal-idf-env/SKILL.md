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

### The `esp_idf_monitor` false-positive trap (hit by 3+ pods, 260829-char-stroke 实证)
Symptom: `python c:/tmp/idf_run.py build` dies with `No module named 'esp_idf_monitor'` +
"venv is corrupted" hint. **Misdiagnosis magnet**: it looks like a broken/incomplete IDF venv
(builders have "fixed" it by reinstalling tools or declaring the env broken — both wrong).
Root cause: `idf_run.py` execs idf.py with **`sys.executable`** — the python you invoke the
wrapper WITH. Plain `python` in Git Bash resolves to the system python (no IDF packages);
the IDF venv itself is fine (`esp_idf_monitor` is present in it).
- **Anti-pattern**: `python c:/tmp/idf_run.py ...` (bare `python`).
- **Fix**: always invoke the wrapper with the venv python (exact command above); wrapper
  inherits whatever python launched it.
- Sanity check if unsure: `"$IDFPY" -c "import esp_idf_monitor"` → should print nothing (ok).
- Two more invocation rules from the same pod: (1) run **from `firmware/`** — idf.py needs
  the project CMakeLists in cwd (bare `c:/tmp/idf_run.py build` from repo root fails with
  "CMakeLists.txt not found in project directory"); (2) this is the **device target** path
  only — host acceptance runs go through WSL `espidf_wsl.sh` ([C4.4] below), never mix.

## WSL + CRLF shell-script trap (host acceptance runs under WSL)
Repo shell scripts checked out on Windows may carry **CRLF** line endings; WSL bash then dies with `$'\r': command not found`. Do NOT edit the repo file in place — generate an LF temp copy and run that:
```
sed 's/\r$//' espidf_wsl.sh > /tmp/espidf_wsl.lf.sh && bash /tmp/espidf_wsl.lf.sh <args>
```
Root fix (pin `*.sh` to LF via `.gitattributes`) is tracked as governance debt — see `docs/ARCH-REVIEW-QUEUE.md` ARQ-260906-shell-scripts-lf-gitattributes.

## Two more WSL gotchas (260915 launch-prep 实测; verbatim evidence = delivery/changes/arch/260915-gov-review-worktree-pool-reuse/evidence-wsl-pitfalls.md)
1. **`espidf_wsl.sh` 头注释的 `-T main` 示例已失效** — IDF v5.5.1 `idf.py` has no `-T` option (`Error: No such option: -T`). Correct usage = `espidf_wsl.sh build`（无 -T）.
2. **git-bash 调 WSL 必须 `MSYS_NO_PATHCONV=1` 前缀** — otherwise MSYS rewrites `/mnt/c/...` into `C:/Program Files/Git/mnt/c/...` (`No such file or directory`). Working form:
```
MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal/firmware/main/test/host_app/espidf_wsl.sh build
MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- /mnt/c/AI/KidPal/firmware/main/test/host_app/build/host_infra_test.elf
```

Related: [[kidpal-serial-monitor]] [[kidpal-nvs-ops]] [[kidpal-live-smoke-debug]]
