# red-baseline-freeze · 260915-m1-k2-keybar-three-slot（acceptance-author 实跑冻结记录，planner 转写）

> scope hook（enforce-scope.mjs：acceptance-author 写面不含 delivery/changes/dev/）拦截，按纪律由 planner 逐字转写，非绕行。转写日期 2026-09-16。

五要素：计数 = `13 Tests 0 Failures 0 Ignored`；命令 = `MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build` + `MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app/build/host_infra_test.elf`；日期 = 2026-09-16；HEAD SHA = `a39826461bdc7ce0adc5bd1e22f3916393bc9164`；面域 = firmware host。

锚对账：在册锚 13/0（n1b r12，量测锚 e990eb2）；`git diff --quiet e990eb2 HEAD -- firmware/` 零差异 → 本 HEAD 有效引用；实跑逐条点名 = K1a key_layer_dispatch 5 + N1b p1_home_battery 5 + 包 0 host_infra_smoke 3，全 PASS；在册红 = 零，无逐条差异。

新齿红基线（通道②，DEV-GOVERNANCE 验收测试纪律 §4）：实编命令 = `gcc -std=c11 -Wall -Wextra -fsyntax-only -I firmware/main/test/acceptance/red-baseline -I firmware/main/test firmware/main/test/keybar_three_slot.acceptance.c`，编译通道 error = 0、warning = 0；解析性红定性（2026-09-16 grep 实测）：keybar.{h,c} 不存在、kidpal_keybar_dots/kidpal_keybar_slots 全仓零命中（test 1/2 + test 4 tooth 1/5 红）；lcd_display.cc 三槽符号零命中、g_kidpal_keycap_label 10 处在世、lv_obj_create(bottom_bar_) 0 处、lv_label_create(bottom_bar_) 恰 2（<3）、kidpal_render_node_front_is_overlay 0 处（test 4 tooth 2-4/6 红，tooth 7 区锚在世、区内无左槽符号 = 红）；kidpal_voice_states.c 未接进 runner（test 3 行为半红；结构半 kCap 恰 5 + 零 结束/关闭 字面现状绿）；host_app/main/CMakeLists.txt 无三件接线行（test 5 tooth 1 红）。MUST-STAY-GREEN 现状绿：test 1 tooth 1（PAGE_COUNT=4 deviation 对账锚）+ test 5 tooth 2/3（canary + 扫描器自检）。

判读纪律：builder 落地后终跑判据 = 相对本次基线（13/0）零新增红 + 本包新齿全绿；义务红清单见验收文件尾「KB-A6 终跑判读」段。KB-A5 = structural，human 持板，证据落本目录。

## acceptance-author 交付附注（转写）

- ADV-1（P4 页槽值 spec 未定）：v1.36 撤 P4 后页循环仍 4（KB-E1 deviation），P4 页槽左右文案 spec 无权威行。齿只钉 rc=0 + 左右非 NULL（存在性），不钉值——值钉定须 human 裁定后按 tighten-only 修订验收。已在 test 2 注释与 seam [1] 显式登记。
- ADV-2（冻结即冒烟合规声明）：新齿未接线（builder duty seam [5]），通道① runner 不受本文件影响（13/0 实跑即冻结相位线束冒烟，error = 0）；通道②实编 = C 类冻结验收实编合规。
- builder duty：接线三件进 HOST_ACCEPTANCE_SRCS（`../../keybar_three_slot.acceptance.c`、`../../../display/keybar.c`、`../../../display/kidpal_voice_states.c`，置于 smoke 之前）；mutationRequired=true——Finalize 时 mutationEvidence.perAcceptance 的 acceptanceId 须与 taskpackage acceptance[] 正文逐字节一致，变异命名不得保留被测符号原字面为子串（KR-A3 教训，seam [6] 已注）。
- blocking：无。
