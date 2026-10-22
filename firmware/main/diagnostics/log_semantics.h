/*
 * log_semantics.h — 板端诊断日志的语义层纯逻辑单源（TaskPackage 261022-diag-log-content）。
 *
 * 契约权威 = firmware/main/test/diag_log.acceptance.c 文件头 [DLG-C1]/[DLG-C2]/[DLG-C3]/[DLG-C4]⑥
 * （冻结，逐字）。本头只做两件事：
 *   1) 冻结数值单源两宏（时间前缀容量 / SystemInfo 降频间隔）——行为断言用冻结件镜像
 *      常量，[DLG-C4]⑥ 扫本头宏字面把等值钉死（EQ-2）；
 *   2) 纯逻辑缝声明——实现家 diagnostics/log_semantics.c，**零芯片依赖 / 零 ESP-IDF
 *      include / 零 FreeRTOS / 零 LVGL / 零 FATFS**，host_app linux target 与设备树
 *      双侧同源（设备侧经 firmware/main/CMakeLists.txt、host 侧经 host_app/main/
 *      CMakeLists.txt 各自登记）。
 *
 * 设计约束（R1，plan 最大风险）：esp_log vprintf 钩子可在任意任务 / ISR 上下文被调用
 * （sd_log.acceptance.c [C1] :50 明示）。故本头的时间面拆成两半——
 *   - 产出方 = 低频任务（application.cc main event loop 1s clock tick）调
 *     kidpal_log_time_cache_update，把墙上时间预格式化成前缀串进缓存；
 *   - 消费方 = 钩子只调 kidpal_log_time_cache 取现成串 memcpy 拼接，**绝不做**
 *     localtime_r / snprintf（拖延串口输出或重入 getAlloc 的风险面）。
 * kidpal_log_time_format 本体是纯决策函数（字段显式注入、无真实时钟、无全局态），
 * 只由产出方经 update 间接使用，钩子路径不触。
 */

#ifndef KIDPAL_DIAGNOSTICS_LOG_SEMANTICS_H
#define KIDPAL_DIAGNOSTICS_LOG_SEMANTICS_H

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- *
 * [DLG-C1]/[DLG-C3] 冻结数值单源（[DLG-C4]⑥ 扫本头宏字面，EQ-2）
 * ------------------------------------------------------------------------ */

/* 时间前缀缓冲容量：[DLG-C1] "[2026-10-22 12:34:56] " = 22 字符 + NUL；BOOT 形态
 * "[BOOT+<ms>ms] " 最长 = 6 + 20 位 ULL 十进制 + 4 = 30 + NUL = 31，32 足容。 */
#define KIDPAL_LOG_TIME_PREFIX_CAP    32u
/* SystemInfo 打印间隔：[DLG-C3] 10s -> 60s（原始痛点 = SRAM 每 10s 一条噪声占篇幅
 * 70%）。间隔数值单源，调用点不得裸写字面。 */
#define KIDPAL_LOG_SYSINFO_INTERVAL_MS 60000LL

/* -------------------------------------------------------------------------- *
 * [DLG-C1] 墙上时间前缀（纯逻辑；墙上时间字段**显式注入**——真实时钟读取归芯片面）
 * ------------------------------------------------------------------------ */

/* 格式化决策单源（无全局态、无真实时钟）：
 *   synced != 0 -> "[YYYY-MM-DD HH:MM:SS] "（year 完整年份如 2026；mon 1..12 /
 *       mday 1..31 / hour 0..23 / min 0..59 / sec 0..59；**任一字段越界 -> -1 且零写入**
 *       （防御：不产出假时间）；year 取 [2000, 9999]——2000 前年份是坏 RTC / 坏 SNTP
 *       信号，抓坏时间落卡会污染全部下游对账；上界保 22 字符定宽不变量）；
 *   synced == 0 -> "[BOOT+<boot_ms>ms] "（SNTP/RTC 未同步降级标注，boot_ms = 开机毫秒数，
 *       无上限、不校验其余字段）；
 *   成功 -> 写入字符数（不含 NUL）；out == NULL 或 cap 不足 -> -1（零写入）。 */
int kidpal_log_time_format(int synced, int year, int mon, int mday,
                           int hour, int min, int sec,
                           unsigned long long boot_ms, char *out, unsigned cap);

/* 预格式化缓存产出（R1 缓解的**产出半**）：低频任务（application.cc 1s clock tick）
 * 调用；内部复用 kidpal_log_time_format 的决策（单源一致）。格式化失败（如时间字段
 * 越界）-> 缓存降级为 ""（无前缀，绝不产出假时间）。 */
void kidpal_log_time_cache_update(int synced, int year, int mon, int mday,
                                  int hour, int min, int sec,
                                  unsigned long long boot_ms);

/* 取当前缓存前缀（钩子只 memcpy 拼接用）：**恒非 NULL**；未 update 过 -> ""（空串 =
 * 降级为无前缀，日志仍可读，绝不返回 NULL 崩钩子）。双缓冲 + 原子索引交换实现——
 * 读侧拿到的永远是某一次完整 update 的成品串（不撕裂），最坏是上一拍的前缀（陈旧
 * 一格，秒级可接受）。真并发一致性归芯片面 + 真机轮 DLG-A6③（GAP-1）。 */
const char *kidpal_log_time_cache(void);

/* -------------------------------------------------------------------------- *
 * [DLG-C2] 语义名映射（纯逻辑闭集 + fallback；int 入参解耦枚举头——host 侧不 include
 * key_gesture_parser.h / kidpal_render_node.h，映射表覆盖全部已知枚举值，越界回退
 * 串形态 "<族>_<n>"）。全部**恒非 NULL**。
 * ------------------------------------------------------------------------ */

/* global_action_t（key_gesture_parser.h）0..5 ->
 * "none" / "session_toggle" / "home" / "page_next" / "overlay_close" / "char_replay";
 * 其余（含负值）-> "act_<n>"。 */
const char *kidpal_log_action_name(int act);
/* key_id_t 0..1 -> "key" / "boot"; 其余 -> "key_<n>"。 */
const char *kidpal_log_key_name(int key);
/* key_gesture_t 0..3 -> "none" / "click" / "double_click" / "long_press";
 * 其余 -> "ges_<n>"。 */
const char *kidpal_log_gesture_name(int g);
/* kidpal_page_id_t（kidpal_render_node.h）0..2 -> "home" / "schedule" / "story";
 * 其余 -> "page_<n>"。 */
const char *kidpal_log_page_name(int page);
/* PAGE_SWITCH 触发源 0..2 -> "key" / "timer" / "net"; 其余 -> "trg_<n>"。 */
const char *kidpal_log_trigger_name(int trg);

/* -------------------------------------------------------------------------- *
 * [DLG-C3] SystemInfo 降频决策（纯逻辑幂等函数；间隔数值单源，禁裸字面）
 * ------------------------------------------------------------------------ */

/* interval <= 0 -> -1（防御：零/负间隔等于每 tick 一条 = 没降频）；
 * now < last -> 0（时钟回退 fail-closed：噪声治理不因回退反弹成风暴）；
 * now - last >= interval -> 1；其余 -> 0。幂等：同入参恒同出（无隐藏状态）。 */
int kidpal_log_sysinfo_due(long long now, long long last, long long interval);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_DIAGNOSTICS_LOG_SEMANTICS_H */
