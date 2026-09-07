/*
 * Frozen acceptance tests — TaskPackage 260829-schedule-p2
 * (subsystem: firmware+server; 课表页 今天/明天渲染 + 语音问课表滚动定位 + 家长录入→可消费链路;
 *  mergeOrder 9; dependsOn 260829-host-test-infra + 260829-voice-roundtrip).
 *
 * Nature: host-runnable structural + BEHAVIORAL frozen acceptance on the device
 * P2 课表页 consumption face, following the voice_roundtrip_b.acceptance.c (VRT)
 * frozen-offline-contract precedent: the builder MUST implement the pure-logic
 * contract below (weak sentinels here are overridden by the strong definitions
 * once the implementation is wired into the host runner), so the red baseline is
 * RED on real missing behaviour, never on a build/link failure.
 *
 * ===========================================================================
 * FROZEN OFFLINE CONTRACT (builder MUST implement, pure C, host-compilable, no
 * chip/FreeRTOS/ESP-component deps, self-contained minimal JSON parse — NO
 * external JSON library dependency; recommended home:
 * firmware/main/display/schedule_page.{c,h}; builder also wires BOTH the
 * implementation source and THIS acceptance file into HOST_ACCEPTANCE_SRCS in
 * firmware/main/test/host_app/main/CMakeLists.txt — same one-line pattern as
 * packages 2/3 did for view_router.c / key_gesture_parser.cc):
 *
 *   typedef struct {
 *       char time[16];         -- "HH:MM" or "HH:MM-HH:MM" as issued by producer
 *       char name[48];         -- 课程/事项名 (UTF-8 bytes, safe-truncated)
 *       char location[48];     -- 地点; payload 缺省该项时为 ""
 *   } kidpal_schedule_item_t;
 *
 *   typedef enum { KSP_DAY_TODAY = 0, KSP_DAY_TOMORROW = 1 } ksp_day_t;
 *
 *   int kidpal_schedule_parse(const char *data_json, char *title, size_t title_cap,
 *                             kidpal_schedule_item_t *items, int cap);
 *       Parse the data object of a contract §1.3 ② schedule view payload
 *       ({"title":…,"items":[{"time","name","location"}…]}).  data_json is the
 *       data OBJECT string (what mcp_server.cc's kidpal/display handler holds as
 *       the `data` argument), not the full {view,…} envelope.  Returns the item
 *       count (>=0); -1 on: NULL data_json / malformed JSON / missing or
 *       non-string "title" / "items" not an array / any item not an object or
 *       missing non-string "time" or "name" / cap < item count.  Tolerant:
 *       missing "location" parses as ""; extra unknown fields ignored (additive
 *       tolerance, 契约 data shape 不锁死扩展位).  title may be NULL (caller
 *       does not want it) but the payload must still carry a title.  Title/
 *       field copies are safe-truncated to the destination cap (always NUL-
 *       terminated, never overflow).  A -1 result is the device degrade signal:
 *       the caller MUST show a friendly degrade hint and MUST NOT blind-render
 *       (V27 / spec §4.0 same discipline as the view whitelist).
 *
 *   int kidpal_schedule_dot_state(const char *start_hhmm, const char *now_hhmm,
 *                                 int is_tomorrow);
 *       P2-A3 圆点两态 (spec S-F1.2: 空心=已上完/明天、实心=待上).  Returns
 *       0 = 空心 (hollow), 1 = 实心 (solid), -1 defensive (NULL args).
 *       start_hhmm may be "HH:MM" or "HH:MM-HH:MM" (leading HH:MM decides).
 *       Semantics (frozen): is_tomorrow != 0 -> 0 (明天全空心, now 无关, NOW NULL
 *       亦返回 0); today: start <= now -> 0 (已开始/已上完), start > now -> 1
 *       (待上).  is_tomorrow == 0 && now_hhmm == NULL -> -1 (cannot decide).
 *       start_hhmm == NULL -> -1.
 *
 *   ksp_day_t kidpal_schedule_page_input(key_id_t key, key_gesture_t g,
 *                                        ksp_day_t current);
 *       P2 页内输入特例 (spec §4.0 全局按键手势表页内特例行): BOOT 长按 =
 *       今天⇄明天切换 -> returns the OPPOSITE of current (标题弹跳切换 by the
 *       render face).  Every other key/gesture combination (KEY long press =
 *       global home, BOOT click = global page-next, KEY click/double-click,
 *       out-of-range key ids) belongs to the global layer: returns current
 *       UNCHANGED.  (key_id_t/key_gesture_t from firmware/main/key_gesture_parser.h;
 *       the global layer's BOOT-LONG-PRESS -> GLOBAL_ACT_NONE routing is package 3's
 *       frozen face — this page-local handler consumes what the global layer
 *       leaves NONE.)
 *
 *   int kidpal_schedule_locate(const kidpal_schedule_item_t *items, int n,
 *                              const char *time, const char *name);
 *       P2-A5 滚动定位: index of the row matching (time, name).  time may be ""
 *       (name-only wildcard); name == NULL or items == NULL or n <= 0 -> -1.
 *       Matching is EXACT on both non-empty terms (course names are
 *       child-facing text — no fuzzy/substring match at this seam).  No match
 *       -> -1.  The voice-query consumption path locates the asked-about row
 *       (producer carries the relevant course in items 顺序/内容 — 契约 data
 *       形状零变更, no new view, no new field; the located row additionally gets
 *       the 星标呼应 draw in the render face).
 *
 *   int kidpal_schedule_page_render(const char *title,
 *                                   const kidpal_schedule_item_t *items, int n,
 *                                   int is_tomorrow, const char *now_hhmm,
 *                                   int locate_row);
 *       P2-A3/A4/A5 render face (pure LVGL on the ACTIVE screen — lv_obj_* and
 *       lv_label/lv_anim only, no Board/Display class deps — so it compiles and
 *       runs in the host linux-target runner).  Clears the active screen first
 *       (idempotent, safe to call repeatedly), draws the title (verbatim from
 *       the payload's title, e.g. 今天的课/明天的课/今天下午) and n rows top-down,
 *       each row = 圆点 + 时间 + 课程名 + 地点 (contract §1.3 ② / S-F1.2), dot
 *       state per kidpal_schedule_dot_state(now_hhmm, is_tomorrow), and when
 *       0 <= locate_row < n draws a STAR marker on that row (P2-A5 星标呼应,
 *       spec S-F1.2/S-F1.3 — 不新增反白高亮态, that state is human-裁砍 and must
 *       NOT be introduced).  Title 切换带弹跳动画 (lv_anim, P2-A4 标题弹跳).
 *       locate_row < 0 -> no star.  Returns n on success; -1 on invalid args
 *       (NULL title, items == NULL with n > 0, n < 0); NEVER crashes.
 * ===========================================================================
 *
 * While the implementation is absent, __attribute__((weak)) defaults below
 * return the sentinel -2 (KSP_UNIMPLEMENTED): every behavioral test asserts
 * NOT-sentinel first, so the red baseline reads "frozen contract not
 * implemented" — a real missing-behaviour red, not a link error.
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef / exitCriteria / sourceRef -> test):
 *   coverage: P2-A1  structural (+S-F1.2 契约 §1.3 ② 消费面) -> test P2-A1
 *               (P2 课表页消费面装配: schedule 页模块存在于设备树 + 编入设备构建
 *               CMakeLists + mcp_server kidpal/display 的 schedule 路由 case 真正
 *               接渲染面(非 default 降级) + 消费调用点 >= 2 文件 + 解析失败降级
 *               语义标记 + view_router 包 2 底座在世守卫)
 *   coverage: P2-A2 (device face: 契约 §1.3 ② 载荷消费) -> test P2-A2-parse
 *               (schedule view data 解析: 正规载荷逐字段 + 畸形/缺字段/越界 cap
 *               全部 -1 降级 + 容忍缺 location/未知扩展字段)
 *   coverage: P2-A3  behavioral -> test P2-A3-dot (圆点两态: 今天已开始=空心/
 *               待上=实心/明天全空心/区间时刻取起时刻/NULL 防御)
 *   coverage: P2-A3  render face -> test P2-A3-render (host-runner-only, lvgl
 *               软渲染真帧: 渲染返回行数 + 帧内容非纯背景 + 今天实心 vs 明天全
 *               空心两帧采样网格可见差异 + 空列表/非法参数不崩) — 需要 lvgl
 *               include path, 独立红基线 stub 编译时被编译排除, 其红态在 builder
 *               接线进 host runner 时兑现 (文件头注记, 如实记录)
 *   coverage: P2-A4  behavioral (+spec §4.0 手势表页内特例) -> test P2-A4
 *               (BOOT 长按切 今天⇄明天 往返; KEY 长按/BOOT 单击/KEY 双击/越界键
 *               一律不改视图 — 全局层语义不被页内吞)
 *   coverage: P2-A5  behavioral (+S-F1.3) -> test P2-A5-locate (定位匹配:
 *               time+name 精确命中/仅 name 通配/未命中 -1/NULL 防御) + 包 2
 *               路由续接守卫 kidpal_display_route("schedule")==VRT_ROUTE_SCHEDULE
 *               (host-runner-only, weak 指针判空跳过)
 *   coverage: P2-E3  (不做当前课反白高亮) / P2-E4 (remind_before_min 废止不消费)
 *               -> test boundary (设备树零 remind_before_min 消费 + 零
 *               alerts/pending.json 引用(提醒投递归包 7) + 扫描器自检防假绿)
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.2]/[C4.4]):
 *   0) builder wiring: add "../../schedule_p2.acceptance.c" AND the
 *      schedule_page implementation source to HOST_ACCEPTANCE_SRCS in
 *      firmware/main/test/host_app/main/CMakeLists.txt.
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
 *   Every TEST_CASE must execute and pass; the runner must exit 0.
 *   (Freeze-phase red baseline used the standalone stub per
 *    firmware/main/test/acceptance/red-baseline/ — see
 *    firmware/main/test/acceptance/red-baseline/schedule-p2-red-baseline-summary.md.)
 *
 * Python twin: server/acceptance/test_schedule_p2.acceptance.py — B1 课表 MD
 *   解析/筛选/决策 26 producer 分组/录入消费链路 (P2-A2 cloud face, P2-A6,
 *   P2-E4) 断言在那边承载; 两面证据合起来才是 P2-E2.
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <assert.h>

/* 包 2 / 包 3 的共享底座头（相对路径, host runner 与独立 stub 编译都可达）。 */
#include "../key_gesture_parser.h"
#include "../display/view_router.h"

#define KSP_MAX_PATH 1024
#define KSP_MAX_READ (8u * 1024u * 1024u)

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — constants + weak defaults.
 * ------------------------------------------------------------------------ */
#define KSP_UNIMPLEMENTED (-2) /* weak-default sentinel — never a pass */

typedef struct {
    char time[16];
    char name[48];
    char location[48];
} kidpal_schedule_item_t;

typedef enum { KSP_DAY_TODAY = 0, KSP_DAY_TOMORROW = 1 } ksp_day_t;

__attribute__((weak)) int kidpal_schedule_parse(const char *data_json, char *title,
                                                size_t title_cap,
                                                kidpal_schedule_item_t *items, int cap)
{
    (void)data_json; (void)title; (void)title_cap; (void)items; (void)cap;
    return KSP_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_schedule_dot_state(const char *start_hhmm,
                                                    const char *now_hhmm, int is_tomorrow)
{
    (void)start_hhmm; (void)now_hhmm; (void)is_tomorrow;
    return KSP_UNIMPLEMENTED;
}

__attribute__((weak)) ksp_day_t kidpal_schedule_page_input(key_id_t key, key_gesture_t g,
                                                           ksp_day_t current)
{
    (void)key; (void)g; (void)current;
    return (ksp_day_t)KSP_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_schedule_locate(const kidpal_schedule_item_t *items, int n,
                                                 const char *time, const char *name)
{
    (void)items; (void)n; (void)time; (void)name;
    return KSP_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_schedule_page_render(const char *title,
                                                      const kidpal_schedule_item_t *items,
                                                      int n, int is_tomorrow,
                                                      const char *now_hhmm, int locate_row)
{
    (void)title; (void)items; (void)n; (void)is_tomorrow; (void)now_hhmm; (void)locate_row;
    return KSP_UNIMPLEMENTED;
}

/* 包 2 底座的真实强符号（host runner 里由 view_router.c 提供；独立 stub 编译时
 * 未链接 —— 以 weak 指针判空跳过，见 P2-A5 测试内注记）。 */
extern __attribute__((weak)) int kidpal_display_route(const char *view);

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers (host-side structural teeth), VRT/WSA precedent.
 * All scans run against the REAL working tree located from the runner cwd.
 * Directories named test / host_app / build / .git / assets are EXCLUDED from
 * every scan (test/ hosts THIS frozen file whose comments legitimately mention
 * the frozen needles; host_app/build are build artifacts; assets are binary
 * art, not implementation surface).
 * ------------------------------------------------------------------------ */

static char ksp_root[KSP_MAX_PATH];
static int ksp_root_ready;

static int ksp_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int ksp_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int ksp_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int ksp_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KSP_MAX_PATH];

    if (ksp_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (ksp_is_regular(probe)) {
            snprintf(ksp_root, sizeof(ksp_root), "%s", cands[i]);
            ksp_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int ksp_rel(char *out, size_t cap, const char *rel)
{
    if (!ksp_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", ksp_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", ksp_root, rel);
    return 1;
}

static int ksp_exists(const char *rel)
{
    char p[KSP_MAX_PATH];
    return ksp_rel(p, sizeof(p), rel) && ksp_is_regular(p);
}

static char *ksp_read(const char *rel, size_t *out_len)
{
    char p[KSP_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!ksp_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KSP_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int ksp_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int ksp_contains_ci(const char *hay, const char *needle)
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

static int ksp_file_contains(const char *rel, const char *needle)
{
    char *buf = ksp_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = ksp_contains(buf, needle);
    free(buf);
    return r;
}

static int ksp_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = ksp_read(rel, NULL);
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

static int ksp_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
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
        char abs_next[KSP_MAX_PATH];
        char rel_next[KSP_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            ksp_is_skipped_dir(e->d_name)) {
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
            count += ksp_count_files_with_needle(abs_next, rel_next, needle, ci,
                                                 first_hit, cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > KSP_MAX_READ) {
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
                int m = ci ? ksp_contains_ci(buf, needle) : ksp_contains(buf, needle);
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

static int ksp_scan_count(const char *rel_dir, const char *needle, int ci,
                          char *first_hit, size_t cap)
{
    char abs[KSP_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!ksp_rel(abs, sizeof(abs), rel_dir) || !ksp_is_dir(abs)) {
        return 0;
    }
    return ksp_count_files_with_needle(abs, rel_dir, needle, ci, first_hit, cap, 0);
}

#define KSP_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(ksp_locate_root(), \
    "KSP RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* Shared red-string for the frozen offline contract not being implemented yet. */
static void ksp_require_implemented(int value, const char *what)
{
    if (value == KSP_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "KSP RED: frozen offline contract NOT implemented — %s "
                 "(builder must land the pure-logic schedule page per the frozen "
                 "contract in firmware/main/test/schedule_p2.acceptance.c header "
                 "and wire it into HOST_ACCEPTANCE_SRCS)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* Canonical schedule view payload data (契约 §1.3 ② 示例), as the mcp_server.cc
 * kidpal/display handler holds it: the data OBJECT as a JSON string. */
static const char *KSP_PAYLOAD_CANONICAL =
    "{\"title\":\"今天下午\",\"items\":"
    "[{\"time\":\"14:00\",\"name\":\"钢琴课\",\"location\":\"琴行\"},"
    "{\"time\":\"16:30\",\"name\":\"英语培训\",\"location\":\"新东方\"}]}";

/* -------------------------------------------------------------------------- *
 * P2-A1 — P2 课表页消费面装配结构（structural, +S-F1.2 / 契约 §1.3 ②）
 * Teeth, each a REAL repo fact:
 *   1) a schedule page module exists in the device tree referencing the frozen
 *      contract symbol kidpal_schedule_parse( — today NOTHING under
 *      firmware/main implements the P2 课表页 contract;
 *   2) the module is compiled into the DEVICE build: firmware/main/CMakeLists.txt
 *      lists the file's basename (assembly fact, not a floating file);
 *   3) the mcp kidpal/display handler really WIRES the schedule route to the
 *      render face: mcp_server.cc carries a "schedule" route ack (today routes
 *      3..6 fall into the default degrade — no schedule face exists);
 *   4) the face is REACHED: >= 2 distinct files reference the render contract
 *      symbol kidpal_schedule_page_render( (definition + consumer call site);
 *      defined-but-never-called fails this tooth;
 *   5) the parse contract carries the explicit degrade semantic (-1 -> friendly
 *      hint, never blind render, V27 discipline on the data shape);
 *   6) the title 弹跳 animation face exists in the render module (lv_anim —
 *      P2-A4 标题弹跳切换);
 *   guards (must stay green): 包 2 路由底座在世 — mcp_server.cc includes
 *      display/view_router.h, firmware/main/CMakeLists.txt 编入 view_router.c;
 *      kidpal/display 工具在 mcp_server.cc 注册。
 * RED today: teeth 1)-6) have no implementation; guards green (native).
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule p2: P2 课表页消费面装配结构 (P2-A1)", "[schedule][P2-A1][structural]")
{
    char hit[KSP_MAX_PATH];
    int n_parse, n_render;

    KSP_REQUIRE_ROOT();

    /* tooth 1: schedule 页模块存在于设备树 */
    n_parse = ksp_scan_count("firmware/main", "kidpal_schedule_parse(", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n_parse >= 1,
        "KSP-A1 RED: no module under firmware/main implements the frozen schedule "
        "page contract (kidpal_schedule_parse absent) — P2 课表页消费面缺失 "
        "(契约 §1.3 ②, S-F1.2)");

    /* tooth 2: 模块被设备构建引用（CMakeLists 装配，非浮文件） */
    {
        char base[KSP_MAX_PATH];
        const char *slash = strrchr(hit, '/');
        snprintf(base, sizeof(base), "%s", slash ? slash + 1 : hit);
        TEST_ASSERT_TRUE_MESSAGE(ksp_file_contains("firmware/main/CMakeLists.txt", base),
            "KSP-A1 RED: the schedule page source is not referenced by "
            "firmware/main/CMakeLists.txt — 模块未编入设备构建（装配缺失）");
    }

    /* tooth 3: mcp kidpal/display 的 schedule 路由真正接渲染面（非 default 降级） */
    TEST_ASSERT_TRUE_MESSAGE(ksp_file_contains("firmware/main/mcp_server.cc", "\"schedule\""),
        "KSP-A1 RED: mcp_server.cc carries no \"schedule\" route ack — kidpal/display "
        "的 schedule 视图仍落 default 降级分支, P2 渲染面未接入消费链路 "
        "(契约 §1.3 ② / §四 mcp 工具调用)");

    /* tooth 4: 定义 + 消费调用点 >= 2 个文件 */
    n_render = ksp_scan_count("firmware/main", "kidpal_schedule_page_render(", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n_render >= 2,
        "KSP-A1 RED: kidpal_schedule_page_render is defined but never called — P2 渲染"
        "面未接入 mcp 消费链路（仅定义不调用不算接入）");

    /* tooth 5: 解析失败 = 显式降级语义标记（绝不盲渲染, V27 纪律在 data 形状上） */
    {
        char *buf = ksp_read(hit, NULL);
        TEST_ASSERT_NOT_NULL_MESSAGE(buf, "KSP-A1 RED: schedule page file unreadable");
        TEST_ASSERT_TRUE_MESSAGE(
            ksp_contains(buf, "降级") || ksp_contains_ci(buf, "degrade") ||
            strstr(buf, "-1") != NULL,
            "KSP-A1 RED: the schedule page module carries no parse-degrade semantic "
            "marker — 解析失败必须显式降级而非盲渲染 (V27 / spec §4.0)");
        free(buf);
    }

    /* tooth 6: P2-A4 标题弹跳动画面存在于渲染模块（lv_anim） */
    {
        char render_hit[KSP_MAX_PATH];
        char *buf;
        (void)ksp_scan_count("firmware/main", "kidpal_schedule_page_render(", 0,
                             render_hit, sizeof(render_hit));
        buf = ksp_read(render_hit, NULL);
        TEST_ASSERT_NOT_NULL_MESSAGE(buf, "KSP-A1 RED: render module unreadable");
        TEST_ASSERT_TRUE_MESSAGE(
            ksp_contains(buf, "lv_anim") || strstr(buf, "弹跳") != NULL,
            "KSP-A1 RED: the render module carries no title bounce animation face "
            "(lv_anim) — P2-A4 标题弹跳切换缺失 (S-F1.2)");
        free(buf);
    }

    /* guards (must stay green): 包 2 路由底座在世, 路由续接不回退 */
    TEST_ASSERT_TRUE_MESSAGE(ksp_file_contains("firmware/main/mcp_server.cc",
                                               "display/view_router.h"),
        "KSP-A1 RED(infra): mcp_server.cc no longer includes display/view_router.h — "
        "包 2 路由底座被移除");
    TEST_ASSERT_TRUE_MESSAGE(ksp_file_contains("firmware/main/CMakeLists.txt", "view_router.c"),
        "KSP-A1 RED(infra): view_router.c no longer compiled into the device build — "
        "包 2 底座装配被破坏");
    TEST_ASSERT_TRUE_MESSAGE(ksp_file_contains("firmware/main/mcp_server.cc", "kidpal/display"),
        "KSP-A1 RED(infra): kidpal/display tool no longer registered in mcp_server.cc — "
        "display 下发到达面被破坏");
}

/* -------------------------------------------------------------------------- *
 * P2-A2 (device face) — schedule view 载荷解析（behavioral, frozen contract）
 * Exercises kidpal_schedule_parse against the canonical contract §1.3 ② payload
 * and the degrade/edge paths.
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule p2: schedule view 载荷解析与降级 (P2-A2 device face)",
          "[schedule][P2-A2]")
{
    kidpal_schedule_item_t items[8];
    char title[64];
    int n;

    /* canonical payload: 2 items, fields exact, title extracted */
    memset(title, 0, sizeof(title));
    memset(items, 0, sizeof(items));
    n = kidpal_schedule_parse(KSP_PAYLOAD_CANONICAL, title, sizeof(title), items, 8);
    ksp_require_implemented(n, "kidpal_schedule_parse unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(n == 2,
        "KSP-A2 RED: canonical contract §1.3 ② payload did not parse to exactly 2 items");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(title, "今天下午") == 0,
        "KSP-A2 RED: payload title not extracted verbatim (今天下午)");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(items[0].time, "14:00") == 0 &&
                                 strcmp(items[0].name, "钢琴课") == 0 &&
                                 strcmp(items[0].location, "琴行") == 0,
        "KSP-A2 RED: item 0 fields wrong — time/name/location 必须逐字段保真 (契约 §1.3 ②)");
    TEST_ASSERT_TRUE_MESSAGE(strcmp(items[1].time, "16:30") == 0 &&
                                 strcmp(items[1].name, "英语培训") == 0 &&
                                 strcmp(items[1].location, "新东方") == 0,
        "KSP-A2 RED: item 1 fields wrong — time/name/location 必须逐字段保真 (契约 §1.3 ②)");

    /* tolerant: missing location -> "", extra unknown fields ignored */
    {
        const char *p = "{\"title\":\"今天\",\"items\":"
                        "[{\"time\":\"10:00\",\"name\":\"美术班\",\"extra\":1}]}";
        memset(items, 0, sizeof(items));
        n = kidpal_schedule_parse(p, NULL, 0, items, 8);
        TEST_ASSERT_TRUE_MESSAGE(n == 1,
            "KSP-A2 RED: payload with a missing location / extra unknown field must "
            "parse (location 缺省 -> \"\", additive tolerance) — 不得因扩展位整包拒绝");
        TEST_ASSERT_TRUE_MESSAGE(strcmp(items[0].location, "") == 0,
            "KSP-A2 RED: missing location must parse as empty string");
    }

    /* degrade: malformed JSON -> -1 (friendly degrade, never blind render) */
    n = kidpal_schedule_parse("{\"title\":", title, sizeof(title), items, 8);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSP-A2 RED: malformed JSON must return -1 (degrade), not 0/partial");
    n = kidpal_schedule_parse(NULL, title, sizeof(title), items, 8);
    TEST_ASSERT_TRUE_MESSAGE(n == -1, "KSP-A2 RED: NULL payload must be defended (-1)");

    /* degrade: missing title / non-array items / item missing name -> -1 */
    n = kidpal_schedule_parse("{\"items\":[]}", title, sizeof(title), items, 8);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSP-A2 RED: payload without title must be rejected (-1) — title 是契约 §1.3 ② "
        "data 形状的必带字段");
    n = kidpal_schedule_parse("{\"title\":\"今天\",\"items\":\"x\"}", title, sizeof(title),
                              items, 8);
    TEST_ASSERT_TRUE_MESSAGE(n == -1, "KSP-A2 RED: non-array items must be rejected (-1)");
    n = kidpal_schedule_parse("{\"title\":\"今天\",\"items\":[{\"time\":\"10:00\"}]}",
                              title, sizeof(title), items, 8);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSP-A2 RED: item missing name must be rejected (-1) — 行缺课程名是畸形载荷");

    /* degrade: cap too small for the item count -> -1 (caller must size up) */
    n = kidpal_schedule_parse(KSP_PAYLOAD_CANONICAL, title, sizeof(title), items, 1);
    TEST_ASSERT_TRUE_MESSAGE(n == -1,
        "KSP-A2 RED: cap < item count must return -1, never overflow/silent truncate");

    /* title_out NULL is legal (caller does not want it) */
    n = kidpal_schedule_parse(KSP_PAYLOAD_CANONICAL, NULL, 0, items, 8);
    TEST_ASSERT_TRUE_MESSAGE(n == 2,
        "KSP-A2 RED: NULL title_out must be tolerated (item count still returned)");

    /* empty items array is a valid (empty) page */
    n = kidpal_schedule_parse("{\"title\":\"今天\",\"items\":[]}", title, sizeof(title),
                              items, 8);
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "KSP-A2 RED: empty items array must parse to 0 items (空课表合法页)");
}

/* -------------------------------------------------------------------------- *
 * P2-A3 — 圆点两态（behavioral, spec S-F1.2: 空心=已上完/明天、实心=待上）
 * RED today: weak-default UNIMPLEMENTED sentinel.
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule p2: 圆点两态——空心=已上完/明天, 实心=待上 (P2-A3)", "[schedule][P2-A3]")
{
    int d;

    /* today: already started -> hollow (已上完) */
    d = kidpal_schedule_dot_state("10:00", "12:00", 0);
    ksp_require_implemented(d, "kidpal_schedule_dot_state unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(d == 0,
        "KSP-A3 RED: a today item whose start already passed must be 空心 (已上完)");

    /* today: upcoming -> solid (待上) */
    d = kidpal_schedule_dot_state("16:30", "12:00", 0);
    TEST_ASSERT_TRUE_MESSAGE(d == 1,
        "KSP-A3 RED: a today item not yet started must be 实心 (待上)");

    /* today: exactly now -> already started -> hollow */
    d = kidpal_schedule_dot_state("12:00", "12:00", 0);
    TEST_ASSERT_TRUE_MESSAGE(d == 0,
        "KSP-A3 RED: start == now must count as 已开始 (空心)");

    /* tomorrow: all hollow regardless of now (S-F1.2: 切明天圆点全变空心) */
    d = kidpal_schedule_dot_state("16:30", "12:00", 1);
    TEST_ASSERT_TRUE_MESSAGE(d == 0,
        "KSP-A3 RED: a tomorrow item must always be 空心 (S-F1.2 明天全空心)");
    d = kidpal_schedule_dot_state("16:30", NULL, 1);
    TEST_ASSERT_TRUE_MESSAGE(d == 0,
        "KSP-A3 RED: tomorrow view must not require now (NULL now -> still 空心)");

    /* range form "HH:MM-HH:MM": leading HH:MM decides */
    d = kidpal_schedule_dot_state("16:30-17:30", "12:00", 0);
    TEST_ASSERT_TRUE_MESSAGE(d == 1,
        "KSP-A3 RED: range time form must use its leading HH:MM (16:30 > 12:00 -> 实心)");
    d = kidpal_schedule_dot_state("10:00-11:30", "12:00", 0);
    TEST_ASSERT_TRUE_MESSAGE(d == 0,
        "KSP-A3 RED: range time form must use its leading HH:MM (10:00 <= 12:00 -> 空心)");

    /* defensive */
    d = kidpal_schedule_dot_state(NULL, "12:00", 0);
    TEST_ASSERT_TRUE_MESSAGE(d == -1, "KSP-A3 RED: NULL start must be defended (-1)");
    d = kidpal_schedule_dot_state("16:30", NULL, 0);
    TEST_ASSERT_TRUE_MESSAGE(d == -1,
        "KSP-A3 RED: NULL now on the TODAY view cannot decide a dot state (-1)");
}

/* -------------------------------------------------------------------------- *
 * P2-A4 — BOOT 长按 今天⇄明天切换（behavioral, spec §4.0 手势表页内特例）
 * The page-local handler consumes exactly what the global layer leaves NONE
 * (BOOT LONG_PRESS -> GLOBAL_ACT_NONE is package 3's frozen route); every other
 * gesture belongs to the global layer and must NOT be swallowed here.
 * RED today: weak-default UNIMPLEMENTED sentinel (returns -2 cast into ksp_day_t).
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule p2: BOOT 长按切 今天⇄明天, 其余手势不吞 (P2-A4)", "[schedule][P2-A4]")
{
    ksp_day_t v;

    /* BOOT 长按: today -> tomorrow */
    v = kidpal_schedule_page_input(KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS, KSP_DAY_TODAY);
    ksp_require_implemented((int)v, "kidpal_schedule_page_input unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(v == KSP_DAY_TOMORROW,
        "KSP-A4 RED: BOOT long press on the today view did not switch to tomorrow "
        "(spec §4.0 页内特例: P2=今天⇄明天切换)");

    /* BOOT 长按: tomorrow -> today (往返) */
    v = kidpal_schedule_page_input(KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS, KSP_DAY_TOMORROW);
    TEST_ASSERT_TRUE_MESSAGE(v == KSP_DAY_TODAY,
        "KSP-A4 RED: BOOT long press on the tomorrow view did not switch back to today");

    /* KEY 长按 = 全局回首页, 页内不得吞 */
    v = kidpal_schedule_page_input(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS, KSP_DAY_TODAY);
    TEST_ASSERT_TRUE_MESSAGE(v == KSP_DAY_TODAY,
        "KSP-A4 RED: KEY long press (global HOME) must NOT be consumed by the page — "
        "全局手势被页内吞掉会破坏「所有页面一致」");

    /* BOOT 单击 = 全局翻页, 页内不得吞 */
    v = kidpal_schedule_page_input(KEY_ID_BOOT, KEY_GESTURE_CLICK, KSP_DAY_TODAY);
    TEST_ASSERT_TRUE_MESSAGE(v == KSP_DAY_TODAY,
        "KSP-A4 RED: BOOT click (global PAGE_NEXT) must NOT toggle the day view");

    /* KEY 双击 = 全局无语义 / P3 特例, P2 内同样不得切换 */
    v = kidpal_schedule_page_input(KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK, KSP_DAY_TODAY);
    TEST_ASSERT_TRUE_MESSAGE(v == KSP_DAY_TODAY,
        "KSP-A4 RED: KEY double click must not toggle the day view on P2");

    /* 越界键位 (PWR 模拟) -> 状态零污染 */
    v = kidpal_schedule_page_input(KEY_ID_COUNT, KEY_GESTURE_LONG_PRESS, KSP_DAY_TODAY);
    TEST_ASSERT_TRUE_MESSAGE(v == KSP_DAY_TODAY,
        "KSP-A4 RED: out-of-range key id must be ignored (state untouched)");
}

/* -------------------------------------------------------------------------- *
 * P2-A5 — 语音问课表滚动定位（behavioral, S-F1.3）+ 包 2 路由续接守卫
 * RED today: weak-default UNIMPLEMENTED sentinel (locate); route guard is
 * host-runner-only (weak pointer check — skipped in the standalone stub run).
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule p2: 语音问课表滚动定位 + 路由续接 (P2-A5)", "[schedule][P2-A5]")
{
    kidpal_schedule_item_t items[2];
    char title[64];
    int idx, r;

    memset(title, 0, sizeof(title));
    memset(items, 0, sizeof(items));
    r = kidpal_schedule_parse(KSP_PAYLOAD_CANONICAL, title, sizeof(title), items, 2);
    ksp_require_implemented(r, "kidpal_schedule_parse unimplemented");

    /* time + name 精确命中 */
    idx = kidpal_schedule_locate(items, r, "14:00", "钢琴课");
    TEST_ASSERT_TRUE_MESSAGE(idx == 0,
        "KSP-A5 RED: locate(14:00, 钢琴课) must hit row 0 — 滚动定位匹配缺失 (S-F1.3)");
    idx = kidpal_schedule_locate(items, r, "16:30", "英语培训");
    TEST_ASSERT_TRUE_MESSAGE(idx == 1,
        "KSP-A5 RED: locate(16:30, 英语培训) must hit row 1");

    /* 仅 name 通配（time 为空串） */
    idx = kidpal_schedule_locate(items, r, "", "钢琴课");
    TEST_ASSERT_TRUE_MESSAGE(idx == 0,
        "KSP-A5 RED: name-only wildcard (empty time) must still locate the row");

    /* 未命中 -> -1 */
    idx = kidpal_schedule_locate(items, r, "14:00", "游泳课");
    TEST_ASSERT_TRUE_MESSAGE(idx == -1,
        "KSP-A5 RED: a non-existent course must locate to -1 (不误定位到别的行)");
    idx = kidpal_schedule_locate(items, r, "16:30", "钢琴课");
    TEST_ASSERT_TRUE_MESSAGE(idx == -1,
        "KSP-A5 RED: time+name must BOTH match (精确匹配, 不做子串模糊)");

    /* defensive */
    idx = kidpal_schedule_locate(NULL, 2, "14:00", "钢琴课");
    TEST_ASSERT_TRUE_MESSAGE(idx == -1, "KSP-A5 RED: NULL items must be defended (-1)");
    idx = kidpal_schedule_locate(items, 0, "14:00", "钢琴课");
    TEST_ASSERT_TRUE_MESSAGE(idx == -1, "KSP-A5 RED: n=0 must locate to -1");
    idx = kidpal_schedule_locate(items, 2, "14:00", NULL);
    TEST_ASSERT_TRUE_MESSAGE(idx == -1, "KSP-A5 RED: NULL name must be defended (-1)");

    /* 包 2 路由续接不回退: view "schedule" 仍路由到 VRT_ROUTE_SCHEDULE。
     * host-runner-only: 独立 stub 编译未链接 view_router.c, 弱符号为 NULL 时跳过。 */
    if (kidpal_display_route != NULL) {
        r = kidpal_display_route("schedule");
        TEST_ASSERT_TRUE_MESSAGE(r == VRT_ROUTE_SCHEDULE,
            "KSP-A5 RED: view \"schedule\" no longer routes to VRT_ROUTE_SCHEDULE — "
            "包 2 共享路由底座续接被破坏 (VRT-E3, 契约 §1.3 ②)");
    }
}

/* -------------------------------------------------------------------------- *
 * P2-A3 render face — host-runner-only LVGL 软渲染真帧冒烟（evidence for
 * P2-A3/A4 渲染面; 真实屏幕观感殿后 9b, P2-E2 口径）。
 * 机制: 先经包 0 冻结渲染缝 host_infra_render_frame() 初始化 LVGL 离屏显示并置
 * rendered 态, 再调被测渲染契约函数在活动屏上真画, lv_refr_now 刷新后用
 * host_infra_read_pixel 采样网格探针断言真实像素。requires lvgl.h — 独立红基线
 * stub 编译无 lvgl include path, 本测试被编译排除（如实记录, 其红态在 builder
 * 接线进 host runner 时兑现）。
 * ------------------------------------------------------------------------ */
#if defined(__has_include)
#if __has_include("lvgl.h")
#define KSP_HAVE_LVGL 1
#include <lvgl.h>
#endif
#endif

#ifdef KSP_HAVE_LVGL

extern int host_infra_render_frame(void);
extern int32_t host_infra_read_pixel(int x, int y);

#define KSP_GRID_STEP 20
#define KSP_GRID_W (400 / KSP_GRID_STEP)
#define KSP_GRID_H (300 / KSP_GRID_STEP)

static void ksp_sample_grid(int32_t *out)
{
    int i = 0;
    for (int y = KSP_GRID_STEP / 2; y < 300; y += KSP_GRID_STEP) {
        for (int x = KSP_GRID_STEP / 2; x < 400; x += KSP_GRID_STEP) {
            out[i++] = host_infra_read_pixel(x, y);
        }
    }
}

static void ksp_render_and_sample(const char *title, int is_tomorrow, int32_t *grid)
{
    kidpal_schedule_item_t items[2];
    int n;

    memset(items, 0, sizeof(items));
    snprintf(items[0].time, sizeof(items[0].time), "10:00");
    snprintf(items[0].name, sizeof(items[0].name), "钢琴课");
    snprintf(items[0].location, sizeof(items[0].location), "琴行");
    snprintf(items[1].time, sizeof(items[1].time), "16:30");
    snprintf(items[1].name, sizeof(items[1].name), "英语培训");
    snprintf(items[1].location, sizeof(items[1].location), "新东方");

    n = kidpal_schedule_page_render(title, items, 2, is_tomorrow, "12:00", -1);
    ksp_require_implemented(n, "kidpal_schedule_page_render unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(n == 2,
        "KSP-A3 render RED: render face did not report both rows drawn");

    /* 推进 LVGL 时间并同步刷新, 让渲染缝 flush_cb 把真帧拷进可探针缓冲。 */
    lv_refr_now(NULL);
    ksp_sample_grid(grid);
}

TEST_CASE("schedule p2: P2 页渲染面真帧——行绘制 + 两态可见差异 (P2-A3/A4 render)",
          "[schedule][P2-A3][render]")
{
    static int32_t grid_today[KSP_GRID_W * KSP_GRID_H];
    static int32_t grid_tomorrow[KSP_GRID_W * KSP_GRID_H];
    int diff = 0, dark = 0, uniform_fill = 1;

    /* 包 0 冻结渲染缝: 初始化 + rendered 态 (fill 0x336699)。 */
    TEST_ASSERT_TRUE_MESSAGE(host_infra_render_frame() == 0,
        "KSP render RED(infra): host_infra_render_frame failed — 包 0 渲染缝不在世");

    /* 今天视图 (now 12:00: 10:00 已上完=空心, 16:30 待上=实心) */
    ksp_render_and_sample("今天的课", 0, grid_today);

    /* 帧内容真实性: 采样网格不再全是缝的背景填充色 */
    for (int i = 0; i < KSP_GRID_W * KSP_GRID_H; i++) {
        if (grid_today[i] != 0x336699) {
            uniform_fill = 0;
        }
        if (grid_today[i] < 0xC0C0C0) {
            dark++;
        }
    }
    TEST_ASSERT_FALSE_MESSAGE(uniform_fill,
        "KSP-A3 render RED: frame still uniform seam-fill after render — 渲染面没有真的"
        "画到屏上 (P2-A3 渲染缺失)");
    TEST_ASSERT_TRUE_MESSAGE(dark >= 5,
        "KSP-A3 render RED: too few dark pixels in the sampled grid — 标题/行文本/圆点"
        "内容缺失 (P2-A3: 每行 = 圆点 + 时间 + 课程名 + 地点)");

    /* 明天视图 (全空心): 与今天实心视图的采样网格必须可见差异 (圆点两态在真帧上
     * 可区分 — S-F1.2 切明天圆点全变空心的渲染证据) */
    ksp_render_and_sample("明天的课", 1, grid_tomorrow);
    for (int i = 0; i < KSP_GRID_W * KSP_GRID_H; i++) {
        if (grid_today[i] != grid_tomorrow[i]) {
            diff++;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(diff > 0,
        "KSP-A3 render RED: today(solid dot) and tomorrow(all-hollow) frames are "
        "pixel-identical — 圆点两态在渲染面上不可见 (P2-A3/S-F1.2)");

    /* 空课表 + 非法参数: 不崩 */
    {
        int n = kidpal_schedule_page_render("今天", NULL, 0, 0, "12:00", -1);
        TEST_ASSERT_TRUE_MESSAGE(n == 0,
            "KSP-A3 render RED: empty items must render an empty page (return 0)");
        lv_refr_now(NULL);
        n = kidpal_schedule_page_render(NULL, NULL, 0, 0, "12:00", -1);
        TEST_ASSERT_TRUE_MESSAGE(n == -1,
            "KSP-A3 render RED: NULL title must be rejected (-1), never crash");
        lv_refr_now(NULL);
    }
}

#endif /* KSP_HAVE_LVGL */

/* -------------------------------------------------------------------------- *
 * boundary / security — 扫描器自检（防假绿）+ 本包范围红线守卫
 *   - scanner integrity: real file exists / bogus file does not / unreadable
 *     path reads NULL / no-match needle matches nothing / counter counts a
 *     known literal — the negative teeth elsewhere must be CAPABLE of failing;
 *   - P2-E4: remind_before_min 已废止（契约 §2.3）—— 设备树零消费;
 *   - P2-E4: 提醒投递面（alerts/pending.json, 归包 7）不出现在设备树。
 * ------------------------------------------------------------------------ */
TEST_CASE("schedule p2: 扫描器自检 + remind_before_min/提醒范围守卫 (boundary/security)",
          "[schedule][boundary][security]")
{
    char hit[KSP_MAX_PATH];
    int n;

    KSP_REQUIRE_ROOT();

    /* scanner integrity */
    TEST_ASSERT_TRUE_MESSAGE(ksp_exists("firmware/main/display/view_router.c"),
        "boundary RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(ksp_exists("firmware/main/display/__ksp_no_such_file__.c"),
        "boundary RED(infra): scanner reports a nonexistent file as present");
    TEST_ASSERT_NULL_MESSAGE(ksp_read("firmware/main/display/__ksp_no_such_file__.c", NULL),
        "boundary RED(infra): ksp_read returned data for a nonexistent file");
    n = ksp_scan_count("firmware/main/display", "__ksp_no_such_needle__", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "boundary RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    n = ksp_count_in_file("firmware/main/display/view_router.c", "VRT_ROUTE_SCHEDULE");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "boundary RED(infra): occurrence counter cannot count a known literal — 计数器失灵");

    /* P2-E4: remind_before_min 字段废止, 设备零消费 */
    n = ksp_scan_count("firmware/main", "remind_before_min", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "P2-E4 RED: firmware references remind_before_min — 该字段已废止、不被消费 "
        "(契约 §2.3 2026-09-03 契约门裁定), 设备侧出现即双源漂移");

    /* P2-E4: 提醒投递面归包 7 — 设备树不承载 alerts/pending.json */
    n = ksp_scan_count("firmware/main", "pending.json", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "P2-E4 RED: firmware references alerts/pending.json — 提醒触发/投递逻辑不在本包 "
        "(归包 7), 课表页只到「录入→可消费 + 渲染 + 语音问」");
}

/* --------------------------------------------------------------------------- *
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-07  initial freeze (acceptance-author).
 * --------------------------------------------------------------------------- */
