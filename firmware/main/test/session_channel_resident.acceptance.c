/*
 * Frozen acceptance tests — TaskPackage 261116-channel-resident-core (subsystem:
 * firmware; mergeOrder 77; rigor=full / riskTier=high; P1 of the
 * 261115-session-channel-resident train).
 *
 * 承载 story = S-F1.4（日程提醒：会话间隙提醒必达）+ requirements §2.1（结束三通道：
 * 60s 无语音超时 / 云端结束意图上游 / 再按 KEY）+ §2.3（录音态子态按键口径）+
 * S-F2.2-AC4（取消与三式收尾回归）。语义权威 = docs/architecture.md §6.4（通道常驻）
 * + ADR-019（会话生命周期与 WS 通道生命周期解耦）+ 契约 §四 listen / kidpal_keepalive。
 * 技术方案 = delivery/taskpackages/261116-channel-resident-core/plan.md「技术方案
 * （planner+architect 预评）」T1–T4 与「验收方向修订建议」1–9。
 *
 * 本文件是 P1 新增的 per-package 冻结验收（SCR-A1..A7 host 结构/行为半）。真机半
 * （轮 A A1–A13）归 human 持板证据，不做 host 伪造。
 *
 * 真机轮 A 修复轮（2026-11-17，human「三个问题都修」）追加 SCR-A8（D3 空素材会话空闲
 * 收尾 producer）。设备结果标记（前置升级闸触发输入，成对；路径须与 launch-card
 * `deviceOutcome.hostUncompilablePaths` 申报一致）:
 *   device-outcome: SCR-A8
 *   host-uncompilable-path: firmware/main/application.cc
 * 语义 = 用户可见结果（记录会话空素材静置 ~60s 自动回列表）落在 host 不可编面
 * application.cc 的主循环运行时；本文件对 SCR-A8 只钉源锚代理（结构），运行时行为
 * 归真机轮 B B-D3。
 *
 * ===========================================================================
 * Frozen seam contract pinned by this file（builder implements；本文件只读扫真实工作树
 * firmware/main/application.cc —— 结构齿 = 源锚代理，逐 byte 行为归真机轮）:
 *
 *   [S1] 收尾点收口（T1）：四个收尾点一律不再「拆通道 + 降 LOW_POWER + 5s 重建」，
 *        改投统一会话结束处理器。形态两类、不得混用：
 *        - 协议回调上下文 `Application::CloseAudioChannel()`（历史名保留，语义 = 会话
 *          收尾）→ 投事件位 `EndVoiceSession()`（不可直调 handler——跨任务不安全）；
 *        - 主循环上下文 `HandleToggleChatEvent` Listening 分支 / `WakeWordInvoke`
 *          Listening 分支 → 直调 `HandleEndVoiceSessionEvent()`（保同步语义）。
 *        四区共同不变量：区内 0 处 `protocol_->CloseAudioChannel(`、0 处
 *        `ScheduleResidentRebuild(`、0 处 `SetPowerSaveLevel(PowerSaveLevel::LOW_POWER)`；
 *        四个 `src=` 审计 token（discard_record / toggle_chat / overlay_close /
 *        wake_invoke）留在各自调用点、恒在世。
 *        needle 收窄说明（修订建议 1）：负锚用 `protocol_->CloseAudioChannel(` 而非
 *        `CloseAudioChannel(`——区切 helper 的 start needle 自身即 `...CloseAudioChannel()`，
 *        宽 needle 结构上不可满足（恒命中 start 自身）。
 *
 *   [S2] 统一处理器动作序（T1）：`HandleEndVoiceSessionEvent` 区 = 唯一收口点：
 *        `!protocol_` 早退 + `auto_connect_in_flight_` 守卫 + 三态门（Connecting /
 *        Listening / Speaking）逐字保留；Speaking 分支 `ResetDecoder()` + `AbortSpeaking`
 *        不动；`protocol_->SendStopListening()` 恰 1（替 `CloseAudioChannel()`）；
 *        `SetDeviceState(kDeviceStateIdle)` 保留；`SetPowerSaveLevel(LOW_POWER)` 与
 *        `ScheduleResidentRebuild()` 删除。
 *
 *   [S3] 五个真拆通道点保绿（must-stay-green）：HandleNetworkDisconnectedEvent /
 *        Reboot / UpgradeFirmware / SetAecMode / ResetProtocol 五区各 `protocol_->
 *        CloseAudioChannel()` 恰 1（既有语义零偏移）。
 *
 *   [S4] Idle 分支八件清位链逐条保留（must-stay-green）：EnableVoiceProcessing(false)
 *        / EnableWakeWordDetection(true) / kidpal_story_session_end /
 *        KidpalStorySessionEndSync / keybar 场景复位（REMINDER 在顶豁免：keyword
 *        `front_is_reminder`）/ kidpal_render_node_voice_end /
 *        kidpal_story_session_ctx_set(0) / RaiseReminderQueue。
 *
 *   [S5] 60s 无语音超时 producer（T2，落主循环 CLOCK_TICK 消费区，非纯模型事件）：
 *        命名常量 `kSessionIdleTimeoutS`（值 60）+ 判据四件（`kDeviceStateListening`
 *        && `!s_vad_speaking`（电平门，修订建议 3）&& clock_ticks_ >= kSessionIdleTimeoutS
 *        && !`kidpal_story_session_ctx_active()`（记录语境豁免））→
 *        `HandleEndVoiceSessionEvent()`（主循环同步）。说话沿重置：MAIN_EVENT_VAD_CHANGE
 *        区内 `clock_ticks_ = 0`（`if (s_vad_speaking)`）。
 *        must-stay-green 地雷：producer 只用命名常量 + 收尾调用，**不得**接上纯模型
 *        事件 `SES_EVT_SESSION_TIMEOUT_60S`（voice_round_ui.acceptance.c VQ-E1 tooth 2
 *        钉全 firmware/main 恰 2 文件）。
 *
 *   [S6] 心跳 no-op 分支（T3）：InitializeProtocol 的 OnIncomingJson lambda 内新增
 *        `kidpal_keepalive` 分支（直接 return），位次 = `#endif`（CONFIG_RECEIVE_CUSTOM_
 *        MESSAGE 块尾）之后、`Unknown message type` ESP_LOGW 之前（偏移量断言）。
 *
 *   [S7] 自愈巡检（T4，同 CLOCK_TICK 区）：判据 = `GetDeviceState() == kDeviceStateIdle`
 *        && `protocol_` && `!auto_connect_in_flight_` && `!protocol_->IsAudioChannelOpened()`
 *        → `TryStartAutoConnect()`。守卫短路顺序承重：`auto_connect_in_flight_` 偏移
 *        **先于** `IsAudioChannelOpened(`（修订建议 9 = 降频机制机判属性）。隔离：巡检
 *        区 0 处 `OpenAudioChannel(`（零第二条重连路径）。
 *
 * ---------------------------------------------------------------------------
 * 强度申报（契约条目 -> 承载断言 -> 性质档；acceptance-evidence T2 纪律）:
 *   §6.4 通道常驻 / ADR-019 解耦 -> SCR-A1（四收尾点负锚 + 正锚闭集）机制级（文本源锚
 *     代理）+ SCR-A3（五真拆通道点正锚）机制级; 行为本体（真实 run 时通道不拆、提醒
 *     仍达）= 真机轮 A4/A8。
 *   §2.1 60s 超时 -> SCR-A5 机制级（命名常量值 60 + 四判据 token + VAD 沿重置）源锚代理;
 *     实时「60s 首发生」= 真机轮 A6，「连续讲 >90s 不误收」= 轮 A A7（电平门为其离线
 *     可满足前提）。
 *   §2.3 录音态按键口径 / S-F2.2-AC4 -> SCR-A1（toggle / wake-invoke 区收口）+ SCR-A3
 *     （五真拆通道点）机制级; 端到端全链 = 真机轮 A5。
 *   契约 §四 kidpal_keepalive -> SCR-A6 机制级（字面 + 偏移位次 = 防 SD 日志噪声冲轮转）。
 *   契约 §四 listen（自愈巡检侧）-> SCR-A7 机制级（判据 token + 守卫短路序 + 零第二条
 *     重连路径负锚）。
 *   requirements §2.1「记录会话不适用 60s 超时」的收窄 / device-round-a-evidence §二
 *     -> SCR-A8 **源锚代理**（常量 60 + 定义位次 + 五判据 token + 审计 token + 零第二条
 *     重连路径 / 零 %10 负锚）; 行为本体（空素材记录会话静置 ~60s 自动回列表、零归档）
 *     落在 host 不可编面 firmware/main/application.cc（头注 device-outcome 标记），
 *     真机判据 = 轮 B B-D3。
 *   弱于契约处（显式缺口，不得当已兑现）:
 *     GAP-1 「四收尾点当刻不再拆通道 + 5s 不重建」的运行时观测 = 真机 KPLOG029 温通道
 *       进聆听 ≤0.82s（轮 A A1/A2）; host 仅钉源锚（结构）。
 *     GAP-2 「自愈巡检使 3 次退避变长跑（无上限）」形态 = P2 封顶长跑承接，本包只在
 *       crossPackageBehaviors 登记; 真机观测 = 轮 A A3/A12。
 *     GAP-3 IsTimeout ESP_LOGE 频率降级为事实降频（守卫短路）→ 未量测，真机轮带量测回报。
 *     GAP-4 SCR-A8 只钉源锚（结构）；「空素材记录会话静置 ~60s 自动回列表 + 零
 *       story_list / 零归档 + 随后普通问答恢复」运行时行为 = 轮 B B-D3（三源对账：
 *       设备 KPLOG `story_empty_session_timeout` + front 无 story_list 推送 + 引擎语境
 *       释放）。host 不可编面 = firmware/main/application.cc（头注 device-outcome）。
 *   行为等价假设（显式；不写即默认不成立）:
 *     EQ-1 「source-token 在世界 ≙ 行为兑现」：结构齿证明代码在场，真行为归轮 A。
 *     EQ-2 「区切 start needle 含自身 ≙ 负锚需收窄」：本文件所有以成员函数签名起切者，
 *       负锚一律用不含量于 start 自身的具体 needle（如 `protocol_->CloseAudioChannel(`）。
 * ===========================================================================
 */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>

/* -------------------------------------------------------------------------- *
 * 被扫文件（真实工作树; root 自手持 cwd 定位——evidence 类扫描齿 root 注入纪律，
 * 禁依赖调用点 cwd; test 目录不进扫描, 本文件自身注释合法提及 needle）。
 * ------------------------------------------------------------------------ */
#define SCR_APP_CC "firmware/main/application.cc"

#define SCR_MAX_PATH 1024
#define SCR_MAX_READ (8u * 1024u * 1024u)

static char scr_root[SCR_MAX_PATH];
static int scr_root_ready;

static int scr_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int scr_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../..", "../../../../../../.."
    };
    char probe[SCR_MAX_PATH];

    if (scr_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (scr_is_regular(probe)) {
            snprintf(scr_root, sizeof(scr_root), "%s", cands[i]);
            scr_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int scr_rel(char *out, size_t cap, const char *rel)
{
    if (!scr_locate_root()) {
        return 0;
    }
    snprintf(out, cap, "%s/%s", scr_root, rel);
    return 1;
}

static int scr_exists(const char *rel)
{
    char p[SCR_MAX_PATH];
    return scr_rel(p, sizeof(p), rel) && scr_is_regular(p);
}

static char *scr_read(const char *rel, size_t *out_len)
{
    char p[SCR_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!scr_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > SCR_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* logic guard: host OOM must fail loudly, not read as "red" */
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        free(buf);
        return NULL;
    }
    buf[sz] = '\0';
    fclose(f);
    if (out_len != NULL) {
        *out_len = (size_t)sz;
    }
    return buf;
}

static int scr_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int scr_count_in_slice(const char *slice, const char *needle)
{
    int count = 0;
    const char *h;
    size_t n;

    if (slice == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = slice; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    return count;
}

/* Slice of rel from the FIRST occurrence of start_needle up to (exclusive) the first
 * occurrence of end_needle AFTER it. Returns a malloc'd NUL-terminated slice or NULL.
 * Caller frees. end_needle == NULL -> slice to EOF. NOTE: the start needle itself is
 * INSIDE the slice (span = e - s) —— 以成员函数签名起切时负锚须收窄避开 start 自身。 */
static char *scr_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = scr_read(rel, NULL);
    const char *s, *e;
    char *slice;
    size_t span;

    if (buf == NULL) {
        return NULL;
    }
    s = strstr(buf, start_needle);
    if (s == NULL) {
        free(buf);
        return NULL;
    }
    if (end_needle == NULL) {
        e = buf + strlen(buf);
    } else {
        e = strstr(s + strlen(start_needle), end_needle);
        if (e == NULL) {
            e = buf + strlen(buf);
        }
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

/* printf-style helper（boot_auto_connect KAC 先例: 动态诊断格式化进静态缓冲再进单串宏）。 */
static char scr_fm_buf[1024];
static const char *scr_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *scr_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(scr_fm_buf, sizeof(scr_fm_buf), fmt, ap);
    va_end(ap);
    return scr_fm_buf;
}

#define SCR_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(scr_locate_root(), \
    "SCR RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* 收尾点区切 start/end 锚（与既有 boot_auto_connect RC-B2 / RC-B3 / voice_round_ui
 * OCE-A1 同族; 函数定义顺序是硬锚, builder 不得在四对相邻函数间插新函数）。 */
#define SCR_HELPER_START  "void Application::CloseAudioChannel()"
#define SCR_HELPER_END    "void Application::HandleToggleChatEvent()"
#define SCR_TOGGLE_START  "void Application::HandleToggleChatEvent()"
#define SCR_TOGGLE_END    "void Application::HandleStartListeningEvent()"
#define SCR_HANDLER_START "void Application::HandleEndVoiceSessionEvent()"
#define SCR_HANDLER_END   "void Application::HandleWakeWordDetectedEvent()"
#define SCR_WAKEINV_START "void Application::WakeWordInvoke("
#define SCR_WAKEINV_END   "bool Application::CanEnterSleepMode()"

/* =========================================================================== *
 * SCR-A1 — 四收尾点收口（负锚三件 + 正锚闭集 + src token 在世）。
 * 设计态红: 四区今天各自 protocol_->CloseAudioChannel() + SetPowerSaveLevel(LOW_POWER)
 *           + ScheduleResidentRebuild()（拆通道旧形态）。
 * =========================================================================== */
/* coverage: SCR-A1 -- 四收尾点（CloseAudioChannel helper / HandleToggleChatEvent Listening / HandleEndVoiceSessionEvent / WakeWordInvoke Listening）负锚：区内 protocol_->CloseAudioChannel( = 0 + ScheduleResidentRebuild( = 0 + SetPowerSaveLevel(PowerSaveLevel::LOW_POWER) = 0；正锚闭集：helper 区 EndVoiceSession(（事件位）、toggle/wake-invoke 区 HandleEndVoiceSessionEvent(（主循环直调）、handler 区 SendStopListening(（收口动作）；四 src 审计 token 各自在区 */
TEST_CASE("会话通道常驻: 四收尾点收口负锚 (SCR-A1)",
          "[session_channel_resident][SCR-A1][structural]")
{
    static const struct {
        const char *start;
        const char *end;
        const char *label;
        const char *src_token;
    } sites[] = {
        { SCR_HELPER_START, SCR_HELPER_END, "CloseAudioChannel helper", "src=discard_record" },
        { SCR_TOGGLE_START, SCR_TOGGLE_END, "ToggleChat Listening", "src=toggle_chat" },
        { SCR_HANDLER_START, SCR_HANDLER_END, "EndVoiceSession handler", "src=overlay_close" },
        { SCR_WAKEINV_START, SCR_WAKEINV_END, "WakeWordInvoke Listening", "src=wake_invoke" },
    };
    char *region;
    int n;
    size_t i;

    SCR_REQUIRE_ROOT();

    for (i = 0; i < sizeof(sites) / sizeof(sites[0]); i++) {
        region = scr_region_between(SCR_APP_CC, sites[i].start, sites[i].end);
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            scr_fm("SCR-A1 RED(infra): %s 区切取失败——函数定义顺序锚漂移须复核"
                   "（禁改名绕开; 禁在相邻函数间插新函数）", sites[i].label));

        /* 负锚 1：区内不再直调协议层拆通道（needle 收窄 = 修订建议 1）。 */
        n = scr_count_in_slice(region, "protocol_->CloseAudioChannel(");
        TEST_ASSERT_TRUE_MESSAGE(n == 0,
            scr_fm("SCR-A1 RED: %s 区 protocol_->CloseAudioChannel( = %d 处（冻结 0）"
                   "——收尾点仍拆通道（会话结束 = 拆通道旧形态未收口; §6.4 / ADR-019）",
                   sites[i].label, n));

        /* 负锚 2：区内不再排常驻重建（B helper 整条退役）。 */
        n = scr_count_in_slice(region, "ScheduleResidentRebuild(");
        TEST_ASSERT_TRUE_MESSAGE(n == 0,
            scr_fm("SCR-A1 RED: %s 区 ScheduleResidentRebuild( = %d 处（冻结 0）——收尾点"
                   "仍排 5s 常驻重建（断而再建绕路未除; plan T1）", sites[i].label, n));

        /* 负锚 3：区内不再降 LOW_POWER（通道在世, 省电剖面维持）。 */
        n = scr_count_in_slice(region, "SetPowerSaveLevel(PowerSaveLevel::LOW_POWER)");
        TEST_ASSERT_TRUE_MESSAGE(n == 0,
            scr_fm("SCR-A1 RED: %s 区 SetPowerSaveLevel(PowerSaveLevel::LOW_POWER) = %d "
                   "处（冻结 0）——收尾降省电档（RC-B6 观测口径重定: 通道仍在世）",
                   sites[i].label, n));

        /* 正锚：收口动作/投递在世。helper 区走 EndVoiceSession(；toggle/wake-invoke 区
         * 走 HandleEndVoiceSessionEvent()；handler 区（= 收口点本体）走 SendStopListening(。
         * helper/toggle/wake-invoke 用「二选一闭集」，兼容 builder 统一投事件位或统一直调。 */
        if (strcmp(sites[i].label, "EndVoiceSession handler") == 0) {
            TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "SendStopListening("),
                "SCR-A1 RED: HandleEndVoiceSessionEvent 区零 SendStopListening(——收口点"
                "未发 listen stop（会话结束未通知 front 收尾; 契约 §四 listen）");
        } else {
            TEST_ASSERT_TRUE_MESSAGE(
                scr_contains(region, "EndVoiceSession(") ||
                    scr_contains(region, "HandleEndVoiceSessionEvent()"),
                scr_fm("SCR-A1 RED: %s 区零收口投递（EndVoiceSession( 与 "
                       "HandleEndVoiceSessionEvent() 皆无）——收尾点未接统一会话结束处理器"
                       "（plan T1 收口形态缺位）", sites[i].label));
        }

        /* src 审计 token 在世（T1: 四字面 token 恒在世; 诊断口径区分四路）。 */
        TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, sites[i].src_token),
            scr_fm("SCR-A1 RED: %s 区缺审计 token %s——收尾诊断口径丢失（四 src 字面"
                   "token 须留在各自调用点; plan T1）", sites[i].label, sites[i].src_token));

        free(region);
    }
}

/* =========================================================================== *
 * SCR-A2 — 统一处理器动作序（SendStopListening 恰 1 + ResetDecoder 恰 1 在 Speaking
 *          分支内 + 三态门 / auto_connect_in_flight_ / !protocol_ 逐字在世）。
 * =========================================================================== */
/* coverage: SCR-A2 -- HandleEndVoiceSessionEvent 统一处理器：protocol_->SendStopListening() 恰 1 + ResetDecoder() 恰 1 且位次在 Speaking 分支内（state == kDeviceStateSpeaking < ResetDecoder < SendStopListening）+ 三态门（kDeviceStateConnecting/Listening/Speaking）+ auto_connect_in_flight_ 守卫 + !protocol_ 早退逐字在世（261024 语义零偏移） */
TEST_CASE("会话通道常驻: 统一处理器动作序 (SCR-A2)",
          "[session_channel_resident][SCR-A2][structural]")
{
    char *region;
    const char *p_speak, *p_reset, *p_stop;
    int n;

    SCR_REQUIRE_ROOT();

    region = scr_region_between(SCR_APP_CC, SCR_HANDLER_START, SCR_HANDLER_END);
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "SCR-A2 RED(infra): HandleEndVoiceSessionEvent 区切取失败——锚漂移须复核");

    /* 收口动作 = SendStopListening()（恰 1）。 */
    n = scr_count_in_slice(region, "SendStopListening()");
    TEST_ASSERT_TRUE_MESSAGE(n == 1,
        scr_fm("SCR-A2 RED: HandleEndVoiceSessionEvent 区 SendStopListening() = %d 处"
               "（冻结恰 1）——统一收口未发 listen stop（0 = 根因现状; >1 = 双发）", n));

    /* ResetDecoder() 恰 1（即时静音）。 */
    n = scr_count_in_slice(region, "ResetDecoder()");
    TEST_ASSERT_TRUE_MESSAGE(n == 1,
        scr_fm("SCR-A2 RED: HandleEndVoiceSessionEvent 区 ResetDecoder() = %d 处（冻结"
               "恰 1）——Speaking 分支即时静音缺失或重复", n));

    /* ResetDecoder 在 Speaking 分支内：state == kDeviceStateSpeaking 位次 < ResetDecoder
     * 位次 < SendStopListening 位次（后者住分支外 = 上界）。 */
    p_speak = strstr(region, "state == kDeviceStateSpeaking");
    p_reset = strstr(region, "ResetDecoder()");
    p_stop = strstr(region, "SendStopListening()");
    TEST_ASSERT_TRUE_MESSAGE(p_speak != NULL && p_reset != NULL && p_stop != NULL &&
                                 p_speak < p_reset && p_reset < p_stop,
        "SCR-A2 RED: ResetDecoder() 未落在 state == kDeviceStateSpeaking 分支内"
        "（位次须 kDeviceStateSpeaking < ResetDecoder < SendStopListening）——即时静音"
        "脱离 Speaking 门（plan T1: 仅 Speaking 复位解码队列）");

    /* 三态门 + in-flight 守卫 + !protocol_ 早退逐字在世（261024 语义零偏移）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        scr_contains(region, "kDeviceStateConnecting") &&
            scr_contains(region, "kDeviceStateListening") &&
            scr_contains(region, "kDeviceStateSpeaking"),
        "SCR-A2 RED: HandleEndVoiceSessionEvent 三态门不全（Connecting/Listening/"
        "Speaking 缺一）——统一收口的三态门被拆（261024 overlay_close 时序自证破）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "auto_connect_in_flight_"),
        "SCR-A2 RED: HandleEndVoiceSessionEvent 区零 auto_connect_in_flight_ 守卫——"
        "与后台 AutoConnect 抢 websocket_ 的竞态入口（plan T1 守卫逐字保留）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "!protocol_"),
        "SCR-A2 RED: HandleEndVoiceSessionEvent 区零 !protocol_ 早退——空协议指针"
        "解引用风险（plan T1 早退逐字保留）");

    free(region);
}

/* =========================================================================== *
 * SCR-A3 — 五个真拆通道点保绿（must-stay-green；既有语义零偏移）。
 * =========================================================================== */
/* coverage: SCR-A3 -- 五个真拆通道点（HandleNetworkDisconnectedEvent / Reboot / UpgradeFirmware / SetAecMode / ResetProtocol）各区 protocol_->CloseAudioChannel() 恰 1（must-stay-green: 真拆通道点不动, local_close_pending_ 分类零误重连不变量不动） */
TEST_CASE("会话通道常驻: 五个真拆通道保绿 (SCR-A3)",
          "[session_channel_resident][SCR-A3][structural]")
{
    static const struct {
        const char *start;
        const char *end;
        const char *label;
    } points[] = {
        { "void Application::HandleNetworkDisconnectedEvent()",
          "void Application::HandleActivationDoneEvent()", "NetworkDisconnected" },
        { "void Application::Reboot()",
          "bool Application::UpgradeFirmware(", "Reboot" },
        { "bool Application::UpgradeFirmware(",
          "void Application::WakeWordInvoke(", "UpgradeFirmware" },
        { "void Application::SetAecMode(",
          "void Application::PlaySound(", "SetAecMode" },
        { "void Application::ResetProtocol()", NULL, "ResetProtocol" },
    };
    char *region;
    int n;
    size_t i;

    SCR_REQUIRE_ROOT();

    for (i = 0; i < sizeof(points) / sizeof(points[0]); i++) {
        region = scr_region_between(SCR_APP_CC, points[i].start, points[i].end);
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            scr_fm("SCR-A3 RED(infra): %s 区切取失败——锚漂移须复核（禁整函数退役绕过）",
                   points[i].label));
        n = scr_count_in_slice(region, "protocol_->CloseAudioChannel()");
        TEST_ASSERT_TRUE_MESSAGE(n == 1,
            scr_fm("SCR-A3 RED: %s 区 protocol_->CloseAudioChannel() = %d 处（冻结恰 1）"
                   "——真拆通道点被误动（must-stay-green; 网络断/重启/OTA/AEC/Reset 五路"
                   "真拆通道语义零偏移破）", points[i].label, n));
        free(region);
    }
}

/* =========================================================================== *
 * SCR-A4 — Idle 分支八件清位链逐条保留（must-stay-green）。
 * =========================================================================== */
/* coverage: SCR-A4 -- HandleStateChangedEvent 的 case kDeviceStateIdle 区八件清位链各 >=1：EnableVoiceProcessing(false) / EnableWakeWordDetection(true) / kidpal_story_session_end / KidpalStorySessionEndSync / keybar 场景复位 kidpal_keybar_scene_reset( + REMINDER 在顶豁免 front_is_reminder / kidpal_render_node_voice_end / kidpal_story_session_ctx_set(0) / RaiseReminderQueue */
TEST_CASE("会话通道常驻: Idle八件清位链保绿 (SCR-A4)",
          "[session_channel_resident][SCR-A4][structural]")
{
    char *region;

    SCR_REQUIRE_ROOT();

    region = scr_region_between(SCR_APP_CC, "case kDeviceStateIdle:",
                                "case kDeviceStateConnecting:");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "SCR-A4 RED(infra): case kDeviceStateIdle 区切取失败——锚漂移须复核");

    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "EnableVoiceProcessing(false)"),
        "SCR-A4 RED: Idle 区零 EnableVoiceProcessing(false)——麦克风口径不变破");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "EnableWakeWordDetection(true)"),
        "SCR-A4 RED: Idle 区零 EnableWakeWordDetection(true)——唤醒检测恢复破");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "kidpal_story_session_end"),
        "SCR-A4 RED: Idle 区零 kidpal_story_session_end——[R2] 录音页态复位破"
        "（record_session_repair RSR-A2 邻居）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "KidpalStorySessionEndSync"),
        "SCR-A4 RED: Idle 区零 KidpalStorySessionEndSync——[R5] 镜像同步破（RSL2-A2）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "kidpal_keybar_scene_reset(") &&
                                 scr_contains(region, "front_is_reminder"),
        "SCR-A4 RED: Idle 区缺 keybar 场景复位 / REMINDER 在顶豁免判据——"
        "chrome_ghost CG-A1（鬼影修复 + 提醒驻留不熄）被顺删");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "kidpal_render_node_voice_end"),
        "SCR-A4 RED: Idle 区零 kidpal_render_node_voice_end——OQ3=c 前台回落破");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "kidpal_story_session_ctx_set(0)"),
        "SCR-A4 RED: Idle 区零 kidpal_story_session_ctx_set(0)——记录语境清位破");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "RaiseReminderQueue"),
        "SCR-A4 RED: Idle 区零 RaiseReminderQueue——S-F1.4 录音态提醒排队例外（轮 A A8）"
        "收尾回 Idle 立刻弹的既有离线齿（reminder_overlay_unify RM-A3 tooth 5）被顺删");

    free(region);
}

/* =========================================================================== *
 * SCR-A5 — 60s 无语音超时 producer（CLOCK_TICK 消费区 + VAD 沿重置）。
 * 设计态红: 纯模型 ses_machine_dispatch 生产调用点 = 0（真机从未 60s 自动关闭）。
 * =========================================================================== */
/* coverage: SCR-A5 -- 60s 无语音超时 producer（requirements §2.1）：命名常量 kSessionIdleTimeoutS 定义值 60 + CLOCK_TICK 消费区判据四件（kDeviceStateListening / !s_vad_speaking 电平门 / kidpal_story_session_ctx_active() 记录语境豁免 / HandleEndVoiceSessionEvent() 主循环直调）+ MAIN_EVENT_VAD_CHANGE 区 clock_ticks_ = 0 说话沿重置（不接纯模型 SES_EVT_SESSION_TIMEOUT_60S） */
TEST_CASE("会话通道常驻: 60s超时producer+说话沿重置 (SCR-A5)",
          "[session_channel_resident][SCR-A5][structural]")
{
    char *clock_region, *vad_region, *def_region;

    SCR_REQUIRE_ROOT();

    clock_region = scr_region_between(SCR_APP_CC, "if (bits & MAIN_EVENT_CLOCK_TICK)",
                                      "void Application::HandleNetworkConnectedEvent()");
    TEST_ASSERT_NOT_NULL_MESSAGE(clock_region,
        "SCR-A5 RED(infra): CLOCK_TICK 消费区切取失败——锚漂移须复核");

    /* 命名常量 kSessionIdleTimeoutS，定义值 60（首定义片段含 60, RC-B2 tooth2 先例形态）。 */
    def_region = scr_region_between(SCR_APP_CC, "kSessionIdleTimeoutS", ";");
    TEST_ASSERT_NOT_NULL_MESSAGE(def_region,
        "SCR-A5 RED: application.cc 无 kSessionIdleTimeoutS 定义——60s 超时命名常量"
        "未落（纯模型无 producer = 根因现状; plan T2）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(def_region, "60"),
        "SCR-A5 RED: kSessionIdleTimeoutS 首定义片段不含 60——阈值偏离 requirements §2.1"
        "（60s 无语音; 偏离回 planner 重冻结）");
    free(def_region);

    /* 判据四件。 */
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(clock_region, "kDeviceStateListening"),
        "SCR-A5 RED: CLOCK_TICK 区无 kDeviceStateListening 判据——非聆听态误计时");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(clock_region, "!s_vad_speaking"),
        "SCR-A5 RED: CLOCK_TICK 区无 !s_vad_speaking 电平门——仅靠 VAD 沿重置会把"
        "无停顿长讲（90s 单上升沿）在第 60 秒误收（轮 A A7 直接翻车; 修订建议 3）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(clock_region, "kidpal_story_session_ctx_active()"),
        "SCR-A5 RED: CLOCK_TICK 区无 kidpal_story_session_ctx_active() 记录语境豁免——"
        "记录会话被 60s 误收（requirements §2.1 v1.65 硬语义）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(clock_region, "HandleEndVoiceSessionEvent()"),
        "SCR-A5 RED: CLOCK_TICK 区无 HandleEndVoiceSessionEvent() 投递——producer 未接"
        "统一收口（纯模型 producer = 0 的根因现状）");
    free(clock_region);

    /* 说话沿重置：VAD_CHANGE 区 clock_ticks_ = 0（plan T2 说话沿重置）。 */
    vad_region = scr_region_between(SCR_APP_CC, "if (bits & MAIN_EVENT_VAD_CHANGE)",
                                    "if (bits & MAIN_EVENT_SCHEDULE)");
    TEST_ASSERT_NOT_NULL_MESSAGE(vad_region,
        "SCR-A5 RED(infra): VAD_CHANGE 区切取失败——锚漂移须复核");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(vad_region, "clock_ticks_ = 0"),
        "SCR-A5 RED: MAIN_EVENT_VAD_CHANGE 区无 clock_ticks_ = 0——说话沿不重置计时"
        "（孩子一开口，60s 计时不清零 = 误收）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(vad_region, "s_vad_speaking"),
        "SCR-A5 RED: MAIN_EVENT_VAD_CHANGE 区无 s_vad_speaking 引用——说话沿重置未"
        "按电平判据门控（plan T2 if (s_vad_speaking) clock_ticks_ = 0）");
    free(vad_region);
}

/* =========================================================================== *
 * SCR-A6 — 心跳 no-op 分支（kidpal_keepalive），位次 = #endif 之后、WARN 之前。
 * 设计态红: OnIncomingJson 无 kidpal_keepalive 分支（每 30s 心跳落 Unknown WARN,
 *          按 ~144KB/日 冲 SD 日志轮转预算）。
 * =========================================================================== */
/* coverage: SCR-A6 -- 契约 §四 kidpal_keepalive：OnIncomingJson lambda 区 kidpal_keepalive 字面 >=1，且位次在 #endif（CONFIG_RECEIVE_CUSTOM_MESSAGE 块尾）之后、Unknown message type ESP_LOGW 之前（偏移量断言 = 防心跳落 WARN 刷 SD 日志; plan T3） */
TEST_CASE("会话通道常驻: 心跳noop分支位次 (SCR-A6)",
          "[session_channel_resident][SCR-A6][structural]")
{
    char *region;
    const char *p_keep, *p_end, *p_warn;

    SCR_REQUIRE_ROOT();

    region = scr_region_between(SCR_APP_CC, "protocol_->OnIncomingJson(",
                                "void Application::Alert(");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "SCR-A6 RED(infra): OnIncomingJson lambda 区切取失败——锚漂移须复核");

    p_keep = strstr(region, "kidpal_keepalive");
    TEST_ASSERT_TRUE_MESSAGE(p_keep != NULL,
        "SCR-A6 RED: OnIncomingJson 区零 kidpal_keepalive——心跳下行帧落 Unknown "
        "message type WARN（每 30s 一条, 冲 SD 日志轮转预算 ~144KB/日; 契约 §四; plan T3）");

    /* 位次：先于 Unknown message type WARN。 */
    p_warn = strstr(region, "Unknown message type");
    TEST_ASSERT_TRUE_MESSAGE(p_warn != NULL && p_keep != NULL && p_keep < p_warn,
        "SCR-A6 RED: kidpal_keepalive 分支未先于 Unknown message type WARN——心跳仍落 "
        "else 分支（WARN 噪声未除）");

    /* 位次：#endif（CONFIG_RECEIVE_CUSTOM_MESSAGE 块尾）之后。区内首个 #endif 即该块
     * 尾（lambda 内更早无条件编译）。 */
    p_end = strstr(region, "#endif");
    TEST_ASSERT_TRUE_MESSAGE(p_end != NULL && p_keep != NULL && p_keep > p_end,
        "SCR-A6 RED: kidpal_keepalive 分支未在 #endif 之后——落在 "
        "CONFIG_RECEIVE_CUSTOM_MESSAGE 条件块内（该配置关时心跳分支消失 = WARN 噪声回归; "
        "修订建议 8）");

    free(region);
}

/* =========================================================================== *
 * SCR-A7 — 自愈巡检（CLOCK_TICK 区 IsAudioChannelOpened + TryStartAutoConnect +
 *          守卫短路序 auto_connect_in_flight_ < IsAudioChannelOpened + 零第二条重连路径）。
 * 设计态红: 通道常驻后会话间隙无下行 → 120s 判死必然到期（首按键失同步第一段根因）。
 * =========================================================================== */
/* coverage: SCR-A7 -- 自愈巡检（§6.4）：CLOCK_TICK 区 IsAudioChannelOpened( >=1 + TryStartAutoConnect( >=1 + 守卫 auto_connect_in_flight_ 且其偏移 < IsAudioChannelOpened( 偏移（降频机制机判属性; 修订建议 9）+ 区内 OpenAudioChannel( = 0（零第二条重连路径; 巡检不直连不新建 task） */
TEST_CASE("会话通道常驻: 自愈巡检短路序 (SCR-A7)",
          "[session_channel_resident][SCR-A7][structural]")
{
    char *region;
    const char *p_inflight, *p_opened;
    int n;

    SCR_REQUIRE_ROOT();

    region = scr_region_between(SCR_APP_CC, "if (bits & MAIN_EVENT_CLOCK_TICK)",
                                "void Application::HandleNetworkConnectedEvent()");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "SCR-A7 RED(infra): CLOCK_TICK 消费区切取失败——锚漂移须复核");

    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "IsAudioChannelOpened("),
        "SCR-A7 RED: CLOCK_TICK 区零 IsAudioChannelOpened(——自愈巡检未判通道开合"
        "（TECH-261012 首按键失同步第一段根因未修）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(region, "TryStartAutoConnect("),
        "SCR-A7 RED: CLOCK_TICK 区零 TryStartAutoConnect(——自愈巡检未接既有重连任务"
        "（§6.4 常驻自愈未接线）");

    /* 守卫短路序：auto_connect_in_flight_ 偏移先于 IsAudioChannelOpened(（降频: in-flight
     * 期短路掉 IsTimeout ESP_LOGE 每秒一条）。 */
    p_inflight = strstr(region, "auto_connect_in_flight_");
    p_opened = strstr(region, "IsAudioChannelOpened(");
    TEST_ASSERT_TRUE_MESSAGE(p_inflight != NULL && p_opened != NULL &&
                                 p_inflight < p_opened,
        "SCR-A7 RED: 巡检守卫短路序破（auto_connect_in_flight_ 须先于 "
        "IsAudioChannelOpened(）——in-flight 期未短路, IsTimeout ESP_LOGE 每秒一条刷屏"
        "（修订建议 9 降频机制; plan T4）");

    /* 零第二条重连路径：巡检区不直连 OpenAudioChannel(（只经 TryStartAutoConnect）。 */
    n = scr_count_in_slice(region, "OpenAudioChannel(");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        scr_fm("SCR-A7 RED: 巡检区出现 OpenAudioChannel( = %d 处——绕过 AutoConnect task "
               "直连（第二条重连路径; 守卫与退避被旁路; plan T4 铁律）", n));

    free(region);
}

/* =========================================================================== *
 * SCR-A8 — 空素材会话空闲收尾 producer（D3；CLOCK_TICK 区）。
 * 设计态红: 记录语境会话说不出任何素材时零收尾 producer —— 设备常驻 Listening，
 *          静置 2 分钟不结束（device-round-a-evidence §二 22:51:03→22:53:24）。
 * 设备结果标记见本文件头注（device-outcome / host-uncompilable-path 成对）：本 case
 * 只钉源锚代理，运行时行为归轮 B B-D3。
 * =========================================================================== */
/* coverage: SCR-A8 -- 空素材空闲收尾 producer（D3）：命名常量 kStoryEmptySessionTimeoutS 首现片段含 60 + 定义位次在 kSessionIdleTimeoutS 之后 + CLOCK_TICK 区 kidpal_story_session_ctx_active() >=2（恰一处非否定 = 新 producer 正向记录语境门）+ !s_story_rec_entered >=1（有素材不命中）+ !s_vad_speaking >=2 + HandleEndVoiceSessionEvent() >=2 + 审计 token story_empty_session_timeout + 区内 OpenAudioChannel( = 0 + 区内无 clock_ticks_ % 10；交叉负锚 = kSessionIdleTimeoutS 首现仍在原定义行（新常量注释不得写回该字面，否则 SCR-A5 假红） */
TEST_CASE("会话通道常驻: 空素材空闲收尾producer (SCR-A8)",
          "[session_channel_resident][SCR-A8][structural]")
{
    char *clock_region;
    char *def_region;
    char *full;
    const char *p_idle_def, *p_empty_def, *line_start, *line_end;
    int n;

    SCR_REQUIRE_ROOT();

    clock_region = scr_region_between(SCR_APP_CC, "if (bits & MAIN_EVENT_CLOCK_TICK)",
                                      "void Application::HandleNetworkConnectedEvent()");
    TEST_ASSERT_NOT_NULL_MESSAGE(clock_region,
        "SCR-A8 RED(infra): CLOCK_TICK 消费区切取失败——锚漂移须复核");

    /* 命名常量 kStoryEmptySessionTimeoutS：首现到下一分号片段含 60（SCR-A5 同形态）。 */
    def_region = scr_region_between(SCR_APP_CC, "kStoryEmptySessionTimeoutS", ";");
    TEST_ASSERT_NOT_NULL_MESSAGE(def_region,
        "SCR-A8 RED: application.cc 无 kStoryEmptySessionTimeoutS —— 空素材空闲收尾"
        "常量未落（记录语境空素材会话零收尾 producer = 静置不结束的根因现状; "
        "device-round-a-evidence §二; plan D3）");
    TEST_ASSERT_TRUE_MESSAGE(scr_contains(def_region, "60"),
        "SCR-A8 RED: kStoryEmptySessionTimeoutS 首现片段不含 60——阈值偏离 "
        "requirements §2.1 单一 60s 口径（plan D3 阈值决策 = 60，不引入第二魔数）");
    free(def_region);

    /* 常量定义位次 + 交叉负锚（新常量不得抢 SCR-A5 的 kSessionIdleTimeoutS 首现区）。 */
    full = scr_read(SCR_APP_CC, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(full,
        "SCR-A8 RED(infra): application.cc 读取失败——源锚代理无输入");
    p_idle_def = strstr(full, "kSessionIdleTimeoutS");
    p_empty_def = strstr(full, "kStoryEmptySessionTimeoutS");
    TEST_ASSERT_TRUE_MESSAGE(p_idle_def != NULL && p_empty_def != NULL &&
                                 p_empty_def > p_idle_def,
        "SCR-A8 RED: kStoryEmptySessionTimeoutS 未定义在 kSessionIdleTimeoutS 之后"
        "——常量区位次漂移（plan D3: 定义在既有 60s 常量之后；也防 SCR-A5 的"
        "「kSessionIdleTimeoutS 首现到下一分号」区切被抢）");

    if (p_idle_def != NULL) {
        size_t line_len;
        char *line;

        line_start = p_idle_def;
        while (line_start > full && line_start[-1] != '\n') {
            line_start--;
        }
        line_end = p_idle_def;
        while (*line_end != '\0' && *line_end != '\n') {
            line_end++;
        }
        line_len = (size_t)(line_end - line_start);
        line = (char *)malloc(line_len + 1);
        TEST_ASSERT_NOT_NULL(line); /* logic guard: host OOM must fail loudly */
        memcpy(line, line_start, line_len);
        line[line_len] = '\0';
        TEST_ASSERT_TRUE_MESSAGE(
            scr_contains(line, "static const int kSessionIdleTimeoutS") &&
                scr_contains(line, "60"),
            scr_fm("SCR-A8 RED(交叉负锚): kSessionIdleTimeoutS 首现行 = [%s]（须为原"
                   "定义行 static const int kSessionIdleTimeoutS = 60）——首现点漂到"
                   "非定义行 = 新常量/注释写回了该字面（SCR-A5 的「首现到下一分号」"
                   "区切被抢 → SCR-A5 阈值断言假红）", line));
        free(line);
    }
    free(full);

    /* 新 producer 判据（与既有 60s 无语音 producer 共存于同区）。 */
    n = scr_count_in_slice(clock_region, "kidpal_story_session_ctx_active()");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        scr_fm("SCR-A8 RED: CLOCK_TICK 区 kidpal_story_session_ctx_active() = %d 处"
               "（冻结 >=2）——空素材收尾缺「记录语境会话」正向门（既有 60s producer "
               "一处 + 新 producer 一处; 非记录语境不得命中）", n));

    n = scr_count_in_slice(clock_region, "!kidpal_story_session_ctx_active()");
    TEST_ASSERT_TRUE_MESSAGE(n == 1,
        scr_fm("SCR-A8 RED: CLOCK_TICK 区 !kidpal_story_session_ctx_active() = %d 处"
               "（冻结恰 1）——既有 60s 无语音 producer 的记录语境豁免须唯一；新 "
               "producer 用正向门，复制成否定门 = 空素材收尾永不命中", n));

    n = scr_count_in_slice(clock_region, "!s_story_rec_entered");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        scr_fm("SCR-A8 RED: CLOCK_TICK 区 !s_story_rec_entered = %d 处（冻结 >=1）——"
               "空素材收尾缺「有素材不命中」门（VAD 起音沿置位、Idle 清位的 file-static"
               "素材信号; 无此门 = 连续讲 >90s 被误收，轮 A A7 同族翻车）", n));

    n = scr_count_in_slice(clock_region, "!s_vad_speaking");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        scr_fm("SCR-A8 RED: CLOCK_TICK 区 !s_vad_speaking = %d 处（冻结 >=2）——新 "
               "producer 缺 VAD 电平门（与 SCR-A5 同承重件：无停顿长讲只有一个上升沿，"
               "仅靠沿重置会在阈值秒误收）", n));

    n = scr_count_in_slice(clock_region, "HandleEndVoiceSessionEvent()");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        scr_fm("SCR-A8 RED: CLOCK_TICK 区 HandleEndVoiceSessionEvent() = %d 处（冻结 "
               ">=2）——空素材收尾未接统一收口处理器（只打日志不收尾 = 设备仍 Listening）",
               n));

    TEST_ASSERT_TRUE_MESSAGE(scr_contains(clock_region, "story_empty_session_timeout"),
        "SCR-A8 RED: CLOCK_TICK 区缺审计 token story_empty_session_timeout——空素材"
        "收尾不可判别（轮 B B-D3 三源对账的设备侧唯一可观测证据）");

    n = scr_count_in_slice(clock_region, "OpenAudioChannel(");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        scr_fm("SCR-A8 RED: CLOCK_TICK 区出现 OpenAudioChannel( = %d 处——空素材收尾"
               "不得直连建连（零第二条重连路径; SCR-A7 同族铁律）", n));

    n = scr_count_in_slice(clock_region, "clock_ticks_ % 10");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        scr_fm("SCR-A8 RED: CLOCK_TICK 区出现 clock_ticks_ %% 10 = %d 处——diag_log "
               "DLG-A4⑤ 负锚被破（分频不得复用 clock_ticks_，与 60s 判据互相干扰）", n));

    free(clock_region);
}

/* ===========================================================================
 * RED baseline 判读口径（freeze 相位）:
 *   本件因登记权不在 acceptance-author（CMakeLists 由 builder 落登记行），freeze 相位
 *   不进 host runner 全量跑；以 build/compile_commands.json 的编译 flags 对
 *   session_channel_resident.acceptance.c 单独 -fsyntax-only 实编证明可编译
 *   （DEV-GOVERNANCE 验收测试纪律 §4 C 类冻结验收实编）。逐条设计态红映射见交付报告；
 *   runner 级红由 builder 首次登记后补跑 / mutation 兜底，不豁免 mutation。
 * =========================================================================== */