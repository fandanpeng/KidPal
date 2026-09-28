---
id: TECH-260928-school-start-upcoming-empty
title: 周中 upcoming 恒空：school_start 配置未部署 ECS 且三条推送路径漏传
domain: tech
status: closed
severity: MAJOR
owner: dev-loop（server 面）
trigger: 下一次 server 侧变更批（/triage-change 路由后随包修）；ECS 运维半随代码修一并收口
opened: 2026-09-28
closed: 2026-09-28
closedBy: dev-loop integrator（changeId 260928-tech-260928-school-start-upcoming-empty；代码半修绿，运维半移交留痕见收口记录）
paths: [server/gateway/front/front_app.py]
source: [主线程 2026-09-28 现场诊断（本条目「证据」段留 verbatim 命令与输出）]
handoff: /triage-change 路由 → dev-loop server Pod；移交触发 = human 确认修复批立项
---

**证据**：2026-09-28（周一）主线程现场诊断，三层实测：

1. ECS 上 `openclaw-schedule.config.yaml` 不存在、service 也未配 `KIDPAL_SCHEDULE_CONFIG`：
   `find /opt/kidpal /root/.openclaw -name 'openclaw-schedule.config.yaml'` → 零命中；
   `systemctl cat kidpal-front.service` 的 Environment 仅有 `KIDPAL_ALERTS_DIR` /
   `KIDPAL_SERVER_DIR` / `KIDPAL_GRID_RESTORE_PUSH=1`。
2. front_app.py 三条推送路径调 `home_upcoming_payload` 均未传 `school_start`（部署面
   `/opt/kidpal/front/front/front_app.py` 与仓内一致）：建连推 `front_app.py:685`、
   零点/即时推 `front_app.py:708`（仓内行号；部署面 :565 同构）；只有「帮我显示课表」
   意图展开路径 `front_app.py:642` 传了 `_schedule_school_start()`。
3. journalctl 实录（`journalctl -u kidpal-front.service --since '2026-09-27'`）：
   - `Sep 28 00:00:00 ... schedule_push sent ... trigger=midnight local=2026-09-28 sent=0`
     （设备不在线，零点推空过）；
   - `Sep 28 08:50:43 ... WebSocket / [accepted]` + `connection open`（设备上线、
     建连推已发但成功不打日志）→ 设备收到的 payload 因 1+2 为
     `items: [], preview: null`；
   - 设备侧 P1 首页（`firmware/main/display/home_p1.c:1166` 空日程分支）只画
     「接下来」帽标 + 云朵，无内容行、无「明早」预告——human 看屏症状吻合。
   旁证：周六课外课（英语 08:00 / 街舞 17:00）走每周固定表、不依赖 school_start，
   周六显示正常——问题只在周一~周五。

**影响面**：周一~周五（在校日）P1 首页「接下来」组与「明早」预告恒空，每日复现；
P1 是产品主界面，核心「课表常显」能力在校日实质不可用。三触发（建连①/零点②/
marker③）全走同两条未传参路径，故任何时刻的推送都产空 upcoming。

**建议改动**（交 review、不自批）：
- 代码半：`front_app.py` 的 `_push_schedule_connect` 与
  `_push_schedule_refresh_to_legs` 两处 `home_upcoming_payload` 调用补传
  `_schedule_school_start()`（与意图展开路径同参对齐；验收面
  `test_m3k1_schedule_producer.acceptance.py` 的 `_home`/`_grid` 已支持该入参，
  属接线回归、非契约变更）。
- 运维半：把 `server/deploy/openclaw-schedule.config.yaml`（school_start: "08:00"）
  放到 `KIDPAL_SCHEDULE_CONFIG` 指向路径（默认 `_DEPLOY_DIR = parents[2]/deploy`，
  ECS 部署布局下 = `/opt/kidpal/deploy/`），并在 kidpal-front.service 显式加
  `Environment=KIDPAL_SCHEDULE_CONFIG=...`。`_schedule_school_start()` 每次调用现读
  文件，配置落位即生效、无需重启。
- 便宜验证：`pytest server/acceptance/test_m3k1_schedule_producer.acceptance.py`
  （A3/A4）+ 真机重连后核对建连推 payload 含 `{"time":"08:00","name":"上学"}`。

**风险**：riskTier: low——纯接线参数补传 + 配置部署，无契约面改动、验收面已支持
同参；爆炸半径已被真机实测揭示。Track B 三要件：① 执行测出 yes（上证据）；
② 问题清晰、修复明确（无真设计分叉——school_start 值已在 repo 配置载体内）yes；
③ 爆炸半径已被执行揭示（周一空屏实测）yes → **review profile = Track B 快路径**。
owner = dev-loop（server 面）。

**收口记录**（2026-09-28，integrator，changeId = `260928-tech-260928-school-start-upcoming-empty`）：

- 代码半已修：`server/gateway/front/front_app.py` 两处 `home_upcoming_payload`
  调用补传第五参 `_schedule_school_start()`——建连推 `_push_schedule_connect`
  （原 :685）与零点/即时推 `_push_schedule_refresh_to_legs`（原 :708），与意图
  展开路径同参对齐。验收齿 = `server/acceptance/test_tech260928_school_start_wiring.acceptance.py`
  （TECH-A1/A2 同参齿 + TECH-A3 grid restore 双 None 守卫齿），verifier scoped
  round 3 全绿 + 独立复核 `78 passed, 2 subtests passed`（红基线 2 failed / 76
  passed → 绿，红即本条目漏参症状本体）。
- 运维半移交项（代码修绿后的移交步骤，不在本包范围）：将
  `server/deploy/openclaw-schedule.config.yaml`（school_start: "08:00"）部署到
  ECS 上 `KIDPAL_SCHEDULE_CONFIG` 指向路径（默认 `_DEPLOY_DIR = parents[2]/deploy`，
  部署布局下 = `/opt/kidpal/deploy/`），并在 `kidpal-front.service` 加
  `Environment=KIDPAL_SCHEDULE_CONFIG=...`。`_schedule_school_start()` 每次调用
  现读文件，配置落位即生效、无需重启。
- 真机复验建议：运维半落位后设备重连，核对建连推 payload 含
  `{"time":"08:00","name":"上学"}`，P1 首页「接下来」组在校日恢复显示。
