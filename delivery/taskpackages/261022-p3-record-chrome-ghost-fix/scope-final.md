# scope-final — 261022-p3-record-chrome-ghost-fix 影响面圈定终验（scope-file）

> 本文件 = final 相位 verify.mjs `--scope-file` 消费的影响面圈定清单（TaskPackage 目录 sibling，Step 4 范围圈定制品）。**圈定失败须举手，不得默认全量、也不得缩到只剩本包档**——本包圈定走三分法①（per-Pod 专属图索引可用，正常圈定，无降级）。

## 1. 圈定可用性（三分法判定）

- per-Pod 专属图索引 `C-AI-KidPal-pod-m2` **可用**（detect_changes 实测命中 seed_symbols 58 / impacted_total 18）→ **正常圈定**，无需降级 git 抽查路径。
- detect_changes 实测：base = 包首提交父 `e5361caf`，changed_files 17（实现面 9 + taskpackage 投影 5 + debt 追记 1 + verify 证据 2），seed_symbols 58，**impacted_total 18（精确）**，全部落 `firmware/main` 单模块，零 server/ 跨面（帧类型负锚守住，crossEnd 豁免声明与实测一致）。

## 2. 影响面对账（detect_changes → 预期缝）

| blast radius 集群 | 代表符号 | 对应实现缝 | 判定 |
|---|---|---|---|
| lcd_display chrome 族（18 中 10，含 Spi/Rgb/Mipi 三 ctor） | KidPalKeybarSync / KidPalVoiceChromeFront / SetStatus / SetEmotion / SetupUI / kidpal_lcd_recording_force_hide / kidpal_lcd_reminder_show | [CG-C3]① 场景单源改造（apply_look 写沿 + sync 通用分支 + chrome 中部判据改走 keybar get/set）+ [CG-C3]② 过渡窗接线 | 预期内 |
| Application 生命周期 | Application.Initialize / Application.Run | [CG-C3]③ HandleStateChangedEvent Idle 收尾沿复位 + 提醒驻留守卫 | 预期内 |
| 渲染节点 | rn_voice_chrome_front_locked | voice chrome 前台消费 scene 态（单源读侧） | 预期内 |
| mcp_server | McpServer.RaiseReminderQueue | [CG-C3]④ story_list 前推/盖章两闭包层栈收口（同文件族） | 预期内 |
| main.cc | app_main | Application 构造链（hop 2 传导） | 预期内 |
| 测试面 | chrome_ghost.test / chrome_ghost.acceptance / record_dialogue_v7_fix.test | 本包验收件 + 单测 + keybar.c 强符号消费的 v7 键帽集行为齿 | 预期内 |

零超出预期符号集的 impact；无跨层调用、无循环依赖、零新增依赖（keybar 新缝全部 additive 尾部追加，纯 C 无芯片/FreeRTOS/LVGL 新依赖）；`kidpal_voice_states.c` / `kidpal_render_node.cc/.h` 在 changeManifest 种子内但 builder 未触碰（预评裁准了 application.cc 收尾沿落点，voice_states 生命周期零改动、render_node 沿用既有收口通道）——缩面非扩面，如实记入 Review Packet 白盒对账。

## 3. test-scope 清单（final verify 消费）

范围 = 本包 `acceptanceTestsFile` ∪ blast radius 映射测试 ∪ 共享 seam 相关测试 ∪ 安全余量。落定如下（**host 全量 runner 一次覆盖全集**，不做挑选式缩档）：

1. **本包验收件 + builder 单测**：`firmware/main/test/chrome_ghost.acceptance.c`（CG-A1..A4 自动断言；CG-A5 = structural live-manual 真机轮观测项，不计 runner 通过面）+ `firmware/main/test/chrome_ghost.test.c`（场景单源复位落点 / 过渡窗回落 / rec_sub 二值边界）。
2. **冻结回归网（CG-A4 点名三族，floor）**：
   - `firmware/main/test/keybar_three_slot.acceptance.c`（键帽三槽 + `kidpal_keybar_scene` 闭集单源引用链——本包动了 keybar.c scene 生命周期，必须全量在跑）；
   - `firmware/main/test/story_page_v2.acceptance.c`（story 页决策表 + 盖章 seq——CG-A3 层栈收口的承载页族）；
   - `firmware/main/test/key_layer_dispatch.acceptance.c`（按键路由 seam——CG-A3「BOOT 列表页路由成列表动作而非 overlay_close」的裁决面）。
3. **blast radius 映射测试**（detect_changes 集群 → 测试件）：
   - `firmware/main/test/reminder_overlay_unify.acceptance.c`（Idle 分支提醒驻留守卫 + kidpal_lcd_reminder_show 消费面）；
   - `firmware/main/test/char_window_overlay.acceptance.c`（CHAR_WINDOW 合法覆盖层语境——过渡窗不得抢前台键帽的负向守卫面）;
   - `firmware/main/test/record_session_live.acceptance.c` / `firmware/main/test/record_session_repair.acceptance.c`（记录会话跨端链路 + story_list 推送 gating——mcp_server 闭包改动面）；
   - `firmware/main/test/record_dialogue_guide.acceptance.c`（控制消费闭环 / 整理过渡态——收尾沿复位的场景上游）+ `firmware/main/test/record_dialogue_v7_fix.test.c`（keybar.c 强符号真跑的 v7 键帽集行为齿，detect_changes impacted 模块）；
   - `firmware/main/test/voice_round_ui.acceptance.c` / `firmware/main/test/voice_rightcol_label.acceptance.c`（voice chrome 演出与右栏——KidPalVoiceChromeFront 消费面）。
4. **注册序守卫**：`firmware/main/test/host_infra_smoke.acceptance.c`（包 0 boundary 齿——chrome_ghost.acceptance.c 登记行须置于 smoke 之前，本件 TEST 5 校验此序）。
5. **安全余量**：host 全量套件 = 上列并集 + 既有全档；回归判据 = 零新增红 vs 基线 227T3F（先存红 p1_auxbar AB-A1/A2/A4，锚 0df2c0c3），转绿后实态 235 Tests 3 Failures（232 = 冻结件 5 例 + builder 单测 3 例新增计入）。
6. **设备编译闸**（CG-X2，final 相位恒跑）：VERIFY_FW_BUILD_REQUIRED=1 真编译通过。

## 4. final bearing verification 执行位

- 命令形（scope-file 缺失 = 红闸，本文件即清单）：
  `node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --scope-file delivery/taskpackages/261022-p3-record-chrome-ghost-fix/scope-final.md --change-id 261022-p3-record-chrome-ghost-fix --round 1 --invocation-id <uuid>`
- **test-scope 清单落盘路径**（一行，final verifier fact / review-packet results 元数据必含）：`delivery/taskpackages/261022-p3-record-chrome-ghost-fix/scope-final.md`。

## 5. 机读清单（acceptance-run-check / unit-run-check 的 scopeFilter 逐行消费；一行一条 repo-relative 路径，散文节仅供人读）

firmware/main/test/chrome_ghost.acceptance.c
firmware/main/test/chrome_ghost.test.c
firmware/main/test/keybar_three_slot.acceptance.c
firmware/main/test/story_page_v2.acceptance.c
firmware/main/test/key_layer_dispatch.acceptance.c
firmware/main/test/reminder_overlay_unify.acceptance.c
firmware/main/test/char_window_overlay.acceptance.c
firmware/main/test/record_session_live.acceptance.c
firmware/main/test/record_session_repair.acceptance.c
firmware/main/test/record_dialogue_guide.acceptance.c
firmware/main/test/record_dialogue_v7_fix.test.c
firmware/main/test/voice_round_ui.acceptance.c
firmware/main/test/voice_rightcol_label.acceptance.c
firmware/main/test/host_infra_smoke.acceptance.c
