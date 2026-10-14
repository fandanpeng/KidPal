---
id: ARQ-260909-graph-cache-snapshot-direction
title: 快照流向前提失真：上条成文称「主/gov 均不设 skip-worktree」，实测主 worktree 早由 merge-driver 对 main 持有 worktree 幂等维持 S（ensureGraphCacheSynced mergeCwd 路）；且快照捕获方向应为 main→各 worktree，而非 worktree→main
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-09
closed: 2026-09-09
closedBy: ""
paths: []
source: ""
---

human 一次性授权直落（明示跳过 Track B，在册）：c:/tmp 沙盒实测四态定案——① merge=ours 单边变更干净拿新版、不冲突不覆盖；② 脏 live 文件让 merge abort 且单设 skip-worktree 不豁免（有效动作 = 还原至干净）；③ 还原后回同步 merge 通过、gov 拿到新快照；④ gov→main ff 不触这两件、脏树不拦、无回滚。落地：主 worktree 撤 S 恢复捕获（H 已验）；merge-driver 收窄 mergeCwd 路不再 ensureGraphCacheSynced（Pod worktree 路保留 = 分发既有实现；自检 merge-friction 20/20、worktree-topology 9/9）；merge-to-main skill 补回同步还原行 + would-be-overwritten 失败信号；AGENTS.md 快照流向成文（源 = main 主 worktree 普通跟踪态；gov/pod 分支禁私带快照 commit——merge=ours 保本方的方向性陷阱）。
