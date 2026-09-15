---
changeId: 260911-render-pages
date: 2026-09-12
reviewer: architect（只读架构预审，plan.md frontmatter 欠账补课——2026-09-11 human 扩包裁定后、acceptance-author intake 前；判据经主线程转写落盘，评审 agent 对包文件零写入，先例同 arch-review.md 头注）
verdict: GREEN-WITH-ITEMS
---

# arch-review-expansion · 260911-render-pages 扩包补课预审

裁断：**GREEN-WITH-ITEMS**——架构成立、无需拆包、无需改路由，进 acceptance-author。
PKG-2（260911-screen-unification）已在本 worktree base（83a6e66）之前合并（merge d80e3191b091，
changes/INDEX.md:15 已 closed），F3 的 rebase 校正条件成熟，本次已实地做实（见 §A）。
Items 5 条（§D），全部可在 intake 内闭合，无一条需要回改 human 裁定或扩包范围。

核实方式：只读 git 树 + Grep 实地。关键锚均给出精确 path:line。

## A. 重点 A：crossEnd 撤 exempt 的新齿面（F3 rebase 校正做实）

### A.1 PKG-2 合并后的真实落点结构（实地核实）

PKG-2 的落地形态与「SetChatMessage/SetStatus/SetEmotion 落点改写 layer API」的字面预期不同，
实际结构是三层：

1. **语义源（application.cc，未动）**：display->SetChatMessage/SetStatus/SetEmotion 调用面
   原样保留（application.cc:75-859 全部调用点在树；五态语义源 = 设备态切换
   application.cc:713-751 STANDBY/CONNECTING/LISTENING/SPEAKING 分支）。PKG-2 没有改写调用侧。
2. **layer 化落点（PKG-2 的，本包零触碰）**：LcdDisplay::KidPalReparentChatToLayer
   (firmware/main/display/lcd_display.cc:1198) 把聊天 chrome 六件（top_bar_/status_bar_/
   bottom_bar_/emoji_box_/preview_image_/low_battery_popup_，:1213-1220）迁到常驻 parent；
   由板级装配调用（firmware/main/boards/waveshare-s3-rlcd-4.2/waveshare-s3-rlcd-4.2.cc:387，
   lv_layer_top()）。前台仲裁 = kidpal_layer.{c,h}（firmware/main/display/kidpal_layer.h:47-52
   六函数）+ kidpal_render_node.cc 的 show_chat/voice_begin/reconcile 路径。
3. **内容驱动面（本包五态换皮的真实落点，F3 要钉的「layer 内容驱动入口」）**：
   - LcdDisplay::SetChatMessage — lcd_display.cc:964（活分支；:502 还有 WeChat 风格孪生定义，
     CONFIG_USE_WECHAT_MESSAGE_STYLE 未设（firmware/sdkconfig:729）为死代码）；
   - LcdDisplay::SetEmotion → lcd_display.cc:973（emoji_image_/emoji_label_ 切换）；
   - LvglDisplay::SetStatus — firmware/main/display/lvgl_display/lvgl_display.cc:72
     (status_label_ 文本；SetStatus 不在 LcdDisplay，在父类)；
   - LcdDisplay::SetupUI — lcd_display.cc:774（活分支；聊天 chrome 建立面——
     status_label_:882、chat_message_label_:906、emoji_image_、bottom_bar_、low_battery_popup_）。

**结论**：F3 所担心的「layer 内容驱动入口符号在 PKG-2 合并前不在树」已消解——入口就是
Display 虚接口的三个实现（内容侧）+ SetupUI 建立面，全部已在树、行号可钉。五态齿
（v3_restore.acceptance.c）的对端断言面可以现在就写死，不需要再留「机制优先、符号 rebase
校正」的活口。

### A.2 counterparts 修正建议（def 形式，planner/acceptance-author 折回 taskpackage）

现有六条全部保留有效（schedule_page.c/story_page.c/char_stroke.c 三条是换皮对端；
application.cc 三条是五态语义源锚点，语义仍在）。建议新增五条显示实现侧对端：

```json
{ "path": "firmware/main/display/lcd_display.cc", "symbol": "LcdDisplay::SetChatMessage" },
{ "path": "firmware/main/display/lcd_display.cc", "symbol": "LcdDisplay::SetEmotion" },
{ "path": "firmware/main/display/lcd_display.cc", "symbol": "LcdDisplay::SetupUI" },
{ "path": "firmware/main/display/lvgl_display/lvgl_display.cc", "symbol": "LvglDisplay::SetStatus" },
{ "path": "firmware/main/display/reminder_page.c", "symbol": "kidpal_reminder_page_render" }
```

理由：application.cc 三符号只是调用点，五态齿若只盯调用面则断言不到「内容真的换成五态
演出」——内容变更发生在上述五个实现符号里。reminder_page.c::kidpal_reminder_page_render
（:1105，提醒态落点；:1144 仍 LV_OPA_50，F4 成立且符号已钉）。

### A.3 五态齿写法建议（advisory，不构成门）

建议把「设备态/role/status/emotion → 姿态 + 气泡文案 + 键帽条文案」的五态选择逻辑抽成
纯 C 模块（照 kidpal_layer.c 先例：无 chip/FreeRTOS/LVGL 依赖、host 可编、weak 哨兵 + 强
符号覆盖），lcd_display.cc 只做 LVGL 控件接线。收益：五态齿获得 host 行为齿（而非纯源码
扫描），红基线「机制优先」有真实可红的断言对象；同时规避 lcd_display.cc 的 #if 双分支
扫描陷阱（§B.3）。若 acceptance-author 判断源码扫描齿已足量，此建议可不入包——判定权
在 acceptance-author。

## B. 重点 B：五态同文件面——断言面重叠与布防

### B.1 所有权拆分核验：不重叠，成立

PKG-2 齿（firmware/main/test/screen_unification.acceptance.c，已冻结在树）断言的全是
结构面：kidpal_layer_* 接线计数（s2，:903-920，阈值型 >=，加法安全）、单屏不变式
（s3，:963-991）、零残留名（s1，:849-885）、冻结面 canary + 直连守卫（s4，:1008-1019）。
本包五态齿断言内容面（五态演出渲染）。两包对 application.cc 三符号的双登记是「语义源
锚点」级别的共享，非断言面重叠。但不重叠是有前提的——见 B.2 六不变式，违反任一条即红
PKG-2 冻结齿（那是 acceptance-author 留痕修订都不该兜的越界，属于「本包不该碰的结构面」）。

### B.2 PKG-2 合并带入的新不变式（builder 铁律/S0.3 勘察清单必须布防）

screen_unification.acceptance.c 的扫描跑真实工作树 firmware/main（排除 test/host_app/
build/.git/assets，:299-307），即本包所有新增/改写的固件源码都在扫描面内：

1. firmware/main 全域零残留字符串：s_chat_overlay / s_page_pinned / s_current_page /
   s_chat_scr / s_voice_ui_scr（s1，:849-876）——含注释（s_page_pinned 齿明确「过时注释
   一律清」）；新文件命名/注释都不得撞这五个词。
2. 无任何文件同时含 GetDeviceState( 与 lv_screen_load(（s1，:880-885）。
3. firmware/main/CMakeLists.txt 保留 kidpal_layer.c 条目（s2，:909-912）；本包若新增源文件
   （如五态纯逻辑模块）在此登记时不得动既有条目。
4. firmware/main/display/ 下 lv_obj_create(NULL) 恰好 1 处（s3，:963-967）——五态内容
   不得自建屏，只能落在 s_kidpal_scr / lv_layer_top() 的 chrome 上。
5. firmware/main/display/ 下每次 lv_screen_load( 实参必须恒为 s_kidpal_scr
   (s3，:983-991，按出现次数钉死)——五态/换皮代码禁止任何屏切换。
6. firmware/main 零 openclaw / 18789 字面（s4，:1014-1019）。
7. 「layer 底座与优先级仲裁零触碰」（RP-A9 既有约束）随 PKG-2 落地后具体化为：
   kidpal_layer.{c,h}、kidpal_render_node.cc 的 show_chat/voice_begin/reconcile 路径、
   LcdDisplay::KidPalReparentChatToLayer、板级装配 :387 接线——均不得改语义。
   例外（内容级、合法）：KidPalReparentChatToLayer 的 chat_top[] 数组
   (lcd_display.cc:1213-1220) 是 chrome 成员清单，五态换皮若替换/改名 chrome 控件
   （如 bottom_bar_ 换键帽条控件）须同步该数组——这是成员表维护不是机制改动；
   S0.3 路点清单应把该数组记为 chrome 成员边界。

### B.3 lcd_display.cc 双分支扫描陷阱（P2-A1 同型教训，新齿布防）

lcd_display.cc 有 #if CONFIG_USE_WECHAT_MESSAGE_STYLE 双分支（:358-773 WeChat /
:773-971 非 WeChat），本板活分支是后者。任何扫 lcd_display.cc 的新齿若用 first-match
或计数定位，都可能被死分支孪生符号（SetChatMessage:502 vs :964、SetupUI:359 vs :774）
绊倒——正是 RP-A1 要修的 first-match 消歧同型。布防：新齿定位活分支（如同时要求命中
文件含 #else 后的活分支特征，或按 A.3 抽纯模块后直接断言模块符号）；扫描齿的定位策略
须在齿头注写明依据。

### B.4 既有冻结齿路径敏感清单（五态/换皮同文件面，逐件核验）

- screen_unification.acceptance.c：见 §B.2 七条，阈值型计数对新代码加法安全。
- key_route_a1.acceptance.c：扫 firmware/main 的 key_route_global/KEY_ID 系符号
  (:501-563)。S7.2 键帽条文字联动只改显示文案，不得改 key_route_global 调用形状；
  RP-A9 已含 PKG-1 路由语义回归复验，足够。
- schedule_reminder_b.acceptance.c：kidpal_reminder_page_render 等为接线计数阈值
  (>= 2，:712-715) + weak 哨兵行为齿——提醒弹窗换皮（F4）只改 kidpal_reminder_page_render
  函数体观感、保签名保接线，计数加法安全，低风险。
- story_p3 / schedule_p2 / ui_art_integration：taskpackage RP-A11 已申报路径敏感
  （story_page.c/schedule_page.c 换皮 + B8 资产闭集），处置通道（acceptance-author
  留痕修订不放松）已在包定义，不重复。
- home_p1.acceptance.c：P1 不在本包换皮面（F7 辩界已在 plan S8.1 划清），无新增风险。

### B.5 新齿文件接线位（infra 硬约束）

v3_restore.acceptance.c 注册进 firmware/main/test/host_app/main/CMakeLists.txt 时，
必须排在 host_infra_smoke.acceptance.c 之前（smoke 恒列表末位 = boundary 齿注册序守卫，
screen_unification s2 齿 :934-936 会机械核查注册序）。照 :85-91 key_route_a1/
screen_unification 的先例注释格式落位。

## C. 前次 F1-F9 状态复核（仅列因 PKG-2 合并而变化者）

- F3（medium-high，五态 symbol 未钉）：已消解，本次做实——见 §A。counterparts 修正建议
  五条（§A.2）+ 五态齿可直接钉符号，无需再留 rebase 活口。
- F4（medium，提醒压暗）：状态不变、符号钉实——kidpal_reminder_page_render
  (reminder_page.c:1105) 内 :1144 仍 LV_OPA_50（文件头注释 :975 声称 d50 网点但代码未兑现，
  注释与实现不一致本身即换皮工作量的一部分）；krm_pose_jump（:1033）已在，姿态资产路径通。
  活控件平铺合成机制不变（schedule_page.c 先例）。
- F6（medium，server 条件依赖）：条件已消解——260911-ecs-deploy-convergence 已 closed
  并合并（merge 16a47bdccdb8，changes/INDEX.md:13），早于本包分支 base。原担心的 deploy
  面并行冲突不存在了；若 S0.2 勘察定根因在云侧 producer，本包按 notes 既定申报扩 server 面
  (subsystem firmware+server) 即可，无需再复核 dependsOn/mergeOrder（对批次①已无先后
  关系可挂）。plan S0.2 的条件复核条款可由 planner 标记为「已解除」。
- F1/F2/F5/F7/F8/F9：PKG-2 合并未改变其状态，包定义折回方案维持原样。

## D. Items（intake 内闭合，非门）

1. counterparts 增补五条（§A.2 def 清单）由 planner 折回 taskpackage.json crossEnd
   （本预审只读不改包文件）；acceptance-author 产五态齿时对端按此清单 + 既有六条钉。
2. PKG-2 七不变式（§B.2）纳入 S0.3 勘察清单与 builder 铁律面（写入五态落点清单即可，
   不需要改 plan 结构）；chat_top[] 记为 chrome 成员边界、允许内容级同步。
3. v3_restore.acceptance.c 注册序：host_app CMakeLists 落位于 host_infra_smoke 之前
   (§B.5)，acceptance-author 新产文件时一并接线。
4. lcd_display.cc 双分支扫描陷阱（§B.3）写进五态齿的定位策略要求。
5. F6 条款解除标记（§C）：planner 在 plan.md S0.2 或 intake 记录标注条件已消解，
   避免执行期空转复核。

## E. 裁断依据摘要

- 拆包判定：不成立——还原轨三块（换皮/五态/字体）与缺陷轨共享同一渲染层文件簇与同一
  U 闭集修订通道，拆包只会放大跨包同文件面；扩包裁定（human 2026-09-11）方向与代码实况
  一致。
- 改路由判定：不成立——不触 docs/contracts/**（B8 是验收齿闭集不是契约面）、不跨系统
  结构性变更（server 面是条件性扩面且已有申报通道）、无 spec 修订需求。
- GREEN-WITH-ITEMS 而非 GREEN 的原因：counterparts 现清单断言面缺内容侧五条，五态齿若按
  现清单产会只锚到调用点、锚不到换皮落点——这正是 crossEnd 撤 exempt 要防的空心齿。
