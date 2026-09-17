/*
 * Frozen acceptance tests — TaskPackage 260915-m1-k4-reminder-overlay-unify (subsystem:
 * firmware; M1-K4; mergeOrder 26; 提醒弹窗四态统一骨架 + 最高层覆盖 + 录音排队例外 +
 * 排队/去重/驻留/过期口径收口)。
 *
 * Story: S-F1.4 · 日程提醒（AC1–AC6）+ docs/requirements.md §2.2 状态表提醒行
 * （v1.23/v1.24 四态统一裁定 + v1.44 层级口径）+ S-F1.4 补充规则（驻留口径 / 过期口径）。
 * 现状差距（plan.md 背景摘录）: RTC 提醒链路 MVP 已交付（到点能弹），但弹窗形态仍是
 * 作废的旧版式（v1.13 白底描边卡面, kidpal_reminder_page_render 矢量占位形态）、层级
 * 口径（最高层覆盖 + 录音排队例外）未接、骨架未与语音轮四态统一。
 *
 * Nature: HOST-SIDE BEHAVIORAL (纯逻辑决策表, weak 哨兵) + STRUCTURAL (tree-scan)
 * frozen acceptance, key_layer_dispatch.acceptance.c（K1a）+ voice_round_ui.acceptance.c
 * （K3）范式。被测面渲染模型已核（acceptance-author 冻结前实查）: 提醒弹窗是 **LVGL
 * 对象模型**（reminder_page.c 走 lv_screen_active()+lv_obj_clean() 对象树, 经
 * kidpal_render_node_begin_reminder 切活动屏到 s_kidpal_scr）——**不是** home_p1.c 那种
 * 400×300 软帧整帧 blit。故本文件钉对象/入口/接线事实, 不做像素探针; 观感像素归真机轮
 * （RM-A8, human 持板）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements，本文件只读扫工作树 + 弱哨兵
 * 行为级验证; 「home =」为冻结落点）:
 *
 *   [1] 提醒层入层门控决策表（RM-A2/A3 机制单源; home = firmware/main/display/
 *       reminder_page.{h,c}, 纯 C、无 chip 依赖, 强符号落 reminder_page.c）:
 *
 *       -- 冻结数值（与本文件内哨兵声明逐值一致; builder 落 reminder_page.h 时逐值
 *          一致, 偏移即编译冲突 fail-loud）:
 *       #define KRM_GATE_RAISE   1   -- 立即入层弹窗（最高层覆盖, RM-A2）
 *       #define KRM_GATE_DEFER   0   -- 排队延后（录音排队例外, RM-A3）
 *       #define KRM_GATE_INVALID (-1) -- 越界防御
 *
 *       int kidpal_reminder_gate(int front_layer, int recording);
 *         -- 提醒到达时的入层门控: front_layer = 到达时前台 owner（kidpal_layer.h
 *            闭集值, KIDPAL_LAYER_*; 分层裁决 owner = key-layer-dispatch, 本包只消费
 *            其闭集、不改裁决语义——plan.md 铁律）; recording = 录音态在世（0/1）。
 *         -- 冻结表（唯一例外 = 录音态, requirements §2.2 提醒行「唯一例外 = 录音态:
 *            录音期间提醒排队延后、录音会话结束立刻弹」）:
 *           recording != 0 -> KRM_GATE_DEFER（录音排队例外——防提醒 TTS 混进录音内容,
 *                             对任何前台一律延后）;
 *           recording == 0 && front_layer >= 0 -> KRM_GATE_RAISE（最高层覆盖: 基层/
 *                             查字窗/播报中/语音轮/聊天/无前台 一律允许入层弹窗——
 *                             实际压层由 layer 栈优先级承载, REMINDER(5) > VOICE(3)/
 *                             SERVER_PAGE(4); PROVISION(6) 更高属配网通道语义, 门控
 *                             恒允许「尝试入层」, 覆盖与否归 layer 栈裁决）;
 *           front_layer < 0 或 recording 不在 {0,1} -> KRM_GATE_INVALID（从不为 UB）。
 *
 *   [2] 弹窗时间行单源（RM-A1 内容机制; home = reminder_page.{h,c}, 现有 static
 *       krm_hm_of 逻辑的导出, 行为零变化）:
 *       int kidpal_reminder_hhmm(const char *fire_at, char *out, size_t cap);
 *         -- fire_at 字面 11..15 位组 "HH:MM"（孩子看到的是课表里的那个时刻, 不做设备
 *            时区二次换算——现有渲染面注记零改动）:
 *           0 = 组出 HH:MM; 1 = 形状不可读 -> "--:--" 兜底占位（含 NULL fire_at,
 *               明确占位不空白）; -1 = out NULL / cap < 6（"--:--"+NUL 最小 6 字节）
 *               —— 恒 NUL 结尾、绝不溢出。
 *
 *   [3] 状态机回归钉点（RM-A4/A5/A6/A7; 强符号消费——schedule_reminder_b.acceptance.c
 *       已按 2026-09-15 归档纪律移入 acceptance/archive/ 不再随 runner 全量跑, 本包
 *       acceptance 集内的承载义务由本文件重钉; 强定义 home = display/reminder_page.c,
 *       零行为改动——BUILDER 把它接回 runner 即绿, 见 [6]）:
 *       kidpal_reminder_init / receive / raise_next / shown_id / shown_alert /
 *       may_emit_receipt / mark_rendered / mark_acked / render_actions / key 全表语义
 *       以归档冻结件 firmware/main/test/acceptance/archive/schedule_reminder_b.
 *       acceptance.c 文件头 FROZEN OFFLINE CONTRACT 为准（含 KD-A4 v1.39 隔离修订后的
 *       按键表）, 本文件消费其强符号做 RM-A4~A7 的行为级复核, 不缩窄不放宽。
 *
 *   [4] 活侧接线（RM-A1/A3; home = firmware/main/display/lcd_display.cc +
 *       firmware/main/mcp_server.cc + firmware/main/application.cc）:
 *       - 弹窗渲染切骨架（取舍 A: 复用 K3 四态骨架不另建, RM-E3 反 fork）: 提醒弹窗
 *         渲染路径消费 kidpal_voice_apply_look(KVST_SCENE_REMIND)（pose-jump + 键帽
 *         「知道了」从 kidpal_voice_states 闭集单源取出）; 右内容区承载大字事项
 *         （title）+ 时间行（经 [2] kidpal_reminder_hm 单源, 渲染面不重写第二份时间
 *         解析）。旧 v1.13 卡面渲染调用点销账: mcp_server.cc RaiseReminderQueue 不再
 *         调 kidpal_reminder_page_render。
 *       - 三件套（RM-A1）: 弹窗 + 提示音 + 语音读出同时发生——动作集机制已冻结
 *         （kidpal_reminder_render_actions: SHOW|TONE 必达, SPEAK 仅 speech 非空）;
 *         提示音强定义（mcp_server.cc Application::PlaySound(Lang::Sounds::OGG_POPUP)）
 *         零回归守卫。
 *       - 录音排队例外接线（RM-A3）: mcp_server.cc RaiseReminderQueue 消费 [1] 门控
 *         （录音态 -> 不 raise, 队列保持）; application.cc 录音会话结束路径触发
 *         McpServer::RaiseReminderQueue()（「录音会话结束立刻弹」; 触发点路由不锁——
 *         tts stop manual 分支 / 会话级联关闭均可, 钉「触发在世」; 若实现采用
 *         mcp_server 侧查询录音态等其它形态, 须返 planner/acceptance-author 复核返钉,
 *         不得静默改齿）。
 *       - 最高层覆盖 + 不穿透（RM-A2 机制半, 零改动红线守卫）: layer 栈
 *         KIDPAL_LAYER_REMINDER(5) > VOICE/SERVER_PAGE 优先级、板级 dispatch 顶层先交
 *         kidpal_reminder_key 裁决（K1a 落地件）——本包零触碰, 齿只守在世。
 *
 *   [5] Builder FIRST ACTION: 把本验收文件与被测实现接进
 *       firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS:
 *         "../../reminder_overlay_unify.acceptance.c"
 *         "../../../display/reminder_page.c"   （[3] 状态机强符号源; 其 kidpal_art_*
 *           引用由 p1_home_battery.acceptance.c 在世 weak 桩覆盖——与 home_p1.c 同款,
 *           无需另接 art_assets.c）
 *       置于 "../../host_infra_smoke.acceptance.c" 之前（smoke 保持列表末位 = 最先
 *       执行, boundary 齿注册序守卫; 本文件 test 8 tooth 1 校验此序）。
 *
 *   [6] mutation 自证（mutationRequired=true）：变异命名**不得保留被测符号原字面为
 *       子串**（K3/K2 教训——strstr 子串仍命中则变异不红）; mutationEvidence.
 *       perAcceptance 逐条回填, acceptanceId 与 taskpackage acceptance[] 正文逐字节
 *       一致。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: RM-A1 四态统一骨架 + 三件套 -> test 2（机制半: 时间行单源决策 + 三件套
 *             动作集 SHOW|TONE 必达/SPEAK 可选 + 骨架第四成员闭集零偏移复核）+
 *             test 7（结构半: apply_look(KVST_SCENE_REMIND) 骨架单源消费 + 时间行
 *             kidpal_reminder_hhmm 单源消费 + 旧 v1.13 卡面调用点销账 + OGG_POPUP
 *             提示音强定义零回归守卫）; 版式几何/网点/pose 位图观感归真机轮 RM-A8
 *   coverage: RM-A2 最高层覆盖 + 按键不穿透 -> test 1（gate RAISE 列: 非录音前台一律
 *             允许入层）+ test 3（layer 栈 REMINDER 压 VOICE/SERVER_PAGE 行为级 +
 *             板级裁决函数/前台查询消费零回归守卫 + REMINDER 入层通道在世）+
 *             test 6（不穿透行为半: KEY/BOOT 单击 DISMISS、其它手势 PASS 驻留不变）
 *   coverage: RM-A3 录音态排队例外 -> test 1（gate DEFER 列: 录音中对任何前台一律
 *             延后 + 越界防御）+ test 7（结构半: RaiseReminderQueue 消费门控 +
 *             application.cc 录音会话结束触发在世）
 *   coverage: RM-A4 排队不叠屏 -> test 4（弹窗期间新提醒入队不动驻留 + raise 不替换
 *             不叠屏 + KEY 关闭后下一条自动弹出）
 *   coverage: RM-A5 去重不重复弹 -> test 5（排队期同 id 屏蔽入队 + 已渲染驻留重推
 *             补回回执不重弹 + 已 acked 重推零动作 + 关闭后不复活）
 *   coverage: RM-A6 驻留到按键 + KEY/BOOT 语义 -> test 6（无超时驻留 + 键位全表:
 *             KEY 单击回当前页 / BOOT 单击关弹窗回原页不翻页（判别式 != key_page_
 *             next）/ KEY 长按关层 / 其它手势 PASS）+ test 7 tooth 7（drain timer
 *             只弹不关——零自动消失的机制面）
 *   coverage: RM-A7 过期档位不补发 -> test 6（V18 顶替非追加送达: 被顶替项不复活
 *             不重弹 + 队列空不追催）+ test 7 tooth 8（提醒模块零三档时刻字面值 +
 *             零本地 timer——设备零本地事项源, 云侧补发口径归云 RM-E1 零触碰）
 *   coverage: RM-A8 真机确认轮（human 持板, kind=structural 对象条目）-> test 7
 *             （真机前置接缝断言: 骨架/门控/录音结束触发接线全部在世 = 真机轮前置）;
 *             真机证据落 delivery/changes/dev/260915-m1-k4-reminder-overlay-unify/
 *             ——见文件尾「RM-A8 真机证据要求」段, 本文件不造 host 伪断言
 *   coverage: RM-A9 host runner 终跑 -> test 8（注册接线结构守卫）+ 本文件尾
 *             「RM-A9 终跑判读」段（suite 级判读: 相对有效基线零新增红 + 本包新齿全绿,
 *             schedule_reminder_b 新基线）
 *   coverage: RM-E1 云侧零改动 -> 无齿条目（三段阶梯时刻/范围口径/权限 = 云侧数据面,
 *             本包零 server 面改动; 零回归由红基线记录承载——见尾注记）
 *   coverage: RM-E2 需求面零修订 -> test 8 tooth 2（requirements 提醒行三锚 + 契约面
 *             canary）
 *   coverage: RM-E3 骨架复用纪律（不 fork）-> test 7 tooth 1（apply_look 单源消费——
 *             提醒态渲染不得绕开 K3 建立的骨架单源另立第二渲染源）
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   0) builder wiring 见 [5]。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build   （git-bash 下 MSYS_NO_PATHCONV=1 必加, 否则 /mnt/c 路径
 *      被改写; idf.py 无 -T 选项, host_app 是独立工程直接 build）
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/
 *      host_infra_test.elf   （仓根 cwd 运行; 结构齿以 krmu_locate_root 自手持 cwd
 *      定位仓根）
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
#include <assert.h>
#include <stdarg.h>

/* 提醒消费面冻结契约头（[3] 状态机强符号声明 + KRM_STATE 与 KRM_KEY、KRM_ACT、KRM_CAP
 * 冻结枚举与容量; 强符号在 builder 接线 reminder_page.c 后覆盖本文件 weak 哨兵）。
 * 相对路径引用, host runner 与独立 stub 编译都可达（K1a/K3 先例同款 include）。 */
#include "../display/reminder_page.h"
/* layer 闭集（[1] front_layer 入参 + test 3 layer 栈行为半; 强符号已入 runner）。 */
#include "../display/kidpal_layer.h"
/* 键路由共享底座头（test 6 key_page_next 判别式消费; K1a/K2/K3 先例同款 include）。 */
#include "../key_gesture_parser.h"
/* 语音轮四态闭集契约头（test 2 骨架第四成员零偏移复核; 强符号已入 runner, K2 接线）。 */
#include "../display/kidpal_voice_states.h"

#define KRMU_MAX_PATH 1024
#define KRMU_MAX_READ (8u * 1024u * 1024u)

/* 被扫文件（结构齿锚, 相对仓根）。 */
#define KRMU_LCD_CC         "firmware/main/display/lcd_display.cc"
#define KRMU_MCP_CC         "firmware/main/mcp_server.cc"
#define KRMU_APP_CC         "firmware/main/application.cc"
#define KRMU_REM_C          "firmware/main/display/reminder_page.c"
#define KRMU_REM_H          "firmware/main/display/reminder_page.h"
#define KRMU_RENDER_NODE_H  "firmware/main/display/kidpal_render_node.h"
#define KRMU_RENDER_NODE_CC "firmware/main/display/kidpal_render_node.cc"
#define KRMU_BOARD_CC       "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"
#define KRMU_CMAKELISTS     "firmware/main/test/host_app/main/CMakeLists.txt"
#define KRMU_REQUIREMENTS   "docs/requirements.md"
#define KRMU_CONTRACT       "docs/contracts/interface-contract.md"

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — weak sentinels（强定义落地后覆盖）。
 * kidpal_reminder_gate / kidpal_reminder_hhmm 强定义 home = display/reminder_page.c
 * （[1]/[2], builder 落地 + 接线后覆盖）; [3] 状态机九符号强定义已在
 * display/reminder_page.c（builder 把该源接回 runner 即覆盖, 见 [5]）。
 * 落地前哨兵返回 KRMU_UNIMPLEMENTED：行为齿先断「非哨兵」，红基线读作
 * 「frozen contract not implemented / not wired」——真实缺失行为红，绝非链接错误。
 * KRM_GATE_* 冻结数值在本文件内自持声明（冻结期 reminder_page.h 尚未落地新符号, C 类
 * 冻结验收实编纪律）; builder 落 reminder_page.h 后须逐值一致（偏移即编译冲突）。
 * ------------------------------------------------------------------------ */
#define KRMU_UNIMPLEMENTED (-2)

#define KRM_GATE_RAISE   1
#define KRM_GATE_DEFER   0
#define KRM_GATE_INVALID (-1)

int kidpal_reminder_gate(int front_layer, int recording);
int kidpal_reminder_hhmm(const char *fire_at, char *out, size_t cap);

__attribute__((weak)) int kidpal_reminder_gate(int front_layer, int recording)
{
    (void)front_layer; (void)recording;
    return KRMU_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_reminder_hhmm(const char *fire_at, char *out, size_t cap)
{
    (void)fire_at; (void)out; (void)cap;
    return KRMU_UNIMPLEMENTED;
}
__attribute__((weak)) void kidpal_reminder_init(kidpal_reminder_t *s)
{
    (void)s;
}
__attribute__((weak)) int kidpal_reminder_receive(kidpal_reminder_t *s, const kidpal_alert_t *a)
{
    (void)s; (void)a;
    return KRMU_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_reminder_raise_next(kidpal_reminder_t *s, long long now_min)
{
    (void)s; (void)now_min;
    return KRMU_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_reminder_shown_id(const kidpal_reminder_t *s, char *out, size_t cap)
{
    (void)s; (void)out; (void)cap;
    return KRMU_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_reminder_shown_alert(const kidpal_reminder_t *s, kidpal_alert_t *out)
{
    (void)s; (void)out;
    return KRMU_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_reminder_may_emit_receipt(const kidpal_reminder_t *s)
{
    (void)s;
    return KRMU_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_reminder_mark_rendered(kidpal_reminder_t *s, const char *id)
{
    (void)s; (void)id;
    return KRMU_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_reminder_mark_acked(kidpal_reminder_t *s, const char *id)
{
    (void)s; (void)id;
    return KRMU_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_reminder_render_actions(const kidpal_alert_t *a)
{
    (void)a;
    return KRMU_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_reminder_key(kidpal_reminder_t *s, key_id_t key, key_gesture_t g,
                                              page_id_t current, page_id_t *page_out)
{
    (void)s; (void)key; (void)g; (void)current; (void)page_out;
    return KRMU_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * 状态实例裸缓冲（4096 字节, 归档冻结件 RMD-A5/A6 齿纪律——kidpal_reminder_t 不透明、
 * reminder_page.c 内 _Static_assert 钉其 <= 4096 字节, host 侧用裸缓冲承载实例 = 定长
 * 有界 RAM 契约的机判形态; _Alignas 保证对齐不低于该结构所需）。
 * ------------------------------------------------------------------------ */
static _Alignas(16) unsigned char krmu_state_buf[4096];

/* fixture: 清零裸缓冲 + init（re-init 遗忘既往; weak 阶段 init 为 no-op, 清零已由
 * memset 兜底, 强定义落地后 init 做同一件事——双保险不改变语义）。 */
static kidpal_reminder_t *krmu_fresh(void)
{
    kidpal_reminder_t *s = (kidpal_reminder_t *)krmu_state_buf;

    memset(krmu_state_buf, 0, sizeof(krmu_state_buf));
    kidpal_reminder_init(s);
    return s;
}

/* 载荷构造 helper（字段公开, 归档冻结件同款; 不经 parse——parse 面归归档件, 本文件
 * 状态机齿直接构造合法载荷）。 */
static void krmu_make_alert(kidpal_alert_t *a, const char *id, const char *fire_at,
                            const char *title, const char *speech)
{
    memset(a, 0, sizeof(*a));
    snprintf(a->id, sizeof(a->id), "%s", id);
    snprintf(a->fire_at, sizeof(a->fire_at), "%s", fire_at);
    snprintf(a->title, sizeof(a->title), "%s", title);
    snprintf(a->speech, sizeof(a->speech), "%s", speech);
}

/* 冻结参照时刻（归档冻结件 fire_at 分钟数参考点单源复用, 2026-09-09 +08:00）:
 *   07:45 -> 29815185 / 16:15 -> 29815695 / 19:00 -> 29815860 */
#define KRMU_EV_1615 29815695LL
#define KRMU_EV_1900 29815860LL

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（host 侧结构齿, key_layer_dispatch / voice_round_ui
 * 先例）。全部扫描跑在真实工作树上, root 以手持 cwd 定位（从仓根或
 * firmware/main/test/host_app 两处都可跑）。test / host_app / build / .git / assets
 * 目录排除——test 目录里住着本冻结文件自身（其注释合法提及冻结 needle）。
 * ------------------------------------------------------------------------ */

static char krmu_root[KRMU_MAX_PATH];
static int krmu_root_ready;

static int krmu_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int krmu_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int krmu_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int krmu_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KRMU_MAX_PATH];

    if (krmu_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (krmu_is_regular(probe)) {
            snprintf(krmu_root, sizeof(krmu_root), "%s", cands[i]);
            krmu_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int krmu_rel(char *out, size_t cap, const char *rel)
{
    if (!krmu_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", krmu_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", krmu_root, rel);
    return 1;
}

static int krmu_exists(const char *rel)
{
    char p[KRMU_MAX_PATH];
    return krmu_rel(p, sizeof(p), rel) && krmu_is_regular(p);
}

static char *krmu_read(const char *rel, size_t *out_len)
{
    char p[KRMU_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!krmu_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KRMU_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int krmu_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int krmu_file_contains(const char *rel, const char *needle)
{
    char *buf = krmu_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = krmu_contains(buf, needle);
    free(buf);
    return r;
}

/* Occurrence count of needle inside one file (0 when unreadable/absent). */
static int krmu_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = krmu_read(rel, NULL);
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
 * occurrence of end_needle AFTER it. Returns a malloc'd NUL-terminated slice or NULL
 * （unreadable file / start needle absent）。Caller frees。 */
static char *krmu_region_between(const char *rel, const char *start_needle,
                                 const char *end_needle)
{
    char *buf = krmu_read(rel, NULL);
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

static int krmu_count_in_slice(const char *slice, const char *needle)
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

#define KRMU_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(krmu_locate_root(), \
    "KRMU RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* Shared red-string for the frozen seam not being implemented yet. */
static void krmu_require_implemented(int value, const char *what)
{
    if (value == KRMU_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "KRMU RED: frozen seam NOT implemented — %s "
                 "(builder must land the seam contract in "
                 "firmware/main/test/reminder_overlay_unify.acceptance.c header and wire "
                 "reminder_page.c + this file into HOST_ACCEPTANCE_SRCS)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* printf-style helper: dynamic diagnostics pre-formatted into a static buffer then
 * passed to the single-string assert macros（KLD/KVR 先例）。 */
static char krmu_fm_buf[1024];
static const char *krmu_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *krmu_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(krmu_fm_buf, sizeof(krmu_fm_buf), fmt, ap);
    va_end(ap);
    return krmu_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — RM-A2/A3 提醒层入层门控决策表（behavioral, frozen seam [1]）
 * 非录音前台一律允许入层（最高层覆盖）; 录音态对任何前台一律延后（唯一例外——防提醒
 * TTS 混进录音内容）; 越界防御。
 * RED today: weak sentinel -2。
 * ------------------------------------------------------------------------ */
/* coverage: RM-A2 -- 最高层覆盖机制齿：gate 非录音列恒 RAISE（基层/查字窗/播报中/语音轮/聊天/无前台 一律允许入层弹窗） */
/* coverage: RM-A3 -- 录音态排队例外机制齿：gate 录音列恒 DEFER（唯一例外, 对任何前台一律延后——防提醒 TTS 混进录音内容） */
TEST_CASE("提醒层入层门控: kidpal_reminder_gate 决策表——最高层覆盖+录音排队例外 (RM-A2/RM-A3)",
          "[reminder_overlay_unify][RM-A2][RM-A3][gate]")
{
    static const int layers[] = {
        KIDPAL_LAYER_NONE, KIDPAL_LAYER_CHAT, KIDPAL_LAYER_LOCAL_PAGE,
        KIDPAL_LAYER_VOICE, KIDPAL_LAYER_SERVER_PAGE, KIDPAL_LAYER_REMINDER,
        KIDPAL_LAYER_PROVISION
    };
    size_t i;

    /* tooth 1 (RM-A2): 非录音（recording == 0）-> 恒 RAISE——提醒弹出时压过查字窗/
     * 播报中等任何前台内容（§2.2 层级口径「最高层覆盖」; 实际压层由 layer 栈优先级
     * 承载——test 3, 门控只锁「允许入层尝试」这一层决策）。 */
    {
        int first = kidpal_reminder_gate(KIDPAL_LAYER_VOICE, 0);
        krmu_require_implemented(first, "kidpal_reminder_gate unimplemented");
        for (i = 0; i < sizeof(layers) / sizeof(layers[0]); i++) {
            TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_gate(layers[i], 0) == KRM_GATE_RAISE,
                krmu_fm("RM-A2 RED: 非录音前台 layer=%d 门控未放行 RAISE——最高层覆盖"
                        "（弹出时压过查字窗/播报中等任何前台）的入层决策缺失 "
                        "(requirements §2.2 提醒行 v1.44 层级口径)", layers[i]));
        }
    }

    /* tooth 2 (RM-A3): 录音（recording != 0）-> 恒 DEFER——录音期间提醒排队延后,
     * 对任何前台一律延后（唯一例外; 防提醒 TTS 混进录音内容 + 不打断创作流,
     * §2.2 提醒行 2026-09-14 裁）。 */
    for (i = 0; i < sizeof(layers) / sizeof(layers[0]); i++) {
        TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_gate(layers[i], 1) == KRM_GATE_DEFER,
            krmu_fm("RM-A3 RED: 录音前台 layer=%d 门控未延后 DEFER——录音排队例外缺失, "
                    "提醒会打断录音或 TTS 混进录音内容（§2.2 唯一例外口径）", layers[i]));
    }

    /* tooth 3: 越界防御（front_layer < 0 / recording 不在 {0,1} -> INVALID, 从不为
     * UB——门控决策表入参闭集防线）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_reminder_gate(KIDPAL_LAYER_LOCAL_PAGE, -1) == KRM_GATE_INVALID &&
        kidpal_reminder_gate(KIDPAL_LAYER_LOCAL_PAGE, 2) == KRM_GATE_INVALID &&
        kidpal_reminder_gate(-1, 0) == KRM_GATE_INVALID &&
        kidpal_reminder_gate(-1, 1) == KRM_GATE_INVALID,
        "RM-A2 RED: 越界前台/录音入参未防御 (INVALID)——门控决策表防线缺失");
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — RM-A1 弹窗内容机制半（behavioral, frozen seam [2] + 骨架闭集复核）
 * 时间行单源（fire_at 字面 HH:MM / 不可读占位 / 溢出防御）+ 三件套动作集
 * （SHOW|TONE 必达, SPEAK 仅 speech 非空——决策 27/V20）+ 骨架第四成员闭集零偏移
 * 复核（KVST_SCENE_REMIND pose-jump + 键帽「知道了」, K2 闭集）。
 * RED today: hhmm weak sentinel -2（render_actions/闭集为在世强符号——接线后转绿,
 * 哨兵期对现状红 = not wired, 见尾注「义务红形态」）。
 * ------------------------------------------------------------------------ */
/* coverage: RM-A1 -- 四态统一骨架机制半：时间行 kidpal_reminder_hhmm 单源（大字事项+时间的「时间」半）+ 三件套动作集（弹窗+提示音必达, 语音可选增强）+ 骨架第四成员（pose-jump/键帽「知道了」）闭集零偏移复核 */
TEST_CASE("提醒弹窗内容机制: 时间行单源+三件套动作集+骨架第四成员闭集 (RM-A1)",
          "[reminder_overlay_unify][RM-A1][hhmm][actions]")
{
    char buf[16];
    kidpal_alert_t a;
    kidpal_voice_look_t look;

    /* ---- tooth 1 (RM-A1): 时间行单源——fire_at 字面 11..15 位组 "HH:MM"（孩子看到
     * 的是课表里的那个时刻, 不做设备时区二次换算）。 */
    {
        int r = kidpal_reminder_hhmm("2026-09-09T16:15:00+08:00", buf, sizeof(buf));
        krmu_require_implemented(r, "kidpal_reminder_hhmm unimplemented");
        TEST_ASSERT_TRUE_MESSAGE(r == 0 && strcmp(buf, "16:15") == 0,
            krmu_fm("RM-A1 RED: 可解析 fire_at 的 HH:MM 组装错误 (r=%d, out=%s)——"
                    "弹窗时间行不是 fire_at 的字面时分（S-F1.4 AC1 大字事项+时间）",
                    r, buf));
    }
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_reminder_hhmm("2026-12-31T07:45:00+08:00", buf, sizeof(buf)) == 0 &&
        strcmp(buf, "07:45") == 0,
        "RM-A1 RED: 第二参照时刻 HH:MM 组装漂移——时间行单源决策表不全");

    /* tooth 2: 形状不可读 -> "--:--" 兜底占位（明确占位不空白; 含 NULL fire_at）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_reminder_hhmm("not-a-time", buf, sizeof(buf)) == 1 &&
        strcmp(buf, "--:--") == 0,
        "RM-A1 RED: 不可读 fire_at 未落 \"--:--\" 兜底占位——弹窗时间行空白/乱码");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_reminder_hhmm(NULL, buf, sizeof(buf)) == 1 && strcmp(buf, "--:--") == 0,
        "RM-A1 RED: NULL fire_at 未防御为兜底占位——渲染面崩溃/空白风险");

    /* tooth 3: 溢出防御（out NULL / cap < 6 -> -1; 恒 NUL 结尾绝不溢出）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_reminder_hhmm("2026-09-09T16:15:00+08:00", NULL, sizeof(buf)) == -1 &&
        kidpal_reminder_hhmm("2026-09-09T16:15:00+08:00", buf, 5) == -1,
        "RM-A1 RED: out NULL / cap < 6 未防御 (-1)——\"--:--\"+NUL 最小 6 字节契约缺失");

    /* ---- tooth 4 (RM-A1 三件套): 动作集决策表——SHOW|TONE 必达（弹窗+提示音, 决策
     * 27 三件套的设备侧两半）, SPEAK 仅 speech 非空（V20 语音解耦降级: 合成缺省仍
     * 弹窗+提示音不阻塞）。 */
    krmu_make_alert(&a, "rm-a1-1", "2026-09-09T16:15:00+08:00", "带画具", "明天美术课要带画具哦");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_reminder_render_actions(&a) == (KRM_ACT_SHOW | KRM_ACT_TONE | KRM_ACT_SPEAK),
        "RM-A1 RED: speech 非空的动作集漂移——三件套（弹窗+提示音+语音读出）动作集缺失 "
        "(决策 27)");
    krmu_make_alert(&a, "rm-a1-2", "2026-09-09T16:15:00+08:00", "带画具", "");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_reminder_render_actions(&a) == (KRM_ACT_SHOW | KRM_ACT_TONE),
        "RM-A1 RED: speech 缺省的动作集漂移——提示音/弹窗必达面被语音合成状态绑架 "
        "(V20 降级不阻塞)");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_render_actions(NULL) == -1,
        "RM-A1 RED: NULL 载荷未防御 (-1)——不盲投递红线缺失");

    /* ---- tooth 5 (RM-A1/E3 guard): 骨架第四成员闭集零偏移——提醒态 = pose-jump +
     * 键帽「知道了」（K2 闭集 kidpal_voice_states 单源, 强符号在 runner; 本复核 =
     * K3 TEST 4 tooth 4 同值零回归守卫, 骨架复用的值面锚）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_state_look(KVST_SCENE_REMIND, &look) == 0 &&
                                 look.pose == KVST_POSE_JUMP &&
                                 look.keycap != NULL &&
                                 strcmp(look.keycap, "知道了") == 0,
        "RM-A1 RED: 提醒态骨架第四成员闭集漂移（pose-jump/键帽「知道了」）——四态统一"
        "骨架的值面锚被触碰（K2 闭集零偏移红线, RM-E3 复用纪律）");
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — RM-A2 layer 栈优先级 + 分层裁决消费零回归守卫（behavioral + structural）
 * 行为半: REMINDER(5) 压 VOICE(3)/SERVER_PAGE(4)（查字窗/播报中被覆盖; 关提醒回前台）。
 * 结构半: 板级 dispatch 顶层先交 kidpal_reminder_key 裁决 + 前台归一化查询消费
 * （K1a 落地件, 本包零触碰守卫）+ REMINDER 入层通道在世（begin_reminder）。
 * ------------------------------------------------------------------------ */
/* coverage: RM-A2 -- 最高层覆盖行为半：layer 栈 REMINDER 压 VOICE/SERVER_PAGE（弹出时压过查字窗/播报中）+ 关提醒回前台 */
/* coverage: RM-A2 -- 不穿透机制零回归守卫：板级 dispatch 顶层先交 kidpal_reminder_key + front_is_overlay 消费 + begin_reminder 入层通道在世（K1a 落地件零触碰） */
TEST_CASE("提醒最高层覆盖: layer 栈优先级+分层裁决消费守卫 (RM-A2)",
          "[reminder_overlay_unify][RM-A2][layer][guard]")
{
    kidpal_layer_stack_t st;
    int n;

    /* ---- 行为半: layer 栈优先级（强符号 kidpal_layer.c 在 runner）。 ------------ */
    memset(&st, 0, sizeof(st));

    /* tooth 1: 播报中/录音态（VOICE）在台 -> 提醒入层压过（top = REMINDER）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_LOCAL_PAGE) == 0 &&
                                 kidpal_layer_push(&st, KIDPAL_LAYER_VOICE) == 0,
        "RM-A2 RED(infra): layer 栈底座回归——fixture 建栈失败（VOICE/LOCAL_PAGE push）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_REMINDER) == 0,
        "RM-A2 RED(infra): REMINDER push 失败——提醒入层通道断");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_top(&st) == KIDPAL_LAYER_REMINDER,
        "RM-A2 RED: 提醒未压过语音轮（VOICE）——最高层覆盖的 layer 优先级漂移"
        "（REMINDER(5) 必须 > VOICE(3), §2.2 层级口径）");

    /* tooth 2: 关提醒回前台——REMINDER 出栈后 top 回 VOICE（覆盖层关层不拆基层）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_pop(&st, KIDPAL_LAYER_REMINDER) == 0 &&
                                 kidpal_layer_top(&st) == KIDPAL_LAYER_VOICE,
        "RM-A2 RED: 关提醒后前台未回落语音轮——覆盖层出栈语义漂移");

    /* tooth 3: 查字窗（SERVER_PAGE）在台 -> 提醒入层压过。 */
    memset(&st, 0, sizeof(st));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_push(&st, KIDPAL_LAYER_LOCAL_PAGE) == 0 &&
                                 kidpal_layer_push(&st, KIDPAL_LAYER_SERVER_PAGE) == 0 &&
                                 kidpal_layer_push(&st, KIDPAL_LAYER_REMINDER) == 0,
        "RM-A2 RED(infra): fixture 建栈失败（SERVER_PAGE 查字窗 + REMINDER）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_layer_top(&st) == KIDPAL_LAYER_REMINDER,
        "RM-A2 RED: 提醒未压过查字窗（SERVER_PAGE）——最高层覆盖对服务端推送页失效 "
        "（REMINDER(5) 必须 > SERVER_PAGE(4), §2.2 层级口径）");

    /* ---- 结构半: 分层裁决消费零回归守卫（K1a 落地件, 本包零触碰）。 ------------ */
    KRMU_REQUIRE_ROOT();

    /* tooth 4: 板级 dispatch 顶层先交 kidpal_reminder_key 裁决（覆盖层顶层按键不穿透
     * 底层窗口的机制事实; K1a 设备侧首次接线, 零回归）。 */
    n = krmu_count_in_file(KRMU_BOARD_CC, "kidpal_reminder_key(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        krmu_fm("RM-A2 RED: 板级 dispatch 零 kidpal_reminder_key 消费（现 %d 处）——"
                "覆盖层顶层按键裁决函数被绕开, 弹窗置顶期间按键穿透底层窗口（K1a "
                "layer-aware 分流落地件被触碰）", n));

    /* tooth 5: 前台归一化查询消费在世（front_is_overlay 分流事实源, K1a）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        krmu_file_contains(KRMU_BOARD_CC, "kidpal_render_node_front_is_overlay("),
        "RM-A2 RED: 板级 dispatch 未消费 kidpal_render_node_front_is_overlay——"
        "layer 分流事实源被绕开（K1a 落地件零回归破坏）");

    /* tooth 6: REMINDER 入层通道在世（begin_reminder 推 REMINDER 层——弹窗渲染的
     * 入层点; kidpal_render_node.cc 设备侧, 结构齿树扫）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        krmu_file_contains(KRMU_RENDER_NODE_H, "kidpal_render_node_begin_reminder"),
        "RM-A2 RED: kidpal_render_node.h 零 kidpal_render_node_begin_reminder 声明——"
        "提醒入层通道被拆除（弹窗无法置前）");
    n = krmu_count_in_file(KRMU_RENDER_NODE_CC, "KIDPAL_LAYER_REMINDER");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        krmu_fm("RM-A2 RED: kidpal_render_node.cc 零 KIDPAL_LAYER_REMINDER 引用（现 "
                "%d 处）——提醒层不入 layer 栈（最高层覆盖无入层事实）", n));
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — RM-A4 排队不叠屏 + 关闭自动弹下一条（behavioral, frozen seam [3]）
 * 弹窗驻留期间新提醒入队不动驻留; raise 判定不替换不叠屏; KEY 关闭后下一条自动弹出。
 * RED today: weak sentinel -2（reminder_page.c 接线后转绿）。
 * ------------------------------------------------------------------------ */
/* coverage: RM-A4 -- 排队不叠屏：弹窗期间新提醒入队不动驻留 + raise_next 不替换不叠屏 + KEY 关闭后下一条自动弹出（AC3） */
TEST_CASE("提醒排队: 排队不叠屏+关闭自动弹下一条 (RM-A4)",
          "[reminder_overlay_unify][RM-A4][queue]")
{
    kidpal_reminder_t *s = krmu_fresh();
    kidpal_alert_t a, b;
    page_id_t page_out = PAGE_P1;
    char sid[KRM_CAP_ID];
    int v;

    krmu_make_alert(&a, "rm-a4-1", "2026-09-09T16:15:00+08:00", "带画具",
                    "明天美术课要带画具哦");
    krmu_make_alert(&b, "rm-a4-2", "2026-09-09T19:00:00+08:00", "练琴",
                    "晚上七点练琴时间到");

    /* fixture: 第一条弹出驻留。 */
    v = kidpal_reminder_receive(s, &a);
    krmu_require_implemented(v, "kidpal_reminder_receive unimplemented");
    TEST_ASSERT_TRUE_MESSAGE(v == KRM_STATE_QUEUED, "RM-A4 RED(infra): 首条接收未入队");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(s, KRMU_EV_1615) == 1,
        "RM-A4 RED(infra): 首条弹出失败——fixture 驻留未建立");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, sid, sizeof(sid)) == 1 &&
                                 strcmp(sid, "rm-a4-1") == 0,
        "RM-A4 RED(infra): 驻留项 id 漂移——fixture 异常");

    /* tooth 1: 弹窗期间第二条到达 = 入队不动驻留（排队, 不叠屏）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &b) == KRM_STATE_QUEUED &&
                                 kidpal_reminder_queue_count(s) == 1,
        "RM-A4 RED: 弹窗驻留期间新提醒未入队——排队面缺失（丢失提醒）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, sid, sizeof(sid)) == 1 &&
                                 strcmp(sid, "rm-a4-1") == 0,
        "RM-A4 RED: 新提醒到达替换了驻留弹窗——叠屏/抢屏（排队不叠屏被破坏, AC3）");

    /* tooth 2: raise 判定不替换不叠屏——驻留未渲染完成时 raise_next 恒 0（渲染完成
     * 前连 V18 顶替都不允许）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(s, KRMU_EV_1615 + 1) == 0,
        "RM-A4 RED: 驻留未渲染时 raise 判定替换了弹窗——叠屏（渲染级不变式被破坏）");

    /* tooth 3: KEY 关闭 -> 下一条自动弹出（FIFO 头 = 第二条; AC3「当前弹窗关闭后
     * 下一条自动弹出」——设备侧由 1s drain timer 驱动同一 raise 判定, 结构面见
     * test 7 tooth 7）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_key(s, KEY_ID_KEY, KEY_GESTURE_CLICK,
                                                 PAGE_P2, &page_out) == KRM_KEY_DISMISS,
        "RM-A4 RED: KEY 单击未 DISMISS——弹窗关不掉, 下一条永远弹不出");
    TEST_ASSERT_TRUE_MESSAGE(page_out == PAGE_P2,
        "RM-A4 RED: KEY 关闭未回当前页——关闭动作携翻页副作用");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(s, KRMU_EV_1615 + 1) == 1,
        "RM-A4 RED: 弹窗关闭后下一条未自动弹出——排队逐个弹缺失（AC3）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, sid, sizeof(sid)) == 1 &&
                                 strcmp(sid, "rm-a4-2") == 0,
        "RM-A4 RED: 自动弹出的不是排队头（FIFO 序漂移）——第二条提醒内容错位");
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — RM-A5 幂等去重不重复弹（behavioral, frozen seam [3]）
 * 同一提醒云端重算/重推不重复弹窗: 排队期同 id 屏蔽入队; 已渲染驻留重推补回回执
 * 不重弹; 已 acked 重推零动作; 关闭后同 id 不复活。
 * RED today: weak sentinel -2（reminder_page.c 接线后转绿）。
 * ------------------------------------------------------------------------ */
/* coverage: RM-A5 -- 去重不重复弹：排队期同 id 屏蔽入队 + 已渲染驻留重推补回回执不重弹 + 已 acked 重推零动作 + 关闭后不复活（AC4） */
TEST_CASE("提醒去重: 幂等去重不重复弹窗 (RM-A5)",
          "[reminder_overlay_unify][RM-A5][dedup]")
{
    kidpal_reminder_t *s = krmu_fresh();
    kidpal_alert_t a;
    char sid[KRM_CAP_ID];

    krmu_make_alert(&a, "rm-a5-1", "2026-09-09T16:15:00+08:00", "带画具",
                    "明天美术课要带画具哦");

    /* tooth 1: 排队期同 id 重推 -> 屏蔽入队（幂等去重, 与渲染无关）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &a) == KRM_STATE_QUEUED,
        "RM-A5 RED(infra): 首条接收未入队");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &a) == KRM_STATE_RECEIVED &&
                                 kidpal_reminder_queue_count(s) == 1,
        "RM-A5 RED: 排队期同 id 重推被重复入队——云端重算即重复弹窗（AC4 破坏）");

    /* tooth 2: 已渲染驻留被重推（回执在途丢失场景）-> 补回回执许可, 不重新弹窗。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(s, KRMU_EV_1615) == 1,
        "RM-A5 RED(infra): 弹出失败");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_mark_rendered(s, "rm-a5-1") == 1,
        "RM-A5 RED(infra): mark_rendered 失败");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &a) == KRM_STATE_RENDERED &&
                                 kidpal_reminder_may_emit_receipt(s) == 1,
        "RM-A5 RED: 已渲染驻留被重推未补回回执许可——送达确认会永久丢失（回执链断裂）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, sid, sizeof(sid)) == 1 &&
                                 strcmp(sid, "rm-a5-1") == 0,
        "RM-A5 RED: 重推触发重新弹窗/替换——同 id 不重复弹窗红线（AC4）");

    /* tooth 3: 渲染即确认出队 -> 回执许可清零; 再重推零动作（不复活、不重弹）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_mark_acked(s, "rm-a5-1") == 1 &&
                                 kidpal_reminder_may_emit_receipt(s) == 0,
        "RM-A5 RED: 渲染即确认的一 id 一回执语义漂移——二次回执/许可残留");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &a) == KRM_STATE_RECEIVED &&
                                 kidpal_reminder_queue_count(s) == 0 &&
                                 kidpal_reminder_raise_next(s, KRMU_EV_1615 + 1) == 0,
        "RM-A5 RED: 已确认驻留被重推复活/重弹——「关掉一个不会再弹一个」红线（AC4）");
}

/* -------------------------------------------------------------------------- *
 * TEST 6 — RM-A6 驻留到按键 + 键位全表 + RM-A7 过期不补发（behavioral, seam [3]）
 * 无超时驻留（不设无操作自动消失——raise 判定无 timer、驻留跨任意时长不消失）;
 * 键位全表（KEY 单击回当前页 / BOOT 单击关弹窗回原页不翻页——判别式 != key_page_next
 * / KEY 长按关层 / 其它手势 PASS 不吞）; V18 顶替非追加送达（被顶替项不复活不重弹、
 * 队列空不追催）。
 * RED today: weak sentinel -2（reminder_page.c 接线后转绿）。
 * ------------------------------------------------------------------------ */
/* coverage: RM-A6 -- 驻留到按键：无超时驻留（跨任意时长 raise 判定恒 0、驻留不变——不设无操作自动消失）+ 键位全表（KEY 单击回当前页 / BOOT 单击回原页不翻页判别式 / KEY 长按关层 / 其它手势 PASS） */
/* coverage: RM-A7 -- 过期档位不补发：V18 顶替非追加送达（被顶替项不复活不重弹）+ 队列空不追催（过期即弃, 云侧补发口径归云 RM-E1） */
TEST_CASE("提醒驻留与过期: 驻留到按键+键位全表+过期不补发 (RM-A6/RM-A7)",
          "[reminder_overlay_unify][RM-A6][RM-A7][key][dwell]")
{
    kidpal_reminder_t *s;
    kidpal_alert_t a, b;
    page_id_t page_out;
    char sid[KRM_CAP_ID];

    krmu_make_alert(&a, "rm-a6-1", "2026-09-09T16:15:00+08:00", "带画具",
                    "明天美术课要带画具哦");
    krmu_make_alert(&b, "rm-a7-1", "2026-09-09T19:00:00+08:00", "练琴",
                    "晚上七点练琴时间到");

    /* ---- Block 1 (RM-A6): 无超时驻留——弹窗跨任意时长驻留不变（过夜驻留无害,
     * 驻留口径 human 已裁: 不设无操作自动消失）。 */
    s = krmu_fresh();
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &a) == KRM_STATE_QUEUED &&
                                 kidpal_reminder_raise_next(s, KRMU_EV_1615) == 1,
        "RM-A6 RED(infra): fixture 弹出失败");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_mark_rendered(s, "rm-a6-1") == 1,
        "RM-A6 RED(infra): mark_rendered 失败");
    {
        long long probes[] = { KRMU_EV_1615 + 1, KRMU_EV_1615 + 60, KRMU_EV_1615 + 1440 };
        for (size_t i = 0; i < sizeof(probes) / sizeof(probes[0]); i++) {
            TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(s, probes[i]) == 0,
                krmu_fm("RM-A6 RED: 时刻 +%lld 分钟 raise 判定非 0——弹窗出现无操作自动"
                        "消失/超时替换（驻留到按键口径被破坏, 2026-09-14 human 裁）",
                        probes[i] - KRMU_EV_1615));
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, sid, sizeof(sid)) == 1 &&
                                 strcmp(sid, "rm-a6-1") == 0,
        "RM-A6 RED: 长时驻留后弹窗 id 漂移——驻留面被无操作事件改写");

    /* ---- Block 2 (RM-A6): 键位全表（每手势独立 fixture）。 ------------------- */
    /* KEY 单击 -> DISMISS, page_out = current（关闭回当前页）。 */
    s = krmu_fresh();
    page_out = PAGE_P1;
    (void)kidpal_reminder_receive(s, &a);
    (void)kidpal_reminder_raise_next(s, KRMU_EV_1615);
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_key(s, KEY_ID_KEY, KEY_GESTURE_CLICK,
                                                 PAGE_P2, &page_out)
                                 == KRM_KEY_DISMISS && page_out == PAGE_P2,
        "RM-A6 RED: KEY 单击未「关闭回当前页」——AC2 关闭语义漂移");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, sid, sizeof(sid)) == 0,
        "RM-A6 RED: KEY 单击后弹窗仍驻留——关闭未生效");

    /* BOOT 单击 -> DISMISS, page_out = current 且 != key_page_next(current)
     * （v1.39 隔离: 关弹窗回原页不翻页——「BOOT≈翻页」等价假绿判别式）。 */
    s = krmu_fresh();
    page_out = PAGE_P1;
    (void)kidpal_reminder_receive(s, &a);
    (void)kidpal_reminder_raise_next(s, KRMU_EV_1615);
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_key(s, KEY_ID_BOOT, KEY_GESTURE_CLICK,
                                                 PAGE_P2, &page_out)
                                 == KRM_KEY_DISMISS && page_out == PAGE_P2,
        "RM-A6 RED: BOOT 单击未关闭回原页——v1.39 覆盖层隔离语义漂移");
    TEST_ASSERT_TRUE_MESSAGE((int)page_out != (int)key_page_next(PAGE_P2),
        "RM-A6 RED: 弹窗内 BOOT 单击产翻页动作——「BOOT≈翻页」等价假绿复活"
        "（v1.39 隔离: 关弹窗回原页不翻页, S-F1.4 AC2）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, sid, sizeof(sid)) == 0,
        "RM-A6 RED: BOOT 单击后弹窗仍驻留——关闭未生效");

    /* KEY 长按 -> DISMISS 回原页（v1.36 返回上一层在覆盖层顶层的实例）。 */
    s = krmu_fresh();
    page_out = PAGE_P1;
    (void)kidpal_reminder_receive(s, &a);
    (void)kidpal_reminder_raise_next(s, KRMU_EV_1615);
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_key(s, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS,
                                                 PAGE_P2, &page_out)
                                 == KRM_KEY_DISMISS && page_out == PAGE_P2,
        "RM-A6 RED: KEY 长按未关层回原页——覆盖层顶层返回上一层实例漂移");

    /* 其它手势（BOOT 长按 / 双击族）-> PASS 不吞、驻留不变（弹窗不吞其它层手势）。 */
    s = krmu_fresh();
    (void)kidpal_reminder_receive(s, &a);
    (void)kidpal_reminder_raise_next(s, KRMU_EV_1615);
    page_out = PAGE_P2;
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_reminder_key(s, KEY_ID_BOOT, KEY_GESTURE_LONG_PRESS, PAGE_P2, &page_out)
            == KRM_KEY_PASS &&
        kidpal_reminder_key(s, KEY_ID_BOOT, KEY_GESTURE_DOUBLE_CLICK, PAGE_P2, &page_out)
            == KRM_KEY_PASS &&
        kidpal_reminder_key(s, KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK, PAGE_P2, &page_out)
            == KRM_KEY_PASS,
        "RM-A6 RED: 弹窗吞掉 BOOT 长按/双击族手势——不穿透口径被扩大（其它层手势"
        "语义零触碰）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, sid, sizeof(sid)) == 1 &&
                                 strcmp(sid, "rm-a6-1") == 0 && page_out == PAGE_P2,
        "RM-A6 RED: PASS 手势改写了驻留或页面——驻留不变红线");

    /* 无弹窗驻留 -> 任意手势 PASS 且不介入（覆盖层裁决不越权）。 */
    s = krmu_fresh();
    page_out = PAGE_P1;
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_key(s, KEY_ID_KEY, KEY_GESTURE_CLICK,
                                                 PAGE_P1, &page_out) == KRM_KEY_PASS &&
                                 page_out == PAGE_P1,
        "RM-A6 RED: 无弹窗时裁决函数吞键——覆盖层越权介入基层手势");

    /* ---- Block 3 (RM-A7): 过期不补发——V18 顶替非追加送达 + 队列空不追催。 ----- */
    s = krmu_fresh();
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &a) == KRM_STATE_QUEUED &&
                                 kidpal_reminder_raise_next(s, KRMU_EV_1615 - 60) == 1,
        "RM-A7 RED(infra): fixture 弹出失败");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_mark_rendered(s, "rm-a6-1") == 1,
        "RM-A7 RED(infra): mark_rendered 失败");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &b) == KRM_STATE_QUEUED,
        "RM-A7 RED(infra): 第二条入队失败");
    /* 事件时刻已过 + 队列非空 -> V18 滞留顶替（永未按键的过期弹窗被 FIFO 头顶替）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(s, KRMU_EV_1615) == 2,
        "RM-A7 RED: 事件已过的滞留弹窗未被顶替——V18 顶替判定缺失（过期弹窗永占前台）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_shown_id(s, sid, sizeof(sid)) == 1 &&
                                 strcmp(sid, "rm-a7-1") == 0,
        "RM-A7 RED: 顶替后驻留项错位——FIFO 头顶替语义漂移");
    /* 被顶替项不复活不补发: 同 id 重推零动作（顶替非追加送达）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_receive(s, &a) == KRM_STATE_RECEIVED &&
                                 kidpal_reminder_queue_count(s) == 0 &&
                                 kidpal_reminder_raise_next(s, KRMU_EV_1900) == 0,
        "RM-A7 RED: 被顶替项同 id 重推复活/追加送达——过期档位补发（不补发不追催口径"
        "被破坏）");
    /* 队列空 -> 不追催: 任意时刻 raise 恒 0（设备零本地事项源, 过期即弃）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_reminder_raise_next(s, KRMU_EV_1900 + 1440) == 0,
        "RM-A7 RED: 队列空时 raise 判定非 0——设备本地产生补发/追催（零本地事项源"
        "红线）");
}

/* -------------------------------------------------------------------------- *
 * TEST 7 — RM-A1/A3/A6/A7 活侧接线结构齿（structural, frozen seam [4]）
 * 骨架复用（apply_look 单源, 反 fork）+ 时间行单源消费 + 旧 v1.13 卡面调用点销账 +
 * 门控接线 + 录音结束触发 + 三件套提示音强定义守卫 + drain timer 只弹不关 +
 * 提醒模块零 timer/零三档字面值。
 * RED today: 骨架/门控/触发 needle 全零命中 + 旧卡面调用点仍在（义务红, 设计态）。
 * ------------------------------------------------------------------------ */
/* coverage: RM-A1 -- 四态统一骨架结构半：lcd_display.cc 提醒弹窗渲染路径消费 kidpal_voice_apply_look(KVST_SCENE_REMIND)（复用 K3 骨架单源, RM-E3 反 fork）+ 时间行经 kidpal_reminder_hhmm 单源 + 旧 v1.13 卡面渲染调用点销账（mcp_server.cc RaiseReminderQueue 零 kidpal_reminder_page_render） */
/* coverage: RM-A3 -- 录音排队例外结构半：mcp_server.cc RaiseReminderQueue 消费 kidpal_reminder_gate（录音态不 raise）+ application.cc 录音会话结束路径触发 RaiseReminderQueue（立刻弹; 触发点路由不锁） */
/* coverage: RM-A6 -- drain timer 只弹不关（AlertDrainTimerCb 区域零 kidpal_reminder_key——零自动消失的机制面守卫） */
/* coverage: RM-A7 -- 提醒模块零本地事项源：reminder_page.c 零 lv_timer（无本地补发/追催/自动消失 timer）+ 零三档阶梯时刻字面值（20:30/07:45 零进固件, RMD-A7 纪律承接） */
/* coverage: RM-A8 -- 真机前置接缝断言（骨架/门控/录音结束触发接线全部在世 = 真机确认轮前置; 真机证据落 changes/dev, human 持板） */
TEST_CASE("提醒弹窗统一: 活侧接线——骨架复用+门控+录音结束触发+零自动消失 (RM-A1/RM-A3/RM-A6/RM-A7/RM-A8)",
          "[reminder_overlay_unify][RM-A1][RM-A3][RM-A6][RM-A7][RM-A8][structural]")
{
    char *region;
    int n;

    KRMU_REQUIRE_ROOT();

    /* tooth 1 (RM-A1/E3): 提醒弹窗渲染路径消费四态骨架单源——lcd_display.cc 出现
     * kidpal_voice_apply_look(KVST_SCENE_REMIND)（pose-jump + 键帽「知道了」从 K2
     * 闭集单源取出; 复用 K3 骨架不另建——RM-E3 反 fork: 绕开 apply_look 另立第二
     * 渲染源即 fork, 本齿咬住）。 */
    n = krmu_count_in_file(KRMU_LCD_CC, "kidpal_voice_apply_look(KVST_SCENE_REMIND");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        krmu_fm("RM-A1 RED: lcd_display.cc 零 kidpal_voice_apply_look(KVST_SCENE_REMIND)"
                " 消费（现 %d 处）——提醒弹窗未切四态统一骨架（v1.13 旧卡面形态在世, "
                "requirements §2.2 提醒行「版式与语音轮三态同一骨架」未落成）", n));

    /* tooth 2 (RM-A1): 时间行单源消费——lcd_display.cc 渲染路径消费
     * kidpal_reminder_hhmm（大字事项+时间的「时间」半; 渲染面重写第二份时间解析 =
     * 事实源分叉）。 */
    n = krmu_count_in_file(KRMU_LCD_CC, "kidpal_reminder_hhmm(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        krmu_fm("RM-A1 RED: lcd_display.cc 零 kidpal_reminder_hhmm 消费（现 %d 处）——"
                "弹窗时间行未接提醒模块单源（渲染面自抄时间解析 = 事实源分叉）", n));

    /* tooth 3 (RM-A1): 旧 v1.13 卡面渲染调用点销账——mcp_server.cc RaiseReminderQueue
     * 区域零 kidpal_reminder_page_render 调用（白底描边卡面形态作废, v1.23/v1.24 裁;
     * 渲染入口迁 lcd_display.cc 骨架面）。 */
    region = krmu_region_between(KRMU_MCP_CC,
                                 "void McpServer::RaiseReminderQueue()",
                                 "void McpServer::EmitRenderReceipt(");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "KRMU RED(infra): mcp_server.cc RaiseReminderQueue 区域切取失败（锚漂移——"
        "RaiseReminderQueue/EmitRenderReceipt 结构被改动须复核本齿锚）");
    n = krmu_count_in_slice(region, "kidpal_reminder_page_render");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        krmu_fm("RM-A1 RED: RaiseReminderQueue 仍调用 kidpal_reminder_page_render "
                "（现 %d 处）——v1.13 白底描边旧卡面渲染调用点未销账（四态统一骨架"
                "切换未完成, requirements §2.2 提醒行）", n));

    /* tooth 4 (RM-A3): 门控接线——RaiseReminderQueue 消费 kidpal_reminder_gate
     * （录音态 -> 不 raise、队列保持; 门控消费即 RM-A3 的机制落点）。 */
    n = krmu_count_in_slice(region, "kidpal_reminder_gate(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        krmu_fm("RM-A3 RED: RaiseReminderQueue 零 kidpal_reminder_gate 消费（现 %d 处）"
                "——录音排队例外未接 raise 判定（录音中提醒照弹, TTS 混进录音内容）", n));
    free(region);

    /* tooth 5 (RM-A3): 录音会话结束立刻弹——application.cc 触发 RaiseReminderQueue
     * 在世（录音会话结束路径主动触发; 触发点路由不锁——tts stop manual 分支 / 会话
     * 级联关闭均可; 若实现采用 mcp_server 侧查询录音态等其它形态, 须返
     * planner/acceptance-author 复核返钉, 不得静默改齿）。 */
    n = krmu_count_in_file(KRMU_APP_CC, "RaiseReminderQueue(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        krmu_fm("RM-A3 RED: application.cc 零 RaiseReminderQueue 触发（现 %d 处）——"
                "「录音会话结束立刻弹」接线缺失（录音结束后提醒滞留队列不弹, §2.2 "
                "唯一例外口径后半句未落成）", n));

    /* tooth 6 (RM-A1 guard): 三件套提示音强定义零回归——mcp_server.cc OGG_POPUP
     * 弹窗音源在世（弹窗 + 提示音 + 语音读出同时发生的提示音一半; 决策 27）。 */
    n = krmu_count_in_file(KRMU_MCP_CC, "OGG_POPUP");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        krmu_fm("RM-A1 RED: mcp_server.cc OGG_POPUP 提示音面丢失（现 %d 处）——三件套"
                "的提示音一半被拆（决策 27 必达面）", n));

    /* tooth 7 (RM-A6): drain timer 只弹不关——AlertDrainTimerCb 区域零
     * kidpal_reminder_key（自动关弹窗 = 无操作自动消失, 驻留口径永不加; drain timer
     * 只承载「关闭后下一条自动弹出」+ V18 顶替的 raise 判定）。 */
    region = krmu_region_between(KRMU_MCP_CC,
                                 "void AlertDrainTimerCb(",
                                 "std::string AlertArgsJson(");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "KRMU RED(infra): mcp_server.cc AlertDrainTimerCb 区域切取失败（锚漂移——"
        "drain timer 结构被改动须复核本齿锚）");
    TEST_ASSERT_TRUE_MESSAGE(krmu_count_in_slice(region, "RaiseReminderQueue") >= 1,
        "RM-A4 RED(guard): drain timer 零 RaiseReminderQueue——「关闭后下一条自动弹出」"
        "的驱动面被拆（排队逐个弹缺失）");
    n = krmu_count_in_slice(region, "kidpal_reminder_key");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        krmu_fm("RM-A6 RED: drain timer 区域出现 %d 处 kidpal_reminder_key——定时器"
                "路径关弹窗 = 无操作自动消失（驻留到按键口径被破坏, 2026-09-14 human "
                "裁: 不设无操作自动消失, 过夜驻留无害）", n));
    free(region);

    /* tooth 8 (RM-A7): 提醒模块零本地事项源——reminder_page.c 零 lv_timer（无本地
     * 补发/追催/自动消失 timer）+ 零三档阶梯时刻字面值（20:30/07:45 永不进固件;
     * 三段阶梯时刻 = 纯服务端配置, RM-E1 云侧零改动的设备侧镜像）。 */
    n = krmu_count_in_file(KRMU_REM_C, "lv_timer");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        krmu_fm("RM-A7 RED: reminder_page.c 出现 %d 处 lv_timer——提醒模块引入本地"
                "定时器（无超时驻留/零本地补发的介质与语义禁令被触碰）", n));
    TEST_ASSERT_TRUE_MESSAGE(krmu_count_in_file(KRMU_REM_C, "20:30") == 0 &&
                                 krmu_count_in_file(KRMU_REM_C, "07:45") == 0,
        "RM-A7 RED: reminder_page.c 出现三档阶梯时刻字面值（20:30/07:45）——设备零"
        "本地事项源红线（三段阶梯时刻 = 纯服务端配置, RM-E1）");
}

/* -------------------------------------------------------------------------- *
 * TEST 8 — boundary：本文件/被测实现注册序守卫 + RM-E2 范围纪律 canary + 扫描器自检
 *   RM-A9 结构半（本验收文件 + reminder_page.c 是否被 builder 接线进 HOST_ACCEPTANCE_
 *   SRCS, smoke 之前）
 *   RM-E2 需求面零修订 canary（requirements §2.2 提醒行锚 + 契约面在世）
 *   扫描器自检（结构齿负向必须能红, 防假绿）
 * ------------------------------------------------------------------------ */
/* coverage: RM-A9 -- host runner 终跑：本文件+reminder_page.c 注册进 HOST_ACCEPTANCE_SRCS 结构守卫 + suite 级判读锚（文件尾「RM-A9 终跑判读」段） */
/* coverage: RM-E2 -- 需求面零修订 canary：requirements §2.2 提醒行锚（最高层覆盖/驻留/录音排队）+ 契约面在世 */
TEST_CASE("提醒弹窗统一: 注册序守卫+需求契约canary+扫描器自检 (RM-A9/RM-E2)",
          "[reminder_overlay_unify][RM-A9][RM-E2][boundary]")
{
    char *mk;
    char hit[KRMU_MAX_PATH];
    char *region;
    int n;

    KRMU_REQUIRE_ROOT();

    /* tooth 1 (RM-A9): 本文件 + reminder_page.c 须接线进 HOST_ACCEPTANCE_SRCS 且排在
     * host_infra_smoke 之前（smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫）。
     * RED until builder wires. */
    mk = krmu_read(KRMU_CMAKELISTS, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "KRMU RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *mine = strstr(mk, "reminder_overlay_unify.acceptance.c");
        const char *impl = strstr(mk, "display/reminder_page.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(mine != NULL,
            "RM-A9 RED: reminder_overlay_unify.acceptance.c 未接线进 "
            "HOST_ACCEPTANCE_SRCS — host 验收构建未接本包（BUILDER DUTY: 与本文件头注 "
            "[5] 同款接线）");
        TEST_ASSERT_TRUE_MESSAGE(impl != NULL,
            "RM-A9 RED: display/reminder_page.c 未接线进 HOST_ACCEPTANCE_SRCS — 状态机"
            "强符号源缺失（weak 哨兵永不被覆盖, test 2/4/5/6 恒红）");
        TEST_ASSERT_TRUE_MESSAGE(smoke != NULL,
            "KRMU RED(infra): host_infra_smoke.acceptance.c 从 HOST_ACCEPTANCE_SRCS "
            "丢失 — 包 0 守卫被移除");
        TEST_ASSERT_TRUE_MESSAGE(mine < smoke && impl < smoke,
            "RM-A9 RED: 本包两行必须排在 host_infra_smoke.acceptance.c 之前 "
            "(smoke 末位 = 最先执行, boundary 齿注册序守卫)");
    }
    free(mk);

    /* tooth 2 (RM-E2): 需求面零修订 canary——requirements §2.2 提醒行三锚在世（层级
     * 口径 / 驻留口径 / 录音排队例外的语义权威; 本包只留实现证据, 零 docs 面改动）+
     * 契约面在世。 */
    TEST_ASSERT_TRUE_MESSAGE(krmu_exists(KRMU_REQUIREMENTS),
        "RM-E2 RED(infra): docs/requirements.md 丢失——需求面被触碰（RM-E2 零修订红线）");
    TEST_ASSERT_TRUE_MESSAGE(
        krmu_file_contains(KRMU_REQUIREMENTS, "提醒 = 最高层覆盖"),
        "RM-E2 RED: requirements §2.2 层级口径锚（提醒 = 最高层覆盖）丢失——需求面被"
        "修订（RM-E2: 发现冲突走 spec-lane 逃生门, 不预先改需求）");
    TEST_ASSERT_TRUE_MESSAGE(
        krmu_file_contains(KRMU_REQUIREMENTS, "不设无操作自动消失"),
        "RM-E2 RED: requirements 驻留口径锚（不设无操作自动消失）丢失——需求面被修订");
    TEST_ASSERT_TRUE_MESSAGE(
        krmu_file_contains(KRMU_REQUIREMENTS, "录音期间提醒排队延后"),
        "RM-E2 RED: requirements 录音排队例外锚丢失——需求面被修订");
    TEST_ASSERT_TRUE_MESSAGE(krmu_exists(KRMU_CONTRACT),
        "KRMU RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");

    /* tooth 3: 扫描器自检（防假绿: 负向齿必须能红）。 */
    TEST_ASSERT_TRUE_MESSAGE(krmu_exists(KRMU_MCP_CC),
        "KRMU RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(krmu_exists("firmware/main/__krmu_no_such__.cc"),
        "KRMU RED(infra): scanner reports a nonexistent file as present");
    n = krmu_count_in_file(KRMU_REM_H, "kidpal_reminder_receive(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "KRMU RED(infra): occurrence counter cannot count a known literal — 计数器失灵");
    region = krmu_region_between(KRMU_MCP_CC,
                                 "void McpServer::RaiseReminderQueue()",
                                 "void McpServer::EmitRenderReceipt(");
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "KRMU RED(infra): region slicer cannot slice a known region — 切区器失灵");
    TEST_ASSERT_TRUE_MESSAGE(
        krmu_count_in_slice(region, "kidpal_reminder_raise_next(") >= 1,
        "KRMU RED(infra): region slicer lost a known literal — 切区器计数失灵");
    free(region);
    (void)hit; /* scan helper 保留给后续增补轮; 本轮计数面用 count_in_file 承载 */
}

/* ---------------------------------------------------------------------------
 * RM-A9 终跑判读（suite 级判读说明, review-packet 呈 supervisor）:
 *   判据 = 相对有效基线零新增红 + 本包新齿全绿。
 *   有效基线 = intake 侧 2026-09-17 acceptance-author 实跑两面（progress.md 2026-09-16
 *   k2 批段锚: server 面 0 collected @ a398264 / firmware host 面 13 Tests / 0 Failures
 *   @ e990eb2; launch 判定两面相对锚均有 diff → 两面重跑, 新基线计数与成因见
 *   delivery/taskpackages/260915-m1-k4-reminder-overlay-unify/red-baseline.md——
 *   schedule_reminder_b 套齿经 260915-m1-k1a-key-layer-dispatch KD-A4 批改 + 归档,
 *   host 面跑的是 k1a 改齿后的新基线）。
 *   本包义务红（设计态, builder 落地后转绿, 不计入「新增红」违约）:
 *     - test 1/2/4/5/6（weak 哨兵 -2: reminder_page.c 未接线 = 真实缺失面红;
 *       其中 test 2 tooth 4/5 与 test 3 为接线后即绿的守卫半）;
 *     - test 7 tooth 1/2/4/5（骨架/时间行/门控/录音结束触发 needle 零命中）+
 *       tooth 3（旧卡面调用点仍在）——真实缺失行为红;
 *     - test 8 tooth 1（注册序: builder 未接线前为真实缺失红）。
 *   终跑复核命令与实测数字见交付报告 red-baseline 段（planner 转写落账）。
 *
 * RED baseline 判读口径（freeze 相位, acceptance-author）:
 *   通道① 既有 host runner 全量实跑（不含本文件——builder 接线前本文件不进 runner,
 *   不构成 runner 红; 接线后本文件义务红按上表定性）。
 *   通道② 本文件新齿 C 类冻结验收实编（DEV-GOVERNANCE 验收测试纪律 §4）:
 *     gcc -std=c11 -Wall -Wextra -fsyntax-only -I firmware/main/test/acceptance/
 *         red-baseline -I firmware/main/test firmware/main/test/reminder_overlay_
 *         unify.acceptance.c
 *     编译通道 error 为零（验收文件本身可编译）+ 解析性红定性: kidpal_reminder_gate /
 *     kidpal_reminder_hhmm / 状态机九符号全部 weak 哨兵 -2（test 1/2/4/5/6 全齿红）、
 *     lcd_display.cc 零 KVST_SCENE_REMIND / kidpal_reminder_hhmm 消费、mcp_server.cc
 *     RaiseReminderQueue 区域零 kidpal_reminder_gate 且 kidpal_reminder_page_render
 *     调用点在世、application.cc 零 RaiseReminderQueue 触发（test 7 义务红齿）、
 *     test 8 tooth 1 注册行缺失——各齿对现状必红, 红基线成立（接缝缺失即测试意图缺的
 *     真实行为）。test 3 为现状即绿的守卫齿（K1a 落地件消费面 + layer 优先级在世,
 *     零回归守卫本性, 不是假绿）。
 *   cwd 口径: 结构齿以真实工作树扫描, root 自手持 cwd 定位（仓根 / firmware/main/
 *   test/host_app 均可）; delivery/changes/dev/ 下证据路径一律按相对路径 + 仓根
 *   cwd 前提（C 侧遗留指针纪律, root 定位吃 cwd）。
 * ---------------------------------------------------------------------------
 * RM-A8 真机证据要求（本文件不承载, human 持板轮; enforce-scope 拦 changes/dev 写入
 * ——本段由 planner 转写进 delivery/changes/dev/260915-m1-k4-reminder-overlay-unify/）:
 *   ① 弹窗演出: 四态统一骨架（左小伴 pose-jump + 右大字事项/时间）真机观感 + 三件套
 *      同时发生（提示音 + 语音读出 + 弹窗驻留）;
 *   ② 最高层覆盖: 查字窗开启 / 语音播报中 / 思考页弹出时到点提醒压过一切前台;
 *   ③ 录音排队例外: 录音中到点不弹、录音会话结束立刻弹（提醒 TTS 不混进录音内容）;
 *   ④ 驻留手感: 过夜/长时驻留不消失; KEY 关闭回当前页 / BOOT 关闭回原页不翻页;
 *      弹窗期间第二条到达排队、关闭后自动弹下一条。
 *   证据形态 = 模拟器截图（统一骨架弹窗、覆盖各前台态）+ 真机轮日志/照片, 落
 *   delivery/changes/dev/260915-m1-k4-reminder-overlay-unify/。
 * ---------------------------------------------------------------------------
 * 范围注记（无齿条目）:
 *   RM-E1——三段阶梯时刻/范围口径/权限 = 云侧数据面（MVP 已交付）, 本包零 server 面
 *   改动: 本文件无 server 面接缝齿; 零回归由红基线记录承载（server 面 intake 实跑
 *   数字 + --ignore=server/acceptance/archive 口径, 见 red-baseline.md）。
 *   AC5 断网无提醒（设备不缓存事项）——介质禁令（零持久层）已在归档冻结件 RMD-A6/A7
 *   钉死, reminder_page.c 现状零 NVS/文件面; 本包零触碰不重钉（test 7 tooth 8 的
 *   零 timer/零字面值齿是其本包侧延伸）。
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-17  initial freeze (acceptance-author, TaskPackage
 *               260915-m1-k4-reminder-overlay-unify)。
 * --------------------------------------------------------------------------- */
