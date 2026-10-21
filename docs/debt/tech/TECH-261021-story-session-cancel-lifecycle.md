---
id: TECH-261021-story-session-cancel-lifecycle
title: P3 记录会话取消/收尾生命周期两侧不对称——丢弃不通知 server（在途引导回合照播 ③ / 丢弃内容仍被自动总结收录 ⑤）+ server 收尾不清设备 ctx（列表页长按困死 ④），三症状同根
domain: tech
status: deferred
severity: MAJOR
owner: dev-loop
trigger: human 单开对话处理（2026-10-21 裁定：本包收口、三症状记 debt、修位同一处；修复走 /triage-change 分流，建议单包）
opened: 2026-10-21
closed: ""
closedBy: ""
paths: ["server/front_app.py", "server/story.py", "firmware/main/display/kidpal_render_node.cc", "firmware/main/display/view_router.c", "firmware/main/mcp_server.cc", "server/deploy/engine-patch/connection.py"]
source: 2026-10-21 真机三轮实测（TF 卡 KPLOG003/004 时间线逐点取证，含 261020-p3-rec-frame-visual 真机目检轮附带发现），human 三次报告 + 日志实锤
handoff: dev-loop（/triage-change 过比例化 review 后立修复包；human 已示意单开对话处理，本条目 = 该对话的技术输入）
---

**现象（human 2026-10-21 真机三轮报告，日志逐点实锤）**：

1. **③ 在途引导回合泄漏**：录音中按「丢弃」结束会话回列表页，片刻后设备对着空气播报引导语。两次实证：旧固件 07:41 轮（discard_rec @ uptime 44062s → 「那后来呢」@ 44117s，55s 后）；新固件 07:55 轮（discard @ 710s → 「那后来呢？」@ 749s，39s 后）与 08:19 轮（discard @ 104s → 「那后来找到什么了？」@ 127.6s，23s 后）。根因 = 丢弃只关设备侧会话，server 侧已派出的引导 LLM 回合照常完成并经 kidpal_speak 注入播报。
2. **④ 列表页长按困死**：一轮 142 秒会话走 **server 正常收尾**（整理页 → 正文页 → 列表页）后，列表页长按 KEY 反复派发 `action=2` 但被拦：`W kidpal_rn: show_home suppressed: record session live (page=2)`（08:09–08:16 六连拦，KPLOG003 tail）。根因 = [kidpal_render_node.cc:1250](../../../firmware/main/display/kidpal_render_node.cc) 守卫 `kidpal_story_session_ctx_active()==1 && (page==RECORDING||page==ORGANIZING)`，而 server 驱动收尾路径**不清** `kidpal_story_session_ctx` 与 `s_story_session_page`（停死在 2=RECORDING）；对照**丢弃路径会清**（08:19 丢弃后 109s 长按回首页成功）——两路径不对称。设计意图（:567-569 注释）本是「收尾后守卫自然放行」，server 收尾路径漏清。
3. **⑤ 丢弃内容仍被自动总结收录**：录音数句 → 丢弃回首页，约 100 秒后 server 自动总结并归档该篇。日志实锤（新固件 08:19 轮）：`discard_rec` @ 104s（设备侧已丢）→ `control start_organizing story_consumed=1 page=6` @ 237.4s（server 卡壳 sweep 把它当「未关会话」按连续 2 轮无回应自动收尾）→ `story_text_dispatch` @ 242.5s、story_list items 6→**7**——用户明确丢弃的内容被收录成新记录。**后果升级：这不是体验瑕疵，是用户想销毁的素材被保留归档**（含隐私语义）。

**同根判读（三症状一个修位）**：P3 记录会话的取消/收尾生命周期在设备↔server 两侧不闭环——

- 设备→server 方向：`discard_rec` 只关设备侧，**不通知 server**（server 的 StoryAccumulator / 卡壳计时器 sweep / 在途引导回合全然不知）；⑤ 是它最重的后果（自动收尾归档），③ 是它的轻症（在途回合照播）。
- server→设备方向：server 驱动的收尾链（`start_organizing` → `story_text` 回列表）**不清设备侧会话 ctx**（④ 困死）；设备只有丢弃路径做了清理。

**代码锚点（261019 批 landing 面）**：

- server 卡壳 sweep / 引导回合 / 自动收尾：261019-p3-record-dialogue-guide 包 S2（front_app `_story_timeout_sweep` 重写 + 引导 LLM 回合 + 三式收尾统一下发 `start_organizing`）；忠实整理与归档 = archive_story 既有链。
- 设备守卫：[kidpal_render_node.cc:570](../../../firmware/main/display/kidpal_render_node.cc) show_home 守卫、:1250 `rn_record_session_live_locked`；ctx 载体 = `kidpal_story_session_ctx`（mcp_server REC_SESSION 消费位）。
- 丢弃路径：story_page KEY 映射 DISCARD_RECORD → `story_key discard_rec`（cancel_count 累计）→ `local_close_settle src=discard_record`。

**修法方向（归 triage 裁，含方向分叉）**：① 设备→server 取消通知——丢弃/打断时下发会话取消（wire 语义是否新增 signal = 分叉：复用既有 stop/abort 通道 vs 新增显式 cancel；后者触契约面须走 /arch-change）；② server→设备收尾清理——story_text 到达/整理页退出路径补 `kidpal_story_session_ctx` 清理与 `s_story_session_page` 复位；③ 在途引导回合取消——server sweep 感知取消后撤销 pending 引导/整理回合（LLM 已在途的丢弃其产出）。⑤ 优先级最高（素材销毁语义），②次之（困死），③随①顺带。三个症状同一包修，单开对话经 /triage-change 分流。

**复验**：修复后真机四轮——① 录音中丢弃：不播引导、不归档、列表条目数不增；② 正常说完收尾回列表：长按回首页即时可用；③ 引导播报中丢弃：引导语立即断、不播完；④ 回归：正常收尾归档链、KEY 收尾路径、录音期提醒排队不回归（S-F1.4 AC6）。

**风险**：riskTier: medium——①若走新增 wire 信号触契约面（/arch-change 升档）；②设备 ctx 清理点与既有收尾链竞态（story_text 渲染与 ctx 清理顺序）；③ sweep 撤销在途 LLM 回合要防误杀正常收尾。Track B 三要件齐（执行测出 + 修法有真分叉 + 爆炸半径已实测揭示）。

---

**登记留痕（2026-10-21）**：三症状由 261020-p3-rec-frame-visual 包真机目检轮附带发现（该包坐标改动与本三症状正交，白盒复核已证）；human 裁定「本包收口、三症状记 debt、修位同一处、单开对话处理」。本条目 = 该对话技术输入。另两项 human 早前裁定「再观察」的观察项（首次按键吞没无日志 / 首页语音入口进录音页 LLM 链路延迟 10-16s）未入本条目，见该包 pod-retro 观察项节。
