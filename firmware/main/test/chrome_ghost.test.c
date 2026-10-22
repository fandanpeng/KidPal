/*
 * Builder unit tests — TaskPackage 261022-p3-record-chrome-ghost-fix
 * （P3 记录会话 chrome 鬼影修复：语音会话收尾沿场景复位 + story -1 过渡窗键帽裁决）。
 *
 * 定位：**冻结验收 chrome_ghost.acceptance.c 的补强**，不是替代——冻结件 TEST 1/2 已
 * 钉行为本体（复位值出闭集 / 三态决策表 / 在途三子态零偏移 / 语音残留负扫描 / NULL
 * 防御）。本件只补冻结集未穷尽的分支与边界：
 *   1. 复位落点常量：复位必须落回头文件声明的默认常量且该常量出语音场景闭集——防
 *      「未来把默认值改回 0 = KVST_SCENE_LISTEN」的静默回归（复位齿只钉了「出闭集」）;
 *   2. scene 存真语义 = 任意 int 原样存取（闭集两端值 / 闭集外值 / 负值）;
 *   3. 过渡窗缝对**非 -1 的有效子态**一律回落（冻结件只测了 READER 一值），且在途
 *      子态表外的有效子态（LIST/STAMP/DELETE/BODY_UNAVAILABLE）同样零触碰;
 *   4. 帧分叉入参边界：READER / ORGANIZING 不吃 rec_sub（任意 rec_sub 同产物）;
 *      RECORDING 的 rec_sub 只做「0 / 非 0」二值分叉（负值同帧 B）。
 *
 * 纯逻辑、零芯片依赖（keybar.c 在 host runner 与设备树两侧同源）。
 * 用例内部均先写后读，不依赖整轮执行序（无顺序耦合）。
 */

#include <unity.h>

#include <stddef.h>
#include <string.h>

#include "../display/keybar.h"

/* 补充断言的统一失败文案前缀（本件自用，不与冻结件符号冲突）。 */
#define CG_BUILDER_RED "CG builder RED: "

/* -------------------------------------------------------------------------- *
 * TEST 1 — scene 单源复位落点与存真语义（补冻结件 TEST 1 未覆盖的边界）。
 * 复位落点必须是**常量**「非语音场景默认」且出闭集——我们要的是「没有任何语音场景
 * 在演」，不是「默认演聆听」；把默认值改回 0(=KVST_SCENE_LISTEN) 就等于没修。
 * ------------------------------------------------------------------------ */
TEST_CASE("chrome ghost builder: keybar scene reset lands on a constant out-of-set default",
          "[chrome_ghost][builder][linux]")
{
    /* 复位落点 = 头文件声明的默认常量, 且该常量出语音场景闭集（EQ-3 的静态前提:
     * 出闭集 -> 通用分支 kidpal_keybar_slots 落 scene 越界 -1 防御 -> 残留键帽画不出）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        KIDPAL_KEYBAR_SCENE_NONE < KVST_SCENE_LISTEN ||
        KIDPAL_KEYBAR_SCENE_NONE > KVST_SCENE_LOWBATT,
        CG_BUILDER_RED "场景默认常量落在语音场景闭集内——复位后通用分支照画语音残留"
        "键帽（默认值 = 0 等于没修）");

    kidpal_keybar_scene_set(KVST_SCENE_THINK);
    kidpal_keybar_scene_reset();
    TEST_ASSERT_EQUAL_INT_MESSAGE(KIDPAL_KEYBAR_SCENE_NONE, kidpal_keybar_scene_get(),
        CG_BUILDER_RED "reset 未落回声明常量（复位实现与头声明漂移）");

    /* 存真语义：任意 int 原样存取（闭集两端值 / 闭集外值 / 负值）。 */
    kidpal_keybar_scene_set(KVST_SCENE_LOWBATT);
    TEST_ASSERT_EQUAL_INT_MESSAGE(KVST_SCENE_LOWBATT, kidpal_keybar_scene_get(),
        CG_BUILDER_RED "set(闭集右端 LOWBATT) 未原样存真");
    kidpal_keybar_scene_set(42);
    TEST_ASSERT_EQUAL_INT_MESSAGE(42, kidpal_keybar_scene_get(),
        CG_BUILDER_RED "set(42) 未原样存真——场景单源被夹到闭集内/被过滤（契约: 任意 "
        "int 原样存储, 越界值由 kidpal_keybar_slots 消费侧防御）");
    kidpal_keybar_scene_set(-7);
    TEST_ASSERT_EQUAL_INT_MESSAGE(-7, kidpal_keybar_scene_get(),
        CG_BUILDER_RED "set(-7) 未原样存真（负值同样原样存储）");

    /* reset 幂等：多次复位恒回同一默认值（复位必须是常量落点）。 */
    kidpal_keybar_scene_reset();
    {
        const int once = kidpal_keybar_scene_get();
        kidpal_keybar_scene_reset();
        kidpal_keybar_scene_reset();
        TEST_ASSERT_EQUAL_INT_MESSAGE(once, kidpal_keybar_scene_get(),
            CG_BUILDER_RED "reset 非幂等——连调后返回值漂移");
    }

    /* 收尾：复位后仍可被 set 驱动（复位不得冻结写沿）, 并回到默认态交后续用例。 */
    kidpal_keybar_scene_set(KVST_SCENE_TALK);
    TEST_ASSERT_EQUAL_INT_MESSAGE(KVST_SCENE_TALK, kidpal_keybar_scene_get(),
        CG_BUILDER_RED "复位后 set 不生效——复位冻结了场景写沿");
    kidpal_keybar_scene_reset();
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — 过渡窗缝对有效子态一律回落（冻结件只测了 READER 一值）。
 * 「过渡窗」的判据是 story_sub == -1；只要子态有效（含 LIST/STAMP/DELETE/
 * BODY_UNAVAILABLE 这些不在在途集的有效子态、乃至越界值），裁决缝必须零触碰回落，
 * 把裁决权交还调用方既有路径（既有子态表 / 查字窗 / 通用分支）。
 * ------------------------------------------------------------------------ */
TEST_CASE("chrome ghost builder: transition seam falls back for every valid story substate",
          "[chrome_ghost][builder][linux]")
{
    static const int valid_subs[] = {
        KSTORY_PAGE_LIST, KSTORY_PAGE_READER, KSTORY_PAGE_RECORDING,
        KSTORY_PAGE_STAMP, KSTORY_PAGE_DELETE, KSTORY_PAGE_BODY_UNAVAILABLE,
        KSTORY_PAGE_ORGANIZING
    };
    const char *sentinel_l = "sentinel-l";
    const char *sentinel_r = "sentinel-r";
    int iu;

    for (iu = 0; iu < (int)(sizeof(valid_subs) / sizeof(valid_subs[0])); iu++) {
        const char *left = sentinel_l;
        const char *right = sentinel_r;
        /* 即便「最后已知子态」在途，只要子态本身有效就不是过渡窗——不接管。 */
        const int rc = kidpal_keybar_story_transition_slots(
            valid_subs[iu], KSTORY_PAGE_RECORDING, 0, &left, &right);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, rc,
            CG_BUILDER_RED "有效子态被判成过渡窗（返回非 0）——既有子态表 / 查字窗 / "
            "通用分支路径会被本缝抢走");
        TEST_ASSERT_TRUE_MESSAGE(left == sentinel_l && right == sentinel_r,
            CG_BUILDER_RED "回落路径改写了 out_*——回落语义必须零触碰");
    }

    /* 越界（非枚举值）的子态同样回落：不是过渡窗就不接管。 */
    {
        const char *left = sentinel_l;
        const char *right = sentinel_r;
        TEST_ASSERT_EQUAL_INT_MESSAGE(0,
            kidpal_keybar_story_transition_slots(99, KSTORY_PAGE_READER, 0,
                                                 &left, &right),
            CG_BUILDER_RED "越界子态(99) 被判成过渡窗");
        TEST_ASSERT_TRUE_MESSAGE(left == sentinel_l && right == sentinel_r,
            CG_BUILDER_RED "越界子态回落路径改写了 out_*");
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — 帧分叉入参边界（rec_sub 只对 RECORDING 的二值分叉有意义）。
 * READER / ORGANIZING 的产物必须与 rec_sub 无关（纯函数无隐藏状态、同入参恒同出）;
 * RECORDING 的 rec_sub 语义 = 「0 / 非 0」，负值与正非零同帧（冻结件只测了 0 / 1）。
 * ------------------------------------------------------------------------ */
TEST_CASE("chrome ghost builder: transition seam rec_sub boundary (ignored off RECORDING)",
          "[chrome_ghost][builder][linux]")
{
    const char *l_a = NULL, *r_a = NULL;
    const char *ref_l = NULL, *ref_r = NULL;
    static const int recs[] = { 0, 1, -3, 2147483647 };
    int ir;

    /* READER：rec_sub 全值域同产物 == 既有冻结子态表。 */
    for (ir = 0; ir < (int)(sizeof(recs) / sizeof(recs[0])); ir++) {
        l_a = NULL; r_a = NULL;
        TEST_ASSERT_EQUAL_INT_MESSAGE(1,
            kidpal_keybar_story_transition_slots(-1, KSTORY_PAGE_READER, recs[ir],
                                                 &l_a, &r_a),
            CG_BUILDER_RED "过渡窗 + READER 未裁决命中");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0,
            kidpal_keybar_story_slots(KSTORY_PAGE_READER, &ref_l, &ref_r),
            CG_BUILDER_RED "参照失效: kidpal_keybar_story_slots(READER) 失败");
        TEST_ASSERT_TRUE_MESSAGE(l_a != NULL && r_a != NULL &&
                                 ref_l != NULL && ref_r != NULL &&
                                 strcmp(l_a, ref_l) == 0 && strcmp(r_a, ref_r) == 0,
            CG_BUILDER_RED "READER 产物随 rec_sub 漂移——rec_sub 对非 RECORDING 子态必须"
            "零影响（纯函数无隐藏状态）");
    }

    /* ORGANIZING：rec_sub 全值域同产物 == 既有冻结子态表（双槽留空）。 */
    for (ir = 0; ir < (int)(sizeof(recs) / sizeof(recs[0])); ir++) {
        l_a = NULL; r_a = NULL;
        TEST_ASSERT_EQUAL_INT_MESSAGE(1,
            kidpal_keybar_story_transition_slots(-1, KSTORY_PAGE_ORGANIZING, recs[ir],
                                                 &l_a, &r_a),
            CG_BUILDER_RED "过渡窗 + ORGANIZING 未裁决命中");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0,
            kidpal_keybar_story_slots(KSTORY_PAGE_ORGANIZING, &ref_l, &ref_r),
            CG_BUILDER_RED "参照失效: kidpal_keybar_story_slots(ORGANIZING) 失败");
        TEST_ASSERT_TRUE_MESSAGE(l_a != NULL && r_a != NULL &&
                                 ref_l != NULL && ref_r != NULL &&
                                 strcmp(l_a, ref_l) == 0 && strcmp(r_a, ref_r) == 0,
            CG_BUILDER_RED "ORGANIZING 产物随 rec_sub 漂移——整理态双槽留空与 rec_sub 无关");
    }

    /* RECORDING：rec_sub 只做 0 / 非 0 二值分叉——帧 A 与帧 B 各自对齐既有帧表,
     * 且负值（非 0）落帧 B 而非未定义行为。 */
    for (ir = 0; ir < (int)(sizeof(recs) / sizeof(recs[0])); ir++) {
        const int frame = (recs[ir] == 0) ? 0 : 1; /* 单源即 story_rec_slots 的分叉语义 */
        l_a = NULL; r_a = NULL;
        TEST_ASSERT_EQUAL_INT_MESSAGE(1,
            kidpal_keybar_story_transition_slots(-1, KSTORY_PAGE_RECORDING, recs[ir],
                                                 &l_a, &r_a),
            CG_BUILDER_RED "过渡窗 + RECORDING 未裁决命中");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0,
            kidpal_keybar_story_rec_slots(frame, &ref_l, &ref_r),
            CG_BUILDER_RED "参照失效: kidpal_keybar_story_rec_slots(frame) 失败");
        TEST_ASSERT_TRUE_MESSAGE(l_a != NULL && r_a != NULL &&
                                 ref_l != NULL && ref_r != NULL &&
                                 strcmp(l_a, ref_l) == 0 && strcmp(r_a, ref_r) == 0,
            CG_BUILDER_RED "过渡窗 RECORDING 的 rec_sub 分叉未与 story_rec_slots 同源"
            "（rec_sub 语义必须是 0 / 非 0 二值, 负值同帧 B）");
    }
}
