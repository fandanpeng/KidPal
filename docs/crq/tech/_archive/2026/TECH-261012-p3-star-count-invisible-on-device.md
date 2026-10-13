---
id: TECH-261012-p3-star-count-invisible-on-device
title: P3 星星记录本列表右上角 ★/×N 计数真机不显示——根因已钉死（chrome 顶带 top_bar_ 50% 白蒙层 × 1bpp 0x7fff 阈值漂白 INK 灰；四轮 TEMP-DIAG bar 透明一行试验真机证实）
domain: tech
status: closed
severity: SHOULD
owner: dev-loop（P3 渲染/显示面）
trigger: 下个动 P3 显示面 / 固件渲染层 / kidpal 页面 flush 链路的包；根因已钉死（260912 第四轮 TEMP-DIAG bar 透明一行试验真机证实——human 观测 ★/×2 可见，全记录见正文追加段），修法随包落（候选 A = 本板 top_bar_ 背景透明化），修完随包真机复验 SL-A5 计数项
opened: 2026-10-12
closed: 2026-10-13
closedBy: 261012-p3-topbar-ink-bleach-fix（dev-loop integrator closeout，修复随包落地）
paths: ["firmware/main/display/story_page.c", "firmware/main/display/kidpal_render_node.cc", "firmware/main/boards/waveshare-s3-rlcd-4.2/custom_lcd_display.cc", "firmware/main/test/story_page_v2.acceptance.c"]
source: 261010-p3-storylist-cold-start-restore SL-A5 真机轮 human 观测（「P3 现在是记录本列表了，但是右上角没有计数」）+ 2026-10-12 现场三轮 TEMP-DIAG 设备实测（串口捕获 c:/tmp/diag-ink.log / c:/tmp/diag-flush.log，诊断代码现仍在本 worktree 未提交 diff 中可复核）
---

**证据**（全部为 2026-10-12 真机 TEMP-DIAG 实测逐字，捕获文件在 c:/tmp/、诊断代码在本 worktree 未提交 diff）：

1. **设备侧数据链全对**：front `story_push_diag TEMP-DIAG restore=True total=2 items_n=2` / `sent ok`（ECS journalctl，20:05:26 与 20:06:17 两轮建连各一帧）；板端 `MCP: story_list_dispatch corr_id=kdp-126645 items=2 counter=2 degrade=false` + `story_list_gate defer_front page=0 voice_front=0 restore=1`；`story_page: STORY_RENDER page=0 vcount=2 lcount=2 rec=0`；`KSY: story_list_draw_diag TEMP-DIAG counter=2 n=2 drawn_star=1 drawn_cnt=1`——view 数据正确（counter=2 恒信 total）、LIST 非空分支画齿确认执行。
2. **面板影子缓冲像素实锤**（RLCD_Display 推面板前探 DispBuffer x[300..399]×y[0..29]，注意极性 = 置位比特是白、清位比特是黑）：P1 态 `topright_ink=2534`（= 466 黑像素 ≈ P1 湿度/电池图标，吻合）；翻 P3 后 `topright_ink=3000` = **3000/3000 全置位 = 整条计数带全白、★/×2 零黑像素进面板缓冲**。同时 flush_diag 证实该屏为全屏刷新（`flush_diag TEMP-DIAG area x[0..399] y[0..299]`）——不存在「局部刷新漏带」。
3. **排除项**（每项有源码/实测依据）：本批（f22a979）story_page.c diff 只动空态分支，非空 LIST 画法（:1137-1139 ★/×N）与 K2 时代逐字相同；font_puhui_14_1 为内嵌 fmt_txt 字体（无运行时 bin 加载），★(U+2605) 与 ×(U+00D7) 均在字符集且 cmap 覆盖（range [7841,4520) / [211,8) 实解析确认）；chrome 顶带 top_bar_ 全宽橡皮擦假说被 P2 表头可见证伪（P2 表头带 schedule_page.c:16 钉 y[0..29] 与计数带同区域、真机可见）。
4. **K2 以来真机从未显示过非空列表计数**——本批之前设备 P3 恒空态（TECH-261010-p3-storylist-cold-start-empty 原账），故无法排除该缺陷自 K2 起即潜伏（K2 看屏终验未覆盖非空列表计数项）。

**影响面**：P3 列表态右上角星星计数（S-F2.1 AC1「×N 右上角」、页身份载体）真机不可见；同画法的 AC5 无内容空态 ×0（story_page.c 空态半 + kidpal_render_node.cc AC5 分支）大概率同样不可见（同坐标同画法，未真机验证）；列表本体、恢复推送、删除对账均正常工作。host 验收面存在覆盖洞：SLP-SLA5 tooth 4 只像素探过空态 ×0，非空分支 ★/×N 计数带从未被像素探针覆盖（本缺陷 host 全绿的原因）。

**建议改动**（候选，交 review 不自批）：① 根因钉死两路（择一或并行）：设备 label 对象级日志（创建后 lv_obj_get_width/height/is_visible/坐标——width==0 即运行时字形缺失实锤；非零则查 z 序/父容器裁剪/多 render 叠加无 clean——rn_render_kidpal_page_locked 每次 flip 新建 label 不删旧对象，叠层行为待核）；host 非空分支像素探针补齿（story_page_v2.acceptance.c 新 tooth：total=2 渲染后探计数带墨，复刻 tooth 4 harness——host 红即可 host 内循环修，host 绿则确证设备 flush/合成层）；② 修复随根因落；③ 收尾删除两处 TEMP-DIAG 诊断代码（custom_lcd_display.cc RLCD_Display 墨量探针 + flush_cb 区域打印；story_page.c:1140 / kidpal_render_node.cc:925 画齿日志）——删前须过 `git diff` 逐行核对仅删诊断行。

**风险**：riskTier: medium——根因未钉死（Track B 三要件 ②「修复明确」当前 no：嫌疑收敛在 LVGL 对象级 vs flush 合成层、两条调查路线未定），不满足快路径就绪；实现修复时按常规 `/triage-change` 分流（预期 dev-loop 单包、firmware 显示面 + 验收齿两处动）。修通前 P3 页身份计数在真机缺位（AC1/AC5 验收项挂起，261010 包 SL-A5 真机轮的计数分项未过）。owner = dev-loop；本条目登记后工作即交出（handoff = 下个动 P3 显示面的包，见 trigger）。

---

**追加（2026-10-12 第四轮 TEMP-DIAG，根因钉死 + 正文两处证据更正）**：

**根因（真机证实）**：xiaozhi 聊天 chrome 顶带 `top_bar_`（全宽 400px、高约 29px = pad_top 12 + 图标行高 15 + pad_bottom ≈2-4、**白色 `LV_OPA_50` 背景**，[lcd_display.cc:944-959](../../../firmware/main/display/lcd_display.cc) 构建）自 260911 归一屏包经 `KidPalReparentChatToLayer(lv_layer_top())`（[waveshare-s3-rlcd-4.2.cc:848](../../../firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc)，无条件执行）起恒浮于所有 KidPal 页之上。本板 flush 阈值 `*buffer < 0x7fff → 黑`（custom_lcd_display.cc Lvgl_flush_cb）；经 LVGL 9.3 `lv_color_16_16_mix`（floor 取整，mix=(128+4)>>3）混合 50% 白后：纯黑 0x000000 → **0x7BEF（< 0x7FFF，存活）**、INK 灰 0x333333 → **0x94B2（≥ 0x7FFF，漂白）**、TEXT 灰 0x222222 → 0x8C71（漂白）。P3 计数 ★/×N 位于 (316/344, 8)、字形带 y[8..26]，整条落在 bar 覆盖区 y[0..28] 且用 KSY_COLOR_INK → 整段漂白，与面板探针 `topright_ink=3000`（零黑像素）精确一致。**定案试验**：custom_lcd_display.cc SetTheme 的 top_bar_ 样式块加一行 `lv_obj_set_style_bg_opa(top_bar_, LV_OPA_TRANSP, 0)`（其余零改动、墨量探针保留），烧板后 human 真机观测翻 P3——「可以看到星星 *2 了」（2026-10-12 21:5x，human 原话）。

**正文两处证据更正**（上文档前段为本日早三轮所记，保留原样、以下更正为准）：① 前段「P1 态 topright_ink=2534」为误记——时间线上 2534 打印在 P2 渲染后（P2 render @19452 → flush 探针 @20132），是 **P2 态**值；P1 的值在捕获开始前已打印、change-only 探针不复打（P1 图内嵌的温/湿/电条为烘焙纯黑像素，按同机制存活，故 P1 态带内有墨）。「≈湿度/电池图标吻合」是巧合的错误归因。② 「chrome 顶带全宽橡皮擦假说被 P2 表头可见证伪」**不成立**——P2 表头/表格线用 KSG_COLOR_TEXT/KSG_COLOR_SEP = **纯黑 0x000000**，50% 混合后落 0x7BEF 侥幸过阈存活；bar 是真实盖着的（P2 带内 466 黑像素 = 线 256 + 周六/周日表头 ≈210，逐像素对得上）。橡皮擦一直在，只是擦不掉纯黑、擦得掉 INK 灰。③ 前段 AC5 预判反了：kidpal_render_node.cc:906-913 AC5 分支用 `lv_color_black()` 纯黑 → 应**可见**；story_page.c:1127-1129 的 n≤0 空态分支用 INK → 不可见——两个 AC5 渲染入口在真机上会一个显示一个不显示（潜在同类缺陷面，修包一并钉口径）。

**连带预测**（随修复真机轮可一并复验，改动前均应不可见）：阅读态页标题（story_page.c:1216，y=8、0x222222）同落 bar 覆盖区，改动前应同样漂白；后续任何放 y[0..28] 且用 INK/TEXT 色的内容同病。

**host 覆盖洞补注**：host harness 无 chrome、无 top_bar_——本根因缺陷面（chrome 浮层 × 阈值交互）host 不可达，host 齿全绿的原因。修包验收齿须落结构面（top_bar_ bg_opa 常量 / KidPal 顶带颜色常量）而非像素探真（探真只能落真机轮）。

**修法候选（交 review 不自批）**：A（结构性候选正解，即本次定案试验本身）= 本板 top_bar_ 背景透明化——wifi/mute 图标为 bar 子件、不透明字形直画不受影响，P1 顶带本就白底视觉无差，把「xiaozhi 半透明聊天顶带」与「KidPal 页面顶带内容」解耦；B（保守治标）= KidPal 顶带内容全改纯黑——不推荐，依赖 0x7BEF < 0x7FFF 约 3% 余量活着，LVGL 升级改混合取整即翻，且 reader 标题/两处空态入口等全要跟改、漏一处即潜伏同类缺陷。TEMP-DIAG 清理面随修扩为四处（原 ③ 的两处诊断 + 前段探针/flush 打印 + 本次 SetTheme 一行，删前 `git diff` 逐行核对仅删诊断行）。

---

**收口（2026-10-13，status: deferred → closed，closedBy = 261012-p3-topbar-ink-bleach-fix）**：修复随包落地 = 候选 A 转正（custom_lcd_display.cc SetTheme `top_bar_` 背景透明化常驻 `LV_OPA_TRANSP`、注释转正式根因/解耦注释）+ TEMP-DIAG 诊断残留归零（三源文件负断言齿承载，36 行诊断 diff 除 SetTheme bg_opa 一行转正外零复活）+ AC5 两入口颜色统一 KSY_COLOR_INK（宏上提 story_page.h 单源）——host 三齿 SLP-TB1/2/3 全数转绿（红基线 165 Tests / 2 Failures 设计态红 → 落地 165 / 0，既有齿零新增红）+ 变异取证三处 turnedRed 全复绿；verifier scoped round 2 八闸全绿（evidence = delivery/taskpackages/261012-p3-topbar-ink-bleach-fix/verifier-evidence/rounds/r2.json）。真机轮 TB-A4（SL-A5 计数分项 / AC5 ×0 两入口 / 阅读态标题顺带目测 / P1 与聊天态顶带回归目测四项）= live-manual structural 豁免、pending-human——待 human 在场持板执行，**尚未执行（非豁免通过、非缺口豁免）**，留痕于包 review-packet「未覆盖面申报」；连带预测三项（阅读态标题自愈 / P1 顶带无差 / 后续落 y[0..28] INK 内容免疫）随真机轮一并复验。
