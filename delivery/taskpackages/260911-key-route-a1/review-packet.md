# Review Packet — 260911-key-route-a1（PKG-1 键位路线落地 · KEY(GPIO18) 三手势接线 + 全局路由 dispatch + BOOT 回归翻页）

> integrator 产出 · 2026-09-11 · 分支 `dev/260911-key-route-a1`（base = main `d087fb7`）。
> 供 supervisor 合并门复核。本包 `riskTier=high`（host-teeth / device-round / state-machine gateProfile），照常触发 supervisor 升级人类。
> 交付本体：板级 KEY(GPIO18) 接线（`iot_button` 三手势注册，消抖复用 `CONFIG_BUTTON_SHORT_PRESS_TIME_MS=180`）→ 喂已冻结 `key_gesture_parser`（设备侧首次接线，现状零调用）→ 全局路由表落地（`keya_dispatch_global_action` 单一 dispatch 权威：KEY 单击=对话 / KEY 长按=回首页 / BOOT 单击=翻页）→ BOOT 回归「主司翻页」去对话化（BOOT 双击过渡移除、BOOT 长按故事翻屏保留）。
> **合并门必读**：本包无 BLOCKING finding。承重 final fact `r3` **非全绿**（`ok:false`），唯一红 = `unit`/`acceptance` 两闸同源的 `test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（SMK-A4）——**在册 existing-red-inventory R1**（owner=260829-smoke-e2e，`screen.md` 证据件缺口），与本包零相关、零新增红。host runner 基线 4 条红（WSA-A6:830 / WSA-A4:663 / P2-A1:571 / VRT-A2·E2:878）均为在册既存红。故合并门需**按红证据 ESCALATE 通道**提 human 裁定「留红如案、红转交各自 owner」，本包自身面（scoped 7 闸 + 5 新齿 + mutation 5 条 + KR-A5 真机轮）全绿。

## Change Size

- Files changed（repo 侧，`d087fb7..HEAD` 收拢后）：
  - **实现面 = 3 文件 +135/−31**（`git diff --stat` 实测）：
    - `firmware/main/boards/waveshare-s3-rlcd-4.2/config.h`（+4）：`KEY_BUTTON_GPIO = GPIO_NUM_18` 定义 + 两键分工/有效电平低注释（`BOOT_BUTTON_GPIO = GPIO_NUM_0` 保留，两键不抢占脚位）。
    - `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`（+135/−31，净 155 行）：`key_button_` 成员 + `boot_gesture_`/`key_gesture_` 双 parser 实例；`keya_now_ms` / `keya_dispatch_global_action`（三枚举 dispatch 消费点）/ `keya_log_gesture`（parser 一致性观测）三静态函数；`InitializeButtons()` 重做（parser init + 双键 `OnPressDown`/`OnPressUp` raw 边沿喂入 + KEY 三手势回调 + BOOT 单击去对话化 + BOOT 双击过渡移除）；构造器 `key_button_(KEY_BUTTON_GPIO, false, 0, CONFIG_BUTTON_SHORT_PRESS_TIME_MS, false)`。
    - `firmware/main/test/host_app/main/CMakeLists.txt`（+7）：注册 `../../key_route_a1.acceptance.c`，置于 `host_infra_smoke.acceptance.c` 之前（smoke 末位 = 最先执行，boundary 齿注册序守卫）。
  - **冻结验收件 1 新文件**：`firmware/main/test/key_route_a1.acceptance.c`（701 行，5 TEST_CASE 机制级树扫齿，C 类冻结验收，承 KR-A1/A2/A3/A4/A6 + KR-E2 canary）。
  - **closeout 制品 3 件 + 证据日志 2 件**（本次积分收拢落 `changes/dev/260911-key-route-a1/`）：`review-packet.md` / `docs-delta.md` / `device-round-record.md` + `device-round-keyroute-{boot,gestures3}.log`。
  - dev-loop 执行 ledger/evidence（`.claude/taskpackages/260911-key-route-a1/`）：`taskpackage.json`（活文件）、`input-brief-{acceptance-author,builder}.md`、`pod-events.jsonl`、`verifier-evidence/rounds/r{1,2,3}.json`、`verify-runs.jsonl`。
- Scope: firmware 板级接线面（taskpackage `subsystem` 钉定）；`docs/contracts/**` 与 `docs/spec.md` **零改动**（`git diff --name-only -- docs/` = 空，contract-lint r2/r3 PASS）。
- Split recommendation: **不拆**——板级接线 + parser 喂入 + 全局路由 + BOOT 去对话化是单一键位职责收敛（ARQ-260911-cross-package-behavior-owner-gap 首用试金石），board 文件面单一、无跨文件面纠缠，拆开任一面都断「端到端 owner 显式登记」语义。与 PKG-2（260911-screen-unification，mergeOrder 19）并行、独立文件面，本包先并。

## Verification Evidence

> **承重 fact 声明（诚实口径）**：final completed-only round = **r3**（`phase:"final"`，identity = changeId+phase+round+invocationId），
> `verifier-evidence/rounds/r3.json`：`ok:false`、`observedGateExit:1`、`authority:"bearing"`、`producedBy:"verify.mjs"`、
> `invocationId 940c4bb8-b0a5-4d0e-87f5-7c37dce82759`、`startedAt 2026-09-11T11:21:39.901Z / completedAt 11:22:10.610Z`。
> `ok:false` 的**唯一**成因 = `unit`/`acceptance` 两 full 闸同源红 `test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（SMK-A4，在册 existing-red-inventory R1）；scoped 7 闸**全绿**。此为本包「留红如案」的 machine 记录，非本包自身面红（见 §Residual Risk / §合并门须知）。
> `current/verdict.json` 为同一 `producedBy:"verify.mjs (script fs-write)"` 的投影（non-bearing compatibility projection），closeout 判断以 r3 fact 为准。

- **verifier 逐轮收敛轨迹（r1→r3）**：
  - **r1**（inv `f13ff9e3-…`，scoped，`ok:false`）：唯一红 = `acceptance-coverage`——`missingCoverage:["KR-A1","KR-A3","KR-A4","KR-A6"]`。根因 = 冻结验收初版 marker-format 缺陷（测试名 `(KR-Ax)` 后缀 + tag `[KR-Ax]` 不被 `acceptance-coverage-check.mjs` 的 `coverage:` 标记 regex 识别）。**非实现缺陷**，是冻结验收件声明形态修正（acceptance-author 重派补 `coverage:` 登记行，断言/needle/接缝契约实质零变更，REVISION LOG 在册）。
  - **r2**（inv `6c6a266c-…`，scoped，`ok:true`）：七闸全绿（`observedGateExit:0`），firmware-build 严格模式真编译（`build-pass`，xiaozhi.bin 0x2c28b0）。
  - **r3**（inv `940c4bb8-…`，final）：scoped 7 闸全绿 + `unit`/`acceptance` 两 full 闸红（唯一 SMK-A4）。`unit` 计数 `1 failed/162 passed/6 skipped/45 deselected`、`acceptance` 计数 `1 failed/125 passed/6 skipped/45 deselected`，与 launch 基线逐字一致，**零新增红**。
- **r3 逐闸（实测 ms）**：taskpackage-required 213 / **firmware-build 14792（`build-pass` 真编译，非降级——ninja `[1/5]…[5/5]` 全跑、`xiaozhi.bin 0x2c28b0`、`0xad750 (20%) free`）** / contract-lint 144 / acceptance-coverage 165（acceptance KR-A1..A6 全在册、`structural:["KR-A5"]`、`suiteGates:[]`、files=[key_route_a1.acceptance.c]）/ mutation-check 160 / docs-sync 146（`docsSyncRequired:false`）/ security 151（`securityReviewRequired:false`）。
- **host runner 全量复跑（integrator 独立实跑，2026-09-11，WSL linux target，cwd=host_app）**：`wsl … ./build/host_infra_test.elf` → **109 Tests / 4 Failures / 0 Ignored**（exit 4）。
  - **本包 5 新齿全绿**：KR-A1（板级 KEY GPIO 接线）/ KR-A2（parser 喂入）/ KR-A3（全局路由表）/ KR-A4（BOOT 去对话化）/ KR-A6（注册序守卫+spec/契约 canary+扫描器自检）——`grep` 逐行 capture 见全部 `:PASS`。
  - **session_logic 全齿绿**：SES-A1~A6 全 PASS（冻结纯逻辑零改动证明）。
  - 4 条红 = 在册既存红，逐行 `:FAIL` 定位：`voice_roundtrip.acceptance.c:878`（VRT-A2·E2）/ `device_ws_switch.acceptance.c:830`（WSA-A6）/ `device_ws_switch.acceptance.c:663`（WSA-A4）/ `schedule_p2.acceptance.c:571`（P2-A1）。**零新增红**。
  - 有效基线口径（planner 裁定，`redBaselineResult`）：104/4（launch 口径 104/3 漏列 VRT-A2/E2 一条，两 worktree 证据文件红同态）→ 本包接线后 109/4 = 基线四条 + 5 新齿，**本包责任 = 相对基线零新增红 + 新齿全绿，均满足**。
- **设备构建产物在场核验（integrator，不重编）**：`firmware/build/xiaozhi.bin` 存在，2894000 字节 = 0x2c28b0（与 r3 firmware-build `build-pass` 逐字一致）。设备真编已由 r1/r2/r3 firmware-build 严格模式三次真过。
- **KR-A5 真机确认轮（human 持板 PASS，pod-events `c0ffe62a-70f3-4080-9945-7b1877459008`）**：COM5 烧录 app@0x20000（hash verified + hard reset）后，KEY 三手势/BOOT 翻页/连按消抖/零自触发全部日志互证；active_high=false 实测稳定、悬空风险销账；HWV-A2 顺带三项核验。详证据 = `device-round-record.md` + 两份归档日志（本目录）。
- **mutation 敏感实测 5 条（builder 实测，落 `taskpackage.mutationEvidence.perAcceptance`）**：KR-A1（GPIO18→17 单点）/ KR-A2（feed→feed_stub 4 处）/ KR-A3（SESSION_TOGGLE→SESSION_STUB 3 处）/ KR-A4（BOOT 单击恢复 HandleStoryKey 对话路径）/ KR-A6（删注册行）全部 `sensitive:true`、`turned red`，还原复跑绿（109/4 基线）；KR-A5 structural N/A（GPIO18 接线敏感性由 KR-A1 mutation 承载，真机证据门 = device-round 相位）。
- **Security gate**：r3 `ok:true`（`securityReviewRequired:false`）。本包无新公网入口、无新 secrets（板级 GPIO 接线不涉网）；新增 `keya_now_ms()` 仅封装 `esp_log_timestamp()`、`keya_dispatch_global_action` 走既有 `Application::Schedule` / `ToggleChatState` 任务上下文，无新 IPC。
- **Docs sync**：r3 `ok:true`（`docsSyncRequired:false`）。判定与逐条理由 = 同目录 `docs-delta.md`（canonical 零改动 + `docs/contracts/**`/`docs/spec.md` 零触碰 + 实现侧注记随代码注释承载）。**不改 docs/contracts/**。
- **Closeout**：本 phase 跑 `node .claude/scripts/closeout-check.mjs --update-index` → 结果见 §Closeout。
- **影响面核对（step 11b）**：Pod worktree 内 diff 对索引不可见 → `detect_changes` 如实标注 N/A (worktree)，以 `git diff --stat`（实现面 3 文件 +135/−31）+ 关键共享符号 `trace_path` 抽查替代。① `trace_path key_gesture_parser_feed`（inbound，主仓历史面）→ `callers_total: 2`（`CustomBoard` / `InitializeButtons`，均在本包板级文件 `waveshare-s3-rlcd-4.2.cc` 内）——证「设备侧首喂」从冻结前零调用 → 二调用点；② `detect_changes`（impact，base main）→ 实现面改动 `seed_symbols: 33`、`impacted_total: 1`，唯一 impact = 本包 `key_route_a1.acceptance.c` 树扫模块（非生产代码调用者）——板级接线零波及既有生产符号，冻结 parser/route 纯逻辑 `.cc/.h` 只被新增调用、零修改，改动只影响预期符号。

## Simplification Debt

> 本包实现面 = 3 文件接线 + 1 新冻结验收件，**无新抽象、无新依赖、无 `minimal:` 标记**（`grep` 全仓 `minimal:` 命中 0 处为本包新增，均非本包引入）。故本表 **无条目、无 no-trigger 项**——诚实声明「无简化债务」而非伪造一条。本包删的是冗余（BOOT 双击既有一键过渡 `OnDoubleClick→cycle_next` 移除——冻结路由表 BOOT DOUBLE_CLICK 恒 `GLOBAL_ACT_NONE`，过渡冗余且无冻结齿钉之），是 `delete` 语义、不留债。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | judgement-call | final 闸红（留红如案，跨包） | **SMK-A4 final 闸红（不伪造不代修）**：r3 `unit`/`acceptance` 两闸唯一红 = `test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（`server/acceptance/_artifacts/smoke-e2e/screen.md` 缺失 → 无屏上文字证据）。与本包板级接线零相关（本包不产 display-line 证据） | Basis: final r3 fact `ok:false` stdout（`AssertionError: SMK-A4 RED: …screen.md missing`）+ existing-red-inventory R1（owner=260829-smoke-e2e，`[evidence]` 族先行红）；heuristic: `[evidence]` 族红非本包冒烟面，不越权代采 | 留红如案、提 human 按红证据 ESCALATE 通道裁（红转交 owner=260829-smoke-e2e：重新采集一次真实屏上文字证据或裁「物理冒烟证据以当时记录为准」）。本包不代修 |
| FYI | judgement-call | host runner 基线红（留红如案，跨包） | **host 基线 4 条红（不伪造）**：109/4 中 4 条红 = VRT-A2·E2:878 / WSA-A6:830 / WSA-A4:663 / P2-A1:571，全部为在册既存跨包红，非本包责任、本包不修、相对有效基线 104/4 零新增 | Basis: host runner 实跑 `:FAIL` 逐行定位（voice_roundtrip.acceptance.c:878 / device_ws_switch.acceptance.c:830/663 / schedule_p2.acceptance.c:571）+ existing-red-inventory R2/R3/R4 + redBaselineResult planner 裁定口径 | 留红如案，基线消解走独立批（launch 明令），不塞本包 |
| 观察项 | judgement-call | 渲染节点页序（归 PKG-2 参考，非缺陷） | **渲染节点循环序 `home→char→schedule→story`**：BOOT 单击 `action=3` 经 `kidpal_render_node_cycle_next()` 的渲染节点页循环是 home→char→schedule→story，与 spec §4.0 冻结纯逻辑页序（P1→P2→P3→P4）不同 | Basis: gestures3.log 逐次 `cycle -> CHAR/SCHEDULE/STORY/HOME` + 代码 hunk `keya_dispatch_global_action` `case GLOBAL_ACT_PAGE_NEXT: Schedule(cycle_next)`（本包只接线，未动渲染节点页序）；heuristic: 页序差异在既存 `cycle_next` 渲染实现，非本包 diff 引入 | 归 PKG-2（260911-screen-unification）归一屏页面体系重构时参考/对齐，本包不开行动 |
| 观察项 | judgement-call | 显示仲裁（归 PKG-2 参考，非缺陷） | **语音 active 期 HOME dispatch 后前台让位 SetupUI**：语音会话活跃期间 KEY 长按回首页后，前台按既有仲裁仍让位 xiaozhi SetupUI（语音态优先） | Basis: gestures3.log L55 `pushing home` 后 L56 `voice active -> show xiaozhi SetupUI (yield KidPal)`（L62→L63 复现）+ 代码 hunk `GLOBAL_ACT_HOME → app.Schedule(show_home)` 未改变显示仲裁优先级；heuristic: 既有仲裁行为非本包引入 | 归 PKG-2 归一屏「仲裁器显式化」参考，本包不开行动 |

**Kind 说明**：四行均 `judgement-call`（无一条是本包 repo diff 引入的 hard-violation；行 1/2 为在册留红如案、行 3/4 为观察项归下游包）。无 BLOCKING → 不停合并门；但 final fact `ok:false` 触发红证据 ESCALATE（见 §合并门须知）。

## Five-Axis Review

1. **Correctness（对 KR-A1~A4/A6 冻结接缝契约 + §4.0 两键分工）**：逐条兑现 260911-key-route-a1 冻结验收件头注「Seam contract [1]-[5]」——[1] config.h `KEY_BUTTON_GPIO→GPIO_NUM_18` + BOOT `GPIO_NUM_0` 保留；[2] `key_button_` 构造 active_high=false + short_press 复用 180 + 三手势注册；[3] 双 parser `init`+`feed`（KEY/BOOT raw 边沿）+ `key_route_global` 两键调用 + `GLOBAL_ACT_SESSION_TOGGLE/HOME/PAGE_NEXT` 三枚举设备侧 dispatch 消费；[4] BOOT 单击去对话化（移除 `HandleStoryKey(KEY_ID_BOOT, CLICK)`）+ BOOT 长按故事翻屏保留；[5] acceptance.c 注册进 HOST_ACCEPTANCE_SRCS 且置于 smoke 前。全部 6 齿（KR-A1 6 齿/KR-A2 3 齿/KR-A3 5 齿/KR-A4 2 齿/KR-A6 4 齿）host 实跑 PASS，是「接线逐字在树上被树扫齿命中」的直接证据，非「跑了没报错」。`key_gesture_parser` 只作一致性观测、dispatch 唯一落在 `iot_button` 手势回调（避免 iot_button 180ms 短按窗与 parser 350ms 双击窗消歧分歧的重复触发），该设计决策注释钉死在 `keya_log_gesture` 头。
2. **Test quality**：本包冻结验收是**机制级（符号/接线级）tree-scan 齿**而非行为级（ARQ-260911-tooth-strength 教训——防「BOOT≈KEY 行为等价」假绿：实现者把 KEY 语义旁路挂 boot_button_/GPIO0 时 KR-A1 tooth 3 仍红）。mutation 5 条敏感实测全 `turned red` → 还原绿（KR-A1 GPIO18→17 单点即破、KR-A2 feed 4 处 stub 即破、KR-A3 dispatch 3 处置换即破、KR-A4 去对话化回退负向齿即破、KR-A6 删注册即破），证齿不是「总是绿的摆设」。session_logic 全齿绿 = 冻结纯逻辑零改动。测试 5 tooth 3 扫描器自检（负向 needle 必须能红）防假绿。
3. **Contract / boundary safety**：`docs/contracts/**` 与 `docs/spec.md` 零触碰（`git diff --name-only -- docs/` 空 + contract-lint r2/r3 PASS + test 5 tooth 2 canary 钉住）。本包路由语义**不新造**——手势→动作权威判定单一落在冻结 `key_route_global`（`key_gesture_parser.h`），设备侧 `keya_dispatch_global_action` 只做「路由输出→应用动作」的机械映射消费，不另造一套 KEY 语义。`*.acceptance.*` 冻结件只读（接受-author 单次 marker-format 修正，断言零变更）。
4. **Operability / security**：新增 3 静态函数（`keya_now_ms`/`keya_dispatch_global_action`/`keya_log_gesture`）局部、无状态泄漏、无动态分配；dispatch 归 `Application::Schedule`（LVGL 屏切换在 main app task，与既有 UI 同任务上下文）；parser feed 返回值仅作 K6 corr_id 日志观测、失败不影响主链。无新公网入口/凭据/依赖。
5. **Maintainability / simplicity**：3 文件改动局部、易懂、无过早抽象、零新依赖（无 requirements/package.json 改动）。BOOT 双击过渡移除是删冗余（`delete` 语义）。无 `minimal:` 标记债务（§Simplification Debt 已声明）。

## S2 Exploratory Scan

TaskPackage 未声明 `s2ExploratoryRequired` / `s2ExploratoryGoals` ⇒ 本相位**未执行** S2 official exploratory scan，无 runId、无 structured `runResult`、无 `knowledgeSuggestion` 待决项。required-by-risk 侧注记：`riskTier=high` 但 planner 未开该面；本包的风险面是 GPIO18 硬件电平/上拉悬空（device-round 相位，human 持板已销账）而非探索式攻击面（板级 GPIO 接线无攻击面增量），planner 已用 KR-A5 真机轮 + mutation 5 条等价覆盖，本 packet 如实记录「未声明 ⇒ 未执行」，不以 builder debug run 冒充 official evidence。

## UI design-review 证据

本包无 UIUX work（TaskPackage 未声明 uiux 字段，改动为板级接线 + host 验收注册，无 viewport/a11y/design 触及；显示层只经既有 `kidpal_render_node_*` 接口消费，不新增 UI 面）。无 UI finding。（两个「观察项」——渲染循环序 + 语音期让位——是既有显示面行为，已归 PKG-2 归一屏参考，不构成本包 UI 缺陷。）

## Residual Risk

- Known limitation / 留红如案（均非本包引入、owner 已登记）：
  1. **SMK-A4 final 闸红**（existing-red R1，owner=260829-smoke-e2e）：`screen.md` 证据件缺口，本包不代采、不伪造，提 human ESCALATE 裁。
  2. **host 基线 4 条红**（VRT-A2·E2 / WSA-A6 / WSA-A4 / P2-A1，existing-red R2/R3/R4 + VRT owner）：在册既存跨包红，本包有效基线 104/4 → 109/4 零新增，基线消解走独立批。
- Follow-up recommended（归下游/另行立项，非本包行动）：
  - 渲染节点循环序 `home→char→schedule→story` 与 spec 冻结页序（P1→P2→P3→P4）的差异 → PKG-2（260911-screen-unification）页面体系重构参考。
  - 语音 active 期 HOME dispatch 后前台让位 SetupUI → PKG-2 归一屏「仲裁器显式化」参考。
- Human decision needed：
  - **红证据 ESCALATE**：final fact r3 `ok:false`（SMK-A4 留红如案），提 human 裁「接受本包合并（红转交 owner=260829-smoke-e2e / 独立批基线消解）或先修 R1 再合并」——由 supervisor-verdict-gate 机械闸 + planner 前置升级闸（`pre-merge-escalation-gate.mjs`）承接，本 integrator 不自判。
  - **risk-accepted 台账**：`taskpackage.decisionCoverage` 五条 decision 的 `landsIn` 全部指向 acceptance/exitCriteria，**无 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头条目** → 本包无 risk-accepted 待追认项（决策「accept-risk-d9e0f1a2-KEY路线悬置」已由 OQ4=A 承接为 `landsIn: acceptance:KR-A1`，非 dropped）。

## Closeout

`node .claude/scripts/closeout-check.mjs --update-index`（本 phase 实跑，2026-09-11）→ 结果见本文件 §Closeout 落笔时实跑（`requiredFiles:["docs-delta.md","review-packet.md"]` 两件 blocking 制品在场）。`device-round-record.md` 为 KR-A5 承重 human 证据叙事（human PASS 裁定 pod-events `c0ffe62a` + 日志互证），非 blocking checklist 项但为 device-round 门证据。
advisories（advisory-only，不阻塞按 SSOT）：① `verdict.json`/`verification.json` 投影·advisory（non-bearing，脚本 fs-write，非 agent 落盘；承重 = r3 fact）；② `pod-retro.md` 为 planner 收尾相位产物（非本相位职责）；③ loop-retro pending 计数触达阈值 advisory。
post-cutoff 不产 session-digest / loop-health / loop-metrics / pod-journal；`pod-events.jsonl` 在场（advisory 满足，含 2 条 human-decision：`8561409b` 执行模式 + `c0ffe62a` KR-A5 PASS）。

## Commit 冻结（合并门复核的对象）

- 功能提交 = builder/acceptance-author 已落工作树（板级 2 文件 + CMakeLists + 冻结验收件），积分收拢提交（本次）把 closeout 制品 3 件 + 证据日志 2 件 + dev-loop ledger（pod-events / verify-runs / r1-r3 facts / input-briefs / taskpackage 活文件）收口。commit 前按 AGENTS.md Hard Rule 用 `git diff --cached --stat` 核对；**不 stage `.codebase-memory/**`（机器再生面常驻脏，Pod 分支禁止私带快照 commit）、不 stage `firmware/build/**`、不 stage `current/verdict.json` 机器指针**（投影由脚本 fs-write）。
- 可回滚路径：全部交付物为 feature 分支 `dev/260911-key-route-a1` 上的本地 commit（**未 push、未动 main**）；回滚 = 分支级 revert（接线是单文件面，revert 即恢复 BOOT 一键过渡 + KEY 悬空旧态）。main 零暴露。
- 合并相位：supervisor 机械闸（`supervisor-verdict-gate.mjs`）裁断 → planner 前置升级闸（`pre-merge-escalation-gate.mjs`）→ 放行后 `merge-driver --pod` 本地合并（本包 mergeOrder 18，先并；PKG-2 19 后并时回归 KEY/BOOT 路由）。本 integrator 不自行判定通过、不执行 merge、不 push。

## 合并门须知（短清单）

1. 无 BLOCKING finding；四行 FYI/观察项均为留红如案/下游参考，均不阻断本包自身面。
2. 承重 fact = **r3**（phase final，scoped 7 闸绿 + unit/acceptance red = SMK-A4 在册 R1，`ok:false`）；supervisor 复核读 `verifier-evidence/rounds/r3.json`。
3. host runner 109/4 = 基线四条在册红 + 本包 5 新齿全绿 + session_logic 全齿绿，零新增红。
4. 冻结面（`docs/contracts/**`、`*.acceptance.*`、`managed_components/**`）git 零触碰（KR-E2 canary 在 watching）。
5. **红证据 ESCALATE**：final fact `ok:false` 触发人类裁决通道（风险面 = SMK-A4 留红如案，owner 已登记），由 supervisor 机械闸 + planner 前置升级闸承接，riskTier=high 照常升级。
6. human 待办：SMK-A4 留红如案裁定（owner 260829-smoke-e2e）；host 基线 4 红走独立批消解（launch 明令）；两个观察项归 PKG-2 归一屏参考。均不阻塞本包合并。