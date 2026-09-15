# red-baseline-freeze · 260915-m1-k1a-key-layer-dispatch

> 前置元数据：changeId: 260915-m1-k1a-key-layer-dispatch · date: 2026-09-15 · author: acceptance-author（intake 义务实跑复核；launch 侧同日已实跑一轮，本文件 = 本 worktree 复跑落账）· status: FROZEN-BASELINE
> 落盘形态注：本文件由 planner 按 acceptance-author 交付全文转写落盘——agent 对 delivery/taskpackages/** 无写权（enforce-scope 拦截，先例 = 260911-render-pages 同款头注）。

## 一、复跑结果（2026-09-15，acceptance-author 于本 worktree 实跑）

复跑命令（git-bash，MSYS_NO_PATHCONV=1 必加防 /mnt/c 路径改写；idf.py 无 -T 选项，host_app 是独立工程直接 build）：

```
MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build
MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app/build/host_infra_test.elf
```

实测 = **129 Tests / 5 Failures / 0 Ignored**（首次构建全量编译 exit 0；ELF 运行 exit 4 = unity 汇总 FAIL，属失败计数常态）。

## 二、与 launch 侧数字比对（129/4 → 129/5，差异 +1 条已定性）

Tests 零差异；Failures +1 = RMD-A5（schedule_reminder_b.acceptance.c:1157），KD-A4 成批修订的设计态义务红，非意外新增。根因：launch 侧实跑在 KD-A4 钉点修订落盘之前（旧口径齿对旧实现天然绿）；修订落盘后三处隔离钉点（rmd-B2 out==PAGE_P1 / rmd-B3 out==PAGE_P4 / rmd-B4 out==PAGE_P3）对当前实现（reminder_page.c kidpal_reminder_key 仍产 key_page_next 翻页/PAGE_P1 硬路由）转设计态红，红基线成立的一部分，builder 随 KD-A1/A3 拆耦合转绿，不计入「新增红」违约。

## 三、失败清单逐条判读（5/5 逐字比对，零意外新增红）

1. SU-E3 screen_unification.acceptance.c:1010，在册（docs/spec.md 归档悬空 canary）；
2. KR-A6 key_route_a1.acceptance.c:636，在册（同根）；
3. RMD-A5 schedule_reminder_b.acceptance.c:1157，**本包义务红（设计态）**，红串与 REVISION LOG 预测逐字吻合；
4. WSA-A6 device_ws_switch.acceptance.c:830，在册（ECS shim 收尾证据缺失，owner=260903-device-ws-switch）；
5. WSA-A4 device_ws_switch.acceptance.c:663，在册（flash.log 缺失，owner 同上）。

在册红出处 = delivery/changes/dev/260911-key-route-a1/existing-red-inventory.md + 260911-render-pages/red-baseline-freeze.md；判读纪律 = 新增红才是本包责任、在册红归各 owner 不代修。

## 四、有效基线口径（KD-A6 终跑判据引用）

有效基线 = 129/5（本复跑版）；终跑判据 = 相对本基线零新增红 + 本包新齿全绿；在册 4 条红非本包责任；RMD-A5 义务红随实现落地转绿（转绿后终跑回落 129/4 形态）。注：key_layer_dispatch.acceptance.c 尚未接线进 HOST_ACCEPTANCE_SRCS（KD-A6 test 5 tooth 1 钉此注册，builder FIRST ACTION），129 不含新齿——接线后总数增长，判读以「新增红为零 + 新齿全绿」为准则，不以总数相等为准。

## 五、C 类冻结验收实编

`gcc -std=c11 -Wall -Wextra -fsyntax-only -I firmware/main/test/acceptance/red-baseline -I firmware/main/test firmware/main/test/key_layer_dispatch.acceptance.c`，WSL gcc 实测 error 为零。解析性红定性（当前树逐字核实）：key_route_layer / kidpal_layer_pop_transients / kidpal_render_node_front_is_overlay / kidpal_render_node_dismiss_front_overlay / GLOBAL_ACT_OVERLAY_CLOSE 实现面全部零命中（仅冻结文件注释提及）；板级 waveshare-s3-rlcd-4.2.cc :290-310 旧直连在世；cycle_next 函数区恰 3 处 kidpal_layer_pop（:480-482）；kidpal_reminder_key 非 test 面恰 2 文件（定义+声明）。

## 六、coverage 闸自证 + mutationEvidence 机检

acceptance-coverage-check.mjs ok:true（KD-A1~A6 全认得、KD-A5 认得为 structural、files=本冻结文件）；perAcceptance 11 条与 acceptance[] 逐字节一致（KD-A1×3/KD-A2×3/KD-A3×2/KD-A4×1/KD-A6×2；KD-A5 structural 不入变异面）。

## 七、cwd 口径（C 侧遗留指针纪律）

结构齿 root 自手持 cwd 定位（仓根 / host_app 两处均可，[C4.4]）；文件内引用 delivery/changes/dev/ 证据路径处已钉明「以仓根为 cwd 的相对路径」前提。

## 附录：清单外阅读申报（brief 第④节）

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |
