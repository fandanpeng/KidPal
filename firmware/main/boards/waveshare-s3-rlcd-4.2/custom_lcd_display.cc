#include <vector>
#include <cstring>
#include <freertos/FreeRTOS.h>
#include <esp_lcd_panel_io.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_memory_utils.h>
#include "custom_lcd_display.h"
#include "lcd_display.h"
#include "esp_lvgl_port.h"
#include "assets/lang_config.h"
#include "settings.h"
#include "config.h"
#include "board.h"
#include "font_size_test_page.h"
#include <lvgl.h> /* 包 260915-m1-n1b-p1-home-battery 任务 B: SetTheme override 落小号 FA 字体 */

LV_FONT_DECLARE(font_awesome_14_1); /* 联网小标与辅助条小标同档用 14px Font Awesome */

/* taskLVGL 任务栈定容（M2-K1 缺陷②修复的板级落点）：esp_lvgl_port 默认 7168 曾被
 * char_page 动画帧历史大栈数组（pts/sc/toks 256 点局部 ≈6.5KB）打穿、栈溢出 panic
 * （2026-09-15 串口实证两轮）。正式修复 = 帧路径预解析去栈（char_page.cc）+ 栈定容
 * 在此收编为命名常量作单源。定容 16384 = 预解析去栈后动画帧栈需求回落 + LVGL 渲染
 * 中间态与保底裕量的硬上界。 */
static const uint32_t KP_TASK_LVGL_STACK_SIZE = 16384;

void CustomLcdDisplay::SetTheme(Theme* theme) {
    /* 基座先把 chrome 按主题样式化（network_label_ 会按 text_font line_height 决策落到
     * icon_font = font_awesome_30_4，30px）；随后强制联网小标回落小号 FA 字体 14px，与
     * 温/湿/电辅助条位图图标（~13px 墨高，home_p1 资产）同大小档。 */
    LcdDisplay::SetTheme(theme);
    DisplayLockGuard lock(this);
    if (network_label_ != nullptr) {
        lv_obj_set_style_text_font(network_label_, &font_awesome_14_1, 0);
    }
    /* M1-N1b 看屏终验（human「wifi 图片明显低了，其它也不整齐」）：chrome 顶带
     * (top_bar_, lv_set_flex cross-center) 只容 network_label_（左）+ mute_label_（右）
     * 两项。mute_label_ 仍走主题 icon_font（大号）会把 top_bar_ 撑高，cross-center 把
     * 联网小标高带放到辅助条中线之下。统一把两项压到同档 14px FA + 抬高 pad_top，
     * 使 top_bar_ 内容高=15、cross-center 落在辅助条中线 y≈19.5（换算：aux 中线在
     * 400x300 帧缓冲 y≈19.5，P1 帧 1:1 blit 到屏 (0,0)，network_label_ line_height 15
     * -> top=pad_top，center=pad_top+7.5=19.5 取 pad_top=12）。 */
    if (mute_label_ != nullptr) {
        lv_obj_set_style_text_font(mute_label_, &font_awesome_14_1, 0);
    }
    if (top_bar_ != nullptr) {
        lv_obj_set_style_pad_top(top_bar_, 12, 0);
        /* 根因修复（TECH-261012 承接, 候选 A 定案）: chrome 顶带 top_bar_ 原为 50%
         * 白蒙层, 经 KidPalReparentChatToLayer(lv_layer_top()) 恒浮于所有 KidPal 页;
         * 本板 1bpp flush 阈值（*buffer < 0x7fff -> 黑）× LVGL lv_color_16_16_mix
         * 混 50% 白把 KSY_COLOR_INK 灰 0x333333 漂白过阈（纯黑侥幸存活）——落
         * y[0..28] 的 P3 计数 ★/×N 真机整段不可见（S-F2.1 AC1 页身份载体缺失）。
         * 解耦修复: 背景透明化断根——顶带内容不再被蒙层漂白, 与落在本带的内容色值
         * 解耦（单一 choke point）, 免去「所有现在/将来落 y[0..28] 的 INK/TEXT 内容
         * 逐个跟改」的敞口义务。 */
        lv_obj_set_style_bg_opa(top_bar_, LV_OPA_TRANSP, 0);
    }
}

void CustomLcdDisplay::Lvgl_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    assert(disp != NULL);
    CustomLcdDisplay *Disp = (CustomLcdDisplay *)lv_display_get_user_data(disp);
    uint16_t *buffer = (uint16_t *)color_p;
  	for(int y = area->y1; y <= area->y2; y++)
  	{
  	 	for(int x = area->x1; x <= area->x2; x++) 
  	 	{
  	 	   	uint8_t color = (*buffer < 0x7fff) ? ColorBlack : ColorWhite;
  	 	   	Disp->RLCD_SetPixel(x,y,color);
  	 	   	buffer++;
  	 	}
  	}
  	Disp->RLCD_Display();
	lv_disp_flush_ready(disp);
}

CustomLcdDisplay::CustomLcdDisplay(esp_lcd_panel_io_handle_t panel_io,
esp_lcd_panel_handle_t panel,
int width, 
int height, 
int offset_x, 
int offset_y,
bool mirror_x, 
bool mirror_y, 
bool swap_xy,
spi_display_config_t spiconfig,
spi_host_device_t spi_host) : LcdDisplay(panel_io, panel, width, height),
mosi_(spiconfig.mosi),
scl_(spiconfig.scl), 
dc_(spiconfig.dc), 
cs_(spiconfig.cs), 
rst_(spiconfig.rst), 
width_(width), 
height_(height)
{
	ESP_LOGI(TAG, "Initialize SPI");
	esp_err_t        ret;
    spi_bus_config_t buscfg   = {};
    int              transfer = width_ * height_;
    buscfg.miso_io_num                   = -1;
    buscfg.mosi_io_num                   = mosi_;
    buscfg.sclk_io_num                   = scl_;
    buscfg.quadwp_io_num                 = -1;
    buscfg.quadhd_io_num                 = -1;
    buscfg.max_transfer_sz               = transfer;
    ret                                  = spi_bus_initialize(spi_host, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    esp_lcd_panel_io_spi_config_t io_config = {};
    io_config.dc_gpio_num = dc_;
    io_config.cs_gpio_num = cs_;
    io_config.pclk_hz = 40 * 1000 * 1000;
    io_config.lcd_cmd_bits = 8;
    io_config.lcd_param_bits = 8;
    io_config.spi_mode = 0;
    io_config.trans_queue_depth = 7;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)spi_host, &io_config, &io_handle));
    gpio_config_t gpio_conf = {};
    gpio_conf.intr_type     = GPIO_INTR_DISABLE;
    gpio_conf.mode          = GPIO_MODE_OUTPUT;
    gpio_conf.pin_bit_mask  = (0x1ULL << rst_);
    gpio_conf.pull_down_en  = GPIO_PULLDOWN_DISABLE;
    gpio_conf.pull_up_en    = GPIO_PULLUP_ENABLE;
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&gpio_conf));
    Set_ResetIOLevel(1);

    DisplayLen                = transfer >> 3; //(1byte 8ipex)
    DispBuffer                = (uint8_t *) heap_caps_malloc(DisplayLen, MALLOC_CAP_SPIRAM);
    assert(DispBuffer);
    /* 261023 诊断（architect 评审 §1.4）: 钉镜像缓冲的 DMA 能力事实——S3 上 PSRAM
     * 不在 DMA 区间, 驱动弹跳拷贝后 DMA 不读本镜像（重影机制排查锚）。 */
    ESP_LOGI(TAG, "rlcd mirror ptr=%p dma_capable=%d len=%d",
             DispBuffer, (int)esp_ptr_dma_capable(DispBuffer), DisplayLen);
	PixelIndexLUT = (uint16_t (*)[300])heap_caps_malloc(transfer * sizeof(uint16_t), MALLOC_CAP_SPIRAM);
	PixelBitLUT   = (uint8_t (*)[300])heap_caps_malloc(transfer * sizeof(uint8_t), MALLOC_CAP_SPIRAM);
    assert(PixelIndexLUT);
    assert(PixelBitLUT);
    if(width_ == 400) {
        InitLandscapeLUT();
    } else {
        InitPortraitLUT();
    }

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();
    lvgl_port_cfg_t port_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    port_cfg.task_priority   = 2;
    port_cfg.timer_period_ms = 50;
    /* taskLVGL 栈定容（M2-K1 缺陷②修复板级落点）：定容值收编命名常量为单源,
     * 依据见 KP_TASK_LVGL_STACK_SIZE 定义处（预解析去栈后栈需求回落 + 硬上界裕量）。 */
    port_cfg.task_stack      = KP_TASK_LVGL_STACK_SIZE;
    lvgl_port_init(&port_cfg);
    lvgl_port_lock(0);

    display_ = lv_display_create(width, height); /* 以水平和垂直分辨率（像素）进行基本初始化 */
    lv_display_set_flush_cb(display_, Lvgl_flush_cb);
    lv_display_set_user_data(display_, this);
	size_t lvgl_buffer_size = LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565) * transfer;
	uint8_t *lvgl_buffer1 = (uint8_t *) heap_caps_malloc(lvgl_buffer_size, MALLOC_CAP_SPIRAM);
    assert(lvgl_buffer1);
	/* 261022 观感修（真机轮 human 实证: 录音页滚动回显重影）: PARTIAL -> FULL。
	 * 根因 = 缓冲本就整帧大小却跑 PARTIAL——一帧按脏区分块多次 flush, 每次 flush
	 * 写镜像一块就 RLCD_Display 推全屏, 面板反复处于「半新半旧」混合态; 录音页
	 * 330ms 全页重绘把它放大成肉眼可见的 tearing。FULL 模式 = 一帧一次整帧推送,
	 * 面板恒一致。代价 = 每次 flush 全帧 SetPixel + SPI 推送（400x300 @40MHz
	 * ≈ 3ms 级, 330ms 节拍下余量充足）。 */
	lv_display_set_buffers(display_, lvgl_buffer1, NULL, lvgl_buffer_size, LV_DISPLAY_RENDER_MODE_FULL);

    ESP_LOGI(TAG, "RLCD init");
    RLCD_Init();

    lvgl_port_unlock();
    if (display_ == nullptr) {
        ESP_LOGE(TAG, "Failed to add display");
        return;
    }

    ESP_LOGI(TAG, "ui start");

    SetupUI();

    /* NFR-READ-1 字号真机对照实验页：2026-09-15 已实测（14px 清晰 / 12px 不可读，
     * 下限定 14px）。实验完毕，调用注释留存备查，恢复正常启动画面。 */
    /* font_size_test_page_show(); */
}

CustomLcdDisplay::~CustomLcdDisplay() {
}

void CustomLcdDisplay::InitPortraitLUT() {
    uint16_t W4 = width_ >> 2;
    for (uint16_t y = 0; y < height_; y++)
    {
        uint16_t byte_y = y >> 1;
        uint8_t  local_y = y & 1;
        for (uint16_t x = 0; x < width_; x++)
        {
            uint16_t byte_x = x >> 2;
            uint8_t  local_x = x & 3;

            uint32_t index = byte_y * W4 + byte_x;
            uint8_t bit = 7 - ((local_x << 1) | local_y);

            PixelIndexLUT[x][y] = index;
            PixelBitLUT  [x][y] = (1 << bit);
        }
    }
}

void CustomLcdDisplay::InitLandscapeLUT() {
    uint16_t H4 = height_ >> 2;
    for (uint16_t y = 0; y < height_; y++)
    {
        uint16_t inv_y = height_ - 1 - y;
        uint16_t block_y = inv_y >> 2;
        uint8_t  local_y  = inv_y & 3;
        for (uint16_t x = 0; x < width_; x++)
        {
            uint16_t byte_x = x >> 1;
            uint8_t  local_x = x & 1;

            uint32_t index = byte_x * H4 + block_y;
            uint8_t bit = 7 - ((local_y << 1) | local_x);

            PixelIndexLUT[x][y] = index;
            PixelBitLUT  [x][y] = (1 << bit);
        }
    }
}

void CustomLcdDisplay::Set_ResetIOLevel(uint8_t level) {
    gpio_set_level((gpio_num_t) rst_, level ? 1 : 0);
}

void CustomLcdDisplay::RLCD_SendCommand(uint8_t Reg) {
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, Reg, NULL, 0));
}

void CustomLcdDisplay::RLCD_SendData(uint8_t Data) {
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, -1, &Data, 1));
}

void CustomLcdDisplay::RLCD_Sendbuffera(uint8_t *Data, int len) {
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_color(io_handle, -1, Data, len));
}

void CustomLcdDisplay::RLCD_Reset(void) {
    Set_ResetIOLevel(1);
    vTaskDelay(pdMS_TO_TICKS(50));
    Set_ResetIOLevel(0);
    vTaskDelay(pdMS_TO_TICKS(20));
    Set_ResetIOLevel(1);
    vTaskDelay(pdMS_TO_TICKS(50));
}

void CustomLcdDisplay::RLCD_ColorClear(uint8_t color) {
    memset(DispBuffer, color, DisplayLen);
}

void CustomLcdDisplay::RLCD_Init() {
    RLCD_Reset();

    RLCD_SendCommand(0xD6);  // NVM Load Control
	RLCD_SendData(0x17);
	RLCD_SendData(0x02);

	RLCD_SendCommand(0xD1); //Booster Enable
	RLCD_SendData(0x01);

	RLCD_SendCommand(0xC0); //Gate Voltage Control
	RLCD_SendData(0x11);   
	RLCD_SendData(0x04);   

	RLCD_SendCommand(0xC1); //VSHP Setting
	RLCD_SendData(0x69);
	RLCD_SendData(0x69);
	RLCD_SendData(0x69);
	RLCD_SendData(0x69);

	RLCD_SendCommand(0xC2);
	RLCD_SendData(0x19);
	RLCD_SendData(0x19);
	RLCD_SendData(0x19);
	RLCD_SendData(0x19);

	RLCD_SendCommand(0xC4);
	RLCD_SendData(0x4B);
	RLCD_SendData(0x4B);
	RLCD_SendData(0x4B);
	RLCD_SendData(0x4B);

	RLCD_SendCommand(0xC5);
	RLCD_SendData(0x19);
	RLCD_SendData(0x19);
	RLCD_SendData(0x19);
	RLCD_SendData(0x19);

	RLCD_SendCommand(0xD8);
	RLCD_SendData(0x80);
	RLCD_SendData(0xE9);

	RLCD_SendCommand(0xB2);
	RLCD_SendData(0x02);

	RLCD_SendCommand(0xB3);
	RLCD_SendData(0xE5);
	RLCD_SendData(0xF6);
	RLCD_SendData(0x05);
	RLCD_SendData(0x46);
	RLCD_SendData(0x77);
	RLCD_SendData(0x77);
	RLCD_SendData(0x77);
	RLCD_SendData(0x77);
	RLCD_SendData(0x76);
	RLCD_SendData(0x45);

	RLCD_SendCommand(0xB4);
	RLCD_SendData(0x05);
	RLCD_SendData(0x46);
	RLCD_SendData(0x77);
	RLCD_SendData(0x77);
	RLCD_SendData(0x77);
	RLCD_SendData(0x77);
	RLCD_SendData(0x76);
	RLCD_SendData(0x45);

	RLCD_SendCommand(0x62);
	RLCD_SendData(0x32);
	RLCD_SendData(0x03);
	RLCD_SendData(0x1F);

	RLCD_SendCommand(0xB7);
	RLCD_SendData(0x13);

	RLCD_SendCommand(0xB0);
	RLCD_SendData(0x64);

	RLCD_SendCommand(0x11); 
	vTaskDelay(pdMS_TO_TICKS(200));     
	RLCD_SendCommand(0xC9);
	RLCD_SendData(0x00);

	RLCD_SendCommand(0x36);
	RLCD_SendData(0x48); 

	RLCD_SendCommand(0x3A);
	RLCD_SendData(0x11); 

	RLCD_SendCommand(0xB9);
	RLCD_SendData(0x20);

	RLCD_SendCommand(0xB8);
	RLCD_SendData(0x29);

	RLCD_SendCommand(0x21);

	RLCD_SendCommand(0x2A); 
	RLCD_SendData(0x12);
	RLCD_SendData(0x2A);

	RLCD_SendCommand(0x2B); 
	RLCD_SendData(0x00);
	RLCD_SendData(0xC7);

	RLCD_SendCommand(0x35);
	RLCD_SendData(0x00);

	RLCD_SendCommand(0xD0);
	RLCD_SendData(0xFF);

	RLCD_SendCommand(0x38);
	RLCD_SendCommand(0x29);

    RLCD_ColorClear(ColorWhite);
}

void CustomLcdDisplay::RLCD_SetPixel(uint16_t x, uint16_t y, uint8_t color) {
    uint32_t idx = PixelIndexLUT[x][y];
    uint8_t  mask = PixelBitLUT[x][y];

    uint8_t *p = &DispBuffer[idx];

    if (color)
        *p |= mask;
    else
        *p &= ~mask;
}

void CustomLcdDisplay::RLCD_Display() {
    RLCD_SendCommand(0x2A);     // Column Address Set
  	RLCD_SendData(0x12);
  	RLCD_SendData(0x2A);

  	RLCD_SendCommand(0x2B);     // Page Address Set
  	RLCD_SendData(0x00);
  	RLCD_SendData(0xC7);

  	RLCD_SendCommand(0x2c);     // Page Address Set

	RLCD_Sendbuffera(DispBuffer,DisplayLen);
}
