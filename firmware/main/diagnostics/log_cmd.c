/*
 * log_cmd.c — 诊断日志按需 USB 串口取回的纯逻辑实现（TaskPackage 261018-sd-log-usb-read）。
 *
 * seam contract 权威 = firmware/main/test/log_cmd.acceptance.c 文件头 [S2]-[S5]（冻结，
 * 逐字）；本文件逐条按冻结签名 / 返回值语义实现，行内注释标注对应条目。
 *
 * 纯逻辑红线：零 FATFS / 零 FreeRTOS / 零 LVGL / 零芯片依赖 —— host_app linux target
 * 与设备固件两侧同源编译。真实 VFS 读 / FATFS 只读 / stdout 输出 / 任务创建都在芯片面
 * （diagnostics/log_cmd_port.cc），本文件不碰（acceptance TEST 2 以可执行源零文件 I/O
 * needle 钉死「拒绝即短路 = 文件访问层零触达」）。
 */

#include "log_cmd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------- *
 * [S2] 命令解析
 * ------------------------------------------------------------------------ */

/* 纯十进制解析（1..9 位，无符号）：成功 1 且写 out，否则 0（零写）。 */
static int ParseDecimal(const char *s, unsigned len, unsigned long *out)
{
    unsigned long v = 0ul;
    unsigned i;

    if (s == NULL || out == NULL || len == 0u || len > 9u) {
        return 0;
    }
    for (i = 0u; i < len; i++) {
        if (s[i] < '0' || s[i] > '9') {
            return 0;
        }
        v = v * 10ul + (unsigned long)(s[i] - '0');
    }
    *out = v;
    return 1;
}

int kidpal_logcmd_parse(const char *line, unsigned len, int *out_cmd, unsigned *out_arg)
{
    const char *rest;
    unsigned rlen;
    const char *sp;
    unsigned tlen;
    int has_arg;
    const char *arg;
    unsigned alen;

    /* NULL 入参 -> -1（零写出；契约 [S2]）。 */
    if (line == NULL || out_cmd == NULL || out_arg == NULL) {
        return -1;
    }
    /* 超长命令行（CMD_MAX < len <= LINE_MAX 由装配器喂入）-> BAD_CMD（拒绝即短路）。 */
    if (len > KIDPAL_LOGCMD_CMD_MAX) {
        *out_cmd = KIDPAL_LOGCMD_BAD_CMD;
        *out_arg = 0u;
        return 0;
    }
    /* 严格小写前缀 "kplog "（6 字节含尾随空格）；否则 SILENT（防串口噪声触发输出）。 */
    if (len < 6u || memcmp(line, "kplog ", 6u) != 0) {
        *out_cmd = KIDPAL_LOGCMD_SILENT;
        *out_arg = 0u;
        return 0;
    }
    rest = line + 6;
    rlen = len - 6u;
    if (rlen == 0u) {              /* "kplog " 后无子命令 -> 空 token */
        *out_cmd = KIDPAL_LOGCMD_BAD_CMD;
        *out_arg = 0u;
        return 0;
    }
    sp = (const char *)memchr(rest, ' ', (size_t)rlen);
    if (sp == rest) {              /* 前导空格 = 空 token（双空格形态） */
        *out_cmd = KIDPAL_LOGCMD_BAD_CMD;
        *out_arg = 0u;
        return 0;
    }
    if (sp == NULL) {
        tlen = rlen;
        has_arg = 0;
        arg = NULL;
        alen = 0u;
    } else {
        tlen = (unsigned)(sp - rest);
        has_arg = 1;
        arg = sp + 1;
        alen = rlen - tlen - 1u;
        if (alen == 0u || memchr(arg, ' ', (size_t)alen) != NULL) {
            /* 尾随空格 / 双空格 -> 空 token -> BAD_CMD。 */
            *out_cmd = KIDPAL_LOGCMD_BAD_CMD;
            *out_arg = 0u;
            return 0;
        }
    }

    /* list：无参数 -> LIST；带尾 token -> BAD_CMD。 */
    if (tlen == 4u && memcmp(rest, "list", 4u) == 0) {
        if (has_arg) {
            *out_cmd = KIDPAL_LOGCMD_BAD_CMD;
            *out_arg = 0u;
            return 0;
        }
        *out_cmd = KIDPAL_LOGCMD_LIST;
        *out_arg = 0u;
        return 0;
    }
    /* tail：缺省 -> TAIL/DEFAULT；"tail N" 纯十进制（1..9 位）非零 -> 钳制到 TAIL_MAX。 */
    if (tlen == 4u && memcmp(rest, "tail", 4u) == 0) {
        unsigned long v;

        if (!has_arg) {
            *out_cmd = KIDPAL_LOGCMD_TAIL;
            *out_arg = KIDPAL_LOGCMD_TAIL_DEFAULT;
            return 0;
        }
        if (!ParseDecimal(arg, alen, &v) || v == 0ul) {
            *out_cmd = KIDPAL_LOGCMD_BAD_CMD;
            *out_arg = 0u;
            return 0;
        }
        if (v > (unsigned long)KIDPAL_LOGCMD_TAIL_MAX) {
            v = (unsigned long)KIDPAL_LOGCMD_TAIL_MAX;
        }
        *out_cmd = KIDPAL_LOGCMD_TAIL;
        *out_arg = (unsigned)v;
        return 0;
    }
    /* cat：缺参 -> BAD_CMD；"cat N" 纯十进制（1..9 位）且 N < CAT_COUNT -> CAT，否则
     * BAD_INDEX（非数字 / 负号 / '+' / '.' '/' 路径字符亦落此）。 */
    if (tlen == 3u && memcmp(rest, "cat", 3u) == 0) {
        unsigned long v;

        if (!has_arg) {
            *out_cmd = KIDPAL_LOGCMD_BAD_CMD;
            *out_arg = 0u;
            return 0;
        }
        if (!ParseDecimal(arg, alen, &v) ||
            v >= (unsigned long)KIDPAL_LOGCMD_CAT_COUNT) {
            *out_cmd = KIDPAL_LOGCMD_BAD_INDEX;
            *out_arg = 0u;
            return 0;
        }
        *out_cmd = KIDPAL_LOGCMD_CAT;
        *out_arg = (unsigned)v;
        return 0;
    }
    /* 未知子命令 -> BAD_CMD。 */
    *out_cmd = KIDPAL_LOGCMD_BAD_CMD;
    *out_arg = 0u;
    return 0;
}

/* -------------------------------------------------------------------------- *
 * [S3] 行装配（增量、跨 feed 边界；内部缓冲 = 调用方 storage，控制块自有）
 * ------------------------------------------------------------------------ */

struct kidpal_logcmd_asm {
    char *buf;            /* 调用方存储（原始字节流，含 '\n' 分隔） */
    unsigned cap;         /* 字节容量（>= LINE_MAX + 1） */
    unsigned len;         /* 已存字节数 */
    unsigned line_start;  /* 当前（未终止）行在 buf 内的起始偏移 */
    int dropped;          /* 上次 feed 有超长行被整行丢弃（report-once） */
    int dropping;         /* 正在丢弃超长行（丢弃到换行为止） */
};

kidpal_logcmd_asm_t *kidpal_logcmd_asm_init(void *storage, unsigned capacity)
{
    kidpal_logcmd_asm_t *a;

    if (storage == NULL || capacity < KIDPAL_LOGCMD_LINE_MAX + 1u) {
        return NULL;  /* 存储注入面守卫：不写 storage */
    }
    a = (kidpal_logcmd_asm_t *)calloc(1, sizeof(*a));
    if (a == NULL) {
        return NULL;
    }
    a->buf = (char *)storage;
    a->cap = capacity;
    a->len = 0u;
    a->line_start = 0u;
    a->dropped = 0;
    a->dropping = 0;
    return a;
}

void kidpal_logcmd_asm_destroy(kidpal_logcmd_asm_t *a)
{
    free(a);  /* 只释放实现自有控制块，不动调用方 storage；NULL 安全 */
}

/* 从缓冲头部移除 n 字节（n <= len），并同步当前行起始偏移。 */
static void AsmDropFront(kidpal_logcmd_asm_t *a, unsigned n)
{
    unsigned rest = a->len - n;

    if (rest > 0u) {
        memmove(a->buf, a->buf + n, (size_t)rest);
    }
    a->len = rest;
    a->line_start = (a->line_start >= n) ? (a->line_start - n) : 0u;
}

int kidpal_logcmd_asm_feed(kidpal_logcmd_asm_t *a, const void *data, unsigned len)
{
    const unsigned char *p = (const unsigned char *)data;
    unsigned i;

    if (a == NULL || data == NULL) {
        return -1;
    }
    for (i = 0u; i < len; i++) {
        unsigned char c = p[i];

        if (a->dropping) {
            if (c == (unsigned char)'\n') {  /* 超长行终止：清丢弃态 + 置 report-once */
                a->dropping = 0;
                a->dropped = 1;
            }
            continue;  /* 丢弃到换行为止（整行字节含行尾） */
        }
        if (c == (unsigned char)'\n') {
            if (a->len >= a->cap) {
                /* 无空位存行尾分隔：本行整行丢弃（容量下界下仅极端积压可达）。 */
                a->len = a->line_start;
                a->dropped = 1;
                continue;
            }
            a->buf[a->len++] = '\n';
            a->line_start = a->len;
        } else {
            if ((a->len - a->line_start) >= KIDPAL_LOGCMD_LINE_MAX ||
                a->len >= a->cap) {
                /* 行长超 LINE_MAX（或缓冲满）：整行丢弃到换行为止。 */
                a->len = a->line_start;
                a->dropping = 1;
                continue;
            }
            a->buf[a->len++] = (char)c;
        }
    }
    return (int)len;
}

int kidpal_logcmd_asm_pop(kidpal_logcmd_asm_t *a, char *out, unsigned cap)
{
    if (a == NULL || out == NULL) {
        return -1;
    }
    /* cap < LINE_MAX + 1 恒 TOOLONG（零消费，契约 [S3]）。 */
    if (cap < KIDPAL_LOGCMD_LINE_MAX + 1u) {
        return LUR_ASM_TOOLONG;
    }
    if (a->dropped) {  /* dropped 标记 report-once：清标记、out 不写 */
        a->dropped = 0;
        return LUR_ASM_DROPPED;
    }
    for (;;) {
        unsigned k;
        unsigned line_len;
        int found = 0;

        for (k = 0u; k < a->len; k++) {
            if (a->buf[k] == '\n') {
                found = 1;
                break;
            }
        }
        if (!found) {
            return 0;  /* 无完整行 */
        }
        line_len = k;
        if (line_len > 0u && a->buf[line_len - 1u] == '\r') {
            line_len--;  /* CRLF 剥 '\r' */
        }
        if (line_len == 0u) {
            AsmDropFront(a, k + 1u);  /* 空行：消费并继续（零产出） */
            continue;
        }
        if (cap < line_len + 1u) {
            return LUR_ASM_TOOLONG;  /* 出参缓冲不足，行不消费 */
        }
        memcpy(out, a->buf, (size_t)line_len);
        out[line_len] = '\0';
        AsmDropFront(a, k + 1u);
        return (int)line_len;
    }
}

/* -------------------------------------------------------------------------- *
 * [S4] 帧格式化
 * ------------------------------------------------------------------------ */

const char *kidpal_logcmd_frame_begin(int cmd)
{
    switch (cmd) {
    case KIDPAL_LOGCMD_LIST:      return "=== KPLOG BEGIN list ===";
    case KIDPAL_LOGCMD_TAIL:      return "=== KPLOG BEGIN tail ===";
    case KIDPAL_LOGCMD_CAT:       return "=== KPLOG BEGIN cat ===";
    case KIDPAL_LOGCMD_BAD_CMD:   return "=== KPLOG BEGIN bad-cmd ===";
    case KIDPAL_LOGCMD_BAD_INDEX: return "=== KPLOG BEGIN bad-index ===";
    default:                      return NULL;
    }
}

const char *kidpal_logcmd_frame_end(void)
{
    return "=== KPLOG END ===";
}

const char *kidpal_logcmd_error_text(int err)
{
    switch (err) {
    case KIDPAL_LOGCMD_ERR_NO_CARD:   return "KPLOG ERROR no-card";
    case KIDPAL_LOGCMD_ERR_NO_FILE:   return "KPLOG ERROR no-file";
    case KIDPAL_LOGCMD_ERR_BAD_CMD:   return "KPLOG ERROR bad-cmd";
    case KIDPAL_LOGCMD_ERR_BAD_INDEX: return "KPLOG ERROR bad-index";
    default:                          return NULL;
    }
}

int kidpal_logcmd_list_line(char *out, unsigned cap, unsigned index,
                            unsigned long bytes, int is_current)
{
    char name[KIDPAL_SD_LOG_NAME_CAP];

    if (out == NULL || index >= KIDPAL_LOGCMD_CAT_COUNT || cap < 40u) {
        return -1;
    }
    /* 8.3 命名单源（LUR-A6①）：文件名只经 kidpal_sd_log_file_name 构造。 */
    if (kidpal_sd_log_file_name(index, name, (unsigned)sizeof(name)) != 0) {
        return -1;
    }
    if (is_current != 0) {
        (void)snprintf(out, (size_t)cap, "%s %lu *", name, bytes);
    } else {
        (void)snprintf(out, (size_t)cap, "%s %lu", name, bytes);
    }
    return 0;
}

int kidpal_logcmd_list_total(char *out, unsigned cap, unsigned long total_bytes,
                             unsigned files)
{
    if (out == NULL || cap < 40u) {
        return -1;
    }
    (void)snprintf(out, (size_t)cap, "KPLOG TOTAL %lu %u files", total_bytes, files);
    return 0;
}

/* -------------------------------------------------------------------------- *
 * [S5] 尾部窗口选取
 * ------------------------------------------------------------------------ */

int kidpal_logcmd_tail_window(const void *data, unsigned len, unsigned n)
{
    const unsigned char *p = (const unsigned char *)data;
    unsigned i;
    unsigned lines;
    unsigned off;

    if (len == 0u) {
        return 0;  /* 空缓冲 */
    }
    if (data == NULL) {
        return -1;  /* NULL data 且 len > 0 */
    }
    if (n == 0u) {
        return (int)len;  /* 空窗口（偏移 = len） */
    }
    /* 数行：每个 '\n' 结束一行；无结尾换行的非空末段算一行。 */
    lines = 0u;
    for (i = 0u; i < len; i++) {
        if (p[i] == '\n') {
            lines++;
        }
    }
    if (p[len - 1u] != '\n') {
        lines++;
    }
    if (n >= lines) {
        return 0;  /* N >= 行数 -> 整缓冲 */
    }
    /* 定位第 (lines - n) 行的起始偏移（跳过前面 lines-n 行）。 */
    {
        unsigned skip = lines - n;
        unsigned seen = 0u;
        unsigned j;

        off = 0u;
        for (j = 0u; j < len; j++) {
            if (p[j] == '\n') {
                seen++;
                if (seen == skip) {
                    off = j + 1u;
                    break;
                }
            }
        }
    }
    return (int)off;
}
