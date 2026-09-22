#!/usr/bin/env bash
# K6 red baseline channel 2 (acceptance-author, 2026-09-23) — script file form
# (WSL inline `bash -c` $? unreliable, R3-round pitfall note; same shape as
# ts_red_r4_wrapper.sh). Standalone harness: unity stub + runner.c, no impl
# linked — all structural teeth scan the real worktree.
set -u
RB=/mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/acceptance/red-baseline
cd "$RB" || exit 99

echo "=== channel 2: standalone structural harness (boot_auto_connect) ==="
gcc -std=c11 -Wall -Wextra -I. -o /tmp/k6_red_runner ../../boot_auto_connect.acceptance.c runner.c
echo "COMPILE_EXIT=$?"
/tmp/k6_red_runner
echo "RUN_EXIT=$?"
