---
changeId: 261104-k2-points-device-ui
title: 设备积分本——points_home/points_ledger 全页、keyset 取页、删除确认、积分语境与语音闭环
rigor: full
riskTier: high
sourceType: proposal
origin:
  ref: delivery/workstreams/261103-star-points-story-cover/PROPOSAL.md
  workstream: 261103-star-points-story-cover
reviewed-by: architect（/triage-change 分流 review 2026-11-03；门 1 待主线程另派）
date: 2026-11-04
gateProfile: ["evidence-integrity", "test-quality", "state-machine", "mcp-tool-contract"]
dependsOn: ["261104-k1-points-ledger-core"]
mergeOrder: 73
---

# 背景摘录（新建型锚「现状与差距」，PROPOSAL §2）

- K0 的 `points_page.c` 只证明设备能消费权威 points_home；正式产品还需要主页四态、进度线、最近三条、完整详细账本、分页缓存、删除确认、积分键位和冷启动恢复。
- 积分语音不是单纯的显示功能：设备要发积分语境和可选 selected_id，front 注入有界摘要，模型只产薄意图，K1 执行后设备按真实回执跳页或拒答。删除则由设备按键发 `kidpal_points_delete`，不能走语音或模型直写。
- 本包把 spike 页面换成正式实现，并接通 K1 的权威账本。QQ 全通道归 K3，目标配置和场景美术归 K4，但本包必须留下它们可复用的 restore 与进度线消费缝。

# 可行性判据

- K1 提供单 writer、完整分页与删除重推，K0a 提供四点导航和枚举驱动语境；本包在这些已合并基线上实现，不需要重新发明数据规则。
- V9 已提供主页正常/空账/未达/负余额、详细账本和删除确认六帧，布局与 1bpp 形态可作 design review 权威。现有故事列表和删除确认可提供交互先例，但故事 total 与积分余额必须保持独立。
- host 可验证解析、分页、键位和缓存决策，server 可验证语境/删除/推送；真实完整句、不完整句、断网与删除后的实际屏显由一轮真机终裁。

# 跨包事实与共享冻结面约束（含跨切片边角）

- **K1 是余额和分录唯一权威。** 设备只缓存 points_home、已取页、revision、游标和选中 id，缓存掉电即失；不得在 NVS/SD 写积分账本、推算另一套余额或离线补写。
- **source_page 分工**：K0a 已有 home/schedule/points_home/story，本包补 points_ledger 和 selected_id，K5 再补 story_cover。`story` 仍只表示 P4 故事列表记录语境，积分两值不得误切累积态。
- **K3/K4 复用缝**：QQ 写入与目标切换都走 restore:true，其他页不抢前台、积分页在屏原地刷新；K4 的场景标和刻度切换不得另造一套页面状态。
- **相邻冻结锚拆包期预扫非零命中**：`record_session_live` 的语境发送、`keybar_three_slot` 的四点/P3 槽、`story_page_v2` 的故事行为、server session_ctx 族均须保留或按本批语义收紧。K0a 已完成的重锚不得被回退成三页或数字 case。
- **分页与删除边角**：版本变化或游标失效先清旧分页再装首页；`page.total` 不用 items.length 顶替。删除确认整屏覆盖、页点消失、10 秒静默取消，确认后只信权威重推的隐式 ack。

# 铁律约束

- 数据未达与确无分录是两态：未达不显示余额数字或进度线，空账显示 0 与目标；不得用 `balance:null`、伪造 0 或断网标志混同两态。
- 负余额如实显示，进度线按 V9 钳位画法，不把负数藏成 0；超目标保持同页结构，兑换后按新余额重画，不新增“待兑现”或“暂无目标”页。
- 完整句才记账，随口事件不落账；兑换只认当前激活目标分值且余额足够，真实兑现和明确确认后才扣。短窗重复与传输重试不得混为一个判据。
- 设备语音不开放删除；删除确认未收到权威对账不得假删。QQ/恢复推送永不抢前台，只有设备主动语音成功或短窗去重命中按契约展示积分主页。
- 不改 requirements、architecture、`docs/contracts/**`，不把故事星星与积分合并；发现必须改线格式、页态语义或本地持久介质时立即停并升级。

# demo 证据形态（与 roadmap 一致）

- host runner + server pytest：四态、进度线、最近三条、单行分录、分页/重置、键位、删除确认、语境与 restore 的 RED→GREEN 和变异证据。
- device build + 六张 V9 帧 design review，含长原因、大分值、越目标、负余额与 1bpp 资源预算核对。
- 一轮真机：完整/不完整语音、兑换正反、短窗重复、断网、删除致负、冷启动恢复与其他页推送；看屏照片、串口、front/引擎日志关联取证。
