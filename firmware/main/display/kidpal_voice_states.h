/*
 * kidpal_voice_states.h — KidPal 语音轮五态演出内容选择单源 (TaskPackage
 * 260911-render-pages, RP-A9 五态演出换皮)。
 *
 * 语义与数值码以冻结验收 firmware/main/test/v3_restore.acceptance.c 文件头
 * FROZEN OFFLINE CONTRACT 为准，不得偏移；本头声明与该文件本地常量/形状逐字一致
 * （C linkage 纯逻辑）。五态场景码 -> 姿态 / V3 气泡文案 / 底部键帽条文案
 * （键帽短语 = V3 §06 逐字，五态键帽行不得偏移）。实现见 kidpal_voice_states.c。
 *
 * 纯 C、无 chip/FreeRTOS/ESP-component/LVGL 依赖 -> 可编入 host_app linux target
 * 与设备树两侧（host 强符号覆盖 weak 哨兵 KVST_UNIMPLEMENTED = -2）。照 kidpal_layer.c
 * 先例。层底座与优先级仲裁零触碰（RP-A9：本模块只做「场景 -> 内容」映射）。
 */
#ifndef KIDPAL_VOICE_STATES_H
#define KIDPAL_VOICE_STATES_H

#ifdef __cplusplus
extern "C" {
#endif

#define KVST_UNIMPLEMENTED (-2) /* weak 哨兵 — 永不是合法返回值 */

/* --- 五态场景码（冻结，数值不得偏移；语义源 = application.cc:713-751 设备态分支
 *      + reminder_page.c 提醒弹窗 + low_battery_popup_ 低电，见 intake-survey-s02-s03.md
 *      S0.3 落点清单） --- */
#define KVST_SCENE_LISTEN   0   /* 聆听 (pose-listen + 气泡「我在听！」) */
#define KVST_SCENE_THINK    1   /* 思考 (pose-think + 问号) */
#define KVST_SCENE_TALK     2   /* 播报 (pose-talk + 声波条 + 同步文字框) */
#define KVST_SCENE_REMIND   3   /* 提醒 (pose-jump + d50 压暗弹窗) */
#define KVST_SCENE_LOWBATT  4   /* 低电打盹 (pose-sleep + zZZ) */

/* --- 姿态码（冻结；映射 B8 闭集 KAP1 资产 pose_<name>_96.bin） --- */
#define KVST_POSE_LISTEN 0
#define KVST_POSE_THINK  1
#define KVST_POSE_TALK   2
#define KVST_POSE_JUMP   3
#define KVST_POSE_SLEEP  4

typedef struct {
    int pose;           /* KVST_POSE_* (该态小伴姿态) */
    const char *bubble; /* 各态内容区标头文案 (「我在听！」/「思考中」/「播报」
                           三态标头, 单源 = 本闭集; REMIND/LOWBATT 无标头 = NULL) */
    const char *keycap; /* 底部键帽条 KEY 短语 (V3 §06, 恒非空; 键帽条任何状态常驻) */
} kidpal_voice_look_t;

int kidpal_voice_state_look(int scene, kidpal_voice_look_t *out);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_VOICE_STATES_H */