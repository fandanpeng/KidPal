# Review Packet — 260915-m1-k1a-key-layer-dispatch（M1-K1a 键位对账块①：覆盖层 BOOT 隔离 + cycle_next 清层/进页分离 + KEY 长按 layer-aware 返回）

> integrator 产出 · 2026-09-15 · 分支 `dev/260915-m1-k1a-key-layer-dispatch`（base = base/m1-home-qa `82a6fb9`）。
> 供 supervisor 合并门复核。本包 `riskTier=high`（host-teeth / state-machine / device-round gateProfile），照常触发 supervisor 升级人类。
> 交付本体：板级 BOOT/KEY 直连拆除（`keya_dispatch_global_action` 硬路由 → 按前台层分流）→ `key_route_layer` 分层路由决策表（覆盖层列：BOOT 单击 = 关层回原页、KEY 长按 = 关层回原页；基层列沿用 `key_route_global`）→ `kidpal_layer_pop_transients` 显式清层步骤与 `cycle_next` 进页分离（拆除「关窗寄生翻页函数」副作用）→ KEY 长按 `GLOBAL_ACT_HOME` 硬路由改 layer-aware 裁决 → KD-A4 验收钉点成批改（`schedule_reminder_b.acceptance.c` 十余处按 v1.39 隔离口径修订，REVISION LOG 留痕）→ 冻结验收 `key_layer_dispatch.acceptance.c`（5 TEST_CASE）接线进 HOST_ACCEPTANCE_SRCS（置于 smoke 前，注册序守卫）。
> **合并门必读**：本包无 BLOCKING finding。承重 final fact **r1 非 full 绿**（`ok:false`），红 = `unit`/`acceptance` 两闸同源的两条 **launch 预报在册 server 基线红**（SMK-A4 + SRE-E4，2 failed / 130 passed / 6 skipped / 45 deselected 与基线逐字一致，零新增）——与板侧 KD-A4 非同一齿。本包自身面（scoped 7 闸全绿 + host runner 零新增红 + 新齿 5 条全绿 + mutation 11 条敏感断言在册）达标。KD-A5（真机确认轮，human 持板）**pending**，见 §Residual Risk。

## 四阶段闭环事实

| 阶段 | 事实 | 锚点 |
|---|---|---|
| intake/planner 派单 | pod 置 running、current/ 指针切至本包、brief 落盘 | pod-events `86c1ec5b`（2026-09-15T04:24Z） |
| acceptance-author | 冻结 `key_layer_dispatch.acceptance.c`（5 TEST_CASE：路由决策表 / dispatch 分流接线 / 清层分离 / 长按映射守卫 / 注册序守卫）+ KD-A4 十处钉点批改（REVISION LOG 2026-09-15）+ mutationEvidence 11 条 + red-baseline-freeze.md（有效基线 **129/5**，本 worktree 复跑落账） | pod-events `0311eddc`（05:39Z） |
| builder | 实现落盘 10 文件 +345/−76（见 §Change Size）；因 **API 配额熔断未交六项回报**，最后一句话自述 =「No residual markers（变异残留清零），下一步验最终树结构断言」——变异执行 provenance 见 §Verification Evidence 专节 | pod-events `61fe7b57`（06:33Z，friction 事件） |
| verifier | verify.mjs 两相位跑通：scoped 7 闸全绿；final 9 闸中 unit/acceptance 两闸红 = 在册基线红零新增；notExecuted = 0；planner 独立复测 host runner 134/4 | pod-events `27d4125e`（08:49Z）+ verify-runs.jsonl 三轮 |
| integrator（本相位） | 承重 bearing final fact r1 发布 + checker PASS + closeout 制品落盘 + 冻结 diff commit | 见 §Verification Evidence |

## Change Size

- Files changed（git diff --stat 实测，base `82a6fb9`）：
  - **实现面 = 10 文件 +345/−76**（`firmware/` 内 diff --stat 汇总）：
    - `firmware/main/key_gesture_parser.cc`（+42）/ `key_gesture_parser.h`（+20/−）：`key_route_layer` 分层路由决策表（`layer_top` 覆盖层列 + 基层列）、`GLOBAL_ACT_OVERLAY_CLOSE` 枚举 append 追加；
    - `firmware/main/display/kidpal_layer.c`（+26）/ `kidpal_layer.h`（+7）：`kidpal_layer_pop_transients` 显式清层步骤（transient 集 = VOICE / REMINDER / SERVER_PAGE，PROVISION 零触碰）；
    - `firmware/main/display/kidpal_render_node.cc`（+92/−）/ `kidpal_render_node.h`（+15）：前台层查询（`kidpal_render_node_front_is_overlay`）/ 关层回原页（`kidpal_render_node_dismiss_front_overlay`）/ `cycle_next` 清层与进页分离（函数区 `kidpal_layer_pop` 计数归零）；
    - `firmware/main/display/reminder_page.c`（+11/−）：`kidpal_reminder_key` 覆盖层裁决（隔离口径：BOOT 单击关层、KEY 长按回 PAGE_P1）；
    - `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`（+90/−）：BOOT/KEY 手势回调直连拆除 → 按前台层分流喂 `key_route_layer`，`kidpal_reminder_key` 设备侧调用点接线（本 worktree 实测板级文件 5 处引用）；
    - `firmware/main/test/host_app/main/CMakeLists.txt`（+7）：`key_layer_dispatch.acceptance.c` 注册进 HOST_ACCEPTANCE_SRCS，置于 smoke 前（注册序守卫）。
  - **冻结验收件**：`firmware/main/test/key_layer_dispatch.acceptance.c`（新文件，5 TEST_CASE 机制级齿）+ `firmware/main/test/schedule_reminder_b.acceptance.c`（+111/−：KD-A4 十处钉点成批修订，REVISION LOG 留痕，断言语义不放松——rmd-B2 `out==PAGE_P1` / rmd-B3 `out==PAGE_P4` / rmd-B4 `out==PAGE_P3` 隔离钉点对实现回退敏感）。
  - **closeout 制品**（本相位落 `changes/dev/260915-m1-k1a-key-layer-dispatch/`）：`review-packet.md` / `docs-delta.md` / `device-round-record.md`（KD-A5 pending 留位，非证据）。
  - dev-loop 执行 ledger/evidence（`delivery/taskpackages/260915-m1-k1a-key-layer-dispatch/`）：`taskpackage.json`（活文件，mutationEvidence 11 条）、`input-brief-{acceptance-author,builder}.md`、`pod-events.jsonl`、`verify-runs.jsonl`、`red-baseline-freeze.md`、`verifier-evidence/rounds/r1.json`（承重 fact）。
- Scope: firmware 键路由派发层（taskpackage `subsystem` 钉定）；`docs/contracts/**` 与 `docs/requirements.md` **零改动**（contract-lint r1 PASS；KD-E2 兑现）。页循环 4→3 / 撤 P4 / P3 键位族零触碰（KD-E1 兑现，板级 `git diff` 无 `show_page(P4)` / PAGE_P4 翻页改动）。
- Split recommendation: **不拆**——分层路由表、清层分离、板级分流接线、钉点成批改是同一隔离口径（v1.39/v1.36）的单一语义收敛，拆开任一面都使「路由决策表 ↔ 设备侧接线 ↔ 钉点」三方失去同批可验性。

## Verification Evidence

> **承重 fact 声明（诚实口径）**：final completed-only round = **r1**（`phase:"final"`，identity = changeId+phase+round+invocationId），
> `delivery/taskpackages/260915-m1-k1a-key-layer-dispatch/verifier-evidence/rounds/r1.json`：`ok:false`、`observedGateExit:1`、`authority:"bearing"`、`producedBy:"verify.mjs"`、`gatesRun:9`、`gatesNotExecuted:[]`（notExecuted=0）、
> `invocationId 7446b951-5e25-4b79-8df2-d466d7a663bf`、`startedAt 2026-09-15T09:05:02.543Z / completedAt 09:05:24.430Z`。
> `ok:false` 的**全部**成因 = `unit`/`acceptance` 两 full 闸同源红 SMK-A4 + SRE-E4（launch 预报在册 server 基线红，见 §红基线判读）；scoped 7 闸**全绿**。
> `current/verdict.json` 为投影（non-bearing compatibility projection）；closeout 判断以 r1 fact 为准。
> 注：verifier 相位三轮（verify-runs.jsonl 08:34/08:35/08:42）为 legacy 投影，未产 bearing fact；r1 由 integrator 以 `--evidence-mode=round --phase=final` 补跑发布——planner 移交口径「final 相位 9 闸」与 r1 结果逐闸一致（7 绿 + 2 红同源在册基线）。
> **checker**：`verifier-evidence-check.mjs` → `{"ok":true,"factOk":false,"evidenceRef":"…/rounds/r1.json"}`——final evidence 结构有效、可进入 closeout。

- **r1 逐闸（实测 ms）**：taskpackage-required 133 / **firmware-build 9052（真编译 `build-pass`，ninja `[1/5]…[5/5]` 全跑、`xiaozhi.bin 0x2c3cd0`、`0xac330 (20%) free`）** / contract-lint 113 / acceptance-coverage 123（KD-A1..A6 全在册、`structural:["KD-A5"]`、files=[key_layer_dispatch.acceptance.c]）/ mutation-check 124（11 条 perAcceptance 与 acceptance[] 逐字节一致）/ docs-sync 120（`docsSyncRequired:false`）/ security 130（`securityReviewRequired:false`）/ unit 6115（红，见下）/ acceptance 5875（红，见下）。
- **红基线判读（KD-A6 判据）**——两层面分述：
  - **host runner（板侧）**：有效基线 129/5（red-baseline-freeze.md 冻结：4 条在册既存红 SU-E3:1010 / KR-A6:636 / WSA-A4:663 / WSA-A6:830 + RMD-A5 义务红设计态）。planner 独立复测终跑 = **134 Tests / 4 Failures / 0 Ignored**：4 红全为在册既存红（同上 4 条），RMD-A5 义务红随实现落地**转绿**，新齿 5 条（KD-A1 决策表 / KD-A2 分流接线 / KD-A3 清层分离 / KD-A3 板级长按映射守卫 / KD-A6 注册序守卫）**全绿** → 「相对有效基线零新增红 + 本包新齿全绿」达标，KD-A6 判据满足（接线后总数 129→134 增长，按基线口径判读、不以总数相等判读）。
  - **server 面（full 闸）**：unit `2 failed, 167 passed, 6 skipped, 45 deselected` / acceptance `2 failed, 130 passed, 6 skipped, 45 deselected`——与 launch 基线逐字一致，零新增。两条红：**SMK-A4**（`test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`，`screen.md` 证据件缺口，owner=260829-smoke-e2e，在册 existing-red）+ **SRE-E4**（`test_schedule_reminder.acceptance.py::test_e4_contract_point3_evidenced_and_poll_form_retired`，spec 的送达语义记录被改动，owner 归 intake 判读，server 面 spec 送达语义记录红）——均与本包板侧 KD-A4 非同一齿、零相关。
- **KD-A5 真机确认轮（kind=structural，human 持板）**：**pending**——本相位时点 human 持板轮未跑，证据目录已留位（同目录 `device-round-record.md`，PENDING 标注，零伪造证据）。这是本包合并门前**唯一未闭合的 structural 义务**，处置建议见 §Residual Risk。
- **Mutation 敏感断言 provenance（诚实义务专节）**：
  - **在册证据**：`taskpackage.mutationEvidence.perAcceptance` 11 条（KD-A1×3 / KD-A2×3 / KD-A3×2 / KD-A4×1 / KD-A6×2，KD-A5 structural 不入变异面），全部 `turnedRed:true`、`sensitive:true`，变异命名纪律合规（全为行为级操作描述，无与原符号共享可识别子串的铸造名——KR-A3 `_TOGGLE_X` 教训）。
  - **provenance 限定（如实记录）**：mutation-check 闸绿 = 机检确认 11 条登记与 acceptance 声明逐字节一致（**结构闸，非执行复跑**）；**11 条 turnedRed 断言的执行证据 = builder 执行后的断链自述（「No residual markers / 变异残留清零」）+ 终树全绿旁证**——builder 因 API 配额熔断未交六项回报（pod-events `61fe7b57` friction 事件在册），**planner 未独立复跑变异**。
  - **可要求独立复验**：supervisor / human 可要求对 11 条变异做独立复跑（每条为单点行为级操作：改 `key_route_layer` 覆盖层列 / 拆板级分流 / 漏 `SERVER_PAGE` 出栈 / 越界清 `PROVISION` / `cycle_next` 回塞清层 / KEY 长按回硬路由 / 板级 HOME 分支改翻页 / `reminder_page.c` 回退隔离口径 / 注销 HOST_ACCEPTANCE_SRCS 注册 / 枚举值偏移，逐条 `affectedTest` 与还原要求已在 mutationEvidence 条目内写明）。在独立复验完成前，本包 mutation 面的证据强度 = 「登记完整 + builder 自述清零 + 终树全绿」，非「逐条实测复红」——与 260829-schedule-reminder 37 条实载先例的强度差如实声明。
- **Security**：r1 `ok:true`（`securityReviewRequired:false`）。无新公网入口 / secrets / 依赖；分层裁决纯函数路由 + 既有 `Application::Schedule` 任务上下文，无新 IPC、无动态分配。
- **Docs sync**：r1 `ok:true`（`docsSyncRequired:false`）。判定与逐条理由 = 同目录 `docs-delta.md`（requirements §2.3 权威矩阵已冻结在文、契约面只读、对账登记更新归主线程对账轮 KD-E3）。**不改 docs/contracts/**。
- **影响面核对（step 11b）**：Pod worktree 内 diff 对索引不可见 → `detect_changes` 如实标注 **N/A (worktree)**，以 `git diff --stat`（实现面 10 文件 +345/−76）+ 关键共享符号 `trace_path` 抽查替代：① `trace_path kidpal_reminder_key`（inbound，主仓索引 = 改前历史面）→ `callers_total: 0`——证改前「正经裁决函数设备侧零调用」状态与 red-baseline C 类实编判读一致；本 worktree `grep` 实测板级 `waveshare-s3-rlcd-4.2.cc` 5 处引用 = 设备侧调用点接线落地（改前 0 处）；② 其余 crossEnd 符号（`key_route_global` / `InitializeButtons` / `kidpal_render_node_begin_page`）调用面均在板级文件内收敛，无跨文件面外溢（`git diff --name-only` 确认改动收敛于上列 10 文件）。
- **Re-index 需求**：主索引（main 侧）尚不含本包新符号（`key_route_layer` 等），re-index 归主线程 / human，不虚报。

## Simplification Debt

> 本包实现面 = 分层路由表（查表替代散落 if 链）+ 显式清层步骤（拆副作用耦合）+ 板级分流接线，**无新依赖、无跨层新调用（清层/裁决都在既有 display 层接口内）、无 `minimal:` 标记**（全仓 `grep minimal:` 无本包新增命中）。**无条目、无 no-trigger 项**——诚实声明「无简化债务」。删的是耦合（「关窗寄生翻页函数」副作用、「BOOT 单击 = 硬路由翻页」耦合），是 `delete` 语义、不留债。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | judgement-call | final 闸红（留红如案，跨包） | **SMK-A4 + SRE-E4 final 闸红（不伪造不代修）**：r1 `unit`/`acceptance` 两闸红 = server 冒烟证据件缺口（screen.md missing）+ server 面 spec 送达语义记录红（「重试到确认」口径是包 7 的验收依据被改动）。均与本包板侧分层派发零相关、零新增（计数与基线逐字一致） | Basis: r1 fact stdout（`SMK-A4 RED: …screen.md missing` / `RMD-E4 RED(infra): spec 的送达语义记录被改动`）+ launch 预报基线逐字比对；heuristic: `[evidence]`/server spec 面红非本包固件齿，不越权代采 | 留红如案：SMK-A4 转 owner=260829-smoke-e2e；SRE-E4 送达语义记录红提 intake 判读归属（可能转 server 面包）。本包不代修 |
| SHOULD | judgement-call | mutation 证据强度 | **11 条 turnedRed 未经独立复跑**：mutation-check 闸绿为结构闸；执行证据 = builder 配额熔断断链自述 + 终树全绿旁证，planner 未独立复验（详见 §Verification Evidence 专节） | Basis: pod-events `61fe7b57` friction 事件 + builder 六项回报缺位 + taskpackage mutationEvidence `_meta` 先例对照（260829 37 条实载）；heuristic: 证据强度降级是诚实申报义务，不构成 hard-violation（登记面完整、无 rule 违反） | supervisor 可选：放行前要求独立复验 11 条（逐条操作与 affectedTest 已写明），或接受当前证据强度进合并门（与 KD-A5 真机轮 pending 一并裁决） |
| FYI | judgement-call | structural 义务 pending | **KD-A5 真机确认轮 pending**：human 持板轮未跑（覆盖层 BOOT 关层不翻页 / 主页面 BOOT 翻页 / KEY 长按返回三行为真机手感），证据目录留位、零伪造 | Basis: taskpackage acceptance `kind:"structural"` + 本相位无 human 设备轮事件（pod-events 4 条无 device-round）+ `device-round-record.md` PENDING 标注；heuristic: structural 证据门 = device-round 相位，无 human 在场证据即 pending，不得以 host 齿冒充 | planner 排 human 持板轮：合并门放行前补，或 human 显式裁「KD-A5 后置、风险接受」——本 integrator 不自判 |
| 观察项 | judgement-call | 覆盖层语义扩展（归下游参考，非缺陷） | **「逐层返回」语义随页集扩展自然成立**：现行页集下 KEY 长按「上一层」= 首页，KD-A3 冻结口径显式声明；M2 查字窗 / M4 P3 键位族落地后如需多级返回栈，须在 `key_route_layer` 语义上扩展 | Basis: KD-A3 acceptance 文本（「现行页集下『上一层』= 首页」）+ key_gesture_parser.h 决策表注释；heuristic: 当前实现与冻结口径一致，多级返回栈是未来页集的需求演化，非本包缺陷 | M2/M4 包在 `key_route_layer` 上扩展时参考，本包不开行动 |

**Kind 说明**：四行均 `judgement-call`（无一条 hard-violation：行 1 为在册留红如案、行 2 为证据强度诚实申报、行 3 为 structural 义务 pending、行 4 为下游参考）。无 BLOCKING → 不停合并门；final fact `ok:false`（在册基线红）照 precedent 走红证据 ESCALATE 通道。

## Five-Axis Review

1. **Correctness（对 KD-A1~A4/A6 冻结接缝契约 + requirements §2.3/§2.2 v1.39 隔离口径）**：逐条兑现——KD-A1 dispatch 按 `layer_top` 分流（覆盖层顶层时 BOOT = `GLOBAL_ACT_OVERLAY_CLOSE` 关层回原页、不产翻页动作）；KD-A2 清层独立成 `kidpal_layer_pop_transients` 显式步骤、`cycle_next` 函数区 `kidpal_layer_pop` 计数归零（tree-scan 齿钉死，非「跑了没报错」）、主页面 BOOT 翻页零回归；KD-A3 覆盖层 KEY 长按 = 关层、主页面 = 回首页、`GLOBAL_ACT_HOME` 硬路由改 layer-aware；KD-A4 十处钉点按隔离口径成批修订且断言语义不放松（RMD-A5 义务红随实现落地转绿是「钉点咬住实现」的直接证据）；KD-A6 注册接线 + 终跑判据达标。host 5 新齿全绿 + 板级文件 `grep` 实测 `kidpal_reminder_key` 5 处引用，是「接线逐字在树上被齿命中」的实证。
2. **Test quality**：冻结验收为机制级（路由决策表 + 结构接线 + 函数区切区 + 数值钉值）而非纯行为级，负向 needle 齿（直连接线复活即红）防假绿；KD-A4 钉点 rmd-B2/B3/B4 对实现回退敏感（mutation 第 9 条钉此）；append-only 枚举纪律有字面钉值齿（`KLD_OVERLAY_CLOSE_VALUE==4`）。mutation 面 11 条登记完整，但执行复跑证据强度降级（见 Findings 行 2）——本轴如实扣分申报，不下假结论。
3. **Contract / boundary safety**：`docs/contracts/**` 与 `docs/requirements.md` 零触碰（contract-lint r1 PASS + KD-E2 兑现）；本包路由语义不新造权威——`key_route_layer` 覆盖层列是 `key_route_global` 的 layer-aware 前置分流，基层判定仍单源在冻结 parser；`*.acceptance.*` 修订 = acceptance-author 权限内留痕修订（REVISION LOG 在册、不豁免验证），非绕过冻结。transient 清层集显式枚举且 PROVISION（配网层）零触碰（KD-E1 红线齿钉住）。
4. **Operability / security**：无新状态泄漏、无动态分配、无新任务上下文；关层与翻页均在 `Application::Schedule` 主任务调度；无新公网入口 / 凭据 / 依赖。
5. **Maintainability / simplicity**：10 文件改动收敛于 key parser / display 层 / 板级接线单 seam，分层决策表查表式可读；无过早抽象、无新依赖、无 `minimal:` 债务（§Simplification Debt 已声明）。

## S2 Exploratory Scan

TaskPackage 未声明 `s2ExploratoryRequired` / `s2ExploratoryGoals` ⇒ 本相位**未执行** S2 official exploratory scan，无 runId、无 structured `runResult`、无 `knowledgeSuggestion` 待决项。required-by-risk 侧注记：`riskTier=high` 但风险面是按键语义正确性（host 齿 + mutation + KD-A5 真机轮覆盖）而非探索式攻击面（板级键路由无攻击面增量），本 packet 如实记录「未声明 ⇒ 未执行」。

## UI design-review 证据

本包无 UIUX work（TaskPackage 未声明 uiux 字段；改动为键路由派发层 + 清层逻辑，显示面只经既有 `kidpal_render_node_*` / `kidpal_layer_*` 接口消费，不新增 UI 面、无 viewport/a11y/design-drift 触及）。无 UI finding。

## Residual Risk

- Known limitation / 留红如案（均非本包引入、owner 已登记或判读归属已写明）：
  1. **SMK-A4 + SRE-E4 final 闸红**（launch 预报在册 server 基线红）：本包不代修；SMK-A4 转 owner=260829-smoke-e2e，SRE-E4 提 intake 判读归属。
  2. **host 在册 4 红**（SU-E3 / KR-A6 同根 docs 归档悬空；WSA-A4 / WSA-A6 owner=260903-device-ws-switch）：在册既存跨包红，基线消解走独立批。
- **KD-A5 pending（本包唯一未闭合 structural 义务）**：human 持板真机轮（覆盖层 BOOT 关层回原页不翻页 / 主页面 BOOT 翻页 / KEY 长按返回上一层三行为手感确认）未跑，证据目录留位（`device-round-record.md` PENDING）。风险 = host 机制齿已钉路由与接线，但真机手感（消抖、演出层时序）无 human 确认；建议 planner 在合并门放行前排 human 持板轮，或由 human 显式裁「KD-A5 后置 + 风险接受」。
- **Mutation 证据强度降级**：11 条 turnedRed 未经独立复跑（builder 配额熔断断链），证据强度 = 登记 + 自述清零 + 终树全绿旁证；独立复验选项已在 Findings 行 2 写明。
- Human decision needed：
  - **红证据 ESCALATE**：final fact r1 `ok:false`（SMK-A4 + SRE-E4 留红如案），提 human 裁「接受本包合并（红转交各自 owner）或先修再合并」——由 supervisor 机械闸 + planner 前置升级闸（`pre-merge-escalation-gate.mjs`）承接，本 integrator 不自判。
  - **risk-accepted 台账**：`taskpackage.decisionCoverage` 四条 decision 的 `landsIn` 全部指向 acceptance/exitCriteria，**无 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头条目** → 本包无 risk-accepted 待追认项。
  - **KD-A5**：排轮或显式风险接受（如上）。

## Closeout

- 制品面：`review-packet.md` / `docs-delta.md`（两件 blocking required 在场）；`device-round-record.md` = KD-A5 pending 留位（非证据、非 blocking 项）。
- 机器件（如实引用路径，不复制正文）：承重 fact = `delivery/taskpackages/260915-m1-k1a-key-layer-dispatch/verifier-evidence/rounds/r1.json`；`current/verdict.json` = 投影·advisory（脚本 fs-write）；checker PASS 见 §Verification Evidence。
- `closeout-check.mjs --read-only` 结果：required 两件在场后重跑，advisory 项（pod-retro.md 归 planner 收尾相位；machine evidence advisory）如实记录，不阻塞。post-cutoff 不产 session-digest / loop-health / loop-metrics / pod-journal；`pod-events.jsonl` 在场（advisory 满足，4 条：intake / acceptance-author 完成 / builder friction / verify 完成）。
- **pod-retro.md 未产**：归 planner 收尾相位职责（Q5=warn advisory、零追溯白名单），本 integrator 不代写。

## Commit 冻结（合并门复核的对象）

- 功能提交 = builder/acceptance-author 已落工作树（firmware 实现 10 文件 + 冻结验收件 2 件），积分收拢提交（本相位）把 closeout 制品 3 件 + dev-loop ledger（taskpackage 活文件 / input-briefs / pod-events / verify-runs / red-baseline-freeze / r1 fact / current/ 指针）收口。commit 前按 AGENTS.md Hard Rule 用 `git diff --cached --stat` 核对；**不 stage `.codebase-memory/**`（机器再生面常驻脏，Pod 分支禁止私带快照 commit）、不 stage `firmware/build/**`**。
- 可回滚路径：全部交付物为 feature 分支 `dev/260915-m1-k1a-key-layer-dispatch` 上的本地 commit（**未 push、未动 main**）；回滚 = 分支级 revert（恢复 BOOT/KEY 直连硬路由 + 关窗寄生翻页 + 钉点旧口径）。main 零暴露。
- 合并相位：supervisor 机械闸（`supervisor-verdict-gate.mjs`）裁断 → planner 前置升级闸 → 放行后 `merge-driver --pod` 本地合并（本包 mergeOrder 22）。本 integrator 不自行判定通过、不执行 merge、不 push。

## 合并门须知（短清单）

1. 无 BLOCKING finding；四行 findings 均留红如案 / 证据强度申报 / structural pending / 下游参考，不阻断本包自身面。
2. 承重 fact = **r1**（phase final，scoped 7 闸绿 + unit/acceptance 红 = SMK-A4 + SRE-E4 在册基线红，`ok:false`）；supervisor 复核读 `delivery/taskpackages/260915-m1-k1a-key-layer-dispatch/verifier-evidence/rounds/r1.json`。
3. host runner 134/4 = 在册 4 红 + RMD-A5 义务红转绿 + 新齿 5 条全绿，相对有效基线 129/5 零新增红，KD-A6 达标。
4. **KD-A5 真机轮 pending**——放行前需 planner 排 human 持板轮或 human 显式风险接受。
5. **Mutation 证据强度**：11 条 turnedRed 未经独立复跑，supervisor 可要求独立复验（操作清单在 mutationEvidence）。
6. 冻结面（`docs/contracts/**`、`docs/requirements.md`）git 零触碰；`schedule_reminder_b.acceptance.c` 修订 = acceptance-author 权限内留痕修订。
7. **红证据 ESCALATE**：final fact `ok:false` 触发人类裁决通道（SMK-A4 + SRE-E4 留红如案），riskTier=high 照常升级。
