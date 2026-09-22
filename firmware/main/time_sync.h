/*
 * 纯逻辑时间缝单源（TaskPackage 260922-m3-k5-sntp-time-sync, seam [S1] 冻结签名）。
 *
 * 纪律：C linkage、零芯片依赖、零 ESP-IDF include——host_app linux target 与设备树
 * 同源（接线见 firmware/main/CMakeLists.txt 设备侧 + firmware/main/test/host_app/main/
 * CMakeLists.txt HOST_ACCEPTANCE_SRCS host 侧）。语义权威 = 冻结验收件
 * firmware/main/test/time_sync.acceptance.c 头注 [S1]。
 */
#ifndef KIDPAL_TIME_SYNC_H
#define KIDPAL_TIME_SYNC_H

#ifdef __cplusplus
extern "C" {
#endif

/* 0..99 -> 0x00..0x99（十位<<4|个位）；越界（<0 或 >99）-> -1。 */
int kidpal_ts_bcd_encode(int dec);

/* 两个 nibble 均 <= 9 -> hi*10+lo（0x59 -> 59）；任一 nibble > 9（坏 BCD）-> -1。 */
int kidpal_ts_bcd_decode(int bcd);

/* tm_year-1900 口径 raw >= 125（即 >= 2025 年）-> 1，否则 0。
 * 语义锚 = lvgl_display.cc 守门 `tm_year >= 2025 - 1900` 同阈值。 */
int kidpal_ts_year_ok(int tm_year_raw);

/* 时钟写回/恢复统一可信门（写回 PCF85063 与 boot settimeofday 恢复共用，
 * 不把坏时间当权威）：os_bit_set != 0（PCF85063 oscillator stop = 晶振曾停振）
 * -> 0；year_ok == 0 -> 0；两者皆可信 -> 1。入参按非零=真布尔解释。 */
int kidpal_ts_write_decision(int os_bit_set, int year_ok);

/* NTP 失败退避（不重试风暴）：failures < 0 -> -1（防御）；否则恒 > 0，
 * 随 failures 非递减且有封顶；delay(f=0) >= 1000。 */
int kidpal_ts_backoff_delay_ms(int consecutive_failures);

/* W 日志节流谓词（返回 1 = 抑制本条）：now - last < min_interval -> 1（含相等）；
 * now - last >= min_interval -> 0；now < last（时钟被回拨/校正）-> 0（fail-open）；
 * min_interval <= 0 -> 0（fail-open 恒放行）。 */
int kidpal_ts_log_throttled(long long now_ms, long long last_ms,
                            long long min_interval_ms);

/* [S6] R3 追加轮第七函数（分钟对齐缝）: now_ms < 0 -> -1（防御）; 否则 =
 * 60000 - (now_ms % 60000), 恒在 [1,60000]（恰在分钟边界 -> 60000, 整周期后仍落
 * 边界, 禁 0 延迟忙旋）。入参口径 = 毫秒时钟且其分钟边界与墙上分钟边界对齐
 * （epoch ms 满足）。设备侧消费: TimeSyncPeriodicLoop 节拍相位按此计算。 */
long long kidpal_ts_ms_to_next_minute(long long now_ms);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_TIME_SYNC_H */
