---
id: ARQ-260923-contract-merge-deadpath
title: 契约面 diff 的 merge-driver 机械路径整条封死——升级闸契约门证据硬编码 false 恒 BLOCK，契约包只能现场即兴人合，既无机械消解路径也无成文人合 runbook
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: 下一治理批（可与 ARQ-260923-dispatch-prompt-hard-terms 并批——同动 merge / planner 编排文面）
opened: 2026-09-23
closed: 2026-09-24
closedBy: 260924-gov-review-settings-deny-contract-merge T3（OQ-3=(a) ②最小改道 runbook，human 裁定 2026-09-24；建议① 接证据留根带量化触发器 → pod-coordination §10e 边界与留根段）
paths: []
source: loop-retro-2026-09-23（260923-m3-k7 pod-retro 卡点 6 + pod-events merge-completed 事件；报告 delivery/changes/retro/2026-09-23-loop-retro.md）
---

**证据**（260923-m3-k7-schedule-grid-restore 实证，2026-09-23）：升级闸内部 `hasContractGateEvidence` 硬编码 `false`（源码自注「后续接入；现 false → 契约面 diff 恒 BLOCK（向严）」）→ 凡碰 `docs/contracts/**` 的包 merge-driver 形同虚设。K7 实走 = pre-merge 预检（零重叠）+ rebase 后 `git merge --ff-only` 人工合并，pod-events merge-completed 事件自记「非 merge-driver：升级闸对契约面 diff 恒 BLOCK（hasContractGateEvidence 占位未接入），human 直接指示合并」。对照已结 ARQ-260902-escalate-merge-no-human-path——该条为 ESCALATE 人批建了 `--human-approval-ref` 机械路径，本条是其孪生缺口：ESCALATE 有人批通道，契约 BLOCK 无任何通道（既不接证据放行、也没有「契约包走人合 runbook」的显式改道条文），每个 planner 都要现场发现一次；且绕开 merge-driver 的 Step 5b 簿记链（K7 用 pod-events 代记补上，靠自觉）。**影响面**：每个契约包合并时（频率低、每次都是即兴操作）。**建议改动**（候选，交 review，不自批，二选一）：① 接入契约门证据——升级闸读 ContractChangePacket 批准凭据（human 契约门签字事件锚，与 `--human-approval-ref` 同构），证据在场即放行（向严接证据）；② 最小改道——merge-driver 对契约面 diff 显式输出「契约包 → 人合 runbook」指引（固定步骤：pre-merge 预检 / human 批准事件落 pod-events / ff 与 no-ff 选定 / Step 5b 簿记等价代记），把 K7 的即兴操作固化成条文（半径小）。**riskTier: low-medium**。owner=architect。

**批 B 起草期实测追注（2026-09-24，主线程，只读实测，供 260924-gov-review-settings-deny-contract-merge 批消费）**：①处方落点复证仍在、未被后续批消化：`hasContractGateEvidence` 硬编码 `false` 两处在场——`pre-merge-escalation-gate.mjs:366`（live buildCtx，带原自注「契约门证据（contract-change-packet）后续接入；现 false → 契约面 diff 恒 BLOCK（向严）」）+ `:393`（self-check mkCtx 夹具默认）；消费点 `:263`（通用 1：`ctx.contractsDiff && !ctx.hasContractGateEvidence` → BLOCK）。②merge-driver 现行为核实：Step 1b（`merge-driver.mjs:1250-1282`）gate BLOCK → `warn` 打印闸输出末 12 行 → `setPod blocked` → `return false`，**无任何人合改道指引**；Track B gov 档为闸直跑形态（protocol 步骤 7），BLOCK 后同样无成文路径。③trigger 行「可与 ARQ-260923-dispatch-prompt-hard-terms 并批」线索**已过时**：该条已结项（frontmatter status: closed，closedBy = 260924-gov-review-planner-orchestration-align T3，件已归 `_archive/2026/`）——本条由 260924-gov-review-settings-deny-contract-merge 批（批 B）单独承载。④孪生先例形态核实：`ARQ-260902-escalate-merge-no-human-path`（closed 2026-09-05）的消解 = 260904 批为升级闸建 `--human-approval-ref <eventId>` 五要素机械证据口——选项①即其同构复刻。⑤runbook 候选宿主核实：`pod-coordination.md` 已有 §10a（merge 冲突消解 runbook）/ §10b（post-merge 簿记落点口径）/ §10c（.vN 槽位）/ §10d（并行相交面分级路由）——选项②的 runbook 条文以新增 §10e 同族落位最顺；K7 实走的人合操作面与 merge-to-main skill 步骤同构（pre-merge 预检 + ff-only）。

**处置结项（2026-09-24，260924-gov-review-settings-deny-contract-merge 批 B landing，OQ 裁定与 GREEN 凭据见该批 PROPOSAL FROZEN v0.4 Status 行）**：二选一已由 human 裁定 OQ-3=(a) = **建议② 最小改道**——`pod-coordination.md` 新增 **§10e「契约包人合 runbook」**四步成文（pre-merge 预检〔merge-to-main 步骤 1–2 同形 + 契约门凭据在场确认〕/ human 批准事件落账〔dev Pod 档 pod-events `human-decision`、decision=`approve-contract-merge-manual` 本节首立口径——K7 实走系复用 `approve-escalated-merge` 事件作批准锚（eventId c6464758）；Track B 治理档 = PROPOSAL Status 凭据段 + landing-summary〕/ ff 与 no-ff 选定〔Track B 恒 ff-only、dev Pod 通道 --no-ff；含 K7 ff-only 即兴偏差的 canonical 收敛限定句〕/ Step 5b 簿记等价代记三件套〔pod-events merge-completed 代记 + changes-index upsert + merge commit message 注明批准锚〕）+ **两处机械指针**（升级闸通用 1 reason 行、merge-driver Step 1b 条件 warn——条件钉通用 1 前缀逐字，通用 4 contract-findings-unaccounted 不误触发）。**闸判定语义零变更**：恒 BLOCK 向严保持，self-check 33 例改动前后全绿（机械证明）。**建议①（接入契约门证据，与 `--human-approval-ref` 同构）留根带量化触发器**：契约包频率上升或 §10e 实走摩擦 ≥1 次实测，届时另批立项（§10e「边界与留根」段 + 该批 PROPOSAL §6 留根 3）。证据：`delivery/changes/arch/260924-gov-review-settings-deny-contract-merge/`（R3 GREEN）。
