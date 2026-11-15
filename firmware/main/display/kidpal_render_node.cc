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
 *  4. 阶段 B：BOOT 双按翻页（P1 首页 → P2 课表 → P3 星星积分本 → P4 星星记录本 → 回 P1
 *     四页循环，TaskPackage 261104-k0a-four-page-skeleton / D-01），
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
#include "kidpal_font.h"   /* 统一 kidpal_font 句柄 seam（261028-m6-k1，[K4] 消费口） */
#include "art_assets.h"    /* K2 积分本图标（pt_cat / ic_coin）经 art 单一资产层装载 */
#include "diagnostics/log_semantics.h" /* 261022-diag-log-content: PAGE_SWITCH 页名/触发名 */

#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_lvgl_port.h>
#include <lvgl.h>

#include <cstdio>
#include <cstring>
#include <string>

static const char *RN_TAG = "kidpal_rn";

/* PAGE_SWITCH 触发源闭集（261022-diag-log-content [DLG-C2]；语义名映射归
 * diagnostics/log_semantics.c kidpal_log_trigger_name——本文件只注值、不另写名表）。 */
#define RN_PAGE_TRIGGER_KEY   0 /* 按键路径：键路由翻页 / 关层回原页 / 页内导航 */
#define RN_PAGE_TRIGGER_TIMER 1 /* 定时器路径 */
#define RN_PAGE_TRIGGER_NET   2 /* 网络 / 云端推送路径（含语音轮收尾的页面回落） */

/* 下一次页面跳转的触发源（默认按键路径）：调用方在调 rn_render_kidpal_page_locked
 * 之前经 rn_note_page_switch_trigger 标注，渲染权威体内读一次即复位。
 * 为什么不给渲染函数加参数：retract_p4_three_page.acceptance.c 的 M2K2-A1 冻结了该
 * 函数的签名文本锚 `static void rn_render_kidpal_page_locked(kidpal_page_id_t page)`
 * ——改签名会打红既有冻结回归网（本包零新增红的硬约束）。单写者 = 持 lvgl 锁的调用方，
 * 读点在渲染体内（同任务、同锁区间），无跨任务竞争。 */
static int s_page_switch_trigger = RN_PAGE_TRIGGER_KEY;

static void rn_note_page_switch_trigger(int trigger)
{
    s_page_switch_trigger = trigger;
}

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

/* P3 列表数据是否已送达（seam [SL3], TaskPackage 261010-p3-storylist-cold-start-restore）:
 * 「有无列表数据」判据载体——列表缓存经 kidpal_render_node_cache_story 装载过 = 1。
 * 空态两态分叉用: 1（数据已送达、确无篇目）= AC5 无内容空态（×0 + AC5 话术）;
 * 0（列表缓存从未装载 = 重启后恢复推送未到/失败）= AC6 数据未达空态（不显计数 +
 * AC6 话术）。判据落在「有无列表数据」而非「断网与否」（requirements v1.63 逐字）。 */
static int s_story_data_received = 0;

/* K3R [R5] 权威会话页态镜像（261001-m4-k3-record-session-live-repair, 真机轮架构修）:
 * 渲染节点自身不持 g_story_view（住 mcp_server.cc）, 由 mcp 录音/收尾路径经本 C 桥写入
 * 权威会话页态——voice_end 的 [R5] 以此作 session_page 权威源, 与节点缓存分叉时取之
 * （取代「裸读节点缓存」的潜伏 bug; 不再依赖 cache_story 掩盖）。 */
static int s_story_session_page = 0;
extern "C" void kidpal_render_node_story_session_page(int page) {
    s_story_session_page = page;
}

/* ------------------------------------------------------------------------- *
 * P3 星星积分本渲染缓存（TaskPackage 261104-k2-points-device-ui 轮 2）
 *
 * 权威帧由 mcp_server 消费链解析后经 cache_points_* 投递（与 cache_story 同型分工：
 * 渲染节点只持有渲染所需 RAM 副本，四态/进度线/分页判定全部由 points_page.c 决策表在
 * mcp 侧裁决，渲染面零重算、零本地持久化——plan 铁律「K1 唯一权威」）。
 * s_points_home_received = 「数据已送达」判据（未达 = 从未收到权威帧 -> K0a 占位口径，
 * 不显数字/不画进度线；与「确认空账」两态显式分叉，S-F6.1 AC4）。
 * ---------------------------------------------------------------------- */
static kidpal_points_home_t        s_points_home = {};
static int                         s_points_home_received = 0;
static kidpal_points_ledger_t      s_points_ledger = {};
static kidpal_points_ledger_state_t s_points_state = {};
static int                         s_points_ledger_received = 0;
static int                         s_points_cursor = 0;

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

/* [S7] 字面源缝设备强定义（P1，[F1]）：经 P0 透传口取统一句柄 16px body。 */
extern "C" const lv_font_t *kidpal_p1_font_face(void)
{
    return kidpal_render_node_body_font(); /* P0 [K4] 透传（16px body / 缺资产 14px） */
}

extern "C" int kidpal_p1_font_glyph(uint32_t codepoint, kidpal_p1_glyph_t *out)
{
    /* [S1] 字面源切 [S7]（16px body 句柄，[F1]）；NULL 回落编译内 font_puhui_14_1
     * （缺 assets 降级 / F-1 边界；FAS-A7 render_node_puhui14 canary 不破——符号引用
     * 在世）。[S1] 签名/字形语义零改（共享核心 p1_glyph_render_core 不变）。 */
    const lv_font_t *face = kidpal_p1_font_face();
    if (face == nullptr) {
        face = &font_puhui_14_1;
    }
    return p1_glyph_render_core(face, codepoint, out);
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

/* 前置声明（定义在后方 D2 R1 根修区; show_home / voice_begin / voice_end 三守卫
 * 消费点在其定义之前）。 */
static int rn_record_session_live_locked(void);

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

int kidpal_render_node_story_on_screen(void)
{
    /* P3 故事列表页在屏 = LOCAL_PAGE 前台且当前页 = STORY（home_on_screen /
     * schedule_on_screen 先例逐字同构; mcp story_list 路由 restore 分支「P3 在屏仅
     * 重绘本页 / 记录语境前台与接管态只填缓存」的判据单一权威, seam [SL2]）。
     * LOCAL_PAGE 判定使记录语境前台（VOICE 层 = 引导态/录音态/问答会话）与接管态
     * （SERVER_PAGE 等非本地页前台）一律返回 0——restore 帧只填缓存、不重绘、不推
     * 前台（K3 [S4] 叠加语义的谓词承载）。只读静态态, 零副作用。 */
    return (kidpal_layer_top(&s_layer_stack) == KIDPAL_LAYER_LOCAL_PAGE &&
            s_local_page == KP_PAGE_STORY) ? 1 : 0;
}

void kidpal_render_node_show_home(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        lvgl_port_unlock();
        ESP_LOGW(RN_TAG, "show_home called before init, skipping");
        return;
    }

    /* D2 真机轮 R1 守卫（261019-p3-record-dialogue-guide fixer）: 记录会话覆盖演出
     * 在世（录音/整理中）-> 首页重建降级 no-op（屏上真相 = P3 会话页, 服务端 home
     * 推送/键长按等入口不得把基层翻回 P1——重建即演出冻结 + s_local_page 陈旧链）。
     * 会话收尾（Idle 分支清 ctx / [R2] 复位 LIST）之后本守卫自然放行。 */
    if (rn_record_session_live_locked()) {
        lvgl_port_unlock();
        ESP_LOGW(RN_TAG,
                 "show_home suppressed: record session live (page=%d corr_id=kdp-%u)",
                 s_story_session_page,
                 static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
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
     * lv_layer_top 不受影响，聆听级表情/状态仍恒可见。
     * D2 R1 守卫: 记录会话在世时**不**回首页重建（屏上真相 = P3 会话页——录音/整理
     * 演出不因新语音轮被首页帧覆盖; 只收盾物理屏）。 */
    if (had_server && s_kidpal_scr != NULL && !rn_record_session_live_locked()) {
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
            /* D2 R1 判据放宽（261019 fixer）: on_story_front = 「本地页 = P3」或
             * 「记录会话在世」。后者兜 s_local_page 被中途翻走的残缝（真值 = 权威
             * 会话页态镜像 s_story_session_page, [R5] 解析消费点语义不变）——记录
             * 语境下 voice_end 恒不出 else 首页重建分支（现场 1 崩链的前提之一）。 */
            int on_story_front = (s_local_page == KP_PAGE_STORY ||
                                  rn_record_session_live_locked()) ? 1 : 0;
            if (on_story_front) {
                int page_state = kidpal_story_render_page_state(s_story_view.page,
                                                                s_story_session_page,
                                                                on_story_front);
                s_story_view.page = page_state; /* 页态同步先于渲染（真值落缓存） */
                int list_flush = kidpal_story_list_deferred_flush(0, KP_PAGE_STORY,
                                                                  page_state);
                ESP_LOGI(RN_TAG, "voice_end: story front list_flush=%d page_state=%d",
                         list_flush, page_state);
                rn_note_page_switch_trigger(RN_PAGE_TRIGGER_NET);
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
 * P3 星星积分本渲染面（TaskPackage 261104-k2-points-device-ui 轮 2）
 *
 * 版式权威 = docs/ui/V9/ 六张 400×300 1bpp 帧（points-home / points-home-empty /
 * points-home-nodata / points-home-negative / points-ledger / points-delete-confirm）+
 * docs/ui/kidpal-ui-design-v9.html；与 story/schedule 既有正式页同族画法（纯 lv_obj
 * 矩形 + lv_label，1bpp 阈值下墨色即黑/白两值，长原因 → LV_LABEL_LONG_DOT 截断、
 * 大分值/越目标/负余额 → 坐标钳位，绝不越出 400×300）。
 *
 * 数据面零重算：四态判定 kidpal_points_home_state、进度线 kidpal_points_progress、
 * 屏幕数 kidpal_points_ledger_screen_total 全住 display/points_page.c；本区只消费。
 * 未达态（s_points_home_received == 0）由调用点内联画 K0a 占位（KP_POINTS_NODATA_HINT，
 * 不显数字/不画进度线），**不经本区**（K0A-A4 冻结结构锚）。
 * ---------------------------------------------------------------------- */
#define RNP_INK        0x000000u
#define RNP_FAINT      0x888888u
#define RNP_LINE_X0    30
#define RNP_LINE_X1    370
#define RNP_LINE_Y     100

static lv_obj_t *rn_points_label(lv_obj_t *scr, const char *text, int x, int y,
                                 uint32_t color, const lv_font_t *font)
{
    lv_obj_t *l = lv_label_create(scr);

    lv_label_set_text(l, text != NULL ? text : "");
    lv_obj_set_pos(l, x, y);
    lv_obj_set_style_text_color(l, lv_color_hex(color), 0);
    lv_obj_set_style_text_font(l, font != NULL ? font : &font_puhui_14_1, 0);
    return l;
}

static void rn_points_box(lv_obj_t *scr, int x, int y, int w, int h, uint32_t color)
{
    lv_obj_t *o = lv_obj_create(scr);

    lv_obj_remove_style_all(o);
    lv_obj_set_size(o, w, h);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_style_bg_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
}

/* 圆角描边框（列表选中行 / 删除确认分录框）。 */
static void rn_points_outline(lv_obj_t *scr, int x, int y, int w, int h)
{
    lv_obj_t *o = lv_obj_create(scr);

    lv_obj_remove_style_all(o);
    lv_obj_set_size(o, w, h);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_style_bg_opa(o, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(o, 3, 0);
    lv_obj_set_style_border_color(o, lv_color_hex(RNP_INK), 0);
    lv_obj_set_style_border_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(o, 12, 0);
    lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
}

/* 积分本图标资产（K2 真机轮 F2/F5）：把 KAP1 1bpp 经 art 单一资产层
 * (kidpal_art_load + kidpal_art_pixel) 合成 ARGB8888 lv_image_dsc（墨=不透明黑 /
 * 纸=透明）。与 story_page.c 的 kstory_art_dsc 同款先例：单槽 loader 缓存即时拷入
 * 自建缓冲后即释放引用；每资产独立缓存槽（pt_cat / ic_coin / 场景标），零互相逐出。
 * 资产不可用（未 mount / 文件缺 / 尺寸超上限）-> NULL，调用方回落既有几何/文字画法。
 *
 * 方案 A（TaskPackage 261104-k4-reward-goal-config）：场景标随激活目标变化，槽位按
 * **名字**（strcmp）命中；同名 = 直接复用，名字变化 = 释放旧像素缓冲后按名重载
 * （复用同一槽，RAM 零增长）。三个槽位固定分工，场景标换目标不会逐出 pt_cat/coin。 */
#define RN_POINTS_ART_SLOT_CAT   0   /* pt_cat_34_28 */
#define RN_POINTS_ART_SLOT_COIN  1   /* ic_coin_36_36 */
#define RN_POINTS_ART_SLOT_SCENE 2   /* 场景标：随激活目标按名重载 */
#define RN_POINTS_ART_SLOTS      3   /* pt_cat_34_28 / ic_coin_36_36 / sc_*_42_42 */

typedef struct {
    /* K4G-L1：存**文件名副本**而非指针——场景标名每次由调用方局部缓冲拼出
     * （<symbol>.bin），存指针会悬垂且同名误判。空串 = 槽未占用。 */
    char name[32];
    lv_image_dsc_t dsc;
    uint32_t *px;
    int st; /* 0=未试 1=可用 2=不可用 */
} rn_points_art_cache_t;

static rn_points_art_cache_t s_rn_points_art[RN_POINTS_ART_SLOTS];

/* K4G-L1 守卫（编译期）：槽名字段必须容得下最长场景文件名
 * （"sc_choco_42_42.bin" = 18 字符 + NUL）。缓冲一旦被收窄，此处编译失败，
 * 而不是运行时把文件名静默截断成不存在的资产（= 场景标又变成不画）。 */
static_assert(sizeof(s_rn_points_art[0].name) >= 24,
              "points art cache name buffer too small for scene asset filenames");

static const lv_image_dsc_t *rn_points_art_dsc(int slot_index, const char *asset,
                                               int max_w, int max_h)
{
    kidpal_art_img_t img;
    rn_points_art_cache_t *slot;

    if (slot_index < 0 || slot_index >= RN_POINTS_ART_SLOTS || asset == NULL) {
        return NULL;
    }
    slot = &s_rn_points_art[slot_index];
    if (slot->name[0] != '\0' && strcmp(slot->name, asset) == 0) {
        if (slot->st == 2) {
            return NULL; /* 同名的不可用资产：记住失败，不再逐帧重试 */
        }
        if (slot->st == 1) {
            return &slot->dsc;
        }
    }
    if (slot->name[0] != '\0') {
        /* 槽内是旧资产（场景标换目标）：释放旧像素缓冲，同一槽按新名重载。 */
        if (slot->px != NULL) {
            lv_free(slot->px);
        }
        slot->px = NULL;
        slot->name[0] = '\0';
        slot->st = 0;
    }
    if (kidpal_art_load(asset, &img) != 0 || img.w <= 0 || img.h <= 0 ||
        img.w > max_w || img.h > max_h) {
        snprintf(slot->name, sizeof(slot->name), "%s", asset); /* 记住失败的资产名：不逐帧重试，也不逐出他槽 */
        slot->st = 2;
        return NULL;
    }
    slot->px = (uint32_t *)lv_malloc((size_t)img.w * img.h * sizeof(uint32_t));
    if (slot->px == NULL) {
        snprintf(slot->name, sizeof(slot->name), "%s", asset);
        slot->st = 2;
        return NULL;
    }
    for (int y = 0; y < img.h; y++) {
        for (int x = 0; x < img.w; x++) {
            int p = kidpal_art_pixel(&img, x, y);
            slot->px[y * img.w + x] = (p == 1) ? 0xFF000000u : 0x00000000u;
        }
    }
    slot->dsc = lv_image_dsc_t();
    slot->dsc.header.cf     = LV_COLOR_FORMAT_ARGB8888;
    slot->dsc.header.w      = img.w;
    slot->dsc.header.h      = img.h;
    slot->dsc.header.stride = (uint32_t)img.w * 4u;
    slot->dsc.data          = (const uint8_t *)slot->px;
    slot->dsc.data_size     = (uint32_t)(img.w * img.h * 4);
    snprintf(slot->name, sizeof(slot->name), "%s", asset);
    slot->st = 1;
    return &slot->dsc;
}

/* 资产落屏：返回 1 = 已画；0 = 资产不可用（调用方画兜底形态）。 */
static int rn_points_art_image(lv_obj_t *scr, int slot_index, const char *asset,
                               int max_w, int max_h, int x, int y)
{
    const lv_image_dsc_t *d = rn_points_art_dsc(slot_index, asset, max_w, max_h);
    lv_obj_t *im;

    if (d == NULL) {
        return 0;
    }
    im = lv_image_create(scr);
    if (im == NULL) {
        return 0;
    }
    lv_image_set_src(im, d);
    lv_obj_set_pos(im, x, y);
    return 1;
}

/* 分值 -> 线上横坐标（钳位 [X0, X1]；target <= 0 防御按 1 计——不除零）。 */
static int rn_points_line_x(int value, int target)
{
    int x;

    if (target <= 0) {
        target = 1;
    }
    x = RNP_LINE_X0 + (RNP_LINE_X1 - RNP_LINE_X0) * value / target;
    if (x < RNP_LINE_X0) {
        x = RNP_LINE_X0;
    }
    if (x > RNP_LINE_X1) {
        x = RNP_LINE_X1;
    }
    return x;
}

/* 进度线（V9 points-home*.png）：基线全程虚线、已走过部分实线、小伴标记在余额位。
 * 负余额已在决策表里钳位成 pos=0/solid_len=0（全线虚线、小伴在起点）——本函数
 * 只按数据画，不再二次判断（两处判断 = 两套真相源）。 */
static void rn_render_points_progress_line(lv_obj_t *scr,
                                           const kidpal_points_progress_t *pg,
                                           const char *target_asset)
{
    char label[16];
    int i;
    int solid_x;
    int comrade_x;
    int t;

    /* 虚线基线（10px 步进的小段）。 */
    for (i = RNP_LINE_X0; i < RNP_LINE_X1; i += 10) {
        rn_points_box(scr, i, RNP_LINE_Y - 1, 5, 3, RNP_INK);
    }
    /* 实线：已走过部分（含超目标越场景标延伸——x 已按同一比例钳位到 X1）。 */
    solid_x = rn_points_line_x(pg->solid_len, pg->target_points);
    if (solid_x > RNP_LINE_X0) {
        rn_points_box(scr, RNP_LINE_X0, RNP_LINE_Y - 2, solid_x - RNP_LINE_X0, 5,
                      RNP_INK);
    }
    /* 刻度点 + 分值标签（起点 / 1/4 / 1/2 / 3/4 / 场景标）。 */
    for (i = 0; i <= 4; i++) {
        int x = RNP_LINE_X0 + (RNP_LINE_X1 - RNP_LINE_X0) * i / 4;

        rn_points_box(scr, x - 4, RNP_LINE_Y - 4, 9, 9, RNP_INK);
        if (i == 0) {
            rn_points_label(scr, "起点", x - 14, RNP_LINE_Y + 12, RNP_INK,
                            &font_puhui_14_1);
        } else {
            t = pg->target_points * i / 4;
            snprintf(label, sizeof(label), "%d", t);
            rn_points_label(scr, label, x - 12, RNP_LINE_Y + 12, RNP_INK,
                            &font_puhui_14_1);
        }
    }
    /* 目标位场景标（planner ruling 9 / V9）：sc-* 42×42 悬于基线上方、
     * 底边距基线 12px 不压线，与 pt-cat 同一悬空纪律；整体略大于猫咪 34×28 属设计
     * 预期——「场景标要像目标不像小图标」。K4：场景标随激活目标变化——逻辑标识
     * （sc-choco/sc-movie/sc-wheel/sc-gift/sc-flag）经 kidpal_points_scene_asset 单源
     * 映射为 assets.h 符号名（= <name>.bin 根名，契约不带后缀）；kidpal_art_load 按
     * 文件名读 spiffs，故此处补 `.bin` 后缀后再进槽（K4G-L1：漏后缀 = 场景标全不画）。
     * 未知标识不画（绝不猜图）。资产不可用回落 = 不画（不画方块）。 */
    {
        const char *scene_bin = kidpal_points_scene_asset(target_asset);

        if (scene_bin != NULL) {
            char scene_file[32];
            int tgt_x = RNP_LINE_X1;            /* 目标分值位 = 刻度末端 */
            int wheel_x = tgt_x - 21;           /* 42 宽中心对目标刻度 */
            int wheel_y = RNP_LINE_Y - 42 - 12; /* 底边 = 基线 - 12px（V9 统一悬空口径） */

            snprintf(scene_file, sizeof(scene_file), "%s.bin", scene_bin);
            (void)rn_points_art_image(scr, RN_POINTS_ART_SLOT_SCENE, scene_file, 42, 42,
                                      wheel_x, wheel_y);
        }
    }
    /* 小伴位置标（V9：pt-cat 34×28 悬于横线上方、底边距基线 12px 不压线；负余额
     * 钳位在起点——数字本身仍由余额文本如实承载）。资产不可用回落 16×16 实心方块。 */
    comrade_x = rn_points_line_x(pg->pos, pg->target_points);
    {
        int cat_x = comrade_x - 17;       /* 34 宽中心对余额位 */
        int cat_y = RNP_LINE_Y - 28 - 12; /* 底边 = 基线 - 12px（V9 统一悬空口径） */

        if (cat_x < 0) {
            cat_x = 0;
        }
        if (rn_points_art_image(scr, RN_POINTS_ART_SLOT_CAT, "pt_cat_34_28.bin",
                                34, 28, cat_x, cat_y) == 0) {
            rn_points_box(scr, comrade_x - 8, RNP_LINE_Y - 30, 16, 16, RNP_INK);
        }
    }
}

/* 分值逐字（正数带 +，负数如实带 -；V9 points-home-negative.png「-40」）。 */
static void rn_points_format_delta(int delta, char *out, size_t cap)
{
    if (delta > 0) {
        snprintf(out, cap, "+%d", delta);
    } else {
        snprintf(out, cap, "%d", delta);
    }
}

/* occurred_at（ISO8601 "YYYY-MM-DDThh:mm:ss+08:00"）-> V9 日期列：
 * 与设备本地 RTC 同月日 -> 「今天」；否则「M月D日」。畸形/缺省 -> 空串（不伪造日期）。 */
static void rn_points_format_date(const char *at, char *out, size_t cap)
{
    int mm;
    int dd;

    if (out == NULL || cap == 0) {
        return;
    }
    out[0] = '\0';
    if (at == NULL || strlen(at) < 10 ||
        at[5] < '0' || at[5] > '9' || at[6] < '0' || at[6] > '9' ||
        at[8] < '0' || at[8] > '9' || at[9] < '0' || at[9] > '9') {
        return;
    }
    mm = (at[5] - '0') * 10 + (at[6] - '0');
    dd = (at[8] - '0') * 10 + (at[9] - '0');
    if (s_rtc_month > 0 && s_rtc_mday > 0 && mm == s_rtc_month && dd == s_rtc_mday) {
        snprintf(out, cap, "今天");
    } else {
        snprintf(out, cap, "%d月%d日", mm, dd);
    }
}

/* 一条分录行（最近三条 / 详细账本行 / 删除确认框共用列位）：delta | before→after |
 * 事由 | 日期（列位由调用方按版面给定）。事由列定宽 + LV_LABEL_LONG_DOT——长原因
 * 截尾，绝不撑破版面（K2D-E4）。 */
static void rn_render_points_entry_row(lv_obj_t *scr, const kidpal_points_entry_t *e,
                                       int y, int with_balances, int reason_x,
                                       int reason_w, int date_x, int date_w)
{
    char buf[64];
    char date[24];

    rn_points_format_delta(e->delta, buf, sizeof(buf));
    rn_points_label(scr, buf, 12, y, RNP_INK, kidpal_render_node_body_font());
    if (with_balances) {
        snprintf(buf, sizeof(buf), "%d→%d", e->balance_before, e->balance_after);
        rn_points_label(scr, buf, 70, y, RNP_INK, &font_puhui_14_1);
    }
    {
        lv_obj_t *reason = rn_points_label(scr, e->reason, reason_x, y, RNP_INK,
                                           kidpal_render_node_body_font());
        lv_obj_set_width(reason, reason_w);
        /* K2 真机轮 F7：定高只容一行——LV_LABEL_LONG_DOT 仅在对象高度小于文本
         * 自然高时才触发截尾；高度留 LV_SIZE_CONTENT 会折行第二行溢出邻行/框。 */
        lv_obj_set_height(reason,
                          lv_font_get_line_height(kidpal_render_node_body_font()));
        lv_label_set_long_mode(reason, LV_LABEL_LONG_DOT);
    }
    rn_points_format_date(e->occurred_at, date, sizeof(date));
    {
        lv_obj_t *d = rn_points_label(scr, date, date_x, y, RNP_INK, &font_puhui_14_1);
        lv_obj_set_width(d, date_w);
        lv_obj_set_style_text_align(d, LV_TEXT_ALIGN_RIGHT, 0);
    }
}

/* 删除确认框内的单条分录（V9 points-delete-confirm.png：列位整体右移进圆角框内，与
 * 列表行不同起点）。 */
static void rn_render_points_delete_entry(const kidpal_points_entry_t *e)
{
    char buf[64];
    char date[24];

    rn_points_format_delta(e->delta, buf, sizeof(buf));
    rn_points_label(s_kidpal_scr, buf, 70, 134, RNP_INK, kidpal_render_node_body_font());
    snprintf(buf, sizeof(buf), "%d→%d", e->balance_before, e->balance_after);
    rn_points_label(s_kidpal_scr, buf, 130, 134, RNP_INK, &font_puhui_14_1);
    {
        lv_obj_t *reason = rn_points_label(s_kidpal_scr, e->reason, 190, 134, RNP_INK,
                                           kidpal_render_node_body_font());
        lv_obj_set_width(reason, 72); /* 190..262（日期列起点），比原 64 略宽减截断 */
        /* K2 真机轮 F7：删除确认圆角框内长原因定高一行截尾，绝不折行第二行溢出
         * 框体（V9 points-delete-confirm.png 单行分录版式）。 */
        lv_obj_set_height(reason,
                          lv_font_get_line_height(kidpal_render_node_body_font()));
        lv_label_set_long_mode(reason, LV_LABEL_LONG_DOT);
    }
    rn_points_format_date(e->occurred_at, date, sizeof(date));
    {
        lv_obj_t *d = rn_points_label(s_kidpal_scr, date, 262, 134, RNP_INK,
                                      &font_puhui_14_1);
        lv_obj_set_width(d, 76);
        lv_obj_set_style_text_align(d, LV_TEXT_ALIGN_RIGHT, 0);
    }
}

/* 积分主页正文（数据已送达；NODATA 由调用点画 K0a 占位，不进本函数）。四态差异：
 * 空账显 0 与目标 + 空账话术（进度线照画）；正常/超目标显数字 + 进度线 + 最近三条；
 * 负余额数字如实负值、进度线钳位。页面结构单页不变（D-02）。 */
static void rn_render_points_home_body_locked(void)
{
    kidpal_points_progress_t pg;
    int state = kidpal_points_home_state(&s_points_home);
    char buf[64];
    int i;

    /* 金币图标 + 余额数字（V9 points-home.png 逐项对表：金币紧贴数字左侧、数字右对齐
     * 贴画布右缘 388、金币与数字同排视觉居中；F5 版币居中偏右 x=214 已退役）。数字如实
     * （负值不藏数）；CLIP = 超大分值只截尾不换行、不撑破顶栏。资产不可用回落裸 ★ 占位。 */
    {
        const int num_x = 244;     /* 数字标签左界（右缘 388 之内的 CLIP 区） */
        const int num_right = 388; /* 画布右缘口径（400 宽 - 12 边距；F5 前同值） */
        const int coin_gap = 6;    /* V9 金币↔数字间距 */
        const int coin_w = 36;     /* ic_coin_36_36 资产宽（36×36 方形） */
        const lv_font_t *num_font = kidpal_render_node_title_font();
        int num_w;
        int coin_x;
        int coin_y;

        snprintf(buf, sizeof(buf), "%d", s_points_home.balance);
        {
            lv_obj_t *bal = rn_points_label(s_kidpal_scr, buf, num_x, 8, RNP_INK,
                                            num_font);

            lv_obj_set_width(bal, num_right - num_x);
            lv_obj_set_style_text_align(bal, LV_TEXT_ALIGN_RIGHT, 0);
            lv_label_set_long_mode(bal, LV_LABEL_LONG_CLIP);
        }
        /* 金币 x = 数字左缘 - 币宽 - 6。数字宽按同字体实测（数值位数/负号随余额变，
         * 估宽会在 3 位以上错位）。金币 y 与数字行视觉居中。 */
        num_w = (int)lv_txt_get_width(buf, (uint32_t)strlen(buf), num_font, 0);
        if (num_w > num_right - num_x) {
            num_w = num_right - num_x; /* CLIP 生效时贴可见数字左缘，不外推到标签框外 */
        }
        coin_x = num_right - num_w - coin_gap - coin_w;
        if (coin_x < 0) {
            coin_x = 0;
        }
        coin_y = 8 + (lv_font_get_line_height(num_font) - coin_w) / 2;
        if (coin_y < 0) {
            coin_y = 0;
        }
        if (rn_points_art_image(s_kidpal_scr, RN_POINTS_ART_SLOT_COIN,
                                "ic_coin_36_36.bin", 36, 36,
                                coin_x, coin_y) == 0) {
            rn_points_label(s_kidpal_scr, "★", coin_x, coin_y, RNP_INK,
                            num_font);
        }
    }
    /* 目标名（场景标位说明；超长截尾防撑破版面）。 */
    {
        lv_obj_t *tgt = rn_points_label(s_kidpal_scr, s_points_home.target_name, 30, 36,
                                        RNP_INK, kidpal_render_node_body_font());
        lv_obj_set_width(tgt, 220);
        lv_label_set_long_mode(tgt, LV_LABEL_LONG_DOT);
    }

    if (kidpal_points_progress(&s_points_home, &pg) == 0) {
        rn_render_points_progress_line(s_kidpal_scr, &pg, s_points_home.target_asset);
    }

    rn_points_label(s_kidpal_scr, "最近得到", 12, 138, RNP_INK,
                    kidpal_render_node_body_font());
    /* planner ruling 11 / F4：原「详细 · 双击看」胶囊（右 248..392、y132..160）删除——
     * 提示已压入左键帽「详·双击」（keybar.c 页槽表 P3 行）。胶囊只是标题行右侧的叠加
     * 元素、不占垂直块位，故删除后：
     *  - 「最近得到」保持左对齐（12,138），标题行右侧留白（row header 右空白，常态，
     *    无需收拢）；
     *  - recent 三行 y=166/192/218 与底部语音提示 y=246 均不下移（与 V9 points-home
     *    余下版面逐行对表，胶囊删除不产生垂直空洞）。 */

    if (state == KPTS_HOME_STATE_EMPTY) {
        lv_obj_t *hint = rn_points_label(s_kidpal_scr, "账本空空的，攒下第一笔积分吧",
                                         40, 192, RNP_INK,
                                         kidpal_render_node_body_font());
        lv_obj_set_width(hint, 320);
        lv_obj_set_style_text_align(hint, LV_TEXT_ALIGN_CENTER, 0);
        return;
    }
    for (i = 0; i < s_points_home.recent_count && i < KPTS_RECENT_MAX; i++) {
        /* K2 真机轮 F3：最近三条补 before→after（列位同详细账本页：delta 12 /
         * before→after 70 / 事由 150 / 日期 300）。 */
        rn_render_points_entry_row(s_kidpal_scr, &s_points_home.recent[i],
                                   166 + i * 26, 1, 150, 136, 300, 88);
    }
    /* 语音记账提示（V9 points-home*.png 底栏上方提示条；空账帧由上面的空账话术承载,
     * 不重复出提示）。 */
    if (s_points_home.recent_count > 0) {
        lv_obj_t *tip = rn_points_label(s_kidpal_scr, "说「加5分」就能记账", 100, 246,
                                        RNP_INK, &font_puhui_14_1);
        lv_obj_set_width(tip, 200);
        lv_obj_set_style_text_align(tip, LV_TEXT_ALIGN_CENTER, 0);
    }
}

/* 详细账本页（V9 points-ledger.png）：单行分录（delta / before→after / 事由 / 日期）+
 * 光标描边框 + 页脚「第 x 屏 · 共 y 屏」（分母 = page.total 全量换算，不以已缓存条数
 * 顶替——全量可及，S-F6.2 AC1）。一屏行数 = 单帧容量 KPTS_LEDGER_ITEMS_MAX = 6。 */
static void rn_render_points_ledger_locked(void)
{
    int i;
    int rows = KPTS_LEDGER_ITEMS_MAX;
    int screens;
    int cur_screen;
    char footer[64];

    if (s_points_ledger_received == 0) {
        lv_obj_t *hint = rn_points_label(s_kidpal_scr, KP_POINTS_NODATA_HINT, 40, 150,
                                         RNP_INK, kidpal_render_node_body_font());
        lv_obj_set_width(hint, 320);
        lv_obj_set_style_text_align(hint, LV_TEXT_ALIGN_CENTER, 0);
        return;
    }
    if (s_points_ledger.item_count <= 0) {
        lv_obj_t *hint = rn_points_label(s_kidpal_scr, "账本空空的，攒下第一笔积分吧",
                                         40, 150, RNP_INK,
                                         kidpal_render_node_body_font());
        lv_obj_set_width(hint, 320);
        lv_obj_set_style_text_align(hint, LV_TEXT_ALIGN_CENTER, 0);
        return;
    }

    for (i = 0; i < s_points_ledger.item_count && i < rows; i++) {
        int y = 12 + i * 34;

        if (i == s_points_cursor) {
            rn_points_outline(s_kidpal_scr, 6, y - 6, 388, 32);
        }
        rn_render_points_entry_row(s_kidpal_scr, &s_points_ledger.items[i], y, 1, 150,
                                   136, 300, 88);
    }

    /* 页脚：screen 数来自 page_total 全量换算；当前屏 = 已缓存页序号（新页到达时
     * cached_count 已含本页）。 */
    screens = kidpal_points_ledger_screen_total(&s_points_state, rows);
    if (screens < 0) {
        screens = 0;
    }
    cur_screen = (s_points_state.cached_count + rows - 1) / rows;
    if (cur_screen < 1) {
        cur_screen = 1;
    }
    if (screens < cur_screen) {
        screens = cur_screen;
    }
    snprintf(footer, sizeof(footer), "第 %d 屏 · 共 %d 屏", cur_screen, screens);
    {
        lv_obj_t *f = rn_points_label(s_kidpal_scr, footer, 120, 246, RNP_INK,
                                      &font_puhui_14_1);
        lv_obj_set_width(f, 160);
        lv_obj_set_style_text_align(f, LV_TEXT_ALIGN_CENTER, 0);
    }
}

/* 删除确认整屏覆盖（V9 points-delete-confirm.png）：标题 + 待删分录取自当前光标行
 * （只读缓存，绝不本地删）；页点消失由 keybar 消费 KPTS_SUBSTATE_DELETE_CONFIRM
 * （dots_overlay）承载。 */
static void rn_render_points_delete_locked(void)
{
    int idx = s_points_cursor;

    if (idx < 0 || idx >= s_points_ledger.item_count) {
        idx = 0;
    }
    rn_points_box(s_kidpal_scr, 0, 0, 400, 300, 0xFFFFFFu);
    {
        lv_obj_t *t = rn_points_label(s_kidpal_scr, "删掉这条?", 120, 70, RNP_INK,
                                      kidpal_render_node_title_font());
        lv_obj_set_width(t, 160);
        lv_obj_set_style_text_align(t, LV_TEXT_ALIGN_CENTER, 0);
    }
    if (s_points_ledger.item_count > 0 && idx < KPTS_LEDGER_ITEMS_MAX) {
        rn_points_outline(s_kidpal_scr, 56, 120, 288, 48);
        rn_render_points_delete_entry(&s_points_ledger.items[idx]);
    }
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
    /* 261022-diag-log-content [DLG-C4]④（plan 改动点 5）：页跳转**一行语义记录**——
     * from→to + 触发源（key/timer/net），回答「设备哪一刻在哪一页、从哪来、被什么
     * 触发」。既有 PAGE_RENDER in/out 两行（K3R D2 真机诊断，下方原样保留）记的是
     * 渲染入出口快照，语义面不同，不互相替代。from 取落笔前的基层页（s_local_page 在
     * 本函数 switch 内才更新）；触发源由调用方经 rn_note_page_switch_trigger 标注，
     * 此处读一次即复位默认（键路径）——防陈旧值串到下一次跳转。 */
    int trigger = s_page_switch_trigger;

    s_page_switch_trigger = RN_PAGE_TRIGGER_KEY;
    ESP_LOGI(RN_TAG, "PAGE_SWITCH from=%s to=%s trigger=%s local_from=%d local_to=%d",
             kidpal_log_page_name(static_cast<int>(s_local_page)),
             kidpal_log_page_name(static_cast<int>(page)),
             kidpal_log_trigger_name(trigger),
             static_cast<int>(s_local_page), static_cast<int>(page));

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
        case KP_PAGE_POINTS_HOME: {
            /* P3 星星积分本正式页（TaskPackage 261104-k2-points-device-ui 轮 2）：四态
             * 主页 + 进度线 + 最近三条 / 详细账本子态 / 删除确认覆盖层（按 points_page.h
             * 的 KPTS_SUBSTATE_* 分派）。未达态沿用 K0a 占位口径（话术单源 =
             * KP_POINTS_NODATA_HINT，本处零话术字面重抄；不显数字、不画进度线）。
             * 数据面判定全住 display/points_page.c，本 case 只做画法分派、零重算。 */
            int pts_sub;

            lv_screen_load(s_kidpal_scr);
            pts_sub = kidpal_points_ui_substate();
            if (pts_sub == KPTS_SUBSTATE_DELETE_CONFIRM) {
                rn_render_points_delete_locked();
            } else if (pts_sub == KPTS_SUBSTATE_LEDGER) {
                /* 详细账本帧无页名标题（V9 points-ledger.png：列表自顶排版）。 */
                rn_render_points_ledger_locked();
            } else {
                lv_obj_t *pts_title = lv_label_create(s_kidpal_scr);
                lv_label_set_text(pts_title, "星星积分本");
                lv_obj_set_style_text_font(pts_title,
                                           kidpal_render_node_title_font(), 0);
                lv_obj_set_style_text_color(pts_title, lv_color_black(), 0);
                lv_obj_set_pos(pts_title, 12, 6);
                if (s_points_home_received == 0) {
                    lv_obj_t *pts_hint = lv_label_create(s_kidpal_scr);
                    lv_label_set_text(pts_hint, KP_POINTS_NODATA_HINT);
                    lv_obj_set_style_text_font(pts_hint,
                                               kidpal_render_node_body_font(), 0);
                    lv_obj_set_style_text_color(pts_hint, lv_color_black(), 0);
                    lv_obj_set_style_text_align(pts_hint, LV_TEXT_ALIGN_CENTER, 0);
                    lv_obj_set_width(pts_hint, 380);
                    lv_label_set_long_mode(pts_hint, LV_LABEL_LONG_WRAP);
                    lv_obj_center(pts_hint);
                } else {
                    rn_render_points_home_body_locked();
                }
            }
            s_local_page = KP_PAGE_POINTS_HOME;
            /* 诊断行不带数值格式化——日志面与渲染面同一页身份口径（K6 结构化日志的
             * correlationId 归 tick/事件面，本行只标页身份）。 */
            ESP_LOGI(RN_TAG, "render page POINTS_HOME (P3) points page");
            break;
        }
        case KP_PAGE_STORY: {
            lv_screen_load(s_kidpal_scr);
            if (s_story_list.count <= 0) {
                /* 空态两态分叉（seam [SL3]; requirements v1.63 S-F2.1 AC5/AC6）——判据
                 * = 「有无列表数据」（s_story_data_received）而非「断网与否」:
                 *   AC5 无内容空态（数据已送达、确无篇目）= ×0 诚实计数（计数画法照常、
                 *       不隐瞒, 页身份保持）+ AC5 话术（story_page.h 单源宏）;
                 *   AC6 数据未达空态（列表缓存从未装载 = 重启后恢复推送未到/失败）=
                 *       不显计数 + AC6 话术（story_page.h 单源新宏）。
                 * 话术单源 = story_page.h 双 macro（notes 边角③ 空态渲染入口两处同源
                 * 纪律不破——本处与 story_page.c 消费同一单源, 不得双写文案）。 */
                lv_obj_t *lbl = lv_label_create(s_kidpal_scr);
                if (s_story_data_received) {
                    lv_label_set_text(lbl, KSTORY_EMPTY_HINT);
                    /* AC5 ×0 诚实计数（数据已送达、确无篇目——显 ×0 不隐瞒）。 */
                    lv_obj_t *star = lv_label_create(s_kidpal_scr);
                    lv_label_set_text(star, "★");
                    lv_obj_set_pos(star, 316, 8);
                    /* 颜色统一到 KSY_COLOR_INK 单源（seam [TB3]; 与 story_page.c 计数
                     * 画点同源——「话术单源 = story_page.h」纪律同款, 偏离侧旧纯黑调用
                     * 退役; 本板 1bpp 阈值下与纯黑同渲, 零视觉差）。 */
                    lv_obj_set_style_text_color(star, lv_color_hex(KSY_COLOR_INK), 0);
                    lv_obj_t *cnt = lv_label_create(s_kidpal_scr);
                    lv_label_set_text_fmt(cnt, "×%d", kidpal_story_counter(&s_story_list));
                    lv_obj_set_pos(cnt, 344, 8);
                    lv_obj_set_style_text_color(cnt, lv_color_hex(KSY_COLOR_INK), 0);
                } else {
                    /* AC6 数据未达空态（列表缓存从未装载——不显计数, 篇数未知不显 ×0
                     * 误导）+ AC6 话术（story_page.h 单源新宏）。 */
                    lv_label_set_text(lbl, KSTORY_EMPTY_HINT_UNREACHED);
                }
                lv_obj_set_style_text_color(lbl, lv_color_black(), 0);
                lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
                lv_obj_set_width(lbl, 380);
                lv_label_set_long_mode(lbl, LV_LABEL_LONG_WRAP);
                lv_obj_center(lbl);
            } else {
                kidpal_story_page_render(&s_story_view, &s_story_list, &s_story_text);
            }
            s_local_page = KP_PAGE_STORY;
            ESP_LOGI(RN_TAG, "render page STORY (P4) count=%d", s_story_list.count);
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
        rn_note_page_switch_trigger(RN_PAGE_TRIGGER_KEY);
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

    rn_note_page_switch_trigger(RN_PAGE_TRIGGER_KEY); /* 键路由翻页 = 按键触发 */
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
    if (v != NULL) {
        memcpy(&s_story_view, v, sizeof(s_story_view));
        /* RSL2-A1 [I1]（261028-p3-record-session-mirror-desync-fix, M1）: 镜像恒等
         * 权威视图页态——镜像写入边与真实状态边同源（g_story_view.page 每次变更终经
         * cache_story 落节点缓存, 镜像随缓存装载单点派生, 不再独立漂移）。既有显式桥
         * kidpal_render_node_story_session_page() 与 mcp_server.cc 两处投递保留为幂等
         * 冗余（冻结 seam [R5] 文字不动）。 */
        s_story_session_page = v->page;
    }
    if (l != NULL) {
        memcpy(&s_story_list, l, sizeof(s_story_list));
        /* seam [SL3]: 列表数据装载 = 送达标志置位（空态两态分叉判据载体——
         * 已送达（含空篇目）-> AC5; 从未装载 -> AC6）。 */
        s_story_data_received = 1;
    }
    if (t != NULL) { memcpy(&s_story_text, t, sizeof(s_story_text)); }
}

/* -------------------------------------------------------------------------
 * P3 星星积分本渲染缓存 + 重渲缝（TaskPackage 261104-k2-points-device-ui 轮 2）
 *
 * 投递方 = mcp_server.cc 的 points_home / points_ledger 消费路由（解析 + 决策表裁决
 * 之后）；本区只持有 RAM 副本与「数据已送达」标志，零持久化、零重算。
 * ---------------------------------------------------------------------- */
extern "C" void kidpal_render_node_cache_points_home(const kidpal_points_home_t *frame)
{
    if (frame == NULL) {
        return;
    }
    memcpy(&s_points_home, frame, sizeof(s_points_home));
    s_points_home_received = 1; /* 未达 -> 已送达（四态判定的 RAM 初态分叉点） */
}

extern "C" void kidpal_render_node_cache_points_ledger(
    const kidpal_points_ledger_t *frame, const kidpal_points_ledger_state_t *state,
    int cursor)
{
    if (frame != NULL) {
        memcpy(&s_points_ledger, frame, sizeof(s_points_ledger));
        s_points_ledger_received = 1;
    }
    if (state != NULL) {
        memcpy(&s_points_state, state, sizeof(s_points_state));
    }
    s_points_cursor = (cursor < 0) ? 0 : cursor;
}

int kidpal_render_node_points_on_screen(void)
{
    return (kidpal_layer_top(&s_layer_stack) == KIDPAL_LAYER_LOCAL_PAGE &&
            s_local_page == KP_PAGE_POINTS_HOME) ? 1 : 0;
}

int kidpal_render_node_points_substate(void)
{
    /* 覆盖层前台（VOICE/服务端页/提醒/查字/删除）时积分子态不可见——键帽归各覆盖层
     * 自己的槽表（返回 -1 = 消费方回落既有路径，绝不画错键帽）。 */
    if (kidpal_layer_top(&s_layer_stack) != KIDPAL_LAYER_LOCAL_PAGE) {
        return -1;
    }
    if (s_local_page != KP_PAGE_POINTS_HOME) {
        return -1;
    }
    return kidpal_points_ui_substate();
}

void kidpal_render_node_refresh_points(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        lvgl_port_unlock();
        return;
    }
    /* 只重渲基层积分页（本地导航 / 在屏 restore 原地刷新）；当前页非积分页 -> no-op
     * ——restore 帧绝不抢前台（D-07）。页内子态由 points_page.h 单源状态机承载。 */
    if (s_local_page == KP_PAGE_POINTS_HOME) {
        rn_note_page_switch_trigger(RN_PAGE_TRIGGER_NET); /* 帧到达 / 页内导航重渲 */
        rn_render_kidpal_page_locked(KP_PAGE_POINTS_HOME);
    }
    lvgl_port_unlock();
}

void kidpal_render_node_show_points_home(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        lvgl_port_unlock();
        return;
    }
    /* 主动接管前台（D-07 例外：仅设备主动语音记账成功 / 显式打开）：清 transient 覆盖层
     * 回基层、落 LOCAL_PAGE 并渲积分主页。restore 帧绝不走本缝（另走 refresh_points）。 */
    (void)kidpal_layer_pop_transients(&s_layer_stack);
    kidpal_layer_push(&s_layer_stack, KIDPAL_LAYER_LOCAL_PAGE);
    rn_note_page_switch_trigger(RN_PAGE_TRIGGER_NET);
    rn_render_kidpal_page_locked(KP_PAGE_POINTS_HOME);
    rn_voice_chrome_front_locked();

    lvgl_port_unlock();
    ESP_LOGI(RN_TAG, "show_points_home: LOCAL_PAGE front (layer top=%d)",
             kidpal_layer_top(&s_layer_stack));
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
    rn_note_page_switch_trigger(RN_PAGE_TRIGGER_NET); /* 云端推送 / restore 路径 */
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
    /* TECH-261028 阅读态归属的「断链环」补口（planner 2026-11-05 裁断, 裁定 B）。
     *
     * 缺陷环：阅读正文画在云端渲染夹钳 SERVER_PAGE(4) 上（mcp_server VRT_ROUTE_STORY_
     * TEXT 收口只 begin_page/end_page、不调 story_local_front, 阅读期前台停 SERVER_PAGE
     * ——与 TECH-261028 的 KPLOG「chrome_front layer_top=4」一致）。归属判据
     * kidpal_story_key_owns 的 page_state 入参就是本 getter, 而下方通用规则在该窗恒报
     * -1 —— 故事页新增的「READER + SERVER_PAGE 归故事页」支永远拿不到 READER, KEY 短按
     * 照旧漏到 key_route_layer 通用覆盖层列 -> SESSION_TOGGLE 误开会话。
     *
     * 补口范围（**只对 READER 放开**）：本地基层页仍是故事页、且阅读面确已落屏为
     * 阅读态时, 把 READER 如实报出。LIST/RECORDING/STAMP/DELETE 等仍走下方 -1 ——
     * CG-A2 过渡窗语义（「覆盖层/夹钳前台时缓存页态不可见」）在其余页态上零改动,
     * 「最后已知子态」兜底链照旧生效。副作用面（预期内）：阅读态键帽因此取到
     * kidpal_keybar_story_slots 的 READER 行「长按退出 / 翻屏」（TECH-261028 症状②
     * 的值域半）。
     *
     * K0A-E3 真机缺陷修复（261104-k0a-four-page-skeleton fixer 轮）：云端阅读正文渲染
     * 走 begin_page -> kidpal_story_page_render -> end_page 夹钳，不经 cache_story，
     * s_story_view.page 在阅读驻留期全程停留 LIST——仅按 s_story_view.page 判永远取
     * 不到 READER（真机 keybar substate=-1 last_known=0）。故增信 s_story_view.page
     * **或** story_page.c 的「最近成功落屏页态」getter（后者由阅读渲染写出）；后者
     * 只在真正渲染过阅读面后为 READER，退出阅读回列表渲染即回落 LIST，不引入驻留外
     * 误判。两个来源任一为 READER 即成立。
     *
     * 值域零新增：返回的是既有 KSTORY_PAGE_* 闭集成员, 消费方无新增分支。 */
    if (kidpal_layer_top(&s_layer_stack) == KIDPAL_LAYER_SERVER_PAGE &&
        s_local_page == KP_PAGE_STORY &&
        (s_story_view.page == KSTORY_PAGE_READER ||
         kidpal_story_page_rendered_page() == KSTORY_PAGE_READER)) {
        return KSTORY_PAGE_READER;
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

int kidpal_render_node_story_rec_substate(void)
{
    /* D3 v7 键帽路由（帧 A/帧 B 分叉判据载体）: 缓存视图的引导播报中子态
     * （[G3] rec_substate; cache_story 单点同步, 与 s_story_view.page 同源同纪）。
     * 无锁 32 位整数读, 可跨任务调用（键帽同步在 LVGL 任务, 写方在 mcp 主线程——
     * 与 g_story_* 既有跨任务宽松度一致）。 */
    return s_story_view.rec_substate;
}

/* -------------------------------------------------------------------------
 * D2 真机轮 R1 根修（2026-10-20, 261019-p3-record-dialogue-guide fixer）
 * ---------------------------------------------------------------------- */

void kidpal_render_node_notify_screen_cleaned(void)
{
    /* clean 联动单点: s_kidpal_img 只指 s_kidpal_scr 的子件, 调用方刚 clean 过
     * 活动屏 -> 无条件置 NULL（不做 lv_obj_is_valid 复验——悬垂地址复用时该判定
     * 会假阳性, D4b-3 守卫的已证缺口）。调用方已持 lvgl 锁（渲染路径持锁纪律）。 */
    s_kidpal_img = NULL;
}

void kidpal_render_node_story_page_front(void)
{
    /* 渲染成功出口的页归属真相单点: 基层内容页 = P3。写点恒在渲染路径（调用方已
     * 持 lvgl 锁）; 32 位对齐指针宽赋值, 与 g_story_* 跨任务宽松度同级。 */
    s_local_page = KP_PAGE_STORY;
    /* K0A-E3 真机缺陷修复（261104-k0a-four-page-skeleton fixer 轮）：阅读正文渲染
     * 走云端夹钳 SERVER_PAGE 且驻留期无任何键帽重 sync——进阅读瞬间的键帽 sync 早于
     * 本渲染（begin_page 期 story_sub 仍是 -1，出列表槽「说话·双击看/下移」），此后
     * 88 秒阅读驻留全程无 KEYBAR 重 sync。阅读面落屏后若本帧页态 = READER，按
     * kidpal_render_node_story_substate() 补口（SERVER_PAGE + READER -> READER）补一次
     * 键帽 sync，令键帽链真正消费到 READER（「长按退出 / 翻屏」）。只对 READER 触发，
     * 其余页态（LIST/STAMP/RECORDING/…）零行为改动，CG-A2 过渡窗与既有同步点不变。 */
    if (kidpal_story_page_rendered_page() == KSTORY_PAGE_READER) {
        rn_keybar_sync_locked();
    }
}

int kidpal_render_node_story_page_is_local(void)
{
    return (s_local_page == KP_PAGE_STORY) ? 1 : 0;
}

/* 记录语境会话在世判据（D2 R1 守卫半; 调用方已持锁或处于单线程渲染语境）:
 * 记录语境标志置位 && 权威会话页态 ∈ {RECORDING, ORGANIZING}。消费点 = show_home /
 * voice_begin(had_server) / voice_end else——记录会话覆盖演出期间, 一切首页重建
 * 路径不得把基层翻回 P1（屏上真相 = P3 会话页; 重建即演出冻结 + 悬垂链前提）。 */
static int rn_record_session_live_locked(void)
{
    int page = s_story_session_page;
    return (kidpal_story_session_ctx_active() == 1 &&
            (page == KSTORY_PAGE_RECORDING || page == KSTORY_PAGE_ORGANIZING)) ? 1 : 0;
}

/* -------------------------------------------------------------------------
 * 统一 kidpal_font 句柄暴露（TaskPackage 261028-m6-k1，P0 spike）：只透传统一句柄 seam
 * （kidpal_font.cc），零排版副作用、不改变 home/schedule/story 现有字体选择行为。P1/P2/P3
 * 后续经 kidpal_font_body()/title() 消费切换。恒非 NULL（缺资产回退编译内 14px）。 */
extern "C" const lv_font_t *kidpal_render_node_body_font(void)
{
    return kidpal_font_body();
}

extern "C" const lv_font_t *kidpal_render_node_title_font(void)
{
    return kidpal_font_title();
}
