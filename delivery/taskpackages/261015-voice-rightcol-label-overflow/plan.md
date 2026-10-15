---
changeId: 261015-voice-rightcol-label-overflow
title: 语音轮右列文字溢出修复——echo/answer/标头三 label 宽 264 旧版式遗留值收窄至 196
rigor: lite
riskTier: medium
sourceType: regression
reviewed-by: architect (/triage-change 分流 review，human 触点①确认 ②单包 Lite + 标头收进)
date: 2026-10-15
---

# 根因假设（architect 复核确认，含 git 证据链）

语音页右列容器在 ui-v5-align 阶段3（commit 9f4c82a）收窄为 x=192、宽 196（版式数学 400-192-12，lcd_display.cc:1193-1197 注释链 + requirements §2.2 x192/y80 冻结锚），但盒内两个 label 的宽度 264 是旧版式遗留值（commit 9cb0c07，260915-m1-k3-voice-round-ui 真机确认轮所定，彼时右列 x=124 起、宽 264）——容器改了、label 没跟着改。

机制：`LV_LABEL_LONG_WRAP` 下 label 按 264px 换行，容器视口仅 196（pad_all=0），LVGL 默认裁剪溢出 → 每行右端约 68px（≈4~5 个 14px CJK 字形，随 ASCII/全角混排浮动）被裁不可见。human 真机照片实证：说话态（TALK，问答框 answer label）行尾 1~2 字/行尾星号被裁。

# 修复方案

三个 label 宽度 264 → **196**（与容器内容宽一致）：

1. [lcd_display.cc:1209](firmware/main/display/lcd_display.cc) `g_kidpal_voice_echo_label`（回显区，LISTEN/THINK 态）
2. [lcd_display.cc:1229](firmware/main/display/lcd_display.cc) `g_kidpal_voice_answer_label`（问答框，TALK/说话态——human 报的 bug 面）
3. [lcd_display.cc:1187](firmware/main/display/lcd_display.cc) `g_kidpal_voice_header_label`（标头）——**human 触点①裁定一并收进**（统一版式卫生）。architect 默认建议是不收（无裁剪缺陷：124+264=388 ≤ 400，短文案左对齐人眼无溢出）；human 改判收编，属触点①一句话改判授权。标头 x=124 位置**不动**（WF-B 标头位是独立版式决策，非本包范围）。

- 同文件先例：THINK 引导行 label（:1886）同父同版式已写 196。
- 滚动条让位（architect 已查证，结论：不必预留）：固件 LVGL = 9.3.0，全仓仅这两个盒用 LV_SCROLLBAR_MODE_AUTO、无 LV_PART_SCROLLBAR 自定义样式 → 默认主题滚动条 ≈5px、盖在右缘之上不缩内容区，仅溢出且滚动中瞬态部分遮盖，14px CJK 字形自带右缘余白，观感风险低。**builder+真机轮终判自由度**：若真机观感不佳可让位改 188~190（MCD-A1 先例：W 终判归 builder + 真机轮，不钉单一魔数）。
- THINK 让位路径（:2040-2048 只 set_pos/set_size echo_box，不碰 label 宽）零联动。

# 影响面

- 改动面：lcd_display.cc 三行（:1187、:1209、:1229），无其它同族子件（architect 全文件核查：`264` 命中仅此三处 + 无关的 home_p1_screenshot.c bbox）。
- 横向行为面：换行点从 264 前移到 196 → 行数变多、滚动更早出现——正是冻结设计（VQ-A2 超屏高滚出 / VQ-A3 长文滚动看完）的预期行为，非副作用。
- 需求/架构/契约/debt/governance 面：零触（faceMap 见分流 review：requirements 版式锚站在修复一侧，spec 未钉像素宽度；contracts 无像素版式；debt 无同族在途件）。**本变更单触实现面，触面 <2，不建 spine**（change-intake.md §1 比例化）。

# 验收要点

## host

- 既有冻结件零宽度钉（architect 已核查：voice_round_ui.acceptance.c 全文无对该两 label 的 set_width/set_size/set_pos needle；归档件同查无）→ 既有齿改后应全绿，无需先改任何冻结验收。
- 新增结构齿仿 MCD-A1 形态：int 提取三个 `lv_obj_set_width(..., W)`，断 W 在场且 W ≤ 196；反证齿 = 264 旧值销账。承载位 acceptance-author 定：新建本包验收文件（常规路径，推荐）或 additive 补进 voice_round_ui.acceptance.c（tighten-only 窗口：纯 additive、删除行=0、REVISION LOG 三件套）。
- 相对有效基线零新增红。

## 真机（human 在场）

- 语音轮三态各发一段长文本目测右缘：echo（LISTEN/THINK）、answer（TALK），行尾不再被裁；含 THINK 态让位（y104/高 148）下换行一致性；标头文案显示不回归。

# changeManifest（预定改动面，仓根相对）

- firmware/main/display/lcd_display.cc

# impactedTests 种子

- firmware/main/test/voice_round_ui.acceptance.c（结构回归，零改动应绿）
- 新增/补齿承载位（acceptance-author 定，见验收要点）

# 非目标（不扩本包）

- 标头 x=124 位置是否应迁 x=192 对齐右列：独立版式决策，非本包。
- 滚动条样式定制（LV_PART_SCROLLBAR）：现状默认主题可接受。
