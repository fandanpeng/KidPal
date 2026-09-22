# Review Packet · 260921-m3-k1-cloud-schedule-producer

> workstream m3-schedule ｜ M3-K1 云端课表数据链路（契约对齐 + 周网格/接下来计算 + 三触发 producer）｜ mergeOrder 33 ｜ rigor full ｜ riskTier medium ｜ subsystem server ｜ sourceType delivery-story
> integrator 收拢制品（2026-09-21）；合并门判读归 planner 主线程 supervisor-verdict-gate 机械闸，本包不 push、不 merge、不在 main 上 commit。契约落地批 `24bb1b8` 已在本分支基线（契约门 HUMAN_APPROVED，gate evidence LANDED）。

## Change Size

- **契约落地批（commit `24bb1b8`，分支基线）**：`docs/contracts/interface-contract.md` +35/−10（§1.3② schedule 周网格形状重定义 + §1.3⑥ home 可选 data.upcoming + §1.5/§1.6/§2.3 连带改写）+ `server/gateway/schedule.py` 模块头契约钉行更新（doc 零行为）+ CDQ 登记 + ADR-004 + gate evidence LANDED。
- **实现（builder 未提交改动 → 本次收口 commit）**：
  - `server/gateway/schedule.py` +106/−1——新增周网格 / home upcoming producer 纯函数段：`week_grid_payload`（学校课逐节 4+2、固定课外课逐门、临时安排 ※ flags bit0、group_upcoming 星标 bit1 呼应、周外一次性安排过滤）、`home_upcoming_payload`（group_upcoming 零偏移复用 + 明日预告 preview）、分界常量留痕（`_PM_START=12:00` / `_EVENING_START=18:00`，契约未钉分界的显式缺口③）；既有 `parse_schedule_md`/`items_for_date`/`group_upcoming` 冻结语义零偏移（E1 must-stay-green 齿在册）。
  - `server/gateway/schedule_push.py`（新文件）——三触发 producer 决策面（纯 python、零 gateway/fastapi 依赖）：`connect_push_payload`（触发① 内容单源 == home_upcoming_payload 同参）、`daily_push_due`（触发② 本地零点判定，timezone 固定 Asia/Shanghai UTC+8 house 先例、其它 zone ValueError 大声拒绝）、`refresh_requested` / `consume_refresh_marker`（触发③ marker 请求/恰一次消费，MARKER_FILENAME 跨组件协议常量）。
  - `server/gateway/front/front_app.py` +116/−1——三触发接线：触发① ws 建连即推；触发②/③ 走 reminder 轮询循环 piggyback（每日零点记账 + marker 消费），payload 经 `_relay_display_blocks_to_device` 既有中继链下发（home 形状必过 display_schema 机械门）；脏 B1 解析失败归日志、轮询照走；单腿失败不连坐。
  - `server/gateway/display_schema.py` +20——Q5 收紧（契约门裁定）：schedule view data 只放行 `{}` 薄载荷或新周网格形状（day/slot/name/flags 四键 floor 校验），旧 `{title,items}` 列表形与缺 courses 畸形一律拒绝。
  - `server/deploy/openclaw-patch/reminder_schedule_lib.py` +31——`drop_refresh_marker`（OpenClaw 主通道改写 MD 后经既有重算入口在 alerts 交接目录落 `schedule.refresh` marker；幂等、尽力而为不抛异常；副通道直改不经此入口 = S-F1.5 AC2 语义）。
  - `server/acceptance/test_m3k1_schedule_producer.acceptance.py`（新文件，17 齿）——A1 契约门齿（legacy 拒绝 + 畸形拒绝 + 三样本 must-stay-green）、A2 网格计算齿、A3 upcoming payload 齿、A4 三触发模拟齿、E1 既有语义零偏移守卫齿；文件头「契约条目 ↔ 齿强度申报」+ 显式缺口①–⑤申报在案。
  - `server/tests/test_schedule_producer_units.py`（新文件，11 测）——实现自带测：分界时刻留痕样本、脏 timezone 拒绝、marker 竞态幂等、Q5 收紧分支、周外过滤等规格未逐字段钉的边界/错误路径。
- **回填与过程账**：`delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/taskpackage.json`（redBaselineResult / mutationEvidence 17 齿 RED→GREEN 回填）+ `pod-events.jsonl`（human-decision / contract-ccp-landed / friction / dispatch-completed 全程留痕）+ `verifier-evidence/rounds/r1–r5.json` + `verify-runs.jsonl`（verify.mjs 程序 fs-write 过程态）+ `delivery/workstreams/m3-schedule/progress.md` K1 节。
- **Scope**：`server/gateway/**` + `server/deploy/openclaw-patch/**` + `server/acceptance/**` + `server/tests/**` + taskpackage 过程账；firmware/** 零命中（板端消费归 K2）；`docs/contracts/**` 全生命周期唯一命中 = 契约门通道的 `24bb1b8`（签字在案）；spec 面 / `.claude/**` 零命中。
- **Split recommendation**：不拆——契约形状、producer 计算、三触发接线、Q5 门收紧是同一条「课表数据下发」链路的四段，拆开任一段都无法独立验收（形状齿依赖契约落地、触发齿依赖 producer 单源）。

## Verification Evidence

- **final round 5 九闸全绿（verifier，`487f57a8-81b8-4d7e-b7c6-a3e294ca9c7a`，2026-09-21，`ok:true` / `observedGateExit:0`）**：evidenceRef `delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/verifier-evidence/rounds/r5.json`（immutable verifier fact，本 packet 只引用不复制）。scoped r4（`8613ac98-…`）同 `ok:true`，scoped r1–r3 过程态在 `verify-runs.jsonl`。
  - taskpackage-required / docs-sync / security / contract-lint：全绿。
  - firmware-build：真编译未降级（xiaozhi.bin 0x30b9b0 bytes、app 分区余量 11%；本包 server-only，闸保固件基线不红）。
  - unit：**75 passed**（server/tests + 云面验收件收集）。
  - acceptance：oracle-pass + executor-run **27 passed**（固件面验收 host 收集 + 云面新件全量真跑）。
  - mutation-check / acceptance-coverage：functional = M3K1-A1/A2/A3/A4 全部在册（17 齿，13 RED→GREEN + 4 must-stay-green，redBaselineResult 13 failed/51 passed 实跑落账 taskpackage.json）；structural = M3K1-A5（live 半 deferred）。
- **机械校验**：`.claude/scripts/verifier-evidence-check.mjs --phase final --round 5 --invocation-id 487f57a8-…` integrator 实跑 **exit 0、`factOk:true`、`observedGateExit:0`**——final evidence 有效，可进 closeout。
- **integrator 独立复跑 demo 命令**：`python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance` → **75 passed in 0.59s**（2026-09-21 integrator 实跑，与 final r5 unit 闸 75 passed 一致）。
- **Impact audit（worktree 降级形态）**：detect_changes **N/A (worktree)**（Pod worktree diff 对主索引不可见）——替代抽查：① `git diff --stat` 闭集核对（见 Change Size，firmware / docs / .claude 零命中）；② 共享符号抽查（CodebaseMemory 图 KidPal 主索引 + grep 降级）：`group_upcoming` 消费闭集 = `week_grid_payload`（星标 bit1）/ `home_upcoming_payload`（items 单源）/ openclaw-schedule.config.yaml 注释引用 / 归档退役件（不入现役）——全部在本包预期面内，零跨层新增消费；`parse_schedule_md` 消费闭集 = 既有 openclaw-patch 面 + front_app 新接线 `_load_schedule_model`（本包接线，预期内）；③ `schedule_push` 消费闭集 = front_app 接线（决策面/接线分工在 docstring 钉住）+ 冻结齿/单测（反射断言）。
- **Docs sync**：`docsSyncRequired=true` 义务全清（契约门通道 `24bb1b8` + schedule.py 钉行 + CDQ + ADR-004），spec 面零改动——详见同目录 `docs-delta.md`。
- **S2 exploratory**：TaskPackage 未声明 `s2ExploratoryRequired` → 无 S2 official scan（非遗漏）。
- **Loop metrics / Pod Journal（post-cutoff dual-format）**：不产 `session-digest.md` / `loop-health.md` / `loop-metrics.json` / `pod-journal.jsonl`；`pod-events.jsonl` 在场（advisory 检查通过：human 契约门裁定 / builder 收工 / friction degraded+over-scan 各条已锚），缺失不阻塞 closeout。
- **机器证据投影**：`current/verdict.json` + `current/verification.json`（verify.mjs 程序 fs-write 投影，non-bearing advisory）已随本次收口 commit 刷新至本包 final r5 口径，并按 SSOT 表晋升副本至本 closeout 目录（只消费不重跑）。

## Demo 对账行（taskpackage `testHost` 证据形态核对）

taskpackage 权威口径（`testHost` 逐字）：**「python -m pytest server/acceptance/ offline 齿；live 三触发 deferred 归 M3-K3 设备轮」**；plan.md demo 证据形态复述：offline 全量绿 + 契约门 CLOSED 路径声明 + M3K1-A5 deferred 挂账声明。逐项对账：

| demo 证据形态项 | 本包实际证据 | 对账 |
|---|---|---|
| offline pytest 全量绿 | `python -m pytest -q --import-mode=importlib -m "not live" server/tests server/acceptance` → **75 passed**（= 既有 47 全绿保持 + 本包冻结齿 17 + 新单测 11；final r5 unit 闸同口径 75 passed，integrator 独立复跑一致）；红基线链：17 齿落盘时 13 RED + 4 must-stay-green（13 failed/51 passed，acceptance-author 实跑落账）→ 实现后全绿 | 一致 |
| 契约门 CLOSED 路径声明 | CONTRACT-CHANGE-PACKET Status = **HUMAN_APPROVED**（2026-09-21 human 裁五问，packet §5）→ gate evidence Status = **LANDED**（C1–C4 全 done）→ 契约 diff + schedule.py 钉行随 `24bb1b8` 落地 → CDQ-260921-m3-k1-schedule-grid = **LANDED**；contract-lint 闸绿（final r5） | 一致 |
| M3K1-A5 live 三触发 deferred 挂账声明 | A5 为 structural 条目在册（acceptance-coverage 闸 structural 列表实证）；live「联网/零点/录入三时点设备实际刷新」目击证据归 M3-K3 设备轮（M3K3-A3，crossPackageBehaviors 第 3 条） | **一致但未闭环**——口径本即 deferred（offline 齿钉不到真 ws 建连帧到达 / 真零点调度，见未覆盖面申报①） |

**一致性结论：demo 证据 == roadmap 效果段承诺。** roadmap §3 K1 条目「demo 证据形态」= 「python -m pytest server/acceptance/ offline 齿（网格计算/upcoming payload/三触发模拟）+ 契约门 evidence 制品；live 三触发复证 deferred 归 K3 设备轮」——offline 齿全量绿（75 passed）+ 契约门 evidence 制品齐（LANDED、三样本证据在包）+ A5 deferred 挂账在案，三项全对账一致；效果段承诺的「一周网格 / 接下来分组 / 三触发 / 契约对齐」四件事均在 offline 可验半落地（live 半显式挂账）。

## 未覆盖面申报（对照 acceptance[] + 契约 §1.3②/§1.3⑥ + roadmap §3 效果段，逐条列「本次无齿/未跑的设计性质 + 现状 + 去向」）

| # | 未覆盖面 | 设计性质与现状 | 去向 |
|---|---|---|---|
| 1 | **live 三触发真机复证（M3K1-A5）**——真机联网/零点/录入三时点首页与课表页实际刷新目击 + 引擎/front 日志关联取证 | structural 条目在册、本包未跑：offline 齿钉不到真 ws 建连帧到达与真零点调度（验收件文件头显式缺口①「行为等价假设『front 接线每触发恰调一次声明面』offline 不可验」）；决策面/接线代码已验（A4 模拟齿 + 单测） | **M3-K3 设备轮**（M3K3-A3，crossPackageBehaviors 第 3 条；单真机不两轮撞车，K1 offline 齿先行） |
| 2 | **preview.time 裁决点**——契约 §1.3⑥ 示例 `preview.time="明早"` vs 板端 seam `hp1_item_t.time ≤5 字节（HH:MM）` 冲突 | 齿按 seam 兼容口径钉：preview.time = 明日首项 HH:MM（producer 实现与冻结齿一致）；「明早」判设备渲染标签（K2 落）——progress.md 2026-09-21 节 planner 逐字核对留观 1 条在案；如 human 契约门本意是载荷直发标签需回裁 | **K2 消费时如有异见回契约门**（K2 落渲染标签时撞 seam 即升级，不就地改齿口径） |
| 3 | **触发①②真实接线 offline 钉不到**——真 ws 建连帧到达 / 真每日零点调度 | 决策面（`connect_push_payload` 单源 / `daily_push_due` 零点判定）+ 接线代码（front_app 建连即推 + 轮询 piggyback）已验（A4 齿 + 11 单测）；真实 ws 帧/真实时钟 tick 的端到端到达 offline 无齿 | **live 归 K3 设备轮**（同 1）；offline 半已被 A4 模拟契约钉住 |
| 4 | **上午/下午/晚上分界时刻未钉**——契约 §1.3② 未定义 slot 分界时刻 | 契约显式缺口（验收件文件头缺口③）：齿只钉无歧义样本（10:00→上午、16:30→下午、18:30/19:00→晚上）；builder 实现处留痕常量 `_PM_START=12:00` / `_EVENING_START=18:00`（schedule.py 注释在案），分界时刻样本入单测 | **K2 消费如与板端直觉冲突回契约门**；当前契约未钉、留痕口径即事实口径 |
| 5 | **timezone 换算只钉 Asia/Shanghai**——其它时区行为不钉 | 显式缺口②：B1 唯一在役时区 Asia/Shanghai（UTC+8 house 先例，host 无 tzdata 禁 zoneinfo）；非在役 zone `daily_push_due` ValueError 大声拒绝（单测在案） | 无行动——非在役面大声失败是设计态；如未来 B1 出现新时区随包扩齿 |
| 6 | **学校 >6 节 slot 归属不钉**——spec 口径 4+2 | 显式缺口④：齿按 spec 4+2 口径（第 1–4 节→上午、5–6 节→下午）；>6 节行为不在齿域 | 无行动——spec 口径外输入，出现时属 spec 变更走相应通道 |
| 7 | **机械门收紧深度 = 形状 floor**——逐字段类型校验深度不钉 | 显式缺口⑤：validate_display 收紧钉到 legacy/缺 courses 拒绝（Q5 裁定口径），builder 实现未超出 floor（四键在场校验），逐字段深度类型校验不承诺 | 无行动——K2 消费如需更深校验随包扩（builder 可更深、非义务） |

以上 7 条**零一条是本包义务内缺口**（1/3 为口径内 deferred，2/4 为显式留观裁决点，5/6/7 为验收件文件头显式申报的「弱于契约处」钉深边界）——故不写 `none`，如实挂账。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | server/gateway/front_app.py | **触发②每日零点推依赖 front 进程跨日常驻且 `_daily_push_local_date` 进程内存记账，front 重启即丢记账**——重启后首个轮询 tick `last_push=None → due=True` 立即补推一帧。行为正确性无损（home upcoming payload 幂等、设备重复收到同内容刷新一次），但「每 local 日恰一次」的恰一次性在重启窗口不成立 | changed-code hunk（front_app 接线 `_daily_push_local_date: str | None = None` 进程内存初值 + schedule_push `daily_push_due` docstring「last_push=None -> True（尚未推过）」）+ 启发式：alerts/pending 既有面均有落盘状态、本记账未沿用（内存记账是最简实现） | 不 BLOCKING 的理由：补推内容与首推逐字节同源（单源函数）、设备侧刷新幂等、S-F1.5/决策 26 无「每日本地日恰一次」的硬语义 bound。如 K3 设备轮 live 目击重复刷新扰人，后续批把记账落盘（pending.json 同目录即可） |
| NIT | judgement-call | server/gateway/schedule.py | `_slot_of` 用字符串字典序比较 HH:MM（`hhmm >= _PM_START`），依赖「时刻恒为零填充 HH:MM」前置——脏输入若带非零填充形态（如 `9:00`）字典序会错格 | changed-code hunk（`_slot_of` 比较实现）+ 启发式：上游 `parse_schedule_md`/`items_for_date` 已对时刻 token 做正规化断言（既有冻结语义），非零填充形态在模型入参前已被拒绝，`_slot_of` 只接正规化输出 | 不动；如 K2 消费发现异常落格样本，随包补一条正规化前置断言 |
| NIT | judgement-call | server/deploy/openclaw-patch/reminder_schedule_lib.py | `drop_refresh_marker` 吞 OSError 全静默（尽力而为设计），主通道重算成功但 marker 落笔失败时无日志痕迹，front 侧触发③退化为「靠①②兜底」且云端无观察点 | changed-code hunk（`except OSError: pass` + docstring 明示尽力而为语义）+ 启发式：该模块 main() 已有 stdout 摘要行、可低成本加一行 marker 结果回显 | 可并入 K3 部署批顺手加一行 stdout 回显（`marker dropped/skip`）；非义务 |

**结构闸结果**：final r5 九闸全绿（结构闸 PASS/FAIL 不复述为 finding）；结构闸之外，author 五轴语义 review 段完整执行（见下）。

## Five-Axis Review（author 语义审查）

1. **正确性**：周网格计算逐格断言（day 0 基映射、4+2 节归属、※/星标 flags 位掩码、周外过滤）在 A2 齿行为级钉住；home upcoming items = `group_upcoming` 同参零偏移复用（决策 26 合并「上学」12:00 回归齿在册）；三触发内容单源（`connect_push_payload` == `home_upcoming_payload` 同参断言齿在册）；Q5 收紧双向钉住（legacy 拒绝 + 新形状/薄载荷/home 带 data 放行）。已知边界 = Findings 第 1 条（重启补推）+ 未覆盖面 2/4。
2. **边界**：脏 B1 解析失败归日志、轮询照走（不炸循环）；marker 尽力而为、单腿失败不连坐；`daily_push_due` 非在役 timezone ValueError 大声拒绝（不静默吞）；MAX_DATA_BYTES 沿用既有上限（周网格量级远低于 64KB，packet Impact 预告一致）。
3. **契约**：契约面唯一命中 = 契约门通道 `24bb1b8`（HUMAN_APPROVED 在案、CDQ LANDED）；§1.3⑥ home data additive（缺 data 行为不变，must-stay-green 齿在册）；flags 引入 UI 语义态 = packet 明示的理念反转（旧钉行撤除随同批）；三触发行为策略不进契约（权威 = arch §5.2/决策 26）按裁定落实现。
4. **安全**：`securityReviewRequired=false`；payload 全由云端 schedule.py 单源机械计算（非 LLM 生成，注入面收敛）；marker 为空文件信号、无内容注入面；display 下发链经既有 `_relay_display_blocks_to_device` + display_schema 机械门（home 形状必过）；无新增依赖。
5. **可运维性**：MARKER_FILENAME 跨组件常量两侧钉同一名字（schedule_push / reminder_schedule_lib 互指注释）；时区固定 UTC+8 house 先例（host 无 tzdata 约束在 docstring 钉住）；front 日志对脏 B1/单腿失败有归因输出；ECS 部署提醒 patch 落地走既有 apply 脚本（部署执行归 K3 批或 planner post-merge，本包不改 OpenClaw 本体架构，M3K1-E3 守住）。

## Simplification Debt

`grep "minimal:"` 本包全部实现/测试文件**零命中**——无简化债务台账条目，全部 `no-trigger`。

## Residual Risk

- **decisionCoverage**：TaskPackage `decisionCoverage: []`（空）——无 `landsIn: dropped && reason: risk-accepted` 条目，无 human 冒险接受项需列示。
- 残余风险实质 = 未覆盖面申报表 7 条（live 半 + 显式留观/钉深边界），无新增高风险敞口：live 三触发是口径内 deferred 且有 offline 行为级齿垫底；preview.time / 分界时刻两裁决点均留痕在案、撞上即回契约门，不存在静默漂移路径。

## 合并门输入摘要

- 前置依赖：无（`dependsOn: []`；契约落地批 `24bb1b8` 已在本分支基线）。
- 回滚方式：单 commit revert 本包实现批即可整体回退（server 面零跨包耦合；`24bb1b8` 契约批独立成 commit、revert 不连带）。
- supervisor 判读输入：final r5 `ok:true` + evidence-check PASS + demo 对账一致 + 7 条未覆盖面全部口径内挂账 + findings 零 BLOCKING（1 SHOULD + 2 NIT）。
