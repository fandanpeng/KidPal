/*
 * schedule_page.c — KidPal P2 一周总览网格页消费面：两个 FROZEN OFFLINE CONTRACT
 * 的实现 (TaskPackage 260921-m3-k2-p2-week-grid-page, 契约 §1.3② 周网格 data /
 * S-F1.2 / NFR-READ-1; 2026-09-21 契约门定稿)。
 *
 * 语义与形状以冻结验收 firmware/main/test/schedule_grid.acceptance.c 文件头为准，
 * 不得偏移；强定义在 host runner 里覆盖验收文件内的 weak 哨兵 (-2) 后被真测
 * （同 view_router.c 先例）。
 *
 * 纯逻辑 + 纯 LVGL（lv_obj_/lv_label only，无 Board/Display class 依赖）
 * -> host_app linux target 与设备树两侧同源可编。解析器为自足式最小 JSON 扫描
 * （无外部 JSON 库依赖，frozen contract 要求）；safe-truncate / JSON 扫描小工具
 * 自 260829-schedule-p2 旧列表页整页重写中沿用（版式主体作废重写，plan.md 取舍）。
 *
 * 降级语义：解析畸形/缺字段/越界 cap 一律 -1 —— 调用方必须友好降级、绝不盲渲染
 * (V27 / spec §4.0)。旧列表形状 {title,items} 无 "week" 键 -> -1（旧形状已随
 * 契约门 2026-09-21 废止，设备侧不得盲渲染旧列表）。
 */

#include "schedule_page.h"

#include <string.h>

#include "lvgl.h"

/* P0 [K4] 统一字体句柄（TaskPackage 261028-m6-k1-font-assets-runtime-loading）。
 * 课表正文 16px 字面源唯一消费口——本文件不另造资产加载/降级分支（F-1 / [F2]）。 */
#include "kidpal_font.h"

/* -------------------------------------------------------------------------- *
 * 共用小工具
 * ------------------------------------------------------------------------ */

/* safe-truncate 拷贝：恒 NUL 结尾、绝不溢出（frozen contract 要求）。
 * planner 扩展授权（260923-m3-k7 裁决，memcmp 确定化）：拷贝前零填充整块目标缓冲，
 * 令 name 超出 NUL 终止符的字节全部为 0——字符串消费面（strlen/strcmp/渲染取 NUL
 * 截断）行为完全中性，但使整 struct 的 memcmp（frozen K7-A7 保真齿）确定化，消除
 * 未初始化填充字节漂移。来自未初始化垃圾的填充在本改动前无法作确定性比对。 */
static void ksp_copy_str(char *dst, size_t cap, const char *src)
{
    size_t i;

    if (dst == NULL || cap == 0) {
        return;
    }
    memset(dst, 0, cap); /* 零填充目标缓冲：name 尾部字节确定化（memcmp 保真前提） */
    for (i = 0; src[i] != '\0' && i + 1 < cap; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
}

/* -------------------------------------------------------------------------- *
 * 自足式最小 JSON 扫描（无外部 JSON 库; frozen contract 要求）。
 * 只需覆盖 producer 下发的 data 对象形状 + skip 未知扩展值。
 * ------------------------------------------------------------------------ */

typedef struct {
    const char *s;
    size_t pos;
} ksp_json_cursor;

static void kspj_ws(ksp_json_cursor *c)
{
    while (c->s[c->pos] == ' ' || c->s[c->pos] == '\t' ||
           c->s[c->pos] == '\n' || c->s[c->pos] == '\r') {
        c->pos++;
    }
}

static char kspj_peek(const ksp_json_cursor *c)
{
    return c->s[c->pos];
}

static int kspj_hex4(ksp_json_cursor *c, unsigned *out)
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

static int kspj_utf8_encode(unsigned cp, char tmp[4])
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

static void kspj_put(char byte, char *out, size_t cap, size_t *used)
{
    if (out != NULL && *used + 1 < cap) {
        out[(*used)++] = byte;
    }
}

/* 解析 JSON 字符串到 out（cap 含 NUL 位; out 可为 NULL = 仅校验）。
 * 支持标准转义 + \uXXXX（含代理对 -> UTF-8）。成功 1 / 畸形 0。 */
static int kspj_string(ksp_json_cursor *c, char *out, size_t cap)
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

                if (!kspj_hex4(c, &cp)) {
                    return 0;
                }
                if (cp >= 0xD800u && cp <= 0xDBFFu) {
                    /* 代理对 -> 合成增补平面码点；孤立代理 = 畸形 */
                    if (c->s[c->pos] == '\\' && c->s[c->pos + 1] == 'u') {
                        unsigned lo;

                        c->pos += 2;
                        if (!kspj_hex4(c, &lo)) {
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
                    int n = kspj_utf8_encode(cp, tmp);

                    for (int i = 0; i < n; i++) {
                        kspj_put(tmp[i], out, cap, &used);
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
            kspj_put(plain, out, cap, &used);
            continue;
        }
        /* 普通 UTF-8 字节原样通过 */
        kspj_put((char)ch, out, cap, &used);
        c->pos++;
    }
    if (out != NULL) {
        out[used] = '\0';
    }
    return 1;
}

/* 跳过任意一个 JSON 值（string/object/array/number/true/false/null），
 * 兼做结构合法性校验。成功 1 / 畸形 0。 */
static int kspj_skip_value(ksp_json_cursor *c)
{
    kspj_ws(c);
    char ch = kspj_peek(c);

    if (ch == '"') {
        return kspj_string(c, NULL, 0);
    }
    if (ch == '{') {
        c->pos++;
        kspj_ws(c);
        if (kspj_peek(c) == '}') {
            c->pos++;
            return 1;
        }
        for (;;) {
            kspj_ws(c);
            if (!kspj_string(c, NULL, 0)) {
                return 0;
            }
            kspj_ws(c);
            if (kspj_peek(c) != ':') {
                return 0;
            }
            c->pos++;
            if (!kspj_skip_value(c)) {
                return 0;
            }
            kspj_ws(c);
            char d = kspj_peek(c);
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
        kspj_ws(c);
        if (kspj_peek(c) == ']') {
            c->pos++;
            return 1;
        }
        for (;;) {
            if (!kspj_skip_value(c)) {
                return 0;
            }
            kspj_ws(c);
            char d = kspj_peek(c);
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

/* 解析整数 JSON number（可选 '-' + 数字 >=1）。小数/指数 = 非整数数字 -> 0（畸形，
 * 契约要求 day/slot/flags 是 JSON 数字且逐字段整数保真）。成功 1 / 畸形 0。 */
static int kspj_int(ksp_json_cursor *c, int *out)
{
    int neg = 0;
    long v = 0;

    kspj_ws(c);
    if (kspj_peek(c) == '-') {
        neg = 1;
        c->pos++;
    }
    if (!(kspj_peek(c) >= '0' && kspj_peek(c) <= '9')) {
        return 0; /* 非数字（含字符串/true/null）-> 畸形 */
    }
    while (c->s[c->pos] >= '0' && c->s[c->pos] <= '9') {
        v = v * 10 + (c->s[c->pos] - '0');
        if (v > 1000000L) {
            return 0; /* 值域上游必拒, 防溢出 */
        }
        c->pos++;
    }
    {
        char ch = kspj_peek(c);

        if (ch == '.' || ch == 'e' || ch == 'E') {
            return 0; /* 小数/指数 = 非整数 -> 畸形 */
        }
    }
    *out = (int)(neg ? -v : v);
    return 1;
}

/* -------------------------------------------------------------------------- *
 * kidpal_schedule_grid_parse — 契约 §1.3② 周网格 data 解析（A4 device face）
 *
 * 形状 = {"week":{"courses":[{day,slot,name,flags},…]}};
 * day/slot/flags 必须是 JSON 数字且在 0..6 / 0..2 / 0..3 值域（越界一律 -1，
 * 绝不盲渲染, V27）; name 非缺失非空、按 KSG_NAME_CAP safe-truncate;
 * flags 缺省 0、未知扩展字段忽略（additive tolerance）; 尾部垃圾 = 畸形 -> -1。
 * ------------------------------------------------------------------------ */

#define KSG_KEY_BUF 16

/* 解析单条 course 条目（cursor 在 '{' 前）。成功 0 / 畸形 -1。 */
static int ksg_parse_course(ksp_json_cursor *arr, kidpal_schedule_course_t *out)
{
    int has_day = 0;
    int has_slot = 0;
    int has_name = 0;
    int day = 0;
    int slot = 0;
    int flags = 0;
    char nbuf[KSG_NAME_CAP];

    kspj_ws(arr);
    if (kspj_peek(arr) != '{') {
        return -1; /* 条目非对象 -> -1 */
    }
    arr->pos++;
    kspj_ws(arr);
    if (kspj_peek(arr) == '}') {
        return -1; /* 空 entry：缺 day/slot/name -> -1 */
    }
    for (;;) {
        char key[KSG_KEY_BUF];

        kspj_ws(arr);
        if (!kspj_string(arr, key, sizeof(key))) {
            return -1;
        }
        kspj_ws(arr);
        if (kspj_peek(arr) != ':') {
            return -1;
        }
        arr->pos++;
        kspj_ws(arr);

        if (strcmp(key, "day") == 0) {
            if (!kspj_int(arr, &day)) {
                return -1; /* 非数字 day -> -1 */
            }
            if (day < 0 || day > 6) {
                return -1; /* day 值域 0..6（day 0 基 周一=0） */
            }
            has_day = 1;
        } else if (strcmp(key, "slot") == 0) {
            if (!kspj_int(arr, &slot)) {
                return -1;
            }
            if (slot < 0 || slot > 2) {
                return -1; /* slot 值域 0..2 */
            }
            has_slot = 1;
        } else if (strcmp(key, "flags") == 0) {
            if (!kspj_int(arr, &flags)) {
                return -1; /* 非整数 flags（含字符串 "1"）-> -1（跨端 wire 是数字） */
            }
            if (flags < 0 || flags > (KSG_FLAG_TEMP | KSG_FLAG_STAR)) {
                return -1; /* flags 值域 0..3 */
            }
        } else if (strcmp(key, "name") == 0) {
            if (kspj_peek(arr) != '"') {
                return -1; /* 缺/非 string name -> -1 */
            }
            if (!kspj_string(arr, nbuf, sizeof(nbuf))) {
                return -1;
            }
            if (nbuf[0] == '\0') {
                return -1; /* name 非空字符串（空串 -> -1） */
            }
            has_name = 1;
        } else {
            if (!kspj_skip_value(arr)) {
                return -1; /* 未知字段值畸形 -> -1（否则忽略扩展位, additive） */
            }
        }

        kspj_ws(arr);
        char d = kspj_peek(arr);
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
    if (!has_day || !has_slot || !has_name) {
        return -1; /* 缺 day/slot/name —— 契约条目必带键 */
    }
    out->day = day;
    out->slot = slot;
    out->flags = flags;
    ksp_copy_str(out->name, sizeof(out->name), nbuf);
    return 0;
}

/* 解析 courses 数组（cursor 在 '['）。两遍式：先计数校验结构（cap 不足 -> -1,
 * 绝不溢出/静默截断），再逐条填充。返回条数 / 畸形 -1。 */
static int ksg_parse_courses_array(ksp_json_cursor *c,
                                   kidpal_schedule_course_t *courses, int cap)
{
    int count;

    if (kspj_peek(c) != '[') {
        return -1; /* courses 非数组 -> -1 */
    }
    /* pass 1: 计数（skip 校验每个元素结构） */
    {
        ksp_json_cursor scan = *c;

        scan.pos++;
        kspj_ws(&scan);
        if (kspj_peek(&scan) == ']') {
            scan.pos++;
            count = 0;
        } else {
            count = 0;
            for (;;) {
                if (!kspj_skip_value(&scan)) {
                    return -1;
                }
                count++;
                kspj_ws(&scan);
                char d = kspj_peek(&scan);
                if (d == ',') {
                    scan.pos++;
                    continue;
                }
                if (d == ']') {
                    scan.pos++;
                    break;
                }
                return -1;
            }
        }
        if (count > cap) {
            return -1; /* cap < 条数 -> -1（绝不溢出/静默截断） */
        }
    }
    /* pass 2: 逐条解析字段（条目分隔 ',' 与闭合 ']' 由本层统一消费） */
    {
        ksp_json_cursor arr = *c;

        arr.pos++;
        for (int i = 0; i < count; i++) {
            if (ksg_parse_course(&arr, &courses[i]) != 0) {
                return -1;
            }
            if (i + 1 < count) {
                kspj_ws(&arr);
                if (kspj_peek(&arr) != ',') {
                    return -1;
                }
                arr.pos++;
            }
        }
        kspj_ws(&arr);
        if (kspj_peek(&arr) != ']') {
            return -1;
        }
        arr.pos++;
        *c = arr; /* 主游标推进过数组 */
    }
    return count;
}

/* 解析 week 对象（cursor 在 '{' 前），返回 courses 条数 / 畸形 -1。
 * 未知扩展键（如 "tz"）忽略（additive tolerance）；缺 courses -> -1。 */
static int ksg_parse_week(ksp_json_cursor *c,
                          kidpal_schedule_course_t *courses, int cap)
{
    int count = -1;

    if (kspj_peek(c) != '{') {
        return -1;
    }
    c->pos++;
    kspj_ws(c);
    if (kspj_peek(c) == '}') {
        return -1; /* week 空对象 = 缺 courses -> -1 */
    }
    for (;;) {
        char key[KSG_KEY_BUF];

        kspj_ws(c);
        if (!kspj_string(c, key, sizeof(key))) {
            return -1;
        }
        kspj_ws(c);
        if (kspj_peek(c) != ':') {
            return -1;
        }
        c->pos++;
        kspj_ws(c);

        if (strcmp(key, "courses") == 0) {
            count = ksg_parse_courses_array(c, courses, cap);
            if (count < 0) {
                return -1;
            }
        } else {
            if (!kspj_skip_value(c)) {
                return -1;
            }
        }

        kspj_ws(c);
        char d = kspj_peek(c);
        if (d == ',') {
            c->pos++;
            continue;
        }
        if (d == '}') {
            c->pos++;
            return count;
        }
        return -1;
    }
}

int kidpal_schedule_grid_parse(const char *data_json,
                               kidpal_schedule_course_t *courses, int cap)
{
    ksp_json_cursor c = { data_json, 0 };
    int count = -1;
    int has_week = 0;

    if (data_json == NULL) {
        return -1; /* NULL 载荷 -> 显式降级（friendly degrade，绝不盲渲染, V27） */
    }

    kspj_ws(&c);
    if (kspj_peek(&c) != '{') {
        return -1;
    }
    c.pos++;
    kspj_ws(&c);
    if (kspj_peek(&c) == '}') {
        return -1; /* 薄载荷 data="{}"：缺 week -> -1（绝不盲渲染, V27） */
    }

    for (;;) {
        char key[KSG_KEY_BUF];

        kspj_ws(&c);
        if (!kspj_string(&c, key, sizeof(key))) {
            return -1;
        }
        kspj_ws(&c);
        if (kspj_peek(&c) != ':') {
            return -1;
        }
        c.pos++;
        kspj_ws(&c);

        if (strcmp(key, "week") == 0) {
            count = ksg_parse_week(&c, courses, cap);
            if (count < 0) {
                return -1;
            }
            has_week = 1;
        } else {
            /* 旧列表形状 {title,items} 无 week 键 -> has_week 保持 0 -> -1；
             * 未知顶层字段值畸形 -> -1（否则忽略扩展位） */
            if (!kspj_skip_value(&c)) {
                return -1;
            }
        }

        kspj_ws(&c);
        char d = kspj_peek(&c);
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

    kspj_ws(&c);
    if (c.s[c.pos] != '\0') {
        return -1; /* 尾部垃圾 = 畸形 JSON */
    }
    if (!has_week || count < 0) {
        return -1; /* 缺 week —— data 形状必带键 */
    }
    return count;
}

/* -------------------------------------------------------------------------- *
 * kidpal_schedule_grid_render — P2 一周总览网格渲染面 (纯 LVGL, WF-F1-02 版式区钉)
 *
 * minimal: 版式常量按冻结验收文件头区钉定坐标（400x300 减键帽条 chrome）;
 * 真机 1bpp 目测/翻页手感 deferred 归 M3-K3 设备轮（M3K2-A7 挂账）。
 * ------------------------------------------------------------------------ */

/* V5 对齐 (ui-v5-align §2 P2): 设计全文纯 1bpp 黑白——旧灰阶 0x222222/0x999999 在
 * RLCD 阈值化后层次丢失且与设计稿黑白口径不符, 收敛为纯黑墨。 */
#define KSG_COLOR_TEXT 0x000000u
#define KSG_COLOR_BG   0xFFFFFFu
#define KSG_COLOR_SEP  0x000000u
#define KSG_FB_W       400
#define KSG_FB_H       300
#define KSG_ROW_PITCH  19 /* 16px 版式格内课程名行 pitch（F-3/P2S-E1；>= KSG_FONT_MIN_PX(14) 下限） */

/* 表头带 y[0..29]; 槽位行带 slot0 y[30..107] / slot1 y[108..185] / slot2 y[186..254]
 * （带高 78/78/69；F-3/P2S-E1 16px 版式重基线, 冻结验收文件头版式区钉, 勿偏移）;
 * slot2 下缘贴 KSG_GRID_BOTTOM=254; y>=255 网格外带零墨。 */
static const int ksg_slot_ys[3][2] = { { 30, 107 }, { 108, 185 }, { 186, 254 } };

/* 渲染层按槽位可画上限（F-3/OQ4/P2S-E2：上午 4 / 下午 4 / 晚上 3）。超限静默
 * 截断（不滚动、不画超限行、不新增提示）——缓存/解析 cap 仍是 KSG_COURSE_CAP=84，
 * 本表只约束"每一格画几门"，绝不参与解析或缓存容量（F-8 铁律）。 */
static const int ksg_slot_caps[3] = { 4, 4, 3 };

/* CJK 文本字体 = P0 统一句柄 [K4]：挂载过 assets 时为 16px body；缺 assets 时
 * kidpal_font.cc [K2]/[K3] 内部落 font_puhui_14_1 14px 兜底（恒非 NULL——此处
 * 不新增 NULL 回落分支, P0 FAS-A4/A7 语义）。 */
static const lv_font_t *ksg_text_font(void)
{
    return kidpal_font_body();
}

/* ★ 星标（flags bit1，0x2605）字形探测——host 字体可能缺该字形，缺则走程序化
 * 兜底画法（纪律同 home_p1 描边兜底）。「标记可见」本身即验收面（M3K2-A3），兜底
 * 画法与真字形在真帧上都落在课程名所在格带内。临时课标记（bit0）已改课名下 2px
 * 程序化下划线，不经字形探测、无字体依赖。 */
static int ksg_glyph_available(uint32_t cp)
{
    const lv_font_t *f = ksg_text_font();
    lv_font_glyph_dsc_t dsc;

    if (f == NULL) {
        return 0;
    }
    return lv_font_get_glyph_dsc(f, &dsc, cp, 0) ? 1 : 0;
}

static lv_obj_t *ksg_rect_obj(lv_obj_t *parent, int x, int y, int w, int h,
                              uint32_t color)
{
    lv_obj_t *r = lv_obj_create(parent);

    lv_obj_remove_style_all(r);
    lv_obj_set_pos(r, (lv_coord_t)x, (lv_coord_t)y);
    lv_obj_set_size(r, (lv_coord_t)w, (lv_coord_t)h);
    lv_obj_set_style_bg_color(r, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(r, LV_OPA_COVER, 0);
    return r;
}

static lv_obj_t *ksg_label(lv_obj_t *parent, const char *txt, int x, int y)
{
    lv_obj_t *l = lv_label_create(parent);

    lv_label_set_text(l, txt);
    lv_obj_set_pos(l, (lv_coord_t)x, (lv_coord_t)y);
    lv_obj_set_style_text_color(l, lv_color_hex(KSG_COLOR_TEXT), 0);
    return l;
}

/* 程序化 ★ 兜底（实心菱形, 10x10）。 */
static void ksg_marker_star_proc(lv_obj_t *scr, int x, int y)
{
    ksg_rect_obj(scr, x + 4, y, 2, 2, KSG_COLOR_TEXT);
    ksg_rect_obj(scr, x + 2, y + 2, 6, 2, KSG_COLOR_TEXT);
    ksg_rect_obj(scr, x, y + 4, 10, 2, KSG_COLOR_TEXT);
    ksg_rect_obj(scr, x + 2, y + 6, 6, 2, KSG_COLOR_TEXT);
    ksg_rect_obj(scr, x + 4, y + 8, 2, 2, KSG_COLOR_TEXT);
}

/* 画一枚标记：字形可用 -> 真字形 label；缺字形 -> 程序化兜底。 */
static void ksg_draw_marker(lv_obj_t *scr, int x, int y, uint32_t cp,
                            void (*fallback)(lv_obj_t *, int, int))
{
    if (ksg_glyph_available(cp)) {
        char utf8[5];

        utf8[0] = (char)(0xE0u | (cp >> 12));
        utf8[1] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
        utf8[2] = (char)(0x80u | (cp & 0x3Fu));
        utf8[3] = '\0';
        (void)ksg_label(scr, utf8, x, y);
    } else {
        fallback(scr, x, y + 3);
    }
}

/* UTF-8 解一码点（畸形返回 0 表示停止；name 由解析面保证合法 UTF-8）。 */
static uint32_t ksg_utf8_next(const char **pp)
{
    const unsigned char *p = (const unsigned char *)*pp;
    uint32_t cp;
    int seq, i;

    if (p[0] == 0) {
        return 0;
    }
    if (p[0] < 0x80u) {
        *pp += 1;
        return p[0];
    }
    if ((p[0] & 0xE0u) == 0xC0u) {
        cp = (uint32_t)(p[0] & 0x1Fu);
        seq = 2;
    } else if ((p[0] & 0xF0u) == 0xE0u) {
        cp = (uint32_t)(p[0] & 0x0Fu);
        seq = 3;
    } else if ((p[0] & 0xF8u) == 0xF0u) {
        cp = (uint32_t)(p[0] & 0x07u);
        seq = 4;
    } else {
        *pp += 1;
        return 0;
    }
    for (i = 1; i < seq; i++) {
        if ((p[i] & 0xC0u) != 0x80u) {
            *pp += 1;
            return 0;
        }
        cp = (cp << 6) | (uint32_t)(p[i] & 0x3Fu);
    }
    *pp += seq;
    return cp;
}

/* [F2-M] 临时课标记（flags bit0）= 课名下 2px 程序化横线。
 *
 * 几何取自 LVGL 字形落位公式（lv_draw_label.c lv_draw_unit_draw_letter 同源）：
 *   ink_top(row) = row_y + (line_height - base_line) - box_h - ofs_y
 *   ink_bottom(row) = ink_top + box_h - 1
 * 逐字形取名字墨 x 跨度（含端点）与墨底缘最大值 → 在墨底缘下方相邻行起画 2px
 * 横线（宽 = 名墨 x 跨度），clip 进行带（band_y0..band_y1）与格带（cell_x0..cell_x1）。
 * 名不扣任何前缀宽（※ 已退役, 名从格左缘起笔）。无字体字形依赖、无随机性。 */
static void ksg_draw_temp_underline(lv_obj_t *scr, const char *name,
                                    int name_x, int row_y,
                                    int cell_x0, int cell_x1,
                                    int band_y0, int band_y1)
{
    const lv_font_t *f = ksg_text_font();
    const char *p = name;
    int pen_x = name_x;
    int ink_left = -1;
    int ink_right = -1;
    int ink_bottom = -1;
    int ux0, ux1, uy0, uy1;

    if (f == NULL || name == NULL) {
        return;
    }
    for (;;) {
        uint32_t cp = ksg_utf8_next(&p);
        lv_font_glyph_dsc_t g;

        if (cp == 0) {
            break;
        }
        if (!lv_font_get_glyph_dsc(f, &g, cp, 0) || g.box_w == 0 || g.box_h == 0) {
            continue;
        }
        if (ink_left < 0 || pen_x + (int)g.ofs_x < ink_left) {
            ink_left = pen_x + (int)g.ofs_x;
        }
        if (pen_x + (int)g.ofs_x + (int)g.box_w - 1 > ink_right) {
            ink_right = pen_x + (int)g.ofs_x + (int)g.box_w - 1;
        }
        {
            int bottom = (int)f->line_height - (int)f->base_line - (int)g.ofs_y - 1;

            if (bottom > ink_bottom) {
                ink_bottom = bottom;
            }
        }
        pen_x += (int)g.adv_w;
    }
    if (ink_left < 0 || ink_right < ink_left || ink_bottom < 0) {
        return;
    }

    ux0 = ink_left;
    ux1 = ink_right;
    uy0 = row_y + ink_bottom + 1; /* 名墨底缘下方相邻行起 */
    uy1 = uy0 + 1;                /* 2px */

    if (ux0 < cell_x0) {
        ux0 = cell_x0;
    }
    if (ux1 > cell_x1) {
        ux1 = cell_x1;
    }
    if (uy0 < band_y0) {
        uy0 = band_y0;
    }
    if (uy1 > band_y1) {
        uy1 = band_y1;
    }
    if (ux1 < ux0 || uy1 < uy0) {
        return;
    }
    (void)ksg_rect_obj(scr, ux0, uy0, ux1 - ux0 + 1, uy1 - uy0 + 1, KSG_COLOR_TEXT);
}

/* (day,slot) 格内一行：[★?] 课程名（★ 10px + 2px 间隙; 名按格宽截断, 不做格内
 * 滚动——格内溢出走截断降级, spec S-F1.2 补充规则口径）。临时课（flags bit0）
 * 在课名下方画 2px 下划线（[F2-M]），※ 前缀形态已退役。 */
static void ksg_cell_row(lv_obj_t *scr, const kidpal_schedule_course_t *co,
                         int cell_x0, int cell_x1, int row_y,
                         int band_y0, int band_y1)
{
    int x = cell_x0 + 2;

    if (co->flags & KSG_FLAG_STAR) {
        ksg_draw_marker(scr, x, row_y, 0x2605u, ksg_marker_star_proc);
        x += 12;
    }
    if (x < cell_x1 - 1) {
        lv_obj_t *nm = ksg_label(scr, co->name, x, row_y);

        lv_obj_set_width(nm, (lv_coord_t)(cell_x1 - 1 - x));
        lv_obj_set_height(nm, KSG_ROW_PITCH);
        lv_label_set_long_mode(nm, LV_LABEL_LONG_CLIP);

        if (co->flags & KSG_FLAG_TEMP) {
            ksg_draw_temp_underline(scr, co->name, x, row_y,
                                    cell_x0, cell_x1, band_y0, band_y1);
        }
    }
}

int kidpal_schedule_grid_render(const kidpal_schedule_course_t *courses, int n,
                                int today_col)
{
    lv_obj_t *scr;
    int rows[7][3] = { { 0 } };

    /* 参数校验先行：非法参数不动屏、不崩（frozen contract）。 */
    if (n < 0 || (n > 0 && courses == NULL)) {
        return -1;
    }
    scr = lv_screen_active();
    if (scr == NULL) {
        return -1; /* LVGL 未初始化（host 缝未置 rendered 态等）-> 不崩 */
    }

    /* 幂等清屏后重画（可反复调用）。全屏白底：y>=252 键帽条带零墨的前提是整屏
     * 铺白，网格家具/课程墨全部落在 y<=KSG_GRID_BOTTOM。 */
    lv_obj_clean(scr);
    lv_obj_set_style_pad_all(scr, 0, 0);
    (void)ksg_rect_obj(scr, 0, 0, KSG_FB_W, KSG_FB_H, KSG_COLOR_BG);

    /* CJK 字体挂到屏（子对象继承; host 真字形经 lv_font_get_glyph_dsc 探测） */
    if (ksg_text_font() != NULL) {
        lv_obj_set_style_text_font(scr, ksg_text_font(), 0);
    }

    /* 家具：表格线 + 表头 + 槽位标签（与课程/今天列无关 -> 帧 diff 相消, 探针
     * 断言只锁差集像素落带）。页内无标题无提示行（v1.20 已删, 操作提示归全局
     * 键帽条）。
     * V5 对齐 (ui-v5-align §2 P2 行 4/5/6): 表线细黑 1px; 外框补全（顶边 y0 +
     * 最左边 x0, 右边收 1px 齐格）; 行标签改槽位带垂直居中。 */
    for (int k = 0; k < 7; k++) {
        ksg_rect_obj(scr, 40 + 51 * k, 0, 1, KSG_GRID_BOTTOM + 1, KSG_COLOR_SEP);
    }
    ksg_rect_obj(scr, 397, 0, 1, KSG_GRID_BOTTOM + 1, KSG_COLOR_SEP);
    ksg_rect_obj(scr, 0, 0, 398, 1, KSG_COLOR_SEP);
    ksg_rect_obj(scr, 0, 0, 1, KSG_GRID_BOTTOM + 1, KSG_COLOR_SEP);
    ksg_rect_obj(scr, 0, 29, 398, 1, KSG_COLOR_SEP);
    ksg_rect_obj(scr, 0, ksg_slot_ys[0][1], 398, 1, KSG_COLOR_SEP);
    ksg_rect_obj(scr, 0, ksg_slot_ys[1][1], 398, 1, KSG_COLOR_SEP);
    ksg_rect_obj(scr, 0, KSG_GRID_BOTTOM, 398, 1, KSG_COLOR_SEP);

    {
        static const char days[7][7] = { "周一", "周二", "周三", "周四",
                                         "周五", "周六", "周日" };
        static const char slots[3][7] = { "上午", "下午", "晚上" };
        int line_h = (ksg_text_font() != NULL) ? (int)ksg_text_font()->line_height : 20;

        for (int d = 0; d < 7; d++) {
            (void)ksg_label(scr, days[d], 40 + 51 * d + 4, 6);
        }
        /* 行标签垂直居中（带高 78/78/69；行高随 16px 句柄实测, 观感面不钉）。 */
        for (int s = 0; s < 3; s++) {
            int band_h = ksg_slot_ys[s][1] - ksg_slot_ys[s][0] + 1;
            int off = (band_h - line_h) / 2;

            if (off < 0) {
                off = 0;
            }
            (void)ksg_label(scr, slots[s], 4, ksg_slot_ys[s][0] + off);
        }
    }

    /* 今天列整列高亮：0..6 合法 -> 该 day 列带整列高亮（边框式, 与未高亮列可
     * 区分, 墨不越带）；越域（含 -1 = 未注入星期）-> 无任何列高亮（不画假「今天」,
     * 兜底纪律同 home_p1 日期未知不画假日期）。 */
    if (today_col >= 0 && today_col <= 6) {
        lv_obj_t *hl = lv_obj_create(scr);

        lv_obj_remove_style_all(hl);
        lv_obj_set_pos(hl, (lv_coord_t)(40 + 51 * today_col), 0);
        lv_obj_set_size(hl, 51, KSG_GRID_BOTTOM + 1);
        lv_obj_set_style_border_width(hl, 2, 0);
        lv_obj_set_style_border_color(hl, lv_color_hex(KSG_COLOR_TEXT), 0);
        lv_obj_set_style_border_opa(hl, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_opa(hl, LV_OPA_TRANSP, 0);
    }

    /* 课程逐门竖排：同 (day,slot) 格内自上而下堆叠（同格多条目保持 producer
     * 下发序）; 行 pitch KSG_ROW_PITCH（16px 版式）= 19; 每格按槽位上限 4/4/3
     * 静默截断（超限不画、不滚动、不提示, F-3/OQ4）。 */
    for (int i = 0; i < n; i++) {
        int d = courses[i].day;
        int s = courses[i].slot;
        int r = rows[d][s];

        if (r >= ksg_slot_caps[s]) {
            continue; /* 静默截断：本槽位超限行不画（缓存/解析 cap 84 不受影响） */
        }
        rows[d][s]++;
        ksg_cell_row(scr, &courses[i], 40 + 51 * d, 90 + 51 * d,
                     ksg_slot_ys[s][0] + 2 + r * KSG_ROW_PITCH,
                     ksg_slot_ys[s][0], ksg_slot_ys[s][1]);
    }

    return n;
}
