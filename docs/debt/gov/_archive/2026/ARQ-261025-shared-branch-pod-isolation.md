---
id: ARQ-261025-shared-branch-pod-isolation
title: 同分支 / 同 worktree 多活跃 Pod——包级 diff 的 base 与合并圈定无机械锚，靠 cherry-pick 与手工对齐
domain: gov
status: closed
severity: SHOULD
owner: architect（launch 面口径）+ dev-loop（merge-driver 圈定）
trigger: 下一治理批（动 `pod-coordination.md` / `merge-driver.mjs` / launch 面时并批）；或下一次同分支双活跃 Pod
opened: 2026-10-25
closed: 2026-10-27
closedBy: 261026-gov-review-p4-pod-board-cluster
paths: [".claude/scripts/merge-driver.mjs", ".claude/references/pod-coordination.md", ".claude/agents/planner.md"]
source: [delivery/retro/2026-10-25-loop-retro.md, delivery/taskpackages/261018-sd-log-persist/pod-retro.md, delivery/taskpackages/261018-sd-log-usb-read/pod-retro.md, delivery/taskpackages/261022-p3-record-front-polish-fix/pod-retro.md, delivery/taskpackages/261023-honcho-memory-wiring-restoration/pod-retro.md]
---

**证据**：① `261018-sd-log-persist`：persist 与 usb-read 两包共用 `dev/pod-m2`，merge-driver 是整分支语义；planner 只能临时分支 cherry-pick 本包提交去过闸链，原分支留给另一包 rebase（retro 原话「要么 launch 面禁止同分支双活跃 Pod，要么 merge-driver 增加按提交范围圈定的能力」）。② `261018-sd-log-usb-read`：persist 收口会话在共享分支落 15 个提交并翻走 `current/` 指针；`git rebase` 在 settings 的 ask 列，headless 会话弹窗未达 human 即自动拒绝，「planner 手搓 rebase → 闸 → merge-driver」序列需要 human 批一次。③ `261022-p3-record-front-polish-fix`：`detect_changes` 先跑 base=main，圈进两个并联固件包的全部符号；切到包首提交父 `80bc2027` 重跑才对。retro 原话「共享分支 diff 和包内 diff 是两张图，圈定永远用后者」。④ `261023-honcho-memory-wiring-restoration`：并行 `/arch-change` 会话在同 worktree 留下 PROPOSAL 与三件 spec 改动，靠 integrator stat 核对识别、零混提。

**影响面**：凡复用 worktree / 分支的批次。包级 diff 取错 base 会把别人的影响面背到自己包上（终验圈定、白盒复核、合并范围三处同时受污染）；merge-driver 整分支语义使「先完成的包」无法独立合并；headless 下 rebase ask 还会在最后一步阻断。本波 4 包命中。

**建议改动**（交 review、不自批）：二选一或并取——① launch 面口径：禁止同一分支同时存在两个活跃 Pod（`pod-coordination.md` §2c 换包协议补一条），或要求每包显式记录自己的 base ref 并在 launch-card 钉死；② merge-driver 支持按提交范围圈定（`--pod` 已有，可评估扩展为 `<base>..<head>` 显式范围），使同分支包可独立过闸链。另建议把「包级 diff 的 base = 包首提交父，不用 main」写成 planner/integrator 的成文动作。

**风险**：riskTier: low——方案①为文面与 launch 检查项；方案②触 merge-driver 执行面但只加可选参数，不改现有默认路径与门判据。

**2026-10-27 · 结项（`261026-gov-review-p4-pod-board-cluster`，部分结项）**：已落 = 方案①（human 立项 ack③ 裁定）——`pod-coordination.md` §2c 增「同一 worktree / 同一常驻分支同一时刻至多一个活跃 Pod」互斥租用禁令（launch 面口径 + launch 检查项，不设锁，与「记录不是锁」既有裁定相容）+「每包 base ref（= 包首提交父）钉死进 launch-card」成文动作（SKILL 启动卡增第七要素：点火时 `git rev-parse HEAD` 的 SHA + 常驻分支名，包级 diff 恒用该 ref 不用 main）。**未闭合面 = 方案②（merge-driver 显式 `<base>..<head>` 范围参数）与 worktree 池化**——human 2026-10-26 已裁另登记独立专项批（workstream gov-debt-reduction 立项 ack③ 在案；PROPOSAL §6 留根 7 互指：池化批若采用不同 base ref 形态，以池化批 SSOT 为准并回收本条）。
