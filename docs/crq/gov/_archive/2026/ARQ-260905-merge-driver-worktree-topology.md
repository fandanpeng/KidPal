---
id: ARQ-260905-merge-driver-worktree-topology
title: merge-driver 不适配本地多 worktree 拓扑：无 origin remote 时 fetch 步无条件 fail-closed + merge 步在本 worktree checkout main 被 git 拒
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-05
closed: 2026-09-06
closedBy: ""
paths: []
source: ""
---

热修（无 origin 跳 fetch + 定位 main 持有者 worktree 执行合并）已随 260829-session-logic 包 3 落地（commit fee5627/ccf5a12/fdbe8c2）；本批补 `--self-check-worktree-topology` 自检（临时 git 夹具 5 断言，含 hasOriginRemote / findMainHolderWorktree 提取具名、主流程与自检共用同一逻辑防假绿），既有四自检零回退（[260906-gov-review-arq-governance-batch](../changes/arch/260906-gov-review-arq-governance-batch/)，T5）。
