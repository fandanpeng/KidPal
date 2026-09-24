---
id: ARQ-260901-openclaw-chatcompletions-public
title: OpenClaw chatCompletions 端点为 full operator access 且随网关公网可达，仅 Bearer 保护
domain: tech
status: accepted-risk
severity: SHOULD
owner: ""
trigger: 生产上线前安全加固 / 任何非验证用途对外暴露时。2026-09-03 更新：方向 B 重切已把「18789 锁 127.0.0.1」钉为 G 包（260903-gateway-directionB）launch 硬前置（V28，PROPOSAL v0.4 §4 门簇①、GWA-E3 部署期断言），锁执行完成即移已结
opened: 2026-09-01
closed: ""
closedBy: ""
paths: []
source: "260831-cloud-link-bringup（dev-loop 收口 review-packet finding #1）"
---

TP-A 开启 OpenAI 兼容端点后实测：`POST /v1/chat/completions` = 完整 operator 权限，随 gateway bind=lan 公网可达，带 token 即可执行任意 agent 指令（无 token → 401，Bearer 已生效）。验证批接受此风险（shim 侧 OpenClaw token 仅经 loopback 转发、secrets.env 权限 600）；生产上线前须锁 127.0.0.1/防火墙拦 HTTP 路径，或 token 降权 + 设备弱权限 token 强制开启。
