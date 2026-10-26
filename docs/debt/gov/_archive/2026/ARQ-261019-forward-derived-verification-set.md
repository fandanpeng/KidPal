---
id: ARQ-261019-forward-derived-verification-set
title: 验证集白名单/计数承诺应从处方文本正推（而非只枚举现状命中）
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: 下个含「命中 ≡ 白名单」或计数承诺型验证条目的起草批，architect 撰写约定增补一条
opened: 2026-10-19
closed: 2026-10-26
closedBy: 261026-gov-review-p2-anchor-template-cluster
paths: [delivery/review/261019-gov-review-docs-root-ledger-rehome/landing-verdict.md]
source: [delivery/review/261019-gov-review-docs-root-ledger-rehome/retro.md]
---

**证据**：261019-gov-review-docs-root-ledger-rehome 批 landing-verdict 两条 ADVISORY 同族：① §5.7「残留扫描 ≡ 白名单」按既有命中枚举 ①–⑯，但 PROPOSAL 处方新文在 9 行（⑰–㉕）必然自带旧路径字面（处方文本逐字含旧路径），落地与处方一致即必然「白名单外命中」；② §5.3 承诺「advisory ≤ 基线 195」，而 T1 映射表把 CDQ-260913 钉 MAJOR（off-rubric），实测 196——验证集承诺与数据来源表两处独立撰写、未交叉对账。两案 landing-verifier 均判 PROPOSAL 侧瑕疵（非落地遗漏、非私货），不阻断但消耗一轮裁定量。

**影响面**：凡含「命中 ≡ 白名单」或「计数 ≤ N」型验证条目的批次都可能复发——验证集从现状枚举而不从处方正推，绿条件与处方文本互相打架，每发一次核验官就要做一次「PROPOSAL 瑕疵 vs 落地遗漏」裁定。

**建议改动**：architect 撰写约定增补一条——起草验证集时，凡白名单型条目从处方文本正推会产生的命中（含处方新文自带字面），凡计数承诺与数据来源表（映射表/配置行）交叉对账后落数字。交 review、不自批。

**风险**：riskTier: low——纯撰写约定增补，零行为面。

**结项注记（2026-10-26，261026-gov-review-p2-anchor-template-cluster）**：261026 批 T4 落 `PROPOSAL.template.md` §5「验证集撰写三规则」第 2 条（白名单/计数从处方正推 + 与数据来源表交叉对账）。规则立起后首个现场印证 = 同批落地期 V7 白名单漏计自检夹具行（起草期预期 1、实跑 2，按本规则覆写并留痕）——执行层遗漏非规则缺口，已记 retro F1。
