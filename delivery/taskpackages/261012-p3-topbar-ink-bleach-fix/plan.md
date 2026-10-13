---
id: 261012-p3-topbar-ink-bleach-fix
title: P3 计数带真机漂白修复——top_bar_ 50% 白蒙层透明化（TECH-261012 承接）
sourceType: regression
rigor: lite
riskTier: medium
reviewed-by: architect (/triage-change proportionate review, 2026-10-12)
date: 2026-10-12
gateProfile: ["types", "test-quality", "evidence-integrity"]
sourceRefs:
  - TECH-261012-p3-star-count-invisible-on-device
  - S-F2.1-AC1
  - S-F2.1-AC5
---

# 根因（已证实非假设）

chrome 顶带 `top_bar_`（lcd_display.cc:944-959，`LV_OPA_50` 白底）经 `KidPalReparentChatToLayer(lv_layer_top())`（waveshare-s3-rlcd-4.2.cc:848）恒浮于所有 KidPal 页；1bpp flush 阈值 `*buffer < 0x7fff → 黑` × LVGL 9.3 `lv_color_16_16_mix` 混 50% 白：INK 0x333333 → 0x94B2 ≥ 阈值漂白、纯黑 0x000000 → 0x7BEF 侥幸存活。P3 计数 (316/344, 8) 落 bar 覆盖区 y[0..28] → 整段白。

证据：面板探针 `topright_ink=3000`（零黑像素）+ 一行定案试验烧板后 human 真机观测「可以看到星星 *2 了」。全记录 = `docs/crq/tech/TECH-261012-p3-star-count-invisible-on-device.md`（含第四轮追加段两处证据更正）。

# 修复 = 候选 A

custom_lcd_display.cc SetTheme 的 `lv_obj_set_style_bg_opa(top_bar_, LV_OPA_TRANSP, 0)`（现 :53 TEMP-DIAG）转正，注释改写为正式根因/解耦注释。

A 优于 B（KidPal 顶带内容全改纯黑）的理由：① 结构性解耦（单一 choke point 断根）vs B 依赖 0x7BEF < 0x7FFF 约 3% 余量、LVGL 改取整即翻；② B 是敞口义务——所有现在/将来落 y[0..28] 的 INK/TEXT 内容全要跟改、漏一处即潜伏同类缺陷（阅读态标题 + 两 AC5 入口正是三个现成例子）；③ A 已被真机定案试验验证、B 未验。B 排除。

# changeManifest 种子（仓根相对）

- firmware/main/boards/waveshare-s3-rlcd-4.2/custom_lcd_display.cc——修复行转正 + 删两处 TEMP-DIAG（flush_cb :62-66、RLCD_Display :382-401）
- firmware/main/display/story_page.c——删 :1140-1141 画齿日志 + AC5 空态分支颜色统一
- firmware/main/display/kidpal_render_node.cc——删 :925-926 诊断日志 + AC5 :909/:913 颜色统一
- firmware/main/display/story_page.h——KSY_COLOR_INK 宏上提单源
- firmware/main/test/story_page_v2.acceptance.c——结构齿 + 既有齿复核

# 连带面裁决

1. **TEMP-DIAG 四处清理——进包（必进）**：36 行未提交插入全删、SetTheme 行「注释转正 + 保留」；纪律 = 删前 `git diff` 逐行核对仅删诊断行（gateProfile evidence-integrity 对应此义务）。
2. **AC5 两入口颜色统一——进包，方向 = 统一到 KSY_COLOR_INK（kidpal_render_node.cc 侧改）**。理由：spec 钉「计数画法照常」（v1.63 空态注记）+「视觉细节归设计稿」，颜色是实现层决定、本门可裁；「照常」的常 = 非空 ×N 分支（story_page.c:1137-1139）用 INK，render_node 的 lv_color_black() 是偏离侧；颜色随「话术单源 = story_page.h」既有单源纪律（notes 边角③）上提。本板 1bpp 阈值下 0x333333 与 0x000000 同渲黑、零视觉差；host 既有齿为带内墨量计数型探针（非精确色值断言）、改色预期不破——builder 仍须逐齿复核确认。
3. **阅读态标题 story_page.c:1216——不单独改动**，修复 A 后自愈；随包真机轮顺带目测复验（写进真机轮清单）。

# 验收面

- host 新结构齿：top_bar_ bg_opa 常量存在性 / KidPal 顶带颜色常量不变式（host 无 chrome 像素探针不可达——host harness 无 top_bar_，TECH 条目已定此口径，像素探真只能落真机轮）。
- 真机轮：SL-A5 计数分项复验 + 空态 ×0 两入口 + 阅读态标题顺带 + P1 顶带与聊天态顶带无回归目测（跨页副作用确认）。
- 纯代码不触资产：单 app 分区烧录即可（无 spiffs_root 变更）。

# 分流裁决记录（architect，2026-10-12）

- 档位：**② 单包 Lite**。非 ①：改自有实现面运行时行为（顶带透明化跨页生效 + 删诊断 + AC5 颜色统一），铁律再小也过门。非 ③：单一来源紧耦合簇、单 owning subsystem（firmware 显示面）、单 Pod、无迁移删数、无悬挂契约决策、一轮可收——根因已真机钉死是关键。非 ② Full：无 security 轴 / 无真 DB / 无状态机 / 无迁移 / 非 core.* 跨系统（纯固件单面）、无超常规多闸。
- riskTier = **medium**（单 subsystem 运行时显示行为 + 真实验收项 + 有界爆炸半径；非 high 无 high 级信号，非 low 非 docs/tooling。爆炸半径注记：top_bar_ 透明化作用于全部页顶带（P1/P2/P3 + 聊天态 chrome），比单点宽，但定案试验真机证实无害——medium 成立不升）。rigor = **lite**（单向守卫方向是 Full 级信号 + lite → 升 Full；本包 medium 无 Full 级信号。Lite 省文牍不省闸；bug 类〔非 proposal 来源〕decisionCoverage 豁免）。
- 逃生门检查：**零命中**。契约面不触 docs/contracts/**（story_list 载荷与 counter 语义零变更）；spec 面——「AC5 两入口颜色统一」不构成 spec 语义变更（requirements.md WF-F2-01 明文「视觉细节归设计稿〔现行母版 v5〕」、空态注记钉「画法照常」不钉色值，统一是实现层常数归一）；修复 A 改变全页顶带视觉（含聊天态 chrome）但聊天 chrome 外观非 KidPal spec 面、经 human 真机定案试验观测，不升门；真机轮含聊天态顺带目测（残余风险登记）。agent 定义 / gate 零触。跨面：触面 = 1，不建 intake spine。
- 人类触点①：确认 ② 单包 Lite（2026-10-12，会话内 AskUserQuestion）。
