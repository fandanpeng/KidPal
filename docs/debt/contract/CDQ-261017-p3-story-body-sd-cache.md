---
id: CDQ-261017-p3-story-body-sd-cache
title: 新增 kidpal_story_fetch（契约面首个端发起请求-应答环）
domain: contract
status: deferred
severity: SHOULD
owner: dev-loop
trigger: ADR-011 已随批落（architecture+契约面同批）；实现面承载包（五档分档）
opened: 2026-10-17
paths: [docs/contracts/interface-contract.md]
hardExclusions: true
source: [delivery/review/261017-arch-review-p3-story-body-sd-cache/]
fourClass: ACCOMPANIES_SPEC_CHANGE
loopEvidence: architect-reviewer **R2 GREEN**（判档 = 主路径；四分类 = ACCOMPANIES_SPEC_CHANGE）；human 立项 ack 2026-10-17
---

# CDQ-261017-p3-story-body-sd-cache · 新增 kidpal_story_fetch（契约面首个端发起请求-应答环）

> 261019-gov-review-docs-root-ledger-rehome 批自 `docs/CONTRACT-DECISION-QUEUE.md` 单文件队列迁入（机械提取 + byte 对账）。四分类括注与 loop 凭据全文见下行原文。

**CDQ 行原文**：

```
| CDQ-261017-p3-story-body-sd-cache | 2026-10-17 | `interface-contract.md` 两触点：① §四新增 `kidpal_story_fetch`（端→网关，`{"session_id","type","ids":[...]}` ids 批量，**契约面首个设备发起请求-应答环**，应答复用 `story_text`、下行零新形状）；② §1.3④ `story_text` 消费语义注记扩展（形状零改动、fetch 应答不触发盖章、卡不在位时不留存当场显示） | ACCOMPANIES_SPEC_CHANGE（随需求 v1.64 + 决策 24 carve-out 携带，同批原子落） | architect-reviewer **R2 GREEN**（判档 = 主路径；四分类 = ACCOMPANIES_SPEC_CHANGE）；human 立项 ack 2026-10-17 | **LANDED**（2026-10-17 landing-verifier PASS：结构门完整模式 exit 0 / T1–T6 逐字核对全中 / canonical 超范围改动 0） | [261017-arch-review-p3-story-body-sd-cache](../delivery/review/261017-arch-review-p3-story-body-sd-cache/) | ADR-011 已随批落（architecture+契约面同批）；实现面承载包（五档分档） |
```

**剩余条件**：

ADR-011 已随批落（architecture+契约面同批）；实现面承载包（五档分档）
