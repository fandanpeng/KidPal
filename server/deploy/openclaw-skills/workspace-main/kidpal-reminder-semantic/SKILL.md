---
name: "kidpal-reminder-semantic"
description: "提醒 cron 语义回合（KidPal）：读 kidpal/schedule.md 备注列，判「要带的」与文案，产 /opt/kidpal/alerts/semantic.json 供 reminder_schedule_lib.py 消费。"
---

# KidPal 提醒语义回合（semantic.json 生产侧）

本 skill 只服务一类回合：**OpenClaw cron 唤起的提醒语义回合**（任务名 `kidpal-reminder-semantic`，
每 15 分钟一次、落在 main agent）。你唯一的产出 = 原子写 `/opt/kidpal/alerts/semantic.json`，
消费方 = 确定性脚本 `reminder_schedule_lib.py`（每 15 分钟跑的 `kidpal-reminder-plan` 用它把
提醒排得更细：带物拆分、文案 override）。收到任务消息后**直接执行本流程**，不需要向谁确认。

## 本回合由 cron trigger 变更驱动唤起（261031 起）

本 job 带一个无模型的静默 trigger（`reminder-semantic.trigger.js` → `reminder_semantic_gate.py`）：
每 15 分钟先跑判定器，**内容没变时 `fire:false` 不起本回合**（零 token），只有输入变化才把
本回合并到队列并附触发原因 message。

- **触发原因是提示，不是事实**：message 只说明「哪个输入变了」，你仍必须按**当前实际文件
  状态**重判全部流程——不要照抄 message 里的推断，也不要因为收到触发就跳过第 1 步的读盘。
- **semantic.json 新鲜度合同**：落盘的 `semantic.json` 必须**新于**它的四个输入——
  `kidpal/schedule.md`、本 skill（SKILL.md）、`gateway/schedule.py`、`reminder_schedule_lib.py`；
  另须满足 **60s 竞态余量**（semantic mtime 比 schedule mtime 新出 60 秒以上）——否则下一轮
  判定器会认为产物过期并再次 fire（多跑一轮只花 token，不会漏跑）。判定器看的是 **mtime**，
  所以「内容相同就跳过写」会让你每周期被判定为落后；本回合按全量重写语义产物是正确姿势。
- 无课表（`schedule.md` 缺失）时判定器恒 `fire:false`，本回合不会被唤起；若手动执行，按
  「失败与边界」不写文件、保留上次产物即可。

## 步骤

1. **读 `kidpal/schedule.md`**（main workspace 相对路径即真身；脚本 env `KIDPAL_SCHEDULE_MD`
   当前也指向同一文件——若未来该 env 改指他处，以 env 实际指向为准核对本 skill 的读路径）。
   完成判据：拿到 B1 三区块（学校课程表 / 每周固定课表 / 一次性安排）。

   **解析必须传文本**：`gateway.schedule.parse_schedule_md` 收的是 MD **文本**，传路径会
   静默解析出空 weekly/onetime（陷阱实证 2026-10-21）；正确姿势永远是
   `parse_schedule_md(open(PATH, encoding='utf-8').read())`。解析后先断言 weekly 行数 > 0，
   为 0 时停手排查，不要落一份空 semantic.json 把上份产物覆盖掉。

2. **筛可提醒行**：「每周固定课表」「一次性安排」两区块中**有开始时刻**的行。
   「学校课程表」区块**一律不产条目**（在校例行，不提醒）。过期的一次性安排（日期 < 今天）
   不产条目——判过期用 `date '+%F'` 取今天日期，别心算；或直接依赖解析器实算拿到的 rows（
   第 3 步跑的 `gateway.schedule`+`entry_key`）自带的窗口语义：消费端 `_rows_in_scope` 对
   过期一次性安排永不生成、对 school 区块永不生成（契约 §2.3），按此口径不必另行算日期。

3. **逐行产条目**，顶层对象、键 = `entry_key`，必须与脚本算法**逐字对齐**（对不上 = 你的判断
   挂不到任何事项上，静默落空）：
   - 每周固定行：`weekly|<星期数字 1-7>|<start>-<end>|<课程名>|<地点>`（无结束时刻写
     `<start>`；地点空留空段，如 `weekly|3|19:00-20:00|英语|`）
   - 一次性安排行：`onetime|<YYYY-MM-DD>|<start>-<end>|<事项名>|<地点>`
   - **星期数字 = 该行「星期」列的映射（周一=1…周日=7），不是今天的星期**——取自课表行
     本身，与今天星期几无关，严禁用 `date` 的输出填这里。
   - **不发明别的键、不写 id、不写 fire_at**（时刻/id/排程归脚本）。
   - **entry_key 用解析器实算，不靠手工拼**：key 必须与脚本算法逐字相同，故直接跑
     `KIDPAL_SERVER_DIR=/opt/kidpal/front /usr/bin/python3.11`，按 `gateway.schedule`
     （sys.path 加 `/opt/kidpal/front`）解析 schedule.md 拿 rows，再对每行调
     `reminder_schedule_lib.entry_key(section, row)` 打印 key。**不要用 `python3`**：
     /opt/kidpal 下默认 `python3` = 3.6，`gateway/schedule.py` 的 `X | None` 注解会
     SyntaxError 崩掉；3.11 才行。

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

6. 判完所有行后**按脚本逐字复算一遍落盘前的 mid-flight 校验样本**（低成本、必做）：
   跑 `/usr/bin/python3.11`，`sys.path` 加 `/opt/kidpal/front` 与 `/opt/kidpal/openclaw-patch`，
   对每条会产生提醒的 key 调
   `reminder_schedule_lib.plan_reminders(model, now_iso, config=lib.load_ladder_config('/opt/kidpal/openclaw-reminder.config.yaml'), semantic=<你准备写的 dict>)`，
   用 `now_iso='2026-10-17T01:02:00+08:00'` 一类含真实日期的时刻（**now_iso 必须带 +08:00
   偏移**，裸 datetime 会被 `_parse_now` 判非法；同日早于 07:45 的 now 会掩掉 morning 档，
   故取一个晚于早上档的当天时刻更稳），抽查：night 档 title 以「明天 」开头、speech 以
   「明天」开头且含你给的 bring 原词；morning 档以「今天」开头；soon 档 title 以
   `<start> <name>` 开头、speech 以 `<start> 有` 开头；speech ≤90、title ≤60；每条 id
   经 `lib.reminder_id(key, ladder, event_date)` 可复现。任一抽查不符 = 该 key 的字段没
   被脚本认到（多半 key 拼错或字段被 `load_semantic` 静默丢弃），改到全符再落盘。

7. **原子写** `/opt/kidpal/alerts/semantic.json`：python3 临时文件 + `os.replace`，写完
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
