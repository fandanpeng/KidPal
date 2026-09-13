/*
 * happy_flow_capture.test.c — Happy Flow 全页面 host 截图驱动（普通 `.test.c`，
 * 非冻结验收，HOST_TEST_CONVENTIONS [C4.3]）。
 *
 * 目的（human 一次性授权 2026-09-13）：把 Happy Flow 涉及的、host runner 已有渲染面
 * 的页面逐页真渲染（与冻结验收同源的同一条 LVGL 软渲染路径），经包 0 渲染缝的
 * additive 落盘函数 host_infra_dump_bmp()（host_infra_seam.c）把整帧写成 24-bit BMP，
 * 供「页面观感验证记录」落 changes/dev/260913-happy-flow-host-capture/。
 *
 * 范围（Happy Flow 中 host 有渲染面的页面）：
 *   P1 首页 / P2 课表（今天 + 明天两态）/ P3 故事（列表/阅读两屏/录音/盖章）/ P4
 *   查生字（真实光栅管线 + 合法合成闭合轮廓笔画「十」，非 D3 真字库数据）/ 提醒弹窗。
 *   语音轮五态演出在 host 无渲染面（RP-A9 内容面 = 设备侧聊天 layer 落点），不在本截图
 *   范围；真机观感终审 = RP-A5 真机对照轮（human 持板）。
 *
 * 截图开关：仅当环境变量 KIDPAL_CAPTURE_DIR 指向已存在目录时落盘；未设置时本测试
 * 只验证各渲染契约返回值（不落盘、不影响日常全量跑）。落盘失败即红（截图轮必须
 * 真拿到图，不假绿）。
 *
 * 与冻结齿的相容性纪律：
 *   - 不调用 kidpal_story_page_input / kidpal_story_on_saved / kidpal_story_rec_tick
 *     （hardware_validation FILE_COUNT 观察对象；本文件以直接设 view 字段驱动状态，
 *      语义等价且避免无谓的扫描计数扰动）；
 *   - 不出现任何「符号级零残留」齿的禁用字符串（s_chat_scr 族 / remind_before_min /
 *      端口字面量等）；
 *   - 大结构体（list 2824B / text 2208B）一律 static，守栈纪律精神；
 *   - 渲染次序：本文件在 HOST_ACCEPTANCE_SRCS 中紧邻 host_infra_smoke 之前 ——
 *      runner 执行序 = 列表逆序，故本文件在包 0 boundary 齿之后、全部既有齿之前执行，
 *      与「boundary 齿必须在进程内首帧渲染前跑」的守卫不冲突。
 *
 * Run protocol（截图轮）:
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) mkdir -p <capture_dir>
 *   3) cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- \
 *        env KIDPAL_CAPTURE_DIR=<abs capture_dir> ./build/host_infra_test.elf
 */
#include "sdkconfig.h"

#if defined(CONFIG_IDF_TARGET_LINUX)

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "lvgl.h"
#include "home_p1.h"
#include "schedule_page.h"
#include "story_page.h"
#include "reminder_page.h"
#include "char_stroke.h"
#include "art_assets.h"

/* 渲染缝符号（host_infra_seam.c：包 0 冻结两件 + additive 截图缝）。 */
extern int host_infra_render_frame(void);
extern int32_t host_infra_read_pixel(int x, int y);
extern int host_infra_dump_bmp(const char *path);

#define HFC_MAX_PATH 1024

static char hfc_root[HFC_MAX_PATH];
static int  hfc_root_ready;

/* 仓根定位（story_page_stack.test.c ssp_locate_root 同款先例）。 */
static int hfc_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[HFC_MAX_PATH];

    if (hfc_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        FILE *f = fopen(probe, "rb");
        if (f != NULL) {
            fclose(f);
            snprintf(hfc_root, sizeof(hfc_root), "%s", cands[i]);
            hfc_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

/* 包 0 渲染缝初始化（acceptance 渲染齿同款序）：初始化 LVGL 离屏显示 + 置 rendered
 * 态 —— s_rendered 只有 render_frame / blit 缝置位, 单靠 lv_refr_now 的 flush 回调
 * 不写该标志。凡走「页面渲染契约 + lv_refr_now」路径的截图测试须先调一次。
 *
 * 另：把 host 截图用 CJK 字体挂为活动屏 local 样式（默认字体仍是 Montserrat——
 * 本 LVGL 9.3 的 Kconfig `LV_FONT_DEFAULT_*` choice 无 lv_conf 映射）。页面标签
 * 不设字体、继承链到屏 local 样式即生效；真机 V3 字体（fonts_root SPIFFS）是
 * 设备面行为, 本设置仅 host 截图轮, 零设备影响。 */
static void hfc_seam_init(void)
{
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "HFC RED(infra): 包 0 渲染缝初始化失败");
#if LV_FONT_SOURCE_HAN_SANS_SC_14_CJK
    /* 全量 CJK（Source Han Sans SC 14）——SimSun 内置子集只有 ~1000 部首,
     * 截图正文大量缺字; 真机正文 = fonts_root font_body(GB2312 全集),
     * 本字体是其在 host 截图轮的最近似替代。 */
    lv_obj_set_style_text_font(lv_screen_active(), &lv_font_source_han_sans_sc_14_cjk, 0);
#elif LV_FONT_SIMSUN_16_CJK
    lv_obj_set_style_text_font(lv_screen_active(), &lv_font_simsun_16_cjk, 0);
#endif
}

/* 落盘一帧：KIDPAL_CAPTURE_DIR 未设 -> 跳过（日常跑不落盘）；已设 -> 必须成功。 */
static void hfc_dump(const char *name)
{
    char dir[HFC_MAX_PATH];
    char path[HFC_MAX_PATH + 64];
    const char *env = getenv("KIDPAL_CAPTURE_DIR");
    int r;

    if (env == NULL || env[0] == '\0') {
        return;
    }
    snprintf(dir, sizeof(dir), "%s", env);
    r = snprintf(path, sizeof(path), "%s/%s", dir, name);
    TEST_ASSERT_TRUE_MESSAGE(r > 0 && (size_t)r < sizeof(path),
        "HFC RED(infra): capture path truncated");
    errno = 0;
    r = host_infra_dump_bmp(path);
    if (r != 0) {
        printf("HFC dump %s -> r=%d errno=%d (%s)\n",
               name, r, errno, strerror(errno));
    }
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "HFC RED(infra): host_infra_dump_bmp failed — 检查 KIDPAL_CAPTURE_DIR 目录"
        "是否存在且可写（截图轮必须真拿到图）");
}

/* 挂载美术资产根（P1 imgfont/资产路径的软生效；失败不致命 —— 截图仍有骨架层）。 */
static void hfc_mount_assets(void)
{
    char path[HFC_MAX_PATH];

    if (!hfc_locate_root()) {
        return;
    }
    snprintf(path, sizeof(path), "%s/firmware/main/assets/spiffs_root", hfc_root);
    (void)kidpal_art_mount(path);
}

TEST_CASE("happy flow capture: P1 首页一帧 (P1-A2 同源渲染路径)",
          "[happy_flow][capture][P1]")
{
    hp1_state_t st;
    hp1_item_t items[2];

    hfc_mount_assets();

    memset(&items, 0, sizeof(items));
    snprintf(items[0].time, sizeof(items[0].time), "14:00");
    snprintf(items[0].name, sizeof(items[0].name), "钢琴课");
    snprintf(items[1].time, sizeof(items[1].time), "16:30");
    snprintf(items[1].name, sizeof(items[1].name), "英语培训");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_home_p1_schedule_inject("2026-09-13", items, 2, NULL) == 0,
        "HFC RED: schedule_inject rejected a valid payload");

    memset(&st, 0, sizeof(st));
    st.hour = 10; st.minute = 24; st.env_valid = 1;
    st.temp_c = 22; st.humidity_pct = 45;
    st.battery_pct = 85; st.charging = 0; st.tick_ms = 0;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_set_state(&st) == 0,
        "HFC RED: set_state rejected a valid state");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_home_p1_render_frame() == 0,
        "HFC RED: P1 render_frame failed — 首页一帧未刷新入缝");

    hfc_dump("01_home_p1.bmp");
}

TEST_CASE("happy flow capture: P2 课表页 今天/明天两态 (P2-A3 同源渲染路径)",
          "[happy_flow][capture][P2]")
{
    static kidpal_schedule_item_t items[2];

    hfc_seam_init();
    memset(items, 0, sizeof(items));
    snprintf(items[0].time, sizeof(items[0].time), "10:00");
    snprintf(items[0].name, sizeof(items[0].name), "钢琴课");
    snprintf(items[0].location, sizeof(items[0].location), "琴行");
    snprintf(items[1].time, sizeof(items[1].time), "16:30");
    snprintf(items[1].name, sizeof(items[1].name), "英语培训");
    snprintf(items[1].location, sizeof(items[1].location), "新东方");

    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_schedule_page_render("今天的课", items, 2, 0, "12:00", -1) == 2,
        "HFC RED: P2 today render did not report both rows");
    lv_refr_now(NULL);
    hfc_dump("02_schedule_today.bmp");

    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_schedule_page_render("明天的课", items, 2, 1, "12:00", -1) == 2,
        "HFC RED: P2 tomorrow render did not report both rows");
    lv_refr_now(NULL);
    hfc_dump("03_schedule_tomorrow.bmp");
}

TEST_CASE("happy flow capture: P3 故事页 列表/阅读/录音/盖章 (P3 同源渲染路径)",
          "[happy_flow][capture][P3]")
{
    static kidpal_story_list_t list;
    static kidpal_story_text_t st;
    kidpal_story_view_t v;

    hfc_seam_init();
    memset(&list, 0, sizeof(list));
    list.total = 4;
    list.count = 3;
    snprintf(list.items[0].id, sizeof(list.items[0].id), "2026-09-10-xiaomiao");
    snprintf(list.items[0].title, sizeof(list.items[0].title), "小猫钓鱼");
    snprintf(list.items[0].date, sizeof(list.items[0].date), "09-10");
    snprintf(list.items[1].id, sizeof(list.items[1].id), "2026-09-11-xiaogou");
    snprintf(list.items[1].title, sizeof(list.items[1].title), "小狗学叫");
    snprintf(list.items[1].date, sizeof(list.items[1].date), "09-11");
    snprintf(list.items[2].id, sizeof(list.items[2].id), "2026-09-12-ciwei");
    snprintf(list.items[2].title, sizeof(list.items[2].title), "勇敢的小刺猬");
    snprintf(list.items[2].date, sizeof(list.items[2].date), "09-12");

    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_view_init(&v, &list) == 0,
        "HFC RED: story_view_init failed on a valid list");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_page_render(&v, &list, NULL) == 0,
        "HFC RED: P3 LIST render failed");
    lv_refr_now(NULL);
    hfc_dump("04_story_list.bmp");

    /* 阅读态（直接设字段驱动状态；不经 page_input —— 见文件头相容性纪律） */
    snprintf(st.id, sizeof(st.id), "2026-09-12-ciwei");
    snprintf(st.title, sizeof(st.title), "勇敢的小刺猬");
    snprintf(st.text, sizeof(st.text),
             "小刺猬背上背着满满的果子。\n"
             "它走啊走，遇见了小兔子。\n"
             "小兔子说：你的果子真多呀！\n"
             "小刺猬把果子分给了朋友们。\n"
             "大家一起吃了甜甜的果子。\n"
             "天黑了，小刺猬回到家。\n"
             "妈妈说：你真是一个大方的好孩子。\n"
             "小刺猬甜甜地睡着了。\n"
             "梦里，果子里长出了星星。\n"
             "星星们陪它一起玩耍。");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_open_reader(&v, &st, KSTORY_READER_LINES_PER_SCREEN) == 0,
        "HFC RED: story_open_reader failed on a valid text");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_page_render(&v, &list, &st) == 0,
        "HFC RED: P3 READER render (screen 0) failed");
    lv_refr_now(NULL);
    hfc_dump("05_story_reader_screen1.bmp");

    v.screen = 1; /* BOOT 长按翻屏后的第 2 屏（screen 字段语义见冻结契约 P3-A5） */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_page_render(&v, &list, &st) == 0,
        "HFC RED: P3 READER render (screen 1) failed");
    lv_refr_now(NULL);
    hfc_dump("06_story_reader_screen2.bmp");

    /* 录音态（REC 演出 + 大时长） */
    v.page = KSTORY_PAGE_RECORDING;
    v.rec_seconds = 47;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_page_render(&v, &list, NULL) == 0,
        "HFC RED: P3 RECORDING render failed");
    lv_refr_now(NULL);
    hfc_dump("07_story_recording.bmp");

    /* 盖章帧（第 4 颗故事果） */
    v.page = KSTORY_PAGE_STAMP;
    v.stamp_index = 4;
    v.counter = 4;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_page_render(&v, &list, NULL) == 0,
        "HFC RED: P3 STAMP render failed");
    lv_refr_now(NULL);
    hfc_dump("08_story_stamp.bmp");
}

/* P4 查生字：真实光栅管线（parse -> transform -> raster）+ 合成「十」闭合轮廓笔画。
 * 笔画数据是合法几何（makemeahanzi 1024×1024 坐标域的闭合多边形），不是 D3 真字库
 * 条目 —— 截图目的是验证 P4 渲染路径（示范字光栅 + 速查卡文本），非字形美学。 */
TEST_CASE("happy flow capture: P4 查生字 光栅管线 + 速查卡 (CHR 同源路径)",
          "[happy_flow][capture][P4]")
{
    static uint8_t bitmap[300 * 50]; /* 400x300 1bpp, stride 50B */
    static const char *const strokes[2] = {
        "M 120 460 L 904 460 L 904 548 L 120 548",  /* 横 */
        "M 468 96 L 556 96 L 556 928 L 468 928",    /* 竖 */
    };
    kidpal_char_info info;
    kidpal_char_card card;
    lv_draw_buf_t *p4buf;
    uint32_t *px;
    lv_obj_t *scr;
    int n_ink = 0;

    hfc_seam_init();
    /* 先清屏：本测试在执行序上紧随 REMINDER 用例（执行序 = 源文件逆序），既有页面
     * 渲染自带先清屏，此处对齐同一纪律，避免前一页残留混入本页截图。 */
    lv_obj_clean(lv_screen_active());
    lv_refr_now(NULL);
    memset(bitmap, 0, sizeof(bitmap));

    for (int s = 0; s < 2; s++) {
        kidpal_cs_point pts[32];
        kidpal_cs_point screen_pts[32];
        int n = kidpal_char_stroke_parse(strokes[s], pts, 32);
        TEST_ASSERT_TRUE_MESSAGE(n >= 3,
            "HFC RED: synthetic stroke parse failed (real D3 parse path)");
        for (int i = 0; i < n; i++) {
            int sx, sy;
            TEST_ASSERT_TRUE_MESSAGE(
                kidpal_char_transform(pts[i].x, pts[i].y, &sx, &sy) == 0,
                "HFC RED: transform failed on in-domain synthetic point");
            screen_pts[i].x = (float)sx;
            screen_pts[i].y = (float)sy;
        }
        TEST_ASSERT_TRUE_MESSAGE(
            kidpal_char_raster_stroke(screen_pts, n, bitmap, 400, 300, 50) == 0,
            "HFC RED: raster_stroke failed on a valid outline");
    }

    /* 1bpp -> ARGB8888 白底黑字 + 浅灰米字格框（P4 版面参照, 非字形本体）。
     * 帧缓冲用 lv_draw_buf_create 分配（LVGL 保证 draw-buf 对齐），画布控件绘制，
     * 与 host_infra_seam 的成功渲染路径同源。 */
    p4buf = lv_draw_buf_create(400, 300, LV_COLOR_FORMAT_ARGB8888,
                               LV_DRAW_BUF_STRIDE(400, LV_COLOR_FORMAT_ARGB8888));
    TEST_ASSERT_NOT_NULL_MESSAGE(p4buf, "HFC RED: draw_buf_create failed");
    px = (uint32_t *)lv_draw_buf_goto_xy(p4buf, 0, 0);
    TEST_ASSERT_NOT_NULL_MESSAGE(px, "HFC RED: draw_buf data NULL");

    for (int i = 0; i < 400 * 300; i++) {
        px[i] = 0xFFFFFFFFu;
    }
    for (int y = 30; y < 250; y++) {
        for (int x = 40; x < 260; x++) {
            int edge = (x == 40 || x == 259 || y == 30 || y == 249);
            int cross = (x == 150 || y == 140);
            int diag = (abs((x - 40) - (y - 30)) < 1) ||
                       (abs((259 - x) - (y - 30)) < 1);
            if (edge || cross || diag) {
                px[(size_t)y * 400 + (size_t)x] = 0xFFCCCCCCu;
            }
        }
    }
    for (int y = 0; y < 300; y++) {
        for (int x = 0; x < 400; x++) {
            if ((bitmap[(size_t)y * 50 + (size_t)(x / 8)] >>
                 (7 - (x % 8))) & 1u) {
                px[(size_t)y * 400 + (size_t)x] = 0xFF000000u;
                n_ink++;
            }
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(n_ink > 200,
        "HFC RED: raster produced almost no ink — 光栅管线产物异常");

    scr = lv_screen_active();
    TEST_ASSERT_NOT_NULL_MESSAGE(scr, "HFC RED(infra): no active screen");
    lv_obj_t *cv = lv_canvas_create(scr);
    lv_canvas_set_draw_buf(cv, p4buf);
    lv_obj_set_pos(cv, 0, 0);

    /* 右栏速查卡（真实 card_build 产物）+ 标题（本测试场景版面, 非 char_page 设备版面） */
    memset(&info, 0, sizeof(info));
    info.pinyin = "shí";
    info.radical = "十";
    info.stroke_count = 2;
    info.words = "十个、十分";
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_card_build(&info, -1, &card) == 0,
        "HFC RED: card_build failed on a valid info");
    {
        struct { const char *text; int x, y; } rows[] = {
            { "查生字", 12, 8 },
            { card.pinyin, 284, 60 },
            { card.stroke_total, 284, 96 },
            { card.progress, 284, 128 },
            { card.radical, 284, 160 },
            { card.words, 284, 192 },
        };
        for (size_t i = 0; i < sizeof(rows) / sizeof(rows[0]); i++) {
            lv_obj_t *l = lv_label_create(scr);
            lv_label_set_text(l, rows[i].text);
            lv_obj_set_pos(l, rows[i].x, rows[i].y);
        }
    }
    lv_refr_now(NULL);
    hfc_dump("09_char_stroke.bmp");

    /* 清场：本页的自建控件不留到后续齿（既有页面渲染自带先清屏, 此处对齐纪律） */
    lv_obj_clean(scr);
    lv_refr_now(NULL);
}

TEST_CASE("happy flow capture: 提醒弹窗 (RMD-A5 同源渲染路径)",
          "[happy_flow][capture][REMINDER]")
{
    static kidpal_alert_t alert;
    static const char *const args =
        "{\"id\":\"rmd-3f2a1b9c4d5e6f70\","
        "\"fire_at\":\"2026-09-09T16:15:00+08:00\","
        "\"title\":\"明天钢琴课 16:30 开始前\","
        "\"speech\":\"16:30 有钢琴课，别忘了带琴谱！\","
        "\"view\":\"{\\\"view\\\":\\\"text\\\",\\\"data\\\":{\\\"text\\\":\\\"16:30 钢琴课\\\"}}\"}";

    hfc_seam_init();
    TEST_ASSERT_TRUE_MESSAGE(kidpal_alert_parse(args, &alert) == 0,
        "HFC RED: canonical alert payload failed to parse");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_page_render(&alert) == 1,
        "HFC RED: reminder page render did not draw");
    lv_refr_now(NULL);
    hfc_dump("10_reminder_popup.bmp");
}

#endif /* CONFIG_IDF_TARGET_LINUX */
