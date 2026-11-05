#!/usr/bin/env bash
# K0PTS red-baseline inner channel (acceptance-author scaffolding, non-frozen)。
# 由 k0pts_red_wrapper.sh 在 WSL Ubuntu-24.04 内调用；也可单独执行：
#   wsl -d Ubuntu-24.04 -- bash "$(wslpath <repo>/firmware/main/test/acceptance/red-baseline)/k0pts_inner.sh"
set -u
cd "$(dirname "$0")" || exit 99
echo "=== K0PTS channel: standalone smoke harness (points_spike.acceptance.c) ==="
echo "cwd=$(pwd)"
gcc -std=c11 -Wall -Wextra -I. -o /tmp/k0pts_red_runner \
    ../../points_spike.acceptance.c runner.c
rc=$?
echo "COMPILE_EXIT=$rc"
if [ "$rc" -ne 0 ]; then
  exit 99
fi
/tmp/k0pts_red_runner
runrc=$?
echo "RUN_EXIT=$runrc"
exit "$runrc"
