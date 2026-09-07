# docs-delta — 260829-home-p1（首页常显信息牌，kidpal-mvp 包 5）

> integrator 收口 · 2026-09-07。subsystem=firmware，rigor=lite，riskTier=medium，
> mergeOrder 8，dependsOn=260829-host-test-infra / 260829-voice-roundtrip（均已
> merged）。本包把 P1 frozen contract 11 纯逻辑函数 + host+SDL 验收做到绿。本文件
> 记录 docs-sync 的 **dist（哪些已改）** 与 **判定（哪些无需改、为什么）**。

## A. 已改（本包落地）

| 目标 | 位置 | 改动 | 理由 |
|---|---|---|---|
| P1 首页纯逻辑模块（新增） | `firmware/main/display/home_p1.{c,h}` | 11 个 FROZEN OFFLINE CONTRACT 纯逻辑函数强定义（schedule_cache inject/count/item/preview、day_rollover、countdown、blink_visible、doze_state、layout_resolve、set_state、render_frame）；C linkage、零芯片依赖，时间/温湿度/电量经 `hp1_state_t` / inject seam 注入 | P1-A2~A5、P1-E2（硬件殿后 9b）、P1-E3（只消费包 2 底座） |
| 设备树接线 | `firmware/main/CMakeLists.txt` | `display/home_p1.c` 编入 SOURCES（带来源注记注释） | P1-A1 装配齿（防孤儿模块） |
| host 测试接线 | `firmware/main/test/host_app/main/CMakeLists.txt` | `home_p1.acceptance.c` + `../../../display/home_p1.c` 编入 `HOST_ACCEPTANCE_SRCS`（强符号覆盖 weak 哨兵）；`host_infra_smoke.acceptance.c` 移列表末（unity 逆序注册 → smoke 最先运行，boundary 齿 fresh-process 保绿，头注写明） | P1-A1/E1（host 单测闸）+ 包 0 boundary 齿守卫 |
| host 渲染缝 additive 扩展 | `firmware/main/test/host_app/main/host_infra_seam.c` | 新增 `host_infra_render_p1_frame()`（强符号覆盖 home_p1 自持 weak 默认，整帧覆盖写 `s_fb`）；不改任何既有断言行为 | 渲染齿像素探针通路 |
| 冻结验收 C 侧（新增） | `firmware/main/test/home_p1.acceptance.c` | 11 TEST_CASE：P1-A1/E1 装配结构 + P1-E3/E2 guard（view_router 回归 + 契约 canary + 纯净性扫描）+ A2~A5 行为齿；weak-symbol frozen contract 照 VRT 先例 | P1-A1~A5、P1-E1~E3 |
| 红基线脚手架（新增，非冻结） | `firmware/test/acceptance/red-baseline/seam_stub.c` | host_infra 渲染缝最小替身（裸 gcc 脚手架不编 LVGL 时供链；渲染齿红于哨兵，替身不掩盖红） | 红基线纪律留痕 |
| 红基线判定摘要（append） | `firmware/main/test/acceptance/red-baseline/red-baseline-summary.md` | HP1 段 append（11T10F exit=10 实测 + 逐条判定表 + 诚实边界；上文 VRT 段零改动） | 红基线纪律留痕 |
| 忽略规则 | `firmware/.gitignore` | 新增 `main/test/host_app/build_hp1/`（独立工程增量构建目录）与 `main/test/host_app/sdkconfig`（机器本地生成件，由 `sdkconfig.defaults` 再生） | 构建产物不入库（`build/` 模式不覆盖 `build_hp1` 名，显式补） |
| mutation 证据 | `.claude/taskpackages/260829-home-p1/taskpackage.json` | `mutationEvidence` 四条补齐（turnedRed=true / sensitive=true / 还原 sha 一致） | mutation-check 闸 |

## B. docs-sync 判定（零变更判定）

- `docs/contracts/interface-contract.md` 本包**只读消费**（`contractRefs`），
  contract-lint PASS（零触碰）。本包是 P1 首页行为实现面，不新增/不改任何契约定义；
  设备与网关/producer 的接口仍以契约既有 E1 display 统一响应块承载，本包消费面
  **渲染 producer 分组结果 verbatim、不自造下发协议**（P1-E3 红线），无契约面暴露。
- `docs/spec.md#§4.1 / §5.1` **零语义变更**——理由：§4.1 S-F1.1（超大时间、冒号眨眼
  双点 4.4s 一眨、日期星期、温度/湿度图标、电量）与 §5.1（设备首先是常显信息牌、
  90% 常显、优先级 时间>接下来>温湿度）描述的行为已被本包实现满足，实现未引入
  spec 未记载的新行为、未偏离任何既有措辞（冲突让位 = 隐藏语义、打盹 = pose-sleep +
  电池每秒闪、跨 0:00 清缓存均已在 spec/PROPOSAL 冻结口径内）。本包为实现面落地，
  spec 无需改。
- `.planning/kidpal-mvp/PROPOSAL.md` 受 `upstreamFrozen sha1:88a5bdb83abdd867` pin，
  禁改防 UPSTREAM_DRIFT。决策映射由 `decisionCoverage.landsIn` 承载（三条全落
  `acceptance:*` / `exitCriteria:*`，无 dropped）。
- `docs/contracts/`、`docs/spec.md`、`docs/templates/**` 本包零写入。

## C. `.planning/kidpal-mvp/progress.md`（已 append）

- 新增「2026-09-07 包 5（260829-home-p1）dev-loop closeout（integrator 收口记）」
  一节，内容与 review-packet 同口径：final r1 九闸全绿（承重 fact 补跑留痕）、红基线
  11T10F→43T2F（11 HP1 全绿 + 2 上游 WSA 红齿）、mutation 四探针、诚实边界（E2 硬件
  殿后 9b / producer 接线归包 6）、留根与下游。历史段落零改动（append 不覆盖）。

## D. ADR 落点说明

- TaskPackage **无 `adrRequired`**；本包架构裁决全部已在上游冻结：PROPOSAL v0.4 §2/§3
  （包 5 范围 + 硬件殿后）、spec §4.1/§5.1（常显信息牌行为与优先级）、PLANNER INTAKE
  设计口径（冻结进验收文件头注：verbatim 消费、隐藏语义、阈值常量 9b owns、眨眼
  相位函数复用）。`decisionCoverage` 三条全落 `acceptance:*`/`exitCriteria:*`，本包
  无新增架构裁权。
- 故**不新增 ADR**。

## E. 里程碑 / delivery 面

- `deliveryImpact=milestone-affecting`，但**无里程碑阶段跳变**（无 accepted 转移）→
  不改 `.delivery/milestones.jsonl`、不跑 `delivery-status.mjs`。本包是 M2/M3 常显
  信息牌能力的实现面推进，已记入 progress.md 收口记。

## F. 无漂移确认

- 契约面零触碰、无影子目录（根级 `contracts/**` 零引用）；无新增 `docs/` 根文件。
- 证据与报告落 `changes/dev/260829-home-p1/` 与 `.claude/taskpackages/260829-home-p1/`，
  不散写。
- 构建产物面：`firmware/main/test/host_app/build_hp1/` 与 `firmware/main/test/host_app/sdkconfig`
  机器本地、不入库（已补 gitignore，by design 未 force-add）。
- 包 2 已 landed 面（`view_router.{c,h}`、`mcp_server.cc`）零改动；guard 齿证明其
  契约行为零回归。server 面零触碰。
