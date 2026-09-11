---
workstream: 设备显示与交互主权包（三屏主权收口 + 显示先行 + 回答驻留 + 键位路线）
slug: 260911-display-interaction-sovereignty
status: active
opened: 2026-09-11
closed:
summary: 落地 PROGRESS-SUMMARY §七.1 遗留 #4/#5/#6/#7 四项——三屏归一屏（OQ1=纯B）、显示先行 M1（OQ2）、回答驻留随归一屏自然成立（OQ3=c）、KEY 键位路线 A A1 全局层先行（OQ4）
owner: portfolio-planner
area: systems
changes:
  - 260911-key-route-a1
  - 260911-screen-unification
  - 260911-display-first-m1
---

# 260911-display-interaction-sovereignty · task_plan

## 目标

让孩子拿起设备问一句话时「听得见的答案、看得见的落点、按得动的按键」成为连贯体验：
归一屏消掉三屏主权混乱的结构性土壤（当日三崩溃同族）、display 页上屏提前 1–3s、
回答结束后屏幕落点确定（非随机）、KEY(GPIO18) 接线落地 spec 两键分工。

## 设计权威

- 拆包设计权威 = 本目录 `PROPOSAL.md`（FROZEN v1.0，2026-09-11 human 触点② grilling
  四 OQ 已裁，冻结 pin `sha1:cd10d1903ac6bb1a` / git blob sha `26138dba`，commit 8a06d0c）。
- 评审记录 `review.md`（architect 分流 GREEN-for-triage，F1–F9）。
- 决策覆盖 `decision-coverage.json`（workstream 级，proposal 决策集 → 承载包映射）。

## 拆包结构（PROPOSAL §4 依赖序；PKG-3 已撤销不在本次范围）

| 包 | changeId | mergeOrder | dependsOn | 一句话 scope |
|---|---|---|---|---|
| PKG-1 | 260911-key-route-a1 | 18 | — | KEY(GPIO18) 板级接线 + parser 喂入 + A1 全局路由 + BOOT 回归翻页 |
| PKG-2 | 260911-screen-unification | 19 | — | 小智聊天控件迁 KidPal 层 layer + 三标志/tick 退役 + char 并屏 + alert/reminder 优先级 + 驻留副产品 |
| PKG-4 | 260911-display-first-m1 | 20 | 260911-screen-unification, 260911-ecs-deploy-convergence | 引擎补丁块闭合即发（M1）+ 冻结齿修订 + front 时序复核 + 设备语音中渲染 |

- 依赖图：PKG-1 ∥ PKG-2 → PKG-4；PKG-4 另有批次①前置（260911-ecs-deploy-convergence，
  mergeOrder 16，overlay 漂移修复）。
- 批次③ 260911-render-pages 原 mergeOrder 17 为占位，本次落锁 = 21（②先③后，其
  decisionCoverage「human-20260911-批次②先③后」预授权 portfolio-planner 排序时落锁）。

## 决策

- WIP limit = 2（PKG-1 ∥ PKG-2 并行，PKG-4 待两者并后起）。
- 跨包 KEY 接线 owner 显式登记（ARQ-260911-cross-package-behavior-owner-gap 试金石，
  首用）：KEY(GPIO18) 接线 + parser 喂入 + 全局路由端到端 owner = 260911-key-route-a1；
  「KEY 长按=回首页」的导航动作面与 PKG-2 归一屏相交——路由语义 owner 归 PKG-1，
  PKG-2 重构显示面时保两键行为回归（PKG-2 验收显式含）。
- 核心包包级 plan.md（ARQ-260911-split-package-implementation-prose-gap 首用）：
  PKG-2 产 `.claude/taskpackages/260911-screen-unification/plan.md`（planRef 指向），
  planner intake 只读消费、改需留痕；PKG-1/PKG-4 改法由 PROPOSAL §2 已裁机制直接承载，
  按 ARQ 分级判据（改法非显然/跨多文件/铁律约束）评估——PKG-4 涉跨系统改法取舍亦产
  plan.md，PKG-1 装配型豁免。

## 非目标

不改 `docs/contracts/**`（M3 变体禁止；M1 开工前 reviewer-first 复核）、spec 零修订
（SPEC-1 本批不触发，冲突走逃生门）、观感/字体/排版/课表填充归批次③、首音时延 #8 归
时延实验线、故事归档双断链 #3 归批次④。
