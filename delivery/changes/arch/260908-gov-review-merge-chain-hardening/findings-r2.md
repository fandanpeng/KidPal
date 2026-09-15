# Challenger Findings · R2 定向复攻 · 260908-gov-review-merge-chain-hardening

> challenger 第 2 轮定向复攻（2026-09-08；agentId a56961531baf669eb，TEXT-MODE 由 moderator 逐字归档）。任务范围 = 攻 defense-r1 三条 REBUT（B-1/B-4/B-5）+ CONCEDE 修向完整性抽验（F1/F2/F3）+ 补攻 R1 未攻面①② + elicitation 三项立场复核。不重开 R1 已 CONCEDE 条目。基线亲核：main tip fae886a，全部行号逐字入文件复核。

## A. defense 三条 REBUT 复攻判定

**A-1 · B-1（versioned-artifact-claim 消费方与让位规则、机械挂点留观测后批）→ REBUT 成立，R1 撤回「真断链」定性。**

可检验证据：① charter P-4（charter :100）自身就把留空点钉为「launch 面无机械消费方」，表内「消费方声明位置 = T2-1」明列——消费方声明（owner=planner / 触发=launch / 让位规则=后到 Pod 从 vN+1 起编）在 PROPOSAL :71 成文，缺口只是 enforcement 挂点，defense 定性（分层问题非断链）与 charter 原文一致；② defense 的 fail-closed 兜底链可机证：claim 漏记 → 两包撞槽 → `*.v[0-9]+.md` 命中 T1-1 特判（PROPOSAL :61）→ blocked + 并集指引，失败态在 board 机械可见，非静默假绿——「效率损失非安全破口」成立，且该论证的覆盖面依赖 F9 修向（登记表扩实测超集）落地，F9 已 CONCEDE；③ §9 毕业先例逐字在案：pod-coordination.md:104-116「便宜档，不建 daemon」「毕业条件（量化，任一满足即评估升级）」「升级只换触发器」，defense 援引属实；④ defense 修向 = 留根一行（`--check-claim` 形态候选 + 毕业条件观测）——成立前提是该行真落留根清单，若 decision packet 后留根清单无此行则 REBUT 作废。残留条件注记：B-1 背后的挂点取舍本身仍属 GA-8 域交拍板（defense 张力点 3 已载），非本轮新增分歧。

**A-2 · B-4（Basis 写作侧对齐属 A-8 闭合路径）→ REBUT 成立，附一条 decision-packet 完整性核验义务。**

可检验证据：charter A-8 行（charter :55）原文「若裁放行，须同时裁『写作侧对齐约定』（B-4）作为补偿控制」——闭合点 = A-8 裁断，这是 charter 自己设计的依赖顺序，defense 引文逐字属实；charter §3 开头明令 decision-packet 必须逐项裁 A-8 且给出「裁断与归档位置」，故 B-4 闭合是 packet 的内置义务而非 PROPOSAL 漏写。defense 的批内承载点（A-8 裁后 T5 gate 核验行注释钉 Basis 匹配形态 + V2 两态夹具对账）与 S-3 期望答案兼容。R1 的「真断链」把「裁定项依赖顺序」误判为「成文缺口」，定性撤回。核验义务：decision packet 裁 A-8 时若未同时载写作侧对齐约定 = 第六关 packet 完整性缺口——此为 moderator/adjudicator 的核验点，不是双方分歧。

**A-3 · B-5（执行者身份三处在场）→ REBUT 大体成立，残留降级 COULD 一条。**

可检验证据：① sweep 表 PROPOSAL :112 pod-coordination 行消费方列逐字 =「planner / integrator / human 消解现场」；② pod-coordination.md:70「planner 在 supervisor APPROVE + 前置升级闸放行后自动触发 merge-driver」= planner 主线程现场在案；:124 第 1 步确无主语（「在 Pod worktree 手动重放」），defense 自认风格残余属实；③ integrator 分支由 architect-notes :19 触发条件（merge-driver abort + blocked 且 planner 派单指名）钉死；④ ask 弹给交互会话 human 操作者 = 平台机制事实（dev-loop.settings.json:6 显式加载于 human 启动监督的 `claude --settings … --agent planner` 会话）；「谁裁授权在场」答 = 弹窗 human，charter P-7 三问有着落。残留（N6，COULD）：ask 的批准判据（human 在弹窗时应核什么——授权 eventId 在场）未成文于本批任何文件；settings _doc 数组（dev-loop.settings.json:3-15）是现成承载位，T4 裁断回填时一行即可，不值得独立修向。

## B. CONCEDE 修向完整性抽验

**B-Ⅰ · F1 修向「并入既有 lib」——自洽，但两处精度修正 + 一个新虚宽维度（N1/N2/N5）。**

- B5 先例亲核属实：lib 头注 :12-13「B5 computeFrozenPin 纯新增……isFrozen 函数体零改动（S8 条件）」——「追加导出、既有导出零改动」的纯新增并入先例在案，并入修向与 GA-6 单源纪律同向（改名 = 造第二个治理 lib 双源），方向成立。
- 消费方计数失实（N2）：F1 修向与 R1 原文均称「六消费方」，实测为 **7**——`portfolio-reconcile.mjs:34` 具名导入 isProposalMdPath / normalizeSourceRef / resolveOriginRef（此前双方 grep 漏计）。等价夹具名册「各消费方既有 self-check 全量复跑零回退」必须补 portfolio-reconcile（其 --self-check 入口在 :329，可跑）。
- F8 未被 F1 修向自动吸收，且 F8 修向文案自身被既有字面证伪（N1，SHOULD）：defense F8 修向写「lib 入口不变量：不读 process.argv、不 process.exit（grep/AST 可机判）」——实测 lib 内 5 处 process.exit（:199/:231/:261/:334/:517，均在 run*SelfCheck 函数体内），按字面 grep 该断言今天即假，与 F2 同病族（负断言被既有字面打破）。须重划界为：「lib 顶层无副作用（import 时不 exit）；process.exit 仅存在于 run*SelfCheck 函数体内；不读 process.argv（此半句实测零命中，成立）」——或改用 import 探针夹具（import 该 lib 后进程存活且导出可调）。结论：追加导出本身不虚宽化 T6 主张（既有导出零改动由 import 语义 + B5 先例承载），但 V2 等价主张在 F8 修向按修正后文案落地前仍虚宽。
- 新虚宽维度（N5，SHOULD，本轮最锋利新 finding）：亲核 merge-driver 全文件仅两处 gate spawn——:920（approvalRefUnlock，T1）与 :1137（Step 1b）。T6「merge-driver T1 的 spawn 复用改 import」按 PROPOSAL :97 只提取「五要素核验共享函数（resolveHumanApproval 等）」——即 :920 的解锁核验集将从「**全闸六条 evaluate + 豁免**（现 spawn 形态含 off-main / 契约面 / blocked 复核，gate :136-160）」收窄为「**五要素事件核验**」。这是 fail-open 方向的核验集收窄（解锁条件宽化，紧邻 canary 4 kept-blocked 的对偶面），PROPOSAL 未申报、V2 等价夹具（PROPOSAL :137，五要素级「spawn 与 import 双路径同结果」）不覆盖解锁路径核验集变化；F8 的显式排除项（changeId 误判）也只覆盖语义耦合、不覆盖核验集范围。若保留 spawn（则「消双跑」名不副实）或收窄（须显式裁 + 补偿断言），二选一须显式裁定——非当前任何裁定项覆盖的新分叉。

**B-Ⅱ · F2 改写文案——方向成立、机证力确更强，但转义形态按字面不可执行（NIT 级随修）。**

defense 修向「`grep -c "Edit(\./docs/contracts/\*\*)"` = 1」在 BRE 下 `\(` 是分组开符、非字面括号——该 pattern 匹配「Edit」+分组内容而不含字面 `(`，对 deny 行实文 `Edit(./docs/contracts/**)` 返 0，正向断言自身永假。须改 `grep -cF 'Edit(./docs/contracts/**)'`（固定串）形态。「permissions 段无指向 docs/contracts 的 allow/ask 条目」可机判化：断言 ask 数组内容逐字 = T4 两形（`Bash(git merge:*)` / `Bash(git rebase:*)` 各 -cF = 1）+ deny 两行 -cF = 1。可写进 V4，比「零命中」强（正向钉死 deny 面 + 白名单化 ask 面）。

**B-Ⅲ · F3 manifest 补行——不引入新锚集问题，两侧形状亲核闭合。**

亲核 parseSection2AnchorLists（lib :345-423）：T7 目标文件行（PROPOSAL :105）唯一 backtick canonical 路径 `docs/ARCH-REVIEW-QUEUE.md` 过 ANCHORABLE_TEST（:40，`docs/` 前缀命中）；注记「（versioned-guard 行销账，随批 landing）」既不含「迁出」（:416）也不含「触发条件|条件」（:417）→ 入并集；「三选一规约落点（评审期定）」非 backtick → 不入锚集、无或锚（或锚判定需 ≥2 canonical 路径，:402-405）。manifest 侧补行后 compareAnchorSets（:425-432）双向差归零。A-5 裁断后 T7 行将成双 canonical 路径（落点 + QUEUE，间隙「 + 」无「或」）→ 并集 2 项均须入 manifest，落点路径随 A-5 回填时补——无新形状问题。defense「直接补、不加条件注记粉饰」的修向自洽。

## C. 补攻 R1 未攻面

**C-① · settings ask 条目经 enforce-scope / scope-map 真跑探针（只读模拟，零文件改动）。**

探针矩阵（逐条按 enforce-scope.mjs 实际代码路径模拟）：
1. `git rebase <target.ref>`（§10a 第 1 步形态）→ enforce-scope Bash 分支（:142-158）只对 extractBashWriteTargets（:86-119）产出做 bashDenyWrite 匹配，纯 git 命令零重定向目标 → exit 0（:157）；deny-main-push 快速出口（无 push 子串放行）→ settings deny 无命中 → ask 如期命中。✓
2. `GIT_EDITOR=true git rebase --continue`（pod-coordination.md:127 逐字，§10a 第 3 步）→ ask 前缀 `Bash(git rebase:*)` 不命中（命令串以 GIT_EDITOR=true 起头）→ 无弹窗。这是**非复合命令**的逃逸：runbook 自带的消解主循环命令形态即绕过 ask——S-5 盲区的新实例，且砍在「每次消解多 1-2 次人交互」成本计算的高频位（rebase --continue 循环逐轮执行）。见 N3。
3. `git merge --no-commit --no-ff main` → 前缀命中 → ask 如期。✓
4. `node .claude/scripts/merge-driver.mjs --pod X` → 不匹配任一前缀；内部 git 走 child_process 绕权限层（merge-driver.mjs:921/:1142 execFileSync 先例）→ 不受 ask 影响（§T4 事实 4 复核成立）。
5. 复合命令 `cd <wt> && git merge …` → 前缀不命中（S-5 已载盲区，复核成立）；enforce-scope 对其零重定向目标 → 亦不拦。
6. scope-map 交互：integrator「allow 10/10」是 Edit/Write 路径策略（enforce-scope :160-196），Bash 分支明文「不套 per-agent allow/deny」（:138-141 注）→ ask 条目与 scope-map 正交、无抢占、不与 canary 12（scope-map 零改）冲突；universal.bashDenyWrite = `["docs/contracts/**", ".claude/**"]`（scope-map.json 实测）对纯 git 命令零命中。
7. settings 无 allow 数组（dev-loop.settings.json:16-21 仅 deny）→ 无 allow 压过 ask 的前序问题。

**C-② · T7 重指清单 CHR-E3 :467 与 live 齿逐行入文件复核。**

- CHR-E3 接线齿 `test_e3_current_prompt_wired_and_carries_char_c1` def 恰在 `server/acceptance/test_char_stroke.acceptance.py:467`，docstring :468 起载「版本无关守卫」语义——architect-notes :111 清单 #1 逐字成立。✓
- CHR-A8 live：def `test_a8_live_block_matches_makemeahanzi` :678，`PROMPT_V2_PATH` 读点断言 :683-685 逐字在（`is_file()` + `_read_text`）。✓
- CHR-A9 live：def :718，读点 :722-724 逐字在。✓ 常量 `PROMPT_V2_PATH = DEPLOY_DIR / "openclaw-system-prompt.v2.md"` :144。✓
- 但齿名漂移一处（N4，COULD）：实际符号 = `test_a8_live_block_matches_makemeahanzi`（文件 :22/:172 自述），PROPOSAL :160 与 architect-notes :117 均写作 `test_a8_live_block_matches_makemehanzi`（少一个 me）——重指清点按名 grep 时该实例漏命中。本批管的正是指向精度，名册符号须逐字订正。

## D. elicitation 三项立场复核

1. **A-8 方向（T5 未识别形态放行 vs 拦）**：建议维持升级 adjudicator——本就是 charter §3 显式裁定项，本轮无新证据改变格局；裁时须带 B-4 闭合义务（A-2 结论），写作侧对齐约定随裁断同 packet 落。
2. **GA-8 ask 张力**：建议维持升级，且裁断输入须附 N3——「ask 摩擦 = 每次消解 1-2 次人交互」的成本收益计算已被探针改写：runbook 第 3 步逐字形态本就不弹，defense 顶 ask 侧的「真人类检查点」论证在最高频命令位不成立，须按残缺覆盖面重新权衡（或修 runbook 命令形态、或收窄 ask 主张）。
3. **V6 模板断言力差**：建议不升级——降为 decision packet 一句注记（V6 断言以临时仓夹具实测为准，schedule-reminder review-packet 实测数据仅叙事参照）+ landing-verifier 按「断言输入非空」真空绿规约核即可；非双方分歧，占 adjudicator 带宽不值。

## 新 finding 汇总（5 条：SHOULD 3 + COULD 2）

| # | 严重度 | 一句话 | 证据 |
|---|---|---|---|
| N1 | SHOULD | F8 修向「lib 不 process.exit」按字面永假——lib 已有 5 处 exit（均在 run*SelfCheck 体内），须重划界或改 import 探针 | governance-gate-shared.mjs:199/:231/:261/:334/:517；process.argv 零命中（半句成立） |
| N2 | COULD | lib 消费方 7 非 6：portfolio-reconcile.mjs:34 具名导入在册，等价夹具名册须补（其 --self-check :329 可跑） | portfolio-reconcile.mjs:34/:329 |
| N3 | SHOULD | T4 ask 前缀不命中 runbook §10a 第 3 步逐字命令 `GIT_EDITOR=true git rebase --continue`——非复合命令的逃逸，砍在最高频消解命令位 | pod-coordination.md:127；探针 C-①.2 |
| N4 | COULD | T7 清单 #7 齿名漂移：实际 `test_a8_live_block_matches_makemeahanzi`，PROPOSAL:160 / architect-notes:117 少一个「me」，按名清点漏命中 | test_char_stroke.acceptance.py:22/:172 vs PROPOSAL:160 |
| N5 | SHOULD | T6 spawn→import 将把 approvalRefUnlock 解锁核验集从「全闸六条+豁免」收窄为「五要素」——fail-open 方向未申报，V2 等价夹具与 F8 排除项均不覆盖；保留 spawn 则「消双跑」落空。二选一须显式裁 | merge-driver.mjs:918-922（唯一 T1 spawn）vs :1137；PROPOSAL:97/:137 |

## 进 adjudicator 的分歧清单

1. **N5（新分叉，建议与 A-3 同场显式裁）**：解锁核验集收窄 vs 保留全闸 spawn——含补偿断言形态与「消双跑」收益记账。
2. **GA-8 ask 张力（原有拍板项，输入更新）**：裁断须看 N3 新探针——ask 的覆盖残缺面已实测，成本收益重算。
3. **B-1 挂点取舍（defense 张力点 3 原样交拍板）**：R1 撤回「真断链」，无新增分歧；仅注记留根行（`--check-claim` + 毕业条件）须真落留根清单。

B-4 / B-5 REBUT 均成立，无分歧进 adjudicator；附带义务两条：A-8 packet 完整性核验（A-2）、ask 批准判据成文（N6 = B-5 残留，COULD，随 T4 回填一行）。
