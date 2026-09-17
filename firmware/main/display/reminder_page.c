/*
 * reminder_page.c — KidPal 提醒弹窗消费面：FROZEN OFFLINE CONTRACT 的实现
 * (TaskPackage 260829-schedule-reminder, 契约 §三/§四, spec S-F1.4, 决策 20·23·24·27)。
 *
 * 语义与形状以冻结验收 firmware/main/test/schedule_reminder_b.acceptance.c 文件头为准，
 * 不得偏移；本文件的强定义在 host runner 里覆盖验收文件内的 weak 哨兵 (-2) 后被真测
 * （同 schedule_page.c / view_router.c 先例）。
 *
 * 纯 C + 自足式最小 JSON 扫描（无外部 JSON 库，frozen contract 要求）+ 纯 LVGL 渲染面
 * （lv_obj_/lv_label only，无 Board/Display class 依赖）-> host_app linux target 与设备
 * 树两侧同源可编。
 *
 * 介质禁令（决策 24）：本模块零持久层、零动态尺寸容器 —— known-id 集与队列都是定长
 * 数组、只驻 RAM（重启即全忘）；三档阶梯时刻是纯服务端配置，本文件永不出现其字面值
 * （RMD-A7 设备零本地事项源齿负向钉死）。
 *
 * 降级语义：解析畸形/缺必需字段一律 -1 —— 调用方必须显式降级、绝不入队、绝不盲渲染
 * (V27 / spec §4.0，与 kidpal_schedule_parse 同纪律)。
 */

#include "reminder_page.h"

#include "art_assets.h"
#include <stdio.h>
#include <string.h>

#include "lvgl.h"

/* -------------------------------------------------------------------------- *
 * 共用小工具
 * ------------------------------------------------------------------------ */

/* safe-truncate 拷贝：恒 NUL 结尾、绝不溢出（frozen contract 要求）。
 * 截断点落在 UTF-8 码点中间时回退到上一个完整码点边界 —— 载荷是大中文事项名，截半
 * 个汉字会在 RLCD 上糊成乱码（设备侧渲染的儿童可见文本）。 */
static size_t krm_utf8_complete_len(const char *s, size_t n)
{
    size_t i = 0;
    size_t last_ok = 0;

    while (i < n) {
        unsigned char c = (unsigned char)s[i];
        int len = 1;
        int k;
        int ok = 1;

        if ((c & 0xC0u) == 0x80u) {
            break; /* 孤立后续字节 = 非法起点，截在此前 */
        }
        if ((c & 0xE0u) == 0xC0u) {
            len = 2;
        } else if ((c & 0xF0u) == 0xE0u) {
            len = 3;
        } else if ((c & 0xF8u) == 0xF0u) {
            len = 4;
        }
        if (i + (size_t)len > n) {
            break; /* 序列被截断 */
        }
        for (k = 1; k < len; k++) {
            if (((unsigned char)s[i + (size_t)k] & 0xC0u) != 0x80u) {
                ok = 0;
                break;
            }
        }
        if (!ok) {
            break;
        }
        i += (size_t)len;
        last_ok = i;
    }
    return last_ok; /* == n 表示整串是完整码点序列 */
}

static void krm_copy_str(char *dst, size_t cap, const char *src)
{
    size_t i;

    if (dst == NULL || cap == 0) {
        return;
    }
    for (i = 0; src != NULL && src[i] != '\0' && i + 1 < cap; i++) {
        dst[i] = src[i];
    }
    dst[krm_utf8_complete_len(dst, i)] = '\0';
}

static int krm_digit(char c)
{
    return c >= '0' && c <= '9';
}

static int krm_is_blank(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

/* -------------------------------------------------------------------------- *
 * 自足式最小 JSON 扫描（无外部 JSON 库; frozen contract 要求）。
 * 只需覆盖 kidpal/alert 的 arguments 对象形状 + skip 未知扩展值。
 * ------------------------------------------------------------------------ */

typedef struct {
    const char *s;
    size_t pos;
} krm_json_cursor;

static void krmj_ws(krm_json_cursor *c)
{
    while (krm_is_blank(c->s[c->pos])) {
        c->pos++;
    }
}

static char krmj_peek(const krm_json_cursor *c)
{
    return c->s[c->pos];
}

static int krmj_hex4(krm_json_cursor *c, unsigned *out)
{
    unsigned v = 0;

    for (int i = 0; i < 4; i++) {
        char ch = c->s[c->pos];
        unsigned d;

        if (krm_digit(ch)) {
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

static int krmj_utf8_encode(unsigned cp, char tmp[4])
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

static void krmj_put(char byte, char *out, size_t cap, size_t *used)
{
    /* 超界字节被丢弃但仍继续扫描（safe-truncate，恒 NUL 结尾、绝不溢出） */
    if (out != NULL && *used + 1 < cap) {
        out[(*used)++] = byte;
    }
}

/* 解析 JSON 字符串到 out（cap 含 NUL 位; out == NULL = 仅校验不落地）。
 * 支持标准转义 + \uXXXX（含代理对 -> UTF-8）。成功 1 / 畸形 0。 */
static int krmj_string(krm_json_cursor *c, char *out, size_t cap)
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
            char plain;

            c->pos++;
            e = c->s[c->pos];
            c->pos++;

            if (e == 'u') {
                unsigned cp;
                char tmp[4];
                int n;

                if (!krmj_hex4(c, &cp)) {
                    return 0;
                }
                if (cp >= 0xD800u && cp <= 0xDBFFu) {
                    unsigned lo;

                    if (c->s[c->pos] != '\\' || c->s[c->pos + 1] != 'u') {
                        return 0; /* 孤立高代理 = 畸形 */
                    }
                    c->pos += 2;
                    if (!krmj_hex4(c, &lo) || lo < 0xDC00u || lo > 0xDFFFu) {
                        return 0;
                    }
                    cp = 0x10000u + ((cp - 0xD800u) << 10) + (lo - 0xDC00u);
                } else if (cp >= 0xDC00u && cp <= 0xDFFFu) {
                    return 0; /* 孤立低代理 = 畸形 */
                }
                n = krmj_utf8_encode(cp, tmp);
                for (int i = 0; i < n; i++) {
                    krmj_put(tmp[i], out, cap, &used);
                }
                continue;
            }

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
            krmj_put(plain, out, cap, &used);
            continue;
        }
        /* 普通 UTF-8 字节原样通过 */
        krmj_put((char)ch, out, cap, &used);
        c->pos++;
    }
    if (out != NULL) {
        out[used] = '\0';
    }
    return 1;
}

/* 跳过任意一个 JSON 值（string/object/array/number/true/false/null），
 * 兼做结构合法性校验。成功 1 / 畸形 0。 */
static int krmj_skip_value(krm_json_cursor *c)
{
    char ch;

    krmj_ws(c);
    ch = krmj_peek(c);

    if (ch == '"') {
        return krmj_string(c, NULL, 0);
    }
    if (ch == '{') {
        c->pos++;
        krmj_ws(c);
        if (krmj_peek(c) == '}') {
            c->pos++;
            return 1;
        }
        for (;;) {
            char d;

            krmj_ws(c);
            if (!krmj_string(c, NULL, 0)) {
                return 0;
            }
            krmj_ws(c);
            if (krmj_peek(c) != ':') {
                return 0;
            }
            c->pos++;
            if (!krmj_skip_value(c)) {
                return 0;
            }
            krmj_ws(c);
            d = krmj_peek(c);
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
    if (ch == '[') {
        c->pos++;
        krmj_ws(c);
        if (krmj_peek(c) == ']') {
            c->pos++;
            return 1;
        }
        for (;;) {
            char d;

            if (!krmj_skip_value(c)) {
                return 0;
            }
            krmj_ws(c);
            d = krmj_peek(c);
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
    if (ch == '-' || krm_digit(ch)) {
        c->pos++;
        while (krm_digit(c->s[c->pos]) || c->s[c->pos] == '.' ||
               c->s[c->pos] == 'e' || c->s[c->pos] == 'E' ||
               c->s[c->pos] == '+' || c->s[c->pos] == '-') {
            c->pos++;
        }
        return 1;
    }
    return 0;
}

/* -------------------------------------------------------------------------- *
 * kidpal_fire_at_minutes — ISO8601 -> 整分钟（UTC 归一）
 *
 * 算法：days_from_civil（1970-01-01 起算，含闰年/世纪规则）-> 当日秒 + 时分秒
 * -> 减偏移（+08:00 表示本地比 UTC 快 8 小时，故 UTC 瞬时 = 本地瞬时 - 偏移）
 * -> 向下取整到分钟。畸形（无 'T'、位数字错、缺/错偏移、尾部垃圾）一律 -1。
 * ------------------------------------------------------------------------ */

static long long krm_days_from_civil(long long y, long long m, long long d)
{
    /* Howard Hinnant days_from_civil：以 0000-03-01 为纪元起点，免查表得公历日序 */
    long long era;
    long long yoe;
    long long doy;
    long long doe;
    long long toe;

    y -= (m <= 2);
    era = (y >= 0 ? y : y - 399) / 400;
    yoe = y - era * 400;                                  /* [0, 399] */
    doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1; /* [0, 365] */
    doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;          /* [0, 146096] */
    toe = era * 146097 + doe - 719468;
    return toe;
}

/* 日历合法性：位数对但日期不存在（2026-02-30 / 2026-13-01）按畸形处理 —— 载荷来自
 * 云端，设备端仍是系统边界（与网关 validate_alert_entry 同一口径，不静默造一个瞬时）。 */
static int krm_date_valid(long long y, long long m, long long d)
{
    static const int month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    long long limit;

    if (m < 1 || m > 12) {
        return 0;
    }
    limit = month_days[m - 1];
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) {
        limit = 29; /* 闰年 2 月 29 日合法 */
    }
    return d >= 1 && d <= limit;
}

long long kidpal_fire_at_minutes(const char *fire_at_iso)
{
    long long year;
    long long mon;
    long long day;
    long long hour;
    long long min;
    long long sec = 0;
    long long days;
    long long secs;
    int off_sign = 0;
    int off_h = 0;
    int off_m = 0;
    const char *p;

    if (fire_at_iso == NULL) {
        return -1; /* NULL 防御 -> 降级 */
    }
    p = fire_at_iso;

    /* YYYY-MM-DD */
    if (!krm_digit(p[0]) || !krm_digit(p[1]) || !krm_digit(p[2]) || !krm_digit(p[3]) ||
        p[4] != '-' || !krm_digit(p[5]) || !krm_digit(p[6]) || p[7] != '-' ||
        !krm_digit(p[8]) || !krm_digit(p[9])) {
        return -1;
    }
    year = (p[0] - '0') * 1000 + (p[1] - '0') * 100 + (p[2] - '0') * 10 + (p[3] - '0');
    mon = (p[5] - '0') * 10 + (p[6] - '0');
    day = (p[8] - '0') * 10 + (p[9] - '0');
    if (!krm_date_valid(year, mon, day)) {
        return -1; /* 日历不存在的时刻 = 畸形（含 2 月 30 日、13 月） */
    }
    p += 10;
    if (*p != 'T' && *p != 't') {
        return -1; /* 无 'T' 分隔（如 "2026-09-09 16:15:00"）-> 畸形 */
    }
    p++;

    /* HH:MM:SS */
    if (!krm_digit(p[0]) || !krm_digit(p[1]) || p[2] != ':' ||
        !krm_digit(p[3]) || !krm_digit(p[4]) || p[5] != ':' ||
        !krm_digit(p[6]) || !krm_digit(p[7])) {
        return -1;
    }
    hour = (p[0] - '0') * 10 + (p[1] - '0');
    min = (p[3] - '0') * 10 + (p[4] - '0');
    sec = (p[6] - '0') * 10 + (p[7] - '0');
    if (hour > 23 || min > 59 || sec > 60) {
        return -1;
    }
    p += 8;

    /* 可选小数秒（向下截断） */
    if (*p == '.') {
        p++;
        if (!krm_digit(*p)) {
            return -1;
        }
        while (krm_digit(*p)) {
            p++;
        }
    }

    /* 偏移：Z / +HH:MM / -HH:MM（缺偏移 = 畸形，本包载荷恒带偏移） */
    if (*p == 'Z' || *p == 'z') {
        p++;
    } else if (*p == '+' || *p == '-') {
        off_sign = (*p == '-') ? -1 : 1;
        p++;
        if (!krm_digit(p[0]) || !krm_digit(p[1]) || p[2] != ':' ||
            !krm_digit(p[3]) || !krm_digit(p[4])) {
            return -1;
        }
        off_h = (p[0] - '0') * 10 + (p[1] - '0');
        off_m = (p[3] - '0') * 10 + (p[4] - '0');
        if (off_h > 23 || off_m > 59) {
            return -1;
        }
        p += 5;
    } else {
        return -1;
    }

    if (*p != '\0') {
        return -1; /* 尾部非数字垃圾 = 畸形 */
    }

    days = krm_days_from_civil(year, mon, day);
    if (days < -1 || days > 2932896LL) {
        return -1; /* 公元 9999 年后无意义 -> 降级（远大于任何课表时刻） */
    }
    secs = days * 86400LL + hour * 3600LL + min * 60LL + sec;
    secs -= off_sign * (off_h * 3600LL + off_m * 60LL);

    /* 向下取整到分钟（C 除法向零截断，负数需手动 floor） */
    if (secs >= 0) {
        return secs / 60;
    }
    return -((-secs + 59) / 60);
}

/* -------------------------------------------------------------------------- *
 * kidpal_alert_parse — 契约 §三 载荷到端解析（畸形 = 显式降级，绝不盲渲染）
 * ------------------------------------------------------------------------ */

#define KRM_KEY_BUF 24

int kidpal_alert_parse(const char *args_json, kidpal_alert_t *out)
{
    krm_json_cursor c;
    char id_buf[KRM_CAP_ID];
    char fire_buf[KRM_CAP_FIRE];
    char title_buf[KRM_CAP_TITLE];
    char speech_buf[KRM_CAP_SPEECH];
    char view_buf[KRM_CAP_VIEW];
    kidpal_alert_t parsed;
    int got_id = 0;
    int got_title = 0;
    int got_fire = 0;

    if (args_json == NULL || out == NULL) {
        return -1; /* NULL 载荷 / NULL 出参 -> 降级 */
    }
    id_buf[0] = '\0';
    fire_buf[0] = '\0';
    title_buf[0] = '\0';

    memset(&parsed, 0, sizeof(parsed));
    memset(speech_buf, 0, sizeof(speech_buf)); /* 缺 speech -> "" */
    memset(view_buf, 0, sizeof(view_buf));     /* 缺 view     -> "" */

    c.s = args_json;
    c.pos = 0;
    krmj_ws(&c);
    if (krmj_peek(&c) != '{') {
        return -1;
    }
    c.pos++;
    krmj_ws(&c);
    if (krmj_peek(&c) == '}') {
        return -1; /* {} 缺 id/title/fire_at —— 必带字段不齐 */
    }

    for (;;) {
        char key[KRM_KEY_BUF];

        krmj_ws(&c);
        if (!krmj_string(&c, key, sizeof(key))) {
            return -1;
        }
        krmj_ws(&c);
        if (krmj_peek(&c) != ':') {
            return -1;
        }
        c.pos++;
        krmj_ws(&c);

        if (strcmp(key, "id") == 0) {
            if (krmj_peek(&c) != '"') {
                return -1; /* §三 业务 id 必须是字符串 token（数字 JSON-RPC 传输 id 是另一件事） */
            }
            if (!krmj_string(&c, id_buf, sizeof(id_buf))) {
                return -1;
            }
            got_id = 1;
        } else if (strcmp(key, "title") == 0) {
            if (krmj_peek(&c) != '"') {
                return -1; /* 弹窗大字必带 */
            }
            if (!krmj_string(&c, title_buf, sizeof(title_buf))) {
                return -1;
            }
            got_title = 1;
        } else if (strcmp(key, "fire_at") == 0) {
            if (krmj_peek(&c) != '"') {
                return -1;
            }
            if (!krmj_string(&c, fire_buf, sizeof(fire_buf))) {
                return -1;
            }
            got_fire = 1;
        } else if (strcmp(key, "speech") == 0) {
            if (krmj_peek(&c) != '"') {
                return -1; /* speech 存在但非串 -> 降级（缺省才容忍为 ""） */
            }
            if (!krmj_string(&c, speech_buf, sizeof(speech_buf))) {
                return -1;
            }
        } else if (strcmp(key, "view") == 0) {
            if (krmj_peek(&c) != '"') {
                return -1; /* 设备 wire 上 view 以 JSON 字符串承载；非串 -> 降级 */
            }
            if (!krmj_string(&c, view_buf, sizeof(view_buf))) {
                return -1;
            }
        } else {
            /* 未知扩展字段：值必须仍是合法 JSON，但内容忽略（additive tolerance） */
            if (!krmj_skip_value(&c)) {
                return -1;
            }
        }

        krmj_ws(&c);
        {
            char d = krmj_peek(&c);

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
    }

    krmj_ws(&c);
    if (c.s[c.pos] != '\0') {
        return -1; /* 尾部垃圾 = 畸形 JSON */
    }
    if (!got_id || !got_title || !got_fire) {
        return -1; /* 缺 id（幂等去重与回执关联无锚）/ 缺 title / 缺 fire_at */
    }
    if (id_buf[0] == '\0' || title_buf[0] == '\0') {
        return -1; /* 空串 id / 空串 title = 非法载荷 */
    }
    if (kidpal_fire_at_minutes(fire_buf) < 0) {
        return -1; /* fire_at 不可解析 -> V18 顶替判定失去算术依据，整条降级 */
    }

    krm_copy_str(parsed.id, sizeof(parsed.id), id_buf);
    krm_copy_str(parsed.fire_at, sizeof(parsed.fire_at), fire_buf);
    krm_copy_str(parsed.title, sizeof(parsed.title), title_buf);
    krm_copy_str(parsed.speech, sizeof(parsed.speech), speech_buf);
    krm_copy_str(parsed.view_json, sizeof(parsed.view_json), view_buf);
    *out = parsed;
    return 0;
}

/* -------------------------------------------------------------------------- *
 * 提醒状态机（四态 received / queued / rendered / acked；定长存储、只驻 RAM）
 * ------------------------------------------------------------------------ */

typedef struct {
    char id[KRM_CAP_ID];
    char fire_at[KRM_CAP_FIRE];
    char title[KRM_CAP_TITLE];
    char speech[KRM_CAP_SPEECH];
    unsigned char rendered; /* 已完成渲染（回执许可前置） */
    unsigned char armed;    /* 回执许可：渲染完成置 1，回执发出清 0 */
    unsigned char acked;    /* 已确认出队 */
} krm_item_t;

struct kidpal_reminder_s {
    char known[KRM_KNOWN_CAP][KRM_CAP_ID]; /* 易失 known-id 集（幂等去重，屏蔽重复入队） */
    int known_count;
    int known_cursor;              /* 集满时的最旧写位（FIFO 淘汰） */
    krm_item_t queue[KRM_QUEUE_CAP];
    int q_head;                    /* 环形 FIFO 头 */
    int q_len;
    krm_item_t shown;              /* 驻留弹窗（无超时，驻留到按键/V18 顶替） */
    int has_shown;
};

/* host runner 用 4096 字节裸缓冲承载状态实例（冻结验收 RMD-A5/A6 齿的纪律）——
 * 定长且有界是本模块的 RAM 契约，超界即内存踩踏。此静态守卫把它钉成编译期事实。 */
_Static_assert(sizeof(kidpal_reminder_t) <= 4096u,
               "kidpal_reminder_t must fit the frozen 4096-byte bare buffer");

static kidpal_reminder_t krm_default_state; /* 设备侧唯一弹窗状态（定长静态、零动态分配） */

kidpal_reminder_t *kidpal_reminder_default(void)
{
    return &krm_default_state;
}

static int krm_known_index(const kidpal_reminder_t *s, const char *id)
{
    int i;

    for (i = 0; i < s->known_count; i++) {
        if (strcmp(s->known[i], id) == 0) {
            return i;
        }
    }
    return -1;
}

static int krm_queue_has(const kidpal_reminder_t *s, const char *id)
{
    int i;

    for (i = 0; i < s->q_len; i++) {
        if (strcmp(s->queue[(s->q_head + i) % KRM_QUEUE_CAP].id, id) == 0) {
            return 1;
        }
    }
    return 0;
}

/* 记入 known-id 集。集满时淘汰最旧且**不在驻留/排队**的条目 —— 驻留与排队项被遗忘
 * 会让同 id 重推被当成新提醒二次弹窗（重复弹窗根除红线），故淘汰必须绕开它们。 */
static void krm_known_record(kidpal_reminder_t *s, const char *id)
{
    int slot = -1;
    int i;

    if (s->known_count < KRM_KNOWN_CAP) {
        slot = s->known_count;
        s->known_count++;
    } else {
        for (i = 0; i < KRM_KNOWN_CAP; i++) {
            int idx = (s->known_cursor + i) % KRM_KNOWN_CAP;
            const char *victim = s->known[idx];

            if (s->has_shown && strcmp(victim, s->shown.id) == 0) {
                continue; /* 驻留项不得被遗忘 */
            }
            if (krm_queue_has(s, victim)) {
                continue; /* 排队项不得被遗忘 */
            }
            slot = idx;
            break;
        }
        if (slot < 0) {
            slot = s->known_cursor; /* 全被占用（队列上界 8 << 集容量 32，理论不可达） */
        }
    }
    krm_copy_str(s->known[slot], KRM_CAP_ID, id);
    s->known_cursor = (slot + 1) % KRM_KNOWN_CAP;
}

static long long krm_event_minutes(const krm_item_t *it)
{
    long long m = kidpal_fire_at_minutes(it->fire_at);

    /* fire_at 不可解析（合法路径下不会发生：入队前 kidpal_alert_parse 已拒）按
     * 「事件已过」处理，绝不让一条脏项永久卡住队列（过期即弃纪律）。 */
    return (m < 0) ? 0 : m;
}

static void krm_store_alert(krm_item_t *it, const kidpal_alert_t *a)
{
    krm_copy_str(it->id, sizeof(it->id), a->id);
    krm_copy_str(it->fire_at, sizeof(it->fire_at), a->fire_at);
    krm_copy_str(it->title, sizeof(it->title), a->title);
    krm_copy_str(it->speech, sizeof(it->speech), a->speech);
    it->rendered = 0;
    it->armed = 0;
    it->acked = 0;
}

static void krm_pop_head(kidpal_reminder_t *s, krm_item_t *dst)
{
    int old_head = s->q_head;

    *dst = s->queue[old_head];
    s->q_head = (old_head + 1) % KRM_QUEUE_CAP;
    s->q_len--;
    memset(&s->queue[old_head], 0, sizeof(s->queue[old_head])); /* 弹出位清零，不残留旧载荷 */
}

void kidpal_reminder_init(kidpal_reminder_t *s)
{
    if (s == NULL) {
        return;
    }
    memset(s, 0, sizeof(*s));
}

int kidpal_reminder_receive(kidpal_reminder_t *s, const kidpal_alert_t *a)
{
    int known_idx;

    if (s == NULL || a == NULL || a->id[0] == '\0') {
        return KRM_STATE_INVALID; /* NULL / 无幂等锚 -> 拒绝，状态零污染 */
    }

    known_idx = krm_known_index(s, a->id);
    if (known_idx >= 0) {
        /* 幂等去重：已知 id 绝不重复入队（关掉一个不会再弹一个） */
        if (s->has_shown && s->shown.rendered && !s->shown.acked &&
            strcmp(s->shown.id, a->id) == 0) {
            /* 已渲染驻留项被重推（回执在途丢失）-> 补回回执、不重新弹窗。
             * 已 acked 的驻留项 = 已确认出队（一 id 一回执, RM-A5 AC4「关掉一个不会
             * 再弹一个」）：重推按已知 id 零动作, 不许二次武装回执、不许复活重弹。 */
            s->shown.armed = 1;
            return KRM_STATE_RENDERED;
        }
        return KRM_STATE_RECEIVED;
    }

    krm_known_record(s, a->id); /* 接收即入易失集（与渲染无关） */

    if (s->q_len >= KRM_QUEUE_CAP) {
        /* 队列有界：溢出既不入队也不崩，且 id 已记账 -> 后续重推也进不来 */
        return KRM_STATE_RECEIVED;
    }
    krm_store_alert(&s->queue[(s->q_head + s->q_len) % KRM_QUEUE_CAP], a);
    s->q_len++;
    return KRM_STATE_QUEUED;
}

int kidpal_reminder_known_count(const kidpal_reminder_t *s)
{
    if (s == NULL) {
        return KRM_STATE_INVALID;
    }
    return s->known_count;
}

int kidpal_reminder_queue_count(const kidpal_reminder_t *s)
{
    if (s == NULL) {
        return KRM_STATE_INVALID;
    }
    return s->q_len;
}

int kidpal_reminder_shown_id(const kidpal_reminder_t *s, char *out, size_t cap)
{
    if (s == NULL || out == NULL) {
        return KRM_STATE_INVALID;
    }
    if (!s->has_shown) {
        return 0; /* 无弹窗：out 不动 */
    }
    krm_copy_str(out, cap, s->shown.id);
    return 1;
}

int kidpal_reminder_shown_alert(const kidpal_reminder_t *s, kidpal_alert_t *out)
{
    if (s == NULL || out == NULL) {
        return KRM_STATE_INVALID;
    }
    if (!s->has_shown) {
        return 0;
    }
    memset(out, 0, sizeof(*out));
    krm_copy_str(out->id, sizeof(out->id), s->shown.id);
    krm_copy_str(out->fire_at, sizeof(out->fire_at), s->shown.fire_at);
    krm_copy_str(out->title, sizeof(out->title), s->shown.title);
    krm_copy_str(out->speech, sizeof(out->speech), s->shown.speech);
    /* view_json 不经队列面（弹窗正文取 title；内嵌 display 块由 display 面按 §1.3
     * 白名单承载）—— 定长有界存储的取舍，见本文件头介质/有界性注记。 */
    return 1;
}

int kidpal_reminder_raise_next(kidpal_reminder_t *s, long long now_min)
{
    if (s == NULL) {
        return -1;
    }

    if (s->has_shown) {
        /* V18 滞留顶替：只允许顶替「已渲染完成且事件时刻已过」的永未按键弹窗 */
        if (s->shown.rendered && now_min >= krm_event_minutes(&s->shown) && s->q_len > 0) {
            krm_pop_head(s, &s->shown); /* 顶替：被顶替项早已出队，不复活、不追加送达 */
            s->has_shown = 1;
            return 2;
        }
        return 0; /* 未渲染（不叠屏）/ 事件未到 / 队列空 */
    }

    if (s->q_len > 0) {
        krm_pop_head(s, &s->shown);
        s->has_shown = 1; /* 弹出即驻留（rendered/armed/acked 由 krm_pop_head 归零） */
        return 1;
    }
    return 0;
}

int kidpal_reminder_may_emit_receipt(const kidpal_reminder_t *s)
{
    if (s == NULL) {
        return KRM_STATE_INVALID;
    }
    if (!s->has_shown || !s->shown.rendered || !s->shown.armed) {
        return 0; /* 接收/排队期、未渲染完成、回执已发 —— 一律不许发送达确认 */
    }
    return 1;
}

int kidpal_reminder_mark_rendered(kidpal_reminder_t *s, const char *id)
{
    if (s == NULL || id == NULL) {
        return KRM_STATE_INVALID;
    }
    if (!s->has_shown || strcmp(s->shown.id, id) != 0) {
        return 0; /* 非驻留弹窗：判定门不可被伪造 */
    }
    s->shown.rendered = 1;
    if (!s->shown.acked) {
        s->shown.armed = 1; /* 渲染即确认：一 id 一回执（已 acked 的不二次武装） */
    }
    return 1;
}

int kidpal_reminder_mark_acked(kidpal_reminder_t *s, const char *id)
{
    if (s == NULL || id == NULL) {
        return KRM_STATE_INVALID;
    }
    if (!s->has_shown || strcmp(s->shown.id, id) != 0) {
        return 0; /* 未知 id / 非驻留项 */
    }
    if (!s->shown.rendered || !s->shown.armed) {
        return 0; /* 未渲染不得确认；回执已发不得二次确认 */
    }
    s->shown.acked = 1;
    s->shown.armed = 0;
    return 1;
}

int kidpal_reminder_render_actions(const kidpal_alert_t *a)
{
    int actions;

    if (a == NULL || a->id[0] == '\0') {
        return -1; /* NULL / 无幂等锚 -> 不给任何动作（不盲投递） */
    }
    actions = KRM_ACT_SHOW | KRM_ACT_TONE; /* 弹窗 + 提示音必达（决策 27 三件套） */
    if (a->speech[0] != '\0') {
        actions |= KRM_ACT_SPEAK;          /* 语音是可选增强：仅有文案时才要播报 */
    }
    return actions;
}

int kidpal_reminder_key(kidpal_reminder_t *s, key_id_t key, key_gesture_t g,
                        page_id_t current, page_id_t *page_out)
{
    if (s == NULL || page_out == NULL) {
        return -1;
    }
    *page_out = current;
    if (!s->has_shown) {
        return KRM_KEY_PASS; /* 无弹窗 -> 完全不介入下层手势 */
    }

    if (key == KEY_ID_KEY && g == KEY_GESTURE_CLICK) {
        s->has_shown = 0; /* 关闭回当前页（条目已于渲染时确认出队） */
        memset(&s->shown, 0, sizeof(s->shown));
        *page_out = current;
        return KRM_KEY_DISMISS;
    }
    if (key == KEY_ID_BOOT && g == KEY_GESTURE_CLICK) {
        s->has_shown = 0; /* 关闭并回原页不翻页：覆盖层 BOOT 密封口径（v1.39 隔离,
                           * requirements §2.3 提醒弹窗行——覆盖层内 BOOT 不产翻页动作,
                           * 260915-m1-k1a KD-A4 修订） */
        memset(&s->shown, 0, sizeof(s->shown));
        *page_out = current;
        return KRM_KEY_DISMISS;
    }
    if (key == KEY_ID_KEY && g == KEY_GESTURE_LONG_PRESS) {
        s->has_shown = 0; /* 关层回原页：v1.36 返回上一层在覆盖层顶层的实例, 不再
                           * 恒回首页（KD-A3 layer-aware 裁决, 260915-m1-k1a KD-A4 修订） */
        memset(&s->shown, 0, sizeof(s->shown));
        *page_out = current;
        return KRM_KEY_DISMISS;
    }
    return KRM_KEY_PASS; /* BOOT 长按 / 双击 / 越界键位：不吞、驻留不变 */
}

/* -------------------------------------------------------------------------- *
 * 提示音缝（三件套的提示音一半）
 *
 * 弱默认 = 不动作（host runner / 无音源平台可链接）；设备侧强定义在
 * firmware/main/mcp_server.cc —— 经 Application::PlaySound(Lang::Sounds::OGG_POPUP)
 * 出音（小智既有 popup 音源，先例 application.cc audio_service_.PlaySound）。
 * ------------------------------------------------------------------------ */
__attribute__((weak)) void kidpal_reminder_play_popup_tone(void)
{
    /* host / 无声源平台默认无音：弹窗与 TTS 决策不受影响（RMD-A3 必达面在动作集里） */
}

/* -------------------------------------------------------------------------- *
 * kidpal_reminder_page_render — 渲染面（纯 LVGL，活动屏）
 *
 * spec S-F1.4 弹窗形态：整屏压（d50 网点聚焦）+ 小伴 pose-jump 位 + 大字事项 +
 * 时间行（fire_at 的 HH:MM）+ 语音缺省时的「仅弹窗」形态。
 * minimal: 网点层按 16bit/32bit 色深用半透明压屏实现「压」的等效灰度（LVGL 软件
 * 渲染器混合）；1bpp 面板的真 halftone 网点疏密与小伴 pose 位图属资产管线/9b
 * （docs/ui/kidpal-ui-design-v2.html data-od-id="state-reminder" 是母版）。
 * 升级路径 = 接 xiaozhi-fonts 中文字库与 pose 位图资源，几何不变。
 * ------------------------------------------------------------------------ */

#define KRM_CARD_X_PCT   12 /* 卡片左上 x（% 屏宽），母版 58/400 */
#define KRM_CARD_W_PCT   76 /* 卡片宽（% 屏宽），母版 284/400 */
#define KRM_CARD_Y_PCT   14 /* 卡片上沿 y（% 屏高） */
#define KRM_CARD_H_PCT   58 /* 卡片高（% 屏高） */
#define KRM_COLOR_INK    0x202020u
#define KRM_COLOR_BG     0xFFFFFFu
#define KRM_BIG_SCALE    512 /* 200% -> 「大字事项」（LVGL 9.3 transform scale, 256=100%） */

static lv_obj_t *krm_fill_rect(lv_obj_t *parent, int32_t x, int32_t y, int32_t w,
                               int32_t h, uint32_t color, lv_opa_t opa)
{
    lv_obj_t *r = lv_obj_create(parent);

    lv_obj_remove_style_all(r);
    lv_obj_set_size(r, w, h);
    lv_obj_set_pos(r, x, y);
    lv_obj_set_style_bg_color(r, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(r, opa, 0);
    lv_obj_clear_flag(r, LV_OBJ_FLAG_SCROLLABLE);
    return r;
}

static lv_obj_t *krm_text(lv_obj_t *parent, const char *text, int32_t x, int32_t y,
                          int32_t w, int32_t h, uint32_t color)
{
    lv_obj_t *label = lv_label_create(parent);

    lv_label_set_text(label, (text != NULL) ? text : "");
    lv_obj_set_pos(label, x, y);
    lv_obj_set_size(label, w, h);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    return label;
}

/* fire_at -> "HH:MM"（UTC 归一后的瞬时按载荷自身偏移展示：直接取字面时分，
 * 孩子看到的是课表里的那个时刻，不做设备时区二次换算——设备无时区配置面）。 */
static void krm_hhmm_of(const char *fire_at, char *out, size_t cap)
{
    if (fire_at != NULL && krm_digit(fire_at[11]) && krm_digit(fire_at[12]) &&
        fire_at[13] == ':' && krm_digit(fire_at[14]) && krm_digit(fire_at[15])) {
        snprintf(out, cap, "%c%c:%c%c", fire_at[11], fire_at[12], fire_at[14], fire_at[15]);
        return;
    }
    krm_copy_str(out, cap, "--:--"); /* 不可解析（合法路径不达）-> 明确占位，不空白 */
}

/* -------------------------------------------------------------------------- *
 * 入层门控 + 时间行单源（TaskPackage 260915-m1-k4-reminder-overlay-unify, frozen
 * seam [1]/[2]）。数值与存储语义 = reminder_page.h 声明 + 冻结验收文件头逐字一致。
 * ------------------------------------------------------------------------ */

int kidpal_reminder_gate(int front_layer, int recording)
{
    if (front_layer < 0 || (recording != 0 && recording != 1)) {
        return KRM_GATE_INVALID; /* 越界前台/录音入参 -> 决策表防线, 从不为 UB */
    }
    if (recording != 0) {
        return KRM_GATE_DEFER; /* 唯一例外 = 录音态：对任何前台一律延后（排队, 防提醒
                                * TTS 混进录音内容; 会话结束由调用方触发恢复弹出） */
    }
    return KRM_GATE_RAISE; /* 非录音前台一律允许入层弹窗（最高层覆盖 RM-A2; 实际压层由
                            * layer 栈优先级承载, 门控只锁「允许入层尝试」） */
}

int kidpal_reminder_hhmm(const char *fire_at, char *out, size_t cap)
{
    if (out == NULL || cap < 6) {
        return -1; /* out NULL / cap < 6 -> "--:--"+NUL 最小 6 字节契约 */
    }
    /* fire_at 字面 11..15 位组 "HH:MM"（child 看到课表里的时刻, 不做时区二次换算） */
    if (fire_at != NULL && krm_digit(fire_at[11]) && krm_digit(fire_at[12]) &&
        fire_at[13] == ':' && krm_digit(fire_at[14]) && krm_digit(fire_at[15])) {
        snprintf(out, cap, "%c%c:%c%c", fire_at[11], fire_at[12], fire_at[14], fire_at[15]);
        return 0;
    }
    krm_copy_str(out, cap, "--:--"); /* 形状不可读（含 NULL）-> 明确占位, 不空白 */
    return 1;
}

/* 小伴 pose-jump 位（占位几何：头 + 身 + 惊叫气泡）。母版是 SVG 位图资产，
 * 位图接入随资产管线（9b）；此处按同位置同尺寸画等价灰度块，保证布局可测。 */
static void krm_pose_jump(lv_obj_t *parent, int32_t x, int32_t y, int32_t size)
{
    lv_obj_t *head;
    lv_obj_t *body;

    head = lv_obj_create(parent);
    lv_obj_remove_style_all(head);
    lv_obj_set_size(head, size * 2 / 5, size * 2 / 5);
    lv_obj_set_pos(head, x + size * 3 / 10, y);
    lv_obj_set_style_radius(head, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(head, lv_color_hex(KRM_COLOR_INK), 0);
    lv_obj_set_style_bg_opa(head, LV_OPA_COVER, 0);

    body = lv_obj_create(parent);
    lv_obj_remove_style_all(body);
    lv_obj_set_size(body, size * 3 / 4, size / 2);
    lv_obj_set_pos(body, x + size / 8, y + size / 2);
    lv_obj_set_style_radius(body, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(body, lv_color_hex(KRM_COLOR_INK), 0);
    lv_obj_set_style_bg_opa(body, LV_OPA_COVER, 0);
}

/*
 * 换皮消费接线 (包 9b HWV-A6)：真·吉祥物姿态位图接入。经资产层 loader
 * (kidpal_art_load + kidpal_art_pixel) 把 pose_jump_96 KAP1 合成成一枚 ARGB8888
 * lv_image_dsc (墨=不透明黑 / 纸及非阈值化=透明)，返回其描述符；不可用则 NULL。
 * 弹窗左上角既有 krm_pose_jump 矢量占位（payload 无关、恒同），本位图叠加于同位
 * 同尺寸 -> 不遮大字（卡右）、不压时间行（卡下），故 RMD-A5 的 diff（标题/时间随
 * 载荷变）与 accum==0（重渲染恒同）两齿都不受影响；几何只依赖 payload 无关的
 * card 尺寸 -> 确定性。未 mount / 加载失败 -> NULL，页面回矢量占位。
 *
 * 介质禁令 (RMD-A6/A7, 决策 24 哑终端纯洁性)：本模块「零动态分配、定长有界 RAM」。
 * 故位图缓冲用编译期定长静态数组承载（pose 盒上界 KRM_POSE_BOX=96，96*96 像素），
 * 绝不走堆。仅首次加载成功时填充一次、之后作为 lv_image 只读数据源长期驻留。
 */
#define KRM_POSE_BOX 96
static lv_image_dsc_t s_krm_pose_dsc;
static uint32_t       s_krm_pose_buf[KRM_POSE_BOX * KRM_POSE_BOX]; /* 定长、非堆 */
static int            s_krm_pose_st = 0; /* 0=未试 1=可用 2=不可用 */

static const lv_image_dsc_t *krm_pose_jump_bitmap_dsc(void)
{
    kidpal_art_img_t img;

    if (s_krm_pose_st == 2) {
        return NULL;
    }
    if (s_krm_pose_st == 1) {
        return &s_krm_pose_dsc;
    }
    if (kidpal_art_load("pose_jump_96.bin", &img) != 0 ||
        img.w <= 0 || img.h <= 0 || img.w > KRM_POSE_BOX || img.h > KRM_POSE_BOX) {
        s_krm_pose_st = 2;
        return NULL;
    }
    for (int y = 0; y < img.h; y++) {
        for (int x = 0; x < img.w; x++) {
            int p = kidpal_art_pixel(&img, x, y);
            s_krm_pose_buf[y * img.w + x] = (p == 1) ? 0xFF000000u : 0x00000000u;
        }
    }
    memset(&s_krm_pose_dsc, 0, sizeof(s_krm_pose_dsc));
    s_krm_pose_dsc.header.cf     = LV_COLOR_FORMAT_ARGB8888;
    s_krm_pose_dsc.header.w      = img.w;
    s_krm_pose_dsc.header.h      = img.h;
    s_krm_pose_dsc.header.stride = (uint32_t)img.w * 4u;
    s_krm_pose_dsc.data          = (const uint8_t *)s_krm_pose_buf;
    s_krm_pose_dsc.data_size     = (uint32_t)(img.w * img.h * 4);
    s_krm_pose_st = 1;
    return &s_krm_pose_dsc;
}

/* -------------------------------------------------------------------------- *
 * 提醒态整屏压暗 (包 260911-render-pages RP-A9/F4)：用 d50 网点资产 (B8 闭集, 4px 格,
 * 单屏稀疏) 合成 ARGB8888 tile, 供弹窗外整屏平铺压暗 —— 常规位深偏移的半透明填充
 * 退役, 改 d50 网点合成 (注释与实现一致)。未 mount/失败 -> NULL (best-effort, 页面
 * 回浅底)。
 * ------------------------------------------------------------------------ */
#define KRM_DOTS_MAX 16
static lv_image_dsc_t s_krm_dots_dsc;
static uint32_t       s_krm_dots_px[KRM_DOTS_MAX * KRM_DOTS_MAX]; /* 定长、非堆 (RMD-A6 禁令) */
static int            s_krm_dots_st  = 0; /* 0=未试 1=可用 2=不可用 */

static const lv_image_dsc_t *krm_d50_overlay_dsc(void)
{
    kidpal_art_img_t img;

    if (s_krm_dots_st == 2) {
        return NULL;
    }
    if (s_krm_dots_st == 1) {
        return &s_krm_dots_dsc;
    }
    if (kidpal_art_load("d50.bin", &img) != 0 ||
        img.w <= 0 || img.h <= 0 || img.w > KRM_DOTS_MAX || img.h > KRM_DOTS_MAX) {
        s_krm_dots_st = 2;
        return NULL;
    }
    for (int y = 0; y < img.h; y++) {
        for (int x = 0; x < img.w; x++) {
            int p = kidpal_art_pixel(&img, x, y);
            s_krm_dots_px[y * img.w + x] = (p == 1) ? 0xFF000000u : 0x00000000u;
        }
    }
    memset(&s_krm_dots_dsc, 0, sizeof(s_krm_dots_dsc));
    s_krm_dots_dsc.header.cf     = LV_COLOR_FORMAT_ARGB8888;
    s_krm_dots_dsc.header.w      = img.w;
    s_krm_dots_dsc.header.h      = img.h;
    s_krm_dots_dsc.header.stride = (uint32_t)img.w * 4u;
    s_krm_dots_dsc.data          = (const uint8_t *)s_krm_dots_px;
    s_krm_dots_dsc.data_size     = (uint32_t)(img.w * img.h * 4);
    s_krm_dots_st = 1;
    return &s_krm_dots_dsc;
}

int kidpal_reminder_page_render(const kidpal_alert_t *a)
{
    lv_obj_t *scr;
    lv_obj_t *card;
    lv_obj_t *big;
    int32_t w;
    int32_t h;
    int32_t card_x;
    int32_t card_y;
    int32_t card_w;
    int32_t card_h;
    char hhmm[8];
    int actions;

    /* 参数校验先行：非法载荷不动屏、不崩（frozen contract） */
    if (a == NULL || a->id[0] == '\0') {
        return -1;
    }
    actions = kidpal_reminder_render_actions(a);
    if (actions < 0) {
        return -1; /* 显式降级：不盲渲染 */
    }

    scr = lv_screen_active();
    if (scr == NULL) {
        return -1; /* LVGL 未初始化 -> 降级不崩 */
    }
    w = lv_display_get_horizontal_resolution(NULL);
    h = lv_display_get_vertical_resolution(NULL);
    if (w <= 0 || h <= 0) {
        return -1;
    }

    /* 先清活动屏：重渲染任一弹窗都不得与上一帧叠加（排队逐个弹「不叠屏」的渲染级不变式） */
    lv_obj_clean(scr);
    lv_obj_set_style_pad_all(scr, 0, 0);

    /* 1) 白底 + 整屏压：d50 网点聚焦（V3 §06「整屏压 50% 网点强制聚焦」; F4: d50
     *    网点合成, 替代常规位深半透明填充）。弹窗卡片随后覆盖为唯一亮区。 */
    krm_fill_rect(scr, 0, 0, w, h, KRM_COLOR_BG, LV_OPA_COVER);
    {
        const lv_image_dsc_t *dots = krm_d50_overlay_dsc();
        lv_obj_t *dim = lv_obj_create(scr);
        lv_obj_remove_style_all(dim);
        lv_obj_set_size(dim, w, h);
        lv_obj_set_pos(dim, 0, 0);
        lv_obj_set_style_bg_opa(dim, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(dim, lv_color_hex(KRM_COLOR_BG), 0);
        if (dots != NULL) {
            lv_obj_set_style_bg_image_src(dim, dots, 0);
            lv_obj_set_style_bg_image_tiled(dim, true, 0);
        }
        lv_obj_clear_flag(dim, LV_OBJ_FLAG_SCROLLABLE);
    }

    /* 2) 弹窗卡片 */
    card_x = w * KRM_CARD_X_PCT / 100;
    card_y = h * KRM_CARD_Y_PCT / 100;
    card_w = w * KRM_CARD_W_PCT / 100;
    card_h = h * KRM_CARD_H_PCT / 100;
    card = krm_fill_rect(scr, card_x, card_y, card_w, card_h, KRM_COLOR_BG, LV_OPA_COVER);
    lv_obj_set_style_radius(card, 6, 0);
    lv_obj_set_style_border_width(card, 3, 0);
    lv_obj_set_style_border_color(card, lv_color_hex(KRM_COLOR_INK), 0);
    lv_obj_set_style_border_opa(card, LV_OPA_COVER, 0);

    /* 3) 小伴 pose-jump 位（卡片左上，报信姿态） */
    {
        int32_t pose = card_h / 4;

        if (pose < 24) {
            pose = 24;
        }
        krm_pose_jump(card, 6, 6, pose);

        /* 换皮消费接线 (包 9b HWV-A6)：真机把 pose_jump_96 KAP1 位图叠在矢量占位同位
         * 同尺寸 (左上角、边长 pose)。几何只吃 payload 无关的 card_h -> 确定性；标题
         * 在其右 (step 4 x=card_h/3+8)、时间在其下 (step 5 y=card_h*5/8)，均不被遮挡 ->
         * RMD-A5 diff（随载荷变）与 accum==0（重渲染恒同）两齿不变。未 mount / 加载
         * 失败 -> helper 返 NULL，保留矢量占位（best-effort 降级，不崩）。 */
        {
            const lv_image_dsc_t *pd = krm_pose_jump_bitmap_dsc();
            if (pd != NULL) {
                lv_obj_t *pi = lv_image_create(card);
                lv_image_set_src(pi, pd);
                lv_obj_set_pos(pi, 6, 6);
                int32_t sc = (int32_t)((pose * 256) / pd->header.w);
                lv_obj_set_style_transform_scale_x(pi, sc, 0);
                lv_obj_set_style_transform_scale_y(pi, sc, 0);
                lv_obj_set_style_transform_pivot_x(pi, 0, 0);
                lv_obj_set_style_transform_pivot_y(pi, 0, 0);
            }
        }
    }

    /* 4) 大字事项（title，含要带的东西）—— 200% 缩放 = 「大字」；LVGL 9.3
     *    transform scale 经中间层合成，host(32bit) 与设备(16bit) 同源确定性。 */
    big = krm_text(card, a->title, card_h / 3 + 8, 6, card_w - card_h / 3 - 12,
                   card_h / 3, KRM_COLOR_INK);
    lv_obj_set_style_transform_scale_x(big, KRM_BIG_SCALE, 0);
    lv_obj_set_style_transform_scale_y(big, KRM_BIG_SCALE, 0);
    lv_obj_set_style_transform_pivot_x(big, 0, 0);
    lv_obj_set_style_transform_pivot_y(big, 0, 0);

    /* 5) 时间行 = fire_at 的 HH:MM */
    krm_hhmm_of(a->fire_at, hhmm, sizeof(hhmm));
    krm_text(card, hhmm, 8, card_h * 5 / 8, card_w - 16, 18, KRM_COLOR_INK);

    /* 6) 语音缺省时的「仅弹窗」形态标注 + 按键提示（KEY 关闭） */
    if ((actions & KRM_ACT_SPEAK) != 0) {
        krm_text(card, "KEY OK", 8, card_h - 22, card_w - 16, 16, KRM_COLOR_INK);
    } else {
        /* 合成缺省/失败 -> 仅弹窗 + 提示音（V20 不阻塞投递）；文案缺省本身即降级标记 */
        krm_text(card, "KEY OK -", 8, card_h - 22, card_w - 16, 16, KRM_COLOR_INK);
    }

    /* 三件套的提示音一半：真起弹窗即出音（弱缝 -> 设备侧强定义 OGG_POPUP） */
    if ((actions & KRM_ACT_TONE) != 0) {
        kidpal_reminder_play_popup_tone();
    }
    return 1;
}
