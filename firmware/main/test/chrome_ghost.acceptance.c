/*
 * Frozen acceptance tests — TaskPackage 261022-p3-record-chrome-ghost-fix
 * (subsystem: firmware/main/display（lcd_display 键帽裁决 kidpal_keybar_sync_locked
 *  过渡窗门控 + keybar 纯逻辑单源新增 scene 生命周期 / 过渡窗裁决缝 +
 *  kidpal_voice_states 场景生命周期 + kidpal_render_node 层栈收口）+
 *  firmware/main/application.cc（会话收尾沿 scene 复位挂点）+ mcp_server.cc
 *  （成功归档路径层栈收口）; rigor=lite; riskTier=medium; sourceType=regression;
 *  板端 chrome 鬼影族——缺陷①键帽鬼影闪烁 + ②成功路径鬼覆盖层吃按键)。
 *
 * Story 语义权威 = debt 条目 TECH-261022-p3-record-session-state-churn 缺陷①②
 * （docs/debt/tech/TECH-261022-p3-record-session-state-churn.md，KPLOG005 真机轮
 * 三源取证逐秒还原）+ reviewed plan（delivery/taskpackages/261022-p3-record-
 * chrome-ghost-fix/plan.md，architect /triage-change 2026-10-22 比例化 review +
 * human 触点①确认）。human 立项原话（2026-10-22）：「日志里边记录的状态乱跳，
 * 跟我在屏幕上看到的状态乱跳是一样的」。
 *
 * 机制事实（debt 已钉到代码）:
 *   缺陷①（全笔 12 次，含无语音会话场景）：键帽裁决 kidpal_keybar_sync_locked
 *   （lcd_display.cc:1353-1457）读渲染节点缓存的故事子态——层切换/语音事件触发的
 *   chrome 重同步跑在故事子态缓存更新之前，读到 -1（kidpal_render_node_story_substate
 *   在 front != LOCAL_PAGE 时恒 -1，含云端渲染夹钳 SERVER_PAGE 推入窗）掉进通用分支
 *   kidpal_keybar_slots(page, g_kidpal_keybar_scene, overlay_top)；而
 *   g_kidpal_keybar_scene 在语音会话结束后永不复位，通用分支（overlay 前台列）恒画出
 *   残留语音场景键帽（聆听残留=「说完啦/结束」、思考残留=「不等了/结束」）约
 *   300~500ms，页级键帽才盖回。两因叠加修法（plan 定准）：① 语音会话收尾沿复位
 *   keybar scene（scene 复位值在语音场景闭集外 -> 通用分支 overlay 列裁决落 -1 防御、
 *   绝不再产出残留键帽）；② 故事会话期（录音/整理/阅读子态在途）键帽裁决对 -1 过渡窗
 *   取最后已知子态（按冻结子态表裁决，绝不掉语音残留）。
 *   缺陷②（13:03:20 实证）：故事归档成功、story_list 已刷新后，云端渲染夹钳
 *   （begin_page 推 SERVER_PAGE，end_page 只释放锁不弹层）遗留覆盖层在层栈里逻辑上
 *   仍占前台——归档后首按 BOOT 被板级路由成 action=4(overlay_close) 而非列表页动作，
 *   dismiss_front_overlay 后才恢复。修法（plan 定准）：成功归档路径补显式覆盖层
 *   dismiss，对齐 4068dc6c 失败路径形态——**帧类型负锚**：复用既有层栈收口通道
 *   （kidpal_render_node_story_local_front / kidpal_render_node_dismiss_front_overlay
 *   二者之一，均既有符号），不得新发明下行帧类型（新帧 = 触契约须转 /arch-change）。
 *
 * Nature: 行为级（纯逻辑决策函数 + weak 哨兵：keybar.c 未落新缝前哨兵恒返回 -99 /
 * NULL = 「冻结契约未实现」设计态红，不是编译错——diag_log / story_page_v2 先例同款；
 * 强符号 home = firmware/main/display/keybar.c，该源已在 host runner 与设备树两侧
 * 在册，builder 落新函数后零新增接线即覆盖哨兵）+ 结构级（真实工作树源锚扫描，
 * diag_log TEST 4/5 先例形态；正向断言只认**注释剥离后可执行源**的字面形态——
 * 260917-m2-k3 sealing pass 教训：只出现在注释里的锚值会假绿）。
 *
 * ===========================================================================
 * Seam contract pinned by this file（builder implements；home = 冻结落点
 * firmware/main/display/keybar.{h,c}——纯逻辑、零芯片依赖、零 ESP-IDF include、
 * 零 FreeRTOS / 零 LVGL -> host_app linux target 与设备树两侧同源；keybar.c 两侧
 * 已在册（host_app/main/CMakeLists.txt「../../../display/keybar.c」+
 * firmware/main/CMakeLists.txt「display/keybar.c」），本包零新增实现接线，
 * builder 只 additive 尾部追加函数 + 头声明 + 设备侧消费改造）:
 *
 *   [CG-C1] 语音场景生命周期单源（CG-A1；home = keybar.c + keybar.h 声明。
 *        现状 g_kidpal_keybar_scene 是 lcd_display.cc 文件级 static、设备侧独占，
 *        host 不可达 -> 场景状态上收 keybar 纯逻辑单源，lcd_display.cc 经 include
 *        消费——keybar.c 既有 dots/slots/char_slots/story_slots 单源同款架构）:
 *
 *          void kidpal_keybar_scene_set(int scene);
 *            -- 语音会话场景写沿（消费点 = lcd_display.cc kidpal_voice_apply_look
 *               现有 g_kidpal_keybar_scene = scene 写点，逐字替换）。
 *          int kidpal_keybar_scene_get(void);
 *            -- 键帽裁决 / chrome 中部显隐读沿（消费点 = kidpal_keybar_sync_locked
 *               通用分支入参与 kidpal_chrome_mid_sync_locked 判据，逐字替换）。
 *          void kidpal_keybar_scene_reset(void);
 *            -- 语音会话收尾沿复位（CG-A1 载体；消费点 = 会话收尾沿，见 [CG-C3]②）。
 *          冻结语义:
 *            - set 存真值（任意 int 原样存储，get 原样返回）;
 *            - reset 后 get() 必须**落在语音场景闭集之外**（< KVST_SCENE_LISTEN 或
 *              > KVST_SCENE_LOWBATT）——复位值是「非语音场景默认」：带该值走通用
 *              分支（overlay != 0）仲裁，kidpal_keybar_slots 落 scene 越界 -1 防御
 *              -> 残留语音键帽（「说完啦/结束」「不等了/结束」）绝不再被画出;
 *            - reset 幂等（连调多次语义不变）；
 *            - 复位后 set 任意合法场景 = 下一轮语音会话照常驱动覆盖层键帽
 *              （修复不得误伤真实语音轮键帽）。
 *            （实现自由：初始静态值 = 复位值同语义、存取加锁与否 = builder 裁量，
 *              与 g_story_* 既有跨任务宽松度同级。）
 *
 *   [CG-C2] story -1 过渡窗键帽裁决（CG-A2；home = keybar.c + keybar.h 声明。
 *        「最后已知子态」由调用方注入——纯函数无隐藏状态，同入参恒同出）:
 *
 *          int kidpal_keybar_story_transition_slots(int story_sub, int last_known_sub,
 *                                                   int rec_sub,
 *                                                   const char **out_left,
 *                                                   const char **out_right);
 *            -- 返回值三态:
 *               -1 : 非法入参（out_left/out_right 任一 NULL）;
 *               0  : 回落既有路径（out_* 不触碰）——非过渡窗（story_sub != -1，
 *                      调用方走既有 story 子态表 / 查字窗 / 通用分支）或过渡窗但
 *                      无在途子态（last_known_sub 不在 {KSTORY_PAGE_READER,
 *                      KSTORY_PAGE_RECORDING, KSTORY_PAGE_ORGANIZING}——LIST/STAMP
 *                      等非在途子态过渡窗按 plan 范围不接管，该场景鬼影由 [CG-C1]
 *                      scene 复位兜底）;
 *               1  : 过渡窗裁决命中（story_sub == -1 且 last_known_sub 在途）——
 *                      按最后已知子态出键帽，值域**零偏移**复用既有冻结单源:
 *                      READER     -> kidpal_keybar_story_slots(KSTORY_PAGE_READER)
 *                                    =（「长按退出」「翻屏」）;
 *                      RECORDING  -> kidpal_keybar_story_rec_slots(rec_sub) 帧分叉
 *                                    （rec_sub==0 帧 A「收尾/丢弃」; !=0 帧 B
 *                                    「打断」/右留空）;
 *                      ORGANIZING -> kidpal_keybar_story_slots(KSTORY_PAGE_ORGANIZING)
 *                                    =（"" ""双槽留空）;
 *                      实现义务 = 内部经 kidpal_keybar_story_slots /
 *                      kidpal_keybar_story_rec_slots 消费（禁重抄键帽字面——KB-E2
 *                      闭集/子态字面单源红线）。
 *
 *   [CG-C3] 设备侧接线（chip 面，host 不可达，只由结构断言扫源锚——diag_log TEST 4
 *        先例形态。负向锚 = 注释剥离后可执行源计数，正向锚只认可执行源字面）:
 *
 *     ① lcd_display.cc 场景单源改造:
 *        - g_kidpal_keybar_scene（旧设备侧 static 场景态）**可执行源零残留**
 *          （负向锚：状态上收 keybar 单源后设备侧不得留第二真相——同窗双写 = 谁生效
 *          说不清）;
 *        - kidpal_voice_apply_look 区内经 kidpal_keybar_scene_set( 写场景（正向锚）;
 *        - kidpal_keybar_sync_locked 区内经 kidpal_keybar_scene_get( 读场景（正向锚，
 *          全文件 get( 计数 >= 2——sync 通用分支入参 + chrome 中部显隐判据两消费点）;
 *        - 既有锚在守（不得删）：sync 区内 kidpal_render_node_story_substate( /
 *          kidpal_keybar_slots( / kidpal_keybar_story_slots( /
 *          kidpal_keybar_story_rec_slots( / kidpal_keybar_char_slots( 五既有裁决消费。
 *     ② 过渡窗裁决接线（缺陷① 修法②）: kidpal_keybar_sync_locked 区内消费
 *        kidpal_keybar_story_transition_slots(（CG-C2 缝；「最后已知子态」追踪器
 *        归属 = builder 实现自由——sync 内 static 或渲染节点缓存补缝均可，行为
 *        本体已由 [CG-C2] 纯函数全承载，见强度申报 GAP-1）。
 *     ③ 会话收尾沿 scene 复位（缺陷① 修法①）: kidpal_keybar_scene_reset( 在世于
 *        会话收尾沿——application.cc HandleStateChangedEvent Idle 分支（既有
 *        kidpal_story_session_ctx_set(0) 同区，RCL-F1 收尾沿先例）**或**
 *        kidpal_render_node.cc kidpal_render_node_voice_end（每轮语音会话收尾恒经，
 *        1g 预评裁量择一；二处任一命中即绿）。
 *     ④ 成功归档路径层栈收口（缺陷②; mcp_server.cc，4068dc6c 失败路径
 *        dismiss_front_overlay 先例同款纪律——夹钳后必须回落）:
 *        - story_list 前推渲染闭包（app.Schedule([view_snapshot, list_snapshot]()
 *          区内）: kidpal_render_node_begin_page( -> kidpal_render_node_end_page(
 *          -> 层栈收口（kidpal_render_node_story_local_front( 或
 *          kidpal_render_node_dismiss_front_overlay(，**只许此二既有通道**——帧类型
 *          负锚的机器形态：收口通道 ∈ 既有符号闭集，绝无新发明下行帧类型/新缝符号）;
 *        - 盖章帧渲染闭包（app.Schedule([stamp_view, stamp_list, stamp_text]() 区内）:
 *          同上序（begin_page -> end_page -> 既有通道收口）;
 *        - 既有锚在守：ScheduleStoryRender 区内 begin_page -> end_page ->
 *          kidpal_render_node_story_local_front( 既有回落形态不得被删（在役先例）。
 *   [CG-C4] keybar.h 声明在守: 四新缝符号（kidpal_keybar_scene_set /
 *        kidpal_keybar_scene_get / kidpal_keybar_scene_reset /
 *        kidpal_keybar_story_transition_slots）声明住 keybar.h（设备侧经 include
 *        消费的接线前提; additive 尾部追加，既有声明零触碰）。
 *   [CG-C5] 登记（anti-vacuity）: host_app/main/CMakeLists.txt HOST_ACCEPTANCE_SRCS
 *        登记 "../../chrome_ghost.acceptance.c"（置于 "../../host_infra_smoke.
 *        acceptance.c" 之前——smoke 末位 = 最先执行，包 0 boundary 注册序守卫；
 *        **builder FIRST ACTION**，登记行不在 acceptance-author 写权内——diag_log
 *        261022 先例口径）。实现源 keybar.c / kidpal_voice_states.c / story_page.c
 *        与回归网成员（keybar_three_slot / story_page_v2 / key_layer_dispatch 三冻结
 *        件）继续在册、不被本包登记改动静默摘除；设备侧 firmware/main/CMakeLists.txt
 *        display/keybar.c 在册守（本包零新增设备登记）。
 * ===========================================================================
 * 契约 / 决策条目 ↔ 验收断言 的强度申报（acceptance-evidence「契约条目 ↔ 齿」节）:
 *
 * | 契约 / 决策条目 | 承载断言 | 性质档 |
 * |---|---|---|
 * | CG-A1 缺陷①修法①（语音会话收尾沿 scene 复位 -> 通用分支不再产出残留键帽） | TEST 1（set/get 存真 + 复位值出闭集 + 复位幂等 + 复位后通用分支 overlay 列仲裁绝不产出「说完啦/不等了」残留 + 下一轮 set 照常驱动真实语音键帽）+ TEST 3（apply_look 写沿 / sync·mid 读沿 / 收尾沿复位接线源锚 + g_kidpal_keybar_scene 零残留负锚） | 行为级（纯逻辑）+ 源锚代理（接线） |
 * | CG-A2 缺陷①修法②（story 会话期 -1 过渡窗取最后已知子态、绝不画语音残留） | TEST 2（三态决策表：非过渡窗回落 / 在途三子态逐值 == 既有冻结子态表零偏移（含 RECORDING 帧分叉 rec_sub）/ 非在途回落 + 全域语音键帽负扫描 + NULL 防御）+ TEST 3（sync 区消费接线源锚 + keybar.h 声明守） | 行为级（纯逻辑）+ 源锚代理（接线） |
 * | CG-A3 缺陷②（成功归档层栈收口——BOOT 成列表动作非 overlay_close） | TEST 4（story_list 前推闭包 + 盖章闭包两处 begin_page->end_page->既有通道收口序锚 + 收口通道 ∈ {story_local_front, dismiss_front_overlay} 既有符号闭集 = 帧类型负锚机器形态 + ScheduleStoryRender 在役先例守） | 源锚代理（机制级锚点；**不等于运行时观察**——层栈收口的真机半归 CG-A5） |
 * | CG-A4 回归网（三族全绿 + host 全量零新增红 vs 基线 227T3F） | TEST 5（本件登记 + 注册序 + 三冻结件与三实现源登记守 + 设备侧 keybar.c 在册守）+ 红基线实跑对照（运行面承载，非本文件断言） | 源锚代理（登记守）+ 实跑记录 |
 * | CG-A5 真机目检（无鬼影双闪 / 归档后首按 BOOT 生效） | **live-manual，无自动断言**（kind=structural，DLG-A6 先例同款口径，不计入 runner 通过面） | 真机观察（human 持板） |
 *
 * 弱于契约处（显式缺口，不得当已兑现）:
 *   GAP-1（最后已知子态追踪器）：[CG-C2] 纯函数以入参注入 last_known_sub，行为
 *         本体全承载；设备侧追踪器（谁在何时记录/更新最后已知子态）host 不可达，
 *         builder 实现自由 -> 其正确性由 TEST 3 接线源锚（sync 区消费）+ 真机轮
 *         CG-A5 ①（无鬼影双闪）承载。追踪器漏更新 = 过渡窗退回回落路径，TEST 3
 *         结构锚不红——显式申报，不由结构断言伪装承载。
 *   GAP-2（层栈收口运行时半）：TEST 4 只证明源码具备「成功归档两闭包 + 既有通道
 *         收口」的接线形状，**不证明**设备上归档后层栈真的无 SERVER_PAGE 残留、
 *         BOOT 真的路由成列表动作——真机轮 CG-A5 ②承载。
 *   GAP-3（复位沿覆盖面）：[CG-C1] 只钉「语音会话收尾沿复位」（plan 范围）；提醒
 *         弹窗（REMIND）dismiss 后的 scene 残留不在 debt ①② 观测集与 plan 范围内，
 *         本包不钉（若真机轮 CG-A5 观测到「知道了/关闭」残留，另起 debt 条目）。
 *   GAP-4（过渡窗在途集边界）：[CG-C2] 在途子态集 = plan 逐字 {READER, RECORDING,
 *         ORGANIZING}；LIST/STAMP/BODY_UNAVAILABLE/DELETE 相位的过渡窗不接管
 *         （plan 修法②逐字范围）——该相位的键帽残留由 [CG-C1] scene 复位兜底，
 *         不在本缝职责内。
 *   GAP-5（双同步不修）：plan「单事件双同步若经 1g 预评评估为良性可留，只修前两因
 *         亦可接受」——本件不断言同步次数（双闪节奏归真机 CG-A5 ①观感终判）。
 *
 * 行为等价假设（显式；不写即默认不成立）:
 *   EQ-1「weak 哨兵被强符号覆盖 = 纯逻辑接线落地」：本文件 standalone 红跑链接
 *        keybar.c / kidpal_voice_states.c（既有强符号在世），新缝哨兵恒红 = 设计态红；
 *        host runner 中 keybar.c 必须在 HOST_ACCEPTANCE_SRCS 在世（TEST 5 登记守
 *        校验），builder 落新函数后哨兵被覆盖转绿。
 *   EQ-2「过渡窗键帽 == 既有冻结子态表」：TEST 2 逐值断言与 kidpal_keybar_story_
 *        slots / story_rec_slots 同参产物相等——过渡窗裁决不得自抄键帽字面
 *        （KB-E2 单源红线），改字面 = 两处同时红。
 *   EQ-3「scene 复位值出闭集 ≙ 通用分支不画残留」：依据 = kidpal_keybar_slots
 *        冻结防御（scene 越界 -> -1，keybar_three_slot TEST 2 防御列承载）——复位值
 *        出闭集则通用分支 overlay 列仲裁落 -1 防御、labels 不被残留键帽覆写。
 *   EQ-4「源锚 = 运行时行为」：TEST 3/4/5 的结构锚不证明设备运行时表现
 *         （同 GAP-1/GAP-2）。
 *
 * 覆盖标记（coverage marker）随 TEST_CASE 名称与首行注释逐条给出。
 *
 * Run protocol（host runner, C4.4 惯例）:
 *   0) builder FIRST ACTION：向 firmware/main/test/host_app/main/CMakeLists.txt 的
 *      HOST_ACCEPTANCE_SRCS 追加 "../../chrome_ghost.acceptance.c"（置于
 *      host_infra_smoke 之前；keybar.c / kidpal_voice_states.c 既有在册零新增；
 *      设备侧 firmware/main/CMakeLists.txt display/keybar.c 既有在册零新增）。
 *   1) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
 *      espidf_wsl.sh build
 *   2) MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 --
 *      firmware/main/test/host_app/build/host_infra_test.elf
 *   本文件亦可独立冒烟（冻结相位实编 + 设计态红取证，diag_log 通道 B 同款）:
 *      cd firmware/main/test/acceptance/red-baseline
 *      gcc -std=c11 -Wall -Wextra -I. -o /tmp/cg_red_runner \
 *          ../../chrome_ghost.acceptance.c ../../display/keybar.c \
 *          ../../display/kidpal_voice_states.c runner.c
 *      cd <repo root>; /tmp/cg_red_runner   # exit code = 失败用例数
 *   （独立通道链接既有强符号 keybar.c / kidpal_voice_states.c——既有冻结行为在
 *      standalone 通道保持绿、只让新缝哨兵红；避免对既有 keybar 符号重复 weak 定义。）
 *   结构扫描类断言支持 KIDPAL_REPO_ROOT 显式注入仓根（缺省走 walk-up 探测，仓根 /
 *   host_app / red-baseline 三处均可跑；acceptance-evidence「root 注入」纪律）。
 *
 * RED baseline（冻结相位形状；冻结落盘时实跑，逐条红名见交付报告与下方实跑记录）:
 *   设计态（净工作树：keybar.{h,c} 无 [CG-C1]/[CG-C2] 新缝、lcd_display.cc 场景态
 *   仍是设备侧 static、五处接线锚未落、mcp_server.cc 两闭包无收口、host CMakeLists
 *   未登记本件——本验收件登记同样归 builder FIRST ACTION）——预期 5 个 TEST_CASE
 *   全红，红因逐条对应当前 story 缺失的真实行为：
 *     TEST 1  -> CG-A1 scene 生命周期 seam 未实现（weak 哨兵 set no-op / get -99）
 *     TEST 2  -> CG-A2 过渡窗裁决 seam 未实现（weak 哨兵返回 -99）
 *     TEST 3  -> CG-C3 接线锚缺席（①场景单源改造 / ②过渡窗消费 / ③收尾沿复位 /
 *                keybar.h 声明守）+ g_kidpal_keybar_scene 7 处在世（负锚）
 *     TEST 4  -> CG-C3 ④成功归档两闭包无层栈收口（13:03:20 实证形态）
 *     TEST 5  -> CG-C5 登记守（chrome_ghost.acceptance.c 未登记——builder FIRST ACTION）
 *   既有 host 全量套件零新增红（先存红 = p1_auxbar AB-A1/A2/A4，锚 227T3F@0df2c0c3）。
 *
 *   冻结落盘实跑（2026-10-22，双通道）:
 *   〔A〕host 全量 runner（工作树、本件尚未登记进 runner——261018/261022-diag 先例
 *        同口径）: `227 Tests 3 Failures`（exit 3）——与已知基线（锚 0df2c0c3）完全
 *        一致：3 条全为既有 p1_auxbar_style.acceptance.c 的 AB-A1 / AB-A2 / AB-A4
 *        先存红，与本包正交；本包零新增红（本件尚未编入 runner，运行面零扰动）。
 *   〔B〕独立冒烟通道（red-baseline runner.c + 既有强符号 keybar.c /
 *        kidpal_voice_states.c, COMPILE_EXIT=0 零 warning）:
 *        `5 Tests 5 Failures`（RUN_EXIT=5），红名 verbatim：
 *          TEST 1 -> CG-A1「冻结 seam 未实现（weak 哨兵 kidpal_keybar_scene_get
 *                    返回 -99）或 set 未存真值」
 *          TEST 2 -> CG-A2「frozen seam NOT implemented (weak sentinel -99) —
 *                    kidpal_keybar_story_transition_slots」
 *          TEST 3 -> CG-A2「kidpal_keybar_sync_locked 未消费 kidpal_keybar_story_
 *                    transition_slots(」（其前的扫描器自检 / 五既有裁决消费 /
 *                    场景区锚等须绿守卫齿全绿后首红即本条）
 *          TEST 4 -> CG-A3「story_list 前推渲染闭包 end_page 后无层栈收口」
 *                    （ScheduleStoryRender 在役先例守先绿）
 *          TEST 5 -> CG-A4「host_app/main/CMakeLists.txt 未登记 chrome_ghost.
 *                    acceptance.c」（回归网三冻结件在守齿先绿）
 *        全部为设计态红（冻结 seam 未实现 / 接线锚未落 / 登记缺位），逐条对应
 *        当前 story 缺失的真实行为。
 *   双向可满足性（/mnt/c/tmp/cg_work 合成树 KIDPAL_REPO_ROOT 注入 + 一次性参考
 *   实现，均不写入工作树）: 绿树 `5 Tests 0 Failures`（COMPILE 零 warning）；七处
 *   定向变异各自只打红对应 TEST_CASE（每处重建净树 + 单变异 + 重编重跑）——
 *   scene 复位值入语音闭集 -> TEST 1 红 / 过渡窗裁决产出语音残留键帽 -> TEST 2 红 /
 *   摘过渡窗接线 -> TEST 3 红 / 场景 static 第二真相源回归 -> TEST 3 红 /
 *   盖章闭包摘收口 -> TEST 4 红 / 摘登记 -> TEST 5 红 / 摘会话收尾沿复位 ->
 *   TEST 3 红——断言非空过。
 *   转绿判据：builder 落地 [CG-C1]/[CG-C2] 新缝 + [CG-C3] 接线 + [CG-C5] 登记
 *   （FIRST ACTION）后全绿；任一仍红而对应源锚已落地 = 锚漂移，复核本文件锚，
 *   不得改判为绿。既有 host 全量套件必须零新增红。
 * ---------------------------------------------------------------------------
 * Coverage map (acceptanceRef -> test):
 *   coverage: CG-A1 -- scene 生命周期行为：set/get 存真往返 / reset 后值出语音场景
 *             闭集（非语音场景默认）/ reset 幂等 / reset 非 no-op / 复位后通用分支
 *             overlay 列仲裁绝不产出「说完啦/结束」「不等了/结束」残留键帽对 /
 *             复位后 set 恢复真实语音轮键帽（修复不误伤）（TEST 1）+ 设备侧接线
 *             （apply_look 写沿 / sync+mid 读沿 / 收尾沿复位 / g_kidpal_keybar_scene
 *             零残留负锚 / keybar.h 声明守）（TEST 3）
 *   coverage: CG-A2 -- 过渡窗裁决行为：三态返回语义（-1 非法 / 0 回落不触碰 out /
 *             1 命中）/ 非过渡窗（story_sub 有效）恒回落 / 在途三子态（READER/
 *             RECORDING 帧 A 帧 B/ORGANIZING）逐值与既有冻结子态表零偏移相等 /
 *             非在途（LIST/STAMP/DELETE/BODY/-1）回落 / 全域左槽绝不为「说完啦」
 *             「不等了」（语音残留负扫描）/ NULL out 防御（TEST 2）+ sync 区消费
 *             接线源锚 + keybar.h 声明守（TEST 3）
 *   coverage: CG-A3 -- 成功归档层栈收口（结构）：story_list 前推闭包与盖章闭包两处
 *             begin_page -> end_page -> 既有通道（story_local_front |
 *             dismiss_front_overlay）收口序锚 + ScheduleStoryRender 在役先例守
 *             （TEST 4）；BOOT 路由成列表动作的运行时半 = 真机轮 CG-A5 ②
 *             （live-manual，不计入 runner 通过面）
 *   coverage: CG-A4 -- 回归网：本件登记进 HOST_ACCEPTANCE_SRCS（builder FIRST
 *             ACTION）+ 注册序（smoke 之前）+ 三冻结件（keybar_three_slot /
 *             story_page_v2 / key_layer_dispatch）登记在守（anti-vacuity）+ 三实现
 *             源（keybar.c / kidpal_voice_states.c / story_page.c）在册守 + 设备侧
 *             display/keybar.c 在册守（TEST 5）+ host 全量实跑对照 227T3F
 *             （先存红 = p1_auxbar AB-A1/A2/A4，锚 0df2c0c3），零新增意外红
 *   coverage: CG-A5 -- **live-manual 真机目检项，不落自动断言、不计入 runner
 *             通过面**（TaskPackage 标 kind=structural；DLG-A6 先例同款口径）。
 *             human 持板：① 翻阅读卡与记录会话全程键帽无鬼影双闪（含无语音会话的
 *             开卡/翻屏场景）；② 记录会话归档成功后首按 BOOT 即生效（不再被
 *             overlay_close 白吃）；③ 与 diag-log-content 收尾真机轮合并执行省烧板。
 *             本 Pod 到真机门前停等（status=awaiting-human:real-device），不自行烧录。
 * --------------------------------------------------------------------------- */

#include <unity.h>

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* 稳定在世底座（键枚举 / 语音场景闭集 / story 子态枚举 / keybar 冻结缝声明——
 * keybar.h 传递包含 kidpal_voice_states.h 与 story_page.h，纯声明头 host 可编）。 */
#include "../display/keybar.h"

/* -------------------------------------------------------------------------- *
 * 未实现哨兵：永不是整型 seam 的合法返回值（diag_log DLG_UNIMPLEMENTED 同款纪律；
 * kidpal_keybar_story_transition_slots 合法返回值域 = {-1, 0, 1}）。指针 seam 的
 * 哨兵 = NULL。强定义 home = display/keybar.c（既有在册源 additive 尾部追加），
 * host runner / 独立通道均被强符号覆盖。
 * ------------------------------------------------------------------------ */
#define CG_UNIMPLEMENTED (-99)

/* [CG-C2] 返回值三态语义闭包（与 keybar.h 落地声明同值——EQ-2 单源断言载体）。 */
#define CG_TRANS_FALLBACK  0 /* 回落既有路径（out_* 不触碰） */
#define CG_TRANS_RESOLVED  1 /* 过渡窗裁决命中（按最后已知子态出键帽） */

/* -------------------------------------------------------------------------- *
 * [CG-C1]/[CG-C2] 新缝声明（与 keybar.h 落地声明逐字同型；本文件先于头落地——
 * diag_log 先例：本文件不 include 尚不存在的声明区，落地后头声明与本处同型，
 * TEST 3 声明守把等形钉死）。
 * ------------------------------------------------------------------------ */
void kidpal_keybar_scene_set(int scene);
int kidpal_keybar_scene_get(void);
void kidpal_keybar_scene_reset(void);
int kidpal_keybar_story_transition_slots(int story_sub, int last_known_sub,
                                         int rec_sub, const char **out_left,
                                         const char **out_right);

/* -------------------------------------------------------------------------- *
 * Weak 哨兵（home = firmware/main/display/keybar.c additive 尾部；强定义覆盖）。
 * 未落地态 = 设计态红，不是编译错（EQ-1）。
 * ------------------------------------------------------------------------ */

__attribute__((weak)) void kidpal_keybar_scene_set(int scene)
{ (void)scene; }
__attribute__((weak)) int kidpal_keybar_scene_get(void)
{ return CG_UNIMPLEMENTED; }
__attribute__((weak)) void kidpal_keybar_scene_reset(void)
{}
__attribute__((weak)) int kidpal_keybar_story_transition_slots(int story_sub,
                                                               int last_known_sub,
                                                               int rec_sub,
                                                               const char **out_left,
                                                               const char **out_right)
{
    (void)story_sub; (void)last_known_sub; (void)rec_sub;
    (void)out_left; (void)out_right;
    return CG_UNIMPLEMENTED;
}

/* -------------------------------------------------------------------------- *
 * 工具：失败消息 / 仓根定位 / 文件读取 / 注释剥离 / 窗内搜索（自包含复制自
 * sd_log.acceptance.c / diag_log.acceptance.c 的助手族，命名前缀 cg_；全 static，
 * 不与先例符号冲突）。
 * ------------------------------------------------------------------------ */

#define CG_MAX_PATH 1024
#define CG_MAX_READ (8u * 1024u * 1024u)

static char cg_fm_buf[1024];
static const char *cg_fm(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
static const char *cg_fm(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(cg_fm_buf, sizeof(cg_fm_buf), fmt, ap);
    va_end(ap);
    return cg_fm_buf;
}

static int cg_is_regular(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static char cg_root[CG_MAX_PATH];
static int cg_root_ready;

/* 仓根定位（acceptance-evidence「root 注入」纪律）：优先显式环境变量
 * KIDPAL_REPO_ROOT，其次由相对候选点探测——不依赖某个调用 cwd 的绝对路径假设。 */
static int cg_locate_root(void)
{
    static const char *const cands[] = {
        ".", "..", "../..", "../../..", "../../../..", "../../../../..",
        "../../../../../.."
    };
    char probe[CG_MAX_PATH];
    const char *env_root;
    size_t i;

    if (cg_root_ready) {
        return 1;
    }
    env_root = getenv("KIDPAL_REPO_ROOT");
    if (env_root != NULL && env_root[0] != '\0') {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", env_root);
        if (cg_is_regular(probe)) {
            snprintf(cg_root, sizeof(cg_root), "%s", env_root);
            cg_root_ready = 1;
            return 1;
        }
    }
    for (i = 0; i < sizeof(cands) / sizeof(cands[0]); i++) {
        snprintf(probe, sizeof(probe), "%s/firmware/main/CMakeLists.txt", cands[i]);
        if (cg_is_regular(probe)) {
            snprintf(cg_root, sizeof(cg_root), "%s", cands[i]);
            cg_root_ready = 1;
            return 1;
        }
    }
    return 0;
}

static int cg_rel(char *out, size_t cap, const char *rel)
{
    if (out == NULL || rel == NULL) {
        return 0;
    }
    if (!cg_locate_root()) {
        return 0;
    }
    snprintf(out, cap, "%s/%s", cg_root, rel);
    return 1;
}

static char *cg_read(const char *rel, size_t *out_len)
{
    char p[CG_MAX_PATH];
    FILE *f;
    long sz;
    char *buf;

    if (!cg_rel(p, sizeof(p), rel)) {
        return NULL;
    }
    f = fopen(p, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0 || (sz = ftell(f)) < 0 ||
        (unsigned long)sz > CG_MAX_READ || fseek(f, 0, SEEK_SET) != 0) {
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

/* 注释剥离（260917-m2-k3 sealing pass / sd_log / diag_log 同款）：正向断言只被
 * **可执行源**满足——注释字节抹成空格；字符串 / 字符字面量体保留（不把字面量里的
 * // 误判为注释）。保持缓冲长度，调用方负责 free。 */
static char *cg_strip_c_comments(const char *src)
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

static char *cg_strip_file(const char *rel)
{
    char *buf = cg_read(rel, NULL);
    char *out;

    if (buf == NULL) {
        return NULL;
    }
    out = cg_strip_c_comments(buf);
    free(buf);
    return out;
}

/* 有界子串搜索（窗内 needle 在场判定）。未命中 -> NULL。 */
static const char *cg_find_in(const char *hay, const char *end, const char *needle)
{
    size_t nl;

    if (hay == NULL || needle == NULL || end == NULL || hay >= end) {
        return NULL;
    }
    nl = strlen(needle);
    if (nl == 0) {
        return hay;
    }
    for (; hay + nl <= end; hay++) {
        if (hay[0] == needle[0] && memcmp(hay, needle, nl) == 0) {
            return hay;
        }
    }
    return NULL;
}

/* 裸源子串出现计数（负向断言专用；调用方传入已剥注释窗时天然只计可执行源）。 */
static int cg_count_sub(const char *hay, const char *needle)
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

/* 函数窗终点：起点 + win 字节，钳制到缓冲末尾。 */
static const char *cg_win_end(const char *start, size_t win)
{
    const char *end = start + win;
    size_t total = strlen(start);

    if ((size_t)(end - start) > total) {
        end = start + total;
    }
    return end;
}

/* 定义点定位：签名 needle 的多处命中（前向声明 vs 定义）中取**其后首个非空白字符
 * 为 '{'** 的那一处——工作树为 CRLF 行尾、声明与定义同签名文本并存（lcd_display.cc
 * sync_locked :488 前向声明 / :1353 定义实证），纯 strstr 会先命中声明。 */
static const char *cg_find_def(const char *hay, const char *needle)
{
    size_t nl = strlen(needle);
    const char *p = hay;

    if (hay == NULL || needle == NULL || nl == 0) {
        return NULL;
    }
    while ((p = strstr(p, needle)) != NULL) {
        const char *q = p + nl;
        while (*q == '\r' || *q == '\n' || *q == ' ' || *q == '\t') {
            q++;
        }
        if (*q == '{') {
            return p;
        }
        p++;
    }
    return NULL;
}

/* 区域切取：文件内 start_needle 首现起至（exclusive）其后首个 end_needle（end 为
 * NULL 或其后无命中 -> 切到 EOF）。调用方 free。NULL = 切取失败（锚漂移/缺失）。 */
static char *cg_region_between(const char *rel, const char *start_needle,
                               const char *end_needle)
{
    char *buf = cg_read(rel, NULL);
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
    if (end_needle == NULL ||
        (e = strstr(s + strlen(start_needle), end_needle)) == NULL) {
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

/* 全序判定：区域内 a、b 依次在场（a 先于 b）。 */
static int cg_order_in(const char *region, const char *end, const char *a,
                       const char *b)
{
    const char *pa = cg_find_in(region, end, a);
    const char *pb;

    if (pa == NULL) {
        return 0;
    }
    pb = cg_find_in(pa + strlen(a), end, b);
    return pb != NULL;
}

#define CG_REQUIRE_ROOT() TEST_ASSERT_TRUE_MESSAGE(cg_locate_root(), \
    "CG RED(infra): repo root not located from cwd — run from the repo root, " \
    "firmware/main/test/host_app or firmware/main/test/acceptance/red-baseline, " \
    "or pass KIDPAL_REPO_ROOT (see acceptance-evidence root injection rule)")

/* 哨兵必查（diag_log DLG_REQUIRE_IMPL 同款；红因 = 冻结 seam 未实现）。 */
static void cg_require_impl(int value, const char *what)
{
    if (value == CG_UNIMPLEMENTED) {
        char msg[512];
        snprintf(msg, sizeof(msg),
                 "CG RED: frozen seam NOT implemented (weak sentinel -99) — %s；"
                 "builder 落地 display/keybar.{h,c} 新缝（本文件头注 [CG-C1]/[CG-C2]）"
                 "并登记 chrome_ghost.acceptance.c 进 HOST_ACCEPTANCE_SRCS 后转绿", what);
        TEST_ASSERT_TRUE_MESSAGE(0, msg);
    }
}

/* 结构扫描面（源锚；相对仓根）。 */
#define CG_LCD_CC     "firmware/main/display/lcd_display.cc"
#define CG_RENDER_CC  "firmware/main/display/kidpal_render_node.cc"
#define CG_MCP_CC     "firmware/main/mcp_server.cc"
#define CG_APP_CC     "firmware/main/application.cc"
#define CG_KEYBAR_C   "firmware/main/display/keybar.c"
#define CG_KEYBAR_H   "firmware/main/display/keybar.h"
#define CG_MAIN_CMAKE "firmware/main/CMakeLists.txt"
#define CG_HOST_CMAKE "firmware/main/test/host_app/main/CMakeLists.txt"

/* 既有函数区锚（lcd_display.cc，keybar_three_slot TEST 4 tooth 7 先例同款序）。 */
#define CG_SYNC_FN   "static void kidpal_keybar_sync_locked(void)"
#define CG_APPLY_FN  "static void kidpal_voice_apply_look(int scene)"
#define CG_MID_FN    "static void kidpal_chrome_mid_sync_locked(void)"

/* 成功归档两闭包锚（mcp_server.cc 可执行源字面——C++ lambda 捕获列表）。 */
#define CG_STAMP_CLOSURE "app.Schedule([stamp_view, stamp_list, stamp_text]() {"
#define CG_LIST_CLOSURE  "app.Schedule([view_snapshot, list_snapshot]() {"
#define CG_SCHED_STORY   "void ScheduleStoryRender() {"

/* 既有层栈收口通道闭集（帧类型负锚的机器形态：收口只许此二既有符号）。 */
#define CG_FRONT_FN  "kidpal_render_node_story_local_front("
#define CG_DISMISS_F "kidpal_render_node_dismiss_front_overlay("

/* 闭包体窗（list 前推闭包 end_page @ +644B、盖章闭包 @ +335B 实测；窗取 1600
 * 覆盖 begin->render->end->收口且不吞后续 route 代码——下一 begin_page @ ~3.5KB）。 */
#define CG_CLOSURE_WINDOW 1600u
/* ScheduleStoryRender 窗（story_local_front @ +1729B 实测）。 */
#define CG_SCHED_WINDOW 2400u
/* sync / apply_look / voice_end 等函数窗。 */
#define CG_FN_WINDOW 8000u

/* ========================================================================== *
 * TEST 1 — CG-A1 语音场景生命周期（behavioral, seam [CG-C1]）。
 * set/get 存真往返 / reset 后值出语音场景闭集（非语音场景默认）/ reset 幂等 /
 * reset 非 no-op / 复位后通用分支（overlay 前台列）仲裁绝不产出「说完啦/结束」
 * 「不等了/结束」残留键帽对 / 复位后 set 恢复真实语音轮键帽（修复不误伤）。
 * RED today（设计态）: keybar.c 无 scene 生命周期 seam -> weak 哨兵 set no-op /
 *           get 恒 -99（往返齿 = 红载体；其后各齿在 unity 首败 abort 下不可达，
 *           绿态后全齿有效）。
 * ========================================================================== */
/* coverage: CG-A1 -- scene 生命周期行为（存真往返 / 复位出闭集 / 幂等 / 非 no-op /
 *           复位后通用分支不产出语音残留键帽对 / set 恢复真实语音键帽） */
TEST_CASE("chrome ghost: keybar scene lifecycle — session-end reset never redraws voice keycaps (CG-A1)",
          "[chrome_ghost][CG-A1][linux][behavior]")
{
    const char *left = NULL;
    const char *right = NULL;
    int s;
    int rc;

    /* ---- 存真往返（红载体：哨兵 get 恒 -99 != 存入值） ---------------------- */
    kidpal_keybar_scene_set(KVST_SCENE_THINK);
    TEST_ASSERT_EQUAL_INT_MESSAGE(KVST_SCENE_THINK, kidpal_keybar_scene_get(),
        "CG-A1 RED: 冻结 seam 未实现（weak 哨兵 kidpal_keybar_scene_get 返回 -99）或"
        "set 未存真值——builder 落地 display/keybar.c scene 生命周期缝（[CG-C1]）并"
        "登记本件进 HOST_ACCEPTANCE_SRCS 后转绿；契约见 chrome_ghost.acceptance.c 头注");
    kidpal_keybar_scene_set(KVST_SCENE_LISTEN);
    TEST_ASSERT_EQUAL_INT_MESSAGE(KVST_SCENE_LISTEN, kidpal_keybar_scene_get(),
        "CG-A1 RED: scene 写沿未存真值（残留=KPLOG005 聆听场景）——apply_look 场景"
        "写沿必须经单源 set 落真值");

    /* ---- 会话收尾沿复位：复位值出语音场景闭集（CG-A1「非语音场景默认」） ----- */
    kidpal_keybar_scene_reset();
    s = kidpal_keybar_scene_get();
    TEST_ASSERT_TRUE_MESSAGE(s < KVST_SCENE_LISTEN || s > KVST_SCENE_LOWBATT,
        cg_fm("CG-A1 RED: 会话收尾沿复位后 scene=%d 仍落在语音场景闭集 [%d..%d] 内"
              "——复位值必须出闭集（EQ-3：出闭集 -> 通用分支 overlay 列仲裁落 "
              "kidpal_keybar_slots scene 越界 -1 防御 -> 残留键帽绝不再被画出；"
              "复位到 0=KVST_SCENE_LISTEN 等于没修）",
              s, KVST_SCENE_LISTEN, KVST_SCENE_LOWBATT));

    /* ---- reset 非 no-op + 幂等 --------------------------------------------- */
    kidpal_keybar_scene_set(KVST_SCENE_THINK);
    kidpal_keybar_scene_reset();
    s = kidpal_keybar_scene_get();
    TEST_ASSERT_TRUE_MESSAGE(s != KVST_SCENE_THINK,
        "CG-A1 RED: reset 后 scene 未变（复位是 no-op）——g_kidpal_keybar_scene "
        "「永不复位」缺陷原样保留（KPLOG005 缺陷①未治）");
    kidpal_keybar_scene_reset();
    s = kidpal_keybar_scene_get();
    TEST_ASSERT_TRUE_MESSAGE(s < KVST_SCENE_LISTEN || s > KVST_SCENE_LOWBATT,
        "CG-A1 RED: reset 幂等性破坏（二次复位后落回语音场景闭集内）");

    /* ---- 复位后通用分支（overlay 前台列）仲裁绝不产出语音残留键帽对 --------- */
    s = kidpal_keybar_scene_get();
    rc = kidpal_keybar_slots(PAGE_P1, s, 1, &left, &right);
    TEST_ASSERT_TRUE_MESSAGE(!(rc == 0 && left != NULL &&
                               (strcmp(left, "说完啦") == 0 ||
                                strcmp(left, "不等了") == 0)),
        cg_fm("CG-A1 RED: 复位后通用分支 overlay 列（rc=%d, 左=%s）仍产出语音残留"
              "键帽——「说完啦/不等了」鬼影未消除（KPLOG005 缺陷①本体；复位值出"
              "闭集时 kidpal_keybar_slots 必须落 scene 越界 -1 防御, EQ-3）",
              rc, left != NULL ? left : "(null)"));
    rc = kidpal_keybar_slots(PAGE_P3, s, 1, &left, &right);
    TEST_ASSERT_TRUE_MESSAGE(!(rc == 0 && left != NULL &&
                               (strcmp(left, "说完啦") == 0 ||
                                strcmp(left, "不等了") == 0)),
        "CG-A1 RED: P3 页同判失败——复位后任何页通用分支均不得画语音残留键帽");

    /* ---- 下一轮语音会话：set 恢复真实语音键帽（修复不得误伤真实语音轮） ----- */
    kidpal_keybar_scene_set(KVST_SCENE_LISTEN);
    s = kidpal_keybar_scene_get();
    TEST_ASSERT_EQUAL_INT_MESSAGE(KVST_SCENE_LISTEN, s,
        "CG-A1 RED: 复位后 set 场景未生效——下一轮语音会话 LISTEN 键帽将丢失");
    rc = kidpal_keybar_slots(PAGE_P1, s, 1, &left, &right);
    TEST_ASSERT_TRUE_MESSAGE(rc == 0 && left != NULL && right != NULL &&
                             strcmp(left, "说完啦") == 0 &&
                             strcmp(right, "结束") == 0,
        cg_fm("CG-A1 RED: set(LISTEN) 后通用分支 overlay 列 (rc=%d, 左=%s, 右=%s) "
              "期望 (说完啦, 结束)——真实语音轮覆盖层键帽被修复误伤",
              rc, left != NULL ? left : "(null)", right != NULL ? right : "(null)"));

    /* 收尾：回复位态（后续用例与其它文件测试从 idle 语义出发）。 */
    kidpal_keybar_scene_reset();
}

/* ========================================================================== *
 * TEST 2 — CG-A2 story -1 过渡窗键帽裁决（behavioral, seam [CG-C2]; 纯函数无
 * 隐藏状态, 同入参恒同出）。
 * 三态返回语义 / 非过渡窗恒回落 / 在途三子态逐值 == 既有冻结子态表零偏移（含
 * RECORDING 帧 A/帧 B 分叉）/ 非在途回落 / 全域语音残留负扫描 / NULL 防御。
 * RED today（设计态）: 裁决 seam 未实现（weak 哨兵返回 -99）。
 * ========================================================================== */
/* coverage: CG-A2 -- 过渡窗裁决三态决策表（非过渡窗回落 / 在途三子态 == 冻结表零
 *           偏移 / 非在途回落 / 语音残留负扫描 / NULL 防御） */
TEST_CASE("chrome ghost: story -1 transition window takes last known substate, never voice residue (CG-A2)",
          "[chrome_ghost][CG-A2][linux][behavior]")
{
    const char *left = NULL;
    const char *right = NULL;
    const char *ref_l = NULL;
    const char *ref_r = NULL;
    int rc;
    int sub;

    /* ---- 非过渡窗（story_sub 有效）恒回落（out 不触碰，调用方走既有子态表） -- */
    rc = kidpal_keybar_story_transition_slots(KSTORY_PAGE_READER, KSTORY_PAGE_READER,
                                              0, &left, &right);
    cg_require_impl(rc, "kidpal_keybar_story_transition_slots");
    TEST_ASSERT_EQUAL_INT_MESSAGE(CG_TRANS_FALLBACK, rc,
        "CG-A2 RED: story_sub 有效（非过渡窗）时裁决返回非 0——过渡窗缝不得接管既有"
        "story 子态表路径（KSTORY_PAGE_READER 现值必须回落既有分支）");
    TEST_ASSERT_TRUE_MESSAGE(left == NULL && right == NULL,
        "CG-A2 RED: 回落路径写入了 out——回落语义必须零触碰（调用方既有分支自行裁决）");

    /* ---- 过渡窗 + 在途 READER：按最后已知子态裁决，与冻结子态表零偏移（EQ-2） */
    rc = kidpal_keybar_story_transition_slots(-1, KSTORY_PAGE_READER, 0,
                                              &left, &right);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CG_TRANS_RESOLVED, rc,
        "CG-A2 RED: 过渡窗（story_sub=-1）+ 在途 READER 未裁决命中（返回非 1）——"
        "故事会话期缓存未同步过渡窗掉通用分支的缺陷原样保留（KPLOG005 缺陷①因二）");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_story_slots(KSTORY_PAGE_READER,
                                                       &ref_l, &ref_r) == 0,
        "CG-A2 RED(infra): kidpal_keybar_story_slots(READER) 底座回归——冻结子态表"
        "参照失效");
    TEST_ASSERT_TRUE_MESSAGE(left != NULL && ref_l != NULL &&
                             strcmp(left, ref_l) == 0 &&
                             right != NULL && ref_r != NULL &&
                             strcmp(right, ref_r) == 0,
        cg_fm("CG-A2 RED: 过渡窗 READER 键帽 (左=%s, 右=%s) != 冻结子态表 (左=%s, "
              "右=%s)——过渡窗裁决自抄字面/漂移（EQ-2 单源红线: 必须内部经 "
              "kidpal_keybar_story_slots 消费）",
              left != NULL ? left : "(null)", right != NULL ? right : "(null)",
              ref_l != NULL ? ref_l : "(null)", ref_r != NULL ? ref_r : "(null)"));

    /* ---- 过渡窗 + 在途 RECORDING：帧分叉 rec_sub 逐帧 == story_rec_slots ------ */
    rc = kidpal_keybar_story_transition_slots(-1, KSTORY_PAGE_RECORDING, 0,
                                              &left, &right);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CG_TRANS_RESOLVED, rc,
        "CG-A2 RED: 过渡窗 + 在途 RECORDING（帧 A）未裁决命中");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_story_rec_slots(0, &ref_l, &ref_r) == 0,
        "CG-A2 RED(infra): kidpal_keybar_story_rec_slots(0) 底座回归");
    TEST_ASSERT_TRUE_MESSAGE(left != NULL && ref_l != NULL &&
                             strcmp(left, ref_l) == 0 &&
                             right != NULL && ref_r != NULL &&
                             strcmp(right, ref_r) == 0,
        cg_fm("CG-A2 RED: 过渡窗 RECORDING 帧 A 键帽 (左=%s, 右=%s) != 冻结帧表 "
              "(左=%s, 右=%s)——帧分叉未走 rec_slots 单源（EQ-2）",
              left != NULL ? left : "(null)", right != NULL ? right : "(null)",
              ref_l != NULL ? ref_l : "(null)", ref_r != NULL ? ref_r : "(null)"));

    rc = kidpal_keybar_story_transition_slots(-1, KSTORY_PAGE_RECORDING, 1,
                                              &left, &right);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CG_TRANS_RESOLVED, rc,
        "CG-A2 RED: 过渡窗 + 在途 RECORDING（帧 B 引导播报中）未裁决命中");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_story_rec_slots(1, &ref_l, &ref_r) == 0,
        "CG-A2 RED(infra): kidpal_keybar_story_rec_slots(1) 底座回归");
    TEST_ASSERT_TRUE_MESSAGE(left != NULL && ref_l != NULL &&
                             strcmp(left, ref_l) == 0 &&
                             right != NULL && ref_r != NULL &&
                             strcmp(right, ref_r) == 0,
        cg_fm("CG-A2 RED: 过渡窗 RECORDING 帧 B 键帽 (左=%s, 右=%s) != 冻结帧表 "
              "(左=%s, 右=%s)——帧 B「打断/右留空」分叉漂移",
              left != NULL ? left : "(null)", right != NULL ? right : "(null)",
              ref_l != NULL ? ref_l : "(null)", ref_r != NULL ? ref_r : "(null)"));

    /* ---- 过渡窗 + 在途 ORGANIZING：双槽留空 == 冻结表 ------------------------ */
    rc = kidpal_keybar_story_transition_slots(-1, KSTORY_PAGE_ORGANIZING, 0,
                                              &left, &right);
    TEST_ASSERT_EQUAL_INT_MESSAGE(CG_TRANS_RESOLVED, rc,
        "CG-A2 RED: 过渡窗 + 在途 ORGANIZING 未裁决命中——整理过渡窗掉通用分支，"
        "KPLOG005 13:03:08「不等了/结束」双连闪场景未治");
    TEST_ASSERT_TRUE_MESSAGE(kidpal_keybar_story_slots(KSTORY_PAGE_ORGANIZING,
                                                       &ref_l, &ref_r) == 0,
        "CG-A2 RED(infra): kidpal_keybar_story_slots(ORGANIZING) 底座回归");
    TEST_ASSERT_TRUE_MESSAGE(left != NULL && ref_l != NULL &&
                             strcmp(left, ref_l) == 0 &&
                             right != NULL && ref_r != NULL &&
                             strcmp(right, ref_r) == 0,
        cg_fm("CG-A2 RED: 过渡窗 ORGANIZING 键帽 (左=%s, 右=%s) != 冻结表 (左=%s, "
              "右=%s)——整理双槽留空语义漂移",
              left != NULL ? left : "(null)", right != NULL ? right : "(null)",
              ref_l != NULL ? ref_l : "(null)", ref_r != NULL ? ref_r : "(null)"));

    /* ---- 过渡窗 + 非在途（LIST/STAMP/DELETE/BODY/-1）回落（plan 逐字范围） --- */
    {
        static const int not_in_flight[] = {
            KSTORY_PAGE_LIST, KSTORY_PAGE_STAMP, KSTORY_PAGE_DELETE,
            KSTORY_PAGE_BODY_UNAVAILABLE, -1
        };
        int i;
        for (i = 0; i < (int)(sizeof(not_in_flight) / sizeof(not_in_flight[0])); i++) {
            left = NULL;
            right = NULL;
            rc = kidpal_keybar_story_transition_slots(-1, not_in_flight[i], 0,
                                                      &left, &right);
            TEST_ASSERT_TRUE_MESSAGE(rc == CG_TRANS_FALLBACK &&
                                     left == NULL && right == NULL,
                cg_fm("CG-A2 RED: 过渡窗 + 非在途子态(%d) = (rc=%d, 左=%s)——非在途"
                      "相位必须回落既有路径（plan 修法②逐字范围: 只接管录音/整理/"
                      "阅读三在途子态; 该相位残留由 [CG-C1] scene 复位兜底）",
                      not_in_flight[i], rc, left != NULL ? left : "(null)"));
        }
    }

    /* ---- 全域语音残留负扫描：任何 (story_sub=-1, last_known) 组合绝不产出
     *      「说完啦」「不等了」左槽（KPLOG005 缺陷①的可执行负锚） --------------- */
    for (sub = -1; sub <= KSTORY_PAGE_ORGANIZING; sub++) {
        int rec;
        for (rec = 0; rec <= 1; rec++) {
            left = NULL;
            right = NULL;
            rc = kidpal_keybar_story_transition_slots(-1, sub, rec, &left, &right);
            TEST_ASSERT_TRUE_MESSAGE(!(rc == CG_TRANS_RESOLVED && left != NULL &&
                                       (strcmp(left, "说完啦") == 0 ||
                                        strcmp(left, "不等了") == 0)),
                cg_fm("CG-A2 RED: 过渡窗裁决在 last_known=%d rec=%d 产出语音残留键帽"
                      "（左=%s）——过渡窗语义 = 绝不画语音残留（KPLOG005 缺陷①）",
                      sub, rec, left != NULL ? left : "(null)"));
        }
    }

    /* ---- 防御: NULL out -> -1 ------------------------------------------------ */
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_keybar_story_transition_slots(-1, KSTORY_PAGE_READER, 0, NULL, &right),
        "CG-A2 RED: NULL out_left 未按 -1 防御");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,
        kidpal_keybar_story_transition_slots(-1, KSTORY_PAGE_READER, 0, &left, NULL),
        "CG-A2 RED: NULL out_right 未按 -1 防御");
}

/* ========================================================================== *
 * TEST 3 — CG-A1/CG-A2 设备侧接线结构（源锚, seam [CG-C3]①②③ + [CG-C4]）。
 * lcd_display.cc 场景单源改造（g_kidpal_keybar_scene 零残留负锚 + 写沿/读沿正向锚）
 * + sync 区过渡窗裁决消费 + 会话收尾沿复位（application.cc Idle 分支 或
 * kidpal_render_node.cc voice_end, 二者任一）+ keybar.h 声明守 + 扫描器自检。
 * RED today（设计态）: 五处接线锚未落 + keybar.h 无新缝声明 + g_kidpal_keybar_scene
 *           7 处在世。
 * ========================================================================== */
/* coverage: CG-A1 -- 设备侧接线（源锚，注释剥离后可执行源）：apply_look 写沿 /
 *           sync+mid 读沿（>=2）/ 收尾沿复位（Idle 分支或 voice_end）/ g_kidpal_
 *           keybar_scene 可执行源零残留负锚 / keybar.h 声明守
 * coverage: CG-A2 -- sync 区 kidpal_keybar_story_transition_slots( 消费源锚 */
TEST_CASE("chrome ghost: device wiring anchors — scene single-source, transition gate, session-end reset (CG-A1/CG-A2)",
          "[chrome_ghost][CG-A1][CG-A2][structural]")
{
    char *code;
    char *region;
    const char *fn;
    const char *fn_end;
    int n;

    CG_REQUIRE_ROOT();

    /* ---- 扫描器自检（diag_log TEST 4 同款：自检不成立 = 全部结构断言失去咬合力） */
    code = cg_strip_file(CG_LCD_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "CG RED(infra): lcd_display.cc unreadable");
    n = cg_count_sub(code, "kidpal_keybar_slots(page");
    TEST_ASSERT_TRUE_MESSAGE(n >= 1,
        "CG RED(infra): 行级锚计数器失灵（lcd_display.cc 已知在世形态 "
        "kidpal_keybar_slots(page 只命中 0 行）——扫描器坏，本文件结构断言失去咬合力");
    {
        /* 注释剥离生效：已知仅存在于注释的中文文本不得残留在可执行源里。 */
        char *raw = cg_read(CG_LCD_CC, NULL);
        TEST_ASSERT_NOT_NULL_MESSAGE(raw, "CG RED(infra): lcd_display.cc unreadable");
        TEST_ASSERT_TRUE_MESSAGE(strstr(raw, "有效 overlay 判据同源") != NULL,
            "CG RED(infra): lcd_display.cc 已知注释锚缺席——注释剥离自检无法成立"
            "（文件被重写？）。");
        TEST_ASSERT_TRUE_MESSAGE(strstr(code, "有效 overlay 判据同源") == NULL,
            "CG RED(infra): 注释剥离失效（已知注释文本仍留在可执行源里）——本文件"
            "全部结构断言会因注释文本假绿，须复核扫描器");
        free(raw);
    }
    free(code);

    /* ---- [CG-C3]① 场景单源改造（lcd_display.cc）---------------------------- */
    code = cg_strip_file(CG_LCD_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "CG RED(infra): lcd_display.cc unreadable");

    /* 既有函数区锚在守（不得改名/移位——后续区切取的前提）。 */
    fn = cg_find_def(code, CG_SYNC_FN);
    TEST_ASSERT_NOT_NULL_MESSAGE(fn,
        "CG RED(infra): lcd_display.cc 无 kidpal_keybar_sync_locked 定义锚——键帽裁决"
        "本体被改名，须复核锚不得改判为绿（keybar_three_slot TEST 4 同源锚，先复核"
        "该件）");
    /* sync 函数区 = 定义点至 apply_look 定义点（相邻函数, 区切取不依赖窗长）。 */
    fn_end = strstr(fn, CG_APPLY_FN);
    if (fn_end == NULL) {
        fn_end = cg_win_end(fn, CG_FN_WINDOW);
    }

    /* 既有裁决消费在守（diagram 五族既有锚不得被本包改造删失）。 */
    TEST_ASSERT_TRUE_MESSAGE(cg_find_in(fn, fn_end, "kidpal_render_node_story_substate(") != NULL,
        "CG RED(infra): sync 区 kidpal_render_node_story_substate( 消费缺席——故事"
        "子态读沿被删（既有锚, 须复核不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(cg_find_in(fn, fn_end, "kidpal_keybar_slots(") != NULL,
        "CG RED(infra): sync 区 kidpal_keybar_slots( 通用分支消费缺席（既有锚）");
    TEST_ASSERT_TRUE_MESSAGE(cg_find_in(fn, fn_end, "kidpal_keybar_story_slots(") != NULL,
        "CG RED(infra): sync 区 kidpal_keybar_story_slots( 消费缺席（既有锚）");
    TEST_ASSERT_TRUE_MESSAGE(cg_find_in(fn, fn_end, "kidpal_keybar_story_rec_slots(") != NULL,
        "CG RED(infra): sync 区 kidpal_keybar_story_rec_slots( 帧分叉消费缺席（既有锚）");
    TEST_ASSERT_TRUE_MESSAGE(cg_find_in(fn, fn_end, "kidpal_keybar_char_slots(") != NULL,
        "CG RED(infra): sync 区 kidpal_keybar_char_slots( 查字窗消费缺席（既有锚）");

    /* ② 过渡窗裁决消费（CG-A2 接线锚，RED today）。 */
    TEST_ASSERT_TRUE_MESSAGE(cg_find_in(fn, fn_end, "kidpal_keybar_story_transition_slots(") != NULL,
        "CG-A2 RED: kidpal_keybar_sync_locked 未消费 kidpal_keybar_story_transition_"
        "slots(——story 会话期 -1 过渡窗仍直接掉通用分支（KPLOG005 缺陷①因二未接; "
        "纯逻辑缝 [CG-C2] 已冻结, 接线缺位 = 断言不能承载的盲区 GAP-1 半）");

    /* ① 写沿/读沿正向锚（CG-A1 接线锚, RED today）。 */
    region = cg_region_between(CG_LCD_CC, CG_APPLY_FN, CG_MID_FN);
    TEST_ASSERT_NOT_NULL_MESSAGE(region,
        "CG RED(infra): lcd_display.cc 的 kidpal_voice_apply_look 函数区切取失败"
        "（签名锚漂移——keybar_three_slot TEST 4 tooth 7 同款区锚, 先复核该件）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(region, "kidpal_keybar_scene_set(") != NULL,
        "CG-A1 RED: kidpal_voice_apply_look 区内未消费 kidpal_keybar_scene_set(——"
        "语音场景写沿未上收 keybar 单源（[CG-C1]；场景写沿留设备侧 = 单源接线缺位）");
    free(region);

    n = cg_count_sub(code, "kidpal_keybar_scene_get(");
    TEST_ASSERT_TRUE_MESSAGE(n >= 2,
        cg_fm("CG-A1 RED: lcd_display.cc kidpal_keybar_scene_get( 仅 %d 处（期望 "
              ">= 2: sync 通用分支入参 + chrome 中部显隐判据两读沿）——场景读沿未"
              "全量上收单源", n));

    /* ① 负向锚：g_kidpal_keybar_scene 可执行源零残留（第二真相源退场）。 */
    n = cg_count_sub(code, "g_kidpal_keybar_scene");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, n,
        cg_fm("CG-A1 RED: lcd_display.cc 仍存在 %d 处 g_kidpal_keybar_scene（可执行"
              "源）——设备侧场景 static 未上收 keybar 单源（[CG-C1]; 同窗双写 = 谁生效"
              "说不清, 第二真相源红线）", n));
    free(code);

    /* ③ 会话收尾沿复位（RED today）：application.cc HandleStateChangedEvent Idle
     * 分支（既有 ctx_set(0) 收尾沿, RCL-F1 先例）或 kidpal_render_node.cc
     * voice_end——二者任一命中即绿（1g 预评裁量, 见头注 [CG-C3]③）。 */
    {
        char *app_code = cg_strip_file(CG_APP_CC);
        int reset_hit = 0;
        TEST_ASSERT_NOT_NULL_MESSAGE(app_code, "CG RED(infra): application.cc unreadable");
        if (strstr(app_code, "kidpal_keybar_scene_reset(") != NULL) {
            reset_hit = 1;
        }
        free(app_code);
        if (!reset_hit) {
            char *rn_code = cg_strip_file(CG_RENDER_CC);
            TEST_ASSERT_NOT_NULL_MESSAGE(rn_code,
                "CG RED(infra): kidpal_render_node.cc unreadable");
            fn = cg_find_def(rn_code, "void kidpal_render_node_voice_end(void)");
            TEST_ASSERT_NOT_NULL_MESSAGE(fn,
                "CG RED(infra): kidpal_render_node.cc 无 voice_end 定义锚（既有锚漂移, "
                "须复核不得改判为绿）");
            fn_end = cg_win_end(fn, CG_FN_WINDOW);
            TEST_ASSERT_TRUE_MESSAGE(cg_find_in(fn, fn_end,
                "kidpal_keybar_scene_reset(") != NULL,
                "CG-A1 RED: 会话收尾沿无 kidpal_keybar_scene_reset(——application.cc "
                "HandleStateChangedEvent Idle 分支与 kidpal_render_node.cc voice_end "
                "两处均缺席（[CG-C3]③; g_kidpal_keybar_scene 语音会话结束后永不复位"
                "缺陷原样保留, KPLOG005 缺陷①因一未治）");
            free(rn_code);
        }
    }

    /* ---- [CG-C4] keybar.h 新缝声明守（设备侧经 include 消费的接线前提） ------ */
    code = cg_strip_file(CG_KEYBAR_H);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "CG RED(infra): keybar.h unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "kidpal_keybar_scene_set(") != NULL &&
                             strstr(code, "kidpal_keybar_scene_get(") != NULL &&
                             strstr(code, "kidpal_keybar_scene_reset(") != NULL &&
                             strstr(code, "kidpal_keybar_story_transition_slots(") != NULL,
        "CG-A1/A2 RED: keybar.h 无新缝四符号声明——设备侧 lcd_display.cc 经 include "
        "消费无从谈起（[CG-C4]; additive 尾部追加, 既有声明零触碰）");
    free(code);
}

/* ========================================================================== *
 * TEST 4 — CG-A3 成功归档路径层栈收口（结构源锚, seam [CG-C3]④; 帧类型负锚）。
 * story_list 前推闭包 + 盖章闭包两处 begin_page -> end_page -> 既有通道收口
 * （story_local_front | dismiss_front_overlay 二者之一——既有符号闭集即负锚机器
 * 形态, 绝无新发明下行帧类型）+ ScheduleStoryRender 在役先例守 + 扫描器自检。
 * RED today（设计态）: 两闭包均无收口（13:03:20 action=4 白吃按键实证形态）。
 * ========================================================================== */
/* coverage: CG-A3 -- 成功归档层栈收口（结构）：story_list 前推闭包与盖章闭包
 *           begin_page->end_page->既有通道收口序锚 + 通道 ∈ 既有符号闭集（帧类型
 *           负锚）+ ScheduleStoryRender 在役先例守；BOOT 路由运行时半 = CG-A5 */
TEST_CASE("chrome ghost: success-archive closures dismiss residual overlay via existing channel (CG-A3)",
          "[chrome_ghost][CG-A3][structural]")
{
    char *code;
    const char *cl;
    const char *cl_end;

    CG_REQUIRE_ROOT();

    /* ---- 扫描器自检（mcp_server.cc: 注释锚 + 可执行字面锚） ------------------ */
    code = cg_strip_file(CG_MCP_CC);
    TEST_ASSERT_NOT_NULL_MESSAGE(code, "CG RED(infra): mcp_server.cc unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(code, "story_saved_stamp") != NULL,
        "CG RED(infra): mcp_server.cc 已知可执行字面锚缺席（story_saved_stamp 日志"
        "串）——注释剥离把字符串字面也剥了？扫描器坏");
    free(code);
    {
        char *raw = cg_read(CG_MCP_CC, NULL);
        TEST_ASSERT_NOT_NULL_MESSAGE(raw, "CG RED(infra): mcp_server.cc unreadable");
        TEST_ASSERT_TRUE_MESSAGE(strstr(raw, "复刻同文件 schedule restore 分支") != NULL,
            "CG RED(infra): mcp_server.cc 已知注释锚缺席——注释剥离自检无法成立"
            "（文件被重写？）。");
        free(raw);
    }

    /* ---- 在役先例守（ScheduleStoryRender: begin -> end -> story_local_front，
     *      既有回落形态不得被本包改造删失） ----------------------------------- */
    code = cg_strip_file(CG_MCP_CC);
    cl = strstr(code, CG_SCHED_STORY);
    TEST_ASSERT_NOT_NULL_MESSAGE(cl,
        "CG RED(infra): mcp_server.cc 无 ScheduleStoryRender 定义锚（既有锚漂移, "
        "须复核不得改判为绿）");
    cl_end = cg_win_end(cl, CG_SCHED_WINDOW);
    TEST_ASSERT_TRUE_MESSAGE(cg_order_in(cl, cl_end,
        "kidpal_render_node_begin_page(", "kidpal_render_node_end_page(") &&
        cg_order_in(cl, cl_end, "kidpal_render_node_end_page(", CG_FRONT_FN),
        "CG RED(infra): ScheduleStoryRender 既有 begin->end->story_local_front 回落"
        "形态缺席（在役先例被删, 须复核不得改判为绿）");

    /* ---- story_list 前推渲染闭包（RED today: 无收口） ------------------------ */
    cl = strstr(code, CG_LIST_CLOSURE);
    TEST_ASSERT_NOT_NULL_MESSAGE(cl,
        "CG RED(infra): story_list 前推闭包锚 app.Schedule([view_snapshot, "
        "list_snapshot]() 漂移——闭包被重构须复核锚, 不得改判为绿");
    cl_end = cg_win_end(cl, CG_CLOSURE_WINDOW);
    TEST_ASSERT_TRUE_MESSAGE(cg_order_in(cl, cl_end,
        "kidpal_render_node_begin_page(", "kidpal_render_node_end_page("),
        "CG RED(infra): story_list 前推闭包 begin/end_page 序漂移（云端夹钳纪律"
        "既有形态, 须复核不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(
        cg_find_in(cl, cl_end, CG_FRONT_FN) != NULL ||
        cg_find_in(cl, cl_end, CG_DISMISS_F) != NULL,
        "CG-A3 RED: story_list 前推渲染闭包 end_page 后无层栈收口——夹钳推入的 "
        "SERVER_PAGE 遗留占逻辑前台, 归档后首按 BOOT 被路由成 action=4(overlay_close) "
        "白吃按键（KPLOG005 13:03:20 实证; 收口须复用既有通道 "
        "kidpal_render_node_story_local_front / dismiss_front_overlay 二者之一——"
        "帧类型负锚: 不得新发明下行帧类型）");

    /* ---- 盖章帧渲染闭包（RED today: 无收口） -------------------------------- */
    cl = strstr(code, CG_STAMP_CLOSURE);
    TEST_ASSERT_NOT_NULL_MESSAGE(cl,
        "CG RED(infra): 盖章闭包锚 app.Schedule([stamp_view, stamp_list, "
        "stamp_text]() 漂移——闭包被重构须复核锚, 不得改判为绿");
    cl_end = cg_win_end(cl, CG_CLOSURE_WINDOW);
    TEST_ASSERT_TRUE_MESSAGE(cg_order_in(cl, cl_end,
        "kidpal_render_node_begin_page(", "kidpal_render_node_end_page("),
        "CG RED(infra): 盖章闭包 begin/end_page 序漂移（云端夹钳纪律既有形态, "
        "须复核不得改判为绿）");
    TEST_ASSERT_TRUE_MESSAGE(
        cg_find_in(cl, cl_end, CG_FRONT_FN) != NULL ||
        cg_find_in(cl, cl_end, CG_DISMISS_F) != NULL,
        "CG-A3 RED: 盖章帧渲染闭包 end_page 后无层栈收口——成功归档路径与失败路径"
        "（4068dc6c dismiss_front_overlay 先例）不对齐, 覆盖层逻辑前台残留吃按键");
    free(code);
}

/* ========================================================================== *
 * TEST 5 — CG-A4 回归网登记守（anti-vacuity / 注册序 / 双侧在册）。
 * 本件登记进 HOST_ACCEPTANCE_SRCS（builder FIRST ACTION）+ 注册序（smoke 之前）
 * + 回归网三冻结件（keybar_three_slot / story_page_v2 / key_layer_dispatch）与三
 * 实现源（keybar.c / kidpal_voice_states.c / story_page.c）在册守 + 设备侧
 * firmware/main/CMakeLists.txt display/keybar.c 在册守。
 * RED today（设计态）: 本件未登记进 host CMakeLists（其余为 must-stay-green 守卫）。
 * ========================================================================== */
/* coverage: CG-A4 -- 回归网登记守：本件登记 + 注册序 + 三冻结件/三实现源在守 +
 *           设备侧 keybar.c 在册守；运行面 = 红基线实跑对照 227T3F（先存红 =
 *           p1_auxbar AB-A1/A2/A4, 锚 0df2c0c3）, 零新增意外红 */
TEST_CASE("chrome ghost: regression net registration guards — self + trio + impl sources (CG-A4)",
          "[chrome_ghost][CG-A4][structural]")
{
    char *cmake;
    const char *p_self;
    const char *p_smoke;

    CG_REQUIRE_ROOT();

    /* ---- host runner 登记守 ------------------------------------------------- */
    cmake = cg_read(CG_HOST_CMAKE, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(cmake, "CG RED(infra): host_app/main/CMakeLists.txt unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "host_infra_smoke.acceptance.c") != NULL,
        "CG RED(infra): host CMakeLists 无 host_infra_smoke.acceptance.c——smoke "
        "末位锚缺席（注册序守卫失去基准, 须复核不得改判为绿）");

    p_self = strstr(cmake, "chrome_ghost.acceptance.c");
    TEST_ASSERT_NOT_NULL_MESSAGE(p_self,
        "CG-A4 RED: host_app/main/CMakeLists.txt 未登记 chrome_ghost.acceptance.c——"
        "本冻结验收件不进 host runner（全部断言不会被执行 = 空过绿）；builder FIRST "
        "ACTION（diag_log 261022 先例口径, 登记行不在 acceptance-author 写权内）");
    p_smoke = strstr(cmake, "host_infra_smoke.acceptance.c");
    TEST_ASSERT_TRUE_MESSAGE(p_self < p_smoke,
        "CG-A4 RED: chrome_ghost.acceptance.c 登记在 host_infra_smoke.acceptance.c "
        "之后——必须置于 smoke 之前（smoke 末位 = 最先执行, 包 0 boundary 注册序守卫）");

    /* 回归网成员在守（不被本包登记改动静默摘除）。 */
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "keybar_three_slot.acceptance.c") != NULL,
        "CG-A4 RED: 回归网成员 keybar_three_slot.acceptance.c 从 host 登记消失——"
        "keybar 冻结齿（含 kidpal_keybar_slots scene 越界防御列, EQ-3 前提）必须"
        "继续全量在跑");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "story_page_v2.acceptance.c") != NULL,
        "CG-A4 RED: 回归网成员 story_page_v2.acceptance.c 从 host 登记消失——"
        "story 子态冻结齿必须继续全量在跑");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "key_layer_dispatch.acceptance.c") != NULL,
        "CG-A4 RED: 回归网成员 key_layer_dispatch.acceptance.c 从 host 登记消失——"
        "键路由/层栈冻结齿（overlay_close 路由语义回归面）必须继续全量在跑");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "../../../display/keybar.c") != NULL,
        "CG-A4 RED: 实现源 display/keybar.c 从 host 登记消失——新缝强符号无处落地"
        "（EQ-1: 哨兵红会掩盖「未接线」）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "../../../display/kidpal_voice_states.c") != NULL,
        "CG-A4 RED: 实现源 display/kidpal_voice_states.c 从 host 登记消失——语音"
        "场景闭集单源（CG-A1 复位值出闭集判据的参照系）失去强符号");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "../../../display/story_page.c") != NULL,
        "CG-A4 RED: 实现源 display/story_page.c 从 host 登记消失——story 子态枚举"
        "与记录语境标志单源失去强符号");
    free(cmake);

    /* ---- 设备构建登记守（本包零新增设备登记——keybar.c 既有在册不得被摘） ----- */
    cmake = cg_read(CG_MAIN_CMAKE, NULL);
    TEST_ASSERT_NOT_NULL_MESSAGE(cmake, "CG RED(infra): firmware/main/CMakeLists.txt unreadable");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "display/keybar.c") != NULL,
        "CG-A4 RED: firmware/main/CMakeLists.txt display/keybar.c 登记缺席——新缝"
        "函数不进设备构建（CG-X2 设备编译闸必红; 既有在册登记被摘 = 回归面受损）");
    TEST_ASSERT_TRUE_MESSAGE(strstr(cmake, "display/kidpal_voice_states.c") != NULL,
        "CG-A4 RED(infra): firmware/main/CMakeLists.txt display/kidpal_voice_states.c "
        "登记缺席（既有在册, 须复核不得改判为绿）");
    free(cmake);
}

/* ---------------------------------------------------------------------------
 * REVISION LOG (acceptance-author only; tighten-only discipline):
 *   2026-10-22 初版冻结（TaskPackage 261022-p3-record-chrome-ghost-fix;
 *               acceptance[] CG-A1~A4 + CG-A5 structural/live-manual 为冻结边界;
 *               语义权威 = debt TECH-261022-p3-record-session-state-churn 缺陷①② +
 *               reviewed plan（architect /triage-change 2026-10-22 比例化 review +
 *               human 触点①确认））。五个 TEST_CASE：CG-A1 行为一件（scene 生命周期
 *               语义 + 鬼影排除 + 真实语音轮不误伤）、CG-A2 行为一件（过渡窗三态
 *               决策表 + 冻结子态表零偏移 + 语音残留负扫描）、CG-A1/A2 结构一件
 *               （场景单源改造 + 过渡窗消费 + 收尾沿复位 + keybar.h 声明守 + 负锚）、
 *               CG-A3 结构一件（成功归档两闭包既有通道收口 + 帧类型负锚机器形态）、
 *               CG-A4 结构一件（登记守 + 注册序 + 回归网在守）。CG-A5 为 live-manual
 *               真机目检项，不落自动断言（DLG-A6 同款口径），本 Pod 到真机门前停等
 *               human。冻结落盘冒烟已实跑（2026-10-22 双通道, 逐字记录见上方
 *               「冻结落盘实跑」段——host 全量 227T3F 与锚 0df2c0c3 一致零新增红;
 *               独立通道 5 Tests 5 Failures 全设计态红, COMPILE_EXIT=0 零 warning;
 *               双向可满足性经 /mnt/c/tmp/cg_work 合成树 + 七处定向变异取证, 不写入
 *               工作树）。输出散文纪律：验收测试 case 不用黑话。
 * --------------------------------------------------------------------------- */
