# existing-red-inventory.md · 260915-m1-k2-keybar-three-slot

> 先存红分类清单（governance 通道：红证据 ESCALATE 合法停止的豁免件）。对偶 `delivery/changes/dev/260915-m1-n1b-p1-home-battery/existing-red-inventory.md` 格式。
> final 轮（2026-09-16）8 闸绿 1 红：unit 闸红，红项**全部为归档件收集 artifact**，无一由本包改动引入。本包脏面闭集（git status 实证）：firmware 5 文件（keybar.{c,h} 新增、lcd_display.cc、设备/host 两个 CMakeLists）+ 冻结验收件 + delivery 治理面记账；**server/ 零触碰**（`git diff e990eb2 HEAD -- server/` 仅含他批 sweep 归档移位，本包工作树无任何 server/ 改动）。
> 判读权威 = taskpackage verify-runs.jsonl verifier-round 记录 + 本清单。

## final unit 闸红（R1）

| # | 红项 | 断言语义 | 归属 | 分类 | 修复意图登记 |
|---|---|---|---|---|---|
| R1 | unit 闸（final 相位）对 `server/acceptance/archive/` 内归档件报红——可见失败块 6 个全在归档件：test_char_stroke（2，v1 prompt / display_schema.py 缺失）+ test_g_relay_fix（4，front_app.py / engine-patch connection.py / deploy wiring / 状态机模块缺失） | 归档件引用的 MVP/验证期 server 面产物（front_app.py、engine-patch、display_schema.py、v1 版本化 prompt）在现役 server 面已退役或不在本 worktree | **非本包**——收集面 artifact：ARQ-260916-server-mvp-teeth-sweep 闭账（77b6b0c）的归档口径注记依赖「治理闸收集 glob（acceptance-run-check.mjs PY_ACC_RE）只命中 acceptance/ 直下」，但 verify.mjs final 相位 unit 闸的 pytest 递归收集仍扫入 archive/ 子目录（VRT-A2 先例同根：物理移目录不脱离 pytest 递归收集） | 归档件对已退役行为断言的红 = 量测口径 artifact，非现役面红 | owner = 治理批（建议登记 ARQ：unit 闸 / 基线量测命令对 archive/ 的排除口径缺口）。launch 面 progress.md 2026-09-16 锚行已预警同款（裸 pytest 递归收集归档件 121F artifact）；server 现役面 = 空（0 collected，sweep 后 acceptance/ 直下零齿），本包面域判据不受影响 |

## 本包自身面（无红）

- host runner：**18 Tests / 0 Failures / 0 Ignored**（13 基线 + 5 新齿全绿，相对冻结基线 13/0 零新增红；KB-A6 达标）。
- 设备固件编译：firmware-build 闸真编译绿（fixer 一轮修复 keybar.c 设备侧源注册后，xiaozhi.elf 链接通过）。
- mutation 电池：6 变异 / 5 acceptanceId 全 sensitive（详见 taskpackage mutationEvidence.perAcceptance）；变异全部还原，终态 18/0。
- KB-A5（真机确认轮）= structural，human 持板，证据随真机轮落本目录。

## 结论

- final 红仅 R1 一项，为**launch 预报过的收集面 artifact**（launch 卡 + progress.md 锚行），非本包引入；不修、不代修、不伪造。
- 合并门剩余红证据 ESCALATE 豁免件即本清单；治理口径缺口（unit 闸归档收集）交独立治理批，不塞给本包。
