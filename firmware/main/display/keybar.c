/*
 * keybar.c — KidPal 底部键帽条三槽纯逻辑单源实现（TaskPackage
 * 260915-m1-k2-keybar-three-slot）。
 *
 * 机制值（页点显隐 / 总数 / 实心下标、左右槽文案）全部住本模块；设备侧
 * （lcd_display.cc）只消费返回值、零重算。纯 C、无 LVGL/chip 依赖 -> host_app linux
 * target 与设备树两侧同源可编（强符号覆盖验收文件 weak 哨兵 -2）。
 *
 * 随态：左槽随态值域只经 kidpal_voice_state_look(scene).keycap 单源取
 * （kidpal_voice_states.c 冻结闭集，零重抄、零偏移 KB-E2）；右槽 BOOT 提示 = 语音轮
 * 「结束」/ 提醒「关闭」冻结值域（住本模块右槽值域，不混入闭集）。主页面基层 = 页
 * 槽表（P 行逐字）。
 */
#include "keybar.h"

#include <stddef.h> /* NULL */

int kidpal_keybar_dots(int page, int overlay_top, kidpal_keybar_dots_t *out)
{
    /* 防御：NULL out 或页越界（<0 或 >= 现行页循环页数）-> -1，从不为 UB。 */
    if (out == NULL || page < 0 || page >= (int)PAGE_COUNT) {
        return -1;
    }
    /* 覆盖层顶层（overlay_top != 0，值来自 kidpal_render_node_front_is_overlay 的
     * 归一化）-> 页点隐藏；隐藏不抹机制状态（total/active 仍报当前值）。 */
    out->visible = (overlay_top != 0) ? 0 : 1;
    out->total   = (int)PAGE_COUNT;
    out->active  = page;
    return 0;
}

int kidpal_keybar_slots(int page, int scene, int overlay_top,
                        const char **out_left, const char **out_right)
{
    /* 防御：任一输出指针 NULL 或页越界 -> -1。 */
    if (out_left == NULL || out_right == NULL || page < 0 || page >= (int)PAGE_COUNT) {
        return -1;
    }

    if (overlay_top == 0) {
        /* 主页面基层：页槽表（规格权威 = requirements §2.2 线框行逐字）。scene 入参
         * 在基层列被忽略（主页面无语音场景语境，任意 int 均合法）。 */
        switch (page) {
            case PAGE_P1: *out_left = "说话";     *out_right = "翻页"; break;
            case PAGE_P2: *out_left = "问课程";   *out_right = "翻页"; break;
            case PAGE_P3: *out_left = "说话·双击看"; *out_right = "下移"; break;
            case PAGE_P4:
            default:
                /* P4 撤页过渡兜底（v1.36 撤 P4、页循环仍 4——KB-E1 deviation）。
                 * 页槽值 spec 未定，只保证给非 NULL 存在性值；值钉定须 human 裁定后
                 * tighten-only 修订（ADV-1 已登记 planner）。 */
                *out_left = "说话"; *out_right = "翻页"; break;
        }
        return 0;
    }

    /* 覆盖层随态：scene 槽表。左槽经 child 层冻结闭集单源取 keycap（零复制、零偏移）;
     * 右槽 BOOT 提示 = 语音轮「结束」/ 提醒「关闭」。scene 越界 -> -1。 */
    if (scene < KVST_SCENE_LISTEN || scene > KVST_SCENE_LOWBATT) {
        return -1;
    }
    {
        kidpal_voice_look_t look;
        if (kidpal_voice_state_look(scene, &look) != 0 || look.keycap == NULL) {
            return -1;
        }
        *out_left  = look.keycap;
        *out_right = (scene == KVST_SCENE_REMIND) ? "关闭" : "结束";
    }
    return 0;
}