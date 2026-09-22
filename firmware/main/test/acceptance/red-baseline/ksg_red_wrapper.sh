#!/usr/bin/env bash
# KSG red-baseline exit-code probe (acceptance-author scaffolding, non-frozen,
# lives beside the unity stub per the schedule-p2 precedent).
# Usage: wsl -d Ubuntu-24.04 -- bash firmware/main/test/acceptance/red-baseline/ksg_red_wrapper.sh
cd /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch || exit 99
VERIFY_FW_BUILD_REQUIRED=1 /mnt/c/tmp/ksg_red_runner > /mnt/c/tmp/ksg_red_out3.txt 2>&1
rc=$?
printf '%s\n' "$rc" > /mnt/c/tmp/ksg_red_exit_final.txt
