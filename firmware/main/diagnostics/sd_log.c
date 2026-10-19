/*
 * sd_log.c — 板端诊断日志落 TF 卡的纯逻辑实现（TaskPackage 261018-sd-log-persist）。
 *
 * seam contract 权威 = firmware/main/test/sd_log.acceptance.c 文件头 [C1]-[C3]
 * （冻结，逐字）；本文件逐条按冻结签名 / 返回值语义实现，行内注释标注对应条目。
 *
 * 纯逻辑红线：零 FATFS / 零 FreeRTOS / 零 LVGL / 零芯片依赖 —— host_app linux
 * target 与设备固件两侧同源编译。真文件 I/O / esp_log 钩子 / PSRAM 分配都在芯片面
 * （diagnostics/sd_log_fs.cc），本文件不碰。
 *
 * 并发模型（[C1]「任意任务 / ISR 上下文可调用，多写者不撕裂」）：写入方在短临界区
 * 内完成「记头 + 记体 + 指针推进」，读者（peek/commit）同样持锁 —— host 侧用
 * pthread 多写者验收，设备侧 esp_log 来自任意任务。实现用 __atomic 自旋锁（C11
 * 原子语义，双架构可用），临界区只有两次 memcpy，无阻塞调用。
 * minimal: 自旋锁不是关中断临界区 —— 若将来出现「ISR 内 ESP_LOG」的真实需求，
 * 升级路径是芯片面在钩子外再套 portENTER_CRITICAL（本层零 FreeRTOS 红线不动）。
 */

#include "sd_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------- *
 * 内部控制块（不透明句柄；acceptance 只持指针，不假设布局）
 * ------------------------------------------------------------------------ */

/* 每条记录在存储里的形态 = [4 字节长度头][记录体]，均可跨存储末尾回绕。 */
#define KIDPAL_SD_LOG_SLOT_HEADER 4u

struct kidpal_sd_log_ring {
    unsigned char *storage;  /* 调用方存储（chip 面 = PSRAM） */
    unsigned capacity;       /* 字节容量 */
    unsigned head;           /* 写指针（字节偏移，0..capacity-1） */
    unsigned tail;           /* 读指针（字节偏移，0..capacity-1） */
    unsigned used;           /* 已占用字节（含记录头） */
    unsigned count;          /* 待消费记录数 */
    unsigned dropped;        /* 因满溢丢最旧的累计记录数 */
    int attached;            /* [C1] 1 = 可 drain；0 = 静默失效（无卡零写盘） */
    volatile int lock;       /* __atomic 自旋锁：0 空闲 / 1 占用 */
};

struct kidpal_sd_log_rot {
    unsigned index;  /* 当前文件号 0..FILE_COUNT-1 */
    unsigned bytes;  /* 当前文件已落盘字节数 */
};

/* -------------------------------------------------------------------------- *
 * 临界区（[C1] 多写者原子入库）
 * ------------------------------------------------------------------------ */

static void sd_log_lock(kidpal_sd_log_ring_t *r)
{
    while (__atomic_exchange_n(&r->lock, 1, __ATOMIC_ACQUIRE) != 0) {
        /* 极短自旋：持锁方只做两次 memcpy / 指针推进。 */
    }
}

static void sd_log_unlock(kidpal_sd_log_ring_t *r)
{
    __atomic_store_n(&r->lock, 0, __ATOMIC_RELEASE);
}

/* -------------------------------------------------------------------------- *
 * 存储读写（允许跨末尾回绕；off < capacity）
 * ------------------------------------------------------------------------ */

static void sd_log_store(kidpal_sd_log_ring_t *r, unsigned off, const void *src,
                         unsigned len)
{
    const unsigned char *p = (const unsigned char *)src;
    unsigned first = r->capacity - off;

    if (first > len) {
        first = len;
    }
    memcpy(r->storage + off, p, first);
    if (len > first) {
        memcpy(r->storage, p + first, len - first);
    }
}

static void sd_log_load(const kidpal_sd_log_ring_t *r, unsigned off, void *dst,
                        unsigned len)
{
    unsigned char *p = (unsigned char *)dst;
    unsigned first = r->capacity - off;

    if (first > len) {
        first = len;
    }
    memcpy(p, r->storage + off, first);
    if (len > first) {
        memcpy(p + first, r->storage, len - first);
    }
}

/* 消费最旧一条（调用方持锁）：count_as_dropped != 0 = 满溢丢最旧，否则 = 正常
 * commit。返回被消费的记录体长度。 */
static unsigned sd_log_consume_oldest(kidpal_sd_log_ring_t *r, int count_as_dropped)
{
    unsigned len = 0;

    sd_log_load(r, r->tail, &len, KIDPAL_SD_LOG_SLOT_HEADER);
    if (len > KIDPAL_SD_LOG_RECORD_MAX) {
        len = KIDPAL_SD_LOG_RECORD_MAX; /* 记账守恒的兜底（正常路径永不触发） */
    }
    r->tail = (r->tail + KIDPAL_SD_LOG_SLOT_HEADER + len) % r->capacity;
    r->used -= KIDPAL_SD_LOG_SLOT_HEADER + len;
    r->count--;
    if (count_as_dropped != 0) {
        r->dropped++;
    }
    return len;
}

/* -------------------------------------------------------------------------- *
 * [C1] 环形缓冲
 * ------------------------------------------------------------------------ */

kidpal_sd_log_ring_t *kidpal_sd_log_ring_init(void *storage, unsigned capacity)
{
    kidpal_sd_log_ring_t *r;

    /* 容量下界 = 至少能装下两条满长记录（保证满溢丢最旧后必有空间接纳新写）。 */
    if (storage == NULL ||
        capacity < 2u * (KIDPAL_SD_LOG_RECORD_MAX + KIDPAL_SD_LOG_SLOT_HEADER)) {
        return NULL;
    }
    r = (kidpal_sd_log_ring_t *)calloc(1, sizeof(*r));
    if (r == NULL) {
        return NULL;
    }
    r->storage = (unsigned char *)storage;
    r->capacity = capacity;
    r->head = 0;
    r->tail = 0;
    r->used = 0;
    r->count = 0;
    r->dropped = 0;
    r->attached = 0;
    r->lock = 0;
    return r;
}

void kidpal_sd_log_ring_destroy(kidpal_sd_log_ring_t *r)
{
    if (r == NULL) {
        return;
    }
    free(r);  /* 只释放控制块，不动调用方 storage */
}

int kidpal_sd_log_ring_write(kidpal_sd_log_ring_t *r, const void *data, unsigned len)
{
    unsigned need;
    unsigned hdr;

    if (r == NULL || data == NULL || len == 0u) {
        return -1;
    }
    if (len > KIDPAL_SD_LOG_RECORD_MAX) {
        len = KIDPAL_SD_LOG_RECORD_MAX;  /* [C1] 超长截断 */
    }
    need = len + KIDPAL_SD_LOG_SLOT_HEADER;

    sd_log_lock(r);
    while (r->capacity - r->used < need) {
        if (r->count == 0u) {
            break; /* 容量下界保证不可达 */
        }
        (void)sd_log_consume_oldest(r, 1);  /* [C1] 满 -> 丢最旧整条再接纳 */
    }
    hdr = len;
    sd_log_store(r, r->head, &hdr, KIDPAL_SD_LOG_SLOT_HEADER);
    r->head = (r->head + KIDPAL_SD_LOG_SLOT_HEADER) % r->capacity;
    sd_log_store(r, r->head, data, len);
    r->head = (r->head + len) % r->capacity;
    r->used += need;
    r->count++;
    sd_log_unlock(r);
    return (int)len;
}

unsigned kidpal_sd_log_ring_pending(const kidpal_sd_log_ring_t *r)
{
    return r == NULL ? 0u : r->count;
}

unsigned kidpal_sd_log_ring_dropped(const kidpal_sd_log_ring_t *r)
{
    return r == NULL ? 0u : r->dropped;
}

int kidpal_sd_log_ring_attach(kidpal_sd_log_ring_t *r)
{
    if (r == NULL) {
        return -1;
    }
    sd_log_lock(r);
    r->attached = 1;  /* [C1]/[C3] 只切标志，不清缓冲（早启动累积保留） */
    sd_log_unlock(r);
    return 0;
}

int kidpal_sd_log_ring_detach(kidpal_sd_log_ring_t *r)
{
    if (r == NULL) {
        return -1;
    }
    sd_log_lock(r);
    r->attached = 0;  /* 同上：只切标志，不动内容 */
    sd_log_unlock(r);
    return 0;
}

int kidpal_sd_log_ring_attached(const kidpal_sd_log_ring_t *r)
{
    return (r == NULL || r->attached == 0) ? 0 : 1;
}

unsigned kidpal_sd_log_ring_peek(kidpal_sd_log_ring_t *r, void *out,
                                 unsigned out_cap)
{
    unsigned len;

    if (r == NULL || out == NULL) {
        return 0;
    }
    sd_log_lock(r);
    if (r->attached == 0 || r->count == 0u) {
        /* [C1]/[C3] 未 attach（无卡）或空 -> 0 且零消耗。 */
        sd_log_unlock(r);
        return 0;
    }
    sd_log_load(r, r->tail, &len, KIDPAL_SD_LOG_SLOT_HEADER);
    if (len > out_cap) {
        sd_log_unlock(r);
        return 0;  /* 调用方缓冲不足 -> 0 且零消耗 */
    }
    sd_log_load(r, (r->tail + KIDPAL_SD_LOG_SLOT_HEADER) % r->capacity, out, len);
    sd_log_unlock(r);
    return len;
}

int kidpal_sd_log_ring_commit(kidpal_sd_log_ring_t *r)
{
    if (r == NULL) {
        return -1;
    }
    sd_log_lock(r);
    if (r->count == 0u) {
        sd_log_unlock(r);
        return 0;
    }
    (void)sd_log_consume_oldest(r, 0);
    sd_log_unlock(r);
    return 1;
}

/* -------------------------------------------------------------------------- *
 * [C2] 8.3 命名与轮转
 * ------------------------------------------------------------------------ */

int kidpal_sd_log_file_name(unsigned index, char *out, unsigned cap)
{
    int n;

    if (out == NULL || index >= KIDPAL_SD_LOG_FILE_COUNT ||
        cap < 13u /* "KPLOG031.TXT" = 12 + NUL */) {
        return -1;
    }
    n = snprintf(out, cap, "KPLOG%03u.TXT", index);
    return (n == 12) ? 0 : -1;
}

kidpal_sd_log_rot_t *kidpal_sd_log_rot_init(void)
{
    return (kidpal_sd_log_rot_t *)calloc(1, sizeof(kidpal_sd_log_rot_t));
}

void kidpal_sd_log_rot_destroy(kidpal_sd_log_rot_t *rot)
{
    free(rot);
}

int kidpal_sd_log_rot_account(kidpal_sd_log_rot_t *rot, unsigned bytes)
{
    if (rot == NULL) {
        return -1;
    }
    rot->bytes += bytes;
    if (rot->bytes >= KIDPAL_SD_LOG_FILE_BYTES) {
        /* 当前文件封口：推进一格（31 之后 wrap 回 000 = 覆盖最旧）；单次越界时
         * 超出部分不计入下一文件，下一文件从 0 起算。 */
        rot->index = (rot->index + 1u) % KIDPAL_SD_LOG_FILE_COUNT;
        rot->bytes = 0;
        return 1;
    }
    return 0;
}

int kidpal_sd_log_rot_index(const kidpal_sd_log_rot_t *rot)
{
    return rot == NULL ? -1 : (int)rot->index;
}

unsigned kidpal_sd_log_rot_bytes_in_file(const kidpal_sd_log_rot_t *rot)
{
    return rot == NULL ? 0u : rot->bytes;
}

/* -------------------------------------------------------------------------- *
 * [C3] 刷盘门控 / 重试决策表
 * ------------------------------------------------------------------------ */

int kidpal_sd_log_should_flush(int attached, unsigned pending_records,
                               unsigned pending_bytes,
                               unsigned long since_last_flush_ms)
{
    if (attached == 0) {
        return 0;  /* 无卡 = 零 fopen 零写盘 */
    }
    if (pending_records == 0u) {
        return 0;
    }
    if (since_last_flush_ms >= KIDPAL_SD_LOG_FLUSH_MS ||
        pending_bytes >= KIDPAL_SD_LOG_FLUSH_BYTES) {
        return 1;
    }
    return 0;
}

int kidpal_sd_log_retry_due(int write_failed, unsigned long since_fail_ms)
{
    if (write_failed != 0 && since_fail_ms >= KIDPAL_SD_LOG_RETRY_MS) {
        return 1;  /* 30s 重试窗口到期：坏卡 / 拔卡后仍有恢复写盘机会 */
    }
    return 0;
}
