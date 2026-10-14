---
id: ARQ-260928-devloop-landing-manifest
title: dev-loop 无文件级改动面预告与对账——改动面约束只有粗粒度三件套（subsystem 声明路径 + scope-map allow 面 + neverTouch），builder 在 allow 面内摸到声明路径外的文件无牙可拦
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: 下次动 taskpackage 格式 / closeout-check / supervisor-gate 闸面的治理批；或攒到一次实测证据（Pod 实际 diff vs 预告面差集）后开批
opened: 2026-09-28
closed: 2026-09-29
closedBy: "实质诉求被 260928-gov-review-dev-loop-preassess-manifest-scope 批（commit 在册、INDEX landed）实现：T4 给 taskpackage.json 标准字段新增 changeManifest[]（文件级改动面预告）+ 新闸 change-manifest-check.mjs（对账面 = firmware/** + server/** ∪ 显式 docs-sync 面，清单外即红 fail-closed，挂 verify-gates scoped 相位每轮跑）。三条待裁分叉均落答案：①硬闸（红 = 逼扩单/留痕，非静默豁免）；②修订留痕 = builder 越界停手 → planner → architect 扩单唯一通道；③挂点取 scoped 相位早停而非 closeout-check/supervisor-gate。注意：该批 PROPOSAL 自declared「承载 ARQ：无根、不关任何既有 ARQ 根」，本条由主线程按实质覆盖证据翻 closed（superseded-by）；human 原裁「下一批再说」的推迟语境已因批落地消解。若认为 docs-sync 对账细节尚有残余缺口，可重开新根。"
paths: []
source: [260928-gov-review-portfolio-boundary-story-propagation 立项会话（human 2026-09-28 裁「下一批再说」，PROPOSAL §6 留根登记不硬治项）]
---

**背景**（新 idea，非执行实测——human 在 260928 治理批立项会话提出「给 dev loop 也加一个 landing-manifest」）：governance lane 的 landing-manifest（PROPOSAL §2 围栏块，一行一个目标文件，landing-verifier 按 manifest 逐文件核保真）在 dev-loop 无对偶。dev-loop 现状对改动面的约束 = 粗粒度三件套：① `subsystem` 声明路径（planner FS-EVIDENCE-1 核 diff 非空且在声明路径内）；② `.claude/scope-map.json` allow 面（enforce-scope 钩子硬拦）；③ neverTouch。**缺口**：allow 面内、声明路径外的文件级漂移（builder 摸了同子系统内不在意图面的文件）无任何牙。

**待裁设计分叉三条**（开批时先裁，不预设答案）：
1. **硬闸 vs advisory**：diff ⊆ manifest 超出即红（实现期探索 legitimately 发现新文件是常态，高频假红风险）vs 超出仅报（advisory 台账化风险）。
2. **manifest 修订留痕机制**：若走硬闸，实现期发现新文件时 manifest 如何合法修订（对偶 acceptance tighten-only 修订窗口 / PROPOSAL FROZEN 后不可回改的例外追认）。
3. **挂点**：closeout-check（收口相位，对偶 taskpackage-required-check 程序化调用）vs supervisor-gate（合并门前）；字段落 taskpackage.json 的形态与谁维护（拆包侧预填 vs planner finalize 时填 vs builder 动工后回填）。

**关联**：本批（260928-gov-review-portfolio-boundary-story-propagation）T3/T4 已动 taskpackage 格式义务与 taskpackage-required-check.mjs——本条开批须基于该批落地后的新状态设计，避免同面叠加。
