# Contract Change Packet · home.upcoming.preview 多项数组化 + 三态语义 + 示例顺修

> Change ID: 261011-p1-home-v5-rework（随包契约变更，先批后落、同 PR git apply——260921-m3-k1 先例）
> Date: 2026-10-11 ｜ Status: HUMAN_APPROVED（architect-reviewer 预审 2026-10-11；human 契约门 AskUserQuestion 批准 2026-10-11，逐字裁定 = 批准 packet §3 全部四触点、随包同 PR git apply 落地）
> 依据骨架 = `.claude/references/contract-change-evidence.md` ｜ 预审 = 本包 architect-reviewer 报告（2026-10-11，同会话 return）
> 需求权威 = requirements.md v1.61（commit 7f68b51）：S-F1.1 AC2/AC5/AC6/AC7 + 补充规则 6c；spine = delivery/changes/intake/261011-p1-home-v5-rework.md

## 1. Scope（要改什么）

`docs/contracts/interface-contract.md` §1.3⑥（:144-158），四触点一次批：

### 触点① `data.upcoming.preview` 形状重定义（单对象 → 多项数组 + 三态语义）

现状（单对象，明日预告首项；明日无项时 producer 发 `null`）：

```json
"preview": { "time": "明早", "name": "春游" }
```

拟改（多项数组）：

```json
"preview": [ { "time": "08:00", "name": "上学" }, { "time": "19:00", "name": "编程" } ]
```

三态语义：
- **非空数组** = 明日项列表——明日日程全量进区：学校课合并「XX:XX 上学」（与「接下来」区同款合并规则）与课外班、临时事件同列，按开始时间升序（requirements S-F1.1 AC2，2026-10-11 human 改判，翻转 v1.57「仅课外班与临时事件」OQ2 口径）。
- **`[]`（空数组）= 云端确认明日无项**——板端据此显示「明天没有什么课喽」空态话术（AC6；AC6 空态话术的显式 wire 载体）。上学日明天区必然非空（周中合并分支自动出「上学」项），空态话术仅非上学日可达。
- **`null` 或缺省 = 未推送/未提供**——板端显示数据未达友好提示、不显示无安排话术（AC7，与空态话术显式分叉；容忍 legacy，向后兼容不变）。

**为什么必须引入空数组语义**：现状 `null` 同时承担「未推送」与「明日确无项」，AC6 话术无 wire 载体；数组化天然承载三态。

### 触点② 示例值顺修（`"time": "明早"` 漂移收回）

现状 `"time": "明早"` 是 v1.57 OQ2 之前的自然语言标签旧漂移——K1 裁决口径「载荷钉 HH:MM、自然语言标签不入 wire」（板端 home_upcoming.h:30-31 对非 HH:MM preview.time 直接拒包），现役解析器本就解析不了契约自己的示例。拟改 = 触点① 新示例即 HH:MM 字面数组；「明天」定名只落板端 chip 渲染资产（spiffs 重烘，另一性质、同批收口），**不作为 wire 值入契约**。

### 触点③ :155 seam 引用签名顺修（预审新发现触点）

契约该行逐字引用板端冻结 seam 旧签名 `kidpal_home_p1_schedule_inject(day_key, items, next_day_preview)`；本包 seam 同形改为 `(day_key, items, count, next_day_items, next_day_count)`，契约引用行随批原位更新，否则契约↔实现漂移。

### 触点④ 契约变更记录行 append

按 §1.3 既有先例格式（:109-110、:158）在 :158 记录行后 append 2026-10-11 记录行（逐字文本见 §3 diff）。

## 2. Alternatives（考虑过什么）

- **单对象扩展字段**（如 `preview: { first: {...}, more: [...] }`）：否——两形状字段并存、板端解析面翻倍，「确认空」仍无载体。
- **双形状并存**（对象或数组皆合法）：否——机械校验与板端解析各自容两形，漂移面翻倍；数组一种形状即可承载三态。
- **并行布尔字段**（如 `upcoming_confirmed_empty: true`）：否——与 `preview` 组合出矛盾第四态（布尔 true 且 preview 有项），多一个可漂移源。
- **新 view（如 `home_upcoming`）**：否——`home` view 天然承载、2026-09-21 已裁 additive 通道在役，新 view 徒增白名单与设备路由面。
- **硬拒 `null`/旧单对象**：否——`null` 是现役在发值；旧单对象是现役板端在解析值。部署缓冲要求板端对旧单对象容忍一个过渡版（板端防御，不入契约形状——预审确认边界正确），契约/producer 先行、板端同批跟上。
- 阶段纪律：不涉及 Phase 2 能力（WebSocket 流式等）提前入契约。

## 3. Proposed Diff（逐字，git apply 用；文件为 CRLF 行尾，落地须保全）

```diff
--- a/docs/contracts/interface-contract.md
+++ b/docs/contracts/interface-contract.md
@@ -141,21 +141,23 @@
```
板子行为：回 P1 首页常显。

**可选 `data.upcoming` 承载「接下来」（2026-09-21 新增，additive——缺 `data` 时设备行为完全不变）**：
```json
{ "view": "home", "data": {
    "upcoming": {
      "day_key": "今天",
      "items": [ { "time": "14:00", "name": "钢琴课" } ],
-      "preview": { "time": "明早", "name": "春游" }
+      "preview": [ { "time": "08:00", "name": "上学" }, { "time": "19:00", "name": "编程" } ]
    }
  } }
```

-- 形状与板端 `home_p1.h` 冻结注入 seam（`hp1_item_t {time,name}` / `kidpal_home_p1_schedule_inject(day_key, items, next_day_preview)`）对齐——payload 即 seam 注入参数的 JSON 化。
+- 形状与板端 `home_p1.h` 冻结注入 seam（`hp1_item_t {time,name}` / `kidpal_home_p1_schedule_inject(day_key, items, count, next_day_items, next_day_count)`）对齐——payload 即 seam 注入参数的 JSON 化。
+- **`preview` 三态语义（2026-10-11 契约门改多项数组）**：明日「接下来」全量逐项 `{time,name}` 数组、按开始时间升序（requirements S-F1.1 AC2——明日日程全量进区：学校课合并「XX:XX 上学」与课外班、临时事件同列）。三态：**非空数组** = 明日项列表；**`[]`（空数组）= 云端确认明日无项**，板端据此显示「明天没有什么课喽」空态话术（AC6——上学日明天区必然非空，话术仅非上学日可达）；**`null` 或缺省 = 未推送/未提供**，板端显示数据未达提示、不显示无安排话术（AC7，与空态话术显式分叉）。`time` 钉 HH:MM 字面（K1 裁决：自然语言标签不入 wire，板端解析拒绝非 HH:MM 值）。
- 分组口径 = 决策 26「接下来」分组（学校课合并「上学」、培训班/一次性逐项、最近项最醒目、再下可带明日预告）；**producer = front 机械计算**（schedule.py `group_upcoming` 单源），OpenClaw（LLM）专注话术、不产此载荷（2026-09-21 契约门裁定）。推送时机（WS 建连 / 每日零点 / 新建日程即时）属行为策略，权威承载 = arch §5.2 场景① + §10 决策 26，不进契约。
+- 分组口径 = 决策 26「接下来」分组（学校课合并「上学」、培训班/一次性逐项、最近项最醒目、再下可带明日预告——`preview` 形状与明日分组口径见上条，2026-10-11 改多项全量）；**producer = front 机械计算**（schedule.py `group_upcoming` 单源），OpenClaw（LLM）专注话术、不产此载荷（2026-09-21 契约门裁定）。推送时机（WS 建连 / 每日零点 / 新建日程即时）属行为策略，权威承载 = arch §5.2 场景① + §10 决策 26，不进契约。

- （契约变更记录：2026-09-21 经契约门批准新增可选 `data.upcoming` 承载首页「接下来」，additive 向后兼容；随 M3-K1 同 PR 落地，同上 [CONTRACT-CHANGE-PACKET.md](../../delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/CONTRACT-CHANGE-PACKET.md)。）
+- （契约变更记录：2026-10-11 经契约门批准 `data.upcoming.preview` 由单 `{time,name}` 对象改多项数组 + 三态语义（非空数组 = 明日全量升序列表 / `[]` = 确认明日无项承载 AC6 空态话术 / `null`·缺省 = 未推送承载 AC7 未达态）——requirements v1.61 S-F1.1 AC2/AC6/AC7；示例值 `"time": "明早"` 旧漂移顺修，K1 裁决载荷钉 HH:MM、自然语言标签不入 wire；板端 seam 同形改（`home_p1.h`），板端对旧单对象形状保留一个过渡版的解析容忍（部署缓冲防御，不入契约形状定义）；随 261011-p1-home-v5-rework 同 PR 落地，见 [CONTRACT-CHANGE-PACKET.md](../../delivery/taskpackages/261011-p1-home-v5-rework/CONTRACT-CHANGE-PACKET.md)。）

### 1.4 控制类指令（OpenClaw 判意图后下发）

```

落地姿势：逐字 patch 过目 → `git apply`（CRLF 陷阱：契约文件为 CRLF，patch 生成与应用须同行尾；必要时 `--ignore-whitespace` 后人工 diff 复核）。

## 4. Impact（影响谁）

| 消费方 | 影响 |
|---|---|
| 板端 `home_upcoming.c` `kup_parse_preview` | 数组解析 + `[]` 语义 + 旧单对象过渡容忍（一个过渡版）；遇非 HH:MM time 维持拒包 |
| 板端 `home_p1.h`/`home_p1.c` | seam 同形改（数组+计数）；多项缓存、空态两态渲染（AC5/AC6 话术 + AC7 未达提示） |
| server `schedule.py` `home_upcoming_payload`（:433-448） | preview 多项化（`group_upcoming(model, tomorrow_iso, "00:00", school_start)` 全量逐项）；**明日无项改发 `[]`、不再发 `None`**——不改成 AC6 话术永不触发（plan R1） |
| `front_app.py` / `schedule_push.py` / `display_schema.py` / `mcp_server.cc` | 核证面预期零 diff（签名未变 / home 无 shape floor / 透传） |
| 既有冻结验收件 | server：test_schedule_producer_units.py preview 齿、test_m3k1_schedule_producer.acceptance.py M3K1-A3 frozen shape 齿；firmware：schedule_grid.acceptance.c A5 段 + seam 签名齿、p1_home_battery/p1_cjk_name inject 调用点——修订授权链 = 本 CCP id + requirements §9 v1.61 行（REVISION LOG 留痕） |
| 实测链路 | 笔顺（makemeahanzi）管道无涉；M3-K1 已跑通 home upcoming 端云链路 = 形状演进，须端云同批部署（ECS 先 + 同会话烧录，app+art 双分区）防 R2 错配窗口 |

**随包收口义务**：① `architecture-gate-evidence.md` 十节 ledger（§3 分类行用预审裁定表）；② `evidence/` 原始验证输出（producer 三样本回归：非空数组 / `[]` / `null`；板端四齿：数组、`[]`、旧单对象容忍、非 HH:MM 拒包）；③ CDQ 行登记（docs/CONTRACT-DECISION-QUEUE.md）+ docs/crq/tech/ 记账条目带 `cdq:` 指针；④ ADR 追加（preview 形状重定义 + 三态语义 = durable semantics 变化）；⑤ 契约 diff 人工逐字核对留痕（pass-oracle 第 4 关无脚本兜底，ARQ-260827-contract-mech-gate-gap 在册，如实降级）。

## 5. Human Question（请契约门裁定）

批准 `docs/contracts/interface-contract.md` §1.3⑥ `data.upcoming.preview` 由单 `{time,name}` 对象改多项数组并钉三态语义（非空数组 = 明日全量升序列表 / `[]` = 确认明日无项 / `null`·缺省 = 未推送），连同示例值 HH:MM 顺修（「明天」定名只落板端 chip 资产、不入 wire）、:155 seam 引用签名同形顺修与变更记录行 append，随 261011-p1-home-v5-rework 同 PR 落地？
