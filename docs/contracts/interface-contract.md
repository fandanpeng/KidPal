# KidPal 端云接口契约（E1 统一响应格式 + B1 课表 MD 结构）

> 与 OpenClaw 对齐用。定义 OpenClaw 如何返回"语音 + 屏幕显示指令"，以及课表 MD 的存储结构。
> 配合 [systems/kidpal/spec.md](../systems/kidpal/spec.md) §3.0.2（D3 笔顺格式已定稿）使用。

---

## 一、E1 统一响应格式

### 1.1 核心思路

OpenClaw 的 `POST /v1/chat/completions` 返回纯文本。为让板子同时获得"要念的话"和"要显示的内容"，约定：
**OpenClaw 在回复文本末尾，用一个特殊 fenced block 嵌入结构化显示指令**，薄桥 shim 负责切分成两路：

```
OpenClaw 回复文本（示例）
─────────────────────────────
"勇"字这样写，一共 9 笔，跟着我一笔一画写哦！    ← 语音部分（送 TTS）

```display                                          ← 显示指令（shim 提取，不念出来）
{ ...结构化JSON... }
```
─────────────────────────────
```

- **语音部分**（` ```display ` 之前的正文）→ shim 送 `/v1/tts` → mp3 → 设备播放
- **显示指令**（` ```display ` 块内 JSON）→ shim 提取 → 通过设备通道下发 → 板子渲染

> 为什么用 ` ```display ` 而不是别的： fenced code block 是 Markdown 标准，DeepSeek 生成稳定；`display` 作为语言标记，shim 用正则即可可靠切分，且不会误伤正文里的其他内容。

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
    "medians": [[[547, 677], ...], ...],
    "structure": "⿱甬力",
    "meaning": "勇敢；有胆量",
    "words": ["勇敢", "勇气"]
  }
}
```
板子行为：切到 P4 查字页，整屏显示"勇"，KEY 单击播笔顺动画（沿 medians），可显示拼音/笔画数/组词。

#### ② `schedule` —— 课表查询结果
```json
{
  "view": "schedule",
  "data": {
    "title": "今天下午",
    "items": [
      { "time": "14:00", "name": "钢琴课", "location": "琴行" },
      { "time": "16:30", "name": "英语培训", "location": "新东方" }
    ]
  }
}
```
板子行为：切到 P2 课表页，列表显示这些课程。

#### ③ `story_list` —— 故事列表
```json
{
  "view": "story_list",
  "data": {
    "items": [
      { "id": "2026-08-20-xiaotuzi", "title": "小兔子找朋友", "date": "08-20" },
      { "id": "2026-08-25-yonggan", "title": "勇敢的小明", "date": "08-25" }
    ]
  }
}
```
板子行为：切到 P3 故事页，显示标题列表，KEY 选择。

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
板子行为：在当前页/首页临时显示这段文字（字幕式），主要信息靠 TTS。

#### ⑥ `home` —— 回首页（无显示内容或结束）
```json
{ "view": "home" }
```
板子行为：回 P1 首页常显。

### 1.4 控制类指令（OpenClaw 判意图后下发）

控制指令也走 display 块，`view: "control"`：
```json
{ "view": "control", "data": { "action": "end_session" } }
```
| action | 含义 | 板子行为 |
|---|---|---|
| `end_session` | 孩子说"拜拜/不用了" | 结束会话，回首页常显 |
| `stop_speaking` | 孩子说"别讲了"（云端确认） | （MVP 主要靠本地喊停，此为云端补充确认） |

### 1.5 shim 的职责（薄，~200行内）

1. 收到设备 `/v1/chat` 的文字 → 转发 OpenClaw `/v1/chat/completions`
2. 拿到回复文本 → 用正则切出 ` ```display ` 块
3. 正文送 TTS → 音频回设备（小智协议 `tts` + Opus）
4. display JSON 经设备通道下发（小智协议 `mcp` 或自定义消息）→ 板子渲染
5. display JSON 里的 `view` 让板子知道切哪页

### 1.6 对 OpenClaw 的要求（system prompt 约束）

在调 `/v1/chat/completions` 时，通过 system prompt 让 DeepSeek **始终**按此格式回复：
> "你是儿童学习伙伴。每次回复分两部分：先用口语化、适合8-12岁孩子的话回答（这部分会被念出来）；然后在末尾附一个 ` ```display ` 代码块，内含屏幕显示指令 JSON，view 取 char/schedule/story_list/story_text/text/home/control 之一。"

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
remind_before_min: 15        # 默认提前15分钟提醒
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
- **frontmatter**：`remind_before_min` 提醒提前量；`child/timezone` 元信息。
- **每周固定课表**：`星期` 取 周一~周日；`时间` 用 `HH:MM-HH:MM` 或 `HH:MM`。查询"今天/明天有什么课"时按星期匹配。
- **一次性安排**：`日期` 用 `YYYY-MM-DD`，过期自动忽略（或归档）。
- **学校课程表**：可选区块，孩子问"今天第几节什么课"时用。
- **DeepSeek 解析**：直接读 MD，按"今天星期几/当前时间"筛选，生成自然语言答复 + `schedule` 显示指令。
- **提醒**：OpenClaw 的 cron 定时扫这两个表，到点前 `remind_before_min` 分钟生成提醒，写入提醒文件（shim `/v1/alerts` 读出）。

### 2.4 录入方式
- 家长通过 QQ 对 OpenClaw 说"每周六上午10点加一节美术班"→ OpenClaw 解析并改写此 MD。
- 或家长直接编辑此文件。

---

## 三、提醒文件结构（配套 B3，供 shim /v1/alerts）

OpenClaw cron 到点写入（如 `alerts/pending.json`）：
```json
{
  "alerts": [
    {
      "id": "2026-08-27-1400-gangqin",
      "fire_at": "2026-08-27T13:45:00+08:00",
      "title": "钢琴课提醒",
      "speech": "还有15分钟就要上钢琴课啦，记得带琴谱哦！",
      "view": { "view": "text", "data": { "text": "14:00 钢琴课（琴行）" } }
    }
  ]
}
```
设备轮询 `/v1/alerts` → shim 读到并清除已下发项 → 设备亮屏显示 + TTS 播报。

---

## 待 OpenClaw 确认点
1. 能否稳定按 system prompt 输出 ` ```display ` 块（DeepSeek 格式遵循度，需实测）。
2. 课表 MD 结构是否满足它的解析/改写（尤其"每周固定 + 一次性"两类）。
3. 提醒 cron 的可行性（OpenClaw 是否有定时任务能力，或用系统 cron + 脚本）。
