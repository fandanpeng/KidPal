# Review Packet — 261104-k2-points-device-ui

> integrator 收口（2026-11-05，本树 dev/pod-m2，包级 base ref b96b978d，HEAD eadb371f）。
> 收口形态 = 离线面收口 + 诚实挂起：K2PTS-L1/L2/L3 真机轮待跑（含 K1L-L1/L2 挂接）、
> 六 V9 帧 design review 待真机看屏对表；本包不宣告 ready-to-merge，终裁归 supervisor 门。

## Change Size

- Files changed（实现面，b96b978d..HEAD 已提交 18 文件 + 工作树 3 件未提交测试面 +
  2 件未跟踪新冻结件）：firmware 显示/路由/协议/mcp 面 points_page.c +250 新件、
  points_page.h +139、kidpal_render_node.cc +518/−（积分正式渲染三分派）、
  mcp_server.cc +395/−（积分 display 路由族 + 删除上行驱动）、keybar.c|h（points_slots
  键帽槽）、view_router.c|h（积分页路由）、story_page.c +9（积分语境映射落点）、
  lcd_display.cc、application.cc（会话积分语境）、protocol.cc|h（kidpal_points_delete
  上行 + 取页请求帧）、host_app CMake 登记守 +10；server front_app.py +109/−
  （积分意图截获 + 语境摘要注入 + 分页 limit=limit）。
- 测试面：新冻结件两件——firmware/main/test/points_device_ui.acceptance.c（K2PTS-P1..P7，
  sha256 26f991de…）+ server/acceptance/test_points_device_ui.acceptance.py（K2D-A1..A4，
  sha256 0339c8ba…）；重锚两件 additive（keybar_three_slot +109、record_session_live +73，
  删除行 0/0）；重钉一件（test_points_e2e_spike.acceptance.py 66 ins / 20 del，
  sha256 521d6874…，planner ruling5 K0PTS-A5 wire 半）。
- Net diff：实现面 1609 insertions / 73 deletions（已提交部分）；实际 diff ⊆ 23 路径
  changeManifest（change-manifest 闸 final 两轮核过，round 1 存量放行 + 工作树对账绿）。
- Scope：设备积分本正式化（四态主页/进度线/详细账本/删除确认/keyset 取页/积分语境/
  语音闭环）+ front 积分意图消费与分页步进修正。零契约/requirements/architecture 改动、
  零 story 数据域触碰、K1 权威账本零重算零本地持久化（K2D-A4 单写入口锚断言钉死）。
- Split recommendation: 不拆——K2 单包承载设备积分本全链，mergeOrder 73（依赖 K1 已合）。

## Verification Evidence

- verifier 内循环相位（planner 现跑）：**8/8 GREEN**（taskpackage-required /
  change-manifest / firmware-build / contract-lint / acceptance-coverage /
  mutation-check / docs-sync / security），两轮收敛（第 1 轮红 = perAcceptance 空骨架
  planner 落笔缺陷，fixer 补证后第 2 轮全绿，verify-red/verify-green 事件在 pod-events）。
- **final 相位（integrator，scope 圈定后实跑）**：scope 清单落盘路径 =
  `delivery/taskpackages/261104-k2-points-device-ui/test-scope-final.txt`（一行锚：本
  final 终验全部跑测以该清单为范围，detect_changes（C-AI-KidPal-pod-m2 专属索引，
  since=b96b978d，seed_symbols=278 / impacted_total=255）+ impactedTestsSeed 合成；
  该路径已记入 verify-runs.jsonl final 轮元数据）。final 两轮：
  - **round 1**（invocationId 811580fe-42e1-4d37-b5e2-7b577971dc0f）：10 闸 ok，但
    acceptance 闸 executor-skipped（scope 清单路径行尾带中文注记 → 闸逐行精确集合匹配
    交集为空）——**诚实留痕不修 fact**；scope 清单格式修正（注记改 `#` 行）后 round 2 重跑。
  - **round 2（承重 final）**（invocationId 3fb1729b-b9fa-47bd-9629-d71f696776f2）：
    `verify.mjs --evidence-mode=round --phase=final --scope-file=<上述清单>` → **ok:true，
    10 闸全跑零 notExecuted**：taskpackage-required ✓ / change-manifest ✓ /
    firmware-build ✓（**VERIFY_FW_BUILD_REQUIRED=1 真编译 fresh 重跑绿**，xiaozhi.bin
    0x328830 字节、app 分区余量 8%）/ contract-lint ✓（1 条存量 warning：arch 历史批
    相对链接断链，非本包引入）/ acceptance-coverage ✓（K2PTS-P1..P7 + K2D-A1..A4
    十一条全覆盖）/ mutation-check ✓（perAcceptance 11/11 对齐）/ docs-sync ✓
    （docsSyncRequired=false 豁免）/ security ✓ / unit ✓（scope 内 8 件 server 测试
    **154 passed / 3 subtests**）/ acceptance ✓（scope 内 5 件 server 冻结验收
    **73 passed / 3 subtests**）。
  - **承重机器件校验**：`verifier-evidence-check.mjs --phase final --round 2
    --invocation-id 3fb1729b-…` → **`{"ok":true,…,"factOk":true,"observedGateExit":0,
    "phase":"final","round":2}`，exit 0**——final evidence 有效，fact ok:true 进 closeout。
  - verifier-round fact 落 `verifier-evidence/rounds/r1.json`（留痕）+ `r2.json`（承重）；
    verify-runs.jsonl 全程 append。
- **圈定合跑（integrator 独立实跑，round 2 闸内同集复证）**：8 件 server 面逐件实测
  ——本包冻结件 4 / K0PTS-A5 重钉件 6 / K1 producer 冻结件 10 / m4k1 种子回归 36 /
  record_dialogue_guide 共享缝 17 / story units 40 / engine_round_close units 4 /
  reminder units 37 = **154 passed / 0 failed / 3 subtests，零误伤**。
- **host 判停（K0S-E4 同族口径）**：`git diff --quiet c1de02fe..HEAD -- firmware/` →
  exit 0（零差异）——builder 轮2 后 host 全量 **287 Tests / 3 Failures**
  （rebaseline-complete 事件 14:16 实测，工作树含两件未提交重锚、2 新齿全绿）以来
  固件零改动（轮3 eadb371f 仅 front_app.py），host 证据续用不重跑；3 红全 p1_auxbar
  AB-A1/A2/A4 存量族（K0S-E4 判停引用，基线锚 265/3 @ 2f2c5871 非本包）。
- Mutation: perAcceptance **11/11 turnedRed**（builder 两轮 11 处实放 2026-11-05，
  sha256 字节级还原前=后全 True，终态 MUTATION_CLEAN 零残留）；敏感变异覆盖
  四态判据 / 键位手势 / 删除静默取消 / 分页 limit / selected_id 注入 / 键帽逐字 /
  mcp 接线 / 删除消费幂等 / 语境有界注入 / 闭集路由 / 单写入口。真机语音与删除证据
  （K2PTS-L1/L2）不被 host 变异证据替代（诚实挂起，见未覆盖面申报）。
- Security: securityReviewRequired=false，闸绿。结论一句：**无扩大写权**——设备侧
  零本地账本写入（K2D-A4 零写盘断言 + D-10 单写入口锚：front 全模块 gw_points.add_points(
  恰 1 处、_consume_kidpal_control 源内零 add/redeem 调用）；删除只走按键确认 +
  kidpal_points_delete 私有上行（非法 id 拒绝审计 WARNING）；零新增依赖。

## Demo 对账（K2D-E1..E5 逐条）

| Exit criteria | 判定 | 在手证据 |
|---|---|---|
| K2D-E1 冻结验收覆盖四态/最近三条/越目标/负余额/单行与全量分页/键位/删除确认/冷启动恢复；红基线、变异证据、修订留痕齐全 | **绿** | host 冻结件 K2PTS-P1..P7 7/7 绿（redBaseline host 7/7 RUN_RED 实跑在案）；mutation 11/11 turnedRed；修订留痕 = K0PTS-A5 重钉（ruling5）+ 两件 additive 重锚（ruling1），pod-events 全程在案 |
| K2D-E2 host 与 server 离线证据：restore/显式打开分流、revision 重置清缓存、selected_id 只读注入、删除幂等对账、不完整句零写入；story/schedule 路由零新增红 | **绿** | server 冻结件 K2D-A1..A4 4/4 绿 + 圈定合跑 154/0 零误伤（m4k1 36、record_dialogue_guide 17、K0PTS-A5 重钉件 6）；host 287/3 判停（3 红存量族） |
| K2D-E3 一轮真机覆盖语音加分/兑换、只提事件不记账、金额不匹配与余额不足拒答、短窗重复、断网可见失败、删除重算与负余额、冷启动恢复；看屏与三源日志互证 | **pending（live-manual，诚实挂起）** | K2PTS-L1/L2/L3 冻结件头 coverage: 承载；含 K1L-L1/L2 挂接（human 2026-11-05 意向）；live 门只认 device-round-evidence.md，未跑不伪造 |
| K2D-E4 六张 V9 积分帧 design review；长原因/大分值/越目标/负余额不破坏 400×300 1bpp 版面；NFR-PERF-4 只记录实测 | **pending（真机看屏对表）** | 六帧渲染代码已落（kidpal_render_node 积分正式渲染三分派 + points_page 四态/进度线/删除覆盖层）；设备渲染为设备专属 TU、离线无屏，真机看屏时与 docs/ui/V9 六帧对表（见 UI design-review 证据节） |
| K2D-E5 diff ⊆ changeManifest；K1 权威账本不被设备重算或持久化；demo 对账与未覆盖面申报；K3/K4 复用本包缝 | **绿** | change-manifest 闸绿（23 路径，ruling3 扩 5 + ruling5 扩 1 均在册）；K2D-A4 单写入口断言 + K2PTS-P4/P5 设备只读消费断言；restore/进度线缝在 kidpal_render_node 与 points_page 公开头文件在位 |

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| SHOULD | judgement-call | launch-card / 合并链 | deviceOutcome 申报的 hostUncompilablePaths 含 `firmware/main/protocols/protocol.cc`，该路径不在 verify-gates.deviceOutcomeGate.hostUncompilablePaths 登记集——pre-merge-escalation-gate 实跑（--probe-out）以 `device-outcome-host-path-unregistered` 判 BLOCK；protocol.cc 为上行帧构造面、host 可编（host build 287/3 实证其编译通过），launch-card 自身已注「Pod intake 按实际 diff 收窄」 | pre-merge-escalation-gate BLOCK 输出 + overlap-probe.json deviceOutcome.reasons（收口实跑）+ host_app build 287/3 编译通过（changed-code hunk：protocol.cc 属已提交 diff 而非设备专属 TU） | 交 planner：合并前将申报收窄为实际设备专属 TU（application.cc / mcp_server.cc / lcd_display.cc / kidpal_render_node.cc，即 touchedHostPaths 实集），不改登记集、不改实现；probe 制品将由 merge-driver 合并时刷新 |
| NIT | judgement-call | server/acceptance | K0PTS-A5 wire 半重钉：K0 冻结件编码的是「front 无条件切页 step=1」实现细节，与正式渲染一屏 6 行的观感冲突；acceptance-author 最小重钉（种子扩至 7 条 + 沿游标循环取页 ≤10 跳防不收敛，首页 next_cursor 强断言逐字保留，语义保持） | changed hunk test_points_e2e_spike.acceptance.py +66/−20 + planner ruling5（pod-events 44ee007c，断言体实读核验） | 已闭合——重钉后单跑 6 passed + 全量 415 passed 硬校验绿；supervisor 复核扩单裁定 |
| NIT | judgement-call | firmware/display | 主页目标名以文字行替代场景资产图标（builder 有意偏差申报）：四预置目标资产图标与刻度联动归 K4 D-09 承接，本包建立进度线消费缝 | builder 申报 + taskpackage decisionCoverage D-09 landsIn=package:261104-k4-reward-goal-config | K4 承接；六帧 design review 真机看屏时可裁（见 UI design-review 证据节） |
| NIT | judgement-call | firmware/display | story_page.c 语境映射落点自由度：契约不锁 kidpal_points_context 映射家，落 story_page.c +9 行（所有权函数委托），acceptance-author 申报在案、planner ruling3 扩单覆盖 | changed hunk story_page.c + planner ruling3（pod-events 530863a8） | no-trigger |

结构闸 PASS/FAIL 不复述为 finding（约定级约束）；gate 全绿仍执行了五轴语义 review
（下节）。无 BLOCKING、无 hard-violation。

## Five-Axis Review

- **类型/边界**：分页纪律三层钉死——reset=true 先清旧分页再装首页全字段替换、同版本
  续页累加、跨版本 reset=false 拒绝且零扰动（K2PTS-P4 变异敏感：limit 6→16 即红）；
  next_request limit=6 不继承 story 16 条窗口、无缓存 -1 不带键；selected_id 只读注入
  三态判据（命中=1/未命中=0/frame NULL=-1），设备不造 id（K2PTS-P5 变异敏感）；
  next_cursor 沿游标循环 ≤10 跳防不收敛（重钉件断言）。负余额数字如实、画法钳位两
  不混（frame.balance 保持 −40、progress pos/solid=0，K2PTS-P1）。
- **错误处理**：删除确认子态三不原则——其余手势不响应、idle_step 10 秒静默取消
  （9999/10000 双向边界断言）、resend_due 5000ms/上限 3（K2PTS-P3 变异敏感：
  KPTS_DELETE_IDLE_MS 10000→5000 即红）；隐式 ack 对账三分支（pending 命中不冲销/
  权威重推不含冲销/空账重推冲销），确认动作绝不本地删缓存；非法 id 拒绝审计 WARNING
  含 points_delete 零重推（K2D-A1）；语境未知值记 WARNING（含值）绝不回退成 story
  （K2D-A3 变异敏感）；注入失败不阻断拦截消费（K2D-A2）。
- **契约/单源**：键位决策表逐格对 §2.3 手势矩阵（主页 KEY 短=语音/双=账本/长=回首页、
  账本 BOOT 永不离页、主页 BOOT=NONE 尊重 K0a 四页骨架冻结域）；键帽逐字对 V9
  points-ledger.png / points-delete-confirm.png（K2PTS-P6 变异敏感，双红实证）；
  kidpal_points_delete / kidpal_points_page / kidpal_session_ctx 三私有帧逐字段对
  契约 §四（POINTS_CTX_SIGNAL_TYPE="kidpal_points_context" 冻结 wire）；语境闭集路由
  零 story_list/零 record 信号（M4K1-A2 must-stay-green 半）；单写入口锚 =
  gw_points.add_points( 恰 1 处源扫描（K2D-A4 变异敏感）；分页 limit 尊重设备请求
  （契约 §四 1–50 界内钳位入口原样，eadb371f 实现向契约对齐）。
- **并发/资源**：设备侧零本地持久化（RAM 缓存可恢复重建、不写 NVS/SD 账本或离线
  队列，K2D-A4 零写盘断言）；删除幂等对账以权威重推为准，不乐观本地删；跨任务无锁
  的 `kidpal_points_ui_selected_id` 理论撕裂窗低概率（builder 观察项，见未覆盖面申报）；
  front 分页消费 limit=limit 每往返满窗 6 行（K0 固定步进 1 的 6 次往返退役）。
- **安全**：见 Verification Evidence security 行——无扩大写权、零新增依赖、删除只走
  按键确认 + 私有上行（儿童误触防线 = 三手势取消 + 10 秒静默取消 + 上限 3 重发）；
  语音记账不完整句零写入（D-04）、设备语音不开放删除（删除只走按键确认路径）。

## Simplification Debt

`minimal:` 标记收成：本包 diff **移除 1 处存量 minimal: 标记**（points_page.c K0
「最小渲染」注释——随正式渲染三分派落地退役，upgrade 兑现而非新增债）、**零新增
minimal: 标记**（b96b978d..工作树 diff grep 实证）。

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| points_page.c 原 K0 minimal: 最小渲染（本包 diff 内移除） | K0 spike 单帧直绘升级为正式三分派渲染 | 已兑现（本包） | 无——已升级退役 |
| kidpal_render_node.cc 积分渲染挂全局 LVGL 定时器刷新路径（随 K0a 先例骨架） | 积分页刷新复用既有 render-node 节拍，未单开专用刷新通道 | 真机刷新观感（NFR-PERF-4 实测） | K2PTS-L2 真机轮看屏翻屏流畅度 |
| 无 upgrade trigger 的其余条目：无。 | | | |

## Residual Risk

- Known limitation: ① **K2PTS-L1/L2/L3 真机轮待跑**（live-manual 三段，human 在场
  轮；L2 含 K1L-L1/L2 挂接）——live 门只认 device-round-evidence.md，收口不伪造；
  **pre-merge-escalation-gate 收口实跑已按此 BLOCK**（device-round-evidence-missing，
  见 Findings SHOULD 行）。② **六 V9 帧 design review 待真机看屏**（K2D-E4）。
  ③ **全量回归 = 人工触发位**：final 跑圈定清单（server 154 / host 判停 287/3），
  server 全量 415 在 builder 轮3 旁证、final 未重跑（K1/K0a 收口惯例）。
  ④ **观察窗**：`kidpal_points_ui_selected_id` 跨任务无锁理论撕裂窗低概率真机观察
  （builder 观察项）。
- Follow-up recommended: K3 QQ 正式通道（复用 restore 缝）、K4 目标切换与场景资产
  刻度联动（复用进度线缝，含主页图标偏差裁决）、K5 story_cover（K0a 页位不回退）。
- Human decision needed: 无新增——本包无 `decisionCoverage` 中 `landsIn: dropped`
  且 `reason` 以 `risk-accepted` 开头的条目（八条 decision 全部 landsIn 到
  exitCriteria 或 K3/K4/K5 承接包），无 risk-accepted 台账项可列。

## UI design-review 证据（uiuxRequired=true，level 2；bootstrap 期纯文本）

- viewport coverage summary：pending——六 V9 积分帧（points-home / -empty / -nodata /
  -negative / points-ledger / points-delete-confirm）在 400×300 1bpp 版面的实际渲染
  需真机看屏；设备渲染为设备专属 TU，离线无屏。host 面 7/7 绿只覆盖数据面与键帽
  逐字，不替代版面目击。
- a11y summary：pending——键帽逐字（「问一问/下移」「删掉/算了」）host 断言已绿；
  对比度/字形覆盖度需看屏（见下条）。
- design-drift summary：builder 申报一处有意偏差——主页目标名以文字行替代场景资产
  图标（归 K4 D-09，设计评审可裁）；其余四态结构/进度线画法/删除确认覆盖层按 V9
  帧结构落，逐帧终判待真机。
- external artifact reference：无（未产截图/视频/HTML，repoVisualArtifactRequired=false）。
- residual risk：字形覆盖度观察项——`→`(U+2192) 与「M月D日」在 puhui 16px cbin 的
  字形覆盖度待看屏（builder 申报；缺字形时须回 acceptance-author 走最小增量）。
- UI finding severity：无 UI BLOCKING；上列观察项按 code-review-gate 既有轴在真机轮
  判读，不新增 gate。

## 未覆盖面申报（诚实挂起清单）

1. K2PTS-L1 真机语音记账目击（加分/兑换/拒答/短窗重复/断网失败提示）——live-manual，
   human 在场轮。
2. K2PTS-L2 真机分页翻屏与删除目击（满窗翻页/删除确认/负余额/重算）——live-manual；
   含 K1L-L1（语音链真机记账）/K2 观察缝活性真机目击挂接。
3. K2PTS-L3 真机冷启动恢复目击——live-manual。
4. 六 V9 帧 design review（K2D-E4）——真机看屏时与 docs/ui/V9 对表。
5. front 删除重推 home→ledger 两帧顺序端到端活性未跑（builder 观察项——离线验收
   逐帧断言在，两帧顺序的全链活性归真机轮）。
6. `kidpal_points_ui_selected_id` 跨任务无锁理论撕裂窗低概率真机观察（builder 观察项）。
7. 字形覆盖度：`→`(U+2192) 与「M月D日」puhui 16px cbin 覆盖待看屏。
8. server 全量 acceptance（415 条）final 未重跑——人工触发位，builder 轮3 旁证在案。

## manifest 扩单记录

- planner ruling3（pod-events 530863a8）：+5 渲染/键路由路径——story_page.c/.h、
  kidpal_render_node.cc/.h、lcd_display.cc（K2D-E3/E4 真机可达与六帧渲染的清单
  遗漏补正；story_page.h 实际零改动、board .cc 零改动）。
- planner ruling5（pod-events 44ee007c）：+1 K0PTS-A5 重钉件
  server/acceptance/test_points_e2e_spike.acceptance.py。
- 终态 changeManifest = 23 路径，change-manifest 闸 final 两轮绿。

## 集成执行回报

未触发 merge-chain 冲突消解（本包 Pod 分支无冲突 abort + blocked 事件，无消解执行）。

## 设备轮证据

deviceOutcome 触发面：launch-card 申报 userVisible=true（S-F6.1/S-F6.2/S-F6.3），
touchedHostPaths 实集 = application.cc / mcp_server.cc / lcd_display.cc /
kidpal_render_node.cc。**device-round-evidence.md 未创建**——K2PTS-L1/L2/L3 真机轮
待跑，缺项不伪造 PASS；收口时点 pre-merge-escalation-gate 实跑 BLOCK 如实留痕
（device-round-evidence-missing + 申报登记集收窄项，见 Findings）。真机轮完成后由
后续轮次按 dev-loop §8d 三形态要件补 device-round-evidence.md。

## 制品清单（closeout）

- verifier-evidence/rounds/r1.json（留痕：scope 注记格式缺陷首跑）+ r2.json（承重
  final，invocationId 3fb1729b-b9fa-47bd-9629-d71f696776f2，factOk:true）
- verify-runs.jsonl（全程 append，含 scope-file 元数据行）
- test-scope-final.txt（圈定清单，路径见 Verification Evidence 一行锚）
- overlap-probe.json（收口实跑快照：overlap no-overlap + deviceOutcome 实态；合并时
  merge-driver 将刷新。**仓库注记**：该文件被 .gitignore:46 全局忽略、不入库——K1
  先例同此（K1 包目录在件是合并链落盘的本地留痕），非异常，不改 .gitignore）
- docs-delta.md（零文档变更申报）
- 本 review-packet.md
- verdict.json / verification.json 机器产线件：本批未产（advisory-only，缺失不阻塞
  closeout——closeout 判断来自 validated final fact r2.json）
