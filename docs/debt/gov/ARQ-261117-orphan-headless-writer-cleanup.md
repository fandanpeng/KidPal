---
id: ARQ-261117-orphan-headless-writer-cleanup
title: headless planner 被停后进程未清场即重拉——存活旧进程与新会话的 acceptance-author 并发写同一冻结验收面
domain: gov
status: deferred
severity: SHOULD
owner: architect（launch / 重拉纪律文面 + 可能的进程面检查脚本）
trigger: 下一个动 launch-prep skill / planner 重拉流程的治理批；或再出现一次双写冻结面
opened: 2026-11-17
closed: ""
closedBy: ""
paths: [".claude/skills/kidpal-pod-launch-prep/SKILL.md", ".claude/agents/planner.md"]
source: ["delivery/taskpackages/261104-k0a-four-page-skeleton/pod-retro.md 踩坑 1", "delivery/taskpackages/261104-k0a-four-page-skeleton/pod-events.jsonl eventType=orphan-writer-resolved", "delivery/retro/2026-11-17-loop-retro.md §二 T6"]
---

**证据**：261104-k0a 踩坑 1 逐字：「**孤儿双写手**（最重）：前两次被停的 headless planner 进程存活，其 acceptance-author 与新会话派出者并发写同一冻结验收面——acceptance-author 停点上报 + planner 定点 taskkill（按 CommandLine 鉴别）+ 已落盘产物在盘评审后接管，不推倒重来。**共性**：headless 拉起被停后必须进程面清场（查残留 `claude -p` 进程）再重拉。」该包 pod-events 有 `orphan-writer-resolved` 事件留痕。同批 261028-m6-k1 / k2a 的配额 / 网关中断续跑均靠 `git status` 读地面真值（ARQ-261025-interrupted-dispatch-progress-note 已结项的正面兑现），但**进程面清场**不在那条覆盖内。

**影响面**：一切 headless / 可被 human 中断的拉起形态（本周期 K0a 是 VS Code 会话 + 两次被停）。双写同一冻结面时，两个 writer 都看不到对方；后果是验收基线被静默改写成混合体，或者后到的 writer 覆盖先到的结论。本包靠 acceptance-author 主动停点上报接住，纯属幸运——那次停点恰好发生在写冲突可见的相位。

**建议改动**（候选，交 review、不自批）：① launch-prep / 重拉 checklist 增加进程面清场步：查残留 `claude -p` / headless planner 进程（按 CommandLine 含 changeId 鉴别），有则先终止再重拉；② 被停会话恢复时先 `git status` + 盘上产物核验再续跑（已有纪律，与 ① 并列写死）；③ 评估做成 launch-prep 的一个固定脚本检查（只读列出残留进程，不做自动 kill——终止动作仍归 human / planner 显式确认）。

**风险**：riskTier: **low**——纪律文面 + 只读检查脚本；不做自动 kill（避免误杀其它会话）。review profile：文面 = 轻路径候选；脚本 = Track B。