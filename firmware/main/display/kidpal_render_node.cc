/*
 * kidpal_render_node.cc — 设备渲染节点（包 260911-screen-unification 归一屏收口）
 *
 * 职责：
 *  1. 提供 host_infra_render_p1_frame 强符号覆盖，把 P1 首页 400×300 ARGB 帧
 *     通过 LVGL XRGB8888 图像渲染至 RLCD。
 *  2. 前台归属 = kidpal_layer 前台 layer 栈管理单一权威（把原先三散同行标志 +
 *     1s tick 轮询对账整体退役）。
 *     layer 栈按优先级表裁决「谁在前台」：配网(7) > 提醒(6) > 查字窗(5) > 服务端推送页(4) >
 *     语音轮(3) > 本地浏览页(2)，聊天 layer(1) 常驻非栈成员。push/pop 语义见
 *     display/kidpal_layer.(c|h) 与冻结验收 screen_unification.acceptance.c。
 *     （2026-09-19 M2-K3 页→窗改造：插入查字窗层，REMINDER 5→6 / PROVISION 6→7。）
 *  3. 单物理屏时序说明（归一屏 SU-RULING-02 方案1，聊天控件物理 reparent 已随本包落地）：
 *     - xiaozhi LcdDisplay::SetupUI() 在 LcdDisplay 构造期把聊天/表情/状态控件建在
 *       LVGL 默认首屏上，此刻 s_kidpal_scr 尚未创建（先 SetupUI 后 render_node_init）。
 *       KidPal 板装配层（waveshare-s3-rlcd-4.2.cc）在 render_node_init 前调
 *       LcdDisplay::KidPalReparentChatToLayer(lv_layer_top())，把聊天 chrome
 *       reparent 到常驻顶层 layer（非屏、恒浮于活动屏之上），聊天内容任何页可见；
 *       整屏不透明背景 container_ 留在原默认首屏（该屏此后不再加载、不再渲染）。
 *     - s_kidpal_scr（lv_obj_create(NULL)）= KidPal 各类内容页的唯一天花板，也是
 *       display/ 下唯一被 lv_screen_load 的顶层屏（真单物理屏不变式）。
 *  4. 阶段 B：BOOT 双按翻页（home→schedule→story→home 三页循环，M2-K2 撤 P4 常驻页），
 *     翻页 = 本地浏览层生命周期（清 transient，回 LOCAL_PAGE 基层）。
 *  5. 崩溃修复（包 260829-hardware-validation）语义保留：服务端推送页面渲染必须经
 *     begin_page()/end_page() 夹钳——begin_page 把 SERVER_PAGE(或 REMINDER) 推入
 *     layer 栈、持 LVGL 锁、把活动屏切到 s_kidpal_scr；页面渲染函数内部
 *     lv_screen_active()+lv_obj_clean() 只清 s_kidpal_scr，绝不触碰聊天层视觉控件
 *     （聊天 chrome 已 reparent 到 lv_layer_top，根除「clean 聊天屏致 status_label_
 *     悬空」同族崩溃）。
 *
 * DRAM 约束：s_p1_fb (480KB) 已通过 CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
 * 置于 PSRAM；本文件不新增任何大静态缓冲。layer 栈为 8*int+int，零堆。
 *
 * 线程约束：show_home() / show_chat() / voice_begin() / voice_end() / begin_page() /
 * cycle_next() 在 main app task（Schedule）或 CustomBoard 构造期调用，持有
 * lvgl_port_lock（递归互斥量）。tick() 从 LVGL 定时器（esp_lvgl_port task）调用，
 * 锁已由 esp_lvgl_port 持有。host_infra_render_p1_frame 由
 * kidpal_home_p1_render_frame() 在锁上下文中同步调用。
 */
#include "kidpal_render_node.h"

#include "home_p1.h"
#include "kidpal_p1_font.h"
#include "schedule_page.h"
#include "story_page.h"
#include "char_page.h"
#include "char_window.h"   /* M2K3-A3 驱动半 seam [9]: kidpal_char_window_idle_step / 5min 上限 */
#include "kidpal_layer.h"

#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_lvgl_port.h>
#include <lvgl.h>

#include <cstring>
#include <string>

static const char *RN_TAG = "kidpal_rn";

/* -------------------------------------------------------------------------
 * 渲染节点内部状态（全部小量，不新增大静态缓冲）
 * ---------------------------------------------------------------------- */

/* 前台 layer 栈（前台 owner 单一权威，取代原先三个分散的覆盖/pin/当前页标志）。 */
static kidpal_layer_stack_t s_layer_stack;

/* KidPal 专属 LVGL 屏（persistent，只在 init 时创建一次，之后复用）。
 * display/ 下唯一的顶层屏创建点（lv_obj_create(NULL)）——归一屏单屏不变式。 */
static lv_obj_t *s_kidpal_scr = NULL;

/* 首页图像控件（每次重建，因为切到 schedule/story 时 lv_obj_clean(s_kidpal_scr)
 * 会销毁它）。仅当本地首页为前台 owner 时由 tick 重渲动画。 */
static lv_obj_t *s_kidpal_img = NULL;

/* P1 帧 LVGL 图像描述符（s_p1_dsc.data 由 host_infra_render_p1_frame 强符号
 * 从 argb 参数赋值，即 s_p1_fb 的地址（PSRAM））。lv_image_set_src 存储此指针，
 * 因此 s_p1_dsc 必须在整个屏幕生命周期内保持存活。 */
static lv_image_dsc_t s_p1_dsc;

/* 当前本地浏览页（阶段 B cycle 状态：翻到 schedule/story 时仍停在 LOCAL_PAGE
 * 层，只是换了页内内容）。区别于前台 owner——前台 owner 由 layer 栈 top() 单一裁决，
 * 本变量只记录「LOCAL_PAGE 基层当前画的是哪一页」，供 tick 首页动画判据与翻页步进。 */
static kidpal_page_id_t s_local_page = KP_PAGE_HOME;

/* 注入状态。 */
static int s_rtc_hour     = 0;
static int s_rtc_minute   = 0;
static int s_rtc_month    = 0; /* 1..12（0 = 未知；板级 PCF85063 日期寄存器带入） */
static int s_rtc_mday     = 0; /* 1..31（0 = 未知） */
static int s_env_valid    = 0;
static int s_temp_c       = 0;
static int s_humidity_pct = 0;
static int s_battery_pct  = 100; /* 保守初始值（无 ADC 读数时使用） */
static int s_charging     = 0;
/* 设备本地星期（周一=0..周日=6; -1 = 未注入/越域不画假「今天」）。M3-K2 今天列
 * 推导数据源：板级 RTC 装配点经 kidpal_render_node_inject_weekday 写入, P2 网格
 * 每次落屏重取（本地零点跨天后板级重读 RTC 即切换, 不依赖云端, NFR-OFF-1）。 */
static int s_rtc_wday     = -1;

/* 动画 LVGL 定时器（在 init 中创建，每 RN_TICK_MS 触发一次）。仅重渲首页动画帧，
 * 不做任何前台/语音对账（对账已由事件驱动 layer push/pop 接管）。 */
static lv_timer_t *s_tick_timer = NULL;
static const uint32_t RN_TICK_MS = 1000;

/* 查字窗 5 分钟驻留计时的设备侧驱动状态（TaskPackage 260917-m2-k3-char-window-overlay,
 * seam [9], M2K3-A3 驱动半；requirements §2.2 查字窗行 v1.43「计时随任意按键重计」）：
 *   s_char_window_idle_ms = 距最近一次操作的累计毫秒（tick 内喂 kidpal_char_window_idle_step
 *     推进/归零；仅在查字窗为前台时累计，否则归零——非查字窗前台不得累计，否则窗一开
 *     首拍即因历史累计越限而立即误关窗）。文件级 static，初值 0。
 *   s_char_window_key_hit = 板级任意键按下的置位标志（kidpal_render_node_char_window_key_hit），
 *     tick 内取出并清位后作为 key_pressed 喂 step。跨任务写（按键回调任务），tick（LVGL
 *     定时器任务）读，32 位对齐整数读写原子。 */
static unsigned long s_char_window_idle_ms = 0;
static volatile int  s_char_window_key_hit = 0;

/* P2 一周总览网格缓存（mcp_server 收到周网格数据解析成功后调
 * cache_schedule_grid 填充; cycle_next 读此渲染 P2; cache_schedule 网格化,
 * 旧 16 项列表缓存作废, 容量 KSG_COURSE_CAP=84 定容冻结值）。 */
static kidpal_schedule_course_t s_grid_courses[KSG_COURSE_CAP];
static int                      s_grid_n = 0;

/* P3 故事缓存。 */
static kidpal_story_view_t s_story_view = {};
static kidpal_story_list_t s_story_list = {};
static kidpal_story_text_t s_story_text = {};

/* K3R [R5] 权威会话页态镜像（261001-m4-k3-record-session-live-repair, 真机轮架构修）:
 * 渲染节点自身不持 g_story_view（住 mcp_server.cc）, 由 mcp 录音/收尾路径经本 C 桥写入
 * 权威会话页态——voice_end 的 [R5] 以此作 session_page 权威源, 与节点缓存分叉时取之
 * （取代「裸读节点缓存」的潜伏 bug; 不再依赖 cache_story 掩盖）。 */
static int s_story_session_page = 0;
extern "C" void kidpal_render_node_story_session_page(int page) {
    s_story_session_page = page;
}

/* ------------------------------------------------------------------------- *
 * 五态 chrome 显隐的前台维度桥 (RP-A5 r3/r4)：姿态/气泡仅当 VOICE 层为前台层时显示，
 * 任何内容页（SERVER_PAGE 答案 / REMINDER / LOCAL_PAGE 浏览）前台时隐藏—— chrome
 * 常驻 lv_layer_top 恒浮于活动屏之上，层栈只裁决前台屏，显隐第二维须由前台切换
 * 点事件驱动。本模块各调用点均已持 lvgl_port_lock（见文件头线程约束），桥内递归
 * 取锁无害。
 * r4 修正: 经 lcd_display.cc 的 C 桥 + SetupUI 登记的 self 指针取 display,
 * 绝不经 Board::GetInstance() —— boot 期 render_node_init 跑在 CustomBoard 构造
 * 期内, 函数内 static 单例重入会 __cxa_guard 永久阻塞 (r3 真机开机挂死根因)。
 * s_kidpal_scr==NULL（未 init / 非 KidPal 归一屏板）时不作为。
 * ---------------------------------------------------------------------- */
extern "C" void kidpal_lcd_voice_chrome_front(int front);
extern "C" void kidpal_lcd_keybar_sync(void);
/* K3R D2 (2026-10-01 真机轮架构修): 录音态强屏蔽桥（render_node -> lcd_display.cc 的
 * sync 执行点）。 */
extern "C" void kidpal_lcd_recording_force_hide(int hide);

static void rn_voice_chrome_front_locked(void)
{
    if (s_kidpal_scr == NULL) {
        return;
    }
    /* K3R D2 录音态强屏蔽（261001-m4-k3-record-session-live-repair, 真机轮架构修）:
     * 把权威录音态（渲染节点缓存 s_story_view.page == RECORDING, 录音 enter 路径已
     * cache_story 预同步）**直接投递 LCD 的 sync 执行点**（kidpal_chrome_mid_sync_locked
     * 的 voice_live 与）——真机现场证「[R4] 判据返回 0 -> front 桥 -> sync 生效」这条链
     * runtime 不生效（弹层后 front=false 而 voice_page 仍驻留）, 故屏蔽必须落执行点。 */
    int force_hide = (s_story_view.page == KSTORY_PAGE_RECORDING) ? 1 : 0;
    kidpal_lcd_recording_force_hide(force_hide);
    /* K3R [R4]: chrome 前台判据含录音页态维度（普通聆听轮 VOICE 前台 chrome 照前,
     * 既有 RP-A5 r3 语义零偏移; 录音态退隐——与层值无关）。 */
    int front = kidpal_story_voice_chrome_front(kidpal_layer_top(&s_layer_stack),
                                                s_story_view.page);
    /* K3R D2 诊断（保留诊断债务）: 层值 + 录音屏蔽位 + 判据出参。 */
    ESP_LOGI(RN_TAG, "chrome_front layer_top=%d force_hide=%d front=%d",
             kidpal_layer_top(&s_layer_stack), force_hide, front);
    kidpal_lcd_voice_chrome_front(front);
}

/* 键帽条三槽刷新 (260915-m1-k2-keybar-three-slot R1 / ①④根修): 由本模块的页面渲染权威
 * (rn_render_home_locked / rn_render_kidpal_page_locked) 每页落屏后经 LcdDisplay C 桥驱动
 * —— 与键路由/页循环同源, 不依赖语音态/前台桥 (基础页/启动后键帽条也须落字与刷新, KB-A1
 * 通栏贴底三槽任何状态常驻)。keybar 只经 kidpal_render_node_front_is_overlay/get_page 读
 * 入参, 本调用零 layer 进出裁决 (KB-E3)。调用点已持 lvgl_port_lock, 桥内递归取锁无害。 */
static void rn_keybar_sync_locked(void)
{
    if (s_kidpal_scr == NULL) {
        return;
    }
    kidpal_lcd_keybar_sync();
}

/* -------------------------------------------------------------------------
 * 强符号覆盖：host_infra_render_p1_frame（home_p1.c 中的弱默认为 -1）
 *
 * 同步调用上下文（show_home() / tick() 持有 lvgl_port_lock）。
 * 更新描述符并让 LVGL 重绘 s_kidpal_img。
 * ---------------------------------------------------------------------- */
extern "C" int host_infra_render_p1_frame(const uint32_t *argb, int w, int h)
{
    if (s_kidpal_img == NULL || argb == NULL) {
        return -1;
    }
    /* K3R D4b-3 对象有效性守卫（261001-m4-k3-record-session-live-repair）: 外部
     * kidpal_*_page_render（如 kidpal_story_page_render 的 lv_obj_clean）会销毁
     * s_kidpal_img 所指图像对象却**不置空** s_kidpal_img（仅 rn_render_home_locked /
     * rn_render_kidpal_page_locked 置空）——悬垂对象出帧致 LoadProhibited panic
     * （backtrace: lv_obj_get_screen <- lv_obj_get_display <- lv_obj_invalidate_area
     * <- lv_obj_invalidate <- lv_image_set_src <- 本函数）。出帧前以
     * lv_obj_is_valid 守卫, 悬垂时复位 NULL 交 tick NULL 守卫, 绝不 lv_image_set_src。 */
    if (!lv_obj_is_valid(s_kidpal_img)) {
        s_kidpal_img = NULL;
        return -1;
    }
    memset(&s_p1_dsc, 0, sizeof(s_p1_dsc));
    s_p1_dsc.header.cf     = LV_COLOR_FORMAT_XRGB8888;
    s_p1_dsc.header.w      = static_cast<uint32_t>(w);
    s_p1_dsc.header.h      = static_cast<uint32_t>(h);
    s_p1_dsc.header.stride = static_cast<uint32_t>(w) * 4u;
    s_p1_dsc.data          = reinterpret_cast<const uint8_t *>(argb);
    s_p1_dsc.data_size     = static_cast<uint32_t>(w) * static_cast<uint32_t>(h) * 4u;
    /* lv_image_set_src 存描述符指针（非拷贝），s_p1_dsc 是文件级静态，
     * 故描述符与 s_p1_fb（PSRAM）均始终存活。 */
    lv_image_set_src(s_kidpal_img, &s_p1_dsc);
    lv_obj_invalidate(s_kidpal_img);
    return 0;
}

/* -------------------------------------------------------------------------
 * 强符号覆盖：art_render_blit_frame（art_assets.c 中的弱默认为 -1）
 *
 * 设备侧死码路径：art_assets.c 的 draw_nine_slice / draw_dots / draw_time
 * 函数不被任何设备代码路径调用；首页换皮经 p1_blit_art() 直接写 s_p1_fb。
 * 覆盖仅用于编译完整性（防止意外调用时链入 g_fb 480KB）。
 * ---------------------------------------------------------------------- */
extern "C" int art_render_blit_frame(const uint32_t *argb, int w, int h)
{
    (void)argb; (void)w; (void)h;
    /* 设备侧不应到达此路径（见上注）。返回 -1 与弱默认一致，避免 g_fb 被意外链入。 */
    return -1;
}

/* -------------------------------------------------------------------------
 * [S1] 设备字形强符号：kidpal_p1_font_glyph（覆盖 home_p1.c 弱默认）
 *
 * ksg_text_font()（schedule_page.c 先例）同款双分支取对话页同款 14px 普通字体
 * font_puhui_14_1。**方案 B（human 拍板 2026-09-22）**：去掉 req_raw_bitmap=1，
 * 走 LVGL 官方解码（lv_font_get_glyph_bitmap 把字体 glyph 转成 A8 写入 draw_buf，
 * 内部按行处理 stride，修掉 live 轮 A6 FAIL 的「fdsc->stride=0 时 bpp=1 PLAIN
 * 连续位流跨行逐位漂移」根因——旧 raw 位流手解假定字节对齐行）→ A8 阈值 128 化
 * 1bpp（A8@128 ≡ A4@8 ≡ A2@2 ≡ A1@1 = 50% 覆盖，逐齿期望零偏移）。字形盒上限收
 * 32×32（对 [S1] 冻结 1..64 的实现收紧子集，非契约变更）。设备 builds 恰一个强
 * 符号覆盖 home_p1 weak；host 侧由 host_infra_seam.c 提供同符号（本 TU 不编入
 * host runner）。
 *
 * **共享核心同构重构（[S6] 追加 seam，第三追加轮 A11）**：解码+阈值化核心重构为
 * 字体参数化 p1_glyph_render_core()，[S1] 入口 = 固定设备字体调核心——与 host seam
 * （host_infra_seam.c p1_glyph_core）同构，两 TU 对同字体输出逐字节一致（TEST 10
 * tooth 5 承载）。**零新增导出符号**：设备生产路径唯一字体入口仍 kidpal_p1_font_glyph，
 * [S6] 为测试可达缝，设备 TU 不导出（头注登记面；[S6] 强符号仅 host runner 定义）。
 * ---------------------------------------------------------------------- */
#if LV_FONT_SOURCE_HAN_SANS_SC_14_CJK
LV_FONT_DECLARE(lv_font_source_han_sans_sc_14_cjk);
#elif defined(ESP_PLATFORM)
LV_FONT_DECLARE(font_puhui_14_1);
#endif

/* 方案 B A8 静态 draw_buf（LVGL 官方解码 scratch；32×32 盒上限）。每个二进制恰
 * 一个 TU 持有一份（设备本 TU + host seam TU），静态内存增量 = 本缓冲大小 + 1bpp
 * 输出（32*((32+7)/8)=128B）。 */
#define P1_GLYPH_BOX_CAP 32
static unsigned char s_glyph_a8_data[LV_DRAW_BUF_SIZE(P1_GLYPH_BOX_CAP, P1_GLYPH_BOX_CAP, LV_COLOR_FORMAT_A8)]
    __attribute__((aligned(LV_DRAW_BUF_ALIGN)));
static lv_draw_buf_t s_glyph_a8_buf;
static int s_glyph_a8_ready = 0;

static int p1_glyph_a8_buf_init(void)
{
    if (s_glyph_a8_ready) {
        return 1;
    }
    if (lv_draw_buf_init(&s_glyph_a8_buf, P1_GLYPH_BOX_CAP, P1_GLYPH_BOX_CAP,
                         LV_COLOR_FORMAT_A8, 0,
                         s_glyph_a8_data, sizeof(s_glyph_a8_data)) != LV_RESULT_OK) {
        return 0;
    }
    s_glyph_a8_ready = 1;
    return 1;
}

/* [S6] 共享核心（设备同构副本，host seam 同名态）：字体参数化解码+阈值化核心。
 * 返回 0 = 字形可用; 非 0 = 不可用。语义与 [S1] 逐字段一致。static——设备 TU
 * 零新增导出符号（[S6] 仅 host runner 定义，设备单一生产入口仍 [S1]）。 */
static int p1_glyph_render_core(const lv_font_t *font, uint32_t codepoint,
                                kidpal_p1_glyph_t *out)
{
    lv_font_glyph_dsc_t dsc;
    const uint8_t *a8;
    static unsigned char s_bits[P1_GLYPH_BOX_CAP * ((P1_GLYPH_BOX_CAP + 7) / 8)];
    int byte_stride, a8_stride;
    int x, y;

    if (out == nullptr || font == nullptr) {
        return -1; /* [S1]: out == NULL -> 非 0；NULL 字体防御 */
    }
    if (!lv_font_get_glyph_dsc(font, &dsc, codepoint, 0)) {
        return -1;
    }
    if (dsc.is_placeholder || dsc.box_w == 0 || dsc.box_h == 0) {
        return -1;
    }
    if (static_cast<int>(dsc.box_w) > P1_GLYPH_BOX_CAP ||
        static_cast<int>(dsc.box_h) > P1_GLYPH_BOX_CAP) {
        return -1; /* 32×32 实现收紧子集（[S1] 冻结 1..64 的实现子集） */
    }
    /* 方案 B：req_raw_bitmap 保持 0（默认）——LVGL 官方解码到 A8 draw_buf。
     * **live 轮乱码根因**（设备 TU 同构修复，host seam 同款注释）：lv_font_get_glyph_dsc
     * 不初始化 req_raw_bitmap（stack 局部未清零）；直接调 font->get_glyph_bitmap 会
     * 旁路掉官方包装 lv_font_get_glyph_bitmap 的 req_raw_bitmap=0 归一化，未清零字段
     * 随机命中 raw 早退 → 返回原图而非解码 A8，文字行乱码。TEST 10（A11）host 实证
     * 复现（通道④分支1），修复 = 显式清零（两 TU 同构同改）。 */
    dsc.req_raw_bitmap = 0;
    if (!p1_glyph_a8_buf_init()) {
        return -1;
    }
    {
        /* **live 轮乱码根因（设备 TU 同构修复，host seam 同款注释）**：LVGL 9
         * get_glyph_bitmap 返回 `lv_draw_buf_t *`（struct），须读返回对象 .data；
         * 旧实现把 struct 指针当 A8 像素读 → 读到 draw_buf 结构头垃圾 → 字形错乱。
         * TEST 10（A11）host 实证复现（通道④分支1），两 TU 同构同改。 */
        const lv_draw_buf_t *s_dec_db =
            static_cast<const lv_draw_buf_t *>(font->get_glyph_bitmap(&dsc, &s_glyph_a8_buf));
        if (s_dec_db == nullptr) {
            return -1;
        }
        a8 = static_cast<const uint8_t *>(s_dec_db->data);
    }
    if (a8 == nullptr) {
        return -1;
    }
    byte_stride = (static_cast<int>(dsc.box_w) + 7) / 8;
    a8_stride = static_cast<int>(lv_draw_buf_width_to_stride(
        static_cast<uint32_t>(dsc.box_w), LV_COLOR_FORMAT_A8));
    std::memset(s_bits, 0, sizeof(s_bits));
    for (y = 0; y < static_cast<int>(dsc.box_h); y++) {
        for (x = 0; x < static_cast<int>(dsc.box_w); x++) {
            if (a8[y * a8_stride + x] >= 128) { /* A8 阈值 128 = 50% 覆盖 */
                s_bits[y * byte_stride + (x >> 3)] |=
                    static_cast<unsigned char>(1u << (7 - (x & 7)));
            }
        }
    }
    out->w = static_cast<int>(dsc.box_w);
    out->h = static_cast<int>(dsc.box_h);
    out->advance = static_cast<int>(dsc.adv_w);
    out->bits = s_bits;
    return 0;
}

extern "C" int kidpal_p1_font_glyph(uint32_t codepoint, kidpal_p1_glyph_t *out)
{
#if LV_FONT_SOURCE_HAN_SANS_SC_14_CJK
    return p1_glyph_render_core(&lv_font_source_han_sans_sc_14_cjk, codepoint, out);
#elif defined(ESP_PLATFORM)
    return p1_glyph_render_core(&font_puhui_14_1, codepoint, out);
#else
    if (out != nullptr) {
        out->w = 0;
        out->h = 0;
        out->advance = 0;
        out->bits = nullptr;
    }
    return -1;
#endif
}

/* -------------------------------------------------------------------------
 * 内部辅助（均须在 lvgl_port_lock 持有下调用）
 * ---------------------------------------------------------------------- */

/* 归一屏后只有 s_kidpal_scr 一个前台屏：聊天 chrome 已 reparent 到 lv_layer_top（恒
 * 浮于活动屏之上、任何页可见），无需因 VOICE 前台切屏。本函数仅保证 s_kidpal_scr 是
 * 活动屏——前台归属由 layer 栈单一裁决，内容页归属由各 push/pop 路径的页面渲染承接。 */
static void rn_reconcile_foreground_locked(void)
{
    if (s_kidpal_scr != NULL) {
        lv_screen_load(s_kidpal_scr);
    }
}

/* 前置声明（rn_render_kidpal_page_locked 定义在后方本地浏览层区; voice_end 的
 * [R3] P3 前台回落重渲在此提前消费, 须先前向声明）。 */
static void rn_render_kidpal_page_locked(kidpal_page_id_t page);

/* 重建并渲染首页 P1 帧，再把它切到前台。s_kidpal_scr 必须已存在。 */
static void rn_render_home_locked(void)
{
    /* 清屏重建（若从 schedule/story 切回，图像控件已被 clean 销毁）。 */
    lv_obj_clean(s_kidpal_scr);
    s_kidpal_img = NULL;

    /* 重建全屏图像控件（400×300 XRGB8888，描述符在 host_infra_render_p1_frame 中填充）。 */
    s_kidpal_img = lv_image_create(s_kidpal_scr);
    lv_obj_set_size(s_kidpal_img, 400, 300);
    lv_obj_set_pos(s_kidpal_img, 0, 0);

    /* 注入状态（tick_ms 取当前毫秒以触发冒号初始相位）。 */
    hp1_state_t st = {};
    st.hour         = s_rtc_hour;
    st.minute       = s_rtc_minute;
    st.month        = s_rtc_month;
    st.mday         = s_rtc_mday;
    st.wday         = s_rtc_wday; /* P1 日期星期（2026-09-26 对齐轮, M3-K2 同源缝） */
    st.env_valid    = s_env_valid;
    st.temp_c       = s_temp_c;
    st.humidity_pct = s_humidity_pct;
    st.battery_pct  = s_battery_pct;
    st.charging     = s_charging;
    st.tick_ms      = static_cast<int>(esp_timer_get_time() / 1000);
    int state_rc = kidpal_home_p1_set_state(&st);
    if (state_rc != 0) {
        ESP_LOGW(RN_TAG, "home_p1_set_state rc=%d (hour=%d min=%d bat=%d)",
                 state_rc, st.hour, st.minute, st.battery_pct);
    }

    /* 渲染首页帧（填充 PSRAM s_p1_fb，触发 host_infra_render_p1_frame 强符号更新
     * s_p1_dsc 与 s_kidpal_img）。 */
    int render_rc = kidpal_home_p1_render_frame();

    /* 把 KidPal 屏切到前台。 */
    lv_screen_load(s_kidpal_scr);
    s_local_page = KP_PAGE_HOME;

    ESP_LOGI(RN_TAG, "render home 400x300 render_rc=%d state_rc=%d tick=%d",
             render_rc, state_rc, st.tick_ms);

    /* 首页基层落屏后驱动键帽条三槽刷新 (keybar R1 / ①根修)。 */
    rn_keybar_sync_locked();
}

/* -------------------------------------------------------------------------
 * 公开 API
 * ---------------------------------------------------------------------- */

void kidpal_render_node_init(void)
{
    if (s_kidpal_scr != NULL) {
        return; /* 幂等 */
    }
    lvgl_port_lock(0);

    /* 聊天 chrome 已由 KidPal 板装配层在 init 前 reparent 到 lv_layer_top()（常驻聊天
     * layer，SU-RULING-02 单物理屏）；本模块只创建并持有唯一前台屏 s_kidpal_scr。 */

    /* 创建专属屏（lv_obj_create(NULL) = 新建一屏，不与任何父对象绑定）。
     * display/ 下唯一的顶层屏创建点（归一屏单屏不变式，各内容页均已并入此体系）。 */
    s_kidpal_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(s_kidpal_scr, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(s_kidpal_scr, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(s_kidpal_scr, 0, 0);
    lv_obj_clear_flag(s_kidpal_scr, LV_OBJ_FLAG_SCROLLABLE);

    /* 动画 tick 定时器（在 LVGL 任务上下文触发，锁已持有）。仅重渲首页动画帧；
     * 前台/语音对账已由事件驱动 layer push/pop 接管（不再轮询 GetDeviceState）。 */
    s_tick_timer = lv_timer_create(
        [](lv_timer_t *t) {
            (void)t;
            kidpal_render_node_tick();
        },
        RN_TICK_MS,
        NULL
    );

    lvgl_port_unlock();
    ESP_LOGI(RN_TAG, "KidPal render node initialized (unified screen + layer stack)");
}

void kidpal_render_node_inject_rtc(int hour, int minute, int month, int mday)
{
    if (hour   >= 0 && hour   <= 23) { s_rtc_hour   = hour;   }
    if (minute >= 0 && minute <= 59) { s_rtc_minute = minute; }
    /* 日期扩展（M1-N1b）：0 = 未知（默认保持），1..12 / 1..31 合法值带入渲染态。 */
    if (month >= 1 && month <= 12) { s_rtc_month = month; }
    if (mday  >= 1 && mday  <= 31) { s_rtc_mday  = mday;  }
}

void kidpal_render_node_inject_env(int env_valid, int temp_c, int humidity_pct)
{
    s_env_valid    = (env_valid == 1) ? 1 : 0;
    if (temp_c      >= -40 && temp_c      <= 60)  { s_temp_c       = temp_c;       }
    if (humidity_pct >=  0 && humidity_pct <= 100) { s_humidity_pct = humidity_pct; }
}

void kidpal_render_node_inject_battery(int battery_pct, int charging)
{
    /* 包 260915-m1-n1b-p1-home-battery：板级 ADC 读数 -> 渲染状态写点（「s_battery_pct
     * 恒 100」销账——声明初始化器的 100 只是无读数时的保守默认，本函数是唯一运行期
     * 写点；三个 hp1_state 组装点 rn_render_home_locked / rn_render_kidpal_page_locked
     * HOME 分支 / kidpal_render_node_tick 继续消费）。 */
    if (battery_pct >= 0 && battery_pct <= 100) { s_battery_pct = battery_pct; }
    s_charging = (charging != 0) ? 1 : 0;
}

void kidpal_render_node_inject_weekday(int mon0_wday)
{
    /* M3-K2 星期注入缝（inject_rtc/inject_env/inject_battery 家族同款第四缝）：
     * 0..6 合法写入（周一=0..周日=6, 与 producer day 同基）; 越域忽略保持原值
     * （防御, 不崩）——未注入/越域时 P2 网格不画假「今天」高亮。 */
    if (mon0_wday >= 0 && mon0_wday <= 6) { s_rtc_wday = mon0_wday; }
}

int kidpal_render_node_today_col(void)
{
    return s_rtc_wday;
}

int kidpal_render_node_home_on_screen(void)
{
    /* P1 首页在屏 = LOCAL_PAGE 前台且当前页 = HOME（同 kidpal_render_node_tick
     * 首页动画重渲判据口径）; mcp home 路由「data.upcoming 注入成功且 P1 在屏 ->
     * 重渲染首页日程主体」的判据单一权威（M3-K2 A5 接线）。 */
    return (kidpal_layer_top(&s_layer_stack) == KIDPAL_LAYER_LOCAL_PAGE &&
            s_local_page == KP_PAGE_HOME) ? 1 : 0;
}

int kidpal_render_node_schedule_on_screen(void)
{
    /* P2 课表页在屏 = LOCAL_PAGE 前台且当前页 = SCHEDULE（home_on_screen 先例逐字
     * 同构; mcp schedule 路由 restore 分支「P2 在屏重绘本页 / 接管态仅填缓存」的判据
     * 单一权威, ADR-005 R10）。LOCAL_PAGE 判定使接管态（SERVER_PAGE 等顶层前台）
     * 一律返回 0——restore 只填缓存、不接管前台、不重绘。只读静态态, 零副作用。 */
    return (kidpal_layer_top(&s_layer_stack) == KIDPAL_LAYER_LOCAL_PAGE &&
            s_local_page == KP_PAGE_SCHEDULE) ? 1 : 0;
}

void kidpal_render_node_show_home(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        lvgl_port_unlock();
        ESP_LOGW(RN_TAG, "show_home called before init, skipping");
        return;
    }

    /* 回首页 = 前台回到本地浏览基层：出掉一切 transient 上层，基层 LOCAL_PAGE 独留。 */
    kidpal_layer_pop(&s_layer_stack, KIDPAL_LAYER_SERVER_PAGE);
    kidpal_layer_pop(&s_layer_stack, KIDPAL_LAYER_REMINDER);
    kidpal_layer_pop(&s_layer_stack, KIDPAL_LAYER_VOICE);
    kidpal_layer_push(&s_layer_stack, KIDPAL_LAYER_LOCAL_PAGE);

    rn_render_home_locked();
    rn_voice_chrome_front_locked();

    lvgl_port_unlock();

    ESP_LOGI(RN_TAG, "show_home: LOCAL_PAGE front (layer top=%d)",
             kidpal_layer_top(&s_layer_stack));
}

/* -------------------------------------------------------------------------
 * 语音轮 / 聊天层前台（layer 栈事件驱动，取代原 1s tick 状态对账）。
 * ---------------------------------------------------------------------- */
void kidpal_render_node_show_chat(void)
{
    lvgl_port_lock(0);

    /* 聊天内容落点恒 = 常驻聊天 layer（CHAT），非独立聊天屏（回答后屏幕落点确定）。
     * 本函数只把聆听/回答聊天面按下前台规则置前：推 VOICE 层，若服务端内容页在更上层
     * 则内容页保持前台、语音不拦内容页（SU-RULING-01 方案A）。 */
    if (kidpal_layer_chat_target() != KIDPAL_LAYER_CHAT) {
        ESP_LOGW(RN_TAG, "chat content target drifted from resident chat layer");
    }
    kidpal_layer_push(&s_layer_stack, KIDPAL_LAYER_VOICE);
    rn_reconcile_foreground_locked();
    rn_voice_chrome_front_locked();

    lvgl_port_unlock();
    ESP_LOGI(RN_TAG, "show_chat: VOICE layer push (top=%d)",
             kidpal_layer_top(&s_layer_stack));
}

void kidpal_render_node_voice_begin(void)
{
    lvgl_port_lock(0);

    /* 新语音轮开始 = 清除上一轮驻留的 transient（服务端回答页 / 提醒），再推 VOICE 层。
     * push 幂等 + 按优先级插入，故多次触发（Connecting 与按键预置）安全。 */
    int had_server = kidpal_layer_has(&s_layer_stack, KIDPAL_LAYER_SERVER_PAGE) == 1;
    kidpal_layer_pop(&s_layer_stack, KIDPAL_LAYER_SERVER_PAGE);
    kidpal_layer_pop(&s_layer_stack, KIDPAL_LAYER_REMINDER);
    kidpal_layer_push(&s_layer_stack, KIDPAL_LAYER_VOICE);

    /* 单物理屏内容回落：上一轮服务端回答页（课表/故事/查字）曾画在 s_kidpal_scr 上，
     * 清层后须把物理内容页从「服务端页」落回本地首页（否则聊天 chrome 迁 top layer 后，
     * 旧服务端页会留在唯一前台屏上、与「新语音轮清除驻留」矛盾）。聊天 chrome 在
     * lv_layer_top 不受影响，聆听级表情/状态仍恒可见。 */
    if (had_server && s_kidpal_scr != NULL) {
        rn_render_home_locked();
    } else {
        rn_reconcile_foreground_locked();
    }
    rn_voice_chrome_front_locked();

    lvgl_port_unlock();
    ESP_LOGI(RN_TAG, "voice_begin: new round (top=%d)",
             kidpal_layer_top(&s_layer_stack));
}

void kidpal_render_node_voice_end(void)
{
    lvgl_port_lock(0);

    if (kidpal_layer_has(&s_layer_stack, KIDPAL_LAYER_VOICE) != 1) {
        lvgl_port_unlock();
        return; /* 本无语音层（非语音轮结束）——不触发回首页重建 */
    }
    kidpal_layer_pop(&s_layer_stack, KIDPAL_LAYER_VOICE);

    if (kidpal_layer_top(&s_layer_stack) == KIDPAL_LAYER_LOCAL_PAGE) {
        /* 语音结束且无服务端页驻留 -> 回家页常显（重建首页帧再切前台）。
         * K3R D1/[R3] + D2/[R5]（261001-m4-k3-record-session-live-repair）: 例外——
         * 本地页 = P3 时前台回落 P3（**不**回家页）: 令键路由的 story_substate 见 P3
         * 页态（D4a）、录音页不被声波层压住（D2）、键帽随态正确。渲 P3 前以 [R5] 解析
         * 渲染页态（on_story_front 时取权威会话页态 session_page——录音 enter 路径已
         * cache_story 预同步 g_story_view 页态进节点缓存, 取代裸读可能陈旧的
         * s_story_view.page）, 并以解析结果喂 [R3] 的 page_state 入参（defer 缓存消费
         * 判据）与渲染页态（页态同步先于渲染）。VOICE 层已弹出, 故 [R3] voice_front 传 0。 */
        if (s_kidpal_scr != NULL) {
            int on_story_front = (s_local_page == KP_PAGE_STORY) ? 1 : 0;
            if (on_story_front) {
                int page_state = kidpal_story_render_page_state(s_story_view.page,
                                                                s_story_session_page,
                                                                on_story_front);
                s_story_view.page = page_state; /* 页态同步先于渲染（真值落缓存） */
                int list_flush = kidpal_story_list_deferred_flush(0, KP_PAGE_STORY,
                                                                  page_state);
                ESP_LOGI(RN_TAG, "voice_end: story front list_flush=%d page_state=%d",
                         list_flush, page_state);
                rn_render_kidpal_page_locked(KP_PAGE_STORY);
            } else {
                rn_render_home_locked();
            }
        }
    } else {
        /* 服务端回答页驻留前台（OQ3=c：回答后屏幕落点确定、非随机回退；语音不拦内容页）。 */
        rn_reconcile_foreground_locked();
    }
    rn_voice_chrome_front_locked();

    lvgl_port_unlock();
    ESP_LOGI(RN_TAG, "voice_end: top=%d", kidpal_layer_top(&s_layer_stack));
}

/* -------------------------------------------------------------------------
 * 服务端推送页面渲染的持锁夹钳（崩溃修复 包 260829-hardware-validation 语义保留，
 * 前台推进 layer 栈）。begin_page 推 SERVER_PAGE，begin_reminder 推 REMINDER（更高，
 * F8：提醒恰在语音轮到点也置前、不崩）；二者都持锁 + 切活动屏到 s_kidpal_scr，
 * 配对 end_page 释放锁。
 * ---------------------------------------------------------------------- */
void kidpal_render_node_begin_page(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        /* 未 init：保持持锁状态交调用方 end_page 释放，渲染函数自行降级。 */
        return;
    }
    /* 服务端内容页接管前台：提醒让位，推 SERVER_PAGE（> VOICE，回答期间内容页可见）。 */
    kidpal_layer_pop(&s_layer_stack, KIDPAL_LAYER_REMINDER);
    kidpal_layer_push(&s_layer_stack, KIDPAL_LAYER_SERVER_PAGE);
    /* 把活动屏切到 KidPal 专属屏，令页面渲染的 lv_screen_active() 取到此屏。 */
    lv_screen_load(s_kidpal_scr);
    /* 答案页接管前台：五态姿态/气泡退隐（chrome 显隐前台维度）。 */
    rn_voice_chrome_front_locked();
}

void kidpal_render_node_end_page(void)
{
    lvgl_port_unlock();
}

/* -------------------------------------------------------------------------
 * 查字窗渲染的持锁夹钳（TaskPackage 260917-m2-k3-char-window-overlay, M2-K3 页→窗改造）:
 * 与 begin_page/end_page 同形，但推的是 CHAR_WINDOW 层——查字窗是**会话内覆盖窗**、
 * 覆盖当前页之上（非常驻页, v1.36 撤 P4），键路由据此走查字窗列（KEY 短按 = 重播）、
 * 键帽条中槽页点经 front_is_overlay 自动隐藏。**不起会话 / 聆听**（无 ToggleChatState /
 * show_chat / voice_begin）——「播完会话即结束、不回聆听态」（§2.2 行 68）。
 * char_page 在活动屏（s_kidpal_scr）上叠加窗面板而不清屏，故关窗（dismiss_front_overlay
 * 清 transient 回基层）后原页原样回来、不翻页。
 * ---------------------------------------------------------------------- */
void kidpal_render_node_begin_char_window(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        /* 未 init：保持持锁状态交调用方 end_char_window 释放，渲染函数自行降级。 */
        return;
    }
    /* 查字窗接管前台：推 CHAR_WINDOW（覆盖当前页之上、REMINDER 之下——提醒仍压过窗）。
     * 不 pop 任何既有层：提醒弹窗等上层驻留时保持原位（层级由 kidpal_layer 优先级承载）。 */
    kidpal_layer_push(&s_layer_stack, KIDPAL_LAYER_CHAR_WINDOW);
    lv_screen_load(s_kidpal_scr);
    /* 五态姿态/气泡退隐（chrome 显隐前台维度）；中槽页点隐藏（覆盖层不显示页点）。 */
    rn_voice_chrome_front_locked();
    rn_keybar_sync_locked();
}

void kidpal_render_node_end_char_window(void)
{
    lvgl_port_unlock();
}

/* -------------------------------------------------------------------------
 * P3 删除确认覆盖层的持锁夹钳（TaskPackage 260924-m4-k2-p3-star-notebook-ui,
 * seam [K10]; WF-F2-05 整屏覆盖层）：与 begin_char_window/end_char_window 同形,
 * 但推的是 STORY_DELETE 层——删除确认覆盖当前页之上、提醒之下（提醒最高层压删除
 * 弹窗, 序不变），键路由据此把全部手势先交故事页裁决（板级前置拦截）、键帽条中槽
 * 页点经 front_is_overlay 自动隐藏。关层（确认/取消/10s idle 静默取消）统一走
 * dismiss_front_overlay（pop_transients 已纳入本层——kidpal_layer.c seam [K10]）。
 * 覆盖层内容（「删掉这篇？」+ 待删条目）由 mcp_server 在本夹钳内经
 * kidpal_story_page_render(DELETE) 绘制。
 * ---------------------------------------------------------------------- */
void kidpal_render_node_begin_story_delete(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        /* 未 init：保持持锁状态交调用方 end_story_delete 释放，渲染函数自行降级。 */
        return;
    }
    /* 删除确认接管前台：推 STORY_DELETE。不 pop 任何既有层：提醒弹窗等上层驻留时
     * 保持原位（层级由 kidpal_layer 优先级承载）。 */
    kidpal_layer_push(&s_layer_stack, KIDPAL_LAYER_STORY_DELETE);
    lv_screen_load(s_kidpal_scr);
    /* 五态姿态/气泡退隐（chrome 显隐前台维度）；中槽页点隐藏（覆盖层不显示页点）。 */
    rn_voice_chrome_front_locked();
    rn_keybar_sync_locked();
}

void kidpal_render_node_end_story_delete(void)
{
    lvgl_port_unlock();
}

void kidpal_render_node_begin_reminder(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        return;
    }
    /* 提醒弹窗进 layer 优先级表最高层之一（REMINDER > VOICE/SERVER_PAGE）：恰在语音轮
     * 到点也置前、不崩（review F8 架构修复）。 */
    kidpal_layer_push(&s_layer_stack, KIDPAL_LAYER_REMINDER);
    lv_screen_load(s_kidpal_scr);
    /* 提醒置前：五态姿态/气泡退隐。 */
    rn_voice_chrome_front_locked();
}

/* -------------------------------------------------------------------------
 * 配网 layer（SU-A2 残项补全）：配网 = 前台优先级最高层（PROVISION=7），配网进/退由
 * WifiBoard 配网模式进/退事件驱动。两个入口各自纯进/纯退、独立持锁释放（进在按钮任务 /
 * 网络回调任务、退在 WifiManager 事件回调，跨任务不成对调用，故不像 begin_page/end_page
 * 那样配锁）。
 * ---------------------------------------------------------------------- */
void kidpal_render_node_begin_provision(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        /* 非 KidPal 归一屏板（render_node 未 init）或未初始化：纯 no-op（layer 栈仅存于
         * KidPal 渲染节点，其它板无单屏体系）。 */
        lvgl_port_unlock();
        return;
    }
    /* 配网进 layer 优先级表最高层：PROVISION(7) > REMINDER(6) > CHAR_WINDOW(5) >
     * SERVER_PAGE(4) > VOICE(3) > LOCAL_PAGE(2)。幂等去重，重复进入安全。 */
    kidpal_layer_push(&s_layer_stack, KIDPAL_LAYER_PROVISION);
    lv_screen_load(s_kidpal_scr);

    lvgl_port_unlock();
    ESP_LOGI(RN_TAG, "begin_provision: PROVISION push (top=%d)",
             kidpal_layer_top(&s_layer_stack));
}

void kidpal_render_node_end_provision(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        lvgl_port_unlock();
        return;
    }
    /* 配网退 layer：不在场 pop 返回 -1（no-op），按优先级表回落。 */
    kidpal_layer_pop(&s_layer_stack, KIDPAL_LAYER_PROVISION);
    rn_reconcile_foreground_locked();
    rn_voice_chrome_front_locked();

    lvgl_port_unlock();
    ESP_LOGI(RN_TAG, "end_provision: PROVISION pop (top=%d)",
             kidpal_layer_top(&s_layer_stack));
}

/* -------------------------------------------------------------------------
 * 阶段 B：BOOT 双按翻页（本地浏览层生命周期）
 * ---------------------------------------------------------------------- */

/* 渲染指定 KidPal 页到 s_kidpal_scr（干净重建 + 注入缓存数据）。供 cycle_next 翻页与
 * dismiss_front_overlay 关层回原页共用（单一页面渲染权威，KD-A2 清层/进页分离后翻页
 * 逻辑一并收敛于此）。必须持有 lvgl_port_lock。本函数只画内容页——页面归属由调用方
 * 经 layer 栈裁决，函数体零层栈副作用（不含任何 kidpal_layer_pop）。 */
static void rn_render_kidpal_page_locked(kidpal_page_id_t page)
{
    /* K3R D2 真机轮诊断（纯日志, 零渲染改动）: 页渲染入口——入参 page + 本地页 +
     * 故事节点缓存/权威会话页态 + 活动屏是否已切到 s_kidpal_scr。 */
    ESP_LOGI(RN_TAG, "PAGE_RENDER in=%d local=%d sview=%d ssess=%d active_ok=%d",
             (int)page, s_local_page, s_story_view.page, s_story_session_page,
             (s_kidpal_scr != NULL && lv_screen_active() == s_kidpal_scr) ? 1 : 0);

    lv_obj_clean(s_kidpal_scr);
    s_kidpal_img = NULL; /* clean 已销毁，避免悬垂 */

    switch (page) {
        case KP_PAGE_HOME: {
            /* show_home 内联逻辑；s_kidpal_scr 已 clean，不必二次 clean。 */
            s_kidpal_img = lv_image_create(s_kidpal_scr);
            lv_obj_set_size(s_kidpal_img, 400, 300);
            lv_obj_set_pos(s_kidpal_img, 0, 0);
            hp1_state_t st = {};
            st.hour         = s_rtc_hour;
            st.minute       = s_rtc_minute;
            st.month        = s_rtc_month;
            st.mday         = s_rtc_mday;
            st.wday         = s_rtc_wday; /* P1 日期星期（2026-09-26 对齐轮） */
            st.env_valid    = s_env_valid;
            st.temp_c       = s_temp_c;
            st.humidity_pct = s_humidity_pct;
            st.battery_pct  = s_battery_pct;
            st.charging     = s_charging;
            st.tick_ms      = static_cast<int>(esp_timer_get_time() / 1000);
            kidpal_home_p1_set_state(&st);
            kidpal_home_p1_render_frame();
            lv_screen_load(s_kidpal_scr);
            s_local_page = KP_PAGE_HOME;
            ESP_LOGI(RN_TAG, "render page HOME (P1)");
            break;
        }
        case KP_PAGE_SCHEDULE: {
            /* 把 s_kidpal_scr 设为活动屏，kidpal_schedule_grid_render 在
             * lv_screen_active() 上清屏并绘制一周总览网格（7 列×3 行）。 */
            lv_screen_load(s_kidpal_scr);
            if (s_grid_n <= 0) {
                /* #11 数据链根因 (R-S0.2): 缓存仅在云端推送过周网格后才填充; 孩子
                 * BOOT 翻页切来时可能本开机未收到 -> 空缓存。改画友好空态 (居中
                 * 提示), 不再渲染空壳页; 云端推送路径 (mcp kidpal/display schedule
                 * 路由) 不动。 */
                lv_obj_t *lbl = lv_label_create(s_kidpal_scr);
                lv_label_set_text(lbl, "还没有课表数据，问我『我的课表』吧");
                lv_obj_set_style_text_color(lbl, lv_color_black(), 0);
                lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_set_width(lbl, 380);
                lv_label_set_long_mode(lbl, LV_LABEL_LONG_WRAP);
                lv_obj_center(lbl);
            } else {
                /* P2 一周总览网格（today_col = 注入星期列; 未注入/越域 -> 网格面
                 * 自行不画假「今天」高亮）。 */
                kidpal_schedule_grid_render(s_grid_courses, s_grid_n, s_rtc_wday);
            }
            s_local_page = KP_PAGE_SCHEDULE;
            ESP_LOGI(RN_TAG, "render page SCHEDULE (P2 grid) courses=%d wday=%d",
                     s_grid_n, s_rtc_wday);
            break;
        }
        case KP_PAGE_STORY: {
            lv_screen_load(s_kidpal_scr);
            if (s_story_list.count <= 0) {
                /* #11 数据链根因同族 (R-S0.2, schedule 空态同款): 故事缓存仅在语音
                 * 查询过故事后才填充; BOOT 翻页切来时可能本开机未查询 -> count=0
                 * 空壳页。改画友好空态 (居中提示), 不再渲染空壳页; 语音查询路径
                 * 不动。文案单源 = KSTORY_EMPTY_HINT（notes 边角③: 空态渲染入口
                 * 两处——本处与 story_page.c——必须同源一致, 不得双写文案）。 */
                lv_obj_t *lbl = lv_label_create(s_kidpal_scr);
                lv_label_set_text(lbl, KSTORY_EMPTY_HINT);
                lv_obj_set_style_text_color(lbl, lv_color_black(), 0);
                lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_set_width(lbl, 380);
                lv_label_set_long_mode(lbl, LV_LABEL_LONG_WRAP);
                lv_obj_center(lbl);
            } else {
                kidpal_story_page_render(&s_story_view, &s_story_list, &s_story_text);
            }
            s_local_page = KP_PAGE_STORY;
            ESP_LOGI(RN_TAG, "render page STORY (P3) count=%d", s_story_list.count);
            break;
        }
        default:
            s_local_page = KP_PAGE_HOME;
            break;
    }

    /* 各基础页落屏后驱动键帽条三槽刷新 (keybar R1 / ①根修: 页槽表与页点随翻页更新,
     * 覆盖层关层回原页也走本函数)。调用方已持 lvgl 锁。 */
    rn_keybar_sync_locked();

    /* K3R D2 真机轮诊断（纯日志）: 页渲染出口——本地页按入参 page 定后回读。 */
    ESP_LOGI(RN_TAG, "PAGE_RENDER out=%d local=%d sview=%d ssess=%d",
             (int)page, s_local_page, s_story_view.page, s_story_session_page);
}

/* 前台归一化查询（layer 栈 owner 侧，层 top 单一裁决）：前台 owner ∈ 覆盖层
 * （VOICE / SERVER_PAGE / REMINDER / CHAR_WINDOW / STORY_DELETE）-> 1；LOCAL_PAGE /
 * PROVISION / CHAT / NONE -> 0。（2026-09-19 M2-K3：闭集纳入 CHAR_WINDOW——查字窗
 * 前台时页点隐藏与键路由同源。2026-09-29 seam [K10]：闭集纳入 STORY_DELETE——删除
 * 确认覆盖层前台时页点隐藏（WF-F2-05 无页点）与键路由同源成立。） */
int kidpal_render_node_front_is_overlay(void)
{
    int top = kidpal_layer_top(&s_layer_stack);
    return (top == KIDPAL_LAYER_VOICE ||
            top == KIDPAL_LAYER_SERVER_PAGE ||
            top == KIDPAL_LAYER_REMINDER ||
            top == KIDPAL_LAYER_CHAR_WINDOW ||
            top == KIDPAL_LAYER_STORY_DELETE) ? 1 : 0;
}

/* 前台顶层是否为会话类覆盖层（VOICE）：OVERLAY_CLOSE 关层级联判据（KD-A5 真机 r1
 * 修复）。只认 VOICE 单层——提醒弹窗 / 服务端推送页等非会话覆盖层关层语义不变
 * （本就无会话）。与 front_is_overlay 同为无锁整数读，板级 dispatch 消费。 */
int kidpal_render_node_front_is_voice(void)
{
    return kidpal_layer_top(&s_layer_stack) == KIDPAL_LAYER_VOICE ? 1 : 0;
}

/* 前台顶层是否为提醒弹窗覆盖层（REMINDER）：提醒材料外壳显隐判据（K4, 关窗自动回落）。 */
int kidpal_render_node_front_is_reminder(void)
{
    return kidpal_layer_top(&s_layer_stack) == KIDPAL_LAYER_REMINDER ? 1 : 0;
}

/* 当前前台顶层 layer 值（提醒门控 kidpal_reminder_gate 的 front_layer 入参单一权威）。 */
int kidpal_render_node_front_layer(void)
{
    return kidpal_layer_top(&s_layer_stack);
}

/* 设备侧显式关层步骤（「关窗」唯一入口，KD-A2 清层/进页分离）：前台为覆盖层时经
 * 显式清层步骤出掉 transient 覆盖层回基层，并重渲基层当前页——关层归关层、不产翻页
 * 动作（v1.39 覆盖层隔离口径）。内部消费 kidpal_layer_pop_transients（单一清层事实源，
 * 不另写逐层 pop 循环）。 */
void kidpal_render_node_dismiss_front_overlay(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        lvgl_port_unlock();
        return;
    }

    if (kidpal_render_node_front_is_overlay() != 0) {
        (void)kidpal_layer_pop_transients(&s_layer_stack);
        kidpal_layer_push(&s_layer_stack, KIDPAL_LAYER_LOCAL_PAGE);
        rn_render_kidpal_page_locked(static_cast<kidpal_page_id_t>(s_local_page));
    }
    rn_voice_chrome_front_locked();

    lvgl_port_unlock();
    ESP_LOGI(RN_TAG, "dismiss_front_overlay: LOCAL_PAGE front (layer top=%d)",
             kidpal_layer_top(&s_layer_stack));
}

/* 板级任意键手势归零查字窗驻留计时（seam [9]e; requirements v1.43「计时随任意按键重计」）。
 * 由板级 KEY/BOOT 按键按下回调调用（waveshare-s3-rlcd-4.2.cc）——置「本拍有按键」标志，
 * tick 内作为 key_pressed 喂 kidpal_char_window_idle_step 使计时归零重计。无锁整数写，
 * 可跨任务调用；非查字窗前台时该标志被 tick 取出即弃（零副作用）。 */
void kidpal_render_node_char_window_key_hit(void)
{
    s_char_window_key_hit = 1;
}

void kidpal_render_node_cycle_next(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        lvgl_port_unlock();
        ESP_LOGW(RN_TAG, "cycle_next called before init");
        return;
    }

    /* 手动翻页 = 用户主动接管导航：经显式清层步骤（kidpal_layer_pop_transients）出掉
     * transient 前台（服务端页/提醒/语音）回基层——「关窗」不再依赖本函数开头逐层
     * kidpal_layer_pop 的副作用（requirements §2.3 对账 ①, KD-A2 清层/进页分离；清层
     * 只住显式步骤），进页归进页。 */
    (void)kidpal_layer_pop_transients(&s_layer_stack);
    kidpal_layer_push(&s_layer_stack, KIDPAL_LAYER_LOCAL_PAGE);

    kidpal_page_id_t next = static_cast<kidpal_page_id_t>(
        (static_cast<int>(s_local_page) + 1) % KP_PAGE_COUNT);

    rn_render_kidpal_page_locked(next);

    /* 浏览页前台：五态姿态/气泡退隐（chrome 显隐前台维度）。 */
    rn_voice_chrome_front_locked();

    lvgl_port_unlock();
}

int kidpal_render_node_get_page(void)
{
    return static_cast<int>(s_local_page);
}

void kidpal_render_node_tick(void)
{
    /* 5 分钟驻留计时的设备侧驱动（seam [9], M2K3-A3 驱动半）：落地面 = 本既有 1s tick
     * （不另起 timer）。仅当查字窗为前台（KIDPAL_LAYER_CHAR_WINDOW 门控）时累计与到点
     * 关窗——非查字窗前台把累计器归零，否则窗一开首拍即因历史累计越限而立即误关窗。
     * 到点（step 返回 1）经关窗唯一入口 kidpal_render_node_dismiss_front_overlay 关窗
     * （回原页、不翻页，与 BOOT/KEY 长按同语义）。
     * 时序安全：本函数由 LVGL 定时器（esp_lvgl_port task）调用、锁已持有；lvgl_port_lock
     * 为**递归互斥量**（LVGL9 端口 xSemaphoreCreateRecursiveMutex），dismiss 内部再取锁
     * 为同任务递归获取、不阻塞不死锁——故本区内直调 dismiss 安全。
     * 注意：本驱动必须在下方 early-return 之前——查字窗前台时 layer top = CHAR_WINDOW，
     * 会被那句「top != LOCAL_PAGE 即 return」挡掉，放在其后则驱动永不执行。 */
    {
        int key_pressed = s_char_window_key_hit;
        s_char_window_key_hit = 0; /* 本拍消费，未消费也清位（防跨窗陈旧置位） */
        if (kidpal_layer_top(&s_layer_stack) == KIDPAL_LAYER_CHAR_WINDOW) {
            if (kidpal_char_window_idle_step(&s_char_window_idle_ms, RN_TICK_MS,
                                             key_pressed) == 1) {
                ESP_LOGI(RN_TAG, "char window idle timeout (5min no-op) -> dismiss");
                kidpal_render_node_dismiss_front_overlay();
            }
        } else {
            s_char_window_idle_ms = 0; /* 非查字窗前台: 不计时（防历史累计误关窗） */
        }
    }

    /* 首页动画重渲（冒号/电池闪烁）：仅当本地首页是前台 owner 且图像控件存活时触发。
     * 由 LVGL 定时器（esp_lvgl_port task）调用，锁已持有。前台归属由 layer 栈
     * top() 单一裁决（服务端页/提醒/语音等更上层在场时不重渲首页，也不会误触
     * 已被页面渲染 clean 销毁的 s_kidpal_img）。此处不存在轮询 GetDeviceState 对账。 */
    if (s_local_page != KP_PAGE_HOME || s_kidpal_img == NULL ||
        kidpal_layer_top(&s_layer_stack) != KIDPAL_LAYER_LOCAL_PAGE) {
        return;
    }
    hp1_state_t st = {};
    st.hour         = s_rtc_hour;
    st.minute       = s_rtc_minute;
    st.month        = s_rtc_month;
    st.mday         = s_rtc_mday;
    st.wday         = s_rtc_wday; /* P1 日期星期（2026-09-26 对齐轮） */
    st.env_valid    = s_env_valid;
    st.temp_c       = s_temp_c;
    st.humidity_pct = s_humidity_pct;
    st.battery_pct  = s_battery_pct;
    st.charging     = s_charging;
    st.tick_ms      = static_cast<int>(esp_timer_get_time() / 1000);
    kidpal_home_p1_set_state(&st);
    kidpal_home_p1_render_frame();
}

/* -------------------------------------------------------------------------
 * 缓存 setter（供 mcp_server.cc 在收到服务端数据后调用）
 * ---------------------------------------------------------------------- */

extern "C" void kidpal_render_node_cache_schedule_grid(const kidpal_schedule_course_t *courses,
                                                       int n)
{
    /* cache_schedule 网格化（M3-K2）：n<=0 或 courses==NULL 时清空缓存 = 空态
     * （BOOT 翻页 cycle_next 落友好空态画面, 绝不盲渲染）; n > KSG_COURSE_CAP
     * 只拷前 cap 条, 绝不溢出（定容 7 天×3 格×每格 4 门冻结值）。 */
    if (n <= 0 || courses == NULL) {
        s_grid_n = 0;
        return;
    }
    int copy = (n > KSG_COURSE_CAP) ? KSG_COURSE_CAP : n;
    memcpy(s_grid_courses, courses, static_cast<size_t>(copy) * sizeof(s_grid_courses[0]));
    s_grid_n = copy;
}

extern "C" void kidpal_render_node_cache_story(const kidpal_story_view_t *v,
                                                const kidpal_story_list_t *l,
                                                const kidpal_story_text_t *t)
{
    if (v != NULL) { memcpy(&s_story_view, v, sizeof(s_story_view)); }
    if (l != NULL) { memcpy(&s_story_list, l, sizeof(s_story_list)); }
    if (t != NULL) { memcpy(&s_story_text, t, sizeof(s_story_text)); }
}

/* -------------------------------------------------------------------------
 * P3 本地导航重渲 + 子态查询（TaskPackage 260924-m4-k2-p3-star-notebook-ui 双通道
 * 方案：列表/阅读/盖章 = P3 本地页内容, 前台保持 LOCAL_PAGE -> 页点自然可见;
 * 云端推送路径维持 begin_page 现状——双通道拆分, plan「P3 子态 × 层模型 × 页点」）。
 * ---------------------------------------------------------------------- */
void kidpal_render_node_refresh_story(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        lvgl_port_unlock();
        return;
    }
    /* 本地导航落屏：消费 cache_story 缓存重渲基层当前页（P3 键路由补全后的页内
     * 输入动作——光标/跳屏/阅读翻屏/退出阅读——均经本函数落屏, 不推 SERVER_PAGE）。
     * 层栈零改动：前台保持现状（本地导航不接管前台、不关任何覆盖层——删除确认
     * 覆盖层的关层走 dismiss_front_overlay 唯一入口）。 */
    rn_render_kidpal_page_locked(KP_PAGE_STORY);

    lvgl_port_unlock();
}

void kidpal_render_node_story_local_front(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        lvgl_port_unlock();
        return;
    }
    /* 云端渲染夹钳（begin_page）用过后的前台回落基层：SERVER_PAGE 出栈、LOCAL_PAGE
     * 回位 + 键帽条刷新（页点可见性随 front_is_overlay 恢复）。只回落本夹钳产生的
     * SERVER_PAGE 单层——REMINDER / STORY_DELETE 等其它层不碰（各自唯一入口管理）。 */
    kidpal_layer_pop(&s_layer_stack, KIDPAL_LAYER_SERVER_PAGE);
    kidpal_layer_push(&s_layer_stack, KIDPAL_LAYER_LOCAL_PAGE);
    rn_keybar_sync_locked();

    lvgl_port_unlock();
}

int kidpal_render_node_story_substate(void)
{
    /* 删除确认覆盖层为前台 -> 删除子态（覆盖层身份与 front_is_overlay 同源）。 */
    if (kidpal_layer_top(&s_layer_stack) == KIDPAL_LAYER_STORY_DELETE) {
        return KSTORY_PAGE_DELETE;
    }
    /* 本地基层当前页 = STORY -> 缓存视图的页态（LIST/READER/RECORDING/STAMP）。
     * 前台为其它覆盖层（VOICE/SERVER_PAGE/REMINDER/CHAR_WINDOW）时缓存页态不可
     * 见 -> -1（消费方回落既有键帽路径, 不抢语音 chrome 键帽）。 */
    if (kidpal_layer_top(&s_layer_stack) != KIDPAL_LAYER_LOCAL_PAGE ||
        s_local_page != KP_PAGE_STORY) {
        return -1;
    }
    return s_story_view.page;
}