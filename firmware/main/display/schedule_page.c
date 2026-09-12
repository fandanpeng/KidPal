/*
 * schedule_page.c — KidPal P2 课表页消费面：五个 FROZEN OFFLINE CONTRACT 的
 * 实现 (TaskPackage 260829-schedule-p2, 契约 §1.3 ② / S-F1.2 / S-F1.3)。
 *
 * 语义与形状以冻结验收 firmware/main/test/schedule_p2.acceptance.c 文件头为准，
 * 不得偏移；强定义在 host runner 里覆盖验收文件内的 weak 哨兵 (-2) 后被真测
 * （同 view_router.c 先例）。
 *
 * 纯逻辑 + 纯 LVGL（lv_obj_/lv_label/lv_anim only，无 Board/Display class 依赖）
 * -> host_app linux target 与设备树两侧同源可编。解析器为自足式最小 JSON 扫描
 * （无外部 JSON 库依赖，frozen contract 要求）。
 *
 * 降级语义：解析畸形/缺字段/越界 cap 一律 -1 —— 调用方必须友好降级、绝不盲渲染
 * (V27 / spec §4.0)。
 */

#include "schedule_page.h"

#include "art_assets.h"
#include <string.h>

#include "lvgl.h"

/* -------------------------------------------------------------------------- *
 * 共用小工具
 * ------------------------------------------------------------------------ */

/* safe-truncate 拷贝：恒 NUL 结尾、绝不溢出（frozen contract 要求）。 */
static void ksp_copy_str(char *dst, size_t cap, const char *src)
{
    size_t i;

    if (dst == NULL || cap == 0) {
        return;
    }
    for (i = 0; src[i] != '\0' && i + 1 < cap; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
}

/* "HH:MM" 前缀解析（前导 HH:MM 决定，区间形式 "HH:MM-HH:MM" 天然兼容）。
 * 非法（位数/分隔/越界 23:59）-> -1。 */
static int ksp_parse_hhmm(const char *s, int *hh, int *mm)
{
    int h, m;

    if (s == NULL) {
        return -1;
    }
    if (s[0] < '0' || s[0] > '9' || s[1] < '0' || s[1] > '9' || s[2] != ':' ||
        s[3] < '0' || s[3] > '9' || s[4] < '0' || s[4] > '9') {
        return -1;
    }
    h = (s[0] - '0') * 10 + (s[1] - '0');
    m = (s[3] - '0') * 10 + (s[4] - '0');
    if (h > 23 || m > 59) {
        return -1;
    }
    *hh = h;
    *mm = m;
    return 0;
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

/* -------------------------------------------------------------------------- *
 * kidpal_schedule_parse — P2-A2 device face
 * ------------------------------------------------------------------------ */

#define KSP_TITLE_BUF 128
#define KSP_KEY_BUF   32

int kidpal_schedule_parse(const char *data_json, char *title, size_t title_cap,
                          kidpal_schedule_item_t *items, int cap)
{
    ksp_json_cursor c = { data_json, 0 };
    char title_buf[KSP_TITLE_BUF];
    int has_title = 0;
    int item_count = -1;

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
        return -1; /* {} 缺 title 与 items */
    }

    for (;;) {
        char key[KSP_KEY_BUF];

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

        if (strcmp(key, "title") == 0) {
            if (kspj_peek(&c) != '"') {
                return -1; /* 非 string title -> -1（title 是契约必带字段） */
            }
            if (!kspj_string(&c, title_buf, sizeof(title_buf))) {
                return -1;
            }
            has_title = 1;
        } else if (strcmp(key, "items") == 0) {
            if (kspj_peek(&c) != '[') {
                return -1; /* 非 array items -> -1 */
            }

            /* pass 1: 计数（skip 校验每个元素结构），cap 不足 -> -1（绝不溢出/
             * 静默截断，调用方必须按数扩容）。 */
            {
                ksp_json_cursor scan = c;
                int count = 0;

                scan.pos++;
                kspj_ws(&scan);
                if (kspj_peek(&scan) == ']') {
                    scan.pos++;
                } else {
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
                if (cap < count) {
                    return -1;
                }
                item_count = count;
            }

            /* pass 2: 逐元素解析字段（time/name 必带 string、location 缺省 ""、
             * 未知扩展字段忽略）。 */
            {
                ksp_json_cursor arr = c;

                arr.pos++;
                for (int i = 0; i < item_count; i++) {
                    int has_time = 0;
                    int has_name = 0;
                    char tbuf[16];
                    char nbuf[48];
                    char lbuf[48];

                    lbuf[0] = '\0'; /* 缺 location 解析为 ""（additive tolerance） */
                    kspj_ws(&arr);
                    if (kspj_peek(&arr) != '{') {
                        return -1; /* item 非对象 -> -1 */
                    }
                    arr.pos++;
                    kspj_ws(&arr);
                    if (kspj_peek(&arr) == '}') {
                        return -1; /* 空 item：缺 time/name -> -1 */
                    }
                    for (;;) {
                        char ikey[KSP_KEY_BUF];

                        kspj_ws(&arr);
                        if (!kspj_string(&arr, ikey, sizeof(ikey))) {
                            return -1;
                        }
                        kspj_ws(&arr);
                        if (kspj_peek(&arr) != ':') {
                            return -1;
                        }
                        arr.pos++;
                        kspj_ws(&arr);
                        if (strcmp(ikey, "time") == 0) {
                            if (kspj_peek(&arr) != '"') {
                                return -1; /* 缺/非 string time -> -1 */
                            }
                            if (!kspj_string(&arr, tbuf, sizeof(tbuf))) {
                                return -1;
                            }
                            has_time = 1;
                        } else if (strcmp(ikey, "name") == 0) {
                            if (kspj_peek(&arr) != '"') {
                                return -1; /* 缺/非 string name -> -1 */
                            }
                            if (!kspj_string(&arr, nbuf, sizeof(nbuf))) {
                                return -1;
                            }
                            has_name = 1;
                        } else if (strcmp(ikey, "location") == 0) {
                            if (kspj_peek(&arr) != '"') {
                                return -1;
                            }
                            if (!kspj_string(&arr, lbuf, sizeof(lbuf))) {
                                return -1;
                            }
                        } else {
                            if (!kspj_skip_value(&arr)) {
                                return -1;
                            }
                        }
                        kspj_ws(&arr);
                        char d = kspj_peek(&arr);
                        if (d == ',') {
                            arr.pos++;
                            continue;
                        }
                        if (d == '}') {
                            arr.pos++;
                            break;
                        }
                        return -1;
                    }
                    if (!has_time || !has_name) {
                        return -1;
                    }
                    /* tbuf/nbuf/lbuf 容量与目标字段逐一相等，safe 拷贝 */
                    ksp_copy_str(items[i].time, sizeof(items[i].time), tbuf);
                    ksp_copy_str(items[i].name, sizeof(items[i].name), nbuf);
                    ksp_copy_str(items[i].location, sizeof(items[i].location), lbuf);

                    kspj_ws(&arr);
                    char d = kspj_peek(&arr);
                    if (i + 1 < item_count) {
                        if (d != ',') {
                            return -1;
                        }
                        arr.pos++;
                    } else {
                        if (d != ']') {
                            return -1;
                        }
                        arr.pos++;
                    }
                }
                if (item_count == 0) {
                    /* 空数组（空课表合法页）：pass 2 循环未跑，须补消费 ']' */
                    kspj_ws(&arr);
                    if (kspj_peek(&arr) != ']') {
                        return -1;
                    }
                    arr.pos++;
                }
                c = arr; /* 主游标推进过数组 */
            }
        } else {
            if (!kspj_skip_value(&c)) {
                return -1; /* 未知顶层字段值畸形 -> -1（否则忽略扩展位） */
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
    if (!has_title || item_count < 0) {
        return -1; /* 缺 title / 缺 items —— data 形状必带字段 */
    }
    if (title != NULL) {
        ksp_copy_str(title, title_cap, title_buf);
    }
    return item_count;
}

/* -------------------------------------------------------------------------- *
 * kidpal_schedule_dot_state — P2-A3 圆点两态 (S-F1.2)
 * ------------------------------------------------------------------------ */

int kidpal_schedule_dot_state(const char *start_hhmm, const char *now_hhmm,
                              int is_tomorrow)
{
    int sh, sm, nh, nm;

    if (start_hhmm == NULL) {
        return -1; /* 防御：无法判定 */
    }
    if (is_tomorrow != 0) {
        return 0; /* 明天全空心：now 无关，NULL 亦 0 (S-F1.2 切明天全空心) */
    }
    if (now_hhmm == NULL) {
        return -1; /* 今天视图无 now 无法判定 */
    }
    if (ksp_parse_hhmm(start_hhmm, &sh, &sm) != 0 ||
        ksp_parse_hhmm(now_hhmm, &nh, &nm) != 0) {
        return -1; /* 时刻不可解析 -> 防御降级 */
    }
    /* start <= now -> 已开始/已上完（空心）；start > now -> 待上（实心） */
    return (sh * 60 + sm) <= (nh * 60 + nm) ? 0 : 1;
}

/* -------------------------------------------------------------------------- *
 * kidpal_schedule_page_input — P2-A4 页内输入特例 (spec §4.0)
 * ------------------------------------------------------------------------ */

ksp_day_t kidpal_schedule_page_input(key_id_t key, key_gesture_t g,
                                     ksp_day_t current)
{
    /* 只消费全局层留下的 BOOT LONG_PRESS (GLOBAL_ACT_NONE, 包 3 冻结路由)；
     * 其余 key/gesture 组合一律不吞（KEY 长按回首页 / BOOT 单击翻页 /
     * KEY 单击双击 / 越界键位 —— 全局手势「所有页面一致」不被页内破坏）。 */
    if (key == KEY_ID_BOOT && g == KEY_GESTURE_LONG_PRESS) {
        return (current == KSP_DAY_TODAY) ? KSP_DAY_TOMORROW : KSP_DAY_TODAY;
    }
    return current;
}

/* -------------------------------------------------------------------------- *
 * kidpal_schedule_locate — P2-A5 滚动定位 (S-F1.3)
 * ------------------------------------------------------------------------ */

int kidpal_schedule_locate(const kidpal_schedule_item_t *items, int n,
                           const char *time, const char *name)
{
    const char *t;

    if (items == NULL || n <= 0 || name == NULL) {
        return -1;
    }
    t = (time == NULL) ? "" : time; /* NULL time 按仅 name 通配宽容处理 */
    for (int i = 0; i < n; i++) {
        int time_ok = (t[0] == '\0') || (strcmp(items[i].time, t) == 0);
        int name_ok = (strcmp(items[i].name, name) == 0);

        if (time_ok && name_ok) {
            return i;
        }
    }
    return -1; /* 未命中（精确匹配，无子串模糊） */
}

/* -------------------------------------------------------------------------- *
 * kidpal_schedule_page_render — P2-A3/A4/A5 渲染面 (纯 LVGL)
 *
 * minimal: 布局常量按 host seam 400x300 定坐标（列表纵向排布），真实屏幕观感
 * 殿后 9b（P2-E2 口径）；行数超屏不滚动（P2 只承担「看安排」）。
 * ------------------------------------------------------------------------ */

#define KSP_ROW_Y0       40
#define KSP_ROW_H        40
#define KSP_DOT_SIZE     38
#define KSP_DOT_BORDER   8
#define KSP_DOT_X        6
#define KSP_COL_TIME_X   52
#define KSP_COL_NAME_X   150
#define KSP_COL_LOC_X    262
#define KSP_STAR_X       356
#define KSP_COLOR_TEXT   0x222222u
#define KSP_COLOR_DOT    0x333333u
#define KSP_COLOR_BG     0xFFFFFFu
#define KSP_COLOR_SEP    0x888888u

static void ksp_title_bounce_anim_cb(void *var, int32_t v)
{
    lv_obj_set_y((lv_obj_t *)var, v);
}

/*
 * 换皮消费接线 (包 9b HWV-A6)：把 U 单一资产层的吉祥物姿态经 loader 原语
 * (kidpal_art_load + kidpal_art_pixel) 合成为一枚 LVGL lv_image (ARGB8888：
 * 墨=不透明黑 0xFF000000 / 纸及非阈值化=透明 0x00000000)，叠加在课表右下空白区
 * (spec §5.5「每页至少露一面」)。选 load+pixel→lv_image 而非整帧 render/draw 缝：
 * 本页正文是活 LVGL 控件，整帧替换会与文字互相覆盖；lv_image 经既有 LVGL->面板
 * flush 即刻真机可见。落点避开圆点两态 (KSP_DOT_X=6)、分隔线 (y<=118) 与列文本
 * (y<=90) —— 对 P2-A3 的 diff/暗色/非均匀采样断言零干扰（姿态只增暗色、且今/明
 * 两帧恒同不改变圆点差异）。未 mount / 加载失败 / malloc 失败 -> 静默跳过，页面
 * 回既有版面 (best-effort，绝不 panic)。姿态常量，故 lv_malloc 一次并缓存复用。
 */
#define KSP_POSE_MAX 96
static lv_image_dsc_t s_ksp_pose_dsc;
static uint32_t      *s_ksp_pose_px  = NULL;
static int            s_ksp_pose_st  = 0; /* 0=未试 1=可用 2=不可用 */

static int ksp_pose_image_ready(void)
{
    kidpal_art_img_t img;

    if (s_ksp_pose_st == 2) {
        return 0;
    }
    if (s_ksp_pose_st == 1) {
        return 1;
    }
    if (kidpal_art_load("pose_daily_96.bin", &img) != 0 ||
        img.w <= 0 || img.h <= 0 || img.w > KSP_POSE_MAX || img.h > KSP_POSE_MAX) {
        s_ksp_pose_st = 2;
        return 0;
    }
    s_ksp_pose_px = (uint32_t *)lv_malloc((size_t)img.w * img.h * sizeof(uint32_t));
    if (s_ksp_pose_px == NULL) {
        s_ksp_pose_st = 2;
        return 0;
    }
    for (int y = 0; y < img.h; y++) {
        for (int x = 0; x < img.w; x++) {
            int p = kidpal_art_pixel(&img, x, y);
            s_ksp_pose_px[y * img.w + x] = (p == 1) ? 0xFF000000u : 0x00000000u;
        }
    }
    memset(&s_ksp_pose_dsc, 0, sizeof(s_ksp_pose_dsc));
    s_ksp_pose_dsc.header.cf     = LV_COLOR_FORMAT_ARGB8888;
    s_ksp_pose_dsc.header.w      = img.w;
    s_ksp_pose_dsc.header.h      = img.h;
    s_ksp_pose_dsc.header.stride = (uint32_t)img.w * 4u;
    s_ksp_pose_dsc.data          = (const uint8_t *)s_ksp_pose_px;
    s_ksp_pose_dsc.data_size     = (uint32_t)(img.w * img.h * 4);
    s_ksp_pose_st = 1;
    return 1;
}

/* -------------------------------------------------------------------------- *
 * 换皮网点弱底纹 (包 260911-render-pages RP-A8)：把三档网点之一 d50 (4x4, 单元 4px)
 * 合成为 ARGB8888 tile, 经 bg_image_tiled 在 `bg` 上平铺成课表页弱底纹。四类用途
 * 白名单「弱底纹」, 单屏面积 4px 格稀疏 (V3 §07)。未 mount/失败 -> NULL (best-effort)。
 * ------------------------------------------------------------------------ */
#define KSP_DOTS_MAX 16
static lv_image_dsc_t s_ksp_dots_dsc;
static uint32_t      *s_ksp_dots_px  = NULL;
static int            s_ksp_dots_st  = 0; /* 0=未试 1=可用 2=不可用 */

static const lv_image_dsc_t *ksp_dots_dsc(void)
{
    kidpal_art_img_t img;

    if (s_ksp_dots_st == 2) {
        return NULL;
    }
    if (s_ksp_dots_st == 1) {
        return &s_ksp_dots_dsc;
    }
    if (kidpal_art_load("d50.bin", &img) != 0 ||
        img.w <= 0 || img.h <= 0 || img.w > KSP_DOTS_MAX || img.h > KSP_DOTS_MAX) {
        s_ksp_dots_st = 2;
        return NULL;
    }
    s_ksp_dots_px = (uint32_t *)lv_malloc((size_t)img.w * img.h * sizeof(uint32_t));
    if (s_ksp_dots_px == NULL) {
        s_ksp_dots_st = 2;
        return NULL;
    }
    for (int y = 0; y < img.h; y++) {
        for (int x = 0; x < img.w; x++) {
            int p = kidpal_art_pixel(&img, x, y);
            s_ksp_dots_px[y * img.w + x] = (p == 1) ? 0xFF000000u : 0x00000000u;
        }
    }
    memset(&s_ksp_dots_dsc, 0, sizeof(s_ksp_dots_dsc));
    s_ksp_dots_dsc.header.cf     = LV_COLOR_FORMAT_ARGB8888;
    s_ksp_dots_dsc.header.w      = img.w;
    s_ksp_dots_dsc.header.h      = img.h;
    s_ksp_dots_dsc.header.stride = (uint32_t)img.w * 4u;
    s_ksp_dots_dsc.data          = (const uint8_t *)s_ksp_dots_px;
    s_ksp_dots_dsc.data_size     = (uint32_t)(img.w * img.h * 4);
    s_ksp_dots_st = 1;
    return &s_ksp_dots_dsc;
}


int kidpal_schedule_page_render(const char *title,
                                const kidpal_schedule_item_t *items, int n,
                                int is_tomorrow, const char *now_hhmm,
                                int locate_row)
{
    /* 参数校验先行：非法参数不动屏、不崩（frozen contract）。 */
    if (title == NULL || n < 0 || (items == NULL && n > 0)) {
        return -1;
    }

    lv_obj_t *scr = lv_screen_active();
    if (scr == NULL) {
        return -1; /* LVGL 未初始化（host 缝未置 rendered 态等）-> 不崩 */
    }

    /* 幂等清屏后重画（可反复调用） */
    lv_obj_clean(scr);
    lv_obj_set_style_pad_all(scr, 0, 0);

    lv_obj_t *bg = lv_obj_create(scr);
    lv_obj_remove_style_all(bg);
    lv_obj_set_size(bg, LV_PCT(100), LV_PCT(100));
    lv_obj_set_pos(bg, 0, 0);
    lv_obj_set_style_bg_color(bg, lv_color_hex(KSP_COLOR_BG), 0);
    lv_obj_set_style_bg_opa(bg, LV_OPA_COVER, 0);
    lv_obj_clear_flag(bg, LV_OBJ_FLAG_SCROLLABLE);

    /* 换皮 (包 260911-render-pages RP-A8): 手绘框族 frm_ (frm_236_236 / frm_sm_160_120 /
     * frm_flat_240_96 9 宫格) 铺开 + d50 网点弱底纹。LVGL 骨架与动态文本语义保留
     * (冻结齿不锁布局, 真实观感归 RP-A5 真机轮)。d50 平铺为弱底纹, 姿态在右下叠加。
     *
     * V3 字体接线 (RP-A4/S0.1, S0.1 决策件 font-decision-s01.md): 正文/标题/数字三
     * 字体经 SPIFFS 非 KAP1 独立落地 fonts_root (冻结路径): font_body.bin (GB2312
     * 全集正文) / font_title_zcool.bin (站酷快乐体标题) / font_num_baloo.bin (Baloo 2
     * 数字) —— 由设备字体加载器 lv_font_load 从 fonts_root 取, 标题→font_title_zcool,
     * 时间→font_num_baloo, 正文→font_body; V3 字号阶梯 (30 标题 / 44 时间) 落位。
     * host runner 无 SPIFFS 字体挂载, 该加载在设备真机轮 (RP-A5) 验证。 */
    {
        const lv_image_dsc_t *dots = ksp_dots_dsc();
        if (dots != NULL) {
            lv_obj_set_style_bg_image_src(bg, dots, 0);
            lv_obj_set_style_bg_image_tiled(bg, true, 0);
        }
    }

    /* 标题逐字上屏 + P2-A4 弹跳切换动画（lv_anim） */
    lv_obj_t *title_label = lv_label_create(scr);
    lv_label_set_text(title_label, title);
    lv_obj_set_pos(title_label, 12, 8);
    lv_obj_set_style_text_color(title_label, lv_color_hex(KSP_COLOR_TEXT), 0);
    {
        lv_anim_t a;

        lv_anim_init(&a);
        lv_anim_set_var(&a, title_label);
        lv_anim_set_values(&a, -10, 8);
        lv_anim_set_duration(&a, 450);
        lv_anim_set_path_cb(&a, lv_anim_path_bounce);
        lv_anim_set_exec_cb(&a, ksp_title_bounce_anim_cb);
        lv_anim_start(&a);
    }

    for (int i = 0; i < n; i++) {
        int y = KSP_ROW_Y0 + KSP_ROW_H * i;

        /* 圆点两态 (S-F1.2)：空心 = 已上完/明天，实心 = 待上。
         * dot_state 防御 -1（不可判定）按「未发生」归空心。 */
        int dot = kidpal_schedule_dot_state(items[i].time, now_hhmm, is_tomorrow);

        lv_obj_t *d = lv_obj_create(scr);
        lv_obj_remove_style_all(d);
        lv_obj_set_size(d, KSP_DOT_SIZE, KSP_DOT_SIZE);
        lv_obj_set_pos(d, KSP_DOT_X, y);
        lv_obj_set_style_radius(d, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_border_width(d, KSP_DOT_BORDER, 0);
        lv_obj_set_style_border_color(d, lv_color_hex(KSP_COLOR_DOT), 0);
        lv_obj_set_style_border_opa(d, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_opa(d, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(d, lv_color_hex(dot == 1 ? KSP_COLOR_DOT
                                                           : KSP_COLOR_BG), 0);

        lv_obj_t *t = lv_label_create(scr);
        lv_label_set_text(t, items[i].time);
        lv_obj_set_pos(t, KSP_COL_TIME_X, y + 10);
        lv_obj_set_style_text_color(t, lv_color_hex(KSP_COLOR_TEXT), 0);

        lv_obj_t *nm = lv_label_create(scr);
        lv_label_set_text(nm, items[i].name);
        lv_obj_set_pos(nm, KSP_COL_NAME_X, y + 10);
        lv_obj_set_style_text_color(nm, lv_color_hex(KSP_COLOR_TEXT), 0);

        lv_obj_t *loc = lv_label_create(scr);
        lv_label_set_text(loc, items[i].location);
        lv_obj_set_pos(loc, KSP_COL_LOC_X, y + 10);
        lv_obj_set_style_text_color(loc, lv_color_hex(KSP_COLOR_TEXT), 0);

        /* P2-A5 星标呼应：画在定位行（设备侧渲染标志，不进 data 形状；
         * 不新增反白高亮态——human 裁砍, P2-E3）。 */
        if (i == locate_row) {
            lv_obj_t *star = lv_label_create(scr);
            lv_label_set_text(star, "★");
            lv_obj_set_pos(star, KSP_STAR_X, y + 10);
            lv_obj_set_style_text_color(star, lv_color_hex(KSP_COLOR_DOT), 0);
        }

        /* 行分隔线（列表视觉；位置压在采样网格整点上，保证 host 真帧可探针） */
        lv_obj_t *sep = lv_obj_create(scr);
        lv_obj_remove_style_all(sep);
        lv_obj_set_size(sep, LV_HOR_RES - 24, 3);
        lv_obj_set_pos(sep, 12, y + 38);
        lv_obj_set_style_bg_color(sep, lv_color_hex(KSP_COLOR_SEP), 0);
        lv_obj_set_style_bg_opa(sep, LV_OPA_COVER, 0);
    }

    /* 换皮：吉祥物每日姿态叠右下空白区（避开圆点/分隔线/列文本）。best-effort。 */
    if (ksp_pose_image_ready()) {
        lv_obj_t *pose = lv_image_create(scr);
        lv_image_set_src(pose, &s_ksp_pose_dsc);
        lv_obj_set_pos(pose, LV_HOR_RES - KSP_POSE_MAX - 4,
                       LV_VER_RES - KSP_POSE_MAX - 6);
    }

    return n;
}
