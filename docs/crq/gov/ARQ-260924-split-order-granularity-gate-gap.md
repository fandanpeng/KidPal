---
id: ARQ-260924-split-order-granularity-gate-gap
title: 拆包顺序链（里程碑登记/roadmap 先于 taskpackages）无机械闸且 portfolio-planner 无尺寸模型——ui-v5-align 事故以 21 包建毁与三轮 human 复裁为代价实测显形
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一治理批（动 portfolio-planner intake / task-intake-and-slicing Slicing Matrix / workstreams README 任一面时并批；① intake 停等半可先行）
opened: 2026-09-24
closed: ""
closedBy: ""
paths: [.claude/agents/portfolio-planner.md, .claude/references/task-intake-and-slicing.md, delivery/workstreams/README.md]
source: [delivery/workstreams/ui-v5-align/findings.md §7（260924 执行记录逐时点）, delivery/workstreams/INDEX.md（hook 2026-09-24 重建行 = workstream 建目录迹）]
---

**证据**（delivery/workstreams/ui-v5-align/findings.md §7 执行记录，2026-09-24 主线程会话）：① 顺序链缺失实测——workstream 建立当日即派 portfolio-planner，此时 milestones.jsonl 无本倡议登记、workstream 无 roadmap/phases、无 PROPOSAL.md；一轮产 15 个 taskpackage（260924-v5-k1a..k5-* 全四件套）并回写 changes 列表；二轮（6 包口径）运行中被 human 中断、6 包目录残留盘面；human 质问原话「roadmap 都没产生 开始在那不断的创建 task package，是哪里逻辑定义的不对？」。② 闸缺口定位逐字——portfolio-planner.md Step 1 fail-closed 核验条件为「输入为 `delivery/workstreams/<ws>/PROPOSAL.md` 时」核模板硬项，非 PROPOSAL 输入（本例 task_plan.md）不触发；Step 5b 门 1（拆包 review 门、切法票）两轮均未跑到；M1–M4 先例链（milestones.jsonl 登记 nextAction=roadmap 摘要 → workstream phases → taskpackages，见 milestones.jsonl M4 行「拆包已落…K1→K2→K3」时序）只存在于先例、无任何机械闸或 agent 前置条件承载。③ 粒度无模型实测——planner 定义（Step 2）切法判据 = 垂直切片（能力完整性）+ 冲突面 + 跨包冻结面扫描，无尺寸维（行数/文件面/单包验证预算/N 上下界）；两轮粒度实质等于派遣方喂入的批次提案（15 包 ← 批次 0-6 加半包；6 包 ← P-A..F），human 三轮复裁（15→6→3）充当缺失的粒度门；终裁三包判据三条 = 包间文件所有权不相交 / 门禁依赖包内同质 / 单包验证预算一 Pod 可消化。④ 工作树迹：INDEX.md hook 重建行（ui-v5-align 2026-09-24）+ 未跟踪 ui-v5-align/ 目录；补序动作（milestones.jsonl M5 行、task_plan Roadmap 节）均发生在两轮建包之后、事故暴露后还原/删除。

**影响面**：任何「建 workstream 后直接派遣拆包」的倡议都会复现（顺序与粒度两缺口与倡议内容无关）；单次代价 = human 复裁轮次 × N + 制品建毁 churn + 里程碑账本污染风险 + 残包被接棒误承风险（本轮 6 残包若未清理即入下一棒 intake）；本轮实测代价 = 3 次派遣（1 完整 2 中断）、21 个 taskpackage 目录建毁、milestones.jsonl 一行污染还原、3 轮 human 复裁。频率 = 每个新倡议一次（M1–M4 靠先例自觉未踩，自觉非机制）。

**建议改动**（候选，交 review、不自批）：① portfolio-planner.md Step 1 intake fail-closed 从「输入为 PROPOSAL.md 才核验」扩为拆包类派遣三件停等拒拆：milestones.jsonl 已登记 / workstream 有 roadmap（task_plan phases 节或 roadmap.md）/ 有 PROPOSAL 或等价业务目的清单——缺任一 AskUserQuestion 停、打回主线程补齐（与既有 PROPOSAL 符合性核验同打回出口，不猜不代写）；② task-intake-and-slicing.md Slicing Matrix 增尺寸维（改动行数 / 文件面 / 单包验证预算 / N 上下界）+ 视觉对齐类切法判例（260924 三包终裁：页单位、文件面不相交、跨页串面项走合并后串行追加轮；视觉批不套功能切片细粒度逻辑）；③ delivery/workstreams/README.md 明写「task_plan phases 为必备节；里程碑登记先于拆包派遣」（M1–M4 先例链成文）。

**风险**：riskTier: medium——治理过程面、每倡议复现，但不触契约与实现面、单批可修。改动面 = G2（agent 定义 + reference）+ G4（workstreams README）mixed，且 ① 属新增前置门（gate 新增/修改 = lightweight 机检硬排除项）；Track B 三要件 ① yes（本条执行记录齐全）/ ② no（强制点存在真设计分叉：intake 停等 vs hook 机械闸 vs 模板纪律）/ ③ yes（爆炸半径本轮实测）→ **review profile = full-adversarial + human（Track A，/arch-change 入口）**，不美化快路径。owner: architect。
