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
