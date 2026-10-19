# landing-verdict · 261019-gov-review-docs-root-ledger-rehome

verdict: PASS + 2 ADVISORY

> 核验方式：零上下文独立重推——结构门、自检、验证集全部亲跑，48 锚逐文件人读 diff，12 条目机械 byte 对账（`node` 脚本比对原 CDQ 行），不采信 landing-summary 自述。核验时点 2026-10-19，gov worktree，landing 未提交（改动在工作树 + 暂存区）。

## 六项核验清单逐项结论

### 1. 逐文件 diff 对照 —— 通过

manifest 48 锚逐一核对实际 diff（`git diff HEAD` / 暂存区 / untracked 全覆盖），逐文件 semanticMatch 见下方留痕表。超出 manifest 的 canonical 改动：**零**——工作树全部改动集逐行核对，除 48 锚外仅四类账外面：`.codebase-memory/` 两件（机器再生快照，AGENTS.md 钉死不追交）、`delivery/review/INDEX.md` + `.changes-index-state.json`（本批 `--upsert` 簿记，状态行 `frozen` 正确、未提前推 landed）、`notes/archify/**`（并行 archify 会话工作产物，非 canonical）、本 review 目录证据件（含本 verdict）。`docs/ARCH-REVIEW-QUEUE.md` 与 `docs/CONTRACT-DECISION-QUEUE.md` 两删除锚实证：暂存 `D`、工作树不存在（`test -f` 均 exit 1）。

各 T 语义保真要点：

- **T1（CDQ 拆账）**：12 条目 + 2 生成 INDEX 全在；**12/12 条目正文 `**CDQ 行原文**` 段与 HEAD 原 CDQ 行机械 byte 对账逐字命中**（`node` 脚本 `txt.includes(row)`，含 260913/260902/261011/261010/261017-devloop 五 closed 与 260913 等 7 deferred）；状态映射与 OQ1(a) 表逐行一致（closed 5 落 `_archive/2026/`、deferred 7 落顶层、severity 260913=MAJOR 余 deferred=SHOULD、closed 日期 260829=08-29 / 260902=09-03 / 261011=10-12 / 261010=10-12 / 261017-devloop=10-17）；frontmatter 均含 `paths: [docs/contracts/interface-contract.md]` + `hardExclusions: true` + 归一 `source`（bare changeId → `delivery/taskpackages/` 或 `delivery/review/`）；closed 条目带 `closed`/`closedBy` 且正文有 `**结项**` 段；CDQ-260913 剩余条件逐字三处过账（trigger / 行原文 / `**剩余条件**` 段）。根文件已删。
- **T2（第四域机制）**：`debt-ledger-shared.mjs` 六处改法 ①–⑥ 逐字到位（DOMAINS contract 行置 requirement 后、ID_RE 扩 CDQ、PREFIX_TO_DOMAIN +CDQ、FM_ORDER 尾部 +`fourClass`,`loopEvidence`、`contract-no-fourclass` advisory、头注与注释更新；`GRANDFATHER_PREFIX = 'ARQ'` 未动）；render 行为零改、自检夹具 +2（夹具 8 非空断言先行符合真空绿规约、夹具 9 域过滤正控）；doctor 行为零改、自检夹具 +4（12–15：正控 / prefix-domain-mismatch 反控 / advisory-非红 / 正控零报，正反控成对可判别）；模板四件改法 ①–④ 逐字到位。
- **T3（闸读点换址）**：单文件读 → `docs/debt/contract/` 递归目录扫描（含 `_archive/<年>/`、排除 `INDEX.md`、目录不在场 try/catch → 空集向严）、`deps.listFiles` 可注入（默认真实递归列举）；核销条款 (a) 与 :378 BLOCK 文案与处方钉文逐字一致；自检夹具 4 例改写为 `listFiles + readText` 目录注入 + 新增 `cdq-archive-still-accounted` / `cdq-no-matching-entry-block` 两例（源码 grep 在场、`total=51` 证明实跑）；`CONTRACT_SPEC_LITERALS` 与 `hasArchLoopContractEvidence` 零改（diff 无此二函数 hunk）。
- **T4（ADR 迁址 + stub 撤销 + README）**：ADR 暂存 diff = 纯 rename 0 增 0 删，工作区 diff = 恰 +1 行迁址记（文本与处方逐字一致，落点凭据历史行与 11 条目零改）；两 stub 删除无重定向残留；`docs/debt/README.md` 四节（四域表 / 找·登记·结项 / 迁址记 / 生成件纪律）与处方口径一致。
- **T5（scope-map + fixture）**：allowFiles 三撤余四份真顶层文档、allowDirs +`docs/decisions`；integrator.allow 撤 stub、`docs/ADR.md`→`docs/decisions/ADR.md`、`docs/debt/**` 不动；adjudicator.allow 撤 stub；fixture adjudicator 行 allow 撤 stub、rationale 撤销注保留旧字面（白名单 ⑯）、`lastReviewed`→2026-10-19；architect 段零改（diff 无 hunk）——零新增写权授予。
- **T6（引用改指 23 处）**：rules 2 件、references 6 件、agents 5 件、skills 3 件、顶层 3 件、追注 2 件逐处对照处方钉文——均逐字或语义等价命中；bare 分工句三件（arch-change:20 ×2、arch-doc-restructure:15、architect-reviewer:27）原样未动；adjudicator:3 description 历史注保留；ARQ-261014 / TECH-261011 追注 = 纯尾部追加、frontmatter 未动（status 仍 deferred）；TECH-261019 新条目与 OQ2 甲案一致（10 处 paths 枚举、v5/v6 机械冻结 + v3/v4 E2 纪律冻结 + showme 豁免写进正文）。
- **私货审查**：五件 agent 定义零职责/权限语义变化（纯改指与枚举扩域）；scope-map 零新增 allow；闸 fail-closed 方向保持。唯一处方外改动 = 模板头注「三份 INDEX」→「各域 INDEX」（landing-summary 诚实注记已披露的同文件计数漂移顺手修，纯文面防自相矛盾，判定为处方 4 件的连带一致性修复，不构成私货）。模板头注同句修改另见 debt-ledger-shared 头注同步改（「三份 INDEX.md」→「各域 INDEX.md」），同性质。

### 2. packet-binding 一致 —— 通过

summary packet-id 行 = 「FROZEN PROPOSAL §2 改法集（T1–T6）+ §5 验证集 · R1 BLOCK→R2 GREEN · change-id 261019-gov-review-docs-root-ledger-rehome」，与 FROZEN 头部 packet-binding 行一致。summary 内嵌 manifest 与 PROPOSAL manifest 48 行逐行一致（结构门 proposalTargets == summaryTargets 亦实证）。T1 绑定面 = 12 条目清单：7 deferred 顶层 + 5 closed `_archive/2026/`，实落与 FROZEN 组件清单一致。落地范围 ⊆ 被批 packet（结构门 `unauthorized: []` + 本 verifier 工作树全量改动集独立核对双确认）。

### 3. 验证集复跑 —— 通过（§5 第 1–11 条全部亲跑，第 12 条 post-merge 待合并后）

| 项 | 亲跑结果 | 判定 |
|---|---|---|
| 1 闸自检 | `{"selfCheck":"pre-merge-escalation-gate","pass":51,"fail":0,"total":51,"ok":true}` exit 0（另 changeid-prefix 4/4） | ✅ |
| 2 debt 机制自检 | `debt-render --self-check: ok (10/10)`、`debt-doctor --self-check: ok (17/17)`，均 exit 0 | ✅ |
| 3 doctor 实仓 | `条目 176 条：gov=113 tech=49 requirement=2 contract=12`；RED 零命中 exit 0；advisory **196**（no-paths=108 severity-off-rubric=**23** legacy=19 tech-no-handoff=33 hard-exclusion-undeclared=13）——196 > 基线 195，见 ADVISORY-2 | ✅（附 ADVISORY-2） |
| 4 渲染幂等 | 连跑两次 8 份 INDEX 全「未变」、无归位移动行；渲染前后 `git status docs/debt` 零差 | ✅ |
| 5 INDEX 内容 | CDQ-260913 命中主 INDEX「待触发（7）」节 :20；open 7 行 = 260913/260914/260921/260924×3/261017 ✓；`_archive/2026/INDEX.md` CDQ 行 = 5；主 INDEX 已结区仅年度指针行 | ✅ |
| 6 剩余条件逐字 | `grep -F` 全角原文命中（trigger 字段 + 行原文 + 剩余条件段三处） | ✅ |
| 7 残留扫描 | 命中 27 行：18 行 ≡ 白名单 ①–⑯；9 行白名单外——独立裁定见 ADVISORY-1 | ✅（附 ADVISORY-1） |
| 8 bare 形态 | `arch-change:20`（两处 bare）、`arch-doc-restructure:15`、`architect-reviewer:27` 仅 bare 分工句；`architect-reviewer:28` 已改指 `docs/decisions/ADR.md`；三文件无 `docs/ADR.md` 路径形残留 | ✅ |
| 9 合并驱动 | 两件新 INDEX 均 `merge: union`（`.gitattributes` 零改成立） | ✅ |
| 10 docs 根终态 | `ls docs/` 顶层文件恰四份（DEV-GOVERNANCE / architecture / glossary / requirements）+ 子目录含 `decisions/` `debt/`；两队列文件不存在、`docs/decisions/ADR.md` 在场 | ✅ |
| 11 ADR 零改 | 暂存 = `docs/{ => decisions}/ADR.md` 纯 rename（0 增 0 删）；工作区 = +1 行迁址记 | ✅ |
| 12 post-merge | 待合并后执行（不属本 verdict 门槛） | ⏳ |

### 4. 结构门复跑 —— 通过

亲跑 `node .claude/scripts/landing-fidelity-check.mjs delivery/review/261019-gov-review-docs-root-ledger-rehome` → **exit 0**，`ok: true`、`frozen: true`、`packetBinding: true`、`unauthorized: []`、`claimedNotLanded: []`、`plannedNotLanded: []`、`gitDegraded: false`；`changedCanonicalNotInProposal: []`。citationAnchorAdvisory 两条 `file-not-exist`（`docs/ADR.md:4` / `docs/ARCH-REVIEW-QUEUE.md:3`）= PROPOSAL 引落地前文件的既有行，属预期（被引文件本批删除/迁移），非缺陷。

### 5. 真空绿规约 —— 通过

- 本批 §5 无重放类命令（无 git 修订符场景）。
- 接线类三处自检逐一核：闸两新夹具名 `cdq-archive-still-accounted` / `cdq-no-matching-entry-block` 源码在场且进 `cases` 数组、`total` 49→51 证明实跑非虚报；render 夹具 8/9（contract 渲染 + 域过滤）与 doctor 夹具 12–15 源码在场、8/8→10/10 与 13/13→17/17 证明实跑；夹具均正反控成对（反转实现必变红形态）。
- 环境红基线：doctor advisory +1 已先跑基线口径比对（分项逐项对账：六分项中五项与基线逐字相等，唯一增量 = severity-off-rubric 22→23），归因见 ADVISORY-2。

### 6. 诚实披露核对 —— 通过（两处偏差登记与实测一致）

summary「验证证据」表与「验证集偏差登记」逐项与亲跑结果一致：闸 51/51、render 10/10、doctor 17/17、条目 176、INDEX 命中、残留 27 行 18 白名单内 + 9 白名单外（编号 ⑰–㉕ 与实际命中行一一对应、无多报漏报）、advisory 196(+1) 如实披露且归因（CDQ-260913 沿用原队列 MAJOR）与我的独立归因相同。「自伤已修」注记（TECH-261011 追注残片）核对：该文件当前 diff 为完整追注段、无残片。无该记未记事项。

## 亲核留痕（关键命令 + 输出）

- `node .claude/scripts/landing-fidelity-check.mjs delivery/review/261019-gov-review-docs-root-ledger-rehome` → exit 0，`unauthorized/claimedNotLanded/plannedNotLanded` 全空。
- `node .claude/scripts/pre-merge-escalation-gate.mjs --self-check` → `{"pass":51,"fail":0,"total":51,"ok":true}` exit 0。
- `node .claude/scripts/debt-render.mjs --self-check` → `ok (10/10)`；`node .claude/scripts/debt-doctor.mjs --self-check` → `ok (17/17)`。
- `node .claude/scripts/debt-doctor.mjs` → `条目 176 条：gov=113 tech=49 requirement=2 contract=12 / advisory 196 条…severity-off-rubric=23 / RED 零命中` exit 0。
- `node .claude/scripts/debt-render.mjs` ×2 → 两跑均 8×「未变」零移动；渲染前后 `git status --porcelain docs/debt` 逐行 diff 为空。
- byte 对账：`git show HEAD:docs/CONTRACT-DECISION-QUEUE.md` 提取 12 数据行（`/^\| CDQ-\d/` 过滤恰 12 行），node 脚本逐条 `txt.includes(row)` → **verbatim_rows=12/12**。
- `grep -F "① 契约 :4 改指（下次契约门顺带）；②③ …一个动作处置" docs/debt/contract/CDQ-260913-spec-split-fallout.md` → 3 处命中。
- `git check-attr merge docs/debt/contract/INDEX.md docs/debt/contract/_archive/2026/INDEX.md` → 两件 `merge: union`。
- `git diff --cached --stat HEAD -- docs/ADR.md docs/decisions/ADR.md` → `docs/{ => decisions}/ADR.md | 0`；工作区 diff 恰 +1 迁址记行。
- `git grep -n "CONTRACT-DECISION-QUEUE\|ARCH-REVIEW-QUEUE\|docs/ADR\.md" -- ":!delivery" ":!docs/debt" ":!docs/archive" ":!server" ":!tools" ":!firmware" ":!notes/showme"` → 27 行（18 白名单内 / 9 见 ADVISORY-1）。
- `git grep -n "ADR\.md" -- .claude/references/arch-change.md .claude/skills/arch-doc-restructure/SKILL.md .claude/agents/architect-reviewer.md` → 仅 4 处 bare 分工句。
- `test -f docs/CONTRACT-DECISION-QUEUE.md` → 1；`test -f docs/ARCH-REVIEW-QUEUE.md` → 1；`test -f docs/decisions/ADR.md` → 0。
- verdict 行形态义务自证：`VERDICT_STATUS_RE`（`.claude/scripts/lib/governance-gate-shared.mjs:632`）亲读——本文件行首 `verdict: PASS + 2 ADVISORY` 捕获 token `PASS`，`changes-index.mjs deriveLanded` 可推 landed。

## 逐文件 semanticMatch 留痕（48 锚）

| 文件（组） | 触达 | semanticMatch |
|---|---|---|
| docs/CONTRACT-DECISION-QUEUE.md | 暂存删除 | ok（12 行全量迁出 byte 对账后删） |
| docs/ARCH-REVIEW-QUEUE.md | 暂存删除 | ok（指路内容并入 README） |
| docs/decisions/ADR.md（自 docs/ADR.md） | rename + 1 行 | ok（append-only 保持，迁址记逐字合处方） |
| docs/debt/contract/ 12 条目 | 新增 | ok（12/12 行原文 verbatim + 状态映射逐行合 OQ1(a)） |
| docs/debt/contract/INDEX.md + _archive/2026/INDEX.md | 生成 | ok（渲染产物、幂等、内容断言过） |
| docs/debt/README.md | 新增 | ok（四节合 T4.3 处方） |
| docs/debt/tech/TECH-261019-…md | 新增（OQ2 甲案条件锚） | ok |
| docs/debt/tech/INDEX.md | 生成 | ok（+TECH-261019 行、其余未动） |
| docs/debt/gov/ARQ-261014-…md | 追注 | ok（纯追加、status 未动） |
| docs/debt/tech/TECH-261011-…md | 追注 | ok（同上） |
| .claude/scripts/debt-ledger-shared.mjs | 改 | ok（T2①–⑥ 逐字） |
| .claude/scripts/debt-render.mjs | 改 | ok（行为零改、夹具 +2 合处方） |
| .claude/scripts/debt-doctor.mjs | 改 | ok（行为零改、夹具 +4 合处方） |
| .claude/scripts/pre-merge-escalation-gate.mjs | 改 | ok（T3 四点全中、不动面零改） |
| docs/templates/debt-item.template.md | 改 | ok（T2.4 四件逐字 + 披露过的头注计数顺手修） |
| .claude/scope-map.json | 改 | ok（T5.1–3 逐字、architect 段零触） |
| .claude/route-fixtures/surface-alignment.json | 改 | ok（T5.4 逐字） |
| .claude/rules/repo-structure.md | 改 | ok（:21/:61/:62/:70/:77 五处合处方） |
| .claude/rules/contracts.md | 改 | ok（:26 改指合处方） |
| references ×6 | 改 | ok（逐件合 T6 处方；arch-change.md 零改成立） |
| agents ×5 | 改 | ok（逐字合处方、零权限语义变化、adjudicator:3 历史注保留） |
| skills ×3 | 改 | ok（改指条目现居路径 + 撤销注合处方） |
| docs/DEV-GOVERNANCE.md | 改 | ok（:90/:109/:206/:240/:252 五处合处方、零门语义变化） |
| docs/glossary.md | 改 | ok（:35–:38 四处合处方） |
| docs/architecture.md | 改 | ok（:368 逐字合处方） |

## ADVISORY 清单（非阻断）

**ADVISORY-1 · §5 验证集第 7 条白名单欠列 9 行（PROPOSAL 侧瑕疵，非落地遗漏）**

独立裁定：残留扫描 27 行中白名单外的 9 行（summary 编号 ⑰–㉕：`repo-structure.md:21/:61`、`debt-ledger-shared.mjs:40`、`kidpal-idf-env/SKILL.md:56`、`kidpal-nvs-ops/SKILL.md:30`、`DEV-GOVERNANCE.md:90/:109`、`glossary.md:37`、`templates/debt-item.template.md:15`）**逐行人读后全部判定为「改写后新文自带的迁址注」而非漏改的活引用**——每行旧路径字面均嵌于「原 X 已删除/已迁/已并入 + 新址在哪」形态的出处注中（㉕ 且为 T2 改法 4① 处方文本逐字自带），删去旧字面即破坏注记本身；无一处是指向旧文件的活指令或机械消费点。同型先例 = 白名单 ⑬（architect.md:52，R1-F1 已钉）。根因假设：R1-F1 修订时白名单按「内容特征」枚举，只预列了处方文本逐字含旧字面的 ⑬，未预见 T2/T6 各改写行落地时需同类出处注。**判 PROPOSAL §5.7 白名单枚举不全，非落地遗漏、非私货 → ADVISORY 不阻断**。本 verdict 即该裁定的在册记录；后续批复用此验证集时应将 9 行按 ⑰–㉕ 补进白名单（或新增「处方新文迁址注」通配类目）。

**ADVISORY-2 · doctor advisory 196 超出 §5.3 承诺的 ≤195（PROPOSAL 内部不一致，落地遵循了承重处方）**

亲跑分项对账：六分项中五项与基线 195 逐字相等（no-paths=108、legacy=19、tech-no-handoff=33、hard-exclusion-undeclared=13 及 RED 零命中），唯一增量 = severity-off-rubric 22→23，来源 = CDQ-260913 条目 `severity: MAJOR`。该 MAJOR 档**正是 FROZEN §2 T1 映射表逐行钉死的值**（「HUMAN_APPROVED → deferred / MAJOR」）——迁移不造数据、不静默改档，landing 遵循了承重的 T1 处方；矛盾出在 PROPOSAL 自身（T1 钉 MAJOR 与 §5.3 预测「不新增 advisory」不同时成立：MAJOR 属 SEVERITIES 并集、不在 RUBRIC_SEVERITIES 五档内，doctor 必出 advisory）。landing-summary 已如实披露且归因一致。**判 PROPOSAL §5.3 预测失准，非落地偏差 → ADVISORY 不阻断**；后续批若再迁历史档 severity，验证集应把「迁移沿用原档 → severity-off-rubric +N」预列入容差。

## 结论

结构门 exit 0 + 逐文件语义保真全过 + 验证集 1–11 亲跑通过 + packet-binding 一致 + 两项偏差均判 PROPOSAL 侧瑕疵（ADVISORY 不阻断）。**放行建议：同意进入合并门**（post-merge 第 12 条验证 + INDEX `--upsert` 簿记归主线程；合并归 human manual-merge 门）。
