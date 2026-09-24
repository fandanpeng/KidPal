---
id: ARQ-260906-live-acceptance-real-path
title: 跨进程 G 类包的 live 验收只「探测可达」、不「真跑一遍全链」，缺陷全留到下游包真机冒烟才连环爆
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-06
closed: 2026-09-06
closedBy: ""
paths: []
source: ""
---

probe ≠ exercise 纪律成文：`docs/DEV-GOVERNANCE.md`「## 验收测试纪律」§2（跨进程包 live 验收必须真 exercise 一次真实端到端路径，最小 tracer-bullet 即可，只探测可达不算 live 验收）+ `acceptance-author.md` References 节引用行（agent def 只加引用、结构零改动）（[260906-gov-review-arq-governance-batch](../changes/arch/260906-gov-review-arq-governance-batch/)，T8）。
