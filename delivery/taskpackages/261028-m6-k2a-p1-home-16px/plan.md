---
changeId: 261028-m6-k2a-p1-home-16px
title: 首页 P1 日程卡 16px——容量不变、运行时字体消费与临时事件下划线
rigor: full
riskTier: high
sourceType: proposal
origin:
  ref: delivery/workstreams/261028-font-typography-scaleup/PROPOSAL.md
  workstream: 261028-font-typography-scaleup
reviewed-by: architect（/triage-change 分流 review 2026-10-28；门 1 待 human 派 architect）
date: 2026-10-28
gateProfile: ["evidence-integrity", "test-quality"]
dependsOn: ["261028-m6-k1-font-assets-runtime-loading"]
mergeOrder: 61
---

# 背景摘录（新建型锚「现状与差距」，PROPOSAL §2）

- 首页「接下来/明天」日程文字目前经 `kidpal_p1_font_glyph` 使用 14px 字形；`line_h=22` 与三态空态、preview 多项容量已经在既有冻结验收中钉住。F-2 要求字号升 16px 而行距和容量不损失，只重基线垂直对齐。
- F-9 经 spine q5 扩为全设备临时标记统一：首页 WF-F1-01 的事件名也要由 ※ 前缀改下划线，与 P2 课表保持同一符号语言。该变化只涉及本地呈现，现有 home wire/seam 不变。
- V8 `canvas-p1` 已给出 16px/22px 与事件名下 2px 画线的冻结布局，requirements §9 v1.69 与 brief v7 对齐注记均在位。

# 可行性判据

- P0 合并后会提供 16px 统一运行时句柄和 host 同源字形，首页无需新建字体加载机制。
- `home_p1.c` 的行流、clip、三态与容量验收可在 host 重基线；设备/host 两个 `kidpal_p1_font_glyph` 消费面已由图索引确认。真机终判集中在 P3，避免多包争用同一设备。
- UI-ready 判据满足：requirements v1.69 frozen、`docs/ui/V8/canvas-p1.png` 在盘、brief v7 对齐注记在位。

# 跨包事实与共享冻结面约束（含跨切片边角）

- **P0→P1 blocking edge**：P0 与本包共享 `kidpal_render_node.cc` 和 host 字体 seam，禁止并行；本包在 P0 合并后的句柄与降级语义上工作，不重写资产装载。
- **P1/P2 的 F-9 分工**：本包只负责首页临时事件，P2 只负责课表临时课；两侧均不改变 wire 输入或新增临时标记字段，★ 语义不扩到首页。
- **相邻冻结锚扫查（拆包期非零命中）**：`p1_cjk_name.acceptance.c` [S1] 钉 `font_puhui_14_1`、14px 文本和 host 强符号，`p1_home_battery.acceptance.c` 钉 14px 行墨及行带。处置 = acceptance-author 收紧型留痕重基线；`line_h=22`、preview 三态、分组与容量不是可撤旧锚。
- **真机收口归属**：本包不独立占真机轮；首页 16px、下划线和缺 assets 降级由 P3 在 P0/P1/P2 全合并后复证。P3 的额外依赖是有意调度边，避免提前终判。
- **文件边界**：P2 的 `schedule_page.*` 与 P3 的 `story_page.*` 不在本包；云端 `home_upcoming_payload` 只读核对，不改 producer。

# 铁律约束

- `line_h=22` 与既有行容量不变；不得借 16px 升级减少「接下来/明天」条目、改变空态或重排数据。
- 辅助条、键帽条、查字窗等 14px 域不动；不全局替换 `font_puhui_14_1`，缺 assets 时仍走 P0 的 14px 降级。
- 契约与 spec 零改；临时事件下划线不催生新 wire 字段。发现必须改 `data.upcoming` 才能成立时立即停并升级，不在 Pod 内自裁。
- 冻结验收修订只走 acceptance-author 收紧型留痕 supersede；注释不算证据，撤改法必红。
- V8 首页帧是布局权威；不重出其他页面，不改 spec/design 文件。

# demo 证据形态（与 roadmap 一致）

- host runner：16px 行流、容量不变、下划线/非临时事件负例、preview 三态与既有回归的 RED→GREEN 证据；变异证明旧 14px/旧 ※ 路径会红。
- device build 与 V8 design review 证据。
- 真机半 deferred 到 P3 终端轮：首页四页并集看屏 + 缺 assets 降级复证，不在本包以 host 结果冒充真机终判。
