#!/usr/bin/env bash
# RSL red-baseline channel (acceptance-author, 2026-09-30) — script file form
# (WSL inline `bash -c` $? unreliable, K6 R3-round pitfall note; k6_red_wrapper.sh
# same shape). Standalone harness: unity stub + runner.c, no impl linked —
# behavioral teeth hit frozen-seam weak sentinels (design-state red), structural
# teeth scan the real worktree from the repo root.
# Usage: wsl -d Ubuntu-24.04 -- bash firmware/main/test/acceptance/red-baseline/rsl_red_wrapper.sh
set -u
RB=/mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/acceptance/red-baseline
ROOT=/mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch
cd "$RB" || exit 99

echo "=== red baseline channel: standalone harness (record_session_live) ==="
gcc -std=c11 -Wall -I. -o /tmp/rsl_red_runner ../../record_session_live.acceptance.c runner.c
rc=$?
printf '%s\n' "$rc" > /tmp/rsl_red_compile_exit.txt
echo "COMPILE_EXIT=$rc"
if [ "$rc" != "0" ]; then exit "$rc"; fi

cd "$ROOT" || exit 99
/tmp/rsl_red_runner
rc=$?
printf '%s\n' "$rc" > /tmp/rsl_red_exit_final.txt
echo "RUN_EXIT=$rc"
