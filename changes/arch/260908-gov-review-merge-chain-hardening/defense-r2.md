# Defender 答辩 · R2 定向 · 260908-gov-review-merge-chain-hardening

> defender 对 challenger findings-r2 五条新 finding（N1–N5）+ N6 残留逐条答辩。全部行号亲核（main 实现面与 charter 基线 fae886a 等价）。立场不变：可机判坐实的缺口痛快 CONCEDE；对定性过重的部分 REBUT 须引原文。一处行号订正先行：N3 引 `pod-coordination.md:127`，逐字核对该命令实为 **:126**（「3. `GIT_EDITOR=true git rebase --continue`（免交互），循环至 rebase 完成；」；:127 是第 4 步 portfolio-reconcile 行）——按 charter §1「行号漂移先核再落」纪律订正，实体指控不受影响。

---

## N1 · CONCEDE（SHOULD）——F8 修向文案按字面假，属实；修向 = import 探针为主 + argv 零命中静态半句

亲核成立：lib 五处 `process.exit` 全数在案——`governance-gate-shared.mjs:199`（runIsFrozenSelfCheck 尾）、`:231`（runExtractTargetsSelfCheck）、`:261`（runComputeFrozenPinSelfCheck）、`:334`（runResolveOriginSelfCheck）、`:517`（runSection2AnchorSelfCheck），均在 run*SelfCheck 函数体内（self-check 终端式出口，import 不触）；`process.argv` 全文件零命中（半句成立）。我 R1 的 F8 修向写「不 `process.exit`（grep/AST 可机判）」按字面 grep 今天即假——与 F2 同病族（负断言被既有字面打破），引不出原文反驳，CONCEDE。

**最终文案（可机判，二选一中我选探针为主）**：
1. **import 探针夹具（主断言）**：`node -e "import('<lib 绝对路径>').then(m => { if (typeof m.resolveHumanApproval !== 'function') process.exit(2); }).catch(() => process.exit(3))"` → 须 exit 0。一个探针同时机证两件事：import 时进程存活（= 顶层零副作用、零 exit）+ 关键提取导出可调（= T6 提取到位）。比任何文字重划界更硬——探针测的是承重属性本身（S-3 失效链②的 import 安全性），不依赖措辞不再漂。
2. **静态半句（辅断言）**：`grep -c "process.argv" <lib>` = 0（实测零命中，成立且持续可判）。
3. 弃「不 process.exit」全称负断言；「process.exit 仅存在于 run*SelfCheck 函数体内」只作 PROPOSAL 注记性说明、**不作 grep 断言**（函数体归属需 AST，grep 不可判，写成断言就是下一个 F2）。

归属：批内（V2/F8 修向文案按上改写）。与 challenger 给的两候选（重划界 / 探针）相比我明确选探针侧，理由如上——重划界文案里「exit 仅在函数体内」半句不可机判，留它就留了下一个按字面假的断言。

## N2 · CONCEDE（COULD）——消费方 7 非 6，名册补 portfolio-reconcile

亲核成立：`portfolio-reconcile.mjs:34` 具名导入 `isProposalMdPath, normalizeSourceRef, resolveOriginRef`（此前双方 grep 均漏计），其 `--self-check` 入口 `:329` 在、可跑。修向：F1 修向的等价夹具名册由六改七，portfolio-reconcile `--self-check` 全量复跑零回退列入 V2 配套。归属：批内，一处计数。

## N3 · CONCEDE（SHOULD）——runbook 自带命令形态逃逸 ask，属实；GA-8 立场调整为「条件保留」，裁定建议 = 修 runbook 命令形态为首选

亲核成立（行号订正为 :126）：runbook §10a 第 3 步逐字命令以 `GIT_EDITOR=true ` 起头，settings ask 前缀 `Bash(git rebase:*)` 按命令串前缀匹配不命中——**非复合命令**的单条逃逸，砍在消解主循环最高频位（rebase --continue 逐轮执行）。这是 S-5 盲区族的新实例且性质更重：S-5 的复合命令绕道是「使用者写法不当」，这条是 **runbook 自己的 canonical 形态就绕过**——ask 的保护主张从 day 1 在主循环位不成立。引不出原文反驳，CONCEDE。

**GA-8 立场调整**：R1 我顶 ask 的核心论据是「每次消解多 1-2 次人交互 = 真人类检查点」。探针改写了成本收益计算的两侧：收益侧在最高频位实际为 **0**（逃逸），残缺面侧比我方申报的（复合命令 + child_process）多出一条 runbook 内建形态。立场由「顶 ask」调整为**「条件保留 ask」**：ask 可保留的前提是先把 runbook 命令形态修到与 ask 互洽；互洽修好后再谈摩擦是否可接受。

**裁定建议（三选一，A-4 域交裁）**——首选 **修 runbook 命令形态**：
- 具体修法：§10a 增一步一次性设置（或由 merge-driver 建 worktree 时代设）Pod worktree 本地 `git config core.editor true`，第 3 步改素形 `git rebase --continue`（以 `git rebase` 起头 → ask 前缀命中）。注意 `git -c core.editor=true rebase --continue` 形态**不解决**问题（前缀是 `git -c` 不命中），须用 config 落盘形态。修后消解主循环每轮真弹窗——这才是 GA-8「真正判断点」论证的自洽形态（人出现在冲突逐个消解的当下，而非仪式性在场）。
- 备选：若裁「逐轮弹窗摩擦不可接受」→ **收窄 ask 主张至 `Bash(git merge:*)` 一条** + rebase 面靠 T3 正文纪律 + hook 留根（诚实降档）。
- 不推荐：维持现状 + 留根——runbook 自带逃逸会让 ask 保护主张名不副实，违反本批自己的诚实边界纪律（architect §T4 两条诚实边界的精神）。
- 无论裁哪侧：A-4 回填时把「ask = 手动 Bash 路径 best-effort；复合命令、env 前缀、child_process 三类绕道在册」写进主张限定句。

归属：A-4 裁断 + 批内回填（runbook 修法在 T2 §10a 补条范围内，manifest 已含该文件）。

## N4 · CONCEDE（COULD）——齿名漂移一处，逐字订正

亲核成立：实际符号 = `test_a8_live_block_matches_makemeahanzi`（文件头注 :22、对照注 :172、def :678 三处一致；`makemeahanzi` 为数据集名），PROPOSAL §8 修订注记 1（:160）与 architect-notes §T7 表 #7（:117）均写作 `makemehanzi`（少一个 me）——按名 grep 清点必漏命中，本批管的正是指向精度，无辩。修向：两处符号逐字订正，一处即净。归属：批内。

## N5 · REBUT（「fail-open」定性）/ CONCEDE（未申报分叉 + 夹具缺口）——Step 1b 背书在场；三选一推荐**收窄 + Step 1b 背书补偿断言**

**亲核新证据（challenger 链条缺的一环）**：`approvalRefUnlock` 的调用点唯一在 merge-driver 入口 `:1368-1370`，守卫 `SINGLE_POD && HUMAN_APPROVAL_REF && !DRY_RUN`；解锁成功（ready-to-merge）后该 Pod 在**同一次 invocation** 内被收集进 ready 集并走 `processPod` → **Step 1b（:1132-1150）无条件再 spawn 全闸**（gateArgs 含 `--human-approval-ref`，:1137-1138），off-main / 契约面 / riskTier / provenance / blocked 任一违例即 BLOCK + `setPod blocked` + return false（:1145-1150）。**即：merge 级 fail-closed 不由 unlock 预核承载、由 Step 1b 承载**——unlock 核验集即便收窄为五要素，任何会被全闸拦下的 Pod 仍在 rebase/merge 之前被 Step 1b 拦下，端到端「没有全闸放行就没有 merge」不变式零变化。DRY_RUN 下 unlock 本就被 :1368 守卫跳过，不存在「解锁后跳过 Step 1b」的旁路；唯一残留瞬态 = 解锁与 Step 1b 之间 board 短暂显示 ready-to-merge（自纠于 :1148 回落 blocked）。「fail-open 方向、紧邻 canary 4 对偶面」的 **merge 安全定性过重，REBUT**——canary 4 守的是「核验失败 kept-blocked」（:925-929 catch 块），收窄形态下该字面与意图均不变。

**但 CONCEDE 两点**：① PROPOSAL T6（:97）「merge-driver T1 的 spawn 复用改为 import」确实隐含解锁核验收窄而**未申报**——「gate 判定语义零变更（纯结构重构）」承诺下这是一个未声明的行为差（瞬态状态机宽化），须显式裁；② V2 等价夹具（:137，五要素级）与 F8 排除项（changeId）均不覆盖解锁路径核验集问题，属实。

**三选一推荐：选项 2（收窄 + 补偿断言）**，理由对着 fail-closed 原则 / GA-4 / canary 对偶面逐条：
- **fail-closed**：端到端不变式由 Step 1b 背书（上引 :1132-1150），收窄不产生新 merge 级破口；补偿断言 = 把这一既有背书从事实变机证——V2 增一条夹具：「unlock 收窄形态下，off-main / provenance 违例 Pod 解锁后仍在同 invocation 被 Step 1b BLOCK 并回落 blocked」。
- **GA-4（gate 语义零变更轴）**：merge 链层面零变更成立（merge 前全闸必经零变）；unlock 瞬态宽化以**申报**消解——T6 条款补一句「解锁预核收窄为五要素事件核验，全闸由 Step 1b 同 invocation 背书（merge-driver :1132-1150）」，申报即消除「未申报」指控。
- **canary 4 对偶面**：kept-blocked / unlocked 语义字面不动；补偿夹具顺带钉住对偶侧。
- **消双跑真兑现**：unlock 不再 spawn，P3 的双进程开销与 argv 语义耦合在 merge-driver 侧真正消除。
- **弃选项 1（保留 spawn）的理由**：「消双跑」名不副实、T6 对 merge-driver 侧收益归零、已知 wart 原样保留——除非 adjudicator 认定 ready-to-merge 瞬态窗口对 board 消费方不可接受（我未见该消费方，board 状态对 planner 是信息面、merge 动作恒重跑 Step 1b）。
- **弃选项 3（全闸六条 + 豁免全部共享化）的理由**：为消一个已被 Step 1b 背书的瞬态差异，把提取面从 `resolveHumanApproval` 等纯函数扩到 `evaluate` 全域 + ctx 装配（含 execSync 副作用面需注入化），等价夹具负担 ×6 条规则 ×两档——成本收益倒挂，且扩面本身增大批内重构风险。

归属：adjudicator 显式裁（与 A-3 同场，challenger 建议我方接受）+ 裁后批内落（T6 申报句 + V2 背书夹具）。

## N6 · CONCEDE（COULD，轻）——ask 批准判据一行，随 T4/A-4 回填

challenger 自己定级「不值得独立修向」，我方同意：B-5 主体 REBUT 成立（R2 A-3 已确认），残留仅「弹窗 human 应核什么」未成文。修向：T4 裁断回填时在 `dev-loop.settings.json` `_doc` 数组（:3-15，现成承载位）加一行「ask 弹窗批准判据：核 human 授权 eventId 在场且可锚到 pod-events 对应事件」。归属：A-4 回填批内，零新增文件。

---

## 汇总

| # | verdict | 修向 | 归属 |
|---|---|---|---|
| N1 | CONCEDE | import 探针（主）+ argv 零命中 grep（辅）；弃「不 process.exit」负断言 | 批内（V2/F8 文案） |
| N2 | CONCEDE | 消费方名册六改七，补 portfolio-reconcile --self-check 复跑 | 批内 |
| N3 | CONCEDE | 首选修 runbook 命令形态（worktree 本地 core.editor=true + 素形 continue）；备选收窄至 merge 命令；A-4 回填主张限定句 | A-4 裁 + 批内回填 |
| N4 | CONCEDE | PROPOSAL:160 / architect-notes:117 符号订正为 makemeahanzi | 批内 |
| N5 | REBUT（定性）+ CONCEDE（须显式裁） | 推荐选项 2：收窄 + Step 1b 背书补偿夹具 + T6 申报句；备选保留 spawn（弃「消双跑」主张）；不建议全闸共享化 | adjudicator 裁 + 批内落 |
| N6 | CONCEDE（轻） | _doc 数组加 ask 批准判据一行 | A-4 回填 |

计数：CONCEDE 5（N1/N2/N3/N4/N6，其中 N6 轻）+ N5 半 REBUT 半 CONCEDE。

**进 adjudicator 的分歧（2 项）**：
1. **N5 解锁核验集**——新分叉。我方推荐选项 2（收窄 + Step 1b 背书补偿断言 + 申报），与 challenger 隐含倾向（保留 spawn 或全闸共享化）的真实分歧点 = 「Step 1b 背书是否足以把收窄定性为可申报的瞬态宽化而非 fail-open」。证据已给（:1368-1370 守卫 + :1132-1150 无条件全闸背书），可机判。
2. **GA-8 / A-4 ask 张力**——原拍板项，输入更新（N3 探针）。我方调整后立场：条件保留 ask，先修 runbook 命令形态（互洽），摩擦不可接受再收窄至 merge 命令 + hook 留根。

无新 deadlock：N1/N3 我方在 challenger 给的候选空间内给出了明确选择；N5 双方共享同一事实集后分歧收敛为三选项排序，交裁即可。

*defender · 2026-09-08 · 基线亲核：merge-driver :915-935/:1115-1153/:1366-1370、gate 全闸 evaluate 与 :344-393 入口、lib 五处 exit 实位、settings :3-21、pod-coordination :120-136、portfolio-reconcile :34/:329、test_char_stroke :22/:172/:678*
