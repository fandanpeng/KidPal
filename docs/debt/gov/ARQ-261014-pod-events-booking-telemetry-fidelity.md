---
id: ARQ-261014-pod-events-booking-telemetry-fidelity
title: pod-events 簿记一致性与遥测保真度——escalation 闭合链缺、actor 归属误落、同决定双记、关联字段失效、模型档位与用量数据失真
domain: gov
status: deferred
severity: SHOULD
owner: architect（schema 约定）+ dev-loop（脚本落地）
trigger: 下次治理批 / 下个动 pod-events schema 约定或 pod-events-aggregate.mjs 的批
opened: 2026-10-14
closed: ""
closedBy: ""
paths: [".claude/scripts/pod-events-aggregate.mjs", ".claude/agents/planner.md"]
source: loop-retro-2026-10-14 18 包复盘（`delivery/changes/retro/2026-10-14-loop-retro.md`）
---

**证据（六类实据，loop-retro-2026-10-14）**：① **escalation-resolved 缺失**：261011-home-v5 `fe6c3f46` / `930257e5`、261012-realtime-facts `81b32bfb` 均由 human-decision 实际闭合但未补 agent-escalation-resolved 事件，聚合器把它们计为 unresolved（同族先例：loop-retro-2026-09-23 已录 k3-char-window / card-fix / display-fix 三例）——机器聚合永久高估未结数；② **actor 归属误落**：261001-m4-k3 `04f83250`（actor 误落 planner → `91bb853b` 归属修正重落，批内唯一簿记纠偏型事件）、260924-perf-k1 `13c53d7b`（actor=planner、source=human，与 K3 同病）；③ **同决定双记**：261001-openclaw `a5c3b27a`（actor=planner）/ `c3d96131`（actor=human）为同一批准的双记；④ **关联字段失效**：260928-char-window-key `4368504c` 用 `resolves` 字段而非 `refEventId` → 聚合器把 `570eb95d` 判 unresolved（实义已经 human `84c4e953` 消解）；⑤ **时间戳倒挂**：261012-realtime-facts `9d4c1f68`（06:45Z）晚于引用它作 approval-ref 的 merge-completed `7ed9f75d`（06:36Z），事后补记痕迹；⑥ **模型/用量数据失真**：260924-m4-k1 两次显式 [MODEL-TIER] opus 升档实际模型均 deepseek-v4.1-flash（byModel 五 dispatch 全 deepseek，retro 只留痕不裁决）、260924-perf-k1 漏升一次 + dispatch-stamp 传 0 充数（`054f8ed8` / `f56287b2` 同 dispatchId 双记、前者 durationMs/toolUses 填 0）、261013-tomorrow-row 全部 10 条 dispatch 的 mainThreadUsage 逐字段相同（cumulativeDelta 全 0，主线程 checkpoint 不可信）。

**影响面**：机器聚合（escalation 未结数 / token 覆盖 / mainThread 用量 / 模型分布）系统性失真，loop-retro 每轮 GC3 需人工补正；MODEL-TIER 升档链路「声明 ≠ 实际」使分层出场成本核算失去数据基础。

**修法候选**（交 review，不自批）：① escalation 事件支持被 human-decision 引用自动闭合（loop-retro-2026-09-23 已提，至今未落地——聚合器按 refEventId / approval-ref 关联即可）；② schema 校验收敛 actor 归属与关联字段（refEventId 单一写法，`resolves` 弃用或聚合器映射兼容）；③ MODEL-TIER 声明与 transcript 实际模型对账进 dispatch-stamp（不一致即 warn）；④ mainThreadUsage 快照刷新机制排查（261013 实证）。**riskTier: low**（纯记账 / 遥测面，不动任何门判据）。
