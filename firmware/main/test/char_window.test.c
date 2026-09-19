/*
 * char_window.test.c — KidPal 查字窗（M2-K3 页→窗改造）builder 单元测试。
 *
 * 定位：**补强面**，不替代冻结验收（firmware/main/test/char_window_overlay.acceptance.c
 * 的 7 个 TEST_CASE 是判定权威）。本文件只覆盖冻结齿未穷尽的**边界与分支**：
 *   - kidpal_char_window_idle_step: dt=0 空拍 / 单拍跳跃超时 / 阈值下侧 1ms /
 *     超时态按键归零优先 / NULL 在按键分支前防御;
 *   - kidpal_layer_pop_transients: 空栈 / CHAR_WINDOW + PROVISION 共存（配网零触碰）/
 *     非栈成员检层防御;
 *   - key_route_layer: 查字窗列与通用覆盖层列的列判别隔离（防塌回）/ 越界键位 /
 *     双击族与 NONE 手势。
 *
 * 被测实现均为纯逻辑、host 同源：display/char_window.c（本包新增）、
 * display/kidpal_layer.c、key_gesture_parser.cc（均已在 HOST_ACCEPTANCE_SRCS）。
 * 注册位见 firmware/main/test/host_app/main/CMakeLists.txt（置于 host_infra_smoke 之前）。
 */
#include <unity.h>

#include <string.h>

#include "../key_gesture_parser.h"
#include "../display/kidpal_layer.h"
#include "../display/char_window.h"

/* -------------------------------------------------------------------------- *
 * 1) 驻留计时边界（M2K3-A3 / seam [3]）
 * ------------------------------------------------------------------------ */
TEST_CASE("char_window 单元: 驻留计时边界-空拍/跳跃超时/复位优先/NULL防御",
          "[char_window][unit][timer][M2K3-A3]")
{
    unsigned long idle;
    int r;

    /* dt = 0：空拍不推进、不关窗（幂等）。 */
    idle = 12345UL;
    r = kidpal_char_window_idle_step(&idle, 0UL, 0);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && idle == 12345UL,
                             "dt=0 空拍不应推进 idle 也不应关窗");

    /* 单拍跳跃即越过上限（采样稀疏也必须关窗，不要求恰好命中阈值）。 */
    idle = 0UL;
    r = kidpal_char_window_idle_step(&idle, KIDPAL_CHAR_WINDOW_IDLE_MS, 0);
    TEST_ASSERT_TRUE_MESSAGE(r == 1 && idle == KIDPAL_CHAR_WINDOW_IDLE_MS,
                             "单拍即达 5 分钟上限应返回关窗 (1)");

    /* 阈值下侧 1ms：仍驻留。 */
    idle = KIDPAL_CHAR_WINDOW_IDLE_MS - 2UL;
    r = kidpal_char_window_idle_step(&idle, 1UL, 0);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && idle == KIDPAL_CHAR_WINDOW_IDLE_MS - 1UL,
                             "差 1ms 到上限不应关窗");

    /* 已超时态下按键：归零优先于超时（按键本身是操作，先复位、本次不关窗）。 */
    idle = KIDPAL_CHAR_WINDOW_IDLE_MS + 5000UL;
    r = kidpal_char_window_idle_step(&idle, 1000UL, 1);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && idle == 0UL,
                             "超时态按键应把计时归零且本次不关窗（复位优先）");

    /* NULL 防御在按键分支之前（key_pressed 非 0 也走 -1，从不为 UB）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_window_idle_step(NULL, 1000UL, 0) == -1 &&
                                 kidpal_char_window_idle_step(NULL, 0UL, 1) == -1,
                             "NULL idle 指针必须防御 (-1)");
}

/* -------------------------------------------------------------------------- *
 * 2) 查字窗层栈边界（M2K3-A1 / seam [1]）
 * ------------------------------------------------------------------------ */
TEST_CASE("char_window 单元: 查字窗层栈边界-空栈清层/配网共存零触碰/检层防御",
          "[char_window][unit][layer][M2K3-A1]")
{
    kidpal_layer_stack_t st;

    /* 空栈清层 = 0（无 transient 可出，不伪装成功）。 */
    memset(&st, 0, sizeof(st));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_pop_transients(&st) == 0,
                             "空栈清层应返回 0");

    /* CHAR_WINDOW + PROVISION 且无基层：只出查字窗一层，配网层原位保留、仍为 top。 */
    memset(&st, 0, sizeof(st));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_CHAR_WINDOW) == 0 &&
                                 kidpal_layer_push(&st, KIDPAL_LAYER_PROVISION) == 0,
                             "fixture: CHAR_WINDOW + PROVISION 入栈");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_pop_transients(&st) == 1 &&
                                 kidpal_layer_top(&st) == KIDPAL_LAYER_PROVISION &&
                                 kidpal_layer_has(&st, KIDPAL_LAYER_CHAR_WINDOW) == 0,
                             "清层只应出查字窗，配网层零触碰");

    /* 非栈成员（CHAT / NONE）检层防御 -1。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_has(&st, KIDPAL_LAYER_CHAT) == -1 &&
                                 kidpal_layer_has(&st, KIDPAL_LAYER_NONE) == -1,
                             "非栈成员检层须 -1");
}

/* -------------------------------------------------------------------------- *
 * 3) 查字窗列路由边界（M2K3-A2 / seam [2]）
 * ------------------------------------------------------------------------ */
TEST_CASE("char_window 单元: 查字窗列路由边界-列判别隔离/越界键位/双击族",
          "[char_window][unit][route][M2K3-A2]")
{
    /* 列判别隔离：CHAR_WINDOW 值必须走查字窗列，不可塌回通用覆盖层列；
     * 同表下 REMINDER 仍走通用列（SESSION_TOGGLE）——防「改法与重心错位」假绿。 */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_CLICK, KIDPAL_LAYER_CHAR_WINDOW) ==
                (int)GLOBAL_ACT_CHAR_REPLAY &&
            (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_CLICK, KIDPAL_LAYER_REMINDER) ==
                (int)GLOBAL_ACT_SESSION_TOGGLE,
        "查字窗列 KEY 短按 = CHAR_REPLAY；提醒覆盖层列仍 = SESSION_TOGGLE");

    /* 越界键位在查字窗列同样拒收（零污染）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer((key_id_t)99, KEY_GESTURE_CLICK, KIDPAL_LAYER_CHAR_WINDOW) ==
                (int)GLOBAL_ACT_NONE &&
            (int)key_route_layer((key_id_t)99, KEY_GESTURE_LONG_PRESS,
                                 KIDPAL_LAYER_CHAR_WINDOW) == (int)GLOBAL_ACT_NONE,
        "查字窗列越界键位须 NONE");

    /* 双击族 / NONE 手势在查字窗列 NONE。 */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK,
                             KIDPAL_LAYER_CHAR_WINDOW) == (int)GLOBAL_ACT_NONE &&
            (int)key_route_layer(KEY_ID_BOOT, KEY_GESTURE_DOUBLE_CLICK,
                                 KIDPAL_LAYER_CHAR_WINDOW) == (int)GLOBAL_ACT_NONE &&
            (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_NONE,
                                 KIDPAL_LAYER_CHAR_WINDOW) == (int)GLOBAL_ACT_NONE,
        "查字窗列双击族 / NONE 手势须 NONE");
}

/* -------------------------------------------------------------------------- *
 * 4) 1s tick 节奏下的驻留累计（M2K3-A3 **驱动半** seam [9] 所消费的纯逻辑契约）
 *    设备侧驱动以 RN_TICK_MS=1000 每拍喂 kidpal_char_window_idle_step；冻结 TEST 4 走
 *    稀疏单拍，本齿复现真实 1s 节奏，钉「满 300 拍（=5 分钟）才关窗、任一拍按键即重计
 *    满 5 分钟」——即「窗一开不得首拍即关」与「计时随任意按键重计」在节奏面成立。
 * ------------------------------------------------------------------------ */
TEST_CASE("char_window 单元: 1s tick 节奏累计-满 300 拍关窗/中途按键重计",
          "[char_window][unit][timer][M2K3-A3]")
{
    const unsigned long tick = 1000UL;
    unsigned long idle = 0UL;
    int i;
    int closed_at = -1;

    /* 连续空拍（累计器初值 0，模拟驱动 tick 步进）：第 300 拍（i=299, idle=300000）
     * 才关窗——窗打开后首拍不得因累计越限而立即误关（seam [9]c 门控契约的纯逻辑侧）。 */
    for (i = 0; i < 300; i++) {
        if (kidpal_char_window_idle_step(&idle, tick, 0) == 1) {
            closed_at = i;
            break;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(closed_at == 299 && idle == 300000UL,
        "1s 节奏下应在第 300 拍（i=299, idle=300000）才关窗——首拍不得误关");

    /* 满 299 拍后按键：归零且本次不关窗（v1.43 计时随任意按键重计）。 */
    idle = 0UL;
    for (i = 0; i < 299; i++) {
        (void)kidpal_char_window_idle_step(&idle, tick, 0);
    }
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_char_window_idle_step(&idle, tick, 1) == 0 && idle == 0UL,
        "第 299 拍按键应归零且不关窗（计时随任意按键重计）");

    /* 重计后重新走满 300 拍才关窗（复位不是一次性）。 */
    closed_at = -1;
    for (i = 0; i < 300; i++) {
        if (kidpal_char_window_idle_step(&idle, tick, 0) == 1) {
            closed_at = i;
            break;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(closed_at == 299 && idle == 300000UL,
        "按键重计后仍须满 300 拍整数才关窗");
}