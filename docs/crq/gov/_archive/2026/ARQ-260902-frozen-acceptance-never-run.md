---
id: ARQ-260902-frozen-acceptance-never-run
title: 冻结验收「冻结后从未执行」形态两度致缺陷（TP-A 先例 + smoke-e2e 包 live 线束 3 缺陷），建议冻结即冒烟执行
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-02
closed: 2026-09-06
closedBy: ""
paths: []
source: ""
---

冻结即冒烟纪律成文：`docs/DEV-GOVERNANCE.md`「## 验收测试纪律」§3（冻结落盘时当场执行一次线束冒烟：live 面真连 SSH、线束助手真调一次、pytest 收集零错误；判据 = 执行通道 error 为零，error ≠ failure——断言 failure 是冻结相位的设计态红基线，不阻塞冻结）+ `acceptance-author.md` 引用行（[260906-gov-review-arq-governance-batch](../changes/arch/260906-gov-review-arq-governance-batch/)，T8）。
