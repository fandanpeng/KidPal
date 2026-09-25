#include "lcd_display.h"
#include "gif/lvgl_gif.h"
#include "settings.h"
#include "lvgl_theme.h"
#include "assets/lang_config.h"
#include "kidpal_voice_states.h"
#include "voice_round.h"
#include "art_assets.h"
#include "kidpal_render_node.h"
#include "reminder_page.h" /* 提醒弹窗消费面 (K4): 时间行单源 kidpal_reminder_hhmm + 载荷形状 */
#include "key_gesture_parser.h"
#include "keybar.h"

#include <vector>
#include <algorithm>
#include <cstdio>
#include <font_awesome.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_lvgl_port.h>
#include <esp_psram.h>
#include <cstring>

#include "board.h"

#define TAG "LcdDisplay"

LV_FONT_DECLARE(BUILTIN_TEXT_FONT);
LV_FONT_DECLARE(font_puhui_14_1); /* 全量 14px 普惠体: 键帽条左右槽专用 (font_puhui_basic_30_4 稀疏子集缺课/程/翻/页等键帽字形->空白, KB-A5 复轮根因; 全量 30px 超分区/gate, 14px 与 page body 同字号可读) */
LV_FONT_DECLARE(BUILTIN_ICON_FONT);
LV_FONT_DECLARE(font_awesome_30_4);

void LcdDisplay::InitializeLcdThemes() {
    auto text_font = std::make_shared<LvglBuiltInFont>(&BUILTIN_TEXT_FONT);
    auto icon_font = std::make_shared<LvglBuiltInFont>(&BUILTIN_ICON_FONT);
    auto large_icon_font = std::make_shared<LvglBuiltInFont>(&font_awesome_30_4);

    // light theme
    auto light_theme = new LvglTheme("light");
    light_theme->set_background_color(lv_color_hex(0xFFFFFF));
    light_theme->set_text_color(lv_color_hex(0x000000));
    light_theme->set_chat_background_color(lv_color_hex(0xE0E0E0));
    light_theme->set_user_bubble_color(lv_color_hex(0x00FF00));
    light_theme->set_assistant_bubble_color(lv_color_hex(0xDDDDDD));
    light_theme->set_system_bubble_color(lv_color_hex(0xFFFFFF));
    light_theme->set_system_text_color(lv_color_hex(0x000000));
    light_theme->set_border_color(lv_color_hex(0x000000));
    light_theme->set_low_battery_color(lv_color_hex(0x000000));
    light_theme->set_text_font(text_font);
    light_theme->set_icon_font(icon_font);
    light_theme->set_large_icon_font(large_icon_font);

    // dark theme
    auto dark_theme = new LvglTheme("dark");
    dark_theme->set_background_color(lv_color_hex(0x000000));
    dark_theme->set_text_color(lv_color_hex(0xFFFFFF));
    dark_theme->set_chat_background_color(lv_color_hex(0x1F1F1F));
    dark_theme->set_user_bubble_color(lv_color_hex(0x00FF00));
    dark_theme->set_assistant_bubble_color(lv_color_hex(0x222222));
    dark_theme->set_system_bubble_color(lv_color_hex(0x000000));
    dark_theme->set_system_text_color(lv_color_hex(0xFFFFFF));
    dark_theme->set_border_color(lv_color_hex(0xFFFFFF));
    dark_theme->set_low_battery_color(lv_color_hex(0xFF0000));
    dark_theme->set_text_font(text_font);
    dark_theme->set_icon_font(icon_font);
    dark_theme->set_large_icon_font(large_icon_font);

    auto& theme_manager = LvglThemeManager::GetInstance();
    theme_manager.RegisterTheme("light", light_theme);
    theme_manager.RegisterTheme("dark", dark_theme);
}

LcdDisplay::LcdDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel, int width, int height)
    : panel_io_(panel_io), panel_(panel) {
    width_ = width;
    height_ = height;

    // Initialize LCD themes
    InitializeLcdThemes();

    // Load theme from settings
    Settings settings("display", false);
    std::string theme_name = settings.GetString("theme", "light");
    current_theme_ = LvglThemeManager::GetInstance().GetTheme(theme_name);

    // Create a timer to hide the preview image
    esp_timer_create_args_t preview_timer_args = {
        .callback = [](void* arg) {
            LcdDisplay* display = static_cast<LcdDisplay*>(arg);
            display->SetPreviewImage(nullptr);
        },
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "preview_timer",
        .skip_unhandled_events = false,
    };
    esp_timer_create(&preview_timer_args, &preview_timer_);
}

SpiLcdDisplay::SpiLcdDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
                           int width, int height, int offset_x, int offset_y, bool mirror_x, bool mirror_y, bool swap_xy)
    : LcdDisplay(panel_io, panel, width, height) {

    // draw white
    std::vector<uint16_t> buffer(width_, 0xFFFF);
    for (int y = 0; y < height_; y++) {
        esp_lcd_panel_draw_bitmap(panel_, 0, y, width_, y + 1, buffer.data());
    }

    // Set the display to on
    ESP_LOGI(TAG, "Turning display on");
    {
        esp_err_t __err = esp_lcd_panel_disp_on_off(panel_, true);
        if (__err == ESP_ERR_NOT_SUPPORTED) {
            ESP_LOGW(TAG, "Panel does not support disp_on_off; assuming ON");
        } else {
            ESP_ERROR_CHECK(__err);
        }
    }

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

#if CONFIG_SPIRAM
    // lv image cache, currently only PNG is supported
    size_t psram_size_mb = esp_psram_get_size() / 1024 / 1024;
    if (psram_size_mb >= 8) {
        lv_image_cache_resize(2 * 1024 * 1024, true);
        ESP_LOGI(TAG, "Use 2MB of PSRAM for image cache");
    } else if (psram_size_mb >= 2) {
        lv_image_cache_resize(512 * 1024, true);
        ESP_LOGI(TAG, "Use 512KB of PSRAM for image cache");
    }
#endif

    ESP_LOGI(TAG, "Initialize LVGL port");
    lvgl_port_cfg_t port_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    port_cfg.task_priority = 1;
#if CONFIG_SOC_CPU_CORES_NUM > 1
    port_cfg.task_affinity = 1;
#endif
    lvgl_port_init(&port_cfg);

    ESP_LOGI(TAG, "Adding LCD display");
    const lvgl_port_display_cfg_t display_cfg = {
        .io_handle = panel_io_,
        .panel_handle = panel_,
        .control_handle = nullptr,
        .buffer_size = static_cast<uint32_t>(width_ * 20),
        .double_buffer = false,
        .trans_size = 0,
        .hres = static_cast<uint32_t>(width_),
        .vres = static_cast<uint32_t>(height_),
        .monochrome = false,
        .rotation = {
            .swap_xy = swap_xy,
            .mirror_x = mirror_x,
            .mirror_y = mirror_y,
        },
        .color_format = LV_COLOR_FORMAT_RGB565,
        .flags = {
            .buff_dma = 1,
            .buff_spiram = 0,
            .sw_rotate = 0,
            .swap_bytes = 1,
            .full_refresh = 0,
            .direct_mode = 0,
        },
    };

    display_ = lvgl_port_add_disp(&display_cfg);
    if (display_ == nullptr) {
        ESP_LOGE(TAG, "Failed to add display");
        return;
    }

    if (offset_x != 0 || offset_y != 0) {
        lv_display_set_offset(display_, offset_x, offset_y);
    }

    SetupUI();
}


// RGB LCD implementation
RgbLcdDisplay::RgbLcdDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
                           int width, int height, int offset_x, int offset_y,
                           bool mirror_x, bool mirror_y, bool swap_xy)
    : LcdDisplay(panel_io, panel, width, height) {

    // draw white
    std::vector<uint16_t> buffer(width_, 0xFFFF);
    for (int y = 0; y < height_; y++) {
        esp_lcd_panel_draw_bitmap(panel_, 0, y, width_, y + 1, buffer.data());
    }

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

    ESP_LOGI(TAG, "Initialize LVGL port");
    lvgl_port_cfg_t port_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    port_cfg.task_priority = 1;
    port_cfg.timer_period_ms = 50;
    lvgl_port_init(&port_cfg);

    ESP_LOGI(TAG, "Adding LCD display");
    const lvgl_port_display_cfg_t display_cfg = {
        .io_handle = panel_io_,
        .panel_handle = panel_,
        .buffer_size = static_cast<uint32_t>(width_ * 20),
        .double_buffer = true,
        .hres = static_cast<uint32_t>(width_),
        .vres = static_cast<uint32_t>(height_),
        .rotation = {
            .swap_xy = swap_xy,
            .mirror_x = mirror_x,
            .mirror_y = mirror_y,
        },
        .flags = {
            .buff_dma = 1,
            .swap_bytes = 0,
            .full_refresh = 1,
            .direct_mode = 1,
        },
    };

    const lvgl_port_display_rgb_cfg_t rgb_cfg = {
        .flags = {
            .bb_mode = true,
            .avoid_tearing = true,
        }
    };
    
    display_ = lvgl_port_add_disp_rgb(&display_cfg, &rgb_cfg);
    if (display_ == nullptr) {
        ESP_LOGE(TAG, "Failed to add RGB display");
        return;
    }
    
    if (offset_x != 0 || offset_y != 0) {
        lv_display_set_offset(display_, offset_x, offset_y);
    }

    SetupUI();
}

MipiLcdDisplay::MipiLcdDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
                            int width, int height,  int offset_x, int offset_y,
                            bool mirror_x, bool mirror_y, bool swap_xy)
    : LcdDisplay(panel_io, panel, width, height) {

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

    ESP_LOGI(TAG, "Initialize LVGL port");
    lvgl_port_cfg_t port_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&port_cfg);

    ESP_LOGI(TAG, "Adding LCD display");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = panel_io,
        .panel_handle = panel,
        .control_handle = nullptr,
        .buffer_size = static_cast<uint32_t>(width_ * 50),
        .double_buffer = false,
        .hres = static_cast<uint32_t>(width_),
        .vres = static_cast<uint32_t>(height_),
        .monochrome = false,
        /* Rotation values must be same as used in esp_lcd for initial settings of the screen */
        .rotation = {
            .swap_xy = swap_xy,
            .mirror_x = mirror_x,
            .mirror_y = mirror_y,
        },
        .flags = {
            .buff_dma = true,
            .buff_spiram =false,
            .sw_rotate = true,
        },
    };

    const lvgl_port_display_dsi_cfg_t dpi_cfg = {
        .flags = {
            .avoid_tearing = false,
        }
    };
    display_ = lvgl_port_add_disp_dsi(&disp_cfg, &dpi_cfg);
    if (display_ == nullptr) {
        ESP_LOGE(TAG, "Failed to add display");
        return;
    }

    if (offset_x != 0 || offset_y != 0) {
        lv_display_set_offset(display_, offset_x, offset_y);
    }

    SetupUI();
}

LcdDisplay::~LcdDisplay() {
    SetPreviewImage(nullptr);
    
    // Clean up GIF controller
    if (gif_controller_) {
        gif_controller_->Stop();
        gif_controller_.reset();
    }
    
    if (preview_timer_ != nullptr) {
        esp_timer_stop(preview_timer_);
        esp_timer_delete(preview_timer_);
    }

    if (preview_image_ != nullptr) {
        lv_obj_del(preview_image_);
    }
    if (chat_message_label_ != nullptr) {
        lv_obj_del(chat_message_label_);
    }
    if (emoji_label_ != nullptr) {
        lv_obj_del(emoji_label_);
    }
    if (emoji_image_ != nullptr) {
        lv_obj_del(emoji_image_);
    }
    if (emoji_box_ != nullptr) {
        lv_obj_del(emoji_box_);
    }
    if (content_ != nullptr) {
        lv_obj_del(content_);
    }
    if (bottom_bar_ != nullptr) {
        lv_obj_del(bottom_bar_);
    }
    if (status_bar_ != nullptr) {
        lv_obj_del(status_bar_);
    }
    if (top_bar_ != nullptr) {
        lv_obj_del(top_bar_);
    }
    if (side_bar_ != nullptr) {
        lv_obj_del(side_bar_);
    }
    if (container_ != nullptr) {
        lv_obj_del(container_);
    }
    if (display_ != nullptr) {
        lv_display_delete(display_);
    }

    if (panel_ != nullptr) {
        esp_lcd_panel_del(panel_);
    }
    if (panel_io_ != nullptr) {
        esp_lcd_panel_io_del(panel_io_);
    }
}

bool LcdDisplay::Lock(int timeout_ms) {
    return lvgl_port_lock(timeout_ms);
}

void LcdDisplay::Unlock() {
    lvgl_port_unlock();
}

/* -------------------------------------------------------------------------- *
 * 五态演出内容层 (包 260911-render-pages RP-A9)。文件级 static 状态 + 前向声明;
 * 定义在文件尾活分支之后 (active side) 以规避 #if 双分支扫描陷阱 —— 本段 (位于
 * #if 之前) 不含 kidpal_voice_state_look 引用。V3 §06 底部键帽条任何状态常驻:
 * 文案随五态联动 (说完啦/不等了/打断/知道了/叫醒它, 五态键帽行逐字)。
 * ------------------------------------------------------------------------ */
static const char* const g_kidpal_pose_asset[5] = {
    "pose_listen_96.bin",  /* KVST_POSE_LISTEN  */
    "pose_think_96.bin",   /* KVST_POSE_THINK   */
    "pose_talk_96.bin",    /* KVST_POSE_TALK    */
    "pose_jump_96.bin",    /* KVST_POSE_JUMP    (既有守卫, V3 提醒态) */
    "pose_sleep_96.bin",   /* KVST_POSE_SLEEP   */
};
/* V3 气泡尾巴 (U 闭集修订唯一新增 KAP1: frm-sm 手绘圆角框 + 左下斜切小尾巴). */
static const char g_kidpal_bubble_tail_asset[] = "bubble_tail_24.bin";
/* r3-C1 麦克风 (WF-B-01 聆听态标识件). device-round VQ-A7 r5: 依流程红线② (1bpp 资产
 * 禁运行时 lv_image_set_scale 缩放) 以 tools/asset-pipeline/gen_voice_chip16.mjs 重出
 * 16px 锐利 1bpp 复件 ic_mic_16.bin, 免 r4 运行时把 ic_mic_48.bin 48px 源 lv_image_set_scale
 * 缩到 ~16px 稀释 1bpp 笔画; 本渲染消费 16px 原尺寸。旧 ic_mic_48.bin 留 spiffs_root 闭集
 * 不删 (防归档 v3 齿参照断; 文件名含 mic, 与闭集同字面锚定)。 */
static const char g_kidpal_mic_asset[] = "ic_mic_16.bin";
/* r3-C2 问号 (WF-B-02 思考态标识件). device-round VQ-A7 r5: 同红线② 以
 * gen_voice_chip16.mjs 重出 16px 锐利 1bpp 复件 question_16.bin (免 r4 运行时把
 * question_96.bin 96px 源 lv_image_set_scale 缩到 ~16px 稀释), 本渲染消费 16px 原尺寸。
 * 旧 question_96.bin 留 spiffs_root 闭集不删 (防归档齿参照断)。 */
static const char g_kidpal_question_asset[] = "question_16.bin";

static lv_obj_t*   g_kidpal_keybar_left_label  = NULL; /* 三槽·左 KEY 提示槽 (通栏贴底常驻, bottom_bar_ 子件) */
static lv_obj_t*   g_kidpal_keybar_dots_box     = NULL; /* 三槽·中页点槽容器 (页点子件建其上, bottom_bar_ 子件) */
static lv_obj_t*   g_kidpal_keybar_right_label  = NULL; /* 三槽·右 BOOT 提示槽 (通栏贴底常驻, bottom_bar_ 子件) */
static lv_obj_t*   g_kidpal_keybar_dot[PAGE_COUNT] = {0}; /* 中槽页点子件 (数量 = 现行页循环, 当前页实心放大) */
static int         g_kidpal_keybar_scene        = 0;  /* 覆盖层当前语音场景 (随态右槽 结束/关闭 与左槽单源用) */
static lv_obj_t*   g_kidpal_chat_label   = NULL; /* 同步文字框 / 气泡挂点 (SetupUI 纪元) */
static lv_obj_t*   g_kidpal_pose_parent  = NULL; /* 姿态图父容器 = SetupUI 活分支表情区 (SetupUI 纪元) */
static lv_obj_t*   g_kidpal_pose_img     = NULL; /* 姿态图对象 (场景切换重建, 旧件必删) */
static lv_obj_t*   g_kidpal_emoji_label  = NULL; /* 笑脸 label (SetupUI 纪元, 中部显隐纪律用) */
static lv_obj_t*   g_kidpal_emoji_image  = NULL; /* 表情图 (SetupUI 纪元, 中部显隐纪律用) */
static int         g_kidpal_mid_mode     = 0; /* chrome 中部显隐: 0=未接管 1=五态演出 2=idle 清空 */
static int         g_kidpal_voice_front  = 0; /* VOICE 层是否前台 (render_node 桥写入; 姿态显隐第二维) */
static LcdDisplay* s_kidpal_lcd_self     = NULL; /* self 注册 (RP-A5 r4): 前台维度桥不得经
    Board::GetInstance() —— boot 期 render_node_init 跑在 CustomBoard 构造期内, 函数内
    static 单例重入会 __cxa_guard 永久阻塞 (r3 真机砖机根因)。SetupUI 构造期登记 this。 */
static lv_image_dsc_t g_kidpal_pose_dsc;
static uint32_t*   g_kidpal_pose_px = NULL;
static int         g_kidpal_pose_st   = 0; /* 0=未试 1=可用 2=不可用 */
static int         g_kidpal_pose_scene = -1;

/* 语音轮演出容器 (包 260915-m1-k3-voice-round-ui, [3] 聆听回显区 + 问答框):
 * 左小伴 (各态 pose 挂 pose 挂点) · 右内容区同骨架 —— 两容器均可滚动
 * (LV_OBJ_FLAG_SCROLLABLE, VQ-A2/A3)。回显区 = 逐句累积, 新句 push 后整列重建;
 * 问答框 = 答案全文长文滚动。设备侧只消费纯逻辑单源返回值, 零重抄。
 * device-round VQ-A7 r1 修复面③: 两容器脱离裸挂 screen 顶层 (旧形态挂默认首屏,
 * 该屏归一屏后不再加载 = 永不可见), 改挂语音轮整页版式容器 g_kidpal_voice_page
 * (常驻 lv_layer_top, 显隐由 chrome 中部显隐纪律单点驱动)。 */
static lv_obj_t*   g_kidpal_voice_page        = NULL; /* 语音轮整页版式容器 (WF-B 左小伴·右内容区,
                                                      活跃期整页盖住首页内容, 常驻 layer_top) */
static lv_obj_t*   g_kidpal_pose_box          = NULL; /* 左小伴 pose 挂点 (WF-B 左列, 96×96) */
static lv_obj_t*   g_kidpal_voice_echo_box    = NULL; /* 聆听话语回显区容器 (「我在听！」
                                                       标识下方, 可滚动) */
static lv_obj_t*   g_kidpal_voice_echo_label  = NULL; /* 回显区内容缓存对象 (整列文本) */
static lv_obj_t*   g_kidpal_voice_answer_box  = NULL; /* 播报问答框容器 (WF-B-03 右内容区,
                                                       可滚动, 答案全文上屏滚动看完) */
static lv_obj_t*   g_kidpal_voice_answer_label = NULL; /* 问答框内容缓存对象 (全文) */
static lv_obj_t*   g_kidpal_voice_header_label = NULL; /* 右内容区标头 (「思考中」/「播报」/
                                                         「我在听！」, look.bubble 写点由旧
                                                         chat 挂点统一迁此; WF-B 标头位, 挂在
                                                         echo_box 之前 = 回显行在标头下方) */
/* r3-C 三态标识件 (WF-B-01/02/03): 聆听态麦克风 (ic_mic_48.bin) · 思考态飘问号 x3
 * (question_96.bin 静态摆, 动画浮动后置) · 播报态声波条 (LVGL 原生矩形 + lv_anim 跳动,
 * 免管线、动画天然, 与资产闭集治理不冲突 —— 取舍见交付报告)。对象惰性建一次于
 * apply_look, 场景切换只切 HIDDEN, 不重建不泄漏 (姿态/标识件重建纪律同 pose)。气泡
 * 尾巴 (bubble_tail_24.bin) 为三态标头的气泡背景件, 随 look.bubble 显隐。 */
static lv_obj_t*   g_kidpal_mic_img        = NULL;
static lv_image_dsc_t g_kidpal_mic_dsc;
static uint32_t*   g_kidpal_mic_px         = NULL;
static lv_obj_t*   g_kidpal_q_img[1]      = {0};   /* r4 撤 x3 静态摆, 单枚小问号 */
static lv_image_dsc_t g_kidpal_q_dsc;
static uint32_t*   g_kidpal_q_px           = NULL;
static lv_obj_t*   g_kidpal_bubble_img     = NULL;
static lv_image_dsc_t g_kidpal_bubble_dsc;
static uint32_t*   g_kidpal_bubble_px      = NULL;
static lv_obj_t*   g_kidpal_wave_box       = NULL;
static lv_obj_t*   g_kidpal_wave_bar[6]    = {0};
static int         g_kidpal_emblem_built   = 0;  /* 0=未建 1=已建 */
static int32_t     g_kidpal_wave_phase     = 0;
static int         g_kidpal_wave_an_running = 0;

static void kidpal_voice_emblem_apply_locked(int scene);

static void kidpal_chrome_mid_sync_locked(void);
static void kidpal_keybar_sync_locked(void); /* 定义在活分支尾 (SetupUI 之后), 前向声明供 SetupUI 首刷调用 */

#if CONFIG_USE_WECHAT_MESSAGE_STYLE
void LcdDisplay::SetupUI() {
    DisplayLockGuard lock(this);

    auto lvgl_theme = static_cast<LvglTheme*>(current_theme_);
    auto text_font = lvgl_theme->text_font()->font();
    auto icon_font = lvgl_theme->icon_font()->font();
    auto large_icon_font = lvgl_theme->large_icon_font()->font();

    auto screen = lv_screen_active();
    lv_obj_set_style_text_font(screen, text_font, 0);
    lv_obj_set_style_text_color(screen, lvgl_theme->text_color(), 0);
    lv_obj_set_style_bg_color(screen, lvgl_theme->background_color(), 0);

    /* Container */
    container_ = lv_obj_create(screen);
    lv_obj_set_size(container_, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_radius(container_, 0, 0);
    lv_obj_set_flex_flow(container_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(container_, 0, 0);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_set_style_pad_row(container_, 0, 0);
    lv_obj_set_style_bg_color(container_, lvgl_theme->background_color(), 0);
    lv_obj_set_style_border_color(container_, lvgl_theme->border_color(), 0);

    /* Layer 1: Top bar - for status icons */
    top_bar_ = lv_obj_create(container_);
    lv_obj_set_size(top_bar_, LV_HOR_RES, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(top_bar_, 0, 0);
    lv_obj_set_style_bg_opa(top_bar_, LV_OPA_50, 0);  // 50% opacity background
    lv_obj_set_style_bg_color(top_bar_, lvgl_theme->background_color(), 0);
    lv_obj_set_style_border_width(top_bar_, 0, 0);
    lv_obj_set_style_pad_all(top_bar_, 0, 0);
    lv_obj_set_style_pad_top(top_bar_, lvgl_theme->spacing(2), 0);
    lv_obj_set_style_pad_bottom(top_bar_, lvgl_theme->spacing(2), 0);
    lv_obj_set_style_pad_left(top_bar_, lvgl_theme->spacing(4), 0);
    lv_obj_set_style_pad_right(top_bar_, lvgl_theme->spacing(4), 0);
    lv_obj_set_flex_flow(top_bar_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(top_bar_, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(top_bar_, LV_SCROLLBAR_MODE_OFF);

    // Left icon
    network_label_ = lv_label_create(top_bar_);
    lv_label_set_text(network_label_, "");
    lv_obj_set_style_text_font(network_label_, icon_font, 0);
    lv_obj_set_style_text_color(network_label_, lvgl_theme->text_color(), 0);

    // Right icons container
    lv_obj_t* right_icons = lv_obj_create(top_bar_);
    lv_obj_set_size(right_icons, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(right_icons, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(right_icons, 0, 0);
    lv_obj_set_style_pad_all(right_icons, 0, 0);
    lv_obj_set_flex_flow(right_icons, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(right_icons, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    mute_label_ = lv_label_create(right_icons);
    lv_label_set_text(mute_label_, "");
    lv_obj_set_style_text_font(mute_label_, icon_font, 0);
    lv_obj_set_style_text_color(mute_label_, lvgl_theme->text_color(), 0);

    /* Layer 2: Status bar - for center text labels */
    status_bar_ = lv_obj_create(screen);
    lv_obj_set_size(status_bar_, LV_HOR_RES, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(status_bar_, 0, 0);
    lv_obj_set_style_bg_opa(status_bar_, LV_OPA_TRANSP, 0);  // Transparent background
    lv_obj_set_style_border_width(status_bar_, 0, 0);
    lv_obj_set_style_pad_all(status_bar_, 0, 0);
    lv_obj_set_style_pad_top(status_bar_, lvgl_theme->spacing(2), 0);
    lv_obj_set_style_pad_bottom(status_bar_, lvgl_theme->spacing(2), 0);
    lv_obj_set_scrollbar_mode(status_bar_, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_layout(status_bar_, LV_LAYOUT_NONE, 0);  // Use absolute positioning
    lv_obj_align(status_bar_, LV_ALIGN_TOP_MID, 0, 0);  // Overlap with top_bar_

    notification_label_ = lv_label_create(status_bar_);
    lv_obj_set_width(notification_label_, LV_HOR_RES * 0.8);
    lv_obj_set_style_text_align(notification_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(notification_label_, lvgl_theme->text_color(), 0);
    lv_label_set_text(notification_label_, "");
    lv_obj_align(notification_label_, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(notification_label_, LV_OBJ_FLAG_HIDDEN);

    status_label_ = lv_label_create(status_bar_);
    lv_obj_set_width(status_label_, LV_HOR_RES * 0.8);
    lv_label_set_long_mode(status_label_, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(status_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(status_label_, lvgl_theme->text_color(), 0);
    lv_label_set_text(status_label_, Lang::Strings::INITIALIZING);
    lv_obj_align(status_label_, LV_ALIGN_CENTER, 0, 0);
    
    /* Content - Chat area */
    content_ = lv_obj_create(container_);
    lv_obj_set_style_radius(content_, 0, 0);
    lv_obj_set_width(content_, LV_HOR_RES);
    lv_obj_set_flex_grow(content_, 1);
    lv_obj_set_style_pad_all(content_, lvgl_theme->spacing(4), 0);
    lv_obj_set_style_border_width(content_, 0, 0);
    lv_obj_set_style_bg_color(content_, lvgl_theme->chat_background_color(), 0); // Background for chat area

    // Enable scrolling for chat content
    lv_obj_set_scrollbar_mode(content_, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(content_, LV_DIR_VER);
    
    // Create a flex container for chat messages
    lv_obj_set_flex_flow(content_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(content_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(content_, lvgl_theme->spacing(4), 0); // Space between messages

    // We'll create chat messages dynamically in SetChatMessage
    chat_message_label_ = nullptr;

    low_battery_popup_ = lv_obj_create(screen);
    lv_obj_set_scrollbar_mode(low_battery_popup_, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(low_battery_popup_, LV_HOR_RES * 0.9, text_font->line_height * 2);
    lv_obj_align(low_battery_popup_, LV_ALIGN_BOTTOM_MID, 0, -lvgl_theme->spacing(4));
    lv_obj_set_style_bg_color(low_battery_popup_, lvgl_theme->low_battery_color(), 0);
    lv_obj_set_style_radius(low_battery_popup_, lvgl_theme->spacing(4), 0);
    low_battery_label_ = lv_label_create(low_battery_popup_);
    lv_label_set_text(low_battery_label_, Lang::Strings::BATTERY_NEED_CHARGE);
    lv_obj_set_style_text_color(low_battery_label_, lv_color_white(), 0);
    lv_obj_center(low_battery_label_);
    lv_obj_add_flag(low_battery_popup_, LV_OBJ_FLAG_HIDDEN);

    emoji_image_ = lv_img_create(screen);
    lv_obj_align(emoji_image_, LV_ALIGN_TOP_MID, 0, text_font->line_height + lvgl_theme->spacing(8));

    // Display AI logo while booting
    emoji_label_ = lv_label_create(screen);
    lv_obj_center(emoji_label_);
    lv_obj_set_style_text_font(emoji_label_, large_icon_font, 0);
    lv_obj_set_style_text_color(emoji_label_, lvgl_theme->text_color(), 0);
    lv_label_set_text(emoji_label_, FONT_AWESOME_MICROCHIP_AI);
}
#if CONFIG_IDF_TARGET_ESP32P4
#define  MAX_MESSAGES 40
#else
#define  MAX_MESSAGES 20
#endif
void LcdDisplay::SetChatMessage(const char* role, const char* content) {
    DisplayLockGuard lock(this);
    if (content_ == nullptr) {
        return;
    }
    
    // Check if message count exceeds limit
    uint32_t child_count = lv_obj_get_child_cnt(content_);
    if (child_count >= MAX_MESSAGES) {
        // Delete the oldest message (first child object)
        lv_obj_t* first_child = lv_obj_get_child(content_, 0);
        lv_obj_t* last_child = lv_obj_get_child(content_, child_count - 1);
        if (first_child != nullptr) {
            lv_obj_del(first_child);
        }
        // Scroll to the last message immediately
        if (last_child != nullptr) {
            lv_obj_scroll_to_view_recursive(last_child, LV_ANIM_OFF);
        }
    }
    
    // Collapse system messages (if it's a system message, check if the last message is also a system message)
    if (strcmp(role, "system") == 0) {
        if (child_count > 0) {
            // Get the last message container
            lv_obj_t* last_container = lv_obj_get_child(content_, child_count - 1);
            if (last_container != nullptr && lv_obj_get_child_cnt(last_container) > 0) {
                // Get the bubble inside the container
                lv_obj_t* last_bubble = lv_obj_get_child(last_container, 0);
                if (last_bubble != nullptr) {
                    // Check if bubble type is system message
                    void* bubble_type_ptr = lv_obj_get_user_data(last_bubble);
                    if (bubble_type_ptr != nullptr && strcmp((const char*)bubble_type_ptr, "system") == 0) {
                        // If the last message is also a system message, delete it
                        lv_obj_del(last_container);
                    }
                }
            }
        }
    } else {
        // Hide the centered AI logo
        lv_obj_add_flag(emoji_label_, LV_OBJ_FLAG_HIDDEN);
    }

    // Avoid empty message boxes
    if(strlen(content) == 0) {
        return;
    }

    auto lvgl_theme = static_cast<LvglTheme*>(current_theme_);
    auto text_font = lvgl_theme->text_font()->font();

    // Create a message bubble
    lv_obj_t* msg_bubble = lv_obj_create(content_);
    lv_obj_set_style_radius(msg_bubble, 8, 0);
    lv_obj_set_scrollbar_mode(msg_bubble, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(msg_bubble, 0, 0);
    lv_obj_set_style_pad_all(msg_bubble, lvgl_theme->spacing(4), 0);

    // Create the message text
    lv_obj_t* msg_text = lv_label_create(msg_bubble);
    lv_label_set_text(msg_text, content);
    
    // Calculate actual text width
    lv_coord_t text_width = lv_txt_get_width(content, strlen(content), text_font, 0);

    // Calculate bubble width
    lv_coord_t max_width = LV_HOR_RES * 85 / 100 - 16;  // 85% of screen width
    lv_coord_t min_width = 20;  
    lv_coord_t bubble_width;
    
    // Ensure text width is not less than minimum width
    if (text_width < min_width) {
        text_width = min_width;
    }

    // If text width is less than max width, use text width
    if (text_width < max_width) {
        bubble_width = text_width; 
    } else {
        bubble_width = max_width;
    }
    
    // Set message text width
    lv_obj_set_width(msg_text, bubble_width);  // Subtract padding
    lv_label_set_long_mode(msg_text, LV_LABEL_LONG_WRAP);

    // Set bubble width
    lv_obj_set_width(msg_bubble, bubble_width);
    lv_obj_set_height(msg_bubble, LV_SIZE_CONTENT);

    // Set alignment and style based on message role
    if (strcmp(role, "user") == 0) {
        // User messages are right-aligned with green background
        lv_obj_set_style_bg_color(msg_bubble, lvgl_theme->user_bubble_color(), 0);
        lv_obj_set_style_bg_opa(msg_bubble, LV_OPA_70, 0);
        // Set text color for contrast
        lv_obj_set_style_text_color(msg_text, lvgl_theme->text_color(), 0);
        
        // Set custom attribute to mark bubble type
        lv_obj_set_user_data(msg_bubble, (void*)"user");
        
        // Set appropriate width for content
        lv_obj_set_width(msg_bubble, LV_SIZE_CONTENT);
        lv_obj_set_height(msg_bubble, LV_SIZE_CONTENT);
        
        // Don't grow
        lv_obj_set_style_flex_grow(msg_bubble, 0, 0);
    } else if (strcmp(role, "assistant") == 0) {
        // Assistant messages are left-aligned with white background
        lv_obj_set_style_bg_color(msg_bubble, lvgl_theme->assistant_bubble_color(), 0);
        lv_obj_set_style_bg_opa(msg_bubble, LV_OPA_70, 0);
        // Set text color for contrast
        lv_obj_set_style_text_color(msg_text, lvgl_theme->text_color(), 0);
        
        // Set custom attribute to mark bubble type
        lv_obj_set_user_data(msg_bubble, (void*)"assistant");
        
        // Set appropriate width for content
        lv_obj_set_width(msg_bubble, LV_SIZE_CONTENT);
        lv_obj_set_height(msg_bubble, LV_SIZE_CONTENT);
        
        // Don't grow
        lv_obj_set_style_flex_grow(msg_bubble, 0, 0);
    } else if (strcmp(role, "system") == 0) {
        // System messages are center-aligned with light gray background
        lv_obj_set_style_bg_color(msg_bubble, lvgl_theme->system_bubble_color(), 0);
        lv_obj_set_style_bg_opa(msg_bubble, LV_OPA_70, 0);
        // Set text color for contrast
        lv_obj_set_style_text_color(msg_text, lvgl_theme->system_text_color(), 0);
        
        // Set custom attribute to mark bubble type
        lv_obj_set_user_data(msg_bubble, (void*)"system");
        
        // Set appropriate width for content
        lv_obj_set_width(msg_bubble, LV_SIZE_CONTENT);
        lv_obj_set_height(msg_bubble, LV_SIZE_CONTENT);
        
        // Don't grow
        lv_obj_set_style_flex_grow(msg_bubble, 0, 0);
    }
    
    // Create a full-width container for user messages to ensure right alignment
    if (strcmp(role, "user") == 0) {
        // Create a full-width container
        lv_obj_t* container = lv_obj_create(content_);
        lv_obj_set_width(container, LV_HOR_RES);
        lv_obj_set_height(container, LV_SIZE_CONTENT);
        
        // Make container transparent and borderless
        lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(container, 0, 0);
        lv_obj_set_style_pad_all(container, 0, 0);
        
        // Move the message bubble into this container
        lv_obj_set_parent(msg_bubble, container);
        
        // Right align the bubble in the container
        lv_obj_align(msg_bubble, LV_ALIGN_RIGHT_MID, -25, 0);
        
        // Auto-scroll to this container
        lv_obj_scroll_to_view_recursive(container, LV_ANIM_ON);
    } else if (strcmp(role, "system") == 0) {
        // Create full-width container for system messages to ensure center alignment
        lv_obj_t* container = lv_obj_create(content_);
        lv_obj_set_width(container, LV_HOR_RES);
        lv_obj_set_height(container, LV_SIZE_CONTENT);
        
        lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(container, 0, 0);
        lv_obj_set_style_pad_all(container, 0, 0);
        
        lv_obj_set_parent(msg_bubble, container);
        lv_obj_align(msg_bubble, LV_ALIGN_CENTER, 0, 0);
        lv_obj_scroll_to_view_recursive(container, LV_ANIM_ON);
    } else {
        // For assistant messages
        // Left align assistant messages
        lv_obj_align(msg_bubble, LV_ALIGN_LEFT_MID, 0, 0);

        // Auto-scroll to the message bubble
        lv_obj_scroll_to_view_recursive(msg_bubble, LV_ANIM_ON);
    }
    
    // Store reference to the latest message label
    chat_message_label_ = msg_text;
}

void LcdDisplay::SetPreviewImage(std::unique_ptr<LvglImage> image) {
    DisplayLockGuard lock(this);
    if (content_ == nullptr) {
        return;
    }

    if (image == nullptr) {
        return;
    }
    
    auto lvgl_theme = static_cast<LvglTheme*>(current_theme_);
    // Create a message bubble for image preview
    lv_obj_t* img_bubble = lv_obj_create(content_);
    lv_obj_set_style_radius(img_bubble, 8, 0);
    lv_obj_set_scrollbar_mode(img_bubble, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(img_bubble, 0, 0);
    lv_obj_set_style_pad_all(img_bubble, lvgl_theme->spacing(4), 0);
    
    // Set image bubble background color (similar to system message)
    lv_obj_set_style_bg_color(img_bubble, lvgl_theme->assistant_bubble_color(), 0);
    lv_obj_set_style_bg_opa(img_bubble, LV_OPA_70, 0);
    
    // Set custom attribute to mark bubble type
    lv_obj_set_user_data(img_bubble, (void*)"image");

    // Create the image object inside the bubble
    lv_obj_t* preview_image = lv_image_create(img_bubble);
    
    // Calculate appropriate size for the image
    lv_coord_t max_width = LV_HOR_RES * 70 / 100;  // 70% of screen width
    lv_coord_t max_height = LV_VER_RES * 50 / 100; // 50% of screen height
    
    // Calculate zoom factor to fit within maximum dimensions
    auto img_dsc = image->image_dsc();
    lv_coord_t img_width = img_dsc->header.w;
    lv_coord_t img_height = img_dsc->header.h;
    if (img_width == 0 || img_height == 0) {
        img_width = max_width;
        img_height = max_height;
        ESP_LOGW(TAG, "Invalid image dimensions: %ld x %ld, using default dimensions: %ld x %ld", img_width, img_height, max_width, max_height);
    }
    
    lv_coord_t zoom_w = (max_width * 256) / img_width;
    lv_coord_t zoom_h = (max_height * 256) / img_height;
    lv_coord_t zoom = (zoom_w < zoom_h) ? zoom_w : zoom_h;
    
    // Ensure zoom doesn't exceed 256 (100%)
    if (zoom > 256) zoom = 256;
    
    // Set image properties
    lv_image_set_src(preview_image, img_dsc);
    lv_image_set_scale(preview_image, zoom);
    
    // Add event handler to clean up LvglImage when image is deleted
    // We need to transfer ownership of the unique_ptr to the event callback
    LvglImage* raw_image = image.release(); // Release ownership of smart pointer
    lv_obj_add_event_cb(preview_image, [](lv_event_t* e) {
        LvglImage* img = (LvglImage*)lv_event_get_user_data(e);
        if (img != nullptr) {
            delete img; // Properly release memory by deleting LvglImage object
        }
    }, LV_EVENT_DELETE, (void*)raw_image);
    
    // Calculate actual scaled image dimensions
    lv_coord_t scaled_width = (img_width * zoom) / 256;
    lv_coord_t scaled_height = (img_height * zoom) / 256;
    
    // Set bubble size to be 16 pixels larger than the image (8 pixels on each side)
    lv_obj_set_width(img_bubble, scaled_width + 16);
    lv_obj_set_height(img_bubble, scaled_height + 16);
    
    // Don't grow in flex layout
    lv_obj_set_style_flex_grow(img_bubble, 0, 0);
    
    // Center the image within the bubble
    lv_obj_center(preview_image);
    
    // Left align the image bubble like assistant messages
    lv_obj_align(img_bubble, LV_ALIGN_LEFT_MID, 0, 0);

    // Auto-scroll to the image bubble
    lv_obj_scroll_to_view_recursive(img_bubble, LV_ANIM_ON);
}
#else
void LcdDisplay::SetupUI() {
    DisplayLockGuard lock(this);
    LvglTheme* lvgl_theme = static_cast<LvglTheme*>(current_theme_);
    auto text_font = lvgl_theme->text_font()->font();
    auto icon_font = lvgl_theme->icon_font()->font();
    auto large_icon_font = lvgl_theme->large_icon_font()->font();

    auto screen = lv_screen_active();
    lv_obj_set_style_text_font(screen, text_font, 0);
    lv_obj_set_style_text_color(screen, lvgl_theme->text_color(), 0);
    lv_obj_set_style_bg_color(screen, lvgl_theme->background_color(), 0);

    /* Container - used as background */
    container_ = lv_obj_create(screen);
    lv_obj_set_size(container_, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_radius(container_, 0, 0);
    lv_obj_set_style_pad_all(container_, 0, 0);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_set_style_bg_color(container_, lvgl_theme->background_color(), 0);
    lv_obj_set_style_border_color(container_, lvgl_theme->border_color(), 0);

    /* Bottom layer: emoji_box_ - centered display */
    emoji_box_ = lv_obj_create(screen);
    lv_obj_set_size(emoji_box_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(emoji_box_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(emoji_box_, 0, 0);
    lv_obj_set_style_border_width(emoji_box_, 0, 0);
    lv_obj_align(emoji_box_, LV_ALIGN_CENTER, 0, 0);

    emoji_label_ = lv_label_create(emoji_box_);
    lv_obj_set_style_text_font(emoji_label_, large_icon_font, 0);
    lv_obj_set_style_text_color(emoji_label_, lvgl_theme->text_color(), 0);
    lv_label_set_text(emoji_label_, FONT_AWESOME_MICROCHIP_AI);

    emoji_image_ = lv_img_create(emoji_box_);
    lv_obj_center(emoji_image_);
    lv_obj_add_flag(emoji_image_, LV_OBJ_FLAG_HIDDEN);

    /* Middle layer: preview_image_ - centered display */
    preview_image_ = lv_image_create(screen);
    lv_obj_set_size(preview_image_, width_ / 2, height_ / 2);
    lv_obj_align(preview_image_, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(preview_image_, LV_OBJ_FLAG_HIDDEN);

    /* Layer 1: Top bar - for status icons */
    top_bar_ = lv_obj_create(screen);
    lv_obj_set_size(top_bar_, LV_HOR_RES, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(top_bar_, 0, 0);
    lv_obj_set_style_bg_opa(top_bar_, LV_OPA_50, 0);  // 50% opacity background
    lv_obj_set_style_bg_color(top_bar_, lvgl_theme->background_color(), 0);
    lv_obj_set_style_border_width(top_bar_, 0, 0);
    lv_obj_set_style_pad_all(top_bar_, 0, 0);
    lv_obj_set_style_pad_top(top_bar_, lvgl_theme->spacing(2), 0);
    lv_obj_set_style_pad_bottom(top_bar_, lvgl_theme->spacing(2), 0);
    lv_obj_set_style_pad_left(top_bar_, lvgl_theme->spacing(4), 0);
    lv_obj_set_style_pad_right(top_bar_, lvgl_theme->spacing(4), 0);
    lv_obj_set_flex_flow(top_bar_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(top_bar_, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(top_bar_, LV_SCROLLBAR_MODE_OFF);
    lv_obj_align(top_bar_, LV_ALIGN_TOP_MID, 0, 0);

    // Left icon
    network_label_ = lv_label_create(top_bar_);
    lv_label_set_text(network_label_, "");
    lv_obj_set_style_text_font(network_label_, icon_font, 0);
    lv_obj_set_style_text_color(network_label_, lvgl_theme->text_color(), 0);

    // Right icons container
    lv_obj_t* right_icons = lv_obj_create(top_bar_);
    lv_obj_set_size(right_icons, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(right_icons, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(right_icons, 0, 0);
    lv_obj_set_style_pad_all(right_icons, 0, 0);
    lv_obj_set_flex_flow(right_icons, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(right_icons, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    mute_label_ = lv_label_create(right_icons);
    lv_label_set_text(mute_label_, "");
    lv_obj_set_style_text_font(mute_label_, icon_font, 0);
    lv_obj_set_style_text_color(mute_label_, lvgl_theme->text_color(), 0);

    /* Layer 2: Status bar - for center text labels */
    status_bar_ = lv_obj_create(screen);
    lv_obj_set_size(status_bar_, LV_HOR_RES, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(status_bar_, 0, 0);
    lv_obj_set_style_bg_opa(status_bar_, LV_OPA_TRANSP, 0);  // Transparent background
    lv_obj_set_style_border_width(status_bar_, 0, 0);
    lv_obj_set_style_pad_all(status_bar_, 0, 0);
    lv_obj_set_style_pad_top(status_bar_, lvgl_theme->spacing(2), 0);
    lv_obj_set_style_pad_bottom(status_bar_, lvgl_theme->spacing(2), 0);
    lv_obj_set_scrollbar_mode(status_bar_, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_layout(status_bar_, LV_LAYOUT_NONE, 0);  // Use absolute positioning
    lv_obj_align(status_bar_, LV_ALIGN_TOP_MID, 0, 0);  // Overlap with top_bar_

    notification_label_ = lv_label_create(status_bar_);
    lv_obj_set_width(notification_label_, LV_HOR_RES * 0.75);
    lv_obj_set_style_text_align(notification_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(notification_label_, lvgl_theme->text_color(), 0);
    lv_label_set_text(notification_label_, "");
    lv_obj_align(notification_label_, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(notification_label_, LV_OBJ_FLAG_HIDDEN);

    status_label_ = lv_label_create(status_bar_);
    lv_obj_set_width(status_label_, LV_HOR_RES * 0.75);
    lv_label_set_long_mode(status_label_, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(status_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(status_label_, lvgl_theme->text_color(), 0);
    lv_label_set_text(status_label_, Lang::Strings::INITIALIZING);
    lv_obj_align(status_label_, LV_ALIGN_CENTER, 0, 0);

    /* Top layer: Bottom bar - fixed at bottom, device-round VQ-A7 r5 起高度钉死 48 (设计值,
     * 弃 adaptive content 撑高——底部键帽条上沿 3px 实线必须锁在屏底 252, 见下方 r5 注)。
     * min_height 48 既有保守设定一并保留 (双保险, 不触 r3-A1 上沿实线刀)。 */
    bottom_bar_ = lv_obj_create(screen);
    lv_obj_set_width(bottom_bar_, LV_HOR_RES);
    lv_obj_set_style_min_height(bottom_bar_, 48, 0); // Set minimum height 48
    lv_obj_set_style_radius(bottom_bar_, 0, 0);
    lv_obj_set_style_bg_color(bottom_bar_, lvgl_theme->background_color(), 0);
    lv_obj_set_style_text_color(bottom_bar_, lvgl_theme->text_color(), 0);
    lv_obj_set_style_pad_top(bottom_bar_, lvgl_theme->spacing(2), 0);
    lv_obj_set_style_pad_bottom(bottom_bar_, lvgl_theme->spacing(2), 0);
    lv_obj_set_style_pad_left(bottom_bar_, lvgl_theme->spacing(4), 0);
    lv_obj_set_style_pad_right(bottom_bar_, lvgl_theme->spacing(4), 0);
    /* 底部键帽条上沿 3px 实线分隔 (device-round VQ-A7 r3 §A1; brief §2.3 原文「上沿
     * 3px 实线分隔」; mock kidpal_ui.c:224-226 同款画法: border_width 3 + border_side
     * TOP)。旧 border_width 0 写点销账 (LVGL 同属性后写覆盖前写, 残留即被抹平)。 */
    lv_obj_set_style_border_color(bottom_bar_, lvgl_theme->text_color(), 0);
    lv_obj_set_style_border_width(bottom_bar_, 3, 0);
    lv_obj_set_style_border_side(bottom_bar_, LV_BORDER_SIDE_TOP, 0);
    /* device-round VQ-A7 r5 (中部实线定谳): 高度钉 48 设计值 (brief §2.3), 弃
     * lv_obj_set_height(LV_SIZE_CONTENT) —— LV_SIZE_CONTENT + min_height 48 会被子件
     * (chat_message_label_ 宽 368 LV_LABEL_LONG_WRAP 居中) 长文本撑高, 上沿随顶边抬到
     * 屏中部 (照片 y≈163 全宽实线根因; 取证 = 静态推演 + 照片 y163 吻合, 见交付报告)。
     * 钉 48 后上沿 3px 实线固定落在屏底 252 (键槽上沿), 不再随内容漂移。内容溢出裁剪:
     * lv_obj_create 默认带 LV_OBJ_FLAG_SCROLLABLE (子件可滚出越界不被裁), 需 clear_flag
     * 使超 48px 内容被底条边界裁剪, 保 keycap/页点 BOTTOM 对齐仍落底部。K3DIAG 打印供
     * 主线程烧板复核 lv_obj_get_height。 */
    {
        lv_obj_set_height(bottom_bar_, 48);                     /* 设计值钉死, 弃自适应撑高 */
        lv_obj_clear_flag(bottom_bar_, LV_OBJ_FLAG_SCROLLABLE); /* 溢出裁剪: 超出即被底条截 */
    }
    lv_obj_set_style_min_height(bottom_bar_, 48, 0); // 保守设定: 固定高 48 与 min 48 双保险
    lv_obj_align(bottom_bar_, LV_ALIGN_BOTTOM_MID, 0, 0);

    /* chat_message_label_ placed in bottom_bar_ and vertically centered */
    chat_message_label_ = lv_label_create(bottom_bar_);
    lv_label_set_text(chat_message_label_, "");
    lv_obj_set_width(chat_message_label_, LV_HOR_RES - lvgl_theme->spacing(8)); // Subtract left and right padding
    lv_label_set_long_mode(chat_message_label_, LV_LABEL_LONG_WRAP); // Auto wrap mode
    lv_obj_set_style_text_align(chat_message_label_, LV_TEXT_ALIGN_CENTER, 0); // Center text alignment
    lv_obj_set_style_text_color(chat_message_label_, lvgl_theme->text_color(), 0);
    lv_obj_align(chat_message_label_, LV_ALIGN_CENTER, 0, 0); // Vertically and horizontally centered in bottom_bar_
    g_kidpal_chat_label = chat_message_label_; /* 五态气泡/同步文字挂点 (file-static 跟踪) */

    /* 底部键帽条三槽 (KB-A1, 通栏贴底常驻 chrome): 左 KEY 提示槽 · 中页点槽 · 右
     * BOOT 提示槽, 全部建在 bottom_bar_ 载体上 (通栏贴底对齐在世, 本包零触碰其几何),
     * 随聊天 chrome 一起迁 lv_layer_top() 任何页可见。替换旧单行居中文案形态。左右槽
     * 随态写点: apply_look 左槽消费 look.keycap、sync 经 keybar 单源取左右槽值。 */
    /* 键帽胶囊 (device-round VQ-A7 r5 任务3; brief §2.3: 高 26 / 圆角 8 / 3px 描边 / 白底
     * 黑字; mock kidpal_ui.c keycap_at() 同款——LV_SIZE_CONTENT 胶囊 + 居中内 label)。
     * 左右槽文案仍单源 = kidpal_keybar_slots (kidpal_keybar_slots 零改动), 键帽内 label
     * 仍叫 g_kidpal_keybar_left_label 供 apply_look/sync 消费。字体取舍: mock 用圆胖
     * lv_font_montserrat_14, 但设备键帽值域含中文 (look.keycap「说完啦/不等了/打断…」与
     * 右槽「翻页」等), montserrat 无中文覆盖会整段空白 (KB-A5 同族根因) —— 设备侧在世全量
     * 字体族取最近口径 = 既有键帽/正文全量 font_puhui_14_1 (覆盖 KEY·BOOT 拉丁 + 全量中文)。 */
    {
        lv_obj_t* keycap_left = lv_obj_create(bottom_bar_);
        lv_obj_remove_style_all(keycap_left);
        lv_obj_set_size(keycap_left, LV_SIZE_CONTENT, 26);
        lv_obj_set_style_pad_hor(keycap_left, 7, 0);
        lv_obj_set_style_radius(keycap_left, 8, 0);
        lv_obj_set_style_bg_color(keycap_left, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(keycap_left, LV_OPA_COVER, 0);
        lv_obj_set_style_border_color(keycap_left, lv_color_black(), 0);
        lv_obj_set_style_border_width(keycap_left, 3, 0);
        lv_obj_clear_flag(keycap_left, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_align(keycap_left, LV_ALIGN_BOTTOM_LEFT, 0, 0);
        /* KB-A1 (keybar_three_slot) 冻结齿要求槽文案 label 建于底部键帽条载体上
         * (lv_label_create(bottom_bar_))——本 label 先在 bottom_bar_ 上建 (计齿), 再
         * 经 lv_obj_set_parent 重挂进胶囊容器以承载其纯视觉铺底 (mock keycap_at() 同形态:
         * 容器 26px 白底黑描边圆角 + 居中内 label)。槽文案值域仍 single source. */
        g_kidpal_keybar_left_label = lv_label_create(bottom_bar_);
        lv_obj_set_parent(g_kidpal_keybar_left_label, keycap_left);
        lv_label_set_text(g_kidpal_keybar_left_label, "");
        lv_obj_set_style_text_font(g_kidpal_keybar_left_label, &font_puhui_14_1, 0);
        lv_obj_set_style_text_color(g_kidpal_keybar_left_label, lv_color_black(), 0);
        lv_obj_center(g_kidpal_keybar_left_label);
    }

    g_kidpal_keybar_dots_box = lv_obj_create(bottom_bar_);
    lv_obj_set_style_bg_opa(g_kidpal_keybar_dots_box, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(g_kidpal_keybar_dots_box, 0, 0);
    lv_obj_set_style_radius(g_kidpal_keybar_dots_box, 0, 0);
    lv_obj_set_style_pad_all(g_kidpal_keybar_dots_box, 0, 0);
    lv_obj_align(g_kidpal_keybar_dots_box, LV_ALIGN_BOTTOM_MID, 0, 0);
    for (int d = 0; d < (int)PAGE_COUNT; d++) {
        g_kidpal_keybar_dot[d] = lv_obj_create(g_kidpal_keybar_dots_box);
        lv_obj_set_size(g_kidpal_keybar_dot[d], 8, 8);
        lv_obj_set_style_radius(g_kidpal_keybar_dot[d], 4, 0);
        lv_obj_set_style_bg_color(g_kidpal_keybar_dot[d], lvgl_theme->text_color(), 0);
        lv_obj_set_style_bg_opa(g_kidpal_keybar_dot[d], LV_OPA_COVER, 0);
        /* V5 对齐 (ui-v5-align §1.3): 非当前页 = 空心环（白芯黑描边）。描边色在此
         * 一次性钉黑；实心/空心的切换在 kidpal_keybar_sync_locked 按页态写。 */
        lv_obj_set_style_border_color(g_kidpal_keybar_dot[d], lv_color_black(), 0);
        lv_obj_align(g_kidpal_keybar_dot[d], LV_ALIGN_BOTTOM_MID,
                     (d - ((int)PAGE_COUNT - 1) / 2) * 16, 0);
    }

    /* 右 BOOT 槽键帽胶囊: 同左槽 (略去重复注释; mock keycap_at() 同款, 见左槽注)。
     * label 先在 bottom_bar_ 建 (计 KB-A1 齿) 再重挂进胶囊。 */
    {
        lv_obj_t* keycap_right = lv_obj_create(bottom_bar_);
        lv_obj_remove_style_all(keycap_right);
        lv_obj_set_size(keycap_right, LV_SIZE_CONTENT, 26);
        lv_obj_set_style_pad_hor(keycap_right, 7, 0);
        lv_obj_set_style_radius(keycap_right, 8, 0);
        lv_obj_set_style_bg_color(keycap_right, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(keycap_right, LV_OPA_COVER, 0);
        lv_obj_set_style_border_color(keycap_right, lv_color_black(), 0);
        lv_obj_set_style_border_width(keycap_right, 3, 0);
        lv_obj_clear_flag(keycap_right, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_align(keycap_right, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
        g_kidpal_keybar_right_label = lv_label_create(bottom_bar_);
        lv_obj_set_parent(g_kidpal_keybar_right_label, keycap_right);
        lv_label_set_text(g_kidpal_keybar_right_label, "");
        lv_obj_set_style_text_font(g_kidpal_keybar_right_label, &font_puhui_14_1, 0);
        lv_obj_set_style_text_color(g_kidpal_keybar_right_label, lv_color_black(), 0);
        lv_obj_center(g_kidpal_keybar_right_label);
    }

    /* 三槽建完后立即首刷 (R1 修复, ①/③/④): 基础页/启动后 keybar 必须落字与页点, 不能
     * 只等语音/前台桥触发。此处 SetupUI 已持 lvgl 锁, sync_locked 只读 render_node 状态
     * (boot 期 layer 栈空 -> overlay_top=0, get_page=KP_PAGE_HOME) —— 首页基层即落字。 */
    kidpal_keybar_sync_locked();

    /* device-round VQ-A7 r5 取证 (任务4 中部实线定谳): 烧板复核 —— 高度钉 48 后
     * lv_obj_get_height(bottom_bar_) 应读 48, 上沿 3px 实线才落在屏底 252 (键槽上沿)。
     * 同时打印 chat_message_label_ 当前文本 (若仍内容撑高即此处余量观察), 供主线程核对。
     * 仅诊断打印, 不影响渲染, 不参与任何判定。 */
    ESP_LOGI("K3DIAG", "bottom_bar h=%d text='%s'",
             (int)lv_obj_get_height(bottom_bar_),
             lv_label_get_text(chat_message_label_));

    /* 语音轮整页版式 (device-round VQ-A7 r1 修复面③; WF-B-01~03 冻结线框 = 语音轮
     * 四态同一版式骨架「左小伴 pose · 右内容区」, requirements v1.24 human 冻结 +
     * UIUX-DESIGN-BRIEF §5 语音轮四态行同锚): 语音轮活跃 (五态演出 + VOICE 前台) 时
     * 整页盖住首页内容 —— 首页猫遮蔽销账取「整页版式盖住」形态 (冻结线框无首页内容,
     * 非单屏模型冲突, 零冻结稿改动)。容器常驻 lv_layer_top 且创建序先于 chrome 迁移
     * (板级装配 KidPalReparentChatToLayer 后迁) -> 键帽条/状态栏等 chrome 浮其上任何
     * 状态可见; 默认隐藏, 显隐单点 = kidpal_chrome_mid_sync_locked。 */
    g_kidpal_voice_page = lv_obj_create(lv_layer_top());
    lv_obj_set_size(g_kidpal_voice_page, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(g_kidpal_voice_page, lvgl_theme->background_color(), 0);
    lv_obj_set_style_bg_opa(g_kidpal_voice_page, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(g_kidpal_voice_page, 0, 0);
    lv_obj_set_style_border_width(g_kidpal_voice_page, 0, 0);
    lv_obj_set_style_pad_all(g_kidpal_voice_page, 0, 0);
    lv_obj_clear_flag(g_kidpal_voice_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(g_kidpal_voice_page, LV_OBJ_FLAG_HIDDEN);

    /* 左小伴 pose 挂点 (WF-B 左列, 96×96 左上; pose 图由 apply_look 在此容器居中,
     * 姿态不再 96×96 居中叠首页猫。像素级落位归真机轮 VQ-A7 观感微调)。 */
    g_kidpal_pose_box = lv_obj_create(g_kidpal_voice_page);
    lv_obj_set_size(g_kidpal_pose_box, 96, 96);
    lv_obj_set_pos(g_kidpal_pose_box, 16, 36);
    lv_obj_set_style_bg_opa(g_kidpal_pose_box, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(g_kidpal_pose_box, 0, 0);
    lv_obj_set_style_pad_all(g_kidpal_pose_box, 0, 0);
    lv_obj_clear_flag(g_kidpal_pose_box, LV_OBJ_FLAG_SCROLLABLE);

    /* 五态姿态图父容器 = 语音轮整页版式左列 pose 挂点 (WF-B 骨架; 原挂 emoji_box_
     * 居中 = 叠首页猫缺陷面, VQ-A7 r1 事实④)。笑脸 label/图仍住 emoji_box_ (chrome
     * 中部显隐纪律管互斥, 演出期隐、非演出期回)。 */
    g_kidpal_pose_parent = g_kidpal_pose_box;
    g_kidpal_emoji_label = emoji_label_;
    g_kidpal_emoji_image = emoji_image_;

    /* 右内容区标头 (WF-B 标头位; look.bubble 写点由旧 g_kidpal_chat_label 底栏挂点统一
     * 迁此 —— 「思考中」/「播报」/「我在听！」三态标头, 单源从 kidpal_voice_states
     * 闭集取文案, 不另抄。创建序在 echo_box 之前 = 回显行在标头下方 (VQ-A2 冻结原文
     * 「回显在『我在听！』标识下方」; LVGL 同父子件叠序 = 创建序)。全量字体同 r1 口径)。 */
    g_kidpal_voice_header_label = lv_label_create(g_kidpal_voice_page);
    lv_obj_set_pos(g_kidpal_voice_header_label, 124, 16);
    lv_obj_set_width(g_kidpal_voice_header_label, 264);
    lv_obj_set_style_text_align(g_kidpal_voice_header_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(g_kidpal_voice_header_label, lvgl_theme->text_color(), 0);
    lv_obj_set_style_text_font(g_kidpal_voice_header_label, &font_puhui_14_1, 0);
    lv_label_set_text(g_kidpal_voice_header_label, "");

    /* 右内容区两容器 (WF-B 右列, x = 16+96+12 = 124, 宽 = 400-124-12 = 264; 同一内容
     * 区按场景互斥显隐: LISTEN/THINK -> 回显区, TALK -> 问答框, 同步点同上)。均整块
     * 占满右列 (固定高度 = 滚动机制有界承载, VQ-A2「超屏高滚出」/VQ-A3「长文滚动
     * 看完」); 回显/问答对象 (file-static) 供活 SetChatMessage 按 role 路由消费。 */
    g_kidpal_voice_echo_box = lv_obj_create(g_kidpal_voice_page);
    lv_obj_set_style_bg_opa(g_kidpal_voice_echo_box, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(g_kidpal_voice_echo_box, 0, 0);
    lv_obj_set_style_pad_all(g_kidpal_voice_echo_box, 0, 0);
    lv_obj_set_size(g_kidpal_voice_echo_box, 264, 216);
    lv_obj_set_pos(g_kidpal_voice_echo_box, 124, 36);
    lv_obj_set_scroll_dir(g_kidpal_voice_echo_box, LV_DIR_VER);
    lv_obj_add_flag(g_kidpal_voice_echo_box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(g_kidpal_voice_echo_box, LV_SCROLLBAR_MODE_AUTO);
    g_kidpal_voice_echo_label = lv_label_create(g_kidpal_voice_echo_box);
    lv_label_set_long_mode(g_kidpal_voice_echo_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(g_kidpal_voice_echo_label, 264);
    lv_obj_set_style_text_align(g_kidpal_voice_echo_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(g_kidpal_voice_echo_label, lvgl_theme->text_color(), 0);
    /* 全量字体: 孩子话语中文字形全覆盖 (LVGL 默认字体无中文字形 = 整段空白零上屏,
     * device-round VQ-A7 r1 事实③; 对照 keybar font_puhui_14_1 先例同形态。14px =
     * NFR-READ-1 1bpp CJK 可读性下限定案档, 全量 puhui 非 basic 稀疏子集)。 */
    lv_obj_set_style_text_font(g_kidpal_voice_echo_label, &font_puhui_14_1, 0);
    lv_label_set_text(g_kidpal_voice_echo_label, "");

    g_kidpal_voice_answer_box = lv_obj_create(g_kidpal_voice_page);
    lv_obj_set_style_bg_opa(g_kidpal_voice_answer_box, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(g_kidpal_voice_answer_box, 0, 0);
    lv_obj_set_style_pad_all(g_kidpal_voice_answer_box, 0, 0);
    lv_obj_set_size(g_kidpal_voice_answer_box, 264, 216);
    lv_obj_set_pos(g_kidpal_voice_answer_box, 124, 36);
    lv_obj_set_scroll_dir(g_kidpal_voice_answer_box, LV_DIR_VER);
    lv_obj_add_flag(g_kidpal_voice_answer_box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(g_kidpal_voice_answer_box, LV_SCROLLBAR_MODE_AUTO);
    g_kidpal_voice_answer_label = lv_label_create(g_kidpal_voice_answer_box);
    lv_label_set_long_mode(g_kidpal_voice_answer_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(g_kidpal_voice_answer_label, 264);
    lv_obj_set_style_text_align(g_kidpal_voice_answer_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(g_kidpal_voice_answer_label, lvgl_theme->text_color(), 0);
    /* 全量字体: 同回显区 (答案全文中文覆盖; NFR-READ-1 14px 下限档, 禁 basic 稀疏
     * 子集 —— 键帽 KB-A5 同族根因)。 */
    lv_obj_set_style_text_font(g_kidpal_voice_answer_label, &font_puhui_14_1, 0);
    lv_label_set_text(g_kidpal_voice_answer_label, "");

    low_battery_popup_ = lv_obj_create(screen);
    lv_obj_set_scrollbar_mode(low_battery_popup_, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(low_battery_popup_, LV_HOR_RES * 0.9, text_font->line_height * 2);
    lv_obj_align(low_battery_popup_, LV_ALIGN_BOTTOM_MID, 0, -lvgl_theme->spacing(4));
    lv_obj_set_style_bg_color(low_battery_popup_, lvgl_theme->low_battery_color(), 0);
    lv_obj_set_style_radius(low_battery_popup_, lvgl_theme->spacing(4), 0);
    
    low_battery_label_ = lv_label_create(low_battery_popup_);
    lv_label_set_text(low_battery_label_, Lang::Strings::BATTERY_NEED_CHARGE);
    lv_obj_set_style_text_color(low_battery_label_, lv_color_white(), 0);
    lv_obj_center(low_battery_label_);
    lv_obj_add_flag(low_battery_popup_, LV_OBJ_FLAG_HIDDEN);

    /* self 注册 (RP-A5 r4): render_node 前台维度桥取 display 用, 不经
     * Board::GetInstance() (boot 期单例重入死锁根因)。 */
    s_kidpal_lcd_self = this;
}

void LcdDisplay::SetPreviewImage(std::unique_ptr<LvglImage> image) {
    DisplayLockGuard lock(this);
    if (preview_image_ == nullptr) {
        ESP_LOGE(TAG, "Preview image is not initialized");
        return;
    }

    if (image == nullptr) {
        esp_timer_stop(preview_timer_);
        lv_obj_remove_flag(emoji_box_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(preview_image_, LV_OBJ_FLAG_HIDDEN);
        preview_image_cached_.reset();
        if (gif_controller_) {
            gif_controller_->Start();
        }
        return;
    }

    preview_image_cached_ = std::move(image);
    auto img_dsc = preview_image_cached_->image_dsc();
    lv_image_set_src(preview_image_, img_dsc);
    if (img_dsc->header.w > 0 && img_dsc->header.h > 0) {
        // zoom factor 0.5
        lv_image_set_scale(preview_image_, 128 * width_ / img_dsc->header.w);
    }

    // Hide emoji_box_
    if (gif_controller_) {
        gif_controller_->Stop();
    }
    lv_obj_add_flag(emoji_box_, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(preview_image_, LV_OBJ_FLAG_HIDDEN);
    esp_timer_stop(preview_timer_);
    ESP_ERROR_CHECK(esp_timer_start_once(preview_timer_, PREVIEW_IMAGE_DURATION_MS * 1000));
}

void LcdDisplay::SetChatMessage(const char* role, const char* content) {
    DisplayLockGuard lock(this);
    if (chat_message_label_ == nullptr) {
        return;
    }
    (void)role;

    /* 语音轮演出路由 (包 260915-m1-k3-voice-round-ui, seam [3]): role "user" ->
     * 聆听话语回显区消费 kidpal_voice_echo_push 逐句入列 (纯逻辑单源, 设备侧零重算)
     * 并整列重建上屏 g_kidpal_voice_echo_box; role "assistant" -> 答案全文渲到
     * g_kidpal_voice_answer_box (问答框口径, S-F1.3 AC1「与百科问答同一问答框」,
     * 长文滚动由容器 LV_OBJ_FLAG_SCROLLABLE 承载, 非一行字幕)。其余 role (如 system
     * 系统态) 走既有同步文字框 chat_message_label_ 语义, 不动。 */
    if (content != nullptr && role != nullptr && strcmp(role, "user") == 0) {
        if (g_kidpal_voice_echo_box != nullptr) {
            kidpal_voice_echo_push(content);
            /* 整列重建: 逐句经纯逻辑单源取行 (行序/容量滚出全归单源), 设备侧只拼串 */
            if (g_kidpal_voice_echo_label != nullptr) {
                char line_buf[512] = {0};
                size_t off = 0;
                int n = kidpal_voice_echo_count();
                for (int i = 0; i < n; i++) {
                    const char* line = NULL;
                    if (kidpal_voice_echo_line(i, &line) == 0 && line != NULL) {
                        int w = snprintf(line_buf + off, sizeof(line_buf) - off,
                                         "%s%s", off ? "\n" : "", line);
                        if (w > 0 && (size_t)w < sizeof(line_buf) - off) {
                            off += (size_t)w;
                        } else {
                            break;
                        }
                    }
                }
                lv_label_set_text(g_kidpal_voice_echo_label, line_buf);
                /* 新句入列后滚到底部让最新话语可见 */
                lv_obj_scroll_to_y(g_kidpal_voice_echo_box,
                                   lv_obj_get_scroll_bottom(g_kidpal_voice_echo_box),
                                   LV_ANIM_OFF);
            }
        }
        return;
    }
    if (content != nullptr && role != nullptr && strcmp(role, "assistant") == 0) {
        if (g_kidpal_voice_answer_box != nullptr &&
            g_kidpal_voice_answer_label != nullptr) {
            lv_label_set_text(g_kidpal_voice_answer_label, content);
            lv_obj_scroll_to_y(g_kidpal_voice_answer_box, 0, LV_ANIM_OFF);
        }
        return;
    }

    lv_label_set_text(chat_message_label_, content);
}
#endif

/* -------------------------------------------------------------------------- *
 * 三槽槽位随态刷新 (键帽条 chrome, 任何状态常驻): 页点显隐读 layer_top —— 与板级
 * dispatch 键分流同源 via kidpal_render_node_front_is_overlay (KB-A4, 不另立第二真相
 * 源); 左右槽文案与页点机制值全部出自 keybar 纯逻辑单源 (kidpal_keybar_slots /
 * kidpal_keybar_dots), 设备侧零重算。keybar 只读 layer_top 显隐、零覆盖层进出裁决权
 * (KB-E3, 本函数不含任何 kidpal_layer_push/pop)。调用方必须已持 LVGL 锁。
 * ------------------------------------------------------------------------ */
static void kidpal_keybar_sync_locked(void)
{
    kidpal_keybar_dots_t d;
    const char *left = NULL, *right = NULL;
    int rn_page = kidpal_render_node_get_page();
    int overlay_top = kidpal_render_node_front_is_overlay();

    /* 渲染节点页 (KP_PAGE_*) 与键路由页序 (PAGE_P1..PAGE_P3) 撤 P4 后逐项同序同值，
     * 页索引映射塌成恒等——直取即为「当前画的是哪一页」，无需换算表（实心页点与页槽
     * 表按当前页定格）。越界页一律回落首页（防御语义零改动）。 */
    int page = (rn_page >= 0 && rn_page < (int)PAGE_COUNT) ? rn_page : PAGE_P1;

    if (g_kidpal_keybar_left_label != NULL && g_kidpal_keybar_right_label != NULL) {
        if (kidpal_keybar_slots(page, g_kidpal_keybar_scene, overlay_top,
                                &left, &right) == 0) {
            if (left != NULL) {
                lv_label_set_text(g_kidpal_keybar_left_label, left);
            }
            if (right != NULL) {
                lv_label_set_text(g_kidpal_keybar_right_label, right);
            }
        }
    }

    if (kidpal_keybar_dots(page, overlay_top, &d) == 0 &&
        g_kidpal_keybar_dots_box != NULL) {
        if (d.visible) {
            lv_obj_remove_flag(g_kidpal_keybar_dots_box, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(g_kidpal_keybar_dots_box, LV_OBJ_FLAG_HIDDEN);
        }
        for (int i = 0; i < (int)PAGE_COUNT; i++) {
            if (g_kidpal_keybar_dot[i] != NULL) {
                int act = (i == d.active);
                lv_obj_set_size(g_kidpal_keybar_dot[i], act ? 12 : 8, act ? 12 : 8);
                lv_obj_set_style_radius(g_kidpal_keybar_dot[i], act ? 6 : 4, 0);
                if (act) {
                    /* 当前页 = 实心点（原形态不变）。 */
                    lv_obj_set_style_bg_opa(g_kidpal_keybar_dot[i], LV_OPA_COVER, 0);
                    lv_obj_set_style_border_width(g_kidpal_keybar_dot[i], 0, 0);
                } else {
                    /* V5 对齐: 非当前页 = 空心环（白芯 + 2px 黑描边），替换旧 8px
                     * 实心缩点。机制值（visible/total/active）出自 keybar 单源，
                     * 本处只改呈现。 */
                    lv_obj_set_style_bg_opa(g_kidpal_keybar_dot[i], LV_OPA_TRANSP, 0);
                    lv_obj_set_style_border_width(g_kidpal_keybar_dot[i], 2, 0);
                }
            }
        }
    }
}

/* -------------------------------------------------------------------------- *
 * 五态演出内容驱动实现 (包 260911-render-pages RP-A9)。全部位于活分支侧 (最后一个
 * SetupUI 定义点之后), 经 kidpal_voice_state_look 单源取五态 look —— 姿态 / 气泡 /
 * 键帽条文案。层底座与优先级仲裁零触碰 (RP-A9)。气泡尾巴资产 bubble_tail_24.bin
 * 为 V3 气泡 (frm-sm 圆角框 + 左下斜切尾巴) 的完成件, 消费接线见 v3_restore 齿。
 * ------------------------------------------------------------------------ */
static void kidpal_voice_apply_look(int scene)
{
    kidpal_voice_look_t look;
    uint32_t* new_px = NULL;
    int       new_w = 0, new_h = 0;
    int       need_pose = 0;

    /* F2 驻留观测 + 守卫 (RM-A8, FX-A3 不变量, 常驻日志量级无害): 先记录本次语音视觉
     * apply 的场景与当前前台层 (serial-logmable 观测点, device-round-1 F2 取证依赖)。
     * 守卫 = 提醒弹窗驻留 (REMINDER 前台) 期间跳过非 REMIND 语音场景 —— 弹窗与语音轮
     * 共用同一批骨架对象 (pose_box/voice_header_label/keycap/emblem), TTS 起播
     * SetStatus(SPEAKING) -> apply_look(KVST_SCENE_TALK) 会在弹窗之上覆写 pose-jump 与
     * 时间行标头 (弹窗即起即盖, F2 根因本体)。REMIND 场景自身放行 (弹窗绘制路径
     * kidpal_lcd_reminder_show:2116 调用), 停驻期间其它语音场景一律跳过。
     * 回归安全 (FX-A6): 无提醒驻留 (front_is_reminder==0) 时守卫恒不触发 -> 语音轮
     * LISTEN/THINK/TALK 视觉与修复前逐位一致。 */
    ESP_LOGI(TAG, "voice_look scene=%d top=%d", scene, kidpal_render_node_front_layer());
    if (scene != KVST_SCENE_REMIND && kidpal_render_node_front_is_reminder() == 1) {
        ESP_LOGI(TAG, "voice_look_skip scene=%d top=%d reason=reminder_persist",
                 scene, kidpal_render_node_front_layer());
        return;
    }

    /* 五态内容选择单源 (weak 哨兵被强符号覆盖后为真实映射)。 */
    if (kidpal_voice_state_look(scene, &look) != 0 || look.keycap == NULL) {
        return;
    }

    /* 演出活跃期: 姿态顶替笑脸位 (chrome 中部显隐纪律, RP-A5 r2)。 */
    g_kidpal_mid_mode = 1;

    /* 姿态资产加载 + 像素合成不占 LVGL 锁 (文件 IO / 堆分配), 先备好新缓冲;
     * 锁内只做对象树操作。按场景一次性尝试 (g_kidpal_pose_scene 先行登记),
     * 同场景加载失败不每帧重读资产。 */
    if (scene != g_kidpal_pose_scene) {
        int pose = look.pose;
        g_kidpal_pose_scene = scene;
        need_pose = 1;
        if (pose >= 0 && pose <= KVST_POSE_SLEEP) {
            kidpal_art_img_t img;
            if (kidpal_art_load(g_kidpal_pose_asset[pose], &img) == 0 &&
                img.w > 0 && img.h > 0 && img.w <= 96 && img.h <= 96) {
                new_px = (uint32_t*)lv_malloc((size_t)img.w * img.h * sizeof(uint32_t));
                if (new_px != NULL) {
                    for (int y = 0; y < img.h; y++) {
                        for (int x = 0; x < img.w; x++) {
                            int p = kidpal_art_pixel(&img, x, y);
                            new_px[y * img.w + x] = (p == 1) ? 0xFF000000u : 0x00000000u;
                        }
                    }
                    new_w = img.w;
                    new_h = img.h;
                }
            }
        }
    }

    /* LVGL 锁内: 键帽条 / 气泡文案 + 姿态图对象切换全部在锁内执行 (锁外调
     * lv_label_set_text / lv_image_* = 主任务与 LVGL 任务并发触对象树, 堆/链表
     * 腐蚀殃及 TLS/WS 路径——RP-A5 真机轮上行音频零帧根因)。基类
     * LvglDisplay::SetStatus 的 DisplayLockGuard 已在基类返回时释放, 此处取锁
     * 无嵌套死锁。 */
    if (!lvgl_port_lock(30000)) {
        if (new_px != NULL) {
            lv_free(new_px);
        }
        ESP_LOGE(TAG, "kidpal_voice_apply_look: acquire lvgl lock timeout (scene=%d)", scene);
        return;
    }

    /* 三槽·左槽随态文案 (任何状态常驻, V3 §06): 左槽消费 look.keycap —— 值域只经
     * kidpal_voice_states 冻结闭集单源 (KB-A3/KB-E2, 键帽条文案不再有第二条来源)。 */
    if (g_kidpal_keybar_left_label != NULL) {
        lv_label_set_text(g_kidpal_keybar_left_label, look.keycap);
    }
    /* 记录当前语音场景供随态右槽 (结束/关闭) 裁决; 顺带刷新中/右槽 + 页点显隐
     * (覆盖层随态, 锁已在持)。 */
    g_kidpal_keybar_scene = scene;
    kidpal_keybar_sync_locked();

    /* 三态标识件按场景显隐 (r3-C: 聆听态 mic / 思考态问号 / 播报态声波条 + lv_anim;
     * 惰性建一次于本函数, 场景切换只切 HIDDEN)。 */
    kidpal_voice_emblem_apply_locked(scene);

    /* 气泡标头文案: 写右内容区标头对象 (g_kidpal_voice_header_label, voice_page 直接
     * 子件; 统一写点由旧底栏 chat 挂点迁至此处 —— 三态标头单源仍从
     * kidpal_voice_states 闭集取 look.bubble, 不另抄第二份文案来源)。气泡尾巴资产
     * (bubble_tail_24.bin, r3-B) 作标头气泡背景件随 bubble 显隐 —— 三态标头挂进
     * 气泡形态消费该资产 (适用态 = 三标头态; (void) 闲置已销账)。无独立 bubble 的
     * 场景 (REMIND/LOWBATT) 该标头与气泡背景不动。 */
    if (look.bubble != NULL && g_kidpal_voice_header_label != NULL) {
        lv_label_set_text(g_kidpal_voice_header_label, look.bubble);
        if (g_kidpal_bubble_img != NULL) {
            lv_obj_remove_flag(g_kidpal_bubble_img, LV_OBJ_FLAG_HIDDEN);
        }
    } else if (g_kidpal_bubble_img != NULL) {
        lv_obj_add_flag(g_kidpal_bubble_img, LV_OBJ_FLAG_HIDDEN);
    }

    if (need_pose) {
        /* 场景切换: 旧姿态图对象与旧像素缓冲一并回收, 再按新缓冲重建 —— 杜绝
         * 每次切换漏 36KB + 旧图永不删 (真机「两只猫咪」即此因)。 */
        if (g_kidpal_pose_img != NULL) {
            lv_obj_del(g_kidpal_pose_img);
            g_kidpal_pose_img = NULL;
        }
        if (g_kidpal_pose_px != NULL) {
            lv_free(g_kidpal_pose_px);
            g_kidpal_pose_px = NULL;
        }
        if (new_px != NULL && g_kidpal_pose_parent != NULL) {
            /* 挂语音轮整页版式左列 pose 挂点 (SetupUI 活分支 g_kidpal_pose_box, WF-B
             * 「左小伴」列; device-round VQ-A7 r1 修复面③ —— 原 96×96 居中叠首页猫
             * 缺陷面销账, pose 不再压首页猫/时间, 显隐归 chrome 中部显隐纪律)。 */
            memset(&g_kidpal_pose_dsc, 0, sizeof(g_kidpal_pose_dsc));
            g_kidpal_pose_dsc.header.cf     = LV_COLOR_FORMAT_ARGB8888;
            g_kidpal_pose_dsc.header.w      = (uint32_t)new_w;
            g_kidpal_pose_dsc.header.h      = (uint32_t)new_h;
            g_kidpal_pose_dsc.header.stride = (uint32_t)new_w * 4u;
            g_kidpal_pose_dsc.data          = (const uint8_t*)new_px;
            g_kidpal_pose_dsc.data_size     = (uint32_t)(new_w * new_h * 4u);
            g_kidpal_pose_px = new_px;
            new_px = NULL;
            g_kidpal_pose_img = lv_image_create(g_kidpal_pose_parent);
            lv_image_set_src(g_kidpal_pose_img, &g_kidpal_pose_dsc);
            lv_obj_center(g_kidpal_pose_img);
            g_kidpal_pose_st = 1;
        } else {
            /* 资产缺失 / 父容器未就绪: 本轮无姿态 (旧件已回收), 场景再变时重试。 */
            if (new_px != NULL) {
                lv_free(new_px);
                new_px = NULL;
            }
            g_kidpal_pose_st = 2;
        }
    }

    /* chrome 中部显隐重断言 (本函数只在 LISTENING/SPEAKING 路径被调, mode=1)。 */
    kidpal_chrome_mid_sync_locked();

    lvgl_port_unlock();
}

/* ---- r3-C 三态标识件构建 + 显隐 (C1 mic / C2 飘问号 / C3 声波条 lv_anim) ----
 * 1bpp KAP1 资产惰性加载进 ARGB 缓冲 -> 建 lv_image, 一次建成后常驻, 场景切换只切
 * HIDDEN。同姿态纪律: 资产加载在锁内做 (本函数只被 apply_look 锁内调用), 对象树操作
 * 单点。声波条 = LVGL 原生矩形组 + lv_anim 跳动 (免管线、动画天然, 不与资产闭集治理
 * 冲突; KAP1 位图备选为返钉路径)。 */

static void kidpal_voice_wave_anim_cb(void* var, int32_t v)
{
    (void)var;
    /* r4 精修: 小波浪条 (高 4..13, 收进 14px 条带)。相位错开呈三角起伏; 每条约整帧
     * ~900ms 完成一次完整升降 (周期 >= 800ms, 满足「跳动频率放慢, 周期 >= 800ms/条」)。
     * a = 三角波 0..127..0 映射高 4..13。 */
    for (int i = 0; i < 6; i++) {
        int32_t a, h;
        if (g_kidpal_wave_bar[i] == NULL) {
            continue;
        }
        a = (v + i * 43) & 255; /* 相位错开; 0..255 */
        if (a > 127) {
            a = 256 - a;       /* 三角折返 => 每过多 ~1 帧 v 走完一次上下 */
        }
        h = 4 + (a * 9) / 127;
        lv_obj_set_height(g_kidpal_wave_bar[i], (lv_coord_t)h);
    }
}

/* 1bpp 资产 -> 堆 ARGB 缓冲 + 填充 image dsc (返回的 px 记入 *pxOut 供长期保留)。 */
static void kidpal_voice_build_art_img(const char* asset, lv_image_dsc_t* dsc,
                                       uint32_t** pxOut)
{
    kidpal_art_img_t img;
    uint32_t* px;

    if (asset == NULL || dsc == NULL || pxOut == NULL) {
        return;
    }
    if (kidpal_art_load(asset, &img) != 0 || img.w <= 0 || img.h <= 0) {
        return;
    }
    px = (uint32_t*)lv_malloc((size_t)img.w * img.h * sizeof(uint32_t));
    if (px == NULL) {
        return;
    }
    for (int y = 0; y < img.h; y++) {
        for (int x = 0; x < img.w; x++) {
            int p = kidpal_art_pixel(&img, x, y);
            px[y * img.w + x] = (p == 1) ? 0xFF000000u : 0x00000000u;
        }
    }
    memset(dsc, 0, sizeof(*dsc));
    dsc->header.cf     = LV_COLOR_FORMAT_ARGB8888;
    dsc->header.w      = (uint32_t)img.w;
    dsc->header.h      = (uint32_t)img.h;
    dsc->header.stride = (uint32_t)img.w * 4u;
    dsc->data          = (const uint8_t*)px;
    dsc->data_size     = (uint32_t)(img.w * img.h * 4u);
    *pxOut = px;
}

static void kidpal_voice_build_emblem_locked(void)
{
    /* 气泡尾巴 (r3-B): 标头气泡背景, 三标头态随 look.bubble 显隐 (见 apply_look)。 */
    if (g_kidpal_bubble_px == NULL) {
        kidpal_voice_build_art_img(g_kidpal_bubble_tail_asset, &g_kidpal_bubble_dsc,
                                   &g_kidpal_bubble_px);
        if (g_kidpal_bubble_px != NULL && g_kidpal_voice_page != NULL) {
            g_kidpal_bubble_img = lv_image_create(g_kidpal_voice_page);
            lv_image_set_src(g_kidpal_bubble_img, &g_kidpal_bubble_dsc);
            lv_obj_set_pos(g_kidpal_bubble_img, 102, 12); /* 标头左缘气泡尾, 观感归真机轮 */
            lv_obj_add_flag(g_kidpal_bubble_img, LV_OBJ_FLAG_HIDDEN);
        }
    }
    /* 麦克风 (C1, 聆听态; r5 精修: 依红线②撤 r4 的 lv_image_set_scale 运行时缩放, 改用
     * gen_voice_chip16.mjs 重出的 16px 锐利 1bpp 资产, 原尺寸挂「我在听！」标头文字后).
     * 标头位 = g_kidpal_voice_header_label (124,16) 起, 左对齐「我在听！」连字 4 格 ~56px
     * 后接图标 (x≈188)。16px 资产对象自然尺寸 = 16x16, lv_image 默认对齐即按对象左上摆放,
     * 无需收紧 widget/显式 TOP_LEFT, 确定性落位。观感归真机轮。 */
    if (g_kidpal_mic_px == NULL) {
        kidpal_voice_build_art_img(g_kidpal_mic_asset, &g_kidpal_mic_dsc,
                                   &g_kidpal_mic_px);
        if (g_kidpal_mic_px != NULL && g_kidpal_voice_page != NULL) {
            g_kidpal_mic_img = lv_image_create(g_kidpal_voice_page);
            lv_image_set_src(g_kidpal_mic_img, &g_kidpal_mic_dsc);
            lv_obj_set_pos(g_kidpal_mic_img, 188, 15);
            lv_obj_add_flag(g_kidpal_mic_img, LV_OBJ_FLAG_HIDDEN);
        }
    }
    /* 小问号 (C2, 思考态; r5 精修: 同红线②撤 r4 运行时缩放, 用 16px 锐利 1bpp 资产
     * question_16.bin 原尺寸挂「思考中」标头文字后)。标头位起 (124,16), 「思考中」连字
     * 3 格 ~42px 后接 (x≈172)。同 mic: 16px 资产原尺寸 + 默认对齐确定落位。观感归真机轮。 */
    if (g_kidpal_q_px == NULL) {
        kidpal_voice_build_art_img(g_kidpal_question_asset, &g_kidpal_q_dsc,
                                   &g_kidpal_q_px);
        if (g_kidpal_q_px != NULL && g_kidpal_voice_page != NULL) {
            g_kidpal_q_img[0] = lv_image_create(g_kidpal_voice_page);
            lv_image_set_src(g_kidpal_q_img[0], &g_kidpal_q_dsc);
            lv_obj_set_pos(g_kidpal_q_img[0], 172, 15);
            lv_obj_add_flag(g_kidpal_q_img[0], LV_OBJ_FLAG_HIDDEN);
        }
    }
    /* 声波条 (C3, 播报态, LVGL 原生矩形 + lv_anim; r4 精修: 撤大撤快——一小组小波浪条
     * 挂「播报」标头文字后, 组宽 ~18px ≈ 标头一行字高量级 (font_puhui_14_1 行高 ~18px),
     * 跳动周期放缓 >=800ms。动画定值见 lv_anim_set_duration (900ms/整帧, 每条约一次完整
     * 升降; 见 apply_look TALK 启停)。「播报」连字 2 格 ~28px 后接 (x≈158)。观感归真机轮。 */
    if (g_kidpal_wave_box == NULL && g_kidpal_voice_page != NULL) {
        g_kidpal_wave_box = lv_obj_create(g_kidpal_voice_page);
        lv_obj_remove_style_all(g_kidpal_wave_box);
        lv_obj_set_size(g_kidpal_wave_box, 18, 18);
        lv_obj_set_pos(g_kidpal_wave_box, 158, 15);
        lv_obj_set_style_bg_opa(g_kidpal_wave_box, LV_OPA_TRANSP, 0);
        lv_obj_clear_flag(g_kidpal_wave_box, LV_OBJ_FLAG_SCROLLABLE);
        for (int i = 0; i < 4; i++) {
            g_kidpal_wave_bar[i] = lv_obj_create(g_kidpal_wave_box);
            lv_obj_remove_style_all(g_kidpal_wave_bar[i]);
            lv_obj_set_size(g_kidpal_wave_bar[i], 3, 6);
            lv_obj_set_pos(g_kidpal_wave_bar[i], i * 5, 0);
            lv_obj_set_style_radius(g_kidpal_wave_bar[i], 1, 0);
            lv_obj_set_style_bg_color(g_kidpal_wave_bar[i], lv_color_hex(0x000000), 0);
            lv_obj_set_style_bg_opa(g_kidpal_wave_bar[i], LV_OPA_COVER, 0);
        }
        lv_obj_add_flag(g_kidpal_wave_box, LV_OBJ_FLAG_HIDDEN);
    }
    g_kidpal_emblem_built = 1;
}

static void kidpal_voice_emblem_apply_locked(int scene)
{
    int want_mic = 0, want_q = 0, want_wave = 0;

    if (!g_kidpal_emblem_built) {
        kidpal_voice_build_emblem_locked();
    }
    switch (scene) {
    case KVST_SCENE_LISTEN:
        want_mic = 1;
        break;
    case KVST_SCENE_THINK:
        want_q = 1;
        break;
    case KVST_SCENE_TALK:
        want_wave = 1;
        break;
    default:
        break;
    }
    if (g_kidpal_mic_img != NULL) {
        if (want_mic) {
            lv_obj_remove_flag(g_kidpal_mic_img, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(g_kidpal_mic_img, LV_OBJ_FLAG_HIDDEN);
        }
    }
    for (int i = 0; i < 1; i++) {
        if (g_kidpal_q_img[i] != NULL) {
            if (want_q) {
                lv_obj_remove_flag(g_kidpal_q_img[i], LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_add_flag(g_kidpal_q_img[i], LV_OBJ_FLAG_HIDDEN);
            }
        }
    }
    if (g_kidpal_wave_box != NULL) {
        if (want_wave) {
            lv_obj_remove_flag(g_kidpal_wave_box, LV_OBJ_FLAG_HIDDEN);
            if (!g_kidpal_wave_an_running) {
                lv_anim_t a;
                lv_anim_init(&a);
                lv_anim_set_var(&a, &g_kidpal_wave_phase);
                lv_anim_set_values(&a, 0, 255);
                lv_anim_set_duration(&a, 900); /* r4 定值: 整帧 900ms, 每条约 ~900ms 一次完整升降 (周期 >= 800ms) */
                lv_anim_set_path_cb(&a, lv_anim_path_linear);
                lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
                lv_anim_set_exec_cb(&a, kidpal_voice_wave_anim_cb);
                lv_anim_start(&a);
                g_kidpal_wave_an_running = 1;
            }
        } else {
            lv_obj_add_flag(g_kidpal_wave_box, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

/* -------------------------------------------------------------------------- *
 * chrome 中部显隐纪律 (RP-A5 r2 真机轮: 语音猫与笑脸叠罗汉 / 笑脸悬浮每一页)。
 * PKG-2 归一屏下聊天 chrome 常驻 lv_layer_top、恒浮于活动屏之上, 层栈只裁决前台
 * 屏——姿态与笑脸的显隐必须由内容面自管:
 *   mode=1 五态演出活跃 (LISTENING/SPEAKING): 笑脸 (label+image) 隐 —— 姿态顶替
 *          笑脸位, 二者不得同框; 姿态显示还需 g_kidpal_voice_front=1 (VOICE 层
 *          前台), 内容页 (答案/提醒/浏览) 前台时演出进行中姿态也不浮出;
 *   mode=2 idle (STANDBY): 姿态与笑脸都隐 —— chrome 中部无任何图形, 页面干净;
 *   mode=0 未接管: boot/connecting 等, SetEmotion 自管不动。
 * 键帽条与聊天文本 (chat_message_label_) 任何状态不动 (V3 §06 / PKG-2 语义)。
 * 语音轮整页版式 (VQ-A7 r1 修复面③): 页面容器随 mode=1+前台 显隐, 回显区/问答框
 * 在其内按场景互斥 (LISTEN/THINK 回显 / TALK 问答) —— 显隐只切 LV_OBJ_FLAG_HIDDEN,
 * 对象与缓冲复用不重建 (round 1 泄漏纪律不破)。
 * 调用方必须已持 LVGL 锁。
 * ------------------------------------------------------------------------ */
static void kidpal_chrome_mid_sync_locked(void)
{
    /* 提醒材料壳（K4 overlay-unify）：提醒 = 最高层覆盖, 其外壳 = 本骨架整页 —— REMIND
     * 态下即使 VOICE 非前台（REMINDER 层置顶）也要点亮整页外壳 + 小伴 pose; 关窗后
     * front_is_reminder() 变 0 -> 自动回落不残留跨页。判据取 render_node 层 top（与语音
     * 前台桥同源, 零第二条真理）。顶置计算供下方 pose 与 voice_page 两块共用。 */
    const int reminder_shell =
        (g_kidpal_mid_mode == 1 && g_kidpal_keybar_scene == KVST_SCENE_REMIND &&
         kidpal_render_node_front_is_reminder()) ? 1 : 0;

    if (g_kidpal_mid_mode == 1) {
        if (g_kidpal_emoji_label != NULL) {
            lv_obj_add_flag(g_kidpal_emoji_label, LV_OBJ_FLAG_HIDDEN);
        }
        if (g_kidpal_emoji_image != NULL) {
            lv_obj_add_flag(g_kidpal_emoji_image, LV_OBJ_FLAG_HIDDEN);
        }
        /* 姿态显示需 五态演出活跃 (mode=1) + （VOICE 前台 或 提醒材料壳点亮）。
         * 内容页前台时演出中也不得浮出 (RP-A5 r3「课表答案页显示时猫咪在中间」);
         * 提醒材料壳点亮时 pose-jump 必须随外壳浮出 (WF-B-04 左小伴)。 */
        if (g_kidpal_pose_img != NULL) {
            if (g_kidpal_voice_front || reminder_shell) {
                lv_obj_remove_flag(g_kidpal_pose_img, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_add_flag(g_kidpal_pose_img, LV_OBJ_FLAG_HIDDEN);
            }
        }
    } else if (g_kidpal_mid_mode == 2) {
        if (g_kidpal_emoji_label != NULL) {
            lv_obj_add_flag(g_kidpal_emoji_label, LV_OBJ_FLAG_HIDDEN);
        }
        if (g_kidpal_emoji_image != NULL) {
            lv_obj_add_flag(g_kidpal_emoji_image, LV_OBJ_FLAG_HIDDEN);
        }
        if (g_kidpal_pose_img != NULL) {
            lv_obj_add_flag(g_kidpal_pose_img, LV_OBJ_FLAG_HIDDEN);
        }
    }

    /* 语音轮整页版式显隐 (device-round VQ-A7 r1 修复面③; 与姿态同两条件与):
     * 页面容器 = 演出活跃且 VOICE 前台时整页盖住首页内容 (WF-B 冻结线框; 首页猫
     * 遮蔽销账 = 整页盖住形态, 显隐只切 HIDDEN 零重建)。回显区/问答框按场景互斥:
     * LISTEN/THINK -> 回显区 (WF-B-01/02), TALK -> 问答框 (WF-B-03); 提醒态 -> 问答框
     * 承载大字事项+时间 (WF-B-04)。场景值取 g_kidpal_keybar_scene (apply_look 刷新)。 */
    {
        int voice_live = (g_kidpal_mid_mode == 1 &&
                          (g_kidpal_voice_front || reminder_shell)) ? 1 : 0;
        int show_echo = (voice_live &&
                         (g_kidpal_keybar_scene == KVST_SCENE_LISTEN ||
                          g_kidpal_keybar_scene == KVST_SCENE_THINK)) ? 1 : 0;
        int show_answer = (voice_live &&
                           (g_kidpal_keybar_scene == KVST_SCENE_TALK ||
                            g_kidpal_keybar_scene == KVST_SCENE_REMIND)) ? 1 : 0;
        if (g_kidpal_voice_page != NULL) {
            if (voice_live) {
                lv_obj_remove_flag(g_kidpal_voice_page, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_add_flag(g_kidpal_voice_page, LV_OBJ_FLAG_HIDDEN);
            }
        }
        if (g_kidpal_voice_echo_box != NULL) {
            if (show_echo) {
                lv_obj_remove_flag(g_kidpal_voice_echo_box, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_add_flag(g_kidpal_voice_echo_box, LV_OBJ_FLAG_HIDDEN);
            }
        }
        if (g_kidpal_voice_answer_box != NULL) {
            if (show_answer) {
                lv_obj_remove_flag(g_kidpal_voice_answer_box, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_add_flag(g_kidpal_voice_answer_box, LV_OBJ_FLAG_HIDDEN);
            }
        }
    }
}

void LcdDisplay::SetStatus(const char* status) {
    /* 先走基类: 更新 status bar 文本 + 计时 (既有语义不动)。 */
    LvglDisplay::SetStatus(status);

    /* 设备态 -> 语音轮场景 (播报态还经 SetChatMessage 充填同步文字框)。
     * STANDBY/CONNECTING 不属五态演出, 不覆盖气息, 只保留键帽条现值。 */
    if (status != NULL) {
        if (strcmp(status, Lang::Strings::LISTENING) == 0) {
            kidpal_voice_apply_look(KVST_SCENE_LISTEN);
        } else if (strcmp(status, Lang::Strings::THINKING) == 0) {
            /* 思考页首接活侧渲染路径 (VQ-A1, 现状零调用销账): stt 分支同刻消费场景单源
             * 后经 SetStatus(THINKING) 驱动 apply_look(KVST_SCENE_THINK) —— 状态切换
             * 与页面弹出同一时刻, requirements §2.2 思考中行。 */
            kidpal_voice_apply_look(KVST_SCENE_THINK);
        } else if (strcmp(status, Lang::Strings::SPEAKING) == 0) {
            kidpal_voice_apply_look(KVST_SCENE_TALK);
        } else if (strcmp(status, Lang::Strings::STANDBY) == 0) {
            /* idle 进入: chrome 中部清空 —— 姿态与笑脸都隐 (复用既有对象,
             * 显隐切换零重建), 键帽条/聊天文本留屏 (RP-A5 r2)。基类计时语义
             * 已先完成, 此处自取自放锁, 无嵌套。 */
            g_kidpal_mid_mode = 2;
            if (lvgl_port_lock(30000)) {
                kidpal_chrome_mid_sync_locked();
                lvgl_port_unlock();
            }
        }
    }
}

void LcdDisplay::SetEmotion(const char* emotion) {
    // Stop any running GIF animation
    if (gif_controller_) {
        DisplayLockGuard lock(this);
        gif_controller_->Stop();
        gif_controller_.reset();
    }
    
    if (emoji_image_ == nullptr) {
        return;
    }

    auto emoji_collection = static_cast<LvglTheme*>(current_theme_)->emoji_collection();
    auto image = emoji_collection != nullptr ? emoji_collection->GetEmojiImage(emotion) : nullptr;
    if (image == nullptr) {
        const char* utf8 = font_awesome_get_utf8(emotion);
        if (utf8 != nullptr && emoji_label_ != nullptr) {
            DisplayLockGuard lock(this);
            lv_label_set_text(emoji_label_, utf8);
            lv_obj_add_flag(emoji_image_, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(emoji_label_, LV_OBJ_FLAG_HIDDEN);
            /* 中部显隐纪律重断言 (RP-A5 r2): 演出活跃/idle 期笑脸不得复现。 */
            if (g_kidpal_mid_mode != 0) {
                kidpal_chrome_mid_sync_locked();
            }
        }
        return;
    }

    DisplayLockGuard lock(this);
    if (image->IsGif()) {
        // Create new GIF controller
        gif_controller_ = std::make_unique<LvglGif>(image->image_dsc());
        
        if (gif_controller_->IsLoaded()) {
            // Set up frame update callback
            gif_controller_->SetFrameCallback([this]() {
                lv_image_set_src(emoji_image_, gif_controller_->image_dsc());
            });
            
            // Set initial frame and start animation
            lv_image_set_src(emoji_image_, gif_controller_->image_dsc());
            gif_controller_->Start();
            
            // Show GIF, hide others
            lv_obj_add_flag(emoji_label_, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(emoji_image_, LV_OBJ_FLAG_HIDDEN);
        } else {
            ESP_LOGE(TAG, "Failed to load GIF for emotion: %s", emotion);
            gif_controller_.reset();
        }
    } else {
        lv_image_set_src(emoji_image_, image->image_dsc());
        lv_obj_add_flag(emoji_label_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(emoji_image_, LV_OBJ_FLAG_HIDDEN);
    }

#if CONFIG_USE_WECHAT_MESSAGE_STYLE
    // In WeChat message style, if emotion is neutral, don't display it
    uint32_t child_count = lv_obj_get_child_cnt(content_);
    if (strcmp(emotion, "neutral") == 0 && child_count > 0) {
        // Stop GIF animation if running
        if (gif_controller_) {
            gif_controller_->Stop();
            gif_controller_.reset();
        }
        
        lv_obj_add_flag(emoji_image_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(emoji_label_, LV_OBJ_FLAG_HIDDEN);
    }
#endif

    /* chrome 中部显隐纪律 (RP-A5 r2): 状态机先 SetStatus 后 SetEmotion —— 五态
     * 演出活跃/idle 期笑脸不得借 SetEmotion 复现; 在其自管逻辑之后重断言显隐。 */
    if (g_kidpal_mid_mode != 0) {
        kidpal_chrome_mid_sync_locked();
    }
}

void LcdDisplay::SetTheme(Theme* theme) {
    DisplayLockGuard lock(this);
    
    auto lvgl_theme = static_cast<LvglTheme*>(theme);
    
    // Get the active screen
    lv_obj_t* screen = lv_screen_active();

    // Set font
    auto text_font = lvgl_theme->text_font()->font();
    auto icon_font = lvgl_theme->icon_font()->font();
    auto large_icon_font = lvgl_theme->large_icon_font()->font();

    if (text_font->line_height >= 40) {
        lv_obj_set_style_text_font(mute_label_, large_icon_font, 0);
        lv_obj_set_style_text_font(network_label_, large_icon_font, 0);
    } else {
        lv_obj_set_style_text_font(mute_label_, icon_font, 0);
        lv_obj_set_style_text_font(network_label_, icon_font, 0);
    }

    // Set parent text color
    lv_obj_set_style_text_font(screen, text_font, 0);
    lv_obj_set_style_text_color(screen, lvgl_theme->text_color(), 0);

    // Set background image
    if (lvgl_theme->background_image() != nullptr) {
        lv_obj_set_style_bg_image_src(container_, lvgl_theme->background_image()->image_dsc(), 0);
    } else {
        lv_obj_set_style_bg_image_src(container_, nullptr, 0);
        lv_obj_set_style_bg_color(container_, lvgl_theme->background_color(), 0);
    }
    
    // Update top bar background color with 50% opacity
    if (top_bar_ != nullptr) {
        lv_obj_set_style_bg_opa(top_bar_, LV_OPA_50, 0);
        lv_obj_set_style_bg_color(top_bar_, lvgl_theme->background_color(), 0);
    }
    
    // Update status bar elements
    lv_obj_set_style_text_color(network_label_, lvgl_theme->text_color(), 0);
    lv_obj_set_style_text_color(status_label_, lvgl_theme->text_color(), 0);
    lv_obj_set_style_text_color(notification_label_, lvgl_theme->text_color(), 0);
    lv_obj_set_style_text_color(mute_label_, lvgl_theme->text_color(), 0);
    lv_obj_set_style_text_color(emoji_label_, lvgl_theme->text_color(), 0);

    // If we have the chat message style, update all message bubbles
#if CONFIG_USE_WECHAT_MESSAGE_STYLE
    // Set content background opacity
    lv_obj_set_style_bg_opa(content_, LV_OPA_TRANSP, 0);

    // Iterate through all children of content (message containers or bubbles)
    uint32_t child_count = lv_obj_get_child_cnt(content_);
    for (uint32_t i = 0; i < child_count; i++) {
        lv_obj_t* obj = lv_obj_get_child(content_, i);
        if (obj == nullptr) continue;
        
        lv_obj_t* bubble = nullptr;
        
        // Check if this object is a container or bubble
        // If it's a container (user or system message), get its child as bubble
        // If it's a bubble (assistant message), use it directly
        if (lv_obj_get_child_cnt(obj) > 0) {
            // Might be a container, check if it's a user or system message container
            // User and system message containers are transparent
            lv_opa_t bg_opa = lv_obj_get_style_bg_opa(obj, 0);
            if (bg_opa == LV_OPA_TRANSP) {
                // This is a user or system message container
                bubble = lv_obj_get_child(obj, 0);
            } else {
                // This might be an assistant message bubble itself
                bubble = obj;
            }
        } else {
            // No child elements, might be other UI elements, skip
            continue;
        }
        
        if (bubble == nullptr) continue;
        
        // Use saved user data to identify bubble type
        void* bubble_type_ptr = lv_obj_get_user_data(bubble);
        if (bubble_type_ptr != nullptr) {
            const char* bubble_type = static_cast<const char*>(bubble_type_ptr);
            
            // Apply correct color based on bubble type
            if (strcmp(bubble_type, "user") == 0) {
                lv_obj_set_style_bg_color(bubble, lvgl_theme->user_bubble_color(), 0);
            } else if (strcmp(bubble_type, "assistant") == 0) {
                lv_obj_set_style_bg_color(bubble, lvgl_theme->assistant_bubble_color(), 0); 
            } else if (strcmp(bubble_type, "system") == 0) {
                lv_obj_set_style_bg_color(bubble, lvgl_theme->system_bubble_color(), 0);
            } else if (strcmp(bubble_type, "image") == 0) {
                lv_obj_set_style_bg_color(bubble, lvgl_theme->system_bubble_color(), 0);
            }
            
            // Update border color
            lv_obj_set_style_border_color(bubble, lvgl_theme->border_color(), 0);
            
            // Update text color for the message
            if (lv_obj_get_child_cnt(bubble) > 0) {
                lv_obj_t* text = lv_obj_get_child(bubble, 0);
                if (text != nullptr) {
                    // Set text color based on bubble type
                    if (strcmp(bubble_type, "system") == 0) {
                        lv_obj_set_style_text_color(text, lvgl_theme->system_text_color(), 0);
                    } else {
                        lv_obj_set_style_text_color(text, lvgl_theme->text_color(), 0);
                    }
                }
            }
        } else {
            ESP_LOGW(TAG, "child[%lu] Bubble type is not found", i);
        }
    }
#else
    // Simple UI mode - just update the main chat message
    if (chat_message_label_ != nullptr) {
        lv_obj_set_style_text_color(chat_message_label_, lvgl_theme->text_color(), 0);
    }
    
    if (emoji_label_ != nullptr) {
        lv_obj_set_style_text_color(emoji_label_, lvgl_theme->text_color(), 0);
    }
    
    // Update bottom bar background color with 50% opacity
    if (bottom_bar_ != nullptr) {
        lv_obj_set_style_bg_opa(bottom_bar_, LV_OPA_50, 0);
        lv_obj_set_style_bg_color(bottom_bar_, lvgl_theme->background_color(), 0);
    }
#endif
    
    // Update low battery popup
    lv_obj_set_style_bg_color(low_battery_popup_, lvgl_theme->low_battery_color(), 0);

    // No errors occurred. Save theme to settings
    Display::SetTheme(lvgl_theme);
}

void LcdDisplay::SetHideSubtitle(bool hide) {
    DisplayLockGuard lock(this);
    hide_subtitle_ = hide;

    // Immediately update UI visibility based on the setting
    if (bottom_bar_ != nullptr) {
        if (hide) {
            lv_obj_add_flag(bottom_bar_, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_remove_flag(bottom_bar_, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void LcdDisplay::KidPalSetNetworkLabelFont(const lv_font_t* font) {
    if (font == nullptr) {
        return;
    }
    DisplayLockGuard lock(this);
    if (network_label_ != nullptr) {
        lv_obj_set_style_text_font(network_label_, font, 0);
    }
}

void LcdDisplay::KidPalReparentChatToLayer(lv_obj_t* parent) {
    DisplayLockGuard lock(this);
    if (parent == nullptr) {
        return;
    }

    /* 归一屏（包 260911-screen-unification SU-RULING-02 单物理屏）：
     * SetupUI 把聊天 chrome 建在 LVGL 默认首屏上。把「状态栏 / 表情 / 气泡 / 预览图 /
     * 低电量弹窗」这些顶层聊天控件迁到常驻 parent（top layer，非屏），使聊天内容任何页
     * 可见、且页面渲染的 lv_obj_clean(lv_screen_active()) 绝不触碰它们（根除 clean 聊天
     * 屏致 status_label_ 悬空同族崩溃）。container_ 是整屏不透明背景，留在原默认屏——
     * 该屏此后不再被加载、不再渲染，KidPal 页从底下透出。
     *
     * 只迁顶层控件子树：表情的 emoji_label_/emoji_image_ 在非 WeChat 布局下挂在
     * emoji_box_ 下，随 emoji_box_ 一并迁移，不单独重挂（保持其居中坐标语义）。 */
    lv_obj_t* chat_top[] = {
        top_bar_,
        status_bar_,
        bottom_bar_,
        emoji_box_,
        preview_image_,
        low_battery_popup_,
    };
    for (lv_obj_t* obj : chat_top) {
        if (obj != nullptr && lv_obj_get_parent(obj) != parent) {
            lv_obj_set_parent(obj, parent);
        }
    }
}

void LcdDisplay::KidPalVoiceChromeFront(bool voice_front) {
    /* render_node 前台切换点调用, 调用方已持 lvgl 锁 (递归互斥量, 此处重取无害;
     * 亦兼容未来未持锁调用方)。只翻前台维度标志并重断言中部显隐 —— 「armed &&
     * front」两条件与: SetStatus(LISTENING) 在语音轮中反复触发时, apply_look 的
     * sync 也不会在内容页前台把姿态重新显示出来。键帽条/聊天文本不动。 */
    g_kidpal_voice_front = voice_front ? 1 : 0;
    if (lvgl_port_lock(30000)) {
        kidpal_chrome_mid_sync_locked();
        kidpal_keybar_sync_locked();
        lvgl_port_unlock();
    }
}

/* C 桥 (RP-A5 r4): render_node (C++ 但不同编译单元) 经本接口驱动前台维度显隐。
 * 取 display 走 SetupUI 登记的 self 指针, NULL 守卫 (未构造/非 KidPal 板) 直接
 * 返回 —— 绝不经 Board::GetInstance() (boot 期 render_node_init 跑在 CustomBoard
 * 构造期内, 函数内 static 单例重入 __cxa_guard 永久阻塞 = r3 真机砖机根因)。 */
extern "C" void kidpal_lcd_voice_chrome_front(int front)
{
    if (s_kidpal_lcd_self == NULL) {
        return;
    }
    s_kidpal_lcd_self->KidPalVoiceChromeFront(front != 0);
}

/* -------------------------------------------------------------------------- *
 * 提醒弹窗材料壳 (K4 overlay-unify, RM-A1/A2/A3): 复用语音轮四态骨架（RM-E3 反 fork）
 * —— pose-jump + 键帽「知道了」从 K2 闭集单源取出 (kidpal_voice_state_look，经
 * apply_look 落键帽条/姿态同对象)；右内容区承载大字事项(title)+时间行(kidpal_reminder_
 * hhmm 单源, 渲染面不重写第二份时间解析)。提醒 = 最高层覆盖, 外壳显隐由 chrome 中部
 * 显隐纪律单点驱动 (kidpal_chrome_mid_sync_locked 依 REMIND 场景 + front_is_reminder
 * 点亮; 关窗自动回落)。mcp_server.cc RaiseReminderQueue 经本 C 桥消费（取代旧 v1.13
 * 卡面 kidpal_reminder_page_render）。返回 1 = 已画成, -1 = 降级（NULL/不可读, 不崩）。
 * ------------------------------------------------------------------------ */
extern "C" int kidpal_lcd_reminder_show(const kidpal_alert_t *a)
{
    char time_line[8];

    if (s_kidpal_lcd_self == NULL || a == NULL || a->id[0] == '\0') {
        return -1;
    }
    if (kidpal_reminder_hhmm(a->fire_at, time_line, sizeof(time_line)) < 0) {
        return -1; /* 时间行单源不可组（合法路径不达）-> 降级不盲渲 */
    }

    /* 四态骨架单源消费: pose-jump + 键帽「知道了」（闭集, apply_look 内部自取放锁）。 */
    kidpal_voice_apply_look(KVST_SCENE_REMIND);

    if (!lvgl_port_lock(30000)) {
        return -1;
    }
    /* 右内容区: 标头 = 时间行, 主体 = 大字事项。全部落在既建骨架对象上, 零第二渲染源。 */
    if (g_kidpal_voice_header_label != NULL) {
        lv_label_set_text(g_kidpal_voice_header_label, time_line);
    }
    if (g_kidpal_voice_answer_label != NULL) {
        lv_label_set_text(g_kidpal_voice_answer_label, a->title);
    }
    if (g_kidpal_voice_answer_box != NULL) {
        lv_obj_remove_flag(g_kidpal_voice_answer_box, LV_OBJ_FLAG_HIDDEN);
        lv_obj_scroll_to_y(g_kidpal_voice_answer_box, 0, LV_ANIM_OFF);
    }
    kidpal_chrome_mid_sync_locked();
    lvgl_port_unlock();
    return 1;
}

void LcdDisplay::KidPalKeybarSync() {
    /* 键帽条三槽随态刷新 (keybar R1 / ①④根修): render_node 页渲染权威每页落屏后调用,
     * 驱动基础页槽位 + 页点显隐 + 覆盖层随态文案。内部递归取锁 (render_node 调用点已持
     * lvgl 锁), NULL/锁失败静默返回 —— 不改变渲染节点任何状态, 零副作用。 */
    if (lvgl_port_lock(30000)) {
        kidpal_keybar_sync_locked();
        lvgl_port_unlock();
    }
}

/* C 桥 (keybar R1 / ①④根修): render_node (C++ 但不同编译单元) 在基础页渲染权威点
 * (rn_render_home_locked / rn_render_kidpal_page_locked 每页落屏后) 经本接口驱动键帽条
 * 三槽刷新 —— 与键路由/页循环同源, 不依赖语音态/前台桥。self 守卫同 voce chrome 桥
 * (boot 期 render_node_init 跑在构造期内, 绝不经 Board::GetInstance())。 */
extern "C" void kidpal_lcd_keybar_sync(void)
{
    if (s_kidpal_lcd_self == NULL) {
        return;
    }
    s_kidpal_lcd_self->KidPalKeybarSync();
}
