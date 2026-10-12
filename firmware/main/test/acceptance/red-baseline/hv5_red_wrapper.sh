#!/usr/bin/env bash
# HV5 red-baseline wrapper (acceptance-author scaffolding, non-frozen; pcn/ksg 先例
# 同款——wsl.exe 引号转发使 $? 判读不可靠, 落盘出口码与输出逐字留痕)。
#
# 2026-10-11 TaskPackage 261011-p1-home-v5-rework 冻结件修订轮红基线：
#   三份修订冻结验收件（schedule_grid / p1_home_battery / p1_cjk_name）合一独立
#   stub 编译 + 当场执行。**不链 home_p1.c**——seam 终形 5 参落地前旧 4 参强定义
#   与 5 参调用 ABI 错配（假绿风险），weak 哨兵 -2 = 「frozen seam not implemented」
#   解析性红（pcn_red_wrapper 的 home_p1.c 同链姿势在本包修订后失效, 本 wrapper
#   取代其红基线通道; pcn_red_wrapper 保留不动, 供历史回放）。
#
# Usage: wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/acceptance/red-baseline/hv5_red_wrapper.sh
set -u
ROOT=/mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch
OUT=/mnt/c/tmp/hv5_red_out.txt
RCF=/mnt/c/tmp/hv5_red_rc.txt

cd "$ROOT" || { echo 99 > "$RCF"; exit 99; }

gcc -std=c11 -Wall -Wextra \
    -I firmware/main/test/acceptance/red-baseline \
    -I firmware/main/display \
    -o /tmp/hv5_red_runner \
    firmware/main/test/schedule_grid.acceptance.c \
    firmware/main/test/p1_home_battery.acceptance.c \
    firmware/main/test/p1_cjk_name.acceptance.c \
    firmware/main/test/acceptance/red-baseline/runner.c \
    firmware/main/test/acceptance/red-baseline/pcn_red_seam_stub.c
gcc_rc=$?
if [ $gcc_rc -ne 0 ]; then
    echo "gcc build rc=$gcc_rc" > "$RCF"
    exit $gcc_rc
fi
echo "gcc build rc=0 (compile channel error zero, DEV-GOVERNANCE 验收测试纪律 §4)" > "$RCF"

/tmp/hv5_red_runner > "$OUT" 2>&1
run_rc=$?
echo "run rc=$run_rc" >> "$RCF"
exit $run_rc
