# Pod Coordination — blackboard / merge token / reconcile（canonical mechanics）

> 状态：canonical dev-loop 协调机制参考。落定自 `changes/arch/dev-loop-os-rte/PROPOSAL.md`（FROZEN v2.1）§7–§9。
> 上层定义见 [`dev-loop-operating-system.md`](dev-loop-operating-system.md)；本文件只讲多 Pod 并行的协调**机制**。角色见 [`.claude/agents/portfolio-planner.md`](../agents/portfolio-planner.md)。

## 1. 原则

portfolio-planner **不是**常驻盯文件的 session；agent 之间**不**互相发消息（平台无此原语）。协调用 **blackboard = 共享 repo 文件**，agent episodic 读写，靠状态而非活会话。

## 2. 文件布局（协调态在 git common dir，不入版本控制）

**问题**：每个 Pod 在独立 `git worktree`，被 git 跟踪的文件是**各 worktree 独立副本**，互相不可见。故协调态**不能**放被跟踪路径（否则各 worktree 看不到彼此，reconcile 无法跨 Pod 聚合）。

**运行时协调态**放所有 linked worktree 共享的 git common dir，不入版本控制：

```text
$(git rev-parse --git-common-dir)/devloop/board.json            # 聚合视图 + merge token（合并进 board，不单列 merge-lock）
$(git rev-parse --git-common-dir)/devloop/pods/<change-id>.json # 各 Pod 生命周期状态（该 Pod 单写）
$(git rev-parse --git-common-dir)/devloop/reconcile.lock        # advisory lock（mkdir，见 §4）
```

`git rev-parse --git-common-dir` 在同机所有 linked worktree 解析到**同一目录**（主仓 `.git/`），跨 worktree 共享且不会被误 commit。

> 实现备注：主 worktree 下该命令返回**相对** `.git`，linked worktree 返回**绝对**路径；脚本必须对结果做 `path.resolve(cwd, commonDir)`（或用 `--path-format=absolute`），否则主 worktree 在非 repo-root cwd 下解析错。

**耐久证据**仍入版本控制：post-cutoff 为 pod-events / pod-retro / taskpackage / review 制品；pre-cutoff 继续兼容 session-digest / loop-health / pod-journal。它们落 `changes/dev/<change-id>/` 与 `.claude/taskpackages/<change-id>/`，随 Pod worktree 合并进 main。

> 跨设备不靠 live board：本机多 worktree 用共享 git common dir；跨设备协作 post-cutoff 走 handoff + pod-retro（叙事连续性）+ pod-events（结构化证据），pre-cutoff 仍可用 session-digest；不共享运行时 board。

## 2b. Launch 前置门（commit-before-launch 校验）

portfolio-planner 交 launch 前（见 [`portfolio-planner.md`](../agents/portfolio-planner.md) Step 6 commit-before-launch 不变量），launch 序列加**前置门**：launch 前对每个 ready 包跑 `git cat-file -e <base-ref>:<path>` 校验**基准 ref**（专用 base ref，非 main）已含 check 集：

- **check 集 + 分层（defender 倾向 / human A-3 裁）**：
  - `taskpackage.json`（`.claude/taskpackages/<changeId>/taskpackage.json`）= **blocking**
  - PROPOSAL（`origin.upstreamRef` 解析）= **blocking**
  - `decision-coverage.json`（`.planning/<ws>/decision-coverage.json`）= **warn**
- `origin.upstreamRef` 校验前按 task-intake DP-1 **剥片段**（`:Sx-Ay` 等锚点，复用不新造）；PROPOSAL 路径**双路解析** `.planning/<ws>` ∨ `changes/(arch|dev)/**`（复用 `isProposalMdPath`，change-triage.md:106）。
- **board 不入 check 集**：board/token 恒在 git common dir（§2）、不入版本控制，其可见性由 common dir 给、不靠 commit；board 失同步由 planner intake-reconcile 修。
- 缺失（blocking 项）→ **拒绝 launch**，报「拆包产物未 commit 到基准 ref」，回 portfolio-planner 补 commit（对偶 planner.md F-09 needs-triage 默认出口）。

> 机械承载（2026-08-25-gov-review-launch-reachability-guard T1 落地）：`node .claude/scripts/portfolio-reconcile.mjs --check-launch <changeId> --base-ref <ref>`——launch 执行者在 `git worktree add` 前跑，blocking 缺即 exit 1 拒绝（对偶本节 `git cat-file -e` 语义）。

> **门归属（非第 7 工程门）**：本前置门是 launch 编排的**结构性前置条件**——判者 = `git cat-file -e` 确定性退出码机械检查（制度族，对偶 `taskpackage-required-check.mjs` 起包结构闸），非 human judgment 门。DEV-GOVERNANCE §5「工程门 = 6 行」折进注（G1，human-only）明文其为「launch 编排结构性前置条件，非第 7 工程门」；本文件只引其相位，不落折进注。

## 3. Pod 状态机

```text
queued -> running -> verifier-green -> ready-to-merge -> merging -> merged
                                        \-> blocked / abandoned
```

它是持久标记，不是活内存；board 只是聚合视图。

## 4. 单写者 + 原子写 + advisory lock

- **Pod 只写自己**：hook 触发时 Pod 只写自己的 `devloop/pods/<change-id>.json`（本 Pod 单写，天然无冲突）。**hook 绝不写 board**。
- **board/token 单一逻辑写者 + 串行化**：聚合 board、轮转 merge token 只由 `portfolio-reconcile.mjs` 做，用 **advisory lock 串行化**——起手 `mkdir` 抢 `devloop/reconcile.lock`（原子、`EEXIST` 即失败），抢不到就幂等 `exit 0`（下次写会再触发）；写 board 用 **temp 文件 + `rename`** 原子替换；结束删锁。多个 Pod 的 hook 并发触发时只有一个真正跑，其余无害退出，无竞态。
- **reconcile 幂等**：每次从全量 `devloop/pods/*.json` 重算 board，不做增量——漏跑/重跑都安全。
- **lock stale 检测**：`mkdir` 锁不会在持锁进程崩溃后自动释放（reconcile 死在 `rmdir` 前 → 锁永久残留 → 后续全 `exit 0` = 死锁）。故 `reconcile.lock` 目录带 mtime：其龄超过可配阈值（默认 60s，远大于一次 reconcile 时长）时可被后续 reconcile 强制 `rmdir` 再重建。与 §8 token TTL 同构，均靠持久态 + mtime，不靠进程存活。

## 5. 通讯 = 写文件

Pod 报「我 ready」= 写自己的 `devloop/pods/<change-id>.json`；reconcile（hook 触发或 human/portfolio-planner 手动）聚合所有 pod 状态、更新 board、轮转 merge token。不是活会话对话、不是 agent 间消息。

## 6. 合并串行化

- supervisor 是 **per-Pod**：每个 Pod 到合并门各跑一次；不是某 Pod 独占、也不是全局合并器。
- **Pod 内交付闭环**：每个 Pod 在自己的 worktree 内跑完 acceptance→builder→verifier↔fixer→integrator→**supervisor**。supervisor APPROVE 后才标 `ready-to-merge`。
- **合并门被 merge-driver 串行化**：N 个 Pod 并行开发、各自闭环；planner 在 supervisor APPROVE + 前置升级闸（`pre-merge-escalation-gate.mjs`）放行后自动触发 `node .claude/scripts/merge-driver.mjs --pod <changeId>`，按 mergeOrder 逐个对 ready-to-merge Pod 执行 rebase→quick-check→merge。
- 顺序由 portfolio-planner 定（依赖序、触共享文件/契约面者优先、冲突面最小化）；它只**排序 + 初始化 board**，不替 supervisor 判过/不过。
- **正确性关键**：merge-driver 在 merge 前 **rebase 到解析出的最新合并基线**（本地 main 优先，origin/main 领先时切 origin/main——含已落地前序 Pod），并跑 quick-check（firmware-build + contract-lint + acceptance）抓 rebase 后集成问题。supervisor 已在 Pod 内复核过 closeout 制品；quick-check 是 rebase 后的补充验证。
- 跨 Pod 冲突在 **merge-driver 的 rebase+quick-check** 处暴露解决。rebase 冲突 → blocked（人工介入）；quick-check 失败 → 重试一次，仍失败 → blocked。
- gate 权在 supervisor（APPROVE/BLOCK/ESCALATE），落地走 merge-driver 的本地 --no-ff 可回滚路径；portfolio-planner 不 merge、不越 gate。

> 一句话：**并行开发、各自闭环，串行合并；portfolio-planner 定序并初始化 board，planner 跑到 supervisor APPROVE 标 ready，merge-driver 逐个 rebase+quick-check+merge，谁都不能并行直落 main。**

## 7. merge token 消费协议

planner（Pod 内单会话）**不轮询、不等待**：

1. planner 在自己的 worktree 内跑完 acceptance→builder→verifier↔fixer→integrator→**supervisor**。supervisor APPROVE → 写 `pods/<change-id>.json` 状态 `ready-to-merge` → **退出**（不自己进合并门、不自旋）。
2. merge token 的**消费者是 merge-driver（`node .claude/scripts/merge-driver.mjs`）**，由 planner 自动触发（单 Pod `--pod`，经前置升级闸放行）。merge-driver 启动时**无条件 reconcile** 获取最新 board：
   - 从 `ready-to-merge` 的 Pod 里按 merge order 选第一个 → 该 Pod 持有 token。
   - 对当前 holder Pod：读 `pods/<change-id>.json` 的 `worktreePath` 定位 worktree → rebase → quick-check → merge → 标 `merged`。
3. 该 Pod merged → reconcile 清空 `mergeToken.holder` → 下一轮选下一个（直至无 ready Pod）。

> token 是「谁现在可以进合并门」的持久标记，被 merge-driver 读取；planner 只标 ready 就走人，避免任何自旋/常驻。

## 8. merge token 生命周期状态机

`mergeToken = { holder: <change-id>|null, grantedAt: <ts> }`。转移：

| 事件 | token 动作 |
|---|---|
| reconcile 选中一个 ready Pod | `holder=<id>`, `grantedAt=now` |
| supervisor **APPROVE** → merged | `holder=null`（释放，选下一个） |
| supervisor **BLOCK** | **立即释放 `holder=null`**；Pod 回 fixer，`ready-to-merge`→`blocked`，修好再 `ready-to-merge` 重新排队。**token 不在 fixer 循环期间被占用**（防自锁）。 |
| supervisor **ESCALATE** | `holder=null` 释放；Pod 挂 `blocked`（等人类），不占 token。人批后重占 token 走 merge-driver 人批路径（`--human-approval-ref`，见 planner.md 合并门段）。 |
| holder Pod **崩溃/abandon**（`grantedAt` 超 TTL 且 `pods/<id>.json` 未推进 / mtime 陈旧） | reconcile **回收** `holder=null` + 标 `abandoned`，升一条告警。 |

> 防饿死：被 BLOCK 退回 fixer 的 Pod 修好重新 `ready-to-merge` 时**保持原 merge-order 位次**（不排队尾），避免新 Pod 持续插队导致旧 Pod 饿死。TTL 须 > 正常 rebase→integrator→supervisor 合并门时长（避免误判回收）；因 ESCALATE 已立即释放 token，不存在「持 token 等人类慢审」的长窗口。

## 9. Watcher / 触发器（便宜档，不建 daemon）

- **Decomposition（拆包）**：需判断 → agent/human，episodic 手动，无法也不该 hook 化。
- **Scheduling / merge-token 轮转**：确定性规则（依赖序、共享面优先、WIP limit、rebase 校验）→ 确定性脚本 `portfolio-reconcile.mjs`，可被 hook 触发。

v1 决定（加便宜档）：

- 建**确定性 `portfolio-reconcile.mjs` + in-session PostToolUse hook**（`portfolio-sync.mjs`，照抄 `planning-sync.mjs`：写 `devloop/pods/<change-id>.json` 后触发 reconcile，非阻塞 `exit 0`，内部抢 §4 advisory lock）；
- **不建**常驻 daemon/cron；
- 判断类 decomposition 仍手动；hook 只自动化确定性的 scheduling/token 轮转；
- 触发器可换（manual→hook→cron→CI 同一个 reconcile），并发量上来再升级，零架构成本。

**毕业条件（量化，任一满足即评估升级）**：并行 Pod 稳定 ≥3、或 merge 频率 ≥2/day、或出现 ≥1 次「会话结束后无人 reconcile 导致 Pod 卡 >TTL」的实测事件。升级只换触发器（同一个 `portfolio-reconcile.mjs`），不改数据结构。owner=architect（随 loop-health 复盘评估）。

## 相关

- [`dev-loop-operating-system.md`](dev-loop-operating-system.md)
- [`.claude/scripts/portfolio-reconcile.mjs`](../scripts/portfolio-reconcile.mjs)
- [`.claude/hooks/portfolio-sync.mjs`](../hooks/portfolio-sync.mjs)
- [`.claude/agents/portfolio-planner.md`](../agents/portfolio-planner.md)
