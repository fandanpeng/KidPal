# landing-verdict · 260907-gov-review-merge-friction

> **核验人**：landing-verifier（独立核验，只读 codebase，唯一可写产物 = 本文件）
> **核验对象**：landing commit `c9f743e`（分支 `dev/260907-gov-review-merge-friction`，基线 main）
> **核验依据**：frozen `PROPOSAL.md`（R2=GREEN，landing-manifest 三路径，T1–T6）+ 逐文件人读 diff + 机械门实跑
> **结论：RED（红）——不放行 merge，退主线程修 landing**

## 1. 结构门（机器）——RED

```
node .claude/scripts/landing-fidelity-check.mjs changes/arch/260907-gov-review-merge-friction main
→ {"ok":false,"gate":"landing-fidelity",
   "message":"architect-landing-summary.md is required（landing 声称 + packet-binding）"}
```

**根因**：证据目录里 landing summary 制品名为 `landing-summary.md`，而结构门硬编码要求 `architect-landing-summary.md`（`landing-fidelity-check.mjs:274`）。三重旁证一致：① frozen PROPOSAL Status 行 packet 定义明写「packet = FROZEN PROPOSAL + landing-manifest 三路径 + **architect-landing-summary**」；② 此前全部七个有 landing 的批次（260904 / 260905 / 260906×3 / 260907×2）制品名均为 `architect-landing-summary.md`；③ 门脚本 claimed-but-not-landed / planned-but-not-landed / packet-binding 三项机检因该文件缺席全部未跑。

**修法**：主线程将 `landing-summary.md` 改名为 `architect-landing-summary.md`（内容 packet-binding 行已在场：引 FROZEN PROPOSAL + landing-manifest 三路径），改后结构门须复跑并绿。降级说明：本轮三项机检跳过属制品缺席所致，非 git 降级；我已以人读 diff 补足语义核对（见 §2），但**结构门绿是放行前提，不可由人读代偿**。

## 2. 逐文件语义核对（人读 diff，三 manifest 文件全覆盖）

| 文件 | structuralTouched | semanticMatch | expected（PROPOSAL 冻结改法） | actual（diff 实证） |
|---|---|---|---|---|
| `.claude/scripts/merge-driver.mjs` | yes | ok（附 1 项轻微偏差，见 §3-D1） | T1 入口消解+board 刷新 / T2 Step 0 白名单收口 / T3 投影特判混合集整体拒绝 / T6 Step 5b 落 main 簿记 | 四改法全数在位且方向一致（逐条见 §4） |
| `.gitattributes` | yes | ok | T4 单行 `.planning/kidpal-mvp/progress.md merge=union` | 该行在位 + 4 行注释（landing-summary 已披露；行为面 == 冻结文） |
| `.claude/references/pod-coordination.md` | yes | ok | T5 追加节不动既有节：runbook + 簿记落点口径 + OQ2=(a) 成立性注记 | 纯新增 §10（10a 五步合法消解序 + 10b 三条口径），既有节零改动（diff 无删改行） |

**diff 范围核对**：`git diff --stat main..c9f743e` 仅五文件——三 manifest 文件 + 证据目录两件（PROPOSAL.md 冻结过程演进 + landing-summary.md 新增）。canonical 面零 scope creep，无 manifest 外私货。

### §4 逐 T 核对要点

- **T1**：`approvalRefUnlock()` 落在 main() ready 收集前（freshBoard 之后），SINGLE_POD + HUMAN_APPROVAL_REF + !DRY_RUN 守卫；gate spawn 参数形态与既有 Step 1b 透传逐字同构（:916 vs :1133，仅多 `--human-approval-ref`），「同 Step 1b 透传路径」成立；setPod(ready-to-merge) + reconcile + `board.pods = refreshed.pods` 回写内存 board（R1-F1 修正落实，不修则仍 no-op 的缺口已闭）；fail-closed kept-blocked 并 stderr 报因；无 ref 零变化（T1c 自检亦证）。
- **T2**：`governanceFaceAutocommit()` 落 Step 1c 后、rebase 步前；白名单两前缀 + `<pod-id>` 目录段精确等值（含 `/` `\` 分隔符、`current`、`.`、`..` 显式拒收——R1-F6 落实，`current/` 不可能落入白名单）；add 精确路径 + pathspec 限定 commit；白名单外 warn 留痕返回 false、不吞（rebase 步随后 fail-closed，现状保持）。
- **T3**：`tryResolveProjectionConflicts()` 挂 rebase catch；冲突集 ⊆ {current/taskpackage.json, current/verdict.json} 才 `checkout --theirs`（rebase 语境 = 被合并包赢，方向与 R1 亲核一致）+ add + `GIT_EDITOR=true git rebase --continue` 循环（上限 16 轮）；含任一非特判文件 / 未进冲突态 / 探测失败 / 超轮次 → 返回 false 走原有整体 abort + blocked——R1-F4「特判不部分生效」落实。
- **T6**：`step5bBookkeeping(mergeCwd, …)` 落 merge OK 后（:1301），mergeCwd = `findMainHolderWorktree(wt) || wt`（:1259，持有 main 的合并现场，与冻结文前提一致）；事件形状（schemaVersion 1 / eventId UUID / changeId / timestamp / eventType=merge-completed / actor=merge-driver / source / payload.summary）与既有 pod-events 事件惯例逐字段相容（抽查 260829-schedule-p2 事件行核对）；append 单文件 + `git commit … -- <evRel>` pathspec 限定（不夹带 staged graph 残留）；失败全函数 try/catch → advisory warn，绝不 errorExit / 未捕获抛出（R1-F3 落实）；DRY_RUN 调用点守卫跳过；既有 Step 6/7 编号未动（F-5 落实）。

## 3. 偏差清单

| # | 位置 | 类别 | 偏差 | 严重度 |
|---|---|---|---|---|
| **D1（阻断）** | 证据目录制品名 | 结构门失败 | landing summary 落为 `landing-summary.md`，frozen packet 与门脚本要求 `architect-landing-summary.md`，致结构门红、三项机检未跑 | 阻断——改名 + 门复跑绿后方可进合并门 |
| **D2（非阻断）** | merge-driver.mjs `step5bBookkeeping` payload | 内容轻微缺项 | 冻结文 T6 要求 payload 摘「main tip sha / human-approval-ref / **冲突消解摘要**」三项；落地 payload.summary 含 main tip + 模式 + approval-ref，**缺冲突消解摘要**成分 | 轻微——不改变行为面与审计锚点效力；建议主线程随 D1 一轮补上（payload.summary 增补冲突消解结果一行，如「无冲突」/「投影特判 N 轮」），或在 retro 如实注记偏差并说明不补理由 |

## 4. 验证集复跑（核验人亲跑，2026-09-07）

| 项 | 结果 |
|---|---|
| `--self-check-retro-advisory` | ok:true（3/3） |
| `--self-check-quick-checks` | ok:true（expectedCmds 全等） |
| `--self-check-detect-changes` | ok:true（16/16） |
| `--self-check-rebase-target` | ok:true（7/7） |
| `--self-check-worktree-topology` | ok:true（9/9） |
| `--self-check-merge-friction`（本批新增第六自检） | ok:true（10/10——T1a/T1b/T1c/T2a/T2b/T2c/T3a/T3b/T4/T6 全 PASS） |
| `route-regression-check.mjs` | 28 ok, 0 fail |
| `lint-descriptions.mjs` | 1 warn（landing-verifier.md 超长 363>320）——先前批次遗留，非本批触面，与本批无关 |

## 5. 破坏性自检

- **planner.md 承诺语义**：planner.md 零改动（不在 diff 内）；T1 兑现其 :96 既有承诺，无语义冲突。
- **gate Step 1b 双跑幂等**：T1 在入口核验一次、Step 1b 合并链内再核验一次，gate 为只读校验（spawn、无写面），双跑幂等成立；且核验失败两处均 fail-closed，向严不向宽。
- **既有五自检语义**：五项复跑全绿，零回退。
- **其他 merge-driver 消费方**：pod-coordination.md 仅追加；pre-merge-escalation-gate.mjs / portfolio-reconcile.mjs 零改动；T6 簿记 commit pathspec 单文件限定，不与 graph skip-worktree 机制交叉（自检 T6 用 staged decoy 实证不夹带）。
- **全量模式后续 Pod 不被中断**：Step 5b 失败语义 advisory-only，符合 R1-F3 承重墙要求。

## 6. packet-binding 核对

landing-summary 引 packet = FROZEN PROPOSAL（R2=GREEN）+ landing-manifest 三路径——binding **内容**在场；但制品名偏差（D1）使门机检无法确认，binding 核对以人读补足：landing 实际范围（三文件）⊆ 被批 packet（T1–T6 目标文件并集），零超出。

## 7. 结论

**RED**——唯一阻断项 D1（summary 制品名不符 frozen packet 与门脚本要求，结构门红）。语义面三 manifest 文件逐条核对全部落地 == 冻结改法（R1 六项打回修正 F-1/F-3/F-4/F-5/F-6 逐一在 diff 实证），验证集亲跑全绿，破坏性自检无发现，仅 D2 一项非阻断 payload 成分缺项。

**放行条件**：主线程完成 D1（改名 `architect-landing-summary.md` + 结构门复跑绿）后方可进合并门；D2 建议同轮处理或如实留痕。本 verdict 不批准合并——合并门归 human。
