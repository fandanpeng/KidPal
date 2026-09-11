/*
 * Frozen acceptance tests — TaskPackage 260911-key-route-a1 (subsystem: firmware).
 * KidPal「设备显示与交互主权」workstream PKG-1 键位路线落地（OQ4=A，A1 全局层先行）。
 * mergeOrder 18, riskTier high, testHost = host runner (WSL linux target) +
 * 真机确认轮（human 持板，GPIO18 电平/三手势）。
 *
 * Story: 板级 KEY(GPIO18) 接线（iot_button 三手势注册，消抖复用
 * CONFIG_BUTTON_SHORT_PRESS_TIME_MS=180）→ 喂已冻结的 key_gesture_parser（设备侧
 * 首次接线，现状零调用）→ 全局路由表落地（KEY 单击=对话 / KEY 长按=回首页）→ BOOT
 * 回归「主司翻页」（单击翻页、去对话化）。页内特例（P2/P3/P4/提醒弹窗）= 本包零触碰、
 * A2 随后另立项。本包是 ARQ-260911-cross-package-behavior-owner-gap 首用试金石：
 * KEY 接线端到端 owner 显式登记归本包，机制级齿防「BOOT≈KEY 行为等价」假绿复发。
 *
 * Nature: HOST-SIDE STRUCTURAL (tree-scan) frozen acceptance（hardware_validation.
 * acceptance.c / device_ws_switch.acceptance.c 树扫先例）。本文件**不引入新的 frozen
 * offline contract**——键位枚举 / 手势解析 / 全局路由表纯逻辑已由
 * session_logic.acceptance.c 冻结（SES-A3/A4/A6，全齿 must-stay-green），本文件只钉
 * 它们的**设备侧接线事实**：板级 KEY GPIO 注册 / iot_button 三手势 / 解析器喂入 /
 * 路由 dispatch 接线 / BOOT 去对话化。全部为符号/接线级机制断言（非行为级），源码
 * 扫描断言 GPIO_NUM_18 / key_gesture_parser_feed / key_route_global 符号面，杜绝
 * 「有键能起对话就算过」的 BOOT≈KEY 行为等价假绿。
 *
 * KR-A5（GPIO18 电平/上拉 human 真机确认）是 device-round 验收项，host 齿无法钉——
 * 本文件不尝试真机构建/烧录；该项证据要求落 changes/dev/260911-key-route-a1/（human
 * 持板轮日志/照片，见本文件尾「真机证据要求」）。KR-A6（host runner 终跑 exit 0）与
 * KR-E3（真机证据齐 + 逃生门零触发）是执行/汇总结论，非本文件断言面。
 *
 * ---------------------------------------------------------------------------
 * Seam contract pinned by this file（builder implements，本文件只读扫工作树）:
 *
 *   [1] firmware/main/boards/waveshare-s3-rlcd-4.2/config.h
 *       新增:  #define KEY_BUTTON_GPIO    GPIO_NUM_18
 *       保留:  #define BOOT_BUTTON_GPIO   GPIO_NUM_0   （两键分工不抢占 BOOT 脚）
 *   [2] firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc
 *       新增成员 `Button key_button_;`（与 boot_button_ 并列）；构造器初值列表新增
 *       `key_button_(KEY_BUTTON_GPIO, <active_high>, 0, <short_press>, false)`
 *       —— short_press_time 必须复用 CONFIG_BUTTON_SHORT_PRESS_TIME_MS（=180，与 BOOT
 *       一致），不得另设短按窗（KR-A1 消抖复用）。active_high 由 KR-A5 真机确认裁定。
 *       InitializeButtons() 必须注册 key_button_ 三手势：
 *         key_button_.OnClick(...)        -> KEY 单击=发起对话（路由 SESSION_TOGGLE）
 *         key_button_.OnDoubleClick(...)  -> KEY 双击（全局层无语义，仍须注册接缝）
 *         key_button_.OnLongPress(...)    -> KEY 长按=回首页（路由 HOME）
 *   [3] 设备侧喂入 + 路由 dispatch（板级文件内或新增 firmware/main/ 下路由文件，二者
 *       任一，只要在 firmware/main 非 test 目录内落点）:
 *       key_gesture_parser_init(&...)        对 KEY / BOOT 两解析器实例各一
 *       key_gesture_parser_feed(&..., KEY_ID_KEY,  ts, pressed)   KEY 按键边沿喂入
 *       key_gesture_parser_feed(&..., KEY_ID_BOOT, ts, pressed)   BOOT 按键边沿喂入
 *       key_route_global(KEY_ID_KEY,  g)                       KEY 手势路由
 *       key_route_global(KEY_ID_BOOT, g)                       BOOT 手势路由
 *       route 输出 -> 应用动作 dispatch 接线（route 枚举 literal 须在设备侧被消费）:
 *         GLOBAL_ACT_SESSION_TOGGLE -> Application::ToggleChatState()（先
 *             kidpal_render_node_show_chat() 保语音态可见，语义与现 BOOT 单击一致）
 *         GLOBAL_ACT_HOME           -> kidpal_render_node_show_home()
 *         GLOBAL_ACT_PAGE_NEXT      -> kidpal_render_node_cycle_next()（BOOT 单击翻页）
 *   [4] BOOT 回归（KR-A4）:
 *       boot_button_.OnClick(...) 回调正文去对话化——不得再调用 ToggleChatState() /
 *       kidpal_render_node_show_chat() / HandleStoryKey(KEY_ID_BOOT,
 *       KEY_GESTURE_CLICK)；BOOT 单击经 key_route_global(KEY_ID_BOOT, CLICK) 产出
 *       GLOBAL_ACT_PAGE_NEXT -> 翻页。（冻结路由表已证 KEY_ID_BOOT+CLICK 恒 PAGE_NEXT、
 *       永非 SESSION_TOGGLE，故接线即强制去对话化。）
 *       BOOT 长按（P3 阅读=翻下一屏，页内特例零触碰）必须保留:
 *       HandleStoryKey(KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS) 仍在。
 *   [5] Builder FIRST ACTION: 把本验收文件注册进
 *       firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS
 *       （相对 host_app/main 为 "../../key_route_a1.acceptance.c"），置于
 *       "../../host_infra_smoke.acceptance.c" 之前（smoke 保持列表末位 = 最先执行，
 *       boundary 齿注册序守卫；acceptance-author 被 scope 排除在该文件外，注册动作
 *       归 builder）。本文件结构齿自身在 test 4 tooth 1 校验此注册。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef / exitCriteria -> test):
 *   KR-A1 板级 KEY 接线（GPIO18 + 三手势 + 消抖复用 180）-> test 1（6 齿）
 *   KR-A2 parser 喂入接线（key_gesture_parser_feed/init 设备侧存在 + KEY id 接线）
 *          -> test 2（3 齿）
 *   KR-A3 全局路由表落地（key_route_global KEY/BOOT 调用 + SESSION_TOGGLE/HOME/
 *          PAGE_NEXT 三动作 device 侧 dispatch 消费）-> test 3（5 齿）
 *   KR-A4 BOOT 回归翻页去对话化 + BOOT 长按故事翻屏零触碰守卫 -> test 4（2 齿）
 *   KR-A6 host runner 终跑（本文件注册进 HOST_ACCEPTANCE_SRCS 的结构守卫）
 *          + KR-E2 spec 零修订 canary + 扫描器自检 -> test 5（boundary）
 *   KR-A5 / KR-E1 / KR-E3 -> 本文件不承载（真机/范围纪律/汇总，见头注与尾部).
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   0) builder wiring: add "../../key_route_a1.acceptance.c" to HOST_ACCEPTANCE_SRCS
 *      in firmware/main/test/host_app/main/CMakeLists.txt, BEFORE
 *      "../../host_infra_smoke.acceptance.c".
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
 *   Freeze-phase red baseline (本文件未接线进 ELF 前的双通道口径): 见文件尾
 *   「RED baseline 判读口径」段。
 * ---------------------------------------------------------------------------
 * Red-baseline 判读口径（2026-09-11 acceptance-author 双通道，见文件尾）:
 *   通道① 既有 host runner 全量实跑 = 104 Tests / 4 Failures（WSA-A6:830 /
 *   WSA-A4:663 / P2-A1:571 / VRT-A2·E2:878 四条在册 existing-red，非本包责任，
 *   本包不修），既有齿相对基线零新增红。
 *   通道② 本文件新齿为 C 类冻结验收实编（gcc -c 编译通道 error 为零，即「验收文件
 *   本身可编译」）+ 解析性红定性（GPIO_NUM_18 在 rlcd-4.2 板 config.h 零命中 /
 *   key_gesture_parser_feed(& 设备侧零调用 / key_route_global(KEY_ID_* 零调用 /
 *   路由动作枚举无 device 侧消费点——各齿对现状必红），故注册进 runner 后本文件
 *   每齿红于真实缺失接线，红基线成立。
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
#include <stdarg.h>

#define KEYA_MAX_PATH 1024
#define KEYA_MAX_READ (8u * 1024u * 1024u)

#define KEYA_BOARD_CC "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"
#define KEYA_BOARD_CFG "firmware/main/boards/waveshare-s3-rlcd-4.2/config.h"
#define KEYA_HOST_MK "firmware/main/test/host_app/main/CMakeLists.txt"

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（host 侧结构齿，HWV/WSA 先例）。全部扫描跑在真实
 * 工作树上，root 以手持 cwd 定位（从仓根或 firmware/main/test/host_app 两处都可跑，
 * 见 HOST_TEST_CONVENTIONS.md [C4.4]）。test / host_app / build / .git / assets 目录
 * 排除——test 目录里住着本冻结文件自身（其注释合法提及冻结 needle）。
 * ------------------------------------------------------------------------ */

static char keya_root[KEYA_MAX_PATH];
static int keya_root_ready;

static int keya_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int keya_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int keya_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int keya_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KEYA_MAX_PATH];

    if (keya_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (keya_is_regular(probe)) {
            snprintf(keya_root, sizeof(keya_root), "%s", cands[i]);
            keya_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int keya_rel(char *out, size_t cap, const char *rel)
{
    if (!keya_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", keya_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", keya_root, rel);
    return 1;
}

static int keya_exists(const char *rel)
{
    char p[KEYA_MAX_PATH];
    return keya_rel(p, sizeof(p), rel) && keya_is_regular(p);
}

static char *keya_read(const char *rel, size_t *out_len)
{
    char p[KEYA_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!keya_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KEYA_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int keya_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int keya_file_contains(const char *rel, const char *needle)
{
    char *buf = keya_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = keya_contains(buf, needle);
    free(buf);
    return r;
}

/* Occurrence count of needle inside one file (0 when unreadable/absent). */
static int keya_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = keya_read(rel, NULL);
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

/* True if some LINE of rel contains BOTH macro and value（config.h 单行 #define 绑定
 * KEY_BUTTON_GPIO <-> GPIO_NUM_18 用；不要求同列紧邻，容对齐空白）。 */
static int keya_define_maps_to(const char *rel, const char *macro, const char *value)
{
    char *buf = keya_read(rel, NULL);
    int r = 0;

    assert(macro != NULL && value != NULL);
    if (buf == NULL) {
        return 0;
    }
    char *line = buf;
    while (line != NULL && *line != '\0') {
        char *nl = strchr(line, '\n');
        if (nl != NULL) {
            *nl = '\0';
        }
        if (keya_contains(line, macro) && keya_contains(line, value)) {
            r = 1;
            break;
        }
        if (nl == NULL) {
            break;
        }
        line = nl + 1;
    }
    free(buf);
    return r;
}

/* Count files under rel_dir whose content matches needle (ci=1 case-insensitive). */
static int keya_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
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
        char abs_next[KEYA_MAX_PATH];
        char rel_next[KEYA_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            keya_is_skipped_dir(e->d_name)) {
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
            count += keya_count_files_with_needle(abs_next, rel_next, needle, ci,
                                                  first_hit, cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > KEYA_MAX_READ) {
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
                int m;
                if (ci) {
                    /* case-insensitive contain */
                    size_t n = strlen(needle), j;
                    char *hh;
                    m = 0;
                    if (n == 0) {
                        m = 1;
                    } else {
                        for (hh = buf; *hh != '\0' && !m; hh++) {
                            for (j = 0; j < n && hh[j] != '\0' &&
                                 tolower((unsigned char)hh[j]) == tolower((unsigned char)needle[j]); j++) {
                            }
                            if (j == n) {
                                m = 1;
                            }
                        }
                    }
                } else {
                    m = keya_contains(buf, needle);
                }
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

static int keya_scan_count(const char *rel_dir, const char *needle, int ci,
                           char *first_hit, size_t cap)
{
    char abs[KEYA_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!keya_rel(abs, sizeof(abs), rel_dir) || !keya_is_dir(abs)) {
        return 0;
    }
    return keya_count_files_with_needle(abs, rel_dir, needle, ci, first_hit, cap, 0);
}

#define KEYA_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(keya_locate_root(), \
    "KEYA RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* printf-style helper: dynamic diagnostics pre-formatted into a static buffer then
 * passed to the single-string assert macros（HWV/KRM 先例）。 */
static char keya_fm_buf[1024];
static const char *keya_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *keya_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(keya_fm_buf, sizeof(keya_fm_buf), fmt, ap);
    va_end(ap);
    return keya_fm_buf;
}

#define KEYA_ASSERT_FILE_COUNT(n, min_n, what, red_hint)                       \
    TEST_ASSERT_TRUE_MESSAGE((n) >= (min_n),                                  \
        keya_fm("KEYA RED: %s referenced by %d file(s), expected >= %d — %s", \
               (what), (n), (min_n), (red_hint)))

/* -------------------------------------------------------------------------- *
 * TEST 1 — KR-A1 板级 KEY 接线机制齿（structural）
 * GPIO18 按键注册存在性（config.h KEY GPIO 定义 + 板级文件 key_button_ 成员构造）、
 * iot_button 三手势全注册、消抖参数复用 CONFIG_BUTTON_SHORT_PRESS_TIME_MS=180。
 * 全部符号/接线级（非行为级）；勾住 GPIO_NUM_18 / key_button_ 符号面，防 BOOT≈KEY
 * 行为等价假绿（实现者把 KEY 语义旁路挂在 boot_button_/GPIO0 上时本齿仍红）。
 * RED today: rlcd-4.2 config.h 无 GPIO_NUM_18（仅 BOOT=GPIO0）、板级仅 boot_button_
 * 无 key_button_ 无三手势、CONFIG_BUTTON_SHORT_PRESS_TIME_MS 仅注释 1 处。
 * ------------------------------------------------------------------------ */
/* coverage: KR-A1 -- 板级 KEY(GPIO18) 接线机制齿：config.h KEY_BUTTON_GPIO=GPIO_NUM_18 + key_button_ 三手势注册 + 消抖复用 180 */
TEST_CASE("键位路线A1: 板级KEY接线——GPIO18按键+三手势+消抖复用180 (KR-A1)",
          "[key_route_a1][KR-A1][structural]")
{
    int n;

    KEYA_REQUIRE_ROOT();

    /* tooth 1: config.h 定义 KEY_BUTTON_GPIO 且其值 = GPIO_NUM_18（KEY 键物理脚钉
     * 死为 18，不与 BOOT 的 GPIO0 混——防「KEY 语义旁路挂在 BOOT 脚」）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        keya_define_maps_to(KEYA_BOARD_CFG, "KEY_BUTTON_GPIO", "GPIO_NUM_18"),
        "KR-A1 RED: rlcd-4.2 config.h 未定义 KEY_BUTTON_GPIO -> GPIO_NUM_18——KEY "
        "(GPIO18) 板级悬空（现状仅 BOOT=GPIO0；spec §4.0/§5.3 两键分工 KEY 键未接线, "
        "ARQ-260911-cross-package-behavior-owner-gap 五包历史掉缝）");

    /* tooth 2 (guard): BOOT 键仍绑 GPIO0，两键分工不抢占脚位。 */
    TEST_ASSERT_TRUE_MESSAGE(
        keya_define_maps_to(KEYA_BOARD_CFG, "BOOT_BUTTON_GPIO", "GPIO_NUM_0"),
        "KR-A1 RED(infra): config.h 丢失 BOOT_BUTTON_GPIO -> GPIO_NUM_0 绑定——BOOT "
        "翻页键底座被破坏（spec §4.0 BOOT 主司翻页）");

    /* tooth 3: 板级文件用 KEY_BUTTON_GPIO 构造 key_button_ 成员（KEY 键真实挂 GPIO18，
     * 不是新瓶装 BOOT 脚）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        keya_file_contains(KEYA_BOARD_CC, "key_button_(KEY_BUTTON_GPIO"),
        "KR-A1 RED: rlcd-4.2 板级无 key_button_(KEY_BUTTON_GPIO) 构造——KEY 键未在板上 "
        "注册 GPIO18 按键（防把 KEY 语义旁路挂在 boot_button_/GPIO0 上，机制级红线）");

    /* tooth 4: iot_button 三手势（short_press/double_click/long_press）全注册在
     * key_button_ 上（不是 boot_button_）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        keya_file_contains(KEYA_BOARD_CC, "key_button_.OnClick(") &&
        keya_file_contains(KEYA_BOARD_CC, "key_button_.OnDoubleClick(") &&
        keya_file_contains(KEYA_BOARD_CC, "key_button_.OnLongPress("),
        "KR-A1 RED: key_button_ 未注册齐三手势（OnClick/OnDoubleClick/OnLongPress）—— "
        "KEY 键短按/双击/长按三相 iot_button 注册不完整（spec §4.0 全局按键手势表）");

    /* tooth 5: 消抖复用 CONFIG_BUTTON_SHORT_PRESS_TIME_MS=180——板级须显式引用该常量
     * 于 KEY 键短按窗（>= 2 处：遗留注释 1 + KEY 构造码 1），不得只靠 sdkconfig 隐式
     * 默认导致漂移。 */
    n = keya_count_in_file(KEYA_BOARD_CC, "CONFIG_BUTTON_SHORT_PRESS_TIME_MS");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        keya_fm("KR-A1 RED: CONFIG_BUTTON_SHORT_PRESS_TIME_MS 在板级仅 %d 处（现状只有"
                "遗留注释）——KEY 键构造码未显式复用 180ms 短按窗（与 BOOT 一致的消抖参数"
                "必须可复核，防漂移）", n));

    /* tooth 6 (guard): 消抖基准常量 180 在 sdkconfig 在世。 */
    TEST_ASSERT_TRUE_MESSAGE(
        keya_file_contains("firmware/sdkconfig", "CONFIG_BUTTON_SHORT_PRESS_TIME_MS=180"),
        "KR-A1 RED(infra): sdkconfig 丢失 CONFIG_BUTTON_SHORT_PRESS_TIME_MS=180 基准——"
        "单击 180ms 触发窗被改（库固有行为，BOOT/KEY 共用）");
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — KR-A2 parser 喂入接线机制齿（structural）
 * 板级按键事件经 key_gesture_parser_feed 喂入（冻结纯逻辑首次上设备）——设备侧
 * 调用存在性（现状零调用）+ KEY id 设备侧接线（KEY 手势喂 KEY_ID_KEY，不是只喂 BOOT）。
 * 断言绑 key_gesture_parser_feed 符号面，非行为级。
 * RED today: key_gesture_parser_feed(& 与 key_gesture_parser_init(& 设备侧零调用
 * （全仓仅 session_logic.acceptance.c 宿主命中的 test 目录内，已被排除）；KEY_ID_KEY
 * 仅 key_gesture_parser.h enum + .cc route 检查 2 文件，无设备侧消费点。
 * ------------------------------------------------------------------------ */
/* coverage: KR-A2 -- parser 喂入机制齿：key_gesture_parser_feed/init 设备侧调用存在 + KEY_ID_KEY 设备侧消费 */
TEST_CASE("键位路线A1: parser喂入接线——key_gesture_parser_feed/init设备侧存在+KEY id接线 (KR-A2)",
          "[key_route_a1][KR-A2][structural]")
{
    char hit[KEYA_MAX_PATH];
    int n;

    KEYA_REQUIRE_ROOT();

    /* tooth 1: 设备侧存在 key_gesture_parser_feed(& 调用点（喂入冻结解析器的真实生产
     * 消费面，非 .h 声明/.cc 定义——用 & 形调用 needle 排除声明/定义）。 */
    n = keya_scan_count("firmware/main", "key_gesture_parser_feed(&", 0, hit, sizeof(hit));
    KEYA_ASSERT_FILE_COUNT(n, 1, "key_gesture_parser_feed(&",
        "板级按键边沿未喂入冻结解析器——key_gesture_parser_feed 设备侧零调用（纯逻辑"
        "冻结全绿但悬空，ARQ-260911-cross-package-behavior-owner-gap 根因)");

    /* tooth 2: 设备侧初始化解析器实例。 */
    n = keya_scan_count("firmware/main", "key_gesture_parser_init(&", 0, hit, sizeof(hit));
    KEYA_ASSERT_FILE_COUNT(n, 1, "key_gesture_parser_init(&",
        "KEY 手势解析器实例未在设备侧初始化（feed 无处落状态, 接线不成立）");

    /* tooth 3: KEY id 被设备侧消费（KEY 手势喂 KEY_ID_KEY，不是只把 BOOT 喂进解析器）——
     * .h enum + .cc route 已是 2 文件，须出现第 3 个设备侧消费点（管线或路由接线文件）。 */
    n = keya_scan_count("firmware/main", "KEY_ID_KEY", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        keya_fm("KR-A2 RED: KEY_ID_KEY 仅 %d 文件引用（.h enum + .cc route, 无设备侧"
                "消费）——KEY 手势未以 KEY_ID_KEY 喂入/路由, 防「只接线 BOOT、KEY 语义"
                "旁路挂 boot」假绿", n));
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — KR-A3 全局路由表落地接线（structural）
 * key_route_global KEY/BOOT 两键位调用存在 + SESSION_TOGGLE / HOME / PAGE_NEXT 三个
 * 路由动作枚举在设备侧被 dispatch 消费（route 输出->应用动作映射接线）。纯逻辑分支
 * 已由冻结 session_logic 齿覆盖，本齿只钉设备侧 dispatch 接线，不得要求纯逻辑任何
 * 改动。
 * RED today: key_route_global(KEY_ID_*) 设备侧零调用（仅 .h 声明 + .cc 定义）；
 * SESSION_TOGGLE/HOME/PAGE_NEXT 各仅 .h enum + .cc return 2 文件，无 dispatch 消费点。
 * ------------------------------------------------------------------------ */
/* coverage: KR-A3 -- 全局路由表落地接线：key_route_global KEY/BOOT 调用 + SESSION_TOGGLE/HOME/PAGE_NEXT 设备侧 dispatch 消费 */
TEST_CASE("键位路线A1: 全局路由表落地——KEY/BOOT路由调用+三动作dispatch消费 (KR-A3)",
          "[key_route_a1][KR-A3][structural]")
{
    char hit[KEYA_MAX_PATH];
    int n;

    KEYA_REQUIRE_ROOT();

    /* tooth 1: KEY 手势经 key_route_global 路由（KEY 单击=对话 / KEY 长按=回首页的
     * 接线入口）。 */
    n = keya_scan_count("firmware/main", "key_route_global(KEY_ID_KEY", 0, hit, sizeof(hit));
    KEYA_ASSERT_FILE_COUNT(n, 1, "key_route_global(KEY_ID_KEY",
        "KEY 手势未喂进全局路由表——KEY 单击=对话/长按=回首页无接线入口");

    /* tooth 2: BOOT 手势经 key_route_global 路由（BOOT 单击=翻页的接线入口）。 */
    n = keya_scan_count("firmware/main", "key_route_global(KEY_ID_BOOT", 0, hit, sizeof(hit));
    KEYA_ASSERT_FILE_COUNT(n, 1, "key_route_global(KEY_ID_BOOT",
        "BOOT 手势未喂进全局路由表——BOOT 单击=翻页无接线入口（BOOT 仍走旧直连路径）");

    /* tooth 3: GLOBAL_ACT_SESSION_TOGGLE 被设备侧 dispatch 消费（KEY 单击->发起对话的
     * 映射接线；.h enum + .cc return 已 2 文件，须第 3 个设备侧 dispatch 点）。 */
    n = keya_scan_count("firmware/main", "GLOBAL_ACT_SESSION_TOGGLE", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        keya_fm("KR-A3 RED: GLOBAL_ACT_SESSION_TOGGLE 仅 %d 文件（.h enum + .cc return;"
                "无设备侧 dispatch 消费）——KEY 单击->ToggleChatState 对话动作接线缺失", n));

    /* tooth 4: GLOBAL_ACT_HOME 被设备侧 dispatch 消费（KEY 长按->回首页）。 */
    n = keya_scan_count("firmware/main", "GLOBAL_ACT_HOME", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        keya_fm("KR-A3 RED: GLOBAL_ACT_HOME 仅 %d 文件（无设备侧 dispatch 消费）——"
                "KEY 长按->回首页动作接线缺失", n));

    /* tooth 5: GLOBAL_ACT_PAGE_NEXT 被设备侧 dispatch 消费（BOOT 单击->翻页）。 */
    n = keya_scan_count("firmware/main", "GLOBAL_ACT_PAGE_NEXT", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        keya_fm("KR-A3 RED: GLOBAL_ACT_PAGE_NEXT 仅 %d 文件（无设备侧 dispatch 消费）——"
                "BOOT 单击->kidpal_render_node_cycle_next 翻页动作接线缺失", n));
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — KR-A4 BOOT 回归翻页去对话化（structural）
 * BOOT 单击不再走 ToggleChatState / 故事键单击路径（去对话化），主司翻页；
 * BOOT 长按故事翻屏（P3 阅读页内特例，KR-E1 零触碰）守卫保留。
 * RED today: 板级仍有 HandleStoryKey(KEY_ID_BOOT, KEY_GESTURE_CLICK)（BOOT 单击
 * 对话/故事路径未去）。
 * ------------------------------------------------------------------------ */
/* coverage: KR-A4 -- BOOT 回归翻页去对话化：HandleStoryKey(KEY_ID_BOOT, CLICK) 对话路径移除 + 长按故事翻屏守卫 */
TEST_CASE("键位路线A1: BOOT回归翻页去对话化+故事翻屏零触碰守卫 (KR-A4)",
          "[key_route_a1][KR-A4][KR-E1][structural]")
{
    KEYA_REQUIRE_ROOT();

    /* tooth 1 (negative): BOOT 单击不再走故事键单击/会话路径——该精确字面（现状 L194
     * HandleStoryKey(KEY_ID_BOOT, KEY_GESTURE_CLICK) 包着的 if->ToggleChatState）必须
     * 从板级移除；BOOT 单击改经 key_route_global(KEY_ID_BOOT, CLICK) 产出 PAGE_NEXT
     * 翻页（test 3 tooth 2/5 正向承接，冻结路由表已证其非 SESSION_TOGGLE）。 */
    TEST_ASSERT_FALSE_MESSAGE(
        keya_file_contains(KEYA_BOARD_CC, "HandleStoryKey(KEY_ID_BOOT, KEY_GESTURE_CLICK)"),
        "KR-A4 RED: BOOT 单击仍挂在 HandleStoryKey(KEY_ID_BOOT, KEY_GESTURE_CLICK) "
        "对话/故事路径上——去对话化未执行（BOOT 应主司翻页, 单击=翻页, spec §4.0）");

    /* tooth 2 (guard, KR-E1): BOOT 长按故事翻屏（P3 阅读=翻下一屏, 页内特例）零触碰——
     * 本包只动全局层, 页内特例留根 A2, 该接缝必须仍在。 */
    TEST_ASSERT_TRUE_MESSAGE(
        keya_file_contains(KEYA_BOARD_CC, "HandleStoryKey(KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS)"),
        "KR-E1 RED: BOOT 长按故事翻屏接缝（HandleStoryKey KEY_ID_BOOT KEY_GESTURE_"
        "LONG_PRESS, P3 阅读页内特例）被触碰——本包零触碰页内特例红线, spec S-F2.3 读者"
        "翻屏须保留");
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — boundary：本文件注册序守卫 + spec/契约 canary + 扫描器自检
 *   KR-A6 结构半（本验收文件是否被 builder 接线进 HOST_ACCEPTANCE_SRCS）
 *   KR-E2 spec 零修订 canary（docs/spec.md 与 docs/contracts/ 契约面仍在）
 *   扫描器自检（结构齿负向必须能红, 防假绿）
 * ------------------------------------------------------------------------ */
/* coverage: KR-A6 -- host runner 终跑：本文件注册进 HOST_ACCEPTANCE_SRCS 结构守卫 + spec/契约 canary + 扫描器自检 */
TEST_CASE("键位路线A1: 注册序守卫+spec契约canary+扫描器自检 (KR-A6)",
          "[key_route_a1][KR-A6][KR-E2][boundary]")
{
    char *mk;
    char hit[KEYA_MAX_PATH];
    int n;

    KEYA_REQUIRE_ROOT();

    /* tooth 1: 本文件须接线进 HOST_ACCEPTANCE_SRCS 且排在 host_infra_smoke 之前
     * （smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫）。RED until builder wires. */
    mk = keya_read(KEYA_HOST_MK, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk, "KEYA RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *mine = strstr(mk, "key_route_a1.acceptance.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(mine != NULL,
            "KR-A6 RED: key_route_a1.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS — host "
            "验收构建未接本包（BUILDER DUTY: 与本文件头注 [5] 同款接线）");
        TEST_ASSERT_TRUE_MESSAGE(smoke != NULL,
            "KEYA RED(infra): host_infra_smoke.acceptance.c 从 HOST_ACCEPTANCE_SRCS 丢失 — "
            "包 0 守卫被移除");
        TEST_ASSERT_TRUE_MESSAGE(mine < smoke,
            "KR-A6 RED: key_route_a1.acceptance.c 必须排在 host_infra_smoke.acceptance.c "
            "之前 (smoke 末位 = 最先执行, boundary 齿注册序守卫)");
    }
    free(mk);

    /* tooth 2 (KR-E2 canary): spec / 契约面仍在（spec 零修订红线）。 */
    TEST_ASSERT_TRUE_MESSAGE(keya_exists("docs/spec.md"),
        "KR-E2 RED(infra): docs/spec.md 丢失——spec 面被触碰（本包 spec 零修订红线）");
    TEST_ASSERT_TRUE_MESSAGE(keya_exists("docs/contracts/interface-contract.md"),
        "KEYA RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");

    /* tooth 3: 扫描器自检（防假绿: 负向齿必须能红）。 */
    TEST_ASSERT_TRUE_MESSAGE(keya_exists("firmware/main/key_gesture_parser.cc"),
        "KEYA RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(keya_exists("firmware/main/__keya_no_such__.cc"),
        "KEYA RED(infra): scanner reports a nonexistent file as present");
    n = keya_scan_count("firmware/main", "__keya_no_such_needle__", 0, hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "KEYA RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    n = keya_count_in_file("firmware/main/key_gesture_parser.cc", "key_route_global");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "KEYA RED(infra): occurrence counter cannot count a known literal — 计数器失灵");
}

/* ---------------------------------------------------------------------------
 * 真机证据要求（KR-A5 / KR-E1 / KR-E3，本文件不承载, human 持板轮）:
 *   KR-A5: KEY(GPIO18) 三手势真机可达 + 电平/上拉确认（悬空风险销账），HWV-A2 顺带
 *   核验；GPIO18 的 active_high（上拉/下拉）真机实测结果回填并冻结进板级 key_button_
 *   构造的 active_high 实参。证据落 changes/dev/260911-key-route-a1/（日志/照片）。
 *   KR-E1: 页内特例 P2 问课程 / P3 光标+双击进阅读 / P3 念给我听 / P4 播笔顺 / 提醒
 *   弹窗 KEY 关闭 = 零触碰留根 A2（本文件 test 4 tooth 2 只守 BOOT 长按故事翻屏一项
 *   板级接缝；其余页内特例的接线状态由既有 HWV-A9 / story_p3 冻结齿承载, 本包不新钉）。
 *   KR-E3: 真机证据齐 + 逃生门零触发记录（Track B spec-lane / 契约门均未触发）。
 * --------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * coverage 登记（对齐 hardware_validation 先例的登记式 coverage 行; 非闸必需——
 * KR-A5 kind=structural 豁免, KR-E1/E2/E3 为 exitCriteria 不入 acceptance 闸）:
 *   coverage: KR-A5  physical 半 -> 真机持板轮（HWV-A2 顺带）, 不落 C 断言;
 *             证据落 changes/dev/260911-key-route-a1/（human 持板日志/照片）
 *   coverage: KR-E1  范围纪律 -> test 4 tooth 2（BOOT 长按故事翻屏 guard）部分承载;
 *             其余页内特例零触碰留根 A2（既有 HWV-A9 / story_p3 冻结齿承载）
 *   coverage: KR-E2  spec 零修订 -> test 5（docs/spec.md 与契约 canary）
 *   coverage: KR-E3  真机证据齐 + 逃生门零触发 -> human 汇总, 不落 C 断言
 * --------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * RED baseline（2026-09-11 双通道, acceptance-author 实测）:
 *   通道① 既有 host runner 全量实跑（本文件尚未接线进 ELF, 与基线同构）:
 *     104 Tests / 4 Failures / 0 Ignored, exit 4——四条红 = WSA-A6:830 / WSA-A4:663 /
 *     P2-A1:571 / VRT-A2·E2:878, 均在册 existing-red-inventory（VRT-A2·E2:878 owner=
 *     260829-voice-roundtrip, local-only 证据族; launch 口径 104/3 漏列该条, 主
 *     worktree progress.md 注记为未提交项——本包有效基线 = 104/4）, 非本包责任、
 *     本包不修。本判定口径 = 既有齿相对基线零新增红 + 本包新齿对现状红成立。
 *   通道② 本包新齿 C 类冻结验收实编（DEV-GOVERNANCE 验收测试纪律 §4）:
 *     gcc -std=c11 -Wall -Wextra -fsyntax-only -I firmware/main/test/acceptance/red-baseline
 *         firmware/main/test/key_route_a1.acceptance.c
 *     编译通道 error 为零（验收文件本身可编译）+ 解析性红定性: GPIO_NUM_18 在 rlcd-4.2
 *     config.h 零命中 / key_gesture_parser_feed(& 设备侧零调用 / key_route_global(KEY_ID_*
 *     零调用 / SESSION_TOGGLE·HOME·PAGE_NEXT 无 device 侧消费点——各齿对现状必红,
 *     红基线成立（接缝缺失即测试意图缺的真实行为）。
 *   cwd 口径: 结构齿以真实工作树扫描, root 自手持 cwd 定位（仓根 / firmware/main/test/
 *   host_app 均可, 已实测）。审查口详见交付报告 redBaselineCommand/redBaselineResult。
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-11  initial freeze (acceptance-author, TaskPackage 260911-key-route-a1).
 *   2026-09-11  marker-format fix (planner 重派, verifier scoped round 1): 补
 *               coverage: 标记行供 acceptance-coverage-check 闸 hasCoverageMarker
 *               regex ① 认 `coverage: <id>` 形态（原测试名 `(KR-Ax)` 后缀 + tag
 *               `[KR-Ax]` 不被闸 regex 识别）。仅增注释行, 断言/needle/测试名/
 *               接缝契约实质零变更。
 * --------------------------------------------------------------------------- */