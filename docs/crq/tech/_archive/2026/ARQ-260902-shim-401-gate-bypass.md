---
id: ARQ-260902-shim-401-gate-bypass
title: server/shim.py `require_device` 依赖 `return JSONResponse` 不短路，无 token 直通 OpenClaw（401 门穿透），并否定 TP-A 报告「无 token → 401」实测结论
domain: tech
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

鉴权缺陷已由 260903-device-ws-switch 修复并 ECS 真机复验（无 token→401 零转发 / 错 token→401 / 带 token 空文本→400）；TP-A 收口制品复核注已追加——`changes/dev/260831-cloud-link-bringup/review-packet.md` 尾部「真实性复核注」append-only 节、旧「无 token→401」结论作废以 WS 包复验为准（[260906-gov-review-arq-governance-batch](../changes/arch/260906-gov-review-arq-governance-batch/)，T9）。
