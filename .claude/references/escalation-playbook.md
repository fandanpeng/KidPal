# 升级手册（escalation-playbook）

把 junior 的卡点决策从“想”变“查表”。三类瓶颈，照表走，不临场判断。

## 1. 故事歧义
- 子系统/验收点绑不上 → AskUserQuestion，不猜
- 验收点缺真库口径 → planner 标 `mutationRequired`，acceptance-author 写真断言
- 需求方向不明 → 升人类，不让 builder 先跑

## 2. 风险升级（supervisor 决定，照 nextAction 做）
| verdict | junior 动作 |
|---|---|
| APPROVE | planner 前置升级闸放行后 `--pod` 本地合并，不直推 main |
| BLOCK + 补证据/补测试/改实现 | 退 fixer，单 gate 修，回 verifier；同一 changeId 连续 2 轮 BLOCK 未转绿 → 熔断升级（ESCALATE），不外环硬磕 |
| ESCALATE | 停，交人类；碰 core.*/契约/迁移/删数据必升。人批后走 `merge-driver --human-approval-ref`（见 planner.md 合并门段） |

## 3. verifier 反复红
- 同 fingerprint 连续 2 轮 或 触 fixBudget=3 → 熔断升级，不硬磕
- 红基线没确认 → 退 acceptance-author，先确认红
- 要改契约 → 进 CONTRACT-DECISION-QUEUE 异步批，切别的 worktree 不空等
