---
# existing-red-inventory.md · 260911-display-first-m1

> 先存红分类清单 + 修复意图登记（governance 通道：红证据 ESCALATE 合法停止的豁免件）。
> 目的：本包 final verifier 证据（r5）非全绿，红项一律为**跨包既存证据件缺口 / 明确 out-of-scope 齿**，无一由本包改动引入。逐条分类、登记修复意图，供 human 裁。
> 原则：不伪造、不静默、不喂哨兵；「留红如案」= 有 owner/有意图登记的显式红。
> 口径（本包）：**新增红才是本包责任**；本包 6 新齿 red 基线 6 failed → 全绿 20 passed/0 failed，零新增红；host 面本包 diff 零触碰（无 firmware 变更），引用 PKG-2 inventory（`260911-screen-unification/existing-red-inventory.md`）与 key-route-a1 inventory（`260911-key-route-a1/existing-red-inventory.md`）即可，不重跑 host 全量。

## 触达本包 final 闸的红

| # | 红项（测试/闸） | 断言语义 | 归属包 | 分类 | 修复意图登记 |
|---|---|---|---|---|---|
| R1 | `acceptance/test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（SMK-A4；同时绊 `unit`/`acceptance` 两闸） | `server/acceptance/_artifacts/smoke-e2e/screen.md` 缺失 → 无屏上文字证据 | 260829-smoke-e2e | 既存证据件缺口（[evidence] 族先行红） | owner=260829-smoke-e2e：需其重新采集一次真实屏上文字证据（screen.md）或裁「物理冒烟证据以当时记录为准、齿永久 skip」。本包不代采（不是本包冒烟面）。与 260911-screen-unification / 260911-key-route-a1 existing-red-inventory R1 同源同处置。 |

## 未触达本包 final 闸、但 host 面既存留红的齿（host 面 firmware 闸为结构闸不真跑 ELF，留红记录供 verifier/human 判读；本包零 firmware diff，引用 PKG-2 inventory）

| # | 红项 | 归属/现象 | 修复意图登记 |
|---|---|---|---|
| R2 | `WSA-A6:830`（`device_ws_switch.acceptance.c:830`，ecs-teardown-evidence.md 落盘缺失）| 260903-device-ws-switch | owner=260903-device-ws-switch（ecs-deploy inventory R3 族）；其宿主齿记录的物理冒烟证据件缺口，非本包范围。 |
| R3 | `WSA-A4:663`（`device_ws_switch.acceptance.c:663`，flash.log 落盘缺失）| 260903-device-ws-switch | owner=260903-device-ws-switch（ecs-deploy inventory R3 族，三制品散失 session-id 不配对）；非本包范围。 |
| R4 | `P2-A1:571`（`schedule_p2.acceptance.c:571`，first-match 歧义/title bounce animation face 缺失）| 批次③渲染批 | owner=批次③渲染观感单包（ecs-deploy inventory R2）；非本包范围。 |
| R5 | `VRT-A2·E2:878`（`voice_roundtrip.acceptance.c:878`，voice-roundtrip 冒烟证据记录缺失）| 260829-voice-roundtrip | owner=260829-voice-roundtrip（ecs-deploy inventory R4，local-only 证据族）；非本包范围。 |

> 注：host 面终跑宽口径 = 109 Tests / 4 Failures（见 260911-key-route-a1 inventory + review-packet，integrator 独立复核实测 2026-09-11）：4 红 = 上表 R2/R3/R4/R5 四条在册既存红。本包实现面零 firmware diff（`git diff --name-only -- firmware/` 空），host matrix 结论由构造不变承载，**本包不重跑 host 全量**（launch 明令 host 基线红走独立批消解）。

## 结论

- 本包自身面**全绿**：scoped 7 闸（taskpackage-required / firmware-build 真编译 / contract-lint / acceptance-coverage / mutation-check / docs-sync / security）r5 全 PASS；6 新齿（DF-A1 五齿 + DF-A2 一齿）red 基线 6 failed → 全绿 20 passed/0 failed/4 skipped；mutation 2 条敏感实测全 turned red；DF-E1 reviewer-first 契约复核 CONFIRM_NO_CONTRACT_CHANGE。
- final 红（R1，SMK-A4）与 host 面既存红（R2-R5）均为**先存跨包缺口**，无一由本包改动引入。
- 修复意图均已登记 owner（R1 沿 ecs-deploy inventory R1 既有登记；R2-R5 引用 ecs-deploy inventory R2/R3/R4 同源），按「留红如案」处理；本包不越权代修、不伪造证据件。
- 提请 human 按红证据 ESCALATE 的 human ruling 通道裁：接受本包合并（先存红转交各自 owner / 独立批消解），或裁定先修 R1/R2-R5 再合并。
---