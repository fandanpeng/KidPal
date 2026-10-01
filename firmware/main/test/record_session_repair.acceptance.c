/*
 * Frozen acceptance tests — TaskPackage 261001-m4-k3-record-session-live-repair
 * (subsystem: firmware; Package A：纯固件四缺陷止血). 前序包 260924-m4-k3-record-session-live
 * 已 merged；本包为其真机 P3 冒烟（c:/tmp/p3-smoke-261001.log, ELF SHA 0422fa290）暴露的
 * 四缺陷止血——**纯固件面**，不含 D3「说完了」与逐句回显内容（随 Package B）。
 *
 * 承载 acceptance（taskpackage.json acceptance[]）: K3R-A1..A5。
 *   K3R-A1（D4a）: 录音语境键手势归属——录音页态 + ctx active 时 KEY 必产
 *        KSPV2_ACT_STOP_RECORD、绝不产 GLOBAL_ACT_SESSION_TOGGLE（正/负双齿）。
 *   K3R-A2（D2）: 录音页与「● 录音中」可见、VOICE 层不再压住录音页。2026-10-01 真机轮
 *        修订后由三齿共同承载: RSR-A2（[R1] 形态无关代理）+ RSR-A6（[R4] 录音态覆盖层
 *        不驻留, 直接可观察）+ RSR-A7（[R5] voice_end 页态同步, 直接可观察）。
 *   K3R-A3（D4b-1）: 会话收尾后录音演出定时器必停、页态必回 LIST（不依赖页态自删）。
 *   K3R-A4（D4b-3）: 外部 kidpal_*_page_render 清屏后渲染节点不得以悬垂对象出帧。
 *   K3R-A5（D1）: story_list defer 缓存存在消费方（VOICE 弹出/前台回落 P3 时以缓存重渲）。
 *
 * 注: 2026-10-01 真机轮「补严齿」纯 additive 修订（D2 两根因 [R4]/[R5] + RSR-A6/A7），
 *     详见文件尾 REVISION LOG R1; 既有 RSR-A1..A5 / RSR-C1 断言逐字未动（tighten-only）。
 *
 * 缺陷根因（plan.md §1，逐行 file:line 证据）:
 *   D4a  board keya_story_page_wants 判据 overlay_top == 0 && page == P3 才交故事页；
 *        录音时 VOICE 层前台 -> 判据不成立 -> KEY 落全局 key_route_layer ->
 *        GLOBAL_ACT_SESSION_TOGGLE -> Listening 态关通道不发 listen stop ->
 *        front 走 ws_disconnect cancel 丢弃（毁稿）。board:197-206 / mcp:326-338。
 *   D2   application.cc:339-353 开口切录音态只切视图页态，未让 VOICE 层让位 ->
 *        录音页被 VOICE 层压在下层。
 *   D4b-1 application.cc:970-974 会话收尾只清 ctx 与 s_story_rec_entered，不复位
 *        g_story_view.page -> 录音定时器判定 page != RECORDING 恒不成立、不自删
 *        （mcp:111-116），每 100ms 的 kidpal_story_page_render（mcp:121）清屏砸毁
 *        首页图像对象。
 *   D4b-3 host_infra_render_p1_frame（kidpal_render_node.cc:170-187）对 s_kidpal_img
 *        无有效性守卫 -> 悬垂对象 lv_image_set_src -> panic（backtrace 见 plan §D4b）。
 *   D1   story_list defer 分支只装缓存（mcp:1062-1063），无「前台回落时以缓存重渲」
 *        消费方 -> P3 切页空态（t=18845 count=0）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；「home =」为冻结落点）:
 *
 *   [R1] 录音语境键手势归属判据（home = display/story_page.{c,h} additive 尾部追加;
 *        D4a/D2 同一设计决策内一致选定——plan §3。板级 keya_story_page_wants 是该判据
 *        的消费点）:
 *          int kidpal_story_key_owns(int front_layer, int kidpal_page,
 *                                    int page_state, int ctx_active);
 *            -- 返回 1 = 故事页拥有该键手势（板级路由进 HandleStoryKey，KEY 由
 *               kidpal_story_page_input 在 RECORDING 页态映 KSPV2_ACT_STOP_RECORD，
 *               **绝不**落 key_route_layer -> GLOBAL_ACT_SESSION_TOGGLE 毁稿路径）;
 *               0 = 交回全局路由。
 *            -- 规则（形参 = 判据的全输入）:
 *               (a) page_state == KSTORY_PAGE_RECORDING(2) && ctx_active != 0
 *                   -> 1（记录语境录音态; **形态无关**：甲形态下 VOICE 已让位、
 *                      front_layer 到此处已归一为 NONE，本支冗余但无害; 乙形态下
 *                      front_layer == VOICE，本支是判据命门。两形态实现同一 contract）;
 *               (b) front_layer == KIDPAL_LAYER_STORY_DELETE(6) -> 1（既有: 删除
 *                   覆盖层吞全手势，任何漏网手势会被通用覆盖层列误映射 SESSION_TOGGLE）;
 *               (c) front_layer == KIDPAL_LAYER_NONE(0) &&
 *                   kidpal_page == KP_PAGE_STORY(2) -> 1（既有 keya_story_page_wants
 *                   基层语义零偏移）;
 *               其余 -> 0（含：VOICE/其它覆盖层前台且非记录语境 / 基层非 P3 /
 *                   记录语境标志未置——K2/K3 主页面路径零回归）。
 *            -- K2 页内映射（RECORDING + KEY -> KSPV2_ACT_STOP_RECORD）本文件不重钉，
 *               由 K2 story_page_v2.acceptance.c 行为齿承载（must-stay-green）。
 *
 *   [R2] 会话收尾录音态复位判据（home = display/story_page.{c,h}; D4b-1）:
 *          int kidpal_story_session_end(int page_state, int *out_page);
 *            -- page_state == KSTORY_PAGE_RECORDING(2) -> *out_page =
 *               KSTORY_PAGE_LIST(0), 返回 1（调用方据此复位 g_story_view.page 并停
 *               录音定时器——页态回落 LIST 后 StoryRecTimerCb 的 page != RECORDING
 *               自删条件成立，定时器终停，mcp:111-116）;
 *               其余 -> *out_page 不改, 返回 0（幂等 no-op, 非录音态零动作）;
 *               out_page == NULL -> -1。
 *            -- 消费点（home = application.cc Idle 分支, 会话收尾复位录音页态） +
 *               录音定时器停止（mcp_server.cc 侧）。
 *
 *   [R3] defer 缓存消费判据（home = display/story_page.{c,h}; D1）:
 *          int kidpal_story_list_deferred_flush(int voice_front, int kidpal_page,
 *                                               int page_state);
 *            -- 返回 1 = 以 g_story_list 最新缓存重渲 P3 前台（defer 分支的缓存
 *               被消费）: voice_front == 0（VOICE 层已弹出/前台回落基层）&&
 *               kidpal_page == KP_PAGE_STORY(2) && page_state == KSTORY_PAGE_LIST(0);
 *               其余 -> 0（VOICE 仍前台不抢推 / 本地页非 P3 / 已入子态不覆盖）。
 *            -- 消费点（home = 前台回落路径: kidpal_render_node_voice_end 或 Idle
 *               分支——「VOICE 弹出 / 前台回落 LOCAL_PAGE 且本地页 == P3」时以缓存重渲,
 *               plan §1 D1 修法方向）。
 *
 *   [R4] LCD 声波 chrome 前台判据（home = display/story_page.{c,h}; D2 根因 1, 2026-10-01
 *        真机轮 REVISION）：D2「弹层 ≠ 隐屏」的决策单源。甲形态只弹逻辑层栈
 *        （kidpal_layer_pop）不改设备状态机——录音期间状态机仍 listening, LCD 照画
 *        listening 态（声波/聆听 chrome 驻留盖住录音页）。故 chrome 前台判据须含
 *        **录音页态维度**（不能只读层值）:
 *          int kidpal_story_voice_chrome_front(int layer_top, int page_state);
 *            -- 返回 1 = LCD 声波/聆听 chrome 应前台可见（姿态 + voice_page 整页容器）;
 *               0 = 退隐（录音页/内容页前台, 覆盖层不驻留）。
 *            -- 规则（形参 = 判据全输入）:
 *               (a) page_state == KSTORY_PAGE_RECORDING(2) -> 0（**录音态覆盖层不驻留**:
 *                   录音页态即令 chrome 退隐, 与 layer_top 无关——甲形态层已弹仍须防
 *                   状态机 listening 残影, 乙形态层未弹亦须显式退隐; 这是真机现场
 *                   「声波覆盖层一直盖着」的机判翻转点）;
 *               (b) layer_top == KIDPAL_LAYER_VOICE(3) -> 1（普通聆听轮 chrome 前台,
 *                   既有 RP-A5 r3 语义零偏移）;
 *               其余 -> 0（内容页/其它覆盖层前台, chrome 退隐——既有语义零偏移）。
 *            -- 判别位（对旧内联逻辑的翻转）: 旧逻辑 = `layer_top == VOICE`（无录音
 *               维度）——(VOICE, RECORDING) 旧答 1（chrome 驻留 = 缺陷）; 本判据答 0。
 *            -- 消费点（home = kidpal_render_node.cc rn_voice_chrome_front_locked:
 *               以本判据取代内联的 `layer_top == KIDPAL_LAYER_VOICE`, 喂当前前台层值
 *               + 当前会话页态; 且进录音态路径须触发一次 chrome 重算, 令退隐即时生效）。
 *
 *   [R5] voice_end 前台回落渲染页态解析判据（home = display/story_page.{c,h}; D2 根因 2,
 *        2026-10-01 真机轮 REVISION）：D2「页态同步须先于渲染」的决策单源。渲染节点
 *        （kidpal_render_node_voice_end, kidpal_render_node.cc:581）读自身缓存
 *        s_story_view.page, 但录音 enter 只更新权威会话页态 g_story_view.page=2
 *        （RECORDING）, 节点缓存尚未经 kidpal_render_node_cache_story 同步（同步点不在
 *        voice_end 路径）——按陈旧 page_state=0（LIST）渲出**列表页**而非录音页:
 *          int kidpal_story_render_page_state(int node_cached_page, int session_page,
 *                                             int on_story_front);
 *            -- 返回应据此渲染 P3 的页态。
 *            -- 规则: on_story_front != 0（本地页 == P3 前台回落）-> session_page
 *               （**以权威会话页态为准**, 节点缓存落后亦取真值——页态同步先于渲染）;
 *               其余 -> node_cached_page（非 P3 前台不加干预, 零回归）。
 *            -- 判别位（对旧内联逻辑的翻转）: 旧逻辑 = 直接用 s_story_view.page（节点
 *               缓存）——(node=LIST, session=RECORDING, front=1) 旧答 LIST（渲列表页 =
 *               缺陷）; 本判据答 RECORDING。
 *            -- 消费点（home = kidpal_render_node.cc kidpal_render_node_voice_end:
 *               渲 P3 前以本判据解析页态, 并把解析结果喂 rn_render_kidpal_page_locked
 *               与 [R3] 的 page_state 入参——取代裸读 s_story_view.page）。
 *            -- session_page 可达机制（builder 择一, 契约只钉决策不钉机制）: 渲染节点
 *               自身不含权威会话视图（g_story_view 住 mcp_server.cc, 节点只持缓存
 *               s_story_view）——builder 可 (i) 在录音 enter 路径先 kidpal_render_node_
 *               cache_story(&g_story_view,...) 预同步, 再调 voice_end（此时两参一致,
 *               判据仍恒取权威值）; 或 (ii) 经镜像 setter / 形参把权威页态喂入节点。
 *               两机制均须经本判据落渲染页态（真值 = 渲染页态由本判据产出）。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: K3R-A1 -- 录音语境键手势归属（[R1] 决策表正/负 + 板级消费接线）
 *             -> RSR-A1
 *   coverage: K3R-A2 -- D2 层栈让位形态无关（[R1] 对 VOICE 前台/无前台两态一致 +
 *             录音路径让位接线）-> RSR-A2
 *   coverage: K3R-A2 -- D2 录音态覆盖层不驻留（[R4] LCD 声波 chrome 前台判据: 录音页态
 *             令 chrome 退隐, 与层值无关 = 「弹层 ≠ 隐屏」机判翻转 + render_node 消费点）
 *             -> RSR-A6
 *   coverage: K3R-A2 -- D2 voice_end 渲染页态同步（[R5] 前台回落页态解析: 以权威会话
 *             页态取代渲染节点陈旧缓存 = 「页态同步先于渲染」机判翻转 + voice_end 消费点）
 *             -> RSR-A7
 *   coverage: K3R-A3 -- 会话收尾复位（[R2] 行为 + Idle 消费 + 定时器自删条件在世）
 *             -> RSR-A3
 *   coverage: K3R-A4 -- 渲染不变量（host_infra_render_p1_frame 对象有效性守卫）
 *             -> RSR-A4
 *   coverage: K3R-A5 -- defer 缓存消费方（[R3] 行为 + 消费点接线）-> RSR-A5
 *   coverage: K3R-C1 -- canary + K3 冻结面在世 + 注册序守卫（回归守卫）
 *             -> RSR-C1
 *
 * ---------------------------------------------------------------------------
 * Run protocol:
 *   0) builder FIRST ACTION: 本文件注册进
 *      firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS
 *      （相对 host_app/main 为 "../../record_session_repair.acceptance.c"），**置于
 *      "../../host_infra_smoke.acceptance.c" 之前**（smoke 末位 = 最先执行, 注册序
 *      守卫; RSR-C1 tooth 4 校验此序）。[R1]-[R5] 强符号落 display/story_page.c
 *      （host runner 已编入）——[R1]-[R3] 零被测实现新接线（结构齿扫真实工作树）;
 *      [R4]/[R5] 为 2026-10-01 真机轮「补严齿」新增（D2 两条根因）, 需 builder 在
 *      display/story_page.{c,h} 落强符号并在 kidpal_render_node.cc 消费点接线。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/
 *      host_infra_test.elf
 *   红基线独立通道（acceptance-author 冻结期, 非冻结门 runner）:
 *     wsl -d Ubuntu-24.04 -- bash firmware/main/test/acceptance/red-baseline/
 *       rsr_red_wrapper.sh     （unity stub + runner.c, 无实现链接; exit code =
 *                               失败 TEST_CASE 数）
 *   RED baseline 判读口径见文件尾。
 * ---------------------------------------------------------------------------
 * 强度申报（契约条目 -> 承载断言 -> 性质档; 弱于契约处逐条）:
 *   K3R-A1 键手势归属 -> RSR-A1 行为级（[R1] 决策表全承载, host runner 真调）+
 *         结构级（板级 keya_story_page_wants 区消费 kidpal_story_key_owns needle）。
 *         **弱于契约**: 板级前置分流（keya_story_page_wants -> HandleStoryKey）为
 *         设备专属编排（board .cc 不可 host 编），其 needle 为**源锚代理**——行为等价
 *         假设 = 板级判据区消费 [R1] 单源即承载归属决策; 真机「按 KEY 落 STOP_RECORD
 *         不产 SESSION_TOGGLE」逐字复证挂设备轮（M4K3R-E3 窗 3）。
 *   K3R-A2 D2 层栈让位 -> RSR-A2 行为级（[R1] 对 front_layer=VOICE 与 NONE 两态给同一
 *         归属结论 = 形态无关）+ 结构级（录音路径让位 needle）。**形态相关齿**
 *         （spine q4 甲/乙二选一由 builder 定）: 甲（进录音态弹 VOICE）-> 录音路径
 *         出现 VOICE 让位调用; 乙（录音页提到 VOICE 之上）-> 录音路径出现层提升调用。
 *         本齿断言**可观察结果**（录音中故事页归属不因 VOICE 前台而失效），不钉具体
 *         形态; 视觉「VOICE 不压顶/录音页可见」为设备轮目击（host 无真层合成）。
 *   K3R-A2 D2 录音态覆盖层不驻留 -> RSR-A6 行为级（[R4] chrome 前台判据决策表全承载,
 *         **判别位 = (VOICE, RECORDING) 由旧答 1 翻转为 0**——真机「声波覆盖层一直盖着」
 *         的机判翻转点）+ 结构级（kidpal_render_node.cc 消费点 needle）。**强于旧
 *         RSR-A2**: 旧齿只测 [R1] 两态同结论（形态无关性）, 测不到屏上覆盖层是否退隐;
 *         本齿直接钉 chrome 退隐判据须含录音页态维度。**弱于契约**: LCD 真层合成 +
 *         状态机 listening 残影的真实消除为设备运行态, host 无真层合成——决策函数输出为
 *         行为缝, 真机屏幕目击挂设备轮 M4K3R-E3 窗 2。
 *   K3R-A2 D2 voice_end 页态同步 -> RSR-A7 行为级（[R5] 页态解析判据决策表全承载,
 *         **判别位 = (node=LIST, session=RECORDING, front=1) 由旧答 LIST 翻转为
 *         RECORDING**——真机「渲列表页而非录音页」的机判翻转点）+ 结构级
 *         （kidpal_render_node.cc voice_end 消费点 needle）。**弱于契约**: 渲染节点
 *         s_story_view/g_story_view 为设备侧状态（render_node 不可 host 编）, host 以纯
 *         逻辑判据输出为行为缝; 真机「P3 显示录音页（非列表页）」逐字复证挂设备轮
 *         M4K3R-E3 窗 2。
 *   K3R-A3 会话收尾复位 -> RSR-A3 行为级（[R2] 决策表全承载）+ 结构级（Idle 消费 +
 *         定时器自删条件 must-stay-green）。**弱于契约**: 录音定时器生命周期为设备
 *         运行态（lv_timer 不可 host 编），「定时器必停」由「页态复位 -> 自删条件成立」
 *         的因果链 + 设备轮目击承载; [R2] 返回 1 = 调用方停定时器的契约信号。
 *   K3R-A4 渲染不变量 -> RSR-A4 结构级（host_infra_render_p1_frame 函数区消费
 *         lv_obj_is_valid needle）。**弱于契约**: 真崩溃路径（外部 kidpal_*_page_render
 *         的 lv_obj_clean 令 s_kidpal_img 悬垂）为设备运行态，host 无真 LVGL 对象图；
 *         结构齿保证守卫在位，真不变量由设备轮 + LVGL panic 不复现复证。
 *   K3R-A5 defer 消费方 -> RSR-A5 行为级（[R3] 决策表全承载）+ 结构级（消费点 needle）。
 *         **弱于契约**: 真机「P3 切页显示后端条目」逐字复证挂设备轮（M4K3R-E3 窗 1）。
 *   fixture 自含性: 全部行为齿 fixture = 纯值域入参（int）, 无资产/时间真值/环境前提;
 *         结构齿依赖仓根定位（cwd 相对探测, K3 rsl_locate_root 同款——runner 须自仓根或
 *         host_app 目录启动, 各结构齿 REQUIRE_ROOT 前置）。
 *   盲区申报: ① 真机四窗（列表显示 / 录音页非声波层 / KEY 归档不崩溃 / 回归）=
 *         设备轮 M4K3R-E3; ② AFE VAD 真沿切页与层合成 = 设备轮; ③ 定时器真实生命周期
 *         = 设备轮; ④ [R1] 在板级被消费后的真实键路由（真按 KEY 出 stop_rec 日志）=
 *         设备轮（串口 story_key stop_rec archive_pend= 目击）; ⑤ **D2 的 host 边界
 *         （2026-10-01 真机轮 REVISION 显式化）**: LCD 真层合成（voice_page 在
 *         lv_layer_top 覆盖活动屏）+ 设备状态机 listening 残影的真实消除 + 渲染节点
 *         s_story_view/g_story_view 真实缓存竞态 —— 三者均为设备运行态, host 无真层
 *         合成、render_node/lcd_display 不可 host 编。本文件以 [R4]/[R5] 纯逻辑判据的
 *         **决策输出**为行为缝（判别位对旧内联逻辑翻转, 见各齿）+ 消费点 needle; 但
 *         「判据正确 + 消费点在世」仍**不充分证明屏上覆盖层真的没了**（旧 RSR-A2 正是
 *         栽在此不充分上）——设备轮 M4K3R-E3 窗 2（P3 KEY -> 须见录音页与「● 录音中」,
 *         不再是声波层）为不可省目击。
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

/* RSR_UNIMPLEMENTED = weak 哨兵返回值（永不是合法返回值; K3 RSL_UNIMPLEMENTED 同款）。
 * display/story_page.c 已编入 host runner -> builder 落 [R1]-[R3] 强符号后哨兵被覆盖
 * 转绿; 独立红跑通道（red-baseline/unity.h stub）无实现 -> 哨兵恒红 = 「frozen
 * contract not implemented」设计态红（非收集错/链接错）。 */
#define RSR_UNIMPLEMENTED (-2)

/* ============================ 冻结数值镜像 ================================ */
/* KSTORY_PAGE_* = display/story_page.h 冻结枚举。本文件不 include story_page.h
 * （独立红跑通道 include 路径最小化, K3 同款; 数值漂移时 K2 SPV2 系列齿先红 +
 * RSR-C1 needle 守卫），以字面镜像 + 注释锚定。 */
#define RSR_PAGE_LIST      0 /* KSTORY_PAGE_LIST */
#define RSR_PAGE_READER    1 /* KSTORY_PAGE_READER */
#define RSR_PAGE_RECORDING 2 /* KSTORY_PAGE_RECORDING */
#define RSR_PAGE_STAMP     3 /* KSTORY_PAGE_STAMP */
#define RSR_PAGE_DELETE    4 /* KSTORY_PAGE_DELETE */
/* KP_PAGE_* = kidpal_render_node.h 冻结枚举（M2K2-A2 齿钉等值: HOME=0/SCHEDULE=1/
 * STORY=2; 本文件镜像消费, 漂移由 M2K2-A2 先红）。 */
#define RSR_KP_PAGE_HOME     0
#define RSR_KP_PAGE_SCHEDULE 1
#define RSR_KP_PAGE_STORY    2
/* KIDPAL_LAYER_* = display/kidpal_layer.h 冻结数值码（M2-K3 + M2-K2 [K10] 留痕修订;
 * 全仓消费点均符号常量引用）。 */
#define RSR_LAYER_NONE         0 /* KIDPAL_LAYER_NONE */
#define RSR_LAYER_LOCAL_PAGE   2 /* KIDPAL_LAYER_LOCAL_PAGE */
#define RSR_LAYER_VOICE        3 /* KIDPAL_LAYER_VOICE */
#define RSR_LAYER_STORY_DELETE 6 /* KIDPAL_LAYER_STORY_DELETE */
#define RSR_LAYER_REMINDER     7 /* KIDPAL_LAYER_REMINDER */

/* 实现面文件锚（结构齿扫描对象; 相对仓根）。 */
#define RSR_BOARD_CC  "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"
#define RSR_MCP_CC    "firmware/main/mcp_server.cc"
#define RSR_APP_CC    "firmware/main/application.cc"
#define RSR_RN_CC     "firmware/main/display/kidpal_render_node.cc"
#define RSR_STORY_H   "firmware/main/display/story_page.h"
#define RSR_K3_C      "firmware/main/test/record_session_live.acceptance.c"
#define RSR_CMAKE     "firmware/main/test/host_app/main/CMakeLists.txt"
#define RSR_REQ_MD    "docs/requirements.md"

/* ============================ 扫描助手（K3 同款形态） ===================== */

#define RSR_MAX_PATH 1024
#define RSR_MAX_READ (8u * 1024u * 1024u)

static char rsr_root[RSR_MAX_PATH];
static int rsr_root_ready = 0;

static int rsr_is_regular(const char *p)
{
    struct stat st;
    return stat(p, &st) == 0 && S_ISREG(st.st_mode);
}

static int rsr_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[RSR_MAX_PATH];
    size_t i;

    if (rsr_root_ready) {
        return 1;
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (rsr_is_regular(probe)) {
            snprintf(rsr_root, sizeof(rsr_root), "%s", cands[i]);
            rsr_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

#define RSR_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(rsr_locate_root(), \
    "RSR RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app")

static char *rsr_read(const char *rel, size_t *out_len)
{
    char path[RSR_MAX_PATH];
    FILE *f;
    char *buf;
    size_t n;

    if (!rsr_root_ready && !rsr_locate_root()) {
        return NULL;
    }
    snprintf(path, sizeof(path), "%s/%s", rsr_root, rel);
    f = fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }
    buf = (char *)malloc(RSR_MAX_READ);
    assert(buf != NULL);
    n = fread(buf, 1, RSR_MAX_READ - 1, f);
    fclose(f);
    buf[n] = '\0';
    if (out_len != NULL) {
        *out_len = n;
    }
    return buf;
}

static int rsr_contains(const char *hay, const char *needle)
{
    return hay != NULL && needle != NULL && strstr(hay, needle) != NULL;
}

static int rsr_file_contains(const char *rel, const char *needle)
{
    char *buf = rsr_read(rel, NULL);
    int ok = rsr_contains(buf, needle);

    free(buf);
    return ok;
}

/* 文件内 [start_needle, end_needle) 切片（start 缺失 -> NULL; end 缺失 -> 切到 EOF;
 * K3 rsl_region_between 同款——锚漂移 = 齿自身 RED(infra) 须复核）。返回 malloc 串,
 * 调用方 free。 */
static char *rsr_region_between(const char *rel, const char *start_needle,
                                const char *end_needle)
{
    char *buf = rsr_read(rel, NULL);
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

static int rsr_region_contains(const char *rel, const char *start_needle,
                               const char *end_needle, const char *needle)
{
    char *region = rsr_region_between(rel, start_needle, end_needle);
    int ok = rsr_contains(region, needle);

    free(region);
    return ok;
}

/* OR 组合的针集扫描（D2 形态无关齿用——任一针命中即成立, 不钉具体让位机制）。 */
static int rsr_region_contains_any(const char *rel, const char *start_needle,
                                   const char *end_needle,
                                   const char *const *needles, int n)
{
    char *region = rsr_region_between(rel, start_needle, end_needle);
    int i;
    int hit = 0;

    for (i = 0; i < n; i++) {
        if (rsr_contains(region, needles[i])) {
            hit = 1;
            break;
        }
    }
    free(region);
    return hit;
}

static char rsr_fm_buf[RSR_MAX_PATH * 2];
static const char *rsr_fm(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(rsr_fm_buf, sizeof(rsr_fm_buf), fmt, ap);
    va_end(ap);
    return rsr_fm_buf;
}

/* ============================ [R1]-[R3] weak 哨兵 ========================= */
/* builder 在 display/story_page.c（host runner 已编入源清单）落强符号后覆盖; 独立
 * 红跑通道无实现 -> 恒哨兵 = 设计态红（非收集错/链接错）。签名漂移 = 链接期显式失败
 * （比哨兵假绿更响——K3 同款纪律）。 */

__attribute__((weak)) int kidpal_story_key_owns(int front_layer, int kidpal_page,
                                                 int page_state, int ctx_active)
{
    (void)front_layer; (void)kidpal_page; (void)page_state; (void)ctx_active;
    return RSR_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_session_end(int page_state, int *out_page)
{
    (void)page_state; (void)out_page;
    return RSR_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_list_deferred_flush(int voice_front,
                                                            int kidpal_page,
                                                            int page_state)
{
    (void)voice_front; (void)kidpal_page; (void)page_state;
    return RSR_UNIMPLEMENTED;
}

/* [R4]/[R5] = 2026-10-01 真机轮 REVISION 新增（D2 两条根因）。同样由 display/
 * story_page.c 强符号覆盖。签名漂移 = 链接期显式失败（比哨兵假绿更响）。 */
__attribute__((weak)) int kidpal_story_voice_chrome_front(int layer_top, int page_state)
{
    (void)layer_top; (void)page_state;
    return RSR_UNIMPLEMENTED;
}

__attribute__((weak)) int kidpal_story_render_page_state(int node_cached_page,
                                                          int session_page,
                                                          int on_story_front)
{
    (void)node_cached_page; (void)session_page; (void)on_story_front;
    return RSR_UNIMPLEMENTED;
}

/* ========================================================================== *
 * TEST 1 — RSR-A1: 录音语境键手势归属 [R1]（D4a 正/负双齿; seam [R1]）。
 * RED today: [R1] 哨兵 -2（判据未实现）+ 板级 keya_story_page_wants 区零消费。
 * ========================================================================== */
/* coverage: K3R-A1 -- 录音语境键手势归属（[R1] 决策表: 记录语境录音态正锚（两形态）+
 *           非记录语境/覆盖层/基层负锚 + 删除覆盖层与基层既有语义 + 板级消费接线） */
/* coverage: S-F2.2-AC4 -- KEY 兜底收尾=保存（录音中 KEY 归故事页 -> STOP_RECORD, 绝不落
 *           GLOBAL_ACT_SESSION_TOGGLE 毁稿路径; 页内映射由 K2 承载） */
TEST_CASE("record_session_repair: 录音语境键手势归属 [R1] (RSR-A1)",
          "[record_session_repair][RSR-A1][behavior][structural]")
{
    RSR_REQUIRE_ROOT();

    /* tooth 1 (正锚 · 乙形态 runtime state): VOICE 层前台 + 记录语境录音态 -> 故事页
     * 拥有 KEY（乙形态下 VOICE 不弹、录音页提到其上的 runtime 状态）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_key_owns(RSR_LAYER_VOICE, RSR_KP_PAGE_STORY,
                              RSR_PAGE_RECORDING, 1) == 1,
        "RSR-A1 RED: [R1] 记录语境录音态（VOICE 前台）未判故事页拥有 KEY（期望 1）"
        "——D4a 毁稿根: 判据不成立 -> KEY 落 key_route_layer -> "
        "GLOBAL_ACT_SESSION_TOGGLE -> Listening 关通道不发 listen stop -> front "
        "ws_disconnect 丢弃（哨兵 -2 = frozen seam [R1] 未实现）");

    /* tooth 2 (正锚 · 甲形态 runtime state): VOICE 层已让位（归一为基层）+ 记录语境
     * 录音态 -> 故事页拥有 KEY（甲形态下 VOICE 已弹、front_layer 到此处为 0）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_key_owns(RSR_LAYER_NONE, RSR_KP_PAGE_STORY,
                              RSR_PAGE_RECORDING, 1) == 1,
        "RSR-A1 RED: [R1] 记录语境录音态（VOICE 已让位）未判故事页拥有 KEY（期望 1）"
        "——甲形态录音中 KEY 归属缺失（两形态须一致给 1, 形态无关）");

    /* tooth 3 (负锚 · 零偏移): 非记录语境不得扩大故事页归属——VOICE 前台普通会话 /
     * 基层非 P3 / 更新层覆盖 / 记录语境标志未置, 一律 0（交回全局路由, K2/K3 零回归）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_key_owns(RSR_LAYER_VOICE, RSR_KP_PAGE_STORY,
                              RSR_PAGE_LIST, 0) == 0,
        "RSR-A1 RED(负锚): VOICE 前台 + 非记录语境被误判故事页拥有（期望 0）——"
        "普通会话/引导态 KEY 归属被侵占（K2/K3 主页面路径零回归红线）");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_key_owns(RSR_LAYER_VOICE, RSR_KP_PAGE_STORY,
                              RSR_PAGE_RECORDING, 0) == 0,
        "RSR-A1 RED(负锚): 录音页态但记录语境标志未置（ctx_active=0）被误判拥有"
        "（期望 0）——唤醒词普通会话切到录音页不得吞 KEY（ctx 门不得缺）");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_key_owns(RSR_LAYER_NONE, RSR_KP_PAGE_SCHEDULE,
                              RSR_PAGE_LIST, 0) == 0,
        "RSR-A1 RED(负锚): 基层非 P3 被误判故事页拥有（期望 0）——基层判据须为 P3");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_key_owns(RSR_LAYER_REMINDER, RSR_KP_PAGE_STORY,
                              RSR_PAGE_LIST, 0) == 0,
        "RSR-A1 RED(负锚): 更高覆盖层（提醒）前台被误判故事页拥有（期望 0）——"
        "提醒弹窗 KEY 归属不得被故事页侵占");

    /* tooth 4 (既有语义零偏移): 删除覆盖层吞全手势 + 基层 P3 故事页归属。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_key_owns(RSR_LAYER_STORY_DELETE, RSR_KP_PAGE_STORY,
                              RSR_PAGE_DELETE, 0) == 1,
        "RSR-A1 RED: [R1] 删除确认覆盖层未判故事页拥有（期望 1）——删除子态漏网手势"
        "会被通用覆盖层列误映射 SESSION_TOGGLE（mcp:197-206 既有语义零偏移）");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_key_owns(RSR_LAYER_NONE, RSR_KP_PAGE_STORY,
                              RSR_PAGE_LIST, 0) == 1,
        "RSR-A1 RED: [R1] 基层 P3 故事页未判拥有（期望 1）——K2 基层页内手势族"
        "零回归红线");

    /* tooth 5 (结构 · 源锚代理): 板级消费 [R1] 单源——归属判据有真实驱动方（不是只有
     * 纯逻辑函数）。落点 = keya_story_page_wants 前置分流判据（文档化落点），但本齿
     * 容放整板文件命中（builder 亦可从键手势回调直接喂入——不钉具体调用点）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsr_file_contains(RSR_BOARD_CC, "kidpal_story_key_owns("),
        "RSR-A1 RED: 板级（waveshare-s3-rlcd-4.2.cc）未消费 kidpal_story_key_owns——"
        "归属判据无驱动方（纯逻辑函数无消费方 = D4a 板级接线盲区; 须由板级判据喂入 "
        "[R1] 单源）");
}

/* ========================================================================== *
 * TEST 2 — RSR-A2: D2 层栈让位形态无关（seam [R1] 对两形态一致 + 录音路径让位）。
 * RED today: [R1] 哨兵 -2 + 录音路径零让位接线。
 * ========================================================================== */
/* coverage: K3R-A2 -- D2 层栈让位（[R1] 对 front_layer=VOICE/NONE 两态给同一归属结论,
 *           形态无关; 录音 VAD 桥路径让位接线; 不含逐句回显内容=Package B） */
TEST_CASE("record_session_repair: D2 层栈让位形态无关 (RSR-A2)",
          "[record_session_repair][RSR-A2][behavior][structural]")
{
    int with_voice;
    int without_voice;

    RSR_REQUIRE_ROOT();

    /* tooth 1 (行为 · 形态无关核心): 记录语境录音态的归属结论不得依赖 VOICE 是否前台
     * （甲=VOICE 让位、乙=录音页提到 VOICE 之上; 两形态 runtime 快照的归属结论必须同为
     * 1）。此即「VOICE 不再压住录音页」的可机判后果——录音页在记录语境下始终是可操作
     * 的前台内容。 */
    with_voice = kidpal_story_key_owns(RSR_LAYER_VOICE, RSR_KP_PAGE_STORY,
                                       RSR_PAGE_RECORDING, 1);
    without_voice = kidpal_story_key_owns(RSR_LAYER_NONE, RSR_KP_PAGE_STORY,
                                          RSR_PAGE_RECORDING, 1);
    TEST_ASSERT_TRUE_MESSAGE(
        with_voice == 1 && without_voice == 1 && with_voice == without_voice,
        rsr_fm("RSR-A2 RED: 记录语境录音态的归属依赖 VOICE 前台形态（VOICE 前台=%d / "
               "无 VOICE=%d, 期望同为 1）——D2 让位未达成形态无关（甲弹 VOICE / 乙提"
               "录音页两形态须给同一归属结论; 哨兵 -2 = frozen seam [R1] 未实现）",
               with_voice, without_voice));

    /* tooth 2 (结构 · 形态相关, 源锚代理): 录音路径出现层让位/提升接线——两形态各自
     * 期望: 甲 -> VOICE 让位调用（如 kidpal_render_node_voice_end / VOICE pop）;
     * 乙 -> 录音页提到 VOICE 之上的层操作。任一形态、任一落点（录音 VAD 桥
     * KidpalStoryVadEnterRecording / application.cc VAD 分支）命中即成立（不钉形态）。 */
    {
        static const char *const needles[] = {
            "kidpal_render_node_voice_end", /* 甲: 弹 VOICE 层 */
            "KIDPAL_LAYER_VOICE",           /* 甲/乙: 层值出现于录音路径 */
            "kidpal_story_layer",           /* 乙: 录音页层提升辅助 */
            "kidpal_layer_",                /* 通用: 层栈操作 */
        };
        int n_n = (int)(sizeof(needles) / sizeof(needles[0]));
        int in_bridge = rsr_region_contains_any(
            RSR_MCP_CC, "void KidpalStoryVadEnterRecording()",
            "static std::string mcp_json_get_string", needles, n_n);
        int in_vad_branch = rsr_region_contains_any(
            RSR_APP_CC, "if (bits & MAIN_EVENT_VAD_CHANGE) {",
            "if (bits & MAIN_EVENT_SCHEDULE) {", needles, n_n);
        TEST_ASSERT_TRUE_MESSAGE(
            in_bridge || in_vad_branch,
            "RSR-A2 RED: 录音路径（KidpalStoryVadEnterRecording / application.cc VAD "
            "分支）零层让位/提升接线——D2 进录音态 VOICE 层未让位（录音页被压在下层, "
            "孩子看不到录音页; 甲=弹 VOICE / 乙=录音页提到 VOICE 之上, 两形态须至少"
            "一处接线）");
    }
}

/* ========================================================================== *
 * TEST 3 — RSR-A3: 会话收尾复位 [R2]（D4b-1; seam [R2] + Idle 消费 + 定时器自删条件）。
 * RED today: [R2] 哨兵 -2 + application.cc Idle 分支零 kidpal_story_session_end 消费。
 * ========================================================================== */
/* coverage: K3R-A3 -- 会话收尾复位（[R2] 决策表: 录音态复位 LIST + 非录音态幂等 + NULL
 *           防御 + Idle 消费接线 + 录音定时器自删条件 must-stay-green） */
TEST_CASE("record_session_repair: 会话收尾复位录音态 [R2] (RSR-A3)",
          "[record_session_repair][RSR-A3][behavior][structural]")
{
    int page;
    int rc;

    RSR_REQUIRE_ROOT();

    /* tooth 1 (正锚): 录音态 -> 复位 LIST + 返回 1（调用方停录音定时器）。 */
    page = -7; /* 脏初值 —— 命中时须被改写, 未命中不得误写 */
    rc = kidpal_story_session_end(RSR_PAGE_RECORDING, &page);
    TEST_ASSERT_TRUE_MESSAGE(
        rc == 1 && page == RSR_PAGE_LIST,
        rsr_fm("RSR-A3 RED: 会话收尾未复位录音页态（rc=%d page=%d, 期望 rc=1 page=0）"
               "——D4b 根: 页态不回落 -> 录音定时器 page!=RECORDING 自删条件恒不成立 "
               "-> 每 100ms 清屏砸毁首页图像对象 -> 悬垂 LVGL panic（哨兵 -2 = frozen "
               "seam [R2] 未实现）",
               rc, page));

    /* tooth 2 (幂等): 非录音态 -> 返回 0, out_page 不改（会话收尾重复调用安全）。 */
    page = 42;
    rc = kidpal_story_session_end(RSR_PAGE_LIST, &page);
    TEST_ASSERT_TRUE_MESSAGE(
        rc == 0 && page == 42,
        rsr_fm("RSR-A3 RED: 非录音态被误复位/误写（rc=%d page=%d, 期望 rc=0 page 不变=42）"
               "——非录音态收尾须幂等 no-op（不得误清列表/阅读/删除子态）",
               rc, page));

    /* tooth 3 (防御): NULL out -> -1。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_session_end(RSR_PAGE_RECORDING, NULL) == -1,
        "RSR-A3 RED: out_page=NULL 未拒绝（期望 -1）——畸形入参不得崩");

    /* tooth 4 (边界): 其它页态一律不误复位（READER/STAMP/DELETE 收尾零动作）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_session_end(RSR_PAGE_READER, &page) == 0 &&
        kidpal_story_session_end(RSR_PAGE_DELETE, &page) == 0 &&
        kidpal_story_session_end(RSR_PAGE_STAMP, &page) == 0,
        "RSR-A3 RED: 非录音页态被误复位（期望全 0）——收尾复位只应命中录音态");

    /* tooth 5 (结构 · 源锚代理): application.cc Idle 分支消费 [R2]（会话收尾复位录音
     * 页态的唯一落点; 现状 Idle 分支只清 ctx + s_story_rec_entered, application.cc:970-974）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsr_region_contains(RSR_APP_CC, "case kDeviceStateIdle:",
                            "case kDeviceStateConnecting:",
                            "kidpal_story_session_end("),
        "RSR-A3 RED: application.cc Idle 收尾分支未消费 kidpal_story_session_end——"
        "会话收尾未复位录音页态（D4b-1 缺陷现场: 收尾只清 ctx 与 s_story_rec_entered, "
        "application.cc:970-974）");

    /* tooth 6 (结构 · must-stay-green): 录音定时器自删条件在世——页态回落 LIST 后
     * StoryRecTimerCb 的 page != RECORDING 自删分支须仍存在（[R2] 复位 -> 定时器终停
     * 的因果链前提）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsr_region_contains(RSR_MCP_CC, "void StoryRecTimerCb(lv_timer_t* timer)",
                            "void StartStoryRecTimer()", "KSTORY_PAGE_RECORDING"),
        "RSR-A3 RED(infra): StoryRecTimerCb 丢失 KSTORY_PAGE_RECORDING 自删条件——"
        "定时器终停前提被移除（must-stay-green; 须复核）");
}

/* ========================================================================== *
 * TEST 4 — RSR-A4: 渲染不变量（D4b-3; host_infra_render_p1_frame 对象有效性守卫）。
 * RED today: 函数区零 lv_obj_is_valid。
 * ========================================================================== */
/* coverage: K3R-A4 -- 渲染不变量（host_infra_render_p1_frame 加对象有效性守卫, 防悬垂
 *           对象出帧致 LVGL panic） */
TEST_CASE("record_session_repair: 渲染不变量对象有效性守卫 (RSR-A4)",
          "[record_session_repair][RSR-A4][structural]")
{
    RSR_REQUIRE_ROOT();

    /* tooth 1: host_infra_render_p1_frame 函数区消费 lv_obj_is_valid（对 s_kidpal_img
     * 出帧前有效性守卫）——外部 kidpal_*_page_render 的 lv_obj_clean 令 s_kidpal_img
     * 悬垂时, 不得以悬垂对象 lv_image_set_src 出帧（backtrace: lv_obj_get_screen <-
     * lv_obj_invalidate <- lv_image_set_src <- host_infra_render_p1_frame）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsr_region_contains(RSR_RN_CC,
                            "host_infra_render_p1_frame(const uint32_t *argb, int w, int h)",
                            "art_render_blit_frame(const uint32_t *argb, int w, int h)",
                            "lv_obj_is_valid"),
        "RSR-A4 RED: host_infra_render_p1_frame 函数区零 lv_obj_is_valid——渲染节点对"
        "s_kidpal_img 出帧前无对象有效性守卫（D4b-3: 外部 page_render 的 lv_obj_clean "
        "令图像对象悬垂 -> lv_image_set_src -> LoadProhibited panic 重启）");

    /* tooth 2 (结构 · must-stay-green): 出帧前 s_kidpal_img NULL 守卫仍在（既有崩修
     * 语义零回退）, 且出帧操作 lv_image_set_src 在区。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsr_region_contains(RSR_RN_CC,
                            "host_infra_render_p1_frame(const uint32_t *argb, int w, int h)",
                            "art_render_blit_frame(const uint32_t *argb, int w, int h)",
                            "s_kidpal_img == NULL") &&
        rsr_region_contains(RSR_RN_CC,
                            "host_infra_render_p1_frame(const uint32_t *argb, int w, int h)",
                            "art_render_blit_frame(const uint32_t *argb, int w, int h)",
                            "lv_image_set_src"),
        "RSR-A4 RED(infra): host_infra_render_p1_frame 丢失 s_kidpal_img NULL 守卫或"
        "lv_image_set_src 出帧操作——崩修语义零回退前提被破（must-stay-green）");
}

/* ========================================================================== *
 * TEST 5 — RSR-A5: defer 缓存消费方 [R3]（D1; seam [R3] + 前台回落消费点）。
 * RED today: [R3] 哨兵 -2 + 前台回落路径零消费。
 * ========================================================================== */
/* coverage: K3R-A5 -- defer 缓存消费方（[R3] 决策表: VOICE 弹出 + 本地页 P3 + 列表态 ->
 *           重渲; 消费点接线） */
TEST_CASE("record_session_repair: defer 缓存消费方 [R3] (RSR-A5)",
          "[record_session_repair][RSR-A5][behavior][structural]")
{
    RSR_REQUIRE_ROOT();

    /* tooth 1 (正锚): VOICE 已弹出（前台回落基层）+ 本地页 P3 + 列表态 -> 以缓存重渲
     * （1）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_list_deferred_flush(0, RSR_KP_PAGE_STORY, RSR_PAGE_LIST) == 1,
        "RSR-A5 RED: 前台回落 P3 时未判以 defer 缓存重渲（期望 1）——D1 现场: defer "
        "分支只装缓存无消费方 -> P3 切页空态 count=0（哨兵 -2 = frozen seam [R3] 未实现）");

    /* tooth 2 (负锚): VOICE 仍前台不抢推 / 本地页非 P3 / 已入子态不覆盖, 一律 0。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_list_deferred_flush(1, RSR_KP_PAGE_STORY, RSR_PAGE_LIST) == 0,
        "RSR-A5 RED(负锚): VOICE 仍前台时误判重渲（期望 0）——记录语境前台不得被"
        "story_list 帧盖掉（defer 防护半）");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_list_deferred_flush(0, RSR_KP_PAGE_HOME, RSR_PAGE_LIST) == 0,
        "RSR-A5 RED(负锚): 本地页非 P3 时误判重渲（期望 0）——非 P3 前台不得被 P3 内容覆盖");
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_list_deferred_flush(0, RSR_KP_PAGE_STORY, RSR_PAGE_RECORDING) == 0,
        "RSR-A5 RED(负锚): 录音态时误判重渲（期望 0）——子态不得被列表缓存覆盖");

    /* tooth 3 (结构 · 源锚代理): 消费点接线——前台回落路径（kidpal_render_node.cc）或
     * mcp 侧消费 [R3] 单源（defer 缓存有真实驱动方）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsr_file_contains(RSR_RN_CC, "kidpal_story_list_deferred_flush(") ||
        rsr_file_contains(RSR_MCP_CC, "kidpal_story_list_deferred_flush("),
        "RSR-A5 RED: 前台回落路径未消费 kidpal_story_list_deferred_flush——defer 缓存"
        "无驱动方（纯逻辑函数无消费方 = D1 列表缺陷盲区; 须在 VOICE 弹出/前台回落 P3 "
        "时以缓存重渲）");
}

/* ========================================================================== *
 * TEST 6 — RSR-C1: canary + K3 冻结面在世 + 注册序守卫（structural guards）。
 * RED today: 注册序 needle 零命中（本文件未进 HOST_ACCEPTANCE_SRCS——builder duty）;
 * canary/K3 面守卫现状绿（must-stay-green）。
 * ========================================================================== */
/* coverage: K3R-C1 -- canary 与守卫（requirements S-F2.2 锚 + story_page.h 动作码语义位
 *           零偏移 + K3 冻结验收件在世 + HOST_ACCEPTANCE_SRCS 注册序） */
TEST_CASE("record_session_repair: canary+注册序守卫 (RSR-C1)",
          "[record_session_repair][RSR-C1][structural]")
{
    char *cm;
    const char *p_self, *p_smoke;

    RSR_REQUIRE_ROOT();

    /* tooth 1: spec canary（S-F2.2 节与 kidpal_session_ctx 锚在世——story 语义权威未漂移;
     * 本包 requirements 面零改动）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsr_file_contains(RSR_REQ_MD, "S-F2.2") &&
        rsr_file_contains(RSR_REQ_MD, "kidpal_session_ctx"),
        "RSR-C1 RED: requirements.md 丢失 S-F2.2 节或 kidpal_session_ctx 锚——story 语义"
        "权威漂移（须复核, 不属 builder 修订面）");

    /* tooth 2: K2 动作码语义位零偏移（story_page.h 闭集在世——本包止血不改 K2 语义位）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsr_file_contains(RSR_STORY_H, "KSPV2_ACT_REC_SESSION") &&
        rsr_file_contains(RSR_STORY_H, "KSPV2_ACT_STOP_RECORD") &&
        rsr_file_contains(RSR_STORY_H, "KSPV2_ACT_DISCARD_RECORD"),
        "RSR-C1 RED: story_page.h 动作码语义位缺失（REC_SESSION/STOP_RECORD/"
        "DISCARD_RECORD）——K2 冻结闭集被改（本包只止血不改语义位）");

    /* tooth 3: K3 冻结验收件在世（本包对 K3 为增量修复, 须零静默改——修订走冻结件
     * REVISION LOG 纪律）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsr_file_contains(RSR_K3_C, "RSL-A1") &&
        rsr_file_contains(RSR_K3_C, "kidpal_story_list_gate"),
        "RSR-C1 RED: K3 冻结验收件 record_session_live.acceptance.c 缺失或签名被改——"
        "前序包冻结面被破（修复须零语义冲突破坏 K3 齿）");

    /* tooth 4: 注册序守卫（builder FIRST ACTION; smoke 末位 = 最先执行——本文件须置于
     * host_infra_smoke.acceptance.c 之前）。 */
    cm = rsr_read(RSR_CMAKE, NULL);
    TEST_ASSERT_TRUE_MESSAGE(cm != NULL,
        "RSR-C1 RED(infra): host_app CMakeLists.txt 读取失败");
    p_self = strstr(cm, "../../record_session_repair.acceptance.c");
    p_smoke = strstr(cm, "../../host_infra_smoke.acceptance.c");
    TEST_ASSERT_TRUE_MESSAGE(
        p_self != NULL && p_smoke != NULL && p_self < p_smoke,
        "RSR-C1 RED: record_session_repair.acceptance.c 未注册进 HOST_ACCEPTANCE_SRCS"
        "（或未置于 host_infra_smoke.acceptance.c 之前）——builder FIRST ACTION 未执行"
        "（注册序 = smoke 末位最先执行的 runner 纪律）");
    free(cm);
}

/* ========================================================================== *
 * TEST 7 — RSR-A6: D2 录音态覆盖层不驻留 [R4]（2026-10-01 真机轮 REVISION 补严齿;
 * D2 根因 1「弹层 ≠ 隐屏」）。seam [R4]（LCD 声波 chrome 前台判据）。
 * RED today（@5950bb7）: [R4] 哨兵 -2（判据未实现）; 且现 kidpal_render_node.cc 内联
 *   逻辑 `layer_top == KIDPAL_LAYER_VOICE` 无录音维度 —— 真机现场「声波覆盖层一直
 *   盖着」: 甲形态只弹逻辑层栈不改状态机, 录音期间状态机仍 listening, LCD 照画 listening
 *   态。判别位 = (VOICE, RECORDING): 旧内联逻辑答 1（驻留 = 缺陷）/ 本判据答 0（退隐）。
 * ========================================================================== */
/* coverage: K3R-A2 -- D2 录音态覆盖层不驻留（[R4] LCD 声波 chrome 前台判据: 录音页态
 *           令 chrome 退隐, 与层值无关 = 「弹层 ≠ 隐屏」机判翻转 + render_node 消费点） */
TEST_CASE("record_session_repair: D2 录音态覆盖层不驻留 [R4] (RSR-A6)",
          "[record_session_repair][RSR-A6][behavior][structural]")
{
    RSR_REQUIRE_ROOT();

    /* tooth 1 (行为 · D2 根因 1 命门/判别位): VOICE 层前台 + 录音页态 -> LCD 声波 chrome
     * 必退隐（0）。旧内联逻辑（layer_top == VOICE, 无录音维度）此支答 1 = chrome 驻留
     * （真机现场）; 本判据答 0 = chrome 退隐。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_voice_chrome_front(RSR_LAYER_VOICE, RSR_PAGE_RECORDING) == 0,
        "RSR-A6 RED: [R4] 录音页态 + VOICE 层前台未令 LCD 声波 chrome 退隐（期望 0, 得 -2 = "
        "frozen seam [R4] 未实现）——D2 真机现场「声波覆盖层一直盖着」: 甲形态只弹逻辑层"
        "栈不改状态机, 录音期间状态机仍 listening, chrome 判据须含录音页态维度方能退隐");

    /* tooth 2 (行为 · 形态无关): VOICE 已让位/无覆盖层 + 录音页态 -> 仍退隐（0）——
     * 甲（层已弹）与乙（层未弹）两形态同一结论 = 录音页态恒令 chrome 退隐。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_voice_chrome_front(RSR_LAYER_NONE, RSR_PAGE_RECORDING) == 0,
        "RSR-A6 RED: [R4] 录音页态 + 无覆盖层前台未令 chrome 退隐（期望 0）——录音页态"
        "须与层值无关地令 chrome 退隐（甲/乙两形态一致）");

    /* tooth 3 (负锚 · 零偏移): 非录音页态 + VOICE 前台 -> chrome 前台（1, 既有聆听轮语义）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_voice_chrome_front(RSR_LAYER_VOICE, RSR_PAGE_LIST) == 1,
        "RSR-A6 RED(负锚): 普通聆听轮（VOICE 前台 + 列表页态）被误令 chrome 退隐（期望 1）——"
        "既有 RP-A5 r3「VOICE 前台则 chrome 前台」语义零回归红线");

    /* tooth 4 (负锚 · 零偏移): 内容页/其它覆盖层前台 -> chrome 退隐（0, 既有语义）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_voice_chrome_front(RSR_LAYER_NONE, RSR_PAGE_LIST) == 0 &&
        kidpal_story_voice_chrome_front(RSR_LAYER_REMINDER, RSR_PAGE_LIST) == 0 &&
        kidpal_story_voice_chrome_front(RSR_LAYER_STORY_DELETE, RSR_PAGE_READER) == 0,
        "RSR-A6 RED(负锚): 内容页/更高覆盖层前台被误令 chrome 前台（期望全 0, 得 -2）——"
        "既有「内容页前台 chrome 退隐」语义零回归红线");

    /* tooth 5 (结构 · 源锚代理): 生产消费点 —— 渲染节点前台桥（rn_voice_chrome_front_
     * locked）消费本判据（不是纯逻辑函数无驱动方）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsr_file_contains(RSR_RN_CC, "kidpal_story_voice_chrome_front("),
        "RSR-A6 RED: kidpal_render_node.cc 未消费 kidpal_story_voice_chrome_front——"
        "chrome 前台判据无驱动方（实现但未接线的死码 = 真机 chrome 仍按旧层值驻留）");
}

/* ========================================================================== *
 * TEST 8 — RSR-A7: D2 voice_end 前台回落渲染页态同步 [R5]（2026-10-01 真机轮
 * REVISION 补严齿; D2 根因 2「页态同步先于渲染」）。seam [R5]（页态解析判据）。
 * RED today（@5950bb7）: [R5] 哨兵 -2（判据未实现）; 且现 kidpal_render_node_voice_end
 *   裸读 s_story_view.page（陈旧 LIST, 未经 cache_story 同步）—— 真机现场
 *   「voice_end: story front list_flush=1 page_state=0 -> render page STORY (P3)」
 *   渲出列表页而非录音页。判别位 = (node=LIST, session=RECORDING, front=1): 旧内联逻辑
 *   答 LIST（渲列表页 = 缺陷）/ 本判据答 RECORDING（渲录音页）。
 * ========================================================================== */
/* coverage: K3R-A2 -- D2 voice_end 渲染页态同步（[R5] 前台回落页态解析: 以权威会话
 *           页态取代渲染节点陈旧缓存 = 「页态同步先于渲染」机判翻转 + voice_end 消费点） */
TEST_CASE("record_session_repair: D2 voice_end 渲染页态同步 [R5] (RSR-A7)",
          "[record_session_repair][RSR-A7][behavior][structural]")
{
    RSR_REQUIRE_ROOT();

    /* tooth 1 (行为 · D2 根因 2 命门/判别位): 节点缓存陈旧 LIST + 权威会话 RECORDING +
     * 本地页 = P3 -> 解析出 RECORDING（页态同步先于渲染）。旧内联逻辑（直接用节点缓存）
     * 此支答 LIST（渲陈旧列表页 = 真机现场）; 本判据答 RECORDING（渲录音页）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_render_page_state(RSR_PAGE_LIST, RSR_PAGE_RECORDING, 1) == RSR_PAGE_RECORDING,
        "RSR-A7 RED: [R5] 节点缓存陈旧（LIST）+ 权威会话录音态未解析为 RECORDING（期望 2, "
        "得 -2 = frozen seam [R5] 未实现）——D2 真机现场「voice_end: story front list_flush=1 "
        "page_state=0 -> render page STORY (P3)」渲出列表页而非录音页（页态同步须先于渲染）");

    /* tooth 2 (行为 · 幂等/零偏移): 缓存与权威一致 -> 取该值（录音态取录音态, 列表态取列表态）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_render_page_state(RSR_PAGE_RECORDING, RSR_PAGE_RECORDING, 1) == RSR_PAGE_RECORDING &&
        kidpal_story_render_page_state(RSR_PAGE_LIST, RSR_PAGE_LIST, 1) == RSR_PAGE_LIST,
        "RSR-A7 RED: [R5] 缓存与权威一致时解析值漂移（期望各自原值）——零偏移红线");

    /* tooth 3 (负锚 · 零回归): 非 P3 前台（on_story_front=0）-> 节点缓存不被权威值覆盖。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_story_render_page_state(RSR_PAGE_RECORDING, RSR_PAGE_LIST, 0) == RSR_PAGE_RECORDING,
        "RSR-A7 RED(负锚): 非 P3 前台时节点缓存被权威值覆盖（期望 0 支返回节点缓存 RECORDING, "
        "得 -2）——非 P3 前台不得被 P3 会话页态篡改（零回归红线）");

    /* tooth 4 (结构 · 源锚代理): 生产消费点 —— voice_end 渲 P3 前消费本判据（取代裸读
     * s_story_view.page）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        rsr_file_contains(RSR_RN_CC, "kidpal_story_render_page_state("),
        "RSR-A7 RED: kidpal_render_node_voice_end 未消费 kidpal_story_render_page_state——"
        "页态解析判据无驱动方（voice_end 仍裸读陈旧 s_story_view.page = 真机渲列表页）");
}

/* --------------------------------------------------------------------------- *
 * RED baseline 判读口径（acceptance-author 冻结期实跑, 2026-10-01; 现状树 @5490a5c;
 * 2026-10-01 真机轮「补严齿」修订后重跑, 现状树 @5950bb7）:
 *   设计态红（= 声明的缺失行为, 非环境/依赖/fixture 缺陷）:
 *     RSR-A1  [R1] 哨兵 -2（归属判据未实现）+ 板级 keya_story_page_wants 区零消费。
 *     RSR-A2  [R1] 哨兵 -2（形态无关判据未实现）+ 录音路径零让位接线。
 *     RSR-A3  [R2] 哨兵 -2（收尾复位判据未实现）+ Idle 分支零消费; tooth 6 现状绿
 *             （must-stay-green）。
 *     RSR-A4  函数区零 lv_obj_is_valid（守卫未落）; tooth 2 现状绿（must-stay-green）。
 *     RSR-A5  [R3] 哨兵 -2（defer 判据未实现）+ 前台回落路径零消费。
 *     RSR-A6  [R4] 哨兵 -2（chrome 前台判据未实现）+ rn_voice_chrome_front_locked 零消费
 *             （D2 根因 1「弹层 ≠ 隐屏」——真机「声波覆盖层一直盖着」机判翻转点）。
 *     RSR-A7  [R5] 哨兵 -2（页态解析判据未实现）+ voice_end 零消费
 *             （D2 根因 2「页态同步先于渲染」——真机「渲列表页而非录音页」机判翻转点）。
 *     RSR-C1  tooth 4 注册序红（未进 HOST_ACCEPTANCE_SRCS——builder FIRST ACTION）;
 *             tooth 1-3 canary/K3 面守卫现状绿（must-stay-green）。
 *   通道 A（独立红跑, red-baseline/unity.h stub + runner.c, 非冻结门 runner; exit code =
 *   失败数, 可靠）:
 *     wsl -d Ubuntu-24.04 -- bash firmware/main/test/acceptance/red-baseline/
 *       rsr_red_wrapper.sh
 *     @5950bb7 结果: COMPILE_EXIT=0（8 齿实编零错, §4 C 类实编）; 8 Tests 6 Failures
 *     （RUN_EXIT=6）: RSR-A1/A2/A3/A5/A6/A7 红（frozen seam 哨兵）; RSR-A4/C1 绿。
 *   通道 B（冻结门 runner = host_app linux ELF, 链真实实现 story_page.c 强符号;
 *   **exit code 不可靠**——linux port 恒 0, 判读以 "N Tests M Failures" 行为准）:
 *     MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *       espidf_wsl.sh build && cd firmware/main/test/host_app && ./build/host_infra_test.elf
 *     @5950bb7 结果: **147 Tests 2 Failures**（基线 145 全绿 + 新增 2 齿 RSR-A6/A7 红）——
 *     证 [R4]/[R5] 未被实现（story_page.c 无强符号 -> 弱哨兵 -2）, 且既有 145 齿零回归。
 *   GREEN 后门面 = 通道 B 全量（既有 145 齿零偏移 + 本文件 8 TEST_CASE 转绿）。
 * --------------------------------------------------------------------------- *
 * REVISION LOG（tighten-only 修订窗口; 纯 additive / 零断言语义变化才可追加）:
 *   R1（2026-10-01, 真机轮「补严齿」, human 已批）— **纯 additive, 零既有断言语义变化**:
 *     背景: 2026-10-01 真机（烧本分支构建）四窗复验——D4a/D4b 已修（story_key stop_rec
 *     archive_pend=1、零 panic）, 但 D2 未修（user 确认「声波覆盖层一直盖着」）。旧齿
 *     RSR-A2 全绿却没抓到: 它只测「[R1] 两态同结论 + 录音路径让位接线**存在**」, 测不到
 *     屏上覆盖层是否真的没了（接线存在性软齿, 真机代价）。
 *     新增（additive）:
 *       - seam [R4] int kidpal_story_voice_chrome_front(int layer_top, int page_state)
 *         + TEST 7 RSR-A6（行为 + 结构, D2 根因 1: 录音态覆盖层不驻留）。
 *       - seam [R5] int kidpal_story_render_page_state(int node_cached_page,
 *         int session_page, int on_story_front) + TEST 8 RSR-A7（行为 + 结构, D2 根因 2:
 *         voice_end 渲染页态同步 = 以权威会话页态取代陈旧节点缓存）。
 *       - header seam contract [R4]/[R5] 节 + coverage map 两行 + 强度申报两行 +
 *         盲区申报⑤（D2 host 边界显式化）+ RED baseline 双通道口径。
 *     未动: RSR-A1..A5 / RSR-C1 全部既有断言逐字不变（tighten-only 只增不改）。
 *     语义变化（如实申报）: 覆盖集扩大——K3R-A2 现由 RSR-A2 + RSR-A6 + RSR-A7 三齿共同
 *     承载, 后两者为 D2 的**直接可观察决策**（判别位对旧内联逻辑翻转）, 强于旧 RSR-A2
 *     的形态无关代理齿。seam [R1]-[R3] 语义零偏移; 新增 [R4]/[R5] 为纯追加。
 *     红基线: 通道 A 8 Tests 6 Failures（RUN_EXIT=6）; 通道 B 147 Tests 2 Failures
 *     （新增 2 齿红, 既有 145 齿零回归）。
 *   （前冻结时点无修订。）
 * --------------------------------------------------------------------------- */
