#include "adc_battery_monitor.h"

AdcBatteryMonitor::AdcBatteryMonitor(adc_unit_t adc_unit, adc_channel_t adc_channel, float upper_resistor, float lower_resistor, gpio_num_t charging_pin)
    : charging_pin_(charging_pin) {

    // Initialize charging pin (only if it's not NC)
    if (charging_pin_ != GPIO_NUM_NC) {
        gpio_config_t gpio_cfg = {
            .pin_bit_mask = 1ULL << charging_pin,
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_ERROR_CHECK(gpio_config(&gpio_cfg));
    }

    // Initialize ADC battery estimation
    adc_battery_estimation_t adc_cfg = {};
    adc_cfg.internal.adc_unit = adc_unit;
    adc_cfg.internal.adc_bitwidth = ADC_BITWIDTH_DEFAULT;
    adc_cfg.internal.adc_atten = ADC_ATTEN_DB_12;
    adc_cfg.adc_channel = adc_channel;
    adc_cfg.upper_resistor = upper_resistor;
    adc_cfg.lower_resistor = lower_resistor;

    // 在ADC配置部分进行条件设置
    if (charging_pin_ != GPIO_NUM_NC) {
        adc_cfg.charging_detect_cb = [](void *user_data) -> bool {
            AdcBatteryMonitor *self = (AdcBatteryMonitor *)user_data;
            return gpio_get_level(self->charging_pin_) == 1;
        };
        adc_cfg.charging_detect_user_data = this;
    } else {
        // 不设置回调，让adc_battery_estimation库使用软件估算
        adc_cfg.charging_detect_cb = nullptr;
        adc_cfg.charging_detect_user_data = nullptr;
    }
    adc_battery_estimation_handle_ = adc_battery_estimation_create(&adc_cfg);

    // Initialize timer
    esp_timer_create_args_t timer_cfg = {
        .callback = [](void *arg) {
            AdcBatteryMonitor *self = (AdcBatteryMonitor *)arg;
            self->CheckBatteryStatus();
        },
        .arg = this,
        .name = "adc_battery_monitor",
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_cfg, &timer_handle_));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle_, 1000000));
}

/* 外部句柄构造（包 260915-m1-n1b-p1-home-battery）：与内部构造同体，仅估计库配置走
 * external 分支（复用板级 ADC unit/calibration，不二次 claim）。估计库对
 * adc_battery_estimation_t 无显式 tag——它以 internal.adc_unit < SOC_ADC_PERIPH_NUM
 * 判别内外部；external.adc_handle 是堆指针（ESP32-S3 堆址远大于该枚举值），写入
 * external 成员后该判别自然落 external 分支（估计库头注「Use external ADC handle
 * if provided」的设计用法）。 */
AdcBatteryMonitor::AdcBatteryMonitor(adc_oneshot_unit_handle_t adc_handle, adc_cali_handle_t adc_cali_handle, adc_channel_t adc_channel, float upper_resistor, float lower_resistor, gpio_num_t charging_pin)
    : charging_pin_(charging_pin) {

    if (charging_pin_ != GPIO_NUM_NC) {
        gpio_config_t gpio_cfg = {
            .pin_bit_mask = 1ULL << charging_pin,
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_ERROR_CHECK(gpio_config(&gpio_cfg));
    }

    adc_battery_estimation_t adc_cfg = {};
    adc_cfg.external.adc_handle = adc_handle;
    adc_cfg.external.adc_cali_handle = adc_cali_handle;
    adc_cfg.adc_channel = adc_channel;
    adc_cfg.upper_resistor = upper_resistor;
    adc_cfg.lower_resistor = lower_resistor;

    if (charging_pin_ != GPIO_NUM_NC) {
        adc_cfg.charging_detect_cb = [](void *user_data) -> bool {
            AdcBatteryMonitor *self = (AdcBatteryMonitor *)user_data;
            return gpio_get_level(self->charging_pin_) == 1;
        };
        adc_cfg.charging_detect_user_data = this;
    } else {
        // 无充电检测引脚：软件估算（电压上升趋势 = 充电，估计库
        // CONFIG_BATTERY_STATE_SOFTWARE_ESTIMATION 路径）
        adc_cfg.charging_detect_cb = nullptr;
        adc_cfg.charging_detect_user_data = nullptr;
    }
    adc_battery_estimation_handle_ = adc_battery_estimation_create(&adc_cfg);

    esp_timer_create_args_t timer_cfg = {
        .callback = [](void *arg) {
            AdcBatteryMonitor *self = (AdcBatteryMonitor *)arg;
            self->CheckBatteryStatus();
        },
        .arg = this,
        .name = "adc_battery_monitor",
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_cfg, &timer_handle_));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle_, 1000000));
}

AdcBatteryMonitor::~AdcBatteryMonitor() {
    if (adc_battery_estimation_handle_) {
        ESP_ERROR_CHECK(adc_battery_estimation_destroy(adc_battery_estimation_handle_));
    }
    
    if (timer_handle_) {
        esp_timer_stop(timer_handle_);
        esp_timer_delete(timer_handle_);
    }
}

bool AdcBatteryMonitor::IsCharging() {
    // 优先使用adc_battery_estimation库的功能
    if (adc_battery_estimation_handle_ != nullptr) {
        bool is_charging = false;
        esp_err_t err = adc_battery_estimation_get_charging_state(adc_battery_estimation_handle_, &is_charging);
        if (err == ESP_OK) {
            return is_charging;
        }
    }
    
    // 回退到GPIO读取或返回默认值
    if (charging_pin_ != GPIO_NUM_NC) {
        return gpio_get_level(charging_pin_) == 1;
    }
    
    return false;
}

bool AdcBatteryMonitor::IsDischarging() {
    return !IsCharging();
}

uint8_t AdcBatteryMonitor::GetBatteryLevel() {
    // 如果句柄无效，返回默认值
    if (adc_battery_estimation_handle_ == nullptr) {
        return 100;
    }
    
    float capacity = 0;
    esp_err_t err = adc_battery_estimation_get_capacity(adc_battery_estimation_handle_, &capacity);
    if (err != ESP_OK) {
        return 100; // 出错时返回默认值
    }
    return (uint8_t)capacity;
}

void AdcBatteryMonitor::OnChargingStatusChanged(std::function<void(bool)> callback) {
    on_charging_status_changed_ = callback;
}

void AdcBatteryMonitor::CheckBatteryStatus() {
    bool new_charging_status = IsCharging();
    if (new_charging_status != is_charging_) {
        is_charging_ = new_charging_status;
        if (on_charging_status_changed_) {
            on_charging_status_changed_(is_charging_);
        }
    }
}