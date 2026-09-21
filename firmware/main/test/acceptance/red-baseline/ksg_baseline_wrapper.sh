#!/usr/bin/env bash
# Existing-teeth baseline probe (acceptance-author scaffolding, non-frozen).
cd /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app || exit 99
VERIFY_FW_BUILD_REQUIRED=1 ./build/host_infra_test.elf > /mnt/c/tmp/ksg_baseline_out.txt 2>&1
rc=$?
printf '%s\n' "$rc" > /mnt/c/tmp/ksg_baseline_exit.txt
