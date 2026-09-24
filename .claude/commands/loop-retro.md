---
description: Dev Loop OS 周期复盘（cadence retro）。按新旧格式读取 Pod 复盘证据，提取共性踩坑、升级主题与流程债，把治理类发现转成 architect-owned ARQ（不自我批准、不直接开 TaskPackage）。
argument-hint: <可选：周期范围或聚焦点，例如 "近 10 个 change" 或 "gate false-green">
---

# /loop-retro

你现在做 **Dev Loop OS 周期复盘**（对应人类敏捷里的 Sprint 回顾，但抽取其控制系统、不照搬仪式）。
权威定义见 `.claude/references/dev-loop-operating-system.md` §6。
本次聚焦（可空）：

> $ARGUMENTS

## 纪律（先读）

- 复盘**只读**证据，不改实现、不改 canonical 定义。
- 新格式（changeId >= 250725 且 pod-events 存在）只读 `delivery/taskpackages/<id>/pod-events.jsonl` + `delivery/changes/dev/<id>/pod-retro.md`；旧格式继续读 loop-health/session-digest/pod-journal/pod-retro。两条路径互斥，retro 不重算 verifier/supervisor 事实。
- **游标**由 `.claude/loop-retro/ledger.jsonl` 判定。候选按 exclusive dispatch：new-format → old-format loop-health → waiting；同一 Pod 只进入一条路径。
- **retro 不自我批准**：任何要改 Dev Loop OS 定义 / agent 角色 / gate / scope-map / K9-DevLoop 边界 / merge 标准的发现，先成为 **architect-owned ARQ candidate**，交 `architect` 处理，**不**直接进普通 planning / TaskPackage。
- 只有 architect 裁定明确需要动实现面（`firmware/**` / `server/**` / 测试）时，才转实现侧 dev-loop。

## 步骤

> **触发条款（何时该起 retro）见权威源** [`dev-loop-operating-system.md` §6「触发条款」](../references/dev-loop-operating-system.md)（复合触发：`pendingAnalysis + waitingOnLoopHealth` 合计（除 excluded，源桶排除）≥ N 或日历兜底；任一 Pod `human-correction` 事件；发起责任 = 主线程）。本命令不重述条文——被触发后按下列步骤执行。

1. **定范围（游标）**：跑 `node .claude/scripts/loop-health-report.mjs --pending`。对未覆盖 changeId 依次判定：(a) date-prefix >= 250725 且 pod-events 存在 → new-format；(b) 否则 loop-health 存在 → old-format；(c) 否则 waiting。两种文件同时存在的 post-cutoff Pod 必须走 new-format，不得双计。经 human-approved 销账（excluded，游标第三态）的 Pod 在 `computeRetroScope` 分桶源头排除，**不进 waiting/pending**（单源传导，权威指 OS §6）；`--pending` 的 `excludedFromTrigger` 桶报其绝对规模。本步骤只对未销账者分流。
2. **按格式读取**：new-format 运行 `node .claude/scripts/pod-events-aggregate.mjs --change-id <id>`，并读 pod-events + pod-retro；不读 loop-health/session-digest/pod-journal。old-format 保持既有 loop-health + session-digest + pod-journal + pod-retro 读取逻辑。Malformed event 行只告警跳过；缺 pod-retro 只 advisory，不阻 retro。
3. **提炼共性**（跨 Pod 归纳，不逐个复述单 Pod 已总结的内容）：
   - top failures / 重复出现的 verifier fingerprint；
   - gate false-green 疑点（supervisor APPROVE 但后续回归 / digest 里有 human correction）；
   - human correction 主题（人类反复纠偏的方向 = 流程或规则缺口信号）；
   - agent escalation 主题（按 category / severity / unresolved 聚合，引用 eventId，不把 event 当 verifier/supervisor 权威）；
   - process debt candidates（重复手工步骤、缺失护栏、模糊边界）；
   - 跨设备 / 跨会话交接失真（crossDeviceInputs 缺 digest 的案例）；
   - **两源偏差信号**：同一摩擦/纠偏主题上 pod-retro（planner 第一人称体感）与 loop-health（integrator 第三人称综合）相互矛盾 → 显式列「planner 体感 vs integrator 综合偏差」条目（冲突本身是有价值信号：planner 避重就轻的主观归因会与 loop-health 客观 fingerprint 对撞成偏差）。
   - **findings 对账 canonical / 已裁决项（GC1，权威指 OS §6）**：分流前逐条核发现所指对象的**最小必要对账集**——现行 canonical 状态（DEV-GOVERNANCE 角色编制 / 门体系、OS、agent def 现行条文）+ 直接相关已裁决项（ARQ / CDQ 主题直接指向该对象者，状态含 approved-registered / landed / accepted-risk）；canonical 已改项必含（如退役编制注，纵无独立 ARQ 亦须核），不 blanket 扫无关活 ARQ。发现与现行 canonical 冲突（如『建议补建已退役 def』）→ 记『复现：已裁决项未被行为闭合』、不新立 ARQ，证据引**真实现行锚点**（核行号、标旧引漂移）。人读判断、advisory、非门。
   - **new-format 覆盖比对（GC3，权威指 OS §6）**：new-format 包逐主题核对 pod-retro 摩擦/纠偏主题是否在 pod-events 有对应 event；机器 counts 与散文主题不可按主题对撞，故人工覆盖比对，显式列『机器聚合 vs 散文主题 覆盖偏差』条目（如 ledger friction=0 vs retro N 主题）→ 转 ARQ candidate，标偏差不采信任一源。**与 `:32` escalation 机器聚合轴之分界**：`:32` escalation 主题由 `pod-events-aggregate.mjs` 机器聚合（counts / category / severity / unresolved）；本 GC3 覆盖比对是人工逐主题核对（机器 counts 与散文主题结构性不可按主题对撞）。机器 escalation 聚合 ≠ 覆盖比对——前者算 escalation 事件分布，后者核 pod-retro 摩擦/纠偏主题是否在 pod-events 有对应 event（含 ledger=0 / retro=N 型偏差）。二者互补不可相代。
   **step 3 对账与 step 4 查重独立串行、作用面不同、互不替代**：对账防把已裁决项当新降级（对现行 canonical / 已裁决项），查重防重复新立（对 queue 同根已登项）；两者皆过方进登记。
4. **分流每条发现**：
   - **治理类**（改 Dev Loop OS 定义 / agent / gate / scope-map / K9 边界 / merge 标准）→ 写成 architect-owned ARQ candidate（含：证据引用、影响面、建议改动、riskTier），**由主线程登记进 CRQ 账本 `docs/crq/gov/`**（一条目一文件 `ARQ-<YYMMDD>-<slug>.md`，骨架 `docs/templates/crq-item.template.md`；写完跑 `node .claude/scripts/crq-render.mjs`）（human-approved 2026-07-28：登记执行者 = 主线程，非 architect——architect 经 `/arch-change`/`/review-governance` 处理已登记 ARQ，不做登记动作）。**登记前查重纪律**：先 `grep -l "<关键词>" docs/crq/*/*.md` 查有无同根已登项；有 → **在既例行追加证据注记，不新立**；无 → 按现行 slug 编号新立。**不**自己改 canonical、**不**开 TaskPackage。
   - **两源偏差类**：① old-format（pod-retro vs loop-health 矛盾）② new-format（pod-retro vs pod-events 覆盖偏差，机器不可按主题对撞、人工覆盖比对）→ 均写成 architect-owned ARQ candidate，标『偏差不采信任一源』。
   - **实现类**（确定的 `firmware/**` 缺陷 / 测试补强）→ 走正常 dev-loop：单切片直接 `planner`，多切片/多 Pod 交 `portfolio-planner` 拆包。
   - **信息类 / 接受风险** → 记入复盘输出，标 owner + 触发条件，不强行加门（别无脑加管控）。
5. **输出复盘小结**：top findings、升级主题、每条分流、advisory、waiting 清单、token coverage（只在有 exact 数据时报告总和，并附 `{dispatchesTotal, dispatchesWithTokenData, dispatchesUnavailable}`）与下一周期关注点。
   - 对账结果（GC1）：N 条发现与现行 canonical 冲突（记复现、不新立 ARQ），含所核现行锚点。
   - 覆盖比对结果（GC3）：new-format 包 M 个，机器聚合 vs 散文主题覆盖偏差 K 处（含 ledger=0 / retro=N 型），转 ARQ candidate 数。
6. **收口游标（推进 ledger）**：分析完跑 `node .claude/scripts/loop-health-report.mjs --record-retro loop-retro-YYYY-MM-DD [--note "..."]`，把本轮 `pendingAnalysis` 追加进 `.claude/loop-retro/ledger.jsonl`；下轮 `--pending` 自动排除。若只分析了部分 Pod，用 `--covered a,b,c` 显式列实际分析的 change-id（其余留下轮）。ledger 随 main 版本控制、跨设备一致。
   - **销账（human-approved，独立手动、非例行步骤）**：对 human 裁为**结构性永不析出**的 Pod（如 pre-journal 无前缀 legacy slug），human 可独立手动跑 `node .claude/scripts/loop-health-report.mjs --writeoff <id1,id2,...> --reason "HUMAN-APPROVED: ..."` 记入 ledger `excludedChangeIds`（游标第三态）；下轮起 `computeRetroScope` 源桶排除、移出触发计数（撤销走 git revert）。**此非 retro 例行步骤**——retro 不自调销账，脚本不自裁分类；销账资格 = human-only 裁决（对 post-cutoff 尚可补证据的 Pod 不销账；判据与 fail-safe 权威指 OS §6，不重述）。

> 一句话：复盘把踩过的坑变成**可追溯的改进候选**；治理改动一律经 architect，别在复盘里直接改 Dev Loop OS 的规则或代码。
