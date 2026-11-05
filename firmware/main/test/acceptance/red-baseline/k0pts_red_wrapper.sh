#!/usr/bin/env bash
# K0PTS red-baseline wrapper (acceptance-author scaffolding, non-frozen;
# rdg/sc/k7/rsr 先例同款)。
#
# 2026-11-04 TaskPackage 261104-k0-points-e2e-spike 冻结轮：新冻结验收件
# firmware/main/test/points_spike.acceptance.c 尚未接线进 host_app 构建
# （HOST_ACCEPTANCE_SRCS 登记属 builder，且按 K0S-E4/LP8 后置到 K0a merged 之后），
# 故按既有先例走独立冒烟通道——unity stub（本目录 unity.h）+ runner.c 直编同一份
# 冻结文件并执行；不链接任何实现源（points_page.c 不在世、view_router.c 需 LVGL 头、
# 不可独立编译），weak 哨兵恒红 = 设计态红（冻结契约未实现），不是编译/链接错。
#
# 通道说明：本机 Git Bash 无 gcc，经 WSL Ubuntu-24.04 系统 gcc 编译（host 单测 C4.4
# 同款 WSL 入口；独立通道不依赖 ESP-IDF，纯 gcc 即可）。编译产物放 WSL 侧 /tmp；
# 命令体在 k0pts_inner.sh（避免 inline 引号/CRLF 踩坑）。
#
# 判读：COMPILE_EXIT=0（C 类冻结验收实编：编译通道 error 为零，DEV-GOVERNANCE §4）
# + 每 TEST_CASE 首个失败即中断（exit code = 失败用例数）。
#
# 注意：**本通道不是 firmware 红基线的权威记录**——K0 的 host/device build 红基线
# 按 K0S-E4/LP8 后置到 K0a(261104-k0a-four-page-skeleton) merged、重取基线后实跑；
# 本通道只承载冻结相位实编 + 设计态红取证（不以冻结相位红冒充 host/device 红基线）。
set -u
RB="$(cd "$(dirname "$0")" && pwd)"
# Git Bash -> WSL 路径换算（/c/... -> /mnt/c/...；wslpath 是 WSL 侧工具、Git Bash 无）。
RB_WSL="$(printf '%s' "$RB" | sed -e 's|^/\([a-z]\)/|/mnt/\1/|')"
echo "=== K0PTS wrapper: launching WSL inner channel ==="
echo "cwd=$RB"
echo "wsl_path=$RB_WSL"
MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash "$RB_WSL/k0pts_inner.sh"
exit $?
