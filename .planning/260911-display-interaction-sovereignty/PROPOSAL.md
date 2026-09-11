---
id: 260911-display-interaction-sovereignty
title: 设备显示与交互主权包（三屏主权收口 + 显示先行 + 回答驻留 + 键位路线）
date: 2026-09-11
tier: 3            # /triage-change 四档：③大 feature 拆包
sourceType: proposal
riskTier: high     # 触 UI 状态机轴 + 真机崩溃结构性面；单向守卫：实现包禁 Lite
freezeState: pending-human
reviewed-by: architect（/triage-change 批次②分流 review，2026-09-11）
sourceRefs:
  - changes/dev/260829-hardware-validation/VOICE-ROUND-FLOW-260910.md
  - changes/dev/260829-hardware-validation/PROGRESS-SUMMARY-260910.md §五/§七.2
  - docs/notes/board-hardware-esp32-s3-rlcd-4.2.md §一
---

# PROPOSAL · 设备显示与交互主权包

> 状态：DRAFT v0.1 · 2026-09-11 · 设计先行稿（方案空间点到、不下最终设计结论，
> human grilling 裁定取舍点后由 portfolio-planner 拆实现包）。
> 承载：PROGRESS-SUMMARY §七.1 遗留 #4/#5/#6/#7 四项。
> review 轨：本件是 dev workstream 设计权威（.planning 通道，/triage-change ③档），
> 非治理/spec 变更；触 spec/契约的子项见 §7 申报，各走各的门。

## 0. 想实现的效果与差距

孩子拿起设备问一句话，**听得见的答案、看得见的落点、按得动的按键**三者应该是
一个连贯的体验：问完先看到（或同步看到）回答/相关页面，回答结束后屏幕停在能看
的东西上，两个键各司其职、行为可预期。现状差距：答案文字写在被服务端页面盖住的
聊天屏上（听得见看不见）；屏幕换页比声音晚 1–3s；回答结束后「停在上一页不动」与
「回首页吞掉答案」两种结局随机出现；spec 设计的两键分工只落地了 BOOT 一键过渡
方案，KEY 键及其页内交互全部悬空。

**非目标**（防 scope creep）：字体子集补字、V3 设计语言换肤、故事页排版、课表
数据填充——归批次③渲染观感；首音时延四段优化（#8）——归时延实验线；故事归档
双断链（#3）——归批次④；ECS overlay 漂移（#1）——归批次①（仅作 PKG-4 前置）。

## 1. 问题与根因（地面真值，证据锚 VOICE-ROUND-FLOW §四/§五）

1. **三屏主权混乱（#4，架构根因）**：s_chat_scr（小智 SetupUI）/ s_kidpal_scr
   （KidPal 四页）/ char 独立屏三块 LVGL 屏并存，前台归属靠三个分散标志
   （s_page_pinned / s_chat_overlay / s_current_page）+ 1s tick 轮询对账，竞态窗口
   1s。当日三个真机崩溃（SetStatus 悬空、clean 聊天屏、栈溢出同族）的结构性土壤。
2. **显示滞后（#5）**：引擎补丁 DisplayFenceStateMachine 在 LLM 全流 finish() 才发
   kidpal_display_blocks 侧通道（connection.py:1442-1456），TTS 已开讲 1–3s 后屏幕
   才换页。
3. **轮间驻留反直觉（#6）**：auto 轮 tts stop 回 Listening 不回 Idle
   （application.cc:458-466）；有 display 块则 pin 住页面（回答文本从未可见），
   无则回首页（答案一闪而过）。
4. **键位漂移（#7）**：spec §4.0/§5.3 两键分工（KEY 主司对话、BOOT 主司翻页）；
   实现仅 BOOT 一键（单击对话/双击翻页/长按故事翻屏），KEY(GPIO18) 板级未接线，
   key_gesture_parser_feed 设备侧零调用（纯逻辑已冻结全绿、悬空）。
   accept-risk 已锚（pod-events d9e0f1a2，human 2026-09-10 留交互批次定路线）。

## 2. 方案空间（初步技术方案 · 候选点到，最终设计由 grilling 裁定）

### 2.1 三屏主权收口（#4，核心结构题 → OQ1）

- **方向 A · 单一显示仲裁器（保守）**：建一个显式状态机持有「当前前台 owner +
  优先级」（语音轮 / 服务端推送页 / 本地浏览页 / 提醒弹窗 / 配网），事件驱动取代
  1s tick 对账；char 独立屏并入 s_kidpal_scr 体系；alert/reminder 进优先级表。
  波及面：render_node + 各 clean 活动屏的路径收口。两套 UI 体系仍并存（观感割裂
  的皮肤面归批次③）。
- **方向 B · 三屏归一屏（彻底）**：把小智聊天控件（状态栏/表情/气泡）迁为 KidPal
  屏上的一个 layer，聊天内容任何页可见，tick/overlay/pin 标志整体退役。动
  SetupUI 建立面 + SetChatMessage/SetStatus/SetEmotion/Alert/配网全部落点，需 host
  矩阵回归评估；#6「回答驻留」在此方向下天然成立（聊天 layer 与页面 layer 叠放）。
- 倾向（供参考，非结论）：A 是 B 的可行中间站且独立可验收；若 grilling 确认
  「回答文本必须与页面同屏可见」为硬需求，则 B 是终态、A 是它的第一步——两方向
  在「仲裁器显式化 + char 并屏」这一步重合，该步无返工风险。

### 2.2 显示先行（#5，机制候选 → OQ2）

- **M1 · 围栏闭合即发（推荐位）**：引擎补丁把「块闭合」从 finish() 提前到 feed()
  内（状态机已支持块级闭合检测，加闭合回调/发射 API）；front 逐块中继已是现状，
  设备侧「语音中渲染」的仲裁规则由 2.1 裁定。预期 display 页上屏提前 1–3s。
  触面：kidpal_display_strip.py 冻结离线契约（feed/finish 语义）需 API 扩展，
  冻结齿修订走 acceptance-author。
- **M2 · 多块不等全流**：现协议已支持 blocks 列表多块，M1 是其推论，不单列。
- **M3 · prompt 改块前置（禁止在 dev-loop 内自行采用）**：与契约 §1.1「在回复
  文本末尾嵌入」冲突，若要走必须先过 architect-reviewer 四分类 + human 契约门。
- 初判：M1 不改 E1 契约设备面（view schema / mcp 信封 / 白名单不变，时序契约
  未钉板）；PKG-4 开工前 reviewer-first 复核一次（见 §7）。

### 2.3 回答驻留（#6 → OQ3）

候选：语音轮结束（Listening 超时或显式结束）后保留 last-answer 可见落点——
(a) last-answer 页驻留至下一次交互（任意键/新语音轮清除）；(b) text 内容驻留 N 秒
后回首页；(c) 方向 B 下聊天 layer 与页面 layer 叠放，驻留自然成立。与 2.1 方向
强耦合，grilling 一并裁。注意与 spec §5.3「说完自动/手动结束回首页」的表述对账：
定案若改「轮 vs 会话」边界语义，spec 修订走 Track B（§7）。

### 2.4 键位路线（#7 → OQ4，human 已锚的取舍点）

- **路线 A · 补接 KEY 落地 spec 两键分工**：板级 GPIO18 + iot_button 三手势注册
  （消抖参数复用现有 CONFIG_BUTTON_SHORT_PRESS_TIME_MS=180）→ 喂已冻结的
  key_gesture_parser → 全局路由表落地；BOOT 回归「主司翻页」（单击翻页），页内
  特例（P2 问课程 / P3 光标+双击进阅读 / P3 念给我听 / P4 播笔顺 / KEY 长按回
  首页 / 提醒弹窗 KEY 关闭）按 spec 逐项接线。代价：页内特例接线量不小 +
  GPIO18 硬件电平/上拉需真机确认（悬空风险）；收益：spec 零修订、交互面完整、
  儿童盲操作可达（对话键固定位置）。spec 无需改。
- **路线 B · spec 过渡修订承认一键**：维持 BOOT 单击=对话/双击=翻页过渡方案，
  spec §4.0/§5.3 过渡性修订（页内特例重新映射或裁剪）。代价：spec 修订走
  Track B + 交互面缩水 + 双击翻页手感对儿童偏难（180ms 窗口内连按两下）；收益：
  实现量最小、无需碰 GPIO18 硬件不确定性。
- 倾向：A。理由：纯逻辑已冻结全绿、硬件 Demo 已证 KEY 三手势可行、B 省下的
  工作量会以「交互面永久缩水 + spec 漂移常态化」偿还。可分期：A1（KEY 全局
  层：对话/回首页 + BOOT 回归翻页）先行，A2（页内特例）随后。

## 3. human 取舍点（OQ，grilling 逐条裁）

- **OQ1 三屏主权方向**：A 仲裁器 / B 归一屏 / A→B 分步。推荐 A→B 分步（重合步
  无返工风险），理由一句话：先杀竞态土壤再谈归一，每步独立可验收。
- **OQ2 显示先行机制**：M1 围栏闭合即发 / M3 prompt 块前置（后者须契约门）。
  推荐 M1，理由：改动小且不触契约设备面。
- **OQ3 回答驻留形态**：(a) last-answer 页驻留 / (b) 驻留 N 秒回首页 / (c) 随
  OQ1=B 自然成立。与 OQ1 联裁。
- **OQ4 键位路线**：A 补接 KEY（可分 A1/A2 两期）/ B spec 过渡修订。推荐 A。
  （human 2026-09-10 已锚此取舍点归本批。）

## 4. 拆包预判与依赖序（供 portfolio-planner intake）

- **PKG-1 键位路线落地**（若 OQ4=A1）：板级 KEY 接线 + parser 喂入 + 全局路由 +
  BOOT 回归翻页。独立文件面，可与 PKG-2 并行。验证：宿主齿（复用
  session_logic 面板）+ 真机（HWV-A2 顺带）。
- **PKG-2 显示仲裁器 / 归一第一步**（核心包，前置 = OQ1 裁定）：显式前台 owner
  状态机 + tick 对账退役（或降级为看门狗）+ char 并屏 + alert/reminder 优先级。
  验证：host 矩阵回归 + 真机语音轮/推送页/提醒并发场景。
- **PKG-3 回答驻留**（前置 = PKG-2）：last-answer 落点 + Listening/Idle 驻留
  语义。验证：真机观感。
- **PKG-4 显示先行 M1**（前置 = PKG-2 仲裁规则 + **批次① overlay 漂移修复**）：
  strip machine API 扩展 + 冻结齿修订（acceptance-author）+ front 时序复核 +
  设备语音中渲染。部署窗口须走修正后的 apply-xiaozhi-overlay.sh。
- **SPEC-1（并行/前置，Track B spec-lane，不属本 dev workstream）**：OQ4=B 时的
  键位过渡修订；OQ3 定案与 spec §5.3/§4.0 冲突时的会话语义钉板。
- 总序：grilling 裁 OQ →（SPEC-1 如需）→ PKG-2 → PKG-3 → PKG-4；PKG-1 并行。

## 5. 验证面（workstream 级）

每实现包验收齿由各自 acceptance-author 钉；workstream 级收口标准：真机一轮
「单击问课表」中，display 页上屏不晚于 TTS 首段后 0.5s（时延实测用
kidpal-live-smoke-debug 三日志源按 correlationId 对齐）、回答结束后屏幕落点确
定（非随机）、两键行为与裁定路线一致、当日三崩溃同族场景（语音轮+推送页并发、
提醒恰在语音轮到点）0 复位 0 崩溃。

## 6. 留根清单

- 两套设计语言并置的观感割裂（字体/皮肤）→ 批次③；首音时延 #8 → 时延实验线；
  故事归档双断链 #3 → 批次④（F2-1 真机判据前置）。
- GPIO18 硬件电平确认 → PKG-1 真机验收项。

## 7. 事实源 / 契约触面申报（逃生门，本 PROPOSAL 不批以下任何一项）

- spec 面（docs/spec.md §4.0/§5.3）：OQ4=B 或 OQ3 语义钉板触发 → Track B
  spec-lane（/arch-change 判级），human 冻结。
- 契约面（docs/contracts/interface-contract.md）：仅 M3 变体触发 →
  architect-reviewer 四分类 + human 契约门。M1 初判无需改契约，PKG-4 开工前
  reviewer-first 复核。
- 冻结 acceptance 面（server/acceptance/test_g_relay_fix.acceptance.py）：M1 必触
  → dev-loop acceptance-author 权限内修订，不豁免验证。
- 真机操作（持板烧录、GPIO18 电平、观感确认）：human。
