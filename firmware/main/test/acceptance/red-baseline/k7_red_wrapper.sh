#!/usr/bin/env bash
# K7 red baseline channel 2 (acceptance-author, 2026-09-23) — script file form
# (K6 k6_red_wrapper.sh 同款; WSL inline `bash -c` $? unreliable, R3-round pitfall
# note). Standalone harness: unity stub + runner.c, no impl linked — structural
# teeth scan the real worktree; TEST 4 behavioral teeth red at the weak sentinel
# (-2) until builder wires the file into the host runner where schedule_page.c's
# strong definition covers it.
set -u
RB=/mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/acceptance/red-baseline
cd "$RB" || exit 99

echo "=== channel 2: standalone structural harness (schedule_grid_restore) ==="
gcc -std=c11 -Wall -Wextra -I. -o /tmp/k7_red_runner \
    ../../schedule_grid_restore.acceptance.c runner.c
echo "COMPILE_EXIT=$?"
/tmp/k7_red_runner
echo "RUN_EXIT=$?"
