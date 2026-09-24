---
id: ARQ-260907-engine-deploy-patch-drift
title: 引擎部署副本代码补丁（engine-patch）的漂移治理缺机械门：镜像更新即丢补丁或补丁语义漂移，当前只靠 compose bind-mount 在位 + 启动标记行人工核验
domain: tech
status: deferred
severity: SHOULD
owner: dev-loop（运维面）
trigger: 下次动 server/deploy 部署面 / 引擎镜像升级前
opened: 2026-09-07
closed: ""
closedBy: ""
paths: []
source: "260907-g-relay-fix（review-packet Findings #1 + docs-delta §C，2026-09-07）"
---

**证据**（260907-g-relay-fix 收口，2026-09-07）：G 包 live relay 缺陷修复引入本仓首例部署副本代码补丁（human 批准，pod-events decision `engine-patch-approval` 事件 `37469170`）——`server/deploy/engine-patch/` 的 `connection.py`（0.9.6 部署副本）+ `kidpal_display_strip.py` 经 `docker-compose.yml` bind-mount 覆盖官方镜像内 `core/`。**风险面**：① 官方镜像 `server_latest` 拉新重建后，镜像内 `core/connection.py` 上游演进，宿主部署副本可能与新版引擎内部 API 不匹配（标记行仍在但行为异常，属语义漂移）；② 卷映射被改动或部署副本文件缺失时容器静默回退原生 `connection.py`，fence 剥离与 relay 链整体失效，唯一机械报警锚 = 启动标记行 `kidpal display-strip patch active`（每次设备建连打一行）不再出现——无 cron 式自动核验、无部署期断言。**缓释现状**：核验方式与漂移告警语义已写入 `server/deploy/README.md`「引擎部署副本补丁」节（`docker logs … | grep` 标记行 + `docker inspect` 核对 Mounts），每次部署/重启后人工执行。**建议改动**（候选，交 review，不自批）：① 部署期断言脚本（`cert-warn-expiry.sh` 先例）挂 cron/systemd 定期核标记行 + Mounts 在位，缺失即告警；② 镜像升级 checklist 步骤（升级前核对上游 `core/` diff 与部署副本基线）；③ 长期评估把 fence 剥离上收为引擎上游 PR 或 front 单点（消部署副本形态）。**riskTier: medium**（静默回退 = live 缺陷整体回归）。owner=dev-loop（运维面）。
