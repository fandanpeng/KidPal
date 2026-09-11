---
changeId: 260911-render-pages
rigor: full
riskTier: medium
sourceType: regression
gateProfile: [host-teeth, device-round, frozen-acceptance-revision]
reviewed-by: architect (triage 2026-09-11, 来源锚: PROGRESS-SUMMARY-260910 §四A/§四D/§七 #9-#12 + review-packet F5/F2)
date: 2026-09-11
---

# 批次③ 渲染观感殿后包（#9 故事页排版 + #10 字体 + #11 课表页 + #12 P2-A1 齿修复）

> 分流裁决：② 单包 Full。改自有实现面（固件渲染层 + 可能 server producer）行为 → 过门；
> 同一渲染层紧耦合簇、单 owning Pod、无迁移删数 → 不拆包；跨 U 包资产闭集边界（#10）+
> 冻结验收件修订（#12 走 acceptance-author）+ 宽保绿面（story_p3 8 齿 / schedule_p2 /
> story_page_stack 3 齿 + 真机观感）→ riskTier=medium → Full。
> 接续顺序锁定（human 2026-09-11 触点①确认）：批次②（显示主权）先、本包后——#11 数据链
> 与 #9 screen_lines 接线与 ② 的 kidpal_render_node.cc / mcp_server.cc 重构面同文件，
> ③ 先行会在 ② 合并时返工。
> 逃生门：执行中任何 delta 的合理化需要改 docs/contracts/**、docs/spec.md、或放松任何冻结
> 验收齿断言 → 停，转 /arch-change + architect-reviewer 四分类，本包不批事实源变更。

## 范围与根因假设（bug 框架）

- **#9 故事页排版**：story_page.c `ksy_label` 建标签不设宽度不开 wrap → 正文（≤2047B）被当
  一行横向裁切；`kidpal_story_page_render` READER 分支画全文不消费 v->screens 分页。冻结
  验收头注 ⑤ 本要求「当前屏正文」→ 本项 = 兑现冻结契约既述行为、关闭 P3-E2 观感殿后项，
  非语义变更。
- **#10 字体**：正文走板级 BUILTIN_TEXT_FONT=font_puhui_basic_14_1（阿里普惠 basic 小子集）
  →「猬」等次常用字缺字形渲染空白；设计字体 font_zcool_30.bin 是 300×52 KAP1 位图样张
  （非可用 CJK 字体），「接正文」需新资产生产——边界归 U 包管线，须先协商。
- **#11 课表页**：真机报「没用课表具体内容 + 非设计字体」——数据链根因未定（server 未推 /
  mcp schedule 路由 → render_node 缓存链断 / 缓存时机），先勘察后修。
- **#12 P2-A1 齿**：schedule_p2.acceptance.c 齿 6 对 `kidpal_schedule_page_render(` first-match
  取首个命中文件要求含 lv_anim|弹跳；先行轮新增消费调用点 kidpal_render_node.cc 恒为首个
  命中且无该标记 → 恒红。扫描器定位歧义，非真实回归（schedule_page.c 实含 lv_anim）。

## 铁律约束（全阶段有效）

1. **冻结齿保绿**：改 story_page.c / schedule_page.c 后必须跑 host runner
   （`wsl -d Ubuntu-24.04 -- bash firmware/main/test/host_app/espidf_wsl.sh build` +
   `./build/host_infra_test.elf`），全部 TEST_CASE 绿、exit 0。齿红 → 升级 acceptance-author，
   builder/fixer 不得改冻结验收件。
2. **栈纪律**（story_page.c 文件头冻结 + story_page_stack.test.c 3 齿）：story 链跑在
   ssl_receive 4096B 栈上，禁止新增大栈缓冲；正文切片用指针/偏移进 story->text（const），
   不复制 2KB 进栈；大对象一律堆分配。
3. **冻结头 additive-only**：story_page.h 冻结声明 verbatim 保留，只允许新增字段/宏
   （如 KSTORY_READER_LINES_PER_SCREEN、view 增 screen_lines 字段），不得改名/删除/改用途。
4. **U 包闭集红线**：firmware/main/assets/spiffs_root/ 落任何新文件即违 ui_art_integration
   .acceptance.c B8 闭集齿（47 枚白名单）→ 新字体资产要么走 U 包闭集修订（acceptance-author
   + 跨包留痕），要么规避（managed_components / 编译内置）。
5. **冻结面 git 零触碰**：docs/contracts/**、managed_components/** 原样；*.acceptance.* 仅限
   本包明示的两处 acceptance-author 留痕修订（P2-A1 齿 6 + 若需 U 闭集）。

## 阶段 0 · 前置决策与勘察（只读，不改东西）

- S0.1 **#10 字体边界协商**（human + U 包 owner 拍板，产出 = 字体决策记录落包 evidence）：
  ① 正文字体资产选型——候选：managed_components/78__xiaozhi-fonts 的
  font_puhui_common_14_1（cbin lv_font_load）或编译内置 font_puhui_14_1（换板级
  BUILTIN_TEXT_FONT）或 U 管线新产字体 bin；② 缺字覆盖策略——实测候选字体对「猬」及
  抽样生字集的 glyph 覆盖（猬 不在最常用 3500 字内，common 子集可能仍缺；不覆盖则定
  fallback 策略或扩子集）；③ 设计字体处置——font_zcool_30.bin 是位图样张非字体，
  「设计字体上正文」需新资产生产，裁定本包做/拆出另立项/降级为标题样张用法。
  **触发器**：若结论 =「需 U 管线新产字体资产且工作量独立」→ 该子项拆出另立项，
  本包先收 #9/#11/#12 + 既有资产缺字解。
- S0.2 **#11 数据链勘察**：只读追 server 推送 → mcp_server.cc schedule 路由 →
  kidpal_render_node_cache_schedule → kidpal_schedule_page_render 全链，定位「无内容」
  真因（producer 未推 / 路由未达 / 缓存时机），落根因记录。根因在云侧 producer → 本包
  含 server 面（subsystem firmware+server）；根因需改契约才能合理化 → 逃生门。

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
- S2.4 验证（verifier）：host runner story_p3 8 齿全绿（重点 render 齿 ① dark>=5 /
  ④ 屏 1 vs 屏 2 帧差异——切片后应更强；⑥ NULL 降级）+ story_page_stack 3 齿绿。

## 阶段 3 · #11 课表页（builder → verifier）

- S3.1 按 S0.2 根因修数据链（固件缓存/接线或 server producer 面，以勘察结论为准）。
- S3.2 字体占位按 S0.1 决策落地（正文/列文本设选定字体；设计字体若裁定仅样张则不动）。
- S3.3 验证：host runner schedule_p2 全齿绿（P2-A1 已于阶段 1 转绿、P2-A3 render 帧齿）+
  真机课表页见真实条目（阶段 5）。

## 阶段 4 · #10 字体落地（builder → verifier，按 S0.1 决策）

- S4.1 按决策接字体（BUILTIN_TEXT_FONT 切换 / cbin lv_font_load 到正文标签 / 新资产
  ——新资产须先过 U 闭集修订）。缺字策略落地（fallback 或扩子集）。
- S4.2 验证：缺字回归证据（「猬」等抽样字渲染可见，截图/真帧）；若动了 spiffs_root →
  ui_art_integration 闭集齿复跑绿。

## 阶段 5 · 真机观感轮 + 收尾（human 持板）

- S5.1 真机轮：故事正文换行完整可见（含「猬」）+ BOOT 长按翻页逐屏正文 +
  课表页真实内容 + HWV-A7 16px CJK 可读性顺带核验；串口/照片证据落
  `changes/dev/260911-render-pages/`。
- S5.2 host runner 终跑全绿（含全量既有齿无回归）；integrator closeout + pod-retro +
  supervisor-verdict-gate + pre-merge-escalation-gate + merge-driver（禁直推 main /
  禁 remote push）。

## 验收 / exitCriteria

1. host runner 全绿：story_p3 8 齿、schedule_p2 全齿（P2-A1 转绿）、story_page_stack 3 齿、
   动到资产则 ui_art_integration 闭集齿；runner exit 0。
2. 字体决策记录在案（选型 + 「猬」覆盖实测证据 + 设计字体处置 + U 包边界留痕）。
3. 真机观感证据：正文换行不裁切、翻页逐屏、课表真实内容（human 持板轮日志）。
4. 冻结面零越界：docs/contracts/**、managed_components/** git 零触碰；*.acceptance.*
   仅 acceptance-author 两处留痕修订（P2-A1 齿 + 若需 U 闭集），REVISION LOG 齐。
5. 栈纪律：story_page_stack.test.c 3 齿绿，story 链无新增大栈缓冲。

## human 触点

- 触点①：确认/改判本裁决（② 单包 Full / riskTier=medium）——已确认（2026-09-11，含
  #12 方案 B 修法与 ② 先 ③ 后接续顺序一并确认）。
- 触点②：S0.1 字体决策拍板（含缺字处置 + 设计字体去留 + 是否拆出资产生产子项）。
- 触点③：阶段 5 真机观感轮持板；acceptance-author 修订若被判定触语义 → 升级裁定。
