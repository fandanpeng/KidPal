---
workstream: 261028-font-typography-scaleup
slug: 261028-font-typography-scaleup
status: active
opened: 2026-10-28
closed:
summary: 字体体系升级——正文/列表 16px、阅读标题 20px 居中省略、课表 pitch 19 容量 4/4/3、临时标记下划线化；字体落 assets 分区 .bin + cbin 运行时装载 + 编译内 14px 兜底（P0 spike 阻塞三个页面包）
owner: main-thread
area: systems
changes: []
---

# task_plan · 261028-font-typography-scaleup（workstream 级分波交付计划）

> 本件 = workstream 拆包顺序链必备节（delivery/workstreams/README.md「拆包顺序链」纪律）。设计权威 = [PROPOSAL.md](PROPOSAL.md)（FROZEN v1.0，裁定 F-1..F-9）；里程碑登记 = `delivery/milestones/milestones.jsonl` M6-font-typography（2026-10-28 补登，state=planning）；上游 anchors = 需求 v1.69 frozen + 架构面 landed（delivery/review/261028-arch-review-font-typography-scaleup/，ADR-017 在位）。

```yaml
workstream: 261028-font-typography-scaleup
phases:
  - id: P0
    name: 字体资产与运行时装载 spike
    scope: gen_fonts.sh 扩产 16px 正文 + 20px 标题全量 CJK 1bpp .bin（产线同源参数）；assets 分区打包与烧写接线（build_default_assets.py + CMakeLists）；kidpal_font 运行时句柄 + 缺资产降级编译内 14px 不黑屏；host 同源加载（host 编入 cbin_font.c 读同一 .bin）；探针页真机取证（消费端观测：真机看屏 + 日志 + 缺 assets 降级）。ADR-017 字体精确尺寸回填归本包
    changeManifest: tools/asset-pipeline/fonts/gen_fonts.sh、firmware/main/assets/fonts_root/**、firmware/scripts/build_default_assets.py、firmware/main/CMakeLists.txt、firmware/main/assets.cc、firmware/main/display/lvgl_display/lvgl_font.*、新增 kidpal_font 运行时装载模块、font_size_test_page.cc（探针复用/退役）、host_app CMakeLists + host_infra_seam.c
    riskTier: high
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality]
  - id: P1
    name: 首页 P1 卡 16px
    scope: home_p1.c text_yoff 微调（line_h=22 不动、容量表零损失）+ kidpal_render_node.cc 字体句柄切换（kidpal_p1_font seam 强符号）+ p1_home_battery / p1_cjk_name 冻结验收重基线
    changeManifest: firmware/main/display/home_p1.c、firmware/main/display/kidpal_render_node.cc、firmware/main/display/kidpal_p1_font.h、firmware/main/test/p1_home_battery.acceptance.c、firmware/main/test/p1_cjk_name.acceptance.c
    riskTier: medium-high
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality]
  - id: P2
    name: 课表网格 16px + pitch 19 + 容量 4/4/3
    scope: KSG_ROW_PITCH 16→19、槽位带 74/74/77→78/78/69、按槽位可画上限 {4,4,3}（KSG_COURSE_CAP 84 不动）、临时课标记 ※ 退役改课名下划线（lv_txt_get_width 测宽 clamp）、可见字数断言 + schedule_grid 冻结头留痕修订 + schedule_grid_restore 镜像同步；真机轮看 pitch 19 紧凑观感
    changeManifest: firmware/main/display/schedule_page.c、firmware/main/display/schedule_page.h、firmware/main/test/schedule_grid.acceptance.c、firmware/main/test/schedule_grid_restore.acceptance.c
    riskTier: high
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality]
  - id: P3
    name: 故事列表/阅读排版 16px + 20px 标题
    scope: 列表 16px + 标题超宽省略（LV_LABEL_LONG_DOT，不压日期列）；阅读正文 16px + 每屏自然行 4→3（K1 seam 数值改，形状不动）+ 标题 20px 居中单行省略；story_page_v2 断言重基线（13.5px/1.72 过时注释清退）；mcp_server.cc 两个分页消费点回归；真机轮翻页手感终判
    changeManifest: firmware/main/display/story_page.c、firmware/main/display/story_page.h、firmware/main/mcp_server.cc（回归）、firmware/main/test/story_page_v2.acceptance.c
    riskTier: medium-high
    rigor: full
    sourceType: proposal
    gateProfile: [evidence-integrity, test-quality]
```

## 顺序与 blocking edges

- **P0 → P1/P2/P3**（字体装载通路是全部页面包的 launch 前置）。
- **P0 → P1 额外串行**（kidpal_render_node.cc 文件重叠，显式 blocking edge，不交两 Pod 并行）。
- P2 与 P3 无文件交集，可在 P1 后并行（merge order P1 → P2/P3 任意序）。
- launchPreconditions：P1/P2/P3 = ① 对应上游 phase 完成 ② anchors.req（v1.69）+ anchors.arch（§12 行）已冻结（现状满足）③ 触 UI 包 UI-ready = requirements §9 v1.69 行 + brief v7 对齐注记（均在位）。
- 真机轮配额：P0（探针页）、P2（pitch 观感）、P3（翻页手感）各一轮；P3 收口轮兼 workstream 收口标准（PROPOSAL §5）。
