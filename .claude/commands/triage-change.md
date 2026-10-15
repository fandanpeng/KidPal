---
description: dev 侧变更分流前置门（对偶 /arch-change、/build-feature）。改 firmware/** 行为的变更开工前先过 architect 一次比例化 review：判四档（③拆包 / ②单包Full / ②单包Lite / ①直接修）→ 人类确认/改判路由 → reviewed plan 按成熟度就近落地（大 feature 落 delivery/workstreams/<ws>/PROPOSAL.md，小 feature/bug 落 delivery/taskpackages/<id>/plan.md）→ 报告下一步命令。
argument-hint: <plan 或想法，例如 plan mode 产出的实现思路，或 "修复线索 SSE 重复行"，或一份 plan 文件路径>
---

# /triage-change

你现在是 **主线程编排者**（人类主线程）。按 `.claude/references/change-triage.md` 的程序推进。
这是 **dev 侧变更分流前置门**，对偶事实源侧的 `/arch-change` 与实现侧的 `/build-feature`：改 `firmware/**` 行为的变更（大 feature / 小 feature / bug 不分入口）开工前先过 architect 一次**比例化 review**，判它走哪档、把 reviewed plan 落到它的家。

- **门性质**：Part-2 **工程门 / bootstrap 临时信任门**（权威在 K9 §11），**不是业务门、不进 DEV-GOVERNANCE §5 六门清单**；与业务运行时 6 门无涉。
- **你编排但落盘归你（主线程）**：判断归被你派发的 `architect`（只读+审+返回裁决，零写入）；**落盘归你这个主线程**（主线程不受 per-agent enforce-scope 限，dev-loop.md §4）。作者≠判官：architect 审+起草，FROZEN 归人类。
- **四面探测（intake，程序 SSOT 见 `change-intake.md`）**：变更进来先做跨面探测——对照 requirements / architecture / contracts + debt 未结账 + governance 面逐面查触面事实（枚举以 SSOT 为准），主线程落盘 spine（触面 ≥2 时）；命中触面事实（触 `docs/contracts/**` 或跨系统结构性）→ 按 facePlan 把该变更的架构/契约面路由 `/arch-change` loop，实现面留本门分档，不再整单踢走；纯实现面变更 → 照旧判四档。

本次输入 plan：

> $ARGUMENTS

逐步执行，并在每一步显式说明「派给谁、为什么、期望产物」：

1. **四面探测（intake）**：读 plan/idea，按 `change-intake.md` 程序做跨面探测——触面 ≥2 时主线程落盘 spine；命中触面事实（触 SSOT / 跨系统结构性）→ 按 facePlan 路由（架构/契约面走 `/arch-change` loop，本门继续承接实现面），不再整单踢走；纯实现面 → 继续。
2. **派 architect 比例化 review**：派 `architect`（只读+审），按 `change-triage.md` 判四档 + 出 reviewed plan。architect **只审 + 返回**（同会话 return，非跨会话 prose），**不写任何文件**。
   - 大 feature（③拆包）：实质单轮 review（findings 逐条 + 处置），生成 `delivery/workstreams/<ws>/review.md` 内容（findings + architect sign-off）。
   - 小 feature/bug（②单包）：快审（确认判据 + 路由裁决），裁决写进 `plan.md` frontmatter，不单独成件。
   - **逃生门（reviewer-first）**：review 中发现疑似触契约/spec/agent 定义/gate → **先派 `architect-reviewer` 四分类** → `ACCOMPANIES_SPEC_CHANGE`/`SPEC_CONTRACT_DRIFT`/`CONTRACT_CHANGE_REQUIRED` 转 `/arch-change` loop（契约变更 = 架构变更的一种；本门无权批准这些）。
3. **人类触点①（确认/改判路由）**：用 AskUserQuestion 呈现 architect 的四档裁决，人类一句话确认或改判（如 Lite→Full）。**落盘前必须过这一关**——Lite/Full 是安全相关判断，architect 误判需人类 backstop。
4. **主线程落盘（只搬运 architect 的 reviewed plan + 裁决，不改内容）**：按确认后的档位把 reviewed plan 落进它的家：
   - **③大 feature** → `delivery/workstreams/<ws>/PROPOSAL.md`（+ `review.md`），frontmatter `freezeState: pending-human`。**按 `docs/templates/WORKSTREAM-PROPOSAL.template.md`（三问 + 分型模板）起草**——业务目的清单 / `workType` 数组 / spec 关系标注三项硬项缺失，会在 portfolio-planner intake 被 fail-closed 打回（260912 拆包链 T1）。`delivery/workstreams/<ws>/` 由你（人类/主线程）创建——这是「设计权威落家」（对 ARQ-065/066 RTE 集权的有意放宽，见 change-triage.md §F4）；portfolio-planner 后续拆包时才建调度态。
   - **②单包** → `delivery/taskpackages/<id>/plan.md`，frontmatter 含 `rigor`/`riskTier`/`sourceType`/`reviewed-by`/`date`。**建包时按 `task-intake-and-slicing.md` §§ origin/changeId 创建 taskpackage.json + 填入 origin（`upstreamType`/`upstreamRef`/`upstreamFrozen`/`workstream`）+ 注入 `YYMMDD-` 日期前缀 changeId + 写 `planRef` 指回 sibling `plan.md`**（建包方职责经 reference 下达、经既有委派链承接；gov-review-devloop-artifact-lifecycle DP-5）。
   - **①直接修** → 不落盘（地板：不改 apps 行为 / 人类明确跳过）。
5. **人类触点②（仅大 feature，冻结）**：人类盖 FROZEN（版本/sha）。**作者≠判官：architect 不能自冻**（scope 机器兜住——architect allow 不含 `delivery/workstreams`）。portfolio-planner 只拆 FROZEN 的 PROPOSAL。
6. **报告下一步（runbook，人类触点③）**：按 facePlan 顺序打出各面执行命令——轻面注明内联执行（本会话继续派发），重面给出 headless 拉起命令（含显式 `--agent`、匹配 settings 与 changeId）；下游会话开场读 spine，不要求 human 重述意图；单包/拆包的下一步命令映射（②单包 → `planner` 跑 `/build-feature <id>`（从 `delivery/taskpackages/<id>/plan.md` 读 reviewed plan 做 intake）；③大 feature → `portfolio-planner`（指 `<ws>`，从 `delivery/workstreams/<ws>/PROPOSAL.md` 拆包））保留为 runbook 条目。
   headless 拉起的是完整顶层会话（非子 agent），「子 agent 不能再派子 agent」约束不变；关键人类门（触点①②等）human 仍必须在场——headless 到门即停，纪律见 `change-intake.md` 三铁律。

> **convention-only 不变量**（本门当前无机器牙齿，靠约定 + 多层人工兜底，机器守卫记 ARQ 后续 G3，详见 change-triage.md）：主线程只搬运 architect 的 reviewed plan、不改内容；单向守卫（riskTier overrides rigor）靠人类触点① + supervisor riskTier 升级兜底；FROZEN 拆包门靠 portfolio-planner 自觉；plan.md planner 只读消费、改需留痕。
> 你不改 `docs/contracts/**`、不改 L0 蓝图、不直推 main；触事实源一律升级 `/arch-change`。
