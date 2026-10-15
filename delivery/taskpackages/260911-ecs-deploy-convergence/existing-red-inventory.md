---
# existing-red-inventory.md · 260911-ecs-deploy-convergence

> 先存红分类清单 + 修复意图登记（governance 通道：红证据 ESCALATE 合法停止的豁免件）。
> 目的：本包 final verifier 证据（r6）非全绿，红项一律为**跨包既存证据件缺口 / 明确 out-of-scope 齿**，无一由本包改动引入。逐条分类、登记修复意图，供 human 裁。
> 原则：不伪造、不静默、不喂哨兵；「留红如案」= 有 owner/有意图登记的显式红。

## 触达本包 final 闸的红

| # | 红项（测试/闸） | 断言语义 | 归属包 | 分类 | 修复意图登记 |
|---|---|---|---|---|---|
| R1 | `acceptance/test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（SMK-A4；同时绊 `unit`/`acceptance` 两闸） | `server/acceptance/_artifacts/smoke-e2e/screen.md` 缺失 → 无屏上文字证据 | 260829-smoke-e2e | 既存证据件缺口（[evidence] 族先行红） | owner=260829-smoke-e2e：需其重新采集一次真实屏上文字证据（screen.md）或裁「物理冒烟证据以当时记录为准、齿永久 skip」。本包不代采（不是本包冒烟面）。 |

## 未触达本包 final 闸、但同批打包过程中显式留红的齿（不影响本包 merge，修源在别处）

| # | 红项 | 归属/现象 | 修复意图登记 |
|---|---|---|---|
| R2 | `P2-A1` first-match 歧义红（host ELF） | 批次③渲染批 | owner=批次③渲染观感单包；已在 260829-hardware-validation 立项尾差登记。 |
| R3 | `WSA-A4` host 齿（provision.log / smoke-roundtrip.log / screen\*.jpg 三制品缺口） | 260903-device-ws-switch closeout 证据遗物散失；flash.log 归位（本包 #13）已完成 | owner=260903-device-ws-switch：真源 partial 在 `C:\tmp` 但往返证据 session-id 不配对（d6f4e629 vs 40024357），无法诚实拼装；留红、需原包重采集或裁「历史证据以 closeout 当刻记录为准」。 |
| R4 | `VRT-A2/E2` host 齿（voice-roundtrip 冒烟证据记录缺失） | 260829-voice-roundtrip | owner=260829-voice-roundtrip：重新做一轮真实往返冒烟并落证据，或裁历史证据随动。 |

## 结论

- 本包自身面（scoped 7 闸 + 部署窗口验收 ECS-A1~A7 可证面）全绿；final 红 R1 与打包过程留红 R2-R4 均为**先存跨包缺口**，无一由本包改动引入。
- 修复意图均已登记 owner，按「留红如案」处理；本包不越权代修、不伪造证据件。
- 提请 human 按红证据 ESCALATE 的 human ruling 通道裁：接受本包合并（先存红转交各自 owner），或裁定先修 R1/R3/R4 再合并。
---