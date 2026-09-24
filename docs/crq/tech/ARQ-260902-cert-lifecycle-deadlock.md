---
id: ARQ-260902-cert-lifecycle-deadlock
title: 自签长期证书到期死局（S5）：OTA 未建前证书到期全部设备 wss 瘫痪+逐台重烧（监控面，human 2026-09-04 accept-as-risk 记账；决策闭环记录另见 §2 已结索引）
domain: tech
status: accepted-risk
severity: SHOULD
owner: ""
trigger: OTA 应答端点建立（spec §6.3 留二期）对账 / 或证书剩余有效期 <5 年
opened: 2026-09-02
closed: ""
closedBy: ""
paths: []
source: 260903-gateway-directionB（dev-loop 收口 + cert-lifecycle-accept-risk 证据，2026-09-05）
---

**human 2026-09-04 拍板 accept-as-risk**（pod-events decision `cert-deadlock-accept-as-risk`；记账 `delivery/changes/dev/260903-gateway-directionB/cert-lifecycle-accept-risk.md`）：维持自签证书 ≥10y + 轮换机械脚本 + 到期前告警（GWA-A9 验收面不变，本包已履职 20y/轮换/告警），**不 OTA 提前**。保留原因 = 自签证书无 CA 链、无自动 OTA 提前；到期若轮换脚本未被 cron 触发或 ECS 失联，wss 设备腿在 `notAfter` 后失效。缓释已就位（20y 有效期 + `cert-renew.sh` 幂等轮换 + `cert-warn-expiry.sh` 到期前 30 天告警 + 证书/脚本路径 frozen `GATEWAY_CERT_PATH`）。

**双重落位裁决（迁移）**：源双重落位：§2 记「human 2026-09-03 裁 accept-as-risk」的决策闭环，§1.3 记持续监控面（证书剩余有效期 <5 年 或 OTA 端点建立时复议）。新模型单状态 ⇒ 取 accepted-risk（监控面为活面），决策闭环作为正文段落保留。

**另一处落位的原文（closed）**：human 2026-09-03 裁 **accept-as-risk**：有效期 ≥10 年（V25 已裁，把到期推出产品生命周期）+ 轮换机械脚本 + 到期前告警，承载 = G 包 `260903-gateway-directionB` GWA-A9/GWA-E3（PROPOSAL v0.4 §4，launch 前置门簇②随本裁销账）；残余风险（OTA 未建前到期）接受为记账风险。证据：[260902-arch-review-kidpal-spec-v0.2](../changes/arch/260902-arch-review-kidpal-spec-v0.2/)（adjudications V25）+ spec 决策 22 + `.planning/kidpal-mvp/PROPOSAL.md` v0.4 §4/§7。
