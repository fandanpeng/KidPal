# dispatch-progress-live-handoff · 261031-openclaw-cron-trigger-gating

> **命名说明**：本文件内容 = 任务要求的 `live-handoff.md`。builder 的 scope-map 只允许写
> `delivery/taskpackages/*/dispatch-progress-*.md`（写 `live-handoff.md` 被 scope 钩子拒绝），
> 故落在本允许路径。请主线程/planner 将其内容落为
> `delivery/taskpackages/261031-openclaw-cron-trigger-gating/live-handoff.md`
> （live 验收读该文件名），或由主线程扩 scope 后重写。**builder 未执行任何 ECS 命令。**

---

# live-handoff · 261031-openclaw-cron-trigger-gating

> **本文件是交班清单，不是证据。** host 相位已完工（判定器 / 薄壳 / apply 工具 / manifest /
> verify / runbook / skill / README 已落盘并通过 host 单测与静态锚验收）；以下 ECS 动作
> 归主线程 / human 执行。每步的真实输出回填到末尾「证据回填区」后，方可跑 `RUN_LIVE=1`
> 的 live 验收。

## 0. 前置

- 部署 clone = `/opt/kidpal/deploy-clone`（已 rebase 到含本包的 main 之后）。
- 解释器：`/opt/kidpal/venv/bin/python3`（3.11）；**不要用裸 `python3`**（3.6 有先例）。
- 本包只新增 `trigger` 字段，**不改**两条 cron 的频率（every 15m）、agent（main）、
  session 目标与 payload message。任何一步发现这三项被动到 → 立即停并回滚。

## 1. 铺放（deploy.sh，幂等）

```bash
cd /opt/kidpal/deploy-clone && git pull && bash server/deploy/deploy.sh
# 判据：5 条新已铺——两个判定器 + 两个薄壳 + cron_triggers.py：
ls -l /opt/kidpal/openclaw-patch/reminder_semantic_gate.py \
      /opt/kidpal/openclaw-patch/schedule_health_gate.py \
      /opt/kidpal/openclaw-patch/cron_triggers.py \
      /opt/kidpal/openclaw-patch/triggers/reminder-semantic.trigger.js \
      /opt/kidpal/openclaw-patch/triggers/schedule-health.trigger.js
# 失败回退：deploy.sh 前置漂移核验 REFUSE = 先查 verify-deploy.sh 的 DRIFT 行，不要 --first-run 硬铺。
```

## 2. 加 trigger（幂等 apply，写入口只许 openclaw CLI）

```bash
/opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/cron_triggers.py apply --dry-run
# 判据：打印两条 `openclaw cron edit <name> --trigger-script <内容…>`，不执行任何写。
/opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/cron_triggers.py apply
# 判据：rc=0；两条命令均成功。
/opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/cron_triggers.py status
# 判据：两行 `OK <name>: trigger 内联文本与 …一致（once=False）`，rc=0。
```

诚实缺口：OpenClaw `cron edit --trigger-script` 的真实入参形态（内联文本 or 文件路径）
**未经 spike 预验证**——这是未跑 spike 的四条未知项之一（另三条见 L2/L5）。若 apply 报
参数错，先读 `openclaw cron edit --help` 对齐后修正 `cron_triggers.py` 的调用行，再重跑
`apply --dry-run`；修正前**不要**手工编辑 openclaw.json。

## 3. L1-L7 观测（真实 ECS 执行，逐段回填）

### L1 · job 结构在位
```bash
openclaw cron show kidpal-reminder-semantic --json
openclaw cron show kidpal-schedule-health --json
openclaw config get cron.triggers.enabled
bash server/deploy/verify-deploy.sh
```
- 判据：两条 job 均含 `trigger.script`；`trigger.once` 非 true；`cron.triggers.enabled` 非
  false；`verify-deploy.sh` drift_count=0（含内联文本逐字比对、once 守卫、全局开关守卫）。
- 失败回退：`cron_triggers.py clear` 回无条件形态止血。

### L2 · 静默两拍零调用（未跑 spike 未知项收口：exec 返回字段实形 / 审批 / state 持久化）
```bash
openclaw cron runs kidpal-reminder-semantic | head -20
openclaw cron runs kidpal-schedule-health | head -20
# 计费面：对照当时的 token 观测（当日基线 20.75M/天）
```
- 判据：连续两个 15 分钟周期**无新增 run**（或新增 run 的 trigger 判定为 fire:false）；
  `state.triggerEvalCount`（若平台暴露）递增；无该 job 的模型调用计费。
- **必须记录**：薄壳 `exec` 返回字段实形是 `res.aggregated` 还是 `res.text`（二者皆空 =
  薄壳需按实测字段修正 —— 修正后重跑 `cron_triggers.py apply`）。这是本批头号未知项。
- 失败回退：若 exec 在无人值守下被审批卡住 / 字段皆空导致每轮 status=error → 立即
  `cron_triggers.py clear`，回报 planner 改走 `read` 工具读文件方案（风险 5）。

### L3 · 变更触发 + 稳定后停火
```bash
touch /root/.openclaw/workspace/kidpal/schedule.md
sleep 900 && openclaw cron runs kidpal-reminder-semantic | head
```
- 判据：下周期出现 1 次 run；`/opt/kidpal/alerts/semantic.json` mtime 更新且
  `python3 -c "import json;json.load(open('/opt/kidpal/alerts/semantic.json'))"` 通过；
  再下一周期不再 fire。
- 失败回退：反复 fire 不停 = 判定器看 mtime 落后（skill 未重写或写入竞态）→ 查
  semantic.json mtime 与四输入 mtime 对比；持续异常则 `clear` 回滚。

### L4 · 健康巡检新指纹必告知 / 恢复清理
```bash
cat > /opt/kidpal/alerts/schedule-parse-error.json <<'EOF'
{"checked_at":"<NOW_ISO8601>","error":"ScheduleParseError","detail":"unit-test bad token"}
EOF
openclaw cron runs kidpal-schedule-health | head
rm -f /opt/kidpal/alerts/schedule-parse-error.json
openclaw cron runs kidpal-schedule-health | head
```
- 判据：新指纹 → 下周期 fire 且 ack（`schedule-parse-health.json`）更新；删除/不新鲜 →
  下周期恢复动作、ack 被清、无 QQ。QQ 真达按既有两层口径：路由未配记「投不出但件与日志在」
  降级证据（`journalctl -u kidpal-front.service | grep schedule_md_err`）。
- 失败回退：静默不 fire = 判定器误判 → 用 host 单测真值表复现；线上先 `clear`。

### L5 · 会话分类不回退
```bash
openclaw cron runs kidpal-reminder-semantic | head
# Honcho 库：cron:* 家族不得出现 message（记忆隔离不回退）
```
- 判据：被 trigger 唤起的 run 其 session key 仍属 `cron:` 家族；Honcho 库无对应 message。
- 失败回退：家族漂移 → `clear` 回滚并登 debt（采集污染）。

### L6 · 回滚演练（--clear-trigger）
```bash
/opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/cron_triggers.py clear
openclaw cron show kidpal-reminder-semantic --json | grep -c '"trigger"'   # 期望 0
openclaw cron runs kidpal-reminder-semantic | head                          # 下周期无条件出现 run
```
- 判据：`--clear-trigger` 后 `cron show` 无 trigger 键、下周期出现 run、频率/agent/message 未变。
- 演练完成后如需继续观察，重新 `cron_triggers.py apply`。

### L7 · 24h 成本复核
```bash
# 对照 2026-10-31 基线：当日 00:00–16:00 20.75M token / 719 次调用 / $0.69
```
- 判据：24h 总 token / 调用次数显著下降（内容未变时应接近 0）；若 schedule.md 变更频繁
  导致节省不达预期 → 不立即回滚，回报 planner 评估 `schedule.refresh` marker 接入即时重算
  （下一批）。

## 4. 全程回退（任一步不达标）

```bash
/opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/cron_triggers.py clear
# 等价手工：openclaw cron edit <name> --clear-trigger（两条都执行）
openclaw cron show kidpal-reminder-semantic --json | grep -c '"trigger"'   # 期望 0
openclaw cron show kidpal-schedule-health  --json | grep -c '"trigger"'    # 期望 0
```
彻底停：`openclaw cron disable <name>`。仓内文件无需回滚（未被消费即无害）。

## 5. 证据回填区（主线程执行后替换占位）

| 项 | 结果 | 证据指针 |
|---|---|---|
| L1 job/trigger/全局开关/verify | 待执行 | |
| L2 两拍静默 + exec 字段实形（aggregated vs text） | 待执行 | |
| L3 变更触发 + 停火 | 待执行 | |
| L4 新指纹告知 + 恢复清理 | 待执行 | |
| L5 cron: 家族 / Honcho 隔离 | 待执行 | |
| L6 --clear-trigger 回滚演练 | 待执行 | |
| L7 24h 成本复核 | 待执行 | |
