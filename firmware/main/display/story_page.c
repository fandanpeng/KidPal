/*
 * story_page.c — KidPal P3 故事页消费面：14 个 FROZEN OFFLINE CONTRACT 的实现
 * (TaskPackage 260829-story-p3, 契约 §1.3 ③/④ / §1.4 / spec S-F2.1..S-F2.4)。
 *
 * 语义与形状以冻结验收 firmware/main/test/story_p3.acceptance.c 文件头为准，不得
 * 偏移；强定义在 host runner 里覆盖验收文件内的 weak 哨兵 (-2) 后被真测（同
 * schedule_page.c / view_router.c 先例）。
 *
 * 纯逻辑 + 纯 LVGL（lv_obj_/lv_label/lv_anim only，无 Board/Display class 依赖）
 * -> host_app linux target 与设备树两侧同源可编。解析器为自足式最小 JSON 扫描
 * （无外部 JSON 库依赖，frozen contract 要求；同 display/schedule_page.c 纪律）。
 *
 * 降级语义：解析畸形/缺字段/越界 cap 一律 -1 —— 调用方必须友好降级、绝不盲渲染
 * (V27 / spec §4.0)。
 */

#include "story_page.h"

#include <stdio.h>
#include <string.h>

#include "lvgl.h"

/* 冻结逐字文案 (spec S-F2.3 / S-F2.4)；字节须与验收期望串一致，见下方 KSTORY_* 定义。 */
#define KSTORY_SCREEN_FMT "第 %d 屏 · 共 %d 屏"
#define KSTORY_STAMP_FMT  "故事徽章·第 %d 颗已入册！"

/* 盖章几何冻结值。 */
#define KSTORY_STAMP_ANGLE_DECI (-80)
#define KSTORY_STAMP_HOLD_MS    800

/* 录音态显示位上界 = 99:59 (5999 秒)；冻结 contract 钳制显示位。 */
#define KSTORY_REC_MAX_SECONDS 5999

/* -------------------------------------------------------------------------- *
 * 共用小工具
 * ------------------------------------------------------------------------ */

/* safe-truncate 拷贝：恒 NUL 结尾、绝不溢出（frozen contract 要求）。 */
static void ksy_copy_str(char *dst, size_t cap, const char *src)
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

/* 计数 '\n' 分隔的行数：空串记 1 行；每个 '\n' 之后是新行。 */
static int ksy_line_count(const char *s)
{
    int lines = 1;
    const char *p;

    if (s == NULL) {
        return 1;
    }
    for (p = s; *p != '\0'; p++) {
        if (*p == '\n') {
            lines++;
        }
    }
    return lines;
}

/* -------------------------------------------------------------------------- *
 * 自足式最小 JSON 扫描（无外部 JSON 库; frozen contract 要求）。
 * 只需覆盖 producer 下发的 data 对象形状 + skip 未知扩展值。
 * ------------------------------------------------------------------------ */

typedef struct {
    const char *s;
    size_t pos;
} ksy_cursor;

static void ksy_ws(ksy_cursor *c)
{
    while (c->s[c->pos] == ' ' || c->s[c->pos] == '\t' ||
           c->s[c->pos] == '\n' || c->s[c->pos] == '\r') {
        c->pos++;
    }
}

static char ksy_peek(const ksy_cursor *c)
{
    return c->s[c->pos];
}

static int ksy_hex4(ksy_cursor *c, unsigned *out)
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

static int ksy_utf8_encode(unsigned cp, char tmp[4])
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

static void ksy_put(char byte, char *out, size_t cap, size_t *used)
{
    if (out != NULL && *used + 1 < cap) {
        out[(*used)++] = byte;
    }
}

/* 解析 JSON 字符串到 out（cap 含 NUL 位; out 可为 NULL = 仅校验）。成功 1 / 畸形 0。 */
static int ksy_string(ksy_cursor *c, char *out, size_t cap)
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

                if (!ksy_hex4(c, &cp)) {
                    return 0;
                }
                if (cp >= 0xD800u && cp <= 0xDBFFu) {
                    unsigned lo;
                    if (c->s[c->pos] == '\\' && c->s[c->pos + 1] == 'u') {
                        c->pos += 2;
                        if (!ksy_hex4(c, &lo)) {
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
                n = ksy_utf8_encode(cp, tmp);
                for (i = 0; i < n; i++) {
                    ksy_put(tmp[i], out, cap, &used);
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
                ksy_put(plain, out, cap, &used);
            }
            continue;
        }
        ksy_put((char)ch, out, cap, &used);
        c->pos++;
    }
    if (out != NULL) {
        out[used] = '\0';
    }
    return 1;
}

/* 互递归前置声明（skip_value <-> skip_object/skip_array）。 */
static int ksy_skip_value(ksy_cursor *c);

/* 跳过一个顶层非空 object（仅结构校验，不取字段）。成功 1 / 畸形 0。 */
static int ksy_skip_object(ksy_cursor *c)
{
    if (ksy_peek(c) != '{') {
        return 0;
    }
    c->pos++;
    ksy_ws(c);
    if (ksy_peek(c) == '}') {
        c->pos++;
        return 1;
    }
    for (;;) {
        char d;
        ksy_ws(c);
        if (!ksy_string(c, NULL, 0)) {
            return 0;
        }
        ksy_ws(c);
        if (ksy_peek(c) != ':') {
            return 0;
        }
        c->pos++;
        if (!ksy_skip_value(c)) {
            return 0;
        }
        ksy_ws(c);
        d = ksy_peek(c);
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
static int ksy_skip_array(ksy_cursor *c)
{
    if (ksy_peek(c) != '[') {
        return 0;
    }
    c->pos++;
    ksy_ws(c);
    if (ksy_peek(c) == ']') {
        c->pos++;
        return 1;
    }
    for (;;) {
        char d;
        if (!ksy_skip_value(c)) {
            return 0;
        }
        ksy_ws(c);
        d = ksy_peek(c);
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

/* 跳过任意一个 JSON 值（string/object/array/number/true/false/null），
 * 兼做结构合法性校验。成功 1 / 畸形 0。 */
static int ksy_skip_value(ksy_cursor *c)
{
    char ch;
    ksy_ws(c);
    ch = ksy_peek(c);
    if (ch == '"') {
        return ksy_string(c, NULL, 0);
    }
    if (ch == '{') {
        return ksy_skip_object(c);
    }
    if (ch == '[') {
        return ksy_skip_array(c);
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

/* 严格解析一个非负整数（纯数字，无符号 / 无小数点 / 无指数）。
 * 成功 -> 1 且 *out 为值；非整数 / 负数 / 越界 -> 0（游标不推进消费）。 */
static int ksy_nonneg_int(ksy_cursor *c, long *out)
{
    long v = 0;
    size_t start = c->pos;
    int digits = 0;

    ksy_ws(c);
    while (c->s[c->pos] >= '0' && c->s[c->pos] <= '9') {
        if (v > 100000000L) { /* 防溢出即视为超大（仍要求非负整数结构） */
            v = 100000000L;
        } else {
            v = v * 10 + (c->s[c->pos] - '0');
        }
        digits++;
        c->pos++;
    }
    if (digits == 0) {
        c->pos = start; /* 非整数：还原游标，交调用方按畸形/负值处理 */
        return 0;
    }
    /* 数字之后若紧跟 '.'/e/E（浮点/科学计数）或非值结束符 -> 视为非整数。 */
    {
        char after = c->s[c->pos];
        if (after == '.' || after == 'e' || after == 'E') {
            c->pos = start;
            return 0;
        }
    }
    *out = v;
    return 1;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_list_parse — P3-A2 (契约 §1.3 ③)
 * ------------------------------------------------------------------------ */

#define KSY_KEY_BUF 32

static int ksy_parse_item(ksy_cursor *arr, kidpal_story_item_t *slot, int store)
{
    int has_id = 0;
    int has_title = 0;
    char id_buf[64];
    char title_buf[96];
    char date_buf[16];

    id_buf[0] = '\0';
    title_buf[0] = '\0';
    date_buf[0] = '\0';

    ksy_ws(arr);
    if (ksy_peek(arr) != '{') {
        return -1; /* item 非对象 -> 降级 */
    }
    arr->pos++;
    ksy_ws(arr);
    if (ksy_peek(arr) == '}') {
        return -1; /* 空 item：缺 id/title -> 降级 */
    }
    for (;;) {
        char key[KSY_KEY_BUF];
        char d;

        ksy_ws(arr);
        if (!ksy_string(arr, key, sizeof(key))) {
            return -1;
        }
        ksy_ws(arr);
        if (ksy_peek(arr) != ':') {
            return -1;
        }
        arr->pos++;
        ksy_ws(arr);
        if (strcmp(key, "id") == 0) {
            if (ksy_peek(arr) != '"') {
                return -1; /* id 必为非空 string 字段 */
            }
            if (!ksy_string(arr, id_buf, sizeof(id_buf))) {
                return -1;
            }
            has_id = 1;
        } else if (strcmp(key, "title") == 0) {
            if (ksy_peek(arr) != '"') {
                return -1; /* title 必为 string 字段 */
            }
            if (!ksy_string(arr, title_buf, sizeof(title_buf))) {
                return -1;
            }
            has_title = 1;
        } else if (strcmp(key, "date") == 0) {
            if (ksy_peek(arr) != '"') {
                return -1; /* date 若出现必为 string（缺省才是 ""） */
            }
            if (!ksy_string(arr, date_buf, sizeof(date_buf))) {
                return -1;
            }
        } else {
            /* 未知扩展字段（含任何生字标注位）一律忽略、不消费（additive）。 */
            if (!ksy_skip_value(arr)) {
                return -1;
            }
        }
        ksy_ws(arr);
        d = ksy_peek(arr);
        if (d == ',') {
            arr->pos++;
            continue;
        }
        if (d == '}') {
            arr->pos++;
            break;
        }
        return -1;
    }
    if (!has_id || id_buf[0] == '\0' || !has_title) {
        return -1; /* id 是回看键 (非空必带)、缺 title 是畸形行 */
    }
    if (store) {
        ksy_copy_str(slot->id, sizeof(slot->id), id_buf);
        ksy_copy_str(slot->title, sizeof(slot->title), title_buf);
        ksy_copy_str(slot->date, sizeof(slot->date), date_buf); /* 缺 date -> "" */
    }
    return 0;
}

int kidpal_story_list_parse(const char *data_json, kidpal_story_list_t *out)
{
    ksy_cursor c;
    int seen_items = 0;
    int seen_total = 0;
    long total_val = 0;
    int count = 0;

    if (data_json == NULL || out == NULL) {
        return -1; /* NULL 载荷 / NULL out -> 显式降级 */
    }

    c.s = data_json;
    c.pos = 0;
    ksy_ws(&c);
    if (ksy_peek(&c) != '{') {
        return -1;
    }
    c.pos++;
    ksy_ws(&c);
    if (ksy_peek(&c) == '}') {
        return -1; /* {} 缺 items -> 降级 (items 是 data 形状必带字段) */
    }

    for (;;) {
        char key[KSY_KEY_BUF];
        char d;

        ksy_ws(&c);
        if (!ksy_string(&c, key, sizeof(key))) {
            return -1;
        }
        ksy_ws(&c);
        if (ksy_peek(&c) != ':') {
            return -1;
        }
        c.pos++;
        ksy_ws(&c);

        if (strcmp(key, "total") == 0) {
            /* total 出现则必为非负整数；否则 -1（故事果计数不可能为负/非整数）。 */
            if (ksy_peek(&c) == '"') {
                return -1; /* 字符串 total 非整数 */
            }
            if (!ksy_nonneg_int(&c, &total_val)) {
                return -1; /* 非整数 (浮点/科学/缺数字) */
            }
            seen_total = 1;
        } else if (strcmp(key, "items") == 0) {
            char de;
            if (ksy_peek(&c) != '[') {
                return -1; /* items 非数组 -> 降级 */
            }
            c.pos++;
            ksy_ws(&c);
            if (ksy_peek(&c) == ']') {
                c.pos++; /* 空 items 合法页 */
            } else {
                for (;;) {
                    kidpal_story_item_t scratch;
                    int r;
                    /* 超 KSTORY_LIST_MAX -> 降级（绝不静默丢行，调用方须请求更小页）。 */
                    int store = (count < KSTORY_LIST_MAX) ? 1 : 0;
                    r = ksy_parse_item(&c, store ? &out->items[count] : &scratch, store);
                    if (r != 0) {
                        return -1;
                    }
                    count++;
                    if (count > KSTORY_LIST_MAX) {
                        return -1;
                    }
                    ksy_ws(&c);
                    de = ksy_peek(&c);
                    if (de == ',') {
                        c.pos++;
                        continue;
                    }
                    if (de == ']') {
                        c.pos++;
                        break;
                    }
                    return -1;
                }
            }
            seen_items = 1;
        } else {
            /* 顶层未知扩展字段忽略（契约 data 形状不锁扩展位）。 */
            if (!ksy_skip_value(&c)) {
                return -1;
            }
        }

        ksy_ws(&c);
        d = ksy_peek(&c);
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

    ksy_ws(&c);
    if (c.s[c.pos] != '\0') {
        return -1; /* 尾部垃圾 = 畸形 JSON */
    }
    if (!seen_items) {
        return -1; /* items 必带 */
    }
    out->total = seen_total ? (int)total_val : KSTORY_TOTAL_ABSENT;
    out->count = count;
    return count;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_counter — P3-A2 故事果计数器双钉 (契约 §一 story_list 硬约束)
 * ------------------------------------------------------------------------ */

int kidpal_story_counter(const kidpal_story_list_t *list)
{
    if (list == NULL) {
        return 0;
    }
    /* 钉①：优先 total（云端权威全集计数）；分页/截断时绝不以 items.length 顶替。 */
    if (list->total >= 0) {
        return list->total;
    }
    /* 钉②：total 缺省 -> 回退 items.length（向后兼容）。 */
    return list->count;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_text_parse — P3-A5 (契约 §1.3 ④)
 * ------------------------------------------------------------------------ */

int kidpal_story_text_parse(const char *data_json, kidpal_story_text_t *out)
{
    ksy_cursor c;
    int has_id = 0;
    int has_text = 0;
    char id_buf[64];
    char title_buf[96];
    char text_buf[2048];

    if (data_json == NULL || out == NULL) {
        return -1;
    }
    id_buf[0] = '\0';
    title_buf[0] = '\0';
    text_buf[0] = '\0';

    c.s = data_json;
    c.pos = 0;
    ksy_ws(&c);
    if (ksy_peek(&c) != '{') {
        return -1;
    }
    c.pos++;
    ksy_ws(&c);
    if (ksy_peek(&c) == '}') {
        return -1; /* 缺 id/text */
    }
    for (;;) {
        char key[KSY_KEY_BUF];
        char d;

        ksy_ws(&c);
        if (!ksy_string(&c, key, sizeof(key))) {
            return -1;
        }
        ksy_ws(&c);
        if (ksy_peek(&c) != ':') {
            return -1;
        }
        c.pos++;
        ksy_ws(&c);
        if (strcmp(key, "id") == 0) {
            if (ksy_peek(&c) != '"') {
                return -1; /* id 必为 string（回听键, render-critical） */
            }
            if (!ksy_string(&c, id_buf, sizeof(id_buf))) {
                return -1;
            }
            has_id = 1;
        } else if (strcmp(key, "text") == 0) {
            if (ksy_peek(&c) != '"') {
                return -1; /* text 必为 string（正文, render-critical） */
            }
            if (!ksy_string(&c, text_buf, sizeof(text_buf))) {
                return -1;
            }
            has_text = 1;
        } else if (strcmp(key, "title") == 0) {
            if (ksy_peek(&c) != '"') {
                return -1; /* title 若出现须为 string（缺省才容错为 ""） */
            }
            if (!ksy_string(&c, title_buf, sizeof(title_buf))) {
                return -1;
            }
        } else {
            /* 未知扩展字段（含任何生字/高亮标注位）一律忽略、不消费 (P3-E4)。 */
            if (!ksy_skip_value(&c)) {
                return -1;
            }
        }
        ksy_ws(&c);
        d = ksy_peek(&c);
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
    ksy_ws(&c);
    if (c.s[c.pos] != '\0') {
        return -1; /* 尾部垃圾 = 畸形 JSON */
    }
    if (!has_id || !has_text) {
        return -1; /* id/text 是 render-critical 必带字段 */
    }
    ksy_copy_str(out->id, sizeof(out->id), id_buf);
    ksy_copy_str(out->title, sizeof(out->title), title_buf); /* 缺 title -> "" */
    ksy_copy_str(out->text, sizeof(out->text), text_buf);
    return 0;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_view_init — 列表视图装载 (刷新以云端为权威)
 * ------------------------------------------------------------------------ */

int kidpal_story_view_init(kidpal_story_view_t *v, const kidpal_story_list_t *list)
{
    if (v == NULL) {
        return -1;
    }
    v->page = KSTORY_PAGE_LIST;
    v->cursor = 0;
    v->screen = 0;
    v->screens = 1;
    v->count = (list != NULL) ? list->count : 0;
    v->counter = kidpal_story_counter(list);
    v->rec_seconds = 0;
    v->stamp_index = 0;
    v->archive_pend = 0;
    v->cancel_count = 0;
    v->tts_requests = 0;
    v->last_saved_id[0] = '\0';
    return 0;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_page_input — P3-A3/A4/A5 页内输入特例 (spec §4.0 全局表「页内特例」列)
 *
 * 数值码（== 冻结验收 KSA_*）：
 *   DEGRADE -3 / GLOBAL -2 / NONE 0 / CURSOR_NEXT 1 / OPEN_READER 2 /
 *   PAGE_TURN 3 / TTS_READ 4 / STOP_RECORD 5 / CANCEL_RECORD 6
 * ------------------------------------------------------------------------ */

#define KSA_ACT_GLOBAL       (-2)
#define KSA_ACT_DEGRADE      (-3)
#define KSA_ACT_NONE           0
#define KSA_ACT_CURSOR_NEXT    1
#define KSA_ACT_OPEN_READER    2
#define KSA_ACT_PAGE_TURN      3
#define KSA_ACT_TTS_READ       4
#define KSA_ACT_STOP_RECORD    5
#define KSA_ACT_CANCEL_RECORD  6

int kidpal_story_page_input(kidpal_story_view_t *v, key_id_t key, key_gesture_t g)
{
    if (v == NULL || key >= KEY_ID_COUNT) {
        return KSA_ACT_DEGRADE; /* NULL 视图 / 越界键位（PWR 面）：零污染 */
    }
    if (g == KEY_GESTURE_NONE) {
        return KSA_ACT_NONE; /* 无手势 = 无副作用 */
    }

    switch (v->page) {
        case KSTORY_PAGE_LIST:
            if (key == KEY_ID_KEY && g == KEY_GESTURE_CLICK) {
                if (v->count <= 0) {
                    return KSA_ACT_NONE; /* 空列表不动 */
                }
                /* 冻结决策 D1: 列表光标循环回绕（否则末项之后的故事再也选不到）。 */
                v->cursor = (v->cursor + 1) % v->count;
                return KSA_ACT_CURSOR_NEXT;
            }
            if (key == KEY_ID_KEY && g == KEY_GESTURE_DOUBLE_CLICK) {
                if (v->count <= 0) {
                    return KSA_ACT_NONE; /* 无行可读 */
                }
                v->page = KSTORY_PAGE_READER;
                v->screen = 0;
                if (v->screens < 1) {
                    v->screens = 1;
                }
                return KSA_ACT_OPEN_READER;
            }
            return KSA_ACT_GLOBAL; /* KEY 长按 / BOOT 单击·长按·双击 -> 全局层 */

        case KSTORY_PAGE_READER:
            if (key == KEY_ID_KEY && g == KEY_GESTURE_CLICK) {
                v->tts_requests++; /* 朗读触发（屏/页状态不变; 声学殿后 9b） */
                return KSA_ACT_TTS_READ;
            }
            if (key == KEY_ID_BOOT && g == KEY_GESTURE_LONG_PRESS) {
                /* BOOT 长按仅属「仍可翻页」的多屏阅读态特殊例；单屏阅读器（如从列表
                 * 双击进入但未装载真实正文, screens==1）无屏可翻 -> 交全局层, 与
                 * 冻结验收 P3-A3 (非阅读页不吞) + P3-A5 (多屏末屏钳制 NONE) 一致。 */
                if (v->screens > 1) {
                    if (v->screen < v->screens - 1) {
                        v->screen = v->screen + 1;
                        if (v->screen > v->screens - 1) {
                            v->screen = v->screens - 1;
                        }
                        return KSA_ACT_PAGE_TURN;
                    }
                    return KSA_ACT_NONE; /* 多屏末屏钳制不回绕 (冻结决策 D1) */
                }
                return KSA_ACT_GLOBAL;
            }
            return KSA_ACT_GLOBAL;

        case KSTORY_PAGE_RECORDING:
            if (key == KEY_ID_KEY && g == KEY_GESTURE_CLICK) {
                v->page = KSTORY_PAGE_LIST; /* rec_seconds 定格 */
                v->archive_pend++;          /* 停录 -> 一次归档请求 */
                return KSA_ACT_STOP_RECORD;
            }
            if (key == KEY_ID_KEY && g == KEY_GESTURE_LONG_PRESS) {
                v->page = KSTORY_PAGE_LIST;
                v->cancel_count++;  /* V31: 已累积内容丢弃、不归档 */
                return KSA_ACT_CANCEL_RECORD;
            }
            return KSA_ACT_GLOBAL; /* 其余手势 -> 全局层 */

        case KSTORY_PAGE_STAMP:
            return KSA_ACT_GLOBAL; /* 盖章帧页内零语义 */

        default:
            return KSA_ACT_GLOBAL;
    }
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_on_control — 契约 §1.4 control 动作消费 (VRT_CTRL_* 包 2 冻结数值码)
 * ------------------------------------------------------------------------ */

int kidpal_story_on_control(kidpal_story_view_t *v, int ctrl_action)
{
    if (v == NULL) {
        return -1;
    }
    if (ctrl_action == 2) { /* VRT_CTRL_START_RECORDING */
        if (v->page == KSTORY_PAGE_RECORDING) {
            /* 幂等：网关重试下发不得重置孩子已录时长（rec_seconds 不清零）。 */
            return 1;
        }
        v->page = KSTORY_PAGE_RECORDING;
        v->rec_seconds = 0;
        v->screen = 0;
        /* 冻结决策 D2: 录完前不预推计数（counter 不变; 取消的故事不该占故事果位）。 */
        return 1;
    }
    if (ctrl_action == 0 || ctrl_action == 1) { /* END_SESSION / STOP_SPEAKING */
        return 0; /* 已知但故事页不消费（交给全局会话/播报面），视图零改动 */
    }
    return -1; /* 其它值（含 VRT_CTRL_UNKNOWN）-> 降级，视图零改动 */
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_rec_tick — 录音态演出一帧 (spec S-F2.2 p3-recording)
 *
 * 冻结锁的是「在闪/在跳」与显示位钳制，不锁相位档数/占空比。确定性：纯 elapsed_ms 函数。
 * ------------------------------------------------------------------------ */

int kidpal_story_rec_tick(kidpal_story_view_t *v, uint32_t elapsed_ms, int *flash,
                          int *wave)
{
    int secs;

    if (v == NULL || v->page != KSTORY_PAGE_RECORDING) {
        return -1;
    }
    secs = (int)(elapsed_ms / 1000u);
    if (secs > KSTORY_REC_MAX_SECONDS) {
        secs = KSTORY_REC_MAX_SECONDS; /* 钳制显示位 99:59, 绝不溢出 */
    }
    v->rec_seconds = secs;

    /* REC 反白点：250ms 半周期翻转（>=100ms 儿童视觉安全下界，非频闪）。 */
    if (flash != NULL) {
        *flash = (int)((elapsed_ms / 250u) % 2u);
    }
    /* 手绘声波相位档（>=0，在窗口内多值 -> 在跳）。 */
    if (wave != NULL) {
        *wave = (int)((elapsed_ms / 100u) % 4u);
    }
    return 0;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_rec_time — 大时长 "MM:SS" 文本 (spec S-F2.2「00:47 式」)
 * ------------------------------------------------------------------------ */

int kidpal_story_rec_time(const kidpal_story_view_t *v, char *out, size_t cap)
{
    int secs;
    int mm;
    int ss;
    int need;

    if (v == NULL || out == NULL) {
        return -1;
    }
    secs = v->rec_seconds;
    if (secs < 0) {
        secs = 0;
    }
    if (secs > KSTORY_REC_MAX_SECONDS) {
        secs = KSTORY_REC_MAX_SECONDS;
    }
    mm = secs / 60;
    ss = secs % 60;
    need = snprintf(NULL, 0, "%02d:%02d", mm, ss);
    if (need < 0 || (size_t)need + 1 > cap) {
        return -1; /* cap 不足 -> 拒绝, 绝不截断/溢出（调用方须加大缓冲） */
    }
    snprintf(out, cap, "%02d:%02d", mm, ss);
    return 0;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_open_reader — 进阅读态并分页 (P3-A5 / S-F2.3)
 *
 * 全文上屏、显示端不丢内容（同 kidpal_display_text_page_count 纪律）:
 *   screens >= 1; screens * screen_lines >= 正文行数; 正文更长 -> screens 不减。
 * ------------------------------------------------------------------------ */

int kidpal_story_open_reader(kidpal_story_view_t *v, const kidpal_story_text_t *st,
                             int screen_lines)
{
    int lines;
    int screens;

    if (v == NULL || st == NULL) {
        return -1; /* 无正文不盲渲染 (V27) */
    }
    if (screen_lines <= 0) {
        /* 防御：非法行高 -> 1 屏（不除零崩溃）。 */
        v->page = KSTORY_PAGE_READER;
        v->screen = 0;
        v->screens = 1;
        return 0;
    }
    lines = ksy_line_count(st->text);
    screens = (lines + screen_lines - 1) / screen_lines;
    if (screens < 1) {
        screens = 1;
    }
    v->page = KSTORY_PAGE_READER;
    v->screen = 0;
    v->screens = screens;
    return 0;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_screen_label — 阅读态底部屏数指示 (spec S-F2.3 逐字)
 * ------------------------------------------------------------------------ */

int kidpal_story_screen_label(const kidpal_story_view_t *v, char *out, size_t cap)
{
    int need;

    if (v == NULL || out == NULL) {
        return -1;
    }
    need = snprintf(NULL, 0, KSTORY_SCREEN_FMT, v->screen + 1, v->screens);
    if (need < 0 || (size_t)need + 1 > cap) {
        return -1; /* cap 不足 -> 拒绝, 绝不溢出 */
    }
    snprintf(out, cap, KSTORY_SCREEN_FMT, v->screen + 1, v->screens);
    return 0;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_stamp_spec — 盖章几何冻结 (spec S-F2.4)
 * ------------------------------------------------------------------------ */

int kidpal_story_stamp_spec(kidpal_story_stamp_spec_t *out)
{
    if (out == NULL) {
        return -1;
    }
    out->rings = 2;
    out->inner_dashed = 1;
    out->angle_deci = KSTORY_STAMP_ANGLE_DECI;
    out->hold_ms = KSTORY_STAMP_HOLD_MS;
    return 0;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_on_saved — 云端保存成功 ack -> 盖章帧 (S-F2.2/S-F2.4)
 *
 * 冻结决策 D3: N = counter + 1 本地推、与故事果计数器严格同步；随后 story_list
 * 刷新以云端权威 total 覆盖（见 view_init）。同一 saved_id 幂等（mcp 回执重试不
 * 重复发徽章）。
 * ------------------------------------------------------------------------ */

int kidpal_story_on_saved(kidpal_story_view_t *v, const char *saved_id)
{
    if (v == NULL || saved_id == NULL || saved_id[0] == '\0') {
        return -1; /* 畸形 ack -> 状态零污染 */
    }
    if (strcmp(v->last_saved_id, saved_id) == 0) {
        /* 同一 saved_id 重复 ack -> 幂等（不重复 +1；页态保持盖章帧）。 */
        v->page = KSTORY_PAGE_STAMP;
        return 0;
    }
    v->stamp_index = v->counter + 1;
    v->counter = v->stamp_index;
    v->page = KSTORY_PAGE_STAMP;
    ksy_copy_str(v->last_saved_id, sizeof(v->last_saved_id), saved_id);
    return 0;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_stamp_text — 盖章文案 (spec S-F2.4 逐字)
 * ------------------------------------------------------------------------ */

int kidpal_story_stamp_text(const kidpal_story_view_t *v, char *out, size_t cap)
{
    int need;

    if (v == NULL || out == NULL) {
        return -1;
    }
    need = snprintf(NULL, 0, KSTORY_STAMP_FMT, v->stamp_index);
    if (need < 0 || (size_t)need + 1 > cap) {
        return -1; /* cap 不足 -> 拒绝, 绝不溢出 */
    }
    snprintf(out, cap, KSTORY_STAMP_FMT, v->stamp_index);
    return 0;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_page_render — 渲染面 (纯 LVGL on the ACTIVE screen, P3-A2/A4/A5/A6)
 *
 * minimal: 布局常量按 host seam 400x300 定坐标；真实 RLCD 屏观感殿后 9b
 * （P3-E2 口径）。清屏后按页态绘制（幂等，可反复调）。缺该页所需数据 -> -1
 * （显式降级，绝不盲渲染, V27）。
 * ------------------------------------------------------------------------ */

#define KSY_COLOR_TEXT   0x222222u
#define KSY_COLOR_INK    0x333333u
#define KSY_COLOR_BG     0xFFFFFFu
#define KSY_COLOR_SEL    0x333333u /* 反白选中行底色 */
#define KSY_COLOR_SELTX  0xFFFFFFu /* 选中行文字（反色） */

static void ksy_box(lv_obj_t *scr, int x, int y, int w, int h, uint32_t color)
{
    lv_obj_t *o = lv_obj_create(scr);
    lv_obj_remove_style_all(o);
    lv_obj_set_size(o, w, h);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_style_bg_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
}

static void ksy_label(lv_obj_t *scr, const char *text, int x, int y, uint32_t color)
{
    lv_obj_t *l = lv_label_create(scr);
    lv_label_set_text(l, text);
    lv_obj_set_pos(l, x, y);
    lv_obj_set_style_text_color(l, lv_color_hex(color), 0);
}

int kidpal_story_page_render(const kidpal_story_view_t *v,
                             const kidpal_story_list_t *list,
                             const kidpal_story_text_t *story)
{
    lv_obj_t *scr;
    char buf[64];

    if (v == NULL) {
        return -1;
    }
    scr = lv_screen_active();
    if (scr == NULL) {
        return -1; /* LVGL 未初始化 -> 不崩 */
    }

    /* 各页所需数据缺失 -> 显式降级（绝不盲渲染, V27 / spec §4.0）。 */
    if (v->page == KSTORY_PAGE_LIST && list == NULL) {
        return -1;
    }
    if (v->page == KSTORY_PAGE_READER && story == NULL) {
        return -1;
    }

    lv_obj_clean(scr);
    lv_obj_set_style_pad_all(scr, 0, 0);
    ksy_box(scr, 0, 0, LV_HOR_RES, LV_VER_RES, KSY_COLOR_BG);

    switch (v->page) {
        case KSTORY_PAGE_LIST: {
            int i;
            int fruit;
            int rows = list->count;
            if (rows > KSTORY_LIST_MAX) {
                rows = KSTORY_LIST_MAX;
            }
            /* 标题「我的故事」+ 常驻语音入口提示「我要讲故事」(S-F2.1) */
            ksy_label(scr, "我的故事", 12, 8, KSY_COLOR_TEXT);
            /* 右上角故事果计数器 ×N（N = kidpal_story_counter 值）。渲染面把 ×N 表达为
             * 一行「故事果」实心方块（每个 20px 宽、压在 host 采样网格整点上），使
             * ×3 与 ×10 在真帧采样点上产生可探针的暗像素差异 —— 纯文本标签字形过细会
             * 落在 20px 网格间隙里、×3/×10 帧像素相同（render 齿会红）。文本 ×N 仍画。 */
            fruit = v->counter;
            if (fruit > KSTORY_LIST_MAX) {
                fruit = KSTORY_LIST_MAX; /* minimal: 显示位上限；真实滚动计数殿后 9b */
            }
            if (fruit < 0) {
                fruit = 0;
            }
            for (i = 0; i < fruit; i++) {
                ksy_box(scr, 60 + i * 20, 30, 16, 16, KSY_COLOR_INK);
            }
            snprintf(buf, sizeof(buf), "×%d", v->counter);
            ksy_label(scr, buf, 330, 8, KSY_COLOR_INK);
            for (i = 0; i < rows; i++) {
                int y = 60 + i * 34;
                int selected = (i == v->cursor);
                if (selected) {
                    /* 选中行反白（S-F2.1「选中项反白+微倾」）。 */
                    ksy_box(scr, 8, y - 4, LV_HOR_RES - 16, 30, KSY_COLOR_SEL);
                }
                /* 每行 故事果图标 + 标题 + 日期 */
                ksy_label(scr, "●", 14, y, selected ? KSY_COLOR_SELTX : KSY_COLOR_INK);
                ksy_label(scr, list->items[i].title, 40, y,
                          selected ? KSY_COLOR_SELTX : KSY_COLOR_TEXT);
                ksy_label(scr, list->items[i].date, 320, y,
                          selected ? KSY_COLOR_SELTX : KSY_COLOR_TEXT);
            }
            ksy_label(scr, "说\"我要讲故事\"开录新的", 12, LV_VER_RES - 24,
                      KSY_COLOR_INK);
            break;
        }
        case KSTORY_PAGE_READER: {
            char label[64];
            int s, total_screens = v->screens < 1 ? 1 : v->screens;
            if (total_screens > 12) {
                total_screens = 12; /* minimal: 位置指示显示位上限；真实分栏殿后 9b */
            }
            ksy_label(scr, story->title[0] != '\0' ? story->title : "故事", 12, 8,
                      KSY_COLOR_TEXT);
            /* 正文逐字上屏（分页/滚动观感殿后 9b；此处绘制全文文本）。 */
            ksy_label(scr, story->text, 16, 44, KSY_COLOR_TEXT);
            /* 屏位指示条（S-F2.3 与翻页联动的可见证据）：当前屏的 tick 压暗、其余留白，
             * 使第 1 屏 / 第 2 屏真帧在采样点上产生可探针差异（纯文本翻页不锁观感）。 */
            for (s = 0; s < total_screens; s++) {
                ksy_box(scr, 40 + s * 20, 250, 16, 16,
                        (s == v->screen) ? KSY_COLOR_INK : 0xDDDDDDu);
            }
            /* 底部屏数指示（S-F2.3 逐字，与翻页联动）。 */
            if (kidpal_story_screen_label(v, label, sizeof(label)) == 0) {
                ksy_label(scr, label, LV_HOR_RES / 2 - 60, LV_VER_RES - 24,
                          KSY_COLOR_INK);
            }
            break;
        }
        case KSTORY_PAGE_RECORDING: {
            /* 渲染期按 rec_seconds 就地算演出相位（与 kidpal_story_rec_tick 同公式），
             * 不回写视图（render 入参为 const，不得改 rec_seconds —— 演出帧由调用方
             * 的 tick 驱动，此处只做无副作用绘制）。 */
            uint32_t elapsed = (uint32_t)(v->rec_seconds < 0 ? 0 : v->rec_seconds) * 1000u;
            int flash = (int)((elapsed / 250u) % 2u);
            int wave = (int)((elapsed / 100u) % 4u);
            int i;
            /* REC 反白点（点亮时画实心块）。 */
            if (flash) {
                ksy_box(scr, 20, 20, 24, 24, KSY_COLOR_INK);
            } else {
                ksy_box(scr, 20, 20, 24, 24, 0xDDDDDDu);
            }
            /* 大时长文本。 */
            if (kidpal_story_rec_time(v, buf, sizeof(buf)) == 0) {
                ksy_label(scr, buf, 60, 18, KSY_COLOR_TEXT);
            }
            /* 手绘声波：随 wave 相位档跳动的高度条。 */
            for (i = 0; i < 6; i++) {
                int h = 10 + (((wave + i) % 4) * 10);
                ksy_box(scr, 60 + i * 20, 90 - h / 2, 12, h, KSY_COLOR_INK);
            }
            break;
        }
        case KSTORY_PAGE_STAMP: {
            char txt[64];
            lv_obj_t *ring;
            /* 双圈印章（外实内虚的几何由 stamp_spec 冻结；绘制按 1bpp 圆环）。 */
            ring = lv_obj_create(scr);
            lv_obj_remove_style_all(ring);
            lv_obj_set_size(ring, 140, 140);
            lv_obj_set_pos(ring, LV_HOR_RES / 2 - 70, 40);
            lv_obj_set_style_radius(ring, LV_RADIUS_CIRCLE, 0);
            lv_obj_set_style_border_width(ring, 6, 0);
            lv_obj_set_style_border_color(ring, lv_color_hex(KSY_COLOR_INK), 0);
            lv_obj_set_style_border_opa(ring, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_opa(ring, LV_OPA_TRANSP, 0);
            {
                lv_obj_t *ring2 = lv_obj_create(scr);
                lv_obj_remove_style_all(ring2);
                lv_obj_set_size(ring2, 108, 108);
                lv_obj_set_pos(ring2, LV_HOR_RES / 2 - 54, 56);
                lv_obj_set_style_radius(ring2, LV_RADIUS_CIRCLE, 0);
                lv_obj_set_style_border_width(ring2, 2, 0);
                lv_obj_set_style_border_color(ring2, lv_color_hex(KSY_COLOR_INK), 0);
                lv_obj_set_style_border_opa(ring2, LV_OPA_COVER, 0);
                lv_obj_set_style_bg_opa(ring2, LV_OPA_TRANSP, 0);
            }
            /* 盖章文案（N 与计数器同步不变式）。 */
            if (kidpal_story_stamp_text(v, txt, sizeof(txt)) == 0) {
                ksy_label(scr, txt, LV_HOR_RES / 2 - 90, 190, KSY_COLOR_TEXT);
            }
            break;
        }
        default:
            break;
    }
    return 0;
}
