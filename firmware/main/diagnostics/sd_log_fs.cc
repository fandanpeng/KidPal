/*
 * sd_log_fs.cc — 板端诊断日志落 TF 卡的芯片面（TaskPackage 261018-sd-log-persist）。
 *
 * 契约权威 = firmware/main/test/sd_log.acceptance.c 文件头 [C4]/[C5]（冻结，逐字）。
 * 本文件承载 host 不可达的三件事：
 *   1) esp_log vprintf 钩子：**先链式转调原 vprintf（串口输出不中断）再写环形缓冲**；
 *   2) KidpalSdLogInit：PSRAM 128KB 环形缓冲分配 + 低优先级写盘任务创建；
 *   3) KidpalSdLogAttach：板级挂载成功分支调用 —— 环置 attached（**不清缓冲**，早
 *      启动累积保留）+ 打开 / 轮转 /sdcard/logs/KPLOGnnn.TXT。
 *
 * 降级纪律（[C3]/SLP-A5）：无卡（未 attach）零 fopen 零写盘；写失败进 30s 重试窗口、
 * 不 commit（同一条仍是最旧，重试不丢条）；整层失效不 panic / 不 abort / 不阻断语音
 * 主链路（本文件零 ESP_ERROR_CHECK( 裸形态）。
 *
 * 写盘任务函数体内**零 ESP_LOG***：钩子会捕获一切 esp_log 输出，任务内再记日志会经
 * vprintf 递归回灌环形缓冲。任务失败诊断只在 attach 等任务外路径记。
 *
 * 写盘口径：peek -> 写文件 -> 轮转记账 -> commit 才推进读指针；每批 fflush + fsync
 * （1s 或 4KB 门控见 kidpal_sd_log_should_flush）——fflush 推流缓冲、fsync 才把
 * FAT 目录项与 FAT 链落卡（掉电/硬复位持久性保证）。
 */

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <unistd.h>  /* fsync（VFS -> f_sync，FAT 目录项落卡） */

#include <esp_heap_caps.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "sd_log.h"

#define TAG "SdLog"

namespace {

/* 全局态（单实例：诊断日志是进程级旁路，不做多实例）。 */
kidpal_sd_log_ring_t *s_ring = nullptr;
kidpal_sd_log_rot_t *s_rot = nullptr;
void *s_ring_storage = nullptr;
char s_dir_path[64] = {0};       /* "<mount_root>/logs" */
FILE *s_file = nullptr;          /* 当前 KPLOGnnn.TXT（单写者：写盘任务） */
unsigned long s_fail_since_ms = 0;
int s_write_failed = 0;
volatile unsigned s_pending_bytes = 0;  /* 未落盘字节（刷盘门控注入量） */

unsigned long NowMs(void)
{
    return (unsigned long)xTaskGetTickCount() * (unsigned long)portTICK_PERIOD_MS;
}

/* 当前轮转索引 + 8.3 命名单源 -> "<dir>/KPLOGnnn.TXT"。 */
int BuildLogPath(char *out, size_t cap)
{
    char name[KIDPAL_SD_LOG_NAME_CAP];
    int n;

    if (kidpal_sd_log_file_name((unsigned)kidpal_sd_log_rot_index(s_rot), name,
                                (unsigned)sizeof(name)) != 0) {
        return -1;
    }
    n = snprintf(out, cap, "%s/%s", s_dir_path, name);
    return (n > 0 && (size_t)n + 1u <= cap) ? 0 : -1;
}

int OpenCurrentFile(void)
{
    char path[96];

    if (s_file != nullptr) {
        return 0;
    }
    if (BuildLogPath(path, sizeof(path)) != 0) {
        return -1;
    }
    s_file = fopen(path, "ab");  /* 追加：重启后同号文件继续写 */
    return s_file == nullptr ? -1 : 0;
}

void CloseCurrentFile(void)
{
    if (s_file != nullptr) {
        (void)fclose(s_file);
        s_file = nullptr;
    }
}

void MarkWriteFailed(void)
{
    s_write_failed = 1;
    s_fail_since_ms = NowMs();
    CloseCurrentFile();  /* 下次重试重新打开（坏卡 / 拔卡后恢复路径） */
}

/* 把缓冲里待写的记录逐条落盘；写失败即停（不 commit，保留最旧条目供 30s 后重试）。 */
void DrainPending(void)
{
    char rec[KIDPAL_SD_LOG_RECORD_MAX];

    for (;;) {
        unsigned len = kidpal_sd_log_ring_peek(s_ring, rec, (unsigned)sizeof(rec));

        if (len == 0u) {
            return;
        }
        if (OpenCurrentFile() != 0) {
            MarkWriteFailed();
            return;
        }
        /* 真机实证修复（TaskPackage 261018-sd-log-persist，2026-10-19 human-decision
         * b0da8396）：fflush 只把 stdio 流缓冲推进 VFS/FATFS 的 RAM 文件对象，
         * FAT 目录项（文件大小）与 FAT 链只有 f_sync 才落卡 —— esptool 硬复位
         * （等价掉电）后目录项仍 0 字节、已写数据全失。fsync(fileno) 经 VFS 触发
         * f_sync 落目录项；fflush 与 fsync 语义互补（先把流缓冲推进 VFS，再把 VFS
         * 推到卡），故两者都保留。fsync 失败与 fwrite 失败同语义：写失败即停、
         * 不 commit（最旧条目保留供 30s 后重试）。 */
        if (fwrite(rec, 1, (size_t)len, s_file) != (size_t)len ||
            fflush(s_file) != 0 || fsync(fileno(s_file)) != 0) {
            MarkWriteFailed();
            return;
        }
        /* 512KB 分片记账：封口则关文件，下一轮按新索引重开（KPLOG031 后 wrap）。 */
        if (kidpal_sd_log_rot_account(s_rot, len) == 1) {
            CloseCurrentFile();
        }
        (void)kidpal_sd_log_ring_commit(s_ring);
        (void)__atomic_fetch_sub(&s_pending_bytes, len, __ATOMIC_RELAXED);
    }
}

}  // namespace

/* 旧 vprintf 指针保存面（变量名字面冻结，[C4]）：由 main.cc 把
 * esp_log_set_vprintf 的返回值交回本文件。 */
static kidpal_log_vprintf_t s_prev_vprintf = nullptr;

/* 写盘任务（[C4] 自由函数形态；函数体内零 ESP_LOG*，防 vprintf 递归回灌）。
 * 200ms 轮询 + kidpal_sd_log_should_flush 门控 = 1s 或 4KB 批量；失败走
 * kidpal_sd_log_retry_due 的 30s 窗口。 */
static void SdLogWriterTask(void *arg)
{
    unsigned long last_flush_ms = NowMs();

    (void)arg;
    for (;;) {
        unsigned long now;
        unsigned pending;
        unsigned pending_bytes;

        vTaskDelay(pdMS_TO_TICKS(200));
        if (s_ring == nullptr || kidpal_sd_log_ring_attached(s_ring) != 1) {
            continue;  /* 无卡 / 未挂载：整层静默失效, 零 fopen 零写盘 */
        }
        now = NowMs();
        if (s_write_failed != 0) {
            if (kidpal_sd_log_retry_due(1, now - s_fail_since_ms) == 0) {
                continue;  /* 30s 重试窗口未到 */
            }
            s_write_failed = 0;
        }
        pending = kidpal_sd_log_ring_pending(s_ring);
        pending_bytes = __atomic_load_n(&s_pending_bytes, __ATOMIC_RELAXED);
        if (kidpal_sd_log_should_flush(1, pending, pending_bytes,
                                       now - last_flush_ms) == 0) {
            continue;
        }
        last_flush_ms = now;
        DrainPending();
    }
}

extern "C" void KidpalSdLogInit(void)
{
    if (s_ring != nullptr) {
        return;
    }
    /* [C4] 环形缓冲落 PSRAM：不挤占内部 RAM（语音 / AEC / LVGL 堆）。 */
    s_ring_storage = heap_caps_malloc(KIDPAL_SD_LOG_BUFFER_BYTES, MALLOC_CAP_SPIRAM);
    if (s_ring_storage == nullptr) {
        return;  /* PSRAM 不可用：整层静默失效, 钩子只链式转调串口 */
    }
    s_ring = kidpal_sd_log_ring_init(s_ring_storage, KIDPAL_SD_LOG_BUFFER_BYTES);
    if (s_ring == nullptr) {
        heap_caps_free(s_ring_storage);
        s_ring_storage = nullptr;
        return;
    }
    s_rot = kidpal_sd_log_rot_init();
    if (s_rot == nullptr) {
        kidpal_sd_log_ring_destroy(s_ring);
        s_ring = nullptr;
        heap_caps_free(s_ring_storage);
        s_ring_storage = nullptr;
        return;
    }
    (void)xTaskCreate(SdLogWriterTask, "sd_log", 4096, nullptr,
                      KIDPAL_SD_LOG_TASK_PRIO, nullptr);
}

extern "C" void KidpalSdLogChainPreviousVprintf(kidpal_log_vprintf_t previous)
{
    s_prev_vprintf = previous;
}

/* [C4] esp_log_set_vprintf 回调：**先**链式转调原 vprintf（串口 COM5 输出不中断），
 * **再**把格式化后的日志写进环形缓冲。记录体不含结尾 NUL（[C1] 原样字节）。 */
extern "C" int KidpalSdLogVprintf(const char *fmt, va_list ap)
{
    va_list copy;
    int ret = 0;

    va_copy(copy, ap);
    if (s_prev_vprintf != nullptr) {
        ret = s_prev_vprintf(fmt, ap);
    }
    if (s_ring != nullptr) {
        char buf[KIDPAL_SD_LOG_RECORD_MAX];
        int n = vsnprintf(buf, sizeof(buf), fmt, copy);

        if (n > 0) {
            unsigned len = (unsigned)n;
            if (len >= KIDPAL_SD_LOG_RECORD_MAX) {
                len = KIDPAL_SD_LOG_RECORD_MAX - 1u;  /* 保 NUL 位, 记录体不超上限 */
            }
            /* 先计入待刷字节再入库：写盘任务据此做 4KB 批量门控（偏差只影响批量
             * 节奏，不影响落盘正确性）。 */
            (void)__atomic_fetch_add(&s_pending_bytes, len, __ATOMIC_RELAXED);
            (void)kidpal_sd_log_ring_write(s_ring, buf, len);
        }
    }
    va_end(copy);
    return ret;
}

/* [C5] 板级挂载成功分支调用：只置可 drain 标志（不清早启动累积的缓冲），再打开
 * 当前轮转文件。无卡路径由板级降级 return 挡掉，不会走到这里 = 零 fopen。 */
extern "C" void KidpalSdLogAttach(const char *mount_root)
{
    char name[KIDPAL_SD_LOG_NAME_CAP];
    char path[96];
    struct stat st;

    if (mount_root == nullptr || s_ring == nullptr || s_rot == nullptr) {
        return;
    }
    snprintf(s_dir_path, sizeof(s_dir_path), "%s/logs", mount_root);
    if (mkdir(s_dir_path, 0777) != 0 && errno != EEXIST) {
        ESP_LOGW(TAG, "log dir unavailable path=%s errno=%d -> diag log disabled",
                 s_dir_path, errno);
        return;  /* 建目录失败：整层静默失效, 不 attach（零 fopen） */
    }
    /* 重启续写 v2（261020 D4-4）：从**盘面状态**播种轮转器。旧实现只 stat「当前
     * index」——而轮转记账态在 RAM、每次启动 rot 归 000，跨启动已轮转的盘面
     * （000 已封口、001 带前次启动的残留）会把当前文件的既有字节整段漏记账：
     * 续写后记账 512KB 才封口，盘面单文件实际 512KB + 残留（真机 KPLOG001 740KB
     * 未轮实证 = 001 残留 ~220KB + 本轮记账 512KB）。新法：000 起找第一个不存在
     * /空/未满的文件号 = 当前文件（此前各号按满额推进轮转器——只用冻结纯逻辑
     * 既有 API，不新增决策面），当前文件按实际字节数播种记账；32 文件全满则整
     * wrap 回 000 覆盖最旧。只增不减纪律：已封口文件不清理不改名。 */
    {
        unsigned idx;

        for (idx = 0u; idx < KIDPAL_SD_LOG_FILE_COUNT; idx++) {
            long fsize;

            if (kidpal_sd_log_file_name(idx, name, (unsigned)sizeof(name)) != 0) {
                break;
            }
            snprintf(path, sizeof(path), "%s/%s", s_dir_path, name);
            if (stat(path, &st) != 0 || st.st_size <= 0) {
                break;  /* 不存在/空 = 当前文件（记账态已是该号、字节 0） */
            }
            fsize = (long)st.st_size;
            if (fsize < (long)KIDPAL_SD_LOG_FILE_BYTES) {
                /* 未满文件 = 当前文件：按实际字节播种（记账态在该号、字节=fsize） */
                (void)kidpal_sd_log_rot_account(s_rot, (unsigned)fsize);
                break;
            }
            /* 已封口（≥512KB）：按满额推进一格，看下一号 */
            (void)kidpal_sd_log_rot_account(s_rot, KIDPAL_SD_LOG_FILE_BYTES);
        }
    }
    (void)kidpal_sd_log_ring_attach(s_ring);
    ESP_LOGI(TAG, "diagnostic log attached at %s (KPLOG000..031, 512KB x 32)",
             s_dir_path);
}

/* [D5] 当前轮转索引只读访问器（包 261018-sd-log-usb-read 增量，只增不改）：
 * 未 init（s_rot/s_ring 空）/ 未 attach（无卡或挂载失败）-> -1；否则返回当前写入
 * 文件号 0..31。供 USB 取回的 list 的 * 标记与 tail 的当前文件选取使用（不靠 mtime）。 */
extern "C" int KidpalSdLogCurrentIndex(void)
{
    if (s_rot == nullptr || s_ring == nullptr) {
        return -1;  /* 未 init */
    }
    if (kidpal_sd_log_ring_attached(s_ring) != 1) {
        return -1;  /* 未 attach（无卡 / 挂载失败）：零 fopen 降级面 */
    }
    return kidpal_sd_log_rot_index(s_rot);
}
