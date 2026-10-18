---
id: ARQ-261018-enforce-scope-worktree-anchor-offbyone
title: enforce-scope 钩子跨树锚定 off-by-one——worktree 的 .git 指向 gitdir 目录（三级深），代码按 gitdir 文件假定用 dirname^3，多剥一层致 commonDir 落到仓根、samePath 恒不等、静默降级 legacy → worktree 内子 agent 写同仓合法路径被误拦
domain: gov
status: deferred
severity: SHOULD
owner: architect（G3 强制执行层）
trigger: 下一次动 `.claude/hooks/**` 的治理批；或再有子 agent 在 worktree 里被误拦时
opened: 2026-10-18
closed: ""
closedBy: ""
paths: [".claude/hooks/enforce-scope.mjs"]
source: 2026-10-18 architect 子会话实测（写 docs/architecture.md 被拦，报 `../KidPal/docs/architecture.md` 超出边界）+ 主线程核代码 enforce-scope.mjs:131-152 / :164-165 + `cat C:/AI/KidPal-pod-m2/.git` 实测 gitdir 指向目录
---

**证据（代码锚 + 地面真值）**：worktree 的 `.git` 文件内容实测为 `gitdir: C:/AI/KidPal/.git/worktrees/KidPal-pod-m2`——指向的是**目录**（该目录下才有 HEAD / COMMIT_EDITMSG / gitdir 等），不是文件。`locateGitTree`（enforce-scope.mjs:141-145）注释假定 gitdir 是 `<commonDir>/worktrees/<name>/gitdir` 这个**文件**，于是用 `dirname(dirname(dirname(gitdir)))` 回推 commonDir：

- worktree 分支算出 commonDir = `C:/AI/KidPal`（仓根，多剥了一层）
- 主 checkout 分支（:138-139，`.git` 是目录）算出 commonDir = `C:/AI/KidPal/.git`

两者 `samePath`（:154-155）永不相等 → `:164` 判定「非本仓路径」→ 静默降级 `mode: 'legacy'` → baseDir 取 pod 树根 → 相对路径被算成 `../KidPal/docs/architecture.md` → 判定越界拦截。钩子 self-check 夹具同样按「gitdir 是文件」构造，因此这个拓扑从未被测到。

**影响面**：所有 worktree 内的子 agent（architect / integrator / builder 等）用 Edit/Write 写「跨树但同仓」的合法路径都会被误拦；本次实际发生一次（architect 落 `docs/architecture.md` 的两处版本指针），绕行方式是改用 Python 直接写——**等于绕过钩子本身**，这是比误拦更坏的后果（护栏失效且无人察觉）。只影响 Edit/Write 通道，Bash 写另有红线不受此影响。

**建议改动（一行级 + 夹具修正）**：worktree 分支把 `dirname^3` 改 `dirname^2`（gitdir 目录 → `<commonDir>/worktrees` → `<commonDir>`）；或两分支统一到同一层级再 samePath。同时修 self-check 夹具，使其 `.git` 指向 gitdir **目录**以复现真实拓扑（否则修完仍无齿）。属 G3 文件，须 human-approved 治理批。

**风险**：riskTier: low——一行边界修正 + 夹具补齿；改错只会让跨树锚定继续降级 legacy，不引入新的越权写面。
