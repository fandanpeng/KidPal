# landing summary · 260907-gov-review-merge-friction

> **性质**：Track B 常驻轻路径治理批（G2+G3）· packet-bound landing
> **packet**：FROZEN PROPOSAL（[PROPOSAL.md](PROPOSAL.md)，R2=GREEN）+ landing-manifest 三路径 + 本 summary
> **分支**：`dev/260907-gov-review-merge-friction`（自 main 切出）

## 落地清单（≡ PROPOSAL §2 改法集 T1–T6）

| T | 目标文件 | 落了什么 |
|---|---|---|
| T1 | `.claude/scripts/merge-driver.mjs` | 人批豁免消解：main() ready 收集前对「单 Pod + `--human-approval-ref` + blocked」形状调 `approvalRefUnlock()`——以同 Step 1b 透传路径 spawn 前置升级闸核验 ref（gate 脚本顶层直跑故不可 import，spawn 即字面复用）；通过 → setPod(ready-to-merge) + reconcile + 刷新内存 board 视图（R1-F1：freshBoard 快照不回写）+ stderr 留痕；失败 fail-closed 保持 blocked 并报因；无 ref 的 blocked Pod 行为零变化。R1-F1/F-4 修正均落 |
| T2 | `.claude/scripts/merge-driver.mjs` | Step 0 治理面自动收口（Step 1c 后、rebase 前）：`git status --porcelain -uall` 检测，全部落在 `.claude/taskpackages/<pod-id>/**` + `changes/dev/<pod-id>/**` 白名单（目录段精确等值判定，R1-F6 防畸形 `--pod` 逃逸；current/ 显式拒收）→ add 精确路径 + pathspec 限定 chore commit；白名单外 warn 留痕保持 fail 行为不吞 |
| T3 | `.claude/scripts/merge-driver.mjs` | 投影冲突 --theirs 特判：rebase catch 先走 `tryResolveProjectionConflicts()`——冲突文件集 ⊆ {current/taskpackage.json, current/verdict.json} 时 `checkout --theirs`（rebase 语境 = 被合并包赢）+ add + `GIT_EDITOR=true git rebase --continue` 循环至完成（上限 16 轮）；含任一非特判文件 / 未进冲突态 / 超轮次 → 返回 false 走原有整体 abort + blocked（R1-F4 特判不部分生效） |
| T4 | `.gitattributes` | 追加 `.planning/kidpal-mvp/progress.md merge=union` + 4 行注释（与 `*.sh eol=lf` 条目及 `.codebase-memory/.gitattributes` 无路径交集） |
| T5 | `.claude/references/pod-coordination.md` | 新增 §10（10a merge 冲突消解 runbook 五步合法消解序 + 10b post-merge 簿记落点口径：Step 5b 机械落 main / INDEX 留下批全量重建 / Step 0 白名单收口），不动既有节 |
| T6 | `.claude/scripts/merge-driver.mjs` | Step 5b（具名函数 `step5bBookkeeping`，主流程与自检共用实现）：merge 成功后在 mergeCwd（持有 main 的 worktree）向 `<pod-id>/pod-events.jsonl` append `merge-completed` 事件行（schemaVersion 1 / actor=merge-driver / payload 摘 main tip + approval-ref）+ pathspec 限定单文件 commit；失败一律 advisory warn 绝不 errorExit/未捕获抛出（R1-F3 承重墙 + 防夹带 staged graph 残留）；DRY_RUN 跳过 |

## 验证记录（≡ PROPOSAL §5）

1. **既有五自检复跑零回退**：retro-advisory 3/3、quick-checks ok:true（expectedCmds 全等）、detect-changes 16/16、rebase-target 7/7、worktree-topology 9/9 —— 全 `ok:true`。
2. **新增自检 `--self-check-merge-friction`（临时仓夹具，脚本自建跑完即删，对齐 worktree-topology 先例）10/10**：T1a 有效 ref 解锁+board 刷新+pod 文件转 ready / T1b 无效 ref fail-closed / T1c 非 blocked 不适用 / T2a 白名单自动收口 status 净 / T2b 白名单外拒绝不吞 / T2c 畸形 changeId 拒收 / T3a 投影冲突 --theirs 消解 rebase 完成 / T3b 混合集整体拒绝 / T4 union 双侧保留 / T6 簿记落行 + pathspec 不夹带 staged 残留。
3. **T5 G2 面惯例**：`route-regression-check.mjs` 28 ok 0 fail；`lint-descriptions.mjs` 1 条 warn 为 landing-verifier.md 描述超长（先前批次遗留，非本批触面）。
4. **landing 后**：`landing-fidelity-check.mjs` + landing-verifier 独立核验（另行产出 landing-verdict.md）；最终权重在下一个真实 Pod merge 段实测「一次跑成」。

## 实现注记（判官 GREEN 后未改语义，仅落地细节如实留痕）

- T1 复用形态：PROPOSAL 冻结文「同 Step 1b 透传路径」——落地为 spawn gate（同参数形态），gate 的 `resolveHumanApproval` 五要素本体不重复实现也未 import（该脚本顶层直跑 + process.exit，import 即带跑闸逻辑，spawn 是唯一安全复用形态）。
- 自检新旗标 `--self-check-merge-friction` 注册在 merge-driver 自检分派区（第六自检，与既有五自检同构）。
- Step 5b 提取为具名函数并注明「主流程与自检共用同一实现，防假绿」（对齐 worktree-topology 自检先例）。
