#include "protocol.h"

#include <cstdio>
#include <string>

#include <esp_log.h>

#define TAG "Protocol"

/* kidpal_story_fetch 无接收者发送原语的活动实例指针（protocol.h 声明）。构造置位、
 * 析构清位——Application::ResetProtocol 销毁协议对象后绝不留悬垂指针。 */
Protocol* Protocol::s_kidpal_active_ = nullptr;

Protocol::Protocol() {
    s_kidpal_active_ = this;
}

Protocol::~Protocol() {
    if (s_kidpal_active_ == this) {
        s_kidpal_active_ = nullptr;
    }
}

void Protocol::OnIncomingJson(std::function<void(const cJSON* root)> callback) {
    on_incoming_json_ = callback;
}

void Protocol::OnIncomingAudio(std::function<void(std::unique_ptr<AudioStreamPacket> packet)> callback) {
    on_incoming_audio_ = callback;
}

void Protocol::OnAudioChannelOpened(std::function<void()> callback) {
    on_audio_channel_opened_ = callback;
}

void Protocol::OnAudioChannelClosed(std::function<void()> callback) {
    on_audio_channel_closed_ = callback;
}

void Protocol::OnNetworkError(std::function<void(const std::string& message)> callback) {
    on_network_error_ = callback;
}

void Protocol::OnConnected(std::function<void()> callback) {
    on_connected_ = callback;
}

void Protocol::OnDisconnected(std::function<void()> callback) {
    on_disconnected_ = callback;
}

void Protocol::SetError(const std::string& message) {
    error_occurred_ = true;
    if (on_network_error_ != nullptr) {
        on_network_error_(message);
    }
}

void Protocol::SendAbortSpeaking(AbortReason reason) {
    std::string message = "{\"session_id\":\"" + session_id_ + "\",\"type\":\"abort\"";
    if (reason == kAbortReasonWakeWordDetected) {
        message += ",\"reason\":\"wake_word_detected\"";
    }
    message += "}";
    SendText(message);
}

void Protocol::SendWakeWordDetected(const std::string& wake_word) {
    std::string json = "{\"session_id\":\"" + session_id_ + 
                      "\",\"type\":\"listen\",\"state\":\"detect\",\"text\":\"" + wake_word + "\"}";
    SendText(json);
}

void Protocol::SendStartListening(ListeningMode mode) {
    std::string message = "{\"session_id\":\"" + session_id_ + "\"";
    message += ",\"type\":\"listen\",\"state\":\"start\"";
    if (mode == kListeningModeRealtime) {
        message += ",\"mode\":\"realtime\"";
    } else if (mode == kListeningModeAutoStop) {
        message += ",\"mode\":\"auto\"";
    } else {
        message += ",\"mode\":\"manual\"";
    }
    message += "}";
    SendText(message);
}

void Protocol::SendStopListening() {
    std::string message = "{\"session_id\":\"" + session_id_ + "\",\"type\":\"listen\",\"state\":\"stop\"}";
    SendText(message);
}

void Protocol::SendMcpMessage(const std::string& payload) {
    std::string message = "{\"session_id\":\"" + session_id_ + "\",\"type\":\"mcp\",\"payload\":" + payload + "}";
    SendText(message);
}

void Protocol::SendKidpalStoryDelete(const char* id) {
    /* 契约 §四 kidpal_story_delete（逐字仿 SendAbortSpeaking 形态）:
     * {"session_id":"...","type":"kidpal_story_delete","id":"<story-id>"}。
     * 单向通知、无应答消息（front 幂等承接, 隐式 ack = story_list 重推对账）。 */
    std::string safe_id = (id != nullptr) ? id : "";
    std::string message = "{\"session_id\":\"" + session_id_ +
                          "\",\"type\":\"kidpal_story_delete\",\"id\":\"" + safe_id + "\"}";
    SendText(message);
}

/* 最小 JSON 字符串转义（前向声明——selected_id 与分页游标共用同一单源实现；
 * 定义在本文件下方 SendKidpalPointsPage 前）。 */
static std::string kidpal_points_json_escape(const char *raw);

void Protocol::SendKidpalSessionCtx(const char* source_page, const char* selected_id) {
    /* 契约 §四 kidpal_session_ctx（逐字仿 SendAbortSpeaking / SendKidpalStoryDelete
     * 形态）: {"session_id":"...","type":"kidpal_session_ctx","source_page":"..."}，
     * 详细账本语境可选尾附 "selected_id":"<opaque>"（K2 additive；设备只发最近收到
     * 的 points_ledger items 里的 id，它不是删除或写入指令）。
     * 单向通知、无应答消息（front 幂等承接, 拦截消费不下透引擎）;
     * 发送失败降级普通会话不重试（notes ③ 负锚——调用恰一次, 无重试环）。 */
    std::string safe_page = (source_page != nullptr) ? source_page : "";
    std::string message = "{\"session_id\":\"" + session_id_ +
                          "\",\"type\":\"kidpal_session_ctx\",\"source_page\":\"" + safe_page + "\"";
    if (selected_id != nullptr && selected_id[0] != '\0') {
        message += ",\"selected_id\":\"" + kidpal_points_json_escape(selected_id) + "\"";
    }
    message += "}";
    SendText(message);
}

/* 契约 §四 kidpal_story_discard（TaskPackage 261021-p3-record-cancel-lifecycle;
 * 逐字仿 SendKidpalStoryDelete 拼串形态）:
 *   {"session_id":"<sid>","type":"kidpal_story_discard"}
 * 两字段、单向通知、无应答消息（front 按本连接会话键权威取消、幂等承接）。
 * 发送时机 = mcp_server DISCARD_RECORD case 内 CloseAudioChannel 之前（同一 TCP
 * 有序, 网关泵先消费 discard 再处理断连）。
 * 无接收者形态：活动实例 = 最近构造且未析构的 Protocol（s_kidpal_active_）；
 * 未就绪 = 静默 no-op（离线降级, 不产生半帧）。 */
void SendKidpalStoryDiscard()
{
    Protocol *p = Protocol::s_kidpal_active_;

    if (p == nullptr) {
        return; /* 协议未就绪（离线降级）：静默 no-op，不产生半帧 */
    }
    std::string message = "{\"session_id\":\"" + p->session_id_ +
                          "\",\"type\":\"kidpal_story_discard\"}";
    p->SendText(message);
}

bool Protocol::IsTimeout() const {
    const int kTimeoutSeconds = 120;
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - last_incoming_time_);
    bool timeout = duration.count() > kTimeoutSeconds;
    if (timeout) {
        ESP_LOGE(TAG, "Channel timeout %ld seconds", (long)duration.count());
    }
    return timeout;
}

/* 契约 §四 kidpal_story_fetch（TaskPackage 261016-p3-story-body-sd-cache; 逐字仿
 * SendKidpalStoryDelete 形态）:
 *   {"session_id":"<sid>","type":"kidpal_story_fetch","ids":["<id>",...]}
 * 设备发起 request、网关以既有 story_text 帧逐篇 response（下行零新形状）。
 * 空 ids（count <= 0 / ids == NULL）= no-op（不发帧, 契约 §四「空 ids 数组 =
 * no-op」）。id 闭集 / 字符集校验归调用方（mcp_server 以只发 story_list items
 * 下发过的 id + kidpal_story_id_safe 门控）。
 * 无接收者形态：活动实例 = 最近构造且未析构的 Protocol（s_kidpal_active_）。
 * 调用纪律：mcp_server 经 Application::Schedule 在主任务闭包内调用（协议发送归
 * 主任务, 与 SendMcpMessage / SendKidpalStoryDelete 同款线程纪律）。 */
void SendKidpalStoryFetch(const char *const *ids, int count)
{
    Protocol *p = Protocol::s_kidpal_active_;

    if (p == nullptr || ids == nullptr || count <= 0) {
        return; /* 空 ids / 协议未就绪（离线降级）：静默 no-op，不产生半帧 */
    }
    std::string message = "{\"session_id\":\"" + p->session_id_ +
                          "\",\"type\":\"kidpal_story_fetch\",\"ids\":[";
    for (int i = 0; i < count; i++) {
        if (i > 0) {
            message += ",";
        }
        message += "\"";
        message += (ids[i] != nullptr) ? ids[i] : "";
        message += "\"";
    }
    message += "]}";
    p->SendText(message);
}

/* 最小 JSON 字符串转义（游标是 front 生成的不透明串，字符集通常只剩 base64url；
 * 仍按纪律转义双引号/反斜杠/控制符，绝不产出半截 JSON）。 */
static std::string kidpal_points_json_escape(const char *raw)
{
    std::string out;
    const unsigned char *s = reinterpret_cast<const unsigned char *>(raw);

    if (raw == nullptr) {
        return out;
    }
    for (; *s != 0; ++s) {
        switch (*s) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b";  break;
            case '\f': out += "\\f";  break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if (*s < 0x20u) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned>(*s));
                    out += buf;
                } else {
                    out += static_cast<char>(*s);
                }
                break;
        }
    }
    return out;
}

/* 契约 §四 kidpal_points_page（TaskPackage 261104-k0-points-e2e-spike; 逐字仿
 * SendKidpalStoryFetch 拼串形态）:
 *   {"session_id":"<sid>","type":"kidpal_points_page","cursor":null|"<opaque>",
 *    "known_revision":<int>,"limit":<int>}
 * 设备发起 request、网关以 §1.3⑧ points_ledger 帧逐次 response（下行零新形状）。
 * cursor NULL/空串 = 请求最新首页；known_revision < 0 = 不带该键（front 不做版本
 * 重置判定）；limit 由调用方保证 1–50（越界由 front 拒绝且不回帧，契约 §四）。
 * 无接收者形态：活动实例 = 最近构造且未析构的 Protocol（s_kidpal_active_）；
 * 未就绪 = 静默 no-op（离线降级，不产生半帧）。 */
void SendKidpalPointsPage(const char *cursor, int known_revision, int limit)
{
    Protocol *p = Protocol::s_kidpal_active_;

    if (p == nullptr) {
        return; /* 协议未就绪（离线降级）：静默 no-op，不产生半帧 */
    }
    std::string message = "{\"session_id\":\"" + p->session_id_ +
                          "\",\"type\":\"kidpal_points_page\",\"cursor\":";
    if (cursor != nullptr && cursor[0] != '\0') {
        message += "\"";
        message += kidpal_points_json_escape(cursor);
        message += "\"";
    } else {
        message += "null";
    }
    if (known_revision >= 0) {
        message += ",\"known_revision\":" + std::to_string(known_revision);
    }
    message += ",\"limit\":" + std::to_string(limit) + "}";
    p->SendText(message);
}

/* 契约 §四 kidpal_points_delete（TaskPackage 261104-k2-points-device-ui; 逐字仿
 * SendKidpalStoryDelete 拼串形态）:
 *   {"session_id":"<sid>","type":"kidpal_points_delete","id":"<points-entry-id>"}
 * 设备在详细账本删除确认子态 KEY 确认后发出；front 整条消费并拦截、交 points.py
 * 删除并按剩余分录重算，以权威 points_home + points_ledger 首页 restore 重推作
 * 隐式 ack。单向通知、无独立应答消息；未收到不含该 id 的对账帧可重发。
 * id 闭集 = 最近收到的 points_ledger items（设备不造 id）；空 id = no-op（不发半帧）。
 * 无接收者形态：活动实例 = 最近构造且未析构的 Protocol（s_kidpal_active_）；
 * 未就绪 = 静默 no-op（离线降级，不产生半帧）。 */
void SendKidpalPointsDelete(const char *id)
{
    Protocol *p = Protocol::s_kidpal_active_;

    if (p == nullptr || id == nullptr || id[0] == '\0') {
        return; /* 无 id / 协议未就绪：静默 no-op，不产生半帧 */
    }
    std::string message = "{\"session_id\":\"" + p->session_id_ +
                          "\",\"type\":\"kidpal_points_delete\",\"id\":\"" +
                          kidpal_points_json_escape(id) + "\"}";
    p->SendText(message);
}
