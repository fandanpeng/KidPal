# Landing Verdict · 260923-arch-review-graph-index-name-errata

> **判定器**：landing-verifier（独立核验，只读 codebase，不参 landing）
> **核验对象**：frozen 清单 `landing-manifest.md`（L1-L5 + 仓外动作 + 不落地项）／方案面 `PROPOSAL.md` + `architect-review.md` + `architect-landing-summary.md`／实际落地 = commit `5530aee`
> **核验日期**：2026-09-23（**形态修复后复验版**，替代前版 RED 结论）

## 0. 总裁断（复验后）

**GREEN — 结构门与语义核双双通过，可放行至 human manual-merge 门（merge 仍归 human）。**

- **前版 RED 的五条形态偏差（D1-D5）已逐条实证修复**（见 §3），且**落地 commit `5530aee` 零改**（`HEAD`=5530aee、`numstat` 仍 4 文件 / 8 ins / 8 del）——即「修形态、不动 diff」的修复路径完全兑现。
- **结构门复跑 `ok: true`**（独立重跑，非采信转述）：`frozen=true` / `packetBinding=true` / 双侧 `proposalTargets≡summaryTargets`（4 文件）/ `unauthorized=[]` / `claimedNotLanded=[]` / `plannedNotLanded=[]` / `gitDegraded=false`。
- **语义核（逐文件 diff）仍全绿**：commit 不可变，前版对 L1-L5 的逐字核对与不落地项核对原样成立（§2），未受形态修复影响。
- **工作树侧证**：`git status` 显示改动**仅**本制品目录（6 件 untracked），`git diff --stat` 空——无任何 canonical 文件被顺带改，修复未混入私货。

## 1. 两个机械门输出（复验）

### 1.1 结构门 `landing-fidelity-check.mjs` — GREEN（exit 0）

```json
{"ok": true, "gate": "landing-fidelity", "frozen": true, "packetBinding": true,
 "proposalTargets": ["AGENTS.md",".claude/skills/codebase-memory/cli/SKILL.md",
   ".claude/skills/codebase-memory/exploring/SKILL.md",".claude/rules/repo-structure.md"],
 "summaryTargets": [（同上 4 文件，双侧一致）],
 "unauthorized": [], "claimedNotLanded": [], "plannedNotLanded": [],
 "gitDegraded": false, "advisory.changedCanonicalNotInProposal": []}
```

独立直调 `governance-gate-shared.mjs` 复验内嵌判据：`isFrozen(PROPOSAL)=true`；`extractTargets(PROPOSAL)` 与 `extractTargets(summary)` 均返回同一 4 文件集；摘要件含 `packet-id` 字面。

### 1.2 设计侧门 `proposal-consolidation-check.mjs` — N/A（Track B）

本变更 Track B、证据集无 `decision-packet.md` → 按流程本门 **N/A**，不跑（Track A 方跑）。

## 2. 逐项 PASS/FAIL 表（语义核 + 仓外 + 不落地）

| 项 | 期望 | 实际（commit diff 实证） | 判 |
|---|---|---|---|
| **L1** AGENTS.md L43 表格行 | 主索引名格 `KidPal`→`C-AI-KidPal`，余列逐字不动 | diff 仅该格变，其余列逐字同 | **PASS** |
| **L2** AGENTS.md L51 规则条 | 整条重写（派生名 / 不传 name / list_projects 兜底 / KidPal-claude 特例） | 新文与清单逐字一致，三要素齐 | **PASS** |
| **L3** cli/SKILL.md :36 / :42 | `"project":"KidPal"`→`C-AI-KidPal` ×2 | 现行文两处均 `C-AI-KidPal`，行号精确 | **PASS** |
| **L4** exploring/SKILL.md :44 / :45 / :64 | 前缀 `KidPal.`→`C-AI-KidPal.` ×3 | 三处前缀替换 + 注释同步，行号精确 | **PASS** |
| **L5** repo-structure.md L26 | 「主 checkout 用主索引 `KidPal`」→`C-AI-KidPal` | 现行文已改，该行余文不动、正名后自洽 | **PASS** |
| **仓外·①** 删旧条目 `KidPal` | 同 root 仅存 `C-AI-KidPal` | `list_projects` 同 root 恰一条 `C-AI-KidPal`，无 `KidPal` 条目（31 项） | **PASS** |
| **仓外·②** `C-AI-KidPal` 健康 | ready（nodes 17952 / edges 48374） | 实测 `ready`、数字逐字相符 | **PASS** |
| **仓外·③** K7 符号可命中 | `search_graph` 命中 | `grid_restore_payload` 1 命中（schedule_push.py:80-93） | **PASS** |
| **仓外·④** 可取源码 | `get_code_snippet` 命中 | 返回 `grid_restore_payload` 全文源码 | **PASS** |
| **不落地项** 零触碰 | 恰 4 文件；contracts/spec/firmware/server/tools 零改；L39 / ui-to-lvgl-pitfalls 不顺带 | commit `--name-only` 恰 4 文件；上述面零改；工作树 `git diff --stat` 空 | **PASS** |
| **结构门** | GREEN | GREEN（§1.1） | **PASS** |

**计数**：语义核 9/9 PASS、不落地项 1/1 PASS、结构门 1 PASS、设计侧门 N/A。

## 3. D1-D5 复验（逐条读文件原文，非采信转述）

| # | 前版偏差 | 复验证据（读现行原文） | 判 |
|---|---|---|---|
| D1 | 缺 `architect-landing-summary.md` | 已在场（3677B）：含 `packet-id` + 落地表（T1-T5→实际编辑）+ §3 验证集实跑结果；`landing-manifest.md` 保留为逐 hunk 详表 | **已修** |
| D2 | PROPOSAL 无 Status 行、`isFrozen=false` | 首段 L3 = `> **状态：FROZEN · 2026-09-23** · 凭据 = …`；直调 `isFrozen=true` | **已修** |
| D3 | 双侧缺 ```landing-manifest 围栏块 | PROPOSAL L10-15 与 summary L8-13 **双侧**各有 4 行围栏块；`extractTargets` 双侧同返 4 文件 | **已修** |
| D4 | 摘要缺 `packet-id` 字面 | summary L4 = `packet-binding：packet-id = \`260923-arch-review-graph-index-name-errata\`…`；字面在 | **已修** |
| D5 | §2 无 `### T<n>` 锚清单 | §2 重构为 `### T1`–`### T5`，每节含 `目标文件：` 行；机检 §2↔manifest 锚对账通过（`unauthorized: []`） | **已修** |

## 4. §2 T1-T5 与 §3 验证集 ↔ 已核 diff 一致性

- **T-node 内容与我前版逐字核过的 diff 一致**：T1↔AGENTS L43 表格格、T2↔AGENTS L51 整条、T3↔cli :36/:42、T4↔exploring :44/:45/:64、T5↔repo-structure L26——改法描述与证据行号均与 commit `5530aee` 实况吻合，**无偏差**。T-node 目标文件集（AGENTS.md / cli SKILL / exploring SKILL / repo-structure）≡ manifest 围栏块 4 文件，无 scope creep。
- **计数口径自洽**：T1-T5 落点 = 8 行（T4 三处含同块相邻两行），与 `git show 5530aee --numstat` 的 4 文件 / 8 ins / 8 del **相符**。
- **§3 验证集独立实跑复验**：① `grep -rn "主索引 \`KidPal\`\|\"project\":\"KidPal\"" AGENTS.md .claude/` → **0 命中**（我复跑，exit=1，与声称相符）；② `numstat` = 4/8/8（复跑相符）；③ 注册表三项（同 root 单条目 / ready 17952-48374 / K7 符号命中）我独立跑过，相符；④ 不落地项零 diff（`git show --name-only` 恰 4 文件）相符。**四条验证集均经我独立复算，全绿。**

## 5. 破坏性自检（复验）

- **未只挑好看的文件核**：commit 全 4 文件逐字核过（含 AGENTS.md 两处、repo-structure 整表行）；敏感面 G1 指令根（AGENTS.md）/ review-loop 引用面（`.claude/rules`、`.claude/skills`）逐一核，无借 landing 夹带权限或写域扩张。
- **形态修复未夹带**：`git status` 改动仅本制品目录、`git diff --stat` 空 → 修复只动制品形态，未碰任何 canonical。
- **L51 新条文自洽 + 特例显式豁免**：条内明写 `KidPal-claude` 为唯一人工命名特例、不受本条约束；与同行在册特例自洽。

## 6. 时序注记（不变）

landing（commit `5530aee` 22:25）先于评审与制品补件。结论：**不影响保真判定**——commit 不可变、diff 可离线逐字复算，语义核与注册表 spot-check 均据实证完成；形态修复后结构门复跑 GREEN，两端闭合；前述时序偏差不再构成障碍（制品形态已合规）。

## 7. 放行建议

**GREEN：结构门 + 语义核双过，D1-D5 实证修复且 diff 零改。建议放行至 human manual-merge 门（本地 main 可回滚；remote push 仍归 human）。** 本判定只读 codebase、只写本 verdict，未改任何 canonical / 制品既有件、未 commit、未 push。

**残余风险承接（转 retro）**：① 历史散文旧名（append-only 不追改，canonical 活面 grep 零残留）；② 双条目复发兜底 = 删人工名条目；③ cli SKILL `:42` 示例现指向正名主索引本身（误照抄 `delete_project` 会删主条目，可再生但产噪音——建议下批改无害占位）；④ AGENTS.md L39「moderate」与 `xiaozhi-rlcd` 两处同族失实留待 human 裁。