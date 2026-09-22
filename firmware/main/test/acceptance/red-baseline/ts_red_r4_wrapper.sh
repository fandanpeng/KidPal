#!/usr/bin/env bash
# R4 red baseline channel 1 (acceptance-author, 2026-09-22) — script file form
# (WSL inline `bash -c` $? unreliable, R3-round pitfall note; same shape as
# ts_red_r3_wrapper.sh).
set -u
RB=/mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/acceptance/red-baseline
cd "$RB" || exit 99

echo "=== channel 1a: sentinel form (no impl linked) ==="
gcc -std=c11 -Wall -Wextra -I. -o /tmp/ts_red_r4 ../../time_sync.acceptance.c runner.c
echo "COMPILE_1A_EXIT=$?"
/tmp/ts_red_r4
echo "RUN_1A_EXIT=$?"

echo "=== channel 1b: strong-symbol form (real time_sync.c linked) ==="
gcc -std=c11 -Wall -Wextra -I. -o /tmp/ts_red_r4b ../../time_sync.acceptance.c ../../../time_sync.c runner.c
echo "COMPILE_1B_EXIT=$?"
/tmp/ts_red_r4b
echo "RUN_1B_EXIT=$?"
