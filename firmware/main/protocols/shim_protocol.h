#ifndef SHIM_PROTOCOL_H
#define SHIM_PROTOCOL_H

#include "protocol.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// KidPal device-side HTTP shim protocol (tracer bullet, acceptance SMK-A1..A4).
//
// Replaces the xiaozhi mqtt/websocket streaming transport with synchronous HTTP
// request/response against the cloud shim (server/shim.py). This carries the
// device leg of the `KEY -> mic -> shim -> cloud LLM -> TTS -> speaker` smoke:
//   uplink   : mic opus frames accumulated, decoded back to WAV PCM16 16 kHz
//              mono, POST /v1/asr {audio:<b64 wav>} -> {text}
//              POST /v1/chat {text} -> {audio:<b64 mp3>, display}
//   downlink : mp3 base64-decoded and pushed to the speaker decode path;
//              display.text shown on screen through the tts/stt message language.
//
// Provisioning carrier: compile-time Kconfig CONFIG_SHIM_BASE with a runtime
// NVS override (Settings namespace "shim", key "shim_base" / "device_token").
// The device token is never hard-coded (security axis secrets-not-in-firmware).
class ShimProtocol : public Protocol {
public:
    ShimProtocol();
    ~ShimProtocol() override;

    bool Start() override;
    bool OpenAudioChannel() override;
    void CloseAudioChannel() override;
    bool IsAudioChannelOpened() const override;
    bool SendAudio(std::unique_ptr<AudioStreamPacket> packet) override;
    void SendStartListening(ListeningMode mode) override;
    void SendStopListening() override;

private:
    struct PendingFrame {
        int sample_rate = 0;
        int frame_duration = 0;
        std::vector<uint8_t> payload;
    };

    bool SendText(const std::string& text) override;

    // HTTP helper: POST JSON to shim_base_ + path, return status + body.
    bool PostJson(const std::string& path, const std::string& body,
                  int& status_code, std::string& response);

    // Uplink finalisation: decode opus -> wav -> /v1/asr -> /v1/chat -> play.
    void FinalizeSession();
    static void FinalizeTask(void* arg);
    void RunFinalize();
    bool BuildWav(std::vector<uint8_t>& wav);

    void EmitTtsStart();
    void EmitTtsSentence(const std::string& text);
    void EmitStt(const std::string& text);

    static std::string NewCorrelationId();
    static std::string Base64Encode(const std::vector<uint8_t>& data);
    static bool Base64Decode(const std::string& in, std::vector<uint8_t>& out);
    static std::string JsonStringField(const std::string& json, const char* key);
    static std::string JsonDisplayText(const std::string& json);

    std::string shim_base_;
    std::string device_token_;
    std::atomic<bool> audio_channel_opened_{false};
    std::atomic<bool> finalizing_{false};
    std::vector<PendingFrame> pending_frames_;
    int accumulated_ms_ = 0;
    int capture_ms_ = 3000;
    std::string correlation_id_;
};

#endif // SHIM_PROTOCOL_H