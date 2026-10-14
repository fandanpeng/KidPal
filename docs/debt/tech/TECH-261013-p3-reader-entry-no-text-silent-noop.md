---
id: TECH-261013-p3-reader-entry-no-text-silent-noop
title: P3 KEY 双击进阅读态对「无正文缓存」故事静默无反应——设计降级缺用户可见反馈，且冷启动恢复的老故事无正文获取通路（S-F2.1 AC2 承诺不可兑现）
domain: tech
status: deferred
severity: SHOULD
owner: dev-loop（P3 显示面 + story_text 通路，疑跨面）
trigger: 下个动 P3 显示面 / story_text 通路 / front story 推送面的包；修复分两层（固件可见降级提示 = 纯固件；正文按需获取通路 = 疑触 front/契约，走 intake 裁），修完随包真机复验「双击进阅读 + 阅读态标题」
opened: 2026-10-13
closed: ""
closedBy: ""
paths: ["firmware/main/mcp_server.cc", "firmware/main/display/story_page.c", "firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc"]
source: 261012-p3-topbar-ink-bleach-fix TB-A4 真机轮（2026-10-13）human 观测「双击查看没有反应，其他都正常了」+ 同轮串口捕获 c:/tmp/diag-doubleclick.log 逐字证据
---

**证据**（2026-10-13 真机轮逐字，串口捕获 c:/tmp/diag-doubleclick.log）：

1. human 在 P3 列表态以正常/快两种节奏双击 KEY 键共 4 次——观测日志 `key_gesture_parser key=0 gesture=2` ×5（手势解析器全部正确识别双击，key=0 = KEY 键）。
2. 每次双击均正确走完页内链：板级 `key_button_.OnDoubleClick` → `keya_story_page_wants` ✓ → `HandleStoryKey(KEY_ID_KEY, KEY_GESTURE_DOUBLE_CLICK)` ✓ → `kidpal_story_page_input` 产出 `KSPV2_ACT_OPEN_READER(2)` ✓ → 消费侧 [mcp_server.cc:305-317](../../../firmware/main/mcp_server.cc) `StoryDispatchAct`——因 `g_story_text.id` 为空打 **`W (…) MCP: story_key open_reader_no_text -> stay list` ×4**（时间戳与双击逐一对应），**留在列表态、零用户可见反馈**。
3. 键帽条提示「说话·双击看」在屏（human 确认「对着」）——产品面在宣传一个实际静默无反应的手势。
4. 链路健康对照组：KEY 短按正常进录音会话（`story_key rec_session_slot page=0` + 键帽条切「说完啦/结束」）、BOOT 短按正常路由。

**根因（两层）**：

- **层 1（纯固件，UX）**：`OPEN_READER` 消费侧的「无正文缓存 -> 消费不动留在列表态」是 V27 不盲渲染纪律的有意降级（[mcp_server.cc:305-317](../../../firmware/main/mcp_server.cc) 注记明载），但降级**无任何用户可见反馈**（仅串口 WARNING）。同文件先例：story_text 解析失败的降级走 `display->SetChatMessage("system", "这个故事暂时打不开，稍后再试试")`（:1149-1151）——同一消费函数内两条降级路径一条可见一条静默，口径不一致。
- **层 2（疑跨面，通路缺失）**：`g_story_text` 全固件唯一写点 = `display_dispatch view=story_text` 云端推送路由（[mcp_server.cc:1154](../../../firmware/main/mcp_server.cc)）。正文到达时机 = 停录归档回传 + 云端「回看」场景主动推——**设备侧没有「进阅读态时按需请求正文」的机制**。冷启动恢复（v1.63）只重推列表不推正文 → 恢复的老故事在本开机**永远**无正文缓存 → 双击永远静默无反应。需求 S-F2.1 AC2「When KEY 双击，Then 进入阅读态」为无条件承诺，当前架构对恢复故事不可兑现。

**影响面**：P3 阅读态（S-F2.3 / WF-F2-04）对「非本开机录制的全部故事」不可达（静默无反应）；键帽条宣传语义失效；261012 包 TB-A4 第 3 项（阅读态标题自愈复验）因此被阻未能观测——bleach 修复 A 本身（去漂白源）不受影响，阅读态一旦可达标题复验随本缺陷修包顺带。

**建议改动**（候选，交 review 不自批）：

- ① 层 1 修（纯固件、小）：无正文降级改用户可见——对齐 :1149 先例（SetChatMessage 提示）或 P3 屏内就地提示（版式归设计稿），文案口径归 story_page.h 单源纪律。
- ② 层 2 修（疑跨面，先 intake）：正文按需获取——候选 a = 设备→网关新增 story_text 请求消息（**触契约 §四私有消息族，走契约门**）；候选 b = front 在冷启动恢复推送列表时附带首屏正文 / 双击事件上报后推对应正文（front 侧改动，不改契约，实时性待评）；候选 c = 云端语音查询路径预装（触发面窄、不解双击场景）。②的通路取舍需 human/architect 裁，①不依赖②可先行。
- ③ 真机复验项（随修包）：双击进阅读 + 阅读态标题（TECH-261012 修复 A 的自愈预测一并闭环）。

**风险**：riskTier: medium——层 1 修复明确（低风险快修）；层 2 通路未裁（触面待 intake 定，候选 a 触契约）。修通前 S-F2.3 阅读态对存量故事不可用。owner = dev-loop；登记后工作交出（handoff = trigger 所列）。
