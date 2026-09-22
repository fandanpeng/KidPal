/*
 * home_upcoming.c — KidPal 首页「接下来」注入接线层实现（A5 纯逻辑面,
 * TaskPackage 260921-m3-k2-p2-week-grid-page, 契约 §1.3⑥ home 可选 data.upcoming
 * / S-F1.1 AC1–AC2 消费接线半）。
 *
 * 语义权威 = 冻结验收 firmware/main/test/schedule_grid.acceptance.c 文件头 A5 段；
 * 强定义在 host runner 里覆盖验收文件内的 weak 哨兵 (-2) 后被真测（同
 * view_router.c / schedule_page.c 先例）。
 *
 * 纯逻辑、零芯片/零 LVGL 依赖 -> host_app linux target 与设备树两侧同源可编。
 * 自足式最小 JSON 扫描（无外部 JSON 库, schedule_page.c 同款纪律）；只消费
 * home_p1 冻结 seam（kidpal_home_p1_schedule_inject），home_p1.{c,h} 零修改。
 *
 * 降级语义：任何畸形/越界/seam 拒收 -> -1 且 seam 状态零变化（绝不半写、绝不盲
 * 渲染, V27）；缺/空 data -> 0 且 seam 状态零变化（旧行为完全不变, 向后兼容）。
 */

#include "home_upcoming.h"

#include "home_p1.h"
#include <string.h>

/* -------------------------------------------------------------------------- *
 * 自足式最小 JSON 扫描（与 schedule_page.c 同款纪律, 本模块自足独立成编）。
 * ------------------------------------------------------------------------ */

typedef struct {
    const char *s;
    size_t pos;
} kup_json_cursor;

static void kup_ws(kup_json_cursor *c)
{
    while (c->s[c->pos] == ' ' || c->s[c->pos] == '\t' ||
           c->s[c->pos] == '\n' || c->s[c->pos] == '\r') {
        c->pos++;
    }
}

static char kup_peek(const kup_json_cursor *c)
{
    return c->s[c->pos];
}

static int kup_hex4(kup_json_cursor *c, unsigned *out)
{
    unsigned v = 0;

    for (int i = 0; i < 4; i++) {
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

static void kup_put(char byte, char *out, size_t cap, size_t *used)
{
    if (out != NULL && *used + 1 < cap) {
        out[(*used)++] = byte;
    }
}

/* 解析 JSON 字符串到 out（cap 含 NUL 位）。支持标准转义 + \uXXXX。成功 1 / 畸形 0。 */
static int kup_string(kup_json_cursor *c, char *out, size_t cap)
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
            c->pos++;
            char e = c->s[c->pos];
            c->pos++;

            if (e == 'u') {
                unsigned cp;

                if (!kup_hex4(c, &cp)) {
                    return 0;
                }
                if (cp >= 0xD800u && cp <= 0xDBFFu) {
                    if (c->s[c->pos] == '\\' && c->s[c->pos + 1] == 'u') {
                        unsigned lo;

                        c->pos += 2;
                        if (!kup_hex4(c, &lo)) {
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
                {
                    char tmp[4];
                    int n;

                    n = (cp < 0x80u) ? 1
                        : (cp < 0x800u) ? 2
                        : (cp < 0x10000u) ? 3 : 4;
                    if (n == 1) {
                        tmp[0] = (char)cp;
                    } else if (n == 2) {
                        tmp[0] = (char)(0xC0u | (cp >> 6));
                        tmp[1] = (char)(0x80u | (cp & 0x3Fu));
                    } else if (n == 3) {
                        tmp[0] = (char)(0xE0u | (cp >> 12));
                        tmp[1] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
                        tmp[2] = (char)(0x80u | (cp & 0x3Fu));
                    } else {
                        tmp[0] = (char)(0xF0u | (cp >> 18));
                        tmp[1] = (char)(0x80u | ((cp >> 12) & 0x3Fu));
                        tmp[2] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
                        tmp[3] = (char)(0x80u | (cp & 0x3Fu));
                    }
                    for (int i = 0; i < n; i++) {
                        kup_put(tmp[i], out, cap, &used);
                    }
                }
                continue;
            }

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
            kup_put(plain, out, cap, &used);
            continue;
        }
        /* 普通 UTF-8 字节原样通过（wire json.dumps ensure_ascii=False） */
        kup_put((char)ch, out, cap, &used);
        c->pos++;
    }
    if (out != NULL) {
        out[used] = '\0';
    }
    return 1;
}

/* 跳过任意一个 JSON 值，兼做结构合法性校验。成功 1 / 畸形 0。 */
static int kup_skip_value(kup_json_cursor *c)
{
    kup_ws(c);
    char ch = kup_peek(c);

    if (ch == '"') {
        return kup_string(c, NULL, 0);
    }
    if (ch == '{') {
        c->pos++;
        kup_ws(c);
        if (kup_peek(c) == '}') {
            c->pos++;
            return 1;
        }
        for (;;) {
            kup_ws(c);
            if (!kup_string(c, NULL, 0)) {
                return 0;
            }
            kup_ws(c);
            if (kup_peek(c) != ':') {
                return 0;
            }
            c->pos++;
            if (!kup_skip_value(c)) {
                return 0;
            }
            kup_ws(c);
            char d = kup_peek(c);
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
        kup_ws(c);
        if (kup_peek(c) == ']') {
            c->pos++;
            return 1;
        }
        for (;;) {
            if (!kup_skip_value(c)) {
                return 0;
            }
            kup_ws(c);
            char d = kup_peek(c);
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

/* -------------------------------------------------------------------------- *
 * 校验辅助（与 home_p1 冻结 seam p1_parse_time / p1_day_key_ok 同口径——本层先验
 * 之外 seam 仍全量校验, 拒绝即整包拒）
 * ------------------------------------------------------------------------ */

/* "H:MM"(4) | "HH:MM"(5) 严格形状 + 0..23/0..59 值域。任何非 HH:MM 的 time
 * （含「明早」字面）在此拒收（K1 裁决口径：自然语言标签不入冻结 seam）。 */
static int kup_time_ok(const char *t)
{
    int hh = 0;
    int mm = 0;
    size_t n = strlen(t);

    if (n == 4) {
        if (t[1] != ':') {
            return 0;
        }
        if (!(t[0] >= '0' && t[0] <= '9') || !(t[2] >= '0' && t[2] <= '9') ||
            !(t[3] >= '0' && t[3] <= '9')) {
            return 0;
        }
        hh = t[0] - '0';
        mm = (t[2] - '0') * 10 + (t[3] - '0');
    } else if (n == 5) {
        if (t[2] != ':') {
            return 0;
        }
        if (!(t[0] >= '0' && t[0] <= '9') || !(t[1] >= '0' && t[1] <= '9') ||
            !(t[3] >= '0' && t[3] <= '9') || !(t[4] >= '0' && t[4] <= '9')) {
            return 0;
        }
        hh = (t[0] - '0') * 10 + (t[1] - '0');
        mm = (t[3] - '0') * 10 + (t[4] - '0');
    } else {
        return 0;
    }
    return hh <= 23 && mm <= 59;
}

/* {time,name} 项解析（time 恒 "HH:MM" 形状, HP1_TIME_LEN=6 内放; name 非空,
 * safe-truncate 到 HP1_NAME_LEN）。成功 0 / 畸形 -1。 */
static int kup_parse_entry(kup_json_cursor *c, hp1_item_t *it)
{
    int has_time = 0;
    int has_name = 0;
    char tbuf[32];
    char nbuf[HP1_NAME_LEN];

    kup_ws(c);
    if (kup_peek(c) != '{') {
        return -1; /* 项非对象 -> -1 */
    }
    c->pos++;
    kup_ws(c);
    if (kup_peek(c) == '}') {
        return -1; /* 空 item：缺 time/name -> -1 */
    }
    for (;;) {
        char key[16];

        kup_ws(c);
        if (!kup_string(c, key, sizeof(key))) {
            return -1;
        }
        kup_ws(c);
        if (kup_peek(c) != ':') {
            return -1;
        }
        c->pos++;
        kup_ws(c);

        if (strcmp(key, "time") == 0) {
            if (kup_peek(c) != '"') {
                return -1; /* 缺/非 string time -> -1 */
            }
            if (!kup_string(c, tbuf, sizeof(tbuf))) {
                return -1;
            }
            has_time = 1;
        } else if (strcmp(key, "name") == 0) {
            if (kup_peek(c) != '"') {
                return -1;
            }
            if (!kup_string(c, nbuf, sizeof(nbuf))) {
                return -1;
            }
            has_name = 1;
        } else {
            if (!kup_skip_value(c)) {
                return -1;
            }
        }

        kup_ws(c);
        char d = kup_peek(c);
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
    if (!has_time || !has_name) {
        return -1; /* time/name 必带 */
    }
    if (!kup_time_ok(tbuf)) {
        return -1; /* time 非 H:MM/HH:MM（含「明早」字面）-> -1, 绝不盲渲染 */
    }
    if (nbuf[0] == '\0') {
        return -1; /* name 非空 */
    }
    /* safe-truncate 拷贝（tbuf 已验形状 <=5+1 恒放得下; nbuf 截到 HP1_NAME_LEN） */
    {
        size_t i;

        for (i = 0; tbuf[i] != '\0' && i + 1 < sizeof(it->time); i++) {
            it->time[i] = tbuf[i];
        }
        it->time[i] = '\0';
        for (i = 0; nbuf[i] != '\0' && i + 1 < sizeof(it->name); i++) {
            it->name[i] = nbuf[i];
        }
        it->name[i] = '\0';
    }
    return 0;
}

/* preview 值解析：null / 缺省 = 无预告（返回 1, *has 置 0）；对象 = 同项校验
 * （返回 0）；其它类型 = 畸形 -1。 */
static int kup_parse_preview(kup_json_cursor *c, hp1_item_t *pv, int *has)
{
    kup_ws(c);
    char ch = kup_peek(c);

    if (ch == 'n') {
        if (!kup_skip_value(c)) {
            return -1;
        }
        *has = 0;
        return 1; /* preview:null 容忍（明日无项 = 无预告, 不降级） */
    }
    if (ch != '{') {
        return -1; /* 非 object/null preview -> -1 */
    }
    *has = 1;
    return kup_parse_entry(c, pv);
}

/* -------------------------------------------------------------------------- *
 * kidpal_home_upcoming_apply — A5 接线层 device face
 * ------------------------------------------------------------------------ */

#define KUP_KEY_BUF    16
#define KUP_DAYKEY_BUF 32 /* 读 31 字节再验 <=15, 防截断假通过 */

int kidpal_home_upcoming_apply(const char *data_json)
{
    kup_json_cursor c = { data_json, 0 };
    hp1_item_t items[HP1_ITEM_MAX];
    hp1_item_t pv;
    char day_key[KUP_DAYKEY_BUF];
    int count = 0;
    int has_items = 0;
    int has_key = 0;
    int has_pv = 0;
    int has_upcoming = 0;

    /* 缺/空 data -> 0 且 seam 状态零变化（旧 {view:home} 行为完全不变, 向后兼容） */
    if (data_json == NULL || data_json[0] == '\0') {
        return 0;
    }

    kup_ws(&c);
    if (kup_peek(&c) != '{') {
        return -1; /* 非 object data -> 畸形 */
    }
    c.pos++;
    kup_ws(&c);

    if (kup_peek(&c) != '}') {
        for (;;) {
            char key[KUP_KEY_BUF];

            kup_ws(&c);
            if (!kup_string(&c, key, sizeof(key))) {
                return -1;
            }
            kup_ws(&c);
            if (kup_peek(&c) != ':') {
                return -1;
            }
            c.pos++;
            kup_ws(&c);

            if (strcmp(key, "upcoming") == 0) {
                if (kup_peek(&c) == 'n') {
                    /* upcoming:null 容忍 = 无 upcoming（additive, 行为同缺键） */
                    if (!kup_skip_value(&c)) {
                        return -1;
                    }
                } else if (kup_peek(&c) == '{') {
                    /* upcoming 对象在：就地解析 day_key / items / preview
                     * （cursor 在 '{' 前, 消费到配对 '}'）。 */
                    c.pos++;
                    kup_ws(&c);
                    if (kup_peek(&c) == '}') {
                        return -1; /* upcoming 空对象 = 缺 day_key/items -> -1 */
                    }
                    for (;;) {
                        char ukey[KUP_KEY_BUF];

                        kup_ws(&c);
                        if (!kup_string(&c, ukey, sizeof(ukey))) {
                            return -1;
                        }
                        kup_ws(&c);
                        if (kup_peek(&c) != ':') {
                            return -1;
                        }
                        c.pos++;
                        kup_ws(&c);

                        if (strcmp(ukey, "day_key") == 0) {
                            if (kup_peek(&c) != '"') {
                                return -1;
                            }
                            if (!kup_string(&c, day_key, sizeof(day_key))) {
                                return -1;
                            }
                            has_key = 1;
                        } else if (strcmp(ukey, "items") == 0) {
                            if (kup_peek(&c) != '[') {
                                return -1; /* items 非数组 -> -1 */
                            }
                            c.pos++;
                            kup_ws(&c);
                            if (kup_peek(&c) == ']') {
                                c.pos++; /* 空数组 = 今日无项, 合法 */
                            } else {
                                for (;;) {
                                    if (count >= HP1_ITEM_MAX) {
                                        return -1; /* count > HP1_ITEM_MAX ->
                                                    * -1（seam 容量越界整包拒收） */
                                    }
                                    if (kup_parse_entry(&c, &items[count]) != 0) {
                                        return -1;
                                    }
                                    count++;
                                    kup_ws(&c);
                                    char d = kup_peek(&c);
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
                            has_items = 1;
                        } else if (strcmp(ukey, "preview") == 0) {
                            if (kup_parse_preview(&c, &pv, &has_pv) < 0) {
                                return -1;
                            }
                        } else {
                            if (!kup_skip_value(&c)) {
                                return -1;
                            }
                        }

                        kup_ws(&c);
                        char d = kup_peek(&c);
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
                    has_upcoming = 1;
                } else {
                    return -1; /* 非 object/null upcoming -> 畸形 */
                }
            } else {
                /* 未知顶层扩展键忽略（additive tolerance）; 值畸形 -> -1 */
                if (!kup_skip_value(&c)) {
                    return -1;
                }
            }

            kup_ws(&c);
            char d = kup_peek(&c);
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
    } else {
        c.pos++; /* "{}" */
    }

    kup_ws(&c);
    if (c.s[c.pos] != '\0') {
        return -1; /* 尾部垃圾 = 畸形 JSON */
    }
    if (!has_upcoming) {
        return 0; /* 无 "upcoming" 键 -> 0 且 seam 状态零变化（向后兼容） */
    }
    if (!has_key || !has_items) {
        return -1; /* day_key / items 必带（upcoming 在 = 形状必带字段） */
    }
    {
        size_t kn = strlen(day_key);

        if (kn == 0 || kn > 15) {
            return -1; /* day_key 必须 1..15 字节字符串 */
        }
    }

    /* 全量校验通过后才经冻结 seam 注入（seam 自身亦全量校验, 拒绝即整包拒,
     * 绝不半写）。 */
    return kidpal_home_p1_schedule_inject(day_key, (count > 0) ? items : NULL,
                                          count, has_pv ? &pv : NULL);
}
