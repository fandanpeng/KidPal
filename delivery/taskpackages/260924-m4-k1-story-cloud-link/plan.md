---
rigor: full
riskTier: medium
sourceType: delivery-story
reviewed-by: pending（门 1 待发起）
date: 2026-09-24
---

# M4-K1 云端故事数据链路（契约三触点对齐 + session_ctx 拦截 + story_list 推送 + 删除执行 + seq 账本 + 收尾/超时）

> 来源：里程碑 `delivery/milestones/milestones.jsonl#M4-star-notebook`（state=planning）nextAction 块①云端半。
> story 语义权威 = `docs/requirements.md` 现文（S-F2.1/S-F2.2/S-F2.4 + §2.1 记录会话段 + §6 阶段四）；支撑锚 = arch §5.1.2（front 故事接线/story.py）、§5.2 场景④、§5.4（故事行【链路载体待契约】）、§7 契约门在办②④；契约面 = interface-contract.md §1.3③④/§1.4/§1.5/§四。

## 背景摘录（新建型锚「现状与差距」）

- **现状·归档链路半条在世**：`server/gateway/story.py`（409 行）功能完整——`archive_story`（哨兵头 `<!-- kidpal-story:1 -->` + `<id>.md` 落盘）、`list_stories`（story_list payload 组装，total=全集计数）、`story_text`（三键）、`StoryAccumulator`（start/feed/stop/cancel/redo/recover，V31 四条回退）；`front_app.py` 已接 `control/start_recording → acc.start`、`stt 帧 → acc.feed`、`listen-stop → acc.stop 归档 + story_text 下发`、断线 cancel。
- **现状·四个真缺口**（readiness 调查 2026-09-24，progress.md 在案）：① `list_stories()` **零生产调用点**——story_list 从不主动下发，S-F2.1「进页即见列表」无数据源（设备只有 LLM 复文 display 块一条不可靠路径）；② `kidpal_session_ctx` front 侧**零拦截**（契约 §四已 LANDED、CDQ-260914 明写实现归 dev-loop；arch to-be ⑥ 接线点 = 上行泵帧解析处）；③ 删除执行面**全缺**（story.py 无 delete、arch §5.4【链路载体待契约】）；④ 盖章流水号 N **无持久源**（板端易失 stamp_index；S-F2.4 AC2「只增不减」不成立）+ 收尾词/120s 自动收尾缺（现收尾仅 listen-stop 一条）。
- **现状·契约漂移三触点**：§1.3③ total 现文「已录故事总数=故事果计数」兼作盖章 N 源（板子行为句写明「据此渲染…盖章帧『第 N 颗』」）↔ requirements v1.47 拆分（计数=现存篇数删除即−1 / 盖章 N=流水号只增不减）；行为句「KEY 选择」= v1.27 前旧交互。§1.3④ 行为句「或仅靠 TTS 朗读」= v1.52 已砍。删除载体无定义。
- **现状·旧冻结面已退役**：`server/acceptance/archive/test_story_p3.acceptance.py` / `test_story_wiring.acceptance.py` 已归档，本包新建齿不走 supersede 通道。
- **差距**（本包交付）：① 契约三触点对齐（经契约门，CCP 在本包目录）；② front session_ctx 拦截；③ story_list 推送 producer 接线；④ 删除执行面 + 审计；⑤ seq 流水号账本 + story_text.seq 下发；⑥ 收尾词检测 + 120s 自动收尾。

## 改法取舍

> 契约预审注记（2026-09-24 architect-reviewer，NEEDS_REVISION → CCP v2 已并入）：以下取舍点中标 Q 编号者随 CCP v2 十问由人类契约门裁定，裁定前不冻相关形状齿。

- **删除载体（取舍点 A，随契约门 Q5 定，D2）**：甲案 = §四新增端→网关单向私有消息 `kidpal_story_delete`（`{"type":"kidpal_story_delete","id":"<story-id>"}`，kidpal_session_ctx 对偶先例；结果不走 RPC 应答——删除完成靠 story_list 重推对账，板端本地计数 −1 即时、云端推送到达后校准）；乙案 = HTTP `POST /command` 复用（否——设备无 HTTP 客户端面，WS 常驻唯一通道）；丙案 = display control 块反向（否——control 是云→端方向，语义不符）。**倾向甲案**。**可靠性强化（预审漏问点 5）**：裸甲案下 delete 帧丢失 = front 不删 → 重推**复活**；拟「隐式 ack + 超时重发」——设备发 delete 后未收到「不含该 id 的 story_list 重推」即超时重发、front 幂等（不新增请求-应答消息对，ADR-005 边界保持）；设备侧重发接线归 K2、front 幂等归本包。front 执行 = 机械操作（校验哨兵头 + 删 MD + 重推），零自研业务逻辑红线不破；硬删 vs 软删/回收站与审计范围 = Q6（拟硬删 + front 审计日志，误删防护由确认子态 + 10s 静默取消承担）。
- **seq 流水号账本形态（取舍点 B，包内落定）**：甲案 = stories 目录内账本文件（如 `<stories_dir>/.seq.json`，原子写、front 单写者）；乙案 = 扫描全目录文件名/哨兵头推导 max（否——删除后 max 回退，违反「只增不减」）；丙案 = MD frontmatter 埋 seq + 目录索引（重——归档形状变更连带消费面）。**倾向甲案**（最小新面、原子写先例 = pending.json）。账本损坏/缺失降级 = 从现存 MD 数 + 安全边际重建并告警（不把 seq 回退当可接受降级）。seq 缺省降级演出形态 = Q4（拟板端自定、防御路径不占 spec 面）。
- **120s 自动收尾归 front（取舍点 C，D4）**：累积态 front 见得到每帧 stt/音频时间戳，静默检测天然在 front；板端零新增计时器（设备侧 60s/120s timer 生产者 = M1 遗留 to-be ②，本包不复活）。收尾后链路 = 正常 stop 归档 → story_text(seq) 下发 → 板端盖章回列表（板端演出收口归 K2/K3）。问答会话 60s 语义零触碰。
- **收尾词 = front 机械检测（取舍点 D，D5）**：累积态 AI 不逐句回复 ⇒ 收尾词不可能经 LLM 判定，只能 front 扫描 stt 文本（「我说完了」/「讲完啦」精确短语 + 变体容错口径归 acceptance 冻齿）。**先查 StoryAccumulator.routing/recover 现状**——若 V31 回退已含收尾词语义则零偏移复用，不新造第二套。
- **story_list 推送触发与 items 上限（取舍点 E，D6 + 预审漏问点 1；契约门已裁 2026-09-24：Q2=丙案、Q10 认可）**：三时点推送（WS 建连 + 归档成功后 + 删除执行后；front producer，复用 mcp `kidpal/display` 既有下行通道）——触发时机 = 行为策略不进契约（arch §5.2④ 扩展承载）。**items 上限 = 丙案定稿**：front 恒推**最新 16 条窗口**（日期倒序）+ `total` 恒全量现存篇数——板端 `KSTORY_LIST_MAX=16` 维持现值零改动（超限整单 -1 降级语义保持为防御面）；警示句「不得以 items.length 顶替 total」load-bearing（>16 篇时窗口条数 ≠ 计数）。知情注记在 CCP §5 Q2（长列表跳屏动机面收窄 + 旧条目设备端浏览/删除可达性收窄，human 过目代价后选定；旧条目可达面如未来需要 = 新需求另立）。板端「进页即见列表」= 推送 + RAM 缓存（决策 24 不变）；**不做 restore 类冷启动语义**（丙案下载荷有界，建连即推已覆盖，Q10 认可）。
- **双源窗口与校验面（取舍点 F，预审漏问点 6 + 2.1-4，随契约门 Q7/Q8/Q9 定）**：v6 prompt 仍指示 LLM 产 story_list/story_text 块且 OpenClaw 无 story 工具（一回合不读写文件）⇒ LLM 产块 = **模型臆造**，与 front 机械推送构成真双源（旧版 CCP「无双源漂移」断言已撤）。拟 **front relay 收紧**：拒绝引擎侧来源的 story_* display 块（relay 路径按 view 过滤；front 自推不经 relay 天然豁免）——K1 即收口双源，v6 产块指令被闸口事实失效（v7 文面收口仍归 K3 不提前）。display_schema 现状 story_* **零形状 floor**（仅 schedule 有）——拟新增轻量 floor（total 整数/items 三键/seq 可选整数，schedule 先例对称）随 Q8；§1.6 模板 story 段（「不产 story JSON」对称句）随 Q9 定随本 diff 或 defer v7。
- **session_ctx 拦截点**：front 上行泵帧解析处（arch to-be ⑥ 既定接线点）；拦截 = 整条消息消费不透传（契约 §四「无论取值一律拦截」）；`source_page=story` 与 `control/start_recording` 收敛同一幂等入口（acc.start 幂等语义既有，重复触发不重复开会话）。
- **随包钉行收口面（预审 2.1-1，契约 diff 同 PR）**：`story.py:12-13` 模块 docstring「契约零改动红线：不给 story_list/story_text 加字段」（seq 即加字段——钉行由契约门裁定取代，M3 schedule.py 先例）+ `:181-183` list_stories docstring（total 兼两义旧口径）+ `:211` story_text docstring（「恰为三键」）——三处逐字更新，pass-oracle 第 4 关人工 diff 核对清单在 CCP §3。

## 阶段划分

1. 契约门先行（M4K1-A1）：CONTRACT-CHANGE-PACKET（本包目录）→ architect-reviewer 四分类预审 → human 契约门签字；三触点形状定稿（含取舍点 A 裁定）。
2. acceptance-author 冻齿：拦截齿（A2）+ 推送齿（A3）+ 删除齿（A4）+ 收尾/超时齿（A5）+ 账本齿（A6），实跑红基线落账。
3. builder：front_app.py 拦截/推送/删除消费接线（+ Q7 裁定形态的 relay story_* 收紧）+ story.py delete_story/seq 账本 + 收尾词/120s + 契约 diff 落文 + story.py 三处钉行更新（模块 docstring/list_stories/story_text——预审钉行表）+ display_schema story floor（Q8 裁定形态）。
4. verifier↔fixer：pytest offline 全量零新增红 + 契约 diff 复读核对（gate evidence §7 validation）。
5. integrator/supervisor 闭环；live 全链 deferred 声明（M4K1-A7 挂 K3 设备轮）。

## 铁律约束

- 不改 `docs/contracts/**` 除非经契约门通道（四分类预审 + human 签字在案，diff 与签字同 PR）；不改 spec、不改 agent 定义/gate。
- `archive_story` / `StoryAccumulator` 既有语义零偏移（增量扩展；V31 四条回退断言保持）；listen-stop 归档链路保持在世。
- `kidpal_story_delete` 为 §四端云消息、**不进 display view 白名单**（display_schema.py 白名单零扩）；OpenClaw 本体（ECS）架构零改动；prompt 面零触碰（v7 归 K3）。
- mutation 齿自证（RED→GREEN 声明 + counterEvidence 回填 taskpackage）。

## demo 证据形态复述

offline：pytest 新增齿 RED→GREEN 全程留痕（红名 verbatim + mutation 应用/还原记录）+ 契约门 evidence 制品（十节骨架）；live 全链不在本包——demo 口径 = offline 全量绿 + 契约门 CLOSED 路径声明 + M4K1-A7 deferred 挂账声明。live「双入口开录→收尾三式→归档盖章 seq→列表推送→删除同步」的最终目击证据归 M4-K3 设备轮（M4K3-A6）。
