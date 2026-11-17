# 实现面③拆包方案 · 261115-arch-review-session-channel-resident

> **性质**：本件是架构批 PROPOSAL 的**配套建议**，供主线程在 `/triage-change` 实现面收尾时消费（五档分流 = ③拆包）。它**不是** reviewed plan，也不替代 planner 的 intake——每包的 `plan.md` / TaskPackage / brief 仍由 planner 按其流程产出。本件的权威输入 = 同目录 `PROPOSAL.md`（§1 三层解耦模型 / §2 T1–T7 改法 / §2A 契约包三节）。
> **human 约束（2026-11-15 原话）**：「确定好方案拆包不要拆的太碎」→ 本方案给 **2 包**（在 human 建议的 2–3 包区间下沿）。
> **起草日期**：2026-11-16 · architect
> **v1.2 修订（2026-11-16，随 PROPOSAL 门 1 F-01 回修同步）**：§2.3 front 要点由「四件」改「五件」（第 2 件重写为**按 `acc.routing` 分层**、撤销「listen stop 当刻取消残余累积」，新增第 3 件 = `_story_timeout_sweep_body` pending 分支补 `_release_record_context`）；§2.1 manifest 净影响与两条新负锚（`story.py` / `test_m4k1_story_cloud_link.acceptance.py`）；§2.4 SCRF-B2 重写 + 新增 B2b/B4b/B6；§4.3 真机 A5 判据按释放时点分层精确化（分 >10s 与 <10s 两子场景）。**包数、包边界、merge order、riskTier、部署时序均未变**——F-01 是 front 侧释放时点的修正，不改拆包形状。

---

## 0. 为什么是 2 包，而不是 1 包或 3 包

**为什么不合成 1 包**：P1（通道常驻核心）与 P2（云端结束意图 + 长重试）的**风险性质不同**——P1 是结构性改动，它的失败模式是「提醒不达 / 记录语境哑火 / 麦克风常开」这类**安全与必达性**事故，且一旦出问题必须能整包回滚而不动其它面；P2 是在 P1 已建的单一处理器上**增加触发源**与改善离线自愈，任一半都可独立回滚。把两者合成一包会让「真机轮 A 发现通道常驻有致命问题」时，连带已经验好的 `end_session` 与长重试一起回退，且 diff 横跨固件会话核心 + front 泵 + display 门禁 + 重连策略 + prompt 制品五个面，单包 verify 的判别力会被摊薄。

**为什么不拆成 3 包**（P2 再切成「云端结束意图」+「长重试」）：两半共享同一次真机轮（轮 B 的「说拜拜全链通」与「拔网线/重启 front 自愈」可以在同一轮持板里跑完，human 只需上板一次），且都不改 P1 的任何结构、互不触碰同一函数区（`mcp_server.cc` control case 0 + `display_schema.py` + prompt vs `application.cc` 的 `TryStartAutoConnect` task 体）。再切一刀只多付一轮 launch 前置 + 一次 supervisor + 一次合并闸，收益为零——正中 human「不要拆太碎」的裁定。

**为什么 60s 超时归 P1 而不是 P2**（这是本方案最容易被误切的一刀，理由必须写清）：60s 无语音超时**不是**一个「额外的结束触发源」，它是**让通道常驻这件事安全的补偿控制**。今天设备在一轮聊完后停在聆听态、麦克风常开，最终是被「约 137s 后 WS 通道超时」这个**意外兜底**拽回 Idle 的（`TECH-261014-session-end-channels-unwired` 真机实证）；P1 把通道做成常驻之后，这个意外兜底就消失了——若 60s producer 不同批落地，P1 的净效果是**「孩子走开后麦克风永久常开」**，属隐私 + 电量 + 「环境音被当输入」三重回归。∴ 60s producer 与「不拆通道」必须原子同包。

---

## 1. 包表

| | **P1** | **P2** |
|---|---|---|
| changeId 建议 | `261116-channel-resident-core` | `261117-session-end-intent-longretry` |
| 名称 | 通道常驻核心（会话/通道解耦 + 保活 + 会话边界收尾 + 60s 兜底） | 云端结束意图通道 + 自动重连封顶长跑 |
| 五档 | ②单包 Full | ②单包 Full |
| riskTier | **high** | **medium** |
| sourceType | `proposal`（依据 = 本批 FROZEN PROPOSAL） | `proposal`（同上） |
| 触面 | firmware + server（跨面原子） | firmware + server + 部署面 prompt 制品 |
| 触契约 | 是（消费 PROPOSAL §2A 已冻结的契约 diff，**包内零契约写权**） | 是（同上） |
| 依赖 | 前置 = 架构批已 FROZEN 且 canonical 三件已 landed（launchPreconditions 的「支撑语义已冻结」判据 = `docs/architecture.md` §12 出现 `261115-arch-review-session-channel-resident` 行） | 前置 = P1 已合 main（P2 的 `end_session` 触发源必须收口到 P1 建立的单一处理器；长重试的守卫三条件语义在 P1 的常驻模型下才成立） |
| 真机轮 | 轮 A（human 持板） | 轮 B（human 持板） |

**riskTier 证成**：
- P1 = high —— ① 触固件会话生命周期核心路径（`application.cc` 状态机 + Idle 分支八件清位链）；② 触提醒必达性（S-F1.4）：会话间隙 `kidpal_speak` 能否出音是常驻模型的承重实证，失败即「提醒推送黑洞」复发；③ 触记录语境抑制链单点（front 漏发 `active=false` = 引擎对该连接永久抑制 LLM = 设备哑火，且连接级 teardown 兜底在 P1 后消失）；④ 冻结验收齿成批重写（`boot_auto_connect.acceptance.c` 的 RC-A3/RC-B2 族 + `voice_round_ui.acceptance.c` 的 `local_close_settle` 4 命中 + `record_session_live.acceptance.c` 的 `CloseAudioChannel` 22 命中）；⑤ 跨两面原子，回滚面大。
- P2 = medium —— ① `end_session` 三段补链中，prompt 与 front 截获是 additive、设备侧只改一个 case 分支（`AbortSpeaking` → `EndVoiceSession`）；② `display_schema.py` 的 self_push 豁免口从 1 值扩到 2 值，relay 恒拒不变（两条既有 relay 侧冻结齿天然保绿）；③ 长重试改的是退避表形态与 task 循环边界，守卫三条件不变；④ 新增「人工意向位」是 P2 内最重的新机制，但它只在离线期活跃，稳态零路径。

---

## 2. P1 · `261116-channel-resident-core` — boundary 与内容

### 2.1 changeManifest（预定改动文件；planner 按 1g 档派 architect 技术预评时以此为种子，实际 manifest 以预评回执为准）

**固件面**

```
firmware/main/application.cc
firmware/main/application.h
firmware/main/test/session_channel_resident.acceptance.c     (新增)
firmware/main/test/boot_auto_connect.acceptance.c            (冻结齿重写)
firmware/main/test/voice_round_ui.acceptance.c               (冻结齿重写)
firmware/main/test/record_session_live.acceptance.c          (冻结齿重写)
firmware/main/test/host_app/CMakeLists.txt                   (新增验收目标登记，条件)
```

**server 面**

```
server/gateway/front/front_app.py
server/acceptance/test_session_channel_resident.acceptance.py  (新增)
server/acceptance/test_m4k3_record_session_live.acceptance.py  (v1.2 升为非条件：:508 docstring 前提精确化 + 新增释放边沿姊妹齿；断言零删弱)
server/acceptance/test_record_cancel_lifecycle.acceptance.py   (断连兜底降级为次路径，条件)
```

**v1.2 修订对 manifest 的净影响（门 1 F-01）**：文件集合**未变**（`front_app.py` 与两件 py 验收本就在内），但 `front_app.py` 的改动面**净增一处** = `_story_timeout_sweep_body:1254-1257` 的 pending 分支补 `_release_record_context` 调用（详见 §2.3 第 3 件）。`server/gateway/story.py` **仍不在 manifest**——`release_pending` / `cancel` / `PENDING_STOP_SECONDS = 10.0` 三个纯逻辑缝的签名与判据逐字零改动，本包只新增 front 侧调用点。riskTier 维持 **high** 不变（该处属既有 high 判定中「记录语境抑制链单点」的同一面，未引入新面）。

**显式不在 P1 manifest（负锚，planner 应据此判越界）**：`firmware/main/mcp_server.cc`（control case 0 归 P2）· `server/gateway/display_schema.py`（self_push 豁免口归 P2）· `server/deploy/openclaw-agents/**` 与 `OPENCLAW_SYSTEM_PROMPT_CARRIER`（告别纪律归 P2）· **`server/gateway/story.py`（v1.2 新增负锚：`release_pending` / `cancel` / `PENDING_STOP_SECONDS` 三缝签名与判据零改动）** · **`server/acceptance/test_m4k1_story_cloud_link.acceptance.py`（v1.2 新增负锚：SAO-A1/A2/A3 + M4K1-A2 是 F-01 的 must-stay-green 断言面，一条都不许动，只许跑绿留痕——见 SCRF-B6）** · `firmware/main/protocols/protocol.cc` 与 `websocket_protocol.cc`（本批设计的关键取舍 = 阈值/判据/分类机制三处逻辑零改动；**条件例外**：若实现判定 `IsTimeout()` 的 `ESP_LOGE` 必须降频以避免自愈巡检每秒刷日志，则该文件入 manifest，由 builder 经 planner 带根因回评、architect 扩单留痕）· `server/deploy/engine-patch/**`（硬约束：human 未授权改引擎补丁）· `docs/**`（canonical 归架构批，实现包零写权）。

### 2.2 固件侧改动要点（六件）

1. **统一会话结束处理器**：`HandleEndVoiceSessionEvent` 成为**唯一**收口点，动作序 = ①（仅 Speaking）`ResetDecoder` + `AbortSpeaking`（261024 既有两步零改动）② `protocol_->SendStopListening()` **取代** `protocol_->CloseAudioChannel()` ③ `SetDeviceState(kDeviceStateIdle)` ④ 省电剖面维持 `PERFORMANCE`（删掉 `SetPowerSaveLevel(LOW_POWER)`）⑤ **删掉** `ScheduleResidentRebuild()` ⑥ 审计行 `local_close_settle` 的 `src=` token 族保留（`overlay_close` 等四值），但语义注记改为「会话收尾（通道保持常驻）」——token 是既有可观测面，删了会让日志分析断代。
2. **四条收尾路径收口**：`Application::CloseAudioChannel()`（录音丢弃面，:972-990）· `HandleToggleChatEvent` 的 Listening 分支（:1031-1041）· `HandleEndVoiceSessionEvent`（:1141-1170）· `WakeWordInvoke` 的 Listening 分支（:1508-1526）——四者一律改为投递 `MAIN_EVENT_END_VOICE_SESSION` 事件位（或直接委托 `EndVoiceSession()`），**不再各自重复五件套**。
   - `WakeWordInvoke` 非本板（waveshare-s3-rlcd-4.2）调用路径（实核：仅 bread-compact / esp32-cgc / sensecap-watcher 三族板级调用），但**仍须一并收口**——留一条旧形态会在同一文件里制造两套会话语义，且 `boot_auto_connect.acceptance.c` 的齿会永久钉住那个分叉。
   - **录音丢弃面的时序保证（承重，必须作为冻结断言）**：`mcp_server` 的 DISCARD_RECORD case 先发 `SendKidpalStoryDiscard()`、后触发会话结束；两帧在同一 TCP 流上有序 → front 先 `acc.cancel()`（实证 `story.py:886-894` 会 `_sessions.pop`）→ 随后 `listen stop` 经 `_story_stop_on_listen_stop` 查 `acc.routing()` 得 `"chat"`（`story.py:654-656`）→ **不进归档分支**。次序颠倒 = 已丢弃素材被归档（不可逆）。
   - **五个真拆通道点零改动**：`HandleNetworkDisconnectedEvent` :508 · `Reboot` :1407 · `UpgradeFirmware` :1426 · `SetAecMode` :1588 · `ResetProtocol` :1601。`local_close_pending_` 分类机制与 `OnAudioChannelClosed` → A 路径（Idle + `TryStartAutoConnect`）逐字保留。
3. **Idle 分支八件清位链逐条保绿**（`application.cc:1244-1299`，这是本包最重要的**不变量面**）：`EnableVoiceProcessing(false)` / `EnableWakeWordDetection(true)` / [R2] `kidpal_story_session_end` + `KidpalStorySessionEndApply` / `KidpalStorySessionEndSync`（次序承重：在 [R2] 之后、keybar 复位与 voice_end 之前）/ `kidpal_keybar_scene_reset`（REMINDER 在顶时豁免）/ `kidpal_render_node_voice_end`（OQ3=c 回答页驻留）/ `kidpal_story_session_ctx_set(0)` + `s_story_rec_entered = false` / `RaiseReminderQueue()`。**这八件是「麦克风口径不变」与「提醒排队延后立即弹」的机制承载，一件都不能因为「通道不关了」而被顺手删掉。**
4. **60s 无语音超时 producer**：落点 = 既有 `MAIN_EVENT_CLOCK_TICK` 消费区（:429-466，1s 周期 esp_timer）+ 既有 `clock_ticks_`（`HandleStateChangedEvent:1235` 状态迁移归零）。判据 = `GetDeviceState() == kDeviceStateListening && clock_ticks_ >= 60 && !kidpal_story_session_ctx_active()` → `EndVoiceSession()`。**豁免**：记录语境（requirements §2.1 v1.65：记录会话不适用 60s，自动收尾归网关侧卡壳计时器族）；整理过渡态天然豁免（设备已 Idle）。**计时重置补强**：`MAIN_EVENT_VAD_CHANGE` 且 VAD 报说话中 → `clock_ticks_ = 0`（Realtime/AEC 形态下孩子连续讲 > 60s 时设备态停在 Listening 不迁移，仅靠状态迁移归零会误收）。60 这个阈值字面应走命名常量（`kSessionIdleTimeoutS`），与 `kResidentRebuildDelayS` / `kAutoConnectBackoffS` 命名常量先例同型。
5. **心跳帧显式 no-op 消费分支**：`OnIncomingJson`（:763-927）在 `else { ESP_LOGW("Unknown message type") }`（:924-926）**之前**新增 `kidpal_keepalive` 分支：只打一条 DEBUG 级（或不打）+ 直接 return。**必须显式**——否则每 30s 一条 WARN 冲 SD 日志 16MB 轮转预算（约 144 KB/日，9 天填满一个轮转窗）。
6. **主循环 tick 自愈巡检**：同一个 CLOCK_TICK 消费区内新增 `state == Idle && protocol_ && !protocol_->IsAudioChannelOpened() && !auto_connect_in_flight_` → `TryStartAutoConnect()`。守卫三条件与「零第二条重连路径」不变量保持（只调既有 `TryStartAutoConnect`，不自建 task、不自己 `OpenAudioChannel`）。**降频**：每秒判一次即可，但 `IsTimeout()` 命中即打 `ESP_LOGE` → 实现必须让该日志只在跳变沿打一次（或把巡检判据改为每 5s 一次），否则死链期每秒一条 ERROR。

### 2.3 front 侧改动要点（五件；v1.2 按门 1 F-01 修订，原「四件」的第 2/3 件重写 + 新增第 3 件）

1. **每连接心跳 task**：在 `ws_gateway`（:2446-2573）内与双泵并列起一个 `asyncio.create_task`，周期 30s 向设备腿 `_push_display_payload` **之外**的裸 `websocket.send_text(json.dumps({"type":"kidpal_keepalive"}))`（心跳不是 display 帧，**不过** `display_schema`、不走 `kidpal/display` mcp 封套）。生命周期与双泵同：加入 `asyncio.wait` 集合**之外**（它是无限任务，不能进 FIRST_COMPLETED 集合，否则心跳永不结束会拖住收尾）——正确形态是 `finally` 里 `cancel()` + 限时回收。发送失败（连接已死）= 静默退出并让泵收尾。间隔与开关走模块级常量 + env：`KIDPAL_CHANNEL_HEARTBEAT`（**代码默认开**，`0/false/off/no` 关——极性说明见 §4 部署时序）与 `KIDPAL_CHANNEL_HEARTBEAT_INTERVAL_S`（默认 30）。
2. **`listen stop` 每会话收尾 = 按 `acc.routing(sid)` 分层，不在 stop 当刻无条件取消**（v1.2 重写；原「②残余累积态取消」动作**撤销**）。落点仍是 `pump_client_to_upstream`（:1782-1840）既有 `_story_stop_on_listen_stop` 旁路消费点（:1826-1828）**之后**，但动作按路由态分五支：

   | `listen stop` 当刻 routing | 动作 | 记录语境释放 |
   |---|---|---|
   | `chat`（问答会话） | 无动作 | 不适用——**带本连接「记录语境曾置位」标记守卫**，标记为假则**零注入**（否则每轮问答都往引擎腿打一帧无用 `active=false`，纯噪声） |
   | `accumulate` + accum 非空 | 已被 `_story_stop_on_listen_stop`→`_story_finish_session` 消费，**收尾三件不得再动该会话**（否则与归档竞态：双归档或毁稿） | 由 `_story_finish_session` 既有三个 `_release_record_context` 调用点（:1716/:1727/:1747）在**整理回合完成后**释放——261022 语义零改动，提前释放会复发孤儿幻听 |
   | `accumulate` + accum 空 → `mark_pending` 落 `pending_stop` | **禁止 cancel、禁止释放**（protected state） | **≤10s**，由第 3 件的 sweep 释放点承载 |
   | `pending_stop`（重复 stop，SAO-A3 形态） | 同上；`mark_pending` 幂等不重置挂起时刻 | 同上 |
   | `accumulate` 且本帧未被归档路径消费（**结构上不可达**，防御性） | **不 cancel**，留既有卡壳引导 / 自动收尾 sweep（2 轮无回应 → 归档、不丢弃，S-F2.2 AC5） | 由 `_story_auto_finish`→`_story_finish_session` 既有释放点覆盖 |

   收尾动作随之重述为三件：**① 分层释放**（按上表）**② 不当刻取消任何累积态** **③ 审计一行**（含冻结 token 如 `session_end_boundary`，记 routing 分支与是否触发释放）。
   **为什么必须撤销当刻 cancel（F-01）**：`pending_stop`（录音态 KEY 收尾、accum 空、manual 形态转写约 **1.7s** 后才到，`front_app.py:577` docstring 实录）与「引导态无内容」在 listen-stop 那一刻**可观状态完全相同**，「本帧未被归档路径消费」这类守卫无法区分；当刻 `cancel` = `story.py:886-894` 的 `_sessions.pop` → 在途转写不可恢复 → 三式收尾之一的归档丢失，违反 S-F2.2 AC4 v1.65。断言级冲突三条 must-stay-green：SAO-A2（`test_m4k1_story_cloud_link.acceptance.py:1891`）+ M4K1-A2（同文件 `:849-882`，`len(files)==1`）+ SAO-A3（重复 stop 不双归档）。
3. **`_story_timeout_sweep_body` 的 pending 分支补释放调用（v1.2 新增，F-01 的承重半）**：`:1254-1257` 今天只 `acc.release_pending(sid)` + 一条 `story_pending_timeout` 日志，**不调 `_release_record_context`**（全文该函数调用点仅 :1716/:1727/:1747，三处全在 `_story_finish_session` 内）。旧模型下这不是缺陷——设备随后断连，引擎侧 `_kidpal_record_context` 随连接消失；**P1 让通道常驻后兜底消失**，pending 超时只回 routing 不解除引擎抑制 = 该连接后续问答**永久哑火**。改法 = 在 `release_pending(sid)` 返回 True 的分支内追加 `await _release_record_context(sid, _session_legs(sid) or list(_alert_legs))`（腿集形态逐字沿 `_story_auto_finish:1353`）。守卫天然成立：`release_pending` 只对 `pending_stop` 返 True，而 `pending_stop` 只能由 `accumulate` 经 `mark_pending` 到达、`accumulate` 只能由 ctx(story) 或 `control/start_recording` 进入（两者都注入过 `active=true`）→ 不会对从未进记录语境的会话注入无谓帧。**`story.py` 侧零改动**（`release_pending` / `cancel` / `PENDING_STOP_SECONDS = 10.0` 三个纯逻辑缝签名与判据逐字保留，本包只新增 front 侧调用点）。
4. **`kidpal_session_ctx` 边沿完整化（兜底层，v1.2 改为分态处置）**：`_consume_kidpal_control`（:992-1177）的 ctx 分支今天只处理「进 `story`」（:1035-1053）与「points 两值注入摘要」（:1054-1065），对 `home`/`schedule`/`story_cover` **零动作**（:1066 的 elif 只做未知值拒绝）。改为：维护本连接的「记录语境曾置位」标记，收到 **非 `story`** 的合法 ctx 且标记为真时**按 routing 分态**——`pending_stop` → **不动作**（保护在途转写，交第 3 件的 sweep；只记一条 INFO 说明延迟释放与 ≤10s 上界）；`accumulate` → `acc.cancel(session_id, reason="session_end_stale")` + 释放（= 常驻时代的 cancel-on-disconnect 同族兜底，沿 human 2026-10-21「cancel-on-disconnect 维持为兜底」裁定口径，**不发明「弃录自动归档」新需求语义**）；`chat` 但标记仍为真 → 只释放（防御性：routing 已被 sweep/finish 清而释放漏了）。**这一层不是可选优化**：P1 之后连接级 teardown 不再是主路径，`listen stop` 一旦漏发（设备崩溃重启前的半帧、泵竞态），旧形态的兜底就没了。但**它只在设备未发 `listen stop` 时可达**——所有正常结束路径都发 stop，故正常形态下本层恒不触发。
5. **连接级 teardown 降为最外层兜底（不删）**：`finally` 三件（腿双摘除 :2559-2562 + `acc.cancel(reason="ws_disconnect")` :2566-2570）**逐字保留**——引擎 24h 日切、网络断、front 重启三种形态仍会走它。改动只是它的**触发频率**从「每次语音会话」降为「每连接一次」，这正是 bootstrap burst 归零的收益来源（连接期一次性六件 :2486-2529 全部保持绑连接，零改动）。

> **v1.2 显式申报的 accepted residual（非缺陷，须写进 P1 的 review-packet 与真机轮 A 判据）**：记录会话以「accum 空」形态结束后 ≤10s 内，引擎侧记录语境抑制仍在世；若孩子在这 ≤10s 内立刻从非 story 页发起新问答会话并马上开口，该次话语可能被抑制（`listen start` 只清 `_kidpal_listen_closed`、不清 `_kidpal_record_context`）。**不得用「提前释放」消除它**——提前释放正是 261022 修掉的孤儿幻听缺陷（`_issue_finish_downlink:1661-1665` docstring 实录 KPLOG006 23:59:30「Thank you」被当普通对话派发并播报「我没听清/再说一遍」，盖过整理演出）。窗口上界 = 既有冻结常量 `PENDING_STOP_SECONDS = 10.0`；触发需三件同时成立（空 accum 结束 + 10s 内换会话 + 立刻开口）；后果 = 一次话语被抑制、再按 KEY 即恢复，**无数据损坏**。用一次可恢复的抑制换「录音态 KEY 收尾归档不丢」（S-F2.2 AC4 硬语义）是本批的有意识取舍。

### 2.4 P1 验收断言方向（acceptance；planner 与 acceptance-author 细化，本处只给方向与承重判据）

**设备侧 C host 验收（新增 `session_channel_resident.acceptance.c`）**

- `SCR-A1` 六触发源收口：四个收尾点函数区（`Application::CloseAudioChannel` / `HandleToggleChatEvent` Listening 分支 / `HandleEndVoiceSessionEvent` / `WakeWordInvoke` Listening 分支）**负锚** = 区内 `CloseAudioChannel(` 命中 0、`ScheduleResidentRebuild(` 命中 0、`SetPowerSaveLevel(PowerSaveLevel::LOW_POWER)` 命中 0；**正锚** = 区内 `EndVoiceSession(` 或 `MAIN_EVENT_END_VOICE_SESSION` 命中 ≥ 1。
- `SCR-A2` 统一处理器动作序：`HandleEndVoiceSessionEvent` 区内 `SendStopListening()` 恰 1、`ResetDecoder()` 恰 1 且在 Speaking 分支内、三态门（Connecting/Listening/Speaking）与 `auto_connect_in_flight_` 守卫与 `!protocol_` 早退三件逐字在世（261024 既有语义零偏移）。
- `SCR-A3` 五个真拆通道点保绿：`HandleNetworkDisconnectedEvent` / `Reboot` / `UpgradeFirmware` / `SetAecMode` / `ResetProtocol` 五区各含 `CloseAudioChannel()` 恰 1（**正锚，must-stay-green**）。
- `SCR-A4` Idle 分支八件清位链：`HandleStateChangedEvent` 的 `kDeviceStateIdle` case 内八个符号各命中 ≥ 1（逐符号枚举，含 `RaiseReminderQueue` 与 REMINDER 在顶豁免判据 `kidpal_render_node_front_is_reminder`）。
- `SCR-A5` 60s producer：CLOCK_TICK 消费区内命名常量（值 60）+ `kDeviceStateListening` 判据 + `kidpal_story_session_ctx_active()` 豁免判据 + `EndVoiceSession` 投递四件在世；VAD 沿重置在 `MAIN_EVENT_VAD_CHANGE` 区内。
- `SCR-A6` 心跳 no-op 分支：`OnIncomingJson` lambda 区内 `kidpal_keepalive` 字面命中 ≥ 1，且其位置**先于** `Unknown message type` 的 `ESP_LOGW`（用区内偏移量断言，防「加了分支但在 WARN 之后」的假绿）。
- `SCR-A7` 自愈巡检：CLOCK_TICK 区内 `IsAudioChannelOpened()` + `TryStartAutoConnect()` 各命中 ≥ 1，且守卫含 `auto_connect_in_flight_`；**负锚** = 巡检区内 `OpenAudioChannel(` 命中 0（零第二条重连路径）。

**既有冻结齿重写（必须在 plan.md 里逐条列「旧钉什么 → 新钉什么 → 为什么」，不得静默改）**

- `boot_auto_connect.acceptance.c`：RC-B2「四会话收尾点各调 `ScheduleResidentRebuild()` 恰 1」→ **整条退役**（改为 SCR-A1 负锚）；RC-A3「helper 区消费 `kResidentRebuildDelayS` + 排入 `TryStartAutoConnect`」→ **整条退役**（helper 与常量一并删）；K6-A1/A3/A5 的守卫三条件与四个人工入口 in-flight 守卫 → **保绿零改动**。
- `voice_round_ui.acceptance.c`：`local_close_settle` 4 命中的 src token 族 → **保绿**（token 保留），但同区「`CloseAudioChannel` + `LOW_POWER` + `ScheduleResidentRebuild`」三件套断言 → 按 SCR-A1 负锚重写。
- `record_session_live.acceptance.c`：`CloseAudioChannel` 22 命中逐条核——录音丢弃面改为「`SendKidpalStoryDiscard()` 先、会话结束事件后」的**有序断言**（新增），真拆通道点保绿。

**server 侧 python 离线验收（新增 `test_session_channel_resident.acceptance.py`）**

- `SCRF-B1` 心跳：注入假时钟跑一个周期 → 设备腿恰收一帧 `{"type":"kidpal_keepalive"}`；env 置 `0` → 零帧（闸有效）；发送抛异常 → task 静默退出、双泵收尾不被拖住（`finally` 三件仍执行）。
- `SCRF-B2` `listen stop` **分层**收尾（v1.2 按门 1 F-01 重写；原第三分句「引导态 → `acc.cancel(reason="session_end")` 被调 + `active=false` 注入恰一帧」**撤销**，改为下方 B2b）：① 问答会话（routing=`chat`、记录语境标记为假）→ 收尾全 no-op、**零 `acc.cancel`、零引擎腿注入**（正锚 + 负锚配对；负锚防「每轮问答都打一帧无用 `active=false`」）；② 记录会话 accum 非空 → `_story_finish_session` 已消费本帧 → 收尾**不再动该会话**（零额外 cancel、零额外注入；释放由整理回合完成后的既有调用点承载）。
- `SCRF-B2b` **`pending_stop` 受保护 + sweep 释放点**（v1.2 新增，F-01 的承重齿）：① ctx(story) → `listen stop`（accum 空）→ 断言 `acc.routing(sid) == "pending_stop"`、**`acc.cancel` 未被调**、**`active=false` 尚未注入（0 帧）**；② 以注入时钟跑 `_story_timeout_sweep(now = pending_since + 10s)` → 断言 `release_pending` 生效（routing 回 `chat`）**且 `active=false` 恰一帧注入该会话归属腿**（腿集 = `_session_legs(sid)`，用假腿对断言注入目标，不打进 `_alert_legs` 全局腿）；③ 反向：`now = pending_since + 9s` → 零释放零注入（10s 阈值判据有效、不提前）；④ 在 ② 之前先投一条 stt → 走 `_story_feed_from_stt` 恢复归档路径 → 断言归档恰一篇且 sweep 不再重复释放（`release_pending` 对非 pending 返 False）。
- `SCRF-B3` discard→stop 有序不归档：同一 session 依次投 `kidpal_story_discard` + `listen stop` → `acc.routing` 回 `chat`、`_story_finish_session` **未被调**、`STORIES_DIR` 文件数不变（**这是「丢弃即丢弃」的承重齿**；v1.2 复核：discard 走 `cancel(reason="discard")` 独立路径，与分层收尾的 `pending_stop` 保护态互不相干，本齿零改动保绿）。
- `SCRF-B4` ctx 边沿兜底 **`accumulate` 分支**（v1.2 收窄）：先投 ctx(story)（进语境、注入 `active=true` 恰一帧，routing=`accumulate`，**不发 `listen stop`**——模拟设备未发结束信号的异常形态）→ 再投 ctx(home) → 断言 `acc.cancel(reason="session_end_stale")` 被调 + 注入 `active=false` 恰一帧；重复 ctx(home) → 零新增注入（幂等）。
- `SCRF-B4b` ctx 边沿兜底 **`pending_stop` 分支不动作**（v1.2 新增，防 F-01 从另一扇门复发）：ctx(story) → `listen stop`（accum 空、落 `pending_stop`）→ 立刻投 ctx(home) → 断言 **`acc.cancel` 未被调、`release_pending` 未被调、`active=false` 零注入、routing 仍 `pending_stop`**（在途转写受保护），只留一条延迟释放的 INFO 审计行；随后 10s sweep 照常释放（接 B2b ②）。
- `SCRF-B5` 连接级 teardown 保绿（must-stay-green）：断开 → 腿双摘除 + `acc.cancel(reason="ws_disconnect")` 逐字不变。
- `SCRF-B6` **既有冻结 oracle 全数保绿（v1.2 新增，F-01 的回归网）**：P1 的 front 半 verify 必须把下列**未改动**的既有验收文件整体跑绿并逐条留痕，任一红即 P1 不得进 integrator——`test_m4k1_story_cloud_link.acceptance.py` 的 SAO-A1 / **SAO-A2**（`:1891`，空累积 stop 不得回 chat）/ SAO-A3（重复 stop 不双归档）/ **M4K1-A2**（`:849-882`，真机序恰一篇归档）；`test_m4k3_record_session_live.acceptance.py` 的 M4K3-A7 四齿（含 `:508` `test_a7_ctx_non_story_pushes_zero_frames` 三断言）；`test_record_dialogue_guide.acceptance.py` 的 RDG-A6 sweep 家族（10s 语义）与 RDG-A7（`kidpal_speak` 提醒出音）。**允许的改动只有两类**：`:508` 的 docstring 前提精确化（「非 story 取值零行为」→「非 story 取值在记录语境未置位时零行为；置位时走分态兜底」）与新增姊妹齿；**不得删弱任何断言**（F-03 处置，PROPOSAL §2A impact 已显式登记）。

---

## 3. P2 · `261117-session-end-intent-longretry` — boundary 与内容

### 3.1 changeManifest（种子）

**固件面**

```
firmware/main/mcp_server.cc                                  (control case 0)
firmware/main/application.cc                                 (TryStartAutoConnect task 体 + 人工意向位)
firmware/main/application.h                                  (意向位成员)
firmware/main/test/session_end_intent_longretry.acceptance.c  (新增)
firmware/main/test/boot_auto_connect.acceptance.c             (K6-A2 tooth5 退避表形态重写)
```

**server 面**

```
server/gateway/display_schema.py                             (self_push 豁免口 1 值 → 2 值)
server/gateway/front/front_app.py                            (end_session 截获 + 机械自推)
server/acceptance/test_record_dialogue_guide.acceptance.py   (1 条断言反转，显式申报)
server/acceptance/test_session_end_intent.acceptance.py       (新增)
```

**部署面制品**

```
server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md      (告别回合纪律)
server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER                  (全量副本 append，沿革不删)
server/deploy/manifest.yaml                                   (条件：仅当承载件登记需刷新)
```

**显式不在 P2 manifest**：`server/acceptance/test_points_e2e_spike.acceptance.py`（:866-868 用缺省 `self_push=False`，本批 relay 恒拒 → 天然保绿，**不得顺手改**）· `server/deploy/engine-patch/**`（硬约束）· `docs/**`。

### 3.2 改动要点

**云端结束意图通道（三段补链，一段都不能少）**

1. **prompt 面**：`workspace-kidpal/AGENTS.md` 新增告别回合纪律——孩子明确告别（「拜拜 / 不用了 / 先到这儿 / 我不聊了」同族）时，回复 = 口语告别一句 + ` ```display ` 块 `{"view":"control","data":{"action":"end_session"}}`；`home` view 行的「孩子说再见/结束」语义**收窄**为「无需显示内容的普通答复」（否则模型会继续用 `home` 承载告别，新纪律形同虚设）。硬约束节同步补一条。承载不变式（ADR-001 三面相等）：渲染面 = 声明面 = 被测面，`OPENCLAW_SYSTEM_PROMPT_CARRIER` 随批 append 全量副本，`verify-deploy.sh` 的 md5 核验照跑。
2. **front 门禁面**：`display_schema.py` 的 `validate_display` self_push 分叉（:156-163）——豁免口从「仅 `start_organizing`」扩为「`start_organizing` + `end_session`」；`GATEWAY_DECISION_CONTROL_ACTIONS` 集合**不变**（四值仍全在，relay 恒拒）；模块 docstring（:14-20）与两处行内注记（:139 / :159）的「end_session 即便自推仍拒」句必须同步改写，否则代码与注释双源。
3. **front 截获-自推面**：`_relay_display_blocks_to_device`（:1531-1567）内、与 `_consume_points_intent`（:1558）同型的位置，新增 `end_session` 薄意图截获：命中 → `continue`（**不透传**）→ 以 `_push_display_payload(websocket, {"view":"control","data":{"action":"end_session"}})` 机械自推恰一帧。截获判据必须是「引擎 display 流来源」（relay 路径），front 自推路径不得自我截获造成回环。
4. **设备消费面**：`mcp_server.cc:1725-1727` 的 `case 0` 从 `app.AbortSpeaking(kAbortReasonNone)` 改为 `app.EndVoiceSession()`（收口到 P1 的统一处理器）。**语义不丢**：`AbortSpeaking` 由处理器的 Speaking 分支承载（且额外补了 `ResetDecoder` 即时静音，比原 `case 0` 更完整）；行内注记「结束会话回首页 (本地停播主地位不变)」按 OQ2 裁断改写为「结束会话（前台按 OQ3=c 驻留规则回落；通道保持常驻）」。

**自动重连封顶长跑 + 人工意向不丢**

5. **退避表**：`kAutoConnectBackoffS[3] = {5,10,20}` → 命名常量表扩为 `{5,10,20,40,60}` + 表耗尽后恒 60s 稳态（`kAutoConnectSteadyRetryS = 60`），task 循环从 `for (attempt < 3)` 改为**无固定上界的长跑**，退出条件只剩三个：连接成功 / 守卫三条件被人工路径接管（bail）/ 网络断（`HandleNetworkDisconnectedEvent` 置的退出请求——需新增一个 volatile 请求位，否则 WiFi 掉了还在傻试）。
6. **人工意向位（承重配套）**：`auto_connect_in_flight_` 在整段 task 生命周期为 true，而四个人工入口（`HandleToggleChatEvent:1015` / `HandleStartListeningEvent:1068` / `HandleEndVoiceSessionEvent:1145` / `HandleWakeWordDetectedEvent:1182`）见它即 no-op → **长跑下离线期孩子的按键与唤醒永久无响应**（比今天 3 次即止更糟，因为今天 15 秒后标志会清）。改法 = 新增 `pending_manual_intent_`（枚举：none / toggle / start_listening / wake_word）：入口见 in-flight 时置意向位 + 一条 INFO 审计行（**不再静默**）；task 每段退避前查该位，命中即把剩余退避截为 0 立刻重试；连接成功后经 `Schedule` 回主循环按意向补投对应事件位并清零。四个人工入口的既有「in-flight 即 return」形态改为「置意向后 return」，`boot_auto_connect.acceptance.c` K6-A5「四个 call site 各接 in-flight 守卫」的齿**保绿**（守卫仍在，只是不再静默）。
7. **功耗取舍**：离线期每 60s 一次 TLS 握手尝试（冷通道 TLS 证书验证段实测 0.53s，KPLOG029），跑在后台 task 内不阻塞主循环；WiFi 已在世。真机轮 B 量一轮离线 30 分钟电量差，数据回填 arch §8.1 新增风险行①。

### 3.3 P2 验收断言方向

**设备侧（新增 `session_end_intent_longretry.acceptance.c`）**

- `SEI-A1` `mcp_server.cc` control `case 0` 区内 `EndVoiceSession()` 恰 1、`AbortSpeaking(` 命中 0（**负锚**：防「两个都调」造成双重收尾）。
- `SEI-A2` 退避表形态：`kAutoConnectBackoffS` 首定义为 5 元素递增秒序列 `{5,10,20,40,60}` + `kAutoConnectSteadyRetryS` 命名常量值为 60（**重写 K6-A2 tooth5**，旧齿钉 `[3]` 长度与 `{5,10,20}`）。
- `SEI-A3` 长跑与退出：task 体内无 `attempt < 3` 字面；三个退出条件（成功 / bail / 网络断请求位）各在世；`auto_connect_in_flight_` 所有退出路径必清（K6 既有不变量保绿）。
- `SEI-A4` 人工意向位：四个人工入口区内 `pending_manual_intent_` 置位命中各 ≥ 1 且**不再是裸 return**；task 体内查意向位 + 截短退避 + 连上后 `Schedule` 补投三件在世。

**server 侧（新增 `test_session_end_intent.acceptance.py`）**

- `SEI-B1` 门禁分叉：`validate_display({...end_session...}, self_push=True)[0]` 为 True（**反转既有 RDG-A5 断言**）；`validate_display({...end_session...})[0]`（缺省 relay）仍为 False（**must-stay-green，与 `test_points_e2e_spike:866-868` 同判据**）；`add_points`/`redeem_points` 在 self_push 下仍恒拒（正锚，防豁免口误扩）。
- `SEI-B2` 截获-自推：引擎 display 流投一块 `control/end_session` → relay 输出**不含**该块（不透传）+ 设备腿恰收一帧自推 `control/end_session`（`_push_display_payload` 单源、过 `display_schema`）。
- `SEI-B3` 回环防护：front 自推的那一帧不再被 relay 路径二次截获（假 socket 双跑一轮，帧数恒 1）。
- `SEI-B4` prompt 承载三面相等：`AGENTS.md` 含告别回合纪律关键词 + `home` 行语义已收窄 + `OPENCLAW_SYSTEM_PROMPT_CARRIER` 尾部含本版全量副本（ADR-001 不变式，沿 `test_261023_agents_md_static_semantics_alignment.acceptance.py` 先例形态）。

---

## 4. merge order · serial WIP · 部署与真机时序

### 4.1 顺序与并发

```
架构批 261115 FROZEN → 主线程 packet-bound landed（canonical 三件）
        ↓ （launchPreconditions 判据 = arch §12 出现本 changeId 行）
P1 261116-channel-resident-core        [serial WIP=1，独占]
        ↓ merge main
真机轮 A（human 持板）
        ↓ 绿
P2 261117-session-end-intent-longretry [从 P1 后的 main 起分支]
        ↓ merge main
真机轮 B（human 持板）
        ↓ 绿
debt 四条结项 + requirements:48 ⚠️ 对账注记刷新（主线程路由，PROPOSAL §6 留根 1/7）
```

**serial WIP=1，两包不并发**。理由：P2 依赖 P1 建立的统一会话结束处理器（`end_session` 必须收口到它）与常驻模型下的守卫三条件语义；且两包都改 `firmware/main/application.cc` 与 `firmware/main/test/boot_auto_connect.acceptance.c`。

### 4.2 `application.cc` 同区域冲突的处置（human 明示必答项）

两包在 `application.cc` 的重叠区有两处：

| 区域 | P1 改什么 | P2 改什么 | 冲突处置 |
|---|---|---|---|
| `MAIN_EVENT_CLOCK_TICK` 消费区（:429-466） | 60s producer + 自愈巡检（两块新增判据） | 无 | **无冲突**（P2 不进此区） |
| `TryStartAutoConnect`（:572-617）+ 四个人工入口的 in-flight 守卫 | 无（守卫逐字保绿，是 P1 的 must-stay-green 面） | 退避表扩 + 长跑循环 + 意向位 + 四入口改「置意向后 return」 | **无冲突**（P1 不进此区） |
| `HandleEndVoiceSessionEvent`（:1141-1170） | 重写为统一处理器（动作序①–⑥） | 无（P2 只让 `mcp_server` case 0 调它） | **无冲突** |

∴ **不需要 rebase 策略**——两包在 `application.cc` 内改的是**不相交的函数区**，这是本拆包方案刻意切出来的性质（P1 = 会话收尾与 tick 区，P2 = 重连 task 区 + mcp control 区）。仍然要求：

1. **P2 从 P1 合并后的 main 起分支**（不在 P1 未合时并行开工），planner 在 P2 的 `plan.md` 里**重新取行号锚**——P1 的 diff 会让 `application.cc` 的行号整体位移，`boot_auto_connect.acceptance.c` 里以 `:1189`/`:985` 形态写死的旧行号注记也会漂（该文件的齿以**函数区切取**为主判据、行号只在注记里，所以是注记漂移而非断言失效，但 plan.md 仍须逐条复核）。
2. **`boot_auto_connect.acceptance.c` 是唯一被两包都改的文件**：P1 退役 RC-A3/RC-B2 两组齿，P2 重写 K6-A2 tooth5。两组齿在文件内位置不同（RC-* 在 :1282 之后、K6-A2 在 :696 附近），仍是**串行不并发**（P1 先合，P2 在其结果上改），无 merge 冲突面。
3. **P1 合并前不得起 P2 的 Pod**（launch 面 WIP=1 硬约束）；若 human 要求压缩工期，可让 P2 的 planner 在 P1 verify 绿、integrator 之前就开始 intake 与 plan 起草（读 P1 的 worktree），但 **builder 不得动工**，且 P2 分支必须等 P1 落 main 后再切。

### 4.3 部署时序（human 授权：agent 可部署 ECS + 重启 front；human 持板烧固件与真机轮）

**P1 部署（关键：front 与固件必须同批上线，心跳默认开）**

1. agent：`server/gateway/front/front_app.py` 铺放到 ECS `/opt/kidpal/front/`（沿 `deploy.sh` / `manifest.yaml` 既有通道），`systemctl restart kidpal-front`，`journalctl -u kidpal-front -f` 确认无 import/启动错误、`/healthz` 200。
2. human：烧 P1 固件（**紧接第 1 步**，窗口越短越好）。
3. 观察窗（第 1→2 步之间，旧固件 + 新 front）：旧固件每 30s 打一条 `Unknown message type: kidpal_keepalive` WARN——**功能不受损**（`OnIncomingJson` 的 else 分支只记日志），仅日志噪声；一小时约 120 行、约 6 KB，对 16MB 轮转预算可忽略。
4. **心跳闸极性 = 代码默认开**，`KIDPAL_CHANNEL_HEARTBEAT=0` 作为**回滚开关**而非上线开关。这与 `KIDPAL_STORY_RESTORE_PUSH`（默认关、烧板后人工开闸）的先例**极性相反**，理由必须写清：那个先例默认关是因为旧固件收到 restore 推送会**无条件渲染 P3 盖掉聆听态**（行为性副作用）；本帧对旧固件的唯一副作用是**一条 WARN 日志**，而若默认关，新固件在闸未开期间会退化成「每 ~120s 被 `IsTimeout()` 判死 + 自愈巡检立刻重连」= **比今天更糟的重连 churn**（今天判死后要等按键才重连）。∴ 默认开是唯一安全极性。
5. **回滚**：`KIDPAL_CHANNEL_HEARTBEAT=0` + `systemctl restart kidpal-front`（front 半），并回烧上一版固件（固件半）。front 半单独回滚即可让设备退回今天的行为（无心跳 → 120s 判死 → 自愈巡检重连；此时巡检会造成 churn，所以**固件也必须一起回滚**——这一点要在 P1 的 review-packet 里写成显式回滚剧本）。

**P1 真机轮 A（human 持板；agent 备串口捕获与日志对账）**

| # | 场景 | 判据 | 负责 |
|---|---|---|---|
| A1 | 温通道按键进聆听时延 | 对照 KPLOG029 基线 **0.82s**，同口径复测（`key_route_dispatch(session_toggle)` → `State listening` 的 uptime 毫秒差）；预期 ≤ 0.82s（省掉建连判定分支） | P1 |
| A2 | 连续 5 次「结束 → 再说话」 | 5 次全走温通道；**零** `WS: Connecting` / `TLS 证书验证` / `Session ID` 行；front 日志核 bootstrap 六件（story_list restore / points_home / alert 腿 / 会话腿 / schedule 摘要注入 / schedule 推）**每连接恰一次**、零 burst | P1 |
| A3 | 闲置 > 5 分钟后首按键 | 跨越旧 120s/137s 判死点；首按键仍走温通道、**一次即进聆听**（`TECH-261012`「连按两次才正常」形态消失）；SD 日志有心跳消费痕迹、**零** `Unknown message type` | P1 |
| A4 | **会话间隙提醒必达且出音**（最重） | 会话结束回 Idle 后，让 front 推一条 `kidpal/alert` + `kidpal_speak`：提醒弹窗渲染 + 回执出队 + **语音真的播出来**。这是 ADR-003 + `_kidpal_listen_closed` 交互的承重实证（`speak_txt` 不经 `chat()`，理论上不受抑制，但必须真机确认） | P1 |
| A5 | 记录会话全链回归（四式） | ① 引导态 KEY 结束 → **无归档**、列表条目数不变、下一次问答会话**不哑火**（记录语境已释放）——**时点判据（v1.2 精确化，门 1 F-01）**：释放**不在 `listen stop` 当刻**，而是「≤10s 由 sweep 释放」或「若孩子在 10s 内已从非 story 页发起新会话则…仍等 sweep」（`pending_stop` 是受保护态，ctx 边沿对它**不动作**，防在途转写被毁）。故本项要**分两个子场景各测一次**：**(a) 间隔 > 10s**——引导态 KEY 结束、等 10s 以上再按 KEY 提问 → 必须正常应答（front 日志可见 `story_pending_timeout released=true` 紧跟一条 `active=false` 注入）；**(b) 间隔 < 10s**——引导态 KEY 结束后立刻（≤3s）按 KEY 提问 → **允许首次话语被抑制**（这是 PROPOSAL §6 留根 9 与 arch §8.1 风险行 ④ 的 accepted residual），但必须：再按一次 KEY 即正常应答、front 日志显示 ≤10s 后释放已发生、**且没有任何素材丢失或误归档**；实测该窗的真实长度与命中体感记入 review-packet（若 human 判不可接受 → 回 `/arch-change` 评估缩短 `PENDING_STOP_SECONDS`，那是既有冻结常量、改值须回 planner 重冻结）② 录音态 KEY 收尾 → 归档成功 + `start_organizing` + `story_text` + 盖章（**accum 非空路径，走 `_story_finish_session`，释放时点 = 整理回合完成后，零变化**）③ 录音态 BOOT 丢弃 → **不入册**、列表不变、下一次问答不哑火（discard 先于 stop 的 TCP 有序性；此路径 routing 已被 discard 打回 `chat`，不涉 `pending_stop` 保护态，释放应为当刻）④ 连续 2 轮引导无回应 → 自动收尾归档 | P1 |
| A6 | 60s 无语音自动关闭 | 进会话后静置：60s ± 数秒内自动落 Idle、麦克风关（`EnableVoiceProcessing(false)`）、回原页（OQ3=c）；**这是真机首次发生**（requirements §2.1 ⚠️ 对账项闭环证据） | P1 |
| A7 | 连续讲 > 60s 不被误收 | 孩子（或 human）连续讲 90s 以上，会话不被 60s 判据打断（VAD 沿重置生效） | P1 |
| A8 | 录音态提醒排队例外 | 录音中让提醒到点 → 排队不弹；会话收尾回 Idle → 立刻弹（S-F1.4 + `RaiseReminderQueue` 保绿） | P1 |
| A9 | 查字窗关闭路径 | 查字播报完 → 会话结束不回聆听（`SendStopListening` + Idle，v1.37 口径）、查字窗驻留到按键或 5 分钟（`application.cc:802` 既有路径受 P1 影响，必须回归） | P1 |
| A10 | 24h 日切（可选，测试环境） | 在测试环境把 `close_connection_no_voice_time` 临时调小复现引擎兜底断连 → 设备识别为服务端侧断 → A 路径自愈重连 → bootstrap 六件重跑一次 → `mem_local_short.save_memory` 触发。**不改生产值** | P1 |

**P2 部署**：agent 铺 front + `display_schema.py` + `deploy.sh` 推 prompt 承载件（`verify-deploy.sh` 的 md5 三面相等核验必须过）+ 重启 front；human 烧 P2 固件。P2 无「新旧混跑」危险窗（旧固件收到自推 `end_session` 会走 `case 0` = `AbortSpeaking`，即今天的行为，不劣化）。

**P2 真机轮 B**

| # | 场景 | 判据 | 负责 |
|---|---|---|---|
| B1 | 说「拜拜」全链 | 模型产 `control/end_session` 薄意图（引擎/front 日志可见）→ front 截获（relay 输出不含该块）→ 自推恰一帧 → 设备结束会话、麦克风关、前台按 OQ3=c 回落、**通道仍常驻**（后续按键走温通道） | P2 |
| B2 | 告别语不误伤 | 孩子说「拜拜」但话题未尽（如「拜拜，明天再聊」）→ 只结束会话，不误触发别的动作；`home` view 仍可用于普通无显示答复 | P2 |
| B3 | 伪造防护 | 引擎 display 流若被注入 `control/end_session`（可用 dryrun 构造）→ relay 恒拒、只有 front 截获-自推单源能下发 | P2 |
| B4 | 拔网线自愈 | 拔网线 3 分钟再插回：设备退避 5/10/20/40/60 后稳态 60s 长跑；网络恢复后自动重连；**期间按 KEY** → 不再静默 no-op（审计行可见意向位）→ 连上后自动接续那次按键意图 | P2 |
| B5 | 重启 front 自愈 | `systemctl restart kidpal-front`：设备识别服务端侧断 → 长跑重试 → front 起来后 60s 内自动重连（对照 `TECH-261014-autoconnect` 的原始事故：3 次即止、离线约 2 分钟直到 human 按 KEY） | P2 |
| B6 | 离线期提醒不丢 | 离线窗内让提醒到点 → 重连后 front 的 60s 超时重推 ×3 语义覆盖该窗（**若覆盖不到 = 仍丢**，如实记录，不在本批扩范围；PROPOSAL §0 差距 4 只承诺「离线窗缩短」，不承诺「离线补发」） | P2 |
| B7 | 离线 30 分钟电量差 | 长跑重试的功耗代价实测，回填 arch §8.1 新增风险行① | P2 |
| B8 | 长跑期人工入口不僵死 | 离线稳态（60s 间隔）下反复按 KEY / 喊唤醒词：每次都有审计行、连上后都能接续；**绝不出现「按了永远没反应」** | P2 |

---

## 5. 交主线程的三条执行注记

1. **launch 前置**：两包的 `launchPreconditions` 都必须核 `docs/architecture.md` §12 出现 `261115-arch-review-session-channel-resident` 行（arch-change.md「定稿载体」节的消费保护判据）。P1 还需核 `docs/contracts/interface-contract.md` 的 §四 `listen` 节与 `kidpal_keepalive` 节已在场（front 心跳帧的形状权威）；P2 还需核 §1.4 表后「截获转发类」段落已在场（`display_schema` 豁免口扩面的契约依据）。
2. **契约写权**：两包的 builder/fixer 均在 dev-loop 会话内，`docs/contracts/**` **禁写**（scope-map per-agent deny + dev-loop.settings.json）；契约 diff 已在架构批由主线程 packet-bound 落地，实现包只**消费**冻结契约。若实现中发现契约表述不足以指导实现，走 planner → architect advisory，不得就地改契约。
3. **debt 结项凭据**：`TECH-261014-session-end-channels-unwired`（severity = MAJOR）的结项凭据 = **真机轮 A 的 A6 + 轮 B 的 B1 双绿**，不是代码合入；`TECH-261012-first-keypress-no-listening` 的结项凭据 = 轮 A 的 A3（「连按两次才正常」形态消失）；`TECH-261114` 的结项凭据 = 轮 A 的 A1 + A2（温通道时延 + 零 burst）；`TECH-261014-autoconnect-gives-up-silently` 的结项凭据 = 轮 B 的 B4 + B5。四条的 `closed`/`closedBy` 字段填写与 `debt-render.mjs` 归位由主线程执行。
