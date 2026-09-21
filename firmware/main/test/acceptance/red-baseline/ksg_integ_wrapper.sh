#!/usr/bin/env bash
# K2 integrator full host-verification probe (acceptance scaffolding, non-frozen).
# Exit codes captured via script file — never via `wsl.exe bash -c '...; echo $?'`
# (wsl.exe fabricates 0 for inline strings; acceptance-author/builder both bitten).
cd /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch || exit 99
VERIFY_FW_BUILD_REQUIRED=1 bash firmware/main/test/host_app/espidf_wsl.sh build > /mnt/c/tmp/ksg_integ_build.log 2>&1
brc=$?
printf '%s\n' "$brc" > /mnt/c/tmp/ksg_integ_build_exit.txt
if [ "$brc" -ne 0 ]; then
  printf '%s\n' "$brc" > /mnt/c/tmp/ksg_integ_run_exit.txt
  exit "$brc"
fi
cd /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app || exit 99
VERIFY_FW_BUILD_REQUIRED=1 ./build/host_infra_test.elf > /mnt/c/tmp/ksg_integ_run.log 2>&1
rrc=$?
printf '%s\n' "$rrc" > /mnt/c/tmp/ksg_integ_run_exit.txt
