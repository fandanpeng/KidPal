# input-brief · builder · 261013-qq-schedule-write-closure

> 2026-10-13 · planner 派发。TaskPackage：`delivery/taskpackages/261013-qq-schedule-write-closure/taskpackage.json`（先读，权威）；技术约束全量 = `plan.md` §4（architect 1g 预评，含四个关键坑，**必读**）。

## 你的交付物（changeManifest 7 件，原子落地）

1. **新增** `server/deploy/openclaw-skills/workspace-main/kidpal-schedule-write/SKILL.md`——QQ 课表录入写入 skill（QSW-A1）。内容骨架（格式权威 = `docs/contracts/interface-contract.md` §2.2/§2.3，只读）：
   - 触发：家长经 QQ 要求新增/修改课表（每周固定 or 一次性安排）；
   - 写入纪律：每周固定表头逐字 `| 星期 | 时间 | 课程 | 地点 | 备注 |`（星期=周一~周日、时间 `HH:MM-HH:MM` 或 `HH:MM`）；一次性安排表头逐字 `| 日期 | 时间 | 事项 | 地点 | 备注 |`（**日期纯 `YYYY-MM-DD`、严禁星期后缀**——2026-10-12 live 事故源）；五列形状；表头用范本不复述散文变体；
   - **编辑前先备份**（同目录 .bak 或记 diff）；
   - 写后机检（plan §4.1 的 importlib 绝对路径命令逐字写入，解释器钉 `/opt/kidpal/venv/bin/python3`、按文件路径加载 `/opt/kidpal/front/gateway/schedule.py`，`parse_schedule_md(md_text)` 先读文本、`group_upcoming(model, 日期, 现在)` 复证新行在列）；
   - 机检不过 → **还原本次编辑** + QQ 回复家长正确格式范本与坏 token，不反复试改；
   - 机检通过 → 立刻跑重算（plan §4.1 的 `/opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/reminder_schedule_lib.py` + cron 同款 env 组），设备「接下来」≈1 分钟刷新。
2. **新增** `server/deploy/openclaw-skills/workspace-main/kidpal-schedule-health/SKILL.md`——解析失败告警的健康巡检 skill（QSW-A4）：检测 `/opt/kidpal/alerts/schedule-parse-error.json`（front 写、每分钟覆写），按 plan §4.3 四分支逻辑（无件→不动；`checked_at` 超 ~2-3 分钟旧→已恢复清状态；fingerprint 相同→已告知跳过；否则→QQ 告知家长坏 token 与正确格式引导）写 ack 状态件 `/opt/kidpal/alerts/schedule-parse-health.json`（`last_notified_fingerprint` = `error|detail`、`last_notified_at`）。与既有 `workspace-main/kidpal-reminder-semantic/SKILL.md` 文风对齐（读它参照）。
3. **改** `server/deploy/manifest.yaml`：两 skill 各一条 entries（字段与既有 5 条 skill 条目完全同款：owner=deploy.sh / verify=md5 / restart=none，dst=`/root/.openclaw/workspace/skills/<同名>/SKILL.md`）+ cron_jobs 增 `kidpal-schedule-health`（expect_agent: main、expect_schedule: "every 15m"）；**同批顺手修** provider note 的字面残留——把 `openclaw/default` 字样改写掉（如「live 旧版默认值为 stale 别名（260930 事故值）」），恢复 260925 冻结齿 `test_mnf_a1`（exitCriteria 在案）。
4. **改** `server/deploy/openclaw-skills/README.md`：镜像表 +2 行（两新 skill，main 侧）。
5. **改** `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md`：增 §8.3c——health cron 注册命令 + **QQ 路由目标两层如实写**（plan §4.3 末段：路由未配时 health 回合仍跑可读告警件、主动告知 fail-closed 投不出，可观测面降级 = 告警件 + journalctl schedule_md_err；路由配置 = human post-deploy 动作、指向 260924 ops-record 的 accounts 映射位；不虚诺）。
6. **改** `server/gateway/front/front_app.py`：`_load_schedule_model()` 的 except 块（现 :686-688 `log.warning("schedule_md_err …")` 旁）挂调用私有 helper `_write_schedule_parse_alert(err_type, detail)`——写 `ALERTS_DIR/schedule-parse-error.json`，字段 `{checked_at: ISO8601 UTC, error: type(e).__name__, detail: str(e)[:120]}`（plan §4.2 定稿）；**写失败静默吞**（自身 try/except，至多 debug 日志——不得反向上抛炸轮询循环）；日志行原样保留；`server/gateway/schedule.py` **零改动**。
7. **append** `docs/crq/tech/TECH-261012-qq-schedule-write-format-gap.md`：收口补记（①④落地；②③部署批联动；QSW-A5 真机殿后；append-only 不覆盖既有行）。

## 红→绿判据

冻结验收（**只读消费、绝不改**）：`server/acceptance/test_261013_qq_schedule_write_closure.acceptance.py`。
红基线（逐字）：`cd server && PYTHONIOENCODING=utf-8 python -m pytest -q --import-mode=importlib -m "not live" acceptance/test_261013_qq_schedule_write_closure.acceptance.py`
基线 = 9 failed, 2 passed（全红落缺件）。你的目标是 11 passed——2 条守卫齿（`test_a3_legal_file_writes_no_alert` / `test_a3_alert_write_failure_is_swallowed`）是 must-stay-green。
另跑回归观察面：`server/tests/` 全档 = 91 passed 基线保持；`test_mnf_a1` 转绿。

## 纪律

- 实现面只写 changeManifest 7 件（+ 测试跑动不改测试）；`*.acceptance.py` 只读；`docs/contracts/**` 只读；不碰 `.claude/**`。
- 结构化日志带 correlationId（K6）如涉及新日志路径，遵 front 既有日志形态。
- 完成报告：逐件落地说明 + 红→绿实测命令与输出 + 任何偏离（偏离先报告再执行）。
- 原子性：两 SKILL.md 与 manifest 条目必须同批落盘（verify-deploy 的 md5 src 存在性）。
- 真机面（QSW-A5）不做——殿后 post-deploy。
