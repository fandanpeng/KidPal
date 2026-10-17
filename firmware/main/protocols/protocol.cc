#include "protocol.h"

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

void Protocol::SendKidpalSessionCtx(const char* source_page) {
    /* 契约 §四 kidpal_session_ctx（逐字仿 SendAbortSpeaking / SendKidpalStoryDelete
     * 形态）: {"session_id":"...","type":"kidpal_session_ctx","source_page":"..."}。
     * 单向通知、无应答消息（front 幂等承接, 拦截消费不下透引擎）;
     * 发送失败降级普通会话不重试（notes ③ 负锚——调用恰一次, 无重试环）。 */
    std::string safe_page = (source_page != nullptr) ? source_page : "";
    std::string message = "{\"session_id\":\"" + session_id_ +
                          "\",\"type\":\"kidpal_session_ctx\",\"source_page\":\"" + safe_page + "\"}";
    SendText(message);
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
