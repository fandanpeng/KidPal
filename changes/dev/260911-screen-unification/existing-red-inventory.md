# existing-red-inventory · 260911-screen-unification（PKG-2 归一屏）

> 在册红分类盘点（integrator closeout，非 blocking 但必交）。口径：**本包 merge 前后
> 均 3 条 host 红 + 1 条 server 红在 main 基线即存在，新增红才是本包责任；这几条均
> 不是本包引入的红，本包零触碰其 owner 面，不代为修复。**

## host 全量 111 Tests / 3 Failures（本包 7 齿全绿）

| 在册红 | 位置 | owner | 消解责任 | 现状/说明 |
|---|---|---|---|---|
| WSA-A6:830 | `firmware/main/test/device_ws_switch.acceptance.c` | 260903-device-ws-switch | 该 owner 补烧录/归档证据 | 本地-only 证据齿（ecs-teardown-evidence.md 缺），从未入仓；本包 diff 零触碰 device_ws_switch 面 |
| WSA-A4:663 | `firmware/main/test/device_ws_switch.acceptance.c` | 260903-device-ws-switch | 该 owner 补烧录/归档证据 | 本地-only 证据齿（flash.log 缺）；同上 |
| P2-A1:571 | `firmware/main/test/schedule_p2.acceptance.c` | 260911-render-pages（批次③） | 批次③ 落锁（mergeOrder 21，本包后并） | KSP-A1 first-match 歧义——标题弹跳动画面（lv_anim）缺失；渲染观感面归批次③，本包零触（SU-E2） |

## server 全量 125 passed / 1 failed / 6 skipped / 45 deselected

| 在册红 | 位置 | owner | 消解责任 | 现状/说明 |
|---|---|---|---|---|
| SMK-A4 | `server/acceptance/test_smoke_e2e.acceptance.py:422` | 260829-smoke-e2e | 该 owner 补 smoke-e2e 证据件 | `server/acceptance/_artifacts/smoke-e2e/screen.md` 缺失（无 on-screen line evidence）；server 面，本包 diff 零触碰 server |

## 判读

- **新增红才是本包责任**：本包冻结齿在红基线（7 Tests 6 Failures）已全部切绿（现 7/7
  PASS），未引入任何 host/server 新增红；verifier final 轮 r5 的 unit/acceptance 两闸
  红 = SMK-A4（server 面，owner=260829-smoke-e2e），与 firmware 本包 diff 无关。
- 3 条 host 红（WSA-A6/A4 + P2-A1）在红基线摘要与 acceptance 件头「上游在册红口径」已
  成文：builder/verifier 不得将其标成本包红、不得修补。
- SMK-A4 在 assemble 合约下 `server` 面由 owner 包负责；本包 merge 边界不覆盖 server。