---
id: ARQ-260923-contract-merge-deadpath
title: 契约面 diff 的 merge-driver 机械路径整条封死——升级闸契约门证据硬编码 false 恒 BLOCK，契约包只能现场即兴人合，既无机械消解路径也无成文人合 runbook
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一治理批（可与 ARQ-260923-dispatch-prompt-hard-terms 并批——同动 merge / planner 编排文面）
opened: 2026-09-23
closed: ""
closedBy: ""
paths: []
source: loop-retro-2026-09-23（260923-m3-k7 pod-retro 卡点 6 + pod-events merge-completed 事件；报告 delivery/changes/retro/2026-09-23-loop-retro.md）
---

**证据**（260923-m3-k7-schedule-grid-restore 实证，2026-09-23）：升级闸内部 `hasContractGateEvidence` 硬编码 `false`（源码自注「后续接入；现 false → 契约面 diff 恒 BLOCK（向严）」）→ 凡碰 `docs/contracts/**` 的包 merge-driver 形同虚设。K7 实走 = pre-merge 预检（零重叠）+ rebase 后 `git merge --ff-only` 人工合并，pod-events merge-completed 事件自记「非 merge-driver：升级闸对契约面 diff 恒 BLOCK（hasContractGateEvidence 占位未接入），human 直接指示合并」。对照已结 ARQ-260902-escalate-merge-no-human-path——该条为 ESCALATE 人批建了 `--human-approval-ref` 机械路径，本条是其孪生缺口：ESCALATE 有人批通道，契约 BLOCK 无任何通道（既不接证据放行、也没有「契约包走人合 runbook」的显式改道条文），每个 planner 都要现场发现一次；且绕开 merge-driver 的 Step 5b 簿记链（K7 用 pod-events 代记补上，靠自觉）。**影响面**：每个契约包合并时（频率低、每次都是即兴操作）。**建议改动**（候选，交 review，不自批，二选一）：① 接入契约门证据——升级闸读 ContractChangePacket 批准凭据（human 契约门签字事件锚，与 `--human-approval-ref` 同构），证据在场即放行（向严接证据）；② 最小改道——merge-driver 对契约面 diff 显式输出「契约包 → 人合 runbook」指引（固定步骤：pre-merge 预检 / human 批准事件落 pod-events / ff 与 no-ff 选定 / Step 5b 簿记等价代记），把 K7 的即兴操作固化成条文（半径小）。**riskTier: low-medium**。owner=architect。
