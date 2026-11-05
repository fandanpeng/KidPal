/*
 * Frozen acceptance tests — TaskPackage 261104-k0-points-e2e-spike
 * (subsystem: firmware/main——display/view_router.h/.c points view 路由 additive +
 *  display/points_page.h/.c points_home / points_ledger 载荷最小消费 + mcp_server.cc
 *  points 视图消费接线 + protocols/protocol.cc kidpal_points_page 上行驱动方;
 *  rigor=full; riskTier=high)。
 *
 * Story 语义权威 = docs/requirements.md v1.71 S-F6.2-AC1（详细账本全量可及——设备
 * 消费半）+ S-F6.3/S-F6.4（记账通路的设备消费半：只消费执行后的权威积分视图）+
 * FROZEN PROPOSAL 261103-star-points-story-cover（D-07 restore 不抢前台 / D-08-spike
 * 最小生产者-消费者 / D-10 薄意图）+ 契约 docs/contracts/interface-contract.md
 * §1.3⑦⑧（points_home / points_ledger 载荷形状，2026-11-04 LANDED）+ §1.4
 * （add_points / redeem_points 网关决定类——**不直接下发设备**）+ §四
 * kidpal_points_page（keyset 分页请求—应答 wire 规则）。
 *
 * Nature: 行为级（view 路由真函数 + 载荷解析/分流/分页缓存纯逻辑真函数 + weak 哨兵：
 * 实现未编入前哨兵恒返回 K0PTS_UNIMPLEMENTED = 「冻结契约未实现」设计态红，不是编译
 * 错）+ 结构级（真实工作树源锚扫描，record_dialogue_guide / sd_log 先例形态；正向断言
 * 只认**注释剥离后可执行源**的字面形态）。
 *
 * 本文件**不 include** display/points_page.h（镜像公共形状，K3R/RDG 先例；落地后
 * host runner 中 display/points_page.c 强符号覆盖本文件 weak 哨兵）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；「home =」为冻结落点）:
 *
 *   [G1] VRT_ROUTE additive 数值码（home = display/view_router.h + view_router.c;
 *        冻结纪律只增不改/移——0..6 七枚既有数值码零偏移、UNKNOWN=-1 不变）:
 *
 *          VRT_ROUTE_POINTS_HOME  = 7   〔枚举尾部 additive（STORY_TEXT=6 之后），
 *                                        冻结数值 = 7；契约 §1.3⑦〕
 *          VRT_ROUTE_POINTS_LEDGER = 8  〔枚举尾部 additive，冻结数值 = 8；契约
 *                                        §1.3⑧〕
 *
 *          int kidpal_display_route(const char *view);
 *            -- "points_home" -> 7；"points_ledger" -> 8；既有 text/home/control/
 *               char/schedule/story_list/story_text -> 0..6 零偏移；NULL / 白名单外
 *               -> VRT_ROUTE_UNKNOWN(-1) 降级（既有语义零偏移）。
 *
 *          【负锚】int kidpal_display_control_action(const char *action);
 *            -- "add_points" / "redeem_points" -> VRT_CTRL_UNKNOWN(-1)：网关决定类
 *               控制动作**不进设备控制路由**（契约 §1.4「不直接下发设备」「front 自推
 *               通路也不把这两个 action 发给设备」；设备侧不可路由 = 误下发即降级，
 *               绝不执行任何记账语义）。既有 end_session->0 / stop_speaking->1 /
 *               start_recording->2 / start_organizing->3 零偏移（must-stay-green，
 *               RDG-F1 冻结域不重开——只钉零偏移）。
 *
 *   [G2] points_home 载荷最小消费 + restore 分流（home = display/points_page.h +
 *        points_page.c 新建；镜像公共形状冻结如下，builder 头文件须逐字同序同型）:
 *
 *          #define KPTS_ENTRY_ID_CAP      64
 *          #define KPTS_ENTRY_REASON_CAP  96
 *          #define KPTS_ENTRY_AT_CAP      40
 *          #define KPTS_TARGET_NAME_CAP   64
 *          #define KPTS_TARGET_ASSET_CAP  16
 *          #define KPTS_RECENT_MAX        3   〔契约 §1.3⑦：recent 时间倒序最多
 *                                              三条〕
 *
 *          typedef struct {                〔统一分录形状，六键（契约 §1.3⑦⑧共用）〕
 *              char id[KPTS_ENTRY_ID_CAP];
 *              int delta;                  〔正数=加分、负数=兑换〕
 *              char reason[KPTS_ENTRY_REASON_CAP];
 *              char occurred_at[KPTS_ENTRY_AT_CAP];
 *              int balance_before;
 *              int balance_after;
 *          } kidpal_points_entry_t;
 *
 *          typedef struct {
 *              int revision;               〔单调递增整数〕
 *              int balance;                〔可正可负；权威值，设备只显示不计算〕
 *              char target_name[KPTS_TARGET_NAME_CAP];
 *              int target_points;          〔正整数；激活目标分值〕
 *              char target_asset[KPTS_TARGET_ASSET_CAP];  〔sc-choco/sc-movie/
 *                                              sc-wheel/sc-gift/sc-flag〕
 *              kidpal_points_entry_t recent[KPTS_RECENT_MAX];
 *              int recent_count;           〔0..3〕
 *              int restore;                〔1 = data.restore true；0 = 缺省/false〕
 *          } kidpal_points_home_t;
 *
 *          int kidpal_points_home_parse(const char *data_json,
 *                                       kidpal_points_home_t *out);
 *            -- 0 = 成功；-1 = 显式降级（NULL 载荷 / NULL out / 非 JSON 对象 / 缺
 *               revision、balance、target / target 缺 name 或 points / recent 超过
 *               KPTS_RECENT_MAX 条 = 载荷违反契约、绝不盲渲染）。restore 键 true=1、
 *               false/缺省=0；未知扩展字段忽略不消费（additive 纪律，story_list_parse
 *               同款）。
 *
 *          int kidpal_points_home_takeover(const kidpal_points_home_t *frame);
 *            -- restore 缺省/false（frame->restore == 0）-> 1 = 主动展示 P3 积分
 *               主页（接管前台——设备主动语音记账成功例外跳积分页，D-07 / 契约 §1.3⑦）；
 *               restore=true（frame->restore != 0）-> 0 = 只填 RAM 缓存不抢前台
 *               （QQ 写入 / WS 建连 / 删除 / 目标切换推送；P3 主页在屏时原地重绘）；
 *               NULL -> -1。
 *          〔「数据未达不显示数字」态（S-F6.1 AC4）= 未收到任何权威帧时的设备 RAM
 *            初态，由消费接线层承载、不经本纯函数——G4 结构锚钉消费接线在世。〕
 *
 *   [G3] points_ledger 载荷解析 + 分页缓存纪律（home = display/points_page.h +
 *        points_page.c；契约 §1.3⑧ + §四 kidpal_points_page）:
 *
 *          #define KPTS_LEDGER_ITEMS_MAX  6   〔设备缺省请求 limit=6；载荷 items
 *                                              超过本容量 = 降级〕
 *          #define KPTS_CURSOR_CAP        128
 *
 *          typedef struct {
 *              int revision;
 *              int balance;
 *              kidpal_points_entry_t items[KPTS_LEDGER_ITEMS_MAX];
 *              int item_count;             〔0..KPTS_LEDGER_ITEMS_MAX〕
 *              int page_total;             〔page.total = 全量分录数〕
 *              char next_cursor[KPTS_CURSOR_CAP];  〔空串 = 无续页〕
 *              int has_more;
 *              int reset;                  〔page.reset；1 = 设备须先丢弃旧分页缓存〕
 *              int restore;
 *          } kidpal_points_ledger_t;
 *
 *          typedef struct {                〔设备侧分页缓存视图（RAM，最小面）〕
 *              int revision;               〔缓存所属账本版本〕
 *              int cached_count;           〔已缓存分录条数〕
 *              int has_more;
 *          } kidpal_points_ledger_view_t;
 *
 *          int kidpal_points_ledger_parse(const char *data_json,
 *                                         kidpal_points_ledger_t *out);
 *            -- 0 / -1 语义同 [G2]；page 四键（total/next_cursor/has_more/reset）
 *               必带、缺任一 = -1；items 超 KPTS_LEDGER_ITEMS_MAX = -1。
 *
 *          int kidpal_points_ledger_apply(kidpal_points_ledger_view_t *v,
 *                                         const kidpal_points_ledger_t *f);
 *            -- f->reset != 0：**丢弃旧分页缓存再装入**（cached_count = f->item_
 *               count、v->revision = f->revision、v->has_more = f->has_more）-> 0；
 *               f->reset == 0（正常续页）：f->revision != v->revision -> **-1 拒绝
 *               跨版本追加**且缓存零扰动（契约 §四「不得跨版本追加」）；同版本 ->
 *               cached_count += f->item_count、v->has_more = f->has_more -> 0；
 *               NULL v / NULL f -> -1。
 *
 *   [G4] 消费闭环 + 上行驱动方 + host runner 登记守（结构锚；home = mcp_server.cc +
 *        protocols/protocol.cc（或 application.cc）+ host_app/main/CMakeLists.txt）:
 *            -- mcp_server.cc：points 视图消费接线在世（VRT_ROUTE_POINTS_HOME 或
 *               kidpal_points_home_parse 引用 >= 1——kidpal/display 消费链把 points
 *               视图接进 points page 消费；设备只消费执行后的权威积分视图，缺消费 =
 *               权威帧下发后设备黑屏降级）；
 *            -- 上行驱动方在世（K0S-E2 设备半）：firmware/main/protocols/protocol.cc
 *               / protocol.h / application.cc 之一引用 "kidpal_points_page" >= 1
 *               （设备分页请求帧构造；无驱动方 = 设备永远发不出分页请求，front 应答
 *               无人消费 = 首尾 canary 消费端半死）；
 *            -- 登记守（anti-vacuity；非 acceptance 条目）：firmware/main/test/
 *               host_app/main/CMakeLists.txt 同含 "points_spike.acceptance.c"（本
 *               文件进 host runner）与 "points_page.c"（行为齿强符号源）。该清单是
 *               K0a/K0 唯一共享构建文件、由 K0 在 K0a merged 后追加——builder 在
 *               重取基线后的 FIRST ACTION（taskpackage changeManifest 预指派）。
 *
 *   [G5] live-manual 真机轮观测项（**不落自动断言、不计入 runner 通过面**，
 *        taskpackage acceptance[] K0PTS-L1..L4 单列）：① 真实设备完整语音记账句、
 *        设备实际显示权威余额和原因（K0S-E1）；② 真实 QQ 双状态 canary（第一笔其他
 *        页不抢前台、第二笔积分页在屏原地刷新；一笔时另一状态如实降级申报，K0S-E3 /
 *        D-07）；③ NFR-PERF-4 实测时延记录（不写死阈值）；④ 首尾两条 canary 经设备
 *        kidpal_points_page 消费看屏/日志目击（K0S-E2 真值半）。文件写入、CLI 返回
 *        或 pytest 读回只能作辅证——offline 断言不替代消费端目击。
 * ===========================================================================
 * 契约 / 决策条目 ↔ 验收断言 的强度申报（acceptance-evidence「契约条目 ↔ 验收
 * 断言」节）:
 *
 * | 契约 / 决策条目 | 承载断言 | 性质档 |
 * |---|---|---|
 * | 契约 §1.3⑦⑧ view 注册（白名单 9） | TEST 1（路由决策表真跑 + view_router.h 枚举锚） | 行为级 + 源锚代理（数值单源） |
 * | 契约 §1.4 add_points/redeem_points 不直接下发设备 | TEST 1 负锚（控制动作恒 UNKNOWN） | 行为级（防误下发执行面） |
 * | D-07 restore 不抢前台（设备半） | TEST 2（takeover 分流真跑） | 行为级（端到端归 live L2②） |
 * | 契约 §1.3⑦ points_home 消费（六键分录/target 四键/recent ≤3） | TEST 2（契约示例载荷逐字段真跑 + 畸形降级） | 行为级 |
 * | 契约 §1.3⑧ + §四 分页缓存纪律（reset 丢弃 / 不跨版本追加） | TEST 3（apply 决策表真跑） | 行为级 |
 * | S-F6.2-AC1 全量可及（设备消费半） | TEST 3（parse 分页字段）+ TEST 4（驱动方锚）+ live L4 | 行为级 + 源锚代理 |
 * | K0S-E2 消费闭环（驱动方 ↔ 消费方） | TEST 4（mcp 消费锚 + protocol 驱动方锚） | 源锚代理（运行时归真机轮） |
 * | 既有 view/control 数值码 0..6 + 动作 0..3 | TEST 1 回归守卫半 | 行为级（must-stay-green，冻结域不重开） |
 *
 * 弱于契约处（显式缺口，不得当已兑现）:
 *   GAP-1 渲染视觉细节（金币符号 / 进度线 / 刻度 / 键帽 / 空态话术排版）= 设计稿面
 *         （docs/ui/V9/points-home.png 冻结页级 ref；design review 承载）——本文件
 *         只钉数据消费与分流语义，不转脆性视觉断言（spike 最小渲染边界，完整 UI 归
 *         K2）。
 *   GAP-2 「数据未达不显示数字 / 空账显示 0 与空账话术」两态（S-F6.1 AC4）的运行时
 *         渲染 = mcp 接线层 + 真机轮；TEST 4 消费锚只钉接线在世。
 *   GAP-3 kidpal_points_page 上行帧的真实 wire 时序（进详细账本 / 翻屏时发送、每请求
 *         至多一回）= 消费接线层行为 + 真机轮；结构锚 ≠ 运行时行为。
 *   GAP-4 restore 推送「P3 主页在屏时原地重绘」的运行时效果 = 真机轮 L2②（D-07）。
 *
 * 行为等价假设（显式；不写即默认不成立）:
 *   EQ-1 「weak 哨兵被强符号覆盖 = 纯逻辑接线落地」：standalone 红跑不链接
 *        points_page.c / view_router.c，哨兵恒红 = 设计态红；host runner 中
 *        points_page.c（新建）+ view_router.c（已在册）+ 本文件必须在
 *        HOST_ACCEPTANCE_SRCS 在世（TEST 4 登记守），否则哨兵红会掩盖「未接线」。
 *   EQ-2 「镜像 == 真头」：本文件 kidpal_points_home_t / kidpal_points_entry_t /
 *        kidpal_points_ledger_t / kidpal_points_ledger_view_t 镜像 = points_page.h
 *        首冻形状（本文件即形状权威；builder 头文件漂移由 TEST 2/3 锚先红兜底）。
 *   EQ-3 「源锚 = 运行时行为」：TEST 4 只证明源码具备消费/驱动形状，不证明设备上
 *        真的完成请求—应答与看屏——真机轮 L2/L4 承载（spike-first 铁律：消费端目击
 *        是通过判据第一位）。
 *   EQ-4 「契约示例载荷 = front 真实推送形状」：TEST 2/3 样例逐字取自 interface-
 *        contract.md §1.3⑦⑧ 示例（冻结 wire 形状）；front 侧形状漂移由 server 面
 *        K0PTS-A3/A5 floor 先红。
 *
 * 覆盖标记（coverage marker）随 TEST_CASE 名称与首行注释逐条给出。
 *
 * Run protocol（host runner, C4.4 惯例；**LP8 后置**——K0a merged 后执行）:
 *   0) builder 接线（K0a merged、重取基线后 FIRST ACTION）: 向
 *      firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS 追加
 *      "../../points_spike.acceptance.c" 并将 "../../../display/points_page.c" 登记
 *      为强符号源（view_router.c 已在册；TEST 4 登记守红 = 未接线）。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *      ./build/host_infra_test.elf
 *   本文件亦可独立冒烟（冻结相位实编 + 设计态红取证，rdg/sc_red_wrapper 同款通道）:
 *      bash firmware/main/test/acceptance/red-baseline/k0pts_red_wrapper.sh
 *      （WSL gcc -std=c11 -Wall -Wextra 编 points_spike.acceptance.c + runner.c 后
 *      执行；exit code = 失败用例数）
 *   结构扫描类断言支持 KIDPAL_REPO_ROOT 显式注入仓根（缺省走 walk-up 探测，仓根 /
 *   host_app / red-baseline 三处均可跑；acceptance-evidence「root 注入」纪律）。
 *
 * RED baseline（**deferred: LP8 后置**——host/device build 与真机终验按 K0S-E4 在
 *   K0a(261104-k0a-four-page-skeleton) merged、重取 firmware 基线后实跑；本文件
 *   冻结落盘只做独立冒烟通道实编 + 设计态红取证，不虚报 host/device 红结果）:
 *   设计态（points_page.h/.c 不在世、view_router 未扩、mcp/protocol 无消费/驱动、
 *   CMakeLists 未登记）——预期 4 个 TEST_CASE 全红，红因逐条对应当前 story 缺失的
 *   真实行为：
 *     TEST 1 -> G1 additive 数值码未实现（哨兵 -99 / view_router.h 枚举锚缺席）
 *     TEST 2 -> G2 points_page 解析/takeover 缺席（哨兵）
 *     TEST 3 -> G3 points_ledger parse/apply 缺席（哨兵）
 *     TEST 4 -> G4 mcp 消费锚 / 上行驱动方锚 / 登记守缺席（builder 接线后转绿）
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: K0PTS-F1 -- view 路由 additive 数值码（0..6 零偏移 + 7/8 新增 +
 *             头文件数值单源）+ 网关决定类控制动作设备侧不可路由负锚（TEST 1）
 *   coverage: K0PTS-F2 -- points_home 载荷最小消费：契约示例逐字段解析 + 畸形/NULL
 *             降级 + restore 分流（takeover：true=只填缓存 / 缺省false=接管前台）
 *             （TEST 2）
 *   coverage: K0PTS-F3 -- points_ledger 载荷解析（page 四键 + items 容量）+ 分页
 *             缓存纪律（reset 丢弃旧缓存 / 同版本续接累加 / 跨版本追加拒绝）（TEST 3）
 *   coverage: K0PTS-F4 -- 消费闭环 + 上行驱动方结构锚 + host runner 登记守（TEST 4；
 *             anti-vacuity 半为 builder FIRST ACTION 锚，非 acceptance 条目）
 *   coverage: K0PTS-L1 -- live-manual ①真实设备完整语音记账句、设备实际显示权威
 *             余额和原因目击（K0S-E1），不落自动断言、不计入 runner 通过面；
 *   coverage: K0PTS-L2 -- live-manual ②真实 QQ 双状态 canary（第一笔其他页不抢
 *             前台、第二笔积分页在屏原地刷新；K0S-E3 / D-07），同上不落自动断言；
 *   coverage: K0PTS-L3 -- live-manual ③NFR-PERF-4 实测时延记录（不写死阈值），
 *             同上不落自动断言；
 *   coverage: K0PTS-L4 -- live-manual ④首尾两条 canary 经设备 kidpal_points_page
 *             消费看屏/日志目击（K0S-E2 真值半），同上不落自动断言。
 *             （2026-11-04 注释级修订：原区间缩写「K0PTS-L1..L4」按 ARQ-097/260731
 *             收紧规则只命中 L1，planner 裁定逐 id 显式标记，与 server 件同形；
 *             四项载体 = spike-evidence.md，真值判定归消费端目击——文件写入、CLI
 *             返回或 pytest 读回只能作辅证。）
 * --------------------------------------------------------------------------- */

#include <unity.h>

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* -------------------------------------------------------------------------- *
 * 冻结数值镜像（view_router.h 冻结枚举；本文件不 include 真头，RDG 先例——数值
 * 漂移时 TEST 1 锚先红）。
 * ------------------------------------------------------------------------ */

#define K0_ROUTE_TEXT       0 /* VRT_ROUTE_TEXT */
#define K0_ROUTE_HOME       1 /* VRT_ROUTE_HOME */
#define K0_ROUTE_CONTROL    2 /* VRT_ROUTE_CONTROL */
#define K0_ROUTE_CHAR       3 /* VRT_ROUTE_CHAR */
#define K0_ROUTE_SCHEDULE   4 /* VRT_ROUTE_SCHEDULE */
#define K0_ROUTE_STORY_LIST 5 /* VRT_ROUTE_STORY_LIST */
#define K0_ROUTE_STORY_TEXT 6 /* VRT_ROUTE_STORY_TEXT */
#define K0_ROUTE_POINTS_HOME   7 /* VRT_ROUTE_POINTS_HOME（本包 additive，冻结 = 7） */
#define K0_ROUTE_POINTS_LEDGER 8 /* VRT_ROUTE_POINTS_LEDGER（本包 additive，冻结 = 8） */
#define K0_ROUTE_UNKNOWN    (-1)

#define K0_CTRL_END_SESSION     0 /* VRT_CTRL_END_SESSION */
#define K0_CTRL_STOP_SPEAKING   1 /* VRT_CTRL_STOP_SPEAKING */
#define K0_CTRL_START_RECORDING 2 /* VRT_CTRL_START_RECORDING */
#define K0_CTRL_START_ORGANIZING 3 /* VRT_CTRL_START_ORGANIZING（RDG-F1 既有） */
#define K0_CTRL_UNKNOWN         (-1)

/* points_page.h 镜像容量（[G2]/[G3] 冻结形状）。 */
#define K0_ENTRY_ID_CAP      64
#define K0_ENTRY_REASON_CAP  96
#define K0_ENTRY_AT_CAP      40
#define K0_TARGET_NAME_CAP   64
#define K0_TARGET_ASSET_CAP  16
#define K0_RECENT_MAX        3
#define K0_LEDGER_ITEMS_MAX  6
#define K0_CURSOR_CAP        128

/* 未实现哨兵：永不是任何整型 seam 的合法返回值。 */
#define K0PTS_UNIMPLEMENTED (-99)

/* kidpal_points_entry_t / kidpal_points_home_t / kidpal_points_ledger_t /
 * kidpal_points_ledger_view_t 镜像（points_page.h 首冻形状，[G2]/[G3]；EQ-2）。 */
typedef struct {
    char id[K0_ENTRY_ID_CAP];
    int delta;
    char reason[K0_ENTRY_REASON_CAP];
    char occurred_at[K0_ENTRY_AT_CAP];
    int balance_before;
    int balance_after;
} kidpal_points_entry_t;

typedef struct {
    int revision;
    int balance;
    char target_name[K0_TARGET_NAME_CAP];
    int target_points;
    char target_asset[K0_TARGET_ASSET_CAP];
    kidpal_points_entry_t recent[K0_RECENT_MAX];
    int recent_count;
    int restore;
} kidpal_points_home_t;

typedef struct {
    int revision;
    int balance;
    kidpal_points_entry_t items[K0_LEDGER_ITEMS_MAX];
    int item_count;
    int page_total;
    char next_cursor[K0_CURSOR_CAP];
    int has_more;
    int reset;
    int restore;
} kidpal_points_ledger_t;

typedef struct {
    int revision;
    int cached_count;
    int has_more;
} kidpal_points_ledger_view_t;

/* -------------------------------------------------------------------------- *
 * 纯逻辑 seam 声明（与真头逐字同型；home = display/view_router.c +
 * display/points_page.c——host runner 强符号覆盖 weak 哨兵）。
 * ------------------------------------------------------------------------ */

int kidpal_display_route(const char *view);
int kidpal_display_control_action(const char *action);
int kidpal_points_home_parse(const char *data_json, kidpal_points_home_t *out);
int kidpal_points_home_takeover(const kidpal_points_home_t *frame);
int kidpal_points_ledger_parse(const char *data_json, kidpal_points_ledger_t *out);
int kidpal_points_ledger_apply(kidpal_points_ledger_view_t *v,
                               const kidpal_points_ledger_t *f);

/* -------------------------------------------------------------------------- *
 * Weak 哨兵（host runner 中 display/view_router.c / display/points_page.c 强定义
 * 覆盖）。
 * ------------------------------------------------------------------------ */

__attribute__((weak)) int kidpal_display_route(const char *view)
{ (void)view; return K0PTS_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_display_control_action(const char *action)
{ (void)action; return K0PTS_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_home_parse(const char *data_json,
                                                   kidpal_points_home_t *out)
{ (void)data_json; (void)out; return K0PTS_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_home_takeover(const kidpal_points_home_t *frame)
{ (void)frame; return K0PTS_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_ledger_parse(const char *data_json,
                                                     kidpal_points_ledger_t *out)
{ (void)data_json; (void)out; return K0PTS_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_points_ledger_apply(kidpal_points_ledger_view_t *v,
                                                     const kidpal_points_ledger_t *f)
{ (void)v; (void)f; return K0PTS_UNIMPLEMENTED; }

/* -------------------------------------------------------------------------- *
 * 工具：失败消息 / 仓根定位 / 文件读取 / 注释剥离 / 窗内搜索（自包含复制自
 * record_dialogue_guide.acceptance.c 的 rdl_* 助手族，命名前缀 k0_；全 static）。
 * ------------------------------------------------------------------------ */

#define K0_MAX_PATH 1024
#define K0_MAX_READ (8u * 1024u * 1024u)

static char k0_fm_buf[1024];
static const char *k0_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *k0_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(k0_fm_buf, sizeof(k0_fm_buf), fmt, ap);
    va_end(ap);
    return k0_fm_buf;
}

static int k0_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static char k0_root[K0_MAX_PATH];
static int k0_root_ready;

/* 仓根定位（acceptance-evidence「root 注入」纪律）：优先显式环境变量
 * KIDPAL_REPO_ROOT，其次由相对候选点探测——不依赖某个调用 cwd 的绝对路径假设。 */
static int k0_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[K0_MAX_PATH];
    const char *env_root;
    size_t i;

    if (k0_root_ready) {
        return 1;
    }
    env_root = getenv("KIDPAL_REPO_ROOT");
    if (env_root != NULL && env_root[0] != '\0') {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", env_root);
        if (k0_is_regular(probe)) {
            snprintf(k0_root, sizeof(k0_root), "%s", env_root);
            k0_root_ready = 1;
            return 1;
        }
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (k0_is_regular(probe)) {
            snprintf(k0_root, sizeof(k0_root), "%s", cands[i]);
            k0_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static char *k0_read(const char *rel, size_t *out_len)
{
    char p[K0_MAX_PATH * 2 + 2]; /* root(<=1024) + '/' + rel — no truncation */
    FILE *f;
    long sz;
    char *buf;

    if (!k0_locate_root()) {
        return NULL;
    }
    snprintf(p, sizeof(p), "%s/%s", k0_root, rel);
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > K0_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

/* 注释剥离（sd_log / RDG 同款）：正向断言只被**可执行源**满足——注释字节抹成空格；
 * 字符串 / 字符字面量体保留。保持缓冲长度，调用方 free。 */
static char *k0_strip_c_comments(const char *src)
{
    size_t n, i = 0;
    char *out;

    assert(src != NULL); /* logic guard: NULL source is a test bug, not a red */
    n = strlen(src);
    out = (char *)malloc(n + 1);
    assert(out != NULL); /* logic guard: host OOM must fail loudly */
    memcpy(out, src, n + 1);

    while (i < n) {
        char c = out[i];
        if (c == '"' || c == '\'') {
            char q = c;
            i++;
            while (i < n) {
                if (out[i] == '\\' && i + 1 < n) { i += 2; continue; }
                if (out[i] == q) { i++; break; }
                i++;
            }
            continue;
        }
        if (c == '/' && i + 1 < n && out[i + 1] == '/') {
            while (i < n && out[i] != '\n') { out[i] = ' '; i++; }
            continue;
        }
        if (c == '/' && i + 1 < n && out[i + 1] == '*') {
            out[i] = ' '; out[i + 1] = ' '; i += 2;
            while (i < n) {
                if (out[i] == '*' && i + 1 < n && out[i + 1] == '/') {
                    out[i] = ' '; out[i + 1] = ' '; i += 2; break;
                }
                if (out[i] != '\n') { out[i] = ' '; }
                i++;
            }
            continue;
        }
        i++;
    }
    return out;
}

static char *k0_strip_file(const char *rel)
{
    char *buf = k0_read(rel, NULL);
    char *out;

    if (buf == NULL) {
        return NULL;
    }
    out = k0_strip_c_comments(buf);
    free(buf);
    return out;
}

/* 裸源子串出现计数（负向断言 / 计数锚专用——已剥注释源上使用只计可执行源）。 */
static int k0_count_sub(const char *hay, const char *needle)
{
    int n = 0;
    const char *p = hay;
    size_t nl;

    if (hay == NULL || needle == NULL) {
        return 0;
    }
    nl = strlen(needle);
    if (nl == 0) {
        return 0;
    }
    while ((p = strstr(p, needle)) != NULL) {
        n++;
        p++;
    }
    return n;
}

/* 行级判定：slice 内同时含 needle1 与 needle2 的行数（注释剥离后的源上使用）。 */
static int k0_count_lines_with_both(const char *slice, const char *needle1,
                                    const char *needle2)
{
    int count = 0;
    const char *line;

    if (slice == NULL || needle1 == NULL || needle2 == NULL) {
        return 0;
    }
    line = slice;
    while (*line != '\0') {
        const char *eol = strchr(line, '\n');
        size_t len = eol ? (size_t)(eol - line) : strlen(line);
        char tmp[K0_MAX_PATH];
        size_t cp = len < sizeof(tmp) - 1 ? len : sizeof(tmp) - 1;

        memcpy(tmp, line, cp);
        tmp[cp] = '\0';
        if (strstr(tmp, needle1) != NULL && strstr(tmp, needle2) != NULL) {
            count++;
        }
        if (eol == NULL) {
            break;
        }
        line = eol + 1;
    }
    return count;
}

#define K0_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(k0_locate_root(), \
    "K0PTS RED(infra): repo root not located from cwd — run from the repo root, " \
    "firmware/main/test/host_app or firmware/main/test/acceptance/red-baseline, " \
    "or pass KIDPAL_REPO_ROOT (see acceptance-evidence root injection rule)")

/* 哨兵守卫：-99 = seam 未实现（设计态红）；host runner 登记后转绿。 */
#define K0_REQUIRE_IMPL_INT(v, what) \
    TEST_ASSERT_TRUE_MESSAGE((v) != K0PTS_UNIMPLEMENTED, \
        k0_fm("%s RED: 冻结 seam 未实现（weak 哨兵返回 -99）——%s；builder 落地 " \
               "display/points_page.c 实现面并登记进 HOST_ACCEPTANCE_SRCS 后转绿", \
               "K0PTS", what))

/* -------------------------------------------------------------------------- *
 * 实现面文件锚（结构扫描对象；相对仓根）。
 * ------------------------------------------------------------------------ */

#define K0_VIEW_ROUTER_H "firmware/main/display/view_router.h"
#define K0_POINTS_H      "firmware/main/display/points_page.h"
#define K0_MCP_CC        "firmware/main/mcp_server.cc"
#define K0_PROTOCOL_CC   "firmware/main/protocols/protocol.cc"
#define K0_PROTOCOL_H    "firmware/main/protocols/protocol.h"
#define K0_APP_CC        "firmware/main/application.cc"
#define K0_HOST_CMAKE    "firmware/main/test/host_app/main/CMakeLists.txt"

/* ---- 冻结 wire 样例（逐字取自 interface-contract.md §1.3⑦⑧ 示例 data 对象；
 *      设备解析的是 _display_wire_arguments 下发的 data 字符串。EQ-4。） --------- */

/* §1.3⑦ 示例（restore:true——QQ/恢复通道形态）：余额 23、目标游乐场 40、
 * recent 一条兑换分录（-40，63 -> 23）。 */
static const char *K0_HOME_SAMPLE_RESTORE =
    "{\"revision\": 12, \"balance\": 23, "
    "\"target\": {\"id\": \"preset-playground\", \"name\": \"游乐场\", "
    "\"points\": 40, \"asset\": \"sc-wheel\"}, "
    "\"recent\": [{\"id\": \"pt_01J...\", \"delta\": -40, "
    "\"reason\": \"兑换游乐场\", "
    "\"occurred_at\": \"2026-11-03T20:00:00+08:00\", "
    "\"balance_before\": 63, \"balance_after\": 23}], "
    "\"restore\": true}";

/* §1.3⑦ 示例（restore 缺省——语音成功路径形态；字段同上）。 */
static const char *K0_HOME_SAMPLE_ACTIVE =
    "{\"revision\": 13, \"balance\": 28, "
    "\"target\": {\"id\": \"preset-playground\", \"name\": \"游乐场\", "
    "\"points\": 40, \"asset\": \"sc-wheel\"}, "
    "\"recent\": [{\"id\": \"pt_01K...\", \"delta\": 5, "
    "\"reason\": \"今天扫地\", "
    "\"occurred_at\": \"2026-11-04T09:00:00+08:00\", "
    "\"balance_before\": 23, \"balance_after\": 28}]}";

/* §1.3⑧ 示例（restore:true + reset:true——WS 建连恢复/重置页形态）。
 * page.next_cursor 非空（has_more=true）供 apply 续页语义使用。 */
static const char *K0_LEDGER_SAMPLE =
    "{\"revision\": 12, \"balance\": 23, "
    "\"items\": [{\"id\": \"pt_01J...\", \"delta\": -40, "
    "\"reason\": \"兑换游乐场\", "
    "\"occurred_at\": \"2026-11-03T20:00:00+08:00\", "
    "\"balance_before\": 63, \"balance_after\": 23}], "
    "\"page\": {\"total\": 25, \"next_cursor\": \"opaque-cursor-or-null\", "
    "\"has_more\": true, \"reset\": true}, "
    "\"restore\": true}";

/* ========================================================================== *
 * TEST 1 — K0PTS-F1 view 路由 additive 数值码（0..6 零偏移 + 7/8 新增 + 头文件
 *          单源）+ 网关决定类控制动作设备侧不可路由负锚。
 * coverage: K0PTS-F1 -- kidpal_display_route 十值决策表真跑 + view_router.h 枚举锚
 *           （POINTS_HOME=7 / POINTS_LEDGER=8 同行；既有 0..6 行零偏移）+
 *           kidpal_display_control_action 负锚（add_points/redeem_points -> UNKNOWN；
 *           既有 0..3 零偏移 must-stay-green，RDG-F1 冻结域不重开）
 * RED today（设计态）: view_router 未扩 points 视图（哨兵 -99）+ 头锚缺席。
 * ========================================================================== */
TEST_CASE("points spike: VRT_ROUTE_POINTS_HOME=7/LEDGER=8 additive, 0..6 zero-offset, "
          "gateway-decision actions unroutable (K0PTS-F1)",
          "[points_spike][K0PTS-F1][linux][behavior][structural]")
{
    static const struct {
        const char *view;
        int expect;
    } table[] = {
        { "text",        K0_ROUTE_TEXT },
        { "home",        K0_ROUTE_HOME },
        { "control",     K0_ROUTE_CONTROL },
        { "char",        K0_ROUTE_CHAR },
        { "schedule",    K0_ROUTE_SCHEDULE },
        { "story_list",  K0_ROUTE_STORY_LIST },
        { "story_text",  K0_ROUTE_STORY_TEXT },
        { "points_home", K0_ROUTE_POINTS_HOME },
        { "points_ledger", K0_ROUTE_POINTS_LEDGER },
    };
    char *hdr;
    size_t i;

    /* ---- 行为：view 路由决策表（哨兵守卫 -> host runner 强符号） -------------- */
    for (i = 0; i < sizeof(table) / sizeof(table[0]); i++) {
        TEST_ASSERT_EQUAL_INT_MESSAGE(
            table[i].expect, kidpal_display_route(table[i].view),
            k0_fm("K0PTS-F1 RED: kidpal_display_route(\"%s\") 未返回 %d —— points "
                  "view 路由 additive 未实现或既有数值码偏移（additive 纪律只增不改/"
                  "移；契约 §1.3 白名单 7→9 的设备消费半；契约见本文件头注 [G1]）",
                  table[i].view, table[i].expect));
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(K0_ROUTE_UNKNOWN, kidpal_display_route("bogus_view"),
        "K0PTS-F1 RED: 未知 view 未降级 VRT_ROUTE_UNKNOWN —— 降级语义被破（V27，"
        "绝不盲渲染）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(K0_ROUTE_UNKNOWN, kidpal_display_route(NULL),
        "K0PTS-F1 RED: NULL view 未降级。");

    /* ---- 负锚：网关决定类控制动作不进设备控制路由（契约 §1.4） ---------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        K0_CTRL_UNKNOWN, kidpal_display_control_action("add_points"),
        "K0PTS-F1 RED: control/add_points 在设备侧可路由 —— 网关决定类不直接下发"
        "设备（契约 §1.4「不直接下发设备」「front 自推通路也不把这两个 action 发给"
        "设备」）；设备侧若可路由 = 记账语义可被 display 流触达（D-10 执行链越界）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        K0_CTRL_UNKNOWN, kidpal_display_control_action("redeem_points"),
        "K0PTS-F1 RED: control/redeem_points 在设备侧可路由 —— 同上（兑换语义越界）。");
    /* 既有动作零偏移（must-stay-green；RDG-F1 冻结域不重开，只钉零偏移）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(K0_CTRL_END_SESSION,
        kidpal_display_control_action("end_session"),
        "K0PTS-F1 RED: end_session 数值码偏移 —— additive 纪律被破（0 不得移位）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(K0_CTRL_STOP_SPEAKING,
        kidpal_display_control_action("stop_speaking"),
        "K0PTS-F1 RED: stop_speaking 数值码偏移（1 不得移位）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(K0_CTRL_START_RECORDING,
        kidpal_display_control_action("start_recording"),
        "K0PTS-F1 RED: start_recording 数值码偏移（2 不得移位）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(K0_CTRL_START_ORGANIZING,
        kidpal_display_control_action("start_organizing"),
        "K0PTS-F1 RED: start_organizing 数值码偏移（3 不得移位，RDG-F1 既有冻结）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(K0_CTRL_UNKNOWN,
        kidpal_display_control_action("bogus_action"),
        "K0PTS-F1 RED: 未知 action 未降级 VRT_CTRL_UNKNOWN。");

    /* ---- 结构：view_router.h 枚举锚（新值在世 + 既有值零偏移） ---------------- */
    K0_REQUIRE_ROOT();
    hdr = k0_strip_file(K0_VIEW_ROUTER_H);
    TEST_ASSERT_NOT_NULL_MESSAGE(hdr, "K0PTS-F1 RED(infra): view_router.h unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        k0_count_lines_with_both(hdr, "VRT_ROUTE_POINTS_HOME", "7") >= 1,
        "K0PTS-F1 RED: view_router.h 无 VRT_ROUTE_POINTS_HOME = 7 同行 —— additive "
        "枚举未落头（数值单源；实现侧用别值 = wire 语义偏移，须复核锚不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(
        k0_count_lines_with_both(hdr, "VRT_ROUTE_POINTS_LEDGER", "8") >= 1,
        "K0PTS-F1 RED: view_router.h 无 VRT_ROUTE_POINTS_LEDGER = 8 同行 —— additive "
        "枚举未落头");
    TEST_ASSERT_TRUE_MESSAGE(
        k0_count_lines_with_both(hdr, "VRT_ROUTE_TEXT", "0") >= 1,
        "K0PTS-F1 RED: view_router.h VRT_ROUTE_TEXT = 0 行偏移/缺失。");
    TEST_ASSERT_TRUE_MESSAGE(
        k0_count_lines_with_both(hdr, "VRT_ROUTE_HOME", "1") >= 1,
        "K0PTS-F1 RED: view_router.h VRT_ROUTE_HOME = 1 行偏移/缺失。");
    TEST_ASSERT_TRUE_MESSAGE(
        k0_count_lines_with_both(hdr, "VRT_ROUTE_CONTROL", "2") >= 1,
        "K0PTS-F1 RED: view_router.h VRT_ROUTE_CONTROL = 2 行偏移/缺失。");
    TEST_ASSERT_TRUE_MESSAGE(
        k0_count_lines_with_both(hdr, "VRT_ROUTE_CHAR", "3") >= 1,
        "K0PTS-F1 RED: view_router.h VRT_ROUTE_CHAR = 3 行偏移/缺失。");
    TEST_ASSERT_TRUE_MESSAGE(
        k0_count_lines_with_both(hdr, "VRT_ROUTE_SCHEDULE", "4") >= 1,
        "K0PTS-F1 RED: view_router.h VRT_ROUTE_SCHEDULE = 4 行偏移/缺失。");
    TEST_ASSERT_TRUE_MESSAGE(
        k0_count_lines_with_both(hdr, "VRT_ROUTE_STORY_LIST", "5") >= 1,
        "K0PTS-F1 RED: view_router.h VRT_ROUTE_STORY_LIST = 5 行偏移/缺失。");
    TEST_ASSERT_TRUE_MESSAGE(
        k0_count_lines_with_both(hdr, "VRT_ROUTE_STORY_TEXT", "6") >= 1,
        "K0PTS-F1 RED: view_router.h VRT_ROUTE_STORY_TEXT = 6 行偏移/缺失。");
    free(hdr);
}

/* ========================================================================== *
 * TEST 2 — K0PTS-F2 points_home 载荷最小消费（契约示例逐字段 + 畸形降级 +
 *          restore 分流）。
 * coverage: K0PTS-F2 -- kidpal_points_home_parse（六键分录 / target 四键 / recent
 *           ≤3 / restore 布尔）+ kidpal_points_home_takeover（restore=true -> 0
 *           只填缓存；缺省/false -> 1 接管前台；NULL -> -1）
 * RED today（设计态）: points_page.h/.c 不在世（哨兵 -99）。
 * ========================================================================== */
TEST_CASE("points spike: points_home parse contract sample and restore takeover fork "
          "(K0PTS-F2)",
          "[points_spike][K0PTS-F2][linux][behavior]")
{
    kidpal_points_home_t f;

    /* ---- restore:true 样例（QQ/恢复通道形态；§1.3⑦ 示例逐字段） --------------- */
    memset(&f, 0, sizeof(f));
    K0_REQUIRE_IMPL_INT(kidpal_points_home_parse(K0_HOME_SAMPLE_RESTORE, &f),
                        "kidpal_points_home_parse");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_points_home_parse(K0_HOME_SAMPLE_RESTORE, &f),
        "K0PTS-F2 RED: 契约示例载荷解析失败 —— points_home 消费缺失（设备黑屏降级 = "
        "K0S-E1「设备实际显示权威余额和原因」不可能成立）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(12, f.revision,
        "K0PTS-F2 RED: revision 解析缺失（权威值，设备只显示不计算）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(23, f.balance,
        "K0PTS-F2 RED: balance 解析缺失 —— 权威余额是设备显示的唯一数据源。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("游乐场", f.target_name,
        "K0PTS-F2 RED: target.name 解析缺失 —— 进度线场景标无目标名。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(40, f.target_points,
        "K0PTS-F2 RED: target.points 解析缺失 —— 进度线刻度无锚。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("sc-wheel", f.target_asset,
        "K0PTS-F2 RED: target.asset 解析缺失（场景美术标识）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, f.recent_count,
        "K0PTS-F2 RED: recent_count 解析缺失。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-40, f.recent[0].delta,
        "K0PTS-F2 RED: 分录 delta 解析缺失（正=加、负=兑，设备单行呈现依据）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("兑换游乐场", f.recent[0].reason,
        "K0PTS-F2 RED: 分录 reason 解析缺失 —— 设备看不到「为什么变动」= K0S-E1 "
        "「权威余额和原因」缺半。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(63, f.recent[0].balance_before,
        "K0PTS-F2 RED: 分录 balance_before 解析缺失（单行「63→23」呈现依据）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(23, f.recent[0].balance_after,
        "K0PTS-F2 RED: 分录 balance_after 解析缺失。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, f.restore,
        "K0PTS-F2 RED: restore=true 未解析为 1 —— 分流载体缺失。");
    /* restore 分流：true = 只填缓存不抢前台（D-07 / 契约 §1.3⑦）。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_home_takeover(&f),
        "K0PTS-F2 RED: restore=true 帧被判定接管前台 —— QQ 写入/WS 建连推送会抢走"
        "孩子正在看的页（D-07 / S-F6.4-AC5 设备半被破）。");

    /* ---- restore 缺省样例（语音成功路径形态）：接管前台 ----------------------- */
    memset(&f, 0, sizeof(f));
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_points_home_parse(K0_HOME_SAMPLE_ACTIVE, &f),
        "K0PTS-F2 RED: restore 缺省载荷解析失败（缺 restore 键必须可解析=0）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, f.restore,
        "K0PTS-F2 RED: restore 缺省未归一为 0。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, kidpal_points_home_takeover(&f),
        "K0PTS-F2 RED: restore 缺省帧未判定接管前台 —— 设备主动语音记账成功必须跳"
        "积分页（S-F6.3-AC2「自动跳转积分本页」/ D-07 例外半）。");

    /* ---- 畸形 / NULL 降级（绝不盲渲染） --------------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_home_parse(NULL, &f),
        "K0PTS-F2 RED: NULL 载荷未降级（V27）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_home_parse("not-json{", &f),
        "K0PTS-F2 RED: 非 JSON 载荷未降级 —— 盲渲染面。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_home_parse("{\"balance\": 5}", &f),
        "K0PTS-F2 RED: 缺 revision/target 的载荷未降级（载荷违反契约 §1.3⑦ 形状）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_home_parse(K0_HOME_SAMPLE_RESTORE, NULL),
        "K0PTS-F2 RED: NULL out 未降级。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_home_takeover(NULL),
        "K0PTS-F2 RED: takeover(NULL) 未降级。");
}

/* ========================================================================== *
 * TEST 3 — K0PTS-F3 points_ledger 载荷解析 + 分页缓存纪律（reset 丢弃 / 同版本
 *          续接 / 跨版本追加拒绝）。
 * coverage: K0PTS-F3 -- kidpal_points_ledger_parse（page 四键 + items 容量 +
 *           canary 分录字段）+ kidpal_points_ledger_apply 决策表
 * RED today（设计态）: points_page.h/.c 不在世（哨兵 -99）。
 * ========================================================================== */
TEST_CASE("points spike: points_ledger parse and pagination cache discipline "
          "(K0PTS-F3)",
          "[points_spike][K0PTS-F3][linux][behavior]")
{
    kidpal_points_ledger_t lg;
    kidpal_points_ledger_view_t v;

    /* ---- 契约示例解析（§1.3⑧） ------------------------------------------------ */
    memset(&lg, 0, sizeof(lg));
    K0_REQUIRE_IMPL_INT(kidpal_points_ledger_parse(K0_LEDGER_SAMPLE, &lg),
                        "kidpal_points_ledger_parse");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_points_ledger_parse(K0_LEDGER_SAMPLE, &lg),
        "K0PTS-F3 RED: 契约示例分页载荷解析失败 —— 详细账本消费缺失（首尾 canary "
        "消费端半死，K0S-E2）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(12, lg.revision, "K0PTS-F3 RED: revision 解析缺失。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(23, lg.balance, "K0PTS-F3 RED: balance 解析缺失。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, lg.item_count, "K0PTS-F3 RED: item_count 解析缺失。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(25, lg.page_total,
        "K0PTS-F3 RED: page.total 解析缺失 —— total 是全量分录数（不能用 items.length "
        "代替，契约 §1.3⑧）。");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("opaque-cursor-or-null", lg.next_cursor,
        "K0PTS-F3 RED: next_cursor 解析缺失 —— keyset 续页载体断裂（K0S-E2）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, lg.has_more, "K0PTS-F3 RED: has_more 解析缺失。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, lg.reset,
        "K0PTS-F3 RED: page.reset 解析缺失 —— 设备无法判定何时丢弃旧分页缓存。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, lg.restore,
        "K0PTS-F3 RED: restore 解析缺失（续页/恢复帧抢前台分流依据）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-40, lg.items[0].delta,
        "K0PTS-F3 RED: 分录字段解析缺失（canary 关联面）。");

    /* ---- apply 决策表 ---------------------------------------------------------- */
    /* ① reset=true：丢弃旧分页缓存再装入（缓存计数被帧内容**替换**，不叠加）。 */
    v.revision = 1; v.cached_count = 9; v.has_more = 1;
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_ledger_apply(&v, &lg),
        "K0PTS-F3 RED: reset=true 帧被拒 —— 设备无法装入首页/重置页。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, v.cached_count,
        "K0PTS-F3 RED: reset=true 未丢弃旧缓存（cached_count 应替换为帧内 1 条，"
        "旧 9 条残留 = 跨版本脏缓存上屏，契约 §四「必须丢弃旧分页缓存再装入」）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(12, v.revision,
        "K0PTS-F3 RED: reset 后缓存版本未随帧更新。");
    /* ② 同版本续页（reset=false）：累加。 */
    {
        kidpal_points_ledger_t cont;
        memset(&cont, 0, sizeof(cont));
        cont.revision = 12; cont.balance = 23; cont.item_count = 2;
        cont.page_total = 25; cont.has_more = 1; cont.reset = 0;
        snprintf(cont.next_cursor, sizeof(cont.next_cursor), "c2");
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_points_ledger_apply(&v, &cont),
            "K0PTS-F3 RED: 同版本续页被拒 —— 正常翻页不可用。");
        TEST_ASSERT_EQUAL_INT_MESSAGE(3, v.cached_count,
            "K0PTS-F3 RED: 同版本续页未累加（1+2 应为 3）—— 翻页丢内容 = 全量可及"
            "（S-F6.2-AC1）被破。");
    }
    /* ③ 跨版本续页（reset=false 且 revision 不一致）：拒绝且缓存零扰动。 */
    {
        kidpal_points_ledger_t stale;
        memset(&stale, 0, sizeof(stale));
        stale.revision = 13; stale.item_count = 5; stale.reset = 0; stale.has_more = 0;
        TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_ledger_apply(&v, &stale),
            "K0PTS-F3 RED: 跨版本续页未被拒绝 —— 旧版本分录追加进新版本缓存 = 账本"
            "失真上屏（契约 §四「不得跨版本追加」明令）。");
        TEST_ASSERT_EQUAL_INT_MESSAGE(3, v.cached_count,
            "K0PTS-F3 RED: 跨版本拒绝路径动了缓存（应零扰动）。");
    }
    /* ④ NULL 降级。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_ledger_apply(NULL, &lg),
        "K0PTS-F3 RED: apply(NULL v) 未降级。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_ledger_apply(&v, NULL),
        "K0PTS-F3 RED: apply(NULL f) 未降级。");

    /* ---- 畸形载荷降级 ---------------------------------------------------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_ledger_parse(NULL, &lg),
        "K0PTS-F3 RED: NULL 载荷未降级。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_ledger_parse("{{{", &lg),
        "K0PTS-F3 RED: 脏 JSON 未降级。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_points_ledger_parse("{\"revision\":1,\"balance\":0,\"items\":[],"
                                   "\"page\":{\"total\":0}}", &lg),
        "K0PTS-F3 RED: page 缺 next_cursor/has_more/reset 未降级（page 四键必带，"
        "契约 §1.3⑧）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_points_ledger_parse(K0_LEDGER_SAMPLE, NULL),
        "K0PTS-F3 RED: NULL out 未降级。");
}

/* ========================================================================== *
 * TEST 4 — K0PTS-F4 消费闭环 + 上行驱动方结构锚 + host runner 登记守。
 * coverage: K0PTS-F4 -- mcp_server.cc points 视图消费接线在世（K0S-E2 消费半）+
 *           protocols/application 侧 kidpal_points_page 上行驱动方在世（K0S-E2
 *           驱动半）+ 登记守（anti-vacuity；builder FIRST ACTION，非 acceptance 条目）
 * RED today（设计态）: 消费/驱动锚缺席 + CMakeLists 未登记。
 * ========================================================================== */
TEST_CASE("points spike: consumption closure, uplink driver anchor, host runner "
          "registration (K0PTS-F4)",
          "[points_spike][K0PTS-F4][structural]")
{
    char *mcp;
    char *proto;
    char *proto_h;
    char *app;
    char *cmake;
    int driver_hits;

    K0_REQUIRE_ROOT();

    /* ---- mcp_server.cc：points 视图消费接线在世 ------------------------------- */
    mcp = k0_strip_file(K0_MCP_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(mcp, "K0PTS-F4 RED(infra): mcp_server.cc unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        (k0_count_sub(mcp, "VRT_ROUTE_POINTS_HOME") +
         k0_count_sub(mcp, "kidpal_points_home_parse")) >= 1,
        "K0PTS-F4 RED: mcp_server.cc 无 points 视图消费接线（VRT_ROUTE_POINTS_HOME / "
        "kidpal_points_home_parse 引用缺席）—— 权威 points_home 帧下发后设备无人消费"
        " = 黑屏降级，K0S-E1 消费端目击不可能成立（设备只消费执行后的权威积分视图，"
        "契约 §1.4）。");
    free(mcp);

    /* ---- 上行驱动方在世（K0S-E2 设备半）：kidpal_points_page 帧构造 ------------ */
    proto = k0_strip_file(K0_PROTOCOL_CC);
    proto_h = k0_strip_file(K0_PROTOCOL_H);
    app = k0_strip_file(K0_APP_CC);
    driver_hits = ((proto != NULL) ? k0_count_sub(proto, "kidpal_points_page") : 0) +
                  ((proto_h != NULL) ? k0_count_sub(proto_h, "kidpal_points_page") : 0) +
                  ((app != NULL) ? k0_count_sub(app, "kidpal_points_page") : 0);
    TEST_ASSERT_TRUE_MESSAGE(
        driver_hits >= 1,
        "K0PTS-F4 RED: kidpal_points_page 上行驱动方缺席（protocol.cc/protocol.h/"
        "application.cc 均未引用）—— 设备永远发不出分页请求，front 应答无人消费 = "
        "首尾 canary 消费端半死（K0S-E2「设备 view 路由和页面消费日志可按同一 canary "
        "关联」被破）。");
    free(proto);
    free(proto_h);
    free(app);

    /* ---- host runner 登记守（anti-vacuity；builder FIRST ACTION） -------------- */
    cmake = k0_read(K0_HOST_CMAKE, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(cmake,
        "K0PTS-F4 RED(infra): host_app/main/CMakeLists.txt unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        strstr(cmake, "points_spike.acceptance.c") != NULL,
        "K0PTS-F4 RED: host_app/main/CMakeLists.txt 未登记 points_spike.acceptance.c"
        " —— 本冻结验收件不进 host runner（全部断言不会被执行 = 空过绿）。该清单是 "
        "K0a/K0 唯一共享构建文件，由 K0 在 K0a merged 后追加（builder FIRST ACTION，"
        "见头注 Run protocol 0）");
    TEST_ASSERT_TRUE_MESSAGE(
        strstr(cmake, "points_page.c") != NULL,
        "K0PTS-F4 RED: host_app/main/CMakeLists.txt 未登记 display/points_page.c 强"
        "符号源 —— weak 哨兵永被覆盖不到（TEST 2/3 行为齿在 host runner 恒红 = 接线"
        "缺陷被掩盖）。");
    free(cmake);
}

/* ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-11-04 初版冻结（TaskPackage 261104-k0-points-e2e-spike; acceptance[]
 *              K0PTS-F1..F4 为冻结边界 + K0PTS-L1..L4 live-manual 单列不落本文件;
 *              requirements v1.71 S-F6.2-AC1/S-F6.3/S-F6.4 + FROZEN PROPOSAL
 *              D-07/D-08-spike/D-10 + 契约 §1.3⑦⑧/§1.4/§四 kidpal_points_page 为
 *              语义权威）。四个 TEST_CASE：K0PTS-F1 行为+结构一件（view 路由 additive
 *              决策表 + 头锚 + 网关决定类不可路由负锚 + 既有零偏移回归守卫）、
 *              K0PTS-F2 行为一件（points_home 契约示例逐字段解析 + restore 分流 +
 *              畸形降级）、K0PTS-F3 行为一件（points_ledger 解析 + apply 分页缓存
 *              纪律决策表）、K0PTS-F4 结构一件（mcp 消费锚 + 上行驱动方锚 + 登记
 *              守 anti-vacuity）。firmware 红基线 deferred: LP8 后置——host/device
 *              build 与真机终验按 K0S-E4 在 K0a merged、重取基线后实跑；冻结落盘
 *              冒烟 = 独立冒烟通道实编（编译通道 error 为零）+ 设计态红取证。
 * --------------------------------------------------------------------------- */
