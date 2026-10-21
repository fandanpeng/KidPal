# test-scope 清单 — 261020-p3-rec-frame-visual（final 相位影响面圈定）
# 合成来源：detect_changes（project=C-AI-KidPal-pod-m2，base=HEAD 4ca3df2d，工作树 diff）+ taskpackage impactedTestsSeed + safety margin。
# 圈定判据：NRF-A1 suiteGate = host 全量套件单 ELF（floor，全量在册，firmware/main/test/*.test.c ∪ *.acceptance.c）；
#   server 零 diff（crossEnd.exempt），pytest 面为基线安全余量（基线锚 2026-10-20 @ b8aaa82b：server 229P0F）。
# 降级留痕：detect_changes / trace_path 图调用边只报 archive test caller（mcp_server.cc / kidpal_render_node.cc
#   调用边图缺）——实际调用点以 grep 核对为 ground truth（7 调用点 / 5 调用作用域，见 review-packet 白盒复核节）。
# 消费：verify.mjs --phase=final --scope-file=<本文件>（unit / acceptance 闸按仓根相对路径求交）。
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
server/acceptance/test_record_dialogue_guide.acceptance.py
server/acceptance/test_schedule_grid_restore.acceptance.py
server/acceptance/test_story_fetch.py
server/acceptance/test_tech260928_school_start_wiring.acceptance.py
server/tests/test_archive_guard_units.py
server/tests/test_engine_round_close_units.py
server/tests/test_reminder_units.py
server/tests/test_schedule_producer_units.py
server/tests/test_story_units.py
