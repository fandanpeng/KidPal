#include <esp_log.h>
#include <esp_err.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <esp_event.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "application.h"
#include "system_info.h"
#include "diagnostics/sd_log.h"  /* 包 261018-sd-log-persist: 诊断日志落 TF 卡 */
#include "diagnostics/log_cmd.h" /* 包 261018-sd-log-usb-read: USB 串口按需取回命令任务 */

#define TAG "main"

extern "C" void app_main(void)
{
    /* 包 261018-sd-log-persist: 板端诊断日志最早期接线。顺序约束（冻结契约 [C5]）：
     * ① 先 KidpalSdLogInit()——PSRAM 环形缓冲 + 低优先级写盘任务就绪（缓冲必须先于
     *    钩子存在，钩子可在装置后任意时刻被调用）；
     * ② 再装 esp_log vprintf 钩子并把 esp_log_set_vprintf 返回的旧 vprintf 交回，
     *    由钩子链式转调（串口输出不中断）；
     * ③ 两者都早于首个 nvs_flash_init()——NVS / WiFi 启动期日志进入环形缓冲，待 SD
     *    挂载成功后从最旧条目 drain 落卡。 */
    KidpalSdLogInit();
    KidpalSdLogChainPreviousVprintf(esp_log_set_vprintf(KidpalSdLogVprintf));

    /* 包 261018-sd-log-usb-read: 在 vprintf 钩子链保存之后启动 USB 串口按需取回命令任务
     * （kplog list/tail/cat，只读；无命令零输出）。顺序约束（冻结契约 [S8]）：必须在
     * KidpalSdLogChainPreviousVprintf 之后——提前启动会与钩子安装竞态。 */
    KidpalLogCmdStart();

    // Initialize NVS flash for WiFi configuration
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing NVS flash to fix corruption");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize and run the application
    auto& app = Application::GetInstance();
    app.Initialize();
    app.Run();  // This function runs the main event loop and never returns
}
