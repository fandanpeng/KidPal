/*
 * Frozen acceptance tests — TaskPackage 260915-m1-k1a-key-layer-dispatch (subsystem:
 * firmware; changeId 编号 M1-K1a; 键位对账 ①⑤ 落地面 · 覆盖层 BOOT 隔离 + KEY 长按
 * 返回上一层 + cycle_next 清层/进页分离)。
 *
 * Story: 共享底座键位对账项——语义权威 = docs/requirements.md §2.3 按键手势（权威键位
 * 定义，v1.47 矩阵 + ⚠️ 键位对账块 ①⑤）+ §2.2 状态表（提醒行/查字窗行 v1.39 隔离口径）。
 * 与 260911-key-route-a1 同 seam 演进：其 KEY 接线 / 全局路由表（key_route_global）已
 * 合并，本包在其上做 layer-aware 派发改造。分层模型（layer_top）owner = 本包；
 * keybar / 语音轮 / 提醒各包只读消费（crossPackageBehaviors 已登记）。
 *
 * Nature: HOST-SIDE STRUCTURAL (tree-scan) + 路由级 BEHAVIORAL frozen acceptance
 * （key_route_a1.acceptance.c / schedule_reminder_b.acceptance.c 双先例合体）。KD-A1
 * 明确要求**机制级断言（layer 分流符号/路由级）而非纯行为断言**——防「BOOT≈翻页」
 * 等价假绿（plan.md 取舍 A：每覆盖层自抢按键事件被否，裁决权必须收敛在 dispatch 层
 * 按 layer_top 分流）。因此：
 *   - 路由级半：新增纯逻辑 key_route_layer(key, g, overlay_top) 决策表 + 显式清层步骤
 *     kidpal_layer_pop_transients(st)，两者都是纯 C、host runner 行为级可验（weak 哨兵
 *     模式，强定义落地后覆盖——key_gesture_parser.cc / kidpal_layer.c 均已编入 runner）；
 *   - 结构半：树级扫描钉 dispatch 分流接线事实（板级手势回调不再直连 key_route_global、
 *     layer 归属查询/显式关层步骤被 dispatch 消费、覆盖层裁决函数 kidpal_reminder_key
 *     首次设备侧接线、OVERLAY_CLOSE 动作三文件消费链、cycle_next 函数体清层副作用
 *     拆除）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements，本文件只读扫工作树 + 弱哨兵
 * 行为级验证; 「home =」为冻结落点，与 260911-key-route-a1 [1]-[5] 同款钉法）:
 *
 *   [1] 路由层（home = firmware/main/key_gesture_parser.{h,cc}，纯逻辑单源）:
 *       global_action_t 枚举**追加**（append-only，既有四值 NONE=0/SESSION_TOGGLE=1/
 *       HOME=2/PAGE_NEXT=3 零偏移）:
 *         GLOBAL_ACT_OVERLAY_CLOSE = 4   -- 覆盖层关层回原页（v1.39 隔离 + v1.36
 *                                            返回上一层 的统一路由动作）
 *       global_action_t key_route_layer(key_id_t key, key_gesture_t g, int overlay_top);
 *         -- layer-aware 全局路由表。overlay_top 由 dispatch 层归一化（0 = 主页面基层，
 *            非 0 = 覆盖层顶层），路由表自身保持 context-free 纯逻辑:
 *         overlay_top != 0（覆盖层顶层）:
 *           KEY  CLICK        -> GLOBAL_ACT_SESSION_TOGGLE（会话主轴零回归；覆盖层
 *                                特例——如提醒弹窗 KEY 关闭——由 dispatch 先交顶层
 *                                裁决函数，裁决消费即不走路由）
 *           KEY  LONG_PRESS   -> GLOBAL_ACT_OVERLAY_CLOSE（返回上一层 = 关层回原页）
 *           BOOT CLICK        -> GLOBAL_ACT_OVERLAY_CLOSE（v1.39 隔离 = 关顶层回原页、
 *                                **不产翻页动作**）
 *           其余（双击族 / BOOT 长按）-> GLOBAL_ACT_NONE
 *         overlay_top == 0（主页面基层）: 与 key_route_global(key, g) **全表等价**
 *           （KEY CLICK->SESSION_TOGGLE / KEY LONG_PRESS->HOME / BOOT CLICK->PAGE_NEXT /
 *            其余 NONE——主页面 BOOT 单击翻页路径零回归，§2.3 主页面矩阵行）
 *         越界键位（>= KEY_ID_COUNT，含 PWR）-> GLOBAL_ACT_NONE（两列一致，零污染）
 *       key_route_global 本体**零改动**（其表为基层列的实现载体；session_logic 齿全绿
 *       must-stay）。
 *
 *   [2] 显式清层步骤（清层/进页分离，KD-A2; home = firmware/main/display/kidpal_layer.{h,c}
 *       + kidpal_render_node.cc）:
 *       int kidpal_layer_pop_transients(kidpal_layer_stack_t *st);
 *         -- 纯逻辑显式清层步骤：出掉全部 transient 覆盖层（KIDPAL_LAYER_VOICE /
 *            KIDPAL_LAYER_SERVER_PAGE / KIDPAL_LAYER_REMINDER，各至多一条、幂等）；
 *            KIDPAL_LAYER_LOCAL_PAGE / KIDPAL_LAYER_PROVISION 不碰（配网层零触碰）；
 *            返回出掉的层数（>=0）；NULL st -> -1。空栈/纯基层栈 -> 0。
 *            host runner 行为级可验（weak 哨兵，强定义落 kidpal_layer.c 后覆盖）。
 *       void kidpal_render_node_dismiss_front_overlay(void);
 *         -- 设备侧显式关层步骤（「关窗」唯一入口）：前台为覆盖层时出掉 transient 层
 *            回基层并重渲基层页；内部消费 kidpal_layer_pop_transients（单一清层事实源）。
 *       kidpal_render_node_cycle_next() **函数体内不再含任何 kidpal_layer_pop 调用**
 *         ——「关窗不再依赖翻页函数开头清 transient 层的副作用」（requirements §2.3
 *         对账 ①），进页归进页；主页面翻页步进逻辑零回归（s_local_page 步进仍在）。
 *
 *   [3] dispatch 分流（KD-A1/A3; home = 板级
 *       firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc）:
 *       int kidpal_render_node_front_is_overlay(void);
 *         -- layer 栈 owner 侧前台归一化查询（home = firmware/main/display/
 *            kidpal_render_node.{h,cc}）：前台 owner ∈ {VOICE, SERVER_PAGE, REMINDER}
 *            -> 1（覆盖层顶层）；LOCAL_PAGE / PROVISION / CHAT / NONE -> 0
 *            （配网态按键语义零触碰）。板级 dispatch 消费它做 layer_top 分流。
 *       板级 KEY/BOOT 手势回调统一改经 layer-aware 分流进入，**不再直连**
 *         keya_dispatch_global_action(key_route_global(KEY_ID_*, ...))（旧硬路由接线
 *         模式拆除——BOOT 单击恒产 PAGE_NEXT 的对账 ① 根因即在这条直连上）:
 *         覆盖层顶层（front_is_overlay()）先交顶层覆盖层裁决函数，DISMISS 即消费；
 *         未消费落 key_route_layer(key, g, front_is_overlay()) -> keya_dispatch_global_action。
 *       顶层覆盖层裁决函数 = kidpal_reminder_key(kidpal_reminder_default(), ...)（提醒
 *         弹窗，**设备侧首次接线**——requirements §2.3 对账 ①「正经裁决函数设备侧零
 *         调用」的修复落点；其决策表已由 schedule_reminder_b.acceptance.c 按 v1.39
 *         隔离口径修订冻结）。
 *       GLOBAL_ACT_OVERLAY_CLOSE 在板级 dispatch 被消费：映射
 *         kidpal_render_node_dismiss_front_overlay()（关层回原页），**永不**映射
 *         kidpal_render_node_cycle_next / 翻页动作。
 *       GLOBAL_ACT_HOME -> kidpal_render_node_show_home() 既有映射零回归（主页面
 *         KEY 长按 = 回首页，§2.3 主页面矩阵行）。
 *
 *   [4] Builder FIRST ACTION: 把本验收文件注册进
 *       firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS
 *       （相对 host_app/main 为 "../../key_layer_dispatch.acceptance.c"），置于
 *       "../../host_infra_smoke.acceptance.c" 之前（smoke 保持列表末位 = 最先执行，
 *       boundary 齿注册序守卫；本文件 test 5 tooth 1 校验此注册）。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: KD-A1 覆盖层 BOOT 隔离机制齿（layer 分流符号/路由级）-> test 1（路由级
 *             决策表行为半: 覆盖层列 BOOT CLICK -> OVERLAY_CLOSE 非 PAGE_NEXT）+
 *             test 2（结构半: 分流接线/裁决函数设备侧接线/OVERLAY_CLOSE 消费链）
 *   coverage: KD-A2 清层与进页分离 -> test 3（行为半: kidpal_layer_pop_transients
 *             显式清层决策表 + 结构半: cycle_next 函数体零 kidpal_layer_pop +
 *             dismiss_front_overlay 显式步骤 + 主页面翻页零回归守卫）
 *   coverage: KD-A3 KEY 长按返回上一层 -> test 1（覆盖层列 KEY LONG_PRESS ->
 *             OVERLAY_CLOSE; 基层列 HOME）+ test 2（板级 KEY 手势经 layer-aware 分流）
 *             + test 4（GLOBAL_ACT_HOME -> show_home 零回归守卫 + 主页面 BOOT 翻页
 *             冻结语义守卫）
 *   coverage: KD-A4 钉点成批改 -> schedule_reminder_b.acceptance.c 文件尾 REVISION
 *             LOG 2026-09-15 条目（acceptance-author 留痕修订，非本文件断言面）
 *   coverage: KD-A5 真机确认轮 -> human 持板轮，不落 C 断言（kind=structural 对象
 *             条目）; 证据落 delivery/changes/dev/260915-m1-k1a-key-layer-dispatch/
 *             （以仓根为 cwd 的相对路径; host runner ELF 的运行 cwd 见 [C4.4]，结构齿
 *             以 kld_locate_root 自手持 cwd 定位仓根）
 *   coverage: KD-A6 host runner 终跑判读 + 本文件注册结构守卫 + KD-E2 spec 零修订
 *             canary + KD-E1 零触碰守卫 + 扫描器自检 -> test 5（suite 级判读说明见
 *             本文件尾「KD-A6 终跑判读」段）
 *   coverage: KD-E1 范围纪律 -> test 4 tooth 2（2026-09-18 留痕修订：原「PAGE_P4 页枚举
 *             零触碰守卫——页循环 4->3 / 撤 P4 归 M2」随 260917-m2-k2-retract-p4-three-
 *             page 落地翻正为「PAGE_P4 已撤 + PAGE_COUNT == 3」同源判别）+ test 4
 *             tooth 3（BOOT 长按故事翻屏接缝零触碰——
 *             P3 键位族 v1.47 归 M4 的在世证据之一）
 *   coverage: KD-E2 需求面零修订 -> test 5 tooth 3（requirements/契约 canary）
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   0) builder wiring: add "../../key_layer_dispatch.acceptance.c" to
 *      HOST_ACCEPTANCE_SRCS in firmware/main/test/host_app/main/CMakeLists.txt,
 *      BEFORE "../../host_infra_smoke.acceptance.c".
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal/firmware/main/
 *      test/host_app/espidf_wsl.sh build   （git-bash 下 MSYS_NO_PATHCONV=1 必加，
 *      否则 /mnt/c 路径被改写; idf.py 无 -T 选项，host_app 是独立工程直接 build）
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- /mnt/c/AI/KidPal/firmware/main/test/
 *      host_app/build/host_infra_test.elf
 *   Freeze-phase red baseline: 见文件尾「KD-A6 终跑判读」+「RED baseline 判读口径」段。
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

/* 包 3 / 包 2（归一屏）共享底座头（相对路径, host runner 与独立 stub 编译都可达）。 */
#include "../key_gesture_parser.h"
#include "../display/kidpal_layer.h"

#define KLD_MAX_PATH 1024
#define KLD_MAX_READ (8u * 1024u * 1024u)

/* 板级装配文件（dispatch 分流 home, seam [3]）。 */
#define KLD_BOARD_CC "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — weak sentinels（强定义落地后覆盖）。
 * key_route_layer 强定义 home = key_gesture_parser.cc（已编入 host runner）；
 * kidpal_layer_pop_transients 强定义 home = display/kidpal_layer.c（已编入）。
 * 落地前哨兵返回 KLD_UNIMPLEMENTED：行为齿先断「非哨兵」，红基线读作
 * 「frozen contract not implemented」——真实缺失行为红，绝非链接错误。
 * ------------------------------------------------------------------------ */
#define KLD_UNIMPLEMENTED (-2)

/* GLOBAL_ACT_OVERLAY_CLOSE 冻结数值（枚举 append-only 追加位，见 seam [1]）。
 * 以字面量钉值而非直接引用枚举名——冻结期枚举尚未落地，本文件必须先可编译（C 类
 * 冻结验收实编纪律）；落地后 assert 的是 key_route_layer 的实际返回值 == 4。 */
#define KLD_OVERLAY_CLOSE_VALUE 4

__attribute__((weak)) global_action_t key_route_layer(key_id_t key, key_gesture_t g,
                                                      int overlay_top)
{
    (void)key; (void)g; (void)overlay_top;
    return (global_action_t)KLD_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_layer_pop_transients(kidpal_layer_stack_t *st)
{
    (void)st;
    return KLD_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（host 侧结构齿，key_route_a1 / schedule_reminder_b 先例）。
 * 全部扫描跑在真实工作树上，root 以手持 cwd 定位（从仓根或 firmware/main/test/host_app
 * 两处都可跑）。test / host_app / build / .git / assets 目录排除——test 目录里住着本
 * 冻结文件自身（其注释合法提及冻结 needle）。
 * ------------------------------------------------------------------------ */

static char kld_root[KLD_MAX_PATH];
static int kld_root_ready;

static int kld_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int kld_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int kld_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int kld_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KLD_MAX_PATH];

    if (kld_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (kld_is_regular(probe)) {
            snprintf(kld_root, sizeof(kld_root), "%s", cands[i]);
            kld_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int kld_rel(char *out, size_t cap, const char *rel)
{
    if (!kld_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", kld_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", kld_root, rel);
    return 1;
}

static int kld_exists(const char *rel)
{
    char p[KLD_MAX_PATH];
    return kld_rel(p, sizeof(p), rel) && kld_is_regular(p);
}

static char *kld_read(const char *rel, size_t *out_len)
{
    char p[KLD_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!kld_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KLD_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int kld_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int kld_file_contains(const char *rel, const char *needle)
{
    char *buf = kld_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = kld_contains(buf, needle);
    free(buf);
    return r;
}

/* Occurrence count of needle inside one file (0 when unreadable/absent). */
static int kld_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = kld_read(rel, NULL);
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

static int kld_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
                                       const char *needle, char *first_hit, size_t cap,
                                       int depth)
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
        char abs_next[KLD_MAX_PATH];
        char rel_next[KLD_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            kld_is_skipped_dir(e->d_name)) {
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
            count += kld_count_files_with_needle(abs_next, rel_next, needle, first_hit,
                                                 cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > KLD_MAX_READ) {
                continue;
            }
            {
                char *buf = kld_read(rel_next, NULL);
                if (buf != NULL) {
                    if (kld_contains(buf, needle)) {
                        if (count == 0 && first_hit != NULL) {
                            snprintf(first_hit, cap, "%s", rel_next);
                        }
                        count++;
                    }
                    free(buf);
                }
            }
        }
    }
    closedir(d);
    return count;
}

static int kld_scan_count(const char *rel_dir, const char *needle, char *first_hit,
                          size_t cap)
{
    char abs[KLD_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!kld_rel(abs, sizeof(abs), rel_dir) || !kld_is_dir(abs)) {
        return 0;
    }
    return kld_count_files_with_needle(abs, rel_dir, needle, first_hit, cap, 0);
}

/* Slice of rel from the FIRST occurrence of start_needle up to (exclusive) the first
 * occurrence of end_needle AFTER it.  Returns a malloc'd NUL-terminated slice or NULL
 * (unreadable file / either needle absent).  Caller frees.  Used for function-body
 * region pins（cycle_next 清层副作用拆除）——两个 needle 都取稳定签名行。 */
static char *kld_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = kld_read(rel, NULL);
    char *s, *e;
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
    e = strstr(s + strlen(start_needle), end_needle);
    if (e == NULL) {
        e = buf + strlen(buf); /* start 是文件内最后一个锚——切到 EOF */
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

static int kld_count_in_slice(const char *slice, const char *needle)
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

#define KLD_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(kld_locate_root(), \
    "KLD RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* Shared red-string for the frozen seam not being implemented yet. */
static void kld_require_implemented(int value, const char *what)
{
    if (value == KLD_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "KLD RED: frozen seam NOT implemented — %s "
                 "(builder must land the layer-aware dispatch per the seam contract in "
                 "firmware/main/test/key_layer_dispatch.acceptance.c header and wire this "
                 "file into HOST_ACCEPTANCE_SRCS)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* printf-style helper: dynamic diagnostics pre-formatted into a static buffer then
 * passed to the single-string assert macros（KEYA/KRM 先例）。 */
static char kld_fm_buf[1024];
static const char *kld_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *kld_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(kld_fm_buf, sizeof(kld_fm_buf), fmt, ap);
    va_end(ap);
    return kld_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — KD-A1/A3 路由级决策表（behavioral, frozen seam [1]）
 * key_route_layer 的覆盖层列 / 基层列逐格断言：覆盖层顶层 BOOT 单击 = OVERLAY_CLOSE
 * （路由级机制断言——非 PAGE_NEXT，「BOOT≈翻页」等价假绿在路由表这一层即被咬）；
 * 覆盖层 KEY 长按 = OVERLAY_CLOSE（返回上一层）；基层列与 key_route_global 全表等价
 * （主页面零回归）。RED today: weak sentinel -2。
 * ------------------------------------------------------------------------ */
/* coverage: KD-A1 -- 覆盖层 BOOT 隔离路由级机制齿：key_route_layer 覆盖层列 BOOT CLICK -> GLOBAL_ACT_OVERLAY_CLOSE(=4) 非 PAGE_NEXT */
/* coverage: KD-A3 -- KEY 长按返回上一层路由级机制齿：覆盖层列 KEY LONG_PRESS -> OVERLAY_CLOSE、基层列 -> HOME */
TEST_CASE("键位分层派发: key_route_layer 决策表——覆盖层隔离列+基层等价列 (KD-A1/KD-A3)",
          "[key_layer_dispatch][KD-A1][KD-A3][route]")
{
    int overlay;

    /* ---- 覆盖层列（overlay_top != 0）-------------------------------------- */
    overlay = 1;

    /* tooth 1: BOOT 单击 -> OVERLAY_CLOSE（v1.39 隔离: 关顶层回原页、不产翻页动作）。
     * 同时断言 != PAGE_NEXT 的机制判别式（等价假绿防线: 若覆盖层列塌回基层翻页动作,
     * 本齿以「值不相等」独立咬住, 不依赖 ==OVERLAY_CLOSE 单点）。 */
    {
        int act = (int)key_route_layer(KEY_ID_BOOT, KEY_GESTURE_CLICK, overlay);
        kld_require_implemented(act, "key_route_layer unimplemented");
        TEST_ASSERT_TRUE_MESSAGE(act == KLD_OVERLAY_CLOSE_VALUE,
            kld_fm("KLD-A1 RED: 覆盖层顶层 BOOT 单击路由动作 = %d, 期望 "
                   "GLOBAL_ACT_OVERLAY_CLOSE(=%d)——v1.39 隔离口径未进路由表 "
                   "(requirements §2.3 对账 ①)", act, KLD_OVERLAY_CLOSE_VALUE));
        TEST_ASSERT_TRUE_MESSAGE(act != (int)GLOBAL_ACT_PAGE_NEXT,
            "KLD-A1 RED: 覆盖层顶层 BOOT 单击仍路由到 PAGE_NEXT——「BOOT≈翻页」等价假绿 "
            "(覆盖层 BOOT 必须关层回原页、不产翻页动作)");
    }

    /* tooth 2: KEY 长按 -> OVERLAY_CLOSE（v1.36 返回上一层在覆盖层顶层的实例）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS, overlay)
            == KLD_OVERLAY_CLOSE_VALUE,
        "KLD-A3 RED: 覆盖层顶层 KEY 长按未路由到 GLOBAL_ACT_OVERLAY_CLOSE——"
        "「返回上一层 = 关层回原页」未接 (requirements §2.3 对账 ⑤)");

    /* tooth 3: KEY 单击保持 SESSION_TOGGLE（会话主轴零回归; 提醒弹窗 KEY 关闭由
     * dispatch 先交顶层裁决函数承载, 路由表不吞该特例）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_CLICK, overlay)
            == (int)GLOBAL_ACT_SESSION_TOGGLE,
        "KLD-A1 RED: 覆盖层顶层 KEY 单击不再路由 SESSION_TOGGLE——会话主轴被 "
        "layer-aware 改造误伤（KEY=正向操作总纲, §2.3）");

    /* tooth 4: 覆盖层列其余手势 NONE（双击族 / BOOT 长按不吞不翻）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK, overlay)
            == (int)GLOBAL_ACT_NONE &&
        (int)key_route_layer(KEY_ID_BOOT, KEY_GESTURE_DOUBLE_CLICK, overlay)
            == (int)GLOBAL_ACT_NONE &&
        (int)key_route_layer(KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS, overlay)
            == (int)GLOBAL_ACT_NONE,
        "KLD-A1 RED: 覆盖层列双击族/BOOT 长按出现非 NONE 动作——本包语义面外手势 "
        "被路由表吞掉（BOOT 长按故事翻屏接缝零触碰, KD-E1）");

    /* ---- 基层列（overlay_top == 0）: 与 key_route_global 全表等价 ---------- */
    {
        int k, g;
        static const key_id_t keys[] = {
            KEY_ID_KEY, KEY_ID_BOOT,
            (key_id_t)99 /* 越界键位（PWR 模拟, >= KEY_ID_COUNT） */
        };
        for (k = 0; k < (int)(sizeof(keys) / sizeof(keys[0])); k++) {
            for (g = (int)KEY_GESTURE_NONE; g <= (int)KEY_GESTURE_LONG_PRESS; g++) {
                int la = (int)key_route_layer(keys[k], (key_gesture_t)g, 0);
                int ga = (int)key_route_global(keys[k], (key_gesture_t)g);
                TEST_ASSERT_TRUE_MESSAGE(la == ga,
                    kld_fm("KLD-A1 RED: 基层列(key=%d,g=%d) key_route_layer=%d != "
                           "key_route_global=%d——主页面路径零回归被破坏 "
                           "(§2.3 主页面矩阵行: BOOT 单击翻页 / KEY 长按回首页)",
                           (int)keys[k], g, la, ga));
            }
        }
    }

    /* tooth 6 (guard): 基层列语义锚（key_route_global 冻结语义零回归, session_logic
     * must-stay 的本包侧复查）: BOOT 单击 = PAGE_NEXT、KEY 长按 = HOME。 */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_global(KEY_ID_BOOT, KEY_GESTURE_CLICK) == (int)GLOBAL_ACT_PAGE_NEXT &&
        (int)key_route_global(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS) == (int)GLOBAL_ACT_HOME,
        "KLD-A1 RED(infra): key_route_global 基层语义锚漂移——BOOT 单击翻页 / KEY 长按"
        "回首页的全局表被本包改动（key_route_global 本体零改动红线）");
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — KD-A1/A3 dispatch 分流接线（structural, frozen seam [2]/[3]）
 * 板级手势回调不再直连 key_route_global（对账 ① 根因拆除）；layer 归属查询 /
 * 显式关层步骤被板级 dispatch 消费；GLOBAL_ACT_OVERLAY_CLOSE 三文件消费链
 * （枚举 .h + 路由 .cc + 板级 dispatch）；覆盖层裁决函数 kidpal_reminder_key 首次
 * 设备侧接线（对账 ①「正经裁决函数设备侧零调用」修复）。
 * RED today: 各 needle 全部零命中 / 旧直连接线仍在（负向齿）。
 * ------------------------------------------------------------------------ */
/* coverage: KD-A1 -- 覆盖层 BOOT 隔离 dispatch 分流结构齿：板级去直连 + front_is_overlay/dismiss_front_overlay 消费 + OVERLAY_CLOSE 三文件消费链 + kidpal_reminder_key 设备侧首次接线 */
TEST_CASE("键位分层派发: dispatch 分流接线——去直连+layer查询+关层步骤+裁决函数接线 (KD-A1/KD-A3)",
          "[key_layer_dispatch][KD-A1][KD-A3][structural]")
{
    char hit[KLD_MAX_PATH];
    int n;

    KLD_REQUIRE_ROOT();

    /* tooth 1: key_route_layer 落在纯逻辑单源（key_gesture_parser.h 声明 + .cc 定义）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kld_file_contains("firmware/main/key_gesture_parser.h", "key_route_layer("),
        "KLD-A1 RED: key_gesture_parser.h 未声明 key_route_layer——layer-aware 路由"
        "表未落纯逻辑单源（seam [1] home 纪律; 防设备侧另立一套手势语义, "
        "ARQ-260911 防 BOOT≈KEY 假绿同款红线）");
    TEST_ASSERT_TRUE_MESSAGE(
        kld_file_contains("firmware/main/key_gesture_parser.cc", "key_route_layer("),
        "KLD-A1 RED: key_gesture_parser.cc 未定义 key_route_layer——路由表无实现"
        "（host runner 行为齿 test 1 也将持续红）");

    /* tooth 2: GLOBAL_ACT_OVERLAY_CLOSE 三文件消费链（.h 枚举追加 + .cc 路由返回 +
     * 板级 dispatch 消费; scan 覆盖 >= 3 文件）。 */
    n = kld_scan_count("firmware/main", "GLOBAL_ACT_OVERLAY_CLOSE", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        kld_fm("KLD-A1 RED: GLOBAL_ACT_OVERLAY_CLOSE 仅 %d 文件引用（期望 >= 3: 枚举"
               "追加 + 路由返回 + dispatch 消费）——覆盖层关层动作无端到端消费链", n));

    /* tooth 3: layer 栈 owner 侧前台归一化查询被板级 dispatch 消费（分流的事实源）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kld_file_contains("firmware/main/display/kidpal_render_node.h",
                          "kidpal_render_node_front_is_overlay"),
        "KLD-A1 RED: kidpal_render_node.h 未声明 kidpal_render_node_front_is_overlay"
        "——layer 归属查询未落 layer 栈 owner 侧（seam [3] home 纪律）");
    TEST_ASSERT_TRUE_MESSAGE(
        kld_file_contains(KLD_BOARD_CC, "kidpal_render_node_front_is_overlay("),
        "KLD-A1 RED: 板级 dispatch 未消费 kidpal_render_node_front_is_overlay——"
        "按键 dispatch 仍不看当前层（requirements §2.3 对账 ①「恒产 PAGE_NEXT、不看"
        "当前层」未拆）");

    /* tooth 4: 板级手势回调去直连——旧接线模式
     * keya_dispatch_global_action(key_route_global(KEY_ID_*, ...)) 必须全部拆除
     * （BOOT 单击恒产 PAGE_NEXT 的硬路由根因; 精确 needle 不含注释字面）。 */
    TEST_ASSERT_FALSE_MESSAGE(
        kld_file_contains(KLD_BOARD_CC, "keya_dispatch_global_action(key_route_global("),
        "KLD-A1 RED: 板级手势回调仍直连 keya_dispatch_global_action("
        "key_route_global(...))——layer-aware 分流未接管手势入口（覆盖层顶层 BOOT 仍"
        "恒产翻页, 对账 ① 根因在世）");

    /* tooth 5: 板级 KEY/BOOT 手势经 key_route_layer 分流（正向接线证据）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kld_file_contains(KLD_BOARD_CC, "key_route_layer(KEY_ID_BOOT") &&
            kld_file_contains(KLD_BOARD_CC, "key_route_layer(KEY_ID_KEY"),
        "KLD-A1 RED: 板级 BOOT/KEY 手势未改经 key_route_layer 分流——layer-aware "
        "派发未接管手势入口（对账 ①⑤ 修复面缺失）");

    /* tooth 6: 显式关层步骤被板级 dispatch 消费（OVERLAY_CLOSE -> dismiss_front_overlay,
     * 永不映射翻页）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kld_file_contains("firmware/main/display/kidpal_render_node.h",
                          "kidpal_render_node_dismiss_front_overlay"),
        "KLD-A2 RED: kidpal_render_node.h 未声明 kidpal_render_node_dismiss_front_overlay"
        "——设备侧显式关层步骤缺失（「关窗」仍只能依赖翻页函数清层副作用）");
    TEST_ASSERT_TRUE_MESSAGE(
        kld_file_contains(KLD_BOARD_CC, "kidpal_render_node_dismiss_front_overlay("),
        "KLD-A1 RED: 板级 dispatch 未把 OVERLAY_CLOSE 映射到 "
        "kidpal_render_node_dismiss_front_overlay——覆盖层关层回原页动作无落点");

    /* tooth 7: 顶层覆盖层裁决函数设备侧首次接线（对账 ①）——kidpal_reminder_key 出现
     * 在 >= 3 文件（reminder_page.c 定义 + reminder_page.h 声明 + 板级 dispatch 调用点;
     * 现状恰 2 文件 = 定义+声明, 设备侧零调用）。 */
    n = kld_scan_count("firmware/main", "kidpal_reminder_key(", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        kld_fm("KLD-A1 RED: kidpal_reminder_key 仅 %d 文件引用（定义+声明, 无设备侧"
               "调用点）——顶层覆盖层裁决函数仍设备侧零接线（requirements §2.3 对账 ①"
               "「正经裁决函数设备侧零调用」未修复）", n));
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — KD-A2 清层与进页分离（behavioral + structural）
 * 行为半: kidpal_layer_pop_transients 显式清层决策表（weak 哨兵, 强定义落
 * kidpal_layer.c 后覆盖）。结构半: cycle_next 函数体（稳定签名切区）零
 * kidpal_layer_pop 调用（清层副作用拆除）+ 翻页步进仍在（进页归进页）+
 * pop_transients 被设备侧消费（单一清层事实源）。
 * RED today: 行为半哨兵 -2; 结构半 cycle_next 区内恰有 3 处 kidpal_layer_pop。
 * ------------------------------------------------------------------------ */
/* coverage: KD-A2 -- 清层/进页分离机制齿：pop_transients 显式清层决策表 + cycle_next 函数体零 kidpal_layer_pop + 主页面翻页步进零回归 */
TEST_CASE("键位分层派发: 清层与进页分离——显式清层步骤+cycle_next去副作用 (KD-A2)",
          "[key_layer_dispatch][KD-A2][layer]")
{
    kidpal_layer_stack_t st;
    int popped;

    /* ---- 行为半: kidpal_layer_pop_transients 决策表 ------------------------ */
    /* fixture: 纯基层栈 [LOCAL_PAGE] */
    memset(&st, 0, sizeof(st));
    st.n = 0;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_LOCAL_PAGE) == 0,
        "KLD-A2 RED(infra): kidpal_layer_push 底座回归——fixture 无法建栈");

    /* tooth 1: 纯基层栈 -> 清层 no-op（0 层被出, 基层不动）。 */
    popped = kidpal_layer_pop_transients(&st);
    kld_require_implemented(popped, "kidpal_layer_pop_transients unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(popped == 0,
        kld_fm("KLD-A2 RED: 纯基层栈清层返回 %d, 期望 0——基层 LOCAL_PAGE 被误当 "
               "transient 清掉（主页面翻页/回首页路径会被误伤）", popped));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_top(&st) == KIDPAL_LAYER_LOCAL_PAGE,
        "KLD-A2 RED: 清层后基层丢失——LOCAL_PAGE 必须不被 pop_transients 触碰");

    /* tooth 2: 单覆盖层栈 [LOCAL_PAGE, REMINDER] -> 出 1 层, top 回基层
     * （提醒弹窗关层回原页的栈级机制）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_REMINDER) == 0,
        "KLD-A2 RED(infra): fixture push REMINDER failed");
    popped = kidpal_layer_pop_transients(&st);
    TEST_ASSERT_TRUE_MESSAGE(popped == 1,
        kld_fm("KLD-A2 RED: 单覆盖层栈清层返回 %d, 期望 1——REMINDER 未被显式步骤出栈",
               popped));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_top(&st) == KIDPAL_LAYER_LOCAL_PAGE,
        "KLD-A2 RED: 覆盖层清掉后前台未回落基层——「关层回原页」栈级机制缺失");

    /* tooth 3: 满覆盖栈 [LOCAL_PAGE, VOICE, SERVER_PAGE, REMINDER] -> 出 3 层
     * （三覆盖层一网打尽, 幂等不残留）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_VOICE) == 0 &&
                                 kidpal_layer_push(&st, KIDPAL_LAYER_SERVER_PAGE) == 0 &&
                                 kidpal_layer_push(&st, KIDPAL_LAYER_REMINDER) == 0,
        "KLD-A2 RED(infra): fixture push transients failed");
    popped = kidpal_layer_pop_transients(&st);
    TEST_ASSERT_TRUE_MESSAGE(popped == 3,
        kld_fm("KLD-A2 RED: 满覆盖栈清层返回 %d, 期望 3（VOICE/SERVER_PAGE/REMINDER "
               "各一条）——transient 集不全或幂等性缺失", popped));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_top(&st) == KIDPAL_LAYER_LOCAL_PAGE,
        "KLD-A2 RED: 满覆盖栈清层后前台未回落基层");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_has(&st, KIDPAL_LAYER_VOICE) == 0 &&
                                 kidpal_layer_has(&st, KIDPAL_LAYER_SERVER_PAGE) == 0 &&
                                 kidpal_layer_has(&st, KIDPAL_LAYER_REMINDER) == 0,
        "KLD-A2 RED: 清层后仍有 transient 残留——覆盖层会叠屏复活");

    /* tooth 4: PROVISION 不碰（配网层零触碰, KD-E1——配网按键语义归配网通道）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_PROVISION) == 0,
        "KLD-A2 RED(infra): fixture push PROVISION failed");
    popped = kidpal_layer_pop_transients(&st);
    TEST_ASSERT_TRUE_MESSAGE(popped == 0,
        kld_fm("KLD-A2 RED: 含 PROVISION 栈清层返回 %d, 期望 0——配网层被本包清层"
               "步骤误碰（配网按键语义零触碰红线）", popped));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_top(&st) == KIDPAL_LAYER_PROVISION,
        "KLD-A2 RED: PROVISION 被清层步骤出栈——配网前台被破坏");

    /* tooth 5: NULL 防御 -> -1。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_pop_transients(NULL) == -1,
        "KLD-A2 RED: NULL stack must be defended (-1)");

    /* ---- 结构半: cycle_next 函数体清层副作用拆除 --------------------------- */
    {
        char *region = kld_region_between("firmware/main/display/kidpal_render_node.cc",
                                          "void kidpal_render_node_cycle_next(void)",
                                          "int kidpal_render_node_get_page(void)");
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            "KLD-A2 RED(infra): kidpal_render_node.cc 的 cycle_next 函数区切取失败"
            "（签名锚漂移——cycle_next/get_page 签名被改动须复核本齿锚）");

        /* tooth 6: 函数体内零 kidpal_layer_pop——「关窗依赖翻页函数开头清 transient
         * 层的副作用」拆除（对账 ①; 清层只住显式步骤）。 */
        TEST_ASSERT_TRUE_MESSAGE(kld_count_in_slice(region, "kidpal_layer_pop(&") == 0,
            kld_fm("KLD-A2 RED: cycle_next 函数体内仍有 %d 处 kidpal_layer_pop——清层"
                   "副作用未拆除, 「关窗」仍寄生在翻页函数开头（requirements §2.3 对账 ①"
                   "耦合未拆; 清层必须走 kidpal_layer_pop_transients 显式步骤）",
                   kld_count_in_slice(region, "kidpal_layer_pop(&")));

        /* tooth 7 (guard): 进页步进仍在——清层拆出不得把翻页逻辑一起拆掉
         * （主页面 BOOT 单击翻页路径零回归, §2.3 主页面矩阵行）。 */
        TEST_ASSERT_TRUE_MESSAGE(kld_contains(region, "s_local_page"),
            "KLD-A2 RED(guard): cycle_next 函数体内不再推进 s_local_page——进页逻辑"
            "被连带拆除（清层/进页分离 = 拆副作用, 不是拆翻页）");
        free(region);
    }

    /* tooth 8: 显式清层步骤被设备侧消费（kidpal_render_node.cc 消费
     * pop_transients——单一清层事实源, 不允许设备侧另写一套逐层 pop 循环）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kld_file_contains("firmware/main/display/kidpal_render_node.cc",
                          "kidpal_layer_pop_transients"),
        "KLD-A2 RED: kidpal_render_node.cc 未消费 kidpal_layer_pop_transients——"
        "显式清层步骤没有落进 layer owner 的关层实现（清层事实源分叉风险）");
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — KD-A3/KD-E1 零回归守卫（structural + 冻结语义锚）
 * 主页面 KEY 长按 = 回首页映射零回归（layer-aware 裁决只改覆盖层列; GLOBAL_ACT_HOME
 * -> show_home 既有映射在世）; KD-E1 范围纪律守卫: 页枚举三页集守卫（原「PAGE_P4
 * 零触碰 / 撤 P4 归 M2」随 260917-m2-k2-retract-p4-three-page 落地翻正为「P4 已撤」）
 * + BOOT 长按故事翻屏接缝（P3 键位族归 M4 的在世证据）零触碰。
 * ------------------------------------------------------------------------ */
/* coverage: KD-A3 -- 主页面 KEY 长按回首页映射零回归守卫 */
/* coverage: KD-E1 -- 范围纪律守卫（2026-09-18 留痕修订随 M2-K2 撤 P4）：PAGE_P4 已撤（头内字面零残留）+ PAGE_COUNT==3 同源判别（复活第 4 页位即红）+ BOOT 长按故事翻屏接缝在世（P3 键位族归 M4） */
TEST_CASE("键位分层派发: 零回归守卫——HOME映射+P4页枚举+BOOT长按故事接缝 (KD-A3/KD-E1)",
          "[key_layer_dispatch][KD-A3][KD-E1][guard]")
{
    KLD_REQUIRE_ROOT();

    /* tooth 1 (KD-A3 guard): 板级 dispatch 仍把 GLOBAL_ACT_HOME 映射到
     * kidpal_render_node_show_home——主页面 KEY 长按 = 回首页（§2.3 主页面矩阵行;
     * 「上一层」在现行页集 = 首页, 取舍 C）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kld_file_contains(KLD_BOARD_CC, "kidpal_render_node_show_home"),
        "KLD-A3 RED(guard): 板级 GLOBAL_ACT_HOME -> show_home 映射丢失——主页面 KEY "
        "长按回首页路径被 layer-aware 改造误伤");

    /* tooth 2 (KD-E1 guard, 2026-09-18 留痕修订随 M2-K2 撤 P4 语义翻正):
     * 原齿（2026-09-15 冻结）正向断言 key_gesture_parser.h 含 PAGE_P4/PAGE_COUNT，其
     * 自注「页循环 4->3 / 撤 P4 归 M2」= 260917-m2-k2-retract-p4-three-page，本包落地
     * 后「零触碰」前提解除 -> 守卫翻正为钉「第 4 页位已撤」的正向事实（复活即红）。
     * PAGE_COUNT == 3 的字面真值单点钉在 retract_p4_three_page.acceptance.c test 3，
     * 本齿以同源判别复对账（不另立第二字面源）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kld_count_in_file("firmware/main/key_gesture_parser.h", "PAGE_P4") == 0 &&
            (int)PAGE_COUNT == 3 && (int)PAGE_P3 == (int)PAGE_COUNT - 1,
        kld_fm("KD-E1 RED: key_gesture_parser.h 页枚举不再等于三页集（PAGE_COUNT=%d, "
               "PAGE_P3=%d, 头内 PAGE_P4 字面 %d 处）——M2-K2 已撤的 P4 页位被复活 / "
               "页循环被改回 4 页（requirements §2.4 3 页循环 + 行 186）",
               (int)PAGE_COUNT, (int)PAGE_P3,
               kld_count_in_file("firmware/main/key_gesture_parser.h", "PAGE_P4")));

    /* tooth 3 (KD-E1 guard): BOOT 长按故事翻屏接缝在世（P3 页内特例; v1.47 P3 键位族
     * 对账 ⑥ 归 M4——该接缝是本包零触碰的在世证据, key_route_a1 KR-A4 tooth 2 同款）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kld_file_contains(KLD_BOARD_CC,
                          "HandleStoryKey(KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS)"),
        "KD-E1 RED: BOOT 长按故事翻屏接缝（HandleStoryKey KEY_ID_BOOT "
        "KEY_GESTURE_LONG_PRESS）被触碰——P3 页内特例归 M4（KD-E1 零触碰红线）");
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — boundary：本文件注册序守卫 + KD-E2 spec/契约 canary + 扫描器自检
 *   KD-A6 结构半（本验收文件是否被 builder 接线进 HOST_ACCEPTANCE_SRCS）
 *   KD-E2 需求面零修订 canary（docs/requirements.md §2.3 对账块锚 + 契约面在世）
 *   扫描器自检（结构齿负向必须能红, 防假绿）
 * ------------------------------------------------------------------------ */
/* coverage: KD-A6 -- host runner 终跑：本文件注册进 HOST_ACCEPTANCE_SRCS 结构守卫 + suite 级判读锚 */
/* coverage: KD-E2 -- 需求面零修订 canary：requirements §2.3 对账块锚 + 契约面在世 */
TEST_CASE("键位分层派发: 注册序守卫+需求契约canary+扫描器自检 (KD-A6/KD-E2)",
          "[key_layer_dispatch][KD-A6][KD-E2][boundary]")
{
    char *mk;
    char hit[KLD_MAX_PATH];
    int n;

    KLD_REQUIRE_ROOT();

    /* tooth 1: 本文件须接线进 HOST_ACCEPTANCE_SRCS 且排在 host_infra_smoke 之前
     * （smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫）。RED until builder wires. */
    mk = kld_read("firmware/main/test/host_app/main/CMakeLists.txt", NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "KLD RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *mine = strstr(mk, "key_layer_dispatch.acceptance.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(mine != NULL,
            "KD-A6 RED: key_layer_dispatch.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS "
            "— host 验收构建未接本包（BUILDER DUTY: 与本文件头注 [4] 同款接线）");
        TEST_ASSERT_TRUE_MESSAGE(smoke != NULL,
            "KLD RED(infra): host_infra_smoke.acceptance.c 从 HOST_ACCEPTANCE_SRCS 丢失 "
            "— 包 0 守卫被移除");
        TEST_ASSERT_TRUE_MESSAGE(mine < smoke,
            "KD-A6 RED: key_layer_dispatch.acceptance.c 必须排在 host_infra_smoke."
            "acceptance.c 之前 (smoke 末位 = 最先执行, boundary 齿注册序守卫)");
    }
    free(mk);

    /* tooth 2 (KD-E2 canary): 需求面在世且 ⚠️ 键位实现侧对账块锚仍在（KD-E2 需求零
     * 修订红线——本包只留实现证据, 对账 ①⑤ 状态更新归主线程对账轮, KD-E3）。 */
    TEST_ASSERT_TRUE_MESSAGE(kld_exists("docs/requirements.md"),
        "KD-E2 RED(infra): docs/requirements.md 丢失——需求面被触碰（KD-E2 零修订红线）");
    TEST_ASSERT_TRUE_MESSAGE(
        kld_file_contains("docs/requirements.md", "键位实现侧对账"),
        "KD-E2 RED: requirements §2.3 ⚠️ 键位实现侧对账块锚丢失——需求面被修订"
        "（KD-E2: 发现冲突走 spec-lane 逃生门, 不预先改需求）");
    TEST_ASSERT_TRUE_MESSAGE(kld_exists("docs/contracts/interface-contract.md"),
        "KLD RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");

    /* tooth 3: 扫描器自检（防假绿: 负向齿必须能红）。 */
    TEST_ASSERT_TRUE_MESSAGE(kld_exists("firmware/main/key_gesture_parser.cc"),
        "KLD RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(kld_exists("firmware/main/__kld_no_such__.cc"),
        "KLD RED(infra): scanner reports a nonexistent file as present");
    n = kld_scan_count("firmware/main", "__kld_no_such_needle__", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "KLD RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    n = kld_count_in_file("firmware/main/key_gesture_parser.cc", "key_route_global");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "KLD RED(infra): occurrence counter cannot count a known literal — 计数器失灵");
    {
        char *region = kld_region_between("firmware/main/key_gesture_parser.cc",
                                          "global_action_t key_route_global(",
                                          "page_id_t key_page_next(");
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            "KLD RED(infra): region slicer cannot slice a known function — 切区器失灵");
        TEST_ASSERT_TRUE_MESSAGE(kld_count_in_slice(region, "GLOBAL_ACT_PAGE_NEXT") >= 1,
            "KLD RED(infra): region slicer lost a known literal — 切区器计数失灵");
        free(region);
    }
}

/* ---------------------------------------------------------------------------
 * KD-A6 终跑判读（suite 级判读说明, review-packet 呈 supervisor）:
 *   判据 = 相对有效基线零新增红 + 本包新齿全绿。
 *   有效基线 = launch 侧 2026-09-15 实跑 host 面全量既有齿: 129 Tests / 4 Failures /
 *   0 Ignored, 四条全为在册既存红（非本包责任, 不代修）:
 *     (1) SU-E3  screen_unification.acceptance.c:1010（docs/spec.md 归档悬空——
 *         spec 单文件已迁 docs/archive/spec.md, canary 锚未随迁）
 *     (2) KR-A6  key_route_a1.acceptance.c:636（同根 docs/spec.md canary）
 *     (3) WSA-A6 schedule_reminder... device_ws_switch.acceptance.c:830（ECS shim
 *         收尾证据, owner = 260903-device-ws-switch）
 *     (4) WSA-A4 device_ws_switch.acceptance.c:663（flash.log 烧录证据, owner 同上）
 *   在册红出处 = delivery/changes/dev/260911-key-route-a1/existing-red-inventory.md
 *   + 260911-render-pages/red-baseline-freeze.md（判读纪律 = 新增红才是本包责任）。
 *   本包义务红（设计态, builder 落地后转绿, 不计入「新增红」违约）:
 *     - 本文件 test 1-4（seam 未落地 = 真实缺失行为红）;
 *     - schedule_reminder_b.acceptance.c RMD-A5-key 三处隔离钉点（KD-A4 成批修订后
 *       对旧「照常翻页」实现为设计态红, 见该文件 REVISION LOG 2026-09-15 条目）。
 *   终跑复核命令与实测数字见
 *   delivery/taskpackages/260915-m1-k1a-key-layer-dispatch/red-baseline-freeze.md。
 *
 * RED baseline 判读口径（freeze 相位, acceptance-author）:
 *   通道① 既有 host runner 全量实跑 = 在册红 4 条不变（见上）+ RMD-A5-key 1 条本包
 *   义务红（KD-A4 修订钉点对旧实现设计态红）。
 *   通道② 本文件新齿 C 类冻结验收实编（DEV-GOVERNANCE 验收测试纪律 §4）:
 *     gcc -std=c11 -Wall -Wextra -fsyntax-only -I firmware/main/test/acceptance/
 *         red-baseline -I firmware/main/test firmware/main/test/key_layer_dispatch.
 *         acceptance.c
 *     编译通道 error 为零（验收文件本身可编译）+ 解析性红定性: key_route_layer /
 *     kidpal_layer_pop_transients / kidpal_render_node_front_is_overlay /
 *     kidpal_render_node_dismiss_front_overlay / GLOBAL_ACT_OVERLAY_CLOSE 全部零命中、
 *     板级 keya_dispatch_global_action(key_route_global( 直连仍在、cycle_next 区内
 *     3 处 kidpal_layer_pop——各齿对现状必红, 红基线成立（接缝缺失即测试意图缺的
 *     真实行为）。
 *   cwd 口径: 结构齿以真实工作树扫描, root 自手持 cwd 定位（仓根 / firmware/main/test/
 *   host_app 均可）; 凡引用 delivery/changes/dev/ 下证据路径一律按相对路径 + 仓根
 *   cwd 前提（C 侧遗留指针纪律, root 定位吃 cwd）。
 * ---------------------------------------------------------------------------
 * 真机证据要求（KD-A5, 本文件不承载, human 持板轮）:
 *   覆盖层内 BOOT = 关层回原页不翻页、主页面 BOOT = 翻页、KEY 长按返回上一层 三行为
 *   真机手感确认; 证据落 delivery/changes/dev/260915-m1-k1a-key-layer-dispatch/
 *   （human 持板轮日志/照片）。
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-15  initial freeze (acceptance-author, TaskPackage
 *               260915-m1-k1a-key-layer-dispatch).
 *   2026-09-18  KD-E1 撤页守卫语义翻正（acceptance-author 留痕修订，授权通道 =
 *               TaskPackage 260917-m2-k2-retract-p4-three-page M2K2-E2「冻结套件钉点
 *               修订一律走 acceptance-author 留痕」）。改了什么：test 4 tooth 2 原正向
 *               断言 key_gesture_parser.h 含 PAGE_P4 与 PAGE_COUNT（其自注「页循环 4->3
 *               / 撤 P4 归 M2」点名的就是该包），M2-K2 撤页落地后「零触碰」前提解除 ->
 *               现钉 PAGE_P4 头内字面零残留 + PAGE_COUNT == 3 + PAGE_P3 == PAGE_COUNT-1
 *               三条同源判别（字面 3 的单点钉死仍住 retract_p4_three_page.acceptance.c
 *               test 3，本齿不另立第二字面源）。为何属留痕通道而非绕闸：只随已冻结的
 *               需求事实（requirements §2.4 行 186）改守卫指向，未放宽本包任何语义断言
 *               ——同 test tooth 1（HOME 映射在世）与 tooth 3（BOOT 长按故事接缝在世）、
 *               以及 test 1/2/3/5 全部零改动（M2K2-A3 回归判据要求本套件除本齿外零
 *               修改仍全绿）。红态后果：本齿对未撤页现状（PAGE_COUNT=4）为设计态红，
 *               builder 落 M2-K2 实现后转绿；上方 2026-09-15 冻结相位的「本包义务红」
 *               历史记录不改写（append-only）。
 * --------------------------------------------------------------------------- */
