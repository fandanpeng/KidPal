# debate-log · 260908-gov-review-merge-chain-hardening

> review-moderator 维护（append-only）。记录对抗内循环每轮派发、修复与裁定输入。

## 2026-09-08 · 编排时序

1. **phase-0 分类**：governance lane（触面全在 `.claude/**` + dev-loop.settings.json）；contract-touching = false；review-depth = full-adversarial（human OQ1 裁并批升级 + G3 强制层恒 full）。
2. **architect 起草候选**（agentId ae67caa0）：§T3 integrator 正文 / §T4 ask 裁定（条件 allow 在权限层不可表达）/ §T7 落点 = acceptance-evidence.md + 重指清单 / §review-bg 六关注面 + 八 canary 候选；3 处行号事实修正入 PROPOSAL §8；1 处申报不改交裁：**Step 1b（:1136-1142）与 approvalRefUnlock（:915-921）实为同形双源**（ARQ「Step 1b 无此形状」断言与源码不符），真实差异仅 findWorktree 回退分支。
3. **scenario-author 冻 charter**（agentId a9f88082）：FROZEN v1.0，10 轴 / 6 场景 / 12 canary（独立重派）/ 9 消费方留空点；开局播「头注互斥残留 + packet-binding 计数」一枚。
4. **moderator 机械残留修复（debate 前）**：PROPOSAL 头注删除 versioned-guard「明确不入本批」互斥残留句（v0.2 编辑遗漏）；packet-binding 「T1–T6」→「T1–T7」。此修复不改任何改法语义；challenger 仍应独立复核修复完整性与其余同类残留。

## 内循环记录

### R1 · challenger（agentId a7405009d10843693）

- 产出 findings-r1.md（moderator 逐字归档，子代理写盘被 harness 拦）：**13 findings（BLOCKING 3 / SHOULD 6 / COULD 4）**；B-0..B-8 九留空点全判（真断链 6 · 半断链 2）；canary 12/12 独立重派（活碰撞 #7=F2、#10=F5）；A-3 事实裁「同形成立」——Step 1b（:1137 gateArgs + :1142 cwd:wt）与 approvalRefUnlock（:919-921）同为「git 在 worktree cwd 求值、文件读在 --project-dir 主仓求值」，唯一差异 = 豁免路径 findWorktree 回退分支；ARQ「常规路径无此形状」对照系失真**确认**。
- 最重三条：F1 T6 lib 撞名（governance-gate-shared.mjs 已存在 518 行、六承重消费方，PROPOSAL 误标「新增」）；F2 V4 负断言永假（settings deny 条目自身含 docs/contracts 字面）；F3 manifest 缺 ARCH-REVIEW-QUEUE.md（V5 必改文件，锚集对账必红）。
- 未攻面 8 项在册（findings-r1.md §六）。

→ 派 defender R1 逐条 REBUT/CONCEDE。

### R1 · defender（agentId a770c3170b2ca2d2c，defense-r1.md 已落盘）

- F 系：CONCEDE 10（含 BLOCKING 3 全认，均独立亲核实锤）+ F9 各半 + F11 判 charter erratum（enforce-scope.mjs 无 --self-check 入口，moderator 注记归档）；REBUT 0。
- B 系：REBUT 3（B-1 claim 消费方在 T2-1 成文、缺的是机械 enforcement、GA-8 克制侧；B-4 写作侧对齐属 A-8 裁定闭合路径；B-5 执行者身份三处在场）+ 各半 2（B-0/B-8）+ CONCEDE 4（B-2/B-3/B-6/B-7）。
- F1 修向推荐**并入既有 lib**（追加导出，lib 头注 B5 纯新增先例；改名造双源）；F2 修向 = 断言改写（deny 两行逐字在位 + permissions 无 allow/ask 条目）；F3 修向 = manifest 直接补 docs/ARCH-REVIEW-QUEUE.md（与 A-5 无关可先行）。
- 交 decision-packet 张力点 4：F1 修向二选一 / A-3 联动回填 / B-1 机械挂点 vs prose（GA-8）/ F9+A-7 登记表 authority。
- deadlock 候选：无。

→ 派 challenger R2（定向：3 条 REBUT 复攻 + CONCEDE 修向完整性抽验 + 未攻面①②补攻）。

### R2 · challenger 定向复攻（agentId a56961531baf669eb，findings-r2.md TEXT-MODE 归档）

- 三条 REBUT 复攻：B-1 **REBUT 成立**（R1 撤回「真断链」；条件 = 留根行须真落）、B-4 **REBUT 成立**（附 decision-packet 完整性核验义务：裁 A-8 必须同场载写作侧对齐约定）、B-5 **大体成立**（残留 N6 COULD：ask 批准判据未成文，随 T4 回填一行）。
- CONCEDE 修向抽验：F1「并入既有 lib」自洽（B5 纯新增先例在案）但消费方实为 **7 非 6**（N2，portfolio-reconcile.mjs:34 漏计）；F8 修向文案自身被 lib 既有 5 处 process.exit 字面证伪（N1）；F2 改写机证力更强但 grep 转义须 -cF 固定串；F3 manifest 补行两侧形状亲核闭合、无新问题。
- **新 finding 5 条**：N1（SHOULD）/ N2（COULD）/ N3（SHOULD，runbook §10a 第 3 步逐字命令 `GIT_EDITOR=true git rebase --continue` 绕过 ask 前缀——ask 覆盖残缺实测）/ N4（COULD，齿名漂移 makemeahanzi）/ **N5（SHOULD，本轮最锋利）**：T6 spawn→import 把 approvalRefUnlock 解锁核验集从「全闸六条+豁免」收窄为「五要素」= fail-open 方向未申报，V2 夹具与 F8 排除项均不覆盖。
- settings ask 探针矩阵 7 条全跑（只读模拟）：`GIT_EDITOR=true` 前缀逃逸 + 复合命令盲区坐实（S-5 新实例）；scope-map 正交无抢占。
- elicitation：A-8 与 GA-8 维持升级（GA-8 须附 N3 重算成本收益）；V6 降为 packet 注记不升级。
- 分歧进 adjudicator 3 条：N5 新分叉 / GA-8 ask 张力（附 N3）/ B-1 挂点取舍。

→ 派 defender R2 答 N1–N5（challenger 建议 N5 先答辩再定级），随后 adjudicator。

### R2 · defender 定向答辩（agentId aa8d520796b0595ef，defense-r2.md 已落盘）

- 判定：CONCEDE 5（N1/N2/N3/N4/N6）+ N5 半 REBUT 半 CONCEDE。
- **N5 决定性新证据**：approvalRefUnlock 调用点唯一在 merge-driver 入口 :1368-1370（SINGLE_POD && HUMAN_APPROVAL_REF && !DRY_RUN 守卫），解锁后的 Pod 在同一 invocation 内必经 processPod Step 1b（:1132-1150）**无条件重 spawn 全闸**——merge 级 fail-closed 由 Step 1b 背书，unlock 预核只是前置；「fail-open」定性过重，「未申报分叉」CONCEDE。推荐选项 2 = 收窄 + Step 1b 背书补偿断言（T6 补申报句 + V2 增背书夹具：off-main/provenance 违例 Pod 解锁后仍被 Step 1b BLOCK 回落 blocked）。弃选项 1（消双跑落空）/选项 3（扩提取面成本倒挂）。
- **GA-8 调整为「条件保留 ask」**：先修 runbook 命令形态（worktree 本地 `git config core.editor true` + 素形 continue；`git -c` 不命中前缀不解决）使 ask 与主张互洽，摩擦实测不可接受再收窄至 `git merge:*` + hook 留根。（challenger 行号 :127 订正为 :126。）
- 无新 deadlock。分歧收敛为 adjudicator 两项：N5 三选项排序（双方共享事实后）+ GA-8/A-4（N3 探针更新输入）。

→ 派 adjudicator 裁定收口。

### 收口 · adjudicator（2026-09-08，制品 adjudications.md / review-packet.md / decision-packet.md 已落盘）

- 独立亲核（非采信双方自述）：A-3 同形（merge-driver :918-921 vs :1136-1142，唯一差异 = findWorktree||projectDir 回退分支）、F1 lib 518 行在册、F2 settings 三处命中（:7/:18-19）、N1 lib 五处 process.exit（:199/:231/:261/:334/:517）+ process.argv 零命中、N2 portfolio-reconcile :34 导入 + :329 自检、N3 pod-coordination :126 逐字（defender 行号订正成立）、N4 makemeahanzi vs makemehanzi 漂移（test_char_stroke :678/:22/:172）、N5 Step 1b 无条件全闸背书（:1368-1370 守卫 → processPod :1117 → :1136-1150）、F11 enforce-scope.mjs 无 self-check 入口、gate off-main/:249 向严默认在案。
- 裁定计数：27 条 finding/A 项 = CONFIRMED 22 / DISMISSED 4（B-1/B-4/B-5 整条按 R2 撤回终态 + F9/B-8 定性拆分部分）/ DEADLOCK 0。deadlock 独立复核：**确认双方声称成立**——所有分歧收敛为共享证据后的选项排序，非僵持。
- A 系 8 项全裁：A-3 = 维持两形只改回退分支（建议即地板）；A-1 = 不触硬排除字面、判据先例四条件归档；A-2 = 门内加项；A-4 = ask 条件保留 + 先修 runbook 命令形态（N3 吸收）；A-5 = acceptance-evidence.md、两排除落点不指回；A-6 = 本批不设门、lint 留根、guard-class 格式钉死；A-7 = 登记表 authority + 正则兜底 + 初始清单实测超集；A-8 = 放行（只拦可识别形态）+ B-4 写作侧对齐同场裁（packet 完整性义务履约）。N5 = 选项 2（收窄 + Step 1b 背书补偿断言 + 申报句）。
- pass-oracle 六关全 GREEN（governance profile）：判据覆盖 / finding 全结 / 高敏项已标 human（9 项拍板清单）/ 契约未触 / canaries 12/12（2 活碰撞随 F2/F5 闭环）/ decision-packet 完整。riskTier 维持 medium，review profile 终态无重报。dev-loop handoff not required（无实现侧修复 finding）。
- 总结论：可冻结（条件 = CONFIRMED 改法回灌 + human 9 项拍板）→ 定稿 → FROZEN → packet-bound landing。F11 charter erratum 交 moderator 注记。
