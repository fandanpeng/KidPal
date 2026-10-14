---
rigor: full
riskTier: high
sourceType: delivery-story
reviewed-by: pending（门 1 重拆重审 2026-09-29）
date: 2026-09-29
---

# M4-K3 记录会话跨端链路 + prompt v8 + story_list 推送重裁 + live 真机确认轮（阶段四收口）

> **快照修正注（2026-09-30，Pod intake / 1g 预评）**：本件拆包期以「prompt v6 现役、本批落 v7」为假设起草。1g architect 预评实证 **v7 槽已被 260925-prompt-v7-round-routing 批占用**（v7 文件在树 + OPENCLAW_SYSTEM_PROMPT_CARRIER 声明现役 v7 + provider.config.yaml 三键指 v7 + test_prompt_v7_round_routing.acceptance.py 在世），本批双指令批改落 **v8**。正文保留拆包原文处出现的「v7/v6」字样均按此注读作「v8/v7」；taskpackage story/E2/E3/notes/docsSyncReason 已同步改写。C4 arch 收口半已提前落地（c735062），E3 原文「随本包 docs-sync 一并落地」措辞作废。

> 来源：里程碑 `delivery/milestones/milestones.jsonl#M4-star-notebook`（state=planning）nextAction 块①记录会话（录音+归档链路）跨端半 + live 收口。
> story 语义权威 = `docs/requirements.md` 现文（S-F2.2 AC1–AC4 + §2.1 记录会话段 + §2.3 矩阵引导态/录音态两行 + WF-F2-02）；支撑锚 = arch §5.2 场景④、§5.5.4、§5.5.5、to-be ⑥；契约面 = `docs/contracts/interface-contract.md` §四（kidpal_session_ctx **已 LANDED**，本包只落实现）/§1.4（start_recording 既有）。
> 重拆注记（2026-09-29）：本件按 260928/260929 拆包新规收窄为五件骨架——「改法取舍」「阶段划分」撤出（包内实现决策承载位 = 冻结 acceptance 断言化 + taskpackage notes + planner 派发 prompt；planner Step 1g architect 前置预评后补写「技术方案」节并留痕）；原拆包侧实现决策要点已压缩进 taskpackage `notes` 备查。重拆新增承载面：defer 包 260928-story-connect-push-defer 摘除 K1 建连即推后，①恢复语义归本包重裁（见跨包节）。

## 背景摘录（新建型锚「现状与差距」）

- **现状·session_ctx 双端零实现（板端半）**：契约 §四已 LANDED（CDQ-260914 明写「实现面归 dev-loop 后续包」）；固件 grep 零命中——protocol 层既有 Send* 面 = 音频/唤醒词/聆听起停/abort/mcp（`protocol.h:70-75`），**无 KidPal §四私有消息类型发送原语**（门 1 F4 枚举修正）；会话发起入口 = `application.cc:763 HandleToggleChatEvent`（arch to-be ⑥ 既定接线点：读 `kidpal_layer_top()`）；front 侧拦截 K1 已 merged（本包 dependsOn 实存前提）。
- **现状·录音态演出半成品**：REC 闪点 + 大时长 + 声波柱在世（story_page.c 录音态渲染，K2 本批对齐 WF-F2-02 版式与回显占位缝）；`control/start_recording` 消费 + `archive_pend` 归档出口 + `kidpal_story_on_control` 幂等在世；**话语回显区无消费接线**（stt 帧现只进语音轮聆听回显，不进 story 视图）；引导态（P3 按 KEY 后、开口前）无实现。
- **现状·手势语义旧口径**：冻结输入表录音态 KEY 长按=丢弃（v1.47 已反转为=保存）；BOOT 丢弃族（短/长/双击同义）设备侧不存在——K2 落输入表语义位、本包接会话动作（收尾=listen-stop 既有 / 丢弃=cancel 语义 → front acc.cancel 既有）。
- **现状·prompt 单指令**（拆包期写「v6 单指令」，现役实为 v7、影子口径与语音纪律已收口——见快照修正注）：故事意图段只有「我要讲故事」族（「帮我记下来」全 deploy 目录零命中）；V31 四条回退（讲错啦丢弃/漏判挽回等）在现役故事段既有。
- **现状·story_list 推送两时点**：K1 原三时点中的①建连即推被 defer 包摘除（真机实证：设备每会话 WS 重连 → 重复推送盖页，旧固件无条件渲染 P3 页盖聆听态 + 零 CJK 字体半截页）；现役 = 归档后 + 删除后两时点；①恢复语义归本包重裁（defer review-packet 未覆盖面第 1 条：真机复验「设备侧停弹页面」也归本包设备轮）。
- **现状·设备侧无 timer 生产者**：`ses_machine_dispatch` 生产代码零调用（60s/120s 双阈值 = M1 遗留 to-be ②）——本包**不复活**（120s 收尾全在 front，K1-A5；板端只消费收尾结果演出）。
- **差距**（本包交付）：① 板端 session_ctx 发送（原语扩展 + 会话发起入口 + source_page 映射）；② 引导态 + 录音态回显消费接线；③ 录音手势族会话接线（保存/丢弃/引导态结束）；④ prompt v8（双指令 + 意图边界 + 影子口径零偏移继承）；⑤ story_list 推送①时点重裁；⑥ live 真机确认轮一次性收口全链（含 K1/K2 deferred live 半）。

## 可行性判据

- **契约前置实存**：kidpal_session_ctx 契约 §四 LANDED（CDQ-260914），front 拦截半 K1 已 merged 本地 main（e12a8b94）——双端对端语义现文可核对，无契约改动面。
- **发送缝前置可期**：K2 在本包前落 kidpal_story_delete protocol 层发送原语（mergeOrder 39→40 串行）——session_ctx 发送为同族扩展，原语形态复用不新造（K2 未 merged 前本包不 launch，dependsOn 实存）。
- **语音轮底座在世**：`kidpal_voice_states` 场景码（M1-K3 冻结）+ WF-B-01 聆听布局 + 气泡文案参数化路径——引导态为底座消费非新渲染面。
- **回退与归档链路在世**：V31 四条回退（现役故事段既有）、StoryAccumulator routing/recover（K1 merged 现文）、control/start_recording → archive_pend 出口既有。
- **prompt 版本化管线在世**：v1–v7 演进先例 + OPENCLAW_SYSTEM_PROMPT_CARRIER 声明翻转机制 + provider.config 三键同步 + 渲染链（apply-xiaozhi-overlay）——v8 走既有管线零新机制。
- **live 轮取证纪律在案**：kidpal-serial-monitor / kidpal-live-smoke-debug / kidpal-flash-device 技能齐，三源日志关联（串口 + front journalctl + 引擎 docker logs）先例多轮（M2/M3 设备轮）。

## 跨包事实与共享冻结面约束（含跨切片边角）

- **session_ctx 时序与幂等（契约 §四语义，front 侧 K1 冻结）**：发送时点 = 首帧音频前（server hello 之后）——发晚了 front 已按普通会话切句送 LLM，累积态切换失效；通道复用的后续会话（wss 常驻第 2+ 次会话）同样发送，不只首连；重复发送安全（front 幂等入口承接）。发送失败/通道未就绪 = 降级普通会话（不阻塞会话建立、不重试风暴），向后兼容负锚。
- **source_page 映射单点**：读取面 = `kidpal_layer_top()`/页枚举，映射在会话发起入口单点——**KEY 按与唤醒词「你好，小智」两入口都过该入口**（漏唤醒词路径 = 引导态失效）；P3→story 之外 home/schedule 闭集映射一次做全（契约 §四「其余取值现版本无行为（闭集预留）」——防未来再开固件面）。
- **story_list 推送①重裁（defer 承接）**：建连即推与设备每会话 WS 重连冲突实证在案（defer 包 review-packet）；重裁输入 = 冲突形态 + K3 新增的引导态会话建立时点（天然候选缝：session_ctx 发送同拍或 front 收到 ctx 后条件推送）；重裁结论 = acceptance 冻齿断言（推送时点行为级断言），defer 遗留的「真机复验设备侧停弹页面」随本包设备轮一并目击。
- **K2 冻结面（rebase 前提）**：本包 intake rebase K2——story_page 输入表语义位（KEY 长按=保存/BOOT=丢弃）、回显占位缝、kidpal_story_delete 发送原语均为 K2 冻结断言，本包同族扩展（session_ctx 原语同缝）不改 K2 语义；K2 host 半 merged 为本包设备轮前置（LP4）。
- **板端零新增计时器**：120s 收尾全在 front（K1-A5 front 驱动）——板端只消费收尾结果演出链（story_text+seq 下发 → 退 REC → 盖章 → 回列表）；60s 问答超时语义零触碰（M1 遗留 to-be ② 不复活）。
- **K1 front 冻结语义（只消费）**：拦截/累积/收尾词检测/120s/账本（StoryAccumulator routing/recover/cancel 语义）零触碰；「讲错啦」语音丢弃 = V31 回退既有链路，只核对零偏移、不重接。
- **prompt v8 约束面**：v8 新文件不回改 v7（版本化演进）；ADR-001 接线不变式 = 渲染面==声明面==被测面（v8 文件 + OPENCLAW_SYSTEM_PROMPT_CARRIER 声明翻转 + provider.config 三键同步 + apply-xiaozhi-overlay.sh 版本行翻转）；影子口径收口 = story_list/story_text 由 front 机械推送单源，v8 故事段明示不产 story 类 display 块。
- **live 轮组织约束**：单真机不撞车 + human 在场持板（LP4）；K2 若落新 art 资产 = app+art 双分区烧录，否则 app-only 合规；A6 八步目击序内失败任一步 = 轮内定位（fixer 预算）而非重开轮。
- **跨切片边角（builder 易漏）**：① 引导态结束（未开口）≠ 录音态丢弃（已录内容）——前者无归档无 cancel 副作用、后者 acc.cancel，两条路径 front 侧语义勿混；② 「帮我记下来」双指令判定不得越界误触课表录入意图（「帮我记下来明天带跳绳」= 课表录入——v8 故事段与课表段意图边界措辞随冻齿断言）；③ live 轮「重启流水号不回退」目击 = front 侧账本证据（K1 面）+ 设备侧盖章 N 目测双证；④ source_page 映射若散落多处实现 = 漏入口根因——单点函数。

## 铁律约束

- 不改 `docs/contracts/**` 与 spec；session_ctx/delete 形状 = 契约冻结版零自定。
- prompt v8 新文件不回改 v7；carrier 声明翻转与渲染面接线的不变式守卫（禁版本字面断言，ADR-001）。
- 板端零新增计时器（60s/120s 设备侧 timer 不复活）；`ses_machine_dispatch` 生产零调用现状不在本包改（M1 遗留面）。
- K1 front 冻结语义（拦截/累积/收尾/账本）与 K2 冻结面（story_page 输入表/渲染缝/发送原语）只消费零修改；`kidpal_voice_states`/语音轮四态 M1 冻结面零触碰。
- OpenClaw 本体（ECS）架构零改动（prompt = 部署制品模式，既有交接/渲染链）。
- mutation 齿自证（RED→GREEN 声明 + counterEvidence 回填 taskpackage）。

## demo 证据形态复述

offline：host 齿（发送缝/引导态/手势接线/回显消费/推送重裁时点）+ prompt v8 版本化 offline 断言 RED→GREEN 全程留痕 + server pytest 回归零新增红 + device build 绿；live：human 持板真机确认轮（八步目击序一次性收口 + 三源日志关联取证 + human 目击记名）——demo 口径 = offline 全量绿 + live 轮证据落账（含 K1-A7/K2 真机目测半 deferred 收口声明 + defer 包「设备侧停弹页面」复验目击）。

## 技术方案（planner+architect 预评）

> 留痕：2026-09-30，planner Step 1g 前置技术预评档产出（architect 档，dispatch id 见 pod-events dispatch-completed，glm-5.3 transcript 派生；advisory 效力——不豁免任何门、不改验收与 TaskPackage 事实、不构成通过判据）。本节为 planner 只读消费 plan.md 规则的唯一例外写入点（task-intake-and-slicing.md:266 例外条款）。本节按 v8 槽位修正后口径书写。

### T1 · session_ctx 板端发送（architect Q1）

- 原语同族扩展：`protocol.h` `SendKidpalStoryDelete` 旁加 `virtual void SendKidpalSessionCtx(const char* source_page)`；`protocol.cc` 基类实现仿 `SendAbortSpeaking` 拼 `{"session_id":"...","type":"kidpal_session_ctx","source_page":"..."}` 走 `SendText` 虚函数——**无协议 id 分配面**（消息只有 type 字符串字段，契约 §四）；`websocket_protocol.{h,cc}` 零改动（K2 delete 原语无 override 先例，`websocket_protocol.cc:63` SendText 虚实现自动生效）。
- 发送点 = `application.cc` `HandleStateChangedEvent` Listening 分支（:946-956）内 `SendStartListening`（:953）之前、且在 `IsAudioProcessorRunning` 门（:951）**之外无条件执行**——契约时序（首帧音频前、hello 之后）天然满足；放门外防通道复用会话（processor 在跑）短路漏发；front 幂等承接重复发送。
- source_page 映射单点：读 `kidpal_render_node_get_page()`/`kidpal_layer_top()` 页枚举（KP_PAGE_STORY→"story"、home/schedule 闭集一次做全、其余→"home"），用页枚举符号非数值（§2.3 对账② 页序漂移注记）；发送失败只 log 不重试（notes ③ 负锚）。
- 双入口收敛实证：KEY 按路径已被 K2 前置分流吃掉（`waveshare-s3-rlcd-4.2.cc:197-206` `keya_story_page_wants` → `HandleStoryKey` → `StoryDispatchAct` case `KSPV2_ACT_REC_SESSION` 现为 log-only 桩）；唤醒词路径 = `application.cc:1070` `WakeWordInvoke`。两入口最终都经 `SetListeningMode`（:1000-1003）→ `SetDeviceState(kDeviceStateListening)` → Listening 分支。

### T2 · 引导态 + 录音态回显（architect Q2）

- 引导态底座全在世：`KVR_CTX_LISTEN_MANUAL` 语境（voice_round.h:32）+ `kidpal_voice_apply_look(KVST_SCENE_LISTEN)`（lcd_display.cc:2123）。气泡换「需要我帮你记点什么？」= `lcd_display.cc` 气泡写点（`g_kidpal_voice_header_label`，:432-435 注明「look.bubble 写点由旧 chat 挂点统一迁此」）加小覆写缝（`kidpal_lcd_voice_bubble_override` 或记录语境标志 + apply_look 内取替）——**不动 `kidpal_voice_states.c` M1 冻结映射**。
- KSPV2_ACT_REC_SESSION 消费位（mcp_server.cc:244-247 log-only 桩）接：Schedule 推语音层前台（`kidpal_render_node_show_chat` 同 GLOBAL_ACT_SESSION_TOGGLE 先例，board 文件 :127-128）+ 会话发起 + 置记录语境标志 + 引导气泡覆写。
- 开口切录音态 = UTT_START 事件沿实存（application.cc:176-178 注册 `on_vad_change` → `MAIN_EVENT_VAD_CHANGE`、:323-328 现只驱动 LED）——加「记录语境 && speaking && 引导态前台 → 切 story RECORDING 页」，事件驱动零新增计时器；齿形态 = 纯逻辑函数（语境标志 + speaking 入参 → 切页动作出参）+ 结构齿，真机沿归 live 轮（AFE VAD host 不可机判）。
- 回显消费：`story_page.c` 尾部追加纯逻辑回显缝（住 story_page 防触碰 M1 冻结面，冻结纪律只增不改；参照 `voice_round.c` `kidpal_voice_echo_*` 行数模型）+ `application.cc` stt 分支（:658-676）记录语境旁路喂入（与 front `_story_feed_from_stt` 旁路同构，不改 wire）。

### T3 · 录音手势族会话接线（architect Q3）

- 语义位已全冻（story_page.h:108-126）：STOP_RECORD(10, KEY 短/长按同义保存)/DISCARD_RECORD(11, BOOT 任一手势丢弃)/REC_SESSION(7)。消费位 = `StoryDispatchAct`（mcp_server.cc:230-334）STOP/DISCARD 现走本地导航 case（:311-325 只 ScheduleStoryRender + log、无会话动作）——K3 接线点即此两 case。
- **STOP → 收尾保存 = 调 `app.StopListening()`**（application.cc:846-859 → `SendStopListening` + Idle），front 归档缝 `_story_stop_on_listen_stop`（front_app.py:121-139）由 `pump_client_to_upstream`（:518-521）消费设备上行 listen/state=stop 触发 stop 归档 + story_text/story_list 下发。**严禁走 `ToggleChatState` Listening 分支**（:802-804 直接 `CloseAudioChannel` 不发 listen stop → front 走 ws_disconnect cancel 丢弃路径而非归档——两条 front 路径混源风险的板端根）。
- DISCARD → 丢弃 = `CloseAudioChannel` 即达 front 断开钩子 `_get_story_acc().cancel(session_id, reason="ws_disconnect")`（front_app.py:972-979，cancel 弹缓冲不落盘）；「讲错啦」语音丢弃 = V31 prompt 回退既有链路零重接。引导态结束（未开口零累积）走既有覆盖层列（key_gesture_parser.cc:162-181）→ SESSION_TOGGLE → CloseAudioChannel，空累积 stop 自然不归档，front 零新分支。
- 冻齿断言方向：STOP → wire 出现 listen/state=stop 且 stories 落盘 + 盖章链闭环（story_text 下发 → `mcp_server.cc` VRT_ROUTE_STORY_TEXT → `on_saved` → 回列表）；DISCARD/引导结束 → 无 listen stop、stories 目录零新增。STOP 后整条板端消费闭环是 K2 冻结齿零偏移复验对象。

### T4 · prompt v8（architect Q4，v7 槽位冲突修正）

- **关键修正：本批落 v8 非 v7**——v7 槽已被 260925-prompt-v7-round-routing 批占用。carrier :50-52 ARQ-260808 槽位预约规则（「bump 前先核该版本号是否已被占用」）正中此案。
- 动作四件：① 新文件 `server/deploy/openclaw-system-prompt.v8.md`——以 v7 为基只动故事段（加「帮我记下来」第二指令 + 意图边界措辞），v7 刚过对抗 review 其余段落零夹带；② `OPENCLAW_SYSTEM_PROMPT_CARRIER` 声明翻转（artifact 行 v7→v8 + 渲染面行 + 历史版本行 append v7 沿革条目，本文件 = 版本演进史唯一全量副本）；③ `provider.config.yaml` 三键同步（artifact/source/deploy_target → v8）；④ **`apply-xiaozhi-overlay.sh:93-94` PROMPT_SRC/PROMPT_DST 版本行翻转**（预评补的第四件，拆包粗填漏渲染脚本）。
- 双指令边界措辞：v7:52 现文 =「我要讲故事」「我来编一个故事」→ start_recording；v8 增「帮我记下来」族，边界 = **「帮我记下来」独立成句或接叙述内容 = 开录；「帮我记下来 + 待办/物品/时间」= 课表录入**；负例「帮我记下来明天带跳绳」显式写进故事段排除例（offline 可机判 prompt 文本含边界例句；LLM 真判定归 live 目击项，notes ⑩）。影子口径（v7:34/:54）与标题 ≤12 字（v7:53）零偏移继承。
- ADR-001 纪律：新齿用裸名 needle 或解析渲染/声明两面得版本（carrier:46-47 先例），禁版本字面断言；v8 翻转初判不破 test_prompt_v7_round_routing（:14 明示「不断言当前接线 = v7」），acceptance-author 须复核一次。

### T5 · story_list 推送①重裁（architect Q5，acceptance 冻齿关键输入）

- 三候选评估：(a) session_ctx 发送同拍（设备侧请求）不成立——story_list 是 front→设备下行，设备无请求消息，加 fetch 消息 = 契约改动越铁律；(c) hello 后条件推送不可行——hello 时 front 无法预知本会话是否记录语境（ctx 时序在其后），条件化只能等 ctx、退化为 (b)；**(b) front 收到 ctx 后条件推送 = 推荐形态**。
- 具体落点：`_consume_kidpal_control` kidpal_session_ctx 分支（front_app.py:180-183）切累积态后同帧 `await _push_story_list(websocket)`，条件收紧为 **routing 沿触发**——`acc.routing(session_id) != "accumulate"` 时才推（front 在 `acc.start` 前自查 routing，story.py 零改动；重复 ctx = 零重复帧，契约「重复发送安全」语义保持）。FIX-A1 建连路径负锚天然保持（ctx 是独立消息、建连路径仍零帧）。
- 「ctx 时点 vs 录音引导态」冲突（defer §3.3 警示）分工解法：**front 推、板端消费面 gating**——`mcp_server.cc` VRT_ROUTE_STORY_LIST 现路（:983-1009）无条件 `begin_page` 推 P3 前台，K3 加「记录语境前台（VOICE 层/引导态/录音态）时只装 `g_story_*` 缓存不切前台」门；defer 遗留「真机复验设备侧停弹页面」正目击此行为。
- 残余缺口如实申报（acceptance 冻齿时显式记录取舍、human 过目）：①=ctx 时点后，「翻到 P3 冷浏览（无会话）」仍无列表数据源——契约零改约束下无干净缝；补偿 = 首次开录/归档/删除后可见 + K2 影子缓存跨会话留存。
- 新增正负锚建议：「ctx(story) 首次 → 恰一帧 story_list」「ctx(非 story)/重复 ctx → 零帧」。

### T6 · server 对端核对（architect Q6）

- 三 counterparts 现文与契约零漂移：`_story_route_display_payload`（front_app.py:91-101，control/start_recording → acc.start，与 §1.4 一致且与 §四 ctx 收敛同一幂等入口）；`_story_feed_from_stt`（:104-118，仅累积态旁路喂入不改 wire）；`StoryAccumulator`（story.py:455+，start 幂等 :541-546 / routing / feed 有界环 / stop 空累积 None / timed_out 纯决策 / cancel 幂等）。`_consume_kidpal_control`（:167-188）整条消费无论取值、source_page=="story" 切累积态、delete → 机械执行 + 重推——§四语义零漂移。
- 板端发送半接线的 front 配套 = **仅 T5 ①条件推送**（拦截半零配套，K1 merged 已备）。
- live 三源对账注意：ctx 帧板端拼 `session_id_`、front 消费用连接级 `session_id`——现拓扑（每会话重开 WS）两者同源，风险低，acceptance 声明一次口径。

### T7 · 测试面（architect Q7）

- host 齿 = `firmware/main/test/record_session_live.acceptance.c`（新，名由 acceptance-author 定）五族齿：发送缝（消息形状逐字 + 单点映射闭集 + 双入口结构齿，`story_page_v2.acceptance.c:1742-1756`「发送原语四件套」扫描齿形态先例）+ 引导态（气泡覆写缝 + 语境标志 + VAD 沿切页纯逻辑）+ 手势接线（STOP→listen stop / DISCARD→close 动作映射 + 负锚「STOP 不走 CloseAudioChannel」）+ 回显消费（story 视图回显纯逻辑新缝 + stt 分支接线结构齿）+ 推送 gating（记录语境前台 story_list 只装缓存不推前台）+ `test/host_app/main/CMakeLists.txt` HOST_ACCEPTANCE_SRCS 追加（:181-198 K2 接线先例）。
- server pytest = `server/acceptance/test_m4k3_record_session_live.acceptance.py`（新，K1 命名先例）ctx 条件推送正负锚 + FIX-A1 语义保持 + K1 27 齿零偏移；prompt v8 offline 新件（ADR-001 无字面断言）；单元回归 `server/tests/test_story_units.py`。
- device build 闸沿用 defer 包口径（firmware-build exit 0 + app 分区体积余量落账，先例 = 11% 余量证据）；live 面（八步目击 + 三源日志）不进 offline 齿。
- 通道②基线：defer 包后 = 193 passed / 4 deselected（零新增红口径）。

### changeManifest 精化与零改动面申报

精化判定已回填 taskpackage（protocols 收窄 protocol.{h,cc}；增 application.h/story_page.h/lcd_display.cc；移出 session_state_machine.h）。**预期零改动（不进 manifest）**：`firmware/main/boards/waveshare-s3-rlcd-4.2/**`（P3 前置分流 K2 已落、会话动作可全部从 mcp_server/application 侧发起）、`voice_round.{h,c}` / `kidpal_voice_states.{h,c}`（M1 冻结面，回显缝住 story_page）、`server/gateway/story.py`（routing 自查零改动）、`websocket_protocol.{h,cc}`（SendText 虚实现复用）。
