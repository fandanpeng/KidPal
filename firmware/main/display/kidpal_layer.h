/*
 * kidpal_layer.h — KidPal 前台 layer 栈管理单一权威 (TaskPackage
 * 260911-screen-unification, PKG-2 归一屏)。
 *
 * 语义与数值码的**现役权威** = 冻结验收 firmware/main/test/char_window_overlay.acceptance.c
 * 文件头 FROZEN OFFLINE CONTRACT seam [1]（2026-09-19 M2-K3：插入查字窗层 + REMINDER /
 * PROVISION 上移；下层 LOCAL_PAGE / VOICE / SERVER_PAGE 三值零偏移）。层模型初版来源
 * screen_unification.acceptance.c 已归档（firmware/main/test/acceptance/archive/，不编入
 * runner、冻结不改——其内冻结数值字面停留于「插入前闭集」，非现役权威）。纯逻辑、无
 * chip/FreeRTOS/ESP-component/cJSON 依赖 ->
 * 可编入 host_app linux target 与设备树两侧（host 强符号覆盖 weak 哨兵，
 * 设备侧由 kidpal_render_node.cc 作为生产消费者）。实现见 kidpal_layer.c。
 *
 * 本模块 = 前台参与者优先级栈合并体：
 *   (1) 前台参与者优先级栈（本地浏览页 / 服务端推送页 / 语音轮 / 提醒弹窗 / 配网 /
 *       查字窗）；
 *   (2) 聊天 layer 常驻性 + 驻留清除时机（OQ3=c）+ 聊天内容落点。
 * 不新建独立状态机对象（PROPOSAL §4）：layer 栈本身即前台 owner 的单一权威。
 */
#ifndef KIDPAL_LAYER_H
#define KIDPAL_LAYER_H

#ifdef __cplusplus
extern "C" {
#endif

/* --- 常量（冻结数值码，不得偏移） ---
 * 2026-09-19 留痕修订（TaskPackage 260917-m2-k3-char-window-overlay, M2-K3 页→窗改造,
 * seam [1]）: **插入**查字窗层并把 REMINDER / PROVISION 整体上移；下层 LOCAL_PAGE /
 * VOICE / SERVER_PAGE 三值零偏移（全仓消费点均为符号常量引用、零数值字面耦合，故
 * 「插入 + 上移」是符号级安全改动）。
 * 2026-09-29 留痕修订（TaskPackage 260924-m4-k2-p3-star-notebook-ui, seam [K10]）:
 * **插入** P3 删除确认覆盖层 KIDPAL_LAYER_STORY_DELETE=6 并把 REMINDER / PROVISION
 * 整体再上移（M2-K3 同款「符号级插入 + 上移」先例；全仓消费点仍均为符号常量引用）。
 * 新层值语义权威 = 冻结验收 firmware/main/test/story_page_v2.acceptance.c seam [K10]
 * （本头注「现役权威」对该层值而言迁指该文件；查字窗层等既有值仍以 char_window_overlay
 * 文件头为准）。合法编码种数 = 7（闭集 LOCAL_PAGE..PROVISION），仍 <
 * KIDPAL_LAYER_STACK_MAX(8)。 */
#define KIDPAL_LAYER_STACK_MAX    8
#define KIDPAL_LAYER_UNIMPLEMENTED (-2)
#define KIDPAL_LAYER_NONE         0
#define KIDPAL_LAYER_CHAT         1
#define KIDPAL_LAYER_LOCAL_PAGE   2
#define KIDPAL_LAYER_VOICE        3
#define KIDPAL_LAYER_SERVER_PAGE  4
/* 查字窗（会话内覆盖窗，非常驻页；v1.36 撤 P4 常驻页改窗）：覆盖当前页之上、提醒之下。 */
#define KIDPAL_LAYER_CHAR_WINDOW  5
/* P3 删除确认覆盖层（260924-m4-k2-p3-star-notebook-ui seam [K10]）：覆盖当前页之上、
 * 提醒之下——「提醒最高层压删除弹窗」序不变（STORY_DELETE < REMINDER, 矩阵口径）。 */
#define KIDPAL_LAYER_STORY_DELETE 6
#define KIDPAL_LAYER_REMINDER     7
#define KIDPAL_LAYER_PROVISION    8

/* 前台优先级 = 数值越大越靠前：配网 > 提醒 > 删除确认弹窗 > 查字窗 > 服务端推送页 >
 * 语音轮 > 本地浏览页（§2.2 行 67 提醒 = 最高层覆盖须压过删除弹窗/查字窗；行 68 查字窗
 * 在当前页之上）。 */

#define KIDPAL_LAYER_EVT_ANSWER_END  0
#define KIDPAL_LAYER_EVT_PAGE_SWITCH 1
#define KIDPAL_LAYER_EVT_NEW_VOICE   2
#define KIDPAL_LAYER_EVT_KEY_PRESS   3

/* --- 类型形状（冻结，不得偏移） --- */
typedef struct {
    int layers[KIDPAL_LAYER_STACK_MAX];   /* 前台参与者栈（实际内部序实现自由） */
    int n;                                /* 当前层数 */
} kidpal_layer_stack_t;

int kidpal_layer_push(kidpal_layer_stack_t *st, int layer);
int kidpal_layer_pop(kidpal_layer_stack_t *st, int layer);
int kidpal_layer_top(const kidpal_layer_stack_t *st);
int kidpal_layer_has(const kidpal_layer_stack_t *st, int layer);
int kidpal_layer_chat_target(void);
int kidpal_layer_chat_clear_event(int event);

/* 显式清层步骤（TaskPackage 260915-m1-k1a-key-layer-dispatch, KD-A2 清层/进页分离）：
 * 「关窗」唯一事实源用的纯逻辑步骤。出掉全部 transient 覆盖层
 * （KIDPAL_LAYER_VOICE / KIDPAL_LAYER_SERVER_PAGE / KIDPAL_LAYER_REMINDER /
 * KIDPAL_LAYER_CHAR_WINDOW / KIDPAL_LAYER_STORY_DELETE，各至多一条、幂等）；
 * KIDPAL_LAYER_LOCAL_PAGE / KIDPAL_LAYER_PROVISION 不碰（配网层零触碰）。
 * 返回出掉的层数（>=0）；NULL st -> -1。空栈/纯基层栈 -> 0。
 * 2026-09-19 留痕修订（260917-m2-k3-char-window-overlay, seam [1]）: transient 集纳入
 * 查字窗层——查字窗是 transient 覆盖层，关窗 = 出清层回基层（回原页、不翻页）。
 * 2026-09-29 留痕修订（260924-m4-k2-p3-star-notebook-ui, seam [K10]）: transient 集纳入
 * 删除确认覆盖层——取消/确认后的关层 = 出清层回基层（回列表、不翻页）。 */
int kidpal_layer_pop_transients(kidpal_layer_stack_t *st);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_LAYER_H */