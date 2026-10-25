---
id: ARQ-261021-proposal-assert-expectations
title: PROPOSAL §5 验证集断言预期值须来自起草期实跑输出、禁止推算（本批三处失配实证）
domain: gov
status: deferred
severity: SHOULD
owner: architect
trigger: 下一批走 /arch-change 或 /gov-change 的 PROPOSAL 起草时消费（判据可机判：§5 预期值是否带实跑留痕）
opened: 2026-10-21
closed: ""
closedBy: ""
paths: [".claude/references/arch-change.md", "docs/templates/PROPOSAL.template.md"]
source: 261021-arch-review-p3-record-cancel-lifecycle retro F1 + landing-summary 偏差申报三处 + landing-verdict 独立裁断（retro.md §3 F1 / §4 建议 1）
---

**事实**：261021 批 PROPOSAL §5 三处机械断言预期与实跑不符——条 3 把 `grep -c`（行计数）当次数口径（新行一行含 2 次命中）；条 4 预期「恒 2」未计入 T1 变更记录条自引 `start_organizing`；条 5 预期「恰 1」未计入 T4 §12 行自引「取消链显式化」。共同根因 = 预期值由起草人推算而非实跑回填；R1/R2 评审实跑记录的是基线值、未覆预期层；landing 期失配差点引发「正确 diff 夹带」误判方向（near-miss，逐项核因化解）。

**修法方向（归 gov-change 消费时裁）**：① arch-change.md Track B 制品义务加一句「§5 每条 post-commit 断言的预期值 = 起草期构造的等价载体实跑输出（如：把插入件临时拼进副本文件跑同命令），回填输出摘要，禁止纯推算」；② PROPOSAL.template.md §验证集骨架同步加占位注记；③ 评审四查纪律对齐——R 轮判官抽查预期值是否带实跑留痕。

**判据（机判/人判均可）**：新批 PROPOSAL §5 每条 pre/post 断言预期值旁是否有「起草期实跑」输出留痕；无留痕 = 判官可合法打回。

**Why**：验证集是 landing-verifier 与机械闸的对账基准，预期值失真会把正确落地判成夹带（或反之带病过门）；一次实跑成本 ~分钟级，失真代价 = 一轮 FAIL→核因→翻正环。
