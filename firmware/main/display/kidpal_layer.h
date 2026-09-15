/*
 * kidpal_layer.h — KidPal 前台 layer 栈管理单一权威 (TaskPackage
 * 260911-screen-unification, PKG-2 归一屏)。
 *
 * 语义与数值码以冻结验收 firmware/main/test/screen_unification.acceptance.c
 * 文件头 FROZEN OFFLINE CONTRACT 为准，不得偏移；本头声明与该文件 fallback 块
 * 逐字一致（C linkage）。纯逻辑、无 chip/FreeRTOS/ESP-component/cJSON 依赖 ->
 * 可编入 host_app linux target 与设备树两侧（host 强符号覆盖 weak 哨兵，
 * 设备侧由 kidpal_render_node.cc 作为生产消费者）。实现见 kidpal_layer.c。
 *
 * 本模块 = 前台参与者优先级栈合并体：
 *   (1) 前台参与者优先级栈（本地浏览页 / 服务端推送页 / 语音轮 / 提醒弹窗 / 配网）；
 *   (2) 聊天 layer 常驻性 + 驻留清除时机（OQ3=c）+ 聊天内容落点。
 * 不新建独立状态机对象（PROPOSAL §4）：layer 栈本身即前台 owner 的单一权威。
 */
#ifndef KIDPAL_LAYER_H
#define KIDPAL_LAYER_H

#ifdef __cplusplus
extern "C" {
#endif

/* --- 常量（冻结数值码，不得偏移） --- */
#define KIDPAL_LAYER_STACK_MAX    8
#define KIDPAL_LAYER_UNIMPLEMENTED (-2)
#define KIDPAL_LAYER_NONE         0
#define KIDPAL_LAYER_CHAT         1
#define KIDPAL_LAYER_LOCAL_PAGE   2
#define KIDPAL_LAYER_VOICE        3
#define KIDPAL_LAYER_SERVER_PAGE  4
#define KIDPAL_LAYER_REMINDER     5
#define KIDPAL_LAYER_PROVISION    6

/* 前台优先级 = 数值越大越靠前：配网 > 提醒 > 服务端推送页 > 语音轮 > 本地浏览页。 */

#define KIDPAL_LAYER_EVT_ANSWER_END  0
#define KIDPAL_LAYER_EVT_PAGE_SWITCH 1
#define KIDPAL_LAYER_EVT_NEW_VOICE   2
#define KIDPAL_LAYER_EVT_KEY_PRESS   3

/* --- 类型形状（冻结，不得偏移） --- */
typedef struct {
    int layers[KIDPAL_LAYER_STACK_MAX];   /* 前台参与者栈（实际内部序实现自由） */
    int n;                                /* 当前层数 */
} kidpal_layer_stack_t;

int kidpal_layer_push(kidpal_layer_stack_t *st, int layer);
int kidpal_layer_pop(kidpal_layer_stack_t *st, int layer);
int kidpal_layer_top(const kidpal_layer_stack_t *st);
int kidpal_layer_has(const kidpal_layer_stack_t *st, int layer);
int kidpal_layer_chat_target(void);
int kidpal_layer_chat_clear_event(int event);

/* 显式清层步骤（TaskPackage 260915-m1-k1a-key-layer-dispatch, KD-A2 清层/进页分离）：
 * 「关窗」唯一事实源用的纯逻辑步骤。出掉全部 transient 覆盖层
 * （KIDPAL_LAYER_VOICE / KIDPAL_LAYER_SERVER_PAGE / KIDPAL_LAYER_REMINDER，各至多一条、
 * 幂等）；KIDPAL_LAYER_LOCAL_PAGE / KIDPAL_LAYER_PROVISION 不碰（配网层零触碰）。
 * 返回出掉的层数（>=0）；NULL st -> -1。空栈/纯基层栈 -> 0。 */
int kidpal_layer_pop_transients(kidpal_layer_stack_t *st);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_LAYER_H */