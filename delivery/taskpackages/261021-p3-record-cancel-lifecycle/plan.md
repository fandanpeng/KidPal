---
changeId: 261021-p3-record-cancel-lifecycle
rigor: full
riskTier: high
sourceType: regression
reviewed-by: architect
date: 2026-10-21
---

# 根因假设 + 修复 + 影响面（P3 记录会话取消/收尾生命周期两侧闭环修复）

> **档位**：② 单包 Full（architect 分流裁决 = ② Full / riskTier high〔debt 预判 medium 升级：scheduler/状态机带用户影响 + 跨端双命中〕/ sourceType regression / rigor full；human 触点① 2026-10-21 确认）。
> **changeId 复用说明**：单包 ② 直接复用 spine changeId `261021-p3-record-cancel-lifecycle`，不另铸（change-intake frontmatter 字段 1）。
> **来源**：debt 条目 `docs/debt/tech/TECH-261021-story-session-cancel-lifecycle.md`（本条目 = 其技术输入的消费；spine = `delivery/intake/261021-p3-record-cancel-lifecycle.md`，q1–q4 全 resolved）。
> **launch 前置**：契约面 anchor 回填前不得 launch（change-intake §8 spine 源条款）——架构+契约批先落，实现后跟（261019 先例）。
> **人类触点① 裁决记录（2026-10-21）**：② Full 确认 / OQ1 = Design B 新增 `kidpal_story_discard` / OQ2 = cancel-on-disconnect 维持 / OQ3 = 复验③重定义 (a)。

## 根因（三症状同根：取消/收尾生命周期两侧未闭环，architect 读码逐点实证）

1. **teardown 悬挂（症状③⑤的直接机制）**：ws_gateway 收尾 `await asyncio.gather(pump_up, pump_dn)`（front_app.py:1496），finally 块才跑 `_unregister_alert_legs` + `_get_story_acc().cancel(session_id, reason="ws_disconnect")`（:1497-1506）。设备丢弃关通道后 pump_dn 立即结束（:982-983/:1018-1020），但 pump_up 阻塞在引擎腿读帧上——引擎在记录语境下被抑制（connection.py:188/:1286/:1306/:1315）、无流量，pump_up 永不返回 → gather 悬挂 → finally 永不执行 → cancel 不落、旧腿不摘。固件侧注释声称「关通道即达 front 断开钩子」（application.cc:894-895、mcp_server.cc:691-693）——设计意图在，机制是断的。
2. **zombie 会话 + zombie 腿跨重连泄漏**：决策 29 常驻重建 T=5s 后新连接建起（新 session_id，front_app.py:1441），旧会话在 sweep（STUCK_GUIDE_SECONDS=8、GUIDE_ROUND_LIMIT=2，story.py:56-59）下继续命中；引导播报 `_speak_to_engine_legs` 用**全局** `_alert_legs` 快照（front_app.py:243/:1053）→ 引导注入打进新连接的引擎腿（debt 三次实证延迟 23-55s 形状一致）；自动收尾 `_story_auto_finish` 同用全局腿（:619）→ `control start_organizing story_consumed=1 page=6` @ 237.4s → 整理归档 → story_list 6→7（症状⑤实锤全对上）。
3. **在途引导回合无存续守卫（症状③竞态残余）**：`_story_guide_round`（front_app.py:587-608）入口把「会话已被取消」吞成空素材照发 LLM（:593-600），返回后无复查直接播报（:601-602）；`mark_guide_round` 用 `_ensure`（story.py:695）会重建会话壳。
4. **server 驱动收尾设备侧不回 Idle（症状④）**：ctx 清位唯一通道 = HandleStateChangedEvent Idle 分支（application.cc:1113-1138，:1132 `kidpal_story_session_ctx_set(0)`）；KEY 收尾路径设备自行 Idle 闭环，**语音收尾/自动收尾路径 front 检出时设备仍在 Listening**，start_organizing 消费只切页渲染（mcp_server.cc:1319-1334）→ ctx 停死 1 → show_home 守卫（kidpal_render_node.cc:570 → rn_record_session_live_locked :1250-1255）按设计意图拦截（:566-569 注释「收尾后自然放行」——server 路径漏清，注释本身即证据）→ 列表页长按六连拦。
5. **丢弃路径零上行帧**：story_page BOOT → KSPV2_ACT_DISCARD_RECORD → `story_key discard_rec`（mcp_server.cc:690-700）→ `Application::CloseAudioChannel()` → `local_close_settle src=discard_record`（application.cc:893-911）——不发 listen stop、不发任何通知。

## 修复（四件，S 段不分 Pod、顺序推进）

| 件 | 内容 | 修位 |
|---|---|---|
| **F1 teardown 悬挂修复（与 wire 分叉无关、必修）** | gather 等双泵 → 任一泵结束即收尾（asyncio.wait FIRST_COMPLETED + cancel 另一泵，或 pump_dn 结束时主动 cancel pump_up），保证 finally 的 cancel + unregister 在设备断连后即时执行；同时修复「未停录断开会话累积缓冲泄漏」（HWV-A11 原意图）与 zombie 腿残留 | front_app.py:1496-1506 |
| **F2 显式丢弃通知（Design B，human 已裁）** | 新增 `kidpal_story_discard`（端→网关私有扩展消息，零载荷或仅可选 reason；形状 = `{"session_id":"...","type":"kidpal_story_discard"}`，沿 protocol.cc 既有拼串先例）：网关整条消费拦截不下透、幂等（session 已 pop 时 cancel 返回 False 即 no-op）、审计 log 一行；**会话绑定以连接权威——front 按本连接 session_id 取消，不信载荷值**。设备在 DISCARD_RECORD case 于 CloseAudioChannel **之前**同帧序发送（同一 TCP 有序，front 泵先消费 discard 再收 disconnect）→ front `acc.cancel(reason="discard")` 立即释放会话。契约落地 = /arch-change 契约包三节（§四新增一行 + 变更记录条），随批原子落 | front_app.py:451-523（_consume_kidpal_control 新增 discard 分支）；firmware mcp_server.cc:690-700 + protocols/protocol.h/.cc（SendKidpalStoryDiscard，逐字仿 SendKidpalStoryDelete / SendAbortSpeaking 形态） |
| **F3 在途回合存续守卫 + 腿集会话归口（症状③）** | ① 双点存续守卫：引导回合入口（build_organizing_input 返回 None = 会话已释放 → 直接返回**不发 LLM**）与 LLM 返回后播报前（复查 `routing(sid)=="accumulate"`，否则丢弃产出、不播报、不计轮）；② mark 不复活（先判 accumulate 再 mark_guide_round）；③ **不引入会话代次**（sid = 每连接 uuid4，cancel 后同名会话不会重建，双点守卫已完备——比例化裁定）；④ 不误杀正常收尾：finish 路径同样先 cancel(reason="finish")（:927），在途引导与收尾竞态时引导产出被丢弃是正确行为（设备已进整理态）；⑤ 腿集归口：新增 session_id → (device_ws, engine_ws) 连接级映射（ws_gateway 登记/摘除，与 _alert_legs 同生命周期），sweep 引导注入与自动收尾只打**该会话归属连接的腿**；提醒（:1316）与课表（:1229）两消费点维持全局腿不动 | front_app.py:587-608 / :611-619 / :239-250 / ws_gateway 腿登记处（:1477/:1498） |
| **F4 start_organizing 消费补 settle（症状④）** | start_organizing 消费沿补 `app.StopListening()`（事件位投递，mcp 回调上下文安全，与 KSPV2_ACT_STOP_RECORD :687 同款形态）→ HandleStopListeningEvent（application.cc:1021-1034）发 listen stop + SetDeviceState(Idle) → Idle 分支既有清位链接管（[R2] 页态复位 + ctx_set(0) :1132 + RaiseReminderQueue :1138——S-F1.4 AC6「会话结束立刻弹」顺带对齐）→ 守卫自然放行。**幂等**：KEY 路径设备已先自行 Idle，重复 settle 见 state!=Listening 即 no-op（:1028 分支），不重发 listen stop。**front 侧安全**：settle 发出的 listen stop 到达时 front 会话已 cancel（finish 先释放后演出，:927 先于 :940），_story_stop_on_listen_stop 因 routing!="accumulate" 直接 False（:379-380），零副作用。**竞态**：ctx 清位先于 story_text 渲染，story_text 路由不依赖 ctx（盖章判据 = archive_pend），次序安全 | firmware mcp_server.cc:1319-1334；application.cc 预判零改（Idle 清位链既有完整） |

**预判零改文件**：server/gateway/story.py（守卫支撑缝如需才动，极小）、server/deploy/engine-patch/connection.py（注入/抑制语义不动）、application.cc（Idle 清位链复用）。

## 影响面（changeManifest 种子，仓根相对）

```
server/gateway/front/front_app.py        # F1 teardown / F2 discard 消费 / F3 守卫+归口
server/gateway/story.py                  # 预判零改~极小（守卫支撑缝如需）
server/tests/                            # pytest 新增/更新（验收种子 1-4）
firmware/main/mcp_server.cc              # F4 case 3 补 settle；F2 DISCARD_RECORD 补发送
firmware/main/protocols/protocol.h       # SendKidpalStoryDiscard 声明
firmware/main/protocols/protocol.cc      # SendKidpalStoryDiscard 定义（逐字仿 SendKidpalStoryDelete）
firmware/main/test/record_session_live.acceptance.c      # host 齿更新（discard 发送链 + 负锚换锚）
firmware/main/test/record_dialogue_guide.acceptance.c    # host 齿更新（settle 消费链 / 幂等）
# 契约面 docs/contracts/interface-contract.md —— 经 /arch-change 主线程 packet-bound 落，不在本包 builder diff 内
```

**负锚换锚**：record_session_live.acceptance.c:739-751 既有齿「DISCARD_RECORD case → CloseAudioChannel 接线」与负锚「零 StopListening」需改写为「SendKidpalStoryDiscard 先于 CloseAudioChannel（帧序）」——齿的语义更新属本包验收面，不是绕齿。

## 验收种子（交 acceptance-author 冻结）

**内循环（server pytest + firmware host）**：
1. 累积态会话 + kidpal_story_discard 到达 → cancel 落、无归档、story_list total 不变、会话 pop、sweep stuck/auto-finish 不再命中；重复 discard / 非累积态 discard = 幂等 no-op。
2. 在途引导回合（mock LLM 延迟注入）+ discard/断连中到达 → 不注入 kidpal_speak、不 mark_guide_round、无会话壳复活。
3. 设备断连 + 引擎静默 → teardown 收尾限时内完成（gather 不悬挂回归齿）、cancel(reason="ws_disconnect") 落、腿与映射摘除。
4. 旧连接 zombie 会话 + 新连接在线 → sweep 引导/自动收尾帧只打旧会话归属腿，新连接腿零帧（跨连接泄漏回归齿）。
5. 固件 host 齿：start_organizing 消费 → StopListening 事件投递；KEY 路径幂等（已 Idle 时重复 settle no-op、不重发 listen stop）；DISCARD_RECORD → discard 帧先于 CloseAudioChannel 的帧序齿。

**真机四轮（OQ3 已裁 (a) 重定义）**：① 录音中丢弃：不播引导、不归档、列表条目数不增；② 正常说完收尾回列表：长按回首页即时可用；③ **重定义**：引导播报刚停、子态回聆听瞬间丢弃 → 在途/在播引导残余立即断、不播完；④ 回归：正常收尾归档链 / KEY 收尾路径 / S-F1.4 AC6 录音期提醒排队（settle 后 Idle 分支 RaiseReminderQueue 顺带把排队提醒弹出，对齐「会话结束立刻弹」）。

**红基线**：host/pytest 红 = 三症状复现齿（① 丢弃后 sweep 自动归档且 story_list total+1；② server 收尾后 ctx 未清且 show_home suppressed；③ 取消后在途引导照播 + zombie 会话跨连接推帧），先红后绿；真机红基线 = TF 卡 KPLOG003/004 既有取证时间线（debt source 字段）引用，不重复取证。

## 环境判注（triage 判注义务）

server 侧 pytest（teardown 取消 / 在途守卫 / discard 消费 / 腿归口，纯 asyncio + mock LLM 可快进计时）+ 固件 host 静态接线齿与 story_page.c 纯逻辑齿（既有 record_session_live / record_dialogue_guide 验收族同缝）可大部分闭环；但 mcp_server.cc / protocol.cc 属设备树编译单元、settle 后守卫放行与烧板复验必须真机——**按 full 环境备，设备编译+烧板轮不可省**（lite 快路径不适用）。

## 爆炸半径（architect 手工 grep inbound 计数；triage 时图工具未挂载，integrator 收口时以 detect_changes 实测校正）

`_story_guide_round` inbound=1（sweep :583）；`_story_auto_finish` inbound=1（:579）；`_story_finish_session` inbound≈4（泵路径 listen-stop :1007 / pending 接通 / 收尾词路径 / :619）；`_consume_kidpal_control` inbound=1（:997）；`_alert_legs` 消费点 4 处 + 登记/摘除 2 处；ws_gateway 收尾单点。固件 StoryDispatchAct case 3 / case 11 = switch 内部单调用族。远超 nano 阈值且命中 nano 排除清单多条（跨端、状态机、触 wire/契约、验收语义净增）——② Full 判档旁证。

## 逃生门核验（architect 结论 + reviewer 正式裁定待落）

疑似触契约 = 是（kidpal_story_discard 新增 → 预判 CONTRACT_CHANGE_REQUIRED；start_organizing「结束聆听」行为细化预判落既有行「设备离开录音聆听演出」语义包络内至多措辞对账；abort 复用排除论证须 reviewer 独立复核）→ 走 facePlan 顺序1 /arch-change，四分类正式裁定归 architect-reviewer，本门无权批准。

## 非目标（包外留根，plan 如实写）

- 语音「讲错啦」取消通道接线（双侧未接线，red-baseline story-p3 V31(b) RED 既有承载位，spine q4 L0 裁不另登）。
- 需求面零变更：复验③按 (a) 重定义，不启 /requirement-change。
- story.py 守卫支撑缝仅在实现期证明确需时才动（预判零改~极小）。
