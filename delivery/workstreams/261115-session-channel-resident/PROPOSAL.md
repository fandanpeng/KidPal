---
workstream: 261115-session-channel-resident
workType: [remediation, refactor]
status: frozen
opened: 2026-11-16
upstreamRefs:
  - delivery/intake/261115-session-channel-resident.md
  - delivery/review/261115-arch-review-session-channel-resident/PROPOSAL.md
  - docs/debt/tech/TECH-261114-overlay-close-channel-rebuild-churn.md
  - docs/debt/tech/TECH-261014-autoconnect-gives-up-silently.md
  - docs/debt/tech/TECH-261014-session-end-channels-unwired.md
  - docs/debt/tech/TECH-261012-first-keypress-no-listening.md
---

# 261115-session-channel-resident · 语音会话与 WS 通道生命周期解耦

> **状态：FROZEN v1.0 · 2026-11-16**（FROZEN 凭据 = human 2026-11-15 立项原话「倾向于治本……拆包不要拆的太碎，拆好后你就 headless 启动 dev loop」+ 2026-11-16 三答（范围全家桶 / 共享短上下文 / 允许 agent 部署 ECS + human 持板真机轮）+ 2026-11-16 冻结屏四答（OQ1/OQ2/OQ3 全裁 (a) + 「冻结 + 按 2 包开工」）；架构+契约面 FROZEN PROPOSAL v1.1 已 R2 GREEN 并 packet-bound 落地。作者≠判官：设计审链归 architect-reviewer R1/R2。）
> **权威序**：spec（`docs/requirements.md` + `docs/architecture.md`）= 行为语义权威；本件 = 路径选择记录。

## 1. 要达成什么（业务目的清单）

| # | 业务目的 | 可观察判据 | workType | 来源 |
|---|---|---|---|---|
| 1 | 「结束 → 再说话」不再付冷启动税 | 连续 5 次「结束再按键」，每次进聆听 ≤1s（对照 KPLOG029 温通道 0.82s），front 日志显示 bootstrap 六件每连接恰一次、无重建 burst | remediation | debt `TECH-261114`（KPLOG029 实测：温 0.82s / 冷 1.57s + 约 15s burst） |
| 2 | 闲置后首按键仍走温通道、不丢首按 | 闲置 >5 分钟（跨越旧 120s/137s 判死点）后首按键直接进聆听，无重连、无「按了没反应」 | remediation | debt `TECH-261012`；req §2.1 |
| 3 | 孩子说「拜拜」真正结束会话 | 说「拜拜/不用了」→ 设备结束本次语音会话、麦克风关、前台按 OQ3=c 回落；WS 通道保持常驻 | remediation | debt `TECH-261014-session-end-channels-unwired`；req §2.1 ② |
| 4 | 60 秒无语音自动关闭首次真机发生，且不误收长讲 | 真机观测到 60s 静默后自动结束会话；孩子连续讲 >60s 不被收掉 | remediation | debt `TECH-261014-session-end-channels-unwired`；req §2.1 ① |
| 5 | 断网/服务端重启后设备自愈，离线期人工动作不丢 | 拔网线 / 重启 front 超过 15 秒后设备自动重连；离线期按键与唤醒在恢复后补投、不丢 | remediation | debt `TECH-261014-autoconnect-gives-up-silently`（S-F1.4 提醒必达） |
| 6 | 会话间隙提醒仍必达且出音 | 通道常驻且无会话时，提醒弹窗照常渲染、语音照常出声（S-F1.4 + ADR-003） | remediation | `TECH-261114` 评审点；ADR-003 |

**起草侧自检两查**：清单引用无命中 requirements「已砍清单」；引用 story 的可行性三态无 🔴（本 workstream 全部为既有语义的实现缺口，无新增需求语义）。

## 2. 现在在哪（问题与根因）

本 workstream 是**整改型**。

**问题行为**：设备每次结束语音会话都把 WS 通道拆掉，5 秒后在后台重建；重建窗内的按键付 1.57s 冷启动并触发约 15 秒的 bootstrap burst（KPLOG029 实测，2026-11-14 15:04 三连会话轮）。同时设备侧存在一道隐性拆通道（120 秒无下行即判死），闲置后首按键必走重连；自动重连只跑 3 次即静默退出；requirements §2.1 的三条结束通道只有「再按 KEY」真正接通。

**根因**（详见 FROZEN PROPOSAL T1–T5 的证据段）：

1. **会话与通道揉在一个概念里**——「结束会话」的物理动作就是「拆通道」，于是每次结束都要重建并重付 bootstrap。
2. **120 秒判据的语义错位**——它本是「按会话建连」时代的死链探测，在常驻模型下探测对象已不存在（会话间隙本来就没有下行）。
3. **3 次退避是 boot 期预算**，被复用成了唯一的长时重连路径。
4. **云端结束意图三重断链**——prompt 无告别纪律、front 门禁双路径恒拒、设备 `case 0` 只停播不结束会话。
5. **60 秒超时零 producer**——纯模型 `ses_machine_dispatch` 生产调用点为 0（本批复核为真）。

## 3. 怎么走（方案空间 + 裁定）

### 3.1 方案空间与裁定

| 裁定 | 内容 | spec 关系 | 说明 |
|---|---|---|---|
| C1 | 通道生命周期与语音会话生命周期解耦：通道终止源收成闭集三条（服务端/网络侧断、引擎 24h 兜底、重启/OTA），会话结束不再拆通道 | `within-spec` | arch §6.4（本批重写）/ ADR-019；决策 29「永不休眠」的兑现 |
| C2 | 会话边界复用既有 `listen start` / `listen stop` + `kidpal_session_ctx`，不新增上行帧 | `within-spec` | 契约 §四 `listen` 帧节（本批补入）；arch §5.1.1 收口行 |
| C3 | 保活 = front 周期下行 `kidpal_keepalive`（30s；代码默认开、`KIDPAL_CHANNEL_HEARTBEAT=0` 为回滚开关）+ 设备主循环 1s tick 自愈巡检 | `within-spec` | 契约 §四 `kidpal_keepalive`；arch §6.4。设备 120s 判据阈值与逻辑零改动，语义升级为真端到端活性判据 |
| C4 | 云端结束意图：模型产薄意图 → front 截获 → self_push 机械自推单源 → 设备 `case 0` 收口统一处理器；relay 直转恒拒 | `within-spec` | 契约 §1.4 / §1.5 / §1.6（本批改写）；req §2.1 ② 实现补齐 |
| C5 | 60 秒无语音超时 producer 落点 = 主循环 1s tick + 既有 `clock_ticks_` + VAD 说话沿重置；判据含记录语境豁免 | `within-spec` | req §2.1 ①（阈值与录音态口径照现行文本）；arch §5.1.1 |
| C6 | 自动重连改封顶递增长跑（`{5,10,20,40,60}` 后恒 60s）+ 人工意向位 | `within-spec` | arch §6.4；S-F1.4 提醒必达 |
| C7 | 引擎 24h 兜底断连接受为唯一剩余重建窗口；引擎代码与 config 零改动 | `within-spec` | 决策 21 保留；ADR-019 |
| C8 | 共享短上下文不变：`user=<device_id>` + OpenClaw 持久会话（ADR-015），通道常驻对其零影响 | `within-spec` | ADR-015；arch §8.1 风险 14 注记 |
| C9 | `end_session` 页面落点 = OQ3=c（有回答页驻留则保持其前台） | `within-spec` | arch OQ3=c + 契约 §1.4 表行（本批对账） |

### 3.2 human 取舍点（OQ）与裁定

- **OQ1 保活载体**：选项 (a) front 下行心跳帧 / (b) 设备 WebSocket Ping / (c) 只调大 120s 阈值。**human 2026-11-16 裁 (a)**——(b)(c) 均有实证否决理由（见 PROPOSAL §4 OQ1 与 §7 评审意见）。
- **OQ2 `end_session` 页面落点**：选项 (a) 三通道同落点、保持 OQ3=c / (b) 忠于旧契约字面强制回首页。**human 2026-11-16 裁 (a)**。
- **OQ3 引擎 24h 日切**：选项 (a) 接受 / (b) 关闭引擎空闲断连。**human 2026-11-16 裁 (a)**。

### 3.3 非目标（防 scope creep）

- **不改引擎代码与 config**：`server/deploy/engine-patch/**` 与 ECS `core/**` 零 diff；`close_connection_no_voice_time: 86400` + `end_prompt.enable: false` 保持。
- **不新立会话上下文机制、不做会话边界注入**：共享短上下文已由 ADR-015 承载（重评触发见 PROPOSAL §6 留根 2）。
- **不改需求语义**：60s 阈值、结束三通道、录音态按键口径、记录会话不适用 60s 全部照现行 requirements 实现。
- **不动既有帧形状**：不新增 view、display 白名单仍 9、既有 `kidpal_*` 上行帧字段零改动。
- **不复活决策 21 方案二**（设备腿常驻、引擎腿按会话起停）——已随 ADR-015 失效。
- **不做远程发布**：本 workstream 到本地合并 + 真机轮；远程发布走 `release-pr.mjs --publish`，PR 合并仍 human。

### 3.4 逃生门申报

实现期若发现与 spec / 契约冲突：回 `/arch-change` 判级并由 human 冻结；契约面由 architect-reviewer 四分类后随批原子落（无人签门）。不在 Pod 内自裁。

## 4. 拆包预判（意图提示，非权威）

**2 包，serial WIP=1**（human 明示「不要拆太碎」；拆包权威 = portfolio-planner 四件套经门 1 冻结）：

| | **P1** | **P2** |
|---|---|---|
| changeId 建议 | `261116-channel-resident-core` | `261117-session-end-intent-longretry` |
| 内容 | 通道常驻核心：四条收尾路径收口到单一处理器 + `listen stop` 取代 `CloseAudioChannel` + 60s 超时 producer + VAD 沿重置 + 心跳 no-op 分支 + tick 自愈巡检；front 侧心跳 producer + `listen stop` 每会话收尾三件 + ctx 边沿兜底 | 云端结束意图三段补链（prompt 告别纪律 + `display_schema` self_push 豁免口扩 + front 截获-自推 + 设备 `case 0` → `EndVoiceSession`）+ 退避封顶长跑 + 离线期人工意向位 |
| 五档 | ②单包 Full | ②单包 Full |
| riskTier / sourceType | **high** / `proposal` | medium / `proposal` |
| 依赖 | 架构批已 FROZEN 且 canonical 三件已 landed（`docs/architecture.md` §12 出现 `261115-arch-review-session-channel-resident` 行） | P1 已合 main（`end_session` 触发源必须收口到 P1 建立的单一处理器；长重试守卫语义在常驻模型下才成立） |
| 真机轮 | 轮 A（human 持板） | 轮 B（human 持板） |

**为什么 60s 超时归 P1**：它不是额外触发源，是让通道常驻安全的补偿控制——今天设备一轮聊完停在聆听态，最终是被「约 137 秒后 WS 通道超时」这个意外兜底拽回 Idle 的；P1 把通道做成常驻后该兜底消失，若 60s producer 不同批落地，净效果是「孩子走开后麦克风永久常开」。

`application.cc` 两包改的是不相交函数区（P1 = 收尾路径 + `HandleEndVoiceSessionEvent` + tick 消费区；P2 = `TryStartAutoConnect` task 体 + 四个人工入口守卫行），故不需 rebase 策略，但 P2 必须从 P1 合并后的 main 起分支、plan.md 重新取行号锚。

## 5. workstream 收口标准

整列车「做完」= P1 轮 A + P2 轮 B 两轮真机全绿，且四条 debt 以**真机凭据**结项（`TECH-261114` = A1+A2；`TECH-261012` = A3；`TECH-261014-session-end` = A6+B1；`TECH-261014-autoconnect` = B4+B5）。收口记账归主线程（`debt-render.mjs` 归位）。

## 6. 留根清单

1. `docs/requirements.md:48` ⚠️「60 秒超时实现侧对账」注记刷新——实现包 integrator docs-delta 或需求面对账小批，主线程路由。
2. 会话边界注入（跨会话语境串味）——本批显式不做；重评触发 = 真机出现可复现实例。
3. 引擎 RAM `Dialogue` 无界增长 / `mem_local_short` 只在连接关闭 save——接受 24h 日切为准；重评需 human 授权引擎代码改动。
4. `IsTimeout()` 命中即打 `ESP_LOGE`——实现包改为跳变沿单次或降频；若需改 `protocol.cc`，经 planner 扩单通道回评补 manifest。
5. RC-B6 省电档观测前提改变（收尾后不再降 LOW_POWER）——轮 A 重定观测口径。
6. `ses_machine_dispatch` 纯模型仍不接生产（维持验收 oracle 身份）——独立议题。
7. `protocol.cc:129-136` 与 `application.h:126-130` 注释漂移——交 P1 planner 扩单通道刷新。
8. debt 四条结项凭据必须是真机、不是合入。

## 7. 评审记录

- 2026-11-16：human 冻结屏四答（OQ1/OQ2/OQ3 全 (a) + 冻结 + 按 2 包开工），本件 FROZEN v1.0。
- 设计审链：architect 起草 → architect-reviewer R1 BLOCK(F1,F2,F3) → v1.1 → R2 GREEN；架构+契约面 packet-bound 落地（reviewedCommit `87dbf775`）。
- 拆包预判已随 human「按 2 包开工」确认；正式拆包权威 = portfolio-planner 四件套经门 1。