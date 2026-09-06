#ifndef WS_PROVISION_H
#define WS_PROVISION_H

// 配网直写 NVS 入口 (spec §6.3, 决策22). 设备服务器地址入口 = 直写 NVS namespace
// "websocket" 键 url/token/version + 短路激活; 自建 OTA 应答端点留二期。
// 写模式惯用法 `Settings settings("websocket", true)` (与 ota.cc 的 OTA 应答下发
// 写入面是两类不同的写入口 —— 本文件是配网直写面).
#include <string>
#include <esp_err.h>

class WsProvision {
public:
    // 生成 >=128-bit CSPRNG 设备弱 token (决策22: 全设备共享单 token, 配网时生成写入,
    // 服务端 constant-time 比对). 返回 48 字符 hex (192-bit), 满足 server/gateway/auth.py
    // TOKEN_MIN_CHARS=22 的下限.
    static std::string GenerateDeviceToken();

    // 配网直写 NVS namespace "websocket": url / token / version.
    // url = 自建网关 wss 地址; version = WS 协议版本 (契约 §四)。token 每次生成。
    static esp_err_t Provision(const std::string& url, const std::string& version);

    // 短路激活辅助: 保证 websocket 命名空间存在 token (配网时生成写入), 不访问
    // tenclass OTA 应答端点。url 由运维配网写入 NVS, 运行时由 websocket_protocol.cc 读取。
    static std::string EnsureWebsocketToken();
};

#endif // WS_PROVISION_H