---
changeId: 260911-render-pages
date: 2026-09-12
author: acceptance-author（红基线判定；本文件由主线程按 agent 交付全文转写落盘——agent 对 changes/dev/** 无写权，先例同 arch-review-expansion.md）
status: FROZEN-BASELINE
---

# 红基线冻结 · 260911-render-pages（human 指示 2026-09-12：不重跑，引在册）

## 一、在册红五条引证（不重跑）

| 红 | owner | 处置 |
|----|-------|------|
| WSA-A6:830 | 260903-device-ws-switch | 在册他人红，不代修 |
| WSA-A4:663 | 260903-device-ws-switch | 在册他人红，不代修 |
| **P2-A1:571** | **本包（RP-A1 修复对象）** | 阶段 1 已修齿——缺陷形态复核见 §二 |
| VRT-A2·E2:878 | 260829-voice-roundtrip（cwd 敏感，判读钉明 cwd） | 在册他人红，不代修 |
| SMK-A4（server 验收唯一红） | 260829-smoke-e2e | 在册他人红，不代修 |

出处 = changes/dev/260911-screen-unification/existing-red-inventory.md + changes/dev/260911-display-first-m1/existing-red-inventory.md（合并夜追记）。KEYA 5/5 + SU 7/7 绿。

## 二、P2-A1 缺陷形态复核（修齿依据）

齿 6 first-match 歧义：恒命中先行轮新增消费调用点 kidpal_render_node.cc（无 lv_anim|弹跳标记）；真定义处 schedule_page.c:727-741 实含 lv_anim 弹跳块——齿意图未被实现回归破坏，修后当前树即绿（红因是扫描定位歧义非实现缺失）。修法 = 定义签名 `int kidpal_schedule_page_render(` 文件族定位（schedule_p2.acceptance.c，tightly-only，REVISION LOG 在文件尾）；方案 A（render_node 补哨兵）禁令遵守。阶段 1 S1.3 验证口径相应放宽为「runner 复跑不回红」。

## 三、还原轨新齿红基线（v3_restore.acceptance.c，接线后首跑预期 = test 1-6 全红 + 守卫绿）

| 齿 | 类别 | 红因（当前树逐字核实） |
|---|---|---|
| test 1 五态内容选择 | behavioral | `kidpal_voice_state_look` weak 哨兵 -2（五场景 pose/键帽逐字「我在听！/说完啦/不等了/打断/知道了/叫醒它」+ 防御） |
| test 2 五态内容驱动接线 | structural | lcd_display.cc **活侧**（最后一个 `SetupUI` 定义点之后；死分支区间零命中——双分支陷阱布防，齿头注写明定位依据、禁 first-match）无 `kidpal_voice_state_look` 驱动 |
| test 3 姿态/键帽/气泡尾巴 | content | `pose_listen/think/talk/sleep_96.bin`、`键帽`、`bubble_tail_24.bin` 实现树零命中；守卫 pose_jump/pose_daily must-stay |
| test 4 提醒 d50 压暗（F4） | structural | reminder_page.c:1144 仍 `LV_OPA_50` + 零 `d50.bin`/draw_dots 消费 |
| test 5 页面换皮框族（RP-A8） | structural | schedule/story/char_stroke 零 `frm_`、schedule 零网点引用（涂鸦观感留 design review/真机轮，不进脆断言） |
| test 6 字体分型闭集（RP-A4/S0.1） | structural | `firmware/main/assets/fonts_root/` 不存在、三字体（font_body/font_title_zcool/font_num_baloo）零接线；含 KAP1-magic 排除 + spiffs_root 字体零侵入（B8 test 3/4 不误伤的机械前提）+ 扫描器自检 |

守卫类齿（application.cc 三语义源锚点、LvglDisplay::SetStatus、双分支结构锚）native green。

## 四、U 闭集修订后的连带红

ui_art_integration 白名单 47→48（唯一新增 KAP1 = `bubble_tail_24.bin` 24×24，V3 §06 气泡尾巴；压暗用既有 d50 网点零新增）：ART-A5 进入「缺 bubble_tail_24 落地」红（owner = 本包阶段 4/5，builder 落地转绿）。

## 五、F2/F5 新齿（落 story_p3.acceptance.c，REVISION LOG 2026-09-12 条目）

- F2 当前屏正文机制齿：渲染真帧 + lv_label 递归走树；判别式对行数参数不敏感（屏 0 含首行、不含末行「第十行」；翻页后首行不再可见）。缺陷形态（story_page.c:1201-1202 自述「此处绘制全文文本」直画全文）两判别式恒红。
- F5 渲染切片契约齿：冻结 `kidpal_story_reader_slice(st, screen, lines_per_screen, out_len)`（weak 默认 NULL 哨兵；指针/偏移进 story->text、边界/防御/越界屏 NULL）+ 结构半（story_page.c 须含该助手引用、禁词 `[512]/[1024]/[2048]/[4096]` 大栈缓冲；当前文件最大栈缓冲 64B 已核实安全）。
- 落点偏离申报：F5 原拟 story_page_stack.test.c 超写域（scope 闸拒绝），依 RP-A2 授权改落 story_p3.acceptance.c 冻结件（强于 .test.c）；若 planner 坚持 .test.c 归位需 builder 搬移。planner 裁定：**维持 story_p3 冻结件落点，不搬移**（冻结件强于 .test.c，偏离已在 REVISION LOG 留痕）。

## 六、接线注记

v3_restore 未接线前不进 runner——红基线形态 = 缺实施 + 缺接线同属 builder 落地面（CMakeLists 注册 + `firmware/main/display/kidpal_voice_states.{c,h}` 新模块）。host runner 构建 exit 0 已验（三修订件编译入 ELF；v3_restore 单独 gcc -fsyntax-only error=0 warning=0）；ELF 运行留 verifier 首轮。

## 七、终态追记（2026-09-12，acceptance-author 第二轮 + builder 实现后）

- 两条冻结齿自冲突已修订（tightly-only，REVISION LOG 各两条）：①v3_restore test 6 × ui_art ART-A5 互斥——spiffs_root 判据改为「font_ 前缀条目必须携 KAP1 magic」（KAP1 样张 font_zcool_30.bin 豁免，非 KAP1 新字体仍拦外）；②story_p3 F5 行为半 strstr 越窗矛盾——改 `kstory_window_contains` out_len 有界窗口判。
- builder 实现落地后 host runner 全量实测：**124 Tests / 3 Failures / 0 Ignored**，3 红逐条比对均为 §一在册非本包红（VRT-A2·E2:878 / WSA-A6:830 / WSA-A4:663）；在册第 4 条红 P2-A1 已由 RP-A1 修齿消解出失败清单。
- 本包齿终态：v3_restore 6/6、story_p3 10/10、schedule_p2 7/7、ui_art_integration 11/11、story_page_stack 3/3、KEYA/SU 不回归。
