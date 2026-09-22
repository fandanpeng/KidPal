#!/usr/bin/env bash
# PCN red-baseline wrapper (acceptance-author scaffolding, non-frozen; ksg 先例同款
# —— wsl.exe 引号转发使 $? 判读不可靠, 落盘出口码与输出逐字留痕)。
# Usage: wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/acceptance/red-baseline/pcn_red_wrapper.sh
set -u
ROOT=/mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch
OUT=/mnt/c/tmp/pcn_red_out.txt
RCF=/mnt/c/tmp/pcn_red_rc.txt

cd "$ROOT" || { echo 99 > "$RCF"; exit 99; }

gcc -std=c11 -Wall -Wextra \
    -I firmware/main/test/acceptance/red-baseline \
    -I firmware/main/display \
    -o /tmp/pcn_red_runner \
    firmware/main/test/p1_cjk_name.acceptance.c \
    firmware/main/display/home_p1.c \
    firmware/main/test/acceptance/red-baseline/runner.c \
    firmware/main/test/acceptance/red-baseline/pcn_red_seam_stub.c
gcc_rc=$?
if [ $gcc_rc -ne 0 ]; then
    echo "gcc build rc=$gcc_rc" > "$RCF"
    exit $gcc_rc
fi
echo "gcc build rc=0 (compile channel error zero, DEV-GOVERNANCE 验收测试纪律 §4)" > "$RCF"

/tmp/pcn_red_runner > "$OUT" 2>&1
run_rc=$?
echo "run rc=$run_rc" >> "$RCF"
exit $run_rc
