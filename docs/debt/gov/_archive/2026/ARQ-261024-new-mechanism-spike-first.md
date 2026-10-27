---
id: ARQ-261024-new-mechanism-spike-first
title: 新机制（首次引入架构的新子系统/外部依赖集成/数据迁移）缺少「端到端最小验证 spike 先行」强制环节——K4 记忆回灌五层问题、三轮返工实证
domain: gov
status: closed
severity: MAJOR
owner: architect（方案与落点）+ 主线程（Track B 起草）
trigger: 下一个引入新子系统 / 新外部依赖集成 / 新数据迁移的批，triage 判档与 launch 前必查本条；或下次治理批
opened: 2026-10-24
closed: 2026-10-27
closedBy: 261027-gov-review-p5-spike-anchor-cluster
paths:
  - .claude/references/change-triage.md
  - .claude/references/dev-loop.md
  - docs/templates/PROPOSAL.template.md
source: "K4 记忆回灌三轮返工（2026-10-23/24）执行记录：delivery/review/261022-arch-review-device-memory-prompt-overhaul/k4-attribution-fix-review-20261024.md、k4-recall-architecture-20261024.md、delivery/taskpackages/261024-m5-k4b-corpus-parser-v2/{review-packet.md,verifier-evidence/}、[honcho-diag] 注入实证日志（openclaw-gateway journal）；human 2026-10-24 指示复盘入册"
---

**证据**（K4 记忆回灌三轮返工，全部有执行记录在案）：

引入「记忆画像 + 历史回灌」这一**新机制**时，开发与数据回灌在前、消费端（召回注入）验证完全缺位。五层问题**每层各吃一轮返工**，且一层比一层晚发现：

1. **解析器假设语料格式未先采样**：梦呓语料实为双层前缀 `[会话引用] User:/Assistant: 内容`,解析器按单层写 → corpus 批次 61% 消息（1026/1683,含小伴回复内嵌段）误归 owner → deriver 把小伴话术抽成孩子事实（实证结论：「owner advised 星星 to return the two yuan」实为小伴建议）。返工轮 1 = 解析器 v1 修复轮。
2. **时间戳回填缺位**（设计遗漏）：历史消息全部盖成处理日 10-24,真实日期 9/24–10/23 全失真 → deriver 相对日期解析（「今早」）全部错位。返工轮 2 = created_at 回填（重灌时一并修）。
3. **影子导出器未验证**：transcript 导出以 seq 当主键 → **1187 行主键冲突静默跳过**、跨会话混采、不可复现 → transcript 批次灌入即废（human D2 裁回滚不重灌）。返工轮 2 内消解。
4. **外部服务参数冲突无监控**：embedding 批量与 DashScope 上限冲突（24 条静默失败）+ deriver 输出上限 2500 截断（**27% 单元静默失败**,仅日志可见）→ 事后调参（LLM_DEFAULT_MAX_TOKENS=8000,失败率降至 ~8%）+ 监控候选在册。
5. **召回注入作用域从未验证（最贵的一层）**：backfill 按回滚隔离设计灌入**独立会话**（kidpal-bf-*）,而插件召回 = `session.context(当前会话)`——**跨会话结论结构性不可见**。三层 Gate（A 离线契约 / B 试灌读回 / C 硬删残留）全部验证的是**生产者侧**,消费端（注入内容）直到灌完、真机两轮答不出、human 坚持「先加日志再测」后,才由 `[honcho-diag]` 打点实证（注入仅 1 段画像卡、停在昨日内容、wangyinnuo=no）。数据（归属/日期/内容）全对,但**放错了召回视野外的房间**。

**影响面**：治理流程面——凡「首次引入新子系统 / 新外部依赖集成 / 新数据迁移」的批,若沿用现有 Gate 链（全部验证生产者侧）,必然复现本模式：开发→灌入→消费端才发现→返工。本次代价 = 三轮返工（解析器 v2、时间戳回填、两批回滚重灌）+ deriver 约 10 小时额外消化 + 两次画像毒化事故 + human 全天注意力。技术面残余（历史重灌正确性、毒结论机制修复）在 TECH-261022 在册,本条只承载**治理机制缺口**。

**建议改动**（候选,交 review 裁定落点与形态,不自批）：

- **原则（human 2026-10-24 原话）**：「其实应该是得先做一轮小验证,然后再做开发,再做数据的回灌」——即**新机制批强制「端到端最小验证 spike」先行**：在真实链路上,用最小数据量从生产端跑到消费端各一次（本例 = 灌一条消息 → 真机问一句 → 看注入）,验证通过才允许全量开发与数据迁移。
- **落点候选**：(a) `/triage-change` 判档清单——新增机制类批的必答问题（「消费端链路是否已被 spike 验证？」）;(b) `/build-feature` plan/launch 阶段——新增「spike 相位」（最小真实链路验证,产物 = 消费端实证记录）;(c) Gate 链补位——试灌 Gate 必须含「消费端注入断言」（本例 = diag 级打点）,不能只验读回。三处不互斥;(a) 最轻,(b) 最结构。
- **形态注意**：若落地为**机械 gate**（新增 check-launch/verify 项）→ 按约定本条升 full-adversarial;若为纪律文面 + 清单项 → Track B 可承载。
- **正向案例留存**：human 坚持「先加日志再测」后,`[honcho-diag]` 一条日志即时定位五层里最贵的一层——「消费端可观测打点」应作为 spike 的标配产物。

**风险**：riskTier: low-medium——改动为治理文面与流程清单（references/templates）,无代码、无数据迁移;主要风险 = spike 要求被形式化空转（跑个假验证就算数）,需在措辞里钉死「spine = 消费端真实观测,非生产者自证」。review profile = **Track B**（三要件①②③全满足:执行测出有实证 / 问题与原则清晰 / 爆炸半径已执行揭示;paths 落 G2 非硬排除）。**形态注记**：若 review 裁定须以机械 gate 落地（G3）,按约定升 full-adversarial + human——本条如实预标,不美化。

---

**勘误与引用 append（2026-10-24，主线程，按 k4-recall-architecture-20261024.md §C 对账）**：① 上文「证据」第 5 条表述需精确化——`session.context` 默认**不限会话**（workspace 级，跨会话本来通）；真正阻断注入的是两层：**(a) 观察者缺位**（backfill 会话成员配置 agent `observe_others=null` → deriver 不扇出 `(agent-kidpal, owner)` 视角结论，backfill 里该类 0 条）+ **(b) 被动窗口 100 条**（`WORKING_REPRESENTATION_MAX_OBSERVATIONS=100` 按 created_at 倒序，9 月事实被更晚结论挤出——**重灌不能修复本症状**，实测同构）。② 「建议改动」落点 (c) 可直接引用分析件 §C 的 **Gate E（E1 成员配置断言 / E2 派生+canary / E3 插件同款 context 请求断言 + 窗口位移模型 / E4 用户可见行为 / E5 通道健康）**。③ 本条 spike-first 原则与「先试灌单文件」的教训叠加：单文件试灌会**假通过**（新结论必在 recent-100 窗口内），spike 验收必须按**全量规模 + 首尾 canary + 用户可见行为**设计。

**2026-10-27 · 结项（`261027-gov-review-p5-spike-anchor-cluster`，T1）**：原则落三处文面 + 一处模板硬项——① `change-triage.md` §2 增「新机制批判定（spike-first 前置问）」：命中判定的变更必须在判档 rationale 写 spike 前置一句、切片序把 spike 包放到正式实现与数据迁移之前；② `dev-loop.md` 增「新机制批 spike 相位」节：spike 定义钉死三条（**消费端真实观测、非生产者自证** / **按全量规模设计、按最小数据执行** + 首尾 canary / **消费端可观测打点为标配产物**），数据迁移类给出四问并引用 Gate E（E1–E5）作断言形态先例，勘误①的「观察者缺位 + 被动窗口」与勘误③的「单文件试灌假通过」均写入正文；③ `planner.md` 增 Step 1i：正式实现 / 数据迁移包 intake 必须引用已闭合 spike 包的 `spike-evidence.md`，spike 未闭合或只到生产者侧一律停；④ `PROPOSAL.template.md` §2 增「新机制 spike 申报」硬项（三选一 + 缺席即判官 BLOCK）。**未落机械 gate**（本批路线 = 文面纪律，判定见本批 PROPOSAL §4 OQ1）；本条原条目三条 paths 之外另落 `planner.md`——无 intake 消费方的纪律等于没有执行面（理由见 PROPOSAL §2 T1）。留根：spike 规则暂无机械强制，观察期后再裁。
