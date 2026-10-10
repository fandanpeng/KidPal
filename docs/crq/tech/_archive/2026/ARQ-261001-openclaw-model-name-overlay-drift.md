---
id: ARQ-261001-openclaw-model-name-overlay-drift
title: 引擎 overlay 的 model_name 被重部署冲刷成 openclaw/default，OpenClaw 双 agent 下返 400
domain: tech
status: closed
severity: SHOULD
owner: ""
trigger: TaskPackage 261001-openclaw-model-name-overlay-drift 落地（修仓内模板 + 重钉 PERF-K1 冻结齿）后转 closed
opened: 2026-10-01
closed: "2026-10-10"
closedBy: 主线程（261010 部署演练闭环，见文末结项记录）
paths:
  - server/deploy/apply-xiaozhi-overlay.sh
  - server/deploy/provider.config.yaml
  - server/gateway/provider.py
  - server/deploy/perf-k1/probe_batch.py
  - server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py
source: 261001 真机部署后 OpenClaw 400（ECS 活配置为唯一真值来源，仓内模板滞后）
---

2026-09-30/10-01 M4 真机部署后，设备侧对话一律失败，引擎日志原文：`Error code: 400 - Multiple agents are configured, but this operation has no explicit owner...`。

根因：OpenClaw 上跑 `main` + `kidpal` 双 agent 且 `ownership: explicit`，引擎活配置 `.config.yaml` 的 `model_name` 却是 `openclaw/default`（无法唯一定位答主）。该值早在 2026-09-25 被手工修为 `openclaw/kidpal`，但**只落在 ECS 活配置、未回流仓内**；本次 ECS 重部署用仓内 overlay（`server/deploy/apply-xiaozhi-overlay.sh` 渲染 `provider.config.yaml`）重写了活配置，把它冲回旧值。

漂移面（仓内滞后于 live 现实）：

- `server/deploy/apply-xiaozhi-overlay.sh:72`（渲染面，live 真源）
- `server/deploy/provider.config.yaml:62`（声明面）
- `server/gateway/provider.py:32`（`OPENCLAW_MODEL` 默认值；**无 live 消费方**——front 只用其 `corr_id` / 常量，live 调用由引擎读自身配置发起）
- `server/deploy/perf-k1/probe_batch.py:42`（探针默认值）
- `server/acceptance/test_perf_k1_roundtrip_latency.acceptance.py:916/:938`（冻结验收钉旧值，须重钉；`:917/:939` 断言文案「契约 §四钉 openclaw/default」为**失准引用**——契约对 model_name 零命中）
- `docs/ADR.md` ADR-006 的支撑句「model_name `openclaw/default` 零改、模型选择对引擎完全不可见」随之失效（ADR 为 append-only → 新增条目 amend，不改正文）

活配置已于 261001 热修（ECS `.config.yaml` → `openclaw/kidpal` + `docker restart`，探针 200；备份 `.config.yaml.bak-260930-modelname-clobber`）。本条目管的是**仓内回流**：不修则下次重部署必然复现。修法见 `delivery/taskpackages/261001-openclaw-model-name-overlay-drift/plan.md`（② 单包 Lite / riskTier=low，architect 分流快审 2026-10-01）。

**结项记录（2026-10-10，主线程部署演练）**：仓内回流已由 TaskPackage 261001 落地（声明/渲染/探针/provider 四面重钉 `openclaw/kidpal` + MNF 齿在场，`test_261001_openclaw_model_name_overlay_drift.acceptance.py` 绿）。261010 部署演练（TECH-261010 决策 21 键同批）发现**最后一处残余**：ECS 部署副本 `/opt/kidpal/xiaozhi-esp32-server/deploy/apply-xiaozhi-overlay.sh` 仍载修前旧值 `model_name: openclaw/default`（仓内已修但副本从未随部署刷新，md5 对账实证）——若直接重跑会把 live 再冲回旧值 400。已同步修复版（含本条重钉 + 决策 21 两键），重跑渲染核验：渲染产物 `model_name: openclaw/kidpal` 在场（plan §MNF-A3 部署演练至此完成）。备份 `apply-xiaozhi-overlay.sh.bak-261010-decision21`。
