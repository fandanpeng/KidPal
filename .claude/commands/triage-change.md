---
description: dev 侧变更分流前置门（对偶 /arch-change、/build-feature）。改 firmware/** 行为的变更开工前先过 architect 一次比例化 review：判四档（③拆包 / ②单包Full / ②单包Lite / ①直接修）→ 人类确认/改判路由 → reviewed plan 按成熟度就近落地（大 feature 落 .planning/<ws>/PROPOSAL.md，小 feature/bug 落 taskpackages/<id>/plan.md）→ 报告下一步命令。
argument-hint: <plan 或想法，例如 plan mode 产出的实现思路，或 "修复线索 SSE 重复行"，或一份 plan 文件路径>
---

# /triage-change

你现在是 **主线程编排者**（人类主线程）。按 `.claude/references/change-triage.md` 的程序推进。
这是 **dev 侧变更分流前置门**，对偶事实源侧的 `/arch-change` 与实现侧的 `/build-feature`：改 `firmware/**` 行为的变更（大 feature / 小 feature / bug 不分入口）开工前先过 architect 一次**比例化 review**，判它走哪档、把 reviewed plan 落到它的家。

- **门性质**：Part-2 **工程门 / bootstrap 临时信任门**（权威在 K9 §11），**不是业务门、不进 DEV-GOVERNANCE §5 六门清单**；与业务运行时 6 门无涉。
- **你编排但落盘归你（主线程）**：判断归被你派发的 `architect`（只读+审+返回裁决，零写入）；**落盘归你这个主线程**（主线程不受 per-agent enforce-scope 限，dev-loop.md §4）。作者≠判官：architect 审+起草，FROZEN 归人类。
- **与 `/arch-change` 的分区（第一刀）**：变更进来先问**碰不碰单一事实源**（`docs/contracts/**` / kernel·systems spec / `.claude/agents` 定义 / gate）——碰 → 跟大小无关，一律直走 `/arch-change`，**根本不进本门**；不碰（纯 firmware/server/docs）→ 进本门由 architect 判四档。

本次输入 plan：

> $ARGUMENTS

逐步执行，并在每一步显式说明「派给谁、为什么、期望产物」：

1. **第一刀（事实源分区）**：读 plan，判它碰不碰单一事实源。碰 → 告知用户直走 `/arch-change`，本门收尾。不碰 → 继续。
2. **派 architect 比例化 review**：派 `architect`（只读+审），按 `change-triage.md` 判四档 + 出 reviewed plan。architect **只审 + 返回**（同会话 return，非跨会话 prose），**不写任何文件**。
   - 大 feature（③拆包）：实质单轮 review（findings 逐条 + 处置），生成 `.planning/<ws>/review.md` 内容（findings + architect sign-off）。
   - 小 feature/bug（②单包）：快审（确认判据 + 路由裁决），裁决写进 `plan.md` frontmatter，不单独成件。
   - **逃生门（reviewer-first）**：review 中发现疑似触契约/spec/agent 定义/gate → **先派 `architect-reviewer` 四分类** → `CONTRACT_CHANGE_REQUIRED`/`SPEC_CONTRACT_DRIFT` 转契约门 + 停、升级 `changes/arch/`（本门无权批准这些）。
3. **人类触点①（确认/改判路由）**：用 AskUserQuestion 呈现 architect 的四档裁决，人类一句话确认或改判（如 Lite→Full）。**落盘前必须过这一关**——Lite/Full 是安全相关判断，architect 误判需人类 backstop。
4. **主线程落盘（只搬运 architect 的 reviewed plan + 裁决，不改内容）**：按确认后的档位把 reviewed plan 落进它的家：
   - **③大 feature** → `.planning/<ws>/PROPOSAL.md`（+ `review.md`），frontmatter `freezeState: pending-human`。**按 `docs/templates/WORKSTREAM-PROPOSAL.template.md`（三问 + 分型模板）起草**——业务目的清单 / `workType` 数组 / spec 关系标注三项硬项缺失，会在 portfolio-planner intake 被 fail-closed 打回（260912 拆包链 T1）。`.planning/<ws>/` 由你（人类/主线程）创建——这是「设计权威落家」（对 ARQ-065/066 RTE 集权的有意放宽，见 change-triage.md §F4）；portfolio-planner 后续拆包时才建调度态。
   - **②单包** → `taskpackages/<id>/plan.md`，frontmatter 含 `rigor`/`riskTier`/`sourceType`/`reviewed-by`/`date`。**建包时按 `task-intake-and-slicing.md` §§ origin/changeId 创建 taskpackage.json + 填入 origin（`upstreamType`/`upstreamRef`/`upstreamFrozen`/`workstream`）+ 注入 `YYMMDD-` 日期前缀 changeId + 写 `planRef` 指回 sibling `plan.md`**（建包方职责经 reference 下达、经既有委派链承接；gov-review-devloop-artifact-lifecycle DP-5）。
   - **①直接修** → 不落盘（地板：不改 apps 行为 / 人类明确跳过）。
5. **人类触点②（仅大 feature，冻结）**：人类盖 FROZEN（版本/sha）。**作者≠判官：architect 不能自冻**（scope 机器兜住——architect allow 不含 `.planning`）。portfolio-planner 只拆 FROZEN 的 PROPOSAL。
6. **报告下一步命令（人类触点③）**：打出人类要起的下一段会话命令——
   - ②单包 → `planner` 跑 `/build-feature <id>`（从 `taskpackages/<id>/plan.md` 读 reviewed plan 做 intake）。
   - ③大 feature → `portfolio-planner`（指 `<ws>`，从 `.planning/<ws>/PROPOSAL.md` 拆包）。
   planner/portfolio 必须是**自己的主线程会话**——子 agent 不能再派子 agent，本门拉不起那个会话；dev loop 本就该人类在场。

> **convention-only 不变量**（本门当前无机器牙齿，靠约定 + 多层人工兜底，机器守卫记 ARQ 后续 G3，详见 change-triage.md）：主线程只搬运 architect 的 reviewed plan、不改内容；单向守卫（riskTier overrides rigor）靠人类触点① + supervisor riskTier 升级兜底；FROZEN 拆包门靠 portfolio-planner 自觉；plan.md planner 只读消费、改需留痕。
> 你不改 `docs/contracts/**`、不改 L0 蓝图、不直推 main；触事实源一律升级 `/arch-change`。
