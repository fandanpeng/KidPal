# KidPal 端云接口契约（E1 统一响应格式 + B1 课表 MD 结构）

> 与 OpenClaw 对齐用。定义 OpenClaw 如何返回"语音 + 屏幕显示指令"，以及课表 MD 的存储结构。
> 配合 [docs/requirements.md](../requirements.md) F3（查字窗·笔顺演示需求）使用；笔顺 JSON 格式（D3）以本契约为权威。
> **方向 B 修订（2026-09-03 生效）**：端云通信转 WebSocket + Opus 流式（小智协议剖面，见 §四）；提醒由轮询改推送（§三重写）；本修订经契约门四题 human 批准（CDQ-260902，2026-09-03）+ spec v0.2 首次对抗审（260902-arch-review-kidpal-spec-v0.2）同 PR 原子落地。修订前 HTTP 形态信封草案存档：`delivery/changes/arch/260902-arch-review-shim-api-field-envelope/CONTRACT-CHANGE-PACKET.v1-http-field-envelope.superseded.md`。

---

## 一、E1 统一响应格式

### 1.1 核心思路

OpenClaw 的 `POST /v1/chat/completions` 返回纯文本。为让板子同时获得"要念的话"和"要显示的内容"，约定：
**OpenClaw 在回复文本末尾，用一个特殊 fenced block 嵌入结构化显示指令**，流式网关负责切分成两路：

```
OpenClaw 回复文本（示例）
─────────────────────────────
"勇"字这样写，一共 9 笔，跟着我一笔一画写哦！    ← 语音部分（送 TTS）

```display                                          ← 显示指令（网关提取，不念出来）
{ ...结构化JSON... }
```
─────────────────────────────
```

- **语音部分**（` ```display ` 之前的正文）→ 网关送 TTS（edge-tts）→ 句级流水线（LLM token 流→句切分→TTS）→ 下行 Opus 帧 → 设备播放
- **显示指令**（` ```display ` 块内 JSON）→ 网关提取并机械校验 → 经 `mcp` kidpal/display 工具调用下发 → 板子渲染

> 为什么用 ` ```display ` 而不是别的： fenced code block 是 Markdown 标准，DeepSeek 生成稳定；`display` 作为语言标记，网关用正则即可可靠切分，且不会误伤正文里的其他内容。

### 1.2 display 指令顶层结构

```json
{
  "view": "<视图类型>",
  "data": { ... },
  "speech_hint": "可选，覆盖TTS用的文本（默认用正文）"
}
```

`view` 决定板子切到哪个页面/怎么渲染。取值如下。

### 1.3 各 view 类型定义

#### ① `char` —— 查生字（接 D3 已定稿格式）
```json
{
  "view": "char",
  "data": {
    "char": "勇",
    "pinyin": "yǒng",
    "radical": "力",
    "strokeCount": 9,
    "strokes": ["M 547 677 Q ...", "..."],
    "medians": [[[547, 677], [547, 500], ...], "..."],
    "structure": "⿱甬力",
    "meaning": "勇敢；有胆量",
    "words": ["勇敢", "勇气"]
  }
}
```
板子行为：切到 P4 查字页，整屏显示"勇"，KEY 单击播笔顺动画（沿 medians），可显示拼音/笔画数/组词。

#### ② `schedule` —— 课表页「一周总览」网格
完整网格载荷（front 计算后下发设备）：
```json
{
  "view": "schedule",
  "data": {
    "week": {
      "courses": [
        { "day": 0, "slot": 0, "name": "语文", "flags": 0 },
        { "day": 0, "slot": 2, "name": "口语", "flags": 1 }
      ]
    }
  }
}
```

- `day` = 0..6、`slot` = 0 上午 / 1 下午 / 2 晚上。**day 为 0 基：`day=0` 对应 B1「星期」列「周一」（数组下标自然序）**；一格多门 = 同 (day, slot) 多条，逐门列出。
- `flags` 位掩码：bit0 = ※ 临时安排（来自 B1「一次性安排」区块）、bit1 = 星标呼应（该课为 group_upcoming 判定的当前「接下来」标记项）。**备注不进网格载荷**（网格/首页只放课程名；备注露面处 = 语音答复 S-F1.3 AC4 + 提醒 speech S-F1.4）。
- **生产纪律（2026-09-21 契约门裁定）**：网格由 front 机械计算（schedule.py 唯一解析器，B1 三区块），**不由 LLM 生成**；「帮我显示课表」时 LLM 只发意图薄载荷 `{ "view": "schedule", "data": {} }`，front 截获后展开成完整网格再下发。旧「不进反白/星标等 UI 语义态」理念就此修订——※/星标判定信息（一次性安排区块归属、当前接下来标记项）只有云端可知、spec S-F1.2 AC3/AC4 要求板上呈现，故以 `flags` 位承载。
- **「今天」列不在载荷内**：板端按设备本地日期零点切换自行推导高亮列（spec S-F1.2 补充规则）。
- **`data.restore`**（可选布尔，additive——缺省（absent）或显式 `false` 时设备行为完全不变）：`true` = 恢复性载荷，只填网格缓存、不接管前台（P2 在屏时仅重绘本页）。降级双面注记：restore 载荷**板端入板后**解析失败 = 板端静默降级（记日志、缓存零变化、不弹提示、不动前台）；**front 校验面 V27**（脏块拒绝/降级提示、不入板盲渲染）在入板前独立生效——两层降级互不替代。
- **生产纪律（谁可置 `data.restore`）**：仅 front 后台附带推送路径可置 `restore`（触发时机属行为策略，权威承载 = arch §5.2 / ADR-005，不进契约）；意图薄载荷 `{view:schedule,data:{}}` 的展开（`expand_schedule_intent_payload`）恒不置——展开产物 = 主动展示、接管前台（M3-K3 冻结行为）。
- 板子行为：切到 P2 课表页渲染一周总览网格（学校课逐节、课外课逐门小字竖排、※/星标按 flags 渲染、今天列整列高亮）；`data.restore: true` 时除外——只填网格缓存、不接管前台，P2 在屏时仅重绘本页；薄载荷不直达设备（front 截获展开）；畸形/旧形状 data 由 front 机械校验拒绝并降级提示，不入板盲渲染（V27）。

- （契约变更记录：2026-09-21 经契约门批准重定义本 view data 形状——旧 `{title, items:[{time,name,location}]}` 列表形即废；定稿 = 周网格扁平形 + flags 位掩码 + 意图薄载荷，随 M3-K1 同 PR 落地，见 [CONTRACT-CHANGE-PACKET.md](../../delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/CONTRACT-CHANGE-PACKET.md)。）
- （契约变更记录：2026-09-23 经契约门批准新增可选 `data.restore` 恢复性推送语义——additive 字段 + 既有板子行为条款 modifying；随 260923-m3-k7-schedule-grid-restore 同 PR 落地，见 [CONTRACT-CHANGE-PACKET.md](../../delivery/changes/arch/260923-arch-review-schedule-grid-cold-boot/CONTRACT-CHANGE-PACKET.md)。）

#### ③ `story_list` —— 故事列表
```json
{
  "view": "story_list",
  "data": {
    "total": 3,
    "items": [
      { "id": "2026-08-20-xiaotuzi", "title": "小兔子找朋友", "date": "08-20" },
      { "id": "2026-08-25-yonggan", "title": "勇敢的小明", "date": "08-25" }
    ]
  }
}
```
板子行为：切到 P3 故事页，显示标题列表，KEY 选择。

- **`total`**（可选，整数）：孩子已录故事总数 = 故事果计数，由云端计算下发。板子据此渲染 P3 右上角故事果计数器（×N）与盖章帧「第 N 颗」（S-F2.1/S-F2.4）。**缺省时板子回退用 `items.length`**（向后兼容）。注意 `total` 是 stories 全集计数，若 `items` 分页/截断，不得以 `items.length` 顶替 `total` 作正式数据源。
- （契约变更记录：2026-08-29 经契约门批准新增 `total`，additive 可选字段，见 delivery/changes/arch/260829-arch-review-kidpal-spec-stories。）

#### ④ `story_text` —— 故事正文（回放/刚存好）
```json
{
  "view": "story_text",
  "data": {
    "id": "2026-08-27-maoxian",
    "title": "我的大冒险",
    "text": "从前有一只小猫……（正文）"
  }
}
```
板子行为：P3 故事页显示标题+正文（滚动），或仅靠 TTS 朗读。

#### ⑤ `text` —— 纯文本回答（无特殊视图）
```json
{
  "view": "text",
  "data": { "text": "现在是上午10点24分，今天天气不错。" }
}
```
板子行为：在当前页/首页临时显示这段文字（全文上屏滚动，见 spec S-F4.1），主要信息靠 TTS。

#### ⑥ `home` —— 回首页（无显示内容或结束）
```json
{ "view": "home" }
```
板子行为：回 P1 首页常显。

**可选 `data.upcoming` 承载「接下来」（2026-09-21 新增，additive——缺 `data` 时设备行为完全不变）**：
```json
{ "view": "home", "data": {
    "upcoming": {
      "day_key": "今天",
      "items": [ { "time": "14:00", "name": "钢琴课" } ],
      "preview": { "time": "明早", "name": "春游" }
    }
  } }
```

- 形状与板端 `home_p1.h` 冻结注入 seam（`hp1_item_t {time,name}` / `kidpal_home_p1_schedule_inject(day_key, items, next_day_preview)`）对齐——payload 即 seam 注入参数的 JSON 化。
- 分组口径 = 决策 26「接下来」分组（学校课合并「上学」、培训班/一次性逐项、最近项最醒目、再下可带明日预告）；**producer = front 机械计算**（schedule.py `group_upcoming` 单源），OpenClaw（LLM）专注话术、不产此载荷（2026-09-21 契约门裁定）。推送时机（WS 建连 / 每日零点 / 新建日程即时）属行为策略，权威承载 = arch §5.2 场景① + §10 决策 26，不进契约。

- （契约变更记录：2026-09-21 经契约门批准新增可选 `data.upcoming` 承载首页「接下来」，additive 向后兼容；随 M3-K1 同 PR 落地，同上 [CONTRACT-CHANGE-PACKET.md](../../delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/CONTRACT-CHANGE-PACKET.md)。）

### 1.4 控制类指令（OpenClaw 判意图后下发）

控制指令也走 display 块，`view: "control"`：
```json
{ "view": "control", "data": { "action": "end_session" } }
```
| action | 含义 | 板子行为 |
|---|---|---|
| `end_session` | 孩子说"拜拜/不用了" | 结束会话，回首页常显 |
| `stop_speaking` | 孩子说"别讲了"（云端确认） | （MVP 主要靠本地喊停，此为云端补充确认） |
| `start_recording` | 孩子说"我要讲故事"（云端判意图） | 网关切该会话为累积态（不按句送对话）、设备切 REC 录音演出（manual 聆听模式）；误判回退路径见 spec S-F2.2（2026-09-03 新增，方向 B 故事录入流式） |

### 1.5 网关的职责（薄，零自研业务逻辑红线不变）

1. 承接设备会话：wss 常驻连接，握手 `Authorization: Bearer <device-token>` 强制鉴权（**token 未设 = 拒绝服务**，不留 validation-mode 后门）。
2. 上行：Opus 帧 → 服务端 VAD 切句 → 云 ASR → 文字 → 转发 OpenClaw（OpenAI 兼容端点，流式）。
3. 切分与校验：拿到回复文本用正则切出 ` ```display ` 块，**机械 schema 校验**（view ∈ 白名单 char/schedule/story_list/story_text/text/home/control、data shape/大小上限、脏 JSON 拒绝并降级友好提示）；schedule 薄载荷（`data:{}`）由 front 截获展开成完整周网格再下发（§1.3②），旧列表形状 schedule data 拒绝并降级提示。课表解析/网格计算仍由 schedule.py 唯一解析器单源承载（arch §5.1.2 明列课表解析组件归 front）——此为机械计算与校验，非自研业务逻辑，**零自研业务逻辑红线不变**。
4. 正文送 TTS（edge-tts）→ 句级流水线 → 下行 Opus 帧；display JSON 经 `mcp` kidpal/display 工具调用下发（小智原生 alert/custom 通道弃用）。
5. 提醒定时推送：`mcp` kidpal/alert，送达/出队语义见 §三。

### 1.6 对 OpenClaw 的要求（system prompt 约束）

在调 `/v1/chat/completions` 时，通过 system prompt 让 DeepSeek **始终**按此格式回复：
> "你是儿童学习伙伴。每次回复分两部分：先用口语化、适合8-12岁孩子的话回答（这部分会被念出来）；然后在末尾附一个 ` ```display ` 代码块，内含屏幕显示指令 JSON，view 取 char/schedule/story_list/story_text/text/home/control 之一。涉及课表时：孩子问课表只用文字回答（备注类提醒在话术里说，如『舞蹈课记得带护膝』），不要生成课表 JSON；需要切到课表页时只发 `{"view":"schedule","data":{}}` 意图薄载荷，完整周网格由网关展开（2026-09-21 契约门改写，见 §1.3②）。"

---

## 二、B1 课表 MD 结构

### 2.1 设计原则
- 人和 AI 都好读好写（家长可能直接改，DeepSeek 要解析查询）
- 用 Markdown 表格承载结构化数据，YAML frontmatter 存元信息
- 支持"每周固定课表"和"一次性培训班/活动"两类

### 2.2 `schedule.md` 结构定稿

````markdown
---
child: 小明
timezone: Asia/Shanghai
remind_before_min: 15        # ⚠️ 已废止（legacy，不再消费）——提前量提醒被三档阶梯取代（2026-09-03 契约门裁定标废止，见 §三）
---

# 小明的课表

## 每周固定课表
| 星期 | 时间 | 课程 | 地点 | 备注 |
|------|------|------|------|------|
| 周一 | 16:30-17:30 | 钢琴课 | 琴行 | 带琴谱 |
| 周三 | 19:00-20:00 | 英语培训 | 新东方 | |
| 周六 | 10:00-11:30 | 美术班 | 少年宫 | 带画笔 |

## 一次性安排
| 日期 | 时间 | 事项 | 地点 | 备注 |
|------|------|------|------|------|
| 2026-08-30 | 14:00-15:00 | 牙医复诊 | 口腔医院 | |
| 2026-09-05 | 09:00 | 开学报到 | 学校 | 带暑假作业 |

## 学校课程表（可选，平时在校）
| 星期 | 第1节 | 第2节 | 第3节 | 第4节 | 第5节 | 第6节 |
|------|-------|-------|-------|-------|-------|-------|
| 周一 | 语文 | 数学 | 英语 | 体育 | 美术 | 自习 |
````

### 2.3 字段说明与解析规则
- **frontmatter**：`child/timezone` 元信息；`remind_before_min` **已废止、不再消费**（2026-09-03 契约门裁定，保留字段为历史可读）。
- **每周固定课表**：`星期` 取 周一~周日；`时间` 用 `HH:MM-HH:MM` 或 `HH:MM`。查询"今天/明天有什么课"时按星期匹配。
- **一次性安排**：`日期` 用 `YYYY-MM-DD`，过期自动忽略（查询按当前时间过滤；MVP 不归档，增长治理留后续）。
- **学校课程表**：可选区块，孩子问"今天第几节什么课"时用。
- **DeepSeek/OpenClaw 课表职责（2026-09-21 契约门改写）**：读 MD 生成自然语言答复（查询按"今天星期几/当前时间"筛选；备注在话术里读回）；**不再产 `schedule` 显示指令 JSON**——需要切课表页时只发意图薄载荷 `{"view":"schedule","data":{}}`（§1.3②），完整周网格与首页「接下来」载荷均由 front 机械计算（schedule.py 单源，§1.3②/§1.3⑥）。
- **提醒（2026-09-03 更新）**：见 §三——OpenClaw cron 算三档时刻写 `alerts/pending.json`，网关到点经 `mcp` kidpal/alert 推送、渲染回执出队；提前量模型（`remind_before_min`）废止。
- **备注列**：携带内容（如"带画具"）复用备注列承载，"备注里哪部分算要带的"由 OpenClaw system prompt 约定（2026-09-02 定，表格主体零变更）。

### 2.4 录入方式
- 家长通过 QQ 对 OpenClaw 说"每周六上午10点加一节美术班"→ OpenClaw 解析并改写此 MD（主通道，自知改写、即时触发三档重算与推送）。
- 或家长直接编辑此文件（副通道，无即时推，靠下次连接/每日跨天兜底刷新）。

---

## 三、提醒推送与出队语义（配套 B3，2026-09-03 方向 B 重写，取代原「提醒文件 + 轮询读到即清」）

OpenClaw cron 按课表算三档时刻并写 `alerts/pending.json`（同机共享文件，交网关）；网关 push helper 轮询到点、经 **mcp 工具调用 `kidpal/alert`**（JSON-RPC tools/call）推送，载荷形状：

```json
{ "id": "<reminder-instance-id>", "fire_at": "ISO8601+08:00",
  "title": "明日美术课", "speech": "明天美术课，别忘了带画具！",
  "view": { "view": "text", "data": { "text": "…" } } }
```

- **`id`** = 确定性散列（schedule 条目 + 档位 + 事件日期），OpenClaw cron 首次生成、**跨重推/重算不变**（重算复用、不重生成）。
- **`speech`** = 排程时预合成的 TTS 文案，音频随推送下发（走下行 Opus/tts 通道）；合成失败/超时 → 降级「仅弹窗+提示音」、不阻塞投递。
- 设备收到 → id 记入易失 known-id 集（幂等去重、屏蔽重复入队）→ 排队 → 渲染 `state-reminder` 富弹窗（**新固件面**，小智原生 alert 处理器不适用、通道弃用）→ **渲染完成回 JSON-RPC 回执**（渲染即确认、即出队）。
- **重试到确认**：网关 60 秒超时未收回执即重推，单档上限 3 次，不做重连即重推；过期仍未送达 = 静默作废不补发（儿童产品不追催）。
- 原「设备轮询 `/v1/alerts` → shim 读到并清除已下发项」形态**废止**（HTTP 轮询端点随 Pod S 台阶退役，见 spec §3.0.0「迁移与既有资产」）。

---

## 四、端云协议剖面（小智协议消息子集，2026-09-03 新增，方向 B）

> 传输层：wss 常驻连接（自签证书 ≥10 年有效期，见 spec 决策 22）；设备鉴权强制（握手 `Authorization: Bearer <device-token>`，token 未设 = 拒绝服务）。本节为格式事实源，spec §6.1 是通道概览。

- **hello 握手**：设备自报 `{format:"opus", sample_rate:16000, channels:1, frame_duration:60}`；服务端回 `session_id` + `audio_params`（下行采样率/帧长，默认 24000/60，设备按需重采样）。
- **Opus 二进制帧**：上行 16kHz/60ms 单声道（BinaryProtocol v2/v3）；下行 24kHz 协商。
- **`stt`/`tts` 状态消息**（云→端）：`stt` 识别文字上屏；`tts` start/sentence_start（带同步文字）/stop 播报状态机。
- **`abort` 消息**（端→云）：喊停/打断 → 云端取消生成（本地 AEC+VAD 即时停播仍是主地位）。
- **`kidpal_session_ctx` 消息**（端→网关，KidPal 私有扩展消息、非小智原生子集；2026-09-14 新增）：设备在**每次发起会话时、首帧音频前**发送 `{"type":"kidpal_session_ctx","source_page":"story"}`——首连时序上在 server hello 之后，wss 常驻连接的通道复用后续会话同样发送，重复发送安全（收敛入口幂等）。`source_page` 取值闭集 = `story` / `home` / `schedule`（story = P3 星星记录本按 KEY 发起的记录语境会话）。**网关消费并拦截、不下透引擎**：`source_page=="story"` 时 front 即切该会话累积态（与 §1.4 `control/start_recording` 收敛同一幂等入口，重复触发不重复开会话）；其余取值现版本无行为（闭集预留，供未来切页语境扩展）。无论取值，整条消息一律由网关拦截。省略整条消息 = 普通会话，向后兼容。设备 manual 聆听仍经既有 listen 消息承载，引擎不感知本信号。
- **`mcp` 工具调用**（云→端）：两种载荷——`kidpal/display`（载 E1 display JSON，view 白名单见 §1.3；网关下发前机械 schema 校验）与 `kidpal/alert`（载 §三 提醒条目）；设备 JSON-RPC 回执 = 送达确认（提醒出队 ack）。小智原生 alert/custom 通道**弃用**。
- **`llm`/`system` 消息**：小智原生——`llm`（emotion）本项目 UI 重做不消费（弃用）；`system`（reboot）留二期 OTA。
- **会话连续性承载**：`type:openai` 接入默认无状态；多轮连续性承载方案（网关侧状态 vs 每轮透传历史）以**部署期首验**定案（两轮追问连续性探针；失败 → 网关适配传会话标识/历史，或 A4 连续追问降级单轮问答并记账，ARQ-260902-multiturn-continuity）。
- **provider 配置硬约束**：对 OpenClaw 的流式请求**禁带 `max_tokens`**（实测触发 agent incomplete-turn 报错）。

---

## 待 OpenClaw 确认点
1. 能否稳定按 system prompt 输出 ` ```display ` 块（DeepSeek 格式遵循度，需实测；方向 B 网关切分路径不复用 v0.1 HTTP 期的 12/12 旧结论，须复验）。
2. 课表 MD 结构是否满足它的解析/改写（尤其"每周固定 + 一次性"两类）。
3. 提醒 cron 的可行性：✅ 已实证（2026-09-02，OpenClaw 原生 cron 在跑、`cron --message` 可定时触发 agent 回合；已有 class-reminder 类任务运行记录）。
