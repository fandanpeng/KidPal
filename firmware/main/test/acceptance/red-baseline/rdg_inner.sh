#!/usr/bin/env bash
# RDG red-baseline inner channel (acceptance-author scaffolding, non-frozen)。
# 由 rdg_red_wrapper.sh 在 WSL Ubuntu-24.04 内调用；也可单独执行：
#   wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m2/firmware/main/test/acceptance/red-baseline/rdg_inner.sh
set -u
cd "$(dirname "$0")" || exit 99
echo "=== RDG channel: standalone smoke harness (record_dialogue_guide.acceptance.c) ==="
echo "cwd=$(pwd)"
gcc -std=c11 -Wall -Wextra -I. -o /tmp/rdg_red_runner \
    ../../record_dialogue_guide.acceptance.c runner.c
rc=$?
echo "COMPILE_EXIT=$rc"
if [ "$rc" -ne 0 ]; then
  exit 99
fi
/tmp/rdg_red_runner
runrc=$?
echo "RUN_EXIT=$runrc"
exit "$runrc"
