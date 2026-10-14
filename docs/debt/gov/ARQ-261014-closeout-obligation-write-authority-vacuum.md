---
id: ARQ-261014-closeout-obligation-write-authority-vacuum
title: 收口义务清单的写权承载缺位——CDQ 行 / 设计稿母版同步 / spine 回填 / 收口挂法句等义务无登记角色可写，撞 enforce-scope 成跨包常态
domain: gov
status: deferred
severity: SHOULD
owner: architect（写点映射 + 文面）+ 主线程（承载裁定）
trigger: 下次治理批；或下个触 DEV-GOVERNANCE 收口节文面的批
opened: 2026-10-14
closed: ""
closedBy: ""
paths: [".claude/scope-map.json", ".claude/agents/planner.md"]
source: loop-retro-2026-10-14 18 包复盘（`delivery/changes/retro/2026-10-14-loop-retro.md`）
---

**证据（本波四包复发）**：① 261011-p1-home-v5-rework `fe6c3f46`（scope-or-permission / medium）：设计稿母版三处内容级同步无可重用执行角色——builder 无 docs/ui 写权、uiux-author 按 def 仅限 requirement-change 档 2（dev-loop 包内不派）、planner/architect/integrator 均无写权 → human 裁 B 主线程直落（`b7e2d94a`）；同包 `930257e5`：CDQ 行无任何登记角色可写 `docs/CONTRACT-DECISION-QUEUE.md`，integrator 与 planner 代落双双被拦 → `d41f0b6e` 裁主线程直落；② 261010-p3-storylist-cold-start-restore：收口义务三件（CDQ 行 + arch/spec 挂法句 + spine 回填）全撞 enforce-scope，pending planner——retro 自称「第三次烧同一类坑」；③ 261010-prompt-v9 `359e51bb`（scope-drift）：planner 两次写权口径误判（integrator 对 docs/architecture.md、architect 对 delivery/changes/dev/**）——universal 段与 agents 段两套口径、核对只看 agents 段枚举。

**定性**：非闸缺陷——enforce-scope 全对；缺的是「收口义务 → 承载写点」的映射表与 finalize 前置核对步骤。每包收口期 planner 现场即兴找写点、找不齐就升级 human 裁「主线程直落」，同型即兴每包重演。

**修法候选**（交 review，不自批）：① DEV-GOVERNANCE 收口义务清单逐项钉承载写点（CDQ 行=主线程 / 设计稿母版=主线程直落 / spine 回填=主线程 / spec 挂法=architect——后者已是 ARQ-260929-docs-sync-spec-handoff 的既有裁定，映射表把它收编成文）；② planner finalize 步骤加「写点核对」前置项（纯文面，不改闸）；③ 高频代落场景（CDQ 行）评估给机械通道或专用写槽。倾向 ①+②。**riskTier: low**（不动闸语义；scope-map 写权矩阵若动走既有批内换授先例）。
