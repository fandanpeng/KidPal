---
id: CDQ-260921-m3-k1-schedule-grid
title: schedule view 周网格重定义 + home data.upcoming（producer 归 front）
domain: contract
status: deferred
severity: SHOULD
owner: dev-loop
trigger: A2–A4 实现齿归 K1 dev-loop（prompt v6 影子契约收口归 K3）
opened: 2026-09-21
paths: [docs/contracts/interface-contract.md]
hardExclusions: true
source: [delivery/taskpackages/260921-m3-k1-cloud-schedule-producer/]
fourClass: ACCOMPANIES_SPEC_CHANGE
loopEvidence: human 契约门裁五问 2026-09-21（packet §5：Q1 扁平形状+day 0 基+备注不进载荷；Q2 甲案薄载荷+连带改写一并批；Q3 home data additive；Q4 upcoming front 机械算；Q5 校验收紧）
---

# CDQ-260921-m3-k1-schedule-grid · schedule view 周网格重定义 + home data.upcoming（producer 归 front）

> 261019-gov-review-docs-root-ledger-rehome 批自 `docs/CONTRACT-DECISION-QUEUE.md` 单文件队列迁入（机械提取 + byte 对账）。四分类括注与 loop 凭据全文见下行原文。

**CDQ 行原文**：

```
| CDQ-260921-m3-k1-schedule-grid | 2026-09-21 | `interface-contract.md` 两触点：① §1.3② schedule view data 形状重定义（旧 `{title,items}` 列表形即废 → 周网格 `{week:{courses:[{day,slot,name,flags}]}}` + 意图薄载荷 `data:{}` front 截获展开；SPEC_CONTRACT_DRIFT）；② §1.3⑥ home view 增可选 `data.upcoming` 承载「接下来」（additive，producer = front 机械算 group_upcoming 单源；CONTRACT_CHANGE_REQUIRED）。连带改写 §1.5 第 3 条（薄载荷截获展开 + 旧形状拒绝 + 零自研红线注记）/ §1.6 system prompt 模板课表段 / §2.3 DeepSeek 课表职责行 | ACCOMPANIES_SPEC_CHANGE（① 随 spec 改动携带 + ② 缺口补齐，四分类裁定表见 packet/gate evidence §3） | human 契约门裁五问 2026-09-21（packet §5：Q1 扁平形状+day 0 基+备注不进载荷；Q2 甲案薄载荷+连带改写一并批；Q3 home data additive；Q4 upcoming front 机械算；Q5 校验收紧） | **LANDED**（2026-09-21 契约 diff + schedule.py 钉行 + queue/ADR/ledger 随包收口同批；validate_display 三样本证据入包 evidence/） | [260921-m3-k1-cloud-schedule-producer](../taskpackages/260921-m3-k1-cloud-schedule-producer/) | A2–A4 实现齿归 K1 dev-loop（prompt v6 影子契约收口归 K3） |
```

**剩余条件**：

A2–A4 实现齿归 K1 dev-loop（prompt v6 影子契约收口归 K3）
