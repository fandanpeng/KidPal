/*
 * log_semantics.c — 板端诊断日志语义层纯逻辑实现（TaskPackage 261022-diag-log-content）。
 *
 * 契约权威 = firmware/main/test/diag_log.acceptance.c 文件头 [DLG-C1]-[DLG-C3]（冻结，
 * 逐字；行为半由该件 TEST 1/2/3 承载）。
 *
 * 铁律：本文件**零芯片依赖**——不 include esp_* / FreeRTOS / LVGL / FATFS，也不读真实
 * 时钟。宿主态（host_app linux target）与设备树编同一份源 = 双侧同源，行为断言在 host
 * 上真跑（[DLG-C4] 结构锚只覆盖芯片面接线，跑不到这里的决策）。
 *
 * 时间前缀的产出/消费分离（plan R1 ISR 缓解，R1 = 本包最大风险）：
 *   - kidpal_log_time_format = 纯决策（字段显式注入）；
 *   - kidpal_log_time_cache_update = 低频任务侧的预格式化产出（application.cc 1s tick）；
 *   - kidpal_log_time_cache = 钩子侧只读取串（sd_log_fs.cc KidpalSdLogVprintf）。
 * 钩子内**禁止**调 format（localtime_r / snprintf 在任意任务 / ISR 上下文的重入与
 * 时延风险就是 R1 本体）。
 */

#include "log_semantics.h"

#include <stdio.h>
#include <string.h>

/* -------------------------------------------------------------------------- *
 * [DLG-C1] 墙上时间前缀格式化决策
 * ------------------------------------------------------------------------ */

int kidpal_log_time_format(int synced, int year, int mon, int mday,
                           int hour, int min, int sec,
                           unsigned long long boot_ms, char *out, unsigned cap)
{
    /* 先全部校验再落笔：任何拒绝路径都保持 out **零写入**（调用方缓冲不得被半串
     * 污染 —— 冻结件 TEST 1 的 'X' 全量不变断言）。 */
    char tmp[KIDPAL_LOG_TIME_PREFIX_CAP];
    int n;

    if (out == NULL) {
        return -1;
    }
    if (synced != 0) {
        if (year < 2000 || year > 9999 || mon < 1 || mon > 12 || mday < 1 || mday > 31 ||
            hour < 0 || hour > 23 || min < 0 || min > 59 || sec < 0 || sec > 59) {
            return -1;
        }
        n = snprintf(tmp, sizeof(tmp), "[%04d-%02d-%02d %02d:%02d:%02d] ",
                     year, mon, mday, hour, min, sec);
    } else {
        /* 未同步降级标注：boot_ms 无上限、不校验其余字段（SNTP/RTC 未对齐时唯一
         * 诚实的时间面 —— 至少能把日志排成开机后的先后序）。 */
        n = snprintf(tmp, sizeof(tmp), "[BOOT+%llums] ", boot_ms);
    }
    if (n < 0 || (unsigned)n >= (unsigned)sizeof(tmp) || cap <= (unsigned)n) {
        return -1; /* 截断 / 容量不足：不写 out（宁缺前缀，不出半串） */
    }
    memcpy(out, tmp, (size_t)n + 1u);
    return n;
}

/* -------------------------------------------------------------------------- *
 * [DLG-C1] 预格式化缓存（双缓冲 + 原子索引；产出 = 低频任务，消费 = 钩子）
 * ------------------------------------------------------------------------ */

/* 两个槽交替写：写方只改「非当前」槽，改完原子发布索引。读方按索引取到的恒是某一次
 * update 的**完整成品串**（永不撕裂）；最坏读到上一拍前缀（1s 陈旧，可接受）。
 * minimal: 两槽 + RELAXED/RELEASE 不是严格 seqlock —— 若将来要求「同一行时间恒与
 * 该行事件同刻」，需升级为按行携带时间戳，那要动 sd_log 记录格式（非本包范围）。 */
static char s_time_cache[2][KIDPAL_LOG_TIME_PREFIX_CAP];
static unsigned s_time_cache_idx; /* 0 / 1；静态零初始化 = 槽 0 为 "" */

void kidpal_log_time_cache_update(int synced, int year, int mon, int mday,
                                  int hour, int min, int sec,
                                  unsigned long long boot_ms)
{
    unsigned cur = __atomic_load_n(&s_time_cache_idx, __ATOMIC_RELAXED);
    unsigned next = (cur == 0u) ? 1u : 0u;

    /* 复用 format 的决策 = 单源一致（冻结件 EQ-5）：钩子拼的前缀与行为断言过的格式
     * 不会各写一套。 */
    if (kidpal_log_time_format(synced, year, mon, mday, hour, min, sec, boot_ms,
                               s_time_cache[next], KIDPAL_LOG_TIME_PREFIX_CAP) < 0) {
        s_time_cache[next][0] = '\0'; /* 坏时间字段：降级无前缀，绝不落假时间 */
    }
    __atomic_store_n(&s_time_cache_idx, next, __ATOMIC_RELEASE);
}

const char *kidpal_log_time_cache(void)
{
    unsigned idx = __atomic_load_n(&s_time_cache_idx, __ATOMIC_ACQUIRE);

    return s_time_cache[idx & 1u];
}

/* -------------------------------------------------------------------------- *
 * [DLG-C2] 语义名映射（闭集 + fallback）
 * ------------------------------------------------------------------------ */

/* fallback 串槽：闭集外值的 "<族>_<n>" 形态。用一组轮转槽而不是单槽 —— 同一条日志行
 * 里可能同时出现同族两个越界值，且任意任务都可能调；轮转使相邻调用不互相覆盖
 * （8 槽 × 24B = 192B，代价可忽略）。 */
#define KIDPAL_LOG_FALLBACK_SLOTS 8u
#define KIDPAL_LOG_FALLBACK_CAP   24u

static char s_fallback_slots[KIDPAL_LOG_FALLBACK_SLOTS][KIDPAL_LOG_FALLBACK_CAP];
static unsigned s_fallback_next;

static const char *ls_fallback(const char *family, int v)
{
    unsigned slot = __atomic_fetch_add(&s_fallback_next, 1u, __ATOMIC_RELAXED);
    char *buf = s_fallback_slots[slot % KIDPAL_LOG_FALLBACK_SLOTS];

    (void)snprintf(buf, KIDPAL_LOG_FALLBACK_CAP, "%s_%d", family, v);
    return buf;
}

const char *kidpal_log_action_name(int act)
{
    /* global_action_t（key_gesture_parser.h）append-only 闭集：既有位零偏移
     * （EQ-3 镜像——闭集扩张时冻结件 fallback 断言转红提醒重钉）。 */
    static const char *const names[] = {
        "none",           /* 0 GLOBAL_ACT_NONE */
        "session_toggle", /* 1 GLOBAL_ACT_SESSION_TOGGLE */
        "home",           /* 2 GLOBAL_ACT_HOME */
        "page_next",      /* 3 GLOBAL_ACT_PAGE_NEXT */
        "overlay_close",  /* 4 GLOBAL_ACT_OVERLAY_CLOSE */
        "char_replay"     /* 5 GLOBAL_ACT_CHAR_REPLAY */
    };

    if (act >= 0 && (unsigned)act < (unsigned)(sizeof(names) / sizeof(names[0]))) {
        return names[act];
    }
    return ls_fallback("act", act);
}

const char *kidpal_log_key_name(int key)
{
    static const char *const names[] = {
        "key",  /* 0 KEY_ID_KEY */
        "boot"  /* 1 KEY_ID_BOOT */
    };

    if (key >= 0 && (unsigned)key < (unsigned)(sizeof(names) / sizeof(names[0]))) {
        return names[key];
    }
    return ls_fallback("key", key);
}

const char *kidpal_log_gesture_name(int g)
{
    static const char *const names[] = {
        "none",         /* 0 KEY_GESTURE_NONE */
        "click",        /* 1 KEY_GESTURE_CLICK */
        "double_click", /* 2 KEY_GESTURE_DOUBLE_CLICK */
        "long_press"    /* 3 KEY_GESTURE_LONG_PRESS */
    };

    if (g >= 0 && (unsigned)g < (unsigned)(sizeof(names) / sizeof(names[0]))) {
        return names[g];
    }
    return ls_fallback("ges", g);
}

const char *kidpal_log_page_name(int page)
{
    static const char *const names[] = {
        "home",     /* 0 KP_PAGE_HOME */
        "schedule", /* 1 KP_PAGE_SCHEDULE */
        "story"     /* 2 KP_PAGE_STORY */
    };

    if (page >= 0 && (unsigned)page < (unsigned)(sizeof(names) / sizeof(names[0]))) {
        return names[page];
    }
    /* 负值同走 fallback：kidpal_render_node_story_substate 用 -1 表「非 P3」，页面面
     * 同样可能收到负值；page 3（撤 P4 后的非法页）绝不能映射出伪语义名。 */
    return ls_fallback("page", page);
}

const char *kidpal_log_trigger_name(int trg)
{
    static const char *const names[] = {
        "key",    /* 0 按键路径（键路由翻页 / 关层回原页 / 页内导航） */
        "timer",  /* 1 定时器路径 */
        "net"     /* 2 网络 / 云端推送路径（含语音轮收尾的页面回落） */
    };

    if (trg >= 0 && (unsigned)trg < (unsigned)(sizeof(names) / sizeof(names[0]))) {
        return names[trg];
    }
    return ls_fallback("trg", trg);
}

/* -------------------------------------------------------------------------- *
 * [DLG-C3] SystemInfo 降频决策
 * ------------------------------------------------------------------------ */

int kidpal_log_sysinfo_due(long long now, long long last, long long interval)
{
    if (interval <= 0) {
        return -1; /* 防御：零/负间隔 = 每 tick 一条，等于没降频 */
    }
    if (now < last) {
        return 0; /* 时钟回退 / tick 重置：fail-closed，噪声治理不得反弹成风暴 */
    }
    return (now - last >= interval) ? 1 : 0;
}
