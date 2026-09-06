#ifndef WS_POLICY_H
#define WS_POLICY_H

// 配网直写的纯逻辑 (host-testable, 无 IDF/NVS 依赖)。WS 命名空间 url/token 校验与
// token hex 编码常驻此头, 供 provisioning/ws_provision.cc (device) 与 host 单测共用。

#include <cstddef>
#include <cstdint>
#include <string>

namespace ws_policy {

// 决策22 强度地板: >=128-bit => 至少 32 个 hex 字符 (16 字节)。配网生成用 24 字节
// (192-bit) 双满足 server/gateway/auth.py TOKEN_MIN_CHARS=22。
inline constexpr size_t kDeviceTokenMinHexLen = 32;

// Raw 随机字节 -> 小写 hex。用于把 CSPRNG 输出的设备 token 编码为可持久化字符串。
inline std::string HexEncode(const uint8_t* bytes, size_t len) {
    static const char kHex[] = "0123456789abcdef";
    std::string out;
    out.reserve(len * 2);
    for (size_t i = 0; i < len; i++) {
        out.push_back(kHex[(bytes[i] >> 4) & 0x0f]);
        out.push_back(kHex[bytes[i] & 0x0f]);
    }
    return out;
}

// 网关 URL 校验 (配网直写写入前): 必须非空且为 ws:// 或 wss://。
inline bool IsValidWsUrl(const std::string& url) {
    return url.rfind("ws://", 0) == 0 || url.rfind("wss://", 0) == 0;
}

// 设备弱 token 强度校验 (决策22 >=128-bit): hex 长度 >= 32 个字符。
inline bool IsStrongDeviceToken(const std::string& token) {
    return token.size() >= kDeviceTokenMinHexLen;
}

}  // namespace ws_policy

#endif  // WS_POLICY_H