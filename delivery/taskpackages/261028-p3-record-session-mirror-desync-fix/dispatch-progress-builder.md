# dispatch-progress · builder

## 已完成段

- FIRST ACTION：`firmware/main/test/host_app/main/CMakeLists.txt` 的 `HOST_ACCEPTANCE_SRCS` 已追加 `../../record_session_lifecycle.acceptance.c`，置于 `host_infra_smoke.acceptance.c` 之前。
- M1（RSL2-A1）：`firmware/main/display/kidpal_render_node.cc` `kidpal_render_node_cache_story()` 的 `v != NULL` 分支内、`memcpy(&s_story_view, ...)` 之后新增 `s_story_session_page = v->page;`。既有 setter 定义 + mcp_server.cc 两处显式投递保留（幂等冗余）。
- M2（RSL2-A2）：`firmware/main/mcp_server.cc` 在 `KidpalStorySessionEndApply` 之后新增 `void KidpalStorySessionEndSync() { kidpal_render_node_story_session_page(g_story_view.page); }`（体内不调 cache_story）；`firmware/main/application.cc` 顶部加 extern 声明，Idle 分支 [R2] 收尾块之后、`kidpal_keybar_scene_reset`/`voice_end` 之前调用 `KidpalStorySessionEndSync();`。
- M3（RSL2-A3）：`firmware/main/application.cc` `HandleStartListeningEvent()` 三处早退分支（WifiConfiguring / !protocol_ / auto_connect_in_flight_）各加 `kidpal_story_session_ctx_set(0);`；Activating 分支不动。未误改 `HandleToggleChatEvent` 的同形分支。

## 已落盘文件（changeManifest 内）

- firmware/main/test/host_app/main/CMakeLists.txt
- firmware/main/display/kidpal_render_node.cc
- firmware/main/mcp_server.cc
- firmware/main/application.cc

## 下一步

- 自验已完成，见下。

## 自验结果

- 通道 B 独立冒烟（rsl2_red_wrapper.sh）：COMPILE_EXIT=0、零 warning、5 Tests 1 Failures；唯一红 = TEST 4 canary 独立通道哨兵（GAP-4 通道形态，story_page.c 不入独立链接，非实现失败）；TEST 1/2/3/5 全 PASS。
- host 全量 runner：242 Tests / 3 Failures / 0 Ignored；本包 5 个 TEST_CASE 全 PASS；3 红 = p1_auxbar AB-A1/A2/A4 先存红（零新增红）。
- 设备编译（VERIFY_FW_BUILD_REQUIRED=1 firmware-build-check.mjs）：build-pass，application.cc / mcp_server.cc / display/kidpal_render_node.cc 三设备 TU 均重编（obj mtime 晚于源）。
- 附带：设备构建自动再生的 firmware/sdkconfig 空 Kconfig 段漂移已还原（非本包改动，零语义）。
