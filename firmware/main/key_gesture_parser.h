#ifndef KEY_GESTURE_PARSER_H
#define KEY_GESTURE_PARSER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * KEY/BOOT 全局按键手势解析（纯逻辑）— KidPal 包 3（TaskPackage 260829-session-logic）。
 *
 * 行为语义权威：docs/spec.md §4.0 全局按键手势表 + §5.3 按键定义。
 * 职责分离：KEY 主司会话（短按启动/结束会话、长按回首页）、BOOT 主司翻页
 * （短按 P1→P2→P3 三页循环）。KEY 长按回首页「所有页面一致」由路由函数
 * 无页面入参表达（全局层、context-free）。
 *
 * 纯逻辑约束：单实例单键（每个物理键一个 key_gesture_parser_t 实例）；
 * 时间全部为毫秒时间戳显式注入（无真实 timer）；不依赖芯片外设 / FreeRTOS /
 * ESP 组件头；linux host 与设备 target 均可编译。
 *
 * PWR（SES-E3）：PWR 为硬件电源键、固件不可读——键位枚举只有 KEY/BOOT，
 * 无 PWR。PWR 永不入手势解析器、全局路由层与会话状态机；任何键位 id
 * >= KEY_ID_COUNT 被拒收：feed() 返回 NONE 且解析器状态零污染，
 * route() 返回 GLOBAL_ACT_NONE。这是纯逻辑接缝上可表达的最大 PWR 断言面。
 */

/* 键位枚举：只有 KEY/BOOT，无 PWR（见上 PWR 注记）。 */
typedef enum {
    KEY_ID_KEY = 0,
    KEY_ID_BOOT,
    KEY_ID_COUNT
} key_id_t;

typedef enum {
    KEY_GESTURE_NONE = 0,
    KEY_GESTURE_CLICK,
    KEY_GESTURE_DOUBLE_CLICK,
    KEY_GESTURE_LONG_PRESS
} key_gesture_t;

/* 三页循环翻页的页 id（页序纯逻辑在本包；跨页渲染/端到端整合由包 9b 兜底，SES-E2）。
 * 2026-09-17 M2-K2 撤 P4 常驻页：第 4 页位整值撤出（不留占位、不复用编号），
 * PAGE_COUNT 随枚举自动 4→3，页序与 spec P1→P2→P3（requirements §2.4 3 页循环）逐项对齐。 */
typedef enum {
    PAGE_P1 = 0,
    PAGE_P2,
    PAGE_P3,
    PAGE_COUNT
} page_id_t;

typedef struct {
    uint32_t debounce_ms, click_max_ms, double_gap_ms;
} key_gesture_config_t;

/* 单键解析器实例（plain C struct，无动态分配）。
 * 语义：
 *   -- 首条 raw 边沿（此前无已接收边沿）永远接收——init 以
 *      last_stable_pressed = -1 标记「尚无边沿」（不是 ts=0）；
 *   -- 与上一条「已接收」边沿相距 <= debounce_ms 的 raw 边沿被忽略（消抖）；
 *   -- 抬起持续时长 >= click_max_ms -> LONG_PRESS，否则为 CLICK 候选；
 *   -- CLICK 候选的抬起时刻与在挂候选相距 <= double_gap_ms -> DOUBLE_CLICK
 *      （并清掉在挂候选）；任何 LONG_PRESS / DOUBLE_CLICK 发射同样清在挂候选；
 *   -- 双击窗过期后的抬起先把过期在挂候选冲刷为 CLICK（feed 返回值），
 *      然后自身成为新在挂候选；
 *   -- poll(p, now) 在 now - pending_ts > double_gap_ms 时冲刷在挂 CLICK。 */
typedef struct {
    key_gesture_config_t cfg;
    int last_stable_pressed;      /* 1=按下 0=抬起 -1=尚无边沿（init 标记） */
    uint32_t last_edge_ms, press_start_ms;
    int pending_click;
    uint32_t pending_click_ts;
    key_gesture_t out;
} key_gesture_parser_t;

/* 以 cfg 初始化解析器：out=NONE、无在挂候选、标记「尚无边沿」。 */
void key_gesture_parser_init(key_gesture_parser_t *p,
                             const key_gesture_config_t *cfg);

/* 注入一条 raw 电平边沿（pressed: 1=down 0=up，ts_ms 毫秒时间戳）。
 * 越界键位（>=KEY_ID_COUNT，如模拟 PWR 的 id）返回 NONE 且不改动任何状态。 */
key_gesture_t key_gesture_parser_feed(key_gesture_parser_t *p,
                                      key_id_t key, uint32_t ts_ms,
                                      int pressed);

/* 周期轮询：在挂 CLICK 超过双击窗后在此冲刷发射。 */
key_gesture_t key_gesture_parser_poll(key_gesture_parser_t *p,
                                      uint32_t now_ms);

/* 页序纯逻辑：P1->P2->P3->P1 回绕（基数 (pg+1) % PAGE_COUNT 同源）；非法页（含越界 id）回 PAGE_P1。 */
page_id_t key_page_next(page_id_t pg);

typedef enum {
    GLOBAL_ACT_NONE = 0,
    GLOBAL_ACT_SESSION_TOGGLE,
    GLOBAL_ACT_HOME,
    GLOBAL_ACT_PAGE_NEXT,
    /* 覆盖层关层回原页（v1.39 隔离 + v1.36 返回上一层 的统一路由动作）。
     * append-only 追加位：既有四值零偏移（TaskPackage 260915-m1-k1a-key-layer-dispatch,
     * key_layer_dispatch.acceptance.c seam [1] 钉值 = 4）。 */
    GLOBAL_ACT_OVERLAY_CLOSE = 4,
    /* 查字窗 KEY 短按 = 重播笔顺（含打断当前播报重演；S-F3.3 AC1）。
     * append-only 追加位：既有五值零偏移（TaskPackage 260917-m2-k3-char-window-overlay,
     * char_window_overlay.acceptance.c seam [2] 钉值 = 5）。 */
    GLOBAL_ACT_CHAR_REPLAY = 5
} global_action_t;

/* 全局路由表（spec §4.0 全局按键手势表，仅全局层——无页面参数、context-free）：
 *   KEY  CLICK        -> GLOBAL_ACT_SESSION_TOGGLE（启动/结束会话；
 *                        会话中打断停播由状态机 SES_EVT_KEY_SHORT_PRESS 承载）
 *   KEY  DOUBLE_CLICK -> GLOBAL_ACT_NONE（KEY 双击全局层无语义）
 *   KEY  LONG_PRESS   -> GLOBAL_ACT_HOME（全局一致）
 *   BOOT CLICK        -> GLOBAL_ACT_PAGE_NEXT（P1→P2→P3 三页循环；
 *                        对话中 BOOT 结束会话由状态机 SES_EVT_BOOT_SHORT_PRESS
 *                        独立承载，路由层与状态机各取所需）
 *   BOOT LONG_PRESS / DOUBLE_CLICK -> GLOBAL_ACT_NONE
 *   任何越界键位（>=KEY_ID_COUNT，如 PWR）-> GLOBAL_ACT_NONE */
global_action_t key_route_global(key_id_t key, key_gesture_t g);

/* layer-aware 全局路由表（TaskPackage 260915-m1-k1a-key-layer-dispatch）：
 * overlay_top 由 dispatch 层归一化——2026-09-19 留痕修订（TaskPackage
 * 260917-m2-k3-char-window-overlay, seam [2]）后其语义**扩为前台层归一化值**：
 *   0 = 基层 / 配网 / 无前台（原 K1a 归一化 0 是其子集）;
 *   非 0 = 前台覆盖层层值（KIDPAL_LAYER_*，取 kidpal_render_node_front_layer()）。
 * 路由表自身保持 context-free 纯逻辑（key_gesture_parser.cc 单源）：
 *   overlay_top == KIDPAL_LAYER_CHAR_WINDOW（查字窗列，本包新增）:
 *     KEY  CLICK      -> GLOBAL_ACT_CHAR_REPLAY（重播笔顺——非 SESSION_TOGGLE）
 *     KEY  LONG_PRESS -> GLOBAL_ACT_OVERLAY_CLOSE（关窗回原页）
 *     BOOT CLICK      -> GLOBAL_ACT_OVERLAY_CLOSE（v1.39 隔离 = 关窗回原页、不产翻页）
 *     其余（双击族 / BOOT 长按）-> GLOBAL_ACT_NONE
 *   其它非 0 值（通用覆盖层列，K1a 原样）:
 *     KEY  CLICK      -> GLOBAL_ACT_SESSION_TOGGLE（会话主轴零回归；提醒弹窗 KEY 关闭
 *                        由 dispatch 先交顶层裁决函数，裁决消费即不走路由）
 *     KEY  LONG_PRESS -> GLOBAL_ACT_OVERLAY_CLOSE（返回上一层 = 关层回原页）
 *     BOOT CLICK      -> GLOBAL_ACT_OVERLAY_CLOSE（v1.39 隔离 = 关顶层回原页、不产翻页）
 *     其余（双击族 / BOOT 长按）-> GLOBAL_ACT_NONE
 *   overlay_top == 0（主页面基层）: 与 key_route_global(key, g) 全表等价
 *     （主页面 BOOT 单击翻页 / KEY 长按回首页，§2.3 主页面矩阵行零回归）。
 *   越界键位（>= KEY_ID_COUNT，含 PWR）-> GLOBAL_ACT_NONE。
 * key_route_global 本体零改动（基层列实现载体），主页面路径零回归。 */
global_action_t key_route_layer(key_id_t key, key_gesture_t g, int overlay_top);

#ifdef __cplusplus
}
#endif

#endif /* KEY_GESTURE_PARSER_H */
