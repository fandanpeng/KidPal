#ifndef WS_TLS_CA_H
#define WS_TLS_CA_H

// V10 (260902-arch-review-kidpal-spec-v0.2): 固件 SSL 层证书加载 + 自签 CA pinning.
// 加载面 = esp_ssl 的 cacert_pem/cert_pem。真自签 CA 由 live 相位注入
// (G 包签发制品, >=10 年, V25) 替换本树 certs/kidpal_ca.pem; 本包先落占位声明 +
// 加载路径, 验收 A3 只断言「CA 文件在树 + 加载路径在树 + 握手校验开启的代码路径」。
#include <esp_err.h>

namespace ws_tls_ca {

// 校验自签 CA 已通过 CONFIG_MBEDTLS_CUSTOM_CERTIFICATE_BUNDLE 编入
// esp_crt_bundle_attach 信任面 (EspSsl 建连默认走该 bundle)。返回 ESP_OK 表示
// wss 握手将按嵌入的自签 CA 开启证书校验 (pinning 代码路径已接通)。
esp_err_t VerifyCacertConfigured();

}  // namespace ws_tls_ca

#endif  // WS_TLS_CA_H