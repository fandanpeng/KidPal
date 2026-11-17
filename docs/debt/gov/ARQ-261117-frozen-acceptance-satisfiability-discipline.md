---
id: ARQ-261117-frozen-acceptance-satisfiability-discipline
title: 冻结验收件带内部矛盾 / 不可满足断言进冻结——「跨用例一致性自检」已三次成文仍三次未执行，红基线只证红因不证绿路径可满足
domain: gov
status: deferred
severity: SHOULD
owner: architect（验收派单与冻结纪律文面）+ dev-loop（执行面实测校准）
trigger: 下一个动 acceptance-author 派单模板 / 冻结纪律文面的治理批；或同型缺陷再出现一次
opened: 2026-11-17
closed: ""
closedBy: ""
paths: [".claude/agents/acceptance-author.md", ".claude/references/dev-loop.md", ".claude/references/taskpackage-authoring.md"]
source: ["delivery/taskpackages/261028-m6-k2b-p2-schedule-16px/pod-retro.md F1", "delivery/taskpackages/261028-m6-k3-p3-story-typography/pod-retro.md F1", "delivery/taskpackages/261104-k1-points-ledger-core/pod-retro.md 踩坑 1", "delivery/taskpackages/261104-k4-reward-goal-config/pod-retro.md 踩坑 1", "delivery/taskpackages/261104-k5-story-cover-nav/pod-retro.md 踩坑 2", "delivery/taskpackages/261028-m6-k2a-p1-home-16px/pod-retro.md F1", "delivery/retro/2026-11-17-loop-retro.md §二 T2"]
---

**证据（六包，同一形态）**：
- 261028-m6-k2b F1 逐字：「builder step-0 通道③现跑 258/7，红名单与义务红名单对不上——追出冻结件 6 处逻辑不可满足缺陷（A1 墨高窗 [15..18] 越证据域：16px 资产实测墨高恒 14；旧位全行白负锚与 7 条日列竖线互斥；TEST 3 三处截断帧对误比 f_a/f_b；tooth6 用 day0 x 带判 day2；helper 极值函数命中带底与竖线）」。
- 261028-m6-k3 F1 逐字：「STY-A2 多行带跨 2 自然行恒越 [13..16]（与 STY-A4 WRAP 折行互斥 = 对任何实现不可满足）+ STY-A6 STAMP 带 y0=150 与印章圈底缘相切（基线态即红）」；同包自评「acceptance-author 本轮已实测校准预判窗，但仍漏了『夹具内容物与既有渲染家具的空间关系』」。
- 261104-k1 踩坑 1 逐字：「三处（A4 revision 与 A8/契约互斥、chain helper ±1 步长假设、A9 期望字节/权威值算错）在红基线期不可见——它们全是『实现缺席型红』，只有实现在盘后才暴露」。
- 261104-k4 踩坑 1 逐字：「K4G-A1 对空目录先快照再断言『非空』（不可满足）；K4G-A7 与 K4G-A2 对『删激活中自定义』给出相反期望……K2 的同一教训没有沉淀成必做步骤」。
- 261104-k5 踩坑 2 逐字：「冻结件跨用例状态残留第二次命中（SPV2-A1 tooth 3 留 COVER、tooth 4 未复位）：K2/K4 之后已写进派单『跨用例一致性自检』，仍然发生」。
- 261028-m6-k2a F1 逐字：「P0 冻结件只钉返回码不钉度量合理性（假绿）……若 P0 假绿未被 P1 冻结件咬出，将带到真机轮才发现 16px 全废」。

**影响面**：每个有冻结验收的 full / lite 包。代价形状 = builder step-0 现跑 → 发现不可满足 → 诊断副本定位 → 回派 acceptance-author 收紧型修订 → 复测，至少烧半轮到一轮；更险的是 builder 若就地歪修（摘 WRAP、缩窗凑数），负锚整体失效、验收镜像报废。正向对照：跨角色对抗每次都接住了——builder 不改冻结件本体、用诊断副本定位；acceptance-author 凭实测收紧型修订，零负锚削弱。但「接住」依赖 builder 自觉，不是机制。

**建议改动**（候选，交 review、不自批）：
1. **自检从散文变执行位**：acceptance-author 交付前对冻结件跑一次机器可判的自检清单——同一常量 / 同一辅助函数跨 case 对表；同一构造不得出现互斥期望；快照 / 前置必须与新建夹具状态匹配；同一 TEST_CASE 内复用的 view 换语义前显式复位。
2. **预判窗实测校准扩两条**：① 带窗先跑一次最小实测（墨高 / 盒宽 / 带位），不凭资产元数据推算；② 带窗定义时把带内既有渲染家具（印章圈、竖线、带底线）一并量进排除面。
3. **度量合理性断言进 host 验收惯例**：凡装载二进制字体 / 图形资产，加 canary 码点盒 / 步进合理域断言，防「返回 true 即绿」（261028-m6-k2a F1 的治本半）。
4. **行为面断言须端到端**：提示词 / 上下文到达类断言要验模型可见性（真实回合行为），载体落盘 / 中间层日志绿不等于语义达（261024-oneshot `f13a53a8` human-correction 与 261104-k2 踩坑 4 同族）。
5. 评估是否把第 1 条做成 `acceptance-coverage-check` 的伴随自检电池（像 `mutation-check --self-check` 那样），让「自检未跑」有机械形状。

**风险**：riskTier: **medium**——第 1–4 条是派单 / 纪律文面与验收设计惯例，低风险；第 5 条若落脚本则新增机检面，需要判单独核「自检 case 落在消费 / 拦截层而不是计算层」（参 `docs/debt/gov/ARQ-281028-mutation-fixture-consumer-layer.md`）。review profile = Track B（文面 + 可能的脚本搭车）。