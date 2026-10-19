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
| `docs/contracts/**` | 契约面（写冻结防线） | 笔顺 JSON / display 响应块 / 课表 MD / shim API 的权威定义（单一事实源 interface-contract.md） | 任何 agent 直改（改动走 `/arch-change` loop；合并过机械凭据闸 `pre-merge-escalation-gate.mjs`——261015 批起无人签门）；在根级新建 `contracts/**`（影子目录） |
| `docs/` 根 | 主线程（治理批 landing） | 仅白名单内的正式顶层文档（见下） | 在 `docs/` 根新建白名单外文件 |
| `docs/notes/` | 主线程 / dev-loop | 经验与策略类文档：踩坑记录、实现策略、协议分析 | 当 spec 用（spec 面 = `docs/requirements.md` + `docs/architecture.md` 两文件，260913-arch-review-spec-split 起） |
| `docs/ui/` | 设计 | UI 设计稿与 brief：kidpal-ui-design HTML、UIUX-DESIGN-BRIEF、brand-spec | 把设计稿当实现代码改（设计稿是资产管线的母版） |
| `docs/templates/` | 治理 | PROPOSAL / retro 等治理制品骨架 | — |
| `docs/archive/` | 治理 | 已结项归档（长期不动的历史制品） | 把未结项塞进去 |
| `docs/debt/` | 主线程（登记）· adjudicator（review 收口）· integrator（scope-map 已授写槽） | **未结决策项账本**（debt 账，原名 crq、261014 批改名）：四域文件夹 `gov/` `tech/` `requirement/` `contract/`（contract = 契约决策账，261019 批自 `docs/CONTRACT-DECISION-QUEUE.md` 并入），**一条目一文件**（YAML frontmatter 机读状态 + 正文 append-only 散文）+ 每域一份**生成件** `INDEX.md`；**已结条目由渲染脚本自动归位 `_archive/<结项年>/`**（镜像 `delivery/workstreams/_archive/<年>/` 先例；位置 = 状态的纯函数，**人不搬文件**，每年一份归档薄行 INDEX） | 手改各 `INDEX.md`（是生成件——改状态去改条目文件再重跑 `.claude/scripts/debt-render.mjs`）；人工移动条目文件（归位/搬回都是渲染器的事）；把 `docs/debt/` 当正式文档面（它是**会持续增长的记录库**，见下方语义说明） |
| `firmware/` | dev-loop（builder/fixer 等） | 固件实现面：小智 V2.1.0 整树基座 + 板级 overlay | review-loop 角色写入 |
| `server/` | dev-loop | 云端 shim 实现面（预留未建） | 同上 |
| `tools/` | dev-loop / 一次性实验 | 模拟器、资产管线等开发工具 | 当正式实现面（实验代码须由 dev-loop 包接管正式化） |
| `ESP32-S3-RLCD-4.2-Demo/**` | 只读参考 | 官方 Demo + 小智源码（第三方，不入版本控制） | 任何写入 |
| `.claude/` | 平台 | 治理机制：agents / rules / hooks / scope-map / settings / scripts / references | 用 Bash 重定向写（bashDenyWrite 红线） |
| `.codebase-memory/` | codebase-memory-mcp 工具（自动生成） | 图索引快照 artifact（`graph.db.zst` + `artifact.json`）与工具自写 `.gitattributes`（`merge=ours` 消二进制冲突）——worktree 从共享快照引导图索引、替代从头 full index | 手改（工具生成派生快照、非数据/结构 SSOT；刷新走 `index_repository(persistence=true)`）。**图索引项目归属（2026-09-05 消歧，ARQ-260905-worktree-graph-index-rules）**：主 checkout 用主索引 `C-AI-KidPal`；**每个 Pod worktree 必须有自己的专属图索引，属 launch 面必办项**——worktree 准备阶段在 bootstrap 后跑一次 `index_repository(repo_path=<worktree>)`，即从仓内快照导入 + 增量补差（快照 2026-09-05 入仓，commit b7aa3df；项目名由工具按路径派生，先例 `C-AI-KidPal-pod-gateway-directionB` / `C-AI-KidPal-pod-host-test-infra`），不得让 Pod 降级「无专属图索引、读文件」；建前先以 `list_projects` 核对既有项目在册状态，历史文档 / 启动卡中的降级注记不作为依据 |
| `delivery/milestones/` | 主线程 | 里程碑注册表：`milestones.jsonl`（单一事实源，state 由人/integrator 维护）+ 脚本再生 `STATUS.md` + `delivery-workspace.json` 绑定元数据 | 手改派生件 STATUS.md（改状态去改 milestones.jsonl 再重跑脚本） |
| `delivery/workstreams/` | 人 · 主线程 planner · dev-loop | workstream 级长周期工作记忆（为什么/发现/进展），准入判据与生命周期治理见 [delivery/workstreams/README.md](../../delivery/workstreams/README.md)；活跃在顶层、完结归 `_archive/<年>/` | 违反准入判据建目录（半年后接手的 agent 靠 diff + taskpackage + commit 能继续的不配建） |
| `delivery/taskpackages/` | dev-loop | dev-loop 包的全程唯一家：生前 plan / briefs / verify 证据 + 身后 review-packet / supervisor-verdict / pod-retro / docs-delta 等收尾制品（含 `current/` 活工作面），一包一目录读齐 | Bash 重定向写（scope-map bashDenyWrite 红线） |
| `delivery/review/**` | review-loop | 架构 review 证据：PROPOSAL / landing-summary / verdict / retro | 当契约 SSOT 用 |
| `delivery/intake/**` | 主线程（跨面 intake 落盘） | spine 制品：`delivery/intake/<changeId>.md`（触面 ≥2 的变更入口记录；程序 SSOT = `.claude/references/change-intake.md`）+ `<changeId>.design.md` companion 设计稿（T4 需要判据见 change-intake.md） | 当各面门制品的家（各面制品落各自既有家，spine 只记 anchors）；手改各面 anchors 以外的历史字段 |
| `delivery/retro/**` | 主线程（loop-retro cadence） | cadence retro 制品归档（loop-retro 产出） | 当 live 工作区用；当各面门制品用 |
| `delivery/archive/**` | 治理（主线程） | 已解散布局的历史整树归档（旧 `changes/` 伞，261015-gov-review-changes-taxonomy T6 归档；路标 = `delivery/archive/README.md` 旧→新映射） | 归档内任何改写（append-only 纪律；历史批重启 = 新立 changeId 引旧批） |

## 变更制品生命周期三层语义（261015-gov-review-changes-taxonomy T2 立）

> 本节回答三个根本问题：「一个东西算诉求、证据还是复盘？」「主动提的与发现的问题按什么分家？」「一个 idea 从冒头到结项，讨论过程记录在哪？」——顶层地图行各自只答「放什么」，本节把目录串成一条生命周期线。

**三层语义模型**：

- **诉求层**（发现的问题与在途变更路由）= `docs/debt/`（发现的问题·暂缓账，一条目一文件 + 状态机）+ `delivery/intake/`（在途变更路由账，spine 一变更一文件）。
- **证据层**（变更走完后的批证据）= `delivery/review/`（需求 / 架构+契约 / 治理三面 loop 的批证据：PROPOSAL / 评审记录 / landing-summary / verdict / retro）+ `delivery/taskpackages/`（dev-loop 包全程唯一家）+ `docs/requirements.md` §9（需求面定稿与评审留痕，设计如此，锚 `requirement-change.md` :47）。
- **复盘层**（周期性收敛与长周期记忆）= `delivery/retro/`（cadence retro 制品）+ `delivery/workstreams/`（跨包长周期工作记忆）。

**触发源判据条文（成文，防回摆）**：「触发源（主动发起 / 发现的问题）= 元数据，不作目录轴」——spine `sources`（human 原话 + refs）、TaskPackage `sourceType` + `sourceRefs`、debt 条目 frontmatter（域 + severity + trigger）各自承载触发源。**理由**：进 loop 后制品同构（主动与发现的批走同一套 Track B 七步、产同一套制品），且主动/被动边界天然模糊（一个发现的 bug 常牵出主动改进），按目录分轴必制造双头记账与搬家成本（dev lane 撤销 + 261015 批 T1 残件即代价实证）。

**idea 记录链纵向地图**（一个 idea 从冒头到结项的记录在哪）：

| 阶段 | 记录落点 |
|---|---|
| 冒头暂缓 | `docs/debt/<域>/<ID>.md` 条目正文（append-only） |
| 在途讨论 | spine `sources`（human 原话逐字）+ `openQ`（问答通道，裁定 + 依据留痕） |
| 成形设计 | `delivery/intake/<changeId>.design.md` companion 设计稿（需要判据见 change-intake.md T4 节） |
| 推进状态 | spine `facePlan` / `anchors` / `status`（投影 = `delivery/intake/INDEX.md`） |

**比例化原则**：留裁定 + 依据，不留推演全程 transcript（human 2026-10-15 裁定③）。

## docs/ 根白名单（唯一可直接放在 docs/ 顶层的东西）

文件：`requirements.md`（需求面 SSOT）· `architecture.md`（架构面 SSOT）· `DEV-GOVERNANCE.md` · `glossary.md`（术语表，已建）——**仅此四份**（2026-10-19 human 裁 docs 根瘦身，261019-gov-review-docs-root-ledger-rehome 方案甲：`ARCH-REVIEW-QUEUE.md` 指路 stub 与 `CONTRACT-DECISION-QUEUE.md` 已删除、内容并入 `docs/debt/`；`ADR.md` 已迁 `docs/decisions/ADR.md`）
子目录：`contracts/` · `notes/` · `ui/` · `templates/` · `archive/`（旧 spec 归档于 `archive/spec.md`）· `debt/`（未结决策项账本，见上表行）· `decisions/`（ADR 架构决策记录，261019 批自 docs 根迁入）。show-me 类一次性视觉解释稿不在 docs 内——2026-10-19 human 直接指示迁至根级 `notes/showme/`（历史曾为 `docs/showme/`，a89f3a6 立）。

### 四种同名物消歧（260923-gov-review-arq-ssot-restructure T6 钉死，261015 批 T5 增 intake 列；仓内同名 `INDEX.md` 现共 10 份且随归档年增长，`find . -name INDEX.md` 可枚举）

| 路径 | 索引什么 | 划分轴 | 谁写 |
|---|---|---|---|
| `delivery/review/INDEX.md` + `delivery/taskpackages/INDEX.md` | **变更批次的落地状态**（changeId / 日期 / draft→frozen→landed / 已合main） | **lane**（Pod 合并流 vs gov 合并流） | `changes-index.mjs` 生成 |
| `delivery/intake/INDEX.md` | **在途变更路由状态**（spine 的 changeId / 触面 / status / anchors 回填 / 漂移标） | **lane**（intake 投影，检测不拦截） | `changes-index.mjs` 生成 |
| `docs/debt/{gov,tech,requirement,contract}/INDEX.md`（+ 各域 `_archive/<年>/INDEX.md`） | **未结决策项**（未结清单 + 已结指针/薄行） | **域**（写者/裁定线不同） | `debt-render.mjs` 生成 |
| `delivery/workstreams/INDEX.md` | workstream 清单 | — | hook 自动生成（编辑 workstreams/ 即重建），勿手改 |

> **引用纪律**：任何引用 `INDEX.md` **必须带目录**（写 `docs/debt/gov/INDEX.md` 或 `delivery/review/INDEX.md`），**禁裸写 `INDEX.md`**——裸写在本仓是歧义的。

### 语义说明：`docs/` 的边界在本批扩了一次（有意识动作）

`docs/` 原定义 = 「仅白名单内的**正式顶层文档**」+ 五类子目录（契约 / 经验 / 设计 / 模板 / 归档）；现共六类——260923 批增账本子目录（时名 `crq`，261014-gov-review-crq-debt-rename 批改名 `debt/`）；曾有的第七类 `showme/`（并行批 a89f3a6，human 直接指示增）已于 2026-10-19 human 直接指示迁出至根级 `notes/showme/`，docs 白名单随之收回。账本子目录的加入使 docs **同时承接一个会持续增长的记录库**（首日 94 条目 + 生成 index〔主 4 + 归档年，随归档年增长〕，只增不减）。这是**有意识的边界扩张**，不是顺手放东西：账本原本就住 `docs/`（`ARCH-REVIEW-QUEUE.md` / `CONTRACT-DECISION-QUEUE.md` 都是 docs 根文件——两队列文件 261019 批删除，内容与决策账全量并入 `docs/debt/`；ADR 同批迁 `docs/decisions/`），铺开进 docs 是**原地铺开**而非换树。**复议触发 = docs 子目录数再增两个，或出现第二个记录库**——届时重新考虑 docs 的边界定义。

> 其它一律不进 docs/ 根。要加新的正式顶层文档 / 类别 → 先登记进 [.claude/scope-map.json](../scope-map.json) 的 `universal.docsTopLevel` 和本表（有意识动作），钩子才放行。

## 工作产物去哪

- 治理 / 架构 / 契约面变更过程产物 → `delivery/review/<change-id>/`（PROPOSAL + landing-summary + verdict + retro 四件；轻路径批 = 瘦制品五件）。入口 = `/arch-change`（架构+契约面）、`/gov-change`（治理面）。
- dev-loop 过程产物 → `delivery/taskpackages/<change-id>/`（包全程唯一家：生前 plan / briefs / verify 证据与身后收尾制品同目录，一包一目录读齐；晋升环节已由 261014-gov-review-dev-lane-merge-taskpackages 撤销）。
- 模拟器/管线的构建产物 → 不进仓（`.gitignore` 已覆盖 `tools/**/node_modules/`、模拟器 Output）。
