/*
 * story_page_v2.test.c — builder 单元测试（TaskPackage 260924-m4-k2-p3-star-notebook-ui;
 * 非冻结件）。补强冻结齿 story_page_v2.acceptance.c 未穷尽的边角与边界:
 *   - cursor_step 跨屏普通步进（屏内非边界、第 2/3 屏内步进 screen 保持）;
 *   - screen_next 局部末屏钳制（屏数与行数不一致防御）;
 *   - stamp_text cap 恰好满（exact-fit 拒绝边界）;
 *   - text_seq 空白/负值/NULL/非 JSON;
 *   - 页内输入未知页值兜底 / NULL 视图 / KEY_GESTURE_NONE;
 *   - 消歧窗 uint32 回绕边界（arm 近 UINT32_MAX）;
 *   - idle_step / resend_due 边界与负 tries;
 *   - reconcile 空 items 窗口 / view_init NULL 列表 / on_saved 畸形 id。
 * 置于 host smoke acceptance 之前（注册序纪律）。
 */

#include <unity.h>
#include <string.h>
#include <stdint.h>

#include "../display/story_page.h"

static kidpal_story_list_t make_list(int total, int count)
{
    kidpal_story_list_t l;
    int i;

    memset(&l, 0, sizeof(l));
    l.total = total;
    l.count = count;
    for (i = 0; i < count; i++) {
        snprintf(l.items[i].id, sizeof(l.items[i].id), "id-%02d", i);
        snprintf(l.items[i].title, sizeof(l.items[i].title), "题%d", i);
        snprintf(l.items[i].date, sizeof(l.items[i].date), "08-%02d", i + 1);
    }
    return l;
}

/* ---- cursor_step 跨屏普通步进（冻结齿只钉屏底/到点两端, 本测补屏内中段） ---- */
TEST_CASE("story_page_v2_ut: cursor_step 屏内中段步进 screen 保持", "[story_page_v2][ut]")
{
    int c = -1, s = -1;

    /* 第 1 屏内 (cursor 1 -> 2, 2%4 != 0): screen 保持 0。 */
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_list_cursor_step(1, 10, 4, 0, &c, &s));
    TEST_ASSERT_EQUAL_INT(2, c);
    TEST_ASSERT_EQUAL_INT(0, s);

    /* 第 2 屏内 (cursor 5 -> 6, 6%4 = 2 != 0): screen 保持 1。 */
    c = -1; s = -1;
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_list_cursor_step(5, 10, 4, 1, &c, &s));
    TEST_ASSERT_EQUAL_INT(6, c);
    TEST_ASSERT_EQUAL_INT(1, s);

    /* 屏底前一格 (cursor 6 -> 7): screen 保持 1。 */
    c = -1; s = -1;
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_list_cursor_step(6, 10, 4, 1, &c, &s));
    TEST_ASSERT_EQUAL_INT(7, c);
    TEST_ASSERT_EQUAL_INT(1, s);
}

/* ---- screen_next 局部末屏 / 屏数行数不一致防御 ---- */
TEST_CASE("story_page_v2_ut: screen_next 局部末屏与不一致防御", "[story_page_v2][ut]")
{
    int c = -1, s = -1;

    /* 局部末屏 (count=6, rows=4, screens=2): screen 1 -> 0, cursor 落 0。 */
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_list_screen_next(1, 2, 4, 6, &s, &c));
    TEST_ASSERT_EQUAL_INT(0, s);
    TEST_ASSERT_EQUAL_INT(0, c);

    /* 不一致防御 (count=5 但 screens=3): screen 1 -> 2, cursor 8 越界 -> 收敛 4。 */
    c = -1; s = -1;
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_list_screen_next(1, 3, 4, 5, &s, &c));
    TEST_ASSERT_EQUAL_INT(2, s);
    TEST_ASSERT_EQUAL_INT(4, c);
}

/* ---- stamp_text cap 恰好满（exact-fit 拒绝边界） ---- */
TEST_CASE("story_page_v2_ut: stamp_text cap 边界", "[story_page_v2][ut]")
{
    char buf[40];

    /* 降级文案 8 个 CJK 字符 = 24 字节 + NUL = 25: cap 24 拒 / cap 25 过。 */
    TEST_ASSERT_EQUAL_INT(-1, kidpal_story_stamp_text(KSTORY_SEQ_ABSENT, buf, 24));
    memset(buf, 0, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_stamp_text(KSTORY_SEQ_ABSENT, buf, 25));
    TEST_ASSERT_EQUAL_STRING("星星徽章已入册！", buf);

    /* NULL out -> -1。 */
    TEST_ASSERT_EQUAL_INT(-1, kidpal_story_stamp_text(6, NULL, 64));
}

/* ---- text_seq 空白 / 负值 / NULL / 非 JSON ---- */
TEST_CASE("story_page_v2_ut: text_seq 边角", "[story_page_v2][ut]")
{
    TEST_ASSERT_EQUAL_INT(KSTORY_SEQ_ABSENT, kidpal_story_text_seq(NULL));
    TEST_ASSERT_EQUAL_INT(KSTORY_SEQ_ABSENT, kidpal_story_text_seq("not json"));
    TEST_ASSERT_EQUAL_INT(KSTORY_SEQ_ABSENT,
                          kidpal_story_text_seq("{\"id\":\"x\",\"text\":\"b\",\"seq\":-3}"));
    TEST_ASSERT_EQUAL_INT(9,
                          kidpal_story_text_seq("{ \"id\" : \"x\" , \"seq\" : 9 , \"text\":\"b\" }"));
    /* seq 在前、其它键在后: 首个 seq 命中即返回。 */
    TEST_ASSERT_EQUAL_INT(3,
                          kidpal_story_text_seq("{\"seq\":3,\"title\":\"t\"}"));
}

/* ---- 页内输入兜底面（未知页值 / NULL 视图 / NONE 手势 / 越界键位） ---- */
TEST_CASE("story_page_v2_ut: page_input 兜底面", "[story_page_v2][ut]")
{
    kidpal_story_list_t l = make_list(2, 2);
    kidpal_story_view_t v;

    TEST_ASSERT_EQUAL_INT(0, kidpal_story_view_init(&v, &l));
    v.page = 99; /* 未知页值 -> 兜底 GLOBAL（不崩溃、不误消费）。 */
    TEST_ASSERT_EQUAL_INT(KSPV2_ACT_GLOBAL,
                          kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_CLICK));
    TEST_ASSERT_EQUAL_INT(KSPV2_ACT_GLOBAL,
                          kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS));

    v.page = KSTORY_PAGE_LIST;
    TEST_ASSERT_EQUAL_INT(KSPV2_ACT_NONE,
                          kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_NONE));
    TEST_ASSERT_EQUAL_INT(KSPV2_ACT_DEGRADE, kidpal_story_page_input(NULL, KEY_ID_KEY, KEY_GESTURE_CLICK));
    TEST_ASSERT_EQUAL_INT(KSPV2_ACT_DEGRADE,
                          kidpal_story_page_input(&v, (key_id_t)KEY_ID_COUNT, KEY_GESTURE_CLICK));
}

/* ---- 消歧窗 uint32 回绕边界（arm 近 UINT32_MAX） ---- */
TEST_CASE("story_page_v2_ut: 消歧窗回绕", "[story_page_v2][ut]")
{
    const uint32_t arm = 0xFFFFFFFFu;

    /* 回绕后 299ms: 未到期 / 窗内冲销。 */
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_debounce_expired(arm, arm + 299u));
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_debounce_cancel(arm, arm + 299u));
    /* 回绕后整 300ms: 到期 / 不冲销（无缝划分）。 */
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_debounce_expired(arm, arm + 300u));
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_debounce_cancel(arm, arm + 300u));
    /* arm == now: 窗内冲销。 */
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_debounce_cancel(arm, arm));
}

/* ---- idle_step / resend_due 边界与负 tries ---- */
TEST_CASE("story_page_v2_ut: idle_step/resend_due 边界", "[story_page_v2][ut]")
{
    unsigned long idle = 0;

    TEST_ASSERT_EQUAL_INT(0, kidpal_story_delete_idle_step(&idle, 9999UL));
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_delete_idle_step(&idle, 1UL)); /* 恰 10000 到点 */
    TEST_ASSERT_EQUAL_INT(-1, kidpal_story_delete_idle_step(NULL, 1UL));

    /* 负 tries（防御）-> 恒不重发; tries=2 到点仍可发第 3 次。 */
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_delete_resend_due(-1, 999999u));
    TEST_ASSERT_EQUAL_INT(1, kidpal_story_delete_resend_due(2, KSPV2_DELETE_RESEND_MS));
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_delete_resend_due(3, KSPV2_DELETE_RESEND_MS));
}

/* ---- reconcile 空 items 窗口 / view_init NULL 列表 / on_saved 畸形 id ---- */
TEST_CASE("story_page_v2_ut: 对账/装载/盖章畸形入参", "[story_page_v2][ut]")
{
    kidpal_story_list_t l = make_list(1, 1);
    kidpal_story_view_t v;
    char big_id[64];

    /* id 在 items 数组但窗口 count=0（空窗口）-> 0（冲销）。 */
    l.count = 0;
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_delete_reconcile_hit("id-00", &l));
    /* NULL pending / NULL list -> -1。 */
    TEST_ASSERT_EQUAL_INT(-1, kidpal_story_delete_reconcile_hit(NULL, &l));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_story_delete_reconcile_hit("id-00", NULL));

    /* view_init(NULL) -> 空视图（count 0 / screens 1 / counter 0）。 */
    memset(&v, 0xFF, sizeof(v));
    TEST_ASSERT_EQUAL_INT(0, kidpal_story_view_init(&v, NULL));
    TEST_ASSERT_EQUAL_INT(0, v.count);
    TEST_ASSERT_EQUAL_INT(1, v.screens);
    TEST_ASSERT_EQUAL_INT(0, v.counter);
    TEST_ASSERT_EQUAL_INT(KSTORY_PAGE_LIST, v.page);

    /* on_saved 畸形 id -> -1 状态零污染。 */
    (void)kidpal_story_view_init(&v, &l);
    TEST_ASSERT_EQUAL_INT(-1, kidpal_story_on_saved(&v, NULL, 5));
    TEST_ASSERT_EQUAL_INT(-1, kidpal_story_on_saved(&v, "", 5));
    TEST_ASSERT_EQUAL_INT(KSTORY_PAGE_LIST, v.page);

    /* stamp_serial 探针: 初值 ABSENT / NULL 视图 ABSENT。 */
    TEST_ASSERT_EQUAL_INT(KSTORY_SEQ_ABSENT, kidpal_story_stamp_serial(&v));
    TEST_ASSERT_EQUAL_INT(KSTORY_SEQ_ABSENT, kidpal_story_stamp_serial(NULL));

    /* pending_delete_id: 无 pending = 空串（非 NULL）; NULL 视图 = NULL。 */
    TEST_ASSERT_NOT_NULL(kidpal_story_pending_delete_id(&v));
    TEST_ASSERT_EQUAL_INT(0, (int)strlen(kidpal_story_pending_delete_id(&v)));
    TEST_ASSERT_NULL(kidpal_story_pending_delete_id(NULL));

    /* 分屏指示边界: 最大屏号 1-based 逐字。 */
    (void)snprintf(big_id, sizeof(big_id), "%d", 0);
    {
        char buf[64];
        TEST_ASSERT_EQUAL_INT(0, kidpal_story_list_screen_label(2, 3, buf, sizeof(buf)));
        TEST_ASSERT_EQUAL_STRING("第 3 屏 · 共 3 屏", buf);
    }
}
