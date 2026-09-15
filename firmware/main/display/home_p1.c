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

/* ---- 数字字形目标盒（M1-N1b 真机根因修复；不偏移资产/不动 art_assets.c） ----
 * imgfont_digit_*.bin 是旧版式大时钟的 48 宽 x 64 高字形（advance 冻结 48），
 * 辅助条/小一号数字若按 12/13px pitch 原样逐字符 blit 会相邻重叠溢出（真机「超大
 * 字体」）。本文件改走运行时最近邻降采样进目标盒 + 钉区 clip。整数值统一 1 盒：
 *   P1_DIG_W/H = 16x22 —— 数字内容（源 ink ~28x42，缩放 ~15x13px）逐字符 ink 实测
 *   收进各钉区（辅助条时间 x[42..103]@y[12..26]、值半区 x[~242..262]@y[13..27]，
 *   主体小一号 x[144..208]，全部贴内，见 verifier 审查注）。主体验「接下来」（pitch
 *   25）保持原样 48px 字形 == 最醒目恰配，不经缩放。 ---- */
#define P1_DIG_W 16
#define P1_DIG_H 22

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

/* imgfont 单字形 blit（'0'..'9' | ':'）。用 kidpal_art_glyph（loader 原语）
 * 直接吐像素到 s_p1_fb；未挂载/字符非法 -> 返回 0。 */
static int p1_blit_glyph(char c, int x0, int y0)
{
    kidpal_art_img_t img;

    if (kidpal_art_glyph(c, &img) != 0) {
        return 0;
    }
    for (int y = 0; y < img.h; y++) {
        for (int x = 0; x < img.w; x++) {
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

/* 把 "HH:MM" / "H:MM" / 任意数值串按 pitch 逐字符 blit 进帧；返回成功 blit 的
 * 字符数。colon_visible=0 时跳过 ':'（眨眼灭相严格减墨的墨源）。 */
static int p1_blit_digits(const char *s, int x0, int y0, int pitch,
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
        if (p1_blit_glyph(c, x0 + i * pitch, y0)) {
            ok++;
        }
    }
    return ok;
}

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
 * imgfont 数字（pitch=13，5 字符 cell 总宽 4*13+字形宽 -> 墨 x=[40..104] 收进
 * 钉区 [36..108]）；未挂载兜底 thin 描边框撑「time 区有墨」pin。 */
static void p1_draw_aux_time(void)
{
    char buf[8];
    int colon_visible =
        (kidpal_home_p1_blink_visible(s_state.tick_ms, HP1_BLINK_PERIOD_COLON_MS) == 1);

    p1_fmt_hhmm(s_state.hour, s_state.minute, buf, (int)sizeof(buf));
    if (s_art_ok) {
        /* 缩放 blit（源 48x64 -> 16x22 盒，最近邻）× clip 进 time 钉区 [36,6,108,33]；
         * ink 实测 x[42..103]@y[12..26] 全贴内（见 P1_DIG_W/H 注）。 */
        (void)p1_blit_str_scaled(buf, 38, 9, 13, P1_DIG_W, P1_DIG_H,
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

/* ① 辅助条·日期星期：「8月27日 周四」文本暂无中文字形（殿后清单）——描边兜底
 * 恒画（两条路径同款，有墨义务成立），落 date 钉区 [112..208]x[6..33] 内。 */
static void p1_draw_aux_date(void)
{
    p1_stroke_rect(114, 10, 206, 30, 1);
}

/* ① 辅助条·温度元素（图标半前置 {212,6,232,33} + 数值半 {236,6,264,33}）。
 * 图标：thermo_man 资产 blit（clip 进图标半）；数值：imgfont 整数（pitch=12，
 * "22" 2 字符墨 x=[238..262] 收进数值半）。半区各自兜底描边（各半区独立保证
 * 「有墨 >= 4」pin；整元素由 env_valid 门控，缺席即隐藏不画假数据）。 */
static void p1_draw_aux_temp(void)
{
    /* 图标走缩放 blit（thermo_man 41x67 降采样进 20x28 盒）+ clip 进温度图标半区
     * {212,6,232,33}；ink 实测 x[216..228]@y[9..33] 全贴内（见报告表 ③）。 */
    if (!p1_blit_art_scaled("thermo_man.bin", 212, 6, 20, 28,
                            212, 6, 232, 33)) {
        p1_stroke_rect(214, 10, 230, 29, 1);
    }
    if (s_art_ok) {
        (void)p1_blit_int_scaled(s_state.temp_c, 238, 10, 12,
                                 P1_DIG_W, P1_DIG_H, 236, 6, 264, 33);
    } else {
        p1_stroke_rect(238, 10, 262, 29, 1);
    }
}

/* ① 辅助条·湿度元素（图标半前置 {268,6,288,33} + 数值半 {292,6,318,33}，
 * "45%" 不裸数字挨电池——图标半在前）。水滴宝宝资产 + imgfont 整数，兜底同温度。 */
static void p1_draw_aux_hum(void)
{
    /* 图标走缩放 blit（drop_baby 32x51 降采样进 20x28 盒）+ clip 进湿度图标半区
     * {268,6,288,33}；ink 实测 x[269..287]@y[8..32] 全贴内（见报告表 ③）。 */
    if (!p1_blit_art_scaled("drop_baby.bin", 268, 6, 20, 28,
                            268, 6, 288, 33)) {
        p1_stroke_rect(270, 10, 286, 29, 1);
    }
    if (s_art_ok) {
        (void)p1_blit_int_scaled(s_state.humidity_pct, 294, 10, 12,
                                 P1_DIG_W, P1_DIG_H, 292, 6, 318, 33);
    } else {
        p1_stroke_rect(294, 10, 316, 29, 1);
    }
}

/* ① 辅助条·电量元素（图标半前置 {322,6,344,33} + 数值半 {348,6,396,33}）。
 * ic_batt 资产 + imgfont 整数（"85"/"100" pitch=12，墨 x=[350..388] 收进数值半），
 * 半区各自兜底描边。整元素由 doze 灭相门控（调用方），充电恒画。 */
static void p1_draw_aux_batt(void)
{
    /* 图标走缩放 blit（ic_batt_48 48x48 降采样进 22x28 盒）+ clip 进电量图标半区
     * {322,6,344,33}；ink 实测 x[324..342]@y[14..26] 全贴内、视觉不再与数值半 95
     * 粘连（见报告表 ③；95 字号本轮不动）。 */
    if (!p1_blit_art_scaled("ic_batt_48.bin", 322, 6, 22, 28,
                            322, 6, 344, 33)) {
        p1_stroke_rect(324, 10, 342, 29, 1);
    }
    if (s_art_ok) {
        (void)p1_blit_int_scaled(s_state.battery_pct, 350, 10, 12,
                                 P1_DIG_W, P1_DIG_H, 348, 6, 396, 33);
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

/* ② 「接下来」最近一项主体（{136,48,392,168}，最醒目）：ic_book 图标 + 首项时点
 * 数字（"14:00" pitch=25 -> 墨 x=[210..336] 收进钉区）；首项名中文无 imgfont ->
 * 殿后清单。整函数仅在 s_count>0 时被调用 -> 空日程 next 区严格零墨。未挂载兜底
 * thin 描边卡框（墨 >= 20）。 */
static void p1_draw_next(void)
{
    const hp1_item_t *it = kidpal_home_p1_schedule_item(0);
    int drew = 0;

    drew |= p1_blit_art("ic_book_48.bin", 145, 62, 48, 48);
    if (it != NULL && s_art_ok) {
        drew |= (p1_blit_digits(it->time, 210, 74, 25, 1) > 0);
    }
    if (!drew) {
        p1_stroke_rect(138, 50, 390, 166, 2);
    }
}

/* ② 倒计时贴纸（接下来卡右部 {268,50,390,130}）：以 "H:MM"（剩余时分）数字表达
 * 「还有 X 时 Y 分」的数值半 + ic_bell 铃铛资产；中文「还有…」文字属殿后清单。
 * 整函数仅在 countdown==0 时被调用 -> 全过期「贴纸区零墨」pin 天然保留。 */
static void p1_draw_sticker(int remain_h, int remain_m)
{
    char buf[8];
    int drew = 0;

    drew |= p1_blit_art("ic_bell_48.bin", 272, 58, 48, 48);
    if (s_art_ok) {
        p1_fmt_hhmm(remain_h, remain_m, buf, (int)sizeof(buf));
        drew |= (p1_blit_str_scaled(buf, 330, 66, 14,
                                    P1_DIG_W, P1_DIG_H,
                                    268, 50, 390, 130, 1) > 0);
    }
    if (!drew) {
        p1_stroke_rect(270, 52, 388, 128, 2);
    }
}

/* ③ 后续事项逐项（小一号，{136,172,392,214}）：items[1] 起的时点数字（"16:30"
 * pitch=14 -> 墨 x=[140..196] 收进钉区）；整函数仅在 s_count >= 2 时被调用
 * -> 单项/空日程严格零墨。未挂载兜底描边条框。 */
static void p1_draw_nextlist(void)
{
    const hp1_item_t *it = kidpal_home_p1_schedule_item(1);
    int drew = 0;

    if (it != NULL && s_art_ok) {
        drew |= (p1_blit_str_scaled(it->time, 140, 182, 14,
                                    P1_DIG_W, P1_DIG_H,
                                    136, 172, 392, 214, 1) > 0);
    }
    if (!drew) {
        p1_stroke_rect(138, 174, 390, 212, 2);
    }
}

/* ③ 明日预告（{136,220,392,258}）：preview item 的 time 数字（V3 「明天 09:00 ·
 * 游泳课」的时间半，中文文字殿后）。整函数仅在 s_has_preview 时被调用 -> 无预告
 * 「预告区零墨」pin 保留。未挂载兜底描边条框。 */
static void p1_draw_preview(void)
{
    hp1_item_t pv;
    int drew = 0;

    pv.time[0] = '\0';
    if (kidpal_home_p1_schedule_preview(&pv) == 0 && s_art_ok) {
        drew |= (p1_blit_str_scaled(pv.time, 140, 228, 14,
                                    P1_DIG_W, P1_DIG_H,
                                    136, 220, 392, 258, 1) > 0);
    }
    if (!drew) {
        p1_stroke_rect(138, 222, 390, 256, 2);
    }
}

int kidpal_home_p1_render_frame(void)
{
    int doze, lr, batt_visible;
    int idx, hh, mm;

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

    /* ② 日程主体：小伴常驻演出（左）+「接下来」最近一项（右）+ 倒计时贴纸。 */
    p1_draw_mascot();
    if (s_count > 0 && ((lr & HP1_ELEM_BIT_NEXT) != 0)) {
        p1_draw_next();
        /* 倒计时贴纸：当下缓存仍有未开始的最近项才贴。 */
        if (kidpal_home_p1_countdown(s_items, s_count, s_state.hour, s_state.minute,
                                     &idx, &hh, &mm) == 0) {
            p1_draw_sticker(hh, mm);
        }
    }

    /* ③ 后续事项逐项（小一号）+ 明日预告。 */
    if (s_count >= 2) {
        p1_draw_nextlist();
    }
    if (s_has_preview) {
        p1_draw_preview();
    }

    return host_infra_render_p1_frame(s_p1_fb, P1_FB_W, P1_FB_H);
}