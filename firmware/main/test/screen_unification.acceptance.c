/*
 * Frozen acceptance tests — TaskPackage 260911-screen-unification（PKG-2 归一屏）
 * (subsystem: firmware; workstream 260911-display-interaction-sovereignty 核心包;
 *  mergeOrder 19; rigor=full; riskTier=high;
 *  gateProfile: host-teeth / state-machine / device-round / frozen-acceptance-revision;
 *  sourceType=proposal; 设计权威 = PROPOSAL FROZEN v1.0（OQ1=纯B / OQ3=c 已裁）+
 *  review.md F6/F7/F8).
 *
 * Story: 把小智聊天控件（状态栏/表情/气泡，SetupUI 建立面）迁为 KidPal 屏
 * (s_kidpal_scr) 的常驻 layer；s_page_pinned / s_chat_overlay / s_current_page
 * 三标志 + 1s tick 轮询对账整体退役；char 独立屏并入 s_kidpal_scr 体系（review F7）；
 * alert/reminder 前台优先级进 layer 优先级表（review F8）；回答驻留随归一屏自然成立
 * （OQ3=c：清除时机 = 下次交互，由 layer 管理顺带定）；layer 管理即前台 owner 单一
 * 权威（仲裁器显式化——方向 A 状态机内核收编于此，不另建状态机对象）。
 *
 * Nature: host-runnable BEHAVIORAL（新纯逻辑 layer 管理器 frozen offline contract，
 * weak-symbol 哨兵先例）+ STRUCTURAL（树级结构齿，符号级零残留断言，参照
 * hardware_validation.acceptance.c 的「运行时以文本读工作树断言」Run protocol 模式）。
 * 红基线红在真实缺失行为（frozen contract 未实现 weak 哨兵 / 符号级残留事实存在），
 * 绝不红在链接/编译错误。
 *
 * ===========================================================================
 * FROZEN OFFLINE CONTRACT（builder MUST implement，纯逻辑，host 可编译，无
 * chip/FreeRTOS/ESP-component/cJSON 依赖；C linkage——若落在 .cc 需 extern "C"；
 * 建议落点：firmware/main/display/kidpal_layer.(c|h)。类型/数值码/语义冻结于本文件）。
 *
 * 本模块 = 前台 layer 栈管理单一权威（仲裁器内核）。它是两块纯逻辑的合并：
 *   (1) 前台参与者优先级栈（本地浏览页 / 服务端推送页 / 语音轮 / 提醒弹窗 / 配网）；
 *   (2) 聊天 layer 常驻性 + 驻留清除时机（OQ3=c）+ 聊天内容落点。
 * 不新建独立状态机对象（PROPOSAL §4）：layer 栈本身即前台 owner 的单一权威。
 *
 *   --- 类型形状（冻结，kidpal_layer.h 须逐字一致） ---
 *   typedef struct {
 *       int layers[KIDPAL_LAYER_STACK_MAX];   // 前台参与者栈（实际内部序实现自由）
 *       int n;                                // 当前层数
 *   } kidpal_layer_stack_t;
 *
 *   --- 常量（冻结数值码，不得偏移） ---
 *   KIDPAL_LAYER_STACK_MAX     8
 *   KIDPAL_LAYER_UNIMPLEMENTED (-2)   // weak 哨兵 — 永不是合法返回值
 *   KIDPAL_LAYER_NONE          0       // 空栈前台 owner
 *   KIDPAL_LAYER_CHAT          1       // 常驻聊天 layer（状态栏/表情/气泡）——非前台栈成员
 *   KIDPAL_LAYER_LOCAL_PAGE    2       // 本地浏览页（首页 P1 / 手动翻页）
 *   KIDPAL_LAYER_VOICE         3       // 语音轮（connecting/listening/speaking）
 *   KIDPAL_LAYER_SERVER_PAGE   4       // 服务端推送页（课表/故事/查字回答页）
 *   KIDPAL_LAYER_REMINDER      5       // 提醒弹窗（alert/reminder 急性通知）
 *   KIDPAL_LAYER_PROVISION     6       // 配网（config mode）
 *
 *   // 前台优先级 = 数值越大越靠前：配网 > 提醒 > 服务端推送页 > 语音轮 > 本地浏览页。
 *
 *   KIDPAL_LAYER_EVT_ANSWER_END  0   // 回答结束（ssml/display 流结束）——不清除，驻留
 *   KIDPAL_LAYER_EVT_PAGE_SWITCH 1   // 页面切换（翻页/手动导航）——不清聊天
 *   KIDPAL_LAYER_EVT_NEW_VOICE   2   // 新语音轮开始——清除聊天内容
 *   KIDPAL_LAYER_EVT_KEY_PRESS   3   // 任意按键——清除聊天内容
 *
 *   --- 函数契约（返回 int；-2 = 未实现哨兵，-1 = 防御/无效） ---
 *   int kidpal_layer_push(kidpal_layer_stack_t *st, int layer);
 *       前台参与者进栈。layer 合法域 = LOCAL_PAGE..PROVISION（2..6）；CHAT(1) 与
 *       NONE(0) 不是前台栈成员 -> -1。按优先级插入（top 恒 = 最高优先级在场层）；
 *       幂等——重复 push 同一 layer 保持单条目、不再叠加、返回 0。成功 -> 0；
 *       NULL / 非法 layer -> -1。
 *
 *   int kidpal_layer_pop(kidpal_layer_stack_t *st, int layer);
 *       出栈指定 layer（任意位置），其余保持相对序。在场并移除 -> 0；
 *       NULL / 非法 layer / 不在场 -> -1。
 *
 *   int kidpal_layer_top(const kidpal_layer_stack_t *st);
 *       前台 owner = 在场最高优先级 layer。空栈 -> KIDPAL_LAYER_NONE(0)；
 *       NULL -> -1。
 *
 *   int kidpal_layer_has(const kidpal_layer_stack_t *st, int layer);
 *       在场 -> 1，不在场 -> 0；NULL / 非法 layer -> -1。
 *
 *   int kidpal_layer_chat_target(void);
 *       s_chat_scr 退役后聊天内容（状态栏/表情/气泡）的落点。冻结：恒返回
 *       KIDPAL_LAYER_CHAT(1)——聊天内容落 KidPal 屏常驻聊天 layer，非独立聊天屏。
 *       回答后屏幕落点确定（非随机）由「恒 = 聊天 layer」钉死。
 *
 *   int kidpal_layer_chat_clear_event(int event);
 *       驻留清除时机判定（OQ3=c）。返回 1 = 清除聊天内容，0 = 保留驻留。
 *       冻结：ANSWER_END(0)->0、PAGE_SWITCH(1)->0、NEW_VOICE(2)->1、KEY_PRESS(3)->1；
 *       未知/负值 event -> 0（保守驻留）。
 *
 * While the implementation is absent, __attribute__((weak)) defaults below return
 * the sentinel KIDPAL_LAYER_UNIMPLEMENTED (-2): every behavioral test asserts
 * NOT-sentinel first, so the red baseline reads "frozen contract not implemented"
 * — a real missing-behaviour red, not a link error.
 * ===========================================================================
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef / exitCriteria -> test, kind):
 *   coverage: SU-A1  structural -> test s1 (聊天 layer 化: 三标志/tick/s_chat_scr
 *               符号级零残留 + 聊天内容落点经 layer 模块)
 *   coverage: SU-A1  behavioral -> test b2 (聊天内容落点恒 = 常驻聊天 layer)  +
 *               test s2 (layer 模块在树 + 编入设备构建 + host 接线 + 生产消费者 wiring)
 *   coverage: SU-A2  structural -> test s1 (s_page_pinned/s_chat_overlay/s_current_page
 *               符号级零残留 + tick 对账轮询屏切换路径移除)
 *   coverage: SU-A2  behavioral -> test b1 (前台归属 = layer 栈优先级表单一权威:
 *               进出序与 push 顺序无关)
 *   coverage: SU-A3  structural -> test s3 (char 页已入 s_kidpal_scr layer 体系、无
 *               独立屏: display/ 单屏不变式)
 *   coverage: SU-A4  behavioral -> test b1 (alert/reminder 优先级进 layer 优先级表:
 *               提醒弹窗 > 语音轮, 提醒恰在语音轮到点弹窗优先级正确)
 *   coverage: SU-A5  behavioral -> test b2 (驻留清除时机: 回答结束/页面切换不清,
 *               新语音轮/按键清; 无新驻留页/定时器) + test b3 (回答后服务端页驻留前台)
 *   coverage: SU-A7  behavioral -> test b3 (两键行为显示归属面: voice/home/翻页导航
 *               前台归属经 layer 管理可到达正确 target——KEY 长按=回首页 = LOCAL_PAGE
 *               可到; KEY 路由语义 owner = PKG-1, 本包只钉显示归属面)
 *   coverage: SU-A6  登记（不落断言）: 全量既有冻结齿复跑 = 矩阵回归动作，非新齿；
 *               渲染路径重构致齿红仅本 acceptance-author 留痕修订（REVISION LOG）。
 *   coverage: SU-A8  登记（不落断言）: 真机 human 持板并发场景，证据落
 *               delivery/changes/dev/260911-screen-unification/，非 host 齿。
 *   coverage: SU-A9  登记（不落断言）: host runner 终跑全绿 exit 0，过程性结论非新齿。
 *   coverage: SU-E3  boundary/security -> test s4 (冻结面零越界 canary: 契约/spec
 *               在世 + 设备不直连 openclaw/18789 + 扫描器自检防假绿)
 *   coverage: SU-E2  观感边界（不落断言）: 字体子集/换肤/排版归批次③
 *               260911-render-pages（mergeOrder 21 本包后），本文件零视觉断言。
 * ---------------------------------------------------------------------------
 * RED baseline（现状 2026-09-11 acceptance-author 复核，机制尚未建）：
 *   - firmware/main/display/kidpal_render_node.cc L58/L63/L75 三标志在位
 *     (s_chat_overlay / s_page_pinned / s_current_page)、L88 RN_TICK_MS=1000 1s tick
 *     对账活跃（L181/L193 路径）、L54 s_chat_scr 独立屏在位；
 *   - char 独立屏在位：char_page.cc L418 `g_page.scr = lv_obj_create(NULL)` +
 *     L420 `lv_screen_load(g_page.scr)`；
 *   - firmware/main 无任何 kidpal_layer_* 模块（frozen contract 未实现）。
 *   -> test s1 三标志/tick 零残留齿与 test s3 单屏不变式齿在基线上必红（机制未建）、
 *      test b1/b2/b3 行为齿红在 weak 哨兵、test s2 结构齿红在模块缺树/未接线；
 *      test s4 boundary/canary 守卫为 GREEN（must-stay-green）。
 *
 * 另注（上游在册红口径，非本包责任，如实记录不修）：host ELF 基线 @ main d087fb7 =
 * 104 Tests / 3 Failures——WSA-A6:830 / WSA-A4:663（WSA 两证据齿本地-only 从未入仓）、
 * P2-A1:571（KSP-A1 first-match 歧义, owner=批次③渲染批）。本包承接的 host-teeth 面
 * 不包含这三条；builder/verifier 不得把它们标成本包红、不得尝试修。
 * ---------------------------------------------------------------------------
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.2]/[C4.4]):
 *   0) builder wiring（acceptance-author 被 enforce-scope 拦在 CMakeLists 外，接线
 *      属 builder；WSA/char_stroke/art 同款先例）: 在
 *      firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS 加
 *        "../../screen_unification.acceptance.c"（须排在 host_infra_smoke.acceptance.c
 *        之前——smoke 末位 = boundary 齿注册序守卫）
 *        与（落地后）"../../../display/kidpal_layer.c"（强符号覆盖弱哨兵）
 *      并加 include dir:
 *        target_include_directories(${COMPONENT_LIB} PRIVATE "../../../display")
 *      （本文件经 __has_include("kidpal_layer.h") 在实现头落地后自动切到真实契约声明）。
 *   1) wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf
 *   每一 TEST_CASE 必须执行并 PASS，runner 必须 exit 0。
 *   （真机观感/并发 0 复位 0 崩溃 = SU-A8 human 持板, 非本 host 文件。）
 *
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-11  initial freeze (acceptance-author, TaskPackage 260911-screen-unification).
 *   2026-09-11  前台优先级语义修订（SU-RULING-01 方案A；actor="human-deputy", 非
 *               human 亲裁）——SERVER_PAGE/VOICE 数值码互换（3↔4）：回答期间服务端
 *               推送页前台可见、语音不拦内容页，对齐 PROPOSAL §0 L26-27「问完先看到/
 *               同步看到回答与相关页面」+ §5 L152-153「display 页上屏不晚于 TTS 首段
 *               后 0.5s、回答结束后屏幕落点确定（非随机）」。tighten toward frozen
 *               design authority（方案A, deputy 代裁），断言语义不放松。b1/b3 前台断
 *               言、数值码 ordering 守卫、pop 回落链按新码序重钉；b3 尾部「手动翻页/
 *               回首页」的 LOCAL_PAGE 前台断言修正为与 frozen 优先级表（LOCAL_PAGE
 *               最低）自洽的「push 基层 + pop 服务端页」达前形式（原「push(LOCAL_PAGE)
 *               即前台」在 SERVER_PAGE 在场时与优先级表矛盾 = 测试错误红, 非语义放松）。
 *               裁断出处：
 *               delivery/changes/arch/260908-gov-review-human-deputy-shadow/deputy-shadow.md#SU-RULING-01。
 *   2026-09-11  单物理屏收紧（SU-RULING-02 方案1；actor="human-deputy", 代裁,
 *               deviation=false, 禁止记成 human 亲裁, canon C-14）——真归一为单物理屏、
 *               物理 reparent 留 PKG-2 包内随包交付。tighten: 补钉「除 s_kidpal_scr 外
 *               不得再有第二个前台屏载体」不变式, 封死 builder 首轮两处符号级零残留
 *               齿被绕过路径（①捕获改名——旧 s_chat_scr 改名为 s_voice_ui_scr, s1 只扫
 *               旧名漏判; ②非创建点——s_voice_ui_scr 靠 lv_screen_active() 捕获而来、
 *               非 lv_obj_create(NULL) 创建点, s3 只数创建点漏判）。新增断言:
 *               (a) s1 补钉 s_voice_ui_scr 符号级零残留（封死改名）; (b) s3 补钉
 *               display/ 下每次 lv_screen_load( 实参恒 = s_kidpal_scr（按出现次数相等
 *               判定, 封死非创建点捕获屏被切前）。据代码实际把「`= lv_screen_active()`
 *               赋值捕获归零」精确化为「第二屏载体加载归零」, 避免误伤 char/schedule/
 *               story/reminder 页渲染用 lv_screen_active() 取活动屏绘制句柄的合法用法。
 *               断言语义不放松（纯加法, s3 既有两齿原样不动）。裁断出处:
 *               delivery/changes/arch/260908-gov-review-human-deputy-shadow/deputy-shadow.md#SU-RULING-02。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>

#define SUNU_MAX_PATH 1024
#define SUNU_MAX_READ (8u * 1024u * 1024u) /* 拒绝 slurp 大于 8 MB 的文件 */

/* -------------------------------------------------------------------------- *
 * Frozen contract probes: use the real header once the builder lands it,
 * frozen fallback declarations before that (single source of truth switch).
 * ------------------------------------------------------------------------ */
#if defined(__has_include)
#if __has_include("kidpal_layer.h")
#include "kidpal_layer.h"
#define SUNU_HAS_REAL_HEADER 1
#endif
#endif

#ifndef SUNU_HAS_REAL_HEADER
/* FROZEN fallback declarations — kidpal_layer.h must carry these VERBATIM. */
#define KIDPAL_LAYER_STACK_MAX    8
#define KIDPAL_LAYER_UNIMPLEMENTED (-2)
#define KIDPAL_LAYER_NONE         0
#define KIDPAL_LAYER_CHAT         1
#define KIDPAL_LAYER_LOCAL_PAGE   2
#define KIDPAL_LAYER_VOICE        3
#define KIDPAL_LAYER_SERVER_PAGE  4
#define KIDPAL_LAYER_REMINDER     5
#define KIDPAL_LAYER_PROVISION    6

#define KIDPAL_LAYER_EVT_ANSWER_END  0
#define KIDPAL_LAYER_EVT_PAGE_SWITCH 1
#define KIDPAL_LAYER_EVT_NEW_VOICE   2
#define KIDPAL_LAYER_EVT_KEY_PRESS   3

typedef struct {
    int layers[KIDPAL_LAYER_STACK_MAX];
    int n;
} kidpal_layer_stack_t;

int kidpal_layer_push(kidpal_layer_stack_t *st, int layer);
int kidpal_layer_pop(kidpal_layer_stack_t *st, int layer);
int kidpal_layer_top(const kidpal_layer_stack_t *st);
int kidpal_layer_has(const kidpal_layer_stack_t *st, int layer);
int kidpal_layer_chat_target(void);
int kidpal_layer_chat_clear_event(int event);
#endif /* !SUNU_HAS_REAL_HEADER */

#define SUNU_UNIMPLEMENTED (-2) /* weak 哨兵别名 — 永不是合法返回值 */

/* -------------------------------------------------------------------------- *
 * weak defaults — builder 的 kidpal_layer.c 强符号覆盖它们。
 * ------------------------------------------------------------------------ */
__attribute__((weak)) int kidpal_layer_push(kidpal_layer_stack_t *st, int layer)
{
    (void)st; (void)layer;
    return SUNU_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_layer_pop(kidpal_layer_stack_t *st, int layer)
{
    (void)st; (void)layer;
    return SUNU_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_layer_top(const kidpal_layer_stack_t *st)
{
    (void)st;
    return SUNU_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_layer_has(const kidpal_layer_stack_t *st, int layer)
{
    (void)st; (void)layer;
    return SUNU_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_layer_chat_target(void)
{
    return SUNU_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_layer_chat_clear_event(int event)
{
    (void)event;
    return SUNU_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（host 侧结构齿），char_stroke/hardware_validation/
 * ui_art_integration 先例。所有扫描读真实工作树（自 runner cwd 逐级上探仓根）。
 * 排除目录：test（本冻结文件所在，其注释合法提及冻结 needle）/ host_app / build /
 * .git / assets（assets 为二进制资产，非实现面）。
 * ------------------------------------------------------------------------ */

static char sunu_root[SUNU_MAX_PATH];
static int  sunu_root_ready;

static int sunu_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int sunu_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int sunu_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int sunu_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[SUNU_MAX_PATH];

    if (sunu_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (sunu_is_regular(probe)) {
            snprintf(sunu_root, sizeof(sunu_root), "%s", cands[i]);
            sunu_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int sunu_rel(char *out, size_t cap, const char *rel)
{
    if (!sunu_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", sunu_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", sunu_root, rel);
    return 1;
}

static int sunu_exists(const char *rel)
{
    char p[SUNU_MAX_PATH];
    return sunu_rel(p, sizeof(p), rel) && sunu_is_regular(p);
}

static char *sunu_read(const char *rel, size_t *out_len)
{
    char p[SUNU_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!sunu_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > SUNU_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* logic guard: host OOM 必须响亮失败, 不得读成 "red" */
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

static int sunu_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* NULL needle 是测试 bug, 不是行为红 */
    return strstr(hay, needle) != NULL;
}

static int sunu_file_contains(const char *rel, const char *needle)
{
    char *buf = sunu_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = sunu_contains(buf, needle);
    free(buf);
    return r;
}

/* 统计 rel_dir 下内容同时含 needle1（必需）与 needle2（可选）的文件数，first_hit
 * 记录首个命中（仓相对路径）。 */
static int sunu_count_files_with_needles(const char *abs_dir, const char *rel_prefix,
                                         const char *needle1, const char *needle2,
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
        char abs_next[SUNU_MAX_PATH];
        char rel_next[SUNU_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            sunu_is_skipped_dir(e->d_name)) {
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
            count += sunu_count_files_with_needles(abs_next, rel_next, needle1, needle2,
                                                   first_hit, cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > SUNU_MAX_READ) {
                continue;
            }
            FILE *f = fopen(abs_next, "rb");
            if (f == NULL) {
                continue;
            }
            char *buf = (char *)malloc((size_t)st.st_size + 1);
            assert(buf != NULL); /* logic guard */
            if (fread(buf, 1, (size_t)st.st_size, f) == (size_t)st.st_size) {
                buf[st.st_size] = '\0';
                int m = sunu_contains(buf, needle1) &&
                        (needle2 == NULL || sunu_contains(buf, needle2));
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

static int sunu_scan_count(const char *rel_dir, const char *needle,
                           char *first_hit, size_t cap)
{
    char abs[SUNU_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!sunu_rel(abs, sizeof(abs), rel_dir) || !sunu_is_dir(abs)) {
        return 0;
    }
    return sunu_count_files_with_needles(abs, rel_dir, needle, NULL, first_hit, cap, 0);
}

static int sunu_scan_count_both(const char *rel_dir, const char *needle1, const char *needle2,
                                char *first_hit, size_t cap)
{
    char abs[SUNU_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!sunu_rel(abs, sizeof(abs), rel_dir) || !sunu_is_dir(abs)) {
        return 0;
    }
    return sunu_count_files_with_needles(abs, rel_dir, needle1, needle2, first_hit, cap, 0);
}

/* 统计 rel_dir 子树内所有常规文件内容中 needle 的累计出现次数（逐文件 strstr 计次，
 * 与 sunu_scan_count 同一排除目录）。用于「每次 lv_screen_load( 的实参必须恒为
 * s_kidpal_scr」这类必须按次数而非按文件判定（同文件既有合法 load 又有违规 load 时,
 * 仅按文件计数会漏判）的收紧齿。 */
static int sunu_count_occurrences(const char *abs_dir, const char *needle, int depth)
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
    assert(needle != NULL); /* NULL needle 是测试 bug, 不是行为红 */
    while ((e = readdir(d)) != NULL) {
        char abs_next[SUNU_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            sunu_is_skipped_dir(e->d_name)) {
            continue;
        }
        snprintf(abs_next, sizeof(abs_next), "%s/%s", abs_dir, e->d_name);
        if (stat(abs_next, &st) != 0) {
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            count += sunu_count_occurrences(abs_next, needle, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > SUNU_MAX_READ) {
                continue;
            }
            FILE *f = fopen(abs_next, "rb");
            if (f == NULL) {
                continue;
            }
            char *buf = (char *)malloc((size_t)st.st_size + 1);
            assert(buf != NULL); /* logic guard */
            if (fread(buf, 1, (size_t)st.st_size, f) == (size_t)st.st_size) {
                buf[st.st_size] = '\0';
                const char *p = buf;
                while ((p = strstr(p, needle)) != NULL) {
                    count++;
                    p++; /* 前进一格, 不吞重叠; 本 tooth 两 needle 无重叠歧义 */
                }
            }
            free(buf);
            fclose(f);
        }
    }
    closedir(d);
    return count;
}

static int sunu_scan_occurrences(const char *rel_dir, const char *needle)
{
    char abs[SUNU_MAX_PATH];

    if (!sunu_rel(abs, sizeof(abs), rel_dir) || !sunu_is_dir(abs)) {
        return 0;
    }
    return sunu_count_occurrences(abs, needle, 0);
}

#define SUNU_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(sunu_locate_root(), \
    "SUNU RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

static void sunu_require_impl(int value, const char *what)
{
    if (value == SUNU_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "SUNU RED: frozen offline contract NOT implemented — %s "
                 "(builder must land the pure-logic layer manager per the frozen "
                 "contract in firmware/main/test/screen_unification.acceptance.c header "
                 "and wire display/kidpal_layer.c into HOST_ACCEPTANCE_SRCS)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* printf 风格断言包装：idf unity 的 *_MESSAGE 宏只收单串消息，本文件若干失败诊断
 * 带动态字段（文件计数/首现路径）——先格式化进静态缓冲再走原宏（原宏在条件为真时
 * 不求值消息参数，静态缓冲单次使用安全; ui_art_integration 同款先例）。 */
#include <stdarg.h>
static char sunu_fm_buf[1024];
static const char *sunu_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *sunu_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(sunu_fm_buf, sizeof(sunu_fm_buf), fmt, ap);
    va_end(ap);
    return sunu_fm_buf;
}
#define SUNU_ASSERT_TRUE_FM(cond, ...)       TEST_ASSERT_TRUE_MESSAGE((cond), sunu_fm(__VA_ARGS__))
#define SUNU_ASSERT_FALSE_FM(cond, ...)      TEST_ASSERT_FALSE_MESSAGE((cond), sunu_fm(__VA_ARGS__))
#define SUNU_ASSERT_NOT_NULL_FM(p, ...)      TEST_ASSERT_NOT_NULL_MESSAGE((p), sunu_fm(__VA_ARGS__))
#define SUNU_ASSERT_NULL_FM(p, ...)          TEST_ASSERT_NULL_MESSAGE((p), sunu_fm(__VA_ARGS__))

/* 初始化一个空栈（栈结构布局冻结，零值即空栈合法态）。 */
static void sunu_stack_init(kidpal_layer_stack_t *st)
{
    assert(st != NULL); /* logic guard: NULL 输出是测试 bug */
    memset(st, 0, sizeof(*st));
}

/* -------------------------------------------------------------------------- *
 * test b1 — SU-A2 / SU-A4 (behavioral, frozen contract):
 * layer 前台优先级表进出序 + 单一权威。前台优先级 = 数值越大越靠前：
 * 配网(6) > 提醒(5) > 服务端页(4) > 语音(3) > 本地页(2)；聊天(1) 常驻非栈成员。
 * 钉：push/pop/top 进出的确定性、push 顺序无关（单一权威, 取代三散标志）、
 * 幂等去重、空栈 top = NONE、提醒恰在语音轮到点 → 提醒为前台（F8 分支）。
 * RED today: weak sentinel (-2)。
 * ------------------------------------------------------------------------ */
TEST_CASE("screen unification: layer 前台优先级表进出序 + 单一权威 (SU-A2/SU-A4)",
          "[screen_unification][SU-A2][SU-A4][behavioral]")
{
    kidpal_layer_stack_t st;
    int r;

    /* 冻结数值码 ordering 逻辑守卫（图层优先级升序） */
    assert(KIDPAL_LAYER_NONE == 0 && KIDPAL_LAYER_CHAT == 1 &&
           KIDPAL_LAYER_LOCAL_PAGE == 2 && KIDPAL_LAYER_VOICE == 3 &&
           KIDPAL_LAYER_SERVER_PAGE == 4 && KIDPAL_LAYER_REMINDER == 5 &&
           KIDPAL_LAYER_PROVISION == 6);

    sunu_stack_init(&st);

    /* 空栈 -> NONE */
    r = kidpal_layer_top(&st);
    sunu_require_impl(r, "kidpal_layer_top unimplemented");
    SUNU_ASSERT_TRUE_FM(r == KIDPAL_LAYER_NONE,
        "SU-A2/A4 RED: empty stack top() must be NONE(0), got %d — 空栈前台 owner 漂移", r);

    /* 单一层进栈 -> 该层为前台 */
    r = kidpal_layer_push(&st, KIDPAL_LAYER_LOCAL_PAGE);
    sunu_require_impl(r, "kidpal_layer_push unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && kidpal_layer_top(&st) == KIDPAL_LAYER_LOCAL_PAGE,
        "SU-A2 RED: push(LOCAL_PAGE) then top() != LOCAL_PAGE — 本地浏览页进栈失败");

    /* 语音轮盖过本地浏览页 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_VOICE) == 0 &&
                             kidpal_layer_top(&st) == KIDPAL_LAYER_VOICE,
        "SU-A2/A4 RED: VOICE must front over LOCAL_PAGE — 语音轮未按优先级表置前");

    /* 服务端推送页盖过语音（SU-RULING-01 方案A: 回答期间服务端页前台可见、语音不拦
     * 内容页——回答文字/表情/状态栏落在常驻聊天 layer, 课表/故事/查字回答页不被语音页
     * 盖住） */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_SERVER_PAGE) == 0 &&
                             kidpal_layer_top(&st) == KIDPAL_LAYER_SERVER_PAGE,
        "SU-A2/A4 RED: SERVER_PAGE must front over VOICE during answer — 回答期间服务端"
        "推送页须前台可见、语音不得拦内容页 (SU-RULING-01 方案A, 聊天 layer 叠加其上)");

    /* 提醒弹窗盖过语音轮（F8 分支: 提醒恰在语音轮到点, 弹窗优先级正确、不崩） */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_REMINDER) == 0 &&
                             kidpal_layer_top(&st) == KIDPAL_LAYER_REMINDER,
        "SU-A4 RED: REMINDER must front over VOICE — 提醒恰在语音轮到点时弹窗须置前 "
        "(review F8: 同族崩溃已修症状未修架构, 此处修架构)");

    /* 配网最高 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_PROVISION) == 0 &&
                             kidpal_layer_top(&st) == KIDPAL_LAYER_PROVISION,
        "SU-A2 RED: PROVISION must be the highest front owner — 配网优先级错误");

    /* 出栈回落: 依优先级表逐级回落 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_pop(&st, KIDPAL_LAYER_PROVISION) == 0 &&
                             kidpal_layer_top(&st) == KIDPAL_LAYER_REMINDER,
        "SU-A2/A4 RED: pop(PROVISION) must fall back to REMINDER — 配网出栈回落错误");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_pop(&st, KIDPAL_LAYER_REMINDER) == 0 &&
                             kidpal_layer_top(&st) == KIDPAL_LAYER_SERVER_PAGE,
        "SU-A2/A4 RED: pop(REMINDER) must fall back to SERVER_PAGE — 提醒出栈回落到服务端"
        "推送页 (服务端页 > 语音, 新码序)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_pop(&st, KIDPAL_LAYER_VOICE) == 0 &&
                             kidpal_layer_top(&st) == KIDPAL_LAYER_SERVER_PAGE,
        "SU-A2/A4 RED: pop(VOICE) must fall back to SERVER_PAGE — 语音出栈后服务端推送页"
        "仍为前台 (语音不拦内容页)");

    /* 单一权威: push 顺序无关——逆序 push 后 top 仍 = 最高优先级（取代三散标志的
     * 「谁在前台」单一裁决） */
    {
        kidpal_layer_stack_t st2;
        sunu_stack_init(&st2);
        TEST_ASSERT_TRUE_MESSAGE(
            kidpal_layer_push(&st2, KIDPAL_LAYER_LOCAL_PAGE) == 0 &&
            kidpal_layer_push(&st2, KIDPAL_LAYER_PROVISION) == 0 &&
            kidpal_layer_push(&st2, KIDPAL_LAYER_REMINDER) == 0 &&
            kidpal_layer_push(&st2, KIDPAL_LAYER_SERVER_PAGE) == 0 &&
            kidpal_layer_push(&st2, KIDPAL_LAYER_VOICE) == 0 &&
            kidpal_layer_top(&st2) == KIDPAL_LAYER_PROVISION,
            "SU-A2 RED: top() depends on push order — 前台归属必须由优先级表单一权威, "
            "push 顺序无关（三散标志的竞态土壤在此被钉死）");
    }

    /* 幂等去重: 重复 push 同一 layer 保持单条目 */
    {
        kidpal_layer_stack_t st3;
        sunu_stack_init(&st3);
        kidpal_layer_push(&st3, KIDPAL_LAYER_VOICE);
        kidpal_layer_push(&st3, KIDPAL_LAYER_VOICE);
        SUNU_ASSERT_TRUE_FM(st3.n == 1,
            "SU-A2 RED: duplicate push(VOICE) stacked twice (n=%d) — 前台归属须幂等去重, "
            "不得靠 push 次数冒充优先级", st3.n);
        TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_has(&st3, KIDPAL_LAYER_VOICE) == 1,
            "SU-A2 RED: has(VOICE) must be 1 after push — 在场判定错误");
        kidpal_layer_pop(&st3, KIDPAL_LAYER_VOICE);
        TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_has(&st3, KIDPAL_LAYER_VOICE) == 0 &&
                                 kidpal_layer_top(&st3) == KIDPAL_LAYER_NONE,
            "SU-A2 RED: a single pop must fully remove the single entry — 幂等去重破坏");
    }

    /* has 语义 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_has(&st, KIDPAL_LAYER_SERVER_PAGE) == 1 &&
                             kidpal_layer_has(&st, KIDPAL_LAYER_REMINDER) == 0,
        "SU-A2 RED: has() 在场/不在场判定错误");

    /* 防御 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(NULL, KIDPAL_LAYER_VOICE) == -1,
        "SU-A2 RED: push(NULL,...) must be defended (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_NONE) == -1 &&
                             kidpal_layer_push(&st, KIDPAL_LAYER_CHAT) == -1 &&
                             kidpal_layer_push(&st, 999) == -1 &&
                             kidpal_layer_push(&st, -3) == -1,
        "SU-A2/A4 RED: push of NONE / CHAT(常驻非栈成员) / 非法码 must be -1 — 前台栈"
        "只收 LOCAL_PAGE..PROVISION 五类");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_top(NULL) == -1,
        "SU-A2 RED: top(NULL) must be defended (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_pop(NULL, KIDPAL_LAYER_VOICE) == -1,
        "SU-A2 RED: pop(NULL,...) must be defended (-1)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_pop(&st, KIDPAL_LAYER_PROVISION) == -1,
        "SU-A2 RED: pop of an absent layer must be -1 — 不在场不得伪成功");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_has(NULL, KIDPAL_LAYER_VOICE) == -1 &&
                             kidpal_layer_has(&st, KIDPAL_LAYER_CHAT) == -1,
        "SU-A2 RED: has() NULL / 非法码 must be -1");
}

/* -------------------------------------------------------------------------- *
 * test b2 — SU-A1 / SU-A5 (behavioral, frozen contract):
 * 聊天 layer 落点 + 驻留清除时机（OQ3=c）。
 * 钉：聊天内容落点恒 = 常驻聊天 layer（无独立屏, 屏幕落点确定非随机）；清除时机 =
 * 下次交互（新语音轮/按键清), 回答结束/页面切换不清；未知事件保守驻留。
 * RED today: weak sentinel (-2)。
 * ------------------------------------------------------------------------ */
TEST_CASE("screen unification: 聊天 layer 落点 + 驻留清除时机 (SU-A1/SU-A5)",
          "[screen_unification][SU-A1][SU-A5][behavioral]")
{
    int r;

    /* 落点: 聊天内容（状态栏/表情/气泡）恒落常驻聊天 layer, 非独立屏 */
    r = kidpal_layer_chat_target();
    sunu_require_impl(r, "kidpal_layer_chat_target unimplemented");
    SUNU_ASSERT_TRUE_FM(r == KIDPAL_LAYER_CHAT,
        "SU-A1 RED: chat content target is not the resident chat layer (got %d) — "
        "s_chat_scr 退役后聊天内容必须落 KidPal 屏常驻聊天 layer, 屏幕落点确定非随机 "
        "(OQ1=纯B 归一屏)", r);

    r = kidpal_layer_chat_clear_event(KIDPAL_LAYER_EVT_ANSWER_END);
    sunu_require_impl(r, "kidpal_layer_chat_clear_event unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0,
        "SU-A5 RED: ANSWER_END must NOT clear chat (retain) — 回答结束后聊天 layer 常驻"
        "可见、文字天然留屏 (OQ3=c), 清除时机不是回答结束");

    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_chat_clear_event(KIDPAL_LAYER_EVT_PAGE_SWITCH) == 0,
        "SU-A1/A5 RED: PAGE_SWITCH must NOT clear chat — 页面切换不清聊天, 聊天内容任何页"
        "可见 (归一屏常驻 layer)");

    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_chat_clear_event(KIDPAL_LAYER_EVT_NEW_VOICE) == 1,
        "SU-A5 RED: NEW_VOICE must clear chat — 清除时机=下次交互(新语音轮)未兑现 (OQ3=c)");

    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_chat_clear_event(KIDPAL_LAYER_EVT_KEY_PRESS) == 1,
        "SU-A5 RED: KEY_PRESS must clear chat — 清除时机=下次交互(任意按键)未兑现 (OQ3=c)");

    /* 未知/负值事件: 保守驻留（不清） */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_chat_clear_event(99) == 0 &&
                             kidpal_layer_chat_clear_event(-1) == 0,
        "SU-A5 RED: unknown event must conservatively retain chat (0) — 未知事件不得误清");
}

/* -------------------------------------------------------------------------- *
 * test b3 — SU-A5 / SU-A7 (behavioral, frozen contract):
 * 两键行为显示归属面 + 语音轮→回答驻留→回首页。KEY 路由语义 owner = PKG-1
 * (260911-key-route-a1, mergeOrder 18 先并); 本包只钉显示归属面:
 * voice / home / 翻页导航的前台归属经 layer 管理可到达正确 target（KEY 长按=回首页
 * 的导航动作面 = 回首页是否为 layer 权威可到达的 target）。
 * RED today: weak sentinel (-2)。
 * ------------------------------------------------------------------------ */
TEST_CASE("screen unification: 语音轮→回答驻留→回首页 前台归属 (SU-A5/SU-A7)",
          "[screen_unification][SU-A5][SU-A7][behavioral]")
{
    kidpal_layer_stack_t st;

    sunu_stack_init(&st);

    /* 语音轮开始 -> 语音前台 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_VOICE) == 0 &&
                             kidpal_layer_top(&st) == KIDPAL_LAYER_VOICE,
        "SU-A7 RED: voice-target 前台归属不可达 — 语音轮经 layer 管理置前失败");

    /* 语音回答期间服务端页前台可见（SU-RULING-01 方案A: 回答内容到达后 SERVER_PAGE
     * 盖过语音、语音不拦内容页; 回答完落点确定、非随机） */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_SERVER_PAGE) == 0 &&
                             kidpal_layer_top(&st) == KIDPAL_LAYER_SERVER_PAGE,
        "SU-A7 RED: 语音回答期间服务端推送页须前台可见 — 服务端页未盖过语音、课表/故事/"
        "查字回答页被语音拦 (SU-RULING-01 方案A)");

    /* 语音轮结束 -> 回答页驻留前台（回答驻留的「服务端页」分支, 非随机落点） */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_pop(&st, KIDPAL_LAYER_VOICE) == 0 &&
                             kidpal_layer_top(&st) == KIDPAL_LAYER_SERVER_PAGE,
        "SU-A5 RED: after voice ends the answer page must remain front (驻留) — 回答后"
        "屏幕落点确定, 服务端回答页不得被内核随机回退");

    /* 用户手动翻页（本地浏览）: 归一屏本地基础层回前台——先推本地浏览页作基层, 再把
     * 驻留的服务端回答页出栈, 本地浏览页为前台（LOCAL_PAGE 是最低优先级基础层, 只有
     * 上层出栈后才前台） */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_LOCAL_PAGE) == 0 &&
                             kidpal_layer_pop(&st, KIDPAL_LAYER_SERVER_PAGE) == 0 &&
                             kidpal_layer_top(&st) == KIDPAL_LAYER_LOCAL_PAGE,
        "SU-A7 RED: 本地浏览页(翻页导航)前台归属不可达 — 手动翻页须把服务端回答页出栈"
        "后本地浏览页回前台（LOCAL_PAGE 经 layer 管理可到达前台）");

    /* KEY 长按=回首页（PKG-1 路由语义）的显示归属面: 回首页 = LOCAL_PAGE 经 layer 权威
     * 可到达——重新推服务端页模拟「服务端页 + 本地页」并存态, 再把服务端页出栈, 仅剩
     * 本地页(=首页 P1) */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_SERVER_PAGE) == 0 &&
                             kidpal_layer_top(&st) == KIDPAL_LAYER_SERVER_PAGE &&
                             kidpal_layer_pop(&st, KIDPAL_LAYER_SERVER_PAGE) == 0 &&
                             kidpal_layer_top(&st) == KIDPAL_LAYER_LOCAL_PAGE,
        "SU-A7 RED: KEY 长按=回首页的导航动作面经 layer 权威不可到达 — 回首页 target "
        "(LOCAL_PAGE 首页) 必须可由 layer 栈管理把服务端页出栈后单留首页 "
        "(路由语义 owner=PKG-1, 本包钉显示归属面: 回首页是 layer 权威可到达的 target)");
}

/* -------------------------------------------------------------------------- *
 * test s1 — SU-A2 (structural, 符号级零残留):
 * 三标志 + 聊天屏载体（s_chat_scr 及其改名 s_voice_ui_scr）+ tick 对账轮询屏切换
 * 路径移除。全部做「符号/路径消失」断言（文本读工作树, 非「没崩」）。扫描跑真实
 * 工作树根, 排除 test/host_app/build/.git/assets。SU-RULING-02 收紧：旧 s_chat_scr
 * 被改名 s_voice_ui_scr 仍是第二前台屏载体, 补钉新旧两名须同归于零（封死捕获改名路径）。
 * ------------------------------------------------------------------------ */
TEST_CASE("screen unification: 三标志 + tick 对账 + 聊天屏符号级零残留 "
          "(s_chat_scr/s_voice_ui_scr) (SU-A2)",
          "[screen_unification][SU-A2][structural]")
{
    char hit[SUNU_MAX_PATH];
    int n;

    SUNU_REQUIRE_ROOT();

    n = sunu_scan_count("firmware/main", "s_chat_overlay", hit, sizeof(hit));
    SUNU_ASSERT_TRUE_FM(n == 0,
        "SU-A2 RED: s_chat_overlay 符号级残留（%d 文件, 首现 %s）— 聊天覆盖散标志必须"
        "整体退役, 前台归属改由 layer 栈管理单一权威", n, hit);

    n = sunu_scan_count("firmware/main", "s_page_pinned", hit, sizeof(hit));
    SUNU_ASSERT_TRUE_FM(n == 0,
        "SU-A2 RED: s_page_pinned 符号级残留（%d 文件, 首现 %s）— 服务端页 pin 散标志"
        "必须整体退役（含 kidpal_render_node.h/.cc/mcp_server.cc 的过时注释一律清）",
        n, hit);

    n = sunu_scan_count("firmware/main", "s_current_page", hit, sizeof(hit));
    SUNU_ASSERT_TRUE_FM(n == 0,
        "SU-A2 RED: s_current_page 符号级残留（%d 文件, 首现 %s）— 当前页散标志必须"
        "整体退役, 页归属并入 layer 栈管理", n, hit);

    n = sunu_scan_count("firmware/main", "s_chat_scr", hit, sizeof(hit));
    SUNU_ASSERT_TRUE_FM(n == 0,
        "SU-A1/A2 RED: s_chat_scr 符号级残留（%d 文件, 首现 %s）— 独立聊天屏必须退役, "
        "聊天控件迁 KidPal 屏常驻 layer", n, hit);

    /* 第二屏载体变量收口（SU-RULING-02 收紧）：builder 把旧 s_chat_scr 改名
     * s_voice_ui_scr 绕过「旧名零残留」齿——补钉新名同归于零, 封死捕获改名路径。
     * reparent 后聊天控件是 s_kidpal_scr 上的常驻 layer, 不应再有独立屏载体变量。 */
    n = sunu_scan_count("firmware/main", "s_voice_ui_scr", hit, sizeof(hit));
    SUNU_ASSERT_TRUE_FM(n == 0,
        "SU-A1/A2 RED: s_voice_ui_scr 符号级残留（%d 文件, 首现 %s）— 第二前台屏载体"
        "变量必须退役（独立聊天屏改名规避被此齿钉死, SU-RULING-02 单物理屏）", n, hit);

    /* tick 对账轮询屏切换路径移除: 无文件同时「读设备态」且「切换屏」——归一屏后前台
     * 变化为事件驱动 layer push/pop, 不再 1s 轮询 GetDeviceState 驱动 lv_screen_load */
    n = sunu_scan_count_both("firmware/main", "GetDeviceState(", "lv_screen_load(",
                             hit, sizeof(hit));
    SUNU_ASSERT_TRUE_FM(n == 0,
        "SU-A2 RED: tick 对账轮询屏切换路径残留（%d 文件, 首现 %s）— 同一文件边读 "
        "GetDeviceState 边 lv_screen_load 的 1s 轮询对账必须移除, 前台归属改为 layer "
        "push/pop 事件驱动", n, hit);
}

/* -------------------------------------------------------------------------- *
 * test s2 — SU-A1 (structural, 聊天 layer 化装配面):
 * link 模块在树（头 + 定义）+ 编入设备构建 + host 接线序守卫 + 生产消费者 wiring。
 * RED today: 模块零树/未接线/无消费者。
 * ------------------------------------------------------------------------ */
TEST_CASE("screen unification: layer 模块在树 + 设备构建 + host 接线 + 消费者 (SU-A1)",
          "[screen_unification][SU-A1][structural]")
{
    char hit[SUNU_MAX_PATH];
    char *mk;
    int n;

    SUNU_REQUIRE_ROOT();

    /* 模块在树（头声明 + 实现定义 >= 2 文件） */
    n = sunu_scan_count("firmware/main", "kidpal_layer_top(", hit, sizeof(hit));
    SUNU_ASSERT_TRUE_FM(n >= 2,
        "SU-A1 RED: no layer module under firmware/main (kidpal_layer_top absent, %d "
        "files) — 前台 layer 管理单一权威未落成 (PROPOSAL §2.1/§4)", n);

    /* 编入设备构建（装配事实, 非浮文件） */
    TEST_ASSERT_TRUE_MESSAGE(sunu_file_contains("firmware/main/CMakeLists.txt",
                                                "kidpal_layer.c"),
        "SU-A1 RED: kidpal_layer.c not referenced by firmware/main/CMakeLists.txt — "
        "layer 管理模块未编入设备构建");

    /* 生产消费者 wiring: 模块头 + 定义 + 至少一个真实生产方（聊天落点/前台事件/页面
     * 渲染路径） >= 3 文件 */
    n = sunu_scan_count("firmware/main", "kidpal_layer_", hit, sizeof(hit));
    SUNU_ASSERT_TRUE_FM(n >= 3,
        "SU-A1 RED: kidpal_layer_ 前缀仅 %d 文件引用, 需 >= 3（头 + 定义 + 生产消费者）"
        " — 归一屏 layer 管理没有真实生产消费调用点（SetupUI 聊天控件迁 layer 的落点、"
        "SetChatMessage/SetStatus/SetEmotion 改写 layer API 未接线）", n);

    /* host 接线序守卫（本文件在 HOST_ACCEPTANCE_SRCS 且排在 smoke 之前; 接线属 builder,
     * 红 = 未接线） */
    mk = sunu_read("firmware/main/test/host_app/main/CMakeLists.txt", NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk, "SUNU RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *mine  = strstr(mk, "screen_unification.acceptance.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(mine != NULL,
            "SU-A1 RED: screen_unification.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS — "
            "host 验收构建未接本包 (BUILDER DUTY)");
        TEST_ASSERT_TRUE_MESSAGE(smoke != NULL,
            "SU-A1 RED(infra): host_infra_smoke.acceptance.c lost from HOST_ACCEPTANCE_SRCS");
        TEST_ASSERT_TRUE_MESSAGE(mine < smoke,
            "SU-A1 RED: screen_unification.acceptance.c 必须排在 host_infra_smoke.acceptance.c "
            "之前 (smoke 末位 = 最先执行, boundary 齿注册序守卫)");
    }
    free(mk);
}

/* -------------------------------------------------------------------------- *
 * test s3 — SU-A3 (structural, char 并屏 单屏不变式 + 第二屏载体清零):
 * char 页已入 s_kidpal_scr layer 体系、无独立屏。display/ 下唯一的顶层屏创建点
 * (lv_obj_create(NULL)) = s_kidpal_scr 一处; char 页自建屏句柄 g_page.scr 退役。
 * SU-RULING-02 收紧（单物理屏）：除 s_kidpal_scr 外不得再有第二个前台屏载体被
 * lv_screen_load 切前——无论它是 lv_obj_create(NULL) 创建（左齿已钉）还是
 * lv_screen_active() 捕获改名而来（新增「加载实参恒 = s_kidpal_scr」齿封死非创建点
 * 绕过路径; s_voice_ui_scr = lv_screen_active() 捕获后在 reconcile 被
 * lv_screen_load(s_voice_ui_scr) 切前即命中）。
 * ------------------------------------------------------------------------ */
TEST_CASE("screen unification: char 页并入 s_kidpal_scr——display/ 单屏不变式 + "
          "第二屏载体清零 (SU-A3)",
          "[screen_unification][SU-A3][structural]")
{
    char hit[SUNU_MAX_PATH];
    int n;
    int n_kidpal_load, n_total_load;

    SUNU_REQUIRE_ROOT();

    /* 单屏不变式: display/ 只有 s_kidpal_scr 一个顶层屏创建点（char 独立屏退役后）。
     * 红基线 = 2 文件（kidpal_render_node.cc 的 s_kidpal_scr + char_page.cc 的 g_page.scr）。 */
    n = sunu_scan_count("firmware/main/display", "lv_obj_create(NULL)", hit, sizeof(hit));
    SUNU_ASSERT_TRUE_FM(n == 1,
        "SU-A3 RED: display/ 有 %d 个顶层屏创建点 (lv_obj_create(NULL)), 必须 == 1（仅 "
        "s_kidpal_scr; 首现 %s）— char 独立屏必须并入 s_kidpal_scr 体系 (review F7), "
        "归一屏后只有一个真实屏", n, hit);

    /* char 页自建屏句柄退役（其 screen 创建点并入上层 layer） */
    n = sunu_scan_count("firmware/main/display", "g_page.scr", hit, sizeof(hit));
    SUNU_ASSERT_TRUE_FM(n == 0,
        "SU-A3 RED: g_page.scr 符号级残留（%d 文件, 首现 %s）— char 页自建独立屏句柄"
        "必须退役, char 展示作为 s_kidpal_scr 上的一层/一页", n, hit);

    /* 第二前台屏载体加载清零（SU-RULING-02 收紧）：每次 lv_screen_load( 的实参必须
     * 恒为 s_kidpal_scr。按「全部 lv_screen_load( 出现次数 == lv_screen_load(s_kidpal_scr)
     * 出现次数」钉死——同文件既有合法 load 又有违规 load（现 kidpal_render_node.cc 既有
     * 8 次 s_kidpal_scr、又有 1 次 s_voice_ui_scr）仅按文件计数判不出的情况, 按次数判
     * 即可红。封死两条已证实绕过路径的通用形态（不依赖变量名）：①捕获改名——
     * s_voice_ui_scr = lv_screen_active() 捕获的屏被 lv_screen_load(s_voice_ui_scr)
     * 切前; ②非创建点——捕获屏不是 lv_obj_create(NULL) 创建点, 单处创建点计数照旧绿。
     * 配套 s1 的 s_voice_ui_scr 名钉（封具体这次改名）双保险。 */
    n_kidpal_load = sunu_scan_occurrences("firmware/main/display", "lv_screen_load(s_kidpal_scr)");
    n_total_load  = sunu_scan_occurrences("firmware/main/display", "lv_screen_load(");
    SUNU_ASSERT_TRUE_FM(n_kidpal_load >= 1,
        "SU-A3 RED: display/ 无任何 lv_screen_load(s_kidpal_scr)（%d 次）— 归一屏的唯一"
        "天花板 s_kidpal_scr 未被加载（本齿防「零加载」假绿）", n_kidpal_load);
    SUNU_ASSERT_TRUE_FM(n_total_load == n_kidpal_load,
        "SU-A3 RED: display/ 有 %d 次 lv_screen_load( 但仅 %d 次加载 s_kidpal_scr — 存在"
        "非 s_kidpal_scr 的第二前台屏载体被切前（捕获改名/非创建点绕过路径仍通, "
        "SU-RULING-02 单物理屏被绕过）", n_total_load, n_kidpal_load);
}

/* -------------------------------------------------------------------------- *
 * test s4 — SU-E3 / boundary/security (structural 守卫):
 * 冻结面零越界 canary + 扫描器自检防假绿 + 设备不直连 openclaw/18789。
 * 全部 must-stay-green。
 * ------------------------------------------------------------------------ */
TEST_CASE("screen unification: 冻结面零越界 + 扫描器自检 + 直连守卫 (SU-E3/boundary)",
          "[screen_unification][SU-E3][boundary][security]")
{
    char hit[SUNU_MAX_PATH];
    int n;

    SUNU_REQUIRE_ROOT();

    /* 契约/spec canary 在世（SU-E3 冻结面零越界: 不碰 docs/contracts + spec） */
    TEST_ASSERT_TRUE_MESSAGE(sunu_exists("docs/contracts/interface-contract.md"),
        "SU-E3 RED(infra): contract file lost — 冻结面被触碰（本包零契约触碰红线）");
    TEST_ASSERT_TRUE_MESSAGE(sunu_exists("docs/spec.md"),
        "SU-E3 RED(infra): spec file lost — spec 零修订红线（SPEC-1 本批不触发）");

    /* 设备不直连 OpenClaw / 18789（WSA/VRT/KSP/KSTORY/CHR 守卫对偶） */
    n = sunu_scan_count("firmware/main", "openclaw", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "security RED: firmware references openclaw directly — 设备只连自建网关");
    n = sunu_scan_count("firmware/main", "18789", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "security RED: firmware references port 18789 — OpenClaw 端口不得出现在设备侧");

    /* 扫描器自检（防假绿: 负向齿必须能红） */
    TEST_ASSERT_TRUE_MESSAGE(sunu_exists("firmware/main/display/view_router.c"),
        "boundary RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(sunu_exists("firmware/main/display/__sunu_no_such__.c"),
        "boundary RED(infra): scanner reports a nonexistent file as present");
    TEST_ASSERT_NULL_MESSAGE(sunu_read("firmware/main/display/__sunu_no_such__.c", NULL),
        "boundary RED(infra): sunu_read returned data for a nonexistent file");
    n = sunu_scan_count("firmware/main/display", "__sunu_no_such_needle__", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "boundary RED(infra): scan matched a needle that cannot exist — 负向断言假绿");
    n = sunu_scan_count("firmware/main/display", "VRT_ROUTE_CHAR", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "boundary RED(infra): occurrence counter cannot count a known literal — 计数器失灵");
}