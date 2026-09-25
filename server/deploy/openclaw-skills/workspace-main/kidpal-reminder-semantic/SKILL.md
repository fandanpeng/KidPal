---
name: "kidpal-reminder-semantic"
description: "提醒 cron 语义回合（KidPal）：读 kidpal/schedule.md 备注列，判「要带的」与文案，产 /opt/kidpal/alerts/semantic.json 供 reminder_schedule_lib.py 消费。"
---

# KidPal 提醒语义回合（semantic.json 生产侧）

本 skill 只服务一类回合：**OpenClaw cron 唤起的提醒语义回合**（任务名 `kidpal-reminder-semantic`，
每 15 分钟一次、落在 main agent）。你唯一的产出 = 原子写 `/opt/kidpal/alerts/semantic.json`，
消费方 = 确定性脚本 `reminder_schedule_lib.py`（每 15 分钟跑的 `kidpal-reminder-plan` 用它把
提醒排得更细：带物拆分、文案 override）。收到任务消息后**直接执行本流程**，不需要向谁确认。

## 步骤

1. **读 `kidpal/schedule.md`**（main workspace 相对路径即真身；脚本 env `KIDPAL_SCHEDULE_MD`
   当前也指向同一文件——若未来该 env 改指他处，以 env 实际指向为准核对本 skill 的读路径）。
   完成判据：拿到 B1 三区块（学校课程表 / 每周固定课表 / 一次性安排）。

2. **筛可提醒行**：「每周固定课表」「一次性安排」两区块中**有开始时刻**的行。
   「学校课程表」区块**一律不产条目**（在校例行，不提醒）。过期的一次性安排（日期 < 今天）
   不产条目——判过期用 `date '+%F'` 取今天日期，别心算。

3. **逐行产条目**，顶层对象、键 = `entry_key`，必须与脚本算法**逐字对齐**（对不上 = 你的判断
   挂不到任何事项上，静默落空）：
   - 每周固定行：`weekly|<星期数字 1-7>|<start>-<end>|<课程名>|<地点>`（无结束时刻写
     `<start>`；地点空留空段，如 `weekly|3|19:00-20:00|英语|`）
   - 一次性安排行：`onetime|<YYYY-MM-DD>|<start>-<end>|<事项名>|<地点>`
   - **星期数字 = 该行「星期」列的映射（周一=1…周日=7），不是今天的星期**——取自课表行
     本身，与今天星期几无关，严禁用 `date` 的输出填这里。
   - **不发明别的键、不写 id、不写 fire_at**（时刻/id/排程归脚本）。

4. **判「备注里哪部分算要带的」（bring）——本回合的核心价值**：从备注原文挑出**孩子要带
   出门的物件名词**，逐个进 `bring` 数组，用备注**原词**、不做同义改写、不补修饰；穿着类
   也算要准备的（「穿球衣、带水壶」→ `["球衣","水壶"]`）。时间地点描述、家长待办（「需
   签字」）、状态说明（「请假」）、纯问候语**不算**携带物。**备注空 = 省略该字段，严禁凭空
   发明要带的东西**。备注为「带 A 和 B」类 → 拆成 `["A", "B"]`（这正是脚本机械兜底做不到
   的拆分，本回合存在的意义）。
   **已知局限（消费端脚本冻结语义，勿误判本 skill 失效）**：判为「非携带物」的备注即使
   省略 bring，脚本仍会把备注整条当单项念出（`bring_items` 的兜底分支无法表达「无携带物」
   的抑制）——本回合能做的是拆分与原词，不能抑制非携带备注的播报。

5. **speech / title 的取舍**：脚本机械兜底对**非携带物备注**会产生病句（备注「提前15分钟
   准备」→「记得带上：提前15分钟准备」，live 实证 2026-09-25）——因此：备注被判为
   **非携带物**（时间/准备/待办/状态类）时**必须写 speech 覆盖**，用**档位无关**表述
   （「18:40 的书法课，提前 15 分钟准备哦」，禁用「今天/明天」——speech 单值三档共用，
   脚本第 372-374 行）；备注**确有携带物**时 speech/title 一般省略（脚本按三档各自生成
   更自然）。字段 ≤60 / ≤90 字。

6. **原子写** `/opt/kidpal/alerts/semantic.json`：python3 临时文件 + `os.replace`，写完
   `chmod 600`（对齐 pending.json 权限）。**全量重写**（每次从当前 schedule.md 重算整个
   文件，不增量合并）。写完自检：`python3 -c "import json;json.load(open('/opt/kidpal/alerts/semantic.json'))"`
   通过才算完成。

## 失败与边界

- schedule.md 缺失 / 读取失败：**不写文件**（保留上次产物，脚本继续兜底），在回合输出里
  说明原因即可，不要重试超过一次。
- 本回合不产 display 块、不面向孩子播报、不改 schedule.md、不算时刻、不生成 id、不写
  pending.json、不改三档配置（`openclaw-reminder.config.yaml`）——那些全是脚本的。
- 本 skill 在 main agent 上执行；板侧 kidpal agent（workspace-kidpal）与设备语音链路
  完全无涉，不要向设备腿写任何东西。
