/*
 * Frozen acceptance tests — TaskPackage 260829-voice-roundtrip
 * (subsystem: firmware+server; 方向 B 语音回环 + display 消费链路; mergeOrder 5;
 *  本包 = 包 4/5/6/8 的设备 display 路由共享底座, PROPOSAL §4).
 *
 * Nature: host-runnable structural + BEHAVIORAL frozen acceptance on the
 * device consumption face, following the device_ws_switch.acceptance.c (WSA)
 * tree-scan precedent for structural teeth, PLUS a frozen pure-logic offline
 * contract for the new consumption logic (mirrors the G package's python
 * frozen-offline-contract pattern in server/acceptance/test_gateway_directionB.acceptance.py:
 * "builder MUST implement these importable pure modules") so the red baseline
 * is RED on real missing behaviour, never on a build/import failure.
 *
 * ===========================================================================
 * FROZEN OFFLINE CONTRACT (builder MUST implement, pure logic, host-compilable,
 * no chip/FreeRTOS/ESP-component deps; C linkage — extern "C" if in a .cc;
 * recommended home: firmware/main/display/view_router.(c|cc) + view_router.h;
 * builder also wires the implementation source into HOST_ACCEPTANCE_SRCS in
 * firmware/main/test/host_app/main/CMakeLists.txt so the strong definitions
 * override the weak defaults below in the host runner — same one-line pattern
 * as package 3 did for session_state_machine.cc):
 *
 *   int kidpal_display_route(const char *view);
 *       Route an E1 display payload view (契约 §1.3, view whitelist
 *       char/schedule/story_list/story_text/text/home/control) to its render
 *       route. Returns:
 *         0 text  1 home  2 control  3 char  4 schedule  5 story_list  6 story_text
 *        -1 VRT_ROUTE_UNKNOWN   (view outside the whitelist -> degrade path
 *                                signal; the caller must show a friendly degrade
 *                                hint and MUST NOT blind-render, V27/§4.0)
 *       NULL view also returns -1 (defensive, never crash).
 *
 *   int kidpal_display_control_action(const char *action);
 *       Parse a control-view action (契约 §1.4). Returns:
 *         0 end_session  1 stop_speaking  2 start_recording
 *        -1 unknown/NULL action.
 *
 *   int kidpal_display_text_page_count(const char *text, int screen_lines);
 *       Full-text (S-F4.1: 答案全文上屏, human 裁定否摘要) pagination for the
 *       scrollable text reading face (超屏高滚动, BOOT 长按翻屏沿用 P3 阅读态
 *       习惯). Returns the number of screens needed to show ALL of `text` at
 *       `screen_lines` lines per screen; >= 1 for any non-NULL text; NULL text
 *       counts as one (empty) screen. screen_lines <= 0 is defensive input:
 *       return 1 (single screen) or -1 (explicit error) — never divide-by-zero.
 *       Property the acceptance pins: pages * screen_lines >= line_count
 *       (全文不丢内容 — no truncation happens here; >3-句 truncation is the
 *       GATEWAY's V32 face, generation 归 G, device only renders full text).
 *
 * While the implementation is absent, __attribute__((weak)) defaults below
 * return the sentinel -2 (VRT_UNIMPLEMENTED): every behavioral test asserts
 * NOT-sentinel first, so the red baseline reads "frozen contract not
 * implemented" — a real missing-behaviour red, not a link error.
 * ===========================================================================
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef / exitCriteria -> test):
 *   coverage: VRT-A1  structural (+VRT-E1) -> test VRT-A1 (display 消费面装配:
 *               路由模块存在于设备树 + 被设备构建 CMakeLists 引用 + 消费调用点
 *               存在(接入会话面) + application.cc 原生 mcp 分支守卫)
 *   coverage: VRT-A2  device face  -> test VRT-A2 (流式语音回环设备面: tts
 *               start/sentence_start(带文字)/stop 状态机原生缝 + stt 识别文字
 *               上屏 + 句级文字同步 SetChatMessage 守卫)
 *   coverage: VRT-A2/E2 evidence  -> test evidence (方向 B 流式回环 live 冒烟
 *               证据: delivery/changes/dev/260829-voice-roundtrip/ 记录件含 session_id/
 *               stt/tts/sentence_start/state-speaking 文字同步真实标记,
 *               WSA-A4 修订版标记口径)
 *   coverage: VRT-A3  behavioral  -> test VRT-A3 (view 路由行为: text/home/
 *               control 三路由 + 7 view 注册表完整(4/5/6/8 additive 底座,
 *               VRT-E3) + 白名单外 -> UNKNOWN 降级 + control 动作解析)
 *   coverage: VRT-A3  behavioral  -> test VRT-A3-text (text 全文分页: 短文 1 屏
 *               + 10 行/4 行屏 >=3 屏且 pages*lines >= 行数(全文不丢) + 单调性
 *               + screen_lines<=0 防御不崩)
 *   coverage: VRT-A5  device face  -> test VRT-A5 (网关兜底设备侧消费: 兜底
 *               text 视图与 A3 同面消费(行为) + 格式不遵循降级信号为独立路由
 *               值(行为) + tts stop 当轮结束原生守卫 + 兜底生成逻辑不在固件
 *               (scope 守卫在 boundary))
 *   coverage: VRT-A6  device face  -> test VRT-A6 (abort 发送路径: protocol.cc
 *               abort 消息形状原生守卫 + 打断调用点 >= 3 处; 云端取消生成由
 *               server 侧证据承载 — 本地 AEC+VAD 即时停播主地位不变, 9a 实测)
 *   boundary/security             -> test boundary (扫描器自检防假绿 + 兜底
 *               文案生成面禁入固件(生成逻辑归 G, F2) + 设备不直连 OpenClaw)
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.2]/[C4.4]):
 *   0) builder wiring: add "../../voice_roundtrip_b.acceptance.c" AND the
 *      view_router implementation source to HOST_ACCEPTANCE_SRCS in
 *      firmware/main/test/host_app/main/CMakeLists.txt.
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
 *   Every TEST_CASE must execute and pass; the runner must exit 0.
 *
 * Evidence dir (A2/E2 live smoke): delivery/changes/dev/260829-voice-roundtrip/ — the
 *   roundtrip/evidence record file is produced by the operator/builder during
 *   the pod's live smoke, then asserted HERE by content (WSA-A4 evidence face
 *   precedent). A missing/empty/unqualified evidence file is RED with its own
 *   specific reason; "no error logged" NEVER counts as pass.
 *
 * RED baseline (vehicle state, 2026-09-07 acceptance-author 实测):
 *   - firmware/main has NO kidpal/display consumption face (no file references
 *     kidpal_display_route or kidpal/display); no view router; no text
 *     full-text pagination logic; no server/deploy wiring yet.
 *   - Native xiaozhi faces EXIST and are pinned as green-guards: tts
 *     start/sentence_start/stop + stt handlers in application.cc (§四 state
 *     machine), abort send path in protocol.cc + AbortSpeaking call sites.
 *   - system prompt / fallback consumption cloud faces are asserted by the
 *     python twin server/tests/test_voice_roundtrip_b.acceptance.py.
 *   -> VRT-A1/A3/A3-text/A5 RED below each for its own missing-behaviour
 *      reason; A2/A6/boundary are green-guards of native faces that MUST stay
 *      green; evidence is red until the live smoke is performed. See
 *      delivery/taskpackages/260829-voice-roundtrip/red-baseline-summary.md.
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-07  initial freeze (acceptance-author).
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>

#define VRT_MAX_PATH 1024
#define VRT_MAX_READ (8u * 1024u * 1024u) /* refuse to slurp files larger than 8 MB */

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — numeric route codes + weak defaults.
 * The weak defaults make this file link standalone (red baseline, builder
 * wiring not yet done); once the builder's strong definitions are linked the
 * weak bodies are discarded and the REAL consumption logic is exercised.
 * ------------------------------------------------------------------------ */
#define VRT_ROUTE_TEXT          0
#define VRT_ROUTE_HOME          1
#define VRT_ROUTE_CONTROL       2
#define VRT_ROUTE_CHAR          3
#define VRT_ROUTE_SCHEDULE      4
#define VRT_ROUTE_STORY_LIST    5
#define VRT_ROUTE_STORY_TEXT    6
#define VRT_ROUTE_UNKNOWN    (-1)
#define VRT_UNIMPLEMENTED    (-2) /* weak-default sentinel — never a pass */

#define VRT_CTRL_END_SESSION      0
#define VRT_CTRL_STOP_SPEAKING    1
#define VRT_CTRL_START_RECORDING  2
#define VRT_CTRL_UNKNOWN       (-1)

__attribute__((weak)) int kidpal_display_route(const char *view)
{
    (void)view;
    return VRT_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_display_control_action(const char *action)
{
    (void)action;
    return VRT_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_display_text_page_count(const char *text, int screen_lines)
{
    (void)text;
    (void)screen_lines;
    return VRT_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers (host-side structural teeth), WSA precedent.
 * All scans run against the REAL working tree located from the runner cwd;
 * paths are repo-root-relative with forward slashes. Directories named
 * test / host_app / build / .git are EXCLUDED from every scan (test/ hosts
 * THIS frozen file whose comments legitimately mention the frozen needles;
 * host_app/build are build artifacts; none is implementation surface).
 * "assets" is additionally skipped: binary 1bpp/font assets are not
 * implementation surface for any needle below.
 * ------------------------------------------------------------------------ */

static char vrt_root[VRT_MAX_PATH];
static int vrt_root_ready;

static int vrt_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int vrt_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int vrt_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int vrt_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[VRT_MAX_PATH];

    if (vrt_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (vrt_is_regular(probe)) {
            snprintf(vrt_root, sizeof(vrt_root), "%s", cands[i]);
            vrt_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int vrt_rel(char *out, size_t cap, const char *rel)
{
    if (!vrt_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", vrt_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", vrt_root, rel);
    return 1;
}

static int vrt_exists(const char *rel)
{
    char p[VRT_MAX_PATH];
    return vrt_rel(p, sizeof(p), rel) && vrt_is_regular(p);
}

/* Read a repo-relative file fully into a NUL-terminated malloc'd buffer, or NULL. */
static char *vrt_read(const char *rel, size_t *out_len)
{
    char p[VRT_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!vrt_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > VRT_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int vrt_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int vrt_contains_ci(const char *hay, const char *needle)
{
    size_t n;

    assert(hay != NULL && needle != NULL);
    n = strlen(needle);
    if (n == 0) {
        return 1;
    }
    for (const char *h = hay; *h != '\0'; h++) {
        size_t i = 0;
        while (i < n && h[i] != '\0' &&
               tolower((unsigned char)h[i]) == tolower((unsigned char)needle[i])) {
            i++;
        }
        if (i == n) {
            return 1;
        }
    }
    return 0;
}

static int vrt_file_contains(const char *rel, const char *needle)
{
    char *buf = vrt_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = vrt_contains(buf, needle);
    free(buf);
    return r;
}

/* Count occurrences of `needle` in one file's content (0 if unreadable). */
static int vrt_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = vrt_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = buf; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    free(buf);
    return count;
}

/* Count files under rel_dir whose content matches needle (recursive; ci flag).
 * Fills `first_hit` (repo-relative, may be NULL) with the first match. */
static int vrt_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
                                       const char *needle, int ci,
                                       char *first_hit, size_t cap, int depth)
{
    DIR *d;
    struct dirent *e;
    int count = 0;

    if (depth > 10) {
        return 0;
    }
    d = opendir(abs_dir);
    if (d == NULL) {
        return 0;
    }
    while ((e = readdir(d)) != NULL) {
        char abs_next[VRT_MAX_PATH];
        char rel_next[VRT_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            vrt_is_skipped_dir(e->d_name)) {
            continue;
        }
        snprintf(abs_next, sizeof(abs_next), "%s/%s", abs_dir, e->d_name);
        if (rel_prefix[0] != '\0') {
            snprintf(rel_next, sizeof(rel_next), "%s/%s", rel_prefix, e->d_name);
        } else {
            snprintf(rel_next, sizeof(rel_next), "%s", e->d_name);
        }
        if (stat(abs_next, &st) != 0) {
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            count += vrt_count_files_with_needle(abs_next, rel_next, needle, ci,
                                                 first_hit, cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > VRT_MAX_READ) {
                continue;
            }
            FILE *f = fopen(abs_next, "rb");
            if (f == NULL) {
                continue;
            }
            char *buf = (char *)malloc((size_t)st.st_size + 1);
            assert(buf != NULL);
            if (fread(buf, 1, (size_t)st.st_size, f) == (size_t)st.st_size) {
                buf[st.st_size] = '\0';
                int m = ci ? vrt_contains_ci(buf, needle) : vrt_contains(buf, needle);
                if (m) {
                    if (count == 0 && first_hit != NULL) {
                        snprintf(first_hit, cap, "%s", rel_next);
                    }
                    count++;
                }
            }
            free(buf);
            fclose(f);
        }
    }
    closedir(d);
    return count;
}

static int vrt_scan_count(const char *rel_dir, const char *needle, int ci,
                          char *first_hit, size_t cap)
{
    char abs[VRT_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!vrt_rel(abs, sizeof(abs), rel_dir) || !vrt_is_dir(abs)) {
        return 0;
    }
    return vrt_count_files_with_needle(abs, rel_dir, needle, ci, first_hit, cap, 0);
}

/* Find a file under evidence_dir whose BASENAME contains any of the name
 * needles (case-insensitive) and whose CONTENT matches all content needles
 * (each of which may be an alternation group: "a|b|c" matches if ANY of the
 * |-separated alternatives is present, case-insensitively). Returns 1 and
 * fills `hit` with the repo-relative path on success. */
static int vrt_find_evidence(const char *rel_dir, const char *const *name_needles,
                             size_t n_name, const char *const *content_groups,
                             size_t n_content, char *hit, size_t cap)
{
    char abs[VRT_MAX_PATH];
    DIR *d;
    struct dirent *e;
    int found = 0;

    if (!vrt_rel(abs, sizeof(abs), rel_dir) || !vrt_is_dir(abs)) {
        return 0;
    }
    d = opendir(abs);
    if (d == NULL) {
        return 0;
    }
    while (!found && (e = readdir(d)) != NULL) {
        char rel_next[VRT_MAX_PATH];
        char *buf;
        int name_ok = 0;
        size_t k;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) {
            continue;
        }
        snprintf(rel_next, sizeof(rel_next), "%s/%s", rel_dir, e->d_name);
        if (!vrt_is_regular(rel_next)) {
            continue;
        }
        for (k = 0; k < n_name && !name_ok; k++) {
            if (vrt_contains_ci(e->d_name, name_needles[k])) {
                name_ok = 1;
            }
        }
        if (n_name > 0 && !name_ok) {
            continue;
        }
        buf = vrt_read(rel_next, NULL);
        if (buf == NULL) {
            continue;
        }
        found = 1;
        for (k = 0; k < n_content && found; k++) {
            /* alternation group "a|b|c": at least one alternative must hit */
            const char *group = content_groups[k];
            int any = 0;
            const char *start = group;
            for (;;) {
                const char *bar = strchr(start, '|');
                size_t len = bar ? (size_t)(bar - start) : strlen(start);
                char alt[256];
                if (len >= sizeof(alt)) {
                    len = sizeof(alt) - 1;
                }
                memcpy(alt, start, len);
                alt[len] = '\0';
                if (alt[0] != '\0' && vrt_contains_ci(buf, alt)) {
                    any = 1;
                    break;
                }
                if (bar == NULL) {
                    break;
                }
                start = bar + 1;
            }
            if (!any) {
                found = 0;
            }
        }
        free(buf);
        if (found) {
            snprintf(hit, cap, "%s", rel_next);
        }
    }
    closedir(d);
    return found;
}

#define VRT_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(vrt_locate_root(), \
    "VRT RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* Shared red-string for the frozen offline contract not being implemented yet. */
static void vrt_require_implemented(int route_value, const char *what)
{
    if (route_value == VRT_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "VRT RED: frozen offline contract NOT implemented — %s "
                 "(builder must land the pure-logic view router per the frozen "
                 "contract in firmware/main/test/voice_roundtrip_b.acceptance.c "
                 "header and wire it into HOST_ACCEPTANCE_SRCS)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* -------------------------------------------------------------------------- *
 * VRT-A1 — display 消费面装配结构（structural, +VRT-E1）
 * Four teeth, each a REAL repo fact:
 *   1) a consumption/route module exists in the device tree referencing the
 *      frozen contract symbol kidpal_display_route( — today NOTHING under
 *      firmware/main references it (kidpal/display consumption face absent);
 *   2) the module is compiled into the DEVICE build: firmware/main/CMakeLists.txt
 *      lists the router file's basename (assembly fact, not a floating file);
 *   3) the route face is REACHED from the session chain: >= 2 distinct files
 *      reference kidpal_display_route( — the definition plus at least one real
 *      consumer call site (application.cc 的 mcp 分支 -> McpServer::ParseMessage
 *      -> 消费链路); defined-but-never-called fails this tooth;
 *   4) the router file carries an explicit unknown/degrade semantic marker —
 *      the whitelist-outside view must be a distinct degrade path, never a
 *      blind render (V27 / spec §4.0 格式不遵循兜底行).
 * Native mcp message branch in application.cc is pinned as a MUST-STAY-GREEN
 * guard (the arrival face the consumption chain rides on).
 * RED today: teeth 1)-4) have no implementation; guard 4) green (native).
 * ------------------------------------------------------------------------ */
TEST_CASE("voice roundtrip b: display 消费面装配结构 (VRT-A1)", "[voice_rt][VRT-A1][structural]")
{
    char hit[VRT_MAX_PATH];
    char *buf;
    int n_files;

    VRT_REQUIRE_ROOT();

    /* tooth 1: 路由/消费模块存在于设备树 */
    n_files = vrt_scan_count("firmware/main", "kidpal_display_route(", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n_files >= 1,
        "VRT-A1 RED: no module under firmware/main implements the frozen display "
        "route contract (kidpal_display_route absent) — mcp kidpal/display 消费面缺失 "
        "(contract §1.5-4 / §四 mcp 工具调用, 设备侧)");

    /* tooth 2: 模块被设备构建引用（CMakeLists 装配，非浮文件） */
    {
        char base[VRT_MAX_PATH];
        const char *slash = strrchr(hit, '/');
        snprintf(base, sizeof(base), "%s", slash ? slash + 1 : hit);
        TEST_ASSERT_TRUE_MESSAGE(vrt_file_contains("firmware/main/CMakeLists.txt", base),
            "VRT-A1 RED: the view-router/consumer source is not referenced by "
            "firmware/main/CMakeLists.txt — 模块未编入设备构建（装配缺失）");
    }

    /* tooth 3: 会话面调用点存在（定义 + 消费调用 >= 2 个文件） */
    TEST_ASSERT_TRUE_MESSAGE(n_files >= 2,
        "VRT-A1 RED: kidpal_display_route is defined but never called — display 消费"
        "面未接入会话链路（application.cc mcp 分支 -> McpServer::ParseMessage -> "
        "消费调用点缺失; 仅定义不调用不算接入）");

    /* tooth 4: 白名单外 view = 独立降级路径的显式语义 */
    buf = vrt_read(hit, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "VRT-A1 RED: router file unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        vrt_contains_ci(buf, "UNKNOWN") || vrt_contains_ci(buf, "unknown") ||
        strstr(buf, "未知") != NULL || strstr(buf, "降级") != NULL,
        "VRT-A1 RED: the router file carries no unknown-view/degrade semantic marker — "
        "白名单外 view 必须是显式降级路径而非盲渲染 (V27 / spec §4.0)");
    free(buf);

    /* native guard (must stay green): application.cc 的 mcp 分支是消费链路的到达面 */
    TEST_ASSERT_TRUE_MESSAGE(vrt_file_contains("firmware/main/application.cc", "\"mcp\""),
        "VRT-A1 RED(infra): application.cc no longer routes type \"mcp\" messages — "
        "原生 mcp 到达分支被破坏");
    TEST_ASSERT_TRUE_MESSAGE(
        vrt_file_contains("firmware/main/application.cc", "ParseMessage"),
        "VRT-A1 RED(infra): application.cc no longer feeds mcp payloads into "
        "McpServer::ParseMessage — 消费链路入口被破坏");
}

/* -------------------------------------------------------------------------- *
 * VRT-A3 — display mcp 消费端 view 路由行为（behavioral, frozen offline contract）
 * Exercises the REAL consumption logic through the frozen contract functions:
 *   - text/home/control 三路由正确（本包的三个 view 承载, VRT-A3 主面）;
 *   - 7 view 注册表完整（char/schedule/story_list/story_text 也在表中 — 本包
 *     是包 4/5/6/8 的共享路由底座, VRT-E3, additive 扩展不改核心）;
 *   - 白名单外 view（"dinosaur"）-> UNKNOWN 降级信号, 绝不盲渲染（V27）;
 *   - NULL view 防御性返回 UNKNOWN（不崩）;
 *   - control 动作解析: end_session / stop_speaking / start_recording（§1.4 全表）,
 *     未知动作 -> UNKNOWN。
 * RED today: every assertion hits the weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("voice roundtrip b: view 路由行为——kidpal/display 载荷按 view 路由 (VRT-A3)",
          "[voice_rt][VRT-A3]")
{
    int r;

    /* text 全文上屏滚动 (契约 §1.3 ⑤ / S-F4.1) */
    r = kidpal_display_route("text");
    vrt_require_implemented(r, "kidpal_display_route unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_ROUTE_TEXT,
        "VRT-A3 RED: view \"text\" is not routed to the text render face — text 全文"
        "上屏滚动路由缺失 (契约 §1.3 ⑤)");

    /* home 回首页 (契约 §1.3 ⑥) */
    r = kidpal_display_route("home");
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_ROUTE_HOME,
        "VRT-A3 RED: view \"home\" is not routed back to P1 首页 — home 路由缺失 "
        "(契约 §1.3 ⑥)");

    /* control (契约 §1.4) */
    r = kidpal_display_route("control");
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_ROUTE_CONTROL,
        "VRT-A3 RED: view \"control\" is not routed to the control executor — control "
        "路由缺失 (契约 §1.4)");

    /* 7 view 注册表完整 — 包 4/5/6/8 的 additive 共享底座 (VRT-E3) */
    r = kidpal_display_route("char");
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_ROUTE_CHAR,
        "VRT-A3 RED: view \"char\" missing from the route registry — 包 4 的底座路由 "
        "不完整 (VRT-E3, 契约 §1.3 ①)");
    r = kidpal_display_route("schedule");
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_ROUTE_SCHEDULE,
        "VRT-A3 RED: view \"schedule\" missing from the route registry — 包 5/6 底座 "
        "路由不完整 (VRT-E3, 契约 §1.3 ②)");
    r = kidpal_display_route("story_list");
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_ROUTE_STORY_LIST,
        "VRT-A3 RED: view \"story_list\" missing from the route registry — 包 8 底座 "
        "路由不完整 (VRT-E3, 契约 §1.3 ③)");
    r = kidpal_display_route("story_text");
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_ROUTE_STORY_TEXT,
        "VRT-A3 RED: view \"story_text\" missing from the route registry — 包 8 底座 "
        "路由不完整 (VRT-E3, 契约 §1.3 ④)");

    /* 白名单外 view -> UNKNOWN 降级信号（绝不盲渲染, V27） */
    r = kidpal_display_route("dinosaur");
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_ROUTE_UNKNOWN,
        "VRT-A3 RED: a view outside the contract whitelist did not return the UNKNOWN "
        "degrade route — 白名单外 view 必须走降级路径, 不得盲渲染 (V27/spec §4.0)");

    /* NULL view 防御性（不崩、归降级） */
    r = kidpal_display_route(NULL);
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_ROUTE_UNKNOWN,
        "VRT-A3 RED: NULL view must be defended (return UNKNOWN), not crash");

    /* control 动作解析 (契约 §1.4 动作表) */
    r = kidpal_display_control_action("end_session");
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_CTRL_END_SESSION,
        "VRT-A3 RED: control action \"end_session\" not parsed — 结束会话动作缺失 "
        "(契约 §1.4)");
    r = kidpal_display_control_action("stop_speaking");
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_CTRL_STOP_SPEAKING,
        "VRT-A3 RED: control action \"stop_speaking\" not parsed — 云端确认停播动作缺失 "
        "(契约 §1.4)");
    r = kidpal_display_control_action("start_recording");
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_CTRL_START_RECORDING,
        "VRT-A3 RED: control action \"start_recording\" not parsed — 包 8 录音接合动作 "
        "缺失 (契约 §1.4)");
    r = kidpal_display_control_action("self_destruct");
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_CTRL_UNKNOWN,
        "VRT-A3 RED: an unknown control action was not rejected — 未知动作必须进降级路径");
    r = kidpal_display_control_action(NULL);
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_CTRL_UNKNOWN,
        "VRT-A3 RED: NULL control action must be defended (return UNKNOWN)");
}

/* -------------------------------------------------------------------------- *
 * VRT-A3 (text 全文上屏) — 分页纯逻辑：全文不丢内容（behavioral）
 * S-F4.1: 答案全文上屏滚动（human 裁定否摘要），超屏高滚动 + BOOT 长按翻屏沿用
 * P3 阅读态习惯。Pinned properties on kidpal_display_text_page_count:
 *   - 短文本 1 屏；
 *   - 10 行文本、每屏 4 行 -> >= 3 屏，且 pages*screen_lines >= 行数（全文不丢）；
 *   - 更长文本页数单调不减；
 *   - screen_lines <= 0 防御输入：返回 1 或显式错误 -1，绝不除零崩溃。
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("voice roundtrip b: text 全文上屏——分页全文不丢 (VRT-A3)", "[voice_rt][VRT-A3]")
{
    static const char *const ten_lines =
        "第一行\n第二行\n第三行\n第四行\n第五行\n第六行\n第七行\n第八行\n第九行\n第十行";
    int pages_short, pages_long, pages_longer, r;

    pages_short = kidpal_display_text_page_count("你好呀", 4);
    vrt_require_implemented(pages_short, "kidpal_display_text_page_count unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(pages_short >= 1,
        "VRT-A3 RED: a short text must occupy at least one screen");
    TEST_ASSERT_TRUE_MESSAGE(pages_short == 1,
        "VRT-A3 RED: a one-line text must fit on a single screen — 短文不应翻屏");

    pages_long = kidpal_display_text_page_count(ten_lines, 4);
    TEST_ASSERT_TRUE_MESSAGE(pages_long >= 3,
        "VRT-A3 RED: 10 lines at 4 lines/screen must span >= 3 screens — 超屏高滚动/"
        "翻屏缺失 (S-F4.1)");
    TEST_ASSERT_TRUE_MESSAGE(pages_long * 4 >= 10,
        "VRT-A3 RED: pages*screen_lines < line count — 分页丢了正文内容 (全文上屏 "
        "S-F4.1: human 裁定否摘要, 不允许显示端丢内容)");

    pages_longer = kidpal_display_text_page_count(
        "第一行\n第二行\n第三行\n第四行\n第五行\n第六行\n第七行\n第八行\n第九行\n第十行"
        "\n十一\n十二\n十三\n十四\n十五\n十六\n十七\n十八\n十九\n二十",
        4);
    TEST_ASSERT_TRUE_MESSAGE(pages_longer >= pages_long,
        "VRT-A3 RED: page count is not monotonic in text length — 分页逻辑不稳定");

    r = kidpal_display_text_page_count(ten_lines, 0);
    TEST_ASSERT_TRUE_MESSAGE(r >= 1 || r == -1,
        "VRT-A3 RED: screen_lines=0 must be defended (1 or explicit -1), not a "
        "divide-by-zero crash");
    r = kidpal_display_text_page_count(NULL, 4);
    TEST_ASSERT_TRUE_MESSAGE(r >= 1,
        "VRT-A3 RED: NULL text must be defended as one (empty) screen, not crash");
}

/* -------------------------------------------------------------------------- *
 * VRT-A2 — 流式语音回环设备面（native green-guards, device face）
 * The xiaozhi-native §四 state machine is the streaming loop's device half
 * (WS 包复活的原生管线). These teeth pin it as MUST-STAY-GREEN:
 *   - tts start -> kDeviceStateSpeaking（播报态）;
 *   - tts sentence_start 带 text -> SetChatMessage("assistant", …)（句级文字同步
 *     上屏, state-speaking 的文字面）;
 *   - tts stop -> 回 Idle/Listening（当轮结束/回聆听）;
 *   - stt 带 text -> SetChatMessage("user", …)（识别文字上屏）。
 * The end-to-end streaming round trip itself is live-proven and asserted by the
 * evidence test below (A2/E2 evidence).
 * ------------------------------------------------------------------------ */
TEST_CASE("voice roundtrip b: 流式语音回环设备面——stt/tts 状态机原生缝 (VRT-A2)",
          "[voice_rt][VRT-A2]")
{
    VRT_REQUIRE_ROOT();

    TEST_ASSERT_TRUE_MESSAGE(vrt_file_contains("firmware/main/application.cc", "\"tts\""),
        "VRT-A2 RED(infra): application.cc no longer handles type \"tts\" messages — "
        "播报状态机到达面被破坏 (契约 §四)");
    TEST_ASSERT_TRUE_MESSAGE(
        vrt_file_contains("firmware/main/application.cc", "\"sentence_start\""),
        "VRT-A2 RED(infra): tts sentence_start state handling missing — 句级下发态缺失");
    TEST_ASSERT_TRUE_MESSAGE(
        vrt_file_contains("firmware/main/application.cc", "SetChatMessage(\"assistant\""),
        "VRT-A2 RED: sentence_start text no longer syncs to the display as the "
        "assistant message — 句级文字同步上屏缺失 (state-speaking 文字面, spec §4.0)");
    TEST_ASSERT_TRUE_MESSAGE(
        vrt_file_contains("firmware/main/application.cc", "SetChatMessage(\"user\""),
        "VRT-A2 RED: stt text no longer syncs to the display as the user message — "
        "识别文字上屏缺失 (契约 §四 stt)");
    TEST_ASSERT_TRUE_MESSAGE(
        vrt_file_contains("firmware/main/application.cc", "\"stt\""),
        "VRT-A2 RED(infra): application.cc no longer handles type \"stt\" messages");
    TEST_ASSERT_TRUE_MESSAGE(
        vrt_file_contains("firmware/main/application.cc", "kDeviceStateSpeaking"),
        "VRT-A2 RED(infra): kDeviceStateSpeaking gone — tts start 播报态缺失");
    TEST_ASSERT_TRUE_MESSAGE(
        vrt_file_contains("firmware/main/application.cc", "kDeviceStateIdle"),
        "VRT-A2 RED(infra): tts stop 的回 Idle/Listening 当轮结束面缺失");
}

/* -------------------------------------------------------------------------- *
 * VRT-A5 — 网关兜底设备侧消费（device face）
 * 生成逻辑归 G（fallback.py 已 landed, GWA-A7）；本包验设备侧消费与显示：
 *   - behavioral: 兜底「错误兜底 + text 视图当轮结束」的 text 视图与 A3 同一面
 *     消费（route("text") == TEXT — 网关 error_fallback_text_view() 下发的就是
 *     这个载荷形状, python 孪生验收钉生成端）；安抚句「到达即播」= 原生 tts
 *     sentence_start 流水线不被门控（A2 的绿色守卫覆盖）；
 *   - behavioral: 格式不遵循降级 = 独立 UNKNOWN 路由（非 text 兜底, 非崩溃）；
 *   - native guard: tts stop 后回 Idle/Listening（当轮结束, spec §4.0）。
 * RED today: the behavioral teeth hit the UNIMPLEMENTED sentinel (消费面缺失),
 * which is exactly the device-side gap this package closes.
 * ------------------------------------------------------------------------ */
TEST_CASE("voice roundtrip b: 网关兜底设备侧消费 (VRT-A5)", "[voice_rt][VRT-A5]")
{
    int r;

    VRT_REQUIRE_ROOT();

    /* 错误兜底 text 视图当轮结束：与 A3 同一 text 渲染面消费（网关下发
     * {"view":"text","data":{"text":ERROR_FALLBACK}}；生成端形状由 python
     * 孪生验收 test_a5_cloud_fallback_alignment 钉死） */
    r = kidpal_display_route("text");
    vrt_require_implemented(r, "kidpal_display_route unimplemented — 兜底 text 视图"
                               "消费面缺失");
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_ROUTE_TEXT,
        "VRT-A5 RED: the gateway error-fallback text view is not consumable — 兜底"
        "载荷 {\"view\":\"text\"} 必须走 A3 的 text 渲染面 (spec §4.0 错误兜底行)");

    /* 格式不遵循降级：独立 UNKNOWN 路由（friendly degrade, 不入盲渲染） */
    r = kidpal_display_route("garbage-view");
    TEST_ASSERT_TRUE_MESSAGE(r == VRT_ROUTE_UNKNOWN,
        "VRT-A5 RED: 格式不遵循降级 must surface as the distinct UNKNOWN route — "
        "降级友好提示路径缺失或与正常渲染混流 (V27 / spec §4.0)");

    /* 当轮结束 native guard：tts stop -> 回 Idle/Listening */
    TEST_ASSERT_TRUE_MESSAGE(
        vrt_file_contains("firmware/main/application.cc", "kDeviceStateIdle") &&
        vrt_file_contains("firmware/main/application.cc", "\"stop\""),
        "VRT-A5 RED(infra): tts stop -> Idle/Listening 当轮结束面缺失 — 兜底后设备"
        "会卡在播报态");
}

/* -------------------------------------------------------------------------- *
 * VRT-A6 — abort 发送路径（native green-guards, device face）
 * 喊停/打断时设备发 abort（契约 §四 端→云）。The send path is xiaozhi-native
 * and already wired: Protocol::SendAbortSpeaking builds
 * {"session_id":…,"type":"abort"…}; Application::AbortSpeaking is invoked from
 * the barge-in call sites (KEY 打断 + 唤醒词/人声打断). These teeth pin the
 * native face as MUST-STAY-GREEN (>= 3 occurrences of AbortSpeaking( in
 * application.cc = definition + at least two real barge-in call sites).
 * 云端取消生成 + 本地 AEC+VAD 即时停播主地位不变: cloud leg asserted by the
 * python twin's abort evidence test; 本地停播主地位由 9a 真机实测承载.
 * ------------------------------------------------------------------------ */
TEST_CASE("voice roundtrip b: abort 发送路径 (VRT-A6)", "[voice_rt][VRT-A6]")
{
    int n;

    VRT_REQUIRE_ROOT();

    TEST_ASSERT_TRUE_MESSAGE(
        vrt_file_contains("firmware/main/protocols/protocol.cc", "SendAbortSpeaking") &&
        vrt_file_contains("firmware/main/protocols/protocol.cc", "abort"),
        "VRT-A6 RED(infra): Protocol::SendAbortSpeaking gone — abort 消息发送路径被破坏 "
        "(契约 §四 端→云)");
    TEST_ASSERT_TRUE_MESSAGE(
        vrt_file_contains("firmware/main/protocols/protocol.cc", "session_id"),
        "VRT-A6 RED(infra): abort message no longer carries session_id — 云端无法关联"
        "取消的会话");
    TEST_ASSERT_TRUE_MESSAGE(
        vrt_file_contains("firmware/main/protocols/protocol.h", "SendAbortSpeaking"),
        "VRT-A6 RED(infra): SendAbortSpeaking declaration missing from protocol.h");

    n = vrt_count_in_file("firmware/main/application.cc", "AbortSpeaking(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        "VRT-A6 RED: AbortSpeaking call sites in application.cc < 3 (definition + at "
        "least two barge-in paths) — 打断/喊停路径未全部发 abort (契约 §四; KEY 打断 "
        "+ 人声打断)");
}

/* -------------------------------------------------------------------------- *
 * VRT-A2/E2 — 流式语音回环 live 冒烟证据（evidence-based, operator-produced）
 * The live streaming round trip (stt 识别回传 -> OpenClaw -> 句级 tts 下行播放 +
 * 文字同步上屏 state-speaking) cannot be scripted here; the acceptance asserts
 * the REAL evidence record exists under delivery/changes/dev/260829-voice-roundtrip/
 * (a file named *roundtrip* or *evidence*) and carries the protocol FACTS of
 * contract §四 — same marker口径 as the WSA-A4 human-authorized amendment
 * (permessage-deflate hides wire literals; each fact accepts the markers this
 * stack actually emits):
 *   fact 1 wss 会话      : "session_id"
 *   fact 2 ASR 识别回传   : "识别文本" | ">>" | "stt"
 *   fact 3 TTS 语音生成   : "语音生成成功" | "发送第一段语音" | "tts"
 *   fact 4 句级下发      : "SentenceType" | "发送第一段语音" | "sentence_start"
 *   fact 5 文字同步上屏   : "state-speaking" | "文字同步" | "上屏"
 * RED today: the evidence dir does not exist — the live smoke has not happened.
 * ------------------------------------------------------------------------ */
TEST_CASE("voice roundtrip b: 流式语音回环 live 冒烟证据 (VRT-A2/E2 evidence)",
          "[voice_rt][VRT-A2][evidence]")
{
    static const char *const name_needles[] = { "roundtrip", "evidence" };
    static const char *const content_groups[] = {
        "session_id",
        "识别文本|>>|stt",
        "语音生成成功|发送第一段语音|tts",
        "SentenceType|发送第一段语音|sentence_start",
        "state-speaking|文字同步|上屏",
    };
    char hit[VRT_MAX_PATH];

    VRT_REQUIRE_ROOT();

    TEST_ASSERT_TRUE_MESSAGE(
        vrt_find_evidence("delivery/changes/dev/260829-voice-roundtrip", name_needles, 2,
                          content_groups, 5, hit, sizeof(hit)),
        "VRT-A2/E2 RED: no qualified roundtrip evidence record under "
        "delivery/changes/dev/260829-voice-roundtrip/ — 流式语音回环 live 冒烟（含句级播放与"
        "文字同步上屏 state-speaking）未执行或证据不合格（五事实标记缺一即红, "
        "「没报错」不算过）");
}

/* -------------------------------------------------------------------------- *
 * boundary / security — 扫描器自检（防假绿）+ 生成端范围守卫
 *   - scanner integrity: a real file exists, a bogus file does not, an unreadable
 *     path reads NULL, a needle that matches nothing really matches nothing,
 *     and the occurrence counter counts a known literal exactly — the negative
 *     teeth elsewhere must be CAPABLE of failing;
 *   - 兜底生成面禁入固件 (F2 归属裁定): the ERROR_FALLBACK literal must NEVER be
 *     hard-coded in firmware/main — the device CONSUMES the gateway's fallback
 *     payloads, it does not generate them (生成逻辑归 G);
 *   - 设备不直连 OpenClaw (WSA 守卫对偶, must stay green).
 * ------------------------------------------------------------------------ */
TEST_CASE("voice roundtrip b: 扫描器自检 + 生成端范围守卫 (boundary/security)",
          "[voice_rt][boundary][security]")
{
    char hit[VRT_MAX_PATH];
    int n;

    VRT_REQUIRE_ROOT();

    /* scanner integrity */
    TEST_ASSERT_TRUE_MESSAGE(vrt_exists("firmware/main/protocols/protocol.h"),
        "boundary RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(vrt_exists("firmware/main/protocols/__vrt_no_such_file__.h"),
        "boundary RED(infra): scanner reports a nonexistent file as present");
    TEST_ASSERT_NULL_MESSAGE(vrt_read("firmware/main/protocols/__vrt_no_such_file__.h", NULL),
        "boundary RED(infra): vrt_read returned data for a nonexistent file");
    n = vrt_scan_count("firmware/main/protocols", "__vrt_no_such_needle__", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "boundary RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    n = vrt_count_in_file("firmware/main/protocols/protocol.cc", "SendAbortSpeaking");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "boundary RED(infra): occurrence counter cannot count a known literal — 计数器失灵");

    /* 生成端范围守卫：兜底文案生成面禁入固件（F2: 生成逻辑归 G, 设备只消费） */
    n = vrt_scan_count("firmware/main", "网络出小差了", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "security RED: the gateway ERROR_FALLBACK literal is hard-coded in firmware "
        "(兜底文案生成逻辑归 G / fallback.py — 设备只消费网关下发的兜底载荷, "
        "F2 归属裁定; 固件自造兜底 = 双源漂移)");

    /* 设备不直连 OpenClaw（WSA 守卫对偶, must stay green） */
    n = vrt_scan_count("firmware/main", "openclaw", 1, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "security RED: firmware references openclaw directly — 设备只连自建网关");
    n = vrt_scan_count("firmware/main", "18789", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "security RED: firmware references port 18789 — OpenClaw 端口不得出现在设备侧");
}
