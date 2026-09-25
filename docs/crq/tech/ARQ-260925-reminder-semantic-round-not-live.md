---
id: ARQ-260925-reminder-semantic-round-not-live
title: 提醒三档「agent 回合产 semantic.json」疑似从未上线——live 只见机械兜底半边（260925-prompt-v7-round-routing 批 S2 实证附带发现）
domain: tech
status: deferred
severity: SHOULD
owner: planner
trigger: 部署执行后定案——注册 agent 型 cron（§8.3b）+ 铺 skill 到 main workspace 后，盯一个完整提醒周期核 semantic.json 出现且 entry_key 对齐、pending.json 变语义版、设备真收带物播报；失败则回退机械兜底维持现状并升级
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

---

## 修复设计定案（2026-09-25，human「你改我审」模式，随 260925-prompt-v7-round-routing 批后落）

- **载体**：新增 agent 型 cron `kidpal-reminder-semantic`（`--every 15m --agent main --no-deliver`，
  message 指向 skill），与 `kidpal-reminder-plan`（command 型 isolated）双任务并行；脚本侧零改动
  （消费链路 `load_semantic` / `bring_items` / title-speech 覆盖经本日核查完整）。
- **落在 main agent**：板侧 kidpal agent 只在设备语音请求时被引擎注入 system prompt，cron 唤起的
  agent 回合不走引擎注入——v7 提醒节的读者假设只覆盖语音回合面；main agent 拥有 kidpal/ 写侧与
  skill 机制，语义回合指示的家 = main workspace skill。v7 提醒节不回改（对 kidpal agent 属
  inert 但语义无错的知识面，语音回合一侧仍可能被问「带什么」类问题受益）。
- **指示的家**：`workspace-main/kidpal-reminder-semantic` skill（镜像
  `server/deploy/openclaw-skills/workspace-main/`）；skill 内钉三个关键语义——entry_key 逐字
  对齐脚本算法、speech/title 单值三档共用故禁相对时间词（脚本第 372-374 行实证）、原子写 + 全量
  重写。
- **验证口径**（L5 证据补课）：注册后盯一个完整周期——semantic.json 出现且 entry_key 与
  schedule.md 行逐字对齐、pending.json 的 speech/bring 变成语义版、设备真收到带物播报。

## 登记项（architect review 260925 第二轮，随批落定或待下批）

- **消费端已知局限（S-A）**：`bring_items` 兜底分支无法表达「无携带物」抑制——判为非携带物
  的备注（需签字/提前到达类）省略 bring 后，脚本仍把备注整条当单项念出。skill 已声明此局限
  防验证误判；根治 = 脚本批支持显式空 bring 抑制（触冻结脚本，另立包）。
- **契约 §2.3 载体表述漂移**：契约第 232 行「备注解析约定由 OpenClaw system prompt 约定钉死」
  ——本批后活载体 = main workspace skill（prompt 侧 inert）。数据形状零变更、纯散文过时，
  待下个触契约的批按四分类「spec↔contract 漂移」捎带 prose 修正。
- **v8 剪枝候补**：v7 提醒节（~1.5k 字符）自本批起成为每语音回合常驻的死重量（其读者
  cron 语义回合在 main agent、不经引擎注入）——P3-9 机制级选项（提醒约定移出常驻 prompt）
  的自然候补，v8 时 prompt 侧改一句指针或全删。
- **S-B/S-C**：skill 读路径以 `KIDPAL_SCHEDULE_MD` 实际指向为核对口；写路径硬编
  `/opt/kidpal/alerts/semantic.json` 默认值与 plan cron 声明一致，alerts 目录迁移时两处同改。
