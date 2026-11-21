/*
 * ===========================================================================
 * 冻结验收 — TaskPackage 261117-session-end-intent-longretry（P2，workstream
 * 261115-session-channel-resident）；SEI-A1..A4 + SEI-C1 注册序守卫。
 * acceptance-author 冻结，builder/fixer 只读；tighten-only 修订窗口外不得改断言语义。
 *
 * device-outcome: SEI-A1 SEI-A2 SEI-A3 SEI-A4
 * host-uncompilable-path: firmware/main/application.cc
 * host-uncompilable-path: firmware/main/mcp_server.cc
 * （成对申报；运行时本体落在 host 不可编面，真机轮 B 由 human 持板验收；本文件在
 *  host 侧把行为钉成可机判的源锚 + 结构不变量。）
 *
 * ---------------------------------------------------------------------------
 * FROZEN OFFLINE CONTRACT（builder MUST implement；本包所有实现落在设备 TU，
 * host 构建只跑结构源锚 —— acceptance-evidence「源锚代理」性质档）
 *
 *   [S1] 设备 control end_session 消费（home = mcp_server.cc McpServer::DoTool 的
 *        control 分派 `switch (a)`）：
 *        `case 0`（= kidpal_display_control_action("end_session") == 0，映射值由
 *        RDG-F1 / K0PTS-F1 独立钉死，本包不改 view_router 映射）内**恰一次**
 *        `EndVoiceSession(`（事件位投递，跨任务安全；不得直调
 *        `HandleEndVoiceSessionEvent()`），且**零** `AbortSpeaking(`（设备不得双重
 *        收尾——统一处理器内部处理 Speaking 分支）、**零** `SendStopListening(`
 *        （收口动作只能由统一处理器发，设备 case 不得自建第二收口路径）。
 *        `case 1`（stop_speaking）既有 `AbortSpeaking(` **恰一** 保绿（must-stay-green）。
 *        回引：契约 §1.4 end_session 表行 + PROPOSAL T5 + ADR-019 ⑥。
 *
 *   [S2] 退避表形态（home = application.cc 文件顶部常量区）：
 *        `kAutoConnectBackoffS` 首定义片段值序 5 < 10 < 20 < 40 < 60（五档递增秒序列）；
 *        追加命名常量 `kAutoConnectSteadyRetryS`，首定义片段含 60（稳态长跑周期）。
 *        **放弃钉数组长度字面**（值序即隐含五元素，见 REVISION 注：K6-A2 tooth7 重钉）。
 *        回引：arch §6.4「退避表 {5,10,20} + 3 次即止改为 {5,10,20,40,60} 后恒 60 秒」
 *        + PROPOSAL T1 改法 4 + ADR-019 ④。
 *
 *   [S3] 长跑与退出（home = application.cc Application::TryStartAutoConnect 函数区
 *        含后台 task lambda）：
 *        - 无 `attempt < 3` / `attempt < 2` 字面（3 次封顶退役）；
 *        - 消费 `kAutoConnectBackoffS` 与 `kAutoConnectSteadyRetryS`（长跑走表 + 稳态）；
 *        - 三个退出条件各在世：① 成功（`OpenAudioChannel()` 返回真 -> break）；
 *          ② bail（每次尝试前重查守卫条件——`GetDeviceState() != kDeviceStateIdle`
 *          / `IsAudioChannelOpened()`）；③ **网络断退出请求位**（`network_exit_requested_`：
 *          置位在 HandleNetworkDisconnectedEvent 函数体三态门之外、task 依此退出、
 *          HandleNetworkConnectedEvent Idle 支路清位）；
 *        - `auto_connect_in_flight_` 所有退出路径必清（K6-A4 既有不变量保绿；本文件
 *          再钉「清点位于 task 段、先清 in-flight 再投补投」的次序）。
 *        回引：arch §6.4 自动重连段 + ADR-019 ④。
 *
 *   [S4] 人工意向位（home = application.h 成员 + application.cc 五个人工入口区 +
 *        TryStartAutoConnect task 段）：离线期手工入口不再静默 no-op。
 *        - `application.h` 新增成员 `EventBits_t pending_manual_intent_ = 0`（直接存
 *          `MAIN_EVENT_*` 位；最后写者胜，不做 OR 累积）；
 *        - 五个人工入口代码点：HandleToggleChatEvent / HandleStartListeningEvent /
 *          HandleEndVoiceSessionEvent / HandleWakeWordDetectedEvent / WakeWordInvoke
 *          ——各自 in-flight 分支内**置 `pending_manual_intent_` + 留审计行
 *          `manual_intent_queued` + 仍 return**（不得改成裸 no-op，也不得新增
 *          `return;` 以外的裸吞）；
 *        - task 段：查 `pending_manual_intent_` -> 截短退避；连上后经 `Schedule(`
 *          按入口来源补投事件位；补投前**先清 `auto_connect_in_flight_`**（否则补投
 *          被自己吞掉）；只在成功路径补投，bail / 断网退出清而不投。
 *        回引：arch §6.4「长跑配套人工意向位」+ ADR-019 ④ + PROPOSAL T1 改法 4。
 *
 *   [S5] 命名缝契约裁定（显式申报；沿用 K6 [S3] `kAutoConnectBackoffS` 先例）：
 *        本冻结件需要三个可机判的稳定标识符 ——
 *          `pending_manual_intent_`（[S4] 意向位成员）、
 *          `network_exit_requested_`（[S3] 网络断退出请求位成员）、
 *          `manual_intent_queued`（[S4] 五入口审计行 token）。
 *        这是「源文本计数断言」得以成立的缝契约，与 K6 判「命名常量是本冻结件的缝
 *        契约裁定」同款。builder 若判定必须换名/换形态，回 planner 重冻结；本 Pod 内
 *        不得自行改名绕过（改名 = 结构假红，不是新实现）。
 *
 * ---------------------------------------------------------------------------
 * 强度申报（契约条目 -> 承载断言 -> 性质档；acceptance-evidence T2 纪律）:
 *   契约 §1.4 end_session 表行（截获转发类 + 设备收口统一处理器）
 *     -> SEI-A1 **源锚代理**（case 0 区内 EndVoiceSession( 恰 1 / AbortSpeaking( 恰 0 /
 *        SendStopListening( 恰 0 + case 1 保绿）。行为本体「说拜拜真的结束会话、麦克风
 *        关、按 OQ3=c 回落、通道仍常驻」= 真机轮 B B1。
 *   arch §6.4 封顶长跑 / ADR-019 ④
 *     -> SEI-A2 **机制级**（值序 + 稳态常量）+ SEI-A3 **源锚代理**（无封顶字面 + 三
 *        退出条件各在世 + 清位次序）。行为本体「拔网线后 5/10/20/40/60 后稳态 60s、
 *        恢复自动重连、离线 30 分钟电量差」= 真机轮 B B4/B7。
 *   arch §6.4 人工意向位 / ADR-019 ④ / PROPOSAL T1 改法 4
 *     -> SEI-A4 **源锚代理**（五入口置位 + 审计行 + task 截短/补投/次序）。行为本体
 *        「离线期按 KEY 不再静默 no-op、连上后接续」= 真机轮 B B8。
 *   requirements §2.1 结束通道①②③ / S-F1.4 离线期必达 -> 由 SEI-A1/A3/A4 的源锚 +
 *     真机轮 B B1/B4/B6 端到端承载。
 *
 *   弱于契约处（显式缺口，不得当已兑现）:
 *     GAP-1 [S1] 只钉「设备 case 0 调用统一处理器」的源形态；「EndVoiceSession 事件位
 *       在主循环被 HandleEndVoiceSessionEvent 真正收口、麦克风关、页按 OQ3=c 回落」的
 *       运行时观测 = 真机轮 B B1（三源日志：设备 KPLOG listen stop 上行 + front 收尾 +
 *       引擎语境释放）。host 不可编面 = firmware/main/application.cc / mcp_server.cc。
 *     GAP-2 [S2]/[S3] 钉值序/常量与「无封顶字面」的机判形态，不钉真实时序（5s/10s/…
 *       的真实秒级节奏在 host 无真实时钟——与 K6 [S3] 同口径，时序归真机窗 B4）。
 *     GAP-3 [S4] 钉「置位 + 审计行 + 补投接线」的源形态；「补投后孩子那一次动作真的
 *       接续（按 KEY 起 listens / 喊唤醒词进聆听 / 结束会话收口）」= 真机轮 B8。
 *     GAP-4 意向位**不设时效**（planner 2026-11-19 裁决 b：忠于 arch §6.4「孩子那一次
 *       动作不丢」字面）；超长离线后补投的观感项 = 真机轮严格观测项，本文件不断言。
 *
 *   行为等价假设（显式；不写即默认不成立）:
 *     EQ-1 「source-token 在世界 ≙ 行为兑现」：结构断言证明代码在场，真行为归轮 B。
 *     EQ-2 「区切 start needle 含自身 ≙ 负锚需收窄」：凡以成员函数签名/switch 片段起切
 *       者，负锚只能用不含量于 start 自身或相邻 case 的具体 needle（见各 case 注释）。
 *     EQ-3 「values 5<10<20<40<60 位序 ≙ 决策表值 = 该五档」：位序断言对 「50/100/…」
 *       这类子串巧合不设防（本包值域固定，且 K6-A2 tooth5/6 与真机 B4 双承载）。
 *     EQ-4 「task lambda 内 break ≙ 退出该次尝试」：无需分辨成功退出/重试循环尾；
 *       三退出条件的在场证明以「各自判据 token + 位次」承载。
 *
 * ---------------------------------------------------------------------------
 * 断言的变异敏感点（mutationRequired=true；执行归 builder、证据归 planner 代落）:
 *   SEI-A1：把 case 0 改回 `AbortSpeaking(kAbortReasonNone)`（旧现状）-> SEI-A1 红
 *     （EndVoiceSession( 0 / AbortSpeaking( 1）；把 case 0 写成 EndVoiceSession() +
 *     保留 AbortSpeaking -> SEI-A1 红（AbortSpeaking( 1）；改 case 1（stop_speaking）
 *     的 AbortSpeaking -> SEI-A1 保绿齿红；case 0 直调 HandleEndVoiceSessionEvent()
 *     -> SEI-A1 红（EndVoiceSession( 0）。
 *   SEI-A2：表改回 {5,10,20} -> 红（缺 40/60 位序）；摘 kAutoConnectSteadyRetryS
 *     或值 != 60 -> 红；表改 {60,40,20,10,5}（降序）-> 红（位序破）。
 *   SEI-A3：恢复 `attempt < 3` -> 红；摘网络断退出请求位（置位/查位/清位任一）-> 红；
 *     把 task 改成有界 3 次 -> 红；清点前移到 xTaskCreate 之前 -> SEI-A3 次序齿红
 *     （并连带 K6-A4 既有齿）。
 *   SEI-A4：五入口任一改回裸 return（不置位）-> SEI-A4 红（该入口区 pending 缺）；
 *     任一入口删审计行 -> 红（manual_intent_queued 缺）；task 段先投补投再清
 *     in-flight -> 红（次序齿）；完全不补投（无 Schedule） -> 红。
 *
 * ---------------------------------------------------------------------------
 * fixture 自含性声明:
 *   - 零外部资产 / 零时间真值 / 零环境前提：全部断言直读仓内真实工作树文本
 *     （firmware/main/application.cc / application.h / mcp_server.cc /
 *     host_app/main/CMakeLists.txt）。仓根由手持 cwd 上溯候选探针定位（root 注入
 *     纪律，禁依赖调用点 cwd）。
 *   - 被扫面不含本文件自身（本文件住 firmware/main/test/ 下，其注释合法提及 needle，
 *     但不进任何被扫文件）。
 *   - 依赖被测文件在场非空 = infra 红（不静默跳过）。
 *
 * Run protocol:
 *   通道①（gate fallback，注册后随 host runner 全量执行）:
 *     wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *     cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
 *   通道②（standalone 结构 harness，未注册前独立红基线；见文件尾 RED baseline 段）:
 *     cd firmware/main/test/acceptance/red-baseline
 *     gcc -std=c11 -Wall -Wextra -I. -o /tmp/sei_red_runner \
 *         ../../session_end_intent_longretry.acceptance.c runner.c
 *     /tmp/sei_red_runner   （exit code = 失败 TEST_CASE 数）
 * =========================================================================== */

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
 * 被扫文件（真实工作树；root 自手持 cwd 定位，禁依赖调用点 cwd）。
 * ------------------------------------------------------------------------ */
#define SEI_APP_CC   "firmware/main/application.cc"
#define SEI_APP_H    "firmware/main/application.h"
#define SEI_MCP_CC   "firmware/main/mcp_server.cc"
#define SEI_HOST_CML "firmware/main/test/host_app/main/CMakeLists.txt"

#define SEI_MAX_PATH 1024
#define SEI_MAX_READ (8u * 1024u * 1024u)

/* 冻结 needle（本文件单源；值 = 2026-11-19 冻结基线逐 needle 实证）:
 *   - application.cc `void Application::TryStartAutoConnect()`（缝函数；task lambda 在内）。
 *   - application.h `auto_connect_in_flight_`（P1/K6 既有）+ `pending_manual_intent_`（本包）。
 *   - mcp_server.cc control 分派 `switch (a)`（:1736 起）内 `case 0:` -> `case 1:`。 */
#define SEI_SEAM_FN_ND       "void Application::TryStartAutoConnect()"
#define SEI_FLAG_ND          "auto_connect_in_flight_"
#define SEI_INTENT_ND        "pending_manual_intent_"
#define SEI_NET_EXIT_ND      "network_exit_requested_"
#define SEI_BACKOFF_ND       "kAutoConnectBackoffS"
#define SEI_STEADY_ND        "kAutoConnectSteadyRetryS"
#define SEI_AUDIT_ND         "manual_intent_queued"
#define SEI_ENDVS_ND         "EndVoiceSession("
#define SEI_ABORT_ND         "AbortSpeaking("
#define SEI_STOPLIS_ND       "SendStopListening("
#define SEI_SWITCH_START     "int a = kidpal_display_control_action(action.c_str());"
#define SEI_SWITCH_END       "case 2:"
#define SEI_DISC_START       "void Application::HandleNetworkDisconnectedEvent()"
#define SEI_DISC_END         "void Application::HandleActivationDoneEvent()"
#define SEI_CONN_START       "void Application::HandleNetworkConnectedEvent()"
#define SEI_CONN_END         "void Application::HandleNetworkDisconnectedEvent()"

static char sei_root[SEI_MAX_PATH];
static int sei_root_ready;

static int sei_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int sei_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../..", "../../../../../../.."
    };
    char probe[SEI_MAX_PATH];

    if (sei_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (sei_is_regular(probe)) {
            snprintf(sei_root, sizeof(sei_root), "%s", cands[i]);
            sei_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int sei_rel(char *out, size_t cap, const char *rel)
{
    if (!sei_locate_root()) {
        return 0;
    }
    if (strcmp(sei_root, ".") == 0) {
        snprintf(out, cap, "%s", rel);
    } else {
        snprintf(out, cap, "%s/%s", sei_root, rel);
    }
    return 1;
}

static int sei_file_contains(const char *rel, const char *needle)
{
    char p[SEI_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;
    int found;

    if (!sei_rel(p, sizeof(p), rel) || !sei_is_regular(p)) {
        return 0;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return 0;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > SEI_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return 0;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* logic guard: host OOM must fail loudly, not read as "red" */
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        free(buf);
        return 0;
    }
    buf[sz] = '\0';
    fclose(f);
    found = strstr(buf, needle) != NULL;
    free(buf);
    return found;
}

static char *sei_read(const char *rel, size_t *out_len)
{
    char p[SEI_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!sei_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > SEI_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int sei_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int sei_count_in_slice(const char *slice, const char *needle)
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

/* Slice of rel from FIRST occurrence of start_needle up to (exclusive) the first
 * occurrence of end_needle AFTER it. end_needle == NULL -> to EOF. Caller frees.
 * NOTE: start needle is INSIDE the slice. */
static char *sei_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = sei_read(rel, NULL);
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

/* Member-function region: from start_needle to the next "\n<sig> Application::"
 * (void/bool/int or bare). Caller frees. */
static char *sei_member_fn(const char *rel, const char *start_needle)
{
    char *buf = sei_read(rel, NULL);
    const char *s;
    const char *e = NULL;
    static const char *const sigs[] = {
        "\nvoid Application::", "\nbool Application::", "\nint Application::",
        "\nApplication::"
    };
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
    for (size_t i = 0; i < sizeof(sigs) / sizeof(sigs[0]); i++) {
        const char *hit = strstr(s + strlen(start_needle), sigs[i]);
        if (hit != NULL && (e == NULL || hit < e)) {
            e = hit;
        }
    }
    if (e == NULL) {
        e = buf + strlen(buf);
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

/* Copy span [start, end) into a fresh NUL-terminated buffer. Caller frees. */
static char *sei_span(const char *start, const char *end)
{
    size_t span;
    char *slice;

    if (start == NULL || end == NULL || end < start) {
        return NULL;
    }
    span = (size_t)(end - start);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, start, span);
    slice[span] = '\0';
    return slice;
}

static char sei_fm_buf[1024];
static const char *sei_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *sei_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(sei_fm_buf, sizeof(sei_fm_buf), fmt, ap);
    va_end(ap);
    return sei_fm_buf;
}

#define SEI_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(sei_locate_root(), \
    "SEI RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* =========================================================================== *
 * SEI-C1 — 注册序守卫（build-wiring guard，非 story 断言；smoke 恒末位）。
 * =========================================================================== */
/* coverage: SEI-C1 -- 本冻结件已登记进 HOST_ACCEPTANCE_SRCS 且位次先于 host_infra_smoke.acceptance.c（smoke 末位 = 最先执行; 包 0 boundary 齿注册序守卫）。登记行缺失即红——builder FIRST ACTION。 */
TEST_CASE("会话结束意图: 注册序守卫 (SEI-C1)",
          "[session_end_intent][SEI-C1][structural]")
{
    char *cml;
    const char *p_self, *p_smoke;

    SEI_REQUIRE_ROOT();

    cml = sei_read(SEI_HOST_CML, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(cml,
        "SEI-C1 RED(infra): host_app/main/CMakeLists.txt 读取失败——构建清单不可达");

    p_self = strstr(cml, "../../session_end_intent_longretry.acceptance.c");
    TEST_ASSERT_TRUE_MESSAGE(p_self != NULL,
        "SEI-C1 RED: HOST_ACCEPTANCE_SRCS 无本冻结件登记行"
        "（\"../../session_end_intent_longretry.acceptance.c\"）——builder FIRST ACTION: "
        "登记后本件才进 host runner（缺行 = 本件全部断言静默不跑）");

    p_smoke = strstr(cml, "../../host_infra_smoke.acceptance.c");
    TEST_ASSERT_TRUE_MESSAGE(p_smoke != NULL,
        "SEI-C1 RED(infra): host_infra_smoke.acceptance.c 登记行缺失——smoke 末位约定被破坏");
    TEST_ASSERT_TRUE_MESSAGE(p_self < p_smoke,
        "SEI-C1 RED: 本冻结件登记行未置于 host_infra_smoke.acceptance.c 之前——注册序纪律"
        "（smoke 恒末位 = 最先执行）被破坏");

    free(cml);
}

/* =========================================================================== *
 * SEI-A1 — 设备 control end_session 收口统一处理器（case 0 恰 1 EndVoiceSession，
 *          零 AbortSpeaking / 零 SendStopListening；case 1 stop_speaking 保绿）。
 * =========================================================================== */
/* coverage: SEI-A1 -- mcp_server.cc control 分派 switch(a) 区：case 0（end_session）内 EndVoiceSession( 恰 1 + AbortSpeaking( 恰 0 + SendStopListening( 恰 0（设备不得双重收尾 / 不得自建第二收口路径）；case 1（stop_speaking）区 AbortSpeaking( 恰 1（must-stay-green）。回引契约 §1.4 end_session 表行 + PROPOSAL T5 + ADR-019 ⑥ */
TEST_CASE("会话结束意图: 设备case0收口统一处理器 (SEI-A1)",
          "[session_end_intent][SEI-A1][structural]")
{
    char *region = NULL;
    char *case0 = NULL;
    char *case1 = NULL;
    const char *c0, *c1, *c2;
    int n;

    SEI_REQUIRE_ROOT();

    region = sei_region_between(SEI_MCP_CC, SEI_SWITCH_START, SEI_SWITCH_END);
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "SEI-A1 RED(infra): mcp_server.cc control 分派区切取失败——`int a = "
        "kidpal_display_control_action(action.c_str());` -> `case 2:` 锚漂移须复核"
        "（禁整 switch 退役绕过）");
    TEST_ASSERT_TRUE_MESSAGE(sei_contains(region, "end_session"),
        "SEI-A1 RED(infra): control 分派区无 `end_session` 语义标注——case 0 语义锚漂移"
        "（case 0 的 end_session 身份由 RDG-F1/K0PTS-F1 钉死的映射值 0 保证；此处须保留"
        "可读标注，改注释请保留 end_session 字样）");

    c0 = strstr(region, "case 0:");
    c1 = c0 != NULL ? strstr(c0, "case 1:") : NULL;
    c2 = c1 != NULL ? strstr(c1, "case 2:") : NULL;
    TEST_ASSERT_TRUE_MESSAGE(c0 != NULL && c1 != NULL,
        "SEI-A1 RED(infra): control 分派区无 case 0 / case 1 子区——动作分派形态漂移须复核");
    (void)c2;

    case0 = sei_span(c0, c1);
    TEST_ASSERT_NOT_NULL_MESSAGE(case0, "SEI-A1 RED(infra): case 0 子区切取失败");

    /* 正锚: 恰一次 EndVoiceSession(（事件位投递统一处理器）。 */
    n = sei_count_in_slice(case0, SEI_ENDVS_ND);
    TEST_ASSERT_TRUE_MESSAGE(n == 1,
        sei_fm("SEI-A1 RED: case 0（end_session）内 EndVoiceSession( = %d 处（冻结恰 1）"
               "——设备收口未接统一处理器（0 = 现状只 AbortSpeaking；>1 = 重复投递）", n));

    /* 负锚: 零 AbortSpeaking(（设备不得双重收尾 —— 统一处理器内部处理 Speaking 分支）。 */
    n = sei_count_in_slice(case0, SEI_ABORT_ND);
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        sei_fm("SEI-A1 RED: case 0（end_session）内 AbortSpeaking( = %d 处（冻结恰 0）"
               "——设备双重收尾（统一处理器已内部处理 Speaking 分支，case 不得另调）", n));

    /* 负锚: 零 SendStopListening(（收口动作只能由统一处理器发）。 */
    n = sei_count_in_slice(case0, SEI_STOPLIS_ND);
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        sei_fm("SEI-A1 RED: case 0（end_session）内 SendStopListening( = %d 处（冻结恰 0）"
               "——设备自建第二收口路径（listen stop 只能经统一处理器恰一次）", n));

    free(case0);

    /* must-stay-green: case 1（stop_speaking）既有 AbortSpeaking( 恰 1。 */
    case1 = sei_span(c1, c2 != NULL ? c2 : c1 + strlen(c1));
    TEST_ASSERT_NOT_NULL_MESSAGE(case1, "SEI-A1 RED(infra): case 1 子区切取失败");
    n = sei_count_in_slice(case1, SEI_ABORT_ND);
    TEST_ASSERT_TRUE_MESSAGE(n == 1,
        sei_fm("SEI-A1 RED: case 1（stop_speaking）内 AbortSpeaking( = %d 处（既有恰 1，"
               "must-stay-green）——云端确认停播补充语义被误改", n));

    free(case1);
    free(region);
}

/* =========================================================================== *
 * SEI-A2 — 退避表形态（值序 5<10<20<40<60 + 稳态常量 60）。
 * =========================================================================== */
/* coverage: SEI-A2 -- application.cc kAutoConnectBackoffS 首定义片段值序 5<10<20<40<60（五档递增秒序列，放弃钉数组长度字面）+ kAutoConnectSteadyRetryS 首定义片段含 60（稳态长跑）；两符号各 >=2 处（定义 + task 内消费）。回引 arch §6.4 + PROPOSAL T1 改法 4 + ADR-019 ④ */
TEST_CASE("会话结束意图: 封顶长跑退避表形态 (SEI-A2)",
          "[session_end_intent][SEI-A2][structural]")
{
    char *def = NULL;
    const char *p5, *p10, *p20, *p40, *p60;
    int n;

    SEI_REQUIRE_ROOT();

    /* 退避表首定义片段值序（不钉排版、不钉数组长度字面）。 */
    def = sei_region_between(SEI_APP_CC, SEI_BACKOFF_ND, ";");
    TEST_ASSERT_NOT_NULL_MESSAGE(def,
        "SEI-A2 RED(infra): kAutoConnectBackoffS 首定义片段切取失败——命名常量缺席"
        "（[S2]/[S5] 缝契约；定义须为该符号在 application.cc 的首次出现）");
    p5 = strstr(def, "5");
    p10 = strstr(def, "10");
    p20 = strstr(def, "20");
    p40 = strstr(def, "40");
    p60 = strstr(def, "60");
    TEST_ASSERT_TRUE_MESSAGE(p5 != NULL && p10 != NULL && p20 != NULL && p40 != NULL &&
                                 p60 != NULL,
        "SEI-A2 RED: kAutoConnectBackoffS 首定义缺档位（须含 5/10/20/40/60 全五档；"
        "缺一 = 封顶长跑退避表未落或仍是 3 档旧表）");
    TEST_ASSERT_TRUE_MESSAGE(p5 < p10 && p10 < p20 && p20 < p40 && p40 < p60,
        "SEI-A2 RED: kAutoConnectBackoffS 值序非 5<10<20<40<60 递增——退避形态偏离冻结"
        "值（arch §6.4: {5,10,20,40,60} 递增后稳态）");
    free(def);

    /* 稳态长跑常量值 60。 */
    def = sei_region_between(SEI_APP_CC, SEI_STEADY_ND, ";");
    TEST_ASSERT_NOT_NULL_MESSAGE(def,
        "SEI-A2 RED: kAutoConnectSteadyRetryS 首定义片段切取失败——稳态长跑命名常量缺席"
        "（[S5] 缝契约）");
    TEST_ASSERT_TRUE_MESSAGE(sei_contains(def, "60"),
        "SEI-A2 RED: kAutoConnectSteadyRetryS 首定义不含 60——稳态长跑周期常量值偏离"
        "冻结值（arch §6.4: 恒 60 秒）");
    free(def);

    /* 定义 + task 内消费（两符号各 >=2 处）。 */
    n = 0;
    {
        char *buf = sei_read(SEI_APP_CC, NULL);
        TEST_ASSERT_NOT_NULL_MESSAGE(buf, "SEI-A2 RED(infra): application.cc 读取失败");
        n = sei_count_in_slice(buf, SEI_BACKOFF_ND);
        free(buf);
    }
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        sei_fm("SEI-A2 RED: kAutoConnectBackoffS 在 application.cc 仅 %d 处（下界 2: 定义 "
               "+ task 内消费）——退避表是孤岛", n));
    n = 0;
    {
        char *buf = sei_read(SEI_APP_CC, NULL);
        TEST_ASSERT_NOT_NULL_MESSAGE(buf, "SEI-A2 RED(infra): application.cc 读取失败");
        n = sei_count_in_slice(buf, SEI_STEADY_ND);
        free(buf);
    }
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        sei_fm("SEI-A2 RED: kAutoConnectSteadyRetryS 在 application.cc 仅 %d 处（下界 2: "
               "定义 + task 内消费）——稳态常量是孤岛（长跑未接 60s）", n));
}

/* =========================================================================== *
 * SEI-A3 — 长跑与退出（无 attempt < N 封顶字面 + 三退出条件各在世 + 清位次序）。
 * =========================================================================== */
/* coverage: SEI-A3 -- TryStartAutoConnect 函数区（含 task lambda）：无 attempt < 3 / attempt < 2 封顶字面 + 消费 kAutoConnectBackoffS/kAutoConnectSteadyRetryS + 成功退出 OpenAudioChannel() + bail 重查（GetDeviceState()!=Idle && IsAudioChannelOpened()）+ 网络断退出请求位（network_exit_requested_ 置位在 HandleNetworkDisconnectedEvent 三态门之外 + task 查位 + HandleNetworkConnectedEvent Idle 支路清位）+ auto_connect_in_flight_ 所有退出路径必清（清点位于 task 段、先清 in-flight 再补投）。回引 arch §6.4 自动重连段 + ADR-019 ④ */
TEST_CASE("会话结束意图: 长跑与三退出条件 (SEI-A3)",
          "[session_end_intent][SEI-A3][structural]")
{
    char *seam = NULL;
    char *disc = NULL;
    char *conn = NULL;
    char *all = NULL;
    const char *p_clear, *p_sched;
    const char *p_net, *p_inflight;
    const char *p_set, *p_gate;
    int n;

    SEI_REQUIRE_ROOT();

    seam = sei_member_fn(SEI_APP_CC, SEI_SEAM_FN_ND);
    TEST_ASSERT_NOT_NULL_MESSAGE(seam,
        "SEI-A3 RED(infra): TryStartAutoConnect 函数区切取失败——缝函数存在但形态漂移"
        "（须复核锚；禁改名绕过）");

    /* 无封顶字面: 3 次即止退役。 */
    n = sei_count_in_slice(seam, "attempt < 3") + sei_count_in_slice(seam, "attempt < 2");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        sei_fm("SEI-A3 RED: task 体内仍见 `attempt < N` 封顶字面 %d 处——3 次即止未退役"
               "（arch §6.4: 封顶递增后退避后恒 60s 长跑）", n));

    /* 长跑消费两表: 退避表 + 稳态常量。 */
    TEST_ASSERT_TRUE_MESSAGE(sei_contains(seam, SEI_BACKOFF_ND),
        "SEI-A3 RED: task 区未消费 kAutoConnectBackoffS——重试延迟未接决策表");
    TEST_ASSERT_TRUE_MESSAGE(sei_contains(seam, SEI_STEADY_ND),
        "SEI-A3 RED: task 区未消费 kAutoConnectSteadyRetryS——稳态长跑段未接（长跑退化）");

    /* 退出条件①成功: OpenAudioChannel() 在场。 */
    TEST_ASSERT_TRUE_MESSAGE(sei_contains(seam, "OpenAudioChannel()"),
        "SEI-A3 RED: task 区无 OpenAudioChannel()——成功退出条件缺失（无法建连）");

    /* 退出条件②bail: 每次尝试前重查守卫（GetDeviceState/IsAudioChannelOpened 双现）。 */
    TEST_ASSERT_TRUE_MESSAGE(sei_count_in_slice(seam, "IsAudioChannelOpened()") >= 2,
        "SEI-A3 RED: task 区 IsAudioChannelOpened() < 2 处——缺「每次尝试前重查守卫」半"
        "（bail 退出条件缺失）");
    TEST_ASSERT_TRUE_MESSAGE(sei_count_in_slice(seam, "kDeviceStateIdle") >= 2,
        "SEI-A3 RED: task 区 kDeviceStateIdle < 2 处——缺「每次尝试前重查守卫」半"
        "（bail 退出条件缺失）");

    /* 退出条件③网络断退出请求位: task 查位 + 早退先于置 in-flight。 */
    TEST_ASSERT_TRUE_MESSAGE(sei_contains(seam, SEI_NET_EXIT_ND),
        "SEI-A3 RED: task/缝区无 network_exit_requested_ ——网络断退出请求位缺失"
        "（WiFi 已断仍空转重试）");
    p_net = strstr(seam, SEI_NET_EXIT_ND);
    p_inflight = strstr(seam, "auto_connect_in_flight_ = true");
    TEST_ASSERT_TRUE_MESSAGE(p_net != NULL && p_inflight != NULL && p_net < p_inflight,
        "SEI-A3 RED: 网络断退出请求位早退未先于置 in-flight——预检块早退缺失（守卫未由"
        "三条件扩四条件；WiFi 已断时置位后仍起 task）");

    /* 清位次序: 先清 auto_connect_in_flight_ 再经 Schedule 补投。 */
    p_clear = strstr(seam, "auto_connect_in_flight_ = false");
    p_sched = strstr(seam, "Schedule(");
    TEST_ASSERT_TRUE_MESSAGE(p_clear != NULL,
        "SEI-A3 RED: task 区无 auto_connect_in_flight_ 清点——退出不清标志（泄漏 true = "
        "人工入口永久 no-op + 自动建连永久失效）");
    TEST_ASSERT_TRUE_MESSAGE(p_sched != NULL,
        "SEI-A3 RED: task 区无 Schedule( 补投——连上后意向位补投缺失（孩子那一次动作丢）");
    TEST_ASSERT_TRUE_MESSAGE(p_clear < p_sched,
        "SEI-A3 RED: 补投 Schedule( 位次先于清 auto_connect_in_flight_——补投会被自己吞掉"
        "（先清 in-flight 再投事件位）");

    free(seam);

    /* 网络断事件置位在函数体最顶端、三态门之外。 */
    disc = sei_region_between(SEI_APP_CC, SEI_DISC_START, SEI_DISC_END);
    TEST_ASSERT_NOT_NULL_MESSAGE(disc,
        "SEI-A3 RED(infra): HandleNetworkDisconnectedEvent 区切取失败——锚漂移须复核");
    p_set = strstr(disc, SEI_NET_EXIT_ND);
    p_gate = strstr(disc, "kDeviceStateConnecting");
    TEST_ASSERT_TRUE_MESSAGE(p_set != NULL,
        "SEI-A3 RED: HandleNetworkDisconnectedEvent 区无 network_exit_requested_ 置位——"
        "断网不通知 task 退出（空转重试）");
    TEST_ASSERT_TRUE_MESSAGE(p_gate != NULL && p_set < p_gate,
        "SEI-A3 RED: 网络断退出请求位置位未在三态门之外（须置于函数体最顶端）——"
        "非会话态断网仍空转重试");
    free(disc);

    /* 连接成功 Idle 支路清位。 */
    conn = sei_region_between(SEI_APP_CC, SEI_CONN_START, SEI_CONN_END);
    TEST_ASSERT_NOT_NULL_MESSAGE(conn,
        "SEI-A3 RED(infra): HandleNetworkConnectedEvent 区切取失败——锚漂移须复核");
    TEST_ASSERT_TRUE_MESSAGE(sei_contains(conn, SEI_NET_EXIT_ND),
        "SEI-A3 RED: HandleNetworkConnectedEvent 区无 network_exit_requested_ 清位——"
        "重连成功后退出请求位不复位（后续自动建连被永久早退）");
    free(conn);

    /* 全文件网络断退出请求位下界（置位 + task 查位 + 清位 >= 3）。 */
    all = sei_read(SEI_APP_CC, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(all, "SEI-A3 RED(infra): application.cc 读取失败");
    n = sei_count_in_slice(all, SEI_NET_EXIT_ND);
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        sei_fm("SEI-A3 RED: 全文件 network_exit_requested_ 仅 %d 处（下界 3: 置位 + task "
               "查位 + 重连清位）——网络断退出请求位生命周期缺环", n));
    free(all);
}

/* =========================================================================== *
 * SEI-A4 — 人工意向位（五入口置位 + 审计行 + task 截短/补投/次序）。
 * =========================================================================== */
/* coverage: SEI-A4 -- application.h 声明 pending_manual_intent_；五个人工入口代码点（HandleToggleChatEvent / HandleStartListeningEvent / HandleEndVoiceSessionEvent / HandleWakeWordDetectedEvent / WakeWordInvoke）各自 in-flight 分支内置 pending_manual_intent_ + 审计行 manual_intent_queued + return；HandleStartListeningEvent 插入槽在 kidpal_story_session_ctx_set(0 之后 return 之前（RSL2-A3 序钉保值）；task 段查意向位 + 截短退避 + 连上后 Schedule 补投。回引 arch §6.4 人工意向位 + ADR-019 ④ + PROPOSAL T1 改法 4 */
TEST_CASE("会话结束意图: 人工意向位不丢 (SEI-A4)",
          "[session_end_intent][SEI-A4][structural]")
{
    static const struct {
        const char *start;
        const char *end;
        const char *label;
    } sites[] = {
        { "void Application::HandleToggleChatEvent()",
          "void Application::HandleStartListeningEvent()", "ToggleChat" },
        { "void Application::HandleStartListeningEvent()",
          "void Application::HandleStopListeningEvent()", "StartListening" },
        { "void Application::HandleEndVoiceSessionEvent()",
          "void Application::HandleWakeWordDetectedEvent()", "EndVoiceSession" },
        { "void Application::HandleWakeWordDetectedEvent()",
          "void Application::HandleStateChangedEvent()", "WakeWordDetected" },
        { "Application::WakeWordInvoke(",
          "bool Application::CanEnterSleepMode()", "WakeWordInvoke" },
    };
    char *seam = NULL;
    const char *p_seam_intent;
    size_t i;

    SEI_REQUIRE_ROOT();

    /* application.h 声明意向位（[S4]）。 */
    TEST_ASSERT_TRUE_MESSAGE(sei_file_contains(SEI_APP_H, SEI_INTENT_ND),
        "SEI-A4 RED: application.h 无 pending_manual_intent_ ——人工意向位成员未落"
        "（[S4] 缝契约；与 auto_connect_in_flight_ 同区）");

    for (i = 0; i < sizeof(sites) / sizeof(sites[0]); i++) {
        char *region = sei_region_between(SEI_APP_CC, sites[i].start, sites[i].end);
        const char *p_flag, *p_ret, *p_intent;
        char *branch = NULL;

        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            sei_fm("SEI-A4 RED(infra): %s 函数区切取失败——人工入口锚漂移须复核"
                   "（禁整函数退役绕过）", sites[i].label));

        p_flag = strstr(region, SEI_FLAG_ND);
        TEST_ASSERT_TRUE_MESSAGE(p_flag != NULL,
            sei_fm("SEI-A4 RED: %s 区无 auto_connect_in_flight_ 守卫——既有 K6-A5 守卫"
                   "语义被拆（须保留守卫，只把动作从裸 return 改为置意向位）",
                   sites[i].label));

        /* in-flight 分支 = auto_connect_in_flight_ 起至其后第一个 return;（含 return）。 */
        p_ret = p_flag != NULL ? strstr(p_flag, "return;") : NULL;
        TEST_ASSERT_TRUE_MESSAGE(p_ret != NULL,
            sei_fm("SEI-A4 RED: %s in-flight 分支无 return; ——守卫分支形态漂移须复核",
                   sites[i].label));
        branch = sei_span(p_flag, p_ret + strlen("return;"));
        TEST_ASSERT_NOT_NULL_MESSAGE(branch, "SEI-A4 RED(infra): in-flight 分支切取失败");

        p_intent = strstr(branch, SEI_INTENT_ND);
        TEST_ASSERT_TRUE_MESSAGE(p_intent != NULL,
            sei_fm("SEI-A4 RED: %s in-flight 分支未置 pending_manual_intent_ ——离线期人工"
                   "入口仍静默 no-op（孩子那一次动作丢；长跑把「按了没反应」放大为恒定）",
                   sites[i].label));
        TEST_ASSERT_TRUE_MESSAGE(sei_contains(branch, SEI_AUDIT_ND),
            sei_fm("SEI-A4 RED: %s in-flight 分支无审计行 manual_intent_queued ——意图置位"
                   "不可观测（真机轮 B8 无审计行证据）", sites[i].label));

        /* StartListening 插入槽: 必须在 kidpal_story_session_ctx_set(0 之后（RSL2-A3 序钉保绿）。 */
        if (strcmp(sites[i].label, "StartListening") == 0) {
            const char *p_ctx = strstr(branch, "kidpal_story_session_ctx_set(0");
            TEST_ASSERT_TRUE_MESSAGE(p_ctx != NULL,
                "SEI-A4 RED(infra): StartListening in-flight 分支无 kidpal_story_session_ctx_set(0"
                "（RSL2-A3 冻结锚丢失，须复核）");
            TEST_ASSERT_TRUE_MESSAGE(p_ctx < p_intent,
                "SEI-A4 RED: StartListening 意向位置位先于 ctx 清位——插入槽须在"
                " kidpal_story_session_ctx_set(0 之后、return; 之前（RSL2-A3 序钉）");
        }

        free(branch);
        free(region);
    }

    /* task 段: 查意向位 + 截短（长跑内查位）+ 补投顺序在 SEI-A3 已钉 Schedule 位次。 */
    seam = sei_member_fn(SEI_APP_CC, SEI_SEAM_FN_ND);
    TEST_ASSERT_NOT_NULL_MESSAGE(seam,
        "SEI-A4 RED(infra): TryStartAutoConnect 函数区切取失败（task 段意向位判据）");
    p_seam_intent = strstr(seam, SEI_INTENT_ND);
    TEST_ASSERT_TRUE_MESSAGE(p_seam_intent != NULL,
        "SEI-A4 RED: task 段无 pending_manual_intent_ 查位——退避不截短（人工按键后仍等"
        "满退避窗才重试）");
    TEST_ASSERT_TRUE_MESSAGE(sei_contains(seam, "pending_manual_intent_ = 0"),
        "SEI-A4 RED: task 段无 pending_manual_intent_ = 0 清位——补投后意向位不复位"
        "（下次重连错误补投陈旧事件）");
    free(seam);
}

/* ---------------------------------------------------------------------------
 * RED baseline 判读口径（freeze 相位，acceptance-author 2026-11-19 实跑回填）:
 *   通道② standalone harness（本件未注册前独立红基线；注册后随 host runner 全量）:
 *     cd firmware/main/test/acceptance/red-baseline
 *     gcc -std=c11 -Wall -Wextra -I. -o /tmp/sei_red_runner \
 *         ../../session_end_intent_longretry.acceptance.c runner.c
 *     /tmp/sei_red_runner   （exit code = 失败 TEST_CASE 数）
 *   编译通道 error 为零（验收文件本身可编译，验收测试纪律 §4）；设计态红判读见回执。
 *   MUST-STAY-GREEN at baseline: SEI-A1 case 1（stop_speaking AbortSpeaking 恰 1）;
 *     SEI-A2 值序片段可切（旧表形态先行）。
 * --------------------------------------------------------------------------- */