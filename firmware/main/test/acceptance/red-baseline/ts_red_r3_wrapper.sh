#!/usr/bin/env bash
# R3 red baseline channel 1 (acceptance-author, 2026-09-22) — script file form
# (WSL inline `bash -c` $? unreliable, prior-round pitfall note).
set -u
RB=/mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/acceptance/red-baseline
cd "$RB" || exit 99

echo "=== channel 1a: sentinel form (no impl linked) ==="
gcc -std=c11 -Wall -Wextra -I. -o /tmp/ts_red_r3 ../../time_sync.acceptance.c runner.c
echo "COMPILE_1A_EXIT=$?"
/tmp/ts_red_r3
echo "RUN_1A_EXIT=$?"

echo "=== channel 1b: strong-symbol form (real time_sync.c linked) ==="
gcc -std=c11 -Wall -Wextra -I. -o /tmp/ts_red_r3b ../../time_sync.acceptance.c ../../../time_sync.c runner.c
echo "COMPILE_1B_EXIT=$?"
/tmp/ts_red_r3b
echo "RUN_1B_EXIT=$?"
