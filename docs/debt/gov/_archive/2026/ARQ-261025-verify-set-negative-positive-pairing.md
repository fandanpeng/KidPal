---
id: ARQ-261025-verify-set-negative-positive-pairing
title: 验证集负向白名单断言缺正向配对——只写「不允许什么」，不写 happy path 必须恰好发生什么
domain: gov
status: closed
severity: SHOULD
owner: architect
trigger: 下一治理批（动 `docs/templates/PROPOSAL.template.md` 的 §5 验证集节或 `track-b-judge-brief.md` 的评审透镜时并批）
opened: 2026-10-25
closed: 2026-10-26
closedBy: 261026-gov-review-p2-anchor-template-cluster
paths: ["docs/templates/PROPOSAL.template.md", ".claude/references/track-b-judge-brief.md"]
source: ["delivery/review/261025-gov-review-release-pr-publish/retro.md", "delivery/review/261025-gov-review-release-pr-publish/PROPOSAL.md", "delivery/review/261025-gov-review-release-pr-publish/landing-verdict.md"]
---

**证据**：`261025-gov-review-release-pr-publish` 同一批两轮 BLOCK 栽在同一根上。① R1-F1（BLOCKING）：§5 写了三处「源码零字符串」断言（`grep -cF "gh pr merge"` = 0、`grep -cF -- "--force"` = 0、`grep -cE "git push|gh pr create"` = 0），与 §2 改法里「自检断言源码无该串」互斥——只能靠刻意拼串同时满足，而拼串后的断言证不了任何行为。② R2-F10（SHOULD）：gh 调用的子命令集合 ⊆ 白名单，脚本一次 PR 都不开也满足；缺的是「happy path 上恰有一次 `pr create` 且 argv 带正确的 `--base` / `--head`」这条正向断言。两轮判官记录在 PROPOSAL §7 R1/R2，修法（`publish-happy-path` 夹具 + argv 白名单）在 v0.4 落地。

**影响面**：所有写「负向白名单 / 零字符串 / ⊆ 允许集」型断言的 PROPOSAL。这类断言看起来最机械，实际是真空绿的反面形态：能被满足，但满足方式本身无意义。本批因此多耗一轮评审（R1 → v0.3 → R2），且 R2 的 F10 是对 F1 同根缺陷的二次发现，说明修完一个入口后另一入口仍然漏。

**建议改动**（交 review、不自批）：在 `docs/templates/PROPOSAL.template.md` 的 §5 验证集节补一条硬规则——「负向断言（不含 / 只允许 / ⊆）必须配一条正向 happy-path 断言（恰一次 / 相邻 flag 对 / 集合相等），否则判官可按真空绿打回」；`track-b-judge-brief.md` 的评审透镜第 6 条「验证集可执行性」加同一配对检查。是否升为【硬项】由下一批裁。

**风险**：low——纯文档/评审简报措辞，不动任何门判据与脚本行为。

**结项注记（2026-10-26，261026-gov-review-p2-anchor-template-cluster）**：261026 批 T5 落两处——`PROPOSAL.template.md` §5 三规则第 3 条 + `track-b-judge-brief.md` 评审透镜第 6 条「配对与实跑抽查」句。「是否升硬项」由本批裁 = **硬项**（取舍 D，理由 = 真空绿反照同族；退路已留：模板【硬项】语义若变同步复核）。
