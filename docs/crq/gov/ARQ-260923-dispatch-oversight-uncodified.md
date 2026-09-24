---
id: ARQ-260923-dispatch-oversight-uncodified
title: 「派发书过目门」口径未成文——planner 每棒靠解释 human 一句口头授权判断是否免过目，两棒如实标注「这是我的解释、可能超出授权」；human 2026-09-23 明确裁定：不用每次都请示
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一治理批（与 ARQ-260923-dispatch-prompt-hard-terms 同族并批——同动 planner.md 派发步文面）
opened: 2026-09-23
closed: ""
closedBy: ""
paths: []
source: loop-retro-2026-09-23 报告 §五 提请 + human 2026-09-23 当场裁定「不用每次都请示」；pod-retro 实证：m2-k2 / m2-k3 人工纠偏体感节、m3-k1 卡点 5
---

**证据**（GC1 实核 canonical 无此条款：`.claude/agents/planner.md` 全文无「过目」字样，「派发书过目门」只作为 planner 侧 de-facto 纪律活在会话记忆里；仓内「过目」的成文点仅 `portfolio-planner.md:46` 门 1「human 过目切法」与 `task-intake-and-slicing.md:233`，属拆包切法面、非派发书面）+ 执行实证：① m2-k2（260917-m2-k2-retract-p4-three-page）human 开头一句「按 dev loop 正常走、没有 block 的问题不用找我拍板」，planner 执行成「派发书过目门在本次 loop 内免除」并落 pod-events 留痕（含原文与适用范围），retro 如实标注「若本意只是『少请示』，那我省掉的过目门是超出授权的；下一包我默认仍按过目纪律走，除非再得一句明确的话」；② m2-k3 human 指示几乎逐字相同，planner 沿用上一包先例并再次标注「它值得被 human 明确一次」；③ 分层出场的正向实证已跨棒成立——m3-k1 retro「过目纪律的价值在前半程（acceptance / builder 任务书 human 各看一眼），后半程的请示是纯摩擦——这个分层是对的」、k5 / k6 / k7 三棒 human 只在真判断点出场（launch 授权 / 真机持板 / 部署与合并批准）、loop 中途零打断。**human 裁定（2026-09-23 会话，loop-retro-2026-09-23 报告 §五 提请后当场给出）**：「不用每次都请示」——planner 的解释升级为明确口径，后续各棒不必再逐次解释一句口头授权。**影响面**：每棒 intake / 派发期——口径不成文则 planner 要么逐次请示（纯摩擦）、要么自行解释授权（越权风险），且解释无法跨棒继承（m2-k3 只能靠读 m2-k2 retro 沿用先例）。**建议改动**（候选，交 review，不自批）：把分层出场口径成文进 planner.md 派发步（或 pod-coordination 一节）——① 常规派发（acceptance-author / builder / fixer / integrator 任务书）不逐次请示，human 要看时才看；② **必升面逐条钉死、向严不减**：合并门 / 契约门 / 生产写入（ECS 部署与开闸）/ high riskTier 的 supervisor ESCALATE / 需求未定或与实况冲突 / F-08 人类显式覆盖留痕；③ 免除动作仍落 pod-events 留痕（含 human 原文与适用范围），使「谁授的权、免到哪」可审计。零新门、纯编排文面，不改任何闸判据。**riskTier: low**。owner=architect。
