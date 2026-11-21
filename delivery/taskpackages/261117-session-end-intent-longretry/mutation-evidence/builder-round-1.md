# 变异证据 · builder 首轮（2026-11-19）

> 执行 = builder（派发 `ab73fc3fdf732c083`），落盘 = planner 代落（builder 无 `delivery/taskpackages/*/mutation-evidence/**` 写权）。
> 方法：每条 SEI 做一次「临时改坏 → 跑 → 确认对应断言转红 → 还原」；最终工作树全绿即还原后取证，无 MUTATION 残留（builder 已 grep 复核）。

| SEI | 变异点 | 转红断言 | 实测输出摘要 |
|---|---|---|---|
| SEI-C1 | `host_app/main/CMakeLists.txt` 删本冻结件登记行 | SEI-C1 | 「HOST_ACCEPTANCE_SRCS 无本冻结件登记行」 |
| SEI-A1 | `mcp_server.cc` `case 0` 改回 `AbortSpeaking(kAbortReasonNone)` | SEI-A1 | 「case 0 内 `EndVoiceSession(` = 0 处（冻结恰 1）」 |
| SEI-A2 | `application.cc` 退避表改回 `{5,10,20}` | SEI-A2 | 「首定义缺档位（须含 5/10/20/40/60 全五档）」 |
| SEI-A3 | 删 `HandleNetworkDisconnectedEvent` 的 `network_exit_requested_` 置位 | SEI-A3 | 「HandleNetworkDisconnectedEvent 区无 network_exit_requested_ 置位」 |
| SEI-A4 | ToggleChat in-flight 分支删 `pending_manual_intent_` 置位 | SEI-A4 | 「ToggleChat in-flight 分支未置 pending_manual_intent_」 |
| SEI-B1 | `display_schema.py` 豁免集缩回 `{start_organizing}` | SEI-B1 | 「自推通路仍拒 end_session」 |
| SEI-B2 | 断开 `front_app.py` 截获接线（`if False and ...`） | SEI-B2（2 条） | 设备腿 control/end_session 帧数 = 0 |
| SEI-B3 | `_consume_end_session_intent` 自推两帧 | SEI-B3（2 条） | 单块设备显示帧总数 = 2（应恰 1） |
| SEI-B4 | `AGENTS.md` home 行重加「再见」 | SEI-B4 | 「home 行仍含『再见』」 |

## 如实边界（builder 申报，planner 记录）

- **mcp_server case 0**：offline 只证事件位投递形态；「设备真收口、麦克风关、OQ3=c 页回落、通道仍常驻」是运行时本体，host 不可编，归真机轮 B B1。
- **意向位不设时效**（planner 裁决 b）：超长离线后补投的观感项不在 offline 断言内，归真机轮 B B8 观测。
- **网络断退出请求位**：offline 只证源锚在场；真实 ≤1s 退出与重连节奏归真机轮 B B4。