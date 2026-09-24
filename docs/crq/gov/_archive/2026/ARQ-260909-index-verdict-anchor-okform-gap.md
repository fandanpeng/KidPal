---
id: ARQ-260909-index-verdict-anchor-okform-gap
title: "changes-index `deriveLanded` 终态锚三通道（VERDICT_STATUS_RE 标签+冒号 / VERDICT_HEADING_RE 段后续行 token 起头 / JSON 行锚）均不识 `**ok: true（GREEN）——…**` 标题行形态——verdict 写此形态则 INDEX landed 推导静默翻不了"
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-09
closed: 2026-09-20
closedBy: ""
paths: []
source: ""
---

260920-gov-review-verdict-anchor-align 三条合一关它——T1（判官/核验官简报成文「verdict 行形态义务：行首 `verdict: <TOKEN>`」）+ T2（锚改 lib 单源 `VERDICT_STATUS_RE`，新批走规范形态即被认）+ T4（**7 个存量批按实况 pin 进 `LANDED_ADJUDICATION`**，逐条注明「依据 = 该批 landing-verdict 的 verdict 行 verbatim」）；落地后 INDEX 的 `frozen | yes` **归零**。**pin 由 6 条更正为 7 条**（起草期 census 时第 7 条 `260914-arch-review-kidpal-session-ctx-contract` 的「已合main」尚为空、后由 backfill 重算为 yes）。landing-verdict PASS、commit `7a58398`。证据：[260920-gov-review-verdict-anchor-align](../changes/arch/260920-gov-review-verdict-anchor-align/)。
