# Review Packet · 261021-p3-record-cancel-lifecycle

- 日期：2026-10-21 · 作者：integrator（dev-loop 收口相位）
- changeId：261021-p3-record-cancel-lifecycle
- rigor=full · riskTier=high · ②单包 Full（human 触点① 2026-10-21 确认）· sourceType=regression
- 预期合并门：riskTier=high → supervisor 合并门升级 human（status=awaiting-human:merge-gate，单向守卫预期行为，taskpackage notes ⑤⑥）

## Change Size

- Files changed：9 tracked 修改 + 3 untracked 新增（`server/tests/test_story_cancel_lifecycle_units.py` builder 配套单测、`verifier-evidence/`、`verify-runs.jsonl`；scope-final / docs-delta / review-packet 三件收口制品随本轮 commit 入内）——实现与验收面全部在 changeManifest 清单内（manifest 含 `server/acceptance/**`、`server/tests/**`、`firmware/main/test/**` glob 与四件具名实现文件）；预判零改件（story.py / connection.py / application.cc）diff 实测零触。
- Net diff：约 +323 / −75 行（含 delivery 簿记；实现面 = front_app.py +124/−19 主体、mcp_server.cc +28、protocol.cc +20、protocol.h +17）。
- Scope：server（front_app.py F1 teardown 不悬挂 / F2 discard 消费分支 / F3①② 双点存续守卫 + F3⑤ 腿集会话归口三缝）+ firmware（mcp_server.cc DISCARD_RECORD 补发帧 + start_organizing 补 settle、protocol.h/.cc SendKidpalStoryDiscard 原语）+ 冻结验收三件（server 9 齿 + firmware 两件换锚/新增锚）+ builder 配套单测 11 齿。
- Split recommendation：不拆——server 与固件是同一取消/丢弃生命周期的两端（wire 一次往返），architect 评审原判、plan §影响面同判。

## Verification Evidence

### verifier 轮证据（事实件 = `verifier-evidence/rounds/rN.json`）

| 轮 | 相位 | 结果 | 要点 |
|---|---|---|---|
| r1–r2 | scoped | 红（元数据轮） | planner 元数据修正轮（identity/metadata 对齐），零实现红。 |
| r3 | scoped | **全绿（exit 0）** | 8/8 闸 PASS（taskpackage-required / change-manifest〔pre-cutoff 存量放行〕/ firmware-build / contract-lint / acceptance-coverage / mutation-check / docs-sync / security）。invocationId = 49a3b722-ae7a-4168-959e-c844135b56c0。 |
| r4 | **final** | **全绿（exit 0）** | 10/10 闸 PASS（r3 八闸 + unit + acceptance），零 notExecuted。invocationId = 5915a0c8-c3bb-4607-9b64-19dba531c8e1。**final fact `ok:true` 成立，closeout 承重判断以 r4 为准**；`current/verdict.json` 投影已同步（non-bearing compatibility projection）。 |

- **test-scope 清单落盘路径一行**：`delivery/taskpackages/261021-p3-record-cancel-lifecycle/scope-final.md`（final 相位影响面圈定清单，detect_changes 正常圈定零降级，server 25 件 + firmware 28 件）。
- `verifier-evidence-check.mjs` 复核：`{"ok":true,"gate":"verifier-evidence","factOk":true,"phase":"final","round":4}`——final evidence 有效，进入 closeout 成立。

### 测试实态（final r4 逐字 summary）

- unit 闸：`353 passed, 4 deselected, 52 subtests passed in 13.34s`（全协议 pytest；261019 接线轮锚 325 → 353，增长 = 本包冻结件 9 齿 + builder 配套单测 11 齿 + 既有件无回归；4 deselected = live 类 marker 机械排除）。
- acceptance 闸：`238 passed, 4 deselected, 33 subtests passed in 12.39s`（261020 锚 229 → 238，增长 = 本包冻结件 9 齿；既有齿零新增红）。
- firmware host：设计态红全转绿后复跑 = **222 Tests 3 Failures 0 Ignored**（exitCriteria ① 收敛达成：仅剩 p1_auxbar AB-A1/A2/A4 三条先存红，零新增红；红基线锚 222T6F → 222T3F，减红 3 条 = 本包设计态红全转绿）。
- 冻结件 9→0 收敛：RCL-A1..A4（server 9 TEST）+ RSL-A5 换锚 / RSL-A8 / RDL TEST 8（firmware host 3 齿）全绿。

### 结构性闸与豁免清单（suiteGates / structural）

- acceptance-coverage 闸输出：`structural: []` / `suiteGates: []`——五条验收 id（RCL-A1..A4 / RCL-F1）全部由冻结验收文件实断言覆盖，**无逐 id 豁免条目**。
- exitCriteria 中过程/证据型条目的承载（非 acceptance[]，无 mutation 义务）：①「既有套件零新增红」= suiteGate 语义，由 unit / acceptance / firmware host 复跑数字承载（见上，222T3F 先存红三锚逐一核对）；② live-manual 如实申报 = 真机轮承载（见「未覆盖面申报」）；③ mutationEvidence = mutation-check 闸 PASS；④「契约零改」= contract-lint 闸 PASS（r4）+ docs/contracts/** diff 实测为零。

### Security gate

PASS（r4）。两轴证据（`securityEvidence`，planner 回填 builder 回执）经本包 diff 与断言齿实地复核支撑：

1. **kidpal_story_discard 伪造面**：`_consume_kidpal_control` discard 分支 = `_get_story_acc().cancel(session_id, reason="discard")`——session_id 取自**本连接**（泵层权威），载荷 `obj.get("session_id")` 仅进审计 `log.info("story_discard corr_id=%s consumed=true cancelled=%s payload_session_id=%s")`、绝不作取消键；整条拦截 `return True` 不下透引擎；cancel 包 try/except 吞掉留痕不外抛（幂等 no-op 不给攻击者错误侧信道）。断言齿：RCL-A1 跨连接伪造 case（连接 B 携带连接 A 会话键 → A 素材完好仅 B 取消）+ builder 单测 `test_discard_connection_authority_ignores_payload_session_id` 双层钉住。实测复核：diff 中连接权威语义与注释、审计行逐条在位。
2. **teardown 与腿集归口资源释放面**：F1 teardown 改 `asyncio.wait FIRST_COMPLETED` + 对侧泵 cancel 回收，`finally` 必然执行 → `_unregister_alert_legs` + `_unregister_session_legs` + `acc.cancel(reason="ws_disconnect")` 消除「teardown 悬挂 → 映射泄漏」；`_session_legs_map` 生命周期与 `_alert_legs` 同源（连接登记 + finally 双摘除、按 device_ws 对象身份摘除）。误摘防护由 RCL-A4 must-stay-green 锚承载（提醒投递真跑仍达新连接两腿、sweep 后全局 `_alert_legs` 登记不被 purge）。已知边界（实现侧申报，见 Findings 第 1 条）：`_session_legs_map` 未设显式 cap——实际界 = 并发连接数（一连接一会话键、摘除与注册同生命周期），无无界增长路径。

### Mutation check

PASS（r4）。5 条 perAcceptance 证据（taskpackage.json `mutationEvidence`），**RCL-A1..A4 / RCL-F1 全 turnedRed=true 且还原复绿**，零豁免。要点：RCL-A3 变异（teardown 回退 gather）为 6s 预算耗尽型红（非挂死），还原后耗时回 0.2s 量级反证悬挂消失；RCL-F1 含附加变异（settle 半，摘 `app.StopListening()`）恰红 RDL TEST 8 一条（222T4F，无误伤）。**tooth 强度观察**（见 Findings 第 2 条）：RSL-A8 tooth1 文件计数锚可被注释满足。

### Docs sync

PASS（r3 / r4 双绿）。零新增 canonical 改动，据实申报见同目录 [docs-delta.md](docs-delta.md)（`docsNoChangeReason` 在册：契约 §四 / arch §5.2 场景④ / requirements v1.66 均随上游 `/arch-change` 批 landing，本包纯 additive 消费）。

## 语义决策登记（planner 裁定三条 + tooth 强度观察）

- **U2（采纳无参自由函数 `SendKidpalStoryDiscard`）**：`SendKidpalStoryFetch` 先例、application 无委托面——发送原语落 `protocol.h`/`protocol.cc` 自由函数（friend 声明 + `s_kidpal_active_` 活动实例形态），mcp_server.cc DISCARD_RECORD case 内回调上下文直接调用（与 INTERRUPT_GUIDE 的 AbortSpeaking 直接发送同款线程形态）。落地核对：protocol.h:137-147 原语注记 + protocol.cc:116-131 定义 + mcp_server.cc 调用点在位；wire 逐字 = 契约 §四 `{"session_id":"...","type":"kidpal_story_discard"}`。
- **U3（采纳 [SC-D] 空表回退全局 `_alert_legs`）**：退化/直调形态保 RDG-A6 must-stay-green；F1 后生产态僵尸会话不存在（teardown finally 必然摘腿）、空表误喷窗口不成立。落地核对：`_story_guide_round` / `_story_auto_finish` 均为 `_session_legs(sid) or list(_alert_legs)` 回退形态；RDG-A6 在 final r4 acceptance 闸内保绿（sweep 引导 + auto-finish 收尾单源链路零回归）。
- **U4（在途整理回合守卫判包外）**：plan F3 只覆盖引导回合；整理态设备按键全吞（story_page.c ORGANIZING 全手势 NONE 既有防线）、无 discard 源；加守卫须会话代次与 decisionCoverage F3「不引入会话代次」裁定冲突——按比例化原则判包外留根，不实现（包外申报见「未覆盖面申报」）。
- **tooth 强度观察（RSL-A8 tooth1）**：record_session_live.acceptance.c RSL-A8 tooth1 为跨文件计数锚（protocol.h 声明 / protocol.cc 定义 / mcp_server.cc 发送点 = 3 文件命中），mutation F1 实测注释自指试跑仅剩 RSL-A5 红——**文件计数锚可被注释满足**（tooth1 敏感度弱于语义断言）。非本包修订面（冻结件、本包零改），转 acceptance-author 后续 tighten 参考（如改为「每个文件命中非注释行」或直接断言 wire 子串在 protocol.cc 非注释区）。

## Builder 实现摘要（对照 plan 四件修复）

- **F1 teardown 悬挂修复（症状③⑤直接机制）**：`ws_gateway` 收尾 `await asyncio.gather(pump_up, pump_dn)` → `asyncio.wait(FIRST_COMPLETED)` 任一泵先结束即收尾 + 对侧泵 cancel 后限时回收 + 泵异常留痕（`ws_pump_ended` warning）；`finally` 的 cancel + 双腿摘除 + 会话腿映射摘除即时可达——HWV-A11 未停录断开不泄漏意图恢复。
- **F2 显式丢弃通知（Design B）**：设备侧 DISCARD_RECORD case `SendKidpalStoryDiscard()` 先于 `CloseAudioChannel()`（同一 TCP 有序，front 泵先消费 discard 再处理断连；关通道保留 = cancel-on-disconnect 兜底）；网关 `_consume_kidpal_control` 新增 discard 分支（连接权威 cancel reason="discard" + 拦截不下透 + 幂等 + 审计一行）。
- **F3 在途回合存续守卫 + 腿集会话归口（症状②③残余）**：`_story_guide_round` 双点守卫（入口 `build_organizing_input` 返 None 直接返回零 LLM / LLM 返回后复查 `routing(sid)=="accumulate"` 否则丢弃产出）；`mark_guide_round` 只在累积态触达（零会话壳复活）；`_session_legs_map` 三缝函数（`_register_session_legs` / `_unregister_session_legs` / `_session_legs`，[SC-D] 名字冻结采纳）ws_gateway 同生命周期登记摘除；引导注入与自动收尾改会话归口腿（空表回退全局腿）；提醒与课表两消费点全局腿不动。
- **F4 start_organizing 消费补 settle（症状④）**：mcp_server.cc case 3 消费块补 `app.StopListening()`（事件位投递，KSPV2_ACT_STOP_RECORD 同款形态）→ HandleStopListeningEvent → Idle 清位链既有完整接管（application.cc 预判零改达成，diff 实测零触）；KEY 路径幂等由既有 `state != Listening` no-op 分支承载。

## 探针重钉闭环（acceptance-author 冻结件修订）

冻结件 `test_record_cancel_lifecycle.acceptance.py` 2026-10-21 探针重钉（REVISION LOG 在盘，授权 = planner 转达）：`_display_payloads` 重建器把 `kidpal/display` 的 name 过滤误下探到 `params.arguments` 层（wire 上 name 住 params 层）——探针层级拷改缺陷，致 A1「零 story_list 重推」负向断言空过、A4 定向正向断言任何实现都不可满足。修法 = 过滤位上提到 params 层（与姊妹冻结件 test_record_dialogue_guide.acceptance.py 同层形态对齐），**全部断言语义零弱化（修后断言变真、变强）**；红基线数字随动重录。本包实地核对：REVISION LOG 条目与过滤位修正均在位，重钉后 RCL-A1/A4 转绿且 mutation 全 turnedRed（重钉后断言对实现敏感实测）。

## 架构 / 依赖卫生检查

- **跨子系统事件流核对**：设备 BOOT 丢弃 → `KSPV2_ACT_DISCARD_RECORD` → `SendKidpalStoryDiscard()`（先）→ `CloseAudioChannel()`（后）→ 网关 pump_dn 收到 `{"session_id","type":"kidpal_story_discard"}` → `_consume_kidpal_control` 整条消费拦截（不下透引擎）→ 连接权威 cancel——wire 形态、发送时机、消费语义三段逐字对齐契约 §四 LANDED 行（实地比对 interface-contract.md:277）。F4 链：start_organizing 消费 → `app.StopListening()` 事件位 → HandleStopListeningEvent 既有 Idle 清位链（application.cc 零改）。无绕过 contracts 的本地类型、无跨层调用新增（protocol 自由函数 = 既有 SendKidpalStoryFetch/SendKidpalStoryDelete 同层形态）。
- **重复实现排查**：`_session_legs_map` 三缝与 `_alert_legs` 登记摘除同位调用、共享生命周期但职责分轴（连接级投递面 vs 会话级归口面）——非重复实现；`_speak_to_engine_legs` legs 形参 additive、缺省零偏移。
- **依赖卫生**：diff 实测零新增第三方 import（server 侧改动只用既有 asyncio/logging；固件侧无新组件）；package/requirements 零 diff。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | firmware/main/test/record_session_live.acceptance.c RSL-A8 tooth1 | 跨文件计数锚（3 文件命中 SendKidpalStoryDiscard）可被注释满足——mutation F1 注释自指试跑 222T4F 仅 RSL-A5 红，tooth1 对「注释满足」不敏感 | changed-code hunk（RSL-A8 齿体）+ mutationEvidence RCL-F1 note（注释自指实测留痕） | no-trigger（本包转 acceptance-author 后续 tighten 参考，见「语义决策登记」tooth 强度观察行；冻结件修订归后续冻结件修订通道） |
| FYI | judgement-call | server/gateway/front/front_app.py `_session_legs_map` | 无显式 cap（对照 `_alert_legs` 有 ALERT_LEG_CAP=8）——实际界 = 并发连接数（一连接一会话键、finally 双摘除同生命周期），无无界增长路径 | changed-code hunk（`_unregister_session_legs` 按 device_ws 身份摘除）+ securityEvidence 已知边界申报 | no-trigger；reviewer 若要求显式上界属实现侧小改可留 fixer，当前比例化接受 |
| FYI | judgement-call | server/gateway/front/front_app.py `_story_guide_round` / `_story_auto_finish` | 映射缺席时回退全局 `_alert_legs`（离线/直调退化形态）——回退路径与新归口路径并存，读者需读注释理解何时走哪条 | changed-code hunk（`_session_legs(sid) or list(_alert_legs)`）+ 语义决策登记 U3（planner 裁定，RDG-A6 must-stay-green 依据） | 无——已裁定并注释承载 |

无 BLOCKING / 无 hard-violation——结构闸 PASS/FAIL 不复述为 finding（gate 全绿仍完整执行了五轴语义 review）。

## Five-Axis Review

1. **Correctness**：三症状（teardown 悬挂 / zombie 跨重连泄漏 / 在途引导残余 + 设备不回 Idle）逐条有冻结断言承载（RCL-A1..A4 全绿）；连接权威 / 幂等 no-op / 不误杀正常收尾（finish 先 cancel 后演出）三类边界由 RCL-A1 幂等与伪造 case + RCL-A2 复查锚 + 缝契约注记钉住；U4 判包外的整理态残余由既有 ORGANIZING 全手势 NONE 防线承载（真机轮复验项）。时钟注入纪律维持（`_story_guide_round` 不落墙钟）。
2. **Test quality**：冻结断言钉真实行为而非实现细节——mock LLM 可门控阻塞（在途竞态真驱）、spy 包装真实 cancel（真实取消语义真跑）、fake ws 双桩 + 6s 预算耗尽型红（悬挂可机判）；builder 配套单测覆盖分支/边界/错误路径 11 齿（非 dict 载荷不消费、未知会话零壳、跨连接隔离摘除等）；mutation 5/5 turnedRed 且还原复绿、零豁免——齿敏感面经变异实测（RSL-A8 tooth1 弱点已如实申报）。
3. **Contract/boundary safety**：零契约漂移——契约 §四 kidpal_story_discard 为上游批 LANDED wire，本包纯 additive 消费（固件 wire 拼串逐字对齐 + 网关消费分支语义五要素全落）；`docs/contracts/**` 零触碰（docs-delta 申报 + contract-lint PASS）；协议原语沿 `SendKidpalStoryFetch`/`SendKidpalStoryDelete` 既有形态（无新抽象层）；预判零改件（story.py / connection.py / application.cc）diff 零触实测。
4. **Operability/security**：连接权威取消防跨会话伪造（载荷键仅审计）；discard 消费异常吞掉留痕（幂等不外抛，不给攻击者错误侧信道）；审计一行含 corr_id + cancelled 结果 + payload_session_id 对照（事后可追伪造尝试）；teardown 泵异常留痕（`ws_pump_ended` warning 可检索）；离线降级静默 no-op 不产半帧（协议未就绪形态与 fetch 同款）。
5. **Maintainability/simplicity**：改动局部性良好——固件全 additive（一处调用 + 一对原语声明定义 + 一处 settle），server 集中 front_app.py 单件；`_speak_to_engine_legs` 以可选形参扩展而非复制函数；过度设计排查：无 `delete`/`stdlib`/`native` 命中；`yagni` 一条 FYI（全局腿回退路径并存，见 Findings 第 3 条，有裁定依据非过度设计）。

## 未覆盖面申报（对照 acceptance[] + 契约条目，逐条一行）

- discard 帧序端到端（真机 TCP 帧序观测：discard 帧先于断连到达 front）：无自动断言（host 不可达，RSL-A5 host 齿钉静态帧序）——现状 = 代码锚全在位（发送时机 + 同 TCP 有序依据注记）；去向 = 真机轮，status=awaiting-human:real-device。
- 真机取消链四项（BOOT 丢弃不播引导不归档不增条目 / 正常收尾后列表页长按即时可用〔show_home 守卫放行〕/ 复验③重定义(a) 引导播报刚停瞬间丢弃残余立断 / 回归：正常归档链 + KEY 收尾 + S-F1.4-AC6 录音期提醒排队弹出）：无自动断言（live-manual structural 口径）——去向 = 真机轮，烧板轮不可省（mcp_server.cc / protocol.cc 设备编译单元），status=awaiting-human:real-device。
- 语音「讲错啦」取消通道接线：双侧未接线（red-baseline story-p3 V31(b) RED 既有承载位）——plan §非目标包外留根（spine q4 L0 裁不另登），去向 = 后续包。
- 在途整理回合守卫（U4 判包外）：无实现无断言——现状 = 整理态设备按键全吞既有防线；去向 = decisionCoverage F3 裁定留根，若真机轮实证整理态出现 discard 源再立包。
- 其余逐项排查：**none**——RCL-A1..A4 / RCL-F1 全部有冻结断言承载，契约 §四 kidpal_story_discard 行有 RCL-A1（网关半）+ RCL-F1①②（设备半）两侧断言，零其它缺口。

## Live-manual 如实申报（不造 offline 假齿）

- discard 帧真机端到端 + 真机取消链四项：到门即停 status=awaiting-human:real-device（烧板轮不可省）。
- offline 通过面不含任何 live 假齿：冻结件 GAP 清单明示 live/真机面申报口径（261019 同款）。

## Residual Risk

decisionCoverage 四条 `landsIn` 全部为 `acceptance:RCL-*`，**零 `dropped` 条目**——无 `landsIn: dropped && reason: risk-accepted*` 承载项需列（防「没写」与「没有」混同，明写：没有）。

- Known limitation：真机运行时行为（帧序端到端 / show_home 放行观感 / 复验③(a) 时序 / AC6 提醒弹出时机）未经实证——host 不可机判面，全部归真机轮。
- Known limitation（实现侧申报）：`_session_legs_map` 无显式 cap（实际界 = 并发连接数，见 Findings 第 1 条）。
- Follow-up recommended：① 真机轮 + 烧板（human 持板，awaiting-human:real-device）；② RSL-A8 tooth1 tighten（acceptance-author 后续参考）；③ 语音「讲错啦」通道接线（后续包）。
- Human decision needed：supervisor 合并门（riskTier=high 升级 human 预期路径）+ 真机轮持板。

## 可回滚路径

- 本包全部改动以单 commit 落 dev/pod-m2（不 push、不碰 main）；回滚 = 该 commit `git revert`（零跨包耦合——本包无被后续包依赖的头；上游 arch/契约批已合 main，revert 实现面不产生契约漂移——契约行描述的能力回到「省略整条消息 = cancel-on-disconnect 兜底」，向后兼容语义契约已写明）。
- 关键面可分解回滚：F2 设备半（protocol 原语 + mcp 调用点）revert 即回纯 cancel-on-disconnect 路径；F1/F3 server 半 revert 即回 gather 悬挂语义（RCL-A3 冻结齿回红——须连带 revert 冻结件，回滚脚本按 commit 逆序执行即可）。
- 数据面零迁移：无 schema/存储格式变更，回滚无数据恢复义务。

## Simplification Debt（`minimal:` 标记台账）

本包零新增 `minimal:` 标记（实现面 diff 实测 `grep -E "minimal:"` 零命中）；台账无条目、upgrade trigger 全部 no-trigger。

## 里程碑判定

`delivery/milestones/milestones.jsonl` 无阶段跳变——本包属 M4-star-notebook（state=in_delivery）K 线内中段修复（TECH-261021 三症状同根修复，不翻 M4 账）；未改 milestones.jsonl、未跑 delivery-status.mjs（无 projection 变更义务，判定与 261019 先例同口径）。

## Step 9b 影响面记账（advisory）

detect_changes（Pod 专属图索引 C-AI-KidPal-pod-m2，base=main，merge_base=be93ca5e，inbound depth=2）：changed_files 16 / seed_symbols 217 / impacted_total 22——改动只影响预期符号面（front_app.py sweep/pump/reminder 轮询相邻、mcp_server HandleStoryKey/StoryTickTimerCb、protocol 链经 Application::Initialize 进设备编译面、acceptance 相邻件 m4k1/m4k3/rdg/story_fetch），无预期外符号命中。per-Pod 索引可用（launch 面必办项在位）→ 正常圈定零降级，无 re-index 需求申报。范围圈定明细见 scope-final.md §1–§3；本步只记账，范围判定归 Step 4（final r4 已按 scope-file 跑毕全绿）。

## 集成执行回报

无——本包单包非拆包，未触发 merge-driver 冲突消解路径（pod-coordination.md §10a/§10c 前置不成立，本节按缺省申报零事件）。

## 等待裁断

- final bearing verify r4 全绿 + closeout 制品齐 → 交 planner 跑 `supervisor-verdict-gate.mjs` 机械闸；riskTier=high → supervisor 合并门升级 human（awaiting-human:merge-gate）。
- 真机轮：awaiting-human:real-device（烧板轮不可省，真机取消链四项 + discard 帧序端到端观测）。

## 真机轮追补（2026-10-22，planner 记；人批合并前补账）

真机轮 + 线上实测揪出两处后端缺陷，同包 fixer 双轮收口（verify r5/r6 scoped 绿）：

| 轮 | 缺陷（真机群证） | 修法 | commit / 证据 |
|---|---|---|---|
| R6 | 整理失败路径困死 ORGANIZING 非交互页：R1-N4 失败分支零页面退出帧 + [R2] 复位缝只认 RECORDING（KPLOG005 1698525-1716875：安抚句播完 Idle 后页面滞留 substate=6，重启为唯一出口） | `_story_finish_session` 失败分支同拍逐设备腿补 `_push_story_list` 退出帧（total=0/items=[] 零虚增）；伴生 M4K1 SAO-A1 负锚换代（「严格零帧」→「恰一帧空列表退出帧」= 更强钉法，REVISION LOG 2026-10-22 条） | 4068dc6c；r5.json；真机复验 `story_list_pushed=1` 生效 |
| R7 | 整理回合 LLM 超时结构性不够：OpenClaw embedded run prep 实测 7.05s（bootstrap-context ~6.5s 固定税，2 核 2GB ECS + 600MB rss Node 进程）+ 整理长输入长输出 → 原缺省 10s 在模型吐 ~1.5s 时掐断（ClientDisconnectError，每次必现） | `_ORGANIZE_LLM_TIMEOUT_S = 60.0` 只挂 organize 调用点；引导回合维持 10s 不动 | 2160fef7；r6.json |

- 部署史：front_app.py 三次人批部署（md5 9663f635 → 3f27a228 → e3584c59，各次旧件备份在 ECS `.bak*`）；设备固件全量烧录一次（启动判定 0 RE-ENUM）。
- **人批合并前真机终态：整理成功出稿归档、设备自动出整合页回列表、无困死（human 2026-10-22 确认「这次没出问题」）。**
- 遗留登记：OpenClaw prep 7s 固定税 + 内存压力增长趋势（rss 600MB 超阈 125% 一次）→ 归 arch/治理面 debt，不塞本包。
