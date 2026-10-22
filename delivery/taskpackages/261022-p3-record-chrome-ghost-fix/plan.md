---
id: 261022-p3-record-chrome-ghost-fix
title: P3 记录会话 chrome 鬼影修复（键帽鬼影闪烁 + 成功路径鬼覆盖层吃按键）
rigor: lite
riskTier: medium
sourceType: regression
reviewed-by: architect
date: 2026-10-22
status: pending
---

# plan · P3 记录会话 chrome 鬼影修复（261022-p3-record-chrome-ghost-fix）

> 来源：/triage-change 2026-10-22，architect 比例化 review 判 **②单包 Lite**（riskTier=medium，sourceType=regression），human 触点①已确认（两包并联、均 Lite）。四面探测触面 = 1（纯实现面），不建 spine；逃生门预查 = 干净（零契约/spec/agent/gate 触点，缺陷②带帧类型负锚）。
> **lite 判注**：「host 验收断言可闭环」——键帽裁决纯逻辑单源在 keybar.c（host_app target 覆盖，keybar_three_slot / story_page_v2 验收件在世），鬼影闪烁的最终观感确认靠 human 真机目检（观感本体的验收形态先例已立），机制面断言 host 闭环。

## human 意图（触发链）

KPLOG005 真机轮 human 观感原话：「日志里边记录的状态乱跳，跟我在屏幕上看到的状态乱跳是一样的」。逐秒还原与三源取证已落 debt 条目 `docs/debt/tech/TECH-261022-p3-record-session-state-churn.md`（缺陷①② = 本包范围；③④⑤ = 并联包 B）。

## 范围（debt 缺陷① + ②）

### 缺陷① 键帽鬼影闪烁（全笔 12 次，含无语音会话场景）

**根因（KPLOG005 实证钉死）**：键帽裁决 `kidpal_keybar_sync_locked`（lcd_display.cc:1353-1457）读渲染节点缓存的故事子态——层切换/语音事件触发的 chrome 重同步跑在故事子态缓存更新之前，读到 **-1**，裁决掉进通用分支 `kidpal_keybar_slots(page, g_kidpal_keybar_scene, overlay_top)`；而 `g_kidpal_keybar_scene` 在语音会话结束后**永不复位**，通用分支恒画出残留场景键帽（聆听残留=「说完啦/结束」、思考残留=「不等了/结束」）约 300~500ms，页级键帽才盖回；同一事件常触发两次 chrome 同步 = 双连闪。三因叠加：场景键帽残留 + 过渡窗读 -1 + 单事件双同步。

**修法方向**：① 语音会话收尾沿复位 keybar scene；② 故事会话期（录音/整理/阅读子态）键帽裁决对 -1 过渡窗取最后已知子态或硬门通用分支；③ 双同步若经 1g 预评评估为良性可留，只修前两因亦可接受（取舍归 1g 预评）。

### 缺陷② 成功路径鬼覆盖层白吃一次按键

**根因**：故事归档成功、列表已刷新后，覆盖层在层栈里逻辑上仍占前台——归档后首按 BOOT 被路由成 `action=4(overlay_close)` 而非列表页动作，`dismiss_front_overlay` 后才恢复（13:03:20 实证；上午 09:31 笔同形态，非新版回归）。困死修复 4068dc6c 只给**失败**路径补了页面退出帧，**成功**路径无对应 dismiss。

**修法方向**：成功归档路径补显式覆盖层 dismiss，对齐 4068dc6c 失败路径形态。**硬约束（帧类型负锚）**：修复不得新发明下行帧类型（新帧 = 触契约 §1.3/§1.4，须转 /arch-change）——复用既有帧与层栈 dismiss 通道。修复落点（front 补帧 vs 固件层栈收口 SERVER_PAGE 残留）由 1g 技术预评定准。

## changeManifest 种子（仓根相对）

- `firmware/main/display/lcd_display.cc`（kidpal_keybar_sync_locked 过渡窗 -1 门控 + scene 复位消费点）
- `firmware/main/display/keybar.c` / `keybar.h`（纯逻辑单源，如需新增过渡窗取值 helper）
- `firmware/main/display/kidpal_voice_states.c`（语音场景生命周期/复位点）
- `firmware/main/display/kidpal_render_node.cc` / `.h`（成功路径覆盖层 dismiss/层栈收口；story_sub -1 来源）
- `firmware/main/mcp_server.cc`（会话收尾桥/盖章路径 dismiss 挂点，如需）
- `firmware/main/application.cc`（仅在 1g 预评确认 scene 复位挂点在会话收尾沿时纳入）
- `firmware/main/test/keybar_three_slot.acceptance.c`、`firmware/main/test/story_page_v2.acceptance.c`（回归锚 + 新验收断言）

## 验收方向

1. **新验收断言**：① 语音会话收尾后 `g_kidpal_keybar_scene` 复位（回非语音场景/默认）；② story 会话期 story_sub=-1 过渡窗不掉通用分支（取最后已知子态或硬门）；③ 成功归档后层栈无残留覆盖层（BOOT 在列表页路由成列表动作而非 overlay_close）。
2. **回归锚**：keybar_three_slot / story_page_v2 / key_layer_dispatch 三族全绿；host 套件零新增红（基线锚 5412af06）。
3. **human 真机目检**（live-manual 诚实申报）：翻阅读卡与记录会话全程键帽无鬼影双闪；归档后首按 BOOT 即生效。

## 不做清单（out of scope）

- 不改 story_page_input 冻结决策表——1g 预评发现必须改 → **停、回 triage 升档重判**，不原地扩；
- 不新发明下行帧类型（帧类型负锚，见上）；
- 不动 RDG-F2 键位分叉 / RDG-F3 整理态吞键；
- 不动 front（server/**）任何文件（并联包 B 的面）；
- 不做「单事件双同步」以外的 chrome 同步重构。

## 风险

1. 缺陷②修复落点未完全钉死（front 补帧 vs 固件层栈收口）——留 1g 技术预评，附帧类型负锚；
2. 1g 预评若发现须触 story_page_input 冻结决策表 → 升档信号（回 triage 重判）；
3. 单向守卫声明：riskTier=medium + rigor=lite，无 security/迁移/触契约信号，不触发 riskTier-overrides-rigor。
