---
id: ARQ-260928-triage-cross-face-intake-gap
title: /triage-change 只判实现侧切分大小——跨面 idea（需求+架构+契约+实现）无统一 intake，human 须在 3-4 个窗口以不同留痕格式重复同一意图，跨面触面靠事后逃生门「踢走」而非事前折叠
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一治理批（human 起草 PROPOSAL 经 /arch-change 走 Track A；或动 triage-change / change-triage / write-story / task-intake 任一面时并批）
opened: 2026-09-28
closed: ""
closedBy: ""
paths: [.claude/commands/triage-change.md, .claude/references/change-triage.md, .claude/skills/write-story/SKILL.md, .claude/references/task-intake-and-slicing.md]
source: [主线程会话 2026-09-28（human idea「查字页进入显示完整打字、按Key播报」逐步 walkthrough 实录 + human 诊断原话）, docs/requirements.md §9 变更记录 v1.55/v1.56/v1.57（单 idea 跨面分头处理先例）, .claude/references/change-triage.md §2/§6, .claude/commands/triage-change.md 第一刀]
---

**证据**：

1. **human 诊断原话（2026-09-28，本条触发源）**：「前两步就不顺……可能要修改架构和 contract，真正的 triage 应该要需求和架构/contract 修改加进来，不然人在一个窗口对抗，有要求不同阶段把同样的事情再说一遍，现在 triage 只关注到实现侧的拆包大小，这个不够。」
2. **walkthrough 实录（2026-09-28 主线程会话）**：human idea「查字页进入时就显示完整打字，按 Key 时再播报」→ 读 requirements 现文发现撞三处已钉 AC（S-F3.1 AC1 逐笔动态演示 / S-F3.1 AC4 播报随窗开 / S-F3.3 AC1 KEY 短按=重播——语义撞车只有读 story 才可见）→ 流程岔为「先需求面（human 裁 + write-story 修订）→ 再 /triage-change → 再 planner Pod」；该顺序在 canonical 文本中**无任何机制强制或检查**，系主线程对话中人肉补齐；且 walkthrough 本身漏面——契约只当 triage 第一刀分支一查而过、架构面完全未查（连走流程的主线程都会漏面，活体证据）。
3. **triage 判据面只有实现侧**（change-triage.md §2）：判定 = slicing matrix + riskTier，四档全为实现侧大小/形状；需求/架构/契约触面仅在「第一刀」当排除条件（碰 → 踢走 /arch-change），不进 intake、不当分面计划。
4. **四面异门或无门**：需求面**无门**（human 指示 + write-story + requirements §9 记账，先例均「跳过 Track B 如实记录」，连 triage 都不过；REQ- 域 CRQ 只登记「改需求的请求」，需求变更本身无比例化——小措辞修订与整条 story 重写同通道）；契约面 = architect-reviewer 四分类 + human 契约门，入口 = 各 loop 逃生门；架构面 = /arch-change（Track A/B）；实现面 = /triage-change。跨四面 idea = human 在 3-4 个窗口、以 3-4 种留痕格式重复同一意图。
5. **跨面检测 = 事后逃生门，非事前 intake**：/triage-change §6 逃生门 = review 中发现疑似触契约/spec → architect-reviewer 四分类 → **停 + 升级走**（routing away，非折叠）；260912 链「需修订 → launchPreconditions（spec 修订未冻结、受影响包不得 launch，包粒度硬依赖）」仅覆盖 ③/proposal 路径，**② 单包路径无等价物**。
6. **先例（requirements §9 变更记录）**：v1.55（human 裁恢复逐笔笔名——requirements 修订 + 契约扩展「届时走契约门」挂起 + 真机实证三面分头处理）；v1.56（human 真机轮口头需求——requirements 修订 + arch 对账块 + dev-loop 修复登记三面，无统一入口）；v1.57（V5 设计稿对齐——spec+design 面）。

**影响面**：每个跨面 idea 复现（需求修订几乎是常态——requirements §9 单 2026-09-14 一天即十余条 human 裁变更）。代价 = human 每面重复进场陈述同一意图 + 跨面顺序（spec 冻结先于实现 launch）在 ② 路径无机制保障、靠人肉记忆 + 逃生门兜底 + 漏面风险（walkthrough 实证主线程也会漏架构面）。频率 = 每个跨面 idea 一次。

**建议改动**（候选，交 review、不自批）：
1. **triage intake 升格为跨面申报**：/triage-change intake 增四面触面申报（requirements / architecture / contract / implementation）+ 一份跨面分面计划（各面要不要动、动哪、谁裁、顺序）；四面门照旧各归各裁（human 裁需求 / human 签契约 / architect 对抗审 spec）——**统一 intake ≠ 统一门**，不重写门体系。
2. **需求面比例化入口**：需求变更当前无任何门，统一 intake 时一并定需求面的比例化判据（措辞修订 vs 语义改写 vs story 重写分档）。
3. **需修订语义推广到 ② 路径**：260912 链 launchPreconditions（上游面未冻结不得实现，包粒度硬依赖）已有 ③ 路径先例——推广至 triage ② 单包路径，使「story/arch/contract 修订未冻结 → 实现包不得 launch」有机制载体而非人肉顺序。
4. **grilling 收敛到一次**：跨面 OQ 在 intake 处一次裁完（分面计划携带裁定进各面门），替代各窗口分别回问。

**风险**：riskTier: medium——治理路由面、每个跨面 idea 复现，不触契约与实现面、单批可修；但 ①③ 属新增前置门（gate 新增/修改 = lightweight 机检硬排除项）。Track B 三要件：① yes（human 诊断 + walkthrough 实录 + §9 先例）/ ② no（真设计分叉：intake 形态、需求面要不要门、② 推广方式均多方案）/ ③ no（爆炸半径未被执行实测揭示，系结构推断 + human 2026-09-28 会话诊断）→ **review profile = full-adversarial + human（Track A，/arch-change 入口）**，不美化快路径。owner: architect。关联：与 [ARQ-260927-role-boundary-story-propagation](ARQ-260927-role-boundary-story-propagation.md) 同链相邻不重叠——该条治拆包链下游（portfolio-planner/planner/acceptance-author 权限换份 + story 投影对账），本条治链最上游 intake 路由（一个 idea 进来先分面对）；两条同批 review 时须互查咬合（统一 intake 的跨面计划正是该条「story 贯彻」候选②的上游）。

**2026-09-28 注记**：建议 2（需求面比例化入口）已由 260928-gov-review-requirement-face 批 T3 承载——三档判据 + 两档流程落 `.claude/references/requirement-change.md`，定稿载体 = §9 版本行即 changeId；建议 1/3/4（intake 四面申报 / launchPreconditions 推广②路径 / grilling 收敛）与 /arch-change 退役安置仍归本条目后续批。
