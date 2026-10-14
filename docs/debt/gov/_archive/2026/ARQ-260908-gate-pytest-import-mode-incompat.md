---
id: ARQ-260908-gate-pytest-import-mode-incompat
title: 两闸闸内执行器裸 pytest 无 --import-mode=importlib，与点分 .acceptance.py 命名制式互斥
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

260909-gov-review-gate-truthfulness T2 落地：两闸执行命令统一为 `python -m pytest -q --import-mode=importlib -m "not live"`（与包 redBaselineCommand 同源，与 T1 条目级筛选同批，向严不向噪）；OQ3=(c) 裁定 `server/pytest.ini` addopts 真单源化另登（自有实现面，dev-loop 或 human 一次性授权）。证据：[260909-gov-review-gate-truthfulness](../changes/arch/260909-gov-review-gate-truthfulness/)（V2 实测 127 collected / 41 deselected）。
