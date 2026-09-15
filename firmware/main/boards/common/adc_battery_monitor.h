#ifndef ADC_BATTERY_MONITOR_H
#define ADC_BATTERY_MONITOR_H

#include <functional>
#include <driver/gpio.h>
#include <adc_battery_estimation.h>
#include <esp_timer.h>

class AdcBatteryMonitor {
public:
    AdcBatteryMonitor(adc_unit_t adc_unit, adc_channel_t adc_channel, float upper_resistor, float lower_resistor, gpio_num_t charging_pin = GPIO_NUM_NC);
    /*
     * 外部句柄构造（包 260915-m1-n1b-p1-home-battery）：板级已持有 ADC oneshot unit +
     * calibration 句柄时复用之（ESP-IDF 对同一 ADC unit 的二次
     * adc_oneshot_new_unit 会返回 ESP_ERR_NOT_FOUND——unit 是排他 claim，内部构造
     * 与板级自有 ADC 初始化不能共存）。估计库经 adc_battery_estimation_t 的
     * external 分支吃外部句柄，不重复创建/销毁 unit。
     */
    AdcBatteryMonitor(adc_oneshot_unit_handle_t adc_handle, adc_cali_handle_t adc_cali_handle,
                      adc_channel_t adc_channel, float upper_resistor, float lower_resistor,
                      gpio_num_t charging_pin = GPIO_NUM_NC);
    ~AdcBatteryMonitor();

    bool IsCharging();
    bool IsDischarging();
    uint8_t GetBatteryLevel();

    void OnChargingStatusChanged(std::function<void(bool)> callback);

private:
    gpio_num_t charging_pin_;
    adc_battery_estimation_handle_t adc_battery_estimation_handle_ = nullptr;
    esp_timer_handle_t timer_handle_ = nullptr;
    bool is_charging_ = false;
    std::function<void(bool)> on_charging_status_changed_;

    void CheckBatteryStatus();
};

#endif // ADC_BATTERY_MONITOR_H
