# 治理体系迁移指南 —— 把本仓的 Dev Loop OS 搬到新项目

> 用途：将来有一个新项目（任意技术栈），如何复用本仓沉淀的治理体系（角色编制 + 双 loop 流程 + 机械强制层）。
> 读者：负责给新项目搭治理的主线程 / architect。
> 前置：已读过 `AGENTS.md`、`docs/DEV-GOVERNANCE.md`、`.claude/references/dev-loop-operating-system.md`。
> 性质：本文是**操作指南**（G3 流程参考），不是契约。新项目落地时按需裁剪。

---

## 0. 一句话结论

**可移植的是「内核」，不是「现状」。** 把本仓当成一个**治理脚手架模板**，而不是一个可以直接 `cp -r` 的整体。真正能搬的是一个比本仓现状小得多的内核；其余是本仓特有的业务记账与栈绑定，搬到新项目就是噪音。

---

## 1. 三层资产盘点：什么能搬、什么要改、什么必须重写

新项目落地前，先把本仓资产按「可移植性」分三层。下表是总览，§4 给出逐件清单。

| 层 | 内容 | 可移植性 | 工作量 |
|---|---|---|---|
| **A. 治理内核** | 角色编制（agents）+ 双 loop 流程（references）+ 铁律 + scope/契约/合并三闸的**结构** | ✅ 直接搬，全局替换项目名 | 小 |
| **B. 栈绑定机械层** | `scope-map.json` 的路径、`verify-gates.json` 的命令、`unit-run/acceptance-run/prisma` 等测试闸 | ⚠️ 结构可搬，内容须按目标栈重写 | 中 |
| **C. 本仓业务记账** | Tier-2 schema 克隆体、K9/K1 spike 记账、S1/S2 系统边界、ARQ 溯源注记、各系统 spec/contracts | ❌ 不可搬 | —（留在本仓） |

**关键认知**：`scope-map.json` 里约 80% 的内容是本仓的业务/历史记账（K9 spike、K1 session、Tier-2 克隆体、ARQ 编号溯源）。新项目的 scope-map 应该**从一张干净的白名单骨架长出来**，而不是从本仓这张删改。这是迁移最容易踩的坑——把别人的历史当成自己的起点。

---

## 2. 三档采用模型：按项目阶段启用，不一刀切

从 ESP32 案例（spec 草稿期、代码未写）学到的最重要一条：**治理强度应匹配项目阶段**。不要一上来全搬。

| 档 | 适用阶段 | 启用内容 | 对应 ESP32 现状 |
|---|---|---|---|
| **档 1 · Spec 治理** | 还在写 spec / 设计，代码未动 | 契约面定义 + arch-review 链（review-moderator / scenario-author / challenger / defender / adjudicator）+ `docs/DEV-GOVERNANCE.md` + Track B 治理路由 | ✅ ESP32 当前就停在这档 |
| **档 2 · 单切片开发** | 开始写第一个 feature | 档 1 + dev-loop 链（acceptance-author / builder / fixer / verifier→landing-verifier / integrator / supervisor）+ scope 闸 + 测试闸 | ESP32 动工时升档 |
| **档 3 · 多 Pod 编排** | 多系统 / 多切片并行 | 档 2 + portfolio-planner（RTE）+ worktree 协调 + merge token | ESP32 暂不需要 |

**落地建议**：新项目从档 1 起步，把 spec 冻结流程跑顺了再升档 2。档 3 等真有多切片并行需求再引入。

---

## 3. 参数化清单：本仓写死了什么，新项目要「填空」什么

本仓脚本不是为可移植设计的——117 处（`.claude/scripts/`）+ 101 处（`.claude/hooks/`）出现 `apps/`、`packages/`、`contracts/`、`pnpm`、`vitest`、`prisma` 等写死项。迁移的本质是**把这些假设收敛成一张可填空的表**。

新项目落地时，逐项替换以下「参数」：

| # | 参数 | 本仓取值 | 新项目要填 | 影响哪些文件 |
|---|---|---|---|---|
| P1 | **源码边界** | `apps/sX-*`、`packages/*` | 目标的源码目录（ESP32：`main/`、`shim/`） | scope-map、enforce-scope、所有 dev-loop agent |
| P2 | **契约面** | `contracts/**` | 目标的契约文件（ESP32：`docs/interface-contract.md`） | scope-map、architect-reviewer、契约门 hooks |
| P3 | **测试命令** | `vitest`、`pnpm test`、Playwright、Prisma | 目标的测试运行器（ESP32：`idf.py unity-app`、`pytest`） | verify-gates.json、unit-run-check、acceptance-run-check |
| P4 | **包管理/构建** | `pnpm` workspace | 目标构建（ESP32：`idf.py build`） | verify-gates、worktree-bootstrap |
| P5 | **文档骨架** | `docs/kernel`、`docs/systems`、`docs/templates` | 目标的 spec 目录结构 | scope-map universal.docsTopLevel |
| P6 | **变更证据目录** | `changes/arch/*`、`changes/dev/*` | 可保持同名（语言无关） | changes-index、merge-driver、pod-events |
| P7 | **项目名 / 图索引名** | `C-AI-AInativecompany` | 新项目名（**必须与目录名一致**，否则图工具 project not found） | CodebaseMemory 索引、AGENTS.md marker |

> ⚠️ **P7 是本仓已踩过的坑**：worktree / 新仓建图索引时，项目名 = 目录名，否则 `project not found`。见 auto-memory `worktree-sibling-directory`。

---

## 4. 逐件迁移清单

### 4.1 直接搬（档 1 就需要）

| 源路径 | 处置 |
|---|---|
| `.claude/agents/` 全部 16 个 | 复制。逐文件把「`apps/**`」「`contracts/**`」等本仓路径替换为 P1/P2 |
| `.claude/references/` | 复制以下流程性文件：`dev-loop-operating-system.md`、`dev-loop.md`、`pod-coordination.md`、`arch-review-protocol.md`、`arch-review-rubric.md`、`arch-scenarios.md`、`change-triage.md`、`error-envelope.md`、`escalation-playbook.md`、`acceptance-evidence.md`、`taskpackage-authoring.md`、`task-intake-and-slicing.md`、`minimal-implementation.md`、`track-b-judge-brief.md`、`track-b-verifier-brief.md`、`plain-file-handoff.md` |
| `AGENTS.md` + `CLAUDE.md` | 复制骨架，重写「Default Mode / 治理路由 / Hard Rules」三节的路径引用；**CodebaseMemory marker 块**保留但改项目名 |
| `docs/DEV-GOVERNANCE.md` | 复制，作为新项目的治理 SSOT 起点 |
| `docs/templates/` | 复制 `PROPOSAL.template.md`、`retro.template.md`、`system-spec.template.md`、`kernel-spec.template.md`、`AGENTS.template.md` |

### 4.2 结构搬、内容重写（档 2 需要）

| 源路径 | 处置 |
|---|---|
| `.claude/scope-map.json` | **只搬骨架**：universal.denyCreate / bashDenyWrite / docsTopLevel + 每个 agent 的 allow/deny **结构**。清空本仓的 Tier-2 克隆体、K9/K1 记账。按 P1/P2/P5 重填路径 |
| `.claude/verify-gates.json` | 搬结构，按 P3/P4 重写测试/构建命令 |
| `.claude/hooks/enforce-scope.mjs`、`deny-main-push.mjs`、`commit-safety.mjs`、`supervisor-gate-check.mjs`、`supervisor-verdict-gate.mjs`、`verify.mjs` | 复制。`enforce-scope.mjs` 跟随新 scope-map；其余基本语言无关 |
| `.claude/scripts/` 流程性脚本 | 复制：`changes-index.mjs`、`merge-driver.mjs`、`pod-events-append.mjs`、`pod-events-aggregate.mjs`、`journal-append.mjs`、`dispatch-stamp.mjs`、`landing-fidelity-check.mjs`、`retro-staleness-check.mjs`、`proposal-consolidation-check.mjs`、`closeout-check.mjs`、`docs-sync-check.mjs` |
| `.claude/dev-loop.settings.json`、`review-loop.settings.json`、`launch.json` | 复制，重写 `permissions.deny` 里的路径为 P1/P2 |

### 4.3 不搬（本仓特有 / 栈专属）

| 源路径 | 原因 |
|---|---|
| `contracts/**` | 本仓业务契约。新项目的契约须按自己的域重写 |
| `docs/systems/**`、`docs/kernel/**` 各系统 spec | 本仓业务 spec |
| `.claude/scripts/prisma-validate-gate.mjs`、`mutation-check.mjs` | Prisma 专属 |
| `.claude/scripts/unit-run-check.mjs`、`acceptance-run-check.mjs`、`acceptance-coverage-check.mjs` 的 vitest 逻辑 | 栈绑定，按 P3 重写 |
| scope-map 的 Tier-2 schema 克隆体、K9/K1/S1/S2 条目 | 本仓业务记账 |
| `apps/**`、`packages/**` | 本仓实现 |

---

## 5. 新项目落地七步（操作清单）

以「档 1 起步、栈已定」为前提：

1. **定 P1–P7 参数**：先在一张表里填好新项目的源码边界、契约面、测试命令、构建命令、文档骨架、项目名。**这一步不定，后面全是无的放矢。**
2. **建仓 + 文档骨架**：按 P5 建 `docs/` 骨架；放 `AGENTS.md`/`CLAUDE.md`/`docs/DEV-GOVERNANCE.md`/`docs/templates/`。
3. **搬治理内核**：复制 agents + references（§4.1），全局替换项目名与路径。
4. **定义契约面**：明确新项目的「契约」是什么文件（ESP32 = `docs/interface-contract.md`），登记进 scope-map 的契约门 + `bashDenyWrite`。这一步决定契约门能不能生效。
5. **建 scope-map 骨架**：从干净白名单骨架填 P1/P2/P5；**vendor 代码（如 ESP32 的官方 Demo 树）显式 deny + 排除出 scope 校验与图索引**。
6. **配机械闸**：装 hooks + 流程性脚本（§4.2）；档 2 时再按 P3/P4 配 verify-gates 与测试闸。
7. **建图索引**：对新仓跑 CodebaseMemory 索引，项目名 = 目录名；把索引进 AGENTS.md marker 块。

> 升档触发：spec 冻结、要动工 → 补档 2（dev-loop 链 + 测试闸）；出现多切片并行 → 补档 3（portfolio-planner + worktree 协调）。

---

## 6. ESP32 落地速查（本指南的第一个实例）

| 参数 | ESP32 取值 |
|---|---|
| P1 源码边界 | `main/`（固件业务）、`main/boards/**`（板级）、`shim/`（云端 FastAPI） |
| P2 契约面 | `docs/contracts/interface-contract.md`（笔顺 JSON / display 块 / 课表 MD / shim API；2026-08-27 归位） |
| P3 测试命令 | 固件 `idf.py unity-app`（Unity）；云端 `pytest`（shim）；硬件相关验收标「需实机」 |
| P4 构建 | `idf.py build` |
| P5 文档骨架 | `docs/`（systems/kidpal + contracts + archive；迁移时初稿位置已归位） |
| P7 项目名 | 目录名 `ESP32` → 图索引项目名须一致 |
| vendor 排除 | `ESP32-S3-RLCD-4.2-Demo/`（官方 Demo + 小智 V2.1.0）→ 只读，deny 写，不进 scope 校验与图索引 |
| 当前档位 | **档 1**（spec v0.1 草稿）——先跑 arch-review 冻结 spec，动工再升档 2 |
| 铁律映射 | 本仓「主 AI 不直改 `apps/**`」→ ESP32「主 AI 不直改 `main/`+`shim/`，一切走 dev-loop」 |

---

## 7. 已知残余风险 / 待办

- **测试闸重写是档 2 的主要工作量**：`unit-run-check`/`acceptance-run-check` 深度绑 vitest，换栈时基本是重写而非改参数。考虑将来把「测试运行器」抽成一个可插拔配置项。
- **硬件项目没有「跑测试即绿」**：ESP32 大量验收需实机（AEC 喊停调阈值是最大技术风险点）。机械闸只能覆盖可自动化的部分，实机验收要在 acceptance 里显式标「需人工实机核验」，不能假装跑测试就过。
- **本指南本身的生命周期**：它是一次性整理。若本仓治理体系演进（如新增角色、改路由），本指南不会自动同步。重大演进后应回来核对 §4 清单。
