# Plan · 260921-m3-k4-p1-cjk-dynamic-text

```yaml
rigor: lite
riskTier: medium
sourceType: null          # origin.type = human-confirmed-plan（无 sourceType 镜像，缺省）
reviewed-by: architect（triage 四档裁决 2026-09-21，②单包 Lite，human 确认不改判）
date: 2026-09-21
```

## 来源与立项

挂账④（`delivery/workstreams/m3-schedule/progress.md` K1 live 半热修段第④条）+ human 2026-09-21 真机轮立项 ack（动态内容用普通字体、与对话页同款）。驱动实证：K1 热修轮 + K3 真机轮两轮目击——首页 upcoming 时间（ASCII）可画、CJK 课程名缺失，云端数据已确认到设备。

## 目标效果（一句话）

首页 P1 软帧在「接下来」卡、后续事项行、明日预告行把日程项 name（开放集合 CJK 文本）用 14px 普通字体（设备 `font_puhui_14_1` / host Source Han 子集）正常渲染进钉区，ASCII time 维持现状，从「画不出即丢弃」变为「画得出、超宽截断不越区」。

## 关键接缝（复用先例）

- **新增字形获取弱符号 seam**（建议形状：`p1_font_glyph(uint32_t cp, /*w,h,adv,bitmap*/...)` 返回位图描述，弱默认 = 不可用；参照 `host_infra_render_p1_frame` 弱/强模式 + `kidpal_art_glyph` 的 img_t 返回形状）。
- **设备强符号**：kidpal_render_node.cc，`ksg_text_font()` 同款双分支取 `font_puhui_14_1`，`lv_font_get_glyph_dsc` + 字形位图（A1/A4/A8 alpha 阈值化成 1bpp 墨）。
- **host 强符号**：host_infra_seam.c，`lv_font_source_han_sans_sc_14_cjk` 同款路径（host_app REQUIRES lvgl 已在，字体已开）。
- **home_p1.c 内新增**：UTF-8 解码（纯逻辑）+ name 逐字形 blit（复用既有钉区 clip/最近邻纪律），落位进 next {136,48,392,168}、nextlist {136,172,392,214}、preview {136,220,392,242} 既有钉区。

**字体裁定收敛（实证）**：human「对话页同款」= `font_puhui_14_1`（lcd_display.cc:1152/1176/1195，`BUILTIN_TEXT_FONT = font_puhui_14_1`）；挂账④原文的 `lv_font_source_han_sans_sc_14_cjk` = host_app 专用（设备 sdkconfig 未启）。schedule_page.c 设备侧用的正是 `font_puhui_14_1`——两个裁定在设备上同一字体，方案照抄 `ksg_text_font()` 双分支同时满足。

## 验收方向

- **host runner 新齿**：注入带 CJK name 的 upcoming → render → pixel-probe：(a) name 墨落对应钉区；(b) 长 name 截断不越钉区右界；(c) 空日程/单项各「零墨」pin 不回归；(d) UTF-8 截断尾序列安全停笔；(e) 字形缺失（弱默认路径）退化为现状不崩。既有 74 齿 must-stay-green。
- **device build 闸**：xiaozhi.bin 构建过 + 体积余量记录（无新增大静态量，帧缓冲不增）。
- **真机目测点**（挂 K3 设备轮窗口顺带或独立开窗）：接下来卡课程名可见且与 time 数字基线协调；后续事项/预告 name；14px 1bpp 阈值化后的可读性（阈值调整属设计迭代不触契约）。

## 非目标

固定标签（接下来/明天/还有）烘焙资产维持不动（殿后）；`hp1_item_t` 与 11 个冻结契约函数零偏移；home_upcoming.{c,h} 零改；契约/spec/agent 定义零改；schedule_page 与 K2 面零触碰。

## 边角（review 发现，builder 易漏）

home_upcoming.c:393–396 按字节把 name 截到 31 字节，会把 UTF-8 多字节字符截半（31 字节 ≈ 10 个 CJK 字符）——P1 解码器必须对不完整尾序列安全停笔（截半字符不画、不越界、不崩）；这是数据已定形状，解法放 P1 侧容错，不动 K2 已合的 home_upcoming。

## 调度注记

本 worktree 有 K3（260921-m3-k3-voice-schedule-entry-live）在途未提交改动——新包排 K3 合并后串行（树池/mergeToken 纪律），真机目测可与 K3 设备轮窗口协调。
