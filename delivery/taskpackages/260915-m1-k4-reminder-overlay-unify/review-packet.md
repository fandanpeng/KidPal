# Review Packet · 260915-m1-k4-reminder-overlay-unify（提醒弹窗四态统一骨架 + 最高层覆盖 + 录音排队例外）

> integrator 收口，面向 supervisor 合并门。verifier final round fact = `delivery/taskpackages/260915-m1-k4-reminder-overlay-unify/verifier-evidence/rounds/r3.json`（2026-09-17T08:39Z，phase=final，round 3，invocationId `acaadbb8-224d-4d6e-849d-8d3e54fc5aa1`，9 闸全绿，ok:true，observedGateExit 0）；`verifier-evidence-check.mjs` PASS（factOk:true，输出 verbatim 见下）。`delivery/taskpackages/current/verdict.json` = non-bearing compatibility projection（verify.mjs fs-write 产出，与 r3 fact 同源同内容）。Pod worktree 常驻分支 dev/260915-m1-k1a-key-layer-dispatch（树池复用第 5 棒，K1a/N1b/K2/K3 同支）。

## Change Size

- Files changed: tracked 13 件 + untracked 5 项（验收齿 1 件 + pod-events / red-baseline / verify-runs / verifier-evidence r1-r3）= 18 项（`git diff --cached --stat` 终值见「commit 交付」段）
- Net diff: firmware 代码面 tracked 8 件 +374/-106；实现本体 = `reminder_page.{h,c}` +64（seam [1] 入层门控 `kidpal_reminder_gate` + [2] 时间行单源 `kidpal_reminder_hhmm` 导出 + RM-A5 去重语义收紧：已 acked 驻留重推零动作）+ `lcd_display.cc` +70（`kidpal_lcd_reminder_show` 四态骨架消费 C 桥 + chrome 中部显隐 reminder_shell 判据）+ `mcp_server.cc` +42/-9（RaiseReminderQueue 消费门控 + 旧 v1.13 卡面渲染调用点销账 + 三件套提示音落点）+ `application.cc` +5（录音会话结束 Idle 路径触发 RaiseReminderQueue）+ `kidpal_render_node.{h,c}` +24（`front_is_reminder` / `front_layer` 层栈查询）+ `test/host_app/main/CMakeLists.txt` +13（本包实现 + 验收齿接线，置于 smoke 之前）；冻结验收件 `reminder_overlay_unify.acceptance.c` 新增 1217 行（冻结即基线，未动）
- Scope: firmware display 提醒弹窗演出 + 层级语义收口（骨架 owner = voice-round-ui 消费复用、分层裁决 owner = key-layer-dispatch 消费）；server/ 零触碰、`docs/contracts/**` 零触碰、`docs/requirements.md` 零触碰
- Split recommendation: 不拆——单包单面，mergeOrder 26（本 workstream 末包），dependsOn [k1a, k2, k3] 均已 merged

## Verification Evidence

### 红基线 → 终跑对比

| 面 | 红基线（2026-09-17 intake，两面 launch 判停后重跑） | 终跑（2026-09-17 integrator 复核） |
|---|---|---|
| host runner 全量 | **26 Tests / 0 Failures / 0 Ignored OK**，exit 0（13/0 旧锚 @ e990eb2 之后 K2/N1b/K3 三包齿入 runner +13；schedule_reminder_b 经 k1a 批改后归档退役；在册既存红 = 空；冻结记录 = [red-baseline.md](../../taskpackages/260915-m1-k4-reminder-overlay-unify/red-baseline.md)） | **34 Tests / 0 Failures / 0 Ignored OK**，exit 0（26 基线 + 本包 8 新 test，零新增红；RM-A9 达标） |
| server 面 | 0 collected，exit 0（`--ignore=server/acceptance/archive` 口径，与锚 @ a398264 一致） | **no tests ran in 0.01s**（同口径，0 collected 一致，exit 0） |
| C 类冻结验收实编 | gcc `-fsyntax-only -Wall -Wextra` error 零 | 齿随 builder 接线后全部转绿（下表） |
| 义务红 | test 1/2/4/5/6 weak 哨兵 -2；test 7 needle 零命中 + 旧卡面调用点在世；test 8 注册序缺——全数解析性红定性 | 全部转绿（builder 落地 + 接线后）；test 3 为现状即绿的零回归守卫齿（K1a 落地件消费面） |

### 逐闸证据表（final r3，均 ok:true / exit 0）

| 闸 | kind | final r3 | 要点 |
|---|---|---|---|
| taskpackage-required | governance | PASS (220ms) | current/taskpackage.json 指向本包 |
| firmware-build | structural | PASS (10.1s) | 真编译；xiaozhi.bin 0x30b480 / app 分区 0x370000，**11% 余量** |
| contract-lint | structural | PASS (248ms) | docs/contracts/interface-contract.md 只读面零触碰 |
| acceptance-coverage | functional | PASS (249ms) | RM-A1~A9 全覆盖；structural = [RM-A8]；验收文件 1 件 |
| mutation-check | functional | PASS (196ms) | 8 functional + 1 structural（RM-A8），逐条 mutationEvidence 在册 |
| docs-sync | governance | PASS (210ms) | docsSyncRequired=false，声明与 diff 一致（见 docs-delta.md） |
| security | security | PASS (233ms) | securityReviewRequired=false |
| unit | functional | PASS (4.6s, full) | **37 passed in 0.30s**，structuralCollected=37，liveDeselected=0 |
| acceptance | functional | PASS (251ms, full) | oracle-pass；firmwareAcceptanceFiles=19 / serverAcceptanceFiles=0；on-target 验收归 acceptance 相位人工/台架（本包承载 = RM-A8 真机轮，pending 见下） |

### verifier-evidence-check 机械核验输出（verbatim）

```
{"ok":true,"gate":"verifier-evidence","evidenceRef":"delivery/taskpackages/260915-m1-k4-reminder-overlay-unify/verifier-evidence/rounds/r3.json","factOk":true,"observedGateExit":0,"phase":"final","round":3}
```

exit 0。

### 全量验证复核（integrator 现跑，2026-09-17）

1. host runner（WSL，仓根 cwd，绝对路径入参）：
   - build：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app/espidf_wsl.sh build` → `ninja: no work to do. Done`，exit 0（构建缓存与源面一致）；
   - run：`MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- /mnt/c/AI/KidPal-pod-m1-k1a-key-layer-dispatch/firmware/main/test/host_app/build/host_infra_test.elf` → 末三行 verbatim：`34 Tests 0 Failures 0 Ignored` / `OK`，exit 0。与 verifier final 一致；**34 = 26 基线 + 本包 8 新 test**（test 1 门控决策表 / test 2 时间行单源+三件套动作集 / test 3 layer 栈覆盖零回归守卫 / test 4 排队不叠屏 / test 5 去重 / test 6 驻留与按键 / test 7 活侧接线结构齿 / test 8 注册序守卫+canary）。
2. server 面：`python -m pytest -q --import-mode=importlib -m "not live" --ignore=server/acceptance/archive server/acceptance` → verbatim：`no tests ran in 0.01s`，exit 0（0 collected，与红基线及 final unit 闸 37 passed 口径互证：server 现役面空态 + 结构性 pytest 37 件全绿）。

### Mutation 摘要（taskpackage mutationEvidence.perAcceptance，8 变异 / 8 functional acceptanceId 全 sensitive）

| acceptanceId | 变异 | counterEvidence | 结果 |
|---|---|---|---|
| RM-A1 | lcd_display_remind_scene_swap_stub | test 7 tooth 1, expect lcd_display.cc 消费 kidpal_voice_apply_look(KVST_SCENE_REMIND) ≥1 处（骨架单源不 fork）; FAIL @ line 991「现 0 处」 | failed as expected |
| RM-A2 | reminder_gate_raise_col_invalid_stub | test 1 tooth 1, expect kidpal_reminder_gate(非录音前台 layer=0,0)==KRM_GATE_RAISE; FAIL @ line 556 | failed as expected |
| RM-A3 | reminder_gate_defer_col_raise_stub | test 1 tooth 2, expect kidpal_reminder_gate(录音 layer=0,1)==KRM_GATE_DEFER; FAIL @ line 567 | failed as expected |
| RM-A4 | reminder_raise_render_guard_removed_stub | test 4 tooth 2, expect raise_next(1615+1)==0（驻留未渲染不得叠屏）; FAIL @ line 766 | failed as expected |
| RM-A5 | reminder_receive_acked_rearm_stub | test 5 tooth 3, expect receive(已 acked 重推)==KRM_STATE_RECEIVED 零动作不复活; FAIL @ line 824 | failed as expected |
| RM-A6 | reminder_key_click_passthrough_stub | test 6 Block 2 KEY 单击, expect ==KRM_KEY_DISMISS && page_out==PAGE_P2; FAIL @ line 880（连带 test 4 tooth 3 共享 key() 语义涟漪） | failed as expected |
| RM-A7 | reminder_v18_expire_noreplace_stub | test 6 Block 3, expect raise_next(1615)==2（V18 滞留顶替）; FAIL @ line 949 | failed as expected |
| RM-A9 | cmake_registration_after_smoke_stub | test 8 tooth 1, expect 本包两行排在 host_infra_smoke.acceptance.c 之前; FAIL @ line 1115 | failed as expected |

（RM-A8 为 structural 对象条目，不由 mutation 承载——由真机轮 human 持板承载，pending 见 Demo 对账行与 Residual Risk。）

## RM-E3 骨架消费/扩展登记（显式声明）

RM-E3 口径：「只用 voice-round-ui 建的骨架，改骨架须回 owner 包登记，不得 fork。」本包对骨架的关系 = **消费者扩展，非 fork、非骨架改动**，逐点登记：

1. **单源零触碰**：骨架单源件 `kidpal_voice_states.{c,h}`（五态 look 闭集）与 `voice_round.c`（场景决策映射）**零改动**（git diff 实证不在本包改动集）——闭集值、决策表行、骨架对象创建路径全部原样。
2. **消费形态**：`lcd_display.cc`（骨架 owner 包 k3 建立的宿主文件）新增 `kidpal_lcd_reminder_show(const kidpal_alert_t *a)` C 桥——经 `kidpal_voice_apply_look(KVST_SCENE_REMIND)` 走闭集单源取 pose-jump + 键帽「知道了」，**零第二渲染源**；`kidpal_chrome_mid_sync_locked` 增 `reminder_shell` 判据（REMIND 场景 + `kidpal_render_node_front_is_reminder()`），REMIND 态复用整页版式 `g_kidpal_voice_page` + 问答框承载大字事项/时间行。
3. **内容承载**：右内容区标头 = 时间行（经 seam [2] `kidpal_reminder_hhmm` 单源，渲染面不重写第二份时间解析）、主体 = 大字事项（`a->title` 直写问答框 label）。
4. **旧形态销账**：mcp_server.cc `RaiseReminderQueue` 改调本 C 桥，旧 v1.13 卡面 `kidpal_reminder_page_render` 调用点清零（test 7 tooth 3 结构齿承载）；函数本体仍留 reminder_page.c（参考面，后续清理候选，见 Simplification Debt）。
5. **登记性质**：本登记即 RM-E3 要求的「回 owner 包登记」载体（changes/dev closeout 制品 + 本节）；voice-round-ui 的骨架契约（WF-B-04 版式、五成员闭集、apply_look 单源）未被本包收窄或放宽。

## Demo 对账行（锚 3：roadmap.md 效果段 ↔ 证据）

roadmap.md §260915-m1-k4-reminder-overlay-unify「完成后人能看到什么」逐句对账：

| 效果句 | host 齿 / 结构断言承载 | 只能真机确认的部分 |
|---|---|---|
| 到点提醒弹窗与语音轮四态**同一版式骨架**（左小伴 pose-jump + 右大字事项/时间） | test 2（时间行单源决策 + 三件套动作集 SHOW\|TONE 必达 + 骨架第四成员闭集零偏移复核）+ test 7 tooth 1/2（apply_look(KVST_SCENE_REMIND) 骨架单源消费 + hhmm 单源消费） | 骨架成页的**视觉观感**（pose 位图、版式几何） |
| 弹出时**压过**查字窗/录音态/播报中等一切前台内容（最高层） | test 1（gate RAISE 列：非录音前台一律允许入层）+ test 3（layer 栈 REMINDER(5) 压 VOICE/SERVER_PAGE 行为级 + 板级裁决/前台查询消费零回归守卫） | 各前台态被压住的**观感与时序手感** |
| 置顶期间**按键不穿透**底层窗口 | test 6（KEY/BOOT 单击 DISMISS、其它手势 PASS 驻留不变——板级 dispatch 顶层先交 kidpal_reminder_key 裁决，K1a 落地件零改动守卫） | 按键**手感**（真机时序） |
| **录音中**到达的提醒**排队延后**、录音会话结束**立刻弹** | test 1（DEFER 列 + 越界防御）+ test 7 tooth 4/5（RaiseReminderQueue 消费门控 + application.cc 录音会话结束触发在世） | 「提醒 TTS 不混进录音内容」的**听感** |
| 弹窗**驻留到按键不自动消失** | test 6（驻留块 + 关闭语义 KEY 回当前页 / BOOT 回原页不翻页，消费 K1a 分层裁决） | 过夜驻留**手感** |
| **同一提醒不重复弹** | test 5（排队期同 id 屏蔽入队 + 已渲染重推补回执 + 已 acked 重推零动作 + 关闭后不复活） | —（纯行为面，host 齿全承载） |
| 三段阶梯提醒的手感与 §2.2 权威口径一致 | test 6 Block 3 V18 滞留顶替（过期档位不补发不追催） | 阶梯时刻的**实际触发手感**（真机） |

**对账结论：一致（host 面）**——效果段逐句的机制/接线面全部由 host 齿与结构断言承载且终跑全绿，无一句在 host 面落空；观感/手感面（骨架视觉、覆盖观感、录音听感、驻留手感）只能真机确认。证据形态按 roadmap ③ 本包所定 = 模拟器截图 + host runner 输出 + 真机轮：

- **host runner 部分**：本轮全量绿（34/0/0，上文 verbatim）——**已交付**。
- **真机轮（RM-A8，human 持板）**：**如实标 PENDING**——沿用 k3 三件套惯例，本包三件套现状：① structural 条目 = taskpackage acceptance RM-A8 `kind: structural` 在册（mutation-check 认得）；② human-decision 事件 = **尚未在 pod-events 登记**（k3 先例由 human 拍板 defer 时由 planner 登记；本包真机轮交接/defer 决定权在 planner/human，integrator 不代登记，提请 planner 在 closeout 后补登记）；③ 本节如实 pending。真机清单已在冻结验收件尾注钉死四项：① 弹窗演出（四态统一骨架观感 + 三件套同时发生）② 最高层覆盖（查字窗/播报中/思考页弹出时压过一切前台）③ 录音排队例外（录音中到点不弹、会话结束立刻弹）④ 驻留手感（过夜不消失 + KEY/BOOT 语义 + 排队下一条自动弹）。**无真机证据，不虚构一致性结论**。
- **模拟器截图面**：**本包未产，如实标缺口**——本包 Pod 流程内未执行模拟器截图（无 uiux 派单、无模拟器运行记录），缺口归属 = RM-A8 真机确认轮（验收件尾注明示证据形态 = 模拟器截图 + 真机轮日志/照片，落本目录），随 human 持板轮一并补齐；不虚构截图面证据。

## 验收对照表（RM-A1~A9 / RM-E1~E3）

| 条目 | 状态 | 证据 |
|---|---|---|
| RM-A1 四态统一骨架 + 三件套 | PASS（机制面） | host 齿 test 2/7（骨架单源消费 + 动作集 + 闭集零偏移）；视觉观感归真机 |
| RM-A2 最高层覆盖 | PASS（机制面） | host 齿 test 1/3/6（gate RAISE 列 + layer 栈行为级 + 不穿透半）；覆盖观感归真机 |
| RM-A3 录音排队例外 | PASS（机制面） | host 齿 test 1/7（DEFER 列 + 门控消费 + 会话结束触发在世）；听感归真机 |
| RM-A4 排队不叠屏 | PASS | host 齿 test 4（入队不动驻留 + raise 不替换 + 关闭后自动弹） |
| RM-A5 去重不重复弹 | PASS | host 齿 test 5（同 id 屏蔽 + 补回执 + 已 acked 零动作 + 不复活） |
| RM-A6 驻留到按键 + KEY/BOOT 语义 | PASS（行为面） | host 齿 test 6（KEY DISMISS 回当前页 / BOOT 回原页不翻页 / 其它手势驻留不变）；驻留手感归真机 |
| RM-A7 过期档位不补发 | PASS | host 齿 test 6 Block 3（V18 滞留顶替、零 timer 零字面值 canary） |
| RM-A8 真机确认轮（structural） | **PENDING** | human 持板轮；四项真机清单见验收件尾注，证据落本目录待补；human-decision 事件提请 planner 补登记 |
| RM-A9 host 终跑 | PASS | 34/0 vs 有效基线 26/0 零新增红；red-baseline.md + integrator 复跑一致 |
| RM-E1 范围纪律（云侧零改动） | PASS | server 面零触碰（git diff 实证 + server 0 collected 一致）；三段阶梯时刻/范围/权限 = 云侧数据面未动 |
| RM-E2 需求面零修订 | PASS | docs-sync 绿 + git status -- docs/ 零命中 + test 8 canary 齿绿 |
| RM-E3 骨架复用纪律 | PASS | 单源件零触碰 + 消费者扩展登记（见上文 RM-E3 节）+ test 7 tooth 1 单源齿 |

## Decision coverage 对账（taskpackage decisionCoverage 5 条 → landsIn acceptance 逐条核）

| decision | landsIn | review-packet 承载 |
|---|---|---|
| M1块④-四态统一骨架（不单独设计） | acceptance:RM-A1 | 验收对照表 RM-A1 行 + Demo 对账行第一句 + RM-E3 节 |
| M1块④-提醒最高层覆盖 | acceptance:RM-A2 | 验收对照表 RM-A2 行 + Demo 对账行第二句 |
| M1块④-录音态排队例外 | acceptance:RM-A3 | 验收对照表 RM-A3 行 + Demo 对账行第三句 |
| M1块④-弹窗驻留到按键 | acceptance:RM-A6 | 验收对照表 RM-A6 行 + Demo 对账行第五句 |
| M1块④-BOOT新语义生效 | acceptance:RM-A6 | 验收对照表 RM-A6 行（BOOT 回原页不翻页，消费 K1a 分层裁决，test 6 承载） |

5/5 条 landsIn acceptance 且在 review-packet 有承载；**无 `landsIn: dropped` 条目**，无 risk-accepted 项需列。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| 无 BLOCKING | — | — | — | — | — |
| NIT | judgement-call | governance | 图索引滞后：CodebaseMemory KidPal 主索引未反映本包新增符号（kidpal_reminder_gate / kidpal_reminder_hhmm 查不到），且仍索引已归档退役的 schedule_reminder_b.acceptance.c 旧路径（k1a 批改归档后未增量刷新），trace_path 抽查以 grep 实证补位（见影响面核对节） | changed-code grep（mcp_server.cc:329/355 消费、reminder_page.c:1042/1055 定义、lcd_display.cc:2111 消费——闭集干净零计划外调用方）+ detect_changes: N/A (worktree) | 无需动作；随树索引自刷新，后续包 trace 抽查同口径降级 grep |
| NIT | judgement-call | governance | k3 三件套惯例第 ② 件（human-decision 事件）在本包 pod-events 尚未登记——pod-events 现仅 2 friction + 3 dispatch-completed，无真机轮 defer/交接 human 拍板事件；本 packet 不代登记 | pod-events.jsonl 全量 5 行实读（无 human-decision 类型行）+ taskpackage RM-A8 kind:structural 已在册 | 提请 planner 在 closeout 后比照 k3 先例（eventId `1dc757f9`）补登记真机轮交接决定，不阻断合并门（RM-A8 本就 pending 于 human） |
| NIT | judgement-call | code hygiene | 旧 v1.13 卡面渲染函数 `kidpal_reminder_page_render` 及其 `minimal:` 网点层标注（reminder_page.c:983）在调用点销账后仍留函数本体（reference 面，无死代码告警；`minimal:` 标记为在世既有、不在本包 diff 内） | changed-code hunk（mcp_server.cc 调用点清零，test 7 tooth 3 反证齿承载）+ git diff grep「minimal:」零命中 | 后续清理批候选；本包不动（保留参考面，避免与本包演出面改动混提） |

## Five-Axis Review

1. **Correctness**：门控决策表（kidpal_reminder_gate）为纯 C 单源、无芯片依赖，三值冻结（RAISE=1/DEFER=0/INVALID=-1）与验收件哨兵声明逐值一致（偏移即预处理器冲突 fail-loud）；录音排队例外由 RaiseReminderQueue 入口门控承载（DEFER 即 return、队列保持、不发回执不叠屏），录音会话结束经 application.cc Idle 路径触发恢复弹出（无排队时 raise_next 返回 0 = 幂等 no-op）；去重语义收紧（已 acked 驻留重推零动作）堵住「一 id 一回执」缺口；时间行单源经 hhmm 导出，渲染面零第二解析。host 8 新 test 全绿 + mutation 8/8 sensitive 背书。
2. **Test quality**：冻结验收件 1217 行，weak 哨兵保证未接线即真实行为红（非链接错误）；决策表行为半 + 活侧接线结构半双承载；test 3 为现状即绿的零回归守卫齿（K1a 落地件消费面，本性非假绿、齿注记自证）；闭集零偏移齿；注册序守卫齿；扫描器自检齿（region slicer 失灵即红）；RM-A9 suite 级判读段钉死基线口径与义务红定性。
3. **Contract/boundary safety**：`docs/contracts/**` 零触碰（contract-lint 绿）；`docs/requirements.md` 零触碰（RM-E2 canary 齿 + git status 实证）；层栈只**消费** K1a 分层裁决（REMINDER(5) 优先级在世零改动，test 3 守卫）；骨架只**消费** k3 单源（RM-E3 节显式登记，单源件零触碰）；server 面 / 云侧数据面零触碰（RM-E1）。旧卡面渲染调用点销账由反证齿钉死，不静默残留。
4. **Operability/security**：无新依赖（reminder_page.c 增量纯 C、host/device 两侧同源；CMakeLists 仅接线两行）；无 secret/网络面；security 闸绿 securityReviewRequired=false；firmware-build 资源余量 11%（0x64b80 free / 0x370000）；降级路径显式（hhmm 不可读 → "--:--" 占位、渲染未成 → 不发回执交网关 60s 重推收敛、不假装送达）。
5. **Maintainability/simplicity**：门控/时间行/去重全部收敛为 reminder_page.c 单一 seam 面，消费点（mcp_server/lcd_display/application）零重复实现；deletion test：删掉 kidpal_reminder_gate 则录音排队语义散回 mcp_server 调用点 = 深模组信号（留）；结构齿以 needle 扫描钉接线，接缝漂移即红；无过早抽象；旧卡面渲染函数留作参考面并登记清理候选（不与本包混提）。

## Simplification Debt

- 仓库 `minimal:` 标记命中 1 处：reminder_page.c:983（旧卡面网点层半透明压屏等效实现）——**在世既有、非本包新增**（本包 diff 内零命中），且其所在渲染函数已随本包销账退出主调用链；随旧函数本体一并列为后续清理批候选。
- 本包无新增降级交付 tradeoff 挂账。

## Residual Risk

- Known limitation: 骨架成页观感（pose 位图、版式几何）、最高层覆盖观感、录音排队听感、驻留手感未经真机确认（RM-A8 pending，human 持板）；机制面已由 host 齿 + mutation 8/8 承载，观感面本包证据面不可达。
- Known limitation（轻）: 三件套之「语音读出」一半不在设备侧——服务端到点向引擎腿注入 kidpal_speak（裁定二在案），设备只画「小伴在说」形态并出提示音；真机轮听感确认含此形态。
- Human decision needed: **RM-A8 真机确认轮**（human 持板，四项清单见验收件尾注）+ 模拟器截图补齐——归属与交接决定在 planner/human（见 Findings NIT-2）。
- decisionCoverage 审计：5 条 decision 全部 landsIn acceptance，**无 `landsIn: dropped` 条目**，无 risk-accepted 项需列。

## S2 exploratory scan

TaskPackage 未声明 `s2ExploratoryRequired=true`，本包不执行 S2 official scan（exploratory evidence 非本包义务）。

## UI design-review 证据

TaskPackage 未声明 uiuxRequired（无 uiux 派单面）；本包 UI 面为骨架消费复用（零新增设计决策，RM-E3 节登记），观感像素面统一归 RM-A8 真机轮承载，不新增 gate。

## 影响面核对（11b，worktree）

- detect_changes: **N/A (worktree)**（Pod worktree diff 对索引不可见，如实标注）；替代抽查：
  - `git diff --stat` 落在预期闭集（13 tracked 件，全部为本包声明面：firmware 8 件 + delivery 治理面 5 件），无计划外文件；
  - 图索引滞后（NIT-1），trace_path 抽查降级 grep 实证：新符号消费闭集 = mcp_server.cc:329（gate）/ 355（show）、lcd_display.cc:2111（hhmm）、reminder_page.c:1042/1055（定义）、kidpal_render_node.cc:636（front_layer）——全部在本包 diff 内、零计划外调用方；
  - crossEnd 三件（reminder_page.c kidpal_reminder_receive / kidpal_reminder_raise_next + kidpal_render_node.h kidpal_render_node_begin_page）: 全部在本包改动面内且语义按冻结验收件 [3] 消费（schedule_reminder_b 归档件 FROZEN OFFLINE CONTRACT 承接）；骨架单源件 kidpal_voice_states.{c,h} / voice_round.c **零触碰**（git diff 实证不在改动集）。

## 回滚路径

- 分支 = dev/260915-m1-k1a-key-layer-dispatch（Pod worktree 常驻分支）；本包 commit = 单 commit，revert 该 commit 即整体回滚（验收件为新增文件 revert 无残留；reminder_page.{h,c} / lcd_display.cc / mcp_server.cc / application.cc / kidpal_render_node.{h,c} / CMakeLists 接线段随 revert 回改前形态）。
- 设备侧回滚 = 重烧上一稳定固件（烧录纪律见 .claude/skills/kidpal-flash-device/SKILL.md，app 偏移 0x20000）。
- 依赖序：mergeOrder 26，dependsOn [k1a, k2, k3] 均已 merged；本包为 M1-home-qa workstream 末包（无下游 dependent 在册），revert 不影响已 merged 包制品；后续新包若 dependsOn 本包须连带评估。

## 待 supervisor 裁断

verifier final fact 9 闸全绿（checker PASS）；host 34/0 vs 基线 26/0 零新增红 + server 面 0 collected；mutation 8/8 sensitive；docs-sync/security 绿；RM-E3 骨架消费/扩展已显式登记；五轴 review 无 BLOCKING；RM-A8 真机轮 pending（三件套中 human-decision 事件缺位已作 NIT-2 提请 planner 补登记）+ 模拟器截图缺口如实标注。integrator 判定：可进合并门；RM-A8 pending 状态与三条 NIT 提请 supervisor 一并裁断。

---

**固件严格模式声明：VERIFY_FW_BUILD_REQUIRED=1（无关闭留痕即开）**——本包 verifier final r3 firmware-build 闸为真编译（10.1s，ninja 实跑，xiaozhi.bin 产出），声明行随卡走不得省。
