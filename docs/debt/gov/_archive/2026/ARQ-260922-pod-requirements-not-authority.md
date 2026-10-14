---
id: ARQ-260922-pod-requirements-not-authority
title: Pod 不先读 requirements story 原文与 AC 就把已定项当开放问题问 human——需求已写明的行为仍给替代推荐让 human 拍板，需求权威被「human 临场偏好」静默替代
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: 下一治理批（与 ARQ-260922-buildfeature-launch-prep-skip 同批并批）→ **2026-09-24 已结**：260924-gov-review-planner-orchestration-align T2 全承载（R2 GREEN）
opened: 2026-09-22
closed: 2026-09-24
closedBy: 260924-gov-review-planner-orchestration-align T2（delivery/changes/arch/260924-gov-review-planner-orchestration-align/）
paths: []
source: 260920-m2-charwindow-card-fix pod-events（human-decision 事件两条，2026-09-20）+ requirements.md S-F3.2 对读 + human 报「pod 不看需求」触发核验（2026-09-22）
---

**证据**（260920-m2-charwindow-card-fix pod-events 两条 human-decision 与 requirements 原文对读，2026-09-22）：①「右栏内容判据（覆盖固定行结构口径）」——human 被拉进去裁「就按语音播报的内容，语音播报什么我们就显示什么」；②「板端定位 = 纯展示面；内容归云端」——human 原话推翻 planner 对前一条的解读。而 `docs/requirements.md` S-F3.2（:497-506）本已写明右栏内容权威口径：AC1「拼音带声调 + 共 N 画 + 部首标签 + 释义 + 组词」+ 补充规则「右栏保持速查卡密度、释义 = 核心释义一行内」——至少部分被请示项需求已答，Pod 未以 story 原文为行为权威先蒸馏、直接把实况差异当开放问题升级。机制面根因：`.claude/agents/planner.md` intake 链对 `sourceRefs` 命中 requirements story 只要求锚点引用形态（taskpackage 示例 `:177`「story 语义权威 = docs/requirements.md 现文」），无一步硬性要求「派发或提问前通读该 story 全节（AC + 补充规则 + 线框行）并当行为权威」；Step 5b 三触发形状（return / propose-drop / notify）覆盖「钉不成断言」，不覆盖「需求已写却推荐替代」这一面。**影响面**：每包 intake/派发期——human attention 被需求已答之事消耗（本例连拍两次、planner 解读还被推翻造成返工轮）；更危险的反向面 = Pod 拿 human 临场偏好替代 requirements 当权威，需求面与实现面即刻静默漂移（本例 human 裁定实际放宽了 S-F3.2 口径，requirements 未随裁回写则已过时）。**建议改动**（候选，交 review，不自批）：planner.md intake 增一条纪律步——`sourceRefs[]` 命中 `docs/requirements.md` story 时，finalize TaskPackage 与任何 AskUserQuestion 之前必须通读该 story 全节并把它当行为权威蒸馏进 acceptance/decisionCoverage；向上升级仅限三类：需求确实未定、需求内部矛盾、需求与契约/实况冲突（冲突走既有 return 形状；requirements 回写走 write-story 面 / human-correction 留痕），禁止「需求已写明但给替代推荐让人拍板」形态。不动 Step 5b 既有三形状语义，只收窄「把已定项当未定项」的误用面。同族边界：human 主动给出偏离需求的指令属 human authority（本例即此形态），按 human-correction 留痕并触发 requirements 回写，不算违规请示。**riskTier: low**（纯编排纪律步，不涉门判定与契约）。**review profile**：Track B 三要件逐条 = ① 执行测出 yes（pod-events 两条 human-decision 与 requirements 原文对读）；② 问题清晰且修复明确 yes（加「先读原文、已定不问」纪律步，无方案选择分叉；「需求 ↔ human 临场偏好冲突」由既有 human authority 机制承载、非本条新开分叉）；③ 半径已被执行揭示 yes（两连问 + 解读被推翻 + 需求面即刻漂移均执行照出）。G 层事实：G2 = `.claude/agents/planner.md` 正文节（不动 frontmatter）。机检硬排除逐项声明：不触 `docs/contracts/**`、无 spec-contract drift、不改 tools/name/model/write scope/authority 语义（只加 intake 正文步骤；是否构成「agent def 新节触硬排除」请判官核、本人不自我裁定）、不新增 gate、不触 self-def、不触 review-loop agent 定义、非 mixed lane、不涉 L0 蓝图。便宜验证计划：`route-regression-check.mjs`（planner.md 同文件必复跑仍绿）+ `lint-descriptions.mjs`（description 不动、跑一遍证未误伤）。owner=architect。

**结项（2026-09-24，260924-gov-review-planner-orchestration-align T2 承载）**：原处方「planner.md intake 增需求权威纪律步」已按 FROZEN PROPOSAL（v0.3，R1 BLOCK → R2 GREEN）落 canonical——新增 Step 1e：`sourceRefs[]` 命中 requirements story 时，finalize TaskPackage 与任何 AskUserQuestion 之前必须通读该 story 全节（AC + 补充规则 + 线框行）并当行为权威蒸馏进 acceptance/decisionCoverage；禁止「需求已写明但给替代推荐让 human 拍板」形态；向上升级仅限三类（需求未定 / 内部矛盾 / 与契约实况冲突）；human authority 同族边界（F-08 面 human-correction 留痕 + 触发 requirements 回写）明文。Step 5b 三形状语义不动。证据：该批 PROPOSAL §2 T2 + §5 V4-T2 + §7 R2 GREEN。
