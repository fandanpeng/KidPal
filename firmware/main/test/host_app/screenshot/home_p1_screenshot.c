/*
 * home_p1_screenshot.c — host 截图自查 harness（M1-N1b builder Part A）。
 *
 * 独立于现役 host runner 齿与 CMakeLists 接线，不进 runner 构建：直接
 *   #include 被测模块源文件（../../display/home_p1.c + ../../display/art_assets.c）
 * 取 statics（s_p1_fb）与全部绘制函数，main() 里 mount 真实资产目录
 * （firmware/main/assets/spiffs_root/），注入代表态，render_frame，把 400x300
 * 1bpp 语义帧 s_p1_fb 转 8-bit 灰度 BMP 写盘（手写 BMP 头，无外部依赖）。
 *
 * rationale: home_p1.c 走真字形真图标路径（kidpal_art_mount 挂真实目录）——与设备
 * SPIFFS 同路径，host 从此有真渲染覆盖；与冻结验收 p1_home_battery.acceptance.c
 * 的 include-home_p1.c 直取 statics 范式一致。
 *
 * 用法（WSL）：
 *   gcc -std=c11 -O1 -o /tmp/p1shot home_p1_screenshot.c \
 *       -I firmware/main/display -I firmware/main/test
 *   /tmp/p1shot [assets_root] [out_dir]
 *     assets_root 默认 /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/assets/spiffs_root
 *     out_dir     默认 .（写 home-*.bmp）
 *
 * 产 hue 5 张 BMP：normal / doze-hidden / doze-visible / charging / empty。
 * 由外部 python+PIL 转 PNG。
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../../display/home_p1.c"     /* s_p1_fb、P1_FB_W/H 静态、绘制函数、render_frame */
#include "../../../display/art_assets.c"  /* kidpal_art_mount/load/pixel/glyph/glyph_advance */

#define SHOT_W 400
#define SHOT_H 300

static const char *g_out_dir = ".";

/* 组装 out 目录下文件路径。 */
static void out_path(char *buf, size_t cap, const char *name)
{
    if (g_out_dir[0] == '\0' || strcmp(g_out_dir, ".") == 0) {
        snprintf(buf, cap, "%s", name);
    } else {
        snprintf(buf, cap, "%s/%s", g_out_dir, name);
    }
}

/* 把 s_p1_fb（uint32 白/黑语义）写成 8-bit 灰度 BMP（400x300，手写头）。 */
static int write_bmp(const char *name)
{
    char path[1024];
    FILE *f;
    int y, x;
    unsigned char pixel_row[SHOT_W];
    int pad = (4 - (SHOT_W * 1) % 4) % 4;

    out_path(path, sizeof(path), name);
    f = fopen(path, "wb");
    if (f == NULL) {
        perror("write_bmp fopen");
        return -1;
    }

    /* BITMAPFILEHEADER (14B) */
    unsigned char hdr[14];
    unsigned int data_size = (SHOT_W + pad) * SHOT_H;
    unsigned int file_size = 14 + 40 + 1024 + data_size;
    unsigned int pixel_off = 14 + 40 + 1024; /* = 1078: palette follows info header */
    hdr[0] = 'B'; hdr[1] = 'M';
    hdr[2] = (unsigned char)(file_size & 0xff); hdr[3] = (unsigned char)((file_size >> 8) & 0xff);
    hdr[4] = (unsigned char)((file_size >> 16) & 0xff); hdr[5] = (unsigned char)((file_size >> 24) & 0xff);
    hdr[6] = 0; hdr[7] = 0; hdr[8] = 0; hdr[9] = 0;            /* reserved */
    hdr[10] = (unsigned char)(pixel_off & 0xff);
    hdr[11] = (unsigned char)((pixel_off >> 8) & 0xff);
    hdr[12] = 0; hdr[13] = 0;
    fwrite(hdr, 1, 14, f);

    /* BITMAPINFOHEADER (40B): 8-bit biBitCount, grayscale palette */
    unsigned char ih[40];
    memset(ih, 0, sizeof(ih));
    ih[0] = 40;
    ih[4] = (unsigned char)(SHOT_W & 0xff); ih[5] = (unsigned char)((SHOT_W >> 8) & 0xff);
    ih[6] = 0; ih[7] = 0;
    ih[8] = (unsigned char)(SHOT_H & 0xff); ih[9] = (unsigned char)((SHOT_H >> 8) & 0xff);
    ih[10] = 0; ih[11] = 0;
    ih[12] = 1;  /* planes */
    ih[14] = 8;  /* biBitCount = 8 */
    /* biClrUsed @32 = 0 -> 8bpp uses all 256 palette entries */
    fwrite(ih, 1, 40, f);

    /* grayscale palette: index = gray value (0..255), 0=black, 255=white */
    {
        unsigned char pal[1024];
        for (int i = 0; i < 256; i++) {
            pal[i * 4 + 0] = (unsigned char)i;
            pal[i * 4 + 1] = (unsigned char)i;
            pal[i * 4 + 2] = (unsigned char)i;
            pal[i * 4 + 3] = 0;
        }
        fwrite(pal, 1, 1024, f);
    }

    /* pixel data, bottom-up rows. P1_COLOR_INK == 0 -> black(0); else (BG white) -> 255. */
    for (y = SHOT_H - 1; y >= 0; y--) {
        for (x = 0; x < SHOT_W; x++) {
            uint32_t px = s_p1_fb[y * SHOT_W + x];
            pixel_row[x] = (px == 0u) ? 0u : 255u;
        }
        fwrite(pixel_row, 1, SHOT_W, f);
        if (pad) {
            unsigned char z[4] = {0, 0, 0, 0};
            fwrite(z, 1, pad, f);
        }
    }
    fclose(f);
    printf("wrote %s\n", path);
    return 0;
}

/* 注入两项目程 + 明日预告（对应验收 hpb_inject_schedule_two_items 的 producer 结果；
 * 2026-10-11 seam 终形 5 参——preview 由单指针改 1 项数组 + 计数）。 */
static void inject_two_items(void)
{
    hp1_item_t items[2];
    hp1_item_t preview[1];

    memset(items, 0, sizeof(items));
    snprintf(items[0].time, sizeof(items[0].time), "14:00");
    snprintf(items[0].name, sizeof(items[0].name), "钢琴课");
    snprintf(items[1].time, sizeof(items[1].time), "16:30");
    snprintf(items[1].name, sizeof(items[1].name), "英语培训");
    memset(preview, 0, sizeof(preview));
    snprintf(preview[0].time, sizeof(preview[0].time), "09:00");
    snprintf(preview[0].name, sizeof(preview[0].name), "游泳课");
    kidpal_home_p1_schedule_inject("2026-09-15", items, 2, preview, 1);
}

/* 注入空日程（有效日键 + 0 项 + preview 未提供态 -1）。 */
static void inject_empty(void)
{
    kidpal_home_p1_schedule_inject("2026-09-15", NULL, 0, NULL, -1);
}

/* 设代表态（hour/minute/month/mday/env/temp/hum/battery/charging/tick）。
 * month/mday = 0（默认）时渲染兜底描边（无假日期）；其它态沿用既有月日。 */
static void set_state(int hour, int minute, int month, int mday, int env, int temp,
                      int hum, int batt, int charging, int tick)
{
    hp1_state_t st;
    memset(&st, 0, sizeof(st));
    st.hour = hour;
    st.minute = minute;
    st.month = month;
    st.mday = mday;
    /* 星期显式钉「未知」：memset 后的 0 会静默等于「周一」（wday 零值 = 周一 属结构
     * footgun），本 harness 不涉星期，显式 -1 走「不画假星期」退回路径（2026-09-26）。 */
    st.wday = -1;
    st.env_valid = env;
    st.temp_c = temp;
    st.humidity_pct = hum;
    st.battery_pct = batt;
    st.charging = charging;
    st.tick_ms = tick;
    if (kidpal_home_p1_set_state(&st) != 0) {
        printf("set_state rejected\n");
        exit(2);
    }
}

/* 辅助条逐元素墨带 bbox 诊断（M1-N1b 看屏终验自查）：对给定 x 窗统计 ink 的
 * ymin..ymax / xmin..xmax 与 y 中线，供报告对齐表。窗 = {name, x0, x1}。 */
static void bbox_win(const char *nm, int x0, int x1)
{
    int ymin = -1, ymax = -1, xmin = -1, xmax = -1, any = 0;
    for (int y = 0; y < 40; y++) { /* 辅助条 y[0..39] 带内；避免扫到下方日程主体墨 */
        for (int x = x0; x <= x1 && x < P1_FB_W; x++) {
            if (s_p1_fb[y * P1_FB_W + x] != (uint32_t)0xFFFFFFu) {
                any = 1;
                if (ymin < 0 || y < ymin) { ymin = y; }
                if (y > ymax) { ymax = y; }
                if (xmin < 0 || x < xmin) { xmin = x; }
                if (x > xmax) { xmax = x; }
            }
        }
    }
    if (!any) {
        printf("  %-10s x[%d..%d]  NO-INK\n", nm, x0, x1);
    } else {
        int yc = (ymin + ymax) / 2;
        int xc = (xmin + xmax) / 2;
        printf("  %-10s x[%d..%d] ink x[%d..%d] y[%d..%d] h=%d center=(%d,%d)\n",
               nm, x0, x1, xmin, xmax, ymin, ymax, ymax - ymin + 1, xc, yc);
    }
}

static void diagn_aux_bbox(void)
{
    printf("aux strip ink bbox (normal 10:24 / 22C / 45%% / 96%%):\n");
    bbox_win("time",      36, 108);
    bbox_win("date",     112, 208);
    bbox_win("temp_icon", 212, 232);
    bbox_win("temp_val",  236, 264);
    bbox_win("hum_icon",  268, 283);
    bbox_win("hum_val",   285, 305);
    bbox_win("hum_pct",   307, 318);
    bbox_win("batt_icon", 322, 344);
    bbox_win("batt_val",  349, 369);
    bbox_win("batt_pct",  370, 396);
    bbox_win("batt_pct_2", 349, 396); /* 数值+% 全窗（测整体中线） */
}

/* 对当前 s_p1_fb 统计辅助条各钉区墨像素数（C 侧，免 python 盲猜）。 */
static void diagn_aux_strip(void)
{
    static const int r_time[4]   = {  36, 6, 108, 33 };
    static const int r_date[4]   = { 112, 6, 208, 33 };
    static const int r_ticon[4]  = { 212, 6, 232, 33 };
    static const int r_tval[4]   = { 236, 6, 264, 33 };
    static const int r_hicon[4]  = { 268, 6, 288, 33 };
    static const int r_hval[4]   = { 292, 6, 318, 33 };
    static const int r_bicon[4]  = { 322, 6, 344, 33 };
    static const int r_bval[4]   = { 348, 6, 396, 33 };
    int ink;
    printf("s_art_ok=%d (via glyph_advance)\n", kidpal_art_glyph_advance() > 0);
    for (int r = 0; r < 8; r++) {
        static const char *nm[8] = {"time", "date", "temp_icon", "temp_val",
                                    "hum_icon", "hum_val", "batt_icon", "batt_val"};
        const int *R = (&r_time[0] == &r_time[0]) ?
            (r==0?r_time: r==1?r_date: r==2?r_ticon: r==3?r_tval:
             r==4?r_hicon: r==5?r_hval: r==6?r_bicon: r_bval) : 0;
        ink = 0;
        for (int y = R[1]; y <= R[3]; y++) {
            for (int x = R[0]; x <= R[2]; x++) {
                if (s_p1_fb[y * P1_FB_W + x] != (uint32_t)0xFFFFFFu) {
                    ink++;
                }
            }
        }
        printf("  %-10s ink=%d\n", nm[r], ink);
    }
}

int main(int argc, char **argv)
{
    const char *assets_root =
        "/mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/assets/spiffs_root";

    if (argc > 1) {
        assets_root = argv[1];
    }
    if (argc > 2) {
        g_out_dir = argv[2];
    }

    printf("mount: %s\n", assets_root);
    if (kidpal_art_mount(assets_root) != 0) {
        printf("ERROR: assets mount failed (path %s)\n", assets_root);
        return 3;
    }
    printf("glyph_advance() = %d\n", kidpal_art_glyph_advance());
    {
        kidpal_art_img_t probe;
        printf("load ic_batt_48.bin = %d\n", kidpal_art_load("ic_batt_48.bin", &probe));
    }

    /* 1) 常态 96%（有日程，env 有效，tick 亮相）。日期 9 月 16 日真实态 -> "16 Sep"。 */
    inject_two_items();
    set_state(10, 24, 9, 16, 1, 22, 45, 96, 0, 0);
    kidpal_home_p1_render_frame();
    diagn_aux_strip();
    diagn_aux_bbox();
    write_bmp("home-normal.bmp");

    /* 2) 低电 10% 灭相窗（tick=900，1s 周期可见窗 3/4 -> 灭相，电池整元素隐藏）。 */
    inject_two_items();
    set_state(10, 24, 9, 16, 1, 22, 45, 10, 0, 900);
    kidpal_home_p1_render_frame();
    write_bmp("home-doze-hidden.bmp");

    /* 3) 低电 10% 亮相窗（tick=0 -> 可见，电池显示）。 */
    inject_two_items();
    set_state(10, 24, 9, 16, 1, 22, 45, 10, 0, 0);
    kidpal_home_p1_render_frame();
    write_bmp("home-doze-visible.bmp");

    /* 4) 充电中（10% + charging=1 + 灭相窗 tick=900 -> 充电叫醒恒画）。 */
    inject_two_items();
    set_state(10, 24, 9, 16, 1, 22, 45, 10, 1, 900);
    kidpal_home_p1_render_frame();
    write_bmp("home-charging.bmp");

    /* 5) 空日程（主体让位零墨，辅助条照常）。 */
    inject_empty();
    set_state(10, 24, 9, 16, 1, 22, 45, 96, 0, 0);
    kidpal_home_p1_render_frame();
    write_bmp("home-empty.bmp");

    /* 6) 日期区特写（9月16日 -> "16 Sep" 独立出图，供 planner 复核日期渲染）。 */
    inject_two_items();
    set_state(9, 30, 9, 16, 1, 22, 45, 96, 0, 0);
    kidpal_home_p1_render_frame();
    write_bmp("home-date.bmp");

    return 0;
}