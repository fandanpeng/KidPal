#include "shim_protocol.h"

#include "application.h"
#include "audio_service.h"
#include "board.h"
#include "settings.h"
#include "system_info.h"

#include <opus_decoder.h>

#include <chrono>
#include <cstdio>
#include <cstring>
#include <memory>

#include <cJSON.h>
#include <esp_log.h>
#include <esp_system.h>
#include <mbedtls/base64.h>

#define TAG "Shim"

// --------------------------------------------------------------------------- //
// helpers
// --------------------------------------------------------------------------- //

std::string ShimProtocol::NewCorrelationId() {
    uint32_t a = esp_random(), b = esp_random(), c = esp_random(), d = esp_random();
    char buf[37];
    snprintf(buf, sizeof(buf), "%08x%08x%08x%08x",
             (unsigned)a, (unsigned)b, (unsigned)c, (unsigned)d);
    return std::string(buf);
}

std::string ShimProtocol::Base64Encode(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        return "";
    }
    size_t dlen = 0, olen = 0;
    mbedtls_base64_encode(nullptr, 0, &dlen, data.data(), data.size());
    std::string out(dlen, '\0');
    mbedtls_base64_encode(reinterpret_cast<unsigned char*>(out.data()), out.size(), &olen,
                          data.data(), data.size());
    out.resize(olen);
    return out;
}

bool ShimProtocol::Base64Decode(const std::string& in, std::vector<uint8_t>& out) {
    if (in.empty()) {
        return false;
    }
    size_t dlen = 0, olen = 0;
    mbedtls_base64_decode(nullptr, 0, &dlen,
                          reinterpret_cast<const unsigned char*>(in.data()), in.size());
    out.resize(dlen);
    if (mbedtls_base64_decode(out.data(), out.size(), &olen,
                              reinterpret_cast<const unsigned char*>(in.data()), in.size()) != 0) {
        out.clear();
        return false;
    }
    out.resize(olen);
    return true;
}

std::string ShimProtocol::JsonStringField(const std::string& json, const char* key) {
    cJSON* root = cJSON_Parse(json.c_str());
    if (root == nullptr) {
        return "";
    }
    std::string out;
    cJSON* item = cJSON_GetObjectItem(root, key);
    if (cJSON_IsString(item)) {
        out = item->valuestring;
    }
    cJSON_Delete(root);
    return out;
}

std::string ShimProtocol::JsonDisplayText(const std::string& json) {
    cJSON* root = cJSON_Parse(json.c_str());
    if (root == nullptr) {
        return "";
    }
    std::string out;
    cJSON* display = cJSON_GetObjectItem(root, "display");
    if (cJSON_IsObject(display)) {
        cJSON* data = cJSON_GetObjectItem(display, "data");
        if (cJSON_IsObject(data)) {
            cJSON* text = cJSON_GetObjectItem(data, "text");
            if (cJSON_IsString(text)) {
                out = text->valuestring;
            }
        }
        if (out.empty()) {
            cJSON* hint = cJSON_GetObjectItem(display, "speech_hint");
            if (cJSON_IsString(hint)) {
                out = hint->valuestring;
            }
        }
    }
    cJSON_Delete(root);
    return out;
}

static void AppendWavHeader(std::vector<uint8_t>& wav, int sample_rate, int channels,
                            int bits_per_sample, int pcm_bytes) {
    const int byte_rate = sample_rate * channels * bits_per_sample / 8;
    const int block_align = channels * bits_per_sample / 8;
    wav.clear();
    wav.resize(44);
    uint8_t* h = wav.data();
    auto put32 = [&](size_t off, uint32_t v) {
        h[off + 0] = (uint8_t)(v & 0xff);
        h[off + 1] = (uint8_t)((v >> 8) & 0xff);
        h[off + 2] = (uint8_t)((v >> 16) & 0xff);
        h[off + 3] = (uint8_t)((v >> 24) & 0xff);
    };
    auto put16 = [&](size_t off, uint16_t v) {
        h[off + 0] = (uint8_t)(v & 0xff);
        h[off + 1] = (uint8_t)((v >> 8) & 0xff);
    };
    memcpy(h, "RIFF", 4);
    put32(4, 36 + pcm_bytes);
    memcpy(h + 8, "WAVE", 4);
    memcpy(h + 12, "fmt ", 4);
    put32(16, 16);
    put16(20, 1);
    put16(22, (uint16_t)channels);
    put32(24, (uint32_t)sample_rate);
    put32(28, (uint32_t)byte_rate);
    put16(32, (uint16_t)block_align);
    put16(34, (uint16_t)bits_per_sample);
    memcpy(h + 36, "data", 4);
    put32(40, (uint32_t)pcm_bytes);
}

// --------------------------------------------------------------------------- //
// lifecycle
// --------------------------------------------------------------------------- //

ShimProtocol::ShimProtocol() {
    // Provisioning carrier: prefer NVS (runtime config), fall back to Kconfig.
    Settings settings("shim", false);
    shim_base_ = settings.GetString("shim_base");
    if (shim_base_.empty()) {
        shim_base_ = CONFIG_SHIM_BASE;
    }
    device_token_ = settings.GetString("device_token");
    if (device_token_.empty()) {
        device_token_ = CONFIG_SHIM_DEVICE_TOKEN;
    }
    capture_ms_ = CONFIG_SHIM_CAPTURE_MS;
    while (!shim_base_.empty() && shim_base_.back() == '/') {
        shim_base_.pop_back();
    }
}

ShimProtocol::~ShimProtocol() = default;

bool ShimProtocol::Start() {
    // HTTP is request/response; nothing persistent to connect here.
    return true;
}

bool ShimProtocol::OpenAudioChannel() {
    if (shim_base_.empty()) {
        ESP_LOGE(TAG, "no shim base configured (CONFIG_SHIM_BASE / NVS shim_base empty)");
        SetError("shim: no shim base URL");
        return false;
    }
    error_occurred_ = false;
    audio_channel_opened_ = true;
    finalizing_ = false;
    pending_frames_.clear();
    accumulated_ms_ = 0;
    server_sample_rate_ = 16000;
    server_frame_duration_ = OPUS_FRAME_DURATION_MS;
    correlation_id_ = NewCorrelationId();
    last_incoming_time_ = std::chrono::steady_clock::now();
    ESP_LOGI(TAG, "shim channel opened base=%s cid=%s", shim_base_.c_str(), correlation_id_.c_str());
    if (on_audio_channel_opened_) {
        on_audio_channel_opened_();
    }
    return true;
}

void ShimProtocol::CloseAudioChannel() {
    // Manual stop (2nd KEY press while listening): flush what we captured.
    if (audio_channel_opened_ && !finalizing_ && !pending_frames_.empty()) {
        FinalizeSession();
        return;
    }
    audio_channel_opened_ = false;
    if (on_audio_channel_closed_) {
        on_audio_channel_closed_();
    }
}

bool ShimProtocol::IsAudioChannelOpened() const {
    return audio_channel_opened_.load() && !error_occurred_ && !IsTimeout();
}

void ShimProtocol::SendStartListening(ListeningMode mode) {
    (void)mode;
    pending_frames_.clear();
    accumulated_ms_ = 0;
    finalizing_ = false;
    correlation_id_ = NewCorrelationId();
    last_incoming_time_ = std::chrono::steady_clock::now();

    // Discard any stale uplink opus left in the send queue from a previous
    // session so the new WAV starts clean.
    auto& audio = Application::GetInstance().GetAudioService();
    while (audio.PopPacketFromSendQueue() != nullptr) {
    }
    ESP_LOGI(TAG, "listening started cid=%s capture_ms=%d", correlation_id_.c_str(), capture_ms_);
}

void ShimProtocol::SendStopListening() {
    ESP_LOGI(TAG, "stop listening received");
    FinalizeSession();
}

bool ShimProtocol::SendAudio(std::unique_ptr<AudioStreamPacket> packet) {
    if (!audio_channel_opened_ || finalizing_) {
        return false;
    }
    int fd = packet->frame_duration > 0 ? packet->frame_duration : OPUS_FRAME_DURATION_MS;
    accumulated_ms_ += fd;
    PendingFrame f;
    f.sample_rate = packet->sample_rate;
    f.frame_duration = fd;
    f.payload = std::move(packet->payload);
    pending_frames_.push_back(std::move(f));
    last_incoming_time_ = std::chrono::steady_clock::now();

    if (accumulated_ms_ >= capture_ms_) {
        ESP_LOGI(TAG, "capture window reached (%d ms), finalizing", accumulated_ms_);
        FinalizeSession();
    }
    return true;
}

bool ShimProtocol::SendText(const std::string& text) {
    (void)text;  // no persistent text channel in request/response transport
    return true;
}

// --------------------------------------------------------------------------- //
// HTTP + uplink finalisation
// --------------------------------------------------------------------------- //

bool ShimProtocol::PostJson(const std::string& path, const std::string& body,
                            int& status_code, std::string& response) {
    auto& board = Board::GetInstance();
    auto network = board.GetNetwork();
    auto http = network->CreateHttp(0);
    if (http == nullptr) {
        ESP_LOGE(TAG, "failed to create http client");
        return false;
    }
    http->SetTimeout(60000);
    http->SetHeader("Content-Type", "application/json");
    http->SetHeader("Accept", "application/json");
    http->SetHeader("X-Correlation-Id", correlation_id_.c_str());
    http->SetHeader("Device-Id", SystemInfo::GetMacAddress().c_str());
    http->SetHeader("Client-Id", board.GetUuid().c_str());
    if (!device_token_.empty()) {
        std::string auth = "Bearer " + device_token_;
        http->SetHeader("Authorization", auth);
    }
    std::string content = body;
    http->SetContent(std::move(content));

    std::string url = shim_base_ + path;
    ESP_LOGI(TAG, "POST %s cid=%s body_bytes=%d", url.c_str(), correlation_id_.c_str(),
             (int)body.size());
    if (!http->Open("POST", url)) {
        ESP_LOGE(TAG, "open failed code=0x%x", http->GetLastError());
        return false;
    }
    status_code = http->GetStatusCode();
    response = http->ReadAll();
    http->Close();
    ESP_LOGI(TAG, "POST %s -> http=%d cid=%s resp_bytes=%d", path.c_str(), status_code,
             correlation_id_.c_str(), (int)response.size());
    return status_code == 200;
}

void ShimProtocol::FinalizeSession() {
    if (finalizing_) {
        return;
    }
    if (pending_frames_.empty()) {
        ESP_LOGW(TAG, "finalize requested with no captured audio");
        return;
    }
    finalizing_ = true;
    xTaskCreate(FinalizeTask, "shim_finalize", 4096 * 3, this, 5, nullptr);
}

void ShimProtocol::FinalizeTask(void* arg) {
    auto* self = static_cast<ShimProtocol*>(arg);
    self->RunFinalize();
    vTaskDelete(nullptr);
}

bool ShimProtocol::BuildWav(std::vector<uint8_t>& wav) {
    if (pending_frames_.empty()) {
        return false;
    }
    int sample_rate = pending_frames_[0].sample_rate > 0 ? pending_frames_[0].sample_rate : 16000;
    int frame_duration = pending_frames_[0].frame_duration > 0 ? pending_frames_[0].frame_duration
                                                               : OPUS_FRAME_DURATION_MS;

    OpusDecoderWrapper decoder(sample_rate, 1, frame_duration);
    std::vector<int16_t> pcm;
    for (auto& f : pending_frames_) {
        std::vector<int16_t> chunk;
        if (decoder.Decode(std::move(f.payload), chunk)) {
            pcm.insert(pcm.end(), chunk.begin(), chunk.end());
        }
    }
    pending_frames_.clear();

    if (pcm.empty()) {
        ESP_LOGE(TAG, "opus decode produced no pcm");
        return false;
    }
    int pcm_bytes = static_cast<int>(pcm.size() * sizeof(int16_t));
    AppendWavHeader(wav, sample_rate, 1, 16, pcm_bytes);
    wav.insert(wav.end(), reinterpret_cast<uint8_t*>(pcm.data()),
               reinterpret_cast<uint8_t*>(pcm.data()) + pcm_bytes);
    return true;
}

void ShimProtocol::RunFinalize() {
    // 1. Decode accumulated uplink opus -> WAV.
    std::vector<uint8_t> wav;
    if (!BuildWav(wav)) {
        ESP_LOGE(TAG, "cid=%s failed to build WAV", correlation_id_.c_str());
        finalizing_ = false;
        audio_channel_opened_ = false;
        if (on_network_error_) {
            on_network_error_("shim: no uplink audio");
        }
        return;
    }
    ESP_LOGI(TAG, "cid=%s wav bytes=%d", correlation_id_.c_str(), (int)wav.size());

    // 2. POST /v1/asr.
    std::string wav_b64 = Base64Encode(wav);
    cJSON* asr_req = cJSON_CreateObject();
    cJSON_AddStringToObject(asr_req, "audio", wav_b64.c_str());
    char* asr_json = cJSON_PrintUnformatted(asr_req);
    std::string asr_body(asr_json);
    cJSON_free(asr_json);
    cJSON_Delete(asr_req);

    int status = 0;
    std::string resp;
    if (!PostJson("/v1/asr", asr_body, status, resp)) {
        ESP_LOGE(TAG, "cid=%s /v1/asr failed http=%d", correlation_id_.c_str(), status);
        finalizing_ = false;
        audio_channel_opened_ = false;
        if (on_network_error_) {
            on_network_error_("shim: /v1/asr failed");
        }
        return;
    }

    std::string asr_text = JsonStringField(resp, "text");
    if (asr_text.empty()) {
        ESP_LOGE(TAG, "cid=%s /v1/asr empty text", correlation_id_.c_str());
        finalizing_ = false;
        audio_channel_opened_ = false;
        if (on_network_error_) {
            on_network_error_("shim: empty asr text");
        }
        return;
    }
    EmitStt(asr_text);

    // 3. POST /v1/chat.
    cJSON* chat_req = cJSON_CreateObject();
    cJSON_AddStringToObject(chat_req, "text", asr_text.c_str());
    char* chat_json = cJSON_PrintUnformatted(chat_req);
    std::string chat_body(chat_json);
    cJSON_free(chat_json);
    cJSON_Delete(chat_req);

    resp.clear();
    if (!PostJson("/v1/chat", chat_body, status, resp)) {
        ESP_LOGE(TAG, "cid=%s /v1/chat failed http=%d", correlation_id_.c_str(), status);
        finalizing_ = false;
        audio_channel_opened_ = false;
        if (on_network_error_) {
            on_network_error_("shim: /v1/chat failed");
        }
        return;
    }

    std::string audio_b64 = JsonStringField(resp, "audio");
    std::string display_text = JsonDisplayText(resp);

    // 4. Speak + display.
    EmitTtsStart();
    // Let the main task apply the state transition (stops mic + ResetDecoder)
    // before we start feeding the playback queue.
    vTaskDelay(pdMS_TO_TICKS(150));
    if (!display_text.empty()) {
        EmitTtsSentence(display_text);
    }

    std::vector<uint8_t> mp3;
    if (!audio_b64.empty() && Base64Decode(audio_b64, mp3) && !mp3.empty()) {
        ESP_LOGI(TAG, "cid=%s playing %d mp3 bytes", correlation_id_.c_str(), (int)mp3.size());
        Application::GetInstance().GetAudioService().PlayMp3(mp3);
    } else {
        ESP_LOGW(TAG, "cid=%s no playable mp3 in chat response", correlation_id_.c_str());
    }

    // 5. Done -> return the device to idle; playback drains in background.
    audio_channel_opened_ = false;
    finalizing_ = false;
    if (on_audio_channel_closed_) {
        on_audio_channel_closed_();
    }
}

void ShimProtocol::EmitTtsStart() {
    if (!on_incoming_json_) {
        return;
    }
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "type", "tts");
    cJSON_AddStringToObject(root, "state", "start");
    on_incoming_json_(root);
    cJSON_Delete(root);
}

void ShimProtocol::EmitTtsSentence(const std::string& text) {
    if (!on_incoming_json_) {
        return;
    }
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "type", "tts");
    cJSON_AddStringToObject(root, "state", "sentence_start");
    cJSON_AddStringToObject(root, "text", text.c_str());
    on_incoming_json_(root);
    cJSON_Delete(root);
}

void ShimProtocol::EmitStt(const std::string& text) {
    if (!on_incoming_json_) {
        return;
    }
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "type", "stt");
    cJSON_AddStringToObject(root, "text", text.c_str());
    on_incoming_json_(root);
    cJSON_Delete(root);
}