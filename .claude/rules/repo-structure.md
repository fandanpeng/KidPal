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
| `docs/showme/` | 主线程（human 直接指示） | 一次性视觉解释稿的 HTML 归档：show-me 类示意图 / 信息图 / 短幻灯（治理演化、拆包链、索引拓扑等）；**不是** UI 设计稿 | 当 spec 或设计母版用（设计稿母版在 `docs/ui/`）；往里塞过程产物（过程产物→ `delivery/changes/**`） |
| `docs/templates/` | 治理 | PROPOSAL / retro 等治理制品骨架 | — |
| `docs/archive/` | 治理 | 已结项归档（长期不动的历史制品） | 把未结项塞进去 |
| `docs/crq/` | 主线程（登记）· adjudicator（review 收口）· integrator（scope-map 已授写槽） | **未结决策项账本**（CRQ = change request 账）：三域文件夹 `gov/` `tech/` `requirement/`，**一条目一文件**（YAML frontmatter 机读状态 + 正文 append-only 散文）+ 每域一份**生成件** `INDEX.md`；**已结条目由渲染脚本自动归位 `_archive/<结项年>/`**（镜像 `delivery/workstreams/_archive/<年>/` 先例；位置 = 状态的纯函数，**人不搬文件**，每年一份归档薄行 INDEX） | 手改各 `INDEX.md`（是生成件——改状态去改条目文件再重跑 `.claude/scripts/crq-render.mjs`）；人工移动条目文件（归位/搬回都是渲染器的事）；把 `docs/crq/` 当正式文档面（它是**会持续增长的记录库**，见下方语义说明） |
| `firmware/` | dev-loop（builder/fixer 等） | 固件实现面：小智 V2.1.0 整树基座 + 板级 overlay | review-loop 角色写入 |
| `server/` | dev-loop | 云端 shim 实现面（预留未建） | 同上 |
| `tools/` | dev-loop / 一次性实验 | 模拟器、资产管线等开发工具 | 当正式实现面（实验代码须由 dev-loop 包接管正式化） |
| `ESP32-S3-RLCD-4.2-Demo/**` | 只读参考 | 官方 Demo + 小智源码（第三方，不入版本控制） | 任何写入 |
| `.claude/` | 平台 | 治理机制：agents / rules / hooks / scope-map / settings / scripts / references | 用 Bash 重定向写（bashDenyWrite 红线） |
| `.codebase-memory/` | codebase-memory-mcp 工具（自动生成） | 图索引快照 artifact（`graph.db.zst` + `artifact.json`）与工具自写 `.gitattributes`（`merge=ours` 消二进制冲突）——worktree 从共享快照引导图索引、替代从头 full index | 手改（工具生成派生快照、非数据/结构 SSOT；刷新走 `index_repository(persistence=true)`）。**图索引项目归属（2026-09-05 消歧，ARQ-260905-worktree-graph-index-rules）**：主 checkout 用主索引 `C-AI-KidPal`；**每个 Pod worktree 必须有自己的专属图索引，属 launch 面必办项**——worktree 准备阶段在 bootstrap 后跑一次 `index_repository(repo_path=<worktree>)`，即从仓内快照导入 + 增量补差（快照 2026-09-05 入仓，commit b7aa3df；项目名由工具按路径派生，先例 `C-AI-KidPal-pod-gateway-directionB` / `C-AI-KidPal-pod-host-test-infra`），不得让 Pod 降级「无专属图索引、读文件」；建前先以 `list_projects` 核对既有项目在册状态，历史文档 / 启动卡中的降级注记不作为依据 |
| `delivery/milestones/` | 主线程 | 里程碑注册表：`milestones.jsonl`（单一事实源，state 由人/integrator 维护）+ 脚本再生 `STATUS.md` + `delivery-workspace.json` 绑定元数据 | 手改派生件 STATUS.md（改状态去改 milestones.jsonl 再重跑脚本） |
| `delivery/workstreams/` | 人 · 主线程 planner · dev-loop | workstream 级长周期工作记忆（为什么/发现/进展），准入判据与生命周期治理见 [delivery/workstreams/README.md](../../delivery/workstreams/README.md)；活跃在顶层、完结归 `_archive/<年>/` | 违反准入判据建目录（半年后接手的 agent 靠 diff + taskpackage + commit 能继续的不配建） |
| `delivery/taskpackages/` | dev-loop | 单切片在途验收包（含 `current/` 活工作面） | 当长期归档用（长期在 `delivery/changes/dev/`）；Bash 重定向写（scope-map bashDenyWrite 红线） |
| `delivery/changes/arch/**` | review-loop | 架构 review 证据：PROPOSAL / landing-summary / verdict / retro | 当契约 SSOT 用 |
| `delivery/changes/dev/**` | dev-loop | 实现变更证据归档 | 当 live 工作区用（live 在 `delivery/taskpackages/`） |

## docs/ 根白名单（唯一可直接放在 docs/ 顶层的东西）

文件：`requirements.md`（需求面 SSOT）· `architecture.md`（架构面 SSOT）· `DEV-GOVERNANCE.md` · `ARCH-REVIEW-QUEUE.md`（**已降为指路 stub**，见下）· `CONTRACT-DECISION-QUEUE.md` · `ADR.md`（架构决策记录单文件日志，260907 批 T7，human 裁 docs 根平铺）· `glossary.md`（术语表，已建）
子目录：`contracts/` · `notes/` · `ui/` · `showme/` · `templates/` · `archive/`（旧 spec 归档于 `archive/spec.md`）· `crq/`（未结决策项账本，见上表行）

### 三种同名物消歧（260923-gov-review-arq-ssot-restructure T6 钉死；仓内同名 `INDEX.md` 现共 8 份且随归档年增长，`find . -name INDEX.md` 可枚举）

| 路径 | 索引什么 | 划分轴 | 谁写 |
|---|---|---|---|
| `delivery/changes/{arch,dev}/INDEX.md` | **变更批次的落地状态**（changeId / 日期 / draft→frozen→landed / 已合main） | **lane**（Pod 合并流 vs gov 合并流） | `changes-index.mjs` 生成 |
| `docs/crq/{gov,tech,requirement}/INDEX.md`（+ 各域 `_archive/<年>/INDEX.md`） | **未结决策项**（未结清单 + 已结指针/薄行） | **域**（写者/裁定线不同） | `crq-render.mjs` 生成 |
| `delivery/workstreams/INDEX.md` | workstream 清单 | — | hook 自动生成（编辑 workstreams/ 即重建），勿手改 |

> **引用纪律**：任何引用 `INDEX.md` **必须带目录**（写 `docs/crq/gov/INDEX.md` 或 `delivery/changes/arch/INDEX.md`），**禁裸写 `INDEX.md`**——裸写在本仓是歧义的。

### 语义说明：`docs/` 的边界在本批扩了一次（有意识动作）

`docs/` 原定义 = 「仅白名单内的**正式顶层文档**」+ 五类子目录（契约 / 经验 / 设计 / 模板 / 归档）；现共七类——并行批（a89f3a6，human 直接指示）增 `showme/`，本批增 `crq/`。`docs/crq/` 的加入使 docs **同时承接一个会持续增长的记录库**（首日 94 条目 + 5 份生成 index〔主 3 + 归档年 2，随归档年增长〕，只增不减）。这是**有意识的边界扩张**，不是顺手放东西：账本原本就住 `docs/`（`ARCH-REVIEW-QUEUE.md` / `CONTRACT-DECISION-QUEUE.md` 都是 docs 根文件），迁进 `docs/crq/` 是**原地铺开**而非换树。**复议触发 = docs 子目录数再增两个，或出现第二个记录库**——届时重新考虑 docs 的边界定义。

> 其它一律不进 docs/ 根。要加新的正式顶层文档 / 类别 → 先登记进 [.claude/scope-map.json](../scope-map.json) 的 `universal.docsTopLevel` 和本表（有意识动作），钩子才放行。

## 工作产物去哪

- 治理 / spec 变更过程产物 → `delivery/changes/arch/<change-id>/`（PROPOSAL + landing-summary + verdict + retro 四件）。
- dev-loop 过程产物 → `delivery/taskpackages/<change-id>/`（短期）→ 晋升 `delivery/changes/dev/<change-id>/`（长期）。
- 模拟器/管线的构建产物 → 不进仓（`.gitignore` 已覆盖 `tools/**/node_modules/`、模拟器 Output）。
