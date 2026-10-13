---
id: TECH-261012-qq-schedule-write-format-gap
title: QQ 课表录入链路缺口——家庭侧 main agent 无写课表 skill 钉 B1 格式，一个脏日期格致整份 schedule.md 解析失败、「接下来」三触发全静默停摆；连带实测刷新 marker 链（触发③）从未 live 部署
domain: tech
status: deferred
severity: MAJOR
owner: dev 批（课表录入链路在途改动方）
trigger: dev 侧在途课表链路批 landing 前对账合并处理；或下次 QQ 录入复现时
opened: 2026-10-12
closed: ""
closedBy: ""
paths: ["server/deploy/openclaw-skills/workspace-main/kidpal-schedule-write/SKILL.md", "server/deploy/openclaw-skills/README.md", "server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md", "server/deploy/openclaw-patch/reminder_schedule_lib.py"]
source: 2026-10-12 主线程 live 排障（ECS 8.137.91.16）：kidpal-front journalctl 16:49–17:06 每分钟 schedule_md_err err=ValueError detail=invalid date token '2026-10-12 周一' 实录 + 17:21:00 trigger=immediate markers=1 sent=2 恢复实录 + 部署面 grep 实测（/opt/kidpal/openclaw-patch/reminder_schedule_lib.py 无 REFRESH_MARKER 命中）
handoff: dev 批 / server+deploy 面（在途课表链路改动收口时合并；本条目只登记不实施，避免与在途批冲突——human 2026-10-12 指示）
---

**证据**（2026-10-12 主线程 live 排障，ECS 8.137.91.16）：

1. 症状链：家长经 QQ 让 OpenClaw main agent（家庭侧，workspace `main`）在 `kidpal/schedule.md`「一次性安排」区块新增行 `| 2026-10-12 周一 | 19:30-19:55 | 英语外教口语课 | 线上 | 提前预习 |`（文件 mtime 16:50）。B1 契约 §2.3（[interface-contract.md](../../contracts/interface-contract.md)「一次性安排：`日期` 用 `YYYY-MM-DD`」）不收带星期后缀的日期格 → `server/gateway/schedule.py` `_parse_date_token` ValueError（P2-A2 脏行大声拒绝、整包拒）→ `_load_schedule_model()` 返回 None → front 三触发全部短路，journalctl 16:49–17:06 **每分钟** `schedule_md_err err=ValueError detail=invalid date token: '2026-10-12 周一'`；提醒侧重算同样解析失败（pending.json 停在 16:34、marker 从未落）。设备「接下来」停留在旧载荷、新事件不显示，且**家长侧零感知**（报错只在服务端日志）。
2. 修复实录（同日）：日期格改 `2026-10-12`、表头「星期/日期」「课程/事项」修回契约范本「日期」「事项」后，front 单源 parser 解析通过（`group_upcoming(model, "2026-10-12", "17:00", "08:10")` 含 19:30 英语外教口语课）；手动 `touch /opt/kidpal/alerts/schedule.refresh` 后 17:21:00 front 日志 `schedule_push sent corr=… trigger=immediate markers=1 sent=2`，设备「接下来」恢复显示（human 确认）。
3. 连带实测（同一排障会话）：
   - ECS `/opt/kidpal/openclaw-patch/reminder_schedule_lib.py` grep `REFRESH_MARKER|drop_refresh_marker` **零命中**——部署副本是 M3-K1（260921-m3-k1-cloud-schedule-producer）之前的旧版，**触发③（marker 即时刷新）整条链从未 live**；仓内新版（[reminder_schedule_lib.py:430-498](../../../server/deploy/openclaw-patch/reminder_schedule_lib.py) 含 `drop_refresh_marker`）未同步到 ECS。即：QQ 录入后即时刷新从来不生效，一直只靠建连推/每日零点兜底。
   - runbook §8.3 的 cron 命令写 `python3`：SSH shell 解析到 `/usr/bin/python3` = **3.6.8**，跑不动脚本（`from __future__ import annotations` SyntaxError）；cron 环境实际按 3.11 才跑通（17:19 重算成功）——命令未钉绝对路径，人工复跑必踩坑。
   - main workspace live 共 5 个 skill（kidpal-board-context / kidpal-char-strokes / kidpal-reminder-semantic / kidpal-schedule-answer / qqbot-reply-diagnosis），仓内镜像 [server/deploy/openclaw-skills/](../../../server/deploy/openclaw-skills/) 只覆盖 2 个——main 侧 `kidpal-schedule-answer`（恰含「改课表」运维须知段，但无 B1 写入格式纪律、无写后机检、无写后即时重算步骤）等 3 个无仓内 diff 副本。
   - openclaw cron `kidpal-reminder-plan` status=error 实为 delivery announce 缺 QQ 路由目标（脚本本体 ok，17:19 诊断行 `planned 13 alert slots` 为证）——噪音性红，单独排查。

**影响面**：QQ 课表录入是家长主用录入通道。无格式纪律 + 无写后机检 = 任何一个脏格（日期带星期、时间形状错、列数错）都让整份 schedule.md 解析失败，**首页「接下来」三触发、提醒重算、课表意图展开全部静默停摆**，直到人工排障；家长侧无任何报错。且触发③未 live，即使课表写对了，QQ 录入后也不会即时刷新（marker 无人落），只能等设备重连或次日零点。

**建议改动**（候选，交 review、不自批）：① main workspace 补 `kidpal-schedule-write` skill：钉 B1 写入格式（日期纯 `YYYY-MM-DD` 不带星期、时间 `HH:MM(-HH:MM)`、五列形状、表头用契约范本）+ 写后机检（过 front 单源 `parse_schedule_md` + `group_upcoming`）+ 通过后立即跑 `reminder_schedule_lib.py` 重算落 marker（≈1 分钟内设备刷新）；② 同步仓内新版 `reminder_schedule_lib.py` 到 `/opt/kidpal/openclaw-patch/`（补 `drop_refresh_marker`），runbook cron 命令钉绝对路径 `python3.11`；③ main 侧 3 个未镜像 skill 补进仓内镜像；④ 评估 front 对 `schedule_md_err` 的 surfaced 告警（消「静默停摆」，形态有设计分叉）。dev 侧在途批正动课表链路，落地前先对账避免冲突（human 2026-10-12 指示：先登记不实施）。

**风险**：riskTier: medium——修复面全在部署面 / OpenClaw skill 文面，不触 `.claude` 治理面、不触 `docs/contracts/**`；但②换 ECS 部署脚本、①改 live agent 行为，均需真机复证，且与 dev 在途批有撞面。review profile：Track B 三要件——① 执行测出 yes（上列 live 实录）；② 修复明确 no 分叉（①②③）/ ④ 含设计分叉（若纳入须小裁定）；③ 爆炸半径已被执行揭示 yes → ①②③ 走 Track B，④ 单独裁定。owner = dev 批（课表录入链路在途改动方）。

---

**补记（2026-10-13，261013-gov-review-deploy-single-source-loop 落地时 append，修正证据 3 第 2 条表述并钉修复项归属）**：

1. **证据 3 第 2 条表述修正**：「仓内新版 reminder_schedule_lib.py 未同步到 ECS」不准确——新版**已在 ECS**（`/opt/kidpal/xiaozhi-esp32-server/deploy/xiaozhi-docker-data/openclaw-patch/`，md5 与仓内一致），但落在无人消费的孤儿位置；cron 实际消费位 `/opt/kidpal/openclaw-patch/` 停 2026-09-11 旧版。真根因 = **overlay 铺放目标与 cron 消费路径分裂**（`apply-xiaozhi-overlay.sh` 原 125–146 行按固定 `$DATA_DIR` 铺放、从不按消费者路径解析规则对账）。
2. **同型第二处分裂（评审新发现）**：`openclaw-reminder.config.yaml` 的真实消费位 `/opt/kidpal/openclaw-reminder.config.yaml` 无文件（lib `KIDPAL_REMINDER_CONFIG` 默认 `dirname/../` 派生；当前被 `DEFAULT_LADDER_CONFIG` 同值回退掩盖）；`openclaw-schedule.config.yaml` 的消费位 `/opt/kidpal/front/deploy/` 亦无文件（front `parents[2]/deploy` 派生，`school_start` 静默回退 None）。
3. **修复项归属钉死（防双做）**：建议②（新版 lib 同步 + runbook 钉绝对路径）与本补记第 1/2 条的部署一致性根因已由 261013-gov-review-deploy-single-source-loop（FROZEN 2026-10-13，含 manifest/deploy.sh/verify 三件套）承接；建议③（仓内镜像补齐）同由该批 T1 承接。**dev 批课表链路收口时只对账建议①（main workspace 补 kidpal-schedule-write skill）与建议④（front 对 schedule_md_err 的 surfaced 告警）**。
