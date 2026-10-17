#!/usr/bin/env bash
# SC red-baseline wrapper (acceptance-author scaffolding, non-frozen; k7/rsr 先例同款)。
#
# 2026-10-17 TaskPackage 261016-p3-story-body-sd-cache 冻结轮：新冻结验收件
# firmware/main/test/story_cache.acceptance.c 尚未接线进 host_app 构建（HOST_ACCEPTANCE_SRCS
# 登记属 builder），故按既有先例走**独立冒烟通道**——unity stub（本目录 unity.h）+ runner.c
# 直编同一份冻结文件并执行；不链接任何实现源（story_cache.c / story_page.c 未在世），
# weak 哨兵恒红 = 设计态红（冻结契约未实现），不是编译/链接错。
#
# 判读：COMPILE_EXIT=0（C 类冻结验收实编：编译通道 error 为零）+ 每 TEST_CASE 首个失败即
# 中断（exit code = 失败用例数）。WSL inline `bash -c` 的 $? 不可靠（历史踩坑），故用
# 脚本文件形态并逐行留痕。
set -u
RB="$(cd "$(dirname "$0")" && pwd)"
cd "$RB" || exit 99

echo "=== SC channel: standalone smoke harness (story_cache.acceptance.c) ==="
echo "cwd=$RB"
gcc -std=c11 -Wall -Wextra -I. -o /tmp/sc_red_runner \
    ../../story_cache.acceptance.c runner.c
echo "COMPILE_EXIT=$?"
/tmp/sc_red_runner
echo "RUN_EXIT=$?"
