---
id: ARQ-260927-role-boundary-story-propagation
title: 拆包链角色权限划分 + story 语义贯彻无机械保障——portfolio-planner 越界写包级实现设计（plan.md 改法/阶段/边角）、需求蒸馏五跳投影无对账、planner Step 1e 与拆包侧 acceptance 双写点无对账
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: （已结项）原触发条款由 260928-gov-review-portfolio-boundary-story-propagation 承接完毕
opened: 2026-09-27
closed: 2026-09-28
closedBy: 260928-gov-review-portfolio-boundary-story-propagation（Track B 全环：R1 BLOCK→闭环→R2 GREEN → landing T1–T9 manifest 18 文件 → landing-verdict PASS → 合并 main；建议 1 权限划分 = T1+OQ-1 裁定 (b)+三承载位、建议 2 = T2①+T4②（③ 语义对账 human 裁不做，记留根）、建议 3 = T3 写点下沉 + 写者换人（核对钉 Step 8b））
paths: [.claude/agents/portfolio-planner.md, .claude/agents/planner.md, .claude/agents/acceptance-author.md, .claude/references/task-intake-and-slicing.md]
source: [delivery/taskpackages/260924-m4-k3-record-session-live/plan.md（commit 498e3c2 chore(portfolio) 拆包落盘）, delivery/taskpackages/260924-m4-k3-record-session-live/taskpackage.json, .claude/agents/planner.md:47（Step 1e）, .claude/agents/portfolio-planner.md:37（Step 2）, .claude/scripts/brief-inputs.mjs]
---

**证据**（静态结构性观测，file:line 均可点开；非 loop-retro/friction/verifier 事故记录，诚实标注）：

1. **portfolio-planner 越界写包级实现设计**——`delivery/taskpackages/260924-m4-k3-record-session-live/plan.md`（commit 498e3c2 `chore(portfolio)` 拆包落盘）实含三块 Pod 级内容：§改法取舍 8 条实现级决策（「发送原语复用 K2 缝」「source_page 映射读 kidpal_layer_top() 闭集三值」「时序 = 首帧音频前、server hello 之后、SendStartListening 之前」「stt 帧 OnIncomingJson 分发喂 story 回显区」）；§阶段划分 5 步 = planner.md 已定义的 Pod 标准 loop 重述（acceptance→builder→verifier↔fixer→integrator→live）；§边角 6 条 builder 易漏（「唤醒词路径也过会话发起入口」「引导态结束≠丢弃」）。这些是「怎么写」，不是「切到哪」——RTE 越界做包级实现设计。
2. **门 1 未审改法/阶段**——`task-intake-and-slicing.md`「门 1」checklist 五项（切片逻辑 / 依赖序与 WIP / coverage 完备 / 意图 vs 裁定一致 / demo 形态可行）无一审「改法取舍 / 阶段划分」正确性——RTE 塞进 plan.md 的实现细节没换来任何一道审，只是碰巧和四件套打包。
3. **story 语义五跳投影、下游无 ground truth**——story 只锚点引用不内嵌原文（`task-intake-and-slicing.md`「story 锚点引用与转写」节），语义经 PROPOSAL 业务目的清单 → roadmap 效果段 → acceptance 骨架 → planner Step 1e 蒸馏 → 冻结验收 五跳，每跳 lossy；builder/verifier/fixer 只消费冻结 acceptance，story 原文不可见。锚 2 只对 AC 号集（「多引、少引都是方向偏差信号」`planner.md:63`），不对 AC 语义——「AC 引用着但断言比 AC 弱/钉反」全漏，即「写着写着脱了需求」。
4. **acceptance 字段双写点无对账**——`.claude/agents/portfolio-planner.md:37/40`（Step 2「N 包 acceptance 并集覆盖」/ Step 4「写 taskpackage.json 含 intake 字段」）先写 acceptance 骨架；`.claude/agents/planner.md:47`（Step 1e「通读 story 全节、蒸馏进 acceptance/decisionCoverage」）再写，而 `.claude/agents/planner.md:41`（Step 1①「以其 acceptance/decisionCoverage 为权威、不回退重归一」）又说别重写——同一字段两个写点、字面有张力、无机械牙让两个投影互相对账。
5. **input-brief 不承载 story 定位**——`.claude/scripts/brief-inputs.mjs` ① 节只处理带 `#§锚点` 的契约/spec 引用；story 裸 ID（`S-F1.4`）进 brief 落成 missing（文件不存在）+ advisory，docRefs 裸 `docs/requirements.md` 落成 bare（无锚点、整文件）——story 阅读面无机器派生清单兜底，定位与蒸馏纯靠散文纪律承载。

**影响面**：每个经 portfolio-planner 拆包的包都复现（改法/阶段/边角前置到拆包，与倡议内容无关）。dev loop 需求漂移：五跳投影每跳 lossy、下游无 ground truth、锚 2 只拦结构不拦语义——漂移静默累积且无纠错点。不修代价 = RTE 职责臃肿 + 链路多一层无对账投影 + 「脱需求」无任何机械闸可拦。频率 = 每批一次。

**建议改动**（候选，交 review、不自批）：
1. **权限划分**：portfolio-planner 收窄到「scope 划分 + 技术可行性判据 + 跨包冻结边界 + demo 证据形态」；plan.md 的改法取舍 / 阶段划分 / 边角下沉到 Pod 内 planner / acceptance-author / builder 自己准备（plan.md 切成拆包侧「可行性判据」与 Pod 侧「实现设计」两层）。
2. **story 贯彻**（三候选，任选或组合）：① 给 builder/verifier 输入面带「story 原文 / 关键 AC 判据」的可读锚（堵下游全盲，可能触 brief-inputs.mjs）；② 机械化 planner Step 1e（acceptance 强制带 AC 回引 + 原文判据，缺则闸红）；③ verifier / supervisor 加一道「story↔acceptance 语义对账」机械牙（堵语义漂移，现有锚 2 只堵编号漂移）。
3. **消解双写点**：Step 1e 与拆包侧 acceptance 骨架的写点合并或明确定位（谁写骨架、谁核保真、核的机械牙在哪）。

**风险**：riskTier: medium——治理过程面、每批复现，不触契约/实现面、单批可修。改动面 = G2（agent 定义 + reference）为主，若走候选①触 brief-inputs.mjs 则升 G2+G3 mixed。Track B 三要件：① yes（现象 file:line 可点开）／② no（真设计分叉：角色权限怎么换、story 怎么贯彻均有多方案）／③ no（爆炸半径未被执行实测揭示，系结构推断 + human 2026-09-27 会话诊断）→ **review profile = full-adversarial + human（Track A，/arch-change 入口）**，不美化快路径。owner: architect。

**结项记录（2026-09-28 append）**：经 260928-gov-review-portfolio-boundary-story-propagation Track B 全环结项——human 立项会话改裁 Track B + 主线程亲起草；评审 R1 BLOCK(F1–F4)→v0.3 闭环（含本条证据 4 的 planner.md:39 转引失真勘误：Step 1① 现文不含 acceptance 权威句，双写点结论由 :37/:40 ↔ :47 独立支撑）→ R2 GREEN → landing T1–T9（manifest 18 文件）→ landing-verdict FAIL→PASS（summary 格件两行修）→ Fast-forward 合并 main。建议 1→T1+T9（human 立项会话另增 input-brief 生成时点归还 Pod 内，翻转 260921 批 T5 裁定）、建议 2→T2①+T4②（候选③ 语义对账 human 裁不做）、建议 3→T3（写点下沉 + 写者换人，核对钉 Step 8b）。笔误「权限换份→权限划分」已改（title + 正文两处）。
