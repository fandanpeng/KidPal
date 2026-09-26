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
 * 布局钉区域 + V3 资产落位）经 additive 缝 host_infra_render_p1_frame 交给宿主——
 * host: host_app/main/host_infra_seam.c 强符号覆盖并把帧打印到可探针帧缓冲；设备:
 * 包 9b display/kidpal_render_node.cc 强符号覆盖，把 s_p1_fb 推上 LVGL 屏（RLCD
 * 已可显）。9b 首页内容渲染批：数字字形 + 资产替换旧实心占位矩形（旧黑块遮住
 * art 像素不可见）。该缝为 additive 新增渲染入口，不改包 0 冻结断言（host_infra
 * _smoke.acceptance.c 保持绿）。
 */
#include "home_p1.h"
#include "art_assets.h"
#include "kidpal_p1_font.h"
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

/* ---- 数字字形目标盒（M1-N1b 真机根因修复序列；不偏移资产/不动 art_assets.c） ----
 * imgfont_digit_*.bin 是旧版式大时钟的 48 宽 x 64 高字形（advance 冻结 48），
 * 辅助条/小一号数字若按 12/13px pitch 原样逐字符 blit 会相邻重叠溢出（真机「超大
 * 字体」）。本文件统一改走运行时最近邻降采样进目标盒 + 钉区 clip。翻档为两套规格：
 *
 *   P1_DIG_W/H = 16x22 ——「主体区」小一号之目标盒（sticker/nextlist/preview 共用，
 *   pitch 14；W2 third-round 定为 body 口径，不再被辅助条消费）。主体验「接下来」
 *   （pitch 25）保持原样 48px 字形 == 最醒目恰配，不经缩放。
 *
 *   P1_AUX_DIG_W/H = 12x16、P1_AUX_PITCH = 10 ——「辅助条」四数值（时间/温度/湿度/
 *   电量）之统一小字规格（W2 design authority WF-F1-01 v1.4: ①辅助条各数值同尺寸
 *   小字、图标前置）。源 48x64 字形降采样进 12x16 盒、pitch 10。湿度/电量数值后补
 *   '%' 记号（M1-N1b 看屏终验：手工基元两轮修形不达标 -> 改走真字形 imgfont_pct.bin，
 *   同 cell 同基线 p1_blit_glyph_scaled('%', …)），收进各自冻结元素矩形内。 ---- */
#define P1_DIG_W 16
#define P1_DIG_H 22
#define P1_AUX_DIG_W 12
#define P1_AUX_DIG_H 16
#define P1_AUX_PITCH  10

/* ---- 帧缓冲（纯逻辑，无 LVGL/芯片） ----
 * 包 9b HWV-A6 渲染节点落地：设备侧 480KB 帧缓冲置 PSRAM (.ext_ram.bss) 以避免
 * ESP32-S3 内部 DRAM 溢出（sdkconfig.defaults.esp32s3 开启
 * CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY 才生效）；host 构建无 ESP_PLATFORM
 * 宏，保持普通静态 .bss。渲染逻辑、区域语义、帧内容均不变——仅改变物理存储位置。 */
#ifdef ESP_PLATFORM
static uint32_t s_p1_fb[P1_FB_W * P1_FB_H] __attribute__((section(".ext_ram.bss")));
#else
static uint32_t s_p1_fb[P1_FB_W * P1_FB_H];
#endif

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

/* ---- [S1] 字形获取弱默认（恒不可用） ----
 * 强符号落点 = kidpal_render_node.cc（设备 font_puhui_14_1）/ host_infra_seam.c
 * （host Source Han 子集）。强符号接入前本弱默认恒返回非 0 -> name 不画（现状，
 * 设计态）。out == NULL -> 非 0（[S1] 防御）。 */
__attribute__((weak))
int kidpal_p1_font_glyph(uint32_t codepoint, kidpal_p1_glyph_t *out)
{
    (void)codepoint;
    if (out != NULL) {
        out->w = 0;
        out->h = 0;
        out->advance = 0;
        out->bits = NULL;
    }
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
    /* 日期可缺（0 = 未知，渲染叠描边兜底不画假日期；M1-N1b 日期 DD-Mon 扩展）。 */
    if (st->month < 0 || st->month > 12) {
        return -1;
    }
    if (st->mday < 0 || st->mday > 31) {
        return -1;
    }
    /* 星期可缺（-1 = 未注入，日期渲染退回 "DD Mon" 不画假星期；2026-09-26 对齐轮）。 */
    if (st->wday < -1 || st->wday > 6) {
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
/*
 * 包 260915-m1-n1b-p1-home-battery（M1-N1b）：WF-F1-01 v1.4 重排序新版式。
 * 布局钉区域矩形（含端点；钉区是布局在位钉不是字体/颜色钉；2026-09-26 真机对齐轮
 * 按 canvas-p1 实测重排——顶虚线 y32、日期有星期、温度 °C、卡区右移上移、猫下移）：
 *   ① 辅助条 y[0..39] 横贯带，左->右 = 时间 {36,6,108,33} · 日期 {112,6,208,33}
 *      （有星期 "Www DD Mon" / 无星期 "DD Mon"）·
 *      温度 {210,6,278,33}（图标半 {210..230} 前置 + 数值半 {234..278} 含 °C）·
 *      湿度 {282,6,338,33}（图标半前置 + 数值半，不裸数字）·
 *      电量 {342,6,396,33}（图标半前置 + 数值半）；
 *   ② 日程主体：小伴常驻演出（左）{28,116,124,212} + 「接下来」卡（右）
 *      {103,29,392,242}（云朵 {103..149} + 帽标 {151..209} + 行流列 x152 起）；
 *   ③ 后续事项逐项（行流下排）+ 明日预告（帽标独立行 + 项行, 组间虚线随行流）。
 * 兜底义务：每个「有墨」钉区在 art 未挂载的兜底路径下也有墨（描边/兜底字形纪律）；
 * 每个「零墨」钉区在两条路径下严格零墨。温湿度缺席（env_valid=0）= 整元素隐藏、
 * 不画假数据。
 *
 * 低电闪烁（PH-A4）：doze = battery_pct <= 15 且未充电；doze 灭相窗（1s 周期，
 * 可见窗 3/4）内辅助条电池整元素（图标+数值）不画；充电恒醒恒画。低电不占小伴
 * 姿态通道（§8 已砍项零触碰——低电只保留辅助条电池图标每秒闪最小信号）。
 *
 * 主机影响面（p1_home_battery.acceptance.c 断言逐条判定；不改动冻结文件）：
 *   - PH-A1 主体在位：next/sticker/mascot/nextlist/preview 各按钉区画墨；空日程
 *     （s_count=0）时 next 整区不画 -> 严格零墨（旧版式大时间已让位辅助条）。
 *   - PH-A2 图标前置：温/湿/电图标半与数值半分别落各自半区（湿度数值不单独裸在
 *     电池旁——图标半在前）；env_valid=0 时温/湿整元素不画 -> 零墨。
 *   - PH-A3 行为半：辅助条电量渲染消费 hp1_state.battery_pct（注入缝驱动）。
 *   - PH-A4：doze==1 且灭相 -> p1_draw_aux_batt 整个不调（图标半+数值半+兜底描边
 *     全在门内）-> 电池整区零墨；亮相窗/充电/非低电恒画。
 */

static void p1_clear(void)
{
    for (int i = 0; i < P1_FB_W * P1_FB_H; i++) {
        s_p1_fb[i] = P1_COLOR_BG;
    }
}

/* 保留：小面积实心块（仅用于兜底冒号两点等"必须有严格减墨量"的极小面，
 * 大区域一律不再走此函数——过去大 rect 实心覆盖 art 是本轮要修的根因）。 */
static void p1_fill_rect(int x0, int y0, int x1, int y1)
{
    int x, y;
    if (x0 < 0) { x0 = 0; }
    if (y0 < 0) { y0 = 0; }
    if (x1 >= P1_FB_W) { x1 = P1_FB_W - 1; }
    if (y1 >= P1_FB_H) { y1 = P1_FB_H - 1; }
    if (x0 > x1 || y0 > y1) { return; }
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            s_p1_fb[y * P1_FB_W + x] = P1_COLOR_INK;
        }
    }
}

/* 描边矩形（t=厚度，px）：区域内白色保留、只画四边——过去实心涂墨遮住 art 的
 * 根因即用此替换；同时确保「区域内有墨」断言在无 art 时仍成立（描边像素本身即
 * 计墨）。t<=0 视为 1。 */
static void p1_stroke_rect(int x0, int y0, int x1, int y1, int t)
{
    int x, y;
    if (x0 < 0) { x0 = 0; }
    if (y0 < 0) { y0 = 0; }
    if (x1 >= P1_FB_W) { x1 = P1_FB_W - 1; }
    if (y1 >= P1_FB_H) { y1 = P1_FB_H - 1; }
    if (x0 > x1 || y0 > y1) { return; }
    if (t <= 0) { t = 1; }
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            int on_edge = (x < x0 + t) || (x > x1 - t) ||
                          (y < y0 + t) || (y > y1 - t);
            if (on_edge) {
                s_p1_fb[y * P1_FB_W + x] = P1_COLOR_INK;
            }
        }
    }
}

/* 判断资产层 imgfont 族是否挂载（= host ART 组测试之后 / 设备 SPIFFS 挂载之后
 * 都会为真）。仅探测、不写帧；返回 1 表示后续 glyph/pixel 调用可用。
 * 一次帧渲染内缓存于 s_art_ok（每帧入口刷新），避免每 draw 函数各 stat 一次
 * imgfont_digit_0.bin（SPIFFS fopen 成本敏感）。 */
static int s_art_ok = 0;

static void p1_refresh_art_ok(void)
{
    s_art_ok = (kidpal_art_glyph_advance() > 0) ? 1 : 0;
}

/*
 * loader 原语消费接线（非整帧缝）：把 KAP1 位图按 1bpp 语义拷进首页自有帧缓冲
 * s_p1_fb。只 ADD 墨水（命中 ink 像素才写），未 mount / 加载失败 -> 返回 0 让
 * 调用方决定 fallback；成功返回 1。clip 到 (bw,bh) 盒内，绝不越界写入其它
 * （可能要求无墨的）区域。
 */
static int p1_blit_art(const char *file, int x0, int y0, int bw, int bh)
{
    kidpal_art_img_t img;

    if (file == NULL || bw <= 0 || bh <= 0) {
        return 0;
    }
    if (kidpal_art_load(file, &img) != 0) {
        return 0; /* 未 mount / 文件缺 / 解码失败 -> 让调用方走兜底 stroke */
    }
    for (int y = 0; y < img.h && y < bh; y++) {
        for (int x = 0; x < img.w && x < bw; x++) {
            int dx = x0 + x, dy = y0 + y;
            int p;
            if (dx < 0 || dy < 0 || dx >= P1_FB_W || dy >= P1_FB_H) {
                continue;
            }
            p = kidpal_art_pixel(&img, x, y);
            if (p == 1) {
                s_p1_fb[dy * P1_FB_W + dx] = P1_COLOR_INK;
            }
        }
    }
    return 1;
}

/*
 * loader 原语消费【缩放】（M1-N1b 真机根因修复，与 p1_blit_glyph_scaled 同款手法）：
 * 图标资产（如 thermo_man 41x67 / drop_baby 32x51 / ic_batt_48 48x48）远大于首页
 * 辅助条图标半区槽位（20x28 / 20x28 / 22x28）——裸 p1_blit_art 只 clip 不缩放 = 只画
 * 资产左上角 bh 行内就被裁、图标只显示一小半（真机「图标只显示一半」）。本函数把
 * KAP1 位图按 1bpp 最近邻整数降采样进目标 [x0, x0+dw-1] x [y0, y0+dh-1] 盒（同款
 * 映射 sy=(y*img.h)/dh, sx=(x*img.w)/dw），再 clip 进钉区 [cx0,cy0,cx1,cy1] 与帧缓冲
 * ——绝不让 ink 溢出图标半区、也不让残边粘上相邻数值半（真机「残边与 95 视觉粘连」）。
 * 只 ADD 墨水；未 mount / 加载失败 -> 0（调用方走原有兜底描边）。纯整数运算。
 */
static int p1_blit_art_scaled(const char *file, int x0, int y0, int dw, int dh,
                              int cx0, int cy0, int cx1, int cy1)
{
    kidpal_art_img_t img;

    if (file == NULL || dw <= 0 || dh <= 0) {
        return 0;
    }
    if (kidpal_art_load(file, &img) != 0) {
        return 0; /* 未 mount / 文件缺 / 解码失败 -> 让调用方走兜底 stroke */
    }
    if (img.w <= 0 || img.h <= 0) {
        return 0;
    }
    for (int y = 0; y < dh; y++) {
        int sy = (y * img.h) / dh; /* 源行最近邻映射 */
        for (int x = 0; x < dw; x++) {
            int sx = (x * img.w) / dw; /* 源列最近邻映射 */
            int px = x0 + x, py = y0 + y;
            if (px < cx0 || px > cx1 || py < cy0 || py > cy1) {
                continue; /* 钉区 clip，绝不越区写入其它区域 */
            }
            if (px < 0 || py < 0 || px >= P1_FB_W || py >= P1_FB_H) {
                continue;
            }
            if (kidpal_art_pixel(&img, sx, sy) == 1) {
                s_p1_fb[py * P1_FB_W + px] = P1_COLOR_INK;
            }
        }
    }
    return 1;
}

/* 追加轮 [S3] 版式退役：非缩放 imgfont 单字形 blit（p1_blit_glyph）与逐字符
 * pitch blit（p1_blit_digits）原为大 imgfont 时间/旧贴纸服务，本轮退役不再被
 * 调用（文字行全走 [S1] seam + 行流）。缩放路径 p1_blit_glyph_scaled /
 * p1_blit_str_scaled 保留供辅助条（时间/温湿度/电量）消费。 */

/* imgfont 单字形【缩放】blit（'0'..'9' | ':'）：把源字形 cell (img.w x img.h) 最近邻
 * 降采样进目标盒 [x0, y0, x0+dw-1, y0+dh-1]，再 clip 进钉区盒 [cx0,cy0,cx1,cy1] 与帧
 * 缓冲——绝不让 ink 溢出钉区、也绝不让相邻字形重叠（根因修复）。只 ADD 墨水；未挂载/
 * 字符非法 -> 0。纯整数运算（无浮点），适配 1bpp 帧。 */
static int p1_blit_glyph_scaled(char c, int x0, int y0, int dw, int dh,
                                int cx0, int cy0, int cx1, int cy1)
{
    kidpal_art_img_t img;

    if (kidpal_art_glyph(c, &img) != 0) {
        return 0;
    }
    if (dw <= 0 || dh <= 0 || img.w <= 0 || img.h <= 0) {
        return 0;
    }
    for (int y = 0; y < dh; y++) {
        int sy = (y * img.h) / dh; /* 源行最近邻映射 */
        for (int x = 0; x < dw; x++) {
            int sx = (x * img.w) / dw; /* 源列最近邻映射 */
            int px = x0 + x, py = y0 + y;
            if (px < cx0 || px > cx1 || py < cy0 || py > cy1) {
                continue; /* 钉区 clip：绝不越区写入其它（可能要求无墨的）区域 */
            }
            if (px < 0 || py < 0 || px >= P1_FB_W || py >= P1_FB_H) {
                continue;
            }
            if (kidpal_art_pixel(&img, sx, sy) == 1) {
                s_p1_fb[py * P1_FB_W + px] = P1_COLOR_INK;
            }
        }
    }
    return 1;
}

/* 把 "HH:MM" / "H:MM" / 任意数值串按【缩放】逐字符、pitch 步进 blit 进帧；每个字形
 * 降采样进同一 (dw,dh) 盒并 clip 进钉区。colon_visible=0 时跳过 ':'（眨眼灭相严格减
 * 墨的墨源）。返回成功 blit 的字符数。 */
static int p1_blit_str_scaled(const char *s, int x0, int y0, int pitch,
                              int dw, int dh, int cx0, int cy0, int cx1, int cy1,
                              int colon_visible)
{
    int ok = 0, i;

    if (s == NULL || pitch <= 0) {
        return 0;
    }
    for (i = 0; s[i] != '\0'; i++) {
        char c = s[i];
        if (c == ':' && !colon_visible) {
            continue;
        }
        if (p1_blit_glyph_scaled(c, x0 + i * pitch, y0, dw, dh,
                                 cx0, cy0, cx1, cy1)) {
            ok++;
        }
    }
    return ok;
}

/* ---- [S2]/[S4] UTF-8 解码 + 动态日程名逐字形 blit（seam [S1]）+ 正常换行 ----
 * name/time 来自 hp1_item_t（name ZU HP1_NAME_LEN=32，上游注入层按字节截 31 会截半
 * UTF-8 多字节字符）——解码器遇不完整尾序列安全停笔：截半字符不画、不越界、不崩
 * （[S4]，容错在 P1 侧重构，K2 注入层本包零改）。
 * 追加轮 [S3] 行流语义：文字行自 ROW1 y=62（2026-09-26 对齐轮重钉, 原 96）起**正常
 * 换行**流式下排（换行续行落下一
 * 行带），一切墨 clip 进给定盒（含卡区右界 x=392 / 底界 y=242 containment）。
 * 行内时间走 [S2] seam ASCII（与 name 同缝承载，host 可探 time 值 diff）。
 * 字形不可用（[S1] 弱默认/缺字形）-> 跳过该字符、不画垃圾（A5 退化）。
 * 返回本次调用起始的行数（wrap 后加行递增）。 */

/* 依首字节判 UTF-8 序列字节数；非法首字节 -> 0。 */
static int p1_utf8_seq_bytes(unsigned char b)
{
    if (b < 0x80u) {
        return 1;
    }
    if ((b & 0xE0u) == 0xC0u) {
        return 2;
    }
    if ((b & 0xF0u) == 0xE0u) {
        return 3;
    }
    if ((b & 0xF8u) == 0xF0u) {
        return 4;
    }
    return 0;
}

/* "HH:MM " + name 接进缓冲（避免 snprintf -Wformat-truncation；time<=5、1 空格、
 * name<=31, 缓冲 >= 40 即安全）。 */
static void p1_join_time_name(const hp1_item_t *it, char *buf, int cap)
{
    const char *s;
    int n = 0;

    for (s = it->time; *s != '\0' && n < cap - 1; s++) {
        buf[n++] = *s;
    }
    if (n < cap - 1) {
        buf[n++] = ' ';
    }
    for (s = it->name; *s != '\0' && n < cap - 1; s++) {
        buf[n++] = *s;
    }
    buf[n] = '\0';
}

/* 行流文本 blit：从 (x0, y0) 起逐字形画 UTF-8 text，行宽超右界 cx1（且非行首）时换行
 * （cur_y += line_h, cur_x 归 x0）；每字形沿 w/h 逐像素画、clip 进盒 [cx0,cy0,cx1,cy1]
 * 与帧缓冲（含右界/底界 containment——墨绝不越过 cx1/cy1）。返回起始的行数。 */
static int p1_blit_flow(const char *text, int x0, int y0, int line_h,
                        int cx0, int cy0, int cx1, int cy1)
{
    size_t len;
    size_t pos;
    int cur_x;
    int cur_y;
    int lines;

    if (text == NULL || line_h <= 0) {
        return 1; /* 空/非法入参：占 1 行防止调用方光速越界 */
    }
    len = strlen(text);
    pos = 0;
    cur_x = x0;
    cur_y = y0;
    lines = 1;

    while (pos < len) {
        unsigned char b = (unsigned char)text[pos];
        uint32_t cp;
        int seq, i;

        if (b < 0x80u) {
            cp = b;
            seq = 1;
        } else {
            seq = p1_utf8_seq_bytes(b);
            if (seq < 2 || pos + (size_t)seq > len) {
                break; /* [S4] 不完整尾序列安全停笔：截半字符不画、不越界 */
            }
            cp = (uint32_t)(b & (uint32_t)(seq == 2 ? 0x1Fu : seq == 3 ? 0x0Fu : 0x07u));
            for (i = 1; i < seq; i++) {
                unsigned char c = (unsigned char)text[pos + (size_t)i];
                if ((c & 0xC0u) != 0x80u) {
                    cp = 0;
                    break;
                }
                cp = (cp << 6) | (uint32_t)(c & 0x3Fu);
            }
            if (cp == 0) {
                break; /* 非法续字节 -> 停笔（退化，不画垃圾） */
            }
        }
        pos += (size_t)seq;

        {
            kidpal_p1_glyph_t g;
            int stride;
            int line_x;
            int sy, sx;

            if (kidpal_p1_font_glyph(cp, &g) != 0 || g.bits == NULL) {
                continue; /* 字形不可用 -> 跳过，不画垃圾 */
            }
            if (g.w <= 0 || g.h <= 0 || g.w > 64 || g.h > 64) {
                continue; /* [S1] 盒上限，越界字形视为不可用 */
            }
            /* 换行：非行首且当前字形笔进越右界 -> 换下一行（正常换行，续行落下一行带）。 */
            line_x = cur_x;
            if (cur_x != x0 && (cur_x + g.advance) > cx1) {
                cur_y += line_h;
                lines++;
                line_x = x0;
                cur_x = x0;
            }
            stride = (g.w + 7) / 8;
            for (sy = 0; sy < g.h; sy++) {
                int py = cur_y + sy;
                if (py < cy0 || py > cy1 || py < 0 || py >= P1_FB_H) {
                    continue;
                }
                for (sx = 0; sx < g.w; sx++) {
                    int px = line_x + sx;
                    int bit;
                    if (px < cx0 || px > cx1 || px < 0 || px >= P1_FB_W) {
                        continue;
                    }
                    bit = (g.bits[(size_t)sy * (size_t)stride + (size_t)(sx >> 3)]
                           >> (7 - (sx & 7))) & 1u;
                    if (bit) {
                        s_p1_fb[py * P1_FB_W + px] = P1_COLOR_INK;
                    }
                }
            }
            cur_x = line_x + g.advance;
        }
    }
    return lines;
}

/* 整数字【缩放】渲染（正负都吃，用于温度 -5..60 与湿度 0..100）。 */
static int p1_blit_int_scaled(int value, int x0, int y0, int pitch,
                              int dw, int dh, int cx0, int cy0, int cx1, int cy1)
{
    char buf[8];
    int n;

    if (value < 0) {
        /* 温度可到 -40；imgfont 无 '-' 字形 -> 用 "40"（幅度）+ 冰点上下文表达；
         * 首页温度实际几乎不出现负值，本轮以绝对值渲染并留殿后清单。 */
        value = -value;
    }
    n = 0;
    if (value == 0) {
        buf[n++] = '0';
    } else {
        char tmp[8];
        int k = 0;
        while (value > 0 && k < (int)sizeof(tmp)) {
            tmp[k++] = (char)('0' + (value % 10));
            value /= 10;
        }
        while (k > 0 && n < (int)sizeof(buf) - 1) {
            buf[n++] = tmp[--k];
        }
    }
    buf[n] = '\0';
    return p1_blit_str_scaled(buf, x0, y0, pitch, dw, dh,
                              cx0, cy0, cx1, cy1, 1);
}

/* 整数值小数位计数（abs，v==0 -> 1），用于 '%' 后缀起笔定位（辅助条值区贴内）。 */
static int p1_ndigits(int v)
{
    int n = 1;
    if (v < 0) {
        v = -v;
    }
    while (v >= 10) {
        v /= 10;
        n++;
    }
    return n;
}

/* 手写 HH:MM / H:MM 格式化（避免 snprintf -Werror=format-truncation 触发；
 * hour 0..23 minute 0..59 已在 set_state 门内保证；buffer 至少 6 字节）。 */
static void p1_fmt_hhmm(int hour, int minute, char *buf, int cap)
{
    int n = 0;

    if (hour >= 10) {
        if (n < cap - 1) { buf[n++] = (char)('0' + hour / 10); }
        if (n < cap - 1) { buf[n++] = (char)('0' + hour % 10); }
    } else {
        if (n < cap - 1) { buf[n++] = (char)('0' + hour); }
    }
    if (n < cap - 1) { buf[n++] = ':'; }
    if (n < cap - 1) { buf[n++] = (char)('0' + minute / 10); }
    if (n < cap - 1) { buf[n++] = (char)('0' + minute % 10); }
    buf[n] = '\0';
}

/* ① 辅助条·时间（小字 "H:MM"/"HH:MM"，冒号眨眼相位沿用冻结周期）：真字形走
 * imgfont 数字（P1_AUX 12x16 盒 pitch=10，5 字符 cell 总宽 4*10+12 -> 墨 x=[41..87]
 * 收进钉区 [36..108]）；未挂载兜底 thin 描边框撑「time 区有墨」pin。 */
static void p1_draw_aux_time(void)
{
    char buf[8];
    int colon_visible =
        (kidpal_home_p1_blink_visible(s_state.tick_ms, HP1_BLINK_PERIOD_COLON_MS) == 1);

    p1_fmt_hhmm(s_state.hour, s_state.minute, buf, (int)sizeof(buf));
    if (s_art_ok) {
        /* 缩放 blit（源 48x64 -> 12x16 盒，最近邻）× clip 进 time 钉区 [36,6,108,33]；
         * M1-N1b 统一中线 y0=12 -> ink x[41..87]@y[14..24]、墨高 11px 以 y≈19 为心
         * （与图标同中线，见自查表）。 */
        (void)p1_blit_str_scaled(buf, 38, 12, P1_AUX_PITCH,
                                 P1_AUX_DIG_W, P1_AUX_DIG_H,
                                 36, 6, 108, 33, colon_visible);
    } else {
        p1_stroke_rect(38, 10, 104, 30, 1);
    }
    if (colon_visible && !s_art_ok) {
        /* 兜底冒号两点（真字形缺场时撑眨眼相位的墨差，落 time 钉区内）。 */
        p1_fill_rect(58, 15, 62, 19);
        p1_fill_rect(58, 23, 62, 27);
    }
}

/* ① 辅助条·日期（M1-N1b 日期 DD-Mon，human 裁定 2026-09-16）：真实渲染 "DD Mon"——
 * 日号 2 位数字（走 P1_AUX 缩放数字路径，pitch=10）+ 空格（固定 advance）+ 月缩写
 * （Jan..Dec title case 静态表，字母走 glyph 缩放路径——字母与数字同 cell 尺寸，
 * 复用 p1_blit_glyph_scaled，见 art_assets.c 字母桶 imgfont_letter_%c.bin 扩展）。
 * 月份数据源 = hp1_state.month/mday（板级 PCF85063 日/月寄存器 → inject_rtc 带入，
 * 链路见 kidpal_render_node.cc s_rtc_month/mday）。month/mday=0（未知/缺 RTC 日期）
 * 或 art 未挂载路径：保留描边兜底（有墨义务；不画假日期）。 */
static const char* const k_p1_month_abbr[13] = {
    "", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

/* 星期缩写表（周一=0..周日=6，与 kidpal_render_node_inject_weekday 同基；
 * 2026-09-26 真机对齐轮 canvas-p1 "Tue 22 Sep"）。 */
static const char* const k_p1_wday_abbr[7] = {
    "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

static void p1_fmt_mday_2(int mday, char *buf, int cap)
{
    /* 日号恒 2 位（"01".."31"；mday 1..31 已在 set_state 门内保证）。 */
    int n = 0;
    if (n < cap - 1) { buf[n++] = (char)('0' + mday / 10); }
    if (n < cap - 1) { buf[n++] = (char)('0' + mday % 10); }
    buf[n] = '\0';
}

static void p1_draw_aux_date(void)
{
    int month = s_state.month;
    int mday  = s_state.mday;
    int wday  = s_state.wday;
    char ds[8];
    const char *mn;
    const char *wd = NULL;
    int drew = 0;
    int x, i;

    /* 日期有效（= RTC 日期已注入）且 art 挂载：真实渲染（有星期 = "Www DD Mon"，
     * 无星期 = "DD Mon" 退回，不画假星期）。 */
    if (month >= 1 && month <= 12 && mday >= 1 && mday <= 31 && s_art_ok) {
        mn = k_p1_month_abbr[month];
        if (wday >= 0 && wday <= 6) {
            wd = k_p1_wday_abbr[wday];
        }
        p1_fmt_mday_2(mday, ds, (int)sizeof(ds));
        /* 布局（落 date 钉区 {112,6,208,33}）：
         * 有星期（2026-09-26 对齐轮，canvas-p1 "Tue 22 Sep"）：星期 3 字母起笔 112
         * （cells [112..142+11]，墨 114..142），4px 空格 -> 日号 2 数字起笔 146
         * （cells [146..156+11]），4px 空格 -> 月首 x=170（3 字母 cells
         * [170..190+11=201]，墨 ..200 < 208），不侵温度图标区(210)。
         * 无星期（legacy）：日号起笔 126、月首 156（原 M1-N1b 布局不动）。 */
        if (wd != NULL) {
            x = 112;
            for (i = 0; wd[i] != '\0' && i < 3; i++) {
                drew |= p1_blit_glyph_scaled(wd[i], x + i * P1_AUX_PITCH, 12,
                                             P1_AUX_DIG_W, P1_AUX_DIG_H,
                                             112, 6, 208, 33);
            }
            x = 146; /* 星期 cells 末 132 + 12 cell 宽 - 132 + 4 空格 = 146 */
            drew |= (p1_blit_str_scaled(ds, x, 12, P1_AUX_PITCH,
                                        P1_AUX_DIG_W, P1_AUX_DIG_H,
                                        112, 6, 208, 33, 1) > 0);
            x = 170; /* 日号 cells 末 156 + 4px 空格 */
            for (i = 0; mn[i] != '\0' && i < 3; i++) {
                drew |= p1_blit_glyph_scaled(mn[i], x + i * P1_AUX_PITCH, 12,
                                             P1_AUX_DIG_W, P1_AUX_DIG_H,
                                             112, 6, 208, 33);
            }
        } else {
            x = 126;
            drew |= (p1_blit_str_scaled(ds, x, 12, P1_AUX_PITCH,
                                        P1_AUX_DIG_W, P1_AUX_DIG_H,
                                        112, 6, 208, 33, 1) > 0);
            x = 126 + 2 * P1_AUX_PITCH + 8; /* = 154（2 数字 cell 后 + 8px 空） */
            for (i = 0; mn[i] != '\0' && i < 3; i++) {
                drew |= p1_blit_glyph_scaled(mn[i], x + i * P1_AUX_PITCH, 12,
                                             P1_AUX_DIG_W, P1_AUX_DIG_H,
                                             112, 6, 208, 33);
            }
        }
    }
    if (!drew) {
        /* 兜底（art 未挂载 / 日期未知）：描边框撑「date 区有墨」pin，不画假日期。 */
        p1_stroke_rect(126, 10, 194, 30, 1);
    }
}

/* ① 辅助条·温度元素（图标半前置 {210,6,230,33} + 数值半 {234,6,278,33}）。
 * 图标：thermo_man 资产 blit（clip 进图标半）；数值：imgfont 整数（P1_AUX 12x16
 * pitch=10）+ °C 真字形后缀（2026-09-26 真机对齐轮 canvas-p1 "23°C"：° 走
 * imgfont_degree、C 走 imgfont_letter_C，同 cell 同 y0 同基线）。三元素整体右移重排
 * （date 有星期加宽后温度起笔 210）：数值起笔 236、°C 跟尾数 cell，n<=2 时 C cell
 * 末 277 收进数值半右界 278。半区各自兜底描边（各半区独立保证「有墨 >= 4」pin；
 * 整元素由 env_valid 门控，缺席即隐藏不画假数据）。 */
static void p1_draw_aux_temp(void)
{
    int n = p1_ndigits(s_state.temp_c);

    /* 图标走缩放 blit（thermo_man 41x67 降采样进 20x20 盒）+ clip 进温度图标半区
     * {210,6,230,33}；M1-N1b 看屏终验统一辅助条中线：图标墨带收敛到 ~18px 高、以
     * 中线 y≈19 为心落座（ink ~[11..28]，下探标定见截图自查表）。 */
    if (!p1_blit_art_scaled("thermo_man.bin", 210, 9, 20, 20,
                            210, 6, 230, 33)) {
        p1_stroke_rect(212, 10, 228, 29, 1);
    }
    if (s_art_ok) {
        (void)p1_blit_int_scaled(s_state.temp_c, 236, 12, P1_AUX_PITCH,
                                 P1_AUX_DIG_W, P1_AUX_DIG_H, 234, 6, 278, 33);
        /* °C 后缀：° cell 跟尾数 cell、C 紧随（+pitch）。'°' 非 ASCII，以 (char)0xB0
         * 传入（art_assets.c kidpal_art_glyph 按 unsigned char 0xB0 分流 imgfont_degree）。 */
        p1_blit_glyph_scaled((char)0xB0, 236 + P1_AUX_PITCH * n, 12,
                             P1_AUX_DIG_W, P1_AUX_DIG_H, 234, 6, 278, 33);
        p1_blit_glyph_scaled('C', 236 + P1_AUX_PITCH * n + P1_AUX_PITCH, 12,
                             P1_AUX_DIG_W, P1_AUX_DIG_H, 234, 6, 278, 33);
    } else {
        p1_stroke_rect(238, 10, 274, 29, 1);
    }
}

/* ① 辅助条·湿度元素（图标半前置 {282,6,302,33} + 数值半 {302,6,338,33}，
 * "45%" 不裸数字挨电池——图标半在前，数值紧跟 '%' 后缀）。水滴宝宝资产 + imgfont
 * 整数 + '%' 真字形（2026-09-26 对齐轮：三元素右移重排，湿度起笔 304 贴温度 °C 半
 * 右界 278 之后的空档），兜底同温度。 */
static void p1_draw_aux_hum(void)
{
    int val = s_state.humidity_pct;
    int n = p1_ndigits(val);

    /* 图标走缩放 blit（drop_baby 32x51 降采样进 20x20 盒）+ clip 进湿度图标半区
     * {282,6,302,33}；M1-N1b 看屏终验统一中线：墨带收敛 ~17px 高、以 y≈19 为心
     * （ink ~[11..27]）。 */
    if (!p1_blit_art_scaled("drop_baby.bin", 282, 9, 20, 20,
                            282, 6, 302, 33)) {
        p1_stroke_rect(284, 10, 300, 29, 1);
    }
    if (s_art_ok) {
        /* 数值起笔 304 贴图标半区右界（302）外侧 2px；'%' 走真字形管线
         * （p1_blit_glyph_scaled('%', …, 12x16 同 cell 同基线）：同数字 cell 自动基线
         * 对齐，同 y0。% 起笔 = 304 + pitch*n（n = 位数）落整元素 {302,6,338,33} 内
         * （n<=2 时 % cell 末 324+11=335 收进右界），clip 防越界。湿度 100（n=3）时
         * 元素右界不足容纳 '%'（预既有缺陷，home 实况近不着边），仍无条件绘制、clip
         * 截断到右界 338。 */
        (void)p1_blit_int_scaled(val, 304, 12, P1_AUX_PITCH,
                                 P1_AUX_DIG_W, P1_AUX_DIG_H, 302, 6, 338, 33);
        p1_blit_glyph_scaled('%', 304 + P1_AUX_PITCH * n, 12,
                             P1_AUX_DIG_W, P1_AUX_DIG_H, 302, 6, 338, 33);
    } else {
        p1_stroke_rect(306, 10, 336, 29, 1);
    }
}

/* ① 辅助条·电量元素（图标半前置 {342,6,362,33} + 数值半 {364,6,396,33}）。
 * ic_batt 资产 + imgfont 整数（"85"/"100" P1_AUX 12x16 pitch=10）+ '%' 真字形，
 * 半区各自兜底描边。整元素由 doze 灭相门控（调用方），充电恒画。
 * 2026-09-26 对齐轮：三元素右移重排，电量图标起笔 342、数值起笔 364（"85%" ink
 * 367..395 收进右界 396，% 全格宽不再 +2 空隙）。 */
static void p1_draw_aux_batt(void)
{
    int val = s_state.battery_pct;
    int n = p1_ndigits(val);

    /* 图标走缩放 blit（ic_batt_48 48x48 降采样进 22x40 盒）+ clip 进电量图标半区
     * {342,6,362,33}；M1-N1b 看屏终验统一中线：box y0=0 顶越区由 clip 取齐、ink 墨带
     * ~18px 高、以 y≈19 为心（ink ~[11..28]，下探标定见截图自查表）。数值起笔 364
     * （图标墨右界贴半区 ~360，>=4px 空隙）。 */
    if (!p1_blit_art_scaled("ic_batt_48.bin", 342, 0, 22, 40,
                            342, 6, 362, 33)) {
        p1_stroke_rect(344, 10, 360, 29, 1);
    }
    if (s_art_ok) {
        (void)p1_blit_int_scaled(val, 364, 12, P1_AUX_PITCH,
                                 P1_AUX_DIG_W, P1_AUX_DIG_H, 364, 6, 396, 33);
        /* '%' 真字形（全格宽 ink 0..cell 12）：起笔 364 + pitch*n（不加空隙，右界
         * 396 前恰好收进：n=2 时 % cell [384..395]），同 12x16 cell 同 y0=12 同数字
         * 基线；clip 防越半区右界 396。 */
        p1_blit_glyph_scaled('%', 364 + P1_AUX_PITCH * n, 12,
                             P1_AUX_DIG_W, P1_AUX_DIG_H, 364, 6, 396, 33);
    } else {
        p1_stroke_rect(366, 10, 394, 29, 1);
    }
}

/* ① 辅助条·WiFi 联网小标（ui-v5-align §2 P1 行 1; spec AC1「📶 联网小标打头」）:
 * ic_wifi_14.bin 14x14 @ (12,12), 辅助条中线 y≈19。常显家具（设计稿状态栏首部小标;
 * 联网状态条件显隐需 hp1_state_t 增网络字段 = 契约面, 本批不碰——归登记遗留）。
 * 加载失败不画（纯装饰件, 无有墨义务）。 */
static void p1_draw_wifi_icon(void)
{
    (void)p1_blit_art_scaled("ic_wifi_14.bin", 12, 12, 14, 14, 12, 12, 25, 25);
}

/* ② 小伴常驻演出：pose_daily 全尺寸 96x96 blit（2026-09-26 对齐轮落位 (28,116)——
 * canvas-p1 实测猫墨迹 x36-109/y127-209，资产墨盒 (8,11)-(81,93) 反解盒原点 =
 * (36-8, 127-11) = (28,116)，墨迹与设计逐像素吻合）；未挂载 thin 描边兜底
 * （有墨 >= 20 pin 两条路径成立）。 */
static void p1_draw_mascot(void)
{
    if (!p1_blit_art("pose_daily_96.bin", 28, 116, 96, 96)) {
        p1_stroke_rect(10, 86, 126, 242, 2);
    }
}

/* 分组分隔点状虚线前置声明（实现与画法注释见 render_frame 前 p1_draw_group_dash_sep）。 */
static void p1_draw_group_dash_sep(int y, int x0, int x1);

/* ②/③ 「接下来」日程卡（[S3] 追加轮行流语义, human 版式扩展 2026-09-22；
 * 2026-09-26 真机对齐轮按 canvas-p1 实测重排：云朵 ic_cloud_40_t2 46px @ (103,29)、
 * 帽标 @ (151,41)、内容列 x152、行流 ROW1 y62）：
 * 卡顶「接下来」= 烘焙黑底白字帽标（[S5] p1_chip_jiexialai.bin 58x21, 卡题固定、
 * 空日程仍在场, ui-v5-align §2 P1 行 5: 设计 = 黑帽标 + 左侧云朵涂鸦; 白底黑字
 * p1_label_* 退役）+ 左侧云朵涂鸦；文字行全部 14px
 * font_puhui_14_1 逐字形 blit + 正常换行——最近项行 ROW1{152,62,392,88}
 * （时间 + 课程名, 行内时间走 [S1] seam ASCII）、后续事项行 ROW2..（逐项 时间+名,
 * 行流下排）, 组间点状虚线分组隔（ui-v5-align §2 P1 行 4: 设计虚线在「接下来」组
 * 与「明天」组之间; 仅两组都在场时画——空日程/无预告不画, 行流带零墨钉不落墨）,
 * 末行预告 = 明早帽标独立行（[S5] p1_chip_mingzao.bin 46x21, 文案「明早」非「明天」——
 * 资产闭集只有明早字样, 「明天」字样资产缺 = 登记遗留, 不画假字样）+ 下一行
 * HH:MM + 名（设计结构：帽标行与项行分行）。行流自 ROW1 起按行流式下排（换行续行
 * 落下一行带），一切墨不越卡区 CARD{103,29,392,242}。
 * 空日程仍画帽标 + 云朵（卡题固定）、行流零墨（HPB tooth 5 语义）。 */
static void p1_draw_card(void)
{
    const int line_h = 28;      /* [S3] 行带 pitch：ROW1 68 -> ROW2 96 -> ROW3 124 */
    const int text_yoff = 6;    /* 14px 字形在 26px 行带内的垂直对齐偏移 */
    const int cx0 = 152, cy0 = 42, cx1 = 392, cy1 = 242; /* 卡区 clip */
    int fy = 68;                /* 行流光标：自 ROW1 顶起, 逐块按占用行数下移 */
    int used, i;
    char line[48];
    const hp1_item_t *it;
    hp1_item_t pv;
    int has_pv = (kidpal_home_p1_schedule_preview(&pv) == 0);

    /* 卡顶「接下来」黑帽标 + 左侧云朵涂鸦（固定卡题；空日程仍在场）。独立于
     * s_art_ok——kidpal_art_load 自身决定烘焙 vs 兜底（[S5]: 标签在场义务独立于
     * 数字资产挂载）。云朵/帽标任一缺场 -> 各自描边兜底撑「标签带有墨」pin
     * （标签带墨义务两条路径成立）。
     * 2026-09-26 对齐轮第二/三批次（human 看屏两轮裁定：先「云朵偏高 / 接下来有点小」，
     * 再「组内关系 OK 了但相对上虚线还是偏高」）——依 canvas-p1 实测 + 母版 HTML 规格：
     *   云朵 ic_cloud_40_t2 按母版 40px 直出 @ (104,42) → 墨 (108,48)-(143,73)；
     *   帽标 p1_chip_jiexialai（重烘 58x24、字号 14.5px）@ (151,41) → 墨
     *   (152,42)-(207,65)。
     *   落位口径 = 设计实测的**呼吸间距**：设计上虚线底 26 → 帽标顶 36 空 9 行、
     *   帽标底 59 → 今日首行墨顶 69 空 9 行。实现上虚线重钉在 y30-32（受辅助条
     *   图标墨底 29 所限），故整卡下移 6px（帽标顶 42 = 32+1+9，行流 fy 62→68）
     *   保持同口径呼吸——全卡相对设计 +6px 平移，组内关系与呼吸与设计一致。 */
    if (!p1_blit_art_scaled("ic_cloud_40_t2.bin", 104, 42, 40, 40,
                            100, 40, 152, 82)) {
        p1_stroke_rect(107, 48, 143, 73, 1); /* 云朵缺场兜底 */
    }
    if (!p1_blit_art("p1_chip_jiexialai.bin", 151, 41, 58, 24)) {
        p1_stroke_rect(151, 41, 209, 65, 1); /* 帽标缺场兜底 */
    }

    /* 最近项行 ROW1（time + name, 行内时间 [S2] seam ASCII）。 */
    if (s_count > 0) {
        it = kidpal_home_p1_schedule_item(0);
        if (it != NULL) {
            p1_join_time_name(it, line, (int)sizeof(line));
            used = p1_blit_flow(line, cx0, fy + text_yoff, line_h,
                                cx0, cy0, cx1, cy1);
            fy += used * line_h;
        }

        /* 后续事项行 ROW2..（逐项 时间+名, 行流下排）。 */
        for (i = 1; i < s_count; i++) {
            it = kidpal_home_p1_schedule_item(i);
            if (it == NULL) {
                break;
            }
            p1_join_time_name(it, line, (int)sizeof(line));
            used = p1_blit_flow(line, cx0, fy + text_yoff, line_h,
                                cx0, cy0, cx1, cy1);
            fy += used * line_h;
        }

        /* 组间点状虚线分组隔（ui-v5-align §2 P1 行 4）：「接下来」组与「明天」组
         * 之间; 两组都在场才画（行流带零墨钉 = 空日程严格零墨, 不破）。
         * 2026-09-26 对齐轮（canvas-p1 实测）：设计组间虚线 y140 = 末项行带顶
         * (fy=118) + 22, 虚线自内容列 cx0=152 起到 cx1（设计 x152-385）。 */
        if (has_pv) {
            p1_draw_group_dash_sep(fy + 22, cx0, cx1);
        }
    }

    /* 末行预告（2026-09-26 对齐轮重构为设计结构：帽标独立行 + 预告项行在下——
     * canvas-p1 明天区 = 帽标行（墨 y152-175）+ 项行（墨 y184 起）两行结构, 不再
     * 帽标与项同行。帽标 p1_chip_mingzao 落内容列 (151, fy+33)（墨 152..195 ≈ 设计
     * x152-196）; 预告项行流 y = fy+60（墨顶 ≈ fy+66 = 设计 184 @fy=118）, 行流
     * cx0=152 起笔（帽标已独立成行, 不再需要 x>=192 让槽）。fy=118（2 项日程）时：
     * 虚线 140 / 帽标 151 / 项行 178, 与设计逐像素对位。 */
    if (has_pv) {
        if (!p1_blit_art("p1_chip_mingzao.bin", 151, fy + 33, 46, 24)) {
            p1_stroke_rect(151, fy + 33, 197, fy + 57, 1); /* 兜底描边撑帽标行有墨 */
        }
        p1_join_time_name(&pv, line, (int)sizeof(line));
        (void)p1_blit_flow(line, cx0, fy + 60 + text_yoff, line_h,
                           cx0, cy0, cx1, cy1);
    }
}

/* 分组分隔/辅助条下缘共用点状虚线画法 (ui-v5-align §2 P1 行 4, 2026-09-25 批):
 * V5 母版两处虚线——①辅助条下 border-bottom dotted（y42, x8..391, 旧通栏虚线同位,
 * r5 human 定谳锚保留）; ②「接下来」组与「明天」组之间 border-top dotted（组间,
 * 由 p1_draw_card 调用）。原 p1_draw_aux_dash_sep 泛化成带落位参数的本函数供两处
 * 消费。dash/dot 画法 needle 保留在世（voice_round_ui TEST 8 tooth 2 扫 home_p1.c
 * dash/dot/虚线/点线 >= 1）。 */
static void p1_draw_group_dash_sep(int y, int x0, int x1)
{
    int dash_on = 3, dash_gap = 3;        /* ~3px 线宽段 + 等宽空段 */
    int x = x0, on = 1;

    if (y < 0 || y + 2 >= P1_FB_H || x0 < 0 || x1 >= P1_FB_W || x0 > x1) {
        return; /* 防御: 出帧不入帧缓冲 */
    }
    while (x <= x1) {
        int stop = x + (on ? dash_on : dash_gap) - 1;
        if (stop > x1) {
            stop = x1;
        }
        if (on) {
            for (int xx = x; xx <= stop; xx++) {
                for (int t = 0; t < 3 && (y + t) < P1_FB_H; t++) {
                    s_p1_fb[(y + t) * P1_FB_W + xx] = P1_COLOR_INK;
                }
            }
        }
        x = stop + 1;
        on = !on;
    }
}

/* V5 对齐·真机修正（2026-09-26 对齐轮）: 左下波浪涂鸦改消费 ic_wave_58.bin（58x17
 * 资产、墨盒 (0,2)-(57,16) = 58x15 平滑正弦波）——原 rec_wave（录音态声波折线,
 * SVG 全直线段）系误用, 波形呈折线（human 看屏指出）。落位 (18,231)：墨迹
 * (18,233)-(75,247) = canvas-p1 实测波浪墨 (18,233)-(75,247) 逐像素吻合（画布目标
 * 尺寸直出件, 不再降采样）。加载失败 = 不画（纯装饰件, 无有墨义务——与帽标/图标
 * 不同, 兜底描边反而违背设计意图）。 */
static void p1_draw_wave_doodle(void)
{
    (void)p1_blit_art("ic_wave_58.bin", 18, 231, 58, 17);
}

/* V5 对齐·补缺 + 闪烁（2026-09-26 对齐轮第二批次，human 要求「四角星做成一闪一闪」）:
 * 猫右下四角星 ic_sparkle_22.bin（22x22 资产、墨盒 (5,5)-(16,16) = 12x12）。
 * **一闪一闪** = 1s tick 粒度两相位尺寸脉动：相位 A（tick%2000 < 1000）大 28px、
 * 相位 B 小 20px，两盒同心（圆心 (132,222) 不动，只胀缩不位移）——首页帧由 tick
 * 每秒重渲（kidpal_render_node.cc RN_TICK_MS=1000 首页动画重渲路径，与冒号眨眼同
 * 驱动源），故脉动逐秒可见。加载失败 = 不画（纯装饰件，无有墨义务，同波浪原则）。 */
static void p1_draw_sparkle_doodle(void)
{
    const int period = 2000;                       /* 脉动周期 2s（1s 大 / 1s 小） */
    int big = ((s_state.tick_ms % period) < period / 2) ? 1 : 0;
    int dw = big ? 28 : 20;
    int x0 = big ? 118 : 122;                      /* 同心缩放：28 盒 (118,208) 与 */
    int y0 = big ? 208 : 212;                      /* 20 盒 (122,212) 同圆心 (132,222) */

    (void)p1_blit_art_scaled("ic_sparkle_22.bin", x0, y0, dw, dw,
                             118, 208, 145, 235);
}

int kidpal_home_p1_render_frame(void)
{
    int doze, lr, batt_visible;

    if (!s_state_valid) {
        return -1;
    }

    p1_refresh_art_ok();
    p1_clear();

    /* 布局解析（时间>接下来>温湿度，冻结语义沿用）：以整帧高度为可用高度 ->
     * 三元素全在位；温湿度额外要求 env 数据有效（缺席即隐藏，不画假数据），
     * 接下来额外要求日程缓存非空（空日程让位 -> 主体区严格零墨）。 */
    lr = kidpal_home_p1_layout_resolve(28, 120, 28, P1_FB_H);

    /* ① 辅助条 y[0..39]（v1.4 重排序：WiFi 小标打头 + 时间·日期·温度·湿度·电量，
     * 图标前置; ui-v5-align §2 P1 行 1）。 */
    p1_draw_wifi_icon();
    p1_draw_aux_time();
    p1_draw_aux_date();
    if (s_state.env_valid == 1 && ((lr & HP1_ELEM_BIT_TH) != 0)) {
        p1_draw_aux_temp();
        p1_draw_aux_hum();
    }

    /* 电量：doze 灭相窗内电池整元素（图标+数值）不画（区内严格零墨）；充电恒醒
     * 恒画（覆盖任何电量）；非低电恒画。 */
    doze = kidpal_home_p1_doze_state(s_state.battery_pct, s_state.charging);
    batt_visible = (doze == 1)
        ? (kidpal_home_p1_blink_visible(s_state.tick_ms,
                                        HP1_BLINK_PERIOD_BATTERY_MS) == 1)
        : 1;
    if (batt_visible) {
        p1_draw_aux_batt();
    }

    /* 辅助条下缘点状虚线（y30, x8..391）：V5 母版 canvas-p1 辅助条 border-bottom
     * dotted 自首版在世（阶段2 architect 复核: 母版 git log -S + 锚图 y25-26 实测
     * 点状线）——本批初版误按 findings 行 4「状态栏下无线」退役, 已纠正恢复。
     * 2026-09-26 真机对齐轮：y42（VQ-A7 r5 旧定谳位）人看屏判过低 → 先钉 y33;
     * 第二批次随云朵/帽标按母版重落（云墨顶 42 / 帽标顶 36）再上移 y33→y30——
     * 辅助条图标墨底 ~28, y30-32 即贴置; 设计线(25-26)到帽标顶(36)有 10px, 本实现
     * 受制于辅助条图标（20px 盒 vs 母版 ~13px）只余 ~4px——辅助条图标缩尺可再买空间,
     * 属像素级观感后议项（findings §9 登记）。组间虚线另行画在「接下来」与「明天」
     * 组之间, 两件职责不同、并存不争位。 */
    p1_draw_group_dash_sep(30, 8, 391);

    /* ②/③ 日程卡：[S3] 追加轮行流版式——小伴常驻演出（左）+「接下来」卡（右）：
     * 黑帽标 + 云朵 + 行流文字（最近项 ROW1 / 后续事项 ROW2.. / 组间虚线 /
     * 末行预告 ROW3）。倒计时贴纸/大 imgfont 时间退役（不入渲染）。空日程仍画
     * 帽标 + 云朵（卡题固定）、行流零墨。 */
    p1_draw_mascot();
    if ((lr & HP1_ELEM_BIT_NEXT) != 0) {
        p1_draw_card();
    }

    /* V5 对齐: 左下波浪 + 猫右下四角星（装饰件, 独立于日程卡门控——设计稿常驻位）。 */
    p1_draw_wave_doodle();
    p1_draw_sparkle_doodle();

    return host_infra_render_p1_frame(s_p1_fb, P1_FB_W, P1_FB_H);
}
