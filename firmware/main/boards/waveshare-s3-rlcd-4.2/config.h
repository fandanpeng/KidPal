#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define ESP32_I2C_HOST      I2C_NUM_0
#define ESP32_LCD_HOST      SPI3_HOST

#define AUDIO_INPUT_SAMPLE_RATE  24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

#define AUDIO_INPUT_REFERENCE    true

#define AUDIO_I2S_GPIO_MCLK  GPIO_NUM_16
#define AUDIO_I2S_GPIO_WS    GPIO_NUM_45
#define AUDIO_I2S_GPIO_BCLK  GPIO_NUM_9
#define AUDIO_I2S_GPIO_DIN   GPIO_NUM_10
#define AUDIO_I2S_GPIO_DOUT  GPIO_NUM_8

#define AUDIO_CODEC_PA_PIN       GPIO_NUM_46
#define AUDIO_CODEC_I2C_SDA_PIN  GPIO_NUM_13
#define AUDIO_CODEC_I2C_SCL_PIN  GPIO_NUM_14
#define AUDIO_CODEC_ES8311_ADDR  ES8311_CODEC_DEFAULT_ADDR
#define AUDIO_CODEC_ES7210_ADDR  ES7210_CODEC_DEFAULT_ADDR

/* 包 9b HWV-A4 (spec §2 硬件盘点): RTC(PCF85063, I2C 独立备用电池走时) + 温湿度
 * (SHTC3) 挂在同一 I2C 主机 (ESP32_I2C_HOST / SDA=13 / SCL=14)。7-bit 地址：
 * PCF85063 = 0x51, SHTC3 = 0x70（板级绑定见 .cc 的 InitializeSensors 真实读路径）。 */
#define RTC_PCF85063_I2C_ADDR    0x51
#define SHTC3_I2C_ADDR           0x70

#define BOOT_BUTTON_GPIO        GPIO_NUM_0
/* 包 260911-key-route-a1 (spec §4.0/§5.3 两键分工): KEY(GPIO18) 主司对话, 与 BOOT(GPIO0)
 * 主司翻页分脚不抢占。GPIO18 有效电平 = 低（GP18_Active 0，板级键 button_ 构造
 * active_high=false；KR-A5 human 持板真机确认后回填/冻结）。 */
#define KEY_BUTTON_GPIO         GPIO_NUM_18

/* 包 261016-sd-card-mount (TF 卡接入——SDMMC 1-bit 挂载 + /sdcard 能力就绪):
 * SD 卡脚位 + VFS 挂载根。来源 = 官方例程 ESP32-S3-RLCD-4.2-Demo/02_ESP-IDF/
 * 06_SD_Card/components/port_bsp/sdcard_bsp.h:15 默认脚位 (只读参考重写, 不直接改
 * 参考树); 1-bit 模式 (本板只引出 D0, 四线需 D1-D3)。triage 实核 21/38/39 在本板
 * firmware/main 零占用, 无冲突。 */
#define SD_MMC_CLK_GPIO         GPIO_NUM_38
#define SD_MMC_CMD_GPIO         GPIO_NUM_21
#define SD_MMC_D0_GPIO          GPIO_NUM_39
#define SD_MMC_MOUNT_ROOT       "/sdcard"

#define RLCD_DC_PIN    GPIO_NUM_5
#define RLCD_CS_PIN    GPIO_NUM_40
#define RLCD_SCK_PIN   GPIO_NUM_11
#define RLCD_MOSI_PIN  GPIO_NUM_12
#define RLCD_RST_PIN   GPIO_NUM_41
#define RLCD_TE_PIN    GPIO_NUM_6

#define RLCD_WIDTH   400
#define RLCD_HEIGHT  300

#define DISPLAY_MIRROR_X false
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY  false

#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  0

#endif 
