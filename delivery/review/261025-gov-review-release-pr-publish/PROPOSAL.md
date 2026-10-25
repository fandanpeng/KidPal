# PROPOSAL · 261025-gov-review-release-pr-publish

> **状态：FROZEN v1.0 · 2026-10-25 · 主线程凭据自冻**——凭据三件联立：① human 立项 ack（2026-10-25 本会话：原话「现在我们有远端仓库了，每次merge到local main还需要人工发起PR，这个流程是不是可以集成到merge deriver 和merge skill里」，+ 四裁 OQ1=(b) 不拦 `gh pr merge` / OQ2=(a) 任何会话 + 机械前提 / OQ3=(b) `--sync` 只做本地 ff / OQ4=(a) 不清理存量分支，经 AskUserQuestion 留痕，§4 有裁定行）；② architect-reviewer **R4 GREEN**（§7：R1 BLOCK(F1–F9) → v0.3 → R2 BLOCK(F10)+N1–N3 → v0.4 → R3 GREEN → v0.5 N4/N5 措辞收口 → R4 GREEN 维持；判档 = 主路径 Track B，Track A 段不触发）；③ 契约面 = 零（无契约包三节）。版本演进：v0.1 起草 → v0.2 四裁裁剪（T5 删除）→ v0.3 R1 修订 → v0.4 R2 修订 → v0.5 N4/N5 收口。**
> **判档**：主路径 Track B（全件制品，retro 必写；非轻路径瘦制品五件）。
> **承载 ARQ**：无（human 直接发起，原话「现在我们有远端仓库了，每次merge到local main还需要人工发起PR，这个流程是不是可以集成到merge deriver 和merge skill里」；已裁边界 = 脚本开 PR、人点 merge；merge-driver 只打印 handoff、不塞 push；skill 增 publish + 回同步段）。
> **review 轨**：Track B 主路径（全件制品，retro 必写；SSOT：`.claude/references/arch-review-protocol.md`「双轨路由」节、`.claude/references/gov-change.md`）。**G1 触面显式申报**：`AGENTS.md` 与 `docs/DEV-GOVERNANCE.md` 中的 remote push 文本按 human 已裁边界收窄，方向已收敛；Track A 段判定归判官（R1 已判不触发，只升不降）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T4）+ §5 验证集。packet = FROZEN PROPOSAL + landing-manifest + architect-landing-summary。
> **riskTier**：medium（新增 remote 写能力〔仅 `release/*` 分支〕+ G1/G2 铁律文本收窄 + G3 脚本面；无契约面改动，钩子面零改动〔OQ1=(b)〕）。
> **review profile**：G1+G2+G3（G1 显式升级申报见上；契约面 = 零）。

```landing-manifest
.claude/scripts/release-pr.mjs
.claude/scripts/merge-driver.mjs
.claude/skills/merge-to-main/SKILL.md
AGENTS.md
docs/DEV-GOVERNANCE.md
.claude/references/arch-review-protocol.md
```

---

## 0. 想实现的效果与差距

**目标效果**：本地 main 合入新批次之后，主线程用一条命令完成「推 release 分支 + 开 PR + 打印下一步」，human 在 GitHub 网页点合并，再用一条命令把本地 main 追平。发布分支的基点结构性锁定为**发布时的本地 main tip**：按 PR merge commit 的父提交结构，该 tip 是 merge commit 的第二父，因此在「发布后本地 main 不再前进」的前提下，human 合并后 `git merge --ff-only origin/main` 恒成立。若发布后、human 合并前本地 main 又前进，则须再发一次（重复 `--publish` 覆盖新提交并合并新 PR）——该条件与残余见 §6。

**现状差距**：

1. 远端是后加的（当前仓库为 PUBLIC；公开的具体日期不在本批亲测范围），发布全程靠人记步骤：切 release 分支、写标题正文、开 PR、合并后回同步，没有任何脚本或 skill 段落承载。同一天同一批发布了 PR #31 与 PR #32 两条，就是重复手工的现成实例。
2. 分支基点由人临场决定。2026-10-25 上午 PR #31 的 `release/261025` 从旧点 `173c2a12` 切出，而当日本地 main tip 是 `3e9d3692`；合并后本地 main 与 origin/main 互非祖先，`merge-driver.mjs` 的 `resolveRebaseTarget` 判 `diverged` → fail-closed blocked，整批合并停摆。
3. 「PR 合并由 human 在网页点」目前只有文档约定，**没有机械拦截**：`gh pr merge` 不被 `deny-main-push.mjs` 拦（实测 exit 0），任何 agent 都能一条命令自合。human 已在 OQ1 裁 (b)：本批不加机器面，此项作为**已裁定的已知缺口**保留（裁定与残余见 §4、§6），不再是本批的改法对象。

**非目标（本批不做）**：

- 不自动合并 PR、也不给 PR 合并加机械拦截（human 网页点击仍是唯一合并动作；OQ1 已裁 (b)，残余记 §6）。
- 不推 main、不 force push、不删远端分支、不做部署、不做 CI 触发的自动发布。
- 不把外向 push 塞进 merge-driver 的 per-Pod 循环（其失败语义是 per-Pod blocked/retry，后置失败会连坐已合并成功的 Pod）；driver 批末只打印一行 handoff。
- 不改契约面（`docs/contracts/**` 零改动），不改任何角色的「不 push / 永不 merge 进 main / 不许直推 main」边界——这些边界仍成立，本批只在**主线程的发布动作**上开一个「只推 `release/*`」的机器口。
- 不清理远端存量 release 分支（OQ4 推荐不清理）。

## 2. 问题、根因与改法（T1…T4）

> 本节全部断言均于 2026-10-25 在本仓实测留痕；命令与关键输出逐字记在各 T 的「证据」段。凡未实测的面，文中显式写「未实测」。

### T1 · 无机械发布入口：本地 main 合并后靠人手推 release 分支、开 PR、回同步

**问题**：仓库有了远端与分支保护后，每一次本地 main 前进都要人工完成四件事：确认分支基点、推 release 分支、写 PR 标题正文、human 合并后回同步。步骤分散、重复劳动，且分支基点由人临场决定，一旦选到旧点就会造成本地 main 与 origin/main 分叉，把代价推迟到下一次 merge-driver 合并时以 blocked 形式暴露。发生频率 = 每批发布一次（`gh pr list --state merged` 实测按日计数：2026-10-23 单日 12 条〔#17–#28〕、10-24 两条、10-25 两条；同一批任务一天内两次重复手工即 PR #31/#32）。

**证据**：

- 远端与仓库属性：`git remote -v` 实测 `origin https://github.com/fandanpeng/KidPal.git (fetch/push)`；`gh repo view --json nameWithOwner,visibility,defaultBranchRef` 实测 `{"defaultBranchRef":{"name":"main"},"nameWithOwner":"fandanpeng/KidPal",...,"visibility":"PUBLIC"}`。
- 分支保护（`gh api repos/:owner/:repo/branches/main/protection`，2026-10-25 实测原文节选）：`"required_approving_review_count":0`、`"enforce_admins":{"enabled":true}`、`"allow_force_pushes":{"enabled":false}`、`"allow_deletions":{"enabled":false}`，且响应中**无** `required_status_checks` 字段。含义：直推 main 恒 403（含 owner），PR 合并不需要 reviewer 批准。
- 仓库合并方式（`gh api repos/:owner/:repo --jq '{...}'` 实测）：`{"allow_merge_commit":true,"allow_rebase_merge":false,"allow_squash_merge":false,"delete_branch_on_merge":true}`。含义：PR 只可能以 merge commit 形态落地，且合并后 head 分支由 GitHub 自动删除。
- 今天同批两条 PR（`gh pr view 31/32` 实测节选）：PR #31 `headRefName=release/261025`、`mergeCommit.oid=90aa6af5b03448e2a2ac326ef85251eb8c6d874d`；PR #32 `headRefName=release/261025-2`、`mergeCommit.oid=fa60dd2f416811a3babfaa4fe080529a26e1d7de`（即当前 main）。
- merge commit 父提交结构（`git log -1 --format='%H %P %s'` 实测逐字）：
  - `90aa6af5... 78758ac740aa65d1b9468f388b9dc5056f737676 173c2a12c5e1b0c7eb9050e2ad6dd3ac875a7446 Merge pull request #31 from fandanpeng:release/261025`
  - `fa60dd2f... 90aa6af5b03448e2a2ac326ef85251eb8c6d874d c349d4f1fa118b6cca7f2f929876679e7a147f50 Merge pull request #32 from fandanpeng:release/261025-2`
  ⇒ 第一父 = GitHub 合并时的 base（当时 origin/main），第二父 = release 分支 tip。**结构性承重点**：脚本若把 release tip 推成恰好等于**发布时的**本地 main tip，则本地 main 是 merge commit 的第二父，恒为其祖先；之后无论 base 如何前进，`git merge --ff-only origin/main` 都成立（限定条件同 §0：发布后本地 main 不再前进；若又前进须再发一次，见 §6）。反之，若 release 分支从旧点切出，本地 main 就与合并结果互非祖先——今晨分叉即此形态。
- 今晨分叉实测：`git log -1 --format='%h %s'` 实测 `173c2a12 chore(devloop): 261024-memory-recall-tool-enable merge 完成事件入 pod-events（merged c583f3e8fa24）`、`3e9d3692 chore(devloop): 261025-kidpal-tools-profile-fix merge 完成事件入 pod-events（merged 9e405295df03）`；`git merge-base --is-ancestor 173c2a12 3e9d3692` 成功（是祖先）、反向失败（不是祖先）。`merge-driver.mjs` 的 `resolveRebaseTarget`（源码 597–643 行）在两 ref 互非祖先时返回 `{ ok: false, reason: 'diverged' }`（631 行），`processPod` 对其 fail-closed 置 blocked（1422–1426 行）；`--self-check-rebase-target` 第 4 案即断言「diverged → blocked」。
- 存量 release 分支（`git ls-remote --heads origin "release/*"` 实测）：共 **14 条**（先前人工估计「16+」与实测不符，以实测为准），逐条映射 PR（后续条目省略同一 `release/` 前缀）：`release/261013→#1`、`261014→#3`、`261014-2→#4`、`261014-3→#5`、`261015→#6`、`261015-2→#7`、`261016→#8`、`261016-2→#9`、`261017→#10`、`261017-2→#11`、`261018→#12`、`261019→#13`、`261019-2→#14`、`261019-3→#15`；PR #16 起的 release 分支（含 #31/#32）远端均不存在。与 `delete_branch_on_merge=true` 一致：自动删分支对设置生效后的新 PR 成立，存量 14 条是历史残留。
- 仓内无自动化：对 `.claude`、`docs`、`delivery` 全量 `grep -rIn -E "release/|gh pr create|pr create"` 实测唯一命中 `delivery/workstreams/device-memory-overhaul/progress.md:129`（叙述性一句）；`.claude/scripts/` 目录清单中无 `release-pr.mjs`（实测 ABSENT）。
- 钩子放行面（合成 stdin，`node .claude/hooks/deny-main-push.mjs`，2026-10-25 实测）：`git push origin main:release/261102` → exit 0；`git push origin HEAD:release/261102` → exit 0；`git push origin main` → exit 2，stderr `merge-gate: git push targets main/master (refspec "main")...`。即「只推 `release/*`」已落在钩子放行侧。
- 环境前提：`gh auth status` exit 0（账号 fandanpeng，token scopes 含 `repo`）；`gh --version` = `gh version 2.96.0`；`gh pr create --help` 实测含 `-H, --head`、`-B, --base`、`-t, --title`、`-F, --body-file`；`gh pr list --json headRefOid` 实测可取（PR #31 返回 `173c2a12c5e1b0c7eb9050e2ad6dd3ac875a7446`）。

**根因**：发布流程停留在「人记步骤」。三个子根因：① 没有单一入口，分支基点、PR 文本、回同步各自靠记忆，重复劳动且漂移；② 没有结构性前置，分支基点选错不会在发布时被任何机械判据拦住，只能在后续合并时以 fail-closed 暴露；③ 没有幂等面，「push 成功但开 PR 失败」「同一 tip 重复跑」都会留下人工判断负担，今晨即为此重复发布了两条 PR。

**改法**：新增 `.claude/scripts/release-pr.mjs`（G3 脚本，头注 `owner=architect, approver=human (G3 script, see DEV-GOVERNANCE.md §6)`，形制对齐 `merge-driver.mjs` 与 `deny-main-push.mjs`），提供 `--publish`、`--sync` 两个子命令与 `--dry-run`、`--self-check`、`--title` 选项。

`--publish`（预检全部通过前不产生任何远端写）：

1. **运行位置**：`git rev-parse --abbrev-ref HEAD` 必须为 `main`，即必须在持有 `refs/heads/main` 的主 worktree（本仓 `C:/AI/KidPal`）上运行；在 gov / Pod worktree 运行直接拒绝（exit 2）。
2. **工作树清洁**：`git status --porcelain -uall` 仅允许 `.codebase-memory/artifact.json` 与 `.codebase-memory/graph.db.zst` 两条（常驻脏的机器再生面，按 `AGENTS.md`「CodebaseMemory 使用纪律」约定不计清洁度）。**豁免按路径计、不限状态码**（`M`、`D`、`??` 等同一路径形态一律放行）；其余任一文件在场即拒绝并逐条点名。
3. **gh 可用**：`gh auth status` 成功（token 需含 `repo` scope）。
4. **分支关系可判定**：`git fetch origin main` 后比较 `main` 与 `origin/main`——相等 → 打印 `nothing to publish` 并 exit 0；`origin/main` 是 `main` 祖先 → 继续；`main` 是 `origin/main` 祖先 → 拒绝（提示先跑 `--sync`）；互非祖先 → 拒绝（diverged，交 human）。判据与 `resolveRebaseTarget` 的 diverged fail-closed 同向。
5. **分支命名**：`release/<YYMMDD>`（本地日期），冲突依次 `-2`、`-3`…（上限 `-20`，超出即拒绝）。冲突判定同时看远端 refs 与历史 PR 的 `headRefName`（`gh pr list --state all --limit 100 --json headRefName`——显式 `--limit 100`，避免 gh 默认 30 条截断；现仓已有 32 条 PR。已合并 PR 的分支虽被 GitHub 自动删，名字仍算已用，保证同日多批单调递增）。远端已有同名分支且 tip 恰等于本地 main tip → 复用该分支、不再 push（覆盖「push 成功、开 PR 失败」的重跑）；同名但 tip 不同 → 不 force，改后缀。
6. **幂等闸**：`gh pr list --state open --json number,url,headRefName,headRefOid` 中存在 `headRefOid` 等于本地 main tip 的 open PR → 打印该 PR 的 URL，exit 0，不 push 不建 PR；同时打印一句「发布后本地 main 若继续前进，须再发一次」（F2 条件提醒）。
7. **push（argv 白名单，行为面断言）**：所有 push 都经纯函数 `planPush` 生成 argv、经可注入的 runner 执行；argv 白名单在自检里以 fake runner 记录实测——源 token 恒为 `main`、目标 token 恒以 `refs/heads/release/` 起头（形如 `main:refs/heads/release/<name>`）、参数集不含 `-f`／`--force`／`--force-with-lease`／`--delete`／`--mirror`／`--all`。子进程设 `GIT_TERMINAL_PROMPT=0` 防凭证交互挂起。push 失败 → exit 1。
8. **开 PR**：`gh pr create --base main --head <name> --title <title> --body-file <tmp>`。标题默认 `release(<YYMMDD>[-n]): <N> 个 Pod 合并`，`--title` 可覆盖；正文从下列范围命令的输出中匹配 merge-driver 合并行，提取 changeId（按序去重）逐条列出：

```text
git log --first-parent --format=%s origin/main..main
^Merge (\S+) \(merge-driver\)
```

正文另写明基线 SHA、release tip SHA、「仓库仅允许 merge commit」与「合并后跑 `--sync`」的下一步提示。方案骨架给出的标题形态 `release(<YYMMDD>): <摘要>` 中，自定义摘要由此处 `--title` 承担（自动摘要只保证机械可生成，不冒充人写的业务摘要）。
9. **退出码**：0 = 成功或幂等无操作；1 = push / gh 执行失败（含「push 成功但开 PR 失败」，此时提示重跑安全，重跑走第 5 条复用分支路径）；2 = 预检拒绝（含运行位置/清洁度/gh 未登录/分支关系不可判定，以及 **`git fetch origin main` 失败或网络不可用**——此时尚未产生任何远端写，归预检拒绝）。
10. **禁止项（行为面断言，不用源码字符串自扫描）**：不合并 PR、不推 main、不 force push、不删分支、不写仓库文件、不改任何 canonical。落地为可执行判据：所有 gh 调用只经 `runGh(args, runner)`，子命令白名单 = `auth status` / `pr list` / `pr create`，白名单外（含 `pr merge`、`api`）在调用前即被拒绝；自检以 fake runner 记录 argv，断言记录的 gh 子命令集合 ⊆ 白名单，且构造 `['pr','merge','31']` 的调用被拒（不执行）。

`--sync`：

1. 运行位置与清洁度同 `--publish` 第 1、2 条。
2. `git fetch origin main`；`main` 与 `origin/main` 相等 → 打印 `already in sync` 并 exit 0；`main` 是 `origin/main` 祖先 → `git merge --ff-only origin/main`（破坏性为零；非 ff 即停）；否则拒绝（exit 2，分叉交 human）。
3. 只做本地追平，**不 push**。gov 回同步（`main → gov/kidpal` merge、`.codebase-memory` 两件定点还原、`debt-render`、`changes-index --upsert`）按 OQ3 裁断决定是否并入；推荐 (b) 写进 T3 的 skill 段、由主线程执行。

`--dry-run`：只跑预检与计划打印，不做任何写。`--self-check`：见 §5 V1（fake git/gh 注入，不联网、不触远端）。

对应验证：§5 V1 的夹具标签（`relation-*`、`dirty-*`、`wrong-worktree`、`name-*`、`open-pr-same-tip-noop`、`extract-changeids`、`push-plan-source-main`、`gh-argv-whitelist`、`publish-happy-path`、`sync-*`）与 V1b 正向锚。

目标文件：`.claude/scripts/release-pr.mjs`（新增）

### T2 · merge-driver 批末无 handoff：发布的下一步靠人记忆

**问题**：merge-driver 把 Pod 合并进本地 main 后直接退出，不告诉操作者下一步要发布；发布动作完全依赖人记得「还有一步」。发生频率 = 每次合并批次结束时一次。

**证据**：`merge-driver.mjs` 批模式在 1682 行打印 `log(\`SUMMARY: ${merged} merged, ${skipped} skipped/blocked, ${ready.length} total ready\`)`，1683 行 `process.exit(skipped > 0 ? 1 : 0);`；单 Pod 模式 1669–1670 行 `const ok = processPod(changeId, devloopDir, projectDir, quickChecks); process.exit(ok ? 0 : 1);`。`grep -n "git push\|release/\|release-pr" .claude/scripts/merge-driver.mjs` 实测零命中——driver 全文既不提发布，也没有 push 面。

**根因**：合并与发布是两个阶段，driver 的职责边界是本地合并（不 push）；但阶段交界处缺一个单向、零网络、零失败面的提示，交接只能落在人的记忆里。

**改法**：

- 新增纯函数 `handoffLine(mergedCount)`（`mergedCount <= 0` 返回 `null`），文本单行（逐字）：

```text
handoff: 本地 main 已前进 ${merged} 个 Pod——发布：node .claude/scripts/release-pr.mjs --publish（只推 release/* 并开 PR）；PR 合并由 human 在网页点；合并后跑 --sync 追平本地 main
```

- 批模式：1682 行 SUMMARY 之后追加 `if (!DRY_RUN && merged > 0) log(handoffLine(merged));`。
- 单 Pod 模式：`processPod` 返回 true 时先 `log(handoffLine(1))` 再 `process.exit(0)`。
- DRY_RUN 不打印（未发生合并，避免「已前进」的假陈述）。
- driver 内不新增任何 push / 网络 / gh 调用；批次失败语义（per-Pod blocked / 退出码）零改动。
- `runMergeFrictionSelfCheck` 增一条断言（编号 T9）：`handoffLine(0)` 为 `null`；`handoffLine(3)` 为单行（不含换行）、含 `release-pr.mjs --publish`、含 `human`。driver 不新增子进程/网络调用的保证由 T2 改法本身与 landing 人读 diff 承载，**不用源码字符串自扫描**（F1 口径）。

对应验证：§5 V3 的 `PASS · T9` 断言与 handoff 正向锚。

目标文件：`.claude/scripts/merge-driver.mjs`（改）

### T3 · merge-to-main skill 无发布会话面，且边界句仍写「不发 PR」

**问题**：`merge-to-main` skill 是主线程合并动作的入口，但它写于无远端时代：描述与约束都明说「不 push、不发 PR」，发布与回同步没有任何落点，实际步骤只活在会话记忆与零散备忘里。

**证据**（逐字）：`SKILL.md:3` description 含「边界: 只做本地合并与验证；不 push、不发 PR、不直推 main/master（对齐 deny-main-push.mjs 与「本地 main 合并由 planner/主线程前置升级闸放行后自动触发、remote push 仍 human」铁律；OQ-3 已裁 (a)）。」；`SKILL.md:26` 含「**永不 push**、永不直推 main/master（`deny-main-push.mjs` 钩着这条铁律；入本地 main 由 planner/主线程经前置升级闸放行后自动合并，remote push 仍由 human 经 PR）。」

**根因**：skill 成文时仓内无远端、发布不存在；远端引入后没有把「发布/回同步」补进同一个入口，边界句停留在旧世界，反而会让照 skill 执行的会话拒绝发布。

**改法**：改两处边界文本，并在「步骤」6 条之后追加第 7、8 条。三处落地文本全部逐字给出：

1. frontmatter `description` 整行，原文（逐字）：

```text
description: "Use when: 把 feature/gov 分支合并到目标分支（main 或同步反向）的本地合并操作——预检 overlap、ff-only 合并、查残余冲突标记、确认落点。触发词: 合并分支, merge 到 main, 同步 main, sync dev, 合分支, merge-to-main。边界: 只做本地合并与验证；不 push、不发 PR、不直推 main/master（对齐 deny-main-push.mjs 与「本地 main 合并由 planner/主线程前置升级闸放行后自动触发、remote push 仍 human」铁律；OQ-3 已裁 (a)）。"
```

替换为（完整句）：

```text
description: "Use when: 把 feature/gov 分支合并到目标分支（main 或同步反向）的本地合并操作——预检 overlap、ff-only 合并、查残余冲突标记、确认落点；远端发布（push release/* + 开 PR）与回同步走 release-pr.mjs。触发词: 合并分支, merge 到 main, 同步 main, sync dev, 合分支, merge-to-main, 发布, publish, 回同步。边界: 本地合并与验证在本 skill；远程发布 = 主线程跑 release-pr.mjs --publish（只推 release/* 并开 PR），PR 合并与其它 push 仍 human。"
```

2. 「约束（must / never，不可省）」第一条，原文（逐字）：

```text
- **永不 push**、永不直推 main/master（`deny-main-push.mjs` 钩着这条铁律；入本地 main 由 planner/主线程经前置升级闸放行后自动合并，remote push 仍由 human 经 PR）。
```

替换为（完整句）：

```text
- **本 skill 永不 push**、永不直推 main/master、永不合并 PR（`deny-main-push.mjs` 钩着推 main/master 面）；入本地 main 由 planner/主线程经前置升级闸放行后自动合并；远程发布 = 主线程跑 `node .claude/scripts/release-pr.mjs --publish`（只推 `release/*` 分支并开 PR），PR 合并由 human 在网页点，其它 remote push 仍 human。
```

3. 在「## 步骤（每步有可检查终点）」的 6 条之后**追加**第 7、8 条（整段原样可落）：

```markdown
7. **发布（远端）** — 在本地 main 合并（前置升级闸放行后由主线程自动落定）之后、于主 worktree（本仓 `C:/AI/KidPal`）分支 `main` 上运行 `node .claude/scripts/release-pr.mjs --publish`。脚本预检：运行位置为主 worktree 且 HEAD 为 `main`；工作树干净（`.codebase-memory/artifact.json`、`.codebase-memory/graph.db.zst` 两件常驻脏豁免）；`gh auth status` 可用；main 与 origin/main 关系可判定（相等 = 无可发布；origin/main 是 main 祖先 = 正常；main 落后或互非祖先 = 拒绝交 human）。发布分支 = 本地 main tip，命名 `release/<YYMMDD>`／`-2`／`-3`…；标题默认 `release(<YYMMDD>): <N> 个 Pod 合并`，可用 `--title` 覆盖。脚本只推 `release/*` 并开 PR，永不合并 PR、永不推 main。重复跑安全：同一 tip 已有 open PR 会打印 URL 直接退出；push 成功但开 PR 失败时，重跑会复用同名同 tip 分支。终点：打印 PR URL；人工核对标题与正文后在 GitHub 网页点合并（仓库仅允许 merge commit，合并后 head 分支由 GitHub 自动删除）。
8. **回同步（human 合并 PR 之后）** — 主 worktree 追平本地 main：

        node .claude/scripts/release-pr.mjs --sync

   终点：打印新的 main tip，`git log --oneline -3` 首行是 `Merge pull request #<n> ...`。脚本只做本地 `merge --ff-only origin/main`，不 push；分叉即停交 human。gov 回同步（主线程在 gov worktree 执行；OQ3 裁 (a) 时改由 `--sync` 一并完成，见 §4）：

        git -C C:/AI/KidPal-gov checkout HEAD -- .codebase-memory/artifact.json .codebase-memory/graph.db.zst
        git -C C:/AI/KidPal-gov merge main
        node .claude/scripts/debt-render.mjs
        node .claude/scripts/changes-index.mjs --upsert <本批 changeId>   # 逐个

   终点：gov worktree 净树（`.codebase-memory` 两件之外无未提交改动）；`docs/debt/*/INDEX.md` 与 `delivery/review/INDEX.md` 与条目状态一致；提交一条 `chore(sync): main → gov/kidpal 回同步`。`--upsert` 必须跑在落地 commit 之前，否则生成件漏带（既有实例两次）。
```

对应验证：§5 V4 的 skill 四条 grep（`release-pr.mjs` ≥ 4、步骤 7/8 标题、`不发 PR` = 0）。

目标文件：`.claude/skills/merge-to-main/SKILL.md`（改）

### T4 · 「remote push 仍 human」铁律文本收窄：只留一个机器例外

**问题**：human 已裁定「脚本开 PR、人点 merge」，但四处 canonical 文本仍把「remote push / remote 同步」整体写成 human-only，与落地后的机制直接矛盾。若不同批收窄，skill 与脚本的合法性会依赖读者自行解释，判官与 landing-verifier 也无处对照。

**证据**（逐字，2026-10-25 全仓固定串扫描实测）：

- `AGENTS.md:14` 句尾：`例外升级，remote push 仍 human。`
- `docs/DEV-GOVERNANCE.md:214` 子句：`human 把关点 = **立项 ack（前端对齐：需求/方向/量级）+ 例外反选（前置升级闸）；本地合并自动，remote 同步仍 human**。`
- `.claude/references/arch-review-protocol.md:56` 句尾：`remote push 仍 human；升级闸拦 → 升级人。`
- `.claude/references/arch-review-protocol.md:58` 子句：`merge 恒由主线程在前置升级闸放行后自动落本地 main，remote 同步仍 human。`

**根因**：这几句写于「远端 push 一律人工」的时代，是铁律的正文明文；机制变了，正文没跟上，形成「文档说不能、脚本在做」的文本漂移。

**改法**：四处逐字替换（原文 → 替换后完整句），措辞统一为「唯一机器例外 = 主线程跑 release-pr.mjs --publish，只推 release/* 并开 PR；PR 合并与其它 push/同步仍 human」。

1. `AGENTS.md:14` 整行（blockquote 行）——原文见上；替换为：

```text
> **前端对齐 + 例外反选（260831-gov-review-auto-merge-local-main 起，261015 批续贯）**：human 只在两个真正判断点出场——① 前端一次性对齐「为什么改（需求）/ 往哪改（方案）/ 改动多大（量级）」= 立项 ack；② 命中例外清单升级拍板。其余（评审/实现/验证/落地核验/合并本地 main）全部 agent + 机械闸自动，例外升级；远程发布 = 主线程跑 `node .claude/scripts/release-pr.mjs --publish`（只推 `release/*` 分支并开 PR），PR 合并与其它 remote push 仍 human。
```

2. `docs/DEV-GOVERNANCE.md:214` 子句——原文见上；替换为：

```text
human 把关点 = **立项 ack（前端对齐：需求/方向/量级）+ 例外反选（前置升级闸）；本地合并自动，远程发布 = 主线程跑 `node .claude/scripts/release-pr.mjs --publish`（只推 `release/*` 分支并开 PR），PR 合并与其它 remote 同步仍 human**。
```

3. `.claude/references/arch-review-protocol.md:56` 句尾——原文见上；替换为：

```text
remote push 仍 human（唯一机器例外 = 主线程跑 `node .claude/scripts/release-pr.mjs --publish`，只推 `release/*` 分支并开 PR；PR 合并与其它 push 仍 human；主线程在 human 网页合并 PR 后跑 `node .claude/scripts/release-pr.mjs --sync` 追平本地 main）；升级闸拦 → 升级人。
```

4. `.claude/references/arch-review-protocol.md:58` 子句——原文见上；替换为：

```text
merge 恒由主线程在前置升级闸放行后自动落本地 main；随后发布走 `node .claude/scripts/release-pr.mjs --publish`（只推 `release/*` 分支并开 PR），PR 合并与其它 remote 同步仍 human。
```

**复扫模式完备性（硬项 · 退役语面）**：模式集显式枚举为 `remote push`、`remote 同步`、`不发 PR`、`永不 push`、`不 push`、`直推 main`、`不直推`、`gh pr`、`release/` 九个固定串，命令 `grep -rn -F <pattern> AGENTS.md CLAUDE.md docs .claude`（排除 `delivery/archive`、`docs/archive`、`docs/notes/showme`、`docs/debt` 历史域）。完备性论证：这些短语均为中文或英文固定串，`-F` 子串匹配对「有无 backtick 包裹」一并覆盖，故无需另列变体；「推」的其它动词形态（推送 / 直推）已由 `remote push`、`直推 main`、`不直推` 覆盖，`推送` 实测只出现在历史归档与 progress 叙述，不在活文本。实测命中与处置：

| 命中 | 处置 |
|---|---|
| `AGENTS.md:14`（remote push） | 本 T 改（上列 1） |
| `docs/DEV-GOVERNANCE.md:214`（remote 同步） | 本 T 改（上列 2） |
| `.claude/references/arch-review-protocol.md:56`（remote push） | 本 T 改（上列 3） |
| `.claude/references/arch-review-protocol.md:58`（remote 同步） | 本 T 改（上列 4） |
| `.claude/skills/merge-to-main/SKILL.md:3`（不发 PR）与 `:26`（永不 push） | T3 改 |
| `integrator.md:3/12/51/66/101`、`adjudicator.md:51`、`landing-verifier.md:45`、`planner.md:109`、`review-moderator.md:64`、`commands/build-feature.md:32/33`、`commands/triage-change.md:36`、`references/dev-loop.md:104/105`、`references/dev-loop-operating-system.md:29/30`、`references/escalation-playbook.md:13`、`skills/kidpal-pod-launch-prep/SKILL.md:21` | 不改：这些是角色边界与合并门边界，发布动作归主线程，边界仍逐字成立。`dev-loop.md:105` 的「不 push、不建/更 PR、不 auto-merge remote」限定主语是 merge executor，本批脚本不是 merge executor |
| `docs/notes/archify/**` 生成图、`delivery/archive/**`、`docs/archive/**`、`docs/debt/**` 历史条目 | 不改：历史快照 / 归档 append-only / 记录库 |
| `CLAUDE.md` | 不改：无该规则文本，经 `@AGENTS.md` 导入传导 |
| `.claude/scripts/merge-driver.mjs:590`（不 push） | 不改：dev-loop agent 不 push 的理由注释（`agent 不 push、origin 陈旧`），仍是 T2 改动文件内的既有注释，与本批「主线程发布动作」不冲突 |
| `docs/contracts/**` | 零命中（契约面零改动） |

对应验证：§5 V4 的四处文本 grep 与旧句退场断言。

目标文件：`AGENTS.md`（改）· `docs/DEV-GOVERNANCE.md`（改）· `.claude/references/arch-review-protocol.md`（改）

原条件项「`gh pr merge` 拦截」已按 human 裁定 OQ1=(b) 删除：本批不加钩子判据、不进 manifest；裁定依据与残余见 §4 OQ1 与 §6。

## 3. 影响面 sweep 分类（ARQ-096 义务，UNCLASSIFIED=0）

| 面 | 文件 | 类别 | 说明 |
|---|---|---|---|
| G1 入口与指令根 | `AGENTS.md`、`docs/DEV-GOVERNANCE.md` | G1（改） | 铁律文本收窄，T4；human 已裁边界（脚本开 PR、人点 merge），判官可判须 Track A 则只升不降 |
| G2 角色与流程定义 | `.claude/skills/merge-to-main/SKILL.md` | G2（改） | 增发布/回同步段 + 边界句，T3；消费方 = 主线程 |
| G2 角色与流程定义 | `.claude/references/arch-review-protocol.md` | G2（改） | Track B 步骤 7 与前端对齐注的远端口径，T4 |
| G3 强制执行层 | `.claude/scripts/release-pr.mjs` | G3（新增） | 发布/回同步单一入口，T1；消费方 = merge-to-main skill 与主线程 |
| G3 强制执行层 | `.claude/scripts/merge-driver.mjs` | G3（改） | 批末 handoff 一行，T2；消费方 = 运行 driver 的主线程/planner 读数 |
| G3 未触 | `.claude/hooks/deny-main-push.mjs` | 未触（human 裁不拦 gh pr merge） | OQ1=(b)：不加 `gh pr merge` 拦截；现状（实测放行）即目标形态；残余记 §6 |
| G3 未触 | `.claude/settings.json`、`.claude/scope-map.json`、`.claude/verify-gates.json` | 未触 | hook 文件已注册（`settings.json` PreToolUse Bash 第 1 条），无需新增注册；新脚本走既有 `Bash(node .claude/scripts/*)` allow；scope-map 无脚本白名单语义 |
| G2 未触 | `.claude/agents/**`、`.claude/commands/**`、`.claude/rules/**`、其余 references | 未触 | 角色边界（不 push / 永不 merge 进 main / 不许直推 main）仍逐字成立；发布动作归主线程，不改变任何角色权限 |
| 契约面 | `docs/contracts/**` | 未触 | 全仓固定串扫描零命中；契约包三节不适用 |
| 证据面 | `delivery/review/261025-gov-review-release-pr-publish/**` | G4（新增） | 本批制品（PROPOSAL / 后续 summary / verdict / retro） |
| 机器再生面 | `.codebase-memory/**` | 未触 | indexer 自刷新，按约定不追交、不 stage |
| 历史/派生 | `docs/notes/archify/**`、`delivery/archive/**`、`docs/archive/**`、`docs/debt/**` | 未触 | 历史快照、归档 append-only、记录库（T4 表已逐条列出） |

**【硬项 · sweep 透镜义务】新断言 pattern 预跑（2026-10-25 实测）**：

- 本批不新增钩子断言（OQ1=(b)：不拦 `gh pr merge`）。为便于核验「现状即目标形态」，合成 stdin 实测保留在 §5 V2 第 4、5 条（预期 exit 0）；仓内活文本 `grep -rn -F "gh pr merge"` 实测 **0 命中**（2026-10-25），即便日后重启该议题也无既有文本冲突。
- 断言模式 = 脚本预检谓词（main worktree / 清洁树 / main 与 origin/main 关系）。对当前仓态预跑实测：`git rev-parse main origin/main` 双 ref 同点 `fa60dd2f416811a3babfaa4fe080529a26e1d7de`（关系 = equal，将打印 `nothing to publish`）；主 worktree `git -C C:/AI/KidPal status --short` 实测含 `.codebase-memory/artifact.json`、`.codebase-memory/graph.db.zst` 与 `delivery/review/INDEX.md`——前两者按路径豁免放行（不限状态码），`INDEX.md` 会被拒绝并点名，属正确 fail-closed 形态（非本批缺陷）。新增的 push/gh argv 白名单断言只作用于脚本自检的 fake runner、不扫描仓内文本（F1 口径）。
- 发布自动化 census：起草前 `release-pr.mjs` 不存在（实测 ABSENT），仓内 `release/|gh pr create|pr create` 唯一命中一句 workstream 叙述（见 T1 证据）。落地后消费方 = `merge-to-main` skill 的 description、约束与步骤 7/8（四处提法）+ `AGENTS.md` 与 `docs/DEV-GOVERNANCE.md` 各一处 + `arch-review-protocol.md` 两处（步骤 7 与前端对齐注）。

## 4. human 取舍点（OQ）

> **裁定状态（2026-10-25）**：四项均已由 human 在立项确认点裁定；下方保留起草期的选项与推荐位以便追溯，**裁定行覆盖推荐位**。

### OQ1 · 「人点 merge」要不要机械拦截？拦到什么程度？

**裁定（2026-10-25，human）：(b) 不拦，只留文档约定。** 依据（记录）：human 在确认点选定不加机器面，接受「PR 合并靠文档约定」的残余；不引入钩子复杂度。处置：原 T5 全节与 `landing-manifest` 对应行已删；§3 钩子行改「未触」；§5 V2 第 4、5 条预期改为 exit 0（现状即目标形态）；§6 增该残余的留根。

- **(a) 只拦 `gh pr merge`（起草期推荐位，未采纳）**：在 `deny-main-push.mjs` 增判据与自检用例（原 T5，已按裁定删除）。代价 = 钩子复杂度上升、需随 gh 参数形态维护；收益 = 把最顺手的自合通道关掉。残余：`gh api ... /merge`、`curl`、网页仍不在覆盖。
- **(b) 不拦，只保留文档约定（已裁：选此项）**：代价 = 唯一人工判断点没有机械牙，任何会话都能一条命令自合；收益 = 钩子零改动。
- **(c) (a) 之外再覆盖 `gh api` 合并端点**：覆盖更宽，但命令行形态多（REST/GraphQL、`-X PUT`、路径与变量展开），启发式误伤面与维护成本明显上升；对单人 public 仓库收益边际。

**起草期推荐 (a)**：与既有 deny-main-push 的设计哲学（抬高门槛、留残余账、不假称沙箱）一致；**该推荐位已被裁定 (b) 覆盖**。

### OQ2 · 脚本调用面：谁可以跑 `release-pr.mjs`？

**裁定（2026-10-25，human）：(a) 任何会话可跑 + 机械前提（维持现方案）。** 依据（记录）：调用者身份不可机判（harness 不向 hook 暴露会话身份），机械判定落在执行前提；真正的门在 PR 合并（human），发布本身非破坏、可撤销。

- **(a) 任何会话可跑，机械判定执行前提（已裁：选此项）**：脚本自身要求「main worktree 且 HEAD=main + 清洁树（memory 两件豁免）+ `gh auth status` 通过 + main/origin-main 关系可判定」。调用者身份**不可机判**：harness 不向 hook 暴露会话身份，`settings.json` 又已全局 allow `Bash(node .claude/scripts/*)`，写文档约定只是自觉。
- **(b) hook 环境令牌（如 `RELEASE_PUBLISH_OK=1`）**：同一会话可在命令里自设变量，假牙，不建议。
- **(c) 收窄 settings allow**：`permissions.deny` 是绝对拒绝，会把主线程一起打死，做不到「只给主线程」。

**起草期推荐 (a)**：真正的门在 PR 合并（human）；`--publish` 本身非破坏、可撤销（删分支），且失败面被预检收窄。机械判定用在**执行前提**上，不用在身份上，避免造一道可绕的假门。

### OQ3 · gov 回同步是否并入脚本？

**裁定（2026-10-25，human）：(b) `--sync` 只做本地 main ff，gov 回同步留 T3 的 skill 段（维持现方案）。** 依据（记录）：脚本单责、失败语义干净；gov worktree 常有在途批次，`--upsert` 该跑哪几行需要判断。

- **(a) `--sync` 全包**：本地 main ff + gov merge + `.codebase-memory` 两件定点还原 + `debt-render` + `changes-index --upsert`。一次命令到底，但脚本要跨 worktree 操作并承担渲染/记账语义，失败语义变复杂。
- **(b) `--sync` 只做本地 main ff，gov 回同步写进 skill 由主线程执行（已裁：选此项）**：脚本单责、失败语义干净；gov worktree 常有在途批次，`--upsert` 该跑哪几行需要判断（记忆条目「--upsert 要跑在落地 commit 之前」两次踩过）。
- **(c) 全写 skill，脚本只管 publish**：脚本最小，但主线程仍要手做本地 ff，重复劳动。

**起草期推荐 (b)，与裁定一致**：主线程仍要点几步，但都是既有 skill 步骤。

### OQ4 · 远端存量 14 条 `release/*` 分支要不要清理？

**裁定（2026-10-25，human）：(a) 不清理（维持现方案）。** 依据（记录）：`delete_branch_on_merge=true` 已覆盖新 PR（PR #16 起实测全删），存量只减不增；不给常驻脚本远端删除能力。

- **(a) 不清理（已裁：选此项）**：新 PR 已被 `delete_branch_on_merge=true` 覆盖（PR #16 起实测全删），存量 14 条是历史快照；删除分支是远端破坏性动作，不值得给常驻脚本这个能力。
- **(b) 脚本加 `--cleanup`**：每次仍需 human 授权，收益低、误删 open PR 分支的风险面大。
- **(c) human 一次性手工清理**：想要干净列表时做一次（可选，命令不入脚本、不写进 skill，避免 agent 顺手跑）。

**起草期推荐 (a)，与裁定一致**，并把 (c) 留作日后 human 自选动作。

## 5. 验证集（landing 前机械前置 + landing 后核验）

> 每条标注评估时点。所有命令在仓根（`C:/AI/KidPal-gov`）运行；涉及远端读的命令只读，不产生 push/PR。自检类遵守真空绿规约：自检输出必须含自检名，且夹具输入非空、断言顺序与 §2 改法一致。

**V1 · `release-pr.mjs` 自检（post-commit；pre-commit 在落地工作树同跑，命令与预期相同）**

```bash
node .claude/scripts/release-pr.mjs --self-check
```

预期：stdout 逐条打印 `  PASS · <label>` 后一行 JSON，含 `"selfCheck":"release-pr"`、`"fail":0`、`"ok":true`、`"total"` ≥ 19；`grep -c "^  PASS · "` ≥ 19。夹具标签必须覆盖（缺任一 = 自检自身红；当前总数 19 = 原 15 + `publish-happy-path` + `sync-equal` / `sync-ff` / `sync-refused`）：

| 标签 | 断言内容 | 对应覆盖要求 |
|---|---|---|
| `relation-equal` | main == origin/main → 打印 `nothing to publish`、exit 0、零远端写 | — |
| `relation-ahead` | origin/main 是 main 祖先 → 继续 | — |
| `relation-behind` | main 是 origin/main 祖先 → 拒绝 exit 2 | — |
| `relation-diverged` | 互非祖先 → 拒绝 exit 2，提示交 human | **diverged 拒绝** |
| `dirty-allowed-only` | 仅 `.codebase-memory` 两件 → 放行 | — |
| `dirty-offender-named` | 混入普通文件 → 拒绝并点名该文件 | — |
| `wrong-worktree` | HEAD ≠ main → 拒绝 exit 2 | — |
| `name-free` | 无冲突 → `release/<YYMMDD>` | — |
| `name-collision-bump` | 同名远端分支 tip 不同 → `-2`，不 force | — |
| `name-same-sha-reuse` | 同名远端分支 tip == main tip → 复用、跳过 push | **重复跑幂等** |
| `name-used-by-merged-pr` | 分支已被自动删但 PR 用过该名 → 换 `-2` | — |
| `open-pr-same-tip-noop` | 同 tip 已有 open PR → 打印 URL、exit 0、不 push 不建 PR | **重复跑幂等** |
| `extract-changeids` | 从 `Merge <id> (merge-driver)` 行按序去重提取；非 merge 行忽略 | — |
| `push-plan-source-main` | fake runner 记录 push argv：源 token 恒 `main`、目标 token 以 `refs/heads/release/` 起头、参数集不含 `-f`／`--force`／`--force-with-lease`／`--delete`／`--mirror`／`--all`；推送 tip == 本地 main tip | **release tip == main tip / push argv 白名单（F1）** |
| `gh-argv-whitelist` | fake runner 记录全部 gh 调用，子命令集合 ⊆ `auth status`／`pr list`／`pr create`；构造 `['pr','merge','31']` 被拒（不执行）；happy path 上的集合相等断言见 `publish-happy-path` | **runGh 白名单（F1）** |
| `publish-happy-path` | create 阶段夹具（预检以 fake 结果打桩，不记录预检 gh 调用）：该阶段 push 恰一次（argv 合规）+ gh 调用恰一次 `pr create`，argv 相邻 flag 对为 `--base`→`main`、`--head`→`<实际分支名>`（argv 另含 `--title` / `--body-file`，不参与相邻性断言）；该阶段记录的 gh 调用集合与预期集合**相等**（不是 ⊆）；PR URL 被打印 | **开 PR 的 happy path（F10 末段 / N4 方案乙 / N5）** |
| `sync-equal` | main == origin/main → 打印 `already in sync`、不 merge、不 push | `--sync` 三态（N2） |
| `sync-ff` | 本地 main 是 origin/main 祖先 → fake runner 记录恰一次 `merge --ff-only origin/main` | `--sync` 三态（N2） |
| `sync-refused` | 本地 main 领先或互非祖先 → exit 2，不 merge、不 push | `--sync` 三态（N2） |

- 外部锚（F6 / F10）：`node .claude/scripts/release-pr.mjs --self-check | grep -c "^  PASS · relation-diverged"` = 1；同形 `push-plan-source-main` = 1、`open-pr-same-tip-noop` = 1、`publish-happy-path` = 1。

**V1b · 脚本源码形状（post-commit；只留正向锚，F1）**

- `grep -cF "refs/heads/release/" .claude/scripts/release-pr.mjs` ≥ 1（正向锚：release 目标前缀；仅源码形状，不作行为证据）。
- 语义边界：源码 grep 只证「该串在源码里」，**不证「能力不在」**；能力面由 V1 的 `push-plan-source-main`、`gh-argv-whitelist` 与 `publish-happy-path` 三个 fake-runner 夹具判。「开 PR」的正面证据由 `publish-happy-path` 承担（F10）；原 `grep -cF "gh pr create"` 正向锚按 F10 删除——改造后 gh 调用是 `['pr','create',…]` argv 形态，连续串未必出现，留着只会误红或退化成注释在场检查。原先的 `grep -cF "gh pr merge" = 0` 与 `grep -cF -- "--force" = 0` 两条按 F1 删除（与「自检断言源码不含该串」互斥，属真空绿反面形态）。

**V2 · 钩子判定（post-commit；pre-commit 在落地工作树同跑，命令与预期相同；不触网）**

1. `node .claude/hooks/deny-main-push.mjs --self-check` → JSON `"fail":0,"ok":true`，且 `"pass"` = 69（OQ1=(b)：钩子零改动，基线 69 不变）。
2. `printf '%s' '{"tool_input":{"command":"git push origin main:release/261102"}}' | node .claude/hooks/deny-main-push.mjs; echo $?` → `0`。
3. `printf '%s' '{"tool_input":{"command":"git push origin main"}}' | node .claude/hooks/deny-main-push.mjs; echo $?` → `2`，stderr 含 `targets main/master`。
4. `printf '%s' '{"tool_input":{"command":"gh pr merge 31 --squash"}}' | node .claude/hooks/deny-main-push.mjs; echo $?` → `0`（OQ1=(b) 已裁：不拦；此条固定「现状即目标形态」，残余见 §6）。
5. `printf '%s' '{"tool_input":{"command":"gh -R fandanpeng/KidPal pr merge 31"}}' | node .claude/hooks/deny-main-push.mjs; echo $?` → `0`（同上，不拦形态）。
6. `printf '%s' '{"tool_input":{"command":"gh pr view 31"}}' | node .claude/hooks/deny-main-push.mjs; echo $?` → `0`；`git commit -m "gh pr merge 31"` 同形 → `0`（引号内数据不误报）。

**V3 · merge-driver 回归（post-commit）**

- `node .claude/scripts/merge-driver.mjs --self-check-rebase-target` → `"ok":true`
- `node .claude/scripts/merge-driver.mjs --self-check-worktree-topology` → `"ok":true`
- `node .claude/scripts/merge-driver.mjs --self-check-merge-friction` → `"ok":true`，且 `grep -c "PASS · T9"` ≥ 1（handoff 断言在场；容纳 T9a/T9b 拆分）
- `grep -c "release-pr.mjs --publish" .claude/scripts/merge-driver.mjs` ≥ 1（正向锚：handoff 文本在场）

**V4 · 文本面收窄与新增段（post-commit；pre-commit 在落地编辑后值相同）**

- `grep -cF "release-pr.mjs --publish" AGENTS.md` = 1；`docs/DEV-GOVERNANCE.md` = 1；`.claude/references/arch-review-protocol.md` = 2（56、58 两行各一）。
- `grep -cF "PR 合并与其它 remote push 仍 human" AGENTS.md` = 1。
- `grep -cF "PR 合并与其它 remote 同步仍 human" docs/DEV-GOVERNANCE.md` = 1；`.claude/references/arch-review-protocol.md` = 1。
- 旧句退场：`grep -cF "例外升级，remote push 仍 human。" AGENTS.md` = 0；`grep -cF "本地合并自动，remote 同步仍 human" docs/DEV-GOVERNANCE.md` = 0；`grep -cF "remote push 仍 human；升级闸拦" .claude/references/arch-review-protocol.md` = 0；`grep -cF "自动落本地 main，remote 同步仍 human" .claude/references/arch-review-protocol.md` = 0。
- `.claude/skills/merge-to-main/SKILL.md`：`grep -cF "release-pr.mjs"` ≥ 4（description、约束、步骤 7、步骤 8 命令各一行）；`grep -cF "7. **发布（远端）**"` = 1；`grep -cF "8. **回同步（human 合并 PR 之后）**"` = 1；`grep -cF "不发 PR"` = 0。
- 钩子面零改动（OQ1=(b)）：`git diff origin/main -- .claude/hooks/deny-main-push.mjs` 输出为空（基线须为已回同步的 origin/main）；landing-fidelity-check 的「清单外 canonical 改动」advisory 亦不含 `.claude/hooks/**`。

**V5 · 环境前提（pre-commit 与 post-commit 各跑一次；外部状态断言，非本批行为）**

- `gh auth status` → exit 0。
- `gh api repos/:owner/:repo/branches/main/protection --jq '[.enforce_admins.enabled, .required_pull_request_reviews.required_approving_review_count, .allow_force_pushes.enabled] | join(",")'` → `true,0,false`（2026-10-25 基线，命令实测）。
- `gh api repos/:owner/:repo --jq .delete_branch_on_merge` → `true`。

环境红先立基线：若外部状态与基线不一致（例如分支保护被关），本批不据此判红，但须在 landing-summary 如实记账并停手问 human。

## 6. 留根清单

- **`gh pr merge` 无机械牙（OQ1=(b) 已裁）**：任何会话一条命令即可自合 PR；human 已知情并裁定只留文档约定。`gh api`／`curl`／网页合并通道同样不在覆盖，与 `deny-main-push.mjs` 既有「启发式门槛非沙箱」注记同口径。若日后重启该议题，交日落复盘登记 ARQ 评估钩子或 REST 端点（本批不扩）。
- **发布后本地 main 继续前进 → 分叉仍可能发生（F2 条件）**：`--publish` 预检只比较 `main` 与 `origin/main`，不阻止发布后继续合并 Pod。若 human 点合并前本地 main 又前进，合并结果的第二父是旧 tip，本地新 tip 与 origin/main 互非祖先，`resolveRebaseTarget` 仍会判 diverged。处置：再跑一次 `--publish` 生成新 release 分支与 PR 并先合并，或交 human 裁决；发布后尽量不再合并。
- **`release-pr.mjs` 的 push 在 `deny-main-push.mjs` 视野外（F8）**：钩子只看 Bash 命令串，主线程跑 `node …release-pr.mjs --publish` 时脚本内部发起的 push 完全在钩子盲区。现有防线三道：脚本内 push argv 白名单（§5 V1 的 `push-plan-source-main` 夹具）、`.claude/scripts/**` 属 G3（agent 不可写）、landing 人读 diff。**不要以为 `deny-main-push` 已对此兜底。**
- **推 `release/*` 即把该分支内容公开**（仓库 PUBLIC）：发布即公开分支内容，撤回 = human 删分支；脚本刻意不提供删分支能力。
- **存量 14 条 `release/*` 不清理**（OQ4=(a)）；`delete_branch_on_merge=true` 覆盖新 PR 已实测（PR #16 起全删）。若清理，由 human 一次性手工执行。
- **gov 回同步的渲染与记账**（OQ3=(b)）：`debt-render`、`changes-index --upsert` 仍由主线程执行，记忆条目「`--upsert` 要跑在落地 commit 之前」继续适用；若日后发现漏跑，另立 ARQ 或并入 `--sync`（OQ3=(a)）。
- **首次真实 `--publish` 是上线事件**：本批验证只到夹具与合成输入；首次真实推送须 human 在场，并在 landing-summary 记「待 human 首跑 + 结果」。
- **发布脚本的 handoff 只是提示**：merge-driver 只打印一行，不能强制发布发生；发布与否仍是主线程动作。
- **工作树不洁会 fail-closed**：主 worktree 当前实测有 `delivery/review/INDEX.md` 未提交改动，`--publish` 会拒绝并点名——这是正确行为，清理归主线程。
- **多机/并发**：另一台机器推 origin/main 时 `--sync` 走 ff；互非祖先时拒绝交 human，不自动 merge。
- **PR 标题自动摘要只保证机械可生成**：业务摘要由 `--title` 承担；不自动推断业务含义。
- **`release-pr.mjs` 未注册进 verify-gates**：自检为 landing 期与手动验证工具（与仓内其它脚本的 self-check 同形制），无 CI 常驻位。

## 7. 评审记录

（判官 append-only 区；主线程不预判结论。）

### R1 - 2026-10-25 - architect

verdict: BLOCK(F1..F9)

**判档（判官职权，只升不降）：主路径 Track B（全件制品，retro 必写），非轻路径瘦制品。** 依据 `gov-change.md`「轻路径判据（路径即判据）」第③条——本批 diff 命中 `.claude/scripts/**`（`release-pr.mjs` 新增、`merge-driver.mjs` 改）、`AGENTS.md`、`docs/DEV-GOVERNANCE.md`，命中任一即「恒 Track B，哪怕只改一个字符」，瘦制品五件通道不适用；旁证 `docs/DEV-GOVERNANCE.md:149`「改行为 / 门力度 / 角色边界 / agent 定义 / 强制执行层 → 全套 Track B」。PROPOSAL 头部沿用模板默认轨名「Track B 常驻轻路径」属照抄模板（模板原文即此标签），但字面含「轻路径」，落地期有被误读成瘦制品五件的风险，见 F7。

**Track A 段判定：不触发。** 依据 `gov-change.md`「Track A 段判据」——判据只看收敛度、不看触及面：① 方向已收敛（§4 四项 OQ 全部经 human 裁定，(b)/(a)/(b)/(a) 逐项落定）；② 本轮未发现未裁的方向级分叉（F2 的修法方向明确，不涉「该不该做 / 怎么做」的岔路）；③ 无人声明红队。G1 触面本身不构成 Track A 判据（`arch-review-protocol.md:75`「governance 类变更（含 self-def / review-loop agent defs）不再强制升级」）。PROPOSAL 头部把 G1 收窄写成「显式申报、不预占裁断」的形态正确，裁定结果 = 无需 Track A。

**结构字段核对（GREEN 前置）**：`landing-manifest` 围栏块在场，6 行、一行一个文件路径、无目录简写，且恰等于 T1–T4「目标文件」锚清单的并集（`release-pr.mjs` 新增；`merge-driver.mjs`、`merge-to-main/SKILL.md`、`AGENTS.md`、`docs/DEV-GOVERNANCE.md`、`arch-review-protocol.md` 改）；packet-id 行在场（Status 头部「packet-binding（FROZEN 生效）：packet-id = 本 PROPOSAL.md §2 改法集（T1–T4）+ §5 验证集」）。两项齐备，结构面无打回。

**四查结论**：① 意图保真——通过。human 原话逐字在册；「集成到 merge driver 与 merge skill」的落地形态为「driver 只打印 handoff、不塞 push」，偏离已在 §0 非目标与 T2 根因段写明理由（per-Pod 失败语义连坐），并覆盖 human 已裁边界「脚本开 PR、人点 merge」。② 纪律合规——有瑕疵：T3 步骤 7 与本批自身分工句不自洽（F3），T4-3 把 `--sync` 归给 human、与 T3 / OQ2=(a) 冲突（F4）。③ 撞车复核——基本通过，census 漏一条（F5）。④ 牵连面完整——通过：manifest ≡ 锚清单；T4 的九串扫描经独立复跑逐条吻合（除 F5 一条）。

**F1（BLOCKING）验证集与自检断言自相矛盾：三处「源码零字符串」grep 与「源码自扫描」断言互斥。**

- (a) §5 V3 末条 `grep -cE "git push|gh pr create" .claude/scripts/merge-driver.mjs` = 0 ↔ §2 T2「`runMergeFrictionSelfCheck` 增一条断言（编号 T9）：…`handoffLine(3)` … 不含 `git push`」。断言要在源码里写死 needle（形如 `!s.includes('git push')`），源码即含该串，grep 必不等于 0。自检输出格式已亲核为 `  PASS · <name>`（`merge-driver.mjs:1328` 逐字 `process.stdout.write(\`  ${r.pass ? 'PASS' : 'FAIL'} · ${r.name}...`），与 V3 的判法同源，冲突成立。
- (b) §5 V1b `grep -cF "gh pr merge" .claude/scripts/release-pr.mjs` = 0 ↔ §2 T1 第 10 条「并以自检断言源码无 `gh pr merge` 字样」及 §5 V1 标签 `no-merge-capability`「源码无 `gh pr merge` 字样」：要判「源码无该串」，needle 必须出现在源码。
- (c) §5 V1b `grep -cF -- "--force" .claude/scripts/release-pr.mjs` = 0 ↔ §2 T1 第 7 条「脚本内断言…命令不含 `--force`」及 §5 V1 标签 `push-plan-source-main`「无 `--force`」：自然写法 `!argv.includes('--force')` 即破 V1b。

三处同根：把「行为约束」写成「源码字符串零命中」。二者只能靠刻意拼串（`'gh pr ' + 'merge'`、`'--for' + 'ce'`）同时满足，而拼串后的断言恰好什么行为都证不了——是「真空绿」的反面形态：自检能被满足，但满足方式本身无意义，且对下一个维护者是陷阱。**修法方向**：择一收口。推荐删掉「源码自扫描」类断言，改判行为面（push 计划 argv 白名单：源恒 `main`、目标前缀 `refs/heads/release/`、无任何 `-f` / `--force` 起头的 token；`gh` 调用的子命令白名单仅 `auth status` / `pr create` / `pr list`），grep 只留正向锚（`refs/heads/release/`）。若保留 grep 侧，须在 §5 注明其语义边界（只证串不在源码，不证能力不在）。

**F2（SHOULD）§0 目标效果过度承诺，且缺留根。**

证据：§0 逐字「按 PR merge commit 的父提交结构，本地 main 恒为该 merge commit 的祖先，human 合并后 `git merge --ff-only origin/main` 恒成立，…这一形态不再发生」。结构断言本身成立（亲核逐字：`fa60dd2f… 90aa6af5… c349d4f1… Merge pull request #32 from fandanpeng:release/261025-2`；推完时本地 main = 第二父 `c349d4f1`，故为其祖先）。但「恒成立」只对「发布后本地 main 不再前进」成立：若 `--publish` 之后、human 点合并之前本地 main 又合并了新 Pod（`--publish` 预检只比 `main` ↔ `origin/main`，不阻止继续合并），human 合并后 origin/main = merge(旧 origin/main, 旧 main tip)，而本地 main 已含旧 tip 之外的提交 ⇒ 两侧互非祖先，`resolveRebaseTarget` 仍在 `merge-driver.mjs:631`（逐字 `return { ok: false, reason: 'diverged' };`）返回 diverged，今晨的分叉形态原样复发。**修法方向**：(i) §0 把「恒成立」限定为「以发布时的 main tip 为基点；发布后本地 main 若继续前进，须再发一次」；(ii) §6 增留根写明该残余与处置（再跑 `--publish` 覆盖新提交并合并新 PR，或交 human）；(iii) 可选：幂等闸命中「同 tip open PR」时打印一句提醒。

**F3（SHOULD）T3 步骤 7 首句主体错述：写成「human 合并本地 main 之后」。**

证据：§2 T3 改法第 3 段拟落文本逐字「7. **发布（远端）** — 在 human 合并本地 main 之后、于主 worktree（本仓 `C:/AI/KidPal`）分支 `main` 上运行 `node .claude/scripts/release-pr.mjs --publish`。」但现行模型里**本地 main 的合并由主线程自动落**：`AGENTS.md:14`「其余（评审/实现/验证/落地核验/合并本地 main）全部 agent + 机械闸自动」、`docs/DEV-GOVERNANCE.md:214`「本地合并自动，remote 同步仍 human」、`arch-review-protocol.md:56` 步骤 7「放行 → 自动合并本地 main」。human 的动作是**在 GitHub 网页合并 PR**——步骤 8 的括注「回同步（human 合并 PR 之后）」写对了，反衬步骤 7 这句错。**修法方向**：改为「在本地 main 合并（前置升级闸放行后由主线程自动落）之后」，与既有 canonical 同口径。

**F4（SHOULD）T4-3 替换句把 `--sync` 归给 human，与 T3 及 OQ2 冲突。**

证据：§2 T4 第 3 条拟落文本逐字「…PR 合并与其它 push 仍 human；human 在网页合并 PR 后跑 `node .claude/scripts/release-pr.mjs --sync` 追平本地 main）；升级闸拦 → 升级人。」而 T3 步骤 8 与 §0「再用一条命令把本地 main 追平」都放在主线程动作链上，OQ2 裁定 (a) 为「任何会话可跑 + 机械前提」，`arch-review-protocol.md:56` 步骤 7 的主体也是主线程。落地后 canonical 会出现「human 跑 --sync」与「主线程跑 --sync」两种口径。**修法方向**：统一为「主线程在 human 网页合并 PR 后跑 `--sync` 追平本地 main」——同一句里 `--publish` 已写「主线程跑」，主语保持一致。

**F5（NIT）T4 census 漏一条活文本命中。**

证据：§2 T4「复扫模式完备性」表声称逐条列出实测命中与处置；判官以同一命令集独立复跑，另有未入表的一条：`grep -n -F "agent 不 push" .claude/scripts/merge-driver.mjs` → 第 590 行，原文逐字为「根因：硬编码 `git rebase origin/main` 在本地工作流（agent 不 push、origin 陈旧）下恒撞过期基线。」（命中模式「不 push」）。该文件本批要改（T2），句意（dev-loop agent 不 push）与本批（主线程发布动作）不冲突。**修法方向**：表内补一行「`merge-driver.mjs:590`（不 push）| 不改：dev-loop agent 不 push 的理由注释，仍然成立」。

**F6（NIT）V1 外部可判性偏弱；V3 一条计数过紧。**

- V1 对外只落到 `grep -c "^  PASS · "` ≥ 15 与 JSON `total` ≥ 15——任意 15 条断言即可满足，表内 15 个标签是否在场没有外部机械判据（「夹具标签必须覆盖（缺任一 = 自检自身红）」是脚本内自证的义务，外部不可验）。**修法方向**：补 2–3 条关键标签的外部锚，例如 `grep -c "^  PASS · relation-diverged"` = 1、`grep -c "^  PASS · push-plan-source-main"` = 1、`grep -c "^  PASS · open-pr-same-tip-noop"` = 1。
- §5 V3 `grep -c "PASS · T9"` = 1 与「增一条断言（编号 T9）」绑定；若落地按仓内既有风格拆成 T9a / T9b（现有多数断言就是带后缀的），计数 = 2 即误红。**修法方向**：改为 ≥ 1。

**F7（NIT）判档标注建议。** 见本记录首段：判档结论 = 主路径。建议 Status 行显式补「主路径（全件制品，retro 必写）」，免落地期按模板默认标签误走瘦制品五件。

**F8（FYI）§6 缺「钩子视野外的 push 通道」留根。**

证据：`deny-main-push.mjs` 的判定对象是 Bash **命令串**（判官实测：`git push origin main:release/261102` → exit 0；`git push origin main` → exit 2，stderr 含 `targets main/master`）。主线程跑 `node .claude/scripts/release-pr.mjs --publish` 时，钩子只看到 `node …`，脚本内部发起的 push 完全在钩子盲区。现有防线 = 脚本内断言 + `.claude/scripts/**` 属 G3 写权（agent 不可写）+ §5 V1b / 自检。**修法方向**：§6 增一条留根写明盲区与三道防线，避免日后误以为「已有 deny-main-push 兜底」。

**F9（NIT）豁免口径需写明按路径计；一个出口码未定。**

- §2 T1 预检第 2 条写「仅允许 `.codebase-memory/artifact.json` 与 `.codebase-memory/graph.db.zst` 两条…其余任一文件（含状态码 `D`）在场即拒绝」；§3 预跑段称主 worktree「前两者按豁免放行」。判官实测主 worktree 当前状态三行为 ` M .codebase-memory/artifact.json`、` D .codebase-memory/graph.db.zst`、` M delivery/review/INDEX.md`——第二件是**删除态 `D`**。按「按路径豁免」口径放行无碍；若实现成「只允许两条 ` M `」的形态，当前主 worktree 会被拒并点名一个「本该豁免」的文件。**修法方向**：第 2 条补一句「豁免按路径计，不限状态码」。
- 第 9 条出口码未覆盖「`git fetch origin main` 失败 / 网络不可用」应归 2（预检拒绝）还是 1（执行失败）。**修法方向**：并入第 9 条一句。

**亲核留痕（2026-10-25，仓根 `C:/AI/KidPal-gov`）**：

| 断言 | 实测命令 | 结果 |
|---|---|---|
| 远端地址 | `git remote -v` | `origin https://github.com/fandanpeng/KidPal.git (fetch/push)`，与 §2 T1 一致 |
| 双 ref 同点 | `git rev-parse main origin/main` | 双 `fa60dd2f416811a3babfaa4fe080529a26e1d7de`，与 §3 预跑段一致 |
| PR #32 merge 父结构 | `git log -1 --format='%H %P %s' fa60dd2f…` | `fa60dd2f… 90aa6af5… c349d4f1… Merge pull request #32 from fandanpeng:release/261025-2`，与 T1 证据逐字一致 |
| PR #31 数据点 | `gh pr view 31 --json headRefOid,headRefName,mergeCommit` | `{"h":"173c2a12c5e1b0c7eb9050e2ad6dd3ac875a7446","m":"90aa6af5…","n":"release/261025"}`，与 T1 证据一致 |
| 分支保护 | `gh api repos/:owner/:repo/branches/main/protection` | `"required_approving_review_count":0`、`"enforce_admins":{"enabled":true}`、`"allow_force_pushes":{"enabled":false}`、`"allow_deletions":{"enabled":false}`，且响应无 `required_status_checks`，与 T1 一致 |
| 仓库合并方式 | `gh api repos/:owner/:repo --jq '{...}'` | `{"allow_merge_commit":true,"allow_rebase_merge":false,"allow_squash_merge":false,"default_branch":"main","delete_branch_on_merge":true,"visibility":"public"}`，与 T1 一致 |
| 存量 release 分支 | `git ls-remote --heads origin "release/*"` | 14 行，末条 `903dc5cf… refs/heads/release/261019-3`，与 T1 的 14 条映射逐条吻合 |
| 按日 PR 计数 | `gh pr list --state merged --limit 100` | #17–#28 全在 2026-10-23、#29/#30 在 10-24、#31/#32 在 10-25，与 T1 一致 |
| 钩子自检基线 | `node .claude/hooks/deny-main-push.mjs --self-check` | `{"selfCheck":"deny-main-push","pass":69,"fail":0,"total":69,"ok":true}`，与 §5 V2 第 1 条的 69 一致 |
| 钩子合成输入 | 五条 `printf … \| node …deny-main-push.mjs` | `main:release/261102`→0、`HEAD:release/261102`→0、`main:refs/heads/release/261102`→0、`git push origin main`→2（stderr 含 `targets main/master`）、`gh pr merge 31 --squash`→0，与 T1 证据 / §5 V2 一致 |
| 新脚本不存在 | `ls .claude/scripts/ \| grep -i release` | 空（ABSENT），与 §3 一致 |
| merge-driver 行号断言 | `sed -n` / `grep -n` 逐段复核 | `resolveRebaseTarget` 起于 597、`diverged` 返回在 631、fail-closed 置 blocked 在 1422–1426、单 Pod `processPod` / `process.exit` 在 1669–1670、`SUMMARY` / `exit` 在 1682–1683、`runMergeFrictionSelfCheck` 起于 1111（现有 T1/T2/T3/T4/T6/T7/T8，T5 空号，T9 不撞号）——与 §2 T1/T2 逐条一致 |
| driver 无发布面（起草期基线） | `grep -n "git push\|release/\|release-pr" .claude/scripts/merge-driver.mjs` | 零命中，与 T2 证据一致 |
| 四句原文引用 | `grep -n` 各固定串 | `AGENTS.md:14`、`docs/DEV-GOVERNANCE.md:214`、`arch-review-protocol.md:56`、`:58`、`SKILL.md:3`、`:26` 逐字与 §2 T3/T4 引用一致 |
| 九串扫描复跑 | `grep -rn -F <pattern> AGENTS.md CLAUDE.md docs .claude`（排除 archive / showme / debt） | 命中与 T4 表一致，另有一条未入表（F5）；`gh pr` 与 `release/` 两串零命中，与 §3 硬项段一致 |
| 自动化 census | `grep -rIn -E "release/\|gh pr create\|pr create" .claude docs delivery`（排除本批目录） | 唯一命中 `delivery/workstreams/device-memory-overhaul/progress.md:129`，与 T1 证据一致 |
| 主 worktree 脏态 | `git -C C:/AI/KidPal status --short` | ` M .codebase-memory/artifact.json` / ` D .codebase-memory/graph.db.zst` / ` M delivery/review/INDEX.md`（见 F9） |
| description 预算 | 非空白字符计数 + `node .claude/scripts/lint-descriptions.mjs` | 现 260 → 拟 279（阈 320；该 lint 只扫 agents，现为 `scanned 16 … flagged 0`），T3 改 description 无超长风险 |
| 节号跳号 | `docs/templates/PROPOSAL.template.md` | §0 → §2 跳号系模板既有形制（原文「节号保持 `2` 不变（机械门定位锚）」），非本批缺陷 |
| OQ1 删除痕迹 | `grep -n "T5\|拦截\|deny-main-push" PROPOSAL.md` | 指向已删 T5 的悬空改法为零；T5 仅出现在 §4 OQ1 的删除记录里（属应有留痕）；manifest 无钩子路径；§3 钩子行「未触」；§5 V2 第 4/5 条预期 exit 0——与裁定处置四条逐一对上 |

**结论**：BLOCK。F1 为 BLOCKING（验证集与改法互斥，若不改，落地期会红在 landing-verifier 手里），F2–F4 为 SHOULD（canonical 落地文本与现行模型 / 本批自身分工句不自洽），F5–F9 为 NIT / FYI，可同轮并回。修订后按 R2 纪律只复核 F1–F9 的到位性与修订是否引入新矛盾，不做全量重核。判官未发现需升级 human 的方向级问题：判档（主路径 Track B）与 Track A 段判定（不触发）均已在上面给出依据，无「影响目标效果」级打回项。

### R2 - 2026-10-25 - architect

verdict: BLOCK(F10)

**R2 纪律**：只核前轮 F1–F9 的修订到位性、以及与修订相伴的新矛盾；不做全量重核。判档维持 **主路径 Track B（全件制品，retro 必写）**；Track A 段仍**不触发**（本轮未发现方向级缺口，判据同 R1）。

**逐条到位性（亲核，逐条对修订后文本复核）**

- **F1（BLOCKING）到位。** 三处互斥已清零：T1 第 7 条改为纯函数 `planPush` 生成 argv + 可注入 runner，白名单断言（源 token 恒 `main`、目标以 `refs/heads/release/` 起头、不含 `-f`/`--force`/`--force-with-lease`/`--delete`/`--mirror`/`--all`）落在 fake runner 记录的 argv 上；T1 第 10 条改为 `runGh(args, runner)` + 子命令白名单，`['pr','merge','31']` 构造调用被拒；T2 的 T9 断言删除「不含 `git push`」子句，改为单行性 + 两个正向包含断言，并显式写「不用源码字符串自扫描」；§5 V1b 只剩两条正向锚并附语义边界说明，原 `grep -cF "gh pr merge" = 0` 与 `grep -cF -- "--force" = 0` 已删；V3 原 `grep -cE "git push|gh pr create" = 0` 一条已删。亲核复扫：§2/§5 内已无任何「源码零命中 ↔ 自扫描断言」配对（残留仅在本 §7 R1 历史记录里，append-only 不改）；需刻意拼串才能同时成立的断言之数 = 0。`PASS · <name>` 输出形态与 `merge-driver.mjs:1328` 一致的前提仍在。
- **F2（SHOULD）到位。** §0 已限定为「发布时的本地 main tip」+「在『发布后本地 main 不再前进』的前提下」+「若又前进须再发一次」；§6 新增对应留根（处置 = 再发一次 / 交 human）；T1 第 6 条幂等闸命中时补打印该条件提醒。
- **F3（SHOULD）到位。** T3 步骤 7 首句改为「在本地 main 合并（前置升级闸放行后由主线程自动落定）之后」——与 `AGENTS.md:14`「其余（…、合并本地 main）全部 agent + 机械闸自动」、`docs/DEV-GOVERNANCE.md:214`「本地合并自动，remote 同步仍 human」、`arch-review-protocol.md:56`「放行 → 自动合并本地 main」三处原文同口径，亲核一致。
- **F4（SHOULD）到位。** T4 第 3 条替换句已改为「主线程在 human 网页合并 PR 后跑 `node .claude/scripts/release-pr.mjs --sync` 追平本地 main」，与 T3 步骤 8、「主线程跑 --publish」及 OQ2=(a)（任何会话可跑）一致。
- **F5（NIT）到位。** T4 表新增 `| .claude/scripts/merge-driver.mjs:590（不 push）| 不改：… |`；亲核实测行号即 590（`grep -n -F "agent 不 push"` → `590:// 根因：硬编码 …`）。
- **F6（NIT）到位。** §5 V1 增外部锚三条（`relation-diverged` / `push-plan-source-main` / `open-pr-same-tip-noop` 各 = 1，标签名与表内逐字一致）；V3 的 `PASS · T9` 改 ≥ 1（容纳 T9a/T9b）。
- **F7（NIT）到位。** Status 头部增「判档：主路径 Track B（全件制品，retro 必写；非轻路径瘦制品五件）」，review 轨改「Track B 主路径」。
- **F8（FYI）到位。** §6 新增「`release-pr.mjs` 的 push 在 `deny-main-push.mjs` 视野外」留根与三道防线。其中「`.claude/scripts/**` 属 G3（agent 不可写）」一句亲核成立：`.claude/scope-map.json` 里全部 20 个注册 agent 的 `allow` 均无 `.claude/scripts/**`，architect 的 `deny` 明确列有该路径。
- **F9（NIT）到位。** T1 第 2 条改为「豁免按路径计、不限状态码（`M`、`D`、`??` 等同一路径形态一律放行）」；第 9 条补「`git fetch origin main` 失败或网络不可用 → exit 2（尚未产生任何远端写）」。
- **结构字段**：`landing-manifest` 仍 6 行、一行一文件路径，且仍 ≡ T1–T4 锚清单并集；packet-id 行在场。**无打回。**
- **修订未改动的计数断言复核**：V4 的 `release-pr.mjs --publish` = 2（protocol 56 / 58 各一）、`PR 合并与其它 remote 同步仍 human` = 1、四条旧句退场 = 0，与修订后拟落文本逐字对得上（T4-3 新增的 `--sync` 提及不改变这两条计数）；V4 的 skill 侧 `release-pr.mjs` ≥ 4 在拟落文本下为 4 行（description / 约束 / 步骤 7 / 步骤 8）✓。

**R2 新增 finding（1 条）**

**F10（SHOULD）「开 PR」这一核心行为仍无可证伪断言，唯一正向证据是源码串锚。**

证据：§5 V1 的 15 个夹具里，push 有 argv 级夹具（`push-plan-source-main`）、gh 侧只有集合约束（`gh-argv-whitelist`：记录的 gh 子命令集合 **⊆** 白名单）、`open-pr-same-tip-noop` 只断言「不 push 不建 PR」、`relation-ahead` 只写「origin/main 是 main 祖先 → 继续」且未规定断言内容。**没有任何一条夹具断言「happy path 上确有一次 PR 创建、且其 argv 为 `--base main --head <name>`」**——集合 ⊆ 是单向包含，脚本一次 gh 都不调也满足；因此若创建路径整体缺失或参数拼错，现验证集仍可全绿。唯一的正向证据是 §5 V1b 的 `grep -cF "gh pr create" ≥ 1`（343 行），而 T1 第 10 条已把所有 gh 调用改成 `runGh(args, runner)` 的 argv 形态，自然写法是 `['pr','create',…]` 数组成员——连续串 `gh pr create` 未必出现在源码里：该锚要么由头部注释满足（则证不了任何行为，正是 F1 要根除的同一形态，只是这次是正向锚）、要么落地时误红。

影响：本批目标效果的核心动作是「推分支 + 开 PR」，第二半没有可证伪的验收面。严重度按 rubric = SHOULD（真缺口，有 workaround，不改会留债）。

修法方向（两步，都很小）：(i) V1 增一条夹具 `publish-happy-path`（或把 `relation-ahead` 写实）——fake runner 记录恰为一次 push（argv 合规）+ 一次 gh `['pr','create','--base','main','--head',<name>]`，并把 `gh-argv-whitelist` 在该夹具上断言为**集合相等**而非 ⊆；(ii) 删或改写 V1b 的 `gh pr create` 正向锚（由上述夹具承载，或注明它只作 usage 注释在场检查、不作行为证据）。

**NIT / 补充（不阻塞）**

- **N1（口径一致性）**：§2 T1 证据段（57 行）「**结构性承重点**：…则本地 main 是 merge commit 的第二父，恒为其祖先；之后无论 base 如何前进，`git merge --ff-only origin/main` 都成立」仍是无条件句。§0 与 §6 已把口径限定为「发布后本地 main 不再前进」；建议此处补同一限定（一句括注即可），免得同一文档两处口径不一、读者各取一条都有据。
- **N2（`--sync` 侧零夹具）**：§5 全部验证项只覆盖 `--publish` 的预检与自检，`--sync` 没有夹具（「already in sync」「ff 成功」「分叉拒绝交 human」三态无一断言）。因 `--ff-only` 本身即安全闸、失败是响亮的，风险低；若要严谨，可在 `publish-happy-path` 之外顺手补 2–3 条 `sync-*` 标签。
- **N3（历史名冲突的取数面，可选）**：T1 第 5 条「冲突判定同时看历史 PR 的 `headRefName`」未写明 `gh pr list` 的 `--limit`；gh 默认 limit 30，而仓库现已有 32 条 PR。同日命名总落在最近 30 条内，实际风险低；写明 `--limit 100` 可把该不确定消掉。

**R2 亲核留痕（2026-10-25）**：

| 核对项 | 命令 / 方法 | 结果 |
|---|---|---|
| 三处互斥是否清零 | 对 §2/§5 全文 `grep -n` 检索「`grep -cE "git push\|gh pr create"`」「`grep -cF "gh pr merge"`」「`grep -cF -- "--force"`」「不含 `git push`」 | 仅命中 §7 R1 历史记录（append-only 不回改）与 V1b 的语义边界说明段；§2 改法正文零命中 ✓ |
| manifest 与锚并集 | `sed -n '11,18p'` | 6 行一文件一路径；仍 = T1(`release-pr.mjs`) + T2(`merge-driver.mjs`) + T3(`SKILL.md`) + T4(`AGENTS.md`/`DEV-GOVERNANCE.md`/`arch-review-protocol.md`) ✓ |
| packet-id 行 | 读 Status 头部 | 「packet-binding（FROZEN 生效）：packet-id = …§2 改法集（T1–T4）+ §5 验证集」在场 ✓ |
| F5 行号 | `grep -n -F "agent 不 push" .claude/scripts/merge-driver.mjs` | `590:// 根因：硬编码 …` = 表内所记行号 ✓ |
| F8 主张 | `node -e` 遍历 `.claude/scope-map.json` 全部 agent 的 allow | 无任何 agent 的 allow 含 `.claude/scripts/**`；architect deny 明确列有该路径 ✓ |
| F6 标签名一致 | 从 V1 表提取标签并与外部锚比 | 15 个标签（无重复），外部锚三条与表内名称逐字一致；`total ≥ 15` 与 15 条自洽 ✓ |
| F3/F4 与 canonical 同口径 | 三处原文对照 `AGENTS.md:14`、`docs/DEV-GOVERNANCE.md:214`、`arch-review-protocol.md:56/:58` | 拟落文本与三处现行模型一致，未引入新漂移 ✓ |
| 修订未回改 R1 | 读 §7 R1 段 | 原文完好，verdict 行仍为 `verdict: BLOCK(F1..F9)`（历史记录，不回改）✓ |

**结论**：**BLOCK(F10)**。F1–F9 全部到位、无一条需要重开；F10 为一条 SHOULD（「开 PR」缺可证伪夹具 + V1b 正向锚错配），修法两步、不动 §2 改法本体，下一轮据此即可判 GREEN。N1–N3 为不阻塞的补充建议。判官无升级 human 项。

### R3 - 2026-10-25 - architect

verdict: GREEN

**R3 范围**：只核 F10 与 N1–N3 的到位性、以及修订是否引入新矛盾；不做全量重核。判档维持 **主路径 Track B（全件制品，retro 必写）**；Track A 段仍**不触发**（无方向级缺口，判据同 R1/R2）。

**逐条到位性（亲核）**

- **F10（SHOULD）到位。** `publish-happy-path` 覆盖了我上轮要求的三个可证伪点，逐条对上：① 恰一次 push 且 argv 合规；② 恰一次 PR 创建，且以「相邻 flag 对」断言 `--base` → `main`、`--head` → 实际分支名；③ 该夹具上记录到的 gh 调用集合**相等**（不是 ⊆），并断言 PR URL 被打印。§5 V1 的 `gh-argv-whitelist` 行同步改为「happy path 上的集合相等断言见 `publish-happy-path`」，两条夹具不再各行其是（一处 ⊆ 能力约束、一处相等全集断言，337 行有显式交叉引用，不构成矛盾）。
- **F10 的第二半（正向锚）到位。** V1b 的 `grep -cF "gh pr create"` 已删（现只在 348 行的删除说明里出现），并写清删除理由与「开 PR 的正面证据由 `publish-happy-path` 承担」。复扫 §5 现存源码锚：V1b 只剩 `grep -cF "refs/heads/release/"` ≥ 1（T1 第 7 条强制该前缀，落地必然出现，且已标注「仅源码形状，不作行为证据」）；V3 的 `release-pr.mjs --publish` 锚绑 T2 逐字 handoff 文本、V4 为文本面断言——**§5 内已无依赖「行为未必产生的连续串」的行为证据**。
- **标签算式自洽。** 机器提取 V1 表 = 19 行、无重复；V1 头部「`total` ≥ 19」「`grep -c "^  PASS · "` ≥ 19」与算式「原 15 + `publish-happy-path` + `sync-equal`/`sync-ff`/`sync-refused` = 19」一致；外部锚四条（`relation-diverged`、`push-plan-source-main`、`open-pr-same-tip-noop`、`publish-happy-path`）与表内名称逐字一致。正文残留 `≥ 15` = 0（仅出现在 §7 R1/R2 历史记录里，append-only 不回改）。
- **N1 到位。** T1 证据段第 57 行已改为「推成恰好等于**发布时的**本地 main tip」，并补「限定条件同 §0：发布后本地 main 不再前进；若又前进须再发一次，见 §6」——与 §0、§6 三处同一口径。
- **N2 到位。** `sync-equal` / `sync-ff` / `sync-refused` 三态与 `--sync` 第 2 条的三分支（相等打印 `already in sync` / 祖先则 `merge --ff-only` / 否则 exit 2）逐条对应，并各自断言「不 merge、不 push」或「恰一次 merge」。
- **N3 到位。** T1 第 5 条改为 `gh pr list --state all --limit 100 --json headRefName`，并写明「显式 `--limit 100`，避免 gh 默认 30 条截断；现仓已有 32 条 PR」。
- **结构字段**：`landing-manifest` 仍 6 行、一行一文件路径，仍 ≡ §2 T1–T4 锚清单并集；packet-id 行在场。**无打回。**
- **未引入新矛盾（抽核）**：T2 / T3 / T4 正文未变（与 R2 判定的到位状态一致）；V4 的计数断言（`release-pr.mjs --publish` = 2、`PR 合并与其它 remote 同步仍 human` = 1、四条旧句退场 = 0、skill 侧 ≥ 4）与拟落文本仍逐字吻合；V2 钩子面零改动、`pass` = 69 未变；§3 的 F1 口径注（「argv 白名单断言只作用于 fake runner、不扫描仓内文本」）与 §5 改法一致。

**NIT / 补充（不阻塞，可在落地期顺手处理）**

- **N4（口径钉一句话）**：`publish-happy-path` 写「恰一次 gh 调用」，而 T1 第 3/5/6 条要求 happy path 上还要先跑 `gh auth status`、`gh pr list --state all`、`gh pr list --state open`——端到端跑下来记录的 gh 调用必然多于一次。两种自洽解法择一写明即可：改成「gh 调用集合 = `{auth status, pr list, pr create}`」（打桩口径下枚举预期集），或注明该夹具是「create 阶段夹具（预检打桩）」。方向已对（相等而非 ⊆、相邻 flag 对、URL 打印），任一读法都仍要对 create argv 做断言，故**不影响可证伪性成立**，不阻塞冻结。
- **N5（可选）**：`publish-happy-path` 的 argv 写成 `['pr','create','--base','main','--head',<实际分支名>]`，而 T1 第 8 条还要带 `--title` / `--body-file`；「相邻 flag 对断言」已把含义钉在相邻性上，但括号里的短 argv 仍可能被读成整段相等。建议在括号内补一句「（argv 另含 `--title` / `--body-file`，不参与相邻性断言）」。

**R3 亲核留痕（2026-10-25）**：

| 核对项 | 方法 | 结果 |
|---|---|---|
| V1 标签数与唯一性 | 从 §5 V1 表逐行提取标签名并去重 | 19 行、重复 0；与头部「≥ 19」及算式「15+1+3」自洽 ✓ |
| 正文残留 ≥ 15 | 全文 `grep -n "≥ 15"` | 仅 §7 R1（440 行）与 R2（526 行）历史记录，正文零残留 ✓ |
| V1b 锚清理 | 检索 `grep -cF "gh pr create"` / `grep -cF "gh pr merge"` / `grep -cE "git push` | 行为断言处零残留；仅 348 行的删除说明与 §7 历史记录提及 ✓ |
| manifest / packet-id | `sed -n '11,19p'`、读 Status 头部 | 6 行一文件一路径；仍 ≡ T1–T4 锚并集；packet-id 行在场 ✓ |
| N1 / N3 | 读 T1 证据段 57 行、T1 第 5 条 | 「发布时的」「限定条件同 §0」已补；`--limit 100` 已写明 ✓ |
| N2 | 比对 V1 的 sync-* 三条与 `--sync` 第 2 条 | 三态逐条对应 ✓ |
| 前轮记录完整 | 读 §7 | R1（`verdict: BLOCK(F1..F9)`）与 R2（`verdict: BLOCK(F10)`）原文完好，未回改 ✓ |

**结论**：**GREEN**。F1–F10 全部到位，N1–N3 到位；N4 / N5 为不阻塞的措辞建议。本批可按主路径 Track B 进入 FROZEN（全件制品四件 + retro，retro 不在轻路径豁免内）。判官无升级 human 项。

### R4 - 2026-10-25 - architect

verdict: GREEN

**R4 范围（极窄）**：只核 N4 / N5 两处修订是否消除两读、是否引入新矛盾；不重开 F1–F10 与 N1–N3，不做其它复核。

- **N4 到位（方案乙）。** `publish-happy-path` 行改为「create 阶段夹具（预检以 fake 结果打桩，不记录预检 gh 调用）：该阶段 push 恰一次（argv 合规）+ gh 调用恰一次 `pr create` …；该阶段记录的 gh 调用集合与预期集合**相等**（不是 ⊆）」。作用域词「该阶段」把断言限定在预检之后，「不记录预检 gh 调用」把打桩口径写明——原先与 T1 第 3/5/6 条（`gh auth status`、`pr list --state all`、`pr list --state open`）的字面冲突与两读一并消解，且「集合相等」现在有确定预期集（= `{pr create}`），可证伪性不降反升。
- **N5 到位。** 同一行括号内新增「argv 另含 `--title` / `--body-file`，不参与相邻性断言」，与 T1 第 8 条的实际 argv 形态一致，不再可能被读成整段相等。
- **未引入新矛盾（抽核）。** 该行「该阶段 push 恰一次」与 `push-plan-source-main`（push argv 白名单）分工不打架——前者断次数、后者断形状，记录面同源；`gh-argv-whitelist` 的 ⊆ 与 `publish-happy-path` 的相等仍各管一面，337 行的交叉引用未变；V1 表仍 19 行、标签名与外部锚四条均未变；V1b、V2–V5、§2 各 T、§3、§6 未见改动痕迹；文件行数维持 567（与 R3 结束时一致），佐证「除这两处外未改其它文字」的申报。结构字段：`landing-manifest` 仍 6 行、仍 ≡ T1–T4 锚清单并集；packet-id 行在场。
- **verdict 锚亲和核。** 以闸的单源 `VERDICT_STATUS_RE` 复跑全文，锚点序列 `BLOCK | BLOCK | BLOCK | BLOCK | GREEN | GREEN`，按 `readGovAnchors`「取最后命中」语义 = **GREEN**；本段以 `verdict: GREEN` 行与 GREEN 结论行收尾，且段内不含任何行首形态的非 GREEN token。

**结论**：GREEN —— N4 / N5 两处已消除两读、无新矛盾，本批判定维持 GREEN，可按主路径 Track B 进入 FROZEN（全件制品四件 + retro）。判官无升级 human 项。

