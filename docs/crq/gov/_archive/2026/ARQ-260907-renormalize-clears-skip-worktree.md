---
id: ARQ-260907-renormalize-clears-skip-worktree
title: git add --renormalize 清 skip-worktree 旗标 + 内容入 staging；且 skip-worktree 旗标完好仍被内容漂移挡 rebase（skip-worktree 不豁免内容比对）
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-07
closed: 2026-09-07
closedBy: ""
paths: []
source: ""
---

merge-driver ensureSkipWorktree 升级 ensureGraphCacheSynced 内容级三拍：no-skip-worktree → git checkout HEAD --（index+worktree 双还原）→ 复旗标；拓扑自检补两形态 4 断言（worktree 漂移 + staged 漂移净树），五自检全绿零回退；renormalize 操作序交互 + rebase 重复 pick 清 todo 指引头注释成文。260829-voice-roundtrip 合并三次 rebase 拒绝病灶根治。 证据：[260907-gov-review-arq-governance-batch-2](../changes/arch/260907-gov-review-arq-governance-batch-2/)。
