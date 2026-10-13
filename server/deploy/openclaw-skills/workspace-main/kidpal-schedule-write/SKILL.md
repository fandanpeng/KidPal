---
name: "kidpal-schedule-write"
description: "家长经 QQ 要求新增/修改课表时 (KidPal)：按 B1 契约格式改写 kidpal/schedule.md，改写前先备份，改写后跑 front 单源 parser 机检，通过才落即时重算 marker，机检不过立刻还原并引导家长重录。"
---

# KidPal 课表录入写入（B1 格式纪律 + 写后机检 + 即时重算）

家长经 QQ 对你说「每周六上午加一节美术班」「下周三加个牙医复诊」时，本 skill 是唯一的课表
写入流程。**课表唯一源 = `kidpal/schedule.md`**（main workspace 相对路径即真身；`/opt/kidpal/schedule.md`
是指向它的软链，front 读的正是这份）。**改 workspace 这份 = 改线上，不用同步、不用推送**；
改完 `diff /opt/kidpal/schedule.md kidpal/schedule.md` 应一致即可。收到任务直接执行本流程，
不需要向谁确认——但任何一步机检不过就**停手、还原、回家长**，不要「再试一次」式反复试改。

## 0. 先备份（不备份不许动笔）

编辑前**必须**先备份：`cp kidpal/schedule.md kidpal/schedule.md.bak`（或逐行记下本次 diff）。
还原键 = 这份备份——机检不过时用它把整份文件还原回改动前状态。**没有备份 = 一旦写坏无处可退**：
整份 schedule.md 解析失败会令首页「接下来」、提醒重算、课表问答全部静默停摆，家长侧零感知。

## 1. 写入格式纪律（B1 契约 §2.2/§2.3，逐字认表头）

**结构 = 三区块**（`# 标题` + 每周固定课表 + 一次性安排 +〔可选〕学校课程表）；YAML frontmatter
保留 `child` / `timezone` 元信息行，不要动。

### 每周固定课表区块——表头逐字范本（一字不能差）

```
| 星期 | 时间 | 课程 | 地点 | 备注 |
|------|------|------|------|------|
| 周一 | 16:30-17:30 | 钢琴课 | 琴行 | 带琴谱 |
```

- `星期` 列只写 **周一 / 周二 / 周三 / 周四 / 周五 / 周六 / 周日**（不要「星期一」「周1」「礼拜一」等变体）；
- `时间` 列写 **`HH:MM-HH:MM`**（如 `16:30-17:30`）或单点 **`HH:MM`**（如 `09:00`）；
- 固定五列，`备注` 列可空（要带的东西写这里，如「带琴谱」）。

### 一次性安排区块——表头逐字范本（一字不能差）

```
| 日期 | 时间 | 事项 | 地点 | 备注 |
|------|------|------|------|------|
| 2026-10-12 | 19:30-19:55 | 英语外教口语课 | 线上 | 提前预习 |
```

- `日期` 列只写**纯 `YYYY-MM-DD`**（如 `2026-10-12`）。**严禁带星期后缀**——`2026-10-12 周一`
  正是 2026-10-12 线上事故的坏格源：front 单源 parser 当场 `ValueError`、整份文件被拒、三触发
  全部静默停摆。**日期格不带任何星期字样**是本 skill 的头号纪律。
- `时间` 同上（`HH:MM-HH:MM` 或 `HH:MM`）；固定五列，`备注` 列可空。

### 通用纪律

- **表头必须用上列范本，不要复述成散文变体**（曾实测表头漂成「星期/日期」「课程/事项」即被
  parser 拒）。列数固定五列；缺列、多列、空表头都会致拒。
- **新增** = 在对应区块表格末尾追加一行；**修改** = 只改目标单元格、不动其它行。不要重排既有
  行、不要改表头、不要在区块外另起表格。
- 日期/时刻别心算：今天 `date '+%F'`、下周三 `date -d 'next wednesday' '+%F'`（时区 Asia/Shanghai）。

## 2. 写后机检（front 单源 parser 实跑，必过）

写完立刻用**钉死的解释器**（`/opt/kidpal/venv/bin/python3`——裸 `python3` 在 ECS 解析到
`/usr/bin/python3` = 3.6.8，跑不动脚本），**按绝对文件路径**加载 front 单源解析器
（`/opt/kidpal/front/gateway/schedule.py`），实际解析并确认新行在列：

```bash
/opt/kidpal/venv/bin/python3 -c '
import importlib.util
from pathlib import Path
spec = importlib.util.spec_from_file_location("sched", "/opt/kidpal/front/gateway/schedule.py")
m = importlib.util.module_from_spec(spec); spec.loader.exec_module(m)
md = Path("/root/.openclaw/workspace/kidpal/schedule.md").read_text(encoding="utf-8")
model = m.parse_schedule_md(md)
print("parse OK", len(model["weekly"]), len(model["onetime"]))
# 复证新行在当日「接下来」（日期/时刻/首节换成本次录入的目标值再跑）:
up = m.group_upcoming(model, "2026-10-12", "17:00", "08:10")
print(up)
'
```

- `parse_schedule_md(md_text)` 吃 **md 文本**（不是文件路径）——必须**先 `read_text()` 再传**；
- `group_upcoming(model, 日期, 现在, school_start)` 吃 **model dict**——用来复证新加的事项确实进入
  当日「接下来」列表；
- 完成判据 = 输出 `parse OK` **且**新行出现在 `group_upcoming` 结果里。**出现任何异常或 `ValueError`
  = 机检不过**（走步骤 3）。
- 说明：`KIDPAL_SERVER_DIR` 是脚本自读的 env、**不是** `sys.path` 注入，所以上面用按文件路径
  `spec_from_file_location` 加载（零 cwd/PYTHONPATH 依赖），别写成 `from gateway import schedule`。

## 3. 机检不过 = 还原优先（不进入开放自修循环）

**任何**机检失败（parser `ValueError`、新行不在列、脚本异常）→：

1. **立刻还原本次编辑**：`cp kidpal/schedule.md.bak kidpal/schedule.md`——回到改动前状态。宁可丢
   这一次录入，也不留一份令三触发全停摆的坏文件；
2. **QQ 回家长**：贴正确格式范本 + 指出**具体坏 token**（从 parser 报错里逐字取，如
   `invalid date token: '2026-10-12 周一'`），引导家长按格式重述一次；
3. **不要反复试改**——整理不是猜谜。写坏一次就还原 + 引导，别在坏文件上迭代。

## 4. 机检通过 = 立刻重算落 marker（设备「接下来」≈1 分钟刷新）

机检通过后才做即时刷新：用 **cron 同款 env 组**跑重算入口（它重算三档提醒 pending.json，并在
alerts 交接目录落**触发③ 即时刷新 marker `schedule.refresh`**，front 下一轮轮询 piggyback 消费后
把「接下来」推给在线设备）：

```bash
KIDPAL_SERVER_DIR=/opt/kidpal/front \
KIDPAL_SCHEDULE_MD=/root/.openclaw/workspace/kidpal/schedule.md \
KIDPAL_ALERTS_DIR=/opt/kidpal/alerts \
KIDPAL_SEMANTIC_JSON=/opt/kidpal/alerts/semantic.json \
/opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/reminder_schedule_lib.py
```

- 期望输出 `planned N alert slots -> …`；脚本成功后会落 `/opt/kidpal/alerts/schedule.refresh`，front
  约 1 分钟内即时推「接下来」、设备首页刷新；
- 脚本报错读不到语义件（`semantic.json` 缺）**仍算通过**——脚本对缺/坏语义输入降级机械兜底继续
  排程，marker 照落；
- 完成判据 = 机检 `parse OK` + 重算 `planned …` 输出 + marker `schedule.refresh` 在场。

## 边界

- 本 skill 只写 `kidpal/schedule.md`（+ 同目录 `.bak`）；**不碰** `/opt/kidpal/front/gateway/schedule.py`
  （冻结单源 parser，只读消费）、不碰三档配置、不产 display 块、不改板侧（workspace-kidpal）。
- 课表问答/展示走 `kidpal-schedule-answer` skill；提醒语义产物由 `kidpal-reminder-semantic` 回合产出；
  本 skill 只负责「把家长的课表改动安全写进 schedule.md 并让它即时生效」。
- 解析失败的机器可读告警与家长转告由 `kidpal-schedule-health` skill 承接，不在本 skill 内。
