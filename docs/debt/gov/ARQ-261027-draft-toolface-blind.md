---
id: ARQ-261027-draft-toolface-blind
title: 起草对 agents 落点的改法时不核执行者 frontmatter 工具面——指涉不存在的工具（如对无 Edit 的 agent 指示「最小 Edit」），判官打回才暴露
domain: gov
status: deferred
severity: SHOULD
owner: architect（起草自检）
trigger: 下一次改 `.claude/agents/**` 正文的治理批起草期（并批消费）；或本条复发一次
opened: 2026-10-27
closed: ""
closedBy: ""
paths: ["docs/templates/PROPOSAL.template.md", ".claude/rules/prompt-authoring.md"]
source: ["delivery/review/261027-gov-review-p75-closeout-shoot-cluster/PROPOSAL.md §7 R1 F2", "delivery/review/261027-gov-review-p75-closeout-shoot-cluster/retro.md F1"]
---

**证据**：261027-gov-review-p75-closeout-shoot-cluster 批 R1 F2——T4 单写者约定指示「做最小 Edit / 改走增量 Edit」，但 uiux-author frontmatter `tools: Read, Write, Grep, Glob`（`.claude/agents/uiux-author.md:4`）无 Edit 工具，指令对执行者不可执行；修订改为 Write 语义判据（R2 核到位）。根因 = 起草期核语义不核执行者工具面；prompt-authoring rule 管文风（Pruning / Leading Words / Completion Criteria）不含工具面对账。

**影响面**：凡改 `.claude/agents/**` 正文的治理批——对 agent 行为的每条指示都隐含「该 agent 有工具做这件事」的假设，盲区每批可复发。

**建议改动**（交 review、不自批，二选一）：① PROPOSAL.template 起草期自检句增补：改 agents 正文时逐条核「指示的操作在该 agent frontmatter tools 面内」，不在面内的改写为在面内语义（如 Write 合成判据）或显式注工具缺口；② prompt-authoring rule 增第四原则「Toolface」（工具面对账）。倾向 ①（模板句轻、起草者必到）。

**风险**：riskTier: low——纯起草自检文面。
