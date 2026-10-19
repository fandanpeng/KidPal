/*
 * story_page.c — KidPal P3 星星记录本消费面（TaskPackage 260924-m4-k2-p3-star-notebook-ui
 * 重写面; 前身 = 260829-story-p3 四态消费面）。
 *
 * 语义与形状以冻结验收 firmware/main/test/story_page_v2.acceptance.c 文件头 seam
 * contract [K1]..[K10] 为准，不得偏移；强定义在 host runner 里覆盖验收文件内的 weak
 * 哨兵 (-9999) 后被真测（同 schedule_page.c / view_router.c 先例）。
 *
 * 纯逻辑 + 纯 LVGL（lv_obj_/lv_label only，无 Board/Display class 依赖）
 * -> host_app linux target 与设备树两侧同源可编。解析器为自足式最小 JSON 扫描
 * （无外部 JSON 库依赖，frozen contract 要求；同 display/schedule_page.c 纪律）。
 *
 * 降级语义：解析畸形/缺字段/越界 cap 一律 -1 —— 调用方必须友好降级、绝不盲渲染
 * (V27 / spec §4.0)。
 */

#include "story_page.h"

#include "art_assets.h"
#include <stdio.h>
#include <string.h>

/* K3R 冻结缝 [R1] 判据的层值/页值单源（additive; kidpal_layer.h = 前台层值闭集权威,
 * kidpal_render_node.h = KP_PAGE_* 页值权威, M2K2-A2 冻结等值；两处漂移由既有冻结齿先红）。 */
#include "kidpal_layer.h"
#include "kidpal_render_node.h"

#include "lvgl.h"

/* K3R D2 真机轮「一次定死」诊断（2026-10-01）: 纯日志, 零渲染行为改动。host 与设备
 * 两侧 log 组件均在场（host linux target comps 含 log）。 */
#include <esp_log.h>
static const char *KSR_TAG = "story_page";

/* CJK 字体显式接线（seam [K9]: 全 label 不赌继承——LVGL 默认 montserrat_14 纯拉丁,
 * P3 全部中文面真机空白; lcd_display.cc 五处先例同款）。 */
LV_FONT_DECLARE(font_puhui_14_1);

/* 冻结逐字文案 (S-F2.3 / S-F2.4 / seam [K7])；字节须与验收期望串一致。 */
#define KSTORY_SCREEN_FMT     "第 %d 屏 · 共 %d 屏"
#define KSTORY_STAMP_FMT_N    "星星徽章·第 %d 颗已入册！"   /* seq 在册（带 N） */
#define KSTORY_STAMP_FMT_PLAIN "星星徽章已入册！"           /* seq 缺省降级（不带 N） */

/* 盖章几何冻结值。 */
#define KSTORY_STAMP_ANGLE_DECI (-80)
#define KSTORY_STAMP_HOLD_MS    800

/* 录音态显示位上界 = 99:59 (5999 秒)；冻结 contract 钳制显示位。 */
#define KSTORY_REC_MAX_SECONDS 5999

/* LIST 版面布局常量（UIUX L2: 按 docs/ui/V5/canvas-p3.png 定坐标; 版式像素级观感
 * 归真机轮 + design review, 本常量只钉行流几何）。 */
#define KSTORY_LIST_ROW_Y0    44   /* 首行文本 y */
#define KSTORY_LIST_ROW_STEP  42   /* 行距 */
#define KSTORY_LIST_FOOTER_Y  226  /* 常驻语音入口提示 y */
#define KSTORY_LIST_SCREEN_Y  262  /* 分屏指示 y（超一屏时） */

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

/* 第 idx 个 '\n'-分隔行的行首指针 (idx 0-based); 越界 -> NULL。ksy_line_count 同性质。 */
static const char *ksy_line_start(const char *s, int idx)
{
    const char *p = s;
    int i;

    if (s == NULL || idx < 0) {
        return NULL;
    }
    for (i = 0; i < idx; i++) {
        p = strchr(p, '\n');
        if (p == NULL) {
            return NULL;
        }
        p++;
    }
    return p;
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
 * kidpal_story_text_seq — story_text data.seq 只读探针 (seam [K6]; 契约 §1.3④)
 *
 * 非负整数 -> 值; 缺省 / 畸形（非整数/字符串/负数）/ 非 JSON -> KSTORY_SEQ_ABSENT
 * 同径降级（seq 是 additive 装饰字段, 不为它整包拒——正文照常入阅读态）。
 * ------------------------------------------------------------------------ */

int kidpal_story_text_seq(const char *data_json)
{
    ksy_cursor c;

    if (data_json == NULL) {
        return KSTORY_SEQ_ABSENT;
    }
    c.s = data_json;
    c.pos = 0;
    ksy_ws(&c);
    if (ksy_peek(&c) != '{') {
        return KSTORY_SEQ_ABSENT;
    }
    c.pos++;
    for (;;) {
        char key[KSY_KEY_BUF];
        char d;

        ksy_ws(&c);
        if (ksy_peek(&c) == '}') {
            return KSTORY_SEQ_ABSENT; /* 缺 seq */
        }
        if (!ksy_string(&c, key, sizeof(key))) {
            return KSTORY_SEQ_ABSENT;
        }
        ksy_ws(&c);
        if (ksy_peek(&c) != ':') {
            return KSTORY_SEQ_ABSENT;
        }
        c.pos++;
        ksy_ws(&c);
        if (strcmp(key, "seq") == 0) {
            long v;
            if (ksy_nonneg_int(&c, &v)) {
                return (int)v;
            }
            return KSTORY_SEQ_ABSENT; /* 非整数/字符串 -> 同径降级 */
        }
        if (!ksy_skip_value(&c)) {
            return KSTORY_SEQ_ABSENT;
        }
        ksy_ws(&c);
        d = ksy_peek(&c);
        if (d == ',') {
            c.pos++;
            continue;
        }
        return KSTORY_SEQ_ABSENT; /* '}' 结束 / 畸形 */
    }
}

/* -------------------------------------------------------------------------- *
 * LIST 分屏纯逻辑三缝 (seam [K8]; 原语形态, 行/屏数显式注入)
 * ------------------------------------------------------------------------ */

int kidpal_story_list_cursor_step(int cursor, int count, int rows, int screen,
                                  int *out_cursor, int *out_screen)
{
    int next;

    if (out_cursor == NULL || out_screen == NULL) {
        return -1;
    }
    if (count <= 0) {
        return 0; /* 零副作用: out 不触碰 */
    }
    if (rows <= 0) {
        rows = 1; /* 防御: 按 1 行/屏, 绝不除零/越界 */
    }
    next = cursor + 1;
    if (next >= count) {
        /* 到底 -> wrap 回首项（永不离页）。 */
        *out_cursor = 0;
        *out_screen = 0;
        return 1;
    }
    if (next % rows == 0) {
        /* 越过当前屏底 -> 翻下一屏并落该屏首项（next 即该屏首项下标）。 */
        *out_cursor = next;
        *out_screen = next / rows;
    } else {
        /* 屏内普通步进。 */
        *out_cursor = next;
        *out_screen = screen;
    }
    return 1;
}

int kidpal_story_list_screen_next(int screen, int screens, int rows, int count,
                                  int *out_screen, int *out_cursor)
{
    int ns;

    if (out_screen == NULL || out_cursor == NULL) {
        return -1;
    }
    if (screens <= 0) {
        return -1;
    }
    if (rows <= 0) {
        rows = 1; /* 防御同 cursor_step */
    }
    ns = (screen + 1) % screens;
    *out_screen = ns;
    *out_cursor = ns * rows;
    if (*out_cursor >= count && count > 0) {
        *out_cursor = count - 1; /* 防御: 屏数与行数不一致时收敛合法界 */
    }
    if (*out_cursor < 0) {
        *out_cursor = 0;
    }
    return 1;
}

int kidpal_story_list_screen_label(int screen, int screens, char *out, size_t cap)
{
    int need;

    if (out == NULL || screen < 0 || screens < 1) {
        return -1;
    }
    /* 与阅读态屏数指示同一单源 KSTORY_SCREEN_FMT（v1.28 与阅读态同心智）。 */
    need = snprintf(NULL, 0, KSTORY_SCREEN_FMT, screen + 1, screens);
    if (need < 0 || (size_t)need + 1 > cap) {
        return -1; /* cap 不足 -> 拒绝, 绝不截断/溢出 */
    }
    snprintf(out, cap, KSTORY_SCREEN_FMT, screen + 1, screens);
    return 0;
}

/* -------------------------------------------------------------------------- *
 * 300ms 单双击消歧纯逻辑三缝 (seam [K2]; 时间戳显式注入, 无真实 timer; 原语形态
 * 零共享态——状态归调用方/mcp 消费侧)。死线 300 严格短于板端 iot_button 双击窗
 * 350（waveshare-s3-rlcd-4.2.cc double_gap_ms=350 零触碰参照）——消歧窗是双击窗
 * 的子集, 防重复触发。
 * ------------------------------------------------------------------------ */

int kidpal_story_debounce_gate(int page)
{
    /* 状态门控（notes 边角②）: 仅列表态挂起消歧窗——双击语义仅存在于列表态;
     * 其余态单击零延迟直发（消歧逻辑不得全局生效）。 */
    return (page == KSTORY_PAGE_LIST) ? 1 : 0;
}

int kidpal_story_debounce_expired(uint32_t arm_ts_ms, uint32_t now_ms)
{
    /* [300, ∞) 到期直发（含端; uint32 回绕安全——差值算术）。 */
    return (uint32_t)(now_ms - arm_ts_ms) >= KSPV2_DISAMBIG_MS ? 1 : 0;
}

int kidpal_story_debounce_cancel(uint32_t arm_ts_ms, uint32_t now_ms)
{
    /* [0, 300) 窗内冲销挂起单击; 与 expired 构成无缝划分（到点后到达的迟到双击
     * 不冲销——单击已直发）。 */
    return (uint32_t)(now_ms - arm_ts_ms) < KSPV2_DISAMBIG_MS ? 1 : 0;
}

/* -------------------------------------------------------------------------- *
 * 删除确认子态 10s idle-step (seam [K3]; char_window kidpal_char_window_idle_step
 * 同款签名/语义缩参; **静默**取消——屏上零提示, SPV2-B2 负锚承载)。
 * ------------------------------------------------------------------------ */

int kidpal_story_delete_idle_step(unsigned long *idle_ms, unsigned long dt_ms)
{
    if (idle_ms == NULL) {
        return -1;
    }
    *idle_ms += dt_ms;
    return (*idle_ms >= KSPV2_DELETE_IDLE_MS) ? 1 : 0;
}

/* -------------------------------------------------------------------------- *
 * P3「正文取不回」覆盖层 10s idle-step (TaskPackage 261016-p3-story-body-sd-cache;
 * 形参 / 语义逐字仿 kidpal_story_delete_idle_step; **静默**消失——屏上不显剩余
 * 秒数, AC7 口径)。
 * ------------------------------------------------------------------------ */

int kidpal_story_body_unavailable_idle_step(unsigned long *idle_ms, unsigned long dt_ms)
{
    if (idle_ms == NULL) {
        return -1;
    }
    *idle_ms += dt_ms;
    return (*idle_ms >= KSTORY_BODY_UNAVAILABLE_IDLE_MS) ? 1 : 0;
}

/* -------------------------------------------------------------------------- *
 * 隐式 ack 重发决策 (seam [K4]; 契约 §四 Q5): tries < 上限 且 age >= 间隔 -> 1
 * （调用方重发同一 id 并 tries++）; 否则 0（未到期 / 已达上限放弃——放弃后静默
 * 停发, 不新增请求-应答消息对）。
 * ------------------------------------------------------------------------ */

int kidpal_story_delete_resend_due(int tries, uint32_t age_ms)
{
    if (tries < 0 || tries >= KSPV2_DELETE_RESEND_MAX) {
        return 0; /* 达上限放弃——重发有界 */
    }
    return age_ms >= KSPV2_DELETE_RESEND_MS ? 1 : 0;
}

/* -------------------------------------------------------------------------- *
 * story_list 重推对账冲销探针 (seam [K5])
 * ------------------------------------------------------------------------ */

const char *kidpal_story_pending_delete_id(const kidpal_story_view_t *v)
{
    if (v == NULL) {
        return NULL;
    }
    return v->pending_delete_id; /* 无 pending = 空串（NUL 结尾） */
}

int kidpal_story_delete_reconcile_hit(const char *pending_id,
                                      const kidpal_story_list_t *list)
{
    int i;

    if (pending_id == NULL || list == NULL) {
        return -1;
    }
    if (pending_id[0] == '\0') {
        return 0; /* 无待账不应冲销任何东西 */
    }
    for (i = 0; i < list->count && i < KSTORY_LIST_MAX; i++) {
        if (strcmp(list->items[i].id, pending_id) == 0) {
            return 1; /* 仍存在, 不冲销 */
        }
    }
    return 0; /* 不在窗口（含空窗口）-> 调用方冲销 pending 并以 total 刷新计数 */
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_text_parse — P3-A5 (契约 §1.3 ④)
 *
 * 栈纪律（包 260829-hardware-validation 真机语音轮 2 修复，勿回退）：本函数被
 * mcp kidpal/display 的 story_text 路由直接调用，整条链跑在 esp-ml307 的
 * `ssl_receive` 任务上（managed_components/78__esp-ml307/src/esp/esp_ssl.cc:62
 * xTaskCreate 栈 = 4096 字节，第三方件不可抬）。故此处**直接解析进 out**（out 由
 * 调用方提供、本就是同一块 sizeof(kidpal_story_text_t)=2208B 内存），不再另设等大的
 * id/title/text 局部缓冲——旧写法在同一条 4096B 栈上叠出 2208+2208 ≥ 4.4KB，即
 * 「A stack overflow in task main / esp_timer」跨任务腐蚀复现的净根因。
 * **禁止在本链新增大栈缓冲；大对象一律堆分配。**
 *
 * 语义与旧实现逐字等价（safe-truncate / 恒 NUL 结尾 / id·text 必为 string / 畸形
 * 返回 -1 / 尾部垃圾检查）；唯一差别：失败路径 (-1) 下 out 可能被部分写入而非保持
 * 原值——调用方拿到 -1 必须走显式降级、绝不消费 out（与本文件头降级纪律一致）。
 * ------------------------------------------------------------------------ */

int kidpal_story_text_parse(const char *data_json, kidpal_story_text_t *out)
{
    ksy_cursor c;
    int has_id = 0;
    int has_text = 0;

    if (data_json == NULL || out == NULL) {
        return -1;
    }
    /* 三字段就地初始化（缺 title 容错为 ""），解析直接写入 out 的对应成员。 */
    out->id[0] = '\0';
    out->title[0] = '\0';
    out->text[0] = '\0';

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
            if (!ksy_string(&c, out->id, sizeof(out->id))) {
                return -1;
            }
            has_id = 1;
        } else if (strcmp(key, "text") == 0) {
            if (ksy_peek(&c) != '"') {
                return -1; /* text 必为 string（正文, render-critical） */
            }
            if (!ksy_string(&c, out->text, sizeof(out->text))) {
                return -1;
            }
            has_text = 1;
        } else if (strcmp(key, "title") == 0) {
            if (ksy_peek(&c) != '"') {
                return -1; /* title 若出现须为 string（缺省才容错为 ""） */
            }
            if (!ksy_string(&c, out->title, sizeof(out->title))) {
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
    /* out->id / out->title / out->text 已由 ksy_string 就地 safe-truncate + NUL
     * 终止写入（缺 title 保持上文的 "" 初始化）；无中段缓冲、无二次拷贝。 */
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
/* KSY_COLOR_INK 已上提 story_page.h 单源（seam [TB3], 包 261012-p3-topbar-ink-bleach-fix）
 * ——本地定义退役, 两渲染入口（本文件 + kidpal_render_node.cc）同源消费、不双写色值;
 * 防 C/C++ 混编重定义（story_page.c 经 :17 include 该头取用）。 */
#define KSY_COLOR_BG     0xFFFFFFu
#define KSY_COLOR_FAINT  0xDDDDDDu
#define KSY_COLOR_BORDER 0x222222u
/* 旧反白选中色宏对（SEL 底色/SELTX 反色字）退役——「选中行带边框（不反白）」,
 * 2026-09-14 human 裁; SPV2-B2 tooth 5 负锚承载（零残留, 注释亦不引用旧名）。 */

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
    /* CJK 字体显式接线（seam [K9]: 不赌继承——默认 montserrat_14 纯拉丁, P3 中文
     * 面真机空白; lcd_display.cc 五处先例同款）。 */
    lv_obj_set_style_text_font(l, &font_puhui_14_1, 0);
}

/*
 * 换皮消费接线 (包 9b HWV-A6)：盖章帧「pose-jump 定格」(spec S-F2.4 / p3-stamp 三件套
 * 「双圈印章 + 文案 + pose-jump」) 的真机位图接入。经 U 单一资产层 loader
 * (kidpal_art_load + kidpal_art_pixel) 把 pose_jump_96 KAP1 合成成一枚 ARGB8888
 * lv_image_dsc (墨=不透明黑 / 纸及非阈值化=透明)，返回其描述符；不可用则 NULL。
 * 单槽 loader 缓存在本函数内即时拷进自建像素缓冲后即释放引用，故与其它页的 reload 无冲突。
 * 只画在 STAMP 页右下角 (rings 收在 x<270、文案 y=190 之左) 的确定空白区，几何与载荷无关，
 * 不改 story_p3 盖章齿 (该齿只断言 stamp != recording)；未 mount / 加载失败 -> NULL,
 * 页面回既有矢量双圈印章 + 文案 (best-effort 降级, 不崩)。
 */
#define KSTORY_POSE_MAX 96
static lv_image_dsc_t s_kstory_pose_dsc;
static uint32_t      *s_kstory_pose_px = NULL;
static int            s_kstory_pose_st = 0; /* 0=未试 1=可用 2=不可用 */

static const lv_image_dsc_t *kstory_pose_jump_dsc(void)
{
    kidpal_art_img_t img;

    if (s_kstory_pose_st == 2) {
        return NULL;
    }
    if (s_kstory_pose_st == 1) {
        return &s_kstory_pose_dsc;
    }
    if (kidpal_art_load("pose_jump_96.bin", &img) != 0 ||
        img.w <= 0 || img.h <= 0 || img.w > KSTORY_POSE_MAX || img.h > KSTORY_POSE_MAX) {
        s_kstory_pose_st = 2;
        return NULL;
    }
    s_kstory_pose_px = (uint32_t *)lv_malloc((size_t)img.w * img.h * sizeof(uint32_t));
    if (s_kstory_pose_px == NULL) {
        s_kstory_pose_st = 2;
        return NULL;
    }
    for (int y = 0; y < img.h; y++) {
        for (int x = 0; x < img.w; x++) {
            int p = kidpal_art_pixel(&img, x, y);
            s_kstory_pose_px[y * img.w + x] = (p == 1) ? 0xFF000000u : 0x00000000u;
        }
    }
    memset(&s_kstory_pose_dsc, 0, sizeof(s_kstory_pose_dsc));
    s_kstory_pose_dsc.header.cf     = LV_COLOR_FORMAT_ARGB8888;
    s_kstory_pose_dsc.header.w      = img.w;
    s_kstory_pose_dsc.header.h      = img.h;
    s_kstory_pose_dsc.header.stride = (uint32_t)img.w * 4u;
    s_kstory_pose_dsc.data          = (const uint8_t *)s_kstory_pose_px;
    s_kstory_pose_dsc.data_size     = (uint32_t)(img.w * img.h * 4);
    s_kstory_pose_st = 1;
    return &s_kstory_pose_dsc;
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
    if ((v->page == KSTORY_PAGE_LIST || v->page == KSTORY_PAGE_DELETE) &&
        list == NULL) {
        return -1;
    }
    if (v->page == KSTORY_PAGE_READER && story == NULL) {
        return -1;
    }

    /* K3R D2 真机轮诊断（纯日志）: story 渲染入口——子态 id + 双 count + 录音秒。 */
    ESP_LOGI(KSR_TAG, "STORY_RENDER page=%d vcount=%d lcount=%d rec=%d",
             v->page, v->count, list != NULL ? list->count : -1, v->rec_seconds);

    lv_obj_clean(scr);
    lv_obj_set_style_pad_all(scr, 0, 0);
    ksy_box(scr, 0, 0, LV_HOR_RES, LV_VER_RES, KSY_COLOR_BG);

    switch (v->page) {
        case KSTORY_PAGE_LIST: {
            int i;
            int n = v->count;
            int start;
            int row_y0 = KSTORY_LIST_ROW_Y0;
            int row_step = KSTORY_LIST_ROW_STEP;
            if (n > list->count) {
                n = list->count; /* 防御: 视图行数不超载荷窗口 */
            }
            if (n > KSTORY_LIST_MAX) {
                n = KSTORY_LIST_MAX;
            }
            if (n <= 0) {
                /* AC5 无内容空态（数据已送达、确无篇目——requirements v1.63 S-F2.1
                 * AC5; seam [SL3]）: ×0 诚实计数（计数画法照常、不隐瞒, 页身份保持）
                 * + AC5 话术 + AC3 常驻语音入口提示照常并存（两句话术不互相顶替）。
                 * 空态话术单源 = KSTORY_EMPTY_HINT（notes 边角③: 空态渲染入口在
                 * kidpal_render_node.cc 与 本处两处, 同源——两处口径一致）。
                 * AC6 数据未达空态（不显计数 + KSTORY_EMPTY_HINT_UNREACHED）走
                 * kidpal_render_node.cc 渲染入口（列表缓存从未装载态）, 不经本路径
                 * ——本 render 路径必有数据在手（list != NULL）。 */
                ksy_label(scr, "★", 316, 8, KSY_COLOR_INK);
                snprintf(buf, sizeof(buf), "×%d", v->counter);
                ksy_label(scr, buf, 344, 8, KSY_COLOR_INK);
                ksy_label(scr, KSTORY_EMPTY_HINT, 60, 140, KSY_COLOR_INK);
                ksy_label(scr, "说「我要讲故事」/「帮我记下来」开录新的",
                          12, KSTORY_LIST_FOOTER_Y, KSY_COLOR_INK);
                break;
            }
            /* 无页名标题（列表 = 星星记录本, v1.27 交互改写）; 右上角星星计数 ×N
             * 裸排（现存篇数恒信 total——SPV2-A7 计数双口径）。 */
            ksy_label(scr, "★", 316, 8, KSY_COLOR_INK);
            snprintf(buf, sizeof(buf), "×%d", v->counter);
            ksy_label(scr, buf, 344, 8, KSY_COLOR_INK);
            start = (v->screen > 0 ? v->screen : 0) * KSTORY_LIST_ROWS_PER_SCREEN;
            for (i = 0; i < KSTORY_LIST_ROWS_PER_SCREEN; i++) {
                int idx = start + i;
                int y = row_y0 + i * row_step;
                if (idx >= n) {
                    break;
                }
                if (idx == v->cursor) {
                    /* 选中行带边框（不反白——反白真机效果差, 2026-09-14 human 裁;
                     * S-F2.1 AC1; 旧反白填充退役）。 */
                    lv_obj_t *sel = lv_obj_create(scr);
                    lv_obj_remove_style_all(sel);
                    lv_obj_set_size(sel, LV_HOR_RES - 16, row_step - 6);
                    lv_obj_set_pos(sel, 8, y - 6);
                    lv_obj_set_style_radius(sel, 10, 0);
                    lv_obj_set_style_bg_opa(sel, LV_OPA_TRANSP, 0);
                    lv_obj_set_style_border_width(sel, 3, 0);
                    lv_obj_set_style_border_color(sel,
                                                  lv_color_hex(KSY_COLOR_BORDER), 0);
                    lv_obj_set_style_border_opa(sel, LV_OPA_COVER, 0);
                    lv_obj_clear_flag(sel, LV_OBJ_FLAG_SCROLLABLE);
                }
                /* 每行 星星图标 + 标题 + 日期。 */
                ksy_label(scr, "★", 16, y, KSY_COLOR_INK);
                ksy_label(scr, list->items[idx].title, 40, y, KSY_COLOR_TEXT);
                ksy_label(scr, list->items[idx].date, 320, y, KSY_COLOR_INK);
            }
            /* 常驻语音入口双指令（S-F2.1 AC3 + intro 对账③: 「我要讲故事」+
             * 「帮我记下来」双触发词逐字; 旧单指令口径退役）。 */
            ksy_label(scr, "说「我要讲故事」/「帮我记下来」开录新的",
                      12, KSTORY_LIST_FOOTER_Y, KSY_COLOR_INK);
            /* 分屏指示（v1.28, 列表超一屏时; 与阅读态同心智逐字）。 */
            if (v->screens > 1) {
                char sbuf[64];
                if (kidpal_story_list_screen_label(v->screen, v->screens,
                                                   sbuf, sizeof(sbuf)) == 0) {
                    ksy_label(scr, sbuf, LV_HOR_RES / 2 - 60,
                              KSTORY_LIST_SCREEN_Y, KSY_COLOR_INK);
                }
            }
            break;
        }
        case KSTORY_PAGE_DELETE: {
            /* 删除确认覆盖层（S-F2.1 AC4 / WF-F2-05 整屏覆盖层, 层值
             * KIDPAL_LAYER_STORY_DELETE; 页点消失由 front_is_overlay 闭集同源成立;
             * 10s 静默取消——屏上零提示, SPV2-B2 tooth 8 负锚）。版式按
             * docs/ui/V5/p3-delete-confirm.png 定坐标。 */
            lv_obj_t *q = lv_label_create(scr);
            lv_label_set_text(q, "删掉这篇？");
            lv_obj_set_pos(q, LV_HOR_RES / 2 - 60, 56);
            lv_obj_set_style_text_color(q, lv_color_hex(KSY_COLOR_TEXT), 0);
            lv_obj_set_style_text_font(q, &font_puhui_14_1, 0);
            if (v->cursor >= 0 && v->cursor < list->count) {
                /* 待删条目（星星图标 + 标题 + 日期, 带边框盒）。 */
                lv_obj_t *sel = lv_obj_create(scr);
                lv_obj_remove_style_all(sel);
                lv_obj_set_size(sel, LV_HOR_RES - 80, 40);
                lv_obj_set_pos(sel, 40, 118);
                lv_obj_set_style_radius(sel, 10, 0);
                lv_obj_set_style_bg_opa(sel, LV_OPA_TRANSP, 0);
                lv_obj_set_style_border_width(sel, 3, 0);
                lv_obj_set_style_border_color(sel, lv_color_hex(KSY_COLOR_BORDER), 0);
                lv_obj_set_style_border_opa(sel, LV_OPA_COVER, 0);
                lv_obj_clear_flag(sel, LV_OBJ_FLAG_SCROLLABLE);
                ksy_label(scr, "★", 60, 130, KSY_COLOR_INK);
                ksy_label(scr, list->items[v->cursor].title, 90, 130, KSY_COLOR_TEXT);
                ksy_label(scr, list->items[v->cursor].date, 300, 130, KSY_COLOR_INK);
            }
            break;
        }
        case KSTORY_PAGE_BODY_UNAVAILABLE: {
            /* WF-F2-06 正文取不回提示（TaskPackage 261016-p3-story-body-sd-cache;
             * requirements v1.64 S-F2.1 AC7）——整屏纯告知覆盖层：大字 + 副句
             * （话术单源 = story_page.h 的两宏）+ 所选条目行（星星 + 标题 + 日期）。
             * **键帽不在本分支绘制**：底栏 [知道了][关闭] 的单源 = keybar.c
             * （kidpal_keybar_story_slots, 由 lcd_display.cc 三槽 chrome 落屏;
             * 本页自画会落在不透明底条之下 = 死绘制、同一语义两个画法源——SC-A5
             * TEST 13 钉唯一画法源）。KEY／BOOT 任一手势关闭回列表（静默关闭）。
             * 版式按 docs/ui/V6/p3-body-unavailable.png 定坐标；标题水平居中（设计稿
             * 视觉 26px 大字：设备侧可用大字字体 font_puhui_20_4 需 LV_FONT_FMT_TXT_LARGE，
             * 该开关仅设备 sdkconfig 开启、host runner sdkconfig 不在本包 changeManifest，
             * 故本包取舍 = 沿用 14px 全量普惠体 + 居中，字号差归真机设计 review/后续轮）。
             * **静默**（不显剩余秒数; 10s 自动消失, GAP-1 观感细节归真机设计 review）。 */
            lv_obj_t *q;
            int hint_y = 64;
            int sub_y = 96;
            int box_y = 130;

            q = lv_label_create(scr);
            lv_label_set_text(q, KSTORY_BODY_UNAVAILABLE_HINT);
            lv_obj_align(q, LV_ALIGN_TOP_MID, 0, hint_y);
            lv_obj_set_style_text_color(q, lv_color_hex(KSY_COLOR_INK), 0);
            lv_obj_set_style_text_font(q, &font_puhui_14_1, 0);

            ksy_label(scr, KSTORY_BODY_UNAVAILABLE_SUB, 150, sub_y, KSY_COLOR_INK);

            /* 所选条目行（正文与所选篇目一致的可视锚——孩子看得懂「是这一篇」）。 */
            if (list != NULL && v->cursor >= 0 && v->cursor < list->count) {
                lv_obj_t *sel = lv_obj_create(scr);
                lv_obj_remove_style_all(sel);
                lv_obj_set_size(sel, LV_HOR_RES - 80, 40);
                lv_obj_set_pos(sel, 40, box_y);
                lv_obj_set_style_radius(sel, 10, 0);
                lv_obj_set_style_bg_opa(sel, LV_OPA_TRANSP, 0);
                lv_obj_set_style_border_width(sel, 3, 0);
                lv_obj_set_style_border_color(sel, lv_color_hex(KSY_COLOR_BORDER), 0);
                lv_obj_set_style_border_opa(sel, LV_OPA_COVER, 0);
                lv_obj_clear_flag(sel, LV_OBJ_FLAG_SCROLLABLE);
                ksy_label(scr, "★", 60, box_y + 12, KSY_COLOR_INK);
                ksy_label(scr, list->items[v->cursor].title, 90, box_y + 12,
                          KSY_COLOR_TEXT);
                ksy_label(scr, list->items[v->cursor].date, 300, box_y + 12,
                          KSY_COLOR_INK);
            }
            break;
        }
        case KSTORY_PAGE_READER: {
            char label[64];
            int s, total_screens = v->screens < 1 ? 1 : v->screens;
            /* 标题（S-F2.3 AC1: 标题 + 正文分页）。 */
            ksy_label(scr, story->title[0] != '\0' ? story->title : "故事", 12, 8,
                      KSY_COLOR_TEXT);
            /* 当前屏正文: 经单源切片助手 kidpal_story_reader_slice 取 st->text 内部
             * 当前屏切片 (指针/偏移, 不复制), 堆上临时 NUL 化后经 lv_label_set_text
             * 上屏 (LVGL 内部拷贝, 随即释放); 只画当前屏、非全文直画。段落设宽度 +
             * WRAP 消横向裁切 (#9 排版缺陷)。>12 屏截断已回收（notes 边角⑧——
             * 真实分栏、长文全篇可达; 旧 total_screens=12 显示位截断退役）。 */
            {
                int slen = 0;
                const char *s = kidpal_story_reader_slice(
                    story, v->screen < 0 ? 0 : v->screen,
                    KSTORY_READER_LINES_PER_SCREEN, &slen);
                char *tmp = NULL;
                if (s != NULL && slen > 0) {
                    tmp = (char *)lv_malloc((size_t)slen + 1);
                    if (tmp != NULL) {
                        memcpy(tmp, s, (size_t)slen);
                        tmp[slen] = '\0';
                    }
                }
                lv_obj_t *body = lv_label_create(scr);
                lv_label_set_text(body, tmp != NULL ? tmp : "");
                lv_obj_set_width(body, LV_HOR_RES - 32);
                lv_label_set_long_mode(body, LV_LABEL_LONG_WRAP);
                lv_obj_set_pos(body, 16, 44);
                lv_obj_set_style_text_color(body, lv_color_hex(KSY_COLOR_TEXT), 0);
                lv_obj_set_style_text_font(body, &font_puhui_14_1, 0);
                if (tmp != NULL) {
                    lv_free(tmp);
                }
            }
            /* 屏位指示条（S-F2.3 与翻页联动的可见证据）：仅在版面几何可容纳时画
             * （17 ticks = 400px 版面上限, minimal: 超长文退化为底部数字指示——
             * 分屏/切片语义不截断, 只是 tick 条不画假指示）。 */
            if (total_screens <= 17) {
                for (s = 0; s < total_screens; s++) {
                    ksy_box(scr, 40 + s * 20, 250, 16, 16,
                            (s == v->screen) ? KSY_COLOR_INK : KSY_COLOR_FAINT);
                }
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
            int wave = (int)((elapsed / 100u) % 4u);
            int i;
            /* K3R D2 真机轮 v5 诊断（纯日志, 保留）: 证明录音态分支被走 + REC 指示现状。 */
            ESP_LOGI(KSR_TAG,
                     "RECORDING render wave=%d rec_seconds=%d rec_block=(20,52,24,24) steady=1",
                     wave, v->rec_seconds);
            /* REC 指示（常亮实心黑块）。K3R D2 真机轮 v5（2026-10-01）:
             * 旧 `flash = (elapsed/250)%2`, `elapsed = rec_seconds*1000`（rec_seconds 为
             * **整秒**）-> `(rec_seconds*4)%2 ≡ 0` 恒 off -> 永远画浅灰 `0xDDDDDD`（1bpp
             * RLCD 面板 = 白 = 不可见）; **黑块那一相从未出现**（真机照片 v4/v5 报 REC
             * 不可见, 时长可见）。
             * 选**常亮**（architect 二选一之固相支）: 1bpp 面板上 off 相 = 白（不可见）,
             * 闪动会周期性令指示消失（照片轮易再报不可见）; 设计稿亦为实心点（● REC）。
             * 常亮与 rec_seconds 解耦、零时间依赖、**无条件可见**, 最稳。位置 y=52 落
             * 顶部 chrome 带（top_bar_/status_bar_ 经 lv_layer_top 恒浮）之下安全区,
             * 与时长(y=48)/声波条(y≥70)零重叠。 */
            ksy_box(scr, 20, 52, 24, 24, KSY_COLOR_INK);
            /* 大时长文本（同上避顶部 chrome 带, y 由 18 下移至 48）。 */
            if (kidpal_story_rec_time(v, buf, sizeof(buf)) == 0) {
                ksy_label(scr, buf, 60, 48, KSY_COLOR_TEXT);
            }
            /* 手绘声波：随 wave 相位档跳动的高度条。 */
            for (i = 0; i < 6; i++) {
                int h = 10 + (((wave + i) % 4) * 10);
                ksy_box(scr, 60 + i * 20, 90 - h / 2, 12, h, KSY_COLOR_INK);
            }
            /* 话语回显区（261019 [G5] 回显常驻, AC2「回显区常驻」防回退锚）: 逐句
             * 经 kidpal_story_echo_line 消费（idx 0 = 最旧），渲染在声波之下——引导
             * 播报中子态 / 整理过渡态**不得清空**回显区（echo_reset 只许定义处调用,
             * TEST 5 计数锚）。 */
            {
                int n = kidpal_story_echo_count();
                int e;
                int base_y = 120;
                for (e = 0; e < n && e < KSTORY_ECHO_MAX_LINES; e++) {
                    const char *line = NULL;
                    if (kidpal_story_echo_line(e, &line) == 0 && line != NULL) {
                        ksy_label(scr, line, 16, base_y + e * 22, KSY_COLOR_TEXT);
                    }
                }
            }
            if (v->rec_substate == 1) {
                /* 引导播报中子态渲染（帧 B, 261019 [G5]）: 气泡换引导文案（单源 =
                 * kidpal_story_guide_bubble, lcd apply_look 覆写缝同源不双写）+ 键帽
                 * 左「打断」右留空（键位分叉的可见证据——KEY 打断/BOOT 无操作,
                 * AC2; 视觉细节归设计稿 p3-recording-guide, GAP-2）。页态保持
                 * RECORDING（引导播报不跳页）。 */
                ksy_label(scr, kidpal_story_guide_bubble(), 16, 8, KSY_COLOR_TEXT);
                ksy_label(scr, "打断", 16, LV_VER_RES - 24, KSY_COLOR_INK);
            }
            break;
        }
        case KSTORY_PAGE_ORGANIZING: {
            /* 整理过渡态渲染（261019 [G5] 分支在世锚; q2 = ⓒ start_organizing 消费
             * 落地的可见半）。视觉细节（pose-think 复用 / 三问号组 / 波浪线 / 云朵 /
             * 「你刚说」标签）归设计稿 state-organizing（GAP-2 纯美学不转脆性断言）;
             * 本分支承载: 过渡提示 + 右区刚讲内容（回显环复用——**不清空**, 整理中
             * 孩子讲过的话不消失, AC2 回显常驻同源）。键帽槽留空 = 非交互过渡态
             * （KEY/BOOT 均不响应, 输入函数 case 已吞全部手势）。 */
            int n = kidpal_story_echo_count();
            int e;
            ksy_label(scr, "整理中…", 16, 40, KSY_COLOR_TEXT);
            ksy_label(scr, "你刚说", 16, 78, KSY_COLOR_INK);
            for (e = 0; e < n && e < KSTORY_ECHO_MAX_LINES; e++) {
                const char *line = NULL;
                if (kidpal_story_echo_line(e, &line) == 0 && line != NULL) {
                    ksy_label(scr, line, 16, 104 + e * 22, KSY_COLOR_TEXT);
                }
            }
            break;
        }
        case KSTORY_PAGE_STAMP: {
            kidpal_story_stamp_spec_t spec;
            int r;
            int rot;
            lv_obj_t *ring;
            /* 盖章几何单源 = 冻结契约 kidpal_story_stamp_spec（rings / 斜盖角 / 内虚 /
             * 定格时长）。渲染面必须经此消费，不得在绘制处硬编码第二套几何
             * （渲染↔契约漂移 = 双源风险；spec S-F2.4；HWV-A9 生产接线齿）。 */
            if (kidpal_story_stamp_spec(&spec) != 0) {
                /* 防御（out 恒合法 -> 不触发）：兜底取冻结常量，绝不用第二处魔法数。 */
                spec.rings = 2;
                spec.inner_dashed = 1;
                spec.angle_deci = KSTORY_STAMP_ANGLE_DECI;
                spec.hold_ms = KSTORY_STAMP_HOLD_MS;
            }
            rot = ((spec.angle_deci % 3600) + 3600) % 3600; /* LVGL9 rotation 单位 0.1°，负角归一 */
            for (r = 0; r < spec.rings && r < 2; r++) {
                /* 双圈印章：外圈实线、内圈按 spec.inner_dashed 取细线变体（1bpp 圆环）。 */
                int is_inner = (r == spec.rings - 1);
                int sz = is_inner ? 108 : 140;
                ring = lv_obj_create(scr);
                lv_obj_remove_style_all(ring);
                lv_obj_set_size(ring, sz, sz);
                lv_obj_set_pos(ring, LV_HOR_RES / 2 - sz / 2, 40 + (is_inner ? 16 : 0));
                lv_obj_set_style_radius(ring, LV_RADIUS_CIRCLE, 0);
                lv_obj_set_style_border_width(ring, (is_inner && spec.inner_dashed) ? 2 : 6, 0);
                lv_obj_set_style_border_color(ring, lv_color_hex(KSY_COLOR_INK), 0);
                lv_obj_set_style_border_opa(ring, LV_OPA_COVER, 0);
                lv_obj_set_style_bg_opa(ring, LV_OPA_TRANSP, 0);
                /* 斜盖 -8°（spec.angle_deci=-80 -> rotation=-80 单位 0.1°）绕圈心。 */
                lv_obj_set_style_transform_pivot_x(ring, sz / 2, 0);
                lv_obj_set_style_transform_pivot_y(ring, sz / 2, 0);
                lv_obj_set_style_transform_rotation(ring, rot, 0);
            }
            /* minimal: 内圈真实虚线（border dash pattern）在本 LVGL9 无 obj 便捷 setter，
             * 暂以线宽变体表意「外实内虚」，1bpp 网点虚线观感 + hold_ms 定格时长由显示
             * 循环/真机观感殿后 9b（几何数值 rings/angle_deci/inner_dashed 已单源消费）。 */
            /* 盖章文案（N = 入册流水号 seq, seam [K7] 单源消费; seq 缺省降级不带 N;
             * 计数器零耦合——旧「N 与计数器同步不变式」退役）。 */
            {
                char txt[96];
                if (kidpal_story_stamp_text(v->stamp_serial, txt, sizeof(txt)) == 0) {
                    lv_obj_t *sl = lv_label_create(scr);
                    lv_label_set_text(sl, txt);
                    lv_obj_set_pos(sl, LV_HOR_RES / 2 - 90, 190);
                    lv_obj_set_style_text_color(sl, lv_color_hex(KSY_COLOR_TEXT), 0);
                    lv_obj_set_style_text_font(sl, &font_puhui_14_1, 0);
                }
            }
            /* pose-jump 定格位图 (p3-stamp 三件套之一, spec S-F2.4)：经资产层 loader
             * 合成、右下角确定空白区 (rings 收在 x<270、文案 y=190 之左 -> 不遮不变量)。
             * 不可用时静默降级, 双圈 + 文案仍在。 */
            {
                const lv_image_dsc_t *pd = kstory_pose_jump_dsc();
                if (pd != NULL) {
                    lv_obj_t *pi = lv_image_create(scr);
                    lv_image_set_src(pi, pd);
                    lv_obj_set_pos(pi, LV_HOR_RES - (int)pd->header.w - 8,
                                   LV_VER_RES - (int)pd->header.h - 8);
                }
            }
            break;
        }
        default:
            break;
    }
    return 0;
}
/* -------------------------------------------------------------------------- *
 * kidpal_story_view_init — 列表视图装载 (刷新以云端为权威)
 *
 * seam [K1]: LIST 态 screens 按列表分屏单源预置（KSTORY_LIST_ROWS_PER_SCREEN）;
 * stamp_serial 初值 = KSTORY_SEQ_ABSENT（N = seq 唯一权威源, 板端不预置流水号——
 * seam [K7]）; pending_delete_id 清账。
 * ------------------------------------------------------------------------ */

/* 待删 id 闭集影子（seam [K1] id 闭集纪律「只发 story_list items 下发过的 id」的
 * 板端载体: view_init 从云端载荷捕获, DELETE 确认时按 cursor 取值记账。单实例消费
 * 现实 = mcp g_story_view 单例; minimal: 影子随视图整体重装刷新, 不做跨视图持久）。 */
static char s_ksy_item_ids[KSTORY_LIST_MAX][64];
static int  s_ksy_item_ids_n = 0;

/* 回列表时按现存行数恢复列表分屏几何（screens/screen 由 count 重导出——阅读态/
 * 删除子态/录音态借用过 screen/screens 字段, 回列表必须归还列表语义）。 */
static void ksy_story_restore_list_screen(kidpal_story_view_t *v)
{
    int rows = KSTORY_LIST_ROWS_PER_SCREEN;
    int screens = (v->count + rows - 1) / rows;

    if (screens < 1) {
        screens = 1;
    }
    v->screens = screens;
    v->screen = (v->cursor >= 0) ? (v->cursor / rows) : 0;
    if (v->screen > screens - 1) {
        v->screen = screens - 1;
    }
}

int kidpal_story_view_init(kidpal_story_view_t *v, const kidpal_story_list_t *list)
{
    int rows = KSTORY_LIST_ROWS_PER_SCREEN;
    int i;

    if (v == NULL) {
        return -1;
    }
    v->page = KSTORY_PAGE_LIST;
    v->cursor = 0;
    v->screen = 0;
    v->count = (list != NULL) ? list->count : 0;
    v->counter = kidpal_story_counter(list);
    v->screens = (v->count + rows - 1) / rows;
    if (v->screens < 1) {
        v->screens = 1;
    }
    v->rec_seconds = 0;
    v->stamp_serial = KSTORY_SEQ_ABSENT;
    v->archive_pend = 0;
    v->cancel_count = 0;
    v->tts_requests = 0;
    v->last_saved_id[0] = '\0';
    v->pending_delete_id[0] = '\0';
    v->rec_substate = 0; /* [G3] 引导播报中子态复位常规（新视图零残留） */

    s_ksy_item_ids_n = 0;
    if (list != NULL) {
        int cap = (list->count < KSTORY_LIST_MAX) ? list->count : KSTORY_LIST_MAX;
        for (i = 0; i < cap; i++) {
            ksy_copy_str(s_ksy_item_ids[i], sizeof(s_ksy_item_ids[i]), list->items[i].id);
        }
        s_ksy_item_ids_n = cap;
    }
    return 0;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_page_input — P3 页内输入特例（seam [K1]: v1.47 矩阵逐格, 动作码
 * 闭集 = story_page.h kidpal_story_act_t 冻结数值; 旧 KSA_* 闭集随 archive 验收退役）。
 *
 * 决策表（矩阵逐格, v1.47 requirements §2.3）:
 *   LIST:    KEY 短=REC_SESSION / KEY 双=OPEN_READER / KEY 长=GLOBAL(回首页) /
 *            BOOT 短=CURSOR_NEXT / BOOT 双=SCREEN_JUMP / BOOT 长=DELETE_ENTER
 *   DELETE:  KEY 短=DELETE_CONFIRM / KEY 双=NONE / KEY 长=NONE（不响应=消费不动,
 *            绝不 GLOBAL——漏网手势会被通用覆盖层列误映射 SESSION_TOGGLE）/ BOOT
 *            短·双·长=DELETE_CANCEL
 *   READER:  KEY 短=NONE / KEY 双=NONE / KEY 长=READER_EXIT / BOOT 短=PAGE_TURN /
 *            BOOT 双=PAGE_TURN / BOOT 长=NONE（页循环 P3 断开, 不离页）
 *   RECORDING: KEY 短=STOP_RECORD / KEY 长=STOP_RECORD（同短按保存, v1.47）/ BOOT
 *            短·双·长=DISCARD_RECORD
 *            RECORDING+rec_substate=1（引导播报中子态, 261019 [G3] 帧 B 键位分叉——
 *            本包固件侧最关键的键位分叉: 子态不拦截则引导播报一句期间按 BOOT 即毁稿）:
 *            KEY 短=INTERRUPT_GUIDE(12) / KEY 长=12（同短按, §2.1 打断停播语义;
 *            页态保持 RECORDING、子态复位 0、零 cancel/归档副作用）/ BOOT
 *            短·双·长=NONE（无操作, 绝不落 DISCARD_RECORD 毁稿路径）
 *   ORGANIZING（整理过渡态, 261019 [G4]）: KEY/BOOT 任意手势=NONE（非交互过渡态,
 *            整理中 KEY/BOOT 均不响应, AC4/§2.3; 页态保持、零副作用）
 *   STAMP:   全手势=GLOBAL（演出帧页内零语义, 零回归守卫）
 *   BODY_UNAVAILABLE（WF-F2-06 取不回提示, 261016 批 append）: KEY／BOOT 任一手势
 *            （短/双/长）= NONE 并关闭回列表（纯告知、无破坏性动作、绝不 GLOBAL）
 *   空列表:  KEY 短=REC_SESSION / BOOT 短·双=NONE / BOOT 长=NONE
 * ------------------------------------------------------------------------ */

int kidpal_story_page_input(kidpal_story_view_t *v, key_id_t key, key_gesture_t g)
{
    if (v == NULL || key >= KEY_ID_COUNT) {
        return KSPV2_ACT_DEGRADE; /* NULL 视图 / 越界键位（PWR 面）：零污染 */
    }
    if (g == KEY_GESTURE_NONE) {
        return KSPV2_ACT_NONE; /* 无手势 = 无副作用 */
    }

    switch (v->page) {
        case KSTORY_PAGE_LIST: {
            int rows = KSTORY_LIST_ROWS_PER_SCREEN;

            if (key == KEY_ID_KEY && g == KEY_GESTURE_CLICK) {
                /* 记录语境会话语义位（S-F2.2 双入口; K3 会话动作接线消费位——
                 * 本包只钉语义位; 空列表不受影响）。 */
                return KSPV2_ACT_REC_SESSION;
            }
            if (key == KEY_ID_KEY && g == KEY_GESTURE_DOUBLE_CLICK) {
                if (v->count <= 0) {
                    return KSPV2_ACT_NONE; /* 无行可读 */
                }
                /* OPEN_READER = 请求语义位：页态不就地切换（阅读态进入由消费侧配对
                 * kidpal_story_open_reader 承载——冻结流 A1 tooth 4 在 OPEN_READER
                 * 后仍断言 LIST 态光标步进; A6 fixture 亦以 open_reader 进阅读）。 */
                return KSPV2_ACT_OPEN_READER;
            }
            if (key == KEY_ID_KEY && g == KEY_GESTURE_LONG_PRESS) {
                return KSPV2_ACT_GLOBAL; /* 回首页（列表 = P3 顶层, 长按一层即到家） */
            }
            if (key == KEY_ID_BOOT && g == KEY_GESTURE_CLICK) {
                if (v->count <= 0) {
                    return KSPV2_ACT_NONE; /* 空列表不动（绝不 GLOBAL 误翻页） */
                }
                /* 光标下移（屏感知: 屏底翻屏落首项 / 到底 wrap 回首项, 永不离页）。 */
                (void)kidpal_story_list_cursor_step(v->cursor, v->count, rows,
                                                    v->screen, &v->cursor, &v->screen);
                return KSPV2_ACT_CURSOR_NEXT;
            }
            if (key == KEY_ID_BOOT && g == KEY_GESTURE_DOUBLE_CLICK) {
                int screens;
                if (v->count <= 0) {
                    return KSPV2_ACT_NONE;
                }
                screens = (v->count + rows - 1) / rows;
                if (screens < 1) {
                    screens = 1;
                }
                /* 跳屏: 翻下一屏、光标落该屏首项（末屏 wrap 回首屏）。 */
                (void)kidpal_story_list_screen_next(v->screen, screens, rows,
                                                    v->count, &v->screen, &v->cursor);
                return KSPV2_ACT_SCREEN_JUMP;
            }
            if (key == KEY_ID_BOOT && g == KEY_GESTURE_LONG_PRESS) {
                if (v->count <= 0) {
                    return KSPV2_ACT_NONE; /* 无行可删 */
                }
                v->page = KSTORY_PAGE_DELETE;
                v->pending_delete_id[0] = '\0'; /* 进子态先清账, 确认时再记 */
                return KSPV2_ACT_DELETE_ENTER;
            }
            return KSPV2_ACT_GLOBAL;
        }

        case KSTORY_PAGE_BODY_UNAVAILABLE:
            /* P3「正文取不回」覆盖层（WF-F2-06, AC7）: 纯告知态 —— KEY／BOOT
             * **任一手势**（短按 / 双击 / 长按）一律关闭回列表, 两键语义一致、
             * 无破坏性动作。返回 NONE（页拥有但无副作用）: 回列表的重渲由消费侧
             * （mcp HandleStoryKey 关闭路径）承载, 绝不 GLOBAL 外泄（漏网手势会被
             * 通用覆盖层列误映射成 SESSION_TOGGLE）。 */
            ksy_story_restore_list_screen(v);
            v->page = KSTORY_PAGE_LIST;
            return KSPV2_ACT_NONE;

        case KSTORY_PAGE_DELETE:
            /* 删除确认子态裁决（S-F2.1 AC4）: **必须吞掉全部手势**（任何漏网手势会
             * 被 key_route_layer 通用覆盖层列误映射成 SESSION_TOGGLE——删除确认里
             * 绝不能开会话）。每次手势消费即复位 idle 累计（mcp 消费侧义务的页内
             * 配套——见 mcp_server.cc HandleStoryKey）。 */
            if (key == KEY_ID_KEY && g == KEY_GESTURE_CLICK) {
                /* 确认删除——本地即时移除（边角⑨: count/counter -1、cursor 收敛
                 * 合法界）+ pending_delete_id 记账（seam [K5] 探针观察）。 */
                if (v->count > 0 && v->cursor >= 0 && v->cursor < s_ksy_item_ids_n) {
                    int i;
                    ksy_copy_str(v->pending_delete_id, sizeof(v->pending_delete_id),
                                 s_ksy_item_ids[v->cursor]);
                    for (i = v->cursor; i + 1 < s_ksy_item_ids_n; i++) {
                        ksy_copy_str(s_ksy_item_ids[i], sizeof(s_ksy_item_ids[i]),
                                     s_ksy_item_ids[i + 1]);
                    }
                    if (s_ksy_item_ids_n > 0) {
                        s_ksy_item_ids_n--;
                    }
                } else {
                    v->pending_delete_id[0] = '\0'; /* 无可删账目 -> 空账 */
                }
                if (v->count > 0) {
                    v->count--;
                }
                if (v->counter > 0) {
                    v->counter--;
                }
                if (v->cursor > v->count - 1) {
                    v->cursor = v->count - 1;
                }
                if (v->cursor < 0) {
                    v->cursor = 0;
                }
                ksy_story_restore_list_screen(v);
                v->page = KSTORY_PAGE_LIST;
                return KSPV2_ACT_DELETE_CONFIRM;
            }
            if (key == KEY_ID_BOOT) {
                /* BOOT 任一手势（短/双/长）= 取消回列表（零副作用）。 */
                ksy_story_restore_list_screen(v);
                v->page = KSTORY_PAGE_LIST;
                return KSPV2_ACT_DELETE_CANCEL;
            }
            /* KEY 双击 / KEY 长按 = 不响应（NONE, 消费不动, 页态保持）。 */
            return KSPV2_ACT_NONE;

        case KSTORY_PAGE_READER:
            if (key == KEY_ID_KEY) {
                if (g == KEY_GESTURE_LONG_PRESS) {
                    /* KEY 长按 = 退出阅读回列表（v1.36 返回上一层实例）。 */
                    ksy_story_restore_list_screen(v);
                    v->page = KSTORY_PAGE_LIST;
                    return KSPV2_ACT_READER_EXIT;
                }
                /* KEY 短按/双击 = 不响应（朗读 v1.52 已砍; tts_requests 零递增——
                 * 旧 TTS_READ 语义退役, S-F2.3 AC2）。 */
                return KSPV2_ACT_NONE;
            }
            if (g == KEY_GESTURE_CLICK || g == KEY_GESTURE_DOUBLE_CLICK) {
                /* BOOT 短按（双击同短按）= 翻下一屏, 末屏 wrap 回首屏、永不离页。 */
                int screens = (v->screens < 1) ? 1 : v->screens;
                v->screen = (v->screen + 1) % screens;
                if (v->screen < 0) {
                    v->screen = 0;
                }
                return KSPV2_ACT_PAGE_TURN;
            }
            /* BOOT 长按 = NONE 不离页（页循环在 P3 断开）。 */
            return KSPV2_ACT_NONE;

        case KSTORY_PAGE_RECORDING:
            if (v->rec_substate == 1) {
                /* 引导播报中子态（帧 B, 261019 [G3] 键位分叉——毁稿防线）:
                 * KEY 短/长按 = 打断引导、继续聆听（子态复位 0, 页态保持 RECORDING,
                 * 零 cancel/归档副作用——打断不是收尾也不是丢弃）; 消费方 = mcp
                 * StoryDispatchAct case 12 -> AbortSpeaking + 继续聆听。 */
                if (key == KEY_ID_KEY &&
                    (g == KEY_GESTURE_CLICK || g == KEY_GESTURE_LONG_PRESS)) {
                    v->rec_substate = 0;
                    return KSPV2_ACT_INTERRUPT_GUIDE;
                }
                if (key == KEY_ID_BOOT) {
                    /* BOOT 任一手势（短/双/长）= 无操作: 页态保持、cancel_count /
                     * archive_pend 不动——绝不落 DISCARD_RECORD 毁稿路径（引导播报
                     * 一句期间按 BOOT 即毁稿 = 本包固件侧最关键分叉, AC2「BOOT 无
                     * 操作」）。子态保持 1（引导播报尚未被打断）。 */
                    return KSPV2_ACT_NONE;
                }
                return KSPV2_ACT_GLOBAL;
            }
            if (key == KEY_ID_KEY &&
                (g == KEY_GESTURE_CLICK || g == KEY_GESTURE_LONG_PRESS)) {
                /* KEY 短按 & 长按同短按 = 停录保存（防长按误触毁稿 v1.47——旧
                 * 「长按丢弃」口径退役, 方向反转）。 */
                v->page = KSTORY_PAGE_LIST; /* rec_seconds 定格 */
                v->archive_pend++;          /* 停录 -> 一次归档请求 */
                ksy_story_restore_list_screen(v);
                return KSPV2_ACT_STOP_RECORD;
            }
            if (key == KEY_ID_BOOT) {
                /* BOOT 任一手势（短/双/长同义）= 取消录音、丢弃退回列表（不设重录
                 * 恢复 v1.49; V31 纪律）。 */
                v->page = KSTORY_PAGE_LIST;
                v->cancel_count++;
                ksy_story_restore_list_screen(v);
                return KSPV2_ACT_DISCARD_RECORD;
            }
            return KSPV2_ACT_GLOBAL;

        case KSTORY_PAGE_ORGANIZING:
            /* 整理过渡态（261019 [G4]）: KEY／BOOT 任意手势 = NONE 不响应——非交互
             * 过渡态（requirements v1.65 AC4「整理中 KEY/BOOT 均不响应」, §2.3 录音态
             * 子态按键口径; 误触会打断整理演出或误触毁稿面）。页态保持、零副作用
             * （绝不 restore/cancel/归档）, 消费即吞手势（绝不 GLOBAL 外泄）。 */
            return KSPV2_ACT_NONE;

        case KSTORY_PAGE_STAMP:
            return KSPV2_ACT_GLOBAL; /* 盖章帧页内零语义（零回归守卫） */

        default:
            return KSPV2_ACT_GLOBAL;
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
        v->rec_substate = 0; /* [G3] 重入录音态 = 常规聆听子态（引导子态不跨沿残留） */
        v->screen = 0;
        /* 冻结决策 D2: 录完前不预推计数（counter 不变; 取消的故事不该占故事果位）。 */
        return 1;
    }
    if (ctrl_action == 3) { /* VRT_CTRL_START_ORGANIZING（261019 [G2], q2 = ⓒ） */
        if (v->page == KSTORY_PAGE_ORGANIZING) {
            /* 重复幂等：网关重试下发不得翻倍副作用、页态保持。 */
            return 1;
        }
        v->page = KSTORY_PAGE_ORGANIZING; /* 整理过渡演出（state-organizing 帧） */
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
 * kidpal_story_reader_slice — READER 渲染切片单源 (RP-A2/F5, REVISION LOG
 * 2026-09-12)。返回指向 st->text 内部 (指针/偏移, 不复制) 的第 screen 屏正文切片,
 * 长度经 out_len 回传 (可为 NULL)。无大栈中转缓冲 (ssl_receive 4096B 栈纪律)。
 * 越界屏 / NULL / lines_per_screen<=0 -> NULL, 绝不 OOB。切片按 '\n' 自然行计
 * (与 open_reader 同性质)。
 * ------------------------------------------------------------------------ */
const char *kidpal_story_reader_slice(const kidpal_story_text_t *st, int screen,
                                      int lines_per_screen, int *out_len)
{
    const char *start;
    const char *end;
    int target;
    int total;

    if (st == NULL || lines_per_screen <= 0 || screen < 0) {
        return NULL;
    }
    total = ksy_line_count(st->text);
    target = screen * lines_per_screen;
    if (target >= total) {
        return NULL; /* 越界屏 */
    }
    start = ksy_line_start(st->text, target);
    if (start == NULL) {
        return NULL;
    }
    /* 本屏内容边界 = 下一屏首行行首; 无下一屏 -> 串尾。 */
    end = ksy_line_start(st->text, target + lines_per_screen);
    if (end == NULL) {
        end = st->text + strlen(st->text);
    }
    if (end < start) {
        return NULL;
    }
    if (out_len != NULL) {
        *out_len = (int)(end - start);
    }
    return start;
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
 * kidpal_story_on_saved — 云端保存成功 ack -> 盖章帧 (S-F2.2/S-F2.4, seam [K7])
 *
 * 三参改形（旧两参退役）: stamp N = seq（入册流水号, 契约 §1.3④ 唯一权威源——
 * 账本 owner = K1; 只增不减、不被删除改写）。**counter 零触碰**（现存篇数权威恒 =
 * story_list.total, 由重推刷新——板端不本地推计数; 旧「N = counter + 1 本地推」
 * 冻结决策 D3 退役）。seq 缺省（KSTORY_SEQ_ABSENT）-> 降级演出不带 N。同一
 * saved_id 幂等（mcp 回执重试不重复演出）。
 * ------------------------------------------------------------------------ */

int kidpal_story_on_saved(kidpal_story_view_t *v, const char *saved_id, int seq)
{
    if (v == NULL || saved_id == NULL || saved_id[0] == '\0') {
        return -1; /* 畸形 ack -> 状态零污染 */
    }
    if (strcmp(v->last_saved_id, saved_id) == 0) {
        /* 同一 saved_id 重复 ack -> 幂等（不重复演出；页态保持盖章帧）。 */
        v->page = KSTORY_PAGE_STAMP;
        return 0;
    }
    v->stamp_serial = seq;
    v->page = KSTORY_PAGE_STAMP;
    ksy_copy_str(v->last_saved_id, sizeof(v->last_saved_id), saved_id);
    return 0;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_stamp_serial — 入册流水号只读探针 (seam [K7])
 * ------------------------------------------------------------------------ */

int kidpal_story_stamp_serial(const kidpal_story_view_t *v)
{
    if (v == NULL) {
        return KSTORY_SEQ_ABSENT;
    }
    return v->stamp_serial;
}

/* -------------------------------------------------------------------------- *
 * kidpal_story_stamp_text — 盖章文案逐字二值决策表 (S-F2.4 AC1 + 契约 Q4 降级
 * 形态, seam [K7]; 决策点 3 冻结文案)
 * ------------------------------------------------------------------------ */

int kidpal_story_stamp_text(int serial, char *out, size_t cap)
{
    int need;

    if (out == NULL) {
        return -1;
    }
    if (serial >= 0) {
        need = snprintf(NULL, 0, KSTORY_STAMP_FMT_N, serial);
        if (need < 0 || (size_t)need + 1 > cap) {
            return -1; /* cap 不足 -> 拒绝, 绝不截断/溢出 */
        }
        snprintf(out, cap, KSTORY_STAMP_FMT_N, serial);
    } else {
        need = snprintf(NULL, 0, "%s", KSTORY_STAMP_FMT_PLAIN);
        if (need < 0 || (size_t)need + 1 > cap) {
            return -1;
        }
        snprintf(out, cap, "%s", KSTORY_STAMP_FMT_PLAIN);
    }
    return 0;
}

/* ========================================================================== *
 * K3 冻结缝追加区（TaskPackage 260924-m4-k3-record-session-live; additive 尾部
 * 追加, 冻结纪律只增不改/移）。seam contract [S1]-[S4] 权威 =
 * firmware/main/test/record_session_live.acceptance.c 文件头; host runner 把本
 * 文件编入源清单, 本区强符号覆盖该验收件 weak 哨兵。
 * ========================================================================== */

/* ---- [S1] source_page 映射单点（notes ④: 映射散落 = 漏入口根因, 一次做全） ---- */

const char *kidpal_story_ctx_source_page(int page)
{
    /* KP_PAGE_* 数值镜像 = M2K2-A2 冻结等值（HOME=0/SCHEDULE=1/STORY=2）;
     * kidpal_render_node.h 枚举漂移时 M2K2-A2 先红、本行为齿随之红。 */
    switch (page) {
    case 2: /* KP_PAGE_STORY */
        return "story";
    case 1: /* KP_PAGE_SCHEDULE */
        return "schedule";
    case 0: /* KP_PAGE_HOME */
    default: /* 负值/未知 -> 闭集缺省兜底, 绝不外溢第四取值 */
        return "home";
    }
}

/* ---- [S2] 引导态纯逻辑二缝（S-F2.2-AC1 屏显引导 + 开口切录音态） ---- */

const char *kidpal_story_guide_bubble(void)
{
    /* AC1 逐字单源; lcd_display.cc apply_look 区覆写缝消费（M1 冻结映射零触碰）。 */
    return "需要我帮你记点什么？";
}

int kidpal_story_recording_enter(int page, int ctx_active, int speaking)
{
    /* 决策表: LIST+记录语境+开口 -> 切; 未开口/非记录语境/已录音态（幂等）/阅读态/
     * 越界 -> 不切。 */
    if (page != KSTORY_PAGE_LIST || ctx_active == 0 || speaking == 0) {
        return 0;
    }
    return 1;
}

/* ---- [S3] story 录音态话语回显纯逻辑缝（与 kidpal_voice_echo_* 零共享态） ---- */

#define KSTORY_ECHO_LINE_CAP 128 /* builder 定容（单句上限; 冻结值只钉行数 4） */
static char s_story_echo[KSTORY_ECHO_MAX_LINES][KSTORY_ECHO_LINE_CAP];
static int s_story_echo_head = 0;  /* 下一写入位（环形） */
static int s_story_echo_n = 0;     /* 当前句数（<= KSTORY_ECHO_MAX_LINES） */

int kidpal_story_echo_reset(void)
{
    memset(s_story_echo, 0, sizeof(s_story_echo));
    s_story_echo_head = 0;
    s_story_echo_n = 0;
    return 0;
}

int kidpal_story_echo_push(const char *text)
{
    size_t i;

    if (text == NULL || text[0] == '\0') {
        return -1; /* NULL/空串拒绝, 不污染计数 */
    }
    for (i = 0; text[i] != '\0'; i++) {
        if (text[i] != ' ' && text[i] != '\t' && text[i] != '\n' &&
            text[i] != '\r' && text[i] != '\v' && text[i] != '\f') {
            break;
        }
    }
    if (text[i] == '\0') {
        return -1; /* 全空白（VAD 切句尾噪）拒绝 */
    }
    /* 满容量挤最旧 = 覆写 head 位（FIFO 环）; 未满则累加。 */
    ksy_copy_str(s_story_echo[s_story_echo_head], KSTORY_ECHO_LINE_CAP, text);
    s_story_echo_head = (s_story_echo_head + 1) % KSTORY_ECHO_MAX_LINES;
    if (s_story_echo_n < KSTORY_ECHO_MAX_LINES) {
        s_story_echo_n++;
    }
    return s_story_echo_n;
}

int kidpal_story_echo_count(void)
{
    return s_story_echo_n;
}

int kidpal_story_echo_line(int idx, const char **out)
{
    int pos;

    if (out == NULL || idx < 0 || idx >= s_story_echo_n) {
        return -1;
    }
    /* idx 0 = 最旧: 环上最旧位 = head - n（模容量）。 */
    pos = (s_story_echo_head - s_story_echo_n + idx + KSTORY_ECHO_MAX_LINES) %
          KSTORY_ECHO_MAX_LINES;
    *out = s_story_echo[pos];
    return 0;
}

/* ---- [S4] story_list 推送 gating 纯逻辑（defer ①恢复重裁板端消费半） ---- */

int kidpal_story_list_gate(int voice_front, int page_state)
{
    /* VOICE 层前台（引导态/录音态/问答会话）或视图已入录音态 -> 只装缓存不推前台
     * （防 story_list 帧盖演出, defer 真机缺陷形态的板端防护）; 其余（LIST 冷浏览/
     * READER/STAMP/DELETE/未知态）-> 推前台（K2 消费面零偏移, 未知态不吞推送）。 */
    if (voice_front != 0 || page_state == KSTORY_PAGE_RECORDING) {
        return 1;
    }
    return 0;
}

/* ---- 记录语境标志（builder 侧消费门; 置位单点 = mcp_server REC_SESSION 消费位,
 * 清位 = 会话收尾 Idle 分支; 盲区申报③: 标志本体不钉） ---- */

static int s_story_ctx_active = 0;

int kidpal_story_session_ctx_active(void)
{
    return s_story_ctx_active;
}

void kidpal_story_session_ctx_set(int active)
{
    s_story_ctx_active = (active != 0) ? 1 : 0;
}

/* ========================================================================== *
 * K3R 冻结缝追加区（TaskPackage 261001-m4-k3-record-session-live-repair;
 * additive 尾部追加, 冻结纪律只增不改/移）。seam contract [R1]-[R3] 权威 =
 * firmware/main/test/record_session_repair.acceptance.c 文件头; host runner 把本
 * 文件编入源清单, 本区强符号覆盖该验收件 weak 哨兵。
 * ========================================================================== */

/* ---- [R1] 录音语境键手势归属判据（D4a/D2 同一设计决策内一致选定） ---- */

int kidpal_story_key_owns(int front_layer, int kidpal_page, int page_state,
                          int ctx_active)
{
    /* (a) 记录语境录音态 -> 故事页拥有 KEY（**形态无关**: 甲 VOICE 已让位
     * front_layer 到此处已归一为 NONE, 本支冗余但无害; 乙 VOICE 前台, 本支是判据
     * 命门。两形态同结论）。若不判此支, KEY 会落 key_route_layer ->
     * GLOBAL_ACT_SESSION_TOGGLE -> Listening 关通道不发 listen stop -> front
     * ws_disconnect cancel 丢弃 = 毁稿。 */
    if (page_state == KSTORY_PAGE_RECORDING && ctx_active != 0) {
        return 1;
    }
    /* (b) 删除确认覆盖层吞全手势（任何漏网手势会被通用覆盖层列误映射
     * SESSION_TOGGLE, 删除确认里绝不能开会话; K2 既有语义零偏移）。 */
    if (front_layer == KIDPAL_LAYER_STORY_DELETE) {
        return 1;
    }
    /* (c) 基层 P3 故事页既有语义（keya_story_page_wants 原「overlay_top == 0 &&
     * page == P3」逐字等价, K2/K3 主页面路径零回归）。 */
    if (front_layer == KIDPAL_LAYER_NONE && kidpal_page == KP_PAGE_STORY) {
        return 1;
    }
    /* 其余（VOICE/其它覆盖层前台且非记录语境 / 基层非 P3 / 记录语境标志未置）-> 0。 */
    return 0;
}

/* ---- [R2] 会话收尾录音态复位判据（D4b-1） ---- */

int kidpal_story_session_end(int page_state, int *out_page)
{
    if (out_page == NULL) {
        return -1; /* 畸形入参拒绝, 不崩 */
    }
    if (page_state == KSTORY_PAGE_RECORDING) {
        /* 录音态 -> 复位 LIST（调用方据此停录音演出定时器; 页态回落 LIST 后
         * StoryRecTimerCb 的 page != RECORDING 自删条件成立, 定时器终停）。 */
        *out_page = KSTORY_PAGE_LIST;
        return 1;
    }
    /* 非录音态幂等 no-op（不得误清列表/阅读/删除子态; out_page 零触碰）。 */
    return 0;
}

/* ---- [R3] defer 缓存消费判据（D1; defer 分支缓存 <-> 前台回落重渲的闸门） ---- */

int kidpal_story_list_deferred_flush(int voice_front, int kidpal_page,
                                     int page_state)
{
    /* VOICE 层已弹出（前台回落基层, voice_front == 0）&& 本地页 == P3 && 列表态 ->
     * 以 g_story_list 最新缓存重渲 P3 前台（defer 分支缓存的消费方; 260928-story-
     * connect-push-defer 摘除建连即推后, 「下次前台回落时以最新缓存渲染」的驱动方
     * 缺陷修复）。其余: VOICE 仍前台不抢推 / 本地页非 P3 / 已入子态不覆盖 -> 0。 */
    if (voice_front == 0 && kidpal_page == KP_PAGE_STORY &&
        page_state == KSTORY_PAGE_LIST) {
        return 1;
    }
    return 0;
}

/* ---- [R4] LCD 声波 chrome 前台判据（D2 根因 1「弹层 ≠ 隐屏」） ---- */

int kidpal_story_voice_chrome_front(int layer_top, int page_state)
{
    /* (a) 录音页态 -> chrome 退隐（**判别位**: 旧内联逻辑 = layer_top == VOICE 无录音
     * 维度, (VOICE, RECORDING) 旧答 1 = chrome 驻留盖住录音页; 本判据答 0）。与 layer
     * 无关——甲形态 VOICE 已弹仍须防状态机 listening 残影, 乙形态 VOICE 未弹亦须显式
     * 退隐。 */
    if (page_state == KSTORY_PAGE_RECORDING) {
        return 0;
    }
    /* (b) 普通聆听轮 chrome 前台（既有 RP-A5 r3 语义零偏移）。 */
    if (layer_top == KIDPAL_LAYER_VOICE) {
        return 1;
    }
    /* 其余（内容页/其它覆盖层前台 / 基层浏览）chrome 退隐。 */
    return 0;
}

/* ---- [R5] voice_end 前台回落渲染页态解析判据（D2 根因 2） ---- */

int kidpal_story_render_page_state(int node_cached_page, int session_page,
                                   int on_story_front)
{
    /* 本地页 = P3 前台回落 -> 以权威会话页态为准（**判别位**: 旧内联逻辑裸读节点缓存
     * s_story_view.page——(node=LIST, session=RECORDING, front=1) 旧答 LIST = 渲陈旧
     * 列表页; 本判据答 RECORDING = 渲录音页。页态同步先于渲染）。非 P3 前台 -> 节点
     * 缓存零回归。 */
    if (on_story_front != 0) {
        return session_page;
    }
    return node_cached_page;
}
