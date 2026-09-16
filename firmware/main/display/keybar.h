/*
 * keybar.h — KidPal 底部键帽条三槽纯逻辑单源（TaskPackage
 * 260915-m1-k2-keybar-three-slot, 键帽条三槽改造）。
 *
 * 通栏贴底三槽 = 左 KEY 提示槽 · 中页点槽 · 右 BOOT 提示槽。条体本身任何状态常驻
 * （基座 chrome，覆盖层盖不掉——既有 bottom_bar_ 载体承载，本纯逻辑模块只裁制
 * 机制值：页点显隐 / 页点总数 / 实心下标、左右槽文案）。
 *
 * 机制值全部出自本模块（page/overlay/scene 入参 -> 槽位值），设备侧（lcd_display.cc）
 * 只消费返回值、零重算——页点显隐与键路由同源（overlay_top 入参即 layer_top 归一化
 * 值，kidpal_render_node_front_is_overlay 提供），零覆盖层进出裁决权（本模块无任何
 * layer 栈 / push / pop 类型或调用，KB-E3）。
 *
 * 左右槽文案值域只引用 kidpal_voice_states 冻结闭集（随态左槽 = look().keycap），
 * 零重抄（KB-E2）——本模块不出现「说完啦/不等了/打断/知道了/叫醒它」字面。
 * 右槽 BOOT 提示 = 语音轮「结束」/ 提醒「关闭」冻结值域（随态右槽）；主页面基层为
 * 页槽表（P 行逐字，WF-F1-01~02 / P3 列表态线框 / P4 撤页过渡兜底）。
 *
 * 纯 C、无 chip/FreeRTOS/ESP-component/LVGL 依赖 -> 可编入 host_app linux target 与
 * 设备树两侧同源（host 强符号覆盖验收文件 weak 哨兵 KKB_UNIMPLEMENTED = -2）。照
 * kidpal_voice_states.c / home_p1.c 先例。
 */
#ifndef KIDPAL_KEYBAR_H
#define KIDPAL_KEYBAR_H

/* 页点总数同源 = 现行页循环页数（key_gesture_parser.h PAGE_COUNT，阶段一 = 4——
 * deviation 对账锚 KB-E1，M2 撤 P4 后随枚举自动 4→3）。key_gesture_parser.h 在
 * firmware/main/（display/ 上一级），相对 include 以兼容 host（仅 -I display）与
 * 设备两侧。 */
#include "../key_gesture_parser.h"
/* 随态左槽值域单源（闭集 + look 结构 + 五态场景码）。 */
#include "kidpal_voice_states.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KKB_UNIMPLEMENTED (-2) /* weak 哨兵 —— 永不是合法返回值 */

/* 页点槽裁决输出（KB-A1 中槽机制值）:
 *   visible: 1 = 页点显示（仅主页面基层）; 0 = 覆盖层隐藏
 *   total:   页点总数 = 现行页循环页数（同源 PAGE_COUNT，阶段一 = 4）
 *   active:  当前实心页点下标（0-based，= 当前页） */
typedef struct {
    int visible;
    int total;
    int active;
} kidpal_keybar_dots_t;

/* 页点槽裁决（KB-A2/KB-A4 机制单源）:
 *   overlay_top != 0（覆盖层顶层）-> visible = 0（隐藏不抹状态, total/active 仍报）;
 *   overlay_top == 0（主页面基层）-> visible = 1, total = PAGE_COUNT, active = page;
 *   防御: out == NULL 或 page 越界（<0 或 >= PAGE_COUNT）-> -1; 成功返回 0。 */
int kidpal_keybar_dots(int page, int overlay_top, kidpal_keybar_dots_t *out);

/* 左右槽文案裁决（KB-A3 机制单源，冻结映射）:
 *   overlay_top == 0（主页面基层, page 槽表）: P1 说话/翻页 · P2 问课程/翻页 ·
 *     P3 说话·双击看/下移 · P4 撤页过渡兜底（只钉存在性, 值钉定归 human 裁定）;
 *     scene 入参被忽略（主页面无语音场景语境）。
 *   overlay_top != 0（覆盖层随态, scene 槽表）: 左槽 = kidpal_voice_state_look(scene)
 *     .keycap（冻结闭集单源, 零复制）; 右槽 = 语音轮「结束」/ 提醒 REMIND「关闭」;
 *     scene 越界（< KVST_SCENE_LISTEN 或 > KVST_SCENE_LOWBATT）-> -1。
 *   防御: out_left/out_right 任一 NULL 或 page 越界 -> -1; 成功返回 0。 */
int kidpal_keybar_slots(int page, int scene, int overlay_top,
                        const char **out_left, const char **out_right);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_KEYBAR_H */