#!/usr/bin/env bash
# RDG red-baseline wrapper (acceptance-author scaffolding, non-frozen; sc/k7/rsr 先例同款)。
#
# 2026-10-19 TaskPackage 261019-p3-record-dialogue-guide 冻结轮：新冻结验收件
# firmware/main/test/record_dialogue_guide.acceptance.c 尚未接线进 host_app 构建
# （HOST_ACCEPTANCE_SRCS 登记属 builder），故按既有先例走独立冒烟通道——unity
# stub（本目录 unity.h）+ runner.c 直编同一份冻结文件并执行；不链接任何实现源
# （story_page.c / view_router.c 需 LVGL 头、不可独立编译），weak 哨兵恒红 = 设计态红
# （冻结契约未实现），不是编译/链接错。
#
# 通道说明：本机 Git Bash 无 gcc（2026-10-19 实测 command not found），故经
# WSL Ubuntu-24.04 系统 gcc 编译（host 单测 C4.4 同款 WSL 入口；独立通道不依赖
# ESP-IDF，纯 gcc 即可）。编译产物放 WSL 侧 /tmp；命令体在 rdg_inner.sh（避免
# inline 引号/CRLF 踩坑）。
#
# 判读：COMPILE_EXIT=0（C 类冻结验收实编：编译通道 error 为零，DEV-GOVERNANCE §4）
# + 每 TEST_CASE 首个失败即中断（exit code = 失败用例数）。
set -u
RB="$(cd "$(dirname "$0")" && pwd)"
echo "=== RDG wrapper: launching WSL inner channel ==="
echo "cwd=$RB"
MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash "/mnt/c/AI/KidPal-pod-m2/firmware/main/test/acceptance/red-baseline/rdg_inner.sh"
exit $?
