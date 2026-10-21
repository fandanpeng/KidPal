# test-scope · 261021-p3-record-cancel-lifecycle（final 相位影响面圈定清单）
# 生成者：integrator（Step 4 范围圈定）· 2026-10-21 · rigor=full
# （final 相位 round=4 与 invocationId 由本包收口 verify.mjs 消费本清单时指定）
#
# §1 圈定方法（正常路径，非降级）
#   detect_changes（Pod 专属图索引 C-AI-KidPal-pod-m2，base=main，merge_base=be93ca5e，
#   inbound depth=2）：changed_files 16（实现面 7 + 冻结件 3 + 簿记 6）/ seed_symbols 217 /
#   impacted_total 22——per-Pod 专属图索引可用（launch 面必办项在位）-> 正常圈定，零降级。
#   改动未 commit（builder 实现在工作树）不影响本仓 detect_changes 覆盖：changed_files 与
#   git status 逐件一致（含 untracked 新件 server/tests/test_story_cancel_lifecycle_units.py），
#   图内新符号 SendKidpalStoryDiscard / _session_legs / _register_session_legs 均可解析。
#   blast radius 映射（inbound）：front_app.py -> _story_timeout_sweep / pump_client_to_upstream /
#   reminder_poll_loop（提醒投递面 = must-stay-green 行为锚）；mcp_server.cc -> McpServer
#   HandleStoryKey / StoryTickTimerCb；protocol.cc -> Application::Initialize（设备编译面，
#   firmware-build 闸承载）；acceptance 面 -> test_m4k1_story_cloud_link（SAO 收尾单源 /
#   connect push）/ test_m4k3_record_session_live（test_a4_discard_after_speech_drops_content
#   ——丢弃语义相邻件）/ test_record_dialogue_guide（RDG-A6 sweep 收尾 = U3 回退路径
#   must-stay-green）/ test_story_fetch（_device_to_engine 共享 seam）。
#   trace_path 抽查（降级预案未启用）：detect_changes 正常返回，未走 git diff --stat 抽查路径。
#
# §2 范围构成（与 TaskPackage impactedTestsSeed 合成；floor = 双方冻结件，本包单包无对端
#    合并冲突消解，floor 即本包冻结三件）
#   ① acceptanceTestsFile（本包冻结验收三件）：
#      server/acceptance/test_record_cancel_lifecycle.acceptance.py（RCL-A1..A4）、
#      firmware/main/test/record_session_live.acceptance.c（RSL-A5 tooth3 换锚 + RSL-A8）、
#      firmware/main/test/record_dialogue_guide.acceptance.c（RDL TEST 8 settle + KEY 幂等）。
#   ② blast radius 映射测试（detect_changes inbound 集）：server/acceptance/
#      test_m4k1_story_cloud_link.acceptance.py（SAO 收尾单源腿集相邻）、
#      test_m4k3_record_session_live.acceptance.py（丢弃语义相邻 + _run_connection 面具）、
#      test_record_dialogue_guide.acceptance.py（RDG-A6 must-stay-green = U3 空表回退锚）、
#      test_story_fetch.py（_device_to_engine 共享 seam）。
#   ③ 共享 seam 相邻件：test_kidpal_speak_sync.acceptance.py（kidpal_speak 注入通道——
#      _speak_to_engine_legs 本包加 legs 形参）、reminder 相关 host 件
#      reminder_overlay_unify.acceptance.c（S-F1.4-AC6 RaiseReminderQueue 链）。
#   ④ server 面向严全量：front_app.py 为网关中枢（F1/F2/F3 全落此件），server/acceptance
#      现役 19 件 + server/tests 6 件全部保留（archive/ 退役面排除、conftest.py 为 harness
#      配置不列正文）——含 builder 配套新件 test_story_cancel_lifecycle_units.py。
#   ⑤ firmware 面向严全量：host_app 单一 ELF 全量注册执行，firmware/main/test/*.c 28 件
#      逐件对账在册（*.test.c 3 件 unit 面、*.acceptance.c 25 件 acceptance 面，两闸按类
#      求交），不缩到只剩本包档。
#   ⑥ 设备编译面：firmware-build / device-round 闸（final 相位恒跑）不消费本清单；本清单
#      只承载 unit/acceptance 求交范围。
#
# §3 未纳入及理由
#   - server/acceptance/archive/ 与 firmware/main/test/acceptance/archive/：已退役面（两闸
#     SKIP_DIRS 排除；detect_changes 因继承历史调用边仍部分判入，按退役口径丢弃）。
#   - 设备运行时链（application.cc Idle 清位链——本包预判零改零 diff、story_page 渲染实机
#     面、discard 帧真机端到端帧序）：host 不可达，由 firmware-build 真编译 + 真机轮
#     （human 持板，status=awaiting-human:real-device）承载，不冒充 host 测试。
#   - server/deploy/engine-patch/connection.py：本包预判零改、diff 零触（注入/抑制语义不动）。
#   - delivery/ 簿记改动（taskpackage / verifier-evidence / verify-runs / pod-events /
#     input-brief / launch-card）：制品面非测试条目。
#
# 以下为清单正文（一行一个仓根相对路径，# 注释）。

server/acceptance/test_261001_openclaw_model_name_overlay_drift.acceptance.py
server/acceptance/test_261010_engine_decision21_overlay_persist.acceptance.py
server/acceptance/test_261013_qq_schedule_write_closure.acceptance.py
server/acceptance/test_261017_openclaw_kidpal_skills.acceptance.py
server/acceptance/test_char_meaning_merge.acceptance.py
server/acceptance/test_engine_stop_truncation_asr_filter.acceptance.py
server/acceptance/test_kidpal_speak_sync.acceptance.py
server/acceptance/test_m3k1_schedule_producer.acceptance.py
server/acceptance/test_m3k3_voice_schedule_entry.acceptance.py
server/acceptance/test_m4k1_story_cloud_link.acceptance.py
server/acceptance/test_m4k3_record_session_live.acceptance.py
server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py
server/acceptance/test_prompt_v7_round_routing.acceptance.py
server/acceptance/test_record_cancel_lifecycle.acceptance.py
server/acceptance/test_record_dialogue_guide.acceptance.py
server/acceptance/test_schedule_grid_restore.acceptance.py
server/acceptance/test_story_fetch.py
server/acceptance/test_tech260928_school_start_wiring.acceptance.py
server/tests/test_archive_guard_units.py
server/tests/test_engine_round_close_units.py
server/tests/test_reminder_units.py
server/tests/test_schedule_producer_units.py
server/tests/test_story_cancel_lifecycle_units.py
server/tests/test_story_units.py
firmware/main/test/boot_auto_connect.acceptance.c
firmware/main/test/char_stroke.acceptance.c
firmware/main/test/char_window.test.c
firmware/main/test/char_window_overlay.acceptance.c
firmware/main/test/host_infra_smoke.acceptance.c
firmware/main/test/key_layer_dispatch.acceptance.c
firmware/main/test/keybar_three_slot.acceptance.c
firmware/main/test/log_cmd.acceptance.c
firmware/main/test/p1_auxbar_style.acceptance.c
firmware/main/test/p1_cjk_name.acceptance.c
firmware/main/test/p1_home_battery.acceptance.c
firmware/main/test/record_dialogue_guide.acceptance.c
firmware/main/test/record_dialogue_v7_fix.test.c
firmware/main/test/record_session_live.acceptance.c
firmware/main/test/record_session_repair.acceptance.c
firmware/main/test/record_session_repair.test.c
firmware/main/test/reminder_overlay_unify.acceptance.c
firmware/main/test/retract_p4_three_page.acceptance.c
firmware/main/test/schedule_grid.acceptance.c
firmware/main/test/schedule_grid_restore.acceptance.c
firmware/main/test/sd_card_mount.acceptance.c
firmware/main/test/sd_log.acceptance.c
firmware/main/test/story_cache.acceptance.c
firmware/main/test/story_page_v2.acceptance.c
firmware/main/test/story_page_v2.test.c
firmware/main/test/time_sync.acceptance.c
firmware/main/test/voice_rightcol_label.acceptance.c
firmware/main/test/voice_round_ui.acceptance.c
