---
changeId: 261028-m6-k2b-p2-schedule-16px
title: 课表网格 16px——pitch 19、带高 78/78/69、容量 4/4/3 与课名下划线
rigor: full
riskTier: high
sourceType: proposal
origin:
  ref: delivery/workstreams/261028-font-typography-scaleup/PROPOSAL.md
  workstream: 261028-font-typography-scaleup
reviewed-by: architect（/triage-change 分流 review 2026-10-28；门 1 待 human 派 architect）
date: 2026-10-28
gateProfile: ["evidence-integrity", "test-quality", "device-round"]
dependsOn: ["261028-m6-k1-font-assets-runtime-loading"]
mergeOrder: 62
---

# 背景摘录（新建型锚「现状与差距」，PROPOSAL §2）

- 课表网格目前以 14px、`KSG_ROW_PITCH=16` 和旧槽位带渲染；临时课以 ※ 前缀占用课名宽度。F-3/F-9 要求 16px 下重排为 pitch 19、带高 78/78/69，并将标记改课名下 2px 画线，使无标记格可见字数回升到 2 字。
- requirements v1.69 已钉渲染容量上午 4/下午 4/晚上 3、超限静默截断不滚动不提示；F-8 同时明确 `KSG_COURSE_CAP=84` 不动，云端仍全量下发，截断只属于板端渲染层。
- V8 `canvas-p2` 与 brief v7 已冻结新网格、下划线和保留 ★ 的观感口径。

# 可行性判据

- P0 合并后提供 16px 统一句柄；课表渲染和 host 真帧探针已在 `schedule_page.c`/`schedule_grid.acceptance.c` 中存在，可在同一消费面重基线。
- 网格常量与冻结头可机械核对，容量/截断、下划线、星标和 restore 镜像均可 host 验证；pitch 紧凑观感另占一轮真机。
- UI-ready 判据满足：requirements v1.69 frozen、`docs/ui/V8/canvas-p2.png` 在盘、brief v7 对齐注记在位。

# 跨包事实与共享冻结面约束（含跨切片边角）

- **P0 先行**：本包只消费 P0 的 16px 句柄和统一 14px 降级，不在 `schedule_page` 内另写资产加载。P1 与本包文件面不相交，可在 P0 后并行；合并仍按 mergeOrder 串行。
- **F-9 跨页一致性**：本包负责课表课名，P1 负责首页事件名；`flags bit0` 继续表示临时课，★ bit1 不动，云端 `week_grid_payload` 与 B1 契约不改。
- **相邻冻结锚扫查（拆包期非零命中）**：`schedule_grid.acceptance.c` 文件头钉旧槽位带、※ 真帧与 14px 下限，`schedule_page.h` 同步钉旧版式；这些是 F-3/F-9 已批准的重锚面。`KSG_COURSE_CAP=84`、`KSG_GRID_BOTTOM=254`、restore wire/前台语义与越带负锚必须保留。
- **restore 镜像同步**：`schedule_grid_restore.acceptance.c` 只随共享版式/标记锚更新，不得借机改变 restore 的只填缓存、不接管前台语义。图 trace 对 `kidpal_schedule_grid_render` inbound=0 是解析缺口，已用 Grep 补录 mcp 消费与 restore 镜像。
- **真机收口归属**：本包自有真机轮负责 pitch/容量/截断；P3 终端轮再复证四页并集，且必须等本包 merged。

# 铁律约束

- 容量上限只改按槽位可画数 4/4/3；不得把 `KSG_COURSE_CAP=84` 缩成渲染上限，不得让云端截断全量课程。
- 不新增超限提示、不做格内滚动；不改变今天列高亮、网格底界或 restore 行为。
- ※ 前缀及其字形探测/程序化兜底退役，★ 星标保留；不得把「全设备 ※ 退役」误改成删除 wire flags。
- 冻结验收只能由 acceptance-author 收紧型留痕 supersede，文件头与 restore 镜像同步；撤改法、删负断言或注释充证据必红。
- 契约/spec/design 零改；需要容量上网线或改 flags 时立即停并走 architect-reviewer/`/arch-change`。

# demo 证据形态（与 roadmap 一致）

- host runner：16px/pitch 19/带高、4/4/3、静默截断、下划线与 ★、可见 2 字、restore 镜像的 RED→GREEN 与变异证据；既有套件零新增红。
- device build + V8 design review。
- 一轮真机：看 pitch 19 紧凑观感、三槽不叠行、晚上第 4 门静默截断、下划线/★ 与长课表现；证据供 P3 终端轮复证。
