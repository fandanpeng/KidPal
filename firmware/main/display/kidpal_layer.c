/*
 * kidpal_layer.c — KidPal 前台 layer 栈管理单一权威 (TaskPackage
 * 260911-screen-unification, PKG-2 归一屏)。纯逻辑实现，语义/数值码以冻结验收
 * firmware/main/test/screen_unification.acceptance.c 文件头 FROZEN OFFLINE
 * CONTRACT 为准。
 *
 * 无 chip/FreeRTOS/ESP-component/cJSON 依赖；host runner 与设备树同源可编
 * （host 强符号覆盖验收文件的 weak 哨兵）。内部把栈保持按 layer 数值升序
 * （实现自由，仅保证 top 恒 = 在场最高优先级层）。
 */

#include "kidpal_layer.h"

#include <stddef.h> /* NULL */

/* 前台栈成员合法域 = LOCAL_PAGE..PROVISION（2..6）。CHAT 常驻非栈成员、
 * NONE 空栈 owner，均非可入栈/可查询编码。 */
static int kidpal_layer_is_stackable(int layer)
{
    return layer >= KIDPAL_LAYER_LOCAL_PAGE && layer <= KIDPAL_LAYER_PROVISION;
}

int kidpal_layer_push(kidpal_layer_stack_t *st, int layer)
{
    int i, j;

    if (st == NULL || !kidpal_layer_is_stackable(layer)) {
        return -1;
    }

    /* 幂等去重：已在场保持单条目、不加叠、返回成功。 */
    for (i = 0; i < st->n; i++) {
        if (st->layers[i] == layer) {
            return 0;
        }
    }

    /* 防御（合法编码最多 5 种不同层，正常不可达）：栈满不得越界写。 */
    if (st->n >= KIDPAL_LAYER_STACK_MAX) {
        return -1;
    }

    /* 按优先级升序插入，保持 top = layers[n-1] = 在场最高优先级层。 */
    j = st->n;
    while (j > 0 && st->layers[j - 1] > layer) {
        st->layers[j] = st->layers[j - 1];
        j--;
    }
    st->layers[j] = layer;
    st->n++;
    return 0;
}

int kidpal_layer_pop(kidpal_layer_stack_t *st, int layer)
{
    int i, found;

    if (st == NULL || !kidpal_layer_is_stackable(layer)) {
        return -1;
    }

    found = 0;
    for (i = 0; i < st->n; i++) {
        if (st->layers[i] == layer) {
            found = 1;
            break;
        }
    }
    if (!found) {
        return -1; /* 不在场 -> 不伪装成功 */
    }

    /* 出栈指定层（任意位置），其余保持相对序（升序不破）。 */
    for (; i < st->n - 1; i++) {
        st->layers[i] = st->layers[i + 1];
    }
    st->n--;
    return 0;
}

int kidpal_layer_top(const kidpal_layer_stack_t *st)
{
    if (st == NULL) {
        return -1;
    }
    if (st->n == 0) {
        return KIDPAL_LAYER_NONE; /* 空栈前台 owner */
    }
    return st->layers[st->n - 1]; /* 升序 -> 最高优先级在末位 */
}

int kidpal_layer_has(const kidpal_layer_stack_t *st, int layer)
{
    int i;

    if (st == NULL || !kidpal_layer_is_stackable(layer)) {
        return -1; /* NULL / 非法码（含 CHAT/NONE，非栈成员） */
    }
    for (i = 0; i < st->n; i++) {
        if (st->layers[i] == layer) {
            return 1;
        }
    }
    return 0;
}

int kidpal_layer_chat_target(void)
{
    /* 独立聊天屏退役后聊天内容（状态栏/表情/气泡）落点恒为常驻聊天
     * layer（KidPal 屏上），非独立聊天屏——回答后屏幕落点确定（非随机）。 */
    return KIDPAL_LAYER_CHAT;
}

int kidpal_layer_chat_clear_event(int event)
{
    switch (event) {
        case KIDPAL_LAYER_EVT_NEW_VOICE:
        case KIDPAL_LAYER_EVT_KEY_PRESS:
            return 1; /* 清除时机 = 下次交互（新语音轮 / 任意按键） */
        case KIDPAL_LAYER_EVT_ANSWER_END:
        case KIDPAL_LAYER_EVT_PAGE_SWITCH:
            return 0; /* 回答结束 / 页面切换不清聊天，聊天内容任何页可见 */
        default:
            return 0; /* 未知/负值事件 -> 保守驻留（不清） */
    }
}

/* 显式清层步骤（KD-A2 清层/进页分离）：出掉全部 transient 覆盖层（一个调用清干净），
 * 基层 LOCAL_PAGE 与配网 PROVISION 恒不碰（配网层零触碰红线）。返回出掉的层数。
 * NULL -> -1；空栈/纯基层栈 -> 0。清层事实源 —— 「关窗」不得另写逐层 pop 循环，须经
 * 本步骤（设备侧由 kidpal_render_node dismiss_front_overlay / cycle_next 消费）。 */
int kidpal_layer_pop_transients(kidpal_layer_stack_t *st)
{
    static const int s_transient[] = {
        KIDPAL_LAYER_VOICE,        /* 语音轮（覆盖层） */
        KIDPAL_LAYER_SERVER_PAGE,  /* 服务端推送页（覆盖层） */
        KIDPAL_LAYER_REMINDER      /* 提醒弹窗（最高层覆盖） */
    };
    int popped = 0;
    size_t i;

    if (st == NULL) {
        return -1;
    }
    for (i = 0; i < sizeof(s_transient) / sizeof(s_transient[0]); i++) {
        if (kidpal_layer_has(st, s_transient[i]) == 1) {
            kidpal_layer_pop(st, s_transient[i]);
            popped++;
        }
    }
    return popped;
}