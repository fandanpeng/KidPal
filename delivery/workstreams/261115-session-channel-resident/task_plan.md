---
workstream: 261115-session-channel-resident
slug: 261115-session-channel-resident
status: active
opened: 2026-11-16
closed:
summary: 语音会话生命周期与 WS 通道生命周期解耦——会话结束改发 listen stop 且通道保持常驻；front 心跳保活 + 设备自愈巡检；60s 无语音超时接线；云端结束意图三段补链；自动重连封顶长跑与人工意向位
owner: main-thread
area: systems
changes:
  - 261116-channel-resident-core
  - 261117-session-end-intent-longretry
---

# task_plan · 261115-session-channel-resident（workstream 级分波交付计划）

> 本件 = workstream 拆包顺序链必备节。设计权威 = [PROPOSAL.md](PROPOSAL.md)（FROZEN v1.0）；拆包评审 = [review.md](review.md)（门 1 首轮 BLOCK，修正后复检）；架构+契约权威 = `delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md`（FROZEN v1.1，R2 GREEN，landed `87dbf775` / merged `7e4c44d8`）。里程碑登记 = `delivery/milestones/milestones.jsonl` `M8-session-channel-resident`（2026-11-16 human 裁「补登」，state=planning / health=green）。

```yaml
workstream: 261115-session-channel-resident
phases:
  - id: P1
    name: 通道常驻核心（会话收尾收口 + 保活心跳 + 自愈巡检 + 60s 超时）
    scope: 四条本地关收尾路径收口到统一处理器并发 listen stop（不再 CloseAudioChannel / 不再 LOW_POWER / 不再 ScheduleResidentRebuild）；60s 无语音超时 producer（主循环 tick + clock_ticks + VAD 沿重置，记录语境豁免）；OnIncomingJson 新增 kidpal_keepalive no-op 分支；主循环 tick 自愈巡检；front 心跳 producer（默认开、=0 回滚）+ listen stop 每会话收尾三件 + ctx 边沿完整化兜底；既有冻结齿重写显式申报
    changeManifest: firmware/main/application.cc、firmware/main/application.h、firmware/main/test/session_channel_resident.acceptance.c（新增）、firmware/main/test/boot_auto_connect.acceptance.c、firmware/main/test/voice_round_ui.acceptance.c、firmware/main/test/record_session_live.acceptance.c、firmware/main/test/host_app/main/CMakeLists.txt、server/gateway/front/front_app.py、server/acceptance/test_session_channel_resident.acceptance.py（新增）
    riskTier: high
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality, state-machine, mcp-tool-contract]
  - id: P2
    name: 云端结束意图三段补链 + 自动重连封顶长跑与人工意向位
    scope: mcp_server control case 0 → EndVoiceSession；display_schema self_push 豁免口扩 end_session（relay 恒拒不变）；front 截获-自推单源；prompt 告别回合纪律（AGENTS.md + carrier 三面对账）；退避表 {5,10,20,40,60} + 恒 60s；四个人工入口人工意向位 + 网络断退出请求位
    changeManifest: firmware/main/mcp_server.cc、firmware/main/application.cc、firmware/main/application.h、firmware/main/test/session_end_intent_longretry.acceptance.c（新增）、firmware/main/test/boot_auto_connect.acceptance.c、server/gateway/display_schema.py、server/gateway/front/front_app.py、server/acceptance/test_session_end_intent.acceptance.py（新增）、server/acceptance/test_record_dialogue_guide.acceptance.py、server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md、server/deploy/OPENCLAW_SYSTEM_PROMPT_CARRIER
    riskTier: medium
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality, state-machine, mcp-tool-contract]
```

## 分波与依赖

- **serial WIP=1**：P1 → 真机轮 A → P2 → 真机轮 B。P2 `dependsOn: [261116-channel-resident-core]`，从 P1 合并后的 main 起分支。
- 两包在 `application.cc` 同一批函数内改不相交行段（`HandleEndVoiceSessionEvent` / `HandleToggleChatEvent`：P1 = 收尾动作序，P2 = in-flight 守卫行）；P2 重取行号锚时须覆盖这两处。
- 架构+契约面已先于实现面 landed（`docs/architecture.md §12` 行在场 = P1 launchPreconditions 判据）。

## 收口标准

两轮真机全绿 + 四条 debt 以真机凭据结项（`TECH-261114` = A1+A2；`TECH-261012` = A3；`TECH-261014-session-end` = A6+B1；`TECH-261014-autoconnect` = B4+B5）。`docs/requirements.md:48` ⚠️ 对账注记由主线程在收口时路由。