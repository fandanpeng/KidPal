---
changeId: 260911-screen-unification
rigor: full
riskTier: high
sourceType: proposal
gateProfile: [host-teeth, state-machine, device-round, frozen-acceptance-revision]
reviewed-by: 设计权威 = PROPOSAL FROZEN v1.0（human 触点② grilling OQ1=纯B / OQ3=c 已裁，pin sha1:cd10d1903ac6bb1a / git blob 26138dba）+ review.md F6/F7/F8；本 plan 由 portfolio-planner 依 ARQ-260911-split-package-implementation-prose-gap 起草，planner intake 只读消费、改需留痕
date: 2026-09-11
---

# PKG-2 归一屏（核心包）· 包级实施方案

> 承载 PROPOSAL §2.1 裁定（OQ1=纯 B 归一屏）+ §2.3（OQ3=c 驻留随 B 自然成立）+
> review F6（波及面全额生效）/ F7（char 并屏）/ F8（alert/reminder 优先级）。
> 逃生门：执行中任何 delta 的合理化需要改 docs/contracts/**、docs/spec.md、或放松任何
> 冻结验收齿断言 → 停，转 /arch-change + architect-reviewer 四分类，本包不批事实源变更。

## 1. 根因（PROPOSAL §1.1，地面真值）

三块 LVGL 屏并存——s_chat_scr（小智 SetupUI 建）/ s_kidpal_scr（KidPal 四页）/
char 独立屏——前台归属靠三个分散标志（`s_page_pinned` / `s_chat_overlay` /
`s_current_page`，kidpal_render_node.cc L58/L63/L75）+ 1s tick 轮询对账，竞态窗口
1s。这是当日三个真机崩溃（SetStatus 悬空、clean 聊天屏、栈溢出同族）的结构性土壤：
任何一条写屏路径都可能与 tick 对账竞争，标志三处分散使「谁在前台」没有单一权威。

## 2. 改法（已裁：纯 B，不经 A 中间站）

把小智聊天控件（状态栏/表情/气泡）迁为 KidPal 屏（s_kidpal_scr）上的一个常驻
layer；聊天内容任何页可见；三标志 + tick 对账整体退役；**layer 管理本身就是前台
owner 的单一权威（仲裁器显式化——原方向 A 的状态机内核收编于 layer 栈的优先级
管理，不另建状态机对象）**。

### 2.1 阶段划分

- **阶段 0 · 勘察（只读）**：盘点全部写屏路径（SetupUI 建立面 + SetChatMessage /
  SetStatus / SetEmotion / Alert / 配网页 + render_node 各 clean 路径 + char 独立屏），
  产 host 矩阵清单（路径 × 触发源 × 现行标志交互），落包 evidence。F6 波及面以
  此清单为验收基线。
- **阶段 1 · 聊天 layer 化**：状态栏/表情/气泡控件从 SetupUI 建立路径迁到
  s_kidpal_scr 的常驻 layer（z 序：聊天 layer 与页面 layer 叠放，页面切换不清聊天）；
  SetChatMessage/SetStatus/SetEmotion 全部落点改写 layer API；s_chat_scr 独立屏退役。
- **阶段 2 · 三标志/tick 退役**：s_page_pinned / s_chat_overlay / s_current_page 与
  1s tick 轮询对账路径移除（机制齿：符号级零残留断言）；前台归属改 layer 栈管理——
  语音轮页 / 服务端推送页 / 本地浏览页 / 提醒弹窗 / 配网按优先级表进栈出栈。
- **阶段 3 · char 并屏**：char 独立屏并入 s_kidpal_scr 体系（review F7，
  render_node.cc L399-416 自证的同族结构），char 展示作为一页/一层。
- **阶段 4 · alert/reminder 优先级 + 驻留副产品**：提醒弹窗进 layer 优先级表
  （review F8——同族崩溃已修症状未修架构，此处修架构）；回答驻留随聊天 layer 常驻
  自然成立（OQ3=c）——**清除时机 = 下次交互（新语音轮 / 任意按键）由 layer 管理
  顺带定，不新建驻留页/定时器**。
- **阶段 5 · host 矩阵回归 + 真机轮**：全量既有冻结齿复跑（见铁律 1）+ human 持板
  真机：语音轮/推送页/提醒并发场景 0 复位 0 崩溃 + 两键行为与裁定路线一致 + 回答
  落点确定。

### 2.2 host 矩阵回归（F6 全额生效）

阶段 1/2/3 每阶段末跑全量：story_p3 8 齿 / schedule_p2 全齿 / home_p1 / char_stroke /
session_logic / ui_art_integration 闭集 / story_page_stack 3 齿。渲染类齿对
kidpal_render_node.cc 路径敏感——重构后红的齿由 acceptance-author 留痕修订（tighten
/路径重指），断言语义不放松。

## 3. 铁律约束（全阶段有效）

1. **冻结齿保绿/留痕修订**：builder/fixer 不得改 `*.acceptance.*`；渲染路径重构致
   齿红 → 升 acceptance-author，REVISION LOG 留痕（如「齿 6 first-match 定位歧义」
   同族——定位口径改、断言语义不改）。
2. **栈纪律**：story 链 4096B 栈约束（story_page_stack 3 齿）与既有固件内存纪律
   全程有效；layer 化不新增大栈缓冲。
3. **观感边界**：字体子集/换肤/排版零触碰（批次③ 260911-render-pages，mergeOrder
   21 在本包后）；本包只动结构与归属，不动观感。
4. **冻结面 git 零触碰**：docs/contracts/**、managed_components/** 原样。
5. **spec 零修订**（SPEC-1 不触发）；OQ3=c 下无独立会话语义变更，若实现中发现与
   spec §5.3 表述冲突 → 逃生门。
6. **两键行为保回归**：PKG-1（260911-key-route-a1，mergeOrder 18 先并）定的路由
   语义（KEY 单击=对话、长按=回首页、BOOT 单击=翻页）在本包重构后必须原样成立
   （SU-A7 显式验收）——归一屏不改变按键语义，只改显示归属。

## 4. 验证 / exitCriteria 对账

- 阶段 5 host 全绿 exit 0 + 真机证据（日志/照片/复位计数）落
  `changes/dev/260911-screen-unification/`。
- 驻留与清除时机记录在案（无新页/定时器的裁定兑现证据）。
- workstream 级收口（§5 五条）中本包承载：屏幕落点确定（非随机）、崩溃同族
  0 复位 0 崩溃、两键行为一致——真机收口轮由末位包 260911-display-first-m1 统一对账。

## 5. human 触点

- 触点①：PROPOSAL 立项 ack + OQ1/OQ3 裁决（已完成，2026-09-11 触点②）。
- 触点②：阶段 5 真机轮持板；acceptance-author 修订若被判触语义 → 升级裁定。
