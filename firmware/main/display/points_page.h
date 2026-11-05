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

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_POINTS_PAGE_H */
