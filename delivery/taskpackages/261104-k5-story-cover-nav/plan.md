---
changeId: 261104-k5-story-cover-nav
title: 故事封皮导航——P4 封皮、列表/阅读下沉、逐层返回与 story_list 共享帧消费
rigor: full
riskTier: high
sourceType: proposal
origin:
  ref: delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md
  workstream: 261103-star-points-story-cover
reviewed-by: architect（/triage-change 分流 review 2026-11-03；门 1 待主线程另派）
date: 2026-11-04
gateProfile: ["evidence-integrity", "test-quality", "state-machine", "mcp-tool-contract"]
dependsOn: ["261104-k2-points-device-ui", "261104-k4-reward-goal-config"]
mergeOrder: 76
---

# 背景摘录（新建型锚「现状与差距」，PROPOSAL §2）

- K0a 已把故事原地迁到 P4，但那只是四页骨架的过渡态：故事列表仍是平级页，封皮、模块内层级和逐层返回尚未实现。front 与设备也还把 story_list 当成“切到列表”的页面帧，而 landed 契约已把它改成 P4 模块共享数据帧。
- 封皮需要书封、标题、现存星星数和“打开/下一页”键帽。列表和阅读下沉后，KEY 长按必须阅读→列表→封皮→首页逐层返回；语音“打开故事”只到封皮，不能跳过模块入口。
- 本包最后落共享导航面，并承担导航并集收口。TECH-261028 已在 K0a 修复，本包只做回归；PROPOSAL §5 的跨包账本与真实 QQ 联合收口由主线程另组织。

# 可行性判据

- K0a 已提供四页枚举、四点 keybar、枚举驱动语境和阅读键修复；K2 已落定 P3 积分页。K5 可以在稳定基线上集中处理 P4 层级，不必再与积分实现并行抢共享文件。
- ADR-018 已明确 story_list.total 是封皮与列表共同计数源，否决新 story_cover view 和 cover_total。front `_push_story_list` 的六个调用者、display_schema floor 与 OpenClaw 静态纪律均已核实，可按契约逐点对齐。
- V9 story-cover 与既有列表/阅读帧齐全，KAP1 管线在 K4 后串行可用；host/server 可验导航与帧语义，真机可完成四页和故事层级并集。

# 跨包事实与共享冻结面约束（含跨切片边角）

- **K0a→K5**：K0a 的 P4 平级故事是合法过渡，不是最终 spec 完成态。本包不得回退四页枚举或阅读键修复，只在其上新增封皮和子页层级。
- **story_list 只有一份计数**：封皮与列表都信 `total`，后台恢复/归档/删除推送恒 restore:true；只有显式“打开故事”的空 data 薄载荷进入封皮。不得新增 story_cover view、cover_total 或独立计数 API。
- **语境分工**：K0a 建枚举映射，K2 扩 points_ledger，本包扩 story_cover。封皮发起的是普通问答，只有 P4 故事列表产生 `story` 并进入记录语境；路过封皮不得自动开录。
- **相邻冻结锚拆包期预扫非零命中**：story_page_v2 的列表返回、record_session_live 的语境、keybar_three_slot 的页槽、chrome_ghost 的转场，以及 server M4K1/M4K3 的 story_list 推送断言均需保留或收紧。旧实现/测试若与“后台 restore:true”冲突，由 acceptance-author 留痕重锚，不能改契约或静默保留旧行为。
- **K4→K5 资产串行**：K4 已产场景资产，本包在同一管线补 `ic-coin` 与书封；生成器、资产清单和生成件不与 K4 并行修改。

# 铁律约束

- P4 封皮是故事模块唯一主入口；KEY 短按才进列表，BOOT 从封皮 wrap 回首页，KEY 长按逐层返回，每按一次只上一层。列表/阅读的 BOOT 仍用于页内浏览和翻屏，不恢复全局翻页。
- 封皮计数等于现存篇数，数据未达不显计数、确认空账可显 ×0；故事星星与积分余额不得合并。列表、录音、盖章、阅读、删除确认等未变帧沿既有 V8/V9 口径，不借下沉重画。S-F2.1 AC7 纳入回归：正文未留存且当刻取不回时给可见提示，不黑屏、不显示别篇；由 Pod 内 acceptance-author 覆盖。
- 语音“打开故事”只到封皮；OpenClaw 只能发 story_list 空 data 薄载荷，完整列表仍只能由 front 产生。任何后台推送不抢前台。
- 阅读 KEY 短按零响应和正确键帽以 K0a 为基线，不得再落全局 SESSION_TOGGLE；本包不重复承接 TECH-261028，也不吸收 TECH-261022 的独立修复范围。
- 不改 requirements、architecture、`docs/contracts/**`，不新增 story_cover view/cover_total；发现需要新线格式、计数源或全局键位新语义时立即停并升级。

# demo 证据形态（与 roadmap 一致）

- host runner + server pytest：封皮/子页导航、四点与覆盖层、story_list total/restore/薄载荷、story_cover/story 语境及故事回归的 RED→GREEN 和变异证据；S-F2.1 AC7 覆盖正文未留存且取不回时的可见提示与不显示别篇。
- 资产管线与 device build：ic-coin/书封产出与装载，V9 design review；既有故事子态和 K4 场景资产零回归。
- 一轮真机导航并集：BOOT 四页、封皮→列表→阅读、逐层返回、语音只到封皮、记录会话、删除/盖章计数和阅读键；导航之外的跨包联合收口项明确交主线程。
