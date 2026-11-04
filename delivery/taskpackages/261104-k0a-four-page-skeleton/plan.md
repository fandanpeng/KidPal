---
changeId: 261104-k0a-four-page-skeleton
title: 四页骨架——P3 积分占位、P4 故事原地迁移、枚举驱动语境与阅读键修复
rigor: full
riskTier: high
sourceType: proposal
origin:
  ref: delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md
  workstream: 261103-star-points-story-cover
reviewed-by: architect（/triage-change 分流 review 2026-11-03；门 1 待主线程另派）
date: 2026-11-04
gateProfile: ["evidence-integrity", "test-quality", "state-machine", "mcp-tool-contract"]
dependsOn: []
mergeOrder: 70
---

# 背景摘录（新建型锚「现状与差距」，PROPOSAL §2）

- 现役固件是三页实现：`KP_PAGE_HOME/SCHEDULE/STORY` 与 `PAGE_P1/P2/P3` 同值，`KP_PAGE_COUNT=3`，keybar 页点和 BOOT 循环都从这个基数派生。积分页尚不存在，故事列表仍直接占 P3 顶层。
- `story_page.c` 的 `kidpal_story_ctx_source_page` 用数值 `case 2` 表示故事。插入 P3 后故事会移到下一个枚举值，若不改映射，积分页按 KEY 会被误报为故事记录语境。
- 本包只建立四页骨架：P3 是数据未达静态占位，P4 是故事页原地迁移，不做封皮和积分业务。TECH-261028 的阅读 KEY 短按误开会话与键帽缺位在本包先修，避免缺陷进入后续层级改造。

# 可行性判据

- `key_gesture_parser`、`keybar` 和故事页纯逻辑均可进入 host 验收；`kidpal_render_node_cycle_next` 的真实调用链与 `application.cc` 的会话发送单点已由图索引和源码实读交叉核实。
- requirements v1.71、架构枚举驱动口径、ADR-018 与 V9 页图均已冻结，四页顺序和阅读键行为没有待裁方向。四页循环、页点、逐页语境可在 host 与真机分别取证。
- K0 可与本包并行开发服务端和新增 points 文件。本包必须留下 P3 接管缝并先落 `firmware/main/test/host_app/main/CMakeLists.txt`；该文件是两包唯一共享构建清单，K0 在本包 merged、重取基线后追加自己的登记。K0 的 host build、device build 与真机终验均在本包合并后进行。

# 跨包事实与共享冻结面约束（含跨切片边角）

- **mergeOrder=70，必须先于 K0 合并。** K0/K0a 是本列车唯一并行对；并行只覆盖 K0 的服务端与新增 points 文件。`firmware/main/test/host_app/main/CMakeLists.txt` 为唯一共享构建清单，K0 在本包 merged 后追加；K0 的 host build、device build、真机终验和 ready-to-merge 均使用本包合并后的基线。
- **K2/K5 共享映射的后续分工**：本包保证 home、schedule、points_home、story 四值；K2 增详细账本语境，K5 增封皮语境并保持故事列表的 `story` 记录语境。任何包都不得恢复数字页位判断。
- **相邻冻结锚拆包期预扫非零命中**：`retract_p4_three_page.acceptance.c` 与 `key_layer_dispatch.acceptance.c` 钉三页集，`keybar_three_slot.acceptance.c` 钉 P3 故事槽，`record_session_live.acceptance.c` 钉数值 source_page，`story_page_v2.acceptance.c` 覆盖故事行为。它们是本批已批准的重锚面，不是可绕开的红；planner 在 changeManifest 定稿后仍须复扫。
- **阅读键修复不得改全局覆盖层表。** TECH-261028 的根因是阅读态归属与键帽裁决漏接；查字、课表答案页等其他覆盖层的 KEY 短按会话入口必须保留。
- TECH-261022-p3-record-session-state-churn 有独立承接包。本包与它共享 keybar/story_page，合并前须复跑双方冻结件，但不承接其键帽鬼影、引导节奏或素材过滤范围。

# 铁律约束

- P3 占位只呈现 S-F6.1 AC4 的数据未达口径：不显示伪造余额、金币或进度线，不提前实现积分业务，也不把静态占位当成 K0/K2 的完成证据。
- 本包故事仍是 P4 平级页；不得提前宣称封皮、故事列表下沉或逐层返回完成。故事 S-F2.1/S-F2.2/S-F2.3 既有行为零回归。
- source_page 必须枚举驱动，禁止新增数字 case 或用默认值把未知页静默归入 story。原有 home/schedule/story 行为保留，points_home 不得切记录语境。
- 既有冻结验收只能由 acceptance-author 做收紧型留痕 supersede；builder/fixer 不得删负断言、改全局表绕过阅读键缺陷或以注释代替证据。
- 不改 requirements、architecture、`docs/contracts/**` 或 debt 条目；发现必须改线格式、会话语义或共享冻结常量时立即停并升级。

# demo 证据形态（与 roadmap 一致）

- host runner：四页循环、四点页点、键帽、枚举驱动 source_page、阅读键归属和故事三 story 回归，含 RED→GREEN 与变异证据。
- device build + V9 相关帧 design review。
- 一轮真机：BOOT 逐页循环与回绕、每页页点、逐页 KEY 会话语境、阅读 KEY 短按零响应、长按回列表与键帽；看屏照片和串口日志互证。
