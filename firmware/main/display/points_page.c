/*
 * points_page.c — KidPal P3 星星积分本设备消费面（TaskPackage
 * 261104-k0-points-e2e-spike；契约 §1.3⑦⑧ + §四 kidpal_points_page）。
 *
 * 语义与形状以冻结验收 firmware/main/test/points_spike.acceptance.c 文件头
 * [G2]/[G3] seam contract 为准，不得偏移（本文件只在 host runner / 设备树引用它们；
 * 冻结件自带 weak 哨兵，本文件的强符号在 host runner 覆盖之）。
 *
 * 纯逻辑、零芯片/FreeRTOS/LVGL/外部 JSON 库依赖 -> host_app linux target 与设备树
 * 两侧同源可编。降级语义：畸形 / 缺必带键 / 超容量一律 -1，调用方必须显式降级、
 * 绝不盲渲染 (V27)。
 *
 * 「数据未达不显示数字」（S-F6.1 AC4）不属本纯逻辑面：它 = 设备从未收到权威帧时的
 * RAM 初态，由 mcp_server 消费接线层承载。
 */

#include "points_page.h"

#include <assert.h>
#include <string.h>

/* -------------------------------------------------------------------------- *
 * 自足式最小 JSON 扫描（无外部 JSON 库；只需覆盖 producer 下发的 data 对象形状
 * 与 skip 未知扩展值）。与 story_page.c 的 ksy_* 扫描器同族、前缀独立（kpt_）。
 * ------------------------------------------------------------------------ */

#define KPT_KEY_BUF 32

typedef struct {
    const char *s;
    size_t pos;
} kpt_cursor;

static void kpt_ws(kpt_cursor *c)
{
    while (c->s[c->pos] == ' ' || c->s[c->pos] == '\t' ||
           c->s[c->pos] == '\n' || c->s[c->pos] == '\r') {
        c->pos++;
    }
}

static char kpt_peek(const kpt_cursor *c)
{
    return c->s[c->pos];
}

/* safe-truncate 拷贝：恒 NUL 结尾、绝不溢出。 */
static void kpt_copy_str(char *dst, size_t cap, const char *src)
{
    size_t i;

    if (dst == NULL || cap == 0) {
        return;
    }
    for (i = 0; src != NULL && src[i] != '\0' && i + 1 < cap; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
}

static int kpt_hex4(kpt_cursor *c, unsigned *out)
{
    unsigned v = 0;
    int i;

    for (i = 0; i < 4; i++) {
        char ch = c->s[c->pos];
        unsigned d;

        if (ch >= '0' && ch <= '9') {
            d = (unsigned)(ch - '0');
        } else if (ch >= 'a' && ch <= 'f') {
            d = (unsigned)(ch - 'a' + 10);
        } else if (ch >= 'A' && ch <= 'F') {
            d = (unsigned)(ch - 'A' + 10);
        } else {
            return 0;
        }
        v = v * 16u + d;
        c->pos++;
    }
    *out = v;
    return 1;
}

static int kpt_utf8_encode(unsigned cp, char *tmp)
{
    if (cp < 0x80u) {
        tmp[0] = (char)cp;
        return 1;
    }
    if (cp < 0x800u) {
        tmp[0] = (char)(0xC0u | (cp >> 6));
        tmp[1] = (char)(0x80u | (cp & 0x3Fu));
        return 2;
    }
    if (cp < 0x10000u) {
        tmp[0] = (char)(0xE0u | (cp >> 12));
        tmp[1] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
        tmp[2] = (char)(0x80u | (cp & 0x3Fu));
        return 3;
    }
    tmp[0] = (char)(0xF0u | (cp >> 18));
    tmp[1] = (char)(0x80u | ((cp >> 12) & 0x3Fu));
    tmp[2] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
    tmp[3] = (char)(0x80u | (cp & 0x3Fu));
    return 4;
}

static void kpt_put(char byte, char *out, size_t cap, size_t *used)
{
    if (out != NULL && *used + 1 < cap) {
        out[(*used)++] = byte;
    }
}

/* 解析 JSON 字符串到 out（cap 含 NUL 位; out 可为 NULL = 仅校验）。成功 1 / 畸形 0。 */
static int kpt_string(kpt_cursor *c, char *out, size_t cap)
{
    size_t used = 0;

    if (out != NULL && cap > 0) {
        out[0] = '\0';
    }
    if (c->s[c->pos] != '"') {
        return 0;
    }
    c->pos++;
    for (;;) {
        unsigned char ch = (unsigned char)c->s[c->pos];

        if (ch == 0) {
            return 0; /* 未闭合字符串 */
        }
        if (ch == (unsigned char)'"') {
            c->pos++;
            break;
        }
        if (ch == (unsigned char)'\\') {
            char e;
            c->pos++;
            e = c->s[c->pos];
            c->pos++;
            if (e == 'u') {
                unsigned cp;
                char tmp[4];
                int n, i;

                if (!kpt_hex4(c, &cp)) {
                    return 0;
                }
                if (cp >= 0xD800u && cp <= 0xDBFFu) {
                    unsigned lo;
                    if (c->s[c->pos] == '\\' && c->s[c->pos + 1] == 'u') {
                        c->pos += 2;
                        if (!kpt_hex4(c, &lo)) {
                            return 0;
                        }
                        if (lo < 0xDC00u || lo > 0xDFFFu) {
                            return 0;
                        }
                        cp = 0x10000u + ((cp - 0xD800u) << 10) + (lo - 0xDC00u);
                    } else {
                        return 0;
                    }
                } else if (cp >= 0xDC00u && cp <= 0xDFFFu) {
                    return 0;
                }
                n = kpt_utf8_encode(cp, tmp);
                for (i = 0; i < n; i++) {
                    kpt_put(tmp[i], out, cap, &used);
                }
                continue;
            }
            {
                char plain;
                switch (e) {
                    case '"':  plain = '"';  break;
                    case '\\': plain = '\\'; break;
                    case '/':  plain = '/';  break;
                    case 'b':  plain = '\b'; break;
                    case 'f':  plain = '\f'; break;
                    case 'n':  plain = '\n'; break;
                    case 'r':  plain = '\r'; break;
                    case 't':  plain = '\t'; break;
                    default:   return 0;
                }
                kpt_put(plain, out, cap, &used);
            }
            continue;
        }
        kpt_put((char)ch, out, cap, &used);
        c->pos++;
    }
    if (out != NULL) {
        out[used] = '\0';
    }
    return 1;
}

/* 互递归前置声明（skip_value <-> skip_object/skip_array）。 */
static int kpt_skip_value(kpt_cursor *c);

/* 跳过一个 object（仅结构校验，不取字段）。成功 1 / 畸形 0。 */
static int kpt_skip_object(kpt_cursor *c)
{
    if (kpt_peek(c) != '{') {
        return 0;
    }
    c->pos++;
    kpt_ws(c);
    if (kpt_peek(c) == '}') {
        c->pos++;
        return 1;
    }
    for (;;) {
        char d;
        kpt_ws(c);
        if (!kpt_string(c, NULL, 0)) {
            return 0;
        }
        kpt_ws(c);
        if (kpt_peek(c) != ':') {
            return 0;
        }
        c->pos++;
        if (!kpt_skip_value(c)) {
            return 0;
        }
        kpt_ws(c);
        d = kpt_peek(c);
        if (d == ',') {
            c->pos++;
            continue;
        }
        if (d == '}') {
            c->pos++;
            return 1;
        }
        return 0;
    }
}

/* 跳过一个数组（仅结构校验）。成功 1 / 畸形 0。 */
static int kpt_skip_array(kpt_cursor *c)
{
    if (kpt_peek(c) != '[') {
        return 0;
    }
    c->pos++;
    kpt_ws(c);
    if (kpt_peek(c) == ']') {
        c->pos++;
        return 1;
    }
    for (;;) {
        char d;
        if (!kpt_skip_value(c)) {
            return 0;
        }
        kpt_ws(c);
        d = kpt_peek(c);
        if (d == ',') {
            c->pos++;
            continue;
        }
        if (d == ']') {
            c->pos++;
            return 1;
        }
        return 0;
    }
}

/* 跳过任意一个 JSON 值（string/object/array/number/true/false/null），兼做结构
 * 合法性校验。成功 1 / 畸形 0。 */
static int kpt_skip_value(kpt_cursor *c)
{
    char ch;
    kpt_ws(c);
    ch = kpt_peek(c);
    if (ch == '"') {
        return kpt_string(c, NULL, 0);
    }
    if (ch == '{') {
        return kpt_skip_object(c);
    }
    if (ch == '[') {
        return kpt_skip_array(c);
    }
    if (strncmp(&c->s[c->pos], "true", 4) == 0) {
        c->pos += 4;
        return 1;
    }
    if (strncmp(&c->s[c->pos], "false", 5) == 0) {
        c->pos += 5;
        return 1;
    }
    if (strncmp(&c->s[c->pos], "null", 4) == 0) {
        c->pos += 4;
        return 1;
    }
    if (ch == '-' || (ch >= '0' && ch <= '9')) {
        c->pos++;
        while ((c->s[c->pos] >= '0' && c->s[c->pos] <= '9') ||
               c->s[c->pos] == '.' || c->s[c->pos] == 'e' || c->s[c->pos] == 'E' ||
               c->s[c->pos] == '+' || c->s[c->pos] == '-') {
            c->pos++;
        }
        return 1;
    }
    return 0;
}

/* 严格解析一个有符号 JSON 整数（无小数点 / 无指数）。成功 1 且 *out 为值；否则 0
 * （游标还原，交调用方按畸形处理）。 */
static int kpt_int(kpt_cursor *c, long *out)
{
    long v = 0;
    size_t start;
    int neg = 0;
    int digits = 0;

    kpt_ws(c);
    start = c->pos;
    if (c->s[c->pos] == '-') {
        neg = 1;
        c->pos++;
    }
    while (c->s[c->pos] >= '0' && c->s[c->pos] <= '9') {
        if (v > 100000000L) { /* 防溢出的饱和处理（仍要求整数结构） */
            v = 100000000L;
        } else {
            v = v * 10 + (c->s[c->pos] - '0');
        }
        digits++;
        c->pos++;
    }
    if (digits == 0) {
        c->pos = start;
        return 0;
    }
    {
        char after = c->s[c->pos];
        if (after == '.' || after == 'e' || after == 'E') {
            c->pos = start;
            return 0;
        }
    }
    *out = neg ? -v : v;
    return 1;
}

/* 解析 JSON 布尔（true/false）到 *out（1/0）。成功 1 / 畸形 0。 */
static int kpt_bool(kpt_cursor *c, int *out)
{
    kpt_ws(c);
    if (strncmp(&c->s[c->pos], "true", 4) == 0) {
        c->pos += 4;
        *out = 1;
        return 1;
    }
    if (strncmp(&c->s[c->pos], "false", 5) == 0) {
        c->pos += 5;
        *out = 0;
        return 1;
    }
    return 0;
}

/* -------------------------------------------------------------------------- *
 * 分录六键解析（契约 §1.3⑦⑧ 共用形状）
 * ------------------------------------------------------------------------ */

static int kpt_parse_entry(kpt_cursor *c, kidpal_points_entry_t *slot)
{
    int has_id = 0, has_delta = 0, has_reason = 0;
    int has_at = 0, has_before = 0, has_after = 0;
    char id_buf[KPTS_ENTRY_ID_CAP];
    char reason_buf[KPTS_ENTRY_REASON_CAP];
    char at_buf[KPTS_ENTRY_AT_CAP];
    long delta = 0, before = 0, after = 0;

    id_buf[0] = '\0';
    reason_buf[0] = '\0';
    at_buf[0] = '\0';

    kpt_ws(c);
    if (kpt_peek(c) != '{') {
        return -1;
    }
    c->pos++;
    kpt_ws(c);
    if (kpt_peek(c) == '}') {
        return -1; /* 空分录 = 缺必带键 */
    }
    for (;;) {
        char key[KPT_KEY_BUF];
        char d;

        kpt_ws(c);
        if (!kpt_string(c, key, sizeof(key))) {
            return -1;
        }
        kpt_ws(c);
        if (kpt_peek(c) != ':') {
            return -1;
        }
        c->pos++;
        kpt_ws(c);
        if (strcmp(key, "id") == 0) {
            if (kpt_peek(c) != '"') {
                return -1;
            }
            if (!kpt_string(c, id_buf, sizeof(id_buf))) {
                return -1;
            }
            has_id = 1;
        } else if (strcmp(key, "delta") == 0) {
            if (!kpt_int(c, &delta)) {
                return -1;
            }
            has_delta = 1;
        } else if (strcmp(key, "reason") == 0) {
            if (kpt_peek(c) != '"') {
                return -1;
            }
            if (!kpt_string(c, reason_buf, sizeof(reason_buf))) {
                return -1;
            }
            has_reason = 1;
        } else if (strcmp(key, "occurred_at") == 0) {
            if (kpt_peek(c) != '"') {
                return -1;
            }
            if (!kpt_string(c, at_buf, sizeof(at_buf))) {
                return -1;
            }
            has_at = 1;
        } else if (strcmp(key, "balance_before") == 0) {
            if (!kpt_int(c, &before)) {
                return -1;
            }
            has_before = 1;
        } else if (strcmp(key, "balance_after") == 0) {
            if (!kpt_int(c, &after)) {
                return -1;
            }
            has_after = 1;
        } else {
            /* 未知扩展字段忽略、不消费（additive）。 */
            if (!kpt_skip_value(c)) {
                return -1;
            }
        }
        kpt_ws(c);
        d = kpt_peek(c);
        if (d == ',') {
            c->pos++;
            continue;
        }
        if (d == '}') {
            c->pos++;
            break;
        }
        return -1;
    }
    if (!has_id || id_buf[0] == '\0' || !has_delta || !has_reason ||
        !has_at || at_buf[0] == '\0' || !has_before || !has_after) {
        return -1; /* 六键必带（契约统一分录形状） */
    }
    kpt_copy_str(slot->id, sizeof(slot->id), id_buf);
    slot->delta = (int)delta;
    kpt_copy_str(slot->reason, sizeof(slot->reason), reason_buf);
    kpt_copy_str(slot->occurred_at, sizeof(slot->occurred_at), at_buf);
    slot->balance_before = (int)before;
    slot->balance_after = (int)after;
    return 0;
}

/* -------------------------------------------------------------------------- *
 * kidpal_points_home_parse — [G2]（契约 §1.3⑦）
 * ------------------------------------------------------------------------ */

static int kpt_parse_target(kpt_cursor *c, kidpal_points_home_t *out,
                            int *name_seen, int *points_seen)
{
    long points = 0;

    kpt_ws(c);
    if (kpt_peek(c) != '{') {
        return -1; /* target 必为对象 */
    }
    c->pos++;
    kpt_ws(c);
    if (kpt_peek(c) == '}') {
        return -1; /* 空 target = 缺 name/points */
    }
    for (;;) {
        char key[KPT_KEY_BUF];
        char d;

        kpt_ws(c);
        if (!kpt_string(c, key, sizeof(key))) {
            return -1;
        }
        kpt_ws(c);
        if (kpt_peek(c) != ':') {
            return -1;
        }
        c->pos++;
        kpt_ws(c);
        if (strcmp(key, "name") == 0) {
            if (kpt_peek(c) != '"') {
                return -1;
            }
            if (!kpt_string(c, out->target_name, sizeof(out->target_name))) {
                return -1;
            }
            *name_seen = 1;
        } else if (strcmp(key, "points") == 0) {
            if (!kpt_int(c, &points)) {
                return -1;
            }
            out->target_points = (int)points;
            *points_seen = 1;
        } else if (strcmp(key, "asset") == 0) {
            if (kpt_peek(c) == '"') {
                if (!kpt_string(c, out->target_asset, sizeof(out->target_asset))) {
                    return -1;
                }
            } else if (!kpt_skip_value(c)) {
                return -1;
            }
        } else {
            /* id 与未知扩展字段：忽略、不消费（设备只显示 name/points/asset）。 */
            if (!kpt_skip_value(c)) {
                return -1;
            }
        }
        kpt_ws(c);
        d = kpt_peek(c);
        if (d == ',') {
            c->pos++;
            continue;
        }
        if (d == '}') {
            c->pos++;
            break;
        }
        return -1;
    }
    return 0;
}

int kidpal_points_home_parse(const char *data_json, kidpal_points_home_t *out)
{
    kpt_cursor c;
    int seen_rev = 0, seen_bal = 0, seen_target = 0;
    int target_name_seen = 0, target_points_seen = 0;
    long v = 0;

    if (data_json == NULL || out == NULL) {
        return -1; /* NULL 载荷 / NULL out -> 显式降级 */
    }
    memset(out, 0, sizeof(*out)); /* 复用缓冲不残留上一帧权威值 */

    c.s = data_json;
    c.pos = 0;
    kpt_ws(&c);
    if (kpt_peek(&c) != '{') {
        return -1;
    }
    c.pos++;
    kpt_ws(&c);
    if (kpt_peek(&c) == '}') {
        return -1; /* {} 缺 revision/balance/target */
    }
    for (;;) {
        char key[KPT_KEY_BUF];
        char d;

        kpt_ws(&c);
        if (!kpt_string(&c, key, sizeof(key))) {
            return -1;
        }
        kpt_ws(&c);
        if (kpt_peek(&c) != ':') {
            return -1;
        }
        c.pos++;
        kpt_ws(&c);
        if (strcmp(key, "revision") == 0) {
            if (!kpt_int(&c, &v)) {
                return -1;
            }
            out->revision = (int)v;
            seen_rev = 1;
        } else if (strcmp(key, "balance") == 0) {
            if (!kpt_int(&c, &v)) {
                return -1;
            }
            out->balance = (int)v;
            seen_bal = 1;
        } else if (strcmp(key, "target") == 0) {
            if (kpt_parse_target(&c, out, &target_name_seen, &target_points_seen) != 0) {
                return -1;
            }
            seen_target = 1;
        } else if (strcmp(key, "recent") == 0) {
            int count = 0;
            if (kpt_peek(&c) != '[') {
                return -1;
            }
            c.pos++;
            kpt_ws(&c);
            if (kpt_peek(&c) == ']') {
                c.pos++; /* 空 recent 合法（权威空账） */
            } else {
                for (;;) {
                    if (count >= KPTS_RECENT_MAX) {
                        return -1; /* recent 超过三条 = 载荷违反契约 */
                    }
                    if (kpt_parse_entry(&c, &out->recent[count]) != 0) {
                        return -1;
                    }
                    count++;
                    kpt_ws(&c);
                    d = kpt_peek(&c);
                    if (d == ',') {
                        c.pos++;
                        continue;
                    }
                    if (d == ']') {
                        c.pos++;
                        break;
                    }
                    return -1;
                }
            }
            out->recent_count = count;
        } else if (strcmp(key, "restore") == 0) {
            if (!kpt_bool(&c, &out->restore)) {
                /* 非布尔 restore：按缺省语义归一为 0（front 侧 floor 已拒非布尔帧）。 */
                if (!kpt_skip_value(&c)) {
                    return -1;
                }
                out->restore = 0;
            }
        } else {
            /* 未知扩展字段忽略（additive 纪律，story_list_parse 同款）。 */
            if (!kpt_skip_value(&c)) {
                return -1;
            }
        }
        kpt_ws(&c);
        d = kpt_peek(&c);
        if (d == ',') {
            c.pos++;
            continue;
        }
        if (d == '}') {
            c.pos++;
            break;
        }
        return -1;
    }
    kpt_ws(&c);
    if (c.s[c.pos] != '\0') {
        return -1; /* 尾部垃圾 = 畸形 JSON */
    }
    if (!seen_rev || !seen_bal || !seen_target) {
        return -1; /* revision / balance / target 必带 */
    }
    if (!target_name_seen || !target_points_seen) {
        return -1; /* target 缺 name 或 points = 载荷违反契约 */
    }
    return 0;
}

int kidpal_points_home_takeover(const kidpal_points_home_t *frame)
{
    if (frame == NULL) {
        return -1;
    }
    /* restore=true -> 0 = 只填缓存不抢前台（D-07 / 契约 §1.3⑦）；
     * 缺省/false -> 1 = 主动展示 P3 积分主页（接管前台）。 */
    return (frame->restore != 0) ? 0 : 1;
}

/* -------------------------------------------------------------------------- *
 * kidpal_points_ledger_parse — [G3]（契约 §1.3⑧）
 * ------------------------------------------------------------------------ */

static int kpt_parse_page(kpt_cursor *c, kidpal_points_ledger_t *out,
                          int *total_seen, int *cursor_seen,
                          int *more_seen, int *reset_seen)
{
    long total = 0;

    kpt_ws(c);
    if (kpt_peek(c) != '{') {
        return -1; /* page 必为对象 */
    }
    c->pos++;
    kpt_ws(c);
    if (kpt_peek(c) == '}') {
        return -1; /* 空 page = 缺四键 */
    }
    for (;;) {
        char key[KPT_KEY_BUF];
        char d;

        kpt_ws(c);
        if (!kpt_string(c, key, sizeof(key))) {
            return -1;
        }
        kpt_ws(c);
        if (kpt_peek(c) != ':') {
            return -1;
        }
        c->pos++;
        kpt_ws(c);
        if (strcmp(key, "total") == 0) {
            if (!kpt_int(c, &total) || total < 0) {
                return -1;
            }
            out->page_total = (int)total;
            *total_seen = 1;
        } else if (strcmp(key, "next_cursor") == 0) {
            if (kpt_peek(c) == '"') {
                if (!kpt_string(c, out->next_cursor, sizeof(out->next_cursor))) {
                    return -1;
                }
            } else if (strncmp(&c->s[c->pos], "null", 4) == 0) {
                c->pos += 4;
                out->next_cursor[0] = '\0';
            } else {
                return -1; /* next_cursor 只允许 string 或 null */
            }
            *cursor_seen = 1;
        } else if (strcmp(key, "has_more") == 0) {
            if (!kpt_bool(c, &out->has_more)) {
                return -1;
            }
            *more_seen = 1;
        } else if (strcmp(key, "reset") == 0) {
            if (!kpt_bool(c, &out->reset)) {
                return -1;
            }
            *reset_seen = 1;
        } else {
            if (!kpt_skip_value(c)) {
                return -1;
            }
        }
        kpt_ws(c);
        d = kpt_peek(c);
        if (d == ',') {
            c->pos++;
            continue;
        }
        if (d == '}') {
            c->pos++;
            break;
        }
        return -1;
    }
    return 0;
}

int kidpal_points_ledger_parse(const char *data_json, kidpal_points_ledger_t *out)
{
    kpt_cursor c;
    int seen_rev = 0, seen_bal = 0;
    int total_seen = 0, cursor_seen = 0, more_seen = 0, reset_seen = 0;
    long v = 0;

    if (data_json == NULL || out == NULL) {
        return -1;
    }
    memset(out, 0, sizeof(*out));

    c.s = data_json;
    c.pos = 0;
    kpt_ws(&c);
    if (kpt_peek(&c) != '{') {
        return -1;
    }
    c.pos++;
    kpt_ws(&c);
    if (kpt_peek(&c) == '}') {
        return -1;
    }
    for (;;) {
        char key[KPT_KEY_BUF];
        char d;

        kpt_ws(&c);
        if (!kpt_string(&c, key, sizeof(key))) {
            return -1;
        }
        kpt_ws(&c);
        if (kpt_peek(&c) != ':') {
            return -1;
        }
        c.pos++;
        kpt_ws(&c);
        if (strcmp(key, "revision") == 0) {
            if (!kpt_int(&c, &v)) {
                return -1;
            }
            out->revision = (int)v;
            seen_rev = 1;
        } else if (strcmp(key, "balance") == 0) {
            if (!kpt_int(&c, &v)) {
                return -1;
            }
            out->balance = (int)v;
            seen_bal = 1;
        } else if (strcmp(key, "items") == 0) {
            int count = 0;
            if (kpt_peek(&c) != '[') {
                return -1;
            }
            c.pos++;
            kpt_ws(&c);
            if (kpt_peek(&c) == ']') {
                c.pos++; /* 权威空账 items:[] */
            } else {
                for (;;) {
                    if (count >= KPTS_LEDGER_ITEMS_MAX) {
                        return -1; /* items 超设备容量 = 降级（不静默截断） */
                    }
                    if (kpt_parse_entry(&c, &out->items[count]) != 0) {
                        return -1;
                    }
                    count++;
                    kpt_ws(&c);
                    d = kpt_peek(&c);
                    if (d == ',') {
                        c.pos++;
                        continue;
                    }
                    if (d == ']') {
                        c.pos++;
                        break;
                    }
                    return -1;
                }
            }
            out->item_count = count;
        } else if (strcmp(key, "page") == 0) {
            if (kpt_parse_page(&c, out, &total_seen, &cursor_seen,
                               &more_seen, &reset_seen) != 0) {
                return -1;
            }
        } else if (strcmp(key, "restore") == 0) {
            if (!kpt_bool(&c, &out->restore)) {
                if (!kpt_skip_value(&c)) {
                    return -1;
                }
                out->restore = 0;
            }
        } else {
            if (!kpt_skip_value(&c)) {
                return -1;
            }
        }
        kpt_ws(&c);
        d = kpt_peek(&c);
        if (d == ',') {
            c.pos++;
            continue;
        }
        if (d == '}') {
            c.pos++;
            break;
        }
        return -1;
    }
    kpt_ws(&c);
    if (c.s[c.pos] != '\0') {
        return -1;
    }
    if (!seen_rev || !seen_bal) {
        return -1; /* revision / balance 必带 */
    }
    if (!total_seen || !cursor_seen || !more_seen || !reset_seen) {
        return -1; /* page 四键必带（契约 §1.3⑧） */
    }
    return 0;
}

int kidpal_points_ledger_apply(kidpal_points_ledger_view_t *v,
                               const kidpal_points_ledger_t *f)
{
    if (v == NULL || f == NULL) {
        return -1;
    }
    if (f->reset != 0) {
        /* 丢掉旧分页缓存再装入（缓存计数被帧内容替换，不叠加）。 */
        v->revision = f->revision;
        v->cached_count = f->item_count;
        v->has_more = f->has_more;
        return 0;
    }
    if (f->revision != v->revision) {
        /* 契约 §四「不得跨版本追加」：拒绝且缓存零扰动。 */
        return -1;
    }
    v->cached_count += f->item_count;
    v->has_more = f->has_more;
    return 0;
}
