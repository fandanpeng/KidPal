---
id: ARQ-260920-changes-index-mergedinmain-stale
title: `changes-index` 的「已合main」列在合并后**不翻 `yes`**——全量重建只读状态缓存、不重算；且 `--upsert` 的 `if (merged)` 守卫因 `'no'` 是**真值**而失效，合并**前**跑过 upsert 会把 `'no'` 永久钉进缓存
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-20
closed: 2026-09-20
closedBy: ""
paths: []
source: ""
---

260920-gov-review-verdict-anchor-align T3 落地——`:371` 守卫由「只要 git 给了确定答复就写」收窄为「**只认 yes**」（`no` 在 JS 中为真值 ⇒ 旧守卫把「未合并」也钉进缓存，而全量重建缓存优先 ⇒ 合并后不翻 yes）；同批收窄 `backfill()` 的无条件写入（git 不可用〔空答复〕时**保旧值**，防已钉 `yes` 被空答复抹掉）；`--self-check` 零回退。landing-verdict PASS、commit `7a58398`。**残余**：「合并后不跑 upsert/backfill 则仍显示旧值」的时序问题仍在（7 条 pin 缓解存量、新批仍依赖流程记得刷新）——见该批 §6 留根②。证据：[260920-gov-review-verdict-anchor-align](../changes/arch/260920-gov-review-verdict-anchor-align/)。
