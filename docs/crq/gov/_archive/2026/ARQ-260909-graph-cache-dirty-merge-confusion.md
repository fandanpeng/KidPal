---
id: ARQ-260909-graph-cache-dirty-merge-confusion
title: 弱模型在 merge/commit 现场对 .codebase-memory 两件常驻脏文件纠结（stage/stash/还原不定）——「脏着别硬清」约定只活在散文与会话记忆，动作发生点无机器可读答案
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

human 一次性授权直落（明示跳过 Track B，在册）：commit-safety.mjs 加 `graphCacheNotice` 白名单提示（git add/commit 双触发点，命令或 staged 命中即给处置答案；自检 12/12）+ merge-to-main skill 前置节例外句（两件脏 = 常态、不计入清洁度判据、禁 stage/stash/还原）+ AGENTS.md「CodebaseMemory 使用纪律」节成文（含 human 裁定：主/gov 两长期 worktree 不设 skip-worktree——主 worktree 是快照刷 main 的源头，skip 会让快照静默陈化；合并冲突本已由 merge=ours 免除）。**注**：「主/gov 均不设 skip-worktree」句与主 worktree 实况不符（merge-driver 幂等维持 S），快照流向裁定亦反转——见下条 ARQ-260909-graph-cache-snapshot-direction，以该条为准（append-only 不改本行）。
