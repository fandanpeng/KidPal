# live 五齿 L1-L5 证据 · 260829-schedule-reminder（260911-ecs-deploy-convergence 交班落盘）

> 【归位注记】本文件目标位置 = `changes/dev/260829-schedule-reminder/live-five-teeth-evidence-260911.md`
> （plan.md S3.1 钉的路径），但 enforce-scope 对 acceptance-author 角色只放行
> `firmware/**/*.acceptance.*` / `firmware/**/acceptance/**` / `server/**/acceptance/**` /
> `tools/**`——`changes/dev/**` 路径被机械拦（PreToolUse hook 实拦留痕）。按 260829-hardware-validation
> 包 device-acceptance-checklist.md 同款先例，先落 scope 内 `server/acceptance/`，请 planner /
> integrator 原样归位到目标路径。内容零变更。
>
> **归位完成（2026-09-11 integrator）**：已原样归位至本路径，`server/acceptance/` 源位移除。
> 唯一内容变更为 planner 交办的 L1 `state.lastRunStatus=ok` 回填（见 §L1）；其余各节零变更。
>
> **定位声明**：本件由 acceptance-author 于 2026-09-11 整理落盘，事实来源 = planner 在本包
> 部署窗口（2026-09-11 10:33-11:04）于 ECS（8.137.91.16）的实测采集；整理者只做落盘核对、
> 不做任何 ECS 写操作。判红判绿标准 = 260829-hardware-validation 包
> `device-acceptance-checklist.md` 表 C。本件不含凭据。

## L1 cron 在册（绿）

部署窗口内 `openclaw cron list` 摘录（2026-09-11 实测）：

```
862ce6ab-079d-4510-aaab-9d59df66e3e8 - kidpal-reminder-plan every 15m ... isolated
```

- 在册 ✓：name=kidpal-reminder-plan、schedule=every 15m，与 DEPLOY-RUNBOOK §8.3 注册命令一致。
- **注册过程留痕**：首版 cron（id 88fceff0）载荷用系统 python3（3.6.8）执行
  `reminder_schedule_lib.py`，因该解释器不支持 `from __future__ import annotations`
  首跑即 error；已删除并重注册为 venv 解释器载荷
  （`/opt/kidpal/venv/bin/python3 /opt/kidpal/openclaw-patch/reminder_schedule_lib.py`，
  即现 862ce6ab 条目）。
- **`state.lastRunStatus=ok` 已回填（2026-09-11 ~12:00 planner 实测）**：`openclaw cron list`
  显示 `862ce6ab … kidpal-reminder-plan … ok (not delivered)`。`(not delivered)` 是投递路由
  注记（本轮 15m 周期无到期档位待投递），脚本载荷本身跑成功 = `lastRunStatus=ok`。表 C 对 L1
  的绿判据（在册且真跑过）据此满足，L1 由「部分绿」转 **绿**。

## L2 pending.json 对照契约 §三（绿）

手工核跑已产出 `/opt/kidpal/alerts/pending.json`（2026-09-11 11:04 实测：4243 字节、
19 档位）。样本条目逐字摘录（无凭据、不需脱敏）：

```json
{"fire_at":"2026-09-16T07:45:00+08:00","id":"rmd-2ca8c1aea5dd7b68e7ecfa72","speech":"今天17:00有游泳课。","title":"今天 17:00 游泳课","view":{"data":{"text":"17:00 游泳课 @游泳馆"},"view":"text"}}
```

按 `docs/contracts/interface-contract.md` §三 载荷形状逐字段核对：

| 契约字段 | 契约要求 | 样本实测 | 结论 |
|---|---|---|---|
| `id` | 确定性散列（schedule 条目 + 档位 + 事件日期），跨重推/重算不变 | `rmd-2ca8c1aea5dd7b68e7ecfa72`（确定性散列形态） | 合 |
| `fire_at` | ISO8601+08:00 | `2026-09-16T07:45:00+08:00` | 合 |
| `title` | 文案标题 | `今天 17:00 游泳课` | 合 |
| `speech` | 排程时预合成的 TTS 文案 | `今天17:00有游泳课。` | 合 |
| `view` | `{"view":"text","data":{"text":"…"}}` | `{"data":{"text":"17:00 游泳课 @游泳馆"},"view":"text"}`（键序不同、JSON 语义等价） | 合 |

结论：条目形状**逐字段合契约 §三**，L2 判绿。

## L3 设备推送（pending-human）

表 C 绿判据 = 到点真经 mcp `kidpal/alert` 推到设备（串口/front 日志摘录）。需要真实到点
档位或在 schedule.md 插入近期条目制造档位，human 在场持板观察。

`[human-回填：L3/L4 设备推送与渲染回执实测]`

## L4 渲染回执出队（pending-human）

表 C 绿判据 = 设备渲染弹窗后回执真回来且档位出队（回执摘录 + 无重复推送计数）。依赖 L3
的到点档位，同窗观察。

（回填位与 L3 共用上行占位行。）

## L5 语义产物（pending · agent 回合后补）

本次为**机械兜底态**：`semantic.json` 缺文件 → `reminder_schedule_lib.py` 按设计降级
`semantic=fallback`，文案用机械兜底（样本条目 speech「今天17:00有游泳课。」即机械兜底文案，
与 L2 摘录同源）。表 C 对 L5 的绿判据 = 真大模型语义产物（备注→带物 + 文案）落语义中间产物
且与机械兜底可区分；当前语义产物不在场——**如实标注 pending：语义产物由 OpenClaw agent
回合产出后补**（agent 回合写 semantic.json、脚本按 entry_key 挂载，DEPLOY-RUNBOOK §8 混合
分层裁定），不伪造、不把机械兜底伪装成语义产物。

## 禁字样核验（probe ≠ exercise 口径）

DEPLOY-RUNBOOK §8.5 所列四类禁字样，planner 已扫 kidpal-front 日志近 10 分钟窗口：
**0 命中**（2026-09-11 部署窗口内实测）。本件为保持证据面机械可 grep 干净，不逐字引用该
四类字样本身。

## 齿态汇总（ECS-A5 交班口径）

| 齿 | 状态 | 依据节 |
|---|---|---|
| L1 | 绿（在册 ✓ / `state.lastRunStatus=ok`，planner ~12:00 回填） | §L1 |
| L2 | 绿 | §L2 |
| L3 | pending-human | §L3 |
| L4 | pending-human | §L4 |
| L5 | pending-agent（机械兜底态如实登记） | §L5 |