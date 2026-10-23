/*
 * Frozen acceptance tests — TaskPackage 261019-p3-record-dialogue-guide
 * (subsystem: firmware/main——application.cc 聆听形态切换与 kvr 场景守卫 +
 *  display/view_router.h/.c VRT_CTRL_START_ORGANIZING=3 additive +
 *  mcp_server.cc 控制 case 3 消费 + display/story_page.c 引导播报中子态/
 *  整理过渡态渲染与键位分叉; rigor=full; riskTier=high)。
 *
 * Story 语义权威 = docs/requirements.md v1.65 S-F2.2（对话式引导记录五 AC）
 * + NFR-PERF-3 + reviewed plan（delivery/taskpackages/261019-p3-record-dialogue-guide/
 * plan.md S3/S4 段）+ FROZEN PROPOSAL 261019-arch-review-p3-record-dialogue-guide
 * （ADR-013 + q2 = ⓒ start_organizing 下行信号）。
 *
 * Nature: 行为级（键位映射/控制消费纯逻辑真函数 + weak 哨兵：实现未编入前哨兵恒
 * 返回 RDL_UNIMPLEMENTED / 动作错值 = 「冻结契约未实现」设计态红，不是编译错）
 * + 结构级（真实工作树源锚扫描，sd_log / record_session_repair 先例形态；正向断言
 * 只认**注释剥离后可执行源**的字面形态——260917-m2-k3 sealing pass 教训）。
 *
 * 本文件**不 include** display/story_page.h（镜像公共前缀 + 尾部追加字段，K3R 先例；
 * 落地后 host runner 中 display/story_page.c 强符号覆盖本文件 weak 哨兵）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；「home =」为冻结落点）:
 *
 *   [G1] VRT_CTRL additive 数值码（home = display/view_router.h + view_router.c;
 *        冻结纪律只增不改/移——0/1/2 三枚既有数值码零偏移）:
 *
 *          VRT_CTRL_START_ORGANIZING = 3   〔枚举尾部 additive，冻结数值 = 3〕
 *
 *          int kidpal_display_control_action(const char *action);
 *            -- "start_organizing" -> 3；既有 "end_session"->0 / "stop_speaking"->1 /
 *               "start_recording"->2 零偏移；NULL / 未知 -> VRT_CTRL_UNKNOWN(-1) 降级。
 *
 *   [G2] 整理过渡页态 + on_control case 3 消费（home = display/story_page.h 枚举
 *        尾部 additive + story_page.c kidpal_story_on_control 扩展）:
 *
 *          KSTORY_PAGE_ORGANIZING = 6      〔枚举尾部 additive（BODY_UNAVAILABLE=5
 *                                          之后），冻结数值 = 6〕
 *
 *          int kidpal_story_on_control(kidpal_story_view_t *v, int ctrl_action);
 *            -- ctrl_action == 3（START_ORGANIZING）-> v->page = 6（整理过渡态）、
 *               返回 1；**重复下发幂等**（已在整理态再收 case 3 -> 页态保持、返回 1、
 *               零副作用翻倍）；既有 case 0/1（-> 返回 0 视图零改动）/ case 2
 *               （-> 返回 1 + page=RECORDING + rec_seconds=0）/ NULL v（-> -1）/
 *               其它值（-> -1 降级）零偏移。
 *
 *   [G3] 引导播报中子态（帧 B）键位分叉（home = display/story_page.h 尾部追加 +
 *        story_page.c kidpal_story_page_input RECORDING case 子态化——本包固件侧
 *        最关键的键位分叉：子态不拦截则引导播报一句期间按 BOOT 即毁稿）:
 *
 *          kidpal_story_view_t 尾部追加字段（冻结纪律只增不改/移; 冻结名）:
 *          int rec_substate;   -- 0 = 常规录音聆听; 1 = 引导播报中
 *          #define KSPV2_ACT_INTERRUPT_GUIDE 12  -- 动作码尾部 additive，冻结数值
 *                        = 12：打断引导播报、回常规录音聆听（消费方 AbortSpeaking
 *                        + 继续聆听；页态保持 RECORDING、子态复位 0）
 *
 *          int kidpal_story_page_input(kidpal_story_view_t *v, key_id_t key,
 *                                      key_gesture_t g);
 *            -- RECORDING + rec_substate==1（引导播报中）:
 *                 KEY 短按 -> 12；KEY 长按 -> 12（同短按，§2.1 打断停播语义）；
 *                 返回后 v->page == RECORDING 且 v->rec_substate == 0（子态复位 =
 *                 继续聆听）；
 *                 BOOT 任一手势（短/双/长）-> KSPV2_ACT_NONE(0)、page 保持
 *                 RECORDING、cancel_count 不动、archive_pend 不动——**无操作，
 *                 绝不落 DISCARD_RECORD 毁稿路径**；
 *               RECORDING + rec_substate==0（常规录音，既有语义回归守卫）:
 *                 KEY 短按/长按 -> KSPV2_ACT_STOP_RECORD(10)；BOOT 短按 ->
 *                 KSPV2_ACT_DISCARD_RECORD(11)（+cancel_count、回 LIST）——既有
 *                 语义零偏移。
 *
 *   [G4] 整理过渡态键位不响应（home = story_page.c 输入函数新页 case）:
 *            -- page == KSTORY_PAGE_ORGANIZING(6)：KEY/BOOT 任意手势 ->
 *               KSPV2_ACT_NONE(0)、page 保持 6、零副作用（非交互过渡态，
 *               requirements v1.65 AC4 / §2.3 录音态子态按键口径）。
 *
 *   [G5] 渲染分支与回显常驻（home = story_page.c；结构锚）:
 *            -- story_page.c 内 KSTORY_PAGE_ORGANIZING 引用 >= 2 处（输入 case +
 *               渲染 case——整理过渡态渲染分支在世：pose-think 复用 / 三问号组 /
 *               「你刚说」标签等视觉细节归设计稿，本锚只钉分支在世）；
 *            -- 引导播报中子态渲染分支在世（rec_substate 渲染引用 + 键帽左
 *               「打断」字面；右槽留空）；
 *            -- **回显常驻**：story_page.c 内 kidpal_story_echo_reset( 出现次数
 *               == 1（仅定义处——引导播报中子态 / 整理过渡态渲染分支**不得清空**
 *               话语回显区，AC2「回显区常驻」防回退锚）+ kidpal_story_echo_line(
 *               渲染消费在世。
 *
 *   [G6] kvr 场景守卫 + 消费闭环（home = application.cc + mcp_server.cc）:
 *            -- application.cc stt-final 消费回调内：ctx 选取段必须联挂记录语境
 *               标志（kidpal_story_session_ctx_active 引用先于
 *               kidpal_voice_round_scene 调用）——记录语境切逐句 stt 形态后 ctx
 *               选取不得按 listening_mode 误映 KVST_SCENE_THINK -> SetStatus(
 *               THINKING) 跳页（切句只回显不进思考，AC1）；
 *            -- KSPV2_ACT_INTERRUPT_GUIDE 消费点在世（application.cc 或
 *               mcp_server.cc 引用 >= 1——驱动方 <-> 消费方闭环，防动作码死代码）；
 *            -- mcp_server.cc 控制 switch 消费 start_organizing（VRT_CTRL_START_
 *               ORGANIZING 引用 + story_consumed 日志同型）；
 *            -- S-F1.4-AC6 录音期提醒排队延后不回归（server 半 RDG-A7）：RM-A3
 *               门控以**页面态**为权威（g_story_view.page == KSTORY_PAGE_RECORDING
 *               判据在世）——引导播报中子态页态仍 RECORDING 自动 defer、整理态页
 *               态已离录音自动放行，聆听形态切换不得漂移门控判据。
 *
 *   [G7] recording_enter 既有缝回归守卫（home = story_page.c [S2] 既有）:
 *            -- kidpal_story_recording_enter 决策表零偏移（记录语境开口切录音态；
 *               既有 RSL 齿保绿的廉价回归锚）。
 *
 *   [G8] live-manual 真机轮观测项（**不落自动断言、不计入 runner 通过面**，
 *        261018 SLP-A6 / 261012 RF-A5 同款口径；本 Pod 到真机门前停等
 *        status=awaiting-human:real-device）: ① VAD 真沿与 AFE/AEC（realtime
 *        聆听形态取舍 host 不可机判）；② 引导播完继续录音、设备不离记录会话
 *        （注入不尾随 auto-listen 真机半，TECH-261014-post-speak-listening-
 *        lingering 防回归 case）；③ 引导播报不跳页（停留记录屏、气泡换引导
 *        文字、回显常驻——AC2 观感半）；④ state-organizing 帧真机可见（翻✅
 *        条件③）；⑤ S-F1.4-AC6 端到端（提醒 TTS 不混进录音内容）。
 * ===========================================================================
 * 契约 / 决策条目 ↔ 验收断言 的强度申报（acceptance-evidence「契约条目 ↔ 验收
 * 断言」节）:
 *
 * | 契约 / 决策条目 | 承载断言 | 性质档 |
 * |---|---|---|
 * | q2 = ⓒ（start_organizing additive 消费） | TEST 1（数值码真跑 + 头文件锚）+ TEST 2（on_control 真跑）+ TEST 6（mcp 消费锚） | 行为级 + 源锚代理 |
 * | S-F2.2-AC2 引导播报中 KEY 打断/BOOT 无操作 | TEST 3（子态键位分叉真跑） | 行为级 |
 * | S-F2.2-AC2 回显区常驻 | TEST 5（echo_reset 计数防回退锚 + echo 渲染在世） | 源锚代理（防回退负锚） |
 * | S-F2.2-AC4 整理中 KEY/BOOT 均不响应 | TEST 4（整理态键位真跑）+ TEST 5（渲染分支在世） | 行为级 + 源锚代理 |
 * | S-F2.2-AC1 切句只回显不进思考（kvr 守卫） | TEST 6（application.cc 结构锚） | 源锚代理（运行时归真机轮 G8③） |
 * | S-F1.4-AC6 录音期提醒排队延后不回归 | TEST 6（RM-A3 页面态判据锚）+ RDG-A7（server 半） | 源锚代理（端到端归真机轮） |
 * | 既有 start_recording / DISCARD / STOP_RECORD 语义 | TEST 2/3 回归守卫半 | 行为级（must-stay-green） |
 *
 * 弱于契约处（显式缺口，不得当已兑现）:
 *   GAP-1 「打断引导 = AbortSpeaking + 继续聆听」的运行时效果（真的停播、真的
 *         回聆听）host 不可达——TEST 3 钉页内映射与子态复位，消费端行为归真机轮
 *         G8②（TEST 6 消费闭环锚为源锚代理）。
 *   GAP-2 渲染视觉细节（三问号组/波浪线/云朵/键帽几何）= 设计稿面，纯美学不转
 *         脆性断言——TEST 5 只钉分支在世与回显常驻。
 *   GAP-3 kvr 守卫运行时（THINKING 不出现于记录语境）= 真机轮 G8③；结构锚 ≠
 *         运行时行为（EQ-3）。
 *   GAP-4 聆听形态（AutoStop vs Realtime）取舍 = 包内裁（application.cc），
 *         host 不可机判（AFE/AEC 依赖）——真机轮 G8① 承载。
 *
 * 行为等价假设（显式；不写即默认不成立）:
 *   EQ-1 「weak 哨兵被强符号覆盖 = 纯逻辑接线落地」：standalone 红跑不链接
 *        story_page.c / view_router.c，哨兵恒红 = 设计态红；host runner 中
 *        story_page.c（已在册）+ 本文件必须在 HOST_ACCEPTANCE_SRCS 在世（TEST 7
 *        登记守），否则哨兵红会掩盖「未接线」。
 *   EQ-2 「镜像 == 真头」：本文件 kidpal_story_view_t 镜像 = story_page.h 现役
 *        形状 + 尾部追加 rec_substate（story_page.h 镜像公共前缀逐字同序同型纪律；
 *        漂移由 K2 SPV2 系列齿先红 + TEST 5 锚守卫兜底）。
 *   EQ-3 「源锚 = 运行时行为」：TEST 5/6 只证明源码具备该形状，不证明设备上真的
 *        不跳页/不清回显——真机轮 G8 承载。
 *
 * 覆盖标记（coverage marker）随 TEST_CASE 名称与首行注释逐条给出。
 *
 * Run protocol（host runner, C4.4 惯例）:
 *   0) builder 接线: 向 firmware/main/test/host_app/main/CMakeLists.txt 的
 *      HOST_ACCEPTANCE_SRCS 追加 "../../record_dialogue_guide.acceptance.c"
 *      （display/story_page.c / view_router.c 已在册，强符号覆盖 weak 哨兵；
 *      登记行**不在 acceptance-author 写权内**，TaskPackage changeManifest 预指派
 *      builder 代落——TEST 7 登记守红 = 未接线，builder FIRST ACTION）。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build
 *   2) cd firmware/main/test/host_app && MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *      ./build/host_infra_test.elf
 *   本文件亦可独立冒烟（冻结相位实编 + 设计态红取证，sc_red_wrapper 同款通道）:
 *      cd firmware/main/test/acceptance/red-baseline
 *      gcc -std=c11 -Wall -Wextra -I. -o /tmp/rdg_red_runner \
 *          ../../record_dialogue_guide.acceptance.c runner.c
 *      cd <repo root>; /tmp/rdg_red_runner      # exit code = 失败用例数
 *   结构扫描类断言支持 KIDPAL_REPO_ROOT 显式注入仓根（缺省走 walk-up 探测，仓根 /
 *   host_app / red-baseline 三处均可跑；acceptance-evidence「root 注入」纪律）。
 *
 * RED baseline（冻结相位形状；冻结落盘时实跑，逐条红名见交付报告）:
 *   设计态（净工作树：view_router 无 start_organizing、story_page 无 ORGANIZING
 *   页/rec_substate/INTERRUPT_GUIDE、application/mcp 无消费、CMakeLists 未登记）
 *   ——预期 7 个 TEST_CASE 全红，红因逐条对应当前 story 缺失的真实行为：
 *     TEST 1 -> G1 additive 数值码未实现（哨兵 -99 / 头文件锚缺席）
 *     TEST 2 -> G2 on_control case 3 未实现（哨兵）
 *     TEST 3 -> G3 引导播报中子态键位分叉未实现（哨兵）
 *     TEST 4 -> G4 整理态键位不响应未实现（哨兵）
 *     TEST 5 -> G5 story_page.h/.c additive 锚缺席（KSTORY_PAGE_ORGANIZING /
 *               rec_substate / KSPV2_ACT_INTERRUPT_GUIDE）
 *     TEST 6 -> G6 kvr 守卫 / 消费闭环 / start_organizing 消费锚缺席 +
 *               G7 recording_enter 哨兵（host runner 登记后该半转绿——先例缝在世）
 *     TEST 7 -> G8/HOST_ACCEPTANCE_SRCS 未登记（anti-vacuity；builder FIRST ACTION）
 *
 * RED baseline 增补（2026-10-21, TaskPackage 261021-p3-record-cancel-lifecycle
 * RCL-F1 落地前的设计态红面）:
 *   TEST 8 -> mcp_server.cc start_organizing 消费块零 StopListening（settle 未落,
 *             设计态红）; application.cc HandleStopListeningEvent 幂等门现状绿
 *             （must-stay-green: 既有状态门单发 listen stop——本齿防回退, 非 builder
 *             落地面）。
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: RDG-F1 -- VRT_CTRL additive 数值码（0/1/2 零偏移 + 3 新增）+ 头文件
 *             数值单源 + on_control case 3 消费与重复幂等（TEST 1 + TEST 2）
 *   coverage: RDG-F2 -- 引导播报中子态键位分叉：KEY 短/长按打断（12 + 子态复位）/
 *             BOOT 任一手势无操作（0、页态保持、零副作用）/ 常规录音既有语义
 *             回归守卫（TEST 3）
 *   coverage: RDG-F3 -- 整理过渡态：KEY/BOOT 均不响应（TEST 4）+ 页态/动作码/
 *             渲染分支 additive 锚 + 回显常驻防回退锚（TEST 5）
 *   coverage: RDG-F4 -- kvr 场景守卫（记录语境不误映 THINK）+ INTERRUPT_GUIDE
 *             消费闭环 + mcp start_organizing 消费 + recording_enter 回归守卫
 *             （TEST 6）
 *   coverage: RDG-F5 -- RM-A3 录音期提醒排队门控页面态权威锚（TEST 6）+ host
 *             runner 登记守（TEST 7，anti-vacuity 非 acceptance 条目）
 *   coverage: RDG-F6 -- **live-manual 真机轮观测项，不落自动断言、不计入 runner
 *             通过面**（G8 五项；human 持板，本 Pod 到真机门前停等）。
 *   coverage: RCL-F1 -- start_organizing 消费 settle 链 + KEY 路径幂等（261021-
 *             p3-record-cancel-lifecycle additive; TEST 8; S-F1.4-AC6 清位链运行时
 *             归真机轮）。
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

#include "../key_gesture_parser.h"

/* -------------------------------------------------------------------------- *
 * 冻结数值镜像（story_page.h / view_router.h 冻结枚举；本文件不 include 真头，
 * record_session_repair 先例——数值漂移时 TEST 1/5 锚先红）。
 * ------------------------------------------------------------------------ */

#define RDL_PAGE_LIST        0 /* KSTORY_PAGE_LIST */
#define RDL_PAGE_READER      1 /* KSTORY_PAGE_READER */
#define RDL_PAGE_RECORDING   2 /* KSTORY_PAGE_RECORDING */
#define RDL_PAGE_STAMP       3 /* KSTORY_PAGE_STAMP */
#define RDL_PAGE_DELETE      4 /* KSTORY_PAGE_DELETE */
#define RDL_PAGE_BODY_UNAVAIL 5 /* KSTORY_PAGE_BODY_UNAVAILABLE */
#define RDL_PAGE_ORGANIZING  6 /* KSTORY_PAGE_ORGANIZING（本包 additive，冻结 = 6） */

#define RDL_CTRL_END_SESSION      0 /* VRT_CTRL_END_SESSION */
#define RDL_CTRL_STOP_SPEAKING    1 /* VRT_CTRL_STOP_SPEAKING */
#define RDL_CTRL_START_RECORDING  2 /* VRT_CTRL_START_RECORDING */
#define RDL_CTRL_START_ORGANIZING 3 /* VRT_CTRL_START_ORGANIZING（本包 additive） */
#define RDL_CTRL_UNKNOWN         (-1)

#define RDL_ACT_NONE            0  /* KSPV2_ACT_NONE */
#define RDL_ACT_STOP_RECORD     10 /* KSPV2_ACT_STOP_RECORD */
#define RDL_ACT_DISCARD_RECORD  11 /* KSPV2_ACT_DISCARD_RECORD */
#define RDL_ACT_INTERRUPT_GUIDE 12 /* KSPV2_ACT_INTERRUPT_GUIDE（本包 additive） */

#define RDL_REC_SUB_NORMAL  0 /* 常规录音聆听 */
#define RDL_REC_SUB_GUIDE   1 /* 引导播报中 */

#define RDL_KEY_PAGE_STORY  2 /* KP_PAGE_STORY（M2K2-A2 冻结等值） */

/* 未实现哨兵：永不是任何整型 seam 的合法返回值。 */
#define RDL_UNIMPLEMENTED (-99)

/* kidpal_story_view_t 镜像（story_page.h 现役形状逐字同序同型 + 尾部追加
 * rec_substate；EQ-2。漂移由 K2 SPV2 系列齿先红 + TEST 5 锚守卫兜底）。 */
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
    /* [G3] 尾部追加（冻结纪律只增不改/移）：引导播报中子态标志。 */
    int rec_substate;
} kidpal_story_view_t;

/* -------------------------------------------------------------------------- *
 * 纯逻辑 seam 声明（与真头逐字同型；home = display/view_router.c +
 * display/story_page.c——host runner 强符号覆盖 weak 哨兵）。
 * ------------------------------------------------------------------------ */

int kidpal_display_control_action(const char *action);
int kidpal_story_on_control(kidpal_story_view_t *v, int ctrl_action);
int kidpal_story_page_input(kidpal_story_view_t *v, key_id_t key, key_gesture_t g);
int kidpal_story_recording_enter(int page, int ctx_active, int speaking);

/* -------------------------------------------------------------------------- *
 * Weak 哨兵（host runner 中 display/story_page.c / view_router.c 强定义覆盖）。
 * ------------------------------------------------------------------------ */

__attribute__((weak)) int kidpal_display_control_action(const char *action)
{ (void)action; return RDL_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_on_control(kidpal_story_view_t *v,
                                                  int ctrl_action)
{ (void)v; (void)ctrl_action; return RDL_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_page_input(kidpal_story_view_t *v,
                                                  key_id_t key, key_gesture_t g)
{ (void)v; (void)key; (void)g; return RDL_UNIMPLEMENTED; }
__attribute__((weak)) int kidpal_story_recording_enter(int page, int ctx_active,
                                                       int speaking)
{ (void)page; (void)ctx_active; (void)speaking; return RDL_UNIMPLEMENTED; }

/* -------------------------------------------------------------------------- *
 * 工具：失败消息 / 仓根定位 / 文件读取 / 注释剥离 / 窗内搜索（自包含复制自
 * sd_log.acceptance.c 的 sdl_* 助手族，命名前缀 rdl_；全 static）。
 * ------------------------------------------------------------------------ */

#define RDL_MAX_PATH 1024
#define RDL_MAX_READ (8u * 1024u * 1024u)

static char rdl_fm_buf[1024];
static const char *rdl_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *rdl_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(rdl_fm_buf, sizeof(rdl_fm_buf), fmt, ap);
    va_end(ap);
    return rdl_fm_buf;
}

static int rdl_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static char rdl_root[RDL_MAX_PATH];
static int rdl_root_ready;

/* 仓根定位（acceptance-evidence「root 注入」纪律）：优先显式环境变量
 * KIDPAL_REPO_ROOT，其次由相对候选点探测——不依赖某个调用 cwd 的绝对路径假设。 */
static int rdl_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[RDL_MAX_PATH];
    const char *env_root;
    size_t i;

    if (rdl_root_ready) {
        return 1;
    }
    env_root = getenv("KIDPAL_REPO_ROOT");
    if (env_root != NULL && env_root[0] != '\0') {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", env_root);
        if (rdl_is_regular(probe)) {
            snprintf(rdl_root, sizeof(rdl_root), "%s", env_root);
            rdl_root_ready = 1;
            return 1;
        }
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (rdl_is_regular(probe)) {
            snprintf(rdl_root, sizeof(rdl_root), "%s", cands[i]);
            rdl_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static char *rdl_read(const char *rel, size_t *out_len)
{
    char p[RDL_MAX_PATH * 2 + 2]; /* root(<=1024) + '/' + rel — no truncation */
    FILE *f;
    long sz;
    char *buf;

    if (!rdl_locate_root()) {
        return NULL;
    }
    snprintf(p, sizeof(p), "%s/%s", rdl_root, rel);
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > RDL_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }
    buf = (char *)malloc((size_t)sz + 1);
    assert(buf != NULL); /* logic guard: host OOM must fail loudly, not read as "red" */    if (sz > 0 && fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
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

/* 注释剥离（260917-m2-k3 sealing pass / sd_log 同款）：正向断言只被**可执行源**
 * 满足——注释字节抹成空格；字符串 / 字符字面量体保留。保持缓冲长度，调用方 free。 */
static char *rdl_strip_c_comments(const char *src)
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

static char *rdl_strip_file(const char *rel)
{
    char *buf = rdl_read(rel, NULL);
    char *out;

    if (buf == NULL) {
        return NULL;
    }
    out = rdl_strip_c_comments(buf);
    free(buf);
    return out;
}

/* 有界子串搜索（needle 在场判定）。未命中 -> NULL。 */
static const char *rdl_find_in(const char *hay, const char *needle)
{
    if (hay == NULL || needle == NULL) {
        return NULL;
    }
    return strstr(hay, needle);
}

/* 裸源子串出现计数（负向断言 / 计数锚专用——已剥注释窗上使用只计可执行源）。 */
static int rdl_count_sub(const char *hay, const char *needle)
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
static int rdl_count_lines_with_both(const char *slice, const char *needle1,
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
        char tmp[RDL_MAX_PATH];
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

#define RDL_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(rdl_locate_root(), \
    "RDG RED(infra): repo root not located from cwd — run from the repo root, " \
    "firmware/main/test/host_app or firmware/main/test/acceptance/red-baseline, " \
    "or pass KIDPAL_REPO_ROOT (see acceptance-evidence root injection rule)")

/* 哨兵守卫：-99 = seam 未实现（设计态红）；host runner 登记后转绿。 */
#define RDL_REQUIRE_IMPL_INT(v, what) \
    TEST_ASSERT_TRUE_MESSAGE((v) != RDL_UNIMPLEMENTED, \
        rdl_fm("%s RED: 冻结 seam 未实现（weak 哨兵返回 -99）——%s；builder 落地 " \
               "display 实现面并登记进 HOST_ACCEPTANCE_SRCS 后转绿", "RDG", what))

static void rdl_view_init(kidpal_story_view_t *v)
{
    memset(v, 0, sizeof(*v));
    v->page = RDL_PAGE_LIST;
    v->rec_substate = RDL_REC_SUB_NORMAL;
}

/* -------------------------------------------------------------------------- *
 * 实现面文件锚（结构齿扫描对象；相对仓根）。
 * ------------------------------------------------------------------------ */

#define RDL_VIEW_ROUTER_H "firmware/main/display/view_router.h"
#define RDL_STORY_H       "firmware/main/display/story_page.h"
#define RDL_STORY_C       "firmware/main/display/story_page.c"
#define RDL_APP_CC        "firmware/main/application.cc"
#define RDL_MCP_CC        "firmware/main/mcp_server.cc"
#define RDL_HOST_CMAKE    "firmware/main/test/host_app/main/CMakeLists.txt"

/* ========================================================================== *
 * TEST 1 — RDG-F1 VRT_CTRL additive 数值码（0/1/2 零偏移 + 3 新增 + 头文件单源）。
 * coverage: RDG-F1 -- kidpal_display_control_action 五值决策表真跑 +
 *           view_router.h 枚举锚（START_ORGANIZING = 3 同行；0/1/2 既有行零偏移）
 * RED today（设计态）: view_router.c 未扩 start_organizing（哨兵 -99）+ 头锚缺席。
 * ========================================================================== */
TEST_CASE("record guide: VRT_CTRL_START_ORGANIZING=3 additive, 0/1/2 zero-offset (RDG-F1)",
          "[record_dialogue_guide][RDG-F1][linux][behavior][structural]")
{
    char *hdr;

    /* ---- 行为：五值决策表（哨兵守卫 -> host runner 强符号） ----------------- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_CTRL_START_ORGANIZING,
        kidpal_display_control_action("start_organizing"),
        "RDG-F1 RED: kidpal_display_control_action(\"start_organizing\") 未返回 3 —— "
        "VRT_CTRL_START_ORGANIZING additive 数值码未实现（q2 = ⓒ 裁定；契约 §1.4 "
        "start_organizing 表行消费半；契约见本文件头注 [G1]）");
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_CTRL_END_SESSION,
        kidpal_display_control_action("end_session"),
        "RDG-F1 RED: end_session 数值码偏移 —— additive 纪律被破（0 不得移位）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_CTRL_STOP_SPEAKING,
        kidpal_display_control_action("stop_speaking"),
        "RDG-F1 RED: stop_speaking 数值码偏移 —— additive 纪律被破（1 不得移位）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_CTRL_START_RECORDING,
        kidpal_display_control_action("start_recording"),
        "RDG-F1 RED: start_recording 数值码偏移 —— additive 纪律被破（2 不得移位）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_CTRL_UNKNOWN,
        kidpal_display_control_action("bogus_action"),
        "RDG-F1 RED: 未知 action 未降级 VRT_CTRL_UNKNOWN —— 降级语义被破。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_CTRL_UNKNOWN, kidpal_display_control_action(NULL),
        "RDG-F1 RED: NULL action 未降级。");

    /* ---- 结构：view_router.h 枚举锚（新值在世 + 既有值零偏移） -------------- */
    RDL_REQUIRE_ROOT();
    hdr = rdl_strip_file(RDL_VIEW_ROUTER_H);
    TEST_ASSERT_NOT_NULL_MESSAGE(hdr, "RDG-F1 RED(infra): view_router.h unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        rdl_count_lines_with_both(hdr, "VRT_CTRL_START_ORGANIZING", "3") >= 1,
        "RDG-F1 RED: view_router.h 无 VRT_CTRL_START_ORGANIZING = 3 同行 —— additive "
        "枚举未落头（数值单源；实现侧用别值 = wire 语义偏移，须复核锚不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(
        rdl_count_lines_with_both(hdr, "VRT_CTRL_END_SESSION", "0") >= 1,
        "RDG-F1 RED: view_router.h VRT_CTRL_END_SESSION = 0 行偏移/缺失。");
    TEST_ASSERT_TRUE_MESSAGE(
        rdl_count_lines_with_both(hdr, "VRT_CTRL_STOP_SPEAKING", "1") >= 1,
        "RDG-F1 RED: view_router.h VRT_CTRL_STOP_SPEAKING = 1 行偏移/缺失。");
    TEST_ASSERT_TRUE_MESSAGE(
        rdl_count_lines_with_both(hdr, "VRT_CTRL_START_RECORDING", "2") >= 1,
        "RDG-F1 RED: view_router.h VRT_CTRL_START_RECORDING = 2 行偏移/缺失。");
    free(hdr);
}

/* ========================================================================== *
 * TEST 2 — RDG-F1 on_control case 3 消费（整理过渡页态 + 重复幂等 + 既有零偏移）。
 * coverage: RDG-F1 -- case 3 -> page=ORGANIZING(6)、返回 1、重复幂等；case 0/1/2/
 *           NULL/未知 既有语义零偏移（must-stay-green）
 * RED today（设计态）: story_page.c on_control 无 case 3（哨兵 -99）。
 * ========================================================================== */
TEST_CASE("record guide: on_control case 3 enters organizing page, idempotent (RDG-F1)",
          "[record_dialogue_guide][RDG-F1][linux][behavior]")
{
    kidpal_story_view_t v;

    /* ---- case 3：进整理过渡态 ------------------------------------------------ */
    rdl_view_init(&v);
    RDL_REQUIRE_IMPL_INT(kidpal_story_on_control(&v, RDL_CTRL_START_ORGANIZING),
                         "kidpal_story_on_control");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1,
        kidpal_story_on_control(&v, RDL_CTRL_START_ORGANIZING),
        "RDG-F1 RED: on_control case 3 未返回 1 —— start_organizing 消费缺失"
        "（契约 §1.4：设备切整理过渡演出；[G2]）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_PAGE_ORGANIZING, v.page,
        "RDG-F1 RED: case 3 后页态未切 KSTORY_PAGE_ORGANIZING(6) —— 整理过渡态"
        "缺席（v7 state-organizing 帧的页态载体）。");
    /* 重复幂等：网关重试下发不得翻倍副作用、页态保持。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(1,
        kidpal_story_on_control(&v, RDL_CTRL_START_ORGANIZING),
        "RDG-F1 RED: 重复 case 3 未返回 1（重复幂等）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_PAGE_ORGANIZING, v.page,
        "RDG-F1 RED: 重复 case 3 页态漂移 —— 重复幂等被破（契约 §1.4「重复幂等」）。");

    /* ---- 既有 case 0/1/2 零偏移（回归守卫） ---------------------------------- */
    rdl_view_init(&v);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_on_control(&v, RDL_CTRL_END_SESSION),
        "RDG-F1 RED: case 0（end_session）既有语义偏移（应返回 0、视图零改动）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_PAGE_LIST, v.page,
        "RDG-F1 RED: case 0 动了页态（既有：视图零改动）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, kidpal_story_on_control(&v, RDL_CTRL_STOP_SPEAKING),
        "RDG-F1 RED: case 1（stop_speaking）既有语义偏移。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1,
        kidpal_story_on_control(&v, RDL_CTRL_START_RECORDING),
        "RDG-F1 RED: case 2（start_recording）既有语义偏移（应返回 1）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_PAGE_RECORDING, v.page,
        "RDG-F1 RED: case 2 未切录音态（既有语义回退）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, v.rec_seconds,
        "RDG-F1 RED: case 2 未清零 rec_seconds（幂等重录语义回退）。");
    /* NULL / 未知 -> 降级。 */
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_on_control(NULL, RDL_CTRL_START_ORGANIZING),
        "RDG-F1 RED: NULL v 未返回 -1（降级语义被破）。");
    rdl_view_init(&v);
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1, kidpal_story_on_control(&v, 99),
        "RDG-F1 RED: 未知 ctrl_action 未返回 -1（降级语义被破）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_PAGE_LIST, v.page,
        "RDG-F1 RED: 未知 ctrl_action 动了页态。");
}

/* ========================================================================== *
 * TEST 3 — RDG-F2 引导播报中子态键位分叉（KEY 打断 / BOOT 无操作 / 常规回归守卫）。
 * coverage: RDG-F2 -- RECORDING + rec_substate=1：KEY 短/长按 -> 12（打断引导，
 *           子态复位 0、页态保持）；BOOT 短/双/长 -> 0（无操作，绝不落 DISCARD
 *           毁稿路径）；rec_substate=0 既有语义零偏移（10/11 回归守卫）
 * RED today（设计态）: story_page.c 无子态分支（哨兵 -99）。
 * ========================================================================== */
TEST_CASE("record guide: guide-playing substate keys - KEY interrupts, BOOT no-op (RDG-F2)",
          "[record_dialogue_guide][RDG-F2][linux][behavior]")
{
    kidpal_story_view_t v;

    /* ---- 引导播报中：KEY 短按 = 打断引导、继续聆听 --------------------------- */
    rdl_view_init(&v);
    v.page = RDL_PAGE_RECORDING;
    v.rec_substate = RDL_REC_SUB_GUIDE;
    RDL_REQUIRE_IMPL_INT(kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK),
                         "kidpal_story_page_input");
    /* 探针恢复：上一行哨兵守卫恒求值调用一次 [G3] 动作（rec_substate 被消费为 0），
     * 此处恢复子态后再钉行为断言——语义零改动，非 tighten-only（2026-10-19）。 */
    v.rec_substate = RDL_REC_SUB_GUIDE;
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_ACT_INTERRUPT_GUIDE,
        kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK),
        "RDG-F2 RED: 引导播报中 KEY 短按未映 KSPV2_ACT_INTERRUPT_GUIDE(12) —— 打断"
        "引导播报、继续聆听缺失（S-F2.2-AC2；[G3] 本包固件侧最关键的键位分叉）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_PAGE_RECORDING, v.page,
        "RDG-F2 RED: 打断后页态漂移 —— 引导播报中必须停留记录会话屏（不跳页）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_REC_SUB_NORMAL, v.rec_substate,
        "RDG-F2 RED: 打断后子态未复位常规录音（继续聆听语义缺失）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, v.cancel_count,
        "RDG-F2 RED: 打断引导误增 cancel_count（打断不是丢弃）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, v.archive_pend,
        "RDG-F2 RED: 打断引导误触发归档请求。");

    /* ---- 引导播报中：KEY 长按同短按（§2.1 打断停播语义） --------------------- */
    rdl_view_init(&v);
    v.page = RDL_PAGE_RECORDING;
    v.rec_substate = RDL_REC_SUB_GUIDE;
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_ACT_INTERRUPT_GUIDE,
        kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_LONG_PRESS),
        "RDG-F2 RED: 引导播报中 KEY 长按未同短按打断 —— 长按被吞 = 孩子打断手感"
        "缺失（AC2「长按同」）。");

    /* ---- 引导播报中：BOOT 任一手势 = 无操作（不拦截则 BOOT 毁稿面） ---------- */
    {
        static const key_gesture_t gestures[] = {
            KEY_GESTURE_CLICK, KEY_GESTURE_DOUBLE_CLICK, KEY_GESTURE_LONG_PRESS
        };
        size_t k;
        for (k = 0; k < sizeof(gestures) / sizeof(gestures[0]); k++) {
            rdl_view_init(&v);
            v.page = RDL_PAGE_RECORDING;
            v.rec_substate = RDL_REC_SUB_GUIDE;
            v.cancel_count = 3;
            TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_ACT_NONE,
                kidpal_story_page_input(&v, KEY_ID_BOOT, gestures[k]),
                rdl_fm("RDG-F2 RED: 引导播报中 BOOT 手势 %zu 未返回 NONE —— 子态不"
                       "拦截则 BOOT 落 DISCARD_RECORD 毁稿路径（一句话引导期间按 "
                       "BOOT 即毁稿；S-F2.2-AC2「BOOT 无操作」）", k));
            TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_PAGE_RECORDING, v.page,
                rdl_fm("RDG-F2 RED: 引导播报中 BOOT 手势 %zu 把页态打出录音态（毁稿"
                       "面）", k));
            TEST_ASSERT_EQUAL_INT_MESSAGE(3, v.cancel_count,
                rdl_fm("RDG-F2 RED: 引导播报中 BOOT 手势 %zu 增了 cancel_count（丢弃"
                       "语义误触）", k));
            TEST_ASSERT_EQUAL_INT_MESSAGE(0, v.archive_pend,
                rdl_fm("RDG-F2 RED: 引导播报中 BOOT 手势 %zu 触发了归档请求", k));
        }
    }

    /* ---- 常规录音（rec_substate=0）既有语义零偏移（回归守卫） ---------------- */
    rdl_view_init(&v);
    v.page = RDL_PAGE_RECORDING;
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_ACT_STOP_RECORD,
        kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK),
        "RDG-F2 RED: 常规录音 KEY 短按既有语义偏移（应 STOP_RECORD 保存）。");
    rdl_view_init(&v);
    v.page = RDL_PAGE_RECORDING;
    (void)kidpal_story_page_input(&v, KEY_ID_BOOT, KEY_GESTURE_CLICK);
    TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_PAGE_LIST, v.page,
        "RDG-F2 RED: 常规录音 BOOT 短按既有丢弃语义偏移（应回列表）。");
}

/* ========================================================================== *
 * TEST 4 — RDG-F3 整理过渡态键位不响应（KEY/BOOT 均不响应，非交互过渡态）。
 * coverage: RDG-F3 -- page=ORGANIZING：KEY/BOOT 任意手势 -> NONE、页态保持、
 *           零副作用（S-F2.2-AC4 / §2.3 非交互过渡态口径）
 * RED today（设计态）: story_page.c 无 ORGANIZING 页 case（哨兵 -99）。
 * ========================================================================== */
TEST_CASE("record guide: organizing page swallows all keys with zero side effects (RDG-F3)",
          "[record_dialogue_guide][RDG-F3][linux][behavior]")
{
    static const key_id_t keys[] = { KEY_ID_KEY, KEY_ID_BOOT };
    static const key_gesture_t gestures[] = {
        KEY_GESTURE_CLICK, KEY_GESTURE_DOUBLE_CLICK, KEY_GESTURE_LONG_PRESS
    };
    kidpal_story_view_t v;
    size_t k, g;

    RDL_REQUIRE_IMPL_INT(kidpal_story_page_input(&v, KEY_ID_KEY, KEY_GESTURE_CLICK),
                         "kidpal_story_page_input");
    for (k = 0; k < sizeof(keys) / sizeof(keys[0]); k++) {
        for (g = 0; g < sizeof(gestures) / sizeof(gestures[0]); g++) {
            rdl_view_init(&v);
            v.page = RDL_PAGE_ORGANIZING;
            v.cancel_count = 7;
            TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_ACT_NONE,
                kidpal_story_page_input(&v, keys[k], gestures[g]),
                rdl_fm("RDG-F3 RED: 整理过渡态 key=%zu gesture=%zu 未返回 NONE —— "
                       "整理中按键响应 = 非交互过渡态被破（S-F2.2-AC4「整理中 "
                       "KEY/BOOT 均不响应」；误触会打断整理演出或误触毁稿面）",
                       (size_t)keys[k], g));
            TEST_ASSERT_EQUAL_INT_MESSAGE(RDL_PAGE_ORGANIZING, v.page,
                rdl_fm("RDG-F3 RED: 整理过渡态 key=%zu gesture=%zu 把页态打出整理态",
                       (size_t)keys[k], g));
            TEST_ASSERT_EQUAL_INT_MESSAGE(7, v.cancel_count,
                rdl_fm("RDG-F3 RED: 整理过渡态 key=%zu gesture=%zu 误增 cancel_count",
                       (size_t)keys[k], g));
            TEST_ASSERT_EQUAL_INT_MESSAGE(0, v.archive_pend,
                rdl_fm("RDG-F3 RED: 整理过渡态 key=%zu gesture=%zu 误触归档请求",
                       (size_t)keys[k], g));
        }
    }
}

/* ========================================================================== *
 * TEST 5 — RDG-F3 页态/动作码/渲染分支 additive 锚 + 回显常驻防回退锚（结构齿）。
 * coverage: RDG-F3 -- story_page.h：KSTORY_PAGE_ORGANIZING = 6 / rec_substate
 *           声明 / KSPV2_ACT_INTERRUPT_GUIDE = 12 同行锚；story_page.c：
 *           ORGANIZING 引用 >= 2（输入 case + 渲染 case）+ 引导子态渲染分支
 *           （rec_substate + 「打断」键帽字面）+ kidpal_story_echo_reset( 计数
 *           == 1（回显常驻防回退）+ kidpal_story_echo_line( 渲染消费在世
 * RED today（设计态）: story_page.h/.c additive 锚全缺席。
 * ========================================================================== */
TEST_CASE("record guide: story_page.h/.c additive anchors and echo persistence (RDG-F3)",
          "[record_dialogue_guide][RDG-F3][structural]")
{
    char *hdr;
    char *code;
    int n;

    RDL_REQUIRE_ROOT();

    /* ---- story_page.h：additive 锚（数值冻结单源） --------------------------- */
    hdr = rdl_strip_file(RDL_STORY_H);
    TEST_ASSERT_NOT_NULL_MESSAGE(hdr, "RDG-F3 RED(infra): story_page.h unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        rdl_count_lines_with_both(hdr, "KSTORY_PAGE_ORGANIZING", "6") >= 1,
        "RDG-F3 RED: story_page.h 无 KSTORY_PAGE_ORGANIZING = 6 同行 —— 整理过渡"
        "页态枚举未落头（additive 尾部 = 6；实现侧用别值 = 与验收镜像 ABI 漂移，"
        "须复核锚不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(
        rdl_count_lines_with_both(hdr, "KSPV2_ACT_INTERRUPT_GUIDE", "12") >= 1,
        "RDG-F3 RED: story_page.h 无 KSPV2_ACT_INTERRUPT_GUIDE = 12 同行 —— 打断"
        "引导动作码未落头（additive 尾部 = 12）");
    TEST_ASSERT_TRUE_MESSAGE(
        rdl_count_sub(hdr, "rec_substate") >= 1,
        "RDG-F3 RED: story_page.h 无 rec_substate 字段声明 —— 引导播报中子态载体"
        "缺席（[G3] 冻结名；镜像 ABI 依据）");
    free(hdr);

    /* ---- story_page.c：渲染分支 + 回显常驻 ----------------------------------- */
    code = rdl_strip_file(RDL_STORY_C);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "RDG-F3 RED(infra): story_page.c unreadable");
    n = rdl_count_sub(code, "KSTORY_PAGE_ORGANIZING");
    TEST_ASSERT_TRUE_MESSAGE(
        n >= 2,
        rdl_fm("RDG-F3 RED: story_page.c 内 KSTORY_PAGE_ORGANIZING 引用 %d 处（应 "
               ">= 2：输入 case + 渲染 case）—— 整理过渡态渲染分支缺席（页态切了"
               "但渲染无人消费 = 黑屏过渡）", n));
    n = rdl_count_sub(code, "打断");
    TEST_ASSERT_TRUE_MESSAGE(
        n >= 1,
        "RDG-F3 RED: story_page.c 内「打断」键帽字面缺席 —— 引导播报中子态"
        "帧 B 键帽左「打断」右留空未渲染（设计稿 p3-recording-guide 帧 B）");
    n = rdl_count_sub(code, "rec_substate");
    TEST_ASSERT_TRUE_MESSAGE(
        n >= 2,
        rdl_fm("RDG-F3 RED: story_page.c 内 rec_substate 引用 %d 处（应 >= 2：输入"
               "映射 + 渲染分支）—— 子态只切不渲染 / 只渲染不响应皆红", n));
    /* 回显常驻（S-F2.2-AC2「回显区常驻」防回退锚）：echo_reset 只允许出现 1 次
     * （定义处）——引导子态 / 整理过渡分支清空回显区 = 孩子讲过的话从屏上消失。 */
    n = rdl_count_sub(code, "kidpal_story_echo_reset(");
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        1, n,
        rdl_fm("RDG-F3 RED: story_page.c 内 kidpal_story_echo_reset( 出现 %d 次"
               "（应恰 1 = 仅定义处）—— 渲染/子态分支清空了话语回显区，AC2「回显区"
               "常驻」被破", n));
    n = rdl_count_sub(code, "kidpal_story_echo_line(");
    TEST_ASSERT_TRUE_MESSAGE(
        n >= 2,
        rdl_fm("RDG-F3 RED: story_page.c 内 kidpal_story_echo_line( 消费 %d 处（应 "
               ">= 2：定义 + 渲染消费）—— 回显渲染面缺席（回显常驻无承载）", n));
    free(code);
}

/* ========================================================================== *
 * TEST 6 — RDG-F4/F5 kvr 场景守卫 + 消费闭环 + start_organizing 消费 +
 *          RM-A3 门控页面态权威 + recording_enter 回归守卫。
 * coverage: RDG-F4 -- application.cc：stt-final 回调内 kidpal_story_session_ctx_
 *           active 引用先于 kidpal_voice_round_scene（记录语境不误映 THINK）+
 *           KSPV2_ACT_INTERRUPT_GUIDE 消费闭环；mcp_server.cc：VRT_CTRL_START_
 *           ORGANIZING 消费 + story_consumed 日志同型；RDG-F5 -- mcp_server.cc
 *           RM-A3 门控 g_story_view.page == KSTORY_PAGE_RECORDING 页面态权威锚 +
 *           recording_enter 决策表回归守卫（既有缝，host runner 转绿半）
 * RED today（设计态）: application/mcp 消费锚缺席 + recording_enter 哨兵。
 * ========================================================================== */
TEST_CASE("record guide: kvr ctx guard, consumption closure, reminder gate anchor (RDG-F4/F5)",
          "[record_dialogue_guide][RDG-F4][RDG-F5][structural][behavior]")
{
    char *app;
    char *mcp;
    const char *p_stt;
    const char *p_ctx;
    const char *p_scene;
    const char *win_start;
    const char *win_end;

    RDL_REQUIRE_ROOT();

    /* ---- application.cc：kvr ctx 选取联挂记录语境标志（顺序锚） --------------- */
    app = rdl_strip_file(RDL_APP_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(app, "RDG-F4 RED(infra): application.cc unreadable");
    p_stt = rdl_find_in(app, "KVR_EVT_STT_FINAL");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_stt,
        "RDG-F4 RED(infra): application.cc 无 KVR_EVT_STT_FINAL —— stt-final 消费回调"
        "锚缺席（既有形态被改写？须复核锚不得改判为绿）");
    /* 回调窗：stt-final 锚前 1200 / 后 800 字节（ctx 选取与场景映射同回调）。 */
    win_start = app + ((p_stt - app) > 1200 ? (size_t)(p_stt - app) - 1200 : 0);
    win_end = p_stt + 800;
    if ((size_t)(win_end - app) > strlen(app)) {
        win_end = app + strlen(app);
    }
    TEST_ASSERT_TRUE_MESSAGE(
        (size_t)(win_end - win_start) > 400,
        "RDG-F4 RED(infra): stt-final 回调窗切片异常——扫描器窗退化");
    {
        char *win = (char *)malloc((size_t)(win_end - win_start) + 1);
        assert(win != NULL); /* logic guard: host OOM must fail loudly */
        memcpy(win, win_start, (size_t)(win_end - win_start));
        win[win_end - win_start] = '\0';
        p_ctx = rdl_find_in(win, "kidpal_story_session_ctx_active");
        p_scene = rdl_find_in(win, "kidpal_voice_round_scene");
        TEST_ASSERT_NOT_NULL_MESSAGE(
            p_ctx,
            "RDG-F4 RED: stt-final 回调内无记录语境标志引用 —— kvr ctx 选取未联挂"
            "记录语境（切逐句 stt 形态后每句 stt 误映 KVST_SCENE_THINK -> SetStatus"
            "(THINKING) 跳页，S-F2.2-AC1「切句只回显不进思考」被破；[G6]）");
        TEST_ASSERT_NOT_NULL_MESSAGE(
            p_scene,
            "RDG-F4 RED(infra): stt-final 回调内无 kidpal_voice_round_scene 调用"
            "（既有场景映射锚漂移，须复核锚不得改判为绿）");
        TEST_ASSERT_TRUE_MESSAGE(
            p_ctx < p_scene,
            "RDG-F4 RED: 记录语境标志引用晚于场景映射 —— ctx 选取必须在映射前分流"
            "（先分流后映射，否则 THINK 映射先发生）");
        free(win);
    }
    /* 打断引导动作码消费闭环（驱动方 <-> 消费方，防死代码）。 */
    {
        char *mcp_probe = rdl_strip_file(RDL_MCP_CC);
        int app_hits = rdl_count_sub(app, "KSPV2_ACT_INTERRUPT_GUIDE");
        int mcp_hits = (mcp_probe != NULL) ? rdl_count_sub(mcp_probe,
                                                            "KSPV2_ACT_INTERRUPT_GUIDE")
                                           : 0;
        free(mcp_probe);
        TEST_ASSERT_TRUE_MESSAGE(
            (app_hits + mcp_hits) >= 1,
            "RDG-F4 RED: KSPV2_ACT_INTERRUPT_GUIDE 无消费点（application.cc / "
            "mcp_server.cc 均未引用）—— 打断动作码死代码 = 引导播报停不下来"
            "（AbortSpeaking 链缺失）");
    }

    /* ---- mcp_server.cc：start_organizing 消费 + RM-A3 页面态权威锚 ------------ */
    mcp = rdl_strip_file(RDL_MCP_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(mcp, "RDG-F4 RED(infra): mcp_server.cc unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        rdl_count_sub(mcp, "VRT_CTRL_START_ORGANIZING") >= 1,
        "RDG-F4 RED: mcp_server.cc 未消费 VRT_CTRL_START_ORGANIZING —— 控制 switch "
        "case 3 缺席（start_organizing 下发后设备无消费 = 收尾演出永不出现）");
    TEST_ASSERT_TRUE_MESSAGE(
        rdl_count_sub(mcp, "control start_organizing story_consumed") >= 1,
        "RDG-F4 RED: mcp_server.cc 无「control start_organizing story_consumed」日志"
        "同型 —— case 3 消费位日志锚缺席（既有 case 2 日志同型，K6 归因面）");
    TEST_ASSERT_TRUE_MESSAGE(
        rdl_count_sub(mcp, "g_story_view.page == KSTORY_PAGE_RECORDING") >= 1,
        "RDG-F5 RED: mcp_server.cc RM-A3 门控未按页面态判定（g_story_view.page == "
        "KSTORY_PAGE_RECORDING 判据缺席）—— 录音期提醒排队延后判据随聆听形态切换"
        "漂移 = S-F1.4-AC6 回归（引导播报中子态页态仍 RECORDING 自动 defer；判据"
        "必须保持页面态权威）");
    free(app);
    free(mcp);

    /* ---- recording_enter 既有缝回归守卫（[G7]；host runner 登记后转绿半） ----- */
    TEST_ASSERT_EQUAL_INT_MESSAGE(1,
        kidpal_story_recording_enter(RDL_PAGE_LIST, 1, 1),
        "RDG-F4 RED: recording_enter(LIST, ctx=1, speaking=1) 未返回 1 —— 记录语境"
        "开口切录音态决策表偏移（既有 [S2] 缝回归）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_story_recording_enter(RDL_PAGE_LIST, 0, 1),
        "RDG-F4 RED: 非记录语境开口误切录音态（决策表 ctx 门偏移）。");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
        kidpal_story_recording_enter(RDL_PAGE_READER, 1, 1),
        "RDG-F4 RED: 阅读态开口误切录音态（决策表页态门偏移）。");
}

/* ========================================================================== *
 * TEST 7 — host runner 登记守（anti-vacuity；非 acceptance 条目）。
 * coverage: RDG-F5（登记半）-- HOST_ACCEPTANCE_SRCS 含本文件 = 全部断言真实执行
 * RED today（设计态）: 未登记（builder FIRST ACTION）。
 * ========================================================================== */
TEST_CASE("record guide: acceptance file registered into host runner (RDG-F5 guard)",
          "[record_dialogue_guide][RDG-F5][structural]")
{
    char *cmake;

    RDL_REQUIRE_ROOT();
    cmake = rdl_read(RDL_HOST_CMAKE, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(cmake,
        "RDG-F5 RED(infra): host_app/main/CMakeLists.txt unreadable");
    TEST_ASSERT_TRUE_MESSAGE(
        strstr(cmake, "record_dialogue_guide.acceptance.c") != NULL,
        "RDG-F5 RED: host_app/main/CMakeLists.txt 未登记 record_dialogue_guide."
        "acceptance.c —— 本冻结验收件不进 host runner（全部断言不会被执行 = 空过绿；"
        "builder FIRST ACTION，见头注 Run protocol 0）");
    free(cmake);
}

/* ========================================================================== *
 * TEST 8 — RCL-F1（261021-p3-record-cancel-lifecycle）: start_organizing 消费
 *          settle 链 + KEY 路径幂等（源锚结构齿; 261019 起的 case 3 消费位补
 *          app.StopListening() 事件位投递——KSPV2_ACT_STOP_RECORD :687 同款形态,
 *          plan F4 修位; Idle 清位链复用 application.cc 既有零改）。
 * RED today: mcp_server.cc case 3 零 StopListening（settle 未落——design-state red）;
 *          application.cc 幂等门现状绿（must-stay-green: HandleStopListeningEvent
 *          既有状态门 = 已 Idle 重复 settle no-op、不重发 listen stop 的机制载体）。
 * ========================================================================== */
/* coverage: RCL-F1 -- start_organizing 消费沿补 StopListening settle（case 3 块内接线锚）+ KEY 路径幂等（HandleStopListeningEvent 状态门单发 listen stop 锚, 已 Idle 重复 settle no-op 不重发） */
/* coverage: S-F1.4-AC6 -- settle 后 Idle 分支既有清位链接管（[R2] 页态复位 + ctx_set(0) + RaiseReminderQueue「会话结束立刻弹」）——运行时归真机轮, 本齿只钉事件位投递与幂等门在世 */
TEST_CASE("record guide: start_organizing settle + KEY idempotent stop path (RCL-F1)",
          "[record_dialogue_guide][RCL-F1][structural]")
{
    char *mcp;
    char *app;
    const char *p_case3;
    const char *p_case4;
    const char *p_vrt;
    char *case3;
    size_t span;
    const char *p_send;
    const char *p_gate;

    RDL_REQUIRE_ROOT();

    /* ---- mcp_server.cc: case 3（start_organizing 消费块）内 StopListening settle */
    mcp = rdl_strip_file(RDL_MCP_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(mcp, "RCL-F1 RED(infra): mcp_server.cc unreadable");
    p_case3 = strstr(mcp, "case 3:");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_case3,
        "RCL-F1 RED(infra): mcp_server.cc 无 case 3 消费位（既有 start_organizing "
        "消费块被移? 须复核本齿锚, 不属 builder 修订面）");
    p_case4 = strstr(p_case3 + 7, "case 4:");
    if (p_case4 == NULL) {
        p_case4 = mcp + strlen(mcp);
    }
    span = (size_t)(p_case4 - p_case3);
    case3 = (char *)malloc(span + 1);
    assert(case3 != NULL); /* logic guard: host OOM must fail loudly, not read as "red" */
    memcpy(case3, p_case3, span);
    case3[span] = '\0';
    p_vrt = strstr(case3, "VRT_CTRL_START_ORGANIZING");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_vrt,
        "RCL-F1 RED(infra): case 3 切片内无 VRT_CTRL_START_ORGANIZING —— 切片锚失真"
        "（case 3/4 锚漂移, 须复核本齿锚不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(
        strstr(case3, "StopListening(") != NULL,
        "RCL-F1 RED: mcp_server.cc start_organizing 消费块零 StopListening —— "
        "server 驱动收尾 settle 缺席（语音/自动收尾路径 front 检出时设备仍在 "
        "Listening, ctx 停死 1 -> show_home 守卫拦截 = 列表页长按六连拦, 根因④; "
        "plan F4: 消费沿补 app.StopListening() 事件位投递, 与 KSPV2_ACT_STOP_RECORD "
        "同款形态; Idle 清位链接管 = [R2] 复位 + ctx_set(0) + RaiseReminderQueue）");
    free(case3);
    free(mcp);

    /* ---- application.cc: KEY 路径幂等（状态门单发 listen stop; must-stay-green） */
    app = rdl_strip_file(RDL_APP_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(app, "RCL-F1 RED(infra): application.cc unreadable");
    {
        const char *fn = strstr(app, "void Application::HandleStopListeningEvent");
        const char *fn_end;
        char *region;
        TEST_ASSERT_NOT_NULL_MESSAGE(fn,
            "RCL-F1 RED(infra): application.cc 无 HandleStopListeningEvent —— "
            "settle 幂等门载体缺席（既有形态被改写? 须复核锚不得改判为绿）");
        fn_end = strstr(fn, "void Application::HandleWakeWordDetectedEvent");
        if (fn_end == NULL) {
            fn_end = app + strlen(app);
        }
        span = (size_t)(fn_end - fn);
        region = (char *)malloc(span + 1);
        assert(region != NULL); /* logic guard: host OOM must fail loudly */
        memcpy(region, fn, span);
        region[span] = '\0';
        /* 幂等机制 = 状态门: listen stop 只在 kDeviceStateListening 分支单发
         * （KEY 路径设备已先自行 Idle, settle 重复见 state != Listening 即 no-op、
         * 不重发 listen stop——RCL-F1「KEY 路径幂等」的机制载体; 行为半归真机轮）。 */
        TEST_ASSERT_EQUAL_INT_MESSAGE(
            1, rdl_count_sub(region, "SendStopListening("),
            "RCL-F1 RED: HandleStopListeningEvent 区内 SendStopListening 出现次数 "
            "!= 1 —— listen stop 多点散射/状态门被破（幂等失效 = 重复 settle 重发"
            "归档触发面, listen stop = front stop 归档语义, 双发即双归档沿）。");
        p_gate = strstr(region, "kDeviceStateListening");
        p_send = strstr(region, "SendStopListening(");
        TEST_ASSERT_TRUE_MESSAGE(
            p_gate != NULL && p_send != NULL && p_gate < p_send,
            "RCL-F1 RED: listen stop 发送点不在 kDeviceStateListening 状态门之后 "
            "—— KEY 路径幂等机制失效（已 Idle 的重复 settle 不得重发 listen stop, "
            "plan F4 幂等条款）。");
        free(region);
    }
    free(app);
}

/* ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-10-21 additive（TaskPackage 261021-p3-record-cancel-lifecycle RCL-F1）:
 *               新增 TEST 8——start_organizing 消费沿 settle 链（mcp_server.cc case 3
 *               块内 StopListening 接线锚; plan F4 修位, 与 KSPV2_ACT_STOP_RECORD
 *               同款形态）+ KEY 路径幂等锚（application.cc HandleStopListeningEvent
 *               状态门单发 listen stop——已 Idle 重复 settle no-op 的机制载体,
 *               must-stay-green 防回退; 行为半归真机轮）。既有 TEST 1-7 断言集合
 *               零变化; 授权 = reviewed plan.md F4 修位表 + taskpackage acceptance
 *               RCL-F1（冻结边界）。设计态红面追加 TEST 8 settle 半。
 *   2026-10-19 初版冻结（TaskPackage 261019-p3-record-dialogue-guide; acceptance[]
 *               RDG-F1..F6 为冻结边界; requirements v1.65 S-F2.2/NFR-PERF-3 +
 *               FROZEN PROPOSAL ADR-013 + reviewed plan S3/S4 为语义权威）。
 *               七个 TEST_CASE：RDG-F1 行为两件（VRT additive 决策表 + 头锚 /
 *               on_control case 3 + 幂等 + 既有零偏移）、RDG-F2 行为一件（引导
 *               播报中子态键位分叉 + 常规回归守卫）、RDG-F3 行为一件（整理态键位
 *               不响应）+ 结构一件（页态/动作码/渲染分支锚 + 回显常驻防回退锚）、
 *               RDG-F4/F5 结构一件（kvr 守卫 + 消费闭环 + RM-A3 页面态权威锚）
 *               + recording_enter 回归守卫 + 登记守（anti-vacuity）。RDG-F6 为
 *               live-manual 真机轮观测项，不落自动断言（G8 五项），本 Pod 到真机
 *               门前停等 human。冻结落盘冒烟 = 独立冒烟通道实编 + 实跑（设计态红
 *               逐条核对，红名见交付报告）+ 既有 host 全量套件零新增红判读。
 *   2026-10-19 TEST3（RDG-F2）探针缺陷最小复钉（语义零改动，非 tighten-only；builder
 *               停点申报：实现落地后 TEST3「Expected 12 Was 10」）——:657 哨兵守卫
 *               宏 RDL_REQUIRE_IMPL_INT 恒求值条件，探针真实执行一次 [G3] KEY 短按
 *               调用并把 rec_substate 消费为 0；:659 行为断言第二次同视图调用落入
 *               常规录音路径返 10。冻结期红因 = 哨兵 -99 时探针 abort、双调用流未
 *               被观察到（盲点）。修法 = 探针后、断言前重设 v.rec_substate =
 *               RDL_REC_SUB_GUIDE（恢复被探针消费的子态）；[G3] 语义、:659 断言期
 *               望值、全部断言集合零变化。
 * --------------------------------------------------------------------------- */
