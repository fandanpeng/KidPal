# TaskPackage Plan · 260923-m3-k7-schedule-grid-restore

> **性质**：dev 包 reviewed plan——上游 = 260923-arch-review-schedule-grid-cold-boot（Track A full-adversarial，adjudicator GREEN + 契约门批准 + landing GREEN，commit `565cb2d`）。本 plan 是上游 reviewed plan 的实现包转写，不新开设计面。
> **reviewed-by**：architect（triage 门比例化 review → /arch-change 对抗 review → adjudicator 终裁 + 契约门 human 批准）
> **date**：2026-09-23
> **rigor**：full ｜ **riskTier**：medium ｜ **sourceType**：arch-change-handoff

## 1. 目标（业务语言）

孩子重启设备后翻到课表页（P2），不再看到「还没有课表数据」空态——云端在既有「接下来」三触发推送点（WS 建连 / 每日 0:00 跨天 / 录入 marker）附带重推周网格**恢复性载荷**（`data.restore: true`），设备收到后**只填 RAM 网格缓存、不接管前台**，P2 在屏时仅重绘本页。S-F1.2「翻到课表页就看到一周总览」从此冷启动即成立。

## 2. 已冻结的设计（上游 spec/契约面，零再设计）

- 契约 §1.3②：`data.restore` additive 布尔（缺省/absent/显式 false = 现行为；true = 恢复性）；modifying L85 板子行为句；生产纪律（仅 front 后台附带推送路径可置；薄载荷展开恒不置）。
- arch §5.2 场景① + ADR-005：三触发附带推 grid；**部署时序 = 固件先行，builder 实测复核前 ECS 推送开关保持默认关**（C1 挂账）；旧固件行为已由仓内代码定案 = 确定性接管前台顶到 P2（`schedule_page.c:607-613` + `mcp_server.cc:620-644`）。
- 降级双面：restore 载荷板端解析失败 = **静默记日志**（不 SetChatMessage、不动前台）；front V27 在入板前独立生效。
- F-C5 残余风险：LLM 幻觉 restore 路径 accepted-as-risk（ADR-005 残余风险段；ARQ-260923 转 watchlist），本包**不设闸**。
- 泛化立场：本机制 = 「数据面更新 vs 前台接管分离」第二实例；不预扩字段。

## 2.1 红线（违反须回 triage 升级）

- docs/contracts/** 与 spec 冻结面零改（上游已落，commit 565cb2d）；本包不改 `expand_schedule_intent_payload` 输出语义（薄载荷展开恒不置 restore）。
- M3-K3 语音「我的课表」→ 接管前台切 P2 = 冻结行为零回归；alert 起轮（`_register_alert_legs` 时序）零回归；home upcoming（K6 锚①）零回归；BOOT 翻页读缓存路径零回归。
- 决策 24 介质禁令：全程 RAM，零 NVS/Flash/SD。
- 部署纪律：**ECS 推送开关默认关**——server 侧新增开关（config/env），builder 只实现开关与默认关状态，**不实测开闸**（开闸 = 烧板验证后 human/integrator 人工动作）。
- restore 缓存写路径须显式持 LVGL 锁（无 begin_page 隐式持锁——challenger C3 canary 注记，转 builder 注意项）。

## 3. 改动面

### server 侧（估几十行）
- `server/gateway/schedule_push.py`：仿 `connect_push_payload`（L73-81）单源委托 `schedule.py week_grid_payload` 增 grid 变体（restore 置位）。
- `server/gateway/front/front_app.py`：建连推送点（L762-773）在 home upcoming 之后追加 grid restore relay（同 model 快照、同 try/except 不阻断建连）；每日 / 录入 marker piggyback 路径同型追加（沿 `run_schedule_poll_once` 同一 due 判定点，勿复制 `_daily_push_local_date` 进程内日账形态——F3 纪律）；**新增 ECS 推送开关**（config/env，默认关，门控 grid restore relay 的发出）。
- `server/gateway/schedule.py`：**零改动**（`week_grid_payload` 单源已存在）。

### firmware 侧（估几十行）
- `firmware/main/mcp_server.cc` schedule 路由（L620-656）：读 `data.restore` 布尔——true 走新分支：`kidpal_schedule_grid_parse` → 成功则 `kidpal_render_node_cache_schedule_grid` + 若 P2 在屏则重绘 P2（仿 `kidpal_render_node_home_on_screen` 先例补 on-screen 查询/重绘小缝）；**不调 `kidpal_render_node_begin_page`**。降级（parse 失败）= 静默记日志。
- `firmware/main/display/kidpal_render_node.cc`：on-screen 查询/重绘小缝（谓词沿 home on_screen 先例口径；SERVER_PAGE 接管态下 restore 仅填缓存、不重绘——ADR-005 R10 括注）。
- BOOT 翻页读路径（kidpal_render_node.cc:728-748）**零改动**。

### 验收测试面（acceptance-author 所有，builder/fixer 不改）
- server pytest：① 建连推送帧序断言（home 先 grid 后、schedule.md 缺失/坏不阻断建连）；② `expand_schedule_intent_payload` 输出恒无 restore（薄载荷回归齿）；③ alert 起轮时序（`_register_alert_legs` 先于建连推送）；④ ECS 开关默认关断言。
- firmware host acceptance（新文件 `firmware/main/test/schedule_grid_restore.acceptance.c`）：⑤ restore 消费分支——层栈断言不新增 SERVER_PAGE（三前台态，key_layer_dispatch weak 哨兵先例）+ 零 SetChatMessage + 缓存 memcmp 幂等；⑥ 语音接管回归守卫（薄载荷路径 begin_page 照旧）；⑦ P2 在屏重绘 + 接管态仅填缓存（R10 谓词口径）。

## 4. 验收锚（acceptance-author 起齿取材，upstream = review-packet §6）

- **K7-A1**：server 建连帧序——upcoming 先、grid restore 后（fake websocket 注入，K1 三触发模拟齿先例）；schedule.md 缺失/坏不阻断建连（try/except 语义保持）。
- **K7-A2**：薄载荷展开恒不置 restore（机械齿，守 F-C14 生产纪律）——`expand_schedule_intent_payload` 输出断言（absent/false 同判，R7 口径）。
- **K7-A3**：ECS 开关默认关——默认配置下 grid restore relay 不发出（C1 挂账的机判齿）。
- **K7-A4**：alert 起轮不回归——`_register_alert_legs` 注册时序零偏移（pytest 时序断言）。
- **K7-A5**：firmware restore 消费——层栈快照不新增 SERVER_PAGE（三前台态：P1/P2 在屏 + 接管态）+ 零 SetChatMessage（weak 哨兵捕获）+ 缓存 memcmp 幂等（重复注入同数据缓存零偏移）。
- **K7-A6**：语音接管回归守卫——薄载荷路径 begin_page + SERVER_PAGE 接管照旧（M3-K3 冻结行为）；P2 在屏重绘语义（本地页前台态重绘、接管态仅填缓存）。
- **K7-A7**：每日 / marker 触发附带——piggyback 路径 grid 附带同型（沿 run_schedule_poll_once 同一 due 判定点、不新增第二套日账）；**注意 R8 分层口径：录入 marker 附带降 SHOULD 级**（非独立必要，ADR-005 分层如实记）——实现包验收按此定级。
- **K7-A8（真机锚，挂设备轮窗口，human 在场，不阻塞 Pod）**：①冷启动无按键 → P1「接下来」恢复 + 串口见 grid restore 载荷接收日志（开关开后）；②冷启动直翻 P2：网格有内容（S-F1.2 AC1 冷启动成立——本包的目标场景）；③语音「我的课表」接管照旧（开关开/关两态各测一次）；④旧固件行为复核实测（C1 evidence：烧新固件前旧固件 + 开关开 = 确定性接管顶到 P2 的实测记录）。

## 4.1 C1 closeout 证据链

1. builder 交付：ECS 开关实现 + 默认关状态 + 配置注入路径文档化；
2. 真机轮：K7-A8④ 旧固件行为复核实测（烧新固件前）；
3. integrator 收口：C1 evidence 归档 + 开闸留痕（开闸 = human 动作，integrator 只备路径不执行）。

## 4.1.1 R1-R11 修复项 → 验收齿增量（upstream findings §2）

- R3/R4（部署纪律）→ K7-A3 开关默认关齿 + K7-A8④ 实测复核。
- R7（false 态）→ 并入 K7-A2 断言面（absent/false 同判）。
- F-C5 裁 ADR 记法 → 齿面维持现状（不设闸）；幻觉路径入 ADR-005 watchlist，无新齿。

## 4.1.2 mutation 要求（沿用 K6 纪律：mutation 实跑落 evidence）

新 acceptance 文件逐锚 mutation 实跑、原始输出落 `delivery/taskpackages/260923-m3-k7-schedule-grid-restore/mutation-evidence/`（逐 acceptance 一文件）。

## 5. 风险与回归面

1. 提醒链路与课表推送共用建连接线段（`_register_alert_legs` 顺序）——K7-A4 齿钉死。
2. M3-K3 语音接管冻结行为——K7-A6 回归守卫。
3. BOOT 翻页读路径零改动——既有 schedule_grid 齿零偏移约束。
4. 旧固件部署窗口——C1 开关门控 + K7-A8④ 实测。
5. F-C5 幻觉 restore 路径——accepted-as-risk，ADR-005 watchlist（本包无齿）。

## 6. 明确不做（已裁面）

- 不设 restore 运行时闸（F-C5 已裁）；不扩协议/消息类型（Shape B 否决）；不改需求 AC（Shape 需求面砍否决）；不持久化（决策 24）；不改需求 NFR 行为「不缓存」语义（仅注记）；不动 §1.6 system prompt。

## 7. 下游链

- 上游冻结制品：`delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/` + 契约 CCP + ADR-005（commit 565cb2d）。
- 收口义务：progress.md 落账（m3-schedule workstream 遗留③销账）+ pod-retro.md（closeout 相位）。
- 设备轮：K7-A8 挂设备轮窗口（human 在场）；C1 开闸 = human 动作。
