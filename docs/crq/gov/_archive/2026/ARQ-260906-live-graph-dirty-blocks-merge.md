---
id: ARQ-260906-live-graph-dirty-blocks-merge
title: CodebaseMemory 后台索引器持续重写 worktree 内被跟踪的 `.codebase-memory/graph.db.zst`，git 树常驻脏，merge-driver 的 rebase/merge 因 unstaged changes 被 fail-closed
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-06
closed: 2026-09-06
closedBy: ""
paths: []
source: ""
---

merge-driver 新增 `ensureSkipWorktree`：rebase 步（Pod worktree）与 merge 步（main 持有者 worktree）两个调用点自动幂等 skip-worktree `.codebase-memory/graph.db.zst` + `artifact.json`，置于 `if (!DRY_RUN)` 守卫之外，成功路径 stderr 一行留痕、失败 advisory-only 永不阻塞合并；dry-run 实证自动重设 + 幂等（[260906-gov-review-arq-governance-batch](../changes/arch/260906-gov-review-arq-governance-batch/)，T6）。
