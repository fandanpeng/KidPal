# Review Packet · 260923-m3-k7-schedule-grid-restore

> workstream m3-schedule ｜ M3-K7 课表网格冷启动恢复（Shape A restore 语义：云端三触发附带推 `data.restore` 周网格恢复性载荷，板端只填缓存不接管前台）｜ rigor full ｜ riskTier medium ｜ subsystem firmware+server ｜ gateProfile [test-quality, cross-end-consistency] ｜ sourceType arch-review ｜ origin = 上游 260923-arch-review-schedule-grid-cold-boot（Track A full-adversarial r1 收口：adjudicator GREEN + F-C5 human 裁 accept-as-risk + 契约门批准 C1 挂账 + landing-verifier GREEN，commit `565cb2d`）。
> integrator 收拢制品（2026-09-23）；合并门判读归 planner 主线程 `supervisor-verdict-gate.mjs` 机械闸，本包不 push、不 merge、不在 main 上 commit。dependsOn []（K6 已 merged，常驻树/常驻分支 `dev/260915-m1-k1a-key-layer-dispatch` 续棒）。

## Change Size

- **实现（工作树未提交改动 → 本次收口 commit）**：代码 7 文件（+226 / -38）。
  - `server/gateway/schedule_push.py`（+41）：ECS 开关常量 `GRID_RESTORE_ENV = "KIDPAL_GRID_RESTORE_PUSH"` + 识别集 + 纯函数 `grid_restore_enabled()`（默认关偏置）+ 变体 producer `grid_restore_payload()`（单源委托 `week_grid_payload`，置 `data["restore"]=True`，键集恰 `{week,restore}`）。
  - `server/gateway/front/front_app.py`（+109 / -22）：新增 `_push_schedule_connect()` 建连推送缝（收编既有内联 home 推 + 追加 grid restore relay，两推各自 try/except 不阻断建连）；`_push_schedule_refresh_to_legs()` piggyback 同型追加 grid（开关门控）；`run_schedule_poll_once()` 合成单批（不新增第二套进程内日账）；`ws_gateway` 建连点改调 `_push_schedule_connect`。
  - `firmware/main/mcp_server.cc`（+78 / -21）：新增 `mcp_json_get_bool()`（absent/非 true 字面 → false）；schedule 路由读 `data.restore` → true 走恢复分支：显式 `lvgl_port_lock` 写缓存 + 仅当 `schedule_on_screen()` 才重绘 P2（**不调 `begin_page`**、不推 SERVER_PAGE），parse 失败静默记日志（不 SetChatMessage）；薄载荷路径语义零改。
  - `firmware/main/display/kidpal_render_node.h`（+9）/ `.cc`（+10）：新增 `kidpal_render_node_schedule_on_screen()`（沿 `home_on_screen` 先例逐字同构，LOCAL_PAGE + KP_PAGE_SCHEDULE 判定，只读零副作用）。
  - `firmware/main/display/schedule_page.c`（+7 / -1）：`ksp_copy_str` 增 `memset(dst, 0, cap)` 零填充（**planner 扩展授权**，memcmp 确定化——见 docs-delta「授权留痕」+ Findings）。
  - `firmware/main/test/host_app/main/CMakeLists.txt`（+10）：`HOST_ACCEPTANCE_SRCS` 注册 `schedule_grid_restore.acceptance.c`（置于 host smoke 之前）。
- **冻结验收面（acceptance-author，已 commit `842419b`，本包零改）**：`server/acceptance/test_schedule_grid_restore.acceptance.py`（844 行，16 齿）+ `firmware/main/test/schedule_grid_restore.acceptance.c`（863 行，5 TEST_CASE，含 MUTATION REPLAY MAP M1..M5）+ `firmware/main/test/acceptance/red-baseline/k7_red_wrapper.sh`（通道②独立线束）。
- **delivery 制品**：taskpackage.json（本包 finalize + `current/` 指针）、`verify-runs.jsonl`（7 轮）、`verifier-evidence/rounds/r1.json`（本包 final 承重 fact，integrator 实跑落）、`mutation-evidence/`（7 件，planner 代落）、本目录 closeout 制品。
- **Scope**：firmware + server 双端内聚，跨端 wire 复用既有 `kidpal/display` tools/call 单帧（零新消息类型、零协议面触碰）；`schedule.py` 单源零改。
- **Split recommendation**：单包，不拆。

## Verification Evidence

- **Structural gates**：final 九闸全绿（`delivery/taskpackages/260923-m3-k7-schedule-grid-restore/verifier-evidence/rounds/r1.json`，phase `final`，round 1，invocationId `b3aeb5a1-49aa-491f-8a7f-944aba7b6d0d`，`ok:true` / `observedGateExit:0`，integrator 2026-09-23 第一方实跑）：taskpackage-required、firmware-build（真编译，`xiaozhi.bin` 0x30e840 bytes，app 分区 0x370000，**0x617c0 bytes = 11% free** 体积余量在案）、contract-lint、acceptance-coverage（K7-A1..A8 八条全覆盖）、mutation-check、docs-sync（false + reason 同口径）、security、unit、acceptance。
- **机械 fact 校验（integrator 实跑 2026-09-23）**：`node .claude/scripts/verifier-evidence-check.mjs --change-id 260923-m3-k7-schedule-grid-restore --phase final --round 1 --invocation-id b3aeb5a1-49aa-491f-8a7f-944aba7b6d0d` → **exit 0 / `factOk:true` / `observedGateExit:0`**。**closeout 判断以 r1 final fact（ok:true）为唯一承重机器件**；本目录 `verdict.json` = r1.json 字节级副本（脚本 fs-write 复制，非 agent 手写），`verification.json` 本包未另立——缺失按 closeout SSOT 为 advisory-only，不阻塞。
- **Acceptance/unit/e2e（integrator 第一方复跑 2026-09-23）**：
  - **server pytest 新齿**：`python -m pytest -q --import-mode=importlib -m "not live" server/acceptance/test_schedule_grid_restore.acceptance.py` → **16 passed**（红基线态 = 10 failed / 6 passed，逐锚转绿）。
  - **unit 闸（full）**：**111 passed**。
  - **acceptance 闸（full）**：57 structural 齿 passed；oracle 面 14 firmware / 4 server 验收文件在世。
  - **firmware host runner（通道①，WSL 全量真跑）**：`espidf_wsl.sh build`（ninja: no work to do，臂面已编译）+ `host_infra_test.elf` → **103 Tests / 0 Failures / 0 Ignored，exit 0**（K6 记账 98/0 + K7 新齿 5 TEST_CASE = 103，**既有齿零偏移成立**）——K7-E1「firmware host runner 新冻结齿 build+run exit 0 + 既有齿零偏移」实测达成。
  - **firmware host runner（通道②，独立线束）**：`k7_red_wrapper.sh` → COMPILE_EXIT=0（-Wall -Wextra 零 error）+ **5 Tests / 1 Failures**——唯一 FAIL = TEST 4 的 weak 哨兵齿（standalone 线束无强符号，**设计态红**，其真验在通道① 103/0 由 `schedule_page.c` 强定义覆盖承载）；余 4 齿（A5 两齿 / A6 守卫 / E2 注册 canary）转绿。两通道签名与设计一致。
  - **device build 闸**：真编译过（见上体积证据）。
  - **真机 e2e 半 = deferred**（见「未覆盖面申报」与 C1 节）。
- **Security gate**：r1 绿（`securityReviewRequired:false`）。新代码无网络输入解析面、无新凭据面、无注入面；开关读 env 为纯函数（不可识别值一律 False，误配不误开闸）；ESP_LOG 无 secret。
- **Mutation check（K6 纪律延续）**：`mutationRequired:true`，八条 acceptance 全部 `sensitive:true` 登记于 taskpackage.json `mutationEvidence.perAcceptance`；闸绿。**变异实跑原始输出落 `delivery/taskpackages/260923-m3-k7-schedule-grid-restore/mutation-evidence/` 七件（planner 代落——builder scope 限制）**；逐条对照见下节。**覆盖差异如实记**：K7-A2 / K7-A4 两组变异无原始输出件（见 Findings SHOULD）。
- **Docs sync**：`docsSyncRequired=false`，r1 闸绿；详见本目录 `docs-delta.md`。
- **S2 official exploratory scan**：TaskPackage 未声明 `s2ExploratoryRequired`，不执行，无 S2 证据义务。
- **UI design-review 证据**：不适用（本包 UI 面 = restore 语义下**不接管前台**的**负向约束**——不推 SERVER_PAGE / 不 `begin_page` / 不 SetChatMessage，由 host 结构齿 + 计数恒值齿承载，渲染/版式链路零触碰；外观面 K7-A8②真机目测挂设备轮）。

## 跨端一致性抽查（gateProfile cross-end-consistency）

| 端 | 契约 §1.3② 落地 | 核对 |
|---|---|---|
| server（producer） | `grid_restore_payload(model, week_date_iso)` = `week_grid_payload(...)` 同参 + `data["restore"] = True`，`data` 键集恰 `{week, restore}` | 单源委托零复制；K7-A2 三齿断言 plain producer 与 expand 产物键集互异（restore 只能由本变体置） |
| wire | 既有 `kidpal/display` tools/call 单帧，`arguments = {view:"schedule", data:"<json.dumps(data)>"}`；`restore` 为 JSON 布尔 true | 零新消息类型；K7-A1 帧序齿实测 `[home, schedule]` |
| firmware（consumer） | `data = arguments["data"]`（`mcp_server.cc:537`）→ `mcp_json_get_bool(data,"restore")`（absent/非 true → false）；解析面 `kidpal_schedule_grid_parse` 经 `kspj_skip_value` 容忍 `true/false` literal（`schedule_page.c:283-287`），restore 键不使整帧被 -1 拒收 | K7-A7 TEST 4 tooth 1/2/3（restore 先/后位序、true/false）在通道①真跑绿 |
| 语义分叉 | restore true = 只填缓存不接管前台（显式持 LVGL 锁）；absent/false = 现行为（`begin_page` 接管） | K7-A5 TEST 1（锁 + 谓词 + begin_page 计数恒 5 + SetChatMessage 区计数恒 1）；K7-A6 TEST 3（薄载荷路径接管链零回归） |

结论：两侧对 `data.restore` 的置位/消费/容错一致，与契约 §1.3② + ADR-005 逐字对齐，无漂移。

## Acceptance 逐锚结果（K7-A1..A8）

| 锚 | 承载齿 | 结果 |
|---|---|---|
| K7-A1 server 建连帧序（home 先 grid 后）+ schedule.md 缺失/坏不阻断 | pytest 4 齿（帧序假腿 + 缺失/坏静默 + err 守卫在世） | PASS |
| K7-A2 薄载荷展开恒不置 restore（absent/false 同判） | pytest 3 齿（expand 输出 / producer 键集 / 完整载荷不被截获） | PASS（must-stay-green 设计态） |
| K7-A3 ECS 开关默认关 + 开关开正例 | pytest 2 齿 + A1 正例 + A7 默认关门齿 | PASS |
| K7-A4 alert 起轮时序零偏移 | pytest 3 齿（注册序位次 + 轮询循环序 + 投递闭环真跑） | PASS |
| K7-A5 firmware restore 消费（不新增 SERVER_PAGE + 零 SetChatMessage + 缓存幂等） | host TEST 1 + TEST 2（通道①） | PASS |
| K7-A6 语音接管回归守卫（M3-K3 冻结行为） | host TEST 3（通道①） | PASS |
| K7-A7 piggyback 单源同型 + wire 容忍 | host TEST 4（通道①真验）+ pytest 3 齿 | PASS |
| K7-A8 真机锚四子项 | 真机窗口（human 在场） | **deferred（挂设备轮，非豁免）** |

## Mutation replay 逐条对照（expectedRed ↔ 实测红 tooth；原始输出落 mutation-evidence/）

| 组 | acceptance | 变异 | 实测红 | expectedRed 对照 | 证据文件 |
|---|---|---|---|---|---|
| A1 | K7-A1 | 对调 `_push_schedule_connect` 内 home/grid 推序（grid 先） | pytest `-k a1`：1 failed / 3 passed，`['schedule','home'] != ['home','schedule']` | ✅ 命中帧序齿 | K7-A1_frame-order.md |
| A3 | K7-A3 | `grid_restore_enabled` 默认偏置翻转（→ True） | pytest `-k K7A3`：1 failed / 1 passed（`True is not false`） | ✅ 命中默认关齿 | K7-A3_switch-default-off.md |
| M1 | K7-A5 | 摘 restore 分支 `lvgl_port_lock(0)` | 通道② 5 Tests / 2 Failures，TEST 1 红（路由区无 lvgl_port_lock） | ✅ 命中 C3 canary 锁齿 | K7-A5_restore-consume-M1-M2.md |
| M2 | K7-A5 | `schedule_on_screen` 去 `== KIDPAL_LAYER_LOCAL_PAGE` 判定 | 通道② 5 / 2，TEST 2 红（谓词形态缺件） | ✅ 命中谓词形态齿 | K7-A5_restore-consume-M1-M2.md |
| M3 | K7-A6 | 重排 cache 到 render 前（破 begin_page→render→cache 序） | 通道② 5 / 2，TEST 3 红（冻结调用链缺件） | ✅ 命中 M3-K2 冻结链守卫 | K7-A6_voice-takeover-M3.md |
| M4 | K7-A7 | 摘 `schedule_push.py` 的 `grid_restore_payload` | pytest `-k a1_connect_frames` 1 failed（`['home'] != ['home','schedule']`）+ native TEST 4 tooth 4 单源 face 红 | ✅ 命中单源 producer face | K7-A7_single-source-M4.md |
| M5 | E2（注册 canary） | 摘 `HOST_ACCEPTANCE_SRCS` 注册行 | 通道② 5 / 2，TEST 5 红（未接线） | ✅ 命中注册序守卫 | K7-E2_registration-M5.md |
| A8 | K7-A8 | 杀自动恢复路径（restore 帧不发） | 真机观测面随失效 turned red（host 侧同因红由 A5 的 M1/M2 承载） | ✅（native 无齿，deferred 设备轮，非豁免） | K7-A8_real-device-deferred.md |

**覆盖差异（如实记）**：taskpackage `mutationEvidence[]` 列八条（A1..A8），实落七件；**K7-A2（expand 恒不置 restore，改置 true）与 K7-A4（`_register_alert_legs` 时序后置）无原始输出件**——二者 mutation + expectedRed 仅存 taskpackage `mutationEvidence[]`，未落 plan §4.1.2 要求的「逐 acceptance 一文件」原始输出。记 SHOULD finding（见下），交 planner 补跑或明示降级。**integrator 未自行补跑**：两组变异须编辑未提交的 K7 实现文件（`schedule.py` / `front_app.py`），而 `front_app.py` 承载本包未提交 diff，`git checkout` 还原会毁绿 diff；byte 级备份/还原风险高于收益，按 scope 归 planner。

## C1 closeout 挂账状态（ADR-005 部署时序 / 契约门 C1）

> C1 = 「builder 实测复核确认旧固件对 `data.restore` 载荷的确定性接管行为**之前**，ECS 推送开关保持默认关」（owner = 实现包 builder / integrator；required evidence 三项）。

| C1 evidence 项 | 状态 | 证据 |
|---|---|---|
| ① 开关实现 + 默认关状态交付 | **已满足** | `GRID_RESTORE_ENV`/`grid_restore_enabled()` 默认关；K7-A3 默认关齿 + A7 三触发默认关全盖齿绿 |
| ② 开关状态留痕（未开闸） | **已满足** | 本 review-packet + `docs-delta.md`；默认状态 = 关，本包全程未开闸、未设 env |
| ③ 旧固件行为复核实测记录 | **未满足（挂设备轮）** | K7-A8④（烧新固件前旧固件 + 开关开 = 确定性接管顶到 P2 实测）→ 设备轮窗口 human 在场收口 |
| ④ 烧板验证后开闸记录 | **未满足（human 动作）** | 开闸 = 真机验证后 human/integrator 人工动作，integrator **只备路径不执行** |

**C1 仍处「挂账」态**：机判半（开关存在 + 默认关）已交付并在齿上钉死；实证半（③④）挂设备轮窗口与 human 开闸。C1 生命周期语义（永续配置 vs 一次性闸）随烧板推进定案（契约变更包 §Closeout Conditions）。

## E2 / exitCriteria 铁律逐项复核（K7-E2）

| 铁律项 | 证据 |
|---|---|
| `docs/contracts/**` 与 spec 冻结面零改 | `git status --porcelain docs/` 零输出；contract-lint r1 绿；契约修改面由上游 `565cb2d` 承载 |
| `expand_schedule_intent_payload` 输出语义零改 | `server/gateway/schedule.py` 零 diff（不在变更面 7 文件）；K7-A2 三齿在绿 |
| M3-K3 语音接管 / alert 起轮 / home upcoming / BOOT 翻页读路径 四冻结行为零回归 | K7-A6/TEST 3 守卫绿（begin_page + SERVER_PAGE push + 调用链序）；K7-A4 三齿绿；host runner 既有 98 齿零偏移（103/0 中 98 与 K6 记账一致）；BOOT 读路径零 diff |
| 决策 24 介质禁令（全程 RAM，零 NVS/Flash/SD） | 变更面无 NVS/Flash/SD 写 API（restore 只写 RAM 缓存 `kidpal_render_node_cache_schedule_grid` + `s_grid_courses`） |
| ECS 开关默认关交付 | K7-A3 齿绿；开关默认关（未开闸） |
| K7-E1 host runner 新齿 build+run exit 0 + 既有齿零偏移 | integrator 实跑 **103 / 0 / 0，exit 0**（见 Verification Evidence） |

## Detect Changes / 影响面核对

- `detect_changes`（codebase-memory `KidPal`）：**N/A (worktree 未提交 diff)**——本包改动全部为未提交工作树 diff，工具对 committed range 生效。替代核对（如实记录）：
  - ① `git diff --stat` 全量在案（实现面 7 文件 +226/-38，见 Change Size）；
  - ② 关键共享符号 `trace_path` 抽查——`kidpal_render_node_cache_schedule_grid`（restore 与薄载荷共用**唯一**缓存写点）inbound 调用者恒 2（`mcp_server.cc` schedule 路由薄载荷分支 + restore 分支，二者同函数内互斥分支，无预期外调用面）；`week_grid_payload` 被 `_push_schedule_connect`/`_push_schedule_refresh_to_legs`/`expand` 三处消费，均既有单源路径；
  - ③ 变更面文件级核对——修改文件全部在派单变更面清单内（`schedule_page.c` 单点例外见 Findings NIT），无清单外文件；
  - ④ 图索引 re-index 需求（`kidpal_render_node_schedule_on_screen` / `grid_restore_payload` / `grid_restore_enabled` 新符号未入图）交主线程/human 定窗口，不阻塞本包。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | 证据完整性 | K7-A2 / K7-A4 两组变异缺 mutation-evidence 原始输出——plan §4.1.2「逐 acceptance 原始输出落 mutation-evidence/（逐 acceptance 一文件）」未全覆盖 | plan.md §4.1.2 + taskpackage exitCriteria「mutation 实跑落 evidence…逐 acceptance 原始输出落」；`mutation-evidence/` 实列 7 件（A1/A3/A5/A6/A7/A8/E2），A2/A4 无对应件，其 mutation+expectedRed 仅存 taskpackage `mutationEvidence[]`；mutation-check 闸绿（闸只校 perAcceptance sensitive 登记，不校原始件） | planner 补跑 A2/A4 两组变异落两件，或明示降级理由并入 pod-retro；未补不得宣称该 exitCriteria 条全清 |
| NIT | judgement-call | 治理过程 | `schedule_page.c` `ksp_copy_str` 零填充（`memset(dst,0,cap)`）落入实现，但**未登记进 taskpackage 变更面**——轻量 scope 修正通道程序要件「planner 带时间戳补登记 taskpackage」当前缺 | plan.md §3 变更面列 `mcp_server.cc`/`kidpal_render_node.cc`，未含 `schedule_page.c`；taskpackage `notes` 禁止改清单亦未含之；代码注释载「planner 扩展授权（260923-m3-k7 裁决）」（授权在、登记缺）；本件 `docs-delta.md` 已留痕 + 必要性论证（frozen K7-A7 memcmp 保真齿） | planner 补登记 taskpackage（或 supervisor `degradationsConfirmed` 承接）；不改行为 |
| NIT | judgement-call | 文档一致性 | frozen server 验收文件头注 `sourceType` 标 `arch-change-handoff`，而 taskpackage 已定为 `arch-review` | `server/acceptance/test_schedule_grid_restore.acceptance.py:2` 头注 vs taskpackage.json `sourceType`；verify-runs.jsonl 首轮红「origin.type (arch-review) must equal sourceType (arch-change-handoff)」→ verifier 按 taskpackage-required 修正为 arch-review | 无（纯注释、不触验收语义；如后续冻齿修订顺带对齐） |

Findings 说明：五轴 review 全执行（见下节）。除上表 1 条 SHOULD + 2 条 NIT 外无 BLOCKING。**SHOULD 与 NIT 均不阻塞合并门**（结构闸/功能闸/安全闸/契约面全绿；SHOULD 为证据完整性、NIT 为登记/注释一致性，均交 planner 裁）；结构闸 PASS/FAIL 不复述为 finding。**若 planner/supervisor 将 SHOULD-1 判为须并门前消项，请按 Red Flags 退回本件重走 closeout**。

## Five-Axis Review

1. **Correctness（对每条 K7-Ax 的边界核对）**：K7-A1——`_push_schedule_connect` 先 home 后 grid、两推各自 try/except、`_load_schedule_model()` None 静默 return（缺失/坏 schedule.md 不阻断建连，try/except 语义保持）；帧序由假腿真帧序钉（非源锚）。K7-A2——`expand` 走既有单源、产物键集与 plain producer 逐字一致、恒不置 restore（absent/false 同判 = 输出不得携带 truthy restore）；完整网格+restore 载荷不被 thin-payload 判据误截获。K7-A3——`grid_restore_enabled` 纯函数、不可识别值一律 False（默认关偏置，误配不误开闸）；A1 正锚承载「开关开 = relay 发出」。K7-A4——`_register_alert_legs → _ensure_alert_loop → 建连推送` 位次零偏移 + 轮询循环 alert 先 schedule 后 + 真跑一轮投递闭环零偏移。K7-A5——restore 分支显式 `lvgl_port_lock`（无 `begin_page` 隐式持锁）、`schedule_on_screen()` 判定（LOCAL_PAGE + KP_PAGE_SCHEDULE，接管态返回 0）、缓存写点唯一、parse 失败静默降级（不 SetChatMessage）；`begin_page` 计数恒 5 / SetChatMessage 区计数恒 1 负 needle 证「不接管前台」。K7-A6——薄载荷路径 `begin_page → render → cache → end_page` 冻结链与 SERVER_PAGE push 零改（must-stay-green）。一处实现更正申报已复核：restore 分支把 `ESP_LOGI(schedule_grid_render…)` 换成 `schedule_grid_restore … cached=%d`，不动 drawn 语义（薄载荷路径 drawn 日志保留）——与 plan 方案形状一致，✓。
2. **Test quality**：server 16 齿 + host 5 TEST_CASE 全结构/行为混扫（真实工作树 grep 结构齿 + 假腿行为齿 + weak 哨兵行为齿）；负向齿自证能红（红基线 10+4 齿红 + 七组变异实测每组红）；MUST-STAY-GREEN 半（A2 三齿 / A4 闭环齿 / A7 默认关门齿 / 既有 98 齿）在红基线与变异中保持绿；断言钉位次 + 键集 + 计数恒值，非实现镜像。**缺口如实**：A2/A4 的变异敏感度缺第一方原始输出（见 Findings SHOULD）。
3. **Contract/boundary safety**：`docs/contracts/**` 零触碰（git status 实证 + contract-lint 绿）；spec 零改；跨端只消费上游已落的 §1.3②（additive + modifying），wire 复用既有单帧；`expand_schedule_intent_payload` 语义零改；`device_state_machine.cc`/`protocol.h` 零 diff。跨端一致性抽查（本件专节）两侧对齐。
4. **Operability/security**：restore 载荷全程有日志锚（`schedule_grid_restore … cached=` / 两推 err 分别 `schedule_connect_home_push_err`/`schedule_connect_grid_push_err`/`schedule_grid_leg_err`），单推/单腿失败不连坐、不外抛、不阻断建连；开关逻辑 fail-safe（不可识别值 = 关）；无网络输入解析面（板端只读已收 JSON）、无凭据写入、ESP_LOG 无 secret。
5. **Maintainability/simplicity**：server 面 `grid_restore_payload` 单源委托 `week_grid_payload`（deletion test：删该变体，网格计算无第二承载，正确——薄模组风险为零）；`_push_schedule_connect` 把建连内联推收编为单缝（三触发共 `_push_schedule_refresh_to_legs` 单源，不复制日账）；firmware 面 `mcp_json_get_bool` / `schedule_on_screen` 沿既有先例逐字同构（无新范式）；缓存写点唯一（不给 restore 另起第二条路径）。`minimal:` 标记 grep 本包变更文件零命中——**Simplification Debt 台账为空**（下表）。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| （无） | 本包变更文件 `minimal:` 标记 grep 零命中 | — | no-trigger |

## 未覆盖面申报（workstream 包硬要求）

| 项 | 设计性质 | 现状 | 去向 |
|---|---|---|---|
| 冷启动真实恢复（P1 恢复 + P2 网格有内容 + 接收日志） | 真机观测锚（host 半：restore 消费分支 + 在屏谓词 + 缓存幂等形态已机判） | host 半已绿（103/0 + 七组变异自证敏感） | 真机窗 K7-A8①（human 在场，开关开后） |
| 开关开闸端到端（ECS 置 env → 真机收 restore 帧） | 部署面实证，offline 钉不到真实 TLS/WS 建连握手时 front 真发两帧 | offline 以「ws_gateway 调 `_push_schedule_connect` ≙ 建连帧序」行为等价假设（A4 时序源锚兜底） | 真机窗 K7-A8④ / C1 开闸（human 动作） |
| 旧固件对 `data.restore` 的确定性接管复核 | 真机实测（仓内代码已定案，复核确认而非定案） | 未实测 | 真机窗 K7-A8④（C1 evidence ③） |
| 语音「我的课表」接管照旧（开关开/关两态） | 真机目测（host 半：K7-A6 冻结链守卫绿） | host 半已绿 | 真机窗 K7-A8③ |
| 每日零点真节拍（asyncio 调度） | offline 以注入 `now_utc` 行为钉恰一次；真节拍归 M3-K1-A5 遗留 live 面 | offline 已钉（A7 帧序/恰一次） | live 面（非本包） |
| F-C5 LLM 幻觉 restore 路径 | accepted-as-risk（ADR-005 残余风险段；ARQ-260923 转 watchlist） | 不设运行时闸（human 裁） | 如未来整体收紧 LLM 直产 display 面时随收口做 |

## Residual Risk

- **decisionCoverage 核验**（taskpackage.json）：字段为 null/未设——**无 `landsIn: dropped` 且 `reason` 以 `risk-accepted` 开头的条目**，无需列示 human 已接受风险项。本包最相关的已接受风险为上游 ADR-005 的 F-C5（LLM 幻觉 restore 路径），承载在 ADR-005 残余风险段（非 taskpackage decisionCoverage），记于此供合并门参考。
- **Known limitation**：
  - **C1 未闭**（尾项③④挂设备轮 + human 开闸）——合入 main 的是「固件先行 + 开关默认关」形态：**开关关 = 新恢复能力在 ECS 侧禁用**，本包对线上行为为零影响（灭活性交付）；设备轮 + human 开闸后能力才生效。合入前请把「C1 仍挂账」计入门控权重（本包 riskTier medium；开闸动作在 human，不在 Pod）。
  - **K7-A8 四子锚真机半 deferred**——host 证据不构成真机实证，设备轮收口前不宣称 S-F1.2 AC1 冷启动在真机成立。
  - **两个 NIT + 一个 SHOULD**（见 Findings）随包入 main，交 planner 收尾处置。
  - `delivery/milestones/STATUS.md` 相对 `milestones.jsonl` 存量漂移（M3 `planning` vs `in_delivery`、在途 20% vs 30%）——**K7 前的既有漂移、非本包引入**；本包 `deliveryImpact=milestone-affecting` 但为**中段 change、无阶段跳变**，故未动 milestones.jsonl、未重生成 STATUS.md（integrator 曾误触发一次重生成，已 `git checkout --` 还原到 HEAD，零残留）。若主线程欲消此漂移，单独跑 `node .claude/scripts/delivery-status.mjs` 即可（`--check` 现报 stale）。
- **Follow-up recommended**：
  - planner 处置 Findings SHOULD-1（补跑 A2/A4 变异或明示降级）+ NIT-1（补登记 scope 扩展）；pod-retro.md（planner 收尾相位）消费本件 + 变异覆盖差异。
  - 设备轮按 K7-A8 清单收口（①P1 恢复 + 接收日志 ②直翻 P2 网格有内容 ③语音接管两态 ④旧固件复核），落 progress.md K7 段终账 + C1 evidence。
  - 图索引 re-index（三个新符号）交主线程/human 定窗口。
- **Human decision needed**：① C1 开闸（烧板验证后人工动作，真机轮窗口内）；② 若 planner 将 Findings SHOULD-1 判为并门前必消项，退回本件重走 closeout。

## 回滚方式

- 单 commit 收口（SHA 见交付报告）；回滚 = 分支上 `git revert <sha>`（单 commit 自含实现 + 制品，无跨 commit 依赖）。实现面回滚等价于回到 K6 收尾态（各改动带 `K7`/`260923-m3-k7` 语义注记锚，逐段可辨；`schedule_page.c` 为零填充单点，还原 1 行）。
- **双保险（功能灭活）**：即使不回滚，功能亦处**默认灭活**态——ECS 侧 `grid_restore_enabled()` 未开闸（默认关，不设 `KIDPAL_GRID_RESTORE_PUSH`），grid restore relay 不发出，线上行为与 K6 态逐字等同；固件侧 restore 分支仅当收到 `restore:true` 才走恢复语义，旧/新固件与开关关态下均不影响既有「接管前台」行为（旧固件另走 ADR-005 定案的确定性接管）。
- 上游锚可独立复跑比对：server pytest K7 面 16 齿 + 既有 111；host runner 通道① 103/0 与 K6 记账 98/0 比对零偏移。