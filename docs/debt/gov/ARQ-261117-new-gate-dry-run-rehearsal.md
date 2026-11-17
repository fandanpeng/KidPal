---
id: ARQ-261117-new-gate-dry-run-rehearsal
title: 新闸 / 闸链改动上线前没有全链 dry-run 演练——第一个端到端走通该路径的包当小白鼠，付出三轮真机 + 四轮闸拉锯
domain: gov
status: deferred
severity: SHOULD
owner: architect（闸上线纪律文面）+ dev-loop（演练执行）
trigger: 下一个新立闸 / 闸链改动（含 merge-driver、deviceOutcome 校验、pre-merge 闸）的治理批
opened: 2026-11-17
closed: ""
closedBy: ""
paths: [".claude/references/gov-change.md", ".claude/references/dev-loop.md", ".claude/skills/merge-to-main/SKILL.md"]
source: ["delivery/taskpackages/261104-k0a-four-page-skeleton/pod-retro.md 踩坑 4", "delivery/retro/2026-11-17-loop-retro.md §二 T7"]
---

**证据**：261104-k0a 踩坑 4 逐字：「**闸链热修簇**（human 两次亲签）：merge-driver 传闸 `--source-ref` 字面量 'HEAD' 不归一化 + 闸 deviceOutcome sourceCommit 严格相等在『证据随合并提交』下自引用不可满足——261028 P8 设备轮硬门立起后首个端到端走通该路径的包把两处缺口全部踩出。**共性**：新闸立起后应有一次『闸链演练』（dry-run 走通全链）再依赖；首包当小白鼠的成本是三轮真机 + 四轮闸拉锯。」同包还记了 device-round-evidence 的 sourceCommit 在 rebase 重写下反复失效，最终以「线性化链上固件同源提交 + 祖先判据（binaryRef SHA-256 硬绑定内容）」收口。

**影响面**：每次新闸 / 闸链改动的第一批真实包。两个缺陷都是「只在证据随合并提交 + rebase 重写同时发生时才现形」的时序型缺陷——单闸自检与单点 fixture 测不出来，只有全链走一遍才会暴露。代价不是重跑一个闸，而是真机轮（human 持板）与闸拉锯交叉消耗。

**建议改动**（候选，交 review、不自批）：① 新闸 / 闸链改动合入前，做一次端到端 dry-run 演练：用夹具包（或本批一个真实小包）把「scoped → integrator 圈定 → final → supervisor → merge-driver → 设备轮证据校验」全链走通，允许在隔离 worktree / 分支上跑；② 演练记录（走了哪些路径、哪些参数形态、结果）作为闸批的落地制品之一；③ 对时序型判据（`--source-ref` 归一化、sourceCommit 与 rebase / 合并提交的关系）建立固定夹具形态，纳入闸自身 self-check，而不是等真实包撞。

**风险**：riskTier: **low**——纯纪律文面 + 演练记录义务，无门判据改动。若判官认为「每个闸批都全链演练」成本过高，可退一步只要求**触及 merge / device 路径的闸改动**演练（本包两个缺陷都属此面）。