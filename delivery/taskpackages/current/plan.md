---
changeId: 261010-p3-storylist-cold-start-restore
rigor: full
riskTier: medium
sourceType: regression
reviewed-by: architect（/triage-change 分流 review，2026-10-10）
date: 2026-10-10
launchPreconditions: 见「上游门」节——spine 面板 anchors 未回填不得 launch（spine 源条款）
---

# P3 story_list 冷启动恢复性推送（TECH-261010-p3-storylist-cold-start-empty · dev 面）

## 上游门（launchPreconditions，先于本包）
spine = delivery/changes/intake/261010-p3-storylist-cold-start.md。launch 前须回填：
1. 需求面 anchor：S-F2.1 补充规则定稿（「进页即见列表 + RAM 缓存」+ 冷启动恢复 = WS 建连附带重推〔恢复性推送、只填缓存不抢前台、P3 在屏时刷新本页〕+ P3 空态两态分叉话术）——/requirement-change loop，可与 REQ-261010-p1-schedule-empty-state 并批裁定。
2. 架构面 anchor：arch §5.2 场景④（:213）「现役两时点」重载含恢复时点①——随需求面批同批落。
3. 契约面 anchor（若取 restore 标记方案——推荐）：story_list data.restore additive（复刻 §1.3② L85 / ADR-005 先例）——architect-reviewer 四分类 + human 契约门后与实现同批原子落。若 human 裁「固件本地页 gating、wire 零变化」备选则本条豁免（该备选改 K2 消费面「非 VOICE 前台一律推前台」既有语义——取舍呈 human，存疑向严）。

## 根因假设
- 三时点①建连即推已于 260928 摘除（front_app.py:1013-1019 注释 + FIX-A1 负锚齿 test_m4k1_story_cloud_link.acceptance.py:1000-1044）；现役触发点 = ②归档后（:573）/③删除后（:223）/①恢复 = ctx(story) 会话建立同拍（:216；固件 application.cc:1014-1020 已接通）。
- BOOT 翻页浏览路径无任何触发点；板端 RAM 缓存重启即失（决策 24）→ 重启后翻 P3 恒走 count<=0 空态分支（kidpal_render_node.cc:874-886）。
- 摘除原因（旧固件无条件渲染 P3 盖聆听态）的约束面已半变：固件 K3 [S4] gating 在场（mcp_server.cc:1088-1118；story_page.c:1966-1975）——但**只覆盖「记录语境前台」半，缺「非 P3 本地页前台」半**（实现前核项已核，见 faceMap）。恢复帧不带标记会在 HOME/P1/P2 前台推 P3 抢前台 = 原缺陷形态复发；带 restore 标记则天然只填缓存（schedule 先例 mcp_server.cc:926-991 实核在场：restore=true → cache_story 只装缓存不 end_page）。

## 修复方案（推荐 = restore 标记，P2 先例复刻）
- front：连接处理段（front_app.py:1005-1012 schedule connect push 同位）追加 _push_story_list(websocket, restore=True)（函数加 restore 形参、下行 data 带 restore: true；②③调用点默认 False 零偏移）；失败不阻断建连（try/except 同 :1009-1012 先例）。
- 固件：mcp_server.cc story_list 分支（:1034-1119）按 data.restore 分流——restore=true → 解析装载缓存 + kidpal_render_node_cache_story 同步 + 若 P3 在屏（s_local_page == KP_PAGE_STORY）刷新本页，否则前台零触碰（复刻 schedule :926-991 形态）；restore 缺省/false = 现行为零偏移（K2 消费面 + K3 [S4] gating 全保持）。
- 验收翻正（正常改验收通道 + human 方向锚留痕，260928 FIX-A2 先例形态）：FIX-A1 负锚齿「建连零 story_list 帧」→「建连恰一帧 restore story_list」；test_m4k3_record_session_live.acceptance.py:25 跨包负锚同批翻正。
- 空态话术按需求面定稿落（两态分叉）；固件空态渲染入口两处同源纪律（kidpal_render_node.cc:875-886 注记）不破。
- crossPackageBehaviors 登记：「story_list 恢复性推送语义」owner = 本包（260928 defer 挂起的「①恢复时机重裁」由本包赎回，ARQ-260911 纪律）；crossEnd counterparts = front_app.py::_push_story_list ↔ mcp_server.cc story_list 分支。

## 影响面
- 文件：server/gateway/front/front_app.py；firmware/main/mcp_server.cc；server/acceptance/test_m4k1_story_cloud_link.acceptance.py + test_m4k3_record_session_live.acceptance.py（FIX-A1 翻正）；firmware story 面测试（restore 分流纯逻辑齿 additive）；契约 docs/contracts/interface-contract.md §1.3③（restore additive——经契约门后落，本包不直改契约）。
- 语义零偏移面：ctx(story) 同拍推送（:216）、②③时点、items ≤16 窗口 / total 语义（契约既有）。
- planningImpact 建议：attach-existing → delivery/workstreams/m4-star-notebook（planner intake 定夺）。

## acceptance 种子
impactedTests：server/acceptance/test_m4k1_story_cloud_link.acceptance.py（FIX-A1 翻正 + ②③ wire 断言零偏移复跑 + restore 帧新断言）；server/acceptance/test_m4k3_record_session_live.acceptance.py（跨包负锚翻正 + K1 27 齿零偏移套件全量）；firmware story 面测试（story_page_v2 + restore 分流纯逻辑新齿）；契约落地后 drift 人工核对如实记录（本仓无机械门，ARQ-260827 口径）。

验收齿草案：
- SL-A1（翻正, host offline）：建连路径恰一帧 story_list 且 data.restore == true（先红〔现零帧〕后绿）。
- SL-A2（固件 host 纯逻辑）：restore 帧 + 非 P3 前台 → 只装缓存零前台接管（cache_story 被调、begin/end_page 不被调）。
- SL-A3（固件 host 纯逻辑）：restore 帧 + P3 在屏（LIST 冷浏览）→ 重渲本页。
- SL-A4（固件 host 回归）：记录语境（VOICE 前台/录音态）收到 restore 帧 → 缓存不抢前台（K3 [S4] gating 与 restore 分流叠加语义钉死）；非 restore 帧（②③）→ K2 消费面零偏移。
- SL-A5（真机轮）：重启设备（云端现存 ≥1 篇）→ 翻到 P3 显示已归档列表（含计数）而非空态；期间任意页前台无 P3 抢占；空态两态话术按需求面定稿验收。
- SL-A6（front host）：恢复推送失败不阻断建连。

## 非目标
- ctx(story) 会话同拍推送、②③时点零改动；P1 首页空态（REQ-261010-p1 需求面另批）；「闲置休眠断连」产品决策；story_list items 窗口 / total 语义。

## demo 证据形态
host 双面（front + 固件）红基线先红后绿留痕 + 真机轮（human 在场）：重启冷启动翻 P3 见列表 + 空态两态 + 无前台抢占。烧录 = app 分区（无资产变更）。
