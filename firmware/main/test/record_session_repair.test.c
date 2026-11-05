/*
 * record_session_repair.test.c — builder 单元测试（TaskPackage
 * 261001-m4-k3-record-session-live-repair）。补冻结验收件 RSR-A1..A5 决策表未穷尽的
 * 分支/边界（[R1] 规则优先级与 ctx 非零语义 / [R2] 越界与幂等 / [R3] 非零与非列表态）。
 * 被测实现 = display/story_page.c 尾部 [R1]-[R3] 冻结缝（host runner 已编入）。
 */
#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include "../display/story_page.h"
#include "../display/kidpal_layer.h"

/* 冻结数值镜像（与 story_page.h / kidpal_render_node.h 等值; 漂移由既有冻结齿先红）。
 * 2026-11-05 随 261104-k0a-four-page-skeleton D-01 四页骨架重锚：KP_PAGE_STORY 2→3
 * （P3 = 星星积分本插入、故事原地迁移 P4；字面真值单点钉 retract test 3）。 */
#define UT_KP_PAGE_STORY 3 /* KP_PAGE_STORY（D-01 四页镜像，落地值 3） */
#define UT_KP_PAGE_HOME  0 /* KP_PAGE_HOME */

/* ---- [R1] kidpal_story_key_owns 决策表补强 ---- */

TEST_CASE("record_session_repair_ut: [R1] 规则优先级与 ctx 非零语义", "[record_session_repair][ut]")
{
    /* (a) 规则优先于更高覆盖层: 记录语境录音态下, 即使 REMINDER 最上层压顶, 仍判故事页
     * 拥有 KEY（(a) 只读 page_state+ctx_active, 与 front_layer 无关）——冻结齿只测
     * VOICE/NONE 前台, REMINDER 前台是未穷尽支。 */
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_key_owns(KIDPAL_LAYER_REMINDER,
                                                   UT_KP_PAGE_STORY,
                                                   KSTORY_PAGE_RECORDING, 1));

    /* (b) ctx_active 非零即成立（负数也 != 0）: -3 等价于 1。 */
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_key_owns(KIDPAL_LAYER_NONE,
                                                   UT_KP_PAGE_STORY,
                                                   KSTORY_PAGE_RECORDING, -3));

    /* (b) 删除覆盖层支与 page_state 无关: 删除层前台 + ctx 未置 + 录音态 -> 规则 (a) 不
     * 成立, 但规则 (b) 命中 -> 1（防删除子态漏网手势误映射 SESSION_TOGGLE）。 */
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_key_owns(KIDPAL_LAYER_STORY_DELETE,
                                                   UT_KP_PAGE_HOME,
                                                   KSTORY_PAGE_RECORDING, 0));
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_key_owns(KIDPAL_LAYER_STORY_DELETE,
                                                   UT_KP_PAGE_HOME,
                                                   KSTORY_PAGE_LIST, 0));

    /* 负锚: 基层 P3 之外 + 记录语境未置 + 非录音态 -> 0（不侵占全局路由）。 */
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_key_owns(KIDPAL_LAYER_NONE,
                                                   UT_KP_PAGE_HOME,
                                                   KSTORY_PAGE_LIST, 0));
    /* 负锚: VOICE 前台 + 越界/负 page_state + 记录语境未置 -> 0（畸形入参不误判,
     * 非记录语境录音态不占故事页归属）。 */
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_key_owns(KIDPAL_LAYER_VOICE,
                                                   UT_KP_PAGE_STORY, -1, 0));
    /* 边界: 基层 P3（NONE 前台）时页态无关（含越界值）——规则 (c) 恒判故事页拥有
     * （K2 基层页内手势族零回归语义）。 */
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_key_owns(KIDPAL_LAYER_NONE,
                                                   UT_KP_PAGE_STORY, -1, 0));
}

/* ---- [R2] kidpal_story_session_end 边界/幂等补强 ---- */

TEST_CASE("record_session_repair_ut: [R2] 越界页态与幂等", "[record_session_repair][ut]")
{
    int page = 77;

    /* 越界/负页态 -> 0 且 out 不触碰（非录音态零动作, 不误清子态）。 */
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_session_end(-1, &page));
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_session_end(99, &page));
    TEST_ASSERT_EQUAL_INT(77, page);

    /* 幂等: 录音态复位 LIST 返回 1; 紧接以 LIST 再调用 -> 0（重复收尾安全）。 */
    page = -5;
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_session_end(KSTORY_PAGE_RECORDING, &page));
    TEST_ASSERT_EQUAL_INT(KSTORY_PAGE_LIST, page);
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_session_end(page, &page));
}

/* ---- [R3] kidpal_story_list_deferred_flush 非零/非列表态补强 ---- */

TEST_CASE("record_session_repair_ut: [R3] 非零 voice_front 与越界页态", "[record_session_repair][ut]")
{
    /* voice_front 非零（含负/任意非零 = VOICE 层仍前台）-> 0（不抢推前台）。 */
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_list_deferred_flush(-1, UT_KP_PAGE_STORY,
                                                              KSTORY_PAGE_LIST));
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_list_deferred_flush(2, UT_KP_PAGE_STORY,
                                                              KSTORY_PAGE_LIST));
    /* 越界/负 page_state -> 0。 */
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_list_deferred_flush(0, UT_KP_PAGE_STORY, -1));
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_list_deferred_flush(0, UT_KP_PAGE_STORY, 99));
    /* 命中支再确认（全真组合）。 */
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_list_deferred_flush(0, UT_KP_PAGE_STORY,
                                                              KSTORY_PAGE_LIST));
}

/* ==========================================================================
 * [F2] K0A-E3-F2 结构齿（261104-k0a-four-page-skeleton fixer 轮，约束版 C1b，
 * architect 2026-11-05 裁定）——**只证落点与顺序，不冒充运行时行为验证**。
 *
 * 缺陷：云端 fetch 阅读路径（mcp_server.cc VRT_ROUTE_STORY_TEXT 普通阅读渲染区）只把
 * READER 落在栈上快照 reader_view，从不提交回 mcp 权威视图 g_story_view、也不
 * cache_story —— 键归属闸（读渲染节点，F1 后=READER）放行 KEY 后，HandleStoryKey 的
 * 输入决策表读 g_story_view=LIST，把 KEY 短按判成 REC_SESSION 误开录音会话
 * （真机 :114 story_key rec_session_slot page=0）。生产侧两针已落；本齿钉死它们不被
 * 后续改动删失/漂移：
 *   ① open_reader(&reader_view … 之后、渲染排队（app.Schedule([reader_view …）之前，
 *      必须存在 `g_story_view = reader_view;`（阅读开态提交回权威视图）；
 *   ② 闭包内 page_render(&reader_view … 之后、end_page(); 之前，必须存在执行期
 *      g_story_* 的 `kidpal_render_node_cache_story(&g_story_view, &g_story_list,
 *      &g_story_text);`（渲染节点缓存同步 READER；不得回写旧快照，守 R2-F4 防时序倒挂）。
 * 自带扫描器负向自检：切片内插入一个绝不存在的探针必须不命中（防「搜索恒真」假绿）。
 * 变异自验（fixer 轮实做，非本齿内）：删①赋值 / 删②缓存 各自令本齿红、还原转绿。
 * ========================================================================== */
#define RRF2_MCP_CC   "firmware/main/mcp_server.cc"
#define RRF2_MAX_PATH 1024
#define RRF2_MAX_READ (8u * 1024u * 1024u)

static char rrf2_root[RRF2_MAX_PATH];
static int rrf2_root_ready = 0;

static int rrf2_is_regular(const char *p)
{
    struct stat st;
    return stat(p, &st) == 0 && S_ISREG(st.st_mode);
}

static int rrf2_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[RRF2_MAX_PATH];
    size_t i;

    if (rrf2_root_ready) {
        return 1;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (rrf2_is_regular(probe)) {
            snprintf(rrf2_root, sizeof(rrf2_root), "%s", cands[i]);
            rrf2_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static char *rrf2_read(const char *rel, size_t *out_len)
{
    char path[RRF2_MAX_PATH];
    FILE *f;
    char *buf;
    size_t n;

    if (!rrf2_root_ready && !rrf2_locate_root()) {
        return NULL;
    }
    snprintf(path, sizeof(path), "%s/%s", rrf2_root, rel);
    f = fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }
    buf = (char *)malloc(RRF2_MAX_READ);
    assert(buf != NULL);
    n = fread(buf, 1, RRF2_MAX_READ - 1, f);
    fclose(f);
    buf[n] = '\0';
    if (out_len != NULL) {
        *out_len = n;
    }
    return buf;
}

/* 文件内 [start_needle, end_needle) 切片（start 缺失 -> NULL; end 缺失 -> 切到 EOF）。
 * 返回 malloc 串，调用方 free。 */
static char *rrf2_region(const char *rel, const char *start_needle,
                         const char *end_needle)
{
    char *buf = rrf2_read(rel, NULL);
    char *s, *e, *slice;
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

TEST_CASE("record_session_repair_ut: [F2] cloud reader commits page state (K0A-E3-F2)",
          "[record_session_repair][ut][structural]")
{
    char *r1;
    char *r2;
    const char *absent = "__rrf2_absent_needle_9f3a__";

    TEST_ASSERT_TRUE_MESSAGE(rrf2_locate_root(),
        "RRF2 RED(infra): repo root not located from cwd — run the ELF from repo root");

    /* ---- ① 阅读开态提交：open_reader(&reader_view … 之后、app.Schedule([reader_view 之前 ---- */
    r1 = rrf2_region(RRF2_MCP_CC, "kidpal_story_open_reader(&reader_view",
                     "app.Schedule([reader_view");
    TEST_ASSERT_NOT_NULL_MESSAGE(r1,
        "RRF2 RED(infra): 阅读开态区切片失败（open_reader(&reader_view -> "
        "app.Schedule([reader_view 锚漂移，须复核）");
    TEST_ASSERT_TRUE_MESSAGE(strlen(r1) > 0, "RRF2 RED(infra): 切片为空");
    /* 扫描器负向自检：绝不存在的探针必须不命中（防搜索恒真假绿）。 */
    TEST_ASSERT_NULL_MESSAGE(strstr(r1, absent),
        "RRF2 RED(infra): 扫描器负向自检失败——不存在的探针竟命中（搜索恒真）");
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(r1, "kidpal_story_open_reader(&reader_view"),
        "RRF2 RED(infra): 起点锚不在切片内");
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(r1, "g_story_view = reader_view;"),
        "K0A-E3-F2 RED①: open_reader(&reader_view 之后、渲染排队之前缺 "
        "`g_story_view = reader_view;`——云端阅读开态未提交回 mcp 权威视图；"
        "HandleStoryKey 输入表仍见 LIST -> KEY 短按 REC_SESSION 误开录音会话");
    /* 顺序：提交必须在 open_reader 之后。 */
    TEST_ASSERT_TRUE_MESSAGE(
        strstr(r1, "g_story_view = reader_view;") >
            strstr(r1, "kidpal_story_open_reader(&reader_view"),
        "RRF2 RED: 提交早于 open_reader（顺序错）");
    free(r1);

    /* ---- ② 渲染节点缓存同步：page_render(&reader_view … 之后、end_page(); 之前 ---- */
    r2 = rrf2_region(RRF2_MCP_CC, "kidpal_story_page_render(&reader_view",
                     "kidpal_render_node_end_page();");
    TEST_ASSERT_NOT_NULL_MESSAGE(r2,
        "RRF2 RED(infra): 阅读闭包切片失败（page_render(&reader_view -> end_page 锚漂移，"
        "须复核）");
    TEST_ASSERT_TRUE_MESSAGE(strlen(r2) > 0, "RRF2 RED(infra): 切片为空");
    TEST_ASSERT_NULL_MESSAGE(strstr(r2, absent),
        "RRF2 RED(infra): 扫描器负向自检失败——不存在的探针竟命中（搜索恒真）");
    TEST_ASSERT_NOT_NULL_MESSAGE(
        strstr(r2, "kidpal_render_node_cache_story(&g_story_view, &g_story_list, &g_story_text);"),
        "K0A-E3-F2 RED②: page_render(&reader_view 之后、end_page 之前缺执行期 g_story_* 的 "
        "`kidpal_render_node_cache_story(&g_story_view, &g_story_list, &g_story_text);`"
        "——渲染节点缓存停留 LIST（deferred_flush 半分裂）；且必须取执行期 g_story_*、"
        "不回写旧快照（R2-F4 防时序倒挂）");
    /* 顺序：缓存必须在 page_render 之后（且在 end_page 之前——由切片右界保证）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        strstr(r2, "kidpal_render_node_cache_story(&g_story_view") >
            strstr(r2, "kidpal_story_page_render(&reader_view"),
        "RRF2 RED: 缓存早于 page_render（顺序错）");
    free(r2);
}
