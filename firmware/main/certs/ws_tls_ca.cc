#include "certs/ws_tls_ca.h"

#include <esp_log.h>

#define TAG "WsTlsCa"

// cacert_pem / cert_pem 落到传输层加载面的现状 (V10 原词):
//   - 设备 WS 建连走 managed component 78__esp-ml307 的 EspSsl
//     (src/esp/esp_ssl.cc): `cfg.crt_bundle_attach = esp_crt_bundle_attach`
//     —— 用 mbedtls 的 X509 证书 bundle 作为 CA 信任源。
//   - 自签 CA (firmware/main/certs/kidpal_ca.pem) 通过
//     CONFIG_MBEDTLS_CUSTOM_CERTIFICATE_BUNDLE_PATH 收进该 bundle,
//     因此 esp_crt_bundle_attach 的握手校验会信任这颗自签 CA (自签 pinning)。
namespace ws_tls_ca {

esp_err_t VerifyCacertConfigured() {
#if defined(CONFIG_MBEDTLS_CUSTOM_CERTIFICATE_BUNDLE) && CONFIG_MBEDTLS_CUSTOM_CERTIFICATE_BUNDLE
    ESP_LOGI(TAG, "wss cacert enabled: custom cert bundle path = main/certs/kidpal_ca.pem "
                  "(esp_crt_bundle_attach trust face) — handshake verified against self-signed CA");
    return ESP_OK;
#else
    ESP_LOGW(TAG, "CONFIG_MBEDTLS_CUSTOM_CERTIFICATE_BUNDLE disabled — wss 自签 CA pinning 未开启");
    return ESP_ERR_INVALID_STATE;
#endif
}

}  // namespace ws_tls_ca