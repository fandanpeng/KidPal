---
changeId: 260911-render-pages
date: 2026-09-12
author: planner（intake 阶段 0 勘察，只读；S0.2 数据链 + S0.3 五态落点）
status: intake-complete
---

# intake 勘察记录 · S0.2 数据链（#11 课表页）+ S0.3 五态落点

## S0.2 · #11 课表页数据链勘察结论

**根因 = 固件侧缓存时机（本包内修，不扩 server 面，不触契约，逃生门未触发）。**

证据链（逐字）：

1. **云侧 producer 无罪**。ECS 引擎日志（docker logs xiaozhi-esp32-server，96h 窗口）实证
   真实课表询问回合的完整链路：
   - 260911 12:42:12「明天都有哪些课程？」→ 12:42:22 板端 ack
     `{"route":"schedule","items":1}`（次日周六，每周固定课表周六行恰 1 条武术班，
     语义正确）；
   - 260912 12:15:00「课表。」→ 12:15:11 板端 ack `{"route":"schedule","items":1}`；
   - 48h 窗口内 kidpal/display 仅出现在 MCP 工具清单广播（会话建立 tools list），
     实际调用集中在上述课表/翻页回合，payload 链路 = OpenClaw agent 按 system prompt v4
     §课表生成 → gateway mcp_relay display_tool_call → 板端 mcp_server.cc display_dispatch
     case 4（firmware/main/mcp_server.cc:544-602）→ kidpal_schedule_parse → 渲染 + 缓存。
2. **schedule.md 数据源有真实内容**。ECS /root/.openclaw/workspace/kidpal/schedule.md
   （1082B，2026-09-11 23:21 更新）：学校课程表（周一~周五 6 节）+ 每周固定课表
   （周一~周日 7 条培训）。producer 侧读数、筛选、载荷生成均正常。
3. **真根因（固件缓存时机）**：`kidpal_render_node_cycle_next`
   (firmware/main/display/kidpal_render_node.cc:438-528) 的 KP_PAGE_SCHEDULE 分支
   （:499-513）直接用 `s_sched_n` 渲染——该缓存仅在**本开机内**发生过一次课表语音查询时
   才被 `kidpal_render_node_cache_schedule`（:562-582）填充。孩子 BOOT 双按翻页切到课表页
   时，若本开机从未查询过课表，n=0 → 渲染出「标题 + 空列表」的课表页。260910 硬件验证轮
   用户报「没用课表具体内容」（PROGRESS-SUMMARY-260910 §四A.3）与此机制吻合：观察发生在
   BOOT 翻页路径而非语音查询路径。

**RP-A3 修法建议（S3.1 承接，builder 实现面）**：cycle_next 的 KP_PAGE_SCHEDULE 分支在
`s_sched_n == 0` 时改画**友好空态**（居中提示文案，如「还没有课表数据，问我『我的课表』
吧」，V3 字体/框族按换皮纪律），不再渲染空壳页；语音查询路径（mcp_server case 4）不动。
可选顺带（不强制，producer 语义优化）：过期课目过滤在 12:15 周六回合返回 items=1
（武术班 10:00-11:30 已过），producer 侧「过期条目处理」可作 server 面增量申报，不阻塞
本包——是否做由 acceptance-author/builder 按工作量裁，不裁契约。

**F6 解除确认**：无需扩 server 面修复，260911-ecs-deploy-convergence 依赖面无涉
（arch-review-expansion.md §C）。

## S0.3 · 五态演出落点清单（rebase 校正版，PKG-2 已合并 d80e319）

PKG-2 实际形态 = Display 虚接口未动、聊天 chrome 经 KidPalReparentChatToLayer 迁
lv_layer_top()（符号与行号核实见 arch-review-expansion.md §A.1，此处只列落点清单）：

| 态 | 内容驱动入口 | 落点 |
|----|--------------|------|
| 通用 | LcdDisplay::SetChatMessage | firmware/main/display/lcd_display.cc:964（活分支；:502 = WeChat 死分支孪生，双分支扫描陷阱 §B.3） |
| 通用 | LvglDisplay::SetStatus | firmware/main/display/lvgl_display/lvgl_display.cc:72 |
| 通用 | LcdDisplay::SetEmotion | firmware/main/display/lcd_display.cc:973 |
| 建立面 | LcdDisplay::SetupUI（chrome 六件） | firmware/main/display/lcd_display.cc:774（status_label_:882 / chat_message_label_:906 / emoji_image_ / bottom_bar_ / low_battery_popup_） |
| chrome 迁移 | LcdDisplay::KidPalReparentChatToLayer | firmware/main/display/lcd_display.cc:1198（chat_top[] 数组 :1213-1220 = chrome 成员边界，允许内容级同步） |
| 提醒态 | kidpal_reminder_page_render | firmware/main/display/reminder_page.c:1105（压暗 :1144 仍 LV_OPA_50，F4 成立；krm_pose_jump 资产路径 :1033 已在） |
| 语义源（锚点，不改） | application.cc 三符号调用面 | firmware/main/application.cc:75-859（五态语义源 = :713-751 设备态分支） |

五态 → 落点映射（换皮执行参考）：

- 聆听 pose-listen + 气泡「我在听！」← application.cc:733 LISTENING 分支（SetStatus/
  SetEmotion）→ lcd_display.cc:964/973 内容面；
- 思考 pose-think ← SPEAKING 前置/llm 思考期（SetEmotion 面同样消费）；
- 播报 pose-talk + 声波条 ← :477 SetChatMessage("assistant") + 声波条新控件
  （SetupUI 建立面加，chat_top[] 成员表同步）；
- 提醒 pose-jump + d50 压暗弹窗 ← reminder_page.c:1105/:1144（F4：LV_OPA_50 → d50 网点
  活控件平铺合成，schedule_page.c 先例机制）；
- 低电 pose-sleep + zZZ ← low_battery_popup_（SetupUI 面）。

**布防清单（builder 铁律补充，来源 arch-review-expansion.md §B.2 七不变式）**：五态/换皮
代码不得撞零残留五字符串（s_chat_overlay/s_page_pinned/s_current_page/s_chat_scr/
s_voice_ui_scr，含注释）、不得自建屏（lv_obj_create(NULL) 恰 1 处约束）、lv_screen_load
实参恒 s_kidpal_scr、CMakeLists 新增源文件不动既有条目、layer 底座零触碰（例外 =
chat_top[] 成员表内容级维护）。五态齿定位策略须避 lcd_display.cc #if 双分支陷阱；
v3_restore.acceptance.c 注册序在 host_infra_smoke 之前。

## 附：勘察方法说明

- 引擎日志取证走 kidpal-ecs-gateway skill 通路（ssh_helper.py），只读，未动任何 ECS 配置。
- 固件符号定位走本 worktree CodebaseMemory 图索引（C-AI-KidPal-pod-render-pages）+ 精读。
- 本记录为只读勘察产物，修法裁决权在包内流程（S3.1 builder 实现以本结论为输入）。
