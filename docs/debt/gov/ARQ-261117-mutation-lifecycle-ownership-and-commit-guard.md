---
id: ARQ-261117-mutation-lifecycle-ownership-and-commit-guard
title: mutationEvidence 生命周期三缺口——perAcceptance 投影无 owner、主线程可在变异态下 commit、MASKED 条目不复测
domain: gov
status: deferred
severity: SHOULD
owner: architect（派单 / 提交纪律文面）+ dev-loop（报告模板执行面）
trigger: 下一个动 planner / builder 派单模板或 verify 前置检查的治理批
opened: 2026-11-17
closed: ""
closedBy: ""
paths: [".claude/agents/planner.md", ".claude/agents/builder.md", ".claude/references/taskpackage-authoring.md", ".claude/hooks/verify.mjs"]
source: ["delivery/taskpackages/261028-m6-k3-p3-story-typography/pod-retro.md F3", "delivery/taskpackages/261104-k3-qq-points-channel/pod-retro.md 踩坑 3", "delivery/taskpackages/261028-m6-k2b-p2-schedule-16px/pod-retro.md F2 / F3", "delivery/retro/2026-11-17-loop-retro.md §二 T3"]
---

**证据（三缺口）**：
1. **perAcceptance 投影无 owner**。261028-m6-k3 F3 逐字：「verify r1 红 = mutation-check 报 7 条 missingSensitive——builder 变异取证齐全，但 taskpackage.mutationEvidence.perAcceptance 投影没人写（builder 不写 taskpackage、acceptance-author 不掌变异、planner finalize 时点早于变异取证完成）」。261104-k3 踩坑 3 逐字：「mutationEvidence.perAcceptance 连续两包留到 verify 才补（K2 同型，K3 复发）：Finalize 时只写了 note，verify.mjs 的 mutation-check 直接红」。261104-k4 遗留同族：「verdict.json advisory：本轮 bearing 事实是 rounds/r1.json，current/verdict.json 未产」。
2. **变异态跨 commit 边界**。261028-m6-k2b F2 逐字：「主线程在 builder 变异轮进行中提交实现轮 commit，把 M3 变异值 `ksg_slot_caps {4,4,4}` 带进 HEAD → 根因 = 提交时序与子 agent 挂着的临时变异态无机械互斥，主线程 commit 前未核工作树是否处于变异态 → 修复 = 18ff3a22 还原 {4,4,3} + M3/M6 修订后复测转红证据回填」。pod-retro 自提「新铁律候选：变异态不得跨 commit 边界」。
3. **MASKED 不复测**。261028-m6-k2b F3 逐字：「M3（容量回退）与 M6（下划线 2→3px）首测 MASKED——当时冻结件缺陷齿先中断、变异齿没跑到。冻结件修订轮修掉缺陷齿后复测，两条均转正……MASKED 不是终点：遮挡齿消除后必须复测，否则变异证据缺两条、判别力覆盖虚假齐全」。

**影响面**：每个带 functional 验收的包（mutationEvidence 必填）。缺口 1 让 verify 必红一轮（round 序数不可回收）；缺口 2 会让变异值随 commit 进入主干并以「已还原」的叙事掩盖（本包靠 18ff3a22 事后钉回，没有机械拦）；缺口 3 让判别力覆盖表虚假齐全，评审无法从证据面看出「这条变异没被真跑过」。

**建议改动**（候选，交 review、不自批）：
1. builder 交付报告模板增加 `perAcceptance` 草案节（按 `acceptance[]` 逐条给变异四字段），planner 收 builder 报告后第一步投影、再跑 verify——把「投影时点」写成明义务，禁止手抄验收全文（与 `docs/debt/gov/ARQ-261028-mutation-evidence-authoring-pointer.md` 的全文匹配口径同批处置）。
2. 主线程 commit 前置检查：工作树含已知变异标记（或 builder 报告声明处于变异轮）时 fail-loud；至少写进 planner 的提交纪律。可评估做成 `verify.mjs` / 轻量脚本的 commit 前检查。
3. `mutationEvidence` 对 `MASKED` 条目强制带「复测后终态」字段（`retested` / `finalSensitive` / `retestRound`），缺则 `mutation-check` 红。

**风险**：riskTier: **low**——第 1、3 条是报告模板与证据 schema（fail-closed 方向、不动判定语义）；第 2 条为新增前置检查，需判官核「变异标记识别不误伤正常改动」。review profile = Track B（跨 agent 定义 + 脚本）。