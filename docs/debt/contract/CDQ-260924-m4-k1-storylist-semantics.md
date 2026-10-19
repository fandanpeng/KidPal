---
id: CDQ-260924-m4-k1-storylist-semantics
title: story_list total 语义收窄为现存篇数 + items ≤16 窗口
domain: contract
status: deferred
severity: SHOULD
owner: dev-loop
trigger: 实现面：front 推送 producer（三时点 + 窗口截断）归 K1；板端窗口消费/计数恒信 total 归 K2（C1/C2）
opened: 2026-09-24
paths: [docs/contracts/interface-contract.md]
hardExclusions: true
source: [delivery/taskpackages/260924-m4-k1-story-cloud-link/]
fourClass: ACCOMPANIES_SPEC_CHANGE
loopEvidence: human 契约门十问裁定 2026-09-24（CCP §5：Q1 认可；Q2 改判丙案 ≠ 拟答乙案）
---

# CDQ-260924-m4-k1-storylist-semantics · story_list total 语义收窄为现存篇数 + items ≤16 窗口

> 261019-gov-review-docs-root-ledger-rehome 批自 `docs/CONTRACT-DECISION-QUEUE.md` 单文件队列迁入（机械提取 + byte 对账）。四分类括注与 loop 凭据全文见下行原文。

**CDQ 行原文**：

```
| CDQ-260924-m4-k1-storylist-semantics | 2026-09-24 | `interface-contract.md` §1.3③ story_list：`total` 语义收窄 = 现存篇数（盖章 N 迁出至 §1.3④ `data.seq`）+ `items` 最新 ≤16 条窗口（Q2 裁丙案：板端 KSTORY_LIST_MAX=16 维持现值；旧条目设备端可达性以窗口为限、扩展 = 新需求另立须再裁 device RPC 边界）+ 板子行为句按 v1.27 交互改写（旧「KEY 选择」句废）+ 节标题术语对账（故事列表→星星记录本列表） | ACCOMPANIES_SPEC_CHANGE（spec↔contract 漂移收口——requirements v1.47/v1.26/v1.27 冻结在前） | human 契约门十问裁定 2026-09-24（CCP §5：Q1 认可；Q2 改判丙案 ≠ 拟答乙案） | **LANDED**（2026-09-24 契约 diff + story.py 三钉行同批，逐字 patch human 过目通道，git apply --check 先行绿） | [260924-m4-k1-story-cloud-link](../taskpackages/260924-m4-k1-story-cloud-link/) | 实现面：front 推送 producer（三时点 + 窗口截断）归 K1；板端窗口消费/计数恒信 total 归 K2（C1/C2） |
```

**剩余条件**：

实现面：front 推送 producer（三时点 + 窗口截断）归 K1；板端窗口消费/计数恒信 total 归 K2（C1/C2）
