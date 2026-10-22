# Define source files
set(SOURCES "audio/audio_codec.cc"
            "audio/audio_service.cc"
            "audio/codecs/no_audio_codec.cc"
            "audio/codecs/box_audio_codec.cc"
            "audio/codecs/es8311_audio_codec.cc"
            "audio/codecs/es8374_audio_codec.cc"
            "audio/codecs/es8388_audio_codec.cc"
            "audio/codecs/es8389_audio_codec.cc"
            "audio/codecs/dummy_audio_codec.cc"
            "audio/processors/audio_debugger.cc"
            "led/single_led.cc"
            "led/circular_strip.cc"
            "led/gpio_led.cc"
            "display/display.cc"
            "display/lcd_display.cc"
            "display/oled_display.cc"
            "display/lvgl_display/lvgl_display.cc"
            "display/emote_display.cc"
            "display/lvgl_display/emoji_collection.cc"
            "display/lvgl_display/lvgl_theme.cc"
            "display/lvgl_display/lvgl_font.cc"
            "display/lvgl_display/lvgl_image.cc"
            "display/lvgl_display/gif/lvgl_gif.cc"
            "display/lvgl_display/gif/gifdec.c"
            "display/lvgl_display/jpg/image_to_jpeg.cpp"
            "display/lvgl_display/jpg/jpeg_to_image.c"
            # display view 路由共享底座 (方向 B, 包 260829-voice-roundtrip; 包 4/5/6/8 消费)
            # 纯逻辑无芯片依赖 -> host_app linux target 同源 (firmware/main/test/host_app/main/CMakeLists.txt)
            "display/view_router.c"
            # P1 首页常显信息牌 (包 260829-home-p1): 冻结离线契约 + 帧构建, 纯逻辑无芯片
            # 依赖 -> host_app linux target 同源 (display/home_p1.c 自持弱渲染缝, 真机
            # LVGL 节点集成殿后)
            "display/home_p1.c"
            # P2 一周总览网格页消费面 (M3-K2, TaskPackage 260921-m3-k2-p2-week-grid-page,
            # 契约 §1.3② 周网格 data 2026-09-21 契约门定稿): 冻结离线契约 2 函数
            # (schedule_page.c / schedule_page.h, 家 display/; 纯逻辑+纯 LVGL ->
            # host 单测同源, 见 firmware/main/test/schedule_grid.acceptance.c)。
            "display/schedule_page.c"
            # 旧列表形状 {title,items} 消费面已随契约门 2026-09-21 废止, 由本文件整页
            # 重写退役（schedule_p2.acceptance.c 已归档）。
            # 首页「接下来」注入接线层 (M3-K2 A5, 契约 §1.3⑥ home 可选 data.upcoming):
            # home_upcoming.c / home_upcoming.h (家 display/; 纯逻辑零芯片依赖 ->
            # host 单测同源)。只消费 home_p1 冻结 seam, home_p1.{c,h} 零修改。
            "display/home_upcoming.c"
            # P3 故事页消费面 (包 260829-story-p3): 冻结离线契约 14 函数 + 纯 LVGL 渲染面。
            # story_page.c / story_page.h (家 display/; 纯逻辑+纯 LVGL -> host 单测同源,
            # 见 firmware/main/test/story_p3.acceptance.c)。
            "display/story_page.c"
            # P3 正文本地缓存纯逻辑面 (TaskPackage 261016-p3-story-body-sd-cache):
            # RAM 索引 / 当屏缺失 / 双击三分支决策 / 预取决策 / id 校验 / 盖章侧效应
            # 守卫——零 FATFS / 零 LVGL / 零芯片依赖 -> host_app linux target 同源
            # (接线见 test/host_app/main/CMakeLists.txt)。SD 真 I/O 薄适配在
            # boards/waveshare-s3-rlcd-4.2/story_sd_io.cc (chip 侧, 由板级 glob 收入)。
            "display/story_cache.c"
            # 查生字 P4 (包 260829-char-stroke): 光栅契约纯逻辑模块 + LVGL 查字页。
            # char_stroke.c 纯逻辑无芯片依赖 -> host_app linux target 同源 (甲烷线接线在 host_app)。
            "display/char_stroke.c"
            "display/char_page.cc"
            # 提醒弹窗消费面 (包 260829-schedule-reminder): 冻结离线契约 kidpal_alert_parse /
            # kidpal_fire_at_minutes / kidpal_reminder_* 状态机 + 纯 LVGL 富弹窗渲染
            # (reminder_page.c / reminder_page.h, 建议家 display/; 纯 C + 自足极简 JSON ->
            # host_app linux target 同源, 见 firmware/main/test/schedule_reminder_b.acceptance.c;
            # 介质禁令: known-id 集与队列定长驻 RAM, 零持久层/零动态分配)
            "display/reminder_page.c"
            # 美术资产共享层 (包 260901-ui-art-integration): 冻结离线契约 9 函数 (asset
            # loader + 渲染原语), 纯逻辑零芯片依赖 -> host_app linux target 同源 (接线在
            # host_app); 设备侧 esp spiffs VFS 注册放芯片装配层, 本模块只收 mount root。
            "display/art_assets.c"
            # 前台 layer 栈管理单一权威 (包 260911-screen-unification): 冻结离线契约
            # 纯逻辑模块, host_app linux target 与设备树同源 (接线见 host_app CMakeLists)。
            "display/kidpal_layer.c"
            # 语音轮五态演出内容选择单源 (包 260911-render-pages RP-A9): 纯逻辑模块
            # (场景 -> 姿态/气泡/键帽条), 无芯片依赖 -> host_app linux target 同源
            # (接线见 host_app CMakeLists)。层底座零触碰, 只做内容面映射。
            "display/kidpal_voice_states.c"
            # 键帽条三槽演出内容选择单源 (包 260915-m1-k2-keybar-three-slot): 纯逻辑模块
            # (槽位 -> 键帽 JSON/点阵), 无芯片依赖 -> host_app linux target 同源 (接线见
            # host_app CMakeLists)。kidpal_voice_states_closed_set 闭集零偏移。
            "display/keybar.c"
            # 语音轮三态演出内容选择单源 (包 260915-m1-k3-voice-round-ui): 纯逻辑模块
            # (kidpal_voice_echo_push / kidpal_voice_round_scene), 无芯片依赖 ->
            # host_app linux target 同源 (接线见 host_app CMakeLists)。
            "display/voice_round.c"
            # 查字窗驻留计时纯逻辑单源 (包 260917-m2-k3-char-window-overlay): 5 分钟无操作
            # 关窗步进 (kidpal_char_window_idle_step + KIDPAL_CHAR_WINDOW_IDLE_MS), 无芯片 /
            # FreeRTOS / LVGL 依赖 -> device 与 host_app linux target 同源 (host 接线见
            # host_app CMakeLists)。设备侧消费者 = kidpal_render_node_tick (1s 节拍累计 +
            # 查字窗前台门控, 到点经 kidpal_render_node_dismiss_front_overlay 关窗); 任意键
            # 归零入口 = kidpal_render_node_char_window_key_hit (板级 KEY/BOOT 按下回调调用)。
            "display/char_window.c"
            # 设备渲染节点 (包 9b HWV-A6 收口 + 包 260911-screen-unification 归一屏):
            # host_infra_render_p1_frame 强符号 + 4 页 LVGL 屏驱动 (KidPal 专属 s_kidpal_scr,
            # display/ 唯一被 lv_screen_load 的前台屏；xiaozhi 聊天 chrome 已 reparent 到
            # lv_layer_top 由板装配层触发)。不进入 host 构建 (host_app CMakeLists 不含本文件，
            # 与 char_page.cc 同理)。
            "display/kidpal_render_node.cc"
            # NFR-READ-1 字号真机对照实验页（human 指示一次性实验构建，不入产线）：
            # 9 档 1bpp NotoSansSC 字号 C 数组字体 (lv_font_conv 产线同源参数) + 顶层对照页。
            "display/font_size_test_page.cc"
            "display/font_test_10.c"
            "display/font_test_12.c"
            "display/font_test_14.c"
            "display/font_test_16.c"
            "display/font_test_18.c"
            "display/font_test_20.c"
            "display/font_test_24.c"
            "display/font_test_28.c"
            "display/font_test_32.c"
            "protocols/protocol.cc"
            "protocols/mqtt_protocol.cc"
            "protocols/websocket_protocol.cc"
            "provisioning/ws_provision.cc"
            "certs/ws_tls_ca.cc"
            "mcp_server.cc"
            "system_info.cc"
            "application.cc"
            "ota.cc"
            "settings.cc"
            "device_state_machine.cc"
            # 纯逻辑时间缝单源 (K5, TaskPackage 260922-m3-k5-sntp-time-sync, seam [S1]):
            # BCD 编解码 / 守门阈值 / 写回可信门 / NTP 退避 / 日志节流六函数, 零芯片依赖
            # -> host_app linux target 同源 (接线见 host_app CMakeLists HOST_ACCEPTANCE_SRCS)。
            "time_sync.c"
            # 会话状态机 + KEY/BOOT 按键手势解析（纯逻辑，包 3；host 单测同源）
            "session_state_machine.cc"
            "key_gesture_parser.cc"
            # 包 261018-sd-log-persist（板端诊断日志落 TF 卡：有界轮转）：
            # diagnostics/sd_log.c = 环形缓冲 / 8.3 轮转命名 / 刷盘与重试决策表纯逻辑
            # （零 FATFS / 零 FreeRTOS / 零 LVGL -> host_app linux target 同源）;
            # diagnostics/sd_log_fs.cc = esp_log vprintf 钩子 + PSRAM 128KB 分配 +
            # 低优先级写盘任务 + 挂载成功 attach（芯片面）。main 目录非 GLOB,
            # 两个源都必须在此显式登记（否则设备构建拿不到）。
            "diagnostics/sd_log.c"
            "diagnostics/sd_log_fs.cc"
            # 包 261018-sd-log-usb-read（诊断日志按需 USB 串口取回：只读命令
            # kplog list/tail/cat）：diagnostics/log_cmd.c = 命令解析 / 行装配 /
            # 帧格式化 / 尾部窗口纯逻辑（零 FATFS / 零 FreeRTOS / 零 LVGL ->
            # host_app linux target 同源）; diagnostics/log_cmd_port.cc =
            # /dev/secondary VFS 读任务 + FATFS 只读 + stdout 输出（芯片面）。
            # main 目录非 GLOB, 两个源都必须在此显式登记（persist 包教训：登记是
            # 缺口高发位）。log_cmd_port.cc 不进 host 构建。
            "diagnostics/log_cmd.c"
            "diagnostics/log_cmd_port.cc"
            # 包 261022-diag-log-content（板端诊断日志内容增强）：diagnostics/log_semantics.c =
            # 墙上时间前缀格式化决策 + 预格式化缓存 + action/key/gesture/page/trigger 语义名
            # 映射 + SystemInfo 降频决策纯逻辑（零芯片依赖 / 零 FreeRTOS / 零 LVGL ->
            # host_app linux target 同源）; diagnostics/log_semantics.h = 冻结数值单源
            # （KIDPAL_LOG_TIME_PREFIX_CAP=32 / KIDPAL_LOG_SYSINFO_INTERVAL_MS=60000LL）。
            # main 目录非 GLOB, 必须在此显式登记（persist 包教训：登记是缺口高发位）。
            "diagnostics/log_semantics.c"
            "assets.cc"
            "main.cc"
            )

set(INCLUDE_DIRS "." "display" "display/lvgl_display" "display/lvgl_display/jpg" "audio" "protocols" "provisioning" "certs")

# Add board common files
file(GLOB BOARD_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/boards/common/*.cc)
list(APPEND SOURCES ${BOARD_COMMON_SOURCES})
list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/boards/common)
# 生成头 lang_config.h 迁至 build 目录后，把组件 binary dir 追加进 include 搜索路径，
# 使既有 include 字面量（全树统一形态 assets/lang_config.h，相对 main/ 解析）解析到生成的头，include 站点零改动。
list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR})

idf_build_get_property(build_components BUILD_COMPONENTS)
# Function to find component dynamically by pattern
function(find_component_by_pattern PATTERN COMPONENT_VAR PATH_VAR)
    foreach(COMPONENT ${build_components})
        if(COMPONENT MATCHES "${PATTERN}")
            set(${COMPONENT_VAR} ${COMPONENT} PARENT_SCOPE)
            idf_component_get_property(COMPONENT_PATH ${COMPONENT} COMPONENT_DIR)
            set(${PATH_VAR} "${COMPONENT_PATH}" PARENT_SCOPE)
            break()
        endif()
    endforeach()
endfunction()

# Set default BUILTIN_TEXT_FONT and BUILTIN_ICON_FONT
set(BUILTIN_TEXT_FONT font_puhui_14_1)
set(BUILTIN_ICON_FONT font_awesome_14_1)

# Add board files according to BOARD_TYPE
# Set default assets if the board uses partition table V2
if(CONFIG_BOARD_TYPE_BREAD_COMPACT_WIFI)
    set(BOARD_TYPE "bread-compact-wifi")
    set(BUILTIN_TEXT_FONT font_puhui_basic_14_1)
    set(BUILTIN_ICON_FONT font_awesome_14_1)
elseif(CONFIG_BOARD_TYPE_BREAD_COMPACT_ML307)
    set(BOARD_TYPE "bread-compact-ml307")
    set(BUILTIN_TEXT_FONT font_puhui_basic_14_1)
    set(BUILTIN_ICON_FONT font_awesome_14_1)
elseif(CONFIG_BOARD_TYPE_BREAD_COMPACT_ESP32)
    set(BOARD_TYPE "bread-compact-esp32")
elseif(CONFIG_BOARD_TYPE_BREAD_COMPACT_ESP32_LCD)
    set(BOARD_TYPE "bread-compact-esp32-lcd")
    set(BUILTIN_TEXT_FONT font_puhui_basic_14_1)
    set(BUILTIN_ICON_FONT font_awesome_14_1)
elseif(CONFIG_BOARD_TYPE_DF_K10)
    set(BOARD_TYPE "df-k10")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_DF_S3_AI_CAM)
    set(BOARD_TYPE "df-s3-ai-cam")
elseif(CONFIG_BOARD_TYPE_ESP_BOX_3)
    set(BOARD_TYPE "esp-box-3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ESP_BOX)
    set(BOARD_TYPE "esp-box")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ESP_BOX_LITE)
    set(BOARD_TYPE "esp-box-lite")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_KEVIN_BOX_2)
    set(BOARD_TYPE "kevin-box-2")
    set(BUILTIN_TEXT_FONT font_puhui_basic_14_1)
    set(BUILTIN_ICON_FONT font_awesome_14_1)
elseif(CONFIG_BOARD_TYPE_KEVIN_C3)
    set(BOARD_TYPE "kevin-c3")
elseif(CONFIG_BOARD_TYPE_KEVIN_SP_V3_DEV)
    set(BOARD_TYPE "kevin-sp-v3-dev")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_KEVIN_SP_V4_DEV)
    set(BOARD_TYPE "kevin-sp-v4-dev")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_KEVIN_YUYING_313LCD)
    set(BOARD_TYPE "kevin-yuying-313lcd")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_LICHUANG_DEV_S3)
    set(BOARD_TYPE "lichuang-dev")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_LICHUANG_DEV_C3)
    set(BOARD_TYPE "lichuang-c3-dev")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_MAGICLICK_S3_2P4)
    set(BOARD_TYPE "magiclick-2p4")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_MAGICLICK_S3_2P5)
    set(BOARD_TYPE "magiclick-2p5")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_MAGICLICK_C3)
    set(BOARD_TYPE "magiclick-c3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_MAGICLICK_C3_V2)
    set(BOARD_TYPE "magiclick-c3-v2")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_M5STACK_CORE_S3)
    set(BOARD_TYPE "m5stack-core-s3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_M5STACK_CORE_TAB5)
    set(BOARD_TYPE "m5stack-tab5")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_M5STACK_ATOM_S3_ECHO_BASE)
    set(BOARD_TYPE "atoms3-echo-base")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_M5STACK_ATOM_S3R_ECHO_BASE)
    set(BOARD_TYPE "atoms3r-echo-base")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_M5STACK_ATOM_S3R_CAM_M12_ECHO_BASE)
    set(BOARD_TYPE "atoms3r-cam-m12-echo-base")
elseif(CONFIG_BOARD_TYPE_M5STACK_ATOM_ECHOS3R)
    set(BOARD_TYPE "atom-echos3r")
elseif(CONFIG_BOARD_TYPE_M5STACK_ATOM_MATRIX_ECHO_BASE)
    set(BOARD_TYPE "atommatrix-echo-base")
elseif(CONFIG_BOARD_TYPE_XMINI_C3_V3)
    set(BOARD_TYPE "xmini-c3-v3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_14_1)
    set(BUILTIN_ICON_FONT font_awesome_14_1)
elseif(CONFIG_BOARD_TYPE_XMINI_C3_4G)
    set(BOARD_TYPE "xmini-c3-4g")
    set(BUILTIN_TEXT_FONT font_puhui_basic_14_1)
    set(BUILTIN_ICON_FONT font_awesome_14_1)
elseif(CONFIG_BOARD_TYPE_XMINI_C3)
    set(BOARD_TYPE "xmini-c3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_14_1)
    set(BUILTIN_ICON_FONT font_awesome_14_1)
elseif(CONFIG_BOARD_TYPE_ESP_KORVO2_V3)
    set(BOARD_TYPE "esp32s3-korvo2-v3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ESP_SPARKBOT)
    set(BOARD_TYPE "esp-sparkbot")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ESP_SPOT_S3)
    set(BOARD_TYPE "esp-spot")
elseif(CONFIG_BOARD_TYPE_ESP_SPOT_C5)
    set(BOARD_TYPE "esp-spot")
elseif(CONFIG_BOARD_TYPE_ESP_HI)
    set(BOARD_TYPE "esp-hi")
    # Set ESP_HI emoji directory for DEFAULT_ASSETS_EXTRA_FILES
    set(DEFAULT_ASSETS_EXTRA_FILES "${CMAKE_BINARY_DIR}/emoji")
elseif(CONFIG_BOARD_TYPE_ECHOEAR)
    set(BOARD_TYPE "echoear")
    set(BUILTIN_TEXT_FONT font_puhui_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ESP_SENSAIRSHUTTLE)
    set(BOARD_TYPE "esp-sensairshuttle")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_AUDIO_BOARD)
    set(BOARD_TYPE "waveshare-s3-audio-board")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_TOUCH_AMOLED_1_8)
    set(BOARD_TYPE "esp32-s3-touch-amoled-1.8")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_TOUCH_AMOLED_2_06)
    set(BOARD_TYPE "waveshare-s3-touch-amoled-2.06")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_C6_TOUCH_AMOLED_2_06)
    set(BOARD_TYPE "waveshare-c6-touch-amoled-2.06")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_TOUCH_LCD_4B)
    set(BOARD_TYPE "waveshare-s3-touch-lcd-4b")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_TOUCH_AMOLED_1_75)
    set(BOARD_TYPE "waveshare-s3-touch-amoled-1.75")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_TOUCH_LCD_1_83)
    set(BOARD_TYPE "waveshare-s3-touch-lcd-1.83")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_TOUCH_LCD_1_85C)
    set(BOARD_TYPE "esp32-s3-touch-lcd-1.85c")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_TOUCH_LCD_1_85)
    set(BOARD_TYPE "esp32-s3-touch-lcd-1.85")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_TOUCH_LCD_1_46)
    set(BOARD_TYPE "esp32-s3-touch-lcd-1.46")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_TOUCH_LCD_3_5)
    set(BOARD_TYPE "esp32-s3-touch-lcd-3.5")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_TOUCH_LCD_3_5B)
    set(BOARD_TYPE "waveshare-s3-touch-lcd-3.5b")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_ePaper_1_54)
    set(BOARD_TYPE "waveshare-s3-epaper-1.54")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_RLCD_4_2)
    set(BOARD_TYPE "waveshare-s3-rlcd-4.2")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_TOUCH_LCD_3_49)
    set(BOARD_TYPE "waveshare-s3-touch-lcd-3.49")
    set(LVGL_TEXT_FONT font_puhui_basic_30_4)
    set(LVGL_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_C6_LCD_1_69)
    set(BOARD_TYPE "waveshare-c6-lcd-1.69")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_C6_TOUCH_LCD_1_83)
    set(BOARD_TYPE "waveshare-c6-touch-lcd-1.83")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_C6_TOUCH_AMOLED_1_43)
    set(BOARD_TYPE "waveshare-c6-touch-amoled-1.43")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_C6_TOUCH_AMOLED_1_32)
    set(BOARD_TYPE "waveshare-c6-touch-amoled-1.32")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_S3_TOUCH_AMOLED_1_32)
    set(BOARD_TYPE "waveshare-s3-touch-amoled-1.32")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_P4_NANO)
    set(BOARD_TYPE "waveshare-p4-nano")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_P4_WIFI6_TOUCH_LCD_4B)
    set(BOARD_TYPE "waveshare-p4-wifi6-touch-lcd-4b")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_P4_WIFI6_TOUCH_LCD_7B)
    set(BOARD_TYPE "waveshare-p4-wifi6-touch-lcd-7b")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WAVESHARE_P4_WIFI6_TOUCH_LCD_XC)
    set(BOARD_TYPE "waveshare-p4-wifi6-touch-lcd-xc")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ESP_P4_FUNCTION_EV_BOARD)
    set(BOARD_TYPE "esp-p4-function-ev-board")
elseif(CONFIG_BOARD_TYPE_BREAD_COMPACT_WIFI_LCD)
    set(BOARD_TYPE "bread-compact-wifi-lcd")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_TUDOUZI)
    set(BOARD_TYPE "tudouzi")
    set(BUILTIN_TEXT_FONT font_puhui_basic_14_1)
    set(BUILTIN_ICON_FONT font_awesome_14_1)
elseif(CONFIG_BOARD_TYPE_LILYGO_T_CIRCLE_S3)
    set(BOARD_TYPE "lilygo-t-circle-s3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_LILYGO_T_CAMERAPLUS_S3_V1_0_V1_1)
    set(BOARD_TYPE "lilygo-t-cameraplus-s3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_LILYGO_T_CAMERAPLUS_S3_V1_2)
    set(BOARD_TYPE "lilygo-t-cameraplus-s3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_LILYGO_T_DISPLAY_S3_PRO_MVSRLORA)
    set(BOARD_TYPE "lilygo-t-display-s3-pro-mvsrlora")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_LILYGO_T_DISPLAY_S3_PRO_MVSRLORA_NO_BATTERY)
    set(BOARD_TYPE "lilygo-t-display-s3-pro-mvsrlora")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_LILYGO_T_DISPLAY_P4)
    set(BOARD_TYPE "lilygo-t-display-p4")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_MOVECALL_MOJI_ESP32S3)
    set(BOARD_TYPE "movecall-moji-esp32s3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_MOVECALL_MOJI2_ESP32C5)
    set(BOARD_TYPE "movecall-moji2-esp32c5")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_MOVECALL_CUICAN_ESP32S3)
    set(BOARD_TYPE "movecall-cuican-esp32s3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ATK_DNESP32S3)
    set(BOARD_TYPE "atk-dnesp32s3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ATK_DNESP32S3_BOX)
    set(BOARD_TYPE "atk-dnesp32s3-box")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ATK_DNESP32S3_BOX0)
    set(BOARD_TYPE "atk-dnesp32s3-box0")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ATK_DNESP32S3_BOX2_WIFI)
    set(BOARD_TYPE "atk-dnesp32s3-box2-wifi")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ATK_DNESP32S3_BOX2_4G)
    set(BOARD_TYPE "atk-dnesp32s3-box2-4g")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ATK_DNESP32S3M_WIFI)
    set(BOARD_TYPE "atk-dnesp32s3m-wifi")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_ATK_DNESP32S3M_4G)
    set(BOARD_TYPE "atk-dnesp32s3m-4g")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_DU_CHATX)
    set(BOARD_TYPE "du-chatx")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_TAIJI_PI_S3)
    set(BOARD_TYPE "taiji-pi-s3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_XINGZHI_CUBE_0_85TFT_WIFI)
    set(BOARD_TYPE "xingzhi-cube-0.85tft-wifi")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_XINGZHI_CUBE_0_85TFT_ML307)
    set(BOARD_TYPE "xingzhi-cube-0.85tft-ml307")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_XINGZHI_CUBE_0_96OLED_WIFI)
    set(BOARD_TYPE "xingzhi-cube-0.96oled-wifi")
    set(BUILTIN_TEXT_FONT font_puhui_basic_14_1)
    set(BUILTIN_ICON_FONT font_awesome_14_1)
elseif(CONFIG_BOARD_TYPE_XINGZHI_CUBE_0_96OLED_ML307)
    set(BOARD_TYPE "xingzhi-cube-0.96oled-ml307")
    set(BUILTIN_TEXT_FONT font_puhui_basic_14_1)
    set(BUILTIN_ICON_FONT font_awesome_14_1)
elseif(CONFIG_BOARD_TYPE_XINGZHI_CUBE_1_54TFT_WIFI)
    set(BOARD_TYPE "xingzhi-cube-1.54tft-wifi")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_XINGZHI_CUBE_1_54TFT_ML307)
    set(BOARD_TYPE "xingzhi-cube-1.54tft-ml307")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_XINGZHI_METAL_1_54_WIFI)
    set(BOARD_TYPE "xingzhi-metal-1.54-wifi")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)  
elseif(CONFIG_BOARD_TYPE_SEEED_STUDIO_SENSECAP_WATCHER)
    set(BOARD_TYPE "sensecap-watcher")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_DOIT_S3_AIBOX)
    set(BOARD_TYPE "doit-s3-aibox")
elseif(CONFIG_BOARD_TYPE_MIXGO_NOVA)
    set(BOARD_TYPE "mixgo-nova")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_GENJUTECH_S3_1_54TFT)
    set(BOARD_TYPE "genjutech-s3-1.54tft")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_CGC)
    set(BOARD_TYPE "esp32-cgc")
    set(BUILTIN_TEXT_FONT font_puhui_basic_14_1)
    set(BUILTIN_ICON_FONT font_awesome_14_1)
elseif(CONFIG_BOARD_TYPE_CGC_144)
    set(BOARD_TYPE "esp32-cgc-144")
    set(BUILTIN_TEXT_FONT font_puhui_basic_14_1)
    set(BUILTIN_ICON_FONT font_awesome_14_1)
elseif(CONFIG_BOARD_TYPE_ESP_S3_LCD_EV_Board)
    set(BOARD_TYPE "esp-s3-lcd-ev-board")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ESP_S3_LCD_EV_Board_2)
    set(BOARD_TYPE "esp-s3-lcd-ev-board-2")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ZHENGCHEN_1_54TFT_WIFI)
    set(BOARD_TYPE "zhengchen-1.54tft-wifi")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_MINSI_K08_DUAL)
    set(BOARD_TYPE "minsi-k08-dual")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_ZHENGCHEN_1_54TFT_ML307)
    set(BOARD_TYPE "zhengchen-1.54tft-ml307")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_SPOTPEAR_ESP32_S3_1_54_MUMA)
    set(BOARD_TYPE "sp-esp32-s3-1.54-muma")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_SPOTPEAR_ESP32_S3_1_28_BOX)
    set(BOARD_TYPE "sp-esp32-s3-1.28-box")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_OTTO_ROBOT)
    set(BOARD_TYPE "otto-robot")
    set(BUILTIN_TEXT_FONT font_puhui_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
elseif(CONFIG_BOARD_TYPE_ELECTRON_BOT)
    set(BOARD_TYPE "electron-bot")
    set(BUILTIN_TEXT_FONT font_puhui_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
elseif(CONFIG_BOARD_TYPE_BREAD_COMPACT_WIFI_CAM)
    set(BOARD_TYPE "bread-compact-wifi-s3cam")
    set(BUILTIN_TEXT_FONT font_puhui_basic_16_4)
    set(BUILTIN_ICON_FONT font_awesome_16_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_JIUCHUAN)
    set(BOARD_TYPE "jiuchuan-s3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_LABPLUS_MPYTHON_V3)
    set(BOARD_TYPE "labplus-mpython-v3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_LABPLUS_LEDONG_V2)
    set(BOARD_TYPE "labplus-ledong-v2")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_SURFER_C3_1_14TFT)
    set(BOARD_TYPE "surfer-c3-1.14tft")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_YUNLIAO_S3)
    set(BOARD_TYPE "yunliao-s3")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_WIRELESS_TAG_WTP4C5MP07S)
    set(BOARD_TYPE "wireless-tag-wtp4c5mp07s")
    set(BUILTIN_TEXT_FONT font_puhui_basic_30_4)
    set(BUILTIN_ICON_FONT font_awesome_30_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
elseif(CONFIG_BOARD_TYPE_AIPI_LITE)
    set(BOARD_TYPE "aipi-lite")
    set(BUILTIN_TEXT_FONT font_puhui_basic_14_1)
    set(BUILTIN_ICON_FONT font_awesome_14_1)
    set(DEFAULT_EMOJI_COLLECTION twemoji_32)
elseif(CONFIG_BOARD_TYPE_HU_087)
    set(BOARD_TYPE "hu-087")
    set(BUILTIN_TEXT_FONT font_puhui_basic_14_1)
    set(BUILTIN_ICON_FONT font_awesome_14_1)
endif()

file(GLOB BOARD_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/boards/${BOARD_TYPE}/*.c
)
list(APPEND SOURCES ${BOARD_SOURCES})

# Select audio processor according to Kconfig
if(CONFIG_USE_AUDIO_PROCESSOR)
    list(APPEND SOURCES "audio/processors/afe_audio_processor.cc")
else()
    list(APPEND SOURCES "audio/processors/no_audio_processor.cc")
endif()
if(CONFIG_IDF_TARGET_ESP32S3 OR CONFIG_IDF_TARGET_ESP32P4)
    list(APPEND SOURCES "audio/wake_words/afe_wake_word.cc")
    list(APPEND SOURCES "audio/wake_words/custom_wake_word.cc")
else()
    list(APPEND SOURCES "audio/wake_words/esp_wake_word.cc")
endif()

# Auto Select Additional Sources
if (CONFIG_USE_ESP_BLUFI_WIFI_PROVISIONING)
    list(APPEND SOURCES "boards/common/blufi.cpp")
endif ()
# Select language directory according to Kconfig
if(CONFIG_LANGUAGE_ZH_CN)
    set(LANG_DIR "zh-CN")
elseif(CONFIG_LANGUAGE_ZH_TW)
    set(LANG_DIR "zh-TW")
elseif(CONFIG_LANGUAGE_EN_US)
    set(LANG_DIR "en-US")
elseif(CONFIG_LANGUAGE_JA_JP)
    set(LANG_DIR "ja-JP")
elseif(CONFIG_LANGUAGE_KO_KR)
    set(LANG_DIR "ko-KR")
elseif(CONFIG_LANGUAGE_VI_VN)
    set(LANG_DIR "vi-VN")
elseif(CONFIG_LANGUAGE_TH_TH)
    set(LANG_DIR "th-TH")
elseif(CONFIG_LANGUAGE_DE_DE)
    set(LANG_DIR "de-DE")
elseif(CONFIG_LANGUAGE_FR_FR)
    set(LANG_DIR "fr-FR")
elseif(CONFIG_LANGUAGE_ES_ES)
    set(LANG_DIR "es-ES")
elseif(CONFIG_LANGUAGE_IT_IT)
    set(LANG_DIR "it-IT")
elseif(CONFIG_LANGUAGE_RU_RU)
    set(LANG_DIR "ru-RU")
elseif(CONFIG_LANGUAGE_AR_SA)
    set(LANG_DIR "ar-SA")
elseif(CONFIG_LANGUAGE_HI_IN)
    set(LANG_DIR "hi-IN")
elseif(CONFIG_LANGUAGE_PT_PT)
    set(LANG_DIR "pt-PT")
elseif(CONFIG_LANGUAGE_PL_PL)
    set(LANG_DIR "pl-PL")
elseif(CONFIG_LANGUAGE_CS_CZ)
    set(LANG_DIR "cs-CZ")
elseif(CONFIG_LANGUAGE_FI_FI)
    set(LANG_DIR "fi-FI")
elseif(CONFIG_LANGUAGE_TR_TR)
    set(LANG_DIR "tr-TR")
elseif(CONFIG_LANGUAGE_ID_ID)
    set(LANG_DIR "id-ID")
elseif(CONFIG_LANGUAGE_UK_UA)
    set(LANG_DIR "uk-UA")
elseif(CONFIG_LANGUAGE_RO_RO)
    set(LANG_DIR "ro-RO")
elseif(CONFIG_LANGUAGE_BG_BG)
    set(LANG_DIR "bg-BG")
elseif(CONFIG_LANGUAGE_CA_ES)
    set(LANG_DIR "ca-ES")
elseif(CONFIG_LANGUAGE_DA_DK)
    set(LANG_DIR "da-DK")
elseif(CONFIG_LANGUAGE_EL_GR)
    set(LANG_DIR "el-GR")
elseif(CONFIG_LANGUAGE_FA_IR)
    set(LANG_DIR "fa-IR")
elseif(CONFIG_LANGUAGE_FIL_PH)
    set(LANG_DIR "fil-PH")
elseif(CONFIG_LANGUAGE_HE_IL)
    set(LANG_DIR "he-IL")
elseif(CONFIG_LANGUAGE_HR_HR)
    set(LANG_DIR "hr-HR")
elseif(CONFIG_LANGUAGE_HU_HU)
    set(LANG_DIR "hu-HU")
elseif(CONFIG_LANGUAGE_MS_MY)
    set(LANG_DIR "ms-MY")
elseif(CONFIG_LANGUAGE_NB_NO)
    set(LANG_DIR "nb-NO")
elseif(CONFIG_LANGUAGE_NL_NL)
    set(LANG_DIR "nl-NL")
elseif(CONFIG_LANGUAGE_SK_SK)
    set(LANG_DIR "sk-SK")
elseif(CONFIG_LANGUAGE_SL_SI)
    set(LANG_DIR "sl-SI")
elseif(CONFIG_LANGUAGE_SV_SE)
    set(LANG_DIR "sv-SE")
elseif(CONFIG_LANGUAGE_SR_RS)
    set(LANG_DIR "sr-RS")
endif()

# Define generation path
set(LANG_JSON "${CMAKE_CURRENT_SOURCE_DIR}/assets/locales/${LANG_DIR}/language.json")
# 生成头落组件 build 目录（不再回写源树）——消灭「untracked 手补生成物」形态与跨 worktree 拷贝传播载体，
# 每个 worktree 首建必然自生成。
set(LANG_HEADER "${CMAKE_CURRENT_BINARY_DIR}/assets/lang_config.h")

# Collect current language audio files
file(GLOB LANG_SOUNDS ${CMAKE_CURRENT_SOURCE_DIR}/assets/locales/${LANG_DIR}/*.ogg)

# If not en-US, collect en-US audio files as fallback for missing files
if(NOT LANG_DIR STREQUAL "en-US")
    file(GLOB EN_US_SOUNDS ${CMAKE_CURRENT_SOURCE_DIR}/assets/locales/en-US/*.ogg)
    
    # Extract filenames (without path) from current language
    set(EXISTING_NAMES "")
    foreach(SOUND_FILE ${LANG_SOUNDS})
        get_filename_component(FILENAME ${SOUND_FILE} NAME)
        list(APPEND EXISTING_NAMES ${FILENAME})
    endforeach()
    
    # Only add en-US audio files that are missing in current language
    foreach(EN_SOUND ${EN_US_SOUNDS})
        get_filename_component(FILENAME ${EN_SOUND} NAME)
        if(NOT ${FILENAME} IN_LIST EXISTING_NAMES)
            list(APPEND LANG_SOUNDS ${EN_SOUND})
            message(STATUS "Using en-US fallback for missing audio: ${FILENAME}")
        endif()
    endforeach()
endif()

file(GLOB COMMON_SOUNDS ${CMAKE_CURRENT_SOURCE_DIR}/assets/common/*.ogg)

# If target chip is ESP32, exclude specific files to avoid build errors
if(CONFIG_IDF_TARGET_ESP32)
    list(REMOVE_ITEM SOURCES "audio/codecs/box_audio_codec.cc"
                             "audio/codecs/es8388_audio_codec.cc"
                             "audio/codecs/es8389_audio_codec.cc"
                             "led/gpio_led.cc"
                             "${CMAKE_CURRENT_SOURCE_DIR}/boards/common/esp32_camera.cc"
                             "display/lvgl_display/jpg/image_to_jpeg.cpp"
                             "display/lvgl_display/jpg/jpeg_to_image.c"
                             )
endif()

idf_component_register(SRCS ${SOURCES}
                    EMBED_FILES ${LANG_SOUNDS} ${COMMON_SOUNDS}
                    INCLUDE_DIRS ${INCLUDE_DIRS}
                    WHOLE_ARCHIVE
                    )

# Use target_compile_definitions to define BOARD_TYPE, BOARD_NAME
# If BOARD_NAME is empty, use BOARD_TYPE
if(NOT BOARD_NAME)
    set(BOARD_NAME ${BOARD_TYPE})
endif()
target_compile_definitions(${COMPONENT_LIB}
                    PRIVATE BOARD_TYPE=\"${BOARD_TYPE}\" BOARD_NAME=\"${BOARD_NAME}\"
                    PRIVATE BUILTIN_TEXT_FONT=${BUILTIN_TEXT_FONT} BUILTIN_ICON_FONT=${BUILTIN_ICON_FONT}
                    )

# Add generation rules
add_custom_command(
    OUTPUT ${LANG_HEADER}
    COMMAND python ${PROJECT_DIR}/scripts/gen_lang.py
            --language "${LANG_DIR}"
            --output "${LANG_HEADER}"
            --assets-dir "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    DEPENDS
        ${LANG_JSON}
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/locales/en-US/language.json
        ${PROJECT_DIR}/scripts/gen_lang.py
    COMMENT "Generating ${LANG_DIR} language config"
)

# 把生成头挂进组件目标（GENERATED + HEADER_FILE_ONLY），使本组件的对象文件编译排序在生成完成之后，
# 封死「ninja 编译与生成无序」编译红。单机制，不再挂冗余 ALL custom target。
set_source_files_properties(${LANG_HEADER} PROPERTIES GENERATED TRUE HEADER_FILE_ONLY TRUE)
target_sources(${COMPONENT_LIB} PRIVATE ${LANG_HEADER})

# Find ESP-SR component dynamically
find_component_by_pattern("espressif__esp-sr" ESP_SR_COMPONENT ESP_SR_COMPONENT_PATH)
if(ESP_SR_COMPONENT_PATH)
    set(ESP_SR_MODEL_PATH "${ESP_SR_COMPONENT_PATH}/model")
endif()

# Find xiaozhi-fonts component dynamically
find_component_by_pattern("xiaozhi-fonts" XIAOZHI_FONTS_COMPONENT XIAOZHI_FONTS_COMPONENT_PATH)
if(XIAOZHI_FONTS_COMPONENT_PATH)
    set(XIAOZHI_FONTS_PATH "${XIAOZHI_FONTS_COMPONENT_PATH}")
endif()

if(CONFIG_BOARD_TYPE_ESP_HI)
set(URL "https://github.com/espressif2022/image_player/raw/main/test_apps/test_8bit")
set(EMOJI_DIR "${CMAKE_BINARY_DIR}/emoji")
file(MAKE_DIRECTORY ${EMOJI_DIR})

# List all files to download
set(FILES_TO_DOWNLOAD "")
list(APPEND FILES_TO_DOWNLOAD "Anger_enter.aaf" "Anger_loop.aaf" "Anger_return.aaf")
list(APPEND FILES_TO_DOWNLOAD "happy_enter.aaf" "happy_loop.aaf" "happ_return.aaf")
list(APPEND FILES_TO_DOWNLOAD "sad_enter.aaf" "sad_loop.aaf" "sad_return.aaf")
list(APPEND FILES_TO_DOWNLOAD "scorn_enter.aaf" "scorn_loop.aaf" "scorn_return.aaf")
list(APPEND FILES_TO_DOWNLOAD "left_enter.aaf" "left_loop.aaf" "left_return.aaf")
list(APPEND FILES_TO_DOWNLOAD "right_enter.aaf" "right_loop.aaf" "right_return.aaf")
list(APPEND FILES_TO_DOWNLOAD "asking.aaf" "blink_once.aaf" "blink_quick.aaf")
list(APPEND FILES_TO_DOWNLOAD "connecting.aaf" "panic_enter.aaf" "panic_loop.aaf")
list(APPEND FILES_TO_DOWNLOAD "panic_return.aaf" "wake.aaf")

foreach(FILENAME IN LISTS FILES_TO_DOWNLOAD)
    set(REMOTE_FILE "${URL}/${FILENAME}")
    set(LOCAL_FILE "${EMOJI_DIR}/${FILENAME}")
    
    # Check if local file exists
    if(EXISTS ${LOCAL_FILE})
        message(STATUS "File ${FILENAME} already exists, skipping download")
    else()
        message(STATUS "Downloading ${FILENAME}")
        file(DOWNLOAD ${REMOTE_FILE} ${LOCAL_FILE}
             STATUS DOWNLOAD_STATUS)
        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        if(NOT STATUS_CODE EQUAL 0)
            message(FATAL_ERROR "Failed to download ${FILENAME} from ${URL}")
        endif()
    endif()
endforeach()

endif()


# Function to build default assets based on configuration
function(build_default_assets_bin)
    # Set output path for generated assets.bin
    set(GENERATED_ASSETS_BIN "${CMAKE_BINARY_DIR}/generated_assets.bin")
    
    # Prepare arguments for build script
    set(BUILD_ARGS
        "--sdkconfig" "${SDKCONFIG}"
        "--output" "${GENERATED_ASSETS_BIN}"
    )
    
    # Add builtin text font if defined
    if(BUILTIN_TEXT_FONT)
        list(APPEND BUILD_ARGS "--builtin_text_font" "${BUILTIN_TEXT_FONT}")
    endif()
    
    # Add default emoji collection if defined
    if(DEFAULT_EMOJI_COLLECTION)
        list(APPEND BUILD_ARGS "--emoji_collection" "${DEFAULT_EMOJI_COLLECTION}")
    endif()
    
    # Add default assets extra files if defined
    if(DEFAULT_ASSETS_EXTRA_FILES)
        list(APPEND BUILD_ARGS "--extra_files" "${DEFAULT_ASSETS_EXTRA_FILES}")
    endif()
    
    list(APPEND BUILD_ARGS "--esp_sr_model_path" "${ESP_SR_MODEL_PATH}")
    list(APPEND BUILD_ARGS "--xiaozhi_fonts_path" "${XIAOZHI_FONTS_PATH}")
    
    # Create custom command to build assets
    add_custom_command(
        OUTPUT ${GENERATED_ASSETS_BIN}
        COMMAND python ${PROJECT_DIR}/scripts/build_default_assets.py ${BUILD_ARGS}
        DEPENDS
            ${SDKCONFIG}
            ${PROJECT_DIR}/scripts/build_default_assets.py
        COMMENT "Building default assets.bin based on configuration"
        VERBATIM
    )
    
    # Create target for generated assets
    add_custom_target(generated_default_assets ALL
        DEPENDS ${GENERATED_ASSETS_BIN}
    )
    
    # Set the generated file path in parent scope
    set(GENERATED_ASSETS_LOCAL_FILE ${GENERATED_ASSETS_BIN} PARENT_SCOPE)
    
    message(STATUS "Default assets build configured: ${GENERATED_ASSETS_BIN}")
endfunction()


# Function to get local assets file path (handles both URL and local file)
function(get_assets_local_file assets_source assets_local_file_var)
    # Check if it's a URL (starts with http:// or https://)
    if(assets_source MATCHES "^https?://")
        # It's a URL, download it
        get_filename_component(ASSETS_FILENAME "${assets_source}" NAME)
        set(ASSETS_LOCAL_FILE "${CMAKE_BINARY_DIR}/${ASSETS_FILENAME}")
        set(ASSETS_TEMP_FILE "${CMAKE_BINARY_DIR}/${ASSETS_FILENAME}.tmp")
        
        # Check if local file exists
        if(EXISTS ${ASSETS_LOCAL_FILE})
            message(STATUS "Assets file ${ASSETS_FILENAME} already exists, skipping download")
        else()
            message(STATUS "Downloading ${ASSETS_FILENAME}")
            
            # Clean up any existing temp file
            if(EXISTS ${ASSETS_TEMP_FILE})
                file(REMOVE ${ASSETS_TEMP_FILE})
            endif()
            
            # Download to temporary file first
            file(DOWNLOAD ${assets_source} ${ASSETS_TEMP_FILE}
                 STATUS DOWNLOAD_STATUS)
            list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
            if(NOT STATUS_CODE EQUAL 0)
                # Clean up temp file on failure
                if(EXISTS ${ASSETS_TEMP_FILE})
                    file(REMOVE ${ASSETS_TEMP_FILE})
                endif()
                message(FATAL_ERROR "Failed to download ${ASSETS_FILENAME} from ${assets_source}")
            endif()
            
            # Move temp file to final location (atomic operation)
            file(RENAME ${ASSETS_TEMP_FILE} ${ASSETS_LOCAL_FILE})
            message(STATUS "Successfully downloaded ${ASSETS_FILENAME}")
        endif()
    else()
        # It's a local file path
        if(IS_ABSOLUTE "${assets_source}")
            set(ASSETS_LOCAL_FILE "${assets_source}")
        else()
            set(ASSETS_LOCAL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${assets_source}")
        endif()
        
        # Check if local file exists
        if(NOT EXISTS ${ASSETS_LOCAL_FILE})
            message(FATAL_ERROR "Assets file not found: ${ASSETS_LOCAL_FILE}")
        endif()
        
        message(STATUS "Using assets file: ${ASSETS_LOCAL_FILE}")
    endif()
    
    set(${assets_local_file_var} ${ASSETS_LOCAL_FILE} PARENT_SCOPE)
endfunction()


partition_table_get_partition_info(size "--partition-name assets" "size")
partition_table_get_partition_info(offset "--partition-name assets" "offset")
if ("${size}" AND "${offset}")
    # Flash assets based on configuration
    if(CONFIG_FLASH_DEFAULT_ASSETS)
        # Build default assets based on configuration
        build_default_assets_bin()
        esptool_py_flash_to_partition(flash "assets" "${GENERATED_ASSETS_LOCAL_FILE}")
        message(STATUS "Generated default assets flash configured: ${GENERATED_ASSETS_LOCAL_FILE} -> assets partition")
    elseif(CONFIG_FLASH_CUSTOM_ASSETS)
        # Flash custom assets
        get_assets_local_file("${CONFIG_CUSTOM_ASSETS_FILE}" ASSETS_LOCAL_FILE)
        esptool_py_flash_to_partition(flash "assets" "${ASSETS_LOCAL_FILE}")
        message(STATUS "Custom assets flash configured: ${ASSETS_LOCAL_FILE} -> assets partition")
    elseif(CONFIG_FLASH_NONE_ASSETS)
        message(STATUS "Assets flashing disabled (FLASH_NONE_ASSETS)")
    endif()
else()
    message(STATUS "Assets partition not found, using v1 partition table")
endif()

# 包 9b (260829-hardware-validation) ART-E2 收口：KAP1 美术资产落地为独立 SPIFFS 分区
# `art`（分区表 firmware/partitions/v2/16m.csv 新增 art,data,spiffs,0x780000,0x80000）。
# 不写 assets——assets 归 CONFIG_FLASH_DEFAULT_ASSETS 的 mmap 镜像（字体/声音/P1 现网
# 资源，二进制格式不同，不可双写）。此规则始终生效（写独立 art 分区，无并存冲突），
# 去掉旧「NOT CONFIG_FLASH_DEFAULT_ASSETS」互斥休眠门。spiffs_create_partition_image
# 依 IDF 签名 <partition> <base_dir> [FLASH_IN_PROJECT] 生成 build/art.bin（spiffsgen），
# FLASH_IN_PROJECT 令其接进 flash 目标（esptool 烧至 0x780000）。单一源目录
# firmware/main/assets/spiffs_root/（47 枚 KAP1 闭集，见 ui_art_integration ART-A2
# artt_expected[]）不重造。设备侧 boards/waveshare-s3-rlcd-4.2 的 InitializeArt 以
# esp_vfs_spiffs_register(base_path="/art", partition_label="art") 注册 VFS 供 loader 取用。
if("${size}" AND "${offset}")
    spiffs_create_partition_image(art "${CMAKE_CURRENT_SOURCE_DIR}/assets/spiffs_root" FLASH_IN_PROJECT)
endif()
