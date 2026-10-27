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

**追注（2026-10-19，261019-gov-review-docs-root-ledger-rehome 批）**：CDQ 单文件队列（上文 :16 所引 `docs/CONTRACT-DECISION-QUEUE.md`）已并入 `docs/debt/contract/` 域并删除原文件——新写点落在 integrator / adjudicator 既有 `docs/debt/**` 写槽内，本条目「CDQ 行写权真空」一项就此闭合。条目**不结**：其余义务面（设计稿母版 / spine 回填 / 挂法句写点映射）未治，status 留 deferred。

**追加证据（loop-retro-2026-10-25，25 包复盘，三包复发）**：① 261015-p1-auxbar-style-fix——planner Pod 会话可派集无 uiux-author（定义在册、harness 会话不含），真母版 docs/ui 内容级同步无人可写，降级并入真机轮 human 门（新子项：per-session cast 可用性，不只是 allow 表）；② 261016-sd-card-mount——docs/notes/** 无任何 agent allow 覆盖，且 --agent planner 起的 Pod 主会话受 per-agent 约束，plan 的「主线程通道代落」假设不成立（新子项：Pod 主会话 ≠ 裸主会话，起包 checklist 应按 agent 粒度核 doc-sync 落点写权）；③ 261022-m5-k1——acceptance-author 的 scope 只许写 server/acceptance，taskpackage 元数据回填被拦、planner 代写多一个往返。原条目「收口义务→承载写点映射表」的修法方向不变且更紧迫。证据：delivery/retro/2026-10-25-loop-retro.md §二 T10。

**追注（2026-10-27，261027-gov-review-p75-closeout-shoot-cluster 批）**：修法候选 ①+② 落地，修法候选 ③（CDQ 类机械通道）本批未取。① 收口义务→承载写点映射 = `docs/DEV-GOVERNANCE.md` §6「收口义务 → 承载写点映射」表（与 §7 入口表分工句同批写明，防双表漂移）；② finalize 前置核对 = `planner.md` Step 8c（纯纪律步、零新门）。2610-25 三子项处置逐条：② Pod 主会话 ≠ 裸主会话写权口径 → **收编**（映射表「按持会话 agent 粒度」边界句 + Step 8c 同句）；③ acceptance-author taskpackage 元数据回填被拦 → **收编**（映射表 taskpackage 行：写点 = planner，派发书不要求 acceptance-author 回写）；① Pod per-session cast 无 uiux-author → **半收编、留根**：映射表已钉「Pod cast 缺省不含 uiux-author → 母版 / 出图归 Pod 外主线程」，但「把 uiux-author 加进 Pod 会话 cast」属 harness 会话级配型问题、非写点映射，本批不治。条目维持 deferred，剩余面 = 子项① 与修法候选③。
