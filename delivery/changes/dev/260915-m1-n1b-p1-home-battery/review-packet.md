# Review Packet — 260915-m1-n1b-p1-home-battery（M1-N1b：首页新版式「接下来」主体 + 电量 ADC 接线销账）

> integrator 产出 · 2026-09-16 · 分支 `dev/260915-m1-k1a-key-layer-dispatch`（本包在 544a804 新提交面直接开发，rebase 条款作废——planner launch 确认在册 taskpackage.notes）。
> 供 supervisor 合并门复核。本包 `riskTier=medium`（host-teeth / device-round gateProfile）。**本相位只收拢冻结，不合并**——PH-A6 真机轮挂起待 human PASS（2026-09-17 晨回归），合并门停等。
> 交付本体：`home_p1.c` v1.4 新版式（「接下来」日程主体 + 倒计时贴纸 + 辅助条小图标前置，WF-F1-01 v1.4 重排序）+ 字形/图标缩放 blit（fixer 两轮真机迭代补丁）+ 电池注入缝（`kidpal_render_node.{h,cc}`）+ ADC 外部句柄构造重载（`boards/common/adc_battery_monitor.{h,cc}`）+ 板级接线与 `GetBatteryLevel` override（`waveshare-s3-rlcd-4.2.cc`，`s_battery_pct` 恒 100 销账）+ host 接线 + 冻结验收 `p1_home_battery.acceptance.c`（5 TEST_CASE）。
> **合并门必读**：承重 final fact **r6 非 full 绿**（`ok:false`），红 = `unit`/`acceptance` 两 full 闸 **3 failed**：SMK-A4 + SRE-E4（K1a closeout 锚在册 server 基线红）**+ VRT-A2（见 Findings 行 1——锚基线引用与验证树态间的漂移红，根因 = 544a804 验收归档 commit，非本包代码面引入）**。本包自身面（scoped 7 闸全绿 + host runner 13/0 + 新齿 5 条全绿 + mutation 10 条执行敏感）达标。PH-A6（真机确认轮，human 持板）**挂起待 human PASS**，见 §Residual Risk。

## 四阶段闭环事实

| 阶段 | 事实 | 锚点 |
|---|---|---|
| intake/planner 派单 | 拆包 M1-home-qa 开工批（块②），brief 落盘，K1a 已并本地 main、本包在 544a804 面直接开发 | taskpackage.notes（2026-09-15） |
| acceptance-author | 冻结 `p1_home_battery.acceptance.c`（5 TEST_CASE）+ 有效基线落账：host 通道① 8 Tests / 0 Failures（544a804 归档后现役面收敛，K1a 期 4 条在册红移出现役面、owner 不变）+ 通道② 新齿 gcc 实编 COMPILE_OK + 解析性红定性 | taskpackage.redBaselineResult（2026-09-15） |
| builder | 实现落盘（见 §Change Size）+ mutation 执行 10 变异全敏感（phase:"mutation" dispatch completed） | pod-events `aa63f058` / `b3d1b84f` |
| verifier | verify.mjs 六轮：r1 红（mutation-evidence 缺失，销账后重跑）→ r2 scoped 全绿 → r3/r6 final（3 failed 见 §红基线判读）→ r4/r5 scoped 全绿（fixer 两轮后复跑） | verify-runs.jsonl 六条 + rounds/r1-r6.json |
| integrator（本相位） | 承重 bearing final fact r6 checker PASS + 真机轮证据归档（boot 捕获两份 + 记录件）+ closeout 制品落盘 + 冻结 diff commit | 见 §Verification Evidence |

## Change Size

- Files changed（git diff --stat 实测）：
  - **实现面 = 7 文件，固件 +721/−360 量级**（与 delivery 记账面合计 14 文件 +744/−360）：
    - `firmware/main/display/home_p1.c`（+424/− 主改动）：v1.4 新版式（「接下来」主体重排、倒计时贴纸、辅助条小图标前置）、低电闪烁（doze 相位门）、字形/图标缩放 blit（fixer r1/r2 补丁）；
    - `firmware/main/display/kidpal_render_node.{h,cc}`（+19）：电池注入缝（`s_battery_pct` 注入写点，替代恒 100）；
    - `firmware/main/boards/common/adc_battery_monitor.{h,cc}`（+82/−）：外部句柄构造重载（死成员 `adc1_handle/cali_handle/vbat_status` 接线）；
    - `firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc`（+125/−）：ADC 接线 + `GetBatteryLevel` override 实装（基座默认 false 销账）+ 板级注入调用（corr_id=kdp-1222）；
    - `firmware/main/test/host_app/main/CMakeLists.txt`（+16/−）：`p1_home_battery.acceptance.c` 注册进 host 接线批。
  - **冻结验收件**：`firmware/main/test/p1_home_battery.acceptance.c`（新文件，5 TEST_CASE，PH-A1~A5/A7 机制级齿）。
  - **delivery 记账面**（planner 自有面，本相位一并收口）：taskpackage 活文件 + input-briefs + pod-events.jsonl + verify-runs.jsonl + verifier-evidence/rounds/r1-r6 + current/ 指针投影。
  - **closeout 制品**（本相位落 `changes/dev/260915-m1-n1b-p1-home-battery/`）：`review-packet.md` / `docs-delta.md` / `device-round-record.md` + boot 捕获日志两份。
- Scope: firmware 首页渲染 + 板级 ADC（taskpackage `subsystem` 钉定）；`docs/contracts/**` 与 `docs/requirements.md` **零改动**（contract-lint r6 PASS；PH-E1 兑现——schedule API 面零触碰，小伴睡姿演出已砍项零触碰）。
- Split recommendation: **不拆**——版式、注入缝、ADC 接线、验收齿是「电量端到端 owner = 本包独占」（crossPackageBehaviors 在册）的单一语义收敛，拆开任一面都使「板级接线 ↔ 注入缝 ↔ 齿」失去同批可验性。

## Verification Evidence

> **承重 fact 声明（诚实口径）**：final completed-only round = **r6**（`phase:"final"`，identity = changeId+phase+round+invocationId），
> `delivery/taskpackages/260915-m1-n1b-p1-home-battery/verifier-evidence/rounds/r6.json`：`ok:false`、`observedGateExit:1`、`authority:"bearing"`、`producedBy:"verify.mjs"`、`gatesRun:9`、`gatesNotExecuted:[]`（notExecuted=0）、
> `invocationId 56629d11-faa6-451b-adda-d6f0f5beedfb`、`startedAt 2026-09-15T16:37:28.299Z / completedAt 16:37:47.522Z`。
> `ok:false` 的全部成因 = `unit`/`acceptance` 两 full 闸 server 面 3 failed（见 §红基线判读）；scoped 7 闸**全绿**。
> `current/verdict.json` 为投影（non-bearing compatibility projection，脚本 fs-write）；closeout 判断以 r6 fact 为准。
> **checker**：`verifier-evidence-check.mjs --phase final --round 6` → `{"ok":true,"factOk":false,...}`——final evidence 结构有效、走红证据 ESCALATE 通道进 closeout。

- **r6 逐闸（实测 ms）**：taskpackage-required 160 / **firmware-build 6274（真编译 build-pass，`xiaozhi.bin 0x2c6850`、`0xa97b0 (19%) free`）** / contract-lint 144 / acceptance-coverage 154（PH-A1..A7 全在册、`structural:["PH-A6"]`、files=[p1_home_battery.acceptance.c]）/ mutation-check 156（10 条 perAcceptance 与 acceptance 声明逐字节一致）/ docs-sync 139（`docsSyncRequired:false`）/ security 130（`securityReviewRequired:false`）/ unit 6124（红，见下）/ acceptance 5804（红，见下）。
- **scoped 中间轮**：r2（fixer 前）与 r4/r5（fixer r1/r2 后）scoped 7 闸全绿；r1 红 = mutation-evidence 缺失（acceptance-author interrupted 断链，重跑销账）。
- **红基线判读（PH-A7 判据）**——两层分述，**含一处与移交口径的偏差申报**：
  - **host runner（板侧，本包面）**：有效基线 8 Tests / 0 Failures（acceptance-author 冻新齿时落账 + planner 独立复核双确认；544a804 归档后现役面收敛，K1a 期 4 条在册红 SU-E3/KR-A6/WSA-A4/WSA-A6 随归档移出现役面、owner 不变不代修）。终跑 = **13 Tests / 0 Failures / 0 Ignored**（8 基线 + 本包新齿 5 条全绿）→ 「相对有效基线零新增红 + 本包新齿全绿」达标，PH-A7 板侧判据满足。
  - **server 面（full 闸）**：unit `3 failed, 166 passed, 6 skipped, 45 deselected` / acceptance `3 failed, 129 passed, 6 skipped, 45 deselected`。**偏差申报：移交口径称「仅 2 失败 = SMK-A4 / SRE-E4」，rounds 事实为 3 failed**——第三条 **VRT-A2**（`test_voice_roundtrip_b.acceptance.py::test_a2_protocol_profile_twin_aligned`：C-side twin `firmware/main/test/voice_roundtrip_b.acceptance.c` 缺失）。已定位根因：commit **544a804**（human 裁定、跳过 dev-loop 的一次性验收归档，2026-09-15 21:21 +0800，`R100` 移入 `acceptance/archive/`）使 server 齿钉住的旧路径失配；K1a 锚基线（2 failed / 130 passed）取于 09:05Z、早于该归档。本包验证窗口内 r1→r6 恒 3 failed，**本包代码面零引入、零新增**；但相对被引用的锚基线计数，这是 1 条未在册红，owner 待判（归 server 面 voice-roundtrip 齿的归档适配，见 Findings 行 1）。SMK-A4（screen.md 证据件缺口，owner=260829-smoke-e2e）与 SRE-E4（spec 送达语义记录红，owner 归 intake 判读）逐条命中 K1a 锚在册红，均与本包固件面零相关。
- **Mutation 敏感断言 provenance**：`taskpackage.mutationEvidence.perAcceptance` 10 条（PH-A1×2 / PH-A2×2 / PH-A3×2 / PH-A4×2 / PH-A5×1 / PH-A7×1，PH-A6 structural 不入变异面），全部 `failed as expected` / `sensitive:true`。执行证据 = builder mutation 相位实跑（pod-events `b3d1b84f` phase:"mutation" completed 1657s/57 toolUses）——**执行在册，非 K1a 式断链自述**。两条边界如实记录：① `PCT_WRITE_BYPASS` 首轮对预期齿 tooth7 insensitive，换点后命中 tooth11（恒值注入守卫）——tooth7 失敏因验收切片宽度含 line499 消费点（切片宽度设计边界，在册）；② 变异命名纪律合规（`PCT_WRITE_BYPASS` 注释字面同批中和，无与被测符号共享可识别子串的铸造名）。mutation-check 闸绿 = 登记与声明逐字节一致（结构闸）。
- **PH-A6 真机确认轮（kind=structural，human 持板）**：**挂起（SUSPENDED）**——烧录两轮 + boot 捕获两份在册（PH-A3 端到端真机实证：`Use external ADC handle` + `battery_inject corr_id=kdp-1222 level=95 charging=0 vbat_ok=1`，真电量 95 非恒 100）；两轮 fixer 真机迭代（字形溢出 → 图标裁半）已修并复烧；看屏复验 / 充电捕获 / 断网替代方案 / **PASS 裁定**待 human 2026-09-17 晨回归（pod-events `372601a6`）。证据与记录 = 同目录 `device-round-record.md` + `device-round-boot-capture-20260916.log` / `device-round-fix-boot-capture-20260916.log`，零伪造 PASS 结论。
- **Security**：r6 `ok:true`（`securityReviewRequired:false`）。无新公网入口 / secrets / 依赖；ADC 读取复用既有 `AdcBatteryMonitor` 组件、注入缝为固件内部 seam、无动态分配面新增。
- **Docs sync**：r6 `ok:true`（`docsSyncRequired:false`）。判定与逐条理由 = 同目录 `docs-delta.md`（S-F1.1 语义权威已冻结在文、契约面只读、PH-E1 数据口径零改动）。**不改 docs/contracts/**。
- **影响面核对（step 11b）**：Pod worktree 内 diff 对索引不可见 → `detect_changes` 如实标注 **N/A (worktree)**，以 `git diff --stat`（上列文件面）+ 关键共享符号 `trace_path` 抽查替代：图索引（main 侧历史面）在册 `AdcBatteryMonitor.GetBatteryLevel`（boards/common）与 `Board.GetBatteryLevel` 基座默认实现（默认返回 false 现状的出处）；waveshare 板级 override 不在索引 = 本包 worktree 新增面，与 git diff 面一致；改动收敛于上列文件，无跨文件面外溢。
- **Re-index 需求**：主索引（main 侧）尚不含本包新符号（板级 override、注入缝等），re-index 归主线程 / human，不虚报。

## Demo 对账行

| 证据形态 | 状态 | 说明 |
|---|---|---|
| 模拟器截图 | **缺项（未采集）** | 理由：PH-A6 真机轮已代偿版式确认（human 持板两轮迭代 + 修复版重烧看屏），模拟器截图可在 human 回归后补采；host 齿盲区（fixture 不挂 art）使模拟器/host 截图的版式保真度有限 |
| host runner | **在册** | 13 Tests / 0 Failures / 0 Ignored（8 基线 + 新齿 5 条全绿，PH-A7 板侧判据）；mutation 10 条执行敏感（表见 taskpackage `mutationEvidence.perAcceptance`） |
| 真机轮 | **挂起** | 已捕获 boot 证据两份（`device-round-boot-capture-20260916.log` / `device-round-fix-boot-capture-20260916.log`，PH-A3 端到端实证在册）；看屏复验 / 充电捕获 / 断网 / PASS 裁定待 human 回归 |

## Simplification Debt

> 本包实现面 = 版式重排 + 缩放 blit + 注入缝 + 外部句柄构造重载，**无新依赖、无跨层新调用（注入缝在既有 render node 接口内、ADC 复用既有组件）、无 `minimal:` 标记**（全仓 `grep minimal:` 无本包新增命中）。**无条目、无 no-trigger 项**——诚实声明「无简化债务」。删的是现状缺陷（`s_battery_pct` 恒 100、`GetBatteryLevel` 基座默认 false 空转），是 `fix` 语义、不留债。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | final 闸红（锚基线漂移，跨包） | **VRT-A2 红 = 锚基线引用与验证树态间漂移，非本包引入**：移交口径「仅 2 失败（SMK-A4/SRE-E4）零新增」与 rounds 事实（3 failed）不符；第三条红根因 = 544a804 验收归档（human 裁定一次性操作）移走 C-side twin，server 齿未同步适配。本包窗口 r1→r6 恒 3 failed，代码面零引入 | Basis: r6 fact stdout（`VRT-A2 RED: C-side twin … missing`）+ `git show 544a804 --name-status`（R100 归档 voice_roundtrip_b.acceptance.c）+ K1a 锚取点 09:05Z 早于归档 21:21 +0800 的时间线；heuristic: `[evidence]` 基线引用漂移是申报义务，归属判读非本包权限，不自裁 | owner 判读归 planner/intake：server 面 VRT-A2 齿做归档适配（改钉 archive 路径或撤存在性断言）或 human 裁例外；本包不代修。合并门放行前须与 SMK-A4/SRE-E4 一并进红证据 ESCALATE 裁决 |
| FYI | judgement-call | final 闸红（留红如案，跨包） | **SMK-A4 + SRE-E4 命中 K1a 锚在册红（不伪造不代修）**：screen.md 证据件缺口（owner=260829-smoke-e2e）+ spec 送达语义记录红（owner 归 intake 判读）。均 server 面、与本包固件齿零相关、计数与 K1a 锚逐字一致 | Basis: r6 fact stdout（`SMK-A4 RED: …screen.md missing` / `RMD-E4 RED(infra): spec 的送达语义记录被改动`）+ taskpackage.redBaselineResult 锚引用；heuristic: 在册留红如案、不越权代采 | 留红如案：维持各自 owner 挂账，本包不代修 |
| BLOCKING（合并门停等项） | judgement-call | structural 义务挂起 | **PH-A6 真机确认轮 PASS 裁定待 human**：烧录/捕获/两轮迭代已完成、PH-A3 端到端真机实证在册，但看屏复验/充电捕获/断网替代方案/PASS 裁定待 human 2026-09-17 晨回归（pod-events `372601a6` 挂起到期）。合并门停等为本包**唯一** blocking 项 | Basis: acceptance `kind:"structural"` + human decision 事件链（`1c42781c` 开烧 / `4c10c620` PH-E2 降级 / `372601a6` 挂起）+ `device-round-record.md` 留位；heuristic: structural 证据门 = human 持板裁定，agent 不得以 host 齿或 boot 日志冒充 PASS | planner 排 human 回归轮：PASS → 合并门放行流程；不 PASS → 交回 fixer 或 human 裁风险接受。本 integrator 不自判 |
| SHOULD | judgement-call | host 齿盲区（follow-up 候选） | **验收 fixture 默认不挂 art（weak 哨兵走兜底），真字形/真图标渲染路径 host 零覆盖**：fixer 两轮真机迭代（48px 字形溢出、图标资产裁半）正是该盲区漏放的缺陷，当前视觉正确性证据 = fixer 手写 ink 实测表 + 真机轮 | Basis: pod-events `1514491d` friction（真机轮抓出盲区）+ `p1_home_battery.acceptance.c` fixture 挂载面 + fixer r1/r2 迭代记录；heuristic: 盲区是测试基建缺口非本包验收违约，修复代价（harness 挂真资产 + 收紧齿）超本包预算 | 登记 follow-up：host harness 挂真 art 资产 + 补「图标半区相邻空隙零墨」收紧齿，归后续测试基建批；planner 立项裁优先级 |
| 观察项 | judgement-call | 基座 chrome 归属（跨包交叉） | **状态栏联网小标偏大一圈（human 真机观察）**：属 `LvglDisplay::UpdateStatusBar` 共享底座面、非本包 seam，与 k2-keybar 包潜在交叉 | Basis: human 真机观察转述（状态面）+ 该符号不在本包 git diff 面；heuristic: 非本包 seam 的观感项只登记归属候选，不开行动 | 待 human 裁归属（本包 / keybar / 底座独立批）后由归属方承载；日期描边框（设计内殿后）同批观感裁定 |

**Kind 说明**：五行均 `judgement-call`（无 hard-violation：行 1 为基线引用漂移申报、行 2 在册留红、行 3 structural 挂起、行 4 测试基建盲区、行 5 跨包归属观察）。**行 3 为合并门停等的 blocking 项**（structural 义务未闭合）；行 1 为升级裁决项（进红证据 ESCALATE）；其余不阻断本包自身面。final fact `ok:false` 照 precedent 走红证据 ESCALATE 通道。

## Five-Axis Review

1. **Correctness（对 PH-A1~A5/A7 冻结接缝契约 + requirements S-F1.1 AC1–AC4）**：逐条兑现——PH-A1 版式 v1.4 重排（「接下来」主体 + 倒计时贴纸 + 小伴演出与趣味点缀保留，空日程门齿钉死）；PH-A2 辅助条小图标前置（env 有效性门 + 图标半区墨齿）；PH-A3 三处死成员接线 + override 实装 + 注入缝（恒值注入守卫齿 + 注入消费文件数齿，mutation 双变异敏感）+ **真机端到端实证（level=95 非恒 100）**；PH-A4 低电闪烁相位门 + 阈值边界齿（真机项按 human 裁定 PH-E2 降级，不虚报绿）；PH-A5 断网 WiFi 斜杠钉点；PH-A7 host 13/0 达标。fixer 两轮真机迭代缺陷（字形溢出/图标裁半）已修并复验（r4/r5 全绿 + 修复版重烧捕获）。
2. **Test quality**：冻结验收为机制级（注入缝符号级 + 相位门 + 阈值边界 + 注册接线序）而非纯行为级，负向 needle 齿（恒值注入/死成员复活/图标丢弃即红）防假绿；mutation 10 条**执行在册**（builder mutation 相位实跑）全敏感，tooth7 切片宽度边界如实记录不下假结论。盲区（fixture 不挂真资产）如实申报为 follow-up（Findings 行 4）。
3. **Contract / boundary safety**：`docs/contracts/**` 与 `docs/requirements.md` 零触碰（contract-lint r6 PASS + PH-E1 兑现：schedule API 面零改动、已砍演出零触碰）；电池注入缝是固件内部 seam 不暴露契约面；外部句柄重载为构造注入、不改 `AdcBatteryMonitor` 既有调用方（其余板类路径零回归——host 8 基线齿全绿佐证）。
4. **Operability / security**：无新公网入口 / secrets / 依赖；ADC 读数经既有组件校准路径（vbat_ok 门）、注入 corr_id（kdp-1222）链路完整可追；真机两轮无 crash/复位循环（boot 捕获日志在册，SRAM free 稳定 ~64KB）。
5. **Maintainability / simplicity**：改动收敛于 display 层单 seam + 板级接线，构造重载查表式可读；无过早抽象、无新依赖、无 `minimal:` 债务（§Simplification Debt 已声明）。

## S2 Exploratory Scan

TaskPackage 未声明 `s2ExploratoryRequired` / `s2ExploratoryGoals` ⇒ 本相位**未执行** S2 official exploratory scan，无 runId、无 structured `runResult`、无 `knowledgeSuggestion` 待决项。required-by-risk 侧注记：`riskTier=medium`，风险面是渲染正确性与硬件读数（host 齿 + mutation + PH-A6 真机轮覆盖），非探索式攻击面，本 packet 如实记录「未声明 ⇒ 未执行」。

## UI design-review 证据（bootstrap 期纯文本口径）

- **Viewport coverage**：首页 400×300 单视口（真机渲染日志 `render home 400x300 render_rc=0` 在册）；新版式各分区（主体日程/倒计时贴纸/辅助条）坐标面由 v1.4 冻结版式承载，无滚动/多视口面。
- **A11y summary**：本包面无可访问性新义务（无新交互控件；低电闪烁为视觉信号，human 裁定 PH-E2 降级为读数偏差记录承载）；自动 a11y 脚本 bootstrap 期 deferred。
- **Design-drift summary（brief↔实现）**：WF-F1-01 v1.4 重排序逐区兑现（host 齿 + 真机两轮迭代收敛）；两轮 drift（字形溢出/图标裁半）由真机轮抓出、fixer 缩放 blit 修复——drift 闭环记录见 `device-round-record.md`。日期「8月27日 周四」描边框 = 设计内殿后（中文字形无资产），观感裁定待 human。
- **External artifact reference**：无（重资产留仓外；boot 捕获日志为本目录在册文本证据）。
- **Residual risk**：真机观感最终裁定待 human（PH-A6 挂起项 1/4）；host 真资产渲染路径零覆盖使自动回归对视觉正确性无担保（Findings 行 4）。UI severity 复用 `code-review-gate.md` 既有轴，无 UI BLOCKING、不新增 gate。

## Residual Risk

- Known limitation / 留红如案（均非本包代码面引入、owner 已登记或判读归属已写明）：
  1. **SMK-A4 + SRE-E4 final 闸红**（K1a 锚在册 server 基线红）：本包不代修；SMK-A4 owner=260829-smoke-e2e，SRE-E4 归 intake 判读。
  2. **VRT-A2 锚基线漂移红**（544a804 归档副作用）：owner 待 planner/intake 判读（Findings 行 1）。
  3. **host 通道 K1a 期 4 红**（SU-E3/KR-A6/WSA-A4/WSA-A6）：随 544a804 归档移出现役面，owner 不变、基线消解走独立批。
- **PH-A6 挂起（本包唯一 blocking 合并门项）**：待 human 2026-09-17 晨回归——① 看屏复验（修复版版式观感）；② 充电捕获（charging=1 真机读数）；③ 断网替代方案（MAC 隔离踢下线候选）；④ 观感与 PASS 裁定。风险 = host 机制齿已钉注入缝与相位门、boot 日志已证端到端接线，但充电路径真机读数与整体观感无 human 确认。
- **挂起项台账（诚实全列）**：① PH-A6 PASS 待 human（blocking）；② 日期描边框设计内殿后（观感待 human）；③ 状态栏联网小标偏大（非本包 seam，归属待裁）；④ host 齿盲区（fixture 不挂真资产，follow-up 候选）。
- **Mutation 证据边界**：10 条执行在册（builder mutation 相位实跑），但 mutation-check 闸本身为结构闸；tooth7 对 `PCT_WRITE_BYPASS` 失敏为切片宽度设计边界（在册非隐瞒），如需可独立复验（逐条操作与还原要求在 mutationEvidence 条目内）。
- Human decision needed：
  - **PH-A6 PASS 裁定**（如上，blocking 合并门）。
  - **红证据 ESCALATE**：final fact r6 `ok:false`（SMK-A4 + SRE-E4 在册红 + VRT-A2 漂移红），提 human 裁「接受本包合并（红转交各自 owner）或先修再合并」——由 supervisor 机械闸 + planner 前置升级闸（`pre-merge-escalation-gate.mjs`）承接，本 integrator 不自判。
  - **risk-accepted 台账**：`taskpackage.decisionCoverage` 四条 decision 的 `landsIn` 全部指向 acceptance/exitCriteria（PH-A1/PH-A3/PH-A4/PH-E1），**无 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头条目** → 本包无 risk-accepted 待追认项。
  - **VRT-A2 owner 判读** + **联网小标归属裁**（如上）。

## Closeout

- 制品面：`review-packet.md` / `docs-delta.md`（两件 blocking required 在场）；`device-round-record.md` = PH-A6 挂起记录 + boot 捕获日志两份（真机证据件，非 PASS 结论）。
- 机器件（如实引用路径，不复制正文）：承重 fact = `delivery/taskpackages/260915-m1-n1b-p1-home-battery/verifier-evidence/rounds/r6.json`；`current/verdict.json` = 投影·advisory（脚本 fs-write）；checker PASS 见 §Verification Evidence。
- post-cutoff 不产 session-digest / loop-health / loop-metrics / pod-journal；`pod-events.jsonl` 在场（advisory 检查满足，12 条：planner friction / 四角色 dispatch-completed / 三条 human decision / friction 两条）。**pod-retro.md 未产**：归 planner 收尾相位职责（Q5=warn advisory、零追溯白名单），本 integrator 不代写。
- `closeout-check.mjs`（默认只读）结果见交付报告；advisory 项如实记录不阻塞。
- milestone/workstream 登记（`milestones.jsonl` / `workstreams/m1-home-qa/progress.md`）归主线程对账轮统一渲染（本包不改，docs-delta.md 已申报）。

## Commit 冻结（合并门复核的对象）

- 功能提交 = builder/acceptance-author/fixer 已落工作树（firmware 实现 7 文件 + 冻结验收件 1 件），积分收拢提交（本相位）把 closeout 制品 4 件（含两份 boot 日志）+ dev-loop ledger（taskpackage 活文件 / input-briefs / pod-events / verify-runs / rounds r1-r6 / current/ 指针投影）收口。commit 前按 AGENTS.md Hard Rule 用 `git diff --cached --stat` 核对；**不 stage `.codebase-memory/**`（机器再生面常驻脏，Pod 分支禁止私带快照 commit）、不 stage `firmware/build/**`**。
- 可回滚路径：全部交付物为当前分支 `dev/260915-m1-k1a-key-layer-dispatch` 上的本地 commit（**未 push、未动 main**）；回滚 = 分支级 revert（恢复恒 100 注入 / 基座默认 GetBatteryLevel / v1.4 前版式）。main 零暴露。
- 合并相位：**停等 PH-A6 human PASS** → supervisor 机械闸（`supervisor-verdict-gate.mjs`）裁断 → planner 前置升级闸 → 放行后 `merge-driver --pod` 本地合并（本包 mergeOrder 24）。本 integrator 不自行判定通过、不执行 merge、不 push。

## 合并门须知（短清单）

1. **合并门停等 PH-A6**：human PASS 裁定（2026-09-17 晨回归）是本包唯一 blocking 项；PASS 留痕入 pod-events 后由 planner 主线程走合并门。
2. 承重 fact = **r6**（phase final，scoped 7 闸绿 + unit/acceptance 3 failed：SMK-A4 + SRE-E4 在册锚红 + VRT-A2 漂移红）；supervisor 复核读 `delivery/taskpackages/260915-m1-n1b-p1-home-battery/verifier-evidence/rounds/r6.json`。
3. **偏差申报**：移交口径「仅 2 失败零新增」与 rounds 事实（3 failed）不符——VRT-A2 根因 544a804 归档（human 裁定一次性操作），本包窗口内零新增，但 owner 判读须进 ESCALATE。
4. host runner 13/0 = 8 基线 + 新齿 5 条全绿，PH-A7 板侧判据达标；mutation 10 条执行在册全敏感。
5. **红证据 ESCALATE**：final fact `ok:false` 触发人类裁决通道；本包自身面无 hard-violation、无 BLOCKING finding（除 structural 挂起停等项）。
6. 冻结面（`docs/contracts/**`、`docs/requirements.md`）git 零触碰；冻结验收 `p1_home_battery.acceptance.c` 零修订。
7. demo 对账：模拟器截图缺项（真机轮代偿，human 回归后可补采）+ host 13/0 在册 + 真机轮挂起（boot 证据在册）。
