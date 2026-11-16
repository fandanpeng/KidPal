# Review Packet · 261116-channel-resident-core

> workstream `261115-session-channel-resident` 的 P1（mergeOrder 77，rigor full，riskTier high）。
> 供 `supervisor-verdict-gate.mjs` 机械复核；BLOCKING / ESCALATE 时升级人类。本包**不含 UIUX 工作面**（`uiux.uiuxRequired=false`），无 UI design-review 节。

## Change Size

- **Files changed**：`changeManifest[]` 11 件 = 9 件修改 + 2 件新增。
  - 固件实现：`firmware/main/application.cc`、`firmware/main/application.h`
  - 固件验收：`firmware/main/test/session_channel_resident.acceptance.c`（新）、`boot_auto_connect.acceptance.c`、`voice_round_ui.acceptance.c`、`record_session_live.acceptance.c`、`record_dialogue_guide.acceptance.c`（planner 扩单入 manifest）、`firmware/main/test/host_app/main/CMakeLists.txt`
  - server 实现：`server/gateway/front/front_app.py`
  - server 验收：`server/acceptance/test_session_channel_resident.acceptance.py`（新）、`server/acceptance/test_m4k3_record_session_live.acceptance.py`（planner 扩单入 manifest）
- **Net diff**：9 件修改 `542 insertions(+) / 115 deletions(-)`（`git diff --stat HEAD -- firmware server`）；2 件新验收件 1859 行（`session_channel_resident.acceptance.c` 647 + `test_session_channel_resident.acceptance.py` 1212）。实现面主体 ≈ 400 行（`application.cc` 171 / `application.h` 28 / `front_app.py` 253）。
- **Zone 负锚兑现**：`mcp_server.cc` / `server/gateway/story.py` / `protocol.cc` / `protocol.h` / `display_schema.py` / `server/deploy/**` / `docs/contracts/**` / `docs/requirements.md` / `docs/architecture.md` **零 diff**（`git diff --stat HEAD` 不含这些路径）。`docs/` 面实际改动仅 `docs/debt/tech/**` 三件账本记录（见 docs-delta）。
- **Split recommendation**：单包，不拆。改动内聚在「会话/通道解耦」一条机制链（固件四收尾点 + 60s producer + 巡检 + 心跳 + front 分层收尾），拆开会破坏「统一收口处理器」的单一原子性；P2（`261117-session-end-intent-longretry`）边界已在 `crossPackageBehaviors` 显式登记，不属拆分缺口。

## Verification Evidence

- **Final round fact（承重）**：`delivery/taskpackages/261116-channel-resident-core/verifier-evidence/rounds/r2.json`，`phase=final / round=2 / ok=true / observedGateExit=0`，10 闸全跑（`timing` scoped 8 + full 2）。
- **机器凭据闸**：`node .claude/scripts/verifier-evidence-check.mjs --change-id 261116-channel-resident-core --phase final --round 2 --invocation-id e5cb4370-0470-4df6-a1ab-316ac66cb4aa` → `exit 0 / factOk=true / phase=final / round=2`。
- **Structural gates**：`taskpackage-required`、`change-manifest`（pre-cutoff 存量放行声明）、`firmware-build`（`VERIFY_FW_BUILD_REQUIRED=1` 真编译 `build-pass`）、`contract-lint`、`acceptance-coverage`（15 acceptance id 全覆盖）、`mutation-check`、`docs-sync`、`security`（`securityReviewRequired=false`）——8 闸 ok。
- **Acceptance / unit（full 相位按 scope 圈定真跑）**：`unit` 187 passed / 3 deselected / 22 subtests（14 文件）；`acceptance` 135 passed / 3 deselected / 3 subtests（server acceptance 11 文件，firmware acceptance 面 11 件存在性核对）。
- **Host runner（planner 亲跑 @ working tree，执行目录 `firmware/main/test/host_app`）**：`306 Tests / 3 Failures / 0 Ignored`——3 红全为上游存量 `p1_auxbar_style.acceptance.c` AB-A1 / AB-A2 / AB-A4（`ic_batt_15` / `ic_thermo_12` / `ic_wifi_14`），与本包无调用关系；本包 SCR-A1..A7 与四件重锚件全绿。
- **Server 全量（planner 亲跑 @ 仓根，`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance`）**：`473 passed / 0 failed / 9 deselected`（launch 基线 457 → +16 = 本包新增 host/server 验收面合计；零新增红）。
- **Mutation check**：15 条 per-acceptance 变异逐条「精确字符串改坏 → 对应冻结验收转红 → sha1 校验还原」留痕（builder dispatch 表）；另如实申报 1 条不敏感项（SCR-A4 `RaiseReminderQueue` needle 被既有注释遮蔽，见 Findings N-01）。
- **Docs sync**：`docsSyncRequired=false`，`docs-sync` 闸 PASS；人工复核理由见 `docs-delta.md`（spec/契约/架构/ADR 四类零 diff；两条 debt 待真机结项）。
- **Scope 圈定（范围圈定清单落盘路径一行）**：`delivery/taskpackages/261116-channel-resident-core/test-scope-final.txt`（25 条）。合成 = `acceptanceTestsFile`（6）∪ `impactedTestsSeed[]`（16）∪ `detect_changes`（project `C-AI-KidPal-pod-channel`，since `base/261115-session-channel-resident` / merge_base `3b3832cb`，inbound depth 2：23 changed files / 334 seed 符号 / 103 impacted 节点，其中 firmware/main 21、server/acceptance 73、server/gateway 5、server/tests 4）映射测试 ∪ 安全余量。per-Pod 专属图索引可用 → **正常圈定、未降级**；不缩到本包档、不默认全量。收口 commit 后 detect_changes 复核：仍 334 seed 符号 / 103 impacted 节点（firmware/main 21、server/gateway 5、server/acceptance 73、server/tests 4），与圈定一致，实现面符号影响无外溢。

## Per-Acceptance 证据（15 条）

| Acceptance | 断言载体（测试） | 结果 | 变异判别力 |
|---|---|---|---|
| SCR-A1 四收尾点收口负锚+正锚闭集 | `session_channel_resident.acceptance.c:288` 四收尾点收口负锚 | PASS | 变异 #1（helper 插回 `protocol_->CloseAudioChannel`）→ 本断言转红 |
| SCR-A2 统一处理器动作序 | `:364` 统一处理器动作序 | PASS | 变异 #2（Speaking 分支复制 `ResetDecoder`）→ 转红 |
| SCR-A3 五真拆通道点保绿 | `:421` 五个真拆通道保绿 | PASS | 变异 #3（`Reboot` 真拆点改 `SendStopListening`）→ 转红 |
| SCR-A4 Idle 八件清位链保绿 | `:463` Idle 八件清位链保绿 | PASS | 变异 #4（`kidpal_keybar_scene_reset` → `..._restore`）→ 转红（含 RSL2-A2 / CG-A1 同向共红） |
| SCR-A5 60s producer + VAD 说话沿重置 | `:504` 60s producer + 说话沿重置 | PASS | 变异 #5（常量 60 → 30）→ 转红 |
| SCR-A6 心跳 no-op 分支位次 | `:560` 心跳 noop 分支位次 | PASS | 变异 #6（`kidpal_keepalive` 分支 `== 0` 改 `false`）→ 转红 |
| SCR-A7 自愈巡检短路序 | `:601` 自愈巡检短路序 | PASS | 变异 #7（两操作数互换）→ 转红 |
| SCRF-B1 心跳 producer（front） | `test_session_channel_resident.acceptance.py::test_scr_b1_*`（4 例：默认开/周期恰一帧/`=0` 零帧/异常静默收尾仍落） | PASS | 变异 #8（极性反转）→ `test_scr_b1_heartbeat_gate_default_on_and_rollback_off` 转红 |
| SCRF-B2 `listen stop` 分层收尾 | `test_scr_b2_chat_noop_and_accumulate_consumed_once`、`test_scr_b2_pending_protected_and_repeated_stop_idempotent` | PASS | 变异 #9（pending 保护改 cancel）→ 转红 |
| SCRF-B2b pending 保护 + sweep 释放 | `test_scr_b2b_sweep_releases_at_ten_seconds`、`test_scr_b2b_stt_recovery_prevents_double_release` | PASS | 变异 #10（sweep 释放改 `pass`）→ 转红 |
| SCRF-B3 discard→stop 有序不归档 | `test_scr_b3_discard_then_stop_no_archive` | PASS | 变异 #11（discard cancel 改 `dropped=False`）→ 转红 |
| SCRF-B4 ctx 边沿兜底分态 | `test_scr_b4_accumulate_cancels_and_releases`、`test_scr_b4_pending_stop_no_action`、`test_scr_b4_chat_marked_only_release` | PASS | 变异 #12（accumulate 删 cancel）→ 转红 |
| SCRF-B4b pending_stop 边沿不动作（F-01 防线） | `test_scr_b4b_pending_stop_edge_then_stt_archives_once` | PASS | 变异 #13（pending 保护改 cancel+释放）→ 转红 |
| SCRF-B5 连接级 teardown 保绿 | `test_scr_b5_teardown_completes_cancel_and_legs_removed` | PASS | 变异 #14（teardown cancel 改 `pass`）→ 转红 |
| SCRF-B6 既有冻结 oracle 保绿回归网 | `test_scr_b6_frozen_oracles_present` + 全量 suite 绿留痕 | PASS | 变异 #15（挂起 `test_m4k1` 文件）→ 转红 |

> firmware SCR-A* 断言的 PASS 证据来自 host runner（planner 亲跑 306/3/0）；`verify.mjs` 的 `acceptance` 闸对固件面只作存在性/结构核对（on-target 需实机），已在闸 output `note` 明示。

## 实施偏离与扩单留痕（builder 三处 + planner 两笔）

**Builder 三处实现偏离（planner 已接收，随本包申报）**：

1. **记录语境标记清位按会话键跨连接**：`_record_ctx_clear(session_id, legs)`（`front_app.py:983`）遍历全部已登记 device websocket 按 `session_id` 清位，而非仅按 `legs` 的 device_ws 清。理由（docstring `:984-990`）：泵直调/离线线束下「收 ctx 的 device ws」与「归属腿 device ws」可能非同一对象，逐腿清会留残标记 → 重复 ctx 触发二次释放破坏 SCRF-B4 幂等。会话键每连接唯一（HWV-A11），语义仍等价「按连接清」。**评估**：正确性成立，代价是释放时 O(n) 扫描存活连接（n 极小），见 Findings N-03。
2. **审计行追加 `state=` 后缀**：四路审计行由原 `state=idle` 改为语义化值——`discard_record`→`state=session_end_requested`（`application.cc:1026`）、`toggle_chat`→`state=session_end`（`:1075`）、`wake_invoke`→`state=session_end`（`:1565`）、`overlay_close` 维持 `state=idle`（`:1215`）。`local_close_settle` + `src=` 冻结 token 零变，故 OCE-A1 / SCR-A1 region 断言（只钉 `local_close_settle` / `src=overlay_close` / `src=<路>` 存在性）不受影响。**评估**：诊断口径更细，但 `state=` 词汇三态不统一，见 Findings N-02。
3. **注释刻意不复述判据字面**：`application.cc` 的 T2 producer / T4 巡检 / T3 keepalive 三处注释明写「注释刻意不写判据原文的符号字面——验收件按源文本计数，注释里的字面会稀释断言区分力」。**评估**：这是对源文本计数断言的合理防稀释，但把「注释不得写回字面」变成隐性维护约束，见 Findings N-01 同族观察。

**Planner 扩单两笔（均已回评补 manifest）**：

1. `firmware/main/test/record_dialogue_guide.acceptance.c` 重锚：builder 首跑曝露 RCL-F1 粗粒度区切（`HandleStopListeningEvent`→`HandleWakeWordDetectedEvent`）误跨统一处理器致假红；最小重锚 = `fn_end` 收窄为 `void Application::HandleEndVoiceSessionEvent` + REVISION LOG，断言语义零变化（tighten-only）。该件 1g 扫描漏件，已入 manifest 与 impactedTestsSeed。
2. `server/acceptance/test_m4k3_record_session_live.acceptance.py` 入 manifest：package-split §2.1 v1.2 已升为非条件成员（A7 ctx 非 story 零帧语义前提精确化 + 新增释放边沿姊妹齿），原文清单漏列；断言零删弱。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （本包新增代码无 `minimal:` 标记） | — | — | `no-trigger` |

> `front_app.py:2654` 的既有 `minimal:` 标记属 `261022-p3-record-front-polish-fix` 的 `_story_sweep_loop`（本包零改该函数），不归本包债务台账。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | Test quality | SCR-A4 的 `RaiseReminderQueue` needle 被 `application.cc:1342` 既有注释字面遮蔽，该 needle 单独不具判别力（变异后 SCR-A4 仍绿，仅 `reminder_overlay_unify` RM-A3 转红）。 | changed-code hunk `session_channel_resident.acceptance.c` SCR-A4 断言表；builder mutation「不敏感项」实测（变异后 SCR-A4 绿、RM-A3 红）；`application.cc:1342` 注释含该符号 | 不阻塞：builder 已用同区 `kidpal_keybar_scene_reset` 承载敏感变异，SCR-A4 整体仍有判别力；留作验收质量观察项，后续触及该区时把 needle 换到代码单点、注释无该字面（交 P2 或专门验收批）。 |
| NIT | judgement-call | Operability | 同族审计行 `state=` 取值三态不统一（`session_end` / `session_end_requested` / `idle`），按 `src` 分别解析日志时需额外汇总规则。 | changed-code hunks `application.cc:1026` / `:1075` / `:1215` / `:1565` | 不阻塞：登记为 builder 偏离；若真机 KPLOG 解析需要统一，P2 归一或加解析别名，本轮不改。 |
| NIT | judgement-call | Maintainability | 实现注释「刻意不复述判据字面」把「注释不得写回被 count 的 token」变成隐性约束；未来编辑若在注释里写回 token 会使源文本计数断言假绿。 | changed-code hunks `application.cc` T2 producer / T4 巡检 / T3 keepalive 注释明写该约定 | 不阻塞：建议在新验收件头注一句「本区注释刻意避判据字面」，防止回摆（可后续批处理；本轮内容治理已由 builder 注释留痕）。 |

> 无 BLOCKING、无 SHOULD（hard-violation 零条）。三条均为 NIT 级 judgement-call，均有 changed-code hunk + heuristic 依据，不伪造 rule citation。

## Five-Axis Review

**1. Correctness（S-Fx.y / 边界条件）**
- **四收尾点收口形态二分正确**：协议接收任务上下文（`CloseAudioChannel` 历史名 helper，调用链 `OnIncomingJson → McpServer::ParseMessage → mcp 回调`）只投事件位 `MAIN_EVENT_END_VOICE_SESSION`（`application.cc:1000-1002`）；主循环上下文（toggle / wake-invoke / CLOCK_TICK 60s）直调 `HandleEndVoiceSessionEvent()`。事件位在 `Run()` `:378` 单源消费。两类不混用，避免跨任务竞态。
- **处理器守卫与动作序逐字保留**：`!protocol_` 早退（`:1189`）、`auto_connect_in_flight_` 守卫（`:1192`）、三态门（Connecting/Listening/Speaking，`:1199`）均在 `protocol_->SendStopListening()`（`:1211`）之前，空指针与竞态安全成立；Speaking 分支 `ResetDecoder()` 先于 `AbortSpeaking` 先于 `SendStopListening` 与 abort 帧同 TCP 有序。
- **60s producer 计时单位正确**：`clock_timer` 周期 `esp_timer_start_periodic(..., 1000000)` = 1 秒（`application.cc:258`），`clock_ticks_++` 每 tick（`:441`），故 `clock_ticks_ >= kSessionIdleTimeoutS(=60)` = 60 秒；状态迁移（`:1282`）与 VAD 起音沿（`:409`）归零。四判据（聆听态 + `!s_vad_speaking` 电平门 + 计时到阈 + 记录语境豁免）齐备；电平门是「连续讲 >90s 不被误收」的承重件（仅沿重置会在第 60 秒误收）。
- **巡检零第二条重连路径**：`Idle && protocol_ && !auto_connect_in_flight_ && !IsAudioChannelOpened()` → `TryStartAutoConnect()`（`:498-503`），守卫短路序 in-flight 先于通道判据（降频承重），区内零直连建连、不新建 task、不复用 `clock_ticks_`、不写 `clock_ticks_ % 10`。
- **front 分层收尾**：`pending_stop` 当刻绝不 cancel（F-01 保护，`front_app.py:1996-2000`）；`accumulate` 已被归档路径消费不动作；`chat` + 标记真只释放；ctx 边沿分态（pending 不动作 / accumulate cancel+释放 / chat 标记真只释放）；未知 `source_page` 仍拒绝 + warning + 不释放（`elif` 逐字保留）。`_story_timeout_sweep` 补 `_release_record_context`（承重缺口闭环）。
- **心跳**：sleep-first（建连零帧，不污染三件真跑 `ws_gateway` 的零偏移前提）、裸 `send_text`（非 display 帧）、`except Exception` 静默退出（`CancelledError` 是 BaseException 不被吞）、`finally` 首行 `cancel()` + `gather(return_exceptions=True)`、不进 `FIRST_COMPLETED` 集合。

**2. Test quality**
- 15 acceptance 各有独立断言载体，且逐条有 per-acceptance 变异转红证据（见上表）；变异含「删/换符号/查极性/交换短路序」多型，非仅覆盖率点缀。
- 断言面区分力真实：源文本计数断言对注释字面敏感（builder 主动避字面防稀释），结构性断言用命中计数 + 位次偏移 + 负锚。
- 观察：SCR-A4 `RaiseReminderQueue` needle 被注释遮蔽（N-01）；注释-断言耦合（N-03 同族）已在 Findings 登记，不影响本轮判别力总量。

**3. Contract / boundary safety**
- `docs/contracts/**` 零 diff；本包 T3/T5–T8 是契约 §四（`kidpal_keepalive` / `kidpal_session_ctx` / `listen` / `kidpal_story_discard`）的**实现落地**，无新帧、无新 view（display 白名单不扩）、无 `SPEC_CONTRACT_DRIFT`。
- discard→stop 顺序约束依赖「`SendKidpalStoryDiscard` 帧先（调用方在同一 TCP 上行）、事件位在主循环下一拍才发 listen stop」——`mcp_server.cc` 零改动即守住 P2 边界与契约 §四顺序。
- `story.py` 三缝（`release_pending` / `cancel` / `PENDING_STOP_SECONDS`）零改动；engine-patch / ECS `core/**` / `close_connection_no_voice_time` / `end_prompt.enable` 零 diff。

**4. Operability / security**
- 心跳极性正确：`KIDPAL_CHANNEL_HEARTBEAT` **代码默认开**，`0/false/off/no` 为回滚开关（与 `KIDPAL_STORY_RESTORE_PUSH` 极性相反，理由 = 默认关会让新固件退化 churn）；周期 `KIDPAL_CHANNEL_HEARTBEAT_INTERVAL_S` 默认 30。
- 审计 token 带 `corr_id=kdp-<ts>`，四路 src 可区分；keepalive no-op 避免每 30s 一条 `Unknown message type` WARN 冲 SD 日志轮转预算。
- `securityReviewRequired=false`；无新 secret、无权限面变更、无外部输入解析新增攻击面（心跳帧仅 `strcmp` 识别 type 后 return）。
- 观测风险：`IsTimeout()` 日志密度真机未量测（见未覆盖面 #5）。

**5. Maintainability / simplicity**
- `CloseAudioChannel()` 保留历史名（头注明确「名字为历史名、语义 = 会话收尾」，改名须碰 `mcp_server.cc` = 越界），删除 `ScheduleResidentRebuild()` / `kResidentRebuildDelayS` 出清冷重建面；`kSessionIdleTimeoutS` 接替常量位置。
- `_record_ctx_seen` 沿 `_POINTS_OBSERVED` 先例（`WeakKeyDictionary` + 属性回退），清位单点归 `_release_record_context`，三既有调用点与新增 sweep / ctx 边沿自动一致。
- 无过早抽象、无重复实现、无新依赖。既有 `minimal:` 标记不归本包。N-02/N-03 为可维护性观察项，不阻塞。

## Demo 对账行（workstream 包纪律）

> 本包 demo 形态（roadmap §3 与 plan「demo 证据形态」节）= host runner + server pytest + device build + 一轮真机轮 A（human 持板）。逐形态对账：

| 证据形态 | 实际结果 | 与 roadmap 效果段（§① 目的 1/2/4/6）一致性结论 |
|---|---|---|
| host runner（设备 C 验收，结构/行为半） | `306 Tests / 3 Failures / 0 Ignored`；3 红全为上游存量 `p1_auxbar_style` AB-A1/A2/A4；SCR-A1..A7 + 四件重锚件全绿 | 与「实现已 land、结构断言真」一致；**不足以单独证明**用户可见效果（时延 / 连续讲 / 自愈），效果确认待真机轮 A |
| server pytest（front 验收） | `473 passed / 0 failed / 9 deselected`；SCRF-B1..B6 全绿 | 与「front 分层收尾 / 心跳 / ctx 分态离线成立」一致；真实 30s 节奏与真机帧序待轮 A |
| device build | `firmware-build` 闸 `VERIFY_FW_BUILD_REQUIRED=1` 真编译 `build-pass`（xiaozhi.bin 生成） | 与「编译面全绿」一致 |
| 一轮真机轮 A（human 持板，front 由 agent 部署） | **PENDING（尚未发生）**——本包无 `device-round-evidence.md`（无 human 目击不得生成） | roadmap 目的 1/2/4/6 的**用户可见效果段全部待此轮**；判据 = `plan.md` `exitCriteria.KRC-E4` 的 A1–A13：A1 温通道进聆听 ≤0.82s 对照 KPLOG029；A2 连续 5 次零冷通道零 burst；A3 闲置 >5 分钟首按键一次进聆听 + 心跳痕迹 + 零 Unknown；A4 提醒必达真出声；A5 记录会话全链四式；A6 60s 静默自动关闭首次真机发生；A7 连续讲 >90s 不被误收；A8 录音态提醒排队例外；A9 查字窗关闭路径；A10 24h 日切（测试环境临时调小）；A11 SD 日志零 Unknown 噪声；A12 RC-B6 省电档观测口径重定；A13 front/固件回滚剧本可执行（两半一起回滚）。三源日志（设备 KPLOG / front systemd / 引擎 docker）互证，证据落本包 `current/` |

## 未覆盖面申报（覆盖断言覆盖轴的设计面差集）

> 对照 `acceptance[]` 15 条 + 契约条目 + roadmap §① 效果段，逐项列「本次无验收断言 / 未跑的设计性质 → 现状 → 去向」。**非 `none`**（真机半与长跑形态均未覆盖）。

| # | 设计性质（无断言/未跑） | 现状 | 去向 |
|---|---|---|---|
| 1 | 温通道按键进聆听 ≤0.82s（目的 1） | 未跑：离线无真实 TLS/AFE 时延轴 | 轮 A A1（对照 KPLOG029） |
| 2 | 连续讲 >90s 不被误收（目的 4 反面） | 未跑：离线仅静态断言电平门在场（SCR-A5） | 轮 A A7 |
| 3 | 心跳 30s 真实节奏 + SD 日志心跳痕迹（目的 2） | 未跑：离线注入假时钟（SCRF-B1） | 轮 A A3 |
| 4 | 三方并发写同一 device websocket 无锁（上泵 / sweep `_story_finish_session` / 心跳） | 未测：front 现无 asyncio 发送锁，P1 不新建锁（超范围） | accepted residual：心跳帧极小、单次 await，实际风险低；轮 A 观测间发/丢帧；若需锁另立批 |
| 5 | `IsTimeout()` 日志密度（T4 事实降频是否达成） | 未量测：离线无法计真机日志速率 | 轮 A A3/A12 带量测；若仍 >每轮一条，回报 planner 走扩单（`protocol.cc` + `protocol.h` 双入 manifest） |
| 6 | `_story_timeout_sweep` 10s 窗口后在途抑制的 accepted residual（listen stop 后 <10s 首次话语可能被抑制、再按一次即恢复且 ≤10s 已释放） | 未以断言覆盖为「可接受形态」：B2/B2b 只断言 pending 保护与 +10s 释放，不断言 <10s 语音行为 | accepted residual（KRC-E4 A5 明列「间隔 <10s 允许首次话语被抑制 = accepted residual 但再按一次即恢复」）；轮 A A5 观测 |
| 7 | 心跳 / `listen stop` 迟到下行帧（stt / sentence_start / tts）把人拽回思考/播报 | 未测：通道常驻后四条收尾路径首次获得该行为 | 轮 A 观测项（「结束后 1–2s 内屏幕是否被迟到帧拽回」）；**非 P1 要修的缺陷** |
| 8 | P1 把「3 次退避即止→静默」变事实长跑（每 ≈35s 一轮 TLS 握手、无上限） | 未测：离线逻辑不改，形态在真实死链期才显形 | 轮 A A3/A12 观测；封顶长跑归 P2 SEI-A2/A3（`crossPackageBehaviors` 已登记形态基线） |
| 9 | `auto_connect_in_flight_` 守卫期（≈35s 窗）内结束会话被静默吞（仅 INFO 日志） | 无断言覆盖：守卫为 261024 逐字保留、P2 SEI-A4 重锚 | 残差/观测；P2 重锚为「守卫在场 + 置意向位 + 审计行」 |
| 10 | 引擎 24h 日切为唯一剩余重建窗口（C7） | 未跑：需测试环境临时调小 `close_connection_no_voice_time` | 轮 A A10 |
| 11 | 录音态提醒排队例外（目的 6 / S-F1.4） | 未跑：离线仅 RM-A3 静态 `RaiseReminderQueue` 在场 | 轮 A A8 |
| 12 | 查字窗关闭路径（播报完不回聆听、驻留按键或 5 分钟） | 未跑 | 轮 A A9 |
| 13 | 断网 / front 重启自愈（目的 5，P2 主面；P1 仅巡检雏形） | 未跑 | P2 轮 B（B4/B5）；P1 A3 部分观测 |
| 14 | 录音会话全链四式（KEY 结束无归档 / 间隔 >10s 正常应答 / KEY 收尾归档 / BOOT 丢弃 / 2 轮无回应收尾） | 离线 SCRF-B2/B3 覆盖部分，真机全链未跑 | 轮 A A5 |
| 15 | RC-B6 省电档观测口径重定（决策 A6） | 未跑 | 轮 A A12 |
| 16 | front/固件回滚剧本可执行（两半一起回滚） | 未演练 | 轮 A A13 |

## 回滚路径与 Merge Packet

- **原子可回滚**：全部实现/验收/簿记改动落在本地 feature 分支 `dev/pool-channel` 的单个收口 commit（31 文件 / +4300 −370；hash 见本包 git log）。回滚 = `git revert <该收口 commit>`（或合并前 `git reset --hard` 至父提交），一步回到 P1 前状态。
- **两半一起回滚（承重）**：front 心跳默认开、设备侧升级为「120s 无心跳 = 死链」；只回滚单侧会让设备退化 churn 或空转。故代码回滚必须 front（`front_app.py`）与固件（`application.cc`/`application.h`）同批；紧急单侧回滚可用 `KIDPAL_CHANNEL_HEARTBEAT=0` 关 front 心跳（回滚开关），但同时须确保设备侧 heartbeat no-op 分支仍在（新固件收到 keepalive 仅一条 DEBUG 或无输出，无副作用）。
- **merge packet**：本地合并 = planner 在 supervisor APPROVE + 前置升级闸（`pre-merge-escalation-gate.mjs`）放行后 `merge-driver --pod`，integrator 不 push、不 merge 主干。合并前排他：P2 从新 main 起分支、重取行号锚（见 `crossPackageHandoff`）。
- **INDEX 投影**：integrator 相位以只读跑（`indexProjection: skipped-read-only`；`changes-index.mjs --upsert` 是 review-lane 专用、对 dev id 报命名错）；**planner 收尾相位已用 `node .claude/scripts/closeout-check.mjs --update-index` 补写本包行**（`delivery/taskpackages/INDEX.md:120`），并随 `cc6f6667` 提交。

## Residual Risk

- **Known limitation**：
  1. 本包全部「用户可见效果」类判据（时延 / 连续讲 / 自愈 / 提醒 / 60s 自动关闭）只有离线结构/单元证据，**真机轮 A 完全未发生**——roadmap 目的 1/2/4/6 的效果段尚未被任何证据闭合。
  2. 心跳三方并发写无锁（未覆盖面 #4）为 accepted residual。
  3. `_story_timeout_sweep` 10s 窗口内在途抑制为 accepted residual（未覆盖面 #6）。
  4. `IsTimeout()` 日志密度与「3 次退避变长跑」形态变更（未覆盖面 #5/#8）待真机量测/观测。
  5. `auto_connect_in_flight_` 守卫期结束会话被静默吞（未覆盖面 #9），归 P2 SEI-A4 重锚。
- **Follow-up recommended**：
  1. 真机轮 A（A1–A13）作为本包出口（`exitCriteria.KRC-E4`）；证据落本包 `current/`，三源日志互证。
  2. 真机后由主线程按真机凭据结项 `/docs/debt/tech/TECH-261114`（A1+A2）与 `TECH-261012`（A3）并 `debt-render.mjs` 归位。
  3. P2（`261117-session-end-intent-longretry`）从新 main 起分支、重取行号锚，接 `crossPackageBehaviors` 五项（含 SCR-A2 in-flight 守卫重锚、`mcp_server.cc` 注释双源）。
  4. `requirements:48` ⚠️ 对账注记由主线程按 workstream 留根 1 路由刷新（本包零 diff）。
- **Human decision needed**：
  1. **真机轮 A（A1–A13，human 持板）**——本包唯一未闭合的出口；未发生前不得宣布 workstream 目的达成、不得结项两条 debt。
  2. 回滚剧本执行口径（A13，两半一起回滚）由 human 在真机轮确认可执行。
  3. **G4 工具缺口（收尾相位实测）**：`supervisor-gate-check.mjs` 的 G4 要求 `.claude/verify-logs/*.log` 非空，但 `verify.mjs` 仅在 gate 失败时写日志（`verify.mjs:288-290` 在 `if (!ok)` 内）——本包 scoped r1 / final r2 全绿 → 零日志 → G4 结构性恒红，会话无法正常收尾。诊断与 patch 见 `gate-tooling-gap-g4.md`；待裁清单见 `pending-human-ruling.md`。`.claude/**` 对 agent 禁写（`scope-map.universal.bashDenyWrite`），planner 无权自修。
- **跨面变更（spine 在册）**：本包收口后建议 status = `ready-to-merge`（待 supervisor 复核 + 前置升级闸放行 + merge-driver 本地合并），workstream `261115-session-channel-resident` P1 锚点更新归主线程（integrator 不直改 spine）。