---
changeId: 261014-voice-round-entry-exit-fixes
title: 语音轮入口/出口三缺口修复——唤醒词合并路径 VOICE 层直推 + 查字/提醒播报后不回聆听 + engine-patch 误 await
rigor: full
riskTier: medium
sourceType: regression
reviewed-by: architect (/triage-change 分流 review，fork② 真分叉裁设备侧——human 授权 architect 裁 + 主线程确认)
date: 2026-10-14
---

# 根因与来源

三条根因、证据链、代码锚点已在 CRQ 条目内闭环，本 plan 不复制，逐条引用：

1. 唤醒词 connecting 态事件被合并跳过致 VOICE 层不推入 —— [TECH-261014-wakeword-connecting-voice-layer](docs/crq/tech/TECH-261014-wakeword-connecting-voice-layer.md)（修法取条目内**候选 A″ 薄修**，A′/B 及否决理由见条目）。
2. 查字/提醒播报后设备滞留聆听态（会话出口三缺口中的 ①②）—— [TECH-261014-post-speak-listening-lingering](docs/crq/tech/TECH-261014-post-speak-listening-lingering.md)（真分叉②本 plan 裁**设备侧**，理由见下「fork② 裁定」节）。
3. engine-patch 对同步 speak_txt 误 await —— [TECH-261014-kidpal-speak-await-nonetype](docs/crq/tech/TECH-261014-kidpal-speak-await-nonetype.md)。

# 包边界

## 动

- `firmware/main/application.cc`（唯一固件实现文件）：唤醒词 idle 分支（A″）+ tts stop Schedule lambda（出口守卫）+ sentence_start 处轮来源采样（fork② 判别器）。
- `server/deploy/engine-patch/connection.py`：`_kidpal_try_speak` 内一处 await 改同步调用（connection.py:383-390）。**human 已裁执行边界：只 commit 到分支，不部署 ECS；联测时人工部署。**
- `firmware/main/test/voice_round_ui.acceptance.c`：新增齿/齿区重锚（须授权链 append REVISION LOG，同 261013 先例模式；其 VQ-E1 冻结齿〔:1081，60 秒 timer 零活侧 producer〕**本批不接线不受扰、须保持绿**）。
- （acceptance-author 酌情）新纯逻辑 seam 文件 + `firmware/main/test/host_app/main/CMakeLists.txt` 接线。

## 不动（显式零触碰面）

- `firmware/main/display/kidpal_render_node.cc` / `kidpal_layer.*` / 各板级驱动（A″ 只在调用点接线，voice_begin 与 chat_clear_event 语义零改）。
- `firmware/main/session_state_machine.cc`——60 秒 timer 接线是已另案登记的 Phase 1 项（requirements.md:47 ⚠️），本批**不得**顺带接线。
- wire 协议零变更：不新增、不修改任何引擎↔设备消息（fork② 裁设备侧的直接推论）。
- `docs/requirements.md` / `docs/architecture.md` / `docs/contracts/**`：零 diff（修法全部实现既有口径：requirements.md:39 / :295 / :300 / :652）。
- 邻接未结 CRQ 不并入：TECH-261012-edgetts-403-hotpatch 耐久化、TECH-261012-first-keypress-no-listening、60 秒 timer 项（裁定留档见分流 review）。

# fork② 裁定：设备侧（真分叉收口）

**裁设备侧——对无会话上下文的 speak 不回 auto-listen。** 承重依据（完整论证见分流 review §C，此处存目）：① 决定性机制事实——引擎从不向设备发 listen 类消息（上游 listenMessageHandler.py 只收不发；设备 OnIncomingJson 无 listen 分支，application.cc:682-799），「引擎侧 listen stop」生效前提是设备新增 wire 消费＝两侧同动 + 新消息语义（命中跨系统结构性升级判据，本批将升 Track A）；auto-listen 决策点整个住在设备 tts stop handler（application.cc:693-702）。② 连续追问（§2.1，requirements.md:39）保护：判别器 = 「Speaking 轮是否从 Listening 进入」，对话轮结构性恒从 Listening 进入、不可误伤；引擎侧迟发 stop 有竞态误伤面。③ ECS 部署延后：设备侧本批真机轮即可全量验证；引擎侧须等人工部署、验证拆场。④ 半径/回滚：~15 行单 handler、app 单分区重烧 vs 引擎 patch 手动部署 + 容器重启。

# 修复

## 改动点 1：唤醒词 A″（application.cc HandleWakeWordDetectedEvent idle 分支，:970-1003）

在 K6 auto-connect in-flight 守卫与 `EncodeWakeWord()` 之后、`if (!protocol_->IsAudioChannelOpened())` **之前**，直接执行轮首 UI 三件（与 connecting 分支 :1058-1064 同款语义、与 KEY 路径入口直推 show_chat 同构）：
1. `kidpal_layer_chat_clear_event(KIDPAL_LAYER_EVT_NEW_VOICE)` 为真时 `SetChatMessage("system", "")`（聊天清屏——合并路径同时漏掉了它，只补压层不补清屏会显形后残留上轮回显文字成新可见 bug，三件须同补）；
2. `kidpal_render_node_voice_begin()`（VOICE 层推入；层栈 push 幂等）。

置于通道判断前 = 同时覆盖「通道已温直跳 listening」子路径（:980-986，无合并也照样漏推的第二活实例）。失败路径语义与现状 connecting 分支一致（连接失败时语音页已推、与今日行为对齐）。**不碰 listening 分支**——对话中段 speaking→listening 回切与提醒场景行为原样，零新增交互面（候选 A′ 的守卫式 ensure 及其 REMINDER pop 顾虑因此整支不触发）。

## 改动点 2：tts stop 出口守卫（application.cc :693-702 的 Schedule lambda 区域）

现行逻辑：`Speaking + listening_mode_==ManualStop → Idle；否则 → Listening`。改为三守卫（序即优先级）：

1. **ManualStop（既有）**：不变。
2. **查字收尾（lingering ①）**：tts stop 时查字窗在前台（判据实现期决策：`kidpal_render_node_front_layer()==KIDPAL_LAYER_CHAR_WINDOW` 或 view=char 驻留态查询，acceptance-author 定）→ 结束会话：`protocol_->SendStopListening()`（引擎侧 ASR/VAD 收到标准收尾、防 24h 兜底下会话悬置）+ `SetDeviceState(kDeviceStateIdle)`；查字窗驻留到按键/5 分钟兜底语义**零改**（requirements v1.37，:652）。
3. **出会话 speak 不回聆听（fork② 设备侧，lingering ②）**：sentence_start（:715 SetDeviceState(Speaking) 之前）采样 `speaking_round_entered_from_ = GetDeviceState()`；tts stop 处消费——`entered_from != kDeviceStateListening` → `SetDeviceState(kDeviceStateIdle)`（不发 listen start、不开麦；提醒弹窗驻留与提示音不受扰，Idle 分支既有 voice_end/REMINDER 层管理承接前台回落）。每轮 sentence_start 重置采样；abort 路径同现行 aborted_ 语义。

**建议（非硬性）**：出口目标态判据（listening_mode / char_front / entered_from 三入参 → 目标态）抽纯逻辑 seam（kidpal_voice_round_scene 同款先例），host 验收可直接断言。回 Idle 触发的 Idle 分支副作用（voice_end、story session end、RaiseReminderQueue）在提醒/查字两场景均为幂等 no-op 或预期行为，acceptance-author 推演留痕。

## 改动点 3：engine-patch 误 await（connection.py :383-390）

`await speak_txt(self, text.strip())` → 同步调用 `speak_txt(self, text.strip())`（或按返回值是否 coroutine 判别以兼容引擎未来改异步，builder 取简形即可）。外层 try/except 降级语义（V20「弹窗+提示音必达」）保持不变。

## 执行边界（human 已裁，随包留痕）

- engine-patch 改动**只 commit 到分支，不部署 ECS**；联测（复验清单第 3 项引擎侧子项）时 human 人工部署。
- 固件烧录：纯代码变更、无 spiffs 资产变更，仅烧 app 分区即可。

# 验收要点

## host（相关子集，不全量）

1. `voice_round_ui.acceptance.c` 全绿：新增齿钉改动点 1/2 的接线在位（A″ 三件套在通道判断前、tts stop 三守卫在位——源文本 structural 齿先例见该文件 :901/:1198）；若抽纯 seam，行为齿按 R<T<F 标定。齿区若因 application.cc 邻近改动需重锚，REVISION LOG append 授权链（本 TaskPackage + human 触点①确认）。
2. VQ-E1 冻结齿保持绿（60 秒 timer 零活侧 producer 事实未被本批改变——本批不接线 timer）。
3. `record_session_live.acceptance.c` 零改动回归绿（会话收尾缝邻接面 canary）。
4. engine 项 host 断言：以 stub speak_txt（同步、返回 None）驱动 `_kidpal_try_speak`，断言无异常、返回 True、认领路径不回归（`server/acceptance/archive/test_g_relay_fix.acceptance.py` 有既有 harness 模式可循；落 `server/tests/` 或 `server/acceptance/` 由 planner/acceptance-author 定）。
5. 相对有效基线零新增红。
6. **all-mock 申报**：本包计划含真机轮 + host structural/行为齿，**不应**是 all-mock 包；若验证期机械闸判 all-mock（如真机轮证据未落），按既有纪律升 riskTier high → human 放行，不得静默。

## 真机（判定标准；human 在场，串口捕获 kidpal-serial-monitor 规程；三条复验项去重合并）

1. **唤醒词入口（wakeword 条目主验 + 扩版四条）**：闲置 >137s（通道必断）后喊唤醒词 → 语音页整轮显示（VOICE 层上台）+ 聊天清屏、无上一轮残留文字；KEY 单击路径回归对照（不受扰）。通道已温（常驻窗口内）立即唤醒 → 语音页同样显示（A″ 覆盖的第二活实例）。
2. **连续追问回归（fork② 不误伤的判定面）**：正常问答 → 播完设备回聆听、可连续追问（§2.1）；多轮追问全程语音页在位。
3. **提醒轮（lingering ② + engine 项闭账）**：提醒到点 → 提示音 + 弹窗 + 语音播报 → 播完设备**回 idle 不回聆听**（串口无 `listen start mode=auto` 上报、麦克风关闭）+ 弹窗驻留到按键（AC1/AC6）+ 环境对话不再被接进会话（AI 插话实证不复现）；顺带定死「设备侧是否真出声」（条目遗留观察项）。引擎侧子项（无 `kidpal_speak 播报注入失败` ERROR 且语音照常下发）**延后至 human 人工部署 ECS 后同轮闭账**，为 exit criterion 非本轮验收门。
4. **查字轮（lingering ①）**：问「X 怎么写」→ 查字窗 + 播报 → 播完**不回聆听**、查字窗驻留不受影响（关窗仍可用）；想查下一个字须关窗后说全句（v1.37 口径）。
5. **驻留面合并回归（wakeword 扩版①②③④与 lingering 重叠去重）**：对话中服务端回答页（view=schedule 类）播完仍驻留；查字窗播完驻留；提醒弹窗播完驻留到按键；新唤醒轮不残留上一轮聊天文字。
6. **附带观察项两条（wakeword 条目同批测出，只查不改）**：wake_round2 155765 段 story_list_dispatch + display_dispatch 与 listening→idle 异常弹跳同帧现象；WS 重连 3.2s 期间 AFE Ringbuffer full 刷屏。发现即登记，不扩包。

# 回归风险

1. **判别器错杀连续追问**（fork② 最大风险）：结构性不可达（对话轮恒从 Listening 进入 Speaking），但 acceptance-author 须推演 abort 后新一轮、realtime（AEC）模式、ManualStop 模式三边角并在齿中钉住。
2. **提醒播报中用户恰起对话**：出会话 speak 守卫按 entered_from 判——提醒 speak 若在 Listening 中插入（front 注入不看设备态），行为同今日（回 Listening），不劣化；engine 侧 kidpal_speak 与对话 TTS 交错的治理不在本批。
3. **查字窗显示晚于 tts stop 的竞态**（display_dispatch 与 TTS 时序）：守卫漏触发则维持现状（回聆听）——已知残余，非新缺陷；wire 时序（display 先行）由既有网关保证。
4. **冻结件重锚面**：voice_round_ui.acceptance.c 邻区齿若需随 application.cc 改动重锚，逐齿推导表随包留痕（261013 先例）。
5. **engine 项部署时点漂移**：commit-only 状态下若容器因故重建（edgetts 热补丁丢失场景，TECH-261012 同族风险），engine 修复随部署源漂移暴露——联测部署时一并核 manifest。

# 非目标 / 已知残余（不扩本包）

1. 60 秒 timer 活侧 producer 接线（Phase 1 已登记项；其缺失使本批修复后的残余滞留仍可达「无限期」——查字守卫的竞态残余 §回归风险 3 即被它放大）。
2. TECH-261012-edgetts-403-hotpatch 耐久化（触发条款名义命中本批，裁定留给真正的 ECS 部署动作批；如 human 愿意随联测部署一并落，另起 Lite 小包或补充轮，不混入本包）。
3. TECH-261012-first-keypress-no-listening（会话生命周期同族，独立批）。
4. 观察项两条（见真机 6）只登记不修。
5. 提醒语音与活跃对话 TTS 交错的引擎侧治理。

# changeManifest（预定改动面，仓根相对）

- firmware/main/application.cc
- server/deploy/engine-patch/connection.py
- firmware/main/test/voice_round_ui.acceptance.c
- firmware/main/test/host_app/main/CMakeLists.txt（仅当新增 seam 文件时）

# impactedTests 种子

- firmware/main/test/voice_round_ui.acceptance.c（新增齿 + 可能的齿区重锚；VQ-E1 齿保持绿）
- firmware/main/test/record_session_live.acceptance.c（零改动 canary）
- firmware/main/test/host_app/main/CMakeLists.txt（接线面在册确认）
- server 侧 host 单测（_kidpal_try_speak stub 断言；落点 planner 定）

# crossEnd 种子（planner 消费）

- counterparts: firmware/main/application.cc（tts stop 出口守卫 / sentence_start 轮来源采样）↔ server/deploy/engine-patch/connection.py（_kidpal_try_speak）——提醒语音链两端（kidpal_speak 下行 / 设备收尾），本批改动不触碰 wire 语义、两端行为契约各自闭环。

---

# 补充轮 261014-r2（2026-10-14 真机联测后立；architect 重裁，human 授权 architect 裁 + 主线程确认）

## 触发

首轮真机联测（human 持板 + 串口全程捕获 `/c/tmp/live-261014-round1.log`）暴露两个正交根因，均由本轮改动面引入或放大；本补充轮 = 返修 + 闭掉既有观察项。**rigor: lite（按 human 裁定，见「载体」节）；riskTier: medium**（②c 触共享断连分类路径，不宜标 trivial）。

## A1（主因，CONFIRMED）· 轮来源采样逐句覆写

`sentence_start` 是**逐句**消息（application.cc:725-751），而 `speaking_round_entered_from_ = GetDeviceState()` 写在同一分支内（:740，紧邻 :741 `SetDeviceState(kDeviceStateSpeaking)`）——注释自称「每轮重置 single write point」，**实为逐句写点**：

- 第 1 句：状态仍 Listening → 采样 = Listening（正确）；
- 第 2 句起：状态已 Speaking → **覆写采样 = Speaking**。

收尾判据③（:718）吃到被覆写的 Speaking → 对话轮回 Idle。**凡多句回答的轮次播完即结束会话**（真机证据：7 句妖怪轮 / 5 句天气轮末段 `speaking -> idle`；单句打招呼采样正确、行为正常）。

**修法（候选 3，优于 tts start 采样与状态机侧改造）**——:740 前加一行守卫：

```c
if (GetDeviceState() != kDeviceStateSpeaking) {
    speaking_round_entered_from_ = GetDeviceState();
}
SetDeviceState(kDeviceStateSpeaking);
```

「逐轮一次」由「状态离场 Speaking 才重采」自然保证：轮间 tts stop 已把状态推出 Speaking，下一轮首句必重采；abort 后同样自然重采，与原注释意图一致。

## A2（次因，CONFIRMED）· 冷通道旧回调迟到打回 Idle

唤醒时通道超时（`Protocol::IsTimeout()` 120s；`IsAudioChannelOpened()` 为 false 但 TCP 未断）→ idle 分支 `OpenAudioChannel()` 阻塞主循环于 TLS 握手（:1018-1024；websocket_protocol.cc:224）；其间 `CreateWebSocket(1)`（:102）裸重赋值析构旧 WebSocket → 其断连回调走 `local_close_pending_ == false` 路径（该标志刚在 :90 被复位）→ 误判「服务端/网络侧断」→ `Schedule(SetDeviceState(Idle))` 积压到主循环解阻后才跑 → 压在新会话 `connecting -> listening` 之后，产生 `listening -> idle` 毛刺。两处核验：`TryStartAutoConnect()` 该时刻为 no-op；**无通道代次判据**。附带观察项①（story_list_dispatch/display_dispatch 与 listening→idle 同帧）＝同一根因，MCP 推送时间巧合、非因果。

**修法（②c）**——websocket_protocol.cc `OpenAudioChannel()` 顶部、:90 `local_close_pending_ = false` **之前**加：

```c
if (websocket_) {
    CloseAudioChannel();   // 旧通道按本地关分类 → 抑制 stale OnDisconnected
}
```

复用既有 `local_close_pending_` 分类；`CloseAudioChannel()` 的 `websocket_.reset()` 同步等 receive task 退出（esp_ssl.cc:77 上限 10s），stale 回调当场抑制、不再延后入队。真服务端断分支（`local_close_pending_ == false` 路径）零改动。

## A3 · 正交矩阵（缺一不可）

| 修的 | 未修 | 残留错 |
|---|---|---|
| A1 | A2 | 冷通道毛刺仍把首句前状态打成 Idle → 判据③ 回 Idle（冷通道单句打招呼仍错） |
| A2 | A1 | 暖通道多句轮第 2 句覆写 Speaking → 回 Idle（连续追问仍错） |

## 明确不做（残余登记，不扩本包）

1. **F2 查字轮漏一轮 ASR**：判据② 设备侧已正确生效（SendStopListening + Idle + 查字窗 top=5 驻留）；「Hmm」音频在聆听期已送达引擎、stop 不可撤回 → 属引擎侧「stop 后丢弃 in-flight audio」缺失，设备侧无解。主线程登记 CRQ 残余项。
2. **阻塞式 OpenAudioChannel 未动**：冷重连 3.2s 主循环阻塞仍在（附带观察项② AFE Ringbuffer full 由它而来），②c 不解决 → 独立项登记。
3. **MQTT 协议同族潜伏**：mqtt_protocol.cc 若同构则同有 stale 回调隐患；本包只修 WS（现役方向 B 路径），登记不动。

## changeManifest 增量

- `firmware/main/application.cc`（A1，在原 manifest 内）
- `firmware/main/protocols/websocket_protocol.cc`（A2，**新增**）
- `firmware/main/test/voice_round_ui.acceptance.c`（新增齿）

## 验收要点（补充轮）

### host 齿

1. **A1 结构齿**（形态 1，零新文件，与既有 structural 齿同款）：切 application.cc sentence_start Schedule lambda 内采样区，断言写点受 `!= kDeviceStateSpeaking` 守卫；另加负向断言防 builder 把守卫误删回无条件逐句写。
2. **A2 结构齿**：切 websocket_protocol.cc `OpenAudioChannel` 函数体，断言 `CloseAudioChannel()` 出现在 `local_close_pending_ = false` 与 `CreateWebSocket` 之前。
3. （acceptance-author 可选）形态 2 行为齿：抽纯 seam `kidpal_voice_round_sample_entered_from(cur, keep)`，host 三断言 R<T<F：`(Speaking,Listening)->Listening`、`(Listening,X)->Listening`、`(Idle,X)->Idle`。
4. VQ-E1 冻结齿保持绿；相对有效基线零新增红。

### 真机复验（human 持板；冷/暖双路径 + 回归）

1. **暖通道多句连续追问（A1 必现路径）**：多句回答轮末段须 `speaking -> listening`（非 Idle）、可连续追问、语音页全程在位。
2. **冷通道单句唤醒（A2 必现路径）**：闲置 >137s 后唤醒 → 无 `listening -> idle` 中间毛刺，`connecting -> listening -> speaking -> listening` 直进。
3. **KEY 单击冷通道回归**（②c 同路径）：闲置 >137s 后 KEY → 无毛刺。
4. **服务端/网络侧断连回归（②c 关键回归面）**：会话中服务端关会话或短暂断网 → 仍落 Idle + 自动重连。
5. **提醒轮回归**：提醒 speak 从 Idle 插入 → 播完回 Idle 不回聆听、弹窗驻留（判据③ entered_from=Idle 正向）。
6. **查字轮回归**：播完回 Idle、查字窗驻留；F2 只记录不复现要求。

## 机械口径

- 原包已 supervisor APPROVE（write-once）→ 补轮须**先显式删旧 `supervisor-verdict.json`** 再重跑闸（write-once 先例）。
- 原包已烧录/部署的制品被本补轮取代：补轮绿后重烧 app 分区（engine-patch 无增量改动，无需重新部署）。
