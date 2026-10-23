#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cJSON.h>
#include <string>
#include <functional>
#include <chrono>
#include <vector>

struct AudioStreamPacket {
    int sample_rate = 0;
    int frame_duration = 0;
    uint32_t timestamp = 0;
    std::vector<uint8_t> payload;
};

struct BinaryProtocol2 {
    uint16_t version;
    uint16_t type;          // Message type (0: OPUS, 1: JSON)
    uint32_t reserved;      // Reserved for future use
    uint32_t timestamp;     // Timestamp in milliseconds (used for server-side AEC)
    uint32_t payload_size;  // Payload size in bytes
    uint8_t payload[];      // Payload data
} __attribute__((packed));

struct BinaryProtocol3 {
    uint8_t type;
    uint8_t reserved;
    uint16_t payload_size;
    uint8_t payload[];
} __attribute__((packed));

enum AbortReason {
    kAbortReasonNone,
    kAbortReasonWakeWordDetected
};

enum ListeningMode {
    kListeningModeAutoStop,
    kListeningModeManualStop,
    kListeningModeRealtime // 需要 AEC 支持
};

class Protocol {
public:
    Protocol();
    virtual ~Protocol();

    inline int server_sample_rate() const {
        return server_sample_rate_;
    }
    inline int server_frame_duration() const {
        return server_frame_duration_;
    }
    inline const std::string& session_id() const {
        return session_id_;
    }

    void OnIncomingAudio(std::function<void(std::unique_ptr<AudioStreamPacket> packet)> callback);
    void OnIncomingJson(std::function<void(const cJSON* root)> callback);
    void OnAudioChannelOpened(std::function<void()> callback);
    void OnAudioChannelClosed(std::function<void()> callback);
    void OnNetworkError(std::function<void(const std::string& message)> callback);
    void OnConnected(std::function<void()> callback);
    void OnDisconnected(std::function<void()> callback);

    virtual bool Start() = 0;
    virtual bool OpenAudioChannel() = 0;
    virtual void CloseAudioChannel() = 0;
    virtual bool IsAudioChannelOpened() const = 0;
    virtual bool SendAudio(std::unique_ptr<AudioStreamPacket> packet) = 0;
    virtual void SendWakeWordDetected(const std::string& wake_word);
    virtual void SendStartListening(ListeningMode mode);
    virtual void SendStopListening();
    virtual void SendAbortSpeaking(AbortReason reason);
    virtual void SendMcpMessage(const std::string& message);
    /* §四 KidPal 私有扩展消息（2026-09-24 契约门 Q5/Q6; TaskPackage
     * 260924-m4-k2-p3-star-notebook-ui）: 端→网关单向通知, 无应答消息——可靠性 =
     * 隐式 ack（超时重发由 mcp_server tick 驱动, story_list 重推对账冲销;
     * front 幂等承接）。id 闭集纪律: 只发 story_list items 下发过的 id。
     * K3 的 kidpal_session_ctx 发送同缝复用（原语形态一次做对）。 */
    virtual void SendKidpalStoryDelete(const char* id);
    /* §四 kidpal_session_ctx（TaskPackage 260924-m4-k3-record-session-live; K2
     * delete 原语同缝同族扩展）: 会话发起信号, 端→网关单向通知——front 收到后
     * 拦截消费（切累积态 + 条件推送 story_list）, 不下透引擎。首帧音频前
     * （server hello 之后）发送; 通道复用会话同样发送; 重复发送安全（front 幂等
     * 承接）; 省略 = 普通会话向后兼容。source_page 闭集 story/schedule/home。 */
    virtual void SendKidpalSessionCtx(const char* source_page);
    /* §四 kidpal_story_fetch（TaskPackage 261016-p3-story-body-sd-cache）: 设备发起
     * 的批量正文取回请求 —— 契约面首个 request → response 请求-应答环（应答 = 既有
     * story_text 帧，零新形状）。发送原语形态见本头末尾自由函数
     * SendKidpalStoryFetch（无接收者：活动实例 = 最近构造且未析构的 Protocol）。 */
    friend void SendKidpalStoryFetch(const char *const *ids, int count);
    /* §四 kidpal_story_discard（TaskPackage 261021-p3-record-cancel-lifecycle F2 /
     * 契约 §四 2026-10-21 LANDED）: 丢弃会话的显式通知，端→网关单向、无应答——
     * front 按本连接会话键权威取消（不归档）、幂等承接。发送时机 = 设备
     * CloseAudioChannel **之前**同连接先行（同一 TCP 有序, front 泵先消费 discard
     * 再处理断连）。无接收者形态同 fetch：活动实例 = s_kidpal_active_。 */
    friend void SendKidpalStoryDiscard();

protected:
    std::function<void(const cJSON* root)> on_incoming_json_;
    std::function<void(std::unique_ptr<AudioStreamPacket> packet)> on_incoming_audio_;
    std::function<void()> on_audio_channel_opened_;
    std::function<void()> on_audio_channel_closed_;
    std::function<void(const std::string& message)> on_network_error_;
    std::function<void()> on_connected_;
    std::function<void()> on_disconnected_;

    int server_sample_rate_ = 24000;
    int server_frame_duration_ = 60;
    bool error_occurred_ = false;
    std::string session_id_;
    std::chrono::time_point<std::chrono::steady_clock> last_incoming_time_;
    /* kidpal_story_fetch 无接收者发送原语的活动实例指针（构造置位 / 析构清位）:
     * 单实例现实 = Application::protocol_ 唯一所有权（ResetProtocol 销毁后指针复位,
     * 绝不悬垂）。 */
    static Protocol* s_kidpal_active_;

    virtual bool SendText(const std::string& text) = 0;
    virtual void SetError(const std::string& message);
    virtual bool IsTimeout() const;
};

/* §四 kidpal_story_fetch 发送原语（TaskPackage 261016-p3-story-body-sd-cache;
 * seam contract [C10] home = protocols/protocol.{h,cc}）。
 * 线形状（逐字）:
 *   {"session_id":"<sid>","type":"kidpal_story_fetch","ids":["<id>",...]}
 * count <= 0 / ids == NULL = 空 ids no-op（不发帧）。调用纪律：由 mcp_server 在
 * Application::Schedule 主任务闭包内调用（协议发送归主任务, 与 SendMcpMessage /
 * SendKidpalStoryDelete 同款线程纪律）；id 闭集 + 字符集校验由调用方与 [C9]
 * kidpal_story_id_safe 承接。 */
void SendKidpalStoryFetch(const char *const *ids, int count);

/* §四 kidpal_story_discard 发送原语（TaskPackage 261021-p3-record-cancel-lifecycle
 * RCL-F1; 逐字仿 SendKidpalStoryDelete 拼串形态）。
 * 线形状（逐字）:
 *   {"session_id":"<sid>","type":"kidpal_story_discard"}
 * 两字段、单向通知、无应答消息（front 拦截消费 + 幂等承接）。发送时机 = 设备
 * CloseAudioChannel 之前同连接先行（同一 TCP 有序, 网关泵先消费 discard 再处理
 * 断连——先断连 = 帧发不出去, 显式丢弃通道退化为掉线路径）。
 * 无接收者形态同 SendKidpalStoryFetch：活动实例 = 最近构造且未析构的 Protocol
 * （s_kidpal_active_）；未就绪 = 静默 no-op（离线降级, 不产生半帧）。 */
void SendKidpalStoryDiscard();

#endif // PROTOCOL_H

