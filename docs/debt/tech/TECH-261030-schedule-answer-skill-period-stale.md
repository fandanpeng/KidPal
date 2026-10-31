---
id: TECH-261030-schedule-answer-skill-period-stale
title: `kidpal-schedule-answer` skill 仍写「表列第1节…第6节」，live `schedule.md` 已有第7节——设备课表问答会漏掉周一/二/四的兴趣课（足球）
domain: tech
status: deferred
severity: SHOULD
owner: dev-loop（server/deploy OpenClaw skill 部署面）
trigger: 下个动 OpenClaw skill / 课表部署面的批；或设备课表问答再现「漏第7节 / 漏足球」
opened: 2026-10-30
closed: ""
closedBy: ""
paths: ["server/deploy/openclaw-skills/workspace-kidpal/kidpal-schedule-answer/SKILL.md", "server/deploy/openclaw-skills/workspace-main/kidpal-schedule-answer/SKILL.md"]
source: 2026-10-30 主线程 ECS 核查（仓内镜像 grep + live 文件直读）；线索来自同日 01:55 OpenClaw main 会话 compact 摘要保留的 2026-09-21 遗留待办
handoff: 与下个动 OpenClaw skill 部署面的批同批；live 两处 workspace 的 skill 副本须与仓内镜像同批改
---

**证据**（2026-10-30 直读，ECS 8.137.91.16）：

1. live `/opt/kidpal/schedule.md:9` 注记 `> 第7节 = 兴趣课（= 下午第三节课）：周一/周二/周四 足球，周三/周五 空。`；`:13` 表头已列到 `第7节`；`:15` 周一第7节 = 足球。
2. 两个 workspace 的 `kidpal-schedule-answer/SKILL.md:19` 仍写「表列是 `第1节…第6节`、行是星期，逐格取值」——取数范围把第 7 节排除在外。
3. 仓内镜像同文同病：`server/deploy/openclaw-skills/workspace-kidpal/kidpal-schedule-answer/SKILL.md:19` 与 `server/deploy/openclaw-skills/workspace-main/kidpal-schedule-answer/SKILL.md:19` 均命中。
4. 2026-09-21 曾尝试直接 patch live skill，未生效：`skill_workshop` 报 `skill "kidpal-schedule-answer" was not used in this run and cannot be repaired autonomously`（该遗留待办由 2026-10-30 compact 摘要保留下来）。

**影响面**：设备课表问答走这份 skill；指令只点名 1–6 节，模型按指令取数时会把第 7 节（兴趣课）当表外内容——「明天有什么课」「下午第三节课是什么」这类问题会漏答或答错，周一/周二/周四三天受影响。不触契约、不触固件，但家长与孩子看到的是错误答案。

**建议改动**（候选，交 review、不自批）：① 两份 skill 第 19 行的列范围改为「第1节…第7节」，并补一句「第7节 = 下午第三节课 = 兴趣课」；② 同批同步 ECS live 两处副本（`/root/.openclaw/workspace-kidpal/skills/kidpal-schedule-answer/`、`/root/.openclaw/workspace/skills/kidpal-schedule-answer/`）；③ 清点 skill 里其余节次硬编码（例如「第1节…第6节」的其他出现处），一次改净。

**风险**：riskTier: low——纯 skill 文案改动，无契约 / 实现面牵连；改动后需真机问一轮课表（含第 7 节）复证。

**关联**：`TECH-261012-qq-schedule-write-format-gap`（同属课表链路口径漂移——那条管写入格式，本条管读取时的取数范围）。
