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
#include "display/char_stroke.h"   /* 查生字 P4 (包 260829-char-stroke): 缺字段防御降级门 */
#include "display/char_page.h"     /* 查生字 P4 LVGL 页 (设备侧) */
#include "display/reminder_page.h" /* 提醒弹窗消费面 (包 260829-schedule-reminder):
                                    * kidpal/alert 载荷解析 + 四态状态机 + 富弹窗渲染 */
#include "display/kidpal_render_node.h" /* 包 9b HWV-A6: 设备渲染节点 show_home/cycle/cache */
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
 * P3 故事页生产接线辅助 (包 260829-story-p3 §9 F7 殿后 / 包 9b HWV-A9 承接)。
 * 录音态演出 tick 的 LVGL 定时器 + 页态重渲染 + 页内输入动作码。story_page.h 是
 * 冻结头、未导出 KSA_ACT_*，此处按冻结验收 story_p3 与 story_page.c:740-748 的
 * 文档化数值码本地镜像一份（升级路径 = story_page.h 一旦导出即转用之；双源风险由
 * 本注释钉明，页内输入动作语义的**单源**仍在冻结实现 kidpal_story_page_input 内）。
 * LVGL 定时器在 LVGL 主处理任务回调（= Schedule 目标线程），与 g_story_* 单例无竞态
 * （对偶 AlertDrainTimer 既有约定）。
 * ------------------------------------------------------------------------ */
namespace {
enum {
    kStoryActGlobal       = -2, /* 页不拥有该手势 -> 交回全局路由层 */
    kStoryActDegrade      = -3, /* 越界键位/NULL -> 降级、视图零改动 */
    kStoryActNone         = 0,  /* 无副作用 */
    kStoryActCursorNext   = 1,  /* 列表光标移动 */
    kStoryActOpenReader   = 2,  /* 进阅读态 */
    kStoryActPageTurn     = 3,  /* 阅读态翻屏 */
    kStoryActTtsRead      = 4,  /* 朗读请求（声学殿后 9b，仅计数） */
    kStoryActStopRecord   = 5,  /* 停录 -> 归档请求（archive_pend++ 由冻结实现置位） */
    kStoryActCancelRecord = 6,  /* 录音中取消 -> 丢弃、不归档 */
};

/* 录音态演出驱动：page==RECORDING 期间每 ~100ms 用冻结 kidpal_story_rec_tick 就地推
 * elapsed/闪点/声波并重绘；一旦离开录音态即自删（无常驻定时器开销）。 */
lv_timer_t* g_story_rec_timer = nullptr;
uint32_t    g_story_rec_start_ms = 0;

/* 故事页三态快照（列表视图 + 列表载荷 + 正文载荷）打包成**一个堆对象**。
 * 尺寸：view 108B + list 2824B + text 2208B ≈ 5.1KB —— 绝不可作栈对象（见本文件
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
        /* 崩溃修复（包 260829-hardware-validation）：服务端推送页面渲染必须夹在
         * begin/end 之间 → 画到 s_kidpal_scr + 持 LVGL 锁，避免 clean 聊天屏致
         * SetStatus 悬空崩溃，并消除与 LVGL 任务的竞态。 */
        kidpal_render_node_begin_page();
        kidpal_story_page_render(&snap->v, &snap->l, &snap->t);
        /* 包 9b HWV-A6: 同步至渲染节点缓存，供 BOOT 双按翻页 cycle_next 读取。 */
        kidpal_render_node_cache_story(&snap->v, &snap->l, &snap->t);
        kidpal_render_node_end_page();
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
}  // namespace

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
    /* 页内特例优先消费 (spec §4.0「页内特例」列)：把 KEY/BOOT 手势交进冻结消费面
     * kidpal_story_page_input，按其返回动作码驱动页态。页不拥有该手势 (GLOBAL/NONE/
     * DEGRADE) -> 返回 false，交回全局路由层 (翻页/会话切换语义不变)。 */
    int act = kidpal_story_page_input(&g_story_view, key, g);
    switch (act) {
        case kStoryActStopRecord:
            /* 停录: 冻结实现已置 archive_pend++ 并把页态切回 LIST、rec_seconds 定格;
             * 演出 tick 的 LVGL timer 在下个回调发现已非 RECORDING -> 自删。回列表渲染。 */
            ESP_LOGI(TAG, "story_key stop_rec archive_pend=%d", g_story_view.archive_pend);
            ScheduleStoryRender();
            return true;
        case kStoryActCancelRecord:
            /* 录音中取消 -> 丢弃、不归档 (V31)。回列表渲染。 */
            ESP_LOGI(TAG, "story_key cancel_rec cancel_count=%d", g_story_view.cancel_count);
            ScheduleStoryRender();
            return true;
        case kStoryActCursorNext:
        case kStoryActOpenReader:
        case kStoryActPageTurn:
            ScheduleStoryRender();
            return true;
        case kStoryActTtsRead:
            /* 朗读请求 (声学殿后 9b): 冻结实现已递增 tts_requests; 此处不外呼播报,
             * 页态不变 -> 不做无副作用重渲染。 */
            return true;
        default:
            /* kStoryActGlobal / kStoryActNone / kStoryActDegrade -> 交回全局路由。 */
            return false;
    }
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
                case 1: /* VRT_ROUTE_HOME —— 回 P1 首页常显 */
                    cJSON_AddStringToObject(ack, "route", "home");
                    /* 包 9b HWV-A6: 渲染节点落地后，home 路由改推首页帧至 RLCD。
                     * 旧行为（SetChatMessage 空串）仅清 xiaozhi 对话区，不显示 KidPal 首页。 */
                    app.Schedule([]() {
                        kidpal_render_node_show_home();
                    });
                    break;
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
                            app.StartListening();
                            if (consumed == 1) {
                                /* 进录音态 -> 首帧渲染 + 起演出 tick 驱动（显示循环侧真实
                                 * 消费 kidpal_story_rec_tick, 录音「正在录」可观察; HWV-A9）。 */
                                ScheduleStoryRender();
                                StartStoryRecTimer();
                            }
                            break;
                        }
                        default: /* 未知 action -> 降级, 不执行 */
                            cJSON_AddBoolToObject(ack, "degrade", true);
                            break;
                    }
                    break;
                }
                case 4: { /* VRT_ROUTE_SCHEDULE —— P2 课表页 (契约 §1.3 ②, 包
                           * 260829-schedule-p2)。解析 data {"title","items":[…]}：
                           * 成功 -> kidpal_schedule_page_render 渲染面真画 (标题
                           * 弹跳/圆点两态/行列表)；解析失败 -> 显式降级 (友好提示,
                           * 绝不盲渲染, V27 / spec §4.0)。 */
                    cJSON_AddStringToObject(ack, "route", "schedule");
                    char corr_id[24];
                    snprintf(corr_id, sizeof(corr_id), "kdp-%u",
                             static_cast<unsigned>(esp_log_timestamp() & 0xFFFFFFu));
                    char title[64];
                    /* items[16] = 16 × 112B = 1792B（含 Schedule 闭包按值捕获的再一份
                     * = 3.5KB 峰值）→ 上堆，理由见本文件头「ssl_receive 栈纪律」。 */
                    auto items = std::make_shared<std::array<kidpal_schedule_item_t, 16>>();
                    int n = kidpal_schedule_parse(data.c_str(), title, sizeof(title),
                                                  items->data(), 16);
                    if (n < 0) {
                        /* 降级：schedule data 畸形/缺字段 -> 友好提示, 不盲渲染 */
                        cJSON_AddBoolToObject(ack, "degrade", true);
                        ESP_LOGW(TAG, "display_dispatch view=schedule corr_id=%s items=0 degrade=true", corr_id);
                        auto display = Board::GetInstance().GetDisplay();
                        app.Schedule([display]() {
                            display->SetChatMessage("system", "课表数据有点问题，稍后再试试");
                        });
                        break;
                    }
                    /* 明天视图判定 (P2-A4 全空心前提)：契约 data 形状零变更、无
                     * is_tomorrow 字段 —— producer 在 title 语义里携带, 设备侧按
                     * title 含「明天」做最小判定 (minimal, 升级路径=producer 显式
                     * 约定 title 词汇表, 不动 data 形状)。 */
                    int is_tomorrow = (strstr(title, "明天") != nullptr) ? 1 : 0;
                    int locate_row = -1; /* 语音问课表定位行由语音消费路径经
                                          * kidpal_schedule_locate 续接, 默认无星标 */
                    /* 设备当前 HH:MM (圆点两态判定; RTC 未同步时按 00:00 全实心降级) */
                    char now_hhmm[8] = "00:00";
                    {
                        time_t now_t = time(nullptr);
                        struct tm tm_now;
                        if (localtime_r(&now_t, &tm_now) != nullptr) {
                            snprintf(now_hhmm, sizeof(now_hhmm), "%02d:%02d",
                                     tm_now.tm_hour, tm_now.tm_min);
                        }
                    }
                    cJSON_AddNumberToObject(ack, "items", n);
                    ESP_LOGI(TAG, "display_dispatch view=schedule corr_id=%s items=%d is_tomorrow=%d degrade=false",
                             corr_id, n, is_tomorrow);
                    auto display = Board::GetInstance().GetDisplay();
                    std::string now_str = now_hhmm;
                    app.Schedule([display, title, items, n, is_tomorrow, now_str, locate_row, corr_id]() {
                        kidpal_render_node_begin_page();
                        int drawn = kidpal_schedule_page_render(
                            title, items->data(), n, is_tomorrow, now_str.c_str(), locate_row);
                        ESP_LOGI(TAG, "schedule_render corr_id=%s items=%d drawn=%d",
                                 corr_id, n, drawn);
                        /* 包 9b HWV-A6: 缓存课表至渲染节点，供 BOOT 双按翻页 cycle_next 读取。 */
                        kidpal_render_node_cache_schedule(title, items->data(), n, is_tomorrow, now_str.c_str());
                        kidpal_render_node_end_page();
                    });
                    break;
                }
                case 3: { /* VRT_ROUTE_CHAR —— 查生字 P4 (包 260829-char-stroke, 本包
                             additive 补 case 3, 只消费不改 view_router 数值码). */
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
                    /* 通过校验 -> 切 P4 页喂数据 (LVGL 主线程延迟执行) */
                    app.Schedule([char_data]() {
                        /* 崩溃修复（包 260829-hardware-validation 语义保留 / 归一屏）：
                         * char 页已并入 s_kidpal_scr 体系——kidpal_char_page_show 在活动屏
                         * 上清屏并绘制（先清后画、不自建独立屏），不触碰聊天层视觉控件，
                         * 天然免疫 SetStatus 悬空崩溃。
                         * 仍夹 begin/end：begin_page 把 SERVER_PAGE 推入 layer 栈 + 持 LVGL
                         * 锁消除与 LVGL 任务竞态 + 切活动屏到 s_kidpal_scr，随后 char_page_show
                         * 直接画在 s_kidpal_scr 上（净显示 = char 页）。 */
                        kidpal_render_node_begin_page();
                        kidpal_char_page_show(char_data.c_str());
                        /* 包 9b HWV-A6: 缓存笔顺 JSON 与有效标志，
                         * 供 BOOT 双按 cycle_next 重新停靠 CHAR 页时渲染。 */
                        kidpal_render_node_cache_char_data(char_data.c_str());
                        kidpal_render_node_end_page();
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
                    kidpal_story_view_init(&g_story_view, parsed.get());
                    g_story_list = *parsed;
                    cJSON_AddNumberToObject(ack, "items", items_n);
                    cJSON_AddNumberToObject(ack, "counter", kidpal_story_counter(parsed.get()));
                    ESP_LOGI(TAG, "story_list_dispatch corr_id=%s items=%d counter=%d degrade=false",
                             corr_id, items_n, kidpal_story_counter(parsed.get()));
                    {
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
                        /* 「上传成功那一瞬 -> 盖章帧」生产触发 (S-F2.2/S-F2.4, HWV-A9)：
                         * 孩子停录时页内输入 (kidpal_story_page_input STOP_RECORD) 把
                         * archive_pend 置位；网关侧停录归档成功后把整段正文以
                         * story_text(record) 组块回传 (本包 front_app .stop -> story_text
                         * 云侧接线)，设备在此收到这条「刚入册」正文即盖章。仅在停录待确认
                         * (archive_pend>0) 且 id 与最近一次盖章不同 (幂等，防 mcp 回执重试
                         * 重复发徽章) 时触发；否则 (回看既有故事) 照常进阅读态。 */
                        int fresh_save =
                            (g_story_view.archive_pend > 0) && parsed->id[0] != '\0' &&
                            std::strcmp(parsed->id, g_story_view.last_saved_id) != 0;
                        if (fresh_save) {
                            if (kidpal_story_on_saved(&g_story_view, parsed->id) == 0) {
                                if (g_story_view.archive_pend > 0) {
                                    g_story_view.archive_pend--; /* 消费一次待归档请求 */
                                }
                                cJSON_AddStringToObject(ack, "stamp", parsed->id);
                                ESP_LOGI(TAG, "story_saved_stamp corr_id=%s id=%s stamp_index=%d",
                                         corr_id, parsed->id, g_story_view.stamp_index);
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
