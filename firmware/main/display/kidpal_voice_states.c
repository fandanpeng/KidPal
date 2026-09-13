/*
 * kidpal_voice_states.c — KidPal 语音轮五态演出内容选择单源的实现
 * (TaskPackage 260911-render-pages, RP-A9 五态演出换皮)。
 *
 * 语义与数值码以冻结验收 firmware/main/test/v3_restore.acceptance.c 文件头
 * FROZEN OFFLINE CONTRACT 为准；强符号在 host runner 里覆盖验收文件内的 weak 哨兵
 * (-2) 后被真测（同 kidpal_layer.c / schedule_page.c 先例）。纯 C、无芯片依赖
 * -> host_app linux target 与设备树两侧同源可编。
 *
 * 冻结映射（键帽短语 = V3 §06 逐字，五态键帽行不得偏移）:
 *   LISTEN  -> pose LISTEN, bubble "我在听！", keycap "说完啦"
 *   THINK   -> pose THINK,  keycap "不等了"   (bubble NULL: 问号/气泡文案归 design review)
 *   TALK    -> pose TALK,   keycap "打断"     (声波条/同步文字框为 LVGL 接线面)
 *   REMIND  -> pose JUMP,   keycap "知道了"
 *   LOWBATT -> pose SLEEP,  keycap "叫醒它"
 * 防御: out == NULL 或 scene 非法 (<0 或 >LOWBATT) -> -1。返回 0 成功。
 */

#include "kidpal_voice_states.h"

#include <stddef.h> /* NULL */

int kidpal_voice_state_look(int scene, kidpal_voice_look_t *out)
{
    static const char *const kListenBubble = "我在听！";
    static const char *const kCapListen    = "说完啦";
    static const char *const kCapThink     = "不等了";
    static const char *const kCapTalk      = "打断";
    static const char *const kCapRemind    = "知道了";
    static const char *const kCapLowbatt   = "叫醒它";

    /* 防御：NULL out 或非法场景 (<LISTEN 或 >LOWBATT) -> -1，从不为 UB。 */
    if (out == NULL || scene < KVST_SCENE_LISTEN || scene > KVST_SCENE_LOWBATT) {
        return -1;
    }

    switch (scene) {
        case KVST_SCENE_LISTEN:
            out->pose   = KVST_POSE_LISTEN;
            out->bubble = kListenBubble;
            out->keycap = kCapListen;
            break;
        case KVST_SCENE_THINK:
            out->pose   = KVST_POSE_THINK;
            out->bubble = NULL; /* 问号/气泡文案归 design review，不锁 */
            out->keycap = kCapThink;
            break;
        case KVST_SCENE_TALK:
            out->pose   = KVST_POSE_TALK;
            out->bubble = NULL; /* 声波条 + 同步文字框为 LVGL 接线面，本纯逻辑不锁 */
            out->keycap = kCapTalk;
            break;
        case KVST_SCENE_REMIND:
            out->pose   = KVST_POSE_JUMP;
            out->bubble = NULL;
            out->keycap = kCapRemind;
            break;
        case KVST_SCENE_LOWBATT:
        default:
            out->pose   = KVST_POSE_SLEEP;
            out->bubble = NULL;
            out->keycap = kCapLowbatt;
            break;
    }
    return 0;
}