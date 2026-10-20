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

    # 包 260915-m1-k3-voice-round-ui（语音轮演出补齐）被测实现：voice_round.c（[1]
    # 纯逻辑单源：kidpal_voice_round_scene 场景映射决策表 + kidpal_voice_echo_* 逐句回显
    # 机制，强符号覆盖 weak 哨兵）。相对 host_app/main 为 ../../../display/。
    "../../../display/voice_round.c"

    # 包 260915-m1-k3-voice-round-ui 冻结验收（场景决策表行为半 + 回显逐句机制 +
    # 活侧渲染接线结构齿 + 打断衔接/闭集零偏移 + 注册序守卫）。**必须置于 host smoke
    # acceptance 之前**（smoke 末位 = 最先执行；本文件 test 5 tooth 1 校验此序）。
    "../../voice_round_ui.acceptance.c"

    # 包 260915-m1-k4-reminder-overlay-unify（提醒弹窗四态统一骨架 + 最高层覆盖 + 录音
    # 排队例外）被测实现：reminder_page.c（[1] 入层门控 kidpal_reminder_gate + [2] 时间行
    # 单源 kidpal_reminder_hhmm + [3] 状态机九符号强定义，覆盖本验收 weak 哨兵）。
    # 其 kidpal_art_* 引用由 p1_home_battery.acceptance.c 在世 weak 桩覆盖（与 home_p1.c
    # 同款, 无需另接 art_assets.c）。**必须置于 host smoke acceptance 之前**（smoke 末位
    # = 最先执行; 本文件 test 8 tooth 1 校验此序）。
    "../../../display/reminder_page.c"

    # 包 260915-m1-k4-reminder-overlay-unify 冻结验收（门控决策表 + 时间行单源 + 排队/
    # 去重/驻留/过期状态机回归 + 活侧接线结构齿 + 注册序守卫）。**必须置于 host smoke
    # acceptance 之前**（smoke 末位 = 最先执行; 本文件 test 8 tooth 1 校验此序）。
    "../../reminder_overlay_unify.acceptance.c"

    # 包 260917-m2-k1-char-link-defect-fix（查字链路两缺陷修复：C 命令三次贝塞尔 +
    # 动画帧去大栈数组 + taskLVGL 栈定容收编）：冻结验收（C 命令零丢笔/离散语义 +
    # M/L/Q/Z 逐位回归/防御面零回归 + 栈修复结构齿 + 注册序守卫）。强符号源
    # char_stroke.c 覆盖 weak 哨兵（其 art_assets.h 引用为纯声明头, parse 路径不调
    # art 函数, 无需另接 art_assets.c——260829 先例）。**必须置于 host smoke acceptance
    # 之前**（smoke 末位 = 最先执行; 本文件 test 4 tooth 1 校验此序）。
    "../../char_stroke.acceptance.c"
    "../../../display/char_stroke.c"

    # 包 260917-m2-k2-retract-p4-three-page（撤 P4 常驻页：页循环 4→3 + 两页枚举对齐 spec
    # P1→P2→P3 + 键帽条页点 4→3）冻结验收（key_page_next 三页回绕 + 枚举对齐/字面 3 单点
    # + keybar 页点同源与第 4 页索引越界的真行为半；render_node/lcd_display/mcp_server 切区
    # 与文本结构半；键路由回归靶 + 查字通路保留靶 + 注册序/需求契约 canary）。被测实现
    # key_gesture_parser.cc / display/keybar.c（+ 其链接依赖 display/kidpal_voice_states.c）
    # 均已在册，本包零新增实现接线。**必须置于 host smoke acceptance 之前**（smoke 末位 =
    # 最先执行; 本文件 test 7 tooth 1 校验此序）。
    "../../retract_p4_three_page.acceptance.c"

    # 包 260917-m2-k3-char-window-overlay（M2-K3 页→窗改造：查字窗层注册 + 键位语义矩阵 +
    # 5 分钟驻留计时 + 页点隐藏 + 右栏对账三项 + 锚改名对账）。冻结验收（层栈行为/结构半 +
    # 查字窗列路由决策表 + 驻留计时行为/结构半 + 右栏对账结构半 + 注册序/需求契约 canary）。
    # 新增纯逻辑模块 display/char_window.c = seam [3] 驻留计时的强符号源（覆盖冻结件 weak
    # 哨兵）。被测实现 kidpal_layer.c / key_gesture_parser.cc 已在册。**必须置于 host smoke
    # acceptance 之前**（smoke 末位 = 最先执行; 本文件 test 7 tooth 1 校验此序）。
    "../../char_window_overlay.acceptance.c"
    "../../../display/char_window.c"

    # 包 260917-m2-k3-char-window-overlay builder 单元测试（驻留计时 / 查字窗层栈 /
    # 查字窗列路由的边界与分支补强——冻结齿未穷尽的边界面）。置于 host smoke acceptance 之前。
    "../../char_window.test.c"

    # 包 260921-m3-k2-p2-week-grid-page（P2 一周总览网格页整页重写 + 首页「接下来」
    # 注入接线）被测实现：schedule_page.c（kidpal_schedule_grid_parse/grid_render 冻结
    # 离线契约强符号源, 覆盖冻结件 weak 哨兵; schedule_page.h 声明头）+ home_upcoming.c
    # （kidpal_home_upcoming_apply A5 接线层强符号源; home_upcoming.h 声明头; 只消费
    # home_p1 冻结 seam——home_p1.c 强符号已在册, 覆盖冻结件 seam 读侧 weak 哨兵,
    # p1_home_battery 同款）。相对 host_app/main 为 ../../../display/。
    "../../../display/schedule_page.c"
    "../../../display/home_upcoming.c"

    # 包 260921-m3-k2-p2-week-grid-page 冻结验收（A1 版式结构 + A4 周网格解析降级 +
    # A2 今天列注入缝 + A5 upcoming 注入 + A6 回归靶 + LVGL 软渲染真帧探针）。
    # **必须置于 host smoke acceptance 之前**（smoke 末位 = 最先执行, 包 0 boundary
    # 齿注册序守卫）。
    "../../schedule_grid.acceptance.c"

    # 包 260921-m3-k4-p1-cjk-dynamic-text（首页 P1 动态日程名 CJK 渲染）被测实现接线：
    # 本包追加轮 [S5] 烘焙固定标签（p1_label_jiexialai.bin / p1_label_mingzao.bin）
    # 由 art_assets.c 提供 kidpal_art_mount/load/pixel 强符号（否则 p1_cjk_name TEST 7/8
    # 走 HPB 弱桩恒败 = 自诊断 infra 红；BUILDER DUTY 5))。未挂载态 art_assets.c 强符号
    # 语义 == 既有弱桩（load/glyph -1、advance -1），其余齿兜底路径零扰动。
    "../../../display/art_assets.c"

    # 包 260921-m3-k4-p1-cjk-dynamic-text（首页 P1 动态日程名 CJK 渲染）冻结验收：
    # [S1] 字形 seam 三符号 + [S2] UTF-8 解码逐字形 blit + 三钉区墨/截断/截半容错/
    # 缺字形退化 + 注册接线守卫。被测实现 home_p1.c 已在册（seam weak 由本包新增
    # host_infra_seam.c 强符号 + kidpal_p1_font.h 声明覆盖）。**必须置于 host smoke
    # acceptance 之前**（smoke 末位 = 最先执行, 本文件 TEST 6 tooth 4 钉存在性）。
    "../../p1_cjk_name.acceptance.c"

    # 包 260921-m3-k4-p1-cjk-dynamic-text 第三追加轮（A11, [S6]）设备字体上 host
    # (human 硬要求 2026-09-22; BUILDER DUTY 8))：把设备字体 font_puhui_14_1.c
    # 编入 host runner 源清单（纯 C、lv_font_fmt_txt 格式、bpp=1 --no-compress,
    # lvgl.h include 在 host 通道可达——REQUIRES lvgl 已在 include path）。相对
    # host_app/main 为 ../../../managed_components/。managed_components 第三方面
    # 只读：编入 = 消费, 非修改。TEST 10 tooth 0a weak 探针经此编入解析到强定义。
    "../../../../managed_components/78__xiaozhi-fonts/src/font_puhui_14_1.c"

    # 包 260922-m3-k5-sntp-time-sync（SNTP 校时 + PCF85063 写回 + 断电续航 + 周期
    # re-inject）被测实现接线：time_sync.c = seam [S1] 纯逻辑时间缝六函数强符号源
    # （覆盖冻结件 weak 哨兵的关键）。相对 host_app/main 为 ../../../（= firmware/main/）。
    "../../../time_sync.c"

    # 包 260922-m3-k5-sntp-time-sync 冻结验收（BCD 编解码 + 守门阈值/可信门 + 退避/节流
    # 决策表行为半 + SNTP 挂点/写回/恢复/re-inject/接线结构半 + 铁律守卫 + 注册序守卫）。
    # **必须置于 host smoke acceptance 之前**（smoke 末位 = 最先执行, 包 0 boundary
    # 齿注册序守卫; 本文件 TEST 6 tooth 5 校验此序）。
    "../../time_sync.acceptance.c"

    # 包 260923-m3-k6-boot-auto-connect（开机自动建连 + 断网重连自动恢复 + 静默失败
    # + in-flight 生命周期 + 人工路径守卫）冻结验收：自动建连缝正锚 / OnNetworkError
    # 拦截 + 有界退避决策表 / 未配网预检只读 / 全部为结构半（守卫/拦截/生命周期注入缝
    # 逻辑 host 机判）+ 回归 + 注册序守卫。**必须置于 host smoke acceptance 之前**
    # （smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫）。
    "../../boot_auto_connect.acceptance.c"

    # 包 260923-m3-k7-schedule-grid-restore（课表网格冷启动恢复：云端三触发附带推
    # data.restore 周网格恢复性载荷, 设备只填缓存不接管前台）冻结验收：restore 消费
    # 分支（读 "restore" + 显式持 LVGL 锁 + on-screen 谓词消费 + begin_page 计数恒 5 +
    # SetChatMessage 区计数恒 1 静默降级）+ 在屏谓词/缓存幂等形态 + 语音接管回归守卫 +
    # restore wire 容忍行为齿（weak 哨兵由在世 schedule_page.c 强定义覆盖）+ 注册/canary
    # 守卫。**必须置于 host smoke acceptance 之前**（smoke 末位 = 最先执行, 包 0
    # boundary 齿注册序守卫; 本文件 TEST 5 tooth 4 校验此序）。

    "../../schedule_grid_restore.acceptance.c"

    # 包 260924-m4-k2-p3-star-notebook-ui（P3 星星记录本展示面重构）被测实现接线：
    # display/story_page.c = 新冻结齿 story_page_v2 的行为齿强符号源（v1.47 输入表
    # 矩阵 + 列表分屏/消歧/删除子态/重发对账/seq 消费纯逻辑 + 渲染面重写；其
    # kidpal_art_* 引用由在世 art_assets.c 强符号覆盖、font_puhui_14_1 由本清单
    # 在世字体源覆盖，无需另接）。相对 host_app/main 为 ../../../display/。
    "../../../display/story_page.c"

    # 包 260924-m4-k2-p3-star-notebook-ui 冻结验收（LIST 态 v1.47 决策表 + 分屏/
    # 消歧/删除子态/重发对账/阅读/计数双口径/盖章 seq 行为半 + CJK 真帧 + 渲染面/
    # 删除链路结构半 + 层值接线 + 注册序守卫）。**必须置于 host smoke acceptance
    # 之前**（smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫; 本文件 SPV2-C1
    # tooth 1 校验此序）。
    "../../story_page_v2.acceptance.c"

    # 包 260924-m4-k2-p3-star-notebook-ui builder 单元测试（冻结齿未穷尽的边角：
    # cursor_step 跨屏普通步进 / 局部末屏跳屏钳制 / stamp_text 恰好满 cap /
    # text_seq 空白与负值 / 输入表未知页值兜底 / 消歧窗回绕边界）。置于 host
    # smoke acceptance 之前。
    "../../story_page_v2.test.c"

    # 包 260924-m4-k3-record-session-live（记录会话跨端链路：session_ctx 发送缝 +
    # 引导态/录音态 + 手势会话接线 + 回显消费 + story_list 推送 gating）冻结验收。
    # 行为齿 [S1]-[S4] 强符号源 = display/story_page.c（已在册，尾部追加缝覆盖
    # weak 哨兵）；[S5]-[S7] 为 needle 结构齿，扫真实工作树。**必须置于 host
    # smoke acceptance 之前**（smoke 末位 = 最先执行, 包 0 boundary 齿注册序
    # 守卫; 本文件 RSL-C1 tooth 4 校验此序）。
    "../../record_session_live.acceptance.c"

    # 包 261001-m4-k3-record-session-live-repair（K3 落地后真机 P3 冒烟四缺陷止血：
    # D1 列表延迟推送无消费方 / D2 会话层压录音页 / D4a 录音态 KEY 落全局 toggle 毁稿 /
    # D4b 录音定时器致首页图像对象悬垂 panic）冻结验收。行为齿 [R1]-[R3] 强符号源 =
    # display/story_page.c（已在册，尾部追加缝覆盖 weak 哨兵）；结构齿扫真实工作树。
    # **必须置于 host smoke acceptance 之前**（smoke 末位 = 最先执行, 包 0 boundary
    # 齿注册序守卫; 本文件 RSR-C1 tooth 4 校验此序）。
    "../../record_session_repair.acceptance.c"

    # 包 261001-m4-k3-record-session-live-repair builder 单元测试（冻结齿未穷尽的分支/
    # 边界：R1 规则优先级与 ctx 非零语义 / R2 越界与幂等 / R3 非零与非列表态）。置于
    # host smoke acceptance 之前。
    "../../record_session_repair.test.c"

    "../../voice_rightcol_label.acceptance.c"

    # 包 261015-p1-auxbar-style-fix（P1 辅助条像素级观感修复：电池/温/湿图标切原生
    # 直出件 + 日期简写全大写 + wifi 资产重出守卫）冻结验收。纯树扫（home_p1.c 不进
    # host 构建——源锚齿 tree-scan 承载）。**必须置于 host smoke acceptance 之前**
    # （smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫）。
    "../../p1_auxbar_style.acceptance.c"

    # 包 261016-sd-card-mount（TF 卡接入——SDMMC 1-bit 挂载 + /sdcard 能力就绪，降级
    # 不崩）冻结验收：纯树扫源锚结构齿（板级 config.h / .cc 不进 host 构建——源锚齿
    # tree-scan 承载，261015-p1-auxbar-style-fix 同款）。**必须置于 host smoke
    # acceptance 之前**（smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫）。
    "../../sd_card_mount.acceptance.c"

    # 包 261016-p3-story-body-sd-cache（P3 story 正文本地缓存：SD 只读镜像 + 列表
    # 预取 + 双击兜底 + id 校验修复）被测实现：display/story_cache.c = seam
    # [C1]-[C9] 纯逻辑七缝强符号源（覆盖冻结件 weak 哨兵）。SD 真 I/O
    # (boards/*/story_sd_io.cc) 是 chip 侧、host 不可达（GAP-2 真机轮承载）。
    "../../../display/story_cache.c"

    # 包 261016-p3-story-body-sd-cache 冻结验收（预取决策 / 三分支 / id 校验 /
    # 写卡门控 / 覆盖层骨架 + 跨端接线结构齿）。**必须置于 host smoke acceptance
    # 之前**（smoke 末位 = 最先执行）。
    "../../story_cache.acceptance.c"

    # 包 261018-sd-log-persist（板端诊断日志落 TF 卡：有界轮转，含 STT/LLM 原文）
    # 被测实现 + 冻结验收。diagnostics/sd_log.c = [C1]-[C3] 纯逻辑强符号源（环形缓冲 /
    # 8.3 轮转命名 / 刷盘与重试决策表，覆盖冻结件 weak 哨兵）；sd_log_fs.cc 是芯片面
    # （vprintf 钩子 / PSRAM / 写盘任务），host 不可达、不进本构建。相对 host_app/main
    # 分别为 ../../../diagnostics/ 与 ../../。**必须置于 host smoke acceptance 之前**。
    "../../../diagnostics/sd_log.c"
    "../../sd_log.acceptance.c"

    # 包 261018-sd-log-usb-read（诊断日志按需 USB 串口取回：只读命令 kplog list/tail/cat）
    # 被测实现 + 冻结验收。diagnostics/log_cmd.c = [S2]-[S5] 纯逻辑强符号源（命令解析 /
    # 行装配 / 帧格式化 / 尾部窗口，覆盖冻结件 weak 哨兵；零 FATFS / 零 FreeRTOS）；
    # diagnostics/log_cmd_port.cc 是芯片面（/dev/secondary VFS 读任务 + FATFS 只读 +
    # stdout 输出），host 不可达、**不进本构建**（冻结件 TEST 7 负向守校验）。相对
    # host_app/main 分别为 ../../../diagnostics/ 与 ../../。**必须置于 host smoke
    # acceptance 之前**（smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫）。
    "../../../diagnostics/log_cmd.c"
    "../../log_cmd.acceptance.c"

    # 包 261019-p3-record-dialogue-guide（P3 对话式引导记录：VRT_CTRL_START_ORGANIZING
    # additive / 整理过渡态 / 引导播报中子态键位分叉 / kvr 场景守卫 / 控制消费闭环）
    # 被测实现 + 冻结验收。display/view_router.c = [G1] kidpal_display_control_action
    # 五值决策表强符号源（覆盖冻结件 weak 哨兵；head 注「view_router.c 已在册」与本
    # 清单现况不符——本包首次登记, builder 交付注记）; display/story_page.c 已在册
    # （尾部追加 rec_substate / ORGANIZING case 覆盖 [G2]-[G5] weak 哨兵）; 结构齿扫
    # 真实工作树（application.cc / mcp_server.cc 不进 host 构建）。相对 host_app/main
    # 分别为 ../../../display/ 与 ../../。**必须置于 host smoke acceptance 之前**
    # （smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫）。
    "../../../display/view_router.c"
    "../../record_dialogue_guide.acceptance.c"

    # 261019-p3-record-dialogue-guide fixer 轮二（2026-10-20）guard 单测: keybar v7
    # 键帽集行为齿（keybar.c 强符号真跑）+ D2 根修缝/守卫与 D3 资产落点源锚结构齿
    # （story_page.c / mcp_server.cc / kidpal_render_node.cc 设备 TU 不进 host 构建,
    # 源锚代理; 渲染节点缝 host 半 = host_infra_seam.c 同符号实现）。置于 smoke 之前。
    "../../record_dialogue_v7_fix.test.c"

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