---
id: ARQ-260906-index-rebuild-dev-status-drift
title: changes-index 全量重建把 post-cutoff 收口包由 closed 翻回 open + arch landed 推导不命中新格式 verdict（双写者谓词漂移病灶）
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-06
closed: 2026-09-06
closedBy: ""
paths: []
source: ""
---

dev 状态推导换收口谓词单源 `closeoutRequiredFiles`（lib 单源，与 closeout-check 承重闸同真值源；R5 判据作废——verification.json/verdict.json 按 closeout-hardening SSOT 为 advisory 永不晋升包目录，对全包恒 false；sv 值/在场均不可作判据五证）；arch landed 推导补双锚（VERDICT_HEADING_RE 编号容差 `(?:\d{1,3}[.、]\s*)?` + JSON 行锚 last-match-wins）。self-check +6 夹具 29/29；重建纠偏 dev 8 行 closed + 3 arch 批翻 landed（[260906-gov-review-index-status-truth-source](../changes/arch/260906-gov-review-index-status-truth-source/)，T1–T4）。
