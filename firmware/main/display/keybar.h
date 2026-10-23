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
 * 页槽表（P 行逐字，WF-F1-01~02 / P3 列表态线框）。
 *
 * 纯 C、无 chip/FreeRTOS/ESP-component/LVGL 依赖 -> 可编入 host_app linux target 与
 * 设备树两侧同源（host 强符号覆盖验收文件 weak 哨兵 KKB_UNIMPLEMENTED = -2）。照
 * kidpal_voice_states.c / home_p1.c 先例。
 */
#ifndef KIDPAL_KEYBAR_H
#define KIDPAL_KEYBAR_H

/* 页点总数同源 = 现行页循环页数（key_gesture_parser.h PAGE_COUNT；2026-09-18 随
 * 260917-m2-k2-retract-p4-three-page 撤 P4，随枚举 = 3，KB-E1 deviation 收回）。本模块
 * 永不写死数字面——总数恒取 (int)PAGE_COUNT。key_gesture_parser.h 在 firmware/main/
 * （display/ 上一级），相对 include 以兼容 host（仅 -I display）与设备两侧。 */
#include "../key_gesture_parser.h"
/* 随态左槽值域单源（闭集 + look 结构 + 五态场景码）。 */
#include "kidpal_voice_states.h"
/* P3 子态键帽槽裁决的 substate 入参枚举（KSTORY_PAGE_*）。 */
#include "story_page.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KKB_UNIMPLEMENTED (-2) /* weak 哨兵 —— 永不是合法返回值 */

/* 页点槽裁决输出（KB-A1 中槽机制值）:
 *   visible: 1 = 页点显示（仅主页面基层）; 0 = 覆盖层隐藏
 *   total:   页点总数 = 现行页循环页数（同源 PAGE_COUNT = 3）
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
 *     P3 说话·双击看/下移（P4 撤页后页槽表只此三行, 越界页交防御列 -1）;
 *     scene 入参被忽略（主页面无语音场景语境）。
 *   overlay_top != 0（覆盖层随态, scene 槽表）: 左槽 = kidpal_voice_state_look(scene)
 *     .keycap（冻结闭集单源, 零复制）; 右槽 = 语音轮「结束」/ 提醒 REMIND「关闭」;
 *     scene 越界（< KVST_SCENE_LISTEN 或 > KVST_SCENE_LOWBATT）-> -1。
 *   防御: out_left/out_right 任一 NULL 或 page 越界 -> -1; 成功返回 0。 */
int kidpal_keybar_slots(int page, int scene, int overlay_top,
                        const char **out_left, const char **out_right);

/* 查字窗键帽槽裁决（ACC-5, WF-F3-01 键帽随态变注 v1.59; TaskPackage
 * 260928-char-window-key-triggered-play additive 追加）:
 *   演示未起播（started == 0）-> 左「播报」右「结束」;
 *   演示已起播（started != 0）-> 左「重播」右「结束」（started 语义 = 未起播 0 /
 *     已起播非 0, 不为 1 特判留缝）;
 *   右槽 BOOT「结束」与覆盖层非提醒右槽一致（§2.1 结束方式③）。
 *   防御: out_left/out_right 任一 NULL -> -1; 成功返回 0。
 * 并行函数（不动 kidpal_keybar_slots 既有签名与值域——additive, 既有齿不破）;
 * 设备侧 lcd_display.cc 查字窗前台分支消费（键帽值域仍走本纯逻辑单源, 零重算）。 */
int kidpal_keybar_char_slots(int started, const char **out_left,
                             const char **out_right);

/* P3 星星记录本子态键帽槽裁决（WF-F2-02/04/05 键帽随态变; TaskPackage
 * 260924-m4-k2-p3-star-notebook-ui additive 追加, 260928 查字窗
 * kidpal_keybar_char_slots 先例形态——并行函数, 不动 kidpal_keybar_slots 既有
 * 签名与值域, 既有齿不破）:
 *   substate = KSTORY_PAGE_DELETE -> 左「删掉」右「算了」（WF-F2-05）;
 *   substate = KSTORY_PAGE_READER -> 左「长按退出」右「翻屏」（WF-F2-04, v1.52
 *     朗读已砍——旧「念给我听」设计稿口径退役）;
 *   substate = KSTORY_PAGE_RECORDING -> 左「收尾」右「结束」（WF-F2-02）;
 *   其余（LIST = 基层页槽表 / STAMP = 演出帧无键帽）-> -1（消费方回落既有路径）。
 *   防御: out_left/out_right 任一 NULL -> -1; 成功返回 0。
 * 设备侧 lcd_display.cc story 子态分支消费（键帽值域仍走本纯逻辑单源, 零重算）。 */
int kidpal_keybar_story_slots(int substate, const char **out_left,
                              const char **out_right);

/* -------------------------------------------------------------------------- *
 * 语音场景生命周期单源（TaskPackage 261022-p3-record-chrome-ghost-fix, CG-A1;
 * additive 追加——既有声明零触碰）。
 *
 * 场景态原先住 lcd_display.cc 文件级 static（设备侧独占、host 不可达）。本包把它上收
 * 本模块：设备侧经 include 消费 set/get/reset，host 侧行为齿可直接驱动（与 dots/
 * slots/char_slots/story_slots 单源同款架构）。跨任务读写 = 对齐 g_story_* 既有宽松度
 * （32 位对齐标量写，单写者语义；调用点分别持 LVGL 锁 / 主循环，无撕裂风险）。
 * ------------------------------------------------------------------------ */

/* 复位值 = 「非语音场景默认」：落在语音场景闭集 [KVST_SCENE_LISTEN..KVST_SCENE_LOWBATT]
 * **之外**——通用分支 kidpal_keybar_slots(page, scene, overlay_top) 遇越界 scene 落 -1
 * 防御，残留语音键帽（「说完啦/结束」「不等了/结束」）绝不再被画出（EQ-3）。 */
#define KIDPAL_KEYBAR_SCENE_NONE (-1)

/* 场景写沿（消费点 = lcd_display.cc kidpal_voice_apply_look：每轮语音演出 / 提醒
 * 弹窗的场景落点）。存真值，get 原样返回。 */
void kidpal_keybar_scene_set(int scene);
/* 场景读沿（消费点 = kidpal_keybar_sync_locked 通用分支入参 +
 * kidpal_chrome_mid_sync_locked 中部显隐判据）。 */
int kidpal_keybar_scene_get(void);
/* 语音会话收尾沿复位（幂等）——写完 get() 恒落 KIDPAL_KEYBAR_SCENE_NONE（出闭集）；
 * 复位后下一轮 set 照常驱动覆盖层键帽（修复不误伤真实语音轮）。 */
void kidpal_keybar_scene_reset(void);

/* story 子态缓存「过渡窗」（story_sub == -1，覆盖层/云端夹钳前台时缓存页态不可见）
 * 的键帽裁决（TaskPackage 261022-p3-record-chrome-ghost-fix, CG-A2; additive）:
 *   story_sub      = kidpal_render_node_story_substate() 现值（-1 = 过渡窗）;
 *   last_known_sub = 调用方注入的「最后已知故事子态」（纯函数无隐藏状态——
 *                    追踪器归属与更新时机归设备侧消费点, 见 GAP-1 申报）;
 *   rec_sub        = 录音子态帧分叉（kidpal_render_node_story_rec_substate）。
 * 返回三态:
 *   -1 : 非法入参（out_left / out_right 任一 NULL）;
 *   0  : 回落既有路径（out_* 不触碰）——非过渡窗（story_sub != -1），或过渡窗但
 *        last_known_sub 不在在途集 {READER, RECORDING, ORGANIZING}（LIST/STAMP/DELETE/
 *        BODY_UNAVAILABLE 等相位不接管，其残留由 scene 复位兜底）;
 *   1  : 过渡窗裁决命中——按最后已知子态出键帽，值域零偏移复用既有冻结单源
 *        （READER/ORGANIZING -> kidpal_keybar_story_slots；RECORDING ->
 *        kidpal_keybar_story_rec_slots 帧分叉）。**内部只经上述两函数取字面**
 *        （KB-E2 单源红线: 本函数不重抄键帽文本, 也绝不出语音轮闭集残留键帽）。
 * 并行函数（不动既有签名与值域——additive, 既有齿不破）。 */
int kidpal_keybar_story_transition_slots(int story_sub, int last_known_sub,
                                         int rec_sub, const char **out_left,
                                         const char **out_right);

/* D3 v7 帧分叉键帽（261019-p3-record-dialogue-guide fixer, 2026-10-20; additive）:
 * RECORDING 子态帧 A/帧 B 键帽集单源。guide_sub == 0 -> 左「收尾」右「丢弃」;
 * guide_sub != 0 -> 左「打断」右留空（空串 = 消费方置空槽）。NULL out -> -1。
 * 消费点 = lcd_display 键帽三槽随态（记录语境录音态）。 */
int kidpal_keybar_story_rec_slots(int guide_sub, const char **out_left,
                                  const char **out_right);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_KEYBAR_H */