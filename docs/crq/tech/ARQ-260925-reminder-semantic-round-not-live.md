---
id: ARQ-260925-reminder-semantic-round-not-live
title: 提醒三档「agent 回合产 semantic.json」疑似从未上线——live 只见机械兜底半边（260925-prompt-v7-round-routing 批 S2 实证附带发现）
domain: tech
status: deferred
severity: SHOULD
owner: ""
trigger: 下次 live 冒烟或提醒链路排障时定案：核 OpenClaw cron 会话日志 + 在交接目录观察一个提醒周期内 semantic.json 是否出现；缺位则补注册 agent 语义回合（cron announce 路由或双 cron 形态二选一），并补 L5 类 live 证据
opened: 2026-09-25
closed: ""
closedBy: ""
paths:
  - server/deploy/openclaw-patch/reminder_schedule_lib.py
  - server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md
source: 260925-prompt-v7-round-routing（architect 终审批外行动项，coordinator 实证）
---

实证（2026-09-25，ECS live）：`kidpal-reminder-plan` cron 每 15 分钟在跑且 status=ok，但 Target=isolated、
announce 无路由（last -> no route, will fail-closed）；交接目录 `/opt/kidpal/alerts/` 只有 `pending.json`，
**`semantic.json` 不存在**。即「备注里哪部分算要带的 + 文案 = agent 回合产语义产物」这一半（ADR-002 混合
分层的语义侧、prompt v6/v7 提醒节的立论前提、runbook §8.3 注记「语义产物由 agent 回合写 semantic.json」）
在 live 上从未观察到产出——三档提醒的 `speech/bring` 很可能一直走脚本机械兜底（备注整条当单项、缺省
兜底文案），带物细粒度（备注拆多物件、儿童化文案）从未生效。

候选根因（未查证）：① cron `--command` 载荷只是 shell 跑确定性脚本，agent 语义回合没有对应的注册面
（runbook §8.3 只注册了脚本侧）；② 语义回合原设计走 cron announce → agent，但 announce 无路由 fail-closed
静默丢弃。修复面 = 补 agent 语义回合的触发与路由（openclaw cron 双任务或 announce 路由修复）+ 明确该回合
落在哪个 agent（main workspace 或板侧 kidpal agent——v7 prompt 提醒节的读者面随之定）+ live 五事实 L5
类证据补课。prompt v7 提醒节本身无需回改（正向措辞、语义无错），但若语义回合落在板侧 agent 之外，其
「仅提醒 cron 回合执行本节」的读者假设要在下版 prompt 复核。
