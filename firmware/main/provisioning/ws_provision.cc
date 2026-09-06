#include "provisioning/ws_provision.h"
#include "provisioning/ws_policy.h"

#include "settings.h"
#include "assets/lang_config.h"

#include <esp_log.h>
#include <esp_random.h>
#include <cstring>

#define TAG "WsProvision"

// 配网时的 token 随机字节数 (192-bit = 48 hex chars; 决策22 要求 >=128-bit,
// server/gateway/auth.py TOKEN_MIN_CHARS=22 下限=约132-bit —— 192-bit 双满足)。
#define WS_TOKEN_RANDOM_BYTES 24

std::string WsProvision::GenerateDeviceToken() {
    uint8_t bytes[WS_TOKEN_RANDOM_BYTES];
    // CSPRNG (决策22, spec §6.3): 配网时生成写入 NVS, 不嵌真实 token.
    esp_fill_random(bytes, sizeof(bytes));

    std::string token = ws_policy::HexEncode(bytes, sizeof(bytes));
    ESP_LOGI(TAG, "generated device token (%d-bit random, hex)", (int)(sizeof(bytes) * 8));
    return token;
}

esp_err_t WsProvision::Provision(const std::string& url, const std::string& version) {
    // 校验网关地址形状 (ws:// 或 wss://) 才写入, 防止把非法地址推进硬伤.
    if (!url.empty() && !ws_policy::IsValidWsUrl(url)) {
        ESP_LOGE(TAG, "invalid websocket gateway url rejected: %s", url.c_str());
        return ESP_ERR_INVALID_ARG;
    }

    // 配网直写 NVS namespace "websocket": url / token / version.
    Settings settings("websocket", true);

    if (!url.empty()) {
        settings.SetString("url", url);
    }
    std::string token = GenerateDeviceToken();
    settings.SetString("token", token);
    int version_int = 0;
    if (!version.empty()) {
        version_int = std::atoi(version.c_str());
    }
    if (version_int != 0) {
        settings.SetInt("version", version_int);
    }
    ESP_LOGI(TAG, "provisioned websocket namespace: url=%s version=%d token=<%d hex>",
             url.empty() ? "(keep existing)" : url.c_str(), version_int, (int)token.size());
    return ESP_OK;
}

std::string WsProvision::EnsureWebsocketToken() {
    Settings read("websocket", false);
    std::string token = read.GetString("token");
    if (!token.empty()) {
        return token;
    }

    // token 缺失 => 短路激活兜底生成写入 (配网直写入口).
    Settings settings("websocket", true);
    token = GenerateDeviceToken();
    settings.SetString("token", token);
    return token;
}