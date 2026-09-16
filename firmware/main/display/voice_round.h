/*
 * voice_round.h — KidPal 语音轮场景映射 + 话语回显纯逻辑单源 (TaskPackage
 * 260915-m1-k3-voice-round-ui)。
 *
 * 语义与数值码以冻结验收 firmware/main/test/voice_round_ui.acceptance.c 文件头
 * FROZEN OFFLINE CONTRACT (frozen seam [1]) 为准，不得偏移；本头声明与该文件本地
 * 常量/形状逐字一致（C linkage 纯逻辑）。纯 C、无 chip/LVGL/ESP 组件依赖 -> 可编入
 * host_app linux target 与设备树两侧（host 强符号覆盖 weak 哨兵 KVR_UNIMPLEMENTED
 * = -2）。照 kidpal_voice_states.c / keybar.c 先例。
 *
 * 两个机制:
 *   1) kidpal_voice_round_scene — 会话语境 × 会话事件 -> 目标 KVST 场景码
 *      （KVST_SCENE_*，kidpal_voice_states.h 冻结闭集单源；返回 <0 = 不切场景，
 *       -1 兼作防御返回值）。场景码本体 = 思考/聆听/播报等语音场景的演出码，语义源
 *      = kidpal_voice_states.h 五态闭集。
 *   2) kidpal_voice_echo_* — 话语回显区机制（逐句入列 + 行数容量滚出，行数模型，
 *      屏高视觉拟合归真机轮）。expose 机制值，设备侧零重算、只消费返回值。
 */
#ifndef KIDPAL_VOICE_ROUND_H
#define KIDPAL_VOICE_ROUND_H

#ifdef __cplusplus
extern "C" {
#endif

#define KVR_UNIMPLEMENTED (-2) /* weak 哨兵 — 永不是合法返回值 */
#define KVR_ECHO_MAX_LINES 6   /* 回显区行数容量（冻结值; 屏高可容行数视觉拟合归设计稿） */

/* --- 会话语境（判定入参闭集，显式数值冻结，与本验收文件内哨兵声明逐值一致） --- */
typedef enum {
    KVR_CTX_LISTEN_AUTO   = 0, /* 聆听中, auto/VAD 切句, Phase 1 问答主路径 */
    KVR_CTX_LISTEN_MANUAL = 1, /* 聆听中, manual 按键段——记录语境整段累积 */
    KVR_CTX_THINKING      = 2, /* 思考中 */
    KVR_CTX_SPEAKING      = 3  /* 播报中 */
} kvr_ctx_t;

/* --- 会话事件（判定入参闭集，显式数值冻结，与本验收文件内哨兵声明逐值一致） --- */
typedef enum {
    KVR_EVT_STT_FINAL    = 0, /* 网关 VAD 切句完成, WS stt 文本下发 */
    KVR_EVT_TTS_START    = 1, /* 回复到达开始播报, tts start */
    KVR_EVT_SPEAK_DONE   = 2, /* 播报完成, tts stop */
    KVR_EVT_ABORT_SPEECH = 3  /* 打断停播, KEY 单击经分层裁决 */
} kvr_evt_t;

/*
 * 会话语境 × 事件 -> 目标 KVST 场景码（KVST_SCENE_*, 冻结映射见 voice_round.c）;
 * 返回 <0 = 不切场景, -1 兼作防御返回值。ctx/ev 越界 -> -1（从不为 UB）。
 */
int kidpal_voice_round_scene(kvr_ctx_t ctx, kvr_evt_t ev);

/* --- 话语回显区机制（VQ-A2; 逐句入列 + 行数容量滚出 = 「回显区滚动, 超屏高滚出」
 *   行数模型） --- */
int kidpal_voice_echo_reset(void);               /* 会话开始清零; 返回 0 */
int kidpal_voice_echo_push(const char *text);    /* 逐句入列（拷贝存储）, 满容量挤最旧;
                                                    返回当前句数; NULL/空串 -> -1 */
int kidpal_voice_echo_count(void);               /* 当前句数 */
int kidpal_voice_echo_line(int idx, const char **out); /* idx 0 = 最旧; 越界/out NULL -> -1 */
int kidpal_voice_echo_capacity(void);            /* 恒 KVR_ECHO_MAX_LINES */

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_VOICE_ROUND_H */