# docs-delta — TaskPackage 260829-host-test-infra（subsystem: firmware，testHost: 基建自身）

> 读者是人（supervisor / human / 后续 wrap-up）。本包 docsOwner = integrator，本文记录
> 「docs-sync 已完成/已确认无漂移」部分，以及「有漂移但 integrator 不直改、路由 planner /
> 主线程」的建议。
> 边界注：integrator scope-map 写允许面含 `firmware/**`（本包唯一的文档产物
> `HOST_TEST_CONVENTIONS.md` 落在 `firmware/main/test/`，属 firmware 面，可由 integrator 直改）。
> `docs/ARCH-REVIEW-QUEUE.md` 在 integrator 写允许面内，本包无新增架构裁决、无 ARQ 需登记，
> 故不改动该文件。`docs/contracts/**` 零触（冻结，本包零契约面触点）。

## 1. 变更摘要

搭建 ESP-IDF linux target 的 host 单测基建（先行包，mergeOrder 2）：让后续所有纯逻辑包
（2/3/4/5/6/8/U）能脱离硬件在 PC 上跑单测。落地三件事：

- **独立 host 单测 ESP-IDF 工程** `firmware/main/test/host_app/`（`COMPONENTS main` 隔离设备组件树，
  避免设备芯片驱动在 linux target 解析失败）：`host_app_main.c`（linux 入口，自动跑全部 TEST_CASE，
  退出码=失败数）+ `host_infra_seam.c`（LVGL 纯软离屏渲染缝，headless，无窗口）+ `seam_bounds.test.c`
  （补充单测）+ CMakeLists×2 + `sdkconfig.defaults`（`CONFIG_IDF_TARGET_LINUX` + unity + LVGL 32bit 色深
  + CLIB 分配器）+ 运行入口 `espidf_wsl.sh` / `espidf.ps1`。
- **冻结验收文件** `firmware/main/test/host_infra_smoke.acceptance.c`（INF-A1..A4 + boundary，非空洞，
  带真实断言：位反转样例逻辑 + LVGL 到虚拟屏 6 像素 === 0x336699 + 未渲染哨兵）。
- **目录约定文档** `firmware/main/test/HOST_TEST_CONVENTIONS.md`（INF-A4 的 [C4.1]-[C4.5] + INF-E2
  host-vs-device 兼容矩阵，同一文档，后续包照此挂测试）。

## 2. 已改（本包 docs-sync，integrator 直接执行）

| 目标 | 位置 | 改动 | 理由 |
|---|---|---|---|
| `firmware/main/test/HOST_TEST_CONVENTIONS.md` §[C4.4] 期望输出 | C4.4 运行命令「期望输出（绿色）」节 | "3 Tests 0 Failures" → "4 Tests 0 Failures"，并把期望输出补成真实 4 条 TEST_CASE 明细 + 说明「第 1 条 seam_bounds.test.c 是补充单测、不算冻结验收；绿判据 = OK + EXIT=0 + 全部 PASS 且至少含冻结验收三样例断言，不要按固定 4 个数硬校」 | 独立复验实测 runner 报告 4 Tests（验收文件 3 个 TEST_CASE + `seam_bounds.test.c` 补充 1 个，均在 `HOST_ACCEPTANCE_SRCS` 挂载）。原文档写 3 Tests 与真实运行不符，会让后续包误以为只有 3 条、按错误期望硬校验。此改随本次提交一起落盘。 |
| `changes/dev/260829-host-test-infra/*` | 新增 | `docs-delta.md` + `review-packet.md` | 本包 closeout/audit blocking 制品（Step 11 / FS-EVIDENCE-3）。 |

`HOST_TEST_CONVENTIONS.md` 其余章节（目录总览 [C4.1] / 放置 [C4.2] / 命名 [C4.3] / 运行命令 [C4.4] /
兼容矩阵 [C4.5]+INF-E2）经 integrator 逐节核对：与冻结验收文件注释、builder 交付实现、以及本次 WSL
独立实跑完全一致，无需再改——这份文档就是 INF-A4/INF-E2 的 SSOT。

## 3. 建议 planner / 主线程（架构/机械闸级，integrator 出范围不直改）

> 这不是 docs 漂移，是**机械闸判据与本包新确立的 host runner 约定未对齐**，下游影响所有
> 后续 host 包。详见 `review-packet.md` 的 BLOCKING finding #1。

`unit-run-check.mjs`（G3 承重闸 `unit`）的 `hasFwUnitRunner()` 与 `acceptance-run-check.mjs`
（G3 承重闸 `acceptance`）的 `hasFwAcceptanceRunner()`，其「runner 证据」启发式只认旧约定：
- `unit` 面认 `firmware/host_test/` 目录或 `firmware/sdkconfig.ci.linux`；
- `acceptance` 面认 `firmware/acceptance/` 目录或 `run_acceptance.sh/.py`。

两者都不识别本包实际落地的 host runner `firmware/main/test/host_app/`（`sdkconfig.defaults` 声明
`CONFIG_IDF_TARGET_LINUX`，`espidf_wsl.sh` 为运行入口，ELF 实跑 4 Tests 全绿）。因此 final 全量验证
`unit` / `acceptance` 两门报「真空绿」红——有测试无 runner 证据。**这不是测试假红**，本 runner 真实
存在且跑绿；是闸判据滞后于基建包确立的新约定。

**建议动作（需 planner/human 改 `.claude/scripts/**`，integrator 出范围）**：
在 `hasFwUnitRunner()` / `hasFwAcceptanceRunner()` 中补一条识别 `firmware/main/test/host_app/
sdkconfig.defaults` 含 `CONFIG_IDF_TARGET_LINUX=y` 即视为有 host runner 的判据（前者已由本包
拟好补丁，见 review-packet finding #1 精确 diff）。不改则后续包 2/3/4/5/6/8 照 INF-A4 挂测试每个都会
在这个真空绿闸上被误伤，基建价值不成立。此改动是 architect 门级（对照 afd31ac 先例：server 面
host/live 区分同款闸缺口由 human 授权直改 + 记 ARQ 收口项）。

## 4. 无漂移确认

- `docs/contracts/interface-contract.md` 本包零写（零契约面触点，只读消费，契约门未触发）。
- `docs/spec.md`、`.planning/kidpal-mvp/PROPOSAL.md`（docRefs）无漂移——PROPOSAL §5/§7 的 host
  基建策略（走 A 先搭 host 基建、linux-target 兼容性不预判、OVM 决策）与本包落地一致。
- 无新增 `docs/` 根文件、无影子目录；文档产物落 `firmware/main/test/HOST_TEST_CONVENTIONS.md`
  （firmware 面）+ `changes/dev/260829-host-test-infra/`，不散写。
- `docs/ARCH-REVIEW-QUEUE.md` 本包无新 ARQ 需登记（基建包自身不引入架构裁决，唯一机械闸缺口走
  §3 升级，不先入 ARQ 队列）。