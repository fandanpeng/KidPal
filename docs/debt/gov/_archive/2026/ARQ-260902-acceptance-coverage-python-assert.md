---
id: ARQ-260902-acceptance-coverage-python-assert
title: acceptance-coverage-check.mjs `hasRealAssertion` 原只认 JS 风格断言，Python unittest 冻结验收被误判「无可识别断言」（热修追认 + 永久化）
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

`hasRealAssertion` 自检成文入库：runSelfCheck 5 用例（2 正 3 负：python-unittest-positive / python-bare-assert-positive / prose-comment-negative / prose-string-negative / empty-negative），已知边界（纯正则不剥注释/字符串）如实声明（[260906-gov-review-arq-governance-batch](../changes/arch/260906-gov-review-arq-governance-batch/)，T2）。
