---
changeId: 261115-session-channel-resident
status: in-flight
sources:
  humanIntentVerbatim:
    - "开始处理这个debt，我倾向于治本，但需要你架构师讨论清楚方案，确定好方案拆包不要拆的太碎，拆好好后 你就headless 启动dev loop 开始开发，有什么问题现在就问我，我等你十分钟"（2026-11-15，指 TECH-261114-overlay-close-channel-rebuild-churn）
    - "全家桶"（2026-11-15 三问一：范围 = 通道常驻主体 + autoconnect 长重试 + 会话结束三通道）
    - "共享短上下文（推荐）"（2026-11-15 三问二：同一常驻通道内多轮语音会话共享引擎对话上下文）
    - "允许 agent 部署 ECS + 你持板真机轮（推荐）"（2026-11-15 三问三；未选「允许改引擎补丁」= 设计须绕开引擎侧代码改动）
  refs:
    - docs/debt/tech/TECH-261114-overlay-close-channel-rebuild-churn.md
    - docs/debt/tech/TECH-261014-autoconnect-gives-up-silently.md
    - docs/debt/tech/TECH-261014-session-end-channels-unwired.md
    - docs/debt/tech/TECH-261012-first-keypress-no-listening.md
    - docs/contracts/interface-contract.md §四 / §1.4
    - docs/requirements.md §2.1（结束三通道）
faceMap:
  requirements: |
    待 architect 复核。主线程初判「不动」：60s 无语音超时与云端结束意图已由
    requirements §2.1 成文（实现侧缺口，非需求缺口）；「会话结束 ≠ 断连」是承载方式，
    不改需求语义。若 architect 认定 §2.1「关闭」措辞需要按新承载对账（档 1 语义修改），
    在返回结论中列为 Open Question，不自行改需求面。
  architecture: |
    动。会话生命周期承载（常驻通道 + 会话边界信号 + 保活）、front 会话级收尾面、
    端云职责与场景④相关叙述。落地 = /arch-change 主路径 Track B（方向已由 human 拍定，
    无 Track A 判据）。
  contract: |
    预期动（待 architect 定改法集后由 architect-reviewer 四分类裁定）。候选面：§四端云
    协议剖面（会话结束上行帧 = 复用既有帧 vs 新增私有帧；通道保活帧形态）、§1.4 控制
    类指令 `end_session` 语义（回首页常显 vs 结束会话但保留通道）、§1.5 front 机械职责
    （会话级收尾与保活心跳是否属机械职责）。§1.4 `end_session` 已存在，未必新增。
  implementation: |
    动。firmware：application.cc（三条本地关路径 + HandleEndVoiceSessionEvent +
    ScheduleResidentRebuild/TryStartAutoConnect + 60s timer producer）、
    protocols/protocol.cc（120s IsTimeout 活性口径）、protocols/websocket_protocol.cc
    （保活/活性上报）、application.h；server：gateway/front/front_app.py（会话级收尾
    触发面、私有帧消费、保活心跳如需）、server/deploy（ECS 部署与 config 层）。
  governance: 不动（主线程初判；若实施中触 .claude/** 则回 intake 加面呈 human）。
facePlan: |
  1. 架构+契约面先行：/arch-change 主路径 Track B——architect 起草 PROPOSAL（含契约包
     三节，若触契约）→ architect-reviewer 四查 + 轻/主判档 → human 冻结 → 主线程
     packet-bound 落 contract/architecture。
  2. 实现面：③拆包（architect 同批给拆包建议，human 确认档位后主线程落 workstream
     PROPOSAL，再 headless 拉 planner 走 /build-feature；WIP=1 串行）。
  3. 交付：agent 部署 ECS（kidpal-front 重启，必要时引擎 config 层调整）；真机回归轮
     human 持板；远程发布边界不变（release-pr.mjs --publish 由主线程跑，PR 合并与
     其它 push 仍 human）。
  launch 硬前置：架构+契约面 anchors 未回填不得 launch 实现面包（change-intake.md §8）。
anchors:
  req: "不适用（主线程初判需求面不动；若 architect 复核判定须对账则回填）"
  arch: "architecture.md §12 changeId 行 `261115-arch-review-session-channel-resident`（2026-11-16 landed，reviewedCommit 87dbf775；门 1 F-01 回修后 **v1.2 增量落地 reviewedCommit fa51381e / merged main 50eb6b56**）——§6.4 整节重写（L1/L2/L3 三层解耦 + 心跳保活 + 自愈巡检 + 封顶长跑）/ §5.1.1 新增语音会话生命周期收口行 + 会话状态机 ⚠️ 对账 + 设备状态机边界句 / §5.1.2 front 生命周期锚 + 每会话收尾 / §5.2 场景①②④ / §8.1 风险 3 改写 + 风险 14 注记 + 新增 15/16/17 / §3.3 沿革注记 / §10 决策 29 取代注记；ADR-019 同批落（docs/decisions/ADR.md）；制品 = delivery/review/261115-arch-review-session-channel-resident/（FROZEN v1.1 + R1 BLOCK(F1,F2,F3) → R2 GREEN + landing-summary）"
  contract: "interface-contract.md §四新增 `listen` 帧节 + `kidpal_keepalive` 保活帧 + discard 时序改以 `listen stop` 为锚 + `kidpal_session_ctx` 边沿完整化；§1.4 `end_session` 截获-自推 + 表后段拆「截获执行类 / 截获转发类」；§1.5 第 3 条分流规则；§1.6 告别回合纪律；两条变更记录（2026-11-16）。四分类 = 主 ACCOMPANIES_SPEC_CHANGE + 次 SPEC_CONTRACT_DRIFT（architect-reviewer R1/R2 裁定）；同批原子落（reviewedCommit 87dbf775）"
---

# 261115-session-channel-resident · spine

## 一句话

把「语音会话生命周期」与「wss 常驻通道生命周期」解耦：结束语音会话不再拆通道，
通道按设计目标态常驻（暖通道 0.82s 直达聆听、零重建 burst），并同批补齐会话结束
三通道与断线自愈（全家桶，human 2026-11-15 裁）。

## 主线程探测到的现状事实（供各面消费）

- 冷热差：温通道按键进聆听 0.82s；冷通道（close 后重建窗）1.57s，另付约 15s 后台
  bootstrap burst（story_list + points_home + home + schedule 四发 MCP + 4 篇故事预取）。
  KPLOG029 实测，见 TECH-261114 条目。
- 三条本地关路径统一形态 = `CloseAudioChannel` + Idle + LOW_POWER + 5s 后常驻重建：
  `firmware/main/application.cc:972-990`（丢弃面）、`:1031-1041`（toggle_chat）、
  `:1141-1170`（overlay_close，261024 批）。
- 设备侧另有一道隐性拆通道：`Protocol::IsTimeout()` 硬编码 120s 无下行数据即判通道死
  （`firmware/main/protocols/protocol.cc:149-158`）——闲置超时后首按键必走重连，
  即 TECH-261012 的根因之一。
- front 的 `session_id` 是每连接不可复用键；连接期一次性推送（story_list restore /
  points_home / schedule 摘要 / schedule 推）与断开期一次性收尾（alert legs /
  session legs / story accumulator cancel）都挂在「连接」而非「会话」上。
- 契约 §1.4 已有 `end_session` 控制动作（「孩子说拜拜/不用了 → 结束会话，回首页常显」），
  设备侧 `mcp_server.cc:1725 → view_router.h:46 → Application::EndVoiceSession` 已有消费链；
  「云端结束意图未接线」的结论需按当前代码复核（debt 写于 2026-10-14）。