---
id: ARQ-260901-openclaw-chatcompletions-public
title: OpenClaw chatCompletions 端点为 full operator access 且随网关公网可达，仅 Bearer 保护
domain: tech
status: closed
severity: SHOULD
owner: ""
trigger: 生产上线前安全加固 / 任何非验证用途对外暴露时。2026-09-03 更新：方向 B 重切已把「18789 锁 127.0.0.1」钉为 G 包（260903-gateway-directionB）launch 硬前置（V28，PROPOSAL v0.4 §4 门簇①、GWA-E3 部署期断言），锁执行完成即移已结
opened: 2026-09-01
closed: 2026-10-12
closedBy: 主线程（2026-10-12 tech 域 CRQ 盘点，human 指示；证据见各条目收口节）
paths: []
source: "260831-cloud-link-bringup（dev-loop 收口 review-packet finding #1）"
---

TP-A 开启 OpenAI 兼容端点后实测：`POST /v1/chat/completions` = 完整 operator 权限，随 gateway bind=lan 公网可达，带 token 即可执行任意 agent 指令（无 token → 401，Bearer 已生效）。验证批接受此风险（shim 侧 OpenClaw token 仅经 loopback 转发、secrets.env 权限 600）；生产上线前须锁 127.0.0.1/防火墙拦 HTTP 路径，或 token 降权 + 设备弱权限 token 强制开启。

---

## 收口（2026-10-12 主线程盘点）

V28 锁已执行且 live 实证：ECS `ss -tlnp` 显示 18789 仅绑 `127.0.0.1`（+`[::1]`），OpenClaw brain loopback-only（`server/deploy/provider.config.yaml:16-18` 声明 + 260903-device-ws-switch 起在役，方向 B 端云四层链路 live）。本条目 trigger 自钉「锁执行完成即移已结」——条件达成，关闭。
