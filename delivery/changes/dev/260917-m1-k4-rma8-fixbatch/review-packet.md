# Review Packet · 260917-m1-k4-rma8-fixbatch（K4 RM-A8 缺陷修复批：F1 渲染回执接线 + F2 弹窗驻留守卫）

> integrator 收口，面向 supervisor 合并门。verifier final round fact = `delivery/taskpackages/260917-m1-k4-rma8-fixbatch/verifier-evidence/rounds/r4.json`（2026-09-17T14:01:45Z，phase=final，round 4，invocationId `14a340ac-9e45-4bc3-8ebd-ba4966fd6d72`，9 闸全绿，ok:true，observedGateExit 0）；`verifier-evidence-check.mjs` PASS（factOk:true，输出 verbatim 见下）。`delivery/taskpackages/current/verdict.json` = non-bearing compatibility projection（verify.mjs fs-write 产出，与 r4 fact 同源同内容）。Pod worktree 常驻分支 dev/260915-m1-k1a-key-layer-dispatch（树池复用第 5 棒续用，K1a/N1b/K2/K3/K4 同支）。

## Change Size

- Files changed: tracked 7 件修改 + untracked 8 项（taskpackage 目录下 input-brief ×2、pod-events.jsonl、verify-runs.jsonl、verifier-evidence/ r1-r4）+ 本目录 closeout 制品 4 件（`git diff --cached --stat` 终值见 commit 段）
- Net diff: 实现面两文件 +25/-2——`mcp_server.cc` +9/-2（F1 回执接线一行 `int armed = kidpal_reminder_mark_rendered(state, shown.id);` @ :366 + alert_render 日志带 armed=%d）+ `lcd_display.cc` +16（F2 `kidpal_voice_apply_look` 入口条件守卫 + voice_look / voice_look_skip 两条常驻 ESP_LOGI 观测点）；冻结验收件 `reminder_overlay_unify.acceptance.c` +115（TEST 9 FX-A1 集成齿 + FX-A4/A5 两条 coverage marker 行 + FX coverage map 块 + REVISION LOG 两行 append）；治理面 = taskpackage.json（mutationEvidence 三条补录 + fixNotes）+ current/ 投影两件（verify.mjs fs-write）+ dev INDEX 幂等重建纠偏（见超读申报段）
- Scope: firmware mcp_server 回执链 + display 驻留守卫；server/ 零触碰、`docs/contracts/**` 零触碰、`docs/requirements.md` 零触碰
- Split recommendation: 不拆——单包回归修复批，mergeOrder 27，dependsOn []（随 K4 收口即就绪）

## Verification Evidence

### 红基线 → 终跑对比

| 面 | 红基线（2026-09-17 intake，acceptance-author 实跑 @ f0a650e） | 终跑（verifier final r4，2026-09-17T14:01Z） |
|---|---|---|
| host runner 全量 | **35 Tests / 1 Failure / 0 Ignored**——既有齿 34 全绿（K4 终跑 34/0/0 锚零新增红），新增 TEST 9（FX-A1）RED 为本批设计态义务红，红名 `F1 渲染回执链接线: RaiseReminderQueue 渲染路径调用 mark_rendered (FX-A1)`，失败点 acceptance.c:1208 tooth 1（渲染路径零 mark_rendered 调用）——红因 = device-round-1 F1 根因本体 | **35 Tests / 0 Failures / 0 Ignored**，exit 0（unit 闸 37 passed 系结构性 pytest 面口径，与 host runner 35 齿面互证口径见 K4 packet 同款注记） |
| server 面 | 0 collected（`--ignore=server/acceptance/archive` 口径） | 0 collected（firmwareAcceptanceFiles=19 / serverAcceptanceFiles=0，acceptance 闸 oracle-pass） |
| 设备编译 | builder deviceBuildCaveat：Pod host-only 无设备 toolchain，lcd_display.cc 未经设备面编译（符号一致性人工核验在案） | **已消解**——final r4 firmware-build 闸真编译通过（ninja 实跑 9.6s，xiaozhi.bin 0x30b5b0 / app 分区 0x370000，11% 余量） |

### FX-A1 RED→GREEN 声明（义务红链完整）

1. **RED**：红基线实跑（HEAD f0a650e，未打修复）TEST 9 tooth 1/2 双红 @ acceptance.c:1208——齿代码零 `mark_rendered` 自调（K4 假绿根源禁令，头注自证），红 = 生产零调用点这一集成事实本体。
2. **修复**：mcp_server.cc:366 一行接线（落位 alert_drawn!=1 早退后、EmitRenderReceipt 前，覆盖 raise_next 返回 1/2 两分支，并激活 :302-305 重推补回执路径）。
3. **GREEN**：接线落位后 host runner 35/0/0（final r4 在案），TEST 9 PASS @ acceptance.c:1190。
4. **变异反证**：MUT-FX-A1-render-receipt-wire-off（接线行改 `int armed = 0;`，变异后全文件 `kidpal_reminder_mark_rendered(` 计 0 处）→ 35/1 双红复现；还原后 35/0/0——齿非注释假绿（注释留行不会红），mutation 证据封死 vacuous 通道。

### 红轨迹消解记录（r1/r2）

| 轮 | 红闸 | 红因 | 消解 |
|---|---|---|---|
| r1（13:39Z，ok:false） | acceptance-coverage | FX-A2~A6 五条「未覆盖」——FX-A4/A5 的 coverage 声明以 map 块 prose 形态存在，不被 marker 正则识别 | acceptance-author 补两条独立 coverage marker 注释行（零齿触碰零逻辑改动），r2 起 coverage 闸绿 |
| r2（13:47Z，ok:false） | mutation-check | FX-A1/A4/A5 mutationEvidence 缺失/不 sensitive——builder 尚未回传补证 | builder 补证轮实跑回传三条（FX-A5 含三段独立变异），r3/r4 mutation-check 绿 |

### 逐闸证据表（final r4，均 ok:true / exit 0）

| 闸 | kind | final r4 | 要点 |
|---|---|---|---|
| taskpackage-required | governance | PASS (198ms) | current/taskpackage.json 指向本包 |
| firmware-build | structural | PASS (9.6s) | 真编译；xiaozhi.bin 0x30b5b0 / 0x370000，11% 余量；**builder deviceBuildCaveat 在此消解** |
| contract-lint | structural | PASS (153ms) | docs/contracts/interface-contract.md 只读面零触碰 |
| acceptance-coverage | functional | PASS (153ms) | FX-A1~A7 全覆盖；structural = [FX-A2, FX-A3, FX-A7]；suiteGates = [FX-A6] |
| mutation-check | functional | PASS (167ms) | 3 functional（FX-A1/A4/A5）mutationEvidence 全 sensitive + 3 structural + 1 suiteGate 在册 |
| docs-sync | governance | PASS (152ms) | docsSyncRequired=false，声明与 diff 一致（见 docs-delta.md） |
| security | security | PASS (144ms) | securityReviewRequired=false |
| unit | functional | PASS (3.0s, full) | **37 passed in 0.31s**，structuralCollected=37，liveDeselected=0 |
| acceptance | functional | PASS (224ms, full) | oracle-pass；on-target 验收归 RM-A8 真机轮（pending 见下） |

### verifier-evidence-check 机械核验输出（verbatim）

```
{"ok":true,"gate":"verifier-evidence","evidenceRef":"delivery/taskpackages/260917-m1-k4-rma8-fixbatch/verifier-evidence/rounds/r4.json","factOk":true,"observedGateExit":0,"phase":"final","round":4}
```

exit 0。

### Mutation 摘要（taskpackage mutationEvidence.perAcceptance，5 次变异 / 3 functional acceptanceId 全 sensitive；FX-A4/A5 为 K4 同款变异式复核 + tooth4/5 新补）

| acceptanceId | 变异 | counterEvidence | 结果 |
|---|---|---|---|
| FX-A1 | MUT-FX-A1-render-receipt-wire-off（mcp_server.cc 接线行改 `int armed = 0;`，命名不含被测符号子串） | TEST 9 tooth1/2 双红 acceptance.c:1208「渲染路径零 mark_rendered 调用（现 0 处）」；还原后 35/0/0 TEST 9 PASS:1190 | failed as expected |
| FX-A4 | MUT-FX-A4-key-click-passthrough（reminder_page.c:942-947 KEY 单击分支改 return KRM_KEY_PASS） | TEST 6 Block 2 红 acceptance.c:880「KEY 单击未关闭回当前页」+ 涟漪 :772（与 K4 34/2 涟漪一致）；还原后 35/0/0 | failed as expected |
| FX-A5 | MUT-FX-A5a-recording-raised（reminder_page.c:1047-1050 gate 录音列 DEFER→RAISE）；MUT-FX-A5b-record-gate-wire-off（mcp_server.cc:329 gate 调用改常量 RAISE）；MUT-FX-A5b-recording-trigger-off（application.cc:758 删 RaiseReminderQueue 触发） | A5a: TEST 1 tooth2 红 :567「录音前台门控未延后 DEFER」；A5b tooth4 红 :1022「零 gate 消费」、tooth5 红 :1032「零触发接线」（TEST 7 返回式断言早停故两齿分别独立取证）；各还原后 35/0/0 | failed as expected |

（FX-A2/A3/A7 为 structural 真机齿/对象条目，不由 mutation 承载——由 RM-A8 真机确认轮 human 持板承载，pending 见下。）

## Demo 对账行（workstream 包必含）

**demo 证据形态**（本包代位锚，见下）= **host runner 输出**（35/0/0，final r4 acceptance 闸实跑在案）+ **F2 常驻观测日志**（`voice_look scene=%d top=%d` 与 `voice_look_skip scene=%d top=%d reason=reminder_persist` 两条 ESP_LOGI 保留为常驻观测点，为 RM-A8 真机串口判据依据）。

**一致性结论**：本包为 regression 修复批（origin=regression, upstreamRef=device-round-1.md），roadmap.md m1-home-qa 无本包独立效果段；**代位锚 = roadmap K4 节效果段 + device-round-1 根因钉死段**。对账结论：**F1/F2 修复方向与 device-round-1 根因钉死方向逐条一致**——F1 修复（mark_rendered 接线）即 device-round-1 F1 节「修复候选（一行级）」原文落位；F2 修复（apply_look 入口条件守卫）即 device-round-1 F2 节候选根因「SetStatus speaking 时无条件 apply_look(KVST_SCENE_TALK) 刷回语音态」的定点消解，且未触发架构级逃生门（FX-E1 条件性逃生门未触发：单一咽喉点定向守卫）。**真机演出效果 pending RM-A8 确认轮（human 持板）**——修复方向一致性不预支真机演出结论；无与效果段冲突项。

## FX-A7 归属裁定与状态标注（planner 裁定随卡传达，integrator 如实落账）

- **模拟器截图缺口归属（FX-A7 内 planner 裁定项）**：该缺口 = K4 收口遗留（K4 review-packet Demo 对账行已登记「模拟器截图面：本包未产，如实标缺口」，归属当时即指向 RM-A8 真机确认轮）。本包为固件修复批、无新 UI 面、无新截图需求 → **归属裁定：随 RM-A8 确认轮重跑一并消解（human 持板轮在真机/模拟器取证时顺带补），不拆独立批**。
- **FX-A2/A3（deferred 真机齿）+ FX-A7 四项清单**：弹窗演出 / 最高层覆盖 / 录音排队例外 / 模拟器截图四项清单 = **pending RM-A8 重跑**（human 持板环节由主线程安排，非本 Pod loop 内）；FX-A6 真机半（无提醒驻留时三态视觉一致目击）同轮承载。真机取证沿用 device-round-1 形态（journalctl + 串口 + human 目击三方互证），证据落本目录。

## 验收对照表（FX-A1~A7 / FX-E1~E3）

| 条目 | 状态 | 证据 |
|---|---|---|
| FX-A1 F1 host 集成齿（RED→GREEN） | **PASS（RED→GREEN 已声明）** | 红基线 35/1（义务红）→ 接线后 35/0/0 + MUT-FX-A1 变异反证 |
| FX-A2 F1 真机齿 | **PENDING RM-A8**（structural deferred） | host 前置 = TEST 9 绿；真机三方互证（acked=1 / 无 60s 重推 / 单次 TTS）归 human 持板轮 |
| FX-A3 F2 不变量靶 | **PENDING RM-A8**（structural deferred） | host 无齿（presence-only 假齿被 builder 裁拒）；常驻观测点 voice_look / voice_look_skip 已留桩，串口判据就位 |
| FX-A4 F2 键位语义 | PASS（机制面） | 既有 TEST 6 Block 2 承载（本包零改语义）+ MUT-FX-A4 同款复核红；真机手感归 RM-A8 |
| FX-A5 录音排队例外补测 | PASS（机制面） | TEST 1 tooth2 + TEST 7 tooth4/5 承载 + MUT-FX-A5 三段独立变异红；真机补测归 RM-A8 |
| FX-A6 语音轮回归靶 | PASS（host 半） | host runner 全量既有齿零新增红（35/0/0，红基线对账）；真机半（会话目击）归 RM-A8 |
| FX-A7 RM-A8 确认轮重跑（structural） | **PENDING RM-A8** | human 持板轮；四项清单 + 模拟器截图缺口归属裁定见上节，证据落本目录待补 |
| FX-E1 F2 前置排查不指向架构级改法 | PASS | fixNotes.f2：根因钉到 file:line（共享骨架对象覆写链三段在案）；escapeHatch = 未触发（单一咽喉点定向守卫） |
| FX-E2 铁律（不改 contracts/spec/agent 定义/gate） | PASS | contract-lint + docs-sync 绿；git diff 零 docs 命中 |
| FX-E3 F0 不在本包 | PASS（范围纪律） | F0（speaking 显示锁假死）零触碰——见「遗留与移交」节 |

## Decision coverage 对账

taskpackage `decisionCoverage = []`（空，分流批无 architect decision 条目）——**无 `landsIn: dropped` 条目，无 risk-accepted 项需列**。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| 无 BLOCKING | — | — | — | — | — |
| NIT | judgement-call | governance | `changes-index.mjs` 无 `--help` 处理路径：传入 `--help` 落入无参分支直接执行幂等全量重建（integrator 本轮意图为查看用法，意外触发 dev INDEX 纠偏重建，详见超读申报段）；重建输出含 k2 行状态判 open（与 board 已 merged 认知表观分歧，未深查，非本包范围） | changed-code 之外的操作轨迹（本 packet 超读申报段 verbatim）+ 脚本头注「用法」段无 --help 条目 | 脚本 --help 行为属治理脚本小缺口，随 ARQ 池登记候选；k2 INDEX 行状态交主线程/supervisor 复核时留意，本包不处理 |
| NIT | judgement-call | test hygiene | TEST 9 为源级结构断言齿（needle 扫描接线事实），注释漂移不红的理论风险由 mutation 证据封死，但锚漂移（RaiseReminderQueue / alert_drawn!=1 / EmitRenderReceipt 三锚改名）会致 KRMU RED(infra) 假红——齿头注已自证该失效模式 | changed-code hunk（acceptance.c TEST 9 tooth 锚切取 fail-loud 注记）+ MUT-FX-A1 证据 | 无需动作；锚漂移属 fail-loud 显性失效，后续改动触锚时按齿内注记复核 |

## Five-Axis Review

1. **Correctness**：F1 = 单一行接线，落位钉死在渲染成功路径（alert_drawn!=1 早退后、EmitRenderReceipt 前）——「画成即武装」，渲染未成不武装（不假装送达，degrade 早退语义保留）；公共路径天然覆盖 raise_next 1（新弹）/2（滞留顶替）两分支，并激活 :302-305 重推补回执路径；armed 值随 alert_render 日志出站可观测。F2 = 单咽喉点定向守卫（scene!=REMIND && front_is_reminder==1 时 skip），REMIND 场景自身放行（弹窗绘制路径 :2132 调用不受影响）；无提醒驻留时守卫恒不触发——FX-A6 回归自辩由 host 全量零新增红承载。grep 实证：`kidpal_reminder_mark_rendered(` 生产调用闭集 = 恰 1 处（mcp_server.cc:366，本包接线）+ 定义/声明两处，零计划外调用方。
2. **Test quality**：TEST 9 齿代码零 mark_rendered 自调（K4 假绿根源禁令头注自证）；双 tooth（接线存在 + 落位正确）分立断言，落位漂移（早退前武装 = 假装送达、EmitRenderReceipt 后武装 = 永不发出）各自独立红；形状选型声明在案（拒绝 mock 剧场 harness，理由 = Over-mocking 反模式 + 桩级 harness 证明不了真机 acked=1）；结构齿 vacuous 通道由 MUT-FX-A1 封死；FX coverage map 双形态（marker 机读 + prose 人读）语义逐字一致（r1 红轨迹即双形态失配的实修记录）。
3. **Contract/boundary safety**：`docs/contracts/**` 零触碰（contract-lint 绿）——回执语义契约已冻结且正确，本包为实现贴合性修复非语义变更；`docs/requirements.md` 零触碰；server 面零触碰；F0 范围纪律守住（FX-E3，零触碰）；守卫读取的 `kidpal_render_node_front_is_reminder` 为 K4 既有层栈查询件，消费不改语义。
4. **Operability/security**：无新依赖、无 secret/网络面；security 闸绿 securityReviewRequired=false；firmware-build 资源余量 11%；两条常驻 ESP_LOGI 为单行 INFO 级、量级无害（守卫注释自证）；F1 修复后网关 acked=1 → 60s 重推链收敛 → TTS 重播消失，可运维性净改善。
5. **Maintainability/simplicity**：两修复共 +25/-2 行、零抽象新增；F2 未引入独立弹窗对象层（escapeHatch 未触发，架构级改法留待真有第二消费者时再议）；deletion test：删守卫即回 F2 缺陷本体（守卫即最小语义承载）；观测点常驻但单行、与既有日志风格一致。

## Simplification Debt

- 本包改动面 `minimal:` 标记命中 **0 处**（mcp_server.cc / lcd_display.cc / acceptance.c grep 零命中）。
- 既有在案债务不重复登记：reminder_page.c:983 旧卡面 `minimal:` 标注为 K4 packet 已登记清理候选，本包未触碰该文件区域。

## Residual Risk

- Known limitation: F1/F2 修复的**真机演出效果未经确认**——RM-A8 确认轮（human 持板）pending，四项清单 + FX-A2/A3/A6 真机半全部挂该轮；本包证据面为 host 机制面 + 设备编译面，观感/链路面不可达。
- Known limitation（轻）: F2 守卫为条件 skip——若未来出现第二类「合法需要覆写 REMINDER 前台」的语音侧需求，须回头重审守卫判据（当前无此需求面）。
- Follow-up recommended: F0（speaking 显示锁假死，device-round-1 独立 finding）**不在本包、未修**——走 ARQ 池或独立修复批（FX-E3 铁律）；主线程分流。
- Human decision needed: **RM-A8 确认轮重跑排期**（human 持板，主线程安排，非本 Pod loop 内）；模拟器截图缺口随该轮顺带补（planner 裁定已落账，见 FX-A7 节）。
- decisionCoverage 审计：decisionCoverage = []，无 `landsIn: dropped` / risk-accepted 项。

## S2 exploratory scan

TaskPackage 未声明 `s2ExploratoryRequired=true`，本包不执行 S2 official scan。

## UI design-review 证据

TaskPackage 无 UIUX 工作面（固件修复批、无新 UI 面、无 uiux 派单）——本节无 viewport/a11y/design-drift 证据义务；模拟器截图缺口归属见 FX-A7 节（随 RM-A8 轮消解，不拆独立批）。

## 影响面核对（11b，worktree）

- detect_changes: **N/A (worktree)**（Pod worktree diff 对索引不可见，如实标注）；替代抽查：
  - `git diff --stat` 落在预期闭集（7 tracked 件：firmware 3 件 + taskpackage 治理面 3 件 + dev INDEX 1 件），无计划外文件；
  - 关键共享符号 trace 抽查（grep 实证，图索引未刷新时同口径降级）：`kidpal_reminder_mark_rendered` 生产闭集 = mcp_server.cc:361-366 接线 1 处 + reminder_page.c:886 定义 + reminder_page.h:150 声明，零计划外调用方；`kidpal_voice_apply_look` 消费闭集 = lcd_display.cc:1793/1798/1800（SetStatus 三态）+ :2132（REMIND 绘制路径）——全部与守卫语义相容（REMIND 放行、无驻留恒不触发）；
  - crossEnd 两件（mcp_server.cc RaiseReminderQueue / lcd_display.cc kidpal_lcd_reminder_show）：前者 = 本包 F1 接线所在函数（改动面内），后者 = 零触碰（F2 守卫在其调用链下游 apply_look 入口，不动 show 本体）。

## 回滚路径

- 分支 = dev/260915-m1-k1a-key-layer-dispatch（Pod worktree 常驻分支）；本包 commit = 单 commit，revert 该 commit 即整体回滚（实现两文件回改前形态；acceptance.c 增量段随 revert 回退——注意 TEST 9 revert 后将回义务红态，回滚 = 回到 device-round-1 缺陷在世状态，符合预期）。
- 设备侧回滚 = 重烧上一稳定固件（K4 收口固件，烧录纪律见 .claude/skills/kidpal-flash-device/SKILL.md，app 偏移 0x20000）。
- 依赖序：mergeOrder 27，dependsOn []；本包随 K4 之后收口，revert 不影响已 merged 包制品。

## 遗留与移交

1. **F0（speaking 显示锁假死）**：不在本包（FX-E3 铁律），本包零触碰；移交 = 主线程分流 ARQ 池或独立修复批。builder 的 deviceBuildCaveat（lcd_display.cc 未经设备面编译）**已由 final r4 firmware-build 闸消解**（真编译通过，xiaozhi.bin 产出在案）——该 caveat 不再是遗留项。
2. **RM-A8 确认轮重跑**：human 持板，主线程安排（非本 Pod loop 内）；清单 = 弹窗演出 / 最高层覆盖 / 录音排队例外 / 模拟器截图四项 + FX-A2/A3/A6 真机半；证据落本目录。
3. **FX-A7 模拟器截图缺口**：归属裁定 = 随 RM-A8 确认轮重跑一并消解，不拆独立批（planner 裁定，上文 FX-A7 节落账）。

## 超读申报段

本轮 integrator 两项超出常规收口动作，如实申报：

1. **`changes-index.mjs --help` 意外触发全量重建**：任务第 6 步要求先 `--help` 看用法；该脚本未实现 --help 分支，落入无参路径执行了幂等全量重建，修改 `delivery/changes/dev/INDEX.md`（diff = 4 行纠偏：k2/k3/k4/n1b 既有目录行补入并按 changeId 排序，dev 计数 24→28）。重建为脚本机械幂等输出、非手改；其中 k2 行状态判 `open` 与 board 已 merged 认知表观分歧，已作 Findings NIT-1 如实登记，未深查（非本包范围）。本包行在 closeout 制品落盘后以同脚本重建刷入（--upsert 为 arch 单行专用，dev 段走全量重建，语义等价达成任务意图）。
2. **K4 review-packet 全文读取**：作为 closeout packet 格式对齐样例跨包只读，未修改。

## 待 supervisor 裁断

verifier final fact 9 闸全绿（checker PASS，r4 含 unit/acceptance full 实跑）；红基线 35/1（义务红）→ 终跑 35/0/0 零新增红；FX-A1 RED→GREEN 已声明且 mutation 反证封死 vacuous；mutation 5 变异 / 3 acceptance 全 sensitive；docs-sync/security 绿；builder deviceBuildCaveat 由设备真编译消解；五轴 review 无 BLOCKING；RM-A8 真机轮 pending（FX-A2/A3/A7 + FX-A6 真机半 + 模拟器截图缺口，全部归属 human 持板轮，planner 裁定已落账）；F0 如实移交未顺手修。integrator 判定：可进合并门；RM-A8 pending 状态与两条 NIT 提请 supervisor 一并裁断。

---

**固件严格模式声明：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开）**——本包 verifier final r4 firmware-build 闸为真编译（9.6s，ninja 实跑，xiaozhi.bin 产出），声明行随卡走不得省。
