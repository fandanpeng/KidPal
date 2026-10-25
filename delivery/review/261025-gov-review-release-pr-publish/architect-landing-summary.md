# architect-landing-summary · 261025-gov-review-release-pr-publish

packet-id：FROZEN PROPOSAL §2 改法集（T1–T4）+ §5 验证集 · 评审轮次 R1 BLOCK(F1–F9) → R2 BLOCK(F10)+N1–N3 → R3 GREEN → R4 GREEN · change-id 261025-gov-review-release-pr-publish

## 1. 落地点 ↔ 改法对应

| 改法（FROZEN §2） | 落地文件 | 结果 |
|---|---|---|
| T1 · 新增发布/回同步单一入口 | `.claude/scripts/release-pr.mjs` | 新增（约 640 行）。`--publish` / `--sync` / `--dry-run` / `--self-check` / `--title`；预检按运行位置、清洁度、gh 可用、分支关系四道；push 经 `planPush` + `assertPushArgv` 白名单；gh 调用只经 `runGh` 且子命令白名单为 `auth status` / `pr list` / `pr create`；自检 19 个夹具全绿 |
| T2 · merge-driver 批末 handoff | `.claude/scripts/merge-driver.mjs` | 新增 `handoffLine` 纯函数并在批模式 SUMMARY 后、单 Pod 模式 `processPod` 返回 true 后打印（DRY_RUN 不打）；`runMergeFrictionSelfCheck` 增 T9a/T9b 断言；driver 内零 push / 零网络 / 零 gh 调用 |
| T3 · merge-to-main skill 发布与回同步段 | `.claude/skills/merge-to-main/SKILL.md` | description 与约束第一条按 FROZEN 文本逐字替换；「步骤」6 条后追加第 7 条（发布）与第 8 条（回同步，含 gov 回同步命令块） |
| T4 · 铁律文本收窄 | `AGENTS.md` · `docs/DEV-GOVERNANCE.md` · `.claude/references/arch-review-protocol.md` | 四处逐字替换（AGENTS.md:14、DEV-GOVERNANCE.md:214、protocol:56、protocol:58）；旧句全部退场 |

## 2. 验证证据

| 编号 | 命令 | 实际输出关键行 | 结果 |
|---|---|---|---|
| V1 | `node .claude/scripts/release-pr.mjs --self-check` | 19 行 `  PASS · <label>`；末行 `{"selfCheck":"release-pr","pass":19,"fail":0,"total":19,"ok":true}` | 绿 |
| V1 外部锚 | 同上重放 + `grep -c "^  PASS · <label>"` | `relation-diverged`=1、`push-plan-source-main`=1、`open-pr-same-tip-noop`=1、`publish-happy-path`=1 | 绿 |
| V1b | `grep -cF "refs/heads/release/" .claude/scripts/release-pr.mjs` | `6`（≥ 1；仅正向锚，不作行为证据） | 绿 |
| V2.1 | `node .claude/hooks/deny-main-push.mjs --self-check` | `{"selfCheck":"deny-main-push","pass":69,"fail":0,"total":69,"ok":true}`（钩子零改动，基线 69 不变） | 绿 |
| V2.2–V2.6 | 合成 stdin 六条 | `git push origin main:release/261102`→0；`git push origin main`→2（stderr 含 `targets main/master`）；`gh pr merge 31 --squash`→0；`gh -R fandanpeng/KidPal pr merge 31`→0；`gh pr view 31`→0；`git commit -m "gh pr merge 31"`→0 | 绿（后四条即 OQ1=(b)「现状即目标形态」） |
| V3 | `merge-driver.mjs --self-check-rebase-target` / `--self-check-worktree-topology` / `--self-check-merge-friction` | 7/7、9/9、22/22；`merge-friction` 输出含 `PASS · T9a` 与 `PASS · T9b`（`grep -c "PASS · T9"` = 2，≥ 1） | 绿 |
| V3 锚 | `grep -c "release-pr.mjs --publish" .claude/scripts/merge-driver.mjs` | `2`（≥ 1） | 绿 |
| V4 | 四处文本 grep | `release-pr.mjs --publish`：AGENTS.md=1、DEV-GOVERNANCE.md=1、protocol=2；`PR 合并与其它 remote push 仍 human` AGENTS.md=1；`PR 合并与其它 remote 同步仍 human` DEV-GOV=1、protocol=1；四条旧句均=0 | 绿 |
| V4 skill | `grep -cF ... merge-to-main/SKILL.md` | `release-pr.mjs`=4、`7. **发布（远端）**`=1、`8. **回同步（human 合并 PR 之后）**`=1、`不发 PR`=0 | 绿 |
| V5 | `gh auth status` / protection / repo 设置 | exit 0；`true,0,false`；`delete_branch_on_merge=true` | 绿（外部状态与 2026-10-25 基线一致） |
| live 预检 | 主 worktree 内 `node <abs>/release-pr.mjs --publish --dry-run` | `[release-pr] dirty: working tree is not clean`，点名 `delivery/review/INDEX.md`，exit 2 | 绿（fail-closed 形态正确，见 §3 第 6 条） |

真空绿规约遵守情况：自检输出含自检名 `release-pr`；19 个夹具输入均为非空构造；`publish-happy-path` 以 fake runner 记录真实 argv（该阶段 push 恰一次、gh create 恰一次、argv 集合相等），不是源码字符串在场检查。

## 3. 诚实注记

1. **首次真实 `--publish` 未执行**：本轮验证只到 19 个夹具、六条钩子合成输入与一次主 worktree dry-run；首次真实 push / 开 PR 属上线事件，须 human 在场，结果待补记。
2. **`gh pr merge` 无机械牙**（OQ1=(b) 已由 human 裁定）：任何会话一条命令即可自合 PR；`gh api` / `curl` / 网页同不在覆盖。留根见 PROPOSAL §6 第 1 条。
3. **`release-pr.mjs` 的 push 在 `deny-main-push.mjs` 视野外**：钩子只看 Bash 命令串。现有三道防线 = 脚本 push argv 白名单、`.claude/scripts/**` 属 G3（agent 不可写）、landing 人读 diff。留根见 §6 第 3 条。
4. **推 `release/*` 即公开该分支内容**（仓库 PUBLIC）：撤回 = human 删分支；脚本刻意不提供删分支能力。
5. **存量 14 条 `release/*` 未清理**（OQ4=(a)）：`delete_branch_on_merge=true` 覆盖新 PR。
6. **主 worktree 的 `delivery/review/INDEX.md` 未提交改动**（纯 CRLF 噪音，`git diff` 无内容差异）会让首次 `--publish` fail-closed 点名；清理归主线程，属正确行为而非缺陷。
7. **gov 回同步未并入脚本**（OQ3=(b)）：`--sync` 只做本地 `merge --ff-only origin/main`；`debt-render` 与 `changes-index --upsert` 仍由主线程按 skill 步骤 8 执行。
8. **未跑全量回归**：本批不触 `verify-gates.json`；验证范围 = 脚本自检、既有 merge-driver 三组自检、钩子自检、文本断言与环境前提。`route-regression-check` 52 ok / 0 fail；`lint-descriptions` scanned 16 / flagged 0。

```landing-manifest
.claude/scripts/release-pr.mjs
.claude/scripts/merge-driver.mjs
.claude/skills/merge-to-main/SKILL.md
AGENTS.md
docs/DEV-GOVERNANCE.md
.claude/references/arch-review-protocol.md
```
