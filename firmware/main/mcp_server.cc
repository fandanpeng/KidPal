/*
 * MCP Server Implementation
 * Reference: https://modelcontextprotocol.io/specification/2024-11-05
 */

#include "mcp_server.h"
#include <esp_log.h>
#include <esp_app_desc.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <ctime>
#include <memory>
#include <string>
#include <vector>
#include <esp_pthread.h>

#include "application.h"
#include "display.h"
#include "oled_display.h"
#include "board.h"
#include "settings.h"
#include "lvgl_theme.h"
#include "lvgl_display.h"
#include "display/view_router.h"
#include "display/schedule_page.h"
#include "display/story_page.h"  /* 故事 P3 (包 260829-story-p3): 列表/阅读/录音/盖章消费面 */
#include "display/story_cache.h" /* P3 正文本地缓存纯逻辑面 (261016-p3-story-body-sd-cache):
                                  * RAM 索引 / 当屏缺失 / 双击三分支 / 预取决策 / id 校验 /
                                  * SD I/O 适配声明 */
#include "display/char_stroke.h"   /* 查生字 P4 (包 260829-char-stroke): 缺字段防御降级门 */
#include "display/char_page.h"     /* 查生字 P4 LVGL 页 (设备侧) */
#include "display/reminder_page.h" /* 提醒弹窗消费面 (包 260829-schedule-reminder):
                                    * kidpal/alert 载荷解析 + 四态状态机 + 富弹窗渲染 */
#include "display/kidpal_render_node.h" /* 包 9b HWV-A6: 设备渲染节点 show_home/cycle/cache */
#include "display/kidpal_layer.h" /* 260924-m4-k2: KIDPAL_LAYER_STORY_DELETE 前台判据（动作码 7 双语义位页态消歧） */
#include "display/home_upcoming.h" /* M3-K2 A5: 首页「接下来」data.upcoming 注入冻结 seam 接线层 */
#include "esp_lvgl_port.h"         /* K7 restore 分支显式持 LVGL 锁（lvgl_port_lock/unlock）:
                                      无 begin_page 隐式持锁, 缓存写路径显式取锁/放锁 */
#include "assets/lang_config.h"    /* Lang::Sounds::OGG_POPUP —— 三件套的提示音音源 */

#define TAG "MCP"

/* P3 故事页设备消费面状态 (包 260829-story-p3)。MCP 工具回调在主线程串行执行
 * (DoToolCall -> app.Schedule)，故此处单例视图无并发竞态；story_list/story_text
 * 装载视图，start_recording 通过 kidpal_story_on_control 切录音态。零外部依赖，
 * 纯 story_page.c 冻结契约函数驱动。
 *
 * ── ssl_receive 栈纪律（包 260829-hardware-validation 真机语音轮 2 修复，勿回退）──
 * kidpal/display / kidpal/alert 这些 MCP 工具回调**同步**跑在 esp-ml307 接收任务上
 * （managed_components/78__esp-ml307/src/esp/esp_ssl.cc:62 `xTaskCreate(...,
 *  "ssl_receive", 4096, ...)` —— 第三方件，栈不可抬）。整条链
 * ReceiveTask → esp_tls recv → HttpClient → WebSocket → Application::OnIncomingJson
 * → McpServer::ParseMessage → DoToolCall → 本回调，全部叠在这 4096 字节里。
 * 因此：**禁止在本文件的路由回调链上新增大栈缓冲**（含把大结构体按值捕获进
 * Schedule 闭包前先在栈上落一份），大对象一律 make_unique / make_shared 堆分配。
 * 参考尺寸：kidpal_story_text_t 2208B、kidpal_story_list_t 2824B、
 * kidpal_story_view_t 108B、kidpal_alert_t 488B（≤1KB，可留栈）。
 * 旧实现同时压 2208B(parsed) + 2208B(parse 内局部缓冲) ≥ 4.4KB → 溢出踩进邻接任务栈，
 * 复位报「A stack overflow in task main / esp_timer」且 backtrace 腐蚀（跨任务腐蚀症状）。 */
namespace {
kidpal_story_view_t g_story_view = {};
kidpal_story_list_t g_story_list = {};
kidpal_story_text_t g_story_text = {};
}  // namespace

/* -------------------------------------------------------------------------- *
 * P3 星星记录本生产接线辅助（TaskPackage 260924-m4-k2-p3-star-notebook-ui 重写面;
 * 前身 = 包 260829-story-p3 §9 F7 / 包 9b HWV-A9）。
 * 页内输入动作码：story_page.h 现已导出冻结闭集 kidpal_story_act_t（KSPV2_ACT_*,
 * seam [K1]）——本文件按旧注释自declared 的「升级路径」转用头文件单源，不再本地
 * 镜像数值枚举；镜像注释块同步于此：旧闭集 DEGRADE-3/GLOBAL-2/NONE0/CURSOR_NEXT1/
 * OPEN_READER2/PAGE_TURN3/TTS_READ4/STOP_RECORD5/CANCEL_RECORD6 退役，新闭集 =
 * KSPV2_ACT_DEGRADE(-3)/GLOBAL(-2)/NONE(0)/OPEN_READER(2)/CURSOR_NEXT(3)/
 * SCREEN_JUMP(4)/DELETE_ENTER(5)/DELETE_CONFIRM(6)/DELETE_CANCEL(7)=REC_SESSION(7,
 * 页态消歧)/PAGE_TURN(8)/READER_EXIT(9)/STOP_RECORD(10)/DISCARD_RECORD(11)。
 * LVGL 定时器在 LVGL 主处理任务回调（= Schedule 目标线程），与 g_story_* 单例无竞态
 * （对偶 AlertDrainTimer 既有约定）。
 * ------------------------------------------------------------------------ */
namespace {
/* 录音态演出驱动：page==RECORDING 期间每 ~100ms 用冻结 kidpal_story_rec_tick 就地推
 * elapsed/闪点/声波并重绘；一旦离开录音态即自删（无常驻定时器开销）。 */
lv_timer_t* g_story_rec_timer = nullptr;
uint32_t    g_story_rec_start_ms = 0;

/* 故事页三态快照（列表视图 + 列表载荷 + 正文载荷）打包成**一个堆对象**。
 * 尺寸：view ~180B + list 2824B + text 2212B ≈ 5.2KB —— 绝不可作栈对象（见本文件
 * 头「ssl_receive 栈纪律」注）；Schedule 闭包按 shared_ptr 捕获，渲染线程侧读快照。 */
struct StorySnapshot {
    kidpal_story_view_t v;
    kidpal_story_list_t l;
    kidpal_story_text_t t;
};

void ScheduleStoryRender() {
    auto snap = std::make_shared<StorySnapshot>();
    snap->v = g_story_view;
    snap->l = g_story_list;
    snap->t = g_story_text;
    Application::GetInstance().Schedule([snap]() {
        /* 崩溃修复（包 260829-hardware-validation）语义保留：页面渲染必须夹在
         * begin/end 之间 → 画到 s_kidpal_scr + 持 LVGL 锁。
         * 双通道拆分（260924-m4-k2 seam 裁法）：本函数 = **本地导航通道**——渲染走
         * 云端夹钳纪律（崩溃修复语义零回退），夹钳后经 story_local_front 把前台
         * 回落 LOCAL_PAGE 基层（页点可见、板级 P3 页内分流基层判据成立）；云端
         * 推送路径（story_list/story_text 路由）维持夹钳后 SERVER_PAGE 前台现状。 */
        kidpal_render_node_begin_page();
        kidpal_story_page_render(&snap->v, &snap->l, &snap->t);
        /* 包 9b HWV-A6: 同步至渲染节点缓存，供 BOOT 双按翻页 cycle_next 读取。 */
        kidpal_render_node_cache_story(&snap->v, &snap->l, &snap->t);
        kidpal_render_node_end_page();
        kidpal_render_node_story_local_front();
    });
}

void StoryRecTimerCb(lv_timer_t* timer) {
    if (g_story_view.page != KSTORY_PAGE_RECORDING) {
        lv_timer_delete(timer); /* 录音态结束 -> 自删, 绝不常驻 */
        g_story_rec_timer = nullptr;
        return;
    }
    uint32_t elapsed = lv_tick_get() - g_story_rec_start_ms;
    int flash = 0;
    int wave = 0;
    if (kidpal_story_rec_tick(&g_story_view, elapsed, &flash, &wave) == 0) {
        kidpal_story_page_render(&g_story_view, &g_story_list, &g_story_text);
    }
}

void StartStoryRecTimer() {
    if (g_story_rec_timer != nullptr) {
        return; /* 幂等: 网关重试 start_recording 不得重置孩子已录时长/重复建 timer */
    }
    g_story_rec_start_ms = lv_tick_get();
    g_story_rec_timer = lv_timer_create(StoryRecTimerCb, 100, nullptr);
}

/* ---- 删除确认子态 + 消歧 + 隐式 ack 重发的设备驱动状态（260924-m4-k2 seam
 * [K2]/[K3]/[K4]/[K5] 消费侧; 纯逻辑决策表住 story_page.c, 这里只持有调用方侧
 * 状态并做 tick 驱动）。单实例消费现实 = 主线程串行（HandleStoryKey 由板级按键
 * 回调任务调用, 但 g_story_* 语义上归主线程; 消歧/重发状态 32 位对齐读写）。
 * 注：板级按键回调任务 ≠ main task——与既有 AlertTagLookup 同级宽松度, 32 位
 * 对齐的标量读写不撕裂；跨任务序由「先记账后驱动」的单写者纪律保证。 ---- */
bool     g_story_debounce_armed = false;     /* 300ms 消歧挂起单击（仅 LIST 态 arm） */
uint32_t g_story_debounce_arm_ms = 0;        /* 挂起时间戳（lv_tick_get） */
key_id_t g_story_debounce_key = KEY_ID_KEY;  /* 挂起单击的键位（冲销后按同键执行双击） */
unsigned long g_story_delete_idle_ms = 0;    /* 删除子态 10s idle 累计（seam [K3] 调用方侧） */
char     g_story_pending_delete_id[64] = {0}; /* 隐式 ack 待账 id（seam [K4]/[K5]） */
int      g_story_pending_tries = 0;          /* 已重发次数 */
uint32_t g_story_pending_sent_ms = 0;        /* 最近一次发送时刻 */
lv_timer_t* g_story_tick_timer = nullptr;    /* ~100ms tick：消歧冲销/idle/重发三驱动 */

/* ---- 正文 SD 只读镜像 + 预取 / 双击兜底共享状态单元（TaskPackage
 * 261016-p3-story-body-sd-cache; architect 预评 ②：预取队列 / 双击插队 /
 * pending 等待态 / SD 索引 = **同一套状态**——块 1（story_text 写卡+预取触发）与
 * 块 3（OPEN_READER 三分支）只消费本单元的入口，不得各建一套互踩的队列）。
 * 单实例消费现实 = mcp 主线程串行（与 g_story_* 既有约定一致）。 ---- */
kidpal_story_cache_t g_story_cache = {};      /* RAM 索引（卡上有哪些 id, [C1]） */
bool     g_story_cache_scanned = false;       /* 启动后惰性索引重建一次性门 */
char     g_story_prefetch_ids[KSTORY_CACHE_MAX][KSTORY_CACHE_ID_MAX] = {}; /* 当屏待取队列 */
int      g_story_prefetch_n = 0;              /* 队列长度（逐篇串行, 头出） */
int      g_story_prefetch_in_flight = 0;      /* 在途未收（破串行守卫） */
char     g_story_prefetch_inflight_id[KSTORY_CACHE_ID_MAX] = {0}; /* 在途篇 id */
uint32_t g_story_prefetch_sent_ms = 0;        /* 最近一次发帧时刻（篇间间隔输入） */
uint32_t g_story_prefetch_inflight_ms = 0;    /* 在途起点（丢帧有界收口） */
bool     g_story_fetch_pending = false;       /* 双击在等该篇应答（SC-A4 等待态） */
char     g_story_fetch_pending_id[KSTORY_CACHE_ID_MAX] = {0};
uint32_t g_story_fetch_pending_ms = 0;        /* 双击等待起点（[C5] 10s 超时输入） */
/* 已发 kidpal_story_fetch 的 id 小环（[C8] from_fetch 判据：fetch 应答恒不盖章） */
char     g_story_fetch_issued[KSTORY_CACHE_MAX][KSTORY_CACHE_ID_MAX] = {};
int      g_story_fetch_issued_n = 0;
unsigned long g_story_body_unavailable_idle_ms = 0; /* WF-F2-06 覆盖层 10s idle 累计 */

/* 前置声明（StoryTickTimerCb 与 StoryDispatchAct 互序） */
bool StoryDispatchAct(int act);
/* 预取 / 双击等待 tick 驱动（定义在 EnsureStoryTickTimer 之后） */
bool StoryPrefetchTick(void);

/* ~100ms tick 三驱动（seam [K2] 消歧冲销执行 / [K3] 10s idle 静默取消 / [K4] 5s
 * 重发）——禁 1s tick 驱动消歧（300ms 死线粒度不够, plan 机制形态注记）。全部
 * 驱动完空转即自删（无常驻开销）。 */
void StoryTickTimerCb(lv_timer_t* timer) {
    uint32_t now = lv_tick_get();
    bool busy = false;

    /* ① 消歧到期冲销（seam [K2]）: 挂起单击过 300ms 死线 -> 直发单击动作
     * （经 input + dispatch 单点, 绝不经 HandleStoryKey——否则单击会被重新挂起）。 */
    if (g_story_debounce_armed) {
        if (kidpal_story_debounce_expired(g_story_debounce_arm_ms, now) == 1) {
            key_id_t key = g_story_debounce_key;
            g_story_debounce_armed = false;
            (void)StoryDispatchAct(
                kidpal_story_page_input(&g_story_view, key, KEY_GESTURE_CLICK));
        } else {
            busy = true;
        }
    }

    /* ② 删除子态 10s idle（seam [K3]）: 到点静默取消回列表（手势重计由
     * HandleStoryKey 消费侧复位累计）。取消 = 任意 BOOT 手势同径（输入函数 DELETE
     * 态消费侧）经 dispatch 单点关层回列表。 */
    if (g_story_view.page == KSTORY_PAGE_DELETE) {
        if (kidpal_story_delete_idle_step(&g_story_delete_idle_ms, 100UL) == 1) {
            g_story_delete_idle_ms = 0;
            (void)StoryDispatchAct(
                kidpal_story_page_input(&g_story_view, KEY_ID_BOOT, KEY_GESTURE_CLICK));
        } else {
            busy = true;
        }
    } else {
        g_story_delete_idle_ms = 0; /* 非删除子态: 不计时（防历史累计误取消） */
    }

    /* ③ 隐式 ack 重发（seam [K4]）: 5s 间隔上限 3 次重发同一 id; 放弃后静默停发
     * （对账冲销 = story_list 重推到达时路由侧消费）。 */
    if (g_story_pending_delete_id[0] != '\0') {
        if (kidpal_story_delete_resend_due(g_story_pending_tries,
                                           now - g_story_pending_sent_ms) == 1) {
            ESP_LOGW(TAG, "story_delete_resend corr_id=kdp-%u id=%s tries=%d",
                     static_cast<unsigned>(now & 0xFFFFFFu),
                     g_story_pending_delete_id, g_story_pending_tries);
            Application::GetInstance().SendKidpalStoryDelete(g_story_pending_delete_id);
            g_story_pending_tries++;
            g_story_pending_sent_ms = now;
        }
        busy = true;
    }

    /* ④ 正文取不回覆盖层 10s idle（WF-F2-06 / AC7; 静默消失——不显示倒计时;
     * 到点静默关闭回列表并复位。任一手势重计 = HandleStoryKey 消费侧义务）。 */
    if (g_story_view.page == KSTORY_PAGE_BODY_UNAVAILABLE) {
        if (kidpal_story_body_unavailable_idle_step(&g_story_body_unavailable_idle_ms,
                                                    100UL) == 1) {
            g_story_body_unavailable_idle_ms = 0;
            g_story_view.page = KSTORY_PAGE_LIST;
            ESP_LOGI(TAG, "story_body_unavailable idle_close");
            ScheduleStoryRender();
        } else {
            busy = true;
        }
    } else {
        g_story_body_unavailable_idle_ms = 0; /* 非该子态: 不计时（防历史累计误关） */
    }

    /* ⑤ 预取 / 双击等待（SC-A1/A4/A5）: 纯逻辑决策住 story_cache.c, 此处只注入
     * 显式输入并执行发帧 / 降级出参。 */
    if (StoryPrefetchTick()) {
        busy = true;
    }

    if (!busy) {
        lv_timer_delete(timer); /* 三驱动全空 -> 自删, 绝不常驻 */
        g_story_tick_timer = nullptr;
    }
}

void EnsureStoryTickTimer() {
    if (g_story_tick_timer == nullptr) {
        g_story_tick_timer = lv_timer_create(StoryTickTimerCb, 100, nullptr);
    }
}

/* -------------------------------------------------------------------------- *
 * P3 正文 SD 只读镜像 / 预取 / 双击兜底 —— 消费侧辅助（261016-p3-story-body-sd-cache）
 *
 * 正确性不对称（plan「关键接线决策」）：**预取是优化、双击插队 fetch 是正确性保证**
 * ——预取挂掉功能照常，插队兜底保证双击必达（或给出可见降级）。
 * ------------------------------------------------------------------------ */

/* RAM 索引惰性重建（启动后一次; 热插拔检测属非目标）。无卡 -> 空索引（整层失效,
 * 双击走 fetch / 降级路径）。 */
void StoryCacheEnsureLoaded() {
    if (g_story_cache_scanned) {
        return;
    }
    g_story_cache_scanned = true;
    (void)kidpal_story_cache_init(&g_story_cache);
    (void)kidpal_story_sd_rebuild_index(
        &g_story_cache, Board::GetInstance().SdCardMounted() ? 1 : 0);
}

/* 已发 fetch 的 id 登记（小环, 满则挤最旧）：answers 判据 from_fetch（[C8] 不盖章）。 */
void StoryMarkFetchIssued(const char* id) {
    int i;

    if (id == nullptr || id[0] == '\0') {
        return;
    }
    for (i = 0; i < g_story_fetch_issued_n; i++) {
        if (std::strcmp(g_story_fetch_issued[i], id) == 0) {
            return; /* 已在环（重复请求不重复记账） */
        }
    }
    if (g_story_fetch_issued_n >= KSTORY_CACHE_MAX) {
        for (i = 0; i + 1 < g_story_fetch_issued_n; i++) {
            std::memcpy(g_story_fetch_issued[i], g_story_fetch_issued[i + 1],
                        KSTORY_CACHE_ID_MAX);
        }
        g_story_fetch_issued_n--;
    }
    std::snprintf(g_story_fetch_issued[g_story_fetch_issued_n], KSTORY_CACHE_ID_MAX,
                  "%s", id);
    g_story_fetch_issued_n++;
}

/* 应答到达时消费 issued 记账: 1 = 本帧是 kidpal_story_fetch 的应答（from_fetch）;
 * 0 = 停录归档 / 历史推送等非 fetch 路径。 */
int StoryConsumeFetchIssued(const char* id) {
    int i;
    int j;

    if (id == nullptr || id[0] == '\0') {
        return 0;
    }
    for (i = 0; i < g_story_fetch_issued_n; i++) {
        if (std::strcmp(g_story_fetch_issued[i], id) == 0) {
            for (j = i; j + 1 < g_story_fetch_issued_n; j++) {
                std::memcpy(g_story_fetch_issued[j], g_story_fetch_issued[j + 1],
                            KSTORY_CACHE_ID_MAX);
            }
            g_story_fetch_issued_n--;
            std::memset(g_story_fetch_issued[g_story_fetch_issued_n], 0,
                        KSTORY_CACHE_ID_MAX);
            return 1;
        }
    }
    return 0;
}

/* id 是否在预取队列（双击插队时去重：同一篇只发一次帧）。 */
void StoryPrefetchDrop(const char* id) {
    int i;
    int j;

    if (id == nullptr) {
        return;
    }
    for (i = 0; i < g_story_prefetch_n; i++) {
        if (std::strcmp(g_story_prefetch_ids[i], id) == 0) {
            for (j = i; j + 1 < g_story_prefetch_n; j++) {
                std::memcpy(g_story_prefetch_ids[j], g_story_prefetch_ids[j + 1],
                            KSTORY_CACHE_ID_MAX);
            }
            g_story_prefetch_n--;
            std::memset(g_story_prefetch_ids[g_story_prefetch_n], 0, KSTORY_CACHE_ID_MAX);
            return;
        }
    }
}

/* 发帧原语（主任务闭包内调 SendKidpalStoryFetch——协议发送归主任务, 与
 * SendMcpMessage / SendKidpalStoryDelete 同款线程纪律；本函数只做拷贝与记账）。 */
void StorySendFetchBatch(const char* const* ids, int count) {
    int i;

    if (ids == nullptr || count <= 0) {
        return; /* 空 ids = no-op（契约 §四） */
    }
    for (i = 0; i < count; i++) {
        StoryMarkFetchIssued(ids[i]);
    }
    std::vector<std::string> copy;
    copy.reserve(static_cast<size_t>(count));
    for (i = 0; i < count; i++) {
        copy.emplace_back(ids[i] != nullptr ? ids[i] : "");
    }
    Application::GetInstance().Schedule([copy]() {
        std::vector<const char*> ptrs;
        ptrs.reserve(copy.size());
        for (const auto& s : copy) {
            ptrs.push_back(s.c_str());
        }
        SendKidpalStoryFetch(ptrs.empty() ? nullptr : ptrs.data(),
                             static_cast<int>(ptrs.size()));
    });
}

/* 当屏缺失批次重算（AC2「进列表 / 翻屏先行为当屏各篇取回本地缺失的正文」）。
 * 出参缓冲走文件级静态（1KB）——本函数在 story_list 路由回调链上被调用（ssl_receive
 * 4096B 栈纪律：禁止新增大栈缓冲, 见本文件头注）。 */
void StoryPrefetchRefresh() {
    static char ids[KSTORY_CACHE_MAX][KSTORY_CACHE_ID_MAX];
    int i;
    int n;

    StoryCacheEnsureLoaded();
    g_story_prefetch_n = 0;
    /* 在途标记保留（逐篇串行语义跨「翻屏重算」成立——上一帧未收前不发新帧）。 */
    if (!Board::GetInstance().SdCardMounted()) {
        return; /* 卡不在位：预取无落点（不产生网络帧） */
    }
    if (g_story_view.page != KSTORY_PAGE_LIST) {
        return; /* 仅列表态预取（离开 P3 即停） */
    }
    n = kidpal_story_cache_missing_on_screen(
        &g_story_cache, g_story_list.items, g_story_list.count, g_story_view.screen,
        KSTORY_LIST_ROWS_PER_SCREEN, ids);
    if (n <= 0) {
        return;
    }
    for (i = 0; i < n && i < KSTORY_CACHE_MAX; i++) {
        std::memcpy(g_story_prefetch_ids[i], ids[i], KSTORY_CACHE_ID_MAX);
    }
    g_story_prefetch_n = n;
    /* 首篇即时可发（篇间间隔从「上一帧」起算；队列新装 = 无上一帧）。 */
    g_story_prefetch_sent_ms = lv_tick_get() - static_cast<uint32_t>(KSTORY_FETCH_INTERVAL_MS);
    EnsureStoryTickTimer();
    ESP_LOGI(TAG, "story_prefetch_armed screen=%d missing=%d", g_story_view.screen, n);
}

/* WF-F2-06 可见降级（AC7）: 整屏覆盖层页态 + 10s idle 起算 + 落屏。 */
void EnterStoryBodyUnavailable() {
    g_story_fetch_pending = false;
    g_story_fetch_pending_id[0] = '\0';
    g_story_body_unavailable_idle_ms = 0;
    g_story_view.page = KSTORY_PAGE_BODY_UNAVAILABLE;
    EnsureStoryTickTimer();
    ScheduleStoryRender();
    ESP_LOGW(TAG, "story_open_degrade page=%d (WF-F2-06 visible hint)",
             g_story_view.page);
}

/* 预取 / 双击等待 tick 驱动（~100ms）。返回 true = 仍需 tick（勿自删）。 */
bool StoryPrefetchTick() {
    uint32_t now = lv_tick_get();
    bool busy = false;
    int voice_front;
    int ws_busy;
    unsigned long since;
    const char* one;

    /* ① 双击等待超时（[C5] (c)）: 10s 取不回 / 卡不在位且拉取失败 -> 可见降级。 */
    if (g_story_fetch_pending) {
        unsigned long wait_ms = static_cast<unsigned long>(now - g_story_fetch_pending_ms);
        if (kidpal_story_open_decide(1, 0, 0, 0, 1, wait_ms) == KSTORY_OPEN_DEGRADE) {
            ESP_LOGW(TAG, "story_fetch_timeout id=%s wait_ms=%lu",
                     g_story_fetch_pending_id, wait_ms);
            EnterStoryBodyUnavailable();
        } else {
            busy = true; /* 等待中（应答到达即由 STORY_TEXT 路由收口） */
        }
    }

    voice_front = (kidpal_render_node_front_layer() == KIDPAL_LAYER_VOICE) ? 1 : 0;

    /* ② 预取挂起（离开 P3 / 语音前台 -> 立即挂起并清在途队列, [C6]）。 */
    if (kidpal_story_prefetch_suspend(g_story_view.page, voice_front) == 1) {
        if (g_story_prefetch_n > 0 || g_story_prefetch_in_flight != 0) {
            ESP_LOGI(TAG, "story_prefetch_suspend page=%d voice_front=%d",
                     g_story_view.page, voice_front);
        }
        g_story_prefetch_n = 0;
        g_story_prefetch_in_flight = 0;
        g_story_prefetch_inflight_id[0] = '\0';
        return busy;
    }

    /* ③ 在途丢帧有界收口（防一次应答丢失令预取永停；双击兜底不受影响）。 */
    if (g_story_prefetch_in_flight != 0 &&
        static_cast<uint32_t>(now - g_story_prefetch_inflight_ms) >= 15000u) {
        g_story_prefetch_in_flight = 0;
        g_story_prefetch_inflight_id[0] = '\0';
    }
    if (g_story_prefetch_n <= 0) {
        return busy;
    }

    /* ④ 发帧决策（显式注入 page_state / voice_front / ws_busy / in_flight /
     * pending_count / since_last_ms——纯逻辑住 story_cache.c, [C6]）。 */
    ws_busy = (Application::GetInstance().GetDeviceState() != kDeviceStateIdle) ? 1 : 0;
    since = static_cast<unsigned long>(now - g_story_prefetch_sent_ms);
    if (kidpal_story_prefetch_due(g_story_view.page, voice_front, ws_busy,
                                  g_story_prefetch_in_flight, g_story_prefetch_n,
                                  since) == 1) {
        /* 先拷贝队头 id 再出队（StoryPrefetchDrop 会重排数组，指针随后失效）。 */
        char sent_id[KSTORY_CACHE_ID_MAX];
        std::snprintf(sent_id, sizeof(sent_id), "%s", g_story_prefetch_ids[0]);
        one = sent_id;
        StorySendFetchBatch(&one, 1);
        StoryPrefetchDrop(sent_id); /* 头出队列（发帧已拷贝 id） */
        g_story_prefetch_in_flight = 1;
        std::snprintf(g_story_prefetch_inflight_id, sizeof(g_story_prefetch_inflight_id),
                      "%s", sent_id);
        g_story_prefetch_inflight_ms = now;
        g_story_prefetch_sent_ms = now;
        ESP_LOGI(TAG, "story_prefetch_send id=%s remaining=%d", sent_id,
                 g_story_prefetch_n);
    }
    return true;
}

/* 删除确认覆盖层渲染（DELETE_ENTER 消费侧）：STORY_DELETE 层夹钳 + DELETE 态
 * 渲染面（「删掉这篇？」+ 待删条目; WF-F2-05 整屏覆盖, 页点经 front_is_overlay
 * 隐藏）。Schedule 闭包内成对调用（与 begin/end_page 同纪律）。 */
void ScheduleStoryDeleteOverlay() {
    auto snap = std::make_shared<StorySnapshot>();
    snap->v = g_story_view;
    snap->l = g_story_list;
    snap->t = g_story_text;
    Application::GetInstance().Schedule([snap]() {
        kidpal_render_node_begin_story_delete();
        kidpal_story_page_render(&snap->v, &snap->l, &snap->t);
        kidpal_render_node_end_story_delete();
    });
}

/* 页内动作码 dispatch 单点（HandleStoryKey 与消歧 tick 到期直发共用; 返回 = 是否
 * 消费）。单源纪律：动作码语义除输入函数外只落本函数一处。 */
bool StoryDispatchAct(int act) {
    switch (act) {
        /* 数值 7 = 双语义位（DELETE_CANCEL == REC_SESSION 同码, 页态消歧——C++ 单一
         * case 标签, 枚举双名在此合并消费）。 */
        case KSPV2_ACT_REC_SESSION:
            if (kidpal_render_node_front_layer() == KIDPAL_LAYER_STORY_DELETE) {
                /* 取消回列表（DELETE 态 BOOT 任一手势 / 零副作用）: 关删除覆盖层 +
                 * 回列表重渲。 */
                ESP_LOGI(TAG, "story_key delete_cancel");
                Application::GetInstance().Schedule([]() {
                    kidpal_render_node_dismiss_front_overlay();
                });
                ScheduleStoryRender();
            } else {
                /* 记录语境会话语义位（S-F2.2 双入口; K3 会话动作接线消费位——本包
                 * 只钉页内缝, 消费即返回、绝不落回全局 SESSION_TOGGLE）。 */
                ESP_LOGI(TAG, "story_key rec_session_slot page=%d", g_story_view.page);
                /* K3 [S6]/[S2]: 会话发起接线——置记录语境标志（引导气泡覆写 / 开口
                 * 切录音态 / stt 旁路喂入的消费门; 清位 = 会话收尾 Idle 分支）+
                 * 回显区清零（防上一轮话语串味）+ 推语音层前台（GLOBAL_ACT_SESSION_
                 * TOGGLE 先例, Schedule 内层操作）+ manual 聆听入口（-> Listening
                 * 分支 -> kidpal_session_ctx 发送点; 双入口 KEY 半汇入）。 */
                kidpal_story_session_ctx_set(1);
                kidpal_story_echo_reset();
                Application::GetInstance().Schedule([]() {
                    kidpal_render_node_show_chat();
                });
                Application::GetInstance().StartListening();
            }
            return true;
        case KSPV2_ACT_DELETE_ENTER:
            /* 进删除确认子态: 页态已切 DELETE; 推 STORY_DELETE 覆盖层（整屏、页点
             * 隐藏）+ 渲染删除确认面。启动 10s idle / 消歧 tick。 */
            ESP_LOGI(TAG, "story_key delete_enter cursor=%d", g_story_view.cursor);
            EnsureStoryTickTimer();
            ScheduleStoryDeleteOverlay();
            return true;
        case KSPV2_ACT_DELETE_CONFIRM: {
            /* 确认删除（边角⑨）: 本地即时移除已由输入函数置账（pending_delete_id /
             * count-- / counter--）; 此处（a）把列表载荷同步本地移除（渲染一致性,
             * 云端 story_list 重推到达后以 total 校准）;（b）发送 kidpal_story_delete
             * （id 只发 story_list items 下发过的 id——闭集来源 = view_init 捕获影子）;
             * （c）记 pending {id, tries=0, sent_ts} 进隐式 ack 重发对账;（d）关删除
             * 覆盖层回列表重渲。 */
            const char* pid = kidpal_story_pending_delete_id(&g_story_view);
            if (pid != nullptr && pid[0] != '\0') {
                int i;
                for (i = 0; i < g_story_list.count; i++) {
                    if (std::strcmp(g_story_list.items[i].id, pid) == 0) {
                        int j;
                        for (j = i; j + 1 < g_story_list.count; j++) {
                            g_story_list.items[j] = g_story_list.items[j + 1];
                        }
                        std::memset(&g_story_list.items[g_story_list.count - 1], 0,
                                    sizeof(g_story_list.items[0]));
                        g_story_list.count--;
                        break;
                    }
                }
                ESP_LOGI(TAG, "story_key delete_confirm corr_id=kdp-%u id=%s count=%d",
                         static_cast<unsigned>(lv_tick_get() & 0xFFFFFFu), pid,
                         g_story_list.count);
                std::snprintf(g_story_pending_delete_id,
                              sizeof(g_story_pending_delete_id), "%s", pid);
                g_story_pending_tries = 0;
                g_story_pending_sent_ms = lv_tick_get();
                Application::GetInstance().SendKidpalStoryDelete(pid);
                EnsureStoryTickTimer();
            }
            Application::GetInstance().Schedule([]() {
                kidpal_render_node_dismiss_front_overlay();
            });
            ScheduleStoryRender();
            return true;
        }
        case KSPV2_ACT_OPEN_READER: {
            /* 双击三分支（TaskPackage 261016-p3-story-body-sd-cache; SC-A3/A4/A5/A6）:
             *   (1) 命中本地镜像（卡在位 + RAM 索引有该篇）-> 读卡进阅读态, **零网络帧**;
             *   (2) 未命中 -> 插队单篇 kidpal_story_fetch（绕过预取串行 / 篇间间隔闸,
             *       优先级高于后台预取）-> 应答到达后进阅读态（STORY_TEXT 路由收口）;
             *   (3) 取不回（离线 / 超时 / 卡不在位且拉取失败）-> WF-F2-06 可见降级。
             * id 错配修复（TECH-261013「显示错内容比无反应更危险」）: 正文一律按
             * 「光标所选行 id」取（卡分支按 id 读文件 / fetch 分支按 id 等应答）,
             * **绝不**沿用上一轮遗留的 g_story_text——旧实现盲开缓存正文 =
             * 显示别篇内容的净根因; 应答侧另以 kidpal_story_reader_id_match 复核。 */
            const char* sel_id = nullptr;
            int card_ready;
            int has_local;
            int dec;

            StoryCacheEnsureLoaded();
            if (g_story_view.cursor >= 0 && g_story_view.cursor < g_story_list.count) {
                sel_id = g_story_list.items[g_story_view.cursor].id;
            }
            card_ready = Board::GetInstance().SdCardMounted() ? 1 : 0;
            has_local = (sel_id != nullptr) ? kidpal_story_cache_has(&g_story_cache, sel_id) : 0;
            dec = kidpal_story_open_decide(sel_id != nullptr ? 1 : 0, has_local,
                                           card_ready, 0, 0, 0);
            if (dec == KSTORY_OPEN_HIT_CARD && sel_id != nullptr) {
                if (kidpal_story_sd_read(sel_id, g_story_text.text,
                                         sizeof(g_story_text.text), card_ready) == 1) {
                    /* 本地镜像只留正文（决策 24）: 标题 / 日期沿用云端列表行。 */
                    std::snprintf(g_story_text.id, sizeof(g_story_text.id), "%s", sel_id);
                    std::snprintf(g_story_text.title, sizeof(g_story_text.title), "%s",
                                  g_story_list.items[g_story_view.cursor].title);
                    g_story_text.seq = KSTORY_SEQ_ABSENT;
                    (void)kidpal_story_open_reader(&g_story_view, &g_story_text,
                                                   KSTORY_READER_LINES_PER_SCREEN);
                    ScheduleStoryRender();
                    ESP_LOGI(TAG, "story_open_hit_card id=%s (zero network frame)",
                             sel_id);
                } else {
                    /* 索引与卡内容漂移（文件被删 / 卡中段失效）: 该篇移出索引后
                     * 走在线取回路径（不整层清空）。 */
                    (void)kidpal_story_cache_remove(&g_story_cache, sel_id);
                    dec = KSTORY_OPEN_FETCH;
                }
            }
            if (dec == KSTORY_OPEN_FETCH && sel_id != nullptr) {
                const char* one = sel_id;
                StoryPrefetchDrop(sel_id); /* 插队去重：该篇不再从后台队列重复发帧 */
                if (!(g_story_prefetch_in_flight != 0 &&
                      std::strcmp(g_story_prefetch_inflight_id, sel_id) == 0)) {
                    StorySendFetchBatch(&one, 1);
                } /* 该篇已在预取在途 -> 直接复用那一帧的应答（不重复发帧） */
                g_story_fetch_pending = true;
                std::snprintf(g_story_fetch_pending_id,
                              sizeof(g_story_fetch_pending_id), "%s", sel_id);
                g_story_fetch_pending_ms = lv_tick_get();
                EnsureStoryTickTimer();
                ESP_LOGI(TAG, "story_open_fetch id=%s (queued ahead of prefetch)",
                         sel_id);
            } else if (dec == KSTORY_OPEN_DEGRADE) {
                EnterStoryBodyUnavailable();
            }
            return true;
        }
        case KSPV2_ACT_CURSOR_NEXT:
        case KSPV2_ACT_SCREEN_JUMP:
            /* 本地导航/子态动作: 页态变更已由输入函数就地完成——双通道本地路径
             * 落屏（cache_story + refresh_story, 前台保持 LOCAL_PAGE、页点可见）。
             * 光标下移 / 翻屏 -> 当屏缺失批次重算（AC2「翻屏时先行为当屏各篇
             * 取回本地缺失的正文」; 卡不在位 / 非列表态时 refresh 内部 no-op）。 */
            StoryPrefetchRefresh();
            ScheduleStoryRender();
            return true;
        case KSPV2_ACT_PAGE_TURN:
        case KSPV2_ACT_READER_EXIT:
            /* 本地导航/子态动作: 页态变更已由输入函数就地完成——双通道本地路径
             * 落屏（cache_story + refresh_story, 前台保持 LOCAL_PAGE、页点可见）。 */
            ScheduleStoryRender();
            return true;
        case KSPV2_ACT_STOP_RECORD:
            /* K3 [S5]: KEY 兜底收尾（短按 & 长按同义, v1.47 防毁稿）= 收尾保存——
             * 调 Application StopListening 面（MAIN_EVENT_STOP_LISTENING ->
             * SendStopListening + Idle; front _story_stop_on_listen_stop 消费
             * listen/state=stop 上行触发 stop 归档 + story_text/story_list 下发）。
             * 负锚（notes (e), 两条 front 路径不混的板端根）: 严禁走会话 toggle/
             * 断连路径——toggle 的 Listening 分支直接关通道不发 listen stop ->
             * front 走 ws_disconnect cancel 丢弃路径而非归档 = 毁稿。 */
            ESP_LOGI(TAG, "story_key stop_rec archive_pend=%d",
                     g_story_view.archive_pend);
            Application::GetInstance().StopListening();
            ScheduleStoryRender();
            return true;
        case KSPV2_ACT_DISCARD_RECORD:
            /* K3 [S5]: BOOT 任一手势 = 丢弃退回列表（不设重录恢复 v1.49）——关通道
             * 即达 front 断开钩子 acc.cancel(session_id, reason="ws_disconnect")
             * （弹缓冲不落盘）。负锚: 严禁走收尾保存面（listen stop 上行会触发
             * front stop 归档 = 丢弃反被落盘, 语义反转回潮; 旧冻结表「KEY 长按=
             * 丢弃」已退役）。 */
            ESP_LOGI(TAG, "story_key discard_rec cancel_count=%d",
                     g_story_view.cancel_count);
            Application::GetInstance().CloseAudioChannel();
            ScheduleStoryRender();
            return true;
        case KSPV2_ACT_INTERRUPT_GUIDE:
            /* 引导播报中打断（261019-p3-record-dialogue-guide [G3]/[G6] 消费闭环）:
             * KEY 短/长按打断引导播报、继续聆听——页态保持 RECORDING、子态复位 0 已由
             * 输入函数就地完成; 此处消费 = AbortSpeaking 既有打断路径（application.cc
             * :927 同族——轮来源 = Listening, tts stop 出口判据③播完回 Listening）。
             * 零 cancel/归档副作用（打断不是收尾也不是丢弃）; 不发 listen stop、不离
             * 记录会话（继续聆听）。 */
            ESP_LOGI(TAG, "story_key interrupt_guide page=%d", g_story_view.page);
            Application::GetInstance().AbortSpeaking(kAbortReasonNone);
            ScheduleStoryRender();
            return true;
        case KSPV2_ACT_NONE:
            /* 页拥有该手势但无副作用（不响应 = 消费不动）——返回 true 吃掉手势,
             * 绝不交回全局路由（删除子态 NONE 漏网 = 误开会话, K1 层派发教训）。 */
            return true;
        default:
            /* KSPV2_ACT_GLOBAL / KSPV2_ACT_DEGRADE -> 交回全局路由。 */
            return false;
    }
}
}  // namespace

/* -------------------------------------------------------------------------- *
 * K3 (260924-m4-k3-record-session-live): VAD 沿切录音态的视图动作桥。
 * application.cc 主循环消费 kidpal_story_recording_enter 决策出参（==1）后经此桥
 * 执行视图半——复用 control/start_recording 冻结消费语义（kidpal_story_on_control
 * 幂等: 非 RECORDING 才切页; 已在录音态重入 = no-op + 录音态渲染 + 演出 tick）。
 * 不进 mcp_server.h 公共头（冻结面零改动, 消费方 extern 声明; 变更清单不含该头,
 * 免扩公共面）。主线程串行消费现实与 g_story_* 既有约定一致。
 * ------------------------------------------------------------------------ */
/* K3R [R5] 权威会话页态镜像投递桥（定义住 kidpal_render_node.cc; mcp_server.h 冻结面
 * 零改动, 消费方 extern 声明——与 KidpalStoryVadEnterRecording 同款免扩公共头纪律）。 */
extern "C" void kidpal_render_node_story_session_page(int page);

void KidpalStoryVadEnterRecording() {
    int consumed = kidpal_story_on_control(&g_story_view, VRT_CTRL_START_RECORDING);
    ESP_LOGI(TAG, "story_vad_recording_enter consumed=%d page=%d",
             consumed, g_story_view.page);
    if (consumed == 1) {
        /* K3R [R5]（2026-10-01 真机轮架构修）: 录音 enter 路径把权威会话页态
         * （g_story_view.page == RECORDING）(a) 经镜像桥投递进渲染节点, 作 voice_end
         * [R5] 的 session_page 权威源（与节点缓存分叉时取真值）; (b) cache_story 预同步
         * 节点缓存（令 [R4]/chrome 判据见 RECORDING）。页态同步先于渲染。 */
        kidpal_render_node_story_session_page(g_story_view.page);
        kidpal_render_node_cache_story(&g_story_view, &g_story_list, &g_story_text);
        /* K3R D2（甲形态）: 开口进录音态 -> VOICE 层让位（REC_SESSION 位在**会话发起**
         * 时推 VOICE 层 [kidpal_render_node_show_chat]; 开口切录音态时让位——基层无覆盖层,
         * 录音页前台可见 + D4a 基层判据天然成立）。voice_end 弹 VOICE 后顶层 = LOCAL_PAGE
         * （本地页 = P3 时回落 P3）; 其 rn_voice_chrome_front_locked 经 D2 强屏蔽桥把手
         * 段投到 LCD sync 执行点, 录音态强制隐声波 chrome（不寄望判据链 runtime 生效）。
         * ScheduleStoryRender 随后把录音页渲染落屏, 录音页不再被声波层压住。 */
        kidpal_render_node_voice_end();
        ScheduleStoryRender();
        StartStoryRecTimer();
    }
}

/* -------------------------------------------------------------------------- *
 * K3R [R2] 会话收尾消费侧桥（261001-m4-k3-record-session-live-repair, D4b-1）。
 * application.cc Idle 分支消费 story_page.c 纯逻辑缝 kidpal_story_session_end 出参
 * 后经此二桥执行设备半——读 g_story_view.page 供判据（KidpalStoryViewPageState）+
 * 复位录音页态并停录音演出定时器（KidpalStorySessionEndApply）。不进 mcp_server.h
 * 公共头（冻结面零改动, 消费方 extern 声明; 变更清单不含该头, 免扩公共面, 与
 * KidpalStoryVadEnterRecording 同款纪律）。主线程串行消费现实与 g_story_* 既有约定一致。
 * ------------------------------------------------------------------------ */
int KidpalStoryViewPageState() {
    return g_story_view.page;
}

void KidpalStorySessionEndApply(int list_page) {
    g_story_view.page = list_page; /* [R2] 出参 = LIST; 页态回落 -> 定时器自删条件成立 */
    if (g_story_rec_timer != nullptr) {
        /* 即时停录音演出定时器（不依赖下一拍 page != RECORDING 自删——收尾即停,
         * 防其在首页重建后仍每 100ms 清屏砸毁首页图像对象, D4b 根）。 */
        lv_timer_delete(g_story_rec_timer);
        g_story_rec_timer = nullptr;
    }
    /* 同步渲染节点缓存 + 权威会话页态镜像（令前台回落路径 voice_end 的 [R3]/[R5] 判据
     * 见 LIST 真值; 录音态强屏蔽随之复位——voice_end 重算 chrome 时 page_state != RECORDING
     * -> 强屏蔽位落 0, 声波 chrome 按常规语义恢复）。 */
    kidpal_render_node_story_session_page(g_story_view.page);
    kidpal_render_node_cache_story(&g_story_view, &g_story_list, &g_story_text);
    ESP_LOGI(TAG, "story_session_end corr_id=kdp-%u page=%d",
             static_cast<unsigned>(lv_tick_get() & 0xFFFFFFu), list_page);
}

/* 从一段 E1 display 载荷的嵌套 data JSON 里取一个字符串字段 (如 text / action)。
 * 取不到/非字符串 -> 空串 (调用方按降级或空内容处理, 不崩)。 */
static std::string mcp_json_get_string(const std::string& json, const char* key) {
    if (json.empty()) {
        return std::string();
    }
    cJSON *root = cJSON_Parse(json.c_str());
    if (root == nullptr) {
        return std::string();
    }
    std::string out;
    cJSON *item = cJSON_GetObjectItem(root, key);
    if (cJSON_IsString(item)) {
        out = item->valuestring;
    }
    cJSON_Delete(root);
    return out;
}

/* 从一段 E1 display 载荷的嵌套 data JSON 里读一个布尔字段（如 restore）。缺省/非
 * JSON true 字面 -> false（契约 §1.3② data.restore additive：absent/false 同判,
 * 只有 JSON true 才算真）。对端 grid restore 载荷 data = json.dumps(对象)，布尔
 * true/false 字面在数据对象顶层。 */
static bool mcp_json_get_bool(const std::string& json, const char* key) {
    if (json.empty()) {
        return false;
    }
    cJSON *root = cJSON_Parse(json.c_str());
    if (root == nullptr) {
        return false;
    }
    cJSON *item = cJSON_GetObjectItem(root, key);
    bool out = cJSON_IsTrue(item) == 1;
    cJSON_Delete(root);
    return out;
}

McpServer::McpServer() {
}

McpServer::~McpServer() {
    for (auto tool : tools_) {
        delete tool;
    }
    tools_.clear();
}

/* -------------------------------------------------------------------------- *
 * kidpal/alert 设备接线 (TaskPackage 260829-schedule-reminder, 契约 §三 提醒推送 /
 * §四 mcp 工具调用, spec S-F1.4, 决策 20·23·24·27)。
 *
 * 四态解耦在设备侧的落点（RMD-A6「接收不回执、渲染完成才回执」）：
 *   ① 工具回调只做「解析 -> 入易失 known-id 集 + 排队」，并回一个**明确不表示送达**的
 *      accepted 结果（"ack": false）—— DoToolCall 的同步自动回复形状因此零改动；
 *   ② 渲染完成时才经 McpServer::ReplyResult(rpc_id, …) 独立发出携带 "ack": true 的
 *      JSON-RPC 回执（= 契约 §三「渲染即确认、即出队」），发出前必过
 *      kidpal_reminder_may_emit_receipt 判定门；
 *   ③ 同 id 重推两态分处：排队未渲染 -> 幂等屏蔽（不入队、不回执）；已渲染驻留 ->
 *      当场补回回执（回执在途丢失时网关重推是唯一收敛路径，重复弹窗必须根除）。
 * 全部状态变更都发生在主循环任务（Schedule 回调 + LVGL timer），彼此无竞态。
 * ------------------------------------------------------------------------ */

/* 三件套的提示音一半：覆盖 reminder_page.c 的弱默认缝（host runner 无声源 -> 静默）。
 * 音源 = 小智既有弹窗音效（先例 application.cc: audio_service_.PlaySound(OGG_POPUP)）。 */
void kidpal_reminder_play_popup_tone(void) {
    Application::GetInstance().PlaySound(Lang::Sounds::OGG_POPUP);
}

namespace {

/* 业务 id -> JSON-RPC 传输数字 id 的关联表：定长环形、零增长（设备侧有界性同 KRM_*）。
 * 渲染回执必须带**同一数字 id**（§三 业务 id 是字符串、设备 mcp 层只认数字 id：
 * ParseMessage 的 cJSON_IsNumber 判据），而排队项在稍后才弹出、届时手上没有那次调用
 * 的 id —— 故到达时记一次。回执同时携带 reminder_id，即使表被冲掉也能按业务 id 归位
 * （网关 consume_receipts 数字 id 与业务 id 都收）。 */
constexpr int kAlertTagCap = 16;
struct AlertTag {
    char id[KRM_CAP_ID];
    int rpc_id;
};
AlertTag g_alert_tags[kAlertTagCap];
int g_alert_tag_next = 0;
lv_timer_t *g_alert_drain_timer = nullptr;

void AlertTagRemember(const std::string& business_id, int rpc_id) {
    if (rpc_id <= 0) {
        return; /* 无有效传输 id 可记（回执仍有 reminder_id 可归位） */
    }
    for (auto& tag : g_alert_tags) {
        if (tag.rpc_id > 0 && std::string(tag.id) == business_id) {
            tag.rpc_id = rpc_id; /* 重推：原地刷新为最新在途 id */
            return;
        }
    }
    auto& slot = g_alert_tags[g_alert_tag_next];
    g_alert_tag_next = (g_alert_tag_next + 1) % kAlertTagCap;
    size_t n = business_id.size();
    if (n >= sizeof(slot.id)) {
        n = sizeof(slot.id) - 1; /* 业务 id 上界 40 (§三 与设备 kidpal_alert_t.id 同界) */
    }
    memcpy(slot.id, business_id.data(), n);
    slot.id[n] = '\0';
    slot.rpc_id = rpc_id;
}

int AlertTagLookup(const std::string& business_id) {
    for (const auto& tag : g_alert_tags) {
        if (tag.rpc_id > 0 && business_id == tag.id) {
            return tag.rpc_id;
        }
    }
    return -1;
}

/* 设备当前整分钟数（V18 顶替判定的时间基）。RTC 未同步时 ~0 -> 事件时刻视为未到，
 * 队列只由「弹窗关闭/新提醒到达」驱动（无超时驻留本就是安全侧降级）。 */
long long AlertNowMinutes() {
    return static_cast<long long>(::time(nullptr)) / 60;
}

void AlertDrainTimerCb(lv_timer_t* timer) {
    (void)timer;
    /* 关掉当前弹窗后自动弹下一个（排队逐个弹）+ V18 事件已过时的滞留弹窗顶替。
     * 只做 raise 判定，不含任何弹窗超时（驻留到按键的语义不变）。 */
    McpServer::GetInstance().RaiseReminderQueue();
}

/* 把 kidpal/alert 的五个 string 属性重组成 arguments JSON 串，交回冻结契约解析器
 * kidpal_alert_parse 做**单源**校验：PropertyList 层只保证类型，不保证 id/title 非空
 * 与 fire_at 可解析（畸形载荷必须按 §三 显式降级而非盲投递）。 */
std::string AlertArgsJson(const PropertyList& arguments) {
    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return std::string();
    }
    cJSON_AddItemToObject(root, "id",
        cJSON_CreateString(arguments["id"].value<std::string>().c_str()));
    cJSON_AddItemToObject(root, "fire_at",
        cJSON_CreateString(arguments["fire_at"].value<std::string>().c_str()));
    cJSON_AddItemToObject(root, "title",
        cJSON_CreateString(arguments["title"].value<std::string>().c_str()));
    cJSON_AddItemToObject(root, "speech",
        cJSON_CreateString(arguments["speech"].value<std::string>().c_str()));
    cJSON_AddItemToObject(root, "view",
        cJSON_CreateString(arguments["view"].value<std::string>().c_str()));
    char* dumped = cJSON_PrintUnformatted(root);
    std::string out = (dumped != nullptr) ? std::string(dumped) : std::string();
    if (dumped != nullptr) {
        cJSON_free(dumped);
    }
    cJSON_Delete(root);
    return out;
}

} // namespace

ReturnValue McpServer::HandleAlertCall(const PropertyList& arguments) {
    kidpal_reminder_t* state = kidpal_reminder_default();
    kidpal_alert_t alert;
    cJSON* ack = cJSON_CreateObject();
    std::string args_json = AlertArgsJson(arguments);

    if (kidpal_alert_parse(args_json.c_str(), &alert) != 0) {
        /* 降级：畸形 / 缺 id / 缺 title / fire_at 不可解析 -> 不入队、不盲渲染 (V27) */
        cJSON_AddStringToObject(ack, "state", "rejected");
        cJSON_AddBoolToObject(ack, "degrade", true);
        cJSON_AddBoolToObject(ack, "ack", false);
        ESP_LOGW(TAG, "alert_dispatch corr_id=unparsed state=rejected degrade=true queue=0");
        return ack;
    }

    std::string business_id = alert.id;
    AlertTagRemember(business_id, pending_call_id_);
    int landed = kidpal_reminder_receive(state, &alert);

    /* 本次工具回复只报「已受理」，绝不代表送达（"ack": false）——送达确认走渲染回执 */
    cJSON_AddStringToObject(ack, "state",
        landed == KRM_STATE_QUEUED ? "queued" : (landed == KRM_STATE_RENDERED ? "residency_rearmed" : "deduped"));
    cJSON_AddStringToObject(ack, "reminder_id", business_id.c_str());
    cJSON_AddBoolToObject(ack, "ack", false);
    cJSON_AddNumberToObject(ack, "queue", kidpal_reminder_queue_count(state));
    cJSON_AddNumberToObject(ack, "known", kidpal_reminder_known_count(state));
    ESP_LOGI(TAG, "alert_dispatch corr_id=%s state=%d queue=%d known=%d degrade=false",
             business_id.c_str(), landed, kidpal_reminder_queue_count(state),
             kidpal_reminder_known_count(state));

    if (landed == KRM_STATE_RENDERED) {
        /* 重推命中已渲染驻留弹窗 -> 立即补回回执（不再弹窗、不再入队） */
        EmitRenderReceipt(business_id);
    }

    /* 弹窗真画在 LVGL 主线程下一个轮次（渲染完成才回执；排队时若有弹窗驻留则不叠屏） */
    Application::GetInstance().Schedule([this]() {
        RaiseReminderQueue();
    });
    return ack;
}

/* 提醒材料壳 C 桥（K4 overlay-unify 消费; 实现 home = display/lcd_display.cc, 复用
 * 语音轮四态骨架 + 时间行单源 kidpal_reminder_hhmm。同名缩略不冲突 —— 声明在此以
 * 匹配既有 kidpal_lcd_voice_chrome_front 的「调用点前向声明」模式）。 */
extern "C" int kidpal_lcd_reminder_show(const kidpal_alert_t *a);

void McpServer::RaiseReminderQueue() {
    kidpal_reminder_t* state = kidpal_reminder_default();
    kidpal_alert_t shown;

    /* 录音排队例外门控（RM-A3, requirements §2.2 唯一例外）: 录音中提醒到点不 raise、
     * 队列保持（防提醒 TTS 混进录音内容）；front_layer 取 K1a layer 栈单一权威。
     * 录音会话结束 -> application.cc Idle 路径触发本函数，此刻 recording=0 即恢复弹出。
     * DEFER（录音中）/ INVALID（防御）-> 队列保持不动、不发回执不叠屏。 */
    {
        int rec_now = (g_story_view.page == KSTORY_PAGE_RECORDING) ? 1 : 0;
        int decision = kidpal_reminder_gate(kidpal_render_node_front_layer(), rec_now);

        if (decision != KRM_GATE_RAISE) {
            ESP_LOGI(TAG, "alert_defer corr_id=none recording=%d state=%d", rec_now,
                     decision);
            return;
        }
    }

    int raised = kidpal_reminder_raise_next(state, AlertNowMinutes());
    if (raised <= 0) {
        return; /* 无可弹 / 驻留未渲染或事件时刻未到 -> 不替换、不叠屏 */
    }
    if (kidpal_reminder_shown_alert(state, &shown) != 1) {
        return;
    }
    /* 崩溃修复（包 260829-hardware-validation 语义保留）+ 提醒前台优先级（review F8，
     * 归一屏）：提醒弹窗渲染与故事/课表同族，走 lv_screen_active()+lv_obj_clean()。
     * 语音轮若恰有提醒到点，clean 聊天层视觉控件会销毁 xiaozhi status_label_ →
     * SetStatus 悬空崩溃。夹 begin_reminder/end 令其画到 s_kidpal_scr 并持锁，且把
     * REMINDER 推入前台 layer 栈（REMINDER > VOICE/SERVER，提醒恰在语音轮到点也置前、
     * 不崩——同族崩溃已修症状未修架构，此处修架构）。材料壳 = 语音轮四态骨架复用
     * （K4, RM-E3 反 fork），旧 v1.13 卡面渲染面已退役。
     * 本函数有两个调用上下文（main-task Schedule / LVGL 定时器 AlertDrainTimerCb），
     * esp_lvgl_port 锁为递归互斥量，同任务嵌套 take 安全、give 平衡。 */
    kidpal_render_node_begin_reminder();
    int alert_drawn = kidpal_lcd_reminder_show(&shown);
    kidpal_render_node_end_page();
    if (alert_drawn != 1) {
        /* 渲染未成 -> 不发回执，交由网关 60s 重推收敛（不假装送达） */
        ESP_LOGW(TAG, "alert_render corr_id=%s drawn=false degrade=true", shown.id);
        return;
    }
    /* F1 回执接线（渲染即确认）：画成即 mark_rendered，EmitRenderReceipt 内的
     * may_emit_receipt 才放行（shown.rendered 须 =1）。修复前生产零调用点、
     * shown.rendered 恒 0 -> 回执静默不发、网关 60s 重推 TTS 重播。落位在公共
     * 渲染后段，天然覆盖 raise_next 返回 1（新弹）与 2（滞留顶替）两条路径。 */
    int armed = kidpal_reminder_mark_rendered(state, shown.id);
    if (g_alert_drain_timer == nullptr) {
        /* 首次有提醒到达才建驱动 timer（1s 轮询 raise 判定；无常驻定时器开销） */
        g_alert_drain_timer = lv_timer_create(AlertDrainTimerCb, 1000, nullptr);
    }
    /* 三件套（RM-A1）提示音一半：真起弹窗即出音（OGG_POPUP 音源强定义在本文件, 见
     * kidpal_reminder_play_popup_tone）。语音一半不在设备侧：服务端到点向引擎腿注入
     * kidpal_speak，借引擎既有 edge-tts + opus 下行链出音（裁定二）；设备只画「小伴
     * 在说」形态并出提示音。 */
    if ((kidpal_reminder_render_actions(&shown) & KRM_ACT_TONE) != 0) {
        kidpal_reminder_play_popup_tone();
    }
    ESP_LOGI(TAG, "alert_render corr_id=%s raised=%d actions=%d tone=ogg_popup speak_leg=engine armed=%d",
             shown.id, raised, kidpal_reminder_render_actions(&shown), armed);
    EmitRenderReceipt(std::string(shown.id));
}

void McpServer::EmitRenderReceipt(const std::string& business_id) {
    kidpal_reminder_t* state = kidpal_reminder_default();

    /* 渲染才允许回执的唯一判定门：不开则一条送达确认都不发（RMD-A6） */
    if (kidpal_reminder_may_emit_receipt(state) != 1) {
        return;
    }
    if (kidpal_reminder_mark_acked(state, business_id.c_str()) != 1) {
        return; /* 未渲染不得确认、已确认不得二次确认（一 id 一回执） */
    }

    cJSON* result = cJSON_CreateObject();
    cJSON* content = cJSON_CreateArray();
    cJSON* text = cJSON_CreateObject();
    cJSON_AddStringToObject(text, "type", "text");
    cJSON_AddStringToObject(text, "text", "reminder rendered");
    cJSON_AddItemToArray(content, text);
    cJSON_AddItemToObject(result, "content", content);
    cJSON_AddBoolToObject(result, "isError", false);
    cJSON_AddBoolToObject(result, "ack", true);           /* 送达确认主判据 (relay_ack) */
    cJSON_AddBoolToObject(result, "rendered", true);
    cJSON_AddStringToObject(result, "reminder_id", business_id.c_str());

    char* dumped = cJSON_PrintUnformatted(result);
    int rpc_id = AlertTagLookup(business_id);
    if (rpc_id <= 0) {
        rpc_id = (pending_call_id_ > 0) ? pending_call_id_ : 0; /* 兜底：数字 id 仅传输关联 */
    }
    if (dumped != nullptr) {
        ReplyResult(rpc_id, dumped); /* -> Application::SendMcpMessage，独立于同步自动回复 */
        cJSON_free(dumped);
    }
    cJSON_Delete(result);
    ESP_LOGI(TAG, "alert_receipt corr_id=%s rpc_id=%d ack=true dequeued=true",
             business_id.c_str(), rpc_id);
}


bool McpServer::HandleStoryKey(key_id_t key, key_gesture_t g) {
    /* 页内特例优先消费（spec §4.0「页内特例」列; seam [K1] v1.47 矩阵）：把 KEY/BOOT
     * 手势交进冻结消费面 kidpal_story_page_input，按其返回动作码驱动页态。页不拥有
     * 该手势 (GLOBAL/DEGRADE) -> 返回 false，交回全局路由层（翻页/会话切换语义不变）。
     * 板级以「front == STORY_DELETE 先交本函数、消费即返回」前置拦截（keya_overlay_
     * consume 同款）——删除子态裁决必须吞掉全部手势（NONE 也消费, 绝不漏网误开会话）。 */
    if (g == KEY_GESTURE_NONE) {
        return false;
    }

    /* 删除子态任意手势消费 = idle 重计（seam [K3] 消费侧义务: 每次手势驱动即复位
     * 累计; host 行为齿不可达半, 盲区申报在案）。 */
    if (g_story_view.page == KSTORY_PAGE_DELETE) {
        g_story_delete_idle_ms = 0;
    }

    /* 正文取不回提示态（WF-F2-06, AC7）: KEY／BOOT **任一手势**立即关闭回列表
     * （纯告知、无破坏性动作）——输入函数已把页态切回 LIST 并吞掉手势；此处负责
     * 回列表重渲 + idle 复位（关闭路径绝不落全局 SESSION_TOGGLE / 翻页）。 */
    if (g_story_view.page == KSTORY_PAGE_BODY_UNAVAILABLE) {
        g_story_body_unavailable_idle_ms = 0;
        (void)kidpal_story_page_input(&g_story_view, key, g);
        ESP_LOGI(TAG, "story_body_unavailable close key=%d gesture=%d", (int)key, (int)g);
        ScheduleStoryRender();
        return true;
    }

    /* 300ms 单双击消歧（seam [K2]; 仅 LIST 态挂起, notes 边角②）: 单击先挂起不直发,
     * 双击到达窗内冲销挂起单击; 其余态 gate=0 单击零延迟直发。双击迟到（>死线,
     * 罕见: 板端双击窗 350 > 死线 300 的边缘带）同样冲销挂起并直发双击——挂起位
     * 清除, 绝不双触发。 */
    if (g == KEY_GESTURE_CLICK && kidpal_story_debounce_gate(g_story_view.page) == 1) {
        g_story_debounce_armed = true;
        g_story_debounce_arm_ms = lv_tick_get();
        g_story_debounce_key = key;
        EnsureStoryTickTimer();
        return true; /* 消费（挂起）; 到期由 ~100ms tick 直发 */
    }
    if (g == KEY_GESTURE_DOUBLE_CLICK && g_story_debounce_armed &&
        kidpal_story_debounce_gate(g_story_view.page) == 1) {
        g_story_debounce_armed = false; /* 冲销挂起单击（窗内 -> cancel 判 1; 迟到
                                         * -> 单击已直发/由本处清除, 双击直发） */
    }

    int act = kidpal_story_page_input(&g_story_view, key, g);
    return StoryDispatchAct(act);
}

void McpServer::AddCommonTools() {
    // *Important* To speed up the response time, we add the common tools to the beginning of
    // the tools list to utilize the prompt cache.
    // **重要** 为了提升响应速度，我们把常用的工具放在前面，利用 prompt cache 的特性。

    // Backup the original tools list and restore it after adding the common tools.
    auto original_tools = std::move(tools_);
    auto& board = Board::GetInstance();

    // Do not add custom tools here.
    // Custom tools must be added in the board's InitializeTools function.

    AddTool("self.get_device_status",
        "Provides the real-time information of the device, including the current status of the audio speaker, screen, battery, network, etc.\n"
        "Use this tool for: \n"
        "1. Answering questions about current condition (e.g. what is the current volume of the audio speaker?)\n"
        "2. As the first step to control the device (e.g. turn up / down the volume of the audio speaker, etc.)",
        PropertyList(),
        [&board](const PropertyList& properties) -> ReturnValue {
            return board.GetDeviceStatusJson();
        });

    AddTool("self.audio_speaker.set_volume", 
        "Set the volume of the audio speaker. If the current volume is unknown, you must call `self.get_device_status` tool first and then call this tool.",
        PropertyList({
            Property("volume", kPropertyTypeInteger, 0, 100)
        }), 
        [&board](const PropertyList& properties) -> ReturnValue {
            auto codec = board.GetAudioCodec();
            codec->SetOutputVolume(properties["volume"].value<int>());
            return true;
        });
    
    auto backlight = board.GetBacklight();
    if (backlight) {
        AddTool("self.screen.set_brightness",
            "Set the brightness of the screen.",
            PropertyList({
                Property("brightness", kPropertyTypeInteger, 0, 100)
            }),
            [backlight](const PropertyList& properties) -> ReturnValue {
                uint8_t brightness = static_cast<uint8_t>(properties["brightness"].value<int>());
                backlight->SetBrightness(brightness, true);
                return true;
            });
    }

    // kidpal/display (方向 B, 包 260829-voice-roundtrip): E1 display 消费端。
    // 网关经 mcp JSON-RPC tools/call 下发 (契约 §四 mcp 工具调用 / §1.5-4)，载荷形状
    // {"view":…,"data":{…}}。view 经共享底座 kidpal_display_route 路由到对应渲染面；
    // 白名单外/NULL view -> UNKNOWN 独立降级路径，绝不盲渲染 (V27/§4.0)。工具回调返回
    // cJSON 结果，McpTool::Call 序列化成 JSON-RPC result -> 送达确认 (对齐 relay_ack)。
    AddTool("kidpal/display",
        "Render an E1 display payload issued by the KidPal gateway on this board. "
        "The payload carries a view (char/schedule/story_list/story_text/text/home/"
        "control whitelist) and optional data; the board routes it to the matching "
        "display face, applies control actions, and returns a delivery acknowledgement.",
        PropertyList({
            Property("view", kPropertyTypeString),
            Property("data", kPropertyTypeString, std::string(""))
        }),
        [](const PropertyList& arguments) -> ReturnValue {
            std::string view = arguments["view"].value<std::string>();
            std::string data = arguments["data"].value<std::string>();
            int route = kidpal_display_route(view.c_str());
            auto& app = Application::GetInstance();

            cJSON *ack = cJSON_CreateObject();
            switch (route) {
                case 0: /* VRT_ROUTE_TEXT —— 全文上屏 (滚动/翻屏由显示阅读面承载, 分页
                         逻辑 kidpal_display_text_page_count; S-F4.1 否摘要) */
                    cJSON_AddStringToObject(ack, "route", "text");
                    {
                        std::string text = mcp_json_get_string(data, "text");
                        auto display = Board::GetInstance().GetDisplay();
                        app.Schedule([display, text]() {
                            display->SetChatMessage("assistant", text.c_str());
                        });
                    }
                    break;
                case 1: { /* VRT_ROUTE_HOME —— 回 P1 首页常显 */
                    cJSON_AddStringToObject(ack, "route", "home");
                    /* 包 9b HWV-A6: 渲染节点落地后，home 路由改推首页帧至 RLCD。
                     * 旧行为（SetChatMessage 空串）仅清 xiaozhi 对话区，不显示 KidPal 首页。
                     *
                     * M3-K2 A5 接线（契约 §1.3⑥ additive, S-F1.1 消费半）：data 非空时
                     * 消费 data.upcoming 经 kidpal_home_upcoming_apply 注入 home_p1 冻结
                     * seam（缺/空 data 行为完全不变——既有 show_home 路径零偏移）；注入
                     * 成功且 P1 首页在屏 -> 重渲染首页日程主体（云端一推首页跟着换）；
                     * P1 不在屏 -> seam 已更新、下次首页落屏自然带出（不抢前台）；
                     * apply -1 -> 显式降级（seam 零变化, 绝不盲渲染, V27）。断网无推送
                     * = apply 不被调、日程区不更新（NFR-OFF-1: 设备零轮询）。 */
                    char corr_id[24];
                    snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                             static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
                    std::string home_data = data;
                    app.Schedule([home_data, corr_id]() {
                        if (home_data.empty()) {
                            /* 缺/空 data: 既有 show_home 路径零偏移 */
                            kidpal_render_node_show_home();
                            return;
                        }
                        int rc = kidpal_home_upcoming_apply(home_data.c_str());
                        if (rc != 0) {
                            ESP_LOGW(TAG, "display_dispatch view=home corr_id=%s "
                                          "upcoming_apply degrade rc=%d", corr_id, rc);
                            return; /* 拒收整包不半写: seam 零变化, 不盲渲染、不改前台 */
                        }
                        ESP_LOGI(TAG, "display_dispatch view=home corr_id=%s "
                                      "upcoming_apply ok", corr_id);
                        if (kidpal_render_node_home_on_screen()) {
                            /* 注入成功且 P1 首页在屏 -> 重渲染首页日程主体 */
                            kidpal_render_node_show_home();
                        }
                    });
                    break;
                }
                case 2: { /* VRT_ROUTE_CONTROL —— 契约 §1.4 动作执行 */
                    cJSON_AddStringToObject(ack, "route", "control");
                    std::string action = mcp_json_get_string(data, "action");
                    int a = kidpal_display_control_action(action.c_str());
                    cJSON_AddStringToObject(ack, "action", action.c_str());
                    switch (a) {
                        case 0: /* end_session —— 结束会话回首页 (本地停播主地位不变) */
                            app.AbortSpeaking(kAbortReasonNone);
                            break;
                        case 1: /* stop_speaking —— 云端确认停播补充 */
                            app.AbortSpeaking(kAbortReasonNone);
                            break;
                        case 2: { /* start_recording —— 契约 §1.4: 网关切累积态 -> 设备
                                   * 进 P3 录音态 (manual 聆听)。真实采音殿后 9b (P3-E2);
                                   * 此处消费页态切换 + 驱动录音态演出渲染 (additive)。 */
                            int consumed = kidpal_story_on_control(&g_story_view,
                                                                   VRT_CTRL_START_RECORDING);
                            ESP_LOGI(TAG, "control start_recording story_consumed=%d page=%d",
                                     consumed, g_story_view.page);
                            /* [G6] 双入口同一记录语境（261019, AC1）: 触发词入口经
                             * control/start_recording 收敛——置记录语境标志（与 REC_SESSION
                             * 入口同态: 回显旁路喂入 / kvr 场景守卫 / 聆听形态切换的消费门;
                             * 清位 = 会话收尾, 既有）。幂等重入（consumed 幂等返回）不重复置。 */
                            kidpal_story_session_ctx_set(1);
                            app.StartListening();
                            if (consumed == 1) {
                                /* 进录音态 -> 首帧渲染 + 起演出 tick 驱动（显示循环侧真实
                                 * 消费 kidpal_story_rec_tick, 录音「正在录」可观察; HWV-A9）。 */
                                ScheduleStoryRender();
                                StartStoryRecTimer();
                            }
                            break;
                        }
                        case 3: { /* start_organizing（261019-p3-record-dialogue-guide
                                   * [G2]/[G6], q2 = ⓒ）: 三式收尾统一下发 -> 设备切
                                   * P3 整理过渡演出（KSTORY_PAGE_ORGANIZING, 非交互
                                   * 过渡态 KEY/BOOT 均不响应; 离态 = story_text 到达
                                   * 既有渲染路径）。重复幂等由 on_control case 3 承载。 */
                            int consumed = kidpal_story_on_control(&g_story_view,
                                                                   VRT_CTRL_START_ORGANIZING);
                            ESP_LOGI(TAG, "control start_organizing story_consumed=%d page=%d",
                                     consumed, g_story_view.page);
                            if (consumed == 1) {
                                /* 进整理过渡态 -> 首帧渲染（state-organizing 帧可见,
                                 * 翻✅条件③的设备半; 渲染分支 = story_page.c [G5]）。 */
                                ScheduleStoryRender();
                            }
                            break;
                        }
                        default: /* 未知 action -> 降级, 不执行 */
                            cJSON_AddBoolToObject(ack, "degrade", true);
                            break;
                    }
                    break;
                }
                case 4: { /* VRT_ROUTE_SCHEDULE —— P2 一周总览网格页 (契约 §1.3② 周网格
                           * data 2026-09-21 契约门定稿, M3-K2 整页重写; 旧列表形状
                           * {title,items} 已废止)。解析 data {"week":{"courses":[…]}}：
                           * 成功 -> kidpal_schedule_grid_render 渲染面真画（7 列×3 行
                           * 网格 + 今天列高亮 + ※/星标）并填充周网格缓存（BOOT 翻页
                           * cycle_next 浏览源）；解析失败 -> 显式降级（友好提示, 绝不
                           * 盲渲染, V27 / spec §4.0）。 */
                    cJSON_AddStringToObject(ack, "route", "schedule");
                    /* 契约 §1.3② data.restore additive（L85）——恢复性载荷标记：true =
                     * 只填缓存不接管前台（重推周网格恢复冷启动, ADR-005）；缺省/显式
                     * false = 现行为（接管前台展示）。 */
                    bool restore = mcp_json_get_bool(data, "restore");
                    char corr_id[24];
                    snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                             static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
                    /* courses[84] = 84 × 60B = ~5KB → 上堆，理由见本文件头「ssl_receive
                     * 栈纪律」。 */
                    auto courses =
                        std::make_shared<std::array<kidpal_schedule_course_t, KSG_COURSE_CAP>>();
                    int n = kidpal_schedule_grid_parse(data.c_str(), courses->data(),
                                                       KSG_COURSE_CAP);
                    if (n < 0) {
                        /* 降级：schedule data 畸形/缺字段/旧形状/越界 cap -> 友好提示,
                         * 不盲渲染（V27）。restore 载荷解析失败 = **静默**记日志、不弹
                         * 提示（ADR-005 板端静默降级, 区别于主动展示路径的弹提示降级——
                         * 不为恢复性后台载荷打扰前台）。 */
                        cJSON_AddBoolToObject(ack, "degrade", true);
                        ESP_LOGW(TAG, "display_dispatch view=schedule corr_id=%s courses=0 degrade=true",
                                 corr_id);
                        if (!restore) {
                            auto display = Board::GetInstance().GetDisplay();
                            app.Schedule([display]() {
                                display->SetChatMessage("system", "课表数据有点问题，稍后再试试");
                            });
                        }
                        break;
                    }
                    cJSON_AddNumberToObject(ack, "courses", n);
                    ESP_LOGI(TAG, "display_dispatch view=schedule corr_id=%s courses=%d degrade=false",
                             corr_id, n);
                    app.Schedule([courses, n, corr_id, restore]() {
                        if (restore) {
                            /* restore 恢复性分支：只填缓存、不接管前台。无 begin_page 隐式
                             * 持锁（begin_page 才推 SERVER_PAGE 接管）, 故**显式持 LVGL 锁**
                             * 写缓存/重绘。 */
                            lvgl_port_lock(0);
                        } else {
                            kidpal_render_node_begin_page();
                        }
                        if (restore) {
                            /* P2 课表页在屏才重绘本页；否则仅填缓存（ADR-005 R10：接管态、
                             * 他页前台只填缓存不重绘）。 */
                            if (kidpal_render_node_schedule_on_screen()) {
                                kidpal_schedule_grid_render(
                                    courses->data(), n, kidpal_render_node_today_col());
                            }
                        } else {
                            int drawn = kidpal_schedule_grid_render(
                                courses->data(), n, kidpal_render_node_today_col());
                            ESP_LOGI(TAG, "schedule_grid_render corr_id=%s courses=%d drawn=%d",
                                     corr_id, n, drawn);
                        }
                        /* M3-K2: 缓存周网格至渲染节点，供 BOOT 双按翻页 cycle_next 读取
                         * （cache_schedule 网格化）。restore 与薄载荷路径共用**唯一**缓存写
                         * 点——不给恢复性载荷另起第二条缓存路径（frozen K7-A5 tooth: 路由区
                         * 缓存调用恰一次）。 */
                        kidpal_render_node_cache_schedule_grid(courses->data(), n);
                        if (restore) {
                            lvgl_port_unlock();
                            ESP_LOGI(TAG, "schedule_grid_restore corr_id=%s courses=%d cached=%d",
                                     corr_id, n, n);
                        } else {
                            kidpal_render_node_end_page();
                        }
                    });
                    break;
                }
                case 3: { /* VRT_ROUTE_CHAR —— 查字窗 (包 260829-char-stroke, 本包
                             additive 补 case 3, 只消费不改 view_router 数值码).
                             2026-09-17 M2-K2 撤 P4 常驻页：查字不再是 BOOT 翻页可停靠
                             页位（页循环 4→3），但本路由与渲染调用点整体保留——撤销的
                             是常驻页位、不是查字能力。
                             2026-09-19 M2-K3 页→窗落地：查字渲染路径改经
                             begin/end_char_window（CHAR_WINDOW 层）叠加**会话内覆盖窗**，
                             关窗走 dismiss_front_overlay 回原页、不翻页。 */
                    cJSON_AddStringToObject(ack, "route", "char");
                    std::string char_data = data;
                    /* 缺字段防御降级门 (闸 A 包内裁定): 必需键六件不全 -> 降级提示、
                     * 不盲渲染、不崩 (对齐 view_router UNKNOWN 语义)。 */
                    if (kidpal_char_data_validate(char_data.c_str()) != 0) {
                        cJSON_AddBoolToObject(ack, "degrade", true);
                        cJSON_AddStringToObject(ack, "degrade_reason",
                            "char data missing required keys (char/pinyin/radical/"
                            "strokeCount/strokes/medians)");
                        break;
                    }
                    /* 通过校验 -> 喂数据渲染查字窗 (LVGL 主线程延迟执行; M2-K3 页→窗改造
                     * 后作为**会话内覆盖窗**——覆盖当前页之上、非常驻页, 服务端下发的
                     * 查字渲染通路保留)。 */
                    app.Schedule([char_data]() {
                        /* 崩溃修复（包 260829-hardware-validation 语义保留 / 归一屏）：
                         * 查字窗已并入 s_kidpal_scr 体系——kidpal_char_page_show 在活动屏
                         * 当前页之上叠一层窗面板（不清屏、不自建独立屏），不触碰聊天层
                         * 视觉控件，天然免疫 SetStatus 悬空崩溃。
                         * 仍夹 begin/end：begin_char_window 把 CHAR_WINDOW 推入 layer 栈
                         * + 持 LVGL 锁消除与 LVGL 任务竞态 + 切活动屏到 s_kidpal_scr，随后
                         * char_page_show 画在 s_kidpal_scr 上（覆盖窗 = 当前页之上的净显示，
                         * 关窗回原页不走翻页）。**不起会话 / 聆听**——播完会话即结束、不回
                         * 聆听态（§2.2 行 68 / F3 窗口交互包 v1.37）。 */
                        kidpal_render_node_begin_char_window();
                        kidpal_char_page_show(char_data.c_str());
                        kidpal_render_node_end_char_window();
                    });
                    break;
                }
                case VRT_ROUTE_STORY_LIST: { /* VRT_ROUTE_STORY_LIST —— P3 故事列表页
                    * (契约 §1.3 ③, 包 260829-story-p3)。解析 data {"total","items":[…]}：
                    * 成功 -> view_init + kidpal_story_page_render 渲染面真画 (标题/×N 计数/
                    * 行列表/反白光标)；解析失败 (-1) -> 显式降级 (友好提示, 绝不盲渲染,
                    * V27 / spec §4.0，与 schedule 同纪律)。 */
                    cJSON_AddStringToObject(ack, "route", "story_list");
                    char corr_id[24];
                    snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                             static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
                    /* kidpal_story_list_t = 2824B → 堆分配（ssl_receive 4096B 栈纪律，
                     * 见本文件头注）。make_unique<T>() 值初始化 = 旧写法的 `= {}` 清零。 */
                    auto parsed = std::make_unique<kidpal_story_list_t>();
                    int items_n = kidpal_story_list_parse(data.c_str(), parsed.get());
                    if (items_n < 0) {
                        /* 降级：story_list data 畸形/缺字段/越界 -> 友好提示, 不盲渲染 */
                        cJSON_AddBoolToObject(ack, "degrade", true);
                        ESP_LOGW(TAG, "display_dispatch view=story_list corr_id=%s items=0 degrade=true", corr_id);
                        auto display = Board::GetInstance().GetDisplay();
                        app.Schedule([display]() {
                            display->SetChatMessage("system", "故事列表有点问题，稍后再试试");
                        });
                        break;
                    }
                    {
                        /* story_list 重推对账冲销（seam [K5]/[K4]; 契约 §四 Q5）:
                         * pending 待账 id 不在重推 items 窗口 -> 冲销（删除已由 front
                         * 执行, 隐式 ack 成立, 重发停）; 仍在窗口 -> 不冲销（重发
                         * 继续, 上限 3 次后静默停发）。冲销后计数由 view_init 以
                         * list->total 重装校准——counter 恒信 total（板端零本地推）。 */
                        if (g_story_pending_delete_id[0] != '\0' &&
                            kidpal_story_delete_reconcile_hit(
                                g_story_pending_delete_id, parsed.get()) == 0) {
                            ESP_LOGI(TAG, "story_delete_reconcile corr_id=%s id=%s cleared=true",
                                     corr_id, g_story_pending_delete_id);
                            g_story_pending_delete_id[0] = '\0';
                            g_story_pending_tries = 0;
                        }
                    }
                    {
                        bool was_delete = (g_story_view.page == KSTORY_PAGE_DELETE);
                        kidpal_story_view_init(&g_story_view, parsed.get());
                        g_story_list = *parsed;
                        if (was_delete) {
                            /* 云端重推到达时删除覆盖层仍在前台 -> view_init 已把页态
                             * 重装回 LIST; 覆盖层随之关层（回列表, 不留幽灵弹窗）。 */
                            app.Schedule([]() {
                                kidpal_render_node_dismiss_front_overlay();
                            });
                        }
                    }
                    /* AC2「进入本列表时已先行为当屏各篇取回本地缺失的正文」: 列表装载
                     * 后重算当屏缺失批次（卡不在位 / 非列表态内部 no-op）。 */
                    StoryPrefetchRefresh();
                    cJSON_AddNumberToObject(ack, "items", items_n);
                    cJSON_AddNumberToObject(ack, "counter", kidpal_story_counter(parsed.get()));
                    ESP_LOGI(TAG, "story_list_dispatch corr_id=%s items=%d counter=%d degrade=false",
                             corr_id, items_n, kidpal_story_counter(parsed.get()));
                    {
                        /* K3 [S4]: story_list 推送 gating（defer 包 260928-story-connect-
                         * push-defer ①恢复重裁板端消费半, plan T5）——记录语境前台
                         * （引导态/录音态/问答会话, 或视图已入录音态）只装缓存绝不推前台
                         * （防 story_list 帧盖掉引导/录音演出, defer 真机缺陷形态的板端
                         * 防护半）; 其余页态照旧推前台（K2 消费面零偏移红线: LIST 冷浏览/
                         * READER/STAMP/DELETE 渲染不变）。
                         *
                         * [SL1] restore 分流（261010-p3-storylist-cold-start-restore;
                         * 复刻同文件 schedule restore 分支 :926-991 在役形态）：恢复性载荷
                         * （WS 建连恢复推送, 契约 §1.3③ D1）-> 只填 RAM 列表缓存、不接管
                         * 前台（P3 在屏时仅重绘本页）; 缺省/显式 false -> 现行为零偏移。
                         * 布尔解析复用 :444-446 在世 helper（absent/非 true 字面同判
                         * false = 契约缺省语义）。 */
                        int story_voice_front =
                            (kidpal_render_node_front_layer() == KIDPAL_LAYER_VOICE) ? 1 : 0;
                        bool story_restore = mcp_json_get_bool(data, "restore");
                        if (story_restore ||
                            kidpal_story_list_gate(story_voice_front,
                                                   g_story_view.page) != 0) {
                            /* restore 或记录语境前台 gated: 只装缓存（唯一缓存同步缝,
                             * 不给恢复性载荷另起第二条缓存路径——K7-A5「唯一缓存写点」
                             * 纪律）、零前台夹钳（不接管前台、不推 SERVER_PAGE——260928
                             * 摘除理由的板端消解路径）。restore 且 P3 在屏（LIST 冷浏览）
                             * -> 经重绘本页单入口仅重绘（自持锁、先缓存后调——.h :277
                             * 在世契约）。 */
                            kidpal_render_node_cache_story(&g_story_view, &g_story_list,
                                                           &g_story_text);
                            if (story_restore && kidpal_render_node_story_on_screen()) {
                                kidpal_render_node_refresh_story();
                            }
                            ESP_LOGI(TAG, "story_list_gate defer_front page=%d voice_front=%d restore=%d",
                                     g_story_view.page, story_voice_front, story_restore ? 1 : 0);
                        } else {
                            kidpal_story_view_t view_snapshot = g_story_view;   /* 108B，留栈 */
                            /* 列表快照 2824B 上堆后按 shared_ptr 捕获（闭包可拷贝）。 */
                            auto list_snapshot = std::make_shared<kidpal_story_list_t>(g_story_list);
                            app.Schedule([view_snapshot, list_snapshot]() {
                                kidpal_render_node_begin_page();
                                kidpal_story_page_render(&view_snapshot, list_snapshot.get(),
                                                         &g_story_text);
                                kidpal_render_node_end_page();
                            });
                        }
                    }
                    break;
                }
                case VRT_ROUTE_STORY_TEXT: { /* VRT_ROUTE_STORY_TEXT —— P3 回看/回听阅读态
                    * (契约 §1.3 ④, spec S-F2.3)。解析 data {"id","title","text"} ->
                    * open_reader 分页 (全文不丢) + kidpal_story_page_render 阅读面真画;
                    * 生字反白高亮已裁不消费 (P3-E4)。解析失败 -> 显式降级, 绝不盲渲染。 */
                    cJSON_AddStringToObject(ack, "route", "story_text");
                    char corr_id[24];
                    snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                             static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
                    /* kidpal_story_text_t = 2208B → 堆分配：本路由正是真机语音轮 2 栈
                     * 溢发的现场（ssl_receive 4096B 栈纪律，见本文件头注）。 */
                    auto parsed = std::make_unique<kidpal_story_text_t>();
                    if (kidpal_story_text_parse(data.c_str(), parsed.get()) != 0) {
                        cJSON_AddBoolToObject(ack, "degrade", true);
                        ESP_LOGW(TAG, "display_dispatch view=story_text corr_id=%s degrade=true", corr_id);
                        auto display = Board::GetInstance().GetDisplay();
                        app.Schedule([display]() {
                            display->SetChatMessage("system", "这个故事暂时打不开，稍后再试试");
                        });
                        break;
                    }
                    g_story_text = *parsed;
                    {
                        /* P3 正文本地留存（决策 24 窄例外; SC-A2/A8）: 写
                         * /sdcard/story/<id>.txt，**写卡成功才登记 RAM 索引**（commit
                         * 承载）；卡不在位 / 坏卡 -> 整层失效（不落盘、不崩溃），正文
                         * 照常 RAM 显示；写失败只清该篇、不整层清空（commit 内承载）。
                         * id 字符集校验在 commit + story_sd_io 双门（[C9] 路径注入面）。 */
                        StoryCacheEnsureLoaded();
                        int sd_mounted = Board::GetInstance().SdCardMounted() ? 1 : 0;
                        int write_rc = kidpal_story_sd_write(parsed->id, parsed->text,
                                                             sd_mounted);
                        (void)kidpal_story_cache_commit(&g_story_cache, parsed->id,
                                                        sd_mounted, write_rc == 1 ? 1 : 0);
                    }
                    {
                        /* 「上传成功那一瞬 -> 盖章帧」生产触发 (S-F2.2/S-F2.4, HWV-A9)：
                         * 孩子停录时页内输入 (kidpal_story_page_input STOP_RECORD) 把
                         * archive_pend 置位；网关侧停录归档成功后把整段正文以
                         * story_text(record) 组块回传 (本包 front_app .stop -> story_text
                         * 云侧接线)，设备在此收到这条「刚入册」正文即盖章。仅在停录待确认
                         * (archive_pend>0) 且 id 与最近一次盖章不同 (幂等，防 mcp 回执重试
                         * 重复发徽章) 时触发；否则 (回看既有故事) 照常进阅读态。
                         * 侧效应守卫 = 纯逻辑 [C8] kidpal_story_stamp_should_fire:
                         * **fetch 应答恒不盖章**（契约 §1.3④; from_fetch 判据 = 本帧 id
                         * 命中已发 kidpal_story_fetch 记账环）；fetch 路径也不置
                         * archive_pend、不调 kidpal_story_on_saved。 */
                        int from_fetch = StoryConsumeFetchIssued(parsed->id);
                        int awaiting = (from_fetch != 0 && g_story_fetch_pending &&
                                        std::strcmp(g_story_fetch_pending_id,
                                                    parsed->id) == 0) ? 1 : 0;
                        int fresh_save = kidpal_story_stamp_should_fire(
                            g_story_view.archive_pend, parsed->id,
                            g_story_view.last_saved_id, from_fetch);
                        if (fresh_save) {
                            /* 盖章 N = 入册流水号 seq（seam [K6]/[K7]; 契约 §1.3④ 唯一
                             * 权威源, 账本 owner = K1; 旧「counter+1 本地推」D3 与
                             * 易失本地盖章流水号退役）。seq 缺省/畸形 -> 探针回 ABSENT,
                             * on_saved 降级演出不带 N（契约 Q4 板端自定）。 */
                            int stamp_seq = kidpal_story_text_seq(data.c_str());
                            parsed->seq = stamp_seq;
                            if (kidpal_story_on_saved(&g_story_view, parsed->id, stamp_seq) == 0) {
                                if (g_story_view.archive_pend > 0) {
                                    g_story_view.archive_pend--; /* 消费一次待归档请求 */
                                }
                                cJSON_AddStringToObject(ack, "stamp", parsed->id);
                                ESP_LOGI(TAG, "story_saved_stamp corr_id=%s id=%s seq=%d",
                                         corr_id, parsed->id, stamp_seq);
                                kidpal_story_view_t stamp_view = g_story_view; /* 108B，留栈 */
                                /* Schedule 闭包需可拷贝 -> unique_ptr 转 shared_ptr 捕获；
                                 * 列表快照同样上堆（2824B 不再落栈）。 */
                                std::shared_ptr<kidpal_story_text_t> stamp_text = std::move(parsed);
                                auto stamp_list = std::make_shared<kidpal_story_list_t>(g_story_list);
                                app.Schedule([stamp_view, stamp_list, stamp_text]() {
                                    kidpal_render_node_begin_page();
                                    kidpal_story_page_render(&stamp_view, stamp_list.get(),
                                                             stamp_text.get());
                                    kidpal_render_node_end_page();
                                });
                            }
                            break;
                        }
                        /* kidpal_story_fetch 应答消费（SC-A4/A5/A6/A8; 契约 §1.3④）:
                         *   在等该篇（双击兜底）-> id 校验通过后进阅读态; id 错配 ->
                         *     绝不显示别篇, 走 WF-F2-06 可见降级;
                         *   后台预取应答 -> 只装缓存（写卡 + 索引已在上面完成），
                         *     **不动前台**（[C7] fetch_answer_enter 恒 0）。
                         * 应答收口同时清预取在途标记 / 队列中的同 id（逐篇串行放行）。 */
                        if (from_fetch != 0) {
                            g_story_prefetch_in_flight = 0;
                            g_story_prefetch_inflight_id[0] = '\0';
                            StoryPrefetchDrop(parsed->id);
                            if (kidpal_story_fetch_answer_enter(awaiting) != 1) {
                                /* 后台预取应答：只装缓存（写卡 + 索引已完成），
                                 * **不动前台**——绝不抢当前演出。 */
                                ESP_LOGI(TAG, "story_fetch_prefetch_cached corr_id=%s id=%s",
                                         corr_id, parsed->id);
                                break;
                            }
                            g_story_fetch_pending = false;
                            g_story_fetch_pending_id[0] = '\0';
                            if (kidpal_story_reader_id_match(parsed.get(), &g_story_list,
                                                             g_story_view.cursor) != 1) {
                                /* SC-A6 / AC7: 被打开正文 id != 光标所选行 id ——
                                 * 拒绝并走降级（显示错内容比无反应更危险）。 */
                                ESP_LOGW(TAG, "story_fetch_answer_mismatch corr_id=%s id=%s "
                                              "cursor=%d -> degrade",
                                         corr_id, parsed->id, g_story_view.cursor);
                                EnterStoryBodyUnavailable();
                                break;
                            }
                            /* id 校验通过 -> 落到下方**既有阅读态渲染单点**（复用同一
                             * begin/end_page 调用点；不新增前台接管调用——K7-A5 文件级
                             * begin_page 计数恒 5 的冻结守卫）。 */
                            ESP_LOGI(TAG, "story_fetch_answer_enter corr_id=%s id=%s",
                                     corr_id, parsed->id);
                        }
                        /* 阅读态分页行高单源 (KSTORY_READER_LINES_PER_SCREEN, 包
                         * 260911-render-pages RP-A2; 替代原硬编码 4 的最小估算)。
                         * counter/列表态字段从当前视图快照续用。 */
                        kidpal_story_view_t reader_view = g_story_view;
                        (void)kidpal_story_open_reader(&reader_view, parsed.get(),
                                                       KSTORY_READER_LINES_PER_SCREEN);
                        cJSON_AddNumberToObject(ack, "screens", reader_view.screens);
                        ESP_LOGI(TAG, "story_text_dispatch corr_id=%s screens=%d degrade=false",
                                 corr_id, reader_view.screens);
                        std::shared_ptr<kidpal_story_text_t> reader_text = std::move(parsed);
                        auto list_snapshot = std::make_shared<kidpal_story_list_t>(g_story_list);
                        app.Schedule([reader_view, list_snapshot, reader_text]() {
                            kidpal_render_node_begin_page();
                            kidpal_story_page_render(&reader_view, list_snapshot.get(),
                                                     reader_text.get());
                            kidpal_render_node_end_page();
                        });
                    }
                    break;
                }
                default: /* VRT_ROUTE_UNKNOWN —— 白名单外 view 的独立降级路径 */
                    cJSON_AddStringToObject(ack, "route", "unknown");
                    cJSON_AddBoolToObject(ack, "degrade", true);
                    cJSON_AddStringToObject(ack, "unknown_view", view.c_str());
                    break;
            }
            return ack; /* cJSON* -> McpTool::Call 序列化为 result(送达确认) 回执 */
        });

    /* kidpal/alert (契约 §三 提醒推送 / §四 mcp 工具调用, 决策 23·27, 包
     * 260829-schedule-reminder): 网关 push helper 到点推送一条提醒档位。载荷
     * {id, fire_at, title, speech, view}；设备 mcp Property 仅 bool/int/string 三型
     * (mcp_server.h)，故 §三 嵌套的 view 显示块以 **JSON 字符串** 承载 —— 与
     * kidpal/display 的 data 字符串化同一纪律。
     * 送达语义（RMD-A6 四态解耦）：本回调只「解析 + 入易失 known-id 集 + 排队」并回
     * accepted（"ack": false）；渲染完成的回执由 HandleAlertCall/RaiseReminderQueue 经
     * McpServer::ReplyResult 独立延后发出（"ack": true = relay_ack 的送达确认）。
     * 畸形载荷 = 显式降级、不入队、不盲渲染 (V27)。 */
    AddTool("kidpal/alert",
        "Deliver one KidPal schedule reminder slot (three-ladder alert) to this board. "
        "The board dedupes by the business id, queues it, raises a resident state-reminder "
        "popup (prompt tone + big item text + time line) and answers with a render receipt "
        "that dequeues the slot; the gateway retries to that receipt and silently voids expiry.",
        PropertyList({
            Property("id", kPropertyTypeString),
            Property("fire_at", kPropertyTypeString),
            Property("title", kPropertyTypeString),
            Property("speech", kPropertyTypeString, std::string("")),
            Property("view", kPropertyTypeString, std::string(""))
        }),
        [this](const PropertyList& arguments) -> ReturnValue {
            return HandleAlertCall(arguments);
        });

#ifdef HAVE_LVGL
    auto display = board.GetDisplay();
    if (display && display->GetTheme() != nullptr) {
        AddTool("self.screen.set_theme",
            "Set the theme of the screen. The theme can be `light` or `dark`.",
            PropertyList({
                Property("theme", kPropertyTypeString)
            }),
            [display](const PropertyList& properties) -> ReturnValue {
                auto theme_name = properties["theme"].value<std::string>();
                auto& theme_manager = LvglThemeManager::GetInstance();
                auto theme = theme_manager.GetTheme(theme_name);
                if (theme != nullptr) {
                    display->SetTheme(theme);
                    return true;
                }
                return false;
            });
    }

    auto camera = board.GetCamera();
    if (camera) {
        AddTool("self.camera.take_photo",
            "Take a photo and explain it. Use this tool after the user asks you to see something.\n"
            "Args:\n"
            "  `question`: The question that you want to ask about the photo.\n"
            "Return:\n"
            "  A JSON object that provides the photo information.",
            PropertyList({
                Property("question", kPropertyTypeString)
            }),
            [camera](const PropertyList& properties) -> ReturnValue {
                // Lower the priority to do the camera capture
                TaskPriorityReset priority_reset(1);

                if (!camera->Capture()) {
                    throw std::runtime_error("Failed to capture photo");
                }
                auto question = properties["question"].value<std::string>();
                return camera->Explain(question);
            });
    }
#endif

    // Restore the original tools list to the end of the tools list
    tools_.insert(tools_.end(), original_tools.begin(), original_tools.end());
}

void McpServer::AddUserOnlyTools() {
    // System tools
    AddUserOnlyTool("self.get_system_info",
        "Get the system information",
        PropertyList(),
        [this](const PropertyList& properties) -> ReturnValue {
            auto& board = Board::GetInstance();
            return board.GetSystemInfoJson();
        });

    AddUserOnlyTool("self.reboot", "Reboot the system",
        PropertyList(),
        [this](const PropertyList& properties) -> ReturnValue {
            auto& app = Application::GetInstance();
            app.Schedule([&app]() {
                ESP_LOGW(TAG, "User requested reboot");
                vTaskDelay(pdMS_TO_TICKS(1000));

                app.Reboot();
            });
            return true;
        });

    // Firmware upgrade
    AddUserOnlyTool("self.upgrade_firmware", "Upgrade firmware from a specific URL. This will download and install the firmware, then reboot the device.",
        PropertyList({
            Property("url", kPropertyTypeString, "The URL of the firmware binary file to download and install")
        }),
        [this](const PropertyList& properties) -> ReturnValue {
            auto url = properties["url"].value<std::string>();
            ESP_LOGI(TAG, "User requested firmware upgrade from URL: %s", url.c_str());
            
            auto& app = Application::GetInstance();
            app.Schedule([url, &app]() {
                bool success = app.UpgradeFirmware(url);
                if (!success) {
                    ESP_LOGE(TAG, "Firmware upgrade failed");
                }
            });
            
            return true;
        });

    // Display control
#ifdef HAVE_LVGL
    auto display = dynamic_cast<LvglDisplay*>(Board::GetInstance().GetDisplay());
    if (display) {
        AddUserOnlyTool("self.screen.get_info", "Information about the screen, including width, height, etc.",
            PropertyList(),
            [display](const PropertyList& properties) -> ReturnValue {
                cJSON *json = cJSON_CreateObject();
                cJSON_AddNumberToObject(json, "width", display->width());
                cJSON_AddNumberToObject(json, "height", display->height());
                if (dynamic_cast<OledDisplay*>(display)) {
                    cJSON_AddBoolToObject(json, "monochrome", true);
                } else {
                    cJSON_AddBoolToObject(json, "monochrome", false);
                }
                return json;
            });

#if CONFIG_LV_USE_SNAPSHOT
        AddUserOnlyTool("self.screen.snapshot", "Snapshot the screen and upload it to a specific URL",
            PropertyList({
                Property("url", kPropertyTypeString),
                Property("quality", kPropertyTypeInteger, 80, 1, 100)
            }),
            [display](const PropertyList& properties) -> ReturnValue {
                auto url = properties["url"].value<std::string>();
                auto quality = properties["quality"].value<int>();

                std::string jpeg_data;
                if (!display->SnapshotToJpeg(jpeg_data, quality)) {
                    throw std::runtime_error("Failed to snapshot screen");
                }

                ESP_LOGI(TAG, "Upload snapshot %u bytes to %s", jpeg_data.size(), url.c_str());
                
                // 构造multipart/form-data请求体
                std::string boundary = "----ESP32_SCREEN_SNAPSHOT_BOUNDARY";
                
                auto http = Board::GetInstance().GetNetwork()->CreateHttp(3);
                http->SetHeader("Content-Type", "multipart/form-data; boundary=" + boundary);
                if (!http->Open("POST", url)) {
                    throw std::runtime_error("Failed to open URL: " + url);
                }
                {
                    // 文件字段头部
                    std::string file_header;
                    file_header += "--" + boundary + "\r\n";
                    file_header += "Content-Disposition: form-data; name=\"file\"; filename=\"screenshot.jpg\"\r\n";
                    file_header += "Content-Type: image/jpeg\r\n";
                    file_header += "\r\n";
                    http->Write(file_header.c_str(), file_header.size());
                }

                // JPEG数据
                http->Write((const char*)jpeg_data.data(), jpeg_data.size());

                {
                    // multipart尾部
                    std::string multipart_footer;
                    multipart_footer += "\r\n--" + boundary + "--\r\n";
                    http->Write(multipart_footer.c_str(), multipart_footer.size());
                }
                http->Write("", 0);

                if (http->GetStatusCode() != 200) {
                    throw std::runtime_error("Unexpected status code: " + std::to_string(http->GetStatusCode()));
                }
                std::string result = http->ReadAll();
                http->Close();
                ESP_LOGI(TAG, "Snapshot screen result: %s", result.c_str());
                return true;
            });
        
        AddUserOnlyTool("self.screen.preview_image", "Preview an image on the screen",
            PropertyList({
                Property("url", kPropertyTypeString)
            }),
            [display](const PropertyList& properties) -> ReturnValue {
                auto url = properties["url"].value<std::string>();
                auto http = Board::GetInstance().GetNetwork()->CreateHttp(3);

                if (!http->Open("GET", url)) {
                    throw std::runtime_error("Failed to open URL: " + url);
                }
                int status_code = http->GetStatusCode();
                if (status_code != 200) {
                    throw std::runtime_error("Unexpected status code: " + std::to_string(status_code));
                }

                size_t content_length = http->GetBodyLength();
                char* data = (char*)heap_caps_malloc(content_length, MALLOC_CAP_8BIT);
                if (data == nullptr) {
                    throw std::runtime_error("Failed to allocate memory for image: " + url);
                }
                size_t total_read = 0;
                while (total_read < content_length) {
                    int ret = http->Read(data + total_read, content_length - total_read);
                    if (ret < 0) {
                        heap_caps_free(data);
                        throw std::runtime_error("Failed to download image: " + url);
                    }
                    if (ret == 0) {
                        break;
                    }
                    total_read += ret;
                }
                http->Close();

                auto image = std::make_unique<LvglAllocatedImage>(data, content_length);
                display->SetPreviewImage(std::move(image));
                return true;
            });
#endif // CONFIG_LV_USE_SNAPSHOT
    }
#endif // HAVE_LVGL

    // Assets download url
    auto& assets = Assets::GetInstance();
    if (assets.partition_valid()) {
        AddUserOnlyTool("self.assets.set_download_url", "Set the download url for the assets",
            PropertyList({
                Property("url", kPropertyTypeString)
            }),
            [](const PropertyList& properties) -> ReturnValue {
                auto url = properties["url"].value<std::string>();
                Settings settings("assets", true);
                settings.SetString("download_url", url);
                return true;
            });
    }
}

void McpServer::AddTool(McpTool* tool) {
    // Prevent adding duplicate tools
    if (std::find_if(tools_.begin(), tools_.end(), [tool](const McpTool* t) { return t->name() == tool->name(); }) != tools_.end()) {
        ESP_LOGW(TAG, "Tool %s already added", tool->name().c_str());
        return;
    }

    ESP_LOGI(TAG, "Add tool: %s%s", tool->name().c_str(), tool->user_only() ? " [user]" : "");
    tools_.push_back(tool);
}

void McpServer::AddTool(const std::string& name, const std::string& description, const PropertyList& properties, std::function<ReturnValue(const PropertyList&)> callback) {
    AddTool(new McpTool(name, description, properties, callback));
}

void McpServer::AddUserOnlyTool(const std::string& name, const std::string& description, const PropertyList& properties, std::function<ReturnValue(const PropertyList&)> callback) {
    auto tool = new McpTool(name, description, properties, callback);
    tool->set_user_only(true);
    AddTool(tool);
}

void McpServer::ParseMessage(const std::string& message) {
    cJSON* json = cJSON_Parse(message.c_str());
    if (json == nullptr) {
        ESP_LOGE(TAG, "Failed to parse MCP message: %s", message.c_str());
        return;
    }
    ParseMessage(json);
    cJSON_Delete(json);
}

void McpServer::ParseCapabilities(const cJSON* capabilities) {
    auto vision = cJSON_GetObjectItem(capabilities, "vision");
    if (cJSON_IsObject(vision)) {
        auto url = cJSON_GetObjectItem(vision, "url");
        auto token = cJSON_GetObjectItem(vision, "token");
        if (cJSON_IsString(url)) {
            auto camera = Board::GetInstance().GetCamera();
            if (camera) {
                std::string url_str = std::string(url->valuestring);
                std::string token_str;
                if (cJSON_IsString(token)) {
                    token_str = std::string(token->valuestring);
                }
                camera->SetExplainUrl(url_str, token_str);
            }
        }
    }
}

void McpServer::ParseMessage(const cJSON* json) {
    // Check JSONRPC version
    auto version = cJSON_GetObjectItem(json, "jsonrpc");
    if (version == nullptr || !cJSON_IsString(version) || strcmp(version->valuestring, "2.0") != 0) {
        ESP_LOGE(TAG, "Invalid JSONRPC version: %s", version ? version->valuestring : "null");
        return;
    }
    
    // Check method
    auto method = cJSON_GetObjectItem(json, "method");
    if (method == nullptr || !cJSON_IsString(method)) {
        ESP_LOGE(TAG, "Missing method");
        return;
    }
    
    auto method_str = std::string(method->valuestring);
    if (method_str.find("notifications") == 0) {
        return;
    }
    
    // Check params
    auto params = cJSON_GetObjectItem(json, "params");
    if (params != nullptr && !cJSON_IsObject(params)) {
        ESP_LOGE(TAG, "Invalid params for method: %s", method_str.c_str());
        return;
    }

    auto id = cJSON_GetObjectItem(json, "id");
    if (id == nullptr || !cJSON_IsNumber(id)) {
        ESP_LOGE(TAG, "Invalid id for method: %s", method_str.c_str());
        return;
    }
    auto id_int = id->valueint;
    
    if (method_str == "initialize") {
        if (cJSON_IsObject(params)) {
            auto capabilities = cJSON_GetObjectItem(params, "capabilities");
            if (cJSON_IsObject(capabilities)) {
                ParseCapabilities(capabilities);
            }
        }
        auto app_desc = esp_app_get_description();
        std::string message = "{\"protocolVersion\":\"2024-11-05\",\"capabilities\":{\"tools\":{}},\"serverInfo\":{\"name\":\"" BOARD_NAME "\",\"version\":\"";
        message += app_desc->version;
        message += "\"}}";
        ReplyResult(id_int, message);
    } else if (method_str == "tools/list") {
        std::string cursor_str = "";
        bool list_user_only_tools = false;
        if (params != nullptr) {
            auto cursor = cJSON_GetObjectItem(params, "cursor");
            if (cJSON_IsString(cursor)) {
                cursor_str = std::string(cursor->valuestring);
            }
            auto with_user_tools = cJSON_GetObjectItem(params, "withUserTools");
            if (cJSON_IsBool(with_user_tools)) {
                list_user_only_tools = with_user_tools->valueint == 1;
            }
        }
        GetToolsList(id_int, cursor_str, list_user_only_tools);
    } else if (method_str == "tools/call") {
        if (!cJSON_IsObject(params)) {
            ESP_LOGE(TAG, "tools/call: Missing params");
            ReplyError(id_int, "Missing params");
            return;
        }
        auto tool_name = cJSON_GetObjectItem(params, "name");
        if (!cJSON_IsString(tool_name)) {
            ESP_LOGE(TAG, "tools/call: Missing name");
            ReplyError(id_int, "Missing name");
            return;
        }
        auto tool_arguments = cJSON_GetObjectItem(params, "arguments");
        if (tool_arguments != nullptr && !cJSON_IsObject(tool_arguments)) {
            ESP_LOGE(TAG, "tools/call: Invalid arguments");
            ReplyError(id_int, "Invalid arguments");
            return;
        }
        DoToolCall(id_int, std::string(tool_name->valuestring), tool_arguments);
    } else {
        ESP_LOGE(TAG, "Method not implemented: %s", method_str.c_str());
        ReplyError(id_int, "Method not implemented: " + method_str);
    }
}

void McpServer::ReplyResult(int id, const std::string& result) {
    std::string payload = "{\"jsonrpc\":\"2.0\",\"id\":";
    payload += std::to_string(id) + ",\"result\":";
    payload += result;
    payload += "}";
    Application::GetInstance().SendMcpMessage(payload);
}

void McpServer::ReplyError(int id, const std::string& message) {
    std::string payload = "{\"jsonrpc\":\"2.0\",\"id\":";
    payload += std::to_string(id);
    payload += ",\"error\":{\"message\":\"";
    payload += message;
    payload += "\"}}";
    Application::GetInstance().SendMcpMessage(payload);
}

void McpServer::GetToolsList(int id, const std::string& cursor, bool list_user_only_tools) {
    const int max_payload_size = 8000;
    std::string json = "{\"tools\":[";
    
    bool found_cursor = cursor.empty();
    auto it = tools_.begin();
    std::string next_cursor = "";
    
    while (it != tools_.end()) {
        // 如果我们还没有找到起始位置，继续搜索
        if (!found_cursor) {
            if ((*it)->name() == cursor) {
                found_cursor = true;
            } else {
                ++it;
                continue;
            }
        }

        if (!list_user_only_tools && (*it)->user_only()) {
            ++it;
            continue;
        }
        
        // 添加tool前检查大小
        std::string tool_json = (*it)->to_json() + ",";
        if (json.length() + tool_json.length() + 30 > max_payload_size) {
            // 如果添加这个tool会超出大小限制，设置next_cursor并退出循环
            next_cursor = (*it)->name();
            break;
        }
        
        json += tool_json;
        ++it;
    }
    
    if (json.back() == ',') {
        json.pop_back();
    }
    
    if (json.back() == '[' && !tools_.empty()) {
        // 如果没有添加任何tool，返回错误
        ESP_LOGE(TAG, "tools/list: Failed to add tool %s because of payload size limit", next_cursor.c_str());
        ReplyError(id, "Failed to add tool " + next_cursor + " because of payload size limit");
        return;
    }

    if (next_cursor.empty()) {
        json += "]}";
    } else {
        json += "],\"nextCursor\":\"" + next_cursor + "\"}";
    }
    
    ReplyResult(id, json);
}

void McpServer::DoToolCall(int id, const std::string& tool_name, const cJSON* tool_arguments) {
    auto tool_iter = std::find_if(tools_.begin(), tools_.end(), 
                                 [&tool_name](const McpTool* tool) { 
                                     return tool->name() == tool_name; 
                                 });
    
    if (tool_iter == tools_.end()) {
        ESP_LOGE(TAG, "tools/call: Unknown tool: %s", tool_name.c_str());
        ReplyError(id, "Unknown tool: " + tool_name);
        return;
    }

    PropertyList arguments = (*tool_iter)->properties();
    try {
        for (auto& argument : arguments) {
            bool found = false;
            if (cJSON_IsObject(tool_arguments)) {
                auto value = cJSON_GetObjectItem(tool_arguments, argument.name().c_str());
                if (argument.type() == kPropertyTypeBoolean && cJSON_IsBool(value)) {
                    argument.set_value<bool>(value->valueint == 1);
                    found = true;
                } else if (argument.type() == kPropertyTypeInteger && cJSON_IsNumber(value)) {
                    argument.set_value<int>(value->valueint);
                    found = true;
                } else if (argument.type() == kPropertyTypeString && cJSON_IsString(value)) {
                    argument.set_value<std::string>(value->valuestring);
                    found = true;
                } else if (argument.type() == kPropertyTypeString &&
                           (cJSON_IsObject(value) || cJSON_IsArray(value))) {
                    /* E1 display 载荷的嵌套 data 是 JSON 对象 (如 {"view":"text","data":
                     * {"text":"…"}})。对声明为 string 型的属性, 把对象/数组序列化后
                     * 整体传入, 让 kidpal/display 消费端自行解析 (无对象的原生小智工具
                     * 不受影响; 该分支仅对 string 型属性 + 对象/数组参数触发)。 */
                    char *dumped = cJSON_PrintUnformatted(value);
                    if (dumped != nullptr) {
                        argument.set_value<std::string>(dumped);
                        cJSON_free(dumped);
                        found = true;
                    }
                }
            }

            if (!argument.has_default_value() && !found) {
                ESP_LOGE(TAG, "tools/call: Missing valid argument: %s", argument.name().c_str());
                ReplyError(id, "Missing valid argument: " + argument.name());
                return;
            }
        }
    } catch (const std::exception& e) {
        ESP_LOGE(TAG, "tools/call: %s", e.what());
        ReplyError(id, e.what());
        return;
    }

    // Use main thread to call the tool
    auto& app = Application::GetInstance();
    app.Schedule([this, id, tool_iter, arguments = std::move(arguments)]() {
        /* kidpal/alert 的「渲染完成才回执」需要在工具回调里拿到本次 JSON-RPC 传输 id
         * （回调签名只有 PropertyList，无 id 通道）：在 Call 之前记下它，回调同步读取
         * （同一主循环任务内，无竞态）。其余工具的同步自动回复与 ack 形状零改动。 */
        pending_call_id_ = id;
        try {
            ReplyResult(id, (*tool_iter)->Call(arguments));
        } catch (const std::exception& e) {
            ESP_LOGE(TAG, "tools/call: %s", e.what());
            ReplyError(id, e.what());
        }
    });
}
