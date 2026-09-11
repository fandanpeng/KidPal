# 260911-display-interaction-sovereignty · progress

> 执行日志（追加式）。

## 2026-09-11 · portfolio-planner 拆包（PKG-1 / PKG-2 / PKG-4）

- 输入：PROPOSAL.md FROZEN v1.0（pin sha1:cd10d1903ac6bb1a / git blob 26138dba，commit 8a06d0c）
  + review.md（F1–F9）+ ARCH-REVIEW-QUEUE 两条 ARQ（cross-package-behavior-owner-gap /
  split-package-implementation-prose-gap，均首用本批）。
- 拆包结果：三包落盘 `.claude/taskpackages/`——
  - `260911-key-route-a1`（PKG-1，mergeOrder 18，dependsOn []）
  - `260911-screen-unification`（PKG-2，mergeOrder 19，dependsOn []，包级 plan.md）
  - `260911-display-first-m1`（PKG-4，mergeOrder 20，dependsOn [screen-unification, ecs-deploy-convergence]，包级 plan.md）
  - PKG-3（回答驻留）随 OQ3=c 撤销不立项。
- 跨包共享冻结面扫描（launch 前预警义务）：未发现常量互斥型冲突；行为碰撞型两处已
  申报（PKG-4 必触 test_g_relay_fix.acceptance.py；PKG-2 渲染类冻结齿路径敏感）——
  均走 acceptance-author 留痕修订通道，不带冲突进 launch。
- KEY owner 登记：`crossPackageBehaviors` 落 decision-coverage.json + 各包 taskpackage；
  KEY(GPIO18) 端到端 owner = 260911-key-route-a1。
- PKG-2 plan.md 落点：`.claude/taskpackages/260911-screen-unification/plan.md`
  （PKG-4 同形：`.claude/taskpackages/260911-display-first-m1/plan.md`）。
- board 初始化：三包 `--set-pod --status queued --merge-order 18/19/20`；
  批次③ 260911-render-pages 占位 mergeOrder 17 → 落锁 21（②先③后）。
- 待办（交 launch 前）：F-03 commit-before-launch（拆包产物 commit 专用 base ref 非 main）
  → launch 侧 `git worktree add`（自 base ref）+ 起 planner 会话（worktree 内跑 claude，
  launch 侧预填 --worktree-path）。
