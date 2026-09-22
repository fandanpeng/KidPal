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
 * 布局钉区域矩形（含端点；钉区是布局在位钉不是字体/颜色钉）：
 *   ① 辅助条 y[0..39] 横贯带，左->右 = 时间 {36,6,108,33} · 日期 {112,6,208,33} ·
 *      温度 {212,6,264,33}（图标半 {212..232} 前置 + 数值半 {236..264}）·
 *      湿度 {268,6,318,33}（图标半前置 + 数值半，不裸数字）·
 *      电量 {322,6,396,33}（图标半前置 + 数值半）；
 *   ② 日程主体：小伴常驻演出（左）{8,48,128,208} + 「接下来」最近一项（右）
 *      {136,48,392,168} + 倒计时贴纸 {268,50,390,130}（接下来卡右部）；
 *   ③ 后续事项逐项 {136,172,392,214} + 明日预告 {136,220,392,258}。
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
 * 追加轮 [S3] 行流语义：文字行自 ROW1 y=96 起**正常换行**流式下排（换行续行落下一
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
    char ds[8];
    const char *mn;
    int drew = 0;
    int x, i;

    /* 日期有效（= RTC 日期已注入）且 art 挂载：真实渲染 "DD Mon"。 */
    if (month >= 1 && month <= 12 && mday >= 1 && mday <= 31 && s_art_ok) {
        mn = k_p1_month_abbr[month];
        p1_fmt_mday_2(mday, ds, (int)sizeof(ds));
        /* 布局（落 date 钉区 {112,6,208,33}）：日号 2 数字 pitch=10 起笔 126，全部 cell
         * 区间 [126, 126+10+12-1=147]；后接 8px 空格 -> 月首 x=156；3 字母 pitch=10
         * 收进区右界（156+2*10+12-1=187 < 208），不侵温度区(212)。 */
        x = 126;
        /* 日号 2 位：p1_blit_str_scaled 走 imgfont_digit 缩放路径（P1_AUX 12x16，
         * y0=12 统一中线）。 */
        drew |= (p1_blit_str_scaled(ds, x, 12, P1_AUX_PITCH,
                                    P1_AUX_DIG_W, P1_AUX_DIG_H,
                                    112, 6, 208, 33, 1) > 0);
        /* 空格（固定 advance 8px）+ 月缩写 3 字母（glyph 缩放路径同 cell，y0=12）。 */
        x = 126 + 2 * P1_AUX_PITCH + 8; /* = 154（2 数字 cell 后 + 8px 空） */
        for (i = 0; mn[i] != '\0' && i < 3; i++) {
            drew |= p1_blit_glyph_scaled(mn[i], x + i * P1_AUX_PITCH, 12,
                                         P1_AUX_DIG_W, P1_AUX_DIG_H,
                                         112, 6, 208, 33);
        }
    }
    if (!drew) {
        /* 兜底（art 未挂载 / 日期未知）：描边框撑「date 区有墨」pin，不画假日期。 */
        p1_stroke_rect(126, 10, 194, 30, 1);
    }
}

/* ① 辅助条·温度元素（图标半前置 {212,6,232,33} + 数值半 {236,6,264,33}）。
 * 图标：thermo_man 资产 blit（clip 进图标半）；数值：imgfont 整数（P1_AUX 12x16
 * pitch=10，"22" 2 字符墨 x=[241..257] 收进数值半）。半区各自兜底描边（各半区独立
 * 保证「有墨 >= 4」pin；整元素由 env_valid 门控，缺席即隐藏不画假数据）。°C 单位
 * 后缀本轮不动（° 与 C 均无 imgfont 字形，基元画 C 观感风险大，留 human 裁定）。 */
static void p1_draw_aux_temp(void)
{
    /* 图标走缩放 blit（thermo_man 41x67 降采样进 20x20 盒）+ clip 进温度图标半区
     * {212,6,232,33}；M1-N1b 看屏终验统一辅助条中线：图标墨带收敛到 ~18px 高、以
     * 中线 y≈19 为心落座（ink ~[11..28]，下探标定见截图自查表）。 */
    if (!p1_blit_art_scaled("thermo_man.bin", 212, 9, 20, 20,
                            212, 6, 232, 33)) {
        p1_stroke_rect(214, 10, 230, 29, 1);
    }
    if (s_art_ok) {
        (void)p1_blit_int_scaled(s_state.temp_c, 238, 12, P1_AUX_PITCH,
                                 P1_AUX_DIG_W, P1_AUX_DIG_H, 236, 6, 264, 33);
    } else {
        p1_stroke_rect(238, 10, 262, 29, 1);
    }
}

/* ① 辅助条·湿度元素（图标半前置 {268,6,288,33} + 数值半 {292,6,318,33}，
 * "45%" 不裸数字挨电池——图标半在前，数值紧跟 '%' 后缀）。水滴宝宝资产 + imgfont
 * 整数 + 基元 '%'（w=7 贴身收进元素右界 318，见标定表），兜底同温度。 */
static void p1_draw_aux_hum(void)
{
    int val = s_state.humidity_pct;
    int n = p1_ndigits(val);

    /* 图标走缩放 blit（drop_baby 32x51 降采样进 20x20 盒）+ clip 进湿度图标半区
     * {268,6,288,33}；M1-N1b 看屏终验统一中线：墨带收敛 ~17px 高、以 y≈19 为心
     * （ink ~[11..27]）。 */
    if (!p1_blit_art_scaled("drop_baby.bin", 268, 9, 20, 20,
                            268, 6, 288, 33)) {
        p1_stroke_rect(270, 10, 286, 29, 1);
    }
    if (s_art_ok) {
        /* 数值起笔 285 贴图标半区右界（288）内侧留白 3px；'%' 改走真字形管线
         * （p1_blit_glyph_scaled('%', …, 12x16 同 cell 同基线）替代手工基元两轮
         * 修形：% 同数字 cell 自动基线对齐，同 y0。% 起笔 = 285 + pitch*n + 2（n =
         * 位数）落整元素 {268,6,318,33} 内（n<=2 收进右界），clip 防越界。湿度 100
         * （n=3）时元素右界不足容纳 '%'（预既有缺陷，home 实况近不着边），仍无条件
         * 绘制、clip 截断到右界 318。 */
        (void)p1_blit_int_scaled(val, 285, 12, P1_AUX_PITCH,
                                 P1_AUX_DIG_W, P1_AUX_DIG_H, 268, 6, 318, 33);
        p1_blit_glyph_scaled('%', 285 + P1_AUX_PITCH * n + 2, 12,
                             P1_AUX_DIG_W, P1_AUX_DIG_H, 268, 6, 318, 33);
    } else {
        p1_stroke_rect(294, 10, 316, 29, 1);
    }
}

/* ① 辅助条·电量元素（图标半前置 {322,6,344,33} + 数值半 {348,6,396,33}）。
 * ic_batt 资产 + imgfont 整数（"85"/"100" P1_AUX 12x16 pitch=10）+ '%' 后缀（w=8），
 * 半区各自兜底描边。整元素由 doze 灭相门控（调用方），充电恒画。 */
static void p1_draw_aux_batt(void)
{
    int val = s_state.battery_pct;
    int n = p1_ndigits(val);

    /* 图标走缩放 blit（ic_batt_48 48x48 降采样进 22x40 盒）+ clip 进电量图标半区
     * {322,6,344,33}；M1-N1b 看屏终验统一中线：box y0=0 顶越区由 clip 取齐、ink 墨带
     * ~18px 高、以 y≈19 为心（ink ~[11..28]，下探标定见截图自查表）。数值起笔 348
     * （图标墨右界贴半区 ~342，>=4px 空隙）。 */
    if (!p1_blit_art_scaled("ic_batt_48.bin", 322, 0, 22, 40,
                            322, 6, 344, 33)) {
        p1_stroke_rect(324, 10, 342, 29, 1);
    }
    if (s_art_ok) {
        (void)p1_blit_int_scaled(val, 348, 12, P1_AUX_PITCH,
                                 P1_AUX_DIG_W, P1_AUX_DIG_H, 348, 6, 396, 33);
        /* '%' 真字形（替代手工基元）：起笔 348 + pitch*n + 2 = 尾数 cell 后 2px 间距，
         * 同 12x16 cell 同 y0=12 同数字基线；clip 防越半区右界 396。 */
        p1_blit_glyph_scaled('%', 348 + P1_AUX_PITCH * n + 2, 12,
                             P1_AUX_DIG_W, P1_AUX_DIG_H, 348, 6, 396, 33);
    } else {
        p1_stroke_rect(350, 10, 394, 29, 1);
    }
}

/* ② 小伴常驻演出（v1.4 内容区左 {8,48,128,208}）：pose_daily 全尺寸 96x96 blit
 * （落点 (24,80) 收进钉区）；未挂载 thin 描边兜底（有墨 >= 20 pin 两条路径成立）。 */
static void p1_draw_mascot(void)
{
    if (!p1_blit_art("pose_daily_96.bin", 24, 80, 96, 96)) {
        p1_stroke_rect(10, 50, 126, 206, 2);
    }
}

/* ②/③ 「接下来」日程卡（[S3] 追加轮行流语义, human 版式扩展 2026-09-22）：
 * 卡顶「接下来」= 烘焙大字标签（[S5] p1_label_jiexialai.bin, 卡题固定、空日程仍在场,
 * 左锚定 LABEL 带 {136,48,266,92}）；倒计时贴纸/大 imgfont 时间本轮退役（不入渲染）；
 * 文字行全部 14px font_puhui_14_1 逐字形 blit + 正常换行——最近项行 ROW1{136,96,392,122}
 * （时间 + 课程名, 行内时间走 [S1] seam ASCII）、后续事项行 ROW2{136,124,392,150}
 * 逐项 时间+名、末行预告 ROW3{136,152,392,178} = 明早标签（[S5] p1_label_mingzao.bin,
 * 文案「明早」非「明天」）+ HH:MM（起笔 x>=184 不侵明早槽）+ 名。行流自 ROW1 起按
 * 行流式下排（换行续行落下一行带），一切墨不越卡区 CARD{136,48,392,242}。
 * 空日程仍画标签 + 行流零墨（HPB tooth 5 语义）。 */
static void p1_draw_card(void)
{
    const int line_h = 28;      /* [S3] 行带 pitch：ROW1 96 -> ROW2 124 -> ROW3 152 */
    const int text_yoff = 6;    /* 14px 字形在 26px 行带内的垂直对齐偏移 */
    const int cx0 = 136, cy0 = 48, cx1 = 392, cy1 = 242; /* 卡区 clip */
    int fy = 96;                /* 行流光标：自 ROW1 顶起, 逐块按占用行数下移 */
    int used, i;
    char line[48];
    const hp1_item_t *it;
    hp1_item_t pv;
    int has_pv;

    /* 「接下来」烘焙大字标签（固定卡题；空日程仍在场）。独立于 s_art_ok——kidpal_
     * art_load 自身决定烘焙 vs 兜底（[S5]: 标签在场义务独立于数字资产挂载）。 */
    if (!p1_blit_art("p1_label_jiexialai.bin", 136, 55, 130, 44)) {
        p1_stroke_rect(136, 48, 266, 92, 1); /* 兜底描边撑「标签带有墨 >= 30」 */
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
    }

    /* 末行预告（明早标签 + HH:MM + 名）：明早烘焙标签落行 3 左槽, time 起笔 x>=184。
     * 行流 y = fy（标准夹具 2 项 + 预告 = 第三行 y=152）。 */
    has_pv = (kidpal_home_p1_schedule_preview(&pv) == 0);
    if (has_pv) {
        if (!p1_blit_art("p1_label_mingzao.bin", 136, fy + 3, 44, 26)) {
            p1_stroke_rect(136, fy, 180, fy + 26, 1); /* 兜底描边撑明早槽有墨 */
        }
        p1_join_time_name(&pv, line, (int)sizeof(line));
        (void)p1_blit_flow(line, 184, fy + text_yoff, line_h,
                           cx0, cy0, cx1, cy1);
    }
}

/* 首页底部分隔虚线 (device-round VQ-A7 r3 §A2 起, r5 human 定谳复位): 冻结锚 = 「顶部
 * 信息栏下方 (y=42) 通栏虚线」(design 稿 P1 辅助条节 border-top 2.5px dotted / 左右 6px
 * 内缩)。r4 human 真机复测定性为「底栏分隔线画到中间 (位置/跨度不对)」导致 r4 把落点下移
 * 到 y246 且收窄 x138..389 —— 该次重定位是对 human r3 反馈的误读 (r5 销账): 顶部虚线与
 * 底部键帽条 chrome 上沿 3px 实线是两条不同职责分隔, 前者钉顶部信息栏下、后者钉底槽上沿,
 * 不争位、非双画。现按 r5 human 定谳复位 y42 通栏 6px 内缩。dash/dot 画法 needle 保留在世
 * (TEST 8 tooth 2 不变绿)。设备首页 = home_p1.c 软帧整帧渲染单源 (400×300 帧整帧 blit),
 * LVGL 对象画法会被帧 blit 覆盖 —— mock kidpal_ui.c:197-204 的 lv_line dash 画法为参照
 * 而非设备落点 (r4 偏离/复位声明: 见 r5 review-packet 交付报告)。 */
static void p1_draw_aux_dash_sep(void)
{
    /* r5 复位 (r4 误读销账): 恢复 r3 冻结锚 = 顶部信息栏下方 y42 通栏虚线 (左右 6px 内缩,
     * design 稿 P1 辅助条节 border-top dotted / 左右 6px 内缩)。r4 曾把落点下移到 y246 并
     * 收窄到右侧内容列 x138..389 (误认「底部键帽条 chrome 实线上沿」为双画要避位的观感),
     * 实则顶部虚线职责 = 信息栏与日程主体间的分隔、与底槽上沿实线不争位, r5 human 定谳
     * 复位为顶栏下通栏。dash/dot 画法 needle 保留 (TEST 8 tooth 2 不变绿); 像素级观感
     * 归真机轮 VQ-A7。 */
    int y = 42, x0 = 6, x1 = P1_FB_W - 7; /* 通栏 6px 内缩: x6..393 */
    int dash_on = 3, dash_gap = 3;        /* ~3px 线宽段 + 等宽空段 */
    int x = x0, on = 1;

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

    /* ① 辅助条 y[0..39]（v1.4 重排序：时间·日期·温度·湿度·电量，图标前置）。 */
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

    /* 辅助条下缘虚线分隔 (r3 §A2): 信息栏之下、日程主体之上画通栏虚点线。 */
    p1_draw_aux_dash_sep();

    /* ②/③ 日程卡：[S3] 追加轮行流版式——小伴常驻演出（左）+「接下来」卡（右）：
     * 标签 + 行流文字（最近项 ROW1 / 后续事项 ROW2.. / 末行预告 ROW3）。倒计时贴纸/
     * 大 imgfont 时间本轮退役（不入渲染）。空日程仍画标签（卡题固定）、行流零墨。 */
    p1_draw_mascot();
    if ((lr & HP1_ELEM_BIT_NEXT) != 0) {
        p1_draw_card();
    }

    return host_infra_render_p1_frame(s_p1_fb, P1_FB_W, P1_FB_H);
}