---
id: TECH-261014-post-speak-listening-lingering
title: 查字/提醒播报后设备滞留聆听态——会话出口三缺口：查字收尾缝零接线、提醒语音链尾随 auto-listen 把设备拉进会话（叠加已登记的 60 秒 timer 零 producer），实证后果 = AI 插话家庭对话
domain: tech
status: deferred
severity: MAJOR
owner: dev-loop
trigger: human 排期后的固件修复包（/triage-change 路由，与 TECH-261014-wakeword-connecting-voice-layer 同包分流——后者移交条款「下一个动 application.cc 状态分发的包顺带承接」正好覆盖本条）；或阶段一「问答演出补齐」批一并带
opened: 2026-10-14
closed: ""
closedBy: ""
paths: ["firmware/main/application.cc", "firmware/main/session_state_machine.cc", "server/deploy/engine-patch/connection.py"]
source: 2026-10-14 human 目击两项（查字大字页后始终聆听 / 提醒弹页处于聆听态且自述无播报）+ 同日 ECS 引擎容器 24h 日志三轮提醒全轨迹（20:15 / 20:30 / 07:45，含提醒后接环境对话实证）+ spec/arch 代码锚对账
handoff: dev-loop（/triage-change 过比例化 review 后立修复包，建议与 TECH-261014-wakeword-connecting-voice-layer 同批）；移交触发 = human 排期或下一固件真机批
---

**现象（human 2026-10-14 目击两项）**：① 问完「字怎么写」，查字窗大字显示后设备始终处于聆听态（可听人说话）；② 提醒到点弹出提醒页，设备同样处于聆听态、可对话，但 human 未听到语音播报。

**设计口径（两处均不匹配）**：

- 查字：[requirements.md:652](../../../docs/requirements.md)（v1.37，§7 开放问题 1 收窄裁定）——「**查字窗播完会话即结束、不回聆听态**（无连续会话即无指代场景）」，想查下一个字须关窗后说全句。通用层另由 §2.1 会话结束三通道（60 秒超时 / 结束意图 / 按键）兜底，其中 60 秒超时的实现侧对账（[requirements.md:47](../../../docs/requirements.md) ⚠️ 注记）已登记为 Phase 1 修复项：设备侧 `SESSION_TIMEOUT_60S` 零活侧 timer 生产者、引擎侧兜底改 24h——**真机 60 秒自动关闭从未发生**。
- 提醒：S-F1.4 AC1（[requirements.md:295](../../../docs/requirements.md)）= 提示音 + 语音读出提醒文案 + 弹窗驻留；AC6（[requirements.md:300](../../../docs/requirements.md)）= 提醒为最高层覆盖、**置顶期间语音输入按提醒窗语义、不穿透底层窗口**——设计里提醒是覆盖层、无会话语义，播完不该把设备留在聆听态。

**证据链（ECS 引擎容器 docker logs，24h 窗口，三轮提醒同模式）**。机制 = 提醒语音出音借引擎对话出音链（arch §5.2 场景⑤：front 注入 `kidpal_speak` 内部指令 → 引擎 `speak_txt` 出音），而该链的天然尾巴是小智原生多轮行为——**播完设备自动回聆听（listen start，mode=auto）**：

```
261013 20:15:00 [core.connection]-ERROR-kidpal_speak 播报注入失败: object NoneType can't be used in 'await' expression (提醒降级：弹窗+提示音仍必达)
261013 20:15:02 [core.handle.sendAudioHandle]-INFO-发送第一段语音: 20:30 的英语外教口语课
261013 20:15:08 [core.handle.textMessageProcessor]-INFO-收到listen消息：{"session_id":"2b075912-5417-4e55-bc20-093f4dbe199e","type":"listen","state":"start","mode":"auto"}
```

```
261014 07:45:01 [core.handle.sendAudioHandle]-INFO-发送第一段语音: 18:40 的书法课
261014 07:45:07 [core.handle.textMessageProcessor]-INFO-收到listen消息：{"session_id":"454d71c1-8606-4723-b47f-bc35afe44db0","type":"listen","state":"start","mode":"auto"}
```

**实证后果（human 未报、日志带出，比目击更严重）**：20:15 提醒播完后设备留在聆听态，把旁边大人的环境对话接进会话并持续应答（同一 session_id 贯穿）：

```
261013 20:16:06 [core.handle.sendAudioHandle]-INFO-发送第一段语音: 我不太明白你说的「缺一个」「不穷」是啥意思呀——我这边看不到你手里那样东西
```

其后数轮应答延续至 20:30 提醒之后。即「提醒后滞留聆听」的实际产品后果 = **AI 主动插话家庭对话 + 麦克风常开**，且为机制性——每轮提醒必发（speak 链尾随 auto-listen 是引擎/设备原生行为，非偶发）。

**代码锚点（本仓 worktree dev/260915-m1-k1a-key-layer-dispatch）**：

- [voice_round_ui.acceptance.c:1081](../../../firmware/main/test/voice_round_ui.acceptance.c) VQ-E1 冻结齿钉死「60 秒 timer 零活侧 producer（`SES_EVT_SESSION_TIMEOUT_60S` 仅存于纯模型两文件，deferred 非 dropped）」；
- [application.cc:1032-1036](../../../firmware/main/application.cc) 全仓唯一会话收尾缝 = 记录会话的 `kidpal_story_session_end`，查字无对应物——「播完即结束」零接线；
- [connection.py:363-390](../../../server/deploy/engine-patch/connection.py) `_kidpal_try_speak` 认领提醒语音后转调 `speak_txt`，无任何「播完不回聆听」的收尾动作。

**修法方向（归 triage 裁，含一个真分叉）**：① 查字收尾——设备侧补缝：`view=char` 显示中 + TTS 播报结束 → 结束会话（窗驻留到按键/5 分钟兜底语义不变，仅关语音通道），触发器判据为实现期决策但设计口径明确（v1.37）；② 提醒后不回聆听——**二选一真分叉**：引擎侧（engine-patch 在提醒 speak 后补发 listen stop / 会话收尾指令）vs 设备侧（对无会话上下文的 speak 不回 auto-listen，需能区分提醒 speak 与对话 speak），排包时裁；③ 通用 60 秒 timer 接线已另案登记（spec §2.1 ⚠️ Phase 1 修复项），本条不重复立案、只作叠加语境——它的存在使 ①② 的滞留从「一小会儿」放大为「无限期」。

**复验**：提醒轮带串口捕获 + 人在场——修后提醒播完设备应回 idle（不回聆听）、弹窗驻留与提示音不变；顺带定死「设备侧是否真出声」：引擎侧已证三轮均合成下发（上引 07:45 轮两段「18:40 的书法课」「提前 15 分钟准备哦」），设备侧播放无证据、human 目击为没听到，本轮一并闭账。查字轮修后播完即结束会话（不再聆听）、查字窗驻留不受影响。

**风险**：riskTier: medium——①为局部新增收尾触发；②触及「播完回聆听」这一小智原生多轮底座语义，须区分提醒 speak 与对话 speak、改错会伤正常问答连续追问（§2.1「继续聆听；可连续追问」），且设备侧/引擎侧二选一为真分叉（存疑向严）。Track B 三要件：① 执行测出 = yes（引擎日志三轮 + 插话实证 + human 目击）；② 问题清晰但修复有真分叉（见②）；③ 爆炸半径已执行揭示 = yes（每轮提醒必发、插话后果实证）。review profile = Track B（分叉在包计划期裁，非悬而未决的设计空洞）；裁定人 = 各面 owner（固件面 + 引擎部署面）。

---

**触发留痕（2026-10-14）**：触发条件成立——同日 /triage-change 比例化 review 过门，三条合流立单包 **`261014-voice-round-entry-exit-fixes`**（②单包 Full / riskTier medium / sourceType regression；reviewed plan 落 `delivery/taskpackages/261014-voice-round-entry-exit-fixes/plan.md`）。**真分叉②已裁：设备侧**（human 授权 architect 裁 + 主线程确认）——决定性机制事实：引擎从不向设备发 listen 类消息（上游 listenMessageHandler.py 只收不发、设备 OnIncomingJson 无 listen 分支），「引擎侧补 listen stop」生效前提是两侧同动 + 新 wire 语义、将命中跨系统结构性升级判据；设备侧判别器 =「Speaking 轮是否从 Listening 进入」，对话轮结构性不可误伤（完整论证见包 plan「fork② 裁定」节）。①查字收尾按 plan 改动点 2 实装（SendStopListening + 回 Idle）。60 秒 timer 维持不并入（非目标 1）。条目状态保持 deferred，闭账条件 = 包 merge + 提醒轮/查字轮真机复验（与 wakeword 条目复验轮同轮去重；引擎侧子项延后至 human 人工部署 ECS 后同轮闭账——human 已裁 engine-patch 只 commit 不部署）。

**真机联测留痕（2026-10-14，同日）**——三项观察，前两项已入补充轮 261014-r2 / 残余登记：

1. **判据③（本条目②的设备侧修法）在真机首轮误伤多句对话轮**——根因 = `speaking_round_entered_from_` 采样写点落在逐句 `sentence_start` 分支被覆写（第 2 句起状态已 Speaking）；**由补充轮 261014-r2 主修 A1（加 `!= kDeviceStateSpeaking` 守卫）返修**，见包 plan「补充轮 261014-r2」节。本条目②的**提醒轮语义未被破坏**（提醒 speak 从 Idle 插入 → entered_from=Idle → 播完回 Idle，正是设计意图），故提醒轮复验项照旧有效。
2. **查字轮（本条目①）设备侧修法生效**（真机：`SendStopListening` + Idle + 查字窗 `layer_top=5` 驻留 ✓），但发现**引擎侧残余 F2**：查字播报停止后仍漏转写一轮 ASR（0.9s 后「Hmm」→ 引擎回「我在这儿呢」）——根因 = 该帧音频在聆听期已送达引擎、stop 不可撤回，引擎侧缺「stop 后丢弃 in-flight audio」；**设备侧无解，登记为引擎侧残余项**，不在本包修。
3. **提醒轮（本条目②）真机复验未完成**：等 18:25 真实提醒轮（engine-patch 部署后同轮闭账 ERROR 消除）。
