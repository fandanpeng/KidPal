---
paths:
  - "docs/**"
---

# Repo 结构与治理契约（结构的单一事实源）

> 数据的单一事实源是 `docs/contracts/**`；**结构**的单一事实源是本文件。
> 创建 / 移动文件前以本表为准。改本表 = 有意识地改结构边界（同步改 [.claude/scope-map.json](../scope-map.json) 的 `universal.docsTopLevel`，两处必须一致——enforce-scope 钩子按 scope-map 机判）。

## 顶层地图（谁拥有 · 放什么 · 永不允许）

| 区 | 拥有者 | 放什么 | 永不允许 |
|---|---|---|---|
| `docs/contracts/**` | 契约门 | 笔顺 JSON / display 响应块 / 课表 MD / shim API 的权威定义（单一事实源 interface-contract.md） | 任何会话直改（改动走 architect-reviewer 四分类 + human 契约门）；在根级新建 `contracts/**`（影子目录） |
| `docs/` 根 | 主线程（治理批 landing） | 仅白名单内的正式顶层文档（见下） | 在 `docs/` 根新建白名单外文件 |
| `docs/notes/` | 主线程 / dev-loop | 经验与策略类文档：踩坑记录、实现策略、协议分析 | 当 spec 用（spec 面 = `docs/requirements.md` + `docs/architecture.md` 两文件，260913-arch-review-spec-split 起） |
| `docs/ui/` | 设计 | UI 设计稿与 brief：kidpal-ui-design HTML、UIUX-DESIGN-BRIEF、brand-spec | 把设计稿当实现代码改（设计稿是资产管线的母版） |
| `docs/templates/` | 治理 | PROPOSAL / retro 等治理制品骨架 | — |
| `docs/archive/` | 治理 | 已结项归档（如 ARCH-REVIEW-QUEUE.closed.md） | 把未结项塞进去 |
| `firmware/` | dev-loop（builder/fixer 等） | 固件实现面：小智 V2.1.0 整树基座 + 板级 overlay | review-loop 角色写入 |
| `server/` | dev-loop | 云端 shim 实现面（预留未建） | 同上 |
| `tools/` | dev-loop / 一次性实验 | 模拟器、资产管线等开发工具 | 当正式实现面（实验代码须由 dev-loop 包接管正式化） |
| `ESP32-S3-RLCD-4.2-Demo/**` | 只读参考 | 官方 Demo + 小智源码（第三方，不入版本控制） | 任何写入 |
| `.claude/` | 平台 | 治理机制：agents / rules / hooks / scope-map / settings / scripts / references | 用 Bash 重定向写（bashDenyWrite 红线） |
| `.codebase-memory/` | codebase-memory-mcp 工具（自动生成） | 图索引快照 artifact（`graph.db.zst` + `artifact.json`）与工具自写 `.gitattributes`（`merge=ours` 消二进制冲突）——worktree 从共享快照引导图索引、替代从头 full index | 手改（工具生成派生快照、非数据/结构 SSOT；刷新走 `index_repository(persistence=true)`）。**图索引项目归属（2026-09-05 消歧，ARQ-260905-worktree-graph-index-rules）**：主 checkout 用主索引 `KidPal`；**每个 Pod worktree 必须有自己的专属图索引，属 launch 面必办项**——worktree 准备阶段在 bootstrap 后跑一次 `index_repository(repo_path=<worktree>)`，即从仓内快照导入 + 增量补差（快照 2026-09-05 入仓，commit b7aa3df；项目名由工具按路径派生，先例 `C-AI-KidPal-pod-gateway-directionB` / `C-AI-KidPal-pod-host-test-infra`），不得让 Pod 降级「无专属图索引、读文件」；建前先以 `list_projects` 核对既有项目在册状态，历史文档 / launch prompt 中的降级注记不作为依据 |
| `changes/arch/**` | review-loop | 架构 review 证据：PROPOSAL / landing-summary / verdict / retro | 当契约 SSOT 用 |
| `changes/dev/**` | dev-loop | 实现变更证据归档 | 当 live 工作区用（live 在 `.claude/taskpackages/`） |

## docs/ 根白名单（唯一可直接放在 docs/ 顶层的东西）

文件：`requirements.md`（需求面 SSOT）· `architecture.md`（架构面 SSOT）· `DEV-GOVERNANCE.md` · `ARCH-REVIEW-QUEUE.md` · `CONTRACT-DECISION-QUEUE.md` · `ADR.md`（架构决策记录单文件日志，260907 批 T7，human 裁 docs 根平铺）· `glossary.md`（占位登记，文件尚未创建）
子目录：`contracts/` · `notes/` · `ui/` · `templates/` · `archive/`（旧 spec 归档于 `archive/spec.md`）

> 其它一律不进 docs/ 根。要加新的正式顶层文档 / 类别 → 先登记进 [.claude/scope-map.json](../scope-map.json) 的 `universal.docsTopLevel` 和本表（有意识动作），钩子才放行。

## 工作产物去哪

- 治理 / spec 变更过程产物 → `changes/arch/<change-id>/`（PROPOSAL + landing-summary + verdict + retro 四件）。
- dev-loop 过程产物 → `.claude/taskpackages/<change-id>/`（短期）→ 晋升 `changes/dev/<change-id>/`（长期）。
- 模拟器/管线的构建产物 → 不进仓（`.gitignore` 已覆盖 `tools/**/node_modules/`、模拟器 Output）。
