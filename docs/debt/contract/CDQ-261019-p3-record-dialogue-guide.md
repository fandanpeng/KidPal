---
id: CDQ-261019-p3-record-dialogue-guide
title: "新增 start_organizing 控制动作 + 记录语境对话态措辞对账（interface-contract.md 三触点）"
domain: contract
status: deferred
severity: SHOULD
owner: dev-loop
trigger: "实现面承载包（五档分档，待 261016-p3-story-body-sd-cache 合并）；ADR-013 已随批落（architecture+契约面同批）"
opened: 2026-10-19
paths: [docs/contracts/interface-contract.md]
hardExclusions: true
source: [delivery/review/261019-arch-review-p3-record-dialogue-guide/]
fourClass: ACCOMPANIES_SPEC_CHANGE
loopEvidence: 261019-arch-review-p3-record-dialogue-guide PROPOSAL FROZEN v1.2（reviewer R1 BLOCK→v1.1 修稿→R2 GREEN）；human 立项 ack 2026-10-19（spine q1-q5）
---

# CDQ-261019-p3-record-dialogue-guide · 新增 start_organizing 控制动作 + 记录语境对话态措辞对账

> 261019 主线程 rebase 合并期按 261019-gov-review-docs-root-ledger-rehome 迁移格式自 `docs/CONTRACT-DECISION-QUEUE.md`（本行原落盘处，该文件同批删除）迁入 contract 域条目。

**CDQ 行原文**：

```
| CDQ-261019-p3-record-dialogue-guide | 2026-10-19 | interface-contract.md 三触点：① §1.4 新增 `start_organizing` 控制动作（记录会话收尾 → 整理过渡态；网关决定类、引擎 display 流拒转）+ 表后变更记录条；② §1.4 `start_recording` 行 + §四 `kidpal_session_ctx` 条措辞对账（「累积态 / manual 聆听模式 / 引擎不感知本信号」→ 记录语境对话态 + 引擎侧抑制由 front 内部注入等价信号承载；wire 形状零改）；③ §1.4 节标题括注对账（「OpenClaw 判意图后下发」→「OpenClaw 判意图或 front 检出后下发」——`start_organizing` = front 检出不经 OpenClaw） | ACCOMPANIES_SPEC_CHANGE（随 requirements v1.65 + ADR-013 同批原子落） | 261019-arch-review-p3-record-dialogue-guide PROPOSAL（reviewer GREEN + landing-verdict PASS） | **LANDED**（2026-10-19 landing-verifier PASS：结构门完整模式 ok:true exit 0 / 27 项逐字对账全 OK / canonical 超范围改动 0 / 机械断言独立复跑全绿——verdict 见 delivery/review/261019-arch-review-p3-record-dialogue-guide/landing-verdict.md） | 261019-p3-record-dialogue-guide | 实现面承载包（五档分档，待 261016-p3-story-body-sd-cache 合并）；ADR-013 随批落 |
```

**剩余条件**：

实现面承载包（五档分档，待 261016-p3-story-body-sd-cache 合并后回本门收尾分档 → /build-feature）。
