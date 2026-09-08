# Host test app `main` component.
# Compiles:
#   - host_app_main.c              -> linux entry (app_main) that auto-runs all tests, exit code = failures.
#   - host_infra_seam.c            -> the LVGL software-render seam host_infra_render_frame / host_infra_read_pixel.
#   - ../../host_infra_smoke.acceptance.c -> the FROZEN acceptance file consumed as host tests.
#
# Convention: later logic packages drop their own `*.acceptance.c` (and any test .c)
# into firmware/main/test/ (or a per-module <subsystem>/test dir); add each such file
# to `HOST_ACCEPTANCE_SRCS` below so it is compiled into this host runner.
#
# The FROZEN acceptance file is consumed DIRECTLY from firmware/main/test/ (one
# level up from this component dir) — single source of truth, never copied. A source
# file outside the component source dir is fine here: this is a test-only project.
set(HOST_ACCEPTANCE_SRCS
    # host_infra_smoke.acceptance.c is listed LAST (registered first at runtime, unity
    # registers TEST_CASE on `main` in reverse link order) so its `boundary` tooth — which
    # asserts the seam exposes the NOT_RENDERED sentinel (-1) "before any frame is flushed"
    # — runs against a FRESH process, before any render-producing test (incl. this package's
    # home_p1 / schedule_p2 render teeth) flushes a frame. Keeping boundary first is the
    # package-0 guard it needs (it cannot pass after any prior render in the process).
    # 包 260829-schedule-p2 (方向 B) 置于列表最前：host runner 实测执行序 = 列表
    # 逆序 (后注册先跑)，schedule 的 LVGL 真帧渲染测试须在 host_infra_smoke 的
    # nothing-rendered 边界齿之后执行，否则缝的 rendered 态会误红该守卫。
    "../../schedule_p2.acceptance.c"
    # 被测实现（5 个 frozen offline contract 函数 + 纯 LVGL 渲染面 -> lvgl include
    # 由本组件 REQUIRES lvgl 提供；P2-A3 render 齿由本接线兑现真跑）。
    # 相对 host_app/main 为 ../../../。
    "../../../display/schedule_page.c"
    # 包 3 (260829-session-logic)：冻结验收 + 被测纯逻辑实现（seam 头在 firmware/main/，
    # 相对 host_app/main 为 ../../../）。
    "../../session_logic.acceptance.c"
    "../../../session_state_machine.cc"
    "../../../key_gesture_parser.cc"
    "../../device_ws_switch.acceptance.c"
    "seam_bounds.test.c"
    "../../ws_provision.test.cpp"
    # 包 260829-voice-roundtrip (方向 B)：冻结验收 + 被测纯逻辑实现（view_router 强符号
    # 覆盖弱哨兵的关键——不编入则冻结离线契约仍红; 相对 host_app/main 为 ../../../）。
    "../../voice_roundtrip_b.acceptance.c"
    "../../../display/view_router.c"
    # 包 260829-home-p1 (首页常显信息牌)：冻结验收 + 被测纯逻辑实现（home_p1 强符号覆盖
    # weak 哨兵; 渲染缝 additive 在 host_infra_seam.c）。
    "../../home_p1.acceptance.c"
    "../../../display/home_p1.c"
    # 包 260829-char-stroke (查生字 P4)：冻结验收 + 被测纯逻辑实现 (char_stroke 强符号
    # 覆盖弱哨兵的关键——不编入则冻结离线契约仍红; 相对 host_app/main 为 ../../../)。
    "../../char_stroke.acceptance.c"
    "../../../display/char_stroke.c"
    # 包 260829-story-p3 (P3 故事页)：冻结验收 + 被测纯逻辑/纯 LVGL 实现 (story_page 强
    # 符号覆盖弱哨兵的关键——不编入则冻结离线契约仍红; 相对 host_app/main 为 ../../../)。
    "../../story_p3.acceptance.c"
    "../../../display/story_page.c"
    # 包 260829-schedule-reminder (提醒弹窗消费面)：冻结验收 + 被测纯 C 实现
    # (reminder_page 强符号覆盖 weak 哨兵的关键——不编入则冻结离线契约仍红;
    #  相对 host_app/main 为 ../../../; 提示音缝在本模块内是弱默认, 设备侧强定义在
    #  mcp_server.cc, host 不参与)。
    "../../schedule_reminder_b.acceptance.c"
    "../../../display/reminder_page.c"
    # 包 260829-schedule-reminder 自带单测（非冻结件）：fire_at 日历/偏移边界、UTF-8 码点
    # 安全截断、known-id 集满淘汰保护驻留/排队项、满队列 FIFO 与 V18 顶替顺序。
    "../../reminder_page.test.c"
    # host_infra_smoke (包 0 守卫) LAST — 见本列表头注释。
    "../../host_infra_smoke.acceptance.c"
)

idf_component_register(
    SRCS
        "host_app_main.c"
        "host_infra_seam.c"
        ${HOST_ACCEPTANCE_SRCS}
    INCLUDE_DIRS "." "../../../display"  # ../../../display: __has_include("char_stroke.h") 契约头探针
    REQUIRES unity lvgl
    # Force every TEST_CASE object into the final link. A unity TEST_CASE registers
    # itself via a constructor; if the owning .o is an unreferenced archive member the
    # linker drops it and unity_run_all_tests() sees "0 Tests". WHOLE_ARCHIVE keeps the
    # acceptance .o (and any later package's *.acceptance.c.o) in the elf so the runner
    # actually registers and runs every frozen TEST_CASE (non-vacuous host runner).
    WHOLE_ARCHIVE
)