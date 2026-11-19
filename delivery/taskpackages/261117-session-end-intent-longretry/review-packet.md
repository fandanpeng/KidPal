# Review Packet

> 包：`261117-session-end-intent-longretry`（P2，workstream `261115-session-channel-resident`，rigor full，riskTier medium，deliveryImpact milestone-affecting）。
> integrator 合并门前审查包。承重 fact = `verifier-evidence/rounds/r2.json`（final round 2，ok=true，observedGateExit=0）；supervisor 输入以本包 + r2 fact + 已 commit diff 为准。

## Change Size
- Files changed: 实现/验收面 **12 件**（10 件跟踪修改 + 2 件新增冻结验收）；另有 delivery 簿记件（plan / taskpackage / current / workstreams / 本收口制品）。
  - 实现面（6）：`firmware/main/application.cc`、`application.h`、`mcp_server.cc`、`server/gateway/display_schema.py`、`server/gateway/front/front_app.py`、`server/deploy/openclaw-agents/workspace-kidpal/AGENTS.md`
  - 验收面（6）：新增 `firmware/main/test/session_end_intent_longretry.acceptance.c`（802 行）、`server/acceptance/test_session_end_intent.acceptance.py`（618 行）；重锚 `firmware/main/test/boot_auto_connect.acceptance.c`、`session_channel_resident.acceptance.c`、`host_app/main/CMakeLists.txt`、`server/acceptance/test_record_dialogue_guide.acceptance.py`
- Net diff: 实现/验收面跟踪件 **+335 / −66**；两件新增冻结验收 **+1420 行**（净实现改动集中在 `application.cc` 长跑循环 99 行、`front_app.py` 截获缝 43 行、`mcp_server.cc` case 0 26 行）。
- Scope: 设备结束通道收口 + 自动重连封顶长跑 + 离线期人工意向位 + front 端 end_session 截获-自推 + 告别 prompt 承载。零引擎改动、零契约/spec diff、`provider.config.yaml`/`OPENCLAW_SYSTEM_PROMPT_CARRIER`/`openclaw-system-prompt.v11.md` 零 diff。
- Split recommendation: 无（单一连贯改动，五个决策 B1–B5 在同一批闭合；拆分会破坏「长跑 + 承重配套意向位」的原子性——意向位不得从长跑拆出）。

## Verification Evidence
- Structural gates: `firmware-build` PASS（`VERIFY_FW_BUILD_REQUIRED=1` 真编译，应用编译通过）；`contract-lint` PASS（1 条 pre-existing 相对链接 broken 警告，非本轮引入）。
- Acceptance/unit/api/ui/e2e: final r2 按 `test-scope-final.txt`（41 条）圈定真跑——`unit` **530 passed / 7 deselected / 104 subtests passed（37.43s）**；`acceptance` **362 passed / 7 deselected / 15 subtests（34.99s）**。固件 on-target 验收（SEI-A1–A4 / SEI-C1）为源锚 + 结构不变量，host runner 结构核验通过，真机行为归轮 B。
- Security gate: PASS（`securityReviewRequired=false`）。relay 对全部网关决定类恒拒（防伪造/注入）保绿；self_push 豁免仅 front 机械单源。
- Mutation check: PASS——9/9 per-acceptance 变异（SEI-A1–A4、SEI-B1–B4、SEI-C1）各 `turnedRed:true` + `restored:true`，证据 `mutation-evidence/builder-round-1.md` + taskpackage `mutationEvidence.perAcceptance`。
- Docs sync: PASS（`docsSyncRequired=false`；见 `docs-delta.md`）。
- Scope 圈定依据：`detect_changes`（project `C-AI-KidPal-pod-m2`，since `05417c1f`，inbound，depth=2）112 种子 → 115 impacted 节点；per-Pod 图索引可用，**正常圈定、未降级**。清单落盘 = `delivery/taskpackages/261117-session-end-intent-longretry/test-scope-final.txt`（41 条：本包冻结件 ∪ 显式并入的固件源锚雷 ∪ impactedTestsSeed ∪ blast radius 映射测试 ∪ 共享 seam 安全余量）。

## Demo 对账（demo 证据形态 ⟷ roadmap#261117-session-end-intent-longretry 效果段）

roadmap 效果段（`delivery/workstreams/261115-session-channel-resident/roadmap.md:51`，逐字）：*「孩子说『拜拜/不用了』时，小伴道别后真正把会话收起来、麦克风关闭、回到原来的页面；网络断掉或服务端重启一阵子之后，设备自己恢复，不用孩子再按一次；离线期间按下的键或喊的唤醒词也不会白丢。」* Demo 证据形态 = **host runner + server pytest + device build + 一轮真机轮 B（human 持板）**。

| roadmap 效果段 | host runner（本轮已绿） | server pytest（本轮已绿） | device build（本轮已绿） | 真机轮 B（待 human） |
|---|---|---|---|---|
| 说「拜拜/不用了」→ 道别后收会话、麦克风关、回原页 | SEI-A1（case 0 收口统一处理器源锚）+ 19 条 must-stay-green 保绿 | SEI-B1/B2/B3（真跑 pump + 真门：截获-自推恰一帧）+ SEI-B4（prompt 承载） | 编译面通过 | **B1/B2/B3**：端到端收口 + 告别语不误伤 + 伪造防护 |
| 网络断/服务端重启一阵后自愈，不用再按 | SEI-A2/A3（退避表值序 + 长跑无封顶 + 三退出条件 + 网络断退出位源锚） | 零偏移保绿 | 编译面通过 | **B4/B5**：拔网线退避 5/10/20/40/60 后稳态 60s + 重启 front 60s 内自愈 |
| 离线期按键/唤醒词不白丢 | SEI-A4（五入口置意向位 + 审计行 + task 截短/补投源锚） | 零偏移保绿 | 编译面通过 | **B8**：每次有审计行、连上后能接续；**B6**：离线期提醒覆盖窗；**B7**：离线 30 分钟电量差 |

说明：host/server 层已把「代码在场 + front wire 分发 + schema 门禁」钉成机判；「设备真收口、真实时序、LLM 遵循、离线期观感」是设计上 host 不可量化的性质，显式归真机轮 B（见下节逐条申报，零伪造 PASS）。

## 未覆盖面申报（对照 acceptance[] + 契约条目 + roadmap 效果段）

> 判据：逐条列「有无验收断言 / 未跑的设计性质 + 现状（本轮覆盖到哪）+ 去向」。零缺口的项标「已覆盖」，其余归真机轮 B（`exitCriteria KSE-E4` B1–B8；human 持板）。

| # | 未覆盖面 | 有验收断言？ | 现状（本轮覆盖到哪） | 去向 |
|---|---|---|---|---|
| 1 | **B1 设备真收口与页回落**（说「拜拜」→ 麦克风关、按 OQ3=c 回落、通道仍常驻） | 无运行时断言（GAP-1 / GAP-B1 已显式申报） | SEI-A1 源锚（case 0 `EndVoiceSession(` 恰 1 / 零双收尾）+ SEI-B1/B2/B3 证明 front wire 分发正确；`EndVoiceSession` 事件位被主循环 `HandleEndVoiceSessionEvent` 真正收口的运行时观测 host 不可量化 | 真机轮 **B1**（三源日志：设备 KPLOG listen stop 上行 + front 收尾 + 引擎语境释放） |
| 2 | **B2 告别语不误伤**（「拜拜，明天再聊」只结束会话） | 无（GAP-B3：LLM 判据） | SEI-B4 证 prompt 措辞在场；LLM 是否产 `control/end_session` 薄意图、是否误伤告别语 offline 不可机判 | 真机轮 **B2** |
| 3 | **B3 伪造防护**（引擎流注入 end_session → relay 恒拒、仅 front 截获-自推单源可下发） | 部分（offline） | SEI-B1 relay 缺省 `assertFalse`（must-stay-green，防伪造/注入破口）行为级覆盖 front 侧恒拒 | 真机轮 **B3**（引擎流真实注入路径） |
| 4 | **B4 真机断网自愈节奏**（退避 5/10/20/40/60 后稳态 60s；恢复自动重连；期间按 KEY 不再静默 no-op） | 无真实时序断言（GAP-2 / GAP-3） | SEI-A2 机制级（值序 + 稳态常量）+ SEI-A3 源锚（无封顶字面 + 三退出条件 + 网络断退出位）；host 无真实时钟，秒级节奏不机判 | 真机轮 **B4** |
| 5 | **B5 重启 front 自愈**（60s 内自动重连，对照原始事故） | 无运行时断言 | SEI-A3 源锚（网络断退出位置位 / task 查位 / 重连清位三件在场 + 预检早退先于置 in-flight）；真实 front 重启窗口自愈归观测 | 真机轮 **B5** |
| 6 | **B6 离线期提醒覆盖窗**（60s 超时重推 ×3 覆盖窗；覆盖不到如实记录、不在本批扩范围） | 无（TECH-261014 提醒必达的重后果面） | 本包打通长跑自愈通道（B4 前置），未对「提醒重推覆盖窗」加机判；覆盖不到 = 如实记录 | 真机轮 **B6** |
| 7 | **B7 离线 30 分钟电量差**（回填 arch §8.1 风险 15） | 无（功耗不可 offline 机判） | 长跑退避/分片睡眠形态已落；实际功耗差需真机 30 分钟实测 | 真机轮 **B7** |
| 8 | **B8 超长离线后补投观感 + 长跑期人工入口不僵死**（每次有审计行、连上后能接续；意向位不设时效） | 无（GAP-3 / GAP-4；planner 裁决 b：意向位不设时效，忠于 arch §6.4「孩子那一次动作不丢」字面） | SEI-A4 源锚（五入口置位 + 审计行 `manual_intent_queued` + task 截短/补投次序）；超长离线后补投的观感项 = 真机严格观测项，本包不断言 | 真机轮 **B8**（观测项） |

其余 acceptance（SEI-A1–A4 / SEI-B1–B4 / SEI-C1）与 19 条 must-stay-green seed 本轮均已机判覆盖，无缺口。

## Simplification Debt
| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （本包改动未引入新 `minimal:` 标记） | — | — | no-trigger |

既有 `server/gateway/front/front_app.py:3072` 一条 `minimal:`（reminder 轮询固定间隔，**pre-existing、非本包引入**，不在本包改动 hunk 内）——沿用 no-trigger，本包不动。

## Findings
| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | Maintainability/Correctness | `pending_manual_intent_` / `network_exit_requested_` 为跨任务共享裸标志（主循环 handler 写、auto-connect task 读），无 `volatile`/原子量；ESP32-S3 上 32 位对齐字访问实践上原子，且与既有 `auto_connect_in_flight_` 同款约定 | changed-code hunk（`application.h` 成员 + `application.cc` task 读）/ 既有 `auto_connect_in_flight_` 先例（非伪造 rule）；heuristic = 跨任务裸标志 | 本批不修（沿用既有约定；如后续可观测到撕裂再统一升为原子量/事件组）——记录为观察项 |
| NIT | judgement-call | Correctness/Observability | `_consume_end_session_intent` 在自推 `_push_display_payload` 抛异常时记 warning 但仍返回 True（「已消费」——控制帧不透传）；推送失败则该次 end_session 意图丢失、不重投 | changed-code hunk（`front_app.py` `_consume_end_session_intent` except 分支） | 本批不修（有日志；控制帧恒不透传是冻结安全语义；契约语义 = 每个被截获薄意图一帧，无重投要求）——记录为观察项 |

无 BLOCKING / SHOULD finding。两条 NIT 均为 judgement-call，不阻塞合并门。

## Five-Axis Review

1. **Correctness**：五个决策逐条落地并机判——B1 end_session 收口统一处理器（`mcp_server.cc` case 0 → `EndVoiceSession()`，不再 `AbortSpeaking`，设备不双重收尾）；B3 退避表 `{5,10,20,40,60}` + `kAutoConnectSteadyRetryS=60` 无界长跑（三退出条件：成功 / bail / 网络断退出位）；B4 人工意向位（五入口置位 + 审计行，task 截短退避 + 连上后先清 in-flight 再 `Schedule` 补投，bail/断网清而不投）；B5 页落点按 OQ3=c（不新增强制回首页特例，`case 0` 只投事件位由统一处理器收口）；front `_consume_end_session_intent` 在 `_relay_display_blocks_to_device` 的 points 截获之后、envelope 生成之前 `continue`（关掉「降级 text + 自推双帧」的错实现）。边界条件（网络断请求位置于三态门之外、预检早退先于置 in-flight、1 秒分片睡眠重查）均在世。无正确性阻断项。
2. **Test quality**：9 条冻结验收断言真实行为（server 侧真跑 pump + 真 `validate_display` 门，非文本比对；固件侧源锚 + 结构不变量适配 host 不可编面并显式申报 GAP-1..4 / GAP-B1..3）；19 条 must-stay-green seed 逐条列证保绿（含 RSL2-A3 / RSL-A5 tooth4 / OCE-A1 三颗硬雷与注册序族）；变异 9/9 各自 turnedRed + restored，证明断言有判别力。测试质量达标。
3. **Contract/boundary safety**：零契约 diff（§1.4/§1.5/§1.6 已由上游架构批冻结）；`display_schema` 只扩 self_push 豁免口（新增 `SELF_PUSH_EXEMPT_CONTROL_ACTIONS={start_organizing,end_session}`），`GATEWAY_DECISION_CONTROL_ACTIONS` 四值一个不摘（K0PTS-A3 逐个 assertIn 保绿）；relay 恒拒不变；无 event/tool/shared-type 漂移；AGENTS.md 改 farewell 行 + home 行收窄落契约 §1.6；carrier/provider/v11 prompt 零 diff（ADR-001 三面相等不变式保绿）。
4. **Operability/security**：安全——relay 对全部网关决定类恒拒（防伪造/注入），self_push 仅 front 机械单源放行，points 双路径恒拒（豁免口未误扩，`test_points_spike_units` / `test_points_ledger_core` 保绿）。可观测——五入口审计行 `manual_intent_queued ... src=queued_*`、front `end_session_intent intercepted corr_id=...`（correlationId 贯穿截获/推送两段）、建连 task 退出打栈高水位日志。输入/权限/secret 面无改动。
5. **Maintainability/simplicity**：改动局部、无过早抽象（`_consume_end_session_intent` 与既有 `_consume_points_intent` 同形单职责）；长跑循环重写清晰（表 + 稳态、分片睡眠）；交接的辅助缝落点遵守「application.cc 零新增成员函数定义」纪律（无新定义插入被锚函数之间）。两条 NIT 记录在 Findings，非结构性问题。

## Residual Risk
- Known limitation:
  - 设备真收口、真实退避时序、LLM 遵循度、离线期提醒覆盖窗、离线功耗差、超长离线补投观感 **host 不可量化**，全部归真机轮 B（B1–B8，human 持板）——本包未伪造任何 PASS。
  - 两条 NIT（跨任务裸标志；自推异常时意图丢失不重投）为已知观察项，理由见 Findings。
- Follow-up recommended:
  - 真机轮 B 通过后，由主线程结项 `TECH-261014-session-end-channels-unwired` / `TECH-261014-autoconnect-gives-up-silently`（本包只刷 trigger + 收口注记，status 维持 deferred）。
  - `docs/requirements.md:48` ⚠️ 对账注记刷新 = 跨包事项，主线程列车收口路由。
  - 真机轮 B 前按 `kidpal-ecs-gateway` 规程铺 `AGENTS.md` 到 ECS，B1 证据同时抓 live md5。
- Human decision needed: 真机轮 B 由 human 持板验收（KSE-E4）。`decisionCoverage` 无 `landsIn: dropped` + `reason: risk-accepted` 条目（五项决策均 landsIn exitCriteria:KSE-E1..E4），residual risk 无 risk-accepted 项。