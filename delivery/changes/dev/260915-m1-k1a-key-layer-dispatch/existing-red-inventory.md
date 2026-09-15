# existing-red-inventory.md · 260915-m1-k1a-key-layer-dispatch

> 先存红分类清单 + 修复意图登记（governance 通道：红证据 ESCALATE 合法停止的豁免件）。
> 目的：本包 verify final 相位与 merge-driver quick-check 非 全绿，红项一律为**launch 预报在册既存红**（launch prompt 红基线节逐条列名），无一由本包改动引入。逐条分类、登记修复意图，供 human 裁。
> 原则：不伪造、不静默、不喂哨兵；「留红如案」= 有 owner/有意图登记的显式红。
> 判读权威 = `delivery/changes/dev/260915-m1-k1a-key-layer-dispatch/red-baseline-freeze.md` + launch prompt 红基线节（2026-09-15）。

## 触达本包 final 闸的红（server 面）

| # | 红项（测试/闸） | 断言语义 | 归属包 | 分类 | 修复意图登记 |
|---|---|---|---|---|---|
| R1 | `acceptance/test_smoke_e2e.acceptance.py::test_a4_display_line_evidence`（SMK-A4；同时绊 unit/acceptance 两闸） | `server/acceptance/_artifacts/smoke-e2e/screen.md` 缺失 → 无屏上文字证据 | 260829-smoke-e2e | 既存证据件缺口（[evidence] 族先行红） | owner=260829-smoke-e2e；本包不代采（不是本包冒烟面）。与 260911-key-route-a1 inventory R1 同源同处置。 |
| R2 | `acceptance/test_schedule_reminder.acceptance.py::test_e4_contract_point3_evidenced_and_poll_form_retired`（SRE-E4） | spec 的送达语义记录被改动（「重试到确认」口径是包 7 的验收依据） | owner 归 intake 判读（launch prompt 明注：与本包板侧 KD-A4 非同一齿，勿混） | spec 面既存红（spec 送达语义记录 vs 现文漂移） | owner 归 intake 判读；非本包范围（本包板侧钉点批改走 acceptance-author REVISION LOG 已闭环，与 server 面此齿不同物）。 |

## 未触达本包 final 闸、但 host 面既存留红的齿

| # | 红项 | 归属/现象 | 修复意图登记 |
|---|---|---|---|
| R3 | `SU-E3:1010`（`screen_unification.acceptance.c:1010`，docs/spec.md 归档悬空 canary） | spec-split 遗留（260913 spec 拆分后 docs/spec.md → docs/archive/spec.md，齿路径悬空） | owner=spec-split 批次；非本包范围。与 KR-A6 同根。 |
| R4 | `KR-A6:636`（`key_route_a1.acceptance.c:636`，同根 docs/spec.md canary） | spec-split 遗留（同 R3 根因） | owner=spec-split 批次；非本包范围。 |
| R5 | `WSA-A6:830`（`device_ws_switch.acceptance.c:830`，ecs-teardown-evidence.md 缺失） | 260903-device-ws-switch | owner=260903-device-ws-switch（key-route-a1 inventory R2 同源）；非本包范围。 |
| R6 | `WSA-A4:663`（`device_ws_switch.acceptance.c:663`，flash.log 缺失） | 260903-device-ws-switch | owner=260903-device-ws-switch（key-route-a1 inventory R3 同源）；非本包范围。 |

> 注：host 面终跑 = 135 Tests / 4 Failures（2026-09-15 实测）：本包新齿（KD 包 5 CASE + 回归锁 1 CASE）全绿 +
> KD-A4 批改义务红 RMD-A5 已随实现落地转绿；4 红 = R3/R4/R5/R6 四条在册既存红，相对有效基线 129/5
> **零新增红**。server 面 = 2 failed / 130 passed / 6 skipped / 45 deselected 与 launch 基线逐字一致（R1/R2）。

## 结论

- 本包自身面**全绿**：scoped 7 闸 r1+fixer 后复跑全 PASS；新齿全绿；KD-A4 批改后套齿全绿；KD-A5 真机确认轮 **PASS**（human 持板 r3，pod-events `83ddeb08`）。
- final/quick-check 红（R1/R2）与 host 面既存红（R3-R6）均为**launch 预报先存跨包缺口**，无一由本包改动引入。
- 修复意图均已登记 owner（R1/R5/R6 沿既往 inventory 登记；R2 归 intake 判读；R3/R4 归 spec-split），按「留红如案」处理；本包不越权代修、不伪造证据件。
- human ruling 通道：human 已裁「批准合并」（pod-events `bdf8bcf0`，decision=approve-escalated-merge），本清单 = 该批准所引用的在册红豁免登记件。
