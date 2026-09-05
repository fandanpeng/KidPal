#ifndef SESSION_STATE_MACHINE_H
#define SESSION_STATE_MACHINE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 会话状态机（纯逻辑，表驱动）— KidPal 包 3（TaskPackage 260829-session-logic）。
 *
 * 行为语义权威：docs/spec.md §4.0（共享底座：语音会话生命周期）+ §5.2/§5.3。
 * 承载：KEY 短按 = 启动/结束会话 + 播报中打断停播；KEY/BOOT 短按在对话中均可
 * 结束会话；思考超 4s 先开口安抚（不冷场）；60s 会话超时收束；manual 录音
 * 累积态接合面（包 8 预留，PROPOSAL §2 包 8）。
 *
 * 纯逻辑约束：不依赖芯片外设 / FreeRTOS / ESP 组件头；所有超时（60s 会话超时、
 * 4s 思考超时）都是显式事件注入（SES_EVT_SESSION_TIMEOUT_60S /
 * SES_EVT_THINKING_TIMEOUT_4S），绝无真实 timer。linux host 与设备 target 均可编译。
 *
 * PWR（SES-E3）：PWR 为硬件电源键、固件不可读——本状态机事件表从定义上不含
 * 任何 PWR 事件，PWR 不入交互状态机。
 */

/* 会话状态：ENDED 是单次会话对象的终态——新会话需对全新实例调 ses_machine_init()。
 * SES_STATE_MANUAL_RECORDING 是包 8 接合面：manual 聆听/录音累积态，
 * 由 SES_EVT_START_RECORDING 从聆听态切入（SES_EVT_STOP_RECORDING 退回聆听态）。 */
typedef enum {
    SES_STATE_IDLE = 0,
    SES_STATE_LISTENING,
    SES_STATE_THINKING,
    SES_STATE_SPEAKING,
    SES_STATE_MANUAL_RECORDING,
    SES_STATE_ENDED
} ses_state_t;

/* 显式注入事件（无真实 timer；超时即事件）。 */
typedef enum {
    SES_EVT_KEY_SHORT_PRESS = 0,
    SES_EVT_BOOT_SHORT_PRESS,
    SES_EVT_END_INTENT,
    SES_EVT_SESSION_TIMEOUT_60S,
    SES_EVT_UTT_START,
    SES_EVT_REPLY_READY,
    SES_EVT_SPEAK_DONE,
    SES_EVT_ABORT_SPEECH,
    SES_EVT_THINKING_TIMEOUT_4S,
    SES_EVT_START_RECORDING,
    SES_EVT_STOP_RECORDING,
    SES_EVT_COUNT
} ses_event_t;

/* 转移 side-effect 标记（供上层执行打断停播 / 安抚播报等动作）。 */
#define SES_FLAG_ABORT_SPEECH   (1u << 0)  /* 打断停播 side-effect marker */
#define SES_FLAG_COMFORT_SPEECH (1u << 1)  /* 4s 思考超时安抚播报 marker */

/* 单次转移记录：accepted==0（被拒绝）时 to==from==当前态、flags==0，
 * 状态机不被改写（非法转移被拒绝、不崩溃）。 */
typedef struct {
    int accepted;
    ses_state_t from, to;
    uint32_t flags;
} ses_transition_t;

typedef struct {
    ses_state_t state;
    ses_transition_t last;
    uint32_t accepted_count, rejected_count;
} ses_machine_t;

/* 初始化为 SES_STATE_IDLE，清空转移记录与计数。 */
void ses_machine_init(ses_machine_t *m);

/* 注入一个事件并推进状态机；返回 &m->last（永不 NULL）。
 * 越界事件 id（>=SES_EVT_COUNT）与任何非法转移一样被拒绝。 */
const ses_transition_t *ses_machine_dispatch(ses_machine_t *m, ses_event_t ev);

#ifdef __cplusplus
}
#endif

#endif /* SESSION_STATE_MACHINE_H */
