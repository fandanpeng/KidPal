---
id: CDQ-260924-m4-k1-story-delete
title: 新增 kidpal_story_delete 私有消息（端→网关硬删）
domain: contract
status: deferred
severity: SHOULD
owner: dev-loop
trigger: Q7 relay 收紧 + Q8 floor 机械执行面归 K1 builder（C1）；板端覆盖层与发送/重发归 K2（C3）；live 复证挂 K3
opened: 2026-09-24
paths: [docs/contracts/interface-contract.md]
hardExclusions: true
source: [delivery/taskpackages/260924-m4-k1-story-cloud-link/]
fourClass: CONTRACT_CHANGE_REQUIRED
loopEvidence: 同上十问裁定（Q5 甲案+隐式 ack / Q6 硬删+窄边界 / Q7 relay 收紧 / Q8 story floor / Q9 §1.6 story 段）
---

# CDQ-260924-m4-k1-story-delete · 新增 kidpal_story_delete 私有消息（端→网关硬删）

> 261019-gov-review-docs-root-ledger-rehome 批自 `docs/CONTRACT-DECISION-QUEUE.md` 单文件队列迁入（机械提取 + byte 对账）。四分类括注与 loop 凭据全文见下行原文。

**CDQ 行原文**：

```
| CDQ-260924-m4-k1-story-delete | 2026-09-24 | `interface-contract.md` §四 新增 `kidpal_story_delete`（端→网关单向私有消息：front 机械硬删 + 审计 + story_list 重推对账；隐式 ack 超时重发不新增应答消息对——ADR-005 边界保持；边界 = 仅 story MD，家长通道本期不建挂账）+ §1.5 第 3 条 story floor/relay 收紧句 + 新增第 6 条故事面机械职责 + §1.6 模板 story 段 | CONTRACT_CHANGE_REQUIRED（arch §5.4【链路载体待契约】+ §7 在办②；判据一致先例 CDQ-260914） | 同上十问裁定（Q5 甲案+隐式 ack / Q6 硬删+窄边界 / Q7 relay 收紧 / Q8 story floor / Q9 §1.6 story 段） | **LANDED**（同上批） | 同上 | Q7 relay 收紧 + Q8 floor 机械执行面归 K1 builder（C1）；板端覆盖层与发送/重发归 K2（C3）；live 复证挂 K3 |
```

**剩余条件**：

Q7 relay 收紧 + Q8 floor 机械执行面归 K1 builder（C1）；板端覆盖层与发送/重发归 K2（C3）；live 复证挂 K3
