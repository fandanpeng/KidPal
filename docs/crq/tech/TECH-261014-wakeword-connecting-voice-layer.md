---
id: TECH-261014-wakeword-connecting-voice-layer
title: 唤醒词路径 connecting 态事件被合并跳过致 VOICE 层不推入、语音页整轮不显示
domain: tech
status: deferred
severity: MAJOR
owner: dev-loop
trigger: human 排期后的固件修复包（/triage-change 路由）；或下一个动 application.cc 状态分发 / display 前台桥的包顺带承接
opened: 2026-10-14
paths: [firmware/main/application.cc, firmware/main/display/kidpal_render_node.cc]
source: [c:/tmp/wake_round2.log（2026-10-14 真机被动串口捕获，300s 窗口，完整唤醒轮）, c:/tmp/listen_state.log（同日 240s 被动捕获，对话中段）]
handoff: dev-loop（/triage-change 过比例化 review 后立修复包）；移交触发 = human 排期或下一固件真机批
---

**证据**：2026-10-14 两轮真机串口捕获（COM5 被动模式，kidpal-serial-monitor 规程；原始日志为会话本地文件，关键行逐字引用如下，规程可复现）。

完整唤醒轮（wake_round2.log）逐字关键行：

```
I (150795) E Protocol: Channel timeout 137 seconds          ← WS 通道已断 137s，唤醒必走重连
I (150795) StateMachine: State: idle -> connecting
I (150835) WS: Connecting to websocket server: wss://8.137.91.16:8000 with version: 1
I (154065) WS: Session ID: 83e724a8-cb18-43d6-98a0-270f1a0e67ff   ← OpenAudioChannel 阻塞 3.2s
I (154065) Application: Wake word detected: 你好小智
I (154125) StateMachine: State: connecting -> listening     ← SetListeningMode 直接推到 listening
（全文件 grep "voice_begin|voice_end" 零命中 —— voice_begin 日志行 "voice_begin: new round" 未出现）
I (156545) kidpal_rn: chrome_front layer_top=2 force_hide=0 front=0   ← 唯一一次前台桥触发来自 show_home
I (156615) kidpal_rn: show_home: LOCAL_PAGE front (layer top=2)
```

对话中段（listen_state.log）逐字关键行：`chrome_mid_sync mid_mode=1 front=0 reminder=0 force_hide=0 voice_live=0`（15 次全部 front=0）+ `StateMachine: listening -> speaking -> listening` 正常翻转 —— 状态机与音频链路完好，仅层前台维度假。

代码锚点（本仓 worktree dev/260915-m1-k1a-key-layer-dispatch @ 0baab93）：

- [application.cc:171-173](../../../firmware/main/application.cc) `SetDeviceState` 仅 `TransitionTo`（置事件位）；
- [application.cc:206-208](../../../firmware/main/application.cc) 状态变化经事件组**位**通知（level-triggered，同位合并）；
- [application.cc:980-984](../../../firmware/main/application.cc) 唤醒词处理器在主循环内联跑：`SetDeviceState(kDeviceStateConnecting)` 置位后 `OpenAudioChannel()` 阻塞（本轮 3.2s TLS 重连），随后 `SetListeningMode` 直接推到 listening —— 主循环回到事件消费点时两态已合并；
- [application.cc:1014-1019](../../../firmware/main/application.cc) `HandleStateChangedEvent` 以 `GetState()` 读**当前态**跑一次 switch —— connecting 分支被整个跳过；
- [application.cc:1064](../../../firmware/main/application.cc) `kidpal_render_node_voice_begin()`（VOICE 层唯一事件驱动推入点）只在 connecting 分支调用；listening 分支（1066-1091）零压层逻辑；
- [kidpal_render_node.cc:599-624](../../../firmware/main/display/kidpal_render_node.cc) `voice_begin` 推 VOICE 层，自注释「push 幂等 + 按优先级插入，故多次触发安全」；
- [kidpal_layer.h:43-44](../../../firmware/main/display/kidpal_layer.h) `KIDPAL_LAYER_LOCAL_PAGE=2 / KIDPAL_LAYER_VOICE=3` —— 日志 top=2 即语音层从未上台。

human 同日目击：KEY 单击可正常进入聆听页（KEY 路径走 [waveshare-s3-rlcd-4.2.cc:127](../../../firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc) `show_chat` 直推 VOICE 层，不经事件分发）——双路径对照与诊断闭环。

**影响面**：触发条件 = 唤醒词发起 + WS 通道已断（闲置超 137 秒后必断，`Protocol: Channel timeout 137 seconds`）。触发后整轮对话语音页（聆听/说话整页骨架，含键帽条语音态）不显示，设备停留在首页但对话功能（ASR→LLM→TTS、状态机翻转）完全正常。日常「隔一会儿再叫小伴」即命中，频率高、用户感知为「设备没听见/没反应」。KEY 单击路径不受影响（有绕行法）。技术债根子 = 事件组位 level-triggered + switch 读当前态的合并语义，任何「中间态的副作用」都可能在快速连跳时被吞。

**建议改动**（2026-10-14 代码级评估后改形：原「listening 分支裸调 voice_begin」形态否决——`voice_begin` 非副作用幂等，每次无条件 pop SERVER_PAGE + REMINDER〔[kidpal_render_node.cc:599-624](../../../firmware/main/display/kidpal_render_node.cc)，自注释「幂等」只覆盖层栈 push 语义〕，轮首清场语义被错用到非轮首进入点：对话中段 speaking→listening 回切会把当轮服务端回答页（如 view=schedule，本条证据日志即含实例）在语音播完瞬间弹掉，违反回答页驻留冻结口径 OQ3=c；查字窗为独立 CHAR_WINDOW=5 层、不在 pop 清单，不受此误伤）：

- **候选 A″（薄修，推荐）**——`HandleWakeWordDetectedEvent` idle 分支、通道判断**之前**直接执行轮首 UI 三件：聊天清屏（`kidpal_layer_chat_clear_event(KIDPAL_LAYER_EVT_NEW_VOICE)` + `SetChatMessage("system", "")`，即 connecting 分支〔[application.cc:1058-1062](../../../firmware/main/application.cc)〕同款语义）+ `kidpal_render_node_voice_begin()`，与 KEY 路径入口直推 `show_chat` 先例（[waveshare-s3-rlcd-4.2.cc:127](../../../firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc)）同构。置于通道判断前可一并覆盖「通道已温直跳 listening」子路径（[application.cc:980-986](../../../firmware/main/application.cc)：`IsAudioChannelOpened()` 为真时不设 connecting——该子路径**无合并也照样漏推**，聊天清屏同样漏，为同症状第二活实例）。不碰 listening 分支 → 对话中段回切与提醒场景行为原样，零新增交互面。注意：合并路径跳过 connecting 分支的同时也跳过了聊天清屏——只补压层不补清屏，语音页显形后上一轮对话回显文字会残留成新可见 bug，故三件须同补。
- **候选 A′（备选）**——listening 分支补**守卫式** ensure 语义（仅 VOICE 层不在栈内才执行，聊天清屏放同一守卫）：对任意未来漏推路径自愈，但「提醒播报后 listening 进入」（TECH-261014-post-speak-listening-lingering 路径）VOICE 缺失仍会触发 voice_begin 的 REMINDER pop、违反 S-F1.4「驻留到按键」，需再加「REMINDER 在顶不动」守卫或依赖同包 lingering 修复消除该进入点。
- **候选 B（否决，维持原判）**——把 STATE_CHANGED 事件位改为逐态队列重放：动主循环核心分发机制，为单个 UI bug 不成比例，且波及所有状态消费方。
- **复验清单（评估后扩）**：真机轮除「闲置 >137s 唤醒 → 语音页应切 + KEY 路径回归对照」外必加四条——① 对话中服务端回答页（view=schedule 类）语音播完后仍驻留；② 查字窗播完后驻留；③ 提醒弹窗播完驻留到按键；④ 新唤醒轮不残留上一轮聊天文字。③④ 与 TECH-261014-post-speak-listening-lingering 复验轮重叠，同包一轮全覆盖。附带观察项两条（与本条同批测出，独立登记或随修复包顺带查）：① wake_round2.log 155765 段——服务端 `story_list_dispatch`（items=1）+ `display_dispatch view=schedule`（courses=39）与 `listening -> idle` 异常弹跳同帧，其后 `idle -> speaking`；② WS 重连 3.2s 期间 `AFE: Ringbuffer of AFE(FEED) is full` 刷约 50 行——重连路径音频喂入积压。

**风险**：riskTier: low —— 候选 A″ 为入口直推（KEY 路径同构先例）、无新状态、不改层栈与事件机制；复验便宜（真机轮清单见建议改动末段，与 post-speak-listening-lingering 复验轮重叠）。Track B 三要件：① 执行测出 = yes（两轮真机串口捕获，见证据）；② 问题清晰修复明确无真设计分叉 = yes（候选 B 已有明确否决理由，非悬而未决分叉）；③ 爆炸半径已被执行揭示 = yes（触发条件、影响范围、绕行路径均由真机实测确认）。review profile = Track B；裁定人 = 各面 owner（固件面）。

---

**触发留痕（2026-10-14）**：触发条件成立——同日 /triage-change 比例化 review 过门，三条合流立单包 **`261014-voice-round-entry-exit-fixes`**（②单包 Full / riskTier medium / sourceType regression，architect 裁决 + human 触点①预授权确认；reviewed plan 落 `delivery/taskpackages/261014-voice-round-entry-exit-fixes/plan.md`）。修法采纳本条**候选 A″ 薄修**（置于唤醒词 idle 分支通道判断之前、聊天清屏+voice_begin 同补；A′/B 维持否决）。条目状态保持 deferred，闭账条件 = 包 merge + 真机复验轮（条目复验清单已并入包 plan 真机节，去重合并）。

**真机联测留痕（2026-10-14，同日）**：首轮真机（`/c/tmp/live-261014-round1.log`）确认 **A″ 生效**（`voice_begin: new round (top=3)` 精确出现在 `idle -> connecting` 之前）。同轮暴露两个新根因（均由本包改动引入/放大），立**补充轮 261014-r2**（同包 append，plan.md「补充轮 261014-r2」节）：① `speaking_round_entered_from_` 采样写点在逐句 sentence_start 分支被覆写 → 判据③ 误伤全部多句对话轮（主因，同包返修）；② 冷通道 `OpenAudioChannel()` 裸析构旧 WebSocket → 断连回调误判为服务端断、迟到打回 Idle（次因，websocket_protocol.cc 修）。

**本条目「附带观察项」处置（2026-10-14 更新）**：
- **观察项①（story_list_dispatch + display_dispatch 与 listening→idle 异常弹跳同帧）＝根因已定**：即上述次因②的毛刺，两个 MCP 推送时间巧合、非因果——**由补充轮 261014-r2 的 ②c 修法闭账**。
- **观察项②（WS 重连 3.2s 期间 AFE Ringbuffer full 刷屏）＝根因已定**：冷重连的 `OpenAudioChannel()` **阻塞主循环**（TLS 握手期）致音频喂入积压；`OpenAudioChannel` 阻塞形态本包未动（②c 只改旧通道拆除分类），**登记为独立残余项**（「主循环 TLS 重连阻塞」），不扩本包。
