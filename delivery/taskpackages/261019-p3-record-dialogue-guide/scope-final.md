# test-scope · 261019-p3-record-dialogue-guide（final 相位影响面圈定清单）
# 生成者：integrator（Step 4 补跑）· 2026-10-20 · rigor=full
# （final 相位 round 与 invocationId 由 planner/TaskPackage 消费本清单时指定）
#
# §1 圈定方法（正常路径，非降级）
#   detect_changes（Pod 专属图索引 C-AI-KidPal-pod-m2，base=main，merge_base=e65a8731，
#   inbound depth=2）：changed_files 41（实现面 + delivery 簿记 + pod-retro）/ seed_symbols 204 /
#   impacted_total 129。实现面子集另以 git diff --name-only 400d5a1c..ef3e5066 -- server/
#   firmware/ tools/ 实测 19 件交叉核对（覆盖冻结件 commit cedd4d1f + 实现 commit ef3e5066）。
#   新符号 is_stuck_phrase / build_organizing_input / kidpal_display_control_action /
#   kidpal_story_on_control 均已入图可解析；新冻结件 record_dialogue_guide.acceptance.c
#   亦在 impacted 集内 —— 图索引可用 -> 正常圈定，零降级。
#   （review-packet §Step 9b 原申报的「未 commit 工作树导致 detect_changes 降级为
#   change-manifest 对账」已由 planner 触发本次补跑闭环，原降级申报撤销。）
#   blast radius 映射（inbound）：story.py -> StoryAccumulator/_ensure/mark_pending 与经手件；
#   front_app.py -> _story_feed_from_stt / _story_finish_on_phrase / _story_stop_on_listen_stop /
#   build_display_tool_calls / _push_story_list 及相邻 server 验收面；story_page.c /
#   view_router.c -> story_page_v2 / record_session_live / reminder_overlay_unify 等共享 seam
#   冻结件；application.cc / mcp_server.cc 的运行时链 host 不可达（见 §3）。
#
# §2 范围构成（与 TaskPackage impactedTestsSeed 合成）
#   ① acceptanceTestsFile（本包冻结验收三件）：
#      server/acceptance/test_record_dialogue_guide.acceptance.py（RDG-A1..A7）、
#      server/acceptance/test_m4k1_story_cloud_link.acceptance.py（M4K1-A5 语义换代重钉，
#      本包改动在册）、firmware/main/test/record_dialogue_guide.acceptance.c（RDG-F1..F5；
#      RDG-F6 live-manual structural 不计入 runner 通过面）。
#   ② 相邻 must-stay-green：server/tests 全量 3 件（96 passed 基线；test_story_units.py 即
#      builder 120s 语义退役重写承载件）+ 261014 截断验收
#      server/acceptance/test_engine_stop_truncation_asr_filter.acceptance.py（stop-truncation
#      豁免重绑回归面）+ M3K1/M3K3/M4K1/M4K3 相邻件（图判定同入影响集）。
#   ③ 共享 seam 相邻冻结件：firmware story_page_v2.acceptance.c + story_page_v2.test.c
#      （story_page.c 尾部追加 rec_substate / ORGANIZING 的既有行为宿主）、
#      record_session_live.acceptance.c（记录会话键位与 story_list gating 相邻）、
#      reminder_overlay_unify.acceptance.c（S-F1.4-AC6 RM-A3 设备半回归）；
#      server test_kidpal_speak_sync.acceptance.py（kidpal_speak 注入通道相邻）、
#      test_story_fetch.py（story 文本消费相邻）。
#   ④ server 面向严全量：本包 server/** 大改（story.py / front_app.py / display_schema.py /
#      engine-patch connection.py），server/tests 3 件 + server/acceptance 现役 17 件全部保留；
#      archive/ 退役面排除（与两闸 SKIP_DIRS 同口径）。conftest.py 为 pytest harness 配置、
#      非收集条目，不列正文。
#   ⑤ firmware 面向严全量：host_app 为单一 ELF 全量注册执行——HOST_ACCEPTANCE_SRCS 登记件与
#      firmware/main/test/*.c 逐件对账一致（27 件），本包新登记件与既有全部冻结件在同一条
#      runner 单命令内天然同跑，不缩到只剩本包档。其中 *.test.c 3 件归 unit 面、
#      *.acceptance.c 24 件归 acceptance 面，两闸按类求交。
#   ⑥ 设备编译面：firmware-build / device-round 闸（final 相位恒跑）不消费本清单；
#      本清单只承载 unit/acceptance 求交范围。
#
# §3 未纳入及理由
#   - server/acceptance/archive/ 与 firmware/main/test/acceptance/archive/：已退役面
#     （两闸枚举 SKIP_DIRS 排除；detect_changes 因继承历史调用边仍部分判入，按退役口径丢弃）。
#   - firmware/main/test/acceptance/red-baseline/rdg_inner.sh、rdg_red_wrapper.sh：非冻结冒烟
#     脚手架（无断言位，taskpackage notes ⑪ 已明示不属 acceptanceTestsFile；承载 =
#     redBaselineCommand + 已登记入 runner 的冻结件本身）。
#   - 设备运行时链（application.cc 的 kvr 场景守卫与 stt-final 回调、mcp_server.cc 控制消费、
#     story_page 渲染实机面、引导/整理 LLM 真实回合、S-F1.4-AC6 端到端）：host 不可达，由
#     firmware-build 真编译 + RDG-F6/S5 真机轮（human 持板，status=awaiting-human:real-device）
#     承载，不冒充 host 测试。
#   - server/deploy/engine-patch/connection.py、server/gateway/**、front/prompts/*.md：实现面与
#     制品面，非测试条目；其行为由 RDG-A3/A4 等冻结件断言承载。
#   - server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py 的 live 类：@pytest.mark.live
#     由 conftest 机械 skip；该件 offline 部分照常入范围（本清单按文件圈定，不按 marker 缩范围）。
#   - tools/ 本包零 diff；delivery/ 簿记改动不属测试面。
#
# 以下为清单正文（一行一个仓根相对路径，# 注释）。

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
server/tests/test_reminder_units.py
server/tests/test_schedule_producer_units.py
server/tests/test_story_units.py
