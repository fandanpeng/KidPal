/*
 * view_router.c — KidPal display view 路由共享底座: 三个 FROZEN OFFLINE CONTRACT
 * 的纯逻辑实现 (TaskPackage 260829-voice-roundtrip, 方向 B display 消费链路).
 *
 * 语义与数值码以冻结验收 firmware/main/test/voice_roundtrip_b.acceptance.c
 * 文件头为准，不得偏移；weak 哨兵被本文件的强定义覆盖后在 host runner 里果真
 * 被测 (同 G 包 python frozen-offline-contract 的 C 对偶)。
 *
 * 纯逻辑、无芯片/FreeRTOS/ESP 组件依赖 -> 可编入 host_app linux target 与设备树
 * 两侧。降级语义：白名单之外 / NULL 一律显式 UNKNOWN (VRT_ROUTE_UNKNOWN)，调用方
 * 必须展示友好降级提示，绝不盲渲染 (V27 / spec §4.0)。
 */

#include "view_router.h"
#include <string.h>

int kidpal_display_route(const char *view)
{
    /* NULL 防御：不崩、归降级路径 (V27 / 契约 §1.3 白名单之外即降级)。 */
    if (view == NULL) {
        return VRT_ROUTE_UNKNOWN;
    }

    /* 7 view 注册表 —— additive 共享底座 (VRT-E3, 包 4/5/6/8 扩展不改核心)。 */
    if (strcmp(view, "text") == 0) {
        return VRT_ROUTE_TEXT;
    }
    if (strcmp(view, "home") == 0) {
        return VRT_ROUTE_HOME;
    }
    if (strcmp(view, "control") == 0) {
        return VRT_ROUTE_CONTROL;
    }
    if (strcmp(view, "char") == 0) {
        return VRT_ROUTE_CHAR;
    }
    if (strcmp(view, "schedule") == 0) {
        return VRT_ROUTE_SCHEDULE;
    }
    if (strcmp(view, "story_list") == 0) {
        return VRT_ROUTE_STORY_LIST;
    }
    if (strcmp(view, "story_text") == 0) {
        return VRT_ROUTE_STORY_TEXT;
    }

    /* 白名单外 view -> UNKNOWN 降级信号：独立路径，与任何正常渲染不混流。
     * (格式不遵循降级, V27 / spec §4.0 —— 绝不盲渲染。) */
    return VRT_ROUTE_UNKNOWN;
}

int kidpal_display_control_action(const char *action)
{
    if (action == NULL) {
        return VRT_CTRL_UNKNOWN;
    }

    /* 契约 §1.4 动作全表。 */
    if (strcmp(action, "end_session") == 0) {
        return VRT_CTRL_END_SESSION;
    }
    if (strcmp(action, "stop_speaking") == 0) {
        return VRT_CTRL_STOP_SPEAKING;
    }
    if (strcmp(action, "start_recording") == 0) {
        return VRT_CTRL_START_RECORDING;
    }

    /* 未知动作 -> 降级 (不执行任何语义)。 */
    return VRT_CTRL_UNKNOWN;
}

int kidpal_display_text_page_count(const char *text, int screen_lines)
{
    /* screen_lines <= 0 是防御输入：返回 1 (单屏)，永不分除零崩溃。
     * (冻结验收允许返回 1 或 -1；返回 1 更稳。) */
    if (screen_lines <= 0) {
        return 1;
    }

    /* NULL text 计为一个 (空) 屏。 */
    if (text == NULL) {
        return 1;
    }

    /* 行计数：首行算 1，每个 '\n' 之后是新行。字符串空 = 1 行。 */
    {
        long line_count = 1;
        for (const char *p = text; *p != '\0'; p++) {
            if (*p == '\n') {
                line_count++;
            }
        }

        if (line_count <= screen_lines) {
            return 1;
        }
        /* ceil(line_count / screen_lines) —— 保证 pages * screen_lines >= line_count
         * (全文不丢内容, S-F4.1 否摘要 —— 显示端不截断)。超 3 句的截断是网关 V32
         * 的有处理失败模式 (G 生成面)，设备只渲染全文。 */
        return (int)((line_count + screen_lines - 1) / screen_lines);
    }
}