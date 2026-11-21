---
id: TECH-261114-overlay-close-channel-rebuild-churn
title: overlay_close 即断 WS + 5s 后常驻重建属绕路——冷通道按键进聆听 1.57s（温通道 0.82s）、每次重建付全套 bootstrap burst；候选 A「close 不断通道」为 P1-⑤ 延迟优化，待现有开发任务完成后排期，建议与 TECH-261014 家族合并立包
domain: tech
status: closed
severity: SHOULD
owner: dev 批（固件面，/triage-change 路由）
trigger: "待 P1 真机轮 A 真机凭据结项——261116-channel-resident-core（T1，候选 A「close 不断通道」）已实现、离线绿；A1 温通道按键进聆听 ≤0.82s 对照 KPLOG029 + A2 连续 5 次「结束→再说话」零冷通道零重建 burst。依据 workstream PROPOSAL §6「debt 结项凭据必须真机、不是合入」，收口记账归主线程（debt-render.mjs 归位）。原：2026-11-14 human 排期「开发任务完成后再开始」；建议与 TECH-261014 家族残余项合并 triage、共担一轮真机回归"
opened: 2026-11-14
closed: "2026-11-19"
closedBy: "261116-channel-resident-core"
paths: ["firmware/main/application.cc", "firmware/main/application.h"]
source: 2026-11-14 延迟专项 P1-⑤ 调查——设备 KPLOG029 实测（当日 15:04 三连会话轮）+ 固件代码锚点 + 引擎/front 日志互证
handoff: 见 trigger 的家族合并建议；交互评审点见正文候选 A
---

**现状成本（2026-11-14 KPLOG029 uptime 毫秒轴实测）**：

- **温通道**（常驻 WS 活着时按键）：key_route_dispatch(session_toggle) → State listening = **0.82s**（41292→42112ms，含 AFE 启动，无重连）。
- **冷通道**（撞上 close 后的重建窗）：idle → connecting → WS Connecting → TLS 证书验证 0.53s → Session ID → listening = **1.57s**（53012→54582ms）。
- **绕路形态**：overlay_close（结束键）→ `CloseAudioChannel` + WiFi LOW_POWER → `kResidentRebuildDelayS`（5s）后 `TryStartAutoConnect` 重建常驻通道（application.h:177 注释原文「ResetDecoder/abort + CloseAudioChannel + Idle + LOW_POWER + 常驻重建」；退避 `kAutoConnectBackoffS[3]={5,10,20}` application.cc:105）。而常驻通道本来就是设计目标态（提醒推送、故事预取都跑在它上面，决策 21「引擎不主动断连」支持长连接）——**断了再建纯绕路**：孩子 5s 内再按键付 1.57s 冷启动，且每次重建付全套 bootstrap burst（story_list + points_home + home + schedule 四发 MCP + 4 个故事预取重跑，实测约 15s 后台流量，15:05:04-21 窗口）。

**候选方案（立包时裁）**：

- **A（治本）**：overlay_close 不断 WS——语音会话 UI 关闭、麦克风口径不变（AFE gating 与 WS 解耦），通道保持常驻。收益：冷启动竞态归零 + 重建 burst 归零 + 「闲置超时后首按键必走重连」场景（TECH-261012-first-keypress-no-listening 形态）一并受益。**评审点**：① 引擎 dialogue 历史跨语音会话累积 × OpenClaw 持久会话（user=MAC，261022-m5-k2b）的双历史叠加——OpenClaw 侧有自动 compaction（11-14 实测 stable session 340 events / 387 archives），引擎侧 dialogue 窗口行为需包内核清；② 功耗面：稳态本就是 PERFORMANCE 模式（重建后同款），断连只省 LOW_POWER 的 5s 窗口，预期净功耗不变，需真机确认；③ close 语义现由 261010 [S9] 本地关收尾补偿承载，改动需回归该批验收面。
- **B（保守）**：`kResidentRebuildDelayS` 5s→1~2s，缩小竞态窗，不消除 burst。
- **C（同族）**：修 autoconnect 3 次静默放弃（TECH-261014-autoconnect-gives-up-silently），提升断网自愈——独立条目已有，合并包时一并。

**影响面**：每次「结束→再说话」循环（孩子高频操作形态）多付 0.75s~1.57s 冷启动税 + 一次 bootstrap burst；竞态窗内按键的孩子体感为「按了没反应」（与 TECH-261014-wakeword-connecting-voice-layer 的用户感知同族，该条 UI 面已由 261014 包修，本条是通道面残根）。

**风险**：riskTier: medium——候选 A 触固件会话生命周期核心路径（application.cc 状态机 + 261010 收尾补偿语义），回归面覆盖各会话类型（问答 / 记录 / 查字 / 提醒推送）；B 为常数改动 riskTier low 但收益有限。立包按 /triage-change 判档，真机回归轮必配。

---

**收口注记（2026-11-16，integrator · 261116-channel-resident-core）**：候选 A 「close 不断通道」已在 P1 实现——`Application::CloseAudioChannel()` 历史名 helper、`HandleToggleChatEvent` Listening 分支、`WakeWordInvoke` Listening 分支、`HandleEndVoiceSessionEvent` 四路收口到统一会话结束处理器（发 `SendStopListening`、通道保持常驻），删除 `ScheduleResidentRebuild()` 与 `kResidentRebuildDelayS`；`SetPowerSaveLevel(LOW_POWER)` 一并移除。离线面已过：final r2 十闸全绿、固件真编译通过、server 全量 473 passed。**本条目未结项**：其成本证据（温 0.82s / 冷 1.57s）与收益均为真机现象，按 workstream PROPOSAL §6「debt 结项凭据必须真机、不是合入」待 P1 真机轮 A（A1 + A2）凭据，收口记账归主线程。status 维持 `deferred`（本次仅刷新 trigger 指向真机验证；债务账 status 闭集不含「verifying」形态，故不改闭集外的值）。

**结项（2026-11-19，consumed-by `261116-channel-resident-core`）**：候选 A 治本已落地并真机验证——四条会话收尾路径改发 `listen stop`、通道保持常驻、front 周期 `kidpal_keepalive` 保活 + 设备自愈巡检。真机凭据（轮 A/E，KPLOG + front 日志）：① 温通道「结束→再按」按键到聆听 **360ms / 590ms**（KPLOG029 基线 0.82s），全程无 `WS: Connecting`、无重建；② bootstrap 六件每连接一次、零 burst；③ 长时间闲置窗口 `Channel timeout` = 0。证据落 `delivery/taskpackages/261116-channel-resident-core/device-evidence/` 与 `device-round-evidence.md`。
