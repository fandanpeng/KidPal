#include "session_state_machine.h"

/*
 * 会话状态机实现（表驱动纯逻辑）。
 *
 * 转移表一行一状态、一列一事件（列序 = ses_event_t 枚举序）；表项
 * {to, flags}，to == SES_EDGE_REJECTED 表示非法转移（拒绝：状态不变、
 * 记 rejected、不崩溃）。表内容逐项对应冻结验收文件
 * firmware/main/test/session_logic.acceptance.c 头注释钉定的转移契约：
 *
 *   IDLE:             KEY_SHORT_PRESS -> LISTENING；其余全拒
 *                     （含 BOOT_SHORT_PRESS：BOOT 永不启动会话）。
 *   LISTENING:        UTT_START->THINKING；END_INTENT /
 *                     SES_EVT_SESSION_TIMEOUT_60S / KEY_SHORT_PRESS /
 *                     BOOT_SHORT_PRESS（对话中 BOOT 也可结束
 *                     会话）-> ENDED；START_RECORDING->MANUAL_RECORDING（包 8
 *                     接合）。REPLY_READY / SPEAK_DONE / ABORT_SPEECH /
 *                     THINKING_TIMEOUT_4S / STOP_RECORDING 拒绝。
 *   THINKING:         REPLY_READY->SPEAKING；THINKING_TIMEOUT_4S->SPEAKING 带
 *                     COMFORT 标记（思考超 4s 先开口安抚，不冷场，spec §4.0）；
 *                     END_INTENT / SES_EVT_SESSION_TIMEOUT_60S /
 *                     KEY_SHORT_PRESS / BOOT_SHORT_PRESS -> ENDED；其余拒绝。
 *   SPEAKING:         SPEAK_DONE->LISTENING；ABORT_SPEECH->LISTENING 带 ABORT
 *                     标记（人声喊停回聆听，不结束会话）；KEY_SHORT_PRESS->
 *                     ENDED 带 ABORT 标记（KEY 打断停播并手动关闭会话）；
 *                     BOOT_SHORT_PRESS->ENDED（不带打断标记）；END_INTENT /
 *                     SES_EVT_SESSION_TIMEOUT_60S -> ENDED；其余拒绝。
 *   MANUAL_RECORDING: STOP_RECORDING->LISTENING；END_INTENT /
 *                     SES_EVT_SESSION_TIMEOUT_60S / KEY_SHORT_PRESS /
 *                     BOOT_SHORT_PRESS -> ENDED（会话生命周期语义在累积态不被破坏）；
 *                     UTT_START / REPLY_READY / START_RECORDING 拒绝
 *                     （累积态不进对话轮、重复切入拒绝）。
 *   ENDED:            终态——一切事件拒绝；新会话须全新实例 init。
 *
 * PWR（SES-E3）：PWR 为硬件电源键、固件不可读，事件表从定义上不含 PWR 事件。
 */

#define SES_STATE_COUNT_ 6u
#define SES_EDGE_REJECTED 0xFFu
#define SES_REJECTED_EDGE { SES_EDGE_REJECTED, 0u }

typedef struct {
    uint8_t to;    /* ses_state_t 值，或 SES_EDGE_REJECTED 表示拒绝 */
    uint8_t flags; /* SES_FLAG_* 组合 */
} ses_edge_t;

/* 列序（= ses_event_t）：
 * KEY_SHORT_PRESS, BOOT_SHORT_PRESS, END_INTENT, SES_EVT_SESSION_TIMEOUT_60S,
 * UTT_START, REPLY_READY, SPEAK_DONE, ABORT_SPEECH,
 * THINKING_TIMEOUT_4S, START_RECORDING, STOP_RECORDING */
static const ses_edge_t k_ses_table[SES_STATE_COUNT_][SES_EVT_COUNT] = {
    /* IDLE：只有 KEY 短按启动会话。 */
    { { SES_STATE_LISTENING, 0u },
      SES_REJECTED_EDGE, SES_REJECTED_EDGE, SES_REJECTED_EDGE,
      SES_REJECTED_EDGE, SES_REJECTED_EDGE, SES_REJECTED_EDGE,
      SES_REJECTED_EDGE, SES_REJECTED_EDGE, SES_REJECTED_EDGE,
      SES_REJECTED_EDGE },
    /* LISTENING。 */
    { { SES_STATE_ENDED, 0u },             /* KEY 短按：手动结束 */
      { SES_STATE_ENDED, 0u },             /* BOOT 短按：对话中也可结束会话 */
      { SES_STATE_ENDED, 0u },             /* 结束意图（拜拜/不用了） */
      { SES_STATE_ENDED, 0u },             /* 60s 会话超时 */
      { SES_STATE_THINKING, 0u },          /* 开口 -> 思考 */
      SES_REJECTED_EDGE,                   /* 未开口不得直接播报 */
      SES_REJECTED_EDGE,
      SES_REJECTED_EDGE,                   /* 无播报可打断 */
      SES_REJECTED_EDGE,                   /* 安抚转移只从思考态出发 */
      { SES_STATE_MANUAL_RECORDING, 0u },  /* 包 8 接合：切入累积态 */
      SES_REJECTED_EDGE },
    /* THINKING。 */
    { { SES_STATE_ENDED, 0u },
      { SES_STATE_ENDED, 0u },
      { SES_STATE_ENDED, 0u },
      { SES_STATE_ENDED, 0u },
      SES_REJECTED_EDGE,                   /* 已在思考不重入 */
      { SES_STATE_SPEAKING, 0u },          /* 正常回包 -> 播报（无标记） */
      SES_REJECTED_EDGE,
      SES_REJECTED_EDGE,
      { SES_STATE_SPEAKING, SES_FLAG_COMFORT_SPEECH }, /* 4s 安抚播报 */
      SES_REJECTED_EDGE,
      SES_REJECTED_EDGE },
    /* SPEAKING。 */
    { { SES_STATE_ENDED, SES_FLAG_ABORT_SPEECH }, /* KEY 打断停播并关闭会话 */
      { SES_STATE_ENDED, 0u },             /* BOOT 结束不带打断标记 */
      { SES_STATE_ENDED, 0u },
      { SES_STATE_ENDED, 0u },
      SES_REJECTED_EDGE,                   /* 插话走 ABORT 非 UTT */
      SES_REJECTED_EDGE,
      { SES_STATE_LISTENING, 0u },         /* 播完回聆听 */
      { SES_STATE_LISTENING, SES_FLAG_ABORT_SPEECH }, /* 人声喊停回聆听 */
      SES_REJECTED_EDGE,
      SES_REJECTED_EDGE,
      SES_REJECTED_EDGE },
    /* MANUAL_RECORDING（包 8 接合：manual 聆听/录音累积态）。 */
    { { SES_STATE_ENDED, 0u },
      { SES_STATE_ENDED, 0u },
      { SES_STATE_ENDED, 0u },
      { SES_STATE_ENDED, 0u },
      SES_REJECTED_EDGE,                   /* 累积态不进对话轮 */
      SES_REJECTED_EDGE,
      SES_REJECTED_EDGE,
      SES_REJECTED_EDGE,
      SES_REJECTED_EDGE,
      SES_REJECTED_EDGE,                   /* 重复切入拒绝 */
      { SES_STATE_LISTENING, 0u } },       /* 退出接缝回聆听，会话仍活着 */
    /* ENDED：终态，一切事件拒绝。 */
    { SES_REJECTED_EDGE, SES_REJECTED_EDGE, SES_REJECTED_EDGE,
      SES_REJECTED_EDGE, SES_REJECTED_EDGE, SES_REJECTED_EDGE,
      SES_REJECTED_EDGE, SES_REJECTED_EDGE, SES_REJECTED_EDGE,
      SES_REJECTED_EDGE, SES_REJECTED_EDGE },
};

void ses_machine_init(ses_machine_t *m)
{
    m->state = SES_STATE_IDLE;
    m->last.accepted = 0;
    m->last.from = SES_STATE_IDLE;
    m->last.to = SES_STATE_IDLE;
    m->last.flags = 0u;
    m->accepted_count = 0u;
    m->rejected_count = 0u;
}

const ses_transition_t *ses_machine_dispatch(ses_machine_t *m, ses_event_t ev)
{
    ses_transition_t *t = &m->last;

    t->accepted = 0;
    t->from = m->state;
    t->to = m->state;
    t->flags = 0u;

    /* 越界事件 id / 越界状态（防御，不崩溃）与非法转移同样走拒绝路径。 */
    if ((uint32_t)ev < (uint32_t)SES_EVT_COUNT &&
        (uint32_t)m->state < SES_STATE_COUNT_) {
        const ses_edge_t *edge = &k_ses_table[m->state][(uint32_t)ev];
        if (edge->to != SES_EDGE_REJECTED) {
            t->accepted = 1;
            t->to = (ses_state_t)edge->to;
            t->flags = edge->flags;
            m->state = t->to;
            m->accepted_count++;
            return t;
        }
    }

    m->rejected_count++;
    return t;
}
