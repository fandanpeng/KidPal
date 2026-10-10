---
id: TECH-261010-p3-storylist-cold-start-empty
title: P3 星星记录本冷启动空态——story_list 建连即推被 260928 止血摘除后「进页即见列表」语义重裁至今未做，重启后翻 P3 恒空
domain: tech
status: deferred
severity: SHOULD
owner: dev-loop（story_list 冷启动恢复推送批）
trigger: 下个动 story 链路 / P3 页渲染的包；与 REQ-261010-p1-schedule-empty-state 同属「冷启动空态」族，可并批裁定
opened: 2026-10-10
closed: ""
closedBy: ""
paths: ["server/gateway/front/front_app.py", "firmware/main/display/story_page.c", "firmware/main/application.cc"]
source: 2026-10-10 human 真机使用反馈第 5 条（P3 停默认空态、不列已有故事）+ 链路代码与 260928 止血包决策记录核对
---

**定性：真 bug，且是「有案可查的已知取舍到期未赎」**——S-F2.1 AC1「Given **进星星记录本**，When 看列表」在冷启动场景未兑现；260928-story-connect-push-defer 止血时显式挂起重裁（plan.md 改法节原文：「届时『进页即见列表 + RAM 缓存』语义重裁……时机需随 K3 一并设计」），该重裁至今未做，本条即它的残余面显性化。

**证据链**：① 云端有数据：ECS `/opt/kidpal/stories/` 现存 4 篇（2026-09-30 ~ 10-01 归档）。② 下发触发点现状（front_app.py）：story_list 三时点①建连即推已于 260928 摘除（:1013-1019 注释 + FIX-A1 负锚齿冻结「建连零 story_list 帧」）；现仅剩②停录归档后（:573）/ ③删除执行后（:223）/ ①恢复 = ctx(story) 会话建立同拍（:216，固件侧 application.cc:1014-1020 KEY 开会话发 `kidpal_session_ctx` source_page=story 已接通）。③ 板端 RAM 缓存重启即失（决策 24）→ **重启后翻到 P3（BOOT 翻页路径）无任何触发点下发 story_list，恒显默认空态**，直到孩子按 KEY 开记录会话或发生归档/删除——恰为用户报的「没有把已有的 story list 列出来」。

**根因史（为什么①被摘）**：旧固件收到 story_list 帧即无条件渲染 P3 页（story_page 零 CJK 半截页），而设备每会话重开 WS → 按 KEY 出聆听态时被盖。止血正确；但承接设计（按正确语境推、只填缓存不抢前台）一直没做。

**修法（对 P2 已有先例的复刻）**：S-F1.2 已钉同类口径——「冷启动恢复 = WS 建连时云端附带重推周网格（恢复性推送，**只填缓存不抢前台**，在屏时刷新本页）」。本条同款：① 固件核验/补「非 P3 前台收到 story_list 只填缓存不偷前台」（K2/K3 后 render node 是否已具备此路由 = 实现前核项）；② front 恢复建连即推（FIX-A1 负锚齿翻正、随包改验收）；③「进页即见列表 + RAM 缓存」语义与 P3 空态话术（与 REQ-261010-p1-schedule-empty-state 同族）一并定稿。**riskTier: medium**（语义重裁涉及推送时机与前台抢占，须设计与验收双面动）。
