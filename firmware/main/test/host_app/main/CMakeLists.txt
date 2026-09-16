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
# 2026-09-15 归档（human 裁定，M1 阶段一已进入）：MVP/验证期 13 个冻结验收（260829–
# 260911 批）已移入 ../acceptance/archive/（清单与纪律见该目录 README.md），不再编入
# host runner 每轮全量跑。现役冻结验收仅剩 key_layer_dispatch（M1-K1a）；M1 后续包
# （K2/K3/K4/N1b）各自新写冻结验收时在此追加，置于 host_infra_smoke 之前。
# 2026-09-15 归档第二批（human 裁定）：6 个普通 .test.c/.cpp 单测（cascade / reminder /
# capture / stack / provision / seam_bounds）与仅被它们消费的被测实现（home_p1/schedule_page/
# story_page/char_stroke/art_assets/reminder_page.c 六个页面·资产模块）一并移入
# ../acceptance/archive/（清单见该目录 README.md）。现役 = key_layer_dispatch +
# host_infra_smoke 两件及其被测实现 key_gesture_parser.cc / kidpal_layer.c；260915 批
# M1-N1b 起各包新冻结验收与其被测实现按下文 HOST_ACCEPTANCE_SRCS 追加。
#
# The FROZEN acceptance file is consumed DIRECTLY from firmware/main/test/ (one
# level up from this component dir) — single source of truth, never copied. A source
# file outside the component source dir is fine here: this is a test-only project.
set(HOST_ACCEPTANCE_SRCS
    # 被测实现（key_layer_dispatch 行为齿的强符号源：key_route_layer 决策表 +
    # kidpal_layer_pop_transients 清层步骤；相对 host_app/main 为 ../../../）。
    "../../../key_gesture_parser.cc"
    "../../../display/kidpal_layer.c"

    # 包 260915-m1-k1a-key-layer-dispatch（键位对账①②⑤ 落地面 / 分层派发改造）：冻结
    # 验收（路由级决策表 key_route_layer + 显式清层步骤 kidpal_layer_pop_transients 的行为
    # 半 + 板级 dispatch 分流/关层步骤/裁决函数接线的结构半 + 零回归守卫 + 注册序守卫）。
    # **必须置于 host smoke acceptance 之前**（smoke 保持末位 = 最先执行, 包 0 boundary
    # 齿注册序守卫; 本包验收 test 5 tooth 1 校验此序）。相对 host_app/main 为 ../../。
    "../../key_layer_dispatch.acceptance.c"

    # 包 260915-m1-n1b-p1-home-battery（首页新版式 + 电量 ADC 接线）被测实现：home_p1.c
    # 冻结离线契约实现（行为齿强符号覆盖 weak 哨兵的关键；相对 host_app/main 为
    # ../../../display/）。art_assets.c 不接（走兜底描边路径，钉区有墨义务同样成立；
    # 挂资产后图标齿才探真资产像素）。
    "../../../display/home_p1.c"

    # 包 260915-m1-n1b-p1-home-battery 冻结验收（新版式 pixel-probe + 低电闪烁决策表 +
    # 注入缝/板级接线结构齿 + 断网钉点 + 注册序守卫）。**必须置于 host smoke acceptance
    # 之前**（smoke 末位 = 最先执行；本文件 test 5 tooth 1 校验此序）。
    "../../p1_home_battery.acceptance.c"

    # 包 260915-m1-k2-keybar-three-slot（键帽条三槽改造）被测实现：keybar.c（[1] 纯逻辑
    # 单源：kidpal_keybar_dots/slots 强符号覆盖 weak 哨兵；含 kidpal_keybar_scene 闭集单源
    # 引用链）+ kidpal_voice_states.c（[2] 冻结闭集单源，在世实现首次入 runner，强符号覆盖
    # kidpal_voice_state_look weak 哨兵）。相对 host_app/main 为 ../../../display/。
    "../../../display/keybar.c"
    "../../../display/kidpal_voice_states.c"

    # 包 260915-m1-k2-keybar-three-slot 冻结验收（页点/左右槽纯逻辑决策表 + 闭集零偏移
    # 行为复核 + 三槽结构/layer_top 同源/零裁决权结构齿 + 注册序守卫）。**必须置于 host
    # smoke acceptance 之前**（smoke 末位 = 最先执行；本文件 test 5 tooth 1 校验此序）。
    "../../keybar_three_slot.acceptance.c"

    # host_infra_smoke (包 0 守卫) LAST — 见本文件头注释。
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