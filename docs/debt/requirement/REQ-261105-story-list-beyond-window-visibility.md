---
id: REQ-261105-story-list-beyond-window-visibility
title: story_list 16 条窗口外旧篇设备不可见（total 计入但 items 窗口只送最新 ≤16）——「全量可及」是否延伸到故事列表待 human 裁
domain: requirement
status: open
severity: SHOULD
owner: /requirement-change loop（若裁要做：S-F2.1 补充规则 + 契约 §1.3③ 同批修订；设备端分页形态可参照积分详细账本 keyset 先例）
trigger: human 裁定「故事列表也要全量可及」时立项；或下一次需求面变更批顺带裁（本条目只记诉求与现状口径，不阻塞任何在途包）
opened: 2026-11-05
closed: ""
closedBy: ""
paths:
  - docs/requirements.md
  - docs/contracts/interface-contract.md
source: "2026-11-05 真机轮（261104-k0a-four-page-skeleton K0A-E3 验收轮）human 发现：设备 ×19 计数但列表只 4 屏 16 条，问「再早的怎么查看，记得设计是翻屏去远端拉」——planner 数据链排查后按 human 裁「记 debt 后收口」登记"
handoff: 现状口径 = 契约 §1.3③ Q2 丙案（2026-09-24 契约门）：items = 最新 ≤16 条显示窗口、total = 全集计数，二者独立——窗口外旧篇（本例 19−16=3 篇）云端仍在、设备按设计不可见。「翻屏远端拉取」的既有设计落在积分详细账本（S-F6.2「全量可及」+ 契约 §1.4 `kidpal_points_page` keyset 分页，2026-11-04 批新增，K2 承接；契约明写「不继承 story_list 的 16 条窗口」）。若 human 裁故事列表也要全量可及 = 需求变更（S-F2.1）+ 契约变更（§1.3③ 窗口口径 / 新增分页消息）同批走 /requirement-change + /arch-change
---

## 诉求本体（human 原话，2026-11-05）

「那再早的怎么查看的，我记得当时设计是翻屏的时候去远端拉没有的呀」——孩子（家长）预期：星星计数 ×19 代表 19 篇都在册，列表理应能翻到全部；现状只到最新 16 条。

## 现状口径（排查结论，2026-11-05 实证）

- 云端 producer（`server/gateway/story.py::list_stories`）= total 19 / items 16，ECS 在线代码实跑对账一致——**数据链无缺陷，是契约口径**。
- 设备端解析全有或全无（`kidpal_story_list_parse` 无静默丢行）；`KSTORY_LIST_MAX=16` 与契约窗口同值。
- 设备列表无任何「拉更早」的消息通道（`story_list` 推送三时点 = 建连恢复 / 归档后 / 删除后，全为整窗重推）。

## 裁定所需背景

- 同类「全量可及」需求语义已在本列车为积分详细账本立过（S-F6.2 AC1「任意历史分录都能翻到」），契约配套 = keyset 分页（`kidpal_points_page`，游标 + revision + has_more）；story_list 若跟进，分页形状可直接参照该先例（契约自己留了句「不继承 story_list 的 16 条窗口」——反向即：story_list 窗口是历史选择，非终局裁定）。
- 需求面 S-F2.1 现行 AC 无「全量可及」字样（v1.28 只钉屏指示与跳屏）——所以这不是实现缺口，是需求语义是否延伸的问题。
