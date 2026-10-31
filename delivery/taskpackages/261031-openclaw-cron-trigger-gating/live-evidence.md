# live-evidence · 261031-openclaw-cron-trigger-gating

> 本文件 = live 验收的真实执行记录。执行时刻 2026-10-31 20:00–21:00 CST，ECS 8.137.91.16，OpenClaw 2026.8.1。
> 执行人 = 主线程（ECS 写操作按包内约定归主线程/human）。部署来源 = origin/main `16cf405f`（PR #49 合并），外加本次 live 实测驱动的 `cron_triggers.py` 热铺修复（已独立 commit，随修复批回流 main）。

## L1 · job 结构在位

```
$ /opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/cron_triggers.py status
OK kidpal-reminder-semantic: trigger 内联文本与 /opt/kidpal/openclaw-patch/triggers/reminder-semantic.trigger.js 一致（once=None）
OK kidpal-schedule-health: trigger 内联文本与 /opt/kidpal/openclaw-patch/triggers/schedule-health.trigger.js 一致（once=None）
STATUS_RC=0
```

```
$ openclaw cron show kidpal-reminder-semantic --json
{ id: 26c6c771-c83e-4883-9b44-3a742023e481, schedule.kind: every, schedule.everyMs: 900000,
  agentId: main, sessionTarget: isolated, payload.kind: agentTurn, trigger.script: <薄壳文本在位> } → rc=0
$ openclaw cron show kidpal-schedule-health --json
{ id: 8c9b98a0-c461-471b-a02a-66317e59dccb, schedule.everyMs: 900000, trigger.script: <在位> } → rc=0
$ openclaw config get cron.triggers.enabled
Config path is valid but unset: cron.triggers.enabled. The runtime default applies → rc=0（默认开启）
```

判据全部满足：两条 job 含 `trigger.script`、无 `once=true`、频率/agent/session/payload message 逐字不变、全局开关未被显式关闭。

## L2 · 静默拍零模型调用

```
$ openclaw cron show kidpal-reminder-semantic --json  (20:53 CST)
state.triggerEvalCount=4, state.lastTriggerFireAtMs=1793448074907(20:01:14), state.lastRunAtMs=1793448021642(20:00:21) → rc=0
$ openclaw cron show kidpal-schedule-health --json  (20:53 CST)
state.triggerEvalCount=4, 无 lastTriggerFireAtMs, state.lastRunAtMs=1793447226226(19:47，触发挂载前的旧 run) → rc=0
```

结论：两条 job 各评估 4 拍（20:00/20:15/20:30/20:45），**仅 reminder 在投用首拍 fire 一次**（原因 = 本次部署更新了 skill，`semantic.json` mtime 落后于 skill，门按设计判定必须重算）；此后 3 拍与 health 全部 4 拍均 `fire:false`，零 agent 回合。`semantic.json` mtime 停在 20:01:09，此后无重写。

**exec 返回字段实形**：薄壳使用容错链 `res?.aggregated ?? res?.text`，live 实测该链成功解析判定器 stdout 并驱动 fire/no-fire——**容错链可用**；两个字段中哪一个承载文本未单独隔离（无 instrumentation），故不宣称单字段结论。

**无人值守 exec 审批**：trigger 评估在 4 拍中均正常执行、无审批阻塞记录（`openclaw cron show` 无审批错误字段），判定为无需交互审批。

## L3 · 变更驱动（skill 输入触发一次重算）

```
$ openclaw cron show kidpal-reminder-semantic --json   (fire 时刻 20:01:14)
state.lastTriggerFireAtMs=1793448074907, lastDurationMs=53265, lastRunStatus=ok → rc=0
$ ls -l --time-style=full-iso /opt/kidpal/alerts/semantic.json
2026-10-31 20:01:09 -rw------- 493 semantic.json → rc=0
```

skill 文件 mtime 变化 → 下一拍 fire → agent 回合 53 秒完成 → `semantic.json` 刷新 → 后续拍静默。`schedule.md` 内容变更走同一条判定路径（判定器 A 的输入之一，host 真值表已逐条钉住），本轮未单独 touch `schedule.md` 重复消耗一次 240k 回合。

## L4 · health 四分支（判定器级，临时文件驱动，不触碰真实告警路径、不给家长发 QQ）

```
$ bash -c 'export KIDPAL_PARSE_ERROR_JSON=/tmp/crg-l4/alert.json KIDPAL_PARSE_HEALTH_JSON=/tmp/crg-l4/health.json; /opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/schedule_health_gate.py'
[branch4] {"fire":true,"message":"课表解析失败出现新坏指纹：ValueError|invalid date token 2026-10-31 周一 —— 详情：…"} rc=0
[branch3] {"fire":false} rc=0
[branch2] {"fire":true,"message":"课表解析告警件已不新鲜（解析已恢复），但 ack 状态件仍在：请按 … 恢复分支清理"} rc=0
[branch1a] {"fire":false} rc=0
[branch1b] {"fire":true,"message":"课表解析告警件已不在场（解析已恢复），但 ack 状态件仍在：请按 … 恢复分支清理"} rc=0
$ rm -rf /tmp/crg-l4 && echo CLEANED
CLEANED → rc=0
```

输入构造：branch4 = 新鲜告警 + 无 ack；branch3 = 新鲜告警 + 同指纹 ack（含 `last_notified_at`）；branch2 = 陈旧告警（checked_at 减 10 分钟）+ 有 ack；branch1a = 无告警无 ack；branch1b = 无告警有 ack。指纹逐字 = `error|detail`；`state` 仅含 `lastFire{fingerprint,atMs}`。真实 QQ 投递未在本轮触发（会打扰家长），投递面按既有两层口径由 skill 承载。

## L5 · 记忆隔离未回退

```
$ openclaw sessions list --agent main --limit 300 --json | grep cron
agent:main:cron:26c6c771-c83e-4883-9b44-3a742023e481:run:bd3f1e2a-5085-4c6d-a89e-130a373e686e done → rc=0
$ docker exec honcho-database-1 psql -U postgres -Atc "select count(*) from messages where created_at > now() - interval '40 minutes';"
0 → rc=0
$ docker exec honcho-database-1 psql -U postgres -Atc "select count(*) from messages where content like '%提醒语义回合%' or content like '%课表解析健康巡检%';"
0 → rc=0
```

被 trigger 唤起的 run 会话 key 仍属 `cron:` 家族；Honcho 无对应 message。

## L6 · 回滚演练（--clear-trigger）

```
$ /opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/cron_triggers.py clear
CLEAR_RC=0
$ openclaw cron show kidpal-reminder-semantic --json  → trigger=null schedule={"kind":"every","everyMs":900000} msgLen=116 → rc=0
$ openclaw cron show kidpal-schedule-health --json     → trigger=null msgLen=137 → rc=0
$ /opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/cron_triggers.py apply
APPLY_RC=0
$ /opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/cron_triggers.py status
OK kidpal-reminder-semantic …（once=None）
OK kidpal-schedule-health …（once=None）
STATUS_RC=0
```

回滚一步可逆：`trigger` 字段消失、频率/agent/payload message 逐字未变；演练在两次 tick 之间完成、未产生无条件 run；恢复 apply 后 status 两条 OK。

## L7 · 成本对照

```
$ openclaw cron show kidpal-reminder-semantic --json  → triggerEvalCount=4, lastRunAtMs=1793448021642 → rc=0
对照基线 = 2026-10-31 00:00–16:00 实测 20.75M token / 719 次调用（两 job 全量空转，设备侧 0 调用）
```

部署后首小时（20:00–21:00）：两 job 共 8 拍静默评估、1 次真实重算（skill 变更）；换算预期日耗从 ~20.75M token 降至「仅内容变更时产生调用」。24h 正式复核点 = 2026-11-01 20:00（按 `triggerEvalCount` 与 provider 用量对照基线），本节记录部署后首小时观测。

## 附注 · live 期间发现并已修复的三个 CLI 契约缺陷

1. `--trigger-script` 收**文件路径**（CLI 读取后内联），传内联文本报 `ENOENT`；
2. `openclaw cron edit` 只认 **automation id（UUID）**，名字会报 `Automation not found`；`cron show`/`cron runs` 认名字（`cron runs` 还需 `--id`）；
3. `cron_triggers.py status` 子命令缺 `--job` 导致 `AttributeError`。

三项均已在实现侧修复并补 host 单测（`server/tests/test_cron_triggers_units.py`），随修复批回流 main。本次 live 验证期间 ECS 上使用的是**未合并的修复版 `cron_triggers.py`（热铺，原文件已备份 `.bak-20261031`）**；代码合并并推送后须以 deploy 复跑确认云端与 main 逐字一致。
