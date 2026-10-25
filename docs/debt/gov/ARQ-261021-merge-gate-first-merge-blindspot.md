---
id: ARQ-261021-merge-gate-first-merge-blindspot
title: 契约凭据闸两处首合并盲区——PROPOSAL 从 main ref 读（首次合并不可满足）+ verdict 锚 first-match 被 FAIL→PASS 轨迹遮蔽
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一次带契约 diff 的 arch 批合并（判据可机判：同型 BLOCK 输出复现即消费）
opened: 2026-10-21
closed: ""
closedBy: ""
paths: [".claude/scripts/pre-merge-escalation-gate.mjs", ".claude/scripts/lib/governance-gate-shared.mjs"]
source: 261021-arch-review-p3-record-cancel-lifecycle 合并期实测（gate 两次 BLOCK 输出 + 逐行读 gate 源码定位）
---

**事实（2026-10-21 实测，261021 批首次合并）**：`pre-merge-escalation-gate.mjs` 对 landing 完整（FROZEN PROPOSAL + PASS landing-verdict + manifest 覆盖齐备）的批仍 BLOCK，两处独立根因：

1. **frozen 判定盲区**：hasArchLoopContractEvidence 从 `git show main:<proposalPath>` 读 PROPOSAL（:143，target 硬绑 'main'，:449）——首次合并时 PROPOSAL 只在源分支、main 上不存在，`git show` 失败 → frozen 恒 false → 三件联立恒不成立。261017/261018/261019 先例契约批同样首合并，实走人批链（human-approval-ref）绕过——即「无人签门、合并过机械凭据闸」的机械凭据对**所有**首合并契约批结构性不可达，人批链是实际常态路径。
2. **verdict 锚 first-match 遮蔽**：verdict 判定取 VERDICT_STATUS_RE 命中的**第一行**（:149 `.find()`）——landing-verdict 按 append-only 纪律保留 FAIL→翻正轨迹时（首行 `> verdict: FAIL`、翻正后追加 `> verdict: PASS`），第一匹配恒 = FAIL → 永判非 PASS。verifier 的 append-only 诚实留痕与门的单源锚语义相撞。

**修法方向（归 gov-change 消费时裁）**：① frozen/manifest 读取改为「source ref 优先、target ref 兜底」（凭据随批走源分支才是首合并语义）；② verdict 锚改「最后一处 PASS/FAIL 取终态」或「读法注记行感知」（与 changes-index deriveLanded 的分层锚先例对齐——其 JSON 行锚「后出者胜」:223-226 已是同型解法）；③ 或明文承认「首合并契约批走人批链」为常态路径并写进 arch-change.md（去「机械凭据闸自动放行」的字面误导）。

**判据（机判）**：复跑 `node .claude/scripts/pre-merge-escalation-gate.mjs <changeId>`（契约批、landing 完整、首合并）——修复后应 PASS 而非 BLOCK；当前形态恒 BLOCK。

**Why**：261015 批裁「无人签门、合并过机械凭据闸」——若机械凭据对所有首合并契约批不可达，该裁定实际退化为「人批链常态」，治理叙事与机械现实不一致；要么修门兑现裁定，要么改叙事如实承认。
