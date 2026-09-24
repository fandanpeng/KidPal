---
id: ARQ-260908-firmware-build-degraded-gate
title: firmware-build 闸只探 PATH idf.py，本仓实际通路（venv+wrapper）恒降级
domain: gov
status: closed
severity: ""
owner: ""
trigger: ""
opened: 2026-09-08
closed: 2026-09-09
closedBy: ""
paths: []
source: ""
---

260909-gov-review-gate-truthfulness T3+T4 落地：双路探测（PATH ∨ venv python + `c:/tmp/idf_run.py` wrapper，顶部常量与 kidpal-idf-env skill 同源）+ 降级带机器可判字段（buildAttempted/toolchainProbes/strictModeEnv）经 verify.mjs 上收 notExecuted；launch 面默认注入 VERIFY_FW_BUILD_REQUIRED=1（OQ1=(b)）。landing 期首真编 build-pass（46/46 全 degraded 历史终结）。supervisor/pre-merge 机械消费按 OQ4=(b) 留根下一批。证据：[260909-gov-review-gate-truthfulness](../changes/arch/260909-gov-review-gate-truthfulness/)。
