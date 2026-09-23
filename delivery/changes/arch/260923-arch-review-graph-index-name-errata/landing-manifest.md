# Landing Manifest · 260923-arch-review-graph-index-name-errata

> packet-bound landing 清单 = PROPOSAL §5（human 立项 ack 2026-09-23「补治理四件再合」+ architect GREEN）。实际落地 = commit `5530aee`（`docs(gov): 图索引主条目正名勘误——KidPal → C-AI-KidPal`）。landing-verifier 逐文件 diff 核对「实际落地 == 本清单」。

## L1 · AGENTS.md — L43 表格行（项目名一格）

- 旧：`| 本仓主索引 | \`KidPal\` | ...`
- 新：`| 本仓主索引 | \`C-AI-KidPal\` | ...`（其余列逐字不动）

## L2 · AGENTS.md — L51 规则条整条重写

- 旧：`- 主索引项目名固定为 \`KidPal\`，改名会导致图工具 project not found。`
- 新：`- 主索引项目名 = 工具按路径派生（\`C:/AI/KidPal\` → \`C-AI-KidPal\`），与全部 Pod worktree / 参考工程条目同格式；\`index_repository\` 刷主仓**不传 \`name\`**——手工另传名会在同 root 产生双条目（2026-09-23 实测发生过一次，已删旧人工名条目）。图工具报 project not found 时先 \`list_projects\` 核对实际条目名，不凭记忆猜。\`KidPal-claude\` 为唯一人工命名特例（\`.claude\` 独立建项），不受本条约束。`

## L3 · .claude/skills/codebase-memory/cli/SKILL.md — 两处示例

- `:36`：`... cli index_status '{"project":"KidPal"}'` → `{"project":"C-AI-KidPal"}`
- `:42`：`... cli delete_project '{"project":"KidPal"}'` → `{"project":"C-AI-KidPal"}`

## L4 · .claude/skills/codebase-memory/exploring/SKILL.md — 三处示例

- `:44`：`get_code_snippet({qualified_name: "KidPal.server.shim.main.app.route.chat.POST"})` → 前缀 `C-AI-KidPal.`
- `:45`：同段注释「qualified_name 前缀 = 项目名 KidPal」→ 「…= 项目名 `C-AI-KidPal`」
- `:64`：`get_code_snippet({qualified_name: "KidPal...buildDisplayJson"})` → 前缀 `C-AI-KidPal...`

## L5 · .claude/rules/repo-structure.md — L26 消歧句

- 旧：`主 checkout 用主索引 \`KidPal\``
- 新：`主 checkout 用主索引 \`C-AI-KidPal\``（该行其余逐字不动；正名后与同行「项目名由工具按路径派生」句自洽）

## 仓外动作（非文件 landing，随本批执行并留痕）

- **删除 CodebaseMemory 注册表条目 `KidPal`**（`delete_project {"project":"KidPal"}` → `{"status":"deleted"}`）。删前已完成：`C-AI-KidPal` full 刷新（nodes 17952 / edges 48374）+ **符号级复验 3/3**（`grid_restore_enabled` / `grid_restore_payload` / `kidpal_render_node_schedule_on_screen`）。
- 证据：本会话实测记录（`index_status` 双侧 + `search_graph` 命中 3 条）。

## 不落地项（零触碰，landing-verifier 逐条核 0 diff）

- `docs/contracts/**`、`docs/requirements.md`、`docs/architecture.md`、`docs/ADR.md`；`firmware/**`、`server/**`、`tools/**`；`delivery/**` 除本制品目录（PROPOSAL / 本 manifest / architect-review / landing-verdict / retro）。
- **不顺带**：`AGENTS.md` L39「moderate 模式」（human 未裁）、`docs/notes/ui-to-lvgl-pitfalls.md` 的 `xiaozhi-rlcd`（human 未裁）、`docs/CONTRACT-DECISION-QUEUE.md`（不触契约面）。

## 同 commit

- 本制品目录 4 件（PROPOSAL + landing-manifest + architect-review + retro）+ `delivery/changes/arch/INDEX.md` 与本 change 的 index 状态（landing 后 upsert）。