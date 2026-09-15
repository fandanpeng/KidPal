# input-brief · builder · 260829-hardware-validation

> **非权威派生物**（脚本生成，Git-tracked）：本文件是**本次派发消费期的封闭输入清单快照**，不是事实源——契约 / spec / 代码各自仍是其自身 SSOT。改本文件无意义，改 refs 后重跑脚本。
> **行号快照语义**：下文行号 = 生成时点解析结果，**非长期事实源**；消费时若 symbol 已移位，重跑 `brief-inputs.mjs` 取新行号，不得照旧行号通读整文件。
> generated-at: 2026-09-09T15:20:05.913Z · 生成器: .claude/scripts/brief-inputs.mjs · 消费方: builder
> 来源 TaskPackage: `.claude/taskpackages/260829-hardware-validation/taskpackage.json` · subsystem: firmware+server · rigor: full

## ① 契约 / spec 锚点节（定位到节，**不抽正文**——正文仍在 SSOT，读它）

| 路径 | 锚点 | 节标题 | 节首行 | 状态 |
|---|---|---|---|---|
| `docs/contracts/interface-contract.md` | — | 无锚点（整文件） | — | bare |
| `.planning/kidpal-mvp/PROPOSAL.md` | — | 无锚点（整文件） | — | bare |
| `docs/spec.md` | `§2` | 2. 硬件能力盘点（依据本地官方 Demo 实测确认） | 20 | ok |
| `docs/spec.md` | `§5.4` | 5.4 页面结构（4 页循环） | 497 | ok |
| `9b` | — | （文件不存在） | — | missing |

## ② 跨端对端面（crossEnd · 本批限自有实现面 firmware/** · server/**）

| 对端文件 | symbol | 行号区间（生成时点） | 其余命中行 |
|---|---|---|---|
| `server/gateway/front/front_app.py` | `ws_gateway` | :507-527 | — |
| `server/gateway/auth.py` | `authorize` | :6-46 | 34 |
| `firmware/main/protocols/websocket_protocol.cc` | `WebsocketProtocol` | :18-20 | 22, 26, 31, 63, 77, 81, 85, 239, 264 |
| `firmware/main/certs/ws_tls_ca.cc` | `VerifyCacertConfigured` | :16-25 | — |
| `server/gateway/front/front_app.py` | `build_display_tool_calls` | :202-207 | 247 |
| `server/gateway/mcp_relay.py` | `display_tool_call` | :10-11 | 99 |
| `server/gateway/display_schema.py` | `validate_display` | :6-7 | 55 |
| `firmware/main/display/view_router.c` | `kidpal_display_route` | :18-51 | — |
| `server/gateway/story.py` | `list_stories` | :177-190 | — |
| `server/gateway/story.py` | `story_text` | :210-211 | — |
| `server/gateway/story.py` | `StoryAccumulator` | :243-283 | — |
| `firmware/main/display/story_page.c` | `kidpal_story_list_parse` | :475-578 | — |
| `firmware/main/display/story_page.c` | `kidpal_story_text_parse` | :613-694 | — |
| `firmware/main/display/story_page.c` | `kidpal_story_open_reader` | :921-946 | — |
| `server/gateway/reminders.py` | `build_alert_envelope` | :287-308 | 326, 494 |
| `server/gateway/reminders.py` | `AlertDispatcher` | :400-410 | — |
| `server/deploy/openclaw-patch/reminder_schedule_lib.py` | `plan_reminders` | :328-334 | 462 |
| `server/deploy/openclaw-patch/reminder_schedule_lib.py` | `write_pending_atomic` | :390-391 | 464 |
| `firmware/main/display/reminder_page.c` | `kidpal_alert_parse` | :507-616 | — |
| `firmware/main/display/reminder_page.c` | `kidpal_reminder_receive` | :772-800 | — |
| `server/gateway/front/front_app.py` | `pump_client_to_upstream` | :304-306 | 540 |
| `firmware/main/application.cc` | `InitializeProtocol` | :335-341 | — |
| `server/gateway/front/front_app.py` | `_story_route_display_payload` | :81-107 | 243 |
| `server/gateway/story.py` | `archive_story` | :92-93 | 253, 331, 389 |
| `firmware/main/display/story_page.c` | `kidpal_story_page_input` | :750-824 | — |
| `firmware/main/display/story_page.c` | `kidpal_story_on_saved` | :991-1006 | — |
| `firmware/main/display/story_page.c` | `kidpal_story_rec_tick` | :858-881 | — |
| `firmware/main/display/story_page.c` | `kidpal_story_stamp_spec` | :971-981 | — |
| `firmware/main/display/story_page.c` | `kidpal_story_on_control` | :830-850 | — |
| `firmware/main/mcp_server.cc` | `DoToolCall` | :940-941 | — |

上述对端解析器源码 = **本包必读输入**：跨端 wire 形状断言须对照对端实际接受什么来钉（数字 id vs 字符串 id、未知 type 处理、字段是否字符串化），不得按本侧契约想象。

## ③ 先例（同 subsystem 已 merged 包，封顶 3 个，按 changeId 日期前缀倒序）

| 先例包 | 冻结验收文件 |
|---|---|
| 260901-ui-art-integration | `firmware/main/test/ui_art_integration.acceptance.c` |
| 260829-char-stroke | `firmware/main/test/char_stroke.acceptance.c` · `server/acceptance/test_char_stroke.acceptance.py` |
| 260829-schedule-reminder | `firmware/main/test/schedule_reminder_b.acceptance.c` · `server/acceptance/test_schedule_reminder.acceptance.py` |

封顶即上限：**不得再自行扩先例阅读面**（治「把先例读了个遍」长尾）。确需更多先例 → 写进超读申报段。

## ④ 清单外阅读申报（超读是常态动作，不申报才是问题）

交付报告须原样带下表回传。清单外阅读**允许**；空表 = 本次未超读。planner 据非空表落一条 `over-scan` friction（advisory 记账，绝不当门）。

| 清单外文件 | 原因（一句话） |
|---|---|
| （无） | — |

