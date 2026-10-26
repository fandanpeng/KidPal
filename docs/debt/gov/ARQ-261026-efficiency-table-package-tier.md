---
id: ARQ-261026-efficiency-table-package-tier
title: 效率表缺「包档位/类型」维度——无规模基准则 token/行数不可判读、拆包合适性无从评估
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一治理批（动 `.claude/scripts/retro-efficiency.mjs` 或 `/loop-retro` 输出契约的批并并）；或 human 点名立项
opened: 2026-10-26
closed: ""
closedBy: ""
paths: [".claude/scripts/retro-efficiency.mjs", ".claude/commands/loop-retro.md"]
source: [delivery/retro/2026-10-25-loop-retro.md, delivery/review/261024-gov-review-retro-efficiency-instrumentation/PROPOSAL.md]
---

**证据**：① 执行记录——`delivery/retro/2026-10-25-loop-retro.md` §八（2026-10-26 补录，commit eafc8e48）：`retro-efficiency.mjs` 首次对本轮 25 包实跑出两张效率表；human 读表后指出缺口。② human 2026-10-26 原话：「其实不是用 source type，我是想看它这个包是单包 light、单包 full 还是 nano，还是说 governance 的变更。因为如果只通过 source 来看，你不知道它跑的到底是什么规模的包，就很难判断它正不正常，也很难推断出我们在拆包的时候拆得合不合适。」（同一对话里已明确否掉「直读 sourceType 当类型列」的初案——sourceType 只答来源、不答规模。）③ 数据源实测（2026-10-26 本 worktree `gov/kidpal`，`node -e` 全量 95 个 taskpackages 目录）：`taskpackage.json.rigor` 值域 = `full` 55 / `lite` 34 / `nano` 1 / 缺值 1（有 pod-events 的 86 个中：full 50 / lite 34 / nano 1 / 缺值 1）——档位已在盘上，纯分析侧可读；`260920-gov-review-dispatch-telemetry-source` 仅事件无 taskpackage（rigor 不可得，需 fallback 处置）；governance 批佐证源 = `sourceType`（8 值域，86/91 有值，`arch-review` 5 / `proposal` 10 / `drift-report` 2 为治理侧来源）。④ 档位枚举权威 = `/triage-change` 五档分流（③拆包 / ②单包 Full / ②单包 Lite / ⓪ nano / ①直接修）。

**影响面**：每次 `/loop-retro` 跑效率表都命中。不修则 Pod 表的 token / agent 分钟 / 代码行始终混档同表——单包 Full 与 nano、治理批与业务批无标识地排在一张表里，跨包判读（「这包 token 偏不偏」「行数正不正常」）没有规模基准；拆包是否合适（一个大 workstream 拆出的 N 个包各自的量级分布）也无从评估。261025 轮 25 包中已实际发生混排（含 1 个 nano、至少 2 个 governance 承载包）。

**建议改动**（候选，交 review、不自批）：纯分析侧两处——① `retro-efficiency.mjs` Pod 表加「档位/类型」列，分类规则候选：`rigor` → full / lite / nano；「拆包」= workstream 归属或 `sourceType === 'planning-workstream'`；「governance」= `sourceType` 治理侧值（arch-review / proposal / drift-report）或 id 线索；缺值与无 taskpackage 行（260920 型）留空并在数据缺口节计数，不推测。规则落点、列名与展示顺序（列插在哪一列）交 PROPOSAL 定。② `.claude/commands/loop-retro.md` 步骤 5 输出契约的列清单同步（九列 → 十列）。边界：零新增记录、零 schema 变更、零门语义；不做按档位的自动阈值告警或「正常区间」判定（human 只要看维度，不要判读机器）。

**风险**：riskTier: low-medium——只读脚本一处 + 命令输出契约文面一处，零 schema / 门 / agent 定义 / G1 / 契约面触面；与 261024 批 T1（同脚本、riskTier low-medium 获判官认可）同量级。review profile = Track B 主路径（触 G3 scripts，非瘦制品轻路径；三要件：① 执行测出 ✓（§八补录执行 + human 读表反馈）② 问题清晰、修复方向明确 ✓（分类映射细节留 OQ，不构成方向分叉）③ 爆炸半径已由执行揭示 ✓（脚本 + 命令文面，已在 261024 批落地时勘界））。
