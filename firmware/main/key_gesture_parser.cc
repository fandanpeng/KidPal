#include "key_gesture_parser.h"

/*
 * KEY/BOOT 全局按键手势解析实现（纯逻辑，显式毫秒时间戳注入，无真实 timer）。
 *
 * 消抖/单击/双击/长按语义逐条对应冻结验收文件
 * firmware/main/test/session_logic.acceptance.c 头注释钉定的解析契约
 * （首边沿永远接收、debounce 折叠抖动、click_max 分界长按、double_gap 窗内
 * 双击/窗外先冲刷旧候选、poll 冲刷在挂单击）；全局路由表对应 spec §4.0
 * 全局按键手势表。PWR（SES-E3）为硬件电源键固件不可读：键位枚举只有
 * KEY/BOOT，任何越界键位一律拒收且解析器状态零污染。
 */

void key_gesture_parser_init(key_gesture_parser_t *p,
                             const key_gesture_config_t *cfg)
{
    p->cfg = *cfg;
    /* 「尚无边沿」标记：-1 而非 ts=0，保证首条 raw 边沿永远接收。 */
    p->last_stable_pressed = -1;
    p->last_edge_ms = 0u;
    p->press_start_ms = 0u;
    p->pending_click = 0;
    p->pending_click_ts = 0u;
    p->out = KEY_GESTURE_NONE;
}

key_gesture_t key_gesture_parser_feed(key_gesture_parser_t *p,
                                      key_id_t key, uint32_t ts_ms,
                                      int pressed)
{
    /* 越界键位（>=KEY_ID_COUNT，含模拟 PWR 的 id）：直接拒收，状态零污染。 */
    if ((uint32_t)key >= (uint32_t)KEY_ID_COUNT) {
        return KEY_GESTURE_NONE;
    }

    /* 消抖：与上一条「已接收」边沿相距 <= debounce_ms 的 raw 边沿忽略。
     * 首条边沿（last_stable_pressed == -1）永远接收。 */
    if (p->last_stable_pressed >= 0 &&
        (uint32_t)(ts_ms - p->last_edge_ms) <= p->cfg.debounce_ms) {
        p->out = KEY_GESTURE_NONE;
        return KEY_GESTURE_NONE;
    }

    p->last_edge_ms = ts_ms;
    p->last_stable_pressed = (pressed != 0) ? 1 : 0;

    if (pressed) {
        /* 按下边沿：只记按下起点，不发射手势。 */
        p->press_start_ms = ts_ms;
        p->out = KEY_GESTURE_NONE;
        return KEY_GESTURE_NONE;
    }

    /* 抬起边沿：先判长按，再判双击，再判过期冲刷，最后落为在挂单击候选。 */
    uint32_t duration = ts_ms - p->press_start_ms;
    if (duration >= p->cfg.click_max_ms) {
        /* 长按：emit 即清在挂候选，不留幽灵单击。 */
        p->pending_click = 0;
        p->out = KEY_GESTURE_LONG_PRESS;
        return KEY_GESTURE_LONG_PRESS;
    }
    if (p->pending_click &&
        (uint32_t)(ts_ms - p->pending_click_ts) <= p->cfg.double_gap_ms) {
        /* 双击：第二次抬起落在双击窗内，直接发 DOUBLE_CLICK。 */
        p->pending_click = 0;
        p->out = KEY_GESTURE_DOUBLE_CLICK;
        return KEY_GESTURE_DOUBLE_CLICK;
    }
    if (p->pending_click) {
        /* 在挂候选已过期（窗外）：先冲刷为 CLICK（本次返回），
         * 本次抬起成为新在挂候选。 */
        p->pending_click_ts = ts_ms;
        p->out = KEY_GESTURE_CLICK;
        return KEY_GESTURE_CLICK;
    }
    /* 无在挂候选：本次单击进入在挂（等待可能的第二击 / poll 冲刷）。 */
    p->pending_click = 1;
    p->pending_click_ts = ts_ms;
    p->out = KEY_GESTURE_NONE;
    return KEY_GESTURE_NONE;
}

key_gesture_t key_gesture_parser_poll(key_gesture_parser_t *p, uint32_t now_ms)
{
    if (p->pending_click &&
        (uint32_t)(now_ms - p->pending_click_ts) > p->cfg.double_gap_ms) {
        p->pending_click = 0;
        p->out = KEY_GESTURE_CLICK;
        return KEY_GESTURE_CLICK;
    }
    p->out = KEY_GESTURE_NONE;
    return KEY_GESTURE_NONE;
}

page_id_t key_page_next(page_id_t pg)
{
    /* 非法页（含越界 id）回 P1。 */
    if ((uint32_t)pg >= (uint32_t)PAGE_COUNT) {
        return PAGE_P1;
    }
    return (page_id_t)(((uint32_t)pg + 1u) % (uint32_t)PAGE_COUNT);
}

global_action_t key_route_global(key_id_t key, key_gesture_t g)
{
    /* 越界键位（含 PWR）：一律无动作。 */
    if ((uint32_t)key >= (uint32_t)KEY_ID_COUNT) {
        return GLOBAL_ACT_NONE;
    }
    if (key == KEY_ID_KEY) {
        switch (g) {
        case KEY_GESTURE_CLICK:
            return GLOBAL_ACT_SESSION_TOGGLE; /* 启动/结束会话；打断停播由状态机承载 */
        case KEY_GESTURE_LONG_PRESS:
            return GLOBAL_ACT_HOME;           /* 回首页，所有页面一致 */
        default:
            return GLOBAL_ACT_NONE;           /* KEY 双击全局层无语义 */
        }
    }
    /* KEY_ID_BOOT */
    if (g == KEY_GESTURE_CLICK) {
        return GLOBAL_ACT_PAGE_NEXT;          /* P1→P2→P3→P4 循环翻页 */
    }
    return GLOBAL_ACT_NONE;                   /* BOOT 长按/双击无全局语义 */
}

global_action_t key_route_layer(key_id_t key, key_gesture_t g, int overlay_top)
{
    /* 越界键位（含 PWR）：一律无动作（覆盖层/基层两列一致，零污染）。 */
    if ((uint32_t)key >= (uint32_t)KEY_ID_COUNT) {
        return GLOBAL_ACT_NONE;
    }

    if (overlay_top != 0) {
        /* 覆盖层顶层列（v1.39 覆盖层 BOOT 隔离 + v1.36 返回上一层）。 */
        switch (g) {
        case KEY_GESTURE_CLICK:
            /* BOOT 单击 = 关顶层回原页、不产翻页动作（隔离口径）；KEY 单击 = 会话主轴
             * 零回归（提醒弹窗 KEY 关闭由 dispatch 先交顶层裁决函数承载）。 */
            if (key == KEY_ID_KEY) {
                return GLOBAL_ACT_SESSION_TOGGLE;
            }
            return GLOBAL_ACT_OVERLAY_CLOSE;
        case KEY_GESTURE_LONG_PRESS:
            /* KEY 长按 = 返回上一层（覆盖层 = 关层回原页）；BOOT 长按覆盖层列无语义。 */
            if (key == KEY_ID_KEY) {
                return GLOBAL_ACT_OVERLAY_CLOSE;
            }
            return GLOBAL_ACT_NONE;
        default:
            return GLOBAL_ACT_NONE;           /* KEY/BOOT 双击族覆盖层列 NONE */
        }
    }

    /* 基层列（overlay_top == 0）: 与 key_route_global 全表等价——主页面路径零回归。
     * key_route_global 本体零改动，其表为基层列的实现载体；KEY/BOOT 两键位经各自字面
     * 调用显式喂进全局路由表（KR-A3 设备侧接线入口的回程自证——base 列即 key_route_global
     * 的别名，非再建一套基层手势语义）。 */
    switch (key) {
    case KEY_ID_KEY:
        return key_route_global(KEY_ID_KEY, g);
    case KEY_ID_BOOT:
        return key_route_global(KEY_ID_BOOT, g);
    default:
        return GLOBAL_ACT_NONE; /* 越界键位已在上方拒收，此处不可达 */
    }
}
