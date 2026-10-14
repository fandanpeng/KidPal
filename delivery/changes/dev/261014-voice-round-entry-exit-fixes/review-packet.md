# Review Packet —— 261014-voice-round-entry-exit-fixes

> integrator 制品轮产出（2026-10-14）。状态：**verifier scoped round 1 全绿（八闸 PASS）+ integrator final 相位 round 3 全绿（十闸 PASS，bearing fact = `verifier-evidence/rounds/r3.json`）** + 四项变异取证齐（taskpackage `mutationEvidence.perAcceptance` functional 四条 turnedRed=true）+ closeout 制品齐（docs-delta.md / review-packet.md）+ feature 分支 commit 冻结 diff；**supervisor 裁断与 merge-driver 归 planner 主线程，本 integrator 不判定通过、不 merge、不 push**。rigor=full 单包、sourceType=regression、riskTier=medium。
>
> 轮次沿革：r1 scoped（八闸全绿，收口前证据态）→ **r2 final（红——scope 清单行内注释形态致 unit/acceptance 两闸 fail-closed「引用文件全部不存在」；圈定清单格式修正，非真实测试红）** → **r3 final（十闸全绿，收口证据态）**。

## 来源链

三条 TECH-261014 技术债（CRQ 各自证据链闭环）→ 主线程 triage：architect `/triage-change` 分流 review 产出 sibling `plan.md`（2026-10-14，1g 前置技术预评本体，frontmatter `reviewed-by` 行为证；fork② 真分叉裁**设备侧**，触发留痕已 append 进各 CRQ 条目）→ human triage 触点① 已裁 → 本包 Full 单包（自建 intake 形状：无 portfolio taskpackage、无 devloop/pods 条目、无 PROPOSAL；launch-card.md 在场已读）。

## 根因与修法

- **改动点 1（VR-A1，`application.cc` HandleWakeWordDetectedEvent idle 分支）**：唤醒词 connecting 态事件经状态合并被跳过，VOICE 层不推入 → 语音页整轮不显示。修法 = idle 分支在 K6 in-flight 守卫与 `EncodeWakeWord()` 之后、`IsAudioChannelOpened()` 判断**之前**直推轮首 UI 两件（`kidpal_layer_chat_clear_event(KIDPAL_LAYER_EVT_NEW_VOICE)` 为真时 `SetChatMessage("system","")` 清屏 + `kidpal_render_node_voice_begin()`），与 connecting 分支 :1058-1064 同款语义、与 KEY 路径入口直推同构；置于通道判断前 = 同时覆盖「通道已温直跳 listening」子路径。listening 分支零触碰。
- **改动点 2（VR-A2/A3，`application.cc` tts stop Schedule lambda + sentence_start）**：查字/提醒播报后设备滞留聆听态。修法 = tts stop 出口改三守卫（序即优先级）：① ManualStop 既有语义不变；② 查字收尾（`kidpal_render_node_front_layer() == KIDPAL_LAYER_CHAR_WINDOW` → `protocol_->SendStopListening()` + `SetDeviceState(kDeviceStateIdle)`，查字窗驻留语义零改）；③ 出会话 speak 不回聆听（轮来源采样 `speaking_round_entered_from_ != kDeviceStateListening` → `SetDeviceState(kDeviceStateIdle)`）；else 保留对话轮回聆听（连续追问保护 §2.1）。轮来源在 sentence_start 处 `SetDeviceState(Speaking)` **之前**采样、每轮重置。
- **改动点 3（VR-A6，`server/deploy/engine-patch/connection.py` `_kidpal_try_speak`）**：引擎原生 `speak_txt` 为同步函数（返回 None），旧形态 `await speak_txt(...)` 对 None 求值抛 TypeError 进 except、每轮提醒必报 ERROR「播报注入失败」。修法 = 同步直调（去 await）；外层 try/except 降级语义（V20「弹窗+提示音必达」）不变。

## Change Size

- 实现面（`git diff`）：`firmware/main/application.cc`（+38，改动点 1+2）、`server/deploy/engine-patch/connection.py`（+6/−1，改动点 3）。
- 验收面：`firmware/main/test/voice_round_ui.acceptance.c`（+315，纯 additive——TEST 9 十齿 + coverage map 补 VR-A1..A8 行 + REVISION LOG append 授权链；acceptance-author 冻结齿）；`server/acceptance/test_kidpal_speak_sync.acceptance.py`（新建 420 行，三 case）。
- 交付面：本包 taskpackage 目录（taskpackage.json / plan.md / launch-card.md / mutation-evidence.md / pod-events.jsonl / verify-runs.jsonl / verifier-evidence/rounds/{r1,r2,r3}.json / test-scope-final.txt / input-brief 两份）+ `current/` 五投影 + `delivery/changes/dev/261014-voice-round-entry-exit-fixes/`（本 closeout 目录）。
- Scope: 会话状态分发行为修复（设备侧）+ engine-patch 一处同步调用修正——零 wire 形状变更、零契约面、零 docs 面、零新增依赖、零新增模块。
- Split recommendation: 不拆——三条 TECH 合流单包（triage 分流已裁），单主题（语音轮入口/出口缺口），原子变。

## Verification Evidence

- **scoped 收口证据**：`verifier-evidence/rounds/r1.json`——phase=scoped、round=1、invocationId=`b72e6503-ee23-49b6-add5-a909b8d3fb82`、ok=true、observedGateExit=0，八闸全 PASS：taskpackage-required / change-manifest（pre-cutoff 存量放行）/ firmware-build（设备侧真编译，xiaozhi.bin 0x312190 bytes、app 分区余量 0x5de70 = 11%）/ contract-lint / acceptance-coverage（functional = VR-A1~A7；structural 豁免 = VR-A8；files = voice_round_ui.acceptance.c + test_kidpal_speak_sync.acceptance.py）/ mutation-check / docs-sync（docsSyncRequired=false）/ security。verify fact 程序 fs-write，integrator 只消费不重跑。
- **final 相位终验（integrator 独立跑，bearing fact = r3.json）**：命令逐字
  `node .claude/hooks/verify.mjs --evidence-mode=round --phase=final --scope-file delivery/taskpackages/261014-voice-round-entry-exit-fixes/test-scope-final.txt --change-id 261014-voice-round-entry-exit-fixes --round 3 --invocation-id afd5d3d1-efc3-40ce-ad5c-c7212523d38c`
  → `{"ok":true,"phase":"final","round":3,"evidenceRef":".../rounds/r3.json","observedGateExit":0}`（EXIT=0）。十闸全绿（八 scoped + unit/acceptance 两 full）：**unit** = `executor-run`，scope 内 16 件、structuralCollected 258、liveDeselected 4 → **258 passed, 4 deselected, 9 subtests passed**；**acceptance** = `oracle-pass`（firmwareAcceptanceFiles 2 / serverAcceptanceFiles 13）+ `executor-run`（structuralCollected 167、liveDeselected 4）→ **167 passed, 4 deselected, 9 subtests passed**。随后 checker 命令逐字
  `node .claude/scripts/verifier-evidence-check.mjs --change-id 261014-voice-round-entry-exit-fixes --phase final --round 3 --invocation-id afd5d3d1-efc3-40ce-ad5c-c7212523d38c`
  → `{"ok":true,"gate":"verifier-evidence","evidenceRef":".../rounds/r3.json","factOk":true,"observedGateExit":0,"phase":"final","round":3}`（EXIT=0）——**final evidence 有效，fact ok:true，可进入 closeout**。
- **r2 final 红（留痕，非真实测试红）**：命令同 r3（round=2），observedGateExit=1——红因 = `unit` + `acceptance` 两闸报 `scope-file 引用文件在盘上全部不存在（防 typo 假绿，fail-closed）`（scopeFileEntries 19）。根因 = 首版 scope 清单每行路径带行内 `#` 注释，而 gate 读入**不剥离行尾注释**（只认整行 `#` 注释），19 条全带注释 → onDisk 命中 0 → fail-closed 举手。**此为圈定清单格式错误（integrator 制品缺陷），非被测代码红**——修正格式后 r3 十闸全绿；fail-closed 机制按设计正确拦截了畸形清单（未静默按空跑绿），属正向信号。清除路径 = 修正清单 + 发布下一 ordinal round（r3），r2 红作为已发布事实留痕（不篡改 evidence）。
- **红基线（acceptance-author 冻结落账 2026-10-14，未修复 HEAD 实跑）**：变更前锚核验逐字命中原锚——server 164 passed / 4 deselected / 9 subtests passed；firmware host 165 Tests / 0 Failures / 0 Ignored。新齿落盘后（HEAD 未修复）设计态红：server = **1 failed / 166 passed / 4 deselected**（唯一红 = `test_kidpal_speak_sync.acceptance.py::KidpalSpeakSyncAcceptance::test_vr_a6_sync_speak_txt_takes_no_error_path`，断言实况 = ERROR 日志 `"kidpal_speak 播报注入失败: 'NoneType' object can't be awaited ..." != []`，即 defect of record 本体）；firmware host = **166 Tests / 1 Failures / 0 Ignored**（唯一红 = TEST 9 首齿 VR-A1 tooth 1，`voice_round_ui.acceptance.c:1695`）。既有 165 条齿全绿（含 VR-A4 VQ-E1、VR-A5 record_session_live）。**推导表之外变红 = 0**。增量算式 = server 164→166 passed(+2 新绿) + 1 设计态红；firmware 165→166 Tests(+1 TEST 9) + 1 设计态红。
- **落地后 host 终验（integrator 独立复跑 2026-10-14）**：**server 面**命令逐字 `python -m pytest -q --import-mode=importlib -m "not live" --ignore=server/acceptance/archive server/acceptance` → **167 passed, 4 deselected, 9 subtests passed**；**firmware host 面**命令逐字 `MSYS_NO_PATHCONV=1 wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build && wsl -d Ubuntu-24.04 -- bash -c "cd firmware/main/test/host_app && ./build/host_infra_test.elf"`（VERIFY_FW_BUILD_REQUIRED=1；build 通道 `ninja: no work to do` = elf 与当前源面同 fresh）→ **166 Tests / 0 Failures / 0 Ignored / OK**。设计态红全数转绿；相对有效基线（server 164/0/4、firmware 165/0）**零新增红**；测试增量恰为新增齿（server +3 passed、firmware +1 TEST 9 十齿）。
- **mutation 取证**（taskpackage `mutationEvidence.perAcceptance`，实测，functional 四条 `turnedRed=true && sensitive=true`；逐字红输出与回退确认见 `delivery/taskpackages/261014-voice-round-entry-exit-fixes/mutation-evidence.md`）：
  - **M1 / VR-A1**：摘 idle 分支 A″ 直推两件（`diff` 1006,1017d1005，12 行）→ `166 Tests 1 Failures`，`voice_round_ui.acceptance.c:1695` VR-A1 RED；回退复绿 166/0。
  - **M2 / VR-A2+A3（同源，合并执行）**：tts stop 三守卫回退原二分支（`diff` 703,712d702 + 714,718d703）→ `166 Tests 1 Failures`（Unity 首失败落 tooth 4），`voice_round_ui.acceptance.c:1748` VR-A2 RED「tts stop 出口缺查字收尾守卫」；回退复绿。
  - **M3 / VR-A2**：仅摘 sentence_start 轮来源采样（`diff` 737,740d736）→ `166 Tests 1 Failures`（落 tooth 7），`voice_round_ui.acceptance.c:1794` VR-A2 RED「sentence_start 未在 SetDeviceState(Speaking) 之前采样轮来源」；回退复绿。
  - **M4 / VR-A6**：`connection.py` 同步调用回退为 await（`390c390`）→ `1 failed, 2 passed`，`test_vr_a6_sync_speak_txt_takes_no_error_path` AssertionError（ERROR 日志 != []）；回退复绿 `167 passed, 4 deselected`。
  - 全部变异完全回退、工作树零残留（末态 `CLEAN_NO_RESIDUE`，`diff -q` 逐一确认）。取证豁免面 = VR-A4（VQ-E1 冻结齿保持绿）/ VR-A5（零改动 canary）/ VR-A7（相对基线零新增红，套件级）/ VR-A8（真机轮 structural 豁免）——无逐 AC 变异对应物。
- **范围圈定终验（Step 4，三分法：per-Pod 专属图索引可用 → 正常圈定，无降级）**：`detect_changes` 实跑（Pod 专属索引 `C-AI-KidPal-pod-m1-k1a-key-layer-dispatch`，base=main，merge_base=0baab93，inbound depth=2，含工作树未 commit diff）→ 29 seed / 90 impacted（impacted_modules：firmware/main 5 / firmware/scripts 19 / server/gateway 20 / server/shim.py 3 / server/acceptance 24 / server/tests 19）。**test-scope 清单落盘路径（一行锚）= `delivery/taskpackages/261014-voice-round-entry-exit-fixes/test-scope-final.txt`**；圈定 = 本包冻结验收件 ∪ blast radius 命中测试面（firmware voice_round_ui.acceptance.c 模块 + server acceptance 8 件 + server tests 3 件，archive/ 按 sweep 剔除）∪ impactedTestsSeed 种子/canary ∪ 共享 seam 安全余量；`firmware/scripts` 19 件为通用符号名（main / create_widgets / test 等）over-approximation 噪声，零真实调用关系、剔除。**floor = 本包两冻结验收件**（voice_round_ui.acceptance.c + test_kidpal_speak_sync.acceptance.py，单包无对端冻结件）。firmware host 套件为单 ELF（`host_infra_test.elf`，WHOLE_ARCHIVE）——圈定清单与实跑天然对齐「套件级全量」；server 面 pytest 按文件选集跑（scope 内 13 acceptance + 3 unit）。
- **影响面核对记账（Step 9b advisory）**：工作树与 Pod 专属图索引一致时跑 detect_changes，改动只影响预期符号（seed 面 = application.cc / connection.py 实现面 + 两验收文件），结果记入 `verdict.json` / `verification.json` + 本 packet；per-Pod 索引可用、无降级、无 re-index 需求。
- **diff 面核对**：`git status` 实证改动面 == changeManifest（`application.cc` / `connection.py` / `voice_round_ui.acceptance.c` / `test_kidpal_speak_sync.acceptance.py`；`host_app/main/CMakeLists.txt` 零改动 = 未抽 seam 文件在册确认）+ delivery/ 簿记（本包 taskpackage 目录 + current/ 五投影 + changes/dev/ 本 closeout 目录）。**零触碰面**（`docs/requirements.md` / `docs/architecture.md` / `docs/contracts/**` / `docs/crq/**` / `record_session_live.acceptance.c` / `session_state_machine.cc` / `kidpal_render_node.cc` / `kidpal_layer.*` / `.claude/**` / `tools/**`）`git status` 零命中。`grep minimal:` 两实现文件零命中。
- **架构/依赖卫生检查摘要**：零新增依赖（application.cc 复用既有 display/layer API；connection.py 仅去 await）；零循环依赖；零绕过 contracts 的本地类型（`speaking_round_entered_from_` 为文件级 static DeviceState，与同文件既有 `s_vad_speaking` / `s_story_rec_entered` 同款，单实例语义等价——planner 裁决接受、随 taskpackage notes 留痕）；A″ 调用点与 connecting 分支同构、无跨层越权接线。

## 未覆盖面申报（对照本包 `acceptance[]` + 本包引用契约条目 + `plan.md` 效果段，逐条一行）

- **VR-A1（唤醒词 A″ 入口直推）**：已覆盖（host structural 齿 TEST 9 tooth 1/2/3 钉两件在位与通道判断前位置 + 变异 M1 turnedRed 实证判别力）。
- **VR-A2（tts stop 三守卫 + 轮来源采样）**：已覆盖（host structural 齿 tooth 4/5/6/7 钉三守卫源文本与顺序优先级 + sentence_start 采样位置 + 变异 M2/M3 turnedRed）。
- **VR-A3（判别器三边角：abort 后新一轮 / realtime(AEC) / ManualStop）**：**部分覆盖——本次无独立行为断言**。现状 = 三边角以「同族守卫变异」的**源锚代理强度**承载（齿钉守卫源文本与采样位置，非独立行为 oracle）——根因：`application.cc` 依赖 ESP-IDF/协议栈、**不可 host 编**，host 侧无法驱动真实状态机走三边角组合。去向 = **行为 oracle 归真机轮 VR-A8**（plan 真机节第 2 项「连续追问回归」+ fork② 不误伤判定面；已随 plan 冻结、由主线程组织联测）。
- **VR-A4（VQ-E1 冻结齿保持绿）**：已覆盖（`voice_round_ui.acceptance.c` VQ-E1 齿在 166 Tests 全量实跑中保持绿，本批不接线 60 秒 timer、未受扰）。
- **VR-A5（`record_session_live.acceptance.c` 零改动回归绿）**：已覆盖（零 diff 在册 + 全量实跑绿——canary 判据「零 diff + 全绿」机械满足）。
- **VR-A6（engine host 断言，同步 speak_txt 驱动 `_kidpal_try_speak`）**：已覆盖（`test_kidpal_speak_sync.acceptance.py` 三 case：正钉零 ERROR / 认领路径不回归 / V20 降级守卫；167 passed 实跑 + 变异 M4 turnedRed）。
- **VR-A7（相对有效基线零新增红）**：已覆盖（server 167 vs 基线 164、firmware host 166 vs 基线 165，增量恰为新增齿；推导表之外变红 = 0）。
- **VR-A8（真机轮六项 live-manual structural 豁免）**：**本次未覆盖——全部延后（human 裁）**。现状 = human 已裁「真机轮不在本 Pod 范围」（taskpackage notes 执行边界③）；本 Pod 终点 = host 面转绿 + 结构齿在位 + commit 本分支，不挂起等 human。去向 = 主线程在 human 回来后组织联测，判定标准权威 = plan 真机节六项（①唤醒词入口含通道已温子路径 + KEY 回归对照 / ②连续追问回归 / ③提醒轮回 idle 不回聆听 + 弹窗驻留 + 引擎侧子项延后至 human 人工部署 ECS 后闭账 / ④查字轮播完不回聆听且窗驻留 / ⑤驻留面合并回归 / ⑥附带观察项两条只登记）；固件烧录 = app 分区单烧（纯代码变更无 spiffs 资产变更），由主线程执行（human 预授权本轮自主烧板）。
- **本包引用契约条目（`docs/contracts/interface-contract.md`）**：**零变更、无验收断言增补义务**。仅作 `contractRefs` canary 引用——fork② 裁设备侧 = wire 协议零变更（不新增、不修改任何引擎↔设备消息），契约条目无 diff，故无「契约条目未覆盖」缺口（`contract-lint` 闸 r1/r3 两轮 PASS 佐证）。
- **engine 侧真实 edge-tts 出声 + 「无 ERROR 且语音照常下发」**：**本次未覆盖——延后至 human 人工部署 ECS 后闭账**。现状 = engine-patch 改动只 commit 到本分支、**不部署 ECS**（human 裁执行边界①）；host 侧断言以 stub `speak_txt` 承载（真控制流、真 except 分支），但不触真实 edge-tts 出声链。去向 = plan 真机节第 3 项「引擎侧子项」（human 人工部署 ECS 后同轮闭账，为 exit criterion、非本轮验收门）。
- **`plan.md` 非目标/已知残余（5 项）**：**本次无验收断言（有意不覆盖）**——① 60 秒 timer 活侧 producer 接线（Phase 1 已登记，本批明令不接线）；② TECH-261012-edgetts-403-hotpatch 耐久化（触发条款名义命中但裁定留 ECS 部署动作批）；③ TECH-261012-first-keypress-no-listening（独立批）；④ 观察项两条（只登记不修）；⑤ 提醒语音与活跃对话 TTS 交错的引擎侧治理（不在本批）。对照面存在但**有意不覆盖**，去向 = 各自登记/后续批（非本包义务）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| firmware/main/application.cc / server/deploy/engine-patch/connection.py / firmware/main/test/voice_round_ui.acceptance.c / server/acceptance/test_kidpal_speak_sync.acceptance.py | 本包改动面 `git diff` grep `minimal:` 零命中——零新增简化债务；改动文件内既有标记（如有）原样在世、不属本批 | n/a | n/a |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | VR-A3 判别力承载方式 | 三边角（abort 后新一轮 / realtime(AEC) / ManualStop）判别力由「同族守卫变异 + 采样位置源锚」承载，非独立行为齿——`application.cc` 不可 host 编，host 侧无法独立驱动三边角行为组合 | changed-code hunk（application.cc tts stop 三守卫块 + sentence_start 采样块）+ mutation-evidence M2/M3 同源多齿联动（Unity 首失败落 tooth 4/7，同族判别力集合 tooth 4/5/6/7）+ heuristic（源锚代理强度） | 无（行为 oracle 归真机轮 VR-A8；已在未覆盖面申报逐条留痕） |
| NIT | judgement-call | unit 层义务载体 | changeManifest 无独立 unit 文件——unit 层义务由在册测试文件承载（engine 侧 `test_kidpal_speak_sync.acceptance.py` 为 unittest.TestCase 且经 AST 抽取驱动真实方法体；firmware 侧 host runner voice_round_ui TEST 9）；plan 对「抽纯逻辑 seam」明载「建议（非硬性）」、本包未采用 | changed-code hunk（connection.py `_kidpal_try_speak`）+ taskpackage notes builder 存疑点 planner 裁决②（unit 闸实跑 258 passed 佐证）+ heuristic（在册件承载） | 无（planner 边界内裁定在案；unit 闸 r3 executor-run 258 passed 实跑承载） |
| NIT | judgement-call | 采样载体形态 | `speaking_round_entered_from_` 实现为 `application.cc` 文件级 static（非 application.h 类成员）——本包 changeManifest 未含 application.h，为守住文件级写权边界 | changed-code hunk（application.cc 文件级静态声明 + sentence_start 采样 + tts stop 消费）+ 同文件既有先例 `s_vad_speaking` / `s_story_rec_entered` + heuristic（单 Application 实例语义等价） | 无（planner 裁决接受、随 taskpackage notes 留痕；冻结齿钉 .cc 内字面且全绿） |

结构闸 PASS/FAIL 不复述为 finding（gate 全绿仍完整执行五轴语义 review，见下）。本轮 findings 面清洁：**无任何 hard-violation**（无 hard 级 findings）；契约/spec 类 finding 的起引前缀约定（`docs/contracts/**` / `docs/requirements.md` / `docs/architecture.md` / `docs/spec.md`）核验行 = **none**——无 Kind=hard-violation 且 Basis 涉契约/spec 的条目。

## Five-Axis Review

- **Correctness（是否满足每个声明的 S-Fx.y，边界是否缺失）**：三点修法逐条兑现既有 spec 口径——改动点 1 覆盖 S-F1.4 入口直推（`requirements.md:39` / `:295` / `:300`）且覆盖第二活实例（通道已温子路径）；改动点 2 兑现 `:652`（v1.37 查字播完会话即结束）+ fork② 出会话不回聆听，else 正向路径保留 §2.1 连续追问保护；改动点 3 修正 defect of record（同步 speak_txt 误 await）。边界：listening 分支零触碰、查字窗驻留/5 分钟兜底零改、V20 降级语义（弹窗+提示音必达）不变——host structural 齿 + 四变异 M1..M4 逐条实证判别力。VR-A3 三边角为源锚代理强度（真机 oracle 归 VR-A8，已申报）。
- **Test quality（是否断言真实行为，含错误路径）**：engine 侧 `test_kidpal_speak_sync.acceptance.py` 经 AST 抽取驱动 `connection.py` **真实方法体**（真控制流、真 except 分支），断言无异常 / 返回 True / 认领路径不回归 / 抛异常降级仍返回 True（错误路径覆盖）；firmware 侧 TEST 9 十齿钉源文本 structural + 采样位置。非「没报错就算过」——四变异 turnedRed 实证敏化。
- **Contract/boundary safety（是否漂移 contracts / events / spec）**：`docs/contracts/**` 零 diff（contract-lint r1/r3 PASS）、`docs/requirements.md` / `docs/architecture.md` 零 diff（docs-sync r1/r3 PASS）——本批实现既有口径、零 spec/contract 漂移；wire 协议零变更（fork② 设备侧推论）；无影子实现绕 contracts 面。
- **Operability/security（ErrorEnvelope / correlationId / 权限 / 输入 / secret / 日志）**：零新增攻击面——纯设备侧会话状态分发 + engine 一处同步调用修正，零新增出网路径 / 零新增输入处理面 / 零 secret。日志面：改动点 3 令「kidpal_speak 播报注入失败」ERROR 在同步成功路径不再误报（降级 except 仍在、可观测面保留）；`securityReviewRequired=false`（security 闸 r1/r3 PASS）。
- **Maintainability/simplicity（是否局部、易懂、无过早抽象/重复）**：改动收敛单文件单函数（application.cc 三块 + connection.py 一处），零过早抽象、零重复实现、零新增依赖；`speaking_round_entered_from_` 文件级静态与同文件既有先例同款（single write point）；simplicity tags 零命中（`minimal:` 零标记）。

## Residual Risk

- **plan.md「回归风险」5 项（逐条承接到位）**：① 判别器错杀连续追问——结构性不可达（对话轮恒从 Listening 进入 Speaking），三边角已落齿 + 变异实证；② 提醒播报中用户恰起对话——按 entered_from 判、行为同今日不劣化（engine 侧交错治理不在本批）；③ 查字窗显示晚于 tts stop 的竞态——守卫漏触发则维持现状（回聆听，已知残余非新缺陷）；④ 冻结件重锚面——本批 voice_round_ui 为纯 additive（未触发重锚，REVISION LOG 已 append 授权链）；⑤ engine 项部署时点漂移——commit-only 状态下容器若重建则修复随部署源漂移，联测部署时一并核 manifest（**Known limitation，见下**）。
- **Known limitation**：① **VR-A8 真机轮六项未闭环**——host 面全绿不构成真机目击（TEST 9 只是 host 源锚代理，非真机状态机 oracle）；host 面 closed、真机探真 open（human 裁本 Pod 终点不含真机轮）。② **engine 侧真实 edge-tts 出声 + 无 ERROR 语音照常下发未闭环**——需 human 人工部署 ECS 后同轮闭账（不部署 = host 断言仅证方法体行为、不证端到端出声）。③ **VR-A3 三边角行为 oracle 未独立覆盖**——归真机轮。以上三项均在「未覆盖面申报」逐条留痕。
- **Follow-up recommended**：VR-A8 真机联测（主线程在 human 回来后组织；判定标准 = plan 真机节六项）+ 三条 TECH CRQ 闭账（条件 = 包 merge + 真机复验轮）+ engine-patch ECS 部署核 manifest。
- **Human decision needed**：真机轮组织（human 在场联测）+ engine-patch ECS 人工部署时点。**all-mock 申报**：本包计划含真机轮（VR-A8），不应判 all-mock；本轮真机证据不落（执行边界③，human 已裁）——若 supervisor 机械闸判 all-mock，以 human launch 边界③（「host 面转绿 + 结构齿在位 + commit 本分支即本轮终点」）为既有 human 裁定留痕、不静默不升门挂起（261012/261013 先例口径）。taskpackage **未启用 decisionCoverage 字段**——`landsIn: dropped && reason: risk-accepted` 条目 = **零**，本 Residual Risk 节按模板申报零条目。

## 执行边界留痕（human 已裁，随包在案）

1. **engine-patch 改动只 commit 到本分支、不部署 ECS**——已遵守（connection.py 改动入本分支 commit，未触 ECS 部署；联测时 human 人工部署）。
2. **真机轮不在本 Pod 范围**——已遵守（判定标准随 plan 真机节冻结，主线程组织联测；未因真机证据缺失挂起）。
3. **不 push；不新建 worktree；合并 main 不在本次范围**——已遵守（本分支 commit、无 push；supervisor 机械闸判定落盘即可，不执行 merge）。
4. **当前分支 = `dev/260915-m1-k1a-key-layer-dispatch`**——已核（`git rev-parse --abbrev-ref HEAD`）。
5. **CRQ 三条目状态未翻转**——已遵守（闭账条件 = 包 merge + 真机复验轮，两者均不在本轮范围；docs/crq/ 零 diff）。
6. **验收件归 acceptance-author**——integrator 未改 `*.acceptance.*` / `**/acceptance/**`（voice_round_ui.acceptance.c / test_kidpal_speak_sync.acceptance.py 由 acceptance-author 冻结、builder 零改，integrator 只读消费）。

## 集成执行回报

本轮无 merge-driver 冲突消解（单包制品轮，Pod 分支无对端冲突，全部改动为工作树未 commit 态一次收口），集成复跑清单 n/a；圈定 floor 注（双方冻结件 ∪ 圈定清单）不适用——**单包无对端冻结件，floor = 本包两冻结验收件 `voice_round_ui.acceptance.c` + `test_kidpal_speak_sync.acceptance.py`**。复跑面 = 影响面圈定清单（test-scope 内 server 13 acceptance + 3 unit 经 final phase 两 full 闸实跑 + firmware host 单 ELF 全量实跑承载，含 record_session_live / key_layer_dispatch / schedule_grid 等全部同 ELF 套件）。detect_changes 记账见 Verification Evidence（29 seed / 90 impacted，无降级、per-Pod 索引可用）。**全量回归 = 人工触发位，不在本次**（final 相位按 scope 圈定，非全量）。
