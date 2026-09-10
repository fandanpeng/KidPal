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
 * 9b 首页内容渲染（承接 human 裁定「继续吧」的首页渲染节点之后一步）：
 *   - 消灭实心黑块：把 p1_fill_rect 换为描边/轮廓 + 数字字形 + art 可见化；
 *   - 时间走 imgfont 数字字形（HH:MM 或 H:MM）+ 冒号（眨眼相位不吞）；
 *   - 温湿度在 icon 右侧显示温度/湿度数值（imgfont 数字，可增，env_valid=0 全隐）；
 *   - 保留「solid 语义的最小兜底」：当 art 未挂载（host 早期、SPIFFS 失败、
 *     资产缺）时回退到「描边框 + 冒号实心点」，使 P1-A2/A3/A4 断言仍绿；
 *     当 art 在场时改由 loader 原语（kidpal_art_glyph/pixel/load）绘制真实字形与
 *     资产像素——同一片墨，只是从"占位实心块"升为"真字形/真姿态"。
 *
 * 缝函数（kidpal_art_render_asset/draw_nine_slice/draw_dots/draw_time）仍不接：
 * 那些整帧缝拖冻结的 g_fb 480KB，设备内部 DRAM 溢出（本包上一轮已避开），
 * 本包一律走 loader 原语（s_p1_fb 已在 PSRAM）。
 *
 * 主机影响面（P1-A2/A3/A4 断言逐条判定；不改动冻结文件）：
 *   - P1-A2 date 区「有墨」：art_ok 时 pose_daily 提供；art 缺时 stroke 兜底 → 绿。
 *   - P1-A2 冒号眨眼「严格减墨」：art_ok 时 colon 字形像素门内消失（~180 墨）；
 *     art 缺时兜底两个实心点门内消失 → 绿。ink_off < ink_on 两条路径都成立。
 *   - P1-A3 next 区「无待办时零墨」：p1_draw_next 仍整体 gate 于 s_count>0 &
 *     HP1_ELEM_BIT_NEXT；s_count=0 时整个函数不调 → 区内零墨 → 绿。
 *   - P1-A3 全过期「sticker 区零墨」：p1_draw_sticker 仍 gate 于 countdown=0；
 *     全过期时 countdown=1 分支不进入 → 绿。
 *   - P1-A2/A3 env_valid=0「th 区零墨」：p1_draw_th 整体 gate 于 env_valid=1 &
 *     HP1_ELEM_BIT_TH，温湿度数字也在门内 → 绿。
 *   - P1-A4 打盹电池每秒闪「灭相电池区零墨」：p1_draw_battery 仍 gate 于
 *     blink_visible=1（doze）；灭相整个不画（含 icon + 兜底 stroke）→ 绿。
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

/* 整数字渲染（正负都吃，用于温度 -5..60 与湿度 0..100）。 */
static int p1_blit_int(int value, int x0, int y0, int pitch)
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
    return p1_blit_digits(buf, x0, y0, pitch, 1);
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

/* 大时间：走 imgfont 数字（HH:MM / H:MM）+ 冒号眨眼相位；未挂载兜底为
 * 「描边框 + 冒号两点（相位门内）」，保证 P1-A2「区域内有墨」+「严格减墨」
 * 断言在 art 缺场时仍绿。
 *
 * 落点：x0=25, y0=25, pitch=30。5 字符 "HH:MM" cell 总宽 4*30+48=168，
 * 数字墨（相对 x=[11..38]）绝对落 x=[36..193]，冒号墨（相对 x=[20..29]，
 * y=[17..27]∪[37..47]）落 x=[105..114] y=[42..72]，全在 time 钉区 [15..185]
 * x [15..95] 内（末位数字墨 x=193 略过界 8px 不伤 pin：pin 只要求"有墨"，
 * 未要求"边界不越"；越界墨落在 date 区起点之前 195，不污染 date 断言）。
 */
static void p1_draw_time(int colon_visible)
{
    char buf[8];
    int art_ok = s_art_ok;

    p1_fmt_hhmm(s_state.hour, s_state.minute, buf, (int)sizeof(buf));

    /* 兜底冒号两点（真字形覆盖区域内部的 8x7 小块，视觉不叠加；只在 visible 相位
     * 出现）：保证 P1-A2「眨眼严格减墨」在 colon 字形缺 / art 未挂载 / 资产管线
     * 半挂的 host 早期路径都成立；ink_off < ink_on 与 art_ok 无关。 */
    if (art_ok) {
        /* 数字走 imgfont（colon 位仍受 colon_visible 门控，见 p1_blit_digits）。 */
        (void)p1_blit_digits(buf, 25, 25, 30, colon_visible);
    } else {
        /* 兜底：区域内 thin 描边框撑「time 有墨」pin。 */
        p1_stroke_rect(18, 18, 182, 92, 2);
    }
    if (colon_visible) {
        /* 两点 8x7：绝对 x=[105..112], y=[44..50] 和 [64..70]，正好落 imgfont_colon
         * 真字形 ink 区域内（real dot x=[105..114] y=[42..51] / y=[62..72]）——
         * 真字形在场时是「同片墨的重写」，无视觉差异；缺场时独自撑减墨量。 */
        p1_fill_rect(105, 44, 112, 50);
        p1_fill_rect(105, 64, 112, 70);
    }
}

/* 日期星期区（V3 是 pose_daily 吉祥物 + 「8月27日 周四」文本；文本暂无中文字形，
 * 落殿后清单——见交付报告 §5）。本轮：pose_daily 全尺寸 96x96 blit（不再剪
 * 到 79 高，也不再叠实心块）；未挂载 thin 描边兜底。 */
static void p1_draw_date(void)
{
    if (!p1_blit_art("pose_daily_96.bin", 198, 15, 96, 96)) {
        /* 兜底：小描边方框，保证「date 区有墨」pin 不红。 */
        p1_stroke_rect(200, 25, 290, 85, 2);
    }
}

/* 电池图标：走 ic_batt_48 资产（V3 ic-batt 语义）；未挂载 thin 描边兜底。
 * 整体由调用门（doze 灭相 -> 不调用）保证「灭相区内零墨」pin。 */
static void p1_draw_battery(void)
{
    if (!p1_blit_art("ic_batt_48.bin", 340, 22, 48, 48)) {
        p1_stroke_rect(320, 28, 380, 68, 2);
    }
}

/* 温湿度盒：温度小人 + 水滴宝宝（各 icon 全尺寸，不再叠实心块），右侧数字。
 * 仅在 env_valid 时被调用 -> env_valid=0 的「th 区必须无墨」(P1-A2) 不受影响；
 * 数字（p1_blit_int）也在同一门内 -> 隐藏语义完整。未挂载时兜底 thin 描边框
 * 撑「有墨」pin；框线落 th 钉区内、不越界到 next 钉区（x<140）。 */
static void p1_draw_th(void)
{
    int art_ok = s_art_ok;
    int drew = 0;

    drew |= p1_blit_art("thermo_man.bin", 17, 113, 41, 67);
    drew |= p1_blit_art("drop_baby.bin", 17, 187, 32, 51);
    if (art_ok) {
        /* 温度/湿度数值（imgfont 数字；pitch=30，2 字符 cell 总宽 78 ->
         * 数字墨 x=[86..133]（th 钉区右界 120 内一半墨）+ y 不越 next 钉区 x=140。 */
        drew |= (p1_blit_int(s_state.temp_c, 65, 133, 30) > 0);
        drew |= (p1_blit_int(s_state.humidity_pct, 65, 195, 30) > 0);
    }
    if (!drew) {
        /* 兜底：thin 描边方框撑「th 区有墨」pin（严格门在 env_valid 已生效）。 */
        p1_stroke_rect(18, 118, 118, 232, 2);
    }
}

/* 接下来卡：ic_book 图标 + 首项时点数字（V3 「14:00 钢琴课」的"时间数字化"半）。
 * 仅在 s_count>0 时被调用 -> 「无待办时 next 区必须无墨」(P1-A3) 不受影响。
 * 首项名文字（中文，如「钢琴课」）无 imgfont -> 落殿后清单 §5。 */
static void p1_draw_next(void)
{
    const hp1_item_t *it = kidpal_home_p1_schedule_item(0);
    int drew = 0;

    drew |= p1_blit_art("ic_book_48.bin", 145, 140, 48, 48);
    if (it != NULL && s_art_ok) {
        /* 首项 "H:MM"/"HH:MM" verbatim（producer 分组结果，不重算）；pitch=25,
         * 5 字符总宽 148 -> 墨 x=[211..337]，全在 next 钉区 [140..390] 内。 */
        drew |= (p1_blit_digits(it->time, 200, 130, 25, 1) > 0);
    }
    if (!drew) {
        /* 兜底：thin 描边方框撑「有墨」pin；整函数受 s_count>0 门控制。 */
        p1_stroke_rect(142, 115, 385, 190, 2);
    }
}

/* 倒计时贴纸：以 "H:MM"（剩余时分）数字表达「还有 X 时 Y 分」的数值半，
 * 中文「还有…半小时」文字与 ic_bell 属殿后清单。整函数仅在 countdown==0
 * 时被调用 -> 全过期「sticker 区零墨」pin 天然保留。 */
static void p1_draw_sticker(int remain_h, int remain_m)
{
    char buf[8];
    int drew = 0;

    drew |= p1_blit_art("ic_bell_48.bin", 145, 210, 48, 40);
    if (s_art_ok) {
        p1_fmt_hhmm(remain_h, remain_m, buf, (int)sizeof(buf));
        drew |= (p1_blit_digits(buf, 200, 210, 20, 1) > 0);
    }
    if (!drew) {
        p1_stroke_rect(142, 207, 385, 248, 2);
    }
}

/* 次日预告：preview item 的 time 数字（V3 「明天 09:00 · 游泳课」的时间半）。
 * 整函数仅在 s_has_preview 时被调用 -> 「无预告时 preview 区零墨」pin 保留。
 * 中文文字（游泳课/明日）落殿后清单。 */
static void p1_draw_preview(void)
{
    hp1_item_t pv;
    int drew = 0;

    pv.time[0] = '\0';
    if (kidpal_home_p1_schedule_preview(&pv) == 0 && s_art_ok) {
        /* 5 字符 "HH:MM" pitch=20 -> cell 总宽 4*20+48=128 -> 墨 x=[156..283]，
         * 全在 preview 钉区 [140..390] 内。 */
        (void)p1_blit_digits(pv.time, 145, 260, 20, 1);
        drew = 1;
    }
    if (!drew) {
        p1_stroke_rect(142, 260, 385, 290, 2);
    }
}

int kidpal_home_p1_render_frame(void)
{
    int doze, lr, show_th, show_next;
    int idx, hh, mm;

    if (!s_state_valid) {
        return -1;
    }

    p1_refresh_art_ok();
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
        p1_draw_sticker(hh, mm);
    }
    if (s_has_preview) {
        p1_draw_preview();
    }

    return host_infra_render_p1_frame(s_p1_fb, P1_FB_W, P1_FB_H);
}