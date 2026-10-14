---
id: ARQ-260913-auto-mode-classifier-legit-workflow
title: auto-mode 分类器第三次误伤 dev-loop 合法工作流，每包烧一轮拦截往返 + human 中场出场，已成本模式
domain: gov
status: deferred
severity: SHOULD
owner: 主线程（派发模板级）
trigger: 单开一批（**须双臂一起落**，不得只落口径臂）
opened: 2026-09-13
closed: ""
closedBy: ""
paths: []
source: loop-retro-2026-09-13（13 包复盘；pod-retro 实证：char-stroke 卡点 1、g-relay-fix 卡点 1、story-p3 人工纠偏节）+ 260918-gov-review-dispatch-chain-friction R1（2026-09-19 实测证伪机制）
---

**证据**（loop-retro-2026-09-13，3 包 retro 实证）：① char-stroke 变异探针（mutation-check 闸要求真实变异→齿红→还原）被分类器判成「造假绿」，方向完全颠倒，AskUserQuestion 拍板后继续；② story-p3 mutation 取证被拦「不可逆破坏 builder 产物」——分类器看不到「下一步就 checkout 还原」的意图，拦得其实合理但成本同付；③ g-relay-fix builder 的 ECS 部署被 Credential Leakage / Data Exfiltration 两形态终身盯防（transcript 出现过内联凭据痕迹后），处置 = 主线程代执行。**影响面**：凡「看起来像攻击」的合法验证动作（变异探针 / ECS 操作 / mutation 取证）都会复发；char-stroke retro 已总结成模式——「派发/执行前先落 human-decision 事件背书」。**建议改动**（候选，交 review，不自批）：① 派发模板固定预防声明「本动作的合法性与还原路径」+「被拦就停、主线程代执行」（g-relay-fix 第二轮已实证省一轮硬碰）；② 或 auto-mode 分类器对清单内验证动作加白名单（动 harness 面，须独立治理批，本登记不预批）。**riskTier: low**。owner=主线程（派发模板级）。**2026-09-19 追记——原改法经实测打错机制，须重新设计（不复用「只写派发臂」的原处方）**：本行随 `260918-gov-review-dispatch-chain-friction` 起草，architect R1 判 BLOCK(F1) 且标「升级 human」——原改法设计为「派发时在 dispatch prompt 里带两句声明」，**该机制假设误伤发生在「派发给子 agent」场景**；R1 核三例误伤的**运行者身份**后证伪：① `260829-char-stroke` 变异探针 = **planner 主会话直跑**（pod-events `human-decision` 事件 verbatim `"decision":"approve-mutation-probe-workflow"`、summary「human 批准 **planner 直跑**变异探针循环」）——**该拦截点无 dispatch prompt 存在**；② `260829-story-p3` mutation 取证 = 主线程直跑（pod-retro 人工纠偏节「主线程能不能直接改实现跑变异（你放行）」）——同上；③ `260907-g-relay-fix` ECS 部署 = 子 agent（派发），**唯此例机制够得到**且第二轮实证有效。**三例里两例够不到，且恰是最疼的两例**；另本行自引的 char-stroke retro 模式「**派发/执行前**先落 human-decision 事件背书」本就含「执行前」臂，原改法只承接了「派发」臂。**修法方向（交 review，不自批）**：条款由「派发臂」扩为**双臂**——补第三句覆盖**自执行姿势**（主会话直跑前先落一条**既有的**事件型 `human-decision` 背书，先例 = `approve-mutation-probe-workflow`，零机制扩张）。**另 R1-F2 发现「生产者可达性断链」**：原改法引 `pod-coordination.md:196` 作「同一层」先例属**引用失真**——该先例实为**双臂结构**（口径臂在 `pod-coordination.md`、携带臂在 `.claude/agents/planner.md` 派发步），原改法只落口径臂、`planner.md` 无指针指向新条款，采纳纯靠自发翻阅；**human 2026-09-19 裁决：补上携带臂**（下一批在 `planner.md` 派发步加一行指针）。**另两处计数更正**：「四类已实证误伤」实为 **3 例实证 + 1 类泛化**（部署动作类无独立实证）。**处置**：该条于 2026-09-19 由 human 裁定移出 `260918-gov-review-dispatch-chain-friction`、退回本队列单开一批；完整证据（三例运行者身份表）、双臂修法方向与携带臂裁决见 `delivery/changes/arch/260918-gov-review-dispatch-chain-friction/PROPOSAL.md` §6 留根① 与 §7 R1。
