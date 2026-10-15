# ARQ-260902-cert-lifecycle-deadlock —— accept-as-risk 记账（GWA-E3 / GWA-A9）

> **_INTEGRATOR LANDING NOTE_**: staged by builder in `server/`; builder write-scope excludes
> `delivery/taskpackages/`. Integrator must land into `delivery/taskpackages/260903-gateway-directionB/`
> (`cert*.md` / `risk*.md` / `deadlock*.md`) for
> `test_e3_cert_deadlock_accept_risk_record` to go green. **ARQ 队列迁移由 integrator 收口**
> （builder 无 docs/** 写权）。

## 人类裁定（2026-09-04 pod-events）

`ARQ-260902-cert-lifecycle-deadlock` 处置定夺 = **accept-as-risk 记账**：证书 ≥10 年有效期 + 轮换机械脚本 + 到期前告警的验收面**不变**（GWA-A9 仍断言 ≥10y/轮换/告警）；「证书生命周期死局（到期需 OTA 提前换证）」本身作为已接受风险记账，**不 OTA 提前**。其 ARQ 队列迁移由 integrator 收口。

## 已履职实现（GWA-A9 验收面）

- **≥10 年有效期**：`server/deploy/cert-renew.sh` 以 `-days 7300`（20 年 ≥ 3650 天下限）签发；ECS 现场已生成 `/opt/kidpal/xiaozhi-esp32-server/cert/server.pem`（notBefore 2026-09-04 → notAfter 2046-08-30，实测 20 年）。
- **轮换机械脚本**：`server/deploy/cert-renew.sh`（`renew|force` 幂等轮换，auto 模式对未到期有效证书跳过）。
- **到期前告警**：`server/deploy/cert-warn-expiry.sh`（解析 notAfter，30 天窗口内/已到期 → WARN + 非零退出，可挂 cron/systemd）。

## 风险记账（accept-as-risk）

- 风险：自签证书无 CA 链、无自动 OTA 提前；到期若轮换脚本未被 cron 触发或 ECS 失联，wss 设备腿在 `notAfter` 后失效。
- 缓释：20 年有效期 + 机械轮换 + 到期前 30 天告警 + 证书/脚本路径 frozen（`GATEWAY_CERT_PATH`）。
- 决策：**接受风险**，不 OTA 提前（human 2026-09-04 裁定）。验收面不变。
- 依据：`ARQ-260902-cert-lifecycle-deadlock`；`delivery/changes/arch/260902-arch-review-kidpal-spec-v0.2/adjudications.md#V25`。