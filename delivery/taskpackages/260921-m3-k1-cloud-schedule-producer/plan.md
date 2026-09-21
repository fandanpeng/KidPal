---
rigor: full
riskTier: medium
sourceType: delivery-story
reviewed-by: pending（门 1 待发起）
date: 2026-09-21
---

# M3-K1 云端课表数据链路（契约对齐 + 周网格/接下来计算 + 三触发 producer）

> 来源：里程碑 `delivery/milestones/milestones.jsonl#M3-schedule`（state=planning）nextAction 三块。
> story 语义权威 = `docs/requirements.md` 现文（S-F1.1/S-F1.2/S-F1.3/S-F1.5 + §6 阶段三）；支撑锚 = arch §5.1/§5.2 场景①/§10 决策 26；契约面 = `docs/contracts/interface-contract.md` §1.3②/§2。

## 背景摘录（新建型锚「现状与差距」）

- **现状·解析器已有、producer 缺位**：`server/gateway/schedule.py` 已有 B1 三区块纯解析（`parse_schedule_md`/`items_for_date`/`group_upcoming`，决策 26 分组口径含合并「上学」项实现注记）——**唯一解析器**，openclaw-patch 复用不自建第二套；但 `front_app.py` 对课表**零接线**（grep 证据：front 只有提醒投递面接线），决策 26 三触发（WS 建连/每日零点/新建日程即时）producer 链路全部未实现，首页「接下来」数据从未从课表出过。
- **现状·契约与 spec 漂移**：契约 E1 §1.3② schedule view 仍为旧列表形状 `{title, items:[{time,name,location}]}`（`schedule.py` 模块头钉行「形状零增减、不进反白/星标等 UI 语义态」即此口径）；requirements v1.19（2026-09-14 human 裁课表大改）已把 P2 改为一周总览网格、数据口径 = 云端按 B1 三区块算好下发 ⇒ spec↔contract 漂移 + 形状重定义；决策 26 首页「接下来」下发通道在契约无承载定义（§1.3⑥ home view 现为无 data 的「回首页」）。
- **现状·旧冻结面已退役**：板端 `firmware/main/test/acceptance/archive/schedule_p2.acceptance.c` 与云端 `server/acceptance/archive/test_schedule_p2.acceptance.py` 均已随 ARQ-260916-server-mvp-teeth-sweep 批归档，本包新建齿不走 supersede 通道。
- **差距**（本包交付）：① 契约对齐（周网格形状 + home upcoming 承载，经契约门）；② B1→周网格计算函数（学校课逐节/课外逐门/※/星标）；③ upcoming → home data payload；④ front 三触发推送接线 + 录入即时通知通道。

## 改法取舍

- **网格由 front 机械计算、不由 LLM 生成**：S-F1.2 补充规则「网格内容由云端按课表 MD 算好下发」——front 持有唯一解析器（schedule.py 单源），周网格 = 纯函数计算，确定性、可 offline 验收；LLM（OpenClaw）只做意图信号（问课表→文字答复、显示课表→触发下发），不再拼网格 JSON（旧 prompt v5 让 LLM 产 `{title,items}` 列表已是错配，周网格更大更不可行）。K3 的 prompt v6 按此分工收口径。
- **「帮我显示课表」触发链（取舍点 A，随契约门定）**：OpenClaw 意图命中后如何拿到网格——甲案：LLM 发薄载荷 display 块（view=schedule、data 空/最小），front 截获后用 schedule.md 展开成完整网格再下发（front 双泵既有截获能力，单源新鲜）；乙案：设备用缓存网格（producer 已推过）仅切页。**倾向甲案**（数据恒新鲜、设备保持哑终端），乙案作降级注记。契约 §1.3② 语义按裁定落文。
- **「接下来」承载（取舍点 B，随契约门定）**：view=home 增可选 data（additive，向后兼容——缺 data 时设备行为不变），形状按 `home_p1.h` 冻结 seam（`hp1_item_t {time,name}` + day_key + items + 明日预告 preview）对齐；不新造 view、不复用 schedule view 承载两类语义。三触发推送复用 mcp `kidpal/display` 既有通道。**upcoming producer 归属（契约门 Q4，预审漏问点 c）**：决策 26 字面 = 「分组由 OpenClaw 算」，拟 front 机械算（schedule.py group_upcoming 单源防漂移）——随契约门一并裁。
- **录入即时通知通道（取舍点 C，包内落定）**：甲案 = OpenClaw 改写 MD 后在既有交接目录 `/opt/kidpal/alerts/` 落 refresh marker 文件，front 既有 1 分钟轮询循环 piggyback 检查（复用 alerts 模式，主/副通道天然可区分——只有 OpenClaw 会写 marker）；乙案 = OpenClaw 调 front 既有 POST /command；丙案 = front 轮询 schedule.md mtime（**否**——无法区分主/副通道，会把 S-F1.5 AC2「副通道无即时推送」语义抹平）。**倾向甲案**（零新端点、零架构改动、ARQ-260902 部署补丁模式先例）。**决策 26 字面偏差注记（门 1 ADV-1，供 human 契约门一并过目裁定）**：架构 §10 决策 26 冻结文字为「QQ 录入路径 = OpenClaw 自改写即时重算+推送、分组由 OpenClaw 算」，甲案下重算+推送执行体落 front（OpenClaw 只落 marker），schedule.py 单源不破（openclaw-patch 复用同一解析器，生产消费归 front）；「即时」上界 = front 轮询周期 ≈ 60 秒（S-F1.5 AC1 无延迟数值 bound）；语义链保持——主通道即时、副通道不即时。若 human 裁字面落位不可偏，回乙案（OpenClaw 直调 front）即可，两案共享下游实现。
- **零点触发归 front**：设备 wss 常驻连接，front 按 B1 frontmatter timezone（Asia/Shanghai）调度每日 00:00 推送（建连/零点/即时三触发全在 producer 侧，设备零新增计时器——RTC 日期仅用于板端「今天」列推导，归 K2）。
- 架构级改法（课表数据入引擎侧/新消息类型）不在本包——如契约门裁「home 承载」需新消息类型，按需触发逃生门转 /arch-change。

## 阶段划分

1. 契约门先行（M3K1-A1）：CONTRACT-CHANGE-PACKET（本包目录）→ architect-reviewer 四分类预审 → human 契约门签字；契约 diff 形状定稿（含取舍点 A/B 裁定）。
2. acceptance-author 冻齿：网格计算齿（A2）+ upcoming payload 齿（A3）+ 三触发模拟齿（A4），实跑红基线落账。
3. builder：schedule.py 周网格函数（增量，既有函数零偏移）+ home payload 生成 + front_app 三触发接线 + 录入即时 marker 通道（取舍点 C 裁定形态）+ 契约 diff 落文 + schedule.py 钉行更新。
4. verifier↔fixer：pytest offline 全量零新增红 + 契约 diff 复读核对（gate evidence §7 validation）。
5. integrator/supervisor 闭环；live 三触发 deferred 声明（M3K1-A5 挂 K3 设备轮）。

## 铁律约束

- 不改 `docs/contracts/**` 除非经契约门通道（四分类预审 + human 签字在案，diff 与签字同 PR）；不改 spec、不改 agent 定义/gate。
- `parse_schedule_md`/`items_for_date`/`group_upcoming` 既有语义零偏移（增量扩展，决策 26 合并项 12:00 断言语义保持）。
- OpenClaw 本体（ECS 部署）架构零改动；通知走既有交接目录/部署补丁模式。
- mutation 齿自证（RED→GREEN 声明 + counterEvidence 回填 taskpackage）。

## demo 证据形态复述

offline：pytest 新增齿 RED→GREEN 全程留痕（红名 verbatim + mutation 应用/还原记录）+ 契约门 evidence 制品（十节骨架）；live 三触发不在本包——demo 口径 = offline 全量绿 + 契约门 CLOSED 路径声明 + M3K1-A5 deferred 挂账声明。live「联网/零点/录入三时点设备实际刷新」的最终目击证据归 M3-K3 设备轮（M3K3-A3）。
