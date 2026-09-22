# Contract Change Packet · M3 课表周网格形状 + 首页 upcoming 承载

> Change ID: 260921-m3-k1-cloud-schedule-producer（随包契约变更，四分类「随 spec 改动携带（同 PR）」拟形）
> Date: 2026-09-21 ｜ Status: HUMAN_APPROVED（2026-09-21 human 契约门裁五问，裁定记录见 §5）
> 关联: `delivery/workstreams/m3-schedule/` ｜ 依据骨架 = `.claude/references/contract-change-evidence.md` ｜ 预审记录 = 主线程会话 2026-09-21 architect-reviewer 报告（裁定表 + 漏问点 a–f 已并入本版 Scope/Human Question）

## 1. Scope（要改什么）

`docs/contracts/interface-contract.md` 两处：

### 触点① E1 §1.3② `schedule` view —— data 形状重定义（spec↔contract 漂移对齐）

现状（旧列表形）：

```json
{ "view": "schedule", "data": { "title": "今天下午",
  "items": [ { "time": "14:00", "name": "钢琴课", "location": "琴行" } ] } }
```

拟改（周网格形，requirements v1.19 口径；具体字段名以契约门定稿为准）：

```json
{ "view": "schedule", "data": {
    "week": {
      "courses": [
        { "day": 0, "slot": 0, "name": "语文", "flags": 0 },
        { "day": 0, "slot": 2, "name": "口语", "flags": 1 }
      ]
    }
  } }
```

- `day` = 0..6（周一=0）、`slot` = 0 上午 / 1 下午 / 2 晚上；一格多门 = 同 (day,slot) 多条，逐门列出。**day 域映射注记（预审漏问点）**：载荷域 0 基与 B1 表「星期取 周一~周日」及 schedule.py 内部 `_WEEKDAY_TOKENS`（周一=1..周日=7）不同——契约定稿时必须写明映射（「day=0 对应 B1『周一』」，0 基 = 数组下标自然序），防 K1/K2 各自脑补差一偏移。
- `flags` 位掩码：bit0 = ※ 临时安排（来自「一次性安排」区块）、bit1 = 星标呼应（该课为云端判定的当前「接下来」标记项）。**理念注记（预审漏问点）**：旧契约理念 = 「不进反白/星标等 UI 语义态」（schedule.py 模块头钉行）；新载荷引入 UI 语义态标志是实质反转——依据 = ※/星标的判定信息（一次性安排区块归属、当前接下来标记项）只有云端可知、spec S-F1.2 AC3/AC4 要求板上呈现、板端无法本地推导。契约落文须写明这一新理念，撤旧钉行。
- 「今天」列**不在载荷内**——按 requirements S-F1.2 补充规则「今天按设备本地日期零点切换」，板端本地推导（设备有 RTC 日期，M1-K1a 已接线）。
- 网格由 front 机械计算（schedule.py 唯一解析器单源），**不由 LLM 生成**；「帮我显示课表」链路取舍点 A（薄载荷 front 展开 vs 设备缓存切页）随本门裁定，甲案倾向 = LLM 发 `data:{}` 薄载荷、front 截获展开。
- **行为句随形状改写（预审漏问点 d）**：§1.3②「板子行为：切到 P2 课表页，列表显示这些课程」现文为已砍旧语义——定稿时行为句草案一并给出（网格渲染、今天列板端本地推导、※/星标渲染、畸形降级不盲渲染 V27）。
- **连带改写面（预审漏问点 a/b，若 Q2 裁甲案则随本 diff 一并批）**：① §1.5 网关职责第 3 条「切分与校验」增写「schedule 薄载荷由 front 截获展开」+ 一句不破「零自研业务逻辑」红线的注记（schedule.py 唯一解析器 + arch §5.1.2 明列课表解析组件归 front）；② §1.6 system prompt 约束模板与 §2.3「DeepSeek 解析……生成答复 + schedule 显示指令」行——甲案下问课表答复走问答框、显示课表走意图薄载荷，两段随形状重定义改写。

### 触点② E1 §1.3⑥ `home` view —— 增可选 `data` 承载「接下来」（新增承载定义）

现状：`{ "view": "home" }`（无 data，语义 = 回首页常显）。
拟改（additive 可选字段，缺 data 时设备行为完全不变——向后兼容）：

```json
{ "view": "home", "data": {
    "upcoming": {
      "day_key": "今天",
      "items": [ { "time": "14:00", "name": "钢琴课" } ],
      "preview": { "time": "明早", "name": "春游" }
    }
  } }
```

- 形状与板端 `home_p1.h` 冻结 seam（`hp1_item_t {time,name}` / `kidpal_home_p1_schedule_inject(day_key, items, next_day_preview)`）对齐——payload 即 seam 注入参数的 JSON 化。
- 分组口径 = 决策 26（group_upcoming 既有语义，云端算好）；推送时机 = 三触发（WS 建连 / 每日零点 / 新建日程即时），复用 mcp `kidpal/display` 既有通道，不新增消息类型。三触发行为策略**不进契约**（预审确认：权威承载 = arch §5.2 场景① + §10 决策 26，契约只管端云边界载荷格式）。
- **producer 归属待裁（预审漏问点 c）**：决策 26 冻结文字 = 「分组由 OpenClaw 算」，prompt v5 课表段现状亦然；但 schedule.py group_upcoming 冻结函数在 front 侧。upcoming 归 OpenClaw 直产还是 front 机械算 → 两条路的契约落文与 prompt v6 改写面完全不同，见 Human Question Q4。

## 2. Alternatives（考虑过什么）

- 新造 view（如 `schedule_grid`/`home_upcoming`）：**否**——旧 `schedule` view 无在役消费方（旧验收已 archive、设备端旧列表页由 M3-K2 整页重写），原地重定义零迁移成本；新造 view 徒增白名单与设备路由面。
- 复用 `schedule` view 承载首页 upcoming：**否**——两类语义（周网格 vs 接下来分组）载荷结构不同，混载逼设备猜意图。
- upcoming 走新私有消息（对偶 `kidpal_session_ctx`）：**否**——mcp display 通道既有且 view=home 语义天然贴合「首页常显」，新增消息类型是更重的契约面。
- 「今天」列由云端下发：**否**——spec 明文设备本地日期零点切换（断网跨天场景下设备自主），云端下发会引入时区/时钟双源。

## 3. Impact（影响谁）

| 消费方 | 影响 |
|---|---|
| 板端 `schedule_page.c`（M3-K2 整页重写中） | 消费新形状；旧列表消费面随重写消亡，无并存期 |
| 板端 `home_p1.c`（冻结 seam，零改） | 经注入 seam 消费 home data；缺 data 行为不变 |
| `gateway/display_schema.py` | view 白名单不变；schedule data 大小上限沿用 MAX_DATA_BYTES（7×3 格 × 逐门课程名，量级远低于 64KB） |
| `gateway/schedule.py` | 模块头钉行「schedule view data 形状 {title,items} 零增减」随本门同 PR 更新；函数增量、既有语义零偏移 |
| OpenClaw prompt（M3-K3 bump v6） | 课表 producer 段重写：答复含备注 + 意图信号化（薄载荷），不再产旧列表 JSON（**影子契约收口面**——v5 课表段事实承载旧形状约定，K3 验收应含「v6 课表段不再产旧形状 JSON」断言） |
| 既有冻结验收 | 板/云 schedule_p2 旧件均已 archive 退役（ARQ-260916 批），无 supersede 面 |
| 过渡窗口期（预审漏问点 f） | K1 契约 diff 落地与 K3 v6 bump 之间 OpenClaw 仍产旧形状 JSON——机械门不校验 data 形状会透传至已重写板端 → 解析失败走 V27 降级（无安全事故但链路错配）。候选处置见 Q5 |

**随包收口义务（gate evidence，预审确认）**：① `architecture-gate-evidence.md` 十节 ledger（§3 四分类行采用预审裁定表）；② `evidence/` 原始验证输出（`validate_display` 三样本回归：新周网格形状 / `data:{}` 薄载荷 / home 带 data 与缺 data 各一）；③ queue 行登记（`docs/CONTRACT-DECISION-QUEUE.md`，漏问点 e）；④ ADR 追加（形状重定义 + home 新承载 = durable semantics 变化）+ 契约正文变更记录注记（先例 = §1.3③ total 的 2026-08-29 记录行）；⑤ 本仓 pass-oracle 第 4 关无脚本 gatekeeper（ARQ-260827-contract-mech-gate-gap 在册）——ledger §5/§8 如实记「人工 diff 逐字核对」降级。

## 4. Human Question（请契约门裁定，五问一次问全）

1. 触点① 周网格形状：`week.courses[{day,slot,name,flags}]` 扁平形 vs 嵌套形、flags 位掩码形态、day 域 0 基 + 与 B1「周一~周日」映射写法——是否认可？——拟扁平形 + 位掩码 + 0 基注明映射（板端自足式最小 JSON 扫描最省解析面）。
2. 取舍点 A：「帮我显示课表」= 甲案（LLM 薄载荷 `data:{}`、front 截获展开新鲜网格）还是乙案（设备缓存切页）？若甲案，§1.5（front 截获展开职责 + 零自研红线注记）/ §1.6（system prompt 模板）/ §2.3（DeepSeek 解析行）三处连带改写是否随本 diff 一并批？——拟甲案 + 一并批。
3. 触点② home 可选 data 承载 upcoming（additive，缺 data 行为不变，`display_schema.py` 现行已机械放行 home 带 data）——是否认可？——拟认可。
4. upcoming payload producer 归属（预审漏问点 c）：OpenClaw 直产（决策 26 既有字面「分组由 OpenClaw 算」，prompt v6 维持分组职责）还是 front 机械算（schedule.py group_upcoming 单源，prompt v6 减负）？——拟 front 机械算（单源防漂移；决策 26 字面偏差同 K1 plan 取舍点 C 注记一并过目）。
5. 旧 schedule 形状即废的过渡口径（漏问点 f）：front 机械校验收紧到新形状（旧形状 display 块拒绝并降级提示），还是登记窗口期错配可接受（V27 降级兜底）？——拟收紧（§1.5「data shape 校验」本已授权）。

## 5. Human Ruling（2026-09-21 契约门裁定记录）

- **Q1 认可**：周网格形状 = 扁平 `week.courses[{day,slot,name,flags}]` + day 0 基注明 B1 映射 + flags 位掩码（human 过目示例载荷后拍板）；连带确认**备注不进网格载荷**（S-F1.2 线框口径——网格/首页只放课程名，备注露面处 = 语音答复 S-F1.3 AC4 + 提醒 speech S-F1.4）。
- **Q2 认可（甲案）**：「帮我显示课表」= LLM 薄载荷 `data:{}`、front 截获展开；§1.5/§1.6/§2.3 三处连带改写随本 diff 一并批。
- **Q3 认可**：home view 增可选 data 承载 upcoming（additive、缺 data 行为不变）。
- **Q4 裁 front 机械算**：upcoming 数据载荷由 front（`server/gateway/front/`，自有网关 kidpal-front.service）用 schedule.py group_upcoming 单源计算；OpenClaw（LLM）专注话术（问课表答复文字、含备注读回），不再产课表 JSON——决策 26「分组由 OpenClaw 算」字面偏差已过目认可（注记见 K1 plan 取舍点 C）。
- **Q5 认可（收紧）**：front 机械校验收紧到新形状，旧 schedule 形状 display 块拒绝并降级提示。
