/*
 * points_page.h — KidPal P3 星星积分本设备消费面：points_home 载荷最小消费 +
 * restore 分流 + points_ledger 载荷解析与分页缓存纪律。
 *
 * **冻结权威 = firmware/main/test/points_spike.acceptance.c 文件头 seam contract
 * [G2]/[G3]**（TaskPackage 261104-k0-points-e2e-spike）。本头形状与该冻结件内的
 * 类型镜像逐字同序同型（EQ-2「镜像 == 真头」）——改本头前先改冻结件，反之亦然。
 * 冻结纪律：只增不改/移。
 *
 * 纯逻辑、零芯片/FreeRTOS/LVGL 依赖 -> host_app linux target 与设备树两侧同源可编
 * （mcp_server.cc 的 kidpal/display 消费链引用）。C linkage。
 *
 * 降级语义：parse 对 NULL / 非 JSON 对象 / 缺必带键 / 超出容量的载荷一律 -1 ——
 * 调用方必须显式降级、绝不盲渲染 (V27，与 story/schedule 同纪律)。
 * 「数据未达不显示数字」态（S-F6.1 AC4）= 设备从未收到权威帧时的 RAM 初态，由消费
 * 接线层承载、不经本纯逻辑面。
 */
#ifndef KIDPAL_POINTS_PAGE_H
#define KIDPAL_POINTS_PAGE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- 容量常量（冻结件 [G2]/[G3] 镜像，逐字同值） ---- */
#define KPTS_ENTRY_ID_CAP      64
#define KPTS_ENTRY_REASON_CAP  96
#define KPTS_ENTRY_AT_CAP      40
#define KPTS_TARGET_NAME_CAP   64
#define KPTS_TARGET_ASSET_CAP  16
#define KPTS_RECENT_MAX        3   /* 契约 §1.3⑦：recent 时间倒序最多三条 */
#define KPTS_LEDGER_ITEMS_MAX  6   /* 设备缺省请求 limit=6；载荷 items 超容量 = 降级 */
#define KPTS_CURSOR_CAP        128

/* ---- ① 统一分录形状（六键；契约 §1.3⑦⑧ 共用） ---- */
typedef struct {
    char id[KPTS_ENTRY_ID_CAP];
    int delta;                  /* 正数 = 加分、负数 = 兑换 */
    char reason[KPTS_ENTRY_REASON_CAP];
    char occurred_at[KPTS_ENTRY_AT_CAP];
    int balance_before;
    int balance_after;
} kidpal_points_entry_t;

/* ---- ② points_home 载荷（契约 §1.3⑦） ---- */
typedef struct {
    int revision;               /* 单调递增整数 */
    int balance;                /* 可正可负；权威值，设备只显示不计算 */
    char target_name[KPTS_TARGET_NAME_CAP];
    int target_points;          /* 正整数；激活目标分值 */
    char target_asset[KPTS_TARGET_ASSET_CAP];  /* sc-choco/sc-movie/sc-wheel/sc-gift/sc-flag */
    kidpal_points_entry_t recent[KPTS_RECENT_MAX];
    int recent_count;           /* 0..KPTS_RECENT_MAX */
    int restore;                /* 1 = data.restore true；0 = 缺省/false */
} kidpal_points_home_t;

/* ---- ③ points_ledger 载荷（契约 §1.3⑧ / §四 kidpal_points_page） ---- */
typedef struct {
    int revision;
    int balance;
    kidpal_points_entry_t items[KPTS_LEDGER_ITEMS_MAX];
    int item_count;             /* 0..KPTS_LEDGER_ITEMS_MAX */
    int page_total;             /* page.total = 全量分录数 */
    char next_cursor[KPTS_CURSOR_CAP];  /* 空串 = 无续页 */
    int has_more;
    int reset;                  /* page.reset；1 = 设备须先丢弃旧分页缓存 */
    int restore;
} kidpal_points_ledger_t;

/* 设备侧分页缓存视图（RAM，最小面）。 */
typedef struct {
    int revision;               /* 缓存所属账本版本 */
    int cached_count;           /* 已缓存分录条数 */
    int has_more;
} kidpal_points_ledger_view_t;

/* ---- 消费面函数（冻结件 [G2]/[G3] seam 名与签名冻结） ---- */

/* points_home data JSON -> out。0 = 成功；-1 = 显式降级（NULL 载荷 / NULL out /
 * 非 JSON 对象 / 缺 revision、balance、target / target 缺 name 或 points /
 * recent 超过 KPTS_RECENT_MAX 条 = 载荷违反契约、绝不盲渲染）。
 * restore 键 true=1、false/缺省=0；未知扩展字段忽略不消费（additive 纪律）。
 * 注：本函数若失败，out 的内容不保证（调用方不得据其渲染）。 */
int kidpal_points_home_parse(const char *data_json, kidpal_points_home_t *out);

/* restore 分流（D-07 / 契约 §1.3⑦）：restore 缺省/false -> 1 = 主动展示 P3 积分主页
 * （接管前台；设备主动语音记账成功例外跳积分页）；restore=true -> 0 = 只填 RAM 缓存
 * 不抢前台（QQ 写入 / WS 建连 / 删除 / 目标切换推送）；NULL -> -1。 */
int kidpal_points_home_takeover(const kidpal_points_home_t *frame);

/* points_ledger data JSON -> out。0 = 成功；-1 = 显式降级（语义同 points_home_parse；
 * page 四键 total/next_cursor/has_more/reset 必带、缺任一 = -1；items 超
 * KPTS_LEDGER_ITEMS_MAX = -1）。 */
int kidpal_points_ledger_parse(const char *data_json, kidpal_points_ledger_t *out);

/* 分页缓存纪律（契约 §四「不得跨版本追加」）：
 *   f->reset != 0      -> 丢弃旧缓存再装入（cached_count 替换、revision 随帧）-> 0；
 *   f->reset == 0      -> f->revision != v->revision -> -1 拒绝且缓存零扰动；
 *                         同版本 -> cached_count += f->item_count -> 0；
 *   NULL v / NULL f    -> -1。 */
int kidpal_points_ledger_apply(kidpal_points_ledger_view_t *v,
                               const kidpal_points_ledger_t *f);

/* ========================================================================== *
 * K2 正式积分本 additive seam（TaskPackage 261104-k2-points-device-ui；
 * seam 权威 = firmware/main/test/points_device_ui.acceptance.c 文件头 [P1]-[P7]）。
 * 冻结纪律：本节只增不改/移——上方 K0 [G2]/[G3] 形状与函数签名零触碰。
 * 纯逻辑、零芯片/FreeRTOS/LVGL 依赖（host_app linux target 与设备树同源）。
 * ========================================================================== */

/* ---- [P1] 积分主页四态（S-F6.1 AC1/AC4/AC5） ---- */
#define KPTS_HOME_STATE_NODATA   0  /* 数据未达：从未收到权威帧，不显示数字/进度线 */
#define KPTS_HOME_STATE_EMPTY    1  /* 确认空账：显示 0 与目标，进度线照常画 */
#define KPTS_HOME_STATE_NORMAL   2  /* 正常（含超目标，同页结构） */
#define KPTS_HOME_STATE_NEGATIVE 3  /* 负余额：如实显示负值不藏数 */

/* frame NULL（RAM 初态 = 未达）-> NODATA；balance<0 -> NEGATIVE；
 * balance==0 且 recent_count==0 -> EMPTY；其余 -> NORMAL。 */
int kidpal_points_home_state(const kidpal_points_home_t *frame);

/* ---- [P2] 进度线画法数据面（S-F6.1 AC2/AC3/AC5；视觉排版归 design review） ---- */
typedef struct {
    int valid;          /* 1 = 本帧可画进度线；负余额也画（钳位） */
    int pos;            /* 小伴位置：0..target；超目标 > target */
    int target_points;  /* 激活目标分值 */
    int solid_len;      /* 实线段长（已走过部分）；负余额 = 0（全虚线） */
    int overshoot;      /* 1 = 余额超激活目标（实线越场景标延伸、同页结构） */
} kidpal_points_progress_t;

/* frame/out NULL -> -1（不画）。负余额：valid=1/pos=0/solid_len=0/overshoot=0
 * （钳位画法；frame.balance 只读不改）。0<=balance<=target：pos=solid_len=balance。
 * balance>target：pos=solid_len=balance、overshoot=1。成功 0。 */
int kidpal_points_progress(const kidpal_points_home_t *frame,
                           kidpal_points_progress_t *out);

/* ---- [P3] 键位决策表（§2.3 P3 积分主页行 / P3 详细账本行） ---- */
#define KPTS_GESTURE_KEY_SHORT   0
#define KPTS_GESTURE_KEY_DOUBLE  1
#define KPTS_GESTURE_KEY_LONG    2
#define KPTS_GESTURE_BOOT_SHORT  3
#define KPTS_GESTURE_BOOT_DOUBLE 4
#define KPTS_GESTURE_BOOT_LONG   5

#define KPTS_ACT_VOICE_SESSION   0  /* 开语音会话（积分语境） */
#define KPTS_ACT_OPEN_LEDGER     1  /* KEY 双击进详细账本 */
#define KPTS_ACT_BACK            2  /* KEY 长按逐层返回 */
#define KPTS_ACT_CURSOR_DOWN     3  /* BOOT 短按光标下移（永不离页） */
#define KPTS_ACT_NEXT_SCREEN     4  /* BOOT 双击翻下一屏 */
#define KPTS_ACT_DELETE_ENTER    5  /* BOOT 长按进删除确认子态 */
#define KPTS_ACT_NONE           (-1)

int kidpal_points_home_key_action(int gesture);
int kidpal_points_ledger_key_action(int gesture);

/* [P3b] 积分主页单击消歧门（§2.3 消歧注记第二处；故事域归 KSPV2 自己的门）。
 * page == KPTS_PAGE_POINTS_HOME -> 1（挂起）；其余（含 P4 故事）-> 0。 */
#define KPTS_DISAMBIG_MS 300u
#define KPTS_PAGE_POINTS_HOME 2   /* KP_PAGE_POINTS_HOME K0a 冻结镜像（四页骨架 [1]） */
int kidpal_points_debounce_gate(int page);

/* ---- [P4] 删除确认子态 + 隐式 ack（S-F6.2 AC3/AC4/AC5 + 契约 §四） ---- */
#define KPTS_DEL_GESTURE_KEY_SHORT   0
#define KPTS_DEL_GESTURE_BOOT_SHORT  1
#define KPTS_DEL_GESTURE_BOOT_DOUBLE 2
#define KPTS_DEL_GESTURE_BOOT_LONG   3
#define KPTS_DEL_GESTURE_OTHER       4

#define KPTS_DEL_ACT_NONE     0
#define KPTS_DEL_ACT_CONFIRM  1  /* 发送 kidpal_points_delete；绝不本地删缓存 */
#define KPTS_DEL_ACT_CANCEL   2  /* 取消回详细账本 */

int kidpal_points_delete_confirm_decide(int gesture);

/* 10 秒静默取消（不显示倒计时）。累计 idle 并判到点：>= 10000 -> 1（调用方静默
 * 取消回账本并复位）；未到点 0；NULL -> -1。任意手势重计 = 输入消费侧义务。 */
#define KPTS_DELETE_IDLE_MS 10000UL
int kidpal_points_delete_idle_step(unsigned long *idle_ms, unsigned long dt_ms);

/* 超时重发：tries < 3 且 age >= 5000 -> 1（调用方重发同一 id 并 tries++）；
 * 否则 0（放弃后静默停发，不假删）。 */
#define KPTS_DELETE_RESEND_MS  5000UL
#define KPTS_DELETE_RESEND_MAX 3
int kidpal_points_delete_resend_due(int tries, uint32_t age_ms);

/* 隐式 ack 对账：pending_id 非空且命中 frame->items -> 1（仍存在，不冲销）；
 * 不在窗口（含 items 空）或 pending 空串 -> 0（调用方冲销 pending）；
 * pending NULL / frame NULL -> -1。 */
int kidpal_points_delete_reconcile_hit(const char *pending_id,
                                       const kidpal_points_ledger_t *frame);

/* ---- [P5] keyset 全量分页客户端状态（S-F6.2 AC1 + 契约 §四；RAM，掉电即失） ---- */
#define KPTS_PAGE_REQ_LIMIT 6   /* 设备缺省请求 limit（= KPTS_LEDGER_ITEMS_MAX） */

typedef struct {
    int revision;           /* 缓存所属账本版本 */
    int cached_count;       /* 已缓存分录条数 */
    int has_more;
    int page_total;         /* 最近权威帧 page.total（不用 items.length 顶替） */
    char next_cursor[KPTS_CURSOR_CAP];
} kidpal_points_ledger_state_t;

/* reset!=0 -> 先清旧分页再装首页（全字段随帧替换）-> 0；
 * reset==0 且 revision 不符 -> -1（跨版本追加拒绝、st 零扰动）；
 * 同版本 -> cached_count += item_count、page_total/next_cursor/has_more 随帧 -> 0；
 * NULL 入参 -> -1。 */
int kidpal_points_ledger_state_apply(kidpal_points_ledger_state_t *st,
                                     const kidpal_points_ledger_t *f);

/* 续页请求驱动：有续页（has_more 且 next_cursor 非空）-> 拷贝游标（cap 不足 -1）、
 * known_revision=st->revision、limit=KPTS_PAGE_REQ_LIMIT，返回 1；
 * 否则 cursor_out 置空串、known_revision=(revision>0?revision:-1)、limit=6，返回 0。
 * 任一输出指针 NULL -> -1。 */
int kidpal_points_ledger_next_request(const kidpal_points_ledger_state_t *st,
                                      char *cursor_out, size_t cap,
                                      int *known_revision_out, int *limit_out);

/* 「共 y 屏」：以 page_total（全量）为准，ceil(page_total / rows_per_screen)；
 * 不用 cached_count 顶替。st NULL / rows<=0 -> -1。 */
int kidpal_points_ledger_screen_total(const kidpal_points_ledger_state_t *st,
                                      int rows_per_screen);

/* ---- [P6] selected_id 只读注入决策（D-08 + 契约 §四） ---- */
/* frame NULL -> -1；selected_id NULL/空串 -> 0（不附带）；命中 items -> 1；
 * 未命中 -> 0（设备不造 id）。 */
int kidpal_points_selected_id_injectable(const kidpal_points_ledger_t *frame,
                                         const char *selected_id);

/* ---- [P7] 积分子态（键帽槽裁决入参；键帽字面住 display/keybar.c） ---- */
#define KPTS_SUBSTATE_NONE           0  /* 基层 P3 主页（回落页槽表） */
#define KPTS_SUBSTATE_LEDGER         1  /* 详细账本列表态 */
#define KPTS_SUBSTATE_DELETE_CONFIRM 2  /* 删除确认整屏覆盖层子态 */

/* ---- 积分 UI 当前语境（RAM 单例；消费点 = mcp_server 写 / application 读） ----
 * 与 keybar.c 的 g_kidpal_keybar_scene 同款「设备侧消费态上收纯逻辑模块」先例
 * （261022-p3-record-chrome-ghost-fix CG-A1）。32 位对齐标量 + 定长 id 单写者，
 * 不另建锁。 */
void kidpal_points_ui_set_substate(int substate);   /* 越界 -> 归 KPTS_SUBSTATE_NONE */
int  kidpal_points_ui_substate(void);
void kidpal_points_ui_set_selected_id(const char *id); /* NULL/空串 = 清空 */
const char *kidpal_points_ui_selected_id(void);        /* 恒非 NULL（无选中时为空串） */

/* ========================================================================== *
 * K4 奖励场景 additive seam（TaskPackage 261104-k4-reward-goal-config；
 * seam 权威 = firmware/main/test/reward_goal_config.acceptance.c 文件头 [G1]/[G2]）。
 * 冻结纪律：本节只增不改/移——上方 K0/K2 形状与函数签名零触碰。
 * 纯逻辑、零芯片/FreeRTOS/LVGL 依赖（host_app linux target 与设备树同源），
 * 使设备 TU kidpal_render_node.cc 只消费映射结果、不再持资产名字面量。
 * ========================================================================== */

/* ---- [G1] 契约逻辑标识 -> assets.h 符号名（= 落盘 <name>.bin 根名） ----
 * 五键映射唯一单源（sc-choco/sc-movie/sc-wheel/sc-gift/sc-flag）。未知标识 /
 * NULL / 空串 -> NULL（绝不猜图；调用方不画场景标）。返回值是编译期字面量，
 * 调用方可安全长期持有指针。 */
const char *kidpal_points_scene_asset(const char *target_asset);

/* ---- [G2] 刻度几何：分值 -> 进度线横坐标 ----
 * target_points <= 0 / track_w <= 0 / x_out == NULL -> -1（不画）；
 * points <= 0 -> *x_out = 0；否则 *x_out = points * track_w / target_points
 * （整数除法向下取整；points == target 恰落 track_w，points > target 越界延伸
 * 到 track_w 之外，绘制裁切归 render_node/design）。成功 0。 */
int kidpal_points_scale_x(int target_points, int points, int track_w, int *x_out);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_POINTS_PAGE_H */
