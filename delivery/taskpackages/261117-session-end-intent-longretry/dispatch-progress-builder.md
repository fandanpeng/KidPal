# dispatch-progress-builder · 261117-session-end-intent-longretry（P2）

> builder 实施留痕（不含 tokens / gates / rounds / verdict）。

## 实施段 1 · 注册 + 设备侧（SEI-C1 / A1 / A2 / A3 / A4）

- `firmware/main/test/host_app/main/CMakeLists.txt`：`HOST_ACCEPTANCE_SRCS` 追加
  `"../../session_end_intent_longretry.acceptance.c"`，置于 `host_infra_smoke.acceptance.c`
  之前（smoke 末位）；host 全量由 322 → 327 Tests。
- `firmware/main/mcp_server.cc`：
  - control 分派 `case 0`（end_session）`AbortSpeaking(kAbortReasonNone)` → `EndVoiceSession()`
    （事件位投递、不直调处理器）；注释按 OQ3=c 改写（保留 end_session 字样）。
  - DISCARD_RECORD 注释按 P1 转手项改写为「丢弃帧先行、listen stop 后至、front 先按
    discard 打回 routing 才不归档」；沿用「listen stop」无括号措辞（RSL-A5 tooth 4 读原始
    文件不剥注释）。
- `firmware/main/application.cc` 常量区：退避表 `{5,10,20,40,60}` + 新增
  `kAutoConnectSteadyRetryS = 60`（注释不复述符号字面，避免源锚切到注释）。
- `firmware/main/application.cc` `TryStartAutoConnect`：task 体改封顶递增长跑（无界循环、
  稳态 60s），去 `attempt < N` 字面；三退出条件（成功 / bail 重查 / 网络断请求位）齐备；
  退避按 1 秒分片重查意向位与退出位；`auto_connect_in_flight_` 单点收尾清位，成功路径
  先清 in-flight 再经 `Schedule` 补投意向位。
- `firmware/main/application.cc` 网络 handler：`HandleNetworkDisconnectedEvent` 函数体最
  顶端（三态门之外）置 `network_exit_requested_`；`TryStartAutoConnect` 预检早退；`Handle-
  NetworkConnectedEvent` 建连成功路径 + Idle 支路（调自动建连前）清位。
- `firmware/main/application.h`：新增 `bool network_exit_requested_ = false;` 与
  `EventBits_t pending_manual_intent_ = 0;`（紧邻 `auto_connect_in_flight_`）。
- `firmware/main/application.cc` 五个手工入口 in-flight 分支（ToggleChat / StartListening /
  EndVoiceSession / WakeWordDetected / WakeWordInvoke）：各置 `pending_manual_intent_` +
  审计行 `manual_intent_queued`，不新增任何 `return;`；StartListening 插入槽在
  `kidpal_story_session_ctx_set(0)` 之后、`return;` 之前（RSL2-A3 序钉）。

## 实施段 2 · 前端侧（SEI-B1 / B2 / B3 / B4）

- `server/gateway/display_schema.py`：新增模块级
  `SELF_PUSH_EXEMPT_CONTROL_ACTIONS = frozenset({"start_organizing", "end_session"})`，
  豁免口改集合成员判定；`end_session` 保留在 `GATEWAY_DECISION_CONTROL_ACTIONS`；
  反转 docstring / 注释「即便自推仍拒」。
- `server/gateway/front/front_app.py`：新增 `END_SESSION_PAYLOAD` 与
  `_consume_end_session_intent`（按 `_consume_points_intent` 同型，自推走既有
  `_push_display_payload` 单源）；`_relay_display_blocks_to_device` 在 points 截获之后、
  build_display_tool_calls 之前接入，命中即 `continue`；反转 `_push_display_payload`
  docstring；审计 token 不复用 `story_record_ctx_enter`。
- `server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`：home 行收窄为
  `| home | 无需显示内容的普通答复 | 空 |`（去「再见/结束」）；回合路由表新增「告别回合」
  行（拜拜/不用了/先到这儿/我不聊了 → `control/end_session`，不得用 home 承载）。
  `OPENCLAW_SYSTEM_PROMPT_CARRIER` 与 `provider.config.yaml` 目标零 diff（现状已满足
  三面相等，未改）；`openclaw-system-prompt.v11.md` 零 diff。

## 全绿结果

- host：327 Tests / 3 Failures（唯一允许红 = 上游存量 p1_auxbar_style AB-A1/A2/A4）。
- server acceptance：502 passed / 9 deselected / 43 subtests。
- server tests：304 passed / 94 subtests。

## 变异证据（每条 SEI：改坏 → 对应断言转红 → 还原；工作树无残留）

| SEI | 变异点 | 转红断言 | 实测输出摘要 |
|---|---|---|---|
| SEI-C1 | CMakeLists 删登记行 | SEI-C1 | "HOST_ACCEPTANCE_SRCS 无本冻结件登记行" |
| SEI-A1 | case 0 改回 `AbortSpeaking(kAbortReasonNone)` | SEI-A1 | "case 0 内 EndVoiceSession( = 0 处（冻结恰 1）" |
| SEI-A2 | 退避表改回 `{5,10,20}` | SEI-A2 | "首定义缺档位（须含 5/10/20/40/60 全五档）" |
| SEI-A3 | 删 HandleNetworkDisconnectedEvent 的 `network_exit_requested_` 置位 | SEI-A3 | "HandleNetworkDisconnectedEvent 区无 network_exit_requested_ 置位" |
| SEI-A4 | ToggleChat 分支删 `pending_manual_intent_` 置位 | SEI-A4 | "ToggleChat in-flight 分支未置 pending_manual_intent_" |
| SEI-B1 | 豁免集缩回 `{start_organizing}` | SEI-B1 | "自推通路仍拒 end_session" |
| SEI-B2 | 断开 end_session 截获接线（`if False and ...`） | SEI-B2（两条） | 设备腿 control/end_session 帧数 = 0 |
| SEI-B3 | `_consume_end_session_intent` 自推两帧 | SEI-B3（两条） | 单块设备显示帧总数 = 2（应恰 1） |
| SEI-B4 | home 行重加「再见」 | SEI-B4 | "home 行仍含 '再见'" |