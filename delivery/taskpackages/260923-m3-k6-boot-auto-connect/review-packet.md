# Review Packet · 260923-m3-k6-boot-auto-connect

> workstream m3-schedule ｜ M3-K6 开机自动建连（Shape A：`auto_connect_in_flight_` 标志互斥 + 状态恒 Idle，双触发点接线 + 静默失败拦截 + 3 次有界退避 + 四人工 call site in-flight 守卫）｜ rigor lite ｜ riskTier medium ｜ subsystem firmware ｜ gateProfile [test-quality] ｜ origin.type human-confirmed-plan（triage ②单包 Lite + Shape A 形状修订，human 离席预授权技术判断、回席补 ack 留痕 plan.md）
> integrator 收拢制品（2026-09-23）；合并门判读归 planner 主线程 supervisor-verdict-gate 机械闸，本包不 push、不 merge、不在 main 上 commit。dependsOn []（K5 已 merged @b869658，常驻树/常驻分支 dev/260915-m1-k1a-key-layer-dispatch 续棒）。

## Change Size

- **实现（未提交改动 → 本次收口 commit）**：代码 4 文件（3 改 + 1 新增验收件 + 1 新增红基线脚本，另 delivery 制品面）。
  - 修改：`firmware/main/application.h`（+9：私有成员 `auto_connect_in_flight_` + 方法声明 `void TryStartAutoConnect();`，各带语义头注）；`firmware/main/application.cc`（+119 行段：[S3] `kAutoConnectBackoffS[3]={5,10,20}` 决策表 + `TryStartAutoConnect()` 缝函数（预检只读 → 守卫三条件 → 置标志 → xTaskCreate "AutoConnect" task：3 次退避重试 / 每次重试前重查三条件 / 所有退出路径必清标志 / 栈高水位日志）+ 双触发点接线（HandleActivationDoneEvent 末尾 + HandleNetworkConnectedEvent Idle 支路）+ OnNetworkError lambda in-flight 拦截分支 + OnAudioChannelOpened lambda 体 Schedule() 包裹 + 四人工 call site 守卫）；`firmware/main/test/host_app/main/CMakeLists.txt`（+7：HOST_ACCEPTANCE_SRCS 注册 boot_auto_connect.acceptance.c，置于 host smoke 之前）。
  - 新增：`firmware/main/test/boot_auto_connect.acceptance.c`（6 TEST_CASE 冻结验收件，1047 行，含 MUTATION REPLAY MAP）；`firmware/main/test/acceptance/red-baseline/k6_red_wrapper.sh`（通道②独立线束脚本，与 K5 ts_red_r4_wrapper 同款）。
  - delivery：taskpackage.json（本包 finalize + current/ 指针）、input-brief 双件、pod-events.jsonl（5 事件）、verify-runs.jsonl（4 轮）、verifier-evidence/rounds/{r1,r2,r3,r4}.json、mutation-evidence/（六件，见下）。
- **Net diff**：实现面 +119/-4（application.cc）+9（application.h）+7（CMakeLists）；验收件 1047 行新增。
- **Scope**：firmware 单文件簇内聚（application.cc/h + host 接线），零新源文件、零协议层触碰——crossEnd exempt（复用既有 hello 握手零新消息形状，server/front 零改动）。
- **Split recommendation**：单包，不拆。

## Verification Evidence

- **Structural gates**：scoped r3 六闸全绿（`delivery/taskpackages/260923-m3-k6-boot-auto-connect/verifier-evidence/rounds/r3.json`，invocationId `7620237c-4fda-45a6-8c7d-90c266dc1aee`）+ final r4 九闸全绿（r4.json，invocationId `16097926-2c4b-456a-aff5-ef971882ca77`，ok:true / observedGateExit:0）：taskpackage-required、firmware-build（真编译，xiaozhi.bin 0x30e700 bytes，app 分区 0x370000，**0x61900 bytes = 11% free** 体积余量在案）、contract-lint、acceptance-coverage（K6-A1..A7 七条全覆盖）、mutation-check、docs-sync（false + reason 同口径）、security、unit（95 passed）、acceptance（41 structural 齿 passed + oracle 面 13 firmware/3 server 验收文件在世）。
- **机械 fact 校验**（integrator 实跑 2026-09-23）：`verifier-evidence-check.mjs --change-id 260923-m3-k6-boot-auto-connect --phase final --round 4 --invocation-id 16097926-2c4b-456a-aff5-ef971882ca77` → exit 0 / factOk:true；补跑 `--phase scoped --round 3 --invocation-id 7620237c-4fda-45a6-8c7d-90c266dc1aee` → exit 0 / factOk:true（K5 同款双查口径）。**closeout 判断以 r4 final fact（ok:true）为唯一承重机器件**；本目录 verdict.json = r4.json 字节级副本（脚本 fs-write 复制，非 agent 手写），verification.json 本包未另立——缺失按 closeout SSOT 为 advisory-only，不阻塞。
- **Acceptance/unit/e2e**：host runner 全量第一方复跑 **98 Tests / 0 Failures / 0 Ignored，exit 0**（2026-09-23 integrator 实跑，espidf_wsl.sh build ninja no work to do + host_infra_test.elf run——既有 92 齿与上游锚 92/0 @ b869658 零偏移 + 本包新齿 6 TEST_CASE 全绿）；device build 闸过（真编译，见上体积证据）；真机 e2e 半 **deferred**（见「未覆盖面申报」）。
- **Security gate**：r4 绿（securityReviewRequired:false）。新代码无网络输入解析（只读 NVS Settings）、无新凭据面（token 只读比对非空）、无注入面；ESP_LOG 无 secret。
- **Mutation check（K5 留根欠账① 本包闭环）**：mutationRequired:true，七条 acceptance 全部 sensitive:true 登记于 taskpackage.json `mutationEvidence.perAcceptance`。**integrator 第一方实跑六组变异（2026-09-23，MUTATION REPLAY MAP 逐字手法；原始输出逐字落 `delivery/taskpackages/260923-m3-k6-boot-auto-connect/mutation-evidence/` 六件，文件头两行注明执行时间与「第一方实跑非转录」）**：每组 = 变异编辑 → 通道②独立命令（k6_red_wrapper.sh：gcc -std=c11 -Wall -Wextra 编译 + unity stub runner 实跑）见红 → 字节级备份回拷还原（sha256 复核一致）→ 复跑确认 6 Tests / 0 Failures 全绿。逐组红 tooth 与 perAcceptance expectedRed 对照全部命中（详见下节）。验收文件 `*.acceptance.*` 本体全程零触碰（整轮仅 Edit application.cc，还原经 sha256 校验）。
- **Docs sync**：docsSyncRequired=false，r3/r4 闸绿；详见本目录 docs-delta.md。
- **S2 official exploratory scan**：TaskPackage 未声明 `s2ExploratoryRequired`，不执行，无 S2 证据义务。
- **UI design-review 证据**：不适用（本包无新 UI 面——boot UI 序列不变是**负向约束**，由 Connecting 计数恒 4 负 needle + 真机目测 K6-A7① 承载，渲染/版式链路零触碰）。

## Mutation replay 逐组对照（expectedRed ↔ 实测红 tooth）

| 组 | 变异内容（application.cc） | 实测红 | expectedRed 对照 | 证据文件 |
|---|---|---|---|---|
| M1 | 摘触发点 1 接线（HandleActivationDoneEvent 末尾 TryStartAutoConnect() 调用删除） | TEST 1 红 @ tooth 8「HandleActivationDoneEvent 区未接 TryStartAutoConnect」（6 Tests / 1 Failures / RUN_EXIT=1） | ✅ 命中 K6-A1「触发点接线敏感 tooth」 | m1-trigger-point1-removed.txt |
| M2 | 摘 OnNetworkError lambda 拦截分支（in-flight if 块删除） | TEST 2 红 @ tooth 3「无 auto_connect_in_flight_ 拦截分支」（1 Failure / RUN_EXIT=1） | ✅ 命中 K6-A2「拦截分支 needle」 | m2-error-intercept-removed.txt |
| M3 | 预检只读形参改写：`Settings("websocket", false)` → `true` | TEST 3 红 @ tooth 2「缝区无 Settings(\"websocket\", false) 只读预检」（1 Failure / RUN_EXIT=1） | ✅ 命中 K6-A3「只读守卫 tooth」 | m3-prescreen-readonly-broken.txt |
| M4 | 删 task 唯一共享清点（`auto_connect_in_flight_ = false` 摘除） | TEST 4 红 @ tooth 3「task 区无清点」+ TEST 2 红 @ tooth 9「缝区 flag < 3 次下界破」（2 Failures / RUN_EXIT=2） | ✅ 命中 K6-A4「生命周期逐路径 tooth」；TEST 2 tooth 9 为同源计数下界共红（缝区查/置/清三态共享载体，map 判定注记预期内——实现为单共享清点，M4 头注「多出口各自清则单路径摘除可能不降计数」的敏感度缺口在本实现形态下不触发） | m4-inflight-clear-removed.txt |
| M5 | 摘 HandleToggleChatEvent call site 守卫（四区之一） | TEST 5 红 @ ToggleChat 区守卫齿（循环首区）（1 Failure / RUN_EXIT=1） | ✅ 命中 K6-A5「call site 守卫 needle」 | m5-togglechat-guard-removed.txt |
| M6 | 摘 OnAudioChannelOpened lambda 体 Schedule() 包裹（回调体直排） | TEST 6 红 @ tooth 6「lambda 体未包 Schedule()」（1 Failure / RUN_EXIT=1） | ✅ 命中 K6-A6「Schedule 包裹 needle」 | m6-schedule-wrap-removed.txt |

每组还原后复跑均 **6 Tests / 0 Failures / RUN_EXIT=0**（全绿确认随各证据文件「RESTORED, GREEN RE-RUN」段逐字在案）；六组全红 = 齿对被测语义敏感、非恒绿。K6-A7（真机锚）无 host 齿，其变异敏感度由 K6-A1 触发点变异（M1）承载，真机实跑挂设备轮窗口（perAcceptance expectedRed 明文，deferred 非豁免）。

## Demo 对账行（workstream 包硬要求）

本包 demo 证据形态 = **WSL host runner 全量 98 Tests / 0 Failures / exit 0（integrator 第一方实跑 2026-09-23）+ device build 闸（xiaozhi.bin 过、0x61900 bytes = 11% free 余量在案，r4 final）+ 通道②新齿 6/0 绿（mutation replay 每组还原后复跑全绿 + host runner 含 6 新 TEST_CASE）**。与本包效果段「设备开机激活完成后自动与 Front 建立 WS 连接（网络重连自动恢复），使 server『设备联网时』推送触发与提醒投递链开机即生效」的一致性结论：host 侧可机判半（双触发点接线、守卫三条件、in-flight 生命周期、静默失败拦截、退避决策表形态、人工路径守卫、Schedule 包裹、零 Connecting 回归负 needle）已全齿覆盖并绿 + 六组变异自证敏感；device build 证明确可编入真机镜像且无资源回退；效果段的**实证半**（开机串口见 WS connect + hello 交换、front 日志见建连推送发出、P1 显示「接下来」、wifi 闪断自动恢复、server 宕机静默零错误卡、task 栈高水位）= K6-A7 四子锚，依赖真机观察，已在 exitCriteria K6-E3 显式登记挂设备轮窗口（human 在场、单一长捕获 + 就绪信号开窗，不撞车纪律）。**真机半未跑 = deferred 状态如实申报，Pod 不等真机，host 半收口先走合并门**——计划内 deferred，不是证据缺失。

## 未覆盖面申报（workstream 包硬要求）

对照 acceptance[] K6-A1..A7 七条 + contractRefs（interface-contract.md）+ roadmap 效果段（S-F1.1 推送三触发）逐条：

| 项 | 设计性质 | 现状 | 去向 |
|---|---|---|---|
| 重试退避实时时序（5s/10s/20s 真实节奏） | host 齿只钉决策表形态（封顶 3 次 + 递增序列，K5 留根②纪律），实时时序 host 不可机判 | host 半已绿（M2/M4 变异自证表形态敏感） | 真机窗 K6-A7②③（wifi 闪断重连 + server 宕机开机场景实测节奏） |
| boot UI 序列真机目测（首页稳落、无 VOICE 层推入） | host 半由 Connecting 计数恒 4 负 needle 承载（防语音轮 UI 接管回归的机判代理），真机观感不可 host 判 | host 半已绿（tooth 1 恒 4 + M1 组六齿全绿零偏移） | 真机窗 K6-A7①（冷启动无按键目测） |
| 「所有退出路径必清 in-flight」逐路径穷尽 | host 结构齿钉清点在世 + 三态下界（查/置/清），控制流逐路径穷尽 host 扫描无法承载（验收件头注强度申报显式声明该缺口） | 结构齿绿 + M4 变异自证清点敏感 + verifier 逐路径核对承载（r3/r4 闸绿；实现为单共享清点 = 成功/重试耗尽/bail 三路径汇同一点） | 真机泄漏表象（in-flight 泄漏 → 按键永久 no-op）归 K6-A7 窗口可观测 |
| K6-A7③ server 宕机开机（静默落首页、零错误卡、此后人工建连可用） | 真机网络置错锚（host 半：拦截分支 + MAIN_EVENT_ERROR 前序齿已绿） | host 半已绿（M2 组自证敏感） | 设备轮窗口（K6-E3） |
| K6-A7④ AutoConnect task 栈高水位无溢出 | 真机栈水位观测锚（host 半：uxTaskGetStackHighWaterMark 日志结构齿已绿，沿用 activation 模板 4096*2） | host 半已绿（tooth 在世） | 设备轮窗口（K6-E3） |
| 症状 B：课表页冷缓存（开机直切课表页空态） | 周网格缓存唯一填充源是语音轮 display tool call，三推送触发均不推周网格——超出本包范围（human 2026-09-23 拍板 A-only） | **不在本包**（plan.md 范围裁定明载） | human 拍板另走设计入口（可能触契约），独立立项 |
| IsTimeout 120s 覆盖式重连（Idle 通道开无流量 >120s 自报超时后覆盖式重连） | 既有行为非新缺陷（websocket_protocol.cc !IsTimeout() 既有语义） | **不修**（plan.md 边角 + K6-E2 scope 纪律明文「不顺手修」；冻结齿 TEST 6 tooth 4 在世守卫） | 无（既有行为保持；真机锚 7-② 避开该路径） |
| contractRefs / roadmap 效果段其余面 | 契约面零触（建连时机不进契约 §1.3⑥）；front 建连推送为既有在役能力 | **none**——契约零触碰 + contract-lint 两轮绿 + 冻结齿 canary PASS；无其他缺口 | 无 |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| NIT | judgement-call | 治理过程 | mutation 实跑欠账跨棒：K5 留根①「mutation 执行面粒度差跨三棒（设计态红≙变异态在案、实跑未落仓内 evidence）」延至本包才闭环（本包 integrator 第一方实跑六组落盘销账） | K6 taskpackage.json notes ① 明文登记 + K5 review-packet R4 addendum 第④节申报；本包 mutation-evidence/ 六件为闭环证据 | 无（本包已闭环，跨棒欠账销账；后续包 builder/收尾相位按 MUTATION REPLAY MAP 常态实跑即可） |

Findings 说明：五轴 review 全执行（见下节），除上表 1 条 NIT 外无 BLOCKING / SHOULD。结构闸 PASS/FAIL 不复述为 finding。

## Five-Axis Review

1. **Correctness（对每条 K6-Ax 的边界核对）**：K6-A1——守卫三条件齐备且「判+起」在主循环 handler 内（双触发点均为 Run() 事件 handler，TryStartAutoConnect 同步执行于 handler 栈，TOCTOU 判设闭合）；task 化沿用 activation 模板（栈 4096*2、优先级 2）；自动路径零 SetDeviceState（tooth 1 计数恒 4 承载）。K6-A2——拦截分支位于 MAIN_EVENT_ERROR 置位之前（tooth 4 位序钉）；人工路径错误路由零改动（MAIN_EVENT_ERROR 仍在世，tooth 2）；退避 {5,10,20} 表 + task 内真消费（tooth 5-8）。K6-A3——预检只读形参逐字对齐 websocket_protocol.cc 读形态（tooth 2）+ url/token 同检 + 空检查先于 xTaskCreate（tooth 4 no-op 前序）+ 缝区零写 API（tooth 5）。K6-A4——单共享清点覆盖成功/重试耗尽/bail 三路径（逐路径核对：task lambda 三出口全部汇至 break/循环尽后同一清点行），M4 变异自证敏感。K6-A5——四 call site 守卫齐备 + WakeWordDetected 守卫先于 EncodeWakeWord（序钉）+ 各区既有 Connecting/OpenAudioChannel token 在世（语义零改动锚）。K6-A6——Schedule 包裹 + 注册序（smoke 之前）+ 既有齿注册在世 + protocol.h 接口面 canary。一处 plan 边角实现化申报已复核：成功路径无状态迁移（Idle→Idle no-op，OnAudioChannelClosed → Schedule(Idle) 结构性无抖动）——与 plan 方案形状一致，✓。
2. **Test quality**：6 TEST_CASE 全结构扫齿（真实工作树扫描，root 自手持 cwd 定位 + 扫描器自检防假绿 tooth + 需求契约 canary）——针脚位次（拦截先于置位、空检查先于 spawn、守卫先于 EncodeWakeWord）均有独立序钉 tooth；负向齿自证能红（设计态红基线 6/6 + 六组变异每组 exit≥1）；MUST-STAY-GREEN 半（Connecting 恒 4、既有语义 token、protocol 零触碰锚、既有注册面）在红基线与六组变异中全部保持绿。断言源文件字面锚 + 位次 + 下界计数，非实现镜像。
3. **Contract/boundary safety**：`docs/contracts/**` 零触碰（git status 实证 + contract-lint 两轮绿 + TEST 6 tooth 2 canary）；spec 零改；server/front 零改（`git diff --name-only | grep server/` 零命中）；device_state_machine.cc 零触碰、protocol.h 接口签名零改（零 diff 实证 + TEST 6 tooth 8 在世锚）；按键/唤醒人工路径仅加守卫（既有分支 token 在世锚四区全绿）；IsTimeout 120s 既有行为不顺手修（tooth 4 守卫 + 零 diff）。
4. **Operability/security**：AutoConnect tag log 足迹钉住（in-flight 期人工交互 no-op 可观测，plan 边角已知代价 ~35s 窗口）；task 退出栈高水位日志（真机锚 ④ 的 host 侧对照锚）；预检失败单条 log 静默 no-op（未配网不噪音）；无网络输入解析面、无凭据写入、ESP_LOG 无 secret；Settings 只读纪律（tooth 5 零写 API 守卫）。
5. **Maintainability/simplicity**：零新源文件——缝逻辑单点落在 application.cc `TryStartAutoConnect()`（预检/守卫/置标/task 四段一函数），四 call site 只加三行守卫不复制逻辑（deletion test：删 TryStartAutoConnect，建连时机语义无第二承载 = 单点可退役面，正确）；决策表命名常量独立于消费（mutation M4 路径证实表/消费同区耦合可机判）；无死代码、无投机参数化。`minimal:` 标记 grep 本包变更文件零命中——Simplification Debt 台账为空。

## E2 铁律逐项复核（K6-E2）

| 铁律项 | 证据 |
|---|---|
| `docs/contracts/**` 与 spec 零改 | `git status --porcelain docs/` 零输出；contract-lint r3/r4 绿；TEST 6 tooth 2 canary PASS |
| server/front 零改 | `git diff --name-only \| grep server/` 零命中；front_app.py 零 diff |
| device_state_machine.cc 零触碰 | 零 diff（不在变更面清单）；冻结齿 TEST 1 tooth 1 计数锚（Connecting 恒 4）PASS |
| protocol.h 接口签名零改 | 零 diff；TEST 6 tooth 8 OpenAudioChannel 接口面在世 PASS |
| 按键/唤醒人工路径语义零改动（仅加守卫） | 四区既有 `SetDeviceState(kDeviceStateConnecting)` + `OpenAudioChannel()` token 在世锚全绿（TEST 5）+ 通道①既有 92 齿零偏移（98/0 中 92 与上游锚 b869658 一致） |
| IsTimeout 120s 覆盖式重连不顺手修 | websocket_protocol.cc 零 diff；TEST 6 tooth 4 `!IsTimeout()` 在世守卫 PASS |

## Detect Changes / 影响面核对

- detect_changes（codebase-memory `KidPal`）：**N/A (worktree 未提交 diff)**——本包改动全部为未提交工作树 diff，工具对 committed range 生效。替代核对（如实记录）：① `git diff --stat` 全量在案（实现面 +135/-4 三文件 + 验收件 1047 行新增，见 Change Size）；② 关键共享符号 trace_path 抽查——`Application::HandleNetworkConnectedEvent`（本包触发点 2 所在函数）inbound 调用者恒 2（`Application::Run` / `app_main`），无预期外调用面；③ 变更面文件级核对——新增文件全部为本包验收件/线束/制品，修改文件全部在派单变更面清单内，无清单外文件。图索引 re-index 需求（TryStartAutoConnect 新符号未入图）交主线程/human 定窗口，不阻塞本包。

## Residual Risk

- decisionCoverage（taskpackage.json）核验：字段为 null——**无 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目**，无需列示 human 已接受风险项。
- Known limitation：
  - K6-A7 四子锚真机半 deferred 挂设备轮窗口——host 证据不构成真机实证，设备轮收口前不宣称效果段实证闭环（合并门判读时请把「真机半未跑」计入门控权重：本包 riskTier medium，效果回潮风险 = 开机不建连回潮到立项前症状，属可观测、低危害类）。
  - in-flight 期间人工交互 no-op 窗口（单次尝试最长 ~10s、最坏 ~35s）——Shape A 已知代价，plan 边角明载，AutoConnect tag log 可观测。
  - 功耗档时序：激活收尾 LOW_POWER 先于建连成功回调 PERFORMANCE——后到覆盖，plan 边角裁定行为可接受（非回归），真机窗顺带目测。
- Follow-up recommended：
  - 设备轮真机捕获按 K6-E3 清单收口（①冷启动无按键 / ②wifi 闪断 / ③server 宕机 / ④栈高水位），落 progress.md K6 段落账 + pod-retro.md（planner closeout 相位义务）。
  - 真机轮后若出现慢速探活需求（重试耗尽后无周期兜底），plan 已裁定留后续独立小包，不入本包。
- Human decision needed：无（human 回席补 Shape A 一句话 ack 属立项留痕义务、非本包新决策点；真机窗口排期属既有设备轮协议）。

## 回滚方式

单 commit 收口（SHA 见交付报告）；回滚 = 分支上 `git revert <sha>`（单 commit 自含实现 + 验收 + 制品，无跨 commit 依赖）；上游锚 92 齿 @ b869658 可独立复跑比对零偏移；实现面回滚等价于回到 K5 收尾态（application.cc/h 的 K6 段全部带 [K6]/M3-K6 注记锚，逐段可辨）。
