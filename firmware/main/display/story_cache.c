/*
 * story_cache.c — P3 story 正文本地缓存纯逻辑面实现（TaskPackage
 * 261016-p3-story-body-sd-cache）。
 *
 * seam contract 权威 = firmware/main/test/story_cache.acceptance.c 文件头 [C1]-[C9]；
 * 本文件逐条按冻结签名 / 返回值语义实现，行内注释标注对应齿（[C1]..[C9]）。
 *
 * 纯逻辑红线：零 FATFS / 零 LVGL / 零 FreeRTOS / 零芯片依赖（host_app linux target
 * 与设备树两侧同源）。SD 真 I/O 不在本文件（boards/<板级目录>/story_sd_io.cc）。
 *
 * 决策 24 窄例外：本地副本只是云端归档的只读镜像——本模块只回答「本地有没有 /
 * 该不该取 / 能不能登记」，不产生任何写云动作；写卡成功才登记（[C3]）。
 */

#include "story_cache.h"

#include <string.h>

/* -------------------------------------------------------------------------- *
 * [C1] RAM 索引
 * ------------------------------------------------------------------------ */

int kidpal_story_cache_init(kidpal_story_cache_t *c)
{
    if (c == NULL) {
        return -1;
    }
    memset(c, 0, sizeof(*c));
    return 0;
}

int kidpal_story_cache_clear(kidpal_story_cache_t *c)
{
    if (c == NULL) {
        return -1;
    }
    memset(c, 0, sizeof(*c));
    return 0;
}

int kidpal_story_cache_has(const kidpal_story_cache_t *c, const char *id)
{
    int i;

    if (c == NULL || id == NULL || id[0] == '\0') {
        return 0;
    }
    for (i = 0; i < c->count && i < KSTORY_CACHE_MAX; i++) {
        if (strcmp(c->ids[i], id) == 0) {
            return 1;
        }
    }
    return 0;
}

int kidpal_story_cache_add(kidpal_story_cache_t *c, const char *id)
{
    size_t len;

    if (c == NULL || id == NULL || id[0] == '\0') {
        return -1;
    }
    len = strlen(id);
    if (len >= KSTORY_CACHE_ID_MAX) {
        /* 超长拒绝（绝不截断——截断会把两个不同 id 撞成同一个索引项）。 */
        return -1;
    }
    if (kidpal_story_cache_has(c, id) == 1) {
        return 0; /* 幂等去重：同 id 不产生重复项 */
    }
    if (c->count < 0 || c->count >= KSTORY_CACHE_MAX) {
        return -1; /* 满容量拒绝，count 不变 */
    }
    memcpy(c->ids[c->count], id, len + 1);
    c->count++;
    return 1;
}

int kidpal_story_cache_remove(kidpal_story_cache_t *c, const char *id)
{
    int i;

    if (c == NULL || id == NULL || id[0] == '\0') {
        return -1;
    }
    for (i = 0; i < c->count && i < KSTORY_CACHE_MAX; i++) {
        if (strcmp(c->ids[i], id) == 0) {
            int j;
            for (j = i; j + 1 < c->count; j++) {
                memcpy(c->ids[j], c->ids[j + 1], KSTORY_CACHE_ID_MAX);
            }
            memset(c->ids[c->count - 1], 0, KSTORY_CACHE_ID_MAX);
            c->count--;
            return 1;
        }
    }
    return 0;
}

/* -------------------------------------------------------------------------- *
 * [C2] 当屏缺失判定（AC2 批量预取的输入）
 * ------------------------------------------------------------------------ */

int kidpal_story_cache_missing_on_screen(
        const kidpal_story_cache_t *cache,
        const kidpal_story_item_t *items, int count,
        int screen, int rows_per_screen,
        char out_ids[][KSTORY_CACHE_ID_MAX])
{
    int start;
    int end;
    int i;
    int n = 0;

    if (cache == NULL || items == NULL || out_ids == NULL ||
        rows_per_screen <= 0 || count < 0 || count > KSTORY_CACHE_MAX) {
        return -1;
    }
    if (screen < 0) {
        return 0; /* 越界屏：out_ids 不写 */
    }
    start = screen * rows_per_screen;
    if (start >= count) {
        return 0; /* 越界屏 / 空 list（count == 0 必经此处） */
    }
    end = (screen + 1) * rows_per_screen;
    if (end > count) {
        end = count;
    }
    for (i = start; i < end; i++) {
        if (kidpal_story_cache_has(cache, items[i].id) == 1) {
            continue; /* 已留存 -> 非缺失项 */
        }
        memcpy(out_ids[n], items[i].id, KSTORY_CACHE_ID_MAX);
        out_ids[n][KSTORY_CACHE_ID_MAX - 1] = '\0';
        n++;
    }
    return n;
}

/* -------------------------------------------------------------------------- *
 * [C3] 写卡策略：先写卡成功才登记；写失败只清该篇、不整层清空；卡不在位整层 no-op
 * ------------------------------------------------------------------------ */

int kidpal_story_cache_commit(kidpal_story_cache_t *c, const char *id,
                              int card_ready, int write_ok)
{
    if (c == NULL || id == NULL || id[0] == '\0') {
        return -1;
    }
    if (kidpal_story_id_safe(id) != 1) {
        return -1; /* [C9] 联立：路径注入面 id 绝不进索引 */
    }
    if (card_ready == 0) {
        /* 卡不在位 / 坏卡：整层失效 -> 索引零触碰（不登记、也不清既有条目——
         * 调用方据此走「拉进 RAM 显示、不落盘」路径）。 */
        return -1;
    }
    if (write_ok == 0) {
        /* 写失败：该 id 不登记；若原已在索引则只移除该篇（防读到不存在的文件），
         * 其它条目保留（不整层清空）。 */
        (void)kidpal_story_cache_remove(c, id);
        return -1;
    }
    return kidpal_story_cache_add(c, id);
}

/* -------------------------------------------------------------------------- *
 * [C4] 读回 id 校验（AC7「正文须与所选篇目一致」；additive seam——不改
 *      kidpal_story_open_reader 冻结签名）
 * ------------------------------------------------------------------------ */

int kidpal_story_reader_id_match(const kidpal_story_text_t *st,
                                 const kidpal_story_list_t *list,
                                 int cursor)
{
    if (st == NULL || list == NULL) {
        return 0;
    }
    if (cursor < 0 || cursor >= list->count || cursor >= KSTORY_LIST_MAX) {
        return 0;
    }
    if (st->id[0] == '\0') {
        return 0;
    }
    /* 严格按 id 比对：**绝不**以 title / text / 序号推断（错配 = 显示别篇内容）。 */
    return (strcmp(st->id, list->items[cursor].id) == 0) ? 1 : 0;
}

/* -------------------------------------------------------------------------- *
 * [C5] 双击三分支决策
 * ------------------------------------------------------------------------ */

int kidpal_story_open_decide(int id_match, int has_local, int card_ready,
                             int fetch_failed, int fetch_in_flight,
                             unsigned long wait_ms)
{
    /* has_local 只在 (b) 与 card_ready 联立消费；单独出现不构成命中。 */
    if (id_match == 0) {
        return KSTORY_OPEN_DEGRADE; /* (a) id 错配：拒绝并降级，绝不显示别篇 */
    }
    if (card_ready != 0 && has_local != 0) {
        return KSTORY_OPEN_HIT_CARD; /* (b) 已留存篇目断网照常打开 */
    }
    if (fetch_failed != 0 ||
        (fetch_in_flight != 0 && wait_ms >= KSTORY_FETCH_TIMEOUT_MS)) {
        return KSTORY_OPEN_DEGRADE; /* (c) 离线 / 超时 / 卡不在位且拉取失败 */
    }
    return KSTORY_OPEN_FETCH; /* (d) 未命中 / 卡不在位 / 尚未取回 -> 单篇插队 fetch */
}

/* -------------------------------------------------------------------------- *
 * [C6] 预取决策（plan「关键接线决策 · 预取器不挤占语音主链路」三条可验收齿）
 * ------------------------------------------------------------------------ */

int kidpal_story_prefetch_due(int page_state, int voice_front, int ws_busy,
                              int in_flight, int pending_count,
                              unsigned long since_last_ms)
{
    if (page_state != KSTORY_PAGE_LIST) {
        return 0; /* 录音 / 会话 / 阅读 / 盖章 / 删除 / 取不回提示态一律不发 */
    }
    if (voice_front != 0) {
        return 0; /* 语音前台绝不挤占主链路 */
    }
    if (ws_busy != 0) {
        return 0; /* 发前查 WebSocket 空闲 */
    }
    if (in_flight != 0) {
        return 0; /* 逐篇串行：在途未收不发下一篇 */
    }
    if (pending_count <= 0) {
        return 0;
    }
    return (since_last_ms >= KSTORY_FETCH_INTERVAL_MS) ? 1 : 0;
}

int kidpal_story_prefetch_suspend(int page_state, int voice_front)
{
    if (page_state != KSTORY_PAGE_LIST) {
        return 1; /* 离开 P3（页态迁移）即停 */
    }
    if (voice_front != 0) {
        return 1; /* 语音前台即停 */
    }
    return 0;
}

/* -------------------------------------------------------------------------- *
 * [C7] fetch 应答消费（契约 §1.3④ 消费语义扩展 + AC4「应答到达后进阅读态」）
 * ------------------------------------------------------------------------ */

int kidpal_story_fetch_answer_enter(int awaiting_this_id)
{
    return (awaiting_this_id != 0) ? 1 : 0;
}

/* -------------------------------------------------------------------------- *
 * [C8] 盖章侧效应守卫（fetch 应答恒不盖章；仅 fresh_save 归档路径可演出）
 * ------------------------------------------------------------------------ */

int kidpal_story_stamp_should_fire(int archive_pend, const char *arrived_id,
                                   const char *last_saved_id, int from_fetch)
{
    const char *last = (last_saved_id != NULL) ? last_saved_id : "";

    if (from_fetch != 0) {
        return 0; /* 契约 §1.3④：盖章演出不随 fetch 应答触发 */
    }
    if (archive_pend <= 0) {
        return 0; /* 回看既有故事不盖章 */
    }
    if (arrived_id == NULL || arrived_id[0] == '\0') {
        return 0;
    }
    if (strcmp(arrived_id, last) == 0) {
        return 0; /* 同 id 幂等（防 mcp 回执重试重复演出） */
    }
    return 1;
}

/* -------------------------------------------------------------------------- *
 * [C9] id 路径安全（exitCriteria「SD 路径安全」+ securityAxes「路径注入」）
 * ------------------------------------------------------------------------ */

int kidpal_story_id_safe(const char *id)
{
    size_t i;
    size_t len;

    if (id == NULL || id[0] == '\0') {
        return 0;
    }
    len = strlen(id);
    if (len >= KSTORY_CACHE_ID_MAX) {
        return 0; /* 超长拒绝（防截断撞名） */
    }
    if (strstr(id, "..") != NULL) {
        return 0; /* 父目录穿越 */
    }
    for (i = 0; i < len; i++) {
        unsigned char ch = (unsigned char)id[i];
        int ok = (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
                 (ch >= '0' && ch <= '9') || ch == '.' || ch == '_' || ch == '-';
        if (!ok) {
            return 0; /* 含 '/' / '\\' / 空格 / 非 ASCII 等一律拒绝 */
        }
    }
    return 1;
}
