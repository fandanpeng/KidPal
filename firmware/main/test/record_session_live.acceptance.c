/*
 * Frozen acceptance tests — TaskPackage 260924-m4-k3-record-session-live (subsystem:
 * firmware; mergeOrder 40; rigor=full / riskTier=high; dependsOn
 * 260924-m4-k1-story-cloud-link + 260924-m4-k2-p3-star-notebook-ui 均已 merged)。
 *
 * 承载 story = S-F2.2（开录双入口：AC1 技术信号发送半 + 引导态 / AC2 切句回显板端半 /
 * AC4 录音手势兜底收尾与丢弃）+ 跨包承接（defer 包 260928-story-connect-push-defer
 * 三时点①恢复重裁的板端 gating 半 + K2 删除发送原语同缝扩展）。
 * Story 语义权威 = docs/requirements.md S-F2.2（AC1–AC4 整节照录见 input-brief
 * ①b 投影）+ §2.3 矩阵「记录会话 · 引导态」/「录音态」两行。契约形状权威 =
 * docs/contracts/interface-contract.md §四（kidpal_session_ctx 端→网关私有扩展消息：
 * {"type":"kidpal_session_ctx","source_page":"story"}，首帧音频前、server hello 之后，
 * 通道复用会话同样发送，重复发送安全，省略 = 普通会话向后兼容；本包只落实现、契约
 * 零改动）。
 *
 * 跨包负锚（不得冲突）：FIX-A1（260928-story-connect-push-defer，human 方向锚
 * b036f554）= WS 建连路径零 story_list 下行帧（server 侧冻齿
 * test_m4k1_story_cloud_link.acceptance.py::test_fix_a1_connect_path_emits_zero_story_list
 * 在世）。本包推送相关断言全部钉在 ctx 触发时点（front 条件推送 + 板端消费 gating），
 * 与建连路径零帧负锚天然不交。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；「home =」为冻结落点）:
 *
 *   [S1] source_page 映射单点（home = display/story_page.{c,h} additive 尾部追加;
 *        契约 §四 source_page 闭集 story/home/schedule「其余取值现版本无行为」+ notes
 *        ④ 映射散落 = 漏入口根因，单点函数一次做全）:
 *          const char *kidpal_story_ctx_source_page(int page);
 *            -- page == 2 (KP_PAGE_STORY) -> "story"; page == 1 (KP_PAGE_SCHEDULE)
 *               -> "schedule"; page == 0 (KP_PAGE_HOME) -> "home"; 其余（负值/未知）
 *               -> "home"（闭集缺省兜底, 绝不外溢第四取值）。返回静态串字面指针,
 *               永不 NULL。
 *            镜像锚: KP_PAGE_* 数值 = retract_p4_three_page.acceptance.c M2K2-A2
 *            冻结（HOME=0/SCHEDULE=1/STORY=2）。本文件不 include kidpal_render_node.h
 *            （设备专属头、host 不编）, 以数值镜像 + 既有跨件枚举齿承载等值——
 *            kidpal_render_node.h 枚举值漂移时 M2K2-A2 先红, 本文件行为齿随之红。
 *
 *   [S2] 引导态纯逻辑二缝（home = display/story_page.{c,h}; S-F2.2 AC1 记录语境引导
 *        + §2.3「记录会话 · 引导态」行; 底座 KVR_CTX_LISTEN_MANUAL + WF-B-01 消费,
 *        M1 冻结映射 kidpal_voice_states.c 零触碰）:
 *          const char *kidpal_story_guide_bubble(void);
 *            -- 引导态气泡逐字单源:「需要我帮你记点什么？」（AC1 逐字; lcd_display.cc
 *               气泡覆写缝消费此单源——写点 g_kidpal_voice_header_label 区, 结构齿
 *               [S6]; 文案不落 M1 冻结映射, 不双写）。
 *          int kidpal_story_recording_enter(int page, int ctx_active, int speaking);
 *            -- 开口切录音态决策（notes ⑨ UTT_START 事件沿的纯逻辑承载）:
 *               ctx_active != 0 && page == KSTORY_PAGE_LIST(0) && speaking != 0
 *               -> 1（调用方把 story 页切 KSTORY_PAGE_RECORDING）; 其余 -> 0。
 *               幂等: 已在录音态（page==2）不重切; 非记录语境（唤醒词普通会话）
 *               不切; 越界 page 值 -> 0（防御）。
 *
 *   [S3] story 录音态话语回显纯逻辑缝（home = display/story_page.{c,h} 尾部追加;
 *        S-F2.2 AC2「切句只用于话语回显」板端半; 参照 voice_round.c
 *        kidpal_voice_echo_* 行数模型、住 story_page 防触碰 M1 冻结面——新缝与
 *        kidpal_voice_echo_* 零共享态、零重命名）:
 *          #define KSTORY_ECHO_MAX_LINES 4   （冻结值; 行数机理恒 FIFO 滚出, 行高
 *                                              视觉拟合归真机轮——强度申报见头注）
 *          int kidpal_story_echo_reset(void);            -- 清零; 返回 0
 *          int kidpal_story_echo_push(const char *text); -- 逐句入列, 满容量挤最旧
 *                                                           （FIFO）; 返回当前句数;
 *                                                           NULL / 空串 / 全空白 -> -1
 *          int kidpal_story_echo_count(void);            -- 当前句数
 *          int kidpal_story_echo_line(int idx, const char **out);
 *            -- idx 0 = 最旧; 越界 / out NULL -> -1; 命中 -> 0 且 *out 指向该句。
 *
 *   [S4] story_list 推送 gating 纯逻辑（home = display/story_page.{c,h}; defer 包
 *        ①恢复重裁 plan T5 的板端消费半——「ctx 时点 vs 录音引导态」冲突分工解法:
 *        front 推、板端记录语境前台只装缓存不切前台）:
 *          int kidpal_story_list_gate(int voice_front, int page_state);
 *            -- voice_front != 0（VOICE 层前台——引导态/录音态/问答会话）
 *               || page_state == KSTORY_PAGE_RECORDING(2) -> 1（仅装 g_story_* 缓存,
 *               绝不 begin_page 推前台——防 story_list 帧盖掉引导/录音演出, defer 包
 *               真机缺陷形态的板端防护半）; 其余 -> 0（推前台, K2 现状零偏移——
 *               LIST 冷浏览 / READER / STAMP / DELETE 照旧渲染）。
 *
 *   [S5] 录音手势会话接线（消费位 = mcp_server.cc StoryDispatchAct STOP/DISCARD
 *        case 拆分; K2 冻结语义位零动——story_page.h:108-126 动作码闭集不改）:
 *          KSPV2_ACT_STOP_RECORD（KEY 短按 & 长按同义, v1.47 防毁稿）-> 收尾保存 =
 *            调 Application StopListening 面（MAIN_EVENT_STOP_LISTENING ->
 *            SendStopListening + Idle; front _story_stop_on_listen_stop 消费
 *            listen/state=stop 触发 stop 归档 + story_text/story_list 下发）。
 *            **负锚（notes (e), 两条 front 路径不混的板端根）**: STOP case 内严禁
 *            CloseAudioChannel / ToggleChatState——ToggleChatState Listening 分支
 *            直接 CloseAudioChannel 不发 listen stop -> front 走 ws_disconnect
 *            cancel 丢弃路径而非归档, 毁稿。
 *          KSPV2_ACT_DISCARD_RECORD（BOOT 任一手势, §2.3 录音态行）-> 丢弃 =
 *            CloseAudioChannel 即达 front 断开钩子 acc.cancel(session_id,
 *            reason="ws_disconnect")（弹缓冲不落盘）; case 内严禁 StopListening
 *            （丢弃不是归档——语义反转负锚, 旧冻结表长按=丢弃已退役）。
 *          KEY 长按同短按保存 = K2 输入表已冻（STOP_RECORD 双手势同码）, 本文件
 *          只钉消费位动作映射, 不重钉输入表。
 *
 *   [S6] 引导态/回显接线结构齿（needle 面; 行为半由 [S2]/[S3] host 机判）:
 *          mcp_server.cc KSPV2_ACT_REC_SESSION 消费位（现 log-only 桩）接会话发起:
 *            case 区内 "kidpal_render_node_show_chat"（推语音层前台, GLOBAL_ACT_
 *            SESSION_TOGGLE 先例）+ "StartListening("（manual 聆听入口 ->
 *            SetListeningMode(kListeningModeManualStop) -> Listening 分支 -> [S7]
 *            发送点; 源锚代理——源锚代理强度申报见头注）。
 *          lcd_display.cc kidpal_voice_apply_look 函数区内消费
 *            "kidpal_story_guide_bubble"（记录语境覆写缝——LOOK 单源 [S2], 不落
 *            M1 冻结映射第二写点）。
 *          application.cc stt 分支区（"stt" .. "llm" strcmp 切片）内
 *            "kidpal_story_echo_push"（记录语境旁路喂入, 与 front
 *            _story_feed_from_stt 旁路同构、不改 wire）。
 *          application.cc 含 "kidpal_story_recording_enter("（VAD 事件沿消费接线;
 *            MAIN_EVENT_VAD_CHANGE 注册在世 = must-stay-green）。
 *
 *   [S7] session_ctx 发送原语 + 发送时点（home = protocols/protocol.{h,cc} 原语 +
 *        application.cc 发送点; 逐字仿 SendAbortSpeaking / SendKidpalStoryDelete
 *        形态, K2 发送缝同族扩展零新机制; 契约 §四「首帧音频前（server hello 之后）
 *        + 通道复用会话同样发送 + 重复发送安全」）:
 *          protocol.h: virtual void SendKidpalSessionCtx(const char* source_page);
 *          protocol.cc: 拼 {"session_id":"...","type":"kidpal_session_ctx",
 *                          "source_page":"..."} 走 SendText（无协议 id 分配面）。
 *          websocket_protocol.{h,cc} 零改动（SendText 虚实现自动生效, K2 delete
 *          原语无 override 先例）。
 *          发送点 = application.cc HandleStateChangedEvent Listening 分支内、
 *            SendStartListening 之前、IsAudioProcessorRunning 门**之外**无条件执行
 *            ——门外执行 = 通道复用会话（processor 在跑）不短路漏发（notes ②③）;
 *            发送失败降级普通会话不重试（notes ③ 负锚——调用恰一次, 无重试环）。
 *          **负锚**: HandleToggleChatEvent 函数区零 SendKidpalSessionCtx（会话收尾/
 *            中断路径不发 ctx——ctx 是会话发起信号）。
 *          source_page 单点消费: Listening 分支调 kidpal_story_ctx_source_page(
 *            kidpal_render_node_get_page())——映射散落 = 漏入口根因（notes ④）;
 *            KEY 按（K2 前置分流 -> REC_SESSION 消费位 -> StartListening）与唤醒词
 *            （WakeWordInvoke）双入口都汇入 Listening 分支 = 单点覆盖双入口。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: M4K3-A1 session_ctx 发送缝（契约 §四 + S-F2.2-AC1 技术信号板端半 +
 *             notes ①②③④）-> RSL-A1（原语四件套 + wire 形状）+ RSL-A2（发送时点/
 *             双入口单点/负锚）+ RSL-A3（source_page 闭集映射）
 *   coverage: M4K3-A2 引导态（S-F2.2-AC1 屏显引导半 + §2.3 引导态行）-> RSL-A4
 *   coverage: M4K3-A3 录音手势接线（S-F2.2-AC4 KEY 兜底收尾/BOOT 丢弃 + AC2 停录
 *             保存链板端半 + notes (e) 两 front 路径不混负锚）-> RSL-A5
 *   coverage: M4K3-A5 回显消费（S-F2.2-AC2 切句只用于话语回显板端半）-> RSL-A6
 *   coverage: M4K3-A6 story_list 推送 gating 板端半（defer ①重裁承接 + plan T5）
 *             -> RSL-A7
 *   coverage: M4K3-A9 零偏移与注册守卫（K2 语义位/K1-K3 契约面/spec canary/注册序）
 *             -> RSL-C1
 *   （M4K3-A4 front 双路径守卫 / M4K3-A7 ctx 条件推送 front 半 / M4K3-A8 prompt v8
 *    = server/acceptance/test_m4k3_record_session_live.acceptance.py 承载;
 *    M4K3-A10 = dropped 显式取舍, 见 taskpackage decisionCoverage。）
 *
 * ---------------------------------------------------------------------------
 * Run protocol:
 *   0) builder FIRST ACTION: 本文件注册进
 *      firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS
 *      （相对 host_app/main 为 "../../record_session_live.acceptance.c"），**置于
 *      "../../host_infra_smoke.acceptance.c" 之前**（smoke 末位 = 最先执行, 注册序
 *      守卫; RSL-C1 tooth 4 校验此序）。本文件零被测实现新接线（[S1]-[S4] 住
 *      story_page.c——host runner 已编入; [S5]-[S7] 为 needle 结构齿, 扫真实工作树）。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/
 *      host_infra_test.elf
 *   红基线独立通道（acceptance-author 冻结期, 非冻结门 runner）:
 *     cd firmware/main/test/acceptance/red-baseline
 *     gcc -std=c11 -I. -o /tmp/rsl_red_runner ../../record_session_live.acceptance.c runner.c
 *     cd 仓根 && /tmp/rsl_red_runner   （exit code = 失败 TEST_CASE 数）
 *   RED baseline 判读口径见文件尾。
 * ---------------------------------------------------------------------------
 * 强度申报（契约条目 -> 承载断言 -> 性质档; 弱于契约处逐条）:
 *   契约 §四 kidpal_session_ctx wire 形状 -> RSL-A1 机制级（protocol.cc 函数区
 *         needle 三片段: session_id / kidpal_session_ctx / source_page）——**弱于
 *         契约**: 不钉逐字节 JSON 串（protocol.cc 非 host 可编面, K2 SPV2-B3 同款
 *         先例）; 行为等价假设 = 三片段在同函数区内拼串经 SendText 下发即承载 §四
 *         形状; 真机 wire 逐字复证挂设备轮（八步目击, M4K3-E3）。
 *   契约 §四 发送时序（首帧音频前 / 通道复用同样发送 / 重复发送安全）-> RSL-A2
 *         机制级（Listening 分支切片序断言: ctx 在 StartListening 前 + 在
 *         IsAudioProcessorRunning 门之外 + 调用恰一次）——「首帧音频前」由
 *         「StartListening 之前」承载（StartListening 后即开音频处理）; 重复发送
 *         安全半由 front 幂等入口承接（K1 冻结, server 齿复验）。
 *   契约 §四 source_page 闭集 -> RSL-A3 行为级（映射决策表全承载）。
 *   S-F2.2-AC1 屏显引导逐字 -> RSL-A4 行为级（guide_bubble 逐字 == AC1 文案）+
 *         结构级（apply_look 区消费 needle）。**弱于契约**: 引导态的视觉呈现
 *         （聆听标识/布局复用 WF-B-01）不钉像素——版式归 PNG/design review + 真机
 *         1bpp 目测挂设备轮; 行为等价假设 = apply_look 记录语境覆写取 [S2] 单源
 *         即承载「屏显引导」。
 *   S-F2.2-AC1 UTT_START 开口切录音态 -> RSL-A4 行为级（recording_enter 决策表）
 *         + 源锚代理（application.cc needle）; AFE VAD 真沿 host 不可机判 -> live
 *         齿形态诚实申报（M3-K3 同款）, 设备轮目击。
 *   S-F2.2-AC4 KEY 兜底收尾=保存 / BOOT=丢弃 -> RSL-A5 机制级（case 区动作映射
 *         needle + 双负锚）——行为半（真按 KEY 后 front 归档）= live 复证; K2 输入
 *         表手势->动作码半已冻（SPV2-A1）, 本文件只钉动作->会话动作半。
 *   S-F2.2-AC2 切句回显 -> RSL-A6 行为级（echo 机理全承载）+ 结构级（stt 分支喂入
 *         needle）; 「AI 不逐句回复」的会话半 = K1 front 累积态 + LISTEN_MANUAL
 *         M1 冻结面（消费零偏移, 不重钉）; 行为等价假设 = stt 分支记录语境旁路
 *         喂入 echo（条件在 needle 外, builder 义务）——条件缺失=live 盲区申报。
 *   defer ①重裁 gating -> RSL-A7 行为级（gate 决策表）+ 结构级（路由消费 needle）;
 *         front 条件推送半 = server 齿（M4K3-A7 行为级真 pump）。
 *   铁律「板端零新增计时器」-> 全文件零 timer 断言面（结构事实: [S1]-[S4] 纯逻辑
 *         注入式原语, RSL-A4 VAD 沿 = 事件驱动）。
 *   fixture 自含性: 全部行为齿 fixture = 纯值域入参 + 字面串, 无资产/时间真值/
 *         环境前提; 结构齿依赖仓根定位（cwd 相对探测, K2 kspv2_locate_root 同款——
 *         runner 须自仓根或 host_app 目录启动, RSL 各齿 REQUIRE_ROOT 前置）。
 *   盲区申报: ① ctx 真机 wire 逐字 + 发送失败降级路径 = 设备轮三源日志目击; ②
 *         AFE VAD speaking 真沿切页 = 设备轮; ③ 记录语境标志的置位/清位时机
 *         （REC_SESSION 置位 / 会话结束清位）= builder 实现细节, 本文件以
 *         guide_bubble 消费链 + recording_enter 决策表承载其可观测后果, 标志本体
 *         不钉（防过度钉实现）; ④ 唤醒词路径的引导态演出（P3 上喊「你好小智」->
 *         ctx(story) 发送但无引导气泡——记录语境标志未置）= 设计口径内（ctx 闭集
 *         语义照发、演出不误导）, 设备轮目击确认无演出冲突。
 * --------------------------------------------------------------------------- */

#include <unity.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>

/* RSL_UNIMPLEMENTED = weak 哨兵返回值（永不是合法返回值; K2 KSPV2_UNIMPLEMENTED
 * 同款）。story_page.c 已编入 host runner -> builder 在该文件落 [S1]-[S4] 强符号后
 * 哨兵被覆盖转绿; 独立红跑通道（red-baseline/unity.h stub）无实现 -> 哨兵恒红 =
 * 「frozen contract not implemented」设计态红。 */
#define RSL_UNIMPLEMENTED (-2)

/* ============================ 冻结数值镜像 ================================ */
/* KSTORY_PAGE_* = display/story_page.h 冻结枚举（SPV2 [K1] 钉）; KSPV2_ACT_* =
 * 同头动作码闭集。本文件不 include story_page.h（独立红跑通道 include 路径最小化;
 * 数值漂移时 K2 SPV2 系列齿先红 + RSL-C1 needle 守卫），以字面镜像 + 注释锚定。 */
#define RSL_PAGE_LIST      0 /* KSTORY_PAGE_LIST */
#define RSL_PAGE_READER    1 /* KSTORY_PAGE_READER */
#define RSL_PAGE_RECORDING 2 /* KSTORY_PAGE_RECORDING */
#define RSL_PAGE_STAMP     3 /* KSTORY_PAGE_STAMP */
#define RSL_PAGE_DELETE    4 /* KSTORY_PAGE_DELETE */
/* KP_PAGE_* = kidpal_render_node.h 冻结枚举（M2K2-A2 齿钉等值: HOME=0/SCHEDULE=1/
 * STORY=2; 本文件镜像消费, 漂移由 M2K2-A2 先红）。 */
#define RSL_KP_PAGE_HOME     0
#define RSL_KP_PAGE_SCHEDULE 1
#define RSL_KP_PAGE_STORY    2
/* [S3] 回显区容量（冻结值; story_page.h 落同值 define——RSL-A6 行为齿按本镜像断言,
 * 两处不一致 = 行为齿红即实现偏移）。 */
#define RSL_ECHO_MAX_LINES 4

/* 板级/实现面文件锚（结构齿扫描对象; 相对仓根）。 */
#define RSL_APP_CC      "firmware/main/application.cc"
#define RSL_MCP_CC      "firmware/main/mcp_server.cc"
#define RSL_PROTO_H     "firmware/main/protocols/protocol.h"
#define RSL_PROTO_CC    "firmware/main/protocols/protocol.cc"
#define RSL_STORY_C     "firmware/main/display/story_page.c"
#define RSL_STORY_H     "firmware/main/display/story_page.h"
#define RSL_LCD_CC      "firmware/main/display/lcd_display.cc"
#define RSL_CMAKE       "firmware/main/test/host_app/main/CMakeLists.txt"
#define RSL_REQ_MD      "docs/requirements.md"
#define RSL_CONTRACT_MD "docs/contracts/interface-contract.md"

/* ============================ 扫描助手（K2 同款形态） ===================== */

#define RSL_MAX_PATH 1024
#define RSL_MAX_READ (8u * 1024u * 1024u)

static char rsl_root[RSL_MAX_PATH];
static int rsl_root_ready = 0;

static int rsl_is_regular(const char *p)
{
    struct stat st;
    return stat(p, &st) == 0 && S_ISREG(st.st_mode);
}

static int rsl_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[RSL_MAX_PATH];
    size_t i;

    if (rsl_root_ready) {
        return 1;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (rsl_is_regular(probe)) {
            snprintf(rsl_root, sizeof(rsl_root), "%s", cands[i]);
            rsl_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

#define RSL_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(rsl_locate_root(), \
    "RSL RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app")

static char *rsl_read(const char *rel, size_t *out_len)
{
    char path[RSL_MAX_PATH];
    FILE *f;
    char *buf;
    size_t n;

    if (!rsl_root_ready && !rsl_locate_root()) {
        return NULL;
    }
    snprintf(path, sizeof(path), "%s/%s", rsl_root, rel);
    f = fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }
    buf = (char *)malloc(RSL_MAX_READ);
    assert(buf != NULL);
    n = fread(buf, 1, RSL_MAX_READ - 1, f);
    fclose(f);
    buf[n] = '\0';
    if (out_len != NULL) {
        *out_len = n;
    }
    return buf;
}

static int rsl_contains(const char *hay, const char *needle)
{
    return hay != NULL && needle != NULL && strstr(hay, needle) != NULL;
}

static int rsl_count_in(const char *hay, const char *needle)
{
    int n = 0;
    size_t ln;
    const char *p;

    if (hay == NULL || needle == NULL || needle[0] == '\0') {
        return 0;
    }
    ln = strlen(needle);
    p = hay;
    while ((p = strstr(p, needle)) != NULL) {
        n++;
        p += ln;
    }
    return n;
}

static int rsl_file_contains(const char *rel, const char *needle)
{
    char *buf = rsl_read(rel, NULL);
    int ok = rsl_contains(buf, needle);
    int n = rsl_count_in(buf, needle);

    (void)n;
    free(buf);
    return ok;
}

static int rsl_file_count(const char *rel, const char *needle)
{
    char *buf = rsl_read(rel, NULL);
    int n = rsl_count_in(buf, needle);

    free(buf);
    return n;
}

/* 文件内 [start_needle, end_needle) 切片（start 缺失 -> NULL; end 缺失 -> 切到 EOF;
 * K2 kspv2_region_between 同款——锚漂移 = 齿自身 RED(infra) 须复核）。返回 malloc
 * 串, 调用方 free。 */
static char *rsl_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = rsl_read(rel, NULL);
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
        e = buf + strlen(buf);
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

static int rsl_region_contains(const char *rel, const char *start_needle,
                               const char *end_needle, const char *needle)
{
    char *region = rsl_region_between(rel, start_needle, end_needle);
    int ok = rsl_contains(region, needle);

    free(region);
    return ok;
}

static int rsl_region_count(const char *rel, const char *start_needle,
                            const char *end_needle, const char *needle)
{
    char *region = rsl_region_between(rel, start_needle, end_needle);
    int n = rsl_count_in(region, needle);

    free(region);
    return n;
}

static char rsl_fm_buf[RSL_MAX_PATH * 2];
static const char *rsl_fm(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(rsl_fm_buf, sizeof(rsl_fm_buf), fmt, ap);
    va_end(ap);
    return rsl_fm_buf;
}

/* ============================ [S1]-[S4] weak 哨兵 ========================= */
/* builder 在 display/story_page.c（host runner 已编入源清单）落强符号后覆盖; 独立
 * 红跑通道无实现 -> 恒哨兵 = 设计态红（非收集错/链接错）。签名漂移 = 链接期显式
 * 失败（比哨兵假绿更响——K2 同款纪律）。 */

__attribute__((weak)) const char *kidpal_story_ctx_source_page(int page)
{ (void)page; return NULL; }

__attribute__((weak)) const char *kidpal_story_guide_bubble(void)
{ return NULL; }

__attribute__((weak)) int kidpal_story_recording_enter(int page, int ctx_active,
                                                       int speaking)
{ (void)page; (void)ctx_active; (void)speaking; return RSL_UNIMPLEMENTED; }

__attribute__((weak)) int kidpal_story_list_gate(int voice_front, int page_state)
{ (void)voice_front; (void)page_state; return RSL_UNIMPLEMENTED; }

__attribute__((weak)) int kidpal_story_echo_reset(void)
{ return RSL_UNIMPLEMENTED; }

__attribute__((weak)) int kidpal_story_echo_push(const char *text)
{ (void)text; return RSL_UNIMPLEMENTED; }

__attribute__((weak)) int kidpal_story_echo_count(void)
{ return RSL_UNIMPLEMENTED; }

__attribute__((weak)) int kidpal_story_echo_line(int idx, const char **out)
{ (void)idx; (void)out; return RSL_UNIMPLEMENTED; }

/* ========================================================================== *
 * TEST 1 — RSL-A1: session_ctx 发送原语四件套 + wire 形状（seam [S7]; 结构齿,
 * K2 SPV2-B3「发送原语四件套」同款先例形态）。
 * RED today: 全部 needle 零命中（protocol 层无 §四私有消息发送原语——plan 背景摘录
 * 「protocol.h:70-75 Send* 面无 KidPal §四消息类型发送原语」）。
 * ========================================================================== */
/* coverage: M4K3-A1 -- session_ctx 发送原语（protocol.h 声明 + protocol.cc wire 形状拼串 + application.cc 消费 = >=3 文件消费链） */
/* coverage: 契约 §四 kidpal_session_ctx -- wire 形状三片段（session_id/kidpal_session_ctx/source_page）函数区同区承载 */
TEST_CASE("record_session_live: session_ctx 发送原语+wire 形状 (RSL-A1)",
          "[record_session_live][RSL-A1][structural]")
{
    char *region;
    int n;

    RSL_REQUIRE_ROOT();

    /* tooth 1: 原语 >= 3 文件消费链（protocol.h 声明 + protocol.cc 定义 +
     * application.cc 发送点调用; K2 SendKidpalStoryDelete tooth-1 同款）。 */
    n = rsl_file_contains(RSL_PROTO_H, "SendKidpalSessionCtx") +
        rsl_file_contains(RSL_PROTO_CC, "SendKidpalSessionCtx") +
        rsl_file_contains(RSL_APP_CC, "SendKidpalSessionCtx");
    TEST_ASSERT_TRUE_MESSAGE(n >= 3,
        rsl_fm("RSL-A1 RED: SendKidpalSessionCtx 仅 %d/3 文件命中（期望 >= 3: "
               "protocol.h/.cc + application.cc）——§四 kidpal_session_ctx 发送原语"
               "无端到端消费链（逐字仿 SendAbortSpeaking 形态; K2 delete 同缝同族）",
               n));

    /* tooth 2: wire 形状三片段同函数区（契约 §四:
     * {"session_id":"...","type":"kidpal_session_ctx","source_page":"..."}——
     * session_id 拼串 + type + source_page 三片段必须在 SendKidpalSessionCtx 函数
     * 区内; 机制级代理, 逐字节复证挂设备轮——强度申报见头注）。 */
    region = rsl_region_between(RSL_PROTO_CC, "SendKidpalSessionCtx", "IsTimeout");
    TEST_ASSERT_TRUE_MESSAGE(region != NULL,
        "RSL-A1 RED(infra): protocol.cc SendKidpalSessionCtx 函数区切取失败"
        "（原语未落/锚漂移——须复核本齿锚）");
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_contains(region, "session_id") && rsl_contains(region, "kidpal_session_ctx") &&
        rsl_contains(region, "source_page") && rsl_contains(region, "SendText("),
        "RSL-A1 RED: protocol.cc SendKidpalSessionCtx 函数区缺 wire 形状片段"
        "（session_id / kidpal_session_ctx / source_page / SendText 下发）——契约 §四 "
        "消息形状未按 SendAbortSpeaking 同族拼串形态承载");
    free(region);

    /* tooth 3 (guard): websocket_protocol.{h,cc} 零改动负锚（K2 delete 原语无
     * override 先例——SendText 虚实现自动生效; 若有人在此层加 override = 缝分裂）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_file_count("firmware/main/protocols/websocket_protocol.cc",
                       "SendKidpalSessionCtx") == 0,
        "RSL-A1 RED(guard): websocket_protocol.cc 出现 SendKidpalSessionCtx——"
        "发送缝须单点住 protocol 基类（K2 同族先例; override = 缝分裂反模式）");
}

/* ========================================================================== *
 * TEST 2 — RSL-A2: 发送时点结构齿（seam [S7]; Listening 分支无条件、StartListening
 * 之前、IsAudioProcessorRunning 门之外; 负锚 ToggleChatEvent）。
 * RED today: application.cc 零 SendKidpalSessionCtx。
 * ========================================================================== */
/* coverage: M4K3-A1 -- 发送时点（Listening 分支切片序: ctx < IsAudioProcessorRunning 门 < ... 且 ctx < SendStartListening; 调用恰一次; HandleToggleChatEvent 负锚） */
TEST_CASE("record_session_live: 发送时点 Listening 分支序+负锚 (RSL-A2)",
          "[record_session_live][RSL-A2][structural]")
{
    char *listening;
    const char *p_ctx, *p_gate, *p_start;

    RSL_REQUIRE_ROOT();

    /* tooth 1: Listening 分支内 SendKidpalSessionCtx 恰一次（重复发送安全由 front
     * 幂等承接, 但板端发送点须单点——无重试环 = notes ③ 负锚）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_region_count(RSL_APP_CC, "case kDeviceStateListening:",
                         "case kDeviceStateSpeaking:", "SendKidpalSessionCtx(") == 1,
        "RSL-A2 RED: HandleStateChangedEvent Listening 分支内 SendKidpalSessionCtx "
        "调用数 != 1（0 = 会话发起发送半未接线, 双入口漏断; >1 = 重试环/多点散射"
        "——notes ③ 负锚: 发送失败降级普通会话, 不重试风暴）");

    listening = rsl_region_between(RSL_APP_CC, "case kDeviceStateListening:",
                                   "case kDeviceStateSpeaking:");
    TEST_ASSERT_TRUE_MESSAGE(listening != NULL,
        "RSL-A2 RED(infra): application.cc Listening 分支切片失败（case 锚漂移"
        "——须复核本齿锚）");

    /* tooth 2: 时序序断言——ctx 调用点在 IsAudioProcessorRunning 门**之前**（= 门
     * 之外无条件执行; 门内执行会短路通道复用会话, notes ②）且在 SendStartListening
     * 之前（= 首帧音频前, 契约 §四时序——发晚了 front 已按普通会话切句送 LLM）。 */
    p_ctx = strstr(listening, "SendKidpalSessionCtx(");
    p_gate = strstr(listening, "IsAudioProcessorRunning");
    p_start = strstr(listening, "SendStartListening(");
    TEST_ASSERT_TRUE_MESSAGE(
        p_ctx != NULL && p_gate != NULL && p_start != NULL && p_ctx < p_gate &&
        p_ctx < p_start,
        "RSL-A2 RED: Listening 分支内时序错位——SendKidpalSessionCtx 须在 "
        "IsAudioProcessorRunning 门之外（通道复用会话不短路漏发）且在 "
        "SendStartListening 之前（首帧音频前, 契约 §四; 发晚 = front 已切句送 LLM, "
        "累积态切换失效）");
    free(listening);

    /* tooth 3 (负锚): 会话收尾/中断路径零 ctx——HandleToggleChatEvent 函数区（含
     * Listening 分支 CloseAudioChannel 那条丢弃路径）严禁发送 session_ctx（ctx 是
     * 会话发起信号, 不是收尾信号; 两条 front 路径不混的板端根, notes (e) 同族）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_region_count(RSL_APP_CC, "void Application::HandleToggleChatEvent()",
                         "void Application::HandleStartListeningEvent()",
                         "SendKidpalSessionCtx") == 0,
        "RSL-A2 RED(负锚): HandleToggleChatEvent 函数区出现 SendKidpalSessionCtx——"
        "会话收尾/中断路径不得发会话发起信号（ctx 只住会话发起发送点）");
}

/* ========================================================================== *
 * TEST 3 — RSL-A3: source_page 映射单点（seam [S1]; 行为齿 + 接线结构齿）。
 * RED today: 行为齿哨兵 NULL; 接线 needle 零命中。
 * ========================================================================== */
/* coverage: M4K3-A1 -- source_page 闭集映射（story/schedule/home 闭集一次做全 + 缺省兜底不外溢） + 会话发起入口单点消费 */
TEST_CASE("record_session_live: source_page 闭集映射单点 (RSL-A3)",
          "[record_session_live][RSL-A3][behavior][structural]")
{
    const char *sp;

    RSL_REQUIRE_ROOT();

    /* tooth 1: 闭集映射决策表（KP_PAGE_* 数值镜像 = M2K2-A2 冻结等值; seam [S1]）。 */
    sp = kidpal_story_ctx_source_page(RSL_KP_PAGE_STORY);
    TEST_ASSERT_TRUE_MESSAGE(sp != NULL && strcmp(sp, "story") == 0,
        "RSL-A3 RED: kidpal_story_ctx_source_page(KP_PAGE_STORY) != \"story\"——"
        "P3 记录语境信号映射缺失（S-F2.2-AC1 技术信号: source_page=story, front 拦截"
        "切累积态的板端半; 哨兵 NULL = frozen seam [S1] 未实现）");
    sp = kidpal_story_ctx_source_page(RSL_KP_PAGE_SCHEDULE);
    TEST_ASSERT_TRUE_MESSAGE(sp != NULL && strcmp(sp, "schedule") == 0,
        "RSL-A3 RED: KP_PAGE_SCHEDULE 未映射 \"schedule\"——契约 §四闭集一次做全"
        "（防未来再开固件面, plan 跨包事实节）");
    sp = kidpal_story_ctx_source_page(RSL_KP_PAGE_HOME);
    TEST_ASSERT_TRUE_MESSAGE(sp != NULL && strcmp(sp, "home") == 0,
        "RSL-A3 RED: KP_PAGE_HOME 未映射 \"home\"——闭集一次做全");
    sp = kidpal_story_ctx_source_page(-1);
    TEST_ASSERT_TRUE_MESSAGE(sp != NULL && strcmp(sp, "home") == 0,
        "RSL-A3 RED: 负值页未兜底 \"home\"——越界缺省兜底缺失（契约闭集不外溢第四"
        "取值: 绝不返回 NULL / 空串 / story/schedule 以外新值）");
    sp = kidpal_story_ctx_source_page(99);
    TEST_ASSERT_TRUE_MESSAGE(sp != NULL && strcmp(sp, "home") == 0,
        "RSL-A3 RED: 未知页值未兜底 \"home\"——同上（闭集防御）");

    /* tooth 2: 会话发起入口单点消费（Listening 分支调 映射(kidpal_render_node_get_page())
     * ——映射散落 = 漏入口根因（notes ④）; get_page 入参 = 页枚举符号非数值耦合的
     * 接线锚（§2.3 对账②页序漂移注记）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_region_contains(RSL_APP_CC, "case kDeviceStateListening:",
                            "case kDeviceStateSpeaking:",
                            "kidpal_story_ctx_source_page(") &&
        rsl_region_contains(RSL_APP_CC, "case kDeviceStateListening:",
                            "case kDeviceStateSpeaking:",
                            "kidpal_render_node_get_page("),
        "RSL-A3 RED: Listening 分支未单点消费 kidpal_story_ctx_source_page("
        "kidpal_render_node_get_page())——source_page 映射散落/未接页枚举读取面"
        "（notes ④: KEY 按与唤醒词双入口都过该单点, 映射散落 = 漏入口根因）");
}

/* ========================================================================== *
 * TEST 4 — RSL-A4: 引导态（seam [S2]/[S6]; 行为齿: 气泡逐字 + 开口切录音态决策表;
 * 结构齿: REC_SESSION 消费位接线 + apply_look 覆写消费 + VAD 沿接线）。
 * RED today: 哨兵 NULL / -2; 结构 needle 零命中（REC_SESSION 现 log-only 桩;
 * application.cc VAD 分支现只驱动 LED）。
 * ========================================================================== */
/* coverage: M4K3-A2 -- 引导态（气泡逐字「需要我帮你记点什么？」+ 开口切录音态决策表 + REC_SESSION 会话发起接线 + apply_look 覆写消费 + VAD 沿接线） */
/* coverage: S-F2.2-AC1 -- 屏显记录语境引导逐字 + 开口第一句即进录音态（决策表承载; AFE VAD 真沿 = live 目击项） */
TEST_CASE("record_session_live: 引导态气泡+开口切录音态 (RSL-A4)",
          "[record_session_live][RSL-A4][behavior][structural]")
{
    int rc;

    RSL_REQUIRE_ROOT();

    /* tooth 1: 引导气泡逐字（AC1: 屏显记录语境引导「需要我帮你记点什么？」;
     * seam [S2] 单源——lcd 覆写消费此源, 不双写）。 */
    {
        const char *bubble = kidpal_story_guide_bubble();
        TEST_ASSERT_TRUE_MESSAGE(bubble != NULL,
            "RSL-A4 RED: kidpal_story_guide_bubble() 返回 NULL——引导态气泡单源 "
            "frozen seam [S2] 未实现（AC1 屏显引导无承载）");
        TEST_ASSERT_TRUE_MESSAGE(strcmp(bubble, "需要我帮你记点什么？") == 0,
            "RSL-A4 RED: 引导气泡文案 != AC1 逐字「需要我帮你记点什么？」——"
            "记录语境引导文案偏移（S-F2.2-AC1; WF-B-01 气泡换文案口径）");
    }

    /* tooth 2: 开口切录音态决策表（seam [S2]; §2.3 引导态行 + notes ⑨ UTT_START
     * 事件沿的纯逻辑承载）。 */
    rc = kidpal_story_recording_enter(RSL_PAGE_LIST, 1, 1);
    TEST_ASSERT_TRUE_MESSAGE(rc == 1,
        "RSL-A4 RED: 记录语境 + 引导态(LIST) + 开口(speaking) 未判切录音态（期望 1)"
        "——开口第一句即进录音态缺失（S-F2.2-AC1; 哨兵 -2 = frozen seam 未实现）");
    rc = kidpal_story_recording_enter(RSL_PAGE_LIST, 1, 0);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        "RSL-A4 RED: 未开口(speaking=0) 误判切录音态（期望 0）——录音不自动开始"
        "（S-F2.2 补充规则: 开口前只引导不录音）");
    rc = kidpal_story_recording_enter(RSL_PAGE_LIST, 0, 1);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        "RSL-A4 RED: 非记录语境(ctx_active=0) 误判切录音态（期望 0）——唤醒词普通"
        "会话开口不得切 P3 录音演出（记录语境标志门缺失 = 普通会话被吞进录音态）");
    rc = kidpal_story_recording_enter(RSL_PAGE_RECORDING, 1, 1);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        "RSL-A4 RED: 已在录音态重判切页（期望 0, 幂等）——重复 VAD 沿不得重置录音"
        "演出（rec_seconds 清零 = 大时长造假）");
    rc = kidpal_story_recording_enter(RSL_PAGE_READER, 1, 1);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        "RSL-A4 RED: 阅读态误判切录音态（期望 0）——记录语境只在引导态(LIST)成立");
    rc = kidpal_story_recording_enter(99, 1, 1);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        "RSL-A4 RED: 越界页值未防御（期望 0）——决策函数不得对未知页态产切页动作");

    /* tooth 3: REC_SESSION 消费位会话发起接线（case 区: 推语音层前台 + manual 聆听
     * 入口; S-F2.2-AC1「在星星记录本按 KEY（无触发词）」双入口板端半; 源锚代理
     * ——强度申报见头注）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_region_contains(RSL_MCP_CC, "case KSPV2_ACT_REC_SESSION:",
                            "case KSPV2_ACT_DELETE_ENTER:",
                            "kidpal_render_node_show_chat") &&
        rsl_region_contains(RSL_MCP_CC, "case KSPV2_ACT_REC_SESSION:",
                            "case KSPV2_ACT_DELETE_ENTER:", "StartListening("),
        "RSL-A4 RED: KSPV2_ACT_REC_SESSION 消费位仍是 log-only 桩——会话发起接线"
        "缺失（推语音层前台 kidpal_render_node_show_chat + StartListening manual "
        "聆听入口; 双入口 KEY 半经此汇入 Listening 发送点）");

    /* tooth 4: apply_look 覆写消费（seam [S6]; LOOK 单源 [S2], M1 冻结映射零触碰
     * ——写点在 lcd_display.cc 气泡覆写缝, 不在 kidpal_voice_states.c）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_region_contains(RSL_LCD_CC,
                            "static void kidpal_voice_apply_look(int scene)",
                            "static void kidpal_chrome_mid_sync_locked(void)",
                            "kidpal_story_guide_bubble"),
        "RSL-A4 RED: lcd_display.cc kidpal_voice_apply_look 区未消费 "
        "kidpal_story_guide_bubble——记录语境引导气泡覆写缝缺失（引导态复用 WF-B-01 "
        "布局、气泡换 AC1 文案的呈现半无驱动方）");

    /* tooth 5: VAD 沿接线（application.cc 消费 recording_enter 决策; MAIN_EVENT_
     * VAD_CHANGE 注册在世 = must-stay-green 前提守卫）。 */
    TEST_ASSERT_TRUE_MESSAGE(rsl_file_contains(RSL_APP_CC, "MAIN_EVENT_VAD_CHANGE"),
        "RSL-A4 RED(infra): application.cc 丢失 MAIN_EVENT_VAD_CHANGE——VAD 事件沿"
        "注册被移除（must-stay-green 前提失效, 须复核）");
    TEST_ASSERT_TRUE_MESSAGE(rsl_file_contains(RSL_APP_CC,
                                               "kidpal_story_recording_enter("),
        "RSL-A4 RED: application.cc 零 kidpal_story_recording_enter 消费——开口切"
        "录音态决策无驱动方（VAD 事件沿接线缺失; notes ⑨ UTT_START 事件沿）");
}

/* ========================================================================== *
 * TEST 5 — RSL-A5: 录音手势会话接线（seam [S5]; 结构齿; STOP/DISCARD case 拆分 +
 * 双负锚——两条 front 路径不混的板端根）。
 * RED today: STOP case 与 DISCARD case 现共用本地导航 case 组（K2 现状: 只
 * ScheduleStoryRender + log, 无会话动作）——STOP 切片内零 StopListening、DISCARD
 * 切片内零 CloseAudioChannel。
 * ========================================================================== */
/* coverage: M4K3-A3 -- 录音手势接线（STOP_RECORD case -> StopListening 收尾保存面 + DISCARD_RECORD case -> CloseAudioChannel 丢弃面 + 交叉负锚） */
/* coverage: S-F2.2-AC4 -- KEY 兜底收尾=保存（listen stop 上行触发 front 归档）+ BOOT 任一手势=丢弃（disconnect cancel 不落盘）; KEY 长按同短按 = K2 输入表已冻不重钉 */
TEST_CASE("record_session_live: 录音手势会话接线+交叉负锚 (RSL-A5)",
          "[record_session_live][RSL-A5][structural]")
{
    RSL_REQUIRE_ROOT();

    /* tooth 1: STOP_RECORD case -> 收尾保存面（Application StopListening 面 ->
     * MAIN_EVENT_STOP_LISTENING -> SendStopListening + Idle; front
     * _story_stop_on_listen_stop 消费 listen/state=stop 归档 = S-F2.2-AC4 兜底
     * 收尾保存链板端半）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_region_contains(RSL_MCP_CC, "case KSPV2_ACT_STOP_RECORD:",
                            "case KSPV2_ACT_DISCARD_RECORD:", "StopListening("),
        "RSL-A5 RED: KSPV2_ACT_STOP_RECORD case 零 StopListening 接线——KEY 兜底"
        "收尾（再按 KEY 保存）无会话动作（S-F2.2-AC4: 停录并保存; front listen-stop "
        "归档缝收不到上行 = 永不归档）");

    /* tooth 2 (负锚): STOP case 严禁 CloseAudioChannel / ToggleChatState——
     * ToggleChatState Listening 分支直接 CloseAudioChannel 不发 listen stop ->
     * front 走 ws_disconnect cancel 丢弃路径而非归档（notes (e): 毁稿根）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_region_count(RSL_MCP_CC, "case KSPV2_ACT_STOP_RECORD:",
                         "case KSPV2_ACT_DISCARD_RECORD:",
                         "CloseAudioChannel") == 0 &&
        rsl_region_count(RSL_MCP_CC, "case KSPV2_ACT_STOP_RECORD:",
                         "case KSPV2_ACT_DISCARD_RECORD:",
                         "ToggleChatState") == 0,
        "RSL-A5 RED(负锚): KSPV2_ACT_STOP_RECORD case 出现 CloseAudioChannel/"
        "ToggleChatState——收尾保存严禁走会话 toggle/断连（不发 listen stop -> front "
        "走 disconnect cancel 丢弃路径 = 毁稿; 两条 front 路径不混的板端根, notes (e)）");

    /* tooth 3: DISCARD_RECORD case -> 丢弃面（CloseAudioChannel 即达 front 断开钩子
     * acc.cancel 弹缓冲不落盘; S-F2.2-AC4 BOOT 任一手势 = 丢弃退回列表）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_region_contains(RSL_MCP_CC, "case KSPV2_ACT_DISCARD_RECORD:",
                            "case KSPV2_ACT_NONE:", "CloseAudioChannel"),
        "RSL-A5 RED: KSPV2_ACT_DISCARD_RECORD case 零 CloseAudioChannel 接线——"
        "BOOT 丢弃无会话动作（S-F2.2-AC4: 取消录音、已录内容丢弃退回列表, 不设重录"
        "恢复 v1.49; front 断开钩子 cancel 收不到断开 = 内容滞留累积态）");

    /* tooth 4 (负锚): DISCARD case 严禁 StopListening——丢弃不是归档（listen stop
     * 会触发 front stop 归档 = 丢弃反而落盘, 语义反转）; 旧冻结表「KEY 长按=丢弃」
     * 已退役（v1.47 反转 = 保存）, 本负锚防语义回潮。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_region_count(RSL_MCP_CC, "case KSPV2_ACT_DISCARD_RECORD:",
                         "case KSPV2_ACT_NONE:", "StopListening(") == 0,
        "RSL-A5 RED(负锚): KSPV2_ACT_DISCARD_RECORD case 出现 StopListening——"
        "丢弃不得走收尾保存面（listen stop = front 归档, 丢弃反被保存 = 语义反转"
        "回潮; 丢弃 = 断连 cancel 直裁不落盘）");
}

/* ========================================================================== *
 * TEST 6 — RSL-A6: story 录音态话语回显（seam [S3]/[S6]; 行为齿: echo 行数机理;
 * 结构齿: application.cc stt 分支喂入）。
 * RED today: 哨兵 -2; needle 零命中（stt 分支现只驱动语音轮回显 + LED）。
 * ========================================================================== */
/* coverage: M4K3-A5 -- 话语回显机理（reset/push/count/line 行数模型 + FIFO 滚出 + 空句拒绝） + stt 分支记录语境旁路喂入接线 */
/* coverage: S-F2.2-AC2 -- 切句只用于话语回显板端半（AI 不逐句回复 = LISTEN_MANUAL 累积态 M1 冻结面消费零偏移, 不重钉） */
TEST_CASE("record_session_live: story 录音态话语回显 (RSL-A6)",
          "[record_session_live][RSL-A6][behavior][structural]")
{
    const char *line = NULL;
    int rc;

    RSL_REQUIRE_ROOT();

    /* tooth 1: 空句拒绝 + reset（seam [S3]）。 */
    rc = kidpal_story_echo_reset();
    TEST_ASSERT_TRUE_MESSAGE(rc == 0,
        "RSL-A6 RED: kidpal_story_echo_reset 返回 != 0（期望 0; 哨兵 -2 = frozen "
        "seam [S3] 未实现）——回显区清零缺失（会话开始不清零 = 上一轮话语串味）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_echo_push(NULL) == -1,
        "RSL-A6 RED: push(NULL) 未拒绝（期望 -1）——畸形入参不得污染回显区");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_echo_push("") == -1,
        "RSL-A6 RED: push(\"\") 未拒绝（期望 -1）——空句不入列");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_echo_push("   \t ") == -1,
        "RSL-A6 RED: push(全空白) 未拒绝（期望 -1）——VAD 切句尾噪不入列");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_echo_count() == 0,
        "RSL-A6 RED: 拒绝路径后 count != 0——拒绝句被计入（污染计数）");

    /* tooth 2: 逐句入列 + 序（idx 0 = 最旧）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_echo_push("从前有一只小猫") == 1,
        "RSL-A6 RED: 首句 push 返回 != 1（期望 = 当前句数 1）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_echo_push("它住在森林里") == 2,
        "RSL-A6 RED: 第二句 push 返回 != 2——句数计数偏移");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_echo_count() == 2,
        "RSL-A6 RED: count != 2——计数与入列不一致");
    rc = kidpal_story_echo_line(0, &line);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && line != NULL &&
                             strcmp(line, "从前有一只小猫") == 0,
        "RSL-A6 RED: line(0) != 最旧句「从前有一只小猫」——回显序错乱（孩子看到的"
        "话语顺序 = 叙述顺序, S-F2.2-AC2 话语回显语义）");
    rc = kidpal_story_echo_line(1, &line);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && line != NULL &&
                             strcmp(line, "它住在森林里") == 0,
        "RSL-A6 RED: line(1) != 第二句——回显序错乱");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_echo_line(2, &line) == -1,
        "RSL-A6 RED: line(越界) 未拒绝（期望 -1）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_echo_line(0, NULL) == -1,
        "RSL-A6 RED: line(out=NULL) 未拒绝（期望 -1）");

    /* tooth 3: FIFO 滚出（容量 = RSL_ECHO_MAX_LINES 镜像 4; 满容量挤最旧——
     * 「回显区滚动, 超屏高滚出」行数模型）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_echo_push("第三句") == 3 &&
                             kidpal_story_echo_push("第四句") == 4,
        "RSL-A6 RED: 入列至容量 4 前计数偏移");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_echo_push("第五句") == RSL_ECHO_MAX_LINES,
        "RSL-A6 RED: 超容量 push 后句数 != 容量 4——满容量未挤最旧（无界增长 = "
        "录音态内存红线破）");
    rc = kidpal_story_echo_line(0, &line);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && line != NULL &&
                             strcmp(line, "它住在森林里") == 0,
        "RSL-A6 RED: 滚出后 line(0) != 「它住在森林里」——FIFO 滚出错位（最旧句"
        "「从前有一只小猫」应被挤出）");
    rc = kidpal_story_echo_line(RSL_ECHO_MAX_LINES - 1, &line);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && line != NULL &&
                             strcmp(line, "第五句") == 0,
        "RSL-A6 RED: 滚出后末位 != 最新句——滚出后序错乱");

    /* tooth 4: stt 分支喂入接线（seam [S6]; 与 front _story_feed_from_stt 旁路
     * 同构——不改 wire、只旁路消费; 记录语境条件在 needle 外 = builder 义务,
     * 行为等价假设见头注盲区申报③）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_region_contains(RSL_APP_CC,
                            "strcmp(type->valuestring, \"stt\")",
                            "strcmp(type->valuestring, \"llm\")",
                            "kidpal_story_echo_push"),
        "RSL-A6 RED: application.cc stt 分支零 kidpal_story_echo_push——切句回显"
        "旁路喂入未接线（S-F2.2-AC2: 切句只用于话语回显; story 录音态回显区无驱动方）");
}

/* ========================================================================== *
 * TEST 7 — RSL-A7: story_list 推送 gating（seam [S4]/[S7] 消费半; 行为齿: gate
 * 决策表; 结构齿: VRT_ROUTE_STORY_LIST 消费 needle）。
 * RED today: 哨兵 -2; needle 零命中（路由现无条件 begin_page 推前台）。
 * ========================================================================== */
/* coverage: M4K3-A6 -- 推送 gating（VOICE 前台/录音态 -> 只装缓存; 列表冷浏览/阅读/盖章/删除 -> 推前台 K2 零偏移） + 路由消费接线 */
/* coverage: 跨包承接 260928-story-connect-push-defer -- ①恢复重裁板端消费半（「ctx 时点 vs 录音引导态」冲突分工: front 推、板端记录语境前台 gating） */
TEST_CASE("record_session_live: story_list 推送 gating (RSL-A7)",
          "[record_session_live][RSL-A7][behavior][structural]")
{
    RSL_REQUIRE_ROOT();

    /* tooth 1: gate 决策表（seam [S4]; 1 = 只装缓存不推前台, 0 = 推前台）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_list_gate(1, RSL_PAGE_LIST) == 1,
        "RSL-A7 RED: VOICE 前台 + LIST 页未判只装缓存（期望 1）——引导态/录音态"
        "被 story_list 帧推前台覆盖 = defer 包真机缺陷形态复潮（盖掉聆听态演出）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_list_gate(0, RSL_PAGE_RECORDING) == 1,
        "RSL-A7 RED: 录音态页未判只装缓存（期望 1）——录音演出被列表页覆盖");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_list_gate(1, RSL_PAGE_RECORDING) == 1,
        "RSL-A7 RED: VOICE 前台 + 录音态未判只装缓存（期望 1）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_list_gate(0, RSL_PAGE_LIST) == 0,
        "RSL-A7 RED: 列表冷浏览被判缓存不推（期望 0）——正常 story_list 渲染被"
        "误 gate（K2 消费面「收到 story_list 即渲染」零偏移红线）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_list_gate(0, RSL_PAGE_READER) == 0 &&
                             kidpal_story_list_gate(0, RSL_PAGE_STAMP) == 0 &&
                             kidpal_story_list_gate(0, RSL_PAGE_DELETE) == 0,
        "RSL-A7 RED: 阅读态/盖章帧/删除子态被误 gate（期望 0 推前台）——K2 现状"
        "零偏移红线（gating 只限记录语境前台, 不得扩到整页族）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_story_list_gate(0, 99) == 0,
        "RSL-A7 RED: 未知页态被判缓存不推（期望 0 兜底推前台）——未知态不得吞"
        "推送（宁可多渲染, 不可丢列表更新）");

    /* tooth 2: 路由消费接线（VRT_ROUTE_STORY_LIST case 区消费 gate——门在路由侧
     * 真实在位, 不是只有决策函数; 现状 = 无条件 begin_page 推前台）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_region_contains(RSL_MCP_CC, "case VRT_ROUTE_STORY_LIST:",
                            "case VRT_ROUTE_STORY_TEXT:",
                            "kidpal_story_list_gate("),
        "RSL-A7 RED: mcp_server.cc VRT_ROUTE_STORY_LIST 路由未消费 "
        "kidpal_story_list_gate——推送 gating 决策无消费方（纯逻辑函数无驱动方 = "
        "defer ①重裁板端半盲区）");
}

/* ========================================================================== *
 * TEST 8 — RSL-C1: canary + 注册序守卫（structural guards）。
 * RED today: 注册序 needle 零命中（本文件未进 HOST_ACCEPTANCE_SRCS——builder
 * duty）; canary/语义位守卫现状绿（must-stay-green）。
 * ========================================================================== */
/* coverage: M4K3-A9 -- canary 与注册守卫（spec/契约锚 + K2 动作码语义位零偏移 + HOST_ACCEPTANCE_SRCS 注册序） */
TEST_CASE("record_session_live: canary+注册序守卫 (RSL-C1)",
          "[record_session_live][RSL-C1][structural]")
{
    char *cm;
    const char *p_self, *p_smoke;

    RSL_REQUIRE_ROOT();

    /* tooth 1: spec canary（S-F2.2 + 技术信号载体在 requirements 现文——story 语义
     * 权威未漂移）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_file_contains(RSL_REQ_MD, "S-F2.2") &&
        rsl_file_contains(RSL_REQ_MD, "kidpal_session_ctx"),
        "RSL-C1 RED: requirements.md 丢失 S-F2.2 节或 kidpal_session_ctx 锚——"
        "story 语义权威漂移（须复核, 不属 builder 修订面）");

    /* tooth 2: 契约 canary（§四 kidpal_session_ctx 消息行在世——本包零契约改动,
     * 契约漂移 = 红旗）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_file_contains(RSL_CONTRACT_MD, "kidpal_session_ctx") &&
        rsl_file_contains(RSL_CONTRACT_MD, "source_page"),
        "RSL-C1 RED: 契约 §四 kidpal_session_ctx/source_page 锚缺失——契约面漂移"
        "（红旗: 本包铁律 = 契约冻结版零自定, 改动走契约门）");

    /* tooth 3: K2 动作码语义位零偏移（story_page.h 闭集在世——K3 接线不改 K2 语义
     * 位; 漂移 = K2 冻结面被破）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsl_file_contains(RSL_STORY_H, "KSPV2_ACT_REC_SESSION") &&
        rsl_file_contains(RSL_STORY_H, "KSPV2_ACT_STOP_RECORD") &&
        rsl_file_contains(RSL_STORY_H, "KSPV2_ACT_DISCARD_RECORD"),
        "RSL-C1 RED: story_page.h 动作码语义位缺失（REC_SESSION/STOP_RECORD/"
        "DISCARD_RECORD）——K2 冻结闭集被改（K3 只接线不改语义位）");

    /* tooth 4: 注册序守卫（builder FIRST ACTION; smoke 末位 = 最先执行——本文件
     * 须置于 host_infra_smoke.acceptance.c 之前）。 */
    cm = rsl_read(RSL_CMAKE, NULL);
    TEST_ASSERT_TRUE_MESSAGE(cm != NULL,
        "RSL-C1 RED(infra): host_app CMakeLists.txt 读取失败");
    p_self = strstr(cm, "../../record_session_live.acceptance.c");
    p_smoke = strstr(cm, "../../host_infra_smoke.acceptance.c");
    TEST_ASSERT_TRUE_MESSAGE(
        p_self != NULL && p_smoke != NULL && p_self < p_smoke,
        "RSL-C1 RED: record_session_live.acceptance.c 未注册进 HOST_ACCEPTANCE_"
        "SRCS（或未置于 host_infra_smoke.acceptance.c 之前）——builder FIRST ACTION "
        "未执行（注册序 = smoke 末位最先执行的 runner 纪律）");
    free(cm);
}

/* --------------------------------------------------------------------------- *
 * RED baseline 判读口径（acceptance-author 冻结期实跑, 2026-09-30; 现状树 @5d58eb9,
 * firmware 自 2c7343a 零差异——既有齿零偏移由引用锚行判停承载, 不重跑全套 host build）:
 *   设计态红（= 声明的缺失行为, 非环境/依赖/fixture 缺陷）:
 *     RSL-A1  发送原语零命中（protocol 层无 §四 发送原语）。
 *     RSL-A2  Listening 分支零 SendKidpalSessionCtx。
 *     RSL-A3  [S1] 哨兵 NULL（映射未实现）+ 接线 needle 零命中。
 *     RSL-A4  [S2] 哨兵 NULL/-2（引导态未实现）+ REC_SESSION log-only 桩 +
 *             apply_look 零消费 + VAD 接线零消费。
 *     RSL-A5  STOP/DISCARD case 组内零会话动作（K2 现状本地导航）。
 *     RSL-A6  [S3] 哨兵 -2（回显缝未实现）+ stt 分支零喂入。
 *     RSL-A7  [S4] 哨兵 -2（gating 未实现）+ 路由零消费。
 *     RSL-C1  tooth 4 注册序红（未进 HOST_ACCEPTANCE_SRCS——builder FIRST ACTION）;
 *             tooth 1-3 canary 守卫现状绿（must-stay-green）。
 *   独立红跑通道（red-baseline/unity.h stub + runner.c, 非冻结门 runner）:
 *     gcc -std=c11 -I. -o /tmp/rsl_red_runner ../../record_session_live.acceptance.c
 *         runner.c  （自 red-baseline/ 目录; 行为齿走哨兵红、结构齿扫真实工作树）
 *   GREEN 后门面 = host_app linux ELF（CMakeLists 接线后全量, 既有 128 齿零偏移 +
 *   本文件 8 TEST_CASE 转绿）。
 * --------------------------------------------------------------------------- *
 * REVISION LOG（tighten-only 修订窗口; 纯 additive / 零断言语义变化才可追加）:
 *   （冻结时点无修订。）
 * --------------------------------------------------------------------------- */
