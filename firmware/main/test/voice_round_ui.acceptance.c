/*
 * Frozen acceptance tests — TaskPackage 260915-m1-k3-voice-round-ui (subsystem:
 * firmware; M1-K3; 语音会话演出补齐: 思考页接线 + 聆听回显区 + 问答框长文滚动 +
 * 打断手感衔接 + 四态版式骨架闭集零偏移)。
 *
 * Story: S-F4.1 · 语音问答（AC1–AC2）+ S-F1.3 · 语音问课表（AC1/AC3, 问答框口径）
 * + docs/requirements.md §2.2 状态表（聆听/思考/播报三行, v1.22/v1.24 口径）。
 * 现状差距（plan.md 背景摘录）: 聆听/播报两态渲染在、演出细节缺（回显区/长文滚动）;
 * 思考态渲染路径零接线（lcd_display.cc 活侧只映射 KVST_SCENE_LISTEN/TALK,
 * KVST_SCENE_THINK 零调用——场景码与姿态资产已备但活侧零消费, §2.2 ⚠️ MVP 未接线,
 * Phase 1 修复项）; 播报中打断手感未按 §2.3 矩阵收口演出侧衔接。
 *
 * Nature: HOST-SIDE BEHAVIORAL (纯逻辑决策表, weak 哨兵) + STRUCTURAL (tree-scan)
 * frozen acceptance, key_layer_dispatch.acceptance.c（K1a）+ keybar_three_slot.
 * acceptance.c（K2）范式。三态渲染面住 lcd_display.cc（LVGL 设备侧, host runner
 * 不可运行）——结构齿树扫工作树钉接线事实; 机制值（场景映射决策表 + 话语回显逐句
 * 入列/滚出模型）钉在纯逻辑单源（home = firmware/main/display/voice_round.{h,c},
 * kidpal_voice_states.c / keybar.c 同款「纯 C、无 chip/LVGL 依赖、host runner 与
 * 设备两侧同源可编」纪律）, host runner 行为级可验（weak 哨兵, 强定义落地后覆盖）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements，本文件只读扫工作树 + 弱哨兵
 * 行为级验证; 「home =」为冻结落点）:
 *
 *   [1] 语音轮场景映射 + 话语回显纯逻辑单源（home = firmware/main/display/
 *       voice_round.{h,c}, 纯 C、无 LVGL/chip 依赖, 强符号落 voice_round.c）:
 *
 *       #define KVR_UNIMPLEMENTED (-2)   -- weak 哨兵值, 永不是合法返回值
 *
 *       -- 会话语境（判定入参闭集, 显式数值冻结——与本验收文件内哨兵声明逐值一致）:
 *       KVR_CTX_LISTEN_AUTO = 0（聆听中, auto/VAD 切句, Phase 1 问答主路径）/
 *       KVR_CTX_LISTEN_MANUAL = 1（聆听中, manual 按键段——记录语境整段累积）/
 *       KVR_CTX_THINKING = 2（思考中）/ KVR_CTX_SPEAKING = 3（播报中）。
 *       -- 会话事件（判定入参闭集）: KVR_EVT_STT_FINAL = 0（网关 VAD 切句完成,
 *       WS stt 文本下发）/ KVR_EVT_TTS_START = 1（回复到达开始播报, tts start）/
 *       KVR_EVT_SPEAK_DONE = 2（播报完成, tts stop）/ KVR_EVT_ABORT_SPEECH = 3
 *       （打断停播, KEY 单击经分层裁决）。
 *
 *       int kidpal_voice_round_scene(kvr_ctx_t ctx, kvr_evt_t ev);
 *         -- 会话语境 × 事件 -> 目标 KVST 场景码（KVST_SCENE_*, kidpal_voice_states
 *            闭集单源; 返回 <0 = 不切场景, -1 兼作防御返回值）。冻结映射
 *            （弹出时机权威 = requirements §2.2 思考中行「进入时机 = 孩子说完这一句
 *            （服务端 VAD 切句完成 / 音频上行结束）即切『思考中』并弹出——状态切换
 *            与页面弹出同一时刻, 不分离」）:
 *         LISTEN_AUTO:   STT_FINAL -> KVST_SCENE_THINK（说完这句即切思考, 弹出同刻,
 *                          4s 安抚计时起点 = 本态入口, VQ-A6 锚）;
 *                        TTS_START -> KVST_SCENE_TALK;
 *                        SPEAK_DONE / ABORT_SPEECH -> -1（聆听态无播报可断/无播可完）。
 *         LISTEN_MANUAL: STT_FINAL -> -1（记录语境整段累积, 切句只用于话语回显,
 *                          不逐句进思考——requirements §2.1 记录会话聆听语义分流）;
 *                        TTS_START -> KVST_SCENE_TALK; 其余 -1。
 *         THINKING:      TTS_START -> KVST_SCENE_TALK（回复到达开始播报即离开本态）;
 *                        STT_FINAL -> -1（已在思考不重入）; 其余 -1。
 *         SPEAKING:      ABORT_SPEECH -> KVST_SCENE_LISTEN（打断停播回聆听——VQ-A4
 *                          演出侧状态机衔接, 键位语义消费 [3] 不自裁）;
 *                        SPEAK_DONE -> KVST_SCENE_LISTEN（播完回聆听, auto 连续对话）;
 *                        其余 -1。
 *         防御: ctx/ev 越界 -> -1（从不为 UB）。
 *
 *       -- 话语回显区机制单源（VQ-A2; 逐句入列 + 行数容量滚出 = 「回显区滚动, 超屏高
 *          滚出」的行数模型; 屏高可容行数的视觉拟合归设计稿/真机轮, 机制值冻结 6）:
 *       #define KVR_ECHO_MAX_LINES 6
 *
 *       int kidpal_voice_echo_reset(void);                 -- 会话开始清零; 返回 0
 *       int kidpal_voice_echo_push(const char *text);      -- 逐句入列（拷贝存储）,
 *                                                             满容量挤最旧; 返回当前
 *                                                             句数; NULL/空串 -> -1
 *       int kidpal_voice_echo_count(void);                 -- 当前句数
 *       int kidpal_voice_echo_line(int idx, const char **out);
 *                                                          -- idx 0 = 最旧; 越界/
 *                                                             out NULL -> -1
 *       int kidpal_voice_echo_capacity(void);              -- 恒 KVR_ECHO_MAX_LINES
 *
 *   [2] 活侧渲染接线（VQ-A1/A7; home = firmware/main/display/lcd_display.cc 设备侧
 *       + firmware/main/application.cc 设备侧触发）:
 *       - 活侧渲染路径首接思考态: lcd_display.cc 出现 kidpal_voice_apply_look
 *         (KVST_SCENE_THINK) 消费（现状零调用销账; 调用点路由不锁——SetStatus 分支
 *         或新显示入口均可, 但必须经既有 apply_look 单点, 不另立第二渲染源）;
 *       - 思考触发 = stt 分支同刻消费 [1]: application.cc OnIncomingJson 的 "stt"
 *         分支区域（strcmp(type->valuestring, "stt") 起）内消费 kidpal_voice_round_
 *         scene()——触发源 = 服务端 VAD 切句完成（stt 下发）, 非 tts start、非定时器
 *         （取舍 A）; 回显（SetChatMessage("user") 既有通道）与思考切换同一回调 =
 *         「状态切换与页面弹出同一时刻」的机制事实。
 *
 *   [3] 聆听回显区 + 问答框（VQ-A2/A3; home = lcd_display.cc 活 SetupUI 分支）:
 *       冻结符号（builder 按名落地, 文件级 static）:
 *         g_kidpal_voice_echo_box    -- 聆听话语回显区容器（「我在听！」标识下方,
 *                                       可滚动 LV_OBJ_FLAG_SCROLLABLE）
 *         g_kidpal_voice_answer_box  -- 播报问答框容器（WF-B-03 右内容区, 可滚动
 *                                       LV_OBJ_FLAG_SCROLLABLE, 答案全文上屏滚动
 *                                       看完, 非一行字幕——回引 NFR-READ-1 字号下限）
 *       活 SetChatMessage 路由（最后一个 LcdDisplay::SetChatMessage 定义 = 活分支;
 *       另一 WeChat 分支不可达, 不作接线面）: role "user" -> 消费 kidpal_voice_echo_
 *       push() 入 [1] 回显缓冲并渲染到 g_kidpal_voice_echo_box; role "assistant" ->
 *       答案全文渲到 g_kidpal_voice_answer_box（问答框口径, S-F1.3 AC1「与百科问答
 *       同一问答框」; 课表数据侧零触碰 VQ-E2）。
 *
 *   [4] 打断手感键位衔接（VQ-A4; 消费 K1a 分层裁决, 零路由改动——crossPackage
 *       Behaviors: 语义 owner = key-layer-dispatch, 本包只对齐消费）:
 *       key_route_layer 覆盖层列语义 = KD-A1/KD-A3 冻结值（KEY 单击 -> SESSION_
 *       TOGGLE 停播并聆听主轴 / KEY 长按 -> OVERLAY_CLOSE / BOOT 单击 -> OVERLAY_
 *       CLOSE 结束会话回原页, 永不 PAGE_NEXT）; 板级 dispatch 既有消费链零回归
 *       （SESSION_TOGGLE -> ToggleChatState、OVERLAY_CLOSE 会话级联 ToggleChatState
 *       两处在世）; application.cc 播报中打断路径 AbortSpeaking(kAbortReasonNone)
 *       在世（HandleToggleChatEvent speaking 分支——停播后经 tts stop 回聆听,
 *       演出侧衔接由 [1] SPEAKING+ABORT->LISTEN 行承载）。
 *
 *   [5] 四态版式骨架 + 闭集零偏移（VQ-A5/E4; 骨架 owner = 本包）:
 *       左小伴（各态 pose, apply_look 消费 kidpal_voice_state_look 单源）· 右内容区
 *       （[3] 回显区/问答框）同骨架; 键帽文案走 kidpal_voice_states 冻结闭集
 *       （五成员逐字零偏移, K2 已入 runner 强符号, 本包行为级复核）; 提醒弹窗同骨架
 *       由 260915-m1-k4-reminder-overlay-unify 复用（跨包登记, 零齿）。
 *
 *   [6] Builder FIRST ACTION: 把本验收文件与被测实现接进
 *       firmware/main/test/host_app/main/CMakeLists.txt 的 HOST_ACCEPTANCE_SRCS:
 *         "../../voice_round_ui.acceptance.c"
 *         "../../../display/voice_round.c"     （[1] 强符号源）
 *       置于 "../../host_infra_smoke.acceptance.c" 之前（smoke 保持列表末位 = 最先
 *       执行, boundary 齿注册序守卫; 本文件 test 5 tooth 1 校验此序）。
 *   [7] mutation 自证（mutationRequired=true）：变异命名**不得保留被测符号原字面为
 *       子串**（KR-A3 _TOGGLE_X / k2 [6] 教训——strstr 子串仍命中则变异不红）;
 *       mutationEvidence.perAcceptance 逐条回填, acceptanceId 与 taskpackage
 *       acceptance[] 正文逐字节一致。
 *
 *   [8] 真机轮回归增补（2026-09-17 device-round VQ-A7 r1 增补齿, 单独成节 = TEST 6;
 *       既有齿 [1]-[7] 零改动）——human 真机确认轮第 1 轮暴露三处 host 断言面缺口
 *       （k2 期教训④重演: 视觉/接线类断言缺口 host 齿兜不住）, 按红基线模型增补
 *       （增补齿 = 设计态红, fixer 实现后转绿, verifier 终验）:
 *       a. TTS_START 喂入时机（VQ-A1 深化）: 活侧现状经 tts start -> SetDeviceState
 *          (kDeviceStateSpeaking) -> SetStatus(SPEAKING) -> apply_look(KVST_SCENE_
 *          TALK) 间接触发场景切换; 方向 B 网关在 stt 之后立即发 tts start（先于 LLM
 *          出结果）, THINKING 存活 10ms 被顶掉。钉: KVR_EVT_TTS_START 喂入点 = tts
 *          sentence_start 处理分支（实际起播时刻）, 且必须经冻结映射单源
 *          kidpal_voice_round_scene 消费（映射表零改动红线: 场景切换不得绕开决策表
 *          ——喂入时机移动, 表值行为级锁已在 TEST 1）; tts start 帧处理路径零场景
 *          切换触发（禁 SetDeviceState(kDeviceStateSpeaking) / kidpal_voice_round_
 *          scene( / kidpal_voice_apply_look( 三载体）。等价「音频真正起播」锚若被
 *          实现采用, 须返 planner 重新钉锚（本齿按 device-round 首选形态冻结）。
 *          VQ-A6 不新增齿: 4s 安抚计时起点语义 = THINKING 入口, 修复后才真实存在;
 *          既有锁面 = TEST 1（入口唯一锚）+ TEST 4 tooth 6（session_state_machine
 *          冻结边）, 增补不触碰。
 *       b. 回显/问答 label 全量中文字体（VQ-A2/A3 深化）: g_kidpal_voice_echo_label
 *          / g_kidpal_voice_answer_label 显式 lv_obj_set_style_text_font 配全量
 *          puhui 家族字体（对照 keybar font_puhui_14_1 先例同形态, 具体型号由实现
 *          按 NFR-READ-1 可读性下限选）; 禁 basic 稀疏子集（缺中文字形 = 整段空白
 *          零上屏回归, 键帽 KB-A5 同族根因）。
 *       c. 可见面挂载 + 骨架反形态（VQ-A5 深化）: echo/answer 两容器须挂载于语音轮
 *          可见子树/语音层而非裸 screen 顶层（聊天 chrome 子树 vs 语音轮专用内容面
 *          = 实现取舍, 父级名不锁; 合格形态三选一 = 创建点已离裸 screen 根 / 显式
 *          lv_obj_set_parent / 入 KidPalReparentChatToLayer 迁移清单）; 且告别
 *          TOP_MID 居中叠装（现状 TOP_MID 直接挂裸 screen 顶层被页面内容盖住 +
 *          姿态 96×96 居中叠首页猫; WF-B「左小伴 · 右内容区」骨架 = requirements
 *          v1.24 human 冻结, UIUX-DESIGN-BRIEF §5 语音轮四态行同锚——正几何像素级
 *          分配归真机轮 VQ-A7 观感轮, 本齿钉反形态销账; 若实现采用右侧专用面板内
 *          居中的合法形态, 须返 planner 重新钉锚）。
 *
 *   [9] 真机轮回归增补第 2 轮（2026-09-17 device-round VQ-A7 r2 增补齿, 单独成节 =
 *       TEST 7; 既有齿 [1]-[8] 与 TEST 1-6 零改动）——human 真机确认轮第 2 轮（板上
 *       54aa903, 功能四项全通）暴露一类渲染面偏差「各态内容区标识件缺失 + 气泡写点
 *       挂旧聊天 chrome」（零状态机/逻辑改动, 全在演出面）:
 *       a. 三态 bubble 闭集标头值（kidpal_voice_states.c 单源）: LISTEN =「我在听！」
 *          （既有零偏移复核）; THINK =「思考中」（human 钉 = §2.2 态名口径, 消解
 *          「问号/气泡文案归 design review」留白）; TALK =「播报」（WF-B-03 线框
 *          标头字面）。问号图标资产 art_assets.c 闭集无 question 资产（planner 核实）
 *          = fail-closed 已上报归后续资产批, 文案标头先行——本齿不造图标齿。
 *       b. 标头写点统一: kidpal_voice_apply_look 的 look.bubble 写点不再写
 *          g_kidpal_chat_label（bottom_bar_ 内 chat_message_label_ 旧挂点）, 改写
 *          g_kidpal_voice_page 右内容区标头对象（新增, 名不锁——标头符号名由实现定,
 *          齿钉「voice_page 直接子件 + apply_look 旧挂点写点清零 + look.bubble 单源
 *          仍在」形态; 等价锚若被采用须返 planner 重新钉锚）。
 *       c. 回显行挂标头下方: WF-B-01/VQ-A2 冻结原文「回显在『我在听！』标识下方」
 *          ——标头对象与 echo_box 同父（g_kidpal_voice_page）且创建序在 echo_box
 *          之前（LVGL 同父子件叠序 = 创建序, 标头在上、回显行在下）。
 *       与 TEST 4 的关系: TEST 4 tooth 4 对 THINK/TALK 只断 pose/keycap、不断
 *       bubble（现值 NULL 无断言面）——三态 bubble 值的新权威 = TEST 7 tooth 1,
 *       TEST 4 零改动零冲突（新值落地后 TEST 4 仍绿, 无须裁量）。
 *
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: VQ-A1 思考页接线机制齿（渲染映射激活 + 弹出时机同刻）-> test 1（行为半:
 *             LISTEN_AUTO+STT_FINAL->KVST_SCENE_THINK 决策行）+ test 3（结构半: 活侧
 *             kidpal_voice_apply_look(KVST_SCENE_THINK) 首接 + stt 分支消费 [1] 单源）
 *   coverage: VQ-A2 聆听回显区 -> test 2（行为半: 逐句入列/行序/容量滚出/防御）+
 *             test 3（结构半: 回显区容器 + push 消费 + user 路由）
 *   coverage: VQ-A3 问答框长文滚动 -> test 3（结构齿: 问答框容器可滚动 + assistant
 *             全文路由——非一行字幕; 屏高滚动观感归真机轮 VQ-A7）
 *   coverage: VQ-A4 打断手感 -> test 1（行为半: SPEAKING+ABORT->LISTEN 衔接行）+
 *             test 4（键位衔接: key_route_layer 覆盖层列 KD-A1/A3 消费对齐 + 板级
 *             ToggleChatState 消费链 + AbortSpeaking 打断路径在世）
 *   coverage: VQ-A5 四态版式骨架统一 -> test 4（闭集五成员逐字零偏移 + 防御）+
 *             test 3（骨架对象: 回显区/问答框布线 + kidpal_voice_state_look 单源
 *             接线在世）; 版式几何观感归设计稿/真机轮
 *   coverage: VQ-A6 超时安抚钉点 -> test 1（计时起点锚: THINK 入口 = STT_FINAL 同刻,
 *             非回复驱动）+ test 4（session_state_machine 冻结边
 *             THINKING_TIMEOUT_4S->SPEAKING+COMFORT 在世, NFR-PERF-1 模型承载;
 *             安抚句到达即播已由 260829-voice-roundtrip VRT-A5 闭环）
 *   coverage: VQ-A7 真机确认轮（human 持板, kind=structural 对象条目）-> test 3
 *             （真机前置接缝断言: 思考页活侧可弹 + 回显/滚动/打断三手感的前置接线
 *             全部在世）; 真机轮证据落 delivery/changes/dev/260915-m1-k3-voice-round-ui/
 *             ——见文件尾「VQ-A7 真机证据要求」段
 *   coverage: VQ-A8 host runner 终跑 -> test 5（注册接线结构守卫）+ 本文件尾
 *             「VQ-A8 终跑判读」段（suite 级判读: 相对有效基线零新增红 + 本包新齿全绿）
 *   coverage: VQ-E1 范围纪律 -> test 5 tooth 2（60 秒 timer 零活侧 producer——
 *             deferred 非 dropped）; 语音喊停 AEC 零触碰无独立齿（无接缝可钉, 范围
 *             声明见文件尾注记）
 *   coverage: VQ-E2 范围纪律 -> 无齿条目（课表答复数据侧归 M3, 本包问答框只锁演出
 *             口径——[3] assistant 路由齿锁的是渲染面, 不引数据面; 范围声明见尾注记）
 *   coverage: VQ-E3 需求面零修订 -> test 5 tooth 3（requirements/契约 canary）
 *   coverage: VQ-E4 四态骨架 owner 声明 -> test 3/test 4（骨架对象 + 闭集单源由本包
 *             建立并钉死）; 提醒包复用行为见 [5]（跨包登记, 零齿）
 *   coverage: VQ-A2 真机 r2 增补 -- 聆听态「我在听！」标头 + 回显行挂标头下方 ->
 *             test 7 tooth 1（LISTEN bubble 零偏移复核）/tooth 3（同父创建序在先）
 *   coverage: VQ-A5 真机 r2 增补 -- 三态 bubble 闭集标头值（THINK「思考中」/TALK
 *             「播报」）+ 标头写点统一挂 voice_page 右内容区、旧聊天 chrome 挂点
 *             销账 -> test 7 tooth 1/tooth 2
 *
 * Run protocol (HOST_TEST_CONVENTIONS.md [C4.4]):
 *   0) builder wiring 见 [6]。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build   （git-bash 下 MSYS_NO_PATHCONV=1 必加, 否则 /mnt/c 路径
 *      被改写; idf.py 无 -T 选项, host_app 是独立工程直接 build）
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/
 *      host_infra_test.elf   （仓根 cwd 运行; 结构齿以 kvr_locate_root 自手持 cwd
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

/* 键路由共享底座头（覆盖层列消费对齐 [4]; K1a/K2 先例同款 include, 强符号已入 runner）。 */
#include "../key_gesture_parser.h"
/* 语音轮五态闭集契约头（[5] 冻结形状: 场景码/look 结构/keycap 值域; 强符号已入 runner）。 */
#include "../display/kidpal_voice_states.h"

#define KVR_MAX_PATH 1024
#define KVR_MAX_READ (8u * 1024u * 1024u)

/* 被扫文件（结构齿锚, 相对仓根）。 */
#define KVR_LCD_CC       "firmware/main/display/lcd_display.cc"
#define KVR_APP_CC       "firmware/main/application.cc"
#define KVR_SES_CC       "firmware/main/session_state_machine.cc"
#define KVR_VOICE_ROUND_H "firmware/main/display/voice_round.h"
#define KVR_VOICE_ROUND_C "firmware/main/display/voice_round.c"
#define KVR_BOARD_CC     "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"
#define KVR_CMAKELISTS   "firmware/main/test/host_app/main/CMakeLists.txt"
#define KVR_REQUIREMENTS "docs/requirements.md"
#define KVR_CONTRACT     "docs/contracts/interface-contract.md"

/* -------------------------------------------------------------------------- *
 * FROZEN OFFLINE CONTRACT — weak sentinels（强定义落地后覆盖）。
 * kidpal_voice_round_scene / kidpal_voice_echo_* 强定义 home = display/voice_round.c
 * （[1], builder 落地 + 接线后覆盖）; kidpal_voice_state_look / key_route_layer 强
 * 符号已在 runner（k1a/k2 接线, 本文件只消费不重复声明）。
 * 落地前哨兵返回 KVR_UNIMPLEMENTED：行为齿先断「非哨兵」，红基线读作
 * 「frozen contract not implemented」——真实缺失行为红，绝非链接错误。
 * 入参闭集类型在本文件内自持声明（冻结期 voice_round.h 尚未落地, C 类冻结验收实编
 * 纪律）; 落地后与 voice_round.h 的同名类型逐值一致（seam [1] 显式数值冻结）。
 * ------------------------------------------------------------------------ */
#define KVR_UNIMPLEMENTED (-2)

typedef enum {
    KVR_CTX_LISTEN_AUTO   = 0,
    KVR_CTX_LISTEN_MANUAL = 1,
    KVR_CTX_THINKING      = 2,
    KVR_CTX_SPEAKING      = 3
} kvr_ctx_t;

typedef enum {
    KVR_EVT_STT_FINAL    = 0,
    KVR_EVT_TTS_START    = 1,
    KVR_EVT_SPEAK_DONE   = 2,
    KVR_EVT_ABORT_SPEECH = 3
} kvr_evt_t;

/* 覆盖层关层动作冻结值（K1a KD-A1: GLOBAL_ACT_OVERLAY_CLOSE = 枚举追加位 4; 以字面量
 * 钉值 + test 4 同时断言 != PAGE_NEXT 判别式, 同 key_layer_dispatch.acceptance.c
 * KLD_OVERLAY_CLOSE_VALUE 钉法）。 */
#define KLD_OVERLAY_CLOSE_FROZEN 4

#define KVR_ECHO_MAX_LINES 6

__attribute__((weak)) int kidpal_voice_round_scene(kvr_ctx_t ctx, kvr_evt_t ev)
{
    (void)ctx; (void)ev;
    return KVR_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_voice_echo_reset(void)
{
    return KVR_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_voice_echo_push(const char *text)
{
    (void)text;
    return KVR_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_voice_echo_count(void)
{
    return KVR_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_voice_echo_line(int idx, const char **out)
{
    (void)idx; (void)out;
    return KVR_UNIMPLEMENTED;
}
__attribute__((weak)) int kidpal_voice_echo_capacity(void)
{
    return KVR_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * Repo-tree scanning helpers（host 侧结构齿, key_layer_dispatch / keybar_three_slot
 * 先例）。全部扫描跑在真实工作树上, root 以手持 cwd 定位（从仓根或
 * firmware/main/test/host_app 两处都可跑）。test / host_app / build / .git / assets
 * 目录排除——test 目录里住着本冻结文件自身（其注释合法提及冻结 needle）。
 * ------------------------------------------------------------------------ */

static char kvr_root[KVR_MAX_PATH];
static int kvr_root_ready;

static int kvr_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int kvr_is_dir(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int kvr_is_skipped_dir(const char *name)
{
    static const char *const skips[] = { "test", "host_app", "build", ".git", "assets" };
    for (size_t i = 0; i < sizeof(skips) / sizeof(skips[0]); i++) {
        if (strcmp(name, skips[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static int kvr_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[KVR_MAX_PATH];

    if (kvr_root_ready) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (kvr_is_regular(probe)) {
            snprintf(kvr_root, sizeof(kvr_root), "%s", cands[i]);
            kvr_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int kvr_rel(char *out, size_t cap, const char *rel)
{
    if (!kvr_locate_root()) {
        return 0;
    }
    if (rel[0] == '\0') {
        snprintf(out, cap, "%s", kvr_root);
        return 1;
    }
    snprintf(out, cap, "%s/%s", kvr_root, rel);
    return 1;
}

static int kvr_exists(const char *rel)
{
    char p[KVR_MAX_PATH];
    return kvr_rel(p, sizeof(p), rel) && kvr_is_regular(p);
}

static char *kvr_read(const char *rel, size_t *out_len)
{
    char p[KVR_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!kvr_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > KVR_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

static int kvr_contains(const char *hay, const char *needle)
{
    assert(hay != NULL && needle != NULL); /* logic guard: a NULL needle is a test bug */
    return strstr(hay, needle) != NULL;
}

static int kvr_file_contains(const char *rel, const char *needle)
{
    char *buf = kvr_read(rel, NULL);
    if (buf == NULL) {
        return 0;
    }
    int r = kvr_contains(buf, needle);
    free(buf);
    return r;
}

/* Occurrence count of needle inside one file (0 when unreadable/absent). */
static int kvr_count_in_file(const char *rel, const char *needle)
{
    char *buf;
    int count = 0;
    const char *h;
    size_t n;

    assert(needle != NULL);
    buf = kvr_read(rel, NULL);
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

static int kvr_count_files_with_needle(const char *abs_dir, const char *rel_prefix,
                                       const char *needle, char *first_hit, size_t cap,
                                       int depth)
{
    DIR *d;
    struct dirent *e;
    int count = 0;

    if (depth > 10) {
        return 0;
    }
    d = opendir(abs_dir);
    if (d == NULL) {
        return 0;
    }
    while ((e = readdir(d)) != NULL) {
        char abs_next[KVR_MAX_PATH];
        char rel_next[KVR_MAX_PATH];
        struct stat st;

        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0 ||
            kvr_is_skipped_dir(e->d_name)) {
            continue;
        }
        snprintf(abs_next, sizeof(abs_next), "%s/%s", abs_dir, e->d_name);
        if (rel_prefix[0] != '\0') {
            snprintf(rel_next, sizeof(rel_next), "%s/%s", rel_prefix, e->d_name);
        } else {
            snprintf(rel_next, sizeof(rel_next), "%s", e->d_name);
        }
        if (stat(abs_next, &st) != 0) {
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            count += kvr_count_files_with_needle(abs_next, rel_next, needle, first_hit,
                                                 cap, depth + 1);
        } else if (S_ISREG(st.st_mode)) {
            if ((unsigned long long)st.st_size > KVR_MAX_READ) {
                continue;
            }
            {
                char *buf = kvr_read(rel_next, NULL);
                if (buf != NULL) {
                    if (kvr_contains(buf, needle)) {
                        if (count == 0 && first_hit != NULL) {
                            snprintf(first_hit, cap, "%s", rel_next);
                        }
                        count++;
                    }
                    free(buf);
                }
            }
        }
    }
    closedir(d);
    return count;
}

static int kvr_scan_count(const char *rel_dir, const char *needle, char *first_hit,
                          size_t cap)
{
    char abs[KVR_MAX_PATH];

    if (first_hit != NULL && cap > 0) {
        first_hit[0] = '\0';
    }
    if (!kvr_rel(abs, sizeof(abs), rel_dir) || !kvr_is_dir(abs)) {
        return 0;
    }
    return kvr_count_files_with_needle(abs, rel_dir, needle, first_hit, cap, 0);
}

/* Slice of rel from the start_needle occurrence selected by use_last (0 = first,
 * 1 = last) up to (exclusive) the first occurrence of end_needle AFTER it
 * (end_needle == NULL -> slice to EOF). Returns a malloc'd NUL-terminated slice or
 * NULL（unreadable file / start needle absent）。Caller frees。use_last 供同一签名
 * 在多预处理器分支重复出现时锁定活分支（lcd_display.cc 双 SetChatMessage 定义）。 */
static char *kvr_region_between_ex(const char *rel, const char *start_needle,
                                   const char *end_needle, int use_last)
{
    char *buf = kvr_read(rel, NULL);
    char *s = NULL, *e;
    char *slice;
    size_t span;

    if (buf == NULL) {
        return NULL;
    }
    for (char *h = buf; (h = strstr(h, start_needle)) != NULL; h += 1) {
        s = h; /* keep last hit */
        if (!use_last) {
            s = h;
            break;
        }
    }
    if (s == NULL) {
        free(buf);
        return NULL;
    }
    e = strstr(s + strlen(start_needle), end_needle);
    if (e == NULL) {
        e = buf + strlen(buf); /* 尾锚 NULL 或 start 是文件内最后一个锚——切到 EOF */
    }
    span = (size_t)(e - s);
    slice = (char *)malloc(span + 1);
    assert(slice != NULL);
    memcpy(slice, s, span);
    slice[span] = '\0';
    free(buf);
    return slice;
}

static int kvr_count_in_slice(const char *slice, const char *needle)
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

#define KVR_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(kvr_locate_root(), \
    "KVR RED(infra): repo root not located from cwd — run the ELF from the repo root " \
    "or from firmware/main/test/host_app (see HOST_TEST_CONVENTIONS.md [C4.4])")

/* Shared red-string for the frozen seam not being implemented yet. */
static void kvr_require_implemented(int value, const char *what)
{
    if (value == KVR_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "KVR RED: frozen seam NOT implemented — %s "
                 "(builder must land display/voice_round.{h,c} per the seam contract in "
                 "firmware/main/test/voice_round_ui.acceptance.c header and wire "
                 "voice_round.c + this file into HOST_ACCEPTANCE_SRCS)", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* printf-style helper: dynamic diagnostics pre-formatted into a static buffer then
 * passed to the single-string assert macros（KEYA/KLD/KKB 先例）。 */
static char kvr_fm_buf[1024];
static const char *kvr_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *kvr_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(kvr_fm_buf, sizeof(kvr_fm_buf), fmt, ap);
    va_end(ap);
    return kvr_fm_buf;
}

/* -------------------------------------------------------------------------- *
 * TEST 1 — VQ-A1/A4/A6 场景映射决策表（behavioral, frozen seam [1]）
 * kidpal_voice_round_scene 全表逐格: 思考触发 = stt 切句完成（弹出同刻, 计时起点
 * 锚）; manual 记录语境不逐句进思考; 思考不重入; 播报打断回聆听; 越界防御。
 * RED today: weak sentinel -2。
 * ------------------------------------------------------------------------ */
/* coverage: VQ-A1 -- 思考页接线机制齿行为半：LISTEN_AUTO+STT_FINAL -> KVST_SCENE_THINK（弹出时机 = 孩子说完这一句, 状态切换与弹出同一时刻） */
/* coverage: VQ-A4 -- 打断手感演出侧衔接行为半：SPEAKING+ABORT_SPEECH -> KVST_SCENE_LISTEN（停播回聆听） */
/* coverage: VQ-A6 -- 超时安抚计时起点锚：思考入口 = STT_FINAL 同刻（非回复驱动、非定时器）; 4s 安抚冻结边守卫在 test 4 */
TEST_CASE("语音轮场景映射: kidpal_voice_round_scene 决策表——思考同刻触发+打断回聆听 (VQ-A1/VQ-A4/VQ-A6)",
          "[voice_round_ui][VQ-A1][VQ-A4][VQ-A6][scene]")
{
    /* tooth 1 (VQ-A1): 聆听 auto 态 + stt 切句完成 -> 思考（弹出时机 = 孩子说完
     * 这一句, 服务端 VAD 切句完成 / 音频上行结束; 状态切换与页面弹出同一时刻,
     * requirements §2.2 思考中行逐字语义）。 */
    {
        int sc = kidpal_voice_round_scene(KVR_CTX_LISTEN_AUTO, KVR_EVT_STT_FINAL);
        kvr_require_implemented(sc, "kidpal_voice_round_scene unimplemented");
        TEST_ASSERT_TRUE_MESSAGE(sc == KVST_SCENE_THINK,
            kvr_fm("KVR-A1 RED: 聆听 auto 态 stt 切句完成场景映射 = %d, 期望 "
                   "KVST_SCENE_THINK(%d)——思考页弹出时机未绑 VAD 切句完成 "
                   "(requirements §2.2 思考中行)", sc, KVST_SCENE_THINK));
        TEST_ASSERT_TRUE_MESSAGE(sc != KVST_SCENE_TALK,
            "KVR-A1 RED: stt 切句完成直接跳播报——「孩子说完即思考」被跳过 "
            "(弹出时机分离, 取舍 A 违约)");
    }

    /* tooth 2 (VQ-A6): 同刻性反向钉——回复到达才切思考是错的: 聆听态收到 tts start
     * 直接进播报（不经思考页）, 且思考态入口只由 stt 事件产生（timer/reply 不产
     * 思考入口）。计时起点 = 本态入口 = stt 同刻（NFR-PERF-1 锚）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_voice_round_scene(KVR_CTX_LISTEN_AUTO, KVR_EVT_TTS_START)
            == KVST_SCENE_TALK &&
        kidpal_voice_round_scene(KVR_CTX_THINKING, KVR_EVT_TTS_START)
            == KVST_SCENE_TALK,
        "KVR-A6 RED: 回复到达（tts start）的播报场景映射缺失——聆听/思考态收包后 "
        "无法进入播报态（VQ-A6 反向锚: 思考入口只在 stt, 回复只驱动播报）");

    /* tooth 3 (VQ-A6): 思考态不重入——思考中再收 stt 不切场景（入口唯一, 计时起点
     * 不被二次刷新）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_voice_round_scene(KVR_CTX_THINKING, KVR_EVT_STT_FINAL) < 0,
        "KVR-A6 RED: 思考中收到 stt 重入思考——本态入口被二次刷新, 4s 安抚计时起点 "
        "锚被破坏（NFR-PERF-1 计时起点 = 本态入口）");

    /* tooth 4 (VQ-A2 语义伴生): manual 记录语境整段累积——切句只用于话语回显,
     * 不逐句进思考（requirements §2.1 记录会话聆听语义分流）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_voice_round_scene(KVR_CTX_LISTEN_MANUAL, KVR_EVT_STT_FINAL) < 0,
        "KVR-A1 RED: manual 语境 stt 切句直接进思考——记录会话整段累积语义被 "
        "逐句回复语义覆盖（requirements §2.1 聆听语义分流）");

    /* tooth 5 (VQ-A4): 播报中打断停播回聆听（演出侧状态机衔接; 键位语义消费 [4]
     * 分层裁决, 本表只锁衔接目标态）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_voice_round_scene(KVR_CTX_SPEAKING, KVR_EVT_ABORT_SPEECH)
            == KVST_SCENE_LISTEN,
        "KVR-A4 RED: 播报中打断未回聆听场景——「立即停播并聆听」演出侧衔接缺失 "
        "(requirements §2.2 打断停播行)");

    /* tooth 6: 播完回聆听（auto 连续对话主轴）+ 非法组合不切场景。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_voice_round_scene(KVR_CTX_SPEAKING, KVR_EVT_SPEAK_DONE)
            == KVST_SCENE_LISTEN &&
        kidpal_voice_round_scene(KVR_CTX_LISTEN_AUTO, KVR_EVT_SPEAK_DONE) < 0 &&
        kidpal_voice_round_scene(KVR_CTX_LISTEN_AUTO, KVR_EVT_ABORT_SPEECH) < 0 &&
        kidpal_voice_round_scene(KVR_CTX_LISTEN_MANUAL, KVR_EVT_SPEAK_DONE) < 0 &&
        kidpal_voice_round_scene(KVR_CTX_LISTEN_MANUAL, KVR_EVT_ABORT_SPEECH) < 0 &&
        kidpal_voice_round_scene(KVR_CTX_SPEAKING, KVR_EVT_TTS_START) < 0 &&
        kidpal_voice_round_scene(KVR_CTX_THINKING, KVR_EVT_SPEAK_DONE) < 0 &&
        kidpal_voice_round_scene(KVR_CTX_THINKING, KVR_EVT_ABORT_SPEECH) < 0,
        "KVR-A4 RED: 播完回聆听/非法组合防线缺失——场景决策表闭集被破坏");

    /* tooth 7: 越界防御（ctx/ev 越界 -> -1, 从不为 UB）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kidpal_voice_round_scene((kvr_ctx_t)-1, KVR_EVT_STT_FINAL) == -1 &&
        kidpal_voice_round_scene((kvr_ctx_t)4, KVR_EVT_STT_FINAL) == -1 &&
        kidpal_voice_round_scene(KVR_CTX_LISTEN_AUTO, (kvr_evt_t)-1) == -1 &&
        kidpal_voice_round_scene(KVR_CTX_LISTEN_AUTO, (kvr_evt_t)4) == -1,
        "KVR-A1 RED: 越界语境/事件未防御 (-1)——场景决策表入参闭集防线缺失");
}

/* -------------------------------------------------------------------------- *
 * TEST 2 — VQ-A2 话语回显区机制决策表（behavioral, frozen seam [1]）
 * 逐句入列、句序保持、行数容量滚出（「回显区滚动, 超屏高滚出」的行数模型）、
 * 会话重开清零、防御。
 * RED today: weak sentinel -2。
 * ------------------------------------------------------------------------ */
/* coverage: VQ-A2 -- 聆听回显区机制齿行为半：逐句入列/句序/容量滚出/清零/防御（孩子说的话逐句回显, 超屏高滚出） */
TEST_CASE("语音轮场景映射: 话语回显区逐句机制——push/滚出/防御 (VQ-A2)",
          "[voice_round_ui][VQ-A2][echo]")
{
    char buf_a[64], buf_b[64], buf_c[64];
    const char *line = NULL;
    int cap, n, i;

    /* tooth 0: 非哨兵（frozen seam 落地校验, 红基线读作 not implemented）。 */
    kvr_require_implemented(kidpal_voice_echo_reset(), "kidpal_voice_echo_reset unimplemented");

    /* tooth 1: 容量冻结值（行数模型, 视觉拟合归真机轮）。 */
    cap = kidpal_voice_echo_capacity();
    TEST_ASSERT_TRUE_MESSAGE(cap == KVR_ECHO_MAX_LINES,
        kvr_fm("KVR-A2 RED: 回显区容量 = %d, 期望 %d——行数容量常量漂移", cap,
               KVR_ECHO_MAX_LINES));

    /* tooth 2: 逐句入列 + 句序保持（「我在听！」下逐句回显, 孩子能确认它听清了没）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_echo_reset() == 0,
        "KVR-A2 RED(infra): echo reset failed");
    snprintf(buf_a, sizeof(buf_a), "今天下午有什么课？");
    snprintf(buf_b, sizeof(buf_b), "那明天呢？");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_echo_push(buf_a) == 1,
        "KVR-A2 RED: 首句 push 返回句数 != 1——逐句入列计数错");
    n = kidpal_voice_echo_push(buf_b);
    TEST_ASSERT_TRUE_MESSAGE(n == 2 && kidpal_voice_echo_count() == 2,
        kvr_fm("KVR-A2 RED: 两句 push 后 count=%d/%d, 期望 2——逐句计数漂移",
               n, kidpal_voice_echo_count()));
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_echo_line(0, &line) == 0 &&
                                 line != NULL && strcmp(line, buf_a) == 0,
        "KVR-A2 RED: 回显句序错——最旧句不再是 line 0（逐句回显顺序被破坏）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_echo_line(1, &line) == 0 &&
                                 line != NULL && strcmp(line, buf_b) == 0,
        "KVR-A2 RED: 回显句序错——最新句不在队尾");

    /* tooth 3: 超容量滚出 = 挤最旧（超屏高滚出的行数模型; 新句恒可见）。 */
    for (i = 2; i < KVR_ECHO_MAX_LINES; i++) {
        snprintf(buf_c, sizeof(buf_c), "第%d句", i);
        TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_echo_push(buf_c) == i + 1,
            "KVR-A2 RED(infra): 容量内 push 计数漂移");
    }
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_echo_count() == KVR_ECHO_MAX_LINES,
        "KVR-A2 RED: 未满容量先滚出——回显提前丢句");
    snprintf(buf_c, sizeof(buf_c), "超限新句");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_echo_push(buf_c) == KVR_ECHO_MAX_LINES,
        "KVR-A2 RED: 超限 push 后句数漂移——滚出模型不是固定容量滑动窗");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_echo_line(0, &line) == 0 && line != NULL &&
                                 strcmp(line, buf_b) == 0,
        "KVR-A2 RED: 滚出错句——最旧句（第一句）未被挤出, 或挤出错位");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_echo_line(KVR_ECHO_MAX_LINES - 1, &line) == 0 &&
                                 line != NULL && strcmp(line, buf_c) == 0,
        "KVR-A2 RED: 滚出后最新句不在队尾——孩子看不到刚说的话");

    /* tooth 4: 防御（NULL/空串拒收; 越界行号 -1; out NULL -1）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_echo_push(NULL) == -1 &&
                                 kidpal_voice_echo_push("") == -1,
        "KVR-A2 RED: NULL/空串句未防御 (-1)——空回显行会污染演出面");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_echo_line(-1, &line) == -1 &&
                                 kidpal_voice_echo_line(KVR_ECHO_MAX_LINES, &line) == -1 &&
                                 kidpal_voice_echo_line(0, NULL) == -1,
        "KVR-A2 RED: 越界行号/NULL out 未防御 (-1)");

    /* tooth 5: 会话重开清零（新语音轮开始 = 回显区清空）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_echo_reset() == 0 &&
                                 kidpal_voice_echo_count() == 0,
        "KVR-A2 RED: 会话重开回显区未清零——上一轮话语残留");
}

/* -------------------------------------------------------------------------- *
 * TEST 3 — VQ-A1/A2/A3/A5/A7 活侧渲染接线（structural, frozen seam [2]/[3]/[5]）
 * 思考页首接活侧渲染路径（零调用销账）; stt 分支同刻消费 [1]; 回显区/问答框布线;
 * 闭集单源接线在世。RED today: 各 needle 全部零命中（负向齿）。
 * ------------------------------------------------------------------------ */
/* coverage: VQ-A1 -- 思考页接线机制齿结构半：lcd_display.cc 活侧首接 KVST_SCENE_THINK（零调用现状销账）+ application.cc stt 分支消费场景单源 */
/* coverage: VQ-A2 -- 聆听回显区结构半：回显区容器 + echo push 消费 + user 路由入活 SetChatMessage */
/* coverage: VQ-A3 -- 问答框长文滚动结构齿：问答框容器可滚动 + assistant 全文路由（非一行字幕） */
/* coverage: VQ-A5 -- 四态骨架结构半：骨架对象（回显区/问答框）+ kidpal_voice_state_look 单源接线在世（骨架 owner = 本包, VQ-E4） */
/* coverage: VQ-A7 -- 真机确认轮前置接缝断言（思考页可弹/回显/滚动/打断四接缝在世; 真机轮证据落 changes/dev, human 持板） */
TEST_CASE("语音轮场景映射: 活侧渲染接线——思考页首接+回显/问答框布线 (VQ-A1/VQ-A2/VQ-A3/VQ-A5/VQ-A7)",
          "[voice_round_ui][VQ-A1][VQ-A2][VQ-A3][VQ-A5][VQ-A7][structural]")
{
    char *region;
    int n;

    KVR_REQUIRE_ROOT();

    /* tooth 1: [1] 单源落盘（voice_round.h + voice_round.c 在世）。 */
    TEST_ASSERT_TRUE_MESSAGE(kvr_exists(KVR_VOICE_ROUND_H),
        "KVR-A1 RED: firmware/main/display/voice_round.h 不存在——场景/回显纯逻辑"
        "单源未落盘（seam [1] home 纪律, kidpal_voice_states.c / keybar.c 同款）");
    TEST_ASSERT_TRUE_MESSAGE(kvr_exists(KVR_VOICE_ROUND_C),
        "KVR-A1 RED: firmware/main/display/voice_round.c 不存在——单源无实现"
        "（host runner 行为齿 test 1/2 也将持续红）");

    /* tooth 2 (VQ-A1/A7): 活侧渲染路径首接思考态——kidpal_voice_apply_look
     * (KVST_SCENE_THINK) 首次被活侧消费（现状零调用销账; §2.2 ⚠️ MVP 未接线
     * Phase 1 修复项; 必须经既有 apply_look 单点, 不另立第二渲染源）。 */
    n = kvr_count_in_file(KVR_LCD_CC, "kidpal_voice_apply_look(KVST_SCENE_THINK");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        kvr_fm("KVR-A1 RED: lcd_display.cc 活侧零 KVST_SCENE_THINK 消费（现 %d 处）"
               "——思考页真机不弹的渲染映射缺口未销账 (requirements §2.2 思考中行 "
               "⚠️ MVP 未接线)", n));

    /* tooth 3 (VQ-A1): 思考触发 = stt 分支同刻消费 [1]——application.cc OnIncomingJson
     * 的 "stt" 分支区域内消费 kidpal_voice_round_scene（触发源 = 服务端 VAD 切句
     * 完成, 非 tts start、非定时器——取舍 A; 回显与思考切换同一回调 = 同一时刻）。 */
    region = kvr_region_between_ex(KVR_APP_CC,
                                   "strcmp(type->valuestring, \"stt\") == 0",
                                   "strcmp(type->valuestring, \"llm\") == 0", 0);
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "KVR-A1 RED(infra): application.cc 的 stt 分支区域切取失败（锚漂移——"
        "OnIncomingJson 消息分派结构被改动须复核本齿锚）");
    TEST_ASSERT_TRUE_MESSAGE(kvr_count_in_slice(region, "kidpal_voice_round_scene(") >= 1,
        "KVR-A1 RED: application.cc stt 分支未消费 kidpal_voice_round_scene——"
        "思考页弹出时机未绑「孩子说完这一句」（VAD 切句完成 / 音频上行结束, "
        "requirements §2.2 思考中行; 弹出与状态切换必须同一时刻）");
    TEST_ASSERT_TRUE_MESSAGE(kvr_count_in_slice(region, "SetChatMessage(\"user\"") >= 1,
        "KVR-A1 RED(guard): stt 分支既有话语回显通道丢失——「网关 ASR 切句成文即经 "
        "WS stt 消息下发上屏」既有链路被破坏（§2.2 聆听中行）");
    free(region);

    /* tooth 4 (VQ-A2): 回显区容器 + 纯缓冲消费——lcd_display.cc 落地冻结符号并消费
     * [1] echo push（逐句入列机制单源, 设备侧零重算）。 */
    n = kvr_count_in_file(KVR_LCD_CC, "g_kidpal_voice_echo_box");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        kvr_fm("KVR-A2 RED: g_kidpal_voice_echo_box 仅 %d 处（期望 >= 2: 声明 + 创建/"
               "消费）——聆听话语回显区未落地（「我在听！」标识下方逐句回显）", n));
    n = kvr_count_in_file(KVR_LCD_CC, "kidpal_voice_echo_push");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "KVR-A2 RED: lcd_display.cc 未消费 kidpal_voice_echo_push——回显逐句机制"
        "未接纯逻辑单源（设备侧自抄一份 = 事实源分叉）");

    /* tooth 5 (VQ-A2/A3): 活 SetChatMessage 路由（最后一个定义 = 活分支; WeChat 分支
     * 不可达不作接线面）: user -> 回显区 / assistant -> 问答框。 */
    region = kvr_region_between_ex(KVR_LCD_CC,
                                   "void LcdDisplay::SetChatMessage(const char* role, const char* content)",
                                   "static void kidpal_keybar_sync_locked(void)", 1);
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "KVR-A2 RED(infra): lcd_display.cc 活 SetChatMessage 区域切取失败（尾锚 "
        "kidpal_keybar_sync_locked 漂移须复核本齿锚）");
    TEST_ASSERT_TRUE_MESSAGE(kvr_count_in_slice(region, "strcmp(role, \"user\")") >= 1,
        "KVR-A2 RED: 活 SetChatMessage 未按 role \"user\" 路由到回显区——stt 话语"
        "仍写单行聊天 label（逐句回显区未接管 user 通道）");
    TEST_ASSERT_TRUE_MESSAGE(kvr_count_in_slice(region, "g_kidpal_voice_echo_box") >= 1,
        "KVR-A2 RED: 活 SetChatMessage user 路由未渲染到 g_kidpal_voice_echo_box"
        "——回显区对象与路由断链");
    TEST_ASSERT_TRUE_MESSAGE(kvr_count_in_slice(region, "strcmp(role, \"assistant\")") >= 1,
        "KVR-A3 RED: 活 SetChatMessage 未按 role \"assistant\" 路由——答复文字通道"
        "未接问答框（S-F1.3 AC1「与百科问答同一问答框」口径缺失）");
    TEST_ASSERT_TRUE_MESSAGE(kvr_count_in_slice(region, "g_kidpal_voice_answer_box") >= 1,
        "KVR-A3 RED: 活 SetChatMessage assistant 路由未渲染到 g_kidpal_voice_answer_box"
        "——问答框对象与路由断链（答案仍是一行字幕, 长文滚动无从谈起）");
    free(region);

    /* tooth 6 (VQ-A3): 问答框容器可滚动（答案全文上屏滚动看完, 回引 NFR-READ-1;
     * 屏高滚动观感归真机轮 VQ-A7, 本齿钉机制事实）。回显区 + 问答框两容器均须
     * LV_OBJ_FLAG_SCROLLABLE（现状 0 处）。 */
    n = kvr_count_in_file(KVR_LCD_CC, "LV_OBJ_FLAG_SCROLLABLE");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        kvr_fm("KVR-A3 RED: lcd_display.cc LV_OBJ_FLAG_SCROLLABLE 仅 %d 处（期望 "
               ">= 2: 回显区 + 问答框）——「超出屏高滚动看完」容器机制缺失 "
               "(requirements §2.2 聆听中/播报中行)", n));

    /* tooth 7 (VQ-A5/E4 guard): 闭集单源接线在世——apply_look 仍经 kidpal_voice_
     * state_look 取四态 look（骨架 owner = 本包, 单源不被绕开）。 */
    n = kvr_count_in_file(KVR_LCD_CC, "kidpal_voice_state_look(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "KVR-A5 RED: lcd_display.cc 不再消费 kidpal_voice_state_look——四态 look "
        "单源被绕开（键帽/姿态第二条来源, 闭集零偏移防线拆除）");
}

/* -------------------------------------------------------------------------- *
 * TEST 4 — VQ-A4/A5/A6 键位衔接 + 闭集零偏移 + 4s 安抚冻结边（行为 + 守卫）
 * 打断手感 = 消费 K1a 分层裁决（KD-A1/KD-A3 语义对齐, 不改）; 闭集五成员逐字
 * 复核（强符号在世）; session_state_machine 4s 安抚冻结边在世。
 * ------------------------------------------------------------------------ */
/* coverage: VQ-A4 -- 打断手感键位衔接：key_route_layer 覆盖层列 KD-A1/A3 语义对齐消费（KEY 单击 SESSION_TOGGLE / 长按+BOOT OVERLAY_CLOSE）+ 板级消费链零回归 */
/* coverage: VQ-A5 -- 四态骨架闭集零偏移：kidpal_voice_state_look 五成员逐字复核（强符号在世）+ 防御 */
/* coverage: VQ-A6 -- 4s 安抚冻结边守卫：session_state_machine.cc THINKING->SPEAKING+COMFORT 边在世（NFR-PERF-1 模型承载） */
TEST_CASE("语音轮场景映射: 打断键位衔接+闭集零偏移+4s安抚冻结边 (VQ-A4/VQ-A5/VQ-A6)",
          "[voice_round_ui][VQ-A4][VQ-A5][VQ-A6][keycap][guard]")
{
    kidpal_voice_look_t look;
    int n;

    KVR_REQUIRE_ROOT();

    /* ---- tooth 1 (VQ-A4): key_route_layer 覆盖层列语义 = K1a KD-A1/KD-A3 冻结值
     * （消费对齐, 不改——crossPackageBehaviors: 分层裁决 owner = key-layer-dispatch;
     * KEY 单击 = SESSION_TOGGLE 停播并聆听主轴 / KEY 长按与 BOOT 单击 = OVERLAY_
     * CLOSE 结束会话回原页, 永不翻页）。key_route_layer 强符号在 runner
     * （key_gesture_parser.cc, K1a 接线）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_CLICK, 1)
            == (int)GLOBAL_ACT_SESSION_TOGGLE,
        "KVR-A4 RED: 覆盖层顶层 KEY 单击不再路由 SESSION_TOGGLE——播报中「立即停播"
        "并聆听」的键位主轴漂移（K1a KD-A1 语义对齐被破坏）");
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer(KEY_ID_KEY, KEY_GESTURE_LONG_PRESS, 1)
            == KLD_OVERLAY_CLOSE_FROZEN &&
        (int)key_route_layer(KEY_ID_BOOT, KEY_GESTURE_CLICK, 1)
            == KLD_OVERLAY_CLOSE_FROZEN,
        "KVR-A4 RED: 覆盖层顶层 KEY 长按 / BOOT 单击未路由 OVERLAY_CLOSE——「结束"
        "会话回原页」键位语义漂移（K1a KD-A1/KD-A3）");
    TEST_ASSERT_TRUE_MESSAGE(
        (int)key_route_layer(KEY_ID_BOOT, KEY_GESTURE_CLICK, 1)
            != (int)GLOBAL_ACT_PAGE_NEXT,
        "KVR-A4 RED: 覆盖层顶层 BOOT 单击仍路由翻页——「BOOT≈翻页」等价假绿复活");

    /* ---- tooth 2 (VQ-A4): 板级消费链零回归——SESSION_TOGGLE 与 OVERLAY_CLOSE 会话
     * 级联两处 ToggleChatState 消费在世（K1a 落地件, 本包零改动红线）。 */
    n = kvr_count_in_file(KVR_BOARD_CC, "app.ToggleChatState()");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        kvr_fm("KVR-A4 RED: 板级 dispatch app.ToggleChatState() 仅 %d 处（期望 >= 2: "
               "SESSION_TOGGLE + OVERLAY_CLOSE 会话级联）——打断/结束会话的板级消费"
               "链被破坏", n));

    /* ---- tooth 3 (VQ-A4): application.cc 播报中打断路径在世——AbortSpeaking
     * (kAbortReasonNone)（HandleToggleChatEvent speaking 分支; 停播后经 tts stop
     * 回聆听, 演出侧衔接由 test 1 tooth 5 承载）。 */
    n = kvr_count_in_file(KVR_APP_CC, "AbortSpeaking(kAbortReasonNone)");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        kvr_fm("KVR-A4 RED: application.cc AbortSpeaking(kAbortReasonNone) 仅 %d 处"
               "（期望 >= 2）——播报中 KEY 打断停播路径缺失", n));

    /* ---- tooth 4 (VQ-A5): 闭集五成员逐字零偏移（kidpal_voice_states 冻结闭集;
     * V3 §06 键帽短语逐字; LISTEN 气泡「我在听！」= 回显区标识锚）。强符号在 runner
     * （kidpal_voice_states.c, K2 接线）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_state_look(KVST_SCENE_LISTEN, &look) == 0 &&
                                 look.pose == KVST_POSE_LISTEN &&
                                 look.bubble != NULL &&
                                 strcmp(look.bubble, "我在听！") == 0 &&
                                 look.keycap != NULL &&
                                 strcmp(look.keycap, "说完啦") == 0,
        "KVR-A5 RED: 聆听态 look 漂移（pose/bubble「我在听！」/keycap「说完啦」"
        "任一偏移）——闭集零偏移红线");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_state_look(KVST_SCENE_THINK, &look) == 0 &&
                                 look.pose == KVST_POSE_THINK &&
                                 look.keycap != NULL &&
                                 strcmp(look.keycap, "不等了") == 0,
        "KVR-A5 RED: 思考态 look 漂移（pose-think/keycap「不等了」）——骨架闭集偏移");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_state_look(KVST_SCENE_TALK, &look) == 0 &&
                                 look.pose == KVST_POSE_TALK &&
                                 look.keycap != NULL &&
                                 strcmp(look.keycap, "打断") == 0,
        "KVR-A5 RED: 播报态 look 漂移（pose-talk/keycap「打断」）——打断键帽提示"
        "是打断手感的屏上面");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_state_look(KVST_SCENE_REMIND, &look) == 0 &&
                                 look.pose == KVST_POSE_JUMP &&
                                 look.keycap != NULL &&
                                 strcmp(look.keycap, "知道了") == 0,
        "KVR-A5 RED: 提醒态 look 漂移（pose-jump/keycap「知道了」）——四态骨架第"
        "四成员偏移（提醒包复用面, VQ-E4）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_state_look(KVST_SCENE_LOWBATT, &look) == 0 &&
                                 look.pose == KVST_POSE_SLEEP &&
                                 look.keycap != NULL &&
                                 strcmp(look.keycap, "叫醒它") == 0,
        "KVR-A5 RED: 低电态 look 漂移——闭集五成员不完整");

    /* tooth 5 (VQ-A5): 防御（NULL out / 场景越界 -> -1）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_state_look(KVST_SCENE_LISTEN, NULL) == -1 &&
                                 kidpal_voice_state_look(-1, &look) == -1 &&
                                 kidpal_voice_state_look(KVST_SCENE_LOWBATT + 1,
                                                         &look) == -1,
        "KVR-A5 RED: kidpal_voice_state_look 防御缺失 (-1)——闭集越界不为 UB");

    /* ---- tooth 6 (VQ-A6): 4s 安抚冻结边在世——session_state_machine 冻结转移边
     * THINKING->SPEAKING 带 COMFORT 标记（NFR-PERF-1 行为模型承载, 260829-session-
     * logic 冻结契约零回归; 安抚句到达即播已由 VRT-A5 闭环, 本齿守模型在世）。 */
    n = kvr_count_in_file(KVR_SES_CC, "SES_STATE_SPEAKING, SES_FLAG_COMFORT_SPEECH");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        kvr_fm("KVR-A6 RED: session_state_machine.cc 4s 安抚转移边（THINKING->"
               "SPEAKING+COMFORT）仅 %d 处——思考超 4 秒先开口安抚的冻结模型被触碰 "
               "(NFR-PERF-1)", n));

    (void)kvr_scan_count; /* scan helper 由 test 5 消费, 此处只压未用告警 */
}

/* -------------------------------------------------------------------------- *
 * TEST 5 — boundary：本文件/被测实现注册序守卫 + VQ-E1/E3 范围纪律 canary + 扫描器自检
 *   VQ-A8 结构半（本验收文件 + voice_round.c 是否被 builder 接线进 HOST_ACCEPTANCE_
 *   SRCS, smoke 之前）
 *   VQ-E1 范围纪律（60 秒 timer 零活侧 producer——deferred 非 dropped）
 *   VQ-E3 需求面零修订 canary（requirements §2.2 锚 + 契约面在世）
 *   扫描器自检（结构齿负向必须能红, 防假绿）
 * ------------------------------------------------------------------------ */
/* coverage: VQ-A8 -- host runner 终跑：本文件+voice_round.c 注册进 HOST_ACCEPTANCE_SRCS 结构守卫 + suite 级判读锚（文件尾「VQ-A8 终跑判读」段） */
/* coverage: VQ-E1 -- 范围纪律：60 秒 timer 零活侧 producer（SES_EVT_SESSION_TIMEOUT_60S 仅存于纯模型两文件, deferred 非 dropped） */
/* coverage: VQ-E3 -- 需求面零修订 canary：requirements §2.2 状态表锚 + 契约面在世 */
TEST_CASE("语音轮场景映射: 注册序守卫+范围纪律canary+扫描器自检 (VQ-A8/VQ-E1/VQ-E3)",
          "[voice_round_ui][VQ-A8][VQ-E1][VQ-E3][boundary]")
{
    char *mk;
    char hit[KVR_MAX_PATH];
    int n;

    KVR_REQUIRE_ROOT();

    /* tooth 1 (VQ-A8): 本文件 + voice_round.c 须接线进 HOST_ACCEPTANCE_SRCS 且排在
     * host_infra_smoke 之前（smoke 末位 = 最先执行, 包 0 boundary 齿注册序守卫）。
     * RED until builder wires. */
    mk = kvr_read(KVR_CMAKELISTS, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(mk,
        "KVR RED(infra): host_app/main/CMakeLists.txt unreadable");
    {
        const char *mine = strstr(mk, "voice_round_ui.acceptance.c");
        const char *src = strstr(mk, "voice_round.c");
        const char *smoke = strstr(mk, "host_infra_smoke.acceptance.c");
        TEST_ASSERT_TRUE_MESSAGE(mine != NULL,
            "VQ-A8 RED: voice_round_ui.acceptance.c 未接线进 HOST_ACCEPTANCE_SRCS "
            "— host 验收构建未接本包（BUILDER DUTY: 与本文件头注 [6] 同款接线）");
        TEST_ASSERT_TRUE_MESSAGE(src != NULL,
            "VQ-A8 RED: display/voice_round.c 未接线进 HOST_ACCEPTANCE_SRCS — 行为齿"
            "强符号源缺失（weak 哨兵永不被覆盖, test 1/2 恒红）");
        TEST_ASSERT_TRUE_MESSAGE(smoke != NULL,
            "KVR RED(infra): host_infra_smoke.acceptance.c 从 HOST_ACCEPTANCE_SRCS "
            "丢失 — 包 0 守卫被移除");
        TEST_ASSERT_TRUE_MESSAGE(mine < smoke && src < smoke,
            "VQ-A8 RED: 本包两行必须排在 host_infra_smoke.acceptance.c 之前 "
            "(smoke 末位 = 最先执行, boundary 齿注册序守卫)");
    }
    free(mk);

    /* tooth 2 (VQ-E1): 60 秒 timer 零活侧 producer——SES_EVT_SESSION_TIMEOUT_60S
     * 只存在于纯模型两文件（session_state_machine.h + .cc, 超时 = 显式事件注入,
     * 绝无真实 timer）; 活侧 producer 接线归 M1 后续批次（deferred 非 dropped,
     * §2.1 对账项）。本包若引入第三处引用 = 范围膨胀, 本齿咬住。 */
    n = kvr_scan_count("firmware/main", "SES_EVT_SESSION_TIMEOUT_60S", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 2,
        kvr_fm("VQ-E1 RED: SES_EVT_SESSION_TIMEOUT_60S 出现于 %d 文件（冻结口径 = 2: "
               "session_state_machine.h/.cc 纯模型）——60 秒 timer 活侧 producer "
               "接线不在本包（VQ-E1 deferred 红线; 首个命中: %s）", n, hit));

    /* tooth 3 (VQ-E3): 需求面零修订 canary——requirements §2.2 状态表锚在世（思考中
     * 行/聆听中行语义权威; 本包只留实现证据, 零 docs 面改动）+ 契约面在世。 */
    TEST_ASSERT_TRUE_MESSAGE(kvr_exists(KVR_REQUIREMENTS),
        "VQ-E3 RED(infra): docs/requirements.md 丢失——需求面被触碰（VQ-E3 零修订"
        "红线）");
    TEST_ASSERT_TRUE_MESSAGE(
        kvr_file_contains(KVR_REQUIREMENTS, "状态切换与页面弹出同一时刻"),
        "VQ-E3 RED: requirements §2.2 思考中行锚（状态切换与页面弹出同一时刻）丢失"
        "——需求面被修订（VQ-E3: 发现冲突走 spec-lane 逃生门, 不预先改需求）");
    TEST_ASSERT_TRUE_MESSAGE(kvr_exists(KVR_CONTRACT),
        "KVR RED(infra): contract file lost — 契约面被触碰（本包零契约触碰红线）");

    /* tooth 4: 扫描器自检（防假绿: 负向齿必须能红）。 */
    TEST_ASSERT_TRUE_MESSAGE(kvr_exists(KVR_APP_CC),
        "KVR RED(infra): scanner cannot see a known-good file — 扫描器失灵");
    TEST_ASSERT_FALSE_MESSAGE(kvr_exists("firmware/main/__kvr_no_such__.cc"),
        "KVR RED(infra): scanner reports a nonexistent file as present");
    n = kvr_scan_count("firmware/main", "__kvr_no_such_needle__", hit, sizeof(hit));
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        "KVR RED(infra): scan matched a needle that cannot exist — 负向断言会假绿");
    n = kvr_count_in_file(KVR_APP_CC, "OnIncomingJson");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "KVR RED(infra): occurrence counter cannot count a known literal — 计数器失灵");
    {
        char *region = kvr_region_between_ex(KVR_APP_CC,
                                             "strcmp(type->valuestring, \"stt\") == 0",
                                             "strcmp(type->valuestring, \"llm\") == 0",
                                             0);
        TEST_ASSERT_NOT_NULL_MESSAGE(region,
            "KVR RED(infra): region slicer cannot slice a known region — 切区器失灵");
        TEST_ASSERT_TRUE_MESSAGE(
            kvr_count_in_slice(region, "SetChatMessage(\"user\"") >= 1,
            "KVR RED(infra): region slicer lost a known literal — 切区器计数失灵");
        free(region);
    }
}

/* -------------------------------------------------------------------------- *
 * TEST 6 — VQ-A1/A2/A3/A5 真机轮回归增补齿（device-round VQ-A7 r1 深化; structural,
 * seam [8]; 增补条目单独成节, 既有齿 TEST 1-5 零改动）
 * 2026-09-17 human 真机确认轮第 1 轮（板上 8a3fc07）暴露三处 host 齿断言面缺口:
 *   ① TTS_START 喂入时机: 方向 B 网关 stt 后 10ms 即发 tts start（先于 LLM 出结果,
 *      第一句答案距 stt 12 秒）, 活侧经 tts start -> SetDeviceState(kDeviceState-
 *      Speaking) -> SetStatus(SPEAKING) -> apply_look(KVST_SCENE_TALK) 间接触发场景
 *      切换, THINKING 存活 10ms 被顶掉——「开始播报」语义须对齐实际起播时刻。
 *   ② 回显/问答 label 只设 text_color 未设字体——LVGL 默认字体无中文字形, 整段空白
 *      零上屏（对照同文件 keybar 三 label 显式 font_puhui_14_1 先例）。
 *   ③ echo/answer 两容器 TOP_MID 直接挂裸 screen 顶层, 被页面内容盖住; 姿态 96×96
 *      居中叠首页猫——WF-B「左小伴 · 右内容区」骨架未落成。
 * RED today（设计态红, 义务红形态见文件尾「增补齿义务红形态」段）。
 * ------------------------------------------------------------------------ */
/* coverage: VQ-A1 -- 增补齿: KVR_EVT_TTS_START 喂入点 = tts sentence_start 处理分支（实际起播时刻, 经冻结映射单源消费）, tts start 帧路径零场景切换触发（THINKING 10ms 被顶掉销账; device-round-vqa7-r1 事实②） */
/* coverage: VQ-A2 -- 增补齿: 回显 label 显式全量中文字体（默认字体无中文字形零上屏销账, keybar 全量 puhui 先例形态; device-round-vqa7-r1 事实③） */
/* coverage: VQ-A3 -- 增补齿: 问答 label 显式全量中文字体（同上, 型号由实现按 NFR-READ-1 下限选, 禁 basic 稀疏子集） */
/* coverage: VQ-A5 -- 增补齿: echo/answer 容器挂载于语音轮可见子树/语音层而非裸 screen 顶层 + 告别 TOP_MID 居中叠装（首页遮蔽/骨架未成页销账; device-round-vqa7-r1 事实④, WF-B 左小伴右内容骨架） */
TEST_CASE("语音轮场景映射: 真机轮回归增补——喂入时机+全量字体+可见面挂载 (VQ-A1/VQ-A2/VQ-A3/VQ-A5)",
          "[voice_round_ui][VQ-A1][VQ-A2][VQ-A3][VQ-A5][structural][device-round]")
{
    char *region;
    char *rparent;
    int n, on_screen, reparented, in_list;

    KVR_REQUIRE_ROOT();

    /* ---- tooth 1 (VQ-A1 增补): tts start 帧处理路径零场景切换触发。区域 = tts 分支
     * 头段（start + stop 两个子分支, sentence_start 锚之前）。禁三种场景切换载体:
     * SetDeviceState(kDeviceStateSpeaking)（现状载体——经 SetStatus(SPEAKING)->
     * apply_look(KVST_SCENE_TALK) 间接触发, 即 THINKING 10ms 被顶掉根因）+
     * kidpal_voice_round_scene( / kidpal_voice_apply_look(（任何直接触发形态）。
     * 修复后喂入与起播态迁往 sentence_start 分支（tooth 2）。 */
    region = kvr_region_between_ex(KVR_APP_CC,
                                   "strcmp(type->valuestring, \"tts\") == 0",
                                   "strcmp(state->valuestring, \"sentence_start\") == 0",
                                   0);
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "KVR-A1 RED(infra): application.cc tts 分支头段切取失败（锚漂移——tts 帧"
        "分派结构被改动须复核本齿锚）");
    TEST_ASSERT_TRUE_MESSAGE(
        kvr_count_in_slice(region, "SetDeviceState(kDeviceStateSpeaking)") == 0 &&
        kvr_count_in_slice(region, "kidpal_voice_round_scene(") == 0 &&
        kvr_count_in_slice(region, "kidpal_voice_apply_look(") == 0,
        "KVR-A1 RED(增补): tts start 帧处理路径仍触发语音轮场景切换——「开始播报」"
        "语义未对齐实际起播时刻, THINKING 页在 LLM 出结果前即被顶掉（device-round "
        "VQ-A7 r1 事实②; 修复方向 = KVR_EVT_TTS_START 喂入点迁往首个 tts "
        "sentence_start 分支, 映射表零改动只动触发点）");
    free(region);

    /* ---- tooth 2 (VQ-A1 增补): KVR_EVT_TTS_START 喂入点 = tts sentence_start 处理
     * 分支（首个分句到达 = 实际起播时刻; 「音频真正起播」等价锚若被采用须返 planner
     * 重新钉锚——本齿按 device-round 首选形态冻结）。喂入必须经冻结映射单源
     * kidpal_voice_round_scene（映射表零改动红线: 场景切换不得绕开决策表——
     * 表值行为级锁在 TEST 1, 喂入通道挪动不触表）。 */
    region = kvr_region_between_ex(KVR_APP_CC,
                                   "strcmp(state->valuestring, \"sentence_start\") == 0",
                                   "strcmp(type->valuestring, \"stt\") == 0",
                                   0);
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "KVR-A1 RED(infra): application.cc sentence_start 分支区域切取失败（锚漂移"
        "——tts 帧分派结构被改动须复核本齿锚）");
    TEST_ASSERT_TRUE_MESSAGE(
        kvr_count_in_slice(region, "kidpal_voice_round_scene(") >= 1 &&
        kvr_count_in_slice(region, "KVR_EVT_TTS_START") >= 1,
        "KVR-A1 RED(增补): application.cc sentence_start 分支未喂入 KVR_EVT_TTS_START"
        "（经 kidpal_voice_round_scene 单源）——「开始播报」场景切换未绑实际起播时刻"
        "（device-round VQ-A7 r1 事实②修复方向; 映射表零改动, 只动触发点）");
    free(region);

    /* ---- tooth 3 (VQ-A2/A3 增补): 回显/问答 label 显式全量中文字体——对照 keybar
     * 先例 lv_obj_set_style_text_font(label, &font_puhui_14_1, 0) 同形态（全量
     * puhui 家族, 具体型号由实现按 NFR-READ-1 可读性下限选）; 禁 basic 稀疏子集
     * （缺中文字形 = 整段空白零上屏回归, 键帽 KB-A5 同族根因）。 */
    n = kvr_count_in_file(KVR_LCD_CC,
        "lv_obj_set_style_text_font(g_kidpal_voice_echo_label, &font_puhui_");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        kvr_fm("KVR-A2 RED(增补): 回显 label 未显式设置全量中文字体（现 %d 处）——"
               "LVGL 默认字体无中文字形, 孩子说的话整段空白零上屏（device-round "
               "VQ-A7 r1 事实③; 对照 keybar font_puhui_14_1 先例, 型号按 NFR-READ-1 "
               "下限选）", n));
    n = kvr_count_in_file(KVR_LCD_CC,
        "lv_obj_set_style_text_font(g_kidpal_voice_answer_label, &font_puhui_");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        kvr_fm("KVR-A3 RED(增补): 问答 label 未显式设置全量中文字体（现 %d 处）——"
               "默认字体无中文字形, 答案整段空白零上屏（device-round VQ-A7 r1 事实③）",
               n));
    TEST_ASSERT_TRUE_MESSAGE(
        kvr_count_in_file(KVR_LCD_CC,
            "lv_obj_set_style_text_font(g_kidpal_voice_echo_label, &font_puhui_basic_") == 0 &&
        kvr_count_in_file(KVR_LCD_CC,
            "lv_obj_set_style_text_font(g_kidpal_voice_answer_label, &font_puhui_basic_") == 0,
        "KVR-A2 RED(增补): 回显/问答 label 误配 basic 稀疏子集字体——子集缺中文字形, "
        "零上屏回归（键帽 KB-A5 同族根因, 禁回退）");

    /* ---- tooth 4 (VQ-A5 增补): echo/answer 容器挂载于语音轮可见子树/语音层而非裸
     * screen 顶层（现状 TOP_MID 直接挂 lv_screen_active() 裸根, 被页面内容盖住）。
     * 行为面「不在被页面内容遮挡的位置」, 不钉死具体父级名（聊天 chrome 子树 /
     * 语音轮专用内容面 = 实现取舍）: 合格形态三选一 = 创建点已不在裸 screen 根 /
     * 显式 lv_obj_set_parent 重挂 / 入 KidPalReparentChatToLayer 聊天 chrome 迁移
     * 清单。运行期可见性本体归真机轮 VQ-A7 oracle, 本齿钉挂载面销账。 */
    rparent = kvr_region_between_ex(KVR_LCD_CC,
                                    "void LcdDisplay::KidPalReparentChatToLayer",
                                    "void LcdDisplay::KidPalVoiceChromeFront",
                                    0);
    TEST_ASSERT_NOT_NULL_MESSAGE(rparent,
        "KVR-A5 RED(infra): lcd_display.cc 聊天 chrome 迁移函数区域切取失败（锚漂移"
        "——KidPalReparentChatToLayer 结构被改动须复核本齿锚）");
    on_screen = kvr_count_in_file(KVR_LCD_CC,
                                  "g_kidpal_voice_echo_box = lv_obj_create(screen)");
    reparented = kvr_count_in_file(KVR_LCD_CC,
                                   "lv_obj_set_parent(g_kidpal_voice_echo_box");
    in_list = kvr_count_in_slice(rparent, "g_kidpal_voice_echo_box");
    TEST_ASSERT_TRUE_MESSAGE(!on_screen || reparented >= 1 || in_list >= 1,
        kvr_fm("KVR-A5 RED(增补): 聆听回显区容器仍裸挂 screen 顶层（裸根创建=%d, "
               "显式重挂=%d, chrome 迁移清单=%d）——被页面内容遮挡, 语音轮期间零可见"
               "（device-round VQ-A7 r1 事实④; 挂载须入语音轮可见子树/语音层, 父级"
               "名不锁）", on_screen, reparented, in_list));
    on_screen = kvr_count_in_file(KVR_LCD_CC,
                                  "g_kidpal_voice_answer_box = lv_obj_create(screen)");
    reparented = kvr_count_in_file(KVR_LCD_CC,
                                   "lv_obj_set_parent(g_kidpal_voice_answer_box");
    in_list = kvr_count_in_slice(rparent, "g_kidpal_voice_answer_box");
    TEST_ASSERT_TRUE_MESSAGE(!on_screen || reparented >= 1 || in_list >= 1,
        kvr_fm("KVR-A5 RED(增补): 播报问答框容器仍裸挂 screen 顶层（裸根创建=%d, "
               "显式重挂=%d, chrome 迁移清单=%d）——同上, 语音轮期间零可见", on_screen,
               reparented, in_list));
    free(rparent);

    /* ---- tooth 5 (VQ-A5 增补): 告别 TOP_MID 居中叠装——两容器不再水平居中压在
     * 姿态列上方（现状 TOP_MID 挂裸 screen + 姿态 96×96 居中 = 叠首页猫遮蔽缺陷面;
     * WF-B「左小伴 · 右内容区」骨架 = requirements v1.24 human 冻结, UIUX-DESIGN-
     * BRIEF §5 语音轮四态行同锚）。正几何（pose 左/内容右的像素级分配）归真机轮
     * VQ-A7 观感轮, 本齿钉反形态销账; 若实现采用右侧专用面板内居中的合法形态, 须返
     * planner 重新钉锚（面板内居中合法, 本齿锚不区分）。 */
    TEST_ASSERT_TRUE_MESSAGE(
        kvr_count_in_file(KVR_LCD_CC,
            "lv_obj_align(g_kidpal_voice_echo_box, LV_ALIGN_TOP_MID") == 0 &&
        kvr_count_in_file(KVR_LCD_CC,
            "lv_obj_align(g_kidpal_voice_echo_box, LV_ALIGN_BOTTOM_MID") == 0 &&
        kvr_count_in_file(KVR_LCD_CC,
            "lv_obj_align(g_kidpal_voice_echo_box, LV_ALIGN_CENTER") == 0 &&
        kvr_count_in_file(KVR_LCD_CC,
            "lv_obj_center(g_kidpal_voice_echo_box") == 0 &&
        kvr_count_in_file(KVR_LCD_CC,
            "lv_obj_align(g_kidpal_voice_answer_box, LV_ALIGN_TOP_MID") == 0 &&
        kvr_count_in_file(KVR_LCD_CC,
            "lv_obj_align(g_kidpal_voice_answer_box, LV_ALIGN_BOTTOM_MID") == 0 &&
        kvr_count_in_file(KVR_LCD_CC,
            "lv_obj_align(g_kidpal_voice_answer_box, LV_ALIGN_CENTER") == 0 &&
        kvr_count_in_file(KVR_LCD_CC,
            "lv_obj_center(g_kidpal_voice_answer_box") == 0,
        "KVR-A5 RED(增补): 回显/问答容器仍水平居中（TOP_MID/CENTER 族）叠装在姿态列"
        "上方——WF-B「左小伴 · 右内容区」骨架未落成, 居中形态即首页猫遮蔽缺陷面"
        "（device-round VQ-A7 r1 事实④）");
}

/* -------------------------------------------------------------------------- *
 * TEST 7 — VQ-A2/VQ-A5 真机轮第 2 轮回归增补齿（device-round VQ-A7 r2; behavioral
 * + structural, seam [9]; 增补条目单独成节, 既有齿 TEST 1-6 零改动）
 * 2026-09-17 human 真机确认轮第 2 轮（板上 54aa903, 功能四项全通: 思考态持续显示/
 * 回显/播报/打断）暴露一类渲染面偏差「各态内容区标识件缺失 + 气泡写点挂旧聊天
 * chrome」（零状态机/逻辑改动, 全在演出面）:
 *   ① 思考态右内容区缺「思考中」标头（WF-B-02）——kidpal_voice_states.c THINK
 *      bubble 现值 NULL（「问号/气泡文案归 design review」留白, human 本次指令
 *      消解 = §2.2 态名口径「思考中」）;
 *   ② 聆听态内容区缺「我在听！」标识（WF-B-01; VQ-A2 冻结原文「回显在『我在听！』
 *      标识下方」——回显行须挂标头下方）;
 *   ③ 播报态右内容区标头缺（WF-B-03 线框标头字面 =「播报」）;
 *   ④ apply_look 的 look.bubble 现写 g_kidpal_chat_label（chat_message_label_,
 *      bottom_bar_ 键帽条内旧挂点）——新整页版式下观感不是右区标头, 统一改挂
 *      g_kidpal_voice_page 右内容区标头位。
 * 问号图标资产: art_assets.c 闭集无 question/问号资产（planner 核实）= fail-closed
 * 已上报（归后续资产批）, 文案标头先行——本齿不造图标齿。
 * 与 TEST 4 的关系（判读声明）: TEST 4 tooth 4 对 THINK/TALK 只断 pose/keycap、
 * 不断 bubble（现值 NULL 无断言面）——三态 bubble 值的新权威 = 本 TEST tooth 1,
 * TEST 4 零改动、零冲突（新值落地后 TEST 4 仍绿, 无须裁量）。
 * RED today（设计态红, 义务红形态见文件尾「增补齿义务红形态（r2）」段）。
 * ------------------------------------------------------------------------ */
/* coverage: VQ-A2 -- 真机 r2 增补齿: 聆听态「我在听！」标头在世 + 回显行挂标头下方（同父创建序在先; device-round-vqa7-r2 事实②, WF-B-01/VQ-A2 冻结原文「回显在『我在听！』标识下方」） */
/* coverage: VQ-A5 -- 真机 r2 增补齿: 三态 bubble 闭集标头值（LISTEN 零偏移复核 + THINK「思考中」human 钉消解 design-review 留白 + TALK「播报」WF-B-03 线框字面）+ 标头写点统一挂 voice_page 右内容区、旧聊天 chrome 挂点销账（device-round-vqa7-r2 事实①③④） */
TEST_CASE("语音轮场景映射: 真机轮r2回归增补——三态标头闭集+写点统一+回显挂标头下 (VQ-A2/VQ-A5)",
          "[voice_round_ui][VQ-A2][VQ-A5][structural][device-round]")
{
    kidpal_voice_look_t look;
    char *region;
    int n;

    KVR_REQUIRE_ROOT();

    /* ---- tooth 1 (VQ-A5/VQ-A2): 三态 bubble 闭集标头值（kidpal_voice_states 单源,
     * 强符号在 runner）——LISTEN 既有零偏移复核; THINK「思考中」（human 钉 = §2.2
     * 态名口径, 消解 design-review 留白）; TALK「播报」（WF-B-03 线框标头字面）。
     * 本齿 = 三态 bubble 值新权威（与 TEST 4 关系见本 TEST 头注判读声明）。 */
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_state_look(KVST_SCENE_LISTEN, &look) == 0 &&
                                 look.bubble != NULL &&
                                 strcmp(look.bubble, "我在听！") == 0,
        "KVR-A2 RED(r2): 聆听态 bubble 漂移——「我在听！」标识文案偏移（WF-B-01; "
        "既有零偏移复核, 与 TEST 4 tooth 4 同值）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_state_look(KVST_SCENE_THINK, &look) == 0 &&
                                 look.bubble != NULL &&
                                 strcmp(look.bubble, "思考中") == 0,
        "KVR-A5 RED(r2): 思考态 bubble 缺失——右内容区无「思考中」标头（WF-B-02; "
        "kidpal_voice_states.c 现值 NULL, design-review 留白由 human 真机 r2 指令"
        "消解 = §2.2 态名口径; 问号图标缺资产 fail-closed 已上报, 文案标头先行）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_voice_state_look(KVST_SCENE_TALK, &look) == 0 &&
                                 look.bubble != NULL &&
                                 strcmp(look.bubble, "播报") == 0,
        "KVR-A5 RED(r2): 播报态 bubble 缺失——右内容区无「播报」标头（WF-B-03 线框"
        "标头字面; kidpal_voice_states.c 现值 NULL）");

    /* ---- tooth 2 (VQ-A5): 标头写点统一——apply_look 的 look.bubble 写点不再写
     * g_kidpal_chat_label（bottom_bar_ 内 chat_message_label_ 旧挂点, 新整页版式
     * 下观感不是右区标头）, 改写 g_kidpal_voice_page 右内容区标头对象（新增, 名
     * 不锁）。三件断言:
     *  a) apply_look 区域零 g_kidpal_chat_label（旧挂点写点清零; 与 b) 合取 =
     *     bubble 写点存在且不在旧挂点, 即「旧挂点写点清零或标头写点唯一」）;
     *  b) apply_look 区域仍保有 look.bubble 写点（单源仍从闭集取文案, 不另抄
     *     第二份文案来源）;
     *  c) 标头对象在世: g_kidpal_voice_page 直接子件新增一件（现状 pose/echo/
     *     answer 三件, 标头 = 第四件; label/容器两形态均合法, 精确符号名不锁）。
     * 运行期观感归真机轮 VQ-A7 oracle, 本齿钉写点/挂载面销账。 */
    region = kvr_region_between_ex(KVR_LCD_CC,
                                   "static void kidpal_voice_apply_look(int scene)",
                                   "static void kidpal_chrome_mid_sync_locked(void)",
                                   0);
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "KVR-A5 RED(infra): lcd_display.cc apply_look 区域切取失败（尾锚 "
        "kidpal_chrome_mid_sync_locked 漂移须复核本齿锚）");
    n = kvr_count_in_slice(region, "g_kidpal_chat_label");
    TEST_ASSERT_TRUE_MESSAGE(n == 0,
        kvr_fm("KVR-A5 RED(r2): apply_look 的 bubble 写点仍挂 g_kidpal_chat_label"
               "（现 %d 处）——旧挂点在 bottom_bar_ 键帽条内, 新整页版式下观感不是"
               "右内容区标头（device-round VQ-A7 r2 事实④; 统一改挂 voice_page 右"
               "内容区标头位, 名不锁）", n));
    TEST_ASSERT_TRUE_MESSAGE(kvr_count_in_slice(region, ", look.bubble)") >= 1,
        "KVR-A5 RED(r2): apply_look 零 look.bubble 写点——三态标头文案不再从 "
        "kidpal_voice_states 闭集单源取出（另抄第二份 = 事实源分叉, 闭集零偏移"
        "防线拆除）");
    free(region);
    n = kvr_count_in_file(KVR_LCD_CC, "create(g_kidpal_voice_page)");
    TEST_ASSERT_TRUE_MESSAGE(n >= 4,
        kvr_fm("KVR-A5 RED(r2): g_kidpal_voice_page 直接子件创建仅 %d 处（期望 "
               ">= 4: pose + 回显区 + 问答框 + 新增标头）——右内容区标头对象未落地 "
               "（device-round VQ-A7 r2 事实①②③; 标头符号名不锁, label/容器两形态"
               "均合法）", n));

    /* ---- tooth 3 (VQ-A2): 回显行挂标头下方——WF-B-01/VQ-A2 冻结原文「回显在
     * 『我在听！』标识下方」。标头对象与 echo_box 同父（needle 含父参
     * g_kidpal_voice_page 即钉同父）且创建序在 echo_box 之前（LVGL 同父子件叠序 =
     * 创建序, 标头在上、回显行在下）。区域 = voice_page 创建起至 echo_box 创建止
     * （尾锚排除）, 其中 create(g_kidpal_voice_page) >= 2 = echo 之前除 pose 外
     * 尚有新子件（= 标头; pose 前后均满足「在 echo_box 之前」, 精确位置不锁）。
     * 若实现采用其它换序机制（lv_obj_move_to_index 等）, 须返 planner 重新钉锚。 */
    region = kvr_region_between_ex(KVR_LCD_CC,
                                   "g_kidpal_voice_page = lv_obj_create(lv_layer_top())",
                                   "g_kidpal_voice_echo_box = lv_obj_create(g_kidpal_voice_page)",
                                   0);
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "KVR-A2 RED(infra): lcd_display.cc voice_page 布局区域切取失败（锚漂移——"
        "SetupUI 整页版式结构被改动须复核本齿锚）");
    n = kvr_count_in_slice(region, "create(g_kidpal_voice_page)");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        kvr_fm("KVR-A2 RED(r2): echo_box 创建之前 voice_page 子件仅 %d 件（期望 "
               ">= 2: pose + 标头）——标头未挂回显行上方, 「回显在『我在听！』标识"
               "下方」未落成（device-round VQ-A7 r2 事实②; WF-B-01 冻结原文）", n));
    free(region);
}

/* ---------------------------------------------------------------------------
 * VQ-A8 终跑判读（suite 级判读说明, review-packet 呈 supervisor）:
 *   判据 = 相对有效基线零新增红 + 本包新齿全绿。
 *   有效基线 = freeze 相位 2026-09-16 acceptance-author 实跑 host 面全量既有齿:
 *     18 Tests / 0 Failures / 0 Ignored OK（n1b 期锚 13 + k2 新齿 5, exit 0;
 *     在册既存红清单 = 空——当前为零新增红基线, 无任何在册红豁免项）。
 *   本包义务红（设计态, builder 落地后转绿, 不计入「新增红」违约）:
 *     - test 1/2（weak 哨兵 -2: voice_round.{h,c} 未落地 = 真实缺失行为红）;
 *     - test 3（各接线 needle 零命中: 思考页零调用销账前为真实缺失红）;
 *     - test 5 tooth 1（注册序: builder 未接线前为真实缺失红）。
 *   终跑复核命令与实测数字见交付报告 red-baseline 段（planner 转写落账）。
 *
 * RED baseline 判读口径（freeze 相位, acceptance-author）:
 *   通道① 既有 host runner 全量实跑 = 18 Tests / 0 Failures / 0 Ignored OK
 *   （2026-09-16 实跑, exit 0; 在册既存红清单 = 空）。
 *   通道② 本文件新齿 C 类冻结验收实编（DEV-GOVERNANCE 验收测试纪律 §4）:
 *     gcc -std=c11 -Wall -Wextra -fsyntax-only -I firmware/main/test/acceptance/
 *         red-baseline -I firmware/main/test firmware/main/test/voice_round_ui.
 *         acceptance.c
 *     编译通道 error 为零（验收文件本身可编译）+ 解析性红定性: kidpal_voice_round_
 *     scene / kidpal_voice_echo_* 全部 weak 哨兵 -2（test 1/2 全齿红）、
 *     kidpal_voice_apply_look(KVST_SCENE_THINK 与 g_kidpal_voice_echo_box /
 *     g_kidpal_voice_answer_box / LV_OBJ_FLAG_SCROLLABLE 在 lcd_display.cc 全零
 *     命中、application.cc stt 分支零 kidpal_voice_round_scene 消费（test 3 全齿
 *     红）、test 5 tooth 1 注册行缺失——各齿对现状必红, 红基线成立（接缝缺失即
 *     测试意图缺的真实行为）。test 4 对现状即绿（消费在世件: K1a 键位语义 /
 *     K2 闭集 / session_state_machine 冻结边——零回归守卫本性, 不是假绿）。
 *   cwd 口径: 结构齿以真实工作树扫描, root 自手持 cwd 定位（仓根 / firmware/main/
 *   test/host_app 均可）; delivery/changes/dev/ 下证据路径一律按相对路径 + 仓根
 *   cwd 前提（C 侧遗留指针纪律, root 定位吃 cwd）。
 * ---------------------------------------------------------------------------
 * 增补齿义务红形态（2026-09-17 device-round VQ-A7 r1 增补, acceptance-author）:
 *   增补齿 = 设计态红（现实现下 TEST 6 FAIL 为预期, fixer 落地后转绿, verifier 终验;
 *   不计入「新增红」违约——真实缺失行为红, 与 freeze 相位义务红同性质）:
 *     - TEST 6 tooth 1（tts start 帧路径仍含 SetDeviceState(kDeviceStateSpeaking)
 *       ——现状喂入载体在位, 场景切换未迁离 tts start 帧）;
 *     - TEST 6 tooth 2（sentence_start 分支零 kidpal_voice_round_scene / KVR_EVT_
 *       TTS_START 喂入——「开始播报」未绑实际起播时刻）;
 *     - TEST 6 tooth 3（echo/answer label 零 lv_obj_set_style_text_font 全量字体
 *       配置——默认字体无中文字形零上屏）;
 *     - TEST 6 tooth 4（echo/answer 容器仍裸挂 screen 根创建, 无显式重挂/迁移清单
 *       在位——被页面内容遮挡）;
 *     - TEST 6 tooth 5（两容器仍 TOP_MID 族居中叠装在姿态列上方）。
 *   义务红消除判据 = TEST 6 全齿绿（喂入在 sentence_start 分支经映射单源 / 字体
 *   全量 puhui 家族 / 挂载离裸根三形态之一 / 居中反形态销账）; suite 终跑判读同
 *   VQ-A8 段口径（相对有效基线零新增红 + 本包齿全绿, 增补齿并入本包齿集）。
 *   锚漂移防护: TEST 6 各区域切取锚（tts/sentence_start/stt 帧分派、KidPalReparent
 *   ChatToLayer 迁移函数）漂移时以 RED(infra) 形态报出, 须返 acceptance-author 复核
 *   钉锚, 不得由 builder/fixer 改齿。
 * ---------------------------------------------------------------------------
 * 增补齿义务红形态（r2）（2026-09-17 device-round VQ-A7 r2 增补, acceptance-author）:
 *   TEST 7 = 设计态红（现实现下 FAIL 为预期, fixer 落地后转绿, verifier 终验;
 *   不计入「新增红」违约——真实缺失行为红）:
 *     - TEST 7 tooth 1（THINK/TALK bubble 现值 NULL——「思考中」/「播报」标头
 *       文案未落 kidpal_voice_states.c 闭集; LISTEN 同齿为既有零偏移复核, 现状绿）;
 *     - TEST 7 tooth 2（apply_look 的 bubble 写点仍挂 g_kidpal_chat_label 旧挂点 +
 *       voice_page 直接子件仅 3 件、标头对象未新增——现 3 < 4）;
 *     - TEST 7 tooth 3（echo_box 创建之前 voice_page 子件仅 pose 1 件——标头不在
 *       回显行上方, 「回显在『我在听！』标识下方」未落成）。
 *   义务红消除判据 = TEST 7 全齿绿（三态 bubble 闭集值在世 / apply_look 旧挂点
 *   写点清零且 look.bubble 单源写点仍经新标头 / 标头同父且创建序在 echo_box 之前）;
 *   suite 终跑判读同 VQ-A8 段口径（相对有效基线零新增红 + 本包齿全绿, r2 增补齿
 *   并入本包齿集）。锚漂移防护同上: apply_look 尾锚（kidpal_chrome_mid_sync_
 *   locked）与 voice_page/echo_box 创建行锚漂移以 RED(infra) 报出, 返 acceptance-
 *   author 复核, 不得由 builder/fixer 改齿。
 * ---------------------------------------------------------------------------
 * VQ-A7 真机证据要求（本文件不承载, human 持板轮; enforce-scope 拦 changes/dev
 * 写入——本段由 planner 转写进 delivery/changes/dev/260915-m1-k3-voice-round-ui/）:
 *   ① 思考页真机首次可弹（说完一句即弹、与状态切换同刻, 不分离）;
 *   ② 聆听回显三手感: 逐句回显在「我在听！」下方 / 滚动 / 超屏高滚出;
 *   ③ 问答框长文滚动: 答案全文上屏、超出屏高滚动看完（NFR-READ-1 字号下限观感）;
 *   ④ 打断手感: 播报中 KEY 单击立即停播并聆听（100% 可靠）; KEY 长按 / BOOT =
 *      结束会话回原页。
 *   证据形态 = 模拟器截图（三态演出各态）+ 真机轮日志/照片, 落
 *   delivery/changes/dev/260915-m1-k3-voice-round-ui/。
 * ---------------------------------------------------------------------------
 * 范围注记（无齿条目）:
 *   VQ-E1 后半——语音喊停（AEC 调优）归二期零触碰: 本包无任何 AEC/VAD 实时喊停
 *   接缝齿（打断唯一手段 = KEY 单击经 [4] 分层裁决）; 二期落地时按 requirements
 *   §2.1 另行走包。
 *   VQ-E2——课表答复数据侧（备注列参与答复）归 M3 语音问课表数据包: 本包 assistant
 *   路由齿只锁渲染面（问答框口径）, 不引课表数据面。
 * ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-09-16  initial freeze (acceptance-author, TaskPackage
 *               260915-m1-k3-voice-round-ui)。
 *   2026-09-17  augment: device-round VQ-A7 r1 三处真机缺陷增补齿（TEST 6 单独成节,
 *               seam [8] 尾注「增补齿义务红形态」段同步; TTS_START 喂入时机 VQ-A1 /
 *               全量中文字体 VQ-A2/A3 / 可见面挂载+骨架反形态 VQ-A5; 既有齿 TEST 1-5
 *               零改动）（acceptance-author, planner 增补派单, 红基线模型: 增补齿 =
 *               设计态红, fixer 实现后转绿）。
 *   2026-09-17  augment: device-round VQ-A7 r2 渲染面偏差增补齿（TEST 7 单独成节,
 *               seam [9] + 尾注「增补齿义务红形态（r2）」段同步; 三态 bubble 闭集
 *               标头值 VQ-A2/A5 / 标头写点统一挂 voice_page 右内容区 VQ-A5 / 回显行
 *               挂标头下方 VQ-A2; 既有齿 TEST 1-6 零改动; TEST 4 对 THINK/TALK
 *               bubble 无断言面, 零冲突无须裁量——判读声明落 TEST 7 头注）
 *               （acceptance-author, planner 小修轮增补派单, 红基线模型同 r1: 增补齿
 *               = 设计态红, fixer 实现后转绿）。
 * --------------------------------------------------------------------------- */
