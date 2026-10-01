/*
 * record_session_repair.test.c — builder 单元测试（TaskPackage
 * 261001-m4-k3-record-session-live-repair）。补冻结验收件 RSR-A1..A5 决策表未穷尽的
 * 分支/边界（[R1] 规则优先级与 ctx 非零语义 / [R2] 越界与幂等 / [R3] 非零与非列表态）。
 * 被测实现 = display/story_page.c 尾部 [R1]-[R3] 冻结缝（host runner 已编入）。
 */
#include <unity.h>
#include "../display/story_page.h"
#include "../display/kidpal_layer.h"

/* 冻结数值镜像（与 story_page.h / kidpal_render_node.h 等值; 漂移由既有冻结齿先红）。 */
#define UT_KP_PAGE_STORY 2 /* KP_PAGE_STORY */
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
