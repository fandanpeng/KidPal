# Host test app `main` component.
# Compiles:
#   - host_app_main.c              -> linux entry (app_main) that auto-runs all tests, exit code = failures.
#   - host_infra_seam.c            -> the LVGL software-render seam host_infra_render_frame / host_infra_read_pixel.
#   - the host smoke acceptance file (host_infra_smoke) -> the FROZEN acceptance file consumed as host tests.
#
# Convention: later logic packages drop their own `*.acceptance.c` (and any test .c)
# into firmware/main/test/ (or a per-module <subsystem>/test dir); add each such file
# to `HOST_ACCEPTANCE_SRCS` below so it is compiled into this host runner.
#
# The FROZEN acceptance file is consumed DIRECTLY from firmware/main/test/ (one
# level up from this component dir) — single source of truth, never copied. A source
# file outside the component source dir is fine here: this is a test-only project.
set(HOST_ACCEPTANCE_SRCS
    # The host smoke acceptance is listed LAST (registered first at runtime, unity
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
    # 包 260829-schedule-reminder (提醒弹窗消费面)：冻结验收 + 被测纯 C 实现
    # (reminder_page 强符号覆盖 weak 哨兵的关键——不编入则冻结离线契约仍红;
    #  相对 host_app/main 为 ../../../; 提示音缝在本模块内是弱默认, 设备侧强定义在
    #  mcp_server.cc, host 不参与)。
    "../../schedule_reminder_b.acceptance.c"
    "../../../display/reminder_page.c"
    # 包 260829-schedule-reminder 自带单测（非冻结件）：fire_at 日历/偏移边界、UTF-8 码点
    # 安全截断、known-id 集满淘汰保护驻留/排队项、满队列 FIFO 与 V18 顶替顺序。
    "../../reminder_page.test.c"
    # 包 260829-story-p3 (P3 故事页)：冻结验收 + 被测纯逻辑/纯 LVGL 实现 (story_page 强
    # 符号覆盖弱哨兵的关键——不编入则冻结离线契约仍红; 相对 host_app/main 为 ../../../)。
    "../../story_p3.acceptance.c"
    "../../../display/story_page.c"
    # 包 260907-hardware-validation (包 9b) 真机语音轮 2 修复自带守卫（非冻结件, [C4.3]
    # 普通 .test.c）：钉 ssl_receive 4096B 栈预算的结构尺寸 + 树级扫描「大栈缓冲不得回潮」
    # + 就地解析语义。相对 host_app/main 为 ../../。
    "../../story_page_stack.test.c"
    # 包 260901-ui-art-integration (包子 U)：冻结验收 + 被测纯逻辑资产层 (art_assets 强
    # 符号覆盖 weak 哨兵——不编入则冻结离线契约仍红; 相对 host_app/main 为 ../../../)。
    # **必须置于 host smoke acceptance 之前**（smoke 保持列表末位 = 最先执行，
    # 包 0 boundary 齿注册序守卫; 验收 test 1 结构齿 6 校验此序）。
    "../../ui_art_integration.acceptance.c"
    "../../../display/art_assets.c"

    # 包 260907-hardware-validation (包 9b)：冻结验收（纯树级结构齿, 自身只依赖
    # unity + 标准 C, 无新增被测符号链接——运行时以文本读工作树断言 F2 接线 /
    # RTC·SHTC3 driver 绑定 / 守卫, 见文件头 Run protocol）。**必须置于 host smoke
    # acceptance 之前**（smoke 保持末位 = 最先执行, 包 0 boundary 齿注册序守卫; 本包
    # 验收 test 4 tooth 1 校验此序）。相对 host_app/main 为 ../../。
    "../../hardware_validation.acceptance.c"

    # 包 260911-key-route-a1 (键位路线落地 PKG-1)：冻结验收（纯树级结构齿, 自身只依赖
    # unity + 标准 C, 无新增被测符号链接——运行时以文本读工作树断言板级 KEY GPIO 注册 /
    # parser 喂入 / 路由 dispatch / BOOT 去对话化, 见文件头 Run protocol）。
    # **必须置于 host smoke acceptance 之前**（smoke 保持末位 = 最先执行, 包 0 boundary
    # 齿注册序守卫; 本包验收 test 5 tooth 1 校验此序）。相对 host_app/main 为 ../../。
    "../../key_route_a1.acceptance.c"

    # 包 260911-screen-unification (PKG-2 归一屏)：冻结验收 + 被测纯逻辑前台 layer 栈
    # (kidpal_layer.c 强符号覆盖 weak 哨兵——不编入则冻结离线契约仍红; 相对 host_app/
    # main 为 ../../../)。**必须置于 host smoke acceptance 之前**（smoke 保持末位 =
    # 最先执行, 包 0 boundary 齿注册序守卫; 验收 test s2 结构齿校验此序）。
    "../../screen_unification.acceptance.c"
    "../../../display/kidpal_layer.c"

    # 包 260911-render-pages (批次③ V3 整屏还原 + 五态演出): 冻结验收 v3_restore
    # (五态纯逻辑契约 6 齿 + 页面换皮框族/字体分型/提醒压暗等) + 被测纯逻辑五态内容
    # 选择模块 (kidpal_voice_states.c 强符号覆盖 weak 哨兵——不编入则冻结离线契约仍红;
    # 相对 host_app/main 为 ../../../)。**必须置于 host smoke acceptance 之前**（smoke
    # 保持列表末位 = 最先执行, 包 0 boundary 齿注册序守卫; v3_restore 无渲染齿, 不影响
    # 该守卫）。
    "../../v3_restore.acceptance.c"
    "../../../display/kidpal_voice_states.c"

    # happy_flow_capture.test.c (human 一次性授权截图轮, 2026-09-13): Happy Flow 各页
    # host 真渲染 -> 渲染缝 additive BMP 落盘缝。普通 .test.c 非冻结件。置于 smoke 之前
    # （执行序 = 列表逆序: 包 0 boundary 齿仍最先跑, 本文件随后, 既有齿不受扰动）。
    "../../happy_flow_capture.test.c"

    # 包 260915-m1-k1a-key-layer-dispatch（键位对账①②⑤ 落地面 / 分层派发改造）：冻结
    # 验收（路由级决策表 key_route_layer + 显式清层步骤 kidpal_layer_pop_transients 的行为
    # 半 + 板级 dispatch 分流/关层步骤/裁决函数接线的结构半 + 零回归守卫 + 注册序守卫）。
    # **必须置于 host smoke acceptance 之前**（smoke 保持末位 = 最先执行, 包 0 boundary
    # 齿注册序守卫; 本包验收 test 5 tooth 1 校验此序）。相对 host_app/main 为 ../../。
    "../../key_layer_dispatch.acceptance.c"

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

# 测试专用组件编译豁免 (仅 host test infra, 不进设备构建): 冻结验收件 v3_restore
# (包 260911-render-pages) 首次编入 host -Werror 时, 其带动态字段的诊断消息触
# -Wformat-truncation (FM 模式: vsnprintf 进静态缓冲, %s 未知长度指针的良性告警;
# 验收件冻结不可改)。此处仅把该告警降为非错误 (仍显示), 不削弱其它更危险的
# -Werror 检查 (test-only 组件, 与设备构建无关)。
# 第二条豁免 (2026-09-13 全量重配时暴露): 多个冻结验收件的 REVISION LOG 注释携带
# 路径字面量 `docs/contracts/**` / `managed_components/**` —— GCC -Wcomment 对块
# 注释内的 `/*` 报警, 属注释体文字、非嵌套注释语法错。冻结件不可改, 仅把
# -Wcomment 降为非错误 (仍显示), 语义零影响。
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error=format-truncation -Wno-error=comment)