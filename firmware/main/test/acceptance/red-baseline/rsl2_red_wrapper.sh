#!/usr/bin/env bash
# RSL2 red-baseline channel (acceptance-author, 2026-10-28) — script file form
# (WSL inline `bash -c` $? unreliable, K6 R3-round pitfall note; rsr/rsl_red_wrapper.sh
# same shape). Standalone harness: unity stub + runner.c, no impl linked —
# structural teeth scan the real worktree from the repo root (KIDPAL_REPO_ROOT /
# walk-up); the RSL2-A4 canary hits the kidpal_story_session_end weak sentinel
# (-99, channel form GAP-4 — story_page.c needs host_app lvgl/esp_log includes
# and is runner-channel only; see record_session_lifecycle.acceptance.c header).
# Usage: wsl -d Ubuntu-24.04 -- bash firmware/main/test/acceptance/red-baseline/rsl2_red_wrapper.sh
set -u
RB=/mnt/c/AI/KidPal-pod-m2/firmware/main/test/acceptance/red-baseline
ROOT=/mnt/c/AI/KidPal-pod-m2
cd "$RB" || exit 99

echo "=== red baseline channel: standalone harness (record_session_lifecycle) ==="
gcc -std=c11 -Wall -Wextra -I. -o /tmp/rsl2_red_runner \
    ../../record_session_lifecycle.acceptance.c runner.c
rc=$?
printf '%s\n' "$rc" > /tmp/rsl2_red_compile_exit.txt
echo "COMPILE_EXIT=$rc"
if [ "$rc" != "0" ]; then exit "$rc"; fi

cd "$ROOT" || exit 99
/tmp/rsl2_red_runner
rc=$?
printf '%s\n' "$rc" > /tmp/rsl2_red_exit_final.txt
echo "RUN_EXIT=$rc"
