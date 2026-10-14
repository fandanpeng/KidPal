# 治理体系迁移指南 —— 把本仓的开发治理搬到任何新项目

> 用途：将来有一个新项目（任意技术栈），如何复用本仓沉淀的治理体系（角色编制 + 双 loop 流程 + 机械强制层）。
> 读者：负责给新项目搭治理的主线程 / architect。
> 前置：已读过 `AGENTS.md`、`docs/DEV-GOVERNANCE.md`、`.claude/references/dev-loop-operating-system.md`。
> 性质：本文是**操作指南**（G3 流程参考），不是契约。新项目落地时按需裁剪。
> 版本：v1.1（2026-08-27）——经一次完整「档 1」实测迁移后修订，§6 踩坑清单为实战回收，非推演。

---

## 0. 一句话结论

**可移植的是「内核」，不是「现状」。** 把本仓当成一个**治理脚手架模板**，而不是一个可以直接 `cp -r` 的整体。真正能搬的是一个比本仓现状小得多的内核；其余是本仓特有的业务记账与栈绑定，搬到新项目就是噪音。

---

## 1. 三层资产盘点：什么能搬、什么要改、什么必须重写

新项目落地前，先把本仓资产按「可移植性」分三层。下表是总览，§4 给出逐件清单。

| 层 | 内容 | 可移植性 | 工作量 |
|---|---|---|---|
| **A. 治理内核** | 角色编制（agents）+ 双 loop 流程（references）+ 铁律 + scope/契约/合并三个机械闸的**结构** | ✅ 直接搬，全局替换项目名 | 小 |
| **B. 栈绑定机械层** | `scope-map.json` 的路径、`verify-gates.json` 的命令、`unit-run/acceptance-run/prisma` 等测试闸 | ⚠️ 结构可搬，内容须按目标栈重写 | 中 |
| **C. 本仓业务记账** | Tier-2 schema 克隆体、K9/K1 spike 记账、S1/S2 系统边界、ARQ 溯源注记、各系统 spec/contracts | ❌ 不可搬 | —（留在本仓） |

**关键认知**：`scope-map.json` 里约 80% 的内容是本仓的业务/历史记账（K9 spike、K1 session、Tier-2 克隆体、ARQ 编号溯源）。新项目的 scope-map 应该**从一张干净的白名单骨架长出来**，而不是从本仓这张删改。这是迁移最容易踩的坑——把别人的历史当成自己的起点。

---

## 2. 三档采用模型：按项目阶段启用，不一刀切

**治理强度应匹配项目阶段**，不要一上来全搬：

| 档 | 适用阶段 | 启用内容 |
|---|---|---|
| **档 1 · Spec 治理** | 还在写 spec / 设计，代码未动 | 契约面定义 + arch-review 对抗链（review-moderator / scenario-author / challenger / defender / adjudicator + architect / architect-reviewer / landing-verifier）+ 契约门 + Track B 治理路由 + scope 闸 / deny-main-push 钩子 |
| **档 2 · 单切片开发** | 开始写第一个 feature | 档 1 + dev-loop 全链（planner / acceptance-author / builder / fixer / verifier / integrator / supervisor 闸）+ `/build-feature` / `/triage-change` 入口 + 测试闸（verify-gates 按 P3/P4 重写）+ codebase-memory 图索引 |
| **档 3 · 多 Pod 编排** | 多系统 / 多切片并行 | 档 2 + portfolio-planner（RTE）+ worktree 协调 + merge token + pod-coordination |

**落地建议**：新项目从档 1 起步，把 spec 冻结流程跑顺了再升档 2。档 3 等真有多切片并行需求再引入。升档触发是「事件」不是日期：spec 冻结待动工 → 升档 2；出现第二个并行切片 → 升档 3。

---

## 3. 参数化清单：本仓写死了什么，新项目要「填空」什么

本仓脚本不是为可移植设计的——117 处（`.claude/scripts/`）+ 101 处（`.claude/hooks/`）出现 `apps/`、`packages/`、`contracts/`、`pnpm`、`vitest`、`prisma` 等写死项。迁移的本质是**把这些假设收敛成一张可填空的表**。动手前先把这张表填好：

| # | 参数 | 本仓取值 | 新项目要填 | 影响哪些文件 |
|---|---|---|---|---|
| P1 | **源码边界** | `apps/sX-*`、`packages/*` | 目标的源码目录集合 | scope-map、enforce-scope、所有 dev-loop agent |
| P2 | **契约面** | `contracts/**` | 目标的契约文件 / 目录（可以是 ts 契约包，也可以是 markdown 协议文档） | scope-map、settings.deny、architect-reviewer、契约门 hooks |
| P3 | **测试命令** | `vitest`、`pnpm test`、Playwright、Prisma | 目标的测试运行器 | verify-gates.json、unit-run-check、acceptance-run-check |
| P4 | **包管理/构建** | `pnpm` workspace | 目标构建体系 | verify-gates、worktree-bootstrap |
| P5 | **文档骨架** | `docs/kernel`、`docs/systems`、`docs/templates` | 目标的 spec 目录结构（单系统项目可为 `docs/systems/<产品名>/`，无需 kernel 层则并入 systems） | scope-map universal.docsTopLevel、agent 定义里的设计面表述 |
| P6 | **变更证据目录** | `delivery/changes/arch/*`、`delivery/taskpackages/*` | 可保持同名（语言无关） | changes-index、merge-driver、pod-events |
| P7 | **项目名 / 图索引名** | `C-AI-AInativecompany` | 新项目名（**必须与目录名一致**，否则图工具报 project not found——本仓已实证踩过） | CodebaseMemory 索引、AGENTS.md marker、skills 内 CLI 示例 |

> ⚠️ **P2 是最容易被低估的一项**：契约面的物理形态由目标项目决定。「契约」不一定是一个 TypeScript 包——对一个端云协议驱动的项目，一页 markdown 格式约定就是它的全部契约。认准「改了必须走人类门的单一事实源」这个语义来选文件，别照抄本仓的目录形状。

---

## 4. 逐件迁移清单

### 4.1 直接搬（档 1 就需要）

| 源路径 | 处置 |
|---|---|
| 对抗链 agents 8 个（architect / architect-reviewer / review-moderator / scenario-author / challenger / defender / adjudicator / landing-verifier） | 复制。逐文件按 P1/P2/P5 替换路径表述（见 §6 隐性假设清单） |
| commands：`arch-change` / `review-arch` / `review-governance`（档 2 再加 `build-feature` / `triage-change` / `loop-retro`） | 复制 + 路径适配 |
| rules：`authoring-agents-skills` / `prompt-authoring` + **按 P2 重写 `contracts.md`**（契约规则强绑契约面形态，必须新写不能复制） | 复制/新写 |
| references 引用闭包 | **grep 驱动收集**（见 §6）：arch 链需要 `arch-review-protocol` / `arch-review-rubric` / `arch-scenarios` / `architect-operating-model` / `architecture-decisions` / `change-triage` / `contract-change-evidence` / `plain-file-handoff` / `track-b-judge-brief` / `track-b-verifier-brief` 等 |
| skills（除本项目专属运维件外基本可全搬） | 复制后 grep 扫一遍命令示例里的项目名/符号名（见 §6） |
| `AGENTS.md` + `CLAUDE.md` | **新写**（骨架参考本仓）：路由节、Hard Rules、vendor 面约定、CodebaseMemory 降级条款。不要复制本仓正文逐段改——本仓版本携带大量 dogfood 历史 |
| `docs/DEV-GOVERNANCE.md` | 复制为起点 + 按档位裁剪：未迁的角色在团队构图表标注「档 N 迁入」，未启用的流程段落如实标注，不删骨架 |
| `docs/templates/` 制品模板 | 复制 PROPOSAL / retro / system-spec / AGENTS / architect-landing-summary 等 |

### 4.2 结构搬、内容重写（视档位）

| 源路径 | 处置 |
|---|---|
| `.claude/scope-map.json` | **只搬结构语义**（universal 三段 + per-agent 边界 shape），内容从空白骨架按 P1/P2/P5 重填。清空本仓全部业务记账 |
| `.claude/settings.json` + `<loop>.settings.json` | **新写**：hooks 注册照抄本仓组合方式；permissions.allow 只收目标项目可复用命令族；loop settings 的 permissions.deny 锁 P2 契约面 |
| 钩子（enforce-scope / deny-main-push / commit-safety） | 已实证对栈零绑定，近乎直搬；`suggest-codebase-memory` 等按需 |
| 治理脚本（changes-index / merge-driver / landing-fidelity-check / proposal-consolidation-check / lint-descriptions / route-regression-check + lib/governance-gate-shared） | 已实证零栈绑定，直搬；跑前 `node --check` 冒烟 |
| `route-fixtures/routing.json` + `surface-alignment.json` | **必须净版重写**，不能复制——fixture 枚举的是「本仓有哪些 command / agent」，照抄会让 route-regression-check 立即崩（实测 ENOENT） |
| 档 2：dev-loop.settings + verify-gates + unit-run/acceptance-run/closeout 等测试闸脚本 | 结构搬、命令按 P3/P4 重写（工作量主要在这里） |

### 4.3 不搬（本仓特有 / 栈专属）

| 源路径 | 原因 |
|---|---|
| `contracts/**`、各系统 spec、`apps/**`、`packages/**` | 本仓业务 |
| `prisma-validate-gate` / `mutation-check` 等 Prisma 专属闸 | 栈绑定，按 P3 找对等物或明示缺口 |
| scope-map 业务记账条目、历史 ARQ 行 | 新仓队列从空收件箱开始 |
| 本项目专属运维 skills | 与目标项目无关 |

---

## 5. 新项目落地八步（操作清单）

以「档 1 起步、栈已定」为前提：

1. **定 P1–P7 参数**：先把 §3 的表填成一张实例表。**这一步不定，后面全是无的放矢。**
2. **建仓 + 文档骨架**：按 P5 建 `docs/` 骨架（systems / contracts / templates / archive）；散件归位一次到位——顶层只留指令根，spec 进 `docs/systems/<系统>/`、契约进契约面、第三方分析笔记进 archive。事后搬家成本高于一步到位（引用是双向的）。
3. **先验依赖闭包，再批量拷贝**：对每个入口 agent/command，grep 它引用的 references / scripts / fixture，收敛出最小闭包集，一次性拷齐；顺手跑栈绑定扫描（grep `apps/|pnpm|vitest|prisma` on hooks/scripts），把资产分成「直搬」vs「重写」vs「声明缺口」三类。
4. **新写指令根与强制层配置**：AGENTS / CLAUDE / DEV-GOVERNANCE（档位裁剪版）/ scope-map / settings×2 / fixtures 净版。
5. **定义契约面并落冻结**：确认 P2 选出的契约文件归位到契约面目录，登记进 scope-map `bashDenyWrite` + loop settings `permissions.deny`；配套 rule 写清楚「影子目录禁令」（防有人另起炉灶建同名平行目录）。
6. **全部文本替换走工程纪律**：不用裸 sed 连环替换（实测多条 `-e` 表达式遇分隔符冲突整批静默跳过）；用 Python 脚本一次执行互斥的替换规则（重叠规则会产生双重替换坏链），替换完全量 grep 验证清零后才算完。
7. **建变更证据骨架 + 冒烟验证**：`delivery/taskpackages/INDEX.md` + `delivery/changes/arch/INDEX.md` 空骨架（INDEX 按 lane 拆分）+ 跑全套冒烟（见 §6.M）。全绿才提交。
8. **建图索引 + 记账收尾**：按 P7 建索引（多分支 worktree 时每支手动建）；为本批写 bootstrap PROPOSAL 记账（见 §6.F），状态钉进 INDEX。

---

## 6. 实战踩坑清单（首次完整迁移迭代回收）

以下各条全部在真实迁移中发生 / 被验证。分六组：

### A. 剪裁决策类

- **先 grep 后定案**：「能不能直搬」不靠判断靠证据——对 hooks / scripts 跑一遍栈关键词扫描，数字说话（本仓实测：3 钩子 + 6 个档 1 脚本对 `apps/|pnpm|vitest|prisma` 零命中，因此直搬；仅 1 个契约检查脚本绑死 Prisma，整体不迁）。
- **机器门缺位要诚实降级，不许假适配**：不适用的机械门（如绑 Prisma 的 spec↔contract 一致性检查）宁可声明「此关降级为人工核对」并登一条 accepted-risk 型 ARQ 追踪补位时机，也不要留一个在新仓根本跑不出真结果的假门。机械层的可信度建立在「跑过就是真的」之上。
- **告警级工具允许带病搬迁**：非阻塞的 lint 类工具（description 长度等）报 warn 不算迁移失败，如实记录即可；blocking 级才是门槛。

### B. 隐性路径假设的四层埋藏点（比想象深）

grep 搜「本仓假设」时要覆盖四个层次，每层都有实锤翻车案例：

1. **结构性路径**（`apps/**`、`contracts/schema/**`）——最容易想到；
2. **工具名**（`prisma validate`、`tsc`、`pnpm test` 出现在 agent 定义的 validationPlan 示例里）——容易漏；
3. **JSON / 输出示例占位符**（agent def 里 `"touchedSurface": ["contracts/schema/**"]` 这类示例块、qualified_name 示例里的项目名前缀）——很容易漏，它们会被模型当范式模仿；
4. **skills 里的命令示例与演示符号**（CLI 用法示例写死原仓绝对路径与项目名、示例 walkthrough 用原仓业务符号）——skills 名义上是纯方法论，实际上凡是带命令/symbol 示例的都埋着原仓坐标。

### C. 文本替换工程纪律

- **禁止裸 sed 连环 `-e`**：一条表达式语法错误 → 整批表达式全部跳过且无报错（实测事故）。批量替换写成一次性 Python 脚本执行，跑完即删。
- **替换规则必须互斥**：两条规则先后命中同一段文本会产生「双重替换坏链」（实测事故：markdown 链接变成 `](x)(x)`）。规则设计时明确顺序与边界，替换后针对「连续闭合括号」之类特征全量复查。
- **替换闭环以 grep 清零为准**：「大概都改了」不算完——对旧路径 / 旧项目名 / 旧工具名的 grep 必须归零（模板文件里刻意保留的除外），残留要能说出理由。
- **引用同步是双向的**：移动一个文件要同步三个方向——①所有指向它的链接；②它自己内部的相对链接深度变化（根相对 → `../../`）；③它自身正文里叙述性的旧路径文字。漏②③是常态。

### D. 配置净版类

- **fixtures 与白名单照抄必崩**：route-fixtures 枚举 command/agent 清单、surface-alignment 列 writeScope 映射——照抄原仓的会让自检脚本立即失败或给出错误覆盖面。这两类文件是「编制清单的投影」，必须按新仓实际编制重写。
- **docsTopLevel 白名单跟着物理现实走**：目录挪走后白名单里的死条目要同步删掉，否则 enforce-scope 在维护一张说谎的地图。
- **vendor 面一等公民对待**：第三方源码树（官方 SDK / demo 工程）显式进 scope-map deny、排除出图索引，并在 AGENTS.md 写明「行为定制在自己的 overlay 做」。不做这步，vendor 代码会淹死 scope 校验与调用图。

### E. 流程记账类

- **迁移本身是元级动作，豁免但要留痕**：建立治理机器的动作无法走七步闭环（机器尚不存在），human 一句授权即落地依据；落地后在 `delivery/changes/arch/<id>/PROPOSAL.md` 补记账，Status 如实标 LANDED-UNREVIEWED 并注 human 授权来源，首个常规批次恢复标准流程。
- **`git mv` 保持 rename 检测**：移动 + 改动混在一步做会掉相似度；mv 后再做内容编辑（或控制编辑幅度），让 git 以高相似度识别 rename，历史可追。

### F. 收尾验证类：迁移完成的最小冒烟套件

以下全绿才算迁移落地（全部在实测中使用过）：

| 检查 | 命令 | 通过判据 |
|---|---|---|
| 脚本语法 | `node --check` 逐一跑全部 hooks/*.mjs、scripts/**/*.mjs | 无 SYNTAX FAIL |
| 配置合法 | node 单行 JSON.parse 各 settings / scope-map / fixtures | 无抛错 |
| 路由不退化 | `node .claude/scripts/route-regression-check.mjs --self-check` | ok 数 > 0 且 0 fail |
| 描述卫生 | `node .claude/scripts/lint-descriptions.mjs` | 仅告警可接受，逐条知悉 |
| 证据索引可用 | `node .claude/scripts/changes-index.mjs` 无参重建 + 对已知目录 `--upsert <id>` | rebuild ok / 幂等 / 对不存在目录正确拒绝 |
| commit 卫生 | staged 后 `git diff --cached --check` | 无冲突标记 / 无 EOF 空行类警告残留 |

---

## 7. 已知残余风险 / 待办

- **测试闸重写是档 2 的主要工作量**：`unit-run-check`/`acceptance-run-check` 深度绑 vitest，换栈时基本是重写而非改参数。考虑将来把「测试运行器」抽成一个可插拔配置项。
- **非纯软件项目的验收没有「跑测试即绿」**：涉及硬件 / 实机 / 物理交互的项目，相当比例验收只能人工实机核验。这类验收必须在 acceptance 制品里显式标注「需人工实机核验」及其具体步骤，绝不能假装自动化通过。
- **本指南的生命周期**：若本仓治理体系演进（新增角色、改路由、脚本变动），本指南不会自动同步。每次用它完成一次迁移后，把新踩的坑回收到 §6——它应当随每次实战迭代。
