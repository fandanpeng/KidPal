---
description: 强制默认实现入口。驱动一个 story / arch-review handoff / CI 或 drift finding 走完整开发期自治循环：intake/slicing → 验收作者 → builder →（verifier↔fixer 内循环）→ integrator/docs-sync → supervisor 合并门。
argument-hint: <issue/story/handoff/finding，例如 "S-F1.4 孩子到点收到日程提醒弹窗" 或 "delivery/changes/arch/arch-review-S1-leads/dev-loop-handoff.json">
---

# /build-feature

你现在是 **planner（主线程编排者）**。按 `.claude/agents/planner.md` 的规则推进。
这是正式实现工作的默认入口；不要由主 AI 直接改 `firmware/**`。
本次目标输入：

> $ARGUMENTS

逐步执行，并在每一步显式说明「派给谁、为什么、期望产物」：

**launch 面预检（Step 1 之前执行；仅 changeId 直启形状触发，260924-gov-review-planner-orchestration-align T1）**：输入为 launch-argument 形状（`/build-feature <changeId>`，即 planner.md Step 1① 的 launch-argument 形状）时，先核 launch 面**五件**——① board 条目（`$(git rev-parse --git-common-dir)/devloop/pods/<changeId>.json` 在场）；② 启动卡（`delivery/taskpackages/<changeId>/launch-card.md` 在场）；③ 运行时 worktree（board 条目 worktreePath 已记录且该路径实为 git worktree）；④ 专属图索引（`list_projects` 在册含该 worktree 路径派生的项目条目，ARQ-260905 launch 面必办）；⑤ spine 上游面（`delivery/changes/intake/<changeId>.md` 在场时核——其 facePlan 中先于实现面的各面 anchors 已回填；未回填 → 不进 intake，报 `awaiting-upstream`，先完成上游面）。**五件齐 → 原样进 Step 1 intake；任一缺 → 不进 planner intake**，先按 `.claude/skills/kidpal-pod-launch-prep/SKILL.md` 把 ② launch 面准备阶段补齐（该 skill「执行主体 = 主线程（human 会话）」条款与本命令运行位同口径）再进 Step 1。raw / human-ad-hoc 无 changeId 形状不触本预检。本预检是 **command 文面路由、非机械闸**：不改 planner.md「启动卡缺失不设门」既有裁定（那治 Pod 内侧交接断裂保护，本段治主线程入口外侧）；若未来把本预检升为 verify-gate / hook 判据即构成新增 gate，须按硬排除走 full 评审，非本条预批范围。

1. **Source intake（先 portfolio handoff 探测，再归一）**：先按 `.claude/agents/planner.md` Step 1 ① **portfolio handoff 探测（F-02 两段契约）**——launch-argument 形状（`/build-feature <changeId>`）按 changeId 探测 `delivery/taskpackages/<changeId>/taskpackage.json` / `devloop/pods/<changeId>.json` / `origin.upstreamRef`→PROPOSAL；命中已存在 portfolio taskpackage → 以其 `sourceType/origin/rigor/riskTier/gateProfile/decisionCoverage` 为权威消费（**不重归一、不覆盖 sourceType**）；交接断裂（命中 board/workstream 但 checkout 无 PROPOSAL/taskpackage）→ fail-closed `needs-triage` + Red Flag，**勿静默归一 human-ad-hoc**。然后按 `.claude/references/task-intake-and-slicing.md` 归一输入来源。若输入是 `delivery/changes/arch/**/dev-loop-handoff.json`，先读 handoff；若是 review finding/CI/drift/临时任务，生成稳定 `sourceRefs[]`。确定 `sourceType`、`sourceRefs[]`、`deliveryImpact`、`planningImpact`、`riskTier`、`gateProfile[]`、子系统与 acceptanceRefs。信息不足就用 AskUserQuestion 问人，别猜。
2. **契约判定**：任何疑似需要改 `docs/contracts/**` / events / schema / mcp-tools / shared-types / 系统 spec 的点，先派 `architect-reviewer` 预审。只有 reviewer 判定必须改契约时，才停在契约变更门交给人类架构师。
3. **Slicing + Planning 预检**：根据 intake 判断 Lite TaskPackage / Full TaskPackage / 挂接已有 `delivery/workstreams` / 新建 `delivery/workstreams` / `needs-triage`。`planningImpact=needs-triage` 或 handoff `blockingDecisions[]` 未解决时停下；不得派 builder。**portfolio handoff 断裂（F-09，changeId 命中 board/workstream 但 checkout 无 PROPOSAL/taskpackage）→ needs-triage fail-closed，不得派 builder（对偶 planner.md Step 1 fail-closed guard）**。开工前确认 `.claude/scope-map.json` 已允许本包验收与实现路径，避免中途补 scope。
4. **Draft TaskPackage**：产出包含 `changeId`、`sourceType`、`sourceRefs[]`、`deliveryImpact`、`planningImpact`、`riskTier`、`gateProfile[]`、`acceptance[]`、`exitCriteria[]`、`iterationBudget`、`fixBudget`、`contractRefs[]`、`docRefs[]`、`planningRefs[]`、`mutationRequired` 的草案；若 `sourceType === 'proposal'` 则必须同时写入 `decisionCoverage[]`（使用 `sourceRefs[]` 数组，proposal `.md` 路径必须入列且同时出现在 `docRefs[]`）。按风险补 `securityReviewRequired`、`securityAxes`、`adrRequired`，并判定是否需要 `s2ExploratoryRequired` / `s2ExploratoryReason` / `s2ExploratoryGoals[]`。若需要 official S2 scan，goal 必须声明 `targetPersona`、provenance metadata 所需的 `changeId/taskPackageId/goalId/triggeredBy`，并默认 `allowDestructive=false`；允许破坏性探索时必须声明 `destructiveScope` 与 `safetyReason`。`maxSteps` / `budget` 可省略，Phase 1 bootstrap 默认不限。必须立刻写入 `delivery/taskpackages/<change-id>/taskpackage.json`，并同步刷新 `delivery/taskpackages/current/taskpackage.json`；不能只在聊天里展示。
   - **条件升级触发器摘要**：按 `planner.md` Step 5b 对 `sourceType=proposal` 做自检；触发形状仅 `return` / `propose-drop` / `notify` 三种。若无一命中则自主推进；缺少完美断言本身不是 notify 触发条件。notify 触发前必须完成至少一次有记录的锐化尝试并产出 `delivery/workstreams/<id>/notify-decisions.md`。非交互环境（CI、自动化 Pod 启动）下 `AskUserQuestion` 不可用时，必须 fail-closed：写入 `delivery/workstreams/<id>/pending-human-ruling.md` 并以 `needs-triage` 标记退出，不得静默推进。
5. **先写验收**：派生 `acceptance-author` → 产出冻结的验收测试、覆盖的 acceptanceRefs/sourceRefs、红基线命令与红基线结果。
6. **Finalize TaskPackage**：补齐 `acceptanceTestsFile[]`、`redBaselineCommand`、`redBaselineResult`、`docsSyncRequired`、`docsOwner: "integrator"`，并把最终版重新写回 `<change-id>/taskpackage.json` 与 `current/taskpackage.json`。缺任何一项或缺落盘记录，不得派 builder。
7. **实现**：派生 `builder`（传完整 TaskPackage）→ 实现 + 单元测试，让冻结验收从红转绿。
8. **内循环（你来驱动，禁止嵌套派生）**：
   - invoke `verify.mjs` → 拿 `{ ok, results, error }`。
   - 红 → 把 `error`（ErrorEnvelope）交 `fixer` 打补丁 → 再 invoke `verify.mjs`。
   - 直到绿，或满 3 轮 → 升级给人类（附最后 ErrorEnvelope + 试过的假设）。
9. **集成 + 文档同步**：全绿后派生 `integrator` → docs-sync、架构/依赖卫生检查、安全证据收口、必要 ADR 归档、范围圈定终验（final 按 scope-file 影响面圈定跑；全量回归 = 人工触发位）；若 TaskPackage 声明 `s2ExploratoryRequired=true`，在 final 复验（scope 圈定）/e2e 绿后、Review Packet 定稿前执行 S2 official scan（必须传 ARQ-062 typed fields；默认 `wait=true`，调用层 600 秒上限，最低证据是 structured runResult）；产出标准 `delivery/changes/dev/<change-id>/` 制品、Review Packet + 备可回滚路径。
   - **里程碑完工度（dogfood）**：若本次收口的 change 触发里程碑阶段跳变（首刀进 `in_delivery` / 收尾 `→ ready_for_acceptance → accepted`），按 `delivery/milestones/milestones.jsonl` 头注释改对应 `state`，跑 `node .claude/scripts/delivery-status.mjs` 重生成 `delivery/milestones/STATUS.md` 并入 Review Packet；中段 change 不动状态、`STATUS.md` 不手改。
   - **closeout 闸**：integrator 必须运行 `node .claude/scripts/closeout-check.mjs --update-index`，这是唯一授权的 closeout index projection 写路径；缺 `delivery/changes/dev/<id>` 三个 blocking 制品（taskpackage/docs-delta/review-packet）、`delivery/milestones` STATUS 漂移、`delivery/workstreams` workstream 缺失则红，verdict/verification 机器件缺失只 advisory。闸绿后在 feature 分支 commit 已绿改动（不 push、不在 main/master commit），供合并门复核已 commit 的 diff。
10. **Supervisor 合并门**：跑 `supervisor-verdict-gate.mjs` 机械闸（planner invoke，不派 supervisor agent）→ 保存 `delivery/changes/dev/<id>/supervisor-verdict.json`。supervisor 输入必须包含 TaskPackage source/risk/gate profile、verifier 证据、Review Packet、closeout 结果与已 commit diff。APPROVE 跑前置升级闸（`node .claude/scripts/pre-merge-escalation-gate.mjs`）→ 放行则 `node .claude/scripts/merge-driver.mjs --pod` 本地合并（可回滚）；BLOCK 退 fixer/planner（同一 changeId 连续 2 轮 BLOCK 未转绿 → 熔断 ESCALATE，不外环硬磕）；ESCALATE 才停人类 review/合并。人批后机械路径：planner 落 pod-events 人批事件（`payload.decision="approve-escalated-merge"`）→ `merge-driver --pod <changeId> --human-approval-ref <eventId>`（见 planner.md 合并门段）。你不直推 main、不改契约、不自评通过。

> 真并行多个子系统：见 [.claude/references/dev-loop.md](../references/dev-loop.md)（多 worktree + 多会话），不要在本会话里同时派多个 builder。
