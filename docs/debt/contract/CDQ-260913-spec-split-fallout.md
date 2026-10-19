---
id: CDQ-260913-spec-split-fallout
title: "spec 拆分归档连带后果挂账（契约 :4 断链 + 验收 canary 翻红）"
domain: contract
status: deferred
severity: MAJOR
owner: dev-loop
trigger: "① 契约 :4 改指（下次契约门顺带）；②③ phase 2 验收基线重建：统一重钉新 canary（requirements.md / architecture.md）+ 旧断言改指或退役，一个动作处置"
opened: 2026-09-13
paths: [docs/contracts/interface-contract.md]
hardExclusions: true
source: [delivery/archive/changes/arch/260913-arch-review-spec-split/]
fourClass: ACCOMPANIES_SPEC_CHANGE
loopEvidence: 随该变更 FROZEN v1.1 批准（human 立项 ack + R4/R6 GREEN，T5「已知机械后果」节）
---

# CDQ-260913-spec-split-fallout · spec 拆分归档连带后果挂账（契约 :4 断链 + 验收 canary 翻红）

> 261019-gov-review-docs-root-ledger-rehome 批自 `docs/CONTRACT-DECISION-QUEUE.md` 单文件队列迁入（机械提取 + byte 对账）。四分类括注与 loop 凭据全文见下行原文。

**CDQ 行原文**：

```
| CDQ-260913-spec-split-fallout | 2026-09-13 | spec 拆分归档（`docs/spec.md` → `docs/archive/spec.md`）的连带后果挂账登记：① `interface-contract.md:4` 对 `../spec.md` 的引用成断链——本批契约零接触（F2 裁 (a)），改指随下次契约门一并处置；② 冻结验收件 spec 存在性 canary 真实翻红：`firmware/main/test/key_route_a1.acceptance.c:636`（KR-E2）、`firmware/main/test/screen_unification.acceptance.c:1010`（SU-E3）断言 `docs/spec.md` 存在；③ `server/acceptance/test_schedule_reminder.acceptance.py:1561` 直读该文件 → FileNotFoundError。②③ 为本变更治理授权下的已知机械后果，冻结件本批不改，处置归 phase 2 验收基线重建 | ACCOMPANIES_SPEC_CHANGE（随 260913-arch-review-spec-split 携带登记） | 随该变更 FROZEN v1.1 批准（human 立项 ack + R4/R6 GREEN，T5「已知机械后果」节） | **HUMAN_APPROVED**（挂账登记，phase 2 处置后 CLOSED） | [260913-arch-review-spec-split](../changes/arch/260913-arch-review-spec-split/) | ① 契约 :4 改指（下次契约门顺带）；②③ phase 2 验收基线重建：统一重钉新 canary（requirements.md / architecture.md）+ 旧断言改指或退役，一个动作处置 |
```

**剩余条件**：

① 契约 :4 改指（下次契约门顺带）；②③ phase 2 验收基线重建：统一重钉新 canary（requirements.md / architecture.md）+ 旧断言改指或退役，一个动作处置
