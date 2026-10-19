---
id: CDQ-260924-m4-k1-storytext-seq
title: story_text 增可选 data.seq（入册流水号）
domain: contract
status: deferred
severity: SHOULD
owner: dev-loop
trigger: seq 账本实现归 K1（M4K1-A6）；板端盖章 N 消费 + 易失 stamp_index 退役归 K2（M4K2-A5）
opened: 2026-09-24
paths: [docs/contracts/interface-contract.md]
hardExclusions: true
source: [delivery/taskpackages/260924-m4-k1-story-cloud-link/]
fourClass: CONTRACT_CHANGE_REQUIRED
loopEvidence: 同上十问裁定（Q3 认可 / Q4 seq 缺省降级演出板端自定）
---

# CDQ-260924-m4-k1-storytext-seq · story_text 增可选 data.seq（入册流水号）

> 261019-gov-review-docs-root-ledger-rehome 批自 `docs/CONTRACT-DECISION-QUEUE.md` 单文件队列迁入（机械提取 + byte 对账）。四分类括注与 loop 凭据全文见下行原文。

**CDQ 行原文**：

```
| CDQ-260924-m4-k1-storytext-seq | 2026-09-24 | `interface-contract.md` §1.3④ story_text：增可选 `data.seq`（入册流水号只增不减，additive——spec 只钉 N 语义不钉载体，先例 CDQ-260829/CDQ-260921）+ 朗读行为句清理（v1.52 砍朗读）+ 节标题「回放」术语对账；producer = front story.py 持久账本（删除不回退、重启存活） | CONTRACT_CHANGE_REQUIRED（独立契约变更） | 同上十问裁定（Q3 认可 / Q4 seq 缺省降级演出板端自定） | **LANDED**（同上批） | 同上 | seq 账本实现归 K1（M4K1-A6）；板端盖章 N 消费 + 易失 stamp_index 退役归 K2（M4K2-A5） |
```

**剩余条件**：

seq 账本实现归 K1（M4K1-A6）；板端盖章 N 消费 + 易失 stamp_index 退役归 K2（M4K2-A5）
