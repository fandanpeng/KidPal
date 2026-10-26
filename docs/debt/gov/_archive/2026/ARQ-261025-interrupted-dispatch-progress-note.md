---
id: ARQ-261025-interrupted-dispatch-progress-note
title: 子 agent 被 429 / 配额中断后无回执——恢复靠 git status 考古，进度注记未成约定
domain: gov
status: closed
severity: COULD
owner: architect（派发契约文面）
trigger: 下一治理批（动 `planner.md` 派发步 / 子 agent brief 模板时并批）；或再出现一次中断恢复
opened: 2026-10-25
closed: 2026-10-26
closedBy: 261026-gov-review-final-phase-dispatch-contract
paths: [".claude/agents/planner.md", ".claude/references/dev-loop.md"]
source: [delivery/retro/2026-10-25-loop-retro.md, delivery/taskpackages/261019-p3-record-dialogue-guide/pod-retro.md, delivery/taskpackages/261022-m5-k3-honcho-memory-production/pod-retro.md]
---

**证据**：① `261022-m5-k3-honcho-memory-production`：acceptance-author 派发被 API 429（周配额）拦腰，冻结相位主体（两验收件 1741 行）已落盘、收口回填未做；429 中断的 agent 不留任何回执，planner 只能靠 `git status` 考古盘点痕迹，再决定续派还是接手。retro 原话：「若派发模板约定『中断前先落进度注记』会更省」。② `261019-p3-record-dialogue-guide`：上会话 HTTP429 中断在验收冻结收尾，恢复时需逐件核验 current 指针与 canonical 投影的字节级刷新，额外付出核验成本。

**影响面**：凡长时子 agent 派发（acceptance-author / builder / fixer / integrator）。429 与配额是环境常态，每次中断都重新付一遍「盘点什么已落、什么没落」的成本，且容易误判为未完成而重做。本波 2 包命中。

**建议改动**（交 review、不自批）：派发契约加一条轻量约定——子 agent 在长任务中每完成一个可交付段落，往包目录写一行进度注记（或 pod-events `friction` 带 `progress` 字段）；planner 恢复时先读注记再考古。不新增门、不要求子 agent 额外派发，只把「中断前留痕」写成惯例。

**风险**：riskTier: low——纯派发文面与惯例，不进任何闸判据。

---

**结项注记（2026-10-26，closedBy = 261026-gov-review-final-phase-dispatch-contract）**：随 P1.5 批 T3 结项——OQ1 经 human 裁 (a) 窄写权，scope-map 对 acceptance-author / builder / fixer 各加 `delivery/taskpackages/*/dispatch-progress-*.md` 单文件形 allow（deny 零改动）；planner.md 派发书立「进度留痕契约」（自包含快照、Write 覆盖、惯例非门），dev-loop.md 文件地图增 `dispatch-progress-<agent>.md` 行。候选「pod-events friction 带 progress 字段」形态经评审弃用（frictionType 闭集，progress 非摩擦）。
