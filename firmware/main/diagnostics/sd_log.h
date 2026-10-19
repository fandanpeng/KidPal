/*
 * sd_log.h — 板端诊断日志落 TF 卡的纯逻辑 seam + 冻结数值单源（TaskPackage
 * 261018-sd-log-persist）。
 *
 * 契约权威 = firmware/main/test/sd_log.acceptance.c 文件头 [C1]-[C5]（冻结，逐字）。
 * 本头只做两件事：
 *   1) [C1]-[C3] 纯逻辑面声明 + 冻结数值宏（环形缓冲容量 / 单文件分片 / 文件数 /
 *      刷盘与重试窗口 / 写盘任务优先级）——行为实现家 diagnostics/sd_log.c，
 *      **零 FATFS / 零 FreeRTOS / 零 LVGL**，host_app linux target 与设备树同源；
 *   2) [C4] 芯片面（esp_log 钩子 / PSRAM 分配 / 写盘任务 / attach）声明——实现家
 *      diagnostics/sd_log_fs.cc，不进 host 构建。
 *
 * 隐私事实（human 2026-10-18 有意识接受，见 docs/debt/tech/
 * TECH-261016-sd-story-confidentiality.md）：落卡内容含 STT 转写与 LLM 回复原文，
 * 明文写可拔 TF 卡。本层不做加密 / 脱敏 / 访问控制（不在本包范围）。
 */

#ifndef KIDPAL_DIAGNOSTICS_SD_LOG_H
#define KIDPAL_DIAGNOSTICS_SD_LOG_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- *
 * [C1]/[C2]/[C3] 冻结数值单源（acceptance TEST 8 扫本头宏字面，EQ-2）
 * ------------------------------------------------------------------------ */

/* 单条记录上限：[C1] 超长记录截断到该值。 */
#define KIDPAL_SD_LOG_RECORD_MAX    512u
/* PSRAM 环形缓冲容量：[C4] chip 面 heap_caps_malloc 按此值注入（128KB）。 */
#define KIDPAL_SD_LOG_BUFFER_BYTES  (128u * 1024u)
/* 单文件分片上限：[C2] 512KB —— 32 个文件 => 目录总量 16MB 封顶。 */
#define KIDPAL_SD_LOG_FILE_BYTES    (512u * 1024u)
/* 日志文件数封顶：[C2] KPLOG000.TXT .. KPLOG031.TXT，之后 wrap 覆盖最旧。 */
#define KIDPAL_SD_LOG_FILE_COUNT    32u
/* 8.3 文件名缓冲："KPLOG031.TXT" = 12 字节 + NUL = 13，留 16 有余量。 */
#define KIDPAL_SD_LOG_NAME_CAP      16u
/* 刷盘窗口：[C3] 1s 或 4KB 批量。 */
#define KIDPAL_SD_LOG_FLUSH_MS      1000UL
#define KIDPAL_SD_LOG_FLUSH_BYTES   4096u
/* 写失败重试窗口：[C3] 30s。 */
#define KIDPAL_SD_LOG_RETRY_MS      30000UL
/* 写盘任务低优先级：[C3]/[C4] <= idle+1，不与语音 / AEC / LVGL 争实时。 */
#define KIDPAL_SD_LOG_TASK_PRIO     1

/* -------------------------------------------------------------------------- *
 * [C1] 有界环形缓冲（控制块归实现自有；调用方只持不透明句柄）
 * ------------------------------------------------------------------------ */

typedef struct kidpal_sd_log_ring kidpal_sd_log_ring_t;

/* storage = 调用方存储（chip 面 = PSRAM 块）；capacity = 字节数。
 * storage == NULL / capacity < 2*(RECORD_MAX+4) -> NULL（不写 storage）。 */
kidpal_sd_log_ring_t *kidpal_sd_log_ring_init(void *storage, unsigned capacity);
/* 释放实现自有控制块，**不动**调用方 storage；NULL 安全。 */
void kidpal_sd_log_ring_destroy(kidpal_sd_log_ring_t *r);

/* 记录 = 任意字节串（原样保存，不补 NUL）；多写者并发下每条记录原子入库；
 * len > RECORD_MAX 截断；满则丢最旧整条（dropped++）再接纳。
 * 返回实际入库字节数；NULL r / NULL data / len == 0 -> -1。 */
int kidpal_sd_log_ring_write(kidpal_sd_log_ring_t *r, const void *data, unsigned len);

unsigned kidpal_sd_log_ring_pending(const kidpal_sd_log_ring_t *r);  /* NULL -> 0 */
unsigned kidpal_sd_log_ring_dropped(const kidpal_sd_log_ring_t *r);  /* NULL -> 0 */

/* attach/detach 只切标志、**不清缓冲**（早启动留存语义）；NULL -> -1。 */
int kidpal_sd_log_ring_attach(kidpal_sd_log_ring_t *r);
int kidpal_sd_log_ring_detach(kidpal_sd_log_ring_t *r);
int kidpal_sd_log_ring_attached(const kidpal_sd_log_ring_t *r);  /* NULL -> 0 */

/* 取最旧未消费记录到 out（原样字节，不补 NUL），**不推进读指针**；
 * 未 attach / 空 / out_cap 不足 -> 0 且零消耗；成功 -> 记录字节数。 */
unsigned kidpal_sd_log_ring_peek(kidpal_sd_log_ring_t *r, void *out,
                                 unsigned out_cap);
/* 消费掉 peek 到的最旧记录：消费 -> 1；无可消费 -> 0；NULL -> -1。 */
int kidpal_sd_log_ring_commit(kidpal_sd_log_ring_t *r);

/* -------------------------------------------------------------------------- *
 * [C2] 轮转与 8.3 命名（纯逻辑、零文件 I/O）
 * ------------------------------------------------------------------------ */

/* index 0..31 -> "KPLOG%03u.TXT"（8.3，全大写，零 LFN 依赖，12 字节 + NUL）
 * 成功 0；index >= FILE_COUNT / NULL out / cap < 13 -> -1。 */
int kidpal_sd_log_file_name(unsigned index, char *out, unsigned cap);

typedef struct kidpal_sd_log_rot kidpal_sd_log_rot_t;

kidpal_sd_log_rot_t *kidpal_sd_log_rot_init(void);   /* index 0 / bytes 0；OOM -> NULL */
void kidpal_sd_log_rot_destroy(kidpal_sd_log_rot_t *rot);  /* NULL 安全 */

/* bytes_in_file += bytes；到 FILE_BYTES 封口 -> index 推进一格（wrap）、
 * bytes 归零、返回 1；未到界 0；NULL -> -1。 */
int kidpal_sd_log_rot_account(kidpal_sd_log_rot_t *rot, unsigned bytes);
int kidpal_sd_log_rot_index(const kidpal_sd_log_rot_t *rot);          /* NULL -> -1 */
unsigned kidpal_sd_log_rot_bytes_in_file(const kidpal_sd_log_rot_t *rot);  /* NULL -> 0 */

/* -------------------------------------------------------------------------- *
 * [C3] 刷盘门控 / 重试决策表（显式注入，无真实 timer / 无全局态）
 * ------------------------------------------------------------------------ */

/* attached == 0 -> 0（无卡 = 零 fopen 零写盘）；无待刷 -> 0；
 * since_last_flush_ms >= FLUSH_MS || pending_bytes >= FLUSH_BYTES -> 1。 */
int kidpal_sd_log_should_flush(int attached, unsigned pending_records,
                               unsigned pending_bytes,
                               unsigned long since_last_flush_ms);
/* write_failed != 0 && since_fail_ms >= RETRY_MS -> 1；其余 -> 0。 */
int kidpal_sd_log_retry_due(int write_failed, unsigned long since_fail_ms);

/* -------------------------------------------------------------------------- *
 * [C4] 芯片面（家 diagnostics/sd_log_fs.cc；不进 host 构建）
 * ------------------------------------------------------------------------ */

/* esp_log vprintf 回调类型（兼容 ESP-IDF vprintf_like_t；不需要 include
 * esp_log.h，保持本头对 host 构建零芯片依赖）。 */
typedef int (*kidpal_log_vprintf_t)(const char *fmt, va_list ap);

/* PSRAM 环形缓冲分配 + 写盘任务创建（app_main 最早期调用）。 */
void KidpalSdLogInit(void);
/* 保存 esp_log_set_vprintf 返回的旧 vprintf 指针（变量名字面冻结）。 */
void KidpalSdLogChainPreviousVprintf(kidpal_log_vprintf_t previous);
/* esp_log_set_vprintf 回调：先链式转调旧 vprintf（串口不中断），再写环形缓冲。 */
int KidpalSdLogVprintf(const char *fmt, va_list ap);
/* 板级挂载成功分支调用：环置 attached（不清缓冲）+ 打开 / 轮转日志文件。
 * 无卡路径不调用本函数。 */
void KidpalSdLogAttach(const char *mount_root);

/* 当前轮转索引只读访问器（包 261018-sd-log-usb-read 增量；D5）：供 USB 取回的
 * list 当前文件 * 标记 / tail 当前文件选取使用（不靠 mtime——FAT 时间粒度 2s、
 * RTC 未同步时会退化）。未 init / 未 attach 返回 -1。 */
int KidpalSdLogCurrentIndex(void);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_DIAGNOSTICS_SD_LOG_H */
