---
changeId: 260911-render-pages
rigor: full
reviewed-by: architect-reviewer
date: 2026-09-11
verdict: GREEN-WITH-ITEMS
contractGate: NONE（#11 数据链为条件性契约风险，plan 逃生门已正确处理）
---

# architect review · 批次③扩包 plan（advisory）

裁断：**GREEN-WITH-ITEMS**——架构成立可放行，2 high + 5 medium 须在 intake 前闭合；
不触 CONTRACT_GATE_REQUIRED。Findings 9 条（F1/F2 high，F3–F7 medium，F8/F9 low）。

**注**：本文件由评审 agent 以只读工具产出判据、因 scope 钩子（universal.bashDenyWrite
`.claude/taskpackages/**`）无法落盘，判据全文经 main thread 转写落此；评审 agent 对被审
对象（plan.md / taskpackage.json）零写入。

## 最重三条

1. **F1（high）字体 bin 被错并入 KAP1 白名单闭集通道**：plan.md 铁律 #4 与 taskpackage.json
   RP-A4 把字体 bin 归入 U 闭集修订，但字体是 lv_font_conv 格式非 KAP1，会同时打破
   ui_art_integration.acceptance.c test 3（KAP1 load）与 test 4（闭集枚举 foreign）——
   非「根本不触 B8」，需分道路径：(a) 编译内置 / (b) 独立非 KAP1 落地 + 分型齿 /
   (c) 预渲染位图。
2. **F2（high）#9 切片修复无齿锁**：story_p3.acceptance.c 齿 ④「屏 1 vs 屏 2 帧差异」由
   屏位条产生、与正文是否切片无关，故「story_p3 8 齿全绿」对「当前屏正文」（冻结头注 ⑤）
   无兑现——正是 ARQ-260911-tooth-strength-below-contract 记录的模式。
3. **F3（medium-high）五态跨包对端 symbol 面未钉**：所有权拆分（PKG-2=layer 拓扑 / 本包=
   内容）成立且已写入 PKG-2 SU-E2，但本包与 PKG-2 双登记的都是旧符号 SetChatMessage/
   SetStatus/SetEmotion；本包五态齿要断言的「layer 内容驱动入口」符号在 PKG-2 合并前
   不存在于树，acceptance-author 无法对其产齿。

## 全文 findings

- **F1（high）字体格式冲突**：气泡尾巴=KAP1，字体=lv_font_conv；落 spiffs_root 必触发 B8
  test 3（load 失败转红）+ test 4（foreign 转红）。S0.1 触点②须逐字钉每字体走
  (a)/(b)/(c) 哪路；气泡尾巴是唯一确定新增 KAP1。
- **F2（high）#9 切片无齿**：需 acceptance-author 新产「当前屏正文」机制齿（进 story_p3
  tighten-only 或 v3_restore.acceptance.c），并补进铁律 #5 修订白名单。
- **F3（medium-high）五态 symbol 未钉**：RP-A9 / v3_restore 五态齿红基线按「机制优先」写，
  符号名留到 PKG-2 合并后 rebase 校正。
- **F4（medium）换皮 live-widget 机制**：冻结 kidpal_art_draw_nine_slice/draw_dots 是整帧
  seam（设备 no-op），活页面需「load+pixel→ARGB8888→lv_image」的 9-slice/网点平铺合成；
  提醒弹窗现状是 reminder_page.c 的 LV_OPA_50 非 d50，换 d50 是真实观感变更需活控件机制。
- **F5（medium）切片缓冲 + additive 口径**：story_page_stack.test.c 只锁解析链
  char text_buf[2048]，渲染切片缓冲无锁，建议补一枚齿；铁律 #3「view 增 screen_lines 字段」
  是误导（它是参数非字段）。
- **F6（medium）server 面隐式依赖**：S0.2 若定出云侧 producer 修法，本包与批次①
  260911-ecs-deploy-convergence 共享 deploy 面，但 dependsOn 未声明、mergeOrder 21 未复核。
- **F7（medium）P1 缺字**：S8.1「四页整屏 V3」含 P1，但阶段 5 不换 P1，且 P1 CJK 是殿后
  清单（home_p1.c 多处）；S4 字体体系不覆盖 P1。
- **F8（low-medium）预算错位**：iteration 3 / fix 4 与较小 PKG-2 相同，扩包后 8 阶段规模
  翻倍；intake 复核。
- **F9（low）动效/坐标转写载体**：五态新增动效未入 V3 §07 白名单核对；S5.1「禁手抄」坐标
  转写管线脚本无 RP 映射承接。

## 关键源码/契约锚（供落地引用）

- plan.md：:44-47 还原轨 / :71-81 铁律 7 条 / :85-103 S0.1-S0.3 / :119-128 S2 /
  :171-180 S7 / :184-186 S8.1
- taskpackage.json：:31 RP-A4 / :73-79 budget·dependsOn·mergeOrder / :135-146 crossEnd
- ui_art_integration.acceptance.c：:663-718 artt_expected 47 枚 / :1040-1145 test 3 /
  :1205-1250 test 4
- story_p3.acceptance.c：:257-258 头注 ⑤ / :1846-1865 齿 ④
- story_page_stack.test.c：:1-4 非冻结 / :203-204 禁词齿
- story_page.c：:1062-1068 / :1202 / :1205-1208 / :612-624
- reminder_page.c：:1143-1144
- home_p1.c：:651-653 / :703-705 / :714 / :733

未发现需改 docs/contracts/** / docs/spec.md 的已定案缺口——不触发 CONTRACT_GATE_REQUIRED。

## 处置记录（2026-09-11，human 裁「折回 high + medium 一并补」）

9 条 findings 全部折回包定义（plan.md / taskpackage.json），无一 defer 到 intake 之外：

- F1 字体分型 → 铁律 #4 重写为「资产落点分型」+ RP-A4 + S0.1 ③ 三分道路径逐字钉。
- F2 #9 切片无齿 → 铁律 #5 + RP-A2 + S2.4 增「当前屏正文」机制齿 + 切片缓冲齿（并入 F5）。
- F3 五态 symbol 未钉 → S0.3 + RP-A9 + crossEnd note 记「机制优先、rebase 校正」。
- F4 换皮 live-widget 机制 → 阶段 5 机制注 + S0.3 提醒压暗 LV_OPA_50→d50 需活控件平铺。
- F5 切片缓冲 + 字段笔误 → 铁律 #3「字段」改「参数」+ 阶段 2/S2.4 补切片缓冲齿。
- F6 server 面条件依赖 → S0.2 增「条件性依赖，勘察结论触发复核 mergeOrder/dependsOn」。
- F7 P1 缺字 → S8.1 划清「P1 复核非换皮、P1 CJK 殿后清单不随 S4 覆盖」。
- F8 budget → iterationBudget 3→4、fixBudget 4→5。
- F9 动效/坐标转写载体 → 铁律 #6 增五态动效白名单核对 + S5.1 转写脚本承接 RP-A8。

裁断维持 GREEN-WITH-ITEMS：放行条件由「intake 前闭合」转为「已闭合」，planner intake 仍应带本文件复读 F1/F2 两个 high 的最终落点。