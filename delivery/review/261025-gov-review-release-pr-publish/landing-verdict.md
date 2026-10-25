# landing-verdict · 261025-gov-review-release-pr-publish

verdict: PASS

> 结论：**PASS + 7 ADVISORY（全部非阻断）**。落地实际 diff 与 FROZEN PROPOSAL §2 改法集（T1–T4）+ §5 验证集语义一致：改法落到位、无漏改、无超出 packet 的私货、敏感面（G1 指令根 / G3 强制层 / G2 skill 与 protocol）逐字吻合。merge 仍归 human。

- change-id：`261025-gov-review-release-pr-publish`
- packet-id：FROZEN PROPOSAL §2 改法集（T1–T4）+ §5 验证集
- 核验基准：`git diff HEAD -- <manifest 路径>`，HEAD = `24b3fb7d`（FROZEN PROPOSAL 提交）+ 未跟踪新文件 `.claude/scripts/release-pr.mjs`
- 判档：Track B 主路径（无 decision-packet → 设计侧门 N/A，见 §4）
- gitDegraded：false

```json
{
  "verdict": "PASS",
  "ok": true,
  "changeId": "261025-gov-review-release-pr-publish",
  "head": "24b3fb7d382084c5c0198d3a70278b8d3aacdb9e",
  "gitDegraded": false,
  "packetBinding": true,
  "filesChecked": 6,
  "advisoryCount": 7
}
```

## 1. 逐文件 diff 语义核对

| 文件 | structuralTouched | semanticMatch | expected（FROZEN §2） | actual（diff 实证） |
|---|---|---|---|---|
| `.claude/scripts/release-pr.mjs`（新，T1） | 是（新增 704 行） | ok | 头注 `owner=architect, approver=human (G3 script, see DEV-GOVERNANCE.md §6)`；`--publish`/`--sync`/`--dry-run`/`--self-check`/`--title`；预检四道（运行位置/清洁度按路径豁免/gh 可用/分支关系）；命名 `release/<YYMMDD>` `-2…-20`；幂等闸；`planPush`+`assertPushArgv` 白名单；`runGh` 子命令白名单；`gh pr create --base main --head`；出口码 0/1/2；自检 19 夹具 | 头注在 4 行；五子命令齐；`EXEMPT_DIRTY_PATHS` 按路径豁免（`classifyDirty`/`parsePorcelainPaths` 不限状态码）；`resolveBranchRelation` 四态 + `fetch` 失败归 exit 2；`pickBranchName` 复用/换后缀；`open-pr-same-tip` 命中打印 URL+条件提醒；`planPush`→`main:refs/heads/release/<name>`，`FORBIDDEN_PUSH_TOKENS`+`startsWith('--force')` 覆盖 force-with-lease；`planPrCreate` argv + `runGh` 白名单（`pr merge` 构造被拒）；出口码三态；自检 19 PASS（见 §3） |
| `.claude/scripts/merge-driver.mjs`（改，T2） | 是 | ok | 纯函数 `handoffLine(mergedCount)`（≤0 返 null）；批模式 SUMMARY 后打印；单 Pod `processPod` true 后打印；DRY_RUN 不打印；`runMergeFrictionSelfCheck` 增 T9；零 push/网络/gh | 新增 `handoffLine`（71–75 行，文本逐字合 FROZEN）；批模式 `if (!DRY_RUN && merged > 0) log(handoffLine(merged))`（1702 行）；单 Pod `if (ok && !DRY_RUN) log(handoffLine(1))`（1688 行）；T9a/T9b 断言（1333–1342 行）；diff 内无新增 push/网络/gh 调用 |
| `.claude/skills/merge-to-main/SKILL.md`（改，T3） | 是 | ok | description 整行替换 + 约束第一条替换 + 步骤 6 后追加第 7/8 条（三处逐字） | description 逐字吻合；约束第一条「**本 skill 永不 push**…」逐字吻合；步骤 7（发布）、8（回同步，含 gov 回同步命令块）逐字追加于步骤 6 之后、`## 约束` 之前 |
| `AGENTS.md`（改，T4-1） | 是（第 14 行整行） | ok | blockquote 行收窄为「远程发布 = 主线程跑 … --publish，PR 合并与其它 remote push 仍 human」 | 第 14 行逐字吻合；旧句「例外升级，remote push 仍 human。」= 0 |
| `docs/DEV-GOVERNANCE.md`（改，T4-2） | 是（第 214 行子句） | ok | 「本地合并自动，远程发布 = 主线程跑 … --publish…，PR 合并与其它 remote 同步仍 human」 | 第 214 行逐字吻合（grep 亲判）；旧句「本地合并自动，远程 同步仍 human」= 0 |
| `.claude/references/arch-review-protocol.md`（改，T4-3/T4-4） | 是（第 56、58 行） | ok | 56 行句尾收窄（唯一机器例外 + --sync 归主线程）；58 行子句收窄（随后发布走 --publish） | 第 56、58 行均逐字吻合；两处旧句退场 grep 均 = 0 |

## 2. packet-binding 一致

- summary `packet-id：` 字面行在场，内容 = 「FROZEN PROPOSAL §2 改法集（T1–T4）+ §5 验证集」· change-id 对得上。
- manifest（PROPOSAL 与 summary 双侧围栏块）均 6 行、一行一文件路径，恰等于 §2 各 T「目标文件」锚清单并集：T1 `release-pr.mjs`（新增）+ T2 `merge-driver.mjs` + T3 `SKILL.md` + T4 `AGENTS.md`/`DEV-GOVERNANCE.md`/`arch-review-protocol.md` = 6。**一致**。
- summary 骨架遵循 `docs/templates/architect-landing-summary.template.md`：标题行 + `packet-id：` 行 + §1 落地点↔改法表 + §2 验证证据 + §3 诚实注记 + 结尾 `landing-manifest` 围栏块，结构齐全，非手写变体。
- 本批未把任何「五件组件 / 条目清单」声明为绑定面（T2 单文件、T3 为文本追加），故该项 N/A。

## 3. 验证集复跑（§5 每条机械命令亲跑）

| 编号 | 命令 | 关键输出 | 与 summary 声称 | 结论 |
|---|---|---|---|---|
| V1 | `node .claude/scripts/release-pr.mjs --self-check` | 19 行 `  PASS · <label>`；`{"selfCheck":"release-pr","pass":19,"fail":0,"total":19,"ok":true}` | 一致 | 绿 |
| V1 外部锚 | `grep -c "^  PASS · <label>"` | `relation-diverged`=1、`push-plan-source-main`=1、`open-pr-same-tip-noop`=1、`publish-happy-path`=1；总 PASS 计数 19 | 一致 | 绿 |
| V1b | `grep -cF "refs/heads/release/" …release-pr.mjs` | `6`（≥1） | 一致 | 绿 |
| V2.1 | `node .claude/hooks/deny-main-push.mjs --self-check` | `{"selfCheck":"deny-main-push","pass":69,"fail":0,"total":69,"ok":true}` | 一致（69 不变，钩子零改动） | 绿 |
| V2.2–V2.6 | 六条合成 stdin | `main:release/261102`→0；`git push origin main`→2（stderr 含 `targets main/master`）；`gh pr merge 31 --squash`→0；`gh -R … pr merge 31`→0；`gh pr view 31`→0；`git commit -m "gh pr merge 31"`→0 | 一致（后四条即 OQ1=(b) 现状即目标形态） | 绿 |
| V3 | `--self-check-rebase-target` / `--self-check-worktree-topology` / `--self-check-merge-friction` | 7/7、9/9、22/22 均 ok:true；`grep -c "PASS · T9"` = 2（≥1） | 一致 | 绿 |
| V3 锚 | `grep -c "release-pr.mjs --publish" merge-driver.mjs` | `2`（≥1） | 一致 | 绿 |
| V4 | 四处文本 grep | `release-pr.mjs --publish`：AGENTS=1 / DEV-GOV=1 / protocol=2；`PR 合并与其它 remote push 仍 human` AGENTS=1；`PR 合并与其它 remote 同步仍 human` DEV-GOV=1 / protocol=1；四条旧句均 0 | 一致 | 绿 |
| V4 skill | `grep -cF … merge-to-main/SKILL.md` | `release-pr.mjs`=4、`7. **发布（远端）**`=1、`8. **回同步（human 合并 PR 之后）**`=1、`不发 PR`=0 | 一致 | 绿 |
| V4 钩子零改动 | `git diff origin/main -- .claude/hooks/deny-main-push.mjs` | 输出空；`git diff HEAD -- .claude/hooks/` 行数 0 | 一致 | 绿 |
| V5 | `gh auth status` / protection / repo | auth exit 0（账号 fandanpeng，scopes 含 repo）；`true,0,false`；`delete_branch_on_merge=true` | 一致（外部状态与 10-25 基线一致） | 绿 |

补充复跑（summary §3 第 8 条声明）：`route-regression-check` → `52 ok, 0 fail`；`lint-descriptions` → `scanned 16, flagged 0`，均与 summary 声称一致。live smoke：在 gov worktree 跑 `--publish --dry-run` → `wrong-worktree … current: gov/kidpal`、exit 2、工作树行数不变（零写）。

## 4. 结构门复跑

- `node .claude/scripts/landing-fidelity-check.mjs delivery/review/261025-gov-review-release-pr-publish` → **exit 0**，`"ok": true`，`"frozen": true`，`"packetBinding": true`，`proposalTargets == summaryTargets`（6/6），`unauthorized: []`、`claimedNotLanded: []`、`plannedNotLanded: []`、`gitDegraded: false`。
- 设计侧门 `proposal-consolidation-check.mjs`：本批为 Track B（无 decision-packet），**N/A**，不跑。
- `cross-surface-check.mjs`：本仓 `.claude/scripts/` 下**不存在**（`ls` 实测 No such file），PROPOSAL 全文对该串零引用（grep = 0）——按「无此门」处理，见 ADVISORY A6。

## 5. 真空绿规约（三乘客）

- **重放类**：V1 重放前无空输入路径；`--self-check` 内部 19 夹具均以非空构造（fake deps 提供 status/mergeLog/sha），输出含自检名 `release-pr` + JSON（`pass/fail/total/ok`）。未使用 `^1` 修订符类命令。**未发现真空绿**。
- **接线类**：新增自检名 `T9a`/`T9b` 实际出现在 `--self-check-merge-friction` 输出（`grep -c "PASS · T9"` = 2），非「跑了没跑」。`assert` 顺序在 `main()` 打印之前，无终端 exit 型自检挡住后续自检的情形。
- **环境红基线**：涉闸执行层（hook self-check 69）与基线一致；V5 外部状态与 10-25 基线一致，无需归因。

## 6. 破坏性自检（我自己的核验深度）

- 逐文件核了 manifest 全部 6 个文件的真实 diff，未只挑好看的文件核；704 行新脚本通读（纯函数面 + 编排面 + 自检面 + live deps + CLI 全读），非抽读。
- 敏感面逐字核：G1 指令根（AGENTS.md 第 14 行、DEV-GOVERNANCE.md 第 214 行）与 G2（SKILL.md description/约束/步骤 7-8、arch-review-protocol.md 56/58）均逐字比对 FROZEN 拟落文本，未夹带权限/写域扩张；G3 脚本面（release-pr.mjs、merge-driver.mjs）无私有后门、无越权 push 面。
- 未把 landing-summary 自述当通过：summary 声称的每条改动均在真实 diff 或亲跑命令输出中找到对应。
- 范围外质询：`git status --porcelain` 除 6 个 manifest 文件外，仅 `.codebase-memory` 两件（机器再生面，按约定不追交）与 `delivery/review/…/architect-landing-summary.md`（证据制品）——**无超出 manifest 的 canonical 改动**，无 scope creep。

## 7. ADVISORY（非阻断观察）

- **A1（断言强度）**：`publish-happy-path` 实现为「push 恰一次 + `pr create` 调用恰一次」，其中 `ghCreateCalls` 先按 `a[0]==='pr' && a[1]==='create'` 过滤再计长度；FROZEN V1 该行字面写「该阶段记录的 gh 调用**集合与预期集合相等**（不是 ⊆）」。当前实现未对 `calls.gh` 全体做集合相等断言。因 create 阶段唯一 gh 调用源是 `ghCreatePr`（fake 只记录 create），实效等价；留作日后若在 create 阶段引入其它 gh 调用时的潜在弱化点。**不影响可证伪性**（F10 的「一次都不调也满足」已被「恰一次 create + 恰一次 push」堵住）。
- **A2（执行顺序）**：`publishFlow` 先跑幂等闸（PROPOSAL 编号第 6 条）再跑分支命名（第 5 条），与 PROPOSAL 列表编号顺序相反。两个行为均在、幂等判定不依赖分支名，语义无损。
- **A3（占位符命名）**：FROZEN §2 T2 handoff 逐字块写 `${merged}`，落地代码用 `${mergedCount}`（函数签名即 `handoffLine(mergedCount)`）。渲染输出完全一致，属占位符命名而非行为差异。
- **A4（前提校验粒度）**：`gh auth status` 校验登录成功，未显式断言 token 含 `repo` scope（FROZEN T1 第 3 条括注「token 需含 repo scope」）。缺 scope 时会在 `gh pr create` 以 exit 1 暴露，非预检即拦。低风险。
- **A5（工具误报）**：`landing-fidelity-check` 的 `verbatimQuoteAdvisory.missing` 列出 `human 把关点 = …`（DEV-GOV:214）与 `merge 恒由主线程…`（protocol:58）两条新句「not-found-verbatim」——长行截获导致的假阴性；我已以 `grep -F` 亲判两串在对应行逐字在场（214 / 58 行）。同一 missing 列表中的旧 description、旧「永不 push」缺在是「替换后已消失」的应有形态。
- **A6（清单悬空引用）**：派遣清单第 4 项仍提及 `cross-surface-check.mjs`，而本仓该脚本不存在（复现确认），按「无此门」处理并以 `landing-fidelity-check.mjs` 作结构门替代。记录以提示清单文本未同步修订。
- **A7（不可独立复现项）**：summary §2「live 预检」声称在主 worktree（`C:/AI/KidPal`）跑 `--publish --dry-run` 命中 `delivery/review/INDEX.md` dirty 并 exit 2；我因 worktree 隔离规约未在 main worktree 内复跑，改以自检 `dirty-offender-named` 夹具（同代码路径：exit 2 + 点名 offender）佐证同一 fail-closed 行为。属证据覆盖差异，非保真缺口。

## 8. 诚实披露核对

summary §3 八条诚实注记与实际逐条一致：① 首次真实 `--publish` 未执行（本轮只到夹具/dry-run，属实）；② `gh pr merge` 无机械牙（OQ1=(b)，V2.4/V2.5 exit 0 实测）；③ push 在钩子视野外（客观）；④ 推 release/* 即公开（仓库 PUBLIC 属实）；⑤ 存量 14 条不清理（OQ4=(a)）；⑥ 主 worktree `INDEX.md` 未提交致 fail-closed（代码路径经夹具佐证）；⑦ gov 回同步未并入脚本（OQ3=(b)，`syncFlow` 仅 ff）；⑧ 未跑全量回归、`route-regression` 52/0、`lint-descriptions` 16/0（已复跑一致）。**未发现该记未记**。

## 9. 处置

绿：建议放行 merge（Track B 自动合并门 / human manual-merge 门仍归 human 与机械闸）。7 条 ADVISORY 均非阻断，A1/A4 可在日后维护时顺手收紧，不构成本批退回理由。verdict 状态行在场（行首 `verdict: PASS`），供 `changes-index.mjs` `deriveLanded` 推 landed。
