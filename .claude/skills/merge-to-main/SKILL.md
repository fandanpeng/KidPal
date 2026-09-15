---
name: merge-to-main
description: "Use when: 把 feature/gov 分支合并到目标分支（main 或同步反向）的本地合并操作——预检 overlap、ff-only 合并、查残余冲突标记、确认落点。触发词: 合并分支, merge 到 main, 同步 main, sync dev, 合分支, merge-to-main。边界: 只做本地合并与验证；不 push、不发 PR、不直推 main/master（对齐 deny-main-push.mjs 与「本地 main 合并由 planner/主线程前置升级闸放行后自动触发、remote push 仍 human」铁律；OQ-3 已裁 (a)）。"
---

# Merge to Main：本地合并预检与执行

把源分支合并进目标分支（典型：`gov` → `main`，或 `main` → 工作分支回同步）。固化自 `/insights` 报告（2026-08-02）16 轮合并马拉松的成功模式：成功的合并都做了 pre-merge diff 预览，失败的都是跳过预检的。

## 前置

- 工作树干净（`git status` 无未提交改动；有则先 commit/stash，别带着脏树合并）。**例外（2026-09-09 human 一次性授权直落）**：`.codebase-memory/artifact.json` 与 `graph.db.zst` 常驻脏 = 常态（indexer 自刷新的机器再生面，约定不追交、别硬清），不计入本条清洁度判据；合并冲突已由 `.codebase-memory/.gitattributes` 的 `merge=ours` 免除——看到它们脏不要 stage、不要 stash、不要 checkout 还原。**唯一例外动作**：反向回同步（main → gov/工作分支）时，merge 需要把这两件换入 main 上的新快照，而脏 live 文件会让 merge abort（实测）——先把这两件 `git checkout HEAD -- .codebase-memory/artifact.json .codebase-memory/graph.db.zst` 还原到 HEAD（丢弃的只是 indexer 可再生漂移，零损失），再执行 merge。
- 合并方向与授权明确：本 skill 只执行**已被批准的**本地合并（supervisor APPROVE / architect GREEN 或 human 批准后的本地合并动作面）；未批准的合并不做。

## 步骤（每步有可检查终点）

1. **查本地分支** — `git branch -a`。终点：确认源/目标分支都在、拼写无误。本地已有分支就**不做无谓 fetch**（报告实证：无谓网络 fetch 是高频摩擦）。
2. **预览 overlap** — `git diff --stat <target>...<source>`。终点：列出改动文件；**标记双侧都改的文件**（本仓热点如 `docs/ARCH-REVIEW-QUEUE.md`、`delivery/changes/INDEX.md`）——这些是冲突高发点，先看清再动。
3. **必要时 pathspec 隔离** — 共享 worktree 场景有 CRLF 噪音时，用 `git add -- <specific-files>` 只暂存意图内的文件。终点：暂存集 == 本次合并意图，无噪音文件。
4. **ff-only 合并** — `git merge --ff-only <source>`（在目标分支上）。终点：合并成功输出 `Fast-forward`。**非 ff 立即停下**：`git merge --abort`（如已开始），列出分歧 commit 交 human 裁——不自行制造 merge commit。
5. **查残余冲突标记** — `git diff --check`（对合并结果区间）。终点：无 conflict marker 残留。有残留 = 事故（报告实证：静默残留曾进过 commit），停下交 human。
6. **确认落点** — `git log --oneline -5`。终点：源分支的 commit 都在目标分支上、顺序符合预期。

## 约束（must / never，不可省）

- **永不 push**、永不直推 main/master（`deny-main-push.mjs` 钩着这条铁律；入本地 main 由 planner/主线程经前置升级闸放行后自动合并，remote push 仍由 human 经 PR）。
- 非 ff 分歧不自裁——列出分歧交 human。
- 合并后发现合错：用 revertable 路径回退（`git revert` 或 human 裁），不 `reset --hard` 丢证据。

## 失败信号 → 处置

- `git merge --ff-only` 报 `Not possible to fast-forward` → 目标分支有独有 commit；`git log --oneline <source>..<target>` 看是什么，交 human 定（rebase / 本地 `--no-ff` 合并 / 走 remote PR）。
- `git diff --check` 报 conflict marker → 停，定位残留文件交 human；**不要**直接 commit。
- `git merge` 报 `Your local changes to the following files would be overwritten by merge` 且命中 `.codebase-memory/` 两件 → 对这两件 `git checkout HEAD --` 还原后重试（indexer 可再生漂移，零损失）；不要 stash、不要把它们 commit 进分支。
- 合并后 `git status` 有意料外文件 → 多为 CRLF/换行噪音，pathspec 重暂存（步骤 3）。
