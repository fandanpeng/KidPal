---
id: ARQ-260901-g3-hook-registration-gaps
title: `.claude/settings.json` 钩子注册系统性漏接（原诊断，经三处 settings 并查后纠偏）
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-01
closed: 2026-09-01
closedBy: ""
paths: []
source: ""
---

human 指示直修（项目初期不走 review）。**先纠原诊断的方法错**：钩子按设计分三处 settings 注册（base `settings.json` 常驻 / `dev-loop.settings.json` 仅 Pod 会话 / `review-loop.settings.json` 仅 review 会话），原审计只比对 base、故把二者误判为漏接——① `supervisor-gate-check`（Stop，合并门第二方验 provenance）**本就已注册在 `dev-loop.settings.json`**（刻意不入常驻、否则每次交互收尾都触 Stop 门），`--self-check` 10/10 健康=假阳；② `portfolio-sync`（PostToolUse）同样已在 `dev-loop.settings.json` 注册、空输入 exit 0 健康=假阳；③ `supervisor-verdict-gate`/`verify` 是 planner 直调的 stage 脚本、非钩子，本就不该入册。真实缺口仅 1 个：`suggest-codebase-memory`（告警级 injector，源仓 base settings.json 常驻注册、本仓三处全漏）→ 已补接 base PreToolUse(Bash) + PostToolUse(Grep|Glob)，冒烟：JSON 有效、PreToolUse exit 0、PostToolUse 正确注入图工具提醒。附带纠正 base `_doc` 陈旧句「dev-loop.settings.json 未迁（档 2）」（实已迁且承载上述 2 钩子，此句正是假阳诱因）。planning-sync 收口批已补接 base PostToolUse。残余：把「钩子在场性 == 三处 settings 并查 + 预期落位」做成自动化断言——评估后**不以 runtime 钩子承载**（下次迁移发生于全新仓、该 guard 脚本届时尚不存在，帮不上迁移），改建为迁移指南人工 checklist 步骤（见 GOVERNANCE-MIGRATION-GUIDE.md §迁移后对账 checklist）。
