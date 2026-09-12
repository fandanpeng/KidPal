---
changeId: 260911-render-pages
rigor: full
riskTier: high
sourceType: regression
gateProfile: [host-teeth, device-round, frozen-acceptance-revision]
reviewed-by: architect (triage 2026-09-11, 来源锚: PROGRESS-SUMMARY-260910 §四A/§四D/§七 #9-#12 + review-packet F5/F2)。扩包注记：2026-09-11 human 扩包裁定（对话内三问）后 riskTier 上调 high 为 main-thread 评估，architect 复判未跑——intake 时 planner 须先补一次 architect 预审（重点：crossEnd 撤 exempt 的新齿面 + 五态同文件面），再进 acceptance-author。
date: 2026-09-11
---

# 批次③ 渲染观感 + V3 整屏还原（#9-#12 缺陷轨 + 换皮还原轨）

> 分流裁决（2026-09-11 两次 human 触点合成）：② 单包 Full。改自有实现面（固件渲染层 + 可能
> server producer）行为 → 过门；同一渲染层紧耦合簇、单 owning Pod、无迁移删数 → 不拆包；
> 2026-09-11 human 扩包裁定（对话内三问钉死）：「V3 整屏设计还原」不另立项、扩进本包，
> 范围 = 页面换皮（P2/P3/P4）+ P4 笔顺观感还原 + 语音轮五态演出换皮 + V3 字体体系并包；
> 字体生产并包（原 S0.1 触发器「拆出另立项」条款**废除**——换皮依赖字体，拆出去就没皮）。
> 还原轨使 riskTier medium→high（UI 面大改 + 新增冻结齿面 + 跨包同文件约束扩大）。
> 接续顺序锁定（human 2026-09-11 触点①确认）：批次②（显示主权）先、本包后——#11 数据链
> 与 #9 screen_lines 接线与 ② 的 kidpal_render_node.cc / mcp_server.cc 重构面同文件；
> 五态演出换皮进一步**依赖 260911-screen-unification 已合并**（聊天 layer 底座是五态落点）。
> 逃生门：执行中任何 delta 的合理化需要改 docs/contracts/**、docs/spec.md、或放松任何冻结
> 验收齿断言 → 停，转 /arch-change + architect-reviewer 四分类，本包不批事实源变更。

## 范围与根因假设（缺陷轨 bug 框架）

- **#9 故事页排版**：story_page.c `ksy_label` 建标签不设宽度不开 wrap → 正文（≤2047B）被当
  一行横向裁切；`kidpal_story_page_render` READER 分支画全文不消费 v->screens 分页。冻结
  验收头注 ⑤ 本要求「当前屏正文」→ 本项 = 兑现冻结契约既述行为、关闭 P3-E2 观感殿后项，
  非语义变更。
- **#10 字体**：正文走板级 BUILTIN_TEXT_FONT=font_puhui_basic_14_1（阿里普惠 basic 小子集）
  →「猬」等次常用字缺字形渲染空白；设计字体 font_zcool_30.bin 是 300×52 KAP1 位图样张
  （非可用 CJK 字体）。
- **#11 课表页**：真机报「没用课表具体内容 + 非设计字体」——数据链根因未定（server 未推 /
  mcp schedule 路由 → render_node 缓存链断 / 缓存时机），先勘察后修。
- **#12 P2-A1 齿**：schedule_p2.acceptance.c 齿 6 对 `kidpal_schedule_page_render(` first-match
  取首个命中文件要求含 lv_anim|弹跳；先行轮新增消费调用点 kidpal_render_node.cc 恒为首个
  命中且无该标记 → 恒红。扫描器定位歧义，非真实回归（schedule_page.c 实含 lv_anim）。

## 范围与改法（还原轨——V3 定稿 docs/ui/kidpal-ui-design-v3.html）

- **P2/P3/P4 页面换皮**：LVGL 骨架与动态文本语义保留（课表列表/故事分页/翻页逻辑不动），
  框族 9 宫格（frm/frm-sm/frm-flat 三框已入 B8 闭集）+ 三档网点（d50/d25/d12）+ 涂鸦
  （星/云/波浪，每屏 ≤2 处）铺开；标题站酷快乐体、数字 Baloo 2 按 V3 字号阶梯
  （112/30/20–22/14/下限 12）落位；页面坐标从设计稿 HTML 转写生成（禁手抄，走
  ui-implementation-strategy §2.5 转写纪律）。资产缺口面：站酷/Baloo 字体 + 气泡尾巴 +
  弹窗压暗层**不在 B8 47 枚闭集内**，一律走 U 闭集修订通道（acceptance-author + 跨包留痕）。
- **五态演出换皮**：PKG-2 迁出的聊天 layer 内容替换为小伴姿态演出——聆听（pose-listen +
  气泡「我在听！」）/ 思考（pose-think + 问号）/ 播报（pose-talk + 声波条 + 同步文字框）/
  提醒（pose-jump + 弹窗 d50 压暗）/ 低电打盹（pose-sleep + zZZ）；底部键帽条任何状态
  常驻（V3 §06 硬要求）。七姿态资产已在 B8 闭集。改法落点在 PKG-2 迁出的聊天 layer
  内容面，不碰 layer 底座与优先级仲裁（那是 PKG-2 的）。
- **P4 笔顺页观感还原**：米字格三态笔道（已写 9px 实心黑 / 当前笔白色衬边 + 握笔小手
  领走 / 未写虚线点链）+ 进度珠 + 右栏拼音/部首/组词速查卡按 V3 布局；Teaching 红线：
  示范字横平竖直规范字形、说明文字回归可爱体；B8 打包策略锁定（笔形 3 态叠加复用 +
  米字格单份背景，禁逐笔逐态整帧烘烤）。
- **V3 字体体系并包**：正文选型与缺字策略走 S0.1 决策；标题（站酷）/数字（Baloo 2）
  资产经 U 管线新产 + U 闭集修订入集。

## 铁律约束（全阶段有效）

1. **冻结齿保绿**：改 story_page.c / schedule_page.c / char_stroke.c / 聊天 layer 内容后
   必须跑 host runner（`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/
   espidf_wsl.sh build` + `./build/host_infra_test.elf`），全部 TEST_CASE 绿、exit 0。
   齿红 → 升级 acceptance-author，builder/fixer 不得改冻结验收件。
2. **栈纪律**（story_page.c 文件头冻结 + story_page_stack.test.c 3 齿）：story 链跑在
   ssl_receive 4096B 栈上，禁止新增大栈缓冲；正文切片用指针/偏移进 story->text（const），
   不复制 2KB 进栈；大对象一律堆分配。
3. **冻结头 additive-only**：story_page.h 冻结声明 verbatim 保留，只允许新增字段/宏
   （如 KSTORY_READER_LINES_PER_SCREEN、view 行程参数 screen_lines），不得改名/删除/改用途。
4. **资产落点分型（F1 更正）**：唯一确定新增 KAP1 = 气泡尾巴——走 U 闭集修订
   （acceptance-author REVISION LOG + 跨包留痕，列入 47 枚白名单）；字体是 lv_font_conv
   产物（非 KAP1），落 spiffs_root 会打红 B8 test 3（KAP1 load）+ test 4（闭集枚举
   foreign），**不得并入 KAP1 闭集**——三分道路径由 S0.1 逐字钉：编译内置（managed_components
   / CMake 内置）/ 独立非 KAP1 SPIFFS 落地 + 分型新齿（不进 B8 齿）/ 预渲染位图资产
   （大号数字块走此路、回 KAP1 通道）。白名单外静默新增仍禁止。
5. **冻结面 git 零触碰**：docs/contracts/**、managed_components/** 原样；*.acceptance.*
   仅限本包明示的 acceptance-author 留痕修订（P2-A1 齿 6 + U 闭集扩容 + #9「当前屏正文」
   机制齿新产 + 换皮致路径敏感齿必要修订），REVISION LOG 齐、断言语义不放松。
6. **V3 红线清单**（换皮执行纪律，V3 §07）：除 #000/#fff 不引入第三色；线宽 ≥3px；网点
   单元 4px、单屏面积 ≤10%、用途四类白名单（腮红/虚线/压暗/弱底纹）；无渐变无投影；可点
   按钮暗示禁止；动效只保留眨眼 4.4s/嘴部 240ms/声波/笔顺 950ms——五态新增动效（嘴部
   换帧/声波条/低电 zZZ）须逐项纳入本白名单核对（F9）；单屏倾斜 ≤3 处。
7. **坐标转写纪律**：换皮坐标从设计稿 HTML style 抽取生成布局表，禁手抄（实战胜于
   一半工时在此，见 ui-implementation-strategy §2.5）。

## 阶段 0 · 前置决策与勘察（只读，不改东西）

- S0.1 **V3 字体方案决策**（human 拍板，产出 = 字体决策记录落包 evidence；**并包裁定下
  原触发器条款废除**——字体生产工作确定在本包内做，本触点只裁方案不裁去留）：① 正文
  选型——managed_components/78__xiaozhi-fonts 的 font_puhui_common_14_1（cbin lv_font_load）
  或编译内置 font_puhui_14_1（换板级 BUILTIN_TEXT_FONT）或 U 管线新产字体 bin；② 缺字
  覆盖策略——实测候选字体对「猬」及抽样生字集的 glyph 覆盖，不覆盖则定 fallback 或扩
  子集；③ 标题/数字字体生产方案——站酷快乐体授权确认 + Baloo 2 子集化（lv_font_conv →
  SPIFFS .bin，或大号数字块预渲染位图资产）；**每字体逐字钉三分道路径**（F1）：编译内置
  （managed_components / CMake 内置）/ 独立非 KAP1 SPIFFS 落地 + 分型新齿 / 预渲染位图
  （回 KAP1 通道）；气泡尾巴 KAP1 闭集修订清单在触点产出。
- S0.2 **#11 数据链勘察**：只读追 server 推送 → mcp_server.cc schedule 路由 →
  kidpal_render_node_cache_schedule → kidpal_schedule_page_render 全链，定位「无内容」
  真因（producer 未推 / 路由未达 / 缓存时机），落根因记录。根因在云侧 producer → 本包
  含 server 面（subsystem firmware+server）；根因需改契约才能合理化 → 逃生门。**F6 条件性
  依赖**：若根因在云侧 producer 且修法需 deploy → 本包与批次① 260911-ecs-deploy-convergence
  共享 deploy 面，届时复核 mergeOrder 是否需增挂 dependsOn（不定死，勘察结论触发）。
- S0.3 **五态落点勘察**（还原轨新增，只读）：勘察 PKG-2 迁出后的聊天 layer 内容面
  （SetChatMessage/SetStatus/SetEmotion → layer API 落点、姿态/气泡/键帽条挂载位置）+
  reminder_page.c 提醒弹窗演出落点（d50 压暗、pose-jump、KEY 关闭交互），产出五态演出
  落点清单；PKG-2 尚未合并时本勘察基于其 frozen PROPOSAL §4 + plan.md
  纸面推演，合并后 rebase 时校正。**跨包交点申报**：application.cc 三符号
  （SetChatMessage/SetStatus/SetEmotion）在 PKG-2 与本包 crossEnd counterparts 双登记
  ——所有权拆分 = PKG-2 齿断言 layer 化落点存在（语义 owner），本包齿断言五态内容渲染
  （换皮 owner），两齿断言面不重叠；本包 rebase 在 PKG-2 之后，不构成 launch 冲突。
  **F3 symbol 未钉**：本包五态齿要断言的「layer 内容驱动入口」符号在 PKG-2 合并前不在
  树，五态齿红基线按「机制优先」写、符号名 rebase 时校正；**F4 机制**：reminder_page.c
  现状压暗是 LV_OPA_50（非 d50 网点），换 d50 = 真实观感变更，走活控件平铺合成机制
  （load+pixel→ARGB8888→lv_image，schedule_page.c 已用该机制），非整帧 art seam（设备 no-op）。
- **planner intake 补记（2026-09-12，architect 扩包预审 GREEN-WITH-ITEMS，见
  arch-review-expansion.md §C/§D.5）**：S0.2 的 F6 条件复核条款**已解除**——
  260911-ecs-deploy-convergence 已 closed 并合并（merge 16a47bd），早于本包 base 83a6e66；
  若根因在云侧 producer，本包按 notes 既定申报扩 server 面（subsystem firmware+server）即可，
  无需再复核 dependsOn/mergeOrder。S0.3 补充：PKG-2 合并带入七条结构不变式（arch-review-
  expansion.md §B.2，五态/换皮代码必须布防；chat_top[] 数组 = chrome 成员边界、允许内容级
  同步），五态落点清单由 intake 勘察产出后落包 evidence。

## 阶段 1 · #12 P2-A1 齿修复（acceptance-author，先行——它红着会污染全批验证信号）

- S1.1 acceptance-author 修 `firmware/main/test/schedule_p2.acceptance.c` 齿 6 的
  first-match 歧义：改为定位**定义文件**（含 `int kidpal_schedule_page_render(` 定义签名、
  或 tooth-1 已定位的含 `kidpal_schedule_parse(` 的模块文件）再断言 lv_anim|弹跳；或遍历
  全部命中文件要求至少其一含标记。**tightly-only**：齿语义不变、不放松任何断言；
  REVISION LOG 留痕（first-match 歧义 + render_node.cc 先行命中事实）。可选顺检（不强制）：
  齿 5 同型 first-match（读首命中文件查降级标记）当前因 render_node.cc 含 "-1" 字面量
  碰巧绿，可一并 tighten 为查定义文件。
- S1.2 **禁止方案 A**：不得在 kidpal_render_node.cc 补 lv_anim/弹跳 标记让红消失（喂哨兵）。
- S1.3 验证：host runner P2-A1 转绿，schedule_p2 其余齿不绊倒。

## 阶段 2 · #9 故事页排版（builder → verifier）

- S2.1 `story_page.h`：新增 `KSTORY_READER_LINES_PER_SCREEN` 常量（按真实 14px 行高与
  400x300 版面算，替代 mcp_server.cc:731 hard-code 4 的「最小估算」）——分页行高单源。
- S2.2 `story_page.c` READER 分支：正文按 `v->screen * 每屏行数` 切片只画当前屏
  （指针/偏移进 story->text，遵守栈纪律；按 '\n' 行计与 open_reader 冻结性质一致）；
  标题/正文标签设宽度（如 LV_HOR_RES-32）+ `lv_label_set_long_mode(LV_LABEL_LONG_WRAP)`
  消横向裁切。屏位指示条与「第 X 屏 · 共 N 屏」label 联动不动（冻结面）。
- S2.3 `mcp_server.cc` story_text dispatch（~731 行）：open_reader 的 screen_lines 改消费
  S2.1 单源常量（消除分页行高漂移；ack 的 screens 字段随之对齐真实分页）。
- S2.4 验证（verifier）：host runner story_p3 8 齿全绿 + story_page_stack 3 齿绿。
  **F2**：齿 ④「屏 1 vs 屏 2 帧差异」由屏位条产生、不锁正文切片——冻结头注 ⑤「当前屏
  正文」需 acceptance-author 新产一枚机制齿（进 story_p3 tighten-only 或
  v3_restore.acceptance.c），断言正文按 v->screen 切片只画当前屏（非仅屏位条变化），
  补进铁律 #5 修订白名单。**F5**：渲染切片缓冲另补一枚尺寸/栈齿（story_page_stack 现
  只锁解析链 char text_buf[2048]）。

## 阶段 3 · #11 课表页（builder → verifier）

- S3.1 按 S0.2 根因修数据链（固件缓存/接线或 server producer 面，以勘察结论为准）。
- S3.2 字体占位按 S0.1 决策落地（正文/列文本设选定字体；完整 V3 字体体系在阶段 4 统一
  落位，此处只保证课表页先接上选定正文字体）。
- S3.3 验证：host runner schedule_p2 全齿绿（P2-A1 已于阶段 1 转绿、P2-A3 render 帧齿）+
  真机课表页见真实条目（阶段 8）。

## 阶段 4 · V3 字体体系落地（builder → verifier，并包裁定后为换皮前置）

- S4.1 按 S0.1 决策接正文字体（BUILTIN_TEXT_FONT 切换 / cbin lv_font_load 到正文标签 /
  U 管线新产 bin——新资产走 U 闭集修订）。缺字策略落地（fallback 或扩子集）。
- S4.2 标题（站酷快乐体）/ 数字（Baloo 2）资产 U 管线新产（tools/asset-pipeline 扩展），
  经 U 闭集修订入集；V3 字号阶梯落位（标题 30 / 数字按页）。
- S4.3 验证：缺字回归证据（「猬」等抽样字渲染可见，截图/真帧）；ui_art_integration
  闭集齿修订后复跑绿。

## 阶段 5 · P2/P3/P4 页面换皮（builder → verifier，还原轨主体一）

> **F4 机制注**：冻结 art 缝 kidpal_art_draw_nine_slice/draw_dots 是整帧 seam（设备 no-op），
> 活页面（课表/故事/提醒）的框族 9 宫格与网点平铺走「load+pixel→ARGB8888→lv_image」
> 合成机制（schedule_page.c 已用同款），不在整帧里烘。

- S5.1 坐标转写：从 kidpal-ui-design-v3.html P2/P3/P4 画布抽坐标生成布局表（管线脚本
  扩展，禁手抄）——**F9**：转写脚本本身由 RP-A8 章承接（tools/asset-pipeline 扩展，
  不另挂 RP）。
- S5.2 P2 课表页换皮：frm/frm-sm 框 + 网点弱底纹 + 波浪线点缀 + 站酷标题「今天的课」+
  Baloo 时间数字 + 两态圆点（空心未上/实心待上，反白行已废弃以 spec S-F1.2 为准）。
- S5.3 P3 故事页换皮：故事果计数器（ic-apple）+ 选中条目反白微倾 + 手绘胖虚线分隔 +
  站酷标题「我的故事」+ voice-chip 语音入口；**录音子状态与盖章成帧一并换皮**（冻结逻辑
  已在 story_page.c：KSTORY_PAGE_RECORDING / kidpal_story_rec_tick / 盖章几何冻结值——
  观感层替换为 pose-record 举话筒 + REC 反白点 + 手绘声波 + stamp_seal_r8 盖章帧，
  冻结语义/数值码零改动）。
- S5.4 涂鸦纪律执行：星/云/波浪每屏 ≤2 处、只在角落留白处；网点四类用途白名单。
- S5.5 验证：host runner 既有冻结齿复跑绿（story_p3 8 / schedule_p2 / 路径敏感齿若红走
  acceptance-author 留痕修订）；换皮新齿（RP-A11 acceptance-author 产）红基线转绿。

## 阶段 6 · P4 笔顺页观感还原（builder → verifier）

- S6.1 米字格三态笔道 + 握笔小手（ic_penhand 已在闭集）+ 进度珠（九颗 dot-mini）+ 右栏
  速查卡（拼音 Baloo/部首/组词可爱体）；示范字规范字形红线（char_stroke 冻结语义不动，
  观感层还原）。
- S6.2 B8 打包策略：笔形 3 态叠加复用 + 米字格单份背景，禁逐笔逐态整帧烘烤
  （~3.1MB 预算红线，ui_art_integration B8 闭集齿守门）。
- S6.3 验证：char_stroke 冻结齿复跑绿 + 换皮新齿绿 + 真机证据（阶段 8）。

## 阶段 7 · 五态演出换皮（builder → verifier，还原轨主体二；依赖 PKG-2 已合并）

- S7.1 按 S0.3 落点清单，聊天 layer 内容替换：五态姿态 + V3 气泡（frm-sm + 斜切尾巴
  ——尾巴资产走 U 闭集修订）+ 播报声波条 + 提醒弹窗压暗（用既有 d50 网点资产铺底，
  reminder_page.c 落点勘察进 S0.3）+ 低电 zZZ；SetChatMessage/
  SetStatus/SetEmotion 落点改写为五态演出驱动（layer 底座与优先级仲裁零触碰）。
- S7.2 底部键帽条任何状态常驻（V3 §06 硬要求：「任何状态下底部提示条永不消失」）——
  键帽条随五态文字联动（KEY 说完啦/不等了/打断/知道了/叫醒它 + BOOT 翻页照旧）。
- S7.3 验证：host 齿覆盖五态分支 + 真机语音轮并发场景（阶段 8）；PKG-1 路由语义回归
  （KEY 单击=对话/长按=回首页/BOOT 翻页）复验。

## 阶段 8 · 真机整屏 V3 对照轮 + 收尾（human 持板）

- S8.1 真机轮：四页 + 语音轮五态逐一对照 V3 设计稿验收——**F7 边界**：P1 走 V3 复核
  （9b 批已换皮，本包阶段 5 不重做 P1；P1 CJK 缺字属殿后清单不随 S4 字体体系覆盖）；
  P2/P3/P4 换皮验收 + 故事正文换行完整可见（含「猬」）+ BOOT 长按翻页逐屏正文 +
  课表页真实内容 + HWV-A7 16px CJK 可读性顺带核验；串口/照片证据落
  `changes/dev/260911-render-pages/`。
- S8.2 **V3 §01 两句检验题实测**（本包还原轨的收口验收）：把设备递给孩子问「你觉得这是
  什么？」期望「小伙伴/玩具」非「闹钟」；再问「它哪里可爱？」答不出 = 改版失败回炉。
- S8.3 host runner 终跑全绿（含全量既有齿无回归）；integrator closeout + pod-retro +
  supervisor-verdict-gate + pre-merge-escalation-gate + merge-driver（禁直推 main /
  禁 remote push）。

## 验收 / exitCriteria

1. host runner 全绿：story_p3 8 齿、schedule_p2 全齿（P2-A1 转绿）、story_page_stack
   3 齿、char_stroke 齿、ui_art_integration 闭集齿（U 闭集修订后）；runner exit 0。
2. 字体决策记录在案（选型 + 「猬」覆盖实测证据 + 标题/数字字体生产方案 + U 闭集修订
   清单与留痕）。
3. 真机整屏对照证据：四页 + 五态逐一 V3 对照 + 正文换行不裁切、翻页逐屏、课表真实
   内容 + 两句检验题实测结果（human 持板轮日志）。
4. 冻结面零越界：docs/contracts/**、managed_components/** git 零触碰；*.acceptance.*
   仅 acceptance-author 留痕修订（P2-A1 齿 + U 闭集扩容 + 路径敏感齿必要修订），
   REVISION LOG 齐。
5. 栈纪律：story_page_stack.test.c 3 齿绿，story 链无新增大栈缓冲。
6. V3 红线清单核对在案（§07 红线逐条核对记录 + §01 检验题结果落 evidence）。

## human 触点

- 触点①：确认/改判本裁决（② 单包 Full / riskTier=medium）——已确认（2026-09-11，含
  #12 方案 B 修法与 ② 先 ③ 后接续顺序一并确认）；2026-09-11 扩包裁定（对话内三问）：
  扩③ scope / 整屏一次还原（页面换皮+P4+五态进包）/ 字体生产并包（触发器条款废除），
  riskTier 复判 high——**扩包裁定已确认，本 plan 骨架据此重写**。
- 触点②：S0.1 字体方案决策拍板（正文选型 + 缺字处置 + 标题/数字生产方案与 U 闭集修订
  清单——并包裁定下不再裁「是否拆出」）。
- 触点③：阶段 8 真机整屏对照轮持板；acceptance-author 修订若被判定触语义 → 升级裁定。
