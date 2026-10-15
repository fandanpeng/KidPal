# Review Packet — 260829-home-p1

> integrator 收口 · 2026-09-07。合并门前的审查包。本包为**首页常显信息牌**（kidpal-mvp
> 包 5，subsystem=firmware，rigor=lite，riskTier=medium，mergeOrder 8，dependsOn=
> 260829-host-test-infra / 260829-voice-roundtrip 均已 merged）：P1 frozen contract
> 11 纯逻辑函数（schedule_cache inject/count/item/preview、day_rollover、countdown、
> blink_visible、doze_state、layout_resolve、set_state、render_frame）+ host+SDL
> 验收绿。RTC 走时 + SHTC3 温湿度直读为硬件强绑定子能力，显式殿后包 9b（P1-E2）。

## Change Size

- **Files changed**（working-tree diff，git status 自查）：
  - 固件实现（新增）：`firmware/main/display/home_p1.{c,h}`（507 + 74 行；C linkage、
    零芯片依赖，时间/温湿度/电量经 `hp1_state_t` / inject seam 注入）
  - 固件接线：`firmware/main/CMakeLists.txt`（`display/home_p1.c` 编入设备树 SOURCES）、
    `firmware/main/test/host_app/main/CMakeLists.txt`（`home_p1.acceptance.c` +
    `../../../display/home_p1.c` 编入 `HOST_ACCEPTANCE_SRCS`；`host_infra_smoke.acceptance.c`
    移列表末 = boundary 齿注册序保绿处置，见 Findings #1）、
    `firmware/main/test/host_app/main/host_infra_seam.c`（additive 扩展
    `host_infra_render_p1_frame()`，不改任何既有断言行为）
  - 固件验收（新增，冻结只读）：`firmware/main/test/home_p1.acceptance.c`（1362 行，
    11 TEST_CASE：P1-A1/E1 装配结构 + P1-E3/E2 guard + A2~A5 行为齿，weak-symbol
    frozen contract 照 VRT 先例）
  - 红基线（新增 + append）：`firmware/main/test/acceptance/red-baseline/seam_stub.c`
    （非冻结脚手架替身，裸 gcc 不编 LVGL 时供链）、`red-baseline-summary.md`
    HP1 段 append（上文 VRT 段零改动）
  - 忽略规则：`firmware/.gitignore`（新增 `main/test/host_app/build_hp1/` 与
    `main/test/host_app/sdkconfig`——host_app 独立工程增量构建目录 + 机器本地生成
    sdkconfig，由 `sdkconfig.defaults` 再生，不入库）
  - 治理/证据：`.claude/taskpackages/260829-home-p1/*`（taskpackage mutationEvidence
    补齐 + verify-runs.jsonl + verifier-evidence/rounds/r1.json + pod-events.jsonl）、
    `.claude/taskpackages/current/*`（投影换指本包）、`changes/dev/260829-home-p1/*`
    （本制品）、`.planning/kidpal-mvp/progress.md`（append 收口记）
- **Net diff**：tracked 修改 9 文件（不含 .codebase-memory）约 +300/−110，另有实现/
  验收/stub/证据等新文件未入 tracked diff。净新增为主（能力新增包，无台阶退役）。
- **Scope**：单子系统（firmware）。只加纯逻辑模块 + host 验收接线；不触碰包 2
  view_router 已 landed 面（guard 齿证明零回归）、不改 server 任何文件。
- **Split recommendation**：不拆。验收与实现 1:1（P1-A1~A5 / E1~E3 全落一份冻结验收
  文件），单包内聚。

## Verification Evidence

### 承重事实（verify.mjs round fact，唯一 blocking 机器件）

- **final r1（invocation `0f011647-9142-40bd-8f1a-8e903f380a6b`）= ok:true /
  observedGateExit 0**，phase=final、round=1，九闸全绿，2026-09-07。
- 承重事实文件：`.claude/taskpackages/260829-home-p1/verifier-evidence/rounds/r1.json`
  （authority=bearing）。
- checker：`verifier-evidence-check.mjs --change-id 260829-home-p1 --phase final
  --round 1 --invocation-id 0f011647-9142-40bd-8f1a-8e903f380a6b` → **PASS（factOk:true，
  observedGateExit 0，EXIT=0）**。
- **诚实留痕（证据补跑）**：verifier 轮（verify-runs.jsonl 前 3 条，13:07:33 /
  13:07:50 scoped 七闸 + 13:08:15 full 九闸）全绿，但 handoff 时未落 bearing fact
  文件（`current/verdict.json` 投影 `legacyNonBearing:true`）。integrator closeout 以
  `--evidence-mode=round --phase=final` 补跑 final r1 发布承重 fact（九闸全绿）；
  verify-runs.jsonl 共 5 条（verifier 3 条 + integrator 补跑 2 条），无结果矛盾。
- 投影·advisory：`.claude/taskpackages/current/verdict.json`（verify.mjs 程序 fs-write，
  补跑后 `legacyNonBearing:false`）。本包未手写 `changes/dev/verdict.json` /
  `verification.json`（R11）。

### 逐闸状态（final r1，诚实口径）

| 闸 | 态 | 要点 |
|---|---|---|
| taskpackage-required | PASS | changeId 一致 |
| firmware-build | PASS·**degraded** | idf.py 本机不可用，固件编译未在本机 re-verify。非假绿：builder/fixer 侧设备构建 Project build complete（espidf_wsl.sh），verifier 标准路径补验 build 成功（pod-events 13:23:39「espidf_wsl.sh build（默认 build/ 目录）成功」），降级放行 |
| contract-lint | PASS | `docs/contracts/interface-contract.md` 零触碰 |
| acceptance-coverage | PASS | P1-A1~A5 全识别（P1-A1 structural，A2..A5 functional），files = `firmware/main/test/home_p1.acceptance.c` |
| mutation-check | PASS | 4 条 functional acceptance 各有 mutationEvidence（turnedRed=true，sensitive=true） |
| docs-sync | PASS | docsOwner=integrator；判定见 docs-delta B 段（零变更判定） |
| security | PASS | securityReviewRequired=false；diff 与新文件密钥扫描零命中（见 Five-Axis #4） |
| unit | PASS·executor-skipped | live-only on-target 测试（@pytest.mark.live，手动触发） |
| acceptance | PASS·oracle-pass/executor-skipped | 结构面 oracle（固件 5 / 服务端 5 验收文件）；on-target 实证由 host ELF + 9b device 验收承载 |

### Host ELF 验收（闸上 runner 判据）

- 冻结验收经 builder 接线进 host_app linux-target ELF（`HOST_ACCEPTANCE_SRCS` +
  `home_p1.acceptance.c` + `../../../display/home_p1.c` 强符号覆盖 weak 哨兵 +
  `host_infra_seam.c` additive `host_infra_render_p1_frame()`）：**43 Tests 2 Failures**
  ——integrator closeout 时从仓根重跑 `build_hp1/host_infra_test.elf` 复现 43T2F；
  本包 11 个 HP1 TEST_CASE 全 PASS，2 红均为**上游 WSA 包 evidence 齿**（见 Scope 外
  事项），非本包引入。
- 红→绿演进：acceptance-author 红基线 **11 Tests 10 Failures，ELF 退出码 = 10**
  （`red-baseline-summary.md` HP1 段，BUILD_CLEAN、红全在 weak 哨兵/装配缺失的真实
  行为上；guard 齿绿）→ builder 落地 + 接线后 weak 被覆盖、行为真执行 → 绿。
- 运行口径注记：evidence 齿按 runner cwd 定位真仓（VRT 齿明示「run the ELF from the
  repo root」）——host ELF 须**从仓根跑**；本包 mutation 探针末次终验同口径。

### Mutation check

4 条 functional acceptance（P1-A2..A5）各有 mutationEvidence、turnedRed=true、
sensitive=true，还原 sha256 与基线一致（`home_p1.c` = `ba237cfe85268469`，integrator
closeout 复核一致）：

1. **P1-A2**：`blink_visible` 相位翻转（`<on_window` 改 `>`）→ blink 逻辑齿 + P1-A2
   render 齿 + P1-A4 render 齿（电池闪同款函数）三红（43T6F）；还原回绿。
2. **P1-A3**：`countdown` 最近项选择反转（取最远未来项）→ countdown 齿红（43T4F）。
3. **P1-A4**：`doze_state` 充电语义反转（充电返回打盹）→ doze 逻辑齿 + render 齿双红
   （43T5F）。
4. **P1-A5**：`layout_resolve` 优先级位标反转（时间槽误标 TH）→ layout 齿红（43T4F）。

粒度局限如实记录：首试常量位移 4400→4410 未转红（周期常量齿对 10ms 位移不敏感），
改行为级相位翻转才真红——探针粒度教训见 Findings #2。四探针均 host_app 独立工程
增量构建（`-B build_hp1`）走 espidf_wsl.sh。

## 架构 / 依赖卫生

- **纯逻辑 seam 注入设计**：`home_p1` 11 函数零芯片依赖——时间/温湿度/电量一律经
  `hp1_state_t` / `schedule_inject` seam 注入，host 与设备树同源同文件；真实硬件读取
  （RTC 走时 + SHTC3 直读）显式殿后包 9b（P1-E2，PROPOSAL §3 留根）。
- **调用面核查**：`kidpal_home_p1_*` 全仓引用仅三处——实现（`home_p1.c/h`）、冻结验收
  （`home_p1.acceptance.c`）、红基线 stub（`seam_stub.c`）；**零生产消费方**（producer
  接线归包 6，inject seam 在接线前只由测试注入——刻意留根，非遗漏）。worktree 支线
  detect_changes: **N/A (worktree)**，以 git diff --stat + 全仓 Grep 符号抽查替代；
  包 2 view_router 冻结契约（home→1 / schedule→4 / text→0 / NULL→-1）经 guard 齿
  强符号真执行零回归。
- **跨包接口形状（producer=包 6 的消费面 seam）**：本包是 P1-A3「接下来」分组呈现的
  **消费面**——设备侧缓存 producer（包 6 OpenClaw 侧）算好的分组结果
  `{time:"H:MM"/"HH:MM", name}` 逐项数组 + 可选次日预告项，**渲染 verbatim，不重算
  分组、不自造下发协议**；P1-E3 红线遵守 = 不实现任何下发链路/WS/mcp 协议代码，只
  消费包 2 display 路由底座，通道接线归包 6。Intent:nointent blockingDecision =
  **NOT-BLOCKED**（设备半只被动收推送，三刷新触发的产生端在 producer/网关侧）。
- **host_infra_seam additive**：新增 `host_infra_render_p1_frame()` 强符号覆盖
  home_p1 自持 weak 默认，整帧覆盖写 `s_fb` 与 `seam_flush_cb` 同口径；包 0 冻结断言
  只关住 `host_infra_render_frame` / `host_infra_read_pixel` 自身，语义零影响。
- **依赖**：零新增第三方依赖（纯 C，无新库、无新组件）。
- **回滚路径**：分支 `dev/260829-home-p1` 单点 commit（基 ec4851b = main），revert 即
  整体回退；合并走 `merge-driver --pod`（--no-ff 语义），合并前前置升级闸 +
  supervisor-verdict-gate 机械闸。

## Simplification Debt

| File:Line | Simplified | Ceiling | Upgrade trigger |
|---|---|---|---|
| —（无新 `minimal:` 标记） | — | — | no-trigger |

> 本包 diff 内无新增 `minimal:` 标记。复用既有面：weak-symbol frozen contract 模式照
> VRT 先例、渲染复用包 0 host_infra 缝（additive 一个函数）、布局解析为纯函数贪心序
> 不建新抽象层；缓存模型 = 定长数组 + 显式容量防御，无投机泛化。

## Findings

| Severity | Kind | Area | Finding | Basis | Required action |
|---|---|---|---|---|---|
| FYI | judgement-call | host 注册序 | `HOST_ACCEPTANCE_SRCS` 重排：`host_infra_smoke.acceptance.c` 移到列表末——unity 按 link 逆序注册 TEST_CASE，smoke 排末 = 最先注册最先运行，其 boundary 齿（「NOT_RENDERED sentinel 在任何帧 flush 前」断言）需 fresh process，先于本包 render 齿（会 flush 帧）运行才能保绿。本包加源引发的既有守卫重排，处置正确且头注写明缘由 | changed-code hunk `firmware/main/test/host_app/main/CMakeLists.txt`（列表头注 + smoke LAST 行）+ heuristic「boundary 齿依赖进程初态」 | 无代码动作；**后继包向 HOST_ACCEPTANCE_SRCS 增源时保持 smoke LAST**（boundary 齿注册序守卫），建议登记进 host_app CMakeLists 头注既有说明之外的后继包注意事项 |
| FYI | judgement-call | 齿粒度 | mutation 齿粒度教训：周期常量齿对 10ms 位移不敏感（4400→4410 不红，采样相位未越界），行为级相位翻转才真红 | taskpackage mutationEvidence[0].notes（诚实留痕） | 无代码动作；后继包设计周期/常量类行为齿时优先行为级探针（相位/边界翻转）而非常量位移 |
| FYI | judgement-call | 集成判读口径 | host runner 基线含 2 条上游 WSA evidence 红齿（WSA-A4 flash.log / WSA-A6 ecs-teardown-evidence.md，见 Scope 外事项）——上游证据文件从未入仓，main 本身即红 | 本次 closeout ELF 复跑输出（两条 FAIL 逐字摘录）+ device_ws_switch.acceptance.c 齿定义 | 无代码动作；**后继包在 host ELF 上做 mutation 复红判读时，失败计数须相对 43T2F 基线**（即「额外红」才算本包齿红），避免把基线红误读为回归 |

> **无 BLOCKING finding。** 结构闸证据不重复包装为 finding（code-review-gate 约定）；
> gate 全绿仍完整执行五轴语义 review（下）。三条 FYI 均不阻断本包。

## Five-Axis Review

1. **Correctness**：P1-A2~A5 语义逐条落地——超大时间 + 眨眼冒号 4.4s 相位（A2：可见窗
   3/4 周期，电池 1s 闪同款相位函数）、日程缓存 inject/count/item/preview + 次日预告 +
   跨 0:00 滚动（A3/E3：分组结果 verbatim、整体替换、容量防御、无待办清空）、低电
   打盹（A4：`<=` 阈值切 state-sleepy、充电叫醒覆盖、机制断言不测魔法数）、布局优先级
   （A5：时间>接下来>温湿度贪心序、冲突时温湿度**隐藏**语义）。host 43T2F 中 11 个
   HP1 TEST_CASE 全 PASS，与冻结判定逐条吻合；红基线 10 红逐齿对缺失行为映射留档。
2. **Test quality**：weak-symbol frozen contract 设计让红基线红在真实缺失行为（weak
   哨兵 -2 / 装配缺失），非编译链接错误（BUILD_CLEAN 佐证）；guard 齿三件套（包 2
   view_router 契约回归 + 契约 §1.3 canary + 模块纯净性扫描）防跨包回归；mutation
   四探针全转红且齿间联动真执行（blink 翻转带红 A4 render 齿）；红基线冻结前红因
   精度修正留痕（P1-A4 render 齿补哨兵前置检查）。
3. **Contract/boundary safety**：契约面零触碰（contract-lint PASS）；数值码/常量/类型
   形状冻结于验收文件头 + `home_p1.h` 双向一致（`HP1_ITEM_MAX 8` / 阈值 15 / 4400ms /
   1000ms / 3-4 可见窗）；P1-E3 红线在实现与断言双层落实（无下发链路代码、设备不重算
   分组）；P1-A1 装配齿关住「设备树 CMakeLists 编入」防孤儿模块。
4. **Operability/security**：纯逻辑模块零 IO、零网络、零文件访问——无敏感操作面；
   `securityReviewRequired=false`，全 diff 与新文件密钥扫描零命中；渲染整帧覆盖写与
   包 0 seam 同口径，不引入越界写（接受 `w==SEAM_FRAME_W && h==SEAM_FRAME_H` 精确
   形状校验，形状不符拒绝）。
5. **Maintainability/simplicity**：单文件 507 行 11 纯函数 + 冻结头自持文档；seam 注入
   模式与 view_router 同款，后继包（包 6 producer 接线、9b 硬件）扩展点清晰（inject
   seam / `hp1_state_t`）；渲染区域矩形是布局在位钉而非字体/颜色钉，审美自由度留给
   design review，不预实现。

## Residual Risk

- **Known limitation**：
  - **P1-E2 硬件殿后**：RTC 走时 + SHTC3 温湿度直读未实现（host 段只测布局/逻辑），
    真实硬件读取殿后包 9b device 验收；打盹阈值默认 15 已钉，**9b owns 最终调定**
    （机制断言只测相对阈值行为）。
  - **渲染面自由度**：区域矩形 = 布局在位钉，非字体/颜色钉；Baloo 数字、反白牌样式、
    温度小人/水滴宝宝等纯审美未测（留未来 UIUX 包 / design review）。
  - **producer 接线缺位**：inject seam 无生产调用方——包 6 接线前，日程缓存只由测试
    注入（P1-A3 消费面形状已冻结，接线风险低）。
  - **firmware-build 闸 degraded**：本机 idf.py 不可用未 re-verify 设备编译；builder/
    fixer 侧设备构建 complete + verifier 标准路径补验 + host ELF 同源编译佐证。
  - **host 基线 2 条上游 WSA 红齿**（scope 外，见下）。
- **Follow-up recommended**：
  - 包 6 producer 接线（inject seam 生产调用方 + 三刷新触发产生端）。
  - 包 9b：RTC/SHTC3 直读 device 验收 + 打盹阈值调定。
  - 上游 WSA 证据齿落盘（flash.log / ecs-teardown-evidence.md 归档）——归 WSA 包或
    主线程处置路径，由 planner/主线程安排。
- **Human decision needed**：无。本包无待人类拍板项。

## decisionCoverage 残险（landsIn=dropped / risk-accepted）

- 本包 `decisionCoverage` 三条（PROPOSAL-§2-包5→P1-A2、spec-§5.1-常显信息牌→P1-A5、
  PROPOSAL-§3-硬件殿后→P1-E2）均 `landsIn: acceptance:*` / `exitCriteria:*`，**无
  `dropped` / `risk-accepted` 条目** → 无此列表。

## Scope 外事项（如实载明，不计本包 findings）

- **上游 WSA evidence 齿红 2 条**：closeout ELF 复跑 43T2F 的 2 红逐字为——
  `WSA-A4 RED: flash.log 落盘缺失 — 烧录证据不存在`、`WSA-A6 RED: ecs-teardown-evidence.md
  落盘缺失 — ECS shim 停服/安全收尾未执行或未记录`。两证据文件从未入仓、main 本身
  即红（本包基 ec4851b 上同样红），非本包引入；判读口径见 Findings #3。
- **VRT evidence 齿 CWD 敏感**：evidence 齿按 runner cwd 定位真仓并明示报错
  「run the ELF from the repo root」——host ELF 从 `host_app/` 目录跑会红在仓定位齿；
  本包所有 ELF 判读（mutation 末次终验 + closeout 复跑）均从仓根执行，口径一致。
- **设备构建实证边界**：本机无 idf.py，设备树编译未在本机独立 re-verify；「Project
  build complete」为 builder/fixer 侧 espidf_wsl.sh 实证 + verifier 标准路径补验留痕
  （pod-events），非本 closeout 亲跑。

## 未测面与特殊放行声明

- **on-target 面**：设备真机渲染（LVGL 节点集成殿后——home_p1 自持 weak 渲染缝，真机
  集成归后继包）与硬件读取（9b）不在本包验收面。
- **firmware-build 降级放行**：非假绿，本机工具链缺失的 re-verification 降级（builder
  侧真编译 + verifier 标准路径补验 + host 同源编译三重佐证）。
- **本包如实载明**：final r1 九闸 ok:true、host 43T2F（11 HP1 全绿 + 2 上游红齿）、
  红基线 11T10F exit=10、mutation 4/4 变红还原 sha 一致、设备构建 complete（builder
  侧）。**不粉饰为「P1 首页端到端全通」——硬件读取与 producer 接线已如实留根（P1-E2 /
  包 6），本包交付物 = frozen contract 11 函数 + host+SDL 验收绿。**
