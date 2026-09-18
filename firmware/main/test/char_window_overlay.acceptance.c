/*
 * Frozen acceptance tests — TaskPackage 260917-m2-k3-char-window-overlay
 * (subsystem: firmware; workstream m2-char-window; M2-K3; mergeOrder 30; riskTier high;
 *  页→窗改造: 查字会话内覆盖窗——层注册 + 键位语义 + 5 分钟自动关窗 + 页点隐藏
 *  + 右栏对账三项 + 锚改名对账)。
 *
 * Story: 里程碑 delivery/milestones/milestones.jsonl#M2-char-window nextAction ③
 * 「页→窗改造」。语义权威 = docs/requirements.md 现文 F3 全节（v1.36 改窗 / v1.37 会话
 * 口径 / v1.38 实现侧对账 / v1.39 BOOT 隔离 / v1.43 5 分钟兜底 / v1.55 播报口径）+
 * §2.2 行 68（查字窗行）/ 行 67（提醒=最高层覆盖，须压过查字窗）+ §2.3 键位矩阵
 * 「查字窗」行 + §2.4 行 186（P4 已撤）。
 *
 * Nature: HOST-SIDE BEHAVIORAL（纯逻辑层模型 kidpal_layer.c / 路由表 key_gesture_parser.cc
 * / 驻留计时纯逻辑模块——均 host runner 可编、真行为面，不走 weak 哨兵）+ HOST-SIDE
 * STRUCTURAL（设备专属面 kidpal_render_node.{h,cc} / char_page.cc / mcp_server.cc 不进
 * host 构建，齿形态照 key_layer_dispatch.acceptance.c KD-A2 结构半与 retract_p4_three_page
 * 先例：稳定签名锚切区 + 区内计数，零行号钉点）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements，本文件只读扫工作树 + 真调已入
 * runner 的纯逻辑实现; 「home =」为冻结落点）:
 *
 *   [1] 查字窗层进 kidpal_layer 栈模型（M2K3-A1; home = firmware/main/display/
 *       kidpal_layer.{h,c}）:
 *       kidpal_layer.h 常量闭集**插入**查字窗层并把 REMINDER/PROVISION 整体上移：
 *         KIDPAL_LAYER_CHAR_WINDOW = 5   （新层——查字窗，会话内覆盖窗，非常驻页）
 *         KIDPAL_LAYER_REMINDER    = 6   （原 5；提醒=覆盖查字窗层）
 *         KIDPAL_LAYER_PROVISION   = 7   （原 6）
 *       其余四值零偏移（LOCAL_PAGE=2 / VOICE=3 / SERVER_PAGE=4 不动；NONE=0 / CHAT=1
 *       非栈成员不动）。优先级 = 数值越大越靠前：PROVISION > REMINDER > CHAR_WINDOW >
 *       SERVER_PAGE > VOICE > LOCAL_PAGE（§2.2 行 67 提醒压过查字窗 + 行 68 窗在
 *       当前页之上）。全仓消费点均为符号常量引用、零数值字面耦合（唯一字面在两处
 *       注释里），故「插入 + 上移」是符号级安全改动。
 *       kidpal_layer_is_stackable 合法域上界随符号（现写 <= KIDPAL_LAYER_PROVISION，
 *       自动跟随）；合法编码种数注释随闭集更新（6 种，仍 < KIDPAL_LAYER_STACK_MAX=8）。
 *       kidpal_layer_pop_transients **transient 集纳入 CHAR_WINDOW**（查字窗是 transient
 *       覆盖层，关窗 = 出清层回基层）——VOICE/SERVER_PAGE/REMINDER/CHAR_WINDOW 四者出净，
 *       LOCAL_PAGE/PROVISION 零触碰不变。
 *       行为契约：push(CHAR_WINDOW) 覆盖当前页（top = CHAR_WINDOW、LOCAL_PAGE 仍在场 =
 *       「当前页之上覆盖」）；pop / pop_transients 后 top 回 LOCAL_PAGE，**不翻页**
 *       （s_local_page 不被本层改动）。
 *
 *   [2] 键位语义矩阵（M2K3-A2; home = firmware/main/key_gesture_parser.{h,cc},
 *       纯逻辑单源）:
 *       global_action_t **append-only 追加**（既有五值 NONE=0/SESSION_TOGGLE=1/HOME=2/
 *       PAGE_NEXT=3/OVERLAY_CLOSE=4 零偏移）:
 *         GLOBAL_ACT_CHAR_REPLAY = 5   -- 查字窗 KEY 短按 = 重播笔顺（含打断当前播报重演）
 *       key_route_layer(key, g, overlay_top) 的第三参语义**扩为前台层归一化值**（原
 *       K1a 0/非 0 归一化是其子集，既有齿语义零回归）:
 *         overlay_top == 0            -> 基层列（与 key_route_global 全表等价，不变）
 *         overlay_top == CHAR_WINDOW  -> **查字窗列（本包新增列）**：
 *           KEY  CLICK      -> GLOBAL_ACT_CHAR_REPLAY（重播——非 SESSION_TOGGLE）
 *           KEY  LONG_PRESS -> GLOBAL_ACT_OVERLAY_CLOSE（关窗回原页）
 *           BOOT CLICK      -> GLOBAL_ACT_OVERLAY_CLOSE（v1.39 隔离，不产翻页）
 *           其余（双击族 / BOOT 长按）-> GLOBAL_ACT_NONE
 *         其它非 0 值 -> 通用覆盖层列（K1a 原样：KEY CLICK->SESSION_TOGGLE /
 *           KEY LONG_PRESS->OVERLAY_CLOSE / BOOT CLICK->OVERLAY_CLOSE / 其余 NONE）
 *       key_route_global 本体零改动（基层列实现载体）。
 *       板级 dispatch（home = boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc）
 *       把**前台层值**（kidpal_render_node_front_layer()）喂进 key_route_layer（覆盖层时；
 *       基层/配网喂 0）——GLOBAL_ACT_CHAR_REPLAY 被消费映射 kidpal_char_page_replay()。
 *
 *   [3] 5 分钟驻留计时（M2K3-A3; home = firmware/main/display/char_window.{h,c}，纯逻辑、
 *       无 chip/FreeRTOS/ESP-component/LVGL 依赖）:
 *       #define KIDPAL_CHAR_WINDOW_IDLE_MS 300000   -- 5 分钟无操作上限（v1.43 兜底）
 *       int kidpal_char_window_idle_step(unsigned long *idle_ms, unsigned long dt_ms,
 *                                        int key_pressed);
 *         -- 驻留计时步进（显式注入毫秒，无真实 timer）：idle_ms 进/出（距最近一次
 *            操作的毫秒数）；key_pressed 非 0 = 本拍有按键 -> *idle_ms 归零（**计时随
 *            任意按键重计**）且返回 0（按键本身是操作，不关窗）；否则 *idle_ms += dt_ms，
 *            返回 (*idle_ms >= KIDPAL_CHAR_WINDOW_IDLE_MS) ? 1 : 0（1 = 该关窗）。
 *            idle_ms == NULL -> -1（从不为 UB）。
 *       关窗落地面 = kidpal_render_node_dismiss_front_overlay（回原页、**不翻页**），
 *       与 BOOT/KEY 长按同语义；**播完会话即结束、不回聆听态**——查字窗显示路径不触发
 *       任何语音会话/聆听入口（结构半 [6]）。
 *
 *   [4] 页点隐藏闭集纳入查字窗层（M2K3-A4; home = firmware/main/display/
 *       kidpal_render_node.cc）:
 *       kidpal_render_node_front_is_overlay() 的覆盖层闭集**纳入 CHAR_WINDOW**：
 *       前台 owner ∈ {VOICE, SERVER_PAGE, REMINDER, CHAR_WINDOW} -> 1；LOCAL_PAGE /
 *       PROVISION / CHAT / NONE -> 0。页点隐藏（keybar）与键路由同源，本条使查字窗前
 *       台时中槽页点自动隐藏（§2.2「覆盖层不显示页点」），左右槽照常给操作提示。
 *
 *   [5] 右栏对账三项 + 锚改名（M2K3-A5/A6; home = firmware/main/display/char_page.{h,cc}）:
 *       a) 部首行补上屏（S-F3.2 AC1）: 速查卡 label 拼装纳入 card.radical（现状
 *          snprintf 只拼 pinyin/stroke_total/words，radical 有值未上屏）；
 *       b) 进度珠死成员清理: struct CharPageState 的 progress_label 死成员（零赋值）
 *          删除 + char_page.cc 头注「进度珠」虚标清除（§8 已砍项 / v1.38 对账①）；
 *       c) 握笔小手 12px 圆环占位删除: charpage_draw_penhand 函数与 CHARPAGE_PRESS_SIZE
 *          常量整体删除（v1.38 对账② 改口径后小手资产不再需要——当前笔改白衬边）；
 *       d) 锚改名 canvas-p4 -> char-window: char_page.{h,cc} 头注携带 `char-window` 锚、
 *          清除「P4 页」常驻页时代自述（v1.38 登记的 Phase 1 dev 修复项）。
 *       另: kidpal_render_node.h/.cc 的 M2-K2 遗留写侧死态 API kidpal_render_node_cache_char
 *       / kidpal_render_node_cache_char_data **整体收拢**（声明 + 定义 + mcp_server.cc
 *       调用点）——M2K2-E3 明注「保留不动、由 M2-K3 改窗时统一收，禁静默删除」。
 *       **不顺手改动 mcp_server.cc 的 view=char 路由本体**（ack route=char + 缺字段
 *       降级门 + kidpal_char_page_show 调用点保留；E2 服务端下发语义不动）。
 *
 *   [6] 查字窗显示时机（M2K3-A3/A6; home = firmware/main/mcp_server.cc +
 *       kidpal_render_node.{h,cc}）:
 *       查字渲染路径把 CHAR_WINDOW 层入栈（覆盖窗显示时机 = 实现侧），关窗走 dismiss
 *       （清 transient 回基层）。**不得**在该路径起会话/聆听（无 ToggleChatState /
 *       kidpal_render_node_show_chat / kidpal_render_node_voice_begin）——「播完会话即
 *       结束、不回聆听态」（§2.2 行 68 / F3 窗口交互包）。
 *
 *   [7] Builder FIRST ACTION（本文件进 runner 的唯一前置）: 向
 *       firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS 追加:
 *         "../../char_window_overlay.acceptance.c"
 *         "../../../display/char_window.c"   （[3] 强符号源; 新纯逻辑模块）
 *       置于 "../../host_infra_smoke.acceptance.c" 之前（smoke 保持末位 = 最先执行，
 *       包 0 boundary 齿注册序守卫；本文件 test 7 tooth 1 校验此序）。被测实现
 *       kidpal_layer.c / key_gesture_parser.cc **已在册**（零新增实现接线）+ char_window.c
 *       **本包新增**（+1 实现源）。
 *
 *   [8] mutation 自证（mutationRequired=true）：变异命名不得保留被测符号原字面为子串
 *       （KR-A3 _TOGGLE_X 教训）；逐齿可反转性见 acceptance-author 交付报告
 *       （mutationEvidence.perAcceptance 由 planner 回填）。
 *
 *   [9] 5 分钟驻留计时**设备侧驱动**（M2K3-A3 驱动半; sealing pass 2026-09-19 补钉;
 *       home = firmware/main/display/kidpal_render_node.{h,cc} + boards/
 *       waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc）:
 *       [3] 的纯逻辑模块 char_window.c 原**全仓零活消费者**（只登记进设备构建、无人调用）
 *       ——A7 真机轮的窗口交互包明确含「5 分钟没动自动关窗」, 驱动不接线则该项在设备上
 *       不可能真绿。本 seam 钉死驱动落地面与形态（builder 建议落点, 已冻结为形态）:
 *       a) **落地面 = kidpal_render_node 既有 1s tick**（kidpal_render_node_tick, 由 init
 *          创建的 1s LVGL 定时器回调驱动; 不另起 timer）。kidpal_render_node.cc 持一个
 *          文件级 static 无操作累计毫秒（初值 0）。
 *       b) **喂 step**: tick 函数体内调
 *          kidpal_char_window_idle_step(&累计器, RN_TICK_MS(1000), key_pressed_flag)。
 *          该调用须在 tick 函数区（本齿切区锚 = tick 定义 -> cache_schedule 定义）内。
 *       c) **查字窗前台门控**: 累计/关窗仅在查字窗为前台时生效——tick 函数区内须引用
 *          KIDPAL_LAYER_CHAR_WINDOW 符号（如 kidpal_layer_top(&s_layer_stack) ==
 *          KIDPAL_LAYER_CHAR_WINDOW / kidpal_layer_has(...) / front_layer() == ...）。
 *          非查字窗前台不得累计（否则窗一开首拍即因历史累计越限而立即关窗）。
 *       d) **到点关窗**: step 返回 1（>= 5 分钟）-> 调
 *          kidpal_render_node_dismiss_front_overlay()（关窗唯一入口, 回原页不翻页,
 *          与 BOOT/KEY 长按同语义; v1.36/v1.39）。该调用须在 tick 函数区内。
 *       e) **任意按键重计**: 新增设备侧公开入口
 *          void kidpal_render_node_char_window_key_hit(void);（kidpal_render_node.h 声明 +
 *          .cc 定义）, 由板级 KEY/BOOT 手势回调入口（waveshare-s3-rlcd-4.2.cc）在**任意
 *          键**手势时调用——置「本拍有按键」标志喂 tick 的 key_pressed, 或直接归零累计器;
 *          两条路径任选, 须满足 v1.43「计时随任意按键重计」。**本 API 名为冻结形态**,
 *          builder 若需改形态须经 planner 重派 acceptance-author。
 *       **LVGL 定时器回调内（关窗）的递归/时序安全** = 实现侧（builder 职责; 无本地板
 *       不可脑补）——本 seam 只钉驱动落地面与形态, 不钉安全证明。承载齿 = test 8（全部
 *       正向断言走注释剥离后计数, 防注释假绿）。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: M2K3-A1 层注册 -> test 1（行为半: 新层可入栈/覆盖当前页/pop 回原页不翻页/
 *             pop_transients 纳查字窗层/优先级序）+ test 2（结构半: kidpal_layer.h 闭集
 *             插入 + pop_transients transient 集纳入 + front_is_overlay 闭集纳入 +
 *             CHAR_WINDOW 入栈点在世）
 *   coverage: M2K3-A2 键位语义矩阵 -> test 3（行为半: key_route_layer 查字窗列全表 +
 *             通用覆盖层列/基层列零回归 + 越界防御）+ test 2（结构半: 板级 dispatch 喂
 *             前台层值 + CHAR_REPLAY 消费映射 replay）
 *   coverage: M2K3-A3 驻留计时 -> test 4（行为半: idle_step 5 分钟阈值/任意按键重计/
 *             防御）+ test 5（结构半: 查字窗显示路径零会话/聆听入口 = 播完不回聆听态）
 *             + test 8（驱动半: seam [9] 设备侧驱动接线——1s tick 喂 idle_step + 查字窗
 *             前台门控 + 到点 dismiss 关窗 + 板级任意键经 char_window_key_hit 归零）
 *   coverage: M2K3-A4 页点隐藏 -> test 2（结构半: front_is_overlay 闭集纳入 CHAR_WINDOW）
 *             + test 6（行为半守卫: 覆盖层归一化值页点隐藏零回归）
 *   coverage: M2K3-A5 右栏对账三项 -> test 5（结构半: 部首行补上屏 + progress_label 死
 *             成员清零 + 头注虚标清除 + 握笔小手圆环/常量清零）+ test 6（行为半:
 *             kidpal_char_card_build 部首字段装配守卫）
 *   coverage: M2K3-A6 锚改名 + K2 遗留收拢 -> test 5（结构半: char-window 锚在世 / P4 页
 *             自述清零 / cache_char 两 API 整体收拢）+ test 5 tooth（mcp view=char 路由
 *             本体零触碰守卫）
 *   coverage: S-F3.3 AC2（sourceRef docs/requirements.md#S-F3.3; acceptance-author 判定
 *             纳入——见报告「S-F3.3 AC2 判定」段）-> test 5（呈现面: char_page.cc 消费
 *             kidpal_char_card_build 的 progress 单源「写完啦·共 N 笔」）+ test 6（数据面
 *             progress 文案零回归守卫）
 *   coverage: M2K3-A7 A8 真机轮（human 持板, kind=structural）-> 不落 C 齿, 见文件尾
 *             「A7/A8 deferred」段（S-F3.1 AC1–AC4 / S-F3.2 AC1 / S-F3.3 AC1–AC2 /
 *             交互包全项 / 提醒压查字窗 / NFR-PERF-2 live 复测）
 *   coverage: M2K3-E1 host 全量零新增红 -> test 7（注册序守卫）+ test 8（驱动面新齿,
*             封口相位设计态红 = 本包最后一个待 builder 补的驱动面）+ 文件尾「终跑判读」段
 *   coverage: M2K3-E2 铁律（零契约/零 spec/零 agent-gate 触及）-> test 7 tooth 2/3
 *             （requirements 查字窗锚 + 契约面在世 canary + 扫描器自检）
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   0) builder wiring 见 [7]。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build   （git-bash 下 MSYS_NO_PATHCONV=1 必加）
 *   2) cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *      bash -c "cd /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/
 *      host_app && ./build/host_infra_test.elf"
 *      （elf 相对 host_app/ 目录，从仓根跑 ./build/... 必报 No such file）
 *   Freeze-phase red baseline: 见文件尾「RED baseline 判读口径」段。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>

/* 共享底座头（相对路径, host runner 与独立 stub 编译都可达）。 */
#include "../key_gesture_parser.h"
#include "../display/kidpal_layer.h"
#include "../display/keybar.h"        /* A4 行为半守卫: kidpal_keybar_dots（页点隐藏机制） */
#include "../display/char_stroke.h"   /* A5 行为半守卫: kidpal_char_card_build（部首字段装配） */

#define CWX_MAX_PATH 1024
#define CWX_MAX_READ (8u * 1024u * 1024u)

/* 被扫文件（结构齿锚, 相对仓根；行号快照非长期事实源, 齿锚全部用符号锚）。 */
#define CWX_LAYER_H    "firmware/main/display/kidpal_layer.h"
#define CWX_LAYER_C    "firmware/main/display/kidpal_layer.c"
#define CWX_KEY_H      "firmware/main/key_gesture_parser.h"
#define CWX_KEY_CC     "firmware/main/key_gesture_parser.cc"
#define CWX_RN_H       "firmware/main/display/kidpal_render_node.h"
#define CWX_RN_CC      "firmware/main/display/kidpal_render_node.cc"
#define CWX_CHAR_H     "firmware/main/display/char_page.h"
#define CWX_CHAR_CC    "firmware/main/display/char_page.cc"
#define CWX_MCP_CC     "firmware/main/mcp_server.cc"
#define CWX_BOARD_CC   "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"
#define CWX_KEYBAR_C   "firmware/main/display/keybar.c"
#define CWX_CMAKELISTS "firmware/main/test/host_app/main/CMakeLists.txt"
#define CWX_REQUIREMENTS "docs/requirements.md"
#define CWX_CONTRACT   "docs/contracts/interface-contract.md"

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — weak sentinels（强定义落地后覆盖）。
 * kidpal_layer_push/pop/... 与 key_route_layer：强定义已在 runner（kidpal_layer.c /
 * key_gesture_parser.cc）——本包新增的**查字窗列**由强定义补实现，非哨兵。
 * kidpal_char_window_idle_step：强定义 home = display/char_window.c（本包新增模块,
 * builder 落地 + 接线后覆盖本哨兵）。
 * ------------------------------------------------------------------------ */
#define CWX_UNIMPLEMENTED (-2)

/* 冻结数值码（append-only 追加位; 以字面量钉值保证冻结期本文件先可编译——K1a
 * GLOBAL_ACT_OVERLAY_CLOSE 先例。builder 落 kidpal_layer.h / key_gesture_parser.h 后
 * assert 的是实际返回值。） */
#define CWX_OVERLAY_CLOSE_VALUE 4   /* GLOBAL_ACT_OVERLAY_CLOSE（K1a 冻结位） */
#define CWX_CHAR_REPLAY_VALUE   5   /* GLOBAL_ACT_CHAR_REPLAY（本包 append-only 追加位） */
/* 查字窗层数值码 fallback（kidpal_layer.h 落地前的编译兜底; 与落地值逐值一致 = 5。
 * 若头已定义则用头值——行为齿传符号, 值不经手本宏。） */
#ifndef KIDPAL_LAYER_CHAR_WINDOW
#define KIDPAL_LAYER_CHAR_WINDOW 5
#endif
/* 5 分钟驻留上限 fallback（char_window.h 落地前编译兜底; 与落地值一致 = 300000）。 */
#ifndef KIDPAL_CHAR_WINDOW_IDLE_MS
#define KIDPAL_CHAR_WINDOW_IDLE_MS 300000UL
#endif

/* [3] 驻留计时步进弱哨兵（home = display/char_window.c）。 */
__attribute__((weak)) int kidpal_char_window_idle_step(unsigned long *idle_ms,
                                                       unsigned long dt_ms,
                                                       int key_pressed)
{
    (void)idle_ms; (void)dt_ms; (void)key_pressed;
    return CWX_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（host 侧结构齿, key_layer_dispatch / retract_p4_three_page
 * 先例同款）。全部扫描跑在真实工作树上, root 以手持 cwd 定位（仓根 /
 * firmware/main/test/host_app 两处都可跑）。test / host_app / build / .git / assets
 * 目录排除——test 目录里住着本冻结文件自身（其注释合法提及冻结 needle）。
 * ------------------------------------------------------------------------ */

static char cwx_root[CWX_MAX_PATH];
static int cwx_root_ready;

static int cwx_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int cwx_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[CWX_MAX_PATH];
    size_t i;

    if (cwx_root_ready) {
        return 1;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (cwx_is_regular(probe)) {
            snprintf(cwx_root, sizeof(cwx_root), "%s", cands[i]);
            cwx_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int cwx_rel(char *out, size_t cap, const char *rel)
{
    if (!cwx_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", cwx_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", cwx_root, rel);
    return 1;
}

static int cwx_exists(const char *rel)
{
    char p[CWX_MAX_PATH];
    return cwx_rel(p, sizeof(p), rel) && cwx_is_regular(p);
}

static char *cwx_read(const char *rel, size_t *out_len)
{
    char p[CWX_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!cwx_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > CWX_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int cwx_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int cwx_file_contains(const char *rel, const char *needle)
{
    char *buf = cwx_read(rel, NULL);
    int r;
    if (buf == NULL) {
        return 0;
    }
    r = cwx_contains(buf, needle);
    free(buf);
    return r;
}

/* Occurrence count of needle inside one file (0 when unreadable/absent). */
static int cwx_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = cwx_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = buf; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    free(buf);
    return count;
}

/* Slice of rel from the FIRST occurrence of start_needle up to (exclusive) the first
 * occurrence of end_needle AFTER it.  Returns a malloc'd NUL-terminated slice or NULL
 * (unreadable file / either needle absent).  Caller frees.  Used for function-body
 * region pins——两个 needle 都取稳定签名行, 禁行号钉点。 */
static char *cwx_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = cwx_read(rel, NULL);
    char *s, *e;
    char *slice;
    size_t span;

    if (buf == NULL) {
        return NULL;
    }
    s = strstr(buf, start_needle);
    if (s == NULL) {
        free(buf);
        return NULL;
    }
    e = strstr(s + strlen(start_needle), end_needle);
    if (e == NULL) {
        e = buf + strlen(buf); /* start 是文件内最后一个锚——切到 EOF */
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

static int cwx_count_in_slice(const char *slice, const char *needle)
{
    int count = 0;
    const char *h;
    size_t n;

    if (slice == NULL) {
        return 0;
    }
    n = strlen(needle);
    for (h = slice; (h = strstr(h, needle)) != NULL; h += (n ? n : 1)) {
        count++;
    }
    return count;
}

/* -------------------------------------------------------------------------- *
 * Comment-stripping scanner support（260917-m2-k3 sealing pass）:
 *   裸源扫描分不清「代码」与「注释」——一个只出现在**注释**里的 needle 会满足
 *   「某代码形态在世」的正向齿（实测教训：builder M4 变异 = 真删部首行后，snprintf
 *   上方一条携带 `card.radical` 字面的说明注释仍让 A5a 齿保持绿；删注释后才转红）。
 *   下列助手把注释字节抹成空格，使正向齿只能被**可执行源**满足。
 *
 *   cwx_strip_c_comments(): C/C++ `//` 行注释 + 块注释。**字符串/字符字面量体保留**
 *   （故 needle 里自带字符串字面量的齿——如 cJSON_AddStringToObject(ack, "route",
 *   "char")——仍能命中），但扫描器会跳过字面量体，不把 `"..."` / '...' 里的 `//`
 *   误判成注释。
 *   cwx_strip_hash_comments(): CMake `#`-到-行尾注释（引号串保留），使被注释掉的
 *   源行无法满足接线齿。
 *   两者都保持缓冲长度（注释字节 -> 空格），调用方负责 free；不可读/NULL -> NULL。
 * ------------------------------------------------------------------------ */

static char *cwx_strip_c_comments(const char *src)
{
    size_t n, i = 0;
    char *out;

    assert(src != NULL);
    n = strlen(src);
    out = (char *)malloc(n + 1);
    assert(out != NULL);
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

static char *cwx_strip_c_comments_file(const char *rel)
{
    char *buf = cwx_read(rel, NULL);
    char *out;

    if (buf == NULL) {
        return NULL;
    }
    out = cwx_strip_c_comments(buf);
    free(buf);
    return out;
}

static char *cwx_strip_hash_comments(const char *src)
{
    size_t n, i = 0;
    char *out;

    assert(src != NULL);
    n = strlen(src);
    out = (char *)malloc(n + 1);
    assert(out != NULL);
    memcpy(out, src, n + 1);

    while (i < n) {
        char c = out[i];
        if (c == '"') {
            i++;
            while (i < n) {
                if (out[i] == '\\' && i + 1 < n) { i += 2; continue; }
                if (out[i] == '"') { i++; break; }
                i++;
            }
            continue;
        }
        if (c == '#') {
            while (i < n && out[i] != '\n') { out[i] = ' '; i++; }
            continue;
        }
        i++;
    }
    return out;
}

/* 注释剥离后的文件级 needle 计数/包含（正向结构齿用；注释无法满足）。 */
static int cwx_count_in_file_code(const char *rel, const char *needle)
{
    char *stripped = cwx_strip_c_comments_file(rel);
    int r;

    if (stripped == NULL) {
        return 0;
    }
    r = cwx_count_in_slice(stripped, needle);
    free(stripped);
    return r;
}

static int cwx_file_code_contains(const char *rel, const char *needle)
{
    char *stripped = cwx_strip_c_comments_file(rel);
    int r;

    if (stripped == NULL) {
        return 0;
    }
    r = cwx_contains(stripped, needle);
    free(stripped);
    return r;
}

/* 注释剥离后的切区 needle 计数（正向结构齿用；注释无法满足）。 */
static int cwx_count_in_slice_code(const char *slice, const char *needle)
{
    char *stripped;
    int r;

    if (slice == NULL) {
        return 0;
    }
    stripped = cwx_strip_c_comments(slice);
    r = cwx_count_in_slice(stripped, needle);
    free(stripped);
    return r;
}

#define CWX_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(cwx_locate_root(), \
    "CWX RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* Shared red-string for the frozen seam not being implemented/wired yet. */
static void cwx_require_implemented(int value, const char *what)
{
    if (value == CWX_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "CWX RED: frozen seam NOT implemented/wired — %s "
                 "(builder must land display/char_window.{h,c} per the seam contract in "
                 "firmware/main/test/char_window_overlay.acceptance.c and wire char_window.c "
                 "+ this file into HOST_ACCEPTANCE_SRCS)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* printf-style helper: dynamic diagnostics pre-formatted into a static buffer then
 * passed to the single-string assert macros（KLD/RTP 先例）。 */
static char cwx_fm_buf[1024];
static const char *cwx_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *cwx_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(cwx_fm_buf, sizeof(cwx_fm_buf), fmt, ap);
    va_end(ap);
    return cwx_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — M2K3-A1 查字窗层栈行为（behavioral，真实现源 kidpal_layer.c 已在册）
 * 新层可入栈 / 覆盖当前页（top = CHAR_WINDOW、LOCAL_PAGE 仍在场）/ pop 回原页不翻页
 * / pop_transients 纳查字窗层 / 优先级序（REMINDER > CHAR_WINDOW > SERVER_PAGE，§2.2
 * 行 67 提醒压过查字窗）/ 下层三值零偏移。
 * RED today: KIDPAL_LAYER_CHAR_WINDOW 未入闭集（is_stackable 上界 = PROVISION=6，
 * push(5) 合法但语义未注册——本齿以「闭集序 + transient 集」的真实缺失行为红）。
 * ------------------------------------------------------------------------ */
/* coverage: M2K3-A1 -- 查字窗层进 kidpal_layer 栈模型: 可入栈/覆盖当前页/pop 回原页不翻页 + pop_transients 纳查字窗层（关窗 = 清 transient 回基层）+ 优先级序 REMINDER > CHAR_WINDOW > SERVER_PAGE + 下层 LOCAL_PAGE/VOICE/SERVER_PAGE 三值零偏移 */
TEST_CASE("查字窗层注册: 栈行为+覆盖语义+清层纳查字窗层+优先级序 (M2K3-A1)",
          "[char_window_overlay][M2K3-A1][layer]")
{
    kidpal_layer_stack_t st;
    int popped;

    /* ---- 下层三值零偏移（插入 = 符号级安全改动的前提，须先钉住）------------- */
    TEST_ASSERT_TRUE_MESSAGE((int)KIDPAL_LAYER_LOCAL_PAGE == 2 &&
                                 (int)KIDPAL_LAYER_VOICE == 3 &&
                                 (int)KIDPAL_LAYER_SERVER_PAGE == 4,
        cwx_fm("M2K3-A1 RED: 下层数值码偏移 (LOCAL_PAGE=%d, VOICE=%d, SERVER_PAGE=%d) "
               "期望 (2, 3, 4)——插入查字窗层把既有下层挤走（既有冻结码不得偏移; "
               "只允许 REMINDER/PROVISION 上移）",
               (int)KIDPAL_LAYER_LOCAL_PAGE, (int)KIDPAL_LAYER_VOICE,
               (int)KIDPAL_LAYER_SERVER_PAGE));

    /* ---- 优先级序（§2.2 行 67 提醒=最高层覆盖须压过查字窗; 窗在服务端推送页之上）-- */
    TEST_ASSERT_TRUE_MESSAGE((int)KIDPAL_LAYER_CHAR_WINDOW > (int)KIDPAL_LAYER_SERVER_PAGE &&
                                 (int)KIDPAL_LAYER_CHAR_WINDOW > (int)KIDPAL_LAYER_LOCAL_PAGE &&
                                 (int)KIDPAL_LAYER_REMINDER > (int)KIDPAL_LAYER_CHAR_WINDOW &&
                                 (int)KIDPAL_LAYER_PROVISION > (int)KIDPAL_LAYER_REMINDER,
        cwx_fm("M2K3-A1 RED: 层优先级序漂移 (CHAR_WINDOW=%d, LOCAL_PAGE=%d, "
               "SERVER_PAGE=%d, REMINDER=%d, PROVISION=%d) 期望 LOCAL_PAGE < "
               "CHAR_WINDOW < REMINDER < PROVISION——查字窗层未落 LOCAL_PAGE 之上 / "
               "REMINDER 之下（§2.2 行 67 提醒压过查字窗 + 行 68 窗在当前页之上）",
               (int)KIDPAL_LAYER_CHAR_WINDOW, (int)KIDPAL_LAYER_LOCAL_PAGE,
               (int)KIDPAL_LAYER_SERVER_PAGE, (int)KIDPAL_LAYER_REMINDER,
               (int)KIDPAL_LAYER_PROVISION));

    /* ---- 入栈 / 覆盖当前页 / pop 回原页 ------------------------------------ */
    memset(&st, 0, sizeof(st));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_LOCAL_PAGE) == 0,
        "M2K3-A1 RED(infra): LOCAL_PAGE push 失败——fixture 建栈失败（底座回归）");

    /* tooth: 查字窗层可入栈（stackable 语义一致——新层是合法栈成员）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_CHAR_WINDOW) == 0,
        cwx_fm("M2K3-A1 RED: KIDPAL_LAYER_CHAR_WINDOW(%d) push 失败——新层不是合法栈"
               "成员（stackable 域未纳查字窗层）", (int)KIDPAL_LAYER_CHAR_WINDOW));

    /* tooth: 弹窗 = 当前页之上覆盖——top = CHAR_WINDOW、LOCAL_PAGE 仍在场。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_top(&st) == KIDPAL_LAYER_CHAR_WINDOW,
        cwx_fm("M2K3-A1 RED: 查字窗入栈后 top=%d 期望 CHAR_WINDOW(%d)——窗未覆盖当前页 "
               "（弹窗 = 当前页之上覆盖, F3）", (int)kidpal_layer_top(&st),
               (int)KIDPAL_LAYER_CHAR_WINDOW));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_has(&st, KIDPAL_LAYER_LOCAL_PAGE) == 1,
        "M2K3-A1 RED: 查字窗入栈后 LOCAL_PAGE 离场——窗必须覆盖在当前页之上（原页仍在"
        "场, 关窗回原页）");

    /* tooth: 幂等去重（重复 push 同层保持单条目）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_CHAR_WINDOW) == 0 &&
                                 kidpal_layer_top(&st) == KIDPAL_LAYER_CHAR_WINDOW,
        "M2K3-A1 RED: 查询窗层重复 push 非幂等——层栈会叠屏");

    /* tooth: 关窗（pop）后回原页——top = LOCAL_PAGE、不翻页（s_local_page 不被本层动,
     * 栈级形态 = LOCAL_PAGE 原样仍在场）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_pop(&st, KIDPAL_LAYER_CHAR_WINDOW) == 0 &&
                                 kidpal_layer_top(&st) == KIDPAL_LAYER_LOCAL_PAGE,
        cwx_fm("M2K3-A1 RED: 查字窗出栈后 top=%d 期望 LOCAL_PAGE(%d)——关窗未回原页 "
               "（v1.36 关窗回原页、不翻页）", (int)kidpal_layer_top(&st),
               (int)KIDPAL_LAYER_LOCAL_PAGE));

    /* ---- pop_transients 纳查字窗层（关窗 = 清 transient 回基层唯一事实源）------- */
    /* tooth: 纯基层 + 查字窗 -> 出 1 层（查字窗是 transient, 关窗出清层回基层）。 */
    memset(&st, 0, sizeof(st));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_LOCAL_PAGE) == 0 &&
                                 kidpal_layer_push(&st, KIDPAL_LAYER_CHAR_WINDOW) == 0,
        "M2K3-A1 RED(infra): fixture 建栈失败（LOCAL_PAGE + CHAR_WINDOW）");
    popped = kidpal_layer_pop_transients(&st);
    TEST_ASSERT_TRUE_MESSAGE(popped == 1 &&
                                 kidpal_layer_top(&st) == KIDPAL_LAYER_LOCAL_PAGE &&
                                 kidpal_layer_has(&st, KIDPAL_LAYER_CHAR_WINDOW) == 0,
        cwx_fm("M2K3-A1 RED: [LOCAL_PAGE, CHAR_WINDOW] 清层返回 %d（期望 1, 查字窗层出净）"
               "——查字窗未纳 transient 集（关窗回原页只能靠翻页副作用, v1.39 隔离被绕）",
               popped));

    /* tooth: 满覆盖栈（VOICE + SERVER_PAGE + CHAR_WINDOW + REMINDER）一网打尽 ->
     * 出 4 层、top 回基层（查字窗与提醒/语音/服务端页同为 transient）。 */
    memset(&st, 0, sizeof(st));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_LOCAL_PAGE) == 0 &&
                                 kidpal_layer_push(&st, KIDPAL_LAYER_VOICE) == 0 &&
                                 kidpal_layer_push(&st, KIDPAL_LAYER_SERVER_PAGE) == 0 &&
                                 kidpal_layer_push(&st, KIDPAL_LAYER_CHAR_WINDOW) == 0 &&
                                 kidpal_layer_push(&st, KIDPAL_LAYER_REMINDER) == 0,
        "M2K3-A1 RED(infra): fixture 满覆盖栈建栈失败");
    popped = kidpal_layer_pop_transients(&st);
    TEST_ASSERT_TRUE_MESSAGE(popped == 4,
        cwx_fm("M2K3-A1 RED: 满覆盖栈清层返回 %d 期望 4（VOICE/SERVER_PAGE/CHAR_WINDOW/"
               "REMINDER 各一条）——查字窗层未入 transient 集或幂等性缺失", popped));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_top(&st) == KIDPAL_LAYER_LOCAL_PAGE,
        "M2K3-A1 RED: 满覆盖栈清层后前台未回落基层——关窗回原页栈级机制缺失");

    /* tooth (guard): PROVISION 仍零触碰（配网层不许被查字窗清层误伤——K1a 语义零回归）。 */
    memset(&st, 0, sizeof(st));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_LOCAL_PAGE) == 0 &&
                                 kidpal_layer_push(&st, KIDPAL_LAYER_CHAR_WINDOW) == 0 &&
                                 kidpal_layer_push(&st, KIDPAL_LAYER_PROVISION) == 0,
        "M2K3-A1 RED(infra): fixture 配网栈建栈失败");
    popped = kidpal_layer_pop_transients(&st);
    TEST_ASSERT_TRUE_MESSAGE(popped == 1 &&
                                 kidpal_layer_top(&st) == KIDPAL_LAYER_PROVISION,
        cwx_fm("M2K3-A1 RED: 含 PROVISION 栈清层返回 %d / top=%d 期望 1 / PROVISION——"
               "配网层被查字窗清层步骤误碰（配网按键语义零触碰红线）", popped,
               (int)kidpal_layer_top(&st)));
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — M2K3-A1/A2/A4 结构半：闭集插入 + 清层集 + 覆盖层闭集 + dispatch 分流
 * 树扫 kidpal_layer.h/.c（层闭集 + transient 集）、kidpal_render_node.cc
 * （front_is_overlay 覆盖层闭集 + CHAR_WINDOW 入栈点）、板级 dispatch（喂前台层值 +
 * CHAR_REPLAY 消费映射 replay）。
 * RED today: KIDPAL_LAYER_CHAR_WINDOW 全仓零命中；board cc 零 front_layer 消费。
 * ------------------------------------------------------------------------ */
/* coverage: M2K3-A1 -- 层注册结构半: kidpal_layer.h 闭集插入 CHAR_WINDOW + pop_transients transient 集纳入 CHAR_WINDOW + 查字窗入栈消费点在世 */
/* coverage: M2K3-A2 -- 键位语义结构半: 板级 dispatch 喂前台层值进 key_route_layer + GLOBAL_ACT_CHAR_REPLAY 消费映射 kidpal_char_page_replay */
/* coverage: M2K3-A4 -- 页点隐藏结构半: kidpal_render_node_front_is_overlay 覆盖层闭集纳入 CHAR_WINDOW（页点隐藏与键路由同源） */
TEST_CASE("查字窗层注册: 闭集插入+清层集+覆盖层闭集+dispatch分流 (M2K3-A1/A2/A4 结构半)",
          "[char_window_overlay][M2K3-A1][M2K3-A2][M2K3-A4][structural]")
{
    char *region;

    CWX_REQUIRE_ROOT();

    /* tooth 1 (A1): kidpal_layer.h 常量闭集含 KIDPAL_LAYER_CHAR_WINDOW（注释剥离后计数
     * ——头注清单也提到该符号, 剥注释后只认 #define 实体, 防注释假绿）。 */
    TEST_ASSERT_TRUE_MESSAGE(cwx_file_code_contains(CWX_LAYER_H, "KIDPAL_LAYER_CHAR_WINDOW"),
        "M2K3-A1 RED: kidpal_layer.h 未定义 KIDPAL_LAYER_CHAR_WINDOW——查字窗层未进"
        "层闭集（A1 明文要求新增查字窗层进 kidpal_layer 栈模型）");

    /* tooth 2 (A1): pop_transients transient 集纳入 CHAR_WINDOW（关窗 = 清 transient 回基层）。 */
    region = cwx_region_between(CWX_LAYER_C,
                                "int kidpal_layer_pop_transients(kidpal_layer_stack_t *st)",
                                "__cwx_no_such_end_anchor__");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "M2K3-A1 RED(infra): kidpal_layer.c 的 pop_transients 函数区切取失败（签名锚漂移"
        "——须复核本齿锚, 不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(cwx_count_in_slice_code(region, "KIDPAL_LAYER_CHAR_WINDOW") >= 1,
        "M2K3-A1 RED: kidpal_layer_pop_transients transient 集未纳 KIDPAL_LAYER_CHAR_WINDOW"
        "——查字窗不是清层对象, 关窗无法经显式清层步骤回基层（KD-A2 清层事实源断）");
    free(region);

    /* tooth 3 (A4): front_is_overlay 覆盖层闭集纳入 CHAR_WINDOW（页点隐藏 + 键分流同源）。 */
    region = cwx_region_between(CWX_RN_CC,
                                "int kidpal_render_node_front_is_overlay(void)",
                                "int kidpal_render_node_front_is_voice(void)");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "M2K3-A4 RED(infra): kidpal_render_node.cc 的 front_is_overlay 函数区切取失败"
        "（签名锚漂移——须复核本齿锚）");
    TEST_ASSERT_TRUE_MESSAGE(cwx_count_in_slice_code(region, "KIDPAL_LAYER_CHAR_WINDOW") >= 1,
        "M2K3-A4 RED: front_is_overlay 覆盖层闭集未纳 KIDPAL_LAYER_CHAR_WINDOW——查字窗"
        "前台时前台归一化不报覆盖层（页点不隐藏 + 键路由不自查字窗列分流; §2.2 覆盖层"
        "不显示页点）");
    free(region);

    /* tooth 4 (A1): 查字窗层存在设备侧入栈消费点（覆盖窗显示时机 = 实现侧，非常驻页）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_count_in_file_code(CWX_RN_CC, "KIDPAL_LAYER_CHAR_WINDOW") >= 1 ||
            cwx_count_in_file_code(CWX_MCP_CC, "KIDPAL_LAYER_CHAR_WINDOW") >= 1,
        "M2K3-A1 RED: 渲染节点 / mcp_server 均零 KIDPAL_LAYER_CHAR_WINDOW 消费——查字窗"
        "层只有声明没有入栈点（覆盖窗显示时机未接, 窗退化为常驻页）");

    /* tooth 5 (A2): 板级 dispatch 把前台层值喂进 key_route_layer（查字窗列的入参前提）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_file_code_contains(CWX_BOARD_CC, "kidpal_render_node_front_layer("),
        "M2K3-A2 RED: 板级 dispatch 未消费 kidpal_render_node_front_layer——layer-aware "
        "路由表拿不到前台层身份（查字窗列永不被选中, KEY 短按仍走 SESSION_TOGGLE）");
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_count_in_file_code(CWX_BOARD_CC, "key_route_layer(") >= 1,
        "M2K3-A2 RED: 板级 dispatch 未消费 key_route_layer——分层路由表被绕开");

    /* tooth 6 (A2): GLOBAL_ACT_CHAR_REPLAY 被消费映射 kidpal_char_page_replay（重播接线）。 */
    TEST_ASSERT_TRUE_MESSAGE(cwx_file_code_contains(CWX_BOARD_CC, "GLOBAL_ACT_CHAR_REPLAY"),
        "M2K3-A2 RED: 板级 dispatch 未消费 GLOBAL_ACT_CHAR_REPLAY——查字窗 KEY 短按重播"
        "动作无落点（S-F3.3 AC1 重播未接）");
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_count_in_file_code(CWX_BOARD_CC, "kidpal_char_page_replay(") >= 1 ||
            cwx_count_in_file_code(CWX_MCP_CC, "kidpal_char_page_replay(") >= 1,
        "M2K3-A2 RED: GLOBAL_ACT_CHAR_REPLAY 未映射 kidpal_char_page_replay——重播动作"
        "不触达查字页重播实现（S-F3.3 AC1）");
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — M2K3-A2 键位语义矩阵：key_route_layer 查字窗列（behavioral，真实现源
 * key_gesture_parser.cc 已在册）
 * 查字窗前台列逐格: KEY 短按 = CHAR_REPLAY（非 SESSION_TOGGLE）、KEY 长按 = OVERLAY_CLOSE、
 * BOOT 单击 = OVERLAY_CLOSE（非 PAGE_NEXT）、其余手势 NONE；通用覆盖层列/基层列零回归；
 * 越界键位零污染。RED today: key_route_layer 无查字窗列（CHAR_WINDOW 值落通用覆盖层列,
 * KEY 短按 -> SESSION_TOGGLE, 本齿咬住）。
 * ------------------------------------------------------------------------ */
/* coverage: M2K3-A2 -- 键位语义矩阵行为半：查字窗列 KEY短按=CHAR_REPLAY / KEY长按=OVERLAY_CLOSE / BOOT单击=OVERLAY_CLOSE(非PAGE_NEXT) / 其余NONE + 通用覆盖层列与基层列零回归 + 越界键位防御 */
TEST_CASE("查字窗键位语义: key_route_layer 查字窗列——重播/关窗/不翻页 (M2K3-A2)",
          "[char_window_overlay][M2K3-A2][route]")
{
    int act;

    /* ---- 查字窗列（overlay_top == KIDPAL_LAYER_CHAR_WINDOW）----------------- */

    /* tooth 1 (本包红齿): KEY 单击 -> CHAR_REPLAY（§2.3 查字窗行 KEY 短按 = 重播笔顺）。
     * 判别式 != SESSION_TOGGLE 独立咬住「塌回通用覆盖层列」的等价假绿。 */
    act = (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_CLICK, KIDPAL_LAYER_CHAR_WINDOW);
    TEST_ASSERT_TRUE_MESSAGE(act == CWX_CHAR_REPLAY_VALUE,
        cwx_fm("M2K3-A2 RED: 查字窗前台 KEY 单击路由 = %d 期望 GLOBAL_ACT_CHAR_REPLAY(=%d)"
               "——查字窗列未落（KEY 短按未接重播, S-F3.3 AC1）", act,
               CWX_CHAR_REPLAY_VALUE));
    TEST_ASSERT_TRUE_MESSAGE(act != (int)GLOBAL_ACT_SESSION_TOGGLE,
        "M2K3-A2 RED: 查字窗前台 KEY 单击仍路由 SESSION_TOGGLE——查字窗 KEY 短按被会话"
        "主轴吞掉（重播未接, §2.3 查字窗行）");

    /* tooth 2: KEY 长按 -> OVERLAY_CLOSE（关窗回原页）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS,
                             KIDPAL_LAYER_CHAR_WINDOW) == CWX_OVERLAY_CLOSE_VALUE,
        "M2K3-A2 RED: 查字窗前台 KEY 长按未路由 OVERLAY_CLOSE——关窗回原页未接"
        "（§2.3 查字窗行 KEY 长按 = 关窗回原页）");

    /* tooth 3: BOOT 单击 -> OVERLAY_CLOSE 且 != PAGE_NEXT（v1.39 隔离口径）。 */
    act = (int)key_route_layer(KEY_ID_BOOT, KEY_GESTURE_CLICK, KIDPAL_LAYER_CHAR_WINDOW);
    TEST_ASSERT_TRUE_MESSAGE(act == CWX_OVERLAY_CLOSE_VALUE &&
                                 act != (int)GLOBAL_ACT_PAGE_NEXT,
        cwx_fm("M2K3-A2 RED: 查字窗前台 BOOT 单击路由 = %d 期望 OVERLAY_CLOSE(=%d) 且 "
               "!= PAGE_NEXT(=%d)——覆盖层内 BOOT 隔离未接（窗内 BOOT 必须关窗回原页、"
               "不翻页, v1.39）", act, CWX_OVERLAY_CLOSE_VALUE,
               (int)GLOBAL_ACT_PAGE_NEXT));

    /* tooth 4: 其余手势 NONE（双击族 / BOOT 长按不吞不翻）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK,
                             KIDPAL_LAYER_CHAR_WINDOW) == (int)GLOBAL_ACT_NONE &&
            (int)key_route_layer(KEY_ID_BOOT, KEY_GESTURE_DOUBLE_CLICK,
                                 KIDPAL_LAYER_CHAR_WINDOW) == (int)GLOBAL_ACT_NONE &&
            (int)key_route_layer(KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS,
                                 KIDPAL_LAYER_CHAR_WINDOW) == (int)GLOBAL_ACT_NONE,
        "M2K3-A2 RED: 查字窗列双击族/BOOT 长按出现非 NONE 动作——本包语义面外手势被"
        "查字窗列吞掉（BOOT 长按故事翻屏接缝零触碰）");

    /* ---- 通用覆盖层列零回归（key_route_layer 非查字窗非 0 值 -> K1a 原样）------- */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_CLICK, KIDPAL_LAYER_VOICE) ==
                (int)GLOBAL_ACT_SESSION_TOGGLE &&
            (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS,
                                 KIDPAL_LAYER_SERVER_PAGE) == CWX_OVERLAY_CLOSE_VALUE &&
            (int)key_route_layer(KEY_ID_BOOT, KEY_GESTURE_CLICK,
                                 KIDPAL_LAYER_SERVER_PAGE) == CWX_OVERLAY_CLOSE_VALUE,
        "M2K3-A2 RED: 通用覆盖层列（VOICE/SERVER_PAGE）语义被查字窗列改造误伤——"
        "既有覆盖层路由（KEY 单击 SESSION_TOGGLE / KEY 长按·BOOT 单击 OVERLAY_CLOSE）"
        "偏离 K1a 冻结语义");

    /* ---- 基层列等价（light guard; 全表等价主齿住 key_layer_dispatch.acceptance.c）- */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_CLICK, 0) ==
                (int)key_route_global(KEY_ID_KEY, KEY_GESTURE_CLICK) &&
            (int)key_route_layer(KEY_ID_BOOT, KEY_GESTURE_CLICK, 0) ==
                (int)key_route_global(KEY_ID_BOOT, KEY_GESTURE_CLICK),
        "M2K3-A2 RED: 基层列不再等价 key_route_global——查字窗列改造破坏了主页面路径"
        "（key_route_global 本体零改动红线）");

    /* ---- 越界键位零污染（含 PWR 模拟）--------------------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer((key_id_t)99, KEY_GESTURE_CLICK,
                             KIDPAL_LAYER_CHAR_WINDOW) == (int)GLOBAL_ACT_NONE,
        "M2K3-A2 RED: 查字窗列越界键位未拒收——PWR 模拟键位被查字窗列接受");
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — M2K3-A3 5 分钟驻留计时（behavioral，frozen seam [3]，weak 哨兵）
 * 5 分钟无操作到达上限 -> 该关窗；任意按键 -> 计时归零重计（不关窗）；未到上限 -> 驻留；
 * 防御。RED today: weak sentinel -2 / 模块未落地。
 * ------------------------------------------------------------------------ */
/* coverage: M2K3-A3 -- 驻留计时行为半：5 分钟阈值（299999 驻留 / 300000 关窗）+ 任意按键重计归零 + 持续超时仍关窗 + NULL 防御 + 常量 300000 */
TEST_CASE("查字窗驻留计时: 5 分钟无操作关窗+任意按键重计 (M2K3-A3)",
          "[char_window_overlay][M2K3-A3][timer]")
{
    unsigned long idle;
    int r;

    /* 常量锚（v1.43 兜底 = 5 分钟）。 */
    TEST_ASSERT_TRUE_MESSAGE(KIDPAL_CHAR_WINDOW_IDLE_MS == 300000UL,
        cwx_fm("M2K3-A3 RED: KIDPAL_CHAR_WINDOW_IDLE_MS = %lu 期望 300000——驻留上限"
               "不是 5 分钟（v1.43 兜底口径）", KIDPAL_CHAR_WINDOW_IDLE_MS));

    /* tooth 1: 未到上限 -> 驻留（累计推进, 不提前关窗）。 */
    idle = 0;
    r = kidpal_char_window_idle_step(&idle, 1000UL, 0);
    cwx_require_implemented(r, "kidpal_char_window_idle_step unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && idle == 1000UL,
        cwx_fm("M2K3-A3 RED: 首拍 (dt=1000, 无键) 返回 %d / idle=%lu 期望 0 / 1000——"
               "计时未累计推进", r, idle));

    /* tooth 2 (本包红齿): 恰好到达 5 分钟 -> 该关窗（回原页、不翻页）。 */
    idle = 299999UL;
    r = kidpal_char_window_idle_step(&idle, 1UL, 0);
    TEST_ASSERT_TRUE_MESSAGE(r == 1 && idle == 300000UL,
        cwx_fm("M2K3-A3 RED: idle 到 300000 返回 %d / idle=%lu 期望 1 / 300000——5 分钟"
               "无操作未自动关窗（v1.43 兜底缺失, §2.2 行 68）", r, idle));

    /* tooth 3: 持续超时仍关窗（幂等不复活）。 */
    r = kidpal_char_window_idle_step(&idle, 1000UL, 0);
    TEST_ASSERT_TRUE_MESSAGE(r == 1,
        cwx_fm("M2K3-A3 RED: 超时后继续累计返回 %d 期望 1——超时态非幂等（窗会复活）", r));

    /* tooth 4 (本包红齿): 任意按键 -> 计时归零重计（按键本身是操作, 不关窗）。 */
    idle = 299000UL;
    r = kidpal_char_window_idle_step(&idle, 1000UL, 1);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && idle == 0UL,
        cwx_fm("M2K3-A3 RED: 按键拍返回 %d / idle=%lu 期望 0 / 0——计时未随任意按键重计"
               "（v1.43「计时随任意按键重计」缺失）", r, idle));

    /* tooth 5: 归零后重新累计（重计不是一次性）。 */
    r = kidpal_char_window_idle_step(&idle, 299999UL, 0);
    TEST_ASSERT_TRUE_MESSAGE(r == 0 && idle == 299999UL,
        cwx_fm("M2K3-A3 RED: 重计后累计返回 %d / idle=%lu 期望 0 / 299999——重计后计时"
               "仍按 5 分钟上限走", r, idle));

    /* tooth 6: 越界防御（NULL -> -1, 从不为 UB）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_window_idle_step(NULL, 1000UL, 0) == -1,
        "M2K3-A3 RED: NULL idle 指针未防御 (-1)——驻留计时把空指针当合法");
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — M2K3-A3/A5/A6 结构半：查字窗显示路径零会话 + 右栏对账三项 + 锚改名 +
 * K2 遗留收拢（structural; char_page.cc / mcp_server.cc / kidpal_render_node.{h,cc}
 * 均为设备装配面, 非 host 编译——源级树扫承载）。
 * RED today: 部首未上屏（snprintf 只三字段）/ progress_label 死成员在世 / 头注虚标
 * 「进度珠」/ charpage_draw_penhand + CHARPAGE_PRESS_SIZE 在世 / char_page 头注
 * 「P4 页」/ cache_char 两 API 声明定义调用全在世。
 * ------------------------------------------------------------------------ */
/* coverage: M2K3-A3 -- 播完不回聆听态结构半: 查字窗显示路径零会话/聆听入口（无 ToggleChatState/show_chat/voice_begin） */
/* coverage: M2K3-A5 -- 右栏对账三项结构半: 部首行补上屏（snprintf 纳入 card.radical）+ 进度珠死成员清零 + 头注「进度珠」虚标清除 + 握笔小手圆环/常量清零 */
/* coverage: M2K3-A6 -- 锚改名 + K2 遗留收拢结构半: char-window 锚在世 / 「P4 页」自述清零 / cache_char 两 API 声明定义调用整体收拢 + mcp view=char 路由本体零触碰守卫 */
/* coverage: S-F3.3 AC2 (sourceRef docs/requirements.md#S-F3.3) -- 演示写完定格文案上屏: char_page.cc 呈现面消费 kidpal_char_card_build 的 progress 单源（「写完啦·共 N 笔」——acceptance-author 判定纳入, 见报告「S-F3.3 AC2 判定」段）+ test 6 数据面文案零回归守卫 */
TEST_CASE("查字窗对账: 显示路径零会话+右栏三项+锚改名+cache_char收拢 (M2K3-A3/A5/A6)",
          "[char_window_overlay][M2K3-A3][M2K3-A5][M2K3-A6][structural]")
{
    char *region;

    CWX_REQUIRE_ROOT();

    /* ---- A3 结构半: 查字窗显示路径零会话/聆听入口（播完不回聆听态）----------- */
    region = cwx_region_between(CWX_MCP_CC, "case 3: { /* VRT_ROUTE_CHAR",
                                "case VRT_ROUTE_STORY_LIST:");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "M2K3-A3 RED(infra): mcp_server.cc 的 case 3（VRT_ROUTE_CHAR）路由块切取失败"
        "（块锚漂移——char 路由疑似被搬动或删除）");
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_count_in_slice(region, "ToggleChatState") == 0 &&
            cwx_count_in_slice(region, "kidpal_render_node_show_chat") == 0 &&
            cwx_count_in_slice(region, "kidpal_render_node_voice_begin") == 0,
        "M2K3-A3 RED: 查字窗显示路径出现会话/聆听入口（ToggleChatState / show_chat / "
        "voice_begin）——「播完会话即结束、不回聆听态」被破坏（§2.2 行 68 / F3 窗口"
        "交互包 v1.37: 麦克风不常开、语音轮演出不抢屏）");

    /* ---- A6 guard: mcp view=char 路由本体零触碰（E2 服务端下发语义不动）------- */
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_count_in_slice_code(region,
                           "cJSON_AddStringToObject(ack, \"route\", \"char\")") >= 1 &&
            cwx_count_in_slice_code(region, "kidpal_char_data_validate(") >= 1 &&
            cwx_count_in_slice_code(region, "kidpal_char_page_show(") >= 1,
        "M2K3-A6 RED: case 3 区内的 view=char 路由本体（ack route=char / 缺字段降级门 / "
        "char_page_show 调用点）被拆——E2 铁律明令**不得顺手改动 view=char 路由本体**"
        "（服务端下发语义不动）");

    /* ---- A5a: 部首行补上屏（card.radical 已装配未上屏, S-F3.2 AC1）------------ */
    region = cwx_region_between(CWX_CHAR_CC, "kidpal_char_card_build(&info_o",
                                "lv_obj_set_style_text_font(cardlabel");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "M2K3-A5 RED(infra): char_page.cc 的速查卡拼装区切取失败（card_build 调用 / "
        "cardlabel 字体锚漂移——须复核本齿锚）");
    /* 注释剥离后计数（sealing pass 收紧）：原裸扫「区内出现 card.radical 字面即过」——
     * 区内**任意注释**提到 card.radical 即可假绿（builder M4 实证: 真删部首行后, snprintf
     * 上方一条携带该字面的说明注释仍让本齿保持绿）。剥 C 块注释与 // 行注释后只认真实消费形态
     * （snprintf 实参 g_page.card.radical）, 注释再也无法满足。 */
    TEST_ASSERT_TRUE_MESSAGE(cwx_count_in_slice_code(region, "card.radical") >= 1,
        "M2K3-A5 RED: 速查卡 label 拼装未纳入 card.radical——部首行装配了未上屏"
        "（S-F3.2 AC1「拼音带声调 + 共 N 画 + 部首标签 + 组词」四行不齐, v1.38 对账③；"
        "本齿注释剥离后计数, 注释无法满足）");
    free(region);

    /* ---- A5b: 进度珠死成员清理 + 头注虚标清除 --------------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(cwx_count_in_file(CWX_CHAR_CC, "progress_label") == 0,
        cwx_fm("M2K3-A5 RED: char_page.cc 仍有 %d 处 progress_label——进度珠死成员"
               "（零赋值）未清理（§8 已砍项 / v1.38 对账① —— 砍除时一并清理）",
               cwx_count_in_file(CWX_CHAR_CC, "progress_label")));
    TEST_ASSERT_TRUE_MESSAGE(cwx_count_in_file(CWX_CHAR_CC, "进度珠") == 0,
        cwx_fm("M2K3-A5 RED: char_page.cc 头注仍有 %d 处「进度珠」虚标——已砍项的"
               "头注自述未清（v1.38 对账①: char_page.cc 头注释虚标）",
               cwx_count_in_file(CWX_CHAR_CC, "进度珠")));

    /* ---- A5c: 握笔小手 12px 圆环占位删除 ------------------------------------- */
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_count_in_file(CWX_CHAR_CC, "charpage_draw_penhand") == 0 &&
            cwx_count_in_file(CWX_CHAR_CC, "CHARPAGE_PRESS_SIZE") == 0,
        cwx_fm("M2K3-A5 RED: char_page.cc 仍有 charpage_draw_penhand %d 处 / "
               "CHARPAGE_PRESS_SIZE %d 处——握笔小手 12px 圆环占位未删除（v1.38 对账②:"
               " 改口径后小手资产不再需要, 当前笔改白衬边）",
               cwx_count_in_file(CWX_CHAR_CC, "charpage_draw_penhand"),
               cwx_count_in_file(CWX_CHAR_CC, "CHARPAGE_PRESS_SIZE")));

    /* ---- S-F3.3 AC2: 演示写完定格文案上屏（acceptance-author 判定纳入; 见报告
     * 「S-F3.3 AC2 判定」段）------------------------------------------------------
     * 需求行 515: Given 演示写完，When 定格，Then 显示「✔ 写完啦 · 共 9 笔」。定点
     * 数据面 = char_stroke.c kidpal_char_card_build 的 out->progress（cur_stroke < 0
     * -> "写完啦·共 N 笔", 已在世且在世齿）; 缺口在**呈现面**——char_page.cc 只拼
     * pinyin/stroke_total/words, 未消费 card.progress（现 hint_label 静态占位
     * 「跟随笔画一笔一画描红」, 无定格态）。本齿钉呈现面消费该单源字段（不钉格式字面,
     * 走同源判别——「写完啦」文案只住 char_stroke.c 契约, 页面不得重抄）。sealing pass
     * 收紧: 原文件级裸扫「出现 card.progress 字面即过」可被任意注释假绿; 剥 C 注释后只认
     * 呈现面对该单源字段的真实消费（charpage_refresh_hint / hint_label 写点）。 */
    TEST_ASSERT_TRUE_MESSAGE(cwx_count_in_file_code(CWX_CHAR_CC, "card.progress") >= 1,
        cwx_fm("M2K3-S-F3.3-AC2 RED: char_page.cc 零 card.progress 消费——演示写完的定格"
               "文案（「写完啦·共 N 笔」单源 = kidpal_char_card_build 的 out->progress）"
               "未上屏（S-F3.3 AC2 缺口: 现 hint_label 静态占位「跟随笔画一笔一画描红」,"
               " 无定格态; requirements 行 515 / WF-F3-01 底部提示条；注释剥离后计数）"));

    /* ---- A6a: 锚改名 canvas-p4 -> char-window（实现侧头注对账）---------------- */
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_count_in_file(CWX_CHAR_CC, "char-window") >= 1 &&
            cwx_count_in_file(CWX_CHAR_CC, "P4 页") == 0,
        cwx_fm("M2K3-A6 RED: char_page.cc 头注零 char-window 锚 / 仍含「P4 页」%d 处——"
               "锚改名对账未落（v1.38 登记 Phase 1 dev 修复项 canvas-p4 -> char-window；"
               "P4 常驻页时代自述须清）", cwx_count_in_file(CWX_CHAR_CC, "P4 页")));
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_count_in_file(CWX_CHAR_H, "char-window") >= 1 &&
            cwx_count_in_file(CWX_CHAR_H, "P4 页") == 0,
        cwx_fm("M2K3-A6 RED: char_page.h 头注零 char-window 锚 / 仍含「P4 页」%d 处——"
               "锚改名对账未落（与 .cc 同批）", cwx_count_in_file(CWX_CHAR_H, "P4 页")));

    /* ---- A6b: K2 遗留写侧死态 API 整体收拢（M2K2-E3「K3 统一收」）------------- */
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_count_in_file(CWX_RN_H, "kidpal_render_node_cache_char") == 0 &&
            cwx_count_in_file(CWX_RN_CC, "kidpal_render_node_cache_char") == 0,
        cwx_fm("M2K3-A6 RED: kidpal_render_node.h/.cc 仍有 kidpal_render_node_cache_char* "
               "%d/%d 处——M2-K2 遗留写侧死态 API 未收拢（M2K2-E3 明注「保留不动、由 "
               "M2-K3 改窗时统一收，禁静默删除」——本包即其承接包）",
               cwx_count_in_file(CWX_RN_H, "kidpal_render_node_cache_char"),
               cwx_count_in_file(CWX_RN_CC, "kidpal_render_node_cache_char")));
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_count_in_file(CWX_MCP_CC, "kidpal_render_node_cache_char") == 0,
        cwx_fm("M2K3-A6 RED: mcp_server.cc 仍有 %d 处 kidpal_render_node_cache_char* 调用点"
               "——死态 API 收拢未同步清调用点（M2K2-E3: 须同步清理 mcp_server.cc 的调用行）",
               cwx_count_in_file(CWX_MCP_CC, "kidpal_render_node_cache_char")));

    /* guard: 收拢后 char 渲染主路径仍在世（死态 API 收拢 ≠ 拆查字能力）。 */
    TEST_ASSERT_TRUE_MESSAGE(cwx_file_code_contains(CWX_MCP_CC, "kidpal_char_page_show(") &&
                                 cwx_exists(CWX_CHAR_CC) && cwx_exists(CWX_CHAR_H),
        "M2K3-A6 RED(guard): 死态 API 收拢连带拆了查字渲染主路径（char_page_show / "
        "char_page.{h,cc}）——收拢只针对写侧死态, 不动查字能力本体");
}

/* -------------------------------------------------------------------------- *
 * TEST 6 — M2K3-A4/A5 行为半守卫（behavioral，真实现源 keybar.c / char_stroke.c
 * 已在册 —— must-stay-green 回归守卫, 钉「查字窗所乘机制零回归」）
 * 页点隐藏机制（覆盖层归一化值 -> visible=0, 隐藏不抹状态）+ 速查卡部首字段装配
 * （card_build 已填 radical, 修复只住 label 拼装 —— 数据面零改动守卫）。
 * ------------------------------------------------------------------------ */
/* coverage: M2K3-A4 -- 页点隐藏行为半守卫: 覆盖层归一化值 -> visible=0 且隐藏不抹状态（查字窗所乘 hiding 机制零回归; 查字窗层进闭集后自动隐藏） */
/* coverage: M2K3-A5 -- 速查卡部首字段装配守卫: kidpal_char_card_build 填 out->radical（数据面已装配, 修复只住 char_page label 拼装） */
TEST_CASE("查字窗对账: 页点隐藏机制+部首字段装配守卫 (M2K3-A4/A5 行为半)",
          "[char_window_overlay][M2K3-A4][M2K3-A5][guard]")
{
    kidpal_keybar_dots_t d;
    kidpal_char_info info;
    kidpal_char_card card;

    /* A4: 覆盖层归一化值（front_is_overlay 输出）-> 页点隐藏, 隐藏不抹机制状态。
     * 查字窗层进 front_is_overlay 闭集后（结构半 test 2 tooth 3）, 窗前台即走本路径
     * 自动隐藏页点（§2.2 comp-keybar「覆盖层不显示页点」）。 */
    memset(&d, 0, sizeof(d));
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_keybar_dots((int)PAGE_P1, 1, &d) == 0 && d.visible == 0 &&
            d.total == (int)PAGE_COUNT && d.active == (int)PAGE_P1,
        cwx_fm("M2K3-A4 RED(guard): 覆盖层归一化值下页点三件套 = (rc=%d, visible=%d, "
               "total=%d, active=%d) 期望 (0, 隐藏, %d, %d)——页点隐藏机制或「隐藏不抹"
               "状态」被查字窗改造误伤（查字窗窗内无页点所乘机制）",
               kidpal_keybar_dots((int)PAGE_P1, 1, &d), d.visible, d.total, d.active,
               (int)PAGE_COUNT, (int)PAGE_P1));

    /* A5 guard: 速查卡数据面装配（radical 有值——v1.38 对账③「card.radical 有值、
     * label 拼装漏」的「有值」半守卫; 修复只住 char_page.cc 的 label 拼装, 不动本函数）。 */
    memset(&card, 0, sizeof(card));
    info.pinyin = "yǒng";
    info.radical = "力";
    info.stroke_count = 9;
    info.words = "勇敢、勇气";
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_card_build(&info, -1, &card) == 0 &&
                                 strcmp(card.radical, "力") == 0 &&
                                 strcmp(card.pinyin, "yǒng") == 0,
        cwx_fm("M2K3-A5 RED(guard): kidpal_char_card_build 未装配 radical（card.radical="
               "\"%s\"）——速查卡数据面字段缺失（部首行补上屏的前提是数据面有值; 本齿"
               "零回归守卫, 修复应只住 char_page label 拼装）", card.radical));

    /* S-F3.3 AC2 guard: 定格 progress 文案单源（cur_stroke < 0 = 演示写完
     * -> 「写完啦·共 N 笔」）。呈现面上屏归 char_page（结构半 test 5）; 本齿守数据面
     * 文案零回归（「✔ 写完啦 · 共 9 笔」的文本半 = 需求行 515）。 */
    TEST_ASSERT_TRUE_MESSAGE(strcmp(card.progress, "写完啦·共 9 笔") == 0,
        cwx_fm("M2K3-S-F3.3-AC2 RED(guard): kidpal_char_card_build(-1) progress=\"%s\" 期望"
               "「写完啦·共 9 笔」——定格文案单源被查字窗改造误伤（S-F3.3 AC2 文本半）",
               card.progress));

    /* guard: 缺字段防御零回归（info 任一 NULL -> -1, 数据面不盲装配）。 */
    info.pinyin = NULL;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_char_card_build(&info, -1, &card) == -1,
        "M2K3-A5 RED(guard): kidpal_char_card_build 缺字段未防御 (-1)——数据面防御"
        "被查字窗改造误伤");
}

/* -------------------------------------------------------------------------- *
 * TEST 7 — boundary：注册序守卫 + 需求/契约 canary + 扫描器自检
 *   M2K3-E1 结构半（本文件 + char_window.c 是否被 builder 接线进 HOST_ACCEPTANCE_SRCS）
 *   M2K3-E2 铁律 canary（requirements 查字窗锚在世 + 契约面 view 白名单含 char）
 *   扫描器自检（结构齿负向必须能红, 防假绿）
 * ------------------------------------------------------------------------ */
/* coverage: M2K3-E1 -- host runner 终跑：本文件+char_window.c 注册进 HOST_ACCEPTANCE_SRCS（置于 smoke 之前）结构守卫 + suite 级判读锚（见文件尾） */
/* coverage: M2K3-E2 -- 需求面零修订 canary：requirements 查字窗锚（§2.2 行 68 查字窗行 / F3 / char-window / 5 分钟）+ 契约面在世且 view 白名单含 char（本包零 spec / 零 contract 触碰） */
TEST_CASE("查字窗: 注册序守卫+需求契约canary+扫描器自检 (M2K3-E1/E2)",
          "[char_window_overlay][M2K3-E1][M2K3-E2][boundary]")
{
    char *mk;
    int n;

    CWX_REQUIRE_ROOT();

    /* tooth 1: 本文件 + char_window.c 接线进 HOST_ACCEPTANCE_SRCS 且排在 host_infra_smoke
     * 之前（smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫）。RED until builder wires.
     * sealing pass 收紧: 剥 CMake `#`-到-行尾注释后扫描——被注释掉的源行（`# "...c"`）
     * 不再能满足接线齿（引号串保留, 长度保持, 相对序判定不受影响）。 */
    mk = cwx_read(CWX_CMAKELISTS, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "CWX RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        char *mk_code = cwx_strip_hash_comments(mk);
        const char *mine = strstr(mk_code, "char_window_overlay.acceptance.c");
        const char *impl = strstr(mk_code, "../../../display/char_window.c");
        const char *smoke = strstr(mk_code, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(mine != NULL,
            "M2K3-E1 RED: char_window_overlay.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS "
            "— 本包冻结验收未进 host 执行面（BUILDER DUTY: 见本文件头注 [7]；注释剥离后"
            "扫描, 被注释掉的源行不算接线）");
        TEST_ASSERT_TRUE_MESSAGE(impl != NULL,
            "M2K3-E1 RED: display/char_window.c 未接线进 HOST_ACCEPTANCE_SRCS — 驻留计时"
            "强符号无处落地（weak 哨兵将持续红, test 4 not-wired）");
        TEST_ASSERT_TRUE_MESSAGE(smoke != NULL,
            "CWX RED(infra): host_infra_smoke.acceptance.c 从 HOST_ACCEPTANCE_SRCS 丢失 "
            "— 包 0 守卫被移除");
        TEST_ASSERT_TRUE_MESSAGE(mine < smoke && impl < smoke,
            "M2K3-E1 RED: 本包两行必须排在 host_infra_smoke.acceptance.c 之前（smoke "
            "末位 = 最先执行, boundary 齿注册序守卫）");
        free(mk_code);
    }
    free(mk);

    /* tooth 2 (M2K3-E2 canary): 需求面查字窗锚在世且未被修订（本包是「需求先行后的
     * 实现对账」, 零 docs 面改动——docsSyncRequired=false）。锚选「撤页改窗后仍然成立」
     * 的稳定句。 */
    TEST_ASSERT_TRUE_MESSAGE(cwx_exists(CWX_REQUIREMENTS),
        "M2K3-E2 RED(infra): docs/requirements.md 丢失——需求面被触碰（零修订红线）");
    TEST_ASSERT_TRUE_MESSAGE(cwx_file_contains(CWX_REQUIREMENTS, "char-window") &&
                                 cwx_file_contains(CWX_REQUIREMENTS, "查字窗"),
        "M2K3-E2 RED: requirements 查字窗锚（char-window / 查字窗）丢失——需求面被修订"
        "（本包零 spec 触碰; 发现冲突走 spec-lane 逃生门）");
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_file_contains(CWX_REQUIREMENTS, "5 分钟无操作自动关窗"),
        "M2K3-E2 RED: requirements「5 分钟无操作自动关窗」锚丢失——驻留兜底口径（v1.43）"
        "被改动");
    TEST_ASSERT_TRUE_MESSAGE(cwx_file_contains(CWX_REQUIREMENTS, "提醒 = 最高层覆盖") ||
                                 cwx_file_contains(CWX_REQUIREMENTS, "最高层覆盖"),
        "M2K3-E2 RED: requirements「最高层覆盖」锚丢失——提醒压过查字窗的层级口径"
        "（§2.2 行 67）被改动");

    /* tooth 3 (M2K3-E2 canary): 契约面在世且 view 白名单含 char（本包零契约触碰）。 */
    TEST_ASSERT_TRUE_MESSAGE(cwx_exists(CWX_CONTRACT),
        "M2K3-E2 RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_file_contains(CWX_CONTRACT,
                          "char/schedule/story_list/story_text/text/home/control"),
        "M2K3-E2 RED: 契约 display view 白名单里的 char 项丢失——view=char 通路被从契约"
        "面撤掉（越包; 查字窗归 M2-K3 且不改契约 E1 view=char）");

    /* tooth 4: 扫描器自检（防假绿: 负向齿必须能红）。 */
    TEST_ASSERT_TRUE_MESSAGE(cwx_exists("firmware/main/key_gesture_parser.cc"),
        "CWX RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(cwx_exists("firmware/main/__cwx_no_such__.cc"),
        "CWX RED(infra): scanner reports a nonexistent file as present");
    n = cwx_count_in_file("firmware/main/key_gesture_parser.cc", "__cwx_no_such_needle__");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "CWX RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    {
        char *region = cwx_region_between("firmware/main/key_gesture_parser.cc",
                                          "global_action_t key_route_layer(",
                                          "__cwx_no_such_end_anchor__");
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            "CWX RED(infra): region slicer cannot slice a known function — 切区器失灵");
        TEST_ASSERT_TRUE_MESSAGE(cwx_count_in_slice(region, "overlay_top") >= 1,
            "CWX RED(infra): region slicer lost a known literal — 切区器计数失灵");
        free(region);
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 8 — M2K3-A3 驻留计时**驱动半**（structural; sealing pass 2026-09-19）
 * seam [3] 只钉了纯逻辑模块 char_window.c；其设备侧**驱动**（喂 dt / 按键归零 / 到点
 * 调 dismiss）原无齿覆盖——模块全仓零活消费者（只登记进设备构建、无人调用）。A7 真机轮
 * 的窗口交互包明确含「5 分钟没动自动关窗」, 驱动不接线则该项在设备上不可能真绿（human
 * 持板轮会被卡住）。本齿钉死设备侧驱动落地面与形态（seam [9]）：kidpal_render_node 既有
 * 1s tick 累计喂 kidpal_char_window_idle_step + 到点复用
 * kidpal_render_node_dismiss_front_overlay 关窗 + 板级任意键手势经
 * kidpal_render_node_char_window_key_hit 归零重计。
 * 全部正向齿走**注释剥离后计数**（sealing pass 收紧, 防注释假绿——见 builder M4 实证）。
 * RED today（设计态, 交 builder 补实现的靶）: RN_CC tick 区零
 * kidpal_char_window_idle_step 消费 / kidpal_render_node_char_window_key_hit 全仓零命中
 * ——驱动未落。**不得为让它绿而把齿改写成愿望式或放宽其它齿。**
 * ------------------------------------------------------------------------ */
/* coverage: M2K3-A3 -- 驻留计时驱动半（seam [9]）: 1s tick 累计喂 kidpal_char_window_idle_step（查字窗前台门控, 引用 KIDPAL_LAYER_CHAR_WINDOW）+ 到点调 kidpal_render_node_dismiss_front_overlay 关窗（回原页不翻页）+ 板级任意键手势经 kidpal_render_node_char_window_key_hit 归零重计 */
TEST_CASE("查字窗驻留计时: 设备侧驱动接线（1s tick 喂 step + 到点 dismiss 关窗 + 按键归零） (M2K3-A3 驱动半)",
          "[char_window_overlay][M2K3-A3][structural]")
{
    char *region;

    CWX_REQUIRE_ROOT();

    /* tooth 1: kidpal_render_node.cc 既有 1s tick（kidpal_render_node_tick）区内喂
     * kidpal_char_window_idle_step——驱动落地面 = 既有 1s 定时器, 不另起 timer。 */
    region = cwx_region_between(CWX_RN_CC, "void kidpal_render_node_tick(void)",
                                "extern \"C\" void kidpal_render_node_cache_schedule");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "M2K3-A3 RED(infra): kidpal_render_node.cc 的 tick 函数区切取失败（签名锚漂移"
        "——须复核本齿锚, 不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_count_in_slice_code(region, "kidpal_char_window_idle_step(") >= 1,
        "M2K3-A3 RED: kidpal_render_node_tick 未消费 kidpal_char_window_idle_step——5 分钟"
        "驻留计时的设备侧驱动未接线（char_window.c 全仓零活消费者, 驻留上限永不被喂 dt, "
        "A7 真机轮「5 分钟自动关窗」项不可能真绿）");

    /* tooth 2: tick 区内的驱动被查字窗前台门控（引用 KIDPAL_LAYER_CHAR_WINDOW）——非查字窗
     * 前台时不得累计/误关（否则窗打开首拍即因历史累计越限而立即关窗）。 */
    TEST_ASSERT_TRUE_MESSAGE(cwx_count_in_slice_code(region, "KIDPAL_LAYER_CHAR_WINDOW") >= 1,
        "M2K3-A3 RED: kidpal_render_node_tick 内的驻留驱动未以 KIDPAL_LAYER_CHAR_WINDOW 门控"
        "——计时无条件累计（非查字窗前台也累计, 窗一开首拍即越限误关）");

    /* tooth 3: 到点复用 kidpal_render_node_dismiss_front_overlay 关窗（关窗唯一入口, 回
     * 原页不翻页, v1.36/v1.39）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_count_in_slice_code(region, "kidpal_render_node_dismiss_front_overlay(") >= 1,
        "M2K3-A3 RED: tick 区内驻留驱动到点未调 kidpal_render_node_dismiss_front_overlay"
        "——5 分钟无操作未经关窗唯一入口关窗（回原页不翻页; v1.36/v1.39 关窗语义）");

    /* tooth 4: 板级任意键手势经 kidpal_render_node_char_window_key_hit 归零重计（v1.43
     * 「计时随任意按键重计」）——RN_H 声明确存在 + 板级 cc 有调用点（KEY/BOOT 手势回调）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_file_code_contains(CWX_RN_H, "kidpal_render_node_char_window_key_hit"),
        "M2K3-A3 RED: kidpal_render_node.h 未声明 kidpal_render_node_char_window_key_hit——"
        "板级任意键归零入口缺失（seam [9]; v1.43 计时随任意按键重计）");
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_count_in_file_code(CWX_BOARD_CC, "kidpal_render_node_char_window_key_hit(") >= 1,
        "M2K3-A3 RED: 板级 KEY/BOOT 手势回调未调 kidpal_render_node_char_window_key_hit——"
        "任意按键不重计驻留计时（v1.43 缺失, 键操作后 5 分钟窗不延长）");

    /* guard（驱动面在世正向）: RN_CC 源（非测试）内确有 kidpal_char_window_idle_step 消费
     * ——模块有真实设备消费者, 不再只登记进设备构建而无人调用。 */
    TEST_ASSERT_TRUE_MESSAGE(
        cwx_count_in_file_code(CWX_RN_CC, "kidpal_char_window_idle_step(") >= 1,
        "M2K3-A3 RED: kidpal_render_node.cc 零 kidpal_char_window_idle_step 消费——驻留计时"
        "纯逻辑模块无设备侧活消费者（seam [3] 只钉模块、驱动缺位）");
    free(region);
}

/* ---------------------------------------------------------------------------
 * 终跑判读（suite 级判读说明, review-packet 呈 supervisor）:
 *   判据 = 相对有效基线零新增红 + 本包新齿全绿。
 *   有效基线 = launch 侧 2026-09-19 @ 210bb6e 现跑 firmware host 面全量：
 *   **46 Tests / 0 Failures / 0 Ignored、OK**（acceptance-author freeze 相位实跑复核，
 *   命令见 Run protocol; 在册红 = 零, 判读纪律 = 新增红才是本包责任）。
 *   本包义务红（设计态, builder 落地后转绿, 不计入「新增红」违约）:
 *     - 本文件 test 1（CHAR_WINDOW 未入闭集优先级序/transient 集——真实缺失行为红）;
 *     - 本文件 test 2（CHAR_WINDOW 零命中 / board 零 front_layer 消费——结构红）;
 *     - 本文件 test 3（key_route_layer 无查字窗列——KEY 单击落 SESSION_TOGGLE 红）;
 *     - 本文件 test 4（char_window.c 未落地 weak 哨兵 -2）;
 *     - 本文件 test 5（部首未上屏 / progress_label + 头注虚标 / penhand 圆环 / P4 页
 *       自述 / cache_char 三处——结构红）;
 *     - 本文件 test 7 tooth 1（注册接线守卫, builder 首步 +1/-0 后转绿）;
 *     - **本文件 test 8（seam [9] 设备侧驱动接线——sealing pass 新增; 封口相位唯一红,
 *       交 builder 补实现; 见「SEALING PASS 2026-09-19」段）**;
 *     - key_layer_dispatch.acceptance.c 新增查字窗列齿（test 1 追加; 见该文件
 *       REVISION LOG 2026-09-19——对未落层现状为设计态红）;
 *     - retract_p4_three_page.acceptance.c test 6 tooth 2/5 翻正齿（见该文件 REVISION
 *       LOG 2026-09-19; 原「cache_char 保留」守卫随本包收拢翻正为「已收」）。
 *   MUST-STAY-GREEN（守卫齿, 现状即绿、绿才是本包做对）:
 *     test 1 下层三值零偏移 + PROVISION 零触碰; test 3 通用覆盖层列/基层列/越界键位;
 *     test 5 mcp view=char 路由本体零触碰 + char 渲染主路径在世; test 6 全部（A4/A5
 *     行为半守卫）; test 7 tooth 2/3/4（需求契约 canary + 扫描器自检）。
 *   builder 接线后期望终跑量级 = 46 + 本文件 8 TEST_CASE + K1a/retract 修订齿增量
 *   （HOST_TEST_CONVENTIONS [C4.4]: 测试数是随套件增长的派生量, 不当硬编码判据, 真判据
 *   = OK + EXIT=0 + 全 PASS）。
 *
 * SEALING PASS 2026-09-19（acceptance-author 封口补钉; tighten-only）:
 *   触发 = builder 如实上报两处验收面欠覆盖（见 pod-events friction scan-miss
 *   2026-09-18T23:24:56）:
 *   ① **A3 驱动半无齿覆盖**: char_window.c 全仓零活消费者 → 追 seam [9] + test 8, 钉死
 *      设备侧驱动落地面（既有 1s tick 喂 step + 查字窗前台门控 + 到点 dismiss 关窗 +
 *      板级任意键经 kidpal_render_node_char_window_key_hit 归零）。**该齿封口相位设计态红**
 *      （驱动未落 = 缺失的真实行为红）, 是交给 builder 补实现的靶; 不为让它绿而放宽。
 *   ② **计数型结构齿强度不足（builder M4 实证假绿）**: 原 A5a 以「切区 + 区内 card.radical
 *      计数 >= 1」实现, 区内**任意注释**提到该字面即可满足——builder 真删部首行后, snprintf
 *      上方一条携带 `card.radical` 的说明注释仍让齿保持绿, 删注释后才转红。同类 `card.progress`
 *      为**文件级**计数、强度更弱。**收紧**: 新增 C/C++ 注释剥离器（cwx_strip_c_comments,
 *      保留字符串字面量体）与 CMake `#` 注释剥离器（cwx_strip_hash_comments）, 把本文件
 *      **全部正向计数/包含齿**改走注释剥离后扫描（cwx_count_in_file_code /
 *      cwx_file_code_contains / cwx_count_in_slice_code）。收紧后真实实现形态仍满足（builder
 *      报告: 真实代码形态已就位——snprintf 实参 g_page.card.radical / charpage_refresh_hint 的
 *      g_page.card.progress 消费）, 故**不应由绿转红**。负向齿（`== 0`）与「头注锚」（A6a
 *      char-window 锚 = 设计上钉注释）**不剥注释**——剥注释会把负向齿放松、把锚齿架空。
 *
 * RED baseline 判读口径（freeze 相位, acceptance-author 2026-09-19 实跑）:
 *   通道① 编译通道（DEV-GOVERNANCE「验收测试纪律」§4 C 类冻结验收实编）:
 *     gcc -std=c11 -Wall -Wextra -fsyntax-only -I firmware/main/test/acceptance/
 *         red-baseline -I firmware/main/test firmware/main/test/
 *         char_window_overlay.acceptance.c
 *     （以及 K1a / retract 两处留痕修订件的同批实编）——编译通道 error 为零。
 *   通道② 执行通道（§3 冻结即冒烟）= 本文件 + 已在册真实现源经边界内脚手架
 *   （firmware/main/test/acceptance/red-baseline 下 unity.h stub + runner.c, 非冻结件）
 *   编成原生 Linux ELF 当场执行; 末行 summary 与逐齿红因见 planner 回填的
 *   redBaselineResult（本报告交命令 + 实测摘要）。
 *   cwd 口径: 结构齿以真实工作树扫描, root 自手持 cwd 定位（仓根 / firmware/main/test/
 *   host_app 均可）。
 * ---------------------------------------------------------------------------
 * A7/A8 deferred（human 持板真机轮; 本文件不承载）:
 *   A7 F3 真机确认轮 = S-F3.1 AC1–AC4（弹窗米字格逐笔演示 / 三态可辨 / 歇 2 秒重播 /
 *   逐笔念笔名 v1.55 口径）+ S-F3.2 AC1 + S-F3.3 AC1–AC2 + 窗口交互包全项（播完不回
 *   聆听态 / 5 分钟自动关窗 / KEY 短按重播长按关窗 / BOOT 关窗不翻页 / 无页点）+ 提醒
 *   弹窗压查字窗层级确认（§2.2 行 67）+ K1 两缺陷 live 复证（查「勇」9 笔完整不崩
 *   无缺笔）。
 *   A8 NFR-PERF-2 live 复测 = 语音应答→查字窗打开实测计时对照 5 秒档（未达升 human 裁,
 *   不预绑优化方案）; 测量口径沿用 2026-09-15 真机实测 ~21.6s 的串口取证形态。
 *   证据落 delivery/changes/dev/260917-m2-k3-char-window-overlay/（沿用 RM-A8 取证形态）。
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-19  initial freeze (acceptance-author, TaskPackage
 *               260917-m2-k3-char-window-overlay). 同批留痕修订两处在世冻结件:
 *               key_layer_dispatch.acceptance.c（追加查字窗列齿, 见该文件 REVISION LOG）、
 *               retract_p4_three_page.acceptance.c（test 6 cache_char 保留守卫随本包
 *               收拢翻正, 见该文件 REVISION LOG）。归档件 firmware/main/test/acceptance/
 *               archive 目录下的 .c 件不编入 runner、不改。
 *   2026-09-19  sealing pass (acceptance-author, same TaskPackage; tighten-only)。触发 =
 *               builder 如实上报两处验收面欠覆盖, planner 判为验收职责面并路由回来:
 *               ① 追 seam [9] + test 8——M2K3-A3 驻留计时**设备侧驱动**（char_window.c
 *                  原全仓零活消费者）。② 收紧计数型结构齿——新增 C/CMake 注释剥离器, 把
 *                  本文件全部正向计数/包含齿改走注释剥离后扫描（防注释假绿; builder M4
 *                  实证: A5a 原裸扫可被注释满足）。**只加齿收紧, 不放松任何既有齿**;
 *                  负向齿与头注锚齿（A6a char-window）不剥注释。另两处在世冻结件
 *                  （key_layer_dispatch / retract_p4_three_page）本轮不动——未实证同类假绿。
 * --------------------------------------------------------------------------- */