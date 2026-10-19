# test-scope · 261018-sd-log-usb-read（final 相位影响面圈定清单）
# 生成者：integrator（Step 4）· 2026-10-19 · round 1（scoped r1 全绿：invocationId
# 56a4e75e-79c4-4c95-ab75-1282fe90835d，final 相位 round/invocationId 由 planner/verifier
# 消费本清单时指定）· rigor=lite
#
# §1 圈定方法（正常路径，非降级）
#   detect_changes（Pod 专属图索引 C-AI-KidPal-pod-m2，base=main，merge_base=21360279，
#   inbound depth=2）：changed_files 52（含 persist 包与 delivery 簿记；本包实现面 =
#   firmware/main/diagnostics/{log_cmd.h,log_cmd.c,log_cmd_port.cc}（新）、
#   firmware/main/diagnostics/{sd_log.h,sd_log_fs.cc}（只读访问器 KidpalSdLogCurrentIndex
#   只增不改）、firmware/main/main.cc（KidpalLogCmdStart 钩子后加位）、两处 CMakeLists
#   显式登记、firmware/main/test/log_cmd.acceptance.c（新冻结验收）、tools/diag/）/
#   seed_symbols 264 / impacted_total 19。图索引可用 -> 正常圈定，无降级。
#   blast radius 映射（inbound）：main.cc -> application/audio/protocols 运行时调用链
#   （设备运行时面，host 不可达，由 firmware-build 真编译 + LUR-A7 真机轮承载）；
#   sd_log.h/sd_log_fs.cc -> sd_log.acceptance.c 相邻冻结齿（must-stay-green）；
#   server 面 2 件非 archive 测试被图判入影响集（继承 persist 批同款保守边）。
#
# §2 范围构成（与 TaskPackage impactedTestsSeed 合成）
#   ① acceptanceTestsFile（本包冻结验收）：firmware/main/test/log_cmd.acceptance.c
#      （7 TEST：LUR-A1..A5 纯逻辑齿 + LUR-A6 源锚结构齿 ×2）
#   ② 相邻冻结 must-stay-green：firmware/main/test/sd_log.acceptance.c（8 TEST：
#      persist 包全齿 keep-green；TEST 6 main.cc 结构锚在 KidpalLogCmdStart 加位后保持绿）
#   ③ 共享 seam / 安全余量：host_app 既有全量套件 = 单一 ELF 全量注册执行，本包新登记
#      （HOST_ACCEPTANCE_SRCS 追加）影响链接/注册序面 -> 全量保留，不缩到只剩本包档。
#      即：下述 firmware 测试清单与本包两件在**同一条 runner 单命令**内天然一起执行，
#      无需另跑独立回归。
#   ④ detect_changes 影响集中的 server 面测试（非 archive，按向严纳入真跑）2 件；
#      archive/ 下 3 件 = 已退役面（闸枚举 SKIP_DIRS 排除），不纳入。
#   ⑤ 设备编译面：firmware-build 闸（final 相位恒跑，rigor=lite scoped 相位后置的
#      设计态在 final 补跑），不消费本清单——本清单只承载 unit/acceptance 求交范围。
#
# §3 未纳入及理由
#   - server/acceptance/archive/ 三件：已退役面，非现役测试面。
#   - 其余 server/tests/*.py 与 server/acceptance/*.py：本包 server/** 零 diff、
#     零端云 wire（crossEnd=exempt），不读本包改动源。
#   - 设备运行时调用链（application/audio/mcp_server/protocols）：host 不可达，由
#     firmware-build 真编译 + LUR-A7 真机轮（human 持板，awaiting-human:real-device）
#     承载，不冒充 host 测试。
#   - tools/diag/kidpal_logs.py：host 取回脚本，无自动齿；首验由真机轮第 1 步单命令
#     smoke（LUR-A7①）承载，非本清单测试条目。
#
# 实测锚（2026-10-19 本 worktree，integrator 独立全量交叉核对）：WSL host_app 全量构建
# + host_infra_test.elf 实跑 = 210 Tests 3 Failures exit 3；3 条全为 p1_auxbar AB-A1/
# A2/A4 先存红；log_cmd 7/7 PASS、sd_log 8/8 PASS —— 零新增红，LUR-X1 host 半成立。
#
# 以下为清单正文（一行一个仓根相对路径，# 注释）。

firmware/main/test/log_cmd.acceptance.c
firmware/main/test/sd_log.acceptance.c
firmware/main/test/sd_card_mount.acceptance.c
firmware/main/test/boot_auto_connect.acceptance.c
firmware/main/test/char_stroke.acceptance.c
firmware/main/test/char_window.test.c
firmware/main/test/char_window_overlay.acceptance.c
firmware/main/test/host_infra_smoke.acceptance.c
firmware/main/test/key_layer_dispatch.acceptance.c
firmware/main/test/keybar_three_slot.acceptance.c
firmware/main/test/p1_auxbar_style.acceptance.c
firmware/main/test/p1_cjk_name.acceptance.c
firmware/main/test/p1_home_battery.acceptance.c
firmware/main/test/record_session_live.acceptance.c
firmware/main/test/record_session_repair.acceptance.c
firmware/main/test/record_session_repair.test.c
firmware/main/test/reminder_overlay_unify.acceptance.c
firmware/main/test/retract_p4_three_page.acceptance.c
firmware/main/test/schedule_grid.acceptance.c
firmware/main/test/schedule_grid_restore.acceptance.c
firmware/main/test/story_cache.acceptance.c
firmware/main/test/story_page_v2.acceptance.c
firmware/main/test/story_page_v2.test.c
firmware/main/test/time_sync.acceptance.c
firmware/main/test/voice_rightcol_label.acceptance.c
firmware/main/test/voice_round_ui.acceptance.c
server/acceptance/test_m4k3_record_session_live.acceptance.py
server/acceptance/test_schedule_grid_restore.acceptance.py
