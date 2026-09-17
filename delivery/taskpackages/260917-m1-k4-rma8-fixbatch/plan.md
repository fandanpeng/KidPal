---
rigor: full
riskTier: high
sourceType: regression
reviewed-by: architect (change-triage 快审, 2026-09-17)
date: 2026-09-17
---

# K4 RM-A8 真机确认轮缺陷修复批（F1 回执链断 + F2 弹窗不驻留）

> 来源：K4（260915-m1-k4-reminder-overlay-unify，已 merged 本地 main）RM-A8 真机确认轮第 1 轮不通过。
> 证据：`delivery/changes/dev/260915-m1-k4-reminder-overlay-unify/device-round-1.md`。
> 分流裁决：② 单包 Full / riskTier=high / 单 Pod。裁决依据见本文件「分流裁决」节。

## 背景与根因

### F1 · 渲染回执永不发出（根因已钉死）

- 现象：网关 `alert_deliver delivered=True` 但 `acked=0`，60s 重推 ×3 后静默作废，每次重推 TTS 重播（用户听到两遍）。
- 根因：`kidpal_reminder_mark_rendered`（firmware/main/display/reminder_page.c:886）生产代码零调用点。渲染路径 `McpServer::RaiseReminderQueue`（firmware/main/mcp_server.cc:319-376）画完弹窗直接 `EmitRenderReceipt`，但 `kidpal_reminder_may_emit_receipt`（reminder_page.c:875-884）要求 `shown.rendered` 恒 0 → 回执静默不发。
- 为何 host 齿没抓住：acceptance 齿自调 mark_rendered 桩级模拟渲染完成（reminder_overlay_unify.acceptance.c:811/859/944），状态机单测绿但集成缺口无齿覆盖。
- 契约定位：契约 §三/§四 回执语义（渲染即确认、一 id 一回执、60s 重推上限 3 次）**已冻结且语义正确**，本修复是纯实现接线，**不改契约**。

### F2 · 弹窗被语音轮 speaking 态视觉覆盖、不驻留（根因候选，builder 前置排查钉死）

- 现象：弹窗可见约 1 秒即消失；65.9s→120.4s 间无任何 page 级渲染日志；layer 栈方向正确（begin_reminder 推 REMINDER(5) 置前）；KEY dismiss 语义本身正确（RM-A6）。
- 候选根因（review 已聚焦，验收仍以不变量为准）：`LcdDisplay::SetStatus`（lcd_display.cc:1769-1784）speaking 时无条件 `kidpal_voice_apply_look(KVST_SCENE_TALK)`，而弹窗 = 同一副四态骨架换 REMIND 场景皮肤（kidpal_lcd_reminder_show, lcd_display.cc:2104-2135，无独立弹窗对象）——TTS 一起骨架即被刷回语音态。
- 违反：RM-A2（最高层覆盖不被压）、RM-A6（驻留到按键）；spec §2.2 层级口径 + S-F1.4 AC1。
- 修复面警告：守卫加在 SetStatus/apply_look/render node 前台判定处都会触及语音轮（K2/K3）共享视觉面，回归半径越过 K4 自身边界——这是本包 Full 的核心理由之一。

## 修复方案

- **F1（一行级接线）**：`RaiseReminderQueue` 在 `alert_drawn != 1` 早退检查之后、`EmitRenderReceipt` 之前，补 `kidpal_reminder_mark_rendered(state, shown.id)`（state 取 `kidpal_reminder_default()`，与同函数一致）。单一修复点同时覆盖 raise_next 返回 1（新弹）与 2（V18 滞留顶替）两条路径，并激活 mcp_server.cc:302 的重推补回执路径。建议在 373 行 alert_render 日志中带出 rendered/armed 状态便于真机核验。
- **F2（前置排查 → 钉根因 → 修复）**：builder 必须先完成排查步骤（见下）把根因钉到 file:line 并写入 fix notes，才允许动手修；修复形状（SetStatus 守卫 / apply_look 场景门控 / render node 前台判定）由钉死的根因决定，本 plan 不预绑机制。

## builder 前置排查步骤（F2 专属，必做、产物留痕）

1. 加临时观测日志：`kidpal_voice_apply_look` 入口带 scene + `kidpal_render_node_front_layer()` 当前 top；SetEmotion/chrome 定时器路径同理。
2. 真机或模拟器复现：弹窗驻留中触发一轮 speaking，串口抓「REMINDER top 期间发生了哪些语音场景视觉 apply」。
3. 产出根因陈述：覆盖路径 file:line + 一句机制解释，写入 fix notes（taskpackage notes / progress）；排查日志若需保留为常驻观测点（建议保留 apply_look 场景日志，量级无害），在 closeout 声明。
4. **排查结论若指向「须拆独立弹窗对象层 / 改四态骨架版式」→ 停，触发逃生门转 /arch-change**（架构级改法不在本包权限内）。

## 验收靶

### F1 验收（可机判，两层）

- **host 齿（RED 基线，本批必须新增）**：现有齿桩级自调 mark_rendered 是假绿根源。新增一条集成齿，形状自选但必须满足：**在未打 F1 修复的当前代码上为 RED**（例如：源级结构断言——mcp_server.cc 的 alert_drawn 检查与 EmitRenderReceipt 之间存在 mark_rendered 调用；或抽取/链接 RaiseReminderQueue 逻辑的 host harness，桩 reminder_show 返回 1、断言回执发出且**测试代码不自调 mark_rendered**）。closeout 声明该齿在修复前 RED、修复后 GREEN。
- **真机齿（RM-A8 重跑判据）**：网关 `alert_deliver` 后 `acked=1`、无 60s 重推、单次 TTS 播放（journalctl + 串口 + human 目击三方互证，沿用 device-round-1 取证形态）。

### F2 验收（不变量靶，机制无关——根因未钉死时验收只能写可观测不变量，不写修法）

- **不变量（真机，RM-A8 重跑判据）**：提醒弹窗驻留期间（弹出至 KEY/BOOT 关窗），任一语音轮 speaking 态视觉事件不得改变 REMINDER 前台视觉——弹窗三要素（pose-jump + 大字事项 + 时间行 + 「知道了」键帽）在 TTS 播报全程屏上可见不中断；串口无 REMINDER top 期间的语音场景视觉接管日志（依赖前置排查第 1 步的观测点——这使「不可覆盖」从 human 目击升级为日志可判）；KEY = 关闭回当前页 / BOOT = 关窗回原页不翻页（RM-A6 / S-F1.4 AC2）；同轮补测录音排队例外（RM-A3：录音中提醒排队、录音结束立刻弹）。
- **回归靶（语音轮不许被修坏）**：无提醒驻留时，语音轮 LISTEN/THINK/TALK 三态视觉与修复前一致（真机一轮语音会话目击 + 现有语音态相关齿全绿）。

### exitCriteria（RM-A8 确认轮闭环）

1. F1 host 齿（RED→GREEN）+ 真机 acked=1 无重推；
2. F2 不变量靶 + 语音轮回归靶全绿，根因陈述留痕；
3. RM-A8 四项清单重跑全绿（弹窗演出 / 最高层覆盖 / 录音排队例外 / 模拟器截图缺口——截图项若 planner 判定属 K4 收口遗留而非本批范围，拆出单独记录并在 closeout 声明去向，不得静默丢）；
4. 修复批 closeout（supervisor 合并门，riskTier=high 升级人类）。

## 铁律与边界

- 不改 `docs/contracts/**`（回执语义已冻结，实现贴合即可）；不改 spec；不改 agent 定义/gate。
- F0（speaking 显示锁假死）**不在本包**：与 K4 弹窗路径无涉，走 ARQ 或独立批，不得顺手修。
- 修复若钉死后发现需架构级改法（拆弹窗对象层/改骨架版式）→ 停，转 /arch-change。
- iterationBudget = 2（F2 排查可能耗一轮）；fixBudget = 2（根因未钉死项硬磕性价比低，触顶熔断升 human，对齐 escalation-playbook「同 fingerprint 连续 2 轮熔断」）。
- gateProfile 建议：["state-machine", "mcp-tool-contract", "test-quality"]。

## 分流裁决记录（architect 快审 2026-09-17）

② 单包 Full / riskTier=high（状态机轴 + F2 共享骨架回归半径 + 前批 host 齿假绿实证）；非 ③（单切片单子系统单 Pod）、非 Lite（单向守卫 + Lite 判据两条不满足 + test-quality 闸）；非 ①（改运行时行为）。逃生门自查：无条件触发；条件性触发器 = F2 排查结论指向架构级改法。F1 契约定位 = 实现缺口非契约缺口，修复零契约编辑。
