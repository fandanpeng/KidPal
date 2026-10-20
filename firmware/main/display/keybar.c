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
            default:
                /* M2-K2 撤 P4：原第 4 页槽位的过渡兜底 case 随枚举整值撤出而消解，
                 * 页槽表只剩 P1/P2/P3 三行。越界页（含原第 4 页索引 = PAGE_COUNT）
                 * 已由上方防御列一律 -1，本分支对合法页（0..PAGE_COUNT-1 = P1..P3）
                 * 不可达；保留仅为 switch 完整性，不复活任何页位。 */
                return -1;
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

int kidpal_keybar_char_slots(int started, const char **out_left,
                             const char **out_right)
{
    /* 防御：任一输出指针 NULL -> -1，从不为 UB。 */
    if (out_left == NULL || out_right == NULL) {
        return -1;
    }
    /* 查字窗键帽随态（v1.59 WF-F3-01）: 未起播「播报」（KEY 短按 = 起播）/
     * 已起播「重播」（KEY 短按 = 从头重播）；右槽 BOOT「结束」恒值（与覆盖层非提醒
     * 右槽一致, §2.1 结束方式③）。started 语义 = 未起播 0 / 已起播非 0。 */
    *out_left  = (started != 0) ? "重播" : "播报";
    *out_right = "结束";
    return 0;
}

int kidpal_keybar_story_slots(int substate, const char **out_left,
                              const char **out_right)
{
    /* 防御：任一输出指针 NULL -> -1，从不为 UB。 */
    if (out_left == NULL || out_right == NULL) {
        return -1;
    }
    /* P3 子态键帽随态（WF-F2-02/04/05; 260924-m4-k2-p3-star-notebook-ui; D3 v7 重排
     * 261019 fixer, 2026-10-20）:
     * 录音态左「收尾」（KEY 短按 = 停录保存, v1.47）/ 右「**丢弃**」（BOOT = 丢弃,
     * 破坏性提示承担误按缓释——v7 p3-recording 修订, 旧「结束」兜底文案退役;
     * 语音轮 keycap 闭集（kidpal_voice_states.c 五成员）只在语音轮四态用,
     * 记录语境不是语音轮态, 本表零闭集字面（KB-E2）/ 阅读态「长按退出」（KEY 长按 =退出阅读回列表）/
     * 删除确认「删掉」（KEY 短按 = 确认删除）; 整理过渡态 = **非交互**, 左右槽均
     * 留空（KEY/BOOT 均不响应, 键帽条照常驻留——v7 state-organizing R2 裁定;
     * 空串 = 消费方置空槽, 非保持旧文案）。正文取不回提示（WF-F2-06,
     * 261016-p3-story-body-sd-cache）: 两键任一手势均关闭回列表, 底栏文案单源 =
     * story_page.h 的 KSTORY_BODY_UNAVAILABLE_KEYCAP_* 宏（本文件零字面重抄, 守
     * KB-E2 闭集字面红线）。LIST/STAMP 不落本表（基层页槽表 / 演出帧无键帽）-> -1,
     * 消费方回落既有路径。 */
    switch (substate) {
        case KSTORY_PAGE_RECORDING:
            *out_left  = "收尾";
            *out_right = "丢弃";
            return 0;
        case KSTORY_PAGE_ORGANIZING:
            *out_left  = "";
            *out_right = "";
            return 0;
        case KSTORY_PAGE_READER:
            *out_left  = "长按退出";
            *out_right = "翻屏";
            return 0;
        case KSTORY_PAGE_DELETE:
            *out_left  = "删掉";
            *out_right = "算了";
            return 0;
        case KSTORY_PAGE_BODY_UNAVAILABLE:
            /* WF-F2-06 正文取不回覆盖层底栏（设计稿 v6 p3-body-unavailable 逐字）:
             * 文案经 story_page.h 单源宏符号引用（见上注, KB-E2 零字面重抄）。 */
            *out_left  = KSTORY_BODY_UNAVAILABLE_KEYCAP_KEY;
            *out_right = KSTORY_BODY_UNAVAILABLE_KEYCAP_BOOT;
            return 0;
        default:
            return -1;
    }
}

int kidpal_keybar_story_rec_slots(int guide_sub, const char **out_left,
                                  const char **out_right)
{
    /* 防御：任一输出指针 NULL -> -1，从不为 UB。 */
    if (out_left == NULL || out_right == NULL) {
        return -1;
    }
    /* D3 v7 帧分叉键帽（261019 fixer; 权威 = v7 p3-recording / p3-recording-guide
     * 两帧 + 规格表「键帽」行）:
     *   guide_sub == 0（帧 A 常规录音聆听）-> 左「收尾」右「丢弃」;
     *   guide_sub != 0（帧 B 引导播报中）-> 左槽 = KSTORY_RECORDING_GUIDE_KEYCAP 宏
     *     （值域住 story_page.h 单源; KB-E2 零字面重抄, 见该宏注记）右**留空**
     *     （BOOT 无操作——空串置空, 不画键帽盒免诱导误按）。
     * 消费点 = lcd_display 键帽三槽随态（story_sub == RECORDING 时经渲染节点
     * kidpal_render_node_story_rec_substate 取子态分叉）。 */
    if (guide_sub != 0) {
        /* 字面经 story_page.h 单源宏（KKB-E2: 该值同字为语音轮闭集成员, keybar.c
         * 零闭集字面重抄——同 KSTORY_BODY_UNAVAILABLE_KEYCAP_* 宏间接先例）。 */
        *out_left  = KSTORY_RECORDING_GUIDE_KEYCAP;
        *out_right = "";
    } else {
        *out_left  = "收尾";
        *out_right = "丢弃";
    }
    return 0;
}