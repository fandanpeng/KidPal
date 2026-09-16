/*
 * voice_round.c — KidPal 语音轮场景映射 + 话语回显纯逻辑单源实现 (TaskPackage
 * 260915-m1-k3-voice-round-ui)。
 *
 * 语义与数值码以冻结验收 firmware/main/test/voice_round_ui.acceptance.c 文件头
 * FROZEN OFFLINE CONTRACT 为准；强符号在 host runner 里覆盖验收文件内的 weak 哨兵
 * (-2) 后被真测（同 kidpal_voice_states.c / keybar.c 先例）。纯 C、无芯片依赖
 * -> host_app linux target 与设备树两侧同源可编。
 *
 * 场景码 KVST_SCENE_* 本体住 kidpal_voice_states.h（五态闭集单源），本模块只谈
 * 「会话语境 × 事件 -> 目标场景码」的转移决策表——演出面（lcd_display.cc）与触发面
 * （application.cc）都只消费本模块返回值，零重抄映射、零偏移。
 *
 * 冻结映射（弹出时机权威 = requirements §2.2 思考中行「进入时机 = 孩子说完这一句
 * （服务端 VAD 切句完成 / 音频上行结束）即切『思考中』并弹出——状态切换与页面弹出
 * 同一时刻, 不分离」）:
 *   LISTEN_AUTO:   STT_FINAL -> KVST_SCENE_THINK（说完这句即切思考, 弹出同刻,
 *                  4s 安抚计时起点 = 本态入口, VQ-A6 锚）;
 *                  TTS_START -> KVST_SCENE_TALK;
 *                  SPEAK_DONE / ABORT_SPEECH -> -1（聆听态无播报可断/无播可完）。
 *   LISTEN_MANUAL: STT_FINAL -> -1（记录语境整段累积, 切句只用于话语回显,
 *                  不逐句进思考——requirements §2.1 记录会话聆听语义分流）;
 *                  TTS_START -> KVST_SCENE_TALK; 其余 -1。
 *   THINKING:      TTS_START -> KVST_SCENE_TALK（回复到达开始播报即离开本态）;
 *                  STT_FINAL -> -1（已在思考不重入）; 其余 -1。
 *   SPEAKING:      ABORT_SPEECH -> KVST_SCENE_LISTEN（打断停播回聆听——VQ-A4
 *                  演出侧状态机衔接, 键位语义消费 [4] 不自裁）;
 *                  SPEAK_DONE -> KVST_SCENE_LISTEN（播完回聆听, auto 连续对话）;
 *                  其余 -1。
 *   防御: ctx/ev 越界 -> -1（从不为 UB）。
 */

#include "voice_round.h"
#include "kidpal_voice_states.h"

#include <stddef.h> /* NULL */
#include <string.h>

/* 决策表: [ctx][ev] = 目标 KVST 场景码, -1 = 不切场景。ctx/ev 越界定格闭集外 = -1。 */
static const int kSceneTable[4][4] = {
    /* ev:             STT_FINAL    TTS_START    SPEAK_DONE   ABORT_SPEECH */
    /* LISTEN_AUTO   */ { KVST_SCENE_THINK,  KVST_SCENE_TALK,   -1, -1 },
    /* LISTEN_MANUAL */ { -1,                 KVST_SCENE_TALK,   -1, -1 },
    /* THINKING      */ { -1,                 KVST_SCENE_TALK,   -1, -1 },
    /* SPEAKING      */ { -1,                 -1,                 KVST_SCENE_LISTEN,
                          KVST_SCENE_LISTEN },
};

int kidpal_voice_round_scene(kvr_ctx_t ctx, kvr_evt_t ev)
{
    /* 防御：ctx/ev 越界（含非法负值 / >= 闭集上界）-> -1，从不为 UB。 */
    if (ctx < KVR_CTX_LISTEN_AUTO || ctx > KVR_CTX_SPEAKING ||
        ev < KVR_EVT_STT_FINAL || ev > KVR_EVT_ABORT_SPEECH) {
        return -1;
    }
    return kSceneTable[ctx][ev];
}

/* 回显区行缓冲：固定行容量 + 定宽行缓冲数组（纯 C、无动态分配——拷贝存储行模型，
 * 行长度超过 KVR_ECHO_LINE_CAP 截断。行模型值 KVR_ECHO_MAX_LINES 冻结于头文件）。 */
#define KVR_ECHO_LINE_CAP 128

static char s_echo_lines[KVR_ECHO_MAX_LINES][KVR_ECHO_LINE_CAP];
static int  s_echo_count = 0;      /* 当前句数 */
static int  s_echo_head  = 0;      /* 队首下标（idx 0 = 最旧句 = 逻辑队首） */

int kidpal_voice_echo_reset(void)
{
    s_echo_count = 0;
    s_echo_head  = 0;
    return 0;
}

int kidpal_voice_echo_capacity(void)
{
    return KVR_ECHO_MAX_LINES;
}

int kidpal_voice_echo_push(const char *text)
{
    int idx;

    if (text == NULL || text[0] == '\0') {
        return -1; /* NULL/空串拒收, 防空回显行污染演出面 */
    }

    if (s_echo_count < KVR_ECHO_MAX_LINES) {
        /* 容量内：追加为队尾（逻辑下标 = 队首 + count - 1）。 */
        idx = (s_echo_head + s_echo_count) % KVR_ECHO_MAX_LINES;
        s_echo_count++;
    } else {
        /* 满容量滚出：挤最旧（队首那行被覆盖），新句仍落队尾物理槽 =
         * 旧队首物理槽，然后队首前移 —— 保持 idx 0 = 最旧、队尾 = 最新。 */
        idx = s_echo_head;
        s_echo_head = (s_echo_head + 1) % KVR_ECHO_MAX_LINES;
    }

    strncpy(s_echo_lines[idx], text, KVR_ECHO_LINE_CAP - 1);
    s_echo_lines[idx][KVR_ECHO_LINE_CAP - 1] = '\0';
    return s_echo_count; /* 返回当前句数（滚出后恒 KVR_ECHO_MAX_LINES） */
}

int kidpal_voice_echo_count(void)
{
    return s_echo_count;
}

int kidpal_voice_echo_line(int idx, const char **out)
{
    if (out == NULL || idx < 0 || idx >= s_echo_count) {
        return -1; /* 越界行号 / NULL out -> -1 */
    }
    *out = s_echo_lines[(s_echo_head + idx) % KVR_ECHO_MAX_LINES];
    return 0;
}