---
# existing-red-inventory.md · 260911-key-route-a1

> 先存红分类清单 + 修复意图登记（governance 通道：红证据 ESCALATE 合法停止的豁免件）。
> 目的：本包 final verifier 证据（r3）非全绿，红项一律为**跨包既存证据件缺口 / 明确 out-of-scope 齿**，无一由本包改动引入。逐条分类、登记修复意图，供 human 裁。
> 原则：不伪造、不静默、不喂哨兵；「留红如案」= 有 owner/有意图登记的显式红。

## 触达本包 final 闸的红

| # | 红项（测试/闸） | 断言语义 | 归属包 | 分类 | 修复意图登记 |
|---|---|---|---|---|---|
| R1 | `acceptance/test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（SMK-A4；同时绊 `unit`/`acceptance` 两闸） | `server/acceptance/_artifacts/smoke-e2e/screen.md` 缺失 → 无屏上文字证据 | 260829-smoke-e2e | 既存证据件缺口（[evidence] 族先行红） | owner=260829-smoke-e2e：需其重新采集一次真实屏上文字证据（screen.md）或裁「物理冒烟证据以当时记录为准、齿永久 skip」。本包不代采（不是本包冒烟面）。与 ecs-deploy-convergence existing-red-inventory R1 同源同处置。 |

## 未触达本包 final 闸、但 host 面既存留红的齿（host 面 firmware 闸为结构闸不真跑 ELF，留红记录供 verifier/human 判读）

| # | 红项 | 归属/现象 | 修复意图登记 |
|---|---|---|---|
| R2 | `WSA-A6:830`（`device_ws_switch.acceptance.c:830`，ecs-teardown-evidence.md 落盘缺失）| 260903-device-ws-switch | owner=260903-device-ws-switch（ecs-deploy inventory R3 族）；其宿主齿记录的物理冒烟证据件缺口，非本包范围。 |
| R3 | `WSA-A4:663`（`device_ws_switch.acceptance.c:663`，flash.log 落盘缺失）| 260903-device-ws-switch | owner=260903-device-ws-switch（ecs-deploy inventory R3 族，三制品散失 session-id 不配对）；非本包范围。 |
| R4 | `P2-A1:571`（`schedule_p2.acceptance.c:571`，first-match 歧义/title bounce animation face 缺失）| 批次③渲染批 | owner=批次③渲染观感单包（ecs-deploy inventory R2）；非本包范围。 |
| R5 | `VRT-A2·E2:878`（`voice_roundtrip.acceptance.c:878`，voice-roundtrip 冒烟证据记录缺失）| 260829-voice-roundtrip | owner=260829-voice-roundtrip（ecs-deploy inventory R4，local-only 证据族）；非本包范围。 |

> 注：host 面终跑宽口径 = 109 Tests / 4 Failures（integrator 独立复核实测，2026-09-11，
> `wsl … ./build/host_infra_test.elf`，cwd=host_app）：本包 5 新齿（KR-A1/A2/A3/A4/A6）全绿 +
> session_logic 全齿绿；4 红 = 上表 R2/R3/R4/R5 四条在册既存红，相对有效基线 104/4（planner
> 裁定：launch 口径 104/3 漏列 VRT-A2·E2 一条）**零新增红**。详见 review-packet §Verification Evidence。

## 结论

- 本包自身面**全绿**：scoped 7 闸（taskpackage-required / firmware-build 真编译 / contract-lint / acceptance-coverage / mutation-check / docs-sync / security）r3 全 PASS；5 新齿（KR-A1~A4/A6）host 全绿；mutation 5 条敏感实测全 turned red；KR-A5 真机确认轮 PASS（human 持板，pod-events `c0ffe62a`）。
- final 红（R1，SMK-A4）与 host 面既存红（R2-R5）均为**先存跨包缺口**，无一由本包改动引入。
- 修复意图均已登记 owner（R1 沿 ecs-deploy inventory R1 既有登记；R2-R5 引用 ecs-deploy inventory R2/R3/R4 同源），按「留红如案」处理；本包不越权代修、不伪造证据件。
- 提请 human 按红证据 ESCALATE 的 human ruling 通道裁：接受本包合并（先存红转交各自 owner / 独立批消解），或裁定先修 R1/R2-R5 再合并。
---