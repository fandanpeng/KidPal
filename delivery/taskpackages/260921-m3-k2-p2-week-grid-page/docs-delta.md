# Docs Delta · 260921-m3-k2-p2-week-grid-page

> integrator 收拢制品（2026-09-21）。本件回答「本包文档面义务是否已全清」；TaskPackage `docsSyncRequired=false`，逐面核对与事实锚如下。

## 结论

- **TaskPackage 声明 `docsSyncRequired=false`（docsSyncReason：纯板端实现——契约形状消费方、diff 归 K1 契约门同 PR；spec 层需求行已冻结（S-F1.2 v1.19 口径），零 docs 面改动）：本包责任范围内成立，义务已全清。** 本包全生命周期对 `docs/**` 零写入（核对方法见下），docs-sync 闸全轮绿（r1–r3，`ok:true`，`docsSyncRequired:false` 回显一致）。
- **零 docs 面改动的理由**：本包是契约 §1.3②/§1.3⑥ 冻结形状的**板端消费侧**——形状权威在 K1（260921-m3-k1-cloud-schedule-producer）契约门通道已落地（`24bb1b8`，HUMAN_APPROVED，CDQ-260921-m3-k1-schedule-grid = LANDED），本包只消费不定义；spec 面 `docs/requirements.md` 的 S-F1.2 / S-F1.1 / NFR-READ-1 需求行在 v1.19 口径已冻结在前，本包是实现向 spec 收敛，需求面只读不修改。
- 实现细节由代码注释/docstring（schedule_page.h / home_upcoming.h / kidpal_render_node.h / CMakeLists 注记，随实现 commit）与 closeout 制品（本目录 review-packet.md）承载，不另立 docs 文档。

## 文档面变更清单

| 面 | 文件 | 本包动作 | 性质 |
|---|---|---|---|
| 契约面 | `docs/contracts/interface-contract.md` | **零触碰**（铁律 M3K2-E2；消费冻结形状 §1.3②/§1.3⑥） | 只读消费 |
| spec 面·需求 | `docs/requirements.md`（docRefs `#F1`；sourceRefs S-F1.2/S-F1.1/NFR-READ-1） | 零触碰（v1.19 口径冻结在前，实现即向其收敛） | 只读消费 |
| spec 面·架构 | `docs/architecture.md` | 零触碰 | 只读消费 |
| 代码注释/docstring | `firmware/main/display/schedule_page.h`、`home_upcoming.h`、`kidpal_render_node.h`、`firmware/main/CMakeLists.txt` | 新形状/新缝的冻结契约钉行与退役注记（旧列表形状 260829-schedule-p2 五函数退役、schedule_p2.acceptance.c 已归档不走 supersede） | doc（随实现） |
| workstream 过程账 | `delivery/workstreams/m3-schedule/progress.md` | K2 完成段 append（append-only 纪律） | 过程账 |
| closeout 制品 | `delivery/changes/dev/260921-m3-k2-p2-week-grid-page/`（本件 + review-packet.md + verdict.json + verification.json） | integrator 收拢制品落盘 | closeout |

## 消费契约冻结形状的事实锚

本包实现与齿所消费的契约条目均为 K1 契约门 2026-09-21 定稿、随 `24bb1b8` 落地的冻结形状：

- **§1.3② schedule view 周网格 data**：`{week:{courses:[{day,slot,name,flags}]}}`——day 0 基（周一=0）、slot 三格（上午/下午/晚上）、flags bit0=※ 临时安排 / bit1=星标呼应、备注不进载荷。板端消费锚：`schedule_page.h` 网格契约常量区（`KSG_FLAG_TEMP 0x1` / `KSG_FLAG_STAR 0x2` / `KSG_COURSE_CAP 84` 逐项注释指向契约门定稿）+ `kidpal_schedule_grid_parse` 自足式最小 JSON 扫描。
- **§1.3⑥ home 可选 `data.upcoming`**（additive）：`{upcoming:{day_key, items:[{time,name}], preview:{time,name}|null}}`，对齐 `home_p1.h` 冻结 seam。板端消费锚：`home_upcoming.h` `kidpal_home_upcoming_apply` docstring（形状逐字段 + preview.time 非 HH:MM 拒收的 K1 裁决口径逐字钉行）+ `mcp_server.cc` VRT_ROUTE_HOME 接线段注释。
- 两处消费面均有行为级冻结齿背书（`firmware/main/test/schedule_grid.acceptance.c` 11 齿，A1–A6 段），与契约条目一一对应。

## 逐面核对

| 面 | 核对证据 |
|---|---|
| 契约面零写入 | `git diff 24bb1b8..worktree -- docs/` 零命中（2026-09-21 integrator 实跑，`docs/contracts/**` 亦零命中）；contract-lint 闸绿（final r3，exit 0） |
| spec 面零写入 | 同上 `git diff` 零命中（docs/ 全目录）；docs-sync 闸绿（final r3，stdout `docsSyncRequired:false` 与 taskpackage `docsSyncReason` 同口径） |
| agent 定义 / gate | `git diff -- .claude/` 零命中（全生命周期） |
| K1 契约批归属 | 本包基线已含 `24bb1b8`（K1 契约落地批在分支基线，progress.md 2026-09-21 节在案）——本包 diff 内零契约 hunks，归属清晰不混提 |
| 实现细节承载 | 代码注释/docstring 随实现 commit 在案（见上清单）；无需 docs 正文承载的内容 = 渲染坐标常量（冻结验收文件头区钉，非 spec 面） |
