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
#include "display/kidpal_layer.h" /* K3R [R1]: 层值闭集供 kidpal_story_key_owns 判据（front_layer 入参语义）*/
#include "display/reminder_page.h" /* 包 260915-m1-k1a-key-layer-dispatch: 顶层覆盖层裁决函数 kidpal_reminder_key 设备侧首次接线 */
#include "display/char_page.h" /* 包 260917-m2-k3-char-window-overlay: 查字窗 KEY 短按重播 kidpal_char_page_replay 设备侧消费 */
#include "key_gesture_parser.h" /* 包 260911-key-route-a1 冻结按键手势解析 + 全局路由表 */
#include "sdkconfig.h"          /* CONFIG_BUTTON_* 按键消抖/长按窗参数 */
#include <esp_adc/adc_oneshot.h>      /* 包 260915-m1-n1b-p1-home-battery: 板级电池 ADC */
#include <esp_adc/adc_cali.h>         /* 包 260915-m1-n1b-p1-home-battery: ADC 校准 */
#include <esp_adc/adc_cali_scheme.h>  /* 包 260915-m1-n1b-p1-home-battery: 曲线拟合校准 */
#include "adc_battery_monitor.h"      /* 包 260915-m1-n1b-p1-home-battery: 充电态监测基座 */
#include <esp_sntp.h>                 /* K5 260922-m3-k5-sntp-time-sync: SNTP 同步成功回调注册 */
#include <sys/time.h>                 /* K5: settimeofday 断电恢复系统时间 */
#include <time.h>                     /* K5: time/localtime_r/mktime（RTC <-> 系统时间桥） */
#include <freertos/task.h>            /* K5: 周期时间维护 task */
#include "time_sync.h"                /* K5 seam [S1]: bcd_encode / write_decision 纯逻辑单源 */
#include <esp_vfs_fat.h>              /* 包 261016-sd-card-mount: esp_vfs_fat_sdmmc_mount / mount_config */
#include <sdmmc_cmd.h>               /* 包 261016-sd-card-mount: sdmmc_card_t / sdmmc_card_print_info */
#include <driver/sdmmc_host.h>       /* 包 261016-sd-card-mount: SDMMC_HOST_DEFAULT / sdmmc_slot_config_t */
#include <stdio.h>                   /* 包 261016-sd-card-mount: 探针 VFS 文件 I/O (fopen/fwrite/fread/fclose/remove) */
#include <errno.h>                   /* 包 261016-sd-card-mount: 探针失败步 errno 诊断 */
#include "diagnostics/sd_log.h"      /* 包 261018-sd-log-persist: 挂载成功分支 KidpalSdLogAttach */

/* 包 260915-m1-n1b-p1-home-battery 任务 B：联网小标切小号 FA 字体（与温/湿/电辅助条小标
 * 同档）。基座 SetupUI 初帧把 network_label_ 落主题 icon_font=font_awesome_30_4（30px），
 * 此处构造期再压到 font_awesome_14_1（14px，WiFi 字形 ink ~13px），并靠 waveshare
 * CustomLcdDisplay::SetTheme override 在主题刷新后维持同档。 */
LV_FONT_DECLARE(font_awesome_14_1);

#define TAG "waveshare_rlcd_4_2"

/* K5（TaskPackage 260922-m3-k5-sntp-time-sync）[S3]：SNTP 同步成功旗标。回调注册于
 * InitializeSensors（esp_sntp_set_time_sync_notification_cb, 全局唯一消费者——本固件
 * 无其他 esp_sntp_init 调用点, 不会被覆盖）；回调在 lwip SNTP task 上下文触发, 只置
 * 旗标不做 I/O, 由板级周期 task（K5-E3 真机面）消费执行 PCF85063 写回。 */
static volatile int s_ts_sntp_synced = 0;
static void ts_sntp_sync_cb(struct timeval *tv)
{
    (void)tv;
    s_ts_sntp_synced = 1;
}

/* 包 260915-m1-n1b-p1-home-battery：板级电池 ADC 采集配置。官方 Demo
 * ESP32-S3-RLCD-4.2-Demo/02_ESP-IDF/03_ADC_Test（只读参考，重写进本文件）已验证：
 * 电池电压经板上分压进 GPIO4 = ADC1_CH3，Adc_GetBatteryLevel 把校准后电压按
 * VBAT = mV*3 / 1000、[3.0, 4.12]V 线性映射到 0..100。 */
#define BATTERY_ADC_CHANNEL          ADC_CHANNEL_3
#define BATTERY_VOLTAGE_DIVIDER      3.0f  /* VBAT = ADC 引脚校准电压 x 3（板上分压比） */
#define BATTERY_VOLTAGE_MIN_V        3.0f  /* <= 3.0V -> 0% */
#define BATTERY_VOLTAGE_MAX_V        4.12f /* >= 4.12V -> 100% */
#define BATTERY_EST_UPPER_RESISTOR   200000.0f /* 分压估算用上臂标称值（1:2 -> 比率 3，
                                              * 仅喂 AdcBatteryMonitor 趋势引擎；显示电量
                                              * 走 ReadBatteryLevel 的 Demo 实测映射，
                                              * PH-A6 真机轮以实测读数校核） */
#define BATTERY_EST_LOWER_RESISTOR   100000.0f

/* 换皮消费接线 (包 9b HWV-A6 / ART-E2 收口)：单一资产层挂载根。装配层约定 VFS 路径
 * "/art"，并在 InitializeArt 里 esp_vfs_spiffs_register 把 art SPIFFS 分区 (分区表
 * partitions/v2/16m.csv: art,data,spiffs,0x780000,0x80000) 注册到该根；KAP1 位图镜像由
 * firmware/main/CMakeLists.txt 的 spiffs_create_partition_image(art ... FLASH_IN_PROJECT)
 * 产 build/art.bin 烧至 0x780000。分区拆分曾撞 U 冻结 canary (assets 0x800000/8M)、经
 * planner 裁定从 ota_1 尾部切 512KB 保 canary 后落地。未烧录/镜像过时时 mount 失败，各页
 * best-effort 降级回占位矩形 (不 panic、不崩)。 */
#define KIDPAL_ART_VFS_ROOT "/art"

/* K5（TaskPackage 260922-m3-k5-sntp-time-sync）：周期时间维护节拍。分钟级时间语义下
 * 节拍相位按墙上分钟边界对齐（R3-2/[S6], 读点秒 ≈0、P1 显示滞后 <1s; 退役挂 boot
 * 相位的固定 60000ms 节拍）——重读 RTC + re-inject P1（[S4], 与 P1 重绘分钟粒度节拍
 * 协调, 不跳字）+ 消费 SNTP 同步成功旗标写回 PCF85063（[S3]）。 */

/* 包 9b HWV-A4 启动回归修复 (fixer): 开机传感器读与音频 codec 共用同一条 I2C 总线
 * (ESP32_I2C_HOST / SDA=13 / SCL=14)。无应答从设备 (SHTC3 缺席) 会让同步事务在
 * i2c_ll_is_bus_busy 上长时间挂起, 并把总线留在 busy 态, 后续 codec (es8311_open)
 * 在同一总线永久自旋 -> task_wdt 卡死。故: (1) 传感器事务用有限超时兜底, 不再传 -1;
 * (2) 任一传感器读失败即复位总线; (3) 传感器读排在 codec 初始化之后。 */
#define HWV_SENSOR_I2C_TIMEOUT_MS 200

/* -------------------------------------------------------------------------- *
 * 包 260911-key-route-a1：冻结全局路由表（key_gesture_parser.h）的设备侧 dispatch
 * 消费点。路由输出（GLOBAL_ACT_*）在此唯一映射至应用动作；手势->动作的权威判定单一
 * 落在 key_route_global，设备侧不另造一套手势语义（ARQ-260911 防 BOOT≈KEY 假绿）。
 * ------------------------------------------------------------------------ */

static uint32_t keya_now_ms(void)
{
    return static_cast<uint32_t>(esp_log_timestamp());
}

/* 前台层归一化值喂入 key_route_layer 第三参（TaskPackage 260917-m2-k3-char-window-overlay,
 * seam [2] 语义扩：0 = 基层 / 配网 / 无前台; 非 0 = 前台覆盖层层值）。覆盖层身份由
 * front_is_overlay 单一裁决（与 keybar 中槽页点隐藏同源），层值由 front_layer 取——
 * 查字窗前台时第三参 = KIDPAL_LAYER_CHAR_WINDOW，路由表据此选中查字窗列。 */
static int keya_front_route_top(void)
{
    if (kidpal_render_node_front_is_overlay() == 0) {
        return 0; /* 基层 / 配网 / 无前台 -> 基层列 */
    }
    return kidpal_render_node_front_layer();
}

/* 冻结路由输出 -> 应用动作 dispatch（GLOBAL_ACT_SESSION_TOGGLE / HOME / PAGE_NEXT 三
 * 枚举在此被设备侧消费）。LVGL 屏切换统一经 Application::Schedule 回 main app task，
 * 与既有 BOOT 单击 show_chat 先例同任务上下文。 */
static void keya_dispatch_global_action(global_action_t act)
{
    auto& app = Application::GetInstance();
    if (act != GLOBAL_ACT_NONE) {
        char corr_id[24];
        snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                 static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
        ESP_LOGI(TAG, "key_route_dispatch corr_id=%s action=%d", corr_id, static_cast<int>(act));
    }
    switch (act) {
    case GLOBAL_ACT_SESSION_TOGGLE:
        /* KEY 单击=发起对话：先把 xiaozhi SetupUI（聆听/表情/状态）切回前台保语音态可见，
         * 再切换会话（语义与现 BOOT 单击 ToggleChatState 严格一致，包 9b HWV-A6 fix）。 */
        app.Schedule([]() { kidpal_render_node_show_chat(); });
        app.ToggleChatState();
        break;
    case GLOBAL_ACT_HOME:
        /* KEY 长按=回首页（所有页面一致，路由无页面入参的全局层语义；主页面 KEY 长按 =
         * 回首页，260915-m1-k1a KD-A3 取舍 C「上一层=首页」在基层的映射，零回归）。 */
        app.Schedule([]() { kidpal_render_node_show_home(); });
        break;
    case GLOBAL_ACT_PAGE_NEXT:
        /* BOOT 单击=翻页（P1->P2->P3->P4 循环，冻结路由表 KEY_ID_BOOT+CLICK 恒产此动作；
         * 主页面基层路径零回归）。 */
        app.Schedule([]() { kidpal_render_node_cycle_next(); });
        break;
    case GLOBAL_ACT_OVERLAY_CLOSE:
        /* 覆盖层关层回原页：映射显式关层步骤 kidpal_render_node_dismiss_front_overlay
         * （「关窗」唯一入口，KD-A2），**永不**映射翻页/cycle_next（v1.39 隔离口径）。
         * 覆盖层顶层在回调入口已先交顶层裁决函数 kidpal_reminder_key（DISMISS 即消费）；
         * 本 case 为 key_route_layer 覆盖层列返回的兜底消费（如覆盖层为非提醒弹窗）。
         * 会话级联（KD-A5 真机 r1 修复）：前台为 VOICE 覆盖层（会话类覆盖层）且设备处于
         * 会话态（Connecting/Listening/Speaking）时，关层同时结束会话——复用 KEY 单击同
         * 一条 ToggleChatState 会话结束路径（listening 态 = CloseAudioChannel），requirements
         * §2.3「问答·聆听/思考」行 BOOT 短按 = 结束会话回原页；提醒弹窗等非会话覆盖层
         * 关层语义不变（本就无会话）。时序安全性：TOGGLE_CHAT 事件在主循环先于 SCHEDULE
         * 队列处理、且 idle 落定经 WS 断开异步抵达，故关层（回原页落定）必先于 idle 的
         * voice_end；voice_end 见 VOICE 已不在栈上而零副作用，不会把「回原页」改写成
         * 「回首页」。 */
        if (kidpal_render_node_front_is_voice() != 0) {
            DeviceState st = app.GetDeviceState();
            if (st == kDeviceStateConnecting || st == kDeviceStateListening ||
                st == kDeviceStateSpeaking) {
                app.ToggleChatState();
            }
        }
        app.Schedule([]() { kidpal_render_node_dismiss_front_overlay(); });
        break;
    case GLOBAL_ACT_CHAR_REPLAY:
        /* 查字窗 KEY 短按 = 重播笔顺（S-F3.3 AC1, 含打断当前播报重演）：映射
         * kidpal_char_page_replay（动画进度归零、从第一笔重新逐笔演示）。不产会话动作、
         * 不翻页——「播完会话即结束、不回聆听态」（§2.2 行 68 / F3 窗口交互包 v1.37）。 */
        app.Schedule([]() { kidpal_char_page_replay(); });
        break;
    default:
        break;
    }
}

/* 覆盖层顶层按键裁决（260915-m1-k1a-key-layer-dispatch, KD-A1/A3）：覆盖层顶层先交顶层
 * 覆盖层裁决函数 kidpal_reminder_key（提醒弹窗，设备侧首次接线——requirements §2.3
 * 对账 ①「正经裁决函数设备侧零调用」修复落点）；返回 KRM_KEY_DISMISS = 顶层覆盖层已
 * 消费（关层回原页），返回 1；未消费（如覆盖层非提醒弹窗 / 无驻留弹窗）返回 0，落
 * layer-aware 路由表 key_route_layer。 */
static int keya_overlay_consume(key_id_t key, key_gesture_t g)
{
    kidpal_reminder_t *r = kidpal_reminder_default();
    page_id_t page_out = PAGE_P1;
    page_id_t cur = static_cast<page_id_t>(kidpal_render_node_get_page());

    if (kidpal_reminder_key(r, key, g, cur, &page_out) == KRM_KEY_DISMISS) {
        kidpal_render_node_dismiss_front_overlay();
        return 1;
    }
    return 0;
}

/* P3 星星记录本页内手势分流前置判据（260924-m4-k2-p3-star-notebook-ui）
 * + K3R [R1] 单源归属判据接线（261001-m4-k3-record-session-live-repair）：
 * 归属决策统一收敛到 story_page.c 纯逻辑缝 kidpal_story_key_owns（seam [R1] 权威 =
 * record_session_repair.acceptance.c 文件头），板级只供全输入：
 *   ① front_layer = keya_front_route_top()（覆盖层归一化值; 基层/配网/无前台 = 0 ==
 *      KIDPAL_LAYER_NONE, 覆盖层前台 = 其层值）;
 *   ② kidpal_page = kidpal_render_node_get_page()（本地浏览页）;
 *   ③ page_state = kidpal_render_node_story_substate()（P3 子态; 非 P3/它覆盖层/-1）;
 *   ④ ctx_active = kidpal_story_session_ctx_active()（记录语境标志）。
 * [R1] 三支（记录语境录音态 / 删除确认覆盖层吞全手势 / 基层 P3）即原两分支 +
 * D4a 修复支：录音时 VOICE 让位后基层无覆盖层, 判据自然成立 -> KEY 交故事页 ->
 * kidpal_story_page_input 在 RECORDING 页态映 KSPV2_ACT_STOP_RECORD, **绝不**落
 * key_route_layer -> GLOBAL_ACT_SESSION_TOGGLE 毁稿路径（D4a 根）。页不拥有该
 * 手势时 HandleStoryKey 返回 false、落全局路由（主页面路径零回归）。 */
static bool keya_story_page_wants(int overlay_top)
{
    int kidpal_page = kidpal_render_node_get_page();
    int page_state = kidpal_render_node_story_substate();
    int ctx_active = kidpal_story_session_ctx_active();
    return kidpal_story_key_owns(overlay_top, kidpal_page, page_state,
                                 ctx_active) == 1;
}

/* 冻结解析器（key_gesture_parser）设备侧产出手势的一致性观测日志（K6 结构化 + corr_id）。
 * parser 输出不在此 dispatch——dispatch 唯一落在 iot_button 手势回调（避免 iot_button
 * 短按窗 180ms 与 parser 双击窗 350ms 在单击/双击消歧时序上的分歧导致重复触发）。 */
static void keya_log_gesture(key_id_t key, key_gesture_t g)
{
    if (g == KEY_GESTURE_NONE) {
        return;
    }
    char corr_id[24];
    snprintf(corr_id, sizeof(corr_id), "kdp-%u",
             static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
    ESP_LOGI(TAG, "key_gesture_parser corr_id=%s key=%d gesture=%d", corr_id,
             static_cast<int>(key), static_cast<int>(g));
}

class CustomBoard : public WifiBoard {
private:
    i2c_master_bus_handle_t i2c_bus_;
    /* 包 9b HWV-A4 driver 绑定: RTC(PCF85063) + 温湿度(SHTC3) 在同一 I2C 主机上的
     * 从设备句柄 (InitializeSensors 挂载 + 开机各读一次, 真实读路径落点)。 */
    i2c_master_dev_handle_t pcf85063_dev_ = nullptr;
    i2c_master_dev_handle_t shtc3_dev_ = nullptr;
    Button boot_button_;
    /* 包 260911-key-route-a1：KEY(GPIO18) 主司对话，与 boot_button_(GPIO0 主司翻页) 并列。
     * 冻结解析器实例（单实例单键，key_gesture_parser.h）：KEY/BOOT 各一，喂 raw 边沿。 */
    Button key_button_;
    key_gesture_parser_t boot_gesture_;
    key_gesture_parser_t key_gesture_;
    CustomLcdDisplay *display_;
    /* 包 260915-m1-n1b-p1-home-battery 电量链路接线（原三处死成员销账：声明 +
     * 真实使用）。adc1_handle/cali_handle = InitializeBatteryAdc 创建的 ADC oneshot
     * unit 与曲线拟合校准句柄（ReadBatteryLevel 消费 + 外部句柄喂
     * AdcBatteryMonitor）；vbat_status = 最近一次电量读数有效性（GetBatteryLevel
     * override 返回值语义：读失败 = 如实报无电量数据）。 */
    adc_oneshot_unit_handle_t adc1_handle = nullptr;
    adc_cali_handle_t cali_handle = nullptr;
    bool vbat_status = false;
    AdcBatteryMonitor *battery_monitor_ = nullptr;

    /* 包 9b HWV-A6: 把 InitializeSensors() 读到的 RTC/温湿度暂存于此，在
     * InitializeLcdDisplay() 之后统一注入渲染节点（LVGL 在传感器读取时尚未初始化）。 */
    int s_rtc_h    = 0;
    int s_rtc_m    = 0;
    int s_rtc_mday = 0; /* M1-N1b 日期 DD-Mon: PCF85063 0x07=日(BCD) 带出 */
    int s_rtc_mon  = 0; /*              PCF85063 0x09=月(BCD) 带出 */
    /* M3-K2 今天列推导: PCF85063 0x08=Weekdays 带出, 周一=0..周日=6（与 producer
     * day 同基）; -1 = 读失败/寄存器值非法（今天列不高亮, 不画假「今天」）。 */
    int s_rtc_wday = -1;
    int s_env_ok   = 0;
    int s_env_temp = 0;
    int s_env_rh   = 0;

    /* 包 261016-sd-card-mount (TF 卡接入): 启动期 SDMMC 挂载标志。InitializeSdCard
     * 成功置真、失败/无卡置假; 经 SdCardMounted() 供将来消费方判 /sdcard 能力就绪
     * （本包自身零消费方）。默认假 = 未挂载保守态。 */
    bool s_sd_mounted = false;

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

    /* RTC(PCF85063) 走时直读: 从 Seconds 寄存器(0x04)连读秒/分/时/日/星期/月 6 字节
     * (BCD/二进制)。真实 I2C 读事务 (写寄存器指针 -> repeated-start -> 读), 非仅挂载。
     * 0x04=Seconds, 0x05=Minutes, 0x06=Hours, 0x07=Days, 0x08=Weekdays, 0x09=Months。
     * 日期字段为 M1-N1b 日期 DD-Mon 扩展带出 (0x07 日 / 0x09 月); 星期字段为 M3-K2
     * 今天列推导带出 (0x08, 二进制 0..6, PCF85063 口径 0=周日)。
     * K5（260922-m3-k5-sntp-time-sync）: 新增 os_bit 出参——Minutes 寄存器 bit7 =
     * OS（oscillator stop, 晶振曾停振）标志, 先读出交 [S1] write_decision 做可信门,
     * 不再掩掉丢弃（plan 边角: OS 置位 = 时间不可信, 不写回/不恢复、等 SNTP）。 */
    bool ReadRtcTime(int* hour, int* min, int* sec, int* mday, int* mon, int* wday_mon0,
                     int* os_bit_out) {
        if (pcf85063_dev_ == nullptr) {
            return false;
        }
        uint8_t reg = 0x04; /* PCF85063: 0x04=Seconds, 0x05=Minutes, 0x06=Hours, 0x07=Days, 0x08=Weekdays, 0x09=Months */
        uint8_t rx[6] = {};
        esp_err_t err = i2c_master_transmit_receive(pcf85063_dev_, &reg, 1, rx, sizeof(rx),
                                                    HWV_SENSOR_I2C_TIMEOUT_MS);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "rtc pcf85063 read err=%s", esp_err_to_name(err));
            RecoverI2cBus(); /* 失败即清总线, 不污染后续 codec 事务 */
            return false;
        }
        if (os_bit_out) *os_bit_out = static_cast<int>((rx[1] >> 7) & 0x01); /* Minutes bit7 = OS, 先读不掩 */
        if (sec)  *sec  = BcdToDec(rx[0] & 0x7F); /* 秒: 去 CV(bit7) 后 BCD->dec */
        if (min)  *min  = BcdToDec(rx[1] & 0x7F); /* 分: 去 OS(bit7) 后 BCD->dec */
        if (hour) *hour = BcdToDec(rx[2] & 0x3F); /* 时: 去 AF(bit7)/保留(bit6) 后 BCD->dec */
        if (mday) *mday = BcdToDec(rx[3] & 0x3F); /* 日=0x07: 去 bit7/bit6 后 BCD->dec (1..31) */
        if (wday_mon0) {
            /* 星期=0x08: 二进制 0..6（PCF85063 口径 0=周日）, 掩 bit7..3 保留位。
             * 映射周一=0 基: 0(周日)->6, 1(周一)..6(周六)->raw-1; >6 非法 -> -1。 */
            int raw = static_cast<int>(rx[4] & 0x07);
            *wday_mon0 = (raw > 6) ? -1 : (raw == 0 ? 6 : raw - 1);
        }
        if (mon)  *mon  = BcdToDec(rx[5] & 0x1F); /* 月=0x09: 去 bit7..5 (century/leap) 后 BCD->dec (1..12) */
        return true;
    }

    /* K5（260922-m3-k5-sntp-time-sync）[S3]：PCF85063 I2C 写路径（现固件只读, 本函数
     * 新增）——UTC+8 本地时间逐字段 BCD 编码写 0x04–0x09（秒/分/时/日/星期/月, 寄存器
     * 指针自动递增一事务完成）。BCD 编码唯一走 [S1] kidpal_ts_bcd_encode 单源（旁路
     * 自写一份 = 行为等价假设破坏, 冻结齿 TEST 5 tooth 2 红）。写入值天然 < 60/24/32,
     * 秒/分寄存器 bit7 标志位（CV/OS）随 0 写入清除。PCF85063 无年寄存器, 年份不写。
     * 调用前置可信门 = [S1] kidpal_ts_write_decision（OS 位 / year plausibility,
     * 见 SyncSystemTimeToRtc）。 */
    bool WriteRtcTime(int hour, int min, int sec, int mday, int wday_sun0, int mon) {
        if (pcf85063_dev_ == nullptr) {
            return false;
        }
        int es = kidpal_ts_bcd_encode(sec);
        int em = kidpal_ts_bcd_encode(min);
        int eh = kidpal_ts_bcd_encode(hour);
        int ed = kidpal_ts_bcd_encode(mday);
        int ew = kidpal_ts_bcd_encode(wday_sun0);
        int eo = kidpal_ts_bcd_encode(mon);
        if (es < 0 || em < 0 || eh < 0 || ed < 0 || ew < 0 || eo < 0) {
            ESP_LOGW(TAG, "rtc pcf85063 write rejected: field out of BCD domain "
                          "(h=%d m=%d s=%d mday=%d wday=%d mon=%d)",
                     hour, min, sec, mday, wday_sun0, mon);
            return false;
        }
        uint8_t tx[7] = { 0x04, static_cast<uint8_t>(es), static_cast<uint8_t>(em),
                          static_cast<uint8_t>(eh), static_cast<uint8_t>(ed),
                          static_cast<uint8_t>(ew), static_cast<uint8_t>(eo) };
        esp_err_t err = i2c_master_transmit(pcf85063_dev_, tx, sizeof(tx),
                                            HWV_SENSOR_I2C_TIMEOUT_MS);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "rtc pcf85063 write err=%s", esp_err_to_name(err));
            RecoverI2cBus(); /* 失败即清总线, 不污染后续 codec 事务 */
            return false;
        }
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
        int h = 0, m = 0, s = 0, mday = 0, mon = 0, wday = -1, os_bit = 0;
        if (ReadRtcTime(&h, &m, &s, &mday, &mon, &wday, &os_bit)) {
            ESP_LOGI(TAG, "rtc_pcf85063 corr_id=%s ok rtc=%02d:%02d:%02d date=%02d-%02d wday=%d os=%d",
                     corr_id, h, m, s, mon, mday, wday, os_bit);
            s_rtc_h = h; /* 包 9b HWV-A6: 暂存供渲染节点注入 */
            s_rtc_m = m;
            s_rtc_mday = mday; /* M1-N1b 日期 DD-Mon: 暂存月/日供注入 */
            s_rtc_mon  = mon;
            s_rtc_wday = wday; /* M3-K2 今天列: 暂存星期供 inject_weekday */

            /* K5（260922-m3-k5-sntp-time-sync）[S3]：boot 读 RTC 后 settimeofday 恢复
             * 系统时间（现状缺口 = 只注入 P1 不恢复, 断网重启后状态栏时钟仍 1970,
             * S-F1.1 AC4 / NFR-OFF-1）。恢复前过 [S1] write_decision 统一可信门:
             *   - os_bit: 晶振曾停振（纽扣电池耗尽）-> 不恢复、等 SNTP;
             *   - year_ok 入参 = RTC 日期字段 plausibility 代理（minimal: PCF85063 无年
             *     寄存器, 世纪位保证 20xx 而且芯片只被本机 SNTP 写回路径写过——写回本身
             *     已过 year_ok 门; 工厂裸片全 0 寄存器 mday=0 在此被拒, 不画假时间）;
             *   - 系统时钟已 plausible（软重启等）则跳过恢复, 不把正确年份回退到基线。
             * 恢复年份基线 2025（tm_year=125, 过 lvgl 守门 tm_year>=2025-1900; 显示面
             * 只消费 HH:MM 与 DD-Mon, 年份不显示, 月/日逐字保留）。 */
            struct tm tm_cur = {};
            time_t now_cur = time(NULL);
            localtime_r(&now_cur, &tm_cur);
            if (!kidpal_ts_year_ok(tm_cur.tm_year)) {
                int plausible = (mon >= 1 && mon <= 12 && mday >= 1 && mday <= 31 &&
                                 h >= 0 && h <= 23 && m >= 0 && m <= 59 && s >= 0 && s <= 59);
                if (kidpal_ts_write_decision(os_bit, plausible)) {
                    struct tm tm_rtc = {};
                    tm_rtc.tm_year = 125; /* 基线 2025（RTC 无年寄存器, 见上注） */
                    tm_rtc.tm_mon = mon - 1;
                    tm_rtc.tm_mday = mday;
                    tm_rtc.tm_hour = h;
                    tm_rtc.tm_min = m;
                    tm_rtc.tm_sec = s;
                    tm_rtc.tm_wday = (wday < 0) ? 0 : (wday == 6 ? 0 : wday + 1); /* 周一=0 基 -> tm 周日=0 基 */
                    tm_rtc.tm_isdst = 0;
                    struct timeval tv = {};
                    tv.tv_sec = mktime(&tm_rtc);
                    tv.tv_usec = 0;
                    settimeofday(&tv, nullptr);
                    ESP_LOGI(TAG, "time_restore corr_id=%s boot_rtc_settimeofday ok "
                                  "rtc=%02d:%02d:%02d date=%02d-%02d os=%d",
                             corr_id, h, m, s, mon, mday, os_bit);
                } else {
                    ESP_LOGW(TAG, "time_restore corr_id=%s rejected_by_write_decision "
                                  "os=%d plausible_date=%d (等 SNTP, 不把坏时间当权威)",
                             corr_id, os_bit, plausible);
                }
            }
        } else {
            ESP_LOGW(TAG, "rtc_pcf85063 corr_id=%s read_deferred", corr_id);
        }
        /* K5 [S3]：注册 SNTP 同步成功回调（全局唯一消费者, 见 s_ts_sntp_synced 注）。 */
        esp_sntp_set_time_sync_notification_cb(ts_sntp_sync_cb);
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

    /* 包 260915-m1-n1b-p1-home-battery：板级电池 ADC 挂起（参考树 03_ADC_Test
     * Adc_PortInit 只读重写）：ADC1 oneshot unit + GPIO4(ADC1_CH3) 通道（12bit /
     * 12dB）+ 曲线拟合校准。句柄落 adc1_handle/cali_handle 死成员（销账），并经
     * 外部句柄构造喂 AdcBatteryMonitor（unit 排他 claim，估计库不重复建 unit）。 */
    void InitializeBatteryAdc() {
        adc_oneshot_unit_init_cfg_t init_cfg = {};
        init_cfg.unit_id = ADC_UNIT_1;
        ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc1_handle));
        adc_oneshot_chan_cfg_t chan_cfg = {};
        chan_cfg.bitwidth = ADC_BITWIDTH_12;
        chan_cfg.atten = ADC_ATTEN_DB_12;
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, BATTERY_ADC_CHANNEL, &chan_cfg));
        adc_cali_curve_fitting_config_t cali_cfg = {};
        cali_cfg.unit_id = ADC_UNIT_1;
        cali_cfg.chan = BATTERY_ADC_CHANNEL;
        cali_cfg.atten = ADC_ATTEN_DB_12;
        cali_cfg.bitwidth = ADC_BITWIDTH_12;
        ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_cfg, &cali_handle));

        /* AdcBatteryMonitor 基座实装：充电态趋势检测（本板充放电管理无充电检测 GPIO
         * 在册——arch 硬件表 CHG/WRN 仅指示灯，官方 Demo 亦无充电检测；电压上升趋势
         * = 充电，PH-A6 真机轮回读）。 */
        battery_monitor_ = new AdcBatteryMonitor(adc1_handle, cali_handle,
                                                 BATTERY_ADC_CHANNEL,
                                                 BATTERY_EST_UPPER_RESISTOR,
                                                 BATTERY_EST_LOWER_RESISTOR,
                                                 GPIO_NUM_NC);
    }

    /* 包 260915-m1-n1b-p1-home-battery：单次电量读（参考树 03_ADC_Test
     * Adc_GetBatteryLevel 只读重写）：raw -> 校准 mV -> VBAT = mV*3/1000 ->
     * [3.0, 4.12]V 线性映射 0..100。读失败置 vbat_status=false 并返回 false
     * （调用方如实报无电量数据，不画假值）。 */
    bool ReadBatteryLevel(int *level_out) {
        int raw = 0;
        int mv = 0;
        esp_err_t err = adc_oneshot_read(adc1_handle, BATTERY_ADC_CHANNEL, &raw);
        if (err == ESP_OK) {
            err = adc_cali_raw_to_voltage(cali_handle, raw, &mv);
        }
        if (err != ESP_OK) {
            vbat_status = false;
            return false;
        }
        float vol = 0.001f * mv * BATTERY_VOLTAGE_DIVIDER;
        int level;
        if (vol <= BATTERY_VOLTAGE_MIN_V) {
            level = 0;
        } else if (vol >= BATTERY_VOLTAGE_MAX_V) {
            level = 100;
        } else {
            level = static_cast<int>(((vol - BATTERY_VOLTAGE_MIN_V) /
                                      (BATTERY_VOLTAGE_MAX_V - BATTERY_VOLTAGE_MIN_V)) * 100.0f);
        }
        vbat_status = true;
        if (level_out != NULL) {
            *level_out = level;
        }
        return true;
    }

    /* K5（260922-m3-k5-sntp-time-sync）[S3]：SNTP 对好的系统时间写回 PCF85063（断电
     * 由纽扣电池续航）。前置双门 = [S1] 单源: 先读 RTC OS 位（ReadRtcTime 不掩掉）,
     * kidpal_ts_write_decision(os_bit, year_ok(系统时间)) 放行才写（OS 停振不写回等
     * SNTP 降级; 系统时间未同步 tm_year:70 态 year_ok=0 同样不写）。由周期 task 在
     * SNTP 同步成功旗标置位后调用（事件驱动写入, 周期节拍消费）。 */
    void SyncSystemTimeToRtc() {
        int h = 0, m = 0, s = 0, mday = 0, mon = 0, wday = -1, os_bit = 0;
        if (!ReadRtcTime(&h, &m, &s, &mday, &mon, &wday, &os_bit)) {
            return; /* 读失败（I2C）不打扰, 下个周期重试 */
        }
        struct tm tm_cur = {};
        time_t now = time(NULL);
        localtime_r(&now, &tm_cur);
        if (!kidpal_ts_write_decision(os_bit, kidpal_ts_year_ok(tm_cur.tm_year))) {
            char corr_id[24];
            snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                     static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
            ESP_LOGW(TAG, "rtc_writeback corr_id=%s rejected_by_write_decision os=%d "
                          "sys_year=%d (不把坏时间当权威, 等 SNTP)", corr_id, os_bit,
                     tm_cur.tm_year + 1900);
            return;
        }
        if (WriteRtcTime(tm_cur.tm_hour, tm_cur.tm_min, tm_cur.tm_sec,
                         tm_cur.tm_mday, tm_cur.tm_wday, tm_cur.tm_mon + 1)) {
            char corr_id[24];
            snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                     static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
            ESP_LOGI(TAG, "rtc_writeback corr_id=%s ok rtc=%02d:%02d:%02d date=%02d-%02d",
                     corr_id, tm_cur.tm_hour, tm_cur.tm_min, tm_cur.tm_sec,
                     tm_cur.tm_mon + 1, tm_cur.tm_mday);
        }
    }

    /* K5（260922-m3-k5-sntp-time-sync）[S4]：周期重读 RTC + 重注入（F2 修——现状
     * boot 一次 ReadRtcTime -> inject 即冻结快照, P1 时间不走的缺陷收口）。重注入经
     * Application::Schedule 派发到主事件循环 task 执行, 与 boot 注入（构造期主 task）
     * 同一任务上下文, 渲染态写点无跨任务竞争; 60s 周期与 P1 分钟级时间粒度协调, 不
     * 跳字/不撕裂（plan 边角 F2）。 */
    void ReinjectRtcTime() {
        int h = 0, m = 0, s = 0, mday = 0, mon = 0, wday = -1, os_bit = 0;
        if (!ReadRtcTime(&h, &m, &s, &mday, &mon, &wday, &os_bit)) {
            return; /* 读失败保持上一次注入值（渲染节点保守默认, 不画假值） */
        }
        auto& app = Application::GetInstance();
        app.Schedule([h, m, mon, mday, wday]() {
            kidpal_render_node_inject_rtc(h, m, mon, mday);
            kidpal_render_node_inject_weekday(wday); /* M3-K2 今天列数据源同步续鲜 */
        });
        char corr_id[24];
        snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                 static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
        ESP_LOGI(TAG, "rtc_reinject corr_id=%s rtc=%02d:%02d date=%02d-%02d wday=%d",
                 corr_id, h, m, mon, mday, wday);
    }

    /* K5 周期时间维护 task 体：分钟对齐节拍消费 SNTP 同步旗标（写回 [S3]）+ 周期重读
     * 重注入（[S4]）。独立 task 承载阻塞 I2C 事务, 不占 esp_timer task（render_node
     * 1s tick 同住, 勿互卡）。 */
    /* R4-1（260922-m3-k5-sntp-time-sync, human 拍板 d2148f66）: 边界后注入偏移常量。
     * 落点裁定 = 板文件（设备时序常量随消费点; time_sync.h 保持 [S1]/[S6] 纯逻辑单源,
     * 零设备语义）。值 = human 拍板定值 2000。 */
#define KIDPAL_TS_INJECT_AFTER_BOUNDARY_MS 2000

    void TimeSyncPeriodicLoop() {
        for (;;) {
            /* R4-1（260922-m3-k5-sntp-time-sync, human 拍板 d2148f66, R3-2 相位修订）:
             * 节拍相位由 [S6] 分钟对齐缝计算——基础延迟 = 距下一墙上分钟边界（epoch ms,
             * CST-8 整分偏移下本地/UTC 分界同 :00）的毫秒数, 再加边界后偏移常量
             * KIDPAL_TS_INJECT_AFTER_BOUNDARY_MS(2000)——唤醒恒落在边界后 ~2s, 远离
             * pdMS_TO_TICKS 向下取整的 tick 量化 (~10ms) 与板级 I2C 读 RTC 耗时窗,
             * 结构性消灭「错过边界 -> 恒慢 60s」双稳态缺陷; 最坏显示滞后 = 2s + 1s
             * 重渲 tick ≈ 3s（human 明示接受上限）。R3 裸形态（纯函数无偏移, 唤醒可
             * 落边界前注入旧分钟值）退役。 */
            struct timeval tv;
            gettimeofday(&tv, NULL);
            long long now_ms = (long long)tv.tv_sec * 1000LL + tv.tv_usec / 1000;
            long long base_ms = kidpal_ts_ms_to_next_minute(now_ms); /* [S6] 防御态 -1 */
            if (base_ms <= 0) {
                base_ms = 60000; /* now<0 时钟未就绪兜底, 下轮重算 */
            }
            long long delay_ms = base_ms + KIDPAL_TS_INJECT_AFTER_BOUNDARY_MS;
            vTaskDelay(pdMS_TO_TICKS((uint32_t)delay_ms));
            if (s_ts_sntp_synced) {
                s_ts_sntp_synced = 0;
                SyncSystemTimeToRtc();
            }
            ReinjectRtcTime();
        }
    }

    static void TimeSyncPeriodicTaskTramp(void* arg) {
        static_cast<CustomBoard*>(arg)->TimeSyncPeriodicLoop();
    }

    void InitializeButtons() {
        /* 冻结解析器（key_gesture_parser）设备侧首次接线：KEY/BOOT 两实例各一。parser 配置
         * 取 iot_button 实际时序（消抖 = DEBOUNCE_TICKS*PERIOD = 2*5=10ms、长按 =
         * CONFIG_BUTTON_LONG_PRESS_TIME_MS=1500、双击窗沿用冻结 session_logic 基准 350ms），
         * 使冻结纯逻辑在设备侧产出手势与物理按键语义一致。 */
        key_gesture_config_t key_cfg = {};
        key_cfg.debounce_ms = CONFIG_BUTTON_DEBOUNCE_TICKS * CONFIG_BUTTON_PERIOD_TIME_MS;
        key_cfg.click_max_ms = CONFIG_BUTTON_LONG_PRESS_TIME_MS;
        key_cfg.double_gap_ms = 350u;
        key_gesture_parser_init(&boot_gesture_, &key_cfg);
        key_gesture_parser_init(&key_gesture_, &key_cfg);

        /* raw 电平边沿喂入（OnPressDown/OnPressUp 已在 iot_button 消抖后触发）：KEY 边沿喂
         * KEY_ID_KEY、BOOT 边沿喂 KEY_ID_BOOT。parser 产出仅作一致性观测（keya_log_gesture，
         * K6 corr_id 日志），手势->动作 dispatch 唯一落在下方 iot_button 手势回调，避免
         * iot_button 短按窗(180ms) 与 parser 双击窗(350ms) 消歧时序分歧导致重复触发。 */
        boot_button_.OnPressDown([this]() {
            /* M2K3-A3 驱动半（seam [9]e, requirements v1.43「计时随任意按键重计」）：任意键
             * 按下即归零查字窗 5 分钟驻留计时（按下边沿覆盖全部手势——单击/长按/双击皆是
             * 按下的超集，故两个键的 OnPressDown 即「任意按键」的完整入口）。 */
            kidpal_render_node_char_window_key_hit();
            (void)key_gesture_parser_feed(&boot_gesture_, KEY_ID_BOOT, keya_now_ms(), 1);
        });
        boot_button_.OnPressUp([this]() {
            key_gesture_t g = key_gesture_parser_feed(&boot_gesture_, KEY_ID_BOOT, keya_now_ms(), 0);
            keya_log_gesture(KEY_ID_BOOT, g);
        });
        key_button_.OnPressDown([this]() {
            kidpal_render_node_char_window_key_hit(); /* 任意键（KEY）按下 -> 驻留计时重计 */
            (void)key_gesture_parser_feed(&key_gesture_, KEY_ID_KEY, keya_now_ms(), 1);
        });
        key_button_.OnPressUp([this]() {
            key_gesture_t g = key_gesture_parser_feed(&key_gesture_, KEY_ID_KEY, keya_now_ms(), 0);
            keya_log_gesture(KEY_ID_KEY, g);
        });

        /* BOOT 单击（去对话化，KR-A4）：不再直连 key_route_global 硬路由（对账 ① 根因
         * 拆除, 260915-m1-k1a KD-A1）——统一经 layer-aware 分流进入：覆盖层顶层先交顶层
         * 覆盖层裁决函数 kidpal_reminder_key（DISMISS = 关层回原页、不产翻页动作, v1.39
         * 隔离口径）；未消费落 key_route_layer（BOOT 单击：覆盖层->OVERLAY_CLOSE /
         * 基层->PAGE_NEXT 翻页, 主页面 BOOT 单击翻页路径零回归）。开机 starting 态进配网
         * 分支保留原位（最小改动、勿引入新行为；spec §5.3「BOOT 长按=配网」与现状「BOOT
         * 单击 starting 态=配网」的既有漂移冻结齿未钉，零触碰）。 */
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            int overlay = keya_front_route_top();
            /* P3 星星记录本页内手势族前置分流（260924-m4-k2; 删除覆盖层吞全手势 +
             * 基层 P3 页内特例; LIST 态 BOOT 单击 = 挂起 300ms 消歧光标下移, 绝不
             * 落全局翻页——「列表态无按键翻页出口」v1.29 口径）。 */
            if (keya_story_page_wants(overlay) &&
                McpServer::GetInstance().HandleStoryKey(KEY_ID_BOOT, KEY_GESTURE_CLICK)) {
                return;
            }
            if (overlay && keya_overlay_consume(KEY_ID_BOOT, KEY_GESTURE_CLICK)) {
                return; /* 顶层覆盖层已消费 BOOT（关层回原页、不翻页） */
            }
            keya_dispatch_global_action(key_route_layer(KEY_ID_BOOT, KEY_GESTURE_CLICK, overlay));
        });
        /* BOOT 长按: P3 页内手势族前置分流（260924-m4-k2; LIST 态 BOOT 长按 = 删除
         * 确认子态入口 DELETE_ENTER, 基层 P3 之外的页保持原 no-op 净效果——旧输入表
         * 对非 P3 页返回 GLOBAL 未被消费）。 */
        boot_button_.OnLongPress([this]() {
            int overlay = keya_front_route_top();
            if (keya_story_page_wants(overlay) &&
                McpServer::GetInstance().HandleStoryKey(KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS)) {
                return;
            }
            McpServer::GetInstance().HandleStoryKey(KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS);
        });
        /* BOOT 双击: P3 页内手势族前置分流（260924-m4-k2 对账③: 「翻下一屏落该屏
         * 首项」语义实现侧首次接线; 既有「一键过渡接线已移除」口径保持——基层非 P3
         * 页仍无语义）。 */
        boot_button_.OnDoubleClick([this]() {
            int overlay = keya_front_route_top();
            if (keya_story_page_wants(overlay) &&
                McpServer::GetInstance().HandleStoryKey(KEY_ID_BOOT, KEY_GESTURE_DOUBLE_CLICK)) {
                return;
            }
        });

        /* KEY 单击：重点对话（layer-aware 分流）。覆盖层顶层先交顶层裁决函数——提醒弹窗
         * KEY 单击 = 关闭回当前页，DISMISS 即消费；未消费落 key_route_layer（KEY 单击：
         * 覆盖层/基层均 SESSION_TOGGLE，会话主轴零回归）。 */
        key_button_.OnClick([this]() {
            int overlay = keya_front_route_top();
            /* P3 星星记录本页内手势族前置分流（260924-m4-k2; LIST 态 KEY 单击 =
             * 记录语境会话语义位 REC_SESSION 挂起消歧, K3 接线消费——绝不落全局
             * SESSION_TOGGLE）。 */
            if (keya_story_page_wants(overlay) &&
                McpServer::GetInstance().HandleStoryKey(KEY_ID_KEY, KEY_GESTURE_CLICK)) {
                return;
            }
            if (overlay && keya_overlay_consume(KEY_ID_KEY, KEY_GESTURE_CLICK)) {
                return;
            }
            keya_dispatch_global_action(key_route_layer(KEY_ID_KEY, KEY_GESTURE_CLICK, overlay));
        });
        /* KEY 双击：注册接缝（KR-A1 三手势全注册），layer-aware 路由（覆盖层/基层双击
         * 均 GLOBAL_ACT_NONE，全局层无语义）。P3 页内前置分流（260924-m4-k2: LIST 态
         * KEY 双击 = 进阅读态 OPEN_READER——「KEY 双击看」实现侧首次接线）。 */
        key_button_.OnDoubleClick([this]() {
            int overlay = keya_front_route_top();
            if (keya_story_page_wants(overlay) &&
                McpServer::GetInstance().HandleStoryKey(KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK)) {
                return;
            }
            keya_dispatch_global_action(key_route_layer(KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK, overlay));
        });
        /* KEY 长按：返回上一层（v1.36 layer-aware 裁决, 260915-m1-k1a KD-A3；覆盖层顶层
         * 先交顶层裁决函数——提醒弹窗 KEY 长按 = 关层回原页，DISMISS 即消费；未消费落
         * key_route_layer：覆盖层->OVERLAY_CLOSE 返回上一层 / 基层->HOME 回首页，原
         * GLOBAL_ACT_HOME->show_home 硬路由改为 layer-aware 裁决，主页面 KEY 长按回首页
         * 映射零回归）。P3 页内前置分流（260924-m4-k2: LIST 态 KEY 长按 = 回首页
         * GLOBAL 交全局（列表 = P3 顶层一层到家）/ READER 态 = 退出阅读回列表）。 */
        key_button_.OnLongPress([this]() {
            int overlay = keya_front_route_top();
            if (keya_story_page_wants(overlay) &&
                McpServer::GetInstance().HandleStoryKey(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS)) {
                return;
            }
            if (overlay && keya_overlay_consume(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS)) {
                return;
            }
            keya_dispatch_global_action(key_route_layer(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS, overlay));
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

    /* 包 261016-sd-card-mount (TF 卡接入——SDMMC 1-bit 挂载 /sdcard 能力就绪, 降级
     * 不崩): 启动期挂载 TF 卡到 SD_MMC_MOUNT_ROOT。卡在位 -> 挂载成功 + 卡信息打印
     * (sdmmc_card_print_info, 真机 A1 观察点) + s_sd_mounted 置真; 无卡/坏卡/挂载
     * 失败 -> ESP_ERROR_CHECK_WITHOUT_ABORT 承接 (只打印错误码, 绝不 panic 重启) +
     * 置 s_sd_mounted=false + WARN 继续, 不阻断启动、不碰语音主链路 (真机 A2 无卡
     * 观察点)。1-bit 模式 (本板只引出 D0); format_if_mount_failed=false = 坏卡/异
     * 格式不销毁数据、只报错降级 (对偶 InitializeArt 既有范式)。热插拔检测第一版
     * 不做。参考 = 官方例程 sdcard_bsp.cpp (只读参考重写, 不直接改参考树)。 */
    void InitializeSdCard() {
        sdmmc_host_t host = SDMMC_HOST_DEFAULT();
        sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
        slot_config.clk   = SD_MMC_CLK_GPIO;   /* GPIO_NUM_38 */
        slot_config.cmd   = SD_MMC_CMD_GPIO;   /* GPIO_NUM_21 */
        slot_config.d0    = SD_MMC_D0_GPIO;    /* GPIO_NUM_39 */
        slot_config.width = 1;                 /* 1-bit: 本板只引出 D0 (SDMMC 四线需 D1-D3) */

        esp_vfs_fat_sdmmc_mount_config_t mount_config = {};
        mount_config.format_if_mount_failed = false; /* 坏卡/异格式不销毁数据, 只报错降级 */
        /* 包 261018-sd-log-persist: 5 -> 8 —— FD 预算给诊断日志文件 + story_sd_io +
         * 开机 SD 探针留头（单写者纪律：日志文件只由诊断写盘任务持有）。 */
        mount_config.max_files              = 8;

        sdmmc_card_t *card = NULL;
        esp_err_t ret = esp_vfs_fat_sdmmc_mount(SD_MMC_MOUNT_ROOT, &host, &slot_config,
                                                &mount_config, &card);
        /* 降级形态: 失败不 abort (WITHOUT_ABORT 只打印错误码, 不 panic 重启); 随后按
         * 返回码/卡句柄判空置板级降级标志, 继续启动不阻断。 */
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        if (ret != ESP_OK || card == NULL) {
            s_sd_mounted = false;
            ESP_LOGW(TAG, "SD card mount failed at %s (ret=%s, card=%p) -> /sdcard "
                     "unavailable, continue", SD_MMC_MOUNT_ROOT, esp_err_to_name(ret),
                     (void *)card);
            return;
        }
        s_sd_mounted = true;
        sdmmc_card_print_info(stdout, card);
        ESP_LOGI(TAG, "SD card mounted at %s (SDMMC 1-bit)", SD_MMC_MOUNT_ROOT);
        /* 包 261018-sd-log-persist: 仅挂载成功分支 attach 诊断日志落盘链——无卡 /
         * 挂载失败在上方降级 return 已退出, 不会走到这里（零 fopen）。attach 只置
         * 可 drain 标志、不清早启动累积的 PSRAM 环形缓冲, 并打开 / 轮转
         * /sdcard/logs/KPLOGnnn.TXT。 */
        KidpalSdLogAttach(SD_MMC_MOUNT_ROOT);
        /* 包 261016-sd-card-mount: 挂载成功路径一次性开机 roundtrip 探针调用点
         * (SD-A5 真机 live-manual 观察触发面, 见 RunBootRoundtripProbe 头注)。
         * 仅成功路径执行——无卡/挂载失败的降级 return 分支 (:856-862) 不跑;
         * 探针内部失败只记日志、继续正常启动, 不改 s_sd_mounted (挂载成功即成功)。 */
        RunBootRoundtripProbe();
    }

    /* 包 261016-sd-card-mount: 一次性开机 SD roundtrip 探针 —— **本包测试义务**
     * (冻结验收 SD-A5 真机 live-manual 条目的观察触发面), **非业务消费**。SD-A5
     * 要求 human 持板在真机观察「/sdcard/SDTEST.TXT 写-读-删 roundtrip 读回与写入
     * 一致、验后即删不驻留」; 但本包自身零消费方(无 MCP 工具/串口命令/开机探针
     * 触发文件读写), 该齿在真机上无触发面, human 无法观察 —— 故在挂载成功路径加
     * 本探针给 human 一个可观察点。human 裁定留痕 = pod-events eventId db8b1406
     * (「齿是冻结权威, 不回调验收; 补最小实现满足冻结齿」)。
     *
     * 临时测试探针, 遵守决策 24 介质禁令口径: 不写业务/身份/事项状态数据, 固定
     * 已知字节序列、**验后即删不驻留**(任一失败步亦尽力 remove 清场)。8.3 大写名
     * (SDTEST.TXT) 受 LFN 关闭约束 (CONFIG_FATFS_LFN_NONE, human 触点①拍板)。
     *
     * 形态: 走 VFS 标准 C 文件 I/O (fopen/fwrite/fread/fclose/remove), 每步判返回
     * 值。**严禁裸 ESP_ERROR_CHECK(** (SD-A3 负向齿: 函数窗内 panic 形态计数=0);
     * 探针内部任何失败只 ESP_LOGW 记失败步 + errno, 继续正常启动、绝不 panic、不
     * 阻断。结果进串口日志: 通过 = ESP_LOGI "SD ROUNDTRIP PASS"; 任一步失败 =
     * ESP_LOGW "SD ROUNDTRIP FAIL ..." 带失败步 (write/read/compare/delete)。 */
    void RunBootRoundtripProbe() {
        static const char kProbePath[] = SD_MMC_MOUNT_ROOT "/SDTEST.TXT"; /* 8.3 大写名 */
        static const char kPayload[] =
            "KIDPAL-SD-ROUNDTRIP-PROBE-0123456789-ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const size_t payload_len = sizeof(kPayload) - 1; /* 不含结尾 NUL */
        char corr_id[24];
        snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                 static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));

        /* step write: 建/截断 8.3 探针文件并写入固定已知字节序列。 */
        FILE *f = fopen(kProbePath, "wb");
        if (f == NULL) {
            ESP_LOGW(TAG, "SD ROUNDTRIP FAIL step=write path=%s corr_id=%s errno=%d",
                     kProbePath, corr_id, errno);
            return;
        }
        size_t written = fwrite(kPayload, 1, payload_len, f);
        int close_rc = fclose(f);
        if (written != payload_len || close_rc != 0) {
            ESP_LOGW(TAG, "SD ROUNDTRIP FAIL step=write path=%s corr_id=%s "
                          "written=%u/%u fclose=%d errno=%d",
                     kProbePath, corr_id, (unsigned)written, (unsigned)payload_len,
                     close_rc, errno);
            remove(kProbePath); /* 尽力清场, 不留残件 (验后即删义务) */
            return;
        }

        /* step read: 读回逐字节比对。 */
        f = fopen(kProbePath, "rb");
        if (f == NULL) {
            ESP_LOGW(TAG, "SD ROUNDTRIP FAIL step=read path=%s corr_id=%s errno=%d",
                     kProbePath, corr_id, errno);
            remove(kProbePath);
            return;
        }
        char rb[128] = {};
        size_t nread = fread(rb, 1, sizeof(rb), f);
        fclose(f);
        if (nread != payload_len) {
            ESP_LOGW(TAG, "SD ROUNDTRIP FAIL step=read path=%s corr_id=%s "
                          "read=%u/%u errno=%d",
                     kProbePath, corr_id, (unsigned)nread, (unsigned)payload_len, errno);
            remove(kProbePath);
            return;
        }
        int mismatch = -1;
        for (size_t i = 0; i < payload_len; i++) {
            if (rb[i] != kPayload[i]) {
                mismatch = static_cast<int>(i);
                break;
            }
        }
        if (mismatch >= 0) {
            ESP_LOGW(TAG, "SD ROUNDTRIP FAIL step=compare path=%s corr_id=%s "
                          "byte_off=%d got=0x%02x want=0x%02x",
                     kProbePath, corr_id, mismatch,
                     (unsigned char)rb[mismatch], (unsigned char)kPayload[mismatch]);
            remove(kProbePath);
            return;
        }

        /* step delete: 验后即删, 探针文件不驻留。 */
        int del_rc = remove(kProbePath);
        if (del_rc != 0) {
            ESP_LOGW(TAG, "SD ROUNDTRIP FAIL step=delete path=%s corr_id=%s rc=%d errno=%d",
                     kProbePath, corr_id, del_rc, errno);
            return;
        }
        ESP_LOGI(TAG, "SD ROUNDTRIP PASS path=%s corr_id=%s bytes=%u "
                      "(write-read-compare-delete ok, 验后即删不驻留)",
                 kProbePath, corr_id, (unsigned)payload_len);
    }

    /* 包 261016-sd-card-mount: 板级查询入口 (本包零消费方, 供将来消费方判 /sdcard
     * 能力就绪; 防死代码误删观察锚)。读启动期挂载标志返回。 */
public:
    /* 包 261016-p3-story-body-sd-cache: 提为 public + override（Board 基类
     * virtual SdCardMounted 默认 false）——story 正文 SD 只读镜像消费方经基类口
     * 拿挂载标志，不碰本类私有态。 */
    bool SdCardMounted() override { return s_sd_mounted; }

    CustomBoard() : boot_button_(BOOT_BUTTON_GPIO),
                    key_button_(KEY_BUTTON_GPIO, false, 0, CONFIG_BUTTON_SHORT_PRESS_TIME_MS, false) {
        /* key_button_ 构造参数（接缝契约钉死）：
         *   KEY_BUTTON_GPIO = GPIO18；active_high=false（GP18_Active 0 = 低有效，
         *   KR-A5 human 持板真机确认后回填/冻结）；long_press_time=0（沿用 boot_button_）；
         *   short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS=180（消抖复用，与 BOOT 一致，
         *   KR-A1 不另设短按窗）；enable_power_save=false。 */
        InitializeI2c();
        /* 启动回归修复 (顺序层): 先在干净总线上把音频 codec (ES8311/ES7210) 建起来
         * (GetAudioCodec 构造 static BoxAudioCodec, 走 es8311_open 的 I2C 事务), 再跑
         * 传感器开机读。这样传感器 (SHTC3 缺席) 即便超时会发生在 codec 已成功之后, 配合
         * 各读函数失败路径的 RecoverI2cBus, 总线不会被残留 busy 锁死到 codec 事务上。 */
        GetAudioCodec();
        InitializeSensors();
        InitializeBatteryAdc();
        InitializeButtons();
        InitializeTools();
        InitializeLcdDisplay();
        InitializeArt();
        InitializeSdCard();
        /* 归一屏（包 260911-screen-unification SU-RULING-02 单物理屏）：
         * CustomLcdDisplay 构造期 SetupUI() 已把聊天 chrome（状态栏/表情/气泡）建在
         * LVGL 默认首屏上。此处把它们 reparent 到常驻顶层 layer（lv_layer_top，非屏、
         * 恒浮于活动屏之上），此后 display/ 只有 s_kidpal_scr 一个前台屏被加载，聊天
         * 内容任何页可见。必须在 kidpal_render_node_init() 之前：init 创建并首载
         * s_kidpal_scr，把原默认首屏留在后面不再加载。 */
        display_->KidPalReparentChatToLayer(lv_layer_top());
        /* 任务 B（包 260915-m1-n1b-p1-home-battery）：联网小标与温/湿/电辅助条小标同档。
         * 基座 SetupUI 初帧把 network_label_ 落 icon_font=font_awesome_30_4（30px，~2x 辅助条
         * ~13px 图标墨高）；构造期压到 font_awesome_14_1（14px），真机小标同档。断网斜杠
         * 链路（GetNetworkStateIcon/PH-A5）与 home_p1 帧缓冲面均不动。 */
        display_->KidPalSetNetworkLabelFont(&font_awesome_14_1);
        /* 包 9b HWV-A6: 渲染节点在 LVGL 与 art SPIFFS 均就绪后初始化。
         * init() 创建 s_kidpal_scr（不渲染，无 I/O）；inject 注入开机 RTC/温湿度；
         * show_home() 渲染首页 P1 帧并加载到 RLCD。 */
        kidpal_render_node_init();
        kidpal_render_node_inject_rtc(s_rtc_h, s_rtc_m, s_rtc_mon, s_rtc_mday);
        /* M3-K2 星期注入缝（inject_rtc/inject_env 同一 RTC 装配点, LVGL 就绪后）:
         * 板级 PCF85063 0x08 星期寄存器 -> 渲染节点注入缝, P2 网格今天列推导数据源。
         * 读失败/非法值保持 -1（渲染节点越域忽略, 不画假「今天」高亮）。 */
        kidpal_render_node_inject_weekday(s_rtc_wday);
        kidpal_render_node_inject_env(s_env_ok, s_env_temp, s_env_rh);
        /* 包 260915-m1-n1b-p1-home-battery 电量装配点（seam [2]）：板级 ADC 读数 ->
         * 渲染节点电池注入缝，inject_rtc/inject_env 同一装配点（LVGL 就绪后）。
         * 读失败不注入（渲染节点保守默认保持），不画假值。 */
        {
            char corr_id[24];
            snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                     static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
            int bat_pct = 0;
            bool chg = battery_monitor_->IsCharging();
            if (ReadBatteryLevel(&bat_pct)) {
                kidpal_render_node_inject_battery(bat_pct, chg ? 1 : 0);
                ESP_LOGI(TAG, "battery_inject corr_id=%s level=%d charging=%d vbat_ok=%d",
                         corr_id, bat_pct, chg ? 1 : 0, vbat_status ? 1 : 0);
            } else {
                ESP_LOGW(TAG, "battery_inject corr_id=%s read_deferred vbat_ok=%d "
                              "(ADC 读失败, 电量保守默认保持)", corr_id,
                         vbat_status ? 1 : 0);
            }
        }
        kidpal_render_node_show_home();
        /* K5（260922-m3-k5-sntp-time-sync）：周期时间维护 task（[S3] SNTP 成功写回
         * PCF85063 + [S4] 周期重读 RTC 重注入修 F2 boot 快照冻结）。须在 LVGL 渲染
         * 节点就绪后启动（注入走 Application::Schedule 主循环, 首个 tick 在 +60s）。 */
        xTaskCreate(TimeSyncPeriodicTaskTramp, "ts_periodic", 4096, this, 1, nullptr);
   }

    /* 包 260915-m1-n1b-p1-home-battery：Board::GetBatteryLevel 基座默认 return false
     * 的现状销账——chrome (lvgl_display UpdateStatusBar) 每 10s 轮询电量显示真值。
     * 显示电量 = ReadBatteryLevel（官方 Demo 已验证映射）；充电态 = AdcBatteryMonitor
     * 趋势检测（顺带消费估计库采样窗喂趋势缓冲），PH-A6 真机轮回读充电停闪表现。 */
    virtual bool GetBatteryLevel(int& level, bool& charging, bool& discharging) override {
        int pct = 0;
        (void)ReadBatteryLevel(&pct); /* 置 vbat_status = 最近一次读有效性 */
        (void)battery_monitor_->GetBatteryLevel(); /* 喂估计库采样窗（趋势引擎） */
        charging = battery_monitor_->IsCharging();
        discharging = !charging;
        level = pct;
        return vbat_status; /* 读失败 = 如实报无电量数据（基座 false 语义） */
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