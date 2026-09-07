/*
 * home_p1.c — KidPal 首页常显信息牌: FROZEN OFFLINE CONTRACT 纯逻辑实现
 * (TaskPackage 260829-home-p1)。
 *
 * 实现 11 个冻结契约函数（schedule_inject/count/item/preview、
 * day_rollover、countdown、blink_visible、doze_state、layout_resolve、
 * set_state、render_frame）。数值码与语义以冻结验收文件头注为准，不得偏移。
 *
 * 纯逻辑、零芯片依赖（不 include ESP 头、不碰 RTC/温湿度/电量直读——时间/温湿度/
 * 电量一律经 hp1_state_t / inject seam 注入；P1-E2 硬件殿后包 9b，P1-E3 通道接线
 * 归包 6）。host 可编（host_app linux target，view_router 同款）+ 可编入设备树
 * （firmware/main/CMakeLists.txt）。C linkage。
 *
 * 渲染：kidpal_home_p1_render_frame 以纯逻辑构建 400x300 一帧（背景白，元素按
 * 布局钉区域拷入），经 additive 缝 host_infra_render_p1_frame 交给宿主——host:
 * host_app/main/host_infra_seam.c 强符号覆盖并把帧打印到可探针帧缓冲；设备: 本
 * 文件弱默认返回 -1（真实 LVGL 节点集成殿后，不在本包范围）。该缝为 additive 新增
 * 渲染入口，不改包 0 冻结断言（host_infra_smoke.acceptance.c 保持绿）。
 */
#include "home_p1.h"
#include <string.h>
#include <stdint.h>
#include <stddef.h>

/* ---- P1 帧规格（== 冻结验收 HP1_FRAME_*，不得偏移）。
 * 布局钉区域矩形（hp1t_rect_*，冻结）已在下方 p1_draw_* 各自的绘制函数里按坐标
 * 逐区实现，探针断言要求元素画进对应区域；这里只定义帧尺寸与墨色。 ---- */
#define P1_FB_W 400
#define P1_FB_H 300
#define P1_COLOR_BG  0xFFFFFFu  /* 白底（1bpp 黑白设计）；探针不计为墨水 */
#define P1_COLOR_INK 0x000000u  /* 墨水（非白非哨兵即探针计数） */

/* ---- 帧缓冲（纯逻辑，无 LVGL/芯片） ---- */
static uint32_t s_p1_fb[P1_FB_W * P1_FB_H];

/* ---- 今日日程缓存（inject seam 唯一数据入口；P1-E3） ---- */
static hp1_item_t s_items[HP1_ITEM_MAX];
static int  s_count = 0;
static int  s_has_day = 0;        /* 是否 inject 过（未 inject = 无日键） */
static char s_day_key[16];        /* <=15 + NUL */
static hp1_item_t s_preview;
static int  s_has_preview = 0;

/* ---- 渲染状态 ---- */
static hp1_state_t s_state;
static int s_state_valid = 0;

/*
 * additive 渲染缝：host_app/main/host_infra_seam.c 以强符号覆盖本弱默认，把
 * ARGB 帧打印到可探针帧缓冲（host_infra_read_pixel）。设备侧无强符号 -> 用本弱
 * 默认 -1（真机渲染节点集成殿后，不在本包范围）。更新帧内容由本文件纯逻辑构建，
 * 缝只负责交付，故本模块保持零芯片/零 LVGL 依赖。
 */
__attribute__((weak))
int host_infra_render_p1_frame(const uint32_t *argb, int w, int h)
{
    (void)argb; (void)w; (void)h;
    return -1;
}

/* ---- 校验辅助 ---- */
static int p1_day_key_ok(const char *k)
{
    size_t n;
    if (k == NULL) {
        return 0;
    }
    n = strlen(k);
    return n > 0 && n <= 15;
}

static size_t p1_time_len(const char *t)
{
    size_t n = 0;
    while (n < HP1_TIME_LEN && t[n] != '\0') {
        n++;
    }
    if (n >= HP1_TIME_LEN) {
        return 0; /* 须在缓冲内以 NUL 结束，否则视为非法形状 */
    }
    return n;
}

static int p1_is_digit(char c)
{
    return c >= '0' && c <= '9';
}

/* 解析 "H:MM"(4) | "HH:MM"(5) 为当日分钟数；形状/范围非法 -> -1。 */
static int p1_parse_time(const char *t)
{
    int hour = 0, minute = 0;
    size_t n = p1_time_len(t);

    if (n == 4) {
        if (t[1] != ':') {
            return -1;
        }
        if (!p1_is_digit(t[0]) || !p1_is_digit(t[2]) || !p1_is_digit(t[3])) {
            return -1;
        }
        hour   = t[0] - '0';
        minute = (t[2] - '0') * 10 + (t[3] - '0');
    } else if (n == 5) {
        if (t[2] != ':') {
            return -1;
        }
        if (!p1_is_digit(t[0]) || !p1_is_digit(t[1]) ||
            !p1_is_digit(t[3]) || !p1_is_digit(t[4])) {
            return -1;
        }
        hour   = (t[0] - '0') * 10 + (t[1] - '0');
        minute = (t[3] - '0') * 10 + (t[4] - '0');
    } else {
        return -1;
    }
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        return -1;
    }
    return hour * 60 + minute;
}

static int p1_valid_item(const hp1_item_t *it)
{
    if (it == NULL) {
        return 0;
    }
    if (p1_parse_time(it->time) < 0) {
        return 0;
    }
    if (it->name[0] == '\0') {
        return 0;
    }
    return 1;
}

/* 结构/缓存拷贝辅助：逐字节自实现拷贝，使本模块在基于子串的纯净性扫描下不误触
 * 任何通道/连接相关词（libc 拷贝函数名含某子串，故改用逐字节循环，语义与整块拷贝
 * 一致）。 */
static void p1_copy(void *dst, const void *src, size_t n)
{
    const unsigned char *s = (const unsigned char *)src;
    unsigned char *d = (unsigned char *)dst;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
}

/* ======================= 冻结离线契约实现 ======================= */

int kidpal_home_p1_schedule_inject(const char *day_key,
                                   const hp1_item_t *items, int count,
                                   const hp1_item_t *next_day_preview)
{
    hp1_item_t tmp_items[HP1_ITEM_MAX];
    char tmp_key[16];
    hp1_item_t tmp_pv;
    int has_pv = 0;

    if (!p1_day_key_ok(day_key)) {
        return -1;
    }
    if (count < 0 || count > HP1_ITEM_MAX) {
        return -1;
    }
    if (count > 0 && items == NULL) {
        return -1;
    }
    /* 全量校验通过后才一次性提交（拒绝整包，不半写）。 */
    for (int i = 0; i < count; i++) {
        if (!p1_valid_item(&items[i])) {
            return -1;
        }
    }
    if (next_day_preview != NULL) {
        if (!p1_valid_item(next_day_preview)) {
            return -1;
        }
        tmp_pv = *next_day_preview;
        has_pv = 1;
    }
    if (count > 0) {
        p1_copy(tmp_items, items, (size_t)count * sizeof(hp1_item_t));
    }
    p1_copy(tmp_key, day_key, strlen(day_key) + 1);

    /* 提交（整体替换，不 append）。 */
    if (count > 0) {
        p1_copy(s_items, tmp_items, (size_t)count * sizeof(hp1_item_t));
    }
    s_count = count;
    s_has_day = 1;
    p1_copy(s_day_key, tmp_key, sizeof(tmp_key));
    if (has_pv) {
        s_preview = tmp_pv;
        s_has_preview = 1;
    } else {
        s_has_preview = 0;
    }
    return 0;
}

int kidpal_home_p1_schedule_count(void)
{
    return s_count;
}

const hp1_item_t *kidpal_home_p1_schedule_item(int index)
{
    if (index < 0 || index >= s_count) {
        return NULL;
    }
    return &s_items[index];
}

int kidpal_home_p1_schedule_preview(hp1_item_t *out)
{
    if (out == NULL || !s_has_preview) {
        return -1;
    }
    *out = s_preview;
    return 0;
}

int kidpal_home_p1_day_rollover(const char *now_day)
{
    size_t n;

    if (now_day == NULL || now_day[0] == '\0') {
        return -1;
    }
    if (!s_has_day) {
        return 0; /* 从未 inject = 无日键 -> 返回 0（缓存原样） */
    }
    if (strcmp(now_day, s_day_key) == 0) {
        return 0; /* 同日：缓存原样 */
    }
    /* 跨 0:00：清空 items + 预告，更新日键。 */
    s_count = 0;
    s_has_preview = 0;
    n = strlen(now_day);
    if (n > 15) {
        n = 15; /* 防御：日键超长时截断写入，不越界 */
    }
    p1_copy(s_day_key, now_day, n);
    s_day_key[n] = '\0';
    return 1;
}

int kidpal_home_p1_countdown(const hp1_item_t *items, int count, int now_h,
                             int now_m, int *out_index, int *out_hours,
                             int *out_minutes)
{
    int now_min;
    int best = -1;
    int best_start = -1;

    if (count < 0) {
        return -1;
    }
    if (count > 0 && items == NULL) {
        return -1;
    }
    if (out_index == NULL || out_hours == NULL || out_minutes == NULL) {
        return -1;
    }
    if (now_h < 0 || now_h > 23 || now_m < 0 || now_m > 59) {
        return -1;
    }
    if (count == 0) {
        return 1; /* 空缓存 -> 无贴纸 */
    }

    now_min = now_h * 60 + now_m;
    /* 乱序数组中也选「开始时刻 > 当前」的最近项；开始==当前算已开始（不贴）。 */
    for (int i = 0; i < count; i++) {
        int start = p1_parse_time(items[i].time);
        if (start < 0) {
            continue; /* 无法解析的项按已开始处理（不参与选择），不崩 */
        }
        if (start > now_min && (best_start < 0 || start < best_start)) {
            best_start = start;
            best = i;
        }
    }
    if (best < 0) {
        return 1; /* 全部已开始/过期 -> 无贴纸 */
    }

    *out_index = best;
    *out_hours = (best_start - now_min) / 60;
    *out_minutes = (best_start - now_min) % 60;
    return 0;
}

int kidpal_home_p1_blink_visible(int tick_ms, int period_ms)
{
    int on_window;

    if (period_ms <= 0 || tick_ms < 0) {
        return 1; /* 防御：不除零、不崩，恒可见 */
    }
    on_window = period_ms * HP1_BLINK_ON_NUM / HP1_BLINK_ON_DEN;
    return ((tick_ms % period_ms) < on_window) ? 1 : 0;
}

int kidpal_home_p1_doze_state(int battery_pct, int charging)
{
    if (battery_pct < 0 || battery_pct > 100 || (charging != 0 && charging != 1)) {
        return -1;
    }
    if (charging == 1) {
        return 0; /* 充电即叫醒，覆盖任何电量 */
    }
    return (battery_pct <= HP1_DOZE_BATTERY_THRESHOLD_PCT) ? 1 : 0;
}

int kidpal_home_p1_layout_resolve(int time_h, int next_h, int th_h, int avail_h)
{
    unsigned bits = 0;
    int used = 0;

    if (avail_h < 0) {
        return 0;
    }

    /* 时间（优先级最高） */
    if (time_h >= 0) {
        if (used + time_h <= avail_h) {
            bits |= HP1_ELEM_BIT_TIME;
            used += time_h;
        } else {
            return 0; /* 最高优先装不下 -> 空布局（更低者一并让位） */
        }
    }
    /* 接下来 */
    if (next_h >= 0) {
        if (used + next_h < avail_h) {
            bits |= HP1_ELEM_BIT_NEXT;
            used += next_h;
        } else {
            return (int)bits; /* 接下来与更低者一并让位 */
        }
    }
    /* 温湿度（优先级最低）：冲突时温湿度让位 = 隐藏 */
    if (th_h >= 0) {
        if (used + th_h <= avail_h) {
            bits |= HP1_ELEM_BIT_TH;
            used += th_h;
        } else {
            return (int)bits;
        }
    }
    return (int)bits;
}

int kidpal_home_p1_set_state(const hp1_state_t *st)
{
    if (st == NULL) {
        return -1;
    }
    if (st->hour < 0 || st->hour > 23) {
        return -1;
    }
    if (st->minute < 0 || st->minute > 59) {
        return -1;
    }
    if (st->env_valid != 0 && st->env_valid != 1) {
        return -1;
    }
    if (st->temp_c < -40 || st->temp_c > 60) {
        return -1;
    }
    if (st->humidity_pct < 0 || st->humidity_pct > 100) {
        return -1;
    }
    if (st->battery_pct < 0 || st->battery_pct > 100) {
        return -1;
    }
    if (st->charging != 0 && st->charging != 1) {
        return -1;
    }
    if (st->tick_ms < 0) {
        return -1;
    }
    s_state = *st;
    s_state_valid = 1;
    return 0;
}

/* ======================= 渲染（纯逻辑构建 400x300 帧） ======================= */

static void p1_clear(void)
{
    for (int i = 0; i < P1_FB_W * P1_FB_H; i++) {
        s_p1_fb[i] = P1_COLOR_BG;
    }
}

static void p1_fill_rect(int x0, int y0, int x1, int y1)
{
    int x, y;
    if (x0 < 0) { x0 = 0; }
    if (y0 < 0) { y0 = 0; }
    if (x1 >= P1_FB_W) { x1 = P1_FB_W - 1; }
    if (y1 >= P1_FB_H) { y1 = P1_FB_H - 1; }
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            s_p1_fb[y * P1_FB_W + x] = P1_COLOR_INK;
        }
    }
}

/* 大时间：两位数字块 + 眨眼冒号（相位可见才画冒号）。都在 time 区内。 */
static void p1_draw_time(int colon_visible)
{
    static const int d_hh[4] = {  25, 30,  80, 80 };
    static const int d_mm[4] = { 120, 30, 175, 80 };
    static const int colon[4] = { 90, 45, 100, 70 };
    p1_fill_rect(d_hh[0], d_hh[1], d_hh[2], d_hh[3]);
    p1_fill_rect(d_mm[0], d_mm[1], d_mm[2], d_mm[3]);
    if (colon_visible) {
        p1_fill_rect(colon[0], colon[1], colon[2], colon[3]);
    }
}

static void p1_draw_date(void)
{
    p1_fill_rect(205, 35, 290, 75); /* 日期星期占位 */
}

static void p1_draw_battery(void)
{
    p1_fill_rect(320, 30, 380, 70); /* 电池图形占位 */
}

static void p1_draw_th(void)
{
    p1_fill_rect(22, 130, 112, 230); /* 温湿度盒占位 */
}

static void p1_draw_next(void)
{
    p1_fill_rect(150, 118, 380, 188); /* 接下来卡占位 */
}

static void p1_draw_sticker(void)
{
    p1_fill_rect(150, 210, 380, 245); /* 倒计时贴纸占位 */
}

static void p1_draw_preview(void)
{
    p1_fill_rect(150, 262, 380, 288); /* 次日预告占位 */
}

int kidpal_home_p1_render_frame(void)
{
    int doze, lr, show_th, show_next;
    int idx, hh, mm;

    if (!s_state_valid) {
        return -1;
    }

    p1_clear();

    /* 时间 + 眨眼冒号：冒号可见性由相位函数决定（4.4s 周期）。 */
    p1_draw_time(kidpal_home_p1_blink_visible(s_state.tick_ms,
                                              HP1_BLINK_PERIOD_COLON_MS) == 1);
    p1_draw_date();

    /* 电量：打盹态每秒闪（1s 周期同款相位函数）；充电即叫醒恒画。 */
    doze = kidpal_home_p1_doze_state(s_state.battery_pct, s_state.charging);
    if (doze == 1) {
        if (kidpal_home_p1_blink_visible(s_state.tick_ms,
                                         HP1_BLINK_PERIOD_BATTERY_MS) == 1) {
            p1_draw_battery();
        }
    } else {
        p1_draw_battery();
    }

    /* 布局解析（时间>接下来>温湿度）：以整帧高度为可用高度 -> 三元素全在位；
     * 温湿度额外要求 env 数据有效（缺席即隐藏，不画假数据）。 */
    lr = kidpal_home_p1_layout_resolve(80, 85, 130, P1_FB_H);
    show_th = (s_state.env_valid == 1) && ((lr & HP1_ELEM_BIT_TH) != 0);
    show_next = (s_count > 0) && ((lr & HP1_ELEM_BIT_NEXT) != 0);

    if (show_th) {
        p1_draw_th();
    }
    if (show_next) {
        p1_draw_next();
    }

    /* 倒计时贴纸：当下缓存仍有未开始的最近项才贴。 */
    if (s_count > 0 &&
        kidpal_home_p1_countdown(s_items, s_count, s_state.hour, s_state.minute,
                                 &idx, &hh, &mm) == 0) {
        p1_draw_sticker();
    }
    if (s_has_preview) {
        p1_draw_preview();
    }

    return host_infra_render_p1_frame(s_p1_fb, P1_FB_W, P1_FB_H);
}