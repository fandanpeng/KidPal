---
id: ARQ-261025-wait-state-closed-set
title: board / launch-card 状态闭集缺「等人操作」一等语义——精确停等只能降级 blocked + 事件承载
domain: gov
status: closed
severity: COULD
owner: architect（状态语义）+ dev-loop（`portfolio-reconcile.mjs`）
trigger: 下一治理批（动 `portfolio-reconcile.mjs` / `pod-coordination.md` 状态面时并批）；或下一次真机门 / 合并门停等
opened: 2026-10-25
closed: 2026-10-27
closedBy: 261026-gov-review-p4-pod-board-cluster
paths: [".claude/scripts/portfolio-reconcile.mjs", ".claude/references/pod-coordination.md"]
source: [delivery/retro/2026-10-25-loop-retro.md, delivery/taskpackages/261016-sd-card-mount/pod-retro.md, delivery/taskpackages/261021-p3-record-cancel-lifecycle/pod-events.jsonl]
---

**证据**：① `261016-sd-card-mount`：launch-card 写 `awaiting-human:real-device`，`portfolio-reconcile.mjs` 的 `POD_STATUSES` 闭集（`.claude/scripts/portfolio-reconcile.mjs:36`）无该值，`--set-pod` fail-loud 拒收；只能降级为闭集合法的 `blocked`，精确停等语义由 pod-events `agent-escalation` 承载（friction `degraded`，retro 明确「建议留给治理面：闭集是否该为『等人操作』单列一档」）。② `261021-p3-record-cancel-lifecycle`：同类停等出现 `awaiting-human:merge-gate` 与 `awaiting-human:real-device` 两个语义，均未进闭集。

**影响面**：所有需要 human 持板 / human 批合并 / human 裁定的停等。当前 board 只能显示 `blocked`，人或外部脚本无法从 board 区分「等真机」「等合并批准」「等裁决」，只能翻 pod-events。本波 2 包命中，且 261016 上一轮就记过同一摩擦。

**建议改动**（交 review、不自批）：评估把「等人操作」提升为一等状态（如 `awaiting-human:<reason>` 受控枚举），或至少让 `--set-pod` 接受带冒号语义的等待态并原样投影到 board；`pod-events` 仍承载精确事由，不改其权威面。若担心膨胀闭集，可只加 `awaiting-human` 一档 + 事件侧 reason。

**风险**：riskTier: low——只动调度态枚举与 board 投影，不改任何门判据。

**2026-10-27 · 结项（`261026-gov-review-p4-pod-board-cluster`，整条闭合）**：`POD_STATUSES` 扩受控族 `awaiting-human[:<reason>]`（reason 限 `[a-z0-9-]+`），读侧归一 / 写侧校验 / token 释放三处同批改齐（board 原样投影、wip 原值分桶、等待态不占 merge token）；**merge-gate 停等一并打通**（human OQ2=(b) 2026-10-27 裁，riskTier 如实升 medium）：`merge-driver.mjs` `approvalRefUnlock` 解锁判据扩为 `blocked ∪ awaiting-human 族`（import 单源），planner 合并门 ESCALATE 分支设值改 `awaiting-human:merge-gate`——放行权不在 status 档位、单源在五要素人批事件核验（`resolveHumanApproval`），族内任一档无合法人批一律 `kept-blocked`（T1d/T1e 夹具钉死），前置升级闸零触碰（等价性链经判官独立复核成立）。V4 四态 token 矩阵实测判别（ready-to-merge 正控保留、awaiting-human 两形态与 blocked 均释放）。无留根。
