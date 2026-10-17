/*
 * Frozen acceptance tests — TaskPackage 261016-p3-story-body-sd-cache
 * (subsystem: firmware/main 纯逻辑面 + 跨端结构面; rigor=full / riskTier=high)。
 *
 * Story 语义权威 = docs/requirements.md v1.64 §S-F2.1（AC2「双击进阅读一视同仁」+ AC7
 * 离线/降级可见提示 + 补充规则「正文缓存边界」整节照录见 input-brief ①b）+ §2.3 矩阵
 * 新行「P3 · 正文取不回提示」与注记（KEY／BOOT 任一手势立即关闭回列表；10 秒无操作
 * 自动消失、不显示倒计时）。
 * 契约形状权威 = docs/contracts/interface-contract.md §四（kidpal_story_fetch：ids 批量
 * 上行私有帧、非法/不存在 id 拒绝 + 审计 + 不回帧、空 ids no-op、应答复用既有 story_text
 * 帧零新形状）+ §1.3④（story_text 消费语义扩展：fetch 应答不触发盖章演出、卡不在位 /
 * 坏卡不留存当场显示）。
 * 上游冻结 = delivery/review/261017-arch-review-p3-story-body-sd-cache/（决策 24 窄例外 +
 * ADR-011）；reviewed plan = delivery/taskpackages/261016-p3-story-body-sd-cache/plan.md
 * （含 2026-10-17 architect 前置技术预评采纳：新增纯逻辑 seam、不改
 * kidpal_story_open_reader 冻结签名）。
 *
 * Nature: 行为级（纯逻辑决策函数 + weak 哨兵模式——story_cache.c 未编入 host runner 前
 * 哨兵恒返回 KSC_UNIMPLEMENTED = 「frozen contract not implemented」设计态红）+
 * 结构级（真实工作树扫描，kld / story_page_v2 先例形态）。
 *
 * 本文件**不 include** display/story_cache.h（该头 builder 尚未落盘 / 落地后形状以本文件
 * 头注为冻结契约）——镜像类型的公共前缀与 story_page.h 现有类型逐字同序同型（story_page_v2
 * 先例）。落地后由 host runner 的强符号覆盖本文件 weak 哨兵。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；「home =」为冻结落点）:
 *
 *   [C0] 页态 additive（home = firmware/main/display/story_page.h）:
 *          KSTORY_PAGE_BODY_UNAVAILABLE = 5
 *            -- P3 正文取不回提示整屏覆盖层（WF-F2-06，与 WF-F2-05 DELETE=4 同族，
 *               尾部追加；LIST0/READER1/RECORDING2/STAMP3/DELETE4 数值零改）。
 *          #define KSTORY_BODY_UNAVAILABLE_HINT "这篇暂时打不开"
 *          #define KSTORY_BODY_UNAVAILABLE_SUB  "等会儿再试试吧"
 *            -- 话术单源（docs/ui/UIUX-DESIGN-BRIEF.md v5 定稿；设计稿 v6
 *               p3-body-unavailable 逐字）。两宏住 story_page.h（既有
 *               「话术单源 = story_page.h」纪律，KSTORY_EMPTY_HINT 同款）。
 *          #define KSTORY_BODY_UNAVAILABLE_IDLE_MS 10000UL
 *          int kidpal_story_body_unavailable_idle_step(unsigned long *idle_ms,
 *                                                      unsigned long dt_ms);
 *            -- 10s 无操作自动消失（home = display/story_page.c；形参/语义逐字仿
 *               kidpal_story_delete_idle_step）：累计 idle 并判到点，>= 10000 返回 1
 *               （调用方静默关闭回列表并复位），未到点 0，NULL -> -1。**静默**——不显示
 *               倒计时（负向结构齿 = 文案单源里不得出现秒数字面，见 TEST 9 tooth 4）。
 *          手势关闭（home = display/story_page.c kidpal_story_page_input）：
 *            page == KSTORY_PAGE_BODY_UNAVAILABLE 时 KEY／BOOT **任一手势**
 *            （短按/双击/长按）一律关闭回列表（v->page == KSTORY_PAGE_LIST），
 *            两键语义一致、无破坏性动作、无 GLOBAL 外泄。
 *
 *   [C1] RAM 索引（home = firmware/main/display/story_cache.{c,h}；纯逻辑、零 FATFS 依赖、
 *        host_app linux target 与设备树两侧同源）:
 *          #define KSTORY_CACHE_MAX 16
 *          #define KSTORY_CACHE_ID_MAX 64
 *          typedef struct { int count; char ids[KSTORY_CACHE_MAX][KSTORY_CACHE_ID_MAX]; }
 *              kidpal_story_cache_t;
 *          int kidpal_story_cache_init(kidpal_story_cache_t *c);
 *            -- NULL -> -1; 其余 count=0 -> 0。
 *          int kidpal_story_cache_clear(kidpal_story_cache_t *c);
 *            -- NULL -> -1; 其余 count=0 -> 0。
 *          int kidpal_story_cache_add(kidpal_story_cache_t *c, const char *id);
 *            -- 新 id 登记 -> 1; 同 id 已在索引 -> 0（幂等去重、不产生重复项）;
 *               满 KSTORY_CACHE_MAX 再 add -> -1（拒绝、count 不变）;
 *               NULL c / NULL id / 空串 -> -1。
 *          int kidpal_story_cache_has(const kidpal_story_cache_t *c, const char *id);
 *            -- 命中 -> 1; 未命中 -> 0; NULL c / NULL id / 空串 -> 0。
 *          int kidpal_story_cache_remove(kidpal_story_cache_t *c, const char *id);
 *            -- 移除 -> 1; 不在索引 -> 0; NULL c / NULL id / 空串 -> -1。
 *
 *   [C2] 当屏缺失判定（home = story_cache.{c,h}; AC2「进列表 / 翻屏先行为当屏各篇取回
 *        本地缺失的正文」的批量预取输入）:
 *          int kidpal_story_cache_missing_on_screen(
 *                  const kidpal_story_cache_t *cache,
 *                  const kidpal_story_item_t *items, int count,
 *                  int screen, int rows_per_screen,
 *                  char out_ids[][KSTORY_CACHE_ID_MAX]);
 *            -- 返回该屏「本地未留存」的条目数，out_ids 按 items 窗口顺序逐条写入
 *               （0-based id 串、NUL 结尾）；无缺失 / 空 list / screen 越界 ->
 *               0（out_ids 不写）；NULL cache / NULL items / NULL out_ids /
 *               rows_per_screen <= 0 / count < 0 / count > KSTORY_CACHE_MAX -> -1。
 *               该屏窗口 = [screen*rows_per_screen, min(count, (screen+1)*rows_per_screen))。
 *
 *   [C3] 写卡策略（home = story_cache.{c,h}; architect 预评 ④「先写卡成功才登记索引，
 *        写失败只清该篇、正文照常 RAM 显示（不整层清空）」+ 决策 24 只读镜像）:
 *          int kidpal_story_cache_commit(kidpal_story_cache_t *c, const char *id,
 *                                        int card_ready, int write_ok);
 *            -- card_ready == 0（卡不在位 / 坏卡整层失效）-> -1，索引**零触碰**
 *               （不登记、不清空整层——调用方据此走「拉进 RAM 显示、不落盘」路径）;
 *               card_ready != 0 && write_ok == 0 -> -1，该 id 不登记；若该 id 原已在
 *               索引则**只移除该篇、其它条目保留**（不整层清空）;
 *               card_ready != 0 && write_ok != 0 -> 登记（= add 语义）: 新增 1 /
 *               已存在 0; NULL c / NULL id / 空串 -> -1（索引零触碰）。
 *            -- 强度申报：本齿钉「登记以写卡成功为门 + 失败只清该篇」的**决策半**；
 *               真 fopen / FATFS 写序（先写后 commit 的调用顺序）在 chip 侧
 *               story_sd_io.cc，host 不可达 -> 真机 SD 读回 roundtrip 承载（见下 GAP-2）。
 *
 *   [C4] 读回 id 校验（home = story_cache.{c,h}; AC7「正文须与所选篇目一致」+ AC2
 *        「双击只认云端列表里存在的篇目」；additive seam——**不改** kidpal_story_open_reader
 *        冻结签名，architect 预评 ①）:
 *          int kidpal_story_reader_id_match(const kidpal_story_text_t *st,
 *                                           const kidpal_story_list_t *list,
 *                                           int cursor);
 *            -- st->id 非空 且 cursor ∈ [0, list->count) 且
 *               strcmp(st->id, list->items[cursor].id) == 0 -> 1; 其余（不等 / 空 id /
 *               cursor 越界 / NULL 入参）-> 0。**绝不**以 title / text / 序号推断。
 *
 *   [C5] 双击三分支决策（home = story_cache.{c,h}; AC2 / AC4 / AC5 / AC7 的消费侧纯逻辑）:
 *          #define KSTORY_OPEN_HIT_CARD 0   -- 命中本地镜像: 读卡进阅读态, 零网络帧
 *          #define KSTORY_OPEN_FETCH    1   -- 未命中: 立即单篇发 kidpal_story_fetch
 *                                             （插队语义 = 绕过预取串行/间隔闸直接发该 id）
 *          #define KSTORY_OPEN_DEGRADE  2   -- 取不回: 可见降级提示（[C0] 覆盖层）
 *          #define KSTORY_FETCH_TIMEOUT_MS 10000UL
 *          int kidpal_story_open_decide(int id_match, int has_local, int card_ready,
 *                                       int fetch_failed, int fetch_in_flight,
 *                                       unsigned long wait_ms);
 *            -- 判定序（先匹配、再本地、再失败、最后等待超时）:
 *               (a) id_match == 0 -> 2（id 错配：拒绝并走降级，**绝不显示别篇内容**）;
 *               (b) card_ready != 0 && has_local != 0 -> 0（已留存篇目断网照常打开）;
 *               (c) fetch_failed != 0
 *                   || (fetch_in_flight != 0 && wait_ms >= KSTORY_FETCH_TIMEOUT_MS)
 *                   -> 2（离线 / 超时 / 卡不在位且拉取失败 -> 可见反馈）;
 *               (d) 其余 -> 1（未命中 / 卡不在位 / 尚未取回 -> 发单篇 fetch）;
 *               注意 (b) 先于 (c)：已留存篇目断网不被失败态拖去降级。
 *
 *   [C6] 预取决策（home = story_cache.{c,h}; plan「关键接线决策 · 预取器不挤占语音主链路」
 *        的三条可验收齿，全部显式注入、无真实 timer / 无全局态）:
 *          #define KSTORY_FETCH_INTERVAL_MS 5000UL
 *          int kidpal_story_prefetch_due(int page_state, int voice_front, int ws_busy,
 *                                        int in_flight, int pending_count,
 *                                        unsigned long since_last_ms);
 *            -- page_state == KSTORY_PAGE_LIST(0) && voice_front == 0 && ws_busy == 0
 *               && in_flight == 0 && pending_count > 0
 *               && since_last_ms >= KSTORY_FETCH_INTERVAL_MS -> 1; 其余 -> 0。
 *               语义: 仅列表态且无语音前台才发（录音 / 会话 / 阅读 / 盖章 / 删除 /
 *               取不回提示态一律不发）; in_flight==0 = 逐篇串行; 篇间间隔 >= 5000ms;
 *               发前 WS 空闲。
 *          int kidpal_story_prefetch_suspend(int page_state, int voice_front);
 *            -- page_state != KSTORY_PAGE_LIST || voice_front != 0 -> 1（调用方立即挂起并
 *               清在途队列——离开 P3 / 语音前台即停）; 其余 -> 0。
 *
 *   [C7] fetch 应答消费（home = story_cache.{c,h}; 契约 §1.3④ 消费语义扩展 + AC4
 *        「应答到达后进阅读态」）:
 *          int kidpal_story_fetch_answer_enter(int awaiting_this_id);
 *            -- awaiting_this_id != 0 -> 1（该篇正是双击在等的 -> 解析后进阅读态）;
 *               0 -> 0（后台预取应答：只按 [C3] 写卡 + 更新索引，不动前台）。
 *
 *   [C8] 盖章侧效应守卫（home = story_cache.{c,h}; 契约 §1.3④「盖章演出不随 fetch 应答
 *        触发」+ architect 预评 ⑧「fetch 路径不置 archive_pend / 不调
 *        kidpal_story_on_saved」）:
 *          int kidpal_story_stamp_should_fire(int archive_pend, const char *arrived_id,
 *                                             const char *last_saved_id, int from_fetch);
 *            -- from_fetch != 0 -> 0（fetch 应答恒不盖章）;
 *               archive_pend <= 0 -> 0; arrived_id == NULL / 空串 -> 0;
 *               arrived_id == last_saved_id（含 last_saved_id == NULL 视同空串）-> 0
 *               （同 id 幂等，不重复演出）; 其余 -> 1（仅 fresh_save 归档路径）。
 *
 *   [C9] id 路径安全（home = story_cache.{c,h}; exitCriteria「SD 路径安全」+ securityAxes
 *        「路径注入」）:
 *          int kidpal_story_id_safe(const char *id);
 *            -- 1 = 合法: 非空、长度 < KSTORY_CACHE_ID_MAX、字符集仅 [A-Za-z0-9._-]、
 *               不含子串 ".."、不含 '/' / '\\'; 其余（含 NULL / 空串 / 超长）-> 0。
 *               消费点 = 写卡前（[C3] 内部或调用侧）与发帧前；禁用户 / 云端输入直通路径
 *               拼接。
 *
 *   [C10] 跨端发送原语（home = firmware/main/protocols/protocol.{h,cc} +
 *         application.{h,cc} + mcp_server.cc 消费点）:
 *          void SendKidpalStoryFetch(const char *const *ids, int count)
 *            -- 契约 §四线形状（逐字）:
 *               {"session_id":"<sid>","type":"kidpal_story_fetch","ids":["<id>",...]}
 *               照 SendKidpalStoryDelete 同型；count <= 0 = 空 ids no-op（不发帧）。
 *               本文件只做结构齿（符号链 + 字面），wire 行为由
 *               server/acceptance/test_story_fetch.py 在对端**真消费**（SC-A7）。
 *
 * ===========================================================================
 * 契约条目 / 决策 ↔ 齿 的强度申报（acceptance-evidence「契约条目 ↔ 齿」节）:
 *
 * | 契约 / 决策条目 | 承载齿 | 性质档 |
 * |---|---|---|
 * | 决策 24 窄例外（只读镜像 / 只留正文 / 写仍走云） | [C1]-[C3] 索引 + 写卡策略；TEST 1/3/6 | 行为级（决策半） |
 * | 契约 §四 kidpal_story_fetch 上行形状 | [C10] 结构齿 TEST 10 + 对端 pytest 真消费 | 机制级（符号/字面）+ 行为级（对端） |
 * | 契约 §1.3④ fetch 应答不盖章 / 卡不在位不留存 | [C3]/[C7]/[C8]；TEST 6/8 | 行为级（决策半） |
 * | S-F2.1-AC2 双击一视同仁 + 当屏预取 | [C2]/[C4]/[C5]；TEST 2/3/4 | 行为级 |
 * | S-F2.1-AC7 可见提示 + 正文与所选篇目一致 | [C0]/[C4]/[C5]；TEST 3/4/9 | 行为级 + 源锚代理（文案单源扫描） |
 * | 预取不挤占语音主链路 | [C6]；TEST 5 | 行为级 |
 * | id 错配修复（TECH-261013） | [C4]/[C5]；TEST 3/4 | 行为级 |
 * | 路径注入（securityAxes） | [C9]；TEST 7 + TEST 6 负向 | 行为级 |
 *
 * 弱于契约处（显式缺口，不得当已兑现）:
 *   GAP-1（渲染 / 观感）：WF-F2-06 的整屏覆盖层像素、所选条目行（星星＋标题＋日期）、
 *         键帽 [知道了][关闭]、「不黑屏」的视觉证据——host 纯逻辑面不可达（需 LVGL 真帧
 *         且属设计观感），归 live-manual 真机轮 + design review；本文件只钉页态值、文案
 *         单源、10s idle、手势关闭四条骨架。
 *   GAP-2（真实 SD I/O）：fopen / FATFS 写序 / 坏卡检测 / 重启后读回 roundtrip 在 chip 侧
 *         story_sd_io.cc，host 不可达 -> 真机 live-manual 轮（SC-A9）。本文件钉登记门控。
 *   GAP-3（插队优先级）：双击 FETCH 的「优先级高于后台预取」以 [C5](d) 的返回码 + 调用侧
 *         语义声明承载；队列内部排序无纯逻辑可钉（builder 设计自由）-> 真机复看 + 代码
 *         复核。**不得**把 TEST 4 的绿读成「插队顺序已证」。
 *   GAP-4（音频不卡顿）：「列表停留期预取不挤占语音主链路（对话不卡顿）」的可听证据归
 *         live-manual（SC-A9）。
 *   GAP-5（LFN 代价）：「CONFIG_FATFS_LFN_HEAP 生效 + heap 水位」属 exitCriteria，不属
 *         SC-A1..A8 自动断言面（真机读数）。
 *
 * 行为等价假设（显式；不写即默认不成立）:
 *   EQ-1「weak 哨兵被强符号覆盖 = 接线落地」：本文件在 standalone 红跑里 weak 先于强符号
 *        生效；host runner 中 display/story_cache.c 与 story_page.c 必须在 HOST_ACCEPTANCE_SRCS
 *        在世（TEST 10 tooth 4 钉登记），否则强符号缺席时本文件会以哨兵红掩盖「未接线」。
 *   EQ-2「页态数值 5 = KSTORY_PAGE_BODY_UNAVAILABLE」：本文件以数值镜像驱动
 *        kidpal_story_page_input；story_page.h 漂移时 TEST 9 先红，等值由头注 [C0] 钉死。
 *   EQ-3「SendKidpalStoryFetch 符号链 = 设备真的会发该帧」：结构齿只是源锚代理，真
 *        wire 形状由对端（front）pytest 的 request→response 行为齿承载（SC-A7）。
 *
 * 覆盖标记（coverage marker）随 TEST_CASE 名称与首行注释逐条给出。
 *
 * Red-baseline channel（本文件落盘时 builder 尚未接线进 host 构建）:
 *   firmware/main/test/acceptance/red-baseline/sc_red_wrapper.sh
 *     = unity stub + runner.c 独立冒烟通道（同一份冻结文件真执行，非冻结脚手架）。
 *   全量套件零新增红另跑 host_infra_test.elf（既有基线见报告）。
 *
 * REVISION LOG
 *   2026-10-17 初版冻结（acceptance-author；SC-A1..A8 的 host 纯逻辑半 + 跨端结构半）。
 */

#include <unity.h>

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

/* 键位枚举（KEY_ID_* / KEY_GESTURE_*）——既有冻结头，host 与设备两侧同源。 */
#include "../key_gesture_parser.h"

/* -------------------------------------------------------------------------- *
 * 镜像类型 / 冻结常量（[C0]-[C9]；builder 落 story_cache.h / story_page.h 后
 * 布局必须逐字同序同型——本文件不 include 实现头，镜像即冻结形状）。
 * ------------------------------------------------------------------------ */

/* GAP EQ-2 / [C0] 页态 additive 值。 */
#define KSC_PAGE_LIST             0
#define KSC_PAGE_READER           1
#define KSC_PAGE_RECORDING        2
#define KSC_PAGE_STAMP            3
#define KSC_PAGE_DELETE           4
#define KSC_PAGE_BODY_UNAVAILABLE 5

/* [C1]/[C2] 索引容量与 id 容量（= story_page.h KSTORY_LIST_MAX 同值）。 */
#define KSTORY_CACHE_MAX    16
#define KSTORY_CACHE_ID_MAX 64

/* [C5]/[C6] 冻结数值（决策点 = plan 关键接线决策，architect 预评采纳）。 */
#define KSTORY_OPEN_HIT_CARD     0
#define KSTORY_OPEN_FETCH        1
#define KSTORY_OPEN_DEGRADE      2
#define KSTORY_FETCH_TIMEOUT_MS  10000UL
#define KSTORY_FETCH_INTERVAL_MS 5000UL

/* 未实现哨兵：永不是本文件任何 seam 的合法返回值（story_page_v2 KSPV2_UNIMPLEMENTED
 * 同款纪律）。 */
#define KSC_UNIMPLEMENTED (-99)

/* story_page.h 现有类型的镜像（公共前缀逐字同序同型；story_page_v2 先例）。 */
typedef struct {
    char id[64];
    char title[96];
    char date[16];
} ksc_story_item_t;

typedef struct {
    int total;
    int count;
    ksc_story_item_t items[KSTORY_CACHE_MAX];
} ksc_story_list_t;

typedef struct {
    char id[64];
    char title[96];
    char text[2048];
    int seq;
} ksc_story_text_t;

typedef struct {
    int page;
    int cursor;
    int count;
    int counter;
    int screen;
    int screens;
    int rec_seconds;
    int stamp_serial;
    int archive_pend;
    int cancel_count;
    int tts_requests;
    char last_saved_id[64];
    char pending_delete_id[64];
} ksc_story_view_t;

/* [C1]/[C2] 新模块结构（builder 的 kidpal_story_cache_t 必须与本镜像同布局）。 */
typedef struct {
    int count;
    char ids[KSTORY_CACHE_MAX][KSTORY_CACHE_ID_MAX];
} ksc_story_cache_t;

/* -------------------------------------------------------------------------- *
 * Weak 哨兵（home = display/story_cache.{c,h} 与 display/story_page.c；host runner
 * 中强定义覆盖。未落地/未接线态 = 设计态红，不是编译错）。
 * ------------------------------------------------------------------------ */

__attribute__((weak)) int kidpal_story_cache_init(ksc_story_cache_t *c)
{ (void)c; return KSC_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_cache_clear(ksc_story_cache_t *c)
{ (void)c; return KSC_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_cache_add(ksc_story_cache_t *c, const char *id)
{ (void)c; (void)id; return KSC_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_cache_has(const ksc_story_cache_t *c, const char *id)
{ (void)c; (void)id; return KSC_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_cache_remove(ksc_story_cache_t *c, const char *id)
{ (void)c; (void)id; return KSC_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_cache_missing_on_screen(
        const ksc_story_cache_t *cache, const ksc_story_item_t *items, int count,
        int screen, int rows_per_screen, char out_ids[][KSTORY_CACHE_ID_MAX])
{
    (void)cache; (void)items; (void)count; (void)screen; (void)rows_per_screen;
    (void)out_ids;
    return KSC_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_story_cache_commit(ksc_story_cache_t *c, const char *id,
                                                    int card_ready, int write_ok)
{ (void)c; (void)id; (void)card_ready; (void)write_ok; return KSC_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_reader_id_match(const ksc_story_text_t *st,
                                                       const ksc_story_list_t *list,
                                                       int cursor)
{ (void)st; (void)list; (void)cursor; return KSC_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_open_decide(int id_match, int has_local,
                                                   int card_ready, int fetch_failed,
                                                   int fetch_in_flight,
                                                   unsigned long wait_ms)
{
    (void)id_match; (void)has_local; (void)card_ready; (void)fetch_failed;
    (void)fetch_in_flight; (void)wait_ms;
    return KSC_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_story_prefetch_due(int page_state, int voice_front,
                                                    int ws_busy, int in_flight,
                                                    int pending_count,
                                                    unsigned long since_last_ms)
{
    (void)page_state; (void)voice_front; (void)ws_busy; (void)in_flight;
    (void)pending_count; (void)since_last_ms;
    return KSC_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_story_prefetch_suspend(int page_state, int voice_front)
{ (void)page_state; (void)voice_front; return KSC_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_fetch_answer_enter(int awaiting_this_id)
{ (void)awaiting_this_id; return KSC_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_stamp_should_fire(int archive_pend,
                                                         const char *arrived_id,
                                                         const char *last_saved_id,
                                                         int from_fetch)
{
    (void)archive_pend; (void)arrived_id; (void)last_saved_id; (void)from_fetch;
    return KSC_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_story_id_safe(const char *id)
{ (void)id; return KSC_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_body_unavailable_idle_step(unsigned long *idle_ms,
                                                                  unsigned long dt_ms)
{ (void)idle_ms; (void)dt_ms; return KSC_UNIMPLEMENTED; }
/* 既有冻结函数：home = display/story_page.c（host runner 已在册）。 */
__attribute__((weak)) int kidpal_story_page_input(ksc_story_view_t *v, key_id_t key,
                                                  key_gesture_t g)
{ (void)v; (void)key; (void)g; return KSC_UNIMPLEMENTED; }

/* -------------------------------------------------------------------------- *
 * 工具（哨兵必查 / 消息格式化 / 仓根定位 / 文件扫描）
 * ------------------------------------------------------------------------ */

static void ksc_require_impl(int value, const char *what)
{
    if (value == KSC_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "SC RED: frozen seam NOT implemented — %s（builder 落地 "
                 "display/story_cache.c 并接线进 HOST_ACCEPTANCE_SRCS 后转绿；红因 = "
                 "冻结契约未实现，见 story_cache.acceptance.c 头注 seam contract）",
                 what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

static char ksc_fm_buf[1024];
static const char *ksc_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *ksc_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(ksc_fm_buf, sizeof(ksc_fm_buf), fmt, ap);
    va_end(ap);
    return ksc_fm_buf;
}

#define KSC_MAX_PATH 1024
#define KSC_MAX_READ (1024 * 1024)

static int ksc_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int ksc_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static char ksc_root[KSC_MAX_PATH];
static int ksc_root_ready;

/* evidence 类扫描齿的 root 注入（acceptance-evidence 纪律）：优先显式环境变量
 * KIDPAL_REPO_ROOT，其次由脚本所处的相对候选点探测——不依赖某个调用 cwd 的绝对路径
 * 假设（仓根 / host_app / red-baseline 三处均可跑）。 */
static int ksc_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KSC_MAX_PATH];
    const char *env_root = getenv("KIDPAL_REPO_ROOT");

    if (ksc_root_ready) {
        return 1;
    }
    if (env_root != NULL && env_root[0] != '\0') {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", env_root);
        if (ksc_is_regular(probe)) {
            snprintf(ksc_root, sizeof(ksc_root), "%s", env_root);
            ksc_root_ready = 1;
            return 1;
        }
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (ksc_is_regular(probe)) {
            snprintf(ksc_root, sizeof(ksc_root), "%s", cands[i]);
            ksc_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int ksc_rel(char *out, size_t cap, const char *rel)
{
    assert(out != NULL && rel != NULL);
    if (!ksc_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", ksc_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", ksc_root, rel);
    return 1;
}

static char *ksc_read(const char *rel, size_t *out_len)
{
    char p[KSC_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!ksc_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KSC_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* logic guard: host OOM must fail loudly, not read as "red" */
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        free(buf);
        return NULL;
    }
    buf[sz] = '\0';
    fclose(f);
    if (out_len != NULL) {
        *out_len = (size_t)sz;
    }
    return buf;
}

static int ksc_file_contains(const char *rel, const char *needle)
{
    char *buf;
    int hit;

    assert(needle != NULL);
    buf = ksc_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    hit = strstr(buf, needle) != NULL;
    free(buf);
    return hit;
}

static int ksc_is_skipped_dir(const char *name)
{
    /* test/ 自身不参与树扫（本文件字面会污染计数）；build/.git 为产物面。 */
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int ksc_count_in_file(const char *path, const char *needle)
{
    FILE *f = fopen(path, "rb");
    char *buf;
    long sz;
    int n = 0;
    char *p;

    if (f == NULL) {
        return 0;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > (unsigned long)KSC_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return 0;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL);
    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
        fclose(f);
        free(buf);
        return 0;
    }
    buf[sz] = '\0';
    fclose(f);
    for (p = strstr(buf, needle); p != NULL; p = strstr(p + 1, needle)) {
        n++;
    }
    free(buf);
    return n;
}

/* 递归统计 dir 下（跳过 skip 目录）含 needle 的文件数；first_hit 返回首个相对路径。 */
static int ksc_scan_count(const char *rel_dir, const char *needle,
                          char *first_hit, size_t cap)
{
    char dirp[KSC_MAX_PATH];
    DIR *d;
    struct dirent *de;
    int files = 0;

    assert(needle != NULL);
    if (!ksc_rel(dirp, sizeof(dirp), rel_dir)) {
        return 0;
    }
    if (!ksc_is_dir(dirp)) {
        return 0;
    }
    d = opendir(dirp);
    if (d == NULL) {
        return 0;
    }
    while ((de = readdir(d)) != NULL) {
        char child[KSC_MAX_PATH + 300]; /* 容量 >= dirp(1023) + '/' + d_name(255) + NUL */
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
            continue;
        }
        if (strlen(dirp) + 1 + strlen(de->d_name) + 1 > sizeof(child)) {
            continue; /* 路径超长 -> 跳过（防御：不截断成别的路径去读） */
        }
        snprintf(child, sizeof(child), "%s/%s", dirp, de->d_name);
        if (ksc_is_dir(child)) {
            if (ksc_is_skipped_dir(de->d_name)) {
                continue;
            }
            {
                char child_rel[KSC_MAX_PATH];
                snprintf(child_rel, sizeof(child_rel), "%s/%s", rel_dir, de->d_name);
                files += ksc_scan_count(child_rel, needle, first_hit, cap);
            }
            continue;
        }
        if (ksc_count_in_file(child, needle) > 0) {
            if (files == 0 && first_hit != NULL && cap > 0) {
                snprintf(first_hit, cap, "%s/%s", rel_dir, de->d_name);
            }
            files++;
        }
    }
    closedir(d);
    return files;
}

static void ksc_put_item(ksc_story_item_t *it, const char *id, const char *title)
{
    assert(it != NULL && id != NULL);
    memset(it, 0, sizeof(*it));
    snprintf(it->id, sizeof(it->id), "%s", id);
    snprintf(it->title, sizeof(it->title), "%s", title != NULL ? title : id);
    snprintf(it->date, sizeof(it->date), "08-20");
}

/* ========================================================================== *
 * TEST 1 — RAM 索引（SC-A2 索引半）
 * coverage: SC-A2 -- story_text 到达且 SD 在位 -> 更新 RAM 索引；索引 add 幂等去重 /
 *           满 16 拒绝 / has 命中与未命中 / remove / clear
 * ========================================================================== */
TEST_CASE("story cache: RAM index init/add dedupe/full/remove/clear (SC-A2)",
          "[story_cache][SC-A2][linux]")
{
    ksc_story_cache_t c;
    char id[32];
    int r;

    r = kidpal_story_cache_init(&c);
    ksc_require_impl(r, "[C1] kidpal_story_cache_init");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, r, "SC-A2 RED: cache_init 合法入参未返回 0。");
    TEST_ASSERT_EQUAL_INT(0, c.count);

    r = kidpal_story_cache_add(&c, "2026-08-20-xiaotuzi");
    ksc_require_impl(r, "[C1] kidpal_story_cache_add");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, r, "SC-A2 RED: 新 id 未登记（应返回 1）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, c.count, "SC-A2 RED: 登记后 count != 1。");

    r = kidpal_story_cache_add(&c, "2026-08-20-xiaotuzi");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, r, "SC-A2 RED: 同 id 重复 add 未幂等（应 0）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, c.count,
                                  "SC-A2 RED: 同 id 重复 add 产生了重复项（count 涨）。");

    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_story_cache_has(&c, "2026-08-20-xiaotuzi"),
                                  "SC-A2 RED: has 命中返回非 1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_cache_has(&c, "2026-08-21-ghost"),
                                  "SC-A2 RED: has 未命中返回非 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_cache_has(NULL, "x"),
                                  "SC-A2 RED: has(NULL) 未返回 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_cache_has(&c, NULL),
                                  "SC-A2 RED: has(c, NULL) 未返回 0。");

    /* 满 16：再加 15 条后 count == 16，第 17 条拒绝（count 不变）。 */
    for (int i = 0; i < 15; i++) {
        snprintf(id, sizeof(id), "id-%02d", i);
        r = kidpal_story_cache_add(&c, id);
        TEST_ASSERT_EQUAL_INT_MESSAGE(1, r, ksc_fm("SC-A2 RED: 第 %d 条 id 未登记。", i + 1));
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(KSTORY_CACHE_MAX, c.count,
                                  "SC-A2 RED: 满容量时 count != 16。");
    r = kidpal_story_cache_add(&c, "overflow-17");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, r,
                                  "SC-A2 RED: 满 16 后 add 未拒绝（应 -1）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(KSTORY_CACHE_MAX, c.count,
                                  "SC-A2 RED: 满容量拒绝后 count 被改动。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_cache_has(&c, "overflow-17"),
                                  "SC-A2 RED: 被拒绝的第 17 条仍进了索引。");

    r = kidpal_story_cache_remove(&c, "2026-08-20-xiaotuzi");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, r, "SC-A2 RED: remove 命中未返回 1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(KSTORY_CACHE_MAX - 1, c.count,
                                  "SC-A2 RED: remove 后 count 未 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_cache_remove(&c, "2026-08-20-xiaotuzi"),
                                  "SC-A2 RED: remove 未命中未幂等返回 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_cache_remove(NULL, "x"),
                                  "SC-A2 RED: remove(NULL) 未返回 -1。");

    r = kidpal_story_cache_clear(&c);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, r, "SC-A2 RED: clear 未返回 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, c.count, "SC-A2 RED: clear 后 count != 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_cache_has(&c, "id-00"),
                                  "SC-A2 RED: clear 后仍 has 命中旧条目。");

    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_cache_init(NULL),
                                  "SC-A2 RED: init(NULL) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_cache_add(&c, NULL),
                                  "SC-A2 RED: add(c, NULL) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_cache_add(&c, ""),
                                  "SC-A2 RED: add(c, 空串) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, c.count,
                                  "SC-A2 RED: 非法 add 触碰了索引。");
}

/* ========================================================================== *
 * TEST 2 — 当屏缺失判定（SC-A1 预取输入 / AC2）
 * coverage: SC-A2/SC-A1 -- 当屏缺失判定（list 3 条 + 空 cache → 3；含 1 条 → 2；
 *           越界/空 list → 0）
 * ========================================================================== */
TEST_CASE("story cache: missing-on-screen selection drives prefetch batch (SC-A1)",
          "[story_cache][SC-A1][linux]")
{
    ksc_story_cache_t c;
    ksc_story_item_t items[6];
    char out[KSTORY_CACHE_MAX][KSTORY_CACHE_ID_MAX];
    int n;

    (void)kidpal_story_cache_init(&c);
    ksc_put_item(&items[0], "id-a", "故事A");
    ksc_put_item(&items[1], "id-b", "故事B");
    ksc_put_item(&items[2], "id-c", "故事C");

    memset(out, 0, sizeof(out));
    n = kidpal_story_cache_missing_on_screen(&c, items, 3, 0, 4, out);
    ksc_require_impl(n, "[C2] kidpal_story_cache_missing_on_screen");
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, n,
                                  "SC-A1 RED: 空 cache + 3 条当屏未返回 3 个缺失。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("id-a", out[0],
                                     "SC-A1 RED: 缺失项未按 items 窗口顺序输出（[0]）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("id-b", out[1],
                                     "SC-A1 RED: 缺失项未按 items 窗口顺序输出（[1]）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("id-c", out[2],
                                     "SC-A1 RED: 缺失项未按 items 窗口顺序输出（[2]）。");

    (void)kidpal_story_cache_add(&c, "id-b");
    memset(out, 0, sizeof(out));
    n = kidpal_story_cache_missing_on_screen(&c, items, 3, 0, 4, out);
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, n,
                                  "SC-A1 RED: 含 1 条本地已留存时缺失数应 2。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("id-a", out[0], "SC-A1 RED: 已留存 id 未跳过（[0]）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("id-c", out[1], "SC-A1 RED: 已留存 id 未跳过（[1]）。");

    (void)kidpal_story_cache_add(&c, "id-a");
    (void)kidpal_story_cache_add(&c, "id-c");
    n = kidpal_story_cache_missing_on_screen(&c, items, 3, 0, 4, out);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, n, "SC-A1 RED: 全部留存时缺失数应 0。");

    /* 越界屏 / 空 list → 0；NULL / 参数非法 → -1。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
                                  kidpal_story_cache_missing_on_screen(&c, items, 3, 1, 4, out),
                                  "SC-A1 RED: 越界屏未返回 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
                                  kidpal_story_cache_missing_on_screen(&c, items, 0, 0, 4, out),
                                  "SC-A1 RED: 空 list 未返回 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
                                  kidpal_story_cache_missing_on_screen(NULL, items, 3, 0, 4, out),
                                  "SC-A1 RED: NULL cache 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
                                  kidpal_story_cache_missing_on_screen(&c, NULL, 3, 0, 4, out),
                                  "SC-A1 RED: NULL items 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
                                  kidpal_story_cache_missing_on_screen(&c, items, 3, 0, 4, NULL),
                                  "SC-A1 RED: NULL out_ids 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
                                  kidpal_story_cache_missing_on_screen(&c, items, 3, 0, 0, out),
                                  "SC-A1 RED: rows_per_screen=0 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
                                  kidpal_story_cache_missing_on_screen(&c, items, 17, 0, 4, out),
                                  "SC-A1 RED: count > KSTORY_CACHE_MAX 未返回 -1（出参溢出面）。");

    /* 第二屏窗口 [4,6)：6 条 list、4 行/屏 -> 第 2 屏 = items[4], items[5]。 */
    ksc_put_item(&items[3], "id-d", "故事D");
    ksc_put_item(&items[4], "id-e", "故事E");
    ksc_put_item(&items[5], "id-f", "故事F");
    (void)kidpal_story_cache_add(&c, "id-e");
    memset(out, 0, sizeof(out));
    n = kidpal_story_cache_missing_on_screen(&c, items, 6, 1, 4, out);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, n,
                                  "SC-A1 RED: 第 2 屏窗口计算错（应只缺 id-f 1 条）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("id-f", out[0],
                                     "SC-A1 RED: 第 2 屏缺失项取值错（应为 id-f）。");
}

/* ========================================================================== *
 * TEST 3 — 读回 id 校验（SC-A6 / SC-A3 list-membership half）
 * coverage: SC-A6 -- id 校验（相等 1 / 不等 0 / 空 id 0 / cursor 越界 0 / NULL 0）
 * ========================================================================== */
TEST_CASE("story cache: reader id match pins opened text to cursor row (SC-A6)",
          "[story_cache][SC-A6][SC-A3][linux]")
{
    ksc_story_list_t list;
    ksc_story_text_t st;
    int r;

    memset(&list, 0, sizeof(list));
    list.total = 3;
    list.count = 3;
    ksc_put_item(&list.items[0], "id-a", "故事A");
    ksc_put_item(&list.items[1], "id-b", "故事B");
    ksc_put_item(&list.items[2], "id-c", "故事C");

    memset(&st, 0, sizeof(st));
    snprintf(st.id, sizeof(st.id), "id-b");
    snprintf(st.title, sizeof(st.title), "故事B");
    snprintf(st.text, sizeof(st.text), "正文B");
    st.seq = 2;

    r = kidpal_story_reader_id_match(&st, &list, 1);
    ksc_require_impl(r, "[C4] kidpal_story_reader_id_match");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, r,
                                  "SC-A6 RED: 正文 id == 光标所选行 id 时未返回 1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_reader_id_match(&st, &list, 0),
                                  "SC-A6 RED: 正文 id != 光标行 id 时未返回 0（错配未拦）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_reader_id_match(&st, &list, -1),
                                  "SC-A6 RED: cursor=-1 未返回 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_reader_id_match(&st, &list, 3),
                                  "SC-A6 RED: cursor==count 越界未返回 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_reader_id_match(NULL, &list, 1),
                                  "SC-A6 RED: NULL st 未返回 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_reader_id_match(&st, NULL, 1),
                                  "SC-A6 RED: NULL list 未返回 0。");

    st.id[0] = '\0';
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_reader_id_match(&st, &list, 1),
                                  "SC-A6 RED: 空 st->id 未返回 0。");

    /* 负锚：title 相同但 id 不同 —— 绝不按 title 推断（AC7 正文须与所选篇目一致）。 */
    memset(&st, 0, sizeof(st));
    snprintf(st.id, sizeof(st.id), "id-zzz");
    snprintf(st.title, sizeof(st.title), "故事B");
    snprintf(st.text, sizeof(st.text), "别篇正文");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_reader_id_match(&st, &list, 1),
                                  "SC-A6 RED: 仅 title 相同即判匹配（应按 id 严格比对）。");
}

/* ========================================================================== *
 * TEST 4 — 双击三分支决策（SC-A3 / SC-A4 / SC-A5 / SC-A6）
 * coverage: SC-A3 -- 命中本地留存 + 仍在云端列表 -> 读卡零网络
 *           SC-A4 -- 未命中 -> 插队 fetch
 *           SC-A5 -- 取不回 -> 可见降级
 *           SC-A6 -- id 不等 -> 拒绝并降级，绝不显示别篇
 * ========================================================================== */
TEST_CASE("story cache: open-reader three-branch decision (SC-A3/A4/A5/A6)",
          "[story_cache][SC-A3][SC-A4][SC-A5][SC-A6][linux]")
{
    int r;

    r = kidpal_story_open_decide(1, 1, 1, 0, 0, 0);
    ksc_require_impl(r, "[C5] kidpal_story_open_decide");
    TEST_ASSERT_EQUAL_INT_MESSAGE(KSTORY_OPEN_HIT_CARD, r,
                                  "SC-A3 RED: 命中本地留存且卡在位未走读卡分支（零网络）。");

    TEST_ASSERT_EQUAL_INT_MESSAGE(KSTORY_OPEN_FETCH,
                                  kidpal_story_open_decide(1, 0, 1, 0, 0, 0),
                                  "SC-A4 RED: 未命中未走插队 fetch 分支。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(KSTORY_OPEN_FETCH,
                                  kidpal_story_open_decide(1, 1, 0, 0, 0, 0),
                                  "AC7 RED: 卡不在位但 id 匹配未走 fetch（应取回后当场显示）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(KSTORY_OPEN_FETCH,
                                  kidpal_story_open_decide(1, 0, 1, 0, 1, 9000),
                                  "SC-A4 RED: fetch 在途且未超时未保持等待（应 FETCH 语义）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(KSTORY_OPEN_DEGRADE,
                                  kidpal_story_open_decide(1, 0, 1, 0, 1, KSTORY_FETCH_TIMEOUT_MS),
                                  "SC-A5 RED: fetch 等待到 10s 边界未走降级。");

    TEST_ASSERT_EQUAL_INT_MESSAGE(KSTORY_OPEN_DEGRADE,
                                  kidpal_story_open_decide(1, 0, 1, 1, 0, 0),
                                  "SC-A5 RED: 拉取失败未走降级（离线双击必须给可见反馈）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(KSTORY_OPEN_DEGRADE,
                                  kidpal_story_open_decide(1, 1, 0, 1, 0, 0),
                                  "AC7 RED: 卡不在位且拉取失败未走降级。");

    /* 已留存篇目断网照常打开：本地命中优先于失败态（判定序 (b) 先于 (c)）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(KSTORY_OPEN_HIT_CARD,
                                  kidpal_story_open_decide(1, 1, 1, 1, 0, 99999),
                                  "AC7 RED: 已留存篇目因拉取失败被拖去降级（离线可看边界破）。");

    /* id 错配：绝不显示别篇，直接降级（即使本地有「其它篇」的正文）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(KSTORY_OPEN_DEGRADE,
                                  kidpal_story_open_decide(0, 1, 1, 0, 0, 0),
                                  "SC-A6 RED: id 不等时未拒绝（会显示别篇内容）—— 严禁。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(KSTORY_OPEN_DEGRADE,
                                  kidpal_story_open_decide(0, 0, 1, 1, 0, 0),
                                  "SC-A6 RED: id 不等 + 失败态未走降级。");
}

/* ========================================================================== *
 * TEST 5 — 预取决策（SC-A1）
 * coverage: SC-A1 -- 仅列表态 + 无语音前台 + WS 空闲 + 逐篇串行 + 篇间间隔发帧；
 *           录音/会话/阅读/盖章/删除/降级态不発；离开 P3 即停
 * ========================================================================== */
TEST_CASE("story cache: prefetch decision never squeezes voice front (SC-A1)",
          "[story_cache][SC-A1][linux]")
{
    int r;

    r = kidpal_story_prefetch_due(KSC_PAGE_LIST, 0, 0, 0, 3, KSTORY_FETCH_INTERVAL_MS);
    ksc_require_impl(r, "[C6] kidpal_story_prefetch_due");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, r,
                                  "SC-A1 RED: 列表态 + 无语音 + WS 空闲 + 无在途 + 间隔到"
                                  "未发帧（应 1）。");

    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
                                  kidpal_story_prefetch_due(KSC_PAGE_LIST, 0, 0, 0, 3,
                                                            KSTORY_FETCH_INTERVAL_MS - 1),
                                  "SC-A1 RED: 篇间间隔未到仍发帧（应 0）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1,
                                  kidpal_story_prefetch_due(KSC_PAGE_LIST, 0, 0, 0, 3,
                                                            KSTORY_FETCH_INTERVAL_MS),
                                  "SC-A1 RED: 间隔恰达阈值未放行（>= 语义）。");

    /* 非列表态一律不发（录音 / 会话 / 阅读 / 盖章 / 删除 / 取不回提示）。 */
    {
        const int states[] = { KSC_PAGE_READER, KSC_PAGE_RECORDING, KSC_PAGE_STAMP,
                               KSC_PAGE_DELETE, KSC_PAGE_BODY_UNAVAILABLE };
        for (size_t i = 0; i < sizeof(states) / sizeof(states[0]); i++) {
            TEST_ASSERT_EQUAL_INT_MESSAGE(
                0, kidpal_story_prefetch_due(states[i], 0, 0, 0, 3, 99999),
                ksc_fm("SC-A1 RED: 页态 %d 下仍发预取帧（应 0）。", states[i]));
        }
    }

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_prefetch_due(KSC_PAGE_LIST, 1, 0, 0, 3, 99999),
                                  "SC-A1 RED: 语音前台置位仍发预取帧（挤占主链路）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_prefetch_due(KSC_PAGE_LIST, 0, 1, 0, 3, 99999),
                                  "SC-A1 RED: WS 忙时未查空闲（应 0）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_prefetch_due(KSC_PAGE_LIST, 0, 0, 1, 3, 99999),
                                  "SC-A1 RED: 在途未收仍并发发下一篇（破逐篇串行）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_prefetch_due(KSC_PAGE_LIST, 0, 0, 0, 0, 99999),
                                  "SC-A1 RED: 无待取项仍发帧（应 0）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_prefetch_due(-1, 0, 0, 0, 3, 99999),
                                  "SC-A1 RED: 越界页态未按 0 处理（防御面）。");

    r = kidpal_story_prefetch_suspend(KSC_PAGE_READER, 0);
    ksc_require_impl(r, "[C6] kidpal_story_prefetch_suspend");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, r,
                                  "SC-A1 RED: 离开列表态未挂起（应清在途队列）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_story_prefetch_suspend(KSC_PAGE_LIST, 1),
                                  "SC-A1 RED: 语音前台置位未挂起预取。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_story_prefetch_suspend(KSC_PAGE_RECORDING, 0),
                                  "SC-A1 RED: 录音态未挂起预取。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_prefetch_suspend(KSC_PAGE_LIST, 0),
                                  "SC-A1 RED: 列表态 + 无语音前台下误挂起（功能被误停）。");
}

/* ========================================================================== *
 * TEST 6 — 写卡策略：先写卡成功才登记 / 失败只清该篇 / 卡不在位整层 no-op（SC-A2）
 * coverage: SC-A2 -- story_text 到达且 SD 在位 → 写 /sdcard/story/<id>.txt + 更新索引；
 *           卡不在位 / 坏卡 → 整层失效（不落盘、不崩溃）；写失败只清该篇
 * coverage: SC-A8 -- 卡不在位时当场显示、不留存（决策半）
 * ========================================================================== */
TEST_CASE("story cache: index registration gated on successful card write (SC-A2/SC-A8)",
          "[story_cache][SC-A2][SC-A8][linux]")
{
    ksc_story_cache_t c;
    int r;

    (void)kidpal_story_cache_init(&c);

    r = kidpal_story_cache_commit(&c, "id-a", 1, 1);
    ksc_require_impl(r, "[C3] kidpal_story_cache_commit");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, r,
                                  "SC-A2 RED: 卡在位 + 写成功的新 id 未登记（应 1）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_story_cache_has(&c, "id-a"),
                                  "SC-A2 RED: 写成功后索引无该 id。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_cache_commit(&c, "id-a", 1, 1),
                                  "SC-A2 RED: 同 id 重复 commit 未幂等（应 0）。");

    /* 写失败（新 id）：不登记，count 不变。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_cache_commit(&c, "id-b", 1, 0),
                                  "SC-A2 RED: 写卡失败仍登记了索引（应先写成功再登记）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_cache_has(&c, "id-b"),
                                  "SC-A2 RED: 写卡失败的 id 仍进了索引。");

    /* 写失败（既有 id）：只清该篇，其它条目保留（不整层清空）。 */
    (void)kidpal_story_cache_commit(&c, "id-b", 1, 1);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_story_cache_has(&c, "id-b"), "precondition");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_cache_commit(&c, "id-b", 1, 0),
                                  "SC-A2 RED: 既有 id 写失败未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_cache_has(&c, "id-b"),
                                  "SC-A2 RED: 写失败未清掉该篇索引（会读到不存在的文件）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_story_cache_has(&c, "id-a"),
                                  "SC-A2 RED: 单篇写失败把整层索引清空了（不整层清空纪律破）。");

    /* 卡不在位 / 坏卡：整层失效 -> 不登记、索引零触碰、不崩。 */
    {
        const int before = c.count;
        TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_cache_commit(&c, "id-c", 0, 1),
                                      "SC-A2 RED: 卡不在位时 commit 未返回 -1。");
        TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_cache_commit(&c, "id-a", 0, 0),
                                      "SC-A2 RED: 卡不在位 + 写失败未返回 -1。");
        TEST_ASSERT_EQUAL_INT_MESSAGE(before, c.count,
                                      "SC-A2 RED: 卡不在位时索引被改动（应整层 no-op）。");
        TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_story_cache_has(&c, "id-a"),
                                      "SC-A2 RED: 卡不在位时既有索引条目被清（不落盘不等于清索引）。");
    }

    /* 非法 id / NULL：索引零触碰（与 [C9] 路径安全联立）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_cache_commit(&c, "../evil", 1, 1),
                                  "SC-A2 RED: 路径穿越 id 被登记（路径注入面）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_cache_has(&c, "../evil"),
                                  "SC-A2 RED: 路径穿越 id 进了索引。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_cache_commit(&c, NULL, 1, 1),
                                  "SC-A2 RED: commit(NULL id) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_cache_commit(&c, "", 1, 1),
                                  "SC-A2 RED: commit(空 id) 未返回 -1。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_cache_commit(NULL, "id-x", 1, 1),
                                  "SC-A2 RED: commit(NULL cache) 未返回 -1。");
}

/* ========================================================================== *
 * TEST 7 — story id 字符集校验（securityAxes 路径注入 / exitCriteria SD 路径安全）
 * coverage: SC-A2 -- story id 进文件名前字符集校验（_slugify/_hash_slug 产 ASCII），
 *           无路径拼接注入面
 * ========================================================================== */
TEST_CASE("story cache: story id character set blocks path injection (SC-A2)",
          "[story_cache][SC-A2][security][linux]")
{
    int r;

    r = kidpal_story_id_safe("2026-08-20-xiaotuzi");
    ksc_require_impl(r, "[C9] kidpal_story_id_safe");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, r, "SC-A2 RED: 合法 slug id 被判非法。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_story_id_safe("s1a2b3c4"),
                                  "SC-A2 RED: hash slug（s+8hex）被判非法。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_story_id_safe("a.b_c-D9"),
                                  "SC-A2 RED: [A-Za-z0-9._-] 混合合法 id 被判非法。");

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_id_safe(NULL),
                                  "SC-A2 RED: NULL id 未判非法。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_id_safe(""),
                                  "SC-A2 RED: 空 id 未判非法。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_id_safe("../outside"),
                                  "SC-A2 RED: 路径穿越 id 未判非法。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_id_safe("a/b"),
                                  "SC-A2 RED: 正斜杠 id 未判非法。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_id_safe("a\\b"),
                                  "SC-A2 RED: 反斜杠 id 未判非法。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_id_safe("a..b"),
                                  "SC-A2 RED: 含 \"..\" 的 id 未判非法。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_id_safe("故事"),
                                  "SC-A2 RED: 非 ASCII id 未判非法（文件名只收 ASCII slug）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_id_safe("a b"),
                                  "SC-A2 RED: 空格 id 未判非法。");

    /* 超长（>= KSTORY_CACHE_ID_MAX）：拒绝，防截断后撞名。 */
    {
        char long_id[KSTORY_CACHE_ID_MAX + 8];
        memset(long_id, 'a', sizeof(long_id));
        long_id[sizeof(long_id) - 1] = '\0';
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_id_safe(long_id),
                                      "SC-A2 RED: 超长 id 未判非法（截断撞名面）。");
        long_id[KSTORY_CACHE_ID_MAX - 1] = '\0';
        TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_story_id_safe(long_id),
                                      "SC-A2 RED: 恰好 63 字符的合法 id 被误拒。");
    }
}

/* ========================================================================== *
 * TEST 8 — fetch 应答消费 + 盖章侧效应守卫（SC-A2 / SC-A8 / 契约 §1.3④）
 * coverage: SC-A5 -- 取不回三态（离线 / 超时 / 卡不在位且拉取失败）由 TEST 4 决策承载
 * coverage: SC-A8 -- fetch 应答不触发星星盖章；卡不在位时当场显示、不留存
 * coverage: SC-A4 -- 应答到达后（等待中）进阅读态
 * ========================================================================== */
TEST_CASE("story cache: fetch answer enters reader only when awaited, never stamps (SC-A4/A8)",
          "[story_cache][SC-A4][SC-A8][linux]")
{
    int r;

    r = kidpal_story_fetch_answer_enter(1);
    ksc_require_impl(r, "[C7] kidpal_story_fetch_answer_enter");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, r,
                                  "SC-A4 RED: 双击在等该篇的 fetch 应答未进阅读态。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_fetch_answer_enter(0),
                                  "SC-A4 RED: 后台预取应答抢了前台（应只装缓存）。");

    r = kidpal_story_stamp_should_fire(1, "id-a", "", 1);
    ksc_require_impl(r, "[C8] kidpal_story_stamp_should_fire");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, r,
                                  "SC-A8 RED: fetch 应答触发了盖章演出（契约 §1.3④：盖章只随"
                                  " fresh_save 归档路径）。");

    /* fresh_save 归档路径的正向半（既有行为不得被本包改坏）：archive_pend > 0 + 新 id。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_story_stamp_should_fire(2, "id-b", "id-a", 0),
                                  "SC-A8 RED: fresh_save 归档路径不再触发盖章（既有行为回归）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_stamp_should_fire(2, "id-a", "id-a", 0),
                                  "SC-A8 RED: 同 id 幂等失效（重复演出）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_stamp_should_fire(0, "id-b", "id-a", 0),
                                  "SC-A8 RED: archive_pend=0 仍盖章（回看既有故事误盖）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_stamp_should_fire(2, "", "id-a", 0),
                                  "SC-A8 RED: 空 arrived_id 仍盖章。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_stamp_should_fire(2, NULL, "id-a", 0),
                                  "SC-A8 RED: NULL arrived_id 未零污染。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_story_stamp_should_fire(2, "id-b", NULL, 0),
                                  "SC-A8 RED: last_saved_id=NULL 未视同空串（首篇不盖章）。");
}

/* ========================================================================== *
 * TEST 9 — WF-F2-06 降级覆盖层骨架：文案单源 / 页态值 / 10s idle / 任一手势关闭（SC-A5）
 * coverage: SC-A5 -- 可见降级提示（WF-F2-06 整屏覆盖层，文案单源 = story_page.h），
 *           不静默、不黑屏；10 秒无操作自动消失；KEY／BOOT 任一手势立即关闭回列表
 * ========================================================================== */
TEST_CASE("story body-unavailable overlay: single-source copy, 10s idle, any gesture closes (SC-A5)",
          "[story_cache][SC-A5][UI][linux]")
{
    unsigned long idle;
    int r;

    /* tooth 1 — 文案单源 + 页态值 additive（源锚代理：扫真实 story_page.h 字面）。
     * 宏名与逐字文案分开断言（容忍空白写法差异，不放宽「单源 + 逐字」要求）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/display/story_page.h",
                          "KSTORY_BODY_UNAVAILABLE_HINT"),
        "SC-A5 RED: story_page.h 无 KSTORY_BODY_UNAVAILABLE_HINT 单源宏（WF-F2-06 大字"
        "话术必须单源，不得散落渲染面）。");
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/display/story_page.h", "这篇暂时打不开"),
        "SC-A5 RED: story_page.h 无大字话术逐字「这篇暂时打不开」（设计稿 v6 定稿）。");
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/display/story_page.h",
                          "KSTORY_BODY_UNAVAILABLE_SUB"),
        "SC-A5 RED: story_page.h 无 KSTORY_BODY_UNAVAILABLE_SUB 单源宏（WF-F2-06 副句）。");
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/display/story_page.h", "等会儿再试试吧"),
        "SC-A5 RED: story_page.h 无副句逐字「等会儿再试试吧」（设计稿 v6 定稿）。");
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/display/story_page.h",
                          "KSTORY_PAGE_BODY_UNAVAILABLE = 5"),
        "SC-A5 RED: story_page.h 页态枚举无 KSTORY_PAGE_BODY_UNAVAILABLE = 5（additive，"
        "WF-F2-05 DELETE=4 之后追加，既有值零改）。");
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/display/story_page.h",
                          "#define KSTORY_BODY_UNAVAILABLE_IDLE_MS 10000UL"),
        "SC-A5 RED: story_page.h 无 KSTORY_BODY_UNAVAILABLE_IDLE_MS 10000UL 单源。");

    /* 负向（产品判据，设计稿 v6 / brief v5 明载）：**不复用** §2.2 通用网络故障固定句
     * 「网络出小差了，等一会儿再试试吧」——本场景失败模式是「这篇还没取回来」、
     * 不是网络故障。 */
    TEST_ASSERT_FALSE_MESSAGE(
        ksc_file_contains("firmware/main/display/story_page.h", "网络出小差了"),
        "SC-A5 RED: WF-F2-06 复用了 §2.2 通用网络故障话术——本场景失败模式不同"
        "（这篇还没取回来），话术不得混用。");

    /* tooth 2 — 10s idle-step（形参/语义仿 delete idle step）。 */
    idle = 0;
    r = kidpal_story_body_unavailable_idle_step(&idle, 6000);
    ksc_require_impl(r, "[C0] kidpal_story_body_unavailable_idle_step");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, r, "SC-A5 RED: 6s 未到点时即关闭（应 0）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(6000UL, idle, "SC-A5 RED: idle 累计未更新（应 6000）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_story_body_unavailable_idle_step(&idle, 4000),
                                  "SC-A5 RED: 累计 10s 未自动消失（应 1）。");
    idle = 0;
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_body_unavailable_idle_step(&idle, 9999),
                                  "SC-A5 RED: 9999ms 未到点即关闭（边界应 <10s 保持）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_body_unavailable_idle_step(NULL, 1),
                                  "SC-A5 RED: NULL idle_ms 未返回 -1。");

    /* tooth 3 — KEY／BOOT 任一手势（短按/双击/长按）一律关闭回列表。 */
    {
        const key_id_t keys[2] = { KEY_ID_KEY, KEY_ID_BOOT };
        const key_gesture_t gestures[3] = { KEY_GESTURE_CLICK, KEY_GESTURE_DOUBLE_CLICK,
                                            KEY_GESTURE_LONG_PRESS };
        for (size_t k = 0; k < 2; k++) {
            for (size_t g = 0; g < 3; g++) {
                ksc_story_view_t v;
                int act;
                memset(&v, 0, sizeof(v));
                v.page = KSC_PAGE_BODY_UNAVAILABLE;
                v.count = 3;
                v.cursor = 1;
                act = kidpal_story_page_input(&v, keys[k], gestures[g]);
                ksc_require_impl(act, "[C0] kidpal_story_page_input 取不回提示态行");
                TEST_ASSERT_EQUAL_INT_MESSAGE(
                    KSC_PAGE_LIST, v.page,
                    ksc_fm("SC-A5 RED: 取不回提示态下 key=%d gesture=%d 未关闭回列表"
                           "（任一手势一律关闭，纯告知无破坏性动作）。",
                           (int)keys[k], (int)gestures[g]));
            }
        }
    }
}

/* ========================================================================== *
 * TEST 10 — 跨端 / 接线结构齿（SC-A1 发帧原语 + SC-A7 对端消费分支 + 模块接线）
 * coverage: SC-A1 -- 设备侧发 kidpal_story_fetch 原语落单缝（protocol/application/
 *           mcp_server 消费链）
 * coverage: SC-A7 -- 对端 front 存在 kidpal_story_fetch 消费分支（行为由 server
 *           pytest 真消费承载，本齿只钉接线存在性）
 * coverage: SC-A2 -- SD 挂载标志前置（board.h virtual + waveshare override）与
 *           story_cache 模块登记
 * ========================================================================== */
TEST_CASE("story cache: fetch primitive + front branch + module wiring anchors (SC-A1/A2/A7)",
          "[story_cache][SC-A1][SC-A2][SC-A7][structural][linux]")
{
    char first[KSC_MAX_PATH] = { 0, 0 };
    int files;

    /* tooth 1 — SendKidpalStoryFetch 符号链（protocol.h/.cc + application + mcp_server）。 */
    files = ksc_scan_count("firmware/main", "SendKidpalStoryFetch", first, sizeof(first));
    TEST_ASSERT_TRUE_MESSAGE(
        files >= 3,
        ksc_fm("SC-A1 RED: SendKidpalStoryFetch 仅 %d 文件命中（首命中 %s，期望 >=3: "
               "protocol.h 声明 + protocol.cc 定义 + application/mcp_server 消费链）。",
               files, first[0] != '\0' ? first : "<none>"));
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/protocols/protocol.h", "SendKidpalStoryFetch"),
        "SC-A1 RED: protocol.h 无 SendKidpalStoryFetch 声明（发送原语未落单缝）。");
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/protocols/protocol.cc", "SendKidpalStoryFetch"),
        "SC-A1 RED: protocol.cc 无 SendKidpalStoryFetch 定义。");
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/protocols/protocol.cc", "\"kidpal_story_fetch\""),
        "SC-A1 RED: protocol.cc 未出现线形状字面 \"kidpal_story_fetch\"。");
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/protocols/protocol.cc", "\"ids\""),
        "SC-A1 RED: protocol.cc 未出现 ids 数组键字面（契约 §四 载荷 = ids 批量）。");

    /* tooth 2 — 双击三分支 + id 校验 + 缓存模块在消费侧真的被调用（源锚代理）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/mcp_server.cc", "kidpal_story_open_decide"),
        "SC-A4 RED: mcp_server.cc OPEN_READER 消费侧未调 kidpal_story_open_decide"
        "（三分支决策未接线）。");
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/mcp_server.cc", "kidpal_story_reader_id_match"),
        "SC-A6 RED: mcp_server.cc 未调 kidpal_story_reader_id_match（id 校验未消费）。");
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/mcp_server.cc", "kidpal_story_cache"),
        "SC-A2 RED: mcp_server.cc 未消费 story_cache 索引（story_text 写卡/索引未接线）。");

    /* tooth 3 — 对端 front 的 kidpal_story_fetch 消费分支（跨端对端面）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("server/gateway/front/front_app.py", "kidpal_story_fetch"),
        "SC-A7 RED: server/gateway/front/front_app.py 无 kidpal_story_fetch 消费分支"
        "（契约 §四 网关消费并拦截）。");

    /* tooth 4 — 模块与冻结件接线（builder 义务；EQ-1 防「未接线也绿」）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/CMakeLists.txt", "display/story_cache.c"),
        "SC-A2 RED: firmware/main/CMakeLists.txt 未接入 display/story_cache.c。");
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/test/host_app/main/CMakeLists.txt",
                          "story_cache.acceptance.c"),
        "EQ-1 RED: host_app HOST_ACCEPTANCE_SRCS 未登记本冻结验收件（builder 义务）。");
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/test/host_app/main/CMakeLists.txt",
                          "display/story_cache.c"),
        "EQ-1 RED: host_app HOST_ACCEPTANCE_SRCS 未登记 display/story_cache.c 强符号源"
        "（weak 哨兵不会被覆盖，行为齿会以哨兵红掩盖未接线）。");

    /* tooth 5 — SD 挂载标志前置（architect 预评 ③ 前置阻断项）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/boards/common/board.h", "SdCardMounted"),
        "SC-A2 RED: boards/common/board.h 无 virtual SdCardMounted()（消费侧拿不到挂载标志）。");
    TEST_ASSERT_TRUE_MESSAGE(
        ksc_file_contains("firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc",
                          "SdCardMounted"),
        "SC-A2 RED: waveshare 板级无 SdCardMounted 实现（前置阻断项未落）。");
}
