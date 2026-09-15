# INCIDENT · 主仓共享 git config 被零字节化（非本包动作，全仓 git 不可用）

> changeId: 260911-display-first-m1 · planner 记录 · 2026-09-12 凌晨 · 供 human 明早复核
> 性质：环境事故，非 dev-loop 决策点。按 deputy canon P-2.9（枚举外新情形→不裁而报）+ 系统纪律「未创建的面不擅自改写」，本 planner **不自行修复**，只记录 + 上报 + 给恢复建议。

## 1. 现状（机械证据）

- `C:/AI/KidPal/.git/config` = **160 字节全 `\x00`**（`xxd` 逐字节验证，无任何有效行）。git 对它的报错 = `fatal: bad config line 1`。
- mtime = **2026-09-12 02:46**（`find C:/AI/KidPal/.git -newermt "2026-09-12 02:00"` 仅它一个文件命中——孤立损坏，非 `.git` 被整体清空）。
- 影响半径 = **全部 worktree + 主 checkout**（`ls C:/AI/KidPal/.git/worktrees/` 共 19 个 worktree；无任何 `config.worktree` 文件——所有 worktree 共享这一份 `C:/AI/KidPal/.git/config`）。任何 `git` 子命令（含 `git config --global` 在内）都因读取它而 fatal。
- **非本 planner 动作**：本包最后一次落盘动作约 01:25（taskpackage finalize 编辑）；02:46 的零字节化与本会话时间线不符。时点与「会话 context 被 re-read / 模型切换 / worktree 检测抖动」的环境切换相重合，推测为 harness 重启/基础设施变更的副作用（未证实，仅时间关联）。

## 2. 完好面核验（无成果丢失）

- 本包全部工件已逐件核验在盘：`server/acceptance/test_g_relay_fix.acceptance.py`（REVISION LOG L185 + `DisplayFirstM1Acceptance` L1192，+239 行）、taskpackage.json finalize 编辑（`on_block_closed` ×3 / `perAcceptance` ×1）、`evidence/DF-E1-contract-recheck.md`（9495 字节）、`input-brief-*` ×2、`pod-events.jsonl`。
- git 数据面完好：`HEAD`/`ORIG_HEAD`/`packed-refs`/`index`（272600 字节，工作区文件表）/`objects`/`logs` 均正常；reflog 尾部到 `438f85a`（main 合并夜收口）完好，全部 commit 署名 `DavidFan <710168693@qq.com>`。
- 身份面完好：全局 `~/.gitconfig` 在（`[user] name=DavidFan email=710168693@qq.com`）——commit 身份**不依赖**被损坏的 repo-local config。
- 分支指针完好：本 worktree `HEAD → ref: refs/heads/dev/260911-display-first-m1`；主仓 `HEAD → ref: refs/heads/main`。

## 3. 恢复风险评估（供 human 拍板）

- **唯一真正丢失的信息 = `[remote "origin"] url`（若有）**。但证据强烈指向该 repo 为本地-only 或 origin 从未 fetch：
  - `refs/remotes/` 目录**不存在**（从未有过 remote-tracking 分支）；
  - `FETCH_HEAD` 内容 = `…branch 'planning/kidpal-mvp-base' of .`（`of .` = 本地路径）；
  - reflog 通篇只有 commit/merge，**零 fetch/pull 活动**。
  - → 恢复 config 不丢任何可工作信息（remote URL 即便曾存在也从未被 agent 消费；push 恒归 human，human 知道自己要 push 到哪）。
- `.gitattributes` 里的 merge 驱动 = `merge=ours`（.codebase-memory/graph.db.zst）+ `merge=union`（.planning/progress.md），**全部是 git 内建策略**，不需要 config 里定义自定义 merge driver。

## 4. 恢复建议（human 明早执行，1 分钟）

最小可工作 config（与 Windows 本地 clone 默认一致；我**未写**，仅给出建议内容）：

```
[core]
	repositoryformatversion = 0
	filemode = false
	bare = false
	logallrefupdates = true
	symlinks = false
	ignorecase = true
```

写完后 `git status` 应恢复正常；若 human 记得要 push 的远端，再 `git remote add origin <url>`（原 origin 已不可从 git 物证恢复，见 §3）。

## 5. 本包停等点（恢复后即可无缝续跑）

- 断点 = acceptance 已冻结 + TaskPackage 已 finalize（红基线实测 6 failed/14 passed、mutationEvidence 两条自证已登记、crossEnd 三对端已解析）、阶段 0 DF-E1 契约复核已 CONFIRM。
- 下一步 = 派 builder（实现 `on_block_closed` 闭合回调 API + connection.py 接线）。该步及其后的 verifier/integrator/supervisor/merge 全链均 git 依赖（FS-EVIDENCE-1 `git diff`、verifier evidence、integrator commit、merge-driver），**在 git 恢复前无法按纪律执行，故停等**。
- 硬件实测（DF-A3 时延）+ 合并（ESCALATE 人批）本就是明早 human 在场窗口的既定安排，不受本事故影响。