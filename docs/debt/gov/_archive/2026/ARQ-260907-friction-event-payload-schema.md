---
id: ARQ-260907-friction-event-payload-schema
title: pod-events friction 事件 payload 无最小 schema 校验：空 payload 证据丢失、非标键归 unknown、ledger=0/retro=N——retro 覆盖比对的机器侧失真
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-07
closed: 2026-09-07
closedBy: ""
paths: []
source: ""
---

pod-events-aggregate.mjs aggregateFriction 新增 malformedFrictions 显式告警列（空 payload / 缺 frictionType / 缺 summary 三判据，unknown 兜底保留双轨）；schema 成文进 DEV-GOVERNANCE「## friction 事件最小 schema」+ planner.md 引用行。真数据重放：voice-roundtrip 两条 empty-payload 与 ws-switch 两条缺键全命中（55a43813 类证据丢失病灶兜住后车）。 证据：[260907-gov-review-arq-governance-batch-2](../changes/arch/260907-gov-review-arq-governance-batch-2/)。

**main 侧追注同步（2026-09-24 main→gov 回同步 merge 迁入；来源 = loop-retro-2026-09-23 第 3 轮对旧队列行的追加证据，逐字搬运）**：**追加证据（loop-retro-2026-09-23，18 包复盘——本行 landed 的是检测面，写入口未闭合且暴露更深覆盖缺口）**：① 空 payload 摩擦继续产生且全被 malformed 告警列亮出（k1a×2 / k4-reminder×2 / n1b×1 / m3-k4×7 empty-payload）——检测在工作、产生侧未收敛；② **零事件型覆盖缺口（malformed 检测结构性拦不住）**：K2（keybar-three-slot）与 K7（schedule-grid-restore）的 pod-events 仅含 human-decision / merge 类事件，dispatch=0、friction=0、escalation=0，而两包 pod-retro 散文摩擦主题极丰富（K2：烧录偏移错位 / 字体缺形 / 捕获窗空转两窗 / fixer 三轮 bisect；K7：闸形状红两轮 / 子代理 Write 被拦三次 / 写权代落 / merge-driver 契约封死）——GC3 覆盖比对「ledger=0 vs retro=N」型偏差的现行实例，按纪律偏差不采信任一源；③ escalation 事件 resolved 闭合链缺：k3-char-window / card-fix / display-fix 的合并门 ESCALATE 事件在 human 批合并（pod-events approve-escalated-merge 与 merge-completed 事件在案）后仍标 unresolved——机器聚合永久高估未结数。**建议（候选，交 review，不自批）**：pod-events 写入口纪律成文（friction / dispatch 事件「当日落」，K6 起相关建议已多棒口头滚动）+ escalation 事件支持被后续 human-decision 引用闭合。证据：[loop-retro-2026-09-23 报告](../changes/retro/2026-09-23-loop-retro.md)。
