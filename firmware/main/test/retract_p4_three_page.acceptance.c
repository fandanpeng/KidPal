/*
 * Frozen acceptance tests — TaskPackage 260917-m2-k2-retract-p4-three-page
 * (subsystem: firmware; changeId 编号 M2-K2; 撤 P4 常驻页 = 页循环 4→3 + 两页枚举
 * 对齐 spec P1→P2→P3 + 键帽条中槽页点 4→3，收回 M1-K2 的 KB-E1 deviation)。
 *
 * Story: 里程碑 delivery/milestones/milestones.jsonl#M2-char-window 的撤页对账项。
 * 语义权威 = docs/requirements.md 现文：§2.4 页面结构（3 页循环）图 + 行 186
 * 「P4 查字页已撤销（2026-09-14 v1.36）」+ §2.2 comp-keybar 行「中 3 页点 / 页点随之
 * 4→3」+ §2.3 对账 ② 页序漂移（「撤 P4 改 3 页时一并理顺」）+ F3 全节（查字改窗、
 * 本页只撤常驻页位、不拆查字能力）。完成后人能看到什么（roadmap ③ 条目）：BOOT 翻页
 * 从 4 页变 3 页循环（首页→课表→故事），查字页不再作为常驻页被翻到；页点从 4 点变
 * 3 点、翻页序与 spec P1→P2→P3 一致；查字能力本身还在（服务端下发仍能触发渲染）。
 *
 * Nature: HOST-SIDE BEHAVIORAL（纯逻辑 key_page_next / kidpal_keybar_dots /
 * kidpal_keybar_slots / key_route_global / key_route_layer —— key_gesture_parser.cc
 * 与 display/keybar.c 均已编入 host runner，真行为面，不走 weak 哨兵）+ HOST-SIDE
 * STRUCTURAL（设备专属面 kidpal_render_node.{h,cc} / lcd_display.cc / mcp_server.cc
 * 不进 host 构建，齿形态照 key_layer_dispatch.acceptance.c KD-A2 结构半与 KD-E1 文本
 * 齿先例：稳定签名锚切区 + 区内计数，零行号钉点）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements，本文件只读扫工作树 + 真调
 * 已入 runner 的纯逻辑实现; 「home =」为冻结落点）:
 *
 *   [1] key 路由页枚举（home = firmware/main/key_gesture_parser.h）:
 *       typedef enum { PAGE_P1 = 0, PAGE_P2, PAGE_P3, PAGE_COUNT } page_id_t;
 *         -- PAGE_P4 **整值撤出**（不留占位、不复用编号），PAGE_COUNT = 3。
 *       page_id_t key_page_next(page_id_t pg);
 *         -- 步进 = (pg + 1) % PAGE_COUNT（基数同源，撤页后自动塌成 mod 3）；
 *            非法页（pg >= PAGE_COUNT，含负值/越界 id）回 PAGE_P1（防御语义零改动）。
 *
 *   [2] 渲染节点页枚举（home = firmware/main/display/kidpal_render_node.h）:
 *       与 [1] 逐项同序、同值：KP_PAGE_HOME = 0 / KP_PAGE_SCHEDULE = 1 /
 *       KP_PAGE_STORY = 2 / KP_PAGE_COUNT = 3；**KP_PAGE_CHAR 整值撤出**（CHAR 退出
 *       本地浏览页循环；查字的前台归属由 M2-K3 改窗决定，本包不钉其枚举最终形态，
 *       只钉「不再是循环停靠页」——见 [3]）。
 *       两枚举数值逐项相等 => lcd_display.cc 的 KP_PAGE_* -> PAGE_Pn 索引映射
 *       塌成恒等（HOME→P1 / SCHEDULE→P2 / STORY→P3）；映射的**实现形状不钉**（switch
 *       保留或化简为直传皆可），本齿以枚举数值等式钉语义（shape-independent）。
 *
 *   [3] 翻页循环与页面渲染权威（home = firmware/main/display/kidpal_render_node.cc）:
 *       kidpal_render_node_cycle_next() 步进基数仍写 `KP_PAGE_COUNT`（同源，不写死
 *       数字），函数区内不再出现 KP_PAGE_CHAR 字面（不得靠「先跳 CHAR 再 +1」之类
 *       旁路把撤掉的页位塞回循环）。
 *       rn_render_kidpal_page_locked()（cycle_next 与 dismiss_front_overlay 共用的
 *       单一页面渲染权威）内 **零 KP_PAGE_CHAR** —— CHAR 不再是可停靠页；P1/P2/P3
 *       三条 case 全在世（撤 CHAR 不得连带拆掉三页渲染）。
 *
 *   [4] 键帽条纯逻辑单源（home = firmware/main/display/keybar.c）:
 *       页点总数同源 = PAGE_COUNT（[1] 撤页后自动 4→3，本模块零新数字面）；
 *       页槽表的 `case PAGE_P4:` 兜底分支随枚举撤出而消解（M1-K2 KB-E1 deviation
 *       的对账收回点）；page >= PAGE_COUNT（含原 P4 索引 3）由防御列一律返回 -1，
 *       kidpal_keybar_slots 不再为第 4 页槽位提供存在性兜底。
 *
 *   [5] 查字数据通路保留面（M2K2-E3 越包红线的对偶，本包零触碰）:
 *       firmware/main/mcp_server.cc 的 `case 3:` 路由块（块注释自标 VRT_ROUTE_CHAR）
 *       （ack route=char + kidpal_char_data_validate 缺字段降级门 +
 *       kidpal_char_page_show 调用点 + kidpal_render_node_cache_char_data 调用点）
 *       整块保留；display/view_router.{h,c} 的 VRT_ROUTE_CHAR = 3 数值码零改动
 *       （契约 §1.3 ① 锚）；display/char_page.{h,cc} + display/char_stroke.c 渲染面
 *       文件保留；kidpal_render_node_cache_char / cache_char_data 两个循环支撑 API
 *       随撤页成写侧死态 —— 本包**保留不动**，由 M2-K3 改窗时统一收（在 landing
 *       summary 如实登记，禁静默删除）。窗语义归 K3，本包不为其新增任何行为断言。
 *
 *   [6] Builder FIRST ACTION（本文件进 runner 的唯一前置）: 向
 *       firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS
 *       追加一行（相对 host_app/main）:
 *         "../../retract_p4_three_page.acceptance.c"
 *       置于 "../../host_infra_smoke.acceptance.c" 之前（smoke 保持末位 = 最先执行，
 *       包 0 boundary 齿注册序守卫；本文件 test 7 tooth 1 校验此序）。被测实现
 *       key_gesture_parser.cc（test 1/3/5 真行为源）与 display/keybar.c（test 4 真
 *       行为源）**已在册**，本包零新增被测实现接线（+1/-0）。
 *
 *   [7] mutation 自证（mutationRequired=true）：变异命名不得保留被测符号原字面为
 *       子串（KR-A3 _TOGGLE_X 教训）；逐齿可反转性见 acceptance-author 交付报告
 *       （mutationEvidence.perAcceptance 由 planner 回填，TaskPackage 面对本角色
 *       enforce-scope 只读）。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: M2K2-A1 三页循环齿 -> test 1（行为半：key_page_next P1→P2→P3→P1 回绕 +
 *             三步环 + 非法页/越界回 P1 错误路径）+ test 2（结构半：render_node.cc
 *             cycle_next 区基数同源 + 零 CHAR 旁路 + 渲染权威区内 CHAR 停靠拆除）
 *   coverage: M2K2-A2 枚举对齐 + 页点 3 点齿 -> test 3（两枚举与 spec P1→P2→P3 逐项
 *             同序、字面 3 单点钉死 + 两 header 撤值文本齿 + 页序漂移 prose 对账齿）
 *             + test 4（kidpal_keybar_dots 三页各自 total 同源 PAGE_COUNT /
 *             覆盖层隐藏不抹状态 / 原 P4 索引 3 落入越界防御 -1 / keybar.c PAGE_P4
 *             槽表兜底 case 零残留）
 *   coverage: M2K2-A3 键路由回归靶 -> test 5（BOOT 单击恒 GLOBAL_ACT_PAGE_NEXT、
 *             覆盖层内 BOOT = OVERLAY_CLOSE 不产翻页、KEY 长按族零偏移 + 路由→页序
 *             复合环 + 步进永不越界）；suite 级判据「key_layer_dispatch.acceptance.c
 *             全套除 KD-E1 撤页守卫修订外零修改仍全绿」见文件尾「终跑判读」段
 *   coverage: M2K2-A4 查字数据通路保留靶 -> test 6（VRT_ROUTE_CHAR 数值码 + mcp
 *             case 3 区内四处消费点 + 文件级调用点在世 + char_page/char_stroke 渲染
 *             面文件在世 + 循环支撑 API 保留守卫）
 *   coverage: M2K2-A5 真机翻页确认 -> 不落 C 齿（kind=structural 对象条目，deferred
 *             归 M2-K3 设备轮；判读与真机证据要求见文件尾「M2K2-A5 deferred」段）
 *   coverage: M2K2-E1 host 全量零新增红 -> 文件尾「终跑判读」段（基线锚 39T/0F @
 *             ca7ff81 + 本文件 7 TEST_CASE 的注册增量）+ test 7（注册序守卫）
 *   coverage: M2K2-E2 铁律（零契约/零 spec/零 agent-gate 触碰；冻结套件修订走
 *             acceptance-author 留痕）-> test 7 tooth 2/3（requirements 三页锚 +
 *             契约面在世 canary + 扫描器自检）+ 两处在世套件修订的 REVISION LOG
 *             条目（key_layer_dispatch.acceptance.c KD-E1 / keybar_three_slot.
 *             acceptance.c KKB-E1 + KB-A3 P4 槽位齿，同批留痕，非本文件断言面）
 *   coverage: M2K2-E3 通路不顺手拆 -> test 6（与 M2K2-A4 同一承载，本包零触碰面以
 *             must-stay-green 守卫形态钉死；死态 API 登记义务在 landing summary）
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   0) builder wiring 见 [6]。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash -c "cd /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app && ./build/host_infra_test.elf"
 *      （elf 相对 host_app/ 目录，从仓根跑 ./build/... 必报 No such file）
 *   Freeze-phase（本文件未接线前，DEV-GOVERNANCE 验收测试纪律 §3/§4 standalone 实编
 *   实跑，同一份冻结件 + 同一批已在册真实现源；相对路径以 red-baseline 目录为 cwd。
 *   注：keybar.c 引用 kidpal_voice_state_look（实现在 kidpal_voice_states.c，runner
 *   内两者均已在册），standalone 链接须一并带上该源，否则 undefined reference）:
 *     cd firmware/main/test/acceptance/red-baseline
 *     g++ -c -o /tmp/m2k2/kgp.o ../../../key_gesture_parser.cc
 *     gcc -std=c11 -Wall -Wextra -I. -o /tmp/m2k2/rtp_elf \
 *         ../../retract_p4_three_page.acceptance.c ../../../display/keybar.c \
 *         ../../../display/kidpal_voice_states.c /tmp/m2k2/kgp.o runner.c
 *     cd <仓根> && /tmp/m2k2/rtp_elf
 *   红基线逐齿判读见文件尾「RED baseline 判读口径」段。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>

/* 真行为面（两文件均已在 HOST_ACCEPTANCE_SRCS 在册，强符号直接消费——本包无
 * 未实现 seam，故不设 weak 哨兵；若在册实现被误摘，链接期即响亮失败，非假绿）。 */
#include "../key_gesture_parser.h"
#include "../display/keybar.h"
/* 设备侧枚举与数值码（header 纯声明、include 链 LVGL-free -> host 可编；
 * .cc 实现不进 host 构建，故只作文本/切区齿的被扫对象，不调用其任何函数）。 */
#include "../display/kidpal_render_node.h"
#include "../display/view_router.h"

#define RTP_MAX_PATH 1024
#define RTP_MAX_READ (8u * 1024u * 1024u)

/* 被扫文件（结构齿锚，相对仓根；行号快照非长期事实源，齿锚全部用符号锚）。 */
#define RTP_KEY_H         "firmware/main/key_gesture_parser.h"
#define RTP_RN_H          "firmware/main/display/kidpal_render_node.h"
#define RTP_RN_CC         "firmware/main/display/kidpal_render_node.cc"
#define RTP_KEYBAR_C      "firmware/main/display/keybar.c"
#define RTP_LCD_CC        "firmware/main/display/lcd_display.cc"
#define RTP_MCP_CC        "firmware/main/mcp_server.cc"
#define RTP_VIEW_ROUTER_C "firmware/main/display/view_router.c"
#define RTP_CHAR_PAGE_CC  "firmware/main/display/char_page.cc"
#define RTP_CHAR_PAGE_H   "firmware/main/display/char_page.h"
#define RTP_CHAR_STROKE_C "firmware/main/display/char_stroke.c"
#define RTP_CMAKELISTS    "firmware/main/test/host_app/main/CMakeLists.txt"
#define RTP_REQUIREMENTS  "docs/requirements.md"
#define RTP_CONTRACT      "docs/contracts/interface-contract.md"

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（host 侧结构齿，key_layer_dispatch / keybar_three_slot
 * 先例同款）。全部扫描跑在真实工作树上，root 以手持 cwd 定位（从仓根或
 * firmware/main/test/host_app 两处都可跑）。test / host_app / build / .git / assets
 * 目录排除——test 目录里住着本冻结文件自身（其注释合法提及冻结 needle）。
 * ------------------------------------------------------------------------ */

static char rtp_root[RTP_MAX_PATH];
static int rtp_root_ready;

static int rtp_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int rtp_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int rtp_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    size_t i;
    for (i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int rtp_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[RTP_MAX_PATH];
    size_t i;

    if (rtp_root_ready) {
        return 1;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (rtp_is_regular(probe)) {
            snprintf(rtp_root, sizeof(rtp_root), "%s", cands[i]);
            rtp_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int rtp_rel(char *out, size_t cap, const char *rel)
{
    if (!rtp_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", rtp_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", rtp_root, rel);
    return 1;
}

static int rtp_exists(const char *rel)
{
    char p[RTP_MAX_PATH];
    return rtp_rel(p, sizeof(p), rel) && rtp_is_regular(p);
}

static char *rtp_read(const char *rel, size_t *out_len)
{
    char p[RTP_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!rtp_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > RTP_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int rtp_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int rtp_file_contains(const char *rel, const char *needle)
{
    char *buf = rtp_read(rel, NULL);
    int r;
    if (buf == NULL) {
        return 0;
    }
    r = rtp_contains(buf, needle);
    free(buf);
    return r;
}

/* Occurrence count of needle inside one file (0 when unreadable/absent). */
static int rtp_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = rtp_read(rel, NULL);
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

static int rtp_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
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
        char abs_next[RTP_MAX_PATH];
        char rel_next[RTP_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            rtp_is_skipped_dir(e->d_name)) {
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
            count += rtp_count_files_with_needle(abs_next, rel_next, needle, first_hit,
                                                 cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > RTP_MAX_READ) {
                continue;
            }
            {
                char *buf = rtp_read(rel_next, NULL);
                if (buf != NULL) {
                    if (rtp_contains(buf, needle)) {
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

static int rtp_scan_count(const char *rel_dir, const char *needle, char *first_hit,
                          size_t cap)
{
    char abs[RTP_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!rtp_rel(abs, sizeof(abs), rel_dir) || !rtp_is_dir(abs)) {
        return 0;
    }
    return rtp_count_files_with_needle(abs, rel_dir, needle, first_hit, cap, 0);
}

/* Slice of rel from the FIRST occurrence of start_needle up to (exclusive) the first
 * occurrence of end_needle AFTER it.  Returns a malloc'd NUL-terminated slice or NULL
 * (unreadable file / either needle absent).  Used for function-body region pins
 * （cycle_next 步进基数区 / rn_render_kidpal_page_locked 停靠页 case 区 / mcp
 * case 3 路由块）——两个 needle 都取稳定签名行，禁行号钉点。 */
static char *rtp_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = rtp_read(rel, NULL);
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

static int rtp_count_in_slice(const char *slice, const char *needle)
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

/* 恒等映射的形状无关判别：`from` 之后 window 字符内出现 `to` 即算一对映射成立。
 * 用于 lcd_display.cc 的 KP_PAGE_* -> PAGE_Pn 映射齿（不钉空格/对齐/分号，
 * 只钉「这一页映射到这一页点」的语义邻接）。 */
static int rtp_maps_within(const char *hay, const char *from, const char *to,
                           int window)
{
    const char *p, *q;
    if (hay == NULL || from == NULL || to == NULL) {
        return 0;
    }
    p = strstr(hay, from);
    if (p == NULL) {
        return 0;
    }
    q = strstr(p, to);
    if (q == NULL) {
        return 0;
    }
    return (int)(q - p) <= window;
}

#define RTP_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(rtp_locate_root(), \
    "M2K2 RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* printf-style helper: dynamic diagnostics pre-formatted into a static buffer then
 * passed to the single-string assert macros（KLD/KKB 先例）。 */
static char rtp_fm_buf[1024];
static const char *rtp_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *rtp_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(rtp_fm_buf, sizeof(rtp_fm_buf), fmt, ap);
    va_end(ap);
    return rtp_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — M2K2-A1 行为半：key_page_next 三页回绕（behavioral，真实现源
 * key_gesture_parser.cc 已在 host runner 在册；现状 4 页基数 -> P3 落在第 4 页必红）。
 * ------------------------------------------------------------------------ */
/* coverage: M2K2-A1 -- 三页循环行为半：P1→P2→P3→P1 回绕（spec §2.4 3 页循环图逐边）+ 三步环 + 非法页/越界一律回 P1 */
TEST_CASE("撤 P4 三页循环: key_page_next 三页回绕——P3 回 P1 + 非法页防御 (M2K2-A1)",
          "[retract_p4_three_page][M2K2-A1][behavior]")
{
    int p, i;

    /* tooth 1/2 (must-stay green): 前两跳 = spec P1→P2→P3，撤页零回归。 */
    TEST_ASSERT_TRUE_MESSAGE((int)key_page_next(PAGE_P1) == (int)PAGE_P2,
        rtp_fm("M2K2-A1 RED(guard): key_page_next(P1) = %d 期望 PAGE_P2(1)——首页→课表"
               "第一跳被本包误伤（requirements §2.4 页序）",
               (int)key_page_next(PAGE_P1)));
    TEST_ASSERT_TRUE_MESSAGE((int)key_page_next(PAGE_P2) == (int)PAGE_P3,
        rtp_fm("M2K2-A1 RED(guard): key_page_next(P2) = %d 期望 PAGE_P3(2)——课表→记录"
               "第二跳被本包误伤（requirements §2.4 页序）",
               (int)key_page_next(PAGE_P2)));

    /* tooth 3: 第三跳必须回绕到 P1 —— 本包核心行为齿。现状 PAGE_COUNT = 4，
     * P3 的下一跳落在第 4 页停靠位（查字页），页循环仍 4 页 = 真实缺失行为。 */
    TEST_ASSERT_TRUE_MESSAGE((int)key_page_next(PAGE_P3) == (int)PAGE_P1,
        rtp_fm("M2K2-A1 RED: key_page_next(P3) = %d 期望回绕 PAGE_P1(0)——页循环仍含"
               "第 4 页停靠位（撤 P4 / 页循环 4→3 未落地; requirements §2.4「BOOT 单击 "
               "→ 切下一页（3 页循环）」+ 行 186「P4 查字页已撤销」）",
               (int)key_page_next(PAGE_P3)));

    /* tooth 4: 从首页起按三步 = 回到首页（循环停靠页恰三页，与 §2.4 图逐字对账）。 */
    p = (int)PAGE_P1;
    for (i = 0; i < 3; i++) {
        p = (int)key_page_next((page_id_t)p);
    }
    TEST_ASSERT_TRUE_MESSAGE(p == (int)PAGE_P1,
        rtp_fm("M2K2-A1 RED: 首页起连翻 3 步落在页 %d 期望回 P1(0)——三步走不完一轮循环"
               "（现循环步数 = %d，非 3; BOOT 翻页手感 = roadmap「4 页变 3 页循环」）",
               p, (int)PAGE_COUNT));

    /* tooth 5 (同源判别, must-stay green): PAGE_COUNT 步必回原点——基数与页数同源，
     * 撤页后本齿自动跟随，不钉字面 3（字面 3 的唯一钉点在 test 3）。 */
    p = (int)PAGE_P1;
    for (i = 0; i < (int)PAGE_COUNT; i++) {
        p = (int)key_page_next((page_id_t)p);
    }
    TEST_ASSERT_TRUE_MESSAGE(p == (int)PAGE_P1,
        rtp_fm("M2K2-A1 RED(infra): 走 PAGE_COUNT(%d) 步未回原点（落在 %d）——步进基数"
               "与页数不同源（key_page_next 必须 (pg+1) %% PAGE_COUNT）",
               (int)PAGE_COUNT, p));

    /* tooth 6 (错误/边界路径): 非法页一律回 P1，防御语义零改动（撤页不得把越界
     * 变成未定义行为，也不得把 P4 旧索引当成合法页继续步进）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_page_next((page_id_t)PAGE_COUNT) == (int)PAGE_P1 &&
        (int)key_page_next((page_id_t)(PAGE_COUNT + 1)) == (int)PAGE_P1 &&
        (int)key_page_next((page_id_t)99) == (int)PAGE_P1 &&
        (int)key_page_next((page_id_t)-1) == (int)PAGE_P1,
        rtp_fm("M2K2-A1 RED: 非法页未回 PAGE_P1（PAGE_COUNT=%d -> %d, +1 -> %d, "
               "99 -> %d, -1 -> %d）——越界防御面被撤页改动",
               (int)PAGE_COUNT, (int)key_page_next((page_id_t)PAGE_COUNT),
               (int)key_page_next((page_id_t)(PAGE_COUNT + 1)),
               (int)key_page_next((page_id_t)99),
               (int)key_page_next((page_id_t)-1)));
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — M2K2-A1 结构半：设备侧循环与停靠面（structural，源切区）
 * kidpal_render_node.cc 不进 host 构建 -> 稳定签名锚切区（KD-A2 结构半先例）：
 * cycle_next 区基数同源 + 零 CHAR 旁路；页面渲染权威区内 CHAR 停靠 case 拆除 +
 * 三页 case 全在世。RED today: 渲染区内 2 处 KP_PAGE_CHAR（case + s_local_page 赋值）。
 * ------------------------------------------------------------------------ */
/* coverage: M2K2-A1 -- 三页循环结构半：cycle_next 区步进基数同源 KP_PAGE_COUNT + 区内零 KP_PAGE_CHAR 旁路 + rn_render_kidpal_page_locked 区内 CHAR 停靠页拆除（三页 case 全在世守卫） */
TEST_CASE("撤 P4 三页循环: render_node 循环基数+CHAR 停靠页拆除 (M2K2-A1 结构半)",
          "[retract_p4_three_page][M2K2-A1][structural]")
{
    char *cyc, *render;

    RTP_REQUIRE_ROOT();

    /* tooth 1 (must-stay green): 步进基数同源写 KP_PAGE_COUNT（不写死数字）——撤页
     * 后 mod 4 自动塌成 mod 3，这是「循环基数随枚举走」的机械形态。 */
    cyc = rtp_region_between(RTP_RN_CC, "void kidpal_render_node_cycle_next(void)",
                             "int kidpal_render_node_get_page(void)");
    TEST_ASSERT_NOT_NULL_MESSAGE(cyc,
        "M2K2-A1 RED(infra): kidpal_render_node.cc 的 cycle_next 函数区切取失败（"
        "cycle_next / get_page 签名锚漂移——须复核本齿锚，不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(rtp_contains(cyc, "% KP_PAGE_COUNT"),
        "M2K2-A1 RED: cycle_next 函数区内不再以 `% KP_PAGE_COUNT` 取模——翻页基数被"
        "写死字面量（页数与循环基数脱离同源，下次页集变动又会漂移）");

    /* tooth 2: 循环区内零 KP_PAGE_CHAR 字面——撤掉的页位不得靠旁路（先跳 CHAR 再
     * +1、if(next==CHAR) next=... 之类）塞回循环。GREEN today、必须保持绿。 */
    TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_slice(cyc, "KP_PAGE_CHAR") == 0,
        rtp_fm("M2K2-A1 RED: cycle_next 函数区内仍有 %d 处 KP_PAGE_CHAR——CHAR 以旁路"
               "形态留在翻页循环里（撤页 = 循环面撤出，不是把停靠位改名后继续存在）",
               rtp_count_in_slice(cyc, "KP_PAGE_CHAR")));
    free(cyc);

    /* tooth 3 (本包红齿): 单一页面渲染权威（cycle_next 与 dismiss_front_overlay
     * 共用）内零 KP_PAGE_CHAR —— CHAR 不再是可停靠页，无数据占位 label 随之外移
     * 或消解（窗渲染归 M2-K3）。 */
    render = rtp_region_between(RTP_RN_CC,
                                "static void rn_render_kidpal_page_locked(kidpal_page_id_t page)",
                                "int kidpal_render_node_front_is_overlay(void)");
    TEST_ASSERT_NOT_NULL_MESSAGE(render,
        "M2K2-A1 RED(infra): kidpal_render_node.cc 的 rn_render_kidpal_page_locked "
        "函数区切取失败（签名锚漂移——须复核本齿锚）");
    TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_slice(render, "KP_PAGE_CHAR") == 0,
        rtp_fm("M2K2-A1 RED: 页面渲染权威区内仍有 %d 处 KP_PAGE_CHAR（case 标签 / "
               "s_local_page 赋值）——查字页仍是 BOOT 翻页可停靠的常驻页位，与 "
               "requirements §2.4 行 186「P4 查字页已撤销」冲突",
               rtp_count_in_slice(render, "KP_PAGE_CHAR")));

    /* tooth 4 (guard): 三页渲染 case 全在世——撤 CHAR 不得连带拆掉 P1/P2/P3 渲染
     * （「撤一页」不是「重建渲染面」）。 */
    TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_slice(render, "case KP_PAGE_HOME") >= 1 &&
                                 rtp_count_in_slice(render, "case KP_PAGE_SCHEDULE") >= 1 &&
                                 rtp_count_in_slice(render, "case KP_PAGE_STORY") >= 1,
        "M2K2-A1 RED(guard): 页面渲染权威区内三页 case 不全——撤 P4 连带拆掉了首页/"
        "课表/记录的渲染分支（应是零删除的减法面）");
    free(render);
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — M2K2-A2 枚举对齐齿（behavioral/编译期可见 + 两 header 文本齿）
 * 两页枚举与 spec P1→P2→P3 逐项同序；字面「3」在整个冻结面上的唯一钉点在此。
 * RED today: PAGE_COUNT=4 且 KP_PAGE_* 漂移序（HOME=0/CHAR=1/SCHEDULE=2/STORY=3）。
 * ------------------------------------------------------------------------ */
/* coverage: M2K2-A2 -- 枚举对齐齿：PAGE_P1=0/P2=1/P3=2/PAGE_COUNT=3 逐项 + KP_PAGE_HOME=0/SCHEDULE=1/STORY=2/COUNT=3 逐项 + 两枚举逐项等值（= lcd 映射塌成恒等的形状无关前提）+ 最后合法页 P3 + key_gesture_parser.h 的 PAGE_P4 撤出 / kidpal_render_node.h 枚举区 KP_PAGE_CHAR 撤出 + 页序漂移 prose 对账 */
TEST_CASE("撤 P4 三页循环: 两页枚举对齐 spec P1→P2→P3（字面 3 单点钉死） (M2K2-A2)",
          "[retract_p4_three_page][M2K2-A2][behavior]")
{
    char *enum_region;

    RTP_REQUIRE_ROOT();

    /* tooth 1: 键路由页枚举 = spec 三页序。**全冻结面唯一的「3」字面钉点**——
     * keybar_three_slot.acceptance.c KKB-E1 齿已随本包改为同源判别（total ==
     * PAGE_COUNT），页点数量的字面真值只在本处出现一次，两处并存会互相遮蔽。 */
    TEST_ASSERT_TRUE_MESSAGE((int)PAGE_P1 == 0 && (int)PAGE_P2 == 1 &&
                                 (int)PAGE_P3 == 2 && (int)PAGE_COUNT == 3,
        rtp_fm("M2K2-A2 RED: key_gesture_parser.h 页枚举 = (P1=%d, P2=%d, P3=%d, "
               "COUNT=%d) 期望 (0, 1, 2, 3)——页循环仍 4 页 / P4 位未撤 "
               "(requirements §2.4 3 页循环 + §2.2「中 3 页点」)",
               (int)PAGE_P1, (int)PAGE_P2, (int)PAGE_P3, (int)PAGE_COUNT));

    /* tooth 2: 最后合法停靠页 = P3（不允许存在第 4 个合法页索引）。 */
    TEST_ASSERT_TRUE_MESSAGE((int)PAGE_P3 == (int)PAGE_COUNT - 1,
        rtp_fm("M2K2-A2 RED: PAGE_P3(%d) != PAGE_COUNT-1(%d)——枚举尾后仍有页位 "
               "（P4 整值撤出的判别式）", (int)PAGE_P3, (int)PAGE_COUNT - 1));

    /* tooth 3: 渲染节点页枚举同序（requirements §2.3 对账 ②「页序漂移……撤 P4 改
     * 3 页时一并理顺」的正向半）。 */
    TEST_ASSERT_TRUE_MESSAGE((int)KP_PAGE_HOME == 0 && (int)KP_PAGE_SCHEDULE == 1 &&
                                 (int)KP_PAGE_STORY == 2 && (int)KP_PAGE_COUNT == 3,
        rtp_fm("M2K2-A2 RED: kidpal_render_node.h 页枚举 = (HOME=%d, SCHEDULE=%d, "
               "STORY=%d, COUNT=%d) 期望 (0, 1, 2, 3)——真机翻页序仍为"
               "首页→查字→课表→故事（§2.3 对账 ② 页序漂移未理顺）",
               (int)KP_PAGE_HOME, (int)KP_PAGE_SCHEDULE, (int)KP_PAGE_STORY,
               (int)KP_PAGE_COUNT));

    /* tooth 4: 两枚举逐项等值 => 设备侧 KP_PAGE_* -> PAGE_Pn 索引映射塌成恒等的
     * 语义前提（映射的实现形状不钉，见 test 3 tooth 6 的形状无关邻接齿）。 */
    TEST_ASSERT_TRUE_MESSAGE((int)KP_PAGE_HOME == (int)PAGE_P1 &&
                                 (int)KP_PAGE_SCHEDULE == (int)PAGE_P2 &&
                                 (int)KP_PAGE_STORY == (int)PAGE_P3 &&
                                 (int)KP_PAGE_COUNT == (int)PAGE_COUNT,
        rtp_fm("M2K2-A2 RED: 两页枚举未逐项同序（KP: %d/%d/%d/%d vs PAGE: %d/%d/%d/%d"
               "）——渲染页与键路由页仍需映射表换算，页序漂移在世（§2.3 对账 ②）",
               (int)KP_PAGE_HOME, (int)KP_PAGE_SCHEDULE, (int)KP_PAGE_STORY,
               (int)KP_PAGE_COUNT, (int)PAGE_P1, (int)PAGE_P2, (int)PAGE_P3,
               (int)PAGE_COUNT));

    /* tooth 5 (结构半): key_gesture_parser.h 内 PAGE_P4 字面零残留（枚举撤值 + 注释
     * 不复活该标识符；prose 里的「P1→P2→P3」不受本齿约束）。 */
    TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_file(RTP_KEY_H, "PAGE_P4") == 0,
        rtp_fm("M2K2-A2 RED: key_gesture_parser.h 仍出现 PAGE_P4 标识符 %d 处——P4 页位"
               "未整值撤出（不留占位、不复用编号）",
               rtp_count_in_file(RTP_KEY_H, "PAGE_P4")));

    /* tooth 6 (结构半, 形状无关邻接): lcd_display.cc 的页索引映射仍按语义逐对钉
     * （撤页后塌成恒等：SCHEDULE→P2 / STORY→P3；不钉空格对齐、不钉 switch 是否被
     * 化简为直传——只钉「若还写这一页，就必须映到这一点」）。 */
    {
        char *lcd = rtp_read(RTP_LCD_CC, NULL);
        TEST_ASSERT_NOT_NULL_MESSAGE(lcd,
            "M2K2-A2 RED(infra): lcd_display.cc 不可读——设备侧页映射齿失去被扫对象");
        TEST_ASSERT_TRUE_MESSAGE(
            rtp_count_in_slice(lcd, "KP_PAGE_SCHEDULE") == 0 ||
                rtp_maps_within(lcd, "KP_PAGE_SCHEDULE", "PAGE_P2", 40),
            "M2K2-A2 RED: lcd_display.cc 出现 KP_PAGE_SCHEDULE 但邻接不到 PAGE_P2——"
            "课表页的页点索引映射漂移");
        TEST_ASSERT_TRUE_MESSAGE(
            rtp_count_in_slice(lcd, "KP_PAGE_STORY") == 0 ||
                rtp_maps_within(lcd, "KP_PAGE_STORY", "PAGE_P3", 40),
            "M2K2-A2 RED: lcd_display.cc 出现 KP_PAGE_STORY 但邻接不到 PAGE_P3——记录页"
            "的页点索引映射漂移");
        /* 撤页后设备侧不得再有 CHAR/P4 页位入映射（枚举整值撤出 -> 引用即编译失败；
         * 含注释里的 "PAGE_P1..PAGE_P4" 区间描述，须随撤页对账）。 */
        TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_slice(lcd, "KP_PAGE_CHAR") == 0,
            rtp_fm("M2K2-A2 RED: lcd_display.cc 仍出现 KP_PAGE_CHAR %d 处——设备侧页"
                   "映射仍把查字页当常驻页", rtp_count_in_slice(lcd, "KP_PAGE_CHAR")));
        TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_slice(lcd, "PAGE_P4") == 0,
            rtp_fm("M2K2-A2 RED: lcd_display.cc 仍出现 PAGE_P4 %d 处——键路由页序第 4"
                   "页索引在设备侧仍在世", rtp_count_in_slice(lcd, "PAGE_P4")));
        free(lcd);
    }
    /* 事实源不变：设备侧仍从渲染节点读当前页（不得另立第二页真相源）。 */
    TEST_ASSERT_TRUE_MESSAGE(rtp_file_contains(RTP_LCD_CC, "kidpal_render_node_get_page("),
        "M2K2-A2 RED(guard): lcd_display.cc 不再消费 kidpal_render_node_get_page——"
        "页点当前位来源被撤页顺手改掉（应只撤 CHAR、不改取数链）");

    /* tooth 7 (结构半): kidpal_render_node.h 枚举区内零 KP_PAGE_CHAR（枚举声明的
     * 冻结落点）+ 三页与 COUNT 四标识符全在世。 */
    enum_region = rtp_region_between(RTP_RN_H, "typedef enum {", "} kidpal_page_id_t;");
    TEST_ASSERT_NOT_NULL_MESSAGE(enum_region,
        "M2K2-A2 RED(infra): kidpal_render_node.h 的页枚举区切取失败（typedef enum / "
        "kidpal_page_id_t 锚漂移——须复核本齿锚）");
    TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_slice(enum_region, "KP_PAGE_CHAR") == 0,
        "M2K2-A2 RED: kidpal_render_node.h 页枚举内仍有 KP_PAGE_CHAR——CHAR 仍占枚举位"
        "（本包只保证撤出循环；K3 改窗若需窗层标识，届时按新包重新钉形）");
    TEST_ASSERT_TRUE_MESSAGE(rtp_contains(enum_region, "KP_PAGE_HOME") &&
                                 rtp_contains(enum_region, "KP_PAGE_SCHEDULE") &&
                                 rtp_contains(enum_region, "KP_PAGE_STORY") &&
                                 rtp_contains(enum_region, "KP_PAGE_COUNT"),
        "M2K2-A2 RED(guard): kidpal_render_node.h 页枚举三页 + COUNT 不全——撤 CHAR 连带"
        "拆掉了三页枚举（减法面，不是重建面）");
    free(enum_region);

    /* tooth 8 (对账齿, §2.3 行 166 ②): 头注释里描述翻页序的 prose 不得继续声称
     * 「home→char→schedule→story」——页序漂移的对账含文档面自述（prose 与实际枚举
     * 冲突是本次漂移的定义，留它 = 留坑）。 */
    TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_file(RTP_RN_H, "home→char→schedule→story") == 0,
        rtp_fm("M2K2-A2 RED(drift): kidpal_render_node.h 仍用「home→char→schedule→"
               "story」描述翻页序 %d 处——枚举已改（或将改）而页序自述未理顺，"
               "§2.3 对账 ② 的漂移在文档面复发",
               rtp_count_in_file(RTP_RN_H, "home→char→schedule→story")));
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — M2K2-A2② 页点 3 点齿（behavioral，真实现源 display/keybar.c 已在册）
 * 三页各自基层 total 同源 PAGE_COUNT（不钉字面，字面 3 的唯一钉点在 test 3）+
 * 覆盖层隐藏不抹状态 + 原 P4 索引（PAGE_P3+1）落越界防御。
 * RED today: 索引 3 仍是合法页（dots/slots 返回 0、槽表兜底 case 在世）。
 * ------------------------------------------------------------------------ */
/* coverage: M2K2-A2 -- 页点齿：三页各自 overlay_top=0 -> visible=1 / total==PAGE_COUNT（同源，随本包 4→3）/ active=当前页 + 覆盖层隐藏不抹状态 + 原第 4 页索引越界防御 -1（dots 与 slots 两径）+ keybar.c PAGE_P4 槽表兜底 case 零残留 */
TEST_CASE("撤 P4 三页循环: 键帽条页点同源 3 点 + 第 4 页槽位撤除 (M2K2-A2)",
          "[retract_p4_three_page][M2K2-A2][behavior]")
{
    kidpal_keybar_dots_t d;
    const char *left = NULL, *right = NULL;
    int page, rc;

    /* tooth 1: 三页基层列——页点显示、总数与现行页循环同源、当前页实心。 */
    for (page = (int)PAGE_P1; page <= (int)PAGE_P3; page++) {
        memset(&d, 0, sizeof(d));
        rc = kidpal_keybar_dots(page, 0, &d);
        TEST_ASSERT_TRUE_MESSAGE(rc == 0,
            rtp_fm("M2K2-A2 RED: kidpal_keybar_dots(page=%d, base) rc=%d 期望 0——"
                   "三页中的合法主页被拒（撤页把保留页一起撤掉了）", page, rc));
        TEST_ASSERT_TRUE_MESSAGE(d.visible == 1,
            rtp_fm("M2K2-A2 RED: 主页面基层 page=%d visible=%d 期望 1——页点仅主页面"
                   "显示的基层半被破坏（requirements §2.2 comp-keybar）", page, d.visible));
        TEST_ASSERT_TRUE_MESSAGE(d.total == (int)PAGE_COUNT,
            rtp_fm("M2K2-A2 RED: page=%d 页点总数 %d != PAGE_COUNT(%d)——页点数量与现行"
                   "页循环脱钩（M1-K2 KB-E1 deviation 的对账收回 = 数量随枚举走，"
                   "本包撤页后应为 3 点，字面真值由 test 3 单点钉死）",
                   page, d.total, (int)PAGE_COUNT));
        TEST_ASSERT_TRUE_MESSAGE(d.active == page,
            rtp_fm("M2K2-A2 RED: page=%d 实心页点下标 %d != %d——当前页实心机制漂移",
                   page, d.active, page));
    }

    /* tooth 2: 覆盖层列——隐藏不抹机制状态（撤页零触碰该语义；查字窗窗内无页点
     * 由这条同源机制承载，窗语义本身归 M2-K3，本齿只钉「撤页没改掉隐藏语义」）。 */
    memset(&d, 0, sizeof(d));
    rc = kidpal_keybar_dots((int)PAGE_P3, 1, &d);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && d.visible == 0 &&
                                 d.total == (int)PAGE_COUNT && d.active == (int)PAGE_P3,
        rtp_fm("M2K2-A2 RED(guard): 覆盖层列 dots(P3, overlay) rc=%d 三件套 = (%d, %d, "
               "%d) 期望 (0, 隐藏, total==PAGE_COUNT, active==%d)——页点隐藏语义或"
               "「隐藏不抹状态」被撤页改动 (§2.2「覆盖层不显示页点」)",
               rc, d.visible, d.total, d.active, (int)PAGE_P3));

    /* tooth 3 (本包红齿 + 边界路径): 原第 4 页索引（= PAGE_P3 + 1）不再是合法页，
     * dots 与 slots 两径一律越界防御 -1。今日 PAGE_COUNT=4 -> 索引 3 合法、返回 0
     * （slots 还由 `case PAGE_P4:` 兜底给一套 P4 槽文案）= 撤页缺失的真实行为。 */
    memset(&d, 0, sizeof(d));
    rc = kidpal_keybar_dots((int)PAGE_P3 + 1, 0, &d);
    TEST_ASSERT_TRUE_MESSAGE(rc == -1,
        rtp_fm("M2K2-A2 RED: kidpal_keybar_dots(page=%d) rc=%d 期望 -1——第 4 页位仍可"
               "渲染页点（页点应随页循环 4→3; requirements §2.2「中 3 页点 / 页点随之 "
               "4→3」，M1-K2 KB-E1 deviation 本包收回）", (int)PAGE_P3 + 1, rc));

    left = NULL;
    right = NULL;
    rc = kidpal_keybar_slots((int)PAGE_P3 + 1, 0, 0, &left, &right);
    TEST_ASSERT_TRUE_MESSAGE(rc == -1,
        rtp_fm("M2K2-A2 RED: kidpal_keybar_slots(page=%d, base) rc=%d 期望 -1——撤页后"
               "第 4 页槽位仍由兜底 case 提供文案（keybar.c 的 `case PAGE_P4:` 兜底分支"
               "应随枚举消解，越界页交防御列）", (int)PAGE_P3 + 1, rc));

    /* tooth 4: 三页槽表文案在世（撤 P4 只减一行，P1/P2/P3 逐字不动）。 */
    left = NULL;
    right = NULL;
    rc = kidpal_keybar_slots((int)PAGE_P3, 0, 0, &left, &right);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && left != NULL && right != NULL &&
                                 strcmp(left, "说话·双击看") == 0 &&
                                 strcmp(right, "下移") == 0,
        "M2K2-A2 RED(guard): P3 页槽 (左, 右) 不再是 (说话·双击看, 下移)——撤页连带改"
        "了保留页的槽文案（requirements §2.2 线框行逐字，KB-A3 冻结映射）");

    /* tooth 5 (结构半): keybar.c 内 PAGE_P4 字面零残留。 */
    TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_file(RTP_KEYBAR_C, "PAGE_P4") == 0,
        rtp_fm("M2K2-A2 RED: keybar.c 仍出现 PAGE_P4 %d 处——页槽表第 4 行兜底未消解 "
               "（M1-K2 登记的 deviation 面尚未收回）",
               rtp_count_in_file(RTP_KEYBAR_C, "PAGE_P4")));
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — M2K2-A3 键路由回归靶（behavioral，must-stay-green 回归集）
 * TaskPackage A3 明示「本齿不新写断言」——回归判据 = 既有 key_layer_dispatch.
 * acceptance.c 全套（除 KD-E1 撤页守卫修订外）零修改仍全绿，suite 级判读见文件尾。
 * 本 test 只承载 A3 三条正文的可机械反转锚（coverage 标记须挂在可执行断言上，
 * 起包格式期望第 2 条）：不新建语义、只复查既有语义在 3 页态下零偏移。
 * ------------------------------------------------------------------------ */
/* coverage: M2K2-A3 -- 键路由回归靶：BOOT 单击恒 GLOBAL_ACT_PAGE_NEXT（基层）+ 覆盖层内 BOOT = OVERLAY_CLOSE 不产翻页 + KEY 长按族零偏移（基层 HOME / 覆盖层 OVERLAY_CLOSE）+ 路由→页序复合环 + 步进永不越界 */
TEST_CASE("撤 P4 三页循环: 键路由回归靶——BOOT 翻页动作+覆盖层隔离+KEY 长按族 (M2K2-A3)",
          "[retract_p4_three_page][M2K2-A3][regression]")
{
    int act, p, i;

    /* tooth 1: 基层路由表零偏移（撤页是页集改动，键位矩阵不动; §2.3 主页面矩阵行）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_global(KEY_ID_BOOT, KEY_GESTURE_CLICK) == (int)GLOBAL_ACT_PAGE_NEXT,
        "M2K2-A3 RED: BOOT 单击不再产 GLOBAL_ACT_PAGE_NEXT——撤页顺手改了键位矩阵"
        "（越包红线; M1-K1a KD-A1 冻结语义）");
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_global(KEY_ID_KEY, KEY_GESTURE_CLICK) ==
            (int)GLOBAL_ACT_SESSION_TOGGLE &&
            (int)key_route_global(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS) ==
                (int)GLOBAL_ACT_HOME,
        "M2K2-A3 RED: KEY 单击/长按族偏移（期望 SESSION_TOGGLE / HOME）——页集变动"
        "误伤会话主轴与回首页路径");

    /* tooth 2: 分层派发列零偏移——覆盖层内 BOOT = 关层回原页、不产翻页动作
     * （v1.39 隔离；撤 P4 不改这条，页点隐藏与按键语义各自单源）。 */
    act = (int)key_route_layer(KEY_ID_BOOT, KEY_GESTURE_CLICK, 1);
    TEST_ASSERT_TRUE_MESSAGE(act == (int)GLOBAL_ACT_OVERLAY_CLOSE &&
                                 act != (int)GLOBAL_ACT_PAGE_NEXT,
        rtp_fm("M2K2-A3 RED: 覆盖层顶层 BOOT 单击路由 = %d 期望 OVERLAY_CLOSE(4) 且 "
               "!= PAGE_NEXT(3)——覆盖层隔离被撤页改动", act));
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS, 1) ==
            (int)GLOBAL_ACT_OVERLAY_CLOSE,
        "M2K2-A3 RED: 覆盖层内 KEY 长按不再 OVERLAY_CLOSE——「返回上一层」被撤页误伤");
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer(KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS, 0) ==
            (int)key_route_global(KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS),
        "M2K2-A3 RED: 基层列 BOOT 长按与全局表不等价——layer-aware 表被页集改动破坏");

    /* tooth 3: 路由动作 -> 页序 的复合环（入口动作 = GLOBAL_ACT_PAGE_NEXT，此后每次
     * 动作恰好推进一页，三步跑完 spec §2.4 图的三页环并回首页）。与 test 1 tooth 4
     * 有意交叉：本齿是「键路由回归靶」的复合半（动作与落点一起看），test 1 是页序
     * 纯逻辑半。 */
    {
        int seq[4];
        TEST_ASSERT_TRUE_MESSAGE(
            (int)key_route_global(KEY_ID_BOOT, KEY_GESTURE_CLICK) ==
                (int)GLOBAL_ACT_PAGE_NEXT,
            "M2K2-A3 RED(infra): 复合环入口路由动作漂移");
        p = (int)PAGE_P1;
        for (i = 0; i < 3; i++) {
            seq[i] = p;
            p = (int)key_page_next((page_id_t)p);
        }
        seq[3] = p;
        TEST_ASSERT_TRUE_MESSAGE(seq[0] == (int)PAGE_P1 && seq[1] == (int)PAGE_P2 &&
                                     seq[2] == (int)PAGE_P3 && seq[3] == (int)PAGE_P1,
            rtp_fm("M2K2-A3 RED: 连翻三步页序 = (%d, %d, %d -> %d) 期望 "
                   "(P1, P2, P3 -> P1)——BOOT 翻页手感（3 页循环）在撤页后仍非三页环",
                   seq[0], seq[1], seq[2], seq[3]));
    }

    /* tooth 4 (边界, must-stay green): 每个合法页的下一跳都落在合法页集内——撤页
     * 若把基数算错（mod 写歪 / 枚举与基数脱钩），本齿先咬。 */
    for (p = 0; p < (int)PAGE_COUNT; p++) {
        int nx = (int)key_page_next((page_id_t)p);
        TEST_ASSERT_TRUE_MESSAGE(nx >= 0 && nx < (int)PAGE_COUNT,
            rtp_fm("M2K2-A3 RED: key_page_next(%d) = %d 越出页集 [0, %d)——撤页后步进"
                   "基数与页数不一致（越界页索引会打穿 keybar/render 的防御列）",
                   p, nx, (int)PAGE_COUNT));
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 6 — M2K2-A4 / M2K2-E3 查字数据通路保留靶（structural，must-stay-green）
 * 撤的是常驻页位，不是查字能力：mcp_server.cc case 3（VRT_ROUTE_CHAR）路由块、
 * kidpal_char_page_show / kidpal_render_node_cache_char_data 两处调用点、
 * view=char 数值码、char_page/char_stroke 渲染面文件必须整体在世（M2-K3 改窗的
 * 前置生存面）。不为窗语义新增行为断言（越包红线）。
 * ------------------------------------------------------------------------ */
/* coverage: M2K2-A4 -- 查字通路保留靶：VRT_ROUTE_CHAR 数值码 == 3（契约 §1.3 ①，路由语义零改动）+ mcp case 3 区内四处消费点（route=char ack / 缺字段降级门 / char_page_show / cache_char_data）+ 文件级调用点在世 + view_router 消费面在世 */
/* coverage: M2K2-E3 -- 通路不顺手拆：char_page.{h,cc} + char_stroke.c 渲染面文件在世 + kidpal_render_node_cache_char / cache_char_data 循环支撑 API 保留（写侧死态，禁静默删除，K3 统一收） */
TEST_CASE("撤 P4 三页循环: 查字数据通路保留——mcp case 3+渲染调用点+数值码 (M2K2-A4)",
          "[retract_p4_three_page][M2K2-A4][M2K2-E3][structural]")
{
    char *region, hit[RTP_MAX_PATH];
    int n;

    RTP_REQUIRE_ROOT();

    /* tooth 1: view=char 路由数值码零改动（契约 §1.3 ① 锚; 撤页不得靠改数值码
     * 「顺手」将 char 挤出白名单）。 */
    TEST_ASSERT_TRUE_MESSAGE((int)VRT_ROUTE_CHAR == 3,
        rtp_fm("M2K2-A4 RED: VRT_ROUTE_CHAR = %d 期望 3——view 路由数值码被撤页改动"
               "（契约面数值码，改动走契约门，不归本包）", (int)VRT_ROUTE_CHAR));
    TEST_ASSERT_TRUE_MESSAGE(rtp_file_contains(RTP_VIEW_ROUTER_C, "return VRT_ROUTE_CHAR;"),
        "M2K2-A4 RED: view_router.c 不再路由到 VRT_ROUTE_CHAR——char view 白名单被撤");

    /* tooth 2: mcp_server.cc case 3 路由块内四处消费点整体在世（稳定锚切区）。 */
    region = rtp_region_between(RTP_MCP_CC, "case 3: { /* VRT_ROUTE_CHAR",
                                "case VRT_ROUTE_STORY_LIST:");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "M2K2-A4 RED(infra): mcp_server.cc 的 case 3（VRT_ROUTE_CHAR）路由块切取失败"
        "（块锚漂移——char 路由疑似已被搬动或删除）");
    TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_slice(region,
                                   "cJSON_AddStringToObject(ack, \"route\", \"char\")") >= 1,
        "M2K2-A4 RED: case 3 区内 ack route=char 丢失——char 路由的响应面被拆");
    TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_slice(region, "kidpal_char_data_validate(") >= 1,
        "M2K2-A4 RED: case 3 区内缺字段降级门丢失——闸 A 包内裁定的防御面被连带拆除");
    TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_slice(region, "kidpal_char_page_show(") >= 1,
        "M2K2-A4 RED: case 3 区内 kidpal_char_page_show 调用点丢失——服务端下发 char "
        "不再触发渲染（查字能力被撤页顺手拆掉 = M2K2-E3 越包红线）");
    TEST_ASSERT_TRUE_MESSAGE(
        rtp_count_in_slice(region, "kidpal_render_node_cache_char_data(") >= 1,
        "M2K2-A4 RED: case 3 区内 cache_char_data 调用点丢失——写侧死态应「保留不动、"
        "K3 统一收」（M2K2-E3 明令禁静默删除）");
    /* char 路由的语义零改动判别：块内不得出现新的页停靠动作（cycle_next 不是 char
     * 路由的消费面——撤页后更不该借 char 路由复活翻页停靠）。 */
    TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_slice(region, "kidpal_render_node_cycle_next(") == 0,
        "M2K2-A4 RED: case 3 区内出现 kidpal_render_node_cycle_next——查字路由被接回"
        "翻页循环（撤页 = 循环面撤出，不得从路由侧开后门）");
    free(region);

    /* tooth 3: 文件级调用点在世 + 消费链广度（>= 3 文件：char_page.{h,cc} 定义声明
     * 链 + mcp_server.cc 设备消费点）。 */
    TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_file(RTP_MCP_CC, "kidpal_char_page_show(") >= 1,
        "M2K2-A4 RED: mcp_server.cc 文件级 kidpal_char_page_show 调用点丢失");
    n = rtp_scan_count("firmware/main", "kidpal_char_page_show(", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        rtp_fm("M2K2-A4 RED: kidpal_char_page_show 仅 %d 文件引用（期望 >= 3: 定义 + "
               "声明 + mcp 消费点）——查字渲染通路在世面不足（首命中 %s）",
               n, hit[0] != '\0' ? hit : "(none)"));

    /* tooth 4: 渲染面与笔顺解析面文件保留（K3 改窗的地基; 本包零触碰）。 */
    TEST_ASSERT_TRUE_MESSAGE(rtp_exists(RTP_CHAR_PAGE_CC) && rtp_exists(RTP_CHAR_PAGE_H) &&
                                 rtp_exists(RTP_CHAR_STROKE_C),
        "M2K2-A4 RED: char_page.{h,cc} / char_stroke.c 渲染面文件缺失——撤页连带删了"
        "查字实现面（M2K2-E3 越包红线）");

    /* tooth 5: 循环支撑 API 保留守卫（E3 明令：写侧死态本包保留不动）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rtp_file_contains(RTP_RN_H, "void kidpal_render_node_cache_char(") &&
            rtp_file_contains(RTP_RN_H, "void kidpal_render_node_cache_char_data("),
        "M2K2-A4 RED(guard): kidpal_render_node.h 的 cache_char / cache_char_data 声明"
        "消失——M2K2-E3 规定两 API 随撤页成写侧死态但**本包保留不动**，删除归 M2-K3");
}

/* -------------------------------------------------------------------------- *
 * TEST 7 — boundary：本文件注册序守卫 + 需求/契约 canary + 扫描器自检
 *   M2K2-E1 结构半（本文件是否被 builder 接线进 HOST_ACCEPTANCE_SRCS）
 *   M2K2-E2 铁律 canary（requirements 三页锚在世 + 契约面在世 = 本包零需求/零契约
 *     触碰的机械证据；锚选在「撤页后仍然成立」的稳定句，不选对账轮会改写的状态注记）
 *   扫描器自检（结构齿负向必须能红，防假绿）
 * ------------------------------------------------------------------------ */
/* coverage: M2K2-E1 -- host 全量终跑：本文件注册进 HOST_ACCEPTANCE_SRCS（置于 smoke 之前）结构守卫 + suite 级判读锚（见文件尾，基线 39T/0F @ ca7ff81） */
/* coverage: M2K2-E2 -- 铁律 canary：requirements §2.4「3 页循环」/§2.2「中 3 页点 · 页点随之 4→3」/行 186「P4 查字页已撤销」三锚在世 + 契约面在世且 view 白名单含 char（本包零 spec / 零 contract 触碰） */
TEST_CASE("撤 P4 三页循环: 注册序守卫+需求契约canary+扫描器自检 (M2K2-E1/M2K2-E2)",
          "[retract_p4_three_page][M2K2-E1][M2K2-E2][boundary]")
{
    char *mk;
    char hit[RTP_MAX_PATH];
    int n;

    RTP_REQUIRE_ROOT();

    /* tooth 1: 本文件须接线进 HOST_ACCEPTANCE_SRCS 且排在 host_infra_smoke 之前
     * （smoke 末位 = 最先执行，包 0 boundary 齿注册序守卫）。RED until builder wires. */
    mk = rtp_read(RTP_CMAKELISTS, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "M2K2 RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *mine = strstr(mk, "retract_p4_three_page.acceptance.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(mine != NULL,
            "M2K2-E1 RED: retract_p4_three_page.acceptance.c 未接线进 "
            "HOST_ACCEPTANCE_SRCS——本包冻结验收未进 host 执行面（BUILDER DUTY: 见本文件"
            "头注 [6]，+1/-0，被测实现 key_gesture_parser.cc / keybar.c 已在册）");
        TEST_ASSERT_TRUE_MESSAGE(smoke != NULL,
            "M2K2 RED(infra): host_infra_smoke.acceptance.c 从 HOST_ACCEPTANCE_SRCS 丢失"
            "——包 0 守卫被移除");
        TEST_ASSERT_TRUE_MESSAGE(mine < smoke,
            "M2K2-E1 RED: retract_p4_three_page.acceptance.c 必须排在 host_infra_smoke."
            "acceptance.c 之前（smoke 末位 = 最先执行，boundary 齿注册序守卫）");
    }
    free(mk);

    /* tooth 2 (M2K2-E2 canary): 需求面三锚在世且未被修订——本包是「需求先行后的实现
     * 对账」，零 docs 面改动（TaskPackage docsSyncRequired=false）。 */
    TEST_ASSERT_TRUE_MESSAGE(rtp_exists(RTP_REQUIREMENTS),
        "M2K2-E2 RED(infra): docs/requirements.md 丢失——需求面被触碰（零修订红线）");
    TEST_ASSERT_TRUE_MESSAGE(rtp_file_contains(RTP_REQUIREMENTS, "页面结构（3 页循环）"),
        "M2K2-E2 RED: requirements §2.4「页面结构（3 页循环）」节标题锚丢失——需求面被"
        "修订（本包零 spec 触碰；发现冲突走 spec-lane 逃生门，不预先改需求）");
    TEST_ASSERT_TRUE_MESSAGE(rtp_file_contains(RTP_REQUIREMENTS, "中 3 页点") &&
                                 rtp_file_contains(RTP_REQUIREMENTS, "页点随之 4→3"),
        "M2K2-E2 RED: requirements §2.2 comp-keybar 的「中 3 页点 / 页点随之 4→3」锚"
        "丢失——页点 4→3 的需求权威被改动（M1-K2 deviation 的收回依据即此句）");
    TEST_ASSERT_TRUE_MESSAGE(rtp_file_contains(RTP_REQUIREMENTS, "P4 查字页已撤销"),
        "M2K2-E2 RED: requirements 行 186「P4 查字页已撤销」锚丢失——撤页需求事实被改");

    /* tooth 3 (M2K2-E2 canary): 契约面在世且 char view 在白名单内（A4 通路保留的
     * 契约侧证据；本包零契约触碰）。 */
    TEST_ASSERT_TRUE_MESSAGE(rtp_exists(RTP_CONTRACT),
        "M2K2 RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");
    TEST_ASSERT_TRUE_MESSAGE(
        rtp_file_contains(RTP_CONTRACT, "char/schedule/story_list/story_text/text/home/control"),
        "M2K2-E2 RED: 契约 display view 白名单里的 char 项丢失——view=char 通路被从契约"
        "面撤掉（越包; 查字窗归 M2-K3 且不改契约）");

    /* tooth 4: 扫描器自检（防假绿: 负向齿必须能红）。 */
    TEST_ASSERT_TRUE_MESSAGE(rtp_exists("firmware/main/key_gesture_parser.cc"),
        "M2K2 RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(rtp_exists("firmware/main/__rtp_no_such__.cc"),
        "M2K2 RED(infra): scanner reports a nonexistent file as present");
    n = rtp_scan_count("firmware/main", "__rtp_no_such_needle__", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "M2K2 RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    n = rtp_count_in_file(RTP_KEY_H, "PAGE_COUNT");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "M2K2 RED(infra): occurrence counter cannot count a known literal — 计数器失灵");
    {
        char *region = rtp_region_between("firmware/main/key_gesture_parser.cc",
                                          "page_id_t key_page_next(",
                                          "global_action_t key_route_global(");
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            "M2K2 RED(infra): region slicer cannot slice a known function — 切区器失灵");
        TEST_ASSERT_TRUE_MESSAGE(rtp_count_in_slice(region, "PAGE_COUNT") >= 2,
            "M2K2 RED(infra): region slicer lost a known literal — 切区器计数失灵");
        free(region);
    }
    /* 自检：恒等邻接判别器本身要能红（把窗口收到 1 字符必不成立），否则 tooth 6 的
     * 形状无关齿是空转齿。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rtp_maps_within("case KP_PAGE_SCHEDULE: page = PAGE_P2;", "KP_PAGE_SCHEDULE",
                        "PAGE_P2", 40) == 1 &&
            rtp_maps_within("case KP_PAGE_SCHEDULE: page = PAGE_P2;", "KP_PAGE_SCHEDULE",
                            "PAGE_P2", 1) == 0,
        "M2K2 RED(infra): 恒等邻接判别器失灵（正向不成立或零窗口仍成立）");
}

/* ---------------------------------------------------------------------------
 * 终跑判读（suite 级判读说明, review-packet 呈 supervisor）:
 *   判据 = 相对有效基线零新增红 + 本包新齿全绿。
 *   有效基线 = launch 侧 2026-09-18 @ HEAD ca7ff81 现跑 firmware host 面全量：
 *   **39 Tests / 0 Failures / 0 Ignored、exit 0**（锚行 =
 *   delivery/workstreams/m2-char-window/progress.md 2026-09-18 段；server 面零差异，
 *   继续引用同文件 be3cf9f 的 37 passed 行）。既有套件零红 = 本包「零新增红」判据的
 *   干净起点（在册红 = 零，判读纪律 = 新增红才是本包责任）。
 *   本包义务红（设计态, builder 落地后转绿, 不计入「新增红」违约）：
 *     - 本文件 test 1 / test 2 / test 3 / test 4 / test 5（撤页缺失行为红，逐齿见下
 *       「RED baseline 判读口径」）;
 *     - 本文件 test 7 tooth 1（注册接线守卫，builder 首步 +1/-0 后转绿）;
 *     - key_layer_dispatch.acceptance.c test 4 tooth 2（KD-E1 撤页守卫语义翻正：由
 *       「PAGE_P4 零触碰」翻为「PAGE_P4 已撤 + PAGE_COUNT == 3」，对未撤页现状为
 *       设计态红——acceptance-author 留痕修订，见该文件 REVISION LOG 2026-09-18）;
 *     - keybar_three_slot.acceptance.c test 1 tooth 1（KKB-E1 由字面 PAGE_COUNT == 4
 *       改为同源判别）+ test 2 第 4 页槽位齿（由「存在性兜底」改为「page == PAGE_COUNT
 *       越界防御 -1」）——两处留痕修订**撤页前后皆绿、不新增红**（2026-09-18
 *       standalone 实测 5 Tests / 0 Failures，见该文件 REVISION LOG 同批条目）；本包
 *       设计态红全部由本文件与上列 KD-E1 翻正齿承载。
 *   MUST-STAY-GREEN（守卫齿，现状即绿、绿才是本包做对；同 test 内的设计态红齿不在此列）:
 *     test 1 tooth 1/2（前两跳）+ tooth 5（基数与页数同源回环）+ tooth 6（非法页防御）;
 *     test 2 tooth 1（`% KP_PAGE_COUNT` 基数）+ tooth 2（循环区零 CHAR 旁路）+
 *       tooth 4（三页渲染 case 全在世）;
 *     test 3 tooth 6 的映射邻接判别（若还写某页必须映到对应页点）与取数链守卫
 *       （仍读 kidpal_render_node_get_page）+ tooth 7 的「三页 + COUNT 标识齐全」;
 *     test 4 tooth 1（三页 total 同源 / visible / active）+ tooth 2（覆盖层隐藏不抹
 *       状态）+ tooth 4（P3 保留页槽文案逐字）;
 *     test 5 tooth 1/2（键路由矩阵与分层派发零偏移）+ tooth 4（步进永不越界）;
 *     test 6 全部（A4/E3 通路保留靶）; test 7 tooth 2/3/4（需求契约 canary + 扫描器
 *       自检）。既有全套件除上列留痕齿外零修改、零新增红。
 *   builder 接线后期望终跑量级 = 46 Tests（39 + 本文件 7）/ 0 Failures（HOST_TEST_
 *   CONVENTIONS [C4.4]：测试数是随套件增长的派生量，不当硬编码判据，真判据 = OK +
 *   EXIT=0 + 全 PASS）。
 *
 * RED baseline 判读口径（freeze 相位, acceptance-author 2026-09-18 实跑）:
 *   通道① 编译通道（DEV-GOVERNANCE「验收测试纪律」§4 C 类冻结验收实编）:
 *     gcc -std=c11 -Wall -Wextra -fsyntax-only -I firmware/main/test/acceptance/
 *         red-baseline firmware/main/test/retract_p4_three_page.acceptance.c
 *     以及完整链接编译（含 g++ -c key_gesture_parser.cc 与 keybar.c +
 *     kidpal_voice_states.c）—— **两 TU 日志皆空 = 编译通道 error 0、warning 0**。
 *     两处在世套件的留痕修订同批各过一遍（key_layer_dispatch.acceptance.c /
 *     keybar_three_slot.acceptance.c 实编，见各自 REVISION LOG 2026-09-18 条目）。
 *   通道② 执行通道（§3 冻结即冒烟）= 同一份冻结件 + 已在册真实现源经边界内脚手架
 *   （firmware/main/test/acceptance/red-baseline 下的 unity.h stub + runner.c，非冻结
 *   件）编成原生 Linux ELF 当场执行，命令见文件头 Run protocol「Freeze-phase」段。
 *   实测（2026-09-18，实现面 = ca7ff81 态）: **7 Tests / 6 Failures，ELF 退出码 = 6**
 *   （脚手架契约 return failures；从仓根与 firmware/main/test/host_app 两个 cwd 各跑
 *   一次结果一致 = root 定位器线束冒烟过）。经 `wsl -- bash -c "... echo $?"` 包装时
 *   退出码恒 0，是已登记的 wsl.exe 参数重组伪影（red-baseline-summary.char-stroke.md
 *   同记），故退出码取证用 `wsl -- <elf>` 直调形态。
 *   逐齿红因（每条都是「本包缺失的真实行为」，无一条环境/依赖/测试自身错误）:
 *     test 1 M2K2-A1 RED: key_page_next(P3) = 3 期望回绕 PAGE_P1(0)（现循环含第 4 页）;
 *     test 2 M2K2-A1 RED: 页面渲染权威区内仍有 2 处 KP_PAGE_CHAR（CHAR 仍是停靠页）;
 *     test 3 M2K2-A2 RED: 页枚举 = (0,1,2,COUNT=4) 期望 (0,1,2,3);
 *     test 4 M2K2-A2 RED: kidpal_keybar_dots(page=3) rc=0 期望 -1（第 4 页位仍可渲染
 *       页点；同 test 未显示的后续齿 slots(page=3)/keybar.c PAGE_P4 字面同源必红）;
 *     test 5 M2K2-A3 RED: 连翻三步页序 = (0,1,2 -> 3) 期望回到 P1（复合环）;
 *     test 6 M2K2-A4 **PASS**（通路保留守卫，设计态即须绿，红判据靠 mutation 反转）;
 *     test 7 M2K2-E1 RED: 本文件未接线进 HOST_ACCEPTANCE_SRCS（builder 首步 +1/-0）。
 *   其余 must-stay 齿（test 1 tooth 1/2/5/6、test 2 tooth 1/2、test 5 tooth 1/2/4、
 *   test 7 tooth 2/3/4）在本轮未触红即通过（stub 首红即止，故其绿态由 builder 接线后
 *   的 host runner 终跑复核，不代跑）。
 *   cwd 口径: 结构齿以真实工作树扫描，root 自手持 cwd 定位（仓根 / firmware/main/
 *   test/host_app 均可）；凡引用 delivery 目录下证据路径一律按相对路径 + 仓根 cwd 前提。
 * ---------------------------------------------------------------------------
 * M2K2-A5 真机翻页确认（deferred, 本文件不承载）:
 *   BOOT 翻 3 页循环手感 + 中槽页点 3 点的真机目击 = deferred 归 M2-K3 设备轮（单
 *   真机不两轮撞车；TaskPackage acceptance[] 第 5 条 kind=structural + note 已声明）。
 *   K3 设备轮的取证口径（本包交接）：① 首页→课表→记录→首页 三跳回绕（第四步回首页）；
 *   ② 中槽页点恰 3 点、当前页实心放大、覆盖层内整槽隐藏；③ 查字服务端下发仍能渲染
 *   （本包保留的通路真机复证，窗语义改造本身归 K3）；④ 页点数量与页集同源，不得出现
 *   「点 3 个、能翻 4 页」或反之。证据落 delivery/changes/dev/260917-m2-k2-retract-p4-
 *   three-page/（真机轮日志/照片由 K3 设备轮一并覆盖登记）。
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-18  initial freeze (acceptance-author, TaskPackage
 *               260917-m2-k2-retract-p4-three-page). 同批留痕修订两处在世冻结齿：
 *               key_layer_dispatch.acceptance.c KD-E1 撤页守卫（语义翻正）、
 *               keybar_three_slot.acceptance.c KKB-E1 infra 齿 + KB-A3 第 4 页槽齿
 *               （同源化）。归档件 firmware/main/test/acceptance/archive 目录下的 .c
 *               件不编入 runner、不改（TaskPackage M2K2-E2 已如实登记现实）。
 * --------------------------------------------------------------------------- */
