# architect-landing-summary · 260923-arch-review-graph-index-name-errata

> packet = FROZEN PROPOSAL（凭据：human 立项 ack 2026-09-23「补治理四件再合」+ architect GREEN @ `architect-review.md` + 契约零触点 + preflight 干跑 ok）+ landing-manifest（本件围栏块 4 文件）+ 本件。
> **packet-binding**：packet-id = `260923-arch-review-graph-index-name-errata` §2 改法集（T1–T5）+ §3 验证集；被批 review = `architect-review.md`（GREEN @ PROPOSAL，2026-09-23）。
> 落地分支：`dev/260915-m1-k1a-key-layer-dispatch`（Pod worktree）。落地日期：2026-09-23；落地 commit：`5530aee`（**落地先于评审，本批为 human「补治理四件再合」的事后收口补件**——时序如实记于 architect-review §4）。
> 配套详表：同目录 `landing-manifest.md`（逐 hunk 旧文→新文对表）+ `landing-verdict.md`（独立核验判定）。

```landing-manifest
AGENTS.md
.claude/skills/codebase-memory/cli/SKILL.md
.claude/skills/codebase-memory/exploring/SKILL.md
.claude/rules/repo-structure.md
```

## 落地内容（T1–T5 → 实际编辑）

| T | 落点 | 实际改法 |
|---|---|---|
| T1 | `AGENTS.md` L43 表格「本仓主索引」格 | `KidPal` → `C-AI-KidPal`（其余列逐字不动） |
| T2 | `AGENTS.md` L51 规则条 | 整条重写：项目名 = 工具按路径派生（`C:/AI/KidPal` → `C-AI-KidPal`）；刷主仓**不传 `name`**；报 project not found 先 `list_projects`；`KidPal-claude` 唯一人工命名特例豁免 |
| T3 | `.claude/skills/codebase-memory/cli/SKILL.md` `:36`/`:42` | 两处示例 `"project":"KidPal"` → `"project":"C-AI-KidPal"` |
| T4 | `.claude/skills/codebase-memory/exploring/SKILL.md` `:44`/`:45`/`:64` | qualified_name 前缀示例 `KidPal.` → `C-AI-KidPal.`（三处，含注释同步） |
| T5 | `.claude/rules/repo-structure.md` L26 消歧句 | 「主 checkout 用主索引 `KidPal`」→「…`C-AI-KidPal`」（该行其余不动） |

**计数口径**：git numstat = 4 文件 / 8 insertions / 8 deletions（PROPOSAL §2 表枚举 T1–T5、实际落点 8 行——T4 三处含同块相邻两行）。INDEX / retro 引「4 文件 8 行」为准。

## 仓外动作（非文件 landing，随本批）

- 删 CodebaseMemory 注册表旧条目 `KidPal`（删前 `C-AI-KidPal` 已 full 刷新 + 符号级复验 3/3）。

## §3 验证集实跑结果（2026-09-23，全绿）

1. `grep -rn "主索引 \`KidPal\`\|\"project\":\"KidPal\"" AGENTS.md .claude/` → **0 命中** ✓
2. `git show 5530aee --numstat` → 恰 4 文件 / 8 insertions / 8 deletions ✓
3. 注册表：`list_projects` 同 root 恰一条 `C-AI-KidPal`（31 项）；`index_status` ready（nodes 17952 / edges 48374）；`search_graph grid_restore_payload` 1 命中（server/gateway/schedule_push.py:80-93）✓
4. 不落地项核：`docs/contracts/**`、requirements/architecture/ADR、firmware/server/tools 零 diff ✓

## 残余风险（转 landing-verdict / retro 承接）

1. **历史散文旧名**（append-only 不追改；canonical 活面 grep 零残留已实证）：3 份 taskpackages intake-notes + dev 归档 + workstream archive 仍带旧名——兜底 = T2 新句「报 project not found 先 `list_projects`」。
2. **双条目复发兜底**：复发处置口径 = 删人工名条目（图数据机器再生）。
3. **cli SKILL `:42` 示例现指向正名主索引本身**（误照抄 delete_project 会删主条目，可再生但产噪音）——建议下批小勘误改无害占位，不阻塞本批。
4. 未纳入本批的两处同族失实（`AGENTS.md` L39 moderate / `docs/notes/ui-to-lvgl-pitfalls.md` xiaozhi-rlcd）——human 未裁。