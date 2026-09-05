#!/usr/bin/env bash
# ESP-IDF HOST (linux target) run helper — invoked via WSL on Windows.
#
# WHY WSL: ESP-IDF's `CONFIG_IDF_TARGET_LINUX` builds a native Linux ELF that must
# RUN on a Linux host. The Windows-native env here has no host C compiler and cannot
# run Linux ELFs, so the supported route (used by the INF-A4/INF-E2 run protocol) is
# the installed WSL Ubuntu 24.04. This script forwards every argument to `idf.py`.
#
# Usage (from repo root, Windows shell):
#   wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh <idf.py args...>
# Example:
#   wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh --preview set-target linux
#   wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh -T main build
set -e

# ALWAYS run idf.py from this script's own directory (the host_app project dir),
# regardless of where the caller invokes it from. This keeps the sdkconfig/build
# artifacts inside firmware/main/test/host_app and never pollutes the firmware root
# or the xiaozhi device project.
cd "$(dirname "$0")"

# Path to the ESP-IDF framework as seen from inside WSL (C:\esp\v5.5.1\esp-idf).
export IDF_PATH=/mnt/c/esp/v5.5.1/esp-idf
# WSL python venv created by the builder (contains the IDF python deps, pinned to
# espidf.constraints.v5.5.txt). On WSL the venv provides `python3` (Debian layout).
VENV_PY="$HOME/.espressif/python_env/idf5.5_py3.12_env/bin/python3"

if [ ! -x "$VENV_PY" ]; then
    echo "error: IDF python venv not found at $VENV_PY" >&2
    exit 2
fi
if [ ! -f "$IDF_PATH/tools/idf.py" ]; then
    echo "error: IDF_PATH not found: $IDF_PATH" >&2
    exit 2
fi

# NOTE: we deliberately do NOT source $IDF_PATH/export.sh here. That script also
# validates and activates the device toolchains (riscv32-esp-elf, openocd-esp32,
# esp-rom-elfs, ...) which are only needed for on-device targets. The linux target
# builds with the WSL system gcc/g++, cmake and ninja (apt), so those device tools
# are neither installed nor wanted. idf.py resolves cmake/ninja through idf_tools;
# they are on PATH from apt.

exec "$VENV_PY" "$IDF_PATH/tools/idf.py" "$@"