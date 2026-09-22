/*
 * 纯逻辑时间缝单源实现（TaskPackage 260922-m3-k5-sntp-time-sync, seam [S1]）。
 * 零芯片依赖、零 ESP-IDF include——host_app linux target 与设备树同源。
 * 消费者：application.cc（SNTP 退避）、boards/waveshare-s3-rlcd-4.2（写回/恢复）、
 * display/lvgl_display/lvgl_display.cc（W 日志节流）。
 */
#include "time_sync.h"

int kidpal_ts_bcd_encode(int dec)
{
    if (dec < 0 || dec > 99) {
        return -1; /* 非法十进制不得编出假 BCD（寄存器 0x04-0x09 写入域防御） */
    }
    return ((dec / 10) << 4) | (dec % 10);
}

int kidpal_ts_bcd_decode(int bcd)
{
    int hi = (bcd >> 4) & 0x0F;
    int lo = bcd & 0x0F;
    if (hi > 9 || lo > 9) {
        return -1; /* 坏 nibble（寄存器脏位）不得被当合法时间 */
    }
    return hi * 10 + lo;
}

int kidpal_ts_year_ok(int tm_year_raw)
{
    /* 阈值 125 = 2025-1900, 与 lvgl_display.cc 守门同值；raw 70（1970, 真机
     * `tm_year: 70` 负锚点）必须被拒。 */
    return (tm_year_raw >= 125) ? 1 : 0;
}

int kidpal_ts_write_decision(int os_bit_set, int year_ok)
{
    if (os_bit_set != 0) {
        return 0; /* 晶振曾停振（如纽扣电池耗尽）-> 时间不可信, 等 SNTP 降级 */
    }
    if (year_ok == 0) {
        return 0; /* 年份不 plausible -> 坏时间不当权威 */
    }
    return 1;
}

int kidpal_ts_backoff_delay_ms(int consecutive_failures)
{
    if (consecutive_failures < 0) {
        return -1; /* 非法失败计数防御 */
    }
    if (consecutive_failures > 5) {
        consecutive_failures = 5; /* 封顶: 最长 32s, 无界指数禁用（长断网重试有上限） */
    }
    return 1000 << consecutive_failures; /* 1s, 2s, 4s, 8s, 16s, 32s（恒 >= 1s） */
}

int kidpal_ts_log_throttled(long long now_ms, long long last_ms,
                            long long min_interval_ms)
{
    if (min_interval_ms <= 0) {
        return 0; /* fail-open 恒放行（观测通道不失明） */
    }
    if (now_ms < last_ms) {
        return 0; /* 时钟被回拨/校正 -> fail-open 放行 */
    }
    if (now_ms - last_ms < min_interval_ms) {
        return 1; /* 间隔未走完（含相等）-> 抑制 */
    }
    return 0; /* 间隔走满 -> 放行 */
}
