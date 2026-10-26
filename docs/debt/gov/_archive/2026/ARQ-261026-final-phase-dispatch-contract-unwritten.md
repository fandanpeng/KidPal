---
id: ARQ-261026-final-phase-dispatch-contract-unwritten
title: final 相位派发契约未成文——谁跑 final / scope-file 谁产 / round 序数怎么算只活在 verify.mjs fail-closed 行为与先例里，八包复现每包烧半轮到一轮
domain: gov
status: closed
severity: MAJOR
owner: architect（派单模板文面承接）
trigger: 下一治理批（动 `.claude/references/planner.md` 派单模板 / launch-prep 或 launch card 文面任一时并批）；或再出现一次 final 相位裸跑被 fail-closed 举手时立即升格
opened: 2026-10-26
closed: 2026-10-26
closedBy: 261026-gov-review-final-phase-dispatch-contract
paths: [".claude/agents/planner.md", ".claude/skills/kidpal-pod-launch-prep/SKILL.md"]
source: [delivery/retro/2026-10-25-loop-retro.md §二 T1（八包复现 + 根因定性）, docs/debt/gov/ARQ-261014-merge-gate-tooling-hotfix-batch.md（④ 教育面原文 + 2026-10-25 追注八包清单；本条自其拆出——human 2026-10-26 裁拆出独立批并登记防忘）]
---

**证据**（delivery/retro/2026-10-25-loop-retro.md §二 T1 逐字）：「final 相位的三个契约（谁跑、scope-file 谁产、round 序数怎么算）只活在 `verify.mjs` 的 fail-closed 行为和个别先例里，没有一处成文进 planner 派单模板或 launch card。」八包复现（ARQ-261014 2026-10-25 追注逐字在案）：261013-lang-config（scope-file 前置未进派单模板）、261014-engine-stop（final r4 缺 --scope-file；ordinal 语义靠试错）、261018-sd-log-usb-read（final scope-file 依赖低估）、261019-p3-record-dialogue-guide（连续第二轮）、261020-p3-rec-frame-visual（首次撞）、261022-m5-k1（final 轮缺 scope-file）、261022-p3-record-chrome-ghost-fix（final round 必须是 next completed ordinal）、261023-agents-md（沿用上一包 round 号误派）。同根先例两例一并并入本条（同属「派发契约只活在先例」）：required-check 参数形状踩两次（260929-char-window-v5，须无参走 current/ 指针）；261014-voice-round（补轮重跑前须显式删旧 write-once verdict，报错不自解释）。

**影响面**：每个 dev-loop 包的 final 相位。retro 原话「本波每包至少烧半轮到一轮」；零上下文 planner 无从得知三个契约，只能靠 fail-closed 举手后试错回补，成本随包数线性增长。retro 当时已建议升严重度并优先处置。

**建议改动**（候选，交 review、不自批）：① planner 派单模板（或 launch card 文面）成文三个契约——final 由谁跑、scope-file 谁产、round 序数怎么算（next completed ordinal 语义）；② `verify.mjs` fail-closed 报错行附「正确序列：scoped 绿 → integrator 圈定 → final」指引（fail-loud 带指引，对齐本仓「失败要可归因」取向）；③ 补轮重跑「先删旧 write-once verdict」一句写入同一文面；④ required-check 无参走 `current/` 指针的参数形状同面成文。

**风险**：riskTier: low——纯治理文面（references / skills），无行为面代码。review profile = Track B（G2 文面；新增义务条款非纯措辞，不走轻路径；三要件全 yes：① 执行测出 = retro T1 八包制品在案、② 修复明确 = 四句成文无设计分叉、③ 爆炸半径已揭示 = 每包半轮到一轮实测）。查重注记：retro GC1 当时按「同根在册」追加至 ARQ-261014④ 不新立；human 2026-10-26 裁拆出独立批（本批 OQ3），本条即拆出载体，ARQ-261014 尾部已加拆出注记防双头记账。

---

**结项注记（2026-10-26，closedBy = 261026-gov-review-final-phase-dispatch-contract）**：随 P1.5 批 Track B 全环结项——FROZEN v0.2（R2 GREEN）→ 七文件落地 → landing-verifier PASS。四件候选改动全落：① 三契约成文进 `.claude/agents/planner.md` 派发书（paths 已订正：references/planner.md 不存在，真身 agents/planner.md）② verify.mjs 缺 scope-file 报错附「正确序列 + 不占序」指引 ③ 补轮删旧 write-once verdict 中文 SOP 成文（同根先例 ⓐ）④ required-check 无参走 current/ 指针成文（同根先例 ⓑ）。起草期并坐实根因加强版：planner.md 与 integrator.md 两处 final 命令相冲突，本批改 planner 侧对齐 integrator 收口。
