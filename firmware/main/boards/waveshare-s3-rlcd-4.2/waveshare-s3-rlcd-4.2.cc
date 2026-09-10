#include <esp_lcd_panel_vendor.h>
#include <driver/i2c_master.h>
#include <driver/spi_common.h>
#include <esp_log.h>
#include <esp_spiffs.h> /* 包 9b ART-E2: art SPIFFS 分区 -> /art VFS 注册（装配层）*/
#include <esp_rom_sys.h> /* esp_rom_delay_us: SHTC3 唤醒 tREC 恢复延时 */
#include "custom_lcd_display.h"
#include "wifi_board.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "codecs/box_audio_codec.h"
#include "wifi_station.h"
#include "mcp_server.h"
#include "view_router.h"
#include "lvgl.h"
#include "custom_lcd_display.h"
#include "display/kidpal_render_node.h" /* 包 9b HWV-A6 渲染节点 */

#define TAG "waveshare_rlcd_4_2"

/* 换皮消费接线 (包 9b HWV-A6 / ART-E2 收口)：单一资产层挂载根。装配层约定 VFS 路径
 * "/art"，并在 InitializeArt 里 esp_vfs_spiffs_register 把 art SPIFFS 分区 (分区表
 * partitions/v2/16m.csv: art,data,spiffs,0x780000,0x80000) 注册到该根；KAP1 位图镜像由
 * firmware/main/CMakeLists.txt 的 spiffs_create_partition_image(art ... FLASH_IN_PROJECT)
 * 产 build/art.bin 烧至 0x780000。分区拆分曾撞 U 冻结 canary (assets 0x800000/8M)、经
 * planner 裁定从 ota_1 尾部切 512KB 保 canary 后落地。未烧录/镜像过时时 mount 失败，各页
 * best-effort 降级回占位矩形 (不 panic、不崩)。 */
#define KIDPAL_ART_VFS_ROOT "/art"

/* 包 9b HWV-A4 启动回归修复 (fixer): 开机传感器读与音频 codec 共用同一条 I2C 总线
 * (ESP32_I2C_HOST / SDA=13 / SCL=14)。无应答从设备 (SHTC3 缺席) 会让同步事务在
 * i2c_ll_is_bus_busy 上长时间挂起, 并把总线留在 busy 态, 后续 codec (es8311_open)
 * 在同一总线永久自旋 -> task_wdt 卡死。故: (1) 传感器事务用有限超时兜底, 不再传 -1;
 * (2) 任一传感器读失败即复位总线; (3) 传感器读排在 codec 初始化之后。 */
#define HWV_SENSOR_I2C_TIMEOUT_MS 200

class CustomBoard : public WifiBoard {
private:
    i2c_master_bus_handle_t i2c_bus_;
    /* 包 9b HWV-A4 driver 绑定: RTC(PCF85063) + 温湿度(SHTC3) 在同一 I2C 主机上的
     * 从设备句柄 (InitializeSensors 挂载 + 开机各读一次, 真实读路径落点)。 */
    i2c_master_dev_handle_t pcf85063_dev_ = nullptr;
    i2c_master_dev_handle_t shtc3_dev_ = nullptr;
    Button boot_button_;
    CustomLcdDisplay *display_;
    adc_oneshot_unit_handle_t adc1_handle;
    adc_cali_handle_t cali_handle;
    bool vbat_status = 0;

    /* 包 9b HWV-A6: 把 InitializeSensors() 读到的 RTC/温湿度暂存于此，在
     * InitializeLcdDisplay() 之后统一注入渲染节点（LVGL 在传感器读取时尚未初始化）。 */
    int s_rtc_h    = 0;
    int s_rtc_m    = 0;
    int s_env_ok   = 0;
    int s_env_temp = 0;
    int s_env_rh   = 0;

    void InitializeI2c() {
        i2c_master_bus_config_t i2c_bus_cfg = {};
        i2c_bus_cfg.i2c_port = ESP32_I2C_HOST;
        i2c_bus_cfg.sda_io_num = AUDIO_CODEC_I2C_SDA_PIN;
        i2c_bus_cfg.scl_io_num = AUDIO_CODEC_I2C_SCL_PIN;
        i2c_bus_cfg.clk_source = I2C_CLK_SRC_DEFAULT;
        i2c_bus_cfg.glitch_ignore_cnt = 7;
        i2c_bus_cfg.intr_priority = 0;
        i2c_bus_cfg.trans_queue_depth = 0;
        i2c_bus_cfg.flags.enable_internal_pullup = 1;
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus_));
    }

    /* 传感器读失败后把 I2C 总线拉回干净态: 发 9 个时钟 + STOP 释放被从设备拉低的总线,
     * 防止一次超时的残留 busy 锁死后续 codec 事务 (启动回归根因的实质修复点)。 */
    void RecoverI2cBus() {
        esp_err_t err = i2c_master_bus_reset(i2c_bus_);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "i2c bus_reset err=%s (sensor read left bus in bad state)",
                     esp_err_to_name(err));
        }
    }

    /* PCF85063 时间寄存器为 BCD 编码 (高 4 位=十位, 低 4 位=个位)。掩掉 CV/OS/AF
     * 等标志位后仍须 BCD->十进制, 否则 raw 0x48 (=48 分) 会被当作 72 (越界分钟)。 */
    static uint8_t BcdToDec(uint8_t b) {
        return static_cast<uint8_t>(((b >> 4) * 10) + (b & 0x0F));
    }

    /* RTC(PCF85063) 走时直读: 从 Seconds 寄存器(0x04)连读秒/分/时 3 字节 (BCD)。
     * 真实 I2C 读事务 (写寄存器指针 -> repeated-start -> 读), 非仅挂载。 */
    bool ReadRtcTime(int* hour, int* min, int* sec) {
        if (pcf85063_dev_ == nullptr) {
            return false;
        }
        uint8_t reg = 0x04; /* PCF85063: 0x04=Seconds, 0x05=Minutes, 0x06=Hours */
        uint8_t rx[3] = {};
        esp_err_t err = i2c_master_transmit_receive(pcf85063_dev_, &reg, 1, rx, sizeof(rx),
                                                    HWV_SENSOR_I2C_TIMEOUT_MS);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "rtc pcf85063 read err=%s", esp_err_to_name(err));
            RecoverI2cBus(); /* 失败即清总线, 不污染后续 codec 事务 */
            return false;
        }
        if (sec)  *sec  = BcdToDec(rx[0] & 0x7F); /* 秒: 去 CV(bit7) 后 BCD->dec */
        if (min)  *min  = BcdToDec(rx[1] & 0x7F); /* 分: 去 OS(bit7) 后 BCD->dec */
        if (hour) *hour = BcdToDec(rx[2] & 0x3F); /* 时: 去 AF(bit7)/保留(bit6) 后 BCD->dec */
        return true;
    }

    /* SHTC3 温湿度直读: (1) POR 后处于睡眠, 必须先唤醒 (0x3517) 才响应; (2) 测量改用
     * 「不拉伸时钟」命令 0x7852 —— 纯写立即返回、不拉 SCL, 写完 esp_rom_delay_us 等待
     * 测量完成后, 再用独立 i2c_master_receive 读回 6 字节 (T_MSB,T_LSB,CRC,R_MSB,R_LSB,CRC)。
     * (no-stretch fallback) 上一版 hold-master 0x7CA2 合并事务里 SHTC3 测量期拉住 SCL,
     * 被 ESP-IDF master 当成总线卡死 ~10ms 即超时 (复烧 boot3 实证), 故退到 no-stretch。 */
    bool ReadShtc3(float* temp_c, float* rh) {
        if (shtc3_dev_ == nullptr) {
            return false;
        }
        uint8_t wakeup[2] = { 0x35, 0x17 }; /* SHTC3 release-from-sleep 0x3517 (强制唤醒) */
        esp_err_t err = i2c_master_transmit(shtc3_dev_, wakeup, sizeof(wakeup), HWV_SENSOR_I2C_TIMEOUT_MS);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "shtc3 wakeup err=%s", esp_err_to_name(err));
            RecoverI2cBus(); /* 唤醒失败也会残留 busy, 复位再返回, 不留脏总线 */
            return false;
        }
        esp_rom_delay_us(250); /* tREC 唤醒恢复 (datasheet <=240us), 留裕量 */

        uint8_t cmd[2] = { 0x78, 0x66 }; /* SHTC3 no-stretch 命令 0x7866 (Measure T first, clock-stretching disabled): 纯写、不拉 SCL、立即返回 */
        err = i2c_master_transmit(shtc3_dev_, cmd, sizeof(cmd), HWV_SENSOR_I2C_TIMEOUT_MS);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "shtc3 cmd err=%s", esp_err_to_name(err));
            RecoverI2cBus();
            return false;
        }
        esp_rom_delay_us(12000); /* 测量期 (T+RH 约 4.5ms + 裕量), 用显式延时替代被 STOP 打断的 stretch-wait */

        uint8_t rx[6] = {}; /* 0x7852 后无 clock-stretch, 独立 receive 即可读回 */
        err = i2c_master_receive(shtc3_dev_, rx, sizeof(rx), HWV_SENSOR_I2C_TIMEOUT_MS);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "shtc3 read err=%s", esp_err_to_name(err));
            RecoverI2cBus();
            return false;
        }
        uint16_t t = (uint16_t)((rx[0] << 8) | rx[1]);
        uint16_t r = (uint16_t)((rx[3] << 8) | rx[4]);
        if (temp_c) *temp_c = -45.0f + 175.0f * ((float)t / 65535.0f);
        if (rh)     *rh     = 100.0f * ((float)r / 65535.0f);
        return true;
    }

    /* 包 9b HWV-A4: 把 RTC(PCF85063) + SHTC3 挂到既有 I2C 主机并开机各真实读一次。
     * 未焊接 / 总线未 ready 时读失败仅告警、不阻断开机 (真机判读由 device checklist 承接)。 */
    void InitializeSensors() {
        i2c_device_config_t dev_cfg = {};
        dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
        dev_cfg.scl_speed_hz = 400000;
        dev_cfg.device_address = RTC_PCF85063_I2C_ADDR;
        ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_, &dev_cfg, &pcf85063_dev_));
        dev_cfg.device_address = SHTC3_I2C_ADDR;
        ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_, &dev_cfg, &shtc3_dev_));

        char corr_id[24];
        snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                 static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
        int h = 0, m = 0, s = 0;
        if (ReadRtcTime(&h, &m, &s)) {
            ESP_LOGI(TAG, "rtc_pcf85063 corr_id=%s ok rtc=%02d:%02d:%02d", corr_id, h, m, s);
            s_rtc_h = h; /* 包 9b HWV-A6: 暂存供渲染节点注入 */
            s_rtc_m = m;
        } else {
            ESP_LOGW(TAG, "rtc_pcf85063 corr_id=%s read_deferred", corr_id);
        }
        float tc = 0.0f, rh = 0.0f;
        if (ReadShtc3(&tc, &rh)) {
            ESP_LOGI(TAG, "shtc3 corr_id=%s ok temp_c=%.2f rh_pct=%.1f", corr_id, tc, rh);
            s_env_ok   = 1;                        /* 包 9b HWV-A6: env 有效标志 */
            s_env_temp = static_cast<int>(tc);      /* 截断为整数（hp1_state_t temp_c 为 int）*/
            s_env_rh   = static_cast<int>(rh + 0.5f);
        } else {
            ESP_LOGW(TAG, "shtc3 corr_id=%s read_deferred", corr_id);
        }
    }

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            /* 包 9b HWV-A9 (F2 设备侧页内输入生产接线): BOOT 单击先交故事页内输入消费面
             * kidpal_story_page_input。冻结契约里故事页不拥有 BOOT 单击 (LIST/REC/STAMP
             * 单击、READER 单击均回 GLOBAL -> HandleStoryKey 返回 false), 故交回既有全局
             * 动作 ToggleChatState, 会话切换语义严格不变。 */
            if (!McpServer::GetInstance().HandleStoryKey(KEY_ID_BOOT, KEY_GESTURE_CLICK)) {
                /* 回归修复 (包 9b HWV-A6 fix): 单击进入语音交互前，先把 xiaozhi SetupUI
                 * 切回前台。启动时渲染节点把 KidPal 首页 lv_screen_load 到前台，若语音期间
                 * 不交还 SetupUI，用户看不到「聆听」表情/状态，会反复点击——第二次点击命中
                 * Listening 态会 CloseAudioChannel，导致引擎侧只见一个空会话超时→告别
                 * （即真机反馈的「单击无变化、无 ASR」根因）。用 Schedule 让 LVGL 屏切换在
                 * main app task 执行（与既有 UI 操作同任务，避免按钮回调线程直接碰 LVGL）。
                 * ToggleChatState 语义（冻结）不变，仅额外保证语音态可见。 */
                app.Schedule([]() { kidpal_render_node_show_chat(); });
                app.ToggleChatState();
            }
        });
        /* BOOT 长按: 此板仅 BOOT 单物理键、全局层未占 BOOT 长按语义, 优先交故事阅读态翻屏
         * (READER 多屏 -> PAGE_TURN, 返回 true, 真机可翻正文); 其余页返回 false, 与此前
         * 「BOOT 长按无任何处理」的旧行为一致。 */
        boot_button_.OnLongPress([this]() {
            McpServer::GetInstance().HandleStoryKey(KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS);
        });
        /* 包 9b HWV-A6 阶段 B: BOOT 双按 = KidPal 4 页翻页（home→char→schedule→story→home）。
         * 关键澄清（本次回归排查实证 esp_button iot_button.c 状态机）：BUTTON_SINGLE_CLICK 与
         * BUTTON_DOUBLE_CLICK 由 btn->repeat 计数互斥分流（repeat==1 走单击、repeat==2 走双击），
         * 注册 OnDoubleClick 不会「吞掉」或额外延迟 OnClick；单击的 ~180ms 触发窗口
         * （CONFIG_BUTTON_SHORT_PRESS_TIME_MS=180 / CONFIG_BUTTON_PERIOD_TIME_MS=5=36 tick）是
         * 库固有行为，本包之前既有、非 OnDoubleClick 引入。真机「单击无 ASR」根因是语音态屏幕
         * 被 KidPal 首页盖住（见上 OnClick 注释），已由本回归修复交还 SetupUI 解决。
         * 长按（1500ms，独立于 repeat 分流）不受影响。走 Schedule 与 MCP 工具同任务上下文。 */
        boot_button_.OnDoubleClick([]() {
            Application::GetInstance().Schedule([]() {
                kidpal_render_node_cycle_next();
            });
        });
    }

    void InitializeTools() {
        auto& mcp_server = McpServer::GetInstance();
        mcp_server.AddTool("self.disp.network", "重新配网", PropertyList(),
        [this](const PropertyList&) -> ReturnValue {
            EnterWifiConfigMode();
            return true;
        });
    }

    void InitializeLcdDisplay() {
        spi_display_config_t spi_config = {};
        spi_config.mosi = RLCD_MOSI_PIN;
        spi_config.scl = RLCD_SCK_PIN;
        spi_config.dc = RLCD_DC_PIN;
        spi_config.cs = RLCD_CS_PIN;
        spi_config.rst = RLCD_RST_PIN;
        display_ = new CustomLcdDisplay(NULL, NULL, RLCD_WIDTH,RLCD_HEIGHT,DISPLAY_OFFSET_X,DISPLAY_OFFSET_Y,DISPLAY_MIRROR_X,DISPLAY_MIRROR_Y,DISPLAY_SWAP_XY,spi_config);
    }

    /* 换皮消费接线 (包 9b ART-E2 收口)：注册 art SPIFFS 分区为 VFS 根 "/art"，再把
     * U 单一资产层挂进该路径。esp_vfs_spiffs_register(base_path="/art",
     * partition_label="art") 属芯片装配层（不放 art_assets.c，保资产层模块纯净性守卫
     * ART-A5/B8）；format_if_mount_failed=false：未烧 art.bin 或镜像过时时不销毁、
     * 不格式化，仅返回错误 -> 随后 kidpal_display_art_mount 的 opendir 失败 -> 各页
     * best-effort 降级回占位矩形（绝不 panic）。KAP1 镜像由 CMake 的
     * spiffs_create_partition_image(art ... FLASH_IN_PROJECT) 产 build/art.bin 烧至 0x780000。 */
    void InitializeArt() {
        esp_vfs_spiffs_conf_t art_conf = {
            .base_path = KIDPAL_ART_VFS_ROOT,   /* "/art"：loader opendir/fopen 前缀 */
            .partition_label = "art",           /* 分区表 partitions/v2/16m.csv 的 art 行 */
            .max_files = 12,                     /* 单槽缓存 + 逐位图取用，留有向余量 */
            .format_if_mount_failed = false,     /* 未烧录/过时不销毁，降级不 panic */
        };
        esp_err_t art_err = esp_vfs_spiffs_register(&art_conf);
        if (art_err != ESP_OK) {
            ESP_LOGW(TAG, "art SPIFFS register failed (%s) on partition \"art\": %s -> UI uses placeholder fallback",
                     esp_err_to_name(art_err), KIDPAL_ART_VFS_ROOT);
        } else {
            ESP_LOGI(TAG, "art SPIFFS mounted at %s (partition \"art\")", KIDPAL_ART_VFS_ROOT);
        }
        if (kidpal_display_art_mount(KIDPAL_ART_VFS_ROOT) != 0) {
            ESP_LOGW(TAG, "art layer mount deferred: %s not available (SPIFFS provision pending) -> UI uses placeholder fallback",
                     KIDPAL_ART_VFS_ROOT);
        } else {
            ESP_LOGI(TAG, "art layer mounted at %s", KIDPAL_ART_VFS_ROOT);
        }
    }

public:
    CustomBoard() : boot_button_(BOOT_BUTTON_GPIO) {
        InitializeI2c();
        /* 启动回归修复 (顺序层): 先在干净总线上把音频 codec (ES8311/ES7210) 建起来
         * (GetAudioCodec 构造 static BoxAudioCodec, 走 es8311_open 的 I2C 事务), 再跑
         * 传感器开机读。这样传感器 (SHTC3 缺席) 即便超时会发生在 codec 已成功之后, 配合
         * 各读函数失败路径的 RecoverI2cBus, 总线不会被残留 busy 锁死到 codec 事务上。 */
        GetAudioCodec();
        InitializeSensors();
        InitializeButtons();
        InitializeTools();
        InitializeLcdDisplay();
        InitializeArt();
        /* 包 9b HWV-A6: 渲染节点在 LVGL 与 art SPIFFS 均就绪后初始化。
         * init() 创建 s_kidpal_scr（不渲染，无 I/O）；inject 注入开机 RTC/温湿度；
         * show_home() 渲染首页 P1 帧并加载到 RLCD。 */
        kidpal_render_node_init();
        kidpal_render_node_inject_rtc(s_rtc_h, s_rtc_m);
        kidpal_render_node_inject_env(s_env_ok, s_env_temp, s_env_rh);
        kidpal_render_node_show_home();
   }

    virtual AudioCodec* GetAudioCodec() override {
        static BoxAudioCodec audio_codec(
            i2c_bus_, 
            AUDIO_INPUT_SAMPLE_RATE, 
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_MCLK, 
            AUDIO_I2S_GPIO_BCLK, 
            AUDIO_I2S_GPIO_WS, 
            AUDIO_I2S_GPIO_DOUT, 
            AUDIO_I2S_GPIO_DIN,
            AUDIO_CODEC_PA_PIN, 
            AUDIO_CODEC_ES8311_ADDR, 
            AUDIO_CODEC_ES7210_ADDR, 
            AUDIO_INPUT_REFERENCE);
        return &audio_codec;
    }

    virtual Display* GetDisplay() override {
        return display_;
    }
};

DECLARE_BOARD(CustomBoard);