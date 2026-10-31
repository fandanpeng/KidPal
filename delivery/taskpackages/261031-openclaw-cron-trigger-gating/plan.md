# Plan · 261031-openclaw-cron-trigger-gating

```yaml
changeId: 261031-openclaw-cron-trigger-gating
title: OpenClaw 两个 15 分钟 agentTurn cron 改变更驱动——cron trigger 静默门 + 仓内 python 判定器
rigor: lite
riskTier: high
sourceType: human-ad-hoc
reviewed-by: architect（/triage-change 比例化 review 2026-10-31；原判 ② 单包 Full + 前置 Lite spike，human 改判见下）
date: 2026-10-31
humanDecision: 2026-10-31 human 裁定「② Lite，跳过 spike」——显式覆盖 change-triage.md §2 单向守卫（riskTier=high ⇒ rigor=full）与 architect 的 spike 前置建议；理由记录：改动面单一、可逆（--clear-trigger 即回退）、live 验收可覆盖未跑 spike 的四条未知项。
```

## 来源与立项

主线程 2026-10-31 在 ECS 8.137.91.16 实测：当日 00:00–16:00 共 **20.75M token / 719 次调用 / $0.69**，全部来自 main agent 的两个 15 分钟 `agentTurn` cron，设备侧 `kidpal` agent **0 次调用**：

| job | 运行次数 | 每次模型调用 | 每次 token | 合计 |
|---|---|---|---|---|
| `kidpal-reminder-semantic` | 65 | 8.0 | 240k | 15.6M |
| `kidpal-schedule-health` | 64 | 3.1 | 79k | 5.1M |

根因不是单次太贵，是**输入没变还在重算**：`/root/.openclaw/workspace/kidpal/schedule.md` 最后修改 2026-10-13 14:06，18 天来每 15 分钟重算同一份 `semantic.json`；`schedule-health` 读的 `schedule-parse-error.json` 只在解析失败时存在，其余时间纯空转。对照同门 `kidpal-reminder-plan` 是 `payload_kind: command`（确定性 python，无 LLM），不花钱。

人类方向已定：改成**变更驱动**——内容没变时零 LLM，变了才跑原语义回合。

## 目标效果

1. `kidpal-reminder-semantic`：`schedule.md`、语义 skill、解析器、排程库四者都没变时，每 15 分钟只跑一个无模型的静默门，不再起 agent 回合；任一变化后最迟一个周期（≤15 分钟）内刷新 `semantic.json`。
2. `kidpal-schedule-health`：无告警件、或同一坏指纹已告知时不起回合；新坏指纹或需要恢复动作时才起。
3. 两条 cron 的频率（15m）、payload message、agent、session 目标全部不变——只加 `trigger` 字段。
4. 回滚一步：`openclaw cron edit <name> --clear-trigger`，回到今天的无条件形态。

预期节省：从 ~20.75M token/天降到接近 0（仅内容真变时产生调用）。

## 方案形状（architect 裁定，主线程搬运）

OpenClaw 2026.8.1 原生支持 cron 条件触发器：`--trigger-script`（job 结构 `trigger: {script, once?}`）。按 job 自己的 every/cron 频率每轮先跑一次**静默无模型**的检查脚本，读冻结的 `trigger.state`，返回 JSON `{fire, message?, state?}`；`fire:false` 只存 state，`fire:true` 才跑 payload 且 `message` 是该轮全部上下文。预算 30s / 5 次工具调用 / state ≤16KB；全局开关 `cron.triggers.enabled`（默认开）。

**两件套设计**（因 trigger 脚本是 code-mode JavaScript，不是 shell）：

- **薄壳 JS**（内容内联进 job）：`exec` 调仓内 python 判定器，透传判定结果；
- **判定器 python**（仓内新增，纯只读）：真正的判定逻辑，可 host 单测。

### 薄壳模板（两条 cron 同构，仅路径不同）

```js
const res = await exec({ command: "/opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/reminder_semantic_gate.py" });
const raw = String(res?.aggregated ?? res?.text ?? "");
const out = JSON.parse(raw.trim());
json({ fire: out.fire === true,
       ...(typeof out.message === "string" && out.message ? { message: out.message } : {}),
       ...(out.state && typeof out.state === "object" ? { state: out.state } : {}) });
```

`res.aggregated ?? res.text` 是未跑 spike 的容错写法——实现包 live 验收须记录 `exec` 返回字段实形，若两者皆空则按实测字段修正（L2）。

### 判定器 A：`reminder_semantic_gate.py`

输入（env，均带默认，argv 可覆写供单测）：`KIDPAL_SCHEDULE_MD`、`KIDPAL_SEMANTIC_JSON`、`KIDPAL_REMINDER_SEMANTIC_SKILL`、`KIDPAL_SCHEDULE_PARSER`、`KIDPAL_REMINDER_LIB`、`KIDPAL_SEMANTIC_GATE_MARGIN_SEC`（60）。

输出：stdout 单行 JSON `{fire, message?, state?}`；正常恒 exit 0，内部异常 exit≠0（薄壳抛错 → 每轮 `status=error`，fail-loud）。

**fire:true 条件（命任一）**：
1. `semantic.json` 缺失；
2. 非法 JSON；
3. `mtime(semantic) - mtime(schedule) <= 60s`（读写竞态余量：agent 读 schedule 后约 31s 才落盘，期间改课表会得到旧内容 + 新 mtime，余量让下一周期补跑）；
4. `mtime(semantic) <= mtime(skill)`、`<= mtime(gateway/schedule.py)`、`<= mtime(reminder_schedule_lib.py)`（margin 0）。

`schedule.md` 缺失 → `fire:false`（skill 自身约定不写、保留旧产物）。

**明确不覆盖**：AGENTS.md / 系统 prompt 变更不触发重算（semantic.json 内容 = schedule × skill × parser × lib 的函数）；出现实证再扩输入集。

### 判定器 B：`schedule_health_gate.py`

输入：`KIDPAL_PARSE_ERROR_JSON`、`KIDPAL_PARSE_HEALTH_JSON`、`KIDPAL_HEALTH_STALE_SEC`（180）、可选 `KIDPAL_HEALTH_FIRE_COOLDOWN_SEC`（3600）。

指纹 = `error + "|" + detail`（与 skill 逐字一致）；新鲜度 = `checked_at` 距 now ≤180s，不可解析时退回文件 mtime。

真值（对应 skill 四分支）：
1. 告警件不在场：ack 不在 → `false`；**ack 在场 → `true`（恢复动作，payload 清 ack，不打扰）**；
2. 在场但不新鲜：同 1（让 payload 走 skill 的「已恢复」清理，不靠门写文件）；
3. 新鲜且指纹 == ack 指纹 → `false`；
4. 新鲜且指纹不同（或 ack 缺失/损坏）→ `true`，message 自包含（指纹 + detail + 指明走 skill 分支 4）；
5. 告警件在场但不可解析 → `true`（宁可让 agent 看一眼也不静默），message 注明按 skill 边界处理、不打扰家长。

防刷屏保险丝：`trigger.state.lastFire={fingerprint, atMs}`，同指纹在冷却窗内且上轮 fire 成功 → `false`。**state 只作反刷屏与观测，不是去重权威**；去重权威仍是 ack 件（payload 单写）。

失败方向原则：「有事可做」的存疑一律向 fire 偏（多跑一轮只花 token）；通知去重绝不靠门自己发明状态。

### 所有权表

| 件 | 单写者 | 读方 | 语义 |
|---|---|---|---|
| `semantic.json` | main agent（payload，原子写） | `reminder_schedule_lib.py` | 不变 |
| `schedule-parse-health.json`（ack） | main agent（payload） | 健康门（只读）+ 人 | 去重权威不变 |
| `schedule-parse-error.json` | front（每分钟覆写） | 健康门（只读）+ agent | 不变 |
| `trigger.state` | 平台（job 自身） | 门脚本（只读快照） | 仅反刷屏与观测，丢了不影响正确性 |

### job 最终形态

```
kidpal-reminder-semantic: schedule every 15m · agent main · sessionTarget isolated
  trigger: {script: <reminder-semantic.trigger.js 内联文本>, once:false}
  payload: {kind: agentTurn, message: <原 message 逐字不变>}
kidpal-schedule-health: 同构，trigger 换 schedule-health.trigger.js，payload message 不变
```

## 目标文件清单

| 仓内路径 | ECS 落点 | 改什么 |
|---|---|---|
| `server/deploy/openclaw-patch/reminder_semantic_gate.py`（新） | `/opt/kidpal/openclaw-patch/` | 判定器 A（纯只读，stdout 一行 JSON） |
| `server/deploy/openclaw-patch/schedule_health_gate.py`（新） | `/opt/kidpal/openclaw-patch/` | 判定器 B（纯只读） |
| `server/deploy/openclaw-patch/triggers/reminder-semantic.trigger.js`（新） | 仓内 + `/opt/kidpal/openclaw-patch/triggers/`（供 verify 比对） | 薄壳 JS |
| `server/deploy/openclaw-patch/triggers/schedule-health.trigger.js`（新） | 同上 | 薄壳 JS |
| `server/deploy/openclaw-patch/cron_triggers.py`（新） | `/opt/kidpal/openclaw-patch/` | 幂等 apply/clear/status：经 `openclaw cron edit` 加/去 trigger，打印 diff，`--dry-run`；不直写 `openclaw.json` |
| `server/deploy/manifest.yaml` | — | 4 条 md5 条目；`cron_jobs` 两条各加 `expect_trigger_file:`；段注释补「trigger 核验」 |
| `server/deploy/verify-deploy.sh` | 部署 clone 内同路径 | cron 段增：内联 trigger 文本 vs 盘上文件（行尾归一后逐字）+ `trigger.once` 不得为 true + `cron.triggers.enabled === false` 守卫 |
| `server/deploy/openclaw-patch/DEPLOY-RUNBOOK.md` | — | 新增 §8.3d（变更驱动说明 + 所有权表 + 铺放/edit/观测/回滚命令）；§8.6 回滚补 `--clear-trigger`；§9 升级检查清单增 trigger 核对 |
| `server/deploy/openclaw-skills/workspace-main/kidpal-reminder-semantic/SKILL.md` | `/root/.openclaw/workspace/skills/...` | 加「本回合由 cron trigger 变更驱动唤起」段：触发原因是提示、必须按当前实际文件状态重判；semantic.json 新鲜度合同（须新于四个输入 + 60s 竞态余量） |
| `server/deploy/openclaw-skills/workspace-main/kidpal-schedule-health/SKILL.md` | `/root/.openclaw/workspace/skills/...` | 加同类段：trigger = 是否要跑的判定层；skill = 四分支行为与 ack 单写者的行为权威 |
| `server/tests/test_semantic_gate_units.py`（新） | — | 判定器 A 真值表（host 齿） |
| `server/tests/test_schedule_health_gate_units.py`（新） | — | 判定器 B 真值表（host 齿） |
| `server/acceptance/test_261031_openclaw_cron_trigger_gating.acceptance.py`（新） | — | 静态锚断言 + `RUN_LIVE=1` 门控 live 段 |

**不动的面**：`docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`、`server/gateway/**`（parser/front）、`reminder_schedule_lib.py`、`kidpal-reminder-plan` job、所有 cron 的频率与 payload message。

## 验收

**Host（pytest）**：
- `reminder_semantic_gate` 真值表：缺失 / 坏 JSON / 落后 schedule / 落后 skill / 落后 parser / 落后 lib / 全新鲜 / schedule 缺失；输出严格单行 JSON 且 fire 为 bool、message 自包含；
- `schedule_health_gate` 真值表：四分支 + 恢复动作 + 指纹换新 + ack 缺失/损坏 + `checked_at` 两种 ISO 形态 + 坏 checked_at 退 mtime + 冷却窗；
- 静态锚：manifest 四条 md5 + 两条 `expect_trigger_file`；verify-deploy 含 trigger 比对与 `triggers.enabled` 守卫；runbook 含 edit/clear-trigger/runs；两份 SKILL.md 含 trigger 段；薄壳引用正确判定器路径且返回 `json({fire...})`；
- 回归：`server/tests` 全绿（含既有 `test_reminder_units.py`）、`test_261013_qq_schedule_write_closure.acceptance.py` 不红。

**ECS live（`RUN_LIVE=1`，human 批准相位）**：
- L1：`cron show --json` 含 `trigger.script`、`once` 非 true、`cron.triggers.enabled` 非 false；
- L2（**未跑 spike 的四条未知项在此收口**）：静默两拍无 run 历史新增、`state.triggerEvalCount` 递增、token 计费面无该 job 调用；记录 `exec` 返回字段实形（`aggregated` vs `text`）；
- L3：`touch schedule.md` → 下周期 run、`semantic.json` mtime 更新且 JSON 合法、再下周期不再 fire；
- L4：注入新鲜且不同指纹的告警件 → 下周期 fire 且 ack 更新（QQ 真达按既有两层口径，路由未配记「投不出但件与日志在」降级证据）；随后做成不新鲜/删除 → 下周期恢复动作、ack 被清、无 QQ；
- L5：被 trigger 唤起的 run 其 session key 仍属 `cron:` 家族、Honcho 库无对应 message（记忆隔离不回退）；
- L6：`--clear-trigger` 回滚演练 → 下周期无条件跑一次、`cron show` 无 trigger、message/agent 未变；
- L7：24h 成本复核（对照 2026-10-31 基线 20.75M/天）。

## 回滚

1. `openclaw cron edit kidpal-reminder-semantic --clear-trigger` 与 `openclaw cron edit kidpal-schedule-health --clear-trigger`（payload / message / 频率零改动，回到今天的无条件 15m）；
2. 验收：`cron show --json` 无 trigger 键、下周期 `cron runs` 出现 run、`semantic.json` 恢复每 15 分钟刷新；
3. 彻底停：`openclaw cron disable <name>`；
4. 仓内文件无需回滚（未被消费即无害），manifest 条目留待后续批清理。

## 风险

1. **静默停摆（头号）**：门判错 = 生产全绿但消费者不工作。缓解 = host 真值表 + live L2/L3 + runbook 观测；残余风险如实登记（门语义无法被 verify 完全机检）。**本次跳过 spike，四条 trigger 语义未经 live 预验证，全部压到 L2/L5 收口**。
2. `fire` 成功但 payload 失败 → state 不持久化、下轮重跑（好）；payload 成功但不落产物 → 门按 mtime 仍每周期 fire（与今天同频，非回归）。
3. 竞态：schedule 在 agent 读取后 ~31s 内被改 → 最多多跑一轮（60s 余量兜住），不会漏跑。
4. ack 件损坏 → 多告知一次（宁可打扰不静默），已列入风险。
5. code-mode `exec` 若在无人值守下需审批卡 → 退回 `read` 工具读文件（3–4 次调用，预算内）；实现包 L2 首次观测即判。
6. OpenClaw 升级可能迁移/改变 trigger 结构 → runbook §9；verify 比对内联文本可抓「盘上变了但 job 内联没更新」。
7. `cron.triggers.enabled` 被关 → 带 trigger 的 job 每轮 `status=error`（非静默）但功能停摆 → verify 守卫 + runbook。
8. 内联文本与仓内文件漂移 → verify 必红，按 runbook 重跑 `edit`。

## 遗留

1. QQ 路由目标未配（既有缺口）：health 的「家长真收到」live 验收无法闭合，按既有两层口径记降级证据。
2. DEPLOY-RUNBOOK §8.3b/§8.3c 的注册命令仍是无条件 cron 形态，同批各加一行「注册后按 §8.3d 加 trigger」。
3. 「semantic.json 新鲜度合同无 spec 级钉点」本批只写进 skill/runbook，不登 debt（登记权归主线程）。
4. 上线 24h 复核实际节省；若 schedule.md 变更意外频繁，再评估把 `schedule.refresh` marker 接入语义即时重算（下一批）。
