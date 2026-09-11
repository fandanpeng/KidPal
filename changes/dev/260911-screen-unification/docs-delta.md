# docs-delta · 260911-screen-unification（PKG-2 归一屏）

> TaskPackage `.claude/taskpackages/260911-screen-unification/taskpackage.json`
> `docsSyncRequired = false`，`docsSyncReason` 已在异议：本包**无 docs 面变更**。

## 结论：无 docs 面变更（无需回灌 canonical docs）

- **spec 零修订**：`docs/spec.md` 零触碰。SPEC-1 本批不触发——OQ3=c 无独立会话
  语义变更、OQ4=A 已在 PKG-1 侧定为 spec 零修订；本包只重排显示面结构/归属，
  不产生新的会话/交互语义。spec↔contract 漂移未出现，无需走 Track B spec-lane。
- **契约零触碰**：`docs/contracts/interface-contract.md` 零触碰（contract-lint 闸
  PASS，SU-E3/boundary 结构齿 `s4` 断言契约/spec canary 在世并 GREEN）。无
  shim API / 笔顺 D3 / display 统一响应块 E1 / 课表 MD B1 的任何变更。
- **无实现侧文档同步需求**：本包属 `firmware` 显示层结构与归属重构 + 设备装配
  接线，不含需要回灌 `docs/` 的 prose 面变更。

## 本包实际改动的实现文件与性质（不含 docs）

| 文件 | 性质 |
|---|---|
| `firmware/main/display/kidpal_layer.h`（新增） | 新纯逻辑模块头：前台 layer 栈管理单一权威，冻结契约类型/数值码逐字一致（C linkage，零芯片依赖）。 |
| `firmware/main/display/kidpal_layer.c`（新增） | 新纯逻辑模块实现：push/pop/top/has 优先级栈 + chat_target 恒=CHAT + chat_clear_event 驻留清除时机（OQ3=c）。 |
| `firmware/main/display/kidpal_render_node.h` / `.cc` | 重构：三散标志（s_chat_overlay/s_page_pinned/s_current_page）+ 1s tick 轮询对账退役，前台归属改 layer 栈事件驱动；voice_begin/voice_end/begin_reminder/begin_provision/end_provision 新增事件入口；s_kidpal_scr 成为 display/ 唯一顶层屏创建点与唯一 lv_screen_load 目标。 |
| `firmware/main/display/char_page.cc` | 查生字页并入 s_kidpal_scr 体系：删独立屏 `g_page.scr = lv_obj_create(NULL)`，改活动屏上一层面板 `g_page.panel`。 |
| `firmware/main/display/lcd_display.h` / `.cc` | 新增 `LcdDisplay::KidPalReparentChatToLayer(lv_obj_t*)`：把 SetupUI 建在默认首屏上的聊天 chrome（状态栏/表情/气泡/预览图/低电量弹窗）reparent 到常驻 top layer。 |
| `firmware/main/application.cc` | 状态跃迁接线：Connecting→voice_begin（新语音轮清驻留 + 推 VOICE）、Idle→voice_end（回落/回答驻留）；聊天清除时机经 `kidpal_layer_chat_clear_event(NEW_VOICE)`。 |
| `firmware/main/mcp_server.cc` | 提醒弹窗夹钳 `begin_page`→`begin_reminder`（REMINDER 进 layer 优先级表，review F8）；char 路由注释随 char 并屏更新。 |
| `firmware/main/boards/common/wifi_board.cc` | 配网 PROVISION 设备接线：配网进/退事件 `begin_provision`/`end_provision`（分层最高 PROVISION=6）。 |
| `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc` | 板装配层在 render_node_init 前调 `KidPalReparentChatToLayer(lv_layer_top())`（单物理屏时序）。 |
| `firmware/main/CMakeLists.txt` | 设备构建编入 `display/kidpal_layer.c`。 |
| `firmware/main/test/host_app/main/CMakeLists.txt` | host 接线：编入 `screen_unification.acceptance.c`（置 smoke 前）+ `display/kidpal_layer.c`（强符号覆盖 weak 哨兵）。 |
| `firmware/main/test/screen_unification.acceptance.c`（新增） | 本包冻结验收件（acceptance-author 产出，structure/behavior 7 齿 + boundary 守卫）。 |

**冻结面 git 零触碰确认**：`docs/contracts/**`、`docs/spec.md`、`managed_components/**`
均不在本包 diff 内（机检 `git diff --name-only` 零命中）。