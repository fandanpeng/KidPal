/*
 * kidpal_render_node.cc — 设备渲染节点（包 9b HWV-A6 收口）
 *
 * 职责：
 *  1. 提供 host_infra_render_p1_frame 强符号覆盖，把 P1 首页 400×300 ARGB 帧
 *     通过 LVGL XRGB8888 图像渲染至 RLCD。
 *  2. 管理一个专属于 KidPal 的 LVGL 屏（s_kidpal_scr），与 xiaozhi chat UI
 *     （SetupUI 屏）共存——「KidPal 是主屏，语音可覆盖」。
 *  3. 阶段 B：BOOT 双按翻页（home→char→schedule→story→home）。
 *  4. 崩溃修复（包 260829-hardware-validation）：服务端推送页面渲染必须经
 *     begin_page()/end_page() 夹钳，只画到 s_kidpal_scr 并持 LVGL 锁、pin 防抢；
 *     禁止直接 lv_screen_active() 渲染——语音轮期间活动屏是聊天屏 s_chat_scr，
 *     clean 它会销毁 xiaozhi 的 status_label_ 等控件，致 SetStatus 悬空崩溃。
 *
 * DRAM 约束：s_p1_fb (480KB) 已通过 CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
 * 置于 PSRAM；本文件不新增任何大静态缓冲。
 *
 * 线程约束：show_home() / cycle_next() 在 main app task（Schedule）或 CustomBoard
 * 构造期调用，持有 lvgl_port_lock。tick() 从 LVGL 定时器（esp_lvgl_port task）
 * 调用，锁已由 esp_lvgl_port 持有。host_infra_render_p1_frame 由
 * kidpal_home_p1_render_frame() 在 show_home()/tick() 的锁上下文中同步调用。
 */
#include "kidpal_render_node.h"

#include "home_p1.h"
#include "schedule_page.h"
#include "story_page.h"
#include "char_page.h"
#include "application.h" /* 回归修复: tick 读 GetDeviceState() 判语音态，交还/切回 SetupUI */

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

/* KidPal 专属 LVGL 屏（persistent，只在 init 时创建一次，之后复用）。 */
static lv_obj_t *s_kidpal_scr = NULL;

/* xiaozhi SetupUI 屏（回归修复 包 9b HWV-A6 fix）。
 * LcdDisplay::SetupUI() 把聊天/表情/状态控件全部建在它运行时的
 * lv_screen_active()（LVGL 默认首屏）上。render_node_init 在建 s_kidpal_scr
 * 之前捕获该屏为 s_chat_scr，之后不再复制其控件，只在语音态把它切回前台。 */
static lv_obj_t *s_chat_scr = NULL;

/* 当前是否处于「SetupUI 覆盖 KidPal」态（true = 聊天屏在前台）。
 * 用于 tick 状态对账的去重，避免每秒重复 lv_screen_load 造成重绘。 */
static bool s_chat_overlay = false;

/* 服务端推送页面是否占据前台（崩溃修复 包 260829-hardware-validation）。
 * begin_page() 置位；tick 检测新一轮语音（Connecting）或 cycle_next 手动翻页时
 * 清除。pin 期间 tick 不把活动屏抢回聊天屏，孩子得以读完推送页面。 */
static bool s_page_pinned = false;

/* 首页图像控件（每次 show_home() 重建，因为切到 schedule/story 时
 * lv_obj_clean(s_kidpal_scr) 会销毁它）。 */
static lv_obj_t *s_kidpal_img = NULL;

/* P1 帧 LVGL 图像描述符（s_p1_dsc.data 由 host_infra_render_p1_frame 强符号
 * 从 argb 参数赋值，即 s_p1_fb 的地址（PSRAM））。lv_image_set_src 存储此指针，
 * 因此 s_p1_dsc 必须在整个屏幕生命周期内保持存活。 */
static lv_image_dsc_t s_p1_dsc;

/* 当前显示页（阶段 B cycle 状态）。 */
static kidpal_page_id_t s_current_page = KP_PAGE_HOME;

/* 注入状态。 */
static int s_rtc_hour     = 0;
static int s_rtc_minute   = 0;
static int s_env_valid    = 0;
static int s_temp_c       = 0;
static int s_humidity_pct = 0;
static int s_battery_pct  = 100; /* 保守初始值（无 ADC 读数时使用） */
static int s_charging     = 0;

/* 动画 LVGL 定时器（在 init 中创建，每 RN_TICK_MS 触发一次）。 */
static lv_timer_t *s_tick_timer = NULL;
static const uint32_t RN_TICK_MS = 1000;

/* P2 课表缓存（mcp_server 收到课表数据后调 cache_schedule 填充；cycle_next 读此）。 */
static char                s_sched_title[64] = "今日课表";
static kidpal_schedule_item_t s_sched_items[16];
static int                 s_sched_n = 0;
static int                 s_sched_is_tomorrow = 0;
static char                s_sched_now[8] = "";

/* P3 故事缓存。 */
static kidpal_story_view_t s_story_view = {};
static kidpal_story_list_t s_story_list = {};
static kidpal_story_text_t s_story_text = {};

/* P4 查生字：有数据则 cycle_next 不跳过。 */
static bool        s_has_char_data = false;
static std::string s_char_json;   /* 动态分配，不占 .bss；mcp_server cache_char_data 填充 */

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
 * 公开 API
 * ---------------------------------------------------------------------- */

void kidpal_render_node_init(void)
{
    if (s_kidpal_scr != NULL) {
        return; /* 幂等 */
    }
    lvgl_port_lock(0);

    /* 回归修复: 在创建/加载 s_kidpal_scr 之前，捕获当前活动屏 = SetupUI 屏
     * （LcdDisplay::SetupUI 已把聊天/表情/状态控件建在此屏上）。此后语音态可
     * lv_screen_load(s_chat_scr) 把 SetupUI 切回前台，让「聆听」表情可见。 */
    s_chat_scr = lv_screen_active();

    /* 创建专属屏（lv_obj_create(NULL) = 新建一屏，不与任何父对象绑定）。 */
    s_kidpal_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(s_kidpal_scr, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(s_kidpal_scr, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(s_kidpal_scr, 0, 0);
    lv_obj_clear_flag(s_kidpal_scr, LV_OBJ_FLAG_SCROLLABLE);

    /* 动画 tick 定时器（在 LVGL 任务上下文触发，锁已持有）。 */
    s_tick_timer = lv_timer_create(
        [](lv_timer_t *t) {
            (void)t;

            auto state = Application::GetInstance().GetDeviceState();

            /* --- 服务端推送页面 pin（崩溃修复 包 260829-hardware-validation）---
             * pin 期间前台是服务端页面（故事/课表），既不能被「voice→聊天屏」抢回，
             * 也不必做首页动画重渲（当前内容非首页，且 s_kidpal_img 可能已被
             * 页面渲染的 lv_obj_clean 销毁）。仅当检测到新一轮语音开始（Connecting）
             * 时清 pin，让本轮后续对账照常（若服务端不推页面则回聊天 UI；若推则
             * begin_page 重新置 pin）。 */
            if (s_page_pinned) {
                if (state == kDeviceStateConnecting) {
                    s_page_pinned = false;
                    ESP_LOGI(RN_TAG, "new voice round -> clear page pin");
                } else {
                    return; /* 服务端页面保持前台，本轮不自动让位 */
                }
            }

            /* --- 状态对账（回归修复 包 9b HWV-A6 fix）---
             * 语音活跃（Connecting/Listening/Speaking）→ 把 SetupUI 切到前台；
             * 语音结束回到 Idle → 把 KidPal 屏交还前台（回首页常显）。
             * 用 s_chat_overlay 去重，只在跃变时 lv_screen_load，避免每秒重绘。 */
            bool voice = false;
            switch (state) {
                case kDeviceStateConnecting:
                case kDeviceStateListening:
                case kDeviceStateSpeaking:
                    voice = true;
                    break;
                default:
                    break;
            }
            if (voice && !s_chat_overlay && s_chat_scr != NULL) {
                lv_screen_load(s_chat_scr);
                s_chat_overlay = true;
                ESP_LOGI(RN_TAG, "voice active -> show xiaozhi SetupUI (yield KidPal)");
            } else if (!voice && s_chat_overlay) {
                /* 会话结束：回首页常显（重建首页帧再切前台）。 */
                s_chat_overlay = false;
                lvgl_port_unlock();
                kidpal_render_node_show_home();
                lvgl_port_lock(0);
                ESP_LOGI(RN_TAG, "voice ended -> restore KidPal home");
            }

            /* --- 首页动画重渲（冒号/电池闪烁）：仅当首页在前台且未被覆盖 --- */
            if (s_current_page != KP_PAGE_HOME || s_kidpal_img == NULL || s_chat_overlay) {
                return;
            }
            hp1_state_t st = {};
            st.hour        = s_rtc_hour;
            st.minute      = s_rtc_minute;
            st.env_valid   = s_env_valid;
            st.temp_c      = s_temp_c;
            st.humidity_pct = s_humidity_pct;
            st.battery_pct = s_battery_pct;
            st.charging    = s_charging;
            st.tick_ms     = static_cast<int>(esp_timer_get_time() / 1000);
            kidpal_home_p1_set_state(&st);
            kidpal_home_p1_render_frame();
        },
        RN_TICK_MS,
        NULL
    );

    lvgl_port_unlock();
    ESP_LOGI(RN_TAG, "KidPal render node initialized (4-page RLCD driver)");
}

void kidpal_render_node_inject_rtc(int hour, int minute)
{
    if (hour   >= 0 && hour   <= 23) { s_rtc_hour   = hour;   }
    if (minute >= 0 && minute <= 59) { s_rtc_minute = minute; }
}

void kidpal_render_node_inject_env(int env_valid, int temp_c, int humidity_pct)
{
    s_env_valid    = (env_valid == 1) ? 1 : 0;
    if (temp_c      >= -40 && temp_c      <= 60)  { s_temp_c       = temp_c;       }
    if (humidity_pct >=  0 && humidity_pct <= 100) { s_humidity_pct = humidity_pct; }
}

void kidpal_render_node_show_home(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        lvgl_port_unlock();
        ESP_LOGW(RN_TAG, "show_home called before init, skipping");
        return;
    }

    /* 清屏，准备首页（若从 schedule/story 切回，图像控件已被 clean 销毁）。 */
    lv_obj_clean(s_kidpal_scr);

    /* 重建全屏图像控件（400×300 XRGB8888，描述符在 host_infra_render_p1_frame 中填充）。 */
    s_kidpal_img = lv_image_create(s_kidpal_scr);
    lv_obj_set_size(s_kidpal_img, 400, 300);
    lv_obj_set_pos(s_kidpal_img, 0, 0);

    /* 注入状态（tick_ms 取当前毫秒以触发冒号初始相位）。 */
    hp1_state_t st = {};
    st.hour         = s_rtc_hour;
    st.minute       = s_rtc_minute;
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
    s_current_page = KP_PAGE_HOME;
    s_chat_overlay = false; /* 首页在前台，非聊天覆盖态 */

    lvgl_port_unlock();

    ESP_LOGI(RN_TAG, "KidPal render node pushing home 400x300 render_rc=%d state_rc=%d tick=%d",
             render_rc, state_rc, st.tick_ms);
}

/* -------------------------------------------------------------------------
 * 回归修复（包 9b HWV-A6 fix）：语音态把 SetupUI 切回前台。
 * ---------------------------------------------------------------------- */
void kidpal_render_node_show_chat(void)
{
    lvgl_port_lock(0);

    if (s_chat_scr == NULL) {
        lvgl_port_unlock();
        ESP_LOGW(RN_TAG, "show_chat: SetupUI screen not captured, skipping");
        return;
    }
    lv_screen_load(s_chat_scr);
    s_chat_overlay = true;

    lvgl_port_unlock();
    ESP_LOGI(RN_TAG, "KidPal render node yielding to xiaozhi SetupUI (voice)");
}

/* -------------------------------------------------------------------------
 * 崩溃修复（包 260829-hardware-validation）：服务端推送页面渲染的持锁夹钳。
 *
 * 服务端推送页面（故事列表/阅读/盖章、课表）渲染函数内部走 lv_screen_active()
 * + lv_obj_clean() 取当前活动屏重建整页。若不预先把活动屏切到 s_kidpal_scr，
 * 语音轮期间活动屏是 xiaozhi 聊天屏 s_chat_scr，clean 它会销毁 status_label_
 * 等控件，LvglDisplay 成员指针失同步 → SetStatus 悬空 → lv_free 垃圾指针崩溃。
 * begin_page 先 load s_kidpal_scr，令后续渲染 clean 的是 KidPal 专属屏（安全，
 * 每次重建）；同时持锁消除与 LVGL 任务的竞态，置 pin 使 tick 不抢回聊天屏。
 * ---------------------------------------------------------------------- */
void kidpal_render_node_begin_page(void)
{
    lvgl_port_lock(0);

    if (s_kidpal_scr == NULL) {
        /* 未 init：保持持锁状态交调用方 end_page 释放，渲染函数自行降级。 */
        return;
    }
    /* 把活动屏切到 KidPal 专属屏，令页面渲染的 lv_screen_active() 取到此屏。 */
    lv_screen_load(s_kidpal_scr);
    s_chat_overlay = false; /* 服务端页面在前台，非聊天覆盖态 */
    s_page_pinned  = true;  /* 防 tick 把页面抢回聊天屏 */
}

void kidpal_render_node_end_page(void)
{
    lvgl_port_unlock();
}

/* -------------------------------------------------------------------------
 * 阶段 B：BOOT 双按翻页
 * ---------------------------------------------------------------------- */

void kidpal_render_node_cycle_next(void)
{
    lvgl_port_lock(0);

    /* 手动翻页 = 用户主动接管导航（崩溃修复 包 260829-hardware-validation）。
     * 解除服务端页面 pin，此后 tick 恢复正常状态对账。 */
    s_page_pinned = false;

    if (s_kidpal_scr == NULL) {
        lvgl_port_unlock();
        ESP_LOGW(RN_TAG, "cycle_next called before init");
        return;
    }

    kidpal_page_id_t next = static_cast<kidpal_page_id_t>(
        (static_cast<int>(s_current_page) + 1) % KP_PAGE_COUNT);

    lv_obj_clean(s_kidpal_scr);
    /* 手动翻页 = 用户主动回到 KidPal 浏览态，清聊天覆盖标志。 */
    s_chat_overlay = false;

    switch (next) {
        case KP_PAGE_HOME: {
            /* show_home() 自己拿锁，这里已在锁内，直接内联渲染逻辑。
             * （s_kidpal_scr 已由上方 lv_obj_clean 清空，不必二次 clean。） */
            s_kidpal_img = lv_image_create(s_kidpal_scr);
            lv_obj_set_size(s_kidpal_img, 400, 300);
            lv_obj_set_pos(s_kidpal_img, 0, 0);
            hp1_state_t st = {};
            st.hour         = s_rtc_hour;
            st.minute       = s_rtc_minute;
            st.env_valid    = s_env_valid;
            st.temp_c       = s_temp_c;
            st.humidity_pct = s_humidity_pct;
            st.battery_pct  = s_battery_pct;
            st.charging     = s_charging;
            st.tick_ms      = static_cast<int>(esp_timer_get_time() / 1000);
            kidpal_home_p1_set_state(&st);
            kidpal_home_p1_render_frame();
            lv_screen_load(s_kidpal_scr);
            s_current_page = KP_PAGE_HOME;
            ESP_LOGI(RN_TAG, "cycle -> HOME (P1)");
            break;
        }
        case KP_PAGE_CHAR: {
            /* 有缓存笔顺数据时 kidpal_char_page_show() 创建独立屏并加载；
             * 无数据时在 s_kidpal_scr 上画"等待数据"占位（cycle_next 仍停靠 CHAR）。 */
            if (s_has_char_data && !s_char_json.empty()) {
                kidpal_char_page_show(s_char_json.c_str());
                /* kidpal_char_page_show 内部调用 lv_screen_load(自己的屏)，
                 * s_kidpal_scr 不再是活动屏。下次 cycle_next 再加载。 */
            } else {
                lv_screen_load(s_kidpal_scr);
                lv_obj_t *lbl = lv_label_create(s_kidpal_scr);
                lv_label_set_text(lbl, "查生字 · 等待数据");
                lv_obj_set_style_text_color(lbl, lv_color_black(), 0);
                lv_obj_center(lbl);
            }
            s_current_page = KP_PAGE_CHAR;
            ESP_LOGI(RN_TAG, "cycle -> CHAR (P4) has_data=%d", s_has_char_data ? 1 : 0);
            break;
        }
        case KP_PAGE_SCHEDULE: {
            /* 把 s_kidpal_scr 设为活动屏，schedule_page_render 在 lv_screen_active()
             * 上清屏并绘制。 */
            lv_screen_load(s_kidpal_scr);
            kidpal_schedule_page_render(
                s_sched_title,
                s_sched_n > 0 ? s_sched_items : NULL,
                s_sched_n,
                s_sched_is_tomorrow,
                s_sched_now,
                -1 /* locate_row: -1 = 不定位，静态展示 */
            );
            s_current_page = KP_PAGE_SCHEDULE;
            ESP_LOGI(RN_TAG, "cycle -> SCHEDULE (P2) n=%d", s_sched_n);
            break;
        }
        case KP_PAGE_STORY: {
            lv_screen_load(s_kidpal_scr);
            kidpal_story_page_render(&s_story_view, &s_story_list, &s_story_text);
            s_current_page = KP_PAGE_STORY;
            ESP_LOGI(RN_TAG, "cycle -> STORY (P3) count=%d", s_story_list.count);
            break;
        }
        default:
            s_current_page = KP_PAGE_HOME;
            break;
    }

    lvgl_port_unlock();
}

int kidpal_render_node_get_page(void)
{
    return static_cast<int>(s_current_page);
}

void kidpal_render_node_tick(void)
{
    /* 仅在 HOME 页且图像控件存活时触发。
     * 由 LVGL 定时器（esp_lvgl_port task）调用，锁已持有。
     * 此处仅重渲 s_p1_fb，不重建控件，不调用 lv_screen_load。 */
    if (s_current_page != KP_PAGE_HOME || s_kidpal_img == NULL) {
        return;
    }
    hp1_state_t st = {};
    st.hour         = s_rtc_hour;
    st.minute       = s_rtc_minute;
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

extern "C" void kidpal_render_node_cache_schedule(const char *title,
                                                   const kidpal_schedule_item_t *items,
                                                   int n,
                                                   int is_tomorrow,
                                                   const char *now_hhmm)
{
    if (title != NULL) {
        strncpy(s_sched_title, title, sizeof(s_sched_title) - 1);
        s_sched_title[sizeof(s_sched_title) - 1] = '\0';
    }
    int copy = (n > 0 && items != NULL) ? (n > 16 ? 16 : n) : 0;
    if (copy > 0) {
        memcpy(s_sched_items, items, static_cast<size_t>(copy) * sizeof(s_sched_items[0]));
    }
    s_sched_n           = copy;
    s_sched_is_tomorrow = is_tomorrow;
    if (now_hhmm != NULL) {
        strncpy(s_sched_now, now_hhmm, sizeof(s_sched_now) - 1);
        s_sched_now[sizeof(s_sched_now) - 1] = '\0';
    }
}

extern "C" void kidpal_render_node_cache_story(const kidpal_story_view_t *v,
                                                const kidpal_story_list_t *l,
                                                const kidpal_story_text_t *t)
{
    if (v != NULL) { memcpy(&s_story_view, v, sizeof(s_story_view)); }
    if (l != NULL) { memcpy(&s_story_list, l, sizeof(s_story_list)); }
    if (t != NULL) { memcpy(&s_story_text, t, sizeof(s_story_text)); }
}

extern "C" void kidpal_render_node_cache_char(int has_data)
{
    s_has_char_data = (has_data != 0);
}

extern "C" void kidpal_render_node_cache_char_data(const char *data_json)
{
    if (data_json != NULL && data_json[0] != '\0') {
        s_char_json = data_json; /* std::string 赋值，动态分配 */
        s_has_char_data = true;
    }
}
