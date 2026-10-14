---
name: "kidpal-schedule-health"
description: "课表解析失败告警巡检回合 (KidPal)：读 /opt/kidpal/alerts/schedule-parse-error.json，命中未确认的解析失败时经 QQ 主动告知家长坏 token 与正确格式，用独立 ack 状态件防重复打扰。"
---

# KidPal 课表解析健康巡检（解析失败 → QQ 转告家长）

家长经 QQ 让 main agent 改写 `kidpal/schedule.md` 时若写进一个坏格（如日期带星期后缀），front
单源 parser 会整份拒、三触发全停摆。front 每分钟轮询命中解析失败时，会把一份机器可读告警件写到
`/opt/kidpal/alerts/schedule-parse-error.json`——**但 front 只能写件、不能主动告知家长**。本 skill
（cron 每 15 分钟唤起 main agent）就是把这份告警**转告给家长**的那半边。收到任务即执行本流程，
不需要向谁确认。

## 检测面与状态件

- **告警件（front 写，每分钟覆写）**：`/opt/kidpal/alerts/schedule-parse-error.json`，字段
  `{checked_at, error, detail}`——`checked_at` = 最近一次解析失败时刻（ISO8601 UTC）、`error` =
  异常类名、`detail` = 坏 token 详情（截断）。**该件不含 ack 字段**（front 每分钟覆写会把写在件里
  的 ack 冲掉），去重靠本 skill 自己的状态件。
- **ack 状态件（本 skill 写，单写者 = main agent）**：`/opt/kidpal/alerts/schedule-parse-health.json`，
  字段 `{"last_notified_fingerprint": "<error>|<detail>", "last_notified_at": "<ISO8601>"}`。
  **指纹（fingerprint）= `error + "|" + detail`**——家长换一个坏 token 即换指纹、会再次告知（正合
  「又写坏一个不同格」场景）。

## 四分支判定（按序，命中即止）

1. **告警件不在场** → 无解析失败。不动状态件（可顺手清 `schedule-parse-health.json`）；本回合结束、
   不告知。
2. **告警件在场但 `checked_at` 距现在 > ~2-3 分钟** → **已恢复**（front 不再刷新 `checked_at` =
   解析已通过）。清状态件、**不打扰家长**；结束。
3. **告警件在场且新鲜、且 `last_notified_fingerprint` == 当前指纹** → **已告知过同一坏格**，跳过、
   不重复打扰（ack 去重语义）。
4. **否则（新鲜告警且指纹与上次不同）** → **经 QQ 主动告知家长**：说明课表有个格格式不对、贴出
   `detail` 里的具体坏 token，并给出正确格式引导（一次性安排「日期」列 = 纯 `YYYY-MM-DD` 不带星期；
   每周固定「星期」列 = 周一~周日；时间 `HH:MM-HH:MM` 或 `HH:MM`；五列）。告知后**写状态件**（新指纹
   + `last_notified_at` = 现在），防止下一回合重复打扰。

## QQ 转告投递与降级（如实两层，不虚诺）

- 本回合是**无入站消息的主动 announce**：经 QQ 投给家长需 OpenClaw 已配 delivery/announce 路由目标
  （家长 openid）。**路由未配时，本回合仍跑、仍能读到告警件**，但主动告知 fail-closed 投不出——此时
  **可观测面降级** = 告警件本身（`schedule-parse-error.json`）+ front journal 的 `schedule_md_err`
  行（`journalctl -u kidpal-front.service | grep schedule_md_err`）；**不要因为投不出就静默装作已告知**。
- 投递目标配置（家长 openid）= post-deploy 运维动作，见 DEPLOY-RUNBOOK §8.3c；本 skill 不自行发明
  openid、不改路由配置。

## 边界

- 本 skill 只读告警件、只写自己的 ack 状态件；不写 `kidpal/schedule.md`、不改 front、不产 display 块、
  不面向设备播报。
- 修复课表本身归家长重录 / `kidpal-schedule-write` skill（本 skill 只告知、不代改写）。
- 检测粒度 = cron 每 15 分钟一轮（与 reminder 面同频）；`checked_at` 是判定「新鲜 / 已恢复」的唯一
  时间锚，别用本 skill 自己的运行时刻顶替。
