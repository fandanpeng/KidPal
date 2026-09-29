---
id: ARQ-260929-agent-def-provenance-noise
title: 9 个 agent def 内嵌过程修改记录（changeId / T 号 / 实证案例点名）——每次派发全量进上下文的 token 债与照历史抄近路诱导
domain: gov
status: blocker
severity: SHOULD
owner: architect
trigger: 下一治理批（/arch-change 入口；与 ARQ-260929-agent-def-reality-drift、ARQ-260923-planner-roster-reference-drift 同车成批；story-author / requirement-reviewer 为目标形态）
opened: 2026-09-29
closed: ""
closedBy: ""
hardExclusions: true
paths: [.claude/agents/planner.md, .claude/agents/acceptance-author.md, .claude/agents/portfolio-planner.md, .claude/agents/integrator.md, .claude/agents/builder.md, .claude/agents/architect.md, .claude/agents/fixer.md, .claude/agents/review-moderator.md, .claude/agents/architect-reviewer.md, .claude/agents/adjudicator.md, .claude/agents/defender.md]
source: "主线程 2026-09-29 agent def 全量核查（16 个 def 逐文件人读 + census 机检；prompt-authoring rule Pruning 原则对读）"
---

**证据**（2026-09-29 census，`grep -o "26[0-9]{4}-gov-review[a-z-]*|260912 拆包链|gov-review-[a-z-]* FROZEN|..."` 按文件计数，下界）：planner.md 28、acceptance-author.md 8、portfolio-planner.md 7、integrator.md 5、builder.md 3、architect.md 3、fixer.md 2、review-moderator.md 1、architect-reviewer.md 1——合计 ≥58 处（窄 pattern 下界；人工复核另有散文型注记未被计：review-moderator.md:12「常驻定位（2026-08-02）」、:104 retro 实证引文、adjudicator.md 卡点 D 实证引文、defender.md:47 heredoc 历史教训句）。16 个 def 中 story-author / requirement-reviewer / scenario-author / challenger / landing-verifier 基本干净（0 命中）。

典型形态：planner.md:31 起包 checklist 内嵌「260924-gov-review-planner-orchestration-align T6 增」+ 三包一代 / 二代复现实证点名；architect.md:63 landing 通道退役史（2026-07-08 批 → 2026-08-02 批 + PROPOSAL 路径引文）；acceptance-author.md:30「260907-g-relay-fix 实证」。

**影响面**：agent def 是每次派发全量进模型上下文的常驻文本——58+ 处溯源注记是纯 token 债；且实证案例点名（「k4-reminder / m2-k2 / m3-k1 跨三棒被同一闸咬」类）诱导 agent 照历史个案抄近路而非按现行判据判断。溯源功能 git blame 与 `delivery/changes/arch/**` 的 PROPOSAL/retro 制品已完整承载，def 内不需要。

**建议改动**（候选，交 review 不自批）：以 story-author / requirement-reviewer（0 注记、纯角色约束）为目标形态清理——**约束句留、溯源注记删**；底线 = prompt-authoring rule「Pruning 硬边界（HQ-1）」：权限边界 / 门 / authority / scope / 升级条件 / must-never 句一个不删（判据：删掉是否改变谁能做什么、何时必须停、哪个门裁断）；消歧必需的少量注记（如 adjudicator「原 ARCH-REVIEW-QUEUE」指路）保留。改后复跑 lint-descriptions.mjs + route-regression-check.mjs。

**风险**：riskTier: low——纯 G2 文本、零行为语义变更、有机械 lint 兜底 + landing-verifier 逐文件人读核对。review profile：**full-adversarial + human（Track A）**——批次混含 architect.md（self-def）与 review-moderator / adjudicator / defender / architect-reviewer（review-loop def，第五阀硬排除），机检硬排除命中，不美化成快路径。
