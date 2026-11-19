---
changeId: 261117-session-end-intent-longretry
title: 云端结束意图三段补链 + 自动重连封顶长跑与离线期人工意向不丢
rigor: full
riskTier: medium
sourceType: proposal
origin:
  ref: delivery/workstreams/261115-session-channel-resident/PROPOSAL.md
  workstream: 261115-session-channel-resident
reviewed-by: 主线程按 FROZEN workstream PROPOSAL 拆包（2026-11-16）；门 1 待 architect checklist review
date: 2026-11-16
gateProfile: ["evidence-integrity", "test-quality", "state-machine", "mcp-tool-contract"]
dependsOn: ["261116-channel-resident-core"]
mergeOrder: 78
---

# 背景摘录（整改型锚「问题与根因」，PROPOSAL §2）

- **云端结束意图三重断链**（TECH-261014-session-end-channels-unwired 的实现侧实测）：① live prompt 全文没有一条规则让模型在告别时下发 `control/end_session`（告别被指向 `home` view）；② front 机械门禁 `display_schema` 的 self_push 豁免口只开给 `start_organizing`，`end_session` 在 relay 与自推两条路径上都被拒；③ 设备 `mcp_server.cc` 的 control `case 0` 只调 `AbortSpeaking`，不结束会话、不回页。三段各自都不是错误决策，叠起来就是一条契约上存在、实现上不可能通的通道。
- **自动重连固定 3 次后静默退出**（TECH-261014-autoconnect-gives-up-silently）：服务端重启或家庭网络瞬断超过约 15 秒即设备静默离线、此后零重试；离线期间 front 经 MCP 下发的提醒全部丢失（无重投无补发），直冲 S-F1.4 提醒必达。P1 把通道做成常驻后，断线自愈成为唯一剩余的重建入口——3 次即止必须改长跑，且长跑必须配套人工意向位（见铁律）。
- 契约 §1.4 早已把 `end_session` 当作「front 机械下发」的原型来类比 `start_organizing`，却在表后段落说它不下发——契约自身自相矛盾，本批裁为「契约面需裁改的一边」（架构批 §1.4 已改，本包只兑现实现与 prompt 承载）。

# 可行性判据

- 契约已随架构批 FROZEN 并 landed：`end_session` 属「截获转发类」（relay 恒拒、front self_push 恰一帧）；`§1.6` 已写入告别回合纪律；设备消费面只需把 `case 0` 从 `AbortSpeaking` 改为 `EndVoiceSession()`（P1 已建立统一处理器）。
- front 已有 `_push_display_payload` 自推单源与 display 流截获先例（add_points / redeem_points 同型），`end_session` 复用同一缝即可，零新依赖。
- 退避表与 task 循环是 P1 之后的改动面（`TryStartAutoConnect` task 体 + 四个人工入口守卫行）。**与 P1 的重叠点**：`HandleEndVoiceSessionEvent` 与 `HandleToggleChatEvent` 两个函数被两包分别改不同行段（P1 = 收尾动作序，P2 = in-flight 守卫行），不是「不相交函数区」；P1 冻结件 `session_channel_resident.acceptance.c` 的 SCR-A2「in-flight 守卫逐字在世」由本包 SEI-A4 重锚为「守卫在场 + 置意向位 + 审计行」（已登记 `impactedTestsSeed` 与跨包转手项）。守卫三条件与「零第二条重连路径」在 P1 的常驻模型下语义成立。

# 跨包事实与共享冻结面约束（含跨切片边角）

- **依赖 P1**：统一会话结束处理器、Idle 分支八件清位链、常驻模型下的守卫三条件语义均以 P1 为准；本包不得另起收尾路径或第二套重连路径。
- **人工意向位是长跑的承重配套（不得省）**：现状 `auto_connect_in_flight_` 在整段 task 生命周期（含退避睡眠）为 true，四个人工入口见它即 no-op。若不配套意向位，长跑会把「按了没反应」从偶发放大为离线期恒定——比今天 3 次即止更糟。改法 = 入口置意向位 + 审计行；task 每段退避前查该位、命中即截短退避；连上后经 `Schedule` 按入口来源补投事件位。
- **网络断退出请求位**：WiFi 已断时不得空转重试；网络断事件置退出请求位，task 据此退出（`auto_connect_in_flight_` 所有退出路径必清）。
- **冻结齿反转与重写显式申报**：`test_record_dialogue_guide.acceptance.py` 的 `self_push=True` + `end_session` 由 `assertFalse` 改 `assertTrue`（断言消息随裁定改写）；`test_points_e2e_spike.acceptance.py` 的 relay 侧判据天然保绿、不得顺手改；`boot_auto_connect.acceptance.c` 的 K6-A2 tooth5 从 `[3]` + `{5,10,20}` 重写为 `{5,10,20,40,60}` + 恒 60s。
- **prompt 静态承载**：`server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md` 是静态语义单一事实源（arch §5.5.5 / ADR-001）；`OPENCLAW_SYSTEM_PROMPT_CARRIER` 是版本演进史唯一全量副本与声明面，两者与 `provider.config.yaml` 的 `static_prompt` 三键同批对账（三面相等不变式）。部署到 ECS 由 agent 按 `kidpal-ecs-gateway` 规程执行。

# 铁律约束

- **不改引擎代码与 config**：`server/deploy/engine-patch/**` 与 ECS `core/**` 零 diff。
- **relay 直转对全部四个网关决定类恒拒**（防伪造/注入）；self_push 豁免只开 `start_organizing` 与 `end_session`；`add_points`/`redeem_points` 在 self_push 下仍恒拒。
- **设备不得双重收尾**：`case 0` 只调 `EndVoiceSession()`，不再同时 `AbortSpeaking`（统一处理器内部处理 Speaking 分支）。
- **页落点按 OQ3=c**：`end_session` 不新增强制回首页特例分支。
- **守卫三条件与零第二条重连路径不动**：长跑只改预算与循环边界，不自建 task、不自己 `OpenAudioChannel`。
- **`auto_connect_in_flight_` 所有退出路径必清**（K6 既有不变量）。

# demo 证据形态（与 roadmap 一致）

- host runner + server pytest：SEI-A1–A4 与 SEI-B1–B4 的 RED→GREEN 与变异证据；relay/self_push 分叉、回环防护、prompt 三面相等为承重断言。
- device build：本轮改动编译面（`mcp_server.cc` / `application.cc` / 验收件）通过。
- 一轮真机轮 B（human 持板）：B1–B8 全绿（说「拜拜」全链通 / 告别语不误伤 / 伪造防护 / 拔网线自愈 / 重启 front 自愈 / 离线期提醒不丢如实记录 / 离线 30 分钟电量差 / 长跑期人工入口不僵死）；60s 自动关闭在轮 A 已首次发生（A6），本轮只回证；三源日志互证，证据落本包 `current/`。
# 验收方向（acceptance-author 细化，非冻结）

> 本节是拆包期的**断言方向与承重判据**，不是冻结验收；`acceptance` / `acceptanceTestsFile` / `redBaselineResult` 在 taskpackage 中保持空骨架，由 Pod 内 acceptance-author 唯一产出并冻结。

- SEI-A1: 设备 control end_session——mcp_server.cc control `case 0` 区内 `EndVoiceSession()` 恰 1、`AbortSpeaking(` 命中 0（负锚：防双重收尾）。回引契约 §1.4、PROPOSAL T5、ADR-019 ⑥。设计态红 = case 0 只 AbortSpeaking。
- SEI-A2: 退避表形态——`kAutoConnectBackoffS` 首定义为 5 元素递增秒序列 `{5,10,20,40,60}` + `kAutoConnectSteadyRetryS` 命名常量值 60；重写 K6-A2 tooth5（旧齿钉 `[3]` 长度与 `{5,10,20}`）并显式申报。回引 PROPOSAL T1 改法 4。
- SEI-A3: 长跑与退出——task 体内无 `attempt < 3` 字面；三个退出条件（成功 / bail / 网络断请求位）各在世；`auto_connect_in_flight_` 所有退出路径必清（K6 既有不变量保绿）。设计态红 = 3 次即 exit。
- SEI-A4: 人工意向位——四个人工入口（HandleToggleChatEvent / HandleStartListeningEvent / HandleEndVoiceSessionEvent / HandleWakeWordDetectedEvent）区内 `pending_manual_intent_` 置位命中各 ≥ 1 且不再是裸 return；task 体内查意向位 + 截短退避 + 连上后 Schedule 补投三件在世。回引 PROPOSAL T1 改法 4 的承重配套。设计态红 = 长跑期人工入口永久 no-op。
- SEI-B1: 门禁分叉——validate_display({view:control, data:{action:end_session}}, self_push=True)[0] 为 True（反转既有 RDG-A5 断言并显式申报）；validate_display(缺省 relay) 仍为 False（must-stay-green，与 test_points_e2e_spike 同判据）；add_points/redeem_points 在 self_push 下仍恒拒（正锚，防豁免口误扩）。回引契约 §1.4 / §1.5。
- SEI-B2: 截获-自推——引擎 display 流投一块 control/end_session → relay 输出不含该块（不透传）+ 设备腿恰收一帧自推 control/end_session（_push_display_payload 单源、过 display_schema）。回引契约 §1.4 截获转发类。
- SEI-B3: 回环防护——front 自推的那一帧不再被 relay 路径二次截获（假 socket 双跑一轮，帧数恒 1）。 回引契约 §1.4 截获转发类、PROPOSAL T5。
- SEI-B4: prompt 承载三面相等——workspace-kidpal AGENTS.md 含告别回合纪律关键词（拜拜/不用了 → control/end_session）+ home 行语义已收窄；OPENCLAW_SYSTEM_PROMPT_CARRIER 声明面按 ADR-001 不变式同批对账（沿 test_261022_m5_k1 / test_261023 先例形态）。回引契约 §1.6、arch §5.5.5。

**建议验收文件（acceptance-author 定名可调整）**：`firmware/main/test/session_end_intent_longretry.acceptance.c`、`server/acceptance/test_session_end_intent.acceptance.py`

**红基线命令（沿用）**：`host: wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && cd firmware/main/test/host_app && wsl -d Ubuntu-24.04 -- ./build/host_infra_test.elf（仓根起）；server: python -m pytest -q --import-mode=importlib -m "not live" server/acceptance（仓根起）`

**红基线预期形态**：新验收件落地时设计态红（实现缺席）；不得以推算代替实跑，数值由 acceptance-author 冻结时回填。

# 技术方案（planner+architect 预评 · 2026-11-19）

> 本节由 planner 按 architect 前置技术预评（ARCH-ADVISORY planning 期档，2026-11-19，advisory 不豁免任何门）摘录回填；实施以 brief 与冻结验收为准。

**总体**：八条 SEI 全部落在已 FROZEN 的契约 §1.4/§1.5/§1.6 与 arch §5.2 / §6.4 包络内，零新依赖、零引擎改动。最大风险不是功能写不出来，而是 `application.cc` / `mcp_server.cc` 被多件冻结验收用「函数定义顺序锚 + 区内子串计数」双向夹住，改动位置错了会出结构性假红、白烧 fixer 预算。

**贯穿全包的最高优先纪律**：`application.cc` **零新增成员函数定义**。现有冻结件用 `"void Application::X()"` → `"void Application::Y()"` 成对字面锚切区，任意两个被锚函数之间插入新定义都会让某个区被截短/放大。确需 helper 时，唯一安全落点 = 文件顶部常量区的 `static` 自由函数。

**逐条要点**：
- **SEI-A1**：`mcp_server.cc:1737-1739` 的 `case 0` 改 `app.EndVoiceSession()`（事件位投递，跨任务不得直调 `HandleEndVoiceSessionEvent()`）；负锚切区取 `case 0:` → `case 1:` 之间（全文件另有 :1036 / :1741 两处 `AbortSpeaking`，不得误判）。同批改写 :1010-1027 DISCARD_RECORD 注释（P1 转手项），**不得写出带左括号的 `StopListening(`**——`record_session_live.acceptance.c` 的 RSL-A5 tooth 4 读原始文件不剥注释，会立即假红。
- **SEI-A2/A3**：`application.cc:131` 表改 `{5,10,20,40,60}`，新增 `kAutoConnectSteadyRetryS=60`；task 体去 `attempt < 3` 字面、改无界循环 + 稳态 60；`auto_connect_in_flight_` 清位保持单点收尾。**注释不得复述符号字面**（`kAutoConnectBackoffS` / `kAutoConnectSteadyRetryS` / `kSessionIdleTimeoutS` / `kStoryEmptySessionTimeoutS`），否则源锚切到注释片段造成假红。网络断退出请求位：置位在 `HandleNetworkDisconnectedEvent` 函数体最顶端、三态门之外；`TryStartAutoConnect` 预检块加早退（守卫由三条件扩四条件，显式申报）；连接成功 / `HandleNetworkConnectedEvent` Idle 支路清位。退避睡眠建议分片（1 秒粒度重查意向位与退出位），使「截短」真截短、断网退出延迟 ≤1 秒。
- **SEI-A4**：`application.h` 紧邻 `auto_connect_in_flight_` 加 `EventBits_t pending_manual_intent_ = 0`（直接存 `MAIN_EVENT_*` 位，最后写者胜、不做 OR 累积）。四处守卫块内**不得新增 `return;`**；`HandleStartListeningEvent` 的插入槽必须在 `kidpal_story_session_ctx_set(0);` 之后、`return;` 之前（RSL2-A3 序钉）。补投顺序：先清 `auto_connect_in_flight_`、再经 `Schedule` 投事件位（否则补投被自己吞掉）；只在成功路径补投，bail / 断网退出清而不投。「唤醒词」按两个代码点落地（`HandleWakeWordDetectedEvent` + `WakeWordInvoke`），语义仍四入口。
- **SEI-B1**：`display_schema.py` 只扩 self_push 豁免口（新增 `SELF_PUSH_EXEMPT_CONTROL_ACTIONS = {start_organizing, end_session}`），**不得把 `end_session` 从 `GATEWAY_DECISION_CONTROL_ACTIONS` 摘出**（K0PTS-A3 四值逐个 assertIn）。同批反转 :134-140 / :157-159 两处「即便自推仍拒」注释。
- **SEI-B2/B3**：截获落点恒为 `_relay_display_blocks_to_device`（两条 relay 路径唯一收敛点），插在 points 截获之后、`build_display_tool_calls` 之前；`continue` 必须早于 envelope 生成，否则设备收「降级 text + 自推」两帧（B3 「帧数恒 1」正是抓这个）。建议断言写成三重：恰 1 帧 control/end_session + 零帧友好降级 text + relay 输出不含该块。自推走设备腿、relay 读引擎腿，回环在结构上不可能——B3 的价值是抓错实现。不加会话态门控、不加每回合去重（契约语义 = 每个被截获薄意图一帧）。
- **SEI-B4**：`AGENTS.md:31` home 行收窄；告别回合纪律写成「回合路由表」里的一行（行级定位是本仓最稳承载形态），新节标题不得含 课表/故事/提醒/记忆/底线；**`server/deploy/openclaw-system-prompt.v11.md` 绝对零 diff**（K1-E1 钉 sha256）。`OPENCLAW_SYSTEM_PROMPT_CARRIER` 改动可选、`provider.config.yaml` 目标零 diff；`server/deploy/manifest.yaml` 本包**不需要改**（AGENTS.md 条目已在册、md5 是 verify 期现算）。真机轮 B 前须按 `kidpal-ecs-gateway` 规程铺 AGENTS.md 到 ECS，B1 证据同时抓 live md5。

**planner 裁决（2026-11-19）**：① 不抽纯逻辑缝，维持源锚结构齿 + 变异证据覆盖差一边界；② 意向位不设时效（忠于 arch 字面），超长离线后补投的观感项显式记入验收件与真机观测；③ 唤醒词按两代码点落地。

**回填**：`changeManifest` 13→14 条（+`host_app/main/CMakeLists.txt`、+`session_channel_resident.acceptance.c`、+`provider.config.yaml`）；`impactedTestsSeed` 4→19 条。
