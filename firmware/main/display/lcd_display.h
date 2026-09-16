#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include "lvgl_display.h"
#include "gif/lvgl_gif.h"

#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <font_emoji.h>

#include <atomic>
#include <memory>

#define PREVIEW_IMAGE_DURATION_MS 5000


class LcdDisplay : public LvglDisplay {
protected:
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;
    
    lv_draw_buf_t draw_buf_;
    lv_obj_t* top_bar_ = nullptr;
    lv_obj_t* status_bar_ = nullptr;
    lv_obj_t* content_ = nullptr;
    lv_obj_t* container_ = nullptr;
    lv_obj_t* side_bar_ = nullptr;
    lv_obj_t* bottom_bar_ = nullptr;
    lv_obj_t* preview_image_ = nullptr;
    lv_obj_t* emoji_label_ = nullptr;
    lv_obj_t* emoji_image_ = nullptr;
    std::unique_ptr<LvglGif> gif_controller_ = nullptr;
    lv_obj_t* emoji_box_ = nullptr;
    lv_obj_t* chat_message_label_ = nullptr;
    esp_timer_handle_t preview_timer_ = nullptr;
    std::unique_ptr<LvglImage> preview_image_cached_ = nullptr;
    bool hide_subtitle_ = false;  // Control whether to hide chat messages/subtitles

    void InitializeLcdThemes();
    void SetupUI();
    virtual bool Lock(int timeout_ms = 0) override;
    virtual void Unlock() override;

protected:
    // Add protected constructor
    LcdDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel, int width, int height);
    
public:
    ~LcdDisplay();
    virtual void SetEmotion(const char* emotion) override;
    virtual void SetChatMessage(const char* role, const char* content) override;
    virtual void SetPreviewImage(std::unique_ptr<LvglImage> image) override;
    /* 五态演出 (包 260911-render-pages RP-A9): 重载 SetStatus 把设备态 (聆听/播报)
     * 映射为语音轮场景, 经 kidpal_voice_state_look 驱动姿态 + 键帽条 + 气泡。additive,
     * 显式调基类并追加五态内容侧面。 */
    virtual void SetStatus(const char* status) override;

    // Add theme switching function
    virtual void SetTheme(Theme* theme) override;

    // Set whether to hide chat messages/subtitles
    void SetHideSubtitle(bool hide);

    /* 板类把 chrome 联网小标（network_label_）切到指定字体（包 260915-m1-n1b-p1-home-battery
     * 任务 B：waveshare-s3-rlcd-4.2 首页联网小标与温/湿/电辅助条小标同档）。基座默认按
     * 主题 icon_font 渲染；板类在构造期调用一次把联网小标定为小号 FA 字体，配合 override
     * SetTheme（自定义层）在主题刷新后维持不跳档。 */
    void KidPalSetNetworkLabelFont(const lv_font_t* font);

    // KidPal 归一屏（包 260911-screen-unification SU-RULING-02 单物理屏）：
    // 把 SetupUI 建立在 LVGL 默认屏上的聊天 chrome（状态栏 / 表情 / 气泡 / 低电量弹窗）
    // 迁到常驻 parent（调用方传 lv_layer_top()，恒浮于活动屏之上、非屏）。整屏不透明
    // 背景 container_ 留在原默认屏（该屏此后不再被加载、不再渲染），让 KidPal 页透出、
    // 聊天内容任何页可见。控件指针不失效（lv_obj_set_parent 只改树不销毁对象）。
    void KidPalReparentChatToLayer(lv_obj_t* parent);

    /* 五态 chrome 显隐的前台维度 (RP-A5 r3): VOICE 层为前台层时姿态/气泡可见,
     * 任何内容页 (SERVER_PAGE 答案 / REMINDER / LOCAL_PAGE 浏览) 前台时隐藏。
     * kidpal_render_node 在前台切换点调用 (调用方已持 lvgl 锁, 递归互斥量)。
     * 键帽条与聊天文本 (PKG-2 任何页可见语义) 不受此开关影响。 */
    void KidPalVoiceChromeFront(bool voice_front);

    /* 键帽条三槽刷新 (260915-m1-k2-keybar-three-slot R1): render_node 页渲染权威
     * (rn_render_home_locked / rn_render_kidpal_page_locked 每页落屏后) 经 C 桥
     * kidpal_lcd_keybar_sync() 驱动 —— 基础页槽位/页点/覆盖层显隐与键路由/页循环同源,
     * 不依赖语音态前台桥 (①/④ 根修: 基础页/启动后 keybar 必须落字与刷新)。内部自取
     * lvgl 锁 (递归), 调用方无需预加锁。 */
    void KidPalKeybarSync();
};

// SPI LCD display
class SpiLcdDisplay : public LcdDisplay {
public:
    SpiLcdDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
                  int width, int height, int offset_x, int offset_y,
                  bool mirror_x, bool mirror_y, bool swap_xy);
};

// RGB LCD display
class RgbLcdDisplay : public LcdDisplay {
public:
    RgbLcdDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
                  int width, int height, int offset_x, int offset_y,
                  bool mirror_x, bool mirror_y, bool swap_xy);
};

// MIPI LCD display
class MipiLcdDisplay : public LcdDisplay {
public:
    MipiLcdDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
                   int width, int height, int offset_x, int offset_y,
                   bool mirror_x, bool mirror_y, bool swap_xy);
};

#endif // LCD_DISPLAY_H
