# PROPOSAL · 260923-arch-review-graph-index-name-errata

> **状态：FROZEN v1.0 · 2026-09-23** · 凭据 = ① human 立项 ack（2026-09-23「补治理四件再合」，原话见 §0 引）；② architect review GREEN（`architect-review.md`，2026-09-23，无打回项无条件项）；③ 契约零触点核查通过（`docs/contracts/**` 零改，无需契约门）；④ landing 实况 = commit `5530aee`（4 文件 8 行；落地先于评审、本批为事后收口补件，时序如实记于 architect-review §4）。
> **承载 ARQ**：无（根 = 工具默认命名与文档约定长期错位，属勘误）。
> **review 轨**：Track B 常驻轻路径（默认路由）——勘误类（非新 idea、不触契约、零 authority/owner/writer/approver 变更）。
> **packet-binding（FROZEN 生效）**：packet-id = 本 `PROPOSAL.md` §2 改法集（T1–T5）+ §3 验证集。packet = FROZEN PROPOSAL + architect-landing-summary + landing-verdict。
> **riskTier**：low（指令根文本勘误 + 注册表单条目清理；零代码、零契约、零 spec）。
> **review profile**：G1（指令根 AGENTS.md + `.claude/`）——approver = human，已在场并已裁。

```landing-manifest
AGENTS.md
.claude/skills/codebase-memory/cli/SKILL.md
.claude/skills/codebase-memory/exploring/SKILL.md
.claude/rules/repo-structure.md
```

---

## 0. 想实现的效果与差距

**目标效果一句话**：仓库 CodebaseMemory 图索引的「项目名」约定与工具实际行为对齐，消除同 root 双条目这一机制性漂移。

**现状差距**：同一路径 `C:/AI/KidPal` 下并存两个索引条目——`KidPal`（人工命名）与 `C-AI-KidPal`（工具按路径派生）；而 `AGENTS.md` 教所有 agent 按 `KidPal` 查图、还写「项目名固定为 `KidPal`」，与实际相反。后果是机制性漂移：`index_repository` 不带 `name` 时默认写派生名，任何一次「忘传 name」的刷新即再造一个条目（2026-09-23 实测发生一次）。

**human 指示（原话）**：「`C-AI-KidPal` 反而是对的，和我们以往的命名格式一致，agent 里写的不完成，改 Agent md 吧，你咨询一下 architect 的意见」。

## 1. 根因与正名判据

**根因**：`AGENTS.md` 的人工命名约定与工具默认（路径派生）长期错位；只要保留人工名，漂移就会复发。

**正名 = `C-AI-KidPal`（工具派生名）**，三条承重证据：

1. **在册先例已成文**：`docs/templates/GOVERNANCE-MIGRATION-GUIDE.md` §P7 明文「项目名必须与目录名一致，否则图工具报 project not found——本仓已实证踩过」，源仓正名即派生形态 `C-AI-AInativecompany`；
2. **对齐工具默认可消灭整类漂移**：不再需要任何人记得传 `name`；
3. **仓内本有自相矛盾**：`.claude/rules/repo-structure.md` 同一行既写「主 checkout 用主索引 `KidPal`」又写「项目名由工具按路径派生」，正名后自洽。

**唯一保留人工命名特例 = `KidPal-claude`**（`.claude` 为根独立建项、`persistence=false`）——新条文显式豁免。

## 2. 问题、根因与改法（T1–T5）

### T1 · AGENTS.md 主索引表格行指向人工名
**问题**：表内「本仓主索引」一格写 `KidPal`，与工具实际条目名 `C-AI-KidPal` 不符，agent 按表查图即 project not found。
**证据**：AGENTS.md L43（commit `5530aee` 前的原文）；注册表实测 `list_projects` 同 root 双条目。
**改法**：该格 `KidPal` → `C-AI-KidPal`（其余列逐字不动）。
目标文件：`AGENTS.md`（改）

### T2 · AGENTS.md「项目名固定为 KidPal」规则条方向相反
**问题**：原条文教「固定用人工名」，与工具默认相悖，且未给漂移兜底。
**证据**：AGENTS.md L51 原文「主索引项目名固定为 `KidPal`，改名会导致图工具 project not found。」
**改法**：整条重写为「项目名 = 工具按路径派生（`C:/AI/KidPal` → `C-AI-KidPal`）；刷主仓不传 `name`；报 project not found 先 `list_projects`；`KidPal-claude` 特例豁免」。
目标文件：`AGENTS.md`（改）

### T3 · cli SKILL 示例命令用人工名
**问题**：命令示例里的 `"project":"KidPal"` 会被后续会话当范式模仿（迁移指南 §6 明说示例即范式），漏改就是下一次 project not found 的种子。
**证据**：`.claude/skills/codebase-memory/cli/SKILL.md` `:36`（index_status 示例）与 `:42`（delete_project 示例）。
**改法**：两处 `"project":"KidPal"` → `"project":"C-AI-KidPal"`。
目标文件：`.claude/skills/codebase-memory/cli/SKILL.md`（改）

### T4 · exploring SKILL qualified_name 前缀示例用人工名
**问题**：qualified_name 前缀 = 项目名，示例 `KidPal.…` 在正名后全失效（同属「演示符号埋坐标」）。
**证据**：`.claude/skills/codebase-memory/exploring/SKILL.md` `:44`、`:45`（同段注释）、`:64`。
**改法**：三处前缀 `KidPal.` / `KidPal...` → `C-AI-KidPal.` / `C-AI-KidPal...`（注释同步）。
目标文件：`.claude/skills/codebase-memory/exploring/SKILL.md`（改）

### T5 · repo-structure 消歧句写人工名（同行自相矛盾）
**问题**：`.codebase-memory/` 行内既写「主 checkout 用主索引 `KidPal`」又写「项目名由工具按路径派生」，同句矛盾。
**证据**：`.claude/rules/repo-structure.md` L26。
**改法**：「主 checkout 用主索引 `KidPal`」→「…`C-AI-KidPal`」（该行其余逐字不动，正名后自洽）。
目标文件：`.claude/rules/repo-structure.md`（改）

## 3. 验证集（landing 后实跑，全绿）

1. `grep -rn "主索引 \`KidPal\`\|\"project\":\"KidPal\"" AGENTS.md .claude/` → **0 命中** ✓（实跑于 commit `5530aee` 后）
2. `git show 5530aee --numstat` → 恰 4 文件 / 8 insertions / 8 deletions ✓
3. 注册表：`list_projects` 同 root 恰一条 `C-AI-KidPal`；`index_status` = ready（nodes 17952 / edges 48374）；K7 三符号 `search_graph` 命中 ✓
4. 不落地项核：`docs/contracts/**`、requirements/architecture/ADR、firmware/server 零 diff ✓

## 4. 副作用处置（仓外动作，已执行）

`C-AI-KidPal` 先 full 刷新 + **符号级复验 3/3**（`grid_restore_enabled` / `grid_restore_payload` / `kidpal_render_node_schedule_on_screen`）→ 后删旧条目 `KidPal`（`delete_project` → `{"status":"deleted"}`），无「无健康条目」窗口。图数据为机器再生面，删条目不删仓内文件。

## 5. Human 取舍点

- **正名选谁**：human 已裁 `C-AI-KidPal`（派生名、消灭漂移）；architect 独立复核同意。
- **顺带发现（未纳入本批，留待 human 定夺）**：① `AGENTS.md` L39 写索引「（moderate 模式）」与实测不符（full/moderate 差约 44% 符号）；② `docs/notes/ui-to-lvgl-pitfalls.md` 写参考索引名 `xiaozhi-rlcd`，与实际条目名 `ESP32-S3-RLCD-4.2-Demo-03_XiaoZhi` 不符。