# Review Packet · 260915-m1-k3-voice-round-ui（语音轮三态演出）

> integrator 收口，面向 supervisor 合并门。verifier final round fact = `delivery/taskpackages/260915-m1-k3-voice-round-ui/verifier-evidence/rounds/r5.json`（2026-09-16T16:22Z，phase=final，round 5，invocationId `b6c6def3-6173-41cb-95fd-14cc35e0d9a3`，9 闸全绿，ok:true，observedGateExit 0）；`verifier-evidence-check.mjs` PASS（factOk:true，输出 verbatim 见下）。`delivery/taskpackages/current/verdict.json` = non-bearing compatibility projection（verify.mjs fs-write 产出，与 r5 fact 同源同内容）。Pod worktree 常驻分支 dev/260915-m1-k1a-key-layer-dispatch（树池复用第 4 棒，K1a/N1b/K2 同支）。

## Change Size

- Files changed: tracked 11 件 + untracked 7 件 = 18 件（`git diff --cached --stat` 终值见「commit 交付」段）
- Net diff: firmware 代码面 tracked +376/-159；实现本体 = `voice_round.{h,c}` 新增（场景决策表 + 逐句回显纯逻辑单源）+ `lcd_display.cc` +101（回显区/问答框容器 + kidpal_voice_apply_look 五态消费）+ `application.cc` +16（stt 分支同刻消费 kidpal_voice_round_scene）+ `session_state_machine.cc` +17/-9（注释对齐权威枚举名 + 齿面）+ 两个 CMakeLists 接线；冻结验收件 `voice_round_ui.acceptance.c` 新增 1076 行（冻结即基线，未动）
- Scope: firmware display 语音轮三态演出补齐——思考页接线（真机首次可弹）+ 聆听回显区 + 问答框长文滚动 + 打断衔接 + 四态版式骨架（骨架 owner = 本包）；server/ 零触碰、`docs/contracts/**` 零触碰、`docs/requirements.md` 零触碰
- Split recommendation: 不拆——单包单面，mergeOrder 25，dependsOn [k1a, k2]（均已 merged）

## Verification Evidence

### 红基线 → 终跑对比

| 面 | 红基线（2026-09-16 freeze 相位） | 终跑（2026-09-17 integrator 复核） |
|---|---|---|
| host runner 全量 | **18 Tests / 0 Failures / 0 Ignored OK**，exit 0（n1b 期锚 13 + k2 新齿 5 = 本包有效基线；在册既存红清单 = 空，无豁免件；冻结记录 = [red-baseline.md](red-baseline.md)） | **23 Tests / 0 Failures / 0 Ignored OK**，exit 0（18 基线 + 本包 5 新 test，零新增红；VQ-A8 达标） |
| server 面 | 0 collected（`--ignore=server/acceptance/archive` 口径） | **no tests ran in 0.01s**（同口径，0 collected 一致，exit 0） |
| 义务红 | test 1/2/3 + test 5 tooth 1 设计态红（weak 哨兵 -2 / needle 零命中 / 注册序缺） | 全部转绿（builder 落地 + 接线后）；test 4 消费在世件零回归守卫本性，两端均绿 |

义务红对照（voice_round_ui.acceptance.c 文件尾「VQ-A8 终跑判读」段）：本包义务红在终跑全数转绿，红基线判读缺口 1 处（scan-miss，见 Findings NIT-1）已由 builder 以注释对齐权威枚举名修正（零行为变更）。

### 逐闸证据表（scoped r4 + final r5，均 ok:true / exit 0）

| 闸 | kind | scoped r4 | final r5 | 要点 |
|---|---|---|---|---|
| taskpackage-required | governance | PASS (202ms) | PASS (224ms) | current/taskpackage.json 指向本包 |
| firmware-build | structural | PASS (9.3s) | PASS (6.8s) | 真编译；xiaozhi.bin 0x30afa0 / app 分区 0x370000，**11% 余量** |
| contract-lint | structural | PASS (149ms) | PASS (152ms) | docs/contracts/interface-contract.md 只读面零触碰 |
| acceptance-coverage | functional | PASS (166ms) | PASS (166ms) | VQ-A1~A8 全覆盖；structural = [VQ-A7]；验收文件 1 件 |
| mutation-check | functional | PASS (161ms) | PASS (153ms) | 7 functional + 1 structural（VQ-A7），逐条 mutationEvidence 在册 |
| docs-sync | governance | PASS (160ms) | PASS (161ms) | docsSyncRequired=false，声明与 diff 一致（见 docs-delta.md） |
| security | security | PASS (161ms) | PASS (145ms) | securityReviewRequired=false |
| unit | functional | —（scoped 不跑） | PASS (3.5s, full) | **37 passed in 0.27s**，structuralCollected=37，liveDeselected=0 |
| acceptance | functional | —（scoped 不跑） | PASS (231ms, full) | oracle-pass；firmwareAcceptanceFiles=18 / serverAcceptanceFiles=0；on-target 验收归 acceptance 相位人工/台架（本包承载 = VQ-A7 真机轮，pending 见下） |

### verifier-evidence-check 机械核验输出（verbatim）

```
{"ok":true,"gate":"verifier-evidence","evidenceRef":"delivery/taskpackages/260915-m1-k3-voice-round-ui/verifier-evidence/rounds/r5.json","factOk":true,"observedGateExit":0,"phase":"final","round":5}
```
exit 0。

### 全量验证复核（integrator 现跑，2026-09-17）

1. host runner（WSL，仓根 cwd）：
   - build：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` → `ninja: no work to do. Done`（构建缓存与源面一致）；
   - run：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf` → 末三行 verbatim：`23 Tests 0 Failures 0 Ignored` / `OK`，exit 0。与 verifier final 一致。
2. server 面：`python -m pytest -q --import-mode=importlib -m "not live" --ignore=server/acceptance/archive server/acceptance` → verbatim：`no tests ran in 0.01s`，exit 0（0 collected，与 final unit 闸 37 passed 口径互证：server 现役面空态 + 结构性 pytest 37 件全绿）。

### Mutation 摘要（taskpackage mutationEvidence.perAcceptance，7 变异 / 7 acceptanceId 全 sensitive）

| acceptanceId | 变异 | counterEvidence | 结果 |
|---|---|---|---|
| VQ-A1 | kvr_scene_stt_auto_row_mute_stub | test 1 tooth 1, expect KVST_SCENE_THINK | failed as expected |
| VQ-A2 | kvr_echo_ring_evict_newest_stub | test 2 tooth 3, expect oldest evicted | failed as expected |
| VQ-A3 | lcd_cc_answer_route_mute_stub | test 3 tooth 5, expect region contains g_kidpal_voice_answer_box | failed as expected |
| VQ-A4 | overlay_boot_click_row_flip_stub | test 4 tooth 1, expect ==4 and !=PAGE_NEXT | failed as expected |
| VQ-A5 | listen_bubble_glyph_drift_stub | test 4 tooth 4, expect verbatim 我在听！ | failed as expected |
| VQ-A6 | ses_cc_comfort_flag_mute_stub | test 4 tooth 6, expect SES_STATE_SPEAKING + SES_FLAG_COMFORT_SPEECH | failed as expected |
| VQ-A8 | host_srcs_line_omit_stub | test 5 tooth 1, expect voice_round_ui.acceptance.c registered before smoke | failed as expected |

（VQ-A7 为 structural 对象条目，不由 mutation 承载——由 test 3 真机前置接缝断言 + 真机轮 human 持板承载。）

## Demo 对账行（锚 3：roadmap.md 效果段 ↔ 证据）

roadmap.md §260915-m1-k3-voice-round-ui「完成后人能看到什么」逐句对账：

| 效果句 | host 齿 / 结构断言承载 | 只能真机确认的部分 |
|---|---|---|
| 孩子说完一句，思考页（pose-think + 飘问号）与状态切换**同一时刻**弹出 | test 1 tooth 1（LISTEN_AUTO+STT_FINAL→KVST_SCENE_THINK 决策行）+ test 3 tooth 2（`kidpal_voice_apply_look(KVST_SCENE_THINK)` 活侧首接）+ test 3 tooth 3（application.cc stt 分支同刻消费单源）+ test 4 tooth 6（THINK 入口 = STT_FINAL 同刻，非回复驱动） | pose-think 资产与飘问号的**视觉观感**、弹出帧率/时序手感 |
| 真机首次可弹 | test 3 真机前置接缝断言全绿（思考页活侧可弹的接线在世） | 「真机可弹」本身 = VQ-A7 human 持板 |
| 说话逐句回显在「我在听！」下方（超屏滚动） | test 2 全齿（逐句入列/行序/容量滚出 KVR_MAX_LINES=6/防御）+ test 3 tooth 4（回显区容器 `LV_OBJ_FLAG_SCROLLABLE` + `kidpal_voice_echo_push` 消费 + user 路由）+ test 4 tooth 4（「我在听！」verbatim） | 滚动手感与超屏滚出**观感**（LVGL 渲染层） |
| 答复长文在问答框滚动看完（回引 NFR-READ-1） | test 3 tooth 5（问答框容器可滚动 + assistant 全文路由——非一行字幕） | 字号下限观感（NFR-READ-1）与长文滚动**观感** |
| 播报中按 KEY 立即停播转聆听 | test 1 SPEAKING+ABORT→LISTEN 衔接行 + test 4 tooth 1/2（key_route_layer 覆盖层列 KD-A1/A3 冻结值，!=PAGE_NEXT）+ test 4 tooth 3（板级 ToggleChatState 消费链 + AbortSpeaking 在世） | 「100% 可靠」打断**手感**（真机按键时序） |
| 对话主路径不再「黑箱等待」（四态版式骨架统一） | test 3（骨架对象：左小伴 `kidpal_voice_state_look` 单源 + 右内容区布线）+ test 4 tooth 4/5（kidpal_voice_states 五成员闭集逐字零偏移） | 版式几何观感（归设计稿/真机轮） |

**对账结论**：host 面一致——效果段逐句的机制/接线面全部由 host 齿与结构断言承载且终跑全绿，无一句在 host 面落空；**观感/手感面（三态视觉、滚动观感、打断手感）只能真机确认**。模拟器截图与真机轮证据 = **pending**：VQ-A7 真机确认轮已按 human 2026-09-16 晚交接 deferred 出 Pod（pod-events eventId `1dc757f9-8439-46b6-aa73-234ffc415654`，human-decision `device-round-deferred-out-of-pod`——Pod 不等真机证据，走完 supervisor 合并门 + 本地 main 合并后向主线程汇报，主线程以 kidpal-flash-device 烧板，human 明早持板测试）。无真机证据，不虚构一致性结论：**host 面一致，真机面待 human 持板确认**。

## 验收对照表（VQ-A1~A8 / VQ-E1~E4）

| 条目 | 状态 | 证据 |
|---|---|---|
| VQ-A1 思考页接线 + 同刻弹出 | PASS（机制面） | host 齿 test 1/3/4（coverage map 行 127-129）；视觉观感归真机 |
| VQ-A2 聆听回显区 | PASS（机制面） | host 齿 test 2/3/4；滚动手感归真机 |
| VQ-A3 问答框长文滚动 | PASS（结构面） | host 齿 test 3 tooth 5；屏高滚动观感归真机（齿注记明示） |
| VQ-A4 打断手感 | PASS（衔接面） | host 齿 test 1/4；100% 可靠手感归真机 |
| VQ-A5 四态版式骨架 | PASS（闭集面） | host 齿 test 3/4（闭集五成员逐字零偏移 + 骨架单源）；几何观感归真机 |
| VQ-A6 超时安抚钉点 | PASS | host 齿 test 1 tooth（计时起点锚）+ test 4 tooth 6（THINKING_TIMEOUT_4S→SPEAKING+COMFORT 冻结边在世） |
| VQ-A7 真机确认轮（structural） | **PENDING** | deferred 出 Pod（pod-events 1dc757f9，human 2026-09-16 拍板）；真机前置接缝断言已全绿（test 3），证据落本目录待 human 持板 |
| VQ-A8 host 终跑 | PASS | 23/0 vs 有效基线 18/0 零新增红；red-baseline.md + integrator 复跑一致 |
| VQ-E1 范围纪律（60s timer deferred） | PASS | host 齿 test 5 tooth 2 canary（SES_EVT_SESSION_TIMEOUT_60S 恰两文件）绿；语音喊停 AEC 零触碰（齿面无接缝，文件尾范围注记） |
| VQ-E2 范围纪律（课表数据侧归 M3） | PASS | assistant 路由齿只锁渲染面；范围声明见文件尾注记 |
| VQ-E3 需求面零修订 | PASS | host 齿 test 5 tooth 3（requirements/契约 canary）绿 + git status 实证 docs/ 零触碰 |
| VQ-E4 四态骨架 owner 声明 | PASS | 骨架由本包建立并钉死（test 3/4）；k4 复用登记 crossPackageBehaviors |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| 无 BLOCKING | — | — | — | — | — |
| NIT | judgement-call | governance | acceptance-author 红基线判读缺口：VQ-E1 test 5 tooth 2 canary 期望 SES_EVT_SESSION_TIMEOUT_60S 恰两文件，但 session_state_machine.cc 原注释用缩写 SESSION_TIMEOUT_60S 使 count=1 被误判绿、义务红未列（builder 以注释对齐权威枚举名修正，零行为变更） | pod-events friction eventId `4940ea5a`（planner 登记 scan-miss）+ changed-code hunk（session_state_machine.cc 注释行）+ heuristic：canary 齿被扫 token 须先核在世实况 | 流程教训入 pod-events 在案；后续包冻结 canary 齿时先核被扫 token 在世实况（无独立 ARQ，不阻断） |
| NIT | judgement-call | governance | uiux-author agent 编制缺位（.claude/agents/ 无 uiux-author.md，Agent 派发必 fail），L2 判定 brief 翻译由 planner 直翻落 taskpackage uiux block | pod-events friction eventId `65f183c6`（planner 登记 degraded）+ taskpackage `uiuxAuthorNote` | 编制问题归治理批裁量，本包零新增设计决策（interaction brief 由冻结 UIUX-DESIGN-BRIEF v4 §5 承载），不阻断 |
| NIT | judgement-call | governance | worktree 图索引滞后：trace_path 抽查 kidpal_voice_state_look 入度未反映本包新增 lcd_display.cc 消费点（后台增量刷新未及新文件），以 git diff grep 实证补位 | changed-code grep（lcd_display.cc:1225 消费 kidpal_voice_state_look 单源）+ detect_changes: N/A (worktree) | 无需动作；随树索引自刷新 |

## Five-Axis Review

1. **Correctness**：场景决策表（kidpal_voice_round_scene）与回显机制（kidpal_voice_echo_push）为纯逻辑单源、无芯片依赖，设备侧零重算；活侧渲染路径经 `kidpal_voice_apply_look` 消费 `kidpal_voice_state_look` 单源取五态 look，不立第二真相源；stt 分支同刻消费决策表（状态切换与弹出同刻，无分离窗口）；lvgl 锁 acquire 超时有 ESP_LOGE 路径。host 5 新 test 全绿 + mutation 7/7 sensitive 背书。
2. **Test quality**：冻结验收件 1076 行，weak 哨兵 KVR_UNIMPLEMENTED(-2) 保证未接线即真实行为红（非链接错误）；决策表行为半 + 活侧接线结构半双承载；MUST-STAY-GREEN 齿（test 4 消费在世件零回归守卫，本性非假绿、齿注记自证）；闭集逐字零偏移齿；注册序守卫齿（smoke 末位最先执行）；真机前置接缝断言独立成齿（VQ-A7 structural）。
3. **Contract/boundary safety**：`docs/contracts/**` 零触碰（contract-lint 绿）；`docs/requirements.md` 零触碰（VQ-E3 canary 齿 + git status 实证）；键位语义只**消费** k1a 冻结裁决（key_route_layer 覆盖层列 KD-A1/A3 冻结值，KLD_OVERLAY_CLOSE_FROZEN=4 钉值 + !=PAGE_NEXT 判别式）；骨架 owner 显式声明（k4 复用登记，防两包各建骨架漂移）；60s timer / AEC / 课表数据侧范围纪律三条零触碰。
4. **Operability/security**：无新依赖（voice_round.{h,c} 纯 C、host/device 两侧同源照 kidpal_voice_states.c 先例）；无 secret/网络面；security 闸绿 securityReviewRequired=false；firmware-build 资源余量 11%（0x65060 free / 0x370000）。
5. **Maintainability/simplicity**：voice_round 模块照 k2 keybar.c 先例做纯逻辑/渲染分离；deletion test：删掉 voice_round.c 则场景决策与回显语义散回 application.cc / lcd_display.cc 多调用点 = 深模组信号（留）；结构齿以 needle 扫描钉接线，接缝漂移即红；无重复实现、无过早抽象。

## Simplification Debt

- 仓库无 `minimal:` 标记惯例命中；本包无降级交付 tradeoff 挂账（k2 期键帽字库挂账不涉本包面）。
- 台账：空。

## Residual Risk

- Known limitation: 三态视觉/滚动/打断观感与手感未经真机确认（VQ-A7 pending，deferred 出 Pod）；真机前置接缝断言全绿但观感面（LVGL 渲染层、真机按键时序）本包证据面不可达。
- Human decision needed: **human 明早持板测试**（pod-events 1dc757f9 交接链：主线程 kidpal-flash-device 烧板 → human 持板核 VQ-A7 四项：① 思考页首次可弹同刻 ② 回显三手感 ③ 长文滚动观感 ④ 打断手感）；证据落本目录。
- decisionCoverage 审计：5 条 decision 全部 landsIn acceptance/exitCriteria，**无 `landsIn: dropped` 条目**，无 risk-accepted 项需列（60s timer 为 deferred 非 dropped，landsIn exitCriteria:VQ-E1 在齿）。

## S2 exploratory scan

TaskPackage 未声明 `s2ExploratoryRequired=true`，本包不执行 S2 official scan（exploratory evidence 非本包义务）。

## UI design-review 证据（uiuxRequired=true, uiuxLevel=2, 纯文本）

- viewport coverage summary: 三态（聆听/思考/播报）各态 = 左小伴（kidpal_voice_state_look 单源 pose/气泡）+ 右内容区（回显区 0.7×HOR_RES 顶部对齐 / 问答框可滚动容器）同骨架；骨架对象布线由 host 齿 test 3 结构断言承载。
- a11y summary: 字号下限回引 NFR-READ-1（齿面锁非一行字幕 + 全文路由；像素级观感归真机轮）；键帽文案走 kidpal_voice_states 冻结闭集（五成员逐字零偏移齿承载）。
- design-drift summary（brief↔实现）: UIUX-DESIGN-BRIEF v4 §5 语音轮行 + requirements §2.2 状态表为冻结权威；实现零新增设计决策（taskpackage uiuxAuthorNote）；「我在听！」等文案 verbatim 齿锁定；无 drift 发现。
- external artifact reference: 无（bootstrap 期不落仓内截图/HTML）。
- residual risk: 版式几何与观感真机待核（VQ-A7 pending）。
- severity 解读: 无 UI BLOCKING（机制面齿全绿）；观感项并入 VQ-A7 human 持板清单，不新增 gate。

## 影响面核对（11b，worktree）

- detect_changes: **N/A (worktree)**（Pod worktree diff 对索引不可见，如实标注）；替代抽查：
  - `git diff --stat` 落在预期闭集（11 tracked 件，全部为本包声明面：firmware 5 件 + host CMakeLists + delivery 治理面 5 件），无计划外文件；
  - trace_path 抽查 `kidpal_voice_state_look`（kidpal_voice_states.c 强符号）入度 = 1（kidpal_keybar_slots，k2 既有消费者）——**图索引未反映本包新增消费点**（索引滞后，NIT-3），以 grep 实证补位：lcd_display.cc:1225 消费、application.cc:499 消费 kidpal_voice_round_scene、lcd_display.cc:1112 消费 kidpal_voice_echo_push，均在本包 diff 内、无计划外调用方；
  - crossEnd 三件（lcd_display.cc / kidpal_voice_states.c / reminder_page.c）: lcd_display.cc 为本包改动面，kidpal_voice_states.c / reminder_page.c **零触碰**（git diff 实证不在改动集）。

## 回滚路径

- 分支 = dev/260915-m1-k1a-key-layer-dispatch（Pod worktree 常驻分支）；本包 commit = 单 commit，revert 该 commit 即整体回滚（voice_round.{h,c} / 验收件为新增文件 revert 无残留；lcd_display.cc / application.cc / session_state_machine.cc / 两 CMakeLists 接线段随 revert 回改前形态）。
- 设备侧回滚 = 重烧上一稳定固件（烧录纪律见 .claude/skills/kidpal-flash-device/SKILL.md，app 偏移 0x20000）。
- 依赖序：mergeOrder 25，dependsOn [k1a, k2] 均已 merged；revert 本包不影响已 merged 包制品；k4（mergeOrder 26，dependsOn 含本包）在 revert 后需重排——revert 须连带评估 k4 依赖。

## 待 supervisor 裁断

verifier final fact 9 闸全绿（checker PASS）；host 23/0 vs 基线 18/0 零新增红 + server 面 0 collected；mutation 7/7 sensitive；docs-sync/security 绿；五轴 review 无 BLOCKING；VQ-A7 真机轮 pending 已按 human 拍板 deferred 出 Pod（交接链在 pod-events 1dc757f9）。integrator 判定：可进合并门；VQ-A7 pending 状态与三条 NIT 提请 supervisor 一并裁断。

## 修复轮 addendum（device-round VQ-A7 r1）

> 2026-09-17 human 持板第 1 轮（板上固件 = 8a3fc07，串口取证主线程转写）结果：VQ-A4 打断手感 ✅；思考页弹出（VQ-A1）/ 回显（VQ-A2）/ 长文滚动问答框（VQ-A3）+ 骨架成页（VQ-A5 观感面）❌。证据 = [device-round-vqa7-r1.md](device-round-vqa7-r1.md)。本节为增量收口：修复 diff + 增补齿 TEST 6 + scoped r6 绿 + final r7 绿，**映射表零改动红线保持**（决策表 TEST 1 齿零改动全绿，喂入时机只动触发点）。

### 三缺陷 ↔ 修复 ↔ 增补齿 ↔ 复验证据对照表

| 真机缺陷（r1 取证） | 修复（fixer 落地） | 增补齿（acceptance-author，TEST 6 单独成节，既有 TEST 1-5 零改动） | 复验证据 |
|---|---|---|---|
| ② THINKING 存活 10ms 被顶掉：方向 B 网关在 stt 后 10ms 即发 `tts start`（先于 LLM 出结果，第一句答案距 stt 12 秒），冻结映射 LISTEN_AUTO: TTS_START→TALK 在 tts start 帧当场生效 | application.cc 修复面①：tts start 帧只复位 `aborted_`，不再 `SetDeviceState(kDeviceStateSpeaking)`；`KVR_EVT_TTS_START` 喂入迁往首个 tts `sentence_start` 分支（实际起播时刻），经冻结映射单源 `kidpal_voice_round_scene` 消费（映射表零改动、只动触发点） | tooth 1：tts 分支头段（tts start 路径）禁三种场景切换载体（`SetDeviceState(kDeviceStateSpeaking)` / `kidpal_voice_round_scene(` / `kidpal_voice_apply_look(`）；tooth 2：sentence_start 分支须含 `KVR_EVT_TTS_START` 喂入且经 `kidpal_voice_round_scene` 单源 | r6 scoped 7 闸全绿（ok:true，exit 0，invocationId `97796e0f-fafe-4c15-8104-6f999b18f065`）+ r7 final 9 闸全绿（ok:true，exit 0，invocationId `ab08b7a4-4751-403c-b872-88c769dc7609`）；VQ-A6 4s 安抚窗口随此修复才真实存在（既有锁面 TEST 1 + TEST 4 tooth 6 不动） |
| ③ 回显/问答 label 只设 text_color 未设字体——LVGL 默认字体无中文字形，整段空白零上屏 | lcd_display.cc 修复面②：两 label 显式 `lv_obj_set_style_text_font(..., &font_puhui_14_1, 0)`（对照同文件 keybar 三 label 先例；14px = NFR-READ-1 可读性下限定案档，全量 puhui 非 basic 稀疏子集） | tooth 3：echo/answer label 各须 ≥1 处全量 puhui 字体配置，且禁 `font_puhui_basic_` 稀疏子集（键帽 KB-A5 同族根因禁回退） | 同上 r6/r7 全绿；host 24/0（见下）含 TEST 6 全齿 |
| ④ 「两只猫」/ 骨架未成页：echo/answer 两容器 TOP_MID 挂裸 screen 顶层被首页内容盖住；姿态 96×96 居中叠首页猫——WF-B「左小伴 · 右内容区」整页版式未落成 | lcd_display.cc 修复面③：新增 `g_kidpal_voice_page` 整页版式容器（常驻 lv_layer_top、全屏不透明背景、默认隐藏、显隐单点 = `kidpal_chrome_mid_sync_locked`）；`g_kidpal_pose_box` 左列挂点（16,36 起 96×96），pose 图改挂于此；echo/answer 挂右内容区（x=124，264×216），回显区/问答框按场景互斥显隐（LISTEN/THINK→回显区，TALK→问答框）；首页猫遮蔽销账取「整页版式盖住」形态（冻结线框无首页内容，非单屏模型冲突，零冻结稿改动） | tooth 4：echo/answer 容器须离裸 screen 根（合格形态三选一：创建点不在裸根 / 显式重挂 / 入 chrome 迁移清单），父级名不锁；tooth 5：告别 TOP_MID/CENTER 族居中叠装（WF-B 反形态销账，正几何像素级归真机轮） | 同上 r6/r7 全绿 |

增补齿 mutation 回填：taskpackage `mutationEvidence.perAcceptance` 增 8 条（MUT-VRZ-FEEDPOINT-RESTORE-EARLYFRAME / MUT-VRZ-DEVSWITCH-RESTORE-EARLYFRAME / MUT-VRZ-GLYPH-DROP-L1 / MUT-VRZ-GLYPH-SWAP-SPARSE / MUT-VRZ-GLYPH-DROP-L2 / MUT-VRZ-MOUNT-RESET-BAREROOT / MUT-VRZ-GEO-RESTORE-STACKED-ALIGN，共 8 条含 VQ-A1 双变异），逐条 `failed as expected`。

### 修复轮复验证据（integrator 现跑，2026-09-17）

1. verifier scoped r6 + final r7 fact 均 `ok:true` / observedGateExit 0；`verifier-evidence-check.mjs`（final r7）输出 verbatim：`{"ok":true,"gate":"verifier-evidence","evidenceRef":"delivery/taskpackages/260915-m1-k3-voice-round-ui/verifier-evidence/rounds/r7.json","factOk":true,"observedGateExit":0,"phase":"final","round":7}`，exit 0。`delivery/taskpackages/current/verdict.json` = verify.mjs fs-write 投影（non-bearing，与 r7 fact 同源；gatesRun 9 全绿）。
2. host runner 全量 verbatim（WSL，仓根 cwd）：
   - build：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` → `ninja: no work to do. Done`，exit 0（构建缓存与源面一致）；
   - run：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- ./firmware/main/test/host_app/build/host_infra_test.elf` → 末三行 verbatim：`24 Tests 0 Failures 0 Ignored` / `OK`，exit 0。**24 = 23 前基线 + TEST 6（1 test case，五 tooth）**，TEST 6 case 本轮 verbatim PASS（`voice_round_ui.acceptance.c:1074`）。
3. `closeout-check.mjs --read-only`：`ok:true`，exit 0（requiredFiles docs-delta.md + review-packet.md 在场；advisory 两条 non-bearing：① machine evidence 投影 advisory——`current/verdict.json` 已由 verify.mjs fs-write 刷新在案；② loop-retro 触发阈值提示，发起责任 = 主线程，非本包 gate）。
4. firmware-build（r6/r7 闸内）：xiaozhi.bin 0x30b090，app 分区 0x370000，**11% 余量**（修复净增 ~0x90 字节，资源面无虞）。
5. 影响面（11b，worktree，增量）：detect_changes 仍 **N/A (worktree)**；修复 diff 落在预期闭集 8 tracked 件（application.cc / lcd_display.cc / acceptance.c + 治理面 5 件）+ 3 untracked 新件（device-round 证据 + r6/r7 fact），无计划外文件；映射表 `voice_round.c` 零触碰（喂入时机只动 application.cc 触发点）。

### Demo 对账行更新（真机 r1 结果）

真机第 1 轮（板上 8a3fc07）：**A4 打断手感过**（KEY 单击立即停播转聆听，human 确认）；**三项不过 = VQ-A1 思考页弹出 / VQ-A2 回显 / VQ-A3 长文滚动问答框 + VQ-A5 骨架成页观感面**——三缺陷已按上表修复落地并 host 面全绿（24/0）。**VQ-A7 复测仍归 human 持板**：修复后固件（本轮 commit 构建）待主线程 kidpal-flash-device 重烧、human 复测四项原清单（① 思考页首次可弹同刻 ② 回显三手感 ③ 长文滚动观感 ④ 打断手感回归确认）。VQ-A7 状态维持 **PENDING（复测轮）**，不因修复转 PASS。

### 修复轮 findings 增量

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| 无 BLOCKING | — | — | — | — | — |
| NIT | judgement-call | governance | fixer 修复注释逐字复现结构齿禁针 token（`SetDeviceState(kDeviceStateSpeaking)` 字面入注释）致 TEST 6 tooth 1 一轮假红，fixer 自纠改写措辞转绿——结构齿树扫含注释，修复轮注释须避免逐字复现禁针序列 | pod-events friction eventId `c721520b-402f-4f46-a29c-a3850954993f`（scope-drift，planner 登记）+ changed-code hunk（application.cc 修复面①注释措辞） | 流程教训入 pod-events 在案；后续修复轮注释避开禁针字面（无独立 ARQ，不阻断） |

### 修复轮 Residual Risk 更新

- 原三条 residual risk 中「三态视觉/滚动/打断观感未经真机确认」**仍在**（VQ-A7 复测 pending，human 持板）；机制面缺陷（喂入时机 / 字体 / 挂载与版式）已由 TEST 6 齿面钉死 + host 24/0 承载。
- decisionCoverage：本轮增 8 条 mutation 记录（landsIn acceptance），仍**无 `landsIn: dropped` 条目**，无 risk-accepted 项需列。
- VQ-A6 4s 安抚计时起点在修复后才真实存在（修复前 THINKING 存活 10ms 无窗口可言）——复测轮 human 可一并观察。

### 修复轮待 supervisor 裁断

修复轮增量：scoped r6 + final r7 全绿（checker PASS）、host 24/0（23 基线 + TEST 6 五齿）、closeout-check 只读 exit 0、映射表零改动红线保持、1 条新 NIT（fixer 注释禁针假红自纠，pod-events 在案）。integrator 判定：修复轮可进合并门，与首轮收口合并裁断；VQ-A7 复测轮维持 human 持板 pending 状态不变。

## 修复轮 addendum r2（标头件）

> 2026-09-17 human 持板第 2 轮（板上固件 = 54aa903 构建）结果：**功能四项全通**——思考态持续显示 ✅（日志实锤 stt→首句 12s 窗口状态稳定）、回显 ✅、播报 ✅、打断 ✅。剩余偏差收敛为一类渲染面问题「各态内容区标识件缺失 + 气泡写点挂旧聊天 chrome」，**零状态机/逻辑改动，全在演出面**。证据 = [device-round-vqa7-r2.md](device-round-vqa7-r2.md)（human 复测主线程转述 + planner intake 只读核实）。本节为增量收口：四点偏差修复 + 增补齿 TEST 7 + scoped r8 绿 + final r9 绿。r1 轮四项功能修复维持不回归（映射表喂入时机/字体/整页版式零回改）。

### 四点标头偏差 ↔ 修复 ↔ 增补齿（TEST 7）↔ 复验对照表

| 真机偏差（r2 取证） | 修复（fixer 落地） | 增补齿（acceptance-author，TEST 7 单独成节，既有 TEST 1-6 零改动） | 复验证据 |
|---|---|---|---|
| ① 思考态右内容区缺「思考中」标头（WF-B-02）——THINK bubble 现值 NULL（「问号/气泡文案归 design review」留白） | kidpal_voice_states.c：THINK bubble = **「思考中」**（human 钉 = §2.2 态名口径，human 真机 r2 指令消解留白） | tooth 1：三态 bubble 闭集值断言（THINK「思考中」） | r8 scoped 7 闸全绿 + r9 final 9 闸全绿（均 ok:true / exit 0，见下复验段） |
| ② 聆听态内容区缺「我在听！」标识（WF-B-01；VQ-A2 冻结原文「回显在『我在听！』标识下方」——回显行悬空无挂点） | lcd_display.cc：新增右内容区标头对象（`g_kidpal_voice_header_label`，g_kidpal_voice_page 直接子件，x=124 宽 264，全量 puhui 字体同 r1 口径），创建序在 echo_box **之前**（LVGL 同父子件叠序 = 创建序，标头在上、回显行在下） | tooth 3：标头与 echo_box 同父（needle 含父参）且创建序在先（voice_page 起至 echo_box 止区间 `create(g_kidpal_voice_page)` ≥ 2 = pose + 标头） | 同上 |
| ③ 播报态右内容区缺「播报」标头（WF-B-03 线框标头字面） | kidpal_voice_states.c：TALK bubble = **「播报」**（WF-B-03 线框字面） | tooth 1：TALK「播报」断言 | 同上 |
| ④ apply_look 的 look.bubble 写 `g_kidpal_chat_label`（bottom_bar_ 键帽条内 chat_message_label_ 旧挂点；新整页版式 g_kidpal_voice_page 为 lv_layer_top 全屏 OPA_COVER，旧挂点观感不可见） | lcd_display.cc：apply_look 的 bubble 写点统一迁至右内容区标头对象；**单源不变**（仍从 kidpal_voice_states 闭集取 look.bubble，不另抄第二份文案来源） | tooth 2：apply_look 区域零 `g_kidpal_chat_label`（旧挂点写点清零）+ `look.bubble` 写点 ≥ 1（单源仍在）+ voice_page 直接子件 ≥ 4（pose + 回显区 + 问答框 + 新增标头） | 同上 |

LISTEN bubble「我在听！」既有值由 tooth 1 同齿零偏移复核（与 TEST 4 tooth 4 同值，TEST 4 零改动零冲突——TEST 4 对 THINK/TALK 只断 pose/keycap、不断 bubble，判读声明落 TEST 7 头注）。映射表 `voice_round.c` 零触碰（本轮只动 kidpal_voice_states.c 闭集值与 lcd_display.cc 演出面）。

### 问号图标缺资产 fail-closed 上报（显式登记）

planner intake 核实：`art_assets.c` 闭集**无 question/问号资产**（只有 imgfont 数字/字母/冒号/百分号）→ 思考态问号图标（WF-B-02）缺资产，按 fail-closed 处理：**文案标头先行**（本轮「思考中」标头落地），图标归**后续资产批**，TEST 7 不造图标齿。登记为 residual risk（见下），后续资产批落图标时须复核 TEST 7 头注「本齿不造图标齿」口径并按需增补。

### 修复轮复验证据（integrator 现跑，2026-09-17）

1. verifier scoped r8 + final r9 fact 均 `ok:true` / observedGateExit 0（r8 invocationId `0590539c-f47d-462d-8139-099468a3637a`，r9 invocationId `ecfc8cd1-ded6-4694-882e-f8eb7fab1e8c`）；`verifier-evidence-check.mjs`（final r9）输出 verbatim：`{"ok":true,"gate":"verifier-evidence","evidenceRef":"delivery/taskpackages/260915-m1-k3-voice-round-ui/verifier-evidence/rounds/r9.json","factOk":true,"observedGateExit":0,"phase":"final","round":9}`，exit 0。`delivery/taskpackages/current/verdict.json` = verify.mjs fs-write 投影（non-bearing，与 r9 fact 同源）。
2. host runner 全量 verbatim（WSL，绝对路径入参）：
   - build：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build` → `ninja: no work to do. Done`，exit 0（构建缓存与源面一致，fixer 轮已建）；
   - run：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app/build/host_infra_test.elf` → 末三行 verbatim：`25 Tests 0 Failures 0 Ignored` / `OK`，exit 0。**25 = 24 前基线 + TEST 7（1 test case，三 tooth）**；TEST 7 case PASS 行 verbatim：`voice_round_ui.acceptance.c:1263:语音轮场景映射: 真机轮r2回归增补——三态标头闭集+写点统一+回显挂标头下 (VQ-A2/VQ-A5):PASS`。
3. firmware-build（r8/r9 闸内真编译）：xiaozhi.bin **0x30b110**，app 分区 0x370000，0x64ef0（**11%**）free（较 r1 修复轮 0x30b090 净增 ~0x80 字节，资源面无虞）。
4. mutation 增补 5 条（taskpackage `mutationEvidence.perAcceptance`：mut-r2t1a / mut-r2t1b / mut-r2t2a / mut-r2t2b / mut-r2t3a），逐条 `failed as expected`（THINK bubble 换值 / TALK bubble 删行 / apply_look 复引旧挂点 / 标头对象删除 / 标头创建序后移，各自对应 TEST 7 齿红）。
5. 影响面（11b，worktree）：detect_changes 仍 **N/A (worktree)**；本轮 diff 落在预期闭集（firmware 4 件：kidpal_voice_states.{c,h} / lcd_display.cc / voice_round_ui.acceptance.c + 治理面：taskpackage ×2 / pod-events / verify-runs / r8/r9 fact / supervisor-verdict / ARCH-REVIEW-QUEUE 一行 ARQ 登记 / device-round-vqa7-r2.md / 本 addendum），无计划外文件。

### 修复轮 findings 增量（r2）

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| 无 BLOCKING | — | — | — | — | — |

（r2 轮零新增 finding：fixer/acceptance-author 落地一轮过，无假红/禁针/锚漂移事件；pod-events r2 段仅 dispatch-completed 两条，无 friction。）

### 修复轮 Residual Risk 更新（r2）

- 问号图标缺资产 fail-closed：思考态图标位暂缺，文案标头已先行；归**后续资产批**（human 知情，device-round-vqa7-r2.md 在案）。
- **VQ-A7 状态维持 PENDING（第 3 轮观感确认）**：r2 功能四项全通后，剩余观感面（三态标头文案上屏位置/字号/版式、问号图标缺位观感）待 human 第 3 轮持板确认；不因 r2 转 PASS。
- decisionCoverage：本轮增 5 条 mutation 记录（landsIn acceptance），仍**无 `landsIn: dropped` 条目**，无 risk-accepted 项需列。

### 修复轮待 supervisor 裁断（r2 增量）

r2 增量：scoped r8 + final r9 全绿（checker PASS）、host 25/0（24 基线 + TEST 7 三齿）、mutation +5 全 sensitive、零新增 BLOCKING finding、问号图标 fail-closed 上报项显式登记（归后续资产批）、VQ-A7 维持 PENDING（human 第 3 轮持板观感确认）。integrator 判定：r2 修复轮可进合并门，与首轮及 r1 修复轮收口合并裁断。
