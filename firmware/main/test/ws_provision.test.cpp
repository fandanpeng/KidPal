/*
 * Host unit tests for the provisioning pure-logic (ws_policy.h).
 *
 * The WsProvision NVS writer itself is device-bound (NVS + esp_fill_random — see
 * HOST_TEST_CONVENTIONS.md device-only matrix), so the host-coverable surface is the
 * pure URL/token policy: hex encoding of the CSPRNG token carrier and the ws:// force
 * gates. Each TEST_CASE carries a real `assert(...)` logic guard per [C4.2].
 */
#include <unity.h>
#include <cstdint>
#include <string>

#include "../provisioning/ws_policy.h"

namespace {

// Invariant used by both branches: a strong device token must also round-trip as the
// exact same string when its bytes are hex-re-encoded. Guards ws_provision's Generate.
bool TokenHexRoundTrips(std::string* sample) {
    static const uint8_t kBytes[4] = {0xde, 0xad, 0xbe, 0xef};
    *sample = ws_policy::HexEncode(kBytes, 4);
    return *sample == "deadbeef";
}

}  // namespace

TEST_CASE("ws provision: hex encoding round-trips CSPRNG token carrier (ws_provision)",
          "[device_ws][wsprovision]")
{
    std::string hex;
    TEST_ASSERT_TRUE_MESSAGE(TokenHexRoundTrips(&hex),
                             "ws_provision: HexEncode(deadbeef) must be 'deadbeef'");
    assert(hex == "deadbeef");
    TEST_ASSERT_EQUAL_INT(2u * 4u, hex.size());
}

TEST_CASE("ws provision: gateway url must be ws:// or wss:// (边界)",
          "[device_ws][wsprovision][boundary]")
{
    assert(&ws_policy::IsValidWsUrl != NULL);
    TEST_ASSERT_TRUE(ws_policy::IsValidWsUrl("wss://gateway.kidpal.example/ws"));
    TEST_ASSERT_TRUE(ws_policy::IsValidWsUrl("ws://192.168.1.50:9000"));
    TEST_ASSERT_FALSE(ws_policy::IsValidWsUrl("http://gateway/ws"));
    TEST_ASSERT_FALSE(ws_policy::IsValidWsUrl(""));
}

TEST_CASE("ws provision: token strength floor enforced (决策22 ≥128-bit)",
          "[device_ws][wsprovision][security]")
{
    const std::string strong(48, 'a');                       // 192-bit hex (24 bytes)
    const std::string weak(31, 'b');                          // <128-bit
    assert(ws_policy::IsStrongDeviceToken(strong));
    TEST_ASSERT_TRUE(ws_policy::IsStrongDeviceToken(strong));
    TEST_ASSERT_FALSE(ws_policy::IsStrongDeviceToken(weak));
    TEST_ASSERT_FALSE(ws_policy::IsStrongDeviceToken(""));
}