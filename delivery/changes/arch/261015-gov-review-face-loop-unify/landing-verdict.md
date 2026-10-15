# landing-verdict · 261015-gov-review-face-loop-unify

verdict: FAIL(F1,F2)

- 核验官：landing-verifier（Track B 独立核验，零上下文重推导，不采信主线程转述）
- 核验对象：commit `68ab057`（landing 提交，base = `b162a7c`），工作树残余仅 `.codebase-memory/` 两件工具自刷新派生物（合规，未 stage）
- 审计口径：`git diff b162a7c..68ab057` 全量 + 六项清单逐项 + §5 验证集亲跑
- packet-binding：summary packet-id 行 = 「FROZEN PROPOSAL §2 改法集（T1–T24）+ §5 验证集（V1–V11）」+ change-id `261015-gov-review-face-loop-unify`，与被批 review id 一致；landing 实际范围 ⊆ PROPOSAL manifest（45 件）∪ {PROPOSAL/summary 自身, `docs/debt/gov/_archive/2026/` 两件 debt-render 生成/归位件（A2 披露）}，无超范围私货 commit。

---

## 六项核验结论

### 1. 逐文件 diff 对照 — **FAIL（F1、F2）**

逐文件已核（47 件全量 diff，敏感面逐字核）：T1–T14、T16–T24 及 T20 十九件改指的**枚举内改法全部落到位**——AGENTS.md 治理路由节（T3）、CLAUDE.md（T4）、DEV-GOVERNANCE §4/§5/§7（T21）、scope-map（T2，deep-compare 全 JSON 仅 3 处语义差异 = architect-reviewer.allow 两条 + `_doc` 注记）、review-loop.settings（T16）、routing.json（T22，commandEntry 三行全删 + 信号重钉，与 V1 复跑绿自洽）、pre-merge-escalation-gate.mjs（T15 主体）、rules/contracts.md 五值 canonical（T14）、architect-reviewer.md 全文重写（T1）、architect.md（T5）、两面 SSOT 新建（T8/T12，判据清单与 FROZEN 逐字一致）、两命令新建/重写（T7/T11）、两命令删除（T9/T13）、CDQ 列义（T18）、§10e 退役（T19）、repo-structure（T24）、architecture.md §12（T10，追加不重排）、debt 结项（T23）、contract-change-evidence 收窄（T17）。

**F1（漏改 · 批定自检用例缺一）**：FROZEN T15 改法明列 self-check **六例**，第六例 `no-contracts-diff-unaffected` 在 `pre-merge-escalation-gate.mjs` 中**不存在**（`grep -n "no-contracts-diff-unaffected"` 零命中）。实测用例账：旧基线 34 处 expect/push → 新 43 处，增量恰 = 5 个具名新例 + 4 个函数 fixture（33 既有 + 9 = 42，与 V5 输出吻合）——第六例未落。行为面覆盖未塌：无契约 diff 不触发由基线例（`contractsDiff:false` + 凭据 `{ok:true}` → pass）与函数早退分支（`contractPaths.length === 0 → ok`，代码读核在场）承担；但该函数早退分支**无机检夹具**（4 个 fn fixture 全部携带契约路径），且 PROPOSAL §5 V5 预期「输出含**六个**契约凭据新用例名」字面不满足——summary V5 声称「含六个契约凭据用例」的计数口径（把改写的既有 `contracts-block` 计入）与 FROZEN 六例名单口径不一致，未披露。
- expected（FROZEN §2 T15）：self-check 含 `no-contracts-diff-unaffected` 具名用例，输出六个契约凭据新用例名。
- actual：脚本无该用例名；输出五名 + 改写既有例 + 4 fixture。
- category：批定改法没落全（漏改，机械可修——补一例即可，真空绿规约要求函数早退肢有判别夹具）。

**F2（漏改 + 该记未记 · 去人签门残留活规范句 4 处，全在在册文件内）**：全仓普查（`grep -rn "人类契约门|human contract gate|人工契约门…"` 排记录类封存面）实证以下**活规范句**与本批批定方向（OQ-1 去人签门）直接矛盾，且所在文件均为本批 manifest 在册、本批已触改的文件——批定目标效果（§0「契约面去掉人签字门」）在这些句子上未落：
1. `AGENTS.md:8`（契约面定义行）：「契约改动走 architect-reviewer 四分类 + **人类契约门**。」
2. `AGENTS.md:39`（**Hard Rules 契约冻结行**，指令根最高承重纪律句）：「任何会话不得用 Write/Edit/Bash 重定向写 `docs/contracts/**`，改动走 architect-reviewer 四分类 + **人类契约门**。」
3. `.claude/agents/architect.md:25`（When to Use 路由句）：「判断一个变更应该走 spec 修订、对抗 review、architect-reviewer、**human contract gate**，还是 planner/dev-loop。」
4. `.claude/references/architect-operating-model.md:156`（Tier-2 契约梯节）：「Any landing in `docs/contracts/**` requires `architect-reviewer` pre-screen and explicit **human contract gate**.」

其中 AGENTS.md 内部即自相矛盾（:8/:39 vs 本批新写的治理路由节「无人签门、合并过机械凭据闸」）；`requirement-change.md`、`docs/DEV-GOVERNANCE.md` 等同批重写件均已改指新语义，残留四句是漏改而非有意保留。summary 诚实注记段（A1/A2/V9/留根）对这 4 处**零披露**——该记未记。
- expected（FROZEN §0 目标效果 + T3/T5/T6 所涉文件的新语义一致性）：指令根与在册 reference 不再指示「人类契约门」为契约改动路径。
- actual：上述 4 处仍指示人签门。
- category：批定改法相对目标效果漏改 + 诚实披露缺漏。修法 = 四句按新语义改指（均在已 manifest 文件内、属 T3/T5/T6 同文件补落，manifest 无需扩张）+ summary 诚实注记补记。

### 2. packet-binding 一致 — **PASS + ADVISORY**

packet-id 行、manifest 围栏块双侧在场；summary 骨架合 `docs/templates/architect-landing-summary.template.md` 形态；manifest ↔ §2 目标文件锚并集对账（R1 焦点 5 / R3 复核口径独立重演：45 件与 T1–T24 锚并集一致，条件锚三件已按 V11 裁出）。**ADVISORY-A**：summary 自带 manifest 围栏块仅 44 件，**漏 `.claude/references/dev-loop-operating-system.md`**（PROPOSAL manifest 在册、实际已落地、summary §1 T20 明细亦列它）——转写漏行；因该件实际已落且在 PROPOSAL（约束性 manifest）在册，不构成 scope 或漏落问题，但 summary 声称集 ≠ PROPOSAL 计划集，下次修 landing 时顺手补齐。

### 3. 验证集复跑 — **PASS（V1–V8 亲跑全绿，V5 见 F1 口径注）**

- V1：`route-regression-check` **52 ok 0 fail** + `--self-check` **104 ok 0 fail**（「all required signals falsifiable」在场）。基线 53（FROZEN 前取）未独立复跑（landing 已提交，历史基线采信 PROPOSAL §5 在案记录），差 1 与 commandEntry 三行全删的解释相容。
- V2：`spec-anchor-check` → `"errors": [], "ok": true`。
- V3：`lint-descriptions` → scanned 16, flagged 0（excluded 1 = architect.md 硬排除，既有口径）。
- V4：scope-map / routing / review-loop.settings 三件 JSON.parse 全 ok。
- V5：`--self-check` **42 pass 0 fail**——绿但用例名集与 FROZEN 六例名单差一（见 F1）。
- V6：引用普查归零实证——`/review-arch|/review-governance` 仅剩 `docs/debt/gov/_archive/2026/ARQ-260928…:32` 结项注记一处（记录类封存合规）；`changes-index.mjs` 命中均为历史 changeId 字面量（`:50/:55/:225/:510` 亲读确认）。
- V7：`debt-render` 归位实证——ARQ-260928 在 `_archive/2026/`、frontmatter `status: closed`、`closedBy` 填本批；gov INDEX 摘要 20→19 未结 / 83→84 已结。
- V8：`--preflight` → findings `[]` / advisories `[]`。
- V9：文审走查（summary 已自报 ADVISORY，非实跑——维持该披露，首三个真实变更批回写 retro）。
- V10：破坏性自检实证——landing diff 中 `docs/contracts/` **空**；`dev-loop.settings.json` 与 `hooks/**` 零改动（name-status 不在场）；scope-map deep-compare 语义差异恰 3 处（= T2）；工作树无未提交 canonical 改动。

### 4. 结构门复跑 — **PASS + 披露缺口归因（非阻断）**

`node .claude/scripts/landing-fidelity-check.mjs delivery/changes/arch/261015-gov-review-face-loop-unify b162a7c` → exit 1，两条 error 均为**已披露且经实证成立的偏差**被机器如实点名：
- `.claude/references/dev-loop.md`（claimed/planned-not-landed）：A1 披露「在册未改」——独立实证成立：该件唯一 `review-arch` 命中 = `gov-review-architect-change-triage` changeId 字面子串（`:32`），无活命令引用可改，零 diff 是语义正确解；留 manifest 作普查覆盖凭证的处置如实。
- `docs/debt/gov/ARQ-260928-arch-change-retirement.md`（同）：A2 披露「debt-render 按状态自动归位 `_archive/2026/`」——实证成立（R070 rename + status=closed 在案）。
机器门无「披露豁免」通道，红是机械如实；语义层两项披露均实证为真，不构成放行障碍。本仓无 `cross-surface-check.mjs`（清单第 4 项按「无此门」处理，与简报注记一致）。

### 5. 真空绿规约 — **PASS + F1**

三件联立肢（FROZEN / verdict PASS / manifest 覆盖）各有否定用例（not-frozen / not-pass / not-covering / no-evidence）+ 4 个函数 fixture 判别性读核成立（r1–r4 反转实现即红）；放宽方向变化的环境红基线如实归因（原 `hasContractGateEvidence: false` 恒 BLOCK 逐字在旧版 `:366`，本次亲读 diff 确认）。缺口 = 第六例（F1）。**ADVISORY-B**：verdict 接受 `\b(PASS|GREEN)\b/i` 双 token——R1-F5 修法要求「token 取舍写一句 deliberation」，代码注释未成文该取舍（GREEN 与 PASS 同为本仓正极终态 token，方向无害，但应补一句注）；**ADVISORY-C**：候选正则 `\d{6}-(?:gov|arch)-review-` 同时覆盖两 lane，宽于 T15③ 字面「arch-review token」，与仓内双 lane 命名及 scope-map 写槽同形，良性。

### 6. 诚实披露核对 — **FAIL（并入 F2）+ ADVISORY**

A1（dev-loop.md 在册未改）、A2（ARQ 路径迁移）均与实际一致且经独立实证；ADVISORY V9（文审走查）、留根 6 条如实。**该记未记**：① F2 的 4 处人签门残留；② F1 的第六用例缺席与 V5 计数口径差；③ ADVISORY-A 的 summary manifest 漏行。**ADVISORY-D**（超枚举微改，无害）：review-loop.settings `_doc` 多删一行「与原仓区别：本仓档 1 无 dev-loop…」（陈旧元数据）；`rules/contracts.md` 头部迁移史句与「spec.md 链接同步改指」括注收缩；**ADVISORY-E**（packet 内部传导，良性）：AGENTS.md 升级判据新增「方向未收敛」条 + 实现面入口行，超出 T3 字面枚举但为 T8/T12 判据与 §0 四面目标的同批传导。

---

## 升级 human（随 FAIL 退回一并呈报）

FROZEN sweep 缺口（**不在本批 manifest 内**，landing 不可修，须 human/后续批裁决）——同普查发现的在册外人签门残留活句 5 处：
1. `.claude/commands/requirement-change.md:24`（「四分类 + 人类契约门，本流程无权处置」）
2. `.claude/references/requirement-change.md:79`（同上；需求面 SSOT——PROPOSAL §0 非目标明示不动需求面 loop，故本批未触，但其契约处置指针已悬空）
3. `.claude/references/architecture-decisions.md:3`（「以 architect-reviewer 和人工契约门为准」）
4. `.claude/scripts/brief-inputs.mjs:24`（注释「契约经人类契约门改节号时…」）
5. `docs/templates/AGENTS.template.md:48`（「确需改契约则停在人工契约门」）

## 退回指令（主线程修订后同核验官 resume 重验）

1. **F2**：四句按新语义改指（AGENTS.md:8/:39、architect.md:25、architect-operating-model.md:156——均在 manifest 在册文件内），summary 诚实注记补记；manifest 无需扩张。
2. **F1**：`pre-merge-escalation-gate.mjs` self-check 补 `no-contracts-diff-unaffected` 具名用例（函数级夹具：`contractPaths` 空 → ok，判别早退分支）；复跑 V5 确认 43 pass 0 fail。
3. **ADVISORY-A**：summary manifest 围栏块补 `dev-loop-operating-system.md` 一行。
4. 复跑 V1–V6、结构门（baseRef 口径）后重写 verdict。

## 亲核留痕

- 结构门：`landing-fidelity-check.mjs <dir>`（工作树口径，红 = 已提交提示）与 `<dir> b162a7c`（baseRef 口径，红 = A1/A2 两披露件）各一次，exit 1 输出全录；`--preflight` exit 0。
- V1–V5、V8：`route-regression-check.mjs`（52 ok 0 fail / self-check 104 ok）、`spec-anchor-check.mjs`（ok:true）、`lint-descriptions.mjs`（16/0）、三 JSON parse、`pre-merge-escalation-gate.mjs --self-check`（42/0）、preflight（[]/[]）。
- V6：两套 grep 实跑（`/review-arch|/review-governance` 归零除封存件；人签门词族普查 = F2/升级清单来源）。
- V10：`git diff b162a7c..68ab057 -- docs/contracts/`（空）、dev-loop.settings/hooks name-status（0 件）、`git status --porcelain`（仅 .codebase-memory 两件）、scope-map 新旧 deep-compare（3 处语义差异）。
- 敏感面逐字核：AGENTS.md / CLAUDE.md / DEV-GOVERNANCE.md / scope-map / review-loop.settings / routing.json / pre-merge-escalation-gate.mjs / architect-reviewer.md / architect.md / review-moderator.md 全 diff 人读；T20 十九件逐件 diff 读；T8/T12 与 FROZEN 判据清单逐字比对。
- 用例账：新旧 gate 脚本 expect/push 计数 34→43；`grep -n "no-contracts-diff-unaffected"` 零命中。
