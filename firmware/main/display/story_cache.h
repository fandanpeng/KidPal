/*
 * story_cache.h — P3 story 正文本地缓存纯逻辑面（TaskPackage
 * 261016-p3-story-body-sd-cache；seam contract [C1]-[C9] 权威 =
 * firmware/main/test/story_cache.acceptance.c 文件头）。
 *
 * 语义权威：docs/requirements.md v1.64 §S-F2.1 AC2/AC7 + 补充规则「正文缓存边界」；
 * 架构决策 24 窄例外（云端 story 归档 = 唯一真相源，板端 SD 只留正文、只读镜像、
 * 一切写入仍走云）；契约 §四 `kidpal_story_fetch` + §1.3④ story_text 消费语义扩展。
 *
 * 纯逻辑（零 FATFS / 零 LVGL / 零 FreeRTOS / 零芯片依赖）：RAM 索引、当屏缺失判定、
 * 读回 id 校验、双击三分支决策、预取决策、fetch 应答消费、盖章侧效应守卫、id 路径
 * 安全——host_app linux target 与设备树两侧同源（对齐 story_page.c / reminder_page.c
 * 既有纯逻辑缝范式）。真 fopen / FATFS 写序在 chip 侧 boards/<板级目录>/story_sd_io.cc
 * （本头尾部只放声明，host 不链接）。
 *
 * 类型镜像纪律：kidpal_story_item_t / kidpal_story_list_t / kidpal_story_text_t /
 * kidpal_story_view_t 全部消费 story_page.h 的在世定义（不另立镜像类型）——冻结验收
 * 文件以「公共前缀逐字同序同型」镜像断言，漂移即行为齿红。
 */
#ifndef KIDPAL_STORY_CACHE_H
#define KIDPAL_STORY_CACHE_H

#include <stddef.h>

#include "story_page.h"  /* kidpal_story_item_t / _list_t / _text_t */

#ifdef __cplusplus
extern "C" {
#endif

/* ---- [C1] RAM 索引（卡上有哪些 id；容量 = story_list 单页窗口上限同值） ---- */
#define KSTORY_CACHE_MAX    16
#define KSTORY_CACHE_ID_MAX 64

typedef struct {
    int count;
    char ids[KSTORY_CACHE_MAX][KSTORY_CACHE_ID_MAX];
} kidpal_story_cache_t;

/* NULL -> -1；其余 count=0 -> 0。 */
int kidpal_story_cache_init(kidpal_story_cache_t *c);
/* NULL -> -1；其余 count=0 -> 0。 */
int kidpal_story_cache_clear(kidpal_story_cache_t *c);
/* 新 id 登记 -> 1；同 id 已在索引 -> 0（幂等去重）；满 KSTORY_CACHE_MAX 再 add ->
 * -1（拒绝、count 不变）；NULL c / NULL id / 空串 -> -1。 */
int kidpal_story_cache_add(kidpal_story_cache_t *c, const char *id);
/* 命中 -> 1；未命中 -> 0；NULL c / NULL id / 空串 -> 0。 */
int kidpal_story_cache_has(const kidpal_story_cache_t *c, const char *id);
/* 移除 -> 1；不在索引 -> 0；NULL c / NULL id / 空串 -> -1。 */
int kidpal_story_cache_remove(kidpal_story_cache_t *c, const char *id);

/* ---- [C2] 当屏缺失判定（AC2「进列表 / 翻屏先行为当屏各篇取回本地缺失的正文」的
 *           批量预取输入） ---- */
/* 返回该屏「本地未留存」的条目数，out_ids 按 items 窗口顺序逐条写入（NUL 结尾）；
 * 无缺失 / 空 list / screen 越界 -> 0（out_ids 不写）；NULL cache / NULL items /
 * NULL out_ids / rows_per_screen <= 0 / count < 0 / count > KSTORY_CACHE_MAX -> -1。
 * 该屏窗口 = [screen*rows_per_screen, min(count, (screen+1)*rows_per_screen))。 */
int kidpal_story_cache_missing_on_screen(
        const kidpal_story_cache_t *cache,
        const kidpal_story_item_t *items, int count,
        int screen, int rows_per_screen,
        char out_ids[][KSTORY_CACHE_ID_MAX]);

/* ---- [C3] 写卡策略（先写卡成功才登记索引；失败只清该篇、不整层清空） ---- */
/* card_ready == 0 -> -1，索引零触碰（调用方走「拉进 RAM 显示、不落盘」路径）；
 * card_ready != 0 && write_ok == 0 -> -1，该 id 不登记；若原已在索引则只移除该篇；
 * card_ready != 0 && write_ok != 0 -> 登记（= add 语义），新增 1 / 已存在 0；
 * NULL c / NULL id / 空串 / 非法 id（[C9]）-> -1（索引零触碰）。 */
int kidpal_story_cache_commit(kidpal_story_cache_t *c, const char *id,
                              int card_ready, int write_ok);

/* ---- [C4] 读回 id 校验（additive seam——**不改** kidpal_story_open_reader 冻结
 *           签名） ---- */
/* st->id 非空 且 cursor ∈ [0, list->count) 且 strcmp 相等 -> 1；其余 -> 0。
 * **绝不**以 title / text / 序号推断。 */
int kidpal_story_reader_id_match(const kidpal_story_text_t *st,
                                 const kidpal_story_list_t *list,
                                 int cursor);

/* ---- [C5] 双击三分支决策（消费侧纯逻辑） ---- */
#define KSTORY_OPEN_HIT_CARD 0 /* 命中本地镜像：读卡进阅读态，零网络帧 */
#define KSTORY_OPEN_FETCH    1 /* 未命中：立即单篇发 kidpal_story_fetch（插队） */
#define KSTORY_OPEN_DEGRADE  2 /* 取不回：可见降级提示（[C0] WF-F2-06 覆盖层） */
#define KSTORY_FETCH_TIMEOUT_MS 10000UL
/* 判定序：(a) id_match == 0 -> 2；(b) card_ready && has_local -> 0；
 * (c) fetch_failed || (fetch_in_flight && wait_ms >= KSTORY_FETCH_TIMEOUT_MS) -> 2；
 * (d) 其余 -> 1。(b) 先于 (c)：已留存篇目断网不被失败态拖去降级。 */
int kidpal_story_open_decide(int id_match, int has_local, int card_ready,
                             int fetch_failed, int fetch_in_flight,
                             unsigned long wait_ms);

/* ---- [C6] 预取决策（显式注入，无真实 timer / 无全局态） ---- */
#define KSTORY_FETCH_INTERVAL_MS 5000UL
/* 仅 page_state == KSTORY_PAGE_LIST && voice_front == 0 && ws_busy == 0 &&
 * in_flight == 0 && pending_count > 0 && since_last_ms >= KSTORY_FETCH_INTERVAL_MS
 * -> 1；其余 -> 0。 */
int kidpal_story_prefetch_due(int page_state, int voice_front, int ws_busy,
                              int in_flight, int pending_count,
                              unsigned long since_last_ms);
/* page_state != KSTORY_PAGE_LIST || voice_front != 0 -> 1（调用方立即挂起并清在途
 * 队列——离开 P3 / 语音前台即停）；其余 -> 0。 */
int kidpal_story_prefetch_suspend(int page_state, int voice_front);

/* ---- [C7] fetch 应答消费 ---- */
/* awaiting_this_id != 0 -> 1（该篇正是双击在等的 -> 解析后进阅读态）；
 * 0 -> 0（后台预取应答：只按 [C3] 写卡 + 更新索引，不动前台）。 */
int kidpal_story_fetch_answer_enter(int awaiting_this_id);

/* ---- [C8] 盖章侧效应守卫（契约 §1.3④：盖章演出不随 fetch 应答触发） ---- */
/* from_fetch != 0 -> 0；archive_pend <= 0 -> 0；arrived_id NULL/空串 -> 0；
 * arrived_id == last_saved_id（last_saved_id NULL 视同空串）-> 0；其余 -> 1。 */
int kidpal_story_stamp_should_fire(int archive_pend, const char *arrived_id,
                                   const char *last_saved_id, int from_fetch);

/* ---- [C9] id 路径安全（写卡前与发帧前消费；禁用户 / 云端输入直通路径拼接） ---- */
/* 1 = 合法：非空、长度 < KSTORY_CACHE_ID_MAX、字符集仅 [A-Za-z0-9._-]、不含子串
 * ".."、不含 '/' / '\\'；其余（含 NULL / 空串 / 超长）-> 0。 */
int kidpal_story_id_safe(const char *id);

/* -------------------------------------------------------------------------- *
 * SD I/O 薄适配面（chip 侧实现 = boards/waveshare-s3-rlcd-4.2/story_sd_io.cc；
 * host_app 不链接——GAP-2 真机 SD 读回 roundtrip 承载）。
 *
 * 设计纪律（architect 预评 ④）：sd_mounted 显式入参注入（不碰板级私有态）；
 * 无卡 / 坏卡短路、绝不 fopen；**先写卡成功才登记 RAM 索引**（由调用方以本函数
 * 返回值为 write_ok 喂 kidpal_story_cache_commit）。
 * ------------------------------------------------------------------------ */

/* 写 /sdcard/story/<id>.txt：1 = 写成功；0 = 卡不在位（整层失效、不落盘）；
 * -1 = 非法 id / 写失败（errno 记日志）。 */
int kidpal_story_sd_write(const char *id, const char *text, int sd_mounted);
/* 读 /sdcard/story/<id>.txt 到 out（NUL 结尾）：1 = 读到；0 = 卡不在位 / 文件不存在；
 * -1 = 非法 id / 参数非法 / 读失败。 */
int kidpal_story_sd_read(const char *id, char *out, size_t cap, int sd_mounted);
/* 启动后 RAM 索引惰性重建（本包非目标 = 热插拔检测）：扫描 /sdcard/story/<id>.txt 把合法
 * id 登记进 cache（重名幂等）。0 = 已完成（含无卡 no-op）；-1 = 参数非法。 */
int kidpal_story_sd_rebuild_index(kidpal_story_cache_t *cache, int sd_mounted);

#ifdef __cplusplus
}
#endif

#endif /* KIDPAL_STORY_CACHE_H */
